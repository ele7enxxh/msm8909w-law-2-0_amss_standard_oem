#ifndef VMPM_SRC_UTIL_H
#define VMPM_SRC_UTIL_H

/*===========================================================================
  FILE: vmpm_src_util.h

  DESCRIPTION: This files includes macros, inline functions, and other
               pre-compiler constructs needed for the Virtual MPM (vMPM)
               driver.

                Copyright (c) 2014 by QUALCOMM Technologies Incorporated.  
                All Rights Reserved.
                QUALCOMM Proprietary

  $Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/vmpm_src_util.h#1 $ 
===========================================================================*/


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/*==========================================================================
 *                      MACRO DECLARATIONS
 *==========================================================================*/
#ifndef UINT32_MAX
  #define UINT32_MAX (0xffffffff)
#endif

/*==========================================================================
 *                      ENUM, CONST, ETC VARIABLES DECLARATIONS
 *==========================================================================*/

/*==========================================================================
 *                      GLOBAL VARIABLES DECLARATIONS
 *==========================================================================*/

/*==========================================================================
 *                      INLINE FUNCTIONS DECLARATIONS
 *==========================================================================*/
/**
 * safe_uint32_addition
 *
 * @brief Adds two unsigned integers of 32 bits and if their 
 *        total exceeds the maximum value 32 bits can hold, it
 *        will clip the result.
 *
 * @param a: First unsigned integer.
 * @param b: Second unsigned integer.
 *
 * @return Total of two input unsigned numbers (with clipping if required).
 */
static __inline uint32 safe_uint32_addition(uint32 a, uint32 b)
{
  if((UINT32_MAX - a) < b)
  {
    /* clip to maximum */
    return UINT32_MAX;
  }

  return (a + b);
}

/**
 * safe_uint32_subtract
 *
 * @brief Subtracts two unsigned integers of 32 bits and ensures
 *        the smaller is always subtracted from the larger
 *        integer.
 *
 * @param a: First unsigned integer.
 * @param b: Second unsigned integer.
 *
 * @return Difference of two input unsigned numbers. (Ensures 
 *         return value >= 0).
 */
static __inline uint32 safe_uint32_subtract(uint32 a, uint32 b)
{
   uint32 result = ((a) < (b))?(b-a):(a-b);
   
   return result;
}
/*===========================================================================   
                      PUBLIC FUNCTION DECLARATIONS                              
===========================================================================*/

#endif  /* VMPM_SRC_UTIL_H */
 

