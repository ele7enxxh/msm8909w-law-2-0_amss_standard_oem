/*! 
  @file
  rf_cdma_data.c
 
  @brief
  This file contains Centralized Global Data used by Common 1xHDR Driver.
 
  @addtogroup RF_CDMA_COMMON_DATA
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_data.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/16/16   vr      1X2G QTA 
12/20/15   vr      Autopin check-in 
12/15/14   vc      Added missing include file for FBRx APIs
11/28/14   vr      Added checks for FBRx NV support
05/20/14   zhw     KW fix
04/11/14   spa     Add support for rxagc tech data ptr
03/09/14   pk      Added interface to query DLNA support
02/20/14   JJ      Added two api to set/get fbrx lm_handle
02/0714    APU     FR 19614: SHDR on Bolt 
11/25/13   cd      Remove flag-based functionality for legacy Tx AGC
11/05/13   cd      Provide updated timing constants for Bolt
10/08/13   cd      Add support for common TxAGC debug flag
10/08/13   cd      Use different Tx AGC timing constants for D3925 as all 
                   devices are on same RFFE bus
10/05/13   cd      Updated Tx aGC timing params based on systems reco
09/26/13   cd      Added functionality to load to RFLM through common TxAGC
09/19/13   APU     Implemented txlm and rxlm getter APIs.
09/16/13   JJ      Added rflm cdma init
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/01/13   spa     Added API to return DM buffer pointer structure
07/25/13   spa     Deinit IRAT data structure to -1 & RXLM_MAX_BUFFERS ,not zero
07/15/13   zhw     Remove dev_status_w in mc layer call flow
07/11/13   spa     Move APIs for mem corruptions sanity check to rf_cdma_data
07/08/13   zhw     Added get write pointer APIs for CDMA data composite members
07/08/13   APU     Use L1 abort flag to skip meas exit events instead of 
                   RF driver having to track it.
07/03/13   spa     Remove intelliceiver power mode from IRAT state machine
07/03/13   APU     Added support for IRAT abort.
06/12/12   APU     Added support to init iRAT specific entities from MC.
05/29/13   JJ      Check Initialized band with RFM_CDMA_MAX_BAND
05/28/13   JJ      fix a bug for rfm_cdma_band_class_type invlid_band
05/23/13   JJ      Change curr_band type to rfm_cdma_band_class_type
04/12/13   cd      Remove max PA state functionality
04/01/13   Saul    Removed obsolete code.
01/03/13   Saul    CDMA. Removed obsolete semaphore member from cdma data.
12/19/12   sty     Added lower bound check in rf_cdma_get_device_status
12/18/12   cd      Add support for ET TxLM handle
12/12/12   sty     removed updating associated_device in 
                   rf_cdma_init_freq_info_for_device()
11/27/12   aki     Added rf_cdma_cleanup_device_status
11/12/12   zhw     Added support for rf_mode query in device status 
11/15/12   Saul    CDMA. TX AGC XPT.
11/09/12   zhw     Clockwork error fix
11/07/12   jfc     Move the rf hal bus vote handle into rf_cdma_data 
09/27/12   zhw     Remove write_sbi/grfc_via_rfsw flag
09/12/12   spa     Removed featurization flag RF_HAS_CDMA_DIME_SUPPORT 
09/13/12   sty     Renamed RF_CDMA_EVENT_ANALOG_SCRIPT to RF_CDMA_STATIC_EVENT
09/10/12   aro     FW Response data structure
08/24/12   bmg     Initialize CDMA device data structure to 0 after allocation
08/30/12   sty     Changes needed in rf_cdma_data_set_assoc_dev() since 
                   members have been moved around in rfm_device_enum_type
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/17/12   zhw     Added RF event handle in rf_cdma_data, which are used to 
                   control CCS event flow
08/16/12   spa     Removed F3 in rf_cdma_get_device_status if device is invalid 
08/07/12   sty     Warning fix
08/06/12   hdz     Init plim.tx_pwr_override to FALSE
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
07/10/12   sty     removed DIME feature from rf_cdma_data_set_script_index
06/29/12   spa     Compiler warning fix 
06/14/12   aro     Added EPT data structure
06/14/12   Saul    CDMA. Set script-writes to happen by FW.
06/08/12   cvd     Dynamically allocate memory to rf_cdma_data.rf_device_status
06/06/12   Saul    CDMA. Sending scripts as Dime shared mem format.
                   MC now using mdsp events.
05/29/12   zhw     Refactored API naming  
05/29/12   zhw     Moved debug flag do_grfc_write to rf_cdma_data 
                   Added API for enable/disable grfc write debug flag  
05/19/12   zhw     Moved debug flag do_sbi_writes_in_rf_sw to rf_cdma_data
                   Added API for enable/disable sbi write debug flag  
05/10/12   zhw     Renamed rtr_handle to script_index
04/30/12   zhw     Added API for getting mutable rf_rx_reco
04/27/12   zhw     Fixed compiler error
04/27/12   zhw     Added API for getting mutable IC
04/26/12   zhw     Added setAPIs for data_hdr and data_1x 
04/20/12   zhw     Doxygen Update
04/19/12   zhw     Added two set APIs: 
                   rf_cdma_data_set_traffic_state()
                   rf_cdma_data_set_isHKADCbusy()
                   Fixed rf_cdma_data_set_rf_cdma_state_type()
                   with new function name rf_cdma_data_set_rf_state()
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
03/22/12   aro     Removed unsed functions and types
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/22/12   spa     Added max device checks for rf_cdma_get_cal_data() and 
                   rf_cdma_get_mutable_cal_data() 
03/22/12   aro     Added data structure to hold FW responses data with
                   an accessor function
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/21/12   cd      Added interface to set HDR Rx AGC logging debug mode
03/15/12   spa     Added max device check for rf_cdma_get_device_status() 
02/29/12   cd      Added new 1x and HDR data types for global data
02/22/12   sty     Removed checks in rf_cdma_get_*()
02/15/12   aro     Created separate semaphore module
02/02/12   sty     Added init values for is_voice and pa_backoff 
01/27/12   hdz     Remove fcomp from max power limit module  
01/05/12   sty     Added rf_cdma_get_mutable_debug_flag_data
12/19/11   sty     [1] Added new API rf_cdma_init_freq_info_for_device()
                   used rf_cdma_init_freq_info_for_device() in 
                   [2] rf_cdma_init_device_status()
12/14/11   aro     Added accessor funtion to get MDSP data pointer
12/13/11   Saul    MCDO. Passing rx/tx bandwidth to device driver.
12/10/11   sty     Added rf_cdma_update_rot_offsets()
12/06/11   sty     Init LO chan element in SSMA recos to invalid 
12/02/11   cd      Set traffic state to TRUE during initialization 
11/30/11   sty     Initialize lo_tuned_to_chan
11/29/11   sty     Check for valid device before returning a pointer in *_get()
                   Added ssma_enabled flag
11/22/11   aro     Converted CDMA Status variable to be static
11/22/11   aro     Deleted unused debug variable
11/11/11   Saul    Thermal Backoff Support.
11/10/11   Saul    SV Limiting. Initial support.
10/30/11   aro     Added function to get pointer to CDMA debug flags
10/30/11   aro     Added function to get pointer to RF TASK dispatcher data
10/30/11   aro     Doxygen update
10/20/11   aro     Added interface to return iC Task data structure
10/06/11   sty     Check for NULL pointer in rf_cdma_get_lock_data()
09/20/11   aro     Added interface to get and set the CDMA lock
07/29/11   Saul    IRAT updates
                   - Handle HDR RXAGC TRACK message by skipping post during IRAT.
                   - Added enable/disable control of message router debug messages.
07/19/11   aro     Updated Cal data query function to be done per device to
                   support simultaneous multiple Rx Cal on Cal v3
06/09/11   sty     Deleted all semaphore-related data - moved to rf_cdma_msgr.c
06/09/11   sty     Added APIs for semaphore operations
06/08/11   Saul    Updates for multi-carrier driver.
06/07/11   Saul    Updates for multi-carrier driver.
04/21/11   aro     [1] Updated CDMA Data interface for RF Calibraiton/Init
                   [2] Updated Doxygen Blocks
04/21/11   aro     Updated CDMA Device Data to have read and mutable interfaces
04/21/11   aro     Changed to move LOCK implementation to common
04/19/11   sty     Fixed doxygen warnings
04/17/11   sty     Added rf_cdma_data_update_rxlm_handle and 
                   rf_cdma_data_update_txlm_handle
03/29/11   sty     Initialized rf_hdr_nominal_pwr_dB256 in 
                   rf_cdma_data_init_device_status
                   Added APi rf_cdma_data_update_assoc_device
03/25/11   sty     Added API - rf_cdma_data_update_hdr_nominal_pwr 
03/21/11   sty     Added API - rf_cdma_is_device_initialized
03/08/11   aro     Added accessor function query CDMA driver Lock Data
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   aro     Renamed intelliceiver function
02/16/11   aro     Added Intelliceiver Data structure
02/15/11   aro     Updated the variable name
02/10/11   sty     Update rxlm and txlm buffers to initial state in 
                   rf_cdma_data_init_device_status()
01/31/11   aro     Added accessor function to resturn RF Cal Data Pointer
01/15/11   aro     [1] Added Parameter Validation
                   [2] Updated rf_cdma_data_get_device_status() interface
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
01/30/11   aro     Added accessor functions to query and update Dev Status
12/18/10   aro     Added Loop to init all devices
12/17/10   aro     Added function to initialize Device Status to defualt value
12/14/10   aro     Removed rf_chain_status and expanded Status field
12/13/10   aro     Renamed the CDMA GLobal Data
12/08/10   aro     Fixed include filename
12/07/10   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_ssma.h"
#include "modem_mem.h"
#include "rfm_gain_state_types.h"
#include "rfcommon_fbrx_api.h" /* For FBRx interfaces */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Variable to store global Data to be used by 1xHDR Driver
*/
static rf_cdma_data_type rf_cdma_data;

