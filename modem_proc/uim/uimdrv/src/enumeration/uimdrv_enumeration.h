#ifndef UIMDRV_ENUMERATION_H
#define UIMDRV_ENUMERATION_H

/*============================================================================
  FILE:         uimdrv_enumeration.h

  OVERVIEW:     File contains the inclusions and definitions necessary for
                configuriguring and storing the nv values got from the efs.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/enumeration/uimdrv_enumeration.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

 when       who        what, where, why
------      ----       -----------------------------------------------------------
01/28/16    sam        Disalbe slot 2 if FEATURE_UIM_WEARABLES is defined
12/18/15    ks         Disabling UIM2 on 9607 target
12/10/15    ks         8917 bringup changes
09/16/15    ks         8937 bringup changes
09/16/15    sam        UART 1_4_2 changes
09/08/15    ll         UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
06/01/15    sam        9x07 bringup changes
01/05/15    ks         supoport for UIM3 external LDO on 8909
11/10/14    akv        Support for user to customize LDO settings
11/06/14    nmb        8952 bring up changes
09/23/14    ks         8909 bring up changes
09/18/14    akv        UIMDRV UT enhancements
08/20/14    nmb        9x45 Bring up changes
08/14/14    ks         Hotswap enhancement for flaky mechanical switches
07/25/14    lxu        Read cmd_rsp_time from nv for usb mode, give cmd_rsp_time and
                       voltage_class to ap when send usb power up to ap
06/23/14    ssr        Fixed to enable the SWP protocol by default for Slot1
06/16/14    ll         Switch to new string based TLMM APIs
05/12/14    ak         8994 Bring up changes
05/06/14    ks         8936 bring up changes
04/11/14    na         Added support for SWP NV item to encode SWP in Terminal Capability
03/27/14    lxu        USB UICC modem implement
02/20/14    ks         8926 h/w slots number increased from 2 to 3.
02/18/14    sam        Usage of newer style diag macros
01/31/14    ak         Remove uimBooleanEnum type
12/27/13    ks         8916 bring up changes
11/27/13    akv        Initialization of hw enumeration based on dal chip family
11/27/13    akv        Changes for voting against/for sleep on 9x35
11/06/13    sam        Changed the number of available slots for 8974 from 3 to 2
10/09/13    ks         Fix in uim_get_hw_max_slots_available func to return number
                       of slots appropriately
09/30/13    ks         Changing uimChipsetEnum values to avoid mis-match
09/30/13    akv        Removal of if-else cases and unused code in hw enumeration
09/19/13    akv        Reference CLK changes to support 4.8MHz
09/16/13    yk         Removed the code for old targets and for old features
09/11/13    na         8x62 bring up changes
08/06/13    rm/ak      Bolt RUMI code
08/1/13     sam        APDU logging mask variable moved to uim_instance_global_type
07/25/13    sam        APDU logging mask selection based on run time
                       configuration
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
02/22/13    js         8x26 bring up changes
02/11/13    akv        UIM API for querying the available/enabled UIM slots
02/01/13    akv        Run time configuration of UICC shutdown feature
12/05/12    js         UIM Parallel Processing changes
11/09/12    akv        BATT ALARM configuration for Dime
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "err.h"                   /* Error handling utilities             */
#include "uimdrv.h"                /* UIM driver header file               */
#include "uimi.h"                  /* UIM Internal header file             */
#if defined( FEATURE_UIM_T_1_SUPPORT )
#include "crc.h"
#include "uimt1.h"                 /* T=1 protocol support                 */
#endif /* FEATURE_UIM_T_1_SUPPORT */
#include "HALhwio.h"                    /* Target definitions */

#ifdef FEATURE_THOR_MODEM
#include "uimthorhwiomacros.h"
#else
  #ifdef FEATURE_BOLT_MODEM
  #include "uimbolthwiomacros.h"
  #else /* FEATURE_BOLT_MODEM */
  #include "uimdimehwiomacros.h"
  #endif
#endif


#include "uimutil.h"

#include "DDITlmm.h"
#include "DDIClock.h"

#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "DALDeviceId.h"

#include "DDIInterruptController.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"

  /* Inclusion for MCPM APIs */
#include "mcpm_api.h"

#include "pm.h"
#include "npa.h"

