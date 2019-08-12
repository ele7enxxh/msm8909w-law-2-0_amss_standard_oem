/*============================================================================
  FILE:         uimdrv_pmic.c

  OVERVIEW:     File conatins the functions used to initialize and control
                the pmic interface.

  DEPENDENCIES: N/A

                Copyright (c) 2012 - 2016 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/pmic/uimdrv_pmic.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
05/30/16    gm         F3 message removal
05/23/16    dd         Enhancing the voltage supply indication to QMI feature
12/10/15    ks         8917 bringup changes
09/16/15    ks         8937 bringup changes
06/01/15    sam        9x07 bringup changes
05/08/15    sam        Configure LDO out of Reset state in every UIM power up
02/23/15    ks         Not wait for QMI ACK after receiving TASK_STOP
01/08/14    ks         Compile warnings
01/05/15    ks         supoport for UIM3 external LDO on 8909
11/12/14    js         Fixed compiler warning
11/11/14    ks         Using the DAL chip enums instad of hardcoding values
11/10/14    akv        Support for user to customize LDO settings
11/06/14    nmb        8952 bring up changes
09/23/14    ks         8909 bring up changes
09/22/14    akv        UIMDRV UT - Featurization while invoking of QMI API removed
09/18/14    akv        UIMDRV UT enhancements
08/28/14    akv        UIMDRV-QMI interactions - Indications and acks for LDO events
08/21/14    ak         Replace 9x45 DAL chip family reference for compilation on BOLT
08/20/14    nmb        9x45 Bring up changes
07/25/14    lxu        Read cmd_rsp_time from nv for usb mode, give cmd_rsp_time and
                       voltage_class to ap when send usb power up to ap
07/09/14    ak         Follow new PMIC requirement for enabling batt removal shutdown
05/12/14    ak         8994 Bring up changes
05/06/14    ks         8936 bring up changes
04/11/14    lxu        Unregister automatic UICC shutdown on card removal in USB mode
02/18/14    sam        Usage of newer style diag macros
01/24/14    yk         Code Clean up
12/27/13    ks         8916 bring up changes
11/27/13    akv        Initialization of hw enumeration based on dal chip family
10/08/13    nmb        HW header file cleanup
10/03/13    sam        F3 log reduction
09/12/12    na         8x62 bring up changes
08/29/13    na         allow UIM2 config on UIM1 when UIM1 is disabled and UIM2 enabled
08/06/13    rm/ak      Bolt RUMI code
07/12/13    nmb        Global Reorganization
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
06/24/13    js         TSTS changes
05/22/13    akv        Configuration of UICC shutdown gpio's and feature
                       enablement based on NV
05/16/13    akv        Fix for compilation warning on Triton
03/27/13    akv        UICC shutdown upon battery removal
03/18/13    js         8x26 UIM3 interface bring up fixes.
02/22/13    js         8x26 bring up changes
02/12/13    js         General F3 macro cleanup
02/01/13    akv        Run time configuration of UICC shutdown feature
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_pmic.h"
#include "uimdrv_uartdm.h"
#include "uimdrv_rumi.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "uimglobals.h"
#include "uim_common_efs.h"
#include "qmi_uim.h"
#include "uimdrv_gpio.h"

static char pmic_npa_group_id_uim_enable[][30]      =
   {PMIC_NPA_GROUP_ID_UIM1_ENABLE, PMIC_NPA_GROUP_ID_UIM2_ENABLE,
    PMIC_NPA_GROUP_ID_UIM3_ENABLE};
static char pmic_npa_group_id_uim_activity[][30]    =
   {PMIC_NPA_GROUP_ID_UIM1_ACTIVITY, PMIC_NPA_GROUP_ID_UIM2_ACTIVITY,
   PMIC_NPA_GROUP_ID_UIM3_ACTIVITY};

/* Table holding the default PMIC NPA reference for different UIM instances. */

