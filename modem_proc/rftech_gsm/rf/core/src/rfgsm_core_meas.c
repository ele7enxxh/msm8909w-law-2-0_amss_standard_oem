/*!
   @file
   rfgsm_core_meas.c

   @brief implements IRAT related functionality

   @details

*/

/*===========================================================================
Copyright (c) 2012 - 2016 by QUALCOMM Technologies Incorporated. 
All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_meas.c#1 $
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/13/16   ase     Crash if LPM->ON Rx wakeup or ON->LPM Rx sleep fail
03/28/16   par     Adding support for 1x2G measurement
12/11/15   par     Fix for X2G RSSI degradation on DUAL WTR RF card
11/25/15   bp      FR25072: for device wakeup, vote for lowest/first supported band
02/11/15   par     G2W abort support added
01/19/15   sml     Resume therm reads during G2T iRAT
11/07/14   tsr     Fix Alternate path bug in IRAT due to GSM 1800_B band support not available in RFC 
11/07/14   piy     Adding Circular Buffer to catch iRAT call flows
10/23/14   sc      Enable event type control from RFSW
10/17/14   rr      Separate rx/tx alternate path core handles
09/12/14   sml     Adding start API Pcycle to know about pre-emption done to RF task
08/30/14   sw      Correct comments and update preload offset to 2500 for X2G irat
08/29/14   tws     Mainline alternate_path support
08/28/14   sc      Clean up Tx data initialisation to reduce wakeup timeline
08/21/14   piy     Adding Additional Info for Debugging GSM Issues
08/11/14   sc      Ensure chain update with alternate path and include IRAT
08/03/14   jr      Suspend temp comp during IRAT time.
07/31/14   tsr     GSM Alternate path support
07/28/14   hk      Enable preconfig/rx disable for SSBI based devices
06/12/14   sw      Use GL1 api to map sub_id to as_id
05/15/14   sc      Use subscription ID passed from GL1 in meas info params
05/14/14   cj/ck   Add Do2G support
04/16/14   sb      IP2 cal Interface and functionality changes
03/26/14   sw      Disable tuners during X2G iRAT scenarios (port from DI3)
03/25/14   sc      Store device in RFLM GSM DM data table for TQ management
03/24/14   sc      Add missing RFLM update for Rx alpha interface
03/05/14   zbz     Fix KW errors
02/28/14   sc      Pass core data through mc to core layers
02/24/14   tsr     Moved RX timing update to core layer
02/20/14   cj      Updated Rxlm index
02/18/14   cj      Abort changes for Bolt
02/17/14   sc      Clean up mDSP APIs
02/10/14   cj      rfgsm_buffer flag added to decide whether to use home/away buffer
02/03/14   cj      T2G preload offset time changed
01/16/14   cj      DM policy check added
01/10/14   cj      rxlm index updated for cleanup script
01/08/14   cj      DM buffer allocation removed from meas APIs
01/02/14   gvn     temp hack to support enqueue policy check
12/27/13   rp      Code changes to replace get critical section pointer with get common device
                   pointer for device comparison.
12/27/13   cj      Debug info updated and rf buffer clear added for build script
12/26/13   hm      Script buffer creation with tech scaling factor, to fix expected vs observed delays in scripts
12/16/13   cj      Code cleanup
12/10/13   cj      Added code to save script creation time
12/09/13   cj      Added time profiling and code optimization
11/27/13   cj      Added gsm tech memory allocation in irat_info_get
11/14/13   cj      Added changes for building preload and trigger script
11/12/13   dps     Updates to support C++ class based GSM Rx and Tx interface
11/07/13   cj      Preload offset time updated in irat_info_get
10/26/13   shb     Updated IRAT APIs to support preload
09/27/13   hk      Disable RX will now be script based instead of direct write.
09/20/13   hk      Removing the SGLTE feature check since all L1s have implemented new header interface.
09/17/13   hm      hooking up WCDMA C++ device driver interface for WTR
09/16/13   zg      Added adaptor obj construction due to new C++ TDSCDMA interface
08/07/13   sml     Fixing KW warning
07/22/13   ec	   Use new F3s/Null pointer checks 
06/30/13   cj      Trigger type changed to dynamic for G2X
05/23/13   cj      Changed trigger type to dynamic for X2G
05/03/13   cj      Added return status update after device driver code update
04/30/13   cj      Shadow back up and restore moved to IRAT framework
04/30/13   cj      Added changes to reset CCS event status to idle for abort
04/24/13   cj      Added changes to disable rx on second WTR after target meas
04/24/13   cj      Added pre_config function to support IRAT framework changes
04/19/13   cj      Added band enum conversion to preconf required
04/07/13   sc      Assign core rxlm index var with param passed from GL1
03/26/13   jyu     Added the change to allow override trigger type
03/19/13   cj      Removed source device pointer comparison
03/19/13   cj      Added support for LTE CA and W DBDC to G IRAT
02/22/13   cj      Updated rfgsm_core_meas_preconf_required for T2G startup 
02/11/13   sc      Added GSM mode flag set to keep track of when in iRAT mode
01/22/13   cj      Change for ARAST-16830
01/18/13   cj      Updated rfgsm_core_meas_write_preconf_script function
01/17/13   cj      Added rfgsm_core_meas_write_preconf_script and 
                   rfgsm_core_meas_preconf_required APIs
01/10/13   sc      Add abort mechanism to enter and exit
01/09/13   sc      Allow RXLM settings to not be re-written
01/03/13   sc      Set flag to indicate iRAT mode to ensure therm read not executed
12/20/12   sc      Ensure G2T cleanup buffer retrieved from source params
12/12/12   sc      Accept NULL parameter for LtoG tech_specific_info_param
12/03/12   sc      Cleanup mDSP shared variable interface
12/05/12   sc      Changes to use source params for LTE G->L Cleanup
11/28/12   aka     Changes to use rfgsm_core_handle_ptr from path based handler
11/27/12   sc      Provide only cleanup_time for G2W cleanup case.
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/19/12   sc      RF-Only mDSP update shared variables cleanup
11/12/12   sc      Handle bad return from common RF CCS iRAT event creation
11/12/12   pl      remove rf_buffer_clear()
11/07/12   sc      Use common info params type to clean up interface
11/07/12   sc      Calculate the iRAT timings instead of explicitly defining
11/07/12   sc      Added Null poiter checks and assignment to l2g_irat_info from irat_info_param.
10/30/12   sc      Remove use of rfmeas_tech_params_type for W2G startup and G2W cleanup
10/09/12   sr      Added rfdevice and mdsp interface API calls.
10/04/12   sr      Initial version.

===========================================================================*/


#include "rfa_variation.h"
#include "rfcom.h"
#include "msg.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_gsm_intf.h"
#include "rfgsm_core_types.h"
#include "rfgsm_msm.h"
#include "rfcommon_core_types.h"
#include "rfc_card_gsm.h"
#include "rfgsm_core.h"
#include "rfgsm_core_temp_comp.h"
#include "rfgsm_core_util.h"
#include "rfgsm_core_meas.h"
#include "rf_hal_buffer.h"
#include "rfmeas_types.h"
#include "rfmeas_mdsp.h"
#include "rfcommon_nv_mm.h"

#include "rfm_internal.h"
#include "rfdevice_trx_gsm_rx.h"
#include "rfdevice_trx_wcdma_rx.h"
#include "rfgsm_data_manager.h"
#include "rflm_dm_api.h"
#include "rflm_meas.h"
#include "gl1_hw.h"
#include "geran_test_mode_api.h"

#ifdef FEATURE_WCDMA
#include "rfdevice_wcdma_type_defs.h"
extern void** rfc_wcdma_get_device
( 
  rfm_device_enum_type rfm_dev, 
  rfc_rxtx_enum_type rx_tx,
  rfcom_wcdma_band_type band, 
  rfdevice_type_enum_type dev_type
);
#endif

#ifdef FEATURE_LTE
#include "rfdevice_lte_type_defs.h"
#include "rfdevice_lte_interface.h"
extern void** rfc_lte_get_device
(
   rfm_device_enum_type rfm_dev, 
   rfc_rxtx_enum_type rx_tx,
   rfcom_lte_band_type band, 
   rfdevice_type_enum_type dev_type
);
#endif /* #ifdef FEATURE_LTE */


#ifdef FEATURE_TDSCDMA
#include "rfdevice_tdscdma_type_defs.h"
#include "rfdevice_trx_tdscdma_rx.h"
extern void** rfc_tdscdma_get_rf_device_obj
( 
  rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
  rfcom_tdscdma_band_type band, rfdevice_type_enum_type dev_type
);
#include "rfdevice_tdscdma_intf.h"
#endif

