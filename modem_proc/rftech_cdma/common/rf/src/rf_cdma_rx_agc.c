/*!
  @file
  rf_cdma_rx_agc.c

  @details
  This file defines interfaces pertaining to CDMA RxAGC configuration.
 
  @addtogroup RF_CDMA_COMMON_RXAGC
  @{ 

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_rx_agc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/09/14   sty     Set the use_events field to FALSE
06/03/14   zhw     [CDMA]Fix rxagc_data ptr for WB jammer
05/13/14   spa     Check for NULL event handle before calling config devices
04/11/14   spa     Add tech data ptr to rxagc data struct/Jammer detect support
04/08/14   sty     KW fixes
12/03/13   sty     removed hardcoding of rf_mode in 
                   rf_cdma_mc_configure_rx_agc_data()
11/27/13   sty     Added correct cell_name when calling 
                   RFLM_CMN_RXAGC_GET_CELL_STATIC_INFO
11/26/13   JJ      fix nb/wb idx issue during retune
11/12/13   spa     Compiler warning fix 
11/08/13   spa     Remove NB/WB index hard coding
10/22/13   spa     Support to populagte DGVA gain offsets
10/14/13   spa     Update WB index in configure RxAGC data
10/09/13   spa     Add call to create RxAGC tasks (for LNA) 
10/08/13   spa     Populate NB-Carrier mapping in config rxagc data
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
04/11/12   aro     Removed unused RxAGC freeze and unfeeze functions
04/11/12   aro     Documentation Update
02/08/12   hdz     Fixed reloading rx agc for div chain 
02/07/12   hdz     Fixed compiler warnings
02/06/12   hdz     Added rf_cdma_update_rx_agc for reloading rx agc in calls
01/15/11   aro     Removed direct access to rfm_mode[] variable
01/10/11   aro     Removed extern rfm_mode[]
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/23/10   aro     Doxygen update
12/18/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_rx_agc.h"
#include "rfm_mode_types.h"
#include "rfcommon_msg.h"
#include "rf_1x_rx_agc.h"
#include "rf_hdr_rx_agc.h"
#include "rflm_dm_api.h"
#include "rfdevice_cdma_interface.h"
#include "rfcommon_time_profile.h"
#include "rflm_c2k_mc.h"
#include "modem_mem.h"
#include "rfm_internal.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Creates the Rx AGC LNA SM script buffers to be populated to FW through CCS
 
  @details
  This API provides the functionality to create dynamic event scripts for
  LNA state changes, to be populated to FW SMEM through the CCS interface.
  The updated LNA SM interface to FW provides for the ability to configure
  multiple LNAs during gain state changes.
 
  @param device
  Device which needs to be configured for Rx AGC LNA SM
 
  @param band
  Band for which configuration is needed
 
  @param rf_mode
  RF mode for which configuration is needed

  @param lna_state
  LNA state for which event must be configured

  @param carrier_mode
  Specify the carrier configuration for 1x and DO

  @param bw_khz
  Bandwidth configuration of Rx AGC operation

  @param power_mode
  Linearity configuration for which LNA query is performed

  @param buffer_index
  Buffer index where FW can find scripts - this is used to determine the shared
  memory location
 
  @return
  TRUE if ON/OFF was success, FALSE in failure case
*/
boolean
rf_cdma_mc_configure_rx_agc_lna_sm
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_mode_enum_type rf_mode,
  rfm_lna_gain_state_type lna_state,
  rf_cdma_carrier_mode_type carrier_mode,
  uint32 bw_khz,
  rfm_cdma_power_mode_type power_mode,
  uint32 buffer_index
)
{
  boolean api_status = TRUE ; 
  /* Buffer object to hold LNA State Machine script data */
  rf_buffer_intf *lna_gain_settings = NULL;
  rf_cdma_event_type event_type;
  uint32 event_id;
  rfcommon_mdsp_event_handle** lna_event_handle;
  const rfcommon_mdsp_event_settings_type *event_settings_ptr;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /*--------------------------------------------------------------------------*/
  /* Step3: Populate the LNA scripts */
  if ( api_status == FALSE )
  {
    RF_MSG( RF_ERROR, 
            "rf_cdma_mc_configure_rx_agc_lna_sm: failed DM API calls" );

    api_status = TRUE;
  }

  switch ( lna_state )
  {
  case RFM_LNA_GAIN_STATE_0:
    event_type = RF_CDMA_EVENT_LNA_G0_SM;
    event_id = RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G0_ID;
    event_settings_ptr = (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G0_ID)) ;
    break;

  case RFM_LNA_GAIN_STATE_1:
    event_type = RF_CDMA_EVENT_LNA_G1_SM;
    event_id = RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G1_ID;
    event_settings_ptr = (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G1_ID)) ;
    break;

  case RFM_LNA_GAIN_STATE_2:
    event_type = RF_CDMA_EVENT_LNA_G2_SM;
    event_id = RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G2_ID;
    event_settings_ptr = (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G2_ID)) ;
    break;

  case RFM_LNA_GAIN_STATE_3:
    event_type = RF_CDMA_EVENT_LNA_G3_SM;
    event_id = RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G3_ID;
    event_settings_ptr = (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G3_ID)) ;
    break;

  case RFM_LNA_GAIN_STATE_4:
    event_type = RF_CDMA_EVENT_LNA_G4_SM;
    event_id = RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G4_ID;
    event_settings_ptr = (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G4_ID)) ;
    break;

  default:
    RF_MSG( RF_ERROR, 
            "rf_cdma_mc_configure_rx_agc_lna_sm: Invalid LNA state" );
    return FALSE;
  } /* switch ( lna_state ) */

  /* Get LNA State event handle */
  lna_event_handle = rf_cdma_data_get_event_handle( device, event_type );

  /* Check for NULL event handle */
  if ( lna_event_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_agc_lna_sm: "
                      "Null LNA event handle! ");
    return FALSE;
  }

  /* Get Logical Device Params */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_agc_lna_sm: "
                      "Null Logical Dev pointer");
    return FALSE;
  }

  /* Create generic buffer interface object */
  lna_gain_settings = rf_buffer_create ( CFW_RF_EVENT_MAX_NUM_OF_SBI  , 
                                       CFW_RF_EVENT_MAX_NUM_OF_RFFE ,  
                                       0 );

  /* Check if valid pointer was returned for buffer object*/
  if( lna_gain_settings == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_agc_lna_sm: "
                      "Unable to create buffer object for LNA SM settings");
    api_status = FALSE;
  }/* if( lna_sm_settings == NULL ) */
  else
  {
    /* Call device interface to obtain the device specific LNA SM settings */
    api_status &= rfdevice_cdma_create_rx_lna_gain_script( 
                                     logical_dev->rx_device[band],
                                     band,
                                     power_mode,
                                     bw_khz,
                                     lna_state,
                                     lna_gain_settings );

    /* Send to MDSP layer if API was success till this point */
    if ( api_status == TRUE )
    {
      /* Configure all LNA states to FW using the settings obtained from the 
      device */
      api_status &= rf_cdma_mdsp_configure_devices( device ,
                                        logical_dev->rx_mdsp_path,
                                        rf_mode,
                                        event_id, 
                                        lna_event_handle,
                                        event_settings_ptr,
                                        lna_gain_settings, 
                                        buffer_index,
                                        rf_cdma_data_get_rxlm_handle (device) ,
                                        NULL,
                                        NULL);
    }
    else
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_mc_configure_rx_agc_lna_sm: Failed to "
                          "generate script for LNA G%d event",
                lna_state ); 
    }
    	
  } /* ! (if( lna_sm_settings == NULL ) ) */

  /* destroy buffer after use */
  if ( lna_gain_settings != NULL )
  {
    rf_buffer_destroy( lna_gain_settings );
  }

  return api_status ;

} /* rf_cdma_mc_configure_rx_agc_lna_sm */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  API that populates the RxAGC module with the relevant config data
 
  @details
  This API is called during the wakeup_rx sequence - when RxAGC needs to be 
  configured, and BEFORE the RF device is tuned to a band/chan
 
  This API calls the relevant Data Manager (DM) APIs to configure the data
  structures for RxAGC
 
  Step 1: Add a "cell" - a cell signifies a Rx path
  Step 2: Update the static data to the cell
  Step 3: Latch - or indicate that cell configuration has completed
 
  @param rf_mode
  Calling mode 1x or HDR, this is needed for creating RxAGC tasks for LNA events
 
  @param device
  RF device/path that is being configured for wakeup
 
  @param rx_handle
  the LM handle that is associated with the above device
 
  @param rxagc_data
  RxAGC config data

  @return
  Flag indicating the status of execution of this function
