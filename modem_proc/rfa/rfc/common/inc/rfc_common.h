#ifndef RFC_COMMON_H
#define RFC_COMMON_H

/*!
  @file
  rfc_common.h

  @brief
  This modules contains prototypes for the common RFC functions and type definitions to support the
  common RFC module.
*/

/*==============================================================================
Copyright (c) 2011 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/inc/rfc_common.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/10/15   fhuo    FR31436: Add rfc_lte_properties_type and rfc_lte_channel_support_type for LTE partial band
03/03/15   vv      Added rfc support for ADC VCM Cal feature
11/30/14   px      Support for sending rffe scan efs dump to rf apps task
11/17/14   dyc     Replace hard coded value.
10/13/14   dr      Adding Featurization for Tabasco
09/10/14   Saul    rename et_modem_chain to fbrx_modem_chain
08/15/14   st      NS07 Port State Type
08/15/14   kg      Framework to read RFC EEPROM and populate local structure.
07/18/14   Saul    Convert to using GPIO protection APIs.
07/14/14   vrb     Support for Alternate Path Selection Table
05/12/14   vv      Added physical and logical device cfg structures
04/23/14   tks     Support for dynamic path selection
03/25/14   bsh     Ported CL: 5493095
03/17/14   kg      Port from Dime
02/11/14   sr      Re-design asdiv_tuner to support multiple RF configurations
03/05/14   kg      Added antenna to rfc_device_info_type.
01/16/14   ndb     Added Daughter Card support(NON_FATAL_DEVICES_SUPPORT).
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech 
10/31/13   shb     Switch to C++ TRX common interface
09/17/13   sb      Fix split band structure format for compatibility with other RF cards
09/17/13   sb      Support for SGLTE/SGTDS coexistence
09/06/13   as      Added RFC_REQ_ENABLE_CA_CONFIG and RFC_REQ_DISABLE_CA_CONFIG to
                   rfc_req_type enum
07/26/13   vs      Add RF interface to query DBDC bands supported
06/11/13   sd      Added support for Port State type
06/05/13   sd      Added new data type for GSM Spur channels
05/29/13   spa     Added delay param to APIs for getting GRFC scripts
05/21/13   sd      Added new data types for attenuation states
04/01/13   sd      Add support for SCELL log path swap
03/24/13   sd      Added RFC_REQ_ENABLE_ANT_SWAP and RFC_REQ_DISABLE_ANT_SWAP to
                   rfc_req_type enum
03/05/13   sr      Added self_test support.
03/05/13   vrb     RFC AG formatting updates with v2.13.132
01/16/13   vrb     Add in Associated DAC and Requires_INIT flag into device properties
01/02/13   jyu     Changed start/stop_delta type to int16
01/3/13    av      Changes to support chain2 for GSM
12/11/12   hdz     Enabled timing support for rfc_common_get_grfc_overwritten_buffer
12/09/12   zhw     Make GRFC overwritten API take RFC logic instead of boolean
11/26/12   sr      changed rfc related data type to uint32 to support efs rfc data.
11/07/12   sbm     API to extract timing information.
10/24/12   zhw     Support for generating GRFC script with init state
10/17/12   kai     Removed qfe_cmn_device
10/16/12   zhw     RF On/Off grfc scripting using overwritten rfc signal
10/11/12   vrb     Adding timing information data type
10/11/12   dw      SVVZ bringup updates
10/02/12   vss     Add support for getting grfc script buffer based on signal type
09/13/12   vrb     Updated comments for AG type definition changes
08/17/12   shb     Include "rf_buffer_intf.h"
08/14/12   vrb     Renamed slave_id to bus in device config structure
08/13/12   vrb     Updated device configuration data structure for RFFE info
08/10/12   vrb     Change ASIC Info Array to type int32
08/08/12   vrb     Increased ASIC info data size by 1, to account for intf_rev
07/27/12   sr      renamed rfdevice related enum types.
07/26/12   vrb     Added Modem Chain into RFC AG struct defn to indicate ADC/DAC
07/21/12   sr      removed the grfc rf engine references.
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
07/18/12   sr      Changes to create devices through device factory.
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
06/29/12   sr      RFC changes : use rf_hal_buffer to generate scripts 
06/29/12   sr      rfc api name change.
06/28/12   sr      dime rfc changes.
06/21/12    vb     Added support for QFE1510
05/18/12   sb      made APIs to get grfc number from signal type
05/15/12   zw      Added API for appending grfc scripts. 
                   The API will memcopy the entries of source_script into dest_script
                   starting from the first empty slot. GRFC counts in dest_script
                   will also be updated
04/24/12   ac      Added support for rfc binary data
04/05/12   sr      made changes to put all the GPIOs into low power mode during sleep.
03/27/12   sty     Added extern device_process
03/26/12   vss     Add API to retrive GRFC engine output type; add output_type field 
                   to rfc_sig_cfg_type
03/26/12   sty     deleted band_supported_query - obsolete 
03/20/12   sr      Added support for rfc binary data
11/17/11   aak     Critical section lock for RFC  
11/17/11   sr      Add rfc_common_get_grfc_info()
11/01/11   vrb     Added tx_wsaw_flag to asic params from AG
09/19/11   aro     Added callback function for NV write
09/14/11   aro     Added structure to Common logical device parameter for a
                   RF Card
08/26/11   sr      RFC V1.3.35 SS release changes. SS format has changed.
08/20/09   sr      added function to check for the rfc common init status.
07/29/11   Saul    IRAT update. Skip setting GPIOs during IRAT.
07/15/11   whc     IRAT GRFC script support for autogen RFC  
07/08/11   sr      Added new API to get the GRFC number from list of signals
06/22/11   sr      Initial Revision.

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "rfcom.h"
#include "DDITlmm.h"
#include "rfc_msm_signal_info_ag.h"
#include "rfdevice_intf_cmd.h" 
//#include "rfdevice_cmn_intf.h" 
#include "rfdevice_cmn_type_defs.h"
#include "rf_buffer_intf.h"
#include "rfdevice_rxtx_common_class.h"
#include "rf_dispatch.h"

/* @brief Fix number of GRFC scripts to 44 entries */
#define RFC_MAX_NUM_OF_GRFC  15
#define RFC_MAX_SLAVES_PER_DEVICE 2
#define RFC_ASIC_INFO_DATA_SIZE 6
#define RFC_GRFC_NUM_NA 0xFF   /* GRFC number not applicable */
#define RFC_INVALID_PARAM 0xFFFFFFFF   
#define RFC_MAX_DEVICE_NUM 25