#define UIM_MAX_HWCONFIG_SIZE                     256  // bytes
#define SIZE_OF_UIM_SLOT_CONFIG                   29
#define SIZE_OF_VERSION_AND_BATT_ALARM_FIELDS     6
#define SIZE_OF_UICC_SHUTDOWN_FEATURE             5
#define SIZE_OF_USB_UICC_SUPPORTED                4
/* SWP in TERMINAL CAPABILITY (TC) */
#define SIZE_OF_SWP_SUPPORTED_IN_TC               4

/* Hotswap on flaky hardware support */
#define SIZE_OF_FLAKY_HOTSWAP_SUPPORTED           4

/* UIM_INSTANCES_CONFIGURABLE  is based on the number of
   UIM instances configurable by NV 70210 */
#define UIM_INSTANCES_CONFIGURABLE                4

#define UIM_ALLOCATED_SIZE                        ((UIM_INSTANCES_CONFIGURABLE*SIZE_OF_UIM_SLOT_CONFIG )   \
                                                   + (SIZE_OF_VERSION_AND_BATT_ALARM_FIELDS)               \
                                                   + (SIZE_OF_UICC_SHUTDOWN_FEATURE)                         \
                                                   + (SIZE_OF_USB_UICC_SUPPORTED)                           \
                                                   + (SIZE_OF_SWP_SUPPORTED_IN_TC)     /* bytes */ \
                                                   + (SIZE_OF_FLAKY_HOTSWAP_SUPPORTED))

#define UIM_MAX_FREE_SIZE                         UIM_MAX_HWCONFIG_SIZE -  UIM_ALLOCATED_SIZE

/* Number of UIM Slots on hardware */
#define NUM_UIM_SLOTS_AVAILABLE_ON_8974           2
#define NUM_UIM_SLOTS_AVAILABLE_ON_9x25           2
#define NUM_UIM_SLOTS_AVAILABLE_ON_9x35           2
#define NUM_UIM_SLOTS_AVAILABLE_ON_8x26           3
#define NUM_UIM_SLOTS_AVAILABLE_ON_8x10           3
#define NUM_UIM_SLOTS_AVAILABLE_ON_8926           3
#define NUM_UIM_SLOTS_AVAILABLE_ON_8x62           3
#define NUM_UIM_SLOTS_AVAILABLE_ON_8916           3
#define NUM_UIM_SLOTS_AVAILABLE_ON_8936           3
/* Only UIM 1 and UIM 2 supported initially on 8994 */
#define NUM_UIM_SLOTS_AVAILABLE_ON_8994           2
#define NUM_UIM_SLOTS_AVAILABLE_ON_9x45           2
#ifdef FEATURE_UIM_WEARABLE
#define NUM_UIM_SLOTS_AVAILABLE_ON_8909           1
#else
#define NUM_UIM_SLOTS_AVAILABLE_ON_8909           3
#endif /*FEATURE_UIM_WEARABLE*/
#define NUM_UIM_SLOTS_AVAILABLE_ON_8952           2
#define NUM_UIM_SLOTS_AVAILABLE_ON_9x07           1
#define NUM_UIM_SLOTS_AVAILABLE_ON_8937           2
#define NUM_UIM_SLOTS_AVAILABLE_ON_8917           2

typedef enum
{
  VER_0 = 0,
  VER_1,
  VER_2,
  VER_3,
  VER_4
}uimVersionEnum;

typedef enum
{
  BADGER =0,
  BOLT,
  UIM_CHIPSET_FAMILY_UNKNOWN
}uimChipsetFamilyEnum;

typedef enum
{
  UIM_PMIC_RESOURCE_TABLE = 0,
  UIM_UARTDM_NUM_TABLE,
  UIM_UARTDM_IRQ_TABLE,
  UIM_CONTROLLER_IRQ_TABLE,
}uimTableEnum;

typedef enum
{
  T0=0,
  T1,
  T0_T1
}uimProtocolEnum;

typedef enum
{
  ACTIVE_LOW =0,
  ACTIVE_HIGH
}uimPolarityeEnum;

typedef enum
{
  BADGER_MSS_UIM0_UART_DM = 0,
  BADGER_MSS_UIM1_UART_DM,
  BADGER_MSS_UIM2_UART_DM,
  INVALID_BADGER_MSS_UART_DM
}uimUartEnumList;

enum
{
  UIM1_GPIO = 0,
  UIM2_GPIO,
  UIM3_GPIO,
  UIM4_GPIO,
  COMMON_GPIO,
  UIM_MAX_GPIO_INSTANCE
};

