/*----------------------------------------------------------------------------*/
/*!

  @brief
  This is the FTM RF file which contains RF specific commands

Copyright (c) 1994-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/src/ftm_gsm_dispatch.c#1 $ //source/qcom/qct/modem/rftech/gsm/main/1.0/ftm/src/ftm_gsm_dispatch.c#9
  $DateTime: 2016/12/13 07:59:51 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/16   mz      FR 30990: SAWless LTE TDD B39: enable IIP2 CAL on GSM SAW device
12/11/14   zbz     Added command to return both PRx and DRx RSSI
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx
                   burst along with GRFC status
11/14/14   ck      Add FTM feature to set pa bias NVs via diag
11/13/14   ck      add capability to disable/enable gsm diversity
10/30/14   ck      move ftm concurrency manager to ftm gsm enter for future
10/30/14   zbz     Add new FTM command to return DRx RSSI
10/28/14   ck      unhook concurrency manager to avoid being blocked
09/24/14   ch/ndb  Passing device info to tuner tune over-ride dispatch API
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/14/14   ec      Add SAWLess control APIs to dispatchers
05/29/14   sb      IP2 cal bug fixes, updates
04/16/14   sb      IP2 cal Interface and functionality changes
03/31/14   tsr     Add support for GSM RX Diversity
03/26/14   sb      Add support for IP2 cal for Dime PM
08/05/13   cj      Implementation added to receive GSM RSSI in meas mode
05/15/13   svi     Fixed GSM RF FTM failure due to unexpected diag return info.
05/06/13   aka     Changes to add payload length as a parameter in FRAME SWEEP APIs
04/23/13   ck      Add FTM IQ capture APIs to move from HWTC to FTM
04/17/13   aka     Changes to support predist override in FTM Mode
04/16/13   svi     Replaced memcpy() by safer memscpy().
04/03/13   ec	   Changes how ftm_gsm_rfctl_vars are initialised
04/02/13   aka     Allocate result buffer dynamically to avoid stack overflow
03/28/13   aka     GSM frame sweep feature changes.
02/21/13   tws     Fix KW errors.
02/19/13   tws     The ftm gsm vars are now created during enter_mode.
02/15/13   sb      Added FTM hook to override tuner tune code
01/18/13   tws     Remove include msm.h
12/05/12   aka     Added rfm_dev in ftm_gsm_get_env_gain_nv
11/27/12   tws     Add device id & as_id to FTM.
11/19/12   aka     Added ftm_gsm_env_gain_nv_data in GSM union
07/20/12   sr      Made changes to pass the RFM_DEVICE to rfc gsm functions
05/31/12   zc      Changing order of when result.rssi gets populated in FTM_GSM_GET_MEAS_PWR
04/18/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API
04/03/12   ggs     Added ftm_gsm_msgr_dispatch_response_handler for handling
                   response messages sent from the GSM module in RF task
02/28/12   sb      Added changes to support disabling Idle frame in FTM mode
01/16/12   sr      Made changes not todo idle_frame processing
                   (temp comp and vbatt comp) during calibration.
11/29/11   sb      Added functionality for GSM IQ CAPTURE
11/17/11   av     Added RXCal verification in FTM
09/08/11   rsr     Port Non-IQ cal feature from older targets
08/29/11   sr      KW fixes.
07/07/11   vrb     Support for KV Cal V2
05/09/11   sar     Included ftm_rf_cmd.h for rfa/api cleanup.
03/24/11   vrb     Compiler Warning Fixes
03/16/11   sr      Removed un-used code.
03/11/11   sr      Removed un-used featurization
03/03/11   sr      off-target error fix.
03/03/11   sr      merge from mdm9k branch
31/01/11   sr      Removed un-used code
11/26/10   ggs     Added support for enhanced AMAM cal (swapped waveform)
08/03/10   ggs     Replaced ftm_polar.h with rfgsm_mdsp_cal.h. Replaced
                   ftm_polar_gen_amam_linear_sweep with rfgsm_mdsp_cal_gen_amam_linear_sweep
07/27/10   ggs     Fixed compiler warnings
04/05/10   rsr     Added feature for Multislot RX.
01/20/10   tws     Use PACK(...) instead of PACKED for qdsp6 compatibility.
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr      changed the debug msg macro from legacy to new one.
12/08/09   sr      removed un-used code.
12/02/09   sr      removed im2 related code since we don't need it for now.
10/25/09   dyc     Featurized ftm_polar.h with FEATURE_POLAR
10/12/09   sr      Fixed warnings.
08/21/09   sar     Updated ADC interface for CMI conformance.
03/31/09   sr      include file cleanup
10/26/07   rsr     Added case for FTM_SET_SMPS_PDM_TBL is dispatch function
08/08/07   jfc     Mainlined T_MSM6280 feature
06/13/07   jfc     Added FTM_TX_CS_SWEEP case in ftm_gsm_extended_dispatch()
10/17/06   ycl     Modifications for initial MSM7600 build.
03/28/06   rsr     Lint cleanup.
                   Changed RX burst API to accept the number of frames over which
                   RSSI is averaged.
                   Added new command to perform RX calibration for all gain ranges.
03/15/06   rsr     Added command for TX RSB sweep calibration.
03/14/06   jac     Added dispatch for GSM IM2 Cal command for MSM6280.
02/27/06   ka      Updated for 6280.
05/26/05   jfc     Added additional 5ms wait before returning RSSI.
05/04/05   rsr     Removed warnings.Cleanup
04/14/05   rsr     Made changes to allow external cal to use DSP waveform generation.
03/30/05   lcl     Removed modulator configure API.
03/11/05   ka      Added support for gsm extended commands.
03/04/05   rsr     Cleaned up API's. Added OPLL calibration API.
02/09/05   rsr/lcl Renamed FEATURE_POLAR_RF to FEATURE_POLAR
12/09/04   rsr     Added support for autocalibration
10/18/04   fpe     Removed FTM_HAS_ASYNC_PKT
5/6/04     fpe     Added gsm commands to ftm_id_type and removed ftm_gsm_id_type
12/15/03   jwh     Added resp to ftm_mc_dispatch() for FTM_HAS_ASYNC_PKT.
04/11/03   rsr     Renamed functions with "ftm_" prefix
03/18/03   lcl     Removed MSM6200 specific header file.
03/15/03   lcl     Added FTM_ to ftm_gsm_id_type.
01/09/03   prk     Removed call to dog_report(DOG_ALL_TASKS_RPT).
12/12/02   xw      Commented out gsm_tx_cont_command due to reliability issue
12/10/02   xw      Code clean up
08-21-02   rjr     Created
*/

