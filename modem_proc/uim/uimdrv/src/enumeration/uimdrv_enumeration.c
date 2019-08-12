/*============================================================================
  FILE:         uimdrv_enumeration.c

  OVERVIEW:     File deals with the configuration and reading of the efs file
                along with enabling defaults according to the chipset got by
                querying the Dal api to get the chip id.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/enumeration/uimdrv_enumeration.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

 when       who        what, where, why
------      ----       -----------------------------------------------------------
05/30/16    gm         F3 message removal
01/28/16    sam        Disalbe slot 2 if FEATURE_UIM_WEARABLES is defined
12/18/15    ks         Disabling UIM2 on 9607 target
12/10/15    ks         8917 bringup changes
09/16/15    ks         8937 bringup changes
09/08/15    ll         UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
06/01/15    sam        9x07 bringup changes
05/13/15    hyo        HW Config validation update 
02/19/15    hyo        Check for all zero HW Config and apply default values
01/19/15    kr         Fixed Compilation warnings
11/11/14    ks         Using the DAL chip enums instad of hardcoding values
11/06/14    nmb        8952 bring up changes
09/23/14    ks         8909 bring up changes
08/25/14    akv        UIM disablement for all chipsets when RUMI_EMULATION flag defined
08/21/14    ak         Replace 9x45 DAL chip family reference for compilation on BOLT
08/20/14    nmb        9x45 Bring up changes
08/14/14    ks         Hotswap enhancement for flaky mechanical switches
07/04/14    ssr        Fix for APDU debug Mask
06/27/14    ks         Fixed to update the m_hwconfig global appropriately
06/23/14    ssr        Fixed to enable the SWP protocol by default for Slot1
06/16/14    ll         Switch to new string based TLMM APIs
05/12/14    ak         8994 Bring up changes
05/06/14    ks         8936 bring up changes
04/11/14    na         Added support for SWP NV item to encode SWP in Terminal Capability
04/01/14    lxu        Use new uim remote power down API to deal with card power
                       down command,set nv 70210 default version to 2
03/27/14    lxu        USB UICC uimdrv implement
02/18/14    sam        Usage of newer style diag macros
02/04/14    sam        Fix for Klocwork issues
01/31/14    ak         Removed uimBooleanEnum type
12/27/13    ks         8916 bring up changes
11/27/13    akv        Initialization of hw enumeration based on dal chip family
11/27/13    akv        Changes for voting against/for sleep on 9x35
10/31/13    ll         Enable both UIM1 and UIM2 by default for 8974pro and 8926
10/16/13    ak         Fix to ensure that disableUim == TRUE for disabled interfaces
10/09/13    ks         Fix in uim_get_hw_max_slots_available func to return number
                       of slots appropriately
09/30/13    akv        Removal of if-else cases and unused code in hw enumeration
09/11/13    na         8x62 bring up changes
08/29/13    na         allow UIM2 config on UIM1 when UIM1 is disabled and UIM2 enabled
08/22/13    js         Add 8926 chip family check
08/22/13    js         Add 8926 and 8974pro chip family checks
08/06/13    rm/ak      Bolt RUMI code
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
06/24/13    js         TSTS changes
05/30/13    rm         Disable UIM2 by default on 8974
05/08/13    akv        Disabling UICC shutdown upon card removal by default on 8x26
04/10/13    nmb        8x10 bring up changes
03/18/13    js         8x26 UIM3 interface bring up fixes.
02/25/13    ak         Validate UIM1 GPIO values for 9x25
02/22/13    js         8x26 bring up changes
02/21/13    ak         9x25 GPIO defaults applied for both versions
02/21/13    ak         UIM1 support on 9x25 v2
02/12/13    js         General F3 macro cleanup
02/11/13    akv        UIM API for querying the available/enabled UIM slots
02/04/13    akv        Fix for using chip family instead of chip id for enumeration
02/01/13    akv        Run time config of UICC shutdown feature and code cleanup
12/05/12    js         UIM Parallel Processing changes
11/09/12    akv        BATT ALARM configuration for Dime
11/02/12    akv        Changes in HW Enumeration for 8x26 RUMI
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_enumeration.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "DALDeviceId.h"
#include "fs_public.h"

#include "uimdrv_gpio.h"
#include "uimdrv_clk.h"
#include "uimdrv_pmic.h"
#include "uimdrv_intctrl.h"
#include "uimdrv_uartdm.h"
#include "uimglobals.h"
#include "uim_common_efs.h"

#include "rex.h"

#define GPIO_NUM_DEFAULT          UIM_INVALID_GPIO_NUM
#define GPIO_FUNC_SEL_DEFAULT     GPIO_FUNC_SEL_1
#define GPIO_DRV_STRENGTH_DEFAULT DS_2MA
#define GPIO_PULL_SETTING_DEFAULT NO_PULL
#define GPIO_PULL_SETTING_PRESENT PULL_UP

/* Local forward declarations */
void configure_uim_enablement(DalChipInfoFamilyType  chipFamily,
                              DalChipInfoVersionType nChipVersion,
                              DalPlatformInfoPlatformType nPlatformType);
