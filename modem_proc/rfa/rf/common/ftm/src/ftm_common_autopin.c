/*!
  @file
  ftm_common_autopin.c

  @brief
  This module contains implementation to acquire and release locks for critical
  section code execution
 
  @addtogroup RF_COMMON_FTM_AUTOPIN
  @{
*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_autopin.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/21/15   vr      Autopin check-in 
12/21/15   bir     GNU C warning Fix
12/09/15   vkm     Updated tx_chain while wtr capture
12/08/15   vkm     added new api for params capture for autopin 
12/07/15   ag       Fix kw warnings
12/03/15   kma     Updated CCS delay by gain state
11/16/15   ag      Autopin Changes for WCDMA
11/27/15   as      KW fix
11/27/15   as      KW fix
11/19/15   whz     Enable TDS event
11/18/15   kma     Logged capture time
11/13/15   kma     Updated WTR capture size API interface
11/10/15   kma     Removed TA featurization
11/12/15   kg      PAR support for LTE
11/04/15   kma     Updated WTR interface
11/03/15   kma     Updated WTR capture time
11/03/15   kma     Added capability to dump capture into EFS
11/02/15   kma     Updated API interface to fetch capture params
10/23/15   kma     Added REFLOG capture for TDS
10/21/15   kma     Added CCS support for WTR capture
10/08/15   ag      Fix W Tx device update sequence
09/30/15   kma     Initial revision

==============================================================================*/

#include "ftm_common_autopin.h"
#include "rfcommon_core_utils.h"
#include "comdef.h"
#include "ftm.h"
#include "ftm_task.h"
#include "ftm_msg.h"
#include "ftmdiag.h"
#include "stringl.h" /* for memscpy */
#include "rflm_ccs_intf.h"
#include "txlm_hal.h"
#include "rfcommon_autopin_msm.h"
#include "rfcommon_autopin.h"

#ifdef FEATURE_LTE
#include "rflte_util.h"
#include "rflte_mc.h"
#include "rfdevice_lte_interface.h"
#include "rflte_ftm_mc.h"
#include "rflm_lte.h"
#endif

#ifdef FEATURE_WCDMA
#include "rfwcdma_mc.h"
#include "ftm_hwtc_wcdma_ino.h"
#include "rfdevice_wcdma_intf.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_wcdma_ccs.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "rfdevice_tdscdma_intf.h"
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_mc.h"
#include "rflm_tds_ccs.h"
#include "rflm_tds_mc.h"
#include "rf_tdscdma_mdsp_rf_event.h"
extern rflm_tds_rf_mode_ctl_s rflmTdsRfModeCtlState;
extern uint32 ftm_tdscdma_get_lm_idx(void);
#endif

#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rf_cdma_utils_freq.h"
#include "rf_buffer_intf.h"
#include "rf_cdma_mc.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_c2k_mc.h"
#include "rflm_ccs_irat_intf.h"
#include "rflm_api_c2k.h"
#include "rflm_cdma_fw_intf.h"
#include "rflm_dm_api.h"
#include "rflm_time.h"
#include "rf_cdma_auto_pin.h"
#endif

/*Fixed 1ms action time with respect to current USTMR*/
#define RF_AUTOPIN_CCS_ACTION_TIME ((uint32)(19200000/1000000.0*1000 + 1))

extern uint32 * 
rflm_ccs_get_data_buf_item_ptr
( 
   rflm_ccs_data_buf_item_type data_buf_item
);

extern uint32 rfcommon_autopin_reflog_sample[RFLM_AUTOPIN_REFLOG_MAX_SAMPLE_SIZE];

uint32 reflog_par_result;

uint32 current_ustmr;
uint32 current_txc_sync;
uint32 wtr_capture_time;
uint32 reflog_start_time;

#ifdef FEATURE_FACTORY_TESTMODE

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_init_tx
 
  @details
  This function tests the common FBRx init tx API
 