#define RFC_MAX_ANTENNA  4
#define RFC_ASDIV_MAX_DEVICES_PER_ANT_PATH 3
#define RFC_ASDIV_CONFIG_SWAP_PAIR 3

/* Bit indicator to specify whether to skip device identification 
  check during RFFE Scan */
#define RFC_SKIP_RFFE_DETECT_BIT_IND 0x80000000 

/* Bit indicator to specify whether the device is available in Daughter card or not 
                            or 
  Bit indicator to specify the NON_FATAL_DEVICES.
*/
#define RFC_NONFATAL_IF_MISSING_BIT_IND  0x40000000

#define RFC_MAJOR_REV_BMSK 0xFF0000
#define RFC_PL_BRANCH_REV_BMSK 0xFF000000

#define RFDEVICE_PRODUCT_REV_MASK 0x0000FFFF

#define RFC_NO_ALTERNATE_PART 0xFFFFFFFF

/* Header for eeprom content to check integrity of data read. ASCII for RFC* */
#define RFC_EEPROM_MAGIC_NUM 0x5246432A

typedef enum
{
  RFC_LOW,
  RFC_HIGH,
  RFC_CONFIG_ONLY,
  RFC_LOGIC_NUM,
  RFC_LOGIC_INVALID = 0xFFFFFFFF
} rfc_logic_type;

typedef enum
{
  RFC_CONFIG_RX,
  RFC_CONFIG_TX,
  RFC_CONFIG_RXTX_MAX,
  RFC_CONFIG_RXTX_INVALID = RFC_CONFIG_RXTX_MAX,
} rfc_rxtx_enum_type;