uint8 uim_get_hw_max_slots_available(DalChipInfoFamilyType  chipFamily,
                                     DalPlatformInfoPlatformType nPlatformType);
uint8 uim_get_chip_index_from_table (DalChipInfoFamilyType  chipFamily,
                                     uimTableEnum   reference_table);
void uim_set_uicc_usb_feature(void);
void uim_set_hotswap_flaky_switch_support(void);
void uim_hw_config_input_validation(uimHWConfig* m_HWConfig_ptr);

/* uimdrv_hw_nv_config_info structure holds all hw interface info got
   by reading NV UIM_HW_CONFIG_NV_EF */
uimdrv_hw_nv_config_info_type         uimdrv_hw_nv_config_info  = {0};
uim_hw_slot_info_type                 uim_hw_slot_info;

typedef struct
{
  DalChipInfoFamilyType dalChipFamily;
  uint8                 numUimSlotsAvailable;
  boolean               bDisableUIM1;
  boolean               bDisableUIM2;
  boolean               bDisableUIM3;
  boolean               bDisableUIM4;
}uimdrv_chipset_info_type;

/* Mapping between DalChipInfoFamilyType and uimChipsetEnum */
/* Also contains UIM1 - UIM4 disable/enable info per chipset */
static uimdrv_chipset_info_type chipset_info_table[] =
{
  /*DalChipInfoFamilyType           UIM Slots Available               (disable)UIM1 - UIM4 */
  {DALCHIPINFO_FAMILY_MSM8974,      NUM_UIM_SLOTS_AVAILABLE_ON_8974,  FALSE, TRUE,  TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8974_PRO,  NUM_UIM_SLOTS_AVAILABLE_ON_8974,  FALSE, FALSE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MDM9x25,      NUM_UIM_SLOTS_AVAILABLE_ON_9x25,  FALSE, TRUE,  TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8x10,      NUM_UIM_SLOTS_AVAILABLE_ON_8x10,  FALSE, FALSE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8x26,      NUM_UIM_SLOTS_AVAILABLE_ON_8x26,  FALSE, FALSE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8926,      NUM_UIM_SLOTS_AVAILABLE_ON_8926,  FALSE, FALSE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MDM9x35,      NUM_UIM_SLOTS_AVAILABLE_ON_9x35,  FALSE, TRUE,  TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8x62,      NUM_UIM_SLOTS_AVAILABLE_ON_8x62,  FALSE, FALSE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8916,      NUM_UIM_SLOTS_AVAILABLE_ON_8916,  FALSE, FALSE,  TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8936,      NUM_UIM_SLOTS_AVAILABLE_ON_8936,  FALSE, FALSE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8994,      NUM_UIM_SLOTS_AVAILABLE_ON_8994,  FALSE, FALSE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MDM9x45,      NUM_UIM_SLOTS_AVAILABLE_ON_9x45,  FALSE, TRUE, TRUE, TRUE},
#ifdef FEATURE_UIM_WEARABLE
  {DALCHIPINFO_FAMILY_MSM8909,      NUM_UIM_SLOTS_AVAILABLE_ON_8909,  FALSE, TRUE, TRUE, TRUE},
#else
  {DALCHIPINFO_FAMILY_MSM8909,      NUM_UIM_SLOTS_AVAILABLE_ON_8909,  FALSE, FALSE, TRUE, TRUE},
#endif /*FEATURE_UIM_WEARABLE*/
  {/* DALCHIPINFO_FAMILY_MSM8952 */ 60, NUM_UIM_SLOTS_AVAILABLE_ON_8952, FALSE, TRUE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MDM9x07,      NUM_UIM_SLOTS_AVAILABLE_ON_9x07,  FALSE, TRUE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8937,      NUM_UIM_SLOTS_AVAILABLE_ON_8937,  FALSE, FALSE, TRUE, TRUE},
  {DALCHIPINFO_FAMILY_MSM8917,      NUM_UIM_SLOTS_AVAILABLE_ON_8917,  FALSE, FALSE, TRUE, TRUE},
  /* Last entry marks end of array */
  {DALCHIPINFO_FAMILY_UNKNOWN,       0x00,                             TRUE, TRUE, TRUE, TRUE}
};