/*------------------------------------------------------------------------------
                               INCLUDES
------------------------------------------------------------------------------*/

#include "rfa_variation.h"
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "stringl.h"
#include "rfcom.h"
#include "ftmicap.h"
#include "ftm_rf_cmd.h"

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef FEATURE_GSM

#include "ftm.h"
#include "ftm_gsm_dispatch.h"
#include "ftm_gsm_rfctl.h"
#include "ftm_common.h"
#include "ftm_common_control.h"
#include "ftm_gsm.h"
#include "ftm_gsm_prot.h"
#include "ftm_gsm_rfprot.h"
#include "rf_test_task_v.h"
#include "rfgsm_mdsp_cal.h" /* Replaces ftm_polar.h  */
#include "dog.h"
#include "rfc_card_gsm.h"
#include "ftm_common_concurrency_manager.h"

typedef PACK(union)
{
  ftm_gsm_extended_result_type     gen_result;
  ftm_gsm_detector_cal_result_type det_result;
  ftm_gsm_amam_sweep_return_type amam_result;
  ftm_gsm_rssi_to_dbm_return_type rssi_to_dbm_result; /*Added from SCMM*/
  ftm_gsm_rssi_return_type rssi_result;
  ftm_gsm_env_gain_nv_data_type  ftm_gsm_env_gain_nv_data;
  ftm_gsm_frame_sweep_result_buffer_type frame_sweep_result;
  ftm_gsm_rssi_prx_drx_return_type rssi_prx_drx_result;
} ftm_gsm_extended_result_union;

ftm_gsm_meas_results g_meas_results = {FALSE, 0};

/*------------------------------------------------------------------------------
                       EXTERNAL VARIABLES
------------------------------------------------------------------------------*/



int16 ftm_gsm_rssi_to_dbm = 0; /*Added from SCMM*/
/*------------------------------------------------------------------------------
                       EXTERNAL FUNCTIONS
------------------------------------------------------------------------------*/
extern void ftm_gsm_configure_and_start_polar_tx_sweep_cal(ftm_gsm_sweep_waveform_type *waveform_ptr,
                                                           uint16 num_steps);

