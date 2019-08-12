/*!
  @file ftm_gsm_meas.c

  @brief
  This file contains all declarations and definitions necessary to use
  the gsm FTM Measurement Main Control.

  @details

*/

/*===========================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/src/ftm_gsm_meas.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/14   vs      Fix linker errors on W/T compile out flavors
10/23/14   sc      Enable event type control from RFSW
10/21/14   sw      Update irat source tech information with valid data (from QRCT)
06/11/14   hk      Fixing OFT compiler errors
03/21/14   cj      Chnages for W2G on Bolt
03/12/14   cj      Removing FW dependency
02/10/14   cj      g2x timing updated
02/03/14   cj      num_source_item added for DSDS
08/05/13   cj      Implementation added to receive GSM RSSI in meas mode
04/19/13   cj      Added header change support for CA changes
04/11/13   tws     Remove FEATURE_DUAL_ACTIVE featurization
03/12/13   cj      Removing app mode config API for OFT
03/12/13   cj      Removed call to GL1 functions for OFT till the availability
                     of overload APIs
03/10/13   cj      Added rxlm buffer execution and debug messgaes for G2X
03/08/13   cj      Added featurization till GL1 provides overload APIs
03/07/13   cj      Updated ftm_gsm_meas_fw_execute_rxlm
03/07/13   cj      Fixed Jenkins errors
03/07/13   cj      Initial Check-in
===========================================================================*/

#include "rfmeas_types.h"
#include "rfmeas_mc.h"

#include "ftm_meas.h"
#include "ftm_gsm_dispatch.h"
#include "rfgsm_mc.h"
#include "ftm_gsm_ctl.h"
#include "ftm_gsm_meas.h"
#include "rfmeas_gsm.h"
#include "rfmeas_ftm_mdsp.h"

#include "rfgsm_msg.h"

#include "ftm.h"

#include "rfcom.h"
#include "ftm_gsm_ctl.h"
#include "fws.h"
#include "ftm_common_control.h"

#include "rfa_variation.h"
#include "target.h"    
#include "comdef.h"   

#include "ftmicap.h"

#include "ftm_common.h"

#include "ftm_rf_cmd.h"


//#include "geran_variation.h"
#include "customer.h"
#include "geran_test_mode_api.h"


#include "sys_type.h"

#include "msg.h"
#include "task.h"
#include "rxlm_intf.h"
#include "ftm_gsm_rfprot.h"


#include "comdef.h"
#include "geran_test_mode_api.h"

#include "gl1_defs.h"
#ifdef FEATURE_GSM_GPRS
#include "gtmrs.h"
#include "mdsp_intf.h"
#endif

#include "rfm.h"
#include "rfm_gsm.h"

#ifdef FEATURE_GSM_GPRS_QDSP6
#include "gfw_sw_intf.h"
#endif

#include "DALSys.h"

#include "modem_fw_memmap.h"

#ifdef FEATURE_MCPM
#include "mcpm_api.h"
#endif
#include "fws.h"

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif
#include "rflm_api_gsm.h"
#include "rflm_time.h"


#undef DEBUG_MDSP_SNOOPING_FIX

#define L1_GSM_TIMER_WAIT_SIG           0x00000100
#define L1_GSM_TIMEOUT_SIG              0x00040000  /* mDSP init, TRM req */
#define FTM_MAX_CM_RSSI_MEASUREMENTS    1
#define FTM_GL1_HW_MCPM_START_STOP_DELAY_LOOP   50
#define RXLM_BUF_IDX_INVALID  0xFF

#define FTM_SYS_MODEM_AS_ID_NONE -1


ftm_lm_buf_type ftm_gsm_rxlm_buffer[2] = { { 0, FALSE }, { 0, FALSE }, };
extern ftm_gsm_meas_results g_meas_results;

