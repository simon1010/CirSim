#pragma once

#ifndef COMP_VOLTAGESOURCE_H
#define COMP_VOLTAGESOURCE_H

#include <IComponent.h>
#include <qgraphicsitem.h>


class CVoltageSource : public IComponent
{
public:
  //CVoltageSource(QGraphicsScene *ac_pScene, QGraphicsItem *ac_pParent = 0);
  CVoltageSource(QPointF ac_Position, QGraphicsScene *ac_pScene, QGraphicsView *ac_pParent = 0);
  ~CVoltageSource();

  QRectF boundingRect() const;
  void mf_DerivedPaint(QPainter *lc_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);
  
  enum { Type = SimulationUtils::ElectronicElements::Element_DC_Source };
  int type() const { return Type; }
  void mf_SetVoltage(const double ac_dfVoltage);
  double mf_dfGetVoltage();
  SAMPLE* mf_dfGetVoltage(int &av_nAvailableSamples);
  
  const static double sc_dfDefaultVoltage;

private:
  virtual DialogSettingsMap mf_GetDialogSettingsMap();
  virtual void mf_SetDialogSettingsMap(DialogSettingsMap ac_NewSettings);

  virtual QString mf_ToolTipGetType();
  virtual QString mf_ToolTipGetValue();
  virtual QString mf_ToolTipGetUnit();

  Terminal *mv_Terminal_Plus;
  Terminal *mv_Terminal_Minus;

  /*Processing*/
  double mv_dfVoltage;
};

#endif // COMP_VOLTAGESOURCE_H