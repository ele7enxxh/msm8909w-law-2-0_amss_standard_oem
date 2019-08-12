#ifndef TZBSP_MPU_H
#define TZBSP_MPU_H

/**
@file tzbsp_mpu.h
@brief Provides API for MPU functionality
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_mpu.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
2012/06/01 sg       Add CP related new APIs
2011/12/19 qazib    Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Flags for tzbsp_mpu_protect_memory API */

/*
 * Specify if memory should not be cleared while unprotecting.
 * Effective only in debug environments
 */
#define TZBSP_MPU_PROT_MEM_FLAG_DONT_CLEAR_MEM 0x1


/** Deprecated */
typedef enum
{
  TZBSP_MEM_TAG_MM_CARVEOUT        = 0x00,
  TZBSP_MEM_TAG_MFC_SHAREDMEM      = 0x01,
  TZBSP_MEM_TAG_MDP_WRITEBACK      = 0x02,
  TZBSP_MEM_TAG_SA_REGION          = 0x03,
  TZBSP_MEM_TAG_QSEE_REQRSP_REGION = 0x04,
  TZBSP_MEM_TAG_SSD_REGION         = 0x05,
  TZBSP_MEM_TAG_SMMU_PT_MEM        = 0x06,
  TZBSP_MEM_TAG_MAX = 0x7FFFFFFF
} tzbsp_mem_tag_id_t;

/** Enumerates memory usage hints */
typedef enum
{
  /** Memory used for video SMMU page tables */
  TZBSP_MEM_MPU_USAGE_CP_PTBL             = 0x1,

  /** Memory used for QSEE Req/Rsp region  */
  TZBSP_MEM_MPU_USAGE_QSEE_REQRSP_REGION  = 0x2,

  /** Memory used for QSEE Secure Area region  */
  TZBSP_MEM_MPU_USAGE_QSEE_SA_REGION      = 0x3,

  /** Memory used for a variety of secure uses. */
  TZBSP_MEM_MPU_USAGE_SDDR_REGION         = 0x4,

  /** Memory used for QSEE Shared Buffer region  */
  TZBSP_MEM_MPU_USAGE_QSEE_SB_REGION      = 0x5,

  /** Max value */
  TZBSP_MEM_MPU_USAGE_MAX,

  TZBSP_MEM_MPU_USAGE_UNKNOWN  = 0x7fffffff
}tzbsp_mem_mpu_usage_e_type;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Dynamically allocate and deallocate an MPU RG and set R/W permissions
 *
 * @param [in] start      start address
 * @param [in] size       size of the region (in bytes)
 * @param [in] usage_hint A tag that specifies this region's usage
 * @param [in] lock       lock or unlock
 * @param [in] flags      Optional Flags
 *
 * Currently supported flags:
 * TZBSP_MPU_PROT_MEM_FLAG_DONT_CLEAR_MEM Don't clear memory on unlock
 *                                        (1->dont clear, 0->clear). Effective
 *                                        only in debug environments.
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */

int tzbsp_mpu_protect_memory ( uint32 start, uint32 size,
                               tzbsp_mem_mpu_usage_e_type usage_hint,
                               boolean lock, uint32 flags);


#endif /* TZBSP_MPU_H */

