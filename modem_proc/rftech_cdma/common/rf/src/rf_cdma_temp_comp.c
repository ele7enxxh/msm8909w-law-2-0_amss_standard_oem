/*! 
  @file
  rf_cdma_temp_comp.c
 
  @brief
  This file defines the common CDMA algorithms to accomplish temperature 
  compensation
 
  @addtogroup RF_CDMA_TEMP_COMP
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_temp_comp.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/20/15   vr      Autopin check-in
12/01/15   vr      Synchronized Therm Read with TxAGC event
11/12/14   spa     Change tx_nv_path to be per band per logical device
09/26/14   JJ      Moved fbrx temp comp from temp comp read handler api to config
                   tx agc api
07/23/14   cd      Update ET delay interface to RFLM, to avoid computation in 
                   critical path
04/17/14   JJ      Added api to read dac therm for temp comp
03/27/14   JJ      Hook up temp_comp for fbrx
01/13/14   cd      Add ET delay programming to RFLM for online mode
01/06/14   hdz     Added rflm txagc logging
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/01/13   Saul    XPT. ET delay vs temp FTM char support
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
05/28/13   APU     Check for HKADC availaiblity before calling
                   rfdevice_hdet_cmn_measure_temp_comp() during start temp comp.
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/24/13   APU     Added rfdevice_hdet_cmn_measure_temp_comp() during temp comp
                   start.
05/08/13   cd      Fixed compiler warning
05/07/13   cd      Temp Comp - Support legacy and xPT temp comp algorithms
04/01/13   zhw     Do HDET temp comp before therm read
04/01/13   cd      Make HDET vs Temp update independent of XPT functionality
02/15/13   spa     Remove code under featurization FEATURE_RF_HAS_QFE1510_HDET
02/04/13   zhw     Use read_therm API for temp comp handler readings.
12/05/12   APU     Fixed off target compiler warnings.                
12/05/12   hdz     Restructured c2k temp comp algorithm
12/05/12   APU     Fixed off target compiler warnings.                
12/03/12   APU     Added a one time check for therm cal monotonicity. 
10/17/12   kai     Changed calling to QFE1510 HDET APIs
10/31/12   adk     Merged antenna tuner code from Nikel.
09/12/12   spa     Removed featurization flag RF_HAS_CDMA_DIME_SUPPORT 
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   aro     Changes to migrate to new precide time profiler
07/31/12   spa     Updated read handler to use rf_hal_buffer scripting for DIME 
07/24/12   spa     Update API status when using configure_temp_comp  
07/24/12   spa     Use configure_temp_comp api to send scripts to fw
08/07/12   hdz     Added rf_cdma_temp_comp_get_therm_index_remainder()
05/14/12   hdz     Added rf_cdma_apps_task.h
04/18/12   hdz     Fixed compiler warnings
04/18/12   hdz     Changed interface of rf_cdma_temp_comp_read_handler() to
                   handle cmd from rf_apps_task.
04/18/12   aro     Added callback function as parameter to override functions
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
03/23/12   hdz     Enabled early termination
03/22/12   aro     Deleted cdma_rf_task file
03/13/12   aro     Added support to enable new RF_TASK dispatching
03/22/12   aro     Mainlined new Dispatch for RF_TASK
03/16/12   hdz     Rename temp_comp.is_combined_with_freq_comp to 
                   temp_comp.init_trigger 
03/13/12   aro     Added support to enable new RF_TASK dispatching. Code
                   is protected with debug flag
02/29/12   cd      Cleaned up legacy band nv tbl references
02/23/12   hdz     Moved set HKADCbusy = FALSE conditionally 
02/23/12   hdz     Block HKADC for the rest of the call is a SW timeout is 
                   detected. 
02/17/12   hdz     Changed MSG level from LOW to ERR if SW timeout is detected 
02/17/12   hdz     Stop timers if a SW time-out is detected 
02/09/12   Saul    MCDO. Tx AGC uses MCDO switch-points.
02/07/12   hdz     Moved therm reading reset from temp_comp_start to temp_comp 
                   init() 
02/06/12   hdz     Changed type of band
01/23/12   hdz     Added API rf_cdma_get_therm_reading()
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
01/19/12   cd      Configure Tx AGC uses PLIM module API to configure lin limit
01/05/12   hdz     Added status check for temp_comp_status 
01/04/12   hdz     Used rf_cdma_obtain_hkadc_lock() to obtain HKADC lock
12/27/11   bmg     Documentation improvements
                   Re-worked spin wait on FW status to reduce delays.
12/22/11   hdz     Add support to make temp comp more robust
12/22/11   shb     Removed logic to restore hkadc settings after therm read - 
                   hdet will take care of this
12/22/11   shb     Renamed therm_param.pdet_lpf_settling_t to 
                   therm_param.therm_settling_t
12/11/11   hdz     Added comment on return value for rf_cdma_temp_comp_start() 
                   Clr mdsp_polling_timer as well in rf_cdma_temp_comp_stop() 
12/10/11   hdz     Fixed SV chain crash due to temp comp
12/08/11   hdz     Added support for rf_1x/hdr_plim_update_temp()
12/06/11   hdz     Extended coverage of critical section for 
                   rf_cdma_temp_comp_read_handle()
12/05/11   hdz     Fixed compiling warning by including "rf_hdr_mdsp.h" and
                   rf_hdr_tx_agc.h 
12/02/11   cd      Added TxLM handle to 1x/HDR configure tx agc 
12/02/11   hdz     Moved 1xhdr temp comp common functions to here
01/08/11   aro     Changed @ingroup to @addtogroup
01/03/11   sty     Added header file
01/03/11   sty     Initial Revision

==============================================================================*/

/*------------------------------------------------------------------------------
 *  Include Files
 *----------------------------------------------------------------------------*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfcommon_time_profile.h"
#include "modem_mem.h"
#include "rf_1x_tx_agc.h"
#include "rf_hdr_tx_agc.h"
#include "rf_cdma_tx_agc_msm.h"
#include "rf_1x_mdsp.h"
#include "rf_hdr_mdsp.h"
#include "rfm_1x_umid.h"
#include "rfm_hdr_umid.h"
#include "rfcommon_msg.h"
#include "rf_cdma_data.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_tx_agc.h"
#include "rfm_internal.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_core_txlin.h"
#include "rf_cdma_temp_comp.h"
#include "rfa_msgr.h"
#include "rfdevice_cdma_interface.h"
#include "rfcommon_math.h"
#include "rfm.h"
#include "rf_1x_power_limiting.h"
#include "rf_hdr_power_limiting.h"
#include "rf_cdma_mc.h"
#include "rf_apps_task.h"
#include "rf_cdma_apps_task.h"
#include "modem_mem.h"
#include "rf_cdma_mdsp.h"
#include "rf_hal_buffer.h"
#include "rf_cdma_constants.h"
#include "rf_cdma_hdet.h"
#include "rfdevice_intf_cmd.h"
#include "rfdevice_hdet_cmn_intf.h"
#include "rfcommon_core_txlin.h"
#include "rfcommon_core_txlin_types.h"
#include "rflm_txagc_api.h"
#include "rflm_c2k_mc.h"

/*------------------------------------------------------------------------------
 *  Preprocessor Definitions and Constants
 *----------------------------------------------------------------------------*/

