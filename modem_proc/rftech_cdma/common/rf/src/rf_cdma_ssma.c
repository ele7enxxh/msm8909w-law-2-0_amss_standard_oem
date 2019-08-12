/*! 
  @file
  rf_cdma_ssma.c
 
  @brief
  This file defines the common CDMA (1x and HDR) interfaces needed for to 
  suppress spurs caused due to SSMA operation and due to deterministic spurs as
  specified in the NV

  @addtogroup RF_CDMA_COMMON_SSMA
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_ssma.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/06/14   spa     Change rx_nv_path to be per band per logical device
06/19/14   spa     Rx tune interface update
04/21/13   fh      Added mutex lock and release for dynamic update
08/26/13   shb     Updates for new C++ transceiver device interface
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
08/30/13   vr      Fix to set nof carriers correctly while configuring notches
06/20/13   fh      Increase Notch fitler BW to 1.8MHz
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/17/13   sty     Reduced message level to Med, udpated comments
05/14/13   sty     Fix buffer overrun in rf_cdma_get_rx_reco()
04/30/13   sty     Call rf_cdma_mdsp_ccs_write_immediate() to retune Rx to new
                   ssma channel
04/10/13   spa     Updated implementation to update notch filter settings 
03/19/13   APU     Replaced memcpy with memscpy.
03/06/13   bm      Updated the current band and channel info used to retrieve
                   spur info to be suppressed
03/05/13   bm      Updated the LO tuned chan variable passed to 
                   rf_cdma_msm_conv_recos_to_reg_vals()
12/27/12   shb     Updates for different physical rx_device per band
12/18/12   sty     KW fix to prevent array overrun
10/31/12   sty     Warning fix
10/31/12   sty     Temp fix for compile
09/10/12   spa     Removed references to v2 device scripting APIs
09/10/12   spa     Added WTR scripting in rf_cdma_ssma_helper_to_tune_device
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/28/12   adk     Do SSBI writes generated in rf_cdma_ssma_helper_to_tune_device
08/21/12   cd      Added case in rf_cdma_get_ssma_recos to handle no support 
                   for secondary chain
08/17/12   spa     Fixed incorrect usage of RFM_MAX_DEVICES 
08/08/12   adk     Added support for scripting buffer interface.
06/15/12   adk     Corrected BB spur mitigation for WTR1605
04/30/12   zhaow   Updated rf_cdma_init_ssma_recos() 
04/18/12   aro     Added callback function as parameter to override functions
04/16/12   sty     Doxygen updates 
04/09/12   sty     get and release device-locks in MC-layer
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
03/29/12   sty     Changed rf_cdma_get_ssma_recos() to only do SSMA operations 
                   and not account for spurs from NV. This is now done in 
                   rf_cdma_get_spurs_from_nv() when MC-layer calls it explicitly
                   Added new API rf_cdma_get_spurs_to_suppress()
                   Changed args to rf_cdma_get_ssma_recos()
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/20/12   sty     Removed checks in rf_cdma_get_notches_to_suppress() which 
                   were preventing notch config when SSMA returned no recos
03/01/12   sty     Print out debug messages only when debug_msgs_enabled is TRUE
                   Added rf_cdma_enable_ssma_debug_msgs()
02/27/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
02/24/12   sty     Fixed KW error 
02/24/12   sty     Fixed Lo chan to have correct value in 
                   rf_cdma_perform_ssma_operations
02/23/12   sty     Changed notch depth variable
02/22/12   sty     Continue with rf_cdma_perform_ssma_operations() only when the 
                   device is valid
02/17/12   sty     Changes for SSMA in DORB mode
02/10/12   sty     Fixed bug in rf_cdma_get_notches_to_suppress - was not acting
                   on all spurs read from NV 
02/08/12   shb     Added argument in CDMA device rx tune api to distinguish 
                   between MEAS script generation and standalone wakeup
02/06/12   sty     Added rf_cdma_update_notch_settings()
02/06/12   sty     Added device check in rf_cdma_update_bb_ssma_recos() 
01/20/12   sty     Added support for dynmic updates of notches in 
                   rf_cdma_update_bb_ssma_recos()
                   Added rf_cdma_update_ssma_notch_recos()
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
01/12/12   sty     Assign valid value to lo_chan to take care of cases where RF 
                   device does not support SSMA
01/10/12   Saul    MCDO. RTR now taking EVDO mode. 
                   Allow operation in multi-carrier mode and bandwidths.
01/05/12   sty     Added rf_cdma_update_ssma_flag() to dynamically disable/enable
                   SSMA feature
01/05/12   sty     Changes in rf_cdma_update_rf_ssma_recos to:
                   [1] do a mem_free()
                   [2] do not tune Rx Lo to new channel if it is same as the old one
01/05/12   sty     fixed F3 and return values
01/05/12   sty     deleted redundant header file
01/05/12   sty     [1] Added rf_cdma_update_rf_ssma_recos()
                   [2] Changed name from rf_cdma_update_ssma_recos to 
                   rf_cdma_update_bb_ssma_recos
                   [3] use rf_state to determine if device is active
01/04/12   aro     Updated code to check for Tx Device to determine if
                   a device is primary device or not
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
12/23/11   sty     Added rf_cdma_update_ssma_recos()
12/21/11   sty     Deleted dummy rf_cdma_apply_rx_reco()
                   var name changes  
                   Renamed rf_cdma_msm_update_notch_flt_settings() to 
                   rf_cdma_msm_conv_recos_to_reg_vals()
12/20/11   sty     update notch filter settings only if RXLM handle is valid
12/20/11   sty     Invoke rf_cdma_msm_update_notch_flt_settings() to update 
                   Notch filter settings
                   Commented out rf_cdma_update_rf_bb_recos() - may need it later
12/19/11   sty     Fixed KW error 
                   Made rf_cdma_update_rf_bb_recos() static
12/15/11   sty     [1] Do not need to update device_status with the recos in 
                   rf_cdma_update_rf_bb_recos() - this was redundant
                   [2] var name changes in rf_cdma_init_ssma_recos()
                   [3] udpated arg-list in rf_cdma_get_rx_reco()
                   [4] added new API - rf_cdma_perform_ssma_operations()
12/14/11   sty     Added more checks in rf_cdma_get_rx_reco()
                   Above API has a cleaner interface - will return 2 pointers
                   to MC-layer with recos for the 2 chains
12/13/11   sty     Changed var name from rfdevice_1x_rx_recommendation_type to 
                   rfdevice_cdma_rx_recommendation_type
12/10/11   sty     Renamed rf_cdma_init_ssma_ptr to rf_cdma_init_ssma_struct
                   Refactored rf_cdma_apply_rx_baseband_reco to 
                   rf_cdma_update_rf_bb_recos 
12/06/11   sty     Added rf_cdma_init_ssma_ptr() - may need to call this API in
                   cases where recos need to be nullified dynamically
11/30/11   sty     Check for null pointers
11/16/11   sty     Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfcommon_msg.h"
#include "rf_cdma_ssma.h"
#include "rf_cdma_data.h"
#include "rfc_cdma.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "modem_mem.h"
#include "rf_1x_mdsp.h"
#include "rf_hdr_mdsp.h"
#include "rfcommon_msm.h"
#include "rf_cdma_constants.h"

/* memscpy */
#include "stringl.h"