const uim_pmic_npa_resource_table_type   uim_pmic_npa_resource_table[] = {

  /* ARAGORN_8974 */  { DALCHIPINFO_FAMILY_MSM8974,
                         /*  UIM1_UART_NUM,           UIM2_UART_NUM,             UIM3_UART_NUM,           UIM4_UART_NUM  */
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* ARAGORN_8974 */

    /* ARAGORN_PRO */ { DALCHIPINFO_FAMILY_MSM8974_PRO,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* ARAGORN_PRO */

     /* ELAN_9X25 */  { DALCHIPINFO_FAMILY_MDM9x25,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* ELAN_9X25 */

     /* DINO_8X10 */  { DALCHIPINFO_FAMILY_MSM8x10,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, E_PMIC_NPA_RESOURCE_UIM3, INVALID_PMIC_NPA_RESOURCE }
                      },  /* DINO_8X10 */

     /* FRODO_8X26 */ { DALCHIPINFO_FAMILY_MSM8x26,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, E_PMIC_NPA_RESOURCE_UIM3, INVALID_PMIC_NPA_RESOURCE }
                      },  /* FRODO_8X26 */

     /* GIMLI_8926 */ { DALCHIPINFO_FAMILY_MSM8926,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, E_PMIC_NPA_RESOURCE_UIM3, INVALID_PMIC_NPA_RESOURCE }
                      },  /* GIMLI_8926 */

     /* TORINO_9X35 */{ DALCHIPINFO_FAMILY_MDM9x35,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* TORINO_9X35 */

     /* THORIN_8X62 */{ DALCHIPINFO_FAMILY_MSM8x62,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* THORIN_8X62 */

  /* BAGHEERA_8916 */ { DALCHIPINFO_FAMILY_MSM8916,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, E_PMIC_NPA_RESOURCE_UIM3, INVALID_PMIC_NPA_RESOURCE }
                      },  /* BAGHEERA_8916 */

     /* SHERE_8936 */ { DALCHIPINFO_FAMILY_MSM8936,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, E_PMIC_NPA_RESOURCE_UIM3, INVALID_PMIC_NPA_RESOURCE }
                      },  /* SHERE_8936 */

    /* ELESSAR_8994 */{ DALCHIPINFO_FAMILY_MSM8994,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* ELESSAR_8994 */

    /* TESLA_9x45 */  { DALCHIPINFO_FAMILY_MDM9x45,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* TESLA_9x45 */

   /* CHEEL_8909 */   { DALCHIPINFO_FAMILY_MSM8909,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* CHEEL_8909 */

   /* TABASCO/SAHI*/  { /* DALCHIPINFO_FAMILY_MSM8952 */ 60,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, E_PMIC_NPA_RESOURCE_UIM3, INVALID_PMIC_NPA_RESOURCE }
                      },  /* TABASCO_SAHI_8952 */

   /* LYKAN_9x07 */   { DALCHIPINFO_FAMILY_MDM9x07,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* CHEEL_8909 */

   /* FEERO_8937 */   { DALCHIPINFO_FAMILY_MSM8937,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* FEERO_8937 */

/* FEERO_LITE_8917 */ { DALCHIPINFO_FAMILY_MSM8917,
                       { E_PMIC_NPA_RESOURCE_UIM1, E_PMIC_NPA_RESOURCE_UIM2, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      },  /* FEERO_LITE_8917 */

    /* DALCHIPINFO_FAMILY_UNKNOWN */{ DALCHIPINFO_FAMILY_UNKNOWN,
                       { INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE, INVALID_PMIC_NPA_RESOURCE }
                      }  /* DALCHIPINFO_FAMILY_UNKNOWN */

}; /* uim_pmic_npa_resource_table[MAX_UIM_CHIPSET_INDEX] */


/**
*  This function does the initial handshaking needed with PMIC
*  to get all client handles used to communicate with PMIC
*  further.
*
*  @param uim_instance   uim instance.
*/
void initialize_pmic(uim_instance_enum_type uim_instance)
{
  uimVccEnum                  pmic_ref;

  RETURN_IF_INSTANCE_INVALID(uim_instance);

  uim_hw_if.pmic[uim_instance].m_pmicNpaHandle = NULL;
  uim_hw_if.pmic[uim_instance].m_pmicActivityHandle  = NULL;

  pmic_ref = m_HWConfig.uim_slot_config[uim_instance].uimVcc;

  /* store the instance of the slot that are active */
  uim_hw_if.pmic[uim_instance].m_Vcc                 = pmic_ref;
  uim_hw_if.pmic[uim_instance].m_uiccShutBattRemoval = m_HWConfig.uicc_shutdown_feature.uiccShutdownBattRemoval;
  uim_hw_if.pmic[uim_instance].m_uiccShutCardRemoval = m_HWConfig.uicc_shutdown_feature.uiccShutdownCardRemoval[uim_instance];

  uimdrv_pmic_efs_get_user_ldo_config(uim_instance);

  uimdrv_pmic_set_voltage_regulator_settings(uim_instance);

  uim_hw_if.pmic[uim_instance].m_pmicNpaHandle =
    npa_create_sync_client(pmic_npa_group_id_uim_enable[pmic_ref],
                           "uim", NPA_CLIENT_REQUIRED);
  uim_hw_if.pmic[uim_instance].m_pmicActivityHandle =
    npa_create_sync_client (pmic_npa_group_id_uim_activity[pmic_ref],
                            "uim", NPA_CLIENT_REQUIRED);

  UIMDRV_MSG_ERR_2 ( uim_instance,"UIM pmicNpaHandle: 0x%x  pmicActivityHandle: 0x%x for pmic instance", 
                     uim_hw_if.pmic[uim_instance].m_pmicNpaHandle, uim_hw_if.pmic[uim_instance].m_pmicActivityHandle);
} /* initialize_pmic */


/**
*  This function gets the customized LDO settings from NV.
*
*  @param uim_instance      uim instance.
*/
void uimdrv_pmic_efs_get_user_ldo_config(uim_instance_enum_type uim_instance)
{
  uimVccEnum                  pmic_ref = INVALID_PMIC_NPA_RESOURCE;
  uim_instance_global_type*   uim_ptr = NULL;

  RETURN_IF_INSTANCE_INVALID(uim_instance);

  pmic_ref = m_HWConfig.uim_slot_config[uim_instance].uimVcc;

  uim_ptr = uim_get_instance_ptr(uim_instance);

  /* === Read the required UIM HW LDO configuration from EFS === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    (uim_common_efs_context_type)pmic_ref,
                                                    (uint8 *)&(uim_ptr->hardware.uim_ldo),
                                                    sizeof(uim_ptr->hardware.uim_ldo)))
  {
  }
  else
  {
    uim_ptr->hardware.uim_ldo.customize_ldo_setting                 = FALSE;
    uim_ptr->hardware.uim_ldo.desired_voltage_class                 = UIM_VOLTAGE_CLASS_INVALID;
    uim_ptr->hardware.uim_ldo.desired_voltage                       = PMIC_UIM_VOLTAGE_INVALID;
    uim_ptr->hardware.uim_ldo.pdown_allowed_upon_card_removal       = FALSE;
    uim_ptr->hardware.uim_ldo.pdown_allowed_upon_error              = FALSE;

    if (UIM_COMMON_EFS_SUCCESS != uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS,
                                                       UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                       (uim_common_efs_context_type)pmic_ref,
                                                       (const char*)&(uim_ptr->hardware.uim_ldo),
                                                       sizeof(uim_ptr->hardware.uim_ldo)))
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Failed writing default values to UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS");
    }
  }
} /* uimdrv_pmic_efs_get_user_ldo_config */


/**
*  This function sets the voltage regulator with the required
*  voltage in PMIC language, the value is based on the desired
*  voltage by the user that is set in NV. The voltage regulator
*  value is passed while making a npa request.
*
*  @param uim_instance      uim instance.
*/
void uimdrv_pmic_set_voltage_regulator_settings(uim_instance_enum_type  uim_instance)
{
  uim_instance_global_type*   uim_ptr = NULL;

  RETURN_IF_INSTANCE_INVALID(uim_instance);

  uim_ptr = uim_get_instance_ptr(uim_instance);

  if (uim_ptr == NULL)
  {
    UIMDRV_MSG_ERR_0(0,"uimdrv_pmic_set_required_voltage_regulator_settings - uim_ptr is NULL");
    return;
  }

  /* Initialize the voltage regulator to have the default settings */
  uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B;
  uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C;

  if (uim_ptr->hardware.uim_ldo.customize_ldo_setting == FALSE ||
      uim_ptr->hardware.uim_ldo.desired_voltage == PMIC_UIM_VOLTAGE_INVALID)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"No customized voltage regulator settings");
    return;
  }

  switch (uim_ptr->hardware.uim_ldo.desired_voltage)
  {
    case PMIC_UIM_ACTIVE_CLASS_B_LOW:
      uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_LOW;
      break;

    case PMIC_UIM_ACTIVE_CLASS_B:
      uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B;
      break;

    case PMIC_UIM_ACTIVE_CLASS_B_HIGH:
      uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_HIGH;
      break;

    case PMIC_UIM_ACTIVE_CLASS_C_LOW:
      uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_LOW;
      break;

    case PMIC_UIM_ACTIVE_CLASS_C:
      uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C;
      break;

    case PMIC_UIM_ACTIVE_CLASS_C_HIGH:
      uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_HIGH;
      break;

    default:
      ERR_FATAL("Incorrect voltage setting",0,0,0);
  }
}/* uimdrv_pmic_set_voltage_regulator_settings */


/**
*  This function powers up the UIM LDO and setting the dual
*  voltage UIM GPIO pads to the appropriate voltage level.
*
*  @param uim_instance   uim instance.
*/
boolean uimPowerOn(uim_instance_enum_type uim_instance)
{
  if(uim_instance>= UIM_MAX_INSTANCES)
  {
    return FALSE;
  }

  if(!(uim_hw_if.pmic[uim_instance].m_pmicNpaHandle) && (uim_hw_if.uim_is_external_ldo[uim_instance] == FALSE))
  {
    UIMDRV_MSG_ERR_0 ( uim_instance,"uimPowerOn - NULL pmicNpaHandle detected");
    return FALSE;
  }

  uim_set_shutdown_control(uim_instance, TRUE);

  if (UIM_VOLTAGE_CLASS_C == uim_hw_if.pmic[uim_instance].m_voltage_class)
  {
    if(uim_hw_if.uim_is_external_ldo[uim_instance] == TRUE)
    {
      uimConfigureUimExtLdoToClassC(uim_instance); 
      return TRUE;
    }
   
    if (uim_hw_if.pmic[uim_instance].m_pmicNpaHandle)
    {
      uimdrv_pmic_set_ldo_voltage(uim_instance,
                                  uim_hw_if.pmic[uim_instance].m_pmicNpaHandle,
                                  uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator);
      uim_hw_if.pmic[uim_instance].m_pmic_npa_mode = uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator;
    }

    uimdrv_rumi_vsel_on_class_c(uim_instance);

    /* Wait to ensure that the 20ns sequencing requirement from the pad
       specification is met */
    uim_clk_busy_wait(1);

    prgUIMCFG(uim_instance, 0, TRUE, UIM_SET_MODE_18, SET);
  }
  else if(UIM_VOLTAGE_CLASS_B == uim_hw_if.pmic[uim_instance].m_voltage_class)
  {
    if(uim_hw_if.uim_is_external_ldo[uim_instance] == TRUE)
    {
      uimConfigureUimExtLdoToClassB(uim_instance); 
      return TRUE;
    }

    prgUIMCFG(uim_instance, 0, TRUE, UIM_CLR_MODE_18, CLR);

    /* Wait to ensure that the 20ns sequencing requirement from the pad
       specification is met */
    uim_clk_busy_wait(1);

    if (uim_hw_if.pmic[uim_instance].m_pmicNpaHandle)
    {
      uimdrv_pmic_set_ldo_voltage(uim_instance,
                                  uim_hw_if.pmic[uim_instance].m_pmicNpaHandle,
                                  uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator);
      uim_hw_if.pmic[uim_instance].m_pmic_npa_mode = uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator;
    }

    uimdrv_rumi_vsel_on_class_b(uim_instance);
  }
  uim_clk_busy_wait(200);
  return TRUE;
} /* uimPowerOn */


/**
*  This function powers down the UIM LDO.
*
*  @param uim_instance   uim instance.
*/
boolean uimPowerOff(uim_instance_enum_type uim_instance)
{
  if(!(uim_hw_if.pmic[uim_instance].m_pmicNpaHandle) && (uim_hw_if.uim_is_external_ldo[uim_instance] == FALSE))
  {
    UIMDRV_MSG_ERR_0(uim_instance,"uimPowerOff - NULL pmicNpaHandle detected");
    return FALSE;
  }

  if(uim_hw_if.uim_is_external_ldo[uim_instance] == TRUE)
  {
    uimConfigureUimExtLdoToZero(uim_instance); 
    return TRUE;
  }
  uimdrv_pmic_set_ldo_voltage(uim_instance,
                              uim_hw_if.pmic[uim_instance].m_pmicNpaHandle,
                              PMIC_NPA_MODE_ID_UIM_STANDBY);
  uimdrv_rumi_vsel_off(uim_instance);
  return TRUE;
} /* uimPowerOff */


/**
*  This function is used to evaluate if the required voltage
*  request on the desired UIM LDO is allowed based on the
*  clients configuration settings of the UIM LDO.
*
*  @param  uim_instance  uim instance id.
*  @param  req           Required request.
*  @return boolean       TRUE - indicates to the calling process
*                        that the ldo bahviour can be modified.
*/
boolean uim_evaluate_req_ldo_voltage_mod(uim_instance_enum_type uim_instance,
                                         npa_resource_state     req)
{
  uim_instance_global_type*   uim_ptr = NULL;

  if (uim_instance >= UIM_MAX_INSTANCES)
  {
    UIM_MSG_ERR_1("Invalid input param instance id 0x%x", uim_instance);
    return FALSE;
  }

  uim_ptr = uim_get_instance_ptr(uim_instance);

  if (uim_ptr == NULL)
  {
    return FALSE;
  }

  /* 1. If the LDO behaviour settings are NOT customized by the user,
        then we will simply return TRUE.
     2. In the case of the Recovery, all LDO modifications made by the user
        with respect to keeping the LDO active all the time is disregarded.
        The settings are honoured after recovery. Hence we return TRUE.
     If recovery is in progress and UIM state is not in error this would
     imply that recovery is still being attempted.
     When we fail recovery the state is set to UIM_ERR_S and the recovery triggered
     flag is cleared later and at point the users requests are entertained. */
  if ( uim_ptr->hardware.uim_ldo.customize_ldo_setting == FALSE ||
        (uim_ptr->flag.recovery_triggered == TRUE &&
         uim_ptr->state.status != UIM_ERR_S) )
  {
    UIMDRV_MSG_MED_0(uim_ptr->id,"Recovery in progress or ldo behavior not modified");
    return TRUE;
  }

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"uim_evaluate_req_ldo_voltage_mod - Evaluating req - 0x%x", req);

  switch (req)
  {
    case PMIC_NPA_MODE_ID_UIM_STANDBY:
    {
      if (uim_ptr->hardware.uim_ldo.pdown_allowed_upon_card_removal == FALSE &&
          uim_ptr->hardware.uim_ldo.pdown_allowed_upon_error == FALSE)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"LDO PDown allowed under all conditions");
        return TRUE;
      }

      if (uim_ptr->hardware.uim_ldo.pdown_allowed_upon_card_removal == TRUE &&
          uim_ptr->command.cmd_ptr != NULL &&
          uim_ptr->command.cmd_ptr->hdr.command == UIM_HOTSWAP_CARD_REM_F)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"LDO PDown allowed only upon card removal and card is removed");
        return TRUE;
      }

      if (  uim_ptr->hardware.uim_ldo.pdown_allowed_upon_error == TRUE &&
            uim_ptr->state.status != UIM_INITIALIZED_S &&
            uim_ptr->state.status != UIM_CLK_OFF_S )
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"LDO Power Down only upon error UIM state is 0x%x", uim_ptr->state.status);
        return TRUE;
      }
      break;
    }

    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_LOW:
    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C:
    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_HIGH:
    {
      if (uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_INVALID ||
          uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_C)
      {
        return TRUE;
      }
      break;
    }

    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_LOW:
    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B:
    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_HIGH:
    {
      if (uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_INVALID ||
          uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_B)
      {
        return TRUE;
      }
      break;
    }

    default:
      ERR_FATAL("UIM_%d uim_evaluate_req_ldo_voltage_mod - vote unrecognized",uim_ptr->id,0,0);
  }
  return FALSE;
} /* uim_evaluate_req_ldo_voltage_mod */


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
void uimdrv_pmic_vote_for_ldo_powermode(uim_instance_global_type  *uim_ptr,
                                        uim_ldo_powermodes_type   powermode)
{
  if(!(uim_hw_if.pmic[uim_ptr->id].m_pmicActivityHandle))
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"uim_vote_for_ldo_powermode - NULL pmicActivityHandle detected");
    return;
  }
  npa_issue_required_request(uim_hw_if.pmic[uim_ptr->id].m_pmicActivityHandle,
                             powermode);
  return;
} /* uimdrv_pmic_vote_for_ldo_powermode */