typedef enum
{
  RFC_PRX_PATH,
  RFC_DRX_PATH, 
  RFC_INVALID_PATH = 0xFF
}rfc_rx_path_type;

typedef enum
{
  RFC_START_SCRIPT,
  RFC_STOP_SCRIPT,
  RFC_SCRIPT_TYPE_MAX,
  RFC_SCRIPT_TYPE_INVALID = RFC_SCRIPT_TYPE_MAX,
} rfc_script_enum_type;

typedef enum
{
  RFC_FILTERED_PATH_FALSE,
  RFC_FILTERED_PATH_TRUE,
  RFC_FILTER_CFG_NOT_APP = 0xFFFFFFFF
}rfc_coex_filter_status_type;

typedef enum
{
  RFC_BW_1P4MHZ,
  RFC_BW_3MHZ, 
  RFC_BW_5MHZ, 
  RFC_BW_10MHZ,
  RFC_BW_15MHZ,
  RFC_BW_20MHZ,
  RFC_BW_ALL,
  RFC_BW_MAX,
}rfc_lte_bw_type;

typedef enum
{
  RFC_PATH_0 = 0,
  RFC_PATH_1 = 1,
  RFC_PATH_MAX = 255,
}rfc_path_override_ind_type;


typedef struct
{
  uint32 start_chan;
  uint32 stop_chan;
  int band;
  rfc_coex_filter_status_type status;
  int sys_bw;
  int path_index;
}rfc_chan_range_type;


typedef struct
{
  /*32 bit element capturing the RFC revision:
    upper 8 bits:  Branch/PL revision
    next 8 bits:   Major revision: This gets updated when there is 
                   a change to GPIO/GRFC mapping information, that 
                   could impact all RF Cards. A major revision 
                   update triggers release for all RF cards. 
    lower 16 bits: Minor revision: Any changes specific to certain 
                   RF cards only, such as signal logic or device
                   configurations. Minor revision update only
                   mandates release of affected RF cards. */
  uint32 rfc_revision; 

  rfc_chan_range_type band_info[];
}rfc_band_split_info_type;


typedef struct
{
  int band1;
  int band2;
} rfc_band_pair_type;


typedef struct
{
  uint32 rfc_revision;
  rfc_band_pair_type band_pair_list[];
} rfc_band_pair_list_type;

typedef struct
{
  rfdevice_type_enum_type device_type;
  rfdevice_id_enum_type device_id;
  uint32 instance;
  uint32 phy_path_num;
  int32 data[RFC_ASIC_INFO_DATA_SIZE];
} rfc_asic_info_type;


/* Structure to hold the eeprom contents */

typedef PACK(struct){
   uint32 rf_rfc_eeprom_magic_num;
   uint16 rf_rfc_eeprom_version_num;
   uint32 rf_rfc_eeprom_res1;
   uint32 rf_rfc_eeprom_res2;
   uint16 rf_rfc_eeprom_metadata[4];
   uint8  rf_rfc_eeprom_hwid;
   uint8  rf_rfc_eeprom_mcn[30];
} rf_rfc_eeprom_data_type;


typedef struct
{
  /*32 bit element capturing the RFC revision:
    upper 8 bits:  Branch/PL revision
    next 8 bits:   Major revision: This gets updated when there is 
                   a change to GPIO/GRFC mapping information, that 
                   could impact all RF Cards. A major revision 
                   update triggers release for all RF cards. 
    lower 16 bits: Minor revision: Any changes specific to certain 
                   RF cards only, such as signal logic or device
                   configurations. Minor revision update only
                   mandates release of affected RF cards. */
  uint32 rfc_revision; 

  /* Modem Chain is specified in ag files per 
     logical path (RFM device) and band.
     For Rx configuration, this represents the ADC/WB chain to be used.
     For Tx configuration, this represents the DAC/TXC/TXR chain to be used.
     This information is required to be band specific as some cards
     split bands across transceivers: All low bands on one TRx, which
     is hardwired to a certain ADC/DAC chain and all high bands on 
     the other TRx, which is hardwired to the other ADC/DAC chain */
  uint32 modem_chain;

  /* This captures which NV container to derive calibrated data from.
     Multiple logical paths (RFM devices) which share the same RF path
     will share the same NV container. */
  uint32 nv_container;

  /* Antenna number */
  uint32 ant_num;
  
  /* Number of physical devices, such as PAs, Antenna Switch modules
     and transceivers */
  uint32 num_devices;

  /* Configuration information for each device, such as port info */
  rfc_asic_info_type rf_asic_info[];
} rfc_device_info_type;