/*!
  @brief
  Number of times that temp comp can fail the slow (10s) update due to HKADC
  contention before dropping into 3ms quick-polling state.
*/
#define SLOW_POLLING_LIMIT 3

/*!
  @brief
  Number of times that temp comp can fail to get the HKADC in the 3ms
  quick-polling state before giving up until the next 10s try.
*/
#define QUICK_POLLING_TIMEOUT 10


/*------------------------------------------------------------------------------
 *  Type Declarations
 *----------------------------------------------------------------------------*/

/*!
  @brief
  This structure is used to pass the address of the temp comp data pointer
  and the timer ID that triggered the update to the
  rf_cdma_temp_comp_read_handler.

  @details
  This structure defines the format of the payload field that is passed
  through the RF command dispatcher to the RF task.  The temperature
  compensation timer callbacks need to pass the parameters required for
  rf_cdma_temp_comp_read_handler() through the payload field of an
  rf_cmd_type structure.
*/
typedef struct
{
  /*!
    @brief
    Will contain the pointer to the temperature compensation data structure
    for the device that the callback is operating on.
  */
  rf_cdma_temp_comp_funcpt_handler_type data_addr;

  /*!
    @brief
    Indicates which timer (10s or 3ms) timer triggered the temperature
    compensation command.
  */
  uint8 timer_id;

} rf_cdma_temp_comp_cmd_payload_type;

/*-----------------------------------------------------------------------------
- *  Static Function Declarations and Definitions
*-----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Gets THERM read data from MDSP

  @details
  Technology independent wrapper that calls the appropriate MDSP API to get
  THERM read data for 1x or HDR

  @param chain
  MDSP chain to read SMEM from

  @param mode
  Technology or RFM mode (1x or HDR). Decides which MDSP interface API is 
  called by this generic API

  @param *mdsp_hdet_read_data
  Pointer to struct where data read from MDSP will be stored
  
  @retval TRUE
  Data query from MDSP was successful
  
  @retval FALSE
  Data query from MDSP failed
*/
boolean
rf_cdma_temp_comp_get_mdsp_therm_data
(
  rfm_device_enum_type device,
  rf_cdma_mdsp_therm_config_data_type* mdsp_therm_data
)
{
  rflm_c2k_therm_data_t* rflm_therm_data;
  int16 current_tx_agc;

  if ( rf_cdma_data_get_rf_mode(device) == RFM_1X_MODE )
  {
    rflm_therm_data = &(rflm_c2k_get_data()->onex_data.therm_data);   
  }
  else
  {
    rflm_therm_data = &(rflm_c2k_get_data()->hdr_data.therm_data);   
  }

  mdsp_therm_data->read_count = rflm_therm_data->num_therm_serviced;
  mdsp_therm_data->therm_read_scheduled = rflm_therm_data->therm_success;

  return TRUE;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Triggers MDSP to schedule THERM read for given mode

  @details
  Technology independent wrapper that calls 1x or HDR MDSP interface API to
  trigger THERM read

  @param mode
  Technology or RFM mode (1x or HDR). Decides which MDSP interface API is 
  called by this generic API
*/
static void
rf_cdma_temp_comp_trigger_therm_event
(
  rfm_mode_enum_type mode
)
{
  rflm_c2k_therm_data_t* therm_event;

  boolean hdet_in_ftm_mode;

  if ( mode == RFM_1X_MODE )
  {
    therm_event = &(rflm_c2k_get_data()->onex_data.therm_data); 
  }
  else
  {
    therm_event = &(rflm_c2k_get_data()->hdr_data.therm_data);
  }    

  /* Set THERM pending to TRUE to signal pending THERM event */
  therm_event->therm_pending = TRUE;
  therm_event->therm_success = FALSE;
}

static boolean
rf_cdma_temp_comp_config_therm_event
(
  rfm_mode_enum_type mode,
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  const rfc_cdma_logical_device_params_type* logical_dev
)
{
  boolean api_status = TRUE;
  rfdevice_rxtx_common_class * cmn_device = rfc_cdma_get_cmn_rxtx_device(device, band, 0);

  rf_buffer_intf* therm_enable_script = rf_buffer_create ( 0 , 
                                                        RF_CDMA_MAX_RFFE_SCRIPT_SIZE ,  
                                                        0 ) ;
  if ( cmn_device == NULL || therm_enable_script == NULL )
  {
    api_status = FALSE;
  }

  if ( api_status == TRUE )
  {
    api_status = rfdevice_cmn_therm_enable( cmn_device, 
                                            RFDEVICE_CREATE_SCRIPT, 
                                            therm_enable_script );
  }

  if ( api_status == TRUE )
  {
    rfcommon_mdsp_event_handle* event_handle = NULL;

    /* Configure Tx ON event using the obtained settings*/
    api_status &= rf_cdma_mdsp_configure_devices( device ,
                                                  logical_dev->tx_mdsp_path,
                                                  mode,
                                                  RF_CDMA_MDSP_CCS_EVENT_THERM_ENABLE_ID, 
                                                  &event_handle,
                                                  (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_THERM_ENABLE_ID)) ,
                                                  therm_enable_script, 
                                                  0,
                                                  rf_cdma_data_get_txlm_handle (device) ,
                                                  NULL,
                                                  NULL);

    /* Reset event status bit field to IDLE no matter which state it is in.*/
    api_status &= rf_cdma_mdsp_ccs_abort_event( &event_handle );

    /* Free up event handle object and related memory */
    api_status &= rf_cdma_mdsp_ccs_cleanup_event( &event_handle );

  }

  if ( therm_enable_script != NULL )
  {
    rf_buffer_destroy(therm_enable_script);
  }

  return api_status;
}



/*!
  @brief
  Verifies the sanity of the temperature compensation NV data.

  @details
  Will check that the temperature compensation NV data is valid and can be
  used the by the temperature compensation algorithm.  If the NV data pointer
  is NULL or the therm cal isn't monotonically increasing/decreasing, it
  will return FALSE and prevents the temp comp algorithm from trying to
  process invalid data.

  @param device
  The device for which the NV should be checked.

  @param band
  The band for which the NV should be checked.

  @return
  TRUE if the NV appears to be safe to use for temperature compensation.
*/
static boolean
rf_cdma_temp_comp_nv_is_valid
(
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type band
)
{
  /* Logical Device */
  const rfc_cdma_logical_device_params_type *logical_dev;
  const rf_1x_config_type* rf_1x_config_ptr;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_if_temp_comp_nv_valid: "
              "NULL pointer for logical device %d",
              device );

    return FALSE;
  }
 
  /* Gets a modifiable pointer, but stores it as a pointer to const
     to prevent changing anything. */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[band], 
                                             band );

  if ( rf_1x_config_ptr == NULL )
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_if_temp_comp_nv_valid: "
              "NULL pointer for NV config path %d, band %d",
              logical_dev->tx_nv_path[band], band );

    return FALSE;
  }


  /*----------------------------------------------------------------------------
    Validation done.. now return the check for monotonictity.
  */
  return (
    rf_1x_config_ptr->cdma_common_ptr->tx_cal. 
    therm_cal_data.is_therm_cal_nv_data_valid ) ;

} /* rf_cdma_if_temp_comp_nv_valid() */

