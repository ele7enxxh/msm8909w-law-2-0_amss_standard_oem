/*!
  @file
  rf_tdscdma_core_antenna_tuner.c

  @brief
  Provides TDSCDMA antenna tuner functionality

  @details

*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_antenna_tuner.c#1 $

when         who    what, where, why
--------   ---     ----------------------------------------------------------
10/21/14   yb      Added priority configuration to config Tuners during CRAT scenarios
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
09/10/14   ych     Fix Tuner callflow issues
09/10/14   ych     Disabled rfnv_tdscdma_b40.c and made corresponding compilation/cw/kw changes
06/22/14   ych	   Add tuner_disable for both sleep/exit and Tx mode in IRAT
06/03/14   jz      Add tuner tune code override command
04/24/14   ych     Added dynamic tuner script for ifreq
04/09/14   jyu     Updated TxLM handle for rf_tdscdma_core_antenna_tuner_program_tx_mode()
03/26/14   kg      Added support for CL LM handle 
02/12/14   ms      Disable tuner in X2T iRAT case (porting per CR600367)
07/12/13   jz       Hook CL NV pointer in tuner set tx mode
06/27/13   ndb     Added rfcommon_nv_ant_tuner_cl_ctrl_type in tuner tx mode 
05/02/13   vb      Tuner CA changes 
04/30/13   vb      Fix for wrong argument when device script needs to run in 
                         IMMEDIATE mode
04/23/13   jyu     Changed the interface to program Tx mode
03/21/13   ms      Tuner TDET support
01/07/13   ndb    Added Tech specific Settings support in rx_init
11/19/12   jyu     Initial version (ported from WCDMA)

============================================================================*/

#ifdef FEATURE_RF_HAS_QTUNER

#include "rf_tdscdma_core_antenna_tuner.h"
#include "rfcommon_msg.h"
#include "rfc_card.h"
#include "rf_tdscdma_mc.h"
#include "rfc_card_tdscdma.h"
#include "rfcommon_atuner_intf.h"
#include "rfnv_tdscdma_b34.h"
#include "rfnv_tdscdma_b39.h"
#include "rfnv_tdscdma_b40.h"

#if 0
static rfm_new_scenario_cb_type *rf_tdscdma_core_tuner_wl1_cb_ptr = NULL;
/* Only as the flag be set, RF can call the WL1 call back fn
   Whenever the WL1 call back fn be called, the flag will be cleared.*/
static boolean enable_wl1_cb_fn_flag = TRUE;
#endif

boolean rf_tdscdma_tuner_rf_more_f3_flag = FALSE;
static boolean rf_tdscdma_QFE_TX_status = TRUE;
#if 0
/*---------------------------------------------------------------------------*/
/*!
  @brief
  rf_tdscdma_core_create_tuner_managers
  @details
  Create tuner manager for all devices
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_create_tuner_managers(void)
{
  boolean ret_val = TRUE;
 
  ret_val &= rfcommon_antenna_tuner_create_tuner_manager(
                                          RFM_DEVICE_0, 
                                          RFM_IMT_MODE, 
                                          RFCMN_ANT_TUNER_AGC_INDEX_PRIMARY);

  ret_val &= rfcommon_antenna_tuner_create_tuner_manager(
                                          RFM_DEVICE_1, 
                                          RFM_IMT_MODE, 
                                          RFCMN_ANT_TUNER_AGC_INDEX_DIVERSITY);

  RF_MSG_1(RF_LOW,"ANT_TUNER: Anntenna tuner manager created", 0);

  return ret_val;
} /* rf_tdscdma_core_create_tuner_managers */
#endif