/*----------------------------------------------------------------------------*/
/*!
  @name SSMA-related functions

  @brief
  This section contains all functions pertaining to SSMA
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Helper function to retune the RF device
 
  @param logical_dev
  Holds RFC info needed for operation of the RF Device
 
  @param rf_mode
  used to check if the device 1x or 1xEVDO mode
 
  @param band
  RF band to which the device is to be tuned to 
 
  @param chan
  RF band to which the device is to be tuned to
 
  @param rx_bw_khz
  Receive BW in KHz
 
  @param tx_bw_khz
  Tramsmit BW in KHz
 
  @param power_mode
  The I-ceiver power mode in which the RF device is to be tuned to (high-lin or 
  low-lin)
 
  @param script_mode
  used to check if tuning is for IRAT or normal mode

  @return
  TRUE on success and FALSE on failure
   
*/
static 
boolean 
rf_cdma_ssma_helper_to_tune_device
(
  const rfc_cdma_logical_device_params_type* logical_dev,
  const rfm_mode_enum_type rf_mode,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type chan,
  const uint32 script_index,
  const uint32 rx_bw_khz,
  const uint32 tx_bw_khz,
  const rfm_cdma_power_mode_type power_mode,
  const rfdevice_cdma_script_operation_type script_mode
)
{
  boolean api_result; /* api result */
  rf_buffer_intf *rx_settings_buffer;

  /* Allocate memory for RTR script container */
  rx_settings_buffer = rf_buffer_create( RFDEVICE_MAX_SSBI_SCRIPT_SIZE, 0,0);

  if( rx_settings_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ssma_helper_to_tune_device: "
                      "rf_buffer_create() failed");
    return FALSE;
  }

  /* Enter Device Critical Section, so that the device call is thread safe */
  rfdevice_cdma_rx_enter_critical_section(logical_dev->rx_device[band]);

  api_result = rfdevice_cdma_create_rx_tune_script( 
                                  logical_dev->rx_device[band], rf_mode, 
                                  band, chan, rx_bw_khz, tx_bw_khz, power_mode, 
                                  rx_settings_buffer, script_mode, NULL ,NULL );

  if ( api_result == TRUE )
  {
    api_result = rf_cdma_mdsp_ccs_write_immediate ( rx_settings_buffer, 
                                                RF_HAL_BUS_RESOURCE_SSBI, 
                                                RFDEVICE_MAX_SSBI_SCRIPT_SIZE );
  }
  else
  {
    RF_MSG (RF_ERROR, "rf_cdma_ssma_helper_to_tune_device: failed to"
                      "retrieve tune script from device " );
  }

  /* Leave Device Critical Section */
  rfdevice_cdma_rx_leave_critical_section(logical_dev->rx_device[band]);

  /* De-allocate memory for RTR script container */
  rf_buffer_destroy( rx_settings_buffer );

  if (api_result == FALSE)
  {
    RF_MSG(RF_ERROR, "rf_cdma_ssma_helper_to_tune_device: failed in api ");
  }
  else
  {
    RF_MSG_3 ( RF_LOW, "rf_cdma_ssma_helper_to_tune_device: Tuned to new "
                   "channel %d, and Band %d for mode %d", chan, band, rf_mode );
  }

  return api_result;

} /* rf_cdma_ssma_helper_to_tune_device */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Read the Spur table from NV, and populate spurs_to_notch_tbl_ptr with all 
  freqs that need to be notched out
    
  @param device  
  RF device on which operation is to be performed 
    
  @param spurs_to_notch_tbl_ptr  
  Points to the structure that contains all freqs that cause spurs in the  
  current opeating band and freq 
    
  @return  
  TRUE on success and FALSE on failure 
*/
static boolean
rf_cdma_get_spurs_from_nv
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type rf_band,
  rfm_cdma_chan_type rf_chan,
  rfcommon_spur_table_type* spurs_to_notch_tbl_ptr
)
{
  uint32 rx_lo_freq_hz; /* Freq in Hz to which Rx LO is tuned to */
  boolean ret_val; /* final result of API */

  /* list of all spurs that have been specified in the NV */
  const rfcommon_spur_table_type *master_tbl_frm_nv;

  /* need a readable ptr to device status */
  const rf_cdma_data_status_type* dev_status =rf_cdma_get_device_status(device);

  /* Get Logical Device Param */
  const rfc_cdma_logical_device_params_type* logical_dev = 
                                   rfc_cdma_get_logical_device_params( device );
  
  /* NULL Pointer Check */
  if ( logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_spurs_from_nv: Invalid logical device" 
                        " for device %d", device );
    return FALSE;
  }

  if ( dev_status == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_spurs_from_nv: Invalid device status" 
                        " pointer for device %d", device );
    return FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* all checks passed */
  /*--------------------------------------------------------------------------*/
  if ( dev_status->rf_mode == RFM_1X_MODE )
  {
    const rf_1x_config_type* onex_config_ptr; /* Pointer to NV data */

    /* Get NV for the requested device and band and check if it is NULL */
    onex_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->rx_nv_path[rf_band],
                                              rf_band );

    if ( onex_config_ptr == NULL )
    {
      /* no NV for the requested device and band - bail out */
      RF_MSG_3( RF_ERROR, "rf_cdma_get_spurs_from_nv: NULL NV for device %d" 
                " on NV path %d for band = %d", device, 
                logical_dev->rx_nv_path[rf_band], rf_band );
      return FALSE;
    }

    master_tbl_frm_nv = &onex_config_ptr->rx_static.onex_spur_table;
  }
  else
  {
    const rf_hdr_config_type* hdr_config_ptr; /* Pointer to NV data */

    /* Get NV for the requested device and band and check if it is NULL */
    hdr_config_ptr = rf_cdma_nv_get_hdr_data( logical_dev->rx_nv_path[rf_band],
                                              rf_band );

    if ( hdr_config_ptr == NULL )
    {
      /* no NV for the requested device and band - bail out */
      RF_MSG_3( RF_ERROR, "rf_cdma_get_spurs_from_nv: NULL NV for device %d" 
                " on NV path %d for band = %d", device, 
                logical_dev->rx_nv_path[rf_band], rf_band );
      return FALSE;
    }

    master_tbl_frm_nv = &hdr_config_ptr->rx_static.hdr_spur_table;

  } /* if ( dev_status_w->rf_mode != RFM_1X_MODE ) */
  
  ret_val = TRUE; /* start with good value */
  /*--------------------------------------------------------------------------*/
  /* Read NV to get all spurs that need to be suppresed                       */
  /*--------------------------------------------------------------------------*/
  rx_lo_freq_hz = 1000*rf_cdma_get_rx_carrier_freq( rf_band, rf_chan);

  /* Read the Spur table from NV and retrieve the list of spurs to be notched 
     out */ 
  ret_val &= rfcommon_msm_get_spurs_to_suppress( rx_lo_freq_hz, 
                                       RF_CDMA_NOTCH_FILTER_BW_HZ,
                                       master_tbl_frm_nv,
                                       RFCOMMON_MAX_SPUR_ENTRIES,
                                       spurs_to_notch_tbl_ptr,
                                       RF_MAX_NUM_NOTCH_FILTERS_AVAIL );

  return ret_val;

} /* rf_cdma_get_spurs_from_nv */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Retrieves all spurs that need to be suppressed 
    
  @details 
  When this API is called, the dyn_notch_settings potentially has a list of 
  spurs that need to be suppressed due to SSMA.  
    
  In addition, there may be spurs that are deterministic in nature in the  
  current (band, chan), that need to be suppressed. These spurs are present in 
  the NV. This API reads the NV (RFNV_CDMA_Cx_[1X/HDR]_SPUR_TABLE_I), and gets a 
  list of spurs that need to be dealt with. The number of notch filters for a  
  target is determined by RF_MAX_NUM_NOTCH_FILTERS_AVAIL.  
    
  If the spurs from SSMA and NV exceed RF_MAX_NUM_NOTCH_FILTERS_AVAIL,  
  then the SSMA spurs are given priority 
   
  @param dyn_notch_settings  
  Points to the structure that contains the dynamic settings. The notch filter  
  settings will be populated by this API
    
  @return  
  TRUE on success and FALSE on failure 
