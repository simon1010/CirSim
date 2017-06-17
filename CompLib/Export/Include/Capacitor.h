#pragma once

#ifndef CAPACITOR_H
#define CAPACITOR_H

#include <IComponent.h>
#include <ComponentDialog.h>
#include <QPainter>
#include <QWidget>

#include <complib_global.h>

class COMPLIB_EXPORT CCapacitor : public IComponent
{
public:
  CCapacitor(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent);
  ~CCapacitor();

  QRectF boundingRect() const;
  void mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);

  enum { Type = SimulationUtils::ElectronicElements::Element_Capacitor };
  int type() const { return Type; }


  /* Processing elements */
  void mf_SetCapacitance(const double ac_dfCapacitance);
  double mf_dfGetCapacitance() const;

  double mf_dfGetVoltage();
  SAMPLE* mf_dfGetVoltage(int &av_nAvailableSamples);

  void mf_Save(QJsonObject &json);
  void mf_Load(QJsonObject &json) override;

private:
  static const double sc_dfDefaultCapacitance;

  static int sv_nCapacitorID;

  Terminal *mv_Terminal_1;
  Terminal *mv_Terminal_2;
  
  double mv_dfCapacitance;

  virtual DialogSettingsMap mf_GetDialogSettingsMap();
  virtual void mf_SetDialogSettingsMap(DialogSettingsMap ac_SettingsMap);

  virtual QString mf_ToolTipGetType();
  virtual QString mf_ToolTipGetValue();
  virtual QString mf_ToolTipGetUnit();

  virtual const int mf_nGetInstanceNumber() const;
};

#endif