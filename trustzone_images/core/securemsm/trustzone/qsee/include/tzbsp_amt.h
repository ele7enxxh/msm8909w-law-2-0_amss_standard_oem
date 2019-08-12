#ifndef TZBSP_AMT_H
#define TZBSP_AMT_H

/**
@file tzbsp_amt.h
@brief Provides API for AMT functionality
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_amt.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
2012/06/25 sg       Initial version

===========================================================================*/
#include "tzbsp_mem.h"
#include "tzbsp_target.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Flags for tzbsp_mpu_amt_config API */

/* 
 * Specify if memory should not be cleared while clearing AMT bits.
 * Effective only in debug environments
 */
#define TZBSP_MPU_AMT_FLAG_DONT_CLEAR_MEM 0x1

#define TZBSP_AMT_NUM_WORDS   (TZBSP_AMT_MAX_BIT_POS/(8*sizeof(uint32)))

/**
 * Structure for the access mask table bits 
 */
typedef struct
{
  /* Access mask table bitmask. Each bit state in this array reflects
   * the corresponding actual state of that bit in AMT  */
  uint32 amtbits[TZBSP_AMT_NUM_WORDS];
} tzbsp_mpu_amt_bits_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
 * Clear the amt locked regions after verifying the current hash
 * of the amt table equals the stored hash value
 *  
 * @param None 
 *  
 * @return \c E_SUCCESS if successful, error code otherwise.
 *            -E_INVALID_ARG indicates that amt mask 
 *                           address was null 
 *            -E_FAILURE indicates crypto operation failed or memcompare 
 *                       failed 
 *  
 */
int tzbsp_mpu_amt_clear(void);

/**
 * API to set the read/write VMIDs for AMT
 *
 * @param [in] rvmid     Bitmask of VMIDs with read access
 * @param [in] wvmid     Bitmask of VMIDs with write access
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */

int tzbsp_mpu_amt_set_vmids(uint32 rvmid, uint32 wvmid);

/**
 * API to Configure the AMT bitmask
 * This API takes an array of bits to be set/cleared in AMT. e.g. An array
 * containing elements [0, 22, 4092] means that the 0th, 22nd and 4092th bits
 * would be set or cleared in the AMT depending on the value of param "enable".
 * Min. value of an element in this array can be 0 and maximum 4095.
 *  
 * @param [in] amt_mask     Pointer to array of bits to be set/cleared in AMT
 * @param [in] num_elements Number of elements in amt_mask
 * @param [in] enable       1 -> set corresponding bits, 0 -> clear corresponding bits
 * @param [in] flags        Optional Flags
 *
 * Currently supported flags: 
 * TZBSP_MPU_AMT_FLAG_DONT_CLEAR_MEM Don't clear memory on unlock
 *                                   (1->dont clear, 0->clear). Effective
 *                                   only in debug environments. 
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 * A return value of -E_FAILURE indicates that the operation was aborted midway
 * and the amt_mask could have been potentially applied partially.
 */

int tzbsp_mpu_amt_config(uint32 *amt_mask, uint32 num_elements, uint8 enable,
                         uint32 flags);

/**
 * API to query if certain bits are set in the AMT
 * This API takes an array of bits and checks if they are set in AMT. e.g. An array
 * containing elements [0, 22, 4092] means that the 0th, 22nd and 4092th bits'
 * state would be checked.
 * Min. value of an element in this array can be 0 and maximum 4095.
 *  
 * @param [in] amt_mask     Pointer to array of bits to checked
 * @param [in] num_elements Number of elements in amt_mask
 *
 * @return \c E_SUCCESS if all bits in array are set, error code otherwise.
 */
int tzbsp_mpu_amt_are_bits_set(uint32 *amt_mask, uint32 num_elements);

/**
 * API to get the bits that are currently set in AMT
 * This API returns the current status of AMT table in the output buffer
 *  
 * @param [out] amt_mask Pointer to array of bits containing status
 *                       corresponding to their state in AMT
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_mpu_amt_get_bits_set(tzbsp_mpu_amt_bits_t *amt_mask);

/**
 * API to get the read/write vmids set for the AMT
 *  
 * @param [out] rvmid Bitmask of read access VMIDs
 * @param [out] wvmid Bitmask of write access VMIDs
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_mpu_amt_get_vmids(uint32 *rvmid, uint32 *wvmid);

/**
 * Check if any portion of given area is being serviced through AMT
 *
 * @param [in] start      start address
 * @param [in] len        length of the region (in bytes)
 *
 * @return \c TRUE if yes, FALSE otherwise.
 */

boolean tzbsp_mpu_amt_is_overlap_amt(uint32 start, uint32 len);


/**
 * Check if any portion of given area is not being serviced 
 * through AMT 
 *
 * @param [in] start      start address
 * @param [in] len        length of the region (in bytes)
 *
 * @return \c TRUE if yes, FALSE otherwise.
 */

boolean tzbsp_mpu_amt_is_not_overlap_amt(uint32 start, uint32 len);

/**
 * Get the size of a memory chunk represented by 1 AMT bit
 *
 * @return Size (in bytes) of a chunk
 */
uint32 tzbsp_mpu_amt_get_chunk_size(void);

#endif /* TZBSP_AMT_H */

