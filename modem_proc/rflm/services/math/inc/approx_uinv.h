/*!
  @file
  approx_uinv.h

  @brief
  Approximate uinv() functions.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/math/inc/approx_uinv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef APPROX_UINV_H
#define APPROX_UINV_H

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

  FUNCTION:  approx_uinv_linlut8

===========================================================================*/
/*!
    @brief
    Approximate unsigned-inverse using linear 8-entry lookup table.

    @detail
    Given an unsigned input, return an approximation of 1/input, and a scale
    factor.
    The scale factor facilitates a variable input Q-factor.
    If input is 0, the output returned is result=0x7FFFFFFF, scale=0 i.e 0 
    is treated as a 1. Valid input range is 0-0xFFFFFFFF.

    Common explanation for the output of ...linlut8(), ...cubelut8() and 
    ...newt():-
      In the output pair, the result is a 31U number while the scale lies in 
      [0,31]. The output Q-factor is Qo= 31 + scale - Qi (Qi=input Q-factor). 
      For Qo outside [0,32], visualize the result as being prefixed (for 
      Qo>32) or suffixed (for Qo<0) with 0s, to have a total of |Qo| bits to 
      the left or right respectively (the Q-pt would be at the left or the 
      right end too) of the 32-bit result's LSB. 
      For e.g: 
        1. For input=0xffffffff, result ~=0x40000000, and scale=31 always. 
           If Qi=32, input numerically equals to (2^32-1)/2^32 ~=1 and 
           Qo=31+31-32=30. Thus output equals ~2^30/2^30=1. 
           Now if Qi=0, input equals (2^32-1) and Qo=31+31-0=62. Prepend 
           (62-32=30 zeros) so result is 0.(30 0s)01(30 0s) ~=2^30/2^62 
           = 2^-32 ~= 1/(2^32-1), the reciprocal of the input. 

        2. Input=0x1 gives result=0x7fffffff and scale=0 always. With Qi=0, 
           input equals 1 and Qo=31+0-0=31. The output= (2^31-1)/2^31 ~=1. 
           If Qi=32, then input equals 2^-32 and Qo=31+0-32=-1. 
           Append 1 zero to the right to get result= 0(31 1s)0.0 
           Output=2*(2^31-1) ~= 2^32, same as the reciprocal of the input.


    Input normalization details common to linlut8, cubelut8 and newt:-
      Any 32-bit input can be normalized to the range [.5,1) by trimming the
      leading 0s & interpreting the remaining bits (0-padded to get 32-bits) 
      as a Q32 number. The first 4 bits of the normalized input are used for 
      a rough inverse estimate, which is stored in a LUT with a 3 bit index 
      (MSB=1 always for norm. input). The norm. input range [.5,1) is thus 
      split into 8 segments.


    approx_uinv_linlut8() specific details:-
    The LUT contains the pair (m,c) that satisfy the equation for inverse:

    1/d = c - m*dn
    where,
      d = normalized input in range [.5,1)
      c = inverse of d[31:28] in Q14, treating d[31:28] as a fraction (Q4).
      m = slope of the linear approximation for 1/d in each segment, in Q15.
      dn= d[27:13] in Q15..fraction in range [0,1]

    d:   ||1 |   ind     |  dn     | ignored   ||
         31    30-28(3)  27-13(15)  12-0(13)

    The 'c' entries are skewed to give symmetrical error at either extreme of
    each segment. For e.g. in the 1st row, c=32690 ~= 16/8=32768(Q14)


    Example:
      input: 3.5625 (57 in Q4)
      output: scale=5, result=0x47dd8000=1205698560 (31U result)
              Qo=31+5-4=32 => output =1205698560/2^32 = 0.280724= 1/3.5625


    @return
    A result/scale pair corresponding to 1/input. The result is 31U while 
    the scale is in [0,31]

    @note
    The optimized assembly function is 5 cycles.

    Maximum absolute relative error is 1.8e-3.
*/
/*=========================================================================*/
extern uint32_result_scale_t approx_uinv_linlut8
(
  uint32 input  /*!< Input value */
);






