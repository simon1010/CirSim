#include "Capacitor.h"
#include <qgraphicssceneevent.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QWidget>

#include <mutex>

const double CCapacitor::sc_dfDefaultCapacitance = 500; //microFarads

int CCapacitor::sv_nCapacitorID = 0;

std::mutex g_cap_mutex;
std::list<SAMPLE> Cap_samples;
std::list<double> Cap_times;

CCapacitor::CCapacitor(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent) : IComponent(ac_Position, ac_pScene, ac_pPrent)
{
  mv_sCompID = QString("C");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);

  mv_Terminal_1 = new Terminal(ac_pScene, this);
  mv_Terminal_2 = new Terminal(ac_pScene, this);

  mf_AddTerminal(mv_Terminal_1);
  mf_AddTerminal(mv_Terminal_2);

  mf_SetCapacitance(sc_dfDefaultCapacitance);

  ++sv_nCapacitorID;

  mv_dfCapVoltage = NAN;
  mv_dfCurrent = NAN;
  mv_EquivalentResistance = NAN;
  mv_dfPreviousOutputVoltage = 0.;
  mv_dfoutputVoltage = 0.;

}

CCapacitor::~CCapacitor()
{
}

QRectF CCapacitor::boundingRect() const
{
  return QRectF(0, 0, 20, 20);
}

void CCapacitor::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  // TODO: update according to gridSize and cursor position -> ( gridSize, 0, gridSize, gridSize )  

  QLine lv_UpperConnector(10, 0, 10, 10);
  QLine lv_LowerConnector(10, 12, 10, 20);

  QLine lv_PositiveElectrode(5, 10, 15, 10); // longer
  QLine lv_NegativeElectrode(5, 12, 15, 12);

  ac_pPainter->drawLine(lv_UpperConnector);
  ac_pPainter->drawLine(lv_PositiveElectrode);
  ac_pPainter->drawLine(lv_LowerConnector);
  ac_pPainter->drawLine(lv_NegativeElectrode);

  mv_Terminal_1->setPos(9, 0);
  mv_Terminal_2->setPos(9, 20);

  mv_Terminal_1->paint(ac_pPainter, ac_pOption, ac_pParent, true);
  mv_Terminal_2->paint(ac_pPainter, ac_pOption, ac_pParent, true);
}

void CCapacitor::mf_SetCapacitance(const double ac_dfCapacitance)
{
  mv_dfCapacitance = ac_dfCapacitance;
}

double CCapacitor::mf_dfGetCapacitance() const
{
  return mv_dfCapacitance;
}

QString CCapacitor::mf_ToolTipGetType()
{
  return QString("Capacitor ");
}

QString CCapacitor::mf_ToolTipGetValue()
{
  char value[10];
  sprintf(value, "%.3f", mv_dfCapacitance);
  return QString(value);
}

QString CCapacitor::mf_ToolTipGetUnit()
{
  return QString(" uF");
}

const int CCapacitor::mf_nGetInstanceNumber() const
{
  return sv_nCapacitorID;
}

IComponent::DialogSettingsMap CCapacitor::mf_GetDialogSettingsMap()
{
  DialogSettingsMap lv_Variables;
  lv_Variables.insert(std::make_pair(QString("Capacitance"), mv_dfCapacitance));

  return lv_Variables;
}

void CCapacitor::mf_SetDialogSettingsMap(IComponent::DialogSettingsMap ac_SettingsMap)
{
  mv_dfCapacitance = ac_SettingsMap[QString("Capacitance")];
}

double CCapacitor::mf_dfGetVoltage()
{
  return mv_dfoutputVoltage;
}

SAMPLE* CCapacitor::mf_dfGetVoltage(int &av_nAvailableSamples, double**  av_pTimesVec)
{
  std::lock_guard<std::mutex> guard(g_cap_mutex);
  av_nAvailableSamples = Cap_samples.size();
  SAMPLE * sample = new SAMPLE[Cap_samples.size()];
  *av_pTimesVec = new double[Cap_samples.size()];
  copy(Cap_samples.begin(), Cap_samples.end(), sample);
  copy(Cap_times.begin(), Cap_times.end(), *av_pTimesVec);
  Cap_samples.clear();
  Cap_times.clear();
  return sample;
}

