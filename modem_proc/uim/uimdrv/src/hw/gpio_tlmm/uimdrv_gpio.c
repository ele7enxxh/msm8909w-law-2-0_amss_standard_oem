/*============================================================================
  FILE:         uimdrv_gpio.c

  OVERVIEW:     Contains functions used by uim driver to control and initialize
                gpio/tlmm interface

  DEPENDENCIES: N/A

                Copyright (c) 2012 - 2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.  Please
use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/gpio_tlmm/uimdrv_gpio.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

 when       who        what, where, why
------      ----       -----------------------------------------------------------
03/12/15    ks         SIM tray by tieing the card detect gpios
01/08/14    ks         Compile warnings
01/05/15    ks         supoport for UIM3 external LDO on 8909
12/03/14    sam        Incorporate the newly added TLMM API to read back GPIO configs
10/31/14    nmb        Correct slot typecasting
09/12/14    ssr        Fix to initialize the Hotswap GPIO when Hotswap feature
                       is enabled
08/25/14    ll         Fix compile warnings medium
06/16/14    ll         Switch to new string based TLMM APIs
05/12/14    ak         8994 Bring up changes
05/06/14    ks         8936 bring up changes
04/21/14    rm/akv     Fix for glitch seen on the IO line while powering down
01/29/14    sam        Usage of newer style diag macros
12/27/13    ks         8916 bring up changes
11/27/13    akv        Initialization of hw enumeration based on dal chip family
10/22/13    ll         Check UIM GPIOs as inputs upon timeout w/recovery disabled
10/08/13    nmb        HW header file cleanup
10/08/13    nmb        Changed default card detect GPIO to pull up
09/11/13    na         8x62 bring up changes
08/06/13    rm/ak      Bolt RUMI code
07/12/13    nmb        Global Reorganization
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
05/23/13    ak         Apply NV values for PULL and DRIVE STRENGTH for UIM1 dedicated
                       UIM controller pads
05/23/13    ak         Reset CLK, DATA, and RESET PULL to NO-PULL when setting to
                       UIM control
05/22/13    akv        Warm reset fixes for Triton
02/21/13    ak         Use UIM instance number for GPIO configuration logic
02/21/13    ak         UIM1 support on 9x25 v2
02/12/13    js         General F3 macro cleanup
12/05/12    js         UIM Parallel Processing changes
11/21/12    rm         Fix to set the GPIO state to LOW after configuring it them
                       to TLMM ownership
11/09/12    akv        BATT ALARM configuration for Dime
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_gpio.h"
#include "uimglobals.h"

/* NOTE: X will be updated based on uim_instance in runtime in updateGpioNameWithInstance() */
#define UIM_GPIO_DATA_NAME                       "uimX_data"
#define UIM_GPIO_CLK_NAME                        "uimX_clk"
#define UIM_GPIO_RESET_NAME                      "uimX_reset"
#define UIM_GPIO_PRESENT_NAME                    "uimX_present"
#define UIM_GPIO_BATT_ALARM_NAME                 "uim_batt_alarm"
#define UIM_GPIO_EXT_LDO_EN                      "uimX_ldo_en"
#define UIM_GPIO_EXT_LDO_VSEL                    "uimX_ldo_vsel"

typedef struct
{
  char DataGpioName[UIM_MAX_GPIO_NAME_LEN];
  char ClkGpioName[UIM_MAX_GPIO_NAME_LEN];
  char ResetGpioName[UIM_MAX_GPIO_NAME_LEN];
  char PresentGpioName[UIM_MAX_GPIO_NAME_LEN];
  char BattAlarmGpioName[UIM_MAX_GPIO_NAME_LEN];
  char UimExtLdoEnGpioName[UIM_MAX_GPIO_NAME_LEN];
  char UimExtLdoVselGpioName[UIM_MAX_GPIO_NAME_LEN];
}uim_gpios_name_type;

static DALGpioIdType disabled_uim_card_presence_gpio_id;

