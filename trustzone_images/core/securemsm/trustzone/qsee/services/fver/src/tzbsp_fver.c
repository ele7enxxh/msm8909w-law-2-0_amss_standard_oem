/**
 * @file tzbsp_fver.c
 *
 * @brief Trustzone feature tracking file
 *
 * The TZBSP feature version API should track TZBSP features and their
 * compatability with the HLOS.  The HLOS will use this API to
 * determine if TZ features exist and how to interact with them.
 * Feature versions are used in lieu of an image version to enable
 * porting of specific features backward.
 *
 * This file should be TZBSP specific.  Any OEM related feature calls
 * should be added to the OEM directory and any secure OS related
 * version calls to the secure OS.  The definitions of the features
 * will be shared between TZBSP and the HLOS via a document.
 */

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/fver/src/tzbsp_fver.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
01/25/12   pre     Initial
=============================================================================*/

#include <comdef.h>
#include "tzbsp_errno.h"
#include "tzbsp_mem.h"
#include "tz_fver.h"


typedef struct fver_s
{
  uint32  feature_id;
  uint32  version;
} fver_t;


/* The list of features must be continuous or else the add_feature
 * function will need to be rewritten
 */
static fver_t tzbsp_feature_list[] =
{
  {TZ_FVER_PIL,                TZ_FVER_SET_VERSION(1, 0, 0)},
  {TZ_FVER_XPU_CONFIG,         TZ_FVER_SET_VERSION(1, 0, 0)},
  {TZ_FVER_POWER_COLLAPSE,     TZ_FVER_SET_VERSION(1, 0, 0)},
  {TZ_FVER_NS_IF,              TZ_FVER_SET_VERSION(1, 0, 0)},
  {TZ_FVER_SECURE_IF,          TZ_FVER_SET_VERSION(1, 0, 0)},
  {TZ_FVER_SSD,                TZ_FVER_SET_VERSION(2, 0, 0)},
  {TZ_FVER_DIAG,               TZ_FVER_SET_VERSION(1, 0, 0)},
  {TZ_FVER_NS_WATCHDOG,        TZ_FVER_SET_VERSION(1, 0, 0)},
  {TZ_FVER_QDSS,               TZ_FVER_SET_VERSION(0, 0, 0)},
#ifdef TZBSP_WITH_MMU
  {TZ_FVER_MMU,                TZ_FVER_SET_VERSION(1, 0, 0)},
#else
  {TZ_FVER_MMU,                TZ_FVER_SET_VERSION(0, 0, 0)},
#endif
  {TZ_FVER_QSEE,               TZ_FVER_SET_VERSION(1, 5, 0)},

  {TZ_FVER_CP,                 TZ_FVER_SET_VERSION(CP_MAJ, CP_MIN, CP_STEP)},

  /* Anything added to this enum must immediately be integrated
   * across PLs and added to feature version document for HLOS.
   * Also make sure to add anything here to the header file for
   * proper documentation.
   */
};

/**
 * For a given TZBSP feature, return the current version
 *
 * @param[in]  feature_id Unique identifier for a given feature
 * @param[out] version    The current version for the given feature
 * @param[in]  version_sz Size of the memory pointed to by version param
 *
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_fver_get_version(uint32  feature_id,
                           uint32* version,
                           uint32  version_sz)
{
  uint32 ii = 0;

  /* Protect from register based syscall attacks */
  if (!tzbsp_is_ns_range(version, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  /* Wrong size output parameter */
  if ((version_sz < sizeof(uint32)) || (version == NULL))
  {
    return -E_INVALID_ARG;
  }

  *version = TZ_FVER_UNSUPPORTED_FEATURE;

  for (ii = 0; ii < TZBSP_ARRAY_SIZE(tzbsp_feature_list); ii++)
  {
    if (tzbsp_feature_list[ii].feature_id == feature_id)
    {
      *version = tzbsp_feature_list[ii].version;
    }
  }

  return E_SUCCESS;
}

