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
  
  virtual DialogSettingsMap mf_GetDialogSettingsMap();
  virtual void mf_SetDialogSettingsMap(DialogSettingsMap ac_SettingsMap);

  virtual QString mf_ToolTipGetType();
  virtual QString mf_ToolTipGetValue();
  virtual QString mf_ToolTipGetUnit();

  virtual const int mf_nGetInstanceNumber() const;

  typedef IDispatchComponent _super;

  void Process_(DspSignalBus & inputs, DspSignalBus & outputs);
  void mf_DoCapacitor(const double ac_P0_VoltageIn, const double ac_P1_VoltageIn, DspSignalBus &inputs, DspSignalBus &outputs);

  double mv_dfCapacitance;
  double mv_dfCurrent;
  double mv_dfCapVoltage;
  double mv_dfoutputVoltage;
  double mv_dfPreviousOutputVoltage;
  double mv_dfResistance;
  double mv_EquivalentResistance;
};

#endif