*/
boolean
rf_cdma_get_spurs_to_suppress
(
  rfm_device_enum_type device,
  lm_handle_type rxlm_handle,
  rfm_cdma_band_class_type rf_band,
  rfm_cdma_chan_type rf_chan,
  uint8 num_active_carriers,
  rf_cdma_msm_dynamic_rxlm_cfg_type *const dyn_settings
)
{

  boolean ret_val; /* final result of API */

  /* holds all spurs read from NV that need to be notched out */
  rfcommon_spur_table_type spurs_to_notch_tbl;

  /* need a readable ptr to device status */
  rf_cdma_data_status_type* dev_status = 
                                    rf_cdma_get_mutable_device_status( device );

  /* NULL pointer check */
  if ( dev_status == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_notches_to_suppress: NULL data for device" 
            "%d", device );
    ret_val = FALSE;
  }
  else if ( num_active_carriers > RF_MAX_NUM_CARRIERS_PER_PATH )
  {
    RF_MSG_1( RF_ERROR, 
              "rf_cdma_get_notches_to_suppress: invalid num carriers %d",
              num_active_carriers );
    ret_val = FALSE;
  }
  else
  {  
    int carr_idx;  /* iterator for carriers */

    /*------------------------------------------------------------------------*/
    /* Read NV to get all spurs that need to be suppressed                    */
    /*------------------------------------------------------------------------*/
    /* start with all zeros */
    memset(&spurs_to_notch_tbl, 0, sizeof(rfcommon_spur_table_type));
    
    /* populate the spurs_to_notch_tbl with the spurs read from NV that need to
       be suppressed */
    ret_val = rf_cdma_get_spurs_from_nv( device, rf_band, 
                                         rf_chan, &spurs_to_notch_tbl );      


    /*----------------------------------------------------------------------------
     Now configure the notch filters of each active carrier to suppress the above    
    ----------------------------------------------------------------------------*/

    /* Append spur data read from NV to rf_rx_reco.bb.per_carrier_notch_reco[carr_idx] */

    for ( carr_idx = 0; carr_idx < num_active_carriers; carr_idx++ )
    {    
      int notch_entry; /* counter used to populate the per-carrier-recos */
    
      /* keeps track of no. of notches from NV that were added to notch list */
      uint8 notch_from_nv; 
    
      /* counter to run thru the list of spurs that were read from NV */
      int list_ctr;
    
      /* Freq in Hz to which Rx LO is tuned to */
      uint32 lo_tuned_to_freq_hz = 
                           1000*rf_cdma_get_rx_carrier_freq( rf_band, rf_chan );

      /* pointer to per-carrier-BB-reco */
      rfdevice_per_carrier_notch_reco_type* per_carr_bb_recos_ptr = 
                    &(dev_status->rf_rx_reco.bb.per_carrier_notch_reco[carr_idx]);
    
      if ( per_carr_bb_recos_ptr->filter_count >= RF_MAX_NUM_NOTCH_FILTERS_AVAIL )
      {
        RF_MSG_2( RF_ERROR, "rf_cdma_retrieve_spurs_from_nv: BB recos already "
                  "has %d notches for Carrier-%d - more spurs cannot be "
                  "suppressed ", RF_MAX_NUM_NOTCH_FILTERS_AVAIL, 
                  carr_idx );
        continue;
      }
    
      /* now, spurs_to_notch_tbl has all the spurs listed in NV for the current 
       Rx LO freq that need to be notched out */
      list_ctr = 0; /* init counters */
      notch_from_nv = 0;
    
      for ( notch_entry = per_carr_bb_recos_ptr->filter_count; 
            notch_entry < RF_MAX_NUM_NOTCH_FILTERS_AVAIL; 
            notch_entry++, list_ctr++ )
      {         
        if ( spurs_to_notch_tbl.rfcommon_spur_tbl[list_ctr].spur_at_freq_hz == 0 )
        {
          /* this means that there are no more spurs to be supressed */
          break;
        }
      
        if ( spurs_to_notch_tbl.rfcommon_spur_tbl[list_ctr].spur_at_freq_hz == 
             lo_tuned_to_freq_hz )
        {
            /* the spur is right on DC - no need to supress this as DC cancellation
               should take care of this spur */
          continue;
        }
      
        per_carr_bb_recos_ptr->has_valid_info = TRUE;
    
        /* get the Spur location w.r.t Rx LO freq */   
        per_carr_bb_recos_ptr->notch_filter[notch_entry].spur_offset = 
          ( lo_tuned_to_freq_hz - 
            spurs_to_notch_tbl.rfcommon_spur_tbl[list_ctr].spur_at_freq_hz );
      
        /* K0 is read from NV */
        per_carr_bb_recos_ptr->notch_filter[notch_entry].notch_depth[0] = 
                       spurs_to_notch_tbl.rfcommon_spur_tbl[list_ctr].notch_depth;
      
        /* get the target-specific params for the notch (K1 and K2) */
        rf_cdma_msm_get_params_for_nv_notches(
                             &(per_carr_bb_recos_ptr->notch_filter[notch_entry]));
      
        notch_from_nv++;
      
      } /* for ( notch_entry = RF_MAX_NUM_NOTCH_FILTERS_AVAIL-1; ... */
    
      per_carr_bb_recos_ptr->filter_count += notch_from_nv;
    
      if ( dev_status->rf_rx_reco.debug_msgs_enabled )
      {
        /* total Spurs to be suppresed = Spurs due to SSMA +
                                         Spurs read from NV */
        RF_MSG_3( RF_LOW, "rf_cdma_retrieve_spurs_from_nv: For Carrier-%d: "
                  "Notches due to Spur table in NV = %d, Total Notches = %d",
                  carr_idx, notch_from_nv, per_carr_bb_recos_ptr->filter_count );
      }

    } /* for (carr_idx=0; carr_idx<num_active_carriers; carr_idx++) */

    /* Convert all BB+NV recos into register settings. Store all reg settings
      in dyn_settings->notch_filt_settings[carr_idx], with carr_idx = 0.
      This is because notch filters for all carriers must be sent to FW at once.
    */

    for ( carr_idx = 0; carr_idx < num_active_carriers; carr_idx++ )
    {    
      /* convert the SSMA BB recos to register settings for current device */
      ret_val &= rf_cdma_msm_conv_recos_to_reg_vals ( 
                      rxlm_handle,
                      rf_band,
                      rf_chan,
                      dev_status->rf_rx_reco.rf.lo_chan,
                      num_active_carriers,
                      dev_status->rf_rx_reco.bb.per_carrier_notch_reco[carr_idx],
                      &(dyn_settings->notch_filt_settings[0 /*carr_idx*/]));

    } /* for (carr_idx=0; carr_idx<num_active_carriers; carr_idx++) */


    /* All notch filter reg settings are collected in dyn_settings->notch_filt_settings[0].
     * Copy them to other carriers, so that FW can use this data assoc with another
     * carr_idx, if necessary.
     */
    for ( carr_idx = 0; carr_idx < num_active_carriers; carr_idx++ )
    {    
      if( carr_idx > 0 )
      {
         size_t data_length = sizeof(rf_cdma_notch_flt_settings_type) ; 
         memscpy( &(dyn_settings->notch_filt_settings[carr_idx]) , data_length ,
                  &(dyn_settings->notch_filt_settings[0]) , data_length ) ;
      }
    } /* for (carr_idx=0; carr_idx<num_active_carriers; carr_idx++) */

  } /* if !( dev_status == NULL ) */

  return ret_val;

} /* rf_cdma_get_notches_to_suppress */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API is called when the SSMA data structure needs to be initialized to  
  its default values. The baseband and RF recos are initialized to their  
  defaults
   
  @param device  
  Device that needs to be initialized
    
  @return  
  TRUE on success and FALSE on failure 
