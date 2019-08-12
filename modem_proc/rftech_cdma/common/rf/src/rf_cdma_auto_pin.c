/*!
  @file
  rf_cdma_auto_pin.c

  @brief
  Provides CDMA Auto Pin functionality

*/

/*===========================================================================

Copyright (c) 2008 -2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:55 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_auto_pin.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/18/16   vr      Update Autopin log packet with PIN ref value
12/29/15   vr      Clear script buffer if Autopin is not supported
12/20/15   vr      Autopin check-in
08/31/15   wwl     Add autopin c2k state machine support
06/26/15   wwl     Initial version.

============================================================================*/
#include "rflm_c2k_msg.h"
#include "rf_cdma_data.h"
#include "rf_task.h"
#include "rf_cdma_tx_agc.h"
#include "rflm_c2k_mc.h"
#include "rf_cdma_constants.h"
#include "rf_cdma_mc.h"
#include "ftm.h"
#include "rfcommon_autopin.h"
#include "rf_cdma_auto_pin.h"
#include "rflm_dm_api.h"
#include "fw_autopin_intf.h"
#include "rfcommon_autopin_msm.h"
#include "rflm_api_fbrx_fw.h"


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Auto Pin Read Handler function

  @details
  This function will be called by RF APPS task dispatcher when auto pin
  command is posted to the command Q beloging to RF APPS task. This function
  will read the PAR from WTR.

  @param result 
  pointer to RFLM autopin data buffer
  
  @return TRUE for success
*/

boolean
rf_cdma_autopin_trigger_read
(
  
)
{
  rfm_device_enum_type tx_device;
  rfdevice_trx_cdma_tx* device_ptr = NULL;
  rfdevice_rxtx_common_class* cmn_device_ptr = NULL;
  uint8 device_idx = 0;
  const rfc_cdma_logical_device_params_type *logical_dev = NULL;
  const rf_cdma_data_status_type *dev_r = NULL;
  boolean status = FALSE;

  rfcommon_autopin_capture_control_type control_input = { 0 };
  


  RF_MSG( RF_ERROR, "rf_cdma_mc_autopin_trigger_read: enter" );

   /* Get the current Tx device*/
  tx_device = rf_cdma_data_get_curr_tx_device();

  logical_dev = rfc_cdma_get_logical_device_params( tx_device );

  dev_r = rf_cdma_get_device_status( tx_device );

  if (logical_dev == NULL || dev_r == NULL)
  {

	RF_MSG_1( RF_ERROR, "rf_cdma_mc_autopin_trigger_read: Dev %d - NULL "
                        "device or RFC data", tx_device );
   return FALSE;

  }

  device_ptr = logical_dev->tx_device[dev_r->curr_band];

  if (device_ptr == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_autopin_trigger_read: dev_ptr %d - NULL "
						  "device or RFC data", tx_device );
	return FALSE;
  } 
  
  cmn_device_ptr = rfdevice_cdma_tx_get_common_device_pointer(device_ptr);

  if (cmn_device_ptr == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_autopin_trigger_read: cmn_dev_ptr %d - NULL "
						  "device or RFC data", tx_device );
	return FALSE;
  } 

  status = rfdevice_cmn_fbrx_autopin_read_and_disable(cmn_device_ptr,
                                             &control_input.wtr_capture_result,
                                             FALSE,
                                             NULL);

  control_input.capture_type = AUTOPIN_PAR_WTR_CAPTURE;
  control_input.valid_capture = status;
  control_input.par = control_input.wtr_capture_result.par;
  
  
  rfcommon_autopin_capture_control(dev_r->txlm_handle,
                                   control_input);

   
  return TRUE;
  
}





