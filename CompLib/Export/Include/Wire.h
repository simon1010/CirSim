#pragma once

#ifndef WIRE_H
#define WIRE_H

#include <IComponent.h>
#include <qgraphicsitem.h>
#include <Terminal.h>

#include <complib_global.h>

class COMPLIB_EXPORT CWire : public QGraphicsLineItem//, virtual IComponent
{
public:
  CWire(QPointF ac_Position, QGraphicsScene *ac_pScene, QGraphicsView *ac_pParent = 0);
  ~CWire();

  QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  QRectF boundingRect() const;
  void paint(QPainter *lc_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);
  void setLine(const QLineF & ac_Line);

  enum { Type = SimulationUtils::ElectronicElements::Element_Wire };
  int type() const { return Type; }

  virtual Terminal* mf_OtherTerminal(Terminal * ac_NotThisTerminal)const;

  void mf_Save(QJsonObject &json);

private:
  bool mv_bHasKeyboard;
  QGraphicsView *mv_pParent;
  void keyPressEvent(QKeyEvent *event);

  Terminal *mv_Terminal_1;
  Terminal *mv_Terminal_2;
};

#endif // COMP_VOLTAGESOURCE_H