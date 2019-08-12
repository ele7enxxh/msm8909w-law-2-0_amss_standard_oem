#ifndef TZBSP_SMMU_H
#define TZBSP_SMMU_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_smmu.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/12   jl      Initial version.
============================================================================*/
#include <comdef.h>
#include "tz_syscall_pub.h"
#include "SMMUTranslation.h"

/* Pagetable to Context Bank mappings */
#define TZBSP_SMMU_VENUS_BITSTREAM_CB   20
#define TZBSP_SMMU_VENUS_FIRMWARE_CB    22
#define TZBSP_SMMU_VENUS_PIXEL_CB       19
#define TZBSP_SMMU_VENUS_NON_PIXEL_CB   21
#define TZBSP_SMMU_MDSS_CP_CB           18

/**
 * Pagetables available for mapping
 */
typedef enum tzbsp_smmu_pts_e {
  TZBSP_SMMU_SS_PT_VENUS_FW = 0,
  TZBSP_SMMU_SS_PT_PIXEL_CP,
  TZBSP_SMMU_SS_PT_NONPIXEL_CP,
  TZBSP_SMMU_SS_PT_BITSTREAM_CP,
  TZBSP_SMMU_SS_PT_MDSS_CP,
  TZBSP_SMMU_SS_PT_LAST
} tzbsp_smmu_pts_t;

/**
 * Call to initialize the SMMU.  Sets up the ASID->CB mappings
 * and basic CB configuration
 *
 * @param [in] subsys   Enumeration value of the subsystem
 *                      SMMU to be configured
 * 
 * @sideeffects   Pagetables will be cleared 
 *
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_smmu_init(tz_device_e_type subsys);

/**
 * Check if the SMMU has been initialized.
 *
 * @param [in] subsys   Enumeration value of the subsystem
 *                      SMMU to be checked
 * 
 * @returns TRUE if SMMU is already configured, FALSE otherwise
 */
boolean tzbsp_smmu_init_done(tz_device_e_type subsys);

/**
 * Same as tzbsp_smmu_map, but handles mapping sections and pages automatically
 *
 * @param [in] pt       Pagetable in which to add mapping
 * @param [in] vaddr    Virtual address to map
 * @param [in] paddr    Physical address to map
 * @param [in] size     Size of the memory to map
 * @param [in] perms    Bitwise-OR of PERM_X, PERM_R, PERM_W
 * @param [in] rtype    @see regionType in SMMUTranslation.h
 * @param [in] smmu_on  Performs invalidate operation if TRUE
 *                        (i.e. SMMU is powered and clocked)
 * 
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_smmu_map(tzbsp_smmu_pts_t pt, uint32 vaddr, uint32 paddr, uint32 size,
                                  uint32 perm, regionType rtype, boolean smmu_on);

/**
 * Used to unmap memory into the SMMU pagetables
 *
 * @param [in] pt     Pagetable in which to unmap memory
 * @param [in] vaddr  Virtual address to unmap
 * @param [in] size   Size of the memory to unmap
 * @param [in] smmu_on  Performs invalidate operation if TRUE
 *                        (i.e. SMMU is powered and clocked)
 * 
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_smmu_unmap(tzbsp_smmu_pts_t pt, uint32 vaddr, uint32 size, boolean smmu_on);

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
 * 
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_smmu_invalidate_tlb(uint32 device_id, uint32 ctxt_bank_id);

/**
 * Clears the indicated pagetables
 *
 * @param [in] pt     Pagetable in which to unmap memory
 * 
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_smmu_clear_pt(tzbsp_smmu_pts_t pt);


/**
 * Checks if physical address is mapped, can only check 1MB 
 * chunks at a time 
 *
 * @param [in] pt      Pagetable in which to check if memory is 
 *                     mapped
 * @param [out] vaddr  Virtual address mapped to the Physical 
 *                     address only if API is successful 
 * @param [in] paddr   check if Physical address is mapped 
 * @param [in] smmu_on indicate if smmu clocks are on or off
 *   
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_smmu_is_mapped(tzbsp_smmu_pts_t pt, uint32* vaddr, uint32 paddr, boolean smmu_on);

#endif /* TZBSP_SMMU_H */

