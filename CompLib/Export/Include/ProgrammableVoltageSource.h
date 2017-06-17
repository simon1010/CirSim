#pragma once

#ifndef PROGRAMMABLEVOLTAGESOURCE_H
#define PROGRAMMABLEVOLTAGESOURCE_H

#include <IComponent.h>
#include <qgraphicsitem.h>


#include <complib_global.h>

class COMPLIB_EXPORT CProgrammableVoltageSource : public IComponent
{
  enum Waveform
  {
    WF_DC,
    WF_AC,
    WF_SQUARE,
    WF_TRIANGLE,
    WF_SAWTOOTH,
    WF_PULSE,

    WF_COUNT
  } mv_Waveform;
public:
  //CVoltageSource(QGraphicsScene *ac_pScene, QGraphicsItem *ac_pParent = 0);
  CProgrammableVoltageSource(QPointF ac_Position, QGraphicsScene *ac_pScene, QGraphicsView *ac_pParent = 0);
  ~CProgrammableVoltageSource();

  QRectF boundingRect() const;
  void mf_DerivedPaint(QPainter *lc_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);

  enum { Type = SimulationUtils::ElectronicElements::Element_ProgrammableVoltageSource };
  int type() const { return Type; }
  void mf_SetMaxVoltage(const double ac_dfMaxVoltage);
  double mf_dfGetMaxVoltage() const;
  
  SAMPLE *mf_dfGetVoltage(int& av_nSamplesAvailable);
 
  void mf_Save(QJsonObject &json);

  const static double sc_dfDefaultMaxVoltage;

private:
  //virtual DialogSettingsMap mf_GetDialogSettingsMap();
  //virtual void mf_SetDialogSettingsMap(DialogSettingsMap ac_NewSettings);

  static int sv_nPVSID;

  virtual QString mf_ToolTipGetType();
  virtual QString mf_ToolTipGetValue();
  virtual QString mf_ToolTipGetUnit();

  virtual const int mf_nGetInstanceNumber() const;

  Terminal *mv_Terminal_Plus;
  Terminal *mv_Terminal_Minus;

  /*Processing*/
  double mv_dfMaxVoltage;
  double mv_dfFrequency;
  double mv_dfPhaseShift;
  double mv_dfDutyCycle;
  double mv_dfFreqT0;
  double mv_dfBias; 

};

#endif // COMP_VOLTAGESOURCE_H