/*============================================================================
             LOCAL Function Prototypes                                                      
=============================================================================*/
void ftm_gsm_allocate_rxlm(ftm_receive_chain_handle_type chain);
boolean ftm_gsm_meas_get_src_rxlm_buffer(rfm_meas_generic_params_type *test_param_ptr);
void* ftm_gsm_meas_get_enter_param(rfm_meas_generic_params_type *test_param_ptr);
void* ftm_gsm_meas_get_setup_param(rfm_meas_generic_params_type *test_param_ptr);
void* ftm_gsm_meas_get_exit_param(rfm_meas_generic_params_type *test_param_ptr);
boolean ftm_gsm_meas_enter_meas(rfm_meas_enter_param_type *meas_enter_param);
boolean ftm_gsm_meas_build_script(rfm_meas_setup_param_type *meas_setup_param);
boolean ftm_gsm_meas_fw_execute_rxlm(rfm_meas_generic_params_type *test_param_ptr, boolean execute_src_rxlm);
boolean ftm_gsm_meas_fw_start_rxagc(rfm_meas_generic_params_type *test_param_ptr, boolean start_src_agc);
boolean ftm_gsm_meas_exit_meas(rfm_meas_exit_param_type *meas_exit_param);
boolean ftm_gsm_meas_exit_gsm(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param);
boolean ftm_gsm_meas_enter_gsm(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param);
void ftm_gsm_meas_stop( void );
void ftm_gsm_meas_start( void );
void ftm_gsm_power_meas_cmd(rfm_meas_generic_params_type *test_param_ptr);
void ftm_gsm_deallocate_rxlm(ftm_receive_chain_handle_type chain);

/*void ftm_gsm_set_phone_mode( rfm_device_enum_type rfm_dev,
                       ftm_mode_id_type mode );*/
/*============================================================================
             LOCAL/EXTERNAL VARIABLES                                                      
=============================================================================*/


static rfm_meas_enter_param_type ftm_gsm_meas_enter_param;
static rfm_meas_exit_param_type ftm_gsm_meas_exit_param;
static rfm_meas_setup_param_type ftm_gsm_meas_setup_param;

static rfm_meas_params_channel_type ftm_gsm_source_channel_param;
static rfm_meas_params_channel_type ftm_gsm_target_channel_param;



static ftm_meas_func_tbl_type ftm_gsm_meas_apis = \
{
   ftm_gsm_meas_get_src_rxlm_buffer,
   ftm_gsm_meas_get_enter_param,
   ftm_gsm_meas_get_setup_param,
   ftm_gsm_meas_get_exit_param,
   ftm_gsm_meas_enter_meas,
   ftm_gsm_meas_build_script,
   ftm_gsm_meas_exit_gsm,
   ftm_gsm_meas_enter_gsm,
   ftm_gsm_meas_exit_meas
};


void ftm_gsm_meas_register(void)
{
   #ifdef FTM_HAS_IRAT
   ftm_meas_register(RFCOM_GSM_MODE, &ftm_gsm_meas_apis);
   #endif
   memset(&ftm_gsm_meas_enter_param,0,sizeof(rfm_meas_enter_param_type));
   memset(&ftm_gsm_meas_exit_param,0,sizeof(rfm_meas_exit_param_type));
   memset(&ftm_gsm_meas_setup_param,0,sizeof(rfm_meas_setup_param_type));
}

void ftm_gsm_meas_deregister(void)
{
   ftm_meas_deregister(RFCOM_GSM_MODE);
}

void ftm_gsm_allocate_rxlm(ftm_receive_chain_handle_type chain)
{
  lm_tech_type tech = LM_GSM;
  lm_status_type status;

  if (!ftm_gsm_rxlm_buffer[chain].is_buf_allocated)
  {
    status = rxlm_allocate_buffer((rxlm_chain_type)chain, tech,
                                  &(ftm_gsm_rxlm_buffer[chain].buf_idx));
    if (status != LM_SUCCESS )
    {
       MSG_ERROR("RxLM allocate buffer failed, status:%d",status,0,0);
       return;
    }
    else
    {
      MSG_HIGH("RXLM buffer allocated for chain:%d, buffer index:%d",
               chain, ftm_gsm_rxlm_buffer[chain].buf_idx, 0);
    }
    ftm_gsm_rxlm_buffer[chain].is_buf_allocated = TRUE;
  }
}


