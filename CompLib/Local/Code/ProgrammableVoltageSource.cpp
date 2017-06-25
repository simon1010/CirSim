#include "ProgrammableVoltageSource.h"
#include <qgraphicssceneevent.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QWidget>
#include <mutex>
#define _USE_MATH_DEFINES
#include <math.h>

const double CProgrammableVoltageSource::sc_dfDefaultMaxVoltage = 1.0;

int CProgrammableVoltageSource::sv_nPVSID = 0;

std::mutex g_samples_mutex;
std::list<SAMPLE> samples;
std::list<double> times;

CProgrammableVoltageSource::CProgrammableVoltageSource(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent) : IComponent(ac_Position, ac_pScene, ac_pPrent)
{
  mv_sCompID = QString("E");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);

  mv_Terminal_Plus = new Terminal(ac_pScene, this);
  mv_Terminal_Minus = new Terminal(ac_pScene, this);

  mf_AddTerminal(mv_Terminal_Plus);
  mf_AddTerminal(mv_Terminal_Minus);
  
  mf_SetMaxVoltage(sc_dfDefaultMaxVoltage);
  
  mv_dfFrequency = 50.;
  mv_dfPhaseShift = 0.;
  mv_dfDutyCycle = 0.5;
  mv_dfFreqT0 = 50.;
  mv_dfBias = 0.;

  // DSP
  mc_dfStartFreq = 10.;
  mc_dfStopFreq = 500.;
  mc_dfSweepDuration = 30.;

  mv_tElapsedTime = 0;
  mv_dfCurrentOut = NAN;
  mv_nIncrement = 1;
  mv_bIncrement = true;
  // DSP
  mv_Waveform = CProgrammableVoltageSource::Waveform::WF_AC;

  ++sv_nPVSID;
}


CProgrammableVoltageSource::~CProgrammableVoltageSource()
{
}

QRectF CProgrammableVoltageSource::boundingRect() const
{
  return QRectF(0, 0, 20, 40);
}

void CProgrammableVoltageSource::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  // TODO: update according to gridSize and cursor position -> ( gridSize, 0, gridSize, gridSize )  

  QLine lv_UpperConnector(10, 0, 10, 12);
  QLine lv_LowerConnector(10, 28, 10, 40);

  QPoint lv_CircleRef(10, 20);
  
  ac_pPainter->drawLine(lv_UpperConnector);
  ac_pPainter->drawEllipse(lv_CircleRef,8,8);
  ac_pPainter->drawLine(lv_LowerConnector);
 
  mv_Terminal_Plus->setPos(9, 0);
  mv_Terminal_Minus->setPos(9, 40);

  mv_Terminal_Plus->paint(ac_pPainter, ac_pOption, ac_pParent, true);
  mv_Terminal_Minus->paint(ac_pPainter, ac_pOption, ac_pParent, true);
}

void CProgrammableVoltageSource::mf_SetMaxVoltage(const double ac_dfVoltage)
{
  mv_dfMaxVoltage = ac_dfVoltage;
}

double CProgrammableVoltageSource::mf_dfGetMaxVoltage() const
{
  return mv_dfMaxVoltage;
}


QString CProgrammableVoltageSource::mf_ToolTipGetType()
{
  return QString("Programmable Voltage Source ");
}

QString CProgrammableVoltageSource::mf_ToolTipGetValue()
{
  char value[10];
  sprintf(value, "%.2f", mf_dfGetMaxVoltage());
  return QString(value);
}

QString CProgrammableVoltageSource::mf_ToolTipGetUnit()
{
  return QString(" V");
}

const int CProgrammableVoltageSource::mf_nGetInstanceNumber() const
{
  return sv_nPVSID;
}

double triangleFunc(double x)
{
  if (x < M_PI)
    return x*(2 / M_PI) - 1;
  return 1 - (x - M_PI)*(2 / M_PI);
}

