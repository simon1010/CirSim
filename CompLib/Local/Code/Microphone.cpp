#include <Microphone.h>
#include <AudioDriverDialog.h>
#include <GridUtils.h>

#include <stdio.h>
#include <math.h>
#include <qgraphicswidget.h>
#include <Windows.h>

#include <fstream>

#define SAMPLE_RATE         (44100)
#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (1)
#define BUFFERS             (1)

typedef float SAMPLE;

static int gNumNoInputs = 0;
int CMicrophone::BuffersInUse = 0;
SAMPLE** CMicrophone::mv_pAudioBuffers = nullptr;
std::mutex CMicrophone::AcquisitionGuard;

int CMicrophone::sv_nMicrophoneID = 0;

CMicrophone::CMicrophone(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pParent) :
IComponent(ac_Position, ac_pScene, ac_pParent)
{
  mv_sCompID = QString("Mic");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);

  mv_Terminal_1 = new Terminal(ac_pScene, this);
  mv_Terminal_2 = new Terminal(ac_pScene, this);

  mf_AddTerminal(mv_Terminal_1);
  mf_AddTerminal(mv_Terminal_2);

  mv_nImpedance = 10;

  mv_pAudioData = new SAMPLE[FRAMES_PER_BUFFER];
  
  mv_pAudioBuffers = new SAMPLE*[BUFFERS];
  for (int i = 0; i < BUFFERS; ++i)
  {
    mv_pAudioBuffers[i] = new SAMPLE[FRAMES_PER_BUFFER];
  }

  mv_bMaydenVoyage = true;
  mv_bIsStreamOpen = false;
  mv_OutputVoltage = 0.;
  mf_ConstructDialog();
  mv_dfCurrentOut = NAN;
  ++sv_nMicrophoneID;
}

void CMicrophone::mf_ConstructDialog()
{
  mv_nSelectedInputDeviceIndex = -1;
  
  mv_SettingsDialog = new AudioDriverDialog();
  Pa_Initialize();
  PaDeviceIndex ind = Pa_GetDeviceCount();

  QString DeviceInformation;
  QStringList DriverList;

  for (int i = 0; i < ind; i++)
  {
    const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
    QString temp;
    temp.sprintf("%s Latency: %f", info->name, info->defaultLowInputLatency);
    DriverList.append(temp);
  }
  qobject_cast<AudioDriverDialog*>(mv_SettingsDialog)->comboBox->addItems(DriverList);
}

static int CallBack(const void *inputBuffer, void *outputBuffer,
  unsigned long framesPerBuffer,
  const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags,
  void *userData)
{    
  if (CMicrophone::BuffersInUse == BUFFERS)
    return paContinue;
  CMicrophone::AcquisitionGuard.lock();
  SAMPLE *out = CMicrophone::mv_pAudioBuffers[CMicrophone::BuffersInUse];//(SAMPLE*)userData;
  const SAMPLE *in = (const SAMPLE*)inputBuffer;
  unsigned int i;
  (void)timeInfo; /* Prevent unused variable warnings. */
  (void)statusFlags;
  (void)outputBuffer;
  (void)userData;

  if (inputBuffer == NULL)
  {
    for (i = 0; i<framesPerBuffer; i++)
    {
      *out++ = 0;  /* left - silent */
     // *out++ = 0;  /* right - silent */
    }
    gNumNoInputs += 1;
  }
  else
  {
    
    for (i = 0; i<framesPerBuffer; i++)
    {
      *out++ = *in++;          /* left - clean */
     // *out++ = *in++;          /* right - clean */
    }
  }
  CMicrophone::BuffersInUse++;
  CMicrophone::AcquisitionGuard.unlock();
  return paContinue;
}



void CMicrophone::mf_SetupHardware()
{
  bool error = false;
  PaStreamParameters inputParameters;
  PaError err = 0;
  int inputChannelSelectors[1];
  PaAsioStreamInfo asioInputInfo;
  asioInputInfo.size = sizeof(PaAsioStreamInfo);
  asioInputInfo.hostApiType = paASIO;
  asioInputInfo.version = 1;
  asioInputInfo.flags = paAsioUseChannelSelectors;
  inputChannelSelectors[0] = 1; /* skip channel 0 and use the second (right) ASIO device channel */
  asioInputInfo.channelSelectors = inputChannelSelectors;
  inputParameters.hostApiSpecificStreamInfo = &asioInputInfo;

  
  if (err != paNoError)
    error = true;
 
  inputParameters.device = mv_nSelectedInputDeviceIndex != -1 ? mv_nSelectedInputDeviceIndex : Pa_GetDefaultInputDevice();
  if (inputParameters.device == paNoDevice) {
    error = true;
  }
  inputParameters.channelCount = 1;       /* mono input */
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
    &mv_pAudioInputStream,
    &inputParameters,
    NULL,
    SAMPLE_RATE,
    FRAMES_PER_BUFFER,
    0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
    CallBack,
    NULL);
  if (err != paNoError)
    error = true;

    err = Pa_StartStream(mv_pAudioInputStream);
    if (err != paNoError)
      error = true;
    else
      mv_bIsStreamOpen = true;
}