boolean ftm_gsm_meas_get_src_rxlm_buffer(rfm_meas_generic_params_type *test_param_ptr)
{
  if(test_param_ptr == NULL)
  {
    return FALSE;
  }
  
  if(test_param_ptr->src_tech != RFCOM_GSM_MODE)
  {
    return FALSE;
  }

  ftm_gsm_allocate_rxlm(FTM_RECEIVE_CHAIN_0);

  if(ftm_gsm_rxlm_buffer[RF_PATH_0].is_buf_allocated)
  {
      test_param_ptr->src_rxlm_buf_idx_ant0 = ftm_gsm_rxlm_buffer[RF_PATH_0].buf_idx;
  }

  return TRUE;
}

void ftm_gsm_deallocate_rxlm(ftm_receive_chain_handle_type chain)
{
  lm_status_type status;

  if (ftm_gsm_rxlm_buffer[chain].is_buf_allocated)
  {
    status = rxlm_deallocate_buffer(ftm_gsm_rxlm_buffer[chain].buf_idx);
      if (status != LM_SUCCESS )
      {
        MSG_ERROR("RxLM allocate buffer failed, status:%d",status,0,0);
      }
      else
      {
      MSG_HIGH("RXLM buffer deallocated for chain:%d",chain,0,0);
      ftm_gsm_rxlm_buffer[chain].is_buf_allocated = FALSE;
    }
  }
}
/* To initialize common variables */
boolean ftm_gsm_param_init(rfm_meas_generic_params_type *test_param_ptr)
{	
	/* Parameter Check */
  if(test_param_ptr == NULL)
  {
    return FALSE;
  }

  /* At least one tech should be gsm */
  if((test_param_ptr->src_tech != RFCOM_GSM_MODE) &&
     (test_param_ptr->tgt_tech != RFCOM_GSM_MODE))
  {
    return FALSE;
  }
  ftm_gsm_source_channel_param.serving_band = (rf_card_band_type)test_param_ptr->src_band;
  ftm_gsm_source_channel_param.serving_chan  = test_param_ptr->src_chan;
  ftm_gsm_source_channel_param.neighbor_band = (rf_card_band_type)test_param_ptr->tgt_band;
  ftm_gsm_source_channel_param.neighbor_chan = test_param_ptr->tgt_chan;

  return TRUE;
}

/*====================================================================================*/

/* To initialize enter param */
void* ftm_gsm_meas_get_enter_param(rfm_meas_generic_params_type *test_param_ptr)
{
 
  MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"Inside ftm_gsm_meas_get_enter_param()");
  if ( ! ftm_gsm_param_init( test_param_ptr ))
  {
	  return test_param_ptr;
  }

  ftm_gsm_meas_enter_param.source_param = &ftm_gsm_source_channel_param;
  ftm_gsm_meas_enter_param.target_param = &ftm_gsm_target_channel_param;

  if (test_param_ptr->src_tech == RFCOM_GSM_MODE)
  {
    ftm_gsm_source_channel_param.serving_rxlm_buf_idx = test_param_ptr->src_rxlm_buf_idx_ant0 ;
  }
  else
  {
    ftm_gsm_source_channel_param.neighbor_rxlm_buf_idx = test_param_ptr->tgt_rxlm_buf_idx_ant0 ;
  }
  if( test_param_ptr->src_tech == RFCOM_GSM_MODE )
  {
    /* Since in some cases source structure is used and in some cases target */
    ftm_gsm_target_channel_param = ftm_gsm_source_channel_param;
  }
  return (&ftm_gsm_meas_enter_param);
}



