#ifndef TZBSP_CP_H
#define TZBSP_CP_H

/**
@file tzbsp_cp.h
@brief Header file for Content Protection related functions.
*/
/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_cp.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/25/12   amen     added usecase enum (prev defined in mpu.h)
06/12/12   rkk      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "tz_syscall_pub.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//dynamic buffer version
#define CP_MAJ  1
#define CP_MIN  1
#define CP_STEP 0

/* -----------------------------------------------------------------------
 * Typedefs
 * ----------------------------------------------------------------------- */

typedef enum
{
  TZBSP_MEM_TAG_USECASE_CP       = 0x01,
  TZBSP_MEM_TAG_USECASE_AUDIO_CP = 0x02,
  TZBSP_MEM_TAG_USECASE_VIDEO_PIXEL_CP = 0x03,
  TZBSP_MEM_TAG_USECASE_MAX      = 0x7FFFFFFF
}tzbsp_mem_usecase_tag_id_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Protects(locks/unlocks) a list of memory chunks with an MPU.
 *
 * @param [in] chunks    Memory chunk list
 * @param [in] mem_usage Hint on the memory usage.
 * @param [in] lock      TRUE for lock, FALSE for unlock 
 * @param[in] flags          Future use
 * @param [out] ret       Pointer to return response to 
 *                       caller
 * @param [in] ret_size  Size of response pointer
 *
 *
 * @return \c E_SUCCESS if the lock was successfully done,
 * error code otherwise.
 */
int tzbsp_memprot_lock2(tz_memprot_chunks_info_t* chunks, 
                        tz_mem_usage_e_type      mem_usage,
                        uint32 lock, uint32 flags, int32* ret, uint32 ret_size);

/**
 * Maps a list of protected memory chunks.
 *
 * @param [in] chunks    Memory chunk list
 * @param [in] map       Map-to information 
 * @param [in] flags     Currently supported flags:
 *                         a) TZ_MEMPROT_FLAG_TLBINVALIDATE 
 * @param [out] ret      Pointer to return response to 
 *                       caller
 * @param [in] ret_size  Size of response pointer
 *
 * @return \c E_SUCCESS if the map was successfully done,
 * error code otherwise.
 */
int tzbsp_memprot_map2(tz_memprot_chunks_info_t* chunks, 
                       tz_memprot_map_info_t*    map, 
                       uint32 flags, int32* ret, uint32 ret_size);

/**
 * Unmaps (removes) Virtual Address to Physical Address mapping 
 * for given range. 
 *
 * @param [in] map       Mapped-at information
 * @param [in] flags     Currently supported flags:
 *                         a) TZ_MEMPROT_FLAG_TLBINVALIDATE 
 * @param [out] ret       Pointer to return response to 
 *                       caller
 * @param [in] ret_size  Size of response pointer
 *
 *
 * @return \c E_SUCCESS if the unmap was successfully done,
 * error code otherwise.
 */
int tzbsp_memprot_unmap2(tz_memprot_map_info_t* map,
                         uint32 flags,int32* ret, uint32 ret_size);

/**
 * Invalidates the context bank of the SMMU associated with the specified
 * pagetable
 *
 * @param [in] device_id     Id of Device which needs TLB 
 *                           invalidated (tz_device_e_type)
 * @param [in] ctxt_bank_id  Context bank id which needs TLB 
 *                           invalidated. These correspond to
 *                           context bank numbers and are unique
 *                           only for the device. Value start
 *                           from 0 which corresponds to CB0
 * @param[in] flags          Future use
 * @param[in] spare          Future use
 * @param [out] ret          Pointer to return response to 
 *                           caller
 * @param [in] ret_size      Size of response pointer
 * 
 * @returns E_SUCCESS on success, negative values on failure
 */

int tzbsp_memprot_tlbinval(uint32 device_id, uint32 ctxt_bank_id,uint32 flags, 
                           uint32 spare, int32* ret, uint32 ret_size);

/**
 * Tests whether all of the range [\c start, \c end] is in secure
 * memory and also tagged for a particular use case.
 *
 * @param [in] tag  Tag ID of the memory it should be tagged with
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end End of the memory range, physical address,
 * included  in the range.
 *
 * @return \c TRUE if the entire area is in secure memory. \c FALSE
 *  if the area contains non-secure memory.
 */

boolean tzbsp_memprot_is_s_tag_area(tzbsp_mem_usecase_tag_id_t tag,
                                    uint32 start, uint32 end);

#endif
