#pragma once

#ifndef MATRIX_H
#define MATRIX_H

class matrix
{
  int mv_nRows, mv_nColumns;
  int **p;

public:
  matrix(const int Rows,const int Columns)
  {
    mv_nRows = Rows;
    mv_nColumns = Columns;
    p = new int*[mv_nRows];
    for (int i = 0; i < mv_nRows; ++i)
    {
      p[i] = new int[mv_nColumns];
      for (int j = 0; j < mv_nColumns; ++j)
        p[i][j] = 0;
    }
  }

  matrix()
  {
  }

  matrix(matrix &ac_Other)
  {
    this->operator=(ac_Other);
  }

  __forceinline matrix& operator+ (const matrix& ac_Other)
  {
    return (*this += ac_Other);
  }

  __forceinline matrix& operator* (const matrix& ac_Other)
  {
    return (*this *= ac_Other);
  }

  __forceinline matrix& operator+= (const matrix& ac_Other)
  {
    for (int i = 0; i < mv_nRows; i++)
    {
      for (int j = 0; j < mv_nColumns; j++)
      {
        p[i][j] += ac_Other.p[i][j];
      }
    }
    return *this;
  }

  __forceinline matrix& operator*= (const matrix& ac_Other)
  {
    if (mv_nColumns == ac_Other.mv_nRows)
    {
      for (int i = 0; i < ac_Other.mv_nRows; ++i)
      {
        for (int k = 0; k < mv_nColumns; ++k)
        {
          p[i][k] *= ac_Other.p[k][i];
        }
      }
    }

    return *this;
  }

  __forceinline matrix& operator= (const matrix& ac_Other)
  {/*Will cause problems, must copy element by element*/
    p = ac_Other.p;
    mv_nColumns = ac_Other.mv_nColumns;
    mv_nRows = ac_Other.mv_nRows;

    return *this;
  }

};

#endif