*/
boolean
rf_cdma_mc_configure_rx_agc_data
(
  const rfm_mode_enum_type  rf_mode,
  const rfm_device_enum_type  device,
  const rfm_cdma_band_class_type band,
  const lm_handle_type  rx_handle,
  const rf_cdma_carrier_mode_type carrier_mode,
  const uint32 bw_khz,
  const rfm_cdma_power_mode_type power_mode,
  rf_cdma_mdsp_rx_agc_config_type* rxagc_data
)
{
  boolean is_retune; /* flag that indicates if the chain is re-tuning */
  boolean res; /* api result */  
  rflm_cmn_rxagc_static_data_t* data_for_cells;
  cell_id_t cell_name = RFLM_CDMA_GENERIC_CELL_ID; /* sample cell id */

  rf_time_tick_type rxagc_cfg_time;    /* API Time Profile */
  rf_time_type rxagc_cfg_time_val;         /* API Time Profile */

  /*--------------------------------------------------------------------------*/
  /* Allocate Memory from heap */
  data_for_cells = modem_mem_alloc( sizeof(rflm_cmn_rxagc_static_data_t),
                                    MODEM_MEM_CLIENT_RFA );
  if ( data_for_cells == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_agc_data: Failed in malloc" );
    return FALSE;
  }

  /*--------------------------------------------------------------------------*/
  /* all checks passed */

  /* check if a cell with cell_name is already present - if yes, then it is 
     an indication that the chain is being retuned. If false, it means the
     chain is being woken up from sleep - so is_retune will be false */
  is_retune = rflm_dm_get_rxagc_data ( rx_handle, RFLM_CMN_RXAGC_IS_CELL_VALID, 
                                       (void*) &cell_name ) ;

  if ( is_retune == FALSE )
  {
    rflm_rxagc_cmn_add_delete_cells_t add_cell_info;
    /*------------------------------------------------------------------------*/
    /* Step 1: add required cells to RxAGC module - only one needed for 1x/HDR*/
    /*------------------------------------------------------------------------*/
    rxagc_cfg_time = rf_time_get_tick();

    add_cell_info.num_cells=1;
    add_cell_info.cell_ids[0] = cell_name; /* sample cell_id */
    add_cell_info.use_events = 0;

    /* add a cell */
    res = rflm_dm_set_rxagc_data ( (rflm_dm_handle_id_t) rx_handle, 
                                   RFLM_CMN_RXAGC_ADD_CELLS, 
                                   (void*) &add_cell_info );

    if ( res == TRUE )
    {
      /* store the cell-ID in rf_cdma_data for future ref */
      rf_cdma_data_set_rxagc_cell_id(device, cell_name);
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_agc_data: Failed to "
                        "add cell" );
    }

    /* start off with all 0's */
    memset (data_for_cells, 0, sizeof(rflm_cmn_rxagc_static_data_t));

  } /*if ( is_retune == FALSE ) */
  else
  {
    data_for_cells->cell_id = cell_name;

    res = rflm_dm_get_rxagc_data ( rx_handle, 
                                   RFLM_CMN_RXAGC_GET_CELL_STATIC_INFO, 
                                   (void*) data_for_cells );

    if ( res == FALSE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_agc_data: Failed to "
                        "get static info for cell" );
    }
  } /* ! if ( is_retune == FALSE ) */
  
  /* safe to proceed only if res == TRUE */
  if ( res == TRUE )
  {
    uint8 ctr;/* iterator  */
    lm_handle_type rxlm_handle = (lm_handle_type)rx_handle;
    rfm_lna_gain_state_type gain_state; /* Iterator for gain states */

    /*------------------------------------------------------------------------*/
    /* Step 2: Update Static info for the cell */
    /*------------------------------------------------------------------------*/
    /* partially fill up the data struct - data_for_cells */
    data_for_cells->cell_id = cell_name;

    for ( ctr=0 ; ctr < RFM_LNA_GAIN_STEP_NUM ; ctr++ )
    {
      uint8 nb_ctr; /* Narrow band counter */

      /* Populate rise and fall switch points for ith LNA state */
      data_for_cells->cell_static_info.fall_thresholds[ctr] = 
        rxagc_data->fall_val[ctr];
      data_for_cells->cell_static_info.rise_thresholds[ctr] =
        rxagc_data->rise_val[ctr];
      /* Populate the LNA phase offsets */
      data_for_cells->cell_static_info.lna_phase_offsets[ctr] = 
        rxagc_data->lna_phase_offset[ctr];

      /* LNA gain offsets, populate to zero, since it is not used by CDMA*/
      data_for_cells->cell_static_info.lna_gain_offsets[ctr] = 0;

      /* Populate cal gain offset for each NB, for the ith LNA state */
      for ( nb_ctr = 0; nb_ctr < RFM_CDMA_CARRIER_NUM; nb_ctr++ )
      {
        data_for_cells->cell_static_info.cal_gain_offset[nb_ctr][ctr] =
          rxagc_data->calibrated_gain_val[nb_ctr][ctr];
      }

    }/*  for ( ctr=0 ; ctr < RFLM_CMN_RXAGC_MAX_LNA_STATES ; ctr++ ) */
    
    /* populate bypass and nonbypass timer values (per LNA gain step)  */
    for ( ctr = 0; ctr < RFM_LNA_GAIN_STEP_NUM; ctr++)
    {
      data_for_cells->cell_static_info.bypass_timer_val[ctr] = 
        rxagc_data->bypass_timer_val[ctr];
      data_for_cells->cell_static_info.nonbypass_timer_val[ctr] = 
        rxagc_data->nonbypass_timer_val[ctr];
    }

    res = rf_cdma_msm_get_dvga_gain_offsets( rxlm_handle,
                            data_for_cells->cell_static_info.dvga_gain_offset );

    /*------------------------------------------------------------------------*/
    /* update LNA SM info to RFLM */
    for ( gain_state = RFM_LNA_GAIN_STATE_0; 
          gain_state < RFM_LNA_GAIN_STATE_NUM; gain_state++ )
    {
      rf_cdma_mc_configure_rx_agc_lna_sm( device, band, rf_mode, 
                                          gain_state, carrier_mode, bw_khz,
                                          power_mode, 0 
                                        /*buffer_index -- dummy for LNA evts*/); 
    }

    res = rflm_dm_set_rxagc_data ( rx_handle, 
                                   RFLM_CMN_RXAGC_UPDATE_CELL_STATIC_INFO, 
                                   (void*) data_for_cells );
    /*------------------------------------------------------------------------*/
    /* Step 3:Everything was successful till this point, call RFLM API to create 
       LNA events for RxAGC, based on Rx handle/tech ID and cell ID, Latch needs
       to be done BEFORE calling API to create rxAGC tasks  */
    /*------------------------------------------------------------------------*/
    if ( res == TRUE )
    {
      /* Scratch area assigned for tech specific usage */
      rflm_cmn_rxagc_tech_data_t cmn_rx_agc_data;
      rflm_tech_id_t tech_id;
      /* Convert RF mode to RFLM enum*/
      if ( rf_mode == RFM_1X_MODE )
      {
        tech_id = RFLM_TECH_1X;
      }
      else
      {
        tech_id = RFLM_TECH_HDR; 
      }

      /* Call API for creating RxAGC tasks */
      res &= rflm_c2k_create_rxagc_tasks( rx_handle, tech_id, cell_name );

      /*----------------------------------------------------------------------*/
      /* Step 4: indicate that cell info update is complete by latching. */
      /*----------------------------------------------------------------------*/
      res &= rflm_dm_set_rxagc_data ( rx_handle, 
                                     RFLM_CMN_RXAGC_LATCH_WB_BUF_INFO, NULL );

      /*----------------------------------------------------------------------*/
      /* Step 5: Get pointer to static tech data */
      res &= rflm_dm_get_rxagc_data( (rflm_dm_handle_id_t) rx_handle,
                              RFLM_CMN_RXAGC_GET_TECH_DATA_PTR,
                              (void*) &cmn_rx_agc_data);

      /* Check for API success and valid pointer */
      if ( res == TRUE )  
      {
        /* Step 6: Save the pointer in cdma data structure */
        rf_cdma_data_set_rxagc_tech_data_ptr( device,
                                              cmn_rx_agc_data.tech_data_ptr );
      }
      else
      {
        RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_agc_data: "
                          "Tech data pointer is NULL! " );
      }
    }/* if ( res == TRUE ) -- updated the cell with static info */

  } /* if ( res == TRUE ) -- all steps for rxagc data config */

  /*--------------------------------------------------------------------------*/
  rxagc_cfg_time_val = rf_time_get_elapsed( rxagc_cfg_time, RF_USEC );

  if ( res == FALSE ) 
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_rx_agc_data: "
                      "Failed in RxAGC data config" );
  }

  /* no need to check if data_for_cells is NULL, since code will not have 
     reached here if data_for_cells were NULL.*/
  modem_mem_free( data_for_cells, MODEM_MEM_CLIENT_RFA );

  RF_MSG_2( RF_MED, "rf_cdma_mc_configure_rx_agc_data: time taken = %d, "
                    "Result = %d ", rxagc_cfg_time_val, res );

  return res;

} /* rf_cdma_mc_configure_rx_agc_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @details
 
  @param device
 
  @param rx_handle

  @return
  Flag indicating the status of execution of this function
*/
boolean
rf_cdma_mc_configure_stop_rx_agc_data
(
  const rfm_device_enum_type device,
  const lm_handle_type rx_handle
)
{
  boolean res;
  rflm_rxagc_cmn_add_delete_cells_t delete_cell_info;

  /*--------------------------------------------------------------------------*/
  /* delete cell from RxAGC module                                            */
  /*--------------------------------------------------------------------------*/
  delete_cell_info.num_cells=1;
  delete_cell_info.cell_ids[0] = rf_cdma_data_get_rxagc_cell_id( device );

  /* delete the cell */
  res = rflm_dm_set_rxagc_data ( (rflm_dm_handle_id_t) rx_handle, 
                                 RFLM_CMN_RXAGC_DELETE_CELLS, 
                                 (void*) &delete_cell_info );

  return res;

} /*rf_cdma_mc_configure_stop_rx_agc_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reload RxAGC
 
  @details
  This function is used to reload RxAGC loop with new configuration.
 
  @param device
  RFM device for which RxAGC is to be reloaded
 
  @return
  Flag indicating the status of execution of this function
*/
boolean
rf_cdma_update_rx_agc
(
  rfm_device_enum_type device
)
{
  const rf_cdma_data_status_type *dev_status = NULL;
  boolean ret_val = TRUE;
  
  /* Get device Status */
  dev_status = rf_cdma_get_device_status( device );
    
  /* pointer check */
  if ( dev_status == NULL )
  {
    /* If this physical device can't be translated to a logical device,
        then no band is supported. */
    RF_MSG_1( RF_ERROR, "rf_cdma_update_rx_agc: Invalid Dev = %d", device );
  
    return FALSE;
  }

  /* rf state and tx_stop status check*/
  if ( !( ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RXTX ) 
          ||
          ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RX ) 
          ||
          ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RXDIV )
          ||
          ( dev_status->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) 
          ||
          ( dev_status->data_hdr.rf_state == RF_CDMA_STATE_RX ) 
          ||
          ( dev_status->data_hdr.rf_state == RF_CDMA_STATE_RXDIV ) ) )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_update_rx_agc: Invalid 1x state = %d/hdr state = %d",
              dev_status->data_1x.rf_state, dev_status->data_hdr.rf_state );
  
    return FALSE;
  }

  if ( dev_status->rf_mode == RFM_1X_MODE )
  {
    ret_val &= rf_1x_update_rx_agc( device );

    RF_MSG( RF_MED, "rf_cdma_update_rx_agc: starting 1x rx agc reloading " );
  }
  else if ( dev_status->rf_mode == RFM_1XEVDO_MODE )
  {
    ret_val &= rf_hdr_update_rx_agc( device );

    RF_MSG( RF_MED, "rf_cdma_update_rx_agc: starting hdr rx agc reloading " );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_update_rx_agc: invalid mod = %d", 
              dev_status->rf_mode );
    ret_val = FALSE;
  }

 return ret_val;
} /* rf_cdma_rx_agc_unfreeze */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function fetches the Jammer detection threshold values from device driver
  and programs the same to RFLM layer.

  @details
  WTR2605 RF chip needs firmware based jammer detection algorithm. To enable
  firmware JD algo, RFSW needs to populate the JD threshold values.
  In the current code base these threshold values are hard coded in device driver.
  This function fetches these threshold values from device driver and populates
  to RFLM.
  
  @param device
  Pointer to Logical Device Parameter to be used during wbagc threshold 
  configuration

