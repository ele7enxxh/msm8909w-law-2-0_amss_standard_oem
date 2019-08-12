/*============================================================================
  @file accel_cal.c

  Gyroscope calibration algorithm source file

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

#include "matrix_double.h"
#include "sns_memmgr.h"

void matrix_double_copy(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                   int Mrows, int Mcols,
                   double O[MATRIX_MAX_ROW][MATRIX_MAX_COL])
{
   SNS_OS_MEMCOPY(O, M, (sizeof(M[0][0])*Mrows*Mcols) );
}


void matrix_double_multiply(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                       int Mrows, int Mcols,
                       double N[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                       int Nrows, int Ncols,
                       double O[MATRIX_MAX_ROW][MATRIX_MAX_COL])
{
   UNREFERENCED_PARAMETER(Nrows);
   int row,col,i;
   double MTemp[MATRIX_MAX_ROW][MATRIX_MAX_COL];
   double NTemp[MATRIX_MAX_ROW][MATRIX_MAX_COL];

   matrix_double_copy(M,MATRIX_MAX_ROW,MATRIX_MAX_COL, MTemp);
   matrix_double_copy(N,MATRIX_MAX_ROW,MATRIX_MAX_COL, NTemp);

   for (row = 0; row < Mrows; row++)
   {
      for (col = 0; col < Ncols; col++)
      {
         O[row][col] = 0;
         for (i = 0; i < Mcols; i++)
         {
            O[row][col] += MTemp[row][i] * NTemp[i][col];
         }
      }
   }
}

void matrix_double_add(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                  int Mrows, int Mcols,
                  double N[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                  int Nrows, int Ncols,
                  double O[MATRIX_MAX_ROW][MATRIX_MAX_COL])
{
   UNREFERENCED_PARAMETER(Nrows);
   UNREFERENCED_PARAMETER(Ncols);
   int row,col;

   for (row = 0; row < Mrows; row++)
   {
      for (col = 0; col < Mcols; col++)
      {
         O[row][col] = M[row][col] + N[row][col];
      }
   }
}

void matrix_double_transpose(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                        int Mrows, int Mcols,
                        double O[MATRIX_MAX_ROW][MATRIX_MAX_COL])
{
   UNREFERENCED_PARAMETER(Mrows);
   UNREFERENCED_PARAMETER(Mcols);
   int row,col;
   double MTemp[MATRIX_MAX_ROW][MATRIX_MAX_COL];

   matrix_double_copy(M,MATRIX_MAX_ROW,MATRIX_MAX_COL, MTemp);

   for (row = 0; row < MATRIX_MAX_ROW; row++ )
   {
      for (col = 0; col < MATRIX_MAX_COL; col++ )
      {
         O[row][col] = MTemp[col][row];
      }
   }

}


void matrix_double_negate(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                     int Mrows, int Mcols,
                     double O[MATRIX_MAX_ROW][MATRIX_MAX_COL])
{
   int row,col;
   double MTemp[MATRIX_MAX_ROW][MATRIX_MAX_COL];

   matrix_double_copy(M,MATRIX_MAX_ROW,MATRIX_MAX_COL, MTemp);

   for (row = 0; row < Mrows; row++)
   {
      for (col = 0; col < Mcols; col++)
      {
         O[row][col] = -1 * MTemp[row][col];
      }
   }
}

void matrix_double_identity(double O[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                       int Orows, int Ocols)
{
   int row,col;

   for (row = 0; row < Orows; row++)
   {
      for (col = 0; col < Ocols; col++)
      {
         if(row == col)
         {
            O[row][col] = 1.0;
         }
         else
         {
            O[row][col] = 0.0;
         }
      }
   }
}

void matrix_double_zero(double O[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                       int Orows, int Ocols)
{
   SNS_OS_MEMZERO(O, sizeof(O[0][0])*Orows*Ocols);
}

