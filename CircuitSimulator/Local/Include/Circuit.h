#pragma once

#ifndef CIRCUIT_H
#define CIRCUIT_H

/*QT*/
#include <QtWidgets\qgraphicsitem.h>

/*STD*/
#include <list>
#include <unordered_map>

/*Private*/
#include <NodePotential.h>
#include <Node.h>
#include <Edge.h>

/*Boost*/
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric;

namespace Solver{

  class Circuit
  {
  public:
    Circuit(QList<QGraphicsItem*> lv_QList_AllElements);
    ~Circuit();

    const int mf_nConnectedElements() const;
    const int mf_nTotalElements()const;
  private:

    int mv_nEdges;
    int mv_nNodes;
    int mv_nLoops;
    NODE_VEC mv_Nodes_List;
    
    int mv_nTotalElements;
    int mv_nConnectedElements;

    QList<QGraphicsItem*> mv_QList_AllElements;

    /* testing part */
    
    ublas::matrix<int> mv_CircuitMatrix;
    Solver::CNodePotential *NodePotentialSolver;
    EDGE_VEC mv_AllEdgesInCircuit;

    bool mf_HandleTerminal(QGraphicsItem *ac_Trminal);
    void mf_ConstructEdgesMatrix();
    const bool mf_bTerminalIsNode(Terminal *ac_Terminal) const;
    void mf_SimplifyEdgesMatrix();
    bool mf_bAreEdgesTheSame(EDGE ac_Edge, EDGE ac_other);
    CNode mf_GetNode(Terminal *ac_Terminal);
    /* End testing part */
    
  };
}
#endif
