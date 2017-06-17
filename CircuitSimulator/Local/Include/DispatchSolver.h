#pragma once
#include <DSPatch.h>
#include <Edge.h>
#include <Node.h>

namespace Solver {

  template <class fromComponent, class toComponent>
  void PortConnect(DspCircuit& circuit, const fromComponent& Comp_1, const int Comp_1_Port, const toComponent& Comp_2, const int Comp_2_Port)
  {
    assert(circuit.ConnectOutToIn(Comp_1, Comp_1.mv_Ports[Comp_1_Port].mv_sCurrent_OUT, Comp_2, Comp_2.mv_Ports[Comp_2_Port].mv_sCurrent_IN));
    assert(circuit.ConnectOutToIn(Comp_1, Comp_1.mv_Ports[Comp_1_Port].mv_sVoltage_OUT, Comp_2, Comp_2.mv_Ports[Comp_2_Port].mv_sVoltage_IN));
    assert(circuit.ConnectOutToIn(Comp_2, Comp_2.mv_Ports[Comp_2_Port].mv_sVoltage_OUT, Comp_1, Comp_1.mv_Ports[Comp_1_Port].mv_sVoltage_IN));
    assert(circuit.ConnectOutToIn(Comp_2, Comp_2.mv_Ports[Comp_2_Port].mv_sCurrent_OUT, Comp_1, Comp_1.mv_Ports[Comp_1_Port].mv_sCurrent_IN));
  }


  class CDispatchSolver
  {
  public:
    CDispatchSolver(const EDGE_VEC& ac_AllEdges, const NODE_VEC& ac_AllNodes);

    static const DspCircuit theCircuit;
  };

}