/*------------------------------------------------------------------------------
 *  Externalized Function Definitions
 *----------------------------------------------------------------------------*/
/*!
  @brief
  Called to handle RF_1X_TEMP_COMP_READ_CMD command.

  @details
  This is the handler function to RF cmd
  RF_1X_TEMP_COMP_READ_CMD and RF_HDR_TEMP_COMP_READ_CMD

  @param module_id
  Current module (1x,hdr,cdma or lte) of operation
  
  @param req_id
  RF command ID that this function handle

  @param req_payload_size
  The payload size of the RF command in the range of 0-72

  @param cmd_ptr
  Pointer to the rf command   
*/
void rf_cdma_temp_comp_read_handler
(
  rf_apps_cmd_type* cmd_ptr
)
{
  uint8 i;
  rf_cdma_temp_comp_data_type* temp_comp_data;
  rf_cdma_temp_comp_cmd_payload_type temp_comp_payload;
  rf_apps_cmd_type *rf_1x_temp_comp_read_cmd = cmd_ptr;

  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  const rf_cdma_data_status_type* dev_status_r;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  uint8 timer_id;
  rfdevice_cdma_therm_read_type therm_read;
  int32 et_path_delay = 0;
  boolean do_vs_temp_delay_update = FALSE;

  /* NV related variables */
  const rf_1x_config_type *rf_1x_config_ptr = NULL;   
  const rf_hdr_config_type *rf_hdr_config_ptr = NULL;
  rf_cdma_tx_cal_type *tx_cal = NULL;

  rfm_cdma_band_class_type rf_band; /* RF band */

  rflm_c2k_data_t* rflm_cdma_data_ptr = rflm_c2k_get_data();
  boolean *therm_read_in_process = NULL;

  if ( cmd_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_read_handler:NULL cmd handler ptr!" );

    return;
  }

  /* copy payload[0-3] to temp_comp_payload.data_addrr.addr_array[0-3] */
  for ( i = 0; i < 4; i++ )
  {
    temp_comp_payload.data_addr.addr_array[i] =
      rf_1x_temp_comp_read_cmd->payload[i];
  }

  temp_comp_data =
    (rf_cdma_temp_comp_data_type*)temp_comp_payload.data_addr.funcpt_handler;

  /* Get the timer ID */
  timer_id = rf_1x_temp_comp_read_cmd->payload[4];

  RF_MSG_1( RF_MED, "rf_cdma_temp_comp_read_handler: timer_id = %d", timer_id );

  /* Get the current device */ 
  dev_status_r = rf_cdma_get_device_status( temp_comp_data->device );
  dev_status_w = rf_cdma_get_mutable_device_status( temp_comp_data->device ); 
  logical_dev = rfc_cdma_get_logical_device_params( temp_comp_data->device );

  if ( NULL == dev_status_r || NULL == dev_status_w || NULL == logical_dev )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_temp_comp_read_handler: device %d NULL status pointer",
              temp_comp_data->device );

    return;
  }

  /* Get current band */
  rf_band = dev_status_r->curr_band;
  
  if ( rf_band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_2( RF_ERROR,"rf_cdma_temp_comp_read_handler: Invalid band %d," 
              "rf_band %d", dev_status_r->curr_band, rf_band);

    return;
  } 
  
  /* Get the NV pointer */
  if ( temp_comp_data->mode == RFM_1X_MODE )
  {
    /* ensure that NV is valid for given device and band */
    rf_1x_config_ptr =
      rf_cdma_nv_get_1x_data( 
                            temp_comp_data->logical_device->tx_nv_path[rf_band], 
                            rf_band );
    therm_read_in_process =  &rflm_cdma_data_ptr->onex_data.therm_data.therm_read_in_process;
    
    if (( NULL == rf_1x_config_ptr) || (NULL == therm_read_in_process) )
    {
      RF_MSG_1( RF_ERROR,
                "rf_cdma_temp_comp_read_handler: "
                "Failed to retrieve valid nv config or valid rflm therm data data for band %d",
                rf_band ); 

      return;
    }
    tx_cal = &rf_1x_config_ptr->cdma_common_ptr->tx_cal;
  }
  else if ( temp_comp_data->mode == RFM_1XEVDO_MODE )
  {
    /* ensure that NV is valid for given device and band */
    rf_hdr_config_ptr =
      rf_cdma_nv_get_hdr_data( 
                          temp_comp_data->logical_device->tx_nv_path[rf_band], 
                          rf_band );
    therm_read_in_process =  &rflm_cdma_data_ptr->hdr_data.therm_data.therm_read_in_process;
    
    if ( ( NULL == rf_hdr_config_ptr ) || (NULL == therm_read_in_process))
    {
      RF_MSG_1(RF_ERROR,"rf_cdma_temp_comp_read_handler:Inviald hdr nv pointer or invalid rflm_therm_data" 
               "from NV for band %d", rf_band); 

      return;
    }
    tx_cal = &rf_hdr_config_ptr->cdma_common_ptr->tx_cal;
  }
  else
  {
    RF_MSG_1(RF_ERROR,"rf_cdma_temp_comp_read_handler:Invalid band %d",rf_band);

    return;
  }

  if (tx_cal == NULL)
  {
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_read_handler: Common tx cal ptr is NULL. Cannot proceed.");

    return;
  }

  /* Extend coverage of critical section */
  rf_common_enter_critical_section( &temp_comp_data->crit_section );

  /* Check the current RF state. Not in RF_CDMA_STATE_RXTX state,sleep timer */
  if ( !( (dev_status_w->data_1x.rf_state == RF_CDMA_STATE_RXTX) 
          ||
          (dev_status_w->data_hdr.rf_state == RF_CDMA_STATE_RXTX) )
       ||
       (temp_comp_data->temp_comp_status == FALSE) )
  {
    timer_clr( &temp_comp_data->temp_sensor_read_timer, T_NONE ); 

    timer_clr( &temp_comp_data->mdsp_polling_timer, T_NONE ); 

    RF_MSG( RF_LOW, "rf_cdma_temp_comp_read_handler: RF is not in "
            "RF_CDMA_STATE_RXTX state, sleep timers");

    rf_common_leave_critical_section( &temp_comp_data->crit_section );

    return ;
  }

    if ( timer_id == TIMER_ID_10S )
    {
	  rf_cdma_temp_comp_trigger_therm_event(temp_comp_data->mode);

        /* Start 3ms timer */
        timer_set(&temp_comp_data->mdsp_polling_timer,3,3,T_MSEC);

        /* Init the quick poll countdown timeout reg*/
        temp_comp_data->quick_poll_countdown_timeout = QUICK_POLLING_TIMEOUT;

    RF_MSG_1( RF_MED, "rf_cdma_temp_comp_read_handler: MDSP Therm read"
                      "triggered. Start polling for %d times",
                      QUICK_POLLING_TIMEOUT);

    rf_common_leave_critical_section( &temp_comp_data->crit_section );

    return ;
    } /* if ( timer_id == TIMER_ID_10S ) */
    else
    {
    rf_cdma_mdsp_therm_config_data_type mdsp_therm_data = {0};

    rf_cdma_temp_comp_get_mdsp_therm_data(temp_comp_data->device, &mdsp_therm_data);

    temp_comp_data->quick_poll_countdown_timeout--;

    if ( temp_comp_data->quick_poll_countdown_timeout < 0 ) 
      {
        timer_clr( &temp_comp_data->mdsp_polling_timer, T_NONE ); 

      RF_MSG( RF_ERROR, "rf_cdma_temp_comp_read_handler: MDSP polling "
              "timer exceed uplimit, quit polling and try during next temp comp");

    rf_common_leave_critical_section( &temp_comp_data->crit_section );

    return ;
  } 
  else 
  {
      if ( mdsp_therm_data.therm_read_scheduled == FALSE ) 
      {
        RF_MSG_1( RF_MED, "rf_cdma_temp_comp_read_handler: Therm not ready. Retry %d times",
                  temp_comp_data->quick_poll_countdown_timeout);      

        rf_common_leave_critical_section( &temp_comp_data->crit_section );

        return ;
      }
      else
      {
    timer_clr( &temp_comp_data->mdsp_polling_timer, T_NONE ); 

        RF_MSG( RF_MED, "rf_cdma_temp_comp_read_handler: MDSP Therm ready. Stop polling");
      }
    }/* if ( temp_comp_data->quick_poll_countdown_timeout >= 0 )  */
  } /* else: if ( timer_id == TIMER_ID_10S ) */

  /*------------------------Enter Temp Comp Routine---------------------------*/

  /*--------------------------------------------------------------------------*/
  /* Do HDET temp comp before therm read. For Tuner HDET, an immediate 
  write is required so that registers are adjusted internally */
  /*--------------------------------------------------------------------------*/
  rfdevice_hdet_cmn_measure_temp_comp( logical_dev->hdet_obj[rf_band],
                                       NULL, RFDEVICE_EXECUTE_IMMEDIATE );

  /*--------------------------------------------------------------------------*/
  /* Do SW therm read */
  /*--------------------------------------------------------------------------*/
  rfdevice_rxtx_common_class * cmn_device = 
              rfc_cdma_get_cmn_rxtx_device(temp_comp_data->device, rf_band, 0);

  temp_comp_data->is_last_reading_valid = 
              rfdevice_cmn_therm_read_and_disable(cmn_device ,
                                                  &(temp_comp_data->last_temp_sensor_read));

  /* Do DAC therm read*/
  rf_cdma_mdsp_iref_therm_read(dev_status_r->txlm_handle);

  RF_MSG_2( RF_LOW, "rf_cdma_temp_comp_read_handler: therm reading: %d, and %d",
            temp_comp_data->last_temp_sensor_read, 
            temp_comp_data->is_last_reading_valid );   
   
  /* Proceed if last therm read is valid */
  if( temp_comp_data->is_last_reading_valid == TRUE )
  {
    /* Get updated ET path delay */
    if( ( tx_cal->et_path_delay_info.et_path_delay_offsets_vs_freq_vs_temp.channels_enabled > 0 ) &&
        ( tx_cal->et_path_delay_info.et_path_delay_offsets_vs_freq_vs_temp.channels_enabled <= RFCOM_TXLIN_NUM_CHANNELS )  )
    {
      do_vs_temp_delay_update = TRUE;
      et_path_delay = rf_cdma_xpt_get_path_delay( temp_comp_data->logical_device->tx_device[rf_band],
                                                  tx_cal, dev_status_r->curr_band, dev_status_r->lo_tuned_to_chan,
                                                  ( rfm_get_calibration_state() == FALSE ),
                                                  dev_status_r->temp_comp.is_last_reading_valid,
                                                  dev_status_r->temp_comp.last_temp_sensor_read );
    }

    /* Do 1x specific sequence to update TX_linearizer_LUT if mode = 
       RFM_1X_MODE; otherwise, do hdr sequence */
    if ( temp_comp_data->mode == RFM_1X_MODE )
    {
      /* Change to FALSE to indicate in the rf_1x_configure_tx_agc()only 
      temp_comp algorithm is applied. Otherwise, both freq_comp and temp comp 
      algorithms are applied                                               */
      dev_status_w->temp_comp.init_trigger = FALSE;

      /* Update AGC with new therm reading */
      rf_1x_configure_tx_agc( temp_comp_data->device,
                              rf_band, 
                              dev_status_r->lo_tuned_to_chan,
                              dev_status_r->txlm_handle,
                              rf_1x_config_ptr ); 

      /*update plim according to temperature */
      rf_1x_plim_update_temp_common( &dev_status_w->plim,
                                     temp_comp_data->device,
                                     rf_1x_config_ptr->cdma_common_ptr,
                                     temp_comp_data->logical_device );

              /* Update lim_vs_temp_vs_freq */
      rf_1x_plim_update_lim_vs_temp_vs_freq( temp_comp_data->device,
                              &dev_status_w->plim,
                              dev_status_w->plim.band,
                              dev_status_w->lo_tuned_to_chan,
                              dev_status_w->temp_comp.last_temp_sensor_read );

      if( do_vs_temp_delay_update == TRUE )
      {
        (void) rflm_1x_mc_config_tx_et_delay( dev_status_r->txlm_handle,
                                              et_path_delay );
      }

    }
    else
    /* HDR mode */
    {
      /* Change to FALSE to indicate in the rf_hdr_configure_tx_agc() only
      temp_comp algorithm is applied. Otherwise, both freq_comp and temp 
      comp algorithms are applied */
      dev_status_w->temp_comp.init_trigger = FALSE;

      /* Update AGC with new therm reading */
      rf_hdr_configure_tx_agc( temp_comp_data->device,
                               dev_status_r->num_carriers,
                               rf_band, 
                               dev_status_r->lo_tuned_to_chan,
                               dev_status_r->txlm_handle,
                               rf_hdr_config_ptr ); 

      /*update plim according to temperature */
      rf_hdr_plim_update_temp_common( &dev_status_w->plim,
                                      temp_comp_data->device,
                                      rf_hdr_config_ptr->cdma_common_ptr,
                                      temp_comp_data->logical_device );

      /* Update lim_vs_temp_vs_freq */
      rf_hdr_plim_update_lim_vs_temp_vs_freq( 
                      temp_comp_data->device,
                      &dev_status_w->plim,
                      dev_status_w->plim.band,
                      dev_status_w->lo_tuned_to_chan,
                      dev_status_w->temp_comp.last_temp_sensor_read );

      if( do_vs_temp_delay_update == TRUE )
      {
        (void) rflm_hdr_mc_config_tx_et_delay( dev_status_r->txlm_handle,
                                               et_path_delay );
      }
    }

    /* Using the latest therm reading, update HDET vs temp data */
       rf_cdma_hdet_update_temperature_params( 
                   &dev_status_w->hdet,
                   dev_status_w->temp_comp.last_temp_sensor_read );

    /* Change to TRUE to indicate in the rf_1x/hdr_configure_tx_agc() both
    freq comp and temp_comp algorithm are applied, otherwise, only temp comp 
    algorithm is applied */
    dev_status_w->temp_comp.init_trigger = TRUE;
  } /* therm_read.therm_quality == TRUE */

  rflm_txagc_lut_diag_log_subpacket(dev_status_r->txlm_handle);

  /* Set is HKADCbusy to FALSE, not in use  */
  rf_cdma_data_set_isHKADCbusy( temp_comp_data->device, FALSE );

  *therm_read_in_process = FALSE;

  RF_MSG( RF_MED, "rf_cdma_temp_comp_read_handler: Temp comp release!" );
  /* Turn on FED Flag */
  
  /* Release critical section lock */
  rf_common_leave_critical_section( &temp_comp_data->crit_section );

  return;

} /* rf_1x_temp_comp_read_handler() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Temp sensor read callback function that will be called to schedule temp
  sensor reads
  
  @details
  The call back function of 1x temp comp polling timer. The first hit after the
  timer set is determined by RF_1x_TEMP_SENSOR_MDSP_FIRST_DELAY_S, while the
  interval between two consecutive call is determined by RF_1x_TEMP_SENSOR_ 
  MDSP_FIRST_DELAY_S. This call back function sends RF_1X_TEMP_COMP_READ_CMD to 
  the queue of RF_task. If the queue is full, it will return right away with 
  error message.

  @param data
  Pointer to current temp comp data 
*/
void
rf_cdma_temp_comp_read_cb
(
  timer_cb_data_type data
)
{
  rf_cdma_temp_comp_data_type* temp_comp_data = 
    (rf_cdma_temp_comp_data_type*)data;
  
  rf_apps_cmd_type *rf_temp_comp_read_cmd;
  rf_cdma_temp_comp_cmd_payload_type temp_comp_payload;
 
  uint8 i;

  if ( temp_comp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_read_cb: Null data!" );
    return;
  }

  /*copy the address of temp_comp_data to rf_1x_temp_comp_funcpt_handler_addr */
  
  temp_comp_payload.data_addr.funcpt_handler
    = ( uint32 )temp_comp_data;

  temp_comp_payload.timer_id = TIMER_ID_10S;

  /* Get the pointer to the free buffer of RF command queue. */
  if ( ( rf_temp_comp_read_cmd = rf_apps_get_buf() ) == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_read_cb: RF command queue is full!" );
    return;
  }

  RF_MSG( RF_MED, "rf_cdma_temp_comp_read_cb: temp_comp" 
          "rf_cdma_temp_comp_read_cb succeed!!" );

  /* fill the cmd_id and module_id according to the mode*/
  if ( temp_comp_data->mode == RFM_1X_MODE )
  {
    rf_temp_comp_read_cmd->msgr_hdr.id = RFA_APPS_1X_TEMP_COMP_READ_CMD;
    rf_temp_comp_read_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_1XHDR_ID;
  }
  else
  {
    rf_temp_comp_read_cmd->msgr_hdr.id = RFA_APPS_HDR_TEMP_COMP_READ_CMD;
    rf_temp_comp_read_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_1XHDR_ID;
  }

  /* copy rf_1x_temp_comp_funcpt_handler_addr to rf_1x_temp_comp_cmd payload*/
  for ( i = 0; i < 4; i++ )
  {
    rf_temp_comp_read_cmd->payload[i] = 
      temp_comp_payload.data_addr.addr_array[i];
  }

   rf_temp_comp_read_cmd->payload[4] = TIMER_ID_10S;
  
  /* place RF_CMD_1X_TEMP_COMP_READ to RF command queue */
  rf_apps_cmd_put( rf_temp_comp_read_cmd );

} /* rf_cdma_temp_comp_read_cb() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  call back function of 3ms timer 
  
  @details
  3 ms timer is started only when temp comp has given up 3 times due to the 
  conflict of HKADC.  
 
  @param data
  Pointer to current temp comp data 
*/
void
rf_cdma_temp_comp_mdsp_poll_cb
(
  timer_cb_data_type data
)
{
  rf_cdma_temp_comp_data_type* temp_comp_data = 
    (rf_cdma_temp_comp_data_type*)data;

  rf_apps_cmd_type *rf_temp_comp_read_cmd;
  rf_cdma_temp_comp_cmd_payload_type temp_comp_payload;
 
  uint8 i;

  if ( temp_comp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_mdsp_poll_cb: Null data!" );
    return;
  }

  /*copy the address of temp_comp_data to rf_1x_temp_comp_funcpt_handler_addr */
  temp_comp_payload.data_addr.funcpt_handler
    = ( uint32 )temp_comp_data;

  temp_comp_payload.timer_id = TIMER_ID_3MS;

  /* Get the pointer to the free buffer of RF command queue. */
  if ( ( rf_temp_comp_read_cmd = rf_apps_get_buf() ) == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_mdsp_poll_cb: RF command queue is full!" );
    return;
  }


  /* fill the cmd_id and module_id according to the mode*/
  if ( temp_comp_data->mode == RFM_1X_MODE )
  {
    rf_temp_comp_read_cmd->msgr_hdr.id = RFA_APPS_1X_TEMP_COMP_READ_CMD;
    rf_temp_comp_read_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_1XHDR_ID;
  }
  else
  {
    rf_temp_comp_read_cmd->msgr_hdr.id = RFA_APPS_HDR_TEMP_COMP_READ_CMD;
    rf_temp_comp_read_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_1XHDR_ID;
  }

  /* copy rf_1x_temp_comp_funcpt_handler_addr to rf_1x_temp_comp_cmd payload*/
  for ( i = 0; i < 4; i++ )
  {
    rf_temp_comp_read_cmd->payload[i] = 
      temp_comp_payload.data_addr.addr_array[i];
  }

  rf_temp_comp_read_cmd->payload[4] = TIMER_ID_3MS; 
  
  /* place RF_CMD_1X_TEMP_COMP_READ to RF command queue */
  rf_apps_cmd_put( rf_temp_comp_read_cmd );
} /* rf_cdma_temp_comp_mdsp_poll_cb() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize 1x temp comp routine.

  @details
  This function will initialize 1x temperature compensation routine and define
  a dedicated timer for polling temperature sensor. The function should be 
  called before any other 1x temp comp functions.

  @param device
  Current device ID

  @param logical_dev
  Pointer to the current logical device
  
  @param temp_comp_data
  pointer to rf_cdma_1x_temp_comp_data

  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_cdma_temp_comp_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_temp_comp_data_type* temp_comp_data
)
{
  boolean init_status = TRUE;

  if ( (temp_comp_data == NULL) || (logical_dev == NULL) )
  {
    init_status = FALSE;
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_init: Null data!" );
    return init_status;
  }

  /* Initialize 1x_temp_comp data  */
  if ( rfc_cdma_is_tx_device(device) == TRUE )
  {
    temp_comp_data->device = device;
    temp_comp_data->logical_device = logical_dev;
    temp_comp_data->slow_poll_skip_count = 0;
    temp_comp_data->temp_comp_status = FALSE;
    temp_comp_data->is_last_reading_valid = FALSE;
    temp_comp_data->last_temp_sensor_read = 0; 
  
    /* Define timer to schedule periodic Temp sensor read */
    timer_def( &temp_comp_data->temp_sensor_read_timer,
               NULL, NULL, 0,
               rf_cdma_temp_comp_read_cb, 
               (timer_cb_data_type)temp_comp_data );

    /* Define timer to poll mdsp shared memory */
    timer_def( &temp_comp_data->mdsp_polling_timer,
               NULL, NULL, 0,
               rf_cdma_temp_comp_mdsp_poll_cb, 
               (timer_cb_data_type)temp_comp_data );
  
    /* Initialize Mutex lock for temp comp critical section - 1 per device */
    init_status &= 
      rf_common_init_critical_section( &temp_comp_data->crit_section );

    temp_comp_data->init_done = TRUE;

    RF_MSG( RF_MED, "rf_cdma_temp_comp_init: temp_comp" 
            "rf_cdma_temp_comp_init succeed!" );
  }
  
  return init_status;

} /* rf_cdma_temp_comp_init() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Start temp sensor polling. 

  @details
  Start the timer defined in rf_1x_temp_comp_init(). This function should be
  called after rf_1x_temp_comp_init().
  
  @param temp_comp_data
  Pointer to rf_cdma_1x_temp_comp_data

  @param mode
  Current rf mode of operation
 
  @param logical_dev
  Pointer to the current logical device
 
  @param band
  Current band of operation
 
  @param chan
  Current channel of operation
  
  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_cdma_temp_comp_start
(
  rf_cdma_temp_comp_data_type * temp_comp_data,
  rfm_mode_enum_type mode,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{
  boolean start_status = TRUE;
  rfdevice_cdma_therm_param_type therm_param;
  boolean temp_comp_nv_is_valid = TRUE;

  const rf_cdma_data_status_type* dev_status_r ;

  if ( (mode != RFM_1X_MODE) && ( mode != RFM_1XEVDO_MODE) )
  {
    start_status = FALSE;

    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_start: Non 1xhdr mode" );
  
    return start_status;
  }

  if ( temp_comp_data == NULL )
  {
    start_status = FALSE;

    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_start: Null data!" );

    return start_status;
  }

  if ( temp_comp_data->init_done == FALSE )
  {
    start_status = FALSE;

    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_start: temp_comp"
            "is not initialized yet!" );

    return start_status;
  }

  dev_status_r = 
    rf_cdma_get_device_status ( temp_comp_data->device ) ; 
  if ( dev_status_r == NULL )
  {
    start_status = FALSE;

    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_start: Bad device_status_r" );
  
    return start_status;
  }

  /* Sanity check of temp comp related NV items. If fails, disable temp comp*/
  if ( rf_cdma_temp_comp_nv_is_valid( temp_comp_data->device, band ) == FALSE )
  {
    temp_comp_nv_is_valid = FALSE;

    RF_MSG( RF_MED, "rf_cdma_temp_comp_start: Bad NV items, disable temp comp" );
         
  }
  else
  {
    /* save current device info to temp_comp_data for future use. */
    temp_comp_nv_is_valid = TRUE;
    temp_comp_data->logical_device = logical_dev;
    temp_comp_data->mode = mode;
    temp_comp_data->slow_poll_skip_count = 0;

    RF_MSG( RF_MED, "rf_cdma_temp_comp_start: Valid NV items." );
           
  }

  rf_common_enter_critical_section( &temp_comp_data->crit_section );

  /* Get therm parameters from device layer*/
  start_status &= 
    rfdevice_cdma_tx_get_therm_param( logical_dev->tx_device[band] ,
                                                    &therm_param );

  if ( (rf_cdma_debug_flags.disable_temp_comp == 0) &&
       (temp_comp_nv_is_valid == TRUE) )
  {
    /* Don't start temp comp if phone is in calibration mode */
    if ( (rfm_get_calibration_state() == FALSE) && 
         (start_status == TRUE) &&
         (therm_param.therm_supported == TRUE) )
    {
      /* Define timer to schedule periodic Temp sensor read */
      timer_set(&temp_comp_data->temp_sensor_read_timer, 
                RF_CDMA_TEMP_SENSOR_MDSP_FIRST_DELAY_S, 
                RF_CDMA_TEMP_SENSOR_MDSP_POLLING_PERIOD_S,
                T_MSEC);
      /* indicate temp comp algorithm is running*/
      temp_comp_data->temp_comp_status = TRUE;
    }
  
  }
   /*--------------------------------------------------------------------------*/
  /* Do HDET temp comp before therm read. For Tuner HDET, an immediate 
  write is required so that registers are adjusted internally. Get the temp
  for the start */
  /*--------------------------------------------------------------------------*/
  if ( dev_status_r->isHKADCbusy == FALSE ) 
  {
    rfdevice_hdet_cmn_measure_temp_comp( logical_dev->hdet_obj[band],
                                       NULL, RFDEVICE_EXECUTE_IMMEDIATE );
  }
  else
  {
    RF_MSG ( RF_ERROR , "rf_cdma_temp_comp_start(): Could not execute " 
             "rfdevice_hdet_cmn_measure_temp_comp() API as HK-ADC is busy " ) ;
  }

  /* Commit temp comp event */

  start_status&= rf_cdma_temp_comp_config_therm_event(mode,
                                                       temp_comp_data->device,
                                                       band,
                                                       logical_dev );
  
  rf_common_leave_critical_section( &temp_comp_data->crit_section );

  /* At every temp comp start, including at retune, determine the 
  temp comp algorithm to be used for the current driver status */
  if ( start_status == TRUE )
  {
    start_status &= rf_cdma_temp_comp_update_algo( temp_comp_data,
                                                   logical_dev, band );
  }

  RF_MSG_1( RF_MED, "rf_cdma_temp_comp_start:" 
            "succeed at device %d !!", temp_comp_data->device );
      
  /* will return TURE even NV for temp comp is invalid. In that case, we just
  disable temp comp, and return TRUE to mc layer. Will return FALSE only when 
  rfdevice_cdma_tx_get_therm_param() return FALSE or ealier pointer check 
  failed.*/
  return start_status;

} /* rf_cdma_temp_comp_start() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stop temp sensor polling. 

  @details
  Stop the active 1x temp comp timer.
  
  @param temp_comp_data
  Pointer to temp_comp_data.

  @return 
  TRUE on success and FALSE on failure
*/
boolean
rf_cdma_temp_comp_stop
(
  rf_cdma_temp_comp_data_type *temp_comp_data
)
{
  boolean stop_status = TRUE;

  if ( temp_comp_data == NULL )
  {
    stop_status = FALSE;
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_stop: Null data!" );
    return stop_status;
  }

  if ( temp_comp_data->init_done == FALSE )
  {
    stop_status = FALSE;
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_stop:" 
            "is not initialized yet!" );
    return stop_status;
  }

  rf_common_enter_critical_section( &temp_comp_data->crit_section );
 
  /*timer will be cleared only not in calibration mode*/
  if ( rfm_get_calibration_state() == FALSE )
  {
    /* Clear callback timers right away*/
    timer_clr( &temp_comp_data->temp_sensor_read_timer, T_NONE ); 

    timer_clr( &temp_comp_data->mdsp_polling_timer, T_NONE );
    
    /* Indicate temp comp algorithm is sleeping*/
    temp_comp_data->temp_comp_status = FALSE; 

    RF_MSG( RF_MED, "rf_cdma_temp_comp_stop: " 
            "rf_cdma_temp_comp_stop succeed!!" );
  }
  rf_common_leave_critical_section( &temp_comp_data->crit_section );

  return stop_status;

} /* rf_cdma_temp_comp_stop() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enables temperature compensation

  @details
  This function enables temp. comp
 
  @param device
  Device for which temp comp. needs to be enabled
 
  @return
  None
*/
void
rf_cdma_enable_temp_comp
(
  rfm_device_enum_type device
)
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disables temperature compensation

  @details
  This function disables temp. comp
 
  @param device
  Device for which temp comp. needs to be disabled
 
  @return
  None