void CCapacitor::mf_Save(QJsonObject &json)
{
  json["name"] = "CCapacitor";
  json["positionX"] = this->pos().x();
  json["positionY"] = this->pos().y();
  json["capcitance"] = mf_dfGetCapacitance();
}

void CCapacitor::mf_Load(QJsonObject & json)
{
  mv_dfCapacitance = json["capcitance"].toDouble();
}

void CCapacitor::Process_(DspSignalBus &inputs, DspSignalBus &outputs)
{
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

  if (!isnan(lv_P0_VoltageIn) && !isnan(lv_P1_VoltageIn) && !isnan(lv_P0_CurrentIn))
  {
    // we should not divide by 0.
    if (lv_P0_CurrentIn != 0.)
      mv_EquivalentResistance = lv_P0_VoltageIn / lv_P0_CurrentIn;
    else
      mv_EquivalentResistance = 0.;

    mv_dfCapVoltage = lv_P0_VoltageIn - lv_P1_VoltageIn; // Vth
    mf_DoCapacitor(lv_P0_VoltageIn, lv_P1_VoltageIn, inputs, outputs);

    lv_P0_VoltageOut = mv_dfoutputVoltage;
    lv_P0_CurrentOut = mv_dfCurrent;
    lv_P1_VoltageOut = mv_dfoutputVoltage;
    lv_P1_CurrentOut = mv_dfCurrent;
  }
  else
  {
    lv_P0_VoltageOut = lv_P1_VoltageIn;
    lv_P0_CurrentOut = lv_P1_CurrentIn;
    lv_P1_VoltageOut = lv_P0_VoltageIn;
    lv_P1_CurrentOut = lv_P0_CurrentIn;
  }

  {
    std::lock_guard<std::mutex> guard(g_cap_mutex);
    Cap_samples.push_back(mv_dfoutputVoltage);
    mv_dfElapsedTime += mv_nTickDuration;
    Cap_times.push_back(mv_dfElapsedTime * pow(10, -9));
  }

  outputs.SetValue(mv_Ports[0].mv_sVoltage_OUT, lv_P0_VoltageOut);
  outputs.SetValue(mv_Ports[0].mv_sCurrent_OUT, lv_P0_CurrentOut);
  outputs.SetValue(mv_Ports[1].mv_sVoltage_OUT, lv_P1_VoltageOut);
  outputs.SetValue(mv_Ports[1].mv_sCurrent_OUT, lv_P1_CurrentOut);
}

void CCapacitor::mf_DoCapacitor(const double ac_P0_VoltageIn, const double ac_P1_VoltageIn, DspSignalBus &inputs, DspSignalBus &outputs)
{
  _super::Process_(inputs, outputs);
  mv_dfResistance = mv_nTickDuration / mv_dfCapacitance; // dt / C = Rth
  if (!isnan(mv_dfCapVoltage) && !mv_bCircuitStable)
  {
    mv_bCircuitStable = true;
    // first trip
    if (isnan(mv_dfPreviousOutputVoltage))
    {
      mv_dfPreviousOutputVoltage = 0.;//mv_dfResistance / (mv_dfResistance + mv_EquivalentResistance) * mv_dfCapVoltage; // may not be correct, better 0. ?
    }
    mv_dfoutputVoltage = ((mv_dfResistance / (mv_dfResistance + mv_EquivalentResistance)) * (mv_dfCapVoltage - mv_dfPreviousOutputVoltage)) + mv_dfPreviousOutputVoltage;
    mv_dfPreviousOutputVoltage = mv_dfoutputVoltage;
    mv_dfCurrent = -mv_dfCapVoltage / mv_dfResistance;//mv_dfoutputVoltage / (mv_dfResistance + mv_EquivalentResistance);
  }
  else if (mv_bCircuitStable)
  {
    mv_bCircuitStable = false;
  }
 // CapLog << /*mv_dfResistance * mv_dfCurrent*/mv_dfoutputVoltage << "," << mv_dfCurrent << std::endl;
}