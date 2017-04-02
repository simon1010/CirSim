#include "Capacitor.h"
#include <qgraphicssceneevent.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QWidget>

const double CCapacitor::sc_dfDefaultCapacitance = 500; //microFarads

CCapacitor::CCapacitor(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent) : IComponent(ac_Position, ac_pScene, ac_pPrent)
{
  mv_sCompID = QString("C");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);

  mv_Terminal_1 = new Terminal(ac_pScene, this);
  mv_Terminal_2 = new Terminal(ac_pScene, this);

  mf_AddTerminal(mv_Terminal_1);
  mf_AddTerminal(mv_Terminal_2);

  mf_SetCapacitance(sc_dfDefaultCapacitance);
}

CCapacitor::~CCapacitor()
{
}

QRectF CCapacitor::boundingRect() const
{
  return QRectF(0, 0, 20, 20);
}

void CCapacitor::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  // TODO: update according to gridSize and cursor position -> ( gridSize, 0, gridSize, gridSize )  

  QLine lv_UpperConnector(10, 0, 10, 10);
  QLine lv_LowerConnector(10, 12, 10, 20);

  QLine lv_PositiveElectrode(5, 10, 15, 10); // longer
  QLine lv_NegativeElectrode(5, 12, 15, 12);

  ac_pPainter->drawLine(lv_UpperConnector);
  ac_pPainter->drawLine(lv_PositiveElectrode);
  ac_pPainter->drawLine(lv_LowerConnector);
  ac_pPainter->drawLine(lv_NegativeElectrode);

  mv_Terminal_1->setPos(9, 0);
  mv_Terminal_2->setPos(9, 20);

  mv_Terminal_1->paint(ac_pPainter, ac_pOption, ac_pParent, true);
  mv_Terminal_2->paint(ac_pPainter, ac_pOption, ac_pParent, true);
}

void CCapacitor::mf_SetCapacitance(const double ac_dfCapacitance)
{
  mv_dfCapacitance = ac_dfCapacitance;
}

double CCapacitor::mf_dfGetCapacitance() const
{
  return mv_dfCapacitance;
}

QString CCapacitor::mf_ToolTipGetType()
{
  return QString("Capacitor ");
}

QString CCapacitor::mf_ToolTipGetValue()
{
  char value[10];
  sprintf(value, "%.3f", mv_dfCapacitance);
  return QString(value);
}

QString CCapacitor::mf_ToolTipGetUnit()
{
  return QString(" uF");
}

IComponent::DialogSettingsMap CCapacitor::mf_GetDialogSettingsMap()
{
  DialogSettingsMap lv_Variables;
  lv_Variables.insert(std::make_pair(QString("Capacitance"), mv_dfCapacitance));

  return lv_Variables;
}

void CCapacitor::mf_SetDialogSettingsMap(IComponent::DialogSettingsMap ac_SettingsMap)
{
  mv_dfCapacitance = ac_SettingsMap[QString("Capacitance")];
}

double CCapacitor::mf_dfGetVoltage()
{
  return 200;
}

SAMPLE* CCapacitor::mf_dfGetVoltage(int &av_nAvailableSamples)
{
  av_nAvailableSamples = 1;
  SAMPLE * sample = new SAMPLE;
  *sample = 200;
  return sample;
}