/*============================================================================*/
/*!
  @name Device Status

  @brief
  This section includes interfaces pertaining to RF Device Status Data
  structure.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialization RF Device Status

  @details
  This function will initialize all fields of RF Device Status to a known
  default state. This function should be called only when CDMA Driver
  is initialized.
 
  @return
  Status of Device Status Initialization
*/
boolean
rf_cdma_init_device_status
(
  void
)
{
  boolean ret_val = TRUE;

  rfm_device_enum_type dev_idx; /* Variable for Device Loop */

  /* Variable for CCS event handle loop*/
  rf_cdma_event_type ccs_handle_idx; 
 /*--------------------------------------------------------------------------*/

  /* Set all global CDMA RF Driver state to a known configuration */
  
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    if( rfc_cdma_get_logical_device_params(dev_idx) != NULL )
    {
      rf_cdma_data.rf_device_status[dev_idx] = (rf_cdma_data_status_type*)
        modem_mem_alloc( sizeof(rf_cdma_data_status_type),
                         MODEM_MEM_CLIENT_RFA );

      if(rf_cdma_data.rf_device_status[dev_idx] == NULL)
      {
        ret_val = FALSE;
        break;
      }

      /* Dynamic allocation should emulate the old global behavior:
         zero initialize the data structure. */
      memset( rf_cdma_data.rf_device_status[dev_idx], 0,
              sizeof(rf_cdma_data_status_type) );

      /* Default 1x state is Sleep State */
      rf_cdma_data.rf_device_status[dev_idx]->data_1x.rf_state = RF_CDMA_STATE_SLEEP;

      /* Default HDR state is Sleep State */
      rf_cdma_data.rf_device_status[dev_idx]->data_hdr.rf_state = RF_CDMA_STATE_SLEEP;

     /* Default assciated/slave device is Invalid Device */ 
     rf_cdma_data.rf_device_status[dev_idx]->assoc_dev = RFM_INVALID_DEVICE;

      /* init all info related to band/chan */
      rf_cdma_init_freq_info_for_device(dev_idx);
  
      /* Initialize the RxLM buffer pointer to an invalid value */
      rf_cdma_data.rf_device_status[dev_idx]->rxlm_handle = RFM_INVALID_LM_BUFFER;
  
      /* Initialize the TxLM buffer pointer to an invalid value */
      rf_cdma_data.rf_device_status[dev_idx]->txlm_handle = RFM_INVALID_LM_BUFFER;
  
      /* Initialize nominal power to an invalid value */
      rf_cdma_data.rf_device_status[dev_idx]->data_hdr.hdr_nominal_pwr_dB256 = 
                                                              RFM_INVALID_NOM_PWR;
  
      /* Initialize to highest max power */
      rf_cdma_data.rf_device_status[dev_idx]->
        plim.max_power_limits.max_power_limit_sar_dbm10 = 420;
  
      /* Initialize to highest max power */
      rf_cdma_data.rf_device_status[dev_idx]->
        plim.max_power_limits.max_power_limit_sv_dbm10 = 420;
  
      /* Initialize to highest max power */
      rf_cdma_data.rf_device_status[dev_idx]->
        plim.max_power_limits.max_power_limit_thermal_backoff_dbm10 = 420;

      /* Initialize to highest max power */
      rf_cdma_data.rf_device_status[dev_idx]->
        plim.max_power_limits.max_power_limit_vs_temp_vs_freq_dbm10 = 420 ;

      /* Initialize to highest max power */
      rf_cdma_data.rf_device_status[dev_idx]->
        plim.max_power_limits.max_power_info.max_hdr_ccp_pwr_dB10 = 420 ;
  
      rf_cdma_data.rf_device_status[dev_idx]->
        plim.max_power_limits.max_power_info.debug_hdr_max_power = TRUE ;

      /* Set traffic state to TRUE during initialization */
      rf_cdma_data.rf_device_status[dev_idx]->traffic_state = TRUE;
  
      /* Set is voice_call flag to TRUE during initialization */
      rf_cdma_data.rf_device_status[dev_idx]->data_1x.is_voice_call = TRUE;
  
      /* Initialize 1x and HDR reverse link configuration */
      rf_cdma_data.rf_device_status[dev_idx]->data_1x.sch_rate = 
        RF_1X_TX_AGC_SCH_FULL_RATE;
      rf_cdma_data.rf_device_status[dev_idx]->data_hdr.rri = 
        RF_HDR_TX_AGC_9K6_BPS_RRI;

      /* Initialize CCS event handle pointers to be NULL*/
      for ( ccs_handle_idx = 0; ccs_handle_idx < RF_CDMA_EVENT_INVALID; 
            ccs_handle_idx++ )
      {
        rf_cdma_data.rf_device_status[dev_idx]->
          event_handle_ptr[ccs_handle_idx] = NULL;
      }

      /* Initialize not to do tx power limit override*/
      rf_cdma_data.rf_device_status[dev_idx]->
        plim.tx_override_pwr_limit_dbm10 = 420;

      rf_cdma_data.rf_device_status[dev_idx]->ept_online_enabled = FALSE;

      rf_cdma_data.rf_device_status[dev_idx]->cdma_iq_gain_val = 0;
      rf_cdma_data.rf_device_status[dev_idx]->mcdo_iq_gain_val = 0;
      rf_cdma_data.rf_device_status[dev_idx]->mcdo_vs_scdo_iq_gain_dB10 = 0;

      rf_cdma_data.rf_device_status[dev_idx]->ept_iq_gain_val = 0;

      rf_cdma_data.rf_device_status[dev_idx]->apt_pa_q_current_enabled = FALSE;

      /* Initialize Tx AGC timing values in usec x 10 */
      rf_cdma_data.rf_device_status[dev_idx]->
                    txagc_timing_params.wtr_early_trig_offset = -80;
      rf_cdma_data.rf_device_status[dev_idx]->
                    txagc_timing_params.wtr_late_trig_offset = -50;
      rf_cdma_data.rf_device_status[dev_idx]->
                    txagc_timing_params.pa_trig_offset = -160;
      rf_cdma_data.rf_device_status[dev_idx]->
                    txagc_timing_params.papm_early_trig_offset = -280;
      rf_cdma_data.rf_device_status[dev_idx]->
                    txagc_timing_params.papm_late_trig_offset = -20;
      // todo init rxagc data
    }

  } /*for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_INVALID_DEVICE; dev_idx++ )*/

  /* Enable SSMA operations by default */
  rf_cdma_data.debug_flag.ssma_enabled = TRUE;

  /* Enable rflm c2k */
  rf_cdma_mdsp_rflm_init();

  return ret_val;
} /* rf_cdma_init_device_status */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Destructor for the rf_cdma_data module.

  @details
  Deallocates all resources that were obtained in rf_cdma_init_device_status().
*/
void
rf_cdma_cleanup_device_status
(
  void
)
{    
  rfm_device_enum_type dev_idx; /* Variable for Device Loop */

  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    if(NULL != rf_cdma_data.rf_device_status[dev_idx])
    {
      modem_mem_free( rf_cdma_data.rf_device_status[dev_idx], MODEM_MEM_CLIENT_RFA );
      rf_cdma_data.rf_device_status[dev_idx] = NULL;
    }
  }
} /* rf_cdma_cleanup_device_status */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API initializes all freq-related info of the device to their initial
  values

  @param device 
  Device for values need to be intialized
 
*/
void
rf_cdma_init_freq_info_for_device
(
  rfm_device_enum_type device
)
{
  uint8 carrier_idx ; /* Variable for carrier indexing */

  if ( device >= RFM_MAX_DEVICES || rf_cdma_data.rf_device_status[device] == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_init_freq_info_for_device: Invalid Device %d", 
              device );
  }
  else
  {
    /* Default RF CDMA band is Invalid Max Band */
    rf_cdma_data.rf_device_status[device]->curr_band = RFM_CDMA_MAX_BAND;
    
    /* Default for RF CDMA chans is RF_CDMA_INVALID_CHAN (65536 in UINT16) */
    for( carrier_idx = 0; carrier_idx < RFM_CDMA_CARRIER_NUM; ++carrier_idx )
    {
      rf_cdma_data.rf_device_status[device]->curr_chans[carrier_idx] 
                                                         = RF_CDMA_INVALID_CHAN;
      rf_cdma_data.rf_device_status[device]->carrier_index[carrier_idx] 
                                                = RF_CDMA_INVALID_CARRIER_INDEX;
    }
  
    /* init lo_tuned_to_chan to invalid channel */
    rf_cdma_data.rf_device_status[device]->lo_tuned_to_chan 
                                                         = RF_CDMA_INVALID_CHAN;
    
    rf_cdma_data.rf_device_status[device]->rx_bw_khz = RF_CDMA_BW_INVALID;
    rf_cdma_data.rf_device_status[device]->tx_bw_khz = RF_CDMA_BW_INVALID;
  
    /* Default number of active carrier */
    rf_cdma_data.rf_device_status[device]->num_carriers 
                                                 = RF_CDMA_INVALID_NUM_CARRIERS;
  
    /* init SSMA recos for device */
    rf_cdma_init_ssma_recos( device );


  } /* if ( device > RFM_MAX_DEVICES ) */

  return;

} /* rf_cdma_init_freq_info_for_device */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable Rf Rx Recommendation Settings

  @details
  This function will return the rf_rx_reco for the given device, which is a
  pointer that can be used to update the Rf Rx Recommendation data structure.
  For read-only purpose the "un-mutable" interface for device status should
  be used.

  @param device 
  Device for which the status is queried
 
  @return
  Pointer to rf_rx_reco structure used for updates for valid devices, and 
  NULL pointer for invalid devices