#define RFGSM_CORE_IRAT_BUFFER_SIZE 500

typedef struct
{
  rfcom_mode_enum_type src_tech;
  rfcom_mode_enum_type target_tech;
  unsigned long long pcycle;
  rfgsm_core_script_type script_type;
}rfgsm_core_irat_logging;

rfgsm_core_irat_logging rfgsm_core_irat_buffer[RFGSM_CORE_IRAT_BUFFER_SIZE];
uint32 rfgsm_core_irat_buffer_index = 0;

#define RFGSM_CORE_IRAT_Q(s , t, p, e)                                                            \
{                                                                                                 \
  rfgsm_core_irat_buffer_index = rfgsm_core_irat_buffer_index % RFGSM_CORE_IRAT_BUFFER_SIZE;      \
  rfgsm_core_irat_buffer[rfgsm_core_irat_buffer_index].src_tech= s;                               \
  rfgsm_core_irat_buffer[rfgsm_core_irat_buffer_index].target_tech= t;                            \
  rfgsm_core_irat_buffer[rfgsm_core_irat_buffer_index].pcycle= p;                                 \
  rfgsm_core_irat_buffer[rfgsm_core_irat_buffer_index].script_type = e;                           \
  rfgsm_core_irat_buffer_index ++;                                                                \
} 

typedef struct
{
  rf_time_tick_type build_script_start_t; 
  rf_time_type build_script_time;
  rf_time_tick_type sw_init_start_t; 
  rf_time_type sw_init_time;
  rf_time_tick_type transceiver_cleanup_start_t; 
  rf_time_type transceiver_cleanup_total_time;
  rf_time_tick_type transceiver_setup_start_t; 
  rf_time_type transceiver_setup_total_time;
  rf_time_tick_type transceiver_script_start_t; 
  rf_time_type transceiver_script_time;
  rf_time_tick_type msm_script_start_t; 
  rf_time_type msm_script_time;  
  rf_time_tick_type mdsp_script_start_t;
  rf_time_type mdsp_script_time;
  	
} rfgsm_meas_build_script_profile_type;

static rfgsm_meas_build_script_profile_type gsm_build_script_profiling;

static void gsm_meas_clear_profiling_data(void);

#define RFGSM_DISABLE_RX_SSBI_SCRIPT_SIZE 2
rfm_meas_result_type rfgsm_core_meas_write_preconf_script( rfm_device_enum_type gsm_device );
rfm_meas_result_type rfgsm_core_meas_preconf_required( rfm_meas_header_type gsm_meas_param,
                                                     boolean* preconf_required );


