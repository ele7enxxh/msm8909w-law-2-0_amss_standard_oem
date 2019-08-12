/*!
  @file
  ftm_common_selftest.c

  @brief
  This module contains definitions and prototypes for common selftest functionality.
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_selftest.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/28/15   ska     Delete FBRx cases for Rx only bands: B29 and B32
11/25/14   jmf     Call to new rfcommon_mdsp API to setup VSWR params in FW
11/04/14   dbz     Add support for VSWR V2 processing
11/03/14   ndb     Fix the compiler error in LTE removed builds
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
10/14/14   jmf     [selftest] Implement FBRx Noise Floor Cal
10/09/14   dbz     Add WCDMA TX upsampling support
10/03/14   dbz     KW error fix
10/01/14   dbz     Add EVM calculation config for TDSCDMA, 1x, WCDMA
09/19/14   jmf     Hook up VSWR v2 and temp fixes for FW RxRMS cap type bypassed to EVM type
09/10/14   dbz     Add support for TX upsampling and EVM calculations for LTE
08/18/14   jmf     [SelfTest] ACLR meas changes
08/05/14   jmf     [SelfTest] Remove RFCMD FW message to set self-test flag
07/31/14   jmf     Add API to query Current Self Test Mode
                   Remove call to RFCMD MSG to set FW Self Test Mode
07/24/14   jmf     [SelfTest] Move self test mode global flag to .c
07/23/14   dbz     Added support for selftest VSWR split and EVM capture for 1x/LTE
07/17/14   jmf     Add api to set self test flag
06/30/14   daa     Created.

==============================================================================*/

#include "rfa_variation.h"
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "ftmicap.h"
#include "stringl.h" /* for memscpy */
#include "ftm_calibration_data_module.h"
#include "ftm_common_selftest.h"
#include "rfcommon_mdsp.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "rex.h"
#include "task.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_common_dispatch.h"
#include "ftm_common.h"
#include "rf_test_task_v.h"
#include "rfcommon_nv.h"
#include "rfm.h"
#include "rfm_internal.h"
#include "ftm_rf_cmd.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rxlm_intf.h"
#endif
#include "ftm_common_control.h"
#include "ftm_msg.h"
#include "rfc_card.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "ftm_common_radio.h"
#include "ftm_cdma_cmd.h"
#include "ftm_rffe.h"
#endif
#include "ftm_gpio.h"
#include "modem_mem.h"
#include "rfcommon_mc.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "ftm_common_xpt.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rfcommon_core_xpt.h"
#endif
#include "rfcommon_mdsp_types.h"
#include "rfcommon_core_utils.h"

#ifdef FEATURE_WCDMA
#include "ftm_wcdma_ctl.h"
#include "rfdevice_wcdma_intf.h"
#include "rfnv_wcdma.h"
#include "rfwcdma_msm.h"
#include "rfdevice_wcdma_pa_intf.h"
#include "rfdevice_wcdma_asm_intf.h"
#include "ftm_wcdma_dispatch.h"
#include "ftm_wcdma_calv3.h"
#endif

#ifdef FTM_HAS_TDSCDMA
#include "ftm_tdscdma_ctl.h"
#include "rfdevice_tdscdma_pa_intf.h"
#include "rfdevice_tdscdma_asm_intf.h"
#include "rf_tdscdma_msm.h"
#include "rf_tdscdma_core_util.h"
#endif


#ifdef FEATURE_CDMA1X
#include "ftm_1x_control.h"
#include "ftm_1x_calibration.h"
#include "rfdevice_cdma_interface.h"
#include "ftm_cdma_dispatch.h"
#include "rfc_cdma.h"
#include "rfm_1x_ftm.h"
#include "rfcommon_core_device_manager.h"
#include "rfcommon_locks.h"
#include "rf_cdma_data.h"
#include "ftm_1x_xpt.h"
#include "ftm_cdma_data.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_mc.h"
#include "rfm_cdma_ftm.h"
#endif

#ifdef FEATURE_GSM
#include "ftm_gsm_ctl.h"
#include "rfdevice_gsm_intf.h"
#include "ftm_gsm_rfprot.h"
#endif

#ifdef FEATURE_LTE
#include "rf_lte_ftm_cmd_proc.h"
#include "rflte_ftm_external_api.h"
#include "ftm_lte_xpt.h"
#include "rflte_ftm_mc.h"
#include "rflte_nv.h"
#include "rflte_msm.h"
#include "rflte_core_util.h"
#include "rfdevice_lte_pa_intf.h"
#include "rfdevice_lte_interface.h"
#include "rfdevice_lte_asm_intf.h"
#include "rflte_msm_iq_capture.h"
#include "ftm_lte_common_dispatch.h"
#endif

#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_ctl.h"
#include "ftm_tdscdma_calv3.h"
#endif

#ifdef FEATURE_RF_HAS_QTUNER

#ifdef FEATURE_CDMA1X
#include "rf_cdma_utils_freq.h"
#include "rfdevice_hdet_cdma_intf.h"
#endif

#ifdef FEATURE_WCDMA
#include "rfwcdma_core_antenna_tuner.h"
#endif

#include "rfdevice_hdet_cmn_intf.h"
#include "rfdevice_hdet_wcdma_intf.h"
#endif

#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_data.h"
#include "rf_qmi_rfrpe_svc.h"

#if defined (FEATURE_XO_FACTORY_CAL)
#include "ftm_calibration_xo.h"
#endif

#include "rfdevice_logical_component_types.h"

// extern boolean ftm_selftest_pwr_only;
// coupler_direction_enum_type tdscdma_coupler_direction = COUPLER_DIRECTION_FORWARD;


// Define Q factor for EVM FIR's.
#define EVM_RX_FIR_QFACTOR 15

// Define VSWR V2 phase constant

