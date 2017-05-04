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
#define FRAMES_PER_BUFFER   (64)
#define BUFFERS             (40000)

typedef float SAMPLE;

static int gNumNoInputs = 0;
int CMicrophone::BuffersInUse = 0;
SAMPLE** CMicrophone::mv_pAudioBuffers = nullptr;
std::mutex CMicrophone::AcquisitionGuard;

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
  mf_ConstructDialog();
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
  return QRectF(QPointF(0, 0), QPointF(22, 22));
}

void CMicrophone::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  // TODO: update according to gridSize and cursor position -> ( gridSize, 0, gridSize, gridSize )  

  QLine lv_xLeftLeg(QPoint(0, 10), QPoint(3, 10));
  QLine lv_xRightLeg(QPoint(17, 10), QPoint(20, 10));

  ac_pPainter->drawEllipse(QPointF(10,10),5,5);
  ac_pPainter->drawLine(4, 5, 16, 5);
  ac_pPainter->drawLine(lv_xLeftLeg);
  ac_pPainter->drawLine(lv_xRightLeg);

  mv_Terminal_1->setPos(QPoint(-1, 10));
  mv_Terminal_2->setPos(QPoint(20, 10));

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
  //Pa_ReadStream(mv_pAudioInputStream, mv_pAudioData, FRAMES_PER_BUFFER);
  //SAMPLE *VoltageVector = (SAMPLE*)mv_pAudioData;
  //av_nSamplesAvailable = FRAMES_PER_BUFFER;
 // AcquisitionGuard.lock();
  if (mv_bMaydenVoyage)
  {
    mf_SetupHardware();
    mv_bMaydenVoyage = false;
  }
  while (BuffersInUse == 0)
  {
    Sleep(1);
  }
  //SAMPLE * VoltageVector = new SAMPLE[BuffersInUse * FRAMES_PER_BUFFER];
  //
  //int ptr = 0;
  //int i = 0;
  ////AcquisitionGuard.lock();
  //while (BuffersInUse > 0)
  //{
  //  memcpy(&VoltageVector[ptr], mv_pAudioBuffers[i], FRAMES_PER_BUFFER*sizeof(float));
  //  i++;
  //  ptr += FRAMES_PER_BUFFER;
  //  BuffersInUse--;
  //}
  //AcquisitionGuard.unlock();
  BuffersInUse = 0;
  av_nSamplesAvailable = FRAMES_PER_BUFFER;
  return mv_pAudioBuffers[0];
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

