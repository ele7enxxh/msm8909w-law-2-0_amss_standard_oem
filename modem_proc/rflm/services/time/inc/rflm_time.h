/*!
  @file
  rflm_time.h

  @brief
  time related macros (for XO and USTMR)

  @detail
 
  @todo:
  
 
*/

/*===========================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
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
06-17-14   jc      update for Jolokia
04-15-14   jc      remove hard coded XO register address value
02-18-14   dbc     make rflm_time_wait_us a nop for off target builds
10-15-13   jc      added ustmr masking function
10-01-13   jc      modified for use with Dime 3925 build
===========================================================================*/

#ifndef RFLM_TIME_H
#define RFLM_TIME_H

#include "rflm.h"
#include "rflm_hwintf.h"
#include "rflm_time_def.h"

/*===========================================================================

      Macros

===========================================================================*/

/*! @brief Number of bits in USTMR count.
*/
#if defined( HWIO_STMR_TIME_RD_ADDR )  
  #define USTMR_COUNT_BITS POPCOUNT(HWIO_STMR_TIME_RD_RMSK)
#elif defined( HWIO_UNIV_STMR_MSTMR_ADDR )
  #define USTMR_COUNT_BITS POPCOUNT(HWIO_UNIV_STMR_MSTMR_RMSK)
#elif defined( HWIO_MSS_STMR_TIME_RD_ADDR )
  #define USTMR_COUNT_BITS POPCOUNT(HWIO_MSS_STMR_TIME_RD_RMSK)
#else
#error "Unknown USTMR HW register"
#endif

// for Bolt
// COMPILE_ASSERT( USTMR_COUNT_BITS==24 );
// for DimePM
// COMPILE_ASSERT( USTMR_COUNT_BITS==27 );

/*! @brief Number of usable bits in XO count.
*/
#define XO_COUNT_BITS         24


/*! @brief Frequency (in Hz) of the XO/TCXO clock.
*/
#define RFLM_TIME_XO_CLK_HZ             19200000
#define RFLM_TIME_XO_FREQ_MHZ_Q16       ((uint32)(RFLM_TIME_XO_CLK_HZ/1000000.0*(1<<16)))


/*===========================================================================

  FUNCTION:  rflm_time_read_xo_time

===========================================================================*/
/*!
    @brief
    Return value of free-running QDSP6SS XO counter.

    The counter used is architecture-dependent, and will not necessarily
    be synchronized with any particular system time. 
    However, it is guaranteed to be running at XO rate, and a full 32-bits.
*/
/*=========================================================================*/
STATIC_INLINE uint32 rflm_time_read_xo_time(void)
{
  return HWIO_IN(RFLM_TIME_Q6SS_XO_COUNTER_REG);

} 


/*===========================================================================

  FUNCTION:  rflm_time_xo_count_diff

===========================================================================*/
/*!
    @brief
    Return the signed modulo difference of STMR counts (a - b).

    @detail
    Given two STMR counts a and b, this function returns the signed value (a-b),
    including wraparound at 2^(USTMR_COUNT_BITS-1), i.e. half the sequence space.
    The sign of the difference can be used to compare two counts 
    (if diff(a,b) >=0, a >= b).

    The difference value is ambiguous if the counters are greater than half
    the USTMR sequence space apart (2^21). 

    Examples:
    ustmr_count_diff(10,0) = 10   (10 is greater than 0)
    ustmr_count_diff(0,10) = -10  (0 is less than 10)
    ustmr_count_diff(0x3FFFFF,0) = -1 (with wraparound, a < b)
    ustmr_count_diff(0,0x3FFFFF) = 1
    ustmr_count_diff(0x200000,0) = -0x200000
    ustmr_count_diff(0,0x200000) = -0x200000  (ambiguous! both a>b and a<b)

    @return
    Signed difference, from (-2^26) to (2^26-1).
*/
/*=========================================================================*/
STATIC_INLINE int32 rflm_time_xo_count_diff
(
  uint32 a,  /*!< First count */
  uint32 b   /*!< Second count */
)
{

  /* Power of 2 signed difference trick:
     Calculate the unsigned difference, convert to a OSTMR_COUNT_BITS signed
     integer, and sign extend to 32 bits. */
  int32 diff = (a - b) << (32-XO_COUNT_BITS);
  return (diff >> (32-XO_COUNT_BITS));
} /* rflm_time_xo_count_diff() */


/*===========================================================================

  FUNCTION:  rflm_time_read_ustmr_count

===========================================================================*/

STATIC_INLINE uint32 rflm_time_read_ustmr_count(void)
{
  #if   defined( HWIO_STMR_TIME_RD_ADDR )  
    // for Jolokia, need to check 1st b/c HWIO_UNIV_STMR_MSTMR_ADDR is defined 
	// but we should not use
    return HWIO_IN(STMR_TIME_RD);
  #elif defined( HWIO_UNIV_STMR_MSTMR_ADDR )
    return HWIO_IN(UNIV_STMR_MSTMR);
  #elif defined( HWIO_MSS_STMR_TIME_RD_ADDR )
    return HWIO_IN(MSS_STMR_TIME_RD);
  #else
  #error "USTMR register not defined"
  #endif
}

/*===========================================================================

  FUNCTION:  rflm_time_ustmr_count_diff

===========================================================================*/

STATIC_INLINE int32 rflm_time_ustmr_count_diff
(
  uint32 a,  /*!< First count */
  uint32 b   /*!< Second count */
)
{

  /* Power of 2 signed difference trick:
     Calculate the unsigned difference, convert to a OSTMR_COUNT_BITS signed
     integer, and sign extend to 32 bits. */
  int32 diff = (a - b) << (32-USTMR_COUNT_BITS);
  return (diff >> (32-USTMR_COUNT_BITS));
} /* rflm_time_ustmr_count_diff() */


/*===========================================================================

  FUNCTION:  rflm_time_mask_ustmr_cnt 

===========================================================================*/
/*!
    @brief
    Return "count" masked to UNIV_STMR bitwidth.

    @return
    Input count masked to number of bits in UNIV_STMR.
*/
/*=========================================================================*/
STATIC_INLINE uint32 rflm_time_mask_ustmr_cnt ( uint64 count )
{ 

  return ((uint32)( count & BITMASK(USTMR_COUNT_BITS) ));  

}


/*===========================================================================

  FUNCTION:  rflm_time_wait_us 

===========================================================================*/
/*!
    @brief
    Wait for specified interval

    @detail
    The call waits for at least specified amount of time (in microseconds).
    It uses XO counter so it is not dependent on processor speed, although
    the accuracy may be less at lower processor speeds. 
     
    @return
    None
*/
/*=========================================================================*/
STATIC_INLINE void rflm_time_wait_us
(
  uint32 interval_usec /*!< Time in microseconds to wait, should be less than 1/2 XO counter period */
)
{
#ifndef TEST_FRAMEWORK
  int64 xo_cycles_to_wait = Q6_P_mpy_RR(interval_usec,RFLM_TIME_XO_FREQ_MHZ_Q16);
  uint32 start_time = rflm_time_read_xo_time();
  int tcycles;
  xo_cycles_to_wait = (xo_cycles_to_wait + 0xFFFF)>>16;
  
  while ( rflm_time_xo_count_diff( rflm_time_read_xo_time(), start_time ) <  xo_cycles_to_wait )
  {
    /* Wait for some time to avoid saturating AHB bus */
    tcycles = 100;
    while (tcycles--)
    { 
      nop();
    }
  }
#endif
}


#endif
