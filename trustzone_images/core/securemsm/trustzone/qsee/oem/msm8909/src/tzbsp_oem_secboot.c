/*===========================================================================
   Copyright (c) 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/oem/msm8909/src/tzbsp_oem_secboot.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/18/14   hw      add tzbsp_oem_fuse_config
03/18/13   jl      Initial version.
=============================================================================*/

#include <comdef.h>
#include "tzbsp.h"
#include "tzbsp_secboot.h"
#include "tzbsp_mrc_fuseprov.h"

/**
 * Returns an alternative root key to be used for authentication
 * of subsystem/secure app images
 *
 * @param[out]  fuse_info   Struct to define the OEM specific fuse
 *                          information
 *
 * @return  \c E_SUCCESS if new fuse values are available,
 *          \c -E_NOT_SUPPORTED otherwise
 */
int32 tzbsp_oem_fuse_info(secboot_fuse_info_type *fuse_info)
{
#if 0
  /* Add OEM-specific fuse information here: */
  fuse_info->root_of_trust = { 0x0 };
  fuse_info->use_root_of_trust_only = TRUE;

  /* Optional overrides, if use_root_of_trust_only == FALSE: */
  fuse_info->msm_hw_id = 0;
  fuse_info->auth_use_serial_num = 0;
  fuse_info->serial_num = 0;

  /* init multiple root cert fuse value */
  fuse_info->root_sel_info.is_root_sel_enabled = SECBOOT_ROOT_CERT_SEL_DISABLED;
  fuse_info->root_sel_info.root_cert_sel = 0;
  fuse_info->root_sel_info.num_root_certs = 0; 

  return E_SUCCESS;
#else
  return -E_NOT_SUPPORTED;
#endif
}

/**
 * to program the MRC root cert index fuse before TZ starts up
 *
 * @return  void
 */
void tzbsp_oem_fuse_config(void)
{
#if 0
  /* int secboot_switch_mrc_index(uint32 apps_root_cert_index, uint32 msa_root_cert_index),
     program root cert index for MRC, and the index is 0-based.
     apps_root_cert_index for the root cert index on apps side (eg. index 15 means the 16th root cert)
     msa_root_cert_index for the index on msa side (eg. index 1 means the 2nd root cert).
     */   
  secboot_switch_mrc_index(15, 1);
#endif
}
