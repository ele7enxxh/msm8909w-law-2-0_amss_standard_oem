/*!
  @file
  ftm_common_enh_internal_calibration.c

  @brief

*/
/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_enh_internal_device_calibration.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/15/14    cd     Fix compiler warnings
09/09/14    cri    Fix for FBRx Cal to ESC Cal transisions
08/18/14    jmf    Add new API for XPT IQ capture without wait
08/05/14    xsh    FTM Rsp Payload memory allocation and copy update to add RSB validation data 
08/05/14    xsh    Seperate efs write from normal rsb cal
08/05/14    xsh    dynamic allocate response buffer for variable rsb cal results to support seperate efs write
07/14/14    xsh    Increased common xpt iq cap waiting time 
5/30/14 xsh add different gain state rsb cal support
5/29/14 xsh add lte scell rx rsb cal support
05/20/14   cri     Move C2k and TDS to FTM handlers for IQ capture
05/19/14   cri     Move fbrx to FTM dispatch interface for IQ capture
05/05/14    aa     Move fbrx lm buffer settings to modem component 
04/08/14   jmf     Updates to accommodate error code returns
03/14/14   jmf     Fixes to IQ capture / Function for band conversion
02/21/14   jmf     C2k FBRx Caps for Droop/DC/RSB Cal
02/14/14   br      Addition of FBRX DC cal functionality
02/06/14   jmf     Functional WCDMA FBRx Capture
10/02/13   xsh     Initial Revision

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
#include "ftm_enh_internal_device_cal_rsb.h"
#include "ftm_enh_internal_device_cal_droop.h"
#include "ftm_enh_internal_device_cal_fb_dc.h"
//#include "rfm_internal.h"
#include "DALSys.h" 
#include "rfcommon_time_profile.h" 
#include "zlib.h"
#include "rfcommon_mc.h"
#include "rflm_api_fbrx_fw_hal.h"
#include "rflm_api_fbrx.h"

#ifdef FEATURE_WCDMA
#include "ftm_wcdma_calv3.h"
#include "ftm_wcdma_ctl.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_calv3.h"
#include "ftm_tdscdma_ctl.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
#include "rflte_ftm_calibration.h"
#include "ftm_lte_xpt.h"
#include "rflte_ftm_mc.h"
#include "rflte_state.h"
#endif /* FEATURE_LTE */

#ifdef FEATURE_CDMA
#include "ftm_1x_xpt.h"
#include "rf_cdma_data.h"
#include "ftm_1x_calibration.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef FEATURE_WCDMA
/* Re-use XPT LM buffer for FBRx Captures of XPT style */
extern ftm_lm_buf_type ftm_wcdma_xpt_fb_path_buffer;
#endif 

#ifdef FEATURE_TDSCDMA
/* Re-use XPT LM buffer for FBRx Captures of XPT style */
extern ftm_lm_buf_type ftm_tdscdma_xpt_fb_path_buffer;
#endif 