/*---------------------------------------------------------------------------*/
/*!
  @brief
  Initial QFE tuner
  @details
  Initial QFE tuner
 
  @param device
  The device path to be enabled.

  @param band
   
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_init
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer  
)
{
  boolean init_status = TRUE;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  uint32 script_token =0;

  /* Get Power tracker object from RFC */
  qtuner_mgr_obj = (void**)rfc_tdscdma_get_rf_device_obj( device, 
                                                      RFC_CONFIG_RX, 
                                                      band, 
                                                      RFDEVICE_TUNER_MANAGER );

  if (qtuner_mgr_obj != NULL)
  {
    while (qtuner_mgr_obj[device_idx]!= NULL)
    {
      /* Initialize tuner */
      init_status &= rfcommon_atuner_manager_init ( qtuner_mgr_obj[device_idx], 
                                                    RFCOM_TDSCDMA_MODE, 
                                                    dev_action, 
                                                    script_buffer,
                                                    0, 
                                                    &script_token);

   //check if below is needed
   #if 0 
      /* Initialize Rx */
      init_status &= rfdevice_antenna_tuner_rx_init( qtuner_obj[device_idx],
                                                     NULL,
                                                     RFM_TDSCDMA_MODE,
                                                     RFDEVICE_EXECUTE_IMMEDIATE);
   #endif

      device_idx++; 
      // need to check and break for MAX DEVICE IDX
      
    }
  }
  else
  {
    init_status = FALSE;
    RF_MSG_2(RF_ERROR, "Qtuner_obj is NULL for Device %d and Band %d for Rx", device, band);
  }

  if(init_status && (rf_tdscdma_tuner_rf_more_f3_flag == TRUE))
  {   
    RF_MSG_1(RF_HIGH, "ANT_TUNER: Anntenna tuner init on device %d", device);
  }

  return init_status;
} /* rf_tdscdma_core_antenna_tuner_init */


/*---------------------------------------------------------------------------*/
/*!
  @brief
  Disable the QFE.

  @details
  Disable the QFE.

  @param device
  The device path to be disabled.

  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_disable
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing  
)
{
  boolean init_status = TRUE;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  uint32 script_token =0;

  /* Get Qtuner object from RFC */
  qtuner_mgr_obj = (void**)rfc_tdscdma_get_rf_device_obj( device, 
                                                      RFC_CONFIG_RX, 
                                                      band, 
                                                      RFDEVICE_TUNER_MANAGER );
  if( qtuner_mgr_obj != NULL)
  {
    while( qtuner_mgr_obj[device_idx]!= NULL)
    {
      /* Disable tuner */
      init_status &= rfcommon_atuner_manager_disable( qtuner_mgr_obj[device_idx],
                                                      dev_action, 
                                                      script_buffer, 
                                                      script_timing, 
                                                      &script_token );
      device_idx++;
    }
  }
  else
  {
    init_status = FALSE;
    RF_MSG_2(RF_ERROR, "Qtuner_obj is NULL for Device %d and Band %d for Rx", device, band);
  }

  if( init_status)
  {   
    RF_MSG_1( RF_HIGH, "ANT_TUNER: Disabled on device %d", device);
  }

  return init_status;

} /* rf_tdscdma_core_antenna_tuner_disable */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the QFE RX.

  @details
  Programs QFE tuner initialization sequence, and set up RX path.

  @param device
  The device path to be enabled.

  @param band

  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_program_rx_mode 