*/
void
rf_cdma_disable_temp_comp
(
  rfm_device_enum_type device
)
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the lock of HKADC

  @details
  This API is called by either HDET routine or temp comp routine to obtain HKADC 
  lock. The procedure is put into the same critical section to prevent race 
  condition. If pass-in pointer is NULL or critical section is not initialized, 
  the lock will not be granted. 
  
  @param *dev_status_w
  Pointer to cdma data of current RTR device for which the HKADC is to be 
  locked on.

  @return
  TRUE granted the lock otherwise, the lock is not granted.
*/
boolean
rf_cdma_obtain_hkadc_lock
(
  rfm_device_enum_type device
)
{
  boolean status = TRUE;

  rf_cdma_data_status_type *dev_status_w;

  dev_status_w = rf_cdma_get_mutable_device_status( device ); 

  if ( dev_status_w == NULL )
  {
    return FALSE;
  }

  if ( (&dev_status_w->hkadc_critical_sect) != NULL )
  {
    /* Enter Device Critical Section, so that the device call is thread safe */
    rf_common_enter_critical_section( &dev_status_w->hkadc_critical_sect );
  
    /* Call Device Driver to perform therm read*/
    if ( dev_status_w->isHKADCbusy == FALSE )
    {
      /* set the flag to true to lock HKADC*/
      rf_cdma_data_set_isHKADCbusy( device, TRUE );

      /* indicate lock successfully*/
      status = TRUE;
    }
    else
    {
      /* indicate lock un-successfully*/
      status = FALSE;
    }

    /* Leave Device Critical Section */
    rf_common_leave_critical_section( &dev_status_w->hkadc_critical_sect );
  }
  else
  {
    /* indicate lock un-successfully*/
    status = FALSE;
  }

  return status;

} /* rf_cdma_obtain_hkadc_lock */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the lock of HKADC

  @details
  This API is called by either HDET routine or temp comp routine to release HKADC 
  lock. The procedure is put into the same critical section to prevent race 
  condition. If pass-in pointer is NULL or critical section is not initialized, 
  the lock will not be granted. 
  
  @param *dev_status_w
  Pointer to cdma data of current RTR device for which the HKADC is to be 
  locked on.

  @return
  TRUE granted the lock otherwise, the lock is not granted.
