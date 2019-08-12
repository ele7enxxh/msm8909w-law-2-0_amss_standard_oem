#ifndef RFC_CARD_H
#define RFC_CARD_H
/*!
  @file
  rfc_card.h

  @brief
  This file contains functions to support the RF Card (RFC) module which
  manages the RF HW configuration.

*/

/*===========================================================================

Copyright (c) 2008 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/rfc/inc/rfc_card.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
02/24/15   am      Change IRAT alt path lookup implementation
10/29/14   vb      Wrapper API to get a device object for the device type and 
                   instance provided
08/13/14   tks     Modified get alt path table function param list 
08/12/14   aak     Move PAM calls for LTE light sleep to the vreg manager
08/12/14   sb      Set XO drive strength based on the WTRs present in an RFC
07/14/14   vrb     Support for Alternate Path Selection Table
07/09/14   aak     Changes for API to control MPP for DAC Vref
04/23/14   tks     Support for dynamic path selection
03/17/14   kg      Port from Dime
02/11/14   sr      Re-design asdiv_tuner to support multiple RF configurations
10/31/13   shb     Switch to C++ TRX common interface
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
06/29/13   hm     Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
05/17/13   kai     Add new api to get cmn device properties
03/11/13   sar     Updated file for APQ, GNSS only target.
02/20/13 sr/aca  Support for WTR query using DAC
01/31/13   pv      Support Dime interface requirements + CA for Device Manager.
09/14/12   jf      Qtuner LTE support (initial)
10/01/12   sb      Removed GRFC engine references in Dime code
08/15/12   rsr     Adding Default smps value flag to apt_supported structure and renamic structures from apt to xPT for Dime
08/09/12   ggs     Added RFFE to rfc_apt_config_enum_type
07/09/12   aak     Move Quiet mode API from device to RFC
04/12/12   sr      Added new vreg_mgr files to replace the rfc*pm.c files.
03/01/12   jf	   Added LPM status support
01/11/12   kg      Added support for QFE1320
11/29/11   sty     Deleted unused RFC_GET_RX_RECOMMENDATION
10/13/11   tnt     Add configurable falling/rising sampling edge support
10/12/11   whc     Remove RFC_SMPS_PA_CHAR_DATA intf from RFC
09/27/11   aro     Added interface to load int device calibration data
09/19/11   aro     Added common RFC interface to perform target specific
                   RC Tuner calibration
09/14/11   aro     Added prototype of rfc_common_get_logical_device_params()
08/25/11   aak     Prototype for rfc_common_enable_autocal_vregs()
08/10/11   pl      Remove all references to legacy RFC structure and related API
08/05/11   sty     Deleted unused enum members
08/01/11   dw      Remove rfc_dc_supported_type
07/07/11   aak     Change to rfc_get_awake_resource_request_type
06/30/11   cd      Added RFC common interface to get devices configuration
                   from card specific instance and return to RFM layer
05/23/11   aak     Prototype for rfc_get_awake_resource_request()
05/17/11   aak     Including rfm_mode_types.h
04/22/11   tnt     Expand rfc_card_type to add rf_spi_id field
04/22/11   aak     Changing rfc_path_state to rfm_path_state
04/21/11   aak     Adding enums and structures for RFC power
04/01/11   aak     Structures and enums to interface with power module
03/24/11   vrb     Compiler Warning Fixes
03/23/11    dw     Fix warnings
03/15/11   sty     Deleted rfc_get_mode_data
03/03/11   ap      Adding Set Dynamic Voltage Scaling Update API
03/03/11    sr     merge from mdm9k branch
03/01/11    dw     Change band type in rfc_apt_supported_type
02/25/11   tnt     Merge some to compile rfc_mdm9x00_8853_a_lte.cpp
02/23/11   tnt     Merge some to make lte/rf/nv compile
02/23/11   Saul    Connected SMPS NV to RFC structure
02/18/11   Saul    Forced enum smps_char_table_type to be 32-bits for NV maintainability.
02/03/11   pl      Make sure C headers are wrapped with extern "C"
01/07/11   sty     Mainlined RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE
01/31/11   can     Support for LTE GST Freq and Temp Comp.
01/12/11   lcl     Moved GSM TX buffer enum and moved dispatch cmd enum to
                   rfcs_cmd_type to be consistent with the rest of the tech
01/05/11   pl      Change Comments on rfc_mode_config_data_enum_type
01/04/11   sty     Added C++ protection
12/20/10   sty     Replaced rf_path_enum_type with rfm_path_enum_type
12/20/10   sty     Deleted rfc_update_card_from_nv
12/20/10   ka      Initial GST support
12/13/10   ka      Common RFC interface for APT/GST
10/29/10   ka      Updates for initial WCDMA APT functionality
10/15/20   ap      rfc_card_type rearch to support VoltageScaling & LP Tx LUTs
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1
10/04/10   av      GPS M5 bringup
09/17/10   tnt     Add new entry, rfc_common_gpio_ptr, in rfc_card_type structure
09/14/10   av      Compiler Warning fix
09/13/10   shb     Deleted unused enums rfc_vco_source_type & rfc_vco_cmd_type
09/13/10   shb     Cleaned unnecessary elements from rfc_card_type structure
09/01/10   av      GSM RFC Rearch
08/25/10   ap      Added Voltage Scaling Macros
08/24/10   shb     Added command to configure GPIO
08/20/10   ap      Support for RF Enhanced Voltage Scaling
07/29/10   aro     Common GRFC Design Implementation for 1xHDR
07/24/10   aro     Mainlined RF_DEVICE_MERGE_INTERIM_FEATURE
07/20/10   ka      Added API to determine if dual carrier is supported
07/08/10   tnt     RFC re-architecture to support GRFC
06/24/10   aro     Device Merge : Linker Error Fix
05/03/10   aro     Removed RFC_SET_CHAIN_FOR_PRIMARY_ANT : undo Rev 10
05/03/10   Vish    Added RFC_GET_RX_RECOMMENDATION.
04/26/10   kma     Removed PM callback registration
04/23/10   aro     Added back RFC_SET_CHAIN_FOR_PRIMARY_ANT for PLF26
04/22/10   tws     Add XO/TCXO system clock flag.
04/12/10   am      fix for using correct LUT tables for non current bands
04/02/10   adk     Merged branch scmm_mdm9k_merge_final
03/18/10   kma     Added RFC layer rfc_pm_config_vregs() function
03/17/10   lcl     Merged SCMM changes for tx_band
03/16/10   kma     Added RFC layer pm callback function
03/05/10   Vish    Merged from src: 01/18/10 - 02/24/10
02/24/10   sty     Mainlined RF_HAS_RFCONFIG_RX_LNA
01/25/10   sr      Deleted RF_HAS_SUPPORT_FOR_FULL_TIME_SHDR_3
01/18/10   sr      Added new rfc_config_tx_band_data() api
02/01/10   sty     Fixed compile errors
02/01/10   aro     Reverting Back to Rev 4
01/29/10   aro     Code Documentation Update
01/29/10   qma     MDM9K branch merge
01/19/10   aro     Added new RFC Dispatch Command
11/30/09   jfc     Add prototype for rfc_get_tx_band_config()
11/0/09    sar     Removed compiler warnings.
11/25/09   sr      Deleted redundant API - rfc_manage_vreg_for_1x()
10/27/09   sr      Added prototype for rfc_manage_vreg_for_1x()
10/26/09   sr      Fixed warnings by adding prototypes
10/07/09   sar     Converted legacy TLMM interface to DAL and removed
                   adc_logical_channel_type.
10/01/09   ckl     Moved rfc_manage_vregs( ) from rfc.h
09/18/09   sar     Updated tlmm and gpio interface for CMI conformance.
09/18/09   sar     Updated ADC interface for CMI conformance.
07/20/09   sr      Fixed warnings by adding prototypes
07/08/09   av      Cleanup and added port configuration (RTR) data  in RFC
06/26/09   sr      Fixed warnings by adding prototypes
03/18/09   sr      changed the ssbi_info type to use common type
03/13/09   sar     Changes needed for porting to SCMM
03/13/09   sr      tlmm include file change and sbi_info_type change
03/09/09   clk     Code cleanup
01/30/09   av      Support for default card file.
01/30/09   av      Support for MDM8200_Global Target
01/30/09   av      Added data structure and functions for Common
                   MC and core support.
11/21/08   dyc     Updated design.
11/10/08   dyc     Initial version.

============================================================================*/

