#include <NodePotential.h>
#include <GaussElimination.h>
#include <boost\numeric\ublas\lu.hpp>
#include <boost\numeric\ublas\tags.hpp>

using namespace boost::numeric::ublas;

namespace Solver
{
  CNodePotential::CNodePotential(const matrix<double>& ac_G, const matrix<double>& ac_Ue, const matrix<double>& ac_A)
  {
    G = ac_G;
    Ue = ac_Ue;
    A = ac_A;
  }

  matrix<double> lf_NegateElements(matrix<double> ac_ToNegate)
  {
    matrix<double> lv_Result = ac_ToNegate;
    for (matrix<double>::size_type i = 0; i < ac_ToNegate.size1(); ++i)
    {
      for (matrix<double>::size_type j = 0; j < ac_ToNegate.size2(); ++j)
      {
        lv_Result(i, j) = -ac_ToNegate(i, j);
      }
    }
    return lv_Result;
  }

  void CNodePotential::mf_Solve()
  {
    At       = trans(A);
    AG       = prod(A, G);
    Gp       = prod(AG, At);
    minusA   = lf_NegateElements(A);
    minusAG  = prod(minusA, G);
    IpSC     = prod(minusAG, Ue);

    GaussElimination EquationSolver(Gp, IpSC, IpSC.size1());
    Vp = EquationSolver.mf_GetResult();

    U = prod(At, Vp);
    matrix<double> aux = Ue + U;
    I = prod(G, aux);
    
  }
  
}