static void gsm_meas_clear_profiling_data()
{
  memset(&gsm_build_script_profiling,0,sizeof(rfgsm_meas_build_script_profile_type));
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function will populate all the timing information related to GSM IRAT measurements. This
    timing information will be used by L1 scheduling the measurements during IRAT gap.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_core_irat_info_get(rfm_meas_irat_info_type *irat_info_param)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfc_gsm_core_config_type *rfc_gsm_core_ptr = NULL;
  rfm_meas_xtoy_irat_info_param_type * x2y_irat_info = NULL;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;

  if (irat_info_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Pointer irat_info_param is NULL!", 0);
    return result;
  }

  src_tech = irat_info_param->header.source_tech;
  target_tech = irat_info_param->header.target_tech;

  x2y_irat_info = (rfm_meas_xtoy_irat_info_param_type *)irat_info_param->irat_info_param;
  if( x2y_irat_info == NULL )
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Pointer irat_info_param->irat_info_param is NULL!", 0);
    return result;
  }

  if(target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = irat_info_param->header.target_param.device;
  }
  else if(src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = irat_info_param->header.source_param[0].device;
	
	/* Preload offset time is initialized to invalid value for G2X */
    irat_info_param->nbr_pretune_offset_time = 0; 
    irat_info_param->home_pretune_offset_time = 0;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",
			src_tech,
			target_tech);
	result = RFM_MEAS_COMMON_FAILURE;
	return result;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return result;
  }

  rfc_gsm_core_ptr = rfgsm_core_handle_ptr->rfc_gsm_core_ptr;
  if( rfc_gsm_core_ptr == NULL )
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"gsm rfc_ptr is NULL! Rx PLL Tune Time could not be acquired", 0); 
    return result;
  }


  /*********************** X2G Startup and PowerMeas *************************/
  // W2G
  if (src_tech == RFCOM_WCDMA_MODE && target_tech == RFCOM_GSM_MODE)
  {
    /* startup max sbi: 38 SBIs = 99QS = 91us */
    x2y_irat_info->start_duration = RFGSM_STARTUP_TIME_CALC_QS(WTOG_STARTUP_SBI_MAX);
    irat_info_param->startup_time = x2y_irat_info->start_duration;

    /*G2G SBI+ PLL = 99QS+163QS (assuming 150us PLL) = 262QS = 242us*/
    x2y_irat_info->power_meas_setup_duration = RFGSM_PWR_MEAS_TIME_CALC_QS(XTOG_PWR_MEAS_SBI_MAX,
                                                                           rfc_gsm_core_ptr->rx_timing->pll_settling_time_qs);
    /* Time when gsm_preload_script should be preloaded to WXE */
    /* W2G max preload RFFE: 480 us, for DSDS 480*2 + overhead = 2500 us */
    irat_info_param->nbr_pretune_offset_time = RFGSM_PRELOAD_OFFSET_TIME_CALC_QS(2500); 

    irat_info_param->home_pretune_offset_time = 0;
  }
  // L2G
  else if (src_tech == RFCOM_LTE_MODE && target_tech == RFCOM_GSM_MODE)
  {
    /* startup max sbi: 150 SBIs = 390QS = 360us */
    x2y_irat_info->start_duration = RFGSM_STARTUP_TIME_CALC_QS(LTOG_STARTUP_SBI_MAX);
    irat_info_param->startup_time = x2y_irat_info->start_duration;

    /*G2G SBI+ PLL = 99QS+163QS (assuming 150us PLL) = 262QS = 242us*/
    x2y_irat_info->power_meas_setup_duration = RFGSM_PWR_MEAS_TIME_CALC_QS(XTOG_PWR_MEAS_SBI_MAX,
                                                                           rfc_gsm_core_ptr->rx_timing->pll_settling_time_qs);
    /* Time when gsm_preload_script should be preloaded to WXE */
    /* L2G max preload RFFE: 480 us, for DSDS 480*2 + overhead = 2500 us */
    irat_info_param->nbr_pretune_offset_time = RFGSM_PRELOAD_OFFSET_TIME_CALC_QS(2500); 

    irat_info_param->home_pretune_offset_time = 0;
  }
  //Do2G

  else if (((src_tech == RFCOM_1XEVDO_MODE) || (src_tech == RFCOM_1X_MODE)) && target_tech == RFCOM_GSM_MODE)
  {
    /* startup max sbi: 150 SBIs = 390QS = 360us */
    x2y_irat_info->start_duration = RFGSM_STARTUP_TIME_CALC_QS(DOTOG_STARTUP_SBI_MAX);
    irat_info_param->startup_time = x2y_irat_info->start_duration;

    /*G2G SBI+ PLL = 99QS+163QS (assuming 150us PLL) = 262QS = 242us*/
    x2y_irat_info->power_meas_setup_duration = RFGSM_PWR_MEAS_TIME_CALC_QS(XTOG_PWR_MEAS_SBI_MAX,
                                                                           rfc_gsm_core_ptr->rx_timing->pll_settling_time_qs);
    /* Time when gsm_preload_script should be preloaded to WXE */
    /* Do2G max preload RFFE: 480 us, for DSDS 480*2 + overhead = 2500 us */
    irat_info_param->nbr_pretune_offset_time = RFGSM_PRELOAD_OFFSET_TIME_CALC_QS(2500); 

    irat_info_param->home_pretune_offset_time = 0;
  }
  // T2G
  else if (src_tech == RFCOM_TDSCDMA_MODE && target_tech == RFCOM_GSM_MODE)
  {
    /* startup max sbi: 65 SBIs = 177QS = 163us */
    x2y_irat_info->start_duration = RFGSM_STARTUP_TIME_CALC_QS(TTOG_STARTUP_SBI_MAX);
    irat_info_param->startup_time = x2y_irat_info->start_duration;

    /*G2G SBI+ PLL = 99QS+163QS (assuming 150us PLL) = 262QS = 242us*/
    x2y_irat_info->power_meas_setup_duration = RFGSM_PWR_MEAS_TIME_CALC_QS(XTOG_PWR_MEAS_SBI_MAX,
                                                                           rfc_gsm_core_ptr->rx_timing->pll_settling_time_qs);
    /* Time when gsm_preload_script should be preloaded to WXE */
    /* T2G preload will be handled by RF */
    irat_info_param->nbr_pretune_offset_time = RFGSM_PRELOAD_OFFSET_TIME_CALC_QS(2500); 

    irat_info_param->home_pretune_offset_time = 0;
  }

  /***************************** G2X Cleanup *********************************/
  // G2W
  else if (src_tech == RFCOM_GSM_MODE && target_tech == RFCOM_WCDMA_MODE)
  {
    /*38 sbis for cleanup to go back to GSM mode in qs*/
    irat_info_param->cleanup_time = RFGSM_CLEANUP_TIME_CALC_QS(WTOG_CLEANUP_SBI_MAX);
  }
  // G2L
  else if (src_tech == RFCOM_GSM_MODE && target_tech == RFCOM_LTE_MODE)
  {
    /*150 sbis for cleanup to go back to GSM mode in qs*/
    x2y_irat_info->cleanup_duration = RFGSM_CLEANUP_TIME_CALC_QS(LTOG_CLEANUP_SBI_MAX);
    irat_info_param->cleanup_time = x2y_irat_info->cleanup_duration;
  }
  // G2T
  else if (src_tech == RFCOM_GSM_MODE && target_tech == RFCOM_TDSCDMA_MODE)
  {
    /*65 sbis for cleanup to go back to GSM mode in qs*/
    x2y_irat_info->cleanup_duration = RFGSM_CLEANUP_TIME_CALC_QS(TTOG_CLEANUP_SBI_MAX);
    irat_info_param->cleanup_time = x2y_irat_info->cleanup_duration;
  }

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function will populate all the Rx Alpha timing information. This
    timing information will be used by FW scheduling the measurements during iRAT gap.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_core_irat_update_rx_timings(rfm_meas_irat_info_type *irat_info_param)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rfgsm_core_data_type rfgsm_core_data;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if (irat_info_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Pointer irat_info_param is NULL!", 0);
    return result;
  }

  if (irat_info_param->tech_specific_info_param == NULL)
  {
    if ( irat_info_param->header.source_tech == RFCOM_LTE_MODE )
    {
      return result;
    }
    else
    {
      result = RFM_MEAS_COMMON_FAILURE;
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Pointer tech_specific_info_param is NULL!", 0);
      return result;
    }
  }
  if(irat_info_param->header.target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = irat_info_param->header.target_param.device;
  }
  else if(irat_info_param->header.source_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = irat_info_param->header.source_param[0].device;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",
			irat_info_param->header.source_tech,
			irat_info_param->header.target_tech);
	result = RFM_MEAS_COMMON_FAILURE;
	return result;
  }

  /* Get the core handle ptr */
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if( rfgsm_core_handle_ptr == NULL )
  {
    RF_MSG_3(MSG_LEGACY_ERROR, 
             "NULL core handle ptr for source_tech %d, target tech %d, dev %d", 
             irat_info_param->header.source_tech, irat_info_param->header.target_tech, rfm_dev);
    result = RFM_MEAS_COMMON_FAILURE;
	  return result;
  }

  if(irat_info_param->sub_id >= 2)
  {
    RF_MSG_4(MSG_LEGACY_ERROR, 
             "Invalid sub ID %d for source_tech %d, target tech %d, dev %d", 
             irat_info_param->sub_id, irat_info_param->header.source_tech, irat_info_param->header.target_tech, rfm_dev);
    result = RFM_MEAS_COMMON_FAILURE;
    return result;
  }

  /* Set the core data */
  rfgsm_core_data.rf_task = RF_MEAS_CALL;
  rfgsm_core_data.rfm_dev = rfm_dev;
  rfgsm_core_data.sub_id = irat_info_param->sub_id; //Default until support available if required
  rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(irat_info_param->sub_id);
  rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

  /* update the timing related shared variables */
  rfgsm_core_update_rx_timing(&rfgsm_core_data);
  if( !rfgsm_core_dm_populate_enter_mode_intf_data(&rfgsm_core_data, 
                                                   irat_info_param->tech_specific_info_param ) )
  {
    RF_MSG_3(MSG_LEGACY_ERROR, 
             "Populate enter mode intf data for IRAT failed source_tech %d, target tech %d, dev %d", 
             irat_info_param->header.source_tech, irat_info_param->header.target_tech, rfm_dev);
    result = RFM_MEAS_COMMON_FAILURE;
    return result;
  }

  return(result);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called first when the measurement is requested. Each technology
    basically prepares for the measurement.
 
  @param rfm_meas_enter_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_core_meas_enter( rfm_meas_enter_param_type *meas_enter_param)
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  boolean is_rf_fr_25072_enabled = rfcommon_nv_get_lpm_feature_enabled();

  if (meas_enter_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"meas_enter_param is NULL!", 0);
    return result;
  }

 if(meas_enter_param->header.target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_enter_param->header.target_param.device;
  }
  else if(meas_enter_param->header.source_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_enter_param->header.source_param[0].device;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",
			meas_enter_param->header.source_tech,
			meas_enter_param->header.target_tech);
	result = RFM_MEAS_COMMON_FAILURE;
	return result;
  }
 
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return result;
  }

  if(meas_enter_param->header.target_tech == RFCOM_GSM_MODE)
  {
    rfgsm_core_handle_ptr->rfgsm_buffer = RFGSM_AWAY_BUFFER;
  }
  else
  {
	rfgsm_core_handle_ptr->rfgsm_buffer = RFGSM_HOME_BUFFER;
  }
  /* Set to NOT GSM Standalone mode */
  /* Ensures that temp comp is not done during iRAT */
  rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_IRAT_MODE;

    /************************************/
    /* New power saving API (LPM -> ON) */
    /************************************/
  if( TRUE == is_rf_fr_25072_enabled)
  {
    /* For X2G, must make sure device is powered on for G */
  if (meas_enter_param->header.target_tech == RFCOM_GSM_MODE)
  {
    /* Enable RX Device (LPM -> ON) for lowest supported band */
    if ( !rfdevice_gsm_rx_pwr_wakeup( rfm_dev, rfgsm_nv_get_lowest_supported_band(rfm_dev) ) )
    {
      ERR_FATAL("rfgsm_core_meas_enter: RX Device %d Pwr Up (LPM->ON) failed for band %d",
                rfm_dev, rfgsm_nv_get_lowest_supported_band(rfm_dev),0);
      return FALSE;
    }
  }
  }
  
  /* Suspend thermistor reads on this device */
  if (rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started )
  {
    rfgsm_core_temp_comp_suspend(rfm_dev);
  }

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to prepare the clean-up and start-up scripts.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_core_meas_build_scripts( rfm_meas_setup_param_type *meas_scripts_param)
{
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfm_meas_params_channel_type *channel_params = NULL;
  rfdevice_gsm_meas_param_type gsm_device_param;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rf_buffer_intf *rf_buffer = NULL;
  rf_buffer_intf *rf_preload_buffer = NULL, *rf_trigger_buffer = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfdevice_meas_script_data_type meas_script_data;
  boolean return_bool = FALSE;
  boolean create_new_script = FALSE;
  boolean display_profiling = FALSE;
  uint16 temp_preload_trans = 0, temp_trx_trans = 0;
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint8 rfc_rx_supported_band_mask = 0;

#ifdef G_POLICY_CHECK_EN
  uint16 buf_mask = 0;
  rfmeas_mdsp_policy_check_params_s policy_check;

  rflm_meas_resource_entry_s override_params = { 0 };
  uint32 override_bitmask = 0, override_err_code = 0;
#endif

  gsm_meas_clear_profiling_data();
  gsm_build_script_profiling.build_script_start_t = rf_time_get_tick();

  if (meas_scripts_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"meas_scripts_param is NULL!", 0);
    return result;
  }

  src_tech = meas_scripts_param->header.source_tech;
  target_tech = meas_scripts_param->header.target_tech;

  if(target_tech == RFCOM_LTE_MODE ||
	  target_tech == RFCOM_TDSCDMA_MODE ||
	  target_tech == RFCOM_WCDMA_MODE )
  {
    channel_params = (rfm_meas_params_channel_type *)meas_scripts_param->source_param;
  }
  else
  {
    channel_params = (rfm_meas_params_channel_type *)meas_scripts_param->target_param;
  }

  if (channel_params == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"channel_params is NULL!", 0);
    return result;
  }

  if(target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.target_param.device;
  }
  else if(src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.source_param[0].device;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",
          src_tech,
          target_tech);
    result = RFM_MEAS_COMMON_FAILURE;
    return result;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return result;
  }

  gsm_device_param.source_tech = src_tech;
  gsm_device_param.source_band = meas_scripts_param->header.source_param[0].band;
  gsm_device_param.target_tech = target_tech;
  gsm_device_param.target_band = meas_scripts_param->header.target_param.band;

   // W2G, L2G and T2G StartUp Script
  if ( target_tech == RFCOM_GSM_MODE && meas_scripts_param->script_type == RFM_MEAS_PROGRAM_STARTUP) 
  {
    RFGSM_CORE_IRAT_Q(src_tech, target_tech, start_pcycle, RFGSM_CORE_IRAT_STARTUP);
	
    MSG_5(MSG_SSID_RF, MSG_LEGACY_ERROR, "src_tech %d, tgt_tech %d, src_band %d, tgt band %d, rxlm_id %d",
          src_tech, target_tech, gsm_device_param.source_band,
          gsm_device_param.target_band, channel_params->neighbor_rxlm_buf_idx);

    /* Optmized call flow for building script */
    //rf_buffer = (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.meas_nontrx_script;
    //rf_trigger_buffer = (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.meas_trigger_script;
    rf_preload_buffer = (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.enter_mode_script;

    rf_buffer_clear(rf_preload_buffer);

    // get the rf_buffer to use in building the scripts based on script type.

    rf_buffer = rfmeas_mdsp_buffer_get(RFM_MEAS_PROGRAM_STARTUP);
    // Use common iRAT buffers for script population
    rf_trigger_buffer = rfmeas_mdsp_buffer_get(RFM_MEAS_PROGRAM_TRIGGER);

    meas_script_data.preload_action = RFDEVICE_MEAS_CREATE_SCRIPT;
    meas_script_data.preload_buf = rf_preload_buffer;
    meas_script_data.meas_action = RFDEVICE_MEAS_CREATE_SCRIPT;
    meas_script_data.meas_buf = rf_trigger_buffer;

    create_new_script = TRUE;
    temp_preload_trans = rf_buffer_get_num_trans(meas_script_data.preload_buf, RF_BUFFER_RFFE);
    temp_trx_trans = rf_buffer_get_num_trans(meas_script_data.meas_buf, RF_BUFFER_RFFE);

    /* optimized call flow for building rf script */
    if ((temp_preload_trans > 0)&&(temp_trx_trans > 0))
    { 

      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Preload %d & Trigger %d script is already created!", 
            temp_preload_trans,temp_trx_trans);

      create_new_script = TRUE;	
    }

    gsm_build_script_profiling.transceiver_script_start_t = rf_time_get_tick();

    if(create_new_script)
    {
      /* setup rf device to make GSM measurements */
      return_bool = rfdevice_gsm_build_meas_script(rfm_dev, gsm_device_param, &meas_script_data);
      if( return_bool == FALSE )
      {
        result = RFM_MEAS_COMMON_FAILURE;
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error in wtr script creation!", 0);
        return result;
      }
    } // ^ create_new_script

    gsm_build_script_profiling.transceiver_script_time = rf_time_get_elapsed(gsm_build_script_profiling.transceiver_script_start_t,RF_USEC);	
    gsm_build_script_profiling.msm_script_start_t = rf_time_get_tick();

    /* Retrieve the rfc rfgsm band supports for RX. GSM Band support is retrieved from RFC during gsm initialisation and stored in rfgsm_core_handles for each RFM device*/
    rfc_rx_supported_band_mask = rfgsm_core_handle_ptr->device_capability.rx_supported_band_mask;

    /*Update the alt rx path settings to rfc. This api updates rfgsm_core_handle_ptr->alternate_path*/
    if(!rfgsm_core_update_irat_alternate_rx_path_port_mapping(rfm_dev, 
                                                              &rfgsm_core_handle_ptr->rx_alternate_path,
                                                              rfc_rx_supported_band_mask))
    {
      RF_MSG_2(MSG_LEGACY_ERROR, 
               "rfgsm_core_meas_build_scripts, alternate path update for dev:%d! failed for band: %d",
               rfm_dev,
               gsm_device_param.target_band);
    }
    else
    {
      RF_MSG_3(MSG_LEGACY_HIGH, 
               "rfgsm_core_meas_build_scripts, alternate path returned:%d for dev:%d! and band: %d",
               rfgsm_core_handle_ptr->rx_alternate_path,
               rfm_dev,
               gsm_device_param.target_band);
    }

    /* Setup RFC (rf-front-end devices) to make GSM measurements  */
    rfc_gsm_mode_config(rfm_dev, rf_buffer, rfgsm_core_handle_ptr->rx_alternate_path);
   
    /* Only program the RXLM buffer if L1 has not set the neighbor LM idx to invalid */
    if( channel_params->neighbor_rxlm_buf_idx != RXLM_INVALID_BUFFER )
    {
      /* Set the core handle var for Rx meas */
      rfgsm_core_handle_ptr->rxlm_buf_index = (uint32)(channel_params->neighbor_rxlm_buf_idx);

      /* setup modem baseband to make GSM measurements */
      rfgsm_msm_init( (uint32)(channel_params->neighbor_rxlm_buf_idx), rfm_dev, 
                      FALSE, rfgsm_core_handle_ptr->rx_alternate_path );
    }
    gsm_build_script_profiling.msm_script_time = rf_time_get_elapsed(gsm_build_script_profiling.msm_script_start_t,RF_USEC);

    /* update the FW shared mem with rf_device and RFC settings */
    gsm_build_script_profiling.mdsp_script_start_t = rf_time_get_tick();

#ifdef G_POLICY_CHECK_EN
    policy_check.buff_mask = meas_script_data.buffer_mask;
    policy_check.channel = 1;
    policy_check.rf_band = 1;

    return_bool = rfmeas_mdsp_meas_write_ccs_irat_script_v2( channel_params->neighbor_rxlm_buf_idx,
                                                             RFM_MEAS_PROGRAM_STARTUP,
                                                             meas_script_data.preload_buf,
                                                             RFLM_MEAS_DM_PRELOAD_SCRIPT,
                                                             meas_script_data.meas_buf,
                                                             RFLM_MEAS_DM_TRIGGER_SCRIPT,
                                                             rf_buffer,
                                                             RFLM_MEAS_DM_NONTRX_SCRIPT,
                                                             FALSE /* combine_trigger_non_trx */,
                                                             &policy_check );
#else

    return_bool = rfmeas_mdsp_meas_write_ccs_irat_script_v2( channel_params->neighbor_rxlm_buf_idx,
                                                             RFM_MEAS_PROGRAM_STARTUP,
                                                             meas_script_data.preload_buf,
                                                             RFLM_MEAS_DM_PRELOAD_SCRIPT,
                                                             meas_script_data.meas_buf,
                                                             RFLM_MEAS_DM_TRIGGER_SCRIPT,
                                                             rf_buffer,
                                                             RFLM_MEAS_DM_NONTRX_SCRIPT,
                                                             FALSE /* combine_trigger_non_trx */,
                                                             NULL );
#endif

    if( return_bool == FALSE )
    {
      result = RFM_MEAS_COMMON_FAILURE;
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error in GSM startup iRAT event creation!", 0);
      return result;
    }

    gsm_build_script_profiling.mdsp_script_time = rf_time_get_elapsed(gsm_build_script_profiling.mdsp_script_start_t,RF_USEC);
  } // ^ target_tech == RFCOM_GSM_MODE && meas_scripts_param->script_type == RFM_MEAS_PROGRAM_STARTUP
  // CleanUp script
  else if (src_tech == RFCOM_GSM_MODE)
  {
    RFGSM_CORE_IRAT_Q(src_tech, target_tech, start_pcycle, RFGSM_CORE_IRAT_CLEANUP);
	
    MSG_5( MSG_SSID_RF, MSG_LEGACY_ERROR, "src_tech %d, tgt_tech %d, src_band %d, tgt band %d, rxlm_id %d",
           src_tech, target_tech, gsm_device_param.source_band,
           gsm_device_param.target_band, channel_params->serving_rxlm_buf_idx);

    // get the rf_buffer to use in building the scripts based on script type.
    rf_buffer  = rfmeas_mdsp_buffer_get(RFM_MEAS_PROGRAM_CLEANUP);
    //rf_buffer = (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.meas_nontrx_script;

    rf_trigger_buffer = (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.meas_trigger_script;
    rf_buffer_clear(rf_trigger_buffer);

    /* go back to band and channel which was set before the measurement gap */
    gsm_device_param.source_band = rfgsm_core_handle_ptr->band;
    gsm_device_param.arfcn = rfgsm_core_handle_ptr->rf_rx_arfcn;

    meas_script_data.preload_action = RFDEVICE_MEAS_SKIP_EXECUTION;
    meas_script_data.preload_buf = NULL;
    meas_script_data.meas_action = RFDEVICE_MEAS_CREATE_SCRIPT;
    meas_script_data.meas_buf = (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.meas_trigger_script;

    gsm_build_script_profiling.transceiver_script_start_t = rf_time_get_tick();

    /* setup rf device to GSM after making X tech measurements */
    rfdevice_gsm_build_meas_script(rfm_dev, gsm_device_param,
                                   &meas_script_data);

    gsm_build_script_profiling.transceiver_script_time = rf_time_get_elapsed(gsm_build_script_profiling.transceiver_script_start_t,RF_USEC);	
    gsm_build_script_profiling.msm_script_start_t = rf_time_get_tick();

    /* setup RFC(rf-front-end devices) to GSM after making X tech measurements */
    rfc_gsm_mode_config(rfm_dev, rf_buffer, rfgsm_core_handle_ptr->rx_alternate_path);

    gsm_build_script_profiling.msm_script_time = rf_time_get_elapsed(gsm_build_script_profiling.msm_script_start_t,RF_USEC);

#ifdef G_POLICY_CHECK_EN
    override_params.pending_preload_cnt = 0;
    override_params.pending_trigger_cnt = 0;
    override_params.preload_seq_id = RFMEAS_MDSP_GET_EVT_SEQ_ID(RFLM_USR_GSM, 
                                                                    1, 
                                                                    1, 
                                                                    1);
    override_params.priority = 9;
    override_params.state = RFLM_MEAS_RES_STATE_PRELOAD_DONE;
    override_bitmask = buf_mask;
    override_err_code = rflm_meas_overwrite_resource_state(override_bitmask, override_params);

    if (override_err_code != 0)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Non zero error returned "
                                            "from overwrite_resource_state: error code %d", 
                                            override_err_code);
    }
#endif // G_POLICY_CHECK_EN
    /* update the FW shared mem with rf_device and RFC settings */
    gsm_build_script_profiling.mdsp_script_start_t = rf_time_get_tick();

    temp_trx_trans = rf_buffer_get_num_trans(meas_script_data.meas_buf, RF_BUFFER_RFFE);

    /* update the FW shared mem with rf_device and RFC settings */
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Trigger %d script is already created!",temp_trx_trans);

#ifdef G_POLICY_CHECK_EN
    policy_check.buff_mask = meas_script_data.buffer_mask;
    policy_check.channel = 1;
    policy_check.rf_band = 1;

    return_bool = rfmeas_mdsp_meas_write_ccs_irat_script_v2( channel_params->serving_rxlm_buf_idx,
                                                             RFM_MEAS_PROGRAM_CLEANUP,
                                                             meas_script_data.preload_buf,
                                                             RFLM_MEAS_DM_PRELOAD_SCRIPT,
                                                             meas_script_data.meas_buf,
                                                             RFLM_MEAS_DM_TRIGGER_SCRIPT,
                                                             rf_buffer,
                                                             RFLM_MEAS_DM_NONTRX_SCRIPT,
                                                             FALSE /* combine_trigger_non_trx */,
                                                             &policy_check );
#else // ^ G_POLICY_CHECK_EN
    return_bool = rfmeas_mdsp_meas_write_ccs_irat_script_v2( channel_params->serving_rxlm_buf_idx,
                                                             RFM_MEAS_PROGRAM_CLEANUP,
                                                             meas_script_data.preload_buf,
                                                             RFLM_MEAS_DM_PRELOAD_SCRIPT,
                                                             meas_script_data.meas_buf,
                                                             RFLM_MEAS_DM_TRIGGER_SCRIPT,
                                                             rf_buffer,
                                                             RFLM_MEAS_DM_NONTRX_SCRIPT,
                                                             FALSE /* combine_trigger_non_trx */,
                                                             NULL );
#endif // ^ !G_POLICY_CHECK_EN
    gsm_build_script_profiling.mdsp_script_time = rf_time_get_elapsed(gsm_build_script_profiling.mdsp_script_start_t,RF_USEC);

    if( !return_bool )
    {
      result = RFM_MEAS_COMMON_FAILURE;
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error in GSM cleanup iRAT event creation!", 0);
      return result;
    }

    /* no need to configure Modem baseband and fw variables as those settings 
       are already present in shared memory and it will be configured by FW */
  }
  else
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"incorrect parameters: src_tech = %d, target_tech = %d, script_type=%d", 
          src_tech, target_tech, meas_scripts_param->script_type);
    return result;
  }
  gsm_build_script_profiling.build_script_time = rf_time_get_elapsed(gsm_build_script_profiling.build_script_start_t,RF_USEC);

  if(display_profiling == TRUE)
  {
    MSG_6(MSG_SSID_RF, MSG_LEGACY_HIGH, 
          "GSM Build start/stop(1,0) %d, build_time %d us, sw_init %d, transceriver %d, asm %d, tuner %d, msm %d, mdsp %d", 
          0, 
          gsm_build_script_profiling.build_script_time, 
          gsm_build_script_profiling.sw_init_time, 
          gsm_build_script_profiling.transceiver_script_time, 
          gsm_build_script_profiling.msm_script_time,
          gsm_build_script_profiling.mdsp_script_time );

    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, 
          "LTE Build start/stop(1,0) %d, transceiver_setup_total_time %d", 
          0, 
          gsm_build_script_profiling.transceiver_setup_total_time);
  } // ^ display_profiling == TRUE

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to exit the measurement mode. It is used to put the RF
    in the right state and do any clean-ups required.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_meas_common_script_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfgsm_core_meas_exit( rfm_meas_exit_param_type *meas_exit_param)
{

  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;  
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfm_meas_params_channel_type *channel_params = NULL;
  rfm_meas_program_script_type script_type = RFM_MEAS_PROGRAM_STARTUP;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  boolean status = TRUE;
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  boolean is_rf_fr_25072_enabled = rfcommon_nv_get_lpm_feature_enabled();

  if (meas_exit_param == NULL)
  {
	result = RFM_MEAS_COMMON_FAILURE;
	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"meas_exit_param is NULL!", 0);
	return result;
  }

  src_tech = meas_exit_param->header.source_tech;
  target_tech = meas_exit_param->header.target_tech;

  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_exit_param->header.target_param.device;
  }
  else if ( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_exit_param->header.source_param[0].device;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",
			src_tech,
			target_tech);
	result = RFM_MEAS_COMMON_FAILURE;
	return result;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return result;
  }

  rfgsm_core_handle_ptr->rfgsm_buffer = RFGSM_HOME_BUFFER;
    
  /* Set GSM mode flag to Default mode as not in iRAT or Standalone */
  rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_DEFAULT_MODE;

  if((target_tech == RFCOM_LTE_MODE) || 
	  (target_tech == RFCOM_TDSCDMA_MODE) ||
	  (target_tech == RFCOM_WCDMA_MODE))
  {
    channel_params = (rfm_meas_params_channel_type *)meas_exit_param->source_param;
  }
  else
  {
    channel_params = (rfm_meas_params_channel_type *)meas_exit_param->target_param;
  }

  if (channel_params == NULL)
  {
    /* To do : result should be changed to failure once GL1 support is available 
	  to provide startup/cleanup buffer info for abort */
    result = RFM_MEAS_COMMON_SUCCESS;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"channel_params is NULL!, CCS Event status cannot be changed to idle", 0);
    /* Resume therm reads */
    if( rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started )
    {
      /* Enable therm reads */
      rfgsm_core_temp_comp_resume(rfm_dev);
    }
    return result;
  }


  /* Revert device shadow due to gap being aborted if flag is set */

  if (meas_exit_param->is_measurement_gap_aborted)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_LOW,"Measurement gap is aborted");	    

    RFGSM_CORE_IRAT_Q(src_tech, target_tech, start_pcycle, RFGSM_CORE_IRAT_ABORT);

    if (target_tech == RFCOM_GSM_MODE)		
    {
      script_type = RFM_MEAS_PROGRAM_STARTUP;
	  
	  status = rfmeas_mdsp_meas_abort_script(channel_params->neighbor_rxlm_buf_idx, RFLM_MEAS_DM_PRELOAD_SCRIPT);
	  if( status == FALSE)
	  {
	    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR," Abort failed for src_tech = %d, target_tech = %d and script type = %d", 
		  src_tech, target_tech, RFLM_MEAS_DM_PRELOAD_SCRIPT );
		return RFM_MEAS_COMMON_FAILURE;
	  }
      status = rfmeas_mdsp_meas_abort_script(channel_params->neighbor_rxlm_buf_idx, RFLM_MEAS_DM_TRIGGER_SCRIPT);
	  if( status == FALSE)
	  {
	    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR," Abort failed for src_tech = %d, target_tech = %d and script type = %d", 
		  src_tech, target_tech, RFLM_MEAS_DM_TRIGGER_SCRIPT );
		return RFM_MEAS_COMMON_FAILURE;
	  }

	  status = rfmeas_mdsp_meas_abort_script(channel_params->neighbor_rxlm_buf_idx, RFLM_MEAS_DM_NONTRX_SCRIPT);
	  if( status == FALSE)
	  {
	    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR," Abort failed for src_tech = %d, target_tech = %d and script type = %d", 
		  src_tech, target_tech, RFLM_MEAS_DM_NONTRX_SCRIPT );
		/* NON TRX writes are not present unless added by source tech in build reset API*/
		return RFM_MEAS_COMMON_FAILURE;
	  }
    }
	else if (src_tech == RFCOM_GSM_MODE)
	{
      script_type = RFM_MEAS_PROGRAM_CLEANUP;
	  status = rfmeas_mdsp_meas_abort_script(channel_params->serving_rxlm_buf_idx, RFLM_MEAS_DM_TRIGGER_SCRIPT);
	  
	  if( status == FALSE)
	  {
	    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR," Abort failed for src_tech = %d, target_tech = %d and script type = %d", 
		  src_tech, target_tech, RFLM_MEAS_DM_TRIGGER_SCRIPT );
		return RFM_MEAS_COMMON_FAILURE;
	  }
	  status = rfmeas_mdsp_meas_abort_script(channel_params->serving_rxlm_buf_idx, RFLM_MEAS_DM_NONTRX_SCRIPT);
	  
	  if( status == FALSE)
	  {
	    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR," Abort failed for src_tech = %d, target_tech = %d and script type = %d", 
		  src_tech, target_tech, RFLM_MEAS_DM_NONTRX_SCRIPT );
		/* NON TRX writes are not present unless added by target tech in build reset API*/
        return RFM_MEAS_COMMON_FAILURE;
	  }
	}

  }

  if (status == TRUE)
  {
    result = RFM_MEAS_COMMON_SUCCESS;
  }
  else
  { 
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR," Abort failed for src_tech = %d, target_tech = %d and script type = %d", 
		  src_tech, target_tech, script_type );
	
    result = RFM_MEAS_COMMON_FAILURE;
  }

    /************************************/
    /* New power saving API (ON -> LPM) */
    /************************************/
  if( TRUE == is_rf_fr_25072_enabled)
  {

  if (target_tech == RFCOM_GSM_MODE)
  {
    /* Disable RX Device (ON -> LPM) for lowest supported band */
    if ( !rfdevice_gsm_rx_pwr_sleep( rfm_dev, rfgsm_nv_get_lowest_supported_band(rfm_dev) ) )
    {
      ERR_FATAL("rfgsm_core_meas_exit: Disable RX Device %d (ON->LPM) failed for band %d",
                rfm_dev, rfgsm_nv_get_lowest_supported_band(rfm_dev),0);
      return FALSE;
    }
  }
  }

  /* Resume therm reads */
  if( rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started )
  {
    /* Enable therm reads */
    rfgsm_core_temp_comp_resume(rfm_dev);
  }

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called by -> rfgsm_mc_meas_build_scripts to support configuration 
    and initialization for second WTR
 
  @param src_device : src device 
 
  @pre
  rfgsm_mc_meas_build_scripts function must have been called

  @retval
  
