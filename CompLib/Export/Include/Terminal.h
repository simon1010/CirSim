#pragma once

#ifndef TERMINAL_H
#define TERMINAL_H

#include <QGraphicsItem>
#include <IComponent.h>
#include <SimulationUtils.h>

// Always call setPos(); !!!
// Use paint(.., bool) with boolean
#include <complib_global.h>

class COMPLIB_EXPORT Terminal : public QGraphicsItem
{
public:

  enum { Type = SimulationUtils::ElectronicElements::Element_Terminal };
  int type() const { return Type; }

  Terminal(QGraphicsScene* ac_pScene, QGraphicsItem *ac_pParent) : QGraphicsItem(ac_pParent),
    mv_pTerminalOf(ac_pParent)
  {
    // maybe redundant?
    setParentItem(ac_pParent);
    mv_nPolarity = -1;
    mv_bIsConnected = false;
  }

  __forceinline int mf_nGetPolarity() const
  {
    return mv_nPolarity;
  }

  __forceinline void mf_nSetPolarity(const int ac_nPolarity)
  {
    mv_nPolarity = ac_nPolarity;
  }

  __forceinline void paint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
  {

  }

  __forceinline void paint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent, bool av_bDraw)
  {
    if (av_bDraw)
    {
      ac_pPainter->setPen(QPen(Qt::red));
      ac_pPainter->drawRect(QRectF(pos().x(), pos().y() - 1, 1, 2));
    }
  }

  __forceinline QRectF boundingRect() const
  {
    return QRectF(0, 0, 2, 2);
  }
  
  __forceinline QGraphicsItem *mf_pTerminalOf() const
  {
    return mv_pTerminalOf;
  }

  __forceinline bool mf_bIsConnected() const
  {
    return mv_bIsConnected;
  }

  __forceinline bool mf_bConnect()
  {
    mv_bIsConnected = true;
    return mv_bIsConnected;
  }

private:

  QGraphicsItem *mv_pTerminalOf;
  bool mv_bIsConnected;

  // +1 or -1 used by polarity sensitive components
  int mv_nPolarity; 
};

#endif