/*
  @brief Add instant number to GPIO name string based on physical slot number
         This physical slot number should retreive from uimUart
*/
void updateGpioNameWithInstance
(
  uim_gpios_name_type *pGpiosName,
  uim_slot_type uim_slot
)
{
  memset(pGpiosName, 0, sizeof(uim_gpios_name_type));
  strlcpy(pGpiosName->DataGpioName, UIM_GPIO_DATA_NAME, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->DataGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->ClkGpioName, UIM_GPIO_CLK_NAME, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->ClkGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->ResetGpioName, UIM_GPIO_RESET_NAME, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->ResetGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->PresentGpioName, UIM_GPIO_PRESENT_NAME, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->PresentGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->BattAlarmGpioName, UIM_GPIO_BATT_ALARM_NAME, UIM_MAX_GPIO_NAME_LEN);

  strlcpy(pGpiosName->UimExtLdoEnGpioName, UIM_GPIO_EXT_LDO_EN, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->UimExtLdoEnGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->UimExtLdoVselGpioName, UIM_GPIO_EXT_LDO_VSEL, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->UimExtLdoVselGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
}/* update_gpio_name_with_instance */


/*
  @brief Initialize GPIO: populate device data
*/
void initialize_gpio(uim_instance_enum_type uim_instance)
{
  DALResult             eRes = DAL_SUCCESS;
  uim_gpios_name_type   GpiosName;
  uim_slot_type         physical_slot;

  uim_instance_global_type *uim_ptr;
  uim_ptr = uim_get_instance_ptr(uim_instance); 
  uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr = NULL;

  if(uim_ptr == NULL)
  {
    return;
  }
  /* Attach TLMM and get TLMM handle for this uim instance */
  eRes = DAL_DeviceAttach(DALDEVICEID_TLMM, &(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr));
  if((NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr)||(eRes != DAL_SUCCESS))
  {
    UIMDRV_MSG_ERR_0(uim_instance,"Failed to register with TLMM DAL");
    return;
  }

  /* Mapping instance to string */
  physical_slot = (uim_slot_type)m_HWConfig.uim_slot_config[uim_instance].uimUart + 1;

  if(( UIM_SLOT_3 == physical_slot ) && 
      (DALCHIPINFO_FAMILY_MSM8909 == uimdrv_hw_nv_config_info.dalChipFamily) )
  {
    uim_hw_if.uim_is_external_ldo[uim_instance] = TRUE;
  }
  else
  {
    uim_hw_if.uim_is_external_ldo[uim_instance] = FALSE;
  }

  updateGpioNameWithInstance(&GpiosName, physical_slot);

  /* Get GpioId via string */
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.DataGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_DataGpioId));
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.ClkGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_ClkGpioId));
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.ResetGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_ResetGpioId));
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.PresentGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_PresentGpioId));
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.BattAlarmGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_BattAlarmGpioId));


  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_SIMTRAY_WITH_GPIOS_TIED,
                               uim_ptr) == TRUE)
  {
    if(uim_hw_slot_info.slot_status[UIM_INSTANCE_1].slot_enabled == FALSE)
    {
      DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
              "uim1_present",
              &(disabled_uim_card_presence_gpio_id));
    }
    else if(uim_hw_slot_info.slot_status[UIM_INSTANCE_2].slot_enabled == FALSE)
    {
      DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
          "uim2_present",
          &(disabled_uim_card_presence_gpio_id));
    }       
  }
  
   /*Populate device specific configuration data to gpio struct*/
  uim_hw_if.gpio[uim_instance].m_DataGpioConfigId.eDirection          = DAL_GPIO_OUTPUT;
  uim_hw_if.gpio[uim_instance].m_DataGpioConfigId.ePull               = (DALGpioPullType)m_HWConfig.uim_slot_config[uim_instance].uimDataGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_DataGpioConfigId.eDriveStrength      = (DALGpioDriveType)m_HWConfig.uim_slot_config[uim_instance].uimDataGpioDrvStrength;
  uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId.eDirection           = DAL_GPIO_OUTPUT;
  uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId.ePull                = (DALGpioPullType)m_HWConfig.uim_slot_config[uim_instance].uimClkGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId.eDriveStrength       = (DALGpioDriveType)m_HWConfig.uim_slot_config[uim_instance].uimClkGpioDrvStrength;
  uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId.eDirection         = DAL_GPIO_OUTPUT;
  uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId.ePull              = (DALGpioPullType)m_HWConfig.uim_slot_config[uim_instance].uimResetGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId.eDriveStrength     = (DALGpioDriveType)m_HWConfig.uim_slot_config[uim_instance].uimResetGpioDrvStrength;
  uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId.eDirection       = DAL_GPIO_INPUT;
  uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId.ePull            = (DALGpioPullType)m_HWConfig.uim_slot_config[uim_instance].uimCardDetectGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId.eDriveStrength   = (DALGpioDriveType)m_HWConfig.uim_slot_config[uim_instance].uimCardDetectGpioDrvStrength;
  uim_hw_if.gpio[uim_instance].m_BattAlarmGpioConfigId.eDirection     = DAL_GPIO_OUTPUT;
  uim_hw_if.gpio[uim_instance].m_BattAlarmGpioConfigId.ePull          = (DALGpioPullType)m_HWConfig.uimBattAlarmGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_BattAlarmGpioConfigId.eDriveStrength = (DALGpioDriveType)m_HWConfig.uimBattAlarmGpioDrvStrength;

  if(uim_hw_if.uim_is_external_ldo[uim_instance])
  {
    DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.UimExtLdoEnGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId));

    DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.UimExtLdoVselGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId));

    uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId.eDirection     = DAL_GPIO_OUTPUT;
    uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId.ePull            = (DALGpioPullType)NO_PULL;
    uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId.eDriveStrength   = (DALGpioDriveType)DS_2MA;

    uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId.eDirection     = DAL_GPIO_OUTPUT;
    uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId.ePull          = (DALGpioPullType)NO_PULL;
    uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId.eDriveStrength = (DALGpioDriveType)DS_2MA;
  }
} /* initialize_gpio */


