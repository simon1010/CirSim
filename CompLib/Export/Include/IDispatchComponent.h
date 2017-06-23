
#ifndef IDISPATCHCOMPONENT
#define IDISPATCHCOMPONENT

#pragma once

#include <DSPatch.h>
#include <iostream>
#include <string>
#include <cmath>

#ifdef max
#undef max
#endif // max

#include <limits>
#include <chrono>

using namespace std;

struct Port
{
  string mv_sCurrent_IN;
  string mv_sCurrent_OUT;
  string mv_sVoltage_IN;
  string mv_sVoltage_OUT;
};

typedef std::vector<Port> t_Ports;

class IDispatchComponent : public DspComponent {
protected:
  IDispatchComponent(const int ac_nPortCount = 2);

  static bool mv_bCircuitStable;

  virtual inline void Process_(DspSignalBus&, DspSignalBus&) override {
   if(!mv_bMaidenTrip)
    {
      mv_nTickDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - mv_LastTime).count();
    }
    else
    {
      mv_bMaidenTrip = false;
    }
    mv_LastTime = std::chrono::steady_clock::now();
  }

  const string mc_sCompID;
  static int sv_nCompNr;
  int64_t mv_nTickDuration;
  std::chrono::steady_clock::time_point mv_LastTime;
  bool mv_bMaidenTrip;
  // convenience
public:
  const string& mv_szGetComponentID() const { return mc_sCompID; }
  t_Ports mv_Ports;
};

#endif // !IDISPATCHCOMPONENT