/**
*  This function is used to issue a request to npa to set the
*  UIM LDO to the desired voltage.
*
*  @param uim_instance   uim instance.
*  @param npa_handle     Client resource handle.
*  @param req            Required request.
*/
void uimdrv_pmic_set_ldo_voltage(uim_instance_enum_type  uim_instance,
                                 npa_client_handle       npa_handle,
                                 npa_resource_state      req)
{
  boolean ldo_behaviour_modification_allowed = FALSE;

  if(!(npa_handle))
  {
    UIMDRV_MSG_ERR_0(uim_instance,"uim_vote_for_ldo_activation_or_deactivation - NULL npa_handle detected");
    return;
  }

  ldo_behaviour_modification_allowed = uim_evaluate_req_ldo_voltage_mod(uim_instance, req);

  if(ldo_behaviour_modification_allowed)
  {
    npa_issue_required_request(npa_handle, req);
  }
  return;
} /* uimdrv_pmic_set_ldo_voltage */


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
)
{
  uim_hw_if.pmic[uim_ptr->id].m_voltage_class = voltage_class;
  return;
} /* uim_program_voltage_class */


/*===========================================================================
FUNCTION UIMDRV_POWER_MANAGER_WAIT_FOR_QMI_ACK

DESCRIPTION
  Power manager decides to either wait on the required ACK from QMI_UIM
  for an indication that was sent or proceed.
===========================================================================*/
void uimdrv_power_manager_wait_for_qmi_ack(uim_instance_global_type          *uim_ptr,
                                           uimdrv_qmi_acknowledgements_type  required_ack)
{
  if( required_ack == UIMDRV_QMI_IND_ACK_NOT_REQUIRED )
  {
    return;
  }
  uimdrv_wait_for_qmi_acknowledgement(uim_ptr, required_ack);
} /* uimdrv_power_manager_wait_for_qmi_ack */