/*Physical Device Structure to store physical device info from RFC*/
typedef struct
{
  rfdevice_id_enum_type rf_device_id; /*PHY_DEVICE_NAME*/
  uint8 phy_dev_instance; /* PHY_DEVICE_INSTANCE*/
  uint32 alternate_part_idx; /* Alternate Part Index Num */
  rfdevice_comm_proto_enum_type rf_device_comm_protocol; /*PHY_DEVICE_COMM_PROTOCOL*/
  uint32 bus[RFC_MAX_SLAVES_PER_DEVICE]; /*PHY_DEVICE_COMM_BUS*/
  uint32 manufacturer_id;
  uint32 product_id;
  uint32 product_rev;
  uint32 default_usid_range_start;
  uint32 default_usid_range_end;
  uint32 assigned_usid;
  uint32 group_id;
  boolean init_required;
  uint32 associated_dac;
} rfc_phy_device_info_type;



/*Logical Device Structure to store logical device info from RFC*/

typedef struct
{
  rfdevice_type_enum_type rf_device_type; /*DEVICE_MODULE_TYPE*/
  rfdevice_id_enum_type rf_device_id; /*LOGICAL_DEVICE_NAME*/
  uint32 rf_asic_id; /*DEVICE_MODULE_TYPE_INSTANCE*/
  uint8 associated_phy_dev_instance; /* ASSOCIATED PHY_DEVICE_INSTANCE*/
} rfc_logical_device_info_type;


typedef struct
{
  rfdevice_type_enum_type rf_device_type;
  rfdevice_id_enum_type rf_device_id;
  /*rf_asic_id*/
  uint32 rf_asic_id; /* Device Type Instance*/
  uint32 alternate_part_idx; /* Alternate Part Index Num */
  /*Communication Protocol for common objects*/
  rfdevice_comm_proto_enum_type rf_device_comm_protocol;
  /*Bus info:
    In case of transceivers, 1st index is TX; 2nd index is RX
    In case of RFFE devices, 1st index is the RFFE bus: bus[0] */
  uint32 bus[RFC_MAX_SLAVES_PER_DEVICE];
  uint32 manufacturer_id;
  uint32 product_id;
  uint32 product_rev;
  uint32 default_usid_range_start;
  uint32 default_usid_range_end;
  uint32 assigned_usid;
  uint32 group_id;
  boolean init_required;
  uint32 associated_dac;
  rfdevice_type_enum_type associated_rf_device_type;
  uint32 associated_rf_asic_id; /* Associated Device Type Instance*/
} rfc_device_cfg_info_type;


typedef struct
{
  rfc_logical_device_info_type* device_info;
  boolean result;
} rfc_device_self_test_info_type;


typedef struct
{
  uint32 num_devices;
  rfc_device_self_test_info_type device[RFC_MAX_DEVICE_NUM];
} rfc_self_test_info_type;

typedef struct
{
  rfc_logic_type logic;
  int32 time_offset;
} rfc_sig_timing_info_type;


typedef struct
{
  rfc_logic_type logic;
  int32 time_offset;
} rfc_sig_tdd_timing_info_type;

typedef struct
{
  int sig_name;
  rfc_sig_tdd_timing_info_type start;
  rfc_sig_tdd_timing_info_type stop;
} rfc_sig_tdd_cfg_type;

typedef struct
{
  uint32 rfc_revision;
  rfc_sig_tdd_cfg_type cfg_sig_list[];
} rfc_sig_tdd_info_type;

typedef struct
{
  int sig_name;
  rfc_sig_timing_info_type start;
  rfc_sig_timing_info_type stop;
} rfc_sig_cfg_type;

typedef struct
{
  uint32 rfc_revision;
  rfc_sig_cfg_type cfg_sig_list[];
} rfc_sig_info_type;

