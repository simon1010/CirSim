#pragma once

#ifndef RESISTOR_H
#define RESISTOR_H

#include <IComponent.h>
#include <ComponentDialog.h>
#include <QPainter>
#include <QWidget>

#include <DSPatch\DspComponent.h>

#include <complib_global.h>

class COMPLIB_EXPORT CResistor : public IComponent
{
public:
  CResistor(QPointF ac_Position, QGraphicsScene *ac_pScene, QGraphicsView *ac_pParent = 0);
  ~CResistor();

  void mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);
  QRectF boundingRect() const;

  enum { Type = SimulationUtils::ElectronicElements::Element_Resistor };
  int type() const { return Type; }

  /* Processing elements */
  int mv_nResistance;
  int mf_nGetResistance();

  double mf_dfGetVoltage();
  SAMPLE* mf_dfGetVoltage(int &av_nAvailableSamples);

  void mf_Save(QJsonObject &json);
  void mf_Load(QJsonObject &json) override;

private:

  /* Connection elements */
  Terminal *mv_Terminal_1;
  Terminal *mv_Terminal_2;

  static int sv_nResistorID;

  virtual DialogSettingsMap mf_GetDialogSettingsMap();
  virtual void mf_SetDialogSettingsMap(DialogSettingsMap ac_SettingsMap);

  virtual QString mf_ToolTipGetType() ;
  virtual QString mf_ToolTipGetValue();
  virtual QString mf_ToolTipGetUnit() ;

  const int mf_nGetInstanceNumber() const;
  // Inherited virtual method from DspComponent
private:
  virtual void Process_(DspSignalBus& inputs, DspSignalBus& outputs);
 };

#endif