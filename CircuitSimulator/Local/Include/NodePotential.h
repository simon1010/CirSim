#pragma once

#ifndef NODEPOTENTIAL_H
#define NODEPOTENTIAL_H

#include "boost/numeric/ublas/matrix.hpp"

using namespace boost::numeric::ublas;

namespace Solver
{
  class CNodePotential
  {

  public:
    CNodePotential(const matrix<double>& ac_G, const matrix<double>& ac_Ue, const matrix<double>& ac_A);
    
    void mf_Solve();

    matrix<double> At      ;
    matrix<double> AG      ;
    matrix<double> Gp      ;
    matrix<double> minusA  ;
    matrix<double> minusAG ;
    matrix<double> IpSC    ;
    matrix<double> Vp      ;

    matrix<double> U;
    matrix<double> I;
  private:

    matrix<double> G;
    matrix<double> Ue;
    matrix<double> A;
  };

}

#endif