static void uim_apply_defaults(void)
{
  DalDeviceHandle               *phChipInfo    = NULL;
  DalChipInfoFamilyType         chipFamily     = DALCHIPINFO_FAMILY_UNKNOWN;
  DalChipInfoVersionType        nChipVersion   = 0;
  DalPlatformInfoPlatformType   nPlatformType  = DALPLATFORMINFO_TYPE_UNKNOWN;
  uint8                         nChipindex     = 0;
  uint8                         nUimInstance   = 0;

  UIM_MSG_HIGH_0("HW configuration applying defaults ");

  uimdrv_hw_nv_config_info.default_nv_applied  = TRUE;

  if (DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo) != DAL_SUCCESS)
  {
    return;
  }

  DalChipInfo_GetChipFamily(phChipInfo, &chipFamily);
  nChipVersion = DalChipInfo_ChipVersion();
  nPlatformType = DalPlatformInfo_Platform();

  UIM_MSG_HIGH_2("Chip Family 0x%x, Chip Version 0x%x", chipFamily, nChipVersion);

  /*
     All UIM instances are initially disabled.
     They are selectively enabled based on the chipset.
  */
  m_HWConfig.uim_slot_config[0].disableUim = TRUE;
  m_HWConfig.uim_slot_config[1].disableUim = TRUE;
  m_HWConfig.uim_slot_config[2].disableUim = TRUE;
  m_HWConfig.uim_slot_config[3].disableUim = TRUE;

  #ifdef T_RUMI_EMULATION
  /*  Workaround for RUMI  - Return after disabling all UIM instances */
  UIM_MSG_HIGH_0("RUMI_EMULATION - Returning after disabling all UIM instances in uim_apply_defaults");
  return;
  #endif /* T_RUMI_EMULATION */

  /* Disable/Enable UIM1-UIM4 based on the chipset */
  configure_uim_enablement(chipFamily, nChipVersion, nPlatformType);

  m_HWConfig.version = VER_4;

  m_HWConfig.uimBattAlarmGpioNum            = GPIO_NUM_DEFAULT;
  m_HWConfig.uimBattAlarmGpioFuncSel        = GPIO_FUNC_SEL_DEFAULT;
  m_HWConfig.uimBattAlarmGpioDrvStrength    = GPIO_DRV_STRENGTH_DEFAULT;
  m_HWConfig.uimBattAlarmGpioPullSetting    = GPIO_PULL_SETTING_DEFAULT;

  m_HWConfig.uicc_shutdown_feature.uiccShutdownBattRemoval  = FALSE;

  for (nUimInstance = 0; nUimInstance < UIM_INSTANCES_CONFIGURABLE; nUimInstance++)
  {
    m_HWConfig.uim_slot_config[nUimInstance].enableUimHotswap    = FALSE;
    m_HWConfig.uim_slot_config[nUimInstance].uimHotswapPolarity  = ACTIVE_HIGH;

    nChipindex = uim_get_chip_index_from_table(chipFamily, UIM_UARTDM_NUM_TABLE);
    m_HWConfig.uim_slot_config[nUimInstance].uimUart  = uim_uartdm_num_table[nChipindex].uartdm_num[nUimInstance];

    nChipindex = uim_get_chip_index_from_table(chipFamily, UIM_PMIC_RESOURCE_TABLE);
    m_HWConfig.uim_slot_config[nUimInstance].uimVcc   = uim_pmic_npa_resource_table[nChipindex].pmic_npa_resource[nUimInstance];

    nChipindex = uim_get_chip_index_from_table(chipFamily, UIM_CONTROLLER_IRQ_TABLE);
    m_HWConfig.uim_slot_config[nUimInstance].uimControllerIRQNum     = uim_controller_irq_num_table[nChipindex].irq_num[nUimInstance];

    nChipindex = uim_get_chip_index_from_table(chipFamily, UIM_UARTDM_IRQ_TABLE);
    m_HWConfig.uim_slot_config[nUimInstance].uimUartIRQNum           = uim_uartdm_irq_num_table[nChipindex].irq_num[nUimInstance];

    m_HWConfig.uim_slot_config[nUimInstance].uimResetGpioNum         = GPIO_NUM_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimResetGpioFuncSel     = GPIO_FUNC_SEL_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimResetGpioDrvStrength = GPIO_DRV_STRENGTH_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimResetGpioPullSetting = GPIO_PULL_SETTING_DEFAULT;

    m_HWConfig.uim_slot_config[nUimInstance].uimDataGpioNum          = GPIO_NUM_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimDataGpioFuncSel      = GPIO_FUNC_SEL_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimDataGpioDrvStrength  = GPIO_DRV_STRENGTH_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimDataGpioPullSetting  = GPIO_PULL_SETTING_DEFAULT;

    m_HWConfig.uim_slot_config[nUimInstance].uimClkGpioNum           = GPIO_NUM_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimClkGpioFuncSel       = GPIO_FUNC_SEL_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimClkGpioDrvStrength   = GPIO_DRV_STRENGTH_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimClkGpioPullSetting   = GPIO_PULL_SETTING_DEFAULT;

    m_HWConfig.uim_slot_config[nUimInstance].uimCardDetectGpioNum           = GPIO_NUM_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimCardDetectGpioFuncSel       = GPIO_FUNC_SEL_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimCardDetectGpioDrvStrength   = GPIO_DRV_STRENGTH_DEFAULT;
    m_HWConfig.uim_slot_config[nUimInstance].uimCardDetectGpioPullSetting   = GPIO_PULL_SETTING_PRESENT;

   /*
     UICC shutdown on card removal should be enabled by default,
     but disabling them on 8x26 due to call drop issues, seen because
     PMIC incorrectly shuts down the UIM LDO.
    */
    if (DALCHIPINFO_FAMILY_MSM8x26 == chipFamily)
    {
       m_HWConfig.uicc_shutdown_feature.uiccShutdownCardRemoval[nUimInstance]  = FALSE;
    }
    else
    {
       m_HWConfig.uicc_shutdown_feature.uiccShutdownCardRemoval[nUimInstance]  = TRUE;
    }
    /* making SWP support in Terminal Capability as NOT SUPPORTED */
    m_HWConfig.uimSwpCapabilitySupported[nUimInstance] = FALSE;
  }

  /* Most of target have the SWP support on slot1 */
  m_HWConfig.uimSwpCapabilitySupported[0] = TRUE;
  
  if (UIM_COMMON_EFS_SUCCESS != uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_HW_CONFIG,
                                                     UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                     UIM_COMMON_EFS_DEVICE,
                                                    (char *)&(m_HWConfig),
                                                     sizeof(m_HWConfig)))
  {
    UIM_MSG_ERR_0("Writing default values to EFS failed");
  }
}/* uim_apply_defaults */


