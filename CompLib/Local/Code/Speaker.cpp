#include <Speaker.h>
#include <mutex>
#define SAMPLE_RATE         (44100)
#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (128)
#define FLOAT_BUFFER_SIZE   (512)
double CSpeaker::mv_OutputSample = 0.;
#include <fstream>
static const char* sc_CapFileName = "c:\\CIRSIM_LOGGING\\SpeakerLog.csv";
std::ofstream CapLog;

std::mutex g_spk_mutex;
typedef std::vector<float> t_FloatBuffer;

t_FloatBuffer Spk_samples;
std::list<double> Spk_times;

CSpeaker::CSpeaker(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pParent):
  IComponent(ac_Position, ac_pScene, ac_pParent)
{
  CapLog.open(sc_CapFileName, std::ios_base::out);
  mv_sCompID = QString("Mic");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);

  mv_Terminal_1 = new Terminal(ac_pScene, this);
  mv_Terminal_2 = new Terminal(ac_pScene, this);

  mf_AddTerminal(mv_Terminal_1);
  mf_AddTerminal(mv_Terminal_2);

  Spk_samples.resize(FLOAT_BUFFER_SIZE);
  for (int i = 0; i < Spk_samples.size(); i++)
  {
    Spk_samples[i] = 0.;
  }
}

CSpeaker::~CSpeaker()
{
  mf_PreDestroy();
}

void CSpeaker::mf_SetupHardware()
{
  PaStreamParameters outputParameters;
  
  PaError err;

  err = Pa_Initialize();
  if (err != paNoError) {
    Pa_Terminate();
    _ASSERT(!"not Initialized");
  }

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) 
  {
    Pa_Terminate();
    _ASSERT(!"not Initialized");
  }

  outputParameters.channelCount = 2;       /* stereo output */
  outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
  outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
    &stream,
    NULL, /* no input */
    &outputParameters,
    SAMPLE_RATE,
    FRAMES_PER_BUFFER,
    NULL,      /* we won't output out of range samples so don't bother clipping them */
    &CSpeaker::SpeakerCallBack,
    nullptr);

  err = Pa_StartStream(stream);

  if (err != paNoError)
  {
    Pa_Terminate();
  }
}

void CSpeaker::mf_PreDestroy()
{
  if (!mv_bMaydenVoyage)
  {
    PaError err;
    err = Pa_StopStream(stream);
    if (err != paNoError) _ASSERT(!"Failed Stop");

    err = Pa_CloseStream(stream);
    if (err != paNoError) _ASSERT(!"Failed Close");

    Pa_Terminate();
  }
}

QRectF CSpeaker::boundingRect() const
{
  return QRectF(QPointF(0, 0), QPointF(20, 40));
}

void CSpeaker::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  QLine lv_UpperConnector(10, 0, 10, 12);
  QLine lv_LowerConnector(10, 28, 10, 40);
  QRect lv_Rectangle(7, 12, 6, 16);
  
  QVector<QPoint> PointsPolygon(4);
  PointsPolygon[0] = QPoint(18, 12);
  PointsPolygon[1] = QPoint(18, 28);
  PointsPolygon[2] = QPoint(13, 23);
  PointsPolygon[3] = QPoint(13, 18);
  QPolygon Polygon(PointsPolygon);

  ac_pPainter->drawLine(lv_UpperConnector);
  ac_pPainter->drawRect(lv_Rectangle);
  ac_pPainter->drawLine(lv_LowerConnector);
  ac_pPainter->drawPolygon(Polygon);

  mv_Terminal_1->setPos(9, 0);
  mv_Terminal_2->setPos(9, 40);

  mv_Terminal_1->paint(ac_pPainter, ac_pOption, ac_pParent, true);
  mv_Terminal_2->paint(ac_pPainter, ac_pOption, ac_pParent, true);
}


QString CSpeaker::mf_ToolTipGetType()
{
  return QString("Speaker ");
}

QString CSpeaker::mf_ToolTipGetValue()
{
  return QString("Default Output Device");
}

QString CSpeaker::mf_ToolTipGetUnit()
{
  return QString("");
}


const int CSpeaker::mf_nGetInstanceNumber() const
{
  return 1; // todo
}

SAMPLE* CSpeaker::mf_dfGetVoltage(int& av_nSamplesAvailable, double ** av_pTimesVec)
{
  av_nSamplesAvailable = FRAMES_PER_BUFFER;
  SAMPLE * sample = new SAMPLE;
  *sample = mv_OutputSample;
  return sample;
}

