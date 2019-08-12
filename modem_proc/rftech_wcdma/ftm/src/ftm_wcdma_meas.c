/*!
  @file ftm_wcdma_meas.c

  @brief
  This file contains all declarations and definitions necessary to use
  the wcdma FTM Measurement Main Control.

  @details

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Inc and all rights therein are expressly reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/src/ftm_wcdma_meas.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/14   rmb     Fix to use the common meas API to build scripts for W2W.
09/19/14   rmb     DBDC support for W2X FTM IRAT cases.
08/11/14   rmb     Bypass RxAGC on flag for FTM X2W Rxagc read after tune to W.
06/06/14   ac      nbr support
05/14/14   rmb     Fix KW Error.
05/13/14   rmb     Enable W2X and L2W FTM IRAT.
04/14/14   rmb     Fix Compilation Error on Bolt 2.0 
04/14/14   rmb     Add changes for L2W FTM IRAT.
04/04/14   rmb     Assign valid RxLM index during Meas exit for Abort.
03/25/14   rmb     Enable G2W FTM IRAT.
03/25/14   rmb     FTM IRAT 2nd phase checkin.
02/27/14   aa      DimePM linker error fixex
02/27/14   rmb     First phase FTM IRAT checkin for G2W. 
02/13/14   dw      [1]Change AGC command function to support multiple device
                   [2]Code cleanup
01/03/14   rmb     Bolt IRAT changes for WCDMA.
06/05/13   aa      Turn of TX when exiting W
05/09/13   aa      updated W2W FTM IRAT for new FTM framework
04/25/13   pl      Populate exit_params fully and correctly
02/27/13   sar     Featurized header file to fix non-lte build.
02/19/13   pl      Remove unused fields
02/06/13   rmb     Fix Compilation Errors.
01/31/13   aa      added a simple state machine for W enter/exit procedure
01/11/13   aa      Initial Check-in
===========================================================================*/

#include "rfmeas_types.h"
#include "rfmeas_mc.h"
#include "ftm_meas.h"

#include "rfwcdma_mc.h"
#include "ftm_wcdma_ctl.h"
#include "ftm_wcdma_meas.h"
#include "rfmeas_wcdma.h"
#include "rfwcdma_mc_meas.h"
#include "rfwcdma_mdsp.h"
#include "rfmeas_ftm_mdsp.h"
#ifdef FEATURE_LTE
#include "intf_sys.h"
#endif
#include "rfwcdma_msg.h"
#include "ftm.h"
#include "rfcom.h"
#include "rfwcdma_core_rxctl.h"
#include "ftm_wcdma_ctl.h"
#include "fws.h"
#include "ftm_common_control.h"
#include "rflm_api_wcdma.h"
#include "rflm_time.h"
#include "rfwcdma_mdsp_sync.h"
#include "rfwcdma_core.h"
#include "rfwcdma_core_util.h"

/* Inverse of mid band frequencies (2^52 / freq in Hz) */
#define INV_MID_FREQ_WCDMA_2100_BAND    2104486
#define INV_MID_FREQ_WCDMA_1900_BAND    2297755
#define INV_MID_FREQ_WCDMA_BC3_BAND     2443624
#define INV_MID_FREQ_WCDMA_BC4_BAND     2111392
#define INV_MID_FREQ_WCDMA_800_BAND     5106122
#define INV_MID_FREQ_WCDMA_BC8_BAND     4775821
#define INV_MID_FREQ_WCDMA_BC9_BAND     2418689

#define RF_WCDMA_X2W_AGC_POLLING_INTERVAL 5 // Every 5 micro sec
#define RF_WCDMA_AGC_SETTLING_TIME 667 // 10 bpgs

/* This RxLM handle is updated during L2W measurements and passed to L1 for L2W Tune away */
extern uint32 l2w_ftm_rxlm_handle;

/* RxLM handle for W2W neighbour in FTM */
extern lm_handle_type ftm_wcdma_neighbour_rxlm_buffer;

extern ftm_lm_buf_type ftm_wcdma_rxlm_buffer[2];
extern void rfmeas_mc_wtow_build_script( rfm_meas_setup_param_type *w2w_meas_param );
extern void rfmeas_mc_wtow_build_script_ftm( rfm_meas_w2w_setup_and_build_scripts_param_type *w2w_meas_param );;
extern void rfwcdma_meas_pre_config_transceiver_v2(rfm_device_enum_type src_device,
                                                   rfm_device_enum_type src_div_device,
                                                   rfm_device_enum_type target_device, 
                                                   rfm_device_enum_type target_div_device, 
                                                   rfm_mode_enum_type src_tech, 
                                                   uint16 src_band, 
                                                   rfcom_wcdma_band_type wcdma_band);


