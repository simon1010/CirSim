#include <SimulationUtils.h>
#include <fstream>

QTime SimulationUtils::SimulationTime = QTime::currentTime();

std::function<float*(int&)> SimulationUtils::TheScopeValue = nullptr;

void SimulationUtils::sf_PrintElementsToFile(const QList<QGraphicsItem*>& ac_QList_AllElements, const char * ac_LogFile)
{
  std::ofstream lv_FileStream;
  lv_FileStream.open(ac_LogFile, std::fstream::out);

  for (const QGraphicsItem * it : ac_QList_AllElements)
  {
    switch (it->type())
    {
    case SimulationUtils::ElectronicElements::Element_DC_Source:
      lv_FileStream << "DC Source" << std::endl;
      break;
    case SimulationUtils::ElectronicElements::Element_Ground:
      lv_FileStream << "Ground" << std::endl;
      break;
    case SimulationUtils::ElectronicElements::Element_Resistor:
      lv_FileStream << "Resistor" << std::endl;
      break;
    case SimulationUtils::ElectronicElements::Element_Terminal:
    {
      lv_FileStream << "Terminal" << std::endl;
      /*if (!mf_HandleTerminal(it))
      {
        std::cout << "Unconnected Terminal, no simulation is done!" << std::endl;
        std::string text = buffer.str();
        CGridUtils::sc_xTheGrid->mf_AddToGridText(text.data());
        return;
      }*/
      break;
    }
    case SimulationUtils::ElectronicElements::Element_Wire:
      lv_FileStream << "Wire" << std::endl;
      break;
    case SimulationUtils::ElectronicElements::Element_ProgrammableVoltageSource:
      lv_FileStream << "Programmable VS" << std::endl;
      break;
    default:
      break;
    }
  }

  lv_FileStream.close();
}