/*
  @ brief Configures the DATA and CLOCK lines
*/
boolean enableUartInterface(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId     ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId));
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId));

  return TRUE;
} /* enableUartInterface */


/*
  @brief  Configures RESET GPIO.
*/
boolean setResetAsOutput(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId));

  return TRUE;
} /* setResetAsOutput */


/*
  @brief  Brings the RESET line LOW.
*/
boolean assertReset(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId )
  {
    return FALSE;
  }

  DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                    uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                    DAL_GPIO_LOW_VALUE);
  return TRUE;
}/* assertReset */


/*
  @brief Brings the RESET line HIGH.
*/
boolean deassertReset(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId )
  {
    return FALSE;
  }
  /* Switch the user of the RESET line to UIM controller */
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId));
  return TRUE;
}/* deassertReset */

/*
  @brief Configures hotswap gpios
*/
boolean configHotswapGpios(uim_instance_enum_type uim_instance)
{
  uim_instance_global_type *uim_ptr;
  uim_ptr = uim_get_instance_ptr(uim_instance); 

  if(uim_ptr == NULL)
  {
    return FALSE;
  }
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_PresentGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_PresentGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId));

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_SIMTRAY_WITH_GPIOS_TIED,
                               uim_ptr) == TRUE)
  {
    if((uim_hw_slot_info.slot_status[UIM_INSTANCE_1].slot_enabled == FALSE)
	  ||(uim_hw_slot_info.slot_status[UIM_INSTANCE_2].slot_enabled == FALSE)) 
    {
      DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
							    disabled_uim_card_presence_gpio_id,
							    UIM_TLMM_IO_MODE,
							    &(uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId));
    }
  }

  return TRUE;
} /* configHotswapGpios */


/* Configure Reset for GPIO/TLMM */
boolean uimConfigureResetDefaultGpio(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0  == uim_hw_if.gpio[uim_instance].m_ResetGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId));
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                          DAL_GPIO_LOW_VALUE);
  return TRUE;
} /* uimConfigureResetDefaultGpio */


