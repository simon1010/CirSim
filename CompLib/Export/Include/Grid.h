#pragma once
#ifndef GRID_H
#define GRID_H

#include <qgraphicsscene.h>
#include <QResizeEvent>
class CGrid : public QGraphicsScene
{
public:
  CGrid(QObject *parent = nullptr, const unsigned ac_nGridSize = 0);
  ~CGrid();

  void drawBackground(QPainter *painter, const QRectF &rect);
  
  const unsigned int mf_nGetGridSize() const;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *ac_pEvent);
  //void mousePressEvent(QGraphicsSceneMouseEvent *ac_pEvent);

  // Grid text: to use as live logger
  // note: no text formatting is done here!
  QString mf_szGetTheGridText() const;
  void mf_SetGridText(const QString& ac_szTextToSet);
  void mf_AddToGridText(const QString& ac_szTextToAdd);

private:
  const unsigned int mc_nGridSize;
  QPen mv_xGridPen;
  QString mv_szTheGridText;
};

#endif