void uim_set_uicc_usb_feature(void)
{
  uint8   nUimInstance   = 0;
  for (nUimInstance = 0; nUimInstance < UIM_INSTANCES_CONFIGURABLE; nUimInstance++)
  {
    if(m_HWConfig.uim_slot_config[nUimInstance].disableUim == TRUE)
    {
      continue;
    }
    if( m_HWConfig.usb_uicc_supported[nUimInstance] == TRUE )
    {
      uim_usb_supported[nUimInstance] = TRUE;
    }
    else
    {
      uim_usb_supported[nUimInstance] = FALSE;
    }
  }
}/* uim_set_uicc_usb_feature */

/* This function sets the flaky hotswap feature globals after reading the NV70210 */
void uim_set_hotswap_flaky_switch_support(void)
{
  uint8	nUimInstance   = 0;
  uim_instance_global_type *uim_ptr	   = NULL;

  for (nUimInstance = 0; nUimInstance < UIM_INSTANCES_CONFIGURABLE; nUimInstance++)
  {
    uim_ptr = uim_get_instance_ptr((uim_instance_enum_type)(nUimInstance));

    if(m_HWConfig.uim_slot_config[nUimInstance].disableUim == TRUE)
    {
      continue;
    }
    if(uim_ptr != NULL)
    {
      if( m_HWConfig.uim_hotswap_flaky_switch[nUimInstance] == TRUE )
      {
        uim_ptr->hotswap.hotswap_flaky_switch_support = TRUE;
      }
      else
      {
        uim_ptr->hotswap.hotswap_flaky_switch_support = FALSE;
      }
    }
  }
}/* uim_set_hotswap_flaky_switch_support */