/*============================================================================
             LOCAL Function Prototypes                                                      
=============================================================================*/
boolean ftm_wcdma_meas_get_src_rxlm_buffer(rfm_meas_generic_params_type *test_param_ptr);
void* ftm_wcdma_meas_get_enter_param(rfm_meas_generic_params_type *test_param_ptr);
void* ftm_wcdma_meas_get_setup_param(rfm_meas_generic_params_type *test_param_ptr);
void* ftm_wcdma_meas_get_exit_param(rfm_meas_generic_params_type *test_param_ptr);
boolean ftm_wcdma_meas_enter_meas(rfm_meas_enter_param_type *meas_enter_param);
boolean ftm_wcdma_meas_build_script(rfm_meas_setup_param_type *meas_setup_param);
boolean ftm_wcdma_meas_fw_start_rxagc(rfm_meas_generic_params_type *test_param_ptr, boolean start_src_agc);
boolean ftm_wcdma_meas_exit_meas(rfm_meas_exit_param_type *meas_exit_param);
boolean ftm_wcdma_meas_exit_wcdma(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param);
boolean ftm_wcdma_meas_enter_wcdma(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param);
boolean ftm_wcdma_meas_stop( rfm_mode_enum_type src_tech );
boolean ftm_wcdma_meas_start( rfm_mode_enum_type src_tech, lm_handle_type neigh_rxlm_idx, uint16 neigh_chan );
boolean ftm_wcdma_meas_suspend_resume( rfm_meas_generic_params_type *test_param_ptr );
uint32 ftm_wcdma_meas_get_inverse_dlo_freq(uint16 chan);
boolean ftm_wcdma_meas_wtow_tune_wcdma( rfm_meas_generic_params_type *test_param_ptr, boolean is_wcdma_neighbour );

/*============================================================================
             LOCAL/EXTERNAL VARIABLES                                                      
=============================================================================*/
static ftm_meas_func_tbl_type ftm_wcdma_meas_apis = \
{
   ftm_wcdma_meas_get_src_rxlm_buffer,
   ftm_wcdma_meas_get_enter_param,
   ftm_wcdma_meas_get_setup_param,
   ftm_wcdma_meas_get_exit_param,
   ftm_wcdma_meas_enter_meas,
   ftm_wcdma_meas_build_script,
   ftm_wcdma_meas_exit_wcdma,
   ftm_wcdma_meas_enter_wcdma,
   ftm_wcdma_meas_exit_meas
};

static rfm_meas_wcdma_enter_param_type ftm_wcdma_meas_enter_param;
static rfm_meas_wcdma_exit_param_type ftm_wcdma_meas_exit_param;
static rfm_meas_wcdma_setup_and_build_scripts_param_type ftm_wcdma_meas_setup_param;
static rfm_meas_params_channel_type ftm_wcdma_meas_g2w_enter_and_setup_param;
static rfm_meas_w2w_setup_and_build_scripts_param_type ftm_wcdma_meas_w2w_setup_param;

static boolean start_called = FALSE;
//static boolean stop_called = FALSE;

void ftm_wcdma_meas_register(void)
{
   ftm_meas_register(RFCOM_WCDMA_MODE, &ftm_wcdma_meas_apis);
   memset(&ftm_wcdma_meas_enter_param,0,sizeof(rfm_meas_wcdma_enter_param_type));
   memset(&ftm_wcdma_meas_exit_param,0,sizeof(rfm_meas_wcdma_exit_param_type));
   memset(&ftm_wcdma_meas_setup_param,0,sizeof(rfm_meas_wcdma_setup_and_build_scripts_param_type));
   memset(&ftm_wcdma_meas_w2w_setup_param,0,sizeof(rfm_meas_w2w_setup_and_build_scripts_param_type));
}
void ftm_wcdma_meas_deregister(void)
{
   ftm_meas_deregister(RFCOM_WCDMA_MODE);
}

boolean ftm_wcdma_meas_get_src_rxlm_buffer(rfm_meas_generic_params_type *test_param_ptr)
{
  //uint32 temp_carrier_idx = 2;
  if(test_param_ptr == NULL)
  {
    return FALSE;
  }
  
  if(test_param_ptr->src_tech != RFCOM_WCDMA_MODE)
  {
    return FALSE;
  }

  ftm_wcdma_allocate_rxlm(FTM_RECEIVE_CHAIN_0);

  if(ftm_wcdma_rxlm_buffer[RF_PATH_0].is_buf_allocated)
  {
      test_param_ptr->src_rxlm_buf_idx_ant0 = ftm_wcdma_rxlm_buffer[RF_PATH_0].buf_idx;
  }

  if(test_param_ptr->src_div_en == TRUE)
  {
     ftm_wcdma_allocate_rxlm(FTM_RECEIVE_CHAIN_1);

     if(ftm_wcdma_rxlm_buffer[RF_PATH_1].is_buf_allocated)
     {
       test_param_ptr->src_rxlm_buf_idx_ant1 = ftm_wcdma_rxlm_buffer[RF_PATH_1].buf_idx;
     }
  }

  else
  {
    test_param_ptr->src_rxlm_buf_idx_ant1 = test_param_ptr->src_rxlm_buf_idx_ant0; 
    // If diversity is not enabled, assign RFLM index of primary to diversity as well so that it gets passed toWFW during W2X suspend/resume
  }
  //rfwcdma_ftm_mc_sm_set_carrier_idx(temp_carrier_idx);
  return TRUE;
}