/* Configure data for GPIO/TLMM */
boolean uimConfigureDataDefaultGpio(uim_instance_enum_type uim_instance)
{
  if( NULL ==uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId));
  DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     uim_hw_if.gpio[uim_instance].m_DataGpioId,
                     DAL_GPIO_LOW_VALUE);
  return TRUE;
} /* uimConfigureDataDefaultGpio */


/* configure clock for GPIO/TLMM */
boolean uimConfigureClkDefaultGpio(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId));
  DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                    uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                    DAL_GPIO_LOW_VALUE);
  return TRUE;
} /* uimConfigureClkDefaultGpio */


/* configure data for UIM controller */
boolean uimConfigureDataUimController(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId));
  return TRUE;
} /* uimConfigureDataUimController */


/* configure clock for UIM controller */
boolean uimConfigureClkUimController(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId));
  return TRUE;
} /* uimConfigureClkUimController */


/* configure bat alarm GPIO */
boolean uimConfigureBattAlarmUimController(uim_instance_enum_type uim_instance)
{
  /* Only configure once by the first task that get BattAlarm GpioId */
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_BattAlarmGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_BattAlarmGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_BattAlarmGpioConfigId));
  return TRUE;
} /* uimConfigureBattAlarmUimController */


/* Configure UIM3 Vcc to 1.8v */
boolean uimConfigureUimExtLdoToClassC(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                          DAL_GPIO_LOW_VALUE);


  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                          DAL_GPIO_HIGH_VALUE);

  return TRUE;
} /* uimConfigureUimExtLdoToClassC */


/* Configure UIM3 Vcc to 3v */
boolean uimConfigureUimExtLdoToClassB(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                          DAL_GPIO_HIGH_VALUE);

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                          DAL_GPIO_HIGH_VALUE);


  return TRUE;
} /* uimConfigureUimExtLdoToClassB */



/* Configure UIM3 Vcc to 0v */
boolean uimConfigureUimExtLdoToZero(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                          DAL_GPIO_LOW_VALUE);

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                          DAL_GPIO_LOW_VALUE);


  return TRUE;
} /* uimConfigureUimExtLdoToZero */


/* Configure all UIM lines as pull-up input GPIOs to log their values for debugging */
void UIM_CONFIGURE_ALL_UIM_AS_INPUT_PULLUP_GPIOS(uim_instance_enum_type uim_instance)
{
  DalTlmm_GpioConfigIdType Settings;

  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId    ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId      ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId     ||
      0 == uim_hw_if.gpio[uim_instance].m_PresentGpioId )
  {
    return;
  }

  uim_memscpy(&Settings,
              sizeof(DalTlmm_GpioConfigIdType),
              &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId),
              sizeof(DalTlmm_GpioConfigIdType));

  Settings.ePull           = DAL_GPIO_PULL_UP;

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_IO_MODE,
                           &Settings);

  uim_memscpy(&Settings,
              sizeof(DalTlmm_GpioConfigIdType),
              &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId),
              sizeof(DalTlmm_GpioConfigIdType));

  Settings.ePull           = DAL_GPIO_PULL_UP;

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_IO_MODE,
                           &Settings);

  uim_memscpy(&Settings,
              sizeof(DalTlmm_GpioConfigIdType),
              &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId),
              sizeof(DalTlmm_GpioConfigIdType));

  Settings.ePull           = DAL_GPIO_PULL_UP;

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_IO_MODE,
                           &Settings);

  if(TRUE == uim_hw_if.intctrl[uim_instance].m_hs_enable)
  {

    uim_memscpy(&Settings,
                sizeof(DalTlmm_GpioConfigIdType),
                &(uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId),
                sizeof(DalTlmm_GpioConfigIdType));

    Settings.ePull           = DAL_GPIO_PULL_UP;

    DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                             uim_hw_if.gpio[uim_instance].m_PresentGpioId,
                             UIM_TLMM_IO_MODE,
                             &Settings);
  }
}


void UIM_RESTORE_ALL_UIM_GPIOS(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId    ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId      ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId     ||
      0 == uim_hw_if.gpio[uim_instance].m_PresentGpioId )
  {
    return;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId));
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId));
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId));

  if(TRUE == uim_hw_if.intctrl[uim_instance].m_hs_enable)
  {
    DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                             uim_hw_if.gpio[uim_instance].m_PresentGpioId,
                             UIM_TLMM_IO_MODE,
                             &(uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId));
  }
}

