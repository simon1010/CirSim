#pragma once

#ifndef COMP_GROUND_H
#define COMP_GROUND_H

#include <IComponent.h>
#include <SimulationUtils.h>

#include <complib_global.h>

class COMPLIB_EXPORT CGround : public IComponent
{
public:
  
  CGround(QPointF ac_Position, QGraphicsScene *ac_pScene, QGraphicsView *ac_pParent = 0);
  ~CGround();

  QRectF boundingRect() const;
  void mf_DerivedPaint(QPainter *lc_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);

  enum { Type = SimulationUtils::ElectronicElements::Element_Ground };
  int type() const { return Type; }

  double mf_dfGetVoltage();
  SAMPLE* mf_dfGetVoltage(int &av_nAvailableSamples, double ** av_pTimesVec);

  void mf_Save(QJsonObject &json);
private:
  Terminal *mv_Terminal;

  static int sv_nGroundID;

  virtual QString mf_ToolTipGetType(){ return QString("Ground"); }
  virtual QString mf_ToolTipGetValue(){ return QString(""); };
  virtual QString mf_ToolTipGetUnit(){ return QString(""); };

  virtual const int mf_nGetInstanceNumber() const;
};

#endif // COMP_VOLTAGESOURCE_H