void* ftm_wcdma_meas_get_enter_param(rfm_meas_generic_params_type *test_param_ptr)
{
  void *enter_param_ptr = NULL;

  /* Parameter Check */
  if(test_param_ptr == NULL)
  {
    return enter_param_ptr;
  }

  /* If W2W then return with enter param NULL as we don't need Enter for W2W */
  if( (test_param_ptr->src_tech == RFCOM_WCDMA_MODE) && (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE) )
  {
    /* Update W2W Neighbour parameters */
    ftm_wcdma_meas_enter_param.device = RFM_DEVICE_0;
    ftm_wcdma_meas_enter_param.carrier_type = RFCOM_SINGLE_CARRIER ;
    ftm_wcdma_meas_enter_param.channel[0] = test_param_ptr->tgt_chan;
    ftm_wcdma_meas_enter_param.rxlm_buf_index = test_param_ptr->tgt_rxlm_buf_idx_ant0;
    enter_param_ptr = (void*)&ftm_wcdma_meas_enter_param;
  }

  /* G2W, use a different one */
  else if((test_param_ptr->src_tech == RFCOM_GSM_MODE) && 
          (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE))
  {
    ftm_wcdma_meas_g2w_enter_and_setup_param.neighbor_chan = test_param_ptr->tgt_chan;
    ftm_wcdma_meas_g2w_enter_and_setup_param.neighbor_rxlm_buf_idx = test_param_ptr->tgt_rxlm_buf_idx_ant0;

    enter_param_ptr = (void*)&ftm_wcdma_meas_g2w_enter_and_setup_param;
  }

  else if (test_param_ptr->src_tech == RFCOM_WCDMA_MODE)
  {
    ftm_wcdma_meas_enter_param.device = RFM_DEVICE_0;
    ftm_wcdma_meas_enter_param.carrier_type = RFCOM_SINGLE_CARRIER ;
    ftm_wcdma_meas_enter_param.channel[0] = test_param_ptr->src_chan;
    ftm_wcdma_meas_enter_param.rxlm_buf_index = test_param_ptr->src_rxlm_buf_idx_ant0 ;
    enter_param_ptr = (void*)&ftm_wcdma_meas_enter_param;
  }

  else if (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE)
  {
    ftm_wcdma_meas_enter_param.device = RFM_DEVICE_0;
    ftm_wcdma_meas_enter_param.carrier_type = RFCOM_SINGLE_CARRIER ;
    ftm_wcdma_meas_enter_param.channel[0] = test_param_ptr->tgt_chan;
    ftm_wcdma_meas_enter_param.rxlm_buf_index = test_param_ptr->tgt_rxlm_buf_idx_ant0 ;
    enter_param_ptr = (void*)&ftm_wcdma_meas_enter_param;
  }
  return (enter_param_ptr);
}

void* ftm_wcdma_meas_get_setup_param(rfm_meas_generic_params_type *test_param_ptr)
{
  void *param_ptr = NULL;
  
  /* Parameter check */
  if(test_param_ptr == NULL)
  {
    return param_ptr;
  }

  /* If W2W, use a different structure, otherwise use regular */
  if( (test_param_ptr->src_tech == RFCOM_WCDMA_MODE) && 
      (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE))
  {
    ftm_wcdma_meas_w2w_setup_param.carrier_type = RFCOM_SINGLE_CARRIER ; // Update rfm_meas_generic_params_type
    ftm_wcdma_meas_w2w_setup_param.home_arfcn[0] = test_param_ptr->src_chan;
    ftm_wcdma_meas_w2w_setup_param.home_arfcn[1] = test_param_ptr->src_chan_carrier_1;
    ftm_wcdma_meas_w2w_setup_param.nbr_arfcn = test_param_ptr->tgt_chan;
    ftm_wcdma_meas_w2w_setup_param.home_buffer_index = test_param_ptr->src_rxlm_buf_idx_ant0;
    ftm_wcdma_meas_w2w_setup_param.nbr_buffer_index = test_param_ptr->tgt_rxlm_buf_idx_ant0;
    
    param_ptr = (void*)&ftm_wcdma_meas_w2w_setup_param;
  }

  /* G2W, use a different one */
  else if((test_param_ptr->src_tech == RFCOM_GSM_MODE) && 
          (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE))
  {
    ftm_wcdma_meas_g2w_enter_and_setup_param.neighbor_chan = test_param_ptr->tgt_chan;
    ftm_wcdma_meas_g2w_enter_and_setup_param.startup_buffer = test_param_ptr->tgt_buffer_index;
    ftm_wcdma_meas_g2w_enter_and_setup_param.neighbor_rxlm_buf_idx = test_param_ptr->tgt_rxlm_buf_idx_ant0;

    param_ptr = (void*)&ftm_wcdma_meas_g2w_enter_and_setup_param;
  }

  /* W2L, W2G or L2W use the regular one */
  else if((test_param_ptr->src_tech == RFCOM_WCDMA_MODE) || 
          (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE))
  {
    if(test_param_ptr->src_tech == RFCOM_WCDMA_MODE)
    {
      ftm_wcdma_meas_setup_param.carrier_types[0] = RFCOM_SINGLE_CARRIER ;
      ftm_wcdma_meas_setup_param.channels[0][0] = test_param_ptr->src_chan;
      if (test_param_ptr->src_extra_carrier_flag == TRUE) 
      {
        ftm_wcdma_meas_setup_param.channels[1][0] = test_param_ptr->src_chan_carrier_1; // DBDC case
      }
      ftm_wcdma_meas_setup_param.buf_index = test_param_ptr->src_buffer_index;
      ftm_wcdma_meas_setup_param.rxlm_buf_index = test_param_ptr->src_rxlm_buf_idx_ant0;
    }
    else
    {
      ftm_wcdma_meas_setup_param.carrier_types[0] = RFCOM_SINGLE_CARRIER;
      ftm_wcdma_meas_setup_param.channels[0][0] = test_param_ptr->tgt_chan;
      ftm_wcdma_meas_setup_param.buf_index = test_param_ptr->tgt_buffer_index;
      ftm_wcdma_meas_setup_param.rxlm_buf_index = test_param_ptr->tgt_rxlm_buf_idx_ant0;
    }

    param_ptr = (void*)&ftm_wcdma_meas_setup_param;
  }
  /* Should not come here... just do nothing */
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "wcdma SETUP called for src tech %d and tgt tech %d",
          test_param_ptr->src_tech, test_param_ptr->tgt_tech);
  }

  return (param_ptr);
}