#define VSWR_V2_PHASE_OFFSET_1 54000
// Offset 1 in degree100 = 36000 + 18000 to rotate the output to -/+180 degree
#define VSWR_V2_PHASE_MODE_CONSTANT 36000
#define VSWR_V2_PHASE_OFFSET_2 18000
// Offset 2 in degree100 = 18000 to rotate the output to -/+180 degree

// Define to limit bandwidth and flatten RF frequency response for EVM
// captures. Filters are used only for LTE. Lengths must be multiple of 2.
// Q factor (EVM_RX_FIR_QFACTOR).
static int16 evm_lte_5_fir[] = {
   1509,  //  0.046061661588133   98916665(Q31)
  -3060,  // -0.093370032975890 -200510619(Q31)
  -2200,  // -0.067147047930421 -144197187(Q31)
   2445,  //  0.074600601253681  160203571(Q31)
   8052,  //  0.245726434557269  527693500(Q31)
  11908,  //  0.363410073088509  780417189(Q31)
  11908,  //  0.363410073088509  780417189(Q31)
   8052,  //  0.245726434557269  527693500(Q31)
   2445,  //  0.074600601253681  160203571(Q31)
  -2200,  // -0.067147047930421 -144197187(Q31)
  -3060,  // -0.093370032975890 -200510619(Q31)
   1509}; //  0.046061661588133   98916665(Q31)

static int16 evm_lte_10_fir[] = {
  -1245,  // -0.037994756976527  -81593119(Q31)
   3370,  //  0.102830278521677  220826342(Q31)
  -1952,  // -0.059581894357404 -127951144(Q31)
  -4571,  // -0.139501682388697 -299577582(Q31)
   5096,  //  0.155531963473947  334002348(Q31)
  17990,  //  0.549011288867506 1178992765(Q31)
  17990,  //  0.549011288867506 1178992765(Q31)
   5096,  //  0.155531963473947  334002348(Q31)
  -4571,  // -0.139501682388697 -299577582(Q31)
  -1952,  // -0.059581894357404 -127951144(Q31)
   3370,  //  0.102830278521677  220826342(Q31)
  -1245}; // -0.037994756976527  -81593119(Q31)

static int16 evm_lte_15_fir[] = {
   2136,  //  0.065179313560233  139971510(Q31)
  -2109,  // -0.064373753947658 -138241584(Q31)
  -3605,  // -0.110011801638832 -236248545(Q31)
    485,  //  0.014798102180322   31778682(Q31)
   7975,  //  0.243373525310991  522640666(Q31)
  13938,  //  0.425366428612390  913467450(Q31)
  13938,  //  0.425366428612390  913467450(Q31)
   7975,  //  0.243373525310991  522640666(Q31)
    485,  //  0.014798102180322   31778682(Q31)
  -3605,  // -0.110011801638832 -236248545(Q31)
  -2109,  // -0.064373753947658 -138241584(Q31)
   2136}; //  0.065179313560233  139971510(Q31)

static int16 evm_lte_20_fir[] = {
   -313,  // -0.009554325511675  -20517758(Q31)
   2824,  //  0.086172917596038  185054931(Q31)
  -3572,  // -0.109006543670225 -234089770(Q31)
  -3770,  // -0.115037420968608 -247040980(Q31)
   6354,  //  0.193901753966908  416400846(Q31)
  17296,  //  0.527832124754499 1133510857(Q31)
  17296,  //  0.527832124754499 1133510857(Q31)
   6354,  //  0.193901753966908  416400846(Q31)
  -3770,  // -0.115037420968608 -247040980(Q31)
  -3572,  // -0.109006543670225 -234089770(Q31)
   2824,  //  0.086172917596038  185054931(Q31)
   -313}; // -0.009554325511675  -20517758(Q31)

static int16 tx_up_filter_coef[] = {21, -243, 898, -2605, 10108, 10108, -2605, 898, -243, 21};

static boolean ftm_common_selftest_get_lte_fbrx_noise_nv_item(uint8 nv_band, rfnv_item_id_enum_type * nv_item_to_write);
static boolean ftm_common_selftest_get_wcdma_fbrx_noise_nv_item(uint8 nv_band, rfnv_item_id_enum_type * nv_item_to_write);

ftm_common_selftest_vswr_v2_results_type ftm_common_selftest_vswr_v2_results = {0,0,0,0,0
};

uint8 ftm_common_selftest_modeset_flag = 0;