/*------------------------------------------------------------------------------
                      PUBLIC FUNCTION IMPLEMENTATIONS
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles requests to run GSM tests and other primitives
   by dispatching the appropriate functions.

   @param
   cmd_ptr -

   @retval
   Response packet
*/
ftm_rsp_pkt_type ftm_gsm_dispatch( rfm_device_enum_type rfm_dev,
                                   ftm_pkt_type  *cmd_ptr )
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_gsm_rfctl_vars_s * vars_ptr = NULL;
  ftm_common_cmd_id_type cmd_type = (ftm_common_cmd_id_type)(cmd_ptr->rf_params.ftm_rf_cmd);
  boolean check = FALSE;

  /*Allow below commands to be run in online mode as well*/
  if((cmd_type == FTM_READ_DEVICE_STATUS) ||
     (cmd_type == FTM_READ_DEVICE_STATUS_ENABLE ) ||
     (cmd_type == FTM_READ_DEVICE_STATUS_DISABLE )||
     (cmd_type == FTM_READ_GRFC_DEVICE_STATUS_ENABLE )||
     (cmd_type == FTM_READ_GRFC_DEVICE_STATUS_DISABLE ))
  {
    check = TRUE;
  }

  if(check == FALSE)
  {
    vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);
    if( vars_ptr == NULL )
    {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "GSM FTM variables are NULL for device %d", rfm_dev);
      return rsp_pkt;
    }
  }

  switch ( (ftm_common_cmd_id_type)(cmd_ptr->rf_params.ftm_rf_cmd) )
  {
  case FTM_SET_MODE:
    ftm_gsm_set_mode( rfm_dev,
                      (ftm_mode_id_type) cmd_ptr->rf_params.ftm_rf_factory_data.mode );
    break;

  case FTM_SET_TX_ON:
    ftm_gsm_set_tx( rfm_dev, ON);
    break;

  case FTM_SET_TX_OFF:
    ftm_gsm_set_tx( rfm_dev, OFF);
    break;

  case FTM_SET_LNA_RANGE:
    ftm_gsm_set_lna_range( rfm_dev,
                           (ftm_lna_range_type) cmd_ptr->rf_params.ftm_rf_factory_data.range );
	/* Also set the LNA range for RXD device*/
    ftm_gsm_set_lna_range( (rfm_dev+1),
                           (ftm_lna_range_type) cmd_ptr->rf_params.ftm_rf_factory_data.range );
	break;
  case FTM_SET_PDM:
    ftm_gsm_set_pdm( rfm_dev, &(cmd_ptr->rf_params.ftm_rf_factory_data).id_val);
    break;

  case FTM_SET_CHAN:
    ftm_gsm_set_arfcn( rfm_dev,
                       cmd_ptr->rf_params.ftm_rf_factory_data.chan );
    break;

    /*  16  - Transmits a continuous waveform (rnd data,PN seq, tone) */
  case FTM_SET_TRANSMIT_CONT:
    ftm_gsm_tx_cont_command( rfm_dev,
                             &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union).ftm_tx_burst );
    break;

    /*  17  - Transmits a specified (or infinite) number of TX bursts                  */
  case FTM_SET_TRANSMIT_BURST:
    ftm_gsm_tx_burst_command( rfm_dev,
                              &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union).ftm_tx_burst );
    break;

    /*  25  - Start to receive or stop receiving Rx burst                              */
  case FTM_SET_RX_BURST:
    ftm_gsm_rx_burst_command( rfm_dev,
                              &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union).ftm_rx_burst,
                              FTM_DIV_FACTOR );
    break;

    /*  30  - Get Raw RSSI                                 */
  case FTM_GET_GSM_RSSI:
    {
      ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);

      ftm_clk_rex_wait(FTM_DIV_FACTOR*5+5);  // wait FTM_DIV_FACTOR frames, plus 5ms more to be safe
      cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_rssi.rssi = vars_ptr->ftm_gsm_state.ftm_rssi;
    }
    break;

  case FTM_GET_GSM_PRX_DRX_RSSI:
    {
      ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);

      ftm_clk_rex_wait(FTM_DIV_FACTOR*5+5);  // wait FTM_DIV_FACTOR frames, plus 5ms more to be safe
      cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_rssi.rssi = vars_ptr->ftm_gsm_state.ftm_rssi;
      cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_rssi.rssi_rxd = vars_ptr->ftm_gsm_state.ftm_rssi_drx;
    }
    break;

    /*  31 - SET_PA_START_DELTA  */
  case FTM_SET_PA_START_DELTA:
    ftm_gsm_set_pa_start_delta( rfm_dev,
                                (int16)cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.generic_16bit_u);
    break;

    /*  32 - SET_PA_STOP_DELTA */
  case FTM_SET_PA_STOP_DELTA:
    ftm_gsm_set_pa_stop_delta( rfm_dev,
                               (int16)cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.generic_16bit_u);
    break;


    /*  34 - SET_RX_CONTINUOUS */
  case FTM_SET_RX_CONTINUOUS:
    ftm_gsm_rx_burst_command( rfm_dev,
                              &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union).ftm_rx_burst,
                              FTM_DIV_FACTOR );
    break;

    /* 37 */
  case FTM_SET_PATH_DELAY:
    cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.generic_32bit_u =
    ftm_gsm_set_delay_match_cfg( rfm_dev,
                             (int32)cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.generic_32bit_u);
    break;

    /* 40 */
  case FTM_SET_TX_FRAME_MATRIX:
    ftm_gsm_set_frame_matrix( rfm_dev,
                          &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union).ftm_frame_matrix);
    break;

    /* 46 */
  case FTM_RX_GAIN_RANGE_CAL:
    vars_ptr->ftm_gsm_cal_flag = TRUE;
    ftm_gsm_do_rx_calibration_prx( rfm_dev,
                               &cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_rxcal_parms);
    vars_ptr->ftm_gsm_cal_flag = FALSE;
    break;
  case FTM_RX_GAIN_RANGE_PRX_DRX_CAL:
    vars_ptr->ftm_gsm_cal_flag = TRUE;
    ftm_gsm_do_rx_calibration_prx_drx( rfm_dev,
                                       &cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_rxcal_prx_drx_parms);
    vars_ptr->ftm_gsm_cal_flag = FALSE;
    break;

    /* 51 */
  case FTM_TX_KV_CAL: /* review the kv cal methods we have and go with the one method */
    vars_ptr->ftm_gsm_cal_flag = TRUE;
    ftm_gsm_do_tx_kv_cal( rfm_dev,
                          &cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.generic_2x16bit_u );
    vars_ptr->ftm_gsm_cal_flag = FALSE;
    break;

  case FTM_GSM_TX_KV_CAL_V2:
    vars_ptr->ftm_gsm_cal_flag = TRUE;
    ftm_gsm_do_tx_kv_cal_v2( rfm_dev,
                             &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union).ftm_kv_val_v2_params );
    vars_ptr->ftm_gsm_cal_flag = FALSE;
   break;

  case FTM_SET_GSM_LINEAR_PA_RANGE:
    ftm_gsm_set_pa_range ( rfm_dev,
                       &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_pa_range_params));
  break;

  case FTM_GSM_SET_LINEAR_RGI:
      ftm_gsm_set_linear_rgi( rfm_dev,
                              &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_lin_rgi_params));
  break;

  case FTM_GSM_SET_TX_SLOT_CONFIG:
	  ftm_gsm_set_tx_slot_config( rfm_dev,
                              &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_gsm_tx_slot_config));
  break;

  case FTM_GSM_SET_PREDIST_OVERRIDE:
      ftm_gsm_set_predist_override( rfm_dev, cmd_ptr->rf_params.ftm_rf_factory_data.on_off);

  break;

  case FTM_GSM_CAPTURE_IQ:
      ftm_gsm_start_stop_iq_capture( rfm_dev,
                                     (word)cmd_ptr->rf_params.ftm_rf_factory_data.gen_w);
  break;

  case FTM_IQ_CAPTURE_GET_CONFIG:
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_config.ftm_error_code =
      ftm_gsm_iq_capture_get_config(&cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_config);
  break;

  case FTM_IQ_CAPTURE_ACQUIRE_SAMPLES:

      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples.ftm_error_code =
      ftm_gsm_iq_capture_acquire_samples(rfm_dev, &cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples);
  break;

  case FTM_IQ_CAPTURE_GET_SAMPLES:
      ftm_gsm_iq_capture_get_samples(rfm_dev, &cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_samples,&rsp_pkt);
  break;

  case FTM_IQ_CAPTURE_ANALYZE_SAMPLES:
      FTM_MSG_HIGH("The cmd is yet to be implemented...", 0, 0, 0);
  break;

  case FTM_SET_FREQ_ADJUST:
      ftm_gsm_override_gl1_freq_err( rfm_dev );
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.total_freq_adjust_hz.total_freq_adjust_hz_gsm =
            ftm_gsm_set_freq_err( rfm_dev,
                                  cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.user_freq_adjust_ppb.user_freq_adjust_ppb_gsm,
                                  cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.center_frequency_hz.center_frequency_hz_gsm,
                                  cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.enable_xo_freq_adjust
                                );
  break;


  case FTM_SET_TUNER_TUNE_CODE_OVERRIDE:
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.result =(ftm_tuner_override_status_type)ftm_gsm_rf_task_tuner_override(rfm_dev,
                                                                                                                                                     (uint8)cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.override_flag,
                                                                                                                                                     (void*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.config_data),
                                                                                                                                                     (uint8*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.nv_data),
                                                                                                                                                     (uint8*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.tuner_id));

  break;

  case FTM_GSM_DO_IP2_CAL:
      FTM_MSG_HIGH("FTM_GSM_DO_IP2_CAL...", 0, 0, 0);
      /*Allocate response packet*/
      rsp_pkt = ftm_common_malloc_rsp_pkt(cmd_ptr, sizeof(ftm_composite_cmd_header_type)+sizeof(ftm_gsm_ip2_cal_resp_type));
      ftm_gsm_set_ip2_cal_params_override(rfm_dev, cmd_ptr, &rsp_pkt);
  break;

  case FTM_GSM_GET_IP2_CAL_RESULTS:
       /*Changes pending*/