/*===========================================================================

  FUNCTION:  approx_uinv_cubelut8

===========================================================================*/
/*!
    @brief
    Approximate unsigned-inverse using cubic polynomial approximation.

    @detail
    Given an unsigned input, return an approximation of 1/input, and a scale
    factor.
    The scale factor facilitates a variable input Q-factor.
    If input is 0, the output returned is result=0x7FFFFFFF, scale=0 i.e 0 
    is treated as a 1. Valid input range is 0-0xFFFFFFFF.

    Please refer the common explanations for the input normalization and 
    the output from the approx_uinv_linlut8() prototype details.

    approx_uinv_cubelut8() specific details:-
    1/d = c0 + c1*dn + c2*dn^2 + c3*dn^3 
    where,
      d = normalized input in range [.5,1)
      dn= fraction left after extracting the LUT index for the segment from d
          dn=d[27:0] in Q28

    d:   ||1 |   ind     |  dn     ||
           31    30-28(3)  27-0(28)

    Example:
      input: 3.5625 (57 in Q4)
      output: scale=5, result=0x47dc5800=1205622784 (31U result)
              Qo=31+5-4=32 => output =1205622784/2^32 = 0.280706= 1/3.5625

    @return
    A result/scale pair corresponding to 1/input. The result is 31U while 
    the scale is in [0,31].

    @note
    The optimized assembly function is 7 cycles.

    Maximum absolute relative error is 1.525879e-5.
*/
/*=========================================================================*/
uint32_result_scale_t approx_uinv_cubelut8
(
  uint32 input  /*!< Input value */
);





/*===========================================================================

  FUNCTION:  approx_uinv_newt

===========================================================================*/
/*!
    @brief
    Approximate unsigned-inverse using Newton-Raphson iterative method.

    @detail
    Given an unsigned input, return an approximation of 1/input, and a scale
    factor.
    The scale factor facilitates a variable input Q-factor.
    If input is 0, the output returned is result=0x7FFFFFFF, scale=0 i.e 0 
    is treated as a 1. Valid input range is 0-0xFFFFFFFF.
    The iteration count must be in [1,3].

    Please refer the common explanations for the input normalization and 
    the output from the approx_uinv_linlut8() prototype details.

    approx_uinv_newt() specific details:-
    Newton's recursive equation for the solution of f(x)=1/x-d is
    x[n+1] =  2*x[n] - d*x[n]^2 where,
      d = normalized input in range [.5,1)
      x[n]= current solution of 1/x -d = 0

    The first approximation for inverse is x[0]=2.9142-2*d, which tracks actual
    inverse 1/d closely, for d in [.5,1).

    Newton's modified iterative solution for 1/d:
    x^2 loses 1 bit compared to x, for e.g x in Q31, x^2 is Q30. d*x^2 loses an
    additional bit as d is Q31. To keep the bit loss to a predictable 1 bit per
    iteration, modify the equation as
    x[n+1]=2*x[n] - d*x[n]*(x[n]<<n).
    With the 2nd term in Q-1, x[n] in Q can be interpreted as 2*x[n] in Q-1
    giving x[n+1] in Q-1, avoiding additional shift operations in the loop.
    Thus, x will be down to Q=31-'iterations' at the end.

    Loop instability:
    If x becomes >=2.0 in a stage (Q is variable in the loop), xp will overflow
    32 bits in the next iteration and the 2nd term becomes negligible compared
    to x. MSBs of x will get stuck at their value and we wont see the expected
    degradation in Q of x. Interpreting this result as Q=Q31-'iterations' is
    then incorrect.
    The solution is to signed saturate the result after a left shift of
    'iterations'-1. This gives the result in Q30, which saturates at 0x7FFFFFFF
    =1.999(Q30) and we get a stable result.


    Example:
      input: 3.5625 (57 in Q4)
      output: scale=5, result=0x47dc11f8=1205604856 (31U result)
              Qo=31+5-4=32 => output =1205604856/2^32 = 0.280702= 1/3.5625

    @return
    A result/scale pair corresponding to 1/input. The result is 31U while 
    the scale is in [0,31].

    @note
    The optimized assembly function is 4+2*(iterations) cycles.

    Maximum absolute relative error : 7.3e-3  6E-5  5.5E-9
                          iterations: 1       2     3
                              cycles: 6       8     10
    Quantization noise makes error worse at more than 3 iterations.
*/
/*=========================================================================*/
extern uint32_result_scale_t approx_uinv_newt
(
  uint32 input,/*!< Input value */
  uint32 iter  /*!< Iterations */
);