( 
  rfm_device_enum_type rfm_device,
  rfcom_tdscdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type rf_tdscdma_exec_type,
  rf_buffer_intf* buffer_ptr,
  int16 script_timing  
)
{
  boolean success = TRUE;
  rfcom_band_type_u band_u;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  uint32 script_token =0;
  rfcom_antenna_tuner_priority_cfg tuner_priority_cfg;

  /* Get Power tracker object from RFC */
  qtuner_mgr_obj = (void**)rfc_tdscdma_get_rf_device_obj( rfm_device, 
                                                      RFC_CONFIG_RX, 
                                                      band, 
                                                      RFDEVICE_TUNER_MANAGER );

  band_u.tdscdma_band = band;
  tuner_priority_cfg.rfm_device = rfm_device;

  if (qtuner_mgr_obj)
  {
    while (qtuner_mgr_obj[device_idx]!= NULL)
    {
      /* Enable tuner manager's rx_mode */
        success &= rfcommon_atuner_manager_set_rx_mode(tuner_priority_cfg,
                                                        qtuner_mgr_obj[device_idx],
                                                        RFCOM_TDSCDMA_MODE,
                                                        band_u,
                                                        chan_num,
                                                        rf_tdscdma_exec_type,
                                                        buffer_ptr,
                                                        script_timing,
                                                        &script_token);

/* There is no way for RF to know when it gets executed by FW.
TFW just polls for common FW to check if update is available and executes it when appropriate time is available. 
Since an update occurs every 200 ms, FW should definitely find time to execute it before the next update*/

        if (success)
        {
          /*Ack if the script construction was successful */
          rfcommon_atuner_manager_acknowledge_script ( qtuner_mgr_obj[device_idx], 
                                                       script_token );
        }
      device_idx++; 
       // need to check and break for MAX DEVICE IDX
    }
  }
  else
  {
    success = FALSE;
    RF_MSG_2(RF_ERROR, "Qtuner_obj is NULL for Device %d and Band %d for Rx", rfm_device, band);
  }

  if(success)
  {
  	rf_tdscdma_QFE_TX_status = FALSE;
  }

  if(success && (rf_tdscdma_tuner_rf_more_f3_flag == TRUE))
  {         
    RF_MSG_1(RF_HIGH, "ANT_TUNER: Programmed in Rx mode on device %d", rfm_device);   
  }

  return success;
} /* rf_tdscdma_core_antenna_tuner_program_rx_mode */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Enable the QFE TX.

  @details
  Programs QFE tuner to set up TX path.

  @param device
  The device path to be enabled.

  @param band
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_program_tx_mode 
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band, 
  uint16 chan_num,
  rf_device_execution_type rf_tdscdma_exec_type,
  rf_buffer_intf* buffer_ptr,
  int16 script_timing  
)
{
  uint32 tx_freq = 0;
  boolean success = TRUE;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  rfcom_band_type_u band_u;
  uint32 script_token =0;
  rfcommon_atuner_cl_ctrl_info_type cl_ctrl_info;

  cl_ctrl_info.lm_handle = rf_tdscdma_mc_lm_bufs.rx_buf_wrk[RF_PATH_0].buf_idx; 

  /* Get Power tracker object from RFC */
  qtuner_mgr_obj = (void**)rfc_tdscdma_get_rf_device_obj(device, 
                                                         RFC_CONFIG_TX, 
                                                         band, 
                                                         RFDEVICE_TUNER_MANAGER );

  tx_freq = rf_tdscdma_core_util_get_freq_from_chan(chan_num);
  band_u.tdscdma_band=band;
     
  /*Need to convert the Tx freq in KHz to Hz */
  tx_freq = tx_freq * 1000;
     
  switch (band)
  {
    case RFCOM_BAND_TDSCDMA_B34:
      cl_ctrl_info.cl_nv_ctrl_info = &rfnv_tdscdma_b34_tx_tbl.ant_tuner_cl;
      break;

    case RFCOM_BAND_TDSCDMA_B39:
      cl_ctrl_info.cl_nv_ctrl_info = &rfnv_tdscdma_b39_tx_tbl.ant_tuner_cl;
      break;

    case RFCOM_BAND_TDSCDMA_B40:
      //cl_ctrl_info.cl_nv_ctrl_info = &rfnv_tdscdma_b40_tx_tbl.ant_tuner_cl;
      break;

    default:
      cl_ctrl_info.cl_nv_ctrl_info = NULL;
      RF_MSG_1(RF_ERROR, "In rf_tdscdma_core_antenna_tuner_program_tx_mode(), Invalid Band %d, NV pointer is NULL", band);
      break;
  }
	 
  if (qtuner_mgr_obj)
  {
    while (qtuner_mgr_obj[device_idx]!= NULL)
    {
      // set tuner tx mode */
      success &= rfcommon_atuner_manager_set_tx_mode(qtuner_mgr_obj[device_idx],
                                                     RFCOM_TDSCDMA_MODE, 
                                                     band_u,
                                                     chan_num,
                                                     rf_tdscdma_exec_type, 
                                                     buffer_ptr, 
                                                     script_timing,
                                                     &script_token,
                                                     &cl_ctrl_info);


      /* There is no way for RF to know when it gets executed by FW.
         TFW just polls for common FW to check if update is available and executes it when appropriate time is available. 
         Since an update occurs every 200 ms, FW should definitely find time to execute it before the next update*/

      if (success)
      {
        /*Ack if the script construction was successful */
        rfcommon_atuner_manager_acknowledge_script ( qtuner_mgr_obj[device_idx], 
                                                     script_token );
      }

      device_idx++;
    }
  }
  else
  {
    success = FALSE;
    RF_MSG_2(RF_ERROR, "Qtuner_obj is NULL for Device %d and Band %d for Tx", device, band);
  }

  if(success)
  {   
    rf_tdscdma_QFE_TX_status = TRUE;
    RF_MSG_2(RF_HIGH, "ANT_TUNER: Programmed in Tx mode on device %d, handle %d", device, cl_ctrl_info.lm_handle); 
  }
  else
  {
    RF_MSG_2(RF_HIGH, "ANT_TUNER: Programmed FAILED in Tx mode on device %d, handle %d", device, cl_ctrl_info.lm_handle); 
  }

  return success;
} /* rf_tdscdma_core_antenna_tuner_program_tx_mode */


