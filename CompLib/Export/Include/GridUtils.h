#pragma once
#include <Grid.h>
#include <qcursor.h>

#ifndef GRIDUTILS_H
#define GRIDUTILS_H

class CGridUtils
{
public:

  static CGrid *sc_xTheGrid;

  static int RoundUp(int toRound, int ac_nGridSize)
  {
    if (toRound % ac_nGridSize == 0)
      return toRound;
    return (ac_nGridSize - toRound % ac_nGridSize) + toRound;
  }

  static int RoundDown(int toRound, int ac_nGridSize)
  {
    if (toRound % ac_nGridSize == 0)
      return toRound;
    return toRound - toRound % ac_nGridSize;
  }

  static int Round(int toRound, int ac_nGridSize)
  {
    if ((toRound % ac_nGridSize) > (ac_nGridSize / 2))
    {
      return RoundUp(toRound, ac_nGridSize);
    }
    else
    {
      return RoundDown(toRound, ac_nGridSize);
    }
  }


  static QPointF sf_xAdjustPositionToGrid(const QPointF &ac_nPosition)
  {
    /* This means you called position adjustment before any grid was created */
    _ASSERT(sc_xTheGrid != nullptr);
    const int lc_nGridSize = sc_xTheGrid->mf_nGetGridSize();
    qreal xV = round(ac_nPosition.x() / lc_nGridSize)*lc_nGridSize;
    qreal yV = round(ac_nPosition.y() / lc_nGridSize)*lc_nGridSize;
    return QPointF(xV, yV);
  }

  // basic snap to grid for cursor, kinda course but it will work for now
  static void sf_xCursorToGrid()
  {
    // see what the mortal wants and compute direction vector :>
    QPoint lv_CursorPosition1 = QCursor::pos();
    _sleep(30);
    QPoint lv_CursorPosition2 = QCursor::pos();

    QPoint Directionality = lv_CursorPosition1 - lv_CursorPosition2;
    QPoint lv_CursorPosition;

    // help mortal decide where to snap the cursor
    if (Directionality.x() > 0)
    {
      lv_CursorPosition = QPoint(ceil(lv_CursorPosition2.x()), lv_CursorPosition2.y());
    }
    else
    {
      lv_CursorPosition = QPoint(floor(lv_CursorPosition2.x()), lv_CursorPosition2.y());
    }
    if (Directionality.y() > 0)
    {
      lv_CursorPosition = QPoint(lv_CursorPosition2.x(), ceil(lv_CursorPosition2.y()));
    }
    else
    {
      lv_CursorPosition = QPoint(lv_CursorPosition2.x(), floor(lv_CursorPosition2.y()));
    }

    QCursor::setPos(CGridUtils::sf_xAdjustPositionToGrid(lv_CursorPosition).toPoint());
  }
};


#endif