/*
  1. uimdrv_hw_nv_configuration() is invoked to read nv
     UIM_HW_CONFIG_NV_EF that contains information used to configure
     UIMDRV HW interfaces.
  2. It is instance independant as all instances run from information
     got from nv UIM_HW_CONFIG_NV_EF
  3. The NV is read only once by either of the following depending on who
     calls uimdrv_hw_nv_configuration() first,
     a. Either the first uim thread to reach dev init and in turn invoke
        uimdrv_hw_nv_configuration().
     b. Else by any external client who calls the UIM exposed api to get
        the UIM slot information. The exposed api is uim_get_hw_slot_info()
        which in turn invokes uimdrv_hw_nv_configuration().
*/
uim_hw_slot_info_type uimdrv_hw_nv_configuration(void)
{
  uint32                                i              = 0;
  uint32                                j              = 0;
  /* This is used to hold the list of enabled UIM i.e
     uim_enabled_slot_config[0 .. (supportedSlot-1)].disableUim = FALSE
   */
  uimSlotConfig                         uim_enabled_slot_config[UIM_INSTANCES_CONFIGURABLE];
  DalDeviceHandle                       *phChipInfo    = NULL;
  DalChipInfoFamilyType                 chipFamily     = DALCHIPINFO_FAMILY_UNKNOWN;
  DalPlatformInfoPlatformType           nPlatformType  = DALPLATFORMINFO_TYPE_UNKNOWN;

  /* Enter critical section */
  rex_enter_crit_sect(&uimdrv_hw_nv_config_info.uimdrv_hw_nv_crit_sect);

  /* EFS not read, read EFS and return the number_of_active_interfaces */
  if (FALSE == uimdrv_hw_nv_config_info.efs_file_read_completed)
  {
    uimdrv_hw_nv_config_info.number_of_active_interfaces = 0;
    memset(&uim_hw_slot_info,0,sizeof(uim_hw_slot_info_type));

    if (DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo) != DAL_SUCCESS)
    {
      UIM_MSG_ERR_0("DAL_DeviceAttach failed in uimdrv_hw_nv_configuration");
      /* Leave critical section and return */
      rex_leave_crit_sect(&uimdrv_hw_nv_config_info.uimdrv_hw_nv_crit_sect);
      return uim_hw_slot_info;
    }

    DalChipInfo_GetChipFamily(phChipInfo, &chipFamily);

    nPlatformType = DalPlatformInfo_Platform();

    uimdrv_hw_nv_config_info.dalChipFamily = chipFamily;

    uimdrv_hw_nv_config_info.hw_max_num_slots_supported =
                                uim_get_hw_max_slots_available(chipFamily,nPlatformType);

    memset(&m_HWConfig,0, sizeof(m_HWConfig));
    memset(&uim_enabled_slot_config, 0, sizeof(uim_enabled_slot_config));

    if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read( UIM_COMMON_EFS_UIMDRV_HW_CONFIG,
                                                       UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                       UIM_COMMON_EFS_DEVICE,
                                                      (uint8 *)&(m_HWConfig),
                                                       sizeof(m_HWConfig)))
    {
      uim_hw_config_input_validation(&m_HWConfig);
    }
    else
    {
      uim_apply_defaults();
    }

    uimdrv_hw_nv_config_info.efs_file_read_completed = TRUE;

    for (i=0; (i< uimdrv_hw_nv_config_info.hw_max_num_slots_supported) && (i<UIM_INSTANCES_CONFIGURABLE); i++)
    {
      uim_hw_slot_info.slot_status[i].slot_available = TRUE;
      if ( m_HWConfig.uim_slot_config[i].disableUim == FALSE )
      {
        uimdrv_hw_nv_config_info.number_of_active_interfaces++;
        uim_hw_slot_info.slot_status[i].slot_enabled = TRUE;
      }
    }

    if ( uimdrv_hw_nv_config_info.number_of_active_interfaces == 0)
    {
      UIM_MSG_ERR_0("Number of active interfaces is Zero, UIM cannot initialize HW");
      /* Leave critical section and return */
      rex_leave_crit_sect(&uimdrv_hw_nv_config_info.uimdrv_hw_nv_crit_sect);
      return uim_hw_slot_info;
    }

    /* scanning through m_HWConfig and picking up the uim_slot_config object corresponding to
       enabled slot, copying it into newly introduced uim_enabled_slot_config object.
       This (uim_enabled_slot_config) newly introduced object will hold the slot configuration
       data of enabled UIM only (UIM_DISABLE = FALSE)
    */
    for (i=0; (i< uimdrv_hw_nv_config_info.hw_max_num_slots_supported) && (i<UIM_INSTANCES_CONFIGURABLE); i++)
    {
      if (uim_hw_slot_info.slot_status[i].slot_enabled == TRUE)
      {
        uim_memscpy(&uim_enabled_slot_config[j],
                    sizeof(uimSlotConfig),
                    &m_HWConfig.uim_slot_config[i],
                    sizeof(m_HWConfig.uim_slot_config[i]));
        m_HWConfig.usb_uicc_supported[j]        = m_HWConfig.usb_uicc_supported[i];
        m_HWConfig.uimSwpCapabilitySupported[j] = m_HWConfig.uimSwpCapabilitySupported[i];
        m_HWConfig.uicc_shutdown_feature.uiccShutdownCardRemoval[j] = m_HWConfig.uicc_shutdown_feature.uiccShutdownCardRemoval[i];
        m_HWConfig.uim_hotswap_flaky_switch[j] = m_HWConfig.uim_hotswap_flaky_switch[i];
        j++;	
      }
    }
    /* while doing memset with ZERO,which indirectly made
       uim_enabled_slot_config[j].disableUim = ZERO (FALSE), causing this UIM to be enabled.
       therefore It is purposely made to DISABLE remaining UIM
       before overwriting m_HWConfig with uim_enabled_slot_config
     */
    for (;j < UIM_INSTANCES_CONFIGURABLE;j++)
    {
      uim_enabled_slot_config[j].disableUim = TRUE;
    }

    uim_memscpy(&m_HWConfig.uim_slot_config,
                sizeof(m_HWConfig.uim_slot_config),
                &uim_enabled_slot_config,
                sizeof(uim_enabled_slot_config));

    uim_set_uicc_usb_feature();
    uim_set_hotswap_flaky_switch_support();
    /* Leave critical section and return */
    rex_leave_crit_sect(&uimdrv_hw_nv_config_info.uimdrv_hw_nv_crit_sect);
    return uim_hw_slot_info;
  }
  else /* EFS already read, just return the uim_hw_slot_info */
  {
    /* Leave critical section and return */
    rex_leave_crit_sect(&uimdrv_hw_nv_config_info.uimdrv_hw_nv_crit_sect);
    return uim_hw_slot_info;
  }
} /* uimdrv_hw_nv_configuration */