//      ftm_gsm_get_ip2_cal_results(rfm_dev,
//                                  (uint8*)&(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.generic_8bit_u),
//                                  0,
//                                  (int16*)rsp_pkt.pkt_payload[0],
//                                  (int16*)rsp_pkt.pkt_payload[1],
//                                  (int32*)rsp_pkt.pkt_payload[2] );
//
//      rsp_pkt.pkt_len        = iRequestPacketSize;
//      rsp_pkt.pkt_payload    = pRequestBytes;
//      rsp_pkt.delete_payload = FALSE;
  break;

  case FTM_GSM_DISABLE_IDLE_FRAME:
    vars_ptr->ftm_gsm_idle_proc_flag = (word)cmd_ptr->rf_params.ftm_rf_factory_data.gen_w;
  break;

  case FTM_GSM_SAWLESS_LIN_OVERRIDE:
    (void)ftm_gsm_sawless_lin_override( rfm_dev,
                                        (ftm_gsm_sawless_lin_type*)&(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_gsm_sawless_lin_params) );
  break;

  case FTM_GSM_GET_SAWLESS_LIN_MODE:
    (void)ftm_gsm_get_sawless_lin_mode( rfm_dev,
                                        (ftm_gsm_sawless_lin_type*)&(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_gsm_sawless_lin_params) );
  break;
  case FTM_SET_SECONDARY_CHAIN:
    ftm_gsm_set_diversity(rfm_dev, (word)cmd_ptr->rf_params.ftm_rf_factory_data.gen_w);
  break;

  case FTM_GSM_SET_PA_BIAS_NV:
    cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_set_pa_bias_nv_params.error_code =
    ftm_gsm_set_pa_bias_nv(rfm_dev,
	                   cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_set_pa_bias_nv_params.band,
	                   &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_set_pa_bias_nv_params.online_smps[0][0]),
	                   cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_set_pa_bias_nv_params.num_pa_state,
	                   &(cmd_ptr->rf_params.ftm_rf_factory_data.gsm_struct.ftm_gsm_data_union.ftm_set_pa_bias_nv_params.ftm_gsm_pa_bias_params[0]));
    break;

  case FTM_READ_DEVICE_STATUS:
  	(void)ftm_gsm_get_device_status_reg_list( rfm_dev,
                                                  (ftm_read_device_status*)&(cmd_ptr->rf_params.ftm_rf_factory_data.device_status) );
  break;
  case FTM_READ_DEVICE_STATUS_ENABLE:
    (void)ftm_gsm_set_device_status_enable( rfm_dev,
                                            (rf_burst_type*)&(cmd_ptr->rf_params.ftm_rf_factory_data.device_status_read_burst_type), TRUE );
  break;
  case FTM_READ_DEVICE_STATUS_DISABLE:
  	(void)ftm_gsm_set_device_status_enable( rfm_dev,
                                                (rf_burst_type*)&(cmd_ptr->rf_params.ftm_rf_factory_data.device_status_read_burst_type), FALSE );
  break;
  case FTM_READ_GRFC_DEVICE_STATUS_ENABLE:
	(void)ftm_gsm_set_grfc_status_read_enable( rfm_dev,
                                                   (rf_burst_type*)&(cmd_ptr->rf_params.ftm_rf_factory_data.device_status_read_burst_type), TRUE );
  break;
  case FTM_READ_GRFC_DEVICE_STATUS_DISABLE:
	(void)ftm_gsm_set_grfc_status_read_enable( rfm_dev,
                                                   (rf_burst_type*)&(cmd_ptr->rf_params.ftm_rf_factory_data.device_status_read_burst_type), FALSE );
  break;

    /*  Illegal, or unknown GSM test id  */
  default:
    break;
  }

  return rsp_pkt;
} /* end ftm_gsm_dispatch */

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function is used to handle RF GSM Responses which are received via
   Message Router.
   For the initial implementation, all this function does is print the message ID.
   Future enhancements may require additional handling/processing of the response message.

   @param
   msg_ptr, msg_size

   @retval
   void
