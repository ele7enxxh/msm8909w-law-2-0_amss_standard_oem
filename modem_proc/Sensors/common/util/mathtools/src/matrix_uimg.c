/*============================================================================
  @file matrix.c

  Matrix utility source file

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "matrix.h"
#include "fixed_point.h"

/*===========================================================================

  FUNCTION: matrix_zero

  ===========================================================================*/
/*!
  @brief Sets a matrix's values to zero

  @param[io] m: The matrix

  @return
  None.

  @note
*/
/*=========================================================================*/
void matrix_zero(matrix_type *m)
{
  int32_t i, j;

  for (i=0; i<m->rows; i++)
  {
    for (j=0; j<m->cols; j++)
    {
      MATRIX_ELEM(m, i, j) = 0;
    }
  }
}

/*===========================================================================

  FUNCTION: matrix_reset

  ===========================================================================*/
/*!
  @brief Initializes a matrix

  @param[io] m: the matrix

  @param[i] rows: Rows

  @param[i] cols: Cols

  @param[i] mem: Pointer to memory for matrix

  @return
  None.

  @note
*/
/*=========================================================================*/
void matrix_reset(matrix_type *m, int32_t rows, int32_t cols, void *mem)
{
  m->matrix = mem;

  m->rows = rows;
  m->cols = cols;
  matrix_zero(m);
}

/*===========================================================================

  FUNCTION: matrix_mem_req

  ===========================================================================*/
/*!
  @brief Calculates size of memory for a rows x cols matrix.

  @param[i] rows: Rows of the matrix

  @param[i] Cols: Columns of the matrix

  @return
  Size (in bytes) of a rows x cols matrix

  @note
*/
/*=========================================================================*/
int32_t matrix_mem_req(int32_t rows, int32_t cols)
{
  return (sizeof(int32_t)*rows*cols);
}