/*----------------------------------------------------------------------------*/
/*!
  @brief
  Auto Pin update Handler function

  @details
  This function will be called by RF APPS task dispatcher when auto pin
  command is posted to the command Q beloging to RF APPS task. This function
  will perform Linearizer update and applies Pin value

  @param cmd_ptr 
  pointer to RFLM command buffer

  @param cid_info
  command ID info

  @param cb_data
  data buffer that carries command content
*/
void
rf_cdma_auto_pin_read_handler
(
  void *cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data
)
{
  boolean ret = FALSE;
  
  ret = rf_cdma_autopin_trigger_read( );

  if (ret == FALSE)
  {
	 RF_MSG(RF_ERROR, "rf_cdma_auto_pin_read_handler: Failed!!!");
	 return;
  }

  
} /* rf_cdma_auto_pin_update_handler */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Init C2K autopin module
*/
void
rf_cdma_auto_pin_cmd_proc_init
(
  void
)
{
  rfm_device_enum_type dev;
  rf_cdma_data_status_type *dev_status = NULL;

  for(dev = RFM_DEVICE_0; dev < RFM_MAX_DEVICES; dev++)
  {
    dev_status = rf_cdma_get_mutable_device_status( dev );
    if(NULL != dev_status)
    {
      dev_status->autopin_st.tx_enabled = FALSE;
      dev_status->autopin_st.pin_data_valid = FALSE;
      dev_status->autopin_st.wakeup_flag = 0;
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update internal C2K autopin state when TX is enabled

  @param tx_dev
  TX device
*/
void
rf_cdma_auto_pin_tx_wakeup
(
  rfm_device_enum_type tx_dev,
  rfm_mode_enum_type rfm_tech
)
{
  rf_cdma_data_status_type *dev_status = NULL;
  dev_status = rf_cdma_get_mutable_device_status( tx_dev );

  if(NULL != dev_status)
  {
    if(0 == dev_status->autopin_st.wakeup_flag)
    {
      /* if this is the first wakeup on the device */
      dev_status->autopin_st.tx_enabled = TRUE;
      dev_status->autopin_st.pin_data_valid = FALSE;
    }
    dev_status->autopin_st.wakeup_flag |= (1 << rfm_tech);
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update internal C2K autopin state when TX is being put into sleep

  @param tx_dev
  TX device
*/
void
rf_cdma_auto_pin_tx_sleep
(
  rfm_device_enum_type tx_dev,
  rfm_mode_enum_type rfm_tech
)
{
  rf_cdma_data_status_type *dev_status = NULL;
  dev_status = rf_cdma_get_mutable_device_status( tx_dev );

  if(NULL != dev_status)
  {
    dev_status->autopin_st.wakeup_flag &= ~(1 << rfm_tech);

    if(0 == dev_status->autopin_st.wakeup_flag)
    {
      /* no tx for both 1X and HDR */
      dev_status->autopin_st.tx_enabled = FALSE;
      dev_status->autopin_st.pin_data_valid = FALSE;
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check to see if there is valid Pin value from autopin module

  @param tx_dev
  Auto Pin TX device
*/
boolean
rf_cdma_auto_pin_data_valid
(
  rfm_device_enum_type tx_dev
)
{
  boolean ret = FALSE;
  rf_cdma_data_status_type *dev_status = NULL;
  dev_status = rf_cdma_get_mutable_device_status( tx_dev );

  if(NULL != dev_status)
  {
    ret = dev_status->autopin_st.pin_data_valid;
  }
  return ret;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get current autopin tx state for given TX dev

  @param tx_dev
  Autopin tx device
*/
boolean
rf_cdma_auto_pin_tx_state
(
  rfm_device_enum_type tx_dev
)
{
  boolean ret = FALSE;
  rf_cdma_data_status_type *dev_status = NULL;
  dev_status = rf_cdma_get_mutable_device_status( tx_dev );

  if(NULL != dev_status)
  {
    ret = dev_status->autopin_st.tx_enabled;
  }
 
  return ret;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Scrpt building for Autopin in every Tx wakeup
 
  @details  
    
  @param device
  Logical device ( Used here to get CCS event handle )
 
  @param mode  
  rfm_moe type

  @param logical_dev
  Pointer to logical dev params

  @param band
  Current band of operation
 
  @return  
  TRUE on successful getting the scripts. FALSE implies scripts
  generation failed.
*/

boolean rf_cdma_configure_autopin_trigger
(

 rfm_device_enum_type device,
 rfm_mode_enum_type mode,
 const rfc_cdma_logical_device_params_type* logical_dev,
 rfm_cdma_band_class_type band
)

{
   
  /* Buffr object to hold PA scripts */
  rf_buffer_intf *autopin_trigger_script_g1 = NULL;
  rf_buffer_intf *autopin_trigger_script_g2 = NULL;
  rfdevice_rxtx_common_class* cmn_device_ptr = NULL;

  boolean result = TRUE;

  cmn_device_ptr = rfdevice_cdma_tx_get_common_device_pointer(logical_dev->tx_device[band]);

  if (cmn_device_ptr == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_autopin_trigger_read: cmn_dev_ptr %d - NULL "
						  "device or RFC data", device );
	return FALSE;
  } 

   /* Create generic buffer interface object for SSBI,
     allocate memory to max script sizes, since the memory is released when 
     object is destroyed later on anyway, APIs returning script size on the fly 
     aren't ready, all driver APIs hard coding it internally */
  autopin_trigger_script_g1 = rf_buffer_create ( 0 , 
                                        RF_CDMA_MAX_RFFE_SCRIPT_SIZE , 
                                        0 ) ;
     
  /*Check if valid pointer was returned for buffer object*/
  if( autopin_trigger_script_g1 == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_tx_wakeup: "
					 "Unable to create buffer object ");
    result = FALSE;
  }/*if( wakeup_tx_script == NULL )*/


  /*-----------------------------------------------------------------------*/
  /* compute RF device script for Autopin G1and push it to firmware */
  
   result &= rfdevice_cmn_create_auto_pin_trigger_script( cmn_device_ptr,
					  autopin_trigger_script_g1,
					  (uint8)AUTOPIN_SCRIPT_G1,
			 		  WTR_DEBUG_MEM_SIZE,
                                          wtr_debug_mem_enable);
  
   if (result == FALSE)
   {
	RF_MSG ( RF_ERROR , "rfdevice_cmn_create_auto_pin_trigger_script:" 
	"rfdevice_cmn_fbrx_autopin_enable failed:");
	/*destroy buffer after use*/
    if ( autopin_trigger_script_g1 != NULL )
    {
       rf_buffer_destroy( autopin_trigger_script_g1 );
    }
  	return result;
   }
  
   /* Get handle to the CCS event */
   rf_cdma_mdsp_event_id_type event_id = RF_CDMA_MDSP_CCS_EVENT_AUTOPIN_TRIGGER_G1_ID ;
   rf_cdma_event_type event_type = RF_CDMA_EVENT_AUTOPIN_TRIGGER_G1_ID ;
   rfcommon_mdsp_event_handle** analog_script_event_handle = NULL ;
   rfcommon_mdsp_event_handle** analog_script_event_handle_2 = NULL ;
  
   analog_script_event_handle =
	   rf_cdma_data_get_event_handle( device, event_type );
  
   /* Check for NULL */
   if ( analog_script_event_handle == NULL )
   {
	  RF_MSG_1 ( RF_ERROR  , "rfdevice_cmn_create_auto_pin_trigger_script: "
				   "NULL event handle returned for event:%d " ,
				   event_type ) ;
	  result = FALSE;
   }
   else
   {
	 /* Push Analog frontend Script to MDSP */
	 result &= rf_cdma_mdsp_configure_devices( device ,
				  logical_dev->tx_mdsp_path,
				  mode,
				  event_id ,
				  analog_script_event_handle,
				  (rf_cdma_mdsp_get_event_info(event_id)),
				  autopin_trigger_script_g1, 
				  0,
				  rf_cdma_data_get_txlm_handle (device) ,
				  NULL, NULL );
	 
   }/* if! ( analog_script_event_handle == NULL ) */	

	
   /*destroy buffer after use*/
   if ( autopin_trigger_script_g1 != NULL )
   {
      rf_buffer_destroy( autopin_trigger_script_g1 );
    }

 /*************************************************************************************************/

   /* Create generic buffer interface object for SSBI/RFFE/GRFC,
     allocate memory to max script sizes, since the memory is released when 
     object is destroyed later on anyway, APIs returning script size on the fly 
     aren't ready, all driver APIs hard coding it internally */
  autopin_trigger_script_g2 = rf_buffer_create ( 0 , 
                                        RF_CDMA_MAX_RFFE_SCRIPT_SIZE , 
                                        0 ) ;
   
  /*Check if valid pointer was returned for buffer object*/
  if( autopin_trigger_script_g2 == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_mc_configure_front_end_for_tx_wakeup: "
					 "Unable to create buffer object ");
    result = FALSE;
  }/*if( wakeup_tx_script == NULL )*/


  /*-----------------------------------------------------------------------*/
  /* compute RF device script for TX enable and push it to firmware */
  
   result &= rfdevice_cmn_create_auto_pin_trigger_script( cmn_device_ptr,
				  autopin_trigger_script_g2,
    				  (uint8)AUTOPIN_SCRIPT_G2,
    				  WTR_DEBUG_MEM_SIZE,
                                  wtr_debug_mem_enable);
    											  
  if (result == FALSE)
  {
	RF_MSG ( RF_ERROR , "rfdevice_cmn_create_auto_pin_trigger_script:" 
	"rfdevice_cmn_fbrx_autopin_enable failed:");
	
	if (autopin_trigger_script_g2 != NULL)
    {
      rf_buffer_destroy( autopin_trigger_script_g2 );
    }
  	return result;
  }
  
   /* Get handle to the CCS event */
   analog_script_event_handle_2 = NULL ;
  
   analog_script_event_handle_2 =
	   rf_cdma_data_get_event_handle( device, RF_CDMA_EVENT_AUTOPIN_TRIGGER_G2_ID );
  
   /* Check for NULL */
   if ( analog_script_event_handle_2 == NULL )
   {
	  RF_MSG_1 ( RF_ERROR  , "rfdevice_cmn_create_auto_pin_trigger_script: "
				   "NULL event handle returned for event:%d " ,
				   RF_CDMA_EVENT_AUTOPIN_TRIGGER_G2_ID ) ;
	  result = FALSE;
   }
   else
   {
	 /* Push Analog frontend Script to MDSP */
	 result &= rf_cdma_mdsp_configure_devices( device ,
				  logical_dev->tx_mdsp_path,
				  mode,
				  RF_CDMA_MDSP_CCS_EVENT_AUTOPIN_TRIGGER_G2_ID ,
				  analog_script_event_handle_2,
				  (rf_cdma_mdsp_get_event_info(RF_CDMA_MDSP_CCS_EVENT_AUTOPIN_TRIGGER_G2_ID)),
				  autopin_trigger_script_g2, 
				  0,
				  rf_cdma_data_get_txlm_handle (device) ,
				  NULL, NULL );
	 
	}/* if! ( analog_script_event_handle == NULL ) */	

if (autopin_trigger_script_g2 != NULL)
{
  rf_buffer_destroy( autopin_trigger_script_g2 );
}

RF_MSG_1 ( RF_ERROR  , "rfdevice_cmn_create_auto_pin_trigger_script: "
			 "iscompleted for :%d " ,
			 device ) ;


return result;
  
 }

 boolean
 rf_cdma_autopin_stop
 ( 
   rfm_device_enum_type device
   //rf_cdma_hdet_data_type* hdet_data
 )
 {
 
   if (ftm_get_mode() == FTM_MODE)
   {
      RF_MSG_1( RF_MED, "rf_cdma_autopin_stop: FTM mode return for device %d,",
			   device);
   }
 
   /* Always clean up HDET trig event smem and event handle */
   rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_AUTOPIN_TRIGGER_G1_ID );
 
   rf_cdma_mc_cleanup_ccs_event( device, RF_CDMA_EVENT_AUTOPIN_TRIGGER_G2_ID );
	 
   RF_MSG_1( RF_MED, "rf_cdma_autopin_stop: Autopin operation stopped for device %d,",
				 device);
	 
   return TRUE;
 
 
 }

 boolean
 rf_cdma_autopin_update_dm
 (
   rfm_device_enum_type device,
   const rfc_cdma_logical_device_params_type* logical_dev,
   rfm_mode_enum_type rfm_tech,
   const lm_handle_type txlm_handle,
   rfm_cdma_band_class_type band,
   uint16 chan
 )
 {
   double fbrx_adc_clk;

   fw_autopin_capture_input_t *reflog_capture_input;
   uint16 *wtr_capture_size;
   rfdevice_rxtx_common_class* device_ptr = NULL;
   uint32 txc_rate_ustmr;
   uint32 new_delay;
   uint32 extra_capture_time;

      
   rflm_autopin_dm_template_t *autopin_template = NULL;
   rflm_tech_id_t tech = rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_tech);

   rflm_c2k_auto_pin_data_t *autopin_data;

   rflm_c2k_data_t* rflm_cdma_data_ptr = rflm_c2k_get_data();

   device_ptr = rfdevice_cdma_tx_get_common_device_pointer(logical_dev->tx_device[band]);

   if ( device_ptr == NULL || rflm_cdma_data_ptr == NULL )
   {
     RF_MSG_1( RF_ERROR, "rf_cdma_mc_autopin_trigger_read: cmn_dev_ptr or rflm_c2k_ptr %d - NULL "
						  "device or RFC data", device );
	 return FALSE;
   } 

   
if (!rfcommon_autopin_is_enabled(rfm_tech))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "AutoPin is not enabled for tech %d",
          rfm_tech);
    
    return RFCOMMON_AUTOPIN_ERROR;
  }

  autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(txlm_handle);

  if (autopin_template == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Failed to get AutoPin buffer from DM for tech %d, Tx handle %d",
          rfm_tech, txlm_handle);
    
    return RFCOMMON_AUTOPIN_ERROR;
  }

  if (rfm_tech == RFM_1X_MODE)
  {

	autopin_data = &(rflm_cdma_data_ptr->onex_data.autopin_data);
		
  }
  else
  {

	autopin_data = &(rflm_cdma_data_ptr->hdr_data.autopin_data);

  }
  reflog_capture_input = &autopin_template->autopin_ctl.reflog_capture_input;
  wtr_capture_size = &autopin_template->autopin_ctl.wtr_capture_size;

 
  rfcommon_msm_get_txc_rollover_period(txlm_handle,
                                         &reflog_capture_input->txc_input_freq_hz, 
                                         &reflog_capture_input->txc_rate_ns,
                                         &reflog_capture_input->txrot_clk_hz);

 

 RF_MSG_3(RF_HIGH, "TXC param: input freq = %d, txc rate = %d, clk rate = %d",
					  reflog_capture_input->txc_input_freq_hz, reflog_capture_input->txc_rate_ns, reflog_capture_input->txrot_clk_hz);


	  
 rfcommon_msm_get_par_capture_params(tech,
                                &(reflog_capture_input->capture_length),
                                &(reflog_capture_input->txc_down_sampling_rate), 
                                &(reflog_capture_input->tx_capture_delay),
                                 wtr_capture_size);

 /* calculate CDMA specific timing params*/

 txc_rate_ustmr = (uint32)(19200000 / 1000000.0 * reflog_capture_input->txc_rate_ns / 1000);

 extra_capture_time = (reflog_capture_input->tx_capture_delay/txc_rate_ustmr)*txc_rate_ustmr;

 reflog_capture_input->tx_capture_delay -= extra_capture_time;

 //Update the extern capture delay to rflm
 autopin_data->delay_in_capture_time = extra_capture_time;
 

 RF_MSG_3(RF_ERROR, "txc_roll_over: %d extra_capt_time:%d tx_capt_delay: %d ", txc_rate_ustmr, autopin_data->delay_in_capture_time, reflog_capture_input->tx_capture_delay);

 if (rfdevice_cmn_get_fbrx_adc_clk_freq(device_ptr,
					   rfm_tech,	   
					   band, 
					   chan, 
					   &fbrx_adc_clk))
 {

	*wtr_capture_size = (uint16)(AUTOPIN_PAR_CAPTURE_WINDOW_CDMA*fbrx_adc_clk/6);

  }
  else
  {
	RF_MSG(RF_ERROR, "rfdevice_cmn_get_fbrx_adc_clk_freq: Failed");
	   
  }
  RF_MSG_1(RF_ERROR, "wtr_capture_size: %d", *wtr_capture_size);
	
			 
  RF_MSG_3(RF_HIGH, "TXC param: REFLOG capture size = %d, downsample rate = %d, capture delay = %d", 
					   reflog_capture_input->capture_length, 
					   reflog_capture_input->txc_down_sampling_rate, 
					   reflog_capture_input->tx_capture_delay);
		
  RF_MSG_1(RF_HIGH, "rf_cdma_autopin_update_dm done for : tx handle %d ", txlm_handle);


 return TRUE;

 }

 
 /*----------------------------------------------------------------------------*/


boolean
rf_cdma_autopin_par_support
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_mode_enum_type rfm_tech,
  rfm_cdma_band_class_type band  
)
{
  boolean ret_status = TRUE;
  rfdevice_rxtx_common_class* device_ptr = NULL;

  rflm_c2k_data_t* rflm_cdma_data_ptr = rflm_c2k_get_data();
  boolean *auto_pin_par_supported = NULL;
  
  device_ptr = rfdevice_cdma_tx_get_common_device_pointer(logical_dev->tx_device[band]);

  if (device_ptr == NULL || rflm_cdma_data_ptr == NULL)
  {
	RF_MSG_1( RF_ERROR, "rf_cdma_mc_autopin_trigger_read: cmn_dev_ptr %d - NULL "
						 "device or RFC data", device );
	return FALSE;
  } 

  if (rfm_tech == RFM_1X_MODE)
  {
	auto_pin_par_supported = &rflm_cdma_data_ptr->onex_data.autopin_data.autopin_par_support;
  }
  else
  {
	auto_pin_par_supported = &rflm_cdma_data_ptr->hdr_data.autopin_data.autopin_par_support;
  }
  
  if ( *auto_pin_par_supported == FALSE )
  {

    ret_status = rfdevice_cmn_get_autopin_par_support(device_ptr, auto_pin_par_supported);

    if (ret_status == FALSE)
    {
	  RF_MSG_1( RF_ERROR, "rf_cdma_autopin_par_support: failed for the current WTR on dev:%d",device);
	  return FALSE;
    }
   
   }
 
  RF_MSG_1( RF_ERROR, "rf_cdma_autopin_par_support: auto_pin_par_supported :%d", *auto_pin_par_supported);
  
  return ret_status;
}

void rf_cdma_autopin_update_pin
(
  uint32 handle,
  uint8 pa_state,
  int16 pin_accum
 )
{

 rfm_device_enum_type device = RFM_DEVICE_0;
 const rfc_cdma_logical_device_params_type* logical_dev;
 rf_cdma_data_status_type *dev_status_w; 
 int16 pin_comp_offset =0;
 rflm_autopin_dm_template_t *autopin_template = NULL;

 
 device = rf_cdma_data_get_curr_tx_device();

 dev_status_w = rf_cdma_get_mutable_device_status( device );

 /* Get Logical Device Param */
 logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL pointer check */
  if ( dev_status_w == NULL ||logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_autopin_update_pin: "
                        "Null device data for device %d ",device);
    return ;
  }/*if ( dev_status_w == NULL )*/

  
  if(rf_cdma_autopin_par_support( device,
  				logical_dev,
				dev_status_w->rf_mode,
				dev_status_w->curr_band) == TRUE)
  	{

      if(dev_status_w->autopin_st.tx_enabled)
      {
        dev_status_w->autopin_st.pin_data_valid = TRUE;
      }
      /* Get the Pin NV value and use it as the init value for AutoPin convergence
       * rf_tdscdma_core_autopin_pin_comp_init[] was populated in rf_tdscdma_core_temp_comp_vs_freq_get_comp*/
      autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(handle);
      if (autopin_template == NULL)
      {
        RF_MSG_2(RF_ERROR,"rf_cdma_autopin_update_pin: Failed to get AutoPin buffer from DM for Tx handle %d, can't log ref pin %d", 
                       handle, dev_status_w->pin_comp_val_temp_vs_freq[pa_state]);
      }
      else
      {
        autopin_template->autopin_ctl.pin_init = dev_status_w->pin_comp_val_temp_vs_freq[pa_state];
      }
      RF_MSG_2( RF_HIGH, "rf_cdma_autopin_update_pin: :%d NV pin :%d", pin_accum,dev_status_w->pin_comp_val_temp_vs_freq[pa_state] );
	  
      pin_comp_offset = pin_accum + dev_status_w->pin_comp_val_temp_vs_freq[pa_state];
	  
	  
      rf_cdma_tx_agc_autopin_update(device, 
			   pin_comp_offset,
                           pa_state ); 

  	}

  


}


