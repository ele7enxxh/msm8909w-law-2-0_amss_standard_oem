/*!
  @file
  ftm_enh_internal_device_calibration_fb_dc.c

  @brief

*/
/*==============================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_enh_internal_device_calibration_fb_dc.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/10/15    br     Send tech,band,bandwidth and fbrx gain state as inputs to stg enable and disable apis
10/08/14    ka     Replace rfc with device api for retrieving device pointer
09/25/14   brath   Update the api used to fetch band enum for LTE bands(specially for split bands).
09/15/14   xsh     Fix Compiler warning
07/30/14   brath   LTE-Add support to update dc value to global nv for fbrx mode 1 (20MHz FBRx bandwidth)
06/10/14   shb     Fixed offtarget compiler error
05/29/14   brath   Updates to provide TX LO frequency for device cal APIs
04/08/14   jmf     Updates to accommodate error code returns
03/14/14   jmf     Fixes to DC Cal Proc / Proper band Enum for NV update
02/5/14    br      Inner FBRx DC functions and implementation of capture functions
03/19/14   br      DC cal for all FBRX gain states,setting up feedback state from within DC cal .
12/15/13   xs      Initial Revision

==============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN

#include "rfa_variation.h"
#include "comdef.h"
#include "ftm_common_timer.h"
#include "ftm_common_control.h"
#include "ftm_common_data.h"
#include "rfcommon_math.h"
#include "modem_mem.h"
#include "string.h"
#include "stringl.h" /* for memscpy */
#include "ftm_msg.h"
#include "ftm_common_enh_internal_device_cal.h"
#include "rfcommon_fbrx_nv.h"
//#include "rfm_internal.h"
//#include "DALSys.h"
#include "rfcommon_time_profile.h"
//#include "zlib.h"
#include "ftm_enh_internal_device_cal_fb_dc.h"
#include "rfdevice_cmn_intf.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rflm_api_fbrx.h"
#include "rfcommon_mdsp.h"
#include "ftm_common_fbrx.h"
#include "rflm_dm_api.h"
#include "rfcommon_core_utils.h"

#ifdef FEATURE_LTE
#include "rflte_util.h"
#include "rflte_mc.h"
#include "rflte_ftm_mc.h"
#endif

#ifdef FEATURE_WCDMA
#include "rfwcdma_mc.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rf_cdma_utils_freq.h"
#endif

#ifdef FEATURE_TDSCDMA
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_mc.h"
#endif

#ifdef FEATURE_WCDMA
extern rfwcdma_mc_state_type rfwcdma_mc_state;
#endif

#ifdef FEATURE_TDSCDMA
extern rf_tdscdma_mc_state_type rf_tdscdma_mc_state;
#endif
/*
  @name: ftm_internal_cal_fb_dc

  @brief: feedback DC cal  
 
  @details
  
  @param
  input:req_instr, payload instruction
  output: res_len:  cal result length
             dc_result: output of DC cal NV
  
  @return
     success or fail
*/
extern uint8 rfcommon_fbrx_msm_convert_tx_cfg_to_mode(rfcommon_fbrx_tx_cfg_type cfg);

static int32 ftm_enh_int_cal_fbrx_dc_val_i;
static int32 ftm_enh_int_cal_fbrx_dc_val_q;

#ifdef FEATURE_FACTORY_TESTMODE

boolean ftm_enh_internal_device_cal_fb_dc_config_fbrx(void) 
{
   boolean ret_val = TRUE;
   return ret_val;
}

boolean ftm_enh_internal_device_cal_fb_dc_update_global_nv_struct(void) 
{
   boolean ret_val = TRUE;
   // update per tech/band spec
   return ret_val;
}

