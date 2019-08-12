#ifndef RF_CDMA_DATA_H
#define RF_CDMA_DATA_H

/*! 
  @file
  rf_cdma_data.h
 
  @brief
  This file contains prototypes and definitions to be used by Centralized 
  1xHDR Global Data.
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_data.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/16/16   vr      1X2G QTA 
12/20/15   vr      Autopin check-in
11/28/14   vr      Added checks for FBRx NV support
06/19/14   spa     Added notch filters used and ssma cb data info
04/23/14   cd      Support for 8 PA state functionality
04/11/14   spa     Add support for rxagc tech data ptr
03/09/14   pk      Added interface to query DLNA support
02/20/14   JJ      Added fbrx_lm_handle in rf_cdma_datatwo and two api to 
                   set/get fbrx lm_handle 
02/0714    APU     FR 19614: SHDR on Bolt 
11/25/13   cd      Remove flag-based functionality for legacy Tx AGC
11/07/13   zhw     Split 1x/HDR wu_time logging structures
10/16/13   cc      Add new ET logging for FTM AGC logs
10/08/13   cd      Add support for common TxAGC debug flag
09/26/13   cd      Added functionality to load to RFLM through common TxAGC
09/23/13   sty     compiler warning fixes
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/01/13   spa     Added struct to hold DM buffer pointers
07/25/13   spa     Remove unused IRAT related data types
07/10/13   zhw     Remove dev_status_w in mc layer call flow
07/11/13   spa     Move APIs for mem corruptions sanity check to rf_cdma_data
07/08/13   zhw     Added get write pointer APIs for CDMA data composite members
07/08/13   APU     Use L1 abort flag to skip meas exit events instead of 
                   RF driver having to track it.
07/03/13   spa     Remove intelliceiver power mode from IRAT state machine
07/03/13   APU     Added support for IRAT abort
06/14/13   spa     Use CDMA wrapper object for antenna tuner
05/30/13   JJ      Remove RF_CDMA_INVALID_BAND
05/29/13   spa     Added events for Rx On/Off
05/23/13   JJ      Change curr_band type to rfm_cdma_band_class_type
04/12/13   cd      Remove max PA state functionality
04/01/13   Saul    Removed obsolete code.
02/09/13   bmg     Added data to track antenna tuner script token state
                   between function calls.
01/12/13   cd      RxAGC LNA SM event configuration support
01/03/13   Saul    CDMA. Removed obsolete semaphore member from cdma data.
12/19/12   zhw     HDET event support. Remove temp comp event support
12/18/12   cd      Add support for ET TxLM handle
11/27/12   aki     Added rf_cdma_cleanup_device_status
11/12/12   zhw     Added support for rf_mode query in device status 
11/26/12   APU     Added static events for IRAT tune in and tune out scripts. 
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
11/07/12   jfc     Move the rf hal bus vote handle into rf_cdma_data 
09/27/12   zhw     Remove write_sbi/grfc_via_rfsw flag
09/13/12   sty     Renamed RF_CDMA_EVENT_ANALOG_SCRIPT to RF_CDMA_STATIC_EVENT
09/10/12   aro     FW Response data structure
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/15/12   zhw     Added RF event handle in rf_cdma_data, which are used to 
                   control CCS event flow
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
07/11/12   cri     Added EPT support for CDMA
07/09/12   cd      Added support to store full-bias and APT sweep frequency 
                   compensated linearizers for use by temp comp
06/14/12   aro     Added EPT data structure
06/08/12   cvd     Dynamically allocate memory to rf_cdma_data.rf_device_status
                   by making it an array of pointers
05/29/12   zhw     Moved debug flag do_grfc_write to rf_cdma_data 
                   Refactor API naming
05/19/12   zhw     Moved debug flag do_sbi_writes_in_rf_sw to rf_cdma_data
                   Added API for enable/disable sbi write debug flag  
05/16/12   hdz     Added sanity signature arrays
05/10/12   zhw     renamed rtr_handle to script_index
04/30/12   zhw     Added API for getting mutable rf_rx_reco 
04/27/12   zhw     Added API for getting mutable IC
04/26/12   zhw     Added setAPIs for data_hdr and data_1x 
04/19/12   zhw     Added two set APIs 
                   rf_cdma_data_set_traffic_state()
                   rf_cdma_data_set_isHKADCbusy()
                   Fixed API naming issure for rf_cdma_data_set_rf_state()
04/13/12   zhw     Added new APIs for setting variables as listed below: 
                   rf_cdma_data_set_rf_mode()
                   rf_cdma_data_set_rf_cdma_state_type()
                   rf_cdma_data_set_curr_band()
                   rf_cdma_data_set_curr_chan()
                   rf_cdma_data_set_carrier_index()
                   rf_cdma_data_set_rx_bw_khz()
                   rf_cdma_data_set_tx_bw_khz()
                   rf_cdma_data_set_num_carriers()
                   rf_cdma_data_set_lo_tuned_to_chan()
                   rf_cdma_data_set_rotator_offsets_in_hz()
                   rf_cdma_data_set_assoc_dev()
                   rf_cdma_data_set_rxlm_handle()
                   rf_cdma_data_set_txlm_handle()
                   rf_cdma_data_set_hkadc_critical_sect()
                   rf_cdma_data_set_rtr_handle()
04/09/12   aro     Check to make sure that there is no diversity device active
                   while doing primary device sleep
04/02/12   hdz     Added rfm_hdr_agc_acq_type agc_mode 
03/22/12   aro     Deleted cdma_rf_task file
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/22/12   aro     Added data structure to hold FW responses data with
                   an accessor function
03/21/12   cd      Added support to set HDR Rx AGC logging debug mode
02/29/12   cd      Added new 1x and HDR data types for global data
02/15/12   aro     Created separate data module for semaphore
02/02/12   sty     Added is_voice and pa_backoff flags to rf_cdma_data_status_type
01/04/12   hdz     Added hkadc_critical_sect 
12/19/11   sty     Added new API - rf_cdma_init_freq_info_for_device()
12/14/11   aro     Migrated CDMA MDSP data to CDMA Data module
12/13/11   sty     Changed var name from rfdevice_1x_rx_recommendation_type to 
                   rfdevice_cdma_rx_recommendation_type
12/13/11   Saul    MCDO. Passing rx/tx bandwidth to device driver.
12/10/11   sty     Added rotator_offsets_in_hz to rf_cdma_data_status_type
                   Added prototype for rf_cdma_update_rot_offsets()
12/02/11   hdz     Rename rf_1x_temp_comp_data type to 
                   rf_cdma_temp_comp_data_type
11/30/11   sty     Added lo_tuned_to_chan to rf_cdma_data_status_type
11/29/11   sty     Added SSMA-related data structures
11/22/11   aro     Converted CDMA Status variable to be static
11/22/11   aro     Deleted unused debug variable
11/07/11   hdz     Added tx_lin_LUT[RF_CDMA_TX_LINEARIZER_NUM] to
                   rf_cdma_data_status_type
11/07/11   hdz     Included rfcommon_core_txlin.h
10/31/11   aro     Renamed module to RF TASK
10/30/11   aro     Added data structure and accessor funcitons for
                   RF task data and CDMA debug flags
10/24/11   hdz     Included rf_1x_temp_comp.h 
10/24/11   hdz     Added temp_comp and isHKADCbusy to  rf_cdma_data_status_type
10/20/11   aro     Added interface to return iC Task data structure
10/20/11   aro     Added data structure for iC Task
10/20/11   aro     Doxygen documentation Update
09/20/11   aro     Added interface to get and set the CDMA lock
08/29/11   aro     Added implementation for  PA Access probe config
08/15/11   shb     Added power limiting data to rf_cdma_data_status_type
08/11/11   sty     Added datatype rf_cdma_warmup_data_type to 
                   rf_cdma_data_status_type for RF WU profiling
08/03/11   shb     Renamed hdet_data to hdet in rf_cdma_data_status_type
07/29/11   Saul    IRAT updates
                   - Handle HDR RXAGC TRACK message by skipping post during IRAT.
                   - Added enable/disable control of message router debug messages.
07/25/11   sty     Added curr_max_pwr_limit to rf_cdma_data_status_type
07/19/11   aro     Updated Cal data query function to be done per device to
                   support simultaneous multiple Rx Cal on Cal v3
07/14/11   aro     Added support for CDMA mode in CDMA data structure
06/26/11   sty     Added data structure for HDET
                   Fixed docygen documentation
06/15/11   aro     Replaced intelliceiver data structure
06/09/11   sty     Deleted semaphore.h
06/09/11   sty     Deleted all semaphore-related data - moved to rf_cdma_msgr.c
06/09/11   sty     Added APIs for semaphore use in CDMA msgr module
06/08/11   Saul    Updates for multi-carrier driver.
06/07/11   Saul    Updates for multi-carrier driver.
05/16/11   aro     Removed DVGA Offset field
05/16/11   aro     Added proper unit conversion for exp RxAGC and Offset values
04/21/11   aro     Updated CDMA Data interface for RF Calibraiton/Init
04/21/11   aro     Updated CDMA Device Data to have read and mutable interfaces
04/21/11   aro     Removed rf_cdma_locks.h
04/21/11   aro     Changed to move LOCK implementation to common
04/19/11   cd      Global CDMA data should store LNA offset calibrated values 
                   as signed int16
04/19/11   sty     Added prototypes for APIs to updated Rx and TxLM
04/11/11   aro     Added RTR Handle field in RF Data structure
03/29/11   sty     Added #define RFM_INVALID_NOM_PWR and prototype for 
                   rf_cdma_data_update_assoc_device()
03/25/11   sty     [1] Added rf_hdr_nominal_pwr_dB256 to 
                       rf_cdma_data_status_type
                   [2] Added rf_cdma_data_update_hdr_nominal_pwr
03/21/11   aro     Compiler Error Fix
03/21/11   sty     Added prototype for rf_cdma_is_device_initialized
03/10/11   sty     Moved mdsp-related #defines to MDSP module
03/08/11   aro     Added Data structure to hold CDMA driver Lock Data
03/08/11   sty     Added #defines used while lading linearizers
03/02/11   aro     Re-structured the RxAGC Param type
02/17/11   aro     Renamed intelliceiver function
02/17/11   aro     Moved LNA Gain Step definition to RFM 
02/16/11   aro     Added Intelliceiver Data structure
02/15/11   aro     Updated the variable name
02/14/11   sty     Changed documentation for LM handles
02/09/11   sty     Added RxLM and TxLM buffer indices in 
                   rf_cdma_data_status_type
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     Added LNA Calibration Support
01/31/11   aro     Added RFCal Data structure and accessor function
01/15/11   aro     Updated rf_cdma_data_get_device_status() interface
01/30/11   aro     Added accessor functions to query and update Dev Status
12/29/10   aro     Removed unused structure
12/23/10   aro     Added Intelliceiver Data structure
12/20/10   aro     Updated to make C++ include compatible
12/20/10   aro     Removed hdr_enabled field
12/17/10   aro     Updated Data Structure for RF Status
12/16/10   aro     Doxygen Update
12/14/10   aro     Removed rf_chain_status and expanded Status field
12/13/10   aro     Added a separate CDMA Status Structure
12/09/10   aro     Created NV specific Data Structure
12/08/10   aro     Fixed include filename
12/07/10   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_path_types.h"
#include "rfm_device_types.h"
#include "rfm_gain_state_types.h"
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_ic_task.h"
#include "rf_cdma_hdet.h"
#include "rf_cdma_power_limiting.h"
#include "rf_cdma_time_profile.h"
#include "rf_cdma_lna.h"
#include "rf_cdma_state_types.h"
#include "rf_cdma_atuner.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_hdr_agc_types.h"
#include "rf_cdma_mdsp_types.h"
#include "rfcommon_locks.h"
#include "rfcommon_core_txlin.h"
#include "rfcommon_core_txlin_types.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#include "rf_cdma_ept.h"
#include "rf_hal_common.h"
#include "rf_cdma_fwrsp.h"
#include "rf_cdma_mdsp_modem.h"

#include "rf_1x_temp_comp.h"
#include "rf_1x_tx_agc.h"
#include "rf_1x_fw_response.h"
#include "rf_cdma_rx_agc.h"
#include "rf_hdr_tx_agc.h"
#include "rf_hdr_fw_response.h"
#include "rfcommon_fbrx_types.h"

#include "sys.h"
#include "rex.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Default Chan (-1) to be used for CDMA.*/
#define RF_CDMA_INVALID_CHAN                                               65535

