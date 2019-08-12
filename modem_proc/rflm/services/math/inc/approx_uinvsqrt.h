/*!
  @file
  approx_uinvsqrt.h

  @brief
  Approximate uinvsqrt() functions

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/math/inc/approx_uinvsqrt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef APPROX_UINVSQRT_H
#define APPROX_UINVSQRT_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

  FUNCTION:  approx_uinvsqrt_linlut8

===========================================================================*/
/*!
    @brief
    Approximate unsigned inverse-squareroot using linear 8-entry lookup 
    table.

    @detail
    Given an unsigned input & its Q-factor, return an approximation of 
    1/sqrt(input), and a scale factor.
    If input is 0, the output returned is result=0x7FFFFFFF, scale=1, i.e 0 
    is treated as input=1, Qi=0. Valid input range is 0-0xFFFFFFFF.

    In the output pair, the result is a 31U number while the scale lies in 
    [0,16]. The output Q-factor is Qo= 30 + scale - floor(Qi/2) (Qi=input 
    Q-factor). For Qo outside [0,32], visualize the result as being prefixed 
    (for Qo>32) or suffixed (for Qo<0) with 0s, to have a total of |Qo| bits 
    to the left or right respectively (the Q-pt would be at the left or the   
    right end too) of the 32-bit result's LSB.                             
    For e.g:                                                               
      1. For input=0xffffffff, Qi=32, the output is result~=0x40000000,
         scale=16. Input equals (2^32-1)/2^32 ~=1 & Qo=30+16-16=30. Thus 
         output equals 2^30/2^30 = 1, same as 1/sqrt(1).
         Now if Qi=0, input equals (2^32-1) and output is result~=0x40000000 
         & scale=16. Qo=30+16-0=46. Prepend (46-32=14 zeros) so result is 
         0.(14 0s)01(30 0s) = 2^-16 ~= 1/sqrt(2^32-1), the inverse 
         squareroot of the input.
                                                                           
      2. For input=1,Qi=1 the output is result=0x5A8240B7 & scale=0. The 
         input equals 1/2=.5 & Qo=30+0-0=30. The output= 1518485687/2^30 
         =1.4142, same as 1/sqrt(.5).  
         If Qi=64, the input is 2^-64 and the output is result=0x7fffffff & 
         scale=1. Qo=30+1-32=-1. Append 1 zero to the right to get result= 
         0(31 1s)0.0. Output=2*(2^31-1) ~= 2^32 = 1/sqrt(2^-64), same as the 
         inverse squareroot of the input.   


    Any 32-bit input can be normalized to the range [.5,1) by trimming the
    leading 0s & interpreting the remaining bits (0-padded to get 32-bits) as a
    Q32 number. The first 4 bits of the normalized input are used for a rough
    inverse estimate, which is stored in a LUT with a 3 bit index( MSB=1 always
    for norm. input). The norm. input range [.5,1) is thus split into 8
    segments.

    The LUT contains the pair (m,c) that satisfy the equation for inverse:

    1/sqrt(d) = c - m*dn
    where,
      d = normalized input in range [.5,1)
      c = 1/sqrt(d[31:28]) with the bit-field as a fraction (Q4).
      m = slope of the linear approximation for 1/sqrt(d) in each segment
      dn= d[27:13] in Q15..fraction in range [0,1]

    d:   ||1 |   ind     |  dn     | ignored   ||
           31    30-28(3)  27-13(15)  12-0(13)

    The 'c' entries are skewed to give symmetrical error at either extreme of
    each segment. 


    Example:
      input: 3.5625 (57 in Q4)
      output: scale=3, result=0x43D27B5C =1137867612 (31U result)
              Qo=30+3-(4/2)=31 =>output=1137867612/2^31=0.5299=1/sqrt(3.5625)

    @return
    A result/scale pair corresponding to 1/sqrt(input). The result is 31U 
    while the scale is in [0,16]

    @note
    The optimized assembly function is 6 cycles.

    Maximum absolute relative error is 6.678e-4.
*/
/*=========================================================================*/

extern uint32_result_scale_t approx_uinvsqrt_linlut8
(
  uint32 input,  /*!< Input value */
  int32 Qi  /*!< Input Q-factor */
);


/*===========================================================================

  FUNCTION:  approx_uinvsqrt_linlut_q4

===========================================================================*/
/*!
    @brief
    Approximate unsigned inverse-squareroot using a linear lookup table.

    @detail
    This function is a bit-exact equivalent to qdsp4_inv_sqrt_lut used in 
    QDSP4 applications.

    Implementation Notes:
                                                                 
    In order to minimize the mean squared error, we try to  find the
    optimal Lut entries. A line can be drawn through each pair of
    consecutive entries, forming an approximation to sqrt(x) for that range. 
    It can be shown that the optimal linear approximation is the one that
    minimizes the mean squared error between the actual sqrt(x) curve and
    its piece-wise linear approximation. Each of these pieces are
    polynomials of degree 1 of the form y(i)=a(i)x + b(i).       

    @return
    1/sqrt(input) in Q30 format.
    if input == 0, returns 0

    @note
    The optimized assembly function is 7 cycles.

*/
/*=========================================================================*/
extern uint32 approx_uinvsqrt_linlut_q4
(
  uint32 input,      /*!< Input value in Q0 */
  uint32 rnd_factor  /*!< Rounding Factor */
);
    
#ifdef __cplusplus
}
#endif

#endif /* APPROX_UINVSQRT_H */