*/
boolean
rf_cdma_init_ssma_recos
(
  rfm_device_enum_type device
)
{
  boolean result; /* final result of API */
  int nf_idx; /* iterator to loop thru notch filters */
  int nf_state; /* iterator to loop thru Notch filter states */
  rfdevice_cdma_rx_reco_type * ssma_ptr; /* Pointer to SSMA structure*/

  ssma_ptr = rf_cdma_get_mutable_rf_rx_reco( device );

  if ( NULL != ssma_ptr )
  {
    int carr_idx; /* carrier index */

    /* init all RF params */
    ssma_ptr->rf.has_reco = FALSE;
    ssma_ptr->rf.lo_chan = RF_CDMA_INVALID_CHAN;
    ssma_ptr->rf.lo_offset_KHz = 0;
    
    /* init all baseband params */
    ssma_ptr->bb.has_reco = FALSE; 
    ssma_ptr->debug_msgs_enabled = FALSE; /* disable printing debug messages */

    for ( carr_idx=0; carr_idx < RF_MAX_NUM_CARRIERS_PER_PATH; carr_idx++ )
    {
      /* temp pointer to per carrier Notch recos */
      rfdevice_per_carrier_notch_reco_type *notch_reco_per_carrier
         = &(ssma_ptr->bb.per_carrier_notch_reco[carr_idx]);
  
      notch_reco_per_carrier->carrier_chan = RF_CDMA_INVALID_CHAN;
      notch_reco_per_carrier->has_valid_info = FALSE;
      notch_reco_per_carrier->filter_count = 0;
      
      /* init all notch filters ... "nf_idx" is Notch Filter Index and "nf_state" 
         is Notch Filter State */
      for ( nf_idx=0; nf_idx < RF_MAX_NUM_NOTCH_FILTERS_AVAIL; nf_idx++ )
      {
        notch_reco_per_carrier->notch_filter[nf_idx].spur_offset = 0;     
        notch_reco_per_carrier->notch_filter[nf_idx].rise_fall_hysteresis_dB10=0;
        
        /* zero-out notch depths for this carrier */
        for (nf_state=0; nf_state < RF_MAX_NUM_NOTCH_FILTER_STATES; nf_state++)
      {
          notch_reco_per_carrier->notch_filter[nf_idx].notch_depth[nf_state] = 0;
      }

        /* zero-out the rise thresholds for this carrier */
        for ( nf_state=0; nf_state < RF_MAX_NUM_NOTCH_FILTER_STATES-1; nf_state++ )
      {       
          notch_reco_per_carrier->notch_filter[nf_idx].rise_threshold[nf_state]=0; 
      }

    } /* init all notch filters */

    } /* for ( carr_idx=0; carr_idx=RF_MAX_NUM_CARRIERS_PER_PATH; carr_idx++ ) */
    result = TRUE;
  } /* if ( NULL != ssma_ptr ) */
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_init_ssma_recos: Null Pointer passed!! ");
    result = FALSE;
  }

  return result;

} /* rf_cdma_init_ssma_recos */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  This function applies the RF part of the SSMA recommendations for the given  
  device
    
  @details  
  This involves retuning the RX LO of the device to the new channel as 
  determined by the SSMA algorithms. Note that the RX LO tuning is done only  
  for the paired (SHDR) device.  
    
  Use case:  
  Device-0 is tuned to Band_A / Chan_a
  Device-1 is tuned to Band_B / Chan_b  
    
  In this combination, assume that no SSMA recos were needed. Now, Device-0 does  
  a retune to Band_C / Chan_c - so the new combo is:  
    
  Device-0 is tuned to Band_C / Chan_c
  Device-1 is tuned to Band_B / Chan_b  
    
  Assume that for this combo, the device-1 channel needs to be tuned to  
  Chan_b1  
    
  This API achieves the above requirement. 
       
  @param device
  The paired device on which the SSMA operation is to be performed
   
