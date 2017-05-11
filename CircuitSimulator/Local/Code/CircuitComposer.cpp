#include <CircuitComposer.h>
#include <SimulationUtils.h>
#include <fstream>

static const char * LogPath = "c:\\CIRSIM_LOGGING\\ComposerLog.log";
static std::ofstream log_file;

CCircuitComposer::CCircuitComposer(const QList<QGraphicsItem*>& ac_QList_AllElements)
{  
  // Interpret the graphic elements into usable circuit components
  mf_InterpretQGrapghicsItems(ac_QList_AllElements);

  // Loggig
  log_file.open(LogPath, std::ofstream::app | std::ofstream::out);

}

void CCircuitComposer::mf_InterpretQGrapghicsItems(const QList<QGraphicsItem*>& ac_QList_AllElements)
{
  SimulationUtils::sf_PrintElementsToFile(ac_QList_AllElements, LogPath);
}