/*---------------------------------------------------------------------------*/
/*!
  @brief
  Disable antenna tuner in tx mode

  @details
  Disable antenna tuner in tx mode
  
  @param device
  The device path to be disabled.  

  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_tx_mode_disable 
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band, 
  uint16 chan_num,
  rf_device_execution_type rf_tdscdma_exec_type,
  rf_buffer_intf* buffer_ptr,
  int16 script_timing
)
{
	uint32 tx_freq = 0;
  boolean success = TRUE;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  rfcom_band_type_u band_u;
  uint32 script_token =0;

  /* Get Power tracker object from RFC */
  qtuner_mgr_obj = (void**)rfc_tdscdma_get_rf_device_obj( device, 
                                                      RFC_CONFIG_TX, 
                                                      rf_tdscdma_mc_state.curr_band_tx, 
                                                      RFDEVICE_TUNER_MANAGER );
                                                      
  tx_freq = rf_tdscdma_core_util_get_freq_from_chan(chan_num);
  band_u.tdscdma_band=band;                                                      

  /*Need to convert the Tx freq in KHz to Hz */
  tx_freq = tx_freq * 1000;

  if (qtuner_mgr_obj)
  {
  	if (rf_tdscdma_QFE_TX_status)
  	{	
      while (qtuner_mgr_obj[device_idx]!= NULL)
      {
        /* set tuner tx mode */
        success &= rfcommon_atuner_manager_tx_disable ( qtuner_mgr_obj[device_idx],
                                                        RFCOM_TDSCDMA_MODE, 
                                                        band_u,
                                                        chan_num,
                                                        rf_tdscdma_exec_type, 
                                                        buffer_ptr, 
                                                        script_timing,
                                                        &script_token);

        device_idx++;
      }
    }
  }
  else
  {
    success = FALSE;
    RF_MSG_2(RF_ERROR, "Qtuner_obj is NULL for Device %d and Band %d for Tx", device, rf_tdscdma_mc_state.curr_band_tx);
  }

  if(success)
  {      
    RF_MSG_1(RF_HIGH,"ANT_TUNER: Tx mode disabled on device %d", device);    
  }

  return success;
} /* rf_tdscdma_core_antenna_tuner_tx_mode_disable */


/*----------------------------------------------------------------------------*/
/*!
  @brief
    rfwcdma_core_antenna_tuner_tune_code_override.
    This function is used to override the antenna tuner settings

  @param 

  @details
*/
uint32 rf_tdscdma_core_antenna_tuner_tune_code_override( rfm_device_enum_type device,
                                                         rfcom_tdscdma_band_type band,
                                                         uint8 override_flag,
                                                         void *data,
                                                         uint8* tuner_nv_ptr,
                                                         uint8* tuner_id_ptr)
{
  uint32 status = RFCMN_ATUNER_ERROR;
  uint8 device_idx = 0;
  void **qtuner_obj = NULL;
  rfcommon_atuner_intf* tuner_mgr = NULL;

  /* Get Tuner device object from RFC */
  qtuner_obj = (void**)rfc_tdscdma_get_rf_device_obj( device, 
                                                      RFC_CONFIG_RX, 
                                                      band, 
                                                      RFDEVICE_TUNER_MANAGER );
  
  if(qtuner_obj == NULL)
  {
    RF_MSG_1(RF_MED,"ANT_TUNER: No Anntenna tuner manager found from RFC for path %d", device);
    return(RFDEVICE_ANTENNA_TUNER_OVERRIDE_TUNER_NOT_PRESENT);
  }

  while (qtuner_obj[device_idx]!= NULL)
  {
    tuner_mgr = (rfcommon_atuner_intf*)qtuner_obj[device_idx];

    /* Override the tuner on the path of interest */
    status = rfcommon_atuner_tune_code_override( tuner_mgr,
                                                 override_flag,
                                                 data,
                                                 tuner_nv_ptr,
                                                 tuner_id_ptr);

    device_idx++;
  }

  RF_MSG_3(RF_HIGH,"ANT_TUNER: Antennat tuners overriden for the device path# %d, status: %d, tuner_num: %d", device, status, device_idx);

  return status;

} /*rf_tdscdma_core_antenna_tuner_tune_code_override*/

