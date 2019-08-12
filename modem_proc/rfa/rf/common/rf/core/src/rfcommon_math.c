/*! 
  @file
  rfcommon_math.c
 
  @brief
  Mathematical functions that are used in the RF driver
 
  @addtogroup RF_COMMON
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_math.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/15/14   cd/hdz  Fixed compiler warning
06/12/13   shb     Added rf_lininterp_s16_s16
04/08/13   aro     Moved inline function to header file
04/03/13   aro     Off-target compilation fix
04/02/13   aro     Added inline Math functions
01/15/13   cd      Added support for 64-bit math operations
11/07/12   cd      Fixed off-target compiler error caused due to merge
10/22/12   aro     Changes the type to UINT64
08/24/12   cd      CDMA EPT Temp and HDET feature support
07/19/12   aro     Function to extract the location of first 1-bit LSB
                   location
07/19/12   aro     Function to extract the location of first 1-bit MSB
                   location
06/12/12   id      Added rf_lininterp_s16_u16
05/22/12   id      Added rf_lininterp_s16_u32 and x_is_sorted_s16
02/27/12   swb     Export x_is_sorted_u32
01/26/12   sty     Moved rf_retrieve_notch_entries() to HAL
01/16/12   sty     Added rf_retrieve_notch_entries() 
11/02/11   hdz     Added rf_lininterp_u16_s16()
11/02/11   hdz     Added rf_lininterp_u16_s8()
07/21/11   aro     Added rf_extract_first_one_bit_mask()
07/20/11   aro     Added utility function to count number of_one_bits
                   in a given number
07/12/11   bmg     Added 8-bit signed interpolation.
03/25/11   aro     Added Math function to calculate Min and Max
03/24/11   bmg     Made all inputs const, to avoid any const-ness warnings
02/22/11   bmg     Fixed limit parameter documentation
                   Reduced number of arithmetic operations in interpolation
02/18/11   sty     Added doxygen Group delimiter
02/14/11   bmg     Added rf_lininterp_u32_u16
02/14/11   bmg     Added N=1 handling for linear interpolation
02/10/11   bmg     Initial Release.
 
==============================================================================*/

#include "comdef.h"
#include "rfcommon_math.h"
#include "err.h"
#include "rfcommon_msg.h"
#ifndef T_WINNT
#include "q6protos.h"
#endif

#define INVLOG2Q13               0x6a4dLL
#define EXP10C0                  0x7fbdLL
#define EXP10C1                  0x556fLL
#define EXP10C2                  0x15efLL

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Performs integer division with rounding to the nearest integer.

  @details
  Rounding is performed correctly based on the sign of the result.

  A fatal error is thrown if attempting to divide by zero.

  @param numerator
  The numerator.

  @param denominator
  The denominator.  Must not be zero.

  @return
  The integer result of numerator / denominator, rounded to the nearest
  integer.