void* ftm_wcdma_meas_get_exit_param(rfm_meas_generic_params_type *test_param_ptr)
{
  void *exit_param_ptr = NULL;

  /* Parameter Check */
  if(test_param_ptr == NULL)
  {
    return exit_param_ptr;
  }

  /* If W2W then return with enter param NULL as we don't need Enter for W2W */
  if( (test_param_ptr->src_tech == RFCOM_WCDMA_MODE) && (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE) )
  {
    return exit_param_ptr;
  }

  else if (test_param_ptr->src_tech == RFCOM_WCDMA_MODE)
  {
    ftm_wcdma_meas_exit_param.device = RFM_DEVICE_0;
    ftm_wcdma_meas_exit_param.channel[0] = test_param_ptr->src_chan;
    ftm_wcdma_meas_exit_param.carrier_type = RFCOM_SINGLE_CARRIER;
    ftm_wcdma_meas_exit_param.buf_index = test_param_ptr->src_buffer_index;
    ftm_wcdma_meas_exit_param.rxlm_buf_index = test_param_ptr->src_rxlm_buf_idx_ant0;
    exit_param_ptr = (void*)&ftm_wcdma_meas_exit_param;
  }
  
  else if (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE)
  {
    ftm_wcdma_meas_exit_param.device = RFM_DEVICE_0;
    ftm_wcdma_meas_exit_param.channel[0] = test_param_ptr->tgt_chan;
    ftm_wcdma_meas_exit_param.carrier_type = RFCOM_SINGLE_CARRIER;
    ftm_wcdma_meas_exit_param.buf_index = test_param_ptr->tgt_buffer_index;
    ftm_wcdma_meas_exit_param.rxlm_buf_index = test_param_ptr->tgt_rxlm_buf_idx_ant0;
    exit_param_ptr = (void*)&ftm_wcdma_meas_exit_param;
  }
  return (exit_param_ptr);
}

boolean ftm_wcdma_meas_enter_meas(rfm_meas_enter_param_type *meas_enter_param)
{
  rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;
  boolean result = TRUE;
  rfm_meas_wcdma_enter_param_type *wtow_meas_param = NULL;
  if(meas_enter_param != NULL)
  {
    /* If W2W then just return with no error */
    if( (meas_enter_param->header.source_tech == RFCOM_WCDMA_MODE) &&
        (meas_enter_param->header.target_tech == RFCOM_WCDMA_MODE) )
    {
       if (meas_enter_param->target_param != NULL) 
       {
          wtow_meas_param = (rfm_meas_wcdma_enter_param_type*)meas_enter_param->target_param;
          /* Update RxLM buf index into state variable for neighbour */
          ftm_wcdma_neighbour_rxlm_buffer = wtow_meas_param->rxlm_buf_index;

          /* Update RxLM buffer for Neighbour before building scripts */
          rfm_update_rxlm_buffer( wtow_meas_param->device, 
                                  wtow_meas_param->channel,
                                  wtow_meas_param->carrier_type,
                                  (uint32)(wtow_meas_param->rxlm_buf_index) );
          return TRUE;
       }

       else
       {
         MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_wcdma_meas_enter_meas: Target enter Params is NULL");
         return FALSE;
       }
    }

    else if(meas_enter_param->header.source_tech == RFCOM_GSM_MODE)
    {
      #if defined(FEATURE_BOLT_MODEM)
      /* Send G2W mdsp sync init command when we start measurement*/
      result = rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_G2W_INIT_CMD_ID, NULL);
      #endif
      if(result == FALSE)
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "G2W MDSP SYNC INIT command failed");
        return result;
      }
 
      /* Wait for Sync init command to complete */
      DALSYS_BusyWait(10000);

      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "Called G2W MDSP SYNC INIT command");     
    }

    else if(meas_enter_param->header.source_tech == RFCOM_LTE_MODE)
    {
      #if defined(FEATURE_BOLT_MODEM)
      /* Send L2W mdsp sync init command when we start measurement*/
      result = rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_L2W_INIT_CMD_ID, NULL);
      #endif

      if(result == FALSE)
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "L2W MDSP SYNC INIT command failed");
        return result;
      }
 
      /* Wait for Sync init command to complete */
      DALSYS_BusyWait(10000);

      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "Called L2W MDSP SYNC INIT command"); 
    }
  }

  else
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_wcdma_meas_enter_meas: Enter Params is NULL");
  }
  
  status = rfmeas_mc_enter(meas_enter_param, NULL);
  if(status != RFM_MEAS_COMMON_SUCCESS)
  {
    return FALSE;
  }
  return TRUE;
}

boolean ftm_wcdma_meas_build_script(rfm_meas_setup_param_type *meas_setup_param)
{
  rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;
  boolean result = TRUE;
  MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"inside ftm_wcdma_meas_build_script()");
  if(meas_setup_param == NULL)
  {
    return FALSE;
  }
  
  status = rfmeas_build_scripts(meas_setup_param, NULL);
  if(status != RFM_MEAS_COMMON_SUCCESS)
  {
    result = FALSE;
  }

  return result;
}


