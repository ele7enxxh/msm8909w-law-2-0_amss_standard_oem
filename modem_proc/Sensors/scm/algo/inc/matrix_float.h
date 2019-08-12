#ifndef MATRIX_FLOAT_H
#define MATRIX_FLOAT_H

/*============================================================================
@file matrix_float.h

Magnetometer calibration algorithm header file

Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/scm/algo/inc/matrix_float.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
EDIT HISTORY

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who  what, where, why
----------  ---  -----------------------------------------------------------
2015-03-26  ps   Updated license header
2013-06-18  ks   initial version

============================================================================*/
#include <stdint.h>
#include "stdbool.h"
#include <float.h>

#ifndef OFFTARGET_UNIT_TESTING

#include "sns_debug_api.h"
#include "sns_memmgr.h"
#define MATRIX_FLOAT_MALLOC(x) SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SAM, (x) )
#define MATRIX_FLOAT_FREE(x) SNS_OS_FREE(x)

#else

#include <stdlib.h>
#define MATRIX_FLOAT_MALLOC(x) malloc(x)
#define MATRIX_FLOAT_FREE(x) free(x)

#endif

#define PREALLOCATE_MATRIX_INVERSION_BUFFER
#define MIN_INV_MATRIX_ROWS_THAT_REQUIRE_BUFFER 3

typedef struct
{
   float min;
   float max;
   float span;
   float variance;
} matrix_float_stats_s;

typedef struct
{ 
   float **cofactor_buff;  // one buffer of max size, will be used for any smaller cofactor matrix as well
   float **determinant_buff; // set of buffers of different size for recursive matrix_float_sq_determinant()
   int max_inverted_matrix_rows; // for confirmation/verification
} matrix_inversion_buffer_s; 

typedef enum
{
   MATRIX_FLOAT_EVAL_ROW = 0,
   MATRIX_FLOAT_EVAL_COL = 1
} matrix_float_stats_eval_type_e;



/*=========================================================================
FUNCTION:  matrix_float_malloc_mem_req
=========================================================================*/
/*!
@brief
Provides the algorithm memory requirement for the specified matrix size

@return
algorithm state memory requirement in bytes if successful, 0 if error
*/
/*=======================================================================*/

int32_t matrix_float_malloc_mem_req(uint32_t rows, uint32_t cols);

/*=========================================================================
FUNCTION:  matrix_float_malloc
=========================================================================*/
/*!
@brief
Allocate memory for a 2D float matrix

@param[o] mat: pointer to pointer to float set to NULL when function is called.
mat is used to return the allocated memory for the 2D mattrix
@param[i] rows: the number of rows of the matrix
@param[i] cols: the number of columns of the matrix

@return true if successful, false otherwise.
*/
/*=======================================================================*/
bool matrix_float_malloc(float ***mat, uint32_t rows, uint32_t cols);

/*=========================================================================
FUNCTION:  matrix_float_free
=========================================================================*/
/*!
@brief
Free memory associated with a 2D float matrix

@param[o] mat:  pointer to pointer to pointer to float that needs to be freed

@return None
*/
/*=======================================================================*/
void matrix_float_free(float ***mat);

/*=========================================================================
FUNCTION:  matrix_float_reset
=========================================================================*/
/*!
@brief
Init 2D float matrix

@param[o] mat:  pointer to pointer to pointer.
@param[i] rows: the number of rows of the matrix
@param[i] cols: the number of columns of the matrix
@param[i] mem: pointer to the memory allocated earlier for the matrix
@return None
*/
/*=======================================================================*/
void matrix_float_reset(float ***mat, uint32_t rows, uint32_t cols, void *mem);


/*=========================================================================
FUNCTION:  matrix_float_inversion_buffer_mem_req
=========================================================================*/
/*!
@brief
Provides the matrix inversion buffers memory requirement for the specified square matrix size

@param[i] max_inverted_matrix_rows: maximum square matrix dimensions in the algo 
that needs to be inverted

@return
buffer memory requirement in bytes if successful, 0 if error
*/
/*=======================================================================*/

int32_t matrix_float_inversion_buffer_mem_req (int max_inverted_matrix_rows); // same as num columns, of course

/*=========================================================================
FUNCTION:  matrix_float_inversion_buffer_reset
=========================================================================*/
/*!
@brief
Reset the buffer

@param[i] pointer to matrix_inversion_buffer_s type structure 
@param[i] max_inverted_matrix_rows: maximum square matrix dimensions in the algo 
that needs to be inverted
@param[i] mem: pointer to the memory allocated earlier for the (cofactor_buff + determinant_buff)

@return
true on success
*/
/*=======================================================================*/

bool matrix_float_inversion_buffer_reset (matrix_inversion_buffer_s *inversion_buff_p,
                                          int max_inverted_matrix_rows, void *mem);

/*=========================================================================
FUNCTION:  matrix_float_sq_determinant
=========================================================================*/
/*!
@brief
Computes the determinant of a given square matrix

@param[i] matA: pointer to 2D square array representing the input matrix
for which determinant has to be computed.
@param[i] rowsA: the number of rows of the square matrix matA

@return
floating point value of the determinant of the square matrix matA
*/
/*=======================================================================*/

float matrix_float_sq_determinant(float **matA,int rowsA, float **determinant_buff);