/*----------------------------------------------------------------------------*/
/*! Initalization number for invalid carrier index (-1) to be used for CDMA.*/
#define RF_CDMA_INVALID_CARRIER_INDEX                                      65535

/*----------------------------------------------------------------------------*/
/*! Initialization Number Of Carriers (0).*/
#define RF_CDMA_INVALID_NUM_CARRIERS                                           0

/*! Used to indicate that the link manager buffer is invalid */
#define RFM_INVALID_LM_BUFFER                                              65535

/*! Used to indicate invalid nominal power (used in HDR)  */
#define RFM_INVALID_NOM_PWR                                               0xFFFF

/*! Size of cdma sanity signature arrays   */
#define RF_CDMA_SANITY_SIGNATURE_SIZE 10

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enum types that define possible RF CDMA event types. These event types will be
  used in conjuntion with device IDs to access the corresponding RF event handle
*/
typedef enum
{
  /*! Configure Analog Script*/
  RF_CDMA_STATIC_EVENT,

  /*! Configure RF ON*/
  RF_CDMA_EVENT_RF_ON,

  /*! Configure RF OFF*/
  RF_CDMA_EVENT_RF_OFF,

  /*! Configure RF HDET Read*/
  RF_CDMA_EVENT_HDET_CONFIG,

  /*! Configure IRAT TUNE IN */
  RF_CDMA_EVENT_IRAT_TUNE_IN ,

  /*! Configure IRAT_TUNE OUT */
  RF_CDMA_EVENT_IRAT_TUNE_OUT ,

  /*! Configure LNA G0 SM */
  RF_CDMA_EVENT_LNA_G0_SM,

  /*! Configure LNA G1 SM */
  RF_CDMA_EVENT_LNA_G1_SM,

  /*! Configure LNA G2 SM */
  RF_CDMA_EVENT_LNA_G2_SM,

  /*! Configure LNA G3 SM */
  RF_CDMA_EVENT_LNA_G3_SM,

  /*! Configure LNA G4 SM */
  RF_CDMA_EVENT_LNA_G4_SM,

  /*! Configure Rx ON */
  RF_CDMA_EVENT_RX_ON,

  /*! Configure Rx OFF*/
  RF_CDMA_EVENT_RX_OFF,

  /*! Configure AutoPin Gain State 1 */
  RF_CDMA_EVENT_AUTOPIN_TRIGGER_G1_ID,
  
  /*! Configure AutoPin Gain State 2 */
  RF_CDMA_EVENT_AUTOPIN_TRIGGER_G2_ID,

  /*! MAX NUM OF EVENTS ALLOWED*/
  RF_CDMA_EVENT_INVALID
}rf_cdma_event_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by/during RF Calibration. There
  should not be stray global variables. If global variable is required, it must 
  be contained within this structure. 