/* To initialize setup param */
void* ftm_gsm_meas_get_setup_param(rfm_meas_generic_params_type *test_param_ptr)
{
  ftm_gsm_meas_setup_param.source_param = &ftm_gsm_source_channel_param;
  ftm_gsm_meas_setup_param.target_param = &ftm_gsm_target_channel_param;

  if ( ! ftm_gsm_param_init( test_param_ptr ))
  {
	  return test_param_ptr;
  }

  /* If W2W, use a different structure, otherwise use regular */
  if( (test_param_ptr->src_tech == RFCOM_GSM_MODE) && 
      (test_param_ptr->tgt_tech == RFCOM_GSM_MODE))
  {
     return test_param_ptr;
  }
  
  if( test_param_ptr->src_tech == RFCOM_GSM_MODE )
  {
    ftm_gsm_source_channel_param.cleanup_buffer = test_param_ptr->src_buffer_index;
    ftm_gsm_source_channel_param.serving_rxlm_buf_idx = test_param_ptr->src_rxlm_buf_idx_ant0;
    ftm_gsm_source_channel_param.startup_buffer = test_param_ptr->tgt_buffer_index;
    ftm_gsm_source_channel_param.neighbor_rxlm_buf_idx = test_param_ptr->tgt_rxlm_buf_idx_ant0;
      /* Since in some cases source structure is used and in some cases target */
    ftm_gsm_target_channel_param = ftm_gsm_source_channel_param;
  }
  if(test_param_ptr->src_tech == RFCOM_GSM_MODE)
  {
    ftm_gsm_meas_setup_param.script_type = RFM_MEAS_PROGRAM_STARTUP;
  }
  else
  {
    ftm_gsm_meas_setup_param.script_type = RFM_MEAS_PROGRAM_CLEANUP;
  }

  return (&ftm_gsm_meas_setup_param);
}


void* ftm_gsm_meas_get_exit_param(rfm_meas_generic_params_type *test_param_ptr)
{
  ftm_gsm_meas_exit_param.source_param = &ftm_gsm_source_channel_param;
  ftm_gsm_meas_exit_param.target_param = &ftm_gsm_target_channel_param;

  if ( ! ftm_gsm_param_init( test_param_ptr ))
  {
    return test_param_ptr;
  }
  if( test_param_ptr->src_tech == RFCOM_GSM_MODE )
  {    /* Since in some cases source structure is used and in some cases target */
    ftm_gsm_target_channel_param = ftm_gsm_source_channel_param;
  }
  return (&ftm_gsm_meas_exit_param);
}


boolean ftm_gsm_meas_enter_meas(rfm_meas_enter_param_type *meas_enter_param)
{
  
  /* For G2X */
  if( meas_enter_param == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR," meas_enter_param is NULL");
    return FALSE;
  }
  if( meas_enter_param->header.source_tech == RFCOM_GSM_MODE )
  {
    rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;

    status = rfmeas_mc_enter(meas_enter_param, NULL);
    if(status != RFM_MEAS_COMMON_SUCCESS)
    {
	  MSG(MSG_SSID_RF, MSG_LEGACY_ERROR," rfmeas_mc_enter returned failure");
      return FALSE;
    }
  }
  return TRUE;
}

boolean ftm_gsm_meas_build_script(rfm_meas_setup_param_type *meas_setup_param)
{
  
  if( meas_setup_param == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR," meas_setup_param is NULL");
    return FALSE;
  }
  if( meas_setup_param->header.source_tech == RFCOM_GSM_MODE )
  {
    rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;
  
    status = rfmeas_build_scripts(meas_setup_param, NULL);
    if(status != RFM_MEAS_COMMON_SUCCESS)
    {
      return FALSE;
	  MSG(MSG_SSID_RF, MSG_LEGACY_ERROR," rfmeas_build_scripts returned failure");
    }
  }
  return TRUE;	
}