boolean
ftm_common_trigger_self_test_iq_cap
(
  ftm_rf_technology_type ftm_rf_tech,
  uint8  device,
  uint8  device_fb,
  uint16 num_samples,
  uint32 ftm_selftest_meas_type,
  uint32 * samp_rate
)
{
  boolean do_processing = (ftm_selftest_meas_type > 0)? TRUE: FALSE;
  uint8 rx_bw_filt_len=0;

#ifdef FEATURE_LTE
  rflte_ftm_mc_sm_type     *lte_ftm_mc_sm_ptr      = NULL; /*holds pointer to LTE ftm State machine structure */

  uint32 fbrx_carrier_idx = 0;

  int16 *rx_bw_filt_coeff = NULL;
  uint8 eq_filt_mask;

#endif
  uint8 tx_upsamp;
  uint8 tx_up_fir_len;
  int8 tx_up_fir_shift;
  int16 *tx_up_fir = NULL;
  volatile boolean api_status = TRUE; /* shouldn't this be volatile? */
  rf_common_xpt_buffer_id_type buffer_ids; /* buffer ids for capture and dpd results */
  rfcommon_xpt_processing_type proc_flag = PROC_NONE;
  uint8 ftm_selftest_xpt_capture_type;
  rfcommon_mdsp_self_test_vswr_params_type vswr_mdsp_params;

  if ( (ftm_selftest_meas_type & ENABLE_ACLR_MEAS ) !=0 )
  {
    if ((ftm_selftest_meas_type & (ENABLE_EVM_MEAS | ENABLE_VSWR_MEAS_V2 | ENABLE_TX_PWR_MEAS) ) == 0)
    {
      //FTM_MSG(FTM_ERROR, "Change to RxRMS cap type");
      //ftm_selftest_xpt_capture_type = XPT_CAPTURE_SELF_TEST_RX_RMS;
      do_processing = FALSE;
    }
  }

  /* Init Buffer ID values */
  buffer_ids.capt_buf_id = 0;
  buffer_ids.res_buf_id = 0;

  /* Ensure no previus capture/processing is still in progress */
  if( (api_status == TRUE) )
  {
    api_status &= rfcommon_xpt_is_capture_done();
    if( api_status == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: IQ capture already "
                 "in progress for tech %d", ftm_rf_tech );
    }

    /* Monitor processing only if it was requested */
    if (do_processing == TRUE)
    {
      api_status &= rfcommon_xpt_is_processing_done();
      if( api_status == FALSE)
      {
        FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: IQ processing already "
                   "in progress for tech %d", ftm_rf_tech );
      }
    }
  }

  /* Set capture and processing flags */
  if( (api_status == TRUE) )
  {
    api_status &= rfcommon_xpt_set_capture_in_progress();
    if (do_processing == TRUE)
    {
      api_status &= rfcommon_xpt_set_processing_in_progress();
    }
    /* Initialize buffer pool for xpt captures */
    rfcommon_core_xpt_init_buffers();

    /* Get buffer ids, TODO: need to pass in do_processing flag to not get dpd result buffer */
    api_status &= rfcommon_core_xpt_get_buffers(&buffer_ids);
  }

  proc_flag = (do_processing == TRUE)?PROC_SELF_TEST:PROC_NONE;

  /* This needs to be changed
     I still don't know how these flags ultimately impact the capture type
     *See rfcommon_mdsp.c:750 or so
  */
  // if (ftm_selftest_pwr_only && ((ftm_selftest_meas_type & ENABLE_EVM_MEAS) == 0))
  if ((ftm_selftest_meas_type & ENABLE_VSWR_MEAS_V2) != 0)
  {
    ftm_selftest_xpt_capture_type = XPT_CAPTURE_SELF_TEST_VSWR;
  }
  else if ((ftm_selftest_meas_type & ENABLE_EVM_MEAS) != 0)
  {
    ftm_selftest_xpt_capture_type = XPT_CAPTURE_SELF_TEST_EVM;
  }
  else // ((ftm_selftest_meas_type & ENABLE_TX_PWR_MEAS) != 0)
  {
    // FW is crashing with RxRMS type capture and selftest flag set
    ftm_selftest_xpt_capture_type = XPT_CAPTURE_SELF_TEST_EVM /*XPT_CAPTURE_SELF_TEST_RX_RMS*/ ;
  }

  /* Trigger tech-specific capture */
  switch ( ftm_rf_tech )
  {
  case FTM_RF_TECH_CDMA:
    #ifdef FEATURE_CDMA1X
    {
      /* Perform capture */
      if(api_status == TRUE)
      {

        /* Reset Rx FIR filter which is used for LTE only */

        rfcommon_mdsp_setup_self_test_params(FALSE, rx_bw_filt_len, 0, NULL, (1<<2));
        if (ftm_selftest_xpt_capture_type == XPT_CAPTURE_SELF_TEST_VSWR)
        {
          vswr_mdsp_params.vswr_split_margin = RFCOMMON_MDSP_VSWR_SPLIT_MARGIN;
          rfcommon_mdsp_setup_self_test_vswr_params(&vswr_mdsp_params);
        } 

        api_status &= ftm_1x_xpt_iq_capture(device,
                                            device_fb,
                                            ftm_selftest_xpt_capture_type,
                                            num_samples,
                                            TRUE,                  /* first_trig */
                                            TRUE,                   /* last_trig */
                                            proc_flag,
                                            buffer_ids,
                                            samp_rate);

      }
    }
    #endif
    break;

  case FTM_RF_TECH_WCDMA:
    #ifdef FEATURE_WCDMA
    {
      if(api_status == TRUE)
      {

        /* do self test specific interface programming*/

        rfcommon_mdsp_setup_self_test_params(TRUE, rx_bw_filt_len, 0, NULL, (1<<2));

       tx_upsamp = 2;
       tx_up_fir_len = 10;
       tx_up_fir_shift = -14;
       tx_up_fir = tx_up_filter_coef;

       rfcommon_mdsp_setup_self_test_upsampling_params (tx_upsamp,tx_up_fir_len, tx_up_fir_shift, tx_up_fir);
       if (ftm_selftest_xpt_capture_type == XPT_CAPTURE_SELF_TEST_VSWR)
       {
         vswr_mdsp_params.vswr_split_margin = RFCOMMON_MDSP_VSWR_SPLIT_MARGIN;
         rfcommon_mdsp_setup_self_test_vswr_params(&vswr_mdsp_params);
       } 

        api_status &= ftm_wcdma_xpt_iq_capture(device,
                                               device_fb,
                                               ftm_selftest_xpt_capture_type,
                                               num_samples,
                                               TRUE, /* first_trig */
                                               TRUE, /* last_trig */
                                               proc_flag,
                                               buffer_ids,
                                               samp_rate);
      }
    }
    #endif
    break;

  case FTM_RF_TECH_TDSCDMA:
    #ifdef FEATURE_TDSCDMA
    {
      if(api_status == TRUE)
      {

        /* Set Tx_freq_adj and Reset Rx FIR filter*/

        rfcommon_mdsp_setup_self_test_params(TRUE, rx_bw_filt_len, 0, NULL, (1<<2));
        if (ftm_selftest_xpt_capture_type == XPT_CAPTURE_SELF_TEST_VSWR)
        {
          vswr_mdsp_params.vswr_split_margin = RFCOMMON_MDSP_VSWR_SPLIT_MARGIN;
          rfcommon_mdsp_setup_self_test_vswr_params(&vswr_mdsp_params);
        } 

        api_status &= ftm_tdscdma_xpt_iq_capture(device,
                                                 device_fb,
                                                 ftm_selftest_xpt_capture_type,
                                                 num_samples,
                                                 TRUE, /* first_trig */
                                                 TRUE, /* last_trig */
                                                 proc_flag,
                                                 buffer_ids,
                                                 samp_rate);
      }
    }
    #endif
    break;

  case FTM_RF_TECH_LTE:
    #ifdef FEATURE_LTE
    {
      if(api_status == TRUE)
      {

        fbrx_carrier_idx = rflte_ftm_mc_get_carrier_index_from_device(device_fb);

        /* Get LTE FTM state machine handle */
        lte_ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();


       switch (lte_ftm_mc_sm_ptr->carrier[fbrx_carrier_idx].tx_bw)
       {
         case RFCOM_BW_LTE_5MHz:
           rx_bw_filt_coeff  = evm_lte_5_fir;
           rx_bw_filt_len = sizeof(evm_lte_5_fir)/sizeof(int16);		 
           break;
         case RFCOM_BW_LTE_10MHz:
           rx_bw_filt_coeff  = evm_lte_10_fir;
           rx_bw_filt_len = sizeof(evm_lte_10_fir)/sizeof(int16);		 
           break;
         case RFCOM_BW_LTE_15MHz:
           rx_bw_filt_coeff  = evm_lte_15_fir;
           rx_bw_filt_len = sizeof(evm_lte_15_fir)/sizeof(int16);		 
           break;
         case RFCOM_BW_LTE_20MHz:
           rx_bw_filt_coeff  = evm_lte_20_fir;
           rx_bw_filt_len = sizeof(evm_lte_20_fir)/sizeof(int16);		 
           break;
         default:
           rx_bw_filt_coeff  = NULL;
           rx_bw_filt_len = 0;
           FTM_MSG(FTM_HIGH,"LTE self-test param, no Rx filter applied");
       }

       /*select correct eq_filt mask based on rb number*/
       if (lte_ftm_mc_sm_ptr->carrier[fbrx_carrier_idx].rb_block < 36)
       {
         eq_filt_mask = ((1<<2)|(1<<1));
       }
       else if (lte_ftm_mc_sm_ptr->carrier[fbrx_carrier_idx].rb_block < 75)
       {
         eq_filt_mask = ((1<<3)|(1<<2));
       }
       else
       {
         eq_filt_mask = ((1<<4)|(1<<3)|(1<<2));
       }

       if (ftm_selftest_xpt_capture_type == XPT_CAPTURE_SELF_TEST_VSWR)
       {
         /* disable band limiting filter for VSWR processing */

         rfcommon_mdsp_setup_self_test_params(FALSE, 0, 0, NULL, 0);

       } else 
       {
         rfcommon_mdsp_setup_self_test_params(FALSE, rx_bw_filt_len, EVM_RX_FIR_QFACTOR, rx_bw_filt_coeff, eq_filt_mask);       
       }

       tx_upsamp = 2;
       tx_up_fir_len = 10;
       tx_up_fir_shift = -14;
       tx_up_fir = tx_up_filter_coef;

       rfcommon_mdsp_setup_self_test_upsampling_params (tx_upsamp,tx_up_fir_len, tx_up_fir_shift, tx_up_fir);
       if (ftm_selftest_xpt_capture_type == XPT_CAPTURE_SELF_TEST_VSWR)
       {
         vswr_mdsp_params.vswr_split_margin = RFCOMMON_MDSP_VSWR_SPLIT_MARGIN;
         rfcommon_mdsp_setup_self_test_vswr_params(&vswr_mdsp_params);
       } 


        api_status &= ftm_lte_xpt_iq_capture(device,
                                             device_fb,
                                             num_samples,
                                             TRUE,
                                             TRUE,
                                             proc_flag,
                                             ftm_selftest_xpt_capture_type,
                                             buffer_ids.capt_buf_id,
                                             buffer_ids.res_buf_id,
                                             samp_rate);
     }
    }
    #endif
    break;

  default:
    FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_self_test_iq_cap: Mode %d not supported",
               ftm_rf_tech );
    api_status = FALSE;
    break;
  } /* Trigger tech-specific capture */

  if ( api_status == FALSE)
  {
    FTM_MSG( FTM_ERROR, "ftm_common_trigger_self_test_iq_cap: api status = FAIL" );
  }
  return api_status;
}