*/
boolean
rf_cdma_update_rf_ssma_recos
(
  rfm_device_enum_type device
)
{

  boolean ret_val; /* api result */

  /* Device Status Write Pointer for device - need a writable ptr to
     update the LO Chan variable */
  rf_cdma_data_status_type* dev_status = 
                                      rf_cdma_get_mutable_device_status(device);

  /* get debug flag info */
  const rf_cdma_debug_flag_type* debug_flags = rf_cdma_get_debug_flag_data();

  if ( debug_flags->ssma_enabled == FALSE )
  {
    RF_MSG(RF_MED, "rf_cdma_update_rf_ssma_recos: SSMA feature not enabled ");
    return TRUE; 
  }

  if (dev_status == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_update_rf_ssma_recos: failed to get device "
                        "status for device %d ", device );
    return FALSE;
  }

  /* check to see if device is active */
  if ( (dev_status->data_1x.rf_state == RF_CDMA_STATE_RX) 
       || 
       (dev_status->data_1x.rf_state == RF_CDMA_STATE_RXTX)
       ||
       (dev_status->data_hdr.rf_state == RF_CDMA_STATE_RX) 
       || 
       (dev_status->data_hdr.rf_state == RF_CDMA_STATE_RXTX) )
  {
    const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

    if ( dev_status->rf_rx_reco.debug_msgs_enabled )
    {
      RF_MSG_1( RF_LOW, "rf_cdma_update_rf_ssma_recos: Dynamic RF updates for "
                        "device %d ", device );
    }

    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );

    /* NULL Pointer Check */
    if ( logical_dev == NULL )
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_update_rf_ssma_recos: Invalid logical device" 
                " for device %d", device );
      return FALSE;
    }

    if (dev_status->lo_tuned_to_chan == dev_status->rf_rx_reco.rf.lo_chan )
    {
      /* no need to retune to the same RxLO channel */
      ret_val = TRUE;
    }
    else
    {
      /* The actual band for tune */
      rfm_cdma_band_class_type band = dev_status->curr_band;
      /*The actual channel for tune*/
      rfm_cdma_chan_type chan = dev_status->rf_rx_reco.rf.lo_chan;

      ret_val = rf_cdma_ssma_helper_to_tune_device ( logical_dev,
                  RFM_1X_MODE, band, chan, dev_status->script_index,
                  dev_status->rx_bw_khz, dev_status->tx_bw_khz,
                  dev_status->intelliceiver.power_mode,
                  RFDEVICE_CDMA_STANDALONE_SCRIPT );

      /* update the new RX LO channel */
      if (ret_val == TRUE)
      {
        RF_MSG_2 ( RF_LOW, "rf_cdma_update_rf_ssma_recos: Tuned from %d to %d", 
                 dev_status->lo_tuned_to_chan, dev_status->rf_rx_reco.rf.lo_chan);  
    
        /* update dev status with new LO channel */
        dev_status->lo_tuned_to_chan = dev_status->rf_rx_reco.rf.lo_chan;
      }
      else
      {
        RF_MSG ( RF_ERROR, "rf_cdma_update_rf_ssma_recos: Failed to tune to new"
                           "channel !!!" );
      }
    } /*if (dev_status->lo_tuned_to_chan == dev_status->rf_rx_reco.rf.lo_chan)*/

  } /* if ( (dev_status->rf_state == RF_CDMA_STATE_RX) ||  ... */
  else
  {
    ret_val = TRUE;
    if ( dev_status->rf_rx_reco.debug_msgs_enabled )
    {
      RF_MSG_3(RF_MED, "rf_cdma_update_rf_ssma_recos: Device %d NOT in RX "
               "or RXTX state - 1x state %d/hdr state %d", 
               device, dev_status->data_1x.rf_state, dev_status->data_hdr.rf_state); 
    }
  }

  return ret_val;

} /* rf_cdma_update_rf_ssma_recos */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  This function does a dynamic update to the notch filter portion of the given  
  RxLm chain 
  
  @param rxlm_handle
  The Rx Link Manager buffer that to be used to configure the Rx Chain.
  
  @param notch_filt_settings
  Pointer that contains the notch filter settings that need to be used for  
  configuring the notches 
 
*/
static boolean
rf_cdma_trigger_notch_update
(
  rfm_mode_enum_type rf_mode,
  lm_handle_type rxlm_handle,
  rf_cdma_mdsp_chain_enum_type rx_mdsp_path,
  rf_cdma_notch_flt_settings_type *const notch_filt_settings
)
{

  boolean ret_val; /* api result */

  /* check for RXLM handle */
  if ( rxlm_handle >= RFM_INVALID_LM_BUFFER )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_update_ssma_notch_recos: Invalid RxLM %d for "
                        "device %d ", rxlm_handle );
    return FALSE;
  }

  /* check for valid notch filter pointer */
  if ( NULL == notch_filt_settings )
  {
    RF_MSG( RF_ERROR, "rf_cdma_update_ssma_notch_recos: Null pointer passed for "
                      "Notch settings" );
    return FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* all checks passed */
  /*--------------------------------------------------------------------------*/

  /* update notch config for the device */
  ret_val = rf_cdma_msm_update_notch_settings(rxlm_handle,notch_filt_settings);
    
  if (ret_val == TRUE)
  {
    /* The notches have to be configured dynamically: need to indicate
       this to FW */
    if ( rf_mode == RFM_1X_MODE )
    { 
      /* call 1x-specific API */
      ret_val = rf_1x_mdsp_trigger_dyn_notch_update( rx_mdsp_path, rxlm_handle,
                                                     NULL, NULL, NULL ); 
    }
    else
    { 
      /* call HDR-specific API */
      ret_val = rf_hdr_mdsp_trigger_dyn_notch_update( rx_mdsp_path, rxlm_handle,
                                                      NULL, NULL, NULL ); 
    }
  } /* if (ret_val == TRUE) ... MSM update success */
  else
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_update_ssma_notch_recos: Failed to update "
                        "notch settings for Mode %d, RxLM %d", 
              rf_mode, rxlm_handle );
  }

  return ret_val;

} /* rf_cdma_trigger_notch_update */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  This function does a dynamic update to the notch filter portion of the given  
  RxLm chain 
  
  @param rxlm_handle
  The Rx Link Manager buffer that to be used to configure the Rx Chain.
  
  @param notch_filt_settings
  Pointer that contains the notch filter settings that need to be used for  
  configuring the notches 
 