*/
void ftm_gsm_msgr_dispatch_response_handler
(
    rfgsm_ftm_msg_u *msg_ptr,
    uint32  msg_size

)
{
  msgr_hdr_s   *msgr_hdr_ptr;

  msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr;

  switch (msgr_hdr_ptr->id)
  {
  case RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_RSP:
       MSG_1(MSG_SSID_FTM, MSG_LEGACY_LOW,
       "RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_RSP: 0x%x UMID received in ftm_gsm_dispatch_handler", msgr_hdr_ptr->id );
       break;

/*! @todo The remainder of these responses are placeholders for possible future use */
  case RFA_RF_GSM_FTM_SET_MODE_RSP:
  case RFA_RF_GSM_FTM_SET_TX_ON_RSP:
  case RFA_RF_GSM_FTM_SET_TX_OFF_RSP:
  case RFA_RF_GSM_FTM_SET_LNA_RANGE_RSP:
  case RFA_RF_GSM_FTM_SET_PDM_RSP:
  case RFA_RF_GSM_FTM_SET_BAND_RSP:
  case RFA_RF_GSM_FTM_SET_TRANSMIT_CONT_RSP:
  case RFA_RF_GSM_FTM_SET_TRANSMIT_BURST_RSP:
  case RFA_RF_GSM_FTM_SET_RX_BURST_RSP:
  case RFA_RF_GSM_FTM_GET_RSSI_RSP:
  case RFA_RF_GSM_FTM_SET_PA_START_DELTA_RSP:
  case RFA_RF_GSM_FTM_SET_PA_STOP_DELTA_RSP:
  case RFA_RF_GSM_FTM_SET_PA_DAC_INPUT_RSP:
  case RFA_RF_GSM_FTM_SET_RX_CONTINUOUS_RSP:
  case RFA_RF_GSM_FTM_SET_PATH_DELAY_RSP:
  case RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_RSP:
  case RFA_RF_GSM_FTM_RX_GAIN_RANGE_CAL_RSP:
  case RFA_RF_GSM_FTM_TX_KV_CAL_RSP:
  case RFA_RF_GSM_FTM_TX_KV_CAL_V2_RSP:
  case RFA_RF_GSM_FTM_SET_LINEAR_RGI_RSP:
  case RFA_RF_GSM_FTM_CAPTURE_IQ_RSP:
  case RFA_RF_GSM_FTM_SET_RX_TIMING_RSP: /* Legacy ftm_gsm_extended dispatch commands */
  case RFA_RF_GSM_FTM_SET_TX_TIMING_RSP:
  case RFA_RF_GSM_FTM_SET_TX_GAIN_SWEEP_RSP:
  case RFA_RF_GSM_FTM_TX_ENVDC_CS_SWEEP_RSP:
  case RFA_RF_GSM_FTM_CFG2_AMAM_SWEEP_V2_RSP:
  case RFA_RF_GSM_FTM_SET_RX_MULTISLOT_RSP:
  case RFA_RF_GSM_FTM_SET_RX_BURST_FOR_EXPECTED_PWR_RSP:
  case RFA_RF_GSM_FTM_GET_MEAS_PWR_RSP:
  case RFA_RF_GSM_FTM_SET_TX_POW_DBM_RSP:
  case RFA_RF_GSM_FTM_TUNE_TX_RSP:
  case RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_RSP:
  case RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_RSP:
  case RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_RSP:
       MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH, "RSP message UMID=0x%x received in ftm_gsm_msgr_dispatch_response_handler", msgr_hdr_ptr->id );
       break;
  default:
       MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "Bad RSP message received: UMID=0x%x, in ftm_gsm_msgr_dispatch_response_handler", msgr_hdr_ptr->id );
       break;
  }

}/* end of ftm_gsm_msgr_dispatch_response_handler */
/*!
   @brief
   This function handles requests to run GSM tests and other primitives
   by dispatching the appropriate functions.

   @param
   cmd_ptr -

   @retval
   Response packet
*/
ftm_rsp_pkt_type ftm_gsm_extended_dispatch( rfm_device_enum_type rfm_dev,
                                            ftm_gsm_extended_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_composite_cmd_header_type  rsp_headers;
  /* local_buffer and extra_data_len allow return data size to depend on a
     request parameter. Unused for now */
  byte                           *local_buffer  = NULL;
  uint16                         extra_data_len = 0;
  uint8                           *buff_ptr;
  /* buffer to hold result data*/
  ftm_gsm_extended_result_union   *result_buff = NULL;
  ftm_gsm_rfctl_vars_s * vars_ptr = NULL;

  /* If the FTM variables have not been initialised, do so now */
  if ( !ftm_gsm_vars_created(rfm_dev) )
  {
    vars_ptr = ftm_gsm_create_and_init_vars(rfm_dev);
  }
  else
  {
    vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);
  }

  if( vars_ptr == NULL )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "GSM FTM variables are NULL for device %d", rfm_dev);
    return rsp_pkt;
  }

  result_buff = (ftm_gsm_extended_result_union *)ftm_malloc(sizeof(ftm_gsm_extended_result_union));

  if (result_buff == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"failed to allocate memory for RSP packet", 0);
    return rsp_pkt;
  }

  /* Initialize response packet size to length of the common headers */
  /* the size of the return parameters will be added in the dispatch cases below */
  rsp_headers.ftm_hdr.cmd_rsp_pkt_size = sizeof(ftm_composite_cmd_header_type);

  buff_ptr = (uint8*)&(cmd_ptr->ftm_gsm_extended_params[0]);

  switch ( cmd_ptr->ftm_hdr.cmd_id )
  {
  case FTM_GSM_SET_RX_TIMING:
    (void)rfc_gsm_command_dispatch( rfm_dev,
                                    (int)RFC_GSM_DEBUG_UPDATE_RX_TIMING,
                                    (void *)buff_ptr);
    result_buff->amam_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_gsm_amam_sweep_return_type);
    break;

  case FTM_GSM_SET_TX_TIMING:
    (void)rfc_gsm_command_dispatch(rfm_dev,
                                   (int)RFC_GSM_DEBUG_UPDATE_TX_TIMING,
                                   (void *)buff_ptr);
    result_buff->amam_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_gsm_amam_sweep_return_type);
    break;

  case FTM_GSM_TX_GAIN_SWEEP:
    vars_ptr->ftm_gsm_cal_flag = TRUE;
    ftm_gen_tx_da_sweep( rfm_dev,
                         (ftm_linear_da_sweep_data_type *)buff_ptr,
                         ((uint16)( cmd_ptr->ftm_hdr.cmd_data_len / ( 5 * sizeof(uint8) )  )) );
    vars_ptr->ftm_gsm_cal_flag = FALSE;
    result_buff->gen_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_gsm_extended_result_type);
    break;

    //@todo: anupamav: fix later: gives compilation error
  case FTM_GSM_TX_ENVDC_CS_SWEEP:
    vars_ptr->ftm_gsm_cal_flag = TRUE;
    ftm_gen_tx_envdc_cs_sweep( rfm_dev,
                               (ftm_linear_envdc_cs_data_type *)buff_ptr,
                               ((uint16)( cmd_ptr->ftm_hdr.cmd_data_len / ( 2 * sizeof(uint16) )  )) );
    vars_ptr->ftm_gsm_cal_flag = FALSE;
    result_buff->gen_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_gsm_extended_result_type);
    break;

  case FTM_GSM_RSSI_TO_DB: /*Added from SCMM*/
    result_buff->rssi_to_dbm_result.db_x16 = ftm_gsm_rssi_to_dbm;
    result_buff->rssi_to_dbm_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_gsm_rssi_to_dbm_return_type);
    break;

  /* AMAM Sweep V2 (First Step: 8PSK extended; Second Step: DC) */
  case FTM_TX_CFG2_AMAM_SWEEP_V2:
    vars_ptr->ftm_gsm_cal_flag = TRUE;
    result_buff->amam_result.pa_ramp_max = rfgsm_mdsp_cal_gen_amam_linear_sweep_v2 ( rfm_dev,
                                                                               (uint16*) buff_ptr);
    vars_ptr->ftm_gsm_cal_flag = FALSE;
    result_buff->amam_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_gsm_amam_sweep_return_type);
    break;

  case FTM_GSM_SET_RX_MULTISLOT :
    ftm_gsm_set_multislot_rx( rfm_dev, (uint8*) buff_ptr);
    result_buff->gen_result.error_code = (int32)FTM_SUCCESS;
    break;

  case FTM_GSM_SETUP_RX_BURST_FOR_EXPECTED_PWR:
    ftm_gsm_core_setup_rx_burst_calver( rfm_dev, (int16*) buff_ptr);
    result_buff->gen_result.error_code = FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size+=sizeof(ftm_gsm_extended_result_type);
    break;

  case FTM_GSM_GET_MEAS_PWR:
    if(g_meas_results.x2g_irat_enabled)
    {
      result_buff->rssi_result.db_x16 = g_meas_results.db_x16;
      result_buff->rssi_result.raw_rssi = g_meas_results.db_x16;
      result_buff->rssi_result.error_code = (int32)FTM_SUCCESS;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size +=sizeof(ftm_gsm_rssi_return_type);
      g_meas_results.x2g_irat_enabled = FALSE;
    }
    else
    {
      result_buff->rssi_result.db_x16 = ftm_gsm_get_rssi_dbx16(rfm_dev);
      result_buff->rssi_result.raw_rssi = ftm_gsm_get_vars_ptr(rfm_dev)->ftm_gsm_state.ftm_rssi;
      result_buff->rssi_result.error_code = (int32)FTM_SUCCESS;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size +=sizeof(ftm_gsm_rssi_return_type);
    }
    break;

  case FTM_GSM_GET_MEAS_PWR_DRX:
    result_buff->rssi_result.db_x16 = ftm_gsm_get_rssi_dbx16(rfgsm_core_get_drx_dev(rfm_dev));
    result_buff->rssi_result.raw_rssi = ftm_gsm_get_vars_ptr(rfm_dev)->ftm_gsm_state.ftm_rssi_drx;
    result_buff->rssi_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size +=sizeof(ftm_gsm_rssi_return_type);
    break;

  case FTM_GSM_GET_MEAS_PWR_PRX_DRX:
    result_buff->rssi_prx_drx_result.db_x16_prx = ftm_gsm_get_rssi_dbx16(rfm_dev);
    result_buff->rssi_prx_drx_result.raw_rssi_prx = ftm_gsm_get_vars_ptr(rfm_dev)->ftm_gsm_state.ftm_rssi;
    result_buff->rssi_prx_drx_result.db_x16_drx = ftm_gsm_get_rssi_dbx16(rfgsm_core_get_drx_dev(rfm_dev));
    result_buff->rssi_prx_drx_result.raw_rssi_drx = ftm_gsm_get_vars_ptr(rfm_dev)->ftm_gsm_state.ftm_rssi_drx;
    result_buff->rssi_prx_drx_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size +=sizeof(ftm_gsm_rssi_prx_drx_return_type);
    break;

  case FTM_GSM_GET_ENV_GAIN_NV_DATA:
    ftm_gsm_get_env_gain_nv(rfm_dev, &(result_buff->ftm_gsm_env_gain_nv_data));
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size +=sizeof(ftm_gsm_env_gain_nv_data_type);
    break;

  case FTM_GSM_FRAME_SWEEP_CFG_CMD:
    ftm_gsm_frame_sweep_cmd(FTM_GSM_FRAME_SWEEP_CFG, (uint8*) buff_ptr, cmd_ptr->ftm_hdr.cmd_data_len);
    break;

  case FTM_GSM_FRAME_SWEEP_EXEC_CMD:
    ftm_gsm_frame_sweep_cmd(FTM_GSM_FRAME_SWEEP_EXECUTE, (uint8*) buff_ptr, cmd_ptr->ftm_hdr.cmd_data_len);
    break;

  case FTM_GSM_FRAME_SWEEP_GET_RESULTS_CMD:
    ftm_gsm_frame_sweep_cmd(FTM_GSM_FRAME_SWEEP_GET_RESULTS, (uint8 *)(&(result_buff->frame_sweep_result)), cmd_ptr->ftm_hdr.cmd_data_len);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size +=sizeof(ftm_gsm_frame_sweep_result_buffer_type);
    break;

  case FTM_GSM_FRAME_SWEEP_CLEANUP_CMD:
    ftm_gsm_frame_sweep_cmd(FTM_GSM_FRAME_SWEEP_CLEANUP, (uint8 *)buff_ptr, cmd_ptr->ftm_hdr.cmd_data_len);
    break;

  default:
    result_buff->gen_result.error_code = (int32)FTM_FAILURE;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_gsm_extended_result_type);
    break;
  }


  /* Create response packet */
  /* Copy common header information from the request to the response */
  rsp_headers.diag_hdr             = cmd_ptr->diag_hdr;
  rsp_headers.ftm_hdr.cmd_id       = cmd_ptr->ftm_hdr.cmd_id;
  rsp_headers.ftm_hdr.cmd_data_len = cmd_ptr->ftm_hdr.cmd_data_len;

  /* At this point the response packet size equals the size of the headers plus
     the size of the fixed return parameters. If the response contains extra data
     whose size is determined by a parameter in the request, then we add it here. */

  rsp_pkt = ftmdiag_create_new_pkt(rsp_headers.ftm_hdr.cmd_rsp_pkt_size + extra_data_len);

  if ( rsp_pkt.pkt_payload != NULL )
  {
    /* copy header information */
    memscpy((void*)rsp_pkt.pkt_payload,
           rsp_headers.ftm_hdr.cmd_rsp_pkt_size + extra_data_len,
           (void*)&rsp_headers,
           sizeof(ftm_composite_cmd_header_type));

    /* copy fixed return parameters */
    memscpy((void*)((byte*)rsp_pkt.pkt_payload + sizeof(ftm_composite_cmd_header_type)),
           (rsp_headers.ftm_hdr.cmd_rsp_pkt_size + extra_data_len - sizeof(ftm_composite_cmd_header_type)),
           (void*)result_buff,
           (rsp_headers.ftm_hdr.cmd_rsp_pkt_size - sizeof(ftm_composite_cmd_header_type)));

    if ( extra_data_len > 0 )
    {
      /* copy extra variable length return data if present */
      memscpy((void*)((byte*)rsp_pkt.pkt_payload + rsp_headers.ftm_hdr.cmd_rsp_pkt_size),
             extra_data_len,
             local_buffer,
             extra_data_len);
    }
  }
  else
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,
          "Cannot allocate memory for response packet payload, cmd id = %d",
          cmd_ptr->ftm_hdr.cmd_id);
  }
  if ( local_buffer != NULL )
  {
    ftm_free(local_buffer);
  }

  if (result_buff != NULL)
  {
    ftm_free(result_buff);
  }

  return rsp_pkt;
} /* end ftm_gsm_extended_dispatch */

#endif /* FEATURE_GSM */
#endif /* FEATURE_FACTORY_TESTMODE */
