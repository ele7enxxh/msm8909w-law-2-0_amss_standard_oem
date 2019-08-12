#ifndef UIMDRV_PMIC_H
#define UIMDRV_PMIC_H
/*============================================================================
  FILE:          uimdrv_pmic.h

  OVERVIEW:     File contains the inclusions and definitions necesssary for
                the pmic interface.

  DEPENDENCIES: N/A

                Copyright (c) 2012-2014QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/pmic/uimdrv_pmic.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

  when      who        what, where, why
------      ----       -----------------------------------------------------------
11/10/14    akv        Support for user to customize LDO settings
07/25/14    lxu        Read cmd_rsp_time from nv for usb mode, give cmd_rsp_time and
                       voltage_class to ap when send usb power up to ap
04/11/14    lxu        Unregister automatic UICC shutdown on card removal in USB mode
11/27/13    akv        Initialization of hw enumeration based on dal chip family
10/08/13    nmb        HW header file cleanup
07/12/13    nmb        Global Reorganization
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
02/22/13    js         8x26 bring up changes
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "comdef.h"
#include "pm.h"
#include "npa.h"
#include "uimdrv_enumeration.h"

typedef enum
{
  PMIC_UIM_LDO_LOW_POWER_MODE,
  PMIC_UIM_LDO_HIGH_POWER_MODE,
  PMIC_UIM_LDO_POWER_MODE_INVALID
}uim_ldo_powermodes_type;

typedef struct
{
  boolean                         customize_ldo_setting;
  uim_voltage_class_type          desired_voltage_class;
  uim_pmic_voltages_avail_type    desired_voltage;
  boolean                         pdown_allowed_upon_card_removal;
  boolean                         pdown_allowed_upon_error;
}uim_ldo_settings_type;

/* Table holding the default PMIC NPA reference for different UIM instances. */
extern const uim_pmic_npa_resource_table_type   uim_pmic_npa_resource_table[];


/**
*  This function does the initial handshaking needed with PMIC
*  to get all client handles used to communicate with PMIC
*  further.
*
*  @param uim_instance   uim instance.
*/
void initialize_pmic
(
  uim_instance_enum_type uim_instance
);


/**
*  This function gets the customized LDO settings from NV.
*
*  @param uim_instance      uim instance.
*/
void uimdrv_pmic_efs_get_user_ldo_config
(
  uim_instance_enum_type uim_instance
);


/**
*  This function is used to issue a request to npa to set the
*  UIM LDO to the desired voltage.
*  
*  @param uim_instance   uim instance.
*  @param npa_handle     Client resource handle.
*  @param req            Required request.
*/
void uimdrv_pmic_set_ldo_voltage
(
  uim_instance_enum_type uim_instance,
  npa_client_handle  npa_handle, 
  npa_resource_state req
);


/**
*  This function is used to set the UIM LDO into HPM (High power
*  mode) during UIM activity and to LPM (Low power mode) during
*  inactivity.
*
*  @param uim_ptr    Pointer to the current instance of the UIM
*                    global structure.
*  @param powermode  Power mode (High power mode or Low power
*                    mode)
*/
void uimdrv_pmic_vote_for_ldo_powermode
(
  uim_instance_global_type *uim_ptr,
  boolean req
);


/**
*  This function is used to set the voltage class uim global
*  variable.
*
*  @param uim_ptr         Pointer to the current instance of the
*                         UIM global structure.
*  @param voltage_class   Voltage class
*/
void uim_program_voltage_class
(
  uim_instance_global_type *uim_ptr,
  uim_voltage_class_type    voltage_class
);


/**
*  This function is used to power off the ldo.
*
*  @param uim_ptr      Pointer to the current instance of the
*                      UIM global structure.
*/
void uim_power_off_ldo_slot
(
  uim_instance_global_type *uim_ptr
);


/**
*  This function is used to power on the UIM ldo and set the
*  Dual GPIO pads to the appropriate voltage.
*
*  @param uim_ptr      Pointer to the current instance of the
*                      UIM global structure.
*/
void uim_power_on_ldo_slot
(
  uim_instance_global_type *uim_ptr
);


/**
*  This function is used to control UICC shutdown upon card
*  removal and UICC shutdown upon battery removal
*
*  @param uim_instance    uim instance.
*  @param enable          TRUE - indicates that the we are
*                         attempting to enable the uicc shutdown
*/
void uim_set_shutdown_control
(
  uim_instance_enum_type uim_instance,
  boolean enable
);


/**
*  This function is used to return the pmic npa node.
*
*  @param uim_instance    uim instance.
*/
uint32 uim_get_pmic_npa_mode
(
  uim_instance_enum_type uim_instance
);


/**
*  This function sets the voltage regulator with the required
*  voltage in PMIC language, the value is based on the desired
*  voltage by the user that is set in NV. The voltage regulator
*  value is passed while making a npa request.
*
*  @param uim_instance      uim instance.
*/
void uimdrv_pmic_set_voltage_regulator_settings
(
  uim_instance_enum_type         uim_instance
);
#endif /* UIMDRV_PMIC_H */
