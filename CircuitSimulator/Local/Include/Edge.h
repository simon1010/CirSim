#pragma once

#ifndef EDGE_H
#define EDGE_H

#include <list>
#include <Node.h>

namespace Solver{
  
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