boolean ftm_enh_internal_device_cal_fb_dc_get_global_nv_struct(void) 
{
   boolean ret_val = TRUE;
   // get per tech/band spec
   return ret_val;
}

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fb_dc_capture_sequence
(
  rfm_mode_enum_type rf_mode,
  rfcom_bw_type_u rfcom_bw,                                   
  uint8 fbrx_gain_state,
  uint8 num_captures,
  uint16 num_iq_samples,
  uint8 debug_mode_en
)
{
   ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
   boolean api_status = TRUE;
   rfdevice_rxtx_common_class *device_ptr = NULL;
   ftm_rf_technology_type ftm_rf_tech  = FTM_RF_TECH_UNKNOWN;
   ftm_cal_xpt_instr_payload_iq_capture_type iq_cap_instr;
   char filename[20] = { '\0' };
   uint8 samp_buf_id = -1;
   uint8 iter = 0;
   uint32 tx_freq = 0;
   uint64 tx_freq_hz=0;

   #ifdef FEATURE_CDMA
   const rf_cdma_data_status_type *dev_status_r;
   rfm_cdma_band_class_type band;
   #endif
   rfcom_band_type_u band_union;

   /* Get Current FTM RF Tech to dispatch tech-specific IQ capture command */
   ftm_rf_tech = ftm_get_rf_technology();

 
   /****************************************************************/
  /* Get current Tx frequency information     */
  /****************************************************************/

  switch ( ftm_rf_tech )
  {
  #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
  case FTM_RF_TECH_CDMA:
    dev_status_r = rf_cdma_get_device_status( RFM_DEVICE_0 ); // what is the current tx device?
    if( dev_status_r != NULL)
    {
      band = rf_cdma_sys_band_type_to_rf_band_type( (sys_band_class_e_type)dev_status_r->curr_band);
      tx_freq = rf_cdma_get_tx_carrier_freq( band, (int)
                                             dev_status_r->lo_tuned_to_chan );
      band_union.cdma_band = dev_status_r->curr_band;
      device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, RFCOM_1X_MODE, band_union);
    }
    else
    {
      FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
      return FTM_ENH_INTERNAL_DEVICE_CAL_GENERAL_FAILURE;
    }
    break;
  #endif

  #ifdef FEATURE_WCDMA
  case FTM_RF_TECH_WCDMA:
    tx_freq = rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
    FTM_MSG_1(FTM_HIGH, "ftm_enh_internal_device_cal_fb_droop_stg_sequence: curr_tx_chan: %d", tx_freq);
    band_union.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
    device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, RFCOM_WCDMA_MODE, band_union);
    break;
  #endif /* FEATURE_WCDMA */
  #ifdef FEATURE_LTE
  case FTM_RF_TECH_LTE:
    tx_freq = rflte_core_get_tx_freq_from_uarfcn(
                                   rflte_ftm_mc_sm_get_tx_chan(),
                                   rflte_ftm_mc_sm_get_band());
    band_union.lte_band = rflte_ftm_mc_sm_get_band();
    device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, RFCOM_LTE_MODE, band_union);
    break;
  #endif
  #ifdef FEATURE_TDSCDMA
  case FTM_RF_TECH_TDSCDMA:
    tx_freq = rf_tdscdma_core_util_get_freq_from_chan(rf_tdscdma_mc_state.curr_chan_tx);
    band_union.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
    device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, RFCOM_TDSCDMA_MODE, band_union);
    break;
  #endif
  default:
    FTM_MSG(FTM_ERROR, "Unsupported Tech Type: Return error handle");
    return FALSE;
    /* Flag Error as Unsupported Tech */
  }
  
   /* check for null pointer and throw error */
   if (device_ptr == NULL) 
   {
      FTM_MSG(FTM_ERROR, "rfcommon_core_get_tx_device_ptr returns NULL");
      return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
   }

  tx_freq_hz = tx_freq * 1000; 

   // check if there is going to be too many captures to save on the EFS
   iq_cap_instr.primary_device = RFM_DEVICE_0; /* What is the tx device, doesn't matter */
   iq_cap_instr.feedback_device = RFM_DEVICE_0; /* What is the fbrx device */
   iq_cap_instr.num_samples = num_iq_samples;
   iq_cap_instr.proc_flag = PROC_NONE;
   iq_cap_instr.capture_type = XPT_CAPTURE_FBRX;

   /* Reset Capture Buffers */
   rfcommon_core_xpt_init_buffers(); // must be done before IQ capture code is called - Mark in Documentation for IQ Capture


   for (iter = 0; iter < num_captures; iter++) 
   {
      iq_cap_instr.first_trigger = 0;
      iq_cap_instr.last_trigger = 0; 
      if (iter == 0) 
      {
         iq_cap_instr.first_trigger = 1;
      }
      if (iter == (num_captures - 1)) 
      {
         iq_cap_instr.last_trigger = 1;
      }
      snprintf(&filename[0],20,"iq_dc_%05d.bin", (int)fbrx_gain_state);
      /*Set up FBRX DC setup*/
      FTM_MSG(FTM_HIGH, "Setting up DC enable");
      api_status = rfdevice_cmn_get_fbrx_dc_cal_enable_script(
         device_ptr,
         RFDEVICE_EXECUTE_IMMEDIATE,
         NULL,
         tx_freq_hz,
         rf_mode,
         band_union,
         rfcom_bw,
         fbrx_gain_state
         );
      if (api_status == FALSE) 
      {
         FTM_MSG(FTM_ERROR, "rfdevice_cmn_get_fbrx_dc_cal_enable_script returned FALSE ");
         rfcommon_core_xpt_free_all_buffers(); 
         return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
      }

      /****************************************************************/
      /*              Setup and Trigger XPT IQ Capture                */
      /****************************************************************/
      ret_val = ftm_enh_internal_device_cal_fbrx_iq_capture(ftm_rf_tech, &iq_cap_instr, &samp_buf_id, debug_mode_en, &filename[0]);
      if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
      {
         FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_capture failed for tech %d", (int)ftm_rf_tech);
         FTM_MSG(FTM_ERROR, "Setting up DC disable");
         api_status = rfdevice_cmn_get_fbrx_dc_cal_disable_script(
                         device_ptr,
                         RFDEVICE_EXECUTE_IMMEDIATE,
                         NULL,
                         tx_freq_hz,
                         rf_mode,
                         band_union,
                         rfcom_bw,
                         fbrx_gain_state
                         );
         rfcommon_core_xpt_free_all_buffers();  // must be done after last IQ capture code is called - Mark in Documentation for IQ Capture
         return ret_val;
      }
      FTM_MSG_1(FTM_HIGH, "Current Capture Buffer is %d", samp_buf_id);
      ftm_enh_int_cal_fbrx_dc_val_i = 0;
      ftm_enh_int_cal_fbrx_dc_val_q = 0;
      FTM_MSG_2(FTM_HIGH, "ftm_enh_int_cal_fbrx_dc_val_i: %d,  ftm_enh_int_cal_fbrx_dc_val_q: %d", ftm_enh_int_cal_fbrx_dc_val_i, ftm_enh_int_cal_fbrx_dc_val_q);

      api_status = ftm_enh_internal_device_cal_fb_dc_process(samp_buf_id, num_iq_samples, 0, &ftm_enh_int_cal_fbrx_dc_val_i, &ftm_enh_int_cal_fbrx_dc_val_q);
      if ( api_status == FALSE )
      {
        FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fb_dc_process returned FALSE for tech %d", (int)ftm_rf_tech );
        FTM_MSG(FTM_ERROR, "Setting up DC disable");
        api_status = rfdevice_cmn_get_fbrx_dc_cal_disable_script(
                           device_ptr,
                           RFDEVICE_EXECUTE_IMMEDIATE,
                           NULL,
                           tx_freq_hz,
                           rf_mode,
                           band_union,
                           rfcom_bw,
                           fbrx_gain_state
                           );
        FTM_MSG_1(FTM_ERROR, "Something went wrong on loop #%d, breaking loop, check FTM Msgs", iter);
        rfcommon_core_xpt_free_all_buffers();  // must be done after last IQ capture code is called - Mark in Documentation for IQ Capture
        return FTM_ENH_INTERNAL_DEVICE_CAL_PROCESSING_FAILED;
      }

      ftm_enh_int_cal_fbrx_dc_val_q = -ftm_enh_int_cal_fbrx_dc_val_q;
      FTM_MSG_2(FTM_HIGH, "ftm_enh_int_cal_fbrx_dc_val_i: %d,  ftm_enh_int_cal_fbrx_dc_val_q: %d", ftm_enh_int_cal_fbrx_dc_val_i, ftm_enh_int_cal_fbrx_dc_val_q);

      /****************************************************************/
      /*                     Disable FBRx DC setup                         */
      /****************************************************************/
      FTM_MSG(FTM_HIGH, "Setting up DC disable");
      api_status = rfdevice_cmn_get_fbrx_dc_cal_disable_script(
         device_ptr,
         RFDEVICE_EXECUTE_IMMEDIATE,
         NULL,
         tx_freq_hz,
         rf_mode,
         band_union,
         rfcom_bw,
         fbrx_gain_state
         );
      if (api_status == FALSE) 
      {
        FTM_MSG_1(FTM_ERROR, "Something went wrong on loop #%d, breaking loop, check FTM Msgs", iter);
        rfcommon_core_xpt_free_all_buffers();  // must be done after last IQ capture code is called - Mark in Documentation for IQ Capture
        return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
      }
   }
   rfcommon_core_xpt_free_all_buffers();  // must be done after last IQ capture code is called - Mark in Documentation for IQ Capture

   return ret_val;

}


