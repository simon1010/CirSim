#include "GaussElimination.h"
#include <iostream>
#include <fstream>

#include <boost/numeric/ublas/io.hpp>

namespace Solver
{
  const int GaussElimination::sc_nMaxRange = 50;

  GaussElimination::GaussElimination(const matrix<double>& ac_a, const matrix<double> &ac_b, const int ac_nRange)
  {
    a = ac_a;
    b = ac_b;
    range = ac_nRange;
  }

  matrix<double>& GaussElimination::mf_GetResult()
  {
    if (checkRange()){
      result = solve();
    }
    return result;
  }


  bool GaussElimination::checkRange()
  {
    if (range < sc_nMaxRange)
    {
      return true;
    }
    else {
      return false;
    }
  }

  matrix<double> GaussElimination::solve()
  {
    std::ofstream LogFile;
    LogFile.open("c:\\CIRSIM_LOGGING\\GaussIntermediary.log");

    matrix<double> x(range,1,0);
    matrix<double> m;

    matrix<double> ma(range, range,0);

    for (int col = 0; col < range; col++)
    {
      ma = a;
  
      LogFile << ma << std::endl;
      LogFile << b << std::endl;      

      m = getPivot(range, col, ma);
      LogFile << "Pivot: " << m << std::endl;

      a = prod(m, ma);
      b = prod(m, b);
    }
    for (int i = range - 1; i >= 0; i--)
    {
      x(i, 0) = b(i, 0);
      for (int j = i + 1; j < range; j++)
      {
        x(i, 0) -= a(i, j) * x(j, 0);
      }
    }
    return x;
  }

  matrix<double> GaussElimination::getPivot(int range, int col, matrix<double> a_matrix)
  {
    matrix<double> result = identity_matrix<double>(range);
    result(col, col) = (1 / a_matrix(col, col));
    for (int i = col + 1; i<range; i++){
      result(i, col) = -a_matrix(i, col) / a_matrix(col, col);
    }
    return result;
  }

}

