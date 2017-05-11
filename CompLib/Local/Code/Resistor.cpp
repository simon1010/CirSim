#include "Resistor.h"

// Used to identify each resistor in the classic notation: e.g. R1, R2, ... Rn 
int CResistor::sv_nResistorID = 0;

CResistor::CResistor(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pParent) : 
IComponent(ac_Position, ac_pScene, ac_pParent)
{
  mv_sCompID = QString("R");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);
  
  mv_Terminal_1 = new Terminal(ac_pScene, this);
  mv_Terminal_2 = new Terminal(ac_pScene, this);

  mf_AddTerminal(mv_Terminal_1);
  mf_AddTerminal(mv_Terminal_2);

  mv_nResistance = 50;

  // Simple instance counter for ID
  ++sv_nResistorID;
}

CResistor::~CResistor()
{
}

QRectF CResistor::boundingRect() const
{
  return QRectF(QPointF(-2,0),QPointF(22,22));
}

void CResistor::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  // TODO: update according to gridSize and cursor position -> ( gridSize, 0, gridSize, gridSize )  
 
  QRect lv_ResistorBody(QPoint(3, 7), QPoint(16, 13));
  QLine lv_xLeftLeg(QPoint(0,10), QPoint(3,10));
  QLine lv_xRightLeg(QPoint(17, 10), QPoint(20, 10));

  ac_pPainter->drawRect(lv_ResistorBody);
  ac_pPainter->drawLine(lv_xLeftLeg);
  ac_pPainter->drawLine(lv_xRightLeg);
  
  mv_Terminal_1->setPos(QPoint(-1, 10));
  mv_Terminal_2->setPos(QPoint(20, 10));

  mv_Terminal_1->paint(ac_pPainter, ac_pOption, ac_pParent, true);
  mv_Terminal_2->paint(ac_pPainter, ac_pOption, ac_pParent, true);

}

QString CResistor::mf_ToolTipGetType()
{
  return QString("Resistor ");
}

QString CResistor::mf_ToolTipGetValue()
{
  char value[10];
  sprintf(value, "%d", mv_nResistance);
  return QString(value);
}

QString CResistor::mf_ToolTipGetUnit()
{
  return QString(" OHM");
}


// This is where the magic happens
void CResistor::Process_(DspSignalBus & inputs, DspSignalBus & outputs)
{
  //TODO: IMPLEMENT
}

int CResistor::mf_nGetResistance()
{
  return mv_nResistance;
}


IComponent::DialogSettingsMap CResistor::mf_GetDialogSettingsMap()
{
  DialogSettingsMap lv_Variables;
  lv_Variables.insert(std::make_pair(QString("Resistance"), mv_nResistance));

  return lv_Variables;
}

void CResistor::mf_SetDialogSettingsMap(IComponent::DialogSettingsMap ac_SettingsMap)
{
  mv_nResistance = ac_SettingsMap[QString("Resistance")];
}

double CResistor::mf_dfGetVoltage()
{
  return 5;
}

SAMPLE* CResistor::mf_dfGetVoltage(int &av_nAvailableSamples)
{
  av_nAvailableSamples = 1;
  SAMPLE * sample = new SAMPLE;
  *sample = 5;
  return sample;
}

const int CResistor::mf_nGetInstanceNumber() const
{
  return sv_nResistorID;
}