typedef struct
{
  rfc_timing_param_type timing_param;
  int32 timing;
} rfc_timing_info_type;


typedef enum
{
  RFC_REQ_INIT,
  RFC_REQ_ENABLE_ANT_SWAP_1,
  RFC_REQ_DISABLE_ANT_SWAP_1,
  RFC_REQ_ENABLE_CA_CONFIG,
  RFC_REQ_DISABLE_CA_CONFIG,
  RFC_REQ_ENABLE_FBRX,
  RFC_REQ_DISABLE_FBRX,
  RFC_REQ_ENABLE_XPT_CAPTURE,
  RFC_REQ_DISABLE_XPT_CAPTURE,
  RFC_REQ_DEFAULT_GET_DATA
}rfc_req_type;


typedef enum
{
  RFC_PORT_STATE_A,
  RFC_PORT_STATE_B,
  RFC_PORT_STATE_NS07 = RFC_PORT_STATE_B,
  RFC_PORT_STATE_DEFAULT
}rfc_port_state_type;


typedef struct
{  
  rf_path_enum_type path;
  rfm_device_enum_type logical_device;
  rfc_rxtx_enum_type rx_tx;
  int band;
  rfc_req_type req;
  rfc_port_state_type port_state;
  uint32 alternate_path;
} rfc_cfg_params_type;

/* ASDiv RFC configuration request */
typedef struct
{
  int32 asd_cfg;  
} rfc_asd_cfg_params_type;

typedef struct
{

  /* Modem Chain is specified in ag files per 
     logical path (RFM device) and band.
     For Rx configuration, this represents the ADC/WB chain to be used.
     For Tx configuration, this represents the DAC/TXC/TXR chain to be used.
     This information is required to be band specific as some cards
     split bands across transceivers: All low bands on one TRx, which
     is hardwired to a certain ADC/DAC chain and all high bands on 
     the other TRx, which is hardwired to the other ADC/DAC chain */
  uint32 modem_chain;

  /* This captures which NV container to derive calibrated data from.
     Multiple logical paths (RFM devices) which share the same RF path
     will share the same NV container. */
  uint32 nv_container;


}rfc_cfg_info_type;


typedef struct
{
  rfc_msm_signal_type msm_sig_name;
  rfc_logic_type init_state;
  DALGpioPullType gpio_pull;
  DALGpioDriveType gpio_drive;
  #if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
  DALGpioSignalType dal_signal;
  #else
  DALGpioIdType gpio_id;
  #endif
} rfc_signal_info_type;

typedef struct
{
  rfm_device_enum_type primary_rx_device;
  rfm_device_enum_type secondary_rx_device;
} rfc_grfc_device_type;

/*!
   @brief
   This data structure defines the structure of a GRFC script entry
*/
typedef struct
{
  uint16 grfc_id;     /*!< the GRFC_ENGINE ID */
  int16 start_delta; /*!< the time unit at which the GRFC_ENGINE should be activated */
  int16 stop_delta;  /*!< the time unit at which the GRFC_ENGINE should be deactivated */
  uint8 start_logic;  /*!< the logic to apply at the start delta, or immediately*/
  uint8 stop_logic;   /*!< the logic to apply at the stop delta.  
                           Most techs can safely ignore this field.*/
} rfc_grfc_script_entry_type;

/*!
   @brief
   This data structure defines the structure of a GRFC script containing 
   mulitple GRFC entries
*/
typedef struct
{
   rfcom_mode_enum_type tech;
   /*!< the tech of the GRFC script is for */

   uint16  num_grfc; 
   /*!< Number of GRFC */
  
   rfc_grfc_script_entry_type grfc_buffer[ RFC_MAX_NUM_OF_GRFC ];
   /*!< buffer to hold GRFC script entries */
} rfc_grfc_script_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure containing all of the platform-dependent parameters to
  configure the proper hardware devices.