boolean ftm_wcdma_meas_exit_meas(rfm_meas_exit_param_type *meas_exit_param)
{
  rfm_meas_result_type status = RFM_MEAS_COMMON_FAILURE;
  boolean result = TRUE;

  status = rfmeas_mc_exit(meas_exit_param, NULL);
  if(status != RFM_MEAS_COMMON_SUCCESS)
  {
    return FALSE;
  }

  /* Send L2W deinit command after all measurements are done*/
  if( (meas_exit_param->header.source_tech == RFCOM_LTE_MODE) && (meas_exit_param->header.target_tech == RFCOM_WCDMA_MODE) )
  {
    #if defined(FEATURE_BOLT_MODEM)
    /* Send L2W deinit command at last after WFW Stop command and also after Abort command if any */
    result = rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_L2W_DEINIT_CMD_ID, NULL);
    #endif

    if(result == FALSE)
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "L2W DEINIT command failed");
      return result;
    }

    /* Wait for Deinit command to complete */
    DALSYS_BusyWait(10000);

    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "Called L2W DEINIT command");     
  }

  return result;
}


boolean ftm_wcdma_meas_exit_wcdma(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param)
{
  boolean result = TRUE;

  if(test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "NULL param pointer");
    return FALSE;
  }

  /* Stop FW only if L2X and use_src_param == TRUE or
     Stop FW if X2L and use_src_param == FALSE
     Nothing to do if it is W2W
   */
  if(((test_param_ptr->src_tech == RFCOM_WCDMA_MODE) &&
      (test_param_ptr->tgt_tech != RFCOM_WCDMA_MODE) &&
      (use_src_param == TRUE)) ||
     ((test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE) &&
      (test_param_ptr->src_tech != RFCOM_WCDMA_MODE) &&
      (use_src_param == FALSE)))
  {

    if(test_param_ptr->src_tech == RFCOM_WCDMA_MODE)
    {
      /* Suspend WFW as this is W2X and resume WFW after tune back */
      result = ftm_wcdma_meas_suspend_resume(test_param_ptr);
    }

    else
    {
      /* Disable FW and release all resources */
      result = ftm_wcdma_meas_stop(test_param_ptr->src_tech); // Stop WFW completely when this is X2W scenario where is X is not W  
    }
  }
  else if ((test_param_ptr->src_tech == RFCOM_WCDMA_MODE) &&
           (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE))
  {
    result = TRUE;
  }
  else
  {
    result = FALSE;
  }
  if(result != TRUE)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Cannot Disable FW in wcdma, use_src_param %d, src_tech %d, tgt_tech %d",
          use_src_param, test_param_ptr->src_tech, test_param_ptr->tgt_tech);
  }
  return result;
}

boolean ftm_wcdma_meas_enter_wcdma(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param)
{
  boolean result = FALSE; 
  static boolean w2w_tune_away_done = FALSE;

  if(test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "NULL param pointer");
    return FALSE;
  }

  /*  Enable FW only if X2L and use_src_param == FALSE, or
      Enable FW only if L2X and use_src_param == TRUE
   */
  if(((test_param_ptr->src_tech == RFCOM_WCDMA_MODE) &&
      (test_param_ptr->tgt_tech != RFCOM_WCDMA_MODE) &&
      (use_src_param == TRUE)) ||
     ((test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE) &&
      (test_param_ptr->src_tech != RFCOM_WCDMA_MODE) &&
      (use_src_param == FALSE)))
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED, "calling ftm_wcdma_meas_start() src = %d, target = %d",test_param_ptr->src_tech ,test_param_ptr->tgt_tech );

    if(test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE)
    {
      result = ftm_wcdma_meas_start(test_param_ptr->src_tech, test_param_ptr->tgt_rxlm_buf_idx_ant0, test_param_ptr->tgt_chan); // Do this only in X2W case
    }

    else
    {
      result = TRUE; // This is W2X case, when we enter WCDMA meas during W2X, Resume should have already happened. Nothing to do now
    }
  }

  /* W2W Case: Tune first to Neighbour and then Tune back to home */
  else if ((test_param_ptr->src_tech == RFCOM_WCDMA_MODE) &&
           (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE))
  {
     if (w2w_tune_away_done == FALSE) 
     {
        /* This means we are tuned to home and Request is for Tune to Neighbour now*/
        result = ftm_wcdma_meas_wtow_tune_wcdma(test_param_ptr, TRUE);
        w2w_tune_away_done = TRUE;
     }

     else
     {
        /* Tune away has already been done and Request is for Tune back to home now*/
        result = ftm_wcdma_meas_wtow_tune_wcdma(test_param_ptr, FALSE);
        w2w_tune_away_done = FALSE;
     }

    /* Wait for Tune Command to complete */
    DALSYS_BusyWait(10000);
  }

  else
  {
    result = FALSE;
  }

  if(result != TRUE)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Cannot Enable FW in wcdma, use_src_param %d, src_tech %d, tgt_tech %d",
          use_src_param, test_param_ptr->src_tech, test_param_ptr->tgt_tech);
    return result;
  }

  return result;
}




/*===========================================================================

  FUNCTION:  rwcdma_mdsp_exit

===========================================================================*/
/*!
  @brief
  This function disables WCDMA FW .

  @return
  None
*/
/*=========================================================================*/

boolean ftm_wcdma_meas_stop( rfm_mode_enum_type src_tech )
{
  boolean result = TRUE;
  #if defined(FEATURE_BOLT_MODEM)
  #ifdef FEATURE_LTE
  rfwcdma_mdsp_sync_l2w_stop_w_cmd_param_type l2w_stop_w_param;
  #endif
  #endif

  MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "inside ftm_wcdma_meas_stop");

  if(start_called == TRUE)
  {
    if( src_tech == RFCOM_GSM_MODE )
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "inside ftm_wcdma_meas_stop for G2W");
#if defined(FEATURE_BOLT_MODEM)
      /* Send Sleep command to mDSP to disable WFW in G2W */
      result = rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_G2W_SLEEP_CMD_ID, NULL);
