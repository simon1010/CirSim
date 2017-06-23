#include <CircuitComposer.h>
#include <SimulationUtils.h>
#include <Node.h>
#include <Wire.h>
#include <fstream>
#include <algorithm>

static const char * LogPath = "c:\\CIRSIM_LOGGING\\ComposerLog.log";
namespace Solver {

  CCircuitComposer::CCircuitComposer(const QList<QGraphicsItem*>& ac_QList_AllElements, const EDGE_VEC& ac_AllEdges, const NODE_VEC& ac_AllNodes)
  {
    // Interpret the graphic elements into usable circuit components
    mf_InterpretQGrapghicsItems(ac_QList_AllElements, ac_AllEdges, ac_AllNodes);
  }

  CCircuitComposer::~CCircuitComposer()
  {
    DSPatch::Finalize();
  }

  void CCircuitComposer::mf_InterpretQGrapghicsItems(const QList<QGraphicsItem*>& ac_QList_AllElements, const EDGE_VEC& ac_AllEdges, const NODE_VEC& ac_AllNodes)
  {
    SimulationUtils::sf_PrintElementsToFile(ac_QList_AllElements, LogPath);

    auto const lv_nNumberOfEdges = ac_AllEdges.size();
    for (int i = 0; i < lv_nNumberOfEdges; ++i)
    {
      for (QGraphicsItem * it : ac_AllEdges[i].Components)
      {
        IComponent* lv_pComponentCast = dynamic_cast<IComponent*>(it);
        if (lv_pComponentCast != nullptr)
        {
          if (mv_TheCircuit.AddComponent(*lv_pComponentCast, lv_pComponentCast->mv_szGetComponentID()))
          {
            mv_MyComponents.push_back(lv_pComponentCast);
          }
        }
      }
    }

    std::reverse(mv_MyComponents.begin(), mv_MyComponents.end());

    int size = mv_MyComponents.size();

    for (int i = 0; i < size; i++)
    {
      if (i + 1 < size)
      {
        PortConnect(mv_TheCircuit, *mv_MyComponents[i], 1, *mv_MyComponents[i + 1], 0);
      }
      else
      {
        PortConnect(mv_TheCircuit, *mv_MyComponents[i], 1, *mv_MyComponents[0], 0);
      }
    }
  }

  void CCircuitComposer::step()
  {
      mv_TheCircuit.Tick();
      mv_TheCircuit.Reset();
  }

  // Start from a voltage source, to follow this path one must handle ALL terminals
  //const IComponent* lc_StartComp = mf_GetFirstVoltageSource();
  //const IComponent* lc_NextComp = mf_ConnectsTo(lc_StartComp);

  const IComponent*  CCircuitComposer::mf_GetFirstVoltageSource() const
  {
    for (auto it : mv_MyComponents)
    {
      if ((it->type() == SimulationUtils::ElectronicElements::Element_ProgrammableVoltageSource)
        || (it->type() == SimulationUtils::ElectronicElements::Element_DC_Source)
        || (it->type() == SimulationUtils::ElectronicElements::Element_Microphone))
      {
        return it;
      }
    }
  }

  const IComponent*  CCircuitComposer::mf_ConnectsTo(const IComponent* ac_pConectee) const
  {
    QGraphicsItem * nextConnection = ac_pConectee->mf_GetTerminal(1)->mf_ConnectsTo();
    IComponent* nextElement = dynamic_cast<IComponent*>(nextConnection);
    while (nextElement == nullptr)
    {
      CWire * wire = dynamic_cast<CWire *>(nextConnection);
      nextConnection = wire->mf_GetTerminal(1)->mf_ConnectsTo();
      nextElement = dynamic_cast<IComponent*>(nextConnection);
    }
    return nextElement;
  }
}


