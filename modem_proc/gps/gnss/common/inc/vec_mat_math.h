/*==============================================================================
  FILE:         vec_mat_math.h

  OVERVIEW:     Vector & Matrix math functions used by gnss/sdp, gnss/le
                and gnss/mgp/pe/nf

                Note that when this file was created, to comply with modularity 
                rules, the legacy function name prefixes of pe_ were retained 
                in this first step.
                Ideally these functions should be renamed with the prefix loc_ 
                (or gnss_) which is consistent with their current modularity 
                level.
                
                Any new similar functions created in this file shall be named 
                with the right-level prefix (e.g. loc_ )

  DEPENDENCIES: NA
 
                Copyright (c) 2015 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/


/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/vec_mat_math.h#1 $ 
   $DateTime: 2016/12/13 07:59:45 $ 
   $Author: mplcsds1 $ 

  when        who  what, where, why
  ----------  ---  -------------------------------------------------------------
  2015-07-01  wtr  new file extracted mostly from mgp_pe_common

==============================================================================*/

#ifndef _VEC_MAT_MATH_H
#define _VEC_MAT_MATH_H

/*------------------------------------------------------------------------------
 * Include Files
 * ---------------------------------------------------------------------------*/

#include "gps_types.h" // for DBL, ..

/*------------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ---------------------------------------------------------------------------*/

/******************************************************************************
 * Function description:
 *  pe_SetIdentityMatrix() sets the input matrix to an identity matrix
 *                         e.g. to initialize a rotation matrix
 * 
 * Parameters: d_SquareMatrix - Pointer to the upper left corner of a square
 *                              matrix to be filled in with 1's on the diagonal
 *             u_Size         - Size of matrix, e.g. 3 for 3x3
 *
 * Returns:  (none)
 *
 ******************************************************************************
*/
void pe_SetIdentityMatrix(DBL * d_SquareMatrix, 
                          U8 u_Size);

/******************************************************************************
 * Function description:
 *  pe_MatrixScalarMultiply() - multiplies a matrix by a scalar
 *
 * Parameters:
 *    d_InMat  - Input matrix
 *    d_Scalar - Scalar multiplier
 *    d_OutMat - Output matrix
 *
 * Return value:  None
 *
 * Notes
 * 1: OutMat could be same as InMat
 * 2: This function can also be used to make a copy of the matrix by using
 *    d_Scalar = 1.0
 ******************************************************************************/
boolean pe_MatrixScalarMultiply(const DBL *d_InMat,  U32 u_InMatRows,  U32 u_InMatCols,
                                  DBL *d_OutMat, U32 u_OutMatRows, U32 u_OutMatCols,
                                  const DBL d_Scalar);

/******************************************************************************
 * Function description:
 *  pe_MatrixMultiply() - computes d_OutMat = d_InMat1 X d_InMat2
 *
 * Parameters:
 *    d_InMat1  - Input Matrix 1
 *    d_InMat2  - Input Matrix 2
 *    d_OutMat  - Output Matrix
 *
 * Return value:  None
 *
 * Note: OutMat CAN NOT be any of InMat1 or InMat2
 ******************************************************************************/
boolean pe_MatrixMultiply(const DBL *d_InMat1, U32 u_InMat1Rows, U32 u_InMat1Cols,
                            const DBL *d_InMat2, U32 u_InMat2Rows, U32 u_InMat2Cols,
                            DBL *d_OutMat, U32 u_OutMatRows, U32 u_OutMatCols);

/******************************************************************************
 * Function description:
 *  pe_MatrixCopy() - copies a matrix into another one of the same size
 *
 * Parameters:
 *    d_InMat   - Input Matrix
 *    d_OutMat  - Output Matrix
 *
 * Return value:  None
 ******************************************************************************/
boolean pe_MatrixCopy(const DBL *d_InMat,  U32 u_InMatRows,  U32 u_InMatCols,
                        DBL *d_OutMat, U32 u_OutMatRows, U32 u_OutMatCols);