// Commented out for 9x35 bringup
#if 0
/*===========================================================================

FUNCTION ftm_common_hdet_disable

DESCRIPTION
   This function will call the tech specific HDET disable or enable API.
   Different tech uses different mechanism to disable or enable HDET.

DEPENDENCIES

RETURN VALUE
   FALSE, fail to disable or enable HDET
   TRUE, successfully disable or enable HDET

SIDE EFFECTS
   None.

===========================================================================*/
boolean ftm_common_hdet_disable(ftm_rf_technology_type ftm_rf_tech, rfm_device_enum_type rfm_dev, boolean flag)
{
   boolean ret=FALSE;
   uint8 cnt=0;

   switch (ftm_rf_tech)
   {
   case FTM_RF_TECH_CDMA:
    #ifdef FEATURE_CDMA1X
    while (FALSE == ret)
    {
     ret = rfm_cdma_ftm_obtain_hdet_lock(rfm_dev,flag);

     /* Will try to obtain hdet lock maximum of 12 times before bailing out */
     if ((TRUE == ret) || (cnt > 12))
     {
      break;
     }

     /* Wait 200us before trying again */
     DALSYS_BusyWait(200);
     cnt++;
    }

    FTM_MSG_HIGH("FTM CDMA HDET: ena/dis=%d result=%d  cnt(200us/cnt)=%d", flag, ret, cnt);
    #endif
    break;
   case FTM_RF_TECH_WCDMA:
    break;
   case FTM_RF_TECH_GSM:
    break;
   case FTM_RF_TECH_LTE:
    #ifdef FEATURE_LTE
    ret = ftm_lte_disable_hdet(flag);
    #endif
    break;
   case FTM_RF_TECH_TDSCDMA:
    #ifdef FEATURE_TDSCDMA
    ret = ftm_tdscdma_disable_hdet(flag);

    if (TRUE == ret)
    {
       /* Wait for 5.1 ms */
     DALSYS_BusyWait(5100);
    }

      FTM_MSG_HIGH("FTM TDSCDMA HDET: ena/dis=%d result=%d  cnt=%d", flag, ret, cnt);
    #endif

    break;
   default:
    cnt=0;  /* Avoid compiler warning if cnt is not being used. */
    break;
   }

   return(ret);
}
#endif

