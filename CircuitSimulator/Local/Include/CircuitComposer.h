#pragma once
#ifndef CIRCUITCOMPOSER_H
#define CIRCUITCOMPOSER_H

#include <QtWidgets\qgraphicsitem.h>
#include <Terminal.h>
#include <Edge.h>
#include <Node.h>
#include <DSPatch.h>
#include <assert.h>

namespace Solver {

  template <class fromComponent, class toComponent>
  void PortConnect(DspCircuit& circuit, const fromComponent& Comp_1, const int Comp_1_Port, const toComponent& Comp_2, const int Comp_2_Port)
  {
    assert(circuit.ConnectOutToIn(Comp_1, Comp_1.mv_Ports[Comp_1_Port].mv_sCurrent_OUT, Comp_2, Comp_2.mv_Ports[Comp_2_Port].mv_sCurrent_IN));
    assert(circuit.ConnectOutToIn(Comp_1, Comp_1.mv_Ports[Comp_1_Port].mv_sVoltage_OUT, Comp_2, Comp_2.mv_Ports[Comp_2_Port].mv_sVoltage_IN));
    assert(circuit.ConnectOutToIn(Comp_2, Comp_2.mv_Ports[Comp_2_Port].mv_sVoltage_OUT, Comp_1, Comp_1.mv_Ports[Comp_1_Port].mv_sVoltage_IN));
    assert(circuit.ConnectOutToIn(Comp_2, Comp_2.mv_Ports[Comp_2_Port].mv_sCurrent_OUT, Comp_1, Comp_1.mv_Ports[Comp_1_Port].mv_sCurrent_IN));
  }

  class CCircuitComposer
  {
  public:

    CCircuitComposer(const QList<QGraphicsItem*>& av_QList_AllElements, const EDGE_VEC& ac_AllEdges, const NODE_VEC& ac_AllNodes);
    ~CCircuitComposer();
    void step();
  private:
    void mf_InterpretQGrapghicsItems(const QList<QGraphicsItem*>& av_QList_AllElements, const EDGE_VEC& ac_AllEdges, const NODE_VEC& ac_AllNodes);
    const IComponent*  mf_GetFirstVoltageSource() const;
    const IComponent*  mf_ConnectsTo(const IComponent* ac_pConectee) const;
    DspCircuit mv_TheCircuit;
    std::vector<IComponent*> mv_MyComponents;
  };

}
#endif // !CIRCUITCOMPOSER_H