boolean ftm_gsm_meas_fw_execute_rxlm(rfm_meas_generic_params_type *test_param_ptr, boolean execute_src_rxlm)
{
#ifndef TEST_FRAMEWORK
  rflm_handle_rx_t rxlm_buf_idx;
  rflm_gsm_irat_ccs_input_type script_vars_in;
  rflm_gsm_irat_ccs_output_type script_vars_out;
  rflm_err_t cleanup_status = FALSE;

  script_vars_in.trigger_time = rflm_time_read_ustmr_count() + 2000;
  MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"Inside ftm_gsm_meas_fw_execute_rxlm()");

  if(test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR," NULL param pointer");
    return FALSE;
  }

  rxlm_buf_idx = test_param_ptr->src_rxlm_buf_idx_ant0;

  MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "executing rfgsm_mc_execute_rxlm() with src_chan = %d, rxlm_buf_idx = %d",  test_param_ptr->src_chan ,rxlm_buf_idx );
  /* Trigger FW to execute Script */
  
#if 0
  /* FW support required */
  geran_test_mode_api_async_intf_app_mode_config( GFW_APP_MODE_MEASUREMENT, 
                                                  rxlm_buf_idx, 
                                                  gl1_hw_rf_map_device2_as_id(RFM_DEVICE_0, ftm_mode));
#else
  cleanup_status = rflm_gsm_prog_irat_trigger_ccs_script(rxlm_buf_idx,
                                        &script_vars_in,
                                        &script_vars_out);
#endif

  MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Exiting ftm_gsm_meas_fw_execute_rxlm %d cleanup status %d",
          rxlm_buf_idx, cleanup_status);
#endif
  return TRUE;

}


boolean ftm_gsm_meas_exit_meas(rfm_meas_exit_param_type *meas_exit_param)
{
  rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;
  if( meas_exit_param == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR," meas_exit_param is NULL");
	return FALSE;
  }

  if( meas_exit_param->header.source_tech == RFCOM_GSM_MODE )
  {
    status = rfmeas_mc_exit(meas_exit_param, NULL);
    if(status != RFM_MEAS_COMMON_SUCCESS)
    {
	  MSG(MSG_SSID_RF, MSG_LEGACY_ERROR," rfmeas_mc_exit returned failure");
      return FALSE;
    } 
  }
  return TRUE;
}


boolean ftm_gsm_meas_exit_gsm(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param)
{
  boolean result = TRUE;
#ifndef TEST_FRAMEWORK
  ftm_rx_burst_type ftm_rx_burst;
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id(RFM_DEVICE_0, ftm_mode);
  uint16 offset = 1250;

  if(test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "NULL param pointer");
    return FALSE;
  }

  if( test_param_ptr->src_tech != RFCOM_GSM_MODE )
  {
    /* ftm_gsm_meas_stop(); */
	mdsp_ftm_wait_for_frame_tick(as_id);
    mdsp_ftm_wait_for_frame_tick(as_id);
	mdsp_ftm_wait_for_frame_tick(as_id);

    geran_test_mode_api_cm_cleanup( offset, as_id );

	mdsp_ftm_wait_for_frame_tick(as_id);
    mdsp_ftm_wait_for_frame_tick(as_id); 
    mdsp_ftm_wait_for_frame_tick(as_id);

    geran_test_mode_api_cm_gsm_terminate( as_id );

  }
  else
  {
	/* Step : 1. Stop AGC,
     */

    ftm_rx_burst.ftm_rx_funct = (ftm_enum_rx_function_type)FTM_RX_FUNCTION_RF_RX_HW_OFF;
    ftm_gsm_rx_burst_command( RFM_DEVICE_0,
                              &ftm_rx_burst,
                              FTM_DIV_FACTOR );
  
    /* Step 2: Stop FW */

    mdsp_ftm_wait_for_frame_tick(as_id);
    mdsp_ftm_wait_for_frame_tick(as_id);
    mdsp_ftm_wait_for_frame_tick(as_id);

    geran_test_mode_api_cm_cleanup(offset, as_id);

    mdsp_ftm_wait_for_frame_tick(as_id);
    mdsp_ftm_wait_for_frame_tick(as_id); 
    mdsp_ftm_wait_for_frame_tick(as_id);

    geran_test_mode_api_cm_gsm_terminate(as_id);

  }