/*===========================================================================

  FUNCTION:  approx_uinv_tylr

===========================================================================*/
/*!
    @brief
    Approximate unsigned-inverse using Taylor's series expansion.

    @detail
    Given an unsigned input and an iteration count, return an approximation of
    1/input, and a scale factor. 
    The iteration count refers to the highest power accumulated in the Taylor 
    series approximation.
    If input is 0, the output returned is result=0xFFFFFFFF, scale=1 .i.e 0 
    is treated as a 1. Valid input range is 0-0xFFFFFFFF.
    The iteration count must be in [3,8].

    Please refer the common explanations for the input normalization and 
    the output from the approx_uinv_linlut8() prototype details. The only 
    difference here is that scale is in [1,32] & the result is a 32U number.

    approx_uinv_tylr() specific details:-
    Taylor's expansion of f(x)=1/(1-x)..|x| < 1:
        f(x)=1+x+x^2+x^3+..ad.inf
        Above is truncated to a fixed length=1+'iterations'. The noise added
        is directly proportional to |x|^(1+'iterations').
    Let a=1-d*x  ..lim a->0
    => d*x = 1-a  OR
    1/d = x/(1-a)
        = x*(1+a+a^2+a^3+..a^hi_pow) where,
      d = normalized input in [.5,1)
      x = init. approx for 1/d,

    Choice of initial approximation:
    The first approximation for inverse is x=2.9142-2*d, as seen in Newton's
    method,  but this gives signed result for a=1-d*x which cannot be
    accomodated in a Q32 unsigned accumulator w/o additional shift operations.
    Hence choose x=2.8283-2*d, which gives unsigned a=1-d*x but at the expense
    of eror symmetry at two extremes of range of d. Hence for symmetric error,
    use x = 2.666 - 1.7778*d,
    which gives same value for a=1-d*x at either extremes .5 and 1


    Example:
      input: 3.5625 (57 in Q4)
      output: scale=6, result=0x8fb8235d=2411209565 (32U result)
              Qo=31+6-4=33 => output =2411209565/2^33 = 0.280702= 1/3.5625
              NOTE: The result is twice the magnitude, compared to other 
              algorithms and so the scale is also 1 more.
    @return
    A result/scale pair corresponding to 1/input. The result is 32U while 
    the scale is in [1,32].

    @note
    The optimized assembly function is 6+(hi_pow) cycles.

    Max absolute relative error: 1.6-4  1.8e-5  2.0e-6  2.5e-7  3.5e-8  3.5e-9
                  highest power: 3      4       5       6       7       8
                         cycles: 9      10      11      12      13      14
    Competitive only for hi_pow=4 but 2 cycles more than ...newt(XX,iter =2) 
    and 3 more than ...cube8lut()

*/
/*=========================================================================*/
extern uint32_result_scale_t approx_uinv_tylr
(
  uint32 input,/*!< Input value */
  uint32 hi_pow /*!< highest power in Taylor's expansion */
);   

#ifdef __cplusplus
}
#endif

#endif /* APPROX_UINV_H */
