#pragma once

#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <IComponent.h>
#include <QPainter>
#include <QWidget>
#include <portaudio.h>
#include <pa_asio.h>
#include <mutex>

class CMicrophone : public IComponent
{
public:
  CMicrophone(QPointF ac_Position, QGraphicsScene *ac_pScene, QGraphicsView *ac_pParent = 0);
  ~CMicrophone();

  void mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);
  QRectF boundingRect() const;

  enum { Type = SimulationUtils::ElectronicElements::Element_Microphone };
  int type() const { return Type; }

  /* Processing elements */
  int mv_nImpedance;
  int mf_nGetImpedance();

  SAMPLE * mf_dfGetVoltage(int& av_nSamplesAvailable);

  void mf_SetupHardware();
  friend static int CallBack(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData);
  

  static SAMPLE ** mv_pAudioBuffers;
  static int BuffersInUse;
  static std::mutex AcquisitionGuard;
private:

  void mf_PreDestroy();

  /* Connection elements */
  Terminal *mv_Terminal_1;
  Terminal *mv_Terminal_2;

  virtual DialogSettingsMap mf_GetDialogSettingsMap();
  virtual void mf_SetDialogSettingsMap(DialogSettingsMap ac_SettingsMap);

  virtual QString mf_ToolTipGetType();
  virtual QString mf_ToolTipGetValue();
  virtual QString mf_ToolTipGetUnit();

  /* Specific */
  void mf_ConstructDialog();

  PaStream *mv_pAudioInputStream;
  void * mv_pAudioData;
  bool mv_bMaydenVoyage;
  bool mv_bIsStreamOpen;
  int mv_nSelectedInputDeviceIndex;

protected:
  void mf_ShowSettingsDialog();
};

#endif