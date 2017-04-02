#include "Node.h"
#include <algorithm>

namespace Solver{

  CNode::CNode() :NodeID(0)
  {
  }
  CNode::CNode(const int ac_NodeID) : NodeID(ac_NodeID)
  {
  }

  void CNode::mf_AddTerminal(Terminal* ac_Terminal)
  {
    mv_Terminals.push_back(ac_Terminal);
  }
  
  bool CNode::mf_bContainsTerminal(Terminal* ac_Terminal)
  {
    for (Terminal *it : mv_Terminals)
    {
      if (it == ac_Terminal)
      {
        return true;
      }
    }
    return false;
  }

  const int CNode::mf_nGetTerminalCount() const
  {
    return mv_Terminals.size();
  }

  Terminal * CNode::mf_GetTerminal(const int ac_nIndex) const
  {
    _ASSERT(ac_nIndex < mv_Terminals.size());
    auto lv_TerminalIt = mv_Terminals.begin();
    std::advance(lv_TerminalIt, ac_nIndex);
    return *lv_TerminalIt;
  }
}