/*
   uim_get_hw_slot_info() is the API is exposed for other uim modules
   to get information on the available/enabled UIM interfaces.
*/
uim_hw_slot_info_type uim_get_hw_slot_info(void)
{
  return uimdrv_hw_nv_configuration();
}/* uim_get_hw_slot_info */


/* Disables/Enables UIM1-UIM4 based on chipset defaults  */
void configure_uim_enablement(DalChipInfoFamilyType  chipFamily,
                              DalChipInfoVersionType nChipVersion,
                              DalPlatformInfoPlatformType nPlatformType)
{
  uint32 i = 0;

  /* Find the corresponding entry in the chipset_info_table */
  /* i will be the index of the correct entry after this loop */
  for (i=0; chipset_info_table[i].dalChipFamily != DALCHIPINFO_FAMILY_UNKNOWN; i++)
  {
    if (chipset_info_table[i].dalChipFamily == chipFamily)
    {
      break;
    }
  }

  if (DALCHIPINFO_FAMILY_UNKNOWN == chipset_info_table[i].dalChipFamily)
  {
    UIM_MSG_ERR_0("INVALID chipFamily");
    return;
  }

  /* Enable/Disable the UIM interfaces based on the chipset_info_table entries */
  m_HWConfig.uim_slot_config[0].disableUim = chipset_info_table[i].bDisableUIM1;
  m_HWConfig.uim_slot_config[1].disableUim = chipset_info_table[i].bDisableUIM2;
  m_HWConfig.uim_slot_config[2].disableUim = chipset_info_table[i].bDisableUIM3;
  m_HWConfig.uim_slot_config[3].disableUim = chipset_info_table[i].bDisableUIM4;

  /* SPECIAL CASES */
  if (DALCHIPINFO_FAMILY_MDM9x25 == chipFamily)
  {
    if(nChipVersion < DALCHIPINFO_VERSION(2,0))
    {
      /* UIM2 is enabled by default instead of UIM1 */
      m_HWConfig.uim_slot_config[0].disableUim = TRUE;
      m_HWConfig.uim_slot_config[1].disableUim = FALSE;
    }
  }
}/* configure_uim_enablement */