*/
boolean
rf_cdma_release_hkadc_lock
(
  rfm_device_enum_type device
)
{
  boolean status = TRUE;

  rf_cdma_data_status_type *dev_status_w;

  dev_status_w = rf_cdma_get_mutable_device_status( device ); 

  if ( dev_status_w == NULL )
  {
    return FALSE;
  }

  if ( (&dev_status_w->hkadc_critical_sect) != NULL )
  {
    /* Enter Device Critical Section, so that the device call is thread safe */
    rf_common_enter_critical_section( &dev_status_w->hkadc_critical_sect );
  
    if ( dev_status_w->isHKADCbusy == TRUE )
    {
      /* set the flag to true to unlock HKADC*/
      rf_cdma_data_set_isHKADCbusy( device, FALSE );

      /* indicate unlock successfully*/
      status = TRUE;
    }
    else
    {
      /* indicate unlock un-successfully*/
      status = FALSE;
    }

    /* Leave Device Critical Section */
    rf_common_leave_critical_section( &dev_status_w->hkadc_critical_sect );
  }
  else
  {
    /* indicate lock un-successfully*/
    status = FALSE;
  }

  return status;

} /* rf_cdma_release_hkadc_lock */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return thermistor reading

  @details
  This API will return thermistor reading per requrest. If the last reading is 
  valid, then the valid reading will be returned; Otherwise, zero will be 
  returned. 
  
  @param device
   Device of current operation is on

  @return
  Thermistor reading