void CSpeaker::mf_Save(QJsonObject &json)
{
  json["name"] = "CSpeaker";
  json["positionX"] = this->pos().x();
  json["positionY"] = this->pos().y();
}

int iter = { 0 };

void CSpeaker::Process_(DspSignalBus &inputs, DspSignalBus &outputs)
{
  if (mv_bMaydenVoyage)
  {
    mf_SetupHardware();
    mv_bMaydenVoyage = false;
  }

  //__super::Process_(inputs, outputs);
  //CapLog << mv_nTickDuration  << "," << std::endl;

  // inputs
  double lv_P0_VoltageIn = NAN;
  double lv_P0_CurrentIn = NAN;
  double lv_P1_VoltageIn = NAN;
  double lv_P1_CurrentIn = NAN;

  // outputs
  double lv_P0_VoltageOut = NAN;
  double lv_P0_CurrentOut = NAN;
  double lv_P1_VoltageOut = NAN;
  double lv_P1_CurrentOut = NAN;
  double lv_Reader = NAN;

  // Read all inputs
  if (inputs.GetValue(mv_Ports[0].mv_sVoltage_IN, lv_Reader)) lv_P0_VoltageIn = lv_Reader;
  if (inputs.GetValue(mv_Ports[0].mv_sCurrent_IN, lv_Reader)) lv_P0_CurrentIn = lv_Reader;
  if (inputs.GetValue(mv_Ports[1].mv_sVoltage_IN, lv_Reader)) lv_P1_VoltageIn = lv_Reader;
  if (inputs.GetValue(mv_Ports[1].mv_sCurrent_IN, lv_Reader)) lv_P1_CurrentIn = lv_Reader;

  // make some noise
  {
    std::lock_guard<std::mutex> guard(g_spk_mutex);
    
    if (iter < FLOAT_BUFFER_SIZE)
    {
      Spk_samples[iter] = (float)lv_P0_VoltageIn;
      iter++;
    }
    else
    {
      Spk_samples[0] = (float)lv_P0_VoltageIn;
      iter = 1;
    }
  }
  // make transparent
  lv_P0_VoltageOut = lv_P1_VoltageIn;
  lv_P0_CurrentOut = lv_P1_CurrentIn;
  lv_P1_VoltageOut = lv_P0_VoltageIn;
  lv_P1_CurrentOut = lv_P0_CurrentIn;

  outputs.SetValue(mv_Ports[0].mv_sVoltage_OUT, lv_P0_VoltageOut);
  outputs.SetValue(mv_Ports[0].mv_sCurrent_OUT, lv_P0_CurrentOut);
  outputs.SetValue(mv_Ports[1].mv_sVoltage_OUT, lv_P1_VoltageOut);
  outputs.SetValue(mv_Ports[1].mv_sCurrent_OUT, lv_P1_CurrentOut);
}

int callBackIter = 0;

int CSpeaker::SpeakerCallBack(const void *inputBuffer, void *outputBuffer,
  unsigned long framesPerBuffer,
  const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags,
  void *userData)
{
  float *out = (float*)outputBuffer;
  unsigned long i;

  (void)timeInfo; /* Prevent unused variable warnings. */
  (void)statusFlags;
  (void)inputBuffer;
  
  {
    std::lock_guard<std::mutex> guard(g_spk_mutex);
  
      if (framesPerBuffer + callBackIter < FLOAT_BUFFER_SIZE)
      {
        for (i = callBackIter; i < framesPerBuffer + callBackIter; i++)
        {
          *out++ = Spk_samples[i];
          *out++ = Spk_samples[i];
          CapLog << Spk_samples[i] << endl;
        }
      }
      else
      {
        int diff = (framesPerBuffer + callBackIter) - FLOAT_BUFFER_SIZE;
        for (int i = callBackIter; i < FLOAT_BUFFER_SIZE; i++)
        {
          *out++ = Spk_samples[i];
          *out++ = Spk_samples[i];
        }
        for (int i = 0; i < diff; i++)
        {
          *out++ = Spk_samples[i];
          *out++ = Spk_samples[i];
        }
      }

      if (callBackIter + framesPerBuffer < FLOAT_BUFFER_SIZE)
        callBackIter += framesPerBuffer;
      else
        callBackIter = 0;

  }
  
  return paContinue;
}