static boolean ftm_enh_internal_device_cal_check_techbw
(
   ftm_rf_technology_type ftm_rf_tech,
   uint8 bw,
   rfcommon_fbrx_tx_cfg_type * rfcommon_fbrx_tech_bw
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Diag Response Packet Limit Backoff
 
  @details
  This Limite_backoff is used as a backoff value from the maximum allowed response packet.
  This backoff is used so that cal results data take up memeory less than Diag 
  Limit leaving room for response packet overheads. The units of this backoff 
  is bytes.
*/
#define FTM_ENH_INTERNAL_DEVICE_CAL_DIAG_LIMIT_BACKOFF 100

/*
extern ftm_enh_internal_device_cal_data_type*
ftm_common_data_get_enh_internal_device_cal_ptr
(
  void
);
*/

ftm_enh_internal_device_cal_dispatch_info_type ftm_enh_internal_device_cal_dispatch_table[ENH_INTERNAL_DEVICE_CAL_MAX];// currently just support one internal cal type per request


/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_enh_internal_device_cal_fbrx_iq_capture_config
  
 
  @details: fbrx iq capture configuration
  
  @param 
   input: 
   output: 
  
  @return
  Status of configuration
*/


static boolean ftm_enh_internal_device_cal_check_techbw
(
   ftm_rf_technology_type ftm_rf_tech,
   uint8 bw,
   rfcommon_fbrx_tx_cfg_type * rfcommon_fbrx_tech_bw
)
{
  boolean ret_val = TRUE;

  if (rfcommon_fbrx_tech_bw == NULL)
  {
    FTM_MSG_2(FTM_ERROR, "Return pointer is NULL for FBRx Tech %d BW %d", ftm_rf_tech, bw);
    return FALSE;
  }

  switch (ftm_rf_tech)
  {
  case FTM_RF_TECH_CDMA:
    switch ((rfcommon_fbrx_tx_cfg_type)bw)
    {
	case RFCOMMON_FBRX_1X:
	case RFCOMMON_FBRX_MCDO_3X:
	case RFCOMMON_FBRX_SB_SVDO:
	case RFCOMMON_FBRX_MCDO_7X:
      *rfcommon_fbrx_tech_bw = bw;
      break;
    default:
      FTM_MSG_2(FTM_ERROR, "ftm_enh_internal_device_cal_check_techbw: Unknown FBRx BW type %d for Tech %d", bw, ftm_rf_tech);
      return FALSE;
    }
    break;
  case FTM_RF_TECH_WCDMA:
    switch ((rfcommon_fbrx_tx_cfg_type)bw)
    {
    case RFCOMMON_FBRX_UMTS_SC:
    case RFCOMMON_FBRX_UMTS_DC:
      *rfcommon_fbrx_tech_bw = bw;
      break;
    default:
      FTM_MSG_2(FTM_ERROR, "ftm_enh_internal_device_cal_check_techbw: Unknown FBRx BW type %d for Tech %d", bw, ftm_rf_tech);
      return FALSE;
    }
    break;
  case FTM_RF_TECH_LTE:
    switch ((rfcommon_fbrx_tx_cfg_type)bw)
    {
    case RFCOMMON_FBRX_LTE_1P4:
    case RFCOMMON_FBRX_LTE_3:
    case RFCOMMON_FBRX_LTE_5:
    case RFCOMMON_FBRX_LTE_10:
    case RFCOMMON_FBRX_LTE_15:
    case RFCOMMON_FBRX_LTE_20:
    case RFCOMMON_FBRX_LTE_20_PLUS_20:
      *rfcommon_fbrx_tech_bw = bw; 
      break;
    default:
      FTM_MSG_2(FTM_ERROR, "ftm_enh_internal_device_cal_check_techbw: Unknown FBRx BW type %d for Tech %d", bw, ftm_rf_tech);
      return FALSE;
    }
    break;
  case FTM_RF_TECH_TDSCDMA:
    switch ((rfcommon_fbrx_tx_cfg_type)bw)
    {
    case RFCOMMON_FBRX_TDS:
      *rfcommon_fbrx_tech_bw = bw;
      break;
    default:
      FTM_MSG_2(FTM_ERROR, "ftm_enh_internal_device_cal_check_techbw: Unknown FBRx BW type %d for Tech %d", bw, ftm_rf_tech);
      return FALSE;
    }
    break;
  default:
    FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_check_techbw: Unsupported Tech Type %d", ftm_rf_tech);
  }
  return ret_val;
}

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fbrx_iq_cap_config
(
   ftm_rf_technology_type tech,
   uint8 bw
)
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  boolean api_status = TRUE;

  rfcommon_fbrx_tx_cfg_type rfcommon_fbrx_tech_bw = RFCOMMON_FBRX_LTE_10;

  lm_handle_type fbrx_lm_handle;
  rfcommon_fbrx_error_type fbrx_lm_update_status;

  #ifdef FEATURE_LTE
  rflte_ftm_mc_sm_type     *lte_ftm_mc_sm_ptr      = NULL; 
  uint32 fbrx_carrier_idx = 0;
  #endif

  api_status = ftm_enh_internal_device_cal_check_techbw(tech, bw, &rfcommon_fbrx_tech_bw);
  if (api_status == FALSE)
  {
    FTM_MSG_2(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_cap_config: invalid tech-bw enum combination for tech %d , tech_bw %d", tech, bw);
    return FTM_ENH_INTERNAL_DEVICE_CAL_INVALID_TECHBW;
  }

  FTM_MSG_4(FTM_HIGH, "ftm_enh_internal_device_cal_fbrx_mode_for_techbw return %d: Obtained FBRxTechBW %d for Tech %d BW %d", ret_val, rfcommon_fbrx_tech_bw, tech, bw);
  rfcommon_mc_allocate_dpd_buffer();

  switch (tech)
  {
  #ifdef FEATURE_WCDMA
  case FTM_RF_TECH_WCDMA:
    api_status = ftm_wcdma_xpt_cal_config(RFM_DEVICE_0, RFM_DEVICE_0, (uint8)FTM_XPT_CONFIG_FBRX, TRUE);
    FTM_MSG_4(FTM_HIGH, "ftm_wcdma_xpt_cal_config(dev %d, dev_fb %d, xpt_mode %d) returned %d", RFM_DEVICE_0, RFM_DEVICE_0, FTM_XPT_CONFIG_FBRX , api_status );
    if (ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated)
    {
	  // Update Static Settings, do we suppose to update rxlm buffer with static value from SS here?
      api_status &= (rfcommon_fbrx_msm_update_static_settings( ftm_wcdma_xpt_fb_path_buffer.buf_idx, rfcommon_fbrx_tech_bw ) == RFCOMMON_FBRX_SUCCESS);
      FTM_MSG_3(FTM_HIGH, "rfcommon_fbrx_lm_update_static_settings (buf_id %d, tech_bw %d) success status %d ", 
                ftm_wcdma_xpt_fb_path_buffer.buf_idx, 
                rfcommon_fbrx_tech_bw, 
                api_status);
      
      // Update dynamic Settings
      api_status &=  (rfcommon_fbrx_msm_update_dynamic_settings_for_xptcap( ftm_wcdma_xpt_fb_path_buffer.buf_idx ) == RFCOMMON_FBRX_SUCCESS);
      FTM_MSG_2(FTM_HIGH, "rfcommon_fbrx_lm_update_dynamic_settings_for_xptcap (buf_id %d ) success status %d ", 
                ftm_wcdma_xpt_fb_path_buffer.buf_idx, 
                api_status);
    }
    else
    {
      api_status = FALSE;
    }
	
    break;
  #endif
  #ifdef FEATURE_LTE
  case FTM_RF_TECH_LTE:
    lte_ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();
    if(lte_ftm_mc_sm_ptr == NULL)
    {
      FTM_MSG( FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_cap_config: cannot get LTE FTM SM handle" );
      return FTM_ENH_INTERNAL_DEVICE_CAL_IQ_CAP_CONFIG_FAILURE;
    }
	api_status = ftm_lte_xpt_cal_configure(RFM_DEVICE_0,RFM_DEVICE_0,FTM_XPT_CONFIG_FBRX);
	FTM_MSG_4(FTM_HIGH, "ftm_lte_xpt_cal_configure(dev %d, dev_fb %d, xpt_mode %d) returned %d", RFM_DEVICE_0, RFM_DEVICE_0, 3, api_status );
	fbrx_carrier_idx = rflte_ftm_mc_get_carrier_index_from_device(RFM_DEVICE_0);
	FTM_MSG_2(FTM_HIGH, "rflte_ftm_mc_get_carrier_index_from_device obtained fbrx_carrier_idx %d for device %d", fbrx_carrier_idx, RFM_DEVICE_0);


    if (rflte_state_get_fbrxlm_buffer_idx(fbrx_carrier_idx) != LM_BUFFER_NOT_ALLOCATED)
    {
	  // Update Static Settings, shouldn't we remove it?
      api_status &= (rfcommon_fbrx_msm_update_static_settings( rflte_state_get_fbrxlm_buffer_idx(fbrx_carrier_idx) , rfcommon_fbrx_tech_bw ) == RFCOMMON_FBRX_SUCCESS);
      FTM_MSG_3(FTM_HIGH, "rfcommon_fbrx_lm_update_static_settings (buf_id %d, tech_bw %d) success status %d ", 
                rflte_state_get_fbrxlm_buffer_idx(fbrx_carrier_idx) , 
                rfcommon_fbrx_tech_bw, 
                api_status);

      // Update dynamic Settings
      api_status &=  (rfcommon_fbrx_msm_update_dynamic_settings_for_xptcap( rflte_state_get_fbrxlm_buffer_idx(fbrx_carrier_idx) ) == RFCOMMON_FBRX_SUCCESS);
      FTM_MSG_2(FTM_HIGH, "rfcommon_fbrx_lm_update_dynamic_settings_for_xptcap (buf_id %d ) success status %d ", 
                rflte_state_get_fbrxlm_buffer_idx(fbrx_carrier_idx) , 
                api_status);  
    }
    else
    {
      api_status = FALSE;
    }
	
    break;
  #endif
  #ifdef FEATURE_CDMA
  case FTM_RF_TECH_CDMA:
  	
    fbrx_lm_handle = rf_cdma_data_get_fbrx_lm_handle(RFM_DEVICE_0);

	// do we need always load static data from SS?    
	fbrx_lm_update_status = rfcommon_fbrx_msm_update_static_settings( fbrx_lm_handle, rfcommon_fbrx_tech_bw );
    api_status = (fbrx_lm_update_status == RFCOMMON_FBRX_SUCCESS);

    FTM_MSG_3(FTM_HIGH, "rfcommon_fbrx_lm_update_static_settings (buf_id %d, tech_bw %d) success status %d ", 
              (uint8)fbrx_lm_handle, 
              rfcommon_fbrx_tech_bw, 
              api_status);

    fbrx_lm_update_status = rfcommon_fbrx_msm_update_dynamic_settings_for_xptcap( fbrx_lm_handle ) ;
    api_status &=  (fbrx_lm_update_status == RFCOMMON_FBRX_SUCCESS);
    FTM_MSG_2(FTM_HIGH, "rfcommon_fbrx_lm_update_dynamic_settings_for_xptcap (buf_id %d ) success status %d ", 
              (uint8)fbrx_lm_handle, 
              api_status);

    break;
  #endif
   #ifdef FEATURE_TDSCDMA
  case FTM_RF_TECH_TDSCDMA:
    api_status = ftm_tdscdma_xpt_cal_config(RFM_DEVICE_0, RFM_DEVICE_0, (uint8)FTM_XPT_CONFIG_FBRX, TRUE);
    FTM_MSG_4(FTM_HIGH, "ftm_tdscdma_xpt_cal_config(dev %d, dev_fb %d, xpt_mode %d) returned %d", RFM_DEVICE_0, RFM_DEVICE_0, FTM_XPT_CONFIG_FBRX , api_status );
    if (ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated)
    {
      // Update Static Settings
       api_status &= (rfcommon_fbrx_msm_update_static_settings( ftm_tdscdma_xpt_fb_path_buffer.buf_idx, rfcommon_fbrx_tech_bw ) == RFCOMMON_FBRX_SUCCESS);
      
	   FTM_MSG_3(FTM_HIGH, "rfcommon_fbrx_lm_update_static_settings (buf_id %d, tech_bw %d) success status %d ", 
                ftm_tdscdma_xpt_fb_path_buffer.buf_idx, 
                rfcommon_fbrx_tech_bw, 
                api_status);
         
      // Update dynamic Settings
      api_status &=  (rfcommon_fbrx_msm_update_dynamic_settings_for_xptcap( ftm_tdscdma_xpt_fb_path_buffer.buf_idx ) == RFCOMMON_FBRX_SUCCESS);
      FTM_MSG_2(FTM_HIGH, "rfcommon_fbrx_lm_update_dynamic_settings_for_xptcap (buf_id %d ) success status %d ", 
                ftm_tdscdma_xpt_fb_path_buffer.buf_idx, 
                api_status);
    }
    else
    {
      api_status = FALSE;
    }
    break;
  #endif
  default:
    FTM_MSG_1(FTM_ERROR, "Tech Type %d not yet supported for ftm_enh_internal_device_cal_fbrx_iq_cap_config", tech);
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
  }

  if (api_status == FALSE)
  {
    FTM_MSG_1(FTM_ERROR, "IQ Capture Config through RxLM Buffer allocation and/or update failed for Tech %d", tech);
    ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(tech);
    return FTM_ENH_INTERNAL_DEVICE_CAL_IQ_CAP_CONFIG_FAILURE;
  }


  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_enh_internal_device_cal_fbrx_iq_capture_deconfig
  
 
  @details: fbrx iq capture de-configuration
  
  @param 
   input: 
   output:
  
  @return
  Status of de-configuration
*/

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig
(
  ftm_rf_technology_type ftm_rf_tech
)
{

  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  boolean api_status = TRUE;

  switch (ftm_rf_tech)
  {
  #ifdef FEATURE_WCDMA
  case FTM_RF_TECH_WCDMA:
    api_status = ftm_wcdma_xpt_cal_config(RFM_DEVICE_0, RFM_DEVICE_0, (uint8)FTM_XPT_CONFIG_FBRX, FALSE);
    break;
  #endif
  #ifdef FEATURE_LTE
  case FTM_RF_TECH_LTE:
    api_status = ftm_lte_xpt_cal_deconfigure(RFM_DEVICE_0, RFM_DEVICE_0, FTM_XPT_CONFIG_FBRX );
    break;
  #endif
  #ifdef FEATURE_CDMA
  case FTM_RF_TECH_CDMA:
    FTM_MSG(FTM_HIGH, "ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig currently does not do anything!");
    break;
  #endif
  #ifdef FEATURE_TDSCDMA
  case FTM_RF_TECH_TDSCDMA:
    api_status = ftm_tdscdma_xpt_cal_config(RFM_DEVICE_0, RFM_DEVICE_0, (uint8)FTM_XPT_CONFIG_FBRX, FALSE);
    break;
  #endif
  default:
    FTM_MSG_1(FTM_ERROR, "Tech Type %d not yet supported for ftm_enh_internal_device_cal_fbrx_iq_capture_deconfig", ftm_rf_tech);
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
  }

  if (api_status == FALSE)
  {
    FTM_MSG_1(FTM_ERROR, "IQ Capture Deconfig through RxLM Buffer deallocation failed for Tech %d", ftm_rf_tech);
    return FTM_ENH_INTERNAL_DEVICE_CAL_IQ_CAP_CONFIG_FAILURE;
  }

  return ret_val;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_enh_internal_device_cal_fbrx_iq_capture
  
 
  @details: fbrx iq capture
  
  @param 
   input: ftm_rf_tech, iq_cap_instr_ptr, cap_buf_id_ptr, debug_mode_en, filename
   output: (cap_buf_id_ptr)
  
  @return
  Status of initialization
*/

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fbrx_iq_capture
(
   ftm_rf_technology_type ftm_rf_tech, 
   ftm_cal_xpt_instr_payload_iq_capture_type * iq_cap_instr_ptr, 
   uint8  * cap_buf_id_ptr, 
   uint8 debug_mode_en, 
   char * filename
)
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  boolean api_status = TRUE;
  boolean capture_done;
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  rf_common_xpt_buffer_id_type buffer_ids;
  uint8 iq_buffer_index= -1;

  #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
  rf_cdma_ept_data_type *ept_data; 
  #endif

  /* Send IQ Capture command only if previous capture is done */
  if ( rfcommon_xpt_is_capture_done() == TRUE )
  {
    api_status &= rfcommon_xpt_set_capture_in_progress();

    api_status &= rfcommon_core_xpt_get_buffers(&buffer_ids);

    /* Dispatch Tech-Specific Calls with updated params */
    switch ( ftm_rf_tech )
    {
    #ifdef FEATURE_WCDMA
    case FTM_RF_TECH_WCDMA:
      api_status = ftm_wcdma_xpt_iq_capture(iq_cap_instr_ptr->primary_device,
                                            iq_cap_instr_ptr->feedback_device,
                                            iq_cap_instr_ptr->capture_type,
                                            iq_cap_instr_ptr->num_samples,
                                            iq_cap_instr_ptr->first_trigger,
                                            iq_cap_instr_ptr->last_trigger,
                                            iq_cap_instr_ptr->proc_flag,
                                            buffer_ids,
                                            NULL);
      FTM_MSG_1(FTM_HIGH, "ftm_wcdma_cal_xpt_iq_capture returns %d", api_status);
      break;
    #endif
    #ifdef FEATURE_LTE
    case FTM_RF_TECH_LTE:
      api_status = ftm_lte_xpt_iq_capture(iq_cap_instr_ptr->primary_device,
                                          iq_cap_instr_ptr->feedback_device,
                                          iq_cap_instr_ptr->num_samples,
                                          iq_cap_instr_ptr->first_trigger,
                                          iq_cap_instr_ptr->last_trigger,
                                          iq_cap_instr_ptr->proc_flag,
                                          iq_cap_instr_ptr->capture_type,
                                          buffer_ids.capt_buf_id,
                                          buffer_ids.res_buf_id,
                                          NULL);

      FTM_MSG_1(FTM_HIGH, "rflte_ftm_calibration_xpt_iq_capture returns %d", api_status);
      break;
    #endif
    #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
    case FTM_RF_TECH_CDMA:
      ept_data = rf_cdma_get_ept_data();
      ept_data->cal_data.count_iq_capture = 0; 
      // Need to reset above count to 0, since right now we have single cap sequence functional, remove this when multi-cap functional
      iq_cap_instr_ptr->feedback_device = RFM_DEVICE_0;
      api_status = ftm_1x_xpt_iq_capture( iq_cap_instr_ptr->primary_device,
                                          iq_cap_instr_ptr->feedback_device,
                                          iq_cap_instr_ptr->capture_type,
                                          iq_cap_instr_ptr->num_samples,
                                          iq_cap_instr_ptr->first_trigger,
                                          iq_cap_instr_ptr->last_trigger,
                                          iq_cap_instr_ptr->proc_flag,
                                          buffer_ids,
                                          NULL);

      FTM_MSG_1(FTM_HIGH, "ftm_1x_cal_xpt_iq_capture returns %d", api_status);
      break;
    #endif 
    #ifdef FEATURE_TDSCDMA
    case FTM_RF_TECH_TDSCDMA:
      api_status = ftm_tdscdma_xpt_iq_capture(iq_cap_instr_ptr->primary_device, 
                                              iq_cap_instr_ptr->feedback_device,
                                              iq_cap_instr_ptr->capture_type,
                                              iq_cap_instr_ptr->num_samples,
                                              (boolean)(iq_cap_instr_ptr->first_trigger),
                                              (boolean)(iq_cap_instr_ptr->last_trigger),
                                              iq_cap_instr_ptr->proc_flag, 
                                              buffer_ids,
                                              NULL);

      FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_cal_xpt_iq_capture returns %d", api_status);
      break;
    #endif
    default:
      FTM_MSG(FTM_ERROR, "Unsupported Tech Type: Return error handle");
      return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
    }

    if (api_status == FALSE)
    {
      FTM_MSG_1(FTM_ERROR, "IQ Capture failed for tech %d", ftm_rf_tech);
      return FTM_ENH_INTERNAL_DEVICE_CAL_IQ_CAPTURE_FAILURE;
    }
    /****************************************************************/
    /*            Get MDSP Buffer Index to save to EFS              */
    /****************************************************************/
    api_status = rfcommon_xpt_fetch_iq_capture_data(&iq_cap_data);
    *cap_buf_id_ptr = iq_cap_data.buffer_id.capt_buf_id;
    FTM_MSG_1(FTM_HIGH, "Current Cap Index: %d", *cap_buf_id_ptr);
   
    capture_done = rfcommon_xpt_wait_for_capture_done( 500, 6000, &iq_buffer_index );
    FTM_MSG_1(FTM_HIGH, "Current Cap Done Index: %d", iq_buffer_index);
    if( capture_done == FALSE )
    {
      FTM_MSG_1( FTM_ERROR, " Sample capture did not complete for tech %d in 5 ms", ftm_rf_tech );
      return FTM_ENH_INTERNAL_DEVICE_CAL_INSUFFICIENT_CAP_TIME;
    }
     
    if (debug_mode_en!=0)
    {
      if (filename==NULL)
      {
        FTM_MSG(FTM_ERROR, "Filename pointer is NULL");
        return FTM_ENH_INTERNAL_DEVICE_CAL_EMPTY_FILENAME;
      }
      api_status = rfcommon_mc_save_xpt_iq_data( iq_buffer_index,iq_cap_instr_ptr->num_samples,&filename[0]);
      if (api_status == FALSE)
      {
        FTM_MSG_1(FTM_ERROR, "rfcommon_mc_save_xpt_iq_data return %d", api_status);
        return FTM_ENH_INTERNAL_DEVICE_CAL_EFS_FILESAVE_FAILED;
      }
    }
  } /* is capture done */
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_capture: "
                        "Previous IQ capture is not complete" );
   }


  return ret_val;
   
}



ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fbrx_fastmulti_iq_capture
(
   ftm_rf_technology_type ftm_rf_tech, 
   ftm_cal_xpt_instr_payload_iq_capture_type * iq_cap_instr_ptr, 
   uint8  * cap_buf_id_ptr,
   uint32 * samp_rate
)
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  boolean api_status = TRUE;
  boolean capture_done;
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  rf_common_xpt_buffer_id_type buffer_ids;
  uint8 iq_buffer_index= -1;

  /* Send IQ Capture command only if previous capture is done */
  if ( rfcommon_xpt_is_capture_done() == TRUE )
  {
    api_status &= rfcommon_xpt_set_capture_in_progress();

    api_status &= rfcommon_core_xpt_get_buffers(&buffer_ids);

    /* Dispatch Tech-Specific Calls with updated params */
    switch ( ftm_rf_tech )
    {
    #ifdef FEATURE_WCDMA
    case FTM_RF_TECH_WCDMA:
      api_status = ftm_wcdma_xpt_iq_capture(iq_cap_instr_ptr->primary_device,
                                            iq_cap_instr_ptr->feedback_device,
                                            iq_cap_instr_ptr->capture_type,
                                            iq_cap_instr_ptr->num_samples,
                                            iq_cap_instr_ptr->first_trigger,
                                            iq_cap_instr_ptr->last_trigger,
                                            iq_cap_instr_ptr->proc_flag,
                                            buffer_ids,
                                            samp_rate);
      FTM_MSG_1(FTM_HIGH, "ftm_wcdma_cal_xpt_iq_capture returns %d", api_status);
      break;
    #endif
    #ifdef FEATURE_LTE
    case FTM_RF_TECH_LTE:
      api_status = ftm_lte_xpt_iq_capture(iq_cap_instr_ptr->primary_device,
                                          iq_cap_instr_ptr->feedback_device,
                                          iq_cap_instr_ptr->num_samples,
                                          iq_cap_instr_ptr->first_trigger,
                                          iq_cap_instr_ptr->last_trigger,
                                          iq_cap_instr_ptr->proc_flag,
                                          iq_cap_instr_ptr->capture_type,
                                          buffer_ids.capt_buf_id,
                                          buffer_ids.res_buf_id,
                                          samp_rate);
      break;
    #endif
    #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
    case FTM_RF_TECH_CDMA:
      iq_cap_instr_ptr->feedback_device = RFM_DEVICE_0;
      api_status = ftm_1x_xpt_iq_capture( iq_cap_instr_ptr->primary_device,
                                          iq_cap_instr_ptr->feedback_device,
                                          iq_cap_instr_ptr->capture_type,
                                          iq_cap_instr_ptr->num_samples,
                                          iq_cap_instr_ptr->first_trigger,
                                          iq_cap_instr_ptr->last_trigger,
                                          iq_cap_instr_ptr->proc_flag,
                                          buffer_ids,
                                          samp_rate);

      FTM_MSG_1(FTM_HIGH, "ftm_1x_cal_xpt_iq_capture returns %d", api_status);
      break;
    #endif 
    #ifdef FEATURE_TDSCDMA
    case FTM_RF_TECH_TDSCDMA:
      api_status = ftm_tdscdma_xpt_iq_capture(iq_cap_instr_ptr->primary_device, 
                                              iq_cap_instr_ptr->feedback_device,
                                              iq_cap_instr_ptr->capture_type,
                                              iq_cap_instr_ptr->num_samples,
                                              (boolean)(iq_cap_instr_ptr->first_trigger),
                                              (boolean)(iq_cap_instr_ptr->last_trigger),
                                              iq_cap_instr_ptr->proc_flag, 
                                              buffer_ids,
                                              samp_rate);

      FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_cal_xpt_iq_capture returns %d", api_status);
      break;
    #endif
    default:
      FTM_MSG(FTM_ERROR, "Unsupported Tech Type: Return error handle");
      return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
    }

    if (api_status == FALSE)
    {
      FTM_MSG_1(FTM_ERROR, "IQ Capture failed for tech %d", ftm_rf_tech);
      return FTM_ENH_INTERNAL_DEVICE_CAL_IQ_CAPTURE_FAILURE;
    }
    /****************************************************************/
    /*            Get MDSP Buffer Index to save to EFS              */
    /****************************************************************/
    api_status = rfcommon_xpt_fetch_iq_capture_data(&iq_cap_data);
    *cap_buf_id_ptr = iq_cap_data.buffer_id.capt_buf_id;

  } /* is capture done */
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_capture: "
                        "Previous IQ capture is not complete" );
    ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_INSUFFICIENT_CAP_TIME;
  }


  return ret_val;
   
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  clear Calibration results buffer
 
  @details
  This function will clear internal cal results buffer and initialize all result parameters. 
 
  This function should be called only after ftm rsp buffer is assigned and before ftm_rsp package is sent back to 
  PC side. 
 
  @return
 