/*===========================================================================
FUNCTION UIM_SET_RESET_AS_OUTPUT_SLOT

DESCRIPTION
  UIM_SET_RESET_AS_OUTPUT configures the GPIO connected to the RESET line.
  UIM1 RESET: GPIO1, FUNC_SEL = 0
  UIM2 RESET: GPIO5, FUNC_SEL = 0
===========================================================================*/
void UIM_SET_RESET_AS_OUTPUT_SLOT(uim_instance_global_type *uim_ptr)
{
  setResetAsOutput(uim_ptr->id);
  return;
} /* UIM_SET_RESET_AS_OUTPUT_SLOT */


/*===========================================================================
FUNCTION ENABLE_UART_INTERFACE_SLOT

DESCRIPTION
  ENABLE_UART_INTERFACE configures the GPIOs connected to the DATA and
  CLOCK lines to utilize the UART (func_sel=1).

  UIM1 DATA: GPIO3, FUNC_SEL = 1
  UIM1 CLOCK: GPIO2, FUNC_SEL = 1
  UIM2 DATA: GPIO7, FUNC_SEL = 1
  UIM2 CLOCK: GPIO6, FUNC_SEL = 1
===========================================================================*/
void ENABLE_UART_INTERFACE_SLOT(uim_instance_global_type *uim_ptr)
{
  enableUartInterface(uim_ptr->id);
  return;
} /* ENABLE_UART_INTERFACE_SLOT */


/*===========================================================================
FUNCTION UIM_CONFIGURE_RESET_DEFAULT_GPIO

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_RESET_DEFAULT_GPIO(uim_instance_global_type *uim_ptr)
{
  uimConfigureResetDefaultGpio(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_RESET_DEFAULT_GPIO */


/*===========================================================================
FUNCTION UIM_CONFIGURE_DATA_DEFAULT_GPIO

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_DATA_DEFAULT_GPIO(uim_instance_global_type *uim_ptr)
{
  uimConfigureDataDefaultGpio(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_DATA_DEFAULT_GPIO */


/*===========================================================================
FUNCTION UIM_CONFIGURE_CLK_DEFAULT_GPIO

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_CLK_DEFAULT_GPIO(uim_instance_global_type *uim_ptr)
{
  uimConfigureClkDefaultGpio(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_CLK_DEFAULT_GPIO */


/*===========================================================================
FUNCTION UIM_CONFIGURE_DATA_FOR_UIM_CONTROLLER

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_DATA_FOR_UIM_CONTROLLER(uim_instance_global_type *uim_ptr)
{
  uimConfigureDataUimController(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_DATA_FOR_UIM_CONTROLLER */


/*===========================================================================
FUNCTION UIM_CONFIGURE_CLK_FOR_UIM_CONTROLLER

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_CLK_FOR_UIM_CONTROLLER(uim_instance_global_type *uim_ptr)
{
  uimConfigureClkUimController(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_CLK_FOR_UIM_CONTROLLER */


/*===========================================================================
FUNCTION UIM_CONFIGURE_BATT_ALARM_FOR_UICC_SHUTDOWN

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_BATT_ALARM_FOR_UICC_SHUTDOWN(uim_instance_global_type *uim_ptr)
{
  uimConfigureBattAlarmUimController(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_BATT_ALARM_FOR_UICC_SHUTDOWN */


/*===========================================================================
FUNCTION UIM_ASSERT_RESET_SLOT

DESCRIPTION
  Brings the RESET line LOW.
===========================================================================*/
void UIM_ASSERT_RESET_SLOT(uim_instance_global_type *uim_ptr)
{
  assertReset(uim_ptr->id);
  return;
} /* UIM_ASSERT_RESET_SLOT */


/*===========================================================================
FUNCTION UIM_DEASSERT_RESET_SLOT

DESCRIPTION
  Brings the RESET line LOW.
===========================================================================*/
void UIM_DEASSERT_RESET_SLOT(uim_instance_global_type *uim_ptr)
{
  deassertReset(uim_ptr->id);
  return;
} /* UIM_DEASSERT_RESET_SLOT */