boolean ftm_enh_internal_device_cal_fb_dc_iq_capture(void) 
{
   boolean ret_val = TRUE;


   return ret_val;
}


boolean ftm_enh_internal_device_cal_fb_dc
(
   void const *req_instr,
   uint16 *res_len,
   void *fb_dc_result
) 
{
   ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
   uint8 fbrx_mode_index = 0;
   ftm_enh_internal_device_cal_data_type *internal_device_cal_data;
   
   uint16 fbrx_band;
   uint16 nv_band=0;
   int32 dc_i_nv;
   int32 dc_q_nv;

   uint8 iter=0;
   uint8 num_captures=1;
   uint8 debug_mode_en=0;
   uint8 num_fbrx_gain_states;
   uint16 num_iq_samples ;
   uint8 fbrx_gain_state[4];
   rfcom_band_type_u band;
   rfcom_bw_type_u rfcom_bw;
   
   ftm_rf_technology_type fbrx_tech = FTM_RF_TECH_UNKNOWN;
   uint8 fbrx_bw = RFCOMMON_FBRX_LTE_10;
   rfm_mode_enum_type fbrx_rfm_mode = RFM_INVALID_MODE;
   ftm_rf_technology_type curr_tx_ftm_rf_tech = FTM_RF_TECH_UNKNOWN;
   rfdevice_rxtx_common_class* device_ptr = NULL;

   ftm_enh_internal_device_cal_instr_payload_fb_dc_type *fb_dc_instr = (ftm_enh_internal_device_cal_instr_payload_fb_dc_type *)req_instr;

   ftm_enh_internal_device_cal_fb_dc_result_type *dc_result = (ftm_enh_internal_device_cal_fb_dc_result_type *)fb_dc_result;

   #ifdef FEATURE_LTE
   uint32 tx_chan = 0;
   uint8 path_override_index = 255;
   rfcom_bw.lte_bw = 3;
   #endif

   internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
   debug_mode_en=internal_device_cal_data->internal_device_cal_config.dbg_mode_en;

   *res_len = sizeof(ftm_enh_internal_device_cal_fb_dc_result_type);

   FTM_MSG_2(FTM_HIGH,"Number of gain states %d Number of samples %d",fb_dc_instr->num_fbrx_gain_states,fb_dc_instr->num_iq_samples);
   num_fbrx_gain_states = fb_dc_instr->num_fbrx_gain_states;
   num_iq_samples = fb_dc_instr->num_iq_samples;

   FTM_MSG(FTM_HIGH,"The gain states are :-");
   for (iter=0;iter<num_fbrx_gain_states ;iter++) 
   {
       fbrx_gain_state[iter] = fb_dc_instr->fbrx_gain_state[iter];
       FTM_MSG_1(FTM_HIGH,":  %d",fb_dc_instr->fbrx_gain_state[iter]);
   }
  

   internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
   fbrx_tech = (ftm_rf_technology_type)internal_device_cal_data->internal_device_cal_config.tech;
   fbrx_band = internal_device_cal_data->internal_device_cal_config.band;
   curr_tx_ftm_rf_tech = ftm_get_rf_technology();
   FTM_MSG_1(FTM_HIGH, "Current_RF_tech %d", curr_tx_ftm_rf_tech);
   fbrx_rfm_mode = ftm_common_ftm_tech_to_rfm_mode(fbrx_tech);
   if (fbrx_tech != curr_tx_ftm_rf_tech) 
   {
      FTM_MSG_2(FTM_ERROR, "Current RF Tech %d does not match Tech for FBRx Tech %d", curr_tx_ftm_rf_tech, fbrx_tech);
      internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_TECH_MISMATCH;
      return FALSE;
   }
   fbrx_bw = internal_device_cal_data->internal_device_cal_config.bw;

   
   if (curr_tx_ftm_rf_tech != FTM_RF_TECH_LTE) 
   {
      nv_band = ftm_convert_tech_bandnum_to_fbrxnv_band_type(fbrx_tech, fbrx_band);
   }

   #ifdef FEATURE_LTE
   else
   {
      tx_chan = (uint32)internal_device_cal_data->internal_device_cal_config.ref_chan;

      rfcom_bw.lte_bw = rflte_ftm_mc_sm_get_tx_bw();
      FTM_MSG_1(FTM_MED,"TxBW = %d",rfcom_bw.lte_bw);
      nv_band = rflte_util_get_band_from_tx_uarfcn(tx_chan);
      nv_band = rflte_util_get_band_from_tx_uarfcn_bw_path_override(nv_band,tx_chan, 
                                                                    rfcom_bw.lte_bw, 
                                                                    path_override_index);
   }
   #endif
   
   FTM_MSG_1(FTM_MED,"NV_band = %d",nv_band);
   FTM_MSG_1(FTM_HIGH, "Entering FB_DC_CAL, Cmd: %s", fb_dc_instr);
   FTM_MSG_2(FTM_HIGH, "FTM RBRX TECH: %d, FTM FBRX BW: %d", fbrx_tech, fbrx_bw);

   band = rfcommon_fbrx_mc_convert_uint8_to_rfcom_band(fbrx_rfm_mode, (uint8)nv_band);

   device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, 
                                                fbrx_rfm_mode, 
                                                band);

   if (device_ptr == NULL) 
   {
     FTM_MSG(FTM_ERROR, "rfcommon_core_get_tx_device_ptr returns NULL");
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
     return FALSE;
   }

    //Looping over FBRx gain states
    for (iter=0;iter< num_fbrx_gain_states;iter++) 
   {
     
      rfcommon_fbrx_mc_enable_rf_dev(device_ptr, fbrx_gain_state[iter]);
      FTM_MSG_1(FTM_HIGH,"Setup FB path for Gain State %d",fbrx_gain_state[iter]);
      FTM_MSG_3(FTM_HIGH, "ftm_enh_internal_device_cal_fb_dc: fbrxGstate: %d,  numCaps: %d,  numSamps: %d",
             fbrx_gain_state[iter],
             num_captures,
             num_iq_samples
             );
  
   ret_val = ftm_enh_internal_device_cal_fbrx_iq_cap_config(fbrx_tech, fbrx_bw);
   if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
   {
     FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_cap_config failed with error code %d", ret_val );
     rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
     ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(fbrx_tech);
     internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
     return FALSE;
   }
   ret_val = ftm_enh_internal_device_cal_fb_dc_capture_sequence(
      fbrx_rfm_mode,
      rfcom_bw,
      fbrx_gain_state[iter],
      num_captures,
      num_iq_samples,
      debug_mode_en
      );
   if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
   {
     FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fb_dc_capture_sequence failed with error code %d", ret_val );
     rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
     ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(fbrx_tech);
     internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
     return FALSE;
   }
   ret_val = ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(fbrx_tech);
   if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
   {
     FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig failed with error code %d", ret_val );
     rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
     internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
     return FALSE;
   }


   // fill up with test vector
   FTM_MSG(FTM_HIGH,"Setting DC results");
   fbrx_mode_index = rfcommon_fbrx_msm_convert_tx_cfg_to_mode((rfcommon_fbrx_tx_cfg_type)fbrx_bw);
   dc_result->fbrx_dc_res.fbrx_mode_nv = fbrx_mode_index;
   dc_result->fbrx_dc_res.dcval[iter][0]= ftm_enh_int_cal_fbrx_dc_val_i;
   dc_result->fbrx_dc_res.dcval[iter][1]= ftm_enh_int_cal_fbrx_dc_val_q;
   dc_result->fbrx_dc_res.fbrx_gain_state[iter] = fb_dc_instr->fbrx_gain_state[iter];
   FTM_MSG_1(FTM_HIGH, "FBRx Mode %d", fbrx_mode_index);
   FTM_MSG_1(FTM_HIGH, "FBRx Gain state %d", fbrx_gain_state[iter]);
   FTM_MSG_2(FTM_HIGH, " Dcval_i: %d, Dcval_q %d", dc_result->fbrx_dc_res.dcval[iter][0] ,dc_result->fbrx_dc_res.dcval[iter][1]);

   rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
   FTM_MSG(FTM_HIGH,"Disabled FBRx setup ");
   switch (fbrx_tech) 
      {
      case FTM_RF_TECH_CDMA:
         if(nv_band < RFM_CDMA_MAX_BAND)
               ftm_enh_internal_device_cal_fb_dc_global_nv(
                                   debug_mode_en,
                                   fbrx_rfm_mode,
                                   fbrx_tech,
                                   fbrx_band,
                                   nv_band,
                                   fbrx_mode_index,
                                   fb_dc_instr->fbrx_gain_state[iter],
                                   &dc_i_nv,
                                   &dc_q_nv,
                                   ftm_enh_int_cal_fbrx_dc_val_i,
                                   ftm_enh_int_cal_fbrx_dc_val_q,
                                   internal_device_cal_data
                                           );
         break;
   case FTM_RF_TECH_WCDMA:
          if(nv_band < RFCOM_NUM_WCDMA_BANDS)
              ftm_enh_internal_device_cal_fb_dc_global_nv(
                                   debug_mode_en,
                                   fbrx_rfm_mode,  
                                   fbrx_tech,
                                   fbrx_band,
                                   nv_band,
                                   fbrx_mode_index,
                                   fb_dc_instr->fbrx_gain_state[iter],
                                   &dc_i_nv,
                                   &dc_q_nv,
                                   ftm_enh_int_cal_fbrx_dc_val_i,
                                   ftm_enh_int_cal_fbrx_dc_val_q,
                                   internal_device_cal_data
                                           );
          break;
   case FTM_RF_TECH_TDSCDMA:
      
          if(nv_band < RFCOM_NUM_TDSCDMA_BANDS)
             ftm_enh_internal_device_cal_fb_dc_global_nv(
                                   debug_mode_en,
                                   fbrx_rfm_mode,
                                   fbrx_tech,
                                   fbrx_band,
                                   nv_band,
                                   fbrx_mode_index,
                                   fb_dc_instr->fbrx_gain_state[iter],
                                   &dc_i_nv,
                                   &dc_q_nv,
                                   ftm_enh_int_cal_fbrx_dc_val_i,
                                   ftm_enh_int_cal_fbrx_dc_val_q,
                                   internal_device_cal_data
                                           );
          
          break;
   case FTM_RF_TECH_LTE:
        if(nv_band < RFCOM_NUM_LTE_BANDS)
           ftm_enh_internal_device_cal_fb_dc_global_nv(
                                   debug_mode_en,
                                   fbrx_rfm_mode,
                                   fbrx_tech,
                                   fbrx_band,
                                   nv_band,
                                   fbrx_mode_index,
                                   fb_dc_instr->fbrx_gain_state[iter],
                                   &dc_i_nv,
                                   &dc_q_nv,
                                   ftm_enh_int_cal_fbrx_dc_val_i,
                                   ftm_enh_int_cal_fbrx_dc_val_q,
                                   internal_device_cal_data
                                           );
         ftm_enh_internal_device_cal_fb_dc_global_nv(
                                   debug_mode_en,
                                   fbrx_rfm_mode,
                                   fbrx_tech,
                                   fbrx_band,
                                   nv_band,
                                   1,
                                   fb_dc_instr->fbrx_gain_state[iter],
                                   &dc_i_nv,
                                   &dc_q_nv,
                                   ftm_enh_int_cal_fbrx_dc_val_i,
                                   ftm_enh_int_cal_fbrx_dc_val_q,
                                   internal_device_cal_data
                                           ); 
        break;
   default:
      FTM_MSG(FTM_ERROR, "Unknown Tech: Return 0!");
      return FALSE;
   }

     
//Global NV read and overwrite
  /* if (debug_mode_en != 0)
   {
      
     nv_read_write_error = rfcommon_fbrx_nv_read_dc_globals( fbrx_rfm_mode,
                                   nv_band,
                                   fbrx_mode_index,
                                   fb_dc_instr->fbrx_gain_state[iter],
                                   &dc_i_nv,
                                  &dc_q_nv);

     if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
     {
       FTM_MSG_5(FTM_ERROR, "rfcommon_fbrx_nv_read_dc_globals() returned error %d for tech %d band %d fbrx_mode %d fbrx_gain_state %d", nv_read_write_error, fbrx_tech, fbrx_band,fbrx_mode_index,fb_dc_instr->fbrx_gain_state[iter] );
       internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
       return FALSE;
     }
     FTM_MSG_4(FTM_MED, "Current DC NV data for Tech %d Band %d FBRx Mode %d Gain State %d", fbrx_tech, fbrx_band, fbrx_mode_index, fb_dc_instr->fbrx_gain_state[iter]);

     FTM_MSG_2(FTM_MED, "DC value I: %d, Q: %d", dc_i_nv, dc_q_nv);


   }

   nv_read_write_error = rfcommon_fbrx_nv_update_dc_globals( fbrx_rfm_mode,
                                       nv_band,
                                       fbrx_mode_index,
                                       fb_dc_instr->fbrx_gain_state[iter],
                                       ftm_enh_int_cal_fbrx_dc_val_i,
                                       ftm_enh_int_cal_fbrx_dc_val_q);

   if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
   {
     FTM_MSG_5(FTM_ERROR, "rfcommon_fbrx_nv_update_dc_globals() returned error %d for tech %d band %d fbrx_mode %d fbrx_gain_state %d", nv_read_write_error, fbrx_tech, fbrx_band,fbrx_mode_index,fb_dc_instr->fbrx_gain_state[iter] );
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
     return FALSE;
   }


   if (debug_mode_en != 0)
   {
     nv_read_write_error = rfcommon_fbrx_nv_read_dc_globals( fbrx_rfm_mode,
                            nv_band,
                                   fbrx_mode_index,
                                   fb_dc_instr->fbrx_gain_state[iter],
                                   &dc_i_nv,
                                  &dc_q_nv);

     if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
     {
       FTM_MSG_5(FTM_ERROR, "rfcommon_fbrx_nv_read_dc_globals() returned error %d for tech %d band %d fbrx_mode %d fbrx_gain_state %d", nv_read_write_error, fbrx_tech, fbrx_band,fbrx_mode_index,fb_dc_instr->fbrx_gain_state[iter] );
       internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
       return FALSE;
     }
     FTM_MSG_4(FTM_MED, "Current DC NV data for Tech %d Band %d FBRx Mode %d Gain State %d", fbrx_tech, fbrx_band, fbrx_mode_index, fb_dc_instr->fbrx_gain_state[iter]);

     FTM_MSG_2(FTM_MED, "DC value I: %d, Q: %d", dc_i_nv, dc_q_nv);


   }*/
   
  }
   return TRUE;
}

