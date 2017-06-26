#include "Resistor.h"
#include <mutex>
// Used to identify each resistor in the classic notation: e.g. R1, R2, ... Rn 
int CResistor::sv_nResistorID = 0;

std::mutex g_res_mutex;
std::list<SAMPLE> Res_samples;
std::list<double> Res_times;

CResistor::CResistor(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pParent) : 
IComponent(ac_Position, ac_pScene, ac_pParent)
{
  mv_sCompID = QString("R");
  mv_sCompName = mv_sCompID + QString(sv_nCompNumber);
  
  mv_Terminal_1 = new Terminal(ac_pScene, this);
  mv_Terminal_2 = new Terminal(ac_pScene, this);

  mf_AddTerminal(mv_Terminal_1);
  mf_AddTerminal(mv_Terminal_2);

  mv_nResistance = 50;
  mv_dfVoltageAcrossResistor = NAN;
  mv_dfCurrent = NAN;
  // Simple instance counter for ID
  ++sv_nResistorID;
}

CResistor::~CResistor()
{
}

QRectF CResistor::boundingRect() const
{
  return QRectF(QPointF(-2,0),QPointF(22,22));
}

void CResistor::mf_DerivedPaint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
  // TODO: update according to gridSize and cursor position -> ( gridSize, 0, gridSize, gridSize )  
 
  QRect lv_ResistorBody(QPoint(3, 7), QPoint(16, 13));
  QLine lv_xLeftLeg(QPoint(0,10), QPoint(3,10));
  QLine lv_xRightLeg(QPoint(17, 10), QPoint(20, 10));

  ac_pPainter->drawRect(lv_ResistorBody);
  ac_pPainter->drawLine(lv_xLeftLeg);
  ac_pPainter->drawLine(lv_xRightLeg);
  
  mv_Terminal_1->setPos(QPoint(-1, 10));
  mv_Terminal_2->setPos(QPoint(20, 10));

  mv_Terminal_1->paint(ac_pPainter, ac_pOption, ac_pParent, true);
  mv_Terminal_2->paint(ac_pPainter, ac_pOption, ac_pParent, true);

}

QString CResistor::mf_ToolTipGetType()
{
  return QString("Resistor ");
}

QString CResistor::mf_ToolTipGetValue()
{
  char value[10];
  sprintf(value, "%d", mv_nResistance);
  return QString(value);
}

QString CResistor::mf_ToolTipGetUnit()
{
  return QString(" OHM");
}


int CResistor::mf_nGetResistance()
{
  return mv_nResistance;
}


IComponent::DialogSettingsMap CResistor::mf_GetDialogSettingsMap()
{
  DialogSettingsMap lv_Variables;
  lv_Variables.insert(std::make_pair(QString("Resistance"), mv_nResistance));

  return lv_Variables;
}

void CResistor::mf_SetDialogSettingsMap(IComponent::DialogSettingsMap ac_SettingsMap)
{
  mv_nResistance = ac_SettingsMap[QString("Resistance")];
}

double CResistor::mf_dfGetVoltage()
{
  return mv_dfVoltageAcrossResistor;
}

SAMPLE* CResistor::mf_dfGetVoltage(int &av_nAvailableSamples, double**  av_pTimesVec)
{
  std::lock_guard<std::mutex> guard(g_res_mutex);
  av_nAvailableSamples = Res_samples.size();
  SAMPLE * sample = new SAMPLE[Res_samples.size()];
  *av_pTimesVec = new double[Res_samples.size()];
  copy(Res_samples.begin(), Res_samples.end(), sample);
  copy(Res_times.begin(), Res_times.end(), *av_pTimesVec);
  Res_samples.clear();
  Res_times.clear();
  return sample;
}

const int CResistor::mf_nGetInstanceNumber() const
{
  return sv_nResistorID;
}


void CResistor::mf_Save(QJsonObject &json)
{
  json["name"] = "CResistor";
  json["positionX"] = this->pos().x();
  json["positionY"] = this->pos().y();
  json["resistance"] = mf_nGetResistance();
}