#endif

      if(result == FALSE)
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "G2W Sleep command failed");
        return result;
      }

      DALSYS_BusyWait(10000);

      fws_app_disable(FW_APP_WCDMA); 

      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "Called G2W Sleep command");
      return TRUE;
    }

    else if(src_tech == RFCOM_LTE_MODE)
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "inside ftm_wcdma_meas_stop for L2W"); 

      #if defined(FEATURE_BOLT_MODEM)
        #ifdef FEATURE_LTE
        l2w_stop_w_param.instance = 0; // Keep instance as zero.
        /* Send L2W Stop command to WFW*/
        result = rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_L2W_STOP_CMD_ID, &l2w_stop_w_param);
        #endif
      #endif

      if(result == FALSE)
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "L2W Stop command failed");
        return result;
      }

      DALSYS_BusyWait(10000);

      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "Called L2W Sleep command");
      return TRUE;
    }

    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_wcdma_meas_stop: Invalid Source tech %d", src_tech); 
      return FALSE;
    }
  }

  else
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_wcdma_meas_stop: X2W start was not called");
    return FALSE;
  }
}

/*===========================================================================

  FUNCTION:  ftm_wcdma_meas_start

===========================================================================*/
/*!
  @brief
  This function enables WCDMA FW .

  @return
  None
*/
/*=========================================================================*/
boolean ftm_wcdma_meas_start( rfm_mode_enum_type src_tech, lm_handle_type neigh_rxlm_idx, uint16 neigh_chan )
{
  #if defined(FEATURE_BOLT_MODEM)
  rfwcdma_mdsp_sync_g2w_cmd_param_type g2w_cmd;
  #ifdef FEATURE_LTE
  rfwcdma_mdsp_sync_l2w_start_w_cmd_param_type l2w_cmd;
  #endif
  #endif
  int32 rxagc_val = -512;
  uint16 counter = 0;
  boolean result = TRUE;
  rfcom_wcdma_band_type neigh_band = rfwcdma_core_convert_chan_to_band(neigh_chan);

  if( src_tech == RFCOM_GSM_MODE )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "inside ftm_wcdma_meas_start for G2W");

    /* Set up paramaters to tune to neighbour in G2W */

    /* Configure this x2W dedicated mode; both G2W and L2W use the same enum G2W dedicated 
       Only difference in FW is that L2W is expected to be running in a high MDSP clk rate,
       and G2W is expected to be running at the low MDSP clk rate. So WFW will compute the number of searches available
       in the gap differently between the two  */

    #if defined(FEATURE_BOLT_MODEM)
    g2w_cmd.g2w_rxlm_buf_idx = neigh_rxlm_idx;
    g2w_cmd.freq_err_q6_hz = 0 ; // Make Freq Error = zero for now
    g2w_cmd.inv_dlo_freq = ftm_wcdma_meas_get_inverse_dlo_freq(neigh_chan);
    g2w_cmd.pre_cb = NULL;
    g2w_cmd.post_cb = NULL;
    g2w_cmd.nas_id = SYS_MODEM_AS_ID_1; // Set nas id to zero

    /* Send Start command to mDSP to enable WFW in G2W mode*/
    result &= rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_G2W_START_CMD_ID, &neigh_chan);
    #endif

    /* Wait for Start Command to complete */
    DALSYS_BusyWait(10000);

    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "Called G2W MDSP START command");

#if defined(FEATURE_BOLT_MODEM)
    /* Send G2W Config command to tune to WCDMA Neighbour */
    result &= rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_G2W_CONFIG_TUNE_CMD_ID, &g2w_cmd);