*/
typedef struct rfc_common_logical_device_params_s
{
  /*!
    @brief
    Pointer to the RF chipset common device that should be used for
    this logical device.

    @details
    If this logical device does not have common device initialized, this pointer
    will be NULL.
  */
  rfdevice_rxtx_common_class* cmn_device;

#ifdef FEATURE_RF_HAS_QTUNER
  /*!
    @brief
    Pointer to the QFE device that should be used for
    this logical device.

    @details
    If this logical device does not have common device initialized, this pointer
    will be NULL.
  */
  void *qfe_tuner_device;
  void *qfe_hdet_device;
#endif

} rfc_common_logical_device_params_type;

typedef enum
{
  RFC_ATTENUATION_STATE_LOW,
  RFC_ATTENUATION_STATE_MID,
  RFC_ATTENUATION_STATE_HIGH,
  RFC_ATTENUATION_STATE_INVALID
} rfc_attenuation_state_type;

typedef struct
{
  uint32 rfc_revision;
  rfc_attenuation_state_type rfc_attenuation_state;
} rfc_cmn_properties_type;

typedef struct
{
  rfm_mode_enum_type tech;
  uint64 band;
  uint8 alt_path;
  rfc_rx_path_type rx_path; 
}rfc_alt_path_sel_type;

typedef struct
{
  uint32 band;
  uint32 chan;
} rfc_band_chan_pair;

typedef struct
{
  uint32 band;
  uint32 start_dl_earfcn;
  uint32 stop_dl_earfcn;
}rfc_lte_channel_support_type;

typedef struct
{
  uint32 rfc_revision;
  rfc_band_chan_pair gsm_spur_channels[];
} rfc_gsm_properties_type;

typedef struct
{
  uint32 rfc_revision;
  rfm_wcdma_dual_band_type dbdc_band_combo[];
}rfc_wcdma_properties_type;

typedef struct
{
  uint32 rfc_revision;
  rfc_lte_channel_support_type lte_channel_support[];
}rfc_lte_properties_type;

/* ASDiv Device information */
typedef struct
{
  rfdevice_type_enum_type device_type;
  uint32 instance;
} rfc_asdiv_dev_info_type;


/* Device information for each antenna path */
typedef struct
{
  uint32 ant_path;
  rfc_asdiv_dev_info_type devices[RFC_ASDIV_MAX_DEVICES_PER_ANT_PATH];
} rfc_asdiv_ant_dev_info_type;

/* Device information for each antenna path */
typedef struct
{
  rfc_asdiv_ant_dev_info_type rfc_asdiv_ant_x_dev_info;
  rfc_asdiv_ant_dev_info_type rfc_asdiv_ant_y_dev_info;
} rfc_asdiv_info_type;

/* ASDiv RFC configuration info. */
typedef struct
{
  /*32 bit element capturing the RFC revision:
    upper 8 bits:  Branch/PL revision
    next 8 bits:   Major revision: This gets updated when there is 
                   a change to GPIO/GRFC mapping information, that 
                   could impact all RF Cards. A major revision 
                   update triggers release for all RF cards. 
    lower 16 bits: Minor revision: Any changes specific to certain 
                   RF cards only, such as signal logic or device
                   configurations. Minor revision update only
                   mandates release of affected RF cards. */
  uint32 rfc_revision; 

  /* ASDiv Configuration information, such as antenna_path and devices */
  rfc_asdiv_info_type rfc_asdiv_info[RFC_ASDIV_CONFIG_SWAP_PAIR];
} rfc_asdiv_config_info_type;

/*Availability status of RFC_DEVICE (Physical & Logical Devices) in the RF card */

/* RFC_DEVICE_PRESENT                                             - The device is physically available and it is in good condition.
   RFC_DEVICE_MISSING_NON_FATAL   - The device is not physically available and the user  wants to discard this error.
   RFC_DEVICE_MISSING_FATAL       - The device is physically available but not in good condition and the user wants to catch this issue. 
   RFC_DEVICE_INVALID_STATUS      - Default status of the device.
*/

typedef enum 
{ 
  RFC_DEVICE_PRESENT, 
  RFC_DEVICE_MISSING_NON_FATAL, 
  RFC_DEVICE_MISSING_FATAL, 
  RFC_DEVICE_INVALID_STATUS = 0xFFFFFFFF 
} rfc_device_availability_enum_type;

/*----------------------------------------------------------------------------*/                    
void
rfc_common_write_nv_cb
(
  rex_sigs_type wait_sig
);
             
