/*============================================================================
  @file accel_cal.c

  Gyroscope calibration algorithm source file 

  Copyright (c) 2010-2011, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/mathtools/inc/matrix_double.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-26  ps   Updated license header
  2012-06-09  ks   initial version  

============================================================================*/

#define MATRIX_MAX_ROW (6)
#define MATRIX_MAX_COL (6)

// O = M
void matrix_double_copy(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                   int Mrows, int Mcols,
                   double O[MATRIX_MAX_ROW][MATRIX_MAX_COL]);

// O = M * N
void matrix_double_multiply(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                       int Mrows, int Mcols,
                       double N[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                       int Nrows, int Ncols,
                       double O[MATRIX_MAX_ROW][MATRIX_MAX_COL]);

// O = M + N
void matrix_double_add(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                  int Mrows, int Mcols,
                  double N[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                  int Nrows, int Ncols,
                  double O[MATRIX_MAX_ROW][MATRIX_MAX_COL]);

// O = M'
void matrix_double_transpose(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                        int Mrows, int Mcols,
                        double O[MATRIX_MAX_ROW][MATRIX_MAX_COL]);

// O = -M
void matrix_double_negate(double M[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                     int Mrows, int Mcols,
                     double O[MATRIX_MAX_ROW][MATRIX_MAX_COL]);

// O = I ( Identity )
void matrix_double_identity(double O[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                       int Orows, int Ocols);

// O = Zero Matrix
void matrix_double_zero(double O[MATRIX_MAX_ROW][MATRIX_MAX_COL],
                       int Orows, int Ocols);