#if 0
/*---------------------------------------------------------------------------*/
/*!
  @brief
  Disable the QFE.

  @details
  Disable the QFE.

  @param device
  The device path to be disabled.

  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_disable 
(
  rfm_device_enum_type device
)
{
  boolean success = TRUE;
  void *qtuner_obj = NULL;

  /* Get Power tracker object from RFC */
  qtuner_obj = rfc_tdscdma_get_device( device, 
                                     RFC_CONFIG_RX, 
                                     rf_tdscdma_mc_state.curr_band_rx[device], 
                                     RFDEVICE_TUNER );

  /* Disable tuner */
  success &= rfdevice_antenna_tuner_disable(qtuner_obj,
                                            NULL, RFDEVICE_EXECUTE_IMMEDIATE);

  if(success)
  {       
    RF_MSG_1(RF_HIGH,"ANT_TUNER: Disabled on device path#%d", device);    
  }
  return success;
} /* rf_tdscdma_core_antenna_tuner_disable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Do QFE antenna tuner standalone pll tuning

  @details
  Do QFE antenna tuner standalone pll tuning

  @param device
  The device path to set

  @param band
  enum indicating band number  

  @param chan
  channel number 
 
  @param buff_obj_ptr
  void pointer to CCS buffer object.
  Note: If this pointer is NULL, then the tune code word will be directly programmed to
        tuner hardware. 
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_program_scenario_standalone
( 
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  uint16 chan,
  void *buff_obj_ptr
)
{
  rfcom_band_type_u band_u;
  boolean success = TRUE;

  if( band >= RFCOM_NUM_TDSCDMA_BANDS )
  {
    /* Check if Invalid band passed */
    RF_MSG_1(RF_HIGH,"ANT_TUNER: Invalid band passed %d", band);    
    return FALSE;
  }
 
  /* Program tuner scenario */
  band_u.tdscdma_band = band;
  success &= rfcommon_antenna_tuner_get_standalone_tune_script(
                                      device,         /* device*/
                                      RFM_IMT_MODE,   /* mode*/
                                      TRUE,           /* is operation Rx */
                                      band_u,         /* band */
                                      chan,           /* dl_chan */
                                      buff_obj_ptr);  /* no script and execute now */
  
  if(success)
  {    
    RF_MSG_1(RF_HIGH,"ANT_TUNER: Programmed Scenario on device path#%d",device);    
  }
  return success;
} /* rf_tdscdma_core_antenna_program_scenario */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Inform WL1 a new scneario occurs.

  @details
  Inform WL1 a new scneario occurs by calling the record call back function.

  @param 
 
  @return

*/
void rf_tdscdma_core_antenna_tuner_notify_new_scneario (void)
{
  if(rf_tdscdma_core_tuner_wl1_cb_ptr != NULL)
  {
    if(enable_wl1_cb_fn_flag)
    {
      rf_tdscdma_core_tuner_wl1_cb_ptr();
      enable_wl1_cb_fn_flag = FALSE;
      RF_MSG_1(RF_HIGH,"ANT_TUNER: WL1 call back function called",0);
    }
    else
    {
      RF_MSG_1(RF_HIGH,"ANT_TUNER: WL1 call back function already called",0);
    }
  }
  else
  {
    RF_MSG_1(RF_HIGH,"ANT_TUNER: WL1 call back function pointer is NULL",0);
  }
} /*rf_tdscdma_core_antenna_tuner_notify_wl1_new_scneario*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides antenna tuner code word script for meas pll tuning.

  @details
  Provides antenna tuner code word script for meas pll tuning.
  And notify WL1 if there is a new scenario in meas.

  @param is_tune_away_script
  This argument is for meas script type;  
    If TRUE : the script requested is for "tune away" case or tuning to target tech
    If FALSE: the script requested is for "tune back" case or tuning to source tech

  @param device_info
  Active device information

  @param num_of_devices_active
  num_of_devices_active

  @param buf_obj_ptr
  meas script buffer pointer
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_get_meas_script
( 
  boolean is_tune_away_script,
  rfcommon_core_antenna_tuner_device_info_type *device_info,
  uint8 num_of_devices_active,
  void *buf_obj_ptr
)
{
  boolean success = TRUE;

  boolean new_scenario_in_meas = FALSE;

  success &= rfcommon_antenna_tuner_get_meas_tune_script(
                                        RFM_IMT_MODE,            /* mode*/
                                        is_tune_away_script,     /* Meas script type */
                                        TRUE,                    /* is operation Rx */
                                        num_of_devices_active,   /* num_of_devices_active */
                                        device_info,             /* device_info */
                                        &new_scenario_in_meas,   /* new_scenario_in_meas */
                                        buf_obj_ptr);            /* buffer object */

  /*Check if a new scenario in the script*/
  if(new_scenario_in_meas)
  {
    rf_tdscdma_core_antenna_tuner_notify_new_scneario();
  }
  else
  {
    RF_MSG_1(RF_HIGH,"ANT_TUNER: No new scenario in meas",0);
  }

  return success;
} /* rf_tdscdma_core_antenna_program_scenario_immediately */