void CResistor::mf_Load(QJsonObject & json)
{
  mv_nResistance = json["resistance"].toDouble();
}

void CResistor::Process_(DspSignalBus &inputs, DspSignalBus &outputs)
{

  //ResLog.open(sc_ResFileName + mc_sCompID + ".csv", std::ios_base::out | std::ios_base::app);

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

  // only do calculations if lv_P0_VoltageIn && lv_P1_VoltageIn are different from their previous values

  if (!isnan(lv_P0_VoltageIn) && !isnan(lv_P1_VoltageIn)) {
    mv_dfVoltageAcrossResistor = lv_P0_VoltageIn - lv_P1_VoltageIn;
    mv_dfCurrent = mv_dfVoltageAcrossResistor / mv_nResistance;

    double lv_dfEquivResistance = NAN;
    double lc_dfDividerVoltage = NAN;
    if (lv_P1_CurrentIn != lv_P0_CurrentIn && !mv_bCircuitStable) {
      if (!isnan(lv_P0_CurrentIn)) {
        lv_dfEquivResistance = lv_P0_VoltageIn / lv_P0_CurrentIn;

        // apply the voltage divider rule
        lc_dfDividerVoltage = (lv_P0_VoltageIn * mv_nResistance) / (mv_nResistance + lv_dfEquivResistance);
        mv_bCircuitStable = true;
      }
      else if (!isnan(lv_P1_CurrentIn)) {
        lv_dfEquivResistance = lv_P1_VoltageIn / lv_P1_CurrentIn;

        // apply the voltage divider rule
        lc_dfDividerVoltage = (lv_P1_VoltageIn * mv_nResistance) / (mv_nResistance + lv_dfEquivResistance);
        mv_bCircuitStable = true;
      }
    }
    else if (mv_bCircuitStable) {
      mv_bCircuitStable = false;
    }
    const double lc_dfAdjustedCurrent = (lc_dfDividerVoltage / mv_nResistance);

    mv_dfVoltageAcrossResistor = !isnan(lc_dfDividerVoltage) ? lc_dfDividerVoltage : mv_dfVoltageAcrossResistor;
    mv_dfCurrent = !isnan(lc_dfAdjustedCurrent) ? lc_dfAdjustedCurrent : mv_dfCurrent;

    lv_P0_VoltageOut = mv_dfVoltageAcrossResistor;
    lv_P1_VoltageOut = mv_dfVoltageAcrossResistor;
    lv_P0_CurrentOut = mv_dfCurrent;
    lv_P1_CurrentOut = mv_dfCurrent;

    //ResLog << mv_dfVoltageAcrossResistor << "," << mv_dfCurrent << (mv_bCircuitStable ? ",true" : ",false") << std::endl;
    //ResLog.close();
  }
  else {
    // make transparent
    lv_P0_VoltageOut = lv_P1_VoltageIn;
    lv_P0_CurrentOut = lv_P1_CurrentIn;
    lv_P1_VoltageOut = lv_P0_VoltageIn;
    lv_P1_CurrentOut = lv_P0_CurrentIn;
  }

  {
    __super::Process_(inputs, outputs);
    std::lock_guard<std::mutex> guard(g_res_mutex);
    Res_samples.push_back(lv_P1_VoltageOut);
    mv_dfElapsedTime += mv_nTickDuration;
    Res_times.push_back(mv_dfElapsedTime * pow(10, -9));
  }


  outputs.SetValue(mv_Ports[0].mv_sVoltage_OUT, lv_P0_VoltageOut);
  outputs.SetValue(mv_Ports[0].mv_sCurrent_OUT, lv_P0_CurrentOut);
  outputs.SetValue(mv_Ports[1].mv_sVoltage_OUT, lv_P1_VoltageOut);
  outputs.SetValue(mv_Ports[1].mv_sCurrent_OUT, lv_P1_CurrentOut);
}