#endif

    if(result == FALSE)
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "G2W MDSP Start or Config Tune command failed");
      return result;
    }
    /* Do not change the timeline by printing F3s or by adding anything else after sending Tune command to WFW
    as this would cause reading wrong RxAGC from the mdsp interface*/
  }

  else if(src_tech == RFCOM_LTE_MODE)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "inside ftm_wcdma_meas_start for L2W"); 

    #if defined(FEATURE_BOLT_MODEM)
      #ifdef FEATURE_LTE
      /* Set the Global l2w handle now for L1 usage*/
      l2w_ftm_rxlm_handle = neigh_rxlm_idx;

      l2w_cmd.instance = 0; // Keep instance as zero.
      l2w_cmd.freq_cell_list_ptr.freq = neigh_chan; // W DL Channel
      l2w_cmd.freq_cell_list_ptr.num_psc = 1; // Number of cell is 1
      l2w_cmd.freq_cell_list_ptr.psc_info[0].psc = 1;
      l2w_cmd.freq_cell_list_ptr.psc_info[0].pn_pos = 100; // Position can be between 0 to 307200. Value does not matter in FTM
      l2w_cmd.freq_cell_list_ptr.tuned_w_freq = 0; // This freq is not used in for W as of now.
      /* Send L2W Start and Tune command to WFW*/
      result = rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_L2W_START_AND_TUNE_CMD_ID, &l2w_cmd);
      #endif
    #endif

    if(result == FALSE)
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "L2W MDSP Start and Tune command failed");
      return result;
    }
    /* Do not change the timeline by printing F3s or by adding anything else after sending Tune command to WFW
    as this would cause reading wrong RxAGC from the mdsp interface*/
  }

  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_wcdma_meas_start: Invalid Source tech %d", src_tech); 
    start_called = TRUE;
    return FALSE;
  }

  /* After send Tune Config command. Keep polling until we FW enables RxAGC. Time-out in 10 ms if FW does not enable RxAGC
  Remember in L2W time between Preload and Trigger for tuneaway is 5 ms*/
  do
  {
    DALSYS_BusyWait(RF_WCDMA_X2W_AGC_POLLING_INTERVAL);
    if (rfwcdma_core_util_is_rxagc_on(RFAGC_RECEIVE_CHAIN_0, RFCOM_MULTI_CARRIER_ID__0) == TRUE)
    {
      break;
    }
    counter++;
  }
  while (counter < (RF_WCDMA_AGC_POLLING_TIMEOUT / RF_WCDMA_X2W_AGC_POLLING_INTERVAL));

  if (counter == (RF_WCDMA_AGC_POLLING_TIMEOUT / RF_WCDMA_X2W_AGC_POLLING_INTERVAL))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "FW never enabled RxAGC after %d milli sec", (RF_WCDMA_AGC_POLLING_TIMEOUT/1000));
    rxagc_val = -512;
  }

  else
  {
    /* Wait for 10 bpgs for RxAGC to settle and then read RxAGC irrespective of RxAGC running or not
      So bypass the RxAGCon flag and read RxAGC */
    DALSYS_BusyWait(RF_WCDMA_AGC_SETTLING_TIME);
    rxagc_val = rfm_get_rx_agc_val_xtow_meas ( RFM_DEVICE_0, neigh_band, TRUE);

    if(rxagc_val == -512)
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RxAGC is floored after 10 BPGs of sample capture ");
    }
  }

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "RxAGC starts after %d mic sec and the RxAGC value after X2W Tune is %d", (counter+1)* RF_WCDMA_X2W_AGC_POLLING_INTERVAL, rxagc_val);
  
  start_called = TRUE;

  return TRUE;
}
/*===========================================================================

  FUNCTION:  ftm_wcdma_meas_suspend_resume

===========================================================================*/
boolean ftm_wcdma_meas_suspend_resume(rfm_meas_generic_params_type *test_param_ptr)
{
  boolean result = TRUE;
  #if defined(FEATURE_BOLT_MODEM)
  rfwcdma_mdsp_sync_w2x_cmd_param_type w2x_cmd;
  #endif

  MSG(MSG_SSID_RF, MSG_LEGACY_MED, "inside ftm_wcdma_meas_suspend_resume");

  #if defined(FEATURE_BOLT_MODEM)
  /* Set up paramaters to suspend and resume to neighbour in W2X Idle scenario*/
  w2x_cmd.rxlm_home_buf_idx = test_param_ptr->src_rxlm_buf_idx_ant0; // Home RxLM index for Primary
  w2x_cmd.rxlm_home_rxd_buf_idx = test_param_ptr->src_rxlm_buf_idx_ant1; // Home RxLM index for Diversity
  w2x_cmd.pre_cb = NULL;
  w2x_cmd.post_cb = NULL;
  #endif

  if(test_param_ptr->tgt_tech == RFCOM_GSM_MODE)
  {
    #if defined(FEATURE_BOLT_MODEM)
    w2x_cmd.mode = MDSP_SYNC_W2X_IDLE_CMD_W2G; // W2G Scenario
    #endif
  }

  else if(test_param_ptr->tgt_tech == RFCOM_LTE_MODE)
  {
    #if defined(FEATURE_BOLT_MODEM)
    w2x_cmd.mode = MDSP_SYNC_W2X_IDLE_CMD_W2L; // W2L Scenario
    #endif
  }

  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_wcdma_meas_suspend_resume: Invalid tech %d for W2X Scenario", test_param_ptr->tgt_tech);
    return FALSE;
  }
#if defined(FEATURE_BOLT_MODEM)
  /* Send W2X Suspend/Resume command to WFW for home cell W to suspend/resume accordingly*/
  result = rfwcdma_mdsp_sync_send_cmd( RFWCDMA_MDSP_W2X_SUSPEND_RESUME_CMD_ID, &w2x_cmd);
