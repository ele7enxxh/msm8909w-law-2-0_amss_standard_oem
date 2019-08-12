#ifndef RFCOMMON_MATH_H
#define RFCOMMON_MATH_H

/*!
  @file
  rfcommon_math.h

  @brief
  Common math routines used by the RF driver.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_math.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/12/13   shb     Added rf_lininterp_s16_s16
04/08/13   aro     Moved inline function to header file
04/03/13   aro     Off-target compilation fix
04/02/13   aro     Added inline Math functions
01/15/13   cd      Added support for 64-bit math operations
10/22/12   aro     Changes the type to UINT64
08/24/12   cd      CDMA EPT Temp and HDET feature support
07/19/12   aro     Function to extract the location of first 1-bit LSB
                   location
07/19/12   aro     Function to extract the location of first 1-bit MSB
                   location
06/12/12   id      Added rf_lininterp_s16_u16
05/22/12   id      Added rf_lininterp_s16_u32 and x_is_sorted_s16
02/27/12   swb     Export x_is_sorted_u32
01/26/12   sty     Moved rf_retrieve_notch_entries to HAL
                   deleted unused rf_notch_table_type_s
01/16/12   sty     Added rf_retrieve_notch_entries()
11/02/11   hdz     Added rf_lininterp_u16_s16()
                   Added rf_lininterp_u16_s8()
07/21/11   aro     Added rf_extract_first_one_bit_mask()
07/20/11   aro     Added utility function to count number of_one_bits
                   in a given number
07/12/11   bmg     Added 8-bit signed interpolation.
03/25/11   aro     Added Math function to calculate Min and Max
03/24/11   bmg     Made all inputs const, to avoid any const-ness warnings
02/10/11   bmg     Initial Release.

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Extract a mask containing only the first 1-bit from LSB
 
  @details
  This function extracts a mask containing only the first 1-bit from LSB. For
  example, if the number under consideration is 01110100, then the mask
  returned will be 000001000.
 
  @param x
  Number under consideration from which the mask is to be generated.
 
  @return
  First one-bit mask
*/
INLINE uint32
inline_rf_extract_first_one_bit_mask
(
  uint32 x
)
{
  uint32 y;

  /* Take 2's complement */
  y = (~x) + 1;

  /* AND the 2's complement value with original value to get the mask
  containing only the first 1-bit (from LSB)*/
  y = y & x;

  return y;

} /* inline_rf_extract_first_one_bit_mask */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Extract a mask containing only the first 1-bit from LSB
 
  @details
  This function extracts a mask containing only the first 1-bit from LSB. For
  example, if the number under consideration is 01110100, then the mask
  returned will be 000001000.
 
  @param x
  Number under consideration from which the mask is to be generated.
 
  @return
  First one-bit mask
*/
INLINE uint64
inline_rf_extract_first_one_bit_mask_uint64
(
  uint64 x
)
{
  uint64 y;

  /* Take 2's complement */
  y = (~x) + 1;

  /* AND the 2's complement value with original value to get the mask
  containing only the first 1-bit (from LSB)*/
  y = y & x;

  return y;

} /* inline_rf_extract_first_one_bit_mask_uint64 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Extract the location of first 1-bit from LSB
 
  @details
  This function will count the location of 1-bit from LSB
 
  @param x
  64-bit number whose first 1-bit LSB location is to be found
 
  @return
  First 1-bit LSB locaiton of the number
*/
INLINE uint8
inline_rf_extract_first_one_bit_lsb_location
(
  uint64 x
)
{

  uint8 a = 0;
  uint64 x_temp = 0;
  uint64 x_temp2 = 0;

  /* Find the 1-bit LSB mask */

  /* Take 2's complement */
  x_temp = (~x) + 1;

  /* AND the 2's complement value with original value to get the mask
  containing only the first 1-bit (from LSB)*/
  x_temp = x_temp & x;

  x_temp2 = x_temp;

  if (!x_temp2)
  {
    return -1;
  }
 
  #define step(b) if (x_temp2 >= ((uint64)1) << b) a += b, x_temp2 >>= b
  step(32);step(16); step(8); step(4); step(2); step(1);
  #undef step

  return a;

} /* inline_rf_extract_first_one_bit_lsb_location */


