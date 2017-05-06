#include "Edge.h"

namespace Solver
{
  CEdge::CEdge(const CNode* ac_Node_1, const CNode* ac_Node_2):
    mv_Node_1(ac_Node_1),
    mv_Node_2(ac_Node_2)
  {
  }

  void CEdge::mf_AddComponent(QGraphicsItem* ac_Component)
  {
    mv_listComponents.push_back(ac_Component);
  }
}