*/
ftm_rsp_pkt_type ftm_common_autopin_capture(ftm_common_pkt_type *cmd_ptr)
{
  ftm_autopin_response_type* response_ptr = NULL;
  ftm_autopin_capture_param_type* param_ptr = NULL;
  boolean capture_status = TRUE;
  rfdevice_fbrx_autopin_data_type wtr_capture_result;
  rfdevice_rxtx_common_class* device_ptr = NULL;
  ftm_rf_technology_type curr_ftm_rf_tech = FTM_RF_TECH_UNKNOWN;
  rfm_device_enum_type tx_device = RFM_DEVICE_0;
  rfcom_band_type_u band_union;
  uint32 refTime;
  uint32 extra_capture_time;
  uint32 new_delay;
  uint32 txc_rate_ustmr;
  uint32 *ccsDestPtr;
  txlm_chain_type tx_chain = TXLM_CHAIN_MAX;
  lm_handle_type tx_handle_id;
  rfcom_lte_bw_type bw ; 
  
  /*REFLOG Capture param*/
  fw_autopin_capture_input_t reflog_capture_input;
  uint16 wtr_capture_size;
  fw_autopin_psamp_input_t reflog_proc_param;
  fw_autopin_output_params_t reflog_output;
 


  /*Variable to point to common FW memory */
  fw_rf_common_intf_s* rfcommon_data_from_fw = (fw_rf_common_intf_s *) fws_smem_get_interface(FW_SMEM_INTF_COMMON_RF);

  #ifdef FEATURE_CDMA
  const rf_cdma_data_status_type *dev_status_r;
  const rfc_cdma_logical_device_params_type* logical_dev;
  #endif

  #ifdef FEATURE_TDSCDMA
  rflm_tds_rf_ccs_intf_s  *ccsIntfPtr = &(rflmTdsRfModeCtlState.ccsInfo.rfCcsIntf);
  rflm_tds_ccs_exec_single_task_info_t taskInfo;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data = { 0 };
  #endif

 #ifdef FEATURE_LTE
  uint32 ccs_data_memory_location_log = 0;
  rflm_lte_tech_data_s* lte_data = NULL;
#endif
  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_autopin_response_type));

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_autopin_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_autopin_capture_param_type*)&(cmd_ptr->ftm_extended_params);

  curr_ftm_rf_tech = ftm_get_rf_technology();

  switch (curr_ftm_rf_tech)
  {
#if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA)
     case FTM_RF_TECH_CDMA:
       tx_device = rf_cdma_data_get_curr_tx_device();
       dev_status_r = rf_cdma_get_device_status(tx_device);     // what is the current tx device?
       /* Get Logical Device Param */
       logical_dev = rfc_cdma_get_logical_device_params( tx_device );
     
         band_union.cdma_band = dev_status_r->curr_band;
         device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_1X_MODE, band_union);
       
       if (dev_status_r == NULL || logical_dev == NULL)
       {
         FTM_MSG(FTM_ERROR, "NULL Logical or dev_status");
       }
       break;
#endif

#ifdef FEATURE_WCDMA
     case FTM_RF_TECH_WCDMA:
         tx_device =  rfwcdma_mc_state.car_path_state[RF_PATH_0][0].tx_rfm_device;
         if (tx_device < RFM_MAX_WAN_DEVICES)
         {
           band_union.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx;
           device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_WCDMA_MODE, band_union);
           tx_handle_id = (rflm_handle_tx_t)rfwcdma_mc_state.txlm_buf_idx; 
         }
         else
	 {
           FTM_MSG_1(FTM_ERROR, "INVALID Tx device %d",tx_device ); 
           return rsp;;
         }
    
         if (device_ptr == NULL)
         {
           FTM_MSG(FTM_ERROR, "Tx device ptr is NULL"); 
           return rsp;
         }

       break;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
     case FTM_RF_TECH_LTE:
       band_union.lte_band = rflte_ftm_mc_sm_get_band();
       tx_device = rflte_ftm_mc_sm_get_tx_device();
       device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_LTE_MODE, band_union);
       break;
#endif
#ifdef FEATURE_TDSCDMA
     case FTM_RF_TECH_TDSCDMA:
       band_union.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
       tx_device = RFM_DEVICE_0;
       device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_TDSCDMA_MODE, band_union);
       break;