/* uim_get_hw_max_slots_available returns the max available uim slots on that chipset */
uint8 uim_get_hw_max_slots_available(DalChipInfoFamilyType       chipFamily,
                                     DalPlatformInfoPlatformType nPlatformType)
{
  uint8 i;
  /* If the chip id is 8626 or 8926 and the platform type is RUMI then disable all UIM slots */
  if ((DALCHIPINFO_FAMILY_MSM8x26  == chipFamily || DALCHIPINFO_FAMILY_MSM8926 == chipFamily)
      && DALPLATFORMINFO_TYPE_RUMI == nPlatformType)
  {
    return 0;
  }
  for (i=0; chipset_info_table[i].dalChipFamily != DALCHIPINFO_FAMILY_UNKNOWN; i++)
  {
    if (chipset_info_table[i].dalChipFamily == chipFamily)
    {
      return chipset_info_table[i].numUimSlotsAvailable;
    }
  }
  UIM_MSG_ERR_0("uim_get_hw_max_slots_available: Chipset unknown, cannot initialize HW");
  return 0;
}/* uim_get_hw_max_slots_available */


/* uim_get_chip_index_from_table returns the index of the information related to the chip family */
uint8 uim_get_chip_index_from_table (DalChipInfoFamilyType  chipFamily, uimTableEnum   reference_table)
{
  uint8 i = 0;

  switch (reference_table)
  {
    case UIM_PMIC_RESOURCE_TABLE :
    {
      /* Locate the location of the chip related entries in supplied table */
      /* i will be the index of the correct entry after this loop */
      for (i=0; uim_pmic_npa_resource_table[i].chipFamily != DALCHIPINFO_FAMILY_UNKNOWN; i++)
      {
        if (uim_pmic_npa_resource_table[i].chipFamily == chipFamily)
        {
          return i;
        }
      }
      break;
    } /* Case - UIM_PMIC_RESOURCE_TABLE */

    case UIM_UARTDM_NUM_TABLE :
    {
      /* Locate the location of the chip related entries in supplied table */
      /* i will be the index of the correct entry after this loop */
      for (i=0; uim_uartdm_num_table[i].chipFamily != DALCHIPINFO_FAMILY_UNKNOWN; i++)
      {
        if (uim_uartdm_num_table[i].chipFamily == chipFamily)
        {
          return i;
        }
      }
      break;
    } /* Case - UIM_UARTDM_NUM_TABLE */

    case UIM_UARTDM_IRQ_TABLE :
    {
      /* Locate the location of the chip related entries in supplied table */
      /* i will be the index of the correct entry after this loop */
      for (i=0; uim_uartdm_irq_num_table[i].chipFamily != DALCHIPINFO_FAMILY_UNKNOWN; i++)
      {
        if (uim_uartdm_irq_num_table[i].chipFamily == chipFamily)
        {
          return i;
        }
      }
      break;
    } /* Case - UIM_UARTDM_IRQ_TABLE */

    case UIM_CONTROLLER_IRQ_TABLE :
    {
      /* Locate the location of the chip related entries in supplied table */
      /* i will be the index of the correct entry after this loop */
      for (i=0; uim_controller_irq_num_table[i].chipFamily != DALCHIPINFO_FAMILY_UNKNOWN; i++)
      {
        if (uim_controller_irq_num_table[i].chipFamily == chipFamily)
        {
          return i;
        }
      }
      break;
    } /* Case - UIM_CONTROLLER_IRQ_TABLE */

   default:
     break;
  }

  ERR_FATAL("uim_get_chip_index_from_table: Chipset unknown, cannot initialize HW",0,0,0);
  return 0;
}/* uim_get_chip_index_from_table */


