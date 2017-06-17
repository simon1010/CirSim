#include <IDispatchComponent.h>


int IDispatchComponent::sv_nCompNr = 0;
bool IDispatchComponent::mv_bCircuitStable = false;

IDispatchComponent::IDispatchComponent(const int ac_nPortCount)
  : mc_sCompID([] {sv_nCompNr++; return std::to_string(sv_nCompNr); }()), mv_nTickDuration(NAN)
{
  mv_bMaidenTrip = true;
  mv_Ports.resize(ac_nPortCount);

  // Connection ID format: <ID>_<ConnectionType>_<PortNr> (E.g. 1_0_II, first component, port 0, Current input )
  for (int i = 0; i < ac_nPortCount; ++i) {
    mv_Ports[i].mv_sCurrent_IN = ([this, i] { return mc_sCompID + "_" + std::to_string(i) + "_II"; }());
    AddInput_(mv_Ports[i].mv_sCurrent_IN); // Current in

    mv_Ports[i].mv_sCurrent_OUT = ([this, i] { return mc_sCompID + "_" + std::to_string(i) + "_IO"; }());
    AddOutput_(mv_Ports[i].mv_sCurrent_OUT); // Current source

    mv_Ports[i].mv_sVoltage_IN = ([this, i] { return mc_sCompID + "_" + std::to_string(i) + "_UI"; }());
    AddInput_(mv_Ports[i].mv_sVoltage_IN); // Voltage in

    mv_Ports[i].mv_sVoltage_OUT = ([this, i] { return mc_sCompID + "_" + std::to_string(i) + "_UO"; }());
    AddOutput_(mv_Ports[i].mv_sVoltage_OUT); // Voltage source
  }
}