*/

rfm_meas_result_type rfgsm_core_meas_write_preconf_script( rfm_device_enum_type rfm_dev )
{  
 
  rf_buffer_intf *rf_buffer = NULL;
  boolean result = FALSE;

  result = rfdevice_gsm_rxonly_init( rfm_dev, RFDEVICE_EXECUTE_IMMEDIATE ,rf_buffer );

  if (result)
  {
    return RFM_MEAS_COMMON_SUCCESS;
  }
  else
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "preconfig script creation failed, %d ", result );
	return RFM_MEAS_COMMON_FAILURE;
  }

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called by -> rfgsm_mc_meas_build_scripts to check if preconfiguration
    is required for target WTR
    
  @param preconf_required : If WTR needs to be pre-configured for target tech
 
 
  @pre
  rfgsm_mc_meas_build_scripts function must have been called

  @retval
  rfm_meas_result_type
*/

rfm_meas_result_type rfgsm_core_meas_preconf_required( rfm_meas_header_type gsm_meas_param,
                                                     boolean* preconf_required )
{
  rfdevice_rxtx_common_class *src_device_instance[4] = {NULL};
  rfdevice_rxtx_common_class *tgt_device_instance_ptr = NULL;
  uint8 source_index = 0;
  rfdevice_trx_gsm_rx *gsm_device_ptr = NULL;
#ifdef FEATURE_WCDMA
  rfdevice_trx_wcdma_rx **wcdma_device_ptr =NULL;
#endif
#ifdef FEATURE_LTE 
  rfdevice_trx_lte_rx **lte_device_ptr = NULL;
#endif
#ifdef FEATURE_TDSCDMA
  rfdevice_trx_tdscdma_rx **tds_device_ptr = NULL;
#endif 
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  
  int32 gsm_band;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  src_tech = gsm_meas_param.source_tech;
  target_tech = gsm_meas_param.target_tech;
  
  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = gsm_meas_param.target_param.device;
  }
  else if ( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = gsm_meas_param.source_param[0].device;
  } 
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d", src_tech, target_tech);
    result = RFM_MEAS_COMMON_FAILURE;
    return result;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return result;
  }  

  /* Get the RFC device from source tech */
	
  switch (gsm_meas_param.source_tech)
  {
    case RFCOM_WCDMA_MODE:
#ifdef FEATURE_WCDMA
	  wcdma_device_ptr = (rfdevice_trx_wcdma_rx **)rfc_wcdma_get_device(gsm_meas_param.source_param[0].device, 
																		RFC_CONFIG_RX,	
		     															(rfcom_wcdma_band_type)gsm_meas_param.source_param[0].band,
																		RFDEVICE_TRANSCEIVER);
	  if(wcdma_device_ptr != NULL)
      {
        source_index=0;
        while ((source_index < 4) && (wcdma_device_ptr[source_index] != NULL))
        {
          src_device_instance[source_index] = 
          rfdevice_wcdma_rx_get_common_device_pointer(wcdma_device_ptr[source_index]);
          source_index++;
        }
      }
	  else
	  {
	    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "wcdma_device_ptr is NULL for device id %d and source band %d",
				gsm_meas_param.source_param[0].device, 
				gsm_meas_param.source_param[0].band);
	  }