*/
int32
rf_divide_with_rounding
(
  const int32 numerator,
  const int32 denominator
)
{
  if ( denominator == 0 )
  {
    ERR_FATAL( "Divide by zero", 0, 0, 0 );
  }

  if (    (numerator > 0 && denominator > 0)
       || (numerator < 0 && denominator < 0) )
  {
    return (numerator + denominator / 2) / denominator;
  }
  else
  {
    return (numerator - denominator / 2) / denominator;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Performs integer division with rounding to the nearest integer.

  @details
  Rounding is performed correctly based on the sign of the result.

  A fatal error is thrown if attempting to divide by zero.

  @param numerator
  The numerator.

  @param denominator
  The denominator.  Must not be zero.

  @return
  The integer result of numerator / denominator, rounded to the nearest
  integer.
*/
int64
rf_divide_with_rounding_64
(
  const int64 numerator,
  const int64 denominator
)
{
  if ( denominator == 0 )
  {
    ERR_FATAL( "Divide by zero", 0, 0, 0 );
  }

  if (    (numerator > 0 && denominator > 0)
       || (numerator < 0 && denominator < 0) )
  {
    return (numerator + denominator / 2) / denominator;
  }
  else
  {
    return (numerator - denominator / 2) / denominator;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Bounds the input value by the minimum and maximum value allowed.

  @details
  This function is used when placing minimum and maximum bounds on a
  variable which is a signed quantity.

  @param x
  The quantity to bound by the min and max

  @param min
  The smallest value that x is allowed to take.

  @param max
  The largest value that x is allowed to take.

  @return
  If x is less than min, returns min.  If x is greather than max,
  returns max.  Otherwise returns x.
*/
int32
rf_limit_s32
(
  const int32 x,
  const int32 min,
  const int32 max
)
{
  if ( max < min )
  {
    ERR_FATAL( "rf_limit_s32 max (%d) is less than min (%d)", max, min, 0 );
  }

  if ( x < min )
  {
    return min;
  }
  else if ( x > max )
  {
    return max;
  }
  else
  {
    return x;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Bounds the input value by the minimum and maximum value allowed.

  @details
  This function is used when placing minimum and maximum bounds on a
  variable which is a signed quantity.

  @param x
  The quantity to bound by the min and max

  @param min
  The smallest value that x is allowed to take.

  @param max
  The largest value that x is allowed to take.

  @return
  If x is less than min, returns min.  If x is greather than max,
  returns max.  Otherwise returns x.
*/
int64
rf_limit_s64
(
  const int64 x,
  const int64 min,
  const int64 max
)
{
  if ( max < min )
  {
    ERR_FATAL( "rf_limit_s64 max (%d) is less than min (%d)", max, min, 0 );
  }

  if ( x < min )
  {
    return min;
  }
  else if ( x > max )
  {
    return max;
  }
  else
  {
    return x;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Bounds the input value by the minimum and maximum value allowed.

  @details
  This function is used when placing minimum and maximum bounds on a
  variable which is an unsigned quantity.

  @param x
  The quantity to bound by the min and max.

  @param min
  The smallest value that x is allowed to take.

  @param max
  The largest value that x is allowed to take.

  @return
  If x is less than min, returns min.  If x is greather than max,
  returns max.  Otherwise returns x.
*/
uint32
rf_limit_u32
(
  const uint32 x,
  const uint32 min,
  const uint32 max
)
{
  if ( max < min )
  {
    ERR_FATAL( "rf_limit_u32 max (%d) is less than min (%d)", max, min, 0 );
  }

  if ( x < min )
  {
    return min;
  }
  else if ( x > max )
  {
    return max;
  }
  else
  {
    return x;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Linear interpolation helper function to check that the input x-coordinate
  array is in the proper sorted format.

  @details
  Verifies that an array of 32-bit unsigned integers is monotonically
  strictly increasing (no duplicates) or strictly decreasing.

  @param x
  The array of 32-bit unsigned values to check for sorted order.

  @param N
  The number of items in the array x.

  @return
  TRUE if the array is sorted, FALSE otherwise.
*/
boolean
x_is_sorted_u32
(
  const uint32 x[],
  const int N
)
{
  int i;

  if ( N < 2 )
  {
    ERR_FATAL( "x_is_unsorted_u32: Array size is too small %d", N, 0, 0 );
  }

  if ( x[0] < x[N-1] )
  {
    for ( i = 0; i < N - 1; i++ )
    {
      if ( x[i] >= x[i+1] )
      {
        return FALSE;
      }
    }
  } /* if ( x[0] < x[N-1] ) */
  else
  {
    for ( i = 0; i < N - 1; i++ )
    {
      if ( x[i] <= x[i+1] )
      {
        return FALSE;
      }
    }
  } /* else: if ( x[0] < x[N-1] ) */

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Linear interpolation helper function to check that the input x-coordinate
  array is in the proper sorted format.

  @details
  Verifies that an array of 16-bit unsigned integers is monotonically
  strictly increasing (no duplicates) or strictly decreasing.

  @param x
  The array of 16-bit unsigned values to check for sorted order.

  @param N
  The number of items in the array x.

  @return
  TRUE if the array is sorted, FALSE otherwise.
*/
static
boolean
x_is_sorted_u16
(
  const uint16 x[],
  const int N
)
{
  int i;

  if ( N < 2 )
  {
    ERR_FATAL( "x_is_unsorted_u16: Array size is too small %d", N, 0, 0 );
  }

  if ( x[0] < x[N-1] )
  {
    for ( i = 0; i < N - 1; i++ )
    {
      if ( x[i] >= x[i+1] )
      {
        return FALSE;
      }
    }
  } /* if ( x[0] < x[N-1] ) */
  else
  {
    for ( i = 0; i < N - 1; i++ )
    {
      if ( x[i] <= x[i+1] )
      {
        return FALSE;
      }
    }
  } /* else: if ( x[0] < x[N-1] ) */

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Linear interpolation helper function to check that the input x-coordinate
  array is in the proper sorted format.

  @details
  Verifies that an array of 16-bit signed integers is monotonically
  strictly increasing (no duplicates) or strictly decreasing.

  @param x
  The array of 16-bit signed values to check for sorted order.

  @param N
  The number of items in the array x.

  @return
  TRUE if the array is sorted, FALSE otherwise.
*/
static
boolean
x_is_sorted_s16
(
  const int16 x[],
  const int N
)
{
  int i;

  if ( N < 2 )
  {
    ERR_FATAL( "x_is_unsorted_s16: Array size is too small %d", N, 0, 0 );
  }

  if ( x[0] < x[N-1] )
  {
    for ( i = 0; i < N - 1; i++ )
    {
      if ( x[i] >= x[i+1] )
      {
        return FALSE;
      }
    }
  } /* if ( x[0] < x[N-1] ) */
  else
  {
    for ( i = 0; i < N - 1; i++ )
    {
      if ( x[i] <= x[i+1] )
      {
        return FALSE;
      }
    }
  } /* else: if ( x[0] < x[N-1] ) */

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute y=f(x) where f(x) is a piecewise-linear function described by
  a table of x and y coordinates, using linear interpolation between
  known points.

  @details
  This function computes
  ((Y[i] - Y[i-1])/(X[i] - X[i-1])*(x - X[i-1]) + Y[i-1] for an input
  value x, which falls between the points [X[i-1], x[i]].  For values
  outside the set of known points, the function will return the Y
  value for the nearest endpoint.

  The piecewise-linear function is described by a set of coordinates
  (X[i], Y[i]) for i in [0, N-1] which are contained in the input
  arrays x and y.

  The input table X must be sorted, either in strictly increasing
  or strictly decreasing order.  There must be no duplicate points
  in the X table, as this does not satisfy the criteria of a
  function, i.e. each x has only one corresponding y value.

  This implementation operates on an independent variable which
  is encoded as an unsigned 32-bit integer, and dependent
  variable which is encoded as a 16-bit signed integer.  It is
  the responsibility of the user to select the correct interpolation
  function to match the given data types.

  @param x
  An array of N 32-bit unsigned integers which represent
  the x coordinates at the inflection points of a piecewise-linear
  function.  This array must be provided in sorted order either
  increasing or decreasing.  There must be no duplicate entries
  in the array.

  @param y
  An array of N 16-bit signed integers which represent
  the y coordinates at the inflection points of a piecewise-linear
  function.  Each element at index i in [0, N-1] is the corresponding
  y coordinate of the x[i] point.

  @param N
  The total number of points described in the x and y arrays.

  @param unknown
  The x coordinate at which the function should be evaluated at.

  @return
  The value of the piecewise-lineary function evaluated at the
  x value unknown.
*/
int16
rf_lininterp_u32_s16
(
  const uint32 x[],
  const int16  y[],
  const int    N,
  const uint32 unknown
)
{
  if ( N <= 0 )
  {
    ERR_FATAL( "rf_lininterp_u32_s16: N <= 0: %d", N, 0, 0 );
  }

  if ( x == NULL || y == NULL )
  {
    ERR_FATAL( "rf_lininterp_u32_s16: null array provided for x or y: %d, %d", (uint32)x, (uint32)y, 0 );
  }

  if ( N == 1 )
  {
    /* Only one item in the array, the function is flat and independent
       of x */
    return y[0];
  }
  else
  {
    if ( !x_is_sorted_u32( x, N ) )
    {
      ERR_FATAL( "rf_lininterp_u32_s16: unsorted input array", 0, 0, 0 );
    }

    if ( x[0] < x[N-1] )
    {
      /* X points are listed in increasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = 0;
      while ( i < N && unknown >= x[i] )
      {
        i++;
      }

      if ( i == 0 )
      {
        /* unknown is smaller than the smallest X value */
        return y[0];
      }
      else if ( i == N )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i-1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i-1], y[i-1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i-1])
                                          + y[i-1]*(x[i]-unknown),
                                          (x[i]-x[i-1]) );
        return (int16)rf_limit_s32( result, -32768, 32767 );
      }
    } /* if ( x[0] < x[N-1] ) */
    else
    {
      /* X points are listed in decreasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = N-1;
      while ( i >= 0 && unknown >= x[i] )
      {
        i--;
      }

      if ( i == -1 )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[0];
      }
      else if ( i == N-1 )
      {
        /* unknown is smaller than or equal to the smallest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i+1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i+1], y[i+1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i+1]) + y[i+1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i+1])
                                          + y[i+1]*(x[i]-unknown),
                                          (x[i]-x[i+1]) );
        return (int16)rf_limit_s32( result, -32768, 32767 );
      }
    } /* else: if ( x[0] < x[N-1] ) */
  } /* else: if ( N == 1 ) */
} /* rf_lininterp_u32_s16 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute y=f(x) where f(x) is a piecewise-linear function described by
  a table of x and y coordinates, using linear interpolation between
  known points.

  @details
  This function computes
  ((Y[i] - Y[i-1])/(X[i] - X[i-1])*(x - X[i-1]) + Y[i-1] for an input
  value x, which falls between the points [X[i-1], x[i]].  For values
  outside the set of known points, the function will return the Y
  value for the nearest endpoint.

  The piecewise-linear function is described by a set of coordinates
  (X[i], Y[i]) for i in [0, N-1] which are contained in the input
  arrays x and y.

  The input table X must be sorted, either in strictly increasing
  or strictly decreasing order.  There must be no duplicate points
  in the X table, as this does not satisfy the criteria of a
  function, i.e. each x has only one corresponding y value.

  This implementation operates on an independent variable which
  is encoded as an unsigned 32-bit integer, and dependent
  variable which is encoded as a 16-bit signed integer.  It is
  the responsibility of the user to select the correct interpolation
  function to match the given data types.

  @param x
  An array of N 32-bit unsigned integers which represent
  the x coordinates at the inflection points of a piecewise-linear
  function.  This array must be provided in sorted order either
  increasing or decreasing.  There must be no duplicate entries
  in the array.

  @param y
  An array of N 16-bit unsigned integers which represent
  the y coordinates at the inflection points of a piecewise-linear
  function.  Each element at index i in [0, N-1] is the corresponding
  y coordinate of the x[i] point.

  @param N
  The total number of points described in the x and y arrays.

  @param unknown
  The x coordinate at which the function should be evaluated at.

  @return
  The value of the piecewise-lineary function evaluated at the
  x value unknown.
*/
uint16
rf_lininterp_u32_u16
(
  const uint32 x[],
  const uint16 y[],
  const int    N,
  const uint32 unknown
)
{
  if ( N <= 0 )
  {
    ERR_FATAL( "rf_lininterp_u32_u16: N <= 0: %d", N, 0, 0 );
  }

  if ( x == NULL || y == NULL )
  {
    ERR_FATAL( "rf_lininterp_u32_u16: null array provided for x or y: %d, %d", (uint32)x, (uint32)y, 0 );
  }

  if ( N == 1 )
  {
    /* Only one item in the array, the function is flat and independent
       of x */
    return y[0];
  }
  else
  {
    if ( !x_is_sorted_u32( x, N ) )
    {
      ERR_FATAL( "rf_lininterp_u32_u16: unsorted input array", 0, 0, 0 );
    }

    if ( x[0] < x[N-1] )
    {
      /* X points are listed in increasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = 0;
      while ( i < N && unknown >= x[i] )
      {
        i++;
      }

      if ( i == 0 )
      {
        /* unknown is smaller than the smallest X value */
        return y[0];
      }
      else if ( i == N )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i-1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i-1], y[i-1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i-1])
                                          + y[i-1]*(x[i]-unknown),
                                          (x[i]-x[i-1]) );
        return (uint16)rf_limit_u32( result, 0, 65535 );
      }
    } /* if ( x[0] < x[N-1] ) */
    else
    {
      /* X points are listed in decreasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = N-1;
      while ( i >= 0 && unknown >= x[i] )
      {
        i--;
      }

      if ( i == -1 )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[0];
      }
      else if ( i == N-1 )
      {
        /* unknown is smaller than or equal to the smallest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i+1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i+1], y[i+1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i+1]) + y[i+1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i+1])
                                          + y[i+1]*(x[i]-unknown),
                                          (x[i]-x[i+1]) );
        return (uint16)rf_limit_u32( result, 0, 65535 );
      }
    } /* else: if ( x[0] < x[N-1] ) */
  } /* else: if ( N == 1 ) */
} /* rf_lininterp_u32_u16 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute y=f(x) where f(x) is a piecewise-linear function described by
  a table of x and y coordinates, using linear interpolation between
  known points.

  @details
  This function computes
  ((Y[i] - Y[i-1])/(X[i] - X[i-1])*(x - X[i-1]) + Y[i-1] for an input
  value x, which falls between the points [X[i-1], x[i]].  For values
  outside the set of known points, the function will return the Y
  value for the nearest endpoint.

  The piecewise-linear function is described by a set of coordinates
  (X[i], Y[i]) for i in [0, N-1] which are contained in the input
  arrays x and y.

  The input table X must be sorted, either in strictly increasing
  or strictly decreasing order.  There must be no duplicate points
  in the X table, as this does not satisfy the criteria of a
  function, i.e. each x has only one corresponding y value.

  This implementation operates on an independent variable which
  is encoded as an unsigned 32-bit integer, and dependent
  variable which is encoded as a 8-bit signed integer.  It is
  the responsibility of the user to select the correct interpolation
  function to match the given data types.

  @param x
  An array of N 32-bit unsigned integers which represent
  the x coordinates at the inflection points of a piecewise-linear
  function.  This array must be provided in sorted order either
  increasing or decreasing.  There must be no duplicate entries
  in the array.

  @param y
  An array of N 8-bit signed integers which represent
  the y coordinates at the inflection points of a piecewise-linear
  function.  Each element at index i in [0, N-1] is the corresponding
  y coordinate of the x[i] point.

  @param N
  The total number of points described in the x and y arrays.

  @param unknown
  The x coordinate at which the function should be evaluated at.

  @return
  The value of the piecewise-lineary function evaluated at the
  x value unknown.
*/
int8
rf_lininterp_u32_s8
(
  const uint32 x[],
  const int8   y[],
  const int    N,
  const uint32 unknown
)
{
  if ( N <= 0 )
  {
    ERR_FATAL( "rf_lininterp_u32_s8: N <= 0: %d", N, 0, 0 );
  }

  if ( x == NULL || y == NULL )
  {
    ERR_FATAL( "rf_lininterp_u32_s8: null array provided for x or y: %d, %d", (uint32)x, (uint32)y, 0 );
  }

  if ( N == 1 )
  {
    /* Only one item in the array, the function is flat and independent
       of x */
    return y[0];
  }
  else
  {
    if ( !x_is_sorted_u32( x, N ) )
    {
      ERR_FATAL( "rf_lininterp_u32_s8: unsorted input array", 0, 0, 0 );
    }

    if ( x[0] < x[N-1] )
    {
      /* X points are listed in increasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = 0;
      while ( i < N && unknown >= x[i] )
      {
        i++;
      }

      if ( i == 0 )
      {
        /* unknown is smaller than the smallest X value */
        return y[0];
      }
      else if ( i == N )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i-1]) > 8388608 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i-1], y[i-1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i-1])
                                          + y[i-1]*(x[i]-unknown),
                                          (x[i]-x[i-1]) );
        return (int8)rf_limit_s32( result, -128, 127 );
      }
    } /* if ( x[0] < x[N-1] ) */
    else
    {
      /* X points are listed in decreasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = N-1;
      while ( i >= 0 && unknown >= x[i] )
      {
        i--;
      }

      if ( i == -1 )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[0];
      }
      else if ( i == N-1 )
      {
        /* unknown is smaller than or equal to the smallest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i+1]) > 8388608 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i+1], y[i+1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i+1]) + y[i+1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i+1])
                                          + y[i+1]*(x[i]-unknown),
                                          (x[i]-x[i+1]) );
        return (int8)rf_limit_s32( result, -128, 127 );
      }
    } /* else: if ( x[0] < x[N-1] ) */
  } /* else: if ( N == 1 ) */
} /* rf_lininterp_u32_s8 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute y=f(x) where f(x) is a piecewise-linear function described by
  a table of x and y coordinates, using linear interpolation between
  known points.

  @details
  This function computes
  ((Y[i] - Y[i-1])/(X[i] - X[i-1])*(x - X[i-1]) + Y[i-1] for an input
  value x, which falls between the points [X[i-1], x[i]].  For values
  outside the set of known points, the function will return the Y
  value for the nearest endpoint.

  The piecewise-linear function is described by a set of coordinates
  (X[i], Y[i]) for i in [0, N-1] which are contained in the input
  arrays x and y.

  The input table X must be sorted, either in strictly increasing
  or strictly decreasing order.  There must be no duplicate points
  in the X table, as this does not satisfy the criteria of a
  function, i.e. each x has only one corresponding y value.

  This implementation operates on an independent variable which
  is encoded as an unsigned 32-bit integer, and dependent
  variable which is encoded as a 8-bit signed integer.  It is
  the responsibility of the user to select the correct interpolation
  function to match the given data types.

  @param x
  An array of N 16-bit unsigned integers which represent
  the x coordinates at the inflection points of a piecewise-linear
  function.  This array must be provided in sorted order either
  increasing or decreasing.  There must be no duplicate entries
  in the array.

  @param y
  An array of N 8-bit signed integers which represent
  the y coordinates at the inflection points of a piecewise-linear
  function.  Each element at index i in [0, N-1] is the corresponding
  y coordinate of the x[i] point.

  @param N
  The total number of points described in the x and y arrays.

  @param unknown
  The x coordinate at which the function should be evaluated at.

  @return
  The value of the piecewise-lineary function evaluated at the
  x value unknown.
*/