*/
boolean
rf_cdma_update_bb_ssma_recos
(
  rfm_device_enum_type device
)
{
  boolean ret_val; /* api result */

  int carr_idx; /* carrier index */

  rf_cdma_msm_dynamic_rxlm_cfg_type dyn_settings;

  /* Pointer to struct that holds per carrier notch filter register settings.
   This struct will be updated with the actual values that need to be 
   programmed to configure the notch filters */
    rf_cdma_notch_flt_settings_type notch_register_vals_per_carr;

  /* need a readable ptr to device status */
  rf_cdma_data_status_type* dev_status = 
                                    rf_cdma_get_mutable_device_status( device );

  const rfc_cdma_logical_device_params_type* logical_dev; /* Logical Device */

  /* get debug flag info */
  const rf_cdma_debug_flag_type* debug_flags = rf_cdma_get_debug_flag_data();

  /* start with all zeros */
  memset( &dyn_settings, 0, sizeof(rf_cdma_msm_dynamic_rxlm_cfg_type));
  memset( &notch_register_vals_per_carr, 0, sizeof(rf_cdma_notch_flt_settings_type));


  if ( debug_flags->ssma_enabled == FALSE )
  {
    RF_MSG(RF_MED, "rf_cdma_update_bb_ssma_recos: SSMA feature not enabled ");
    return TRUE; 
  }

  /* check for valid device */
  if ( device >= RFM_MAX_DEVICES )
  {
    /* it is possible for the paired_device to be RFM_MAX_DEVICE - it is not
       an error - the RFC has been coded that way. It is valid for a given 
       device to have no paired device. hence, return TRUE */
    return TRUE;
  }

  /* check for NULL pointer */
  if (dev_status == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_update_bb_ssma_recos: failed to get device "
                        "status for device %d ", device );
    return FALSE;
  }

  /* get the logical parms for current device */
  logical_dev = rfc_cdma_get_logical_device_params(device);  

  /* check for NULL pointers */
  if (logical_dev == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_update_bb_ssma_recos: Invalid logical_dev "
                        "pointer for device %d", device);
    return FALSE;
  }

  /* check to see if device is active */
  if ( !( (dev_status->data_1x.rf_state == RF_CDMA_STATE_RX) 
          ||
          (dev_status->data_1x.rf_state == RF_CDMA_STATE_RXTX)
          ||
          (dev_status->data_hdr.rf_state == RF_CDMA_STATE_RX) 
          ||
          (dev_status->data_hdr.rf_state == RF_CDMA_STATE_RXTX) ) )
  {
    if ( dev_status->rf_rx_reco.debug_msgs_enabled )
    {
      RF_MSG_3( RF_LOW, "rf_cdma_update_bb_ssma_recos: Device %d NOT in RX "
                "or RXTX state - 1x state %d/hdr state %d . EXIT", device,
                dev_status->data_1x.rf_state, dev_status->data_hdr.rf_state ); 
    }
    return TRUE;
  }

  /* check for valid RF mode */
  if ( ( dev_status->rf_mode != RFM_1X_MODE ) && 
       ( dev_status->rf_mode != RFM_1XEVDO_MODE ) )
  {
    RF_MSG_3( RF_ERROR, "rf_cdma_update_bb_ssma_recos: Invalid Mode %d  "
              "device %d, Mode %d, RxLM %d. EXIT", device, 
              dev_status->rf_mode, dev_status->rxlm_handle );
    return FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* all checks passed -- device is active -- update all active carriers      */
  /*--------------------------------------------------------------------------*/
  ret_val = TRUE;

  rf_cdma_get_spurs_to_suppress( device, dev_status->rxlm_handle, 
                                 dev_status->curr_band, dev_status->lo_tuned_to_chan,
                                 dev_status->num_carriers, &dyn_settings );


  for( carr_idx = 0; carr_idx < (int) dev_status->num_carriers; ++carr_idx )
  {
    RF_MSG_2( RF_LOW, "rf_cdma_update_bb_ssma_recos: Dynamic updates for device"
                      " %d, Carrier %d; (BEGIN)",
                        device, carr_idx );

    /* convert the SSMA BB recos to register settings for current device */
    ret_val &= rf_cdma_msm_conv_recos_to_reg_vals ( 
                  dev_status->rxlm_handle,
                  dev_status->curr_band,
                  dev_status->curr_chans[ carr_idx],
                  dev_status->lo_tuned_to_chan,
                  dev_status->num_carriers,
                  dev_status->rf_rx_reco.bb.per_carrier_notch_reco[carr_idx],
                  &notch_register_vals_per_carr );
  }/*for( carr_idx = 0; carr_idx < (int) dev_st.......)*/

    RF_MSG_1( RF_LOW, "rf_cdma_update_bb_ssma_recos: device %d (END)",
                       device);
 
  /* Acquire Handle lock before writing any settings */
    ret_val &= ( rxlm_acquire_lock(dev_status->rxlm_handle) == LM_SUCCESS );

  /* update notch config for the device */
    ret_val &= rf_cdma_trigger_notch_update ( dev_status->rf_mode,
                                              dev_status->rxlm_handle,
                                              logical_dev->rx_mdsp_path,
                                              &notch_register_vals_per_carr );

  /* update rotators for the device */
    ret_val &= rf_cdma_msm_update_xo_params( dev_status->rf_mode, 
                                             dev_status->rxlm_handle,
                                             dev_status->rotator_offsets_in_hz );

  /* Acquire Handle lock before writing any settings */
    ret_val &= ( rxlm_release_lock(dev_status->rxlm_handle) == LM_SUCCESS );
  /* update device status with freq offsets */
  if (ret_val == TRUE)
  {
    ret_val = rf_cdma_data_update_rot_offsets(device, dyn_settings.freq_offset_hz);
  }

  return ret_val;

} /* rf_cdma_update_bb_ssma_recos */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function retrieves RF and BB recommendations from the RF device for the 
  specified current and paired devices

  @param curr_device
  Rx path that is requesting recommendation
  
  @param curr_device_enabled
  TRUE if RX for this device is to be enabled
  
  @param curr_device_has_valid_tx_path
  TRUE if the device is capable of TX - this means that the device is primary
 
  @param rfdevice_rx_ptr
  Pointer that holds RF device structures
 
  @param band
  Requesting device's band, if enabled
 
  @param chan
  Requesting device's channel, if enabled
 
  @param num_carriers
  Num of carriers active, or to be activated
 
  @param  chan_list_ptr
  Pointer to the list of channels to be tuned to
 
  @param rx_bw_khz
  RX bandwidth (in KHz)
 
  @param tx_bw_khz
  TX bandwidth (in KHz) - is 0 when device is in Rx-only state
 
  @param paired_device
  device that forms a ( primary <==> SHDR ) relation with above param (device)

  @param paired_device_enabled
  TRUE if the paired device is in RX or RXTX state
  
  @param curr_dev_reco_ptr
  Pointer through which RF and BB recommendations are provided for PRx
 
  @param paired_dev_reco_ptr
  Pointer through which RF and BB recommendations are provided for DRx
