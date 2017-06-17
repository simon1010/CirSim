#include <CircuitComposer.h>
#include <SimulationUtils.h>
#include <Node.h>
#include <Wire.h>
#include <fstream>

static const char * LogPath = "c:\\CIRSIM_LOGGING\\ComposerLog.log";

CCircuitComposer::CCircuitComposer(const QList<QGraphicsItem*>& ac_QList_AllElements)
{  
  // Interpret the graphic elements into usable circuit components
  mf_InterpretQGrapghicsItems(ac_QList_AllElements);
}

void CCircuitComposer::mf_InterpretQGrapghicsItems(const QList<QGraphicsItem*>& ac_QList_AllElements)
{
  SimulationUtils::sf_PrintElementsToFile(ac_QList_AllElements, LogPath);

  // todo: do some pre-verifications?

  // identify and add to circuit
  for (QGraphicsItem * it : ac_QList_AllElements)
  {
    IComponent* lv_pComponentCast = dynamic_cast<IComponent*>(it);
    if (lv_pComponentCast != nullptr)
    {
      mv_TheCircuit.AddComponent(*lv_pComponentCast);
    }
  }

  auto const componentCount = mv_TheCircuit.GetComponentCount();
  // make the connections

}