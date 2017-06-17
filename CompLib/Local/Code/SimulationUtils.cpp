#include <SimulationUtils.h>
#include <Wire.h>
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <fstream>

#include <Comp_VoltageSource.h>

QTime SimulationUtils::SimulationTime = QTime::currentTime();

std::function<float*(int&)> SimulationUtils::TheScopeValue = nullptr;

void SimulationUtils::sf_PrintElementsToFile(const QList<QGraphicsItem*>& ac_QList_AllElements, const char * ac_LogFile)
{
  QFile lv_JSonSaveFile(ac_LogFile);

  if (!lv_JSonSaveFile.open(QIODevice::WriteOnly)) {
    qWarning("Couldn't open save file.");
    return;
  }

  QJsonObject lv_qSimulationSaveObject;
  QJsonArray lv_JSonCompArray;
  /*for (QGraphicsItem * it : ac_QList_AllElements)
  { 
    QJsonObject lv_qComponentSaveObject;
    
    bool lv_bWrite = false;
    auto JSWrite = [&lv_bWrite]() {lv_bWrite = true; };

    IComponent * lv_pComponent = dynamic_cast<IComponent *>(it);
    switch (it->type())
    {
    case SimulationUtils::ElectronicElements::Element_DC_Source:
      
      lv_FileStream << "DC Source";
      lv_FileStream << " pos: x: " << it->pos().x() << " y : " << it->pos().y() << std::endl;

      lv_pComponent->mf_Save(lv_qComponentSaveObject);
      JSWrite();
      
      break;
    case SimulationUtils::ElectronicElements::Element_Ground:
      lv_FileStream << "Ground";
      lv_FileStream << " pos: x: " << it->pos().x() << " y : " << it->pos().y() << std::endl;

      lv_pComponent->mf_Save(lv_qComponentSaveObject);
      JSWrite();

      break;
    case SimulationUtils::ElectronicElements::Element_Resistor:
      lv_FileStream << "Resistor";
      lv_FileStream << " pos: x: " << it->pos().x() << " y : " << it->pos().y() << std::endl;

      lv_pComponent->mf_Save(lv_qComponentSaveObject);
      JSWrite();

      break;
    case SimulationUtils::ElectronicElements::Element_Terminal:
    {
      lv_FileStream << "Terminal" << std::endl;
      break;
    }
    case SimulationUtils::ElectronicElements::Element_Wire:
    {
      lv_FileStream << "Wire";
      CWire * lv_pWire = dynamic_cast<CWire*>(const_cast<QGraphicsItem *>(it));
      lv_FileStream << "from pos: x: " << lv_pWire->line().x1() << " y : " << lv_pWire->line().y1()
        << " to pos: x:  " << lv_pWire->line().x2() << " y : " << lv_pWire->line().y2() << std::endl;

      lv_pWire->mf_Save(lv_qComponentSaveObject);
      JSWrite();
    
      break;
    }
    case SimulationUtils::ElectronicElements::Element_ProgrammableVoltageSource:
    {
      lv_FileStream << "Programmable VS";
    lv_FileStream << " pos: x: " << it->pos().x() << " y : " << it->pos().y() << std::endl;

    lv_pComponent->mf_Save(lv_qComponentSaveObject);
    JSWrite();
    break;
    }
    case SimulationUtils::ElectronicElements::Element_Capacitor:
    {
      lv_FileStream << "Capacitor";
      lv_FileStream << " pos: x: " << it->pos().x() << " y : " << it->pos().y() << std::endl;

      lv_pComponent->mf_Save(lv_qComponentSaveObject);
      JSWrite();
      break;
    }
    default:
      break;
    }
    if (lv_bWrite)
      lv_JSonCompArray.append(lv_qComponentSaveObject);
  }*/
  
  for (QGraphicsItem * it : ac_QList_AllElements)
  {
    QJsonObject lv_qComponentSaveObject;

    bool lv_bWrite = false;
    auto JSWrite = [&lv_bWrite]() {lv_bWrite = true; };
    

    if (it->type() == SimulationUtils::ElectronicElements::Element_Wire)
    {
      CWire * lv_pWire = dynamic_cast<CWire*>(const_cast<QGraphicsItem *>(it));
      lv_pWire->mf_Save(lv_qComponentSaveObject);
      JSWrite();
    }
    else if (it->type() == SimulationUtils::ElectronicElements::Element_Terminal) 
    {
      // ???
    }
    else
    {
      IComponent * lv_pComponent = dynamic_cast<IComponent *>(it);
      if (lv_pComponent != nullptr) // we have text objects on grid also
      {
        lv_pComponent->mf_Save(lv_qComponentSaveObject);
        JSWrite();
      }
    }
    if (lv_bWrite)
      lv_JSonCompArray.append(lv_qComponentSaveObject);
  }

  lv_qSimulationSaveObject["components"] = lv_JSonCompArray;
  
  QJsonDocument lv_JSonSaveDoc(lv_qSimulationSaveObject);
  lv_JSonSaveFile.write(lv_JSonSaveDoc.toJson());
}