*/
static void
rf_cdma_get_rx_reco
(
  rfm_device_enum_type const curr_device,
  rfm_cdma_band_class_type const band,
  rfm_cdma_chan_type const chan,
  rfm_device_enum_type const paired_device,
  rfdevice_cdma_rx_reco_type *const curr_dev_reco_ptr,
  rfdevice_cdma_rx_reco_type *const paired_dev_reco_ptr
)
{
  /* Data capsule that has both input and output args to get recos from RF 
     device */ 
  rfdevice_cdma_get_recos_args_type rx_reco_capsule;

  /* to get device status for current and paired devices */
  const rf_cdma_data_status_type *curr_dev_status_r;
  const rf_cdma_data_status_type *paired_device_status_r;

  /* to get logical info from RFC for current device */
  const rfc_cdma_logical_device_params_type* curr_logical_dev;

  /*--------------------------------------------------------------------------*/
  /* end of all declarations ---> validate all input args now                 */
  /*--------------------------------------------------------------------------*/
  if ((curr_device > RFM_MAX_DEVICES) || (paired_device > RFM_MAX_DEVICES))
  {  
    RF_MSG_3( RF_ERROR, "rf_cdma_get_rx_reco: one of the devices is invalid - "
                        "should be less than %d - device=%d, paired_device=%d",  
                        RFM_MAX_DEVICES, curr_device, paired_device );
    return;              
  }

  if (NULL == curr_dev_reco_ptr)
  {
    RF_MSG( RF_ERROR, "rf_cdma_get_rx_reco: Null pointer passed for "
                      "curr_dev_reco_ptr !!");
    return;              
  }
    
  if  (NULL == paired_dev_reco_ptr)
  {  
    RF_MSG( RF_ERROR, "rf_cdma_get_rx_reco: Null pointer passed for "
                      "paired_dev_reco_ptr !!");
    return;              
  }
  
  /* get the logical parms for current device */
  curr_logical_dev = rfc_cdma_get_logical_device_params( curr_device );

  /* check for NULL pointers */
  if ( curr_logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_rx_reco: Invalid logical_dev pointer for "
                        "device %d", curr_device);
    return;              
  }

  /* Get device status - needed to find the current status of device */
  curr_dev_status_r = rf_cdma_get_device_status( curr_device );

  /* check for NULL pointers */
  if (curr_dev_status_r == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_rx_reco: Invalid device_status_r "
                        "pointer for device %d", curr_device);
    return;              
  }

  /* Get device status - needed to find the current status of paired device */
  paired_device_status_r = rf_cdma_get_device_status(paired_device);

  /* check for NULL pointers */
  if (paired_device_status_r == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_get_rx_reco: Invalid paired_device_status_r "
                        "pointer for paired_device %d", paired_device);
    return;              
  }

  /*--------------------------------------------------------------------------*/
  /* All checks passed ... safe to proceed                                    */
  /*--------------------------------------------------------------------------*/

  /* check if the current device is primary - this can be done by checking
     the TX path - if it is valid, then it is primary
  */
  if ( rfc_cdma_is_tx_device(curr_device) == TRUE ) 
  {
    /* assign the correct reco pointers for current and paired devices:
    TX is active - so current device is primary
    ==> Since current device is primary, the paired device has to be SHDR. 
        Assign pointers accordingly */
    rx_reco_capsule.prx_reco_ptr = curr_dev_reco_ptr;
    rx_reco_capsule.shdr_reco_ptr = paired_dev_reco_ptr;

    /* Requesting device => PRx .. need to enable this device - so set
       prx_enabled to TRUE */
    rx_reco_capsule.prx_enabled = TRUE;
    rx_reco_capsule.prx_band = band;
    rx_reco_capsule.prx_chan = chan;
    rx_reco_capsule.num_prx_carriers = curr_dev_status_r->num_carriers;
    rx_reco_capsule.prx_carrier_chan_list_ptr = curr_dev_status_r->curr_chans;
    rx_reco_capsule.prx_bw_khz = curr_dev_status_r->rx_bw_khz;
    rx_reco_capsule.tx_bw_khz = curr_dev_status_r->tx_bw_khz;

      /* assign the chan to which MC-layer wants to tune to. If SSMA module 
        (with info retrieved from RF device) decides to move the LO, then the 
        var (lo_chan) will change */
      curr_dev_reco_ptr->rf.lo_chan = chan;

    /* Other path => SHDR path */
    rx_reco_capsule.shdr_enabled = 
                       ( (paired_device_status_r->data_1x.rf_state == RF_CDMA_STATE_RX)
                         ||
                         (paired_device_status_r->data_hdr.rf_state == RF_CDMA_STATE_RX) );

    if ( rx_reco_capsule.shdr_enabled )
    {
      rx_reco_capsule.shdr_band = paired_device_status_r->curr_band;
      /* SHDR is always only for 1x - which means it is single-carrier, and
         hence using RFM_CDMA_CARRIER_0 is ok */
      rx_reco_capsule.shdr_chan = 
                       paired_device_status_r->curr_chans[RFM_CDMA_CARRIER_0];
      paired_dev_reco_ptr->rf.lo_chan = 
                       paired_device_status_r->curr_chans[RFM_CDMA_CARRIER_0];
    }
  } /*if (logical_dev->txlm_chain != TXLM_CHAIN_MAX) */


  else
  {
    /*  TX is NOT active - so current device is SHDR device
    ==> Since current device is SHDR, the paired device has to be primary.
        Assign pointers accordingly */
    rx_reco_capsule.prx_reco_ptr = paired_dev_reco_ptr;
    rx_reco_capsule.shdr_reco_ptr = curr_dev_reco_ptr;

    /* Requesting path => SHDR path - need to enable this device .. so set
       shdr_enabled to TRUE */
    rx_reco_capsule.shdr_enabled = TRUE;
    rx_reco_capsule.shdr_band = band;
    rx_reco_capsule.shdr_chan = chan;

      /* assign the chan to which MC-layer wants to tune to. If SSMA module 
        (with info retrieved from RF device) decides to move the LO, then the 
        var (lo_chan) will change */
      curr_dev_reco_ptr->rf.lo_chan = chan;

    /* Other path => PRx path */
    rx_reco_capsule.prx_enabled = 
      ( (paired_device_status_r->data_1x.rf_state == RF_CDMA_STATE_RX) 
        || 
        (paired_device_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX)
        ||
        (paired_device_status_r->data_hdr.rf_state == RF_CDMA_STATE_RX) 
        || 
        (paired_device_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX) );

    if ( rx_reco_capsule.prx_enabled )
    {
      rx_reco_capsule.prx_band = paired_device_status_r->curr_band;
      rx_reco_capsule.prx_chan = paired_device_status_r->lo_tuned_to_chan;
      rx_reco_capsule.num_prx_carriers = paired_device_status_r->num_carriers;
      rx_reco_capsule.prx_carrier_chan_list_ptr = 
                                             paired_device_status_r->curr_chans; 
      rx_reco_capsule.prx_bw_khz = paired_device_status_r->rx_bw_khz;
      rx_reco_capsule.tx_bw_khz = paired_device_status_r->tx_bw_khz;
      paired_dev_reco_ptr->rf.lo_chan = paired_device_status_r->lo_tuned_to_chan;
    } /* if ( rx_reco_capsule.prx_enabled ) */

  } /* else */

  { /* sub-block where the recommendations are retrieved from device */

    /* The (band/chan) passed in as args to this API are the config for which
       SSMA spurs need to be determined. 
       Need band on which device is currently operating on - this is needed to 
       dereference the correct transceiver */
    rfm_cdma_band_class_type current_band = curr_dev_status_r->curr_band;

    if ( current_band < RFM_CDMA_MAX_BAND )
    {
      /* Get SSMA recos from device driver */
      rfdevice_cdma_rx_cmd_dispatch( curr_logical_dev->rx_device[current_band],
                             RFDEVICE_GET_RX_RECOMMENDATION, &rx_reco_capsule );
    }
    else
    {
      /* this is not an error case - this API could be called with invalid 
         band / chan 
         (use case: Device-0 is on HDR, Device-1 is on 1x. Now, 
         if device-0 sleeps, need to update the SSMA settings based on new 
         channel config :- 
              device-0 sleeping - so band/chan is invalid
              device-1 tuned to 1x
         ) */
      RF_MSG ( RF_MED, "rf_cdma_get_rx_reco: Attempting to call a device "
                        "API with bad band " );
    }

  } /* END:: sub-block where the recommendations are retrieved from device */

  /*
    results are returned through 'curr_dev_reco_ptr' and 'paired_dev_reco_ptr'
  */
  if ( curr_dev_status_r->rf_rx_reco.debug_msgs_enabled )
  {
    RF_MSG_6( RF_LOW, "rf_cdma_get_rx_reco: " 
              "Curr Dev[HasReco(%d) LOChan(%d) LOOffset(%d KHz)] "
              "Paired Dev[HasReco(%d) LOChan(%d) LOOffset(%d KHz)]",
              curr_dev_reco_ptr->rf.has_reco, curr_dev_reco_ptr->rf.lo_chan,  
              curr_dev_reco_ptr->rf.lo_offset_KHz, 
              paired_dev_reco_ptr->rf.has_reco, paired_dev_reco_ptr->rf.lo_chan, 
              paired_dev_reco_ptr->rf.lo_offset_KHz);
  }

} /* rf_cdma_get_rx_reco */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  retrieve the SSMA recos for given devices
 
  @details
  This API performs all SSMA-related operations:
  -# retrieves RF and BB recommendations for the specified device and paired
     device
  -# updates device status of current and paired device with the BB and new LO 
     channel if applicable 
  -# updates the dynamic settings data structures for the two devices

  @param curr_device
  Rx path that is requesting recommendation
 
  @param curr_dev_dyn_settings
  Container that holds the dynamic settings for the device that is being tuned

  @param band
  Requesting device's band, if enabled
 
  @param chan
  Requesting device's channel, if enabled

  @param paired_device
  device that forms a ( primary <==> SHDR ) relation with above param (device)
 
  @param paired_dev_dyn_settings
  Container that holds the dynamic settings for the paired device
 
  @return
  TRUE on success and FALSE on failure
   