*/
typedef struct
{

  int16 lna_offset[RFM_CDMA_POWER_MODE_MAX][RFM_LNA_GAIN_STATE_NUM ];
  /*!< Variable to store LNA offsets measured during LNA calibration
  for all LNA gain Steps.The value stored in this variable is fetched during 
  consecutive LNA offset calibration. The units is in 1/640th of dB. */ 

} rf_cdma_data_calibration_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  1x reverse link channels configuration composite data structure.

  @details
  This structure is used to pass the state of all the channels that can
  be active in the 1x reverse link.
*/
typedef struct
{
  /*! Flag indicating, if the radio is in 1x traffic call for voice (and not in
      access or data call). This info will later be used during configure_tx_agc
      to decide if PA switch points need to be backed off */
  boolean is_voice_call;

  /*! Current SCH rate configuration, if SCH is on */
  rf_1x_tx_agc_sch_rate_type sch_rate;

  /*! Current 1X RF State. The default state for this field will be 
  RF_CDMA_STATE_SLEEP. */
  rf_cdma_state_type rf_state;

} rf_1x_data_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  1x reverse link channels configuration composite data structure.

  @details
  This structure is used to pass the state of all the channels that can
  be active in the 1x reverse link.
*/
typedef struct
{
  /*! Holds the turnaround constant to be used in Tx calculations, as 
  specified by the Access Network. Nominal power stored in dB256 units 
  (since L1 needs it to be in dB256) */ 
  int16 hdr_nominal_pwr_dB256;

  /*! Current EVDO RRI value, provided by L1 */
  rf_hdr_tx_agc_rri_type rri;

  /*! Current HDR RF State. The default state for this field will be 
  RF_CDMA_STATE_SLEEP. */
  rf_cdma_state_type rf_state;

} rf_hdr_data_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  IRAT specific data structure.

  @details
  This structure contains all the items pertaining to IRAT. When we enter IRAT
  from either CDMA to X OR FROM X to CDMA, we init this structure with the 
  pertinent information. On leaving IRAT, while going back to the source tech,
  either CDMA or tech-X, we deinit\invalidate all the contents.