boolean ftm_common_fbrx_iq_capture_processing
(
ftm_rf_technology_type ftm_rf_tech,
uint8 fbrx_chain,
uint32 ftm_selftest_capture_type,
uint16 num_samples,
int32 **iq_buf_rx,
int32 **iq_buf_tx,
uint16 *evm_value_ptr,
uint32 *rx_rms_mag_ptr,
uint32 *samp_rate,
ftm_common_selftest_fbrxgs_info_type * fbrx_gs_info
)
{
  uint8 iq_buffer_index;
  uint8 result_buffer_index      = 0;
  rfm_device_enum_type tx_device = RFM_DEVICE_0;
  boolean capture_trigerred      = FALSE;
  boolean capture_done           = FALSE;
  boolean processing_done        = FALSE;
  uint32 config_delay_ms         = 2;
  boolean api_status             = TRUE;
  rfdevice_rxtx_common_class *device_ptr = NULL;
  rfcommon_mdsp_selftest_vswr_meas_type vswr_meas;
  memset(&vswr_meas,0,sizeof(rfcommon_mdsp_selftest_vswr_meas_type));

  boolean do_processing = (ftm_selftest_capture_type > 0)? TRUE: FALSE;

  if ( (ftm_selftest_capture_type & ENABLE_ACLR_MEAS ) !=0 )
  {
    if ((ftm_selftest_capture_type & (ENABLE_EVM_MEAS | ENABLE_VSWR_MEAS_V2 | ENABLE_TX_PWR_MEAS) ) == 0)
    {
      do_processing = FALSE;
    }
  }

  // #if defined(FEATURE_CDMA) || defined(FEATURE_TDSCDMA)
  // const rfc_common_logical_device_params_type *device_params_ptr = NULL;
  // #endif

  /* HDET should have been already disabled in calibration state */
  // if (rfm_get_calibration_state() == FALSE)
  // {
  //   /* Disable HDET loop */
  //   ftm_common_hdet_disable(ftm_rf_tech, tx_device, TRUE);

  //   /* Wait for HDET to disable.  Temporary workaround for LTE HDET disable issue */
  //   ftm_clk_rex_wait(3);
  // }

  /* Allocate DPD sample capture buffer */
  rfcommon_mc_allocate_dpd_buffer();
  device_ptr = rfc_common_get_cmn_device_object(0);
  /* check for null pointer and throw error */
  if (device_ptr == NULL)
  {
    FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
    return FALSE;
  }

  /* Trigger tech-specific capture */
  switch ( ftm_rf_tech )
  {
  case FTM_RF_TECH_CDMA:
  #ifdef FEATURE_CDMA1X
  {
    fbrx_chain = RFM_DEVICE_1;
    ftm_1x_xpt_cal_config((rfm_device_enum_type)tx_device,fbrx_chain);

    // if (ftm_common_get_selftest_flag()) {

    //   /* if in self-test mode, override the default WTR and coupler gain */

    //   device_params_ptr = rfc_common_get_logical_device_params(RFM_DEVICE_0);

    //   if(device_params_ptr != NULL){

    //     ftm_common_set_coupler_attn(ftm_common_get_selftest_coupler_fb_attn_state());

    //     rfdevice_cmn_set_et_path(device_params_ptr->cmn_device,RFM_1X_MODE,ftm_common_get_selftest_wtr_fb_attn_state(),TRUE);
    //   }
    //  }
    }
  #endif
    break;

  case FTM_RF_TECH_WCDMA:
    #ifdef FEATURE_WCDMA
    {
      fbrx_chain = RFM_DEVICE_1;
      ftm_wcdma_xpt_cal_config((rfm_device_enum_type)tx_device,fbrx_chain, (uint8)FTM_XPT_CONFIG_EPT, TRUE); /* True for config, False for deconfig */

      // if (ftm_common_get_selftest_flag()) {

      //     ftm_common_set_coupler_attn(ftm_common_get_selftest_coupler_fb_attn_state());

      //     ftm_wcdma_feedback_setup(RFM_DEVICE_0,
      //                             RFM_DEVICE_1,
      //                             FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR,
      //                             ftm_common_get_selftest_wtr_fb_attn_state(),
      //                             TRUE,
      //                             FALSE,
      //                             TRUE);
      // }
    }
    #endif
    break;

  case FTM_RF_TECH_TDSCDMA:
    #ifdef FEATURE_TDSCDMA
    {
      fbrx_chain = RFM_DEVICE_1;
      ftm_tdscdma_xpt_cal_config((rfm_device_enum_type)tx_device, fbrx_chain,(uint8)FTM_XPT_CONFIG_EPT, TRUE); /* True for config, False for deconfig */


      // if (ftm_common_get_selftest_flag()) {
      //   device_params_ptr = rfc_common_get_logical_device_params(RFM_DEVICE_0);

      //   if(device_params_ptr != NULL){


      //     ftm_common_set_coupler_direction(tdscdma_coupler_direction);


      //     ftm_common_set_coupler_attn(ftm_common_get_selftest_coupler_fb_attn_state());

      //     rfdevice_cmn_set_et_path(device_params_ptr->cmn_device,RFM_TDSCDMA_MODE,ftm_common_get_selftest_wtr_fb_attn_state(),TRUE);
      //   }
      // }
    }
    #endif
    break;

  case FTM_RF_TECH_LTE:
  #ifdef FEATURE_LTE
  {
    tx_device = rflte_ftm_mc_sm_get_tx_device();
    ftm_lte_xpt_cal_configure((rfm_device_enum_type)tx_device,fbrx_chain, (uint8)FTM_XPT_CONFIG_ET);

     //rflte_ftm_mc_sm_set_use_fb_path(TRUE);


    // if (ftm_common_get_selftest_flag()) {

    //   /* if in self-test mode, override the default WTR and coupler gain */

    //   ftm_common_set_coupler_attn(ftm_common_get_selftest_coupler_fb_attn_state());

    //   rflte_ftm_mc_set_et_path(ftm_common_get_selftest_wtr_fb_attn_state(), TRUE);

    // }

  }
  #endif
    break;

  default:
    FTM_MSG_1( FTM_ERROR, "ftm_common_fbrx_iq_capture_processing: Mode %d not supported",
               ftm_rf_tech );
    api_status = FALSE;
    break;
  }

  if (fbrx_gs_info->override_enable)
  {
    rfdevice_cmn_override_fbrx_gain_state(device_ptr, 
                                        ftm_common_ftm_tech_to_rfm_mode(ftm_rf_tech), 
                                        fbrx_gs_info->com_band, 
                                        fbrx_gs_info->selftest_fbrx_gs, 
                                        fbrx_gs_info->wtr_lte_bw, 
                                        RFDEVICE_EXECUTE_IMMEDIATE,
                                        TRUE, 
                                        NULL
                                        );
  }


  ftm_clk_rex_wait(config_delay_ms);

  FTM_MSG_1(FTM_HIGH, "xpt_cal_configure  done", 0);

  if (api_status == TRUE)
  {
    /* trigger capture*/
    capture_trigerred = ftm_common_trigger_self_test_iq_cap(ftm_rf_tech,
                                                          tx_device,
                                                          fbrx_chain,
                                                          num_samples,
                                                          ftm_selftest_capture_type,
                                                          samp_rate);
  }

  /* Wait for capture done */
  if((capture_trigerred == TRUE) )
  {
    capture_done = rfcommon_xpt_wait_for_capture_done( 500, 5000, &iq_buffer_index );
    if (fbrx_gs_info->override_enable)
    {
      rfdevice_cmn_override_fbrx_gain_state(device_ptr, 
                                          ftm_common_ftm_tech_to_rfm_mode(ftm_rf_tech), 
                                          fbrx_gs_info->com_band, 
                                          fbrx_gs_info->selftest_fbrx_gs, 
                                          fbrx_gs_info->wtr_lte_bw, 
                                          RFDEVICE_EXECUTE_IMMEDIATE,
                                          FALSE, 
                                          NULL
                                          );
    }
    if( capture_done == FALSE )
    {
      FTM_MSG_1( FTM_ERROR, "ftm_lte_fbrx_iq_capture_trigger_handler: Sample "
               "capture did not complete", 0);
      return FALSE;
    }
  }
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_lte_fbrx_iq_capture_trigger_handler: Sample "
              "capture trigger failed", 0);
    if (fbrx_gs_info->override_enable)
    {
      rfdevice_cmn_override_fbrx_gain_state(device_ptr, 
                                          ftm_common_ftm_tech_to_rfm_mode(ftm_rf_tech), 
                                          fbrx_gs_info->com_band, 
                                          fbrx_gs_info->selftest_fbrx_gs, 
                                          fbrx_gs_info->wtr_lte_bw, 
                                          RFDEVICE_EXECUTE_IMMEDIATE,
                                          FALSE, 
                                          NULL
                                          );
    }    
    return FALSE;
  }

  /* fetch the collected samples*/
  rfcommon_mc_fetch_selftest_iq_data(iq_buffer_index, num_samples, iq_buf_rx, iq_buf_tx);

  if(do_processing)
  {
    /* Wait for processing done */
    processing_done = rfcommon_xpt_wait_for_processing_done( 1000, 35000, &result_buffer_index );

    if ( processing_done == FALSE)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_common_trigger_ept_iq_cap: "
                   "Processing did not complete for tech %d", ftm_rf_tech );
      return FALSE;
    }
    rfcommon_mc_fetch_selftest_meas(result_buffer_index, evm_value_ptr, rx_rms_mag_ptr);

    /* get VSWR split capture measurement */

    if ( (ftm_selftest_capture_type & ENABLE_VSWR_MEAS_V2 ) !=0 )
    {
      rfcommon_mdsp_selftest_vswr_meas(result_buffer_index, &vswr_meas);


      ftm_common_selftest_vswr_v2_results.vswr_v2_mag = (vswr_meas.refl_pwr -vswr_meas.fwd_pwr);

      ftm_common_selftest_vswr_v2_results.vswr_v2_phase = (vswr_meas.fwd_phase- vswr_meas.refl_phase + 54000) % 36000 - 18000;

      ftm_common_selftest_vswr_v2_results.vswr_v2_return_loss = - ftm_common_selftest_vswr_v2_results.vswr_v2_mag;

      ftm_common_selftest_vswr_v2_results.vswr_v2_fwd_pwr = vswr_meas.fwd_pwr;

      ftm_common_selftest_vswr_v2_results.vswr_v2_rev_pwr = vswr_meas.refl_pwr;


      FTM_MSG_6(FTM_HIGH, "Selftest VSWR results:"

                "VSWR_gain = %d dB100; VSWR_phase = %d Degree*100"
                            
                "fwd_phase = %d; " 
                                           
                "fwd_pwr = %d; " 
   
                "refl_phase = %d; " 
                                           
                "refl_pwr = %d; ",

                ftm_common_selftest_vswr_v2_results.vswr_v2_mag, 

                ftm_common_selftest_vswr_v2_results.vswr_v2_phase,
                            
                vswr_meas.fwd_phase,
                                           
                vswr_meas.fwd_pwr,
                                       
                                           
                vswr_meas.refl_phase,
                                           
                vswr_meas.refl_pwr);
     
    }
  }

  /* release capture buffer, returns false now, TODO: implement deallocation of buffer_id and return true */
  rfcommon_core_xpt_release_capture_buffer(iq_buffer_index);

  /* release results buffer, returns false now, TODO: implement deallocation of buffer_id and return true */
  rfcommon_core_xpt_release_results_buffer(result_buffer_index);

  // if (ftm_common_get_selftest_flag()) {

  //    if in self-test mode, override the default WTR and coupler gain

  //   ftm_common_set_coupler_attn(COUPLER_ATTN_NORMAL);

  // }

  /* release all buffer ids */
  rfcommon_core_xpt_free_all_buffers();

  /* HDET should have been already disabled in calibration state */
  // if (rfm_get_calibration_state() == FALSE)
  // {
  //   /* Release HDET loop */
  //   ftm_common_hdet_disable(ftm_rf_tech, tx_device, FALSE);
  // }

  return TRUE;
}