#endif
	  break;

    case RFCOM_LTE_MODE: 
#ifdef FEATURE_LTE 
	  lte_device_ptr = (rfdevice_trx_lte_rx **) rfc_lte_get_device(gsm_meas_param.source_param[0].device,
                                                                       RFC_CONFIG_RX,   
                                                                       (rfcom_lte_band_type)gsm_meas_param.source_param[0].band,
                                                                       RFDEVICE_TRANSCEIVER);
	  if(lte_device_ptr != NULL)
          {
            source_index=0;
            while ((source_index < 4) && 
                   (lte_device_ptr[source_index] != NULL))
            {
             src_device_instance[source_index] = 
              rfdevice_lte_rx_get_common_device_pointer(lte_device_ptr[source_index]);
              source_index++;
            }
          }
	  else
	  {
	    MSG_2(MSG_SSID_RF, 
                  MSG_LEGACY_ERROR, 
                  "lte_device_ptr is NULL for device id %d and source band %d",
                  gsm_meas_param.source_param[0].device,
                  gsm_meas_param.source_param[0].band);
	  }
#endif	/* #ifdef FEATURE_LTE */			
	  break;

      case RFCOM_TDSCDMA_MODE:
#ifdef FEATURE_TDSCDMA
        tds_device_ptr = ( rfdevice_trx_tdscdma_rx **) rfc_tdscdma_get_rf_device_obj(gsm_meas_param.source_param[0].device, 
                               RFC_CONFIG_RX,	
                               gsm_meas_param.source_param[0].band,
                               RFDEVICE_TRANSCEIVER);
        if(tds_device_ptr != NULL)
        {
          source_index=0;
          while ((source_index < 4) && (tds_device_ptr[source_index] != NULL))
          {
            src_device_instance[source_index] 
            = rfdevice_tdscdma_rx_get_common_device_pointer(tds_device_ptr[source_index]);
            source_index++;
          }
        }
        else
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "tds_device_ptr is NULL for device id %d and source band %d",
				gsm_meas_param.source_param[0].device,
				gsm_meas_param.source_param[0].band);
        }
