/*!
  @file
  approx_dB_to_ulin.h

  @brief
  Approximate dB_to_ulin() functions.
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
#ifndef APPROX_DB_TO_ULIN_H
#define APPROX_DB_TO_ULIN_H

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

  FUNCTION:  approx_dB_to_ulin_linlut8

===========================================================================*/
/*!
    @brief
    dB to linear conversion using linear 8-segment lookup table.

    @detail
    Given a signed input value( in dB..Q24), return an approximation of
    10^(input/10), and a scale factor.

    Details common to all approx_dB_to_ulin functions:
      The scale_factor is signed and lies in the range [-32,31].
      The scale_factor corresponds to the right shift amount in a 64-bit shift
      operation on the result & interpreting the resultant 64 bits
      as a Q32 number. For other output Q-factors, the scale_factor is to be
      adjusted appropriately (ensure no overflow/underflow).
      For e.g. Q16 result can be obtained w/o underflow for inputs in range
      -48.1647dB:127dB with right shift amount given by:
      right shift = scale_factor + 16.
  
      Considering only the positive input dB range upto 96.32dB, no shift is
      required if we treat the result as floating point. The Qo is given by:
      Qo = 32 + scale_factor..Qo is in the range [0,31]

      Valid input range is Q24 numbers corresponding to real values within
      +/-96.32 (dB range corresponding to linear 32-bit no. range). Inputs between
      96.33dB:127dB or -128dB:-96.33dB can be used, but the right shift for the
      result in 64 bits, now interpreted as Q21, is given by:
      right shift =scale_factor + sign(scale_factor)*11

      Consider, 10^y = 2^x with y = input/10
      => x = log2(10)*y = 3.321928*y
           = .3321928*input = i + f ..i is an integer & f is in [0,1)

      Thus, a power of 10 is converted to a power of 2, where the integer part is
      simply a bit-shift amount. Calculate 2^f using linear interpolation
      & bit-shift by 'i' to get the final result.

    Details specific to approx_dB_to_ulin_linlut8():
    The first 3 bits of f are used for a rough estimate of 2^f. A LUT with a
    3-bit index is used for this purpose. The fractional range is split into
    the 8 segments (hence the suffix 8): [0.0, 0.125),[0.125, 0.25),...,
    [0.875,1.0)

    The LUT contains c & m that satisfy the following equation:

    2^f = c + m*fn
    where,
      d       = .3321928*input  = i + f..Q25
      i       = integral part of d  = d[31:25] ..in range [-32,31]
      f       = fractional part of d= d[24:0]..Q25
      ind     = LUT index for the segment = d[24:22] in range [0, 7]
      fn      = d[21:7] in Q15..fraction left after extracting 'ind' from f
      c[ind] ~= 2^( ind/2^3 )..Q15
      m      ~= c[ind+1] - c[ind]..Q16

                     <-------f------------------->
    d:   ||  i      |  ind     | fn      |ignored ||
           31:25(7)   24:22(3)  21:7(15)  6:0(7)

    Final result:
    10^(input/10) = ( 2^f ) << i = (c + m*fn) << i

    Example:
    ||       input                  ||                  output               ||
    |      dB..Q24      | i  | f    || (scale, result)   |  2^f  |Final result|
-------------------------------------------------------------------------------
    -96.00dB=0xa0000000 =-32+ 0.1095 =>(31, 0x8a189ac0)  =1.0789 =2.511954e-10
    -16.00dB=0xf0000000 =-6 + 0.6849 =>(5, 0xcddfb7c0)   =1.6084 =2.513109e-02
    0.00dB=0x       0   =00 + 0.0000 =>(-1, 0x7ff00000)  =0.9995 =9.995117e-01
    16.00dB=0x10000000  =05 + 0.3151 =>(-6, 0x9f509220)  =1.2446 =3.982868e+01
    96.00dB=0x60000000  =31 + 0.8905 =>(-32, 0xed4130c0) =1.8536 =3.980473e+09

    @return
    A result corresponding to 10^(input/10).

    @note
    The optimized assembly function is 5 cycles.

    Maximum absolute relative error is 5.8121e-04
*/
/*=========================================================================*/
extern uint32_result_scale_t approx_dB_to_ulin_linlut8
(
  int32 input /*!< Input value */
);