void ftm_common_selftest_set_flag( uint8 * flag_value)
{
  if (*flag_value != ftm_common_selftest_modeset_flag)
  {
    FTM_MSG_2(FTM_HIGH, "ftm_common_selftest_set_flag: current value %d set to %d",ftm_common_selftest_modeset_flag,*flag_value);
    ftm_common_selftest_modeset_flag = *flag_value;
  }
  else
  {
    FTM_MSG_1(FTM_HIGH, "ftm_common_selftest_set_flag: No Change to current value %d",ftm_common_selftest_modeset_flag);
  }
}


uint8 ftm_common_get_selftest_capture_flag(void)
{
  return ftm_common_selftest_modeset_flag;
}


boolean ftm_common_selftest_fbrx_noise_floor_nvwrite
(
  ftm_rf_technology_type tech, 
  uint8 nv_band, 
  rfcommon_nv_fbrx_noise_data_type * fbrx_noise_nv_struct
)
{
  boolean nv_valid = FALSE;
  rfnv_item_id_enum_type nv_item_to_write = RFNV_ITEMS_ENUM_MAX;
  nv_stat_enum_type nv_write_stat = NV_STAT_ENUM_MAX;

  nv_valid = ftm_common_selftest_get_fbrx_noise_nv_item(tech, nv_band, &nv_item_to_write);

  if (nv_valid == FALSE)
  {
    FTM_MSG_3(FTM_ERROR, "Failed to obtain NV item for tech %d and band %d, nv_valid flag : %d", tech, nv_band, nv_valid);
    return FALSE;
  }

  if ( nv_valid )
  {
    /* Write KV calibration code to NV */
    nv_write_stat = rfcommon_rfnv_set_item(nv_item_to_write,
                           (rfnv_item_type *)(fbrx_noise_nv_struct),
                           sizeof(rfcommon_nv_fbrx_noise_data_type),
                           NULL,
                           (rex_sigs_type)0,
                           NULL);
    if (nv_write_stat != NV_DONE_S)
    {
      FTM_MSG_1(FTM_ERROR, "Couldn't write NV, status: %d",nv_write_stat);
      return FALSE;
    }
  }
  else
  {
    FTM_MSG(FTM_ERROR, "NV location is not valid or the api failed, not writing NV!");
    return FALSE;
  }

  return TRUE;

}