*/
void
ftm_enh_internal_device_calibration_clear_result_buffer
(
  void
)
{
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data; /* Internal Cal Data */

  /* Get Pointer to Internal Cal Data */
  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

   /* Reset the Result helper variable  to reset state */
  internal_device_cal_data->internal_device_cal_result.result_rev = 0;     /* reset result_revision as 0 */
  internal_device_cal_data->internal_device_cal_result.write_ptr = NULL;  /* NULL Write Pointer */
  internal_device_cal_data->internal_device_cal_result.total_result_sz = 0; /* reset total result size */
  internal_device_cal_data->internal_device_cal_result.res_mask = 0; /* reset the results map as 0, */
  internal_device_cal_data->internal_device_cal_result.res_max_len = 0;
  if(internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr!=NULL)
  {
	modem_mem_free(internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr,MODEM_MEM_CLIENT_RFA);
	internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr = NULL;
  }
  memset(internal_device_cal_data->internal_device_cal_result.buff_ptr, 0, MAX_ENH_INTERNAL_DEVICE_CAL_RES_SIZE	); /* reset result buffer */
  internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len = 0;
  internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len = 0;
  return;

} /* ftm_enh_internal_device_calibration_clear_result_buffer */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Internal Device Calibration data Configuration

  @details

  