*/
typedef struct 
{
  /*! This stores the buffer\script index as passed by
     L1 
  */
  uint32                    irat_buffer_index ;

  /*! This stores the RXLM index as passed by  L1 
  */
  lm_handle_type            irat_rxlm_handle ;

} rf_cdma_irat_info_type ;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Status of CDMA autopin module.
*/
typedef struct
{
  boolean tx_enabled;
  boolean pin_data_valid;
  uint32  wakeup_flag;
} rfcdma_autopin_state_type;
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Status of CDMA Driver.
*/ 
typedef struct
{

  uint32 signature1[RF_CDMA_SANITY_SIGNATURE_SIZE];

  rfm_mode_enum_type rf_mode;
  /*!< Current RFM Mode to which the device is entered for. This will just 
  be a local copy of rfm_mode for CDMA driver usage. */

  /*! CDMA Band to which the device is tuned to. The default band for CDMA 
  system will be the invalid Band*/
  rfm_cdma_band_class_type curr_band;  

  /*! CDMA RF channels to which the device is tuned to. The default value 
  for these fields will be RF_CDMA_INVALID_CHAN indicating invalid Channel. 
  Item index 0 is to be used for single-carrier configuration.*/
  uint16 curr_chans[RFM_CDMA_CARRIER_NUM];

  /*! An array in integers that specify the carrier the corresponding channel
  should be placed on in the multi-carrier Rx front.  This carrier will
  deliver samples into the corresponding sample RAM for demodulation.*/
  uint16 carrier_index[RFM_CDMA_CARRIER_NUM];

  /*!< The current Rx bandwidth in kHz under which the device is operating */
  uint32 rx_bw_khz;

  /*!< The current Tx bandwidth in kHz under which the device is operating */
  uint32 tx_bw_khz;

  /*! The number of active carriers.*/
  uint8 num_carriers;

  /*! An array of booleans indicating which carriers are active or not. */
  /*rfm_cdma_carrier_en_mask_type carrier_mask;*/

  /*! the "actual" channel to which the LO (RX and TX) is tuned to
      - for 1x only, it would be the same as curr_chans[RFM_CDMA_CARRIER_0] 
      - for MCDO, it depends on the channels in the curr_chans[] array
      - In case of SHDR, this depends on calculations from SSMA module 
      
      This variable is updated everytime the device is tuned to a band/channel
  */
  uint16 lo_tuned_to_chan;

  /*! Holds the current freq offsets (in Hz) that the rotator is programmed to. 
      
      For HDR:
      
      The freq_offset of each rotator depends on difference between
      center_freq of curr_chans[] and each element of curr_chans[]
      
      rotator_offsets[carrier_x] = center_freq - curr_chans[carrier_x]
      
      For 1x:
      it is the same as HDR above, except that, rotator_offsets[] will
      all be 0's (since only curr_chan[RFM_CDMA_CARRIER_0] is valid)
      
      In case SSMA is involved, for 1x, the rotator_offsets[RFM_CDMA_CARRIER_0]
      would be (lo_tuned_to_chan - curr_chan[RFM_CDMA_CARRIER_0])
      
      SSMA does not affect rotatots in HDR
      
      This variable is updated everytime the device is tuned to a band/channel
  */
  int32 rotator_offsets_in_hz[RFM_CDMA_CARRIER_NUM];

  /*!                                                            
  @brief
  This is used when the device has diversity enabled
   
  @details 
  This variable indicates other associated device with this device. 
  When the system is operating in Rx Diversity Mode, this field for
  primary device will hold the the slave device number on which Rx diversity is 
  turned on. On the other hand, this field for secondary device will hold the 
  master device number to which the secondary device is associated with. The 
  determination of master or slave device during diversity mode will be 
  identified by checking the rf_state. If rf_state is  RF_CDMA_STATE_RX and 
  RF_CDMA_STATE_RXTX, then it is a master device; whereas if it is 
  RF_CDMA_STATE_RXDIV, then it is a slave device. In case of non-diversity mode 
  of operation, this field will hold invalid device, RFM_INVALID_DEVICE. The 
  default value for this field will also be RFM_INVALID_DEVICE. */
  rfm_device_enum_type assoc_dev;

  /*!< Structure Containing Common Global Data used by CDMA Interlliceiver
  Driver. There should not be stray global variables. If global variable is 
  required, it must be contained within this structure.  */
  rf_cdma_ic_data_type intelliceiver;

  /*! Data tells the DLNA support on this device */  
  boolean is_dlna_supported;

  uint32 signature2[RF_CDMA_SANITY_SIGNATURE_SIZE];

  /*! Structure containing all data related to HDET */
  rf_cdma_hdet_data_type hdet;

  uint32 signature3[RF_CDMA_SANITY_SIGNATURE_SIZE];

  /*! Structure containing all data related to Temp Comp */
  rf_cdma_temp_comp_data_type temp_comp;

  uint32 signature4[RF_CDMA_SANITY_SIGNATURE_SIZE];

  /*!critical section protection for hdet/temp comp when obtaining hkadc lock */
  rf_lock_data_type hkadc_critical_sect;
  
  /*! Structure containing all data for TX power limiting */
  rf_cdma_plim_data_type plim;

  /*! This variable is used to get a handle to RxLM buffer */
  lm_handle_type rxlm_handle;

  /*! This variable is used to get a handle to FBRx LM buffer */
  lm_handle_type fbrx_lm_handle;

  /*! This variable is used to get a handle to TxLM buffer */
  lm_handle_type txlm_handle;

  /*! Script Buffer Index */
  uint32 script_index;

  /*! Holds global data specific to 1x mode operation */
  rf_1x_data_status_type data_1x;

  /*! Holds global data specific to 1x mode operation */
  rf_hdr_data_status_type data_hdr;

  /*! Flag indicating, if the radio is in CDMA traffic state. This variable 
  will be set when 1x (or HDR) L1 explicting calls PA Access Probe configuraion 
  function. This is will be reset to FALSE, when Tx is put to sleep. */ 
  boolean traffic_state;

  /*!Structure containing all data related to RF warmup-time profiling for 1X*/
  rf_cdma_warmup_data_type rf_wu_time_1x;

  /*!Structure containing all data related to RF warmup-time profiling for HDR*/
  rf_cdma_warmup_data_type rf_wu_time_hdr;

  /*! RF Calibration Data */
  rf_cdma_data_calibration_type cal_data;

  /*! current full-bias sweep freq comp-ed tx linearizer table */
  tx_linearizer_table_type fb_swp_tx_linearizer[RFCOM_TXLIN_NUM_LINEARIZERS];
 
  /*! current APT sweep freq comp-ed tx linearizer table */
  tx_linearizer_table_type apt_swp_tx_linearizer[RFCOM_TXLIN_NUM_LINEARIZERS];
  
  /*! Flag indicating if auxiliary data (DPD) is present along with 
      the standard tx linearizer data, per PA state */
  tx_linearizer_aux_data_type tx_lin_aux_data[RFCOM_TXLIN_NUM_LINEARIZERS];
 
  /*! Structure that contains all the baseband and RF recommendations for the 
     current <band, chan> */
  rfdevice_cdma_rx_reco_type rf_rx_reco;

  /*! The current PRx HDR AGC mode*/
  rfm_hdr_agc_acq_type agc_mode;
 
  /*! Flag to indicate if HKADC is in use or not */
  boolean isHKADCbusy;

  rfcommon_mdsp_event_handle* event_handle_ptr[RF_CDMA_EVENT_INVALID];

  /*! Flag to indicate if APT PA Quiescent current bis is enabled */
  boolean apt_pa_q_current_enabled;

  /*! Flag to indicate if EPT Tx operation is enabled through NV */
  boolean ept_online_enabled;

  /*! Store default IQ gain value for CDMA, obtained from TxLM */
  int16 cdma_iq_gain_val;

  /*! Store default IQ gain value for MCDO, obtained from TxLM */
  int16 mcdo_iq_gain_val ;

  /*! Store differences in IQ gain values for MCDO vs SCDO in 
    dB10 units */
  int16 mcdo_vs_scdo_iq_gain_dB10 ;

  /*! Store the EPT IQ gain value for CDMA, obtained from NV */
  int16 ept_iq_gain_val;

  /*! Store EPT nominal envelope scale gain */
  int32 ept_env_scale_nominal;

  /*! Store ET nominal envelope scale gain */
  int32 et_env_scale_nominal;

  /*! Pointer to handle to be used for voting for the RF HAL (RFCMD app) */
  rf_hal_bus_client_handle* rf_hal_bus_vote_handle;
 
  /*! Current antenna tuner script token to be acknowledged when FW 
      queued script execution is complete. */
  ant_tuner_device_tokens_type ant_tuner_script_token_data;

  /*! Structure to hold FW Response Data  */
  rf_cdma_fwrsp_data_type fwrsp_data;

  /*! Structure to hold Irat Specific Data  */
  rf_cdma_irat_info_type irat_data ;

  /*! Temporary structure to hold Tx AGC timing information */
  rfcommon_core_txlin_agc_timing_params txagc_timing_params;

  uint32 signature5[RF_CDMA_SANITY_SIGNATURE_SIZE];

  /*! Pin Compensation values */
  int16 pin_comp_val_temp_vs_freq[RFCOM_TXLIN_NUM_LINEARIZERS];
  
  /*! Pout Compensation values over freq */
  int8 pout_comp_val_freq[RFCOM_TXLIN_NUM_LINEARIZERS]
                         [RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN];
  
  /* Pout Compensation values over temp vs freq */
  int16 pout_comp_val_temp_vs_freq[RFCOM_TXLIN_NUM_LINEARIZERS];
 
  /*! Structure to hold Rx AGC related information */
  rf_cdma_rxagc_data_t rxagc_data;
 
  /*! Structure to hold Rx AGC related information */
  uint8 num_notches_used;

  /*! Callback data info for SSMA callback API*/
  rfdevice_cdma_ssma_callback_data_type ssma_cb_data;

  rfcdma_autopin_state_type autopin_st;
 
  /*! This flag allows us to skip Pout Compensation */
  boolean skip_pout_comp ;

} rf_cdma_data_status_type;

