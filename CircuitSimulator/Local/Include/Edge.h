#pragma once

#ifndef EDGE_H
#define EDGE_H

#include <list>
#include <Node.h>

namespace Solver{
  /*class CEdge
  {
  public:
    CEdge();
    CEdge(const CNode *ac_Node_1, const CNode *ac_Node_2);

    void mf_AddComponent(QGraphicsItem* ac_Component);

  private:
    const CNode * mv_Node_1;
    const CNode * mv_Node_2;

    std::list<QGraphicsItem* > mv_listComponents;
  };*/
  
  struct EDGE
  {
    std::vector<QGraphicsItem*> Components;
    CNode FromNode;
    CNode ToNode;
  };

  // map of edges and the node they belong to
  typedef std::vector<EDGE> EDGE_VEC;


}
#endif

