#ifndef TZ_KS_COMMON_H
#define TZ_KS_COMMON_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      TrustZone Keystore

General Description
  This is an extension of the generic keystore defined in ks.h/ks.c

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ks/inc/tz_ks_common.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when         who     what, where, why
--------     ---     -------------------------------------------------------
01/30/13     pre     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ks.h"
#include "IxErrno.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

typedef enum
{
  TZ_KS_DIR_MD        = KS_DIR_MD,

  KS_OEM_PUB_TEST_KEY = KS_FREE_MD_VAL + 0, /* 1 */
  KS_OEM_PUB_KEY      = KS_FREE_MD_VAL + 1, /* 2 */

  KS_DVC_PRI_TEST_KEY = KS_FREE_MD_VAL + 2, /* 3 */
  KS_DVC_PRI_KEY      = KS_FREE_MD_VAL + 3, /* 4 */

  KS_DVC_AES_TEST_KEY = KS_FREE_MD_VAL + 4, /* 5 */
  KS_DVC_AES_KEY      = KS_FREE_MD_VAL + 5, /* 6 */

  KS_NS_KEY           = KS_FREE_MD_VAL + 6, /* 7 */

  KS_ENT_MD_MAX_USAGE = 0x7FFFFFFF
} tz_ks_ent_md_t;


/**
   Returns the base address of the pre-allocated TZ keystore memory
   used for both the pre-provisioned keystore and the dynamic
   keystore.
 */
uint8* tz_ks_get_ks_region_start_ptr(void);

/**
   Returns the total size of the pre-allocated TZ keystore memory used
   for both the pre-provisioned keystore and the dynamic keystore.
 */
uint32 tz_ks_get_ks_region_size(void);

/**
   Uses a listener service to load both the pre-provisioned and
   dynamic keystores from eMMC into memory.
 */
int tz_ks_load(void);

/**
   Uses listener services to save both the pre-provisioned keystore
   and dynamic keystore back into eMMC.
 */
int tz_ks_save(void);

/**
   Get the offset to the dynamic keystore
 */
uint32 tz_ks_get_dy_ks_offset(void);

#endif /* #define TZ_KS_COMMON_H */