typedef enum
{
  DS_2MA=0,
  DS_4MA,
  DS_6MA,
  DS_8MA,
  DS_10MA,
  DS_12MA,
  DS_14MA,
  DS_16MA
}uimDriveStrengthEnum;

typedef enum
{
  NO_PULL =0,
  PULL_DOWN,
  KEEPER,
  PULL_UP
}uimPullSettingEnum;

typedef enum
{
  BATT_ALARM  = 0,
  RESET       = 0,
  CLK,
  DATA,
  CARD_DETECT,
  MAX_GPIO_INDEX
}uimGpioIndex;

typedef enum
{
  GPIO_FUNC_SEL_0 = 0,
  GPIO_FUNC_SEL_1,
  GPIO_FUNC_SEL_2,
  GPIO_FUNC_SEL_3,
  GPIO_FUNC_SEL_4,
  GPIO_FUNC_SEL_5,
  GPIO_FUNC_SEL_6
}uimGpioFuncSelEnum;

typedef enum
{
  E_PMIC_NPA_RESOURCE_UIM1,
  E_PMIC_NPA_RESOURCE_UIM2,
  E_PMIC_NPA_RESOURCE_UIM3,
  INVALID_PMIC_NPA_RESOURCE
}uimVccEnum;

/* UIM Configuration Index*/
typedef enum
{
  VERSION,
  DISABLE_UIM1,
  DISABLE_UIM2,
  ENABLE_UIM1_HOTSWAP,
  ENABLE_UIM2_HOTSWAP,
  UIM1_HOTSWAP_POLARITY,
  UIM2_HOTSWAP_POLARITY,
  UIM1_CONTROLLER_IRQ_NUM,
  UIM2_CONTROLLER_IRQ_NUM,
  UIM1_UART,
  UIM2_UART,
  UIM1_UART_IRQ_NUM,
  UIM2_UART_IRQ_NUM,
  UIM1_RESET_GPIO_NUM,
  UIM1_RESET_GPIO_FUNC_SELECTION,
  UIM1_RESET_GPIO_DRV_STRENGTH,
  UIM1_RESET_PULL_SETTING,
  UIM1_DATA_GPIO_NUM,
  UIM1_DATA_GPIO_FUNC_SELECTION,
  UIM1_DATA_GPIO_DRV_STRENGTH,
  UIM1_DATA_PULL_SETTING,
  UIM1_CLOCK_GPIO_NUM,
  UIM1_CLOCK_GPIO_FUNC_SELECTION,
  UIM1_CLOCK_GPIO_DRV_STRENGTH,
  UIM1_CLOCK_PULL_SETTING,
  UIM1_CARD_DETECT_GPIO_NUM,
  UIM1_CARD_DETECT_GPIO_FUNC_SELECTION,
  UIM1_CARD_DETECT_GPIO_DRV_STRENGTH,
  UIM1_CARD_DETECT_PULL_SETTING,
  UIM2_RESET_GPIO_NUM,
  UIM2_RESET_GPIO_FUNC_SELECTION,
  UIM2_RESET_GPIO_DRV_STRENGTH,
  UIM2_RESET_PULL_SETTING,
  UIM2_DATA_GPIO_NUM,
  UIM2_DATA_GPIO_FUNC_SELECTION,
  UIM2_DATA_GPIO_DRV_STRENGTH,
  UIM2_DATA_PULL_SETTING,
  UIM2_CLOCK_GPIO_NUM,
  UIM2_CLOCK_GPIO_FUNC_SELECTION,
  UIM2_CLOCK_GPIO_DRV_STRENGTH,
  UIM2_CLOCK_PULL_SETTING,
  UIM2_CARD_DETECT_GPIO_NUM,
  UIM2_CARD_DETECT_GPIO_FUNC_SELECTION,
  UIM2_CARD_DETECT_GPIO_DRV_STRENGTH,
  UIM2_CARD_DETECT_PULL_SETTING,
  UIM1_VCC,
  UIM2_VCC
}uimConfigReqIndex;