/******************************************************************************
 * Function description:
 *  pe_MatrixTranspose() - computes matrix transpose
 *
 * Parameters:
 *    d_InMat   - Input Matrix
 *    d_OutMat  - Output Matrix, transpose of d_InMat
 *
 * Return value:  true or false
 ******************************************************************************/
boolean pe_MatrixTranspose(const DBL *d_InMat,  U32 u_InMatRows,  U32 u_InMatCols,
                             DBL *d_OutMat, U32 u_OutMatRows, U32 u_OutMatCols);
/******************************************************************************
 * Function description:
 *  pe_MatrixSwapRows() - swaps two rows of a matrix
 *
 * Parameters:
 *    d_Mat   - Input Matrix
 *    u_Rows, u_Cols  - Dimensions of input matrix
 *    u_SwapRow1, u_SwapRow2 - rows to swap
 *
 * Return value:  true or false
 ******************************************************************************/
boolean pe_MatrixSwapRows(DBL *d_Mat, U32 u_Rows, U32 u_Cols, 
                            U32 u_SwapRow1, U32 u_SwapRow2);

/******************************************************************************
 * Function description:
 *  pe_MatrixTrace3x3() - computes the trace of a 3x3 matrix
 *                        - ie. sum of diagonal
 *
 * Parameters:
 *    d_Mat   - Input Matrix
 *
 * Return value:  sum of diagonal elements (trace)
 ******************************************************************************/
DBL pe_MatrixTrace3x3(const DBL d_Mat[][3]);

/******************************************************************************
 * Function description:
 *  pe_MatrixDet3x3() - computes the determinant of a 3x3 matrix
 *
 * Parameters:
 *    d_Mat   - Input Matrix
 *
 * Return value:  determinant of 3x3
 ******************************************************************************/
DBL pe_MatrixDet3x3(const DBL d_Mat[][3]);

/******************************************************************************
 * Function description:
 *  pe_MatrixAdd() - computes d_OutMat = d_InMat1 + d_InMat2
 *
 * Parameters:
 *    d_InMat1  - Input Matrix 1
 *    d_InMat2  - Input Matrix 2
 *    d_OutMat  - Output Matrix
 *
 * Return value:  true if successful
 *
 * Note: OutMat can be InMat1 or InMat2
 ******************************************************************************/
boolean pe_MatrixAdd(const DBL *d_InMat1, U32 u_InMat1Rows, U32 u_InMat1Cols,
                       const DBL *d_InMat2, U32 u_InMat2Rows, U32 u_InMat2Cols,
                       DBL *d_OutMat, U32 u_OutMatRows, U32 u_OutMatCols);

/******************************************************************************
 * Function description:
 *  pe_MatrixAdd() - computes d_OutMat = d_InMat1 - d_InMat2
 *
 * Parameters:
 *    d_InMat1  - Input Matrix 1
 *    d_InMat2  - Input Matrix 2
 *    d_OutMat  - Output Matrix
 *
 * Return value:  true if successful
 *
 * Note: OutMat can be InMat1 or InMat2
 ******************************************************************************/
boolean pe_MatrixSubtract(const DBL *d_InMat1, U32 u_InMat1Rows, U32 u_InMat1Cols,
                            const DBL *d_InMat2, U32 u_InMat2Rows, U32 u_InMat2Cols,
                            DBL *d_OutMat, U32 u_OutMatRows, U32 u_OutMatCols);
/******************************************************************************
 * Function description:
 *  pe_MatrixInvert3x3() - inverts a 3x3 matrix if it's not singular
 *
 * Parameters:
 *    d_InMat   - Input Matrix
 *    d_OutMat  - Output Matrix, transpose of d_InMat
 *
 * Return value:  true or false
 ******************************************************************************/
boolean pe_MatrixInvert3x3(const DBL d_Mat[][3], DBL d_MatInv[][3]);