*/
rfdevice_cdma_rx_reco_type*
rf_cdma_get_mutable_rf_rx_reco
(
  rfm_device_enum_type device
)
{
  /* Device Device Status for the requested device */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( &(rf_cdma_data.rf_device_status[device]->rf_rx_reco) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_rf_rx_reco: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_get_mutable_rf_rx_reco */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Read-Only RF Device Status

  @details
  This function will return the device status for the given device. This can
  be used only for read-only functionality.

  @param device 
  Device for which the status is queried
 
  @return
  Constant Pointer to Device Status Structure if device is valid, else, returns 
  a NULL pointer
*/
const rf_cdma_data_status_type*
rf_cdma_get_device_status
(
  rfm_device_enum_type device
)
{
  /* Device Device Status for the requested device */
  if ( ( ( device >= RFM_DEVICE_0 ) && ( device < RFM_MAX_DEVICES ) )
         && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( (rf_cdma_data.rf_device_status[device]) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_device_status: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_get_device_status */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF HDET data

  @details
  This function will return the HDET data for the given device, which is a
  pointer that can be used to update the HDET data data structure.For
  read-only purpose the "un-mutable" interface for device status should
  be used.

  @param device 
  Device for which the status is queried
 
  @return
  Pointer to HDET data Structure used for updates for valid devices, and 
  NULL pointer for invalid devices
*/
rf_cdma_hdet_data_type*
rf_cdma_get_mutable_hdet
(
  rfm_device_enum_type device
)
{
  /* Device Device Status for the requested device */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( &(rf_cdma_data.rf_device_status[device]->hdet) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_hdet: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_get_mutable_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF Power Limiting

  @details
  This function will return the Power Limiting for the given device, which is a
  pointer that can be used to update the Power Limiting data structure.For
  read-only purpose the "un-mutable" interface for device status should
  be used.

  @param device 
  Device for which the status is queried
 
  @return
  Pointer to Power Limiting Structure used for updates for valid devices, and 
  NULL pointer for invalid devices
*/
rf_cdma_plim_data_type*
rf_cdma_get_mutable_plim
(
  rfm_device_enum_type device
)
{
  /* Device Device Status for the requested device */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( &(rf_cdma_data.rf_device_status[device]->plim) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_plim: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_get_mutable_plim */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF Temp Comp

  @details
  This function will return the Temp Comp for the given device, which is a
  pointer that can be used to update the Temp Comp data structure.For
  read-only purpose the "un-mutable" interface for device status should
  be used.

  @param device 
  Device for which the status is queried
 
  @return
  Pointer to Temp Comp Structure used for updates for valid devices, and 
  NULL pointer for invalid devices
*/
rf_cdma_temp_comp_data_type*
rf_cdma_get_mutable_temp_comp
(
  rfm_device_enum_type device
)
{
  /* Device Device Status for the requested device */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( &(rf_cdma_data.rf_device_status[device]->temp_comp) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_temp_comp: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_get_mutable_temp_comp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF Intelliceiver

  @details
  This function will return the IntelliCeiver for the given device, which is a
  pointer that can be used to update the IntelliCeiver data structure.For
  read-only purpose the "un-mutable" interface for device status should
  be used.

  @param device 
  Device for which the status is queried
 
  @return
  Pointer to Intelliceiver Structure used for updates for valid devices, and 
  NULL pointer for invalid devices
*/
rf_cdma_ic_data_type*
rf_cdma_get_mutable_intelliceiver
(
  rfm_device_enum_type device
)
{
  /* Device Device Status for the requested device */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( &(rf_cdma_data.rf_device_status[device]->intelliceiver) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_intelliceiver: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_get_mutable_intelliceiver */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF HKADC Crit Sect

  @details
  This function will return the HKADC Crit Sect for the given device, which is a
  pointer that can be used to update the HKADC Crit Sect data structure.For
  read-only purpose the "un-mutable" interface for device status should
  be used.

  @param device 
  Device for which the status is queried
 
  @return
  Pointer to HKADC Crit Sect Structure used for updates for valid devices, and 
  NULL pointer for invalid devices
*/
rf_lock_data_type*
rf_cdma_get_mutable_hkadc_critical_sect
(
  rfm_device_enum_type device
)
{
  /* Device Device Status for the requested device */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( &(rf_cdma_data.rf_device_status[device]->hkadc_critical_sect) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_intelliceiver: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_get_mutable_hkadc_critical_sect */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Antenna Tuner Token

  @details
  This function will return the Antenna Tuner Token for the given device,
  which is a pointer that can be used to update the Antenna Tuner Token data
  structure.For read-only purpose the "un-mutable" interface for device status
  should be used.

  @param device 
  Device for which the status is queried
 
  @return
  Pointer to Intelliceiver Structure used for updates for valid devices, and 
  NULL pointer for invalid devices
*/
ant_tuner_device_tokens_type*
rf_cdma_get_mutable_ant_tuner_token
(
  rfm_device_enum_type device
)
{
  /* Device Device Status for the requested device */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( &(rf_cdma_data.rf_device_status[device]->ant_tuner_script_token_data) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_ant_tuner_token: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_get_mutable_ant_tuner_token */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF Device Status

  @details
  This function will return the device status for the given device. The
  pointer returned from this function can be used to update the RF Device
  status as well. For read-only purpose the "un-mutable" interface should
  be used.

  @param device 
  Device for which the status is queried
 
  @return
  Pointer to Device Status Structure used for updates for valid devices, and 
  NULL pointer for invalid devices
*/
rf_cdma_data_status_type*
rf_cdma_get_mutable_device_status
(
  rfm_device_enum_type device
)
{
  /* Return RF Cal Data Pointer */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return ( (rf_cdma_data.rf_device_status[device]) );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_device_status: Invalid Device %d",
              device );
    return NULL;
  }

} /* rf_cdma_data_get_device_status */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if a device has been initialized 

  @details
  This function checks to see if data structures for a given device have been
  initialized and have the expected values. This API can be used to my MC-level
  APIs to do a sanity check on the device before performing operations on the
  device
 
  @param device
  RF device for which check has to be done

  @return
  True if device is initialized, and false otherwise
*/
boolean 
rf_cdma_is_device_initialized
(
  rfm_device_enum_type device
)
{
  uint8 carrier_idx; /* Variable for carrier indexing */
  boolean chans_inited = TRUE; /* Variable to indicate if all chans have been initialized */
  boolean carriers_inited = TRUE; /* Variable to indicate if all carriers have been initialized */

  //if( device >= RFM_MAX_DEVICES || rf_cdma_data.rf_device_status[device] == NULL )
  //{
  //  RF_MSG_1( RF_ERROR, "rf_cdma_is_device_initialized: Invalid Device %d", 
  //            device );
  //  return FALSE;
  //}

  for( carrier_idx = 0; carrier_idx < RFM_CDMA_CARRIER_NUM; ++carrier_idx )
  {
    chans_inited &= 
      ( 
        (rf_cdma_data.rf_device_status[device]->curr_chans[carrier_idx]==RF_CDMA_INVALID_CHAN)
        ?  TRUE : FALSE
      );
    carriers_inited &= 
      ( 
        (rf_cdma_data.rf_device_status[device]->carrier_index[carrier_idx]==RF_CDMA_INVALID_CARRIER_INDEX)
        ?  TRUE : FALSE
      );
    /* If current carrir already not in init state, then no need to continue loop */
    if( (chans_inited == FALSE) || ( carriers_inited == FALSE ) )
    {
      break;
    }
  }

  if (
        (rf_cdma_data.rf_device_status[device]->data_1x.rf_state == RF_CDMA_STATE_SLEEP) 
        &&
        (rf_cdma_data.rf_device_status[device]->data_hdr.rf_state == RF_CDMA_STATE_SLEEP) 
        &&
        (rf_cdma_data.rf_device_status[device]->curr_band==RFM_CDMA_MAX_BAND) 
        &&
        (chans_inited==TRUE) 
        &&
        (carriers_inited==TRUE) 
        &&
        (rf_cdma_data.rf_device_status[device]->assoc_dev==RFM_INVALID_DEVICE) 
     )
  {
    return TRUE;
  }
  else
  {
    /* device has not been iniialized */
    return FALSE;
  }

} /* rf_cdma_is_device_initialized */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query to check the device has associated device
 
  @details
  For the given device, this function will query its state variable to
  check if it has any listed active device or not.
 
  @param master_device
  Master RFM device for which associated device is to be checked
 
  @return
  Flag indicating if the given device has the associated device is listed
  or not.
*/
boolean
rf_cdma_has_associated_device
(
  rfm_device_enum_type device
)
{
  boolean has_assoc_dev = FALSE;

  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    if ( rf_cdma_data.rf_device_status[device]->assoc_dev != RFM_INVALID_DEVICE )
    {
      has_assoc_dev = TRUE;
    } /* if ( rf_cdma_data.rf_device_status[device].assoc_dev != 
         RFM_INVALID_DEVICE ) */
    else
    {
      has_assoc_dev = FALSE;
    } /* if ! ( rf_cdma_data.rf_device_status[device].assoc_dev != 
         RFM_INVALID_DEVICE ) */
  } /* if ( device <= RFM_MAX_DEVICES ) */
  else
  {
    has_assoc_dev = FALSE;

    RF_MSG_1( RF_ERROR, "rf_cdma_has_associated_device: Invalid device %d",
              device );
  }

  RF_MSG_2( RF_HIGH, "rf_cdma_has_associated_device: Device %d, "
                      "has_assoc_dev = %d ", device, has_assoc_dev );

  return has_assoc_dev;
  
} /* rf_cdma_has_associated_device */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get CDMA Lock Data

  @details
  This function will return the CDMA LOCK data.

  @return
  Writeable Pointer to CDMA Lock Data
*/
rf_lock_data_type*
rf_cdma_get_lock_data
(
  void
)
{

  /* throw error message if NULL pointer */
  if ( rf_cdma_data.crit_section == 0 )
  {
    RF_MSG( RF_ERROR,
    "rf_cdma_get_lock_data: Null pointer !! Critical Section not initialized !!");
  }

  return rf_cdma_data.crit_section;
} /* rf_cdma_get_lock_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set CDMA Lock Data

  @details
  This function will set the CDMA LOCK data.

  @param lock_data
  Lock data to be set as CDMA Lock
*/
void
rf_cdma_set_lock_data
(
  rf_lock_data_type *lock_data
)
{
  rf_cdma_data.crit_section = lock_data;
} /* rf_cdma_get_lock_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  update the device data structure with the rotator offsets

  @param device
  RF device for which updates need to be done
 
  @param freq_offsets_hz
  Array that contains the offsets (in Hz)
 
  @return
  TRUE on success and FALSE on failure 
*/
boolean
rf_cdma_data_update_rot_offsets
(
  rfm_device_enum_type device,
  int32 freq_offsets_hz[]
)
{
  boolean res;

  /* ensure valid device */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    if ( rf_cdma_data.debug_flag.ssma_enabled == TRUE )
    {
       int carr_idx; /* carrier index */
      
       /* update the freq offsets */
       for (carr_idx = 0; carr_idx < RFM_CDMA_CARRIER_NUM; carr_idx++)
       {
         rf_cdma_data.rf_device_status[device]->rotator_offsets_in_hz[carr_idx] = 
                                                        freq_offsets_hz[carr_idx];
       }
    } /* if ( rf_cdma_data.debug_flag.ssma_enabled == TRUE ) */

    res = TRUE;
  }
  else
  {
     RF_MSG_1( RF_ERROR, "rf_cdma_data_update_rot_offsets: Invalid Device %d", 
               device );
     res = FALSE;
  }

  /* return result */
  return res;

} /* rf_cdma_update_rot_offsets */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set rf_mode for rf_cdma_data

  @details
  This function will set rf_mode for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param rf_mode
  rf_mode to be set
  
*/
void
rf_cdma_data_set_rf_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
)
{
  if ( device >= RFM_MAX_DEVICES || rf_cdma_data.rf_device_status[device] == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rf_mode: Invalid Device %d", 
              device );
  }
  else if( rf_mode != RFM_PARKED_MODE && 
           rf_mode != RFM_1X_MODE && 
           rf_mode != RFM_1XEVDO_MODE)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rf_mode: Invalid RF Mode %d", 
              rf_mode );
  }
  else
  {
    rf_cdma_data.rf_device_status[device]->rf_mode = rf_mode;
  }
} /* rf_cdma_data_set_rf_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set 1x rf_state for rf_cdma_data

  @details
  This function will set 1x rf_state for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param rf_state
  rf_state to be set
  
*/
void
rf_cdma_data_set_1x_rf_state
(
  rfm_device_enum_type device,
  rf_cdma_state_type rf_state
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->data_1x.rf_state = rf_state;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_1x_rf_state: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_1x_rf_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set hdr rf_state for rf_cdma_data

  @details
  This function will set hdr rf_state for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param rf_state
  rf_state to be set
  
*/
void
rf_cdma_data_set_hdr_rf_state
(
  rfm_device_enum_type device,
  rf_cdma_state_type rf_state
)
{
   
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->data_hdr.rf_state = rf_state;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_hdr_rf_state: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_hdr_rf_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get 1x rf_state from rf_cdma_data

  @details
  This function will get 1x rf_state from rf_cdma_data.

  @param device
  RF device for which rf_state is needed.
 
  @return rf_state
  rf_state that currently the device is in.
  
*/
rf_cdma_state_type
rf_cdma_data_get_1x_rf_state
(
  rfm_device_enum_type device
)
{

  rf_cdma_state_type rf_state = RF_CDMA_STATE_INVALID ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_state = rf_cdma_data.rf_device_status[device]->data_1x.rf_state ;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_1x_rf_state: Invalid Device %d", 
              device );
  }
  return rf_state ;
} /* rf_cdma_data_get_1x_rf_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get hdr rf_state from rf_cdma_data

  @details
  This function will get hdr rf_state from rf_cdma_data.

  @param device
  RF device for which rf_state is needed.
 
  @return rf_state
  rf_state that currently the device is in.
  
*/
rf_cdma_state_type
rf_cdma_data_get_hdr_rf_state
(
  rfm_device_enum_type device
)
{
  rf_cdma_state_type rf_state = RF_CDMA_STATE_INVALID ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_state = rf_cdma_data.rf_device_status[device]->data_hdr.rf_state ;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_hdr_rf_state: Invalid Device %d", 
              device );
  }
  return rf_state ;
} /* rf_cdma_data_get_hdr_rf_state */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rf_state from rf_cdma_data for the given mode and device

  @details
  This function will get rf_state from rf_cdma_data.

  @param device
  RF device for which rf_state is needed.

  @param device
  RF Mode for which rf_state is needed.
 
  @return rf_state
  rf_state that currently the device is in.
  
*/
rf_cdma_state_type
rf_cdma_data_get_rf_state
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
)
{
  rf_cdma_state_type rf_state = RF_CDMA_STATE_INVALID ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    if( rf_mode == RFM_1X_MODE )
    {
      rf_state = rf_cdma_data.rf_device_status[device]->data_1x.rf_state ;
    }
	else if( rf_mode == RFM_1XEVDO_MODE )
	{
	  rf_state = rf_cdma_data.rf_device_status[device]->data_hdr.rf_state ;
	}
	else
	{
	  RF_MSG_2( RF_ERROR, "rf_cdma_data_get_hdr_rf_state: Invalid mode %d of device %d", 
                rf_mode,device );
	}
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_hdr_rf_state: Invalid Device %d", 
              device );
  }
  return rf_state ;
} /* rf_cdma_data_get_hdr_rf_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set curr_band for rf_cdma_data

  @details
  This function will set curr_band for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param curr_band
  curr_band to be set
  
*/
void
rf_cdma_data_set_curr_band
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type curr_band
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->curr_band = curr_band;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_curr_band: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_curr_band */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get curr_band from rf_cdma_data

  @details
  This function will get curr_band from rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @return curr_band
  curr_band that is set 
  
*/
rfm_cdma_band_class_type
rf_cdma_data_get_curr_band
(
  rfm_device_enum_type device
)
{
  rfm_cdma_band_class_type curr_band = RFM_CDMA_MAX_BAND ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    curr_band = rf_cdma_data.rf_device_status[device]->curr_band ;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_curr_band: Invalid Device %d", 
              device );
  }
  return curr_band ;
} /* rf_cdma_data_set_curr_band */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set curr_chans for rf_cdma_data

  @details
  This function will set curr_chans for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param carrier
  Carrier number targeted
 
  @param curr_chans
  curr_chans to be set
  
*/
void
rf_cdma_data_set_curr_chan
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  uint16 curr_chans
)
{
  if ( device >= RFM_MAX_DEVICES || rf_cdma_data.rf_device_status[device] == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_curr_chan: Invalid Device %d", 
              device );
  }
  else if ( carrier >= RFM_CDMA_CARRIER_NUM )  
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_curr_chan: Invalid Carrier %d", 
              carrier );
  }
  else
  {  
    rf_cdma_data.rf_device_status[device]->curr_chans[carrier] = curr_chans;
  }
} /* rf_cdma_data_set_curr_chan */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set carrier_index for rf_cdma_data

  @details
  This function will set carrier_index for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param carrier
  Carrier number targeted
 
  @param carrier_index
  carrier_index to be set
  
