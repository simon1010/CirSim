#include "Grid.h"
#include <qpainter.h>
#include <GridUtils.h>
#include <QCursor.h>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include <Windows.h>
#include <iostream>
#include <sstream>


//#define DEBUG

CGrid::CGrid(QObject *parent, const unsigned ac_nGridSize) :
QGraphicsScene(parent),
mc_nGridSize(ac_nGridSize)
{
}

CGrid::~CGrid()
{
}


void CGrid::drawBackground(QPainter *painter, const QRectF &rect)
{
  painter->setPen(mv_xGridPen);
  // use viewport instead of centered rect.
  // bring window size and adjust accordingly
 
  int LEFT = CGridUtils::Round(rect.left(), mc_nGridSize);// use absolute value because negative coords dont seem to work with %
  int RIGHT = rect.right();
  int TOP = CGridUtils::Round(rect.top(), mc_nGridSize);
  int BOTTOM = rect.bottom();

  int HORIZONAL_GRID_ELEMENTS_ADJUST = abs(LEFT) % mc_nGridSize;
  int VERTICAL_GRID_ELEMENTS_ADJUST = abs(TOP) % mc_nGridSize;

  qreal left = LEFT - HORIZONAL_GRID_ELEMENTS_ADJUST;
  qreal top = TOP - VERTICAL_GRID_ELEMENTS_ADJUST;
  QVector<QPointF> points;
  
  for (qreal x = left; x < RIGHT; x += mc_nGridSize){
    for (qreal y = top; y < BOTTOM; y += mc_nGridSize){
      points.append(QPointF(x, y));
    }
  }
  
#ifdef DEBUG
  for each (QPointF var in points)
  {
    char textX[10], textY[10];
    itoa(var.x(), textX, 10);
    itoa(var.y(), textY, 10);
    painter->drawText(var.toPoint(), QString(strcat(strcat(textX, ";"), textY)));
  }
#endif
  painter->drawPoints(points.data(), points.size());
}

const unsigned int CGrid::mf_nGetGridSize() const
{
  return mc_nGridSize;
}

void CGrid::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
 /* char MousePositionLogging[100];
  QTransform t;
  t.scale(1, -1);
  t.translate(0, -height() + 1);
  QPointF pos = event->pos() * t;

  sprintf(MousePositionLogging, "Position < %d | %d > ----> < %d | %d >", pos.x(), pos.y());*/
  
  QGraphicsScene::mouseMoveEvent(event);
}

QString CGrid::mf_szGetTheGridText() const
{
  return mv_szTheGridText;
}

void CGrid::mf_SetGridText(const QString& ac_szTextToSet)
{
  mv_szTheGridText = ac_szTextToSet;
}

void CGrid::mf_AddToGridText(const QString& ac_szTextToAdd)
{
  mv_szTheGridText += ac_szTextToAdd;
}