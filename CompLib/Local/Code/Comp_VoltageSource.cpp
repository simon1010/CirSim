#include "Comp_VoltageSource.h"
#include <qgraphicssceneevent.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QWidget>

const double CVoltageSource::sc_dfDefaultVoltage = 5.0;

CVoltageSource::CVoltageSource(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent) : IComponent(ac_Position, ac_pScene, ac_pPrent)
{
  mv_sCompID = QString("E");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);

  mv_Terminal_Plus  = new Terminal(ac_pScene, this);
  mv_Terminal_Minus = new Terminal(ac_pScene, this);

  mv_Terminal_Plus->mf_nSetPolarity(+1);
  mv_Terminal_Minus->mf_nSetPolarity(-1);
 
  mf_AddTerminal(mv_Terminal_Plus);
  mf_AddTerminal(mv_Terminal_Minus);

  mf_SetVoltage(sc_dfDefaultVoltage);
}


CVoltageSource::~CVoltageSource()
{
}

QRectF CVoltageSource::boundingRect() const
{
  return QRectF(0, 0, 20, 20);
}

void CVoltageSource::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  // TODO: update according to gridSize and cursor position -> ( gridSize, 0, gridSize, gridSize )  
  
  QLine lv_UpperConnector(10, 0 , 10, 10);
  QLine lv_LowerConnector(10, 12, 10, 20);

  QLine lv_PositiveElectrode(0, 10, 20, 10); // longer
  QLine lv_NegativeElectrode(5, 12, 15, 12);
    
  ac_pPainter->drawLine(lv_UpperConnector);
  ac_pPainter->drawLine(lv_PositiveElectrode);
  ac_pPainter->drawLine(lv_LowerConnector);
  ac_pPainter->drawLine(lv_NegativeElectrode);

  mv_Terminal_Plus->setPos(9, 0);
  mv_Terminal_Minus->setPos(9, 20);
  
  mv_Terminal_Plus->paint(ac_pPainter, ac_pOption, ac_pParent, true);
  mv_Terminal_Minus->paint(ac_pPainter, ac_pOption, ac_pParent, true);
}

void CVoltageSource::mf_SetVoltage(const double ac_dfVoltage)
{
  mv_dfVoltage = ac_dfVoltage;
}

double CVoltageSource::mf_dfGetVoltage()
{
  return mv_dfVoltage;
}


QString CVoltageSource::mf_ToolTipGetType()
{
  return QString("DC Voltage Source ");
}

QString CVoltageSource::mf_ToolTipGetValue()
{
  char value[10];
  sprintf(value, "%.2f", mf_dfGetVoltage());
  return QString(value);
}

QString CVoltageSource::mf_ToolTipGetUnit()
{
  return QString(" V");
}

const int CVoltageSource::mf_nGetInstanceNumber() const
{
  return 0;
}

void CVoltageSource::Process_(DspSignalBus & inputs, DspSignalBus & outputs)
{
  // TODO: IMPLEMENT
}

IComponent::DialogSettingsMap CVoltageSource::mf_GetDialogSettingsMap()
{
  DialogSettingsMap lv_Variables;
  lv_Variables.insert(std::make_pair(QString("Voltage"), mf_dfGetVoltage()));

  return lv_Variables;
}

void CVoltageSource::mf_SetDialogSettingsMap(IComponent::DialogSettingsMap ac_NewSettings)
{
  mf_SetVoltage(ac_NewSettings[QString("Voltage")]);
}

SAMPLE* CVoltageSource::mf_dfGetVoltage(int &av_nAvailableSamples, double ** av_pTimesVec)
{
  av_nAvailableSamples = 1;
  SAMPLE * sample = new SAMPLE;
  *sample = mf_dfGetVoltage();
  return sample;
}

void CVoltageSource::mf_Save(QJsonObject &json)
{
  json["name"] = "CVoltageSource";
  json["positionX"] = this->pos().x();
  json["positionY"] = this->pos().y();
  json["voltage"] = mf_dfGetVoltage();
}

void CVoltageSource::mf_Load(QJsonObject & json)
{
  mv_dfVoltage = json["voltage"].toDouble();
}