#endif
     default:
       FTM_MSG(FTM_ERROR, "ftm_common_autopin_capture: Unsupported Tech Type");
  }

  switch (param_ptr->capture_type)
  {
     case FTM_AUTOPIN_PAR_WTR_CAPTURE:
       capture_status &= rfdevice_cmn_fbrx_autopin_enable(device_ptr,
                                                          RFDEVICE_EXECUTE_IMMEDIATE,
                                                          NULL,
                                                          param_ptr->fbrx_gain_state,
                                                          WTR_DEBUG_MEM_SIZE,
                                                          wtr_debug_mem_enable);
       if (capture_status)
       {
         capture_status &= rfdevice_cmn_fbrx_autopin_read_and_disable(device_ptr,
                                                                      &wtr_capture_result,
                                                                      wtr_debug_mem_enable,
                                                                      wtr_debug_mem);
       }
       else
       {
         FTM_MSG_1( FTM_HIGH, "ftm_common_autopin_capture: WTR capture trigger status %d", capture_status);
       }

       if (capture_status)
       {
         response_ptr->wtr_capture_response.sum_i_q_sqr = wtr_capture_result.sum_i_q_sqr;
         response_ptr->wtr_capture_response.sum_i_and_q_sqr_ave = wtr_capture_result.sum_i_and_q_sqr_ave;
         response_ptr->wtr_capture_response.p_ave = wtr_capture_result.p_ave;
         response_ptr->wtr_capture_response.sum_i = wtr_capture_result.sum_i;
         response_ptr->wtr_capture_response.sum_q = wtr_capture_result.sum_q;
         response_ptr->wtr_capture_response.i_dc = wtr_capture_result.i_dc;
         response_ptr->wtr_capture_response.q_dc = wtr_capture_result.q_dc;
         response_ptr->wtr_capture_response.fbrx_accum = wtr_capture_result.fbrx_accum;
         response_ptr->wtr_capture_response.sum_peaks = wtr_capture_result.sum_peaks;
         response_ptr->wtr_capture_response.num_peaks = wtr_capture_result.num_peaks;
         response_ptr->wtr_capture_response.par = wtr_capture_result.par;
         FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: PAR %d, Number of peaks %d, sum of peaks %d, Avg power %d", 
                    wtr_capture_result.par, 
                    wtr_capture_result.num_peaks,
                    wtr_capture_result.sum_peaks,
                    wtr_capture_result.p_ave );
         FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: sum i %d, sum q %d, i dc %d, q dc %d", 
                    wtr_capture_result.sum_i, 
                    wtr_capture_result.sum_q,
                    wtr_capture_result.i_dc,
                    wtr_capture_result.q_dc );
       }
       response_ptr->status = capture_status;
       
       break;

     case FTM_AUTOPIN_PAR_WTR_REFLOG_CAPTURE:
       switch (curr_ftm_rf_tech)
       {
       #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA)
          case FTM_RF_TECH_CDMA:
       {

	  rflm_c2k_data_t* rflm_cdma_data_ptr = rflm_c2k_get_data();
	  txlm_hal_get_chain(dev_status_r->txlm_handle, &tx_chain);
	  rfm_mode_enum_type rfm_tech_mode = ftm_common_ftm_tech_to_rfm_mode(
		  				curr_ftm_rf_tech );
	  rflm_tech_id_t rflm_tech_mode = rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech (
	  	                               rfm_tech_mode);
	  rflm_cdma_mdsp_script_id_t event_id;
          rflm_ccs_event_info_s event_info;
	  /*! @brief onex c2k tx data */
	  rflm_c2k_tx_data_t* tx_data; 
	  rfmeas_event_infor_type* autopin_event;
	  rflm_autopin_dm_template_t *autopin_template = NULL;
	  rflm_c2k_auto_pin_data_t* autopin_data;

	  if (rflm_cdma_data_ptr != NULL)
	  {
   	    if (rfm_tech_mode == RFM_1X_MODE)
 	    {
	      autopin_data = &(rflm_cdma_data_ptr->onex_data.autopin_data);
            } 
	    else
	    {
	      autopin_data = &(rflm_cdma_data_ptr->hdr_data.autopin_data);
	    }
  	  }


          if ( rfm_get_calibration_state() == TRUE )	
          {
            fw_autopin_capture_input_t *autopin_temp_dm_ptr;
   	    uint16 *wtr_capture_size;	
	
	    autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(dev_status_r->txlm_handle);
	    if (autopin_template == NULL)
	    {
	      FTM_MSG_2(FTM_HIGH, "Failed to get AutoPin buffer from DM for tech %d, Tx handle %d",
	        rfm_tech_mode, dev_status_r->txlm_handle);
 	      return rsp;
	    }

	    autopin_temp_dm_ptr = &autopin_template->autopin_ctl.reflog_capture_input;
	    wtr_capture_size = &autopin_template->autopin_ctl.wtr_capture_size;

	    autopin_temp_dm_ptr->tx_chain = (uint32)tx_chain;
	    autopin_temp_dm_ptr->tx_iq_sample_ptr = rfcommon_autopin_reflog_sample;
	    autopin_temp_dm_ptr->ftm_mode = TRUE;
			  
 	    FTM_MSG_2(FTM_HIGH, "ftm_common_autopin_capture: chain %d, tx handle %d ", tx_chain, dev_status_r->txlm_handle);


            if (param_ptr->fbrx_gain_state == 2)
	    {
              event_id = RFLM_CDMA_MDSP_CCS_EVENT_AUTOPIN_TRIGGER_G2_ID;
	    }	
	    else
	    {
	      event_id = RFLM_CDMA_MDSP_CCS_EVENT_AUTOPIN_TRIGGER_G1_ID;
	    }

	    autopin_event = 
	 	 (rfmeas_event_infor_type*)rflm_dm_get_script_buf_ptr(
					 (rflm_dm_handle_id_t) dev_status_r->txlm_handle, event_id);
	    ASSERT(autopin_event != NULL);

	    current_ustmr = HWIO_IN(STMR_TIME_RD);
	    current_txc_sync = HWIO_INI(UNIV_STMR_TX_SYNC_TIMEn, autopin_temp_dm_ptr->tx_chain);

 	    txc_rate_ustmr = (uint32)(19200000 / 1000000.0 * autopin_temp_dm_ptr->txc_rate_ns / 1000);

	    /*Calculate capture time = Current TXC sync time + current USTMR time + 2*TXC rollover period 
            This is FTM mode only. In online mode ref time = slot boundary or subframe boundary*/
            refTime = ((current_txc_sync + (current_ustmr & 0xC00000)) +
                       2 * txc_rate_ustmr ) & 0xFFFFFF;
              
            refTime += autopin_data->delay_in_capture_time;

            autopin_temp_dm_ptr->tx_capture_time = (refTime&0xFFFFFF);

	   /*Adjust WTR capture time = ref time + capture delay -  CCS programming time 181us*/
	    refTime +=  autopin_temp_dm_ptr->tx_capture_delay;


            if (wtr_debug_mem_enable)
            {
             if (param_ptr->fbrx_gain_state == 2)
             {
              refTime -= RF_AUTOPIN_CCS_WTR_PAR_G2_DELAY_DEBUG;
             }
             else
             {
              refTime -= RF_AUTOPIN_CCS_WTR_PAR_G1_DELAY_DEBUG;
             }
            }
           else
           {
            if (param_ptr->fbrx_gain_state == 2)
            {
             refTime -= RF_AUTOPIN_CCS_WTR_PAR_G2_DELAY;
            }
            else
            {
              refTime -= RF_AUTOPIN_CCS_WTR_PAR_G1_DELAY;
            }
           }
           /*Add time alignment*/
           refTime += AUTOPIN_PAR_CDMA_CAPTURE_TIME_OFFSET;
	  
           wtr_capture_time = refTime & 0x3FFFFF;
           reflog_start_time = autopin_temp_dm_ptr->tx_capture_time + autopin_temp_dm_ptr->tx_capture_delay;

           event_info = rflm_ccs_enqueue_common_rf_event( (rflm_ccs_rf_event_header_task_type *)&(autopin_event->header_task),
						    (refTime&0x3FFFFF),
					            (uint32*)rflm_c2k_get_ccs_mem_addr( 
						    event_id,rflm_tech_mode ),
						    RFLM_CDMA_ENABLE_HKADC_BUF_SIZE*4,
						    ftm_common_autopin_wtr_callback,
						    NULL,
					        rflm_c2k_ccs_get_tx_tq(rflm_tech_mode)->tq_handle);

           /*Config REFLOG capture*/
           rfcommon_data_from_fw->fw_autopin_fn(autopin_temp_dm_ptr,
					 &reflog_proc_param,
					 &reflog_output,
					 ftm_common_autopin_reflog_callback);
  
	    FTM_MSG_6 ( FTM_HIGH,  "C2K AutoPin scheduled at USTMR 0x%08x, curr USTMR 0x%08x,"
		                     " tx_captu_time  0x%08x tx_capt_delay:  0x%08x wtr_debug_mem_enable:%d,"
		                     "nof Transaction bytes: %d",
				     refTime, rflm_time_read_ustmr_count(),
				     autopin_temp_dm_ptr->tx_capture_time,
				     autopin_temp_dm_ptr->tx_capture_delay, wtr_debug_mem_enable, event_info.size_transaction_data_bytes );


        }
		 
  }

            break;
       #endif

       #ifdef FEATURE_WCDMA
          case FTM_RF_TECH_WCDMA:

            rfcommon_msm_get_txc_rollover_period((lm_handle_type)tx_handle_id,
                                                 &reflog_capture_input.txc_input_freq_hz,
                                                 &reflog_capture_input.txc_rate_ns,
                                                 &reflog_capture_input.txrot_clk_hz);

            FTM_MSG_3(FTM_HIGH, "TXC param: input freq = %d, txc rate = %d, clk rate = %d",
                      reflog_capture_input.txc_input_freq_hz, reflog_capture_input.txc_rate_ns, reflog_capture_input.txrot_clk_hz);

            rfcommon_msm_get_par_capture_params(RFLM_TECH_WCDMA,
                                                &(reflog_capture_input.capture_length),
                                                &(reflog_capture_input.txc_down_sampling_rate),
                                                &(reflog_capture_input.tx_capture_delay),
                                                &wtr_capture_size);

            FTM_MSG_3(FTM_HIGH, "TXC param: REFLOG capture size = %d, downsample rate = %d, capture delay = %d",
                      reflog_capture_input.capture_length,
                      reflog_capture_input.txc_down_sampling_rate,
                      reflog_capture_input.tx_capture_delay);

            (void)txlm_hal_get_chain(tx_handle_id, &tx_chain);
            reflog_capture_input.tx_chain = (uint32)tx_chain;
            reflog_capture_input.tx_iq_sample_ptr = rfcommon_autopin_reflog_sample;
            reflog_capture_input.ftm_mode = TRUE;

            rflm_ccs_event_info_s event_info; /* Event enqueing info */
            uint32 *ccs_mem_offset; /* CSS memory offset address */
            uint32 script_size; /* Event size */
            rflm_wcdma_ccs_tq_t *tq_ptr; /* Task Queue Pair to be used */
            rfmeas_event_infor_type *autopin_event; /* Event info */

            if (param_ptr->fbrx_gain_state == 1)
            {

              /* Get the ENABLE AUTOPIN  event; and assert if not valid */
              autopin_event =
                 (rfmeas_event_infor_type *)rflm_dm_get_script_buf_ptr(tx_handle_id, RF_WCDMA_EVENT_ENABLE_AUTOPIN_GAIN1);
              /* Get the CSS memory offset for the event; and assert if not valid */
              ccs_mem_offset = rflm_wcdma_get_ccs_mem_addr(tx_handle_id, RF_WCDMA_EVENT_ENABLE_AUTOPIN_GAIN1);

            } else if (param_ptr->fbrx_gain_state == 2)
            {
              /* Get the ENABLE AUTOPIN  event; and assert if not valid */
              autopin_event =
                 (rfmeas_event_infor_type *)rflm_dm_get_script_buf_ptr(tx_handle_id, RF_WCDMA_EVENT_ENABLE_AUTOPIN_GAIN2);
              /* Get the CSS memory offset for the event; and assert if not valid */
              ccs_mem_offset = rflm_wcdma_get_ccs_mem_addr(tx_handle_id, RF_WCDMA_EVENT_ENABLE_AUTOPIN_GAIN2);

            } else
            {
              FTM_MSG_1(RF_ERROR, "Wrong gain state %d passed", param_ptr->fbrx_gain_state);
	      return rsp;
            }

            ASSERT(autopin_event != NULL);

            ASSERT(ccs_mem_offset != 0x0);

            /* Get the script size for the event */
            script_size = rflm_wcdma_get_event_max_size(RF_WCDMA_EVENT_ENABLE_AUTOPIN_GAIN1);

            /* Get the TQ pair to be used for the event; and assert if not valid */
            tq_ptr = rflm_wcdma_ccs_get_tx_tq();
            ASSERT(tq_ptr != NULL);
            current_ustmr = HWIO_IN(STMR_TIME_RD);
            current_txc_sync = HWIO_INI(UNIV_STMR_TX_SYNC_TIMEn, reflog_capture_input.tx_chain);

			txc_rate_ustmr = ((uint32)(19200000 / 1000000.0 * reflog_capture_input.txc_rate_ns / 1000));

            /*Calculate capture time = Current TXC sync time + current USTMR time + 2*TXC rollover period 
              This is FTM mode only. In online mode ref time = slot boundary or subframe boundary*/
            refTime = ((current_txc_sync + (current_ustmr & 0xC00000)) +
                       2 * txc_rate_ustmr ) & 0xFFFFFF;

			
            extra_capture_time = (((uint32)reflog_capture_input.tx_capture_delay)/txc_rate_ustmr)*txc_rate_ustmr;
            new_delay = reflog_capture_input.tx_capture_delay - extra_capture_time;
						
	    refTime = refTime + extra_capture_time;
	    reflog_capture_input.tx_capture_delay = new_delay; 

            reflog_capture_input.tx_capture_time = refTime;
            /*Adjust WTR capture time = ref time + capture delay -  CCS programming time 181us*/
            refTime += reflog_capture_input.tx_capture_delay; 
            if (wtr_debug_mem_enable)
            {
              if (param_ptr->fbrx_gain_state == 2)
              {
                refTime -= RF_AUTOPIN_CCS_WTR_PAR_G2_DELAY_DEBUG;
              }
              else
              {
                refTime -= RF_AUTOPIN_CCS_WTR_PAR_G1_DELAY_DEBUG;
              }
            }
            else
            {
              if (param_ptr->fbrx_gain_state == 2)
              {
                refTime -= RF_AUTOPIN_CCS_WTR_PAR_G2_DELAY;
              }
              else
              {
                refTime -= RF_AUTOPIN_CCS_WTR_PAR_G1_DELAY;
              }
            }
            
            /*Add time alignment*/
            refTime += AUTOPIN_PAR_WCDMA_CAPTURE_TIME_OFFSET;
    
            wtr_capture_time = refTime & 0x3FFFFF;
            reflog_start_time = reflog_capture_input.tx_capture_time + reflog_capture_input.tx_capture_delay;

            FTM_MSG_5(FTM_HIGH, "ftm_common_autopin_capture: Enqueue at USTMR time %d, chain %d, tx handle %d,ccs_mem_offset 0x%x,script_size %d  ", refTime&0x3FFFFF, tx_chain, tx_handle_id,ccs_mem_offset,script_size);        

            /* Enqueue the event to CCS */
            event_info = rflm_ccs_enqueue_common_rf_event((rflm_ccs_rf_event_header_task_type *)(&(autopin_event->header_task)),
                                                          (refTime&0x3FFFFF),
                                                          ccs_mem_offset,
                                                          script_size,
                                                          ftm_common_autopin_wtr_callback,
                                                          NULL,
                                                          tq_ptr->tq_handle);


            /*Config REFLOG capture*/
            rfcommon_data_from_fw->fw_autopin_fn(&reflog_capture_input,
                                                 &reflog_proc_param,
                                                 &reflog_output,
                                                 ftm_common_autopin_reflog_callback);

            /* Debug Message */
            FTM_MSG_3(RF_MED,
                      "[WCDMA][ENABLE_AUTOPIN][Schedule] @ Autopin_script_ptr 0x%x, ccs_mem_offset 0x%x, "
                      "script_size %d ",
                      autopin_event, ccs_mem_offset,
                      script_size);



            /* Check the final script size*/
            if (event_info.size_transaction_data_bytes > RFLM_WCDMA_ENABLE_AUTOPIN_SIZE * 4)
            {
              ERR_FATAL("[WCDMA][ENABLE_AUTOPIN][Schedule]: Script size %d exceeds"
                        "allocated script size %d",
                        event_info.size_transaction_data_bytes,
                        RFLM_WCDMA_ENABLE_AUTOPIN_SIZE * 4, 0);
            } /* if( event_info.size_transaction_data_bytes > RFLM_WCDMA_WRITE_HDET_SIZE*4 ) */


            /* Debug Message */
            FTM_MSG_3(RF_MED,
                      "[WCDMA][ENABLE_AUTOPIN][Schedule] @ USTMR %d,"
                      "CCS trans size %d, num of tasks %d ",
                      refTime,
                      event_info.size_transaction_data_bytes,
                      event_info.num_ccs_tasks);
            break;
       #endif /* FEATURE_WCDMA */

       #ifdef FEATURE_LTE
          case FTM_RF_TECH_LTE:
            tx_handle_id = rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX);
            lte_data = rflm_lte_dm_get_data_ptr(RFLM_LTE_MC_CELL0);
            txlm_hal_get_chain(tx_handle_id, &tx_chain);

            if(lte_data != NULL)
            {
              rfcommon_msm_get_txc_rollover_period(tx_handle_id, 
                                                   &reflog_capture_input.txc_input_freq_hz, 
                                                   &reflog_capture_input.txc_rate_ns,
                                                   &reflog_capture_input.txrot_clk_hz);
              FTM_MSG_3(FTM_HIGH, "TXC param: input freq = %d, txc rate = %d, clk rate = %d", 
                        reflog_capture_input.txc_input_freq_hz, reflog_capture_input.txc_rate_ns, reflog_capture_input.txrot_clk_hz);
              
              bw = rflte_ftm_mc_sm_get_tx_bw();
              rfcommon_msm_get_par_capture_params_lte(&(reflog_capture_input.capture_length),
                                                  &(reflog_capture_input.txc_down_sampling_rate), 
                                                  &(reflog_capture_input.tx_capture_delay),
                                                  &wtr_capture_size,
                                                   bw);
              
              FTM_MSG_3(FTM_HIGH, "TXC param: REFLOG capture size = %d, downsample rate = %d, capture delay = %d", 
                        reflog_capture_input.capture_length, 
                        reflog_capture_input.txc_down_sampling_rate, 
                        reflog_capture_input.tx_capture_delay);
              
              reflog_capture_input.tx_chain = (uint32)tx_chain;
              reflog_capture_input.tx_iq_sample_ptr = rfcommon_autopin_reflog_sample;
              reflog_capture_input.ftm_mode = TRUE;
              
              current_ustmr = HWIO_IN(STMR_TIME_RD);
              current_txc_sync = HWIO_INI(UNIV_STMR_TX_SYNC_TIMEn, reflog_capture_input.tx_chain);
              
              /*Calculate capture time = Current TXC sync time + current USTMR time + 2*TXC rollover period 
                This is FTM mode only. In online mode ref time = slot boundary or subframe boundary*/
              refTime = ((current_txc_sync + (current_ustmr&0xC00000))+
                        2*((uint32)(19200000/1000000.0*(reflog_capture_input.txc_rate_ns/1000))))&0xFFFFFF;
              
              reflog_capture_input.tx_capture_time = refTime;
              /*Adjust WTR capture time = ref time + capture delay -  CCS programming time 181us*/
              refTime += reflog_capture_input.tx_capture_delay; 
              if (wtr_debug_mem_enable)
              {
                if (param_ptr->fbrx_gain_state == 2)
                {
                  refTime -= RF_AUTOPIN_CCS_WTR_PAR_G2_DELAY_DEBUG;
                }
                else
                {
                  refTime -= RF_AUTOPIN_CCS_WTR_PAR_G1_DELAY_DEBUG;
                }
              }
              else
              {
                if (param_ptr->fbrx_gain_state == 2)
                {
                  refTime -= RF_AUTOPIN_CCS_WTR_PAR_G2_DELAY;
                }
                else
                {
                  refTime -= RF_AUTOPIN_CCS_WTR_PAR_G1_DELAY;
                }
              }
              
              /*Add time alignment*/
              refTime += AUTOPIN_PAR_LTE_CAPTURE_TIME_OFFSET;
              
              wtr_capture_time = refTime & 0x3FFFFF;
              reflog_start_time = reflog_capture_input.tx_capture_time + reflog_capture_input.tx_capture_delay;
              
              if (param_ptr->fbrx_gain_state ==2)
              {
                 rflm_lte_ccs_commit_rf_event((refTime & 0x3FFFFF),
                                           &ccs_data_memory_location_log,
                                           RFLM_LTE_CALL_BACK_INTERRUPT_ENABLED,
                                           ftm_common_autopin_wtr_callback,
                                           lte_data,
                                           RFLM_LTE_RF_EVENT_HDET_PAR_ENABLE_GS2,
                                           RFLM_LTE_MC_CELL0);
              }
              else
              {
                rflm_lte_ccs_commit_rf_event((refTime & 0x3FFFFF),
                                           &ccs_data_memory_location_log,
                                           RFLM_LTE_CALL_BACK_INTERRUPT_ENABLED,
                                           ftm_common_autopin_wtr_callback,
                                           lte_data,
                                           RFLM_LTE_RF_EVENT_HDET_PAR_ENABLE_GS1,
                                           RFLM_LTE_MC_CELL0);
              
              
              }
              
              /*Config REFLOG capture*/
              rfcommon_data_from_fw->fw_autopin_fn(&reflog_capture_input,
                                                   &reflog_proc_param,
                                                   &reflog_output,
                                                   ftm_common_autopin_reflog_callback);
            }
            else
            {
              FTM_MSG(FTM_ERROR, "ftm_common_autopin_capture: failed for LTE due to NULL DM pointer");
            }
            break;
       #endif

       #ifdef FEATURE_TDSCDMA
          case FTM_RF_TECH_TDSCDMA:

            /*Build CCS event*/
            tx_handle_id = (lm_handle_type)ftm_tdscdma_get_lm_idx();
            rf_tds_build_event_data.tx_device = tx_device;
            rf_tds_build_event_data.band = rf_tdscdma_mc_state.curr_band_tx;
            rf_tds_build_event_data.txlm_buffer_index = tx_handle_id;
            rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_AUTOPIN, &rf_tds_build_event_data);
            (void)txlm_hal_get_chain(tx_handle_id, &tx_chain);
            /*Hard code TQ0*/
            ccsDestPtr = rflm_ccs_get_data_buf_item_ptr((rflm_ccs_data_buf_item_type)0);

            rfcommon_msm_get_txc_rollover_period(tx_handle_id, 
                                                 &reflog_capture_input.txc_input_freq_hz, 
                                                 &reflog_capture_input.txc_rate_ns,
                                                 &reflog_capture_input.txrot_clk_hz);
            FTM_MSG_3(FTM_HIGH, "TXC param: input freq = %d, txc rate = %d, clk rate = %d", 
                      reflog_capture_input.txc_input_freq_hz, reflog_capture_input.txc_rate_ns, reflog_capture_input.txrot_clk_hz);

            rfcommon_msm_get_par_capture_params(RFLM_TECH_TDSCDMA,
                                                &(reflog_capture_input.capture_length),
                                                &(reflog_capture_input.txc_down_sampling_rate), 
                                                &(reflog_capture_input.tx_capture_delay),
                                                &wtr_capture_size);

            FTM_MSG_3(FTM_HIGH, "TXC param: REFLOG capture size = %d, downsample rate = %d, capture delay = %d", 
                      reflog_capture_input.capture_length, 
                      reflog_capture_input.txc_down_sampling_rate, 
                      reflog_capture_input.tx_capture_delay);
            
            reflog_capture_input.tx_chain = (uint32)tx_chain;
            reflog_capture_input.tx_iq_sample_ptr = rfcommon_autopin_reflog_sample;
            reflog_capture_input.ftm_mode = TRUE;
            /*FTM mode does not have gap*/
            reflog_capture_input.txc_rate_ns -= 12500;

            current_ustmr = HWIO_IN(STMR_TIME_RD);
            current_txc_sync = HWIO_INI(UNIV_STMR_TX_SYNC_TIMEn, reflog_capture_input.tx_chain);

            /*Calculate capture time = Current TXC sync time + current USTMR time + 2*TXC rollover period 
              This is FTM mode only. In online mode ref time = slot boundary or subframe boundary*/
            refTime = ((current_txc_sync + (current_ustmr&0xC00000))+
                      2*((uint32)(19200000/1000000.0*(reflog_capture_input.txc_rate_ns/1000.0))))&0xFFFFFF;

            reflog_capture_input.tx_capture_time = refTime;
            /*Adjust WTR capture time = ref time + capture delay -  CCS programming time 181us*/
            refTime += reflog_capture_input.tx_capture_delay; 
            if (wtr_debug_mem_enable)
            {
              if (param_ptr->fbrx_gain_state == 2)
              {
                refTime -= RF_AUTOPIN_CCS_WTR_PAR_G2_DELAY_DEBUG;
              }
              else
              {
                refTime -= RF_AUTOPIN_CCS_WTR_PAR_G1_DELAY_DEBUG;
              }
            }
            else
            {
              if (param_ptr->fbrx_gain_state == 2)
              {
                refTime -= RF_AUTOPIN_CCS_WTR_PAR_G2_DELAY;
              }
              else
              {
                refTime -= RF_AUTOPIN_CCS_WTR_PAR_G1_DELAY;
              }
            }

            /*Add time alignment*/
            refTime += AUTOPIN_PAR_TDSCDMA_CAPTURE_TIME_OFFSET;
         
			wtr_capture_time = refTime & 0x3FFFFF;
            reflog_start_time = reflog_capture_input.tx_capture_time + reflog_capture_input.tx_capture_delay;
            ccsDestPtr = rflm_tds_ccs_event_commit(rflmTdsRfModeCtlState.script.autopinWriteScriptPtrArray[param_ptr->fbrx_gain_state-1], 
                                                   ccsDestPtr,
                                                   ccsIntfPtr->ccsRfPairPtr.tq0_handle,
                                                   (refTime & 0x3FFFFF),
                                                   ftm_common_autopin_wtr_callback,
                                                   &taskInfo,
                                                   NULL,
                                                   NULL);

            /*Config REFLOG capture*/
            rfcommon_data_from_fw->fw_autopin_fn(&reflog_capture_input,
                                                 &reflog_proc_param,
                                                 &reflog_output,
                                                 ftm_common_autopin_reflog_callback);

            break;
       #endif

          default:
            FTM_MSG(FTM_ERROR, "ftm_common_autopin_capture: Unsupported Tech Type");
       }
       
       break;

     case FTM_AUTOPIN_PAR_FETCH_RESULTS:
       /*FET WTR PAR result*/
       capture_status &= rfdevice_cmn_fbrx_autopin_read_and_disable(device_ptr,
                                                                    &wtr_capture_result,
                                                                    wtr_debug_mem_enable,
                                                                    wtr_debug_mem);

       if (capture_status)
       {
         response_ptr->wtr_capture_response.sum_i_q_sqr = wtr_capture_result.sum_i_q_sqr;
         response_ptr->wtr_capture_response.sum_i_and_q_sqr_ave = wtr_capture_result.sum_i_and_q_sqr_ave;
         response_ptr->wtr_capture_response.p_ave = wtr_capture_result.p_ave;
         response_ptr->wtr_capture_response.sum_i = wtr_capture_result.sum_i;
         response_ptr->wtr_capture_response.sum_q = wtr_capture_result.sum_q;
         response_ptr->wtr_capture_response.i_dc = wtr_capture_result.i_dc;
         response_ptr->wtr_capture_response.q_dc = wtr_capture_result.q_dc;
         response_ptr->wtr_capture_response.fbrx_accum = wtr_capture_result.fbrx_accum;
         response_ptr->wtr_capture_response.sum_peaks = wtr_capture_result.sum_peaks;
         response_ptr->wtr_capture_response.num_peaks = wtr_capture_result.num_peaks;
         response_ptr->wtr_capture_response.par = wtr_capture_result.par;
         FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: PAR %d, Number of peaks %d, sum of peaks %d, Avg power %d", 
                    wtr_capture_result.par, 
                    wtr_capture_result.num_peaks,
                    wtr_capture_result.sum_peaks,
                    wtr_capture_result.p_ave );
         FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: sum i %d, sum q %d, i dc %d, q dc %d", 
                    wtr_capture_result.sum_i, 
                    wtr_capture_result.sum_q,
                    wtr_capture_result.i_dc,
                    wtr_capture_result.q_dc );
       }
       else
       {
         FTM_MSG(FTM_ERROR, "ftm_common_autopin_capture: Failed to fetch WTR PAR result");
       }

       response_ptr->reflog_capture_response.result.par = reflog_par_result;
       (void)rfcommon_autopin_msm_store_data(rfcommon_autopin_reflog_sample,wtr_debug_mem);

       response_ptr->status = capture_status;
       FTM_MSG_1( FTM_HIGH, "ftm_common_autopin_capture: REFLOG capture PAR is %d", reflog_par_result);
       FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: reflog sample[0] %d, [1] %d, [2] %d, [3] %d", 
                  rfcommon_autopin_reflog_sample[0], 
                  rfcommon_autopin_reflog_sample[1],
                  rfcommon_autopin_reflog_sample[2],
                  rfcommon_autopin_reflog_sample[3]);
       
       break;

     default:
       FTM_MSG_1( FTM_HIGH, "ftm_common_autopin_capture: Unsupported capture type %d", param_ptr->capture_type);

  }

  return rsp;
}


void ftm_common_autopin_wtr_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg)
{
  //Pending implementation
  FTM_MSG_2( FTM_HIGH, "ftm_common_autopin_capture: WTR PAR Capture start time is %d and CCS end time %d", 
             wtr_capture_time, 
             status.end_time);
  return;
}


void ftm_common_autopin_reflog_callback(fw_autopin_output_params_t *pout)
{
  reflog_par_result = pout->result.par;
  FTM_MSG_3( FTM_HIGH, "ftm_common_autopin_reflog_callback: REFLOG capture start time is %d and done time is %d, PAR is %d", 
             reflog_start_time,
             HWIO_IN(STMR_TIME_RD),
             reflog_par_result);
  return;
}


#endif /*FEATURE_FACTORY_TESTMODE */



