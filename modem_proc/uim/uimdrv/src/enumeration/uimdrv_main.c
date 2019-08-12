/*============================================================================
  FILE:         uimdrv_main.c

  OVERVIEW:     Initializes all the HW classes.

  DEPENDENCIES: N/A

                Copyright (c) 2012 - 2015 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/enumeration/uimdrv_main.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
02/03/15    xj         Add USB UICC enabled on slot interface
07/04/14    ssr        Fix for APDU debug Mask
03/27/14    lxu        USB UICC modem implement
09/30/13    akv        Removal of if-else cases and unused code in hw enumeration
08/1/13     sam        APDU logging mask variable moved to uim_instance_global_type
07/25/13    sam        APDU logging mask selection based on run time
                       configuration
07/12/13    nmb        Global Reorganization
02/22/13    js         8x26 bring up changes
02/21/13    ak         UIM1 support on 9x25 v2
02/11/13    akv        UIM API for querying the available/enabled UIM slots
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_enumeration.h"
#include "DDIChipInfo.h"
#include "uimdrv_pmic.h"
#include "uimdrv_intctrl.h"
#include "uimdrv_gpio.h"
#include "uimdrv_clk.h"
#include "uimdrv_uartdm.h"
#include "uimdrv_main.h"

#include "uimglobals.h"

/* Initializing and declaring of HW config and uim interfaces */
uim_interface_instances              uim_hw_if;
uimHWConfig                          m_HWConfig;
boolean                              uim_usb_supported[UIM_INSTANCES_CONFIGURABLE] = {FALSE};

uimLogMaskType uim_apdu_logging_mask_selector(void)
{
  if (uimdrv_hw_nv_config_info.number_of_active_interfaces == 1)
  {
    return SINGLE_SLOT_LOGGING_MASK_1098;
  }
  else
  {
    return MULTI_SLOT_LOGGING_MASK_14CE;
  }
} /* uim_apdu_logging_mask_selector */

/* Initializes all the HW classes */
boolean initialize_hw_classes(uim_instance_global_type *uim_ptr)
{
  if(m_HWConfig.uim_slot_config[uim_ptr->id].disableUim == FALSE)
  {
    initialize_uartdm(uim_ptr->id);
    initialize_clkregime(uim_ptr->id);
    initialize_pmic(uim_ptr->id);
    initialize_gpio(uim_ptr->id);
    initialize_intctrl(uim_ptr);
    uim_ptr->debug.apdu_log_mask.log_mask = uim_apdu_logging_mask_selector();
    return TRUE;
  }
  return FALSE;
} /* initialize_hw_classes */

boolean uimIsDualSlotEnabled (void)
{
  if ( uimdrv_hw_nv_config_info.number_of_active_interfaces  > 1)
  {
    return TRUE;
  }
  return FALSE;
}

#ifdef FEATURE_UIM_USB_UICC
boolean uim_is_usb_enabled(uim_instance_global_type *uim_ptr)
{
  if(uim_ptr->id > UIM_MAX_INSTANCES)
  {
    return FALSE;
  }
  return uim_usb_supported[uim_ptr->id];
}
#endif /* FEATURE_UIM_USB_UICC */

boolean uim_is_usb_enabled_on_slot(uim_instance_enum_type uim_id)
{
#ifdef FEATURE_UIM_USB_UICC
  if(uim_id > UIM_MAX_INSTANCES)
  {
    return FALSE;
  }
  return uim_usb_supported[uim_id];
#else
  return FALSE;
#endif /* FEATURE_UIM_USB_UICC */
}