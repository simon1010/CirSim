#include "ProgrammableVoltageSource.h"
#include <qgraphicssceneevent.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QWidget>

#define _USE_MATH_DEFINES
#include <math.h>

const double CProgrammableVoltageSource::sc_dfDefaultMaxVoltage = 5.0;

int CProgrammableVoltageSource::sv_nPVSID = 0;

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

SAMPLE* CProgrammableVoltageSource::mf_dfGetVoltage(int &av_nAvailableSamples)
{  
  SAMPLE * sample = new SAMPLE;
  //double w = 2 * M_PI*(sim.t - freqTimeZero)*frequency + phaseShift;
  double t = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0; //SimulationUtils::SimulationTime.elapsed();
  double w = 2 * M_PI*( t - mv_dfFreqT0)*mv_dfFrequency + mv_dfPhaseShift;

    switch (mv_Waveform)
    {
    case WF_DC: *sample = mv_dfMaxVoltage + mv_dfBias; break;
    case WF_AC: *sample = sin(5 * t)*mv_dfMaxVoltage + mv_dfBias; break;
    case WF_SQUARE:
      *sample = mv_dfBias + (((int)w % (int)(2 * M_PI) > (2 * M_PI*mv_dfDutyCycle)) ?
        -mv_dfMaxVoltage : mv_dfMaxVoltage); break;
    case WF_TRIANGLE:
      *sample = mv_dfBias + triangleFunc((int)w % (int)(2 * M_PI))*mv_dfMaxVoltage; break;
    case WF_SAWTOOTH:
      *sample = mv_dfBias + ((int)w % (int)(2 * M_PI))*(mv_dfMaxVoltage / M_PI) - mv_dfMaxVoltage; break;
    case WF_PULSE:
      *sample = (((int)w % (int)(2 * M_PI)) < 1) ? mv_dfMaxVoltage + mv_dfBias : mv_dfBias; break;
    default: *sample = 0; break;
    }
    av_nAvailableSamples = 1;
    return sample;  
}

void CProgrammableVoltageSource::mf_Save(QJsonObject &json)
{
  json["name"] = "CProgrammableVoltageSource";
  json["positionX"] = this->pos().x();
  json["positionY"] = this->pos().y();
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