#endif
      break;
	default:
	  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unsupported tech/Pre config is exempted for this tech %d",gsm_meas_param.source_param[0].band);
	  result = RFM_MEAS_COMMON_SUCCESS;
	  *preconf_required = FALSE;
	  return result;
	  break;
  }
   /* Get transceiver instance from taget tech, GSM */
   gsm_band = gsm_meas_param.target_param.band;
   
   if( gsm_band > RFCOM_NUM_GSM_BANDS )
   {
     gsm_band = rfgsm_core_rf_card_band_to_rfcom_gsm_band((rf_card_band_type)gsm_band); 

   }
   gsm_device_ptr = (rfdevice_trx_gsm_rx *)rfc_gsm_get_phy_device(gsm_meas_param.target_param.device, 
																		RFC_CONFIG_RX,	
		     															(rfcom_gsm_band_type)gsm_band,
																		RFDEVICE_TRANSCEIVER);
   if(gsm_device_ptr != NULL)
   {
     tgt_device_instance_ptr 
     = rfdevice_gsm_rx_get_common_device_pointer(gsm_device_ptr);
	 if( tgt_device_instance_ptr == NULL)
	 {
       result = RFM_MEAS_COMMON_FAILURE;
	   MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "tgt device pointer is null");
	   return result;
	 }
   }
   else
   {
     MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, 
		 "gsm_device_ptr is NULL for device id % and target band received %d, target band used %d",
	 		gsm_meas_param.target_param.device,
	 		gsm_meas_param.target_param.band,
			gsm_band);
   }
   result = RFM_MEAS_COMMON_SUCCESS ;

   /* For target device, search the list of the source devices */

   for (source_index = 0; source_index < 4; source_index++) 
   {
     if((src_device_instance[source_index] != NULL) && (tgt_device_instance_ptr != NULL))
     {
       if(rfdevice_compare_common_rxtx_devices(tgt_device_instance_ptr,src_device_instance[source_index]))
       {
         *preconf_required = FALSE ;
         return result;
       }
     }
   }
   /*preconfig is not needed for RFFE protocol based devices*/
   if(rfgsm_core_handle_ptr->ssbi_protocol_supported == TRUE)
   {
    *preconf_required = TRUE;
   }
   else
   {
     *preconf_required = FALSE;
   }	 
   
  return result;
   
}