*/


void 
ftm_enh_internal_device_calibration_data_init
(
  void
)
{
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data; /* Internal Cal Data */
	
	/* Get Pointer to Internal Cal Data */
  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

  internal_device_cal_data->internal_device_cal_config.dbg_mode_en = FALSE;
  internal_device_cal_data->internal_device_cal_config.res_ver = 0;  
  internal_device_cal_data->internal_device_cal_config.flag = 0;
  internal_device_cal_data->internal_device_cal_config.tot_instr_size = 0;
  internal_device_cal_data->internal_device_cal_config.internal_cal_mask = 0;
  internal_device_cal_data->internal_device_cal_config.tech = FTM_RF_TECH_LTE;
  internal_device_cal_data->internal_device_cal_config.band = RFCOM_BAND_LTE_B1;
  internal_device_cal_data->internal_device_cal_config.bw = (uint8)RFCOMMON_FBRX_LTE_10 ; // has to be of bw type for FBRx
  internal_device_cal_data->internal_device_cal_config.ref_chan = 18300;
  internal_device_cal_data->internal_device_cal_config.device_mask =0;
  memset(internal_device_cal_data->internal_device_cal_config.instruction,0,MAX_ENH_INTERNAL_DEVICE_CAL_INSTR_SIZE);
  internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr = NULL;

  
  ftm_enh_internal_device_calibration_clear_result_buffer();

  internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_NOT_STARTED;
  internal_device_cal_data->internal_device_cal_status.error_log = 0; 
  internal_device_cal_data->internal_device_cal_status.error_mask = 0;
  internal_device_cal_data->internal_device_cal_status.is_active = FALSE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push internal cal instruction to internal_cal_configure data

  @details
  
   
  @param instr_size
  Total size of all internal calibration instructions in this packet

  @param instr_ptr
  Pointer to the instruction of the packet. This pointer will be used to
  copy RF calibration configuration from Request packet to the Node.

  @return
  Error code produced during allocation of packet node.
*/
ftm_enh_internal_device_cal_error_code_type
ftm_enh_internal_device_calibration_push_instruction
(
  uint16 instr_size,
  uint8 *instr_ptr
)
{
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data; /* internal_cal Data */

  /* Get Pointer to internal_cal Data */
  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

  internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS; 

  if(instr_size > MAX_ENH_INTERNAL_DEVICE_CAL_INSTR_SIZE)
  {
     
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_INSTRUCTION_OVERSIZE; 
	 internal_device_cal_data->internal_device_cal_config.tot_instr_size = instr_size;
  }
  else
  {
	memscpy( internal_device_cal_data->internal_device_cal_config.instruction,
				 instr_size, instr_ptr, instr_size );
	internal_device_cal_data->internal_device_cal_config.tot_instr_size = instr_size;
		/* Use SUCCESS as error code */
	internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_PACKET_COPIED;
  }
  return internal_device_cal_data->internal_device_cal_status.error_log;

} /* ftm_enh_internal_device_calibration_push_configuration */


/*----------------------------------------------------------------------------*/
/*!
  @function name
   ftm_enh_internal_calibration_configure
   
  @brief
  Core function to configure internal calibration data structure based on ftm_enh_internal_cal ftm cmd

  @details
  This function will configure the Internal Calibration data structure based on the data received from PC side. 

    
  @param header
  Pointer to the data structure containing the command payload for internal calibration configuration
 
  @return
    error_code indicating the status of internal cal
*/
ftm_enh_internal_device_cal_error_code_type
ftm_enh_internal_device_calibration_configure
(
  ftm_enh_internal_device_cal_req_pkt_type* header
)
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  
  ftm_enh_internal_device_cal_req_payload_type *cal_pkt; /* Internal Calibration Packet */

  //ftm_common_timer_status_type timer_status = FTM_TIMER_FAILURE;  // time out will be considered later

  ftm_enh_internal_device_cal_data_type *internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

  FTM_MSG_7(FTM_MED,"enh_internal_device_cal: commonAPI FTM cmd: cmd %d sub_sys:%d"
  	"mode:%d, ftm_command_id:%d" 
  	"req_len:%d, rsp_len:%d, enh_internal_cal_param:0x%x",
         header->cmd, header->sub_sys,
         header->mode, header->cdma_cmd,
         header->req_len, header->rsp_len, header->enh_internal_cal_param
        );


  internal_device_cal_data->internal_device_cal_status.is_active = TRUE; // set internal-cal flag active true
  /* Typecast cal_param to extract the command header */
  cal_pkt = (ftm_enh_internal_device_cal_req_payload_type*)(&(header->enh_internal_cal_param));

  /* Store the results revision mode for this sweep */
  internal_device_cal_data->internal_device_cal_result.result_rev = cal_pkt->packet_header.res_rev;	
  
  /* Based on the Flag, enable or disable the debug mode */
  internal_device_cal_data->internal_device_cal_config.dbg_mode_en = cal_pkt->packet_header.dbg_mode_en ;

  /* extract flag from header */
  internal_device_cal_data->internal_device_cal_config.flag = cal_pkt->packet_header.flag;
	
  /* Store packet instruction size */
  internal_device_cal_data->internal_device_cal_config.tot_instr_size = cal_pkt->packet_header.tot_instr_size;

  internal_device_cal_data->internal_device_cal_config.tech = cal_pkt->packet_header.tech;
	
  internal_device_cal_data->internal_device_cal_config.bw = cal_pkt->packet_header.bw;

  internal_device_cal_data->internal_device_cal_config.band = cal_pkt->packet_header.band;

  internal_device_cal_data->internal_device_cal_config.ref_chan = cal_pkt->packet_header.ref_chan;

  internal_device_cal_data->internal_device_cal_config.device_mask= cal_pkt->packet_header.device_mask;


  /* Store internal cal Mask */
  internal_device_cal_data->internal_device_cal_config.internal_cal_mask = cal_pkt->packet_header.internal_cal_type_mask;
  
  FTM_MSG_9(FTM_MED,"enh_internal_device_cal commonAPI, res_rev:%d, dbg_mode:%d,"
		  " flag:%d, tot_instr_size:%d,"
		  " tech:%d, band:%d,"
		  " ref_chan:%d, device_mask: %d, internal_cal_mask:%d,",	internal_device_cal_data->internal_device_cal_result.result_rev,internal_device_cal_data->internal_device_cal_config.dbg_mode_en,
		  internal_device_cal_data->internal_device_cal_config.flag,	internal_device_cal_data->internal_device_cal_config.tot_instr_size,
		  internal_device_cal_data->internal_device_cal_config.tech,internal_device_cal_data->internal_device_cal_config.band,
		  internal_device_cal_data->internal_device_cal_config.ref_chan, internal_device_cal_data->internal_device_cal_config.device_mask, internal_device_cal_data->internal_device_cal_config.internal_cal_mask
		);

  FTM_MSG_1(FTM_MED,"enh_internal_device_cal commonAPI, bw:%d",
             internal_device_cal_data->internal_device_cal_config.bw
	   );
  /* Push the Calibration Request Data to New Node */
  ret_val = ftm_enh_internal_device_calibration_push_instruction( 
                         cal_pkt->packet_header.tot_instr_size,
                         &(cal_pkt->instr_ptr) );


  internal_device_cal_data->internal_device_cal_status.error_log = ret_val; /* return the status */
  return ret_val;

} /* ftm_enh_internal_device_calibration_configure */