int8 
rf_lininterp_u16_s8
(
  const uint16 x[],
  const int8   y[],
  const int    N,
  const uint16 unknown
)
{
  if ( N <= 0 )
  {
    ERR_FATAL( "rf_lininterp_u16_s8: N <= 0: %d", N, 0, 0 );
  }

  if ( x == NULL || y == NULL )
  {
    ERR_FATAL( "rf_lininterp_u16_s8: null array provided for x or y: %d, %d", 
    (uint32)x, (uint32)y, 0 );
  }

  if ( N == 1 )
  {
    /* Only one item in the array, the function is flat and independent
       of x */
    return y[0];
  }
  else
  {
    if ( !x_is_sorted_u16( x, N ) )
    {
      ERR_FATAL( "rf_lininterp_u16_s8: unsorted input array", 0, 0, 0 );
    }

    if ( x[0] < x[N-1] )
    {
      /* X points are listed in increasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = 0;
      while ( i < N && unknown >= x[i] )
      {
        i++;
      }

      if ( i == 0 )
      {
        /* unknown is smaller than the smallest X value */
        return y[0];
      }
      else if ( i == N )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i-1])
                                          + y[i-1]*(x[i]-unknown),
                                          (x[i]-x[i-1]) );
        return (int8)rf_limit_s32( result, -128, 127 );
      }
    } /* if ( x[0] < x[N-1] ) */
    else
    {
      /* X points are listed in decreasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = N-1;
      while ( i >= 0 && unknown >= x[i] )
      {
        i--;
      }

      if ( i == -1 )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[0];
      }
      else if ( i == N-1 )
      {
        /* unknown is smaller than or equal to the smallest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        /* Compute delta_y/(delta_x) * (unknown-x[i+1]) + y[i+1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i+1])
                                          + y[i+1]*(x[i]-unknown),
                                          (x[i]-x[i+1]) );
        return (int8)rf_limit_s32( result, -128, 127 );
      }
    } /* else: if ( x[0] < x[N-1] ) */
  } /* else: if ( N == 1 ) */
} /* rf_lininterp_u16_s8 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute y=f(x) where f(x) is a piecewise-linear function described by
  a table of x and y coordinates, using linear interpolation between
  known points.

  @details
  This function computes
  ((Y[i] - Y[i-1])/(X[i] - X[i-1])*(x - X[i-1]) + Y[i-1] for an input
  value x, which falls between the points [X[i-1], x[i]].  For values
  outside the set of known points, the function will return the Y
  value for the nearest endpoint.

  The piecewise-linear function is described by a set of coordinates
  (X[i], Y[i]) for i in [0, N-1] which are contained in the input
  arrays x and y.

  The input table X must be sorted, either in strictly increasing
  or strictly decreasing order.  There must be no duplicate points
  in the X table, as this does not satisfy the criteria of a
  function, i.e. each x has only one corresponding y value.

  This implementation operates on an independent variable which
  is encoded as an unsigned 32-bit integer, and dependent
  variable which is encoded as a 16-bit signed integer.  It is
  the responsibility of the user to select the correct interpolation
  function to match the given data types.

  @param x
  An array of N 16-bit unsigned integers which represent
  the x coordinates at the inflection points of a piecewise-linear
  function.  This array must be provided in sorted order either
  increasing or decreasing.  There must be no duplicate entries
  in the array.

  @param y
  An array of N 16-bit signed integers which represent
  the y coordinates at the inflection points of a piecewise-linear
  function.  Each element at index i in [0, N-1] is the corresponding
  y coordinate of the x[i] point.

  @param N
  The total number of points described in the x and y arrays.

  @param unknown
  The x coordinate at which the function should be evaluated at.

  @return
  The value of the piecewise-lineary function evaluated at the
  x value unknown.
*/
int16
rf_lininterp_u16_s16
(
  const uint16 x[],
  const int16  y[],
  const int    N,
  const uint16 unknown
)
{
  if ( N <= 0 )
  {
    ERR_FATAL( "rf_lininterp_u16_s16: N <= 0: %d", N, 0, 0 );
  }

  if ( x == NULL || y == NULL )
  {
    ERR_FATAL( "rf_lininterp_u16_s16: null array provided for x or y: %d, %d", 
    (uint32)x, (uint32)y, 0 );
  }

  if ( N == 1 )
  {
    /* Only one item in the array, the function is flat and independent
       of x */
    return y[0];
  }
  else
  {
    if ( !x_is_sorted_u16( x, N ) )
    {
      ERR_FATAL( "rf_lininterp_u16_s16: unsorted input array", 0, 0, 0 );
    }

    if ( x[0] < x[N-1] )
    {
      /* X points are listed in increasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = 0;
      while ( i < N && unknown >= x[i] )
      {
        i++;
      }

      if ( i == 0 )
      {
        /* unknown is smaller than the smallest X value */
        return y[0];
      }
      else if ( i == N )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i-1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i-1], y[i-1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i-1])
                                          + y[i-1]*(x[i]-unknown),
                                          (x[i]-x[i-1]) );
        return (int16)rf_limit_s32( result, -32768, 32767 );
      }
    } /* if ( x[0] < x[N-1] ) */
    else
    {
      /* X points are listed in decreasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = N-1;
      while ( i >= 0 && unknown >= x[i] )
      {
        i--;
      }

      if ( i == -1 )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[0];
      }
      else if ( i == N-1 )
      {
        /* unknown is smaller than or equal to the smallest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i+1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i+1], y[i+1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i+1]) + y[i+1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i+1])
                                          + y[i+1]*(x[i]-unknown),
                                          (x[i]-x[i+1]) );
        return (int16)rf_limit_s32( result, -32768, 32767 );
      }
    } /* else: if ( x[0] < x[N-1] ) */
  } /* else: if ( N == 1 ) */
} /* rf_lininterp_u16_s16 */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute y=f(x) where f(x) is a piecewise-linear function described by
  a table of x and y coordinates, using linear interpolation between
  known points.

  @details
  This function computes
  ((Y[i] - Y[i-1])/(X[i] - X[i-1])*(x - X[i-1]) + Y[i-1] for an input
  value x, which falls between the points [X[i-1], x[i]].  For values
  outside the set of known points, the function will return the Y
  value for the nearest endpoint.

  The piecewise-linear function is described by a set of coordinates
  (X[i], Y[i]) for i in [0, N-1] which are contained in the input
  arrays x and y.

  The input table X must be sorted, either in strictly increasing
  or strictly decreasing order.  There must be no duplicate points
  in the X table, as this does not satisfy the criteria of a
  function, i.e. each x has only one corresponding y value.

  This implementation operates on an independent variable which
  is encoded as an unsigned 32-bit integer, and dependent
  variable which is encoded as a 16-bit signed integer.  It is
  the responsibility of the user to select the correct interpolation
  function to match the given data types.

  @param x
  An array of N 16-bit signed integers which represent
  the x coordinates at the inflection points of a piecewise-linear
  function.  This array must be provided in sorted order either
  increasing or decreasing.  There must be no duplicate entries
  in the array.

  @param y
  An array of N 32-bit unsigned integers which represent
  the y coordinates at the inflection points of a piecewise-linear
  function.  Each element at index i in [0, N-1] is the corresponding
  y coordinate of the x[i] point.

  @param N
  The total number of points described in the x and y arrays.

  @param unknown
  The x coordinate at which the function should be evaluated at.

  @return
  The value of the piecewise-lineary function evaluated at the
  x value unknown.
*/
uint32
rf_lininterp_s16_u32
(
  const int16 x[],
  const uint32  y[],
  const int    N,
  const int16 unknown
)
{
  if ( N <= 0 )
  {
    ERR_FATAL( "rf_lininterp_s16_u32: N <= 0: %d", N, 0, 0 );
  }

  if ( x == NULL || y == NULL )
  {
    ERR_FATAL( "rf_lininterp_s16_u32: null array provided for x or y: %d, %d", 
    (uint32)x, (uint32)y, 0 );
  }

  if ( N == 1 )
  {
    /* Only one item in the array, the function is flat and independent
       of x */
    return y[0];
  }
  else
  {
    if ( !x_is_sorted_s16( x, N ) )
    {
      ERR_FATAL( "rf_lininterp_s16_u32: unsorted input array", 0, 0, 0 );
    }

    if ( x[0] < x[N-1] )
    {
      /* X points are listed in increasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = 0;
      while ( i < N && unknown >= x[i] )
      {
        i++;
      }

      if ( i == 0 )
      {
        /* unknown is smaller than the smallest X value */
        return y[0];
      }
      else if ( i == N )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i-1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i-1], y[i-1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i-1])
                                          + y[i-1]*(x[i]-unknown),
                                          (x[i]-x[i-1]) );
        return (uint32)rf_limit_u32( result, 0, 65535 );
      }
    } /* if ( x[0] < x[N-1] ) */
    else
    {
      /* X points are listed in decreasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = N-1;
      while ( i >= 0 && unknown >= x[i] )
      {
        i--;
      }

      if ( i == -1 )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[0];
      }
      else if ( i == N-1 )
      {
        /* unknown is smaller than or equal to the smallest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i+1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i+1], y[i+1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i+1]) + y[i+1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i+1])
                                          + y[i+1]*(x[i]-unknown),
                                          (x[i]-x[i+1]) );
        return (uint32)rf_limit_u32( result, 0, 65535 );
      }
    } /* else: if ( x[0] < x[N-1] ) */
  } /* else: if ( N == 1 ) */
} /* rf_lininterp_s16_u32 */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute y=f(x) where f(x) is a piecewise-linear function described by
  a table of x and y coordinates, using linear interpolation between
  known points.

  @details
  This function computes
  ((Y[i] - Y[i-1])/(X[i] - X[i-1])*(x - X[i-1]) + Y[i-1] for an input
  value x, which falls between the points [X[i-1], x[i]].  For values
  outside the set of known points, the function will return the Y
  value for the nearest endpoint.

  The piecewise-linear function is described by a set of coordinates
  (X[i], Y[i]) for i in [0, N-1] which are contained in the input
  arrays x and y.

  The input table X must be sorted, either in strictly increasing
  or strictly decreasing order.  There must be no duplicate points
  in the X table, as this does not satisfy the criteria of a
  function, i.e. each x has only one corresponding y value.

  This implementation operates on an independent variable which
  is encoded as an unsigned 16-bit integer, and dependent
  variable which is encoded as a 16-bit signed integer.  It is
  the responsibility of the user to select the correct interpolation
  function to match the given data types.

  @param x
  An array of N 16-bit signed integers which represent
  the x coordinates at the inflection points of a piecewise-linear
  function.  This array must be provided in sorted order either
  increasing or decreasing.  There must be no duplicate entries
  in the array.

  @param y
  An array of N 16-bit unsigned integers which represent
  the y coordinates at the inflection points of a piecewise-linear
  function.  Each element at index i in [0, N-1] is the corresponding
  y coordinate of the x[i] point.

  @param N
  The total number of points described in the x and y arrays.

  @param unknown
  The x coordinate at which the function should be evaluated at.

  @return
  The value of the piecewise-lineary function evaluated at the
  x value unknown.
*/
uint16
rf_lininterp_s16_u16
(
  const int16 x[],
  const uint16  y[],
  const int    N,
  const int16 unknown
)
{
  if ( N <= 0 )
  {
    ERR_FATAL( "rf_lininterp_s16_u16: N <= 0: %d", N, 0, 0 );
  }

  if ( x == NULL || y == NULL )
  {
    ERR_FATAL( "rf_lininterp_s16_u16: null array provided for x or y: %d, %d", 
    (uint32)x, (uint32)y, 0 );
  }

  if ( N == 1 )
  {
    /* Only one item in the array, the function is flat and independent
       of x */
    return y[0];
  }
  else
  {
    if ( !x_is_sorted_s16( x, N ) )
    {
      ERR_FATAL( "rf_lininterp_s16_u16: unsorted input array", 0, 0, 0 );
    }

    if ( x[0] < x[N-1] )
    {
      /* X points are listed in increasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = 0;
      while ( i < N && unknown >= x[i] )
      {
        i++;
      }

      if ( i == 0 )
      {
        /* unknown is smaller than the smallest X value */
        return y[0];
      }
      else if ( i == N )
      {
        int32 res;
        /* unknown is equal to or larger than the largest X value */
        //return y[N-1];
        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
        (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
        minimize truncation in the division step. */
        res = rf_divide_with_rounding( y[N-1]  *(unknown-x[N-2])
                                       + y[N-2]*(x[N-1]-unknown),
                                       (x[N-1]-x[N-2]) );
        return (uint16)rf_limit_u32( res, 0, 65535 );
      }
      else
      {
        int32 result;

        if ( (unknown - x[i-1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i-1], y[i-1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i-1])
                                          + y[i-1]*(x[i]-unknown),
                                          (x[i]-x[i-1]) );
        return (uint16)rf_limit_u32( result, 0, 65535 );
      }
    } /* if ( x[0] < x[N-1] ) */
    else
    {
      /* X points are listed in decreasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = N-1;
      while ( i >= 0 && unknown >= x[i] )
      {
        i--;
      }

      if ( i == -1 )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[0];
      }
      else if ( i == N-1 )
      {
        /* unknown is smaller than or equal to the smallest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i+1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i+1], y[i+1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i+1]) + y[i+1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i+1])
                                          + y[i+1]*(x[i]-unknown),
                                          (x[i]-x[i+1]) );
        return (uint16)rf_limit_u32( result, 0, 65535 );
      }
    } /* else: if ( x[0] < x[N-1] ) */
  } /* else: if ( N == 1 ) */
} /* rf_lininterp_s16_u16 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute y=f(x) where f(x) is a piecewise-linear function described by
  a table of x and y coordinates, using linear interpolation between
  known points.

  @details
  This function computes
  ((Y[i] - Y[i-1])/(X[i] - X[i-1])*(x - X[i-1]) + Y[i-1] for an input
  value x, which falls between the points [X[i-1], x[i]].  For values
  outside the set of known points, the function will return the Y
  value for the nearest endpoint.

  The piecewise-linear function is described by a set of coordinates
  (X[i], Y[i]) for i in [0, N-1] which are contained in the input
  arrays x and y.

  The input table X must be sorted, either in strictly increasing
  or strictly decreasing order.  There must be no duplicate points
  in the X table, as this does not satisfy the criteria of a
  function, i.e. each x has only one corresponding y value.

  This implementation operates on an independent variable which
  is encoded as an unsigned 32-bit integer, and dependent
  variable which is encoded as a 16-bit signed integer.  It is
  the responsibility of the user to select the correct interpolation
  function to match the given data types.

  @param x
  An array of N 16-bit signed integers which represent
  the x coordinates at the inflection points of a piecewise-linear
  function.  This array must be provided in sorted order either
  increasing or decreasing.  There must be no duplicate entries
  in the array.

  @param y
  An array of N 16-bit signed integers which represent
  the y coordinates at the inflection points of a piecewise-linear
  function.  Each element at index i in [0, N-1] is the corresponding
  y coordinate of the x[i] point.

  @param N
  The total number of points described in the x and y arrays.

  @param unknown
  The x coordinate at which the function should be evaluated at.

  @return
  The value of the piecewise-lineary function evaluated at the
  x value unknown.
*/
int16
rf_lininterp_s16_s16
(
  const int16 x[],
  const int16  y[],
  const int    N,
  const int16 unknown
)
{
  if ( N <= 0 )
  {
    ERR_FATAL( "rf_lininterp_s16_s16: N <= 0: %d", N, 0, 0 );
  }

  if ( x == NULL || y == NULL )
  {
    ERR_FATAL( "rf_lininterp_s16_s16: null array provided for x or y: %d, %d", 
    (uint32)x, (uint32)y, 0 );
  }

  if ( N == 1 )
  {
    /* Only one item in the array, the function is flat and independent
       of x */
    return y[0];
  }
  else
  {
    if ( !x_is_sorted_s16( x, N ) )
    {
      ERR_FATAL( "rf_lininterp_s16_s16: unsorted input array", 0, 0, 0 );
    }

    if ( x[0] < x[N-1] )
    {
      /* X points are listed in increasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = 0;
      while ( i < N && unknown >= x[i] )
      {
        i++;
      }

      if ( i == 0 )
      {
        /* unknown is smaller than the smallest X value */
        return y[0];
      }
      else if ( i == N )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i-1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i-1], y[i-1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i-1]) + y[i-1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i-1])
                                          + y[i-1]*(x[i]-unknown),
                                          (x[i]-x[i-1]) );
        return (int16)rf_limit_s32( result, -32768, 32767 );
      }
    } /* if ( x[0] < x[N-1] ) */
    else
    {
      /* X points are listed in decreasing order. */

      /* Find the first index i where x[i] > unknown */
      int i = N-1;
      while ( i >= 0 && unknown >= x[i] )
      {
        i--;
      }

      if ( i == -1 )
      {
        /* unknown is equal to or larger than the largest X value */
        return y[0];
      }
      else if ( i == N-1 )
      {
        /* unknown is smaller than or equal to the smallest X value */
        return y[N-1];
      }
      else
      {
        int32 result;

        if ( (unknown - x[i+1]) > 32768 )
        {
          RF_MSG_5( RF_HIGH,
                    "interpolation between (%d,%d) and (%d,%d) at %d may overflow 32-bit implementation",
                    x[i+1], y[i+1], x[i], y[i], unknown );
        }
        /* Compute delta_y/(delta_x) * (unknown-x[i+1]) + y[i+1] as
           (y1 * (x-x0) + y0 * (x1-x)) / (x1-x0) to to
           minimize truncation in the division step. */
        result = rf_divide_with_rounding(   y[i]  *(unknown-x[i+1])
                                          + y[i+1]*(x[i]-unknown),
                                          (x[i]-x[i+1]) );
        return (int16)rf_limit_s32( result, -32768, 32767 );
      }
    } /* else: if ( x[0] < x[N-1] ) */
  } /* else: if ( N == 1 ) */
} /* rf_lininterp_s16_s16 */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return Max value between the two passed in.

  @details
  This function compares and returns the maximum of between two numbers.

  @param x
  First signed number to be checked

  @param y
  Second signed number to be checked

  @return
  If x is less than y, returns y.  If x is greather than y,
  returns x.  Otherwise returns x.