/*----------------------------------------------------------------------------*/
/*! Structure containing CDMA debug flags */
typedef struct
{

  boolean ssma_enabled;
  /*!< Flag to enable/disable SSMA */

  boolean rx_agc_logging[RFM_MAX_DEVICES];
  /*! Flag to enable/disable Rx AGC logs per device */
} rf_cdma_debug_flag_type;

/*----------------------------------------------------------------------------*/
/*! Structure containing FBRx enable/disable flags */
typedef struct
{

  boolean fbrx_enabled_1x;
  /*!< Flag to enable/disable fbrx for 1x */

  boolean fbrx_enabled_hdr;
  /*! Flag to enable/disable fbrx for HDR */
} rf_cdma_fbrx_enable_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold FW response data pertaining to CDMA */
typedef struct
{
  rf_1x_fw_response_data_type onex_fw_rsp;
  /*!< Structure for 1x FW response data */

  rf_hdr_fw_response_data_type hdr_fw_rsp;
  /*!< Structure for 1x FW response data */

} rf_cdma_fw_response_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by 1xHDR Driver. There should not
  be stray global variables. If global variable is required, it must be 
  contained within this structure. 
*/ 
typedef struct
{
  rf_cdma_data_status_type *rf_device_status[RFM_MAX_DEVICES];
  /*!< CDMA Driver Status */
  
  rf_cdma_mdsp_data_type mdsp;
  /*!< Data structure to hold all CDMA MDSP data */
  
  rf_cdma_ic_task_data_type ic_task;
  /*!< Data structure for CDMA intelliceiver task */

  rf_cdma_fw_response_data_type fw_response;
  /*!< Data structure for FW response */
    
  rf_cdma_ept_data_type ept;
  /*!< Structure Containing Common Global Data used by/during RF 
  1x EPT Calibration */
  
  rf_lock_data_type *crit_section;
  /*!< Global Mutex for Common CDMA Driver */

  rf_cdma_debug_flag_type debug_flag;
  /*!< Structure containing CDMA debug flags */

  rf_cdma_fbrx_enable_type fbrx_en;
  /*!< Structure containing FBRx enable flags for CDMA 1X & HDR */

} rf_cdma_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_init_device_status
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_cleanup_device_status
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_init_freq_info_for_device
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
const rf_cdma_data_status_type*
rf_cdma_get_device_status
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rfdevice_cdma_rx_reco_type*
rf_cdma_get_mutable_rf_rx_reco
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_hdet_data_type*
rf_cdma_get_mutable_hdet
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_plim_data_type*
rf_cdma_get_mutable_plim
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_temp_comp_data_type*
rf_cdma_get_mutable_temp_comp
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_ic_data_type*
rf_cdma_get_mutable_intelliceiver
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ant_tuner_device_tokens_type*
rf_cdma_get_mutable_ant_tuner_token
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_lock_data_type*
rf_cdma_get_mutable_hkadc_critical_sect
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_data_status_type*
rf_cdma_get_mutable_device_status
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_is_device_initialized
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_has_associated_device
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_lock_data_type*
rf_cdma_get_lock_data
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_set_lock_data
(
  rf_lock_data_type *lock_data
);

