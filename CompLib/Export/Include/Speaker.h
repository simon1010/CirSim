#pragma once
#include <IComponent.h>
#include <QPainter>
#include <QWidget>
#include <portaudio.h>
#include <pa_asio.h>
#include <mutex>

#include <complib_global.h>

class COMPLIB_EXPORT CSpeaker : public IComponent
{
public:
  CSpeaker(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pParent);
  ~CSpeaker();

  void mf_SetupHardware();

  void mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);
  QRectF boundingRect() const;

  enum { Type = SimulationUtils::ElectronicElements::Element_Speaker };
  int type() const { return Type; }

  SAMPLE * mf_dfGetVoltage(int& av_nSamplesAvailable);

  void mf_Save(QJsonObject & json);

private:
  void Process_(DspSignalBus & inputs, DspSignalBus & outputs);

  static int SpeakerCallBack(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData);

  PaStream *stream;

  void mf_PreDestroy();

  /* Connection elements */
  Terminal *mv_Terminal_1;
  Terminal *mv_Terminal_2;

  //virtual DialogSettingsMap mf_GetDialogSettingsMap();
  //virtual void mf_SetDialogSettingsMap(DialogSettingsMap ac_SettingsMap);

  virtual QString mf_ToolTipGetType();
  virtual QString mf_ToolTipGetValue();
  virtual QString mf_ToolTipGetUnit();

  virtual const int mf_nGetInstanceNumber() const;

  /* Specific */
  //void mf_ConstructDialog();
  static double mv_OutputSample;
  bool mv_bMaydenVoyage = { true };


protected:
  //void mf_ShowSettingsDialog();
};