/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack internal_calibration payload
 
  @details
  This function is called  to unpack the payload associated
  with the given internal cal instruction. This function will store the RF 
  calibration instruction data at current internal cal dispatch_table
  data structure.
 
 
  @return error_code

  
*/
#ifndef T_WINNT
inline
#endif
ftm_enh_internal_device_cal_error_code_type
ftm_enh_internal_device_calibration_unpack_instruction
(
  void
)
{
 
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data;
  ftm_enh_internal_device_cal_error_code_type * ret_val;
  uint8 itr = 0 ;
  

  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
  ret_val = &(internal_device_cal_data->internal_device_cal_status.error_log); 

  
  /* Print debug message to track instruction address*/
  if ( internal_device_cal_data->internal_device_cal_config.dbg_mode_en == TRUE )
  {
     // print debug message
      
  } 

	

   
  internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr = (uint8 *) internal_device_cal_data->internal_device_cal_config.instruction;

  internal_device_cal_data->internal_device_cal_result.write_ptr = internal_device_cal_data->internal_device_cal_result.buff_ptr;
  ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr = internal_device_cal_data->internal_device_cal_result.write_ptr;

  ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit = ENH_INTERNAL_DEVICE_CAL_MAX;

  FTM_MSG_2(FTM_MED,"enh_internal_cal: internal cal type %d, bit_shifted: %d", internal_device_cal_data->internal_device_cal_config.internal_cal_mask, (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT));
  if (((internal_device_cal_data->internal_device_cal_config.internal_cal_mask)&(1 << ENH_INTERNAL_DEVICE_CAL_FBDROOP_BIT)))
  {
    FTM_MSG_1(FTM_MED,"enh_internal_cal: droop cal is called, itr%d", itr);
	ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit = ENH_INTERNAL_DEVICE_CAL_FBDROOP_BIT ;
	ftm_enh_internal_device_cal_dispatch_table[itr].process_fp = ftm_enh_internal_device_cal_fb_droop;
	ftm_enh_internal_device_cal_dispatch_table[itr].payload_ptr = internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr;
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr = internal_device_cal_data->internal_device_cal_result.write_ptr;
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_sz = sizeof(ftm_enh_internal_device_cal_fb_droop_result_type);
	//internal_device_cal_data->internal_device_cal_result.write_ptr +=	sizeof(ftm_enh_internal_device_cal_fb_droop_result_type);
	//internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr += sizeof( ftm_enh_internal_device_cal_instr_payload_fb_droop_type ); 
    itr++;
  }
  else if (((internal_device_cal_data->internal_device_cal_config.internal_cal_mask)&(1<< ENH_INTERNAL_DEVICE_CAL_GROUP_DELAY_BIT)))
  {
	FTM_MSG_1(FTM_MED,"enh_internal_cal: group_delay cal is called, itr:%d", itr);
	ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit = ENH_INTERNAL_DEVICE_CAL_GROUP_DELAY_BIT ;
	ftm_enh_internal_device_cal_dispatch_table[itr].process_fp = ftm_enh_internal_device_cal_group_delay;
	ftm_enh_internal_device_cal_dispatch_table[itr].payload_ptr = internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr;
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr = internal_device_cal_data->internal_device_cal_result.write_ptr;
	
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_sz = sizeof(ftm_enh_internal_device_cal_group_delay_result_type);
	internal_device_cal_data->internal_device_cal_result.write_ptr +=	sizeof(ftm_enh_internal_device_cal_group_delay_result_type);
	internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr += sizeof(ftm_enh_internal_device_cal_instr_payload_group_delay_type); 
    itr++;
  }
  else if (((internal_device_cal_data->internal_device_cal_config.internal_cal_mask)& (1<< ENH_INTERNAL_DEVICE_CAL_IP2_BIT)))
  {
	FTM_MSG_1(FTM_MED,"enh_internal_cal: ip2 cal is called, itr:%d", itr);
	ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit = ENH_INTERNAL_DEVICE_CAL_IP2_BIT ;
	ftm_enh_internal_device_cal_dispatch_table[itr].process_fp = ftm_enh_internal_device_cal_fb_ip2;
	ftm_enh_internal_device_cal_dispatch_table[itr].payload_ptr = internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr;
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr = internal_device_cal_data->internal_device_cal_result.write_ptr;
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_sz = sizeof(ftm_enh_internal_device_cal_ip2_result_type);
	
	internal_device_cal_data->internal_device_cal_result.write_ptr +=	sizeof(ftm_enh_internal_device_cal_ip2_result_type);
	internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr += sizeof(ftm_enh_internal_device_cal_instr_payload_ip2_type); 
     itr++;
  }

  else if ( ((internal_device_cal_data->internal_device_cal_config.internal_cal_mask)&(1<< ENH_INTERNAL_DEVICE_CAL_DC_BIT)))
  {
	FTM_MSG_1(FTM_MED,"enh_internal_cal: dc cal is called, itr:%d", itr);
	ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit = ENH_INTERNAL_DEVICE_CAL_DC_BIT ;
	ftm_enh_internal_device_cal_dispatch_table[itr].process_fp = ftm_enh_internal_device_cal_fb_dc;
	ftm_enh_internal_device_cal_dispatch_table[itr].payload_ptr = internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr;
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr = internal_device_cal_data->internal_device_cal_result.write_ptr;
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_sz = sizeof(ftm_enh_internal_device_cal_fb_dc_result_type);

	//internal_device_cal_data->internal_device_cal_result.write_ptr +=	sizeof(ftm_enh_internal_device_cal_dc_result_type);
	//internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr += sizeof(ftm_enh_internal_device_cal_instr_payload_dc_type); 
    itr++;
  }

  else if (((internal_device_cal_data->internal_device_cal_config.internal_cal_mask)&(1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT)))
  {
	FTM_MSG_1(FTM_MED,"enh_internal_cal: rsb cal is called, itr:%d", itr);
	ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit = ENH_INTERNAL_DEVICE_CAL_RSB_BIT ;
	ftm_enh_internal_device_cal_dispatch_table[itr].process_fp = ftm_enh_internal_device_cal_rsb;
	ftm_enh_internal_device_cal_dispatch_table[itr].payload_ptr = internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr;
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr = internal_device_cal_data->internal_device_cal_result.write_ptr;	
	ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_sz = sizeof(ftm_enh_internal_device_cal_instr_payload_rsb_type);
	//internal_device_cal_data->internal_device_cal_result.write_ptr +=	sizeof(ftm_internal_device_cal_rsb_result_type);
	//internal_device_cal_data->internal_device_cal_config.read_node_payload_ptr += sizeof(ftm_internal_device_cal_instr_payload_rsb_type); 
    itr++;
   }

	*ret_val =  FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS; // need add error check later
    ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit = ENH_INTERNAL_DEVICE_CAL_MAX;

	if(itr ==0)
	{
       internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_NOT_PROCESSED;
	   *ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_INVALID_CAL_TYPE;
	}
	else if(itr > 1)
	{
       //crash the phone  or return error?
       *ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_OVER_ONE_TYPE_PER_REQ; // we don't support multiple internal cal_type per internal cal request for now. We may remove it in the future to support multiple internal cal type per request
	}
  return *ret_val; 	
} 



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal Calibration Process Routine

  @details
  This function will perform detailed Internal Calibration. 

  This function will be performed after internal calibration instruction is extracted, and the configuration is saved in ftm_internal_cal_data

  This function will call internal_cal process_fp based on enabled internal cal bit.