*/
int16
rf_max_s16
(
  const int16 x,
  const int16 y
)
{

  if ( x < y )
  {
    return y;
  }
  else if ( x > y )
  {
    return x;
  }
  else
  {
    return x;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return Min value between the two passed in.

  @details
  This function compares and returns the minimum of between two numbers.

  @param x
  First signed number to be checked

  @param y
  Second signed number to be checked

  @return
  If x is less than y, returns x.  If x is greather than y,
  returns y.  Otherwise returns x.
*/
int16
rf_min_s16
(
  const int16 x,
  const int16 y
)
{

  if ( x < y )
  {
    return x;
  }
  else if ( x > y )
  {
    return y;
  }
  else
  {
    return x;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Count the number of ones in a given number
 
  @details
  This function analyzes the integer passed to this function and counts the
  number of 1 bits.
 
  This function performs AND operation between the given integer and the same
  integer substracted by 1. This process will be done repeatedly before the 
  integer under consideration reaches 0. The count of the iteration equals the 
  number of 1 bits in the integer.
  - x = x AND (x - 1)

  @param x
  The integer to count the number of 1 bits in.
 
  @return
  Number of 1 bits in the number
*/ 
uint8
rf_count_one_bits
(
  uint32 x
)
{
  uint8 ones_count = 0;

  while ( x != 0 )
  {
    x = x & ( x - 1 );
    ones_count++;
  }

  return ones_count;
} /* rf_count_one_bits */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Count the number of ones in a given number
 
  @details
  This function analyzes the integer passed to this function and counts the
  number of 1 bits.
 
  This function performs AND operation between the given integer and the same
  integer substracted by 1. This process will be done repeatedly before the 
  integer under consideration reaches 0. The count of the iteration equals the 
  number of 1 bits in the integer.
  - x = x AND (x - 1)

  @param x
  The integer to count the number of 1 bits in.
 
  @return
  Number of 1 bits in the number
*/ 
uint8
rf_count_one_bits_uint64
(
  uint64 x
)
{
  uint8 ones_count = 0;

  while ( x != 0 )
  {
    x = x & ( x - 1 );
    ones_count++;
  }

  return ones_count;
} /* rf_count_one_bits_uint64 */

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
uint32
rf_extract_first_one_bit_mask
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

} /* rf_extract_first_one_bit_mask */

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
uint64
rf_extract_first_one_bit_mask_uint64
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

} /* rf_extract_first_one_bit_mask_uint64 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Extract the location of first 1-bit from MSB
 
  @details
  This function will count the location of 1-bit from MSB
 
  @param x
  64-bit number whose first 1-bit MSB location is to be found
 
  @return
  First 1-bit MSB locaiton of the number
*/
uint8
rf_extract_first_one_bit_msb_location
(
  uint64 x
)
{
  uint8 a = 0;

  if (!x)
  {
    return -1;
  }
 
  #define step(b) if (x >= ((uint64)1) << b) a += b, x >>= b
  step(32);step(16); step(8); step(4); step(2); step(1);
  #undef step

  return a;

} /* rf_extract_first_one_bit_msb_location */

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
uint8
rf_extract_first_one_bit_lsb_location
(
  uint64 x
)
{

  uint8 a = 0;
  uint64 x_temp = 0;

  /* Find the 1-bit LSB mask */

  /* Take 2's complement */
  x_temp = (~x) + 1;

  /* AND the 2's complement value with original value to get the mask
  containing only the first 1-bit (from LSB)*/
  x_temp = x_temp & x;

  a = rf_extract_first_one_bit_msb_location(x_temp);

  return a;

} /* rf_extract_first_one_bit_lsb_location */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate 10^x where x is in the range [-1,1].
 
  @details
  Calculate 10^x where x is in the range [-1,1] using Q6 intrinsics
 
  @param x
  int32 input, Q26
 
  @internal
  adj  Optional qfactor adjustment, default adj=15 for Q15 output
  adj = -qfactor + 30
 
  @return
  10^x int64 output, Q15 (default)
 
*/
int64
rf_compute_exp10
(
  int32 x
)
{
  #ifndef T_WINNT
  int64 acc;
  int64 inv;
  int32 sh;
  int32 adj = 15;
  acc = Q6_P_mpy_RR(x , INVLOG2Q13) >> (16+7);
  sh = (acc >> 16) - adj;
  inv = ~acc ;
  /*  inv = ~acc & 0xFFFF;
   *   acc = (EXP10C2*(inv*inv >> 16) - 
   *         EXP10C1*(inv) +
   *         (EXP10C0 << 16)); */
  
  acc = EXP10C0 << 16;
  acc = Q6_P_mpyunac_RlRl( acc, (uint16)EXP10C1, (uint16)inv );  //UU
  acc = Q6_P_mpyuacc_RhRl( acc, (uint32)((uint16)inv * (uint16)inv), EXP10C2 ); //UU

  return Q6_P_asl_PR( acc, sh );
  #else
  #error code not present
#endif

} /* rf_compute_exp10 */

/*! @} */