*/
void
rf_cdma_data_set_carrier_index
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  uint16 carrier_index
)
{
  if ( device >= RFM_MAX_DEVICES || rf_cdma_data.rf_device_status[device] == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_carrier_index: Invalid Device %d", 
              device );
  }
  else if ( carrier >= RFM_CDMA_CARRIER_NUM )  
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_carrier_index: Invalid Carrier %d", 
              carrier );
  }else
  {  
    rf_cdma_data.rf_device_status[device]->carrier_index[carrier] = carrier_index;
  }
} /* rf_cdma_data_set_carrier_index */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set rx_bw_khz for rf_cdma_data

  @details
  This function will set rx_bw_khz for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param rx_bw_khz
  rx_bw_khz to be set
  
*/
void
rf_cdma_data_set_rx_bw_khz
(
  rfm_device_enum_type device,
  uint32 rx_bw_khz
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->rx_bw_khz = rx_bw_khz;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rx_bw_khz: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_rx_bw_khz */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rx_bw_khz from rf_cdma_data

  @details
  This function will get rx_bw_khz for rf_cdma_data if
  previously set.

  @param device
  RF device for which updates need to be done

  @return rx_bw_khz
  rx_bw_khz that was set earlier.
 
*/
uint32
rf_cdma_data_get_rx_bw_khz
(
  rfm_device_enum_type device
)
{
  uint32 rx_bw_khz = 0 ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rx_bw_khz = rf_cdma_data.rf_device_status[device]->rx_bw_khz;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_rx_bw_khz: Invalid Device %d", 
              device );     
  }
  return rx_bw_khz;
} /* rf_cdma_data_get_rx_bw_khz */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set tx_bw_khz for rf_cdma_data

  @details
  This function will set tx_bw_khz for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param tx_bw_khz
  tx_bw_khz to be set
  