/******************************************************************************
	                              FTM Only APIs
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*!
  @brief
  FTM API to set tuner scenario immediately

  @details
  This API to be called only in FTM. API to set the tuner scenario immediately
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_program_scenario_immediately(void)
{
  boolean success = TRUE;
  rfcom_band_type_u band_u;

  /* Enforce Scenario on QFE device path: RFM_DEVICE_0 */
  if( ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RX ) ||
      ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX ) )
  {
    /* Program tuner scenario */
    band_u.tdscdma_band = rf_tdscdma_mc_state.curr_band_rx[RF_PATH_0];
    success &= rfcommon_antenna_tuner_get_standalone_tune_script(
                                     RFM_DEVICE_0,                /* device*/
                                     RFM_IMT_MODE,                /* mode*/
                                     TRUE,                        /* is operation Rx */
                                     band_u,                      /* band */
                                     rf_tdscdma_mc_state.rf_chan_rx, /* dl_chan */
                                     NULL);                       /* no script and execute now */
    RF_MSG_1(RF_MED,"ANT_TUNER: Scenario Enforced for Path-0", 0);
  }

  /* Enforce Scenario on QFE device path: RFM_DEVICE_1 */
  if( ( rf_tdscdma_mc_state.rf_state[RF_PATH_1] == RF_TDSCDMA_STATE_RX ) ||
      ( rf_tdscdma_mc_state.rf_state[RF_PATH_1] == RF_TDSCDMA_STATE_RXTX ) )
  {
    /* Program tuner scenario */
    /*<< During rf_tdscdma_enter_mode current band for all paths are made invalid hence
         use curr_band_rx from PATH_0 */
    band_u.tdscdma_band = rf_tdscdma_mc_state.curr_band_rx[RF_PATH_0];
    success &= rfcommon_antenna_tuner_get_standalone_tune_script(
                                     RFM_DEVICE_1,                /* device*/
                                     RFM_IMT_MODE,                /* mode*/
                                     TRUE,                        /* is operation Rx */
                                     band_u,                      /* band */
                                     rf_tdscdma_mc_state.rf_chan_rx, /* dl_chan */
                                     NULL);                       /* no script and execute now */

    RF_MSG_1(RF_MED,"ANT_TUNER: Scenario Enforced for Path-1", 0);
  }

  return success;
} /* rf_tdscdma_core_antenna_program_scenario_immediately */

