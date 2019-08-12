#ifndef BOOT_EXTERN_SECCFG_INTERFACE_H
#define BOOT_EXTERN_SECCFG_INTERFACE_H


/*===========================================================================

                    BOOT EXTERN SECCFG DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external seccfg driver

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_seccfg_interface.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/14   ck      Added boot_qsee_cond_zero_peripheral_memory
04/25/14   ck      When seccfg library does not exist functions must be inlined
03/17/14   ck      Added seccfg function for determining if ramdump is allowed
03/17/14   ck      Renamed to boot_extern_seccfg_interface
12/16/13   ck      Initial Creation

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_comdef.h"
#include "secboot.h"


/**
 * @brief This function will determine if ram dumps are allowed or not
 *
 * @param[in]
 *   secboot_info  Pointer to shared data
 *
 * @return
 *   Boolean indicating if ram dumps are allowed or not
 *
 * @sideeffects
 *   None
 *
 *
 */
#ifdef FEATURE_BOOT_EXTERN_SECCFG_COMPLETED

boot_boolean
boot_qsee_is_memory_dump_allowed(secboot_verified_info_type * secboot_info);

#else

static inline boot_boolean
boot_qsee_is_memory_dump_allowed(secboot_verified_info_type * secboot_info)
{
  return TRUE;
}

#endif // FEATURE_BOOT_EXTERN_SECCFG_COMPLETED


/**
 * @brief This function will determine if device is retail unlocked
 *
 * @param[in]
 *   secboot_pbl_info  Pointer to shared data
 *   secboot_sbl_info  Pointer to shared data
 *
 * @return
 *   Boolean indicating if device is retail unlocked
 *
 * @sideeffects
 *   None
 *
 *
 */
#ifdef FEATURE_BOOT_EXTERN_SECCFG_COMPLETED

boot_boolean
boot_qsee_is_retail_unlocked(secboot_verified_info_type * secboot_info);

#else
static inline boot_boolean
boot_qsee_is_retail_unlocked(secboot_verified_info_type * secboot_info)
{
  return TRUE;
}

#endif // FEATURE_BOOT_EXTERN_SECCFG_COMPLETED


/**
 * @brief This function will zero out peripheral memory when retail ram dumps are allowed
 *
 * @param[in]
 *   None
 *
 * @return
 *   None
 *
 * @sideeffects
 *   None
 *
 *
 */
#ifdef FEATURE_BOOT_EXTERN_SECCFG_COMPLETED

void boot_qsee_zero_peripheral_memory(void);

#else
static inline void boot_qsee_zero_peripheral_memory(void)
{
}

#endif // FEATURE_BOOT_EXTERN_SECCFG_COMPLETED


/**
 * @brief This function will zero out peripheral memory only if target 
 *       conditions require it.  I.E. secboot enabled with debug_enabled off
 *
 * @param[in]
 *   secboot_info  Pointer to shared data
 *
 * @return
 *   None
 *
 * @sideeffects
 *   None
 *
 *
 */
#ifdef FEATURE_BOOT_EXTERN_SECCFG_COMPLETED

void
boot_qsee_cond_zero_peripheral_memory(secboot_verified_info_type * secboot_info);

#else

static inline void
boot_qsee_cond_zero_peripheral_memory(secboot_verified_info_type * secboot_info)
{
}

#endif // FEATURE_BOOT_EXTERN_SECCFG_COMPLETED

#endif /* BOOT_EXTERN_SECCFG_INTERFACE_H */
