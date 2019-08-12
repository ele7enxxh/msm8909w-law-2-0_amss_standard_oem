/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      F T M  W C D M A  D I S P A T C H

GENERAL DESCRIPTION
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2002- 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/src/ftm_wcdma_dispatch.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/04/15   sd      Added support for FTM WCDMA Sensitivity Estimation
12/18/14   ars/rmb Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
11/13/14   jmf     [selftest] WCDMA FBRx Noise floor calibration/cancellation
10/09/14   vbh     Update call to set mode API with device parameter
09/23/14   kai     Add device info in ftm_wcdma_set_secondary_chain
09/23/14   vbh     CW cleanup- Use of right argument 
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/22/14   vbh     Updated the calls to set_tx apis with device parameters
08/06/14   aro     Support to override TempComp mode of operation
12/05/13   ska     Graceful exit if FTM_WCDMA_SET_MODE is issued when the phone is not in FTM mode
05/16/14   aro     Added support to perform Tx Power meter read
05/16/14   kai     Added XPT IQ capture offloading to RF thread
04/20/14   bn      WCDMA 8 PA state support and related cleanup
03/25/14   rmb     Add a new FTM command for get rxagc API during X2W measurements.
02/13/14   dw      [1]Change AGC command function to support multiple device
                   [2]Code cleanup
02/18/14   vbh     FTM_Radio_API updates - add few FTM dispatch cases
02/13/14   ac      add cmd to test the send_script api
12/18/13   zhh     add a check in set waveform command to check for RxTX state
02/05/13   dw      Add on_off argument in tm_wcdma_set_offset_cw()
09/10/13   aa      Enable/Disable NBEE RXCAL on the fly
08/21/13   dj      Added FTM_INT_CALV3_FEEDBACK_SETUP command
07/30/13   aro     Added WCDMA radio dispatch
07/15/13   ars     Added FTM interface for self-test APIs
07/10/13   aa      RX calibration optimization using NBEE
05/22/13  ka/ac    dc hsupa changes
05/21/13   rmb     Added GET MUTLI SYNTH and RESET MULTI SYNTH Command.
05/09/13   jl      Port IQ capture code
04/05/13   rmb     Added FTM APIs for WCDMA Sleep, Wakeup and Pretune to chan.
02/15/13   ac      fix for hdet read
02/15/13   ac      adding the common api for hdet read
02/07/13   ndb/vb  Added  FTM_SET_TUNER_TUNE_CODE_OVERRIDE support 
01/30/13   ka      Updated FTM interfaces to logical device
11/15/12   ka      Initial support for DCHSUPA
11/15/12   kai     Added radio setup operation
11/14/12   dw      Add cmd dispatch to set PA quiescent current
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
06/06/12   sbo     Added missing break statement.
04/23/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API
03/06/12   kai     Added API for setting/getting Tx datapath IQ gain
02/14/12   swb     Added API for FTM Tx Frequency Adjustment
01/11/12   vb      HDET Enable-Disable for every HDET read
12/17/11   dw      Flush mdsp cache after set LNA range command dispatch
11/03/11   swb     WCDMA Non-signaling Power Control Override 
10/17/11   ars     Added FTM_SET_TX_POWER_DBM
10/17/11   ars     Added FTM_SET_PA_STATE and FTM_SET_TX_POWER_DBM FTM Layer
09/09/11   shb     Use FTM_NUM_HDET_TO_RETURN in WCDMA. value is set back to 
                   32 and will not cause cal crash