/******************************************************************************
	                           WL1 INTERFACE APIs
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*!
  @brief
  rf_tdscdma_core_antenna_tuner_incremental_update

  @details
  This API should be called  by rfm_tdscdma_antenna_tuner_tick()
  Inside this API check all the active paths then give a call to
  rfcommon_antenna_tuner_incremental_update
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_incremental_update ( void )
{
  boolean success = TRUE;
  rfcom_band_type_u band_u;
  uint8 num_of_devices_active = 0;
  rfcommon_core_antenna_tuner_device_info_type active_device_list[RFCMN_ANT_TUNER_MAX_TECH_DEVICES];

  /* Disable the WL1 call back function */
  rf_tdscdma_core_tuner_wl1_cb_ptr = NULL;
  enable_wl1_cb_fn_flag = FALSE;

  /* Check all the active paths */
  if( ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RX ) ||
      ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX ) )
  {
    band_u.tdscdma_band = rf_tdscdma_mc_state.curr_band_rx[RF_PATH_0];
    active_device_list[num_of_devices_active].device = RFM_DEVICE_0;
    active_device_list[num_of_devices_active].band = band_u;
    active_device_list[num_of_devices_active].chan = rf_tdscdma_mc_state.rf_chan_rx;
    num_of_devices_active++;
  }

  if( ( rf_tdscdma_mc_state.rf_state[RF_PATH_1] == RF_TDSCDMA_STATE_RX ) ||
      ( rf_tdscdma_mc_state.rf_state[RF_PATH_1] == RF_TDSCDMA_STATE_RXTX ) )
  {
    band_u.tdscdma_band = rf_tdscdma_mc_state.curr_band_rx[RF_PATH_1];
    active_device_list[num_of_devices_active].device = RFM_DEVICE_1;
    active_device_list[num_of_devices_active].band = band_u;
    active_device_list[num_of_devices_active].chan = rf_tdscdma_mc_state.rf_chan_rx;
    num_of_devices_active++;
  }

  /*Meas period has ended as the incremental updates has started*/
  success &= rfcommon_antenna_tuner_enable_new_scenario_notify( RFM_TDSCDMA_MODE,
                                                                FALSE );
  
  /* Programs antenna tuner code word script for incremental updates */	  
  success &= rfcommon_antenna_tuner_incremental_update( 
                                       RFM_IMT_MODE,           /* mode*/
                                       TRUE,                   /* is operation Rx*/
                                       num_of_devices_active,  /* num of devices*/
                                       active_device_list);    /* device info*/

  if(success)
  { 
    RF_MSG_1(RF_HIGH,"ANT_TUNER: Scenario updated incrementally", 0);
  }
  return success;
} /* rf_tdscdma_core_antenna_tuner_incremental_update */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Register a call back function

  @details
  This functions registers a call back function that RFSW can use to inform
  WL1 when a new scneario occurs.
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_meas_cb_register 
( 
  rfm_new_scenario_cb_type* new_scenario_cb
)
{
  boolean success = TRUE;
  boolean new_scenario = FALSE;
  rfcom_band_type_u band_u;

  if (new_scenario_cb != NULL)
  {
    /* Enable the WL1 call back function */
    rf_tdscdma_core_tuner_wl1_cb_ptr = new_scenario_cb;
    enable_wl1_cb_fn_flag = TRUE;

    RF_MSG_1(RF_HIGH,"ANT_TUNER: WL1 meas call back fn registed", 0);

    /*Meas period has started */
    success &= rfcommon_antenna_tuner_enable_new_scenario_notify( RFM_TDSCDMA_MODE,
                                                                  TRUE );
    
    /* Check if there is any new/pending scenario when the meas period
       starts */
    if( ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RX ) ||
        ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX ) )
    {
      band_u.tdscdma_band = rf_tdscdma_mc_state.curr_band_rx[RF_PATH_0];
      new_scenario = rfcommon_antenna_tuner_check_new_scenario( RFM_DEVICE_0,
                                                                RFM_TDSCDMA_MODE,
                                                                FALSE,
                                                                band_u);
    }
    else
    {
      RF_MSG_1(RF_ERROR,"ANT_TUNER: Meas API called when TDSCDMA Driver not in valid state!", 0);
      return FALSE;
    }

    if( new_scenario )
    {
      /* New scenario found, inform WL1 with the same so that WL1 can update all the 
         W2W buffers */
      rf_tdscdma_core_antenna_tuner_notify_new_scneario();
    }
  }
  else
  {
    RF_MSG_1(RF_ERROR,"ANT_TUNER: WL1 meas call back fn is NULL", 0);
    return FALSE;
  }

  return success;
} /* rf_tdscdma_core_antenna_tuner_meas_cb_register */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Acknowledgement from meas script caller when script is successful

  @details
  Acknowledgement from meas script caller when the meas script is executed
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rf_tdscdma_core_antenna_tuner_meas_script_success( void )
{
  boolean success = TRUE;

  success = rfcommon_antenna_tuner_meas_script_success(RFM_TDSCDMA_MODE, TRUE, RFM_DEVICE_0);
  /* Enable the WL1 call back function */
  enable_wl1_cb_fn_flag = TRUE;

  return success;
} /* rf_tdscdma_core_antenna_tuner_meas_script_success */
#endif


#endif /* FEATURE_RF_HAS_QTUNER */