*/
ftm_enh_internal_device_cal_error_code_type
ftm_enh_internal_device_calibration_process
(
  void
)
{
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data; /* Internal Calibration Sweep Data */
	ftm_enh_internal_device_cal_error_code_type ret_val;
  
	boolean ret_val2 = FALSE;
	uint16 res_len = 0;
  /* Get Calibration Data */
  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

  /* Reset the segment Error to 0 */
  internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;


  /* ------------------------------------------------------------------------ */

  /* Unpack instruction */
  ret_val = ftm_enh_internal_device_calibration_unpack_instruction();

 
  internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_NOT_PROCESSED;

  if ( ret_val >= 0 )
  {
    uint8 itr =0;
	internal_device_cal_data->internal_device_cal_result.res_mask = 0;
	internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_START_CAL; 
	internal_device_cal_data->internal_device_cal_result.total_result_sz = 0;
	
	while ( ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit != ENH_INTERNAL_DEVICE_CAL_MAX)
	{
       internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_PROCESSING; 
	   FTM_MSG_1(FTM_MED,"enh_internal_cal: process_fp is called, itr%d", itr);
	   ret_val2 = ftm_enh_internal_device_cal_dispatch_table[itr].process_fp(
	   	              															ftm_enh_internal_device_cal_dispatch_table[itr].payload_ptr,&res_len,
														   	              		(void *) ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr
														   	              	);
       
	   if(ret_val2 == TRUE) // if cal is good, save the results to response payload structure
	                        // rsp payload structure: Internal_cal_type (4 bytes), cal_res_length(2 bytes), cal_results (number of cal_res_length)
       {
          
		  ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len = res_len;
		  internal_device_cal_data->internal_device_cal_result.total_result_sz +=(res_len
											                                      +sizeof(uint32) //length of cal type
			                    								                  +sizeof(uint16)); // length of cal_result	   
         if ( (ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit == ENH_INTERNAL_DEVICE_CAL_RSB_BIT) &&
		        ((internal_device_cal_data->internal_device_cal_config.flag ==2)||
		        (internal_device_cal_data->internal_device_cal_config.flag ==3 	)))
         {
			 internal_device_cal_data->internal_device_cal_result.total_result_sz += internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len;
         }
	   	}
	   else
	   {
         ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len = 0;
		 ret_val = internal_device_cal_data->internal_device_cal_status.error_log;
	   }
	   
	   internal_device_cal_data->internal_device_cal_result.res_mask |= ((ret_val2 ==TRUE? 1:0) << ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit);
       internal_device_cal_data->internal_device_cal_status.error_mask  |= ((ret_val2 ==FALSE? 1:0) << ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit);
	   itr++;
	}
    internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_COMPLETE; 
	
  } /* if ( instr_flag == TRUE ) */
  else /* error */
  {

  }
 
/*
  // add debug message here
  if ( ( internal_device_cal_data->internal_device_cal_config.dbg_mode_en == TRUE ) )
  {
 
  } 
*/
   return ret_val; 
	   
} 