09/09/11    dw     Fix WCDMA Cal crash
08/22/11    gh     Add command for W CalV3 config radio
08/21/11    ac     BC19 changes
07/11/11    vb     Cleanup FTM_SET_MULTI_CHAN
06/23/11    dw     Merge SC/DC DVGA offset funcitons
06/16/11   sar     Removed deprecated files..
05/09/11   sar     Added ftm_rf_cmd.h for CMI4.
04/15/11   ad      Added preliminary support for IQ capture and ICI multi-carrier cal
03/18/11   sty     Deleted unused rft_check_synth_lock
02/24/11   dw/ac   merge B3 DC support
01/19/10    dw     variable name change for RxLM
01/05/10   dw      Initial support for RxLM
10/19/10   vb      All device functions are now accessed thru device interface functions
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
07/20/10   ka      Fix for B2 DC 
05/12/10   ac      B11,B4,B2,B8 dual carrier bringup
04/14/10   ad      Fix compiler errors in WPLT build
04/11/10   ac      B9 j8220 bringup support
03/19/10   ad      Uncomment dynamic IQ capture API
03/16/10   kguo    Merged from scmm_final_merge branch
03/04/10   ad      Added support for dynamic IQ capture in INO cont. for WCDMA
03/03/10   wen     Fixed compilation errors
01/25/10    vb     Lint error fixes
01/21/10   sar     Lint fixes.
02/16/10   ad      Fix B5 merge changes
02/11/10    ac     merged dual carrier changes
02/10/10    ka     Restored ftm_wcdma_tune_to_chan call.
02/03/10    ka     Merged ICI changes from mdm9k dev branch.
01/11/10   kguo    Merged mdm9k dev branch to main\latest
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/08/09    bn     Separate Tx and Rx PLL for current savings
11/25/09   ntn     Code Cleanup
11/15/09   lp      Added temporary featuriztion for CMI not defined
11/11/09   ka      Dual carrier support.
10/19/09   bn      Removed any reference to adc.h
10/12/09   bn      Enabled HDET/Therm ADC reads and added FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2
10/07/09   sar     Updated TLMM to DAL.
09/29/09   sr      Changed pll_status in FTM_GET_SYNTH_STATE
09/18/09   sar     Updated ADC interface for CMI conformance.
09/13/09   vb      ICI calibration support
08/20/09   ka      Corrected location of #undef
08/18/09   ka      Remove IM2 and CtoN.
08/11/09   bn      FTM WCDMA SET TxAGC API support
08/11/09   bn      FTM WCDMA Release 7 waveform feature
08/11/09   bn      FTM WCDMA Offset CW tone
08/11/09   bn      Enable 12-bit HDET/THERM ADC reads
07/06/09   ckl     Fixed Rx AGC reporting error.
03/07/08   bn      Added support to enable RF Txplim debug message through diag
08/09/07   jfc     Mainlining T_MSM7200 feature
08/08/07   jfc     Mainlined T_MSM6280 feature
10/17/06   ycl     Modifications for initial MSM7600 build.
07/13/06   xw      Added support for 7200.
06/15/06   ka      Added support for internal thermistor.
05/09/06   lcl     Fixed a bug in DC calibration.
04/12/06    bn     Added ftm_set_secondary_chain and ftm_second_chain_test_call
04/07/06   jac     Changed path parameter enum type.
04/07/06   lcl     Added DC calibration for interBand CM.
03/24/06   jac     Added path parameter to get im2 functions.
03/28/06   adm     Fixed Compiler warning.
03/06/06   dp/bhas Add WCDMA diversity support.
02/21/06   tws     Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
05/04/05   rsr     Removed warnings.Cleanup
10/18/04   bhas    Removed FTM_HAS_ASYNC_PKT
10/05/04    ka     Added FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL.
09/07/04   lcl     Added get IM2 with suppression.
09/03/04    xw     Changed FTM_GET_HDET to FTM_GET_HDET_FROM_TX_SWEEP_CAL.
08/20/04    ka     Added command to configure tx sweep cal.
08/20/04    xw     Added 1800 band support.
08-21-02   rjr     Created
===========================================================================*/

#include "rfa_variation.h"
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef FEATURE_WCDMA

#include "msm.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_wcdma_ctl.h"
#include "rfm.h"
#include "rftxplim.h"
#include "bsp.h"
#include "rfwcdma_mdsp.h"
#include "rfdevice_wcdma_intf.h"
#include "rfdevice_hdet_wcdma_intf.h"
#include "rfdevice_hdet_cmn_intf.h"
#include "ftm_wcdma_radio.h"

#include "rfwcdma_core_temp_comp.h"
#include "ftm_rf_cmd.h"
#include "ftm_msg.h"

#ifdef FTM_HAS_IM2_CAL
#include "ftm_wcdma_im2_cal.h"
#endif

#include "ftm_umts_cton_lib.h"

#include "ftm_wcdma_dispatch.h"
#include "ftm_common.h"
#include "ftm_common_ino.h"
#include "rf_test_task_v.h"

#include "ftm_wcdma_calv3.h"

#include <stdlib.h>

#undef FTM_HAS_IM2_CAL

#define RF_ADC_PA_THERM        3 /* RTR thermistor ADC ID */
#define RF_ADC_PA_POWER_DETECT 4 /* RTR HDET ADC ID */

/* Macro to redefine the rf_tune_to_chan call. To allow support of different API across target. */
#define RF_TUNE_TO_CHAN(path, chan)         rfm_tune_to_chan(path, chan, (rfm_synth_tune_enum_type)RFCOM_NUM_TUNES, NULL, NULL)
//rf_tune_to_chan_wcdma( (path), (chan), RF_PATH_0);

/* To track the current RF mode */
extern ftm_rf_mode_type ftm_current_rf_mode;
extern rfwcdma_core_temp_comp_value_type rfwcdma_core_temp_comp_value;
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode;
extern uint8 ftm_wcdma_eul_mpr_idx;