typedef PACKED struct PACKED_POST
{
  boolean                 disableUim;
  boolean                 enableUimHotswap;
  uimPolarityeEnum        uimHotswapPolarity;
  uint16                  uimControllerIRQNum;
  uimUartEnumList         uimUart;
  uint16                  uimUartIRQNum;
  uimVccEnum              uimVcc;
  uint16                  uimResetGpioNum;
  uint8                   uimResetGpioFuncSel;
  uimDriveStrengthEnum    uimResetGpioDrvStrength;
  uimPullSettingEnum      uimResetGpioPullSetting;

  uint16                  uimDataGpioNum;
  uint8                   uimDataGpioFuncSel;
  uimDriveStrengthEnum    uimDataGpioDrvStrength;
  uimPullSettingEnum      uimDataGpioPullSetting;

  uint16                  uimClkGpioNum;
  uint8                   uimClkGpioFuncSel;
  uimDriveStrengthEnum    uimClkGpioDrvStrength;
  uimPullSettingEnum      uimClkGpioPullSetting;

  uint16                  uimCardDetectGpioNum;
  uint8                   uimCardDetectGpioFuncSel;
  uimDriveStrengthEnum    uimCardDetectGpioDrvStrength;
  uimPullSettingEnum      uimCardDetectGpioPullSetting;
}uimSlotConfig;

typedef PACKED struct PACKED_POST
{
  boolean                 uiccShutdownBattRemoval;
  boolean                 uiccShutdownCardRemoval[UIM_INSTANCES_CONFIGURABLE];
}uiccShutdownFeature;

  /* UIM Device Enumration, HW \SW modules
  */
typedef PACKED struct PACKED_POST
{
  uimVersionEnum          version;
  uint16                  uimBattAlarmGpioNum;
  uint8                   uimBattAlarmGpioFuncSel;
  uimDriveStrengthEnum    uimBattAlarmGpioDrvStrength;
  uimPullSettingEnum      uimBattAlarmGpioPullSetting;
  uimSlotConfig           uim_slot_config[UIM_INSTANCES_CONFIGURABLE];
  uiccShutdownFeature     uicc_shutdown_feature;
  boolean                 usb_uicc_supported[UIM_INSTANCES_CONFIGURABLE];
  /* SWP (Single Wire Protocol) support for NFC interface */
  boolean                 uimSwpCapabilitySupported[UIM_INSTANCES_CONFIGURABLE];
  /* Hotswap support for hw with flaky hotswap switch */
  boolean                 uim_hotswap_flaky_switch[UIM_INSTANCES_CONFIGURABLE];
  uint8                   rfu[UIM_MAX_FREE_SIZE]; // Max Size - 256
}uimHWConfig;

enum
{
  UIM1,
  UIM2,
  UIM_MAX
};

/* This type identifies the convention of the SIM card interface */
typedef enum
{
  /* Identifies direct convention on the SIM interface */
  DIRECT_CONVENTION,
  /* Identifies inverse convention on the SIM interface */
  INVERSE_CONVENTION
} uartdm_convention_type;

/* Shadow vars for UART DM registers */
typedef struct
{
  dword nMR1;
  dword nMR2;
  dword nCSR;
  dword nCR;
  dword nMISR;
  dword nIMR;
  dword nIPR;
  dword nTFWR;
  dword nRFWR;
  dword nRXINIT;
  dword nIRDA;
  dword nDMEM;
  dword nTXNUM;
  dword nBADR;
  dword nTXFIFO;
  dword nRXFIFO;
  dword nSIMCFG;
  dword nUIMCFG;
  dword nWWT;
}uartdm_shadow_type;

/* LDO Voltages available and supported by PMIC. */
typedef enum
{
  PMIC_UIM_VOLTAGE_INVALID,
  PMIC_UIM_ACTIVE_CLASS_B_LOW,
  PMIC_UIM_ACTIVE_CLASS_B,
  PMIC_UIM_ACTIVE_CLASS_B_HIGH,
  PMIC_UIM_ACTIVE_CLASS_C_LOW,
  PMIC_UIM_ACTIVE_CLASS_C,
  PMIC_UIM_ACTIVE_CLASS_C_HIGH
}uim_pmic_voltages_avail_type;

typedef struct
{
  uint32                  start_addr;
  uint32                  uart_version;
  uartdm_shadow_type      mRegShadow;
  uartdm_convention_type  mUimConvention;
}uartdm_interface;

typedef struct
{
  DalDeviceHandle       *m_pDalClockHandle_ptr;
  dword                  m_simClkSourceFreq;
  dword                  m_simClkFreq;
  dword                  m_simMaxFreqSupported;
  dword                  m_preferred_clock_stop_ctl;
  dword                  m_preferred_clock_ctl;
  uim_dal_clk_info_type  m_UartClkInfo;
  uim_dal_clk_info_type  m_SimClkInfo;
  uim_dal_clk_info_type  m_HclkInfo;
}clkregime_interface;

