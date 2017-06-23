#include <Circuit.h>
#include <QSet>
#include <SimulationUtils.h>
#include <GridUtils.h>
#include <Terminal.h>
#include <vector>
#include <fstream>

#include <Resistor.h>
#include <Wire.h>
#include <Comp_VoltageSource.h>
#include <DispatchSolver.h>
// add isTerminalCOnnected() to check for infinite loops within path search

namespace Solver{

  Circuit::Circuit(QList<QGraphicsItem*> av_QList_AllElements)
  { 
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());
    mv_QList_AllElements = av_QList_AllElements;
    mv_nTotalElements = mv_QList_AllElements.size();
    QSet<Terminal*> lv_UnhadledTerminals;
    mv_nNodes = 0;
    /* Most probably useless */
    
    QSet<QGraphicsItem*> lv_ConnectedObjects;
    for (QGraphicsItem * it : av_QList_AllElements)
    {
      // check all terminals
      if (it->type() == SimulationUtils::ElectronicElements::Element_Terminal)
      {
        Terminal *lv_pTerminal = qgraphicsitem_cast<Terminal*> (it);
        lv_UnhadledTerminals.insert(lv_pTerminal);
        
        // Terminal belongs to
        QGraphicsItem *lv_pTerminalOf = lv_pTerminal->parentItem();
        QList<QGraphicsItem*> lv_ConnectedToTerminal = lv_pTerminal->collidingItems();
        for (QGraphicsItem * terminalConnectsTo : lv_ConnectedToTerminal)
        {
          // if a terminal connects to another teminal, the two objects are connected.
          if (terminalConnectsTo->type() == SimulationUtils::ElectronicElements::Element_Terminal)
          {
            QGraphicsItem * lv_pOtherItem = terminalConnectsTo->parentItem();
            _ASSERT(lv_pOtherItem != nullptr);
            lv_ConnectedObjects.insert(lv_pOtherItem);
            lv_ConnectedObjects.insert(lv_pTerminalOf);
          }
        }
      }
    }
    
    mv_nConnectedElements = lv_ConnectedObjects.size();
    
    // add a bit of stability
    if (lv_UnhadledTerminals.size() == 0)
    {
      CGridUtils::sc_xTheGrid->mf_SetGridText("Nothing to Solve!");
      return;
    }

    for (QGraphicsItem * it : av_QList_AllElements)
    {
      if (it->type() == SimulationUtils::ElectronicElements::Element_Terminal)
      {
        if (!mf_HandleTerminal(it))
        {        
          std::cout << "Unconnected Terminal, no simulation is done!" << std::endl;
          std::string text = buffer.str();
          CGridUtils::sc_xTheGrid->mf_AddToGridText(text.data());
          return;
        }
      }
    }

    if (mv_nNodes > 0)
    {
      mf_ConstructEdgesMatrix();
    }
    else
    {
      // if all elements are connected, we have a simple single loop circuit
      Terminal *firstTerminal = *lv_UnhadledTerminals.begin();
      Terminal *StartTerminal = nullptr;
      EDGE lv_Edge; // The edge
      Terminal *T_Ref;
      bool foudPath = false;

      while (!foudPath)
      {
        if (nullptr == StartTerminal)
          StartTerminal = firstTerminal;
        // Hey Terminal, Al cui fiu esti?
        QGraphicsItem *T_Parent = firstTerminal->mf_pTerminalOf();

        // Insert parent into path          
        lv_Edge.Components.push_back(T_Parent);

        // currently support only two terminal components
        // Parent, who is your other terminal?

        IComponent *ParentComponent = dynamic_cast<IComponent *>(T_Parent);
        if (nullptr == ParentComponent)
        {
          // cast failed -> we have a wire
          CWire * Parent_Wire = dynamic_cast<CWire*>(T_Parent);
          T_Ref = Parent_Wire->mf_OtherTerminal(firstTerminal);
        }
        else
        {
          T_Ref = ParentComponent->mf_OtherTerminal(firstTerminal);
        }

        QList<QGraphicsItem*> ConnectsTo = T_Ref->collidingItems();
        for (QGraphicsItem* it : ConnectsTo)
        {
          Terminal * otherTerminal = dynamic_cast<Terminal*>(it);
          if (nullptr != otherTerminal)
          {
            firstTerminal = otherTerminal;
            if (firstTerminal == StartTerminal)
            {
              foudPath = true;
              mv_AllEdgesInCircuit.push_back(lv_Edge);
            }
            break;
          }
        }
      }

      if (foudPath)
      {
        mv_nEdges = 1;
      }   
    }

    mv_nLoops = mv_nEdges - mv_nNodes;