boolean ftm_common_selftest_get_fbrx_noise_nv_item(  ftm_rf_technology_type tech, 
                                                     uint8 nv_band, 
                                                     rfnv_item_id_enum_type * nv_item_to_write)
{
  boolean nv_valid = FALSE;
  switch (tech)
  {
  case FTM_RF_TECH_CDMA:
    FTM_MSG(FTM_ERROR, "ftm_common_selftest_get_fbrx_noise_nv_item does not support NV items for CDMA tech!");
    break;
  case FTM_RF_TECH_WCDMA:
    nv_valid = ftm_common_selftest_get_wcdma_fbrx_noise_nv_item(nv_band, nv_item_to_write);
    break;
  case FTM_RF_TECH_GSM:
    FTM_MSG(FTM_ERROR, "ftm_common_selftest_get_fbrx_noise_nv_item does not support NV items for GSM tech!");
    break;
  case FTM_RF_TECH_LTE: 
    nv_valid = ftm_common_selftest_get_lte_fbrx_noise_nv_item(nv_band, nv_item_to_write);
    break;
  case FTM_RF_TECH_TDSCDMA:
    FTM_MSG(FTM_ERROR, "ftm_common_selftest_get_fbrx_noise_nv_item does not support NV items for TDSCDMA tech!");
    break;
  default:
    FTM_MSG_1(FTM_ERROR, "Unknown Tech %d, cannot get nv item!", tech);
  }

  return nv_valid;
}