typedef struct
{
  uimVccEnum                    m_Vcc;
  npa_client_handle             m_pmicNpaHandle;
  npa_client_handle             m_pmicActivityHandle;
  uint32                        m_voltage_class;
  uint32                        m_vreg;
  uint32                        m_classC_voltageRegulator;
  uint32                        m_classB_voltageRegulator;
  boolean                       m_uiccShutBattRemoval;
  boolean                       m_uiccShutCardRemoval;
  uint32                        m_pmic_npa_mode;
}pmic_interface;

typedef struct
{
  DalDeviceHandle       *m_TlmmHandle_ptr;
  /* address to struct */
  DALGpioIdType            m_DataGpioId;
  DALGpioIdType            m_ClkGpioId;
  DALGpioIdType            m_ResetGpioId;
  DALGpioIdType            m_PresentGpioId;
  DALGpioIdType            m_BattAlarmGpioId;
  DALGpioIdType            m_UimExtLdoEnGpioId;
  DALGpioIdType            m_UimExtLdoVselGpioId;
  /* struct */
  DalTlmm_GpioConfigIdType m_DataGpioConfigId;
  DalTlmm_GpioConfigIdType m_ClkGpioConfigId;
  DalTlmm_GpioConfigIdType m_ResetGpioConfigId;
  DalTlmm_GpioConfigIdType m_PresentGpioConfigId;
  DalTlmm_GpioConfigIdType m_BattAlarmGpioConfigId;
  DalTlmm_GpioConfigIdType m_UimExtLdoEnGpioConfigId;
  DalTlmm_GpioConfigIdType m_UimExtLdoVselGpioConfigId;
}gpio_interface;

typedef struct
{
  DalDeviceHandle       *m_IntCtrl_handle_ptr;
  DalDeviceHandle       *m_UimCtrl_handle_ptr;
  uint32                 client_callback_param;
  boolean                m_hs_enable;
  uimPolarityeEnum       m_hs_polarity;
  uint16                 m_uartIRQNum;
  uint16                 m_uimControllerIRQNum;
}intctrl_interface;

typedef struct
{
  uartdm_interface                uart[UIM_MAX_INSTANCES];
  clkregime_interface             clkreg[UIM_MAX_INSTANCES];
  pmic_interface                  pmic[UIM_MAX_INSTANCES];
  gpio_interface                  gpio[UIM_MAX_INSTANCES];
  intctrl_interface               intctrl[UIM_MAX_INSTANCES];
  boolean                         uim_is_external_ldo[UIM_MAX_INSTANCES];
}uim_interface_instances;

typedef struct
{
  DalChipInfoFamilyType     chipFamily;
  uint16      irq_num[UIM_INSTANCES_CONFIGURABLE];
}uim_controller_irq_num_table_type;

typedef struct
{
  DalChipInfoFamilyType     chipFamily;
  uint16      irq_num[UIM_INSTANCES_CONFIGURABLE];
}uim_uartdm_irq_num_table_type;

typedef struct
{
  DalChipInfoFamilyType     chipFamily;
  uint16      pmic_npa_resource[UIM_INSTANCES_CONFIGURABLE];
}uim_pmic_npa_resource_table_type;

typedef struct
{
  DalChipInfoFamilyType     chipFamily;
  uint16      uartdm_num[UIM_INSTANCES_CONFIGURABLE];
}uim_uartdm_num_table_type;


/* ---------------------------------------------------------------
   STRUCTURE:      uimdrv_hw_nv_config_info_type

   DESCRIPTION:
     Structure holds all hw interface information got by reading nv
     UIM_HW_CONFIG_NV_EF
   ----------------------------------------------------------------*/
typedef struct {
  DalChipInfoFamilyType        dalChipFamily;                        /* Chip family for which HW enum being initialized */
  uint32                       hw_max_num_slots_supported;           /* Max number of slots supported ny HW */
  uint32                       number_of_active_interfaces;          /* Total number of active uim interfaces */
  boolean                      efs_file_read_completed;              /* EFS file read done */
  boolean                      default_nv_applied;                   /* UIMDRV HW running on Default NV */
  rex_crit_sect_type           uimdrv_hw_nv_crit_sect;               /* UIMDRV HW NV file read critical section variable */
}uimdrv_hw_nv_config_info_type;

#endif /* UIMDRV_ENUMERATION_H */