/*----------------------------------------------------------------------------*/

/*!
  @brief
    This function is called by -> rfgsm_core_meas_cleanup to check if rx need to be 
	disabled on source/target WTR for G2X/X2G case after G measurements are done
    
  @param disable_rx : If rx needs to be disabled
 
 
  @pre
  rfgsm_mc_meas_build_reset_scripts function must have been called

  @retval
  rfm_meas_result_type
*/

rfm_meas_result_type rfgsm_core_meas_disable_rx_required( rfm_meas_header_type gsm_meas_param,
                                                     boolean* disable_rx )
{ 
  rfm_meas_param_type gsm_param;
  rfm_meas_param_type xtech_param;

  rfcom_mode_enum_type xtech = RFM_INVALID_MODE;

  rfdevice_rxtx_common_class *xtech_device_instance[4]={NULL};
  rfdevice_rxtx_common_class *gsm_device_instance_ptr = NULL;
  uint8 xtech_index = 0;
  rfdevice_trx_gsm_rx *gsm_device_ptr = NULL;
#ifdef FEATURE_WCDMA
  rfdevice_trx_wcdma_rx **wcdma_device_ptr =NULL;
#endif
#ifdef FEATURE_LTE 
  rfdevice_trx_lte_rx **lte_device_ptr = NULL;
#endif
#ifdef FEATURE_TDSCDMA
  rfdevice_trx_tdscdma_rx **tds_device_ptr = NULL;
#endif
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  int32 gsm_band;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  src_tech = gsm_meas_param.source_tech;
  target_tech = gsm_meas_param.target_tech;
  
  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = gsm_meas_param.target_param.device;
  }
  else if ( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = gsm_meas_param.source_param[0].device;
  } 
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",src_tech,target_tech);
    result = RFM_MEAS_COMMON_FAILURE;
    return result;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return result;
  }  

  /* Get the RFC device from source tech */
  if( RFCOM_GSM_MODE == gsm_meas_param.source_tech )
  {
    xtech = gsm_meas_param.target_tech;
	xtech_param = gsm_meas_param.target_param;
	gsm_param = gsm_meas_param.source_param[0];
	  
  }
  else if ( RFCOM_GSM_MODE == gsm_meas_param.target_tech )
  {
	xtech = gsm_meas_param.source_tech;
	xtech_param = gsm_meas_param.source_param[0];
	gsm_param = gsm_meas_param.target_param;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",
			gsm_meas_param.source_tech,
			gsm_meas_param.target_tech);
	result = RFM_MEAS_COMMON_FAILURE;
	return result;

  }

  switch ( xtech )
  {
    case RFCOM_WCDMA_MODE:
#ifdef FEATURE_WCDMA
	  wcdma_device_ptr = (rfdevice_trx_wcdma_rx **)rfc_wcdma_get_device(xtech_param.device, 
																		RFC_CONFIG_RX,	
		     															(rfcom_wcdma_band_type)xtech_param.band,
																		RFDEVICE_TRANSCEIVER);
	  if(wcdma_device_ptr != NULL)
      {
        xtech_index=0;
        while ((xtech_index < 4) && (wcdma_device_ptr[xtech_index] != NULL))
        {
          xtech_device_instance[xtech_index] = 
          rfdevice_wcdma_rx_get_common_device_pointer(wcdma_device_ptr[xtech_index]);
          xtech_index++;
        }
      }
	  else
	  {
	    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "WCDMA device ptr is NULL for device id %d and band %d",xtech_param.device, xtech_param.band);
	  }
	  

#endif
	  break;

	case RFCOM_LTE_MODE: 
#ifdef FEATURE_LTE 
	  lte_device_ptr = (rfdevice_trx_lte_rx **) rfc_lte_get_device(xtech_param.device,
                                                                       RFC_CONFIG_RX,
                                                                       (rfcom_lte_band_type)xtech_param.band,
                                                                       RFDEVICE_TRANSCEIVER);
	  if(lte_device_ptr != NULL)
          {
            xtech_index=0;
            while ((xtech_index < 4) && 
                   (lte_device_ptr[xtech_index] != NULL))
            {
               xtech_device_instance[xtech_index] = 
               rfdevice_lte_rx_get_common_device_pointer(lte_device_ptr[xtech_index]);	        
               xtech_index++;
            }
          }
	  else
	  {
	    MSG_2(MSG_SSID_RF, 
                  MSG_LEGACY_ERROR, 
                  "LTE device ptr is NULL for device id %d and band %d",
                  xtech_param.device, 
                  xtech_param.band);
	  }
#endif				
	  break;

    case RFCOM_TDSCDMA_MODE:
#ifdef FEATURE_TDSCDMA
      tds_device_ptr = (rfdevice_trx_tdscdma_rx **) rfc_tdscdma_get_rf_device_obj(xtech_param.device, 
                                                                            RFC_CONFIG_RX,	
                                                                            (rfcom_tdscdma_band_type)xtech_param.band,
                                                                            RFDEVICE_TRANSCEIVER);
      if(tds_device_ptr != NULL)
      {
        xtech_index=0;
        while ((xtech_index < 4) && (tds_device_ptr[xtech_index] != NULL))
        {
          xtech_device_instance[xtech_index]  
                 = rfdevice_tdscdma_rx_get_common_device_pointer(tds_device_ptr[0]);	
          xtech_index++;
        }
      }
      else
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "tds_device_ptr is NULL for device id %d and band %d",
		        xtech_param.device, xtech_param.band);
      }
#endif
      break;
	default:
	  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unsupported tech/ rx disable is exempted for this tech %d",xtech_param.band);
	  result = RFM_MEAS_COMMON_SUCCESS;
	  *disable_rx = FALSE;
	  return result;
	  break;
  }
   /* Get transceiver instance from taget tech, GSM */
   gsm_band = gsm_param.band;
   
   if( gsm_band > RFCOM_NUM_GSM_BANDS )
   {
     gsm_band = rfgsm_core_rf_card_band_to_rfcom_gsm_band((rf_card_band_type)gsm_band); 

   }
   gsm_device_ptr = (rfdevice_trx_gsm_rx *)rfc_gsm_get_phy_device(gsm_param.device,   
															 RFC_CONFIG_RX,
															 (rfcom_gsm_band_type)gsm_band,
															 RFDEVICE_TRANSCEIVER);
   if(gsm_device_ptr != NULL)
   {
     gsm_device_instance_ptr = 
     rfdevice_gsm_rx_get_common_device_pointer(gsm_device_ptr);	
	 if( gsm_device_instance_ptr == NULL)
	 {
       result = RFM_MEAS_COMMON_FAILURE;
	   MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "gsm device pointer is null");
	   return result;
	 }
   }
   else
   {
     MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM device ptr is NULL for device id %d and band received %d, band used %d",
	          gsm_param.device, gsm_param.band, gsm_band );
   }
   result = RFM_MEAS_COMMON_SUCCESS ;

   /* For target device, search the list of the source devices */

   for (xtech_index = 0; xtech_index < 4; xtech_index++) 
   {
     if((xtech_device_instance[xtech_index] != NULL) && (gsm_device_instance_ptr != NULL))
     {
       if(rfdevice_compare_common_rxtx_devices(gsm_device_instance_ptr,xtech_device_instance[xtech_index]))
       {
         *disable_rx = FALSE;
         return result;
       }
     }
   }
   /*disable rx is not needed for RFFE protocol based devices*/
   if(rfgsm_core_handle_ptr->ssbi_protocol_supported == TRUE)
   {
     *disable_rx = TRUE;
   }
   else
   {
     *disable_rx = FALSE;
   }	
  
  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
	This function is called by -> rfgsm_core_meas_cleanup to 
	disable rx on target WTR
 
  @param src_device : src device 
 
  @pre
  rfgsm_mc_meas_build_scripts function must have been called

  @retval
  
