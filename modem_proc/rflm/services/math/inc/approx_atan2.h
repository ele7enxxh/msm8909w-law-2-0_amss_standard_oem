/*!
  @file
  approx_atan2.h

  @brief
  Approximate atan2() functions.
*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef APPROX_ATAN2_H
#define APPROX_ATAN2_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  approx_atan2_poly7

===========================================================================*/
/*!
    @brief
    Approximate atan2(y,x)/pi using 7th degree polynomial approximation.

    @detail
    Given a pair of signed inputs, return an approximation of atan2(y,x)/pi.
    If the input pair is (0,0), the output returned is 0.
    Valid input range is 0-0xFFFFFFFF

    Adjust the inputs so that they are in the same Q-factor Qi. The output is
    Qo=31. The numerical value returned is in the range (-1,1] (as a multiple
    of pi).

    The input pair represents a vector in the coordinate space. With both
    x, y >=0, atan2(y,x)=atan(y/x). In general,
      atan2(y,x)= +/- atan(|y/x|) +/- pi/2 * (0/1/2).

    Hence we project the vector onto the 1st quadrant to calculate atan(|y/x|),
    while the other coefficients used above are calculated from the signs of x
    & y. Once in the 1st quadrant, we need to go a step further & reflect the
    vector about y=x line if |y|>|x| as we can now narrow down the input range
    for atan(f) to f in [0,1] instead of f in [0,inf]..f=|y/x|. This change can
    be undone by flipping the sign & adding pi/2 i.e.
      angle_1st_quad = pi/2 -angle_1st_oct if |x|<|y|
                     = angle_1st_oct otherwise.

    atan(f) is approximated using a 7th degree polynomial approximation.
    The expression used for the complete inverse tranformation is as follows:

    Actual_angle= (1-2*s3)*angle_1st_oct + offset where,

      Actual_angle = atan2(y, x)/pi
      angle_1st_oct =atan2(|y/x|)/pi
      offset = .5*(2*is_y_ge_0-1)( is_ay_gt_ax + 2* !(is_ay_gt_ax|is_x_ge_0) )
      (Listed below are those variables derived from the sign of x & y.)
      s3 =  is_x_ge_0^is_y_ge_0^is_ay_gt_ax
      is_x_ge_0 = (x>=0)..as 0/1
      is_y_ge_0 = (y>=0)..as 0/1
      is_ay_gt_ax=(|x|<|y|)..as 0/1


    Example:
      input:y=-2147483648 x=-2147483648

      output: -1610611935 = 0xA0000321 =-7.499996e-01..Q31 (pi * -.75)

    @return
    A result corresponding to atan2(y,x)/pi.

    @note
    The optimized assembly function is 22 cycles.

    Maximum absolute relative error is 3.9419e-007.
*/
/*=========================================================================*/
extern int32 approx_atan2_poly7
(
  int32 y,   /*!< Input value */
  int32 x   /*!< Input value */
);


#ifdef __cplusplus
}
#endif

#endif /* APPROX_ATAN2_H */