boolean ftm_enh_internal_device_cal_fb_dc_process
(
   uint8 cap_buf_id, 
   uint16 num_iq_samples,
   uint16 discard_samps,
   int32 * dc_val_i, 
   int32 * dc_val_q
)
{
  boolean ret_val = TRUE;

  ret_val &= rfcommon_mdsp_calc_xpt_cap_rx_dc(cap_buf_id, num_iq_samples, discard_samps, dc_val_i, dc_val_q);
  return ret_val;
}

//Global NV read and overwrite
boolean ftm_enh_internal_device_cal_fb_dc_global_nv
(
 uint8 debug_mode_en,
 rfm_mode_enum_type fbrx_rfm_mode,
 ftm_rf_technology_type fbrx_tech,
 uint16 fbrx_band,
 uint16 nv_band,
 uint8 fbrx_mode_index,
 uint8 fbrx_gain_state,
 int32 * dc_i_nv,
 int32 * dc_q_nv,
 int32 dc_val_i,
 int32 dc_val_q,
 ftm_enh_internal_device_cal_data_type *internal_device_cal_data
 )
{
   boolean ret_val=TRUE;
   rfcommon_fbrx_error_type nv_read_write_error;
   if (debug_mode_en != 0)
   {
     
     nv_read_write_error = rfcommon_fbrx_nv_read_dc_globals( fbrx_rfm_mode,
                                   nv_band,
                                   fbrx_mode_index,
                                   fbrx_gain_state,
                                   dc_i_nv,
                                  dc_q_nv);

     if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
     {
       FTM_MSG_5(FTM_ERROR, "rfcommon_fbrx_nv_read_dc_globals() returned error %d for tech %d band %d fbrx_mode %d fbrx_gain_state %d", nv_read_write_error, fbrx_tech, fbrx_band,fbrx_mode_index,fbrx_gain_state );
       internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
       ret_val= FALSE;
     }
     FTM_MSG_4(FTM_MED, "Current DC NV data for Tech %d Band %d FBRx Mode %d Gain State %d", fbrx_tech, fbrx_band, fbrx_mode_index, fbrx_gain_state);

     FTM_MSG_2(FTM_MED, "DC value I: %d, Q: %d", dc_i_nv, dc_q_nv);


   }
    FTM_MSG(FTM_HIGH,"Update dc globals");
 
   nv_read_write_error = rfcommon_fbrx_nv_update_dc_globals( fbrx_rfm_mode,
                                       nv_band,
                                       fbrx_mode_index,
                                       fbrx_gain_state,
                                       dc_val_i,
                                       dc_val_q);

   if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
   {
     FTM_MSG_5(FTM_ERROR, "rfcommon_fbrx_nv_update_dc_globals() returned error %d for tech %d band %d fbrx_mode %d fbrx_gain_state %d", nv_read_write_error, fbrx_tech, fbrx_band,fbrx_mode_index,fbrx_gain_state);
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
     ret_val= FALSE;
   }


   if (debug_mode_en != 0)
   {
     nv_read_write_error = rfcommon_fbrx_nv_read_dc_globals( fbrx_rfm_mode,
                            nv_band,
                                   fbrx_mode_index,
                                   fbrx_gain_state,
                                   dc_i_nv,
                                  dc_q_nv);

     if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
     {
       FTM_MSG_5(FTM_ERROR, "rfcommon_fbrx_nv_read_dc_globals() returned error %d for tech %d band %d fbrx_mode %d fbrx_gain_state %d", nv_read_write_error, fbrx_tech, fbrx_band,fbrx_mode_index,fbrx_gain_state );
       internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
       ret_val=FALSE;
     }
     FTM_MSG_4(FTM_MED, "Current DC NV data for Tech %d Band %d FBRx Mode %d Gain State %d", fbrx_tech, fbrx_band, fbrx_mode_index, fbrx_gain_state);

     FTM_MSG_2(FTM_MED, "DC value I: %d, Q: %d", dc_i_nv, dc_q_nv);


   }
   return ret_val;
}
#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

/*! @} */