#endif
  DALSYS_BusyWait(10000); 
  
  MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "Called W2X Suspend Resume Command");

  if(result == FALSE)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "W2X Suspend Resume Command failed");
  }

  return result;
}
/*===========================================================================

  FUNCTION:  ftm_wcdma_meas_get_inverse_dlo_freq

===========================================================================*/
/*!
  @brief
  This function retrieves inverse of the DL frequency based on the operating channel.
 
  @param
  chan - DL channel

  @return
  Inverse of DL channel
*/
/*=========================================================================*/
uint32 ftm_wcdma_meas_get_inverse_dlo_freq(uint16 chan)
  {
  rfcom_wcdma_band_type wcdma_band = rfwcdma_core_convert_chan_to_band(chan);
  uint32 inv_wcdma_freq;

  switch(wcdma_band)
  { 
  case RFCOM_BAND_IMT:
    inv_wcdma_freq = INV_MID_FREQ_WCDMA_2100_BAND;
    break;

  case RFCOM_BAND_1900:
    inv_wcdma_freq = INV_MID_FREQ_WCDMA_1900_BAND;
    break;

  case RFCOM_BAND_BC3:
    inv_wcdma_freq = INV_MID_FREQ_WCDMA_BC3_BAND;
    break;

  case RFCOM_BAND_BC4:
    inv_wcdma_freq = INV_MID_FREQ_WCDMA_BC4_BAND;
    break;

  case RFCOM_BAND_800:
    inv_wcdma_freq = INV_MID_FREQ_WCDMA_800_BAND;
    break;

  case RFCOM_BAND_BC8:
    inv_wcdma_freq = INV_MID_FREQ_WCDMA_BC8_BAND;
    break;

  case RFCOM_BAND_BC9:
    inv_wcdma_freq = INV_MID_FREQ_WCDMA_BC9_BAND;
    break;

  default:
    inv_wcdma_freq = INV_MID_FREQ_WCDMA_2100_BAND;
    break;
  }

  return inv_wcdma_freq;
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function will test interfreq and interband for WCDMA to WCDMA
   by writing preload and trigger scripts for either home or neighbour
   depeding on RFLM handle and multi carrier index

    @param test_param_ptr
    All paramters that has info about source and target info in W2W
 
    @param neighborxlm_buf_idx
    Home/Neighbour WCDMA RxLM buffer Index
 
    @return
    API return status

*/
boolean ftm_wcdma_meas_wtow_tune_wcdma( rfm_meas_generic_params_type *test_param_ptr, boolean is_wcdma_neighbour )
{
  rflm_wcdma_rf_send_script_in_t  in_preload = {0,FALSE,RF_WCDMA_EVENT_RX_MEAS_PRELOAD};
  rflm_wcdma_rf_send_script_in_t  in_trigger = {0,FALSE,RF_WCDMA_EVENT_RX_MEAS_TRIGGER};
  rflm_wcdma_rf_send_script_out_t out_preload, out_trigger;
  rflm_err_t ret_err = RFLM_ERR_NONE;
  rfwcdma_mdsp_agc_action_type cmd_action[RFM_DEVICE_4] = 
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };
  uint32 rxlm_buf_idx[RFM_DEVICE_4] = {0};
  uint32 multi_carrier_idx = 1; // Need to changed later for DBDC and SBDC support.
  boolean tune_api_status = TRUE;

  MSG(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_write_scripts start");

  if (test_param_ptr == NULL) 
  {
     MSG(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_wcdma_meas_wtow_tune_wcdma: Test Params is NULL");
     return FALSE;
  }

  in_preload.effect_time = rflm_time_read_ustmr_count();

  /* Add about 1 ms (19200) between Preload and trigger write time */
  in_trigger.effect_time = rflm_time_read_ustmr_count() + 19200;

  if(is_wcdma_neighbour)
  {

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_write_scripts: Neighbour with handle %d:", ftm_wcdma_neighbour_rxlm_buffer);

    /* Push DM buffer to CCS for Preload event */
    ret_err |= rflm_wcdma_rf_send_script( test_param_ptr->tgt_rxlm_buf_idx_ant0,
                                          &in_preload,
                                          &out_preload );

    /* Push DM buffer to CCS for Trigger/Non Trx event */
    ret_err |= rflm_wcdma_rf_send_script( test_param_ptr->tgt_rxlm_buf_idx_ant0,
                                          &in_trigger,
                                          &out_trigger );

    cmd_action[RFM_DEVICE_0] = RFWCDMA_MDSP_AGC_CMD_DISABLE;
    rxlm_buf_idx[RFM_DEVICE_0] =  test_param_ptr->src_rxlm_buf_idx_ant0;

    /* Stop RxAGC on home and start on Neighbour */
    rfwcdma_core_rxctl_enable_rxagc(multi_carrier_idx,
                                    cmd_action,
                                    rxlm_buf_idx,
                                    FALSE,0,0);

    /* Wait for Disable command to be completed */
    DALSYS_BusyWait(2000);

    cmd_action[RFM_DEVICE_0] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
    rxlm_buf_idx[RFM_DEVICE_0] =  test_param_ptr->tgt_rxlm_buf_idx_ant0;

    rfwcdma_core_rxctl_enable_rxagc(multi_carrier_idx,
                                    cmd_action,
                                    rxlm_buf_idx,
                                    FALSE,0,0);
  }

  else
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_write_scripts: Home with handle %d:", ftm_wcdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);

    /* Push DM buffer to CCS for Preload event */
    ret_err |= rflm_wcdma_rf_send_script( test_param_ptr->src_rxlm_buf_idx_ant0,
                                          &in_preload,
                                          &out_preload );
    /* No Preload for Home */
    /* Push DM buffer to CCS for Trigger/Non Trx event */
    ret_err |= rflm_wcdma_rf_send_script( test_param_ptr->src_rxlm_buf_idx_ant0,
                                          &in_trigger,
                                          &out_trigger ); 


    cmd_action[RFM_DEVICE_0] = RFWCDMA_MDSP_AGC_CMD_DISABLE;
    rxlm_buf_idx[RFM_DEVICE_0] = test_param_ptr->tgt_rxlm_buf_idx_ant0;

     /* Stop RxAGC on Neighbour and start on Home */
    rfwcdma_core_rxctl_enable_rxagc(multi_carrier_idx,
                                    cmd_action,
                                    rxlm_buf_idx,
                                    FALSE,0,0);


    /* Wait for Disable command to be completed */
    DALSYS_BusyWait(2000);

    cmd_action[RFM_DEVICE_0] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
    rxlm_buf_idx[RFM_DEVICE_0] = test_param_ptr->src_rxlm_buf_idx_ant0;

    rfwcdma_core_rxctl_enable_rxagc(multi_carrier_idx,
                                    cmd_action,
                                    rxlm_buf_idx,
                                    FALSE,0,0);

  }

  if(ret_err != RFLM_ERR_NONE)
  {
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_wcdma_meas_wtow_tune_wcdma: Failed for Home RxLM %d andNeighbour RxLM %d", 
          test_param_ptr->src_rxlm_buf_idx_ant0, test_param_ptr->tgt_rxlm_buf_idx_ant0);
    tune_api_status = FALSE;
  }

  return tune_api_status;
}