// C headers
#ifdef __cplusplus
extern "C" {
#endif
#include "rfcom.h"
#include "rfc_pm.h"
#include "DDITlmm.h"
#include "rfcommon_core_types.h"
#include "rfc_bsp.h"
#include "rfm_device_config_type.h"
#include "rf_hal_grfc.h"
#ifdef __cplusplus
}
#endif

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "txlm_intf.h"
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

//C++ - safe headers
#include "rfm_path_types.h"
#include "rfm_mode_types.h"
#include "rfm_resources.h"
#include "rfc_common.h"
#include "rf_hal_grfc.h"
#ifdef __cplusplus
   extern "C"{
#endif

#include "rfdevice_rxtx_common_class.h"

/*----------------------------------------------------------------------------*/
#define RF_MAX_ANTS 4

/*----------------------------------------------------------------------------*/
/*!
  @brief
  End of data flag for rfc_gpio_config_type

  @details
  Used to signify the end of the GPIO configuration list or structure.
*/
#define GPIO_CONFIG_END  ((DALGpioSignalType)DAL_PLACEHOLDER_DALGpioSignalType)

/* RTR S2 Voltage Scaling */
#define RF1_NOMINAL_VOLTAGE 1300
#define RF1_SCALED_VOLTAGE 1238

/*RF RTR POR warmup time is 650us and TX warmup time is 200us*/
#define RTR_HW_POR_WARMUP_TIME 650
#define RTR_HW_TX_WARMUP_TIME  200

#define RF_PA_SMPS_MAX_FREQ_LIST_SIZE 16
#define RF_PA_SMPS_MAX_SMPS_BIAS_LIST_SIZE 128

#define RFC_GST_NUM_PA_STATES 4
#define RFC_GST_NUM_FREQS     16
/*----------------------------------------------------------------------------*/
typedef enum
{
  RFC_CONFIG_GPIO_FOR_SSBI_CORE,   /*!< Configure GPIOs to select SSBI core on device */
  RFC_GET_RTR_TX_PHYSICAL_BUS,
  RFC_GET_RTR_RX_PHYSICAL_BUS,
  RFC_GET_QFE_PHYSICAL_BUS,
  RFC_IS_DUAL_CARRIER_SUPPORTED,
  RFC_GET_RF1_VOLTAGE,
  RFC_SET_RF1_VOLTAGE,
  RFC_GNSS_DISABLE_XO_RESO_SHUTDOWN,
  RFC_IS_LP_TX_LUT_SUPPORTED,
  RFC_GET_APT_CONFIG,
  RFC_SET_GST_TABLE_FROM_NV,
  RFC_GSM_DEBUG_UPDATE_RX_TIMING,
  RFC_GSM_DEBUG_UPDATE_TX_TIMING,
  RFC_GSM_UPDATE_FROM_NV,
  RFC_GSM_TX_BUFFER_CTL,
  RFC_GSM_TX_INDICATOR,
  RFC_SET_SMPS_TABLE_FROM_NV,
  RFC_MAX_CMD = 0xFFFF
} rfcs_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This data enum determines what type of band configuration data to
  return.

  @detail
  Specific types of configuration data can be obtained from the
  rfc_get_rx_band_config() and rfc_get_tx_band_config() APIs.  This enum
  is used to select the requested data pointer to return.

*/
typedef enum
{
  RFC_GPIO_DATA,        /*!< band-dependent GPIO settings data structure pointer */
  RFC_GRFC_DATA,        /*!< band-dependent GRFC settings data structure pointer */
  RFC_CORE_DATA,        /*!< band-dependent Core parameters data structure pointer */
  RFC_RF_CTRL_DATA,     /*!< band-dependent GRFC settings data structure pointer */
  RFC_LTE_LPM_ENABLE_STATUS, /*!< Enum to flag LPM for LTE*/
  #ifdef FEATURE_RF_HAS_QTUNER
  RFC_QTUNER_PRESENT,   /*!< Enum to get QTUNER availability info*/
  #endif
  RFC_UNKNOWN_DATA  /*!< Enum to flag unspecfied data */
} rfc_band_config_data_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This data enum determines what type of band configuration data to
  return.

  @detail
  Specific types of configuration data can be obtained from the
  rfc_get_rx_band_config() and rfc_get_tx_band_config() APIs.  This enum
  is used to select the requested data pointer to return.

*/
typedef enum
{
  RFC_GPIO_GRFC_CONFIG_DATA,        /*!< Obsolete??? */
  RFC_MODE_GPIO_DATA,               /*!< Mode-dependent RX/TX-independent GPIO settings data structure pointer */
  RFC_MODE_GRFC_DATA,               /*!< Mode-dependent RX/TX-independent GRFC settings data structure pointer */
  RFC_MODE_RX_RF_CTRL_DATA,         /*!< Mode-dependent band-independent RX RF Control GRFC settings data structure pointer */
  RFC_MODE_RX_GRFC_ENGINE_DATA,     /*!< Mode-dependent band-independent RX GRFC settings structure pointer */
  RFC_MODE_TX_RF_CTRL_DATA,         /*!< Mode-dependent band-independent TX RF Control GRFC settings data structure pointer */
  RFC_MODE_TX_GRFC_ENGINE_DATA,     /*!< Mode-dependent band-independent TX GRFC settings structure pointer */
  RFC_MODE_CORE_DATA,               /*!< Mode-dependent band-independent Core parameters data structure pointer */
  RFC_MODE_UNKNOWN_DATA  /*!< Enum to flag unspecfied data */

} rfc_mode_config_data_enum_type;

typedef enum
{
  RFC_APT_NOT_SUPPORTED,
  RFC_APT_TX_PDM,
  RFC_APT_GP_PDM,
  RFC_APT_GST,
  RFC_APT_RFFE
} rfc_apt_config_enum_type;

typedef struct
{
  rf_card_band_type band;
  rfc_apt_config_enum_type apt_config;
  uint16 default_smps_val;
} rfc_xpt_supported_type;

/*! Discontinue use of this struc
    Instead, convert to using struct: rfc_smps_nv_type
*/
typedef struct
{
  rfcom_mode_enum_type mode;
  rf_card_band_type band;
  uint16 *smps_values;
} rfc_gst_nv_type; //This struct will be deleted, see above

/*!
  @brief
  This enum determines the type of APT data
  that is being populated from NV into RFC structure.
*/
typedef enum
{
  SMPS_NV_TYPE_PA_SMPS_PDM_LEVEL,
  SMPS_NV_TYPE_CHAR_TBL_TYPE_LIST,
  SMPS_NV_TYPE_CHAR_TBL_PA_RANGE_LIST,
  SMPS_NV_TYPE_CHAR_TBL_NUM_PWR_LIST,
  SMPS_NV_TYPE_CHAR_TBL_SMPS0,
  SMPS_NV_TYPE_CHAR_TBL_SMPS1,
  SMPS_NV_TYPE_CHAR_TBL_SMPS2,
  SMPS_NV_TYPE_CHAR_TBL_SMPS3,
  SMPS_NV_TYPE_CHAR_TBL_POUT0,
  SMPS_NV_TYPE_CHAR_TBL_POUT1,
  SMPS_NV_TYPE_CHAR_TBL_POUT2,
  SMPS_NV_TYPE_CHAR_TBL_POUT3
} smps_nv_item_type;

/*!
  @brief
  This structure contains the APT data
  that is being populated from NV in RFC structure.
*/
typedef struct
{
  rfcom_mode_enum_type mode;
  rf_card_band_type band;
  smps_nv_item_type item_type;
  void *item_vals;
} rfc_smps_nv_type;
/* PA SMPS Characterization Table */
typedef enum
{
  SMPS_CHAR_TABLE_GST = 0,
  SMPS_CHAR_TABLE_APT = 1,
  SMPS_CHAR_TABLE_NOT_SUPPORTED = 2,

  /* items must be added before this entry */
  SMPS_CHAR_TBL_MAX = 0x7FFFFFFF
} smps_char_table_type;

typedef struct
{
  smps_char_table_type char_table;
  uint8 pa_range;
  uint8 pwr_list_size;
  uint16 freq_list[RF_PA_SMPS_MAX_FREQ_LIST_SIZE];
  int16 pwr_list[RF_PA_SMPS_MAX_SMPS_BIAS_LIST_SIZE];
  uint16 smps_vals[RF_PA_SMPS_MAX_FREQ_LIST_SIZE][RF_PA_SMPS_MAX_SMPS_BIAS_LIST_SIZE];
} rfc_smps_pa_characterization_table_type;

/* RFC Power Management module */
typedef enum
{
  RF_CHIP_NOMINAL_PROCESS,
  RF_CHIP_SLOW_SLOW_PROCESS,
  RF_CHIP_PROCESS_MAX
} rfc_rf_chip_process;

typedef enum
{
  RF_CHIP_0,
  RF_CHIP_1,
  RF_CHIP_2,
  RF_CHIP_MAX
} rfc_rf_chip_number;

typedef enum
{
  RFC_VOLTAGE_SCALE_ON,
  RFC_VOLTAGE_SCALE_OFF,
  RFC_VOLTAGE_SCALE_INVALID,
  RFC_VOLTAGE_SCALE_MAX = RFC_VOLTAGE_SCALE_INVALID
} rfc_voltage_scaling_type;

typedef enum
{
  RF_XO_CLK_1,
  RF_XO_CLK_2,
  RF_XO_MAX_CLKS,    /* All the XO clocks need to be set to requested drive strength*/
  RF_XO_CLK_INVALID,
}rfc_xo_clk_type;



typedef struct
{
  boolean active;        /* TRUE= XO clock needed*/
  uint8 trx_count;       /* Number of WTRs mapped to a given XO clock*/
  uint8 drive_strength;  /* XO drive strength required for given XO clock*/
  boolean xo_set;
}rfc_xo_settings_type;


/* This structure needs to be populated inside of rfc_manage_vregs() by each technology and a pointer
   passed to power mgmt */
typedef struct
{
  rfcom_mode_enum_type  mode;        //1x, EVDO, LTE, WCDMA, GSM, GPS
  uint16                band;        //current band
  rfc_rf_chip_number    rf_chip_num; //the RTR on the RF card to be used (0,1,...)
  rf_path_enum_type     path;        //path on the RTR (0, 1 or 2)
  rfm_path_state        state;       // RX, RXTX, or Sleep
} rfc_power_management_info;

typedef struct
{
  rfm_mode_enum_type          mode;
  rfm_path_state              state;
  uint16                      band;
} rfc_mode_info;

typedef struct
{
  rfc_mode_info          path_info[RF_PATH_MAX];
  rfc_rf_chip_process    rf_chip_process;
} rfc_rf_chip_info;

/* end - RFC Power Management module */
/*----------------------------------------------------------------------------*/
boolean rfc_set_card(rf_hw_type rf_hw);

/*----------------------------------------------------------------------------*/
typedef void* (*rfc_measurement_get_band_config_type)
(
   rf_path_enum_type,
   rfcom_mode_enum_type,
   rf_card_band_type,
   rfc_band_config_data_enum_type
);

/*----------------------------------------------------------------------------*/
typedef void* (*rfc_cmd_dispatch_type)(rfm_path_enum_type, int, void *);

/*----------------------------------------------------------------------------*/
typedef void (*rfc_cmd_type)(void *, rfcs_cmd_type);

/*----------------------------------------------------------------------------*/
extern void *rfc_cmd( void* param1, ... );

/*----------------------------------------------------------------------------*/
extern void  rfc_empty_func( void* param1, ... );

/*----------------------------------------------------------------------------*/
extern int16 rfc_empty_func_return_0( void* param1, ... );

/*----------------------------------------------------------------------------*/
int32 rfc_command_dispatch( rfm_path_enum_type path, int32 cmd, void *data );

/*----------------------------------------------------------------------------*/
boolean rfc_get_cmn_device_properties(rfc_cmn_properties_type **ptr);

/*----------------------------------------------------------------------------*/
rfc_alt_path_sel_type* rfc_get_alt_path_tbl
(
  uint32 *tbl_size
);

/*----------------------------------------------------------------------------*/
rfc_alt_path_sel_type* rfc_get_irat_alt_path_tbl
(
  uint32 *tbl_size,
  uint32 *num_band_groups
); 

/*----------------------------------------------------------------------------*/
boolean rfc_update_alt_port_mapping
(
  rfm_device_enum_type rfm_dev, 
  rfcom_mode_enum_type curr_mode, 
  rf_card_band_type rf_band, 
  uint8 alt_path
); 

/*----------------------------------------------------------------------------*/
/* Antenna tunability */
typedef struct rf_ant_attrib
{
  int ant_id;
  boolean tunnable[RF_MAX_BAND];
}rfc_ant_attrib_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Mode independent core configuration data types.

  @details

*/
typedef struct
{
  int slave_id;
  int bus_num;
}ssbi_info; // to do - remove this - satya

/*----------------------------------------------------------------------------*/
typedef struct
{
  const rfcommon_sbi_info_type* rf_device_ssbi[3];         /*!< sample config variable1 */
} rfc_core_config_type;

/*! @todo dchow:11/21/08: Sample implemented only.. needs porting */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Mode independent feature configuration data types.

  @details

*/
typedef struct
{
  int feature1;       /*!< sample feature variable1 (struct) */
  int feature2;       /*!< sample feature variable2 (struct) */
} rfc_feature_config_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This data structure defines the GPIO signal and setting value.

  @details
  Dependencies is tlmm inteface to manage the GPIO signals.

*/
typedef struct
{
  DALGpioSignalType signal;   /*!< GPIO signal name as defined by TLMM */
  DALGpioValueType  value;    /*!< GPIO value to set */

} rfc_set_gpio_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This data structure defines the settings associated with a particular GFRC
   signal.

   @details
   Dependencies is tlmm inteface to manage the GPIO signals.

*/
typedef struct
{
  DALGpioSignalType signal; /*!< GPIO/GRFC signal name as defined by TLMM */
  DALGpioValueType value;   /*!< GPIO/GRFC value to set */
  uint32 mask;            /*!< GRFC mask information */
  uint32 pol_mask;        /*!< GRFC mask for polarity */
  uint32 tristate_mask;   /*!< GRFC mask for tri-state condition */
  uint32 timed_mask;      /*!< GRFC mask for timing values */
  uint32 time_ctl_addr;   /*!< GRFC timing control address */
  uint32 time_value;      /*!< GRFC timing value */

} rfc_set_grfc_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This data structure defines the GPIO and GRFC attrributes and settings.

  @details
  This structure is mode independent.

*/
typedef struct
{
  /*! ptr to the gpio/grfc definitions */
  DALGpioSignalType *rfc_config_gpio_grfc_ptr;

  /*! ptr to the gpio settings */
  rfc_set_gpio_type *rfc_set_gpio_ptr;

  /*! ptr to the grfc settings */
  rfc_set_grfc_type *rfc_set_grfc_ptr;

} rfc_gpio_config_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This generic data structure defines the RF configuration attributes on
   a per mode and band basis.

   @details
   The core, feature and gpio configuration pointers will be updated
   with a mode change.  There will be a rfc_mode_attribute structure assigned
   on a path basis.

   The rx and tx band configuration pointers will be updated with a band
   change.

   Pointers not used in a mode or band should be assigned to NULL.
*/
typedef struct
{
   /*! RF mode ID to identify this structure type */
   rfcom_mode_enum_type mode_id;

   /*! ptr to mode specific core settings */
   rfc_core_config_type *rfc_core_config_ptr;

   /*! ptr to mode specific feature settings */
   rfc_feature_config_type *rfc_feature_config_ptr;

   /*! ptr to mode GPIO settings */
   rfc_gpio_config_type *rfc_gpio_config_ptr;

   /*! ptr to Rx band settings  */
   void *rfc_band_rx_config_ptr;

   /*! ptr to Tx band settings */
   void *rfc_band_tx_config_ptr;

   /*! ptr to the power management configuration function ptrs */
   rfc_pwr_mgnt_type *rfc_pwr_mgnt_ptr;


} rfc_park_attribute_type;

/*----------------------------------------------------------------------------*/
extern void rfc_get_awake_resource_request( rfm_resource_info* params );
/*----------------------------------------------------------------------------*/
extern void rfc_set_quiet_mode(rfm_mode_enum_type mode, boolean on_off);
/*----------------------------------------------------------------------------*/
extern void rfc_common_enable_autocal_vregs(boolean on_off);
/*----------------------------------------------------------------------------*/
extern void rfc_common_enable_mpp_dac_vref(boolean on_off);
/*----------------------------------------------------------------------------*/
extern void rfc_lte_light_sleep_manage_vregs(void);
/*----------------------------------------------------------------------------*/
extern void rfc_lte_light_sleep_wakeup_manage_vregs(void);
/*----------------------------------------------------------------------------*/

extern rfdevice_rxtx_common_class*
rfc_common_get_cmn_device_param
(
  rfdevice_type_enum_type dev_type, uint8 instance
);
/*----------------------------------------------------------------------------*/
extern const rfc_common_logical_device_params_type*
rfc_common_get_logical_device_params
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rfdevice_rxtx_common_class*
rfc_common_get_cmn_device_object
(
  uint8 instance
);

/*----------------------------------------------------------------------------*/
rfdevice_class*
rfc_common_get_cmn_rf_device_object
(
  rfdevice_type_enum_type dev_type, 
  uint8 instance
);

/*----------------------------------------------------------------------------*/
const boolean
rfc_common_do_tuner_device_calibration
(
  void
);

/*----------------------------------------------------------------------------*/
const boolean
rfc_common_do_internal_device_calibration
(
  void
);

/*----------------------------------------------------------------------------*/
const boolean rfc_common_get_rfc_data
(
  void
);

/*----------------------------------------------------------------------------*/
const boolean
rfc_load_internal_device_calibration
(
  void
);
/*----------------------------------------------------------------------------*/

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
rfdevice_rxtx_common_class*
rfc_common_get_transceiver_for_chain

(
  txlm_dac_type txdac
);

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
/*----------------------------------------------------------------------------*/
/*!
   @brief
   Enum to define rf signal level

   @details
   LOW = GND, HIGH = VDD
*/
typedef enum
{
    RF_SIGNAL_LEVEL_LOW     = 0,
    RF_SIGNAL_LEVEL_HIGH    = 1
}rf_sig_out_val_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Enum to define pa range signal level

   @details
*/
typedef enum
{
    RF_PA_RANGE_00      = 0x00,
    RF_PA_RANGE_01      = 0x01,
    RF_PA_RANGE_10      = 0x02,
    RF_PA_RANGE_11      = 0x03
}rf_pa_range_val_type;


/*----------------------------------------------------------------------------*/
/*!
   @brief
   Common data structure for rf control signal

   @details
   This data structure will be used in tech-specific RF Card files to define
   what rf control signal connect to what grfc out
*/
typedef struct
{
    rf_control_signal_type  rf_signal;
    rf_grfc_out_type        grfc_out;
}rfc_control_signal_info_type;


typedef struct
{
  uint16 grfc_rf_signal;
  uint8 ref_start_signal;
  int16 offset_from_ref_start;
  int16 start_delta;
  uint8 ref_stop_signal;
  int16 offset_from_ref_stop;
  int16 stop_delta;

}rfc_grfc_rfctl_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This data structure defines the GPIO/GRFC/RF_CONTROL_SIG attrributes and settings.
  It is an expansion of rfc_gpio_config_type

  @details
  This structure is mode independent.

*/
typedef struct
{
  /*! ptr to the gpio/grfc definitions */
  DALGpioSignalType *rfc_config_gpio_grfc_ptr;

  /*! ptr to the rx rf control signal settings */
  rfc_control_signal_info_type *rfc_rf_control_sig_rx_ptr;

  /*! ptr to the tx rf control signal settings */
  rfc_control_signal_info_type *rfc_rf_control_sig_tx_ptr;

} rfc_gpio_grfc_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure to define top level parameter for SPI ant tuner.
  Each card will have one of this structure defined if ant tuner
  is enabled for the card

  @details

*/
typedef struct
{
  uint8 protocol;             /* SPI = 1, RFFE = 2, 0 ow */
  uint8 clock_always_on;      /* 1 = clk to slave is always on */
  uint8 num_bits_per_trans;
  uint8 deassert_wait_time;   /* clk cycles to deassert CS between transaction*/
  uint8 cs_active_hi;         /* 1= cs active hi, 0 = cs active low */
  uint8 core_index;           /* GSBI core id */
  uint8 cs_1;                 /* first CS  */
  uint8 cs_2;                 /* second CS  */
  uint8 init_enable;          /* 1=send initialization codeword, 0 = o.w. */
  uint8 shutdown_enable;      /* 1=send shutdown codeword, 0 = o.w. */
  uint16 clock_freq_khz;
  uint16 sampling_falling_edge;  /* 1 = MOSI data is sampled on falling edge, 0 = rising edge*/
  uint16 number_of_bins;      /*number of bins in a current band*/
  uint32 init_tunecode;       /* initialization codeword*/
  uint32 shutdown_tunecode;   /* shutdown codeword*/
} rfc_spi_ant_tuner_protocol_type;

extern rfc_spi_ant_tuner_protocol_type rfc_spi_ant_tuner_para;

typedef struct
{
  uint8 antenna_num ;
    
}rfc_antenna_parameter_type;

/*----------------------------------------------------------------------------*/
boolean rfc_get_devices_configuration
(
  rfm_devices_configuration_type *dev_cfg
);

/*----------------------------------------------------------------------------*/
boolean rfc_set_xo_drive_strength( void);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

#endif /* RFC_CARD_H */