/*=========================================================================
FUNCTION:  matrix_float_sq_cofactor
=========================================================================*/
/*!
@brief
Computes the cofactor matrix for a given square matrix

@param[i] matA: pointer to 2D square array representing the input matrix
for which cofactor matrix has to be computed.
@param[i] rowsA: the number of rows of the square matrix matA
@param[o] matO: pointer to 2D square array to which cofactor matrix for
matA is written

@return true if successful, false otherwise.
*/
/*=======================================================================*/
bool matrix_float_sq_cofactor(float **matA,int rowsA, bool is_symmetric,
                              matrix_inversion_buffer_s *matrix_inversion_buff_p, float **matO);

/*=========================================================================
FUNCTION:  matrix_float_sq_transpose
=========================================================================*/
/*!
@brief
Transposes a given square matrix

@param[i] matA: pointer to 2D square array representing the input matrix
that needs to be transposed. matA is replaced with it's transpose.
@param[i] rowsA: the number of rows of the square matrix matA

@return None
*/
/*=======================================================================*/
void matrix_float_sq_transpose(float **matA,int rowsA);

/*=========================================================================
FUNCTION:  matrix_float_sq_inverse
=========================================================================*/
/*!
@brief
Computes the inverse of a given square matrix

@param[i] matA: pointer to 2D square array representing the input for which
the inverse needs to be computed
@param[i] rowsA: the number of rows of the square matrix matA
@param[i] detLimit: the determinant limit that must be exceeded in order for
inversion to be conducted. 

@return boolean set to true if inversion was succesful.
*/
/*=======================================================================*/
bool matrix_float_sq_inverse(float **matA,int rowsA,float detLimit, bool is_symmetric,
                             matrix_inversion_buffer_s *matrix_inversion_buff_p,float **matO);

/*=========================================================================
FUNCTION:  matrix_float_transpose
=========================================================================*/
/*!
@brief
Transposes a given rectangular matrix

@param[i] matA: pointer to 2D square array representing the input matrix
that needs to be transposed.
@param[i] rowsA: the number of rows of the matrix matA
@param[i] colsA: the number of columns of the matrix matA
@param[i] matO: pointer to 2D square array representing the output matrix
to which the transpose needs to be written to.
@param[i] rowsO: the number of rows of the matrix matO
@param[i] colsO: the number of columns of the matrix matO

@return boolean set to true if succesful
*/
/*=======================================================================*/
bool matrix_float_transpose(float **matA,int rowsA, int colsA,
                            float **matO, int rowsO, int colsO);

/*=========================================================================
FUNCTION:  matrix_float_multiply
=========================================================================*/
/*!
@brief
Computes the product of two matrices

@param[i] matA: pointer to 2D array representing the first matrix to be multiplied
@param[i] rowsA: the number of rows of the matrix matA
@param[i] colsA: the number of columns of the matrix matA
@param[i] matB: pointer to 2D array representing the second matrix to be multiplied with
The number of rows of matB must be equal to number of columns of matA
@param[i] colsB: the number of columns of the matrix matB
@param[o] matO: pointer to 2D array representing the product of matA & mat B

@return boolean set to true if multiplication was succesful.
*/
/*=======================================================================*/
bool matrix_float_multiply(float **matA,
                           int rowsA,
                           int colsA,
                           float **matB,
                           int colsB,
                           float **matO);

/*=========================================================================
FUNCTION:  matrix_float_transpose_first_multiplicand_and_multiply
=========================================================================*/
/*!
@brief
Computes the product of two matrices Atrans*B

@param[i] matA: pointer to 2D array representing the first matrix to be multiplied after transposition
@param[i] rowsA: the number of rows of the matrix matA
@param[i] colsA: the number of columns of the matrix matA
@param[i] matB: pointer to 2D array representing the second matrix to be multiplied with
The number of rows of matB must be equal to number of columns of matA
@param[i] colsB: the number of columns of the matrix matB
@param[o] matO: pointer to 2D array representing the product of matA & mat B

@return boolean set to true if multiplication was succesful.
*/
/*=======================================================================*/
bool matrix_float_transpose_first_multiplicand_and_multiply(float **matA,
                                                            int rowsA,
                                                            int colsA,
                                                            float **matB,
                                                            int colsB,
                                                            float **matO);

/*=========================================================================
FUNCTION:  matrix_float_scalar_multiply
=========================================================================*/
/*!
@brief
Multiplies all the elements of a given matrix by a scalar value

@param[i] matA: pointer to 2D array representing the matrix upon which scalar
multiplication needs to be performed. matA is replaced with
the scalara multiplied output.
@param[i] rowsA: the number of rows of the matrix matA
@param[i] colsA: the number of columns of the matrix matA
@param[i] mul: the scalar multiplier

@return boolean set to true if multiplication was succesful.
*/
/*=======================================================================*/
bool matrix_float_scalar_multiply(float **matA,
                                  int rowsA,
                                  int colsA,
                                  float mul);



/*=========================================================================
FUNCTION:  matrix_float_statistics
=========================================================================*/
/*!
@brief
Computes statistics of a specified row / column in a given 2D matrix array

@param[i] matA: pointer to 2D square array representing the input matrix
that needs to be evaluated
@param[i] rowsA: the number of rows of the matrix matA
@param[i] colsA: the number of columns of the matrix matA
@param[i] eval_type: enum specifying the kind of evaluation requested
@param[i] eval_num: the column / row of the matrix that needs to be evaluated
@param[o] stats: pointer to matrix_float_stats_s struct to which the evaluated
statistics are returned

@return boolean set to true if succesful
*/
/*=======================================================================*/
bool matrix_float_statistics(float **matA,
                             uint32_t rowsA,
                             uint32_t colsA,
                             matrix_float_stats_eval_type_e eval_type,
                             uint32_t eval_num,
                             matrix_float_stats_s *stats);

#endif /* MATRIX_FLOAT_H */
