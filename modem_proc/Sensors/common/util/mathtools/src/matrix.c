/*============================================================================
  @file matrix.c

  Matrix utility source file

  Copyright (c) 2010-2011, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/mathtools/src/matrix.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-26  ps   Updated license header
  2011-01-24  ad   initial version

============================================================================*/

#include "matrix.h"
#include "fixed_point.h"

/*===========================================================================

  FUNCTION: matrix_multiply

  ===========================================================================*/
/*!
  @brief Multiplies two matrices together

  @param[io] result: The resultant, A * B.

  @param[i] A: First matrix

  @param[i] B: Second matrix

  @return
  None.

  @note
  Result must be allocated before calling this function.
*/
/*=========================================================================*/
void matrix_multiply(matrix_type *result, matrix_type *A, matrix_type *B)
{
  int32_t i, j, k;

  for (i=0; i<A->rows; i++)
  {
    for (j=0; j<B->cols; j++)
    {
      for (k=0; k<A->cols; k++)
      {
        MATRIX_ELEM(result,i,j) += FX_MUL_Q16(MATRIX_ELEM(A,i,k), MATRIX_ELEM(B,k,j));
      }
    }
  }
}

/*===========================================================================

  FUNCTION: matrix_copy

  ===========================================================================*/
/*!
  @brief Copies a matrix

  @param[io] to: The destination

  @param[i] from: The source

  @return
  None.

  @note
  "to" must be allocated before calling.
*/
/*=========================================================================*/
void matrix_copy(matrix_type *to, matrix_type *from)
{
  int32_t i, j;

  for (i=0; i<to->rows; i++)
  {
    for (j=0; j<to->cols; j++)
    {
      MATRIX_ELEM(to, i, j) = MATRIX_ELEM(from, i, j);
    }
  }
}

/*===========================================================================

  FUNCTION: vector_norm

  ===========================================================================*/
/*!
  @brief  Calculates the norm of a vector

  @param[io] m: Matrix (vector) to normalize

  @return
  The sqrt of the sum of squares in the vector.

  @note
*/
/*=========================================================================*/
int32_t vector_norm(matrix_type *m)
{
  int32_t i, length, sum;

  length = (m->rows > m->cols) ? m->rows : m->cols;
  sum    = 0;

  for (i=0; i<length; i++)
  {
    sum += FX_MUL_Q16(MATRIX_ELEM(m, i, 0), MATRIX_ELEM(m, i, 0));
  }

  return sqrtFxQ16(sum);
}

/*===========================================================================

  FUNCTION: vector_normalize

  ===========================================================================*/
/*!
  @brief  Normalizes a vector

  @param[io] m: Matrix to normalize

  @return
  None.

  @note
*/
/*=========================================================================*/
void vector_normalize(matrix_type *m)
{
  int32_t i, norm_inv, norm, length;

  norm     = vector_norm(m);
  norm_inv = FX_DIV_Q16(FX_ONE_Q16, norm);
  length   = (m->rows > m->cols) ? m->rows : m->cols;

  for (i=0; i<length; i++)
  {
    MATRIX_ELEM(m, i, 0) = FX_MUL_Q16(MATRIX_ELEM(m, i, 0), norm_inv);
  }
}