#endif
  return result;
}

boolean ftm_gsm_meas_enter_gsm(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param)
{
  boolean result = TRUE;
#ifndef TEST_FRAMEWORK
  rfm_meas_program_script_type script_type = RFM_MEAS_PROGRAM_STARTUP;
  uint32 buffer_index = 0;
  rex_tcb_type*  task_ptr;
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id(RFM_DEVICE_0, ftm_mode);
  uint16 offset = 1250;
  gl1_hw_cm_rf_info_t  rf_info;
  MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "inside ftm_gsm_meas_enter_gsm");

  if(test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "NULL param pointer");
    return FALSE;
  }

  task_ptr = TASK_HANDLE(ftm)  ;

  geran_test_mode_api_cm_gsm_init( NULL,
                                   task_ptr,
                                   NULL,
                                   L1_GSM_TIMER_WAIT_SIG,
                                   L1_GSM_TIMEOUT_SIG,
                                   as_id );

  if( test_param_ptr->src_tech != RFCOM_GSM_MODE )
  {
    /* X2G */
    rf_info.serving_rat = test_param_ptr->src_tech;
    rf_info.serving_band = test_param_ptr->src_band;
    rf_info.serving_chan = test_param_ptr->src_chan;
    rf_info.bw = test_param_ptr->src_bw;
    rf_info.rxlm_cleanup_buf_idx = test_param_ptr->src_rxlm_buf_idx_ant0;
    rf_info.startup_buf_idx = 0;
    rf_info.cleanup_buf_idx = 1;
    rf_info.rxlm_gsm_buf_idx = test_param_ptr->tgt_rxlm_buf_idx_ant0;
    rf_info.num_source_item = 1;
    rf_info.source_param[RFM_MEAS_DL_PATH].device = RFM_DEVICE_0;
    rf_info.source_param[RFM_MEAS_DL_PATH].div_device = RFM_DEVICE_1;
    rf_info.source_param[RFM_MEAS_DL_PATH].div_en = test_param_ptr->src_div_en;    
    rf_info.source_param[RFM_MEAS_DL_PATH].band = test_param_ptr->src_band;   
     
    if( test_param_ptr->src_tech == RFCOM_LTE_MODE )
    {
#ifdef FEATURE_GSM_DBDC_W2G

      rf_info.u.l2x_tuneaway_param.carrier[0].band = test_param_ptr->src_band;
      rf_info.u.l2x_tuneaway_param.carrier[0].earfcn = test_param_ptr->src_chan;
      rf_info.u.l2x_tuneaway_param.carrier[0].bandwidth = test_param_ptr->src_bw;

      rf_info.u.l2x_tuneaway_param.carrier[1].band = test_param_ptr->src_band_carrier_1;
      rf_info.u.l2x_tuneaway_param.carrier[1].earfcn = test_param_ptr->src_chan_carrier_1;
      rf_info.u.l2x_tuneaway_param.carrier[1].bandwidth = test_param_ptr->src_bw_carrier_1;
      rf_info.u.l2x_tuneaway_param.num_carriers = 1;


#else

	  rf_info.l2x_tuneaway_param.carrier[0].band = test_param_ptr->src_band;
	  rf_info.l2x_tuneaway_param.carrier[0].earfcn = test_param_ptr->src_chan;
      rf_info.l2x_tuneaway_param.carrier[0].bandwidth = (lte_bandwidth_idx_e) test_param_ptr->src_bw;
	
	  rf_info.l2x_tuneaway_param.carrier[1].band = test_param_ptr->src_band_carrier_1;
	  rf_info.l2x_tuneaway_param.carrier[1].earfcn = test_param_ptr->src_chan_carrier_1;
      rf_info.l2x_tuneaway_param.carrier[1].bandwidth = (lte_bandwidth_idx_e) test_param_ptr->src_bw_carrier_1;
	  rf_info.l2x_tuneaway_param.num_carriers = 1;
#endif
      }
      else if( test_param_ptr->src_tech == RFCOM_WCDMA_MODE )
      {
	  rf_info.u.w_tune_params.channels[0][0] = test_param_ptr->src_chan;

	  if( test_param_ptr->src_extra_carrier_flag == TRUE)
	  {
	    rf_info.u.w_tune_params.channels[0][1] = test_param_ptr->src_chan_carrier_1;
        rf_info.u.w_tune_params.carrier_types[0]= RFCOM_DUAL_CARRIER;
	  }
	  else
	  {
	    rf_info.u.w_tune_params.carrier_types[0]= RFCOM_SINGLE_CARRIER;
	  }	
	  
	}
	else
	{
	}

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Calling geran_test_mode_api_cm_startup at frame %d",0 );

    geran_test_mode_api_cm_startup(offset, &rf_info, as_id);

    MSG_HIGH("gl1_hw_x2g_startup executed",0 ,0,0);
  
    MSG_HIGH("Calling ftm_gsm_power_meas_cmd",0 ,0,0);
  }
  else
  {
    /* G2X and we are tuning back */
    script_type = RFM_MEAS_PROGRAM_CLEANUP;
    buffer_index = test_param_ptr->src_buffer_index;

    /* execute RF script from RFSW side */
    rfmeas_ftm_mdsp_execute_irat_script(script_type,buffer_index);
	
    /* Send RXLM to FW */
	use_src_param = TRUE;
    result = ftm_gsm_meas_fw_execute_rxlm(test_param_ptr, use_src_param);
	if (result != TRUE)
    {
      MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Error in executing IRAT buffer, use_src_param %d, src_tech %d, tgt_tech %d",
          use_src_param, test_param_ptr->src_tech, test_param_ptr->tgt_tech);
      return result;
    }
    mdsp_ftm_wait_for_frame_tick(as_id);
    mdsp_ftm_wait_for_frame_tick(as_id); 
    mdsp_ftm_wait_for_frame_tick(as_id);

    ftm_gsm_deallocate_rxlm(FTM_RECEIVE_CHAIN_0);

  }

  ftm_gsm_power_meas_cmd(test_param_ptr);
  MSG_HIGH("Executed ftm_gsm_meas_fw_start_rxagc",0 ,0,0);