/* Default RFC AG Accessor function to detect uninitialized RFC AG functions */
void rfc_common_default_func(void);

/* return FALSE if the rfc common is NOT initialized.  */
boolean rfc_common_init_status(void);

/* Initializes the RFC common module.  */
boolean rfc_common_init(rfc_signal_info_type *rfc_sig_table, int32 num_signals);

/* This function Initializes the specified RF signals into their default state. */
boolean rfc_common_init_default(rfc_sig_cfg_type *rfc_signals);

/* This function configuers the specified RF signals to the specified state(HIGH/LOW). */
boolean rfc_common_config_signal(rfc_sig_cfg_type *rf_signals);

/* This function returns the signal number for the requested signal type from the signal list. */
int32 rfc_common_get_grfc_engine_num(rfc_sig_cfg_type *rf_signals, rfc_signal_type sig_type);

/* This function returns the GRFC/GPIO/MSM signal number based on signal name */
int32 rfc_common_get_grfc_num(int32 rf_sig_name);

/* This function returns the GRFC number based on signal type */
int32 rfc_common_get_grfc_num_of_output_type(rfc_signal_type );

/* This function returns the GRFC/GPIO/MSM signal output type based on signal name */
rfc_gpio_grfc_type rfc_common_get_grfc_output_type(int32 rf_sig_name);

boolean rfc_common_append_grfc_scripts(rfc_grfc_script_type *dest_script,rfc_grfc_script_type *source_script);

/* initialize the specified RFC signal to its default state */
void rfc_common_init_rfc_signal(int32 rf_sig_name);

int32 rfc_common_get_grfc_info(rfc_sig_cfg_type *rf_signals, rfc_signal_type sig_type,
                               rfc_sig_cfg_type *rfc_grfc_list);

boolean rfc_common_get_timing_info(rfc_timing_info_type *device_info_ptr,
                                   rfc_timing_param_type timing_type,int32 *rfc_timing);

/* put all the rf signals into active mode if they are not already in active mode */
boolean rfc_common_wakeup(rf_path_enum_type path, rfm_mode_enum_type mode);

/* put all the rf signals into low power mode if none of the other techs are active and if the 
signals are not already in low power mode. */ 
boolean rfc_common_sleep(rf_path_enum_type path, rfm_mode_enum_type mode); 

boolean rfc_common_generate_grfc_script(rfc_sig_cfg_type *rfc_signal_list, 
                                        rf_buffer_intf *rf_buffer,
                                        int16 delay );

boolean rfc_common_generate_init_grfc_script(rfc_sig_cfg_type *rfc_signal_list,  
                                              rf_buffer_intf *rf_buffer,
                                              int16 delay );

rfc_signal_type rfc_common_get_signal_type(int rfc_signal_name);
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Maintains the information on the Common RFC Lock.
 
  @details
  Referenced through RFC code to acquire and release the locks.
  */ 
rf_lock_data_type* rfc_get_lock_data(void);


extern uint8 device_process[2];

boolean rfc_common_set_grfc (uint8 grfc, rfc_logic_type logic);

boolean rfc_read_eeprom();

int32 
rfc_common_get_grfc_buffer
(
  rfc_sig_cfg_type *rf_signals, 
  rfc_signal_type sig_type,
  rf_buffer_intf* script
);

int32 
rfc_common_get_grfc_overwritten_buffer
(
  rfc_sig_cfg_type *rf_signals, 
  rfc_signal_type sig_type,
  rfc_logic_type logic,
  rf_buffer_intf* script,
  int16 delay
);

void 
rfc_common_rffe_scan_efs_dump_cmd_put(void);

void 
rfc_common_rffe_scan_efs_dump
(
  void* cmd_ptr, 
  rf_dispatch_cid_info_type* cid_info, 
  void* cb_data
);

boolean rfc_config_adc_dc_cal(
                                 boolean rx_chain_enable ,     /*Enable/disbale trx rx chain*/
                                 uint32  rx_modem_chain        /*modem chain to map the ADC*/                        
                               );

boolean rfc_get_valid_adc_list( uint32* adc_list );

#ifdef __cplusplus
}
#endif



#endif
