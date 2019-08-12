#ifndef MSMSHIFT_H
#define MSMSHIFT_H
/*
===========================================================================
*/
/**
  @file msmshift.h 
  
  This file contains macros for determining bit shift values from bit masks.
  
  The macros in this file are used for determining bit shift values given
  a bit mask.  This can be used to avoid storing both a mask and shift
  value.
*/
/*  
  ====================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/msmshift.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
  ====================================================================
*/ 

/*=========================================================================
      Macros
==========================================================================*/

/**
  @addtogroup deprecated_functions
  @{
*/

/* ============================================================================
**  Macro : SHIFT_DN_FROM_MASK
** ============================================================================*/
/**
  Convert a bitmask to a right bitshift.

  This macro will convert a bitmask into a down-shift value to remove leading
  zeros.  Should not really be used, there are better ways to do this.

  @param x  [in] -- 32-bit mask value

  @return
  Returns the right-shift count for the given mask.

  @dependencies
  None.
*/ 

#define SHIFT_DN_FROM_MASK(x)    \
  ((x & 0x00000001) ? 0  : ((x & 0x00000002) ? 1  : ((x & 0x00000004) ? 2  : \
  ((x & 0x00000008) ? 3  : ((x & 0x00000010) ? 4  : ((x & 0x00000020) ? 5  : \
  ((x & 0x00000040) ? 6  : ((x & 0x00000080) ? 7  : ((x & 0x00000100) ? 8  : \
  ((x & 0x00000200) ? 9  : ((x & 0x00000400) ? 10 : ((x & 0x00000800) ? 11 : \
  ((x & 0x00001000) ? 12 : ((x & 0x00002000) ? 13 : ((x & 0x00004000) ? 14 : \
  ((x & 0x00008000) ? 15 : ((x & 0x00010000) ? 16 : ((x & 0x00020000) ? 17 : \
  ((x & 0x00040000) ? 18 : ((x & 0x00080000) ? 19 : ((x & 0x00100000) ? 20 : \
  ((x & 0x00200000) ? 21 : ((x & 0x00400000) ? 22 : ((x & 0x00800000) ? 23 : \
  ((x & 0x01000000) ? 24 : ((x & 0x02000000) ? 25 : ((x & 0x04000000) ? 26 : \
  ((x & 0x08000000) ? 27 : ((x & 0x10000000) ? 28 : ((x & 0x20000000) ? 29 : \
  ((x & 0x40000000) ? 30 : ((x & 0x80000000) ? 31 : 0 \
    ))))))))))))))))))))))))))))))))

/** @cond */
#define SHIFT_FROM_MASK(x)       SHIFT_DN_FROM_MASK(x##_MASK)
/** @endcond */


/* ============================================================================
**  Macro : SHIFT_UP_FROM_MASK
** ============================================================================*/
/**
  Convert a bitmask to a left bitshift.

  This macro will convert a bitmask into an up-shift value to remove leading
  zeros.  Should not really be used, there are better ways to do this.

  @param x  [in] -- 32-bit mask value

  @return
  Returns the left-shift count for the given mask.

  @dependencies
  None.
*/ 

#define SHIFT_UP_FROM_MASK(x)   \
  ((x & 0x80000000) ? 0  : ((x & 0x40000000) ? 1  : ((x & 0x20000000) ? 2  : \
  ((x & 0x10000000) ? 3  : ((x & 0x08000000) ? 4  : ((x & 0x04000000) ? 5  : \
  ((x & 0x02000000) ? 6  : ((x & 0x01000000) ? 7  : ((x & 0x00800000) ? 8  : \
  ((x & 0x00400000) ? 9  : ((x & 0x00200000) ? 10 : ((x & 0x00100000) ? 11 : \
  ((x & 0x00080000) ? 12 : ((x & 0x00040000) ? 13 : ((x & 0x00020000) ? 14 : \
  ((x & 0x00010000) ? 15 : ((x & 0x00008000) ? 16 : ((x & 0x00004000) ? 17 : \
  ((x & 0x00002000) ? 18 : ((x & 0x00001000) ? 19 : ((x & 0x00000800) ? 20 : \
  ((x & 0x00000400) ? 21 : ((x & 0x00000200) ? 22 : ((x & 0x00000100) ? 23 : \
  ((x & 0x00000080) ? 24 : ((x & 0x00000040) ? 25 : ((x & 0x00000020) ? 26 : \
  ((x & 0x00000010) ? 27 : ((x & 0x00000008) ? 28 : ((x & 0x00000004) ? 29 : \
  ((x & 0x00000002) ? 30 : ((x & 0x00000001) ? 31 : 0 \
    ))))))))))))))))))))))))))))))))

/** @cond */
#define SHIFT_UP_FROM_MASK_W(x)  SHIFT_UP_FROM_MASK(x)
/** @endcond */


/* ============================================================================
**  Macro : SHIFT_TEST
** ============================================================================*/
/**
  Check if a bit is set in a mask.

  This macro checks if the specified bit in the mask is equal to 1.
  We could remove it except that the PMIC drivers still refers to it.

  @param val   [in] -- Value to test.
  @param shift  [in] -- Bit position to check.

  @return
  Returns the masked bit position (meaning 0 if the bit was not set).

  @dependencies
  None.
*/ 

#define SHIFT_TEST(val, shift) ((val) & (1U << (shift)))


/* ============================================================================
**  Macro : SHIFT_GET
** ============================================================================*/
/**
  Legacy macro to check if a bit is set.

  This legacy macro used to be used by SHIFT_FROM_MASK to
  test if the specified bit in the mask is equal 1 or 0. If
  the specified bit equal to 1 then it returns # of shifts else
  check the next bit.
  It is still used by PMIC directly so we cannot remove it.

  @param val   [in] -- Value to test.
  @param shift  [in] -- Bit position to check.
  @param next_shift [in] -- Next bit position.

  @return
  Returns "shift" if the bit was set, otherwise returns "next_shift".

  @dependencies
  None.
*/ 

#define SHIFT_GET(val, shift, next_shift) \
  (SHIFT_TEST((val),(shift)) ? (shift) : (next_shift))

/**
  @}
*/ 

#endif /* MSMSHIFT_H */

