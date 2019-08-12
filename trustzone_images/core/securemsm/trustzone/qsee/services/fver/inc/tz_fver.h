#ifndef TZ_FVER_H
#define TZ_FVER_H
/**
 * @file tz_fver.h
 *
 * @brief Trustzone feature tracking file
 *
 * The TZ feature version API should track TZ features and their
 * compatability with the HLOS.  The HLOS will use this API to
 * determine if TZ features exist and how to interact with them.
 * Feature versions are used in lieu of an image version to enable
 * porting of specific features backward.
 *
 * This file should be TZ specific.  Any OEM related feature calls
 * should be added to the OEM directory and any secure OS related
 * version calls to the secure OS.  The definitions of the features
 * will be shared between TZ and the HLOS via a document.
 */

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The TZ_SysCall_API_mainpage.dox file contains all descriptions that 
      are displayed in the output PDF generated using Doxygen and LaTeX. 
      To edit or update any of the file/group text in the PDF, edit the 
      TZ_SysCall_API_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2012-2013 by Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/fver/inc/tz_fver.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
05/08/13   leo     Edited Doxygen comments and markup for Family B updates.
03/08/12   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
02/27/12   pre     Initial
=============================================================================*/

#include "tzbsp_cp.h"

/** @addtogroup tz_macros_constants
@{ */

/** @name Feature Version Macros
@{ */

/**
    Macro used to set the version that can be queried by the
    #TZ_INFO_GET_FEATURE_VERSION_ID syscall.

    - Major = 10 bits (maximum 1023). \n
      Major indicates a change in the syscall interface. \n
    - Minor = 10 bits (maximum 1023). \n
      Minor version is the addition/removal of features without
      a system call change. \n
    - Patch = 12 bits (maximum 4095). \n
      Patch can indicate bugfixes or hardware workarounds.
*/
#define TZ_FVER_SET_VERSION(major, minor, patch) \
  (((major & 0x7FF) << 22) | ((minor & 0x7FF) << 12) | (patch & 0xFFF))

/** Feature is unsupported. */
#define TZ_FVER_UNSUPPORTED_FEATURE  0

/** @} */ /* end_namegroup Feature Version Macros */

/** @} */ /* end_addtogroup tz_macros_constants */

/** @ingroup tz_datatypes

   Unique identifier for the feature that carries a version. These must
   be QSEE features only. Once added, an enumerator may never be
   removed from this list.
*/
typedef enum
{
  TZ_FVER_PIL                      =  0, /**< Peripheral image loader. */
  TZ_FVER_XPU_CONFIG               =  1, /**< xPU configuration. */
  TZ_FVER_POWER_COLLAPSE           =  2, /**< Power collapse. */
  TZ_FVER_NS_IF                    =  3, /**< Nonsecure interface
                                                 (syscall). */
  TZ_FVER_SECURE_IF                =  4, /**< Secure interface (executive). */
  TZ_FVER_SSD                      =  5, /**< Secure software download. */
  TZ_FVER_DIAG                     =  6, /**< Diagnostic services. */
  TZ_FVER_NS_WATCHDOG              =  7, /**< Nonsecure WDOG. */
  TZ_FVER_QDSS                     =  8, /**< Qualcomm debug subsystem. */
  TZ_FVER_MMU                      =  9, /**< Memory management unit. */
  TZ_FVER_QSEE                     = 10, /**< QSEE application layer. */
  TZ_FVER_CP                       = 12, /**< Content Protection versioning. */
} tz_feature_t;

/* ********************************************************************
   IMPORTANT: Versions Below Are For Documentation Only.  The versions
              must be updated in tzbsp_fver.c.
 **********************************************************************/

/** @addtogroup tz_macros_constants
@{ */

/** @name Feature Version Macros
@{ */

/** Secure PIL version: 1.0.0. */
#define TZ_FVER_PIL_VERSION             TZ_FVER_SET_VERSION(1, 0, 0)
/** xPU configuration version: 1.0.0. */
#define TZ_FVER_XPU_CONFIG_VERSION      TZ_FVER_SET_VERSION(1, 0, 0)
/** Power collapse version: 1.0.0. */
#define TZ_FVER_POWER_COLLAPSE_VERSION  TZ_FVER_SET_VERSION(1, 0, 0)
/** Nonsecure interface version: 1.0.0. */
#define TZ_FVER_NS_IF_VERSION           TZ_FVER_SET_VERSION(1, 0, 0)
/** Secure interface version: 1.0.0. */
#define TZ_FVER_SECURE_IF_VERSION       TZ_FVER_SET_VERSION(1, 0, 0)
/** Secure software download version: 1.0.0. */
#define TZ_FVER_SSD_VERSION             TZ_FVER_SET_VERSION(2, 0, 0)
/** Diagnostic services version: 1.0.0. */
#define TZ_FVER_DIAG_VERSION            TZ_FVER_SET_VERSION(1, 0, 0)
/** Nonsecure WDOG version: 1.0.0. */
#define TZ_FVER_NS_WATCHDOG_VERSION     TZ_FVER_SET_VERSION(1, 0, 0)
/** Qualcommm debug subsystem version: Unsupported. */
#define TZ_FVER_QDSS_VERSION            TZ_FVER_SET_VERSION(0, 0, 0)

#ifdef TZBSP_WITH_MMU
  /** MMU version: 1.0.0. */
  #define TZ_FVER_MMU_VERSION           TZ_FVER_SET_VERSION(1, 0, 0)
#else
  /** MMU version: Unsupported. */
  #define TZ_FVER_MMU_VERSION           TZ_FVER_SET_VERSION(0, 0, 0)
#endif

/* Qualcomm secure execution environment version: 1.0.0. */
#define TZ_FVER_QSEE_VERSION            TZ_FVER_SET_VERSION(1, 3, 0)

/* Trustzone CP version: 1.1.0
   1.1.0 : Dynamic buffer allocation */
#define TZ_FVER_CP_VERSION              TZ_FVER_SET_VERSION(CP_MAJ, CP_MIN, CP_STEP)

/** @} */ /* end_namegroup Feature Version Macros */

/** @} */ /* end_addtogroup tz_macros_constants */

#endif /* TZ_FVER_H */