*/
void
rf_cdma_data_set_tx_bw_khz
(
  rfm_device_enum_type device,
  uint32 tx_bw_khz
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->tx_bw_khz = tx_bw_khz;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_tx_bw_khz: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_tx_bw_khz */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set num_carriers for rf_cdma_data

  @details
  This function will set num_carriers for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param tx_bw_khz
  num_carriers to be set
  
*/
void
rf_cdma_data_set_num_carriers
(
  rfm_device_enum_type device,
  uint32 num_carriers
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->num_carriers = (uint8) num_carriers;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_num_carriers: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_num_carriers */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set lo_tuned_to_chan for rf_cdma_data

  @details
  This function will set lo_tuned_to_chan for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param tx_bw_khz
  lo_tuned_to_chan to be set
  
*/
void
rf_cdma_data_set_lo_tuned_to_chan
(
  rfm_device_enum_type device,
  uint16 lo_tuned_to_chan
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->lo_tuned_to_chan = lo_tuned_to_chan;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_lo_tuned_to_chan: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_lo_tuned_to_chan */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set rotator_offsets_in_hz for rf_cdma_data

  @details
  This function will set rotator_offsets_in_hz for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param carrier
  Carrier number targeted
 
  @param carrier_index
  rotator_offsets_in_hz to be set
  
*/
void
rf_cdma_data_set_rotator_offsets_in_hz
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  int32 rotator_offsets_in_hz
)
{
  if ( device >= RFM_MAX_DEVICES || rf_cdma_data.rf_device_status[device] == NULL )
  {
    RF_MSG_1( RF_ERROR, 
              "rf_cdma_data_set_rotator_offsets_in_hz: Invalid Device %d", 
              device );
  }
  else if ( carrier >= RFM_CDMA_CARRIER_NUM )  
  {
    RF_MSG_1( RF_ERROR, 
              "rf_cdma_data_set_rotator_offsets_in_hz: Invalid Carrier %d", 
              carrier );
  }
  else
  {  
    rf_cdma_data.rf_device_status[device]->rotator_offsets_in_hz[carrier] = 
                                                          rotator_offsets_in_hz;
  }
} /* rf_cdma_data_set_rotator_offsets_in_hz */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set assoc_dev for rf_cdma_data

  @details
  This function will set assoc_dev for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param tx_bw_khz
  assoc_dev to be set
  
*/
void
rf_cdma_data_set_assoc_dev
(
  rfm_device_enum_type device,
  rfm_device_enum_type assoc_dev
)
{
  /*
     the arg: device should be a valid one
     arg: assoc_dev (or slave device) may be invalid - as in, RFM_MAX_DEVICES. 
 
     For instance, in cases where diversity is not enabled, a device 
     would not have any associated device - i.e., RFM_MAX_DEVICES 
 
     However, assoc_dev may not be greater than RFM_MAX_DEVICES 
 */
  if ( device >= RFM_MAX_DEVICES || rf_cdma_data.rf_device_status[device] == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_assoc_dev: Invalid Device %d", 
              device );
  }
  else if ( assoc_dev > RFM_INVALID_DEVICE || assoc_dev == RFM_MAX_DEVICES 
            || assoc_dev == RFM_DEVICE_GPS )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_assoc_dev: Invalid Associated Device %d", 
              assoc_dev );
  }
  else
  {
    rf_cdma_data.rf_device_status[device]->assoc_dev = assoc_dev;
  }
} /* rf_cdma_data_set_assoc_dev */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set rxlm_handle for rf_cdma_data

  @details
  This function will set rxlm_handle for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param rxlm_handle
  rxlm_handle to be set
  
*/
void
rf_cdma_data_set_rxlm_handle
(
  rfm_device_enum_type device,
  lm_handle_type rxlm_handle
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->rxlm_handle = rxlm_handle;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rxlm_handle: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_rxlm_handle */



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rxlm_handle from rf_cdma_data

  @details
  This function will get rxlm_handle from rf_cdma_data.

  @param device
  RF device for which RxLm handle needs to be obtained. 
 
  @return rxlm_handle
  The rxlm_handle for the given device.
  
*/
lm_handle_type
rf_cdma_data_get_rxlm_handle
(
  rfm_device_enum_type device
)
{
  lm_handle_type rxlm_handle = (lm_handle_type)~0 ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rxlm_handle = rf_cdma_data.rf_device_status[device]->rxlm_handle ;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rxlm_handle: Invalid Device %d", 
              device );
  }
  return rxlm_handle ;
} /* rf_cdma_data_get_rxlm_handle */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set fbrx_lm_handle for rf_cdma_data

  @details
  This function will set fbrx_lm_handle for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param fbrx_lm_handle
  fbrx_lm_handle to be set
  
*/
void
rf_cdma_data_set_fbrx_lm_handle
(
  rfm_device_enum_type device,
  lm_handle_type fbrx_lm_handle
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->fbrx_lm_handle = fbrx_lm_handle;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_fbrx_lm_handle: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_fbrx_lm_handle */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set fbrx_enable for rf_cdma_data

  @details
  This function will set fbrx_enable to TRUE / FALSE based on NV .

  @rflm_tech_id_t
  Rflm_tech for which the FBRx support needs to be read from NV

  @return fbrx_lm_handle
  The fbrx_lm_handle for the given device.
  
*/
void
rf_cdma_data_set_fbrx_enable
(
  rfm_mode_enum_type mode
)
{
  rflm_tech_id_t rflm_tech = rfcommon_fbrx_mc_convert_rfm_mode_to_rflm_tech(mode);
  
  rfcommon_fbrx_control_enum_type fbrx_ctrl_val = rfcommon_fbrx_nv_get_control_val(rflm_tech);

  if ( fbrx_ctrl_val == RFCOMMON_FBRX_DISABLE_NO_UPDATES || fbrx_ctrl_val == RFCOMMON_FBRX_INVALID  )
  {
	if ( mode == RFM_1X_MODE )
	{
	  rf_cdma_data.fbrx_en.fbrx_enabled_1x = FALSE;
	}
	else if ( mode == RFM_1XEVDO_MODE )
	{
	  rf_cdma_data.fbrx_en.fbrx_enabled_hdr = FALSE;
	}
  }
  else
  {
	if ( mode == RFM_1X_MODE )
	{
	  rf_cdma_data.fbrx_en.fbrx_enabled_1x = TRUE;
	}
	else if ( mode == RFM_1XEVDO_MODE )
	{
	  rf_cdma_data.fbrx_en.fbrx_enabled_hdr = TRUE;
	}
  }

} /* rf_cdma_data_set_fbrx_enable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get fbrx_enable from rf_cdma_data

  @details
  This function will get fbrx_enable from rf_cdma_data .

  @rfm_mode
  CDMA 1x/Hdr

  @return True/False
  FBRx enable or disable status
  
*/
boolean
rf_cdma_data_get_fbrx_enable
(
  rfm_mode_enum_type mode
)
{
  if ( mode == RFM_1X_MODE )
  {
	return rf_cdma_data.fbrx_en.fbrx_enabled_1x;
  }
  else if ( mode == RFM_1XEVDO_MODE)
  {
	return rf_cdma_data.fbrx_en.fbrx_enabled_hdr;
  }
  else
  {
	RF_MSG_1( RF_ERROR, "rf_cdma_data_get_fbrx_enable: Invalid Mode %d", 
				   mode );
	return FALSE;

  }
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get fbrx_lm_handle from rf_cdma_data

  @details
  This function will get fbrx_lm_handle from rf_cdma_data.

  @param device
  RF device for which FBRx Lm handle needs to be obtained. 
 
  @return fbrx_lm_handle
  The fbrx_lm_handle for the given device.
  
*/
lm_handle_type
rf_cdma_data_get_fbrx_lm_handle
(
  rfm_device_enum_type device
)
{
  lm_handle_type fbrx_lm_handle = (lm_handle_type)~0 ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    fbrx_lm_handle = rf_cdma_data.rf_device_status[device]->fbrx_lm_handle ;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_fbrx_lm_handle: Invalid Device %d", 
              device );
  }
  return fbrx_lm_handle ;
} /* rf_cdma_data_get_fbrx_lm_handle */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the flag to skip the pout_comp

  @details
  This function will set the flag to skip the pout_comp to rf_cdma_data.

  @param device
  RF device for which the pout compensation skip_or_not flag needs to be set.
 
  @return skip_or_not
  The pout compensation skip_or_not for the given device.