*/
uint16
rf_cdma_get_therm_reading
(
  rfm_device_enum_type device
)
{
  uint16 therm_reading;

  rf_cdma_data_status_type *dev_status_w;

  dev_status_w = rf_cdma_get_mutable_device_status( device ); 

  /*If pointer is NULL, return reading as zero*/
  if ( dev_status_w == NULL )
  {
    therm_reading = 0x0;
  }
  else
  { 
    /*If last reading is valid, return the last reading*/
    if ( dev_status_w->temp_comp.is_last_reading_valid == TRUE )
    {
      therm_reading = dev_status_w->temp_comp.last_temp_sensor_read;
    }
    /*Last reading is invalid, return zero */
    else
    {
      therm_reading = 0x0;
    }
  }

  return therm_reading;

} /* rf_cdma_get_therm_reading */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate index of therm bin the current temperature falls in.

  @details
  Assuming therm bin is monotonically increasing, e.g., bin[i] < bin[j] as long 
  as i <j:
  
  index satisfies cur_temp < bin[index +1] where index is the biggest among all 
  possible
    
  @param device
  Current device the Tx is on
*/
uint8 
rf_cdma_temp_comp_get_therm_index_remainder
(
  rfm_device_enum_type device,
  uint8 *remainder,
  uint16 *therm_raw
)
{
  uint8 therm_index = 0;
  const rfc_cdma_logical_device_params_type *logical_dev;
  const rf_1x_config_type* rf_1x_config_ptr;
  uint32 therm_reading;
  rf_cdma_data_status_type *dev_status_w;
  
  if ( rf_cdma_temp_comp_nv_is_valid( device, 
                                      rf_cdma_data_get_curr_band(device) ) == FALSE )
  {
    RF_MSG( RF_MED, "rf_cdma_temp_comp_get_therm_index_remainder: "
                    "Bad NV items, return 0" );

    *remainder = 0;
    *therm_raw = 0;

    return 0;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if ( (logical_dev == NULL) || (dev_status_w == NULL) )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_if_temp_comp_nv_valid: "
              "NULL pointer for logical device %d",
              device );

    *remainder = 0;
    *therm_raw = 0;
    return 0;
  }
 
  /* Gets a modifiable pointer, but stores it as a pointer to const
     to prevent changing anything. */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( 
                      logical_dev->tx_nv_path[dev_status_w->curr_band], 
                      dev_status_w->curr_band );

  if ( rf_1x_config_ptr == NULL )
  {
    RF_MSG_2( RF_LOW,
              "rf_cdma_if_temp_comp_nv_valid: "
              "NULL pointer for NV config path %d, band %d",
              logical_dev->tx_nv_path[dev_status_w->curr_band], 
              dev_status_w->curr_band );

    *remainder = 0;
    *therm_raw = 0;

    return 0;
  }
  
  therm_reading = (uint32)(rf_cdma_get_therm_reading ( device ) ) ;

  *therm_raw = (uint16) therm_reading ;

  /*RF_MSG_8( RF_ERROR,
            " %d, %d, %d, %d, %d, %d, %d, %d",
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[0],
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[1],
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[2],
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[3],
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[4],
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[5],
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[6],
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[7]);*/

  if ( therm_reading == 0x0)
  {
    *remainder = 0;
    return 0;
  }
  else
  {
    if( therm_reading < 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[1])
    {
      therm_index = 0;
    }
    else if (therm_reading < 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[2])
    {
      therm_index = 1;
    }
    else if (therm_reading < 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[3])
    {
      therm_index = 2;
    }
    else if (therm_reading < 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[4])
    {
      therm_index = 3;
    }
    else if (therm_reading < 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[5])
    {
      therm_index = 4;
    }
    else if (therm_reading < 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[6])
    {
      therm_index = 5;
    }
    else if (therm_reading < 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[7])
    {
      therm_index = 6;
    }
    else
    {
      therm_index = 7;
    }


  }
  
  if ( therm_reading <= 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[0])
  {
    *remainder = 0;
  }
  else if ( therm_reading >= 
      rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[7] )
  {
    *remainder = 0;
  }
  else
  {
    /* for fixing KW issue*/
    if( therm_index < 7)
    {
      *remainder = (uint8)(((therm_reading - 
        rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[therm_index])<<8)/ 
        (rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[therm_index+1] - 
        rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[therm_index]) ) ; 
    }
    else
    {
      *remainder  = 0;
    }
  }

  RF_MSG_4( RF_LOW,
            "index = %d, remainder =  %d, therm_reading = %d, %d",
            therm_index, *remainder, therm_reading, 
            rf_1x_config_ptr->cdma_common_ptr->tx_cal.therm_cal_data.tx_therm_cal[therm_index]);

  return therm_index;
}