/*===========================================================================

  FUNCTION:  approx_dB_to_ulin_cubefit

===========================================================================*/
/*!
    @brief
    dB to linear conversion using a single cubic polynomial fit.

    @detail
    Given a signed input value( in dB), return an approximation of
    10^(input/10), and a scale factor.

    Please refer the details common to all approx_dB_to_ulin functions in
    approx_dB_to_ulin_linlut8() prototype.

    The cubic polynomial fit for 2^f is as follows:

          c0       c1         c2           c3
    2^f = 0.9998 + 0.6968*f + 0.2242*f^2 + 0.078979*f^3
    where,
        d = .3321928*input  = i + f..Q16
        i = integral part of d  = d[31:16]..in range [-32,31]
        f = fractional part of d= d[15:0]..Q16

    d:   ||  i      |   f      ||
           31:16(16)   15-0(16)

    Final result:
    10^(input/10) = ( 2^f ) << i = (c0 + c1*f +c2*f^2 + c3*f^3) << i

    Example:
    ||       input                  ||                  output               ||
    |      dB..Q24      | i  | f    || (scale, result)   |  2^f  |Final result|
-------------------------------------------------------------------------------
    -96.00dB=0xa0000000 =-32+ 0.1095 =>(31, 0x8a1b2575)  =1.0790 =2.512134e-10
    -16.00dB=0xf0000000 =-6 + 0.6849 =>(5, 0xcdc7d1a6)   =1.6077 =2.511970e-02
    0.00dB=0x       0   =00 + 0.0000 =>(-1, 0x7ffc0000)  =0.9999 =9.998779e-01
    16.00dB=0x10000000  =05 + 0.3151 =>(-6, 0x9f405f29)  =1.2442 =3.981286e+01
    96.00dB=0x60000000  =31 + 0.8905 =>(-32, 0xed4d9ed2) =1.8539 =3.981287e+09

    @return
    A result corresponding to 10^(input/10).

    @note
    The optimized assembly function is 6 cycles.

    Maximum absolute relative error is 1.3463e-04
*/
/*=========================================================================*/
extern uint32_result_scale_t approx_dB_to_ulin_cubefit
(
  int32 input /*!< Input value */
);



/*===========================================================================

  FUNCTION:  approx_dB_to_ulin_quadlut8

===========================================================================*/
/*!
    @brief
    dB to linear conversion using 2nd degree 8-segment lookup table.

    @detail
    Given a signed input value( in dB), return an approximation of
    10^(input/10), and a scale factor.

    Please refer the details common to all approx_dB_to_ulin functions in
    approx_dB_to_ulin_linlut8() prototype.

    The first 3 bits of f are used to split the range [0,1) into 8 segments
    (hence the suffix 8):[0.0, 0.125),[0.125, 0.25),...,[0.875,1.0). 2^f is
    approximated by a cubic polynomial in each of these segments & a LUT with
    a 3 bit index( MSB=1 always for the norm. input) stores the coefficients.

    The LUT contains c's that satisfy the following equation in each segment:

    2^f = c0 + c1*fn + c2*fn^2
    where,
        d   = .3321928*input  = i + f..Q24
        i   = integral part of d  = d[31:24] ..in range [-32,31]
        f   = fractional part of d= d[23:0]..Q24
        ind = LUT index for the segment = d[23:21] in range [0, 7]
        fn  = d[20:0] in Q21..fraction left after extracting 'ind' from f

                     <-------f----------->
    d:   ||  i      |  ind     | fn       ||
           31:24(8)   23:21(3)  20:0(21)


    Final result:
    10^(input/10) = ( 2^f ) << i = (c0 + c1*fn + c2*fn^2) << i

    Example:
    ||       input                  ||                  output               ||
    |      dB..Q24      | i  | f    || (scale, result)   |  2^f  |Final result|
-------------------------------------------------------------------------------
    -96.00dB=0xa0000000 =-32+ 0.1095 =>(31, 0x8a17c28e)  =1.0789 =2.511894e-10
    -16.00dB=0xf0000000 =-6 + 0.6849 =>(5, 0xcdc60cfc)   =1.6076 =2.511885e-02
    0.00dB=0x       0   =00 + 0.0000 =>(-1, 0x8000184e)  =1.0000 =1.000003e+00
    16.00dB=0x10000000  =05 + 0.3151 =>(-6, 0x9f3e30a5)  =1.2441 =3.981073e+01
    96.00dB=0x60000000  =31 + 0.8905 =>(-32, 0xed4a2fbb) =1.8538 =3.981062e+09

    @return
    A result corresponding to 10^(input/10).

    @note
    The optimized assembly function is 7 cycles.

    Maximum absolute relative error is 4.4531e-06

*/
/*=========================================================================*/
extern uint32_result_scale_t approx_dB_to_ulin_quadlut8
(
  int32 input /*!< Input value */
);

#ifdef __cplusplus
}
#endif

#endif /* APPROX_DB_TO_ULIN_H */