*/ 
boolean
rf_cdma_set_pout_comp_skip
(
  rfm_device_enum_type device ,
  boolean              skip_or_not 
)
{
  boolean success = TRUE ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->skip_pout_comp = skip_or_not ;
  }
  else
  {
    success = FALSE ;
    RF_MSG_1( RF_ERROR, "rf_cdma_set_pout_comp_skip: Invalid Device %d", 
              device );
  }
  return success ;
} /* rf_cdma_set_pout_comp_skip */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the flag to skip the pout_comp

  @details
  This function will set the flag to skip the pout_comp to rf_cdma_data.

  @param device
  RF device for which the pout compensation skip_or_not flag needs to be set.
 
  @return skip_or_not
  The pout compensation skip_or_not for the given device.
*/ 
boolean
rf_cdma_get_pout_comp_skip
(
  rfm_device_enum_type device 
)
{
  boolean pout_skip_or_not_flag = FALSE ;
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    pout_skip_or_not_flag = rf_cdma_data.rf_device_status[device]->skip_pout_comp  ;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_pout_comp_skip: Invalid Device %d", 
              device );
  }
  return pout_skip_or_not_flag ;
} /* rf_cdma_get_pout_comp_skip */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set txlm_handle for rf_cdma_data

  @details
  This function will set txlm_handle for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param txlm_handle
  txlm_handle to be set
  
*/
void
rf_cdma_data_set_txlm_handle
(
  rfm_device_enum_type device,
  lm_handle_type txlm_handle
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->txlm_handle = txlm_handle;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_txlm_handle: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_txlm_handle */

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Get txlm_handle from rf_cdma_data

  @details
  This function will get txlm_handle from rf_cdma_data.

  @param device
  RF device for which txlm_handle needs to obtained
 
  @return txlm_handle
  The txlm_handle for the given device
  
*/

lm_handle_type
rf_cdma_data_get_txlm_handle
(
  rfm_device_enum_type device
)
{
  lm_handle_type txlm_handle = (lm_handle_type) ~0 ; 
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    txlm_handle = rf_cdma_data.rf_device_status[device]->txlm_handle ;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_txlm_handle: Invalid Device %d", 
              device );
  }
  return txlm_handle ;
} /* rf_cdma_data_get_txlm_handle */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set hdr_nominal_pwr for rf_cdma_data

  @details
  This function will set hdr_nominal_pwr for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param hdr_nominal_pwr_dB256
  hdr_nominal_pwr to be set. For details refer to data_hdr in rf_cdma_data_status_type
  
*/

void
rf_cdma_data_set_hdr_nominal_pwr
(
  rfm_device_enum_type device,
  int16 hdr_nominal_pwr_dB256 
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->data_hdr.hdr_nominal_pwr_dB256 = 
                                                          hdr_nominal_pwr_dB256;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_hdr_nominal_pwr: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_hdr_nominal_pwr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set hdr rri for rf_cdma_data

  @details
  This function will set hdr rri for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param rri
  rri to be set. For details refer to data_hdr in rf_cdma_data_status_type.
  
*/

void
rf_cdma_data_set_hdr_rri
(
  rfm_device_enum_type device,
  rf_hdr_tx_agc_rri_type rri
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->data_hdr.rri = rri;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_hdr_rri_type: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_hdr_rri_type */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set is_voice_call for rf_cdma_data

  @details
  This function will set is_voice_call for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param is_voice_call
  is_voice_call to be set. For details refer to data_1x in 
  rf_cdma_data_status_type. 
  
*/

void
rf_cdma_data_set_is_voice_call
(
  rfm_device_enum_type device,
  boolean is_voice_call
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->data_1x.is_voice_call = is_voice_call;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_is_voice_call: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_is_voice_call */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set 1x_sch_rate for rf_cdma_data

  @details
  This function will set 1x_sch_rate for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param 1x_sch_rate
  1x_sch_rate to be set. For details refer to data_hdr in 
  rf_cdma_data_status_type
  
*/

void
rf_cdma_data_set_1x_sch_rate
(
  rfm_device_enum_type device,
  rf_1x_tx_agc_sch_rate_type sch_rate
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->data_1x.sch_rate = sch_rate;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_1x_sch_rate: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_1x_sch_rate */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set hkadc_critical_sect for rf_cdma_data

  @details
  This function will set hkadc_critical_sect for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param hkadc_critical_sect
  hkadc_critical_sect to be set
  
*/
void
rf_cdma_data_set_hkadc_critical_sect
(
  rfm_device_enum_type device,
  rf_lock_data_type hkadc_critical_sect
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {

    rf_cdma_data.rf_device_status[device]->hkadc_critical_sect = hkadc_critical_sect;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_hkadc_critical_sect: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_hkadc_critical_sect */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set script_index for rf_cdma_data

  @details
  This function will set script_index for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param script_index
  script_index to be set
  
*/
void
rf_cdma_data_set_script_index
(
  rfm_device_enum_type device,
  uint32 script_index
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->script_index = script_index;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_script_index: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_script_index */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set traffic_state for rf_cdma_data

  @details
  This function will set traffic_state for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param traffic_state
  traffic_state to be set
  
*/
void
rf_cdma_data_set_traffic_state
(
  rfm_device_enum_type device,
  boolean traffic_state
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->traffic_state = traffic_state;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_traffic_state: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_traffic_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set isHKADCbusy for rf_cdma_data

  @details
  This function will set isHKADCbusy for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @param isHKADCbusy
  isHKADCbusy to be set
  
*/
void
rf_cdma_data_set_isHKADCbusy
(
  rfm_device_enum_type device,
  boolean isHKADCbusy
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->isHKADCbusy = isHKADCbusy;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_isHKADCbusy: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_isHKADCbusy */


/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_pa_q_current_enabled
(
   rfm_device_enum_type device,
   boolean enable_pa_q_current
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->apt_pa_q_current_enabled = 
                                                            enable_pa_q_current;
  } 
  else
  {
    RF_MSG_1(RF_ERROR, "rf_cdma_data_set_pa_q_current_enabled: Invalid Device %d",
             device);
  }
} /* rf_cdma_data_set_pa_q_current_enabled */

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_ept_online_enabled
(
   rfm_device_enum_type device,
   boolean enable_ept_online
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->ept_online_enabled = enable_ept_online;
  } 
  else
  {
    RF_MSG_1(RF_ERROR, "rf_cdma_data_set_pa_ept_online_enabled: Invalid Device %d",
             device);
  }
} /* rf_cdma_data_set_ept_online_enabled */

/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_ept_iq_gain_val
(
   rfm_device_enum_type device,
   int16 ept_iq_gain
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->ept_iq_gain_val = ept_iq_gain;
  } 
  else
  {
    RF_MSG_1(RF_ERROR, "rf_cdma_data_set_ept_iq_gain_val: Invalid Device %d",
             device);
  }
} /* rf_cdma_data_set_ept_iq_gain_val */


/*----------------------------------------------------------------------------*/
void
rf_cdma_data_set_cdma_iq_gain_val
(
   rfm_device_enum_type device,
   int16 cdma_iq_gain
)
{
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    rf_cdma_data.rf_device_status[device]->cdma_iq_gain_val = cdma_iq_gain;
  } 
  else
  {
    RF_MSG_1(RF_ERROR, "rf_cdma_data_set_cdma_iq_gain_val: Invalid Device %d",
             device);
  }
} /* rf_cdma_data_set_cdma_iq_gain_val */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get apt_pa_q_current_enabled status for rf_cdma_data

  @details
  This function will return apt_pa_q_current_enabled for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @return
  TRUE for PA Q current enabled. FALSE for PA Q Current disabled.
 
*/
boolean
rf_cdma_data_get_pa_q_current_enabled
(
   rfm_device_enum_type device
)
{
  boolean result = FALSE;

  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    result = rf_cdma_data.rf_device_status[device]->apt_pa_q_current_enabled ;
  } 
  else
  {
    RF_MSG_1(RF_ERROR, "rf_cdma_data_get_pa_q_current_enabled: Invalid Device %d",
             device);
  }

  return result;
}/* rf_cdma_data_get_pa_q_current_enabled */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rf_mode for rf_cdma_data

  @details
  This function will return rf_mode for rf_cdma_data.

  @param device
  RF device for which updates need to be done
 
  @return
  RF mode.
 
*/
rfm_mode_enum_type
rf_cdma_data_get_rf_mode
(
  rfm_device_enum_type device
)
{
  rfm_mode_enum_type rf_mode = RFM_INVALID_MODE ;

  if ( device >= RFM_MAX_DEVICES || rf_cdma_data.rf_device_status[device] == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rf_mode: Invalid Device %d", 
              device );
  }
  else
  {
    rf_mode = rf_cdma_data.rf_device_status[device]->rf_mode;
  }

  return rf_mode;
} /* rf_cdma_data_get_rf_mode */

/*! @} */

/*============================================================================*/
/*!
  @name Calibration

  @brief
  This section includes interfaces pertaining to RF Calibration Data
  structure.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Read-Only RF Calibration Data

  @details
  This function will return the RF Calibration Data structure. This can
  be used only for read-only functionality.

  @return
  Constant Pointer to RF Calibration Data Structure for valid devices, else, 
  returns a NULL pointer
*/
const rf_cdma_data_calibration_type *
rf_cdma_get_cal_data
(
  rfm_device_enum_type device
)
{

  /* Return RF Cal Data Pointer */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return &(rf_cdma_data.rf_device_status[device]->cal_data);
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_cal_data: Invalid Device %d",
              device );
    return NULL;
  }
} /* rf_cdma_get_cal_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF Calibration Data

  @details
  This function will return the RF Calibration Data. The
  pointer returned from this function can be used to update the RF Calibration
  data as well. For read-only purpose the "un-mutable" interface should
  be used.

  @return
  Pointer to RF Calibration Data Structure used for updates - for valid devices,
  and NULL pointer for invalid devices
*/
rf_cdma_data_calibration_type *
rf_cdma_get_mutable_cal_data
(
  rfm_device_enum_type device
)
{
  /* Return RF Cal Data Pointer */
  if ( device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL )
  {
    return &(rf_cdma_data.rf_device_status[device]->cal_data);
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_mutable_cal_data: Invalid Device %d",
              device );
    return NULL;
}
} /* rf_cdma_get_mutable_cal_data */