boolean
rf_cdma_temp_comp_check_monotonicity
(
  uint16    items[] ,
  uint16    size
)
{
  uint8 i;

  if ( items == NULL )
  {
    RF_MSG( RF_ERROR,
              "rf_cdma_temp_comp_check_monotonicity(): "
              "NULL pointer for items " ) ;

    return FALSE;
  }
  if ( size == 0 )
  {
    RF_MSG( RF_ERROR,
              "rf_cdma_temp_comp_check_monotonicity(): "
              "0 items not allowed " ) ;

    return FALSE;
  }

  if ( items[0] < items[1] )
  {
    /* Therm cal is increasing */
    for ( i = 1 ; i < size ; i++ )
    {
      if ( items[i-1] >= items[i] )
      {
        RF_MSG( RF_ERROR,
                "rf_cdma_temp_comp_check_monotonicity(): "
                "Non-monotonically increasing items " );
        return FALSE;
      }
    } /* for ( i = 1; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ ) */
  } /* if therm_bin[0] < therm_bin[1] */
  else
  {
    /* Therm cal is non-increasing */
    for ( i = 1; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
    {
      if ( items[i-1] <= items[i] )
      {
        RF_MSG( RF_ERROR,
                "rf_cdma_if_temp_comp_nv_valid(): "
                "Non-monotonically decreasing items " );
        return FALSE;
      }
    } /* for ( i = 1; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ ) */
  } /* else: if therm_bin[0] < therm_bin[1] */

  /* If none of the above checks failed, NV appears valid.  Return true. */
  return TRUE;
} /* rf_cdma_if_temp_comp_nv_valid() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates the current temp comp algorithm to be used (refer to
  rfcommon_core_txlin_get_temp_comp_algo()) and updates it to the CDMA temp
  comp device data structure
 
  @param temp_comp_data
  Temp Comp data structure to be updated for current device

  @param logical_dev
  Logical device pointer

  @param band
  Band for which the temp comp algorithm must be updated

  @return
  TRUE if the API is successful, else FALSE

*/
boolean
rf_cdma_temp_comp_update_algo
(
  rf_cdma_temp_comp_data_type *temp_comp_data,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rfm_cdma_band_class_type band
)
{
  /* Static NV data pointer */
  const rf_cdma_tx_static_type *tx_static_nv_ptr;
  /* Multilinearizer NV data retrieval */
  const tx_band_cal_type *tx_multilin_nv_cal_ptr = NULL;

  /* Check for calibration state */
  if ( rfm_get_calibration_state() == TRUE )
  {
    temp_comp_data->temp_comp_algo = RFCOM_TXLIN_TEMP_COMP_ALGO_INVALID;

    return TRUE;
  }

  /* Get a pointer to Tx static NV data */
  tx_static_nv_ptr = rf_cdma_nv_get_tx_static( 
                           logical_dev->tx_nv_path[band], band );
  if ( tx_static_nv_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_update_algo: "
                      "Failed to get valid static NV pointer" );
    return FALSE;
  }

  /* Get a pointer to the Tx calibration data */
  tx_multilin_nv_cal_ptr = rf_cdma_nv_get_tx_multilin_cal(
                                          logical_dev->tx_nv_path[band],
                                          band );

  /* Null pointer check */
  if ( tx_multilin_nv_cal_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_temp_comp_update_algo: "
                      "Failed to get valid multilin NV pointer" );

    return FALSE;
  }

  /* Obtain the current temp comp algorithm and store it to device 
  status */
  temp_comp_data->temp_comp_algo = 
    rfcommon_core_txlin_get_temp_comp_algo( 
       (tx_multi_lin_rev_type)tx_multilin_nv_cal_ptr->nv_rev_type,
       (rfcom_txlin_temp_comp_nv_data_type*)
                                   &tx_static_nv_ptr->lin_temp_comp_data );

  RF_MSG_1( RF_MED, "rf_cdma_temp_comp_update_algo: Updated algorithm for "
                    "to %d",
            temp_comp_data->temp_comp_algo );

  return TRUE;

}/* rf_cdma_temp_comp_update_algo */
#endif /* FEATURE_CDMA1X */

/*! @} */