/*!
  @defgroup RF_COMMON_MATH Common RF Driver math routines
  @{
*/

/*!
  @name Numeric limiting

  @details
  These functions are used to limit a value to bounded minimum and
  maximum values.

  The function used should be matched with the data type that is
  being limited.
  @{
*/

/*----------------------------------------------------------------------------*/
int32
rf_limit_s32
(
  const int32 x,
  const int32 min,
  const int32 max
);

/*----------------------------------------------------------------------------*/
uint32
rf_limit_u32
(
  const uint32 x,
  const uint32 min,
  const uint32 max
);

/*----------------------------------------------------------------------------*/
int64
rf_limit_s64
(
  const int64 x,
  const int64 min,
  const int64 max
);

/*!
  @}
*/

/*!
  @name Linear Interpolation

  @details
  These functions are used to estimate an output value by
  linearily interpolating between the two nearest data points
  in a table of independent and dependent variables.  For data
  beyond the ends of the table, the value at the nearest endpoint
  is returned.

  The function used should be matched with the data type that is
  being interpolated, and the format of the data tables.
  @{
*/

/*----------------------------------------------------------------------------*/
int16
rf_lininterp_u32_s16
(
  const uint32 x[],
  const int16  y[],
  const int    N,
  const uint32 unknown
);

/*----------------------------------------------------------------------------*/
uint16
rf_lininterp_u32_u16
(
  const uint32 x[],
  const uint16 y[],
  const int    N,
  const uint32 unknown
);

/*----------------------------------------------------------------------------*/
int8
rf_lininterp_u32_s8
(
  const uint32 x[],
  const int8   y[],
  const int    N,
  const uint32 unknown
);

/*----------------------------------------------------------------------------*/
int8
rf_lininterp_u16_s8
(
  const uint16 x[],
  const int8   y[],
  const int    N,
  const uint16 unknown
);

/*----------------------------------------------------------------------------*/
int16
rf_lininterp_u16_s16
(
  const uint16 x[],
  const int16  y[],
  const int    N,
  const uint16 unknown
);

/*----------------------------------------------------------------------------*/
uint32
rf_lininterp_s16_u32
(
  const int16 x[],
  const uint32  y[],
  const int    N,
  const int16 unknown
);

/*----------------------------------------------------------------------------*/
uint16
rf_lininterp_s16_u16
(
  const int16 x[],
  const uint16  y[],
  const int    N,
  const int16 unknown
);

/*----------------------------------------------------------------------------*/
int16
rf_lininterp_s16_s16
(
  const int16 x[],
  const int16  y[],
  const int    N,
  const int16 unknown
);

/*!
  @}
*/

/*----------------------------------------------------------------------------*/
int32
rf_divide_with_rounding
(
  const int32 numerator,
  const int32 denominator
);

/*----------------------------------------------------------------------------*/
int64
rf_divide_with_rounding_64
(
  const int64 numerator,
  const int64 denominator
);

/*----------------------------------------------------------------------------*/
int16
rf_max_s16
(
  const int16 x,
  const int16 y
);

/*----------------------------------------------------------------------------*/
int16
rf_min_s16
(
  const int16 x,
  const int16 y
);

/*----------------------------------------------------------------------------*/
uint8
rf_count_one_bits
(
  uint32 x
);

/*----------------------------------------------------------------------------*/
uint8
rf_count_one_bits_uint64
(
  uint64 x
);

/*----------------------------------------------------------------------------*/
uint32
rf_extract_first_one_bit_mask
(
  uint32 x
);

/*----------------------------------------------------------------------------*/
uint64
rf_extract_first_one_bit_mask_uint64
(
  uint64 x
);

/*----------------------------------------------------------------------------*/
uint8
rf_extract_first_one_bit_msb_location
(
  uint64 x
);


/*----------------------------------------------------------------------------*/
uint8
rf_extract_first_one_bit_lsb_location
(
  uint64 x
);

/*----------------------------------------------------------------------------*/
boolean
x_is_sorted_u32
(
  const uint32 x[],
  const int N
);

/*----------------------------------------------------------------------------*/
int64
rf_compute_exp10
(
  int32 x
);

#ifdef __cplusplus
}
#endif

#endif  /* RFCOMMON_MATH_H */