/*! @} */

/*============================================================================*/
/*!
  @name MDSP

  @brief
  This section includes interfaces pertaining to CDMA MDSP Data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF CDMA MDSP Data

  @details
  This function will return the RF CDMA MDSP Data. The
  pointer returned from this function can be used to update the RF MDSP
  data as well.

  @return
  Pointer to RF CDMA MDSP Data Structure used for updates
*/
rf_cdma_mdsp_data_type *
rf_cdma_get_mdsp_data
(
  void
)
{

  /* Return RF iC Task Data Pointer */
  return &(rf_cdma_data.mdsp);

} /* rf_cdma_get_mdsp_data */

/*! @} */

/*============================================================================*/
/*!
  @name Task

  @brief
  This section includes interfaces pertaining to intelliceiver and RF Task
  Data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF CDMA intelliceiver Task Data

  @details
  This function will return the RF CDMA intelliceiver Task Data. The
  pointer returned from this function can be used to update the RF iC Task
  data as well.

  @return
  Pointer to RF CDMA intelliceiver Task Data Structure used for updates
*/
rf_cdma_ic_task_data_type *
rf_cdma_get_ic_task_data
(
  void
)
{

  /* Return RF iC Task Data Pointer */
  return &(rf_cdma_data.ic_task);

} /* rf_cdma_get_ic_task_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF 1x Fw response Data

  @details
  This function will return the RF 1x FW response Data. The
  pointer returned from this function can be used to update the
  data as well.

  @return
  Pointer to RF 1x firmware Data Structure used for updates
*/
rf_1x_fw_response_data_type *
rf_cdma_get_1x_fw_response_data
(
  void
)
{

  /* Return RF Task Data Pointer */
  return &(rf_cdma_data.fw_response.onex_fw_rsp);

} /* rf_cdma_get_1x_fw_response_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF HDR Fw response Data

  @details
  This function will return the RF HDR FW response Data. The
  pointer returned from this function can be used to update the
  data as well.

  @return
  Pointer to RF HDR firmware Data Structure used for updates
*/
rf_hdr_fw_response_data_type *
rf_cdma_get_hdr_fw_response_data
(
  void
)
{

  /* Return RF Task Data Pointer */
  return &(rf_cdma_data.fw_response.hdr_fw_rsp);

} /* rf_cdma_get_hdr_fw_response_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF CDMA FW response Data

  @details
  This function will return the RF CDMA FW response Data. The
  pointer returned from this function can be used to update the
  data as well.

  @return
  Pointer to RF CDMA firmware Data Structure used for updates
*/
rf_cdma_fwrsp_data_type *
rf_cdma_get_fwrsp_data
(
   rfm_device_enum_type device
)
{

  /* Return RF Cal Data Pointer */
  if ( device < RFM_MAX_DEVICES && 
       rf_cdma_data.rf_device_status[device] != NULL )
  {
    return &(rf_cdma_data.rf_device_status[device]->fwrsp_data);
  } /* if ( device < RFM_MAX_DEVICES && 
       rf_cdma_data.rf_device_status[device] != NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_fwrsp_data: Invalid Device %d", device );
    return NULL;
  } /* if ( device < RFM_MAX_DEVICES && 
       rf_cdma_data.rf_device_status[device] != NULL ) */
} /* rf_cdma_get_fwrsp_data */

/*! @} */

/*============================================================================*/
/*!
  @name EPT

  @brief
  This section includes interfaces pertaining to EPT Data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF CDMA EPT Data

  @details
  This function will return the RF EPT Data. The
  pointer returned from this function can be used to update the
  data as well.

  @return
  Pointer to CDMA EPT Data Structure used for updates
*/
rf_cdma_ept_data_type *
rf_cdma_get_ept_data
(
  void
)
{

  /* Return RF Task Data Pointer */
  return &(rf_cdma_data.ept);

} /* rf_cdma_get_ept_data */

/*! @} */

/*============================================================================*/
/*!
  @name Debug

  @brief
  This section includes interfaces pertaining to Debug Data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get RF debug flag Data

  @details
  This function will return the RF debug flag Data.

  @return
  Pointer to RF CDMA debug flag Data Structure used for updates
*/
const rf_cdma_debug_flag_type *
rf_cdma_get_debug_flag_data
(
  void
)
{
  /* Return RF Debug Flag Data Pointer */
  return &(rf_cdma_data.debug_flag);
} /* rf_cdma_get_debug_flag_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return a mutable pointer of type rf_cdma_debug_flag_type

*/
rf_cdma_debug_flag_type *
rf_cdma_get_mutable_debug_flag_data
(
  void
)
{
  /* Return RF Debug Flag Data Pointer */
  return &(rf_cdma_data.debug_flag);
} /* rf_cdma_set_debug_flag_data */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return pointer to RF event handle for designated device and event type
 
  @param device
  RF Device for which RF event handle is to be returned
 
  @param event_type
  RF event type for which RF event handle is to be returned

  @return
  RF event handle for designated device and event type
*/
rfcommon_mdsp_event_handle ** 
rf_cdma_data_get_event_handle
(
  rfm_device_enum_type device,
  rf_cdma_event_type event_type
)
{
  if ( device >= RFM_MAX_DEVICES )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_event_handle: "
                        "Invalid Dev %d", device );
    return NULL;
  }
  if ( event_type >= RF_CDMA_EVENT_INVALID )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_event_handle: "
                        "Invalid CCS event type %d", event_type );
    return NULL;
  }

  return &(rf_cdma_data.rf_device_status[device]
                                         ->event_handle_ptr[event_type]);

}/* rf_cdma_data_get_event_handle */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the enable/disable status of the Rx AGC logging based on the device
  provided.
 
  @param device
  RF Device for which Rx AGC logging must be enabled
 
  @param enable
  Flag to indicate enable/disable status

  @return
  Returns whether the Rx AGC logging debug mode was updated successfully
*/
boolean
rf_cdma_data_set_rxagc_log_mode
(
  rfm_device_enum_type device,
  boolean enable
)
{
  boolean ret_val = TRUE;

  /* Update the Rx AGC logging flag based on device */
  if ( device < RFM_MAX_DEVICES )
  {
    rf_cdma_data.debug_flag.rx_agc_logging[device] = enable;
    ret_val = TRUE;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rxagc_log_mode: "
                        "Invalid Dev %d",
              device );
    ret_val = FALSE;
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the RF HAL bus vote handle per device
 
  @param device
  RF Device for which handle belongs
 
  @param handle
  Pointer to RF HAL bus vote handle
 
  @return
  Returns whether the handle was updated successfully
*/
boolean
rf_cdma_data_set_rf_hal_bus_vote_handle
(
  rfm_device_enum_type device,
  rf_hal_bus_client_handle* handle
)
{
  boolean ret_val = TRUE;
  
  /* Update the handle based on device */
  if ( device < RFM_MAX_DEVICES )
  {
    rf_cdma_data.rf_device_status[device]->rf_hal_bus_vote_handle = handle;
    ret_val = TRUE;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rf_hal_bus_vote_handle: "
                        "Invalid input: device=%d",
              device );
    ret_val = FALSE;
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the RF HAL bus vote handle per device
 
  @param device
  RF Device for which handle belongs
 
  @param handle
  Pointer to RF HAL bus vote handle

  @return
  The handle given the device
*/
rf_hal_bus_client_handle*
rf_cdma_data_get_rf_hal_bus_vote_handle
(
  rfm_device_enum_type device
)
{
  rf_hal_bus_client_handle* ret_val = NULL;

  if ( device < RFM_MAX_DEVICES )
  {
    ret_val = rf_cdma_data.rf_device_status[device]->rf_hal_bus_vote_handle;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_rf_hal_bus_vote_handle: "
                        "Invalid input: device=%d",
              device );
  }

  return ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize iRat specific info when entering iRat for a given device
 
  @param device
  RF Device for which handle belongs
 
  @param buffer_index
  Index to the buffer that has all the SSBI, RFFE etc device writes.

  @param rxlm_handle
  rxlm_handle to be set

  @return success
  True if the API is successful False otherwise
*/
boolean
rf_cdma_init_irat 
(
  rfm_device_enum_type            device ,
  uint32                          buffer_index ,
  lm_handle_type                  rxlm_handle 
)
{
  boolean success = FALSE ;

  if ( device < RFM_MAX_DEVICES )
  {
    rf_cdma_irat_info_type* irat_data = 
      &(rf_cdma_data.rf_device_status[device]->irat_data) ;

    irat_data->irat_buffer_index =  buffer_index ;
    irat_data->irat_rxlm_handle =   rxlm_handle ;
    success =  TRUE ;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_init_irat: "
                        "Invalid input: device=%d",
                        device );
  }
  return success ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize iRat specific info when entering iRat for a given device
 
  @param device
  RF Device for which handle belongs
 
  @return success
  True if the API is successful False otherwise
*/
boolean
rf_cdma_deinit_irat 
(
  rfm_device_enum_type            device
)
{
  boolean success = FALSE ;

  if ( device < RFM_MAX_DEVICES )
  {
    rf_cdma_irat_info_type* irat_data = 
      &(rf_cdma_data.rf_device_status[device]->irat_data) ;

    /* Invalidate buffer index and LM handle*/
    irat_data->irat_buffer_index = -1;
    irat_data->irat_rxlm_handle = RXLM_MAX_BUFFERS;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_deinit_irat: "
                        "Invalid input: device=%d",
                        device );
  }
  return success ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return pointer to the TxAGC timing params for both 1x and HDR. Currently
  stored in CDMA data, will move to NV or RFC eventually
 
  @param device
  RF Device for which timing informaiton is requested
 
  @return
  Pointer to the data structure holding the timing params
*/
rfcommon_core_txlin_agc_timing_params*
rf_cdma_data_get_txagc_timing_params
(
  rfm_device_enum_type device
)
{
  if ( device >= RFM_MAX_DEVICES )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_get_txagc_timing_params: "
                        "Invalid Dev %d", device );
    return NULL;
}

  return &(rf_cdma_data.rf_device_status[device]->txagc_timing_params);

}/* rf_cdma_data_get_txagc_timing_params */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize signature arrays for cdma data sanity check
 
  @details
  This function will initialize the signature arrays for the given device
 
  @param device
  The device that will initialize the signature arrays
   
  @return void
