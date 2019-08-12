#ifndef TZBSP_VMIDMT_H
#define TZBSP_VMIDMT_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_vmidmt.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/12   sg      First version.
============================================================================*/

#include <HALvmidmt.h>

#include "tz_syscall_pub.h"

/**
 * API to map a vmid. Allows only specific instances to be remapped
 *
 * @param [in] master VMIDMT instance
 * @param [in] index  Index for which VMID has to be mapped
 * @param [in] vmid   VMID to be mapped
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_vmidmt_map_vmid(HAL_vmidmt_InstanceType master,
                          uint8 index, uint32 vmid);

/**
 * Restore the VMIDMT configuration for all VMIDMT instances corresponding
 * to a given device
 *
 * @param [in] device Device for which sec. config has to be restored
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_vmidmt_restore_device(tz_device_e_type);

int tzbsp_vmidmt_config_ssd(HAL_vmidmt_InstanceType master, 
                        const HAL_vmidmt_SecureStatusDetType secureStatusDetIndex, 
                        boolean secState);

/**
 * Set memtype attribute for a vmidmt index
 * @param [in] device  Device for which memtype has to be set
 * @param [in] idx     Index for which memtype has to be set.
 *                     This should be 0xFF if all indexes of given
 *                     device need to be updated
 * @param [in] memtype Memtype attribute value
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_vmidmt_set_memtype(tz_device_e_type device, uint32 idx,
                             uint32 memtype);
int tzbsp_vmidmt_map_vmid_ext(HAL_vmidmt_InstanceType master,
                              uint32 index, uint32 vmid, bool32 secure);
#endif /* TZBSP_VMIDMT_H */