/*===========================================================================

FUNCTION UIM_CONFIG_HOTSWAP_GPIOS

DESCRIPTION
  This function configures the GPIO that is connected to the card detection
  switch.

===========================================================================*/
void uim_config_hotswap_gpios(uim_instance_global_type *uim_ptr)
{
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
    return;
  }
  configHotswapGpios(uim_ptr->id);
  return;
} /* uim_config_hotswap_gpios */


/*===========================================================================

FUNCTION UIM_POPULATE_HW_GPIO_INFO

DESCRIPTION
  Retrieve uim gpio values and configurations

===========================================================================*/
boolean uim_populate_hw_gpio_info
(
  uim_instance_global_type *uim_ptr,
  uim_gpio_info_struct     *uim_gpio_info_ptr
)
{
  DALGpioValueType             eValue;
  DalTlmm_GpioIdSettingsType   eSetting;
  uim_gpios_name_type          GpiosName;
  uim_instance_enum_type       uim_instance = uim_ptr->id;

  /* Mapping instance to string */
  updateGpioNameWithInstance(&GpiosName, ((uim_instance_enum_type)m_HWConfig.uim_slot_config[uim_instance].uimUart + 1));

  //get GPIO values
  DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_DataGpioId, &eValue);
  uim_gpio_info_ptr->gpio_input.data = eValue & 0x1;
  DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ClkGpioId, &eValue);
  uim_gpio_info_ptr->gpio_input.clk  = eValue & 0x1;
  DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ResetGpioId, &eValue);
  uim_gpio_info_ptr->gpio_input.reset = eValue & 0x1;
  DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_PresentGpioId, &eValue);
  uim_gpio_info_ptr->gpio_input.detect = eValue & 0x1;

  //get GPIO configs and protocal setting
  DalTlmm_GetGpioIdSettings(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_DataGpioId, &eSetting);
  uim_gpio_info_ptr->data_config.eDirection     =  eSetting.Settings.eDirection;
  uim_gpio_info_ptr->data_config.ePull          =  eSetting.Settings.ePull;
  uim_gpio_info_ptr->data_config.eDriveStrength =  eSetting.Settings.eDriveStrength;

  DalTlmm_GetGpioIdSettings(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ClkGpioId, &eSetting);
  uim_gpio_info_ptr->clk_config.eDirection      =  eSetting.Settings.eDirection;
  uim_gpio_info_ptr->clk_config.ePull           =  eSetting.Settings.ePull;
  uim_gpio_info_ptr->clk_config.eDriveStrength  =  eSetting.Settings.eDriveStrength;

  DalTlmm_GetGpioIdSettings(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ResetGpioId, &eSetting);
  uim_gpio_info_ptr->reset_config.eDirection    =  eSetting.Settings.eDirection;
  uim_gpio_info_ptr->reset_config.ePull         =  eSetting.Settings.ePull;
  uim_gpio_info_ptr->reset_config.eDriveStrength=  eSetting.Settings.eDriveStrength;

  if(uim_ptr->flag.runtime_disable_recovery)
  {
    UIM_CONFIGURE_ALL_UIM_AS_INPUT_PULLUP_GPIOS(uim_ptr->id);
    uim_clk_busy_wait (100);

    //get resetted GPIO values
    DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_DataGpioId, &eValue);
    uim_gpio_info_ptr->gpio_input_pull_up.data = eValue & 0x1;
    DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ClkGpioId, &eValue);
    uim_gpio_info_ptr->gpio_input_pull_up.clk  = eValue & 0x1;
    DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ResetGpioId, &eValue);
    uim_gpio_info_ptr->gpio_input_pull_up.reset = eValue & 0x1;
    DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_PresentGpioId, &eValue);
    uim_gpio_info_ptr->gpio_input_pull_up.detect = eValue & 0x1;

    UIM_RESTORE_ALL_UIM_GPIOS(uim_ptr->id);
  }
  return TRUE;
} /* uim_populate_hw_gpio_info */
