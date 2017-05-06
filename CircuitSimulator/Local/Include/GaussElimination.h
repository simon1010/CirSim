#pragma once

#ifndef GAUSS_ELIMINATION_H
#define GAUSS_ELIMINATION_H

#include <boost/numeric/ublas/matrix.hpp>
using namespace boost::numeric::ublas;

namespace Solver{

  class GaussElimination
  {
  public:
    GaussElimination(const matrix<double>& ac_a, const matrix<double> &ac_b, const int ac_nRange);
    matrix<double>& mf_GetResult();
    matrix<double> solve();

  private:
    static const int sc_nMaxRange;

    int range; // the range of the equations system
    matrix<double> a; //the coefficients square matrix
    matrix<double> b; // the free terms column
    matrix<double> result;

    bool checkRange();
    matrix<double> getPivot(int range, int col, matrix<double> a_matrix);    

  };
}
#endif