static boolean ftm_common_selftest_get_lte_fbrx_noise_nv_item(uint8 nv_band, rfnv_item_id_enum_type * nv_item_to_write)
{
  boolean ret_value = TRUE;
  switch ((rfcom_lte_band_type)nv_band)
  {
  case   RFCOM_BAND_LTE_B1:
    *nv_item_to_write = RFNV_LTE_C0_B1_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B2:
    *nv_item_to_write = RFNV_LTE_C0_B2_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B3:
    *nv_item_to_write = RFNV_LTE_C0_B3_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B4:
    *nv_item_to_write = RFNV_LTE_C0_B4_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B5:
    *nv_item_to_write = RFNV_LTE_C0_B5_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B6:
    *nv_item_to_write = RFNV_LTE_C0_B6_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B7:
    *nv_item_to_write = RFNV_LTE_C0_B7_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B8:
    *nv_item_to_write = RFNV_LTE_C0_B8_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B9:
    *nv_item_to_write = RFNV_LTE_C0_B9_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B10:
    *nv_item_to_write = RFNV_LTE_C0_B10_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B11:
    *nv_item_to_write = RFNV_LTE_C0_B11_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B12:
    *nv_item_to_write = RFNV_LTE_C0_B12_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B13:
    *nv_item_to_write = RFNV_LTE_C0_B13_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B14:
    *nv_item_to_write = RFNV_LTE_C0_B14_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B15:
    *nv_item_to_write = RFNV_LTE_C0_B15_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B16:
    *nv_item_to_write = RFNV_LTE_C0_B16_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B17:
    *nv_item_to_write = RFNV_LTE_C0_B17_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B18:
    *nv_item_to_write = RFNV_LTE_C0_B18_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B19:
    *nv_item_to_write = RFNV_LTE_C0_B19_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B20:
    *nv_item_to_write = RFNV_LTE_C0_B20_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B21:
    *nv_item_to_write = RFNV_LTE_C0_B21_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B22:
    *nv_item_to_write = RFNV_LTE_C0_B22_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B23:
    *nv_item_to_write = RFNV_LTE_C0_B23_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B24:
    *nv_item_to_write = RFNV_LTE_C0_B24_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B25:
    *nv_item_to_write = RFNV_LTE_C0_B25_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B26:
    *nv_item_to_write = RFNV_LTE_C0_B26_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B27:
    *nv_item_to_write = RFNV_LTE_C0_B27_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B28:
    *nv_item_to_write = RFNV_LTE_C0_B28_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B28_B:
    *nv_item_to_write = RFNV_LTE_C0_B28_B_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B30:
    *nv_item_to_write = RFNV_LTE_C0_B30_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B31:
    *nv_item_to_write = RFNV_LTE_C0_B31_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B33:
    *nv_item_to_write = RFNV_LTE_C0_B33_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B34:
    *nv_item_to_write = RFNV_LTE_C0_B34_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B35:
    *nv_item_to_write = RFNV_LTE_C0_B35_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B36:
    *nv_item_to_write = RFNV_LTE_C0_B36_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B37:
    *nv_item_to_write = RFNV_LTE_C0_B37_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B38:
    *nv_item_to_write = RFNV_LTE_C0_B38_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B39:
    *nv_item_to_write = RFNV_LTE_C0_B39_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B39_B:
    *nv_item_to_write = RFNV_LTE_C0_B39_B_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B40:
    *nv_item_to_write = RFNV_LTE_C0_B40_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B40_B:
    *nv_item_to_write = RFNV_LTE_C0_B40_B_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B41:
    *nv_item_to_write = RFNV_LTE_C0_B41_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B41_B:
    *nv_item_to_write = RFNV_LTE_C0_B41_B_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B41_C:
    *nv_item_to_write = RFNV_LTE_C0_B41_C_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B42:
    *nv_item_to_write = RFNV_LTE_C0_B42_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B43:
    *nv_item_to_write = RFNV_LTE_C0_B43_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_LTE_B44:
    *nv_item_to_write = RFNV_LTE_C0_B44_FBRX_NOISE_DATA_I;
    break;
  default:
    *nv_item_to_write = RFNV_LTE_C0_B1_FBRX_NOISE_DATA_I;
    ret_value = FALSE;
  }

  return ret_value;
}


static boolean ftm_common_selftest_get_wcdma_fbrx_noise_nv_item(uint8 nv_band, rfnv_item_id_enum_type * nv_item_to_write)
{
  boolean ret_value = TRUE;
  switch ((rfcom_wcdma_band_type)nv_band)
  {
  case   RFCOM_BAND_IMT:
    *nv_item_to_write = RFNV_WCDMA_C0_B1_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_1900:
    *nv_item_to_write = RFNV_WCDMA_C0_B2_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_BC3:
    *nv_item_to_write = RFNV_WCDMA_C0_B3_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_BC4:
    *nv_item_to_write = RFNV_WCDMA_C0_B4_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_800:
    *nv_item_to_write = RFNV_WCDMA_C0_B5_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_BC8:
    *nv_item_to_write = RFNV_WCDMA_C0_B8_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_BC9:
    *nv_item_to_write = RFNV_WCDMA_C0_B9_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_BC11:
    *nv_item_to_write = RFNV_WCDMA_C0_B11_FBRX_NOISE_DATA_I;
    break;
  case   RFCOM_BAND_BC19:
    *nv_item_to_write = RFNV_WCDMA_C0_B19_FBRX_NOISE_DATA_I;
    break;
  default:
    *nv_item_to_write = RFNV_WCDMA_C0_B1_FBRX_NOISE_DATA_I;
    ret_value = FALSE;
  }

  return ret_value;
}




#endif /* FEATURE_FACTORY_TESTMODE */