SAMPLE* CProgrammableVoltageSource::mf_dfGetVoltage(int &av_nAvailableSamples, double ** av_pTimesVec)
{  
  //SAMPLE * sample = new SAMPLE;
  ////double w = 2 * M_PI*(sim.t - freqTimeZero)*frequency + phaseShift;
  //double t = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0; //SimulationUtils::SimulationTime.elapsed();
  //double w = 2 * M_PI*( t - mv_dfFreqT0)*mv_dfFrequency + mv_dfPhaseShift;
  //
  //  switch (mv_Waveform)
  //  {
  //  case WF_DC: *sample = mv_dfMaxVoltage + mv_dfBias; break;
  //  case WF_AC: *sample = sin(5 * t)*mv_dfMaxVoltage + mv_dfBias; break;
  //  case WF_SQUARE:
  //    *sample = mv_dfBias + (((int)w % (int)(2 * M_PI) > (2 * M_PI*mv_dfDutyCycle)) ?
  //      -mv_dfMaxVoltage : mv_dfMaxVoltage); break;
  //  case WF_TRIANGLE:
  //    *sample = mv_dfBias + triangleFunc((int)w % (int)(2 * M_PI))*mv_dfMaxVoltage; break;
  //  case WF_SAWTOOTH:
  //    *sample = mv_dfBias + ((int)w % (int)(2 * M_PI))*(mv_dfMaxVoltage / M_PI) - mv_dfMaxVoltage; break;
  //  case WF_PULSE:
  //    *sample = (((int)w % (int)(2 * M_PI)) < 1) ? mv_dfMaxVoltage + mv_dfBias : mv_dfBias; break;
  //  default: *sample = 0; break;
  //  }
  //  av_nAvailableSamples = 1;
  //  return sample;  

  std::lock_guard<std::mutex> guard(g_samples_mutex);
  av_nAvailableSamples = samples.size();//1;
  SAMPLE * sample = new SAMPLE[samples.size()];
  *av_pTimesVec = new double[samples.size()];
  copy(samples.begin(), samples.end(), sample);
  copy(times.begin(), times.end(), *av_pTimesVec);
  
  times.clear();
  samples.clear();
  return sample;
}

void CProgrammableVoltageSource::mf_Save(QJsonObject &json)
{
  json["name"] = "CProgrammableVoltageSource";
  json["positionX"] = this->pos().x();
  json["positionY"] = this->pos().y();
}

void CProgrammableVoltageSource::Process_(DspSignalBus &inputs, DspSignalBus &outputs)
{
  // Current
  double lv_dfMaxCurrent = NAN;
  bool lv_bHaveMaxCurrent = inputs.GetValue(mv_Ports[0].mv_sCurrent_IN, lv_dfMaxCurrent);
  if (lv_bHaveMaxCurrent && !isnan(lv_dfMaxCurrent)) {
    mv_dfCurrentOut = lv_dfMaxCurrent;
  }

  //194000;
  _super::Process_(inputs, outputs);

  bool useSweep = true;
  if (!useSweep)
    mv_tElapsedTime += 80000;//mv_nTickDuration;

  mv_dfElapsedTime += mv_nTickDuration;

  mv_VoltageOut = useSweep ? mf_dfGetSweep(194000)//mv_nTickDuration)//
    : mv_dfMaxVoltage * sin(2 * M_PI * 100 * mv_tElapsedTime * pow(10, -9));
  
  {
    std::lock_guard<std::mutex> guard(g_samples_mutex);
    samples.push_back(mv_VoltageOut);
    times.push_back(mv_dfElapsedTime * pow(10, -9));
  }

  outputs.SetValue(mv_Ports[1].mv_sVoltage_OUT, mv_VoltageOut);
  outputs.SetValue(mv_Ports[1].mv_sCurrent_OUT, mv_dfCurrentOut);

  outputs.SetValue(mv_Ports[0].mv_sVoltage_OUT, 0.0);
  outputs.SetValue(mv_Ports[0].mv_sCurrent_OUT, mv_dfCurrentOut);
}

double CProgrammableVoltageSource::mf_dfGetSweep(const int64_t ac_nTickDuration)
{
  // Handle sweep up and then down smoothly
  if (mv_bIncrement && mv_tElapsedTime >= (mc_dfSweepDuration / 2)) {
    mv_bIncrement = false;
    mv_nIncrement = -1;
  }
  else if (!mv_bIncrement && mv_tElapsedTime <= 0) {
    mv_bIncrement = true;
    mv_nIncrement = 1;
  }

  mv_tElapsedTime += ac_nTickDuration * pow(10, -9) * mv_nIncrement; // from ns to s => may produce rounding error
  double delta = mv_tElapsedTime / mc_dfSweepDuration;
  double t = mc_dfSweepDuration * delta;

  double phase = 2 * M_PI * t * (mc_dfStartFreq + (mc_dfStopFreq - mc_dfStartFreq) * delta / 2);
  while (phase > 2 * M_PI) phase = fmod(phase, (2 * M_PI));

  return mv_dfMaxVoltage * sin(phase);
}

//
//IComponent::DialogSettingsMap CProgrammableVoltageSource::mf_GetDialogSettingsMap()
//{
//  DialogSettingsMap lv_Variables;
//  lv_Variables.insert(std::make_pair(QString("Voltage"), mf_dfGetVoltage()));
//
//  return lv_Variables;
//}
//
//void CProgrammableVoltageSource::mf_SetDialogSettingsMap(IComponent::DialogSettingsMap ac_NewSettings)
//{
//  mf_SetVoltage(ac_NewSettings[QString("Voltage")]);
//}