/*----------------------------------------------------------------------------*/
/*!
@name: ftm_internal_device_calibration_send_results

  @brief
  Send Internal_Calibration Sweep Results to PC

  @details
  
  @param ftm_req_data
  Pointer to FTM Internal Cal Request Pointer This will be the pointer
  of the FTM packet. This does not include the Diag header. The parameter 
  structure can be seen in the following illustration.

  @return
  Response packet to PC
*/
ftm_rsp_pkt_type
ftm_enh_internal_device_calibration_send_results
(
  void *ftm_req_data
)
{

  ftm_enh_internal_device_cal_data_type *internal_device_cal_data; /* Internal Cal Data */
  int16 result_len = 0; /* Length of internal calibration results to be sent */

  int16 diag_limit = 4096;
  /* Get Diag Limitation on Max size of Response packet that can be sent */
	ftm_enh_internal_device_cal_error_code_type ret_val;
    /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */

  ftm_enh_internal_device_cal_rsp_pkt_type *rsp_pkt; /*Packet Payload for each cmd*/

  uint8 *rspPayloadPtr = NULL;
  uint8 itr =0;
  uint16 rspPayloadIndex = 0;
  uint16 enh_dev_cal_rsp_len = 0; 
  uint16 trunk_len = 0;
  
  /* Get Pointer to Internal Cal Data */
  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
  ret_val = internal_device_cal_data->internal_device_cal_status.error_log; 
  

  
  if(ret_val<0)
  {

    ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
                                 FPOS( ftm_enh_internal_device_cal_rsp_pkt_type, cal_result ) ); 

    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    rsp_pkt = (ftm_enh_internal_device_cal_rsp_pkt_type*)ftm_rsp_data.pkt_payload;
  
    if (rsp_pkt == NULL  )
    {
      ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
      FTM_MSG( FTM_ERROR, "enh_internal_device_cal ftm_calibration : Failed " );
	  ftm_enh_internal_device_calibration_clear_result_buffer();
       return ftm_rsp_data;
    } /* if (rsp_pkt == NULL  ) */
    else
    {
	   rsp_pkt->tech = internal_device_cal_data->internal_device_cal_config.tech;
	   rsp_pkt->band = internal_device_cal_data->internal_device_cal_config.band;
	   rsp_pkt->bw = internal_device_cal_data->internal_device_cal_config.bw;
       rsp_pkt->cal_status_error_code = ret_val;
	   rsp_pkt->cal_type_error_mask = internal_device_cal_data->internal_device_cal_status.error_mask;
	   rsp_pkt->cal_result_len = 0;
	   FTM_MSG_4(FTM_MED,"Enh_Internal_cal return Error,  tech:%d, band:%d, error_code: %d, error_mask:%d ",
				rsp_pkt->tech,rsp_pkt->band,rsp_pkt->cal_status_error_code, rsp_pkt->cal_result_len);
    }
	ftm_enh_internal_device_calibration_clear_result_buffer();
	return ftm_rsp_data;
  }

  /* Other defines */
 
  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
 
 
  /* Allow some margin to Diag Limitation. Thus backoff the diag_limit with
   some margin. This margin is used so that cal results data take up
   memeory less than Diag Limit leaving room for response packet overheads. */
  diag_limit = diag_limit - FTM_ENH_INTERNAL_DEVICE_CAL_DIAG_LIMIT_BACKOFF;


  if (diag_limit < internal_device_cal_data->internal_device_cal_result.total_result_sz)
  {
     //add some error code here
  }

  /* The length of result to be sent should be the lower one between the Diag 
     Limit and the total unread result buffer. If the length of result is
     less than zero, then the final result len ceil'ed up to zero. */
  result_len = rf_max_s16(rf_min_s16( diag_limit, internal_device_cal_data->internal_device_cal_result.total_result_sz ), 0);  
	
  trunk_len = internal_device_cal_data->internal_device_cal_result.total_result_sz -result_len;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
	FPOS( ftm_enh_internal_device_cal_rsp_pkt_type, cal_result ) + result_len); 
 
  
  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_enh_internal_device_cal_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  if(rsp_pkt == NULL)
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
    FTM_MSG( FTM_ERROR, "ftm_enh_internal_device_calibration return results : Failed " );
  }  

  else
  {
    rsp_pkt->tech = internal_device_cal_data->internal_device_cal_config.tech;
    rsp_pkt->band = internal_device_cal_data->internal_device_cal_config.band;
    rsp_pkt->bw = internal_device_cal_data->internal_device_cal_config.bw;
    rsp_pkt->cal_status_error_code = internal_device_cal_data->internal_device_cal_status.error_log;
    rsp_pkt->cal_type_error_mask = internal_device_cal_data->internal_device_cal_status.error_mask; 
    rsp_pkt->cal_result_len = result_len;

	FTM_MSG_3(FTM_MED, "error_code :%d, error_mask :%d, result_len :%d", rsp_pkt->cal_status_error_code, rsp_pkt->cal_type_error_mask, rsp_pkt->cal_result_len);
    /* Copy the Chunk of result from result buffer to the response packet payload */
    rspPayloadPtr = &(rsp_pkt->cal_result);
    itr = 0;
    while ( ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit != ENH_INTERNAL_DEVICE_CAL_MAX)
    {
//	  if( ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len != 0)
	  enh_dev_cal_rsp_len = ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len;
      if ( (ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit == ENH_INTERNAL_DEVICE_CAL_RSB_BIT) &&
		        ((internal_device_cal_data->internal_device_cal_config.flag ==2)||
		        (internal_device_cal_data->internal_device_cal_config.flag ==3 	)))
      {
		enh_dev_cal_rsp_len += internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len;
      }
      {
        memset(rspPayloadPtr, 0, sizeof(uint32));
        memscpy( rspPayloadPtr,  sizeof(ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit),
		 &( ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit ), sizeof(ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit));			
        rspPayloadPtr += sizeof(uint32);
			
        memscpy(rspPayloadPtr, sizeof(uint16), &enh_dev_cal_rsp_len ,sizeof(uint16));
			
	    rspPayloadPtr +=sizeof(uint16);
			
	    memscpy(rspPayloadPtr, 
		        ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len, 
		        ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr,
		        ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len);
			
	    rspPayloadPtr += ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len;

	    FTM_MSG_5(FTM_MED,"Enh_Internal_cal Rsp Payload, CalType:%d, Internal cal rsp Payload Len: %d, total payLoad len: %d, tech:%d, band:%d",
  		  ftm_enh_internal_device_cal_dispatch_table[itr].ic_bit, ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len, 
		  result_len, rsp_pkt->tech,rsp_pkt->band);

	   	if(internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len >0 )
		{
		   if(trunk_len>0)
		   {
			   internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len -=trunk_len;
			   FTM_MSG_1(FTM_MED,"rsb cal, saving efs validation payload trunked because of max limit of ftm rsp size, trunk_len %d",trunk_len);
		   }
				
		   memscpy(rspPayloadPtr, internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len,
		   	internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr,internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len);
		   rspPayloadPtr += internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len;
	   	}

       if(internal_device_cal_data->internal_device_cal_config.dbg_mode_en>=2)
       {
         for (rspPayloadIndex = 0; rspPayloadIndex< ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.res_len; rspPayloadIndex++)
         {
	       FTM_MSG_1(FTM_MED, "payLoad: 0x%x ", ftm_enh_internal_device_cal_dispatch_table[itr].res_buff_info.buff_ptr[rspPayloadIndex]);
          }
       	}
      }
      itr++;
    }
  }	   
  
  ftm_enh_internal_device_calibration_clear_result_buffer();
  return ftm_rsp_data;

} /* ftm_enh_internal_device_calibration_send_results */