/*----------------------------------------------------------------------------*/
const rf_cdma_data_calibration_type *
rf_cdma_get_cal_data
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_data_calibration_type *
rf_cdma_get_mutable_cal_data
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_mdsp_data_type *
rf_cdma_get_mdsp_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_cdma_ic_task_data_type *
rf_cdma_get_ic_task_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_1x_fw_response_data_type *
rf_cdma_get_1x_fw_response_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_hdr_fw_response_data_type *
rf_cdma_get_hdr_fw_response_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_cdma_fwrsp_data_type *
rf_cdma_get_fwrsp_data
(
   rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
const rf_cdma_debug_flag_type *
rf_cdma_get_debug_flag_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_cdma_ept_data_type *
rf_cdma_get_ept_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_cdma_debug_flag_type *
rf_cdma_get_mutable_debug_flag_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_event_handle ** 
rf_cdma_data_get_event_handle
(
  rfm_device_enum_type device,
  rf_cdma_event_type event_type
);


/*----------------------------------------------------------------------------*/
boolean
rf_cdma_data_update_rot_offsets
(
  rfm_device_enum_type device,
  int32 freq_offsets_hz[]
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_data_set_rxagc_log_mode
(
  rfm_device_enum_type device,
  boolean enable
);

void
rf_cdma_data_set_rf_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_1x_rf_state
(
  rfm_device_enum_type device,
  rf_cdma_state_type rf_state
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_hdr_rf_state
(
  rfm_device_enum_type device,
  rf_cdma_state_type rf_state
);

/*----------------------------------------------------------------------------*/
rf_cdma_state_type
rf_cdma_data_get_1x_rf_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_state_type
rf_cdma_data_get_hdr_rf_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rf_cdma_state_type
rf_cdma_data_get_rf_state
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_curr_band
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type curr_band
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_curr_chan
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  uint16 curr_chans
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_carrier_index
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  uint16 carrier_index
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_rx_bw_khz
(
  rfm_device_enum_type device,
  uint32 rx_bw_khz
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_data_get_rx_bw_khz
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_tx_bw_khz
(
  rfm_device_enum_type device,
  uint32 tx_bw_khz
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_num_carriers
(
  rfm_device_enum_type device,
  uint32 num_carriers
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_lo_tuned_to_chan
(
  rfm_device_enum_type device,
  uint16 lo_tuned_to_chan
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_rotator_offsets_in_hz
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  int32 rotator_offsets_in_hz
);
  
/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_assoc_dev
(
  rfm_device_enum_type device,
  rfm_device_enum_type assoc_dev
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_rxlm_handle
(
  rfm_device_enum_type device,
  lm_handle_type rxlm_handle
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_fbrx_lm_handle
(
  rfm_device_enum_type device,
  lm_handle_type fbrx_lm_handle
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_set_pout_comp_skip
(
  rfm_device_enum_type device ,
  boolean              skip_or_not 
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_get_pout_comp_skip
(
  rfm_device_enum_type device 
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_txlm_handle
(
  rfm_device_enum_type device,
  lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_hkadc_critical_sect
(
  rfm_device_enum_type device,
  rf_lock_data_type hkadc_critical_sect
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_script_index
(
  rfm_device_enum_type device,
  uint32 script_index
);
  
/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_traffic_state
(
  rfm_device_enum_type device,
  boolean traffic_state
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_isHKADCbusy
(
  rfm_device_enum_type device,
  boolean isHKADCbusy
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_hdr_nominal_pwr
(
  rfm_device_enum_type device,
  int16 hdr_nominal_pwr_dB256 
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_hdr_rri
(
  rfm_device_enum_type device,
  rf_hdr_tx_agc_rri_type rri
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_is_voice_call
(
  rfm_device_enum_type device,
  boolean is_voice_call
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_1x_sch_rate
(
  rfm_device_enum_type device,
  rf_1x_tx_agc_sch_rate_type sch_rate
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_data_set_rf_hal_bus_vote_handle
(
  rfm_device_enum_type device,
  rf_hal_bus_client_handle* handle
);

/*----------------------------------------------------------------------------*/
rf_hal_bus_client_handle*
rf_cdma_data_get_rf_hal_bus_vote_handle
(
  rfm_device_enum_type device
);


/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_pa_q_current_enabled
(
   rfm_device_enum_type device,
   boolean enable_pa_q_current
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_ept_online_enabled
(
   rfm_device_enum_type device,
   boolean enable_ept_online
);


/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_ept_iq_gain_val
(
   rfm_device_enum_type device,
   int16 ept_iq_gain
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_cdma_iq_gain_val
(
   rfm_device_enum_type device,
   int16 cdma_iq_gain
);

/*----------------------------------------------------------------------------*/
rfm_mode_enum_type
rf_cdma_data_get_rf_mode
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_init_irat 
(
  rfm_device_enum_type            device ,
  uint32                          buffer_index ,
  lm_handle_type                  rxlm_handle 
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_deinit_irat 
(
  rfm_device_enum_type            device
) ;

/*----------------------------------------------------------------------------*/
rfcommon_core_txlin_agc_timing_params*
rf_cdma_data_get_txagc_timing_params
(
  rfm_device_enum_type device
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_data_sanity_check_init
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_sanity_check
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
lm_handle_type
rf_cdma_data_get_txlm_handle
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
lm_handle_type
rf_cdma_data_get_rxlm_handle
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
lm_handle_type
rf_cdma_data_get_fbrx_lm_handle
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_data_get_fbrx_enable
(
  rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_fbrx_enable
(
  rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_rxagc_cell_id
(
  rfm_device_enum_type device,
  cell_id_t cell_id
);

/*----------------------------------------------------------------------------*/
cell_id_t
rf_cdma_data_get_rxagc_cell_id
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_rxagc_tech_data_ptr
(
  rfm_device_enum_type device,
  void* tech_data_ptr
);

/*----------------------------------------------------------------------------*/
void*
rf_cdma_data_get_rxagc_tech_data_ptr
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_data_set_dlna_support
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rfm_cdma_band_class_type
rf_cdma_data_get_curr_band
(
  rfm_device_enum_type device
) ;

rfm_device_enum_type 
rf_cdma_data_get_curr_tx_device
(
  void
);


#endif /* FEATURE_CDMA1X */
#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_DATA_H */