/*===========================================================================
FUNCTION UIMDRV_QMI_POWER_MANAGEMENT_CALLBACK

DESCRIPTION
  Callback to manage the acknowledgements from QMI that are related to UIM LDO.
  The acknowledgements are for the ldo indications that we sent to QMI_UIM.
===========================================================================*/
static void uimdrv_qmi_power_management_callback(uim_slot_type  uim_slot,
                                                 uimdrv_qmi_acknowledgements_type received_ack)
{
  uim_instance_global_type *uim_ptr = NULL;
  uim_instance_enum_type uim_instance = (uim_instance_enum_type) uim_slot - 1;

  RETURN_IF_INSTANCE_INVALID(uim_instance);

  uim_ptr = uim_get_instance_ptr(uim_instance);

  if (received_ack == UIMDRV_QMI_INVALID_IND_ACK)
  {
    return;
  }

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"UIMDRV received ACK from QMI - 0x%x",received_ack);

  /* ACK received from QMI for an Indication that was sent.
     The supplied callback would match the required ack and received ack and
     sets the UIM_QMI_ACK_RECEIVED_SIG accordingly. */

  uim_ptr->uim_qmi_interaction.received_ack_from_qmi = received_ack;

  if ( uim_ptr->uim_qmi_interaction.required_ack_from_qmi == uim_ptr->uim_qmi_interaction.received_ack_from_qmi )
  {
    rex_set_sigs(uim_ptr->tcb_ptr, UIM_QMI_ACK_RECEIVED_SIG);
  }

} /* uimdrv_qmi_power_management_callback */