/******************************************************************************
 * Function description:
 *  pe_CrossProduct3X1() - computes C = A X B
 *
 * Parameters:
 *    DBL d_A[], DBL d_B[], DBL d_C[]
 *
 * Return value:  d_C stores the cross product of A and B
 ******************************************************************************/
void pe_CrossProduct3X1(const DBL d_A[], const DBL d_B[], DBL d_C[]);

/******************************************************************************
 * Function description:
 *  pe_DotProduct3X1() - computes A . B
 *
 * Parameters:
 *    DBL d_A[], DBL d_B[]
 *
 * Return value:  dot product of d_A and d_B
 ******************************************************************************/
DBL pe_DotProduct3X1(const DBL d_A[], const DBL d_B[]);

/******************************************************************************
 * Function description:
 *  pe_VectorDotProdDblFlt() - computes the dot product of two vectors
 *
 * Parameters:
 *    DBL d_A[], FLT f_B[], q_Rows 
 *
 * Return value:  dot product of d_A and f_B
 ******************************************************************************/
DBL pe_VectorDotProdDblFlt(const DBL d_A[], const FLT f_B[], U32 q_Rows);

/******************************************************************************
 * Function description:
 *  pe_VectorNorm() - computes the 2-norm of an arbitrary length vector
 *
 * Parameters:
 *    DBL d_V, q_Rows
 *
 * Return value:  2-norm of d_V, where the length of d_V is q_Rows
 ******************************************************************************/
DBL pe_VectorNorm(const DBL *d_V, U32 q_Rows);

/******************************************************************************
 * Function description:
 *  pe_VectorNormFlt() - computes the 2-norm of an arbitrary length vector
 *
 * Parameters:
 *    FLT pf_V, q_Rows
 *
 * Return value:  2-norm of f_V, where the length of f_V is q_Rows
 ******************************************************************************/
FLT pe_VectorNormFlt(const FLT * pf_V, U32 q_Rows);

/******************************************************************************
 * Function description:
 *  pe_VectorSumofSquares() - computes the sum of squares of an arbitrary 
 *                              length vector    
 * Parameters:
 *    DBL d_V, q_Rows
 *
 * Return value:  sum of squares of d_V, where the length of d_V is q_Rows
 ******************************************************************************/
DBL pe_VectorSumofSquares(const DBL *d_V, U32 q_Rows);

/*==============================================================================

  FUNCTION      le_VectorSumofSquaresFlt()

  DESCRIPTION   computes the sum of squares of an arbitrary length vector

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS    Description of the parameters to the functions

    pf_V   - input - pointer to single precision floating array of values
    q_Rows - input - number of values to sum-square (length of pf_V)

  RETURN VALUE  The sum of squares

  SIDE EFFECTS  None

==============================================================================*/
FLT le_VectorSumOfSquaresFlt(const FLT * pf_V, U32 q_Rows);

// old name alias to avoid changing a lot of code
#define pe_VectorSumofSquaresFlt le_VectorSumOfSquaresFlt

/******************************************************************************
 * Function description:
 *  pe_VectorNormalize() - normalizes vector d_V
 *
 * Parameters:
 *    DBL d_V, u_Rows
 *
 * Return value:  - none
 *                - modifies d_V so that it has length 1 (ie. |d_V| == 1)
 ******************************************************************************/
void pe_VectorNormalize(DBL *d_V, U32 u_Rows);

/******************************************************************************
 * Function description:
 *  pe_CovTransform()
 *                  Transform covariance matrix from one co-ordinate system
 *                  to another. 3 X 3 matrices for position or velocity        
 *                  can be transformed. 
 *
 * Parameters:      d_InputCov       - 3 X 3 Input  covariance matrix 
 *                  d_OutputCov      - 3 X 3 Output covariance matrix 
 *                                    
 *                  d_Xform        - Rotation matrix
 *
 * Return value:    None
 ******************************************************************************/
void pe_CovTransform(const DBL  d_Xform[][3], const DBL d_InputCov[][3], DBL d_OutputCov[][3]);

#endif  // _VEC_MAT_MATH_H