*/
boolean
rf_cdma_data_sanity_check_init
(
  rfm_device_enum_type device
)
{
  boolean status= TRUE;
  /* Check for valid device */
  if ( device < RFM_MAX_DEVICES )
  {
    uint8 i;
    for ( i=0; i<RF_CDMA_SANITY_SIGNATURE_SIZE; i++ )
    {
      rf_cdma_data.rf_device_status[device]->signature1[i] = 0xAAAAAAAA; 
      rf_cdma_data.rf_device_status[device]->signature2[i] = 0xAAAAAAAA; 
      rf_cdma_data.rf_device_status[device]->signature3[i] = 0xAAAAAAAA; 
      rf_cdma_data.rf_device_status[device]->signature4[i] = 0xAAAAAAAA; 
      rf_cdma_data.rf_device_status[device]->signature5[i] = 0xAAAAAAAA; 
    }
  }/* if ( device < RFM_MAX_DEVICES ) */
  else
  {
    RF_MSG_1( RF_ERROR," rf_cdma_data_sanity_check_init: Invalid device: %d ",
              device);
    status= FALSE;
  }

  return status;
}/*rf_cdma_data_sanity_check_init*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Do CDMA data sanity check for signature array for given device
 
  @details
  This function will perform sanity check for the signature array for the
  given device. If checking fail, err_fatal will crash the phone.
 
  @param device
  The device that will initialize the signature arrays
   
  @return void
*/
void
rf_cdma_data_sanity_check
(
  rfm_device_enum_type device
)
{
  /* Check for valid device */
  if ( device < RFM_MAX_DEVICES )
  {
    uint8 i;
    for ( i=0; i<RF_CDMA_SANITY_SIGNATURE_SIZE; i++ )
    {
      if ( (rf_cdma_data.rf_device_status[device]->signature1[i] != 0xAAAAAAAA) ||
           (rf_cdma_data.rf_device_status[device]->signature2[i] != 0xAAAAAAAA) || 
           (rf_cdma_data.rf_device_status[device]->signature3[i] != 0xAAAAAAAA) ||
           (rf_cdma_data.rf_device_status[device]->signature4[i] != 0xAAAAAAAA) || 
           (rf_cdma_data.rf_device_status[device]->signature5[i] != 0xAAAAAAAA))
      {
        RF_MSG_1( RF_MED, "rf_cdma_data_sanity_check: failed for device = %d", 
                  device);
        ERR_FATAL( "memory corruption", 0, 0, 0 );
      }
    }
  }/*  if ( device < RFM_MAX_DEVICES ) */
  else
  {
    RF_MSG_1( RF_ERROR," rf_cdma_data_sanity_check_init: Invalid device: %d ",
              device);
  }

  return;
}/*rf_cdma_data_sanity_check*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @details
 
  @param device
  The device that will initialize the signature arrays
   
  @return void
*/
void
rf_cdma_data_set_rxagc_cell_id
(
  rfm_device_enum_type device,
  cell_id_t cell_id
)
{
  if (device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL)
  {
    rf_cdma_data.rf_device_status[device]->rxagc_data.cell_id = cell_id;
  } 
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rxagc_cell_id: Invalid Device %d", 
              device );
  }
} /* rf_cdma_data_set_rxagc_cell_id */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  
 
  @details
 
 
  @param device
  The device that will initialize the signature arrays
   
  @return
*/
cell_id_t
rf_cdma_data_get_rxagc_cell_id
(
  rfm_device_enum_type device
)
{
  cell_id_t cell_id = RFLM_CMN_RXAGC_INVALID_CELL_ID;

  if (device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL)
  {
    cell_id = rf_cdma_data.rf_device_status[device]->rxagc_data.cell_id;
  } 

  return cell_id;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Save the pointer to common RxAGC tech data pointer 
  @details
 
  @param device
  The device for which ptr needs to be set
 
  @param tech_data_ptr
  void pointer to the tech data
 
  @return void
*/
void
rf_cdma_data_set_rxagc_tech_data_ptr
(
  rfm_device_enum_type device,
  void* tech_data_ptr
)
{
  if (device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL)
  {
    rf_cdma_data.rf_device_status[device]->rxagc_data.rxagc_tech_data =
                                                              tech_data_ptr;
  } 
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_data_set_rxagc_tech_data_ptr: "
                        "Invalid Device %d", device );
  }
} /* rf_cdma_data_set_rxagc_tech_data_ptr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the pointer to common RxAGC tech data pointer
 
  @details
 
  @param device
  The device for which ptr is queried
 
  @return void pointer to the tech data
*/
void*
rf_cdma_data_get_rxagc_tech_data_ptr
(
  rfm_device_enum_type device
)
{
  void* tech_data_ptr = NULL;

  if (device < RFM_MAX_DEVICES && rf_cdma_data.rf_device_status[device] != NULL)
  {
    tech_data_ptr = 
              rf_cdma_data.rf_device_status[device]->rxagc_data.rxagc_tech_data;
  } 

  return tech_data_ptr;

}/* rf_cdma_data_get_rxagc_tech_data_ptr*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will set the dlna_support data of the device in the CDMA data structure
 
  @details
  This function will set the dlna_support data of the device in the CDMA data structure
 
  @param device
  The dlna_support of the device to be set
   
  @return void
*/

boolean
rf_cdma_data_set_dlna_support 
(
  rfm_device_enum_type device
)
{
  rfm_cdma_band_class_type band;  
  const rfc_cdma_logical_device_params_type *logical_dev;
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  logical_dev = rfc_cdma_get_logical_device_params( device );
    /* Get Read-only and Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if( (dev_status_w == NULL) || (logical_dev == NULL) )
  {
    RF_MSG(RF_ERROR, "rf_cdma_data_set_dlna_support: NULL pointer");
    return FALSE;
  }

  dev_status_w->is_dlna_supported = FALSE; 

  /* Check if DLNA is supported for this device */
  for ( band = RFM_CDMA_BC0;
        band < RFM_CDMA_MAX_BAND;
        band++ )
  {
    /* If DLNA is supported for any band, it is enabled */
    if (rfdevice_cdma_is_dlna_supported(logical_dev->rx_device[band]) == TRUE)
    {
      dev_status_w->is_dlna_supported= TRUE;
      break;
    }
  }
  
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the first available RFM device that's Tx active according
  to rf_cdma_data.
  !!! Warning: This API is a HACK. It is written to make up for certain module's
  bad design. Avoid using this API as much as possible. Device should always
  come from external input. !!!
   
  @return device
  RFM device that is currently active.

*/
rfm_device_enum_type 
rf_cdma_data_get_curr_tx_device
(
  void
)
{
  rfm_device_enum_type ret_device = RFM_INVALID_DEVICE;
  rfm_device_enum_type rfm_device_id;
  const rf_cdma_data_status_type* dev_status_r;

  /*----------------------------------------------------------------------------
     Loop through all devices and update eligible devices
  ----------------------------------------------------------------------------*/
  for ( rfm_device_id = RFM_DEVICE_0; rfm_device_id < RFM_MAX_WAN_DEVICES; 
        rfm_device_id++  )
  {
    /* Get writable rf_cdma_data pointer*/
    dev_status_r = rf_cdma_get_device_status( rfm_device_id );

    if( dev_status_r == NULL)
    {
      continue;
    } /* NULL pointer, skip*/

    else if ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX || 
              dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX )
    {
      ret_device = rfm_device_id;
      break;
    }
  } /* for ( rfm_device_id = RFM_DEVICE_0; ... */

  RF_MSG_1(RF_MED, "rf_cdma_data_get_curr_tx_device: Best match %d", 
                    rfm_device_id );
  return ret_device;
} /* rf_cdma_data_get_curr_tx_device */


#endif /* FEATURE_CDMA1X */

/*! @} */

/*! @} */