*/
boolean
rf_cdma_get_ssma_recos
(
  const rfm_device_enum_type curr_device,
  const lm_handle_type curr_rxlm_handle,
  rfm_cdma_band_class_type const band,
  rfm_cdma_chan_type const chan,
  const rfm_device_enum_type paired_device
)
{
  boolean ret_val; /* return value of API */

  /* Device Status Write Pointer for paired device - need a writable ptr to
     update the RF and BB recos for the device */
  rf_cdma_data_status_type *paired_dev_status_w =           
              rf_cdma_get_mutable_device_status( paired_device );
  
  /* Device Status Write Pointer for paired device - need a writable ptr to
     update the RF and BB recos for the device */
  rf_cdma_data_status_type *curr_dev_status_w = 
                               rf_cdma_get_mutable_device_status( curr_device );
   
  /* get the logical parms for current device */
  const rfc_cdma_logical_device_params_type* curr_logical_dev = 
                              rfc_cdma_get_logical_device_params( curr_device );
  
  /* get debug flag info */
  const rf_cdma_debug_flag_type* debug_flags = rf_cdma_get_debug_flag_data();

  /*--------------------------------------------------------------------------*/
  /* end of all declarations                                                  */
  /*--------------------------------------------------------------------------*/

  /* validate the device value - it cannot be RFM_MAX_DEVICES or higher */
  if ( curr_device >= RFM_MAX_DEVICES )
  {
    RF_MSG_1 ( RF_ERROR, "rf_cdma_perform_ssma_operations: Invalid device %d", 
               curr_device ); 
    return FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /*                              validate all input args now                 */
  /*--------------------------------------------------------------------------*/
  if (curr_dev_status_w == NULL)
  {
    RF_MSG(RF_ERROR,"rf_cdma_perform_ssma_operations: Null pointer for device"); 
    return FALSE;
  }

  if ( curr_logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_perform_ssma_operations: Invalid logical_dev "
                        "pointer for device %d", curr_device);
    return FALSE;
  }

  /* update the LO chan to valid value, since this info used during RTR tune..
     If this API fails for any reason, we would atleast have a valid value for
     the RTR to tune to.
  */
  curr_dev_status_w->rf_rx_reco.rf.lo_chan = chan;

  if ( debug_flags->ssma_enabled == FALSE )
  {
    RF_MSG(RF_MED, "rf_cdma_perform_ssma_operations: SSMA feature not enabled ");
    return TRUE;
  }

  if ( paired_device == RFM_INVALID_DEVICE )
  {
    RF_MSG(RF_MED, "rf_cdma_perform_ssma_operations: Invalid Paired Device ");
    return TRUE; 
  }

  if (paired_dev_status_w == NULL)
  {
     RF_MSG(RF_ERROR, "rf_cdma_perform_ssma_operations: Null pointer for " 
                     "paired device !!! "); 
     return FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* all checks passed ... safe to proceed */
  /*--------------------------------------------------------------------------*/

  /* start with initial values */
  ret_val = TRUE;

  /* check if paired device is valid - SSMA recos need to be applied only in 
    SHDR mode, where both chains are active. If paired_device==RFM_MAX_DEVICES,
    it means that the current device does not have a paired device, and hence
    cannot operate in SHDR mode. So SSMA operations do not need to be performed
  */
  if ( paired_device != RFM_INVALID_DEVICE )
  {
    /* get the SSMA recomendations and apply them if necessary */
    rf_cdma_get_rx_reco( curr_device, band, chan,
                         paired_device, 
                         &curr_dev_status_w->rf_rx_reco,
                         &paired_dev_status_w->rf_rx_reco );
  }
  
  return ret_val;

} /* rf_cdma_get_ssma_recos */

/*============================================================================*/
/*!
  @name SSMA-related functions (DEBUG)

  @brief
  This section contains all debug related APIs pertaining to SSMA
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Update the SSMA_enabled flag 
 
  @param enable_ssma
  Boolean flag that enables SSMA feature if 1 and disables if set to 0
 
  @return
  TRUE on success and FALSE on failure
   
*/
boolean
rf_cdma_update_ssma_flag
(
  boolean enable_ssma
)
{
  boolean retval; /* ret val of API */

  rf_cdma_debug_flag_type *debug_info = rf_cdma_get_mutable_debug_flag_data();

  if (debug_info != NULL)
  {
    debug_info->ssma_enabled = enable_ssma;
    retval= TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_update_ssma_flag: Null Pointer passed!! ");
    retval = FALSE;
  }

  return retval;

} /* rf_cdma_update_ssma_flag */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enables or disables SSMA debug F3 messages
 
  @param ssma_data
  Pointer to SSMA data structure for which debug messages need to be 
  enabled/disabled
 
  @param debug_en
  Flag indicating, if the debug messages need to be enabled or not
 
  @return
  Status indicating, if the debug messages are enabled or not
*/
boolean
rf_cdma_enable_ssma_debug_msgs
(
  rfdevice_cdma_rx_reco_type *ssma_data,
  boolean debug_en
)
{
  boolean ret_val;

  if ( ssma_data != NULL )
  {
    ssma_data->debug_msgs_enabled = debug_en;
    RF_MSG_1( RF_LOW, 
              "rf_cdma_enable_ssma_debug_msgs: enabled SSMA debug messages (yes" 
              "/no) ->  %d", debug_en );
    ret_val = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_enable_ssma_debug_msgs: Failed to "
                      "configure debug messages for SSMA !" );
    ret_val = FALSE;
  }

  return ret_val;

} /* rf_cdma_enable_ssma_debug_msgs */
#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */


/*! @} */
