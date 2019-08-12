#ifndef __PM_CONFIG_H__
#define __PM_CONFIG_H__

/**
  @file pm_access_cfg.h

  Header file for PMIC SPMI peripheral interrupt ownership and
  devices access.
*/
/*
  ====================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_access_cfg.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "comdef.h"
#include "pm_lib_err.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*
 * pm_periph_intr_owner
 *
 * A type to choose the interrupt owner ID for the peripheral.
 */
typedef enum {
  PM_INTR_OWNER_APPS = 0,
  PM_INTR_OWNER_APPS_TZ,
  PM_INTR_OWNER_MSS,
  PM_INTR_OWNER_LPASS,
  PM_INTR_OWNER_RPM,
  PM_INTR_OWNER_WCONNECT,
} pm_periph_intr_owner;

/*
 * pm_access_type
 *
 * A bitmap defines PMIC access type.
 */
typedef enum {
  PM_ACCESS_ENABLE            = 0x01, /**< Enables an XPU. */
  PM_ACCESS_NO_INTERRUPTS     = 0x02, /**< Disables XPU error interrupts. */
  PM_ACCESS_MODEM_PROT        = 0x04, /**< Marks modem only items. */
  PM_ACCESS_SEC               = 0x08, /**< Secure partition */
  PM_ACCESS_NON_SEC            = 0x10, /**< Non Secure partition */
  PM_ACCESS_RWGE              = 0x0100, /**< Read/write global enable. */
  PM_ACCESS_ROGE              = 0x0200,/**< Read-only global enable. */
  PM_ACCESS_RWE               = 0x0400, /**< Read/write VMID enable. */
  PM_ACCESS_ROE               = 0x0800, /**< Read-only VMID enable. */
} pm_access_type;

/*
 * pm_vmid_access_type
 *
 * A value defines VMID access type.
 */
typedef enum {
  PM_VMID_NOACCESS,
  PM_VMID_RPM,
  PM_VMID_TZ,
  PM_VMID_AP,
  PM_VMID_MSS,
  PM_VMID_LPASS,
  PM_VMID_WLAN,
  PM_VMID_CP,
  PM_VMID_VIDEO,
  PM_VMID_DEHR,
  PM_VMID_OCMEM_DM,
}pm_vmid_access_type;


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
*  pm_spmi_rpu_type
*
*  Structure to hold PMIC XPU protection configurations.
*/
typedef struct
{
  uint16 pm_ppid;                  /* Physical Peripheral ID */
  uint16 pm_apid;                  /* Application Peripheral ID. */
  uint16 pm_flag;                  /* Defines R/W/RO access. */
  uint16 pm_read_write_vmid;       /* Single VMID read/write protection. */
} pm_spmi_rpu_type;

/**
*  pm_spmi_mpu_type
*
*  Structure to hold PMIC XPU protection configurations.
*/
typedef struct
{
  uint16 pm_gid;                   /* Group ID */
  uint16 pm_ppid;                  /* Physical Peripheral ID */
  uint16 pm_apid;                  /* Application Peripheral ID. */
  uint16 pm_flag;                  /* Defines R/W/RO access. */
  uint16 pm_read_write_vmid;       /* Single VMID read/write protection. */
} pm_spmi_mpu_type;


pm_err_flag_type pm_spmi_xpu_config
(
  const pm_spmi_rpu_type ** rpu_cfg_ptr,
  uint32* cfg_sz_ptr
);

#endif /* __PM_CONFIG_H__ */

