#include "Ground.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QWidget>

int CGround::sv_nGroundID = 0;

CGround::CGround(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent) : IComponent(ac_Position, ac_pScene, ac_pPrent)
{
  mv_sCompID = QString("G");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);

  mv_Terminal = new Terminal(ac_pScene, this);

  mf_AddTerminal(mv_Terminal);

  ++sv_nGroundID;
}


CGround::~CGround()
{
}

QRectF CGround::boundingRect() const
{
  return QRectF(0, 0, 20, 20);
}

void CGround::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  // TODO: update according to gridSize and cursor position -> ( gridSize, 0, gridSize, gridSize )  
  ac_pPainter->drawLine(10, 0, 10, 16);
  ac_pPainter->drawLine(4, 16, 16, 16);
  ac_pPainter->drawLine(6, 18, 14, 18);
  ac_pPainter->drawLine(8, 20, 12, 20);

  mv_Terminal->setPos(10, 0);
  mv_Terminal->paint(ac_pPainter, ac_pOption, ac_pParent, true);
}

double CGround::mf_dfGetVoltage()
{
  return 0;
}

SAMPLE* CGround::mf_dfGetVoltage(int &av_nAvailableSamples)
{
  av_nAvailableSamples = 1;
  SAMPLE * sample = new SAMPLE;
  *sample = 0;
  return sample;
}

const int CGround::mf_nGetInstanceNumber() const
{
  return sv_nGroundID;
}