*/

rfm_meas_result_type rfgsm_core_meas_rx_disable( rfm_device_enum_type rfm_dev )
{  
 
  rf_buffer_intf *rf_buffer = NULL;
  rfm_meas_result_type result = RFM_MEAS_COMMON_FAILURE;

  rf_buffer = rf_buffer_create_tech_scaling(RFGSM_DISABLE_RX_SSBI_SCRIPT_SIZE,0,0,NUM_QS_PER_100_USEC);
  result = rfdevice_gsm_rx_disable( rfm_dev,RFDEVICE_CREATE_SCRIPT,rf_buffer );
  rf_buffer_delete(rf_buffer);

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfdevice_gsm_rx_disable %d", result);
  result = RFM_MEAS_COMMON_SUCCESS;
  return result;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to do preconfiguration for target WTR, if required.
    
  @param preconf_required : If WTR needs to be pre-configured for target tech
  
  @pre
  rfm_meas_common_enter function must have been called

  @retval
  rfm_meas_result_type
*/

rfm_meas_result_type rfgsm_core_meas_pre_config( rfm_meas_setup_param_type *meas_scripts_param)
{
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfm_meas_params_channel_type *channel_params = NULL;
  rfm_meas_header_type gsm_meas_param;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  boolean preconf_required = FALSE;
  
  if (meas_scripts_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"meas_scripts_param is NULL!", 0);
    return result;
  }
  
  src_tech = meas_scripts_param->header.source_tech;
  target_tech = meas_scripts_param->header.target_tech;
  
  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.target_param.device;
  }
  else if ( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.source_param[0].device;
  } 
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",
			src_tech,
			target_tech);
	result = RFM_MEAS_COMMON_FAILURE;
	return result;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return result;
  }  


  if(target_tech == RFCOM_LTE_MODE || target_tech == RFCOM_TDSCDMA_MODE)
  {
    channel_params = (rfm_meas_params_channel_type *)meas_scripts_param->source_param;
  }
  else
  {
    channel_params = (rfm_meas_params_channel_type *)meas_scripts_param->target_param;
  }

  if (channel_params == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"channel_params is NULL!", 0);
    return result;
  }

  gsm_meas_param = meas_scripts_param->header;

  // W2G, L2G and T2G StartUp Script
  if ( target_tech == RFCOM_GSM_MODE && meas_scripts_param->script_type == RFM_MEAS_PROGRAM_STARTUP) 
  {
  	/* To check if source is using two WTRs , if yes then dont preconfigure target WTR */
	if (gsm_meas_param.num_source_item > 1)
    {
	  preconf_required = FALSE;
	}
	else
	{
       /* Before building measurement script check if target tech is present at second WTR and
            if so then first initialize and configure the second WTR */
      result = rfgsm_core_meas_preconf_required( gsm_meas_param, & preconf_required );
	
      if (RFM_MEAS_COMMON_FAILURE == result)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"WTR preconf script creation failed", 0);
        return result;
      }
	}

    if (preconf_required)
    {
      result = rfgsm_core_meas_write_preconf_script(rfm_dev);
      /* result will be Failure if SBI writes is 0 and that will be the case even in success scenario */
      /* Since the second WTR will already have required configuration before sleep and after first measurement gap */
      if ( result == RFM_MEAS_COMMON_FAILURE )
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"WTR is already configured/ script writing failed %d", result);
      }
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"preconfig not done for target tech %d and script type %d", target_tech, meas_scripts_param->script_type );
  }
  result = RFM_MEAS_COMMON_SUCCESS;
  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to check if rx needs to be disabled on second WTR .
	If yes then call rfgsm_core_meas_rx_disable
    
  @param preconf_required : If WTR needs to be pre-configured for target tech
 
 
  @pre
  rfgsm_mc_meas_build_scripts function must have been called

  @retval
  rfm_meas_result_type
*/

rfm_meas_result_type rfgsm_core_meas_cleanup(rfm_meas_setup_param_type *meas_scripts_param)
{	
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfm_meas_params_channel_type *channel_params = NULL;
  rfm_meas_header_type gsm_meas_param;
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  boolean disable_rx = FALSE;
  uint32 end_time = 0;
  if (meas_scripts_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"meas_scripts_param is NULL!", 0);
    return result;
  }
  
  src_tech = meas_scripts_param->header.source_tech;
  target_tech = meas_scripts_param->header.target_tech;
  
  if ( target_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.target_param.device;
  }
  else if( src_tech == RFCOM_GSM_MODE)
  {
    rfm_dev = meas_scripts_param->header.source_param[0].device;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM is not source or target tech, source_tech is %d, target tech is %d",
			src_tech,
			target_tech);
	result = RFM_MEAS_COMMON_FAILURE;
	return result;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return result;
  }



  if(target_tech == RFCOM_LTE_MODE || target_tech == RFCOM_TDSCDMA_MODE)
  {
    channel_params = (rfm_meas_params_channel_type *)meas_scripts_param->source_param;
  }
  else
  {
    channel_params = (rfm_meas_params_channel_type *)meas_scripts_param->target_param;
  }

  if (channel_params == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"channel_params is NULL!", 0);
    return result;
  }

  gsm_meas_param = meas_scripts_param->header;
  
    // W2G, L2G and T2G disable rx on target WTR after measurement.
	// G2W, G2L and G2T disable rx on source WTR before target measurement starts.
  if (( target_tech == RFCOM_GSM_MODE && meas_scripts_param->script_type == RFM_MEAS_PROGRAM_CLEANUP) ||
	  (src_tech == RFCOM_GSM_MODE && meas_scripts_param->script_type == RFM_MEAS_PROGRAM_STARTUP))
  {
	if (gsm_meas_param.num_source_item > 1)
    {
	  disable_rx = FALSE;
	}
	else
	{
      /* Before disabling rx on second WTR check if the WTR was configured just for target measurements */
   
      result = rfgsm_core_meas_disable_rx_required( gsm_meas_param, & disable_rx );
  	
      if (RFM_MEAS_COMMON_FAILURE == result)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Check for disable drx failed on target WTR", 0);
        return result;
      }
	}
    if (disable_rx)
	{
	  result = rfgsm_core_meas_rx_disable(rfm_dev);	  
      if ( result == RFM_MEAS_COMMON_FAILURE )
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RX diasble failed for target wtr %d", disable_rx);
      }
	}
  }
  else
  {
    end_time = geran_test_mode_api_get_qsym_count(SYS_MODEM_AS_ID_NONE);
    MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH,"cleanup not done for for target tech %d and script type %d at QSC %d", target_tech, meas_scripts_param->script_type, end_time);
  }
  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called during X2G case for disable tuners
    
  @pre
    Target tech should be GSM only
 


*/

void rfgsm_core_disable_rffe(rfm_meas_setup_param_type *meas_scripts_param)
{

  rf_buffer_intf *rf_buffer = NULL;  
  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if (meas_scripts_param == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"meas_scripts_param is NULL!", 0);
    return ;
  }

  if (meas_scripts_param->header.target_tech != RFCOM_GSM_MODE)
  {
    //MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"target tech is not GSM, it is %d. RFFE/Tuners is not disabled", meas_scripts_param->header.target_tech);
	return;
  }
  
  rfm_dev = meas_scripts_param->header.target_param.device;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL! for rdm_dev %d", rfm_dev);
    return;
  }

  rf_buffer  = rfmeas_mdsp_buffer_get(RFM_MEAS_PROGRAM_CLEANUP);  
  
  rfc_gsm_disable_rffe(rfm_dev, rf_buffer, (rfcom_gsm_band_type)meas_scripts_param->header.target_param.band);  
  
}