*/
boolean rf_cdma_configure_jd_thresh
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band
)
{
  boolean api_status;
  /* JD thresholds obtained from device driver */
  rfdevice_1x_jd_thresh_type jd_data; 
  /* local copy of JD thresholds in the format required for firmware */
  rf_cdma_mdsp_wbagc_config_type jd_thresh_vals;
  rfm_lna_gain_step_type lna_gain_step;

  /* null pointer check */
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_configure_jd_thresh: Invalid logical device ");
    return FALSE;
  }

  jd_data.is_valid = FALSE;
  /* Fetch JD threshold values from device driver */
  api_status = rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[band],
                                              RFDEVICE_GET_JD_THRESH,
                                              &jd_data);

  if(( api_status == TRUE ) && ( jd_data.is_valid == TRUE ))
  {
    /* Populate the local structure with the thresholds obtained from device 
       driver */
    for(lna_gain_step = 0; lna_gain_step < RFM_LNA_GAIN_STEP_NUM; lna_gain_step++)
    {
      jd_thresh_vals.wbagc_jamdet_thresh[lna_gain_step] = jd_data.jd_thresh;
      jd_thresh_vals.jd_thresh_minus_hyst[lna_gain_step] = 
                                     jd_data.jd_thresh_minus_hyst;

      RF_MSG_3( RF_HIGH, "rf_cdma_configure_jd_thresh: threshold %d, pwr_hyst %d"
                         "for lna gain step %d ", 
                         jd_thresh_vals.wbagc_jamdet_thresh[lna_gain_step],
                         jd_thresh_vals.jd_thresh_minus_hyst[lna_gain_step],
                         lna_gain_step);

    }

    /* Call MDSP API :
     - Get the tech specific data pointer from RxAGC data
     - Save the pointer in rf cdma data structure
     - Update the scratch area with the thresholds for all LNA states
    */
    api_status  = rf_cdma_mdsp_configure_jd( device,
                                            logical_dev->rx_mdsp_path, 
                                            &jd_thresh_vals );
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_configure_jd_thresh: Invalid threshold values ");
    api_status =  FALSE;
  }

  return api_status;
}/* rf_cdma_configure_jd_thresh */


#endif /* FEATURE_CDMA1X */

/*! @} */