/*
   uim_hw_config_input_validation checks if crucial items for enabled UIM
   instance is set to 0.
 */
void uim_hw_config_input_validation(uimHWConfig* m_HWConfig_ptr)
{
  uint32 i = 0;

  if (m_HWConfig_ptr != NULL)
  {
    /* For each enabled UIM instance, check crucial items if they are 0 */
    for (i = 0; i < UIM_INSTANCES_CONFIGURABLE; i++)
    {
        if (FALSE == m_HWConfig_ptr->uim_slot_config[i].disableUim &&
          (m_HWConfig_ptr->uim_slot_config[i].uimControllerIRQNum == 0 ||
           m_HWConfig_ptr->uim_slot_config[i].uimUartIRQNum == 0 ||
           m_HWConfig_ptr->uim_slot_config[i].uimResetGpioNum == 0 ||
           m_HWConfig_ptr->uim_slot_config[i].uimDataGpioNum == 0 ||
           m_HWConfig_ptr->uim_slot_config[i].uimClkGpioNum == 0 ||
           m_HWConfig_ptr->uim_slot_config[i].uimCardDetectGpioNum == 0))
      {
        UIM_MSG_LOW_0("UIM_HW_CONFIG_NV_EF nearly zero, populating defaults");
        uim_apply_defaults();
        break;
      }
    }
  }
} /* uim_hw_config_input_validation */