/*----------------------------------------------------------------------------*/
/*
  @Func Name: ftm_enh_device_internal_calibration
  @brief
  Decoding enh_internal_calibration ftm request packet based on internal_cal request diag package, process
  internal cal based on decoded message and send response back to PC

  @details
  This function will decode and configure the enhanced internal Calibration data structure based on the ftm_req_data received 
  from PC side and Process the internal cal based on decoded message, and Send results back to PC.  

  
  This function should not be called again before the response packet is reveived by PC. 
  If the function is called in between, then the integrity of configuration data cannot be
  guaranteed, and resulting is possible calibration failure.

  Following sub-fuctions are called inside to perform the above functionalities
     1. Internal_calibration_configure():  decode and configure internal cal data structure based on the ftm_req_data received 
        from PC (the ftm diag package)
     2. Ftm_internal_calibration_process(): Process the internal cal based on decoded message
     3. Ftm_internal_calibration_send_results():Send results back to PC

  @param ftm_req_data
  This will be the pointer of the FTM packet. This does not include the Diag header. 

  @return ftm_rsp_pkt_type
    Reponse Packet for ftm_enh_internal_device_calibration. 

*/
ftm_rsp_pkt_type
ftm_enh_internal_device_calibration
(
  void *ftm_req_data
)
{
  /* Request Packet */
  ftm_enh_internal_device_cal_req_pkt_type* header =
    (ftm_enh_internal_device_cal_req_pkt_type*)ftm_req_data;
  /* Other defines */
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS; /* Return value */
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data = NULL; /* Internal Cal Data */

   
   /* Get Pointer to Internal Cal Data */
  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

  /*  NULL pointer check for internal_cal data container */
  if (internal_device_cal_data == NULL )
  {
     ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_DATA_POINTER_INVALID ;  

  } /* if (cal_data == NULL ) */

  /* Configure the Internal Calibration Cal_data based on ftm diag package using the header pointer */
  if(ret_val == FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
  {
    ftm_enh_internal_device_calibration_data_init();
	ret_val = ftm_enh_internal_device_calibration_configure( header );
  }
  if(ret_val == FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
  {
     internal_device_cal_data->internal_device_cal_result.res_max_len = 0 ; // this is for rsb calver extra payload
	 if(internal_device_cal_data->internal_device_cal_config.internal_cal_mask & (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT))
     {
       if((internal_device_cal_data->internal_device_cal_config.flag ==2) || (internal_device_cal_data->internal_device_cal_config.flag ==3 ) )
       {
		   internal_device_cal_data->internal_device_cal_result.res_max_len = MAX_ENH_INTERNAL_DEVICE_CAL_RSB_VER_LENGTH;
		   internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr = (uint8 *) modem_mem_alloc( 
					   internal_device_cal_data->internal_device_cal_result.res_max_len, MODEM_MEM_CLIENT_RFA );

		   if( internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr == NULL)
		   {
			   internal_device_cal_data->internal_device_cal_result.res_max_len = 0;
			   internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_MEM_ALLOC_FAILURE;
			   internal_device_cal_data->internal_device_cal_status.error_mask = internal_device_cal_data->internal_device_cal_config.internal_cal_mask;
			   ret_val = internal_device_cal_data->internal_device_cal_status.error_log;
		   }
	   }
     }
  }
  /* Process Internal Cal */

   if(ret_val == FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
   {
   	 ret_val = ftm_enh_internal_device_calibration_process();
   }
   internal_device_cal_data->internal_device_cal_status.error_log = ret_val;

  /* process Cal Results send back */

  return ftm_enh_internal_device_calibration_send_results(ftm_req_data);
  
}

uint16 ftm_convert_tech_bandnum_to_fbrxnv_band_type
(
   ftm_rf_technology_type fbrx_tech, 
   uint16 fbrx_band)
{
  uint16 nv_band = 0;
  switch (fbrx_tech)
  {
  case FTM_RF_TECH_LTE:
    switch (fbrx_band)
    {
    case 1:
      nv_band = (uint16)RFCOM_BAND_LTE_B1;
      break;
    case 2:
      nv_band = (uint16)RFCOM_BAND_LTE_B2;
      break;
    case 3:
      nv_band = (uint16)RFCOM_BAND_LTE_B3;
      break;
    case 4:
      nv_band = (uint16)RFCOM_BAND_LTE_B4;
      break;
    case 5:
      nv_band = (uint16)RFCOM_BAND_LTE_B5;
      break;
    case 6:
      nv_band = (uint16)RFCOM_BAND_LTE_B6;
      break;
    case 7:
      nv_band = (uint16)RFCOM_BAND_LTE_B7;
      break;
    case 8:
      nv_band = (uint16)RFCOM_BAND_LTE_B8;
      break;
    case 9:
      nv_band = (uint16)RFCOM_BAND_LTE_B9;
      break;
    case 10:
      nv_band = (uint16)RFCOM_BAND_LTE_B10;
      break;
    case 11:
      nv_band = (uint16)RFCOM_BAND_LTE_B11;
      break;
    case 12:
      nv_band = (uint16)RFCOM_BAND_LTE_B12;
      break;
    case 13:
      nv_band = (uint16)RFCOM_BAND_LTE_B13;
      break;
    case 14:
      nv_band = (uint16)RFCOM_BAND_LTE_B14;
      break;
    case 15:
      nv_band = (uint16)RFCOM_BAND_LTE_B15;
      break;
    case 16:
      nv_band = (uint16)RFCOM_BAND_LTE_B16;
      break;
    case 17:
      nv_band = (uint16)RFCOM_BAND_LTE_B17;
      break;
    case 18:
      nv_band = (uint16)RFCOM_BAND_LTE_B18;
      break;
    case 19:
      nv_band = (uint16)RFCOM_BAND_LTE_B19;
      break;
    case 20:
      nv_band = (uint16)RFCOM_BAND_LTE_B20;
      break;
    case 21:
      nv_band = (uint16)RFCOM_BAND_LTE_B21;
      break;
    case 22:
      nv_band = (uint16)RFCOM_BAND_LTE_B22;
      break;
    case 23:
      nv_band = (uint16)RFCOM_BAND_LTE_B23;
      break;
    case 24:
      nv_band = (uint16)RFCOM_BAND_LTE_B24;
      break;
    case 25:
      nv_band = (uint16)RFCOM_BAND_LTE_B25;
      break;
    case 26:
      nv_band = (uint16)RFCOM_BAND_LTE_B26;
      break;
    case 27:
      nv_band = (uint16)RFCOM_BAND_LTE_B27;
      break;
    case 28:
      nv_band = (uint16)RFCOM_BAND_LTE_B28;
      break;
    case 29:
      nv_band = (uint16)RFCOM_BAND_LTE_B29;
      break;
    case 30:
      nv_band = (uint16)RFCOM_BAND_LTE_B30;
      break;
    case 31:
      nv_band = (uint16)RFCOM_BAND_LTE_B31;
      break;
    case 32:
      nv_band = (uint16)RFCOM_BAND_LTE_B32;
      break;
    case 33:
      nv_band = (uint16)RFCOM_BAND_LTE_B33;
      break;
    case 34:
      nv_band = (uint16)RFCOM_BAND_LTE_B34;
      break;
    case 35:
      nv_band = (uint16)RFCOM_BAND_LTE_B35;
      break;
    case 36:
      nv_band = (uint16)RFCOM_BAND_LTE_B36;
      break;
    case 37:
      nv_band = (uint16)RFCOM_BAND_LTE_B37;
      break;
    case 38:
      nv_band = (uint16)RFCOM_BAND_LTE_B38;
      break;
    case 39:
      nv_band = (uint16)RFCOM_BAND_LTE_B39;
      break;
    case 40:
      nv_band = (uint16)RFCOM_BAND_LTE_B40;
      break;
    case 41:
      nv_band = (uint16)RFCOM_BAND_LTE_B41;
      break;
    case 42:
      nv_band = (uint16)RFCOM_BAND_LTE_B42;
      break;
    case 43:
      nv_band = (uint16)RFCOM_BAND_LTE_B43;
      break;
    case 44:
      nv_band = (uint16)RFCOM_BAND_LTE_B44;
      break;
     //Split Bands
    case 45:
       nv_band = (uint16)RFCOM_BAND_LTE_B28_B;
       break;
    case 46:
       nv_band = (uint16)RFCOM_BAND_LTE_B40_B;
       break;
    case 47:
       nv_band = (uint16)RFCOM_BAND_LTE_B41_B;
       break;
    case 48:
       nv_band = (uint16)RFCOM_BAND_LTE_B41_C;
       break;
    case 49:
       nv_band = (uint16)RFCOM_BAND_LTE_B39_B;
       break;
    default:
      FTM_MSG(FTM_ERROR, "Unknown band: Return 0!");
    }
    break;
  case FTM_RF_TECH_WCDMA:
    switch (fbrx_band)
    {
    case WCDMA_FTM_BAND_1:
      nv_band = (uint16)RFCOM_BAND_IMT;
      break;

    case WCDMA_FTM_BAND_2:
      nv_band = (uint16)RFCOM_BAND_1900;
      break;

    case WCDMA_FTM_BAND_3:
      nv_band = (uint16)RFCOM_BAND_BC3;
      break;

    case WCDMA_FTM_BAND_4:
      nv_band = (uint16)RFCOM_BAND_BC4;
      break;

    case WCDMA_FTM_BAND_5:
      nv_band = (uint16)RFCOM_BAND_800;
      break;

    case WCDMA_FTM_BAND_8:
      nv_band = (uint16)RFCOM_BAND_BC8;
      break;

    case WCDMA_FTM_BAND_9:
      nv_band = (uint16)RFCOM_BAND_BC9;
      break;

    case WCDMA_FTM_BAND_11:
      nv_band = (uint16)RFCOM_BAND_BC11;
      break;

    case WCDMA_FTM_BAND_19:
      nv_band = (uint16)RFCOM_BAND_BC19;
      break;

    default:
      FTM_MSG(FTM_ERROR, "Unknown WCDMA band: Return 0!");
    }  
   break;
  case FTM_RF_TECH_CDMA:
    switch (fbrx_band)
    {
    case 0:
      nv_band = (uint16)RFM_CDMA_BC0;
      break;
    case 1:
      nv_band = (uint16)RFM_CDMA_BC1;
      break;
    case 2:
      nv_band = (uint16)RFM_CDMA_BC2;
      break;
    case 3:
      nv_band = (uint16)RFM_CDMA_BC3;
      break;
    case 4:
      nv_band = (uint16)RFM_CDMA_BC4;
      break;
    case 5:
      nv_band = (uint16)RFM_CDMA_BC5;
      break;
    case 6:
      nv_band = (uint16)RFM_CDMA_BC6;
      break;
    case 7:
      nv_band = (uint16)RFM_CDMA_BC7;
      break;
    case 8:
      nv_band = (uint16)RFM_CDMA_BC8;
      break;
    case 9:
      nv_band = (uint16)RFM_CDMA_BC9;
      break;
    case 10:
      nv_band = (uint16)RFM_CDMA_BC10;
      break;
    case 11:
      nv_band = (uint16)RFM_CDMA_BC11;
      break;
    case 12:
      nv_band = (uint16)RFM_CDMA_BC12;
      break;
    case 13:
      nv_band = (uint16)RFM_CDMA_BC13;
      break;
    case 14:
      nv_band = (uint16)RFM_CDMA_BC14;
      break;
    case 15:
      nv_band = (uint16)RFM_CDMA_BC15;
      break;
    case 16:
      nv_band = (uint16)RFM_CDMA_BC16;
      break;
    case 17:
      nv_band = (uint16)RFM_CDMA_BC17;
      break;
    case 18:
      nv_band = (uint16)RFM_CDMA_BC18;
      break;
    case 19:
      nv_band = (uint16)RFM_CDMA_BC19;
      break;
    case 20:
      nv_band = (uint16)RFM_CDMA_BC20;
      break;
    default:
      FTM_MSG(FTM_ERROR, "Unknown CDMA band: Return 0!");
    }
    break;
  case FTM_RF_TECH_TDSCDMA:
    switch (fbrx_band)
    {
    case TDSCDMA_FTM_BAND_B34:
      nv_band = (uint16)RFCOM_BAND_TDSCDMA_B34;
      break;

    case TDSCDMA_FTM_BAND_B39:
      nv_band = (uint16)RFCOM_BAND_TDSCDMA_B39;
      break;

    case TDSCDMA_FTM_BAND_B40:
      nv_band = (uint16)RFCOM_BAND_TDSCDMA_B40;
      break;

    default:
      FTM_MSG(FTM_ERROR, "Unknown CDMA band: Return 0!");
    }  
    break;
  default:
    FTM_MSG(FTM_ERROR, "Unsupported Tech Type: Return 0!");
  }
  return nv_band;
}




#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

/*! @} */


