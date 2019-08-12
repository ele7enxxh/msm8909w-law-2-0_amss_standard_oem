#ifndef TZBSP_SECBOOT_H
#define TZBSP_SECBOOT_H

/**
@file tzbsp_secboot.h
@brief TZ secboot interface
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_secboot.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/30/12   jl       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "secboot.h"
#include "secboot_hw.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* SW Types for each of the subsystems; used during image authentication */
#define SECBOOT_SBL_SW_TYPE         0x0
#define SECBOOT_ADSP_SW_TYPE        0x4
#define SECBOOT_TZ_KERNEL_SW_TYPE   0x7
#define SECBOOT_APPSBL_SW_TYPE      0x9
#define SECBOOT_RPM_FW_SW_TYPE      0xA
#define SECBOOT_TZ_EXEC_SW_TYPE     0xC
#define SECBOOT_WCNSS_SW_TYPE       0xD
#define SECBOOT_VIDEO_SW_TYPE       0xE
#define SECBOOT_VPU_SW_TYPE         0xF
#define SECBOOT_BCSS_SW_TYPE        0x10
#define SECBOOT_QHEE_SW_TYPE        0x15
  
#define SECBOOT_INVALID_SW_TYPE     (-1)

/**
 * @brief Returns the debug state (enabled or disabled) as specified by PBL
 *
 * @return  TRUE if debug is enabled, FALSE otherwise
 */
boolean tzbsp_is_debug_enabled(void);

/**
 * @brief Returns the retail crash dump state (enabled or 
 *        disabled) as specified by PBL
 *
 * @return  TRUE if retail crash dump is enabled, FALSE 
 *          otherwise
 */
boolean tzbsp_is_retail_crash_dump_enable(void);

/**
 * @brief This function checks if the code needs to be authenticated. If
 *        authentication is enabled, callers MUST authenticate the
 *        code successfully before it is allowed to execute.
 *
 * @param code_segment   [in]  Segment of SECURE_BOOTn register holding
 *                             authentication information for the code
 *
 * @return \c FALSE if authentication is not required
 *         \c TRUE if authentication IS required
 *
 */
boolean tzbsp_secboot_hw_is_auth_enabled(uint32 code_segment);

/**
 * Authenticates an image using the secboot routines provided by PBL
 *
 * @param image_info[in]        Struct containing information about the image
 *                              to be authenticated
 * @param verified_info[in,out] Struct populated with info about the image
 *                              if authentication is successful.  Memory
 *                              allocated by the caller.
 *
 * @return  \c E_SUCCESS on success, \c -E_FAILURE otherwise
 */
int tzbsp_secboot_authenticate_image(secboot_image_info_type* image_info,
                                     secboot_verified_info_type* verified_info);

#endif /* TZBSP_SECBOOT_H */
