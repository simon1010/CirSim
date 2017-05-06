#pragma once

#ifndef STGRECTANGLE_H
#define STGRECTANGLE_H

#include <QGraphicsRectItem>
#include <QApplication>
#include <Grid.h>
#include <QObject>

class CSTGRegctangle : public QObject, public QGraphicsRectItem
{
  Q_OBJECT
public:
  
  CSTGRegctangle(QRect &ac_xRect, QGraphicsItem *ac_pParent = nullptr, CGrid* ac_pScene = nullptr);
  
  CSTGRegctangle(QGraphicsRectItem &ac_xCopyFrom);

private:

  QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  QPointF mf_xAdjustPositionToGrid(const unsigned ac_nGridSize, const QPointF &ac_nPosition);
  QRect mf_xAdjustRectToGrid(const QRect &ac_xRectangle, const unsigned ac_nGridSize);

  const CGrid * const mv_xTheGrid;

};


#endif