/* Flag to set the RF TXPLIM debug message */
extern boolean rfwcdma_core_txplim_dbgmsg;

extern int2 ftm_tx_cal_sweep_hdet_val[];

extern void ftm_hspa_iq_mismatch_cal( rfcom_device_enum_type device, ftm_iqm_result_type *iqm_result );
extern void ftm_hspa_do_ici_cal(rfcom_device_enum_type device, ftm_ici_result_type *ici_result);
extern void ftm_hspa_do_ici_cal_multi_carrier(rfcom_device_enum_type device, rfcom_multi_carrier_hspa_id_type mode, 
                                       rfcom_multi_carrier_id_type carrier, ftm_ici_result_type *ici_result);
extern void ftm_hspa_do_iq_ino(rfcom_device_enum_type device, void *req_pkt);
extern ftm_lm_buf_type ftm_wcdma_rxlm_buffer[];

/*===========================================================================

FUNCTION      FTM_ERROR_MSG

DESCRIPTION   Prints and error message and returns 0.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
int ftm_error_msg ( void )
{
  FTM_MSG_ERROR("Band not supported in this software version.",0,0,0);
  return ( 0 );
}


/*===========================================================================

FUNCTION FTM_WCDMA_DISPATCH

DESCRIPTION
   This function handles wcdma ftm requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_wcdma_dispatch(rfm_device_enum_type device, ftm_pkt_type  *cmd_ptr)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  int i=0;

  #ifdef FTM_HAS_IM2_CAL
  ftm_im2_type im2_value;
  #endif

#ifndef FEATURE_CMI
#ifndef T_MSM7X30
  int ftm_adc_read_cnt = 0;
#endif /* !T_MSM7X30 */
#endif /* !FEATURE_CMI */
  boolean pll_status;
  uint16 adc_data = 0;

  switch (cmd_ptr->rf_params.ftm_rf_cmd)
  {
  case FTM_SET_MODE:
    rsp_pkt = ftm_wcdma_set_mode(device, cmd_ptr->rf_params.ftm_rf_factory_data.mode);
    break;

  case FTM_SET_PDM:
    ftm_wcdma_set_pdm((ftm_pdm_id_type) cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id,
                      cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val);
    break;

  case FTM_SET_SMPS_PA_BIAS_VAL:
    ftm_wcdma_set_smps_pa_bias_val(cmd_ptr->rf_params.ftm_rf_factory_data.gen_w);
    break;

  case FTM_SET_PA_CURRENT:
    ftm_wcdma_set_pa_current(cmd_ptr->rf_params.ftm_rf_factory_data.gen_b);
    break;

  case FTM_SET_SMPS_PA_BIAS_OVERRIDE:
    ftm_wcdma_set_smps_pa_bias_override(cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    break;

  case FTM_SET_TX_ON:
    ftm_wcdma_set_tx(device,ON);
    break;

  case FTM_SET_TX_OFF:
    ftm_wcdma_set_tx(device,OFF);
    break;

  case FTM_SET_CHAN:
    ftm_wcdma_tune_to_chan(device, cmd_ptr->rf_params.ftm_rf_factory_data.chan);
    break;

  case FTM_SET_MULTI_CHAN:   
    ftm_wcdma_tune_to_multi_chan(device, (cmd_ptr->rf_params.ftm_rf_factory_data.chan_array));
    break;

  case FTM_SET_CDMA_CW_WAVEFORM:
    rsp_pkt = ftm_wcdma_set_cw(cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    break;

  case FTM_SET_OFFSET_CW:
    ftm_wcdma_set_offset_cw(cmd_ptr->rf_params.ftm_rf_factory_data.gen_d, TRUE);
    break;

  case FTM_GET_CAGC_RX_AGC:
    /* Only allow user polling Rx AGC while in WCDMA mode */
/*! @todo changlee bringup Ask Anar to fix this issue */
//    if ( (ftm_mode == FTM_MODE) &&
    if (
         ( (ftm_current_rf_mode == FTM_DB_RF_WCDMA_IMT) ||
           (ftm_current_rf_mode == FTM_DB_RF_WCDMA_1900MHZ_A) ||
           (ftm_current_rf_mode == FTM_DB_RF_WCDMA_1900MHZ_B) ||
           (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC3) ||
           (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC4) ||
           (ftm_current_rf_mode == FTM_DB_RF_WCDMA_800MHZ) ||
	   (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC19) ||
           (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC8) ||
           (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC9) ||
           (ftm_current_rf_mode == FTM_DB_RF_WCDMA_BC11)           
         )
       )
    cmd_ptr->rf_params.ftm_rf_factory_data.gen_w = (uint16) rfm_get_receive_agc_val(device,
                                                                                    RFCOM_SERVINGCELL_RXAGC_RAW, 
                                                                                    RFCOM_RXAGC_IN_DB10);
    break;

  case FTM_GET_CAGC_DUAL_CARRIER_RX_AGC:
    cmd_ptr->rf_params.ftm_rf_factory_data.gen_w = rfm_get_receive_agc_car1_val(device, RFCOM_SERVINGCELL_RXAGC_RAW, RFCOM_RXAGC_IN_DB10);
    break;

  case FTM_SET_PA_RANGE:
    ftm_wcdma_set_pa_range((byte) cmd_ptr->rf_params.ftm_rf_factory_data.range);
    break;

  case FTM_SET_LNA_RANGE:
    ftm_wcdma_set_lna_range(device, (byte)cmd_ptr->rf_params.ftm_rf_factory_data.range);
    break;

  case FTM_GET_ADC_VAL:
    /* If HDET channel then call rf_read_hdet, as it may need to be read from RTR rather than ADC */
    if (cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id == RF_ADC_PA_POWER_DETECT)
    {
      rfdevice_wcdma_hdet_read_type hdet_output={0};
      /* Read HDET */
      rfdevice_hdet_wcdma_read_hdet( RFM_DEVICE_0,
                                         ftm_curr_wcdma_mode,
                                         FALSE, 
                                     &hdet_output );

      if ( hdet_output.hdet_quality == TRUE )
      {
        cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val = hdet_output.hdet_reading;
      }
      else
      {
        cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val = -1;
      }


    }
    /* If therm channel then call rfi_read_therm, as it may need to be read from RTR rather than ADC */
    else if (cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id == RF_ADC_PA_THERM)
    {
      /*! @todo Bhaskar Direct therm reads are used here */
      rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0,ftm_curr_wcdma_mode,RFDEVICE_GET_THERMISTER_VALUE, &adc_data);
      cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val = adc_data;
      //cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val = rfwcdma_core_temp_comp_value.last_raw_temp_value;
    }
    /* otherwise read from adc driver as usual */
    else
    {
     FTM_MSG_HIGH("Unsupported ADC reads", 0, 0, 0);
     #ifndef FEATURE_CMI
     #ifndef T_MSM7X30
	 #ifndef FEATURE_WPLT
      do
      {
        cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val = (int16)adc_read( (adc_mux_device)cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id );
        ftm_adc_read_cnt++;
      }while (
              ((cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val == 0) ||
              (cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val == 255)) && (ftm_adc_read_cnt < 3)
              );
	 #endif /* FEATURE_WPLT */
     #endif /* !T_MSM7X30 */
     #endif /* !FEATURE_CMI */
    }
    break;

  case FTM_GET_SYNTH_STATE:
    rfdevice_wcdma_rx_cmd_dispatch(device,ftm_curr_wcdma_mode,RFDEVICE_GET_RX_SYNC_LOCK, &pll_status);
    cmd_ptr->rf_params.ftm_rf_factory_data.on_off = pll_status;
    break;

  case FTM_RESET_MULTI_SYNTH_STATE:	
    cmd_ptr->rf_params.ftm_rf_factory_data.on_off=ftm_wcdma_reset_multisynth_status(device);
    break;

  case FTM_GET_MULTI_SYNTH_STATE:  	
    cmd_ptr->rf_params.ftm_rf_factory_data.\
    ftm_get_multi_synth_lock_rsp.status_flag = TRUE;
    ftm_wcdma_get_multisynth_status(device,
                                    &cmd_ptr->rf_params.ftm_rf_factory_data.ftm_get_multi_synth_lock_rsp.rx_synth_status,
                                    &cmd_ptr->rf_params.ftm_rf_factory_data.ftm_get_multi_synth_lock_rsp.tx_synth_status
                                    );     
    break;

  case FTM_SET_TUNER_TUNE_CODE_OVERRIDE:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.result =(ftm_tuner_override_status_type)ftm_wcdma_set_tuner_tune_code_override(device,
		                                                                                                                                           (uint8)cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.override_flag,
		                                                                                                                                           (void*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.config_data),
		                                                                                                                                           (uint8*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.nv_data),
		                                                                                                                                           (uint8*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.tuner_id));
    break;

  case FTM_SET_LNA_OFFSET:
    ftm_wcdma_set_lna_offset(device, (byte) cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id,
                             cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val);
    break;

  case FTM_SET_DVGA_OFFSET:
  case FTM_SET_DUAL_CARRIER_DVGA_OFFSET:
    ftm_wcdma_set_dvga_gain_offset(device, (int16)cmd_ptr->rf_params.ftm_rf_factory_data.gen_w);
    break;

  case FTM_ENABLE_SEND_SCRIPT:
     ftm_wcdma_enable_scripts(device,(int8)cmd_ptr->rf_params.ftm_rf_factory_data.gen_w);
     break;

  case FTM_GET_CDMA_IM2:
    #ifdef FTM_HAS_IM2_CAL
    if (ftm_wcdma_get_im2( device, &im2_value) == TRUE)
    {
      cmd_ptr->rf_params.ftm_rf_factory_data.byte_struct.byte0 = im2_value.i_dac;
      cmd_ptr->rf_params.ftm_rf_factory_data.byte_struct.byte1 = im2_value.q_dac;
      cmd_ptr->rf_params.ftm_rf_factory_data.byte_struct.byte2 = im2_value.transconductor;
    }
    else
    {
      cmd_ptr->rf_params.ftm_rf_factory_data.byte_struct.byte0 = 0;
      cmd_ptr->rf_params.ftm_rf_factory_data.byte_struct.byte1 = 0;
      cmd_ptr->rf_params.ftm_rf_factory_data.byte_struct.byte2 = 0;
      FTM_MSG_HIGH("IM2 calibration is failed!", 0, 0, 0);
    }
    #else
    FTM_MSG_HIGH("WCDMA IM2 Cal cmd is not supported ...", 0, 0, 0);
    #endif
    break;

  case FTM_GET_CDMA_IM2_WITH_SUPPRESSION:
    #ifdef FTM_HAS_IM2_CAL
    (void)ftm_wcdma_get_im2_with_suppression(device, &cmd_ptr->rf_params.ftm_rf_factory_data.mix_struct);
    #else
    FTM_MSG_HIGH("WCDMA IM2 Cal cmd is not supported ...", 0, 0, 0);
    #endif
    break;


  case FTM_TX_SWEEP_CAL:
    ftm_uplink_sweep_cal(cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    break;

  case FTM_GET_DVGA_OFFSET:
  case FTM_GET_DUAL_CARRIER_DVGA_OFFSET:

    cmd_ptr->rf_params.ftm_rf_factory_data.gen_w = (uint16)ftm_wcdma_get_dvga_gain_offset(device,
                                                                                           cmd_ptr->rf_params.ftm_rf_factory_data.gen_w);
    break;

  case FTM_GET_LNA_OFFSET:
    cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val =
    ftm_wcdma_get_lna_offset(device,
                              (uint8)(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id), (uint16)(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val) );
    break;

  case FTM_GET_HDET_FROM_TX_SWEEP_CAL:
    /* Returns the HDET values stored in ftm_tx_cal_sweep_hdet_val[] during the previous Tx Cal Sweep */
    cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val =
    ftm_tx_cal_sweep_hdet_val[cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id];
    break;

  case FTM_SET_HDET_TRACKING:
  	ftm_wcdma_set_hdet_tracking((uint8)(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id),
  							  (uint16)(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val) );
  	FTM_MSG_MED("FTM_SET_HDET_TRACKING: %d %d",
                cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id,
                cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val, 0);
    break;

  case FTM_CONFIGURE_TX_SWEEP_CAL:
    ftm_wcdma_configure_tx_sweep_cal(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id,
                                     (byte)cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val);
    break;

  case FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL:
    for(i=0;i<FTM_NUM_HDET_TO_RETURN;i++)
    {
      cmd_ptr->rf_params.ftm_rf_factory_data.hdet_array[i] = (byte)ftm_tx_cal_sweep_hdet_val[i];
    }
    break;

  case FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2:
    for(i=0;i<FTM_NUM_HDET_TO_RETURN;i++)
    {
      cmd_ptr->rf_params.ftm_rf_factory_data.hdet_array_16_bit[i] = (uint16)ftm_tx_cal_sweep_hdet_val[i];
    }
    break;

    /* Not supported */
   /*
  case FTM_GET_CTON:
    temp_cton = ftm_umts_get_CtoN();
    cmd_ptr->rf_params.ftm_rf_factory_data.gen_d = (dword)temp_cton;
    FTM_MSG_MED("FTM_GET_CTON: %d.%d",(temp_cton/10),abs(temp_cton%10), 0);

    break; */

  case FTM_SET_TX_SWEEP_STEP_SIZE:
    ftm_wcdma_set_tx_sweep_step_size(cmd_ptr->rf_params.ftm_rf_factory_data.gen_b);
    break;

  case FTM_DO_DC_CAL:
     ftm_do_dc_cal();
     break;

  #ifdef FEATURE_WCDMA_RX_DIVERSITY
  case FTM_SET_SECONDARY_CHAIN:
    ftm_wcdma_set_secondary_chain(RFM_DEVICE_1,cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    FTM_MSG_MED("FTM_SET_SECONDARY_CHAIN: %d", cmd_ptr->rf_params.ftm_rf_factory_data.on_off, 0, 0);
    break;

  case FTM_SECOND_CHAIN_TEST_CALL:
    ftm_second_chain_test_call(cmd_ptr->rf_params.ftm_rf_factory_data.gen_b);
    FTM_MSG_MED("FTM_SECOND_CHAIN_TEST_CALL: %d", cmd_ptr->rf_params.ftm_rf_factory_data.gen_b, 0, 0);
    break;
  #endif /* FTM_HAS_WCDMA_RX_DIVERSITY */

  case FTM_ENABLE_RF_TXPLIM_DEBUG_MSG:
    rfwcdma_core_txplim_dbgmsg = cmd_ptr->rf_params.ftm_rf_factory_data.gen_b;
    FTM_MSG_MED("Enable RF txplim debug message: %d", cmd_ptr->rf_params.ftm_rf_factory_data.gen_b, 0, 0);
    break;

  case FTM_ENABLE_EUL_WAVEFORM:
    ftm_wcdma_eul_mpr_idx = ftm_configure_eul_waveform(
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.bc, 
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.bd,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.dhs,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.dec,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.ded,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.bed2,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.mu,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.num_dpdch,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.hsdpcch_en,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_eul_data.modulation_scheme);
     cmd_ptr->rf_params.ftm_rf_factory_data.gen_b = ftm_wcdma_eul_mpr_idx;
    break;

  case FTM_DISABLE_EUL_WAVEFORM:
    ftm_disable_eul_waveform();
    ftm_wcdma_eul_mpr_idx = 0;
    break;

  case FTM_WCDMA_SET_TXAGC:
    break;

  case FTM_WCDMA_TXAGC_SWEEP:
    break;
   
  case  FTM_WCDMA_TPC_CONTROL:
  	ftm_wcdma_tpc_control(
  	 cmd_ptr->rf_params.ftm_rf_factory_data.ftm_wcdma_tpc_control_data.override_enable,
  	 cmd_ptr->rf_params.ftm_rf_factory_data.ftm_wcdma_tpc_control_data.tpc_pattern);
    break;

  case FTM_DO_IQ_MISMATCH_CAL:
    ftm_hspa_iq_mismatch_cal(device, 
                             &cmd_ptr->rf_params.ftm_rf_factory_data.iqm_result);
    break;

  case FTM_DO_ICI_CAL:
    ftm_hspa_do_ici_cal(device, 
                        &cmd_ptr->rf_params.ftm_rf_factory_data.ici.result);
    break;

  case FTM_DO_ICI_CAL_MULTI_CARRIER:
    ftm_hspa_do_ici_cal_multi_carrier(device, 
                        cmd_ptr->rf_params.ftm_rf_factory_data.ici.mode,
                        cmd_ptr->rf_params.ftm_rf_factory_data.ici.carrier, 
                        &cmd_ptr->rf_params.ftm_rf_factory_data.ici.result);
    break;

  case FTM_DO_WCDMA_IQ_LOGGING:
    /*ftm_hspa_do_iq_ino(ftm_wcdma_get_rx_chain_handle(device), 
					    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_iq_log.testbus_sel, 
					    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_iq_log.log_client_id, 
                        cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_iq_log.buf_size,
					    &cmd_ptr->rf_params.ftm_rf_factory_data.ftm_config_iq_log.result); */
	ftm_hspa_do_iq_ino(device, (void *)&(cmd_ptr->rf_params.ftm_rf_factory_data));	
    break;
   
  case FTM_INT_WCDMA_CALV3_CONFIGURE_RADIO:
    rsp_pkt = ftm_wcdma_calv3_configure_radio(cmd_ptr);
    break;

  case FTM_SET_TX_POWER_DBM:
    ftm_wcdma_set_tx_power_dbm(cmd_ptr->rf_params.ftm_rf_factory_data.tx_power_dbm_data.enable_flag, cmd_ptr->rf_params.ftm_rf_factory_data.tx_power_dbm_data.tx_power.tx_power_dbm_wcdma);
    break;

  case FTM_GET_RX_LEVEL_DBM:
       ftm_wcdma_get_rx_level_dbm( device,
           &cmd_ptr->rf_params.ftm_rf_factory_data.rx_level_dbm_data.rx_level_agc.rx_level_agc_wcdma,
           &cmd_ptr->rf_params.ftm_rf_factory_data.rx_level_dbm_data.rx_level_dbm.rx_level_dbm_wcdma);
    break;

  case FTM_WCDMA_GET_RX_LEVEL_DBM_X2W_MEAS:
       /* Tools is going to query RxAGC at any time during or after the gap. So bypass the rxagc on flag 
        and read the rxagc from mdsp async interface.*/
       ftm_wcdma_get_rx_level_dbm_xtow_meas( device, (uint16)(cmd_ptr->rf_params.ftm_rf_factory_data.chan),
         &cmd_ptr->rf_params.ftm_rf_factory_data.rx_level_dbm_data.rx_level_agc.rx_level_agc_wcdma,
         &cmd_ptr->rf_params.ftm_rf_factory_data.rx_level_dbm_data.rx_level_dbm.rx_level_dbm_wcdma, TRUE);
  break;

  case FTM_SET_PA_STATE:
    ftm_wcdma_set_pa_state((uint8)cmd_ptr->rf_params.ftm_rf_factory_data.gen_b);
    break;

  case FTM_SET_FREQ_ADJUST:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.total_freq_adjust_hz.total_freq_adjust_hz_wcdma = 
      ftm_wcdma_set_freq_adjust(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.user_freq_adjust_ppb.user_freq_adjust_ppb_wcdma,
                              cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.center_frequency_hz.center_frequency_hz_wcdma,
                              cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.enable_xo_freq_adjust
                              );
    break;

  case FTM_SET_DIGITAL_IQ_GAIN:
    ftm_wcdma_set_iqgain_value((int16)cmd_ptr->rf_params.ftm_rf_factory_data.gen_w);
    break;

  case FTM_GET_DIGITAL_IQ_GAIN:
    cmd_ptr->rf_params.ftm_rf_factory_data.gen_w = (int16)ftm_wcdma_get_iqgain_value();
    break;

  case FTM_IQ_CAPTURE_GET_CONFIG:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_config.ftm_error_code =
    ftm_wcdma_iq_capture_get_config(&cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_config);
    break;	


  case FTM_IQ_CAPTURE_ACQUIRE_SAMPLES:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples.ftm_error_code =
    ftm_wcdma_iq_capture_acquire_samples(device,&cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples);
    break;	

  case FTM_IQ_CAPTURE_GET_SAMPLES:
    ftm_wcdma_iq_capture_get_samples(device,&cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_samples,&rsp_pkt);
    break;	

  case FTM_IQ_CAPTURE_ANALYZE_SAMPLES:
  	
	cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples.ftm_error_code =
	ftm_wcdma_iq_capture_analyze_samples(device,&cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_analyze_samples,&rsp_pkt );
	break;	
   
  case FTM_INT_CALV3_RADIO_SETUP:
    rsp_pkt = ftm_wcdma_calv3_radio_setup(cmd_ptr);
    break;

  case FTM_SET_MULTI_CARRIER_TX:   
    ftm_wcdma_set_tx_multi_carrier( device, 
                                    (cmd_ptr->rf_params.ftm_rf_factory_data.multi_tx_data.state),
                                    (cmd_ptr->rf_params.ftm_rf_factory_data.multi_tx_data.multi_carrier_idx));
    break;

  case FTM_SET_MULTI_CARRIER_TX_GAINS:
    ftm_wcdma_set_tx_multi_carrier_gains( (cmd_ptr->rf_params.ftm_rf_factory_data.chan_array[0]),
                                          (cmd_ptr->rf_params.ftm_rf_factory_data.chan_array[1]));
    break;
  
  case FTM_WCDMA_SLEEP:
    ftm_wcdma_sleep(device);
    break;

  case FTM_WCDMA_WAKEUP:
    ftm_wcdma_wakeup(device);
    break;

  case FTM_WCDMA_PRETUNE_TO_CHAN:
    ftm_wcdma_pretune_to_chan(device, (cmd_ptr->rf_params.ftm_rf_factory_data.chan_array));
    break;

  case FTM_RF_MEAS_TX_PARAMS:
    ftm_wcdma_fbrx_meas_tx_params( device,
                                   cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.num_averages,
                                   cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.capture_offset,
                                   cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.tx_measurement_config,
                                   &rsp_pkt );
    break;

  case FTM_FBRX_IQ_CAPTURE_TRIGGER:
    ftm_wcdma_fbrx_iq_capture_trigger( device,
                                       cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_trigger.sample_size,
                                       cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_trigger.fbrx_chain,
                                       &rsp_pkt );
    break;

  case FTM_FBRX_IQ_CAPTURE_GET_SAMPLES:
    ftm_wcdma_fbrx_iq_capture_get_samples ( device,
                                            cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_source,
                                            cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_offset,
                                            cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_size,
                                            &rsp_pkt );
    break;

  case FTM_RF_MEAS_FBRX_NOISE:
    ftm_wcdma_fbrx_meas_noise_params( cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_noise_params.curr_tech_band,
                                      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_noise_params.device,
                                      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_noise_params.num_captures_per_meas, // num averages to be implemented
                                      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_noise_params.tx_chan[0],//0th entry is valid W
                                      &rsp_pkt );
    break;

  case FTM_INT_CALV3_FEEDBACK_SETUP:
    rsp_pkt = ftm_wcdma_calv3_feedback_setup(cmd_ptr);
    break;

  case FTM_INT_CALV3_XPT_IQ_CAPTURE:
    rsp_pkt = ftm_wcdma_cal_xpt_iq_capture(cmd_ptr);
    break;

  case FTM_GET_MULTIPLE_RX_LEVEL_DBM:
    ftm_wcdma_get_multiple_rx_level_dbm( device,
                                         cmd_ptr->rf_params.ftm_rf_factory_data.get_multiple_rx_level_dbm_data.select_rx_chain,
                                         &rsp_pkt);
    break;

  default:
    FTM_MSG_HIGH("The cmd is not supported ...", 0, 0, 0);
    break;
  }
  return rsp_pkt;
} /* end ftm_wcdma_dispatch */

/*============================================================================*/
/*!
  @name 1xHDR Radio Control Dispatch

  @brief
  This section contains the dispatches used to control the SVDO Radio.

  @image html ftm_cdma_rfm_command.jpg
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  WCDMA Radio Test Dispatch function.

  @details
  This function handles requests to run tests and  other primitives
  by dispatching the appropriate functions.

  @param *ftm_req_data
  Request Data Packet

  @return
  Response packet to the diag
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_dispatch
(
  void  * ftm_req_data
)
{
  /* Request Packet : extract the FTM Command only */
  ftm_composite_cmd_header_type
    * header =
    (ftm_composite_cmd_header_type*)ftm_req_data;

  /* Create Response Packet : Defaults to Bad Packet */
  ftm_rsp_pkt_type ftm_rsp_data = 
                 ftm_common_create_rsp_pkt( FTM_COMMON_BAD_RSP );

  /* Dispatch Commands */
  switch (header->ftm_hdr.cmd_id)
  {

  case FTM_WCDMA_RADIO_TRIGGER_AGC_LOG:
    ftm_rsp_data = ftm_wcdma_radio_trigger_agc_log( ftm_req_data );
    break;


  case FTM_WCDMA_RADIO_SET_RX_CAL_NBEE:
    ftm_rsp_data = ftm_wcdma_set_rxcal_optimization( ftm_req_data );
    break;


  case FTM_WCDMA_RADIO_PRE_TUNE:
    ftm_rsp_data = ftm_wcdma_radio_pre_tune( ftm_req_data );
    break;

  case FTM_WCDMA_RADIO_TUNE:
    ftm_rsp_data = ftm_wcdma_radio_tune( ftm_req_data );
    break;

  case FTM_WCDMA_RADIO_RX_WAKEUP:
    ftm_rsp_data = ftm_wcdma_radio_rx_wakeup( ftm_req_data );
    break;

  case FTM_WCDMA_RADIO_RX_SLEEP:
    ftm_rsp_data = ftm_wcdma_radio_rx_sleep( ftm_req_data );
    break;
  case FTM_WCDMA_RADIO_TX_INIT:
    ftm_rsp_data = ftm_wcdma_radio_tx_init( ftm_req_data );
    break;

  case FTM_WCDMA_RADIO_TX_ENABLE:
    ftm_rsp_data = ftm_wcdma_radio_tx_enable( ftm_req_data );
    break;

  case FTM_WCDMA_RADIO_TX_DISABLE:
    ftm_rsp_data = ftm_wcdma_radio_tx_disable( ftm_req_data );
    break;

  case FTM_WCDMA_RADIO_TX_WAKEUP:
    ftm_rsp_data = ftm_wcdma_radio_tx_wakeup( ftm_req_data );
    break;

  case FTM_WCDMA_RADIO_GET_RXAGC:
    ftm_rsp_data = ftm_wcdma_radio_get_rxagc(ftm_req_data);
    break;

  case FTM_WCDMA_RADIO_OVERRIDE_PLIM:
    ftm_rsp_data = ftm_wcdma_radio_override_plim(ftm_req_data);
    break;

  case FTM_WCDMA_RADIO_OVERRIDE_TEMPCOMP:
    ftm_rsp_data = ftm_wcdma_radio_override_tempcomp(ftm_req_data);
    break;

  default:
    FTM_MSG_1( FTM_ERROR, "WCDMA Radio Dispatch : Invalid Command - %d",
               header->ftm_hdr.cmd_id );
    break;

  }

  FTM_MSG_1( FTM_MED, "WCDMA Radio Dispatch : Command - %d",
             header->ftm_hdr.cmd_id );

  /* Return Response packet */
  return ftm_rsp_data;

} /* ftm_wcdma_radio_dispatch */

/*! \} */


#endif /* FEATURE_WCDMA */
#endif /* FEATURE_FACTORY_TESTMODE */
