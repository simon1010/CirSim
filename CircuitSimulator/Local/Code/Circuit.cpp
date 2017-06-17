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
    /**/
    /**/
    /**/QSet<QGraphicsItem*> lv_ConnectedObjects;
    /**/for (QGraphicsItem * it : av_QList_AllElements)
    /**/{
    /**/  // check all terminals
    /**/  if (it->type() == SimulationUtils::ElectronicElements::Element_Terminal)
    /**/  {
    /**/    Terminal *lv_pTerminal = qgraphicsitem_cast<Terminal*> (it);
    /**/    lv_UnhadledTerminals.insert(lv_pTerminal);
    /**/    
    /**/    // Terminal belongs to
    /**/    QGraphicsItem *lv_pTerminalOf = lv_pTerminal->parentItem();
    /**/    QList<QGraphicsItem*> lv_ConnectedToTerminal = lv_pTerminal->collidingItems();
    /**/    for (QGraphicsItem * terminalConnectsTo : lv_ConnectedToTerminal)
    /**/    {
    /**/      // if a terminal connects to another teminal, the two objects are connected.
    /**/      if (terminalConnectsTo->type() == SimulationUtils::ElectronicElements::Element_Terminal)
    /**/      {
    /**/        QGraphicsItem * lv_pOtherItem = terminalConnectsTo->parentItem();
    /**/        _ASSERT(lv_pOtherItem != nullptr);
    /**/        lv_ConnectedObjects.insert(lv_pOtherItem);
    /**/        lv_ConnectedObjects.insert(lv_pTerminalOf);
    /**/      }
    /**/    }
    /**/  }
    /**/}
    /**/
    /**/std::ofstream LogFile;
    /**/LogFile.open("c:\\CIRSIM_LOGGING\\Circuit.log");
    /**/
    /**/mv_nConnectedElements = lv_ConnectedObjects.size();
    /**/
    /**/
    /* End most probably useless */
    
    // add a bit of stability
    if (lv_UnhadledTerminals.size() == 0)
    {
      CGridUtils::sc_xTheGrid->mf_SetGridText("Nothing to Solve!");
      return;
    }

    for (QGraphicsItem * it : av_QList_AllElements)
    {
      switch (it->type())
      {
      case SimulationUtils::ElectronicElements::Element_DC_Source:
        LogFile << "DC Source" << std::endl;
        break;
      case SimulationUtils::ElectronicElements::Element_Ground:  
        LogFile << "Ground" << std::endl;
        break;
      case SimulationUtils::ElectronicElements::Element_Resistor:
        LogFile << "Resistor" << std::endl;
        break;
      case SimulationUtils::ElectronicElements::Element_Terminal:
      {
        LogFile << "Terminal" << std::endl;
        if (!mf_HandleTerminal(it))
        {        
          std::cout << "Unconnected Terminal, no simulation is done!" << std::endl;
          std::string text = buffer.str();
          CGridUtils::sc_xTheGrid->mf_AddToGridText(text.data());
          return;
        }
        break;
      }
      case SimulationUtils::ElectronicElements::Element_Wire:  
        LogFile << "Wire" << std::endl;
        break;
      case SimulationUtils::ElectronicElements::Element_ProgrammableVoltageSource:
        LogFile << "Programmable VS" << std::endl;
        break;
      default:
        break;
      }
    }
    LogFile << "Nodes in circuit: " << mv_nNodes << std::endl;
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

      double lv_dfResistance = 0.0;
      double lv_dfVoltage = 0.0;

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
          switch (ParentComponent->type())
          {
          case SimulationUtils::ElectronicElements::Element_ProgrammableVoltageSource:
          case SimulationUtils::ElectronicElements::Element_DC_Source:
          {
            CVoltageSource *VS = qgraphicsitem_cast<CVoltageSource*>(ParentComponent);
            if (T_Ref->mf_nGetPolarity() == 1)
            {
              lv_dfVoltage -= VS->mf_dfGetVoltage();
            }
            else
            {
              lv_dfVoltage += VS->mf_dfGetVoltage();
            }

            break;
          }
          case SimulationUtils::ElectronicElements::Element_Ground:

            break;
          case SimulationUtils::ElectronicElements::Element_Resistor:
          {
            CResistor *Resistor = qgraphicsitem_cast<CResistor*>(ParentComponent);
            lv_dfResistance += Resistor->mv_nResistance;
            break;
          }
          case SimulationUtils::ElectronicElements::Element_Terminal:
            break;
          case SimulationUtils::ElectronicElements::Element_Wire:
            break;
          default:
            break;
          }
        }

        QList<QGraphicsItem*> ConnectsTo = T_Ref->collidingItems();
        for (QGraphicsItem* it : ConnectsTo)
        {
          Terminal * otherTerminal = dynamic_cast<Terminal*>(it);
          if (nullptr != otherTerminal)
          {
            firstTerminal = otherTerminal;
            if (firstTerminal == StartTerminal)
              foudPath = true;
            break;
          }
        }
      }

      if (foudPath)
      {
        mv_nEdges = 1;
        LogFile <<"Resistance : "<< lv_dfResistance << std::endl;
        LogFile << "Voltage total : " << lv_dfVoltage << std::endl;
        LogFile << "Current : " << lv_dfVoltage / lv_dfResistance << std::endl;
      }
     
    }
    LogFile << "Edges in circuit: " << mv_nEdges << std::endl;

    // Instantiate the brand new solver
    CDispatchSolver lv_DispatchSolver(mv_AllEdgesInCircuit, mv_Nodes_List);

    /*
       The algorithm of the method is the following:
    1. The matrix Ue and G are given.
    2. The matrix A is filled.
      If a graph contains n + 1 nodes and e edges, the complete incidence
       matrix A has the dimensions (n + 1) x e having the following values for the elements:
       1  = Aij if the edge j is incident to the node i and leaves the node;
       -1 = Aij a if the edge j is incident to the node i and enters into the node;
       0  = Aij if the edge j is not incident to the node i;
    3. The matrix G’ is computed (rel. 6.11).
    4. The matrix I’sc is computed (rel. 6.11)
    5. By solving the equations system the matrix V’ is obtained.
    6. The edges voltages U (rel. 6.6) and the electrical currents I are obtained*/

    /*Construct the matrixes*/
    /*
    Where G is a square matrix, having e x e as dimensions and containing on the main
    diagonal the edges conductances and all other terms 0. For AC circuits, if there are some
    mutual inductances between edges, then the corresponding terms in G matrix will take into
    account these terms non equal to 0.

    The matrix Ue will be a column matrix having e elements and will contains the voltage
    sources of the circuits positioned on the corresponding edges. If an edge does not contain any
    voltage source then the corresponding value of the matrix Ue will be 0.

    */
    /*
    Another matrix that can be used for a graph is the matrix B, called the matrix of an
      independent loops system, having the dimensions(l x e), where l = e - n, and has the
    elements :
      1 bij if the edge j belongs to the loop i and has the same direction.
      -1 ij b if the edge j belongs to the loop i and has the opposite direction.
      0 bij if the edge j does not belong to the loop i.
   */

    mv_nLoops = mv_nEdges - mv_nNodes;

    /*ublas::matrix<int> B(mv_nLoops, mv_nEdges,0);
    ublas::matrix<int> A(mv_nNodes, mv_nEdges,0);
    ublas::matrix<double> G(mv_nEdges, mv_nEdges, 0);
    ublas::matrix<double> Ue(mv_nEdges, 1, 0);
    ublas::matrix<double> R(mv_nEdges, 1, 0);
  
    for (int i = 0; i < mv_nNodes; i++)
    {
      for (size_t j = 0; j < mv_nEdges; j++)
      {
        int fromID = mv_AllEdgesInCircuit[j].FromNode.mf_GetNodeID();
        int refID = mv_Nodes_List[i].mf_GetNodeID();
        int toID = mv_AllEdgesInCircuit[j].ToNode.mf_GetNodeID();
        if (fromID == refID)
        {
          A(i, j) = -1;
        }
        else if (toID == refID)
        {
          A(i, j) = 1;
        }
        else
        {
          A(i, j) = 0;
        }
        for (QGraphicsItem * it : mv_AllEdgesInCircuit[j].Components)
        {
          if (it->type() == SimulationUtils::ElectronicElements::Element_DC_Source)
          {
            CVoltageSource *lv_VoltageSource = qgraphicsitem_cast<CVoltageSource *>(it);
            Ue(j, 0) = lv_VoltageSource->mf_dfGetVoltage();
          }
          if (it->type() == SimulationUtils::ElectronicElements::Element_Resistor)
          {
            CResistor *lv_Resistor = qgraphicsitem_cast<CResistor *>(it);
            const int lc_nResistance = lv_Resistor->mf_nGetResistance();
            G(j, j) = (1. / lc_nResistance);
            R(j, 0) = lc_nResistance;
          }
        }
      }
    }*/
       
    /*std::stringstream buffer;
      std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

      std::cout << "Bla" << std::endl;

    std::string text = buffer.str(); // text will now contain "Bla\n"*/

   /* std::cout << "\nIncidence Matrix :" << A << std::endl;
    std::cout << "Voltage source Matrix: " << Ue << std::endl;
    std::cout << "Conductance Matrix: " << G << std::endl;
    */


   /* NodePotentialSolver = new CNodePotential(G,Ue,A);
    NodePotentialSolver->mf_Solve();
*/
   /* std::cout << "   At      " << NodePotentialSolver->At << std::endl;
    std::cout << "   AG      " << NodePotentialSolver->AG << std::endl;
    std::cout << "   Gp      " << NodePotentialSolver->Gp << std::endl;
    std::cout << "   minusA  " << NodePotentialSolver->minusA << std::endl;
    std::cout << "   minusAG " << NodePotentialSolver->minusAG << std::endl;
    std::cout << "   IpSC    " << NodePotentialSolver->IpSC << std::endl;
    std::cout << "   Vp      " << NodePotentialSolver->Vp << std::endl;*/
    
   /* std::cout << "   U      " << NodePotentialSolver->U << std::endl;
    std::cout << "   I      " << NodePotentialSolver->I << std::endl;*/

    std::string text = buffer.str();
    CGridUtils::sc_xTheGrid->mf_SetGridText(text.data());
    
    LogFile.close();
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