CMicrophone::~CMicrophone()
{
}

void CMicrophone::mf_PreDestroy()
{
  if (mv_bIsStreamOpen == true)
  {
    Pa_CloseStream(mv_pAudioInputStream);
    Pa_Terminate();
  }
}

QRectF CMicrophone::boundingRect() const
{
  return QRectF(QPointF(0, 0), QPointF(20, 40));
}

void CMicrophone::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  QLine lv_UpperConnector(10, 0, 10, 12);
  QLine lv_LowerConnector(10, 28, 10, 40);
  QLine lv_Style(0, 12, 0, 28);

  QPoint lv_CircleRef(10, 20);

  ac_pPainter->drawLine(lv_UpperConnector);
  ac_pPainter->drawEllipse(lv_CircleRef, 8, 8);
  ac_pPainter->drawLine(lv_LowerConnector);
  ac_pPainter->drawLine(lv_Style);

  mv_Terminal_1->setPos(9, 0);
  mv_Terminal_2->setPos(9, 40);

  mv_Terminal_1->paint(ac_pPainter, ac_pOption, ac_pParent, true);
  mv_Terminal_2->paint(ac_pPainter, ac_pOption, ac_pParent, true);
}

QString CMicrophone::mf_ToolTipGetType()
{
  return QString("Microphone ");
}

QString CMicrophone::mf_ToolTipGetValue()
{
  return QString(qobject_cast<AudioDriverDialog*>(mv_SettingsDialog)->comboBox->currentText());
}

QString CMicrophone::mf_ToolTipGetUnit()
{
  return QString("");
}

const int CMicrophone::mf_nGetInstanceNumber() const
{
  return sv_nMicrophoneID;
}

int CMicrophone::mf_nGetImpedance()
{
  return mv_nImpedance;
}


IComponent::DialogSettingsMap CMicrophone::mf_GetDialogSettingsMap()
{
  DialogSettingsMap lv_Variables;
  lv_Variables.insert(std::make_pair(QString("Impedance"), mv_nImpedance));

  return lv_Variables;
}

void CMicrophone::mf_SetDialogSettingsMap(IComponent::DialogSettingsMap ac_SettingsMap)
{
  mv_nImpedance = ac_SettingsMap[QString("Impedance")];
}

SAMPLE* CMicrophone::mf_dfGetVoltage(int& av_nSamplesAvailable)
{
  av_nSamplesAvailable = FRAMES_PER_BUFFER;
  SAMPLE * sample = new SAMPLE;
  *sample = mv_OutputVoltage;
  return sample;
}


void CMicrophone::mf_ShowSettingsDialog()
{ 
  if (nullptr != mv_SettingsDialog)
  {
    if (mv_SettingsDialog->exec())
    {
      bool lv_bReinitPA = (-1 != mv_nSelectedInputDeviceIndex);
      // Ok pressed, apply settings to component
      mv_nSelectedInputDeviceIndex = qobject_cast<AudioDriverDialog*>(mv_SettingsDialog)->comboBox->currentIndex() + 1;
      if (lv_bReinitPA)
      {
        Pa_CloseStream(mv_pAudioInputStream);
        mf_SetupHardware();
      }
    }
    else
    {
      // cancel
    }
  }
}

void CMicrophone::mf_Save(QJsonObject &json)
{
  json["name"] = "CMicrophone";
  json["positionX"] = this->pos().x();
  json["positionY"] = this->pos().y();
}

void CMicrophone::Process_(DspSignalBus &inputs, DspSignalBus &outputs)
{
  if (mv_bMaydenVoyage)
  {
    mf_SetupHardware();
    mv_bMaydenVoyage = false;
  }
  while (BuffersInUse == 0)
  {
    Sleep(1);
  }
  // Current
  double lv_dfMaxCurrent = NAN;
  bool lv_bHaveMaxCurrent = inputs.GetValue(mv_Ports[0].mv_sCurrent_IN, lv_dfMaxCurrent);
  if (lv_bHaveMaxCurrent && !isnan(lv_dfMaxCurrent)) {
    mv_dfCurrentOut = lv_dfMaxCurrent;
  }

  mv_OutputVoltage = *mv_pAudioBuffers[0];
  
  BuffersInUse = 0;
  
  outputs.SetValue(mv_Ports[1].mv_sVoltage_OUT, mv_OutputVoltage);
  outputs.SetValue(mv_Ports[1].mv_sCurrent_OUT, mv_dfCurrentOut);

  outputs.SetValue(mv_Ports[0].mv_sVoltage_OUT, 0.0);
  outputs.SetValue(mv_Ports[0].mv_sCurrent_OUT, mv_dfCurrentOut);
}