/**
*  This function is used to power off the ldo.
*
*  @param uim_ptr      Pointer to the current instance of the
*                      UIM global structure.
*/
void uim_power_off_ldo_slot(uim_instance_global_type *uim_ptr)
{
  if(!uim_ptr->flag.powering_down_task)
  {
    qmi_uim_supply_voltage_ind((uim_ptr->id + 1),
                              UIMDRV_LDO_AWAITING_DEACTIVATION_IND,
                              uimdrv_qmi_power_management_callback);
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"UIM_POWER_OFF_SLOT notified QMI with Indication - 0x%x",UIMDRV_LDO_AWAITING_DEACTIVATION_IND);
    if(uim_ptr->setting.voltage_indication_to_qmi.is_ack_needed)
    {
      uimdrv_power_manager_wait_for_qmi_ack( uim_ptr, UIMDRV_LDO_AWAITING_DEACTIVATION_IND_ACK );
    }
  }
  uimdrv_rumi_config_gpios(uim_ptr->id);
  uimPowerOff(uim_ptr->id);
  return;
} /* uim_power_off_ldo_slot */


/**
*  This function is used to power on the UIM ldo and set the
*  Dual GPIO pads to the appropriate voltage.
*
*  @param uim_ptr      Pointer to the current instance of the
*                      UIM global structure.
*/
void uim_power_on_ldo_slot(uim_instance_global_type *uim_ptr)
{
  uimdrv_rumi_config_gpios(uim_ptr->id);
  uimPowerOn(uim_ptr->id);
  qmi_uim_supply_voltage_ind ( (uim_ptr->id + 1), UIMDRV_LDO_ACTIVATED_IND, NULL);
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"UIM_POWER_ON_SLOT notified QMI with Indication - 0x%x",UIMDRV_QMI_IND_ACK_NOT_REQUIRED);
  uimdrv_power_manager_wait_for_qmi_ack( uim_ptr, UIMDRV_QMI_IND_ACK_NOT_REQUIRED );
  return;
} /* uim_power_on_ldo_slot */


/**
*  This function is used to control UICC shutdown upon card
*  removal and UICC shutdown upon battery removal
*
*  @param uim_instance    uim instance.
*  @param enable          TRUE - indicates that the we are
*                         attempting to enable the uicc shutdown
*/
void uim_set_shutdown_control(uim_instance_enum_type uim_instance, boolean enable)
{
  pm_err_flag_type pm_err = PM_ERR_FLAG__SUCCESS;
  /* UICC Shutdown upon card removal */
  /*
     PMIC interface expects UIM to call pm_uicc_cntrl_enable with parameters
      -- 1 and TRUE for enabling UICC shutdown upon UIM1 card removal,
      -- 2 and TRUE for enabling UICC shutdown upon UIM2 card removal,
      -- 3 and TRUE for enabling UICC shutdown upon UIM3 card removal.
  */
  UIMDRV_MSG_HIGH_3(uim_instance, "UICC set shutdown upon card removal for slot is %d. m_uiccShutCardRemoval = %d, m_hs_enable = %d",
                 enable,
                 uim_hw_if.pmic[uim_instance].m_uiccShutCardRemoval,
                 uim_hw_if.intctrl[uim_instance].m_hs_enable);

  /* Bring UIM LDO out of reset state in every UIM power up */
  pm_err = pm_uicc_cntrl_enable(uim_hw_if.pmic[uim_instance].m_Vcc + 1, FALSE);
  if (TRUE == uim_hw_if.pmic[uim_instance].m_uiccShutCardRemoval &&
      TRUE == uim_hw_if.intctrl[uim_instance].m_hs_enable )
  {
    pm_err = pm_uicc_cntrl_enable(uim_hw_if.pmic[uim_instance].m_Vcc + 1, enable);
    if (PM_ERR_FLAG__SUCCESS != pm_err)
    {
      UIMDRV_MSG_ERR_1(uim_instance, "PMIC UICC card removal enable failed: %d",pm_err);
    }
  }

  /* UICC Shutdown upon battery removal */
  /*
     PMIC interface expects UIM to call pm_uicc_cntrl_enable with parameters
      -- 0 and TRUE for enabling battery removal batt alarm
      -- 1 and TRUE for enabling UICC shutdown for UIM1
      -- 2 and TRUE for enabling UICC shutdown for UIM2
      -- 3 and TRUE for enabling UICC shutdown for UIM3
  */
  UIMDRV_MSG_HIGH_2(uim_instance, "UICC set shutdown upon batt removal for slot is %d. m_uiccShutBattRemoval = %d",
                         enable,uim_hw_if.pmic[uim_instance].m_uiccShutBattRemoval);
  if (TRUE == uim_hw_if.pmic[uim_instance].m_uiccShutBattRemoval)
  {
    pm_err = pm_uicc_cntrl_enable(PM_BUA_BATT_ALARM, enable);
    if (PM_ERR_FLAG__SUCCESS != pm_err)
    {
      UIMDRV_MSG_ERR_1(uim_instance, "PMIC UICC BATT ALARM enable failed: %d",pm_err);
    }

    pm_err = pm_uicc_cntrl_enable(uim_hw_if.pmic[uim_instance].m_Vcc + 1, enable);
    if (PM_ERR_FLAG__SUCCESS != pm_err)
    {
      UIMDRV_MSG_ERR_1(uim_instance, "PMIC UICC BATT ALARM instance enable failed: %d",pm_err);
    }
  }
  return;
} /* uim_set_shutdown_control */


/**
*  This function is used to return the pmic npa node.
*
*  @param uim_instance    uim instance.
*/
uint32 uim_get_pmic_npa_mode(uim_instance_enum_type uim_instance)
{
  return uim_hw_if.pmic[uim_instance].m_pmic_npa_mode;
}/* uim_get_pmic_npa_mode */
