#pragma once

#ifndef NODE_H
#define NODE_H

#include <list>
#include <Terminal.h>

namespace Solver{
  class CNode
  {
  public:
    CNode();
    CNode(int NodeID);

    void mf_AddTerminal(Terminal* ac_Terminal);
    bool mf_bContainsTerminal(Terminal* ac_Terminal);

    const int mf_nGetTerminalCount() const;
    Terminal * mf_GetTerminal(const int ac_nIndex) const;
    const int mf_GetNodeID()const { return NodeID; }
  private:
    int NodeID;
    
    // the node is built from these terminals
    std::list<Terminal*> mv_Terminals;
  };

  typedef std::vector<CNode> NODE_VEC;
}
#endif