    std::string text = buffer.str();
    CGridUtils::sc_xTheGrid->mf_SetGridText(text.data());
  }

  Circuit::~Circuit()
  {
  }

  const int Circuit::mf_nConnectedElements()const
  {
    return mv_nConnectedElements;
  }

  const int Circuit::mf_nTotalElements()const
  {
    return mv_nTotalElements;
  }

  const EDGE_VEC & Circuit::mf_AllEdgesInCircuit() const
  {
    return mv_AllEdgesInCircuit;
  }

  const NODE_VEC & Circuit::mf_AllNodesInCircuit() const
  {
    return mv_Nodes_List;
  }

  const bool Circuit::mf_bTerminalIsNode(Terminal* ac_Terminal) const
  {
    if (mv_Nodes_List.size() == 0)
      return false;

    for (CNode node : mv_Nodes_List)
    {
      if (node.mf_bContainsTerminal(ac_Terminal))
        return true;
    }
    return false;
  }

  // Always use mf_bTerminalIsNode before calling this method!
  CNode Circuit::mf_GetNode(Terminal *ac_Terminal)
  {
    for (CNode node : mv_Nodes_List)
    {
      if (node.mf_bContainsTerminal(ac_Terminal))
        return node;
    }
    _ASSERT(false);
    return CNode(-1);
  }

  bool Circuit::mf_HandleTerminal(QGraphicsItem *ac_Trminal)
  {
    Terminal *lv_pCurrentTerminal = qgraphicsitem_cast<Terminal*> (ac_Trminal);
    if (!mf_bTerminalIsNode(lv_pCurrentTerminal))
    {
      QList<QGraphicsItem*> lv_ConnectedToTerminal = lv_pCurrentTerminal->collidingItems();
      int TerminalCount = 0;
      for (QGraphicsItem * terminalConnectsTo : lv_ConnectedToTerminal)
      {
        if (terminalConnectsTo->type() == SimulationUtils::ElectronicElements::Element_Terminal)
        {
          TerminalCount++;
        }
      }
      if (TerminalCount == 0)
      {
        // the terminal is unconnected, fuck it stop execution.
        return false;
      }
      if (TerminalCount >= 2)
      {
        // this is a node
        ++mv_nNodes;
        // if we don't have a node already containing the terminal,
        // Create a new node and add all new terminals.
        CNode lv_Node(mv_nNodes);
        lv_Node.mf_AddTerminal(lv_pCurrentTerminal);
        for (QGraphicsItem * terminalConnectsTo : lv_ConnectedToTerminal)
        {
          if (terminalConnectsTo->type() == SimulationUtils::ElectronicElements::Element_Terminal)
          {
            Terminal *lv_pTerminalToAdd = qgraphicsitem_cast<Terminal*> (terminalConnectsTo);
            lv_Node.mf_AddTerminal(lv_pTerminalToAdd);
          }
        }
        mv_Nodes_List.push_back(lv_Node);
      }
    }
    return true;
  }

  bool Circuit::mf_bAreEdgesTheSame(EDGE ac_Edge, EDGE ac_other)
  {
    for (QGraphicsItem * component : ac_Edge.Components)
    {
      for (QGraphicsItem * other_component : ac_other.Components)
      {
        if (component == other_component)
        {
          return true;
        }
      }
    }
    return false;
  }

  void Circuit::mf_ConstructEdgesMatrix()
  {    
    for (CNode node : mv_Nodes_List)
    {
      int lv_Terminals = node.mf_nGetTerminalCount();
      for (int i = 0; i < lv_Terminals; ++i)
      {
        // Node, give me a terminal
        Terminal * T = node.mf_GetTerminal(i);
        bool lv_bFoundPath = false;
        EDGE lv_Edge;
        lv_Edge.FromNode = node;
        while (!lv_bFoundPath)
        {
          // Hey Terminal, Al cui fiu esti?
          QGraphicsItem *T_Parent = T->mf_pTerminalOf();

          // Insert parent into path          
          lv_Edge.Components.push_back(T_Parent);

          // currently support only two terminal components
          // Parent, who is your other terminal?
          Terminal *T_Ref;
          IComponent *ParentComponent = dynamic_cast<IComponent *>(T_Parent);
          if (nullptr == ParentComponent)
          {
            // cast failed -> we have a wire
            CWire * Parent_Wire = dynamic_cast<CWire*>(T_Parent);
            T_Ref = Parent_Wire->mf_OtherTerminal(T);
          }
          else
          {
            T_Ref = ParentComponent->mf_OtherTerminal(T);
          }

          // is  the other terminal a node?
          if (mf_bTerminalIsNode(T_Ref))
          {
            // Path complete, we have a new edge
            lv_Edge.ToNode = mf_GetNode(T_Ref);
            mv_AllEdgesInCircuit.push_back(lv_Edge);
            lv_bFoundPath = true;
          }
          else
          {
            QList<QGraphicsItem*> ConnectsTo = T_Ref->collidingItems();
            for (QGraphicsItem* it : ConnectsTo)
            {
              Terminal * otherTerminal = dynamic_cast<Terminal*>(it);
              if (nullptr != otherTerminal)
              {
                T = otherTerminal;
                break;
              }
            }
          }
        }
      }
    }

    // All nodes are processed, simplify the matrix
    mf_SimplifyEdgesMatrix();
    mv_nEdges = mv_AllEdgesInCircuit.size();
  }

  void Circuit::mf_SimplifyEdgesMatrix()
  {
    if (mv_AllEdgesInCircuit.size() > 1)
    {
      for (EDGE_VEC::iterator it = mv_AllEdgesInCircuit.begin(); it != mv_AllEdgesInCircuit.end(); ++it)
      {
        EDGE_VEC::iterator next = it;
        ++next;
        for (; (next != mv_AllEdgesInCircuit.end()); ++next)
        {
          if (next->Components.size() == it->Components.size())
          {
            if (mf_bAreEdgesTheSame(*it, *next))
            {
              mv_AllEdgesInCircuit.erase(next);
              break;
            }
          }
        }
      }
    }      
  }
}