#endif
  return result;
}




/*===========================================================================

  FUNCTION:  rgsm_mdsp_exit

===========================================================================*/
/*!
  @brief
  This function disables gsm FW .

  @return
  None
*/
/*=========================================================================*/


void ftm_gsm_meas_stop( void )
{

 
  rfm_device_enum_type device = RFM_DEVICE_0;
 
  ftm_rfmode_exit( device, FTM_STATE_GSM );
 
}

/*===========================================================================

  FUNCTION:  ftm_gsm_meas_start

===========================================================================*/
/*!
  @brief
  This function enables gsm FW .

  @return
  None
*/
/*=========================================================================*/
void ftm_gsm_meas_start( void )
{
  ftm_rfmode_enter(RFM_DEVICE_0,FTM_STATE_GSM_IRAT);
}




void ftm_gsm_power_meas_cmd(rfm_meas_generic_params_type *test_param_ptr)
{
#ifndef FEATURE_DUAL_ACTIVE
#ifndef TEST_FRAMEWORK
  ARFCN_T                     arfcns[MAX_CM_RSSI_MEASUREMENTS_REQ]; 
  gl1_hw_rx_tags_type         tags[MAX_CM_RSSI_MEASUREMENTS_REQ];
  gl1_hw_burst_position_type  position[MAX_CM_RSSI_MEASUREMENTS_REQ];
  uint16                      offset[MAX_CM_RSSI_MEASUREMENTS_REQ];
  dBx16_T                     rx_signal_strength[MAX_CM_RSSI_MEASUREMENTS_REQ];
  sys_modem_as_id_e_type as_id = FTM_SYS_MODEM_AS_ID_NONE;
  int i = 0;
  int num_of_req = 1;
  boolean result = TRUE;
  uint16 channel;
  uint16 band;
  gl1_hw_measurement_T  samples[1];

  if( test_param_ptr->src_tech == RFCOM_GSM_MODE )
  {
    /* G2X */
    channel = (uint16)test_param_ptr->src_chan;
	band = test_param_ptr->src_band;
  }
  else
  {
	/* X2G */
	channel = (uint16)test_param_ptr->tgt_chan;
	band = test_param_ptr->tgt_band;
  }

  for(i = 0; i < FTM_MAX_CM_RSSI_MEASUREMENTS; i++) {

    arfcns[i].num = channel;
	switch( band )
	{
		case RFCOM_BAND_GSM850:
              arfcns[i].band = SYS_BAND_CELL_850;
			  break;
		case RFCOM_BAND_GSM900:
              arfcns[i].band = SYS_BAND_EGSM_900;
			  break;
		case RFCOM_BAND_GSM1800:
              arfcns[i].band = SYS_BAND_DCS_1800;
			  break;
		case RFCOM_BAND_GSM1900:
              arfcns[i].band = SYS_BAND_PCS_1900;
			  break;
		default:
			  MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Invalid band  %d or channel received %d",
          arfcns[i].band, arfcns[i].num);
    }
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "band received %d, channel received %d",
          arfcns[i].band, arfcns[i].num);

	MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "band expected, GSM_850 %d, GSM 900 %d",
         RFGSM_BAND_GSM850 , RFGSM_BAND_GSM900);

	MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "band expected, GSM 1800 %d, GSM 1900 %d",
          RFGSM_BAND_GSM1800, RFGSM_BAND_GSM1900);

    position[i] = GL1_HW_ONE_BURST_IN_GAP;

    tags[i].data_tag = 0;
    tags[i].metrics_tag = 0;
    tags[i].gain.range = 0;
    tags[i].gain.range_high_limitx16 = 0;
    tags[i].gain.range_low_limitx16 = 0;
    tags[i].gain.valx16 = 0;

    rx_signal_strength[i] = 0;
  }

  for(i = 0;  i < num_of_req; i++) 
  {
    offset[i] = (1250 + 625);
  
  }

  MSG_HIGH("Calling geran_test_mode_api_cm_power_measure",0 ,0,0);

  geran_test_mode_api_cm_power_measure( arfcns,
                                        rx_signal_strength,
                                        num_of_req,
                                        offset,
                                        position,
                                        tags,
                                        as_id );

  /* Delay of 2 frames atleast */
  mdsp_ftm_wait_for_frame_tick(as_id);
  mdsp_ftm_wait_for_frame_tick(as_id);
  mdsp_ftm_wait_for_frame_tick(as_id);

  result = geran_test_mode_api_cm_get_power_measure_results(samples, num_of_req, tags, as_id);

  if (result != TRUE)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Error in getting gsm RxAGC, src_tech %d, tgt_tech %d",
          test_param_ptr->src_tech, test_param_ptr->tgt_tech);
  }
  MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "RxAGC, samples %d, tags %d",
          samples[0].dBm_x16, tags[0].gain.range_high_limitx16);


  mdsp_ftm_wait_for_frame_tick(as_id);

  g_meas_results.x2g_irat_enabled = TRUE;

  g_meas_results.db_x16 = samples[0].dBm_x16;

#endif
#endif
}

