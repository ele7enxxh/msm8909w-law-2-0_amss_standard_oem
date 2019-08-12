/*!
  @file
  approx_ulin_to_dB.h

  @brief
  Approximate ulin_to_dB() functions.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/math/inc/approx_ulin_to_dB.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef APPROX_ULIN_TO_DB_H
#define APPROX_ULIN_TO_DB_H

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

  FUNCTION:  approx_ulin_to_dB_linlut8

===========================================================================*/
/*!
    @brief
    Linear range to dB conversion using linear 8-segment lookup table.

    @detail
    Given an unsigned input and the its Q-factor, return an approximation
    of 10*log10(input).
    Valid input range is 0 to 0xFFFFFFFF & Q-factor from 0 to 32 ( Q-factors
    outside this range should be checked for overflow/underflow before use).

    The output Q-factor is fixed at Q24. Valid output is in the range
    +/-96.32dB.

    If input is 0, the output returned is 0x80000000=-128dB in Q24.

    Any 32-bit input can be normalized to the range [1, 2) by trimming the
    leading 0s & interpreting the remaining bits (0-padded to get 32-bits)as a
    Q31 number. The first 4 bits of the normalized input are used for a rough
    estimate of 10*log10(norm. input). A LUT with a 3 bit index( MSB=1 always 
    for the norm. input) stores these values. The input range is split into the 
    8 segments (hence the suffix 8):
    [1.0, 1.125),[1.125, 1.25),...,[1.875,2.0)

    The LUT contains c's that satisfies the following equation:
    10*log10(d) = c[ind+1]*dn  + c[ind]*(1-dn) where,
        d       = normalized input in range [1,2)
        ind     = LUT index for the segment = d[30:28] in range [0, 7]
        c[ind] ~= 10*log10( 1+ ind/2^3 ) in Q12
        dn      = d[27:0] in Q28..fraction left after extracting 'ind' from d

    d:   ||1 |   ind     |  dn     ||
           31    30-28(3)  27-0(28)

    The actual 'c' entries have been skewed for symmetric error in each segment.

    Final result:
    10*log10(input) = 10*log10( d*2^-(Qi+cl0-31) )
                    = 10*log10(d) - 3.0103*(Qi+cl0-31)
                    = c[ind+1]*dn  + c[ind]*(1-dn) - 3.0103*(Qi+cl0-31)
                    The last term is the dB offset due to the relative scale
                    difference between the real value and norm. range [1,2) i.e.
                    its 0 for real value of input in [1,2)

    Example:
               input         =>    output..Q24
    --------------------------------------------------
    0xffffffff..Q0= 4.29e+09 	 0x6054bd9b = 96.33102 dB
    0xffffffff..Q32= 1.00e+00 	 0x00005cfb = 0.00142 dB
    0x0000001f..Q0= 3.10e+01 	 0x0ee9a414 = 14.91266 dB
    0x00000001..Q0= 1.00e+00 	 0x00010000 = 0.00391 dB
    0x000000ff..Q16= 3.89e-03 	 0xe7e6c7d3 = -24.09851 dB
    0x00000001..Q32= 2.33e-10 	 0x9fac9f60 = -96.32569 dB

    @return
    A result corresponding to 10*log10(input).

    @note
    The optimized assembly function is 6 cycles.

    Maximum absolute error is less than 0.004012 dB
*/
/*=========================================================================*/
extern int32 approx_ulin_to_dB_linlut8
(
  uint32 input,  /*!< Input value */
  int32 Qi      /*!< Input Q-factor */
);

       

/*===========================================================================

  FUNCTION:  approx_ulin_to_dB_cubelut8

===========================================================================*/
/*!
    @brief
    Linear range to dB conversion using cubic 32-segment lookup table.

    @detail
    Given an unsigned input and the its Q-factor, return an approximation
    of 10*log10(input).

    Valid input range is 0 to 0xFFFFFFFF & Q-factor from 0 to 32 ( Q-factors
    outside this range should be checked for overflow/underflow before use).

    The output Q-factor is fixed at Q24. Valid output is in the range
    +/-96.32dB.

    If input is 0, the output returned is 0x80000000=-128dB in Q24.

    Any 32-bit input can be normalized to the range [1, 2) by trimming the
    leading 0s & interpreting the remaining bits (0-padded to get 32-bits)as a
    Q31 number. The normalized input range [1,2) is split into 8 segments using 
    3 MSBs after the 1st (MSB=1 always for norm. input). The corresponding dB 
    value is approximated by a cubic polynomial in each segment as follows:

    10*log10(d) = c0 + c1*dn + c2*dn^2 + c3*dn^3
    where,
        d = normalized input in range [1,2)
        dn= d[27:2] in Q26..fraction left after extracting 'ind' from d

    d:   ||1 |   ind     |  dn     |ignored ||
          31    30-28(3)  27-2(26)  1-0(2)

    Final result:
    10*log10(input) = 10*log10( d*2^-(Qi+cl0-31) )
                    = 10*log10(d) - 3.0103*(Qi+cl0-31)
                    = c0 + c1*dn + c2*dn^2 + c3*dn^3 - 3.0103*(Qi+cl0-31)
                    The last term is the dB offset due to the relative scale
                    difference between the real value and norm. range [1,2) i.e.
                    its 0 for real value of input in [1,2)

    Example:
               input         =>    output..Q24
    --------------------------------------------------
    0xffffffff..Q0= 4.29e+09 	 0x60546034 = 96.32959 dB
    0xffffffff..Q32= 1.00e+00 	 0xffffff94 = -0.00001 dB
    0x0000001f..Q0= 3.10e+01 	 0x0ee9e18c = 14.91360 dB
    0x00000001..Q0= 1.00e+00 	 0x00000000 = 0.00000 dB
    0x000000ff..Q16= 3.89e-03 	 0xe7e68d5a = -24.09941 dB
    0x00000001..Q32= 2.33e-10 	 0x9fab9f60 = -96.32960 dB

    @return
    A result corresponding to 10*log10(input).

    @note
    The optimized assembly function is 7 cycles.

    Maximum absolute error is less than 0.0000483 dB
*/
/*=========================================================================*/
extern int32 approx_ulin_to_dB_cubelut8
(
  uint32 input,  /*!< Input value */
  int32 Qi      /*!< Input Q-factor */
);                                    

#ifdef __cplusplus
}
#endif

#endif /* APPROX_ULIN_TO_DB_H */
