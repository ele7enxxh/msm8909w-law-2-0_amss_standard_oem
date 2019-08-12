/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      F T M  T D S C D M A  D I S P A T C H

GENERAL DESCRIPTION
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2011 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/src/ftm_tdscdma_dispatch.c#1 $
  $DateTime: 2016/12/13 07:59:50 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/14   jyu     Add support for C2 and C3 FTM test
10/13/14   al      Add support to FTM_TDSCDMA_SET_DEVICES
09/30/14   jyu     Add support to FTM_SET_ALT_PATH_OVERRIDE_INDEX
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
07/17/14   ms      Sawless RX support 
07/09/14   ych     Added XPT IQ capture offloading to RF thread 
06/03/14   jz      Add tuner tune code override command
04/29/14   jps     Add missing break statement
04/28/14   jps     Support for TDS port switching
09/10/13   aka     Added FTM interface for Self test cmds 
09/06/13   ars     Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
08/21/13   dj      Added FTM_INT_CALV3_FEEDBACK_SETUP command 
07/23/13   ych     Opcodes calv4 support
07/03/13   zg      Added self-cal support.
06/26/13   jyu     Added support to remove the requirement to specify band info 
                   when it is not ready 
06/19/13   kb      [XPT] Added support for FTM Tx waveform switching
06/18/13   ms      Change default band to B34 
06/03/13   nsh     Add support for FTM_GET_MULTI_SYNTH_STATE for Dime
05/25/13   qzh     Porting TDSCDMA IQ capture from NIKEL to DIME
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
03/22/13   zg      Updated IQ catpure API.
03/21/13   ms      Tuner TDET support 
03/07/13   zg      Added IQ capture support.
02/12/13   ms      remove check for current state in FTM_GET_CAGC_RX_AGC 
02/08/13   jyu     Removed hard-coded device info 
09/28/12   jyu     Enabled diversity 
07/31/12   qzh     Rfdevice interface change to use rfm_device instead of path
07/28/12   jyu     Temporarily zero out RF FTM test code for merge compilation
07/18/12   jps     Add FTM error handling for RF mode commands 
07/16/12   jps     Initial FTM burst TX/RX framework
03/26/12   hy      TD-SCDMA Cal time optimization
11/16/11   hy      Added TDSCDMA V2/V3 Cal, add get tx sync state    
05/19/11   hy      Created
===========================================================================*/

#include "rfa_variation.h"
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmicap.h"

#ifdef FTM_HAS_TDSCDMA

#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_tdscdma_ctl.h"
#include "ftm_common_control.h"
#include "bsp.h"
#include "rfdevice_tdscdma_intf.h"
#include "rfdevice_hdet_tdscdma_intf.h"
#include "rf_tdscdma_core_temp_comp.h"
#include "ftm_rf_cmd.h"

#include "ftm_tdscdma_dispatch.h"
#include "ftm_tdscdma_calv3.h"
#include "ftm_common.h"
#include "ftm_common_ino.h"
#include "rf_test_task_v.h"
#include "rf_tdscdma_mc.h"

#include <stdlib.h>

#define RF_TDSCDMA_ADC_PA_THERM        3 /* RTR thermistor ADC ID */
#define RF_TDSCDMA_ADC_PA_POWER_DETECT 4 /* RTR HDET ADC ID */

/* To track the current RF mode */
extern ftm_rf_mode_type ftm_current_rf_mode;

extern uint8 ftm_tdscdma_eul_mpr_idx;

/* Flag to set the RF TXPLIM debug message */
extern boolean rf_tdscdma_core_txplim_dbgmsg;

extern int2 ftm_tdscdma_tx_cal_sweep_hdet_val[];

/* For IQ capture testing */
extern uint16 ftm_tdscdma_current_chan;
extern ftm_multi_synth_lock_status_enum_type tx_pll_multi_state ;
extern ftm_multi_synth_lock_status_enum_type rx_pll_multi_state ;

extern void ftm_tdshspa_iq_mismatch_cal( ftm_receive_chain_handle_type chain, ftm_iqm_result_type *iqm_result );
extern void ftm_tdshspa_do_ici_cal(ftm_receive_chain_handle_type chain, ftm_ici_result_type *ici_result);
extern void ftm_tdshspa_do_iq_ino(ftm_receive_chain_handle_type chain, void *req_pkt);


/*===========================================================================

FUNCTION      FTM_TDSCDMA_GET_RX_CHAIN_HANDLE

DESCRIPTION   Performa a mapping from rfcom device type to an
              FTM receive chain type.  The mapping is simple, and
              always implies that Transceiver 0 is the Primary
              receive path during FTM mode.


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_receive_chain_handle_type ftm_tdscdma_get_rx_chain_handle( rfcom_device_enum_type device)
{
  ftm_receive_chain_handle_type retval = FTM_RECEIVE_CHAIN_0;

  if ( device == RFCOM_TRANSCEIVER_0)
  {
    retval = FTM_RECEIVE_CHAIN_0;
  }
//#ifdef FEATURE_RF_TDSCDMA_RX_DIVERSITY
  else if ( device == RFCOM_RECEIVER_DIV)
  {
    retval = FTM_RECEIVE_CHAIN_1;
  }
//#endif

  return ( retval );
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_DISPATCH

DESCRIPTION
   This function handles tdscdma ftm requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_tdscdma_dispatch(rfm_device_enum_type device, ftm_pkt_type  *cmd_ptr)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  int i=0;

  boolean pll_status = FALSE;
  uint16 adc_data = 0;
  
  FTM_MSG_HIGH("ftm_tdscdma_dispatch(device=%d, cmd=%d)", device, cmd_ptr->rf_params.ftm_rf_cmd, 0);

  if((device == RFM_DEVICE_0) || (device == RFM_DEVICE_1))
  {
    // Assumption is pri_device=tx_device and sec_device=pri_device (always use as a pair)
    ftm_tdscdma_current_pri_device = RFM_DEVICE_0;
    ftm_tdscdma_current_sec_device = RFM_DEVICE_1;
    ftm_tdscdma_current_tx_device  = RFM_DEVICE_0;
  }
  else if((device == RFM_DEVICE_2) || (device == RFM_DEVICE_3))
  {
    // Assumption is pri_device=tx_device and sec_device=pri_device (always use as a pair)
    ftm_tdscdma_current_pri_device = RFM_DEVICE_2;
    ftm_tdscdma_current_sec_device = RFM_DEVICE_3;
    ftm_tdscdma_current_tx_device  = RFM_DEVICE_2;
  }
  else
  {
    FTM_MSG_HIGH("Wrong device %d passed into ftm_tdscdma_dispatch!", 
                 device, 0, 0);
  }
    
  FTM_MSG_HIGH("ftm_tdscdma_dispatch updated pri_device=%d, sec_device=%d, tx_device=%d", 
               ftm_tdscdma_current_pri_device,
               ftm_tdscdma_current_sec_device,
               ftm_tdscdma_current_tx_device);

  switch (cmd_ptr->rf_params.ftm_rf_cmd)
  {
  case FTM_SET_MODE:
    ftm_tdscdma_set_mode(cmd_ptr->rf_params.ftm_rf_factory_data.mode);
    break;

  case FTM_SET_PDM:
    ftm_tdscdma_set_pdm((ftm_pdm_id_type) cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id,
                      cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val);
    break;

  case FTM_SET_SMPS_PA_BIAS_VAL:
    ftm_tdscdma_set_smps_pa_bias_val(cmd_ptr->rf_params.ftm_rf_factory_data.gen_w);
    break;

  case FTM_SET_SMPS_PA_BIAS_OVERRIDE:
    ftm_tdscdma_set_smps_pa_bias_override(cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    break;

  case FTM_SET_TX_ON:
    ftm_tdscdma_set_tx(ON);
    break;

  case FTM_SET_TX_OFF:
    ftm_tdscdma_set_tx(OFF);
    break;

  case FTM_TDSCDMA_SET_TX_CONTINOUS:
    ftm_tdscdma_set_tx_continuous(cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    break;

  case FTM_SET_CHAN:
    ftm_tdscdma_tune_to_chan(ftm_tdscdma_get_rx_chain_handle( device),cmd_ptr->rf_params.ftm_rf_factory_data.chan);
    break;

  case FTM_SET_CDMA_CW_WAVEFORM:
    ftm_tdscdma_set_cw(cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    break;

  case FTM_SET_OFFSET_CW:
    ftm_tdscdma_set_offset_cw(cmd_ptr->rf_params.ftm_rf_factory_data.gen_d);
    break;

  case FTM_GET_CAGC_RX_AGC:
    /* Only allow user polling Rx AGC while in TDSCDMA mode */
    // remove check for current state for FTM IRAT functionality as there is no side effect on standalone

    cmd_ptr->rf_params.ftm_rf_factory_data.gen_w = ftm_tdscdma_get_rx_agc(ftm_tdscdma_get_rx_chain_handle(device));
     #if 0 
    if (ftm_get_current_state(RFM_DEVICE_0) == FTM_STATE_TDSCDMA)
    {
      cmd_ptr->rf_params.ftm_rf_factory_data.gen_w = ftm_tdscdma_get_rx_agc(ftm_tdscdma_get_rx_chain_handle(device));
    }
  
    else
    {
      cmd_ptr->rf_params.ftm_rf_factory_data.gen_w = -999;
      FTM_MSG_HIGH("Skip FTM_GET_CAGC_RX_AGC because UE not in FTM_STATE_TDSCDMA", 0, 0, 0);
    }
     #endif
    break;

  case FTM_TDSCDMA_SLOT_CONFIG_OVERRIDE: /*temporarily used this command for iq capture*/

    /* perform one instance of IQ capture on the current channel */
    ftm_tdscdma_do_iq_capture( ftm_tdscdma_current_chan );
    break;

  case FTM_TDSCDMA_TX_SELF_CAL_ENABLE:

    ftm_tdscdma_config_tx_selfcal(TRUE);
    break;

  case FTM_TDSCDMA_TX_SELF_CAL_DISABLE:

    ftm_tdscdma_config_tx_selfcal(FALSE);
    break;

  case FTM_SET_PA_RANGE:
    ftm_tdscdma_set_pa_range((byte) cmd_ptr->rf_params.ftm_rf_factory_data.range);
    break;

  case FTM_SET_LNA_RANGE:
    ftm_tdscdma_set_lna_range( ftm_tdscdma_get_rx_chain_handle( device), (byte)cmd_ptr->rf_params.ftm_rf_factory_data.range, ftm_tdscdma_rx_lin_state);
    break;

  case FTM_GET_ADC_VAL:
    /* If HDET channel then call rf_read_hdet, as it may need to be read from RTR rather than ADC */
    if (cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id == RF_TDSCDMA_ADC_PA_POWER_DETECT)
    {
      if (is_wtr_hdet)
      {        
        rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0, 
                                         RFCOM_TDSCDMA_MODE, 
                                         RFCOM_BAND_TDSCDMA_ANY_BAND,
                                         RFDEVICE_GET_HDET_VALUE, 
                                         &adc_data);

      }
      else
      {
        (void)rfdevice_hdet_tdscdma_tx_hdet_read(RFM_DEVICE_0,
                                                 RFCOM_BAND_TDSCDMA_ANY_BAND,
                                                 FALSE, 
                                                 &adc_data,
                                                 NULL,
                                                 RFDEVICE_EXECUTE_IMMEDIATE);
      }
  
      cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val = adc_data;

    }
    /* If therm channel then call rfi_read_therm, as it may need to be read from RTR rather than ADC */
    else if (cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id == RF_TDSCDMA_ADC_PA_THERM)
    {
      rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0, 
                                       RFCOM_TDSCDMA_MODE, 
                                       RFCOM_BAND_TDSCDMA_ANY_BAND,
                                       RFDEVICE_GET_THERMISTER_VALUE, 
                                       &adc_data);
      cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val = adc_data;
    }
    /* otherwise read from adc driver as usual */
    else
    {
     FTM_MSG_HIGH("Unsupported ADC reads", 0, 0, 0);
    }
    break;

  case FTM_GET_SYNTH_STATE:
    rfdevice_tdscdma_rx_cmd_dispatch(RFM_DEVICE_0, 
                                     RFCOM_TDSCDMA_MODE, 
                                     RFCOM_BAND_TDSCDMA_ANY_BAND,
                                     RFDEVICE_GET_RX_SYNC_LOCK, 
                                     &pll_status);
    cmd_ptr->rf_params.ftm_rf_factory_data.on_off = pll_status;
    break;

  case FTM_TDSCDMA_GET_TX_SYNTH_STATE:
    rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0, 
                                     RFCOM_TDSCDMA_MODE, 
                                     RFCOM_BAND_TDSCDMA_ANY_BAND,
                                     RFDEVICE_GET_TX_SYNC_LOCK, 
                                     &pll_status);
    cmd_ptr->rf_params.ftm_rf_factory_data.on_off = pll_status;
    break;

  case FTM_SET_LNA_OFFSET:
    ftm_tdscdma_set_lna_offset(ftm_tdscdma_get_rx_chain_handle( device),  (byte) cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id,
                             cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val);
    break;

  case FTM_TX_SWEEP_CAL:
    ftm_tdscdma_uplink_sweep_cal(cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    break;

  case FTM_GET_LNA_OFFSET:
    cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val =
    ftm_tdscdma_get_lna_offset( ftm_tdscdma_get_rx_chain_handle( device),
                              (uint8)(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id), (uint16)(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val),ftm_tdscdma_rx_lin_state );
    break;

  case FTM_GET_HDET_FROM_TX_SWEEP_CAL:
    /* Returns the HDET values stored in ftm_tdscdma_tx_cal_sweep_hdet_val[] during the previous Tx Cal Sweep */
    if (FTM_UPLINK_SWEEP_CAL_SIZE > cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id)
    {
      cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val =
            ftm_tdscdma_tx_cal_sweep_hdet_val[cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id];
      
    }
    else
    {
      FTM_MSG_HIGH("The input parameter is out of bound ...", 0, 0, 0);
    }
    break;

  case FTM_SET_HDET_TRACKING:
  	ftm_tdscdma_set_hdet_tracking((uint8)(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id),
  							  (uint16)(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val) );
  	FTM_MSG_MED("FTM_SET_HDET_TRACKING: %d %d",
                cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id,
                cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val, 0);
    break;

  case FTM_CONFIGURE_TX_SWEEP_CAL:
    ftm_tdscdma_configure_tx_sweep_cal(cmd_ptr->rf_params.ftm_rf_factory_data.id_val.id,
                                     (byte)cmd_ptr->rf_params.ftm_rf_factory_data.id_val.val);
    break;

  /* Legacy Not supported */
  /* 
  case FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL:
 */
  case FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2:
    for(i=0;i<FTM_NUM_HDET_TO_RETURN;i++)
    {
      cmd_ptr->rf_params.ftm_rf_factory_data.hdet_array_16_bit[i] = (uint16)ftm_tdscdma_tx_cal_sweep_hdet_val[i];
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
    ftm_tdscdma_set_tx_sweep_step_size(cmd_ptr->rf_params.ftm_rf_factory_data.gen_b);
    break;

  case FTM_DO_DC_CAL:
     ftm_tdscdma_do_dc_cal();
     break;

  case FTM_SET_SECONDARY_CHAIN:
    ftm_tdscdma_set_secondary_chain(cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    FTM_MSG_MED("FTM_SET_SECONDARY_CHAIN: %d", cmd_ptr->rf_params.ftm_rf_factory_data.on_off, 0, 0);
    break;

  case FTM_SECOND_CHAIN_TEST_CALL:
    ftm_tdscdma_second_chain_test_call(cmd_ptr->rf_params.ftm_rf_factory_data.gen_b);
    FTM_MSG_MED("FTM_SECOND_CHAIN_TEST_CALL: %d", cmd_ptr->rf_params.ftm_rf_factory_data.gen_b, 0, 0);
    break;

  case FTM_ENABLE_RF_TXPLIM_DEBUG_MSG:
    rf_tdscdma_core_txplim_dbgmsg = cmd_ptr->rf_params.ftm_rf_factory_data.gen_b;
    FTM_MSG_MED("Enable RF txplim debug message: %d", cmd_ptr->rf_params.ftm_rf_factory_data.gen_b, 0, 0);
    break;

  case FTM_ENABLE_EUL_WAVEFORM:
    ftm_tdscdma_eul_mpr_idx = ftm_tdscdma_configure_eul_waveform(
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
     cmd_ptr->rf_params.ftm_rf_factory_data.gen_b = ftm_tdscdma_eul_mpr_idx;
    break;

  case FTM_DISABLE_EUL_WAVEFORM:
    ftm_tdscdma_disable_eul_waveform();
    ftm_tdscdma_eul_mpr_idx = 0;
    break;

  case FTM_TDSCDMA_SET_TXAGC:
    ftm_tdscdma_set_txagc(
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_set_txagc_data.txagc,
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_set_txagc_data.parange_override,
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_set_txagc_data.parange);
    break;

  case FTM_TDSCDMA_TXAGC_SWEEP:
    ftm_tdscdma_txagc_sweep(
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_txagc_sweep_data.start,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_txagc_sweep_data.stop,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_txagc_sweep_data.step,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_txagc_sweep_data.delay,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_txagc_sweep_data.parange_override,
      cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_txagc_sweep_data.parange);
    break;
   
  case FTM_DO_IQ_MISMATCH_CAL:
    ftm_tdshspa_iq_mismatch_cal(ftm_tdscdma_get_rx_chain_handle(device), 
                             &cmd_ptr->rf_params.ftm_rf_factory_data.iqm_result);
    break;

  case FTM_DO_ICI_CAL:
    ftm_tdshspa_do_ici_cal(ftm_tdscdma_get_rx_chain_handle(device), 
                        &cmd_ptr->rf_params.ftm_rf_factory_data.ici.result);
    break;

    
  case FTM_TDSCDMA_CALV3_CONFIGURE_RADIO:
    rsp_pkt = ftm_tdscdma_calv3_configure_radio(cmd_ptr);
    break;

  case FTM_TDSCDMA_CALV3_RETUNE_RADIO:
    rsp_pkt = ftm_tdscdma_calv3_retune_radio(cmd_ptr);
    break;
  
  case FTM_TDSCDMA_CALV3_TEAR_DOWN_RADIO:
    rsp_pkt = ftm_tdscdma_calv3_tear_down_radio(cmd_ptr);
    break;
   
  case FTM_GET_RX_LEVEL_DBM:
    ftm_tdscdma_get_rx_level_dbm( device,
           &cmd_ptr->rf_params.ftm_rf_factory_data.rx_level_dbm_data.rx_level_agc.rx_level_agc_tdscdma,
           &cmd_ptr->rf_params.ftm_rf_factory_data.rx_level_dbm_data.rx_level_dbm.rx_level_dbm_tdscdma);
    break;

  case FTM_SET_FREQ_ADJUST:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.total_freq_adjust_hz.total_freq_adjust_hz_tdscdma = 
      ftm_tdscdma_set_freq_adjust(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.user_freq_adjust_ppb.user_freq_adjust_ppb_tdscdma,
                              cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.center_frequency_hz.center_frequency_hz_tdscdma,
                              cmd_ptr->rf_params.ftm_rf_factory_data.ftm_set_freq_adjust.enable_xo_freq_adjust
                              );
    break;

  case FTM_TDSCDMA_SET_BURST_TX_PARAMS:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_params.error_code = 
      ftm_tdscdma_set_burst_tx_params(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_params.num_slots,
                                    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_params.mod_type,
                                    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_params.ts1_pwrlvl,
                                    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_params.ts2_pwrlvl,
                                    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_params.ts3_pwrlvl,
                                    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_params.ts4_pwrlvl,
                                    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_params.ts5_pwrlvl
                                    );
    break;

  case FTM_TDSCDMA_BURST_TX:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_on_off.error_code = 
      ftm_tdscdma_burst_tx(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_tx_on_off.on_off);
    break;

  case FTM_TDSCDMA_BURST_RX:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_rx_params.error_code = 
      ftm_tdscdma_burst_rx(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_rx_params.exp_dl_pwr,
                         cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_burst_rx_params.enable
                         );
    break;

  case FTM_TDSCDMA_SWITCH_TX_WAVEFORM:
    /* 0 means SB, 1 means DPD waveform */
    /* todo - will not use on_off */
    ftm_tdscdma_switch_tx_waveform((byte)cmd_ptr->rf_params.ftm_rf_factory_data.on_off);
    break;

  case FTM_TDSCDMA_IQ_LOGGING:
	ftm_tdscdma_cmd_get_iq_data_from_fw(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_iq_cap_device);	
    break;

  case FTM_IQ_CAPTURE_GET_CONFIG:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_config.ftm_error_code =
    ftm_tdscdma_iq_capture_get_config(&cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_config);
    break;	

  case FTM_IQ_CAPTURE_ACQUIRE_SAMPLES:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples.ftm_error_code =
    ftm_tdscdma_iq_capture_acquire_samples(device, &cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_acquire_samples);
    break;	

  case FTM_IQ_CAPTURE_GET_SAMPLES:
    ftm_tdscdma_iq_capture_get_samples(device, &cmd_ptr->rf_params.ftm_rf_factory_data.ftm_iq_capture_cmd.ftm_iq_capture_get_samples, &rsp_pkt);
    break;	

  case FTM_IQ_CAPTURE_ANALYZE_SAMPLES:
    FTM_MSG_HIGH("The cmd is yet to be implemented...", 0, 0, 0); 
    break;

 case FTM_GET_MULTI_SYNTH_STATE:
     FTM_MSG_HIGH("MULTI_SYNTH:FTM_GET_MULTI_SYNTH_STATE received in TDS mode ",0,0,0);
     ftm_tdscdma_get_multi_synth_handler();
	 cmd_ptr->rf_params.ftm_rf_factory_data.multi_synth.tx_pll_state = tx_pll_multi_state;
	 cmd_ptr->rf_params.ftm_rf_factory_data.multi_synth.rx_pll_state = rx_pll_multi_state;
	 
     break;

 case FTM_RESET_MULTI_SYNTH_STATE:
     FTM_MSG_HIGH("MULTI_SYNTH:FTM_RESET_MULTI_SYNTH_STATE received in TDS mode ",0,0,0);
	 cmd_ptr->rf_params.ftm_rf_factory_data.on_off = ftm_tdscdma_reset_multi_synth_state();
    
     break;
 
 case FTM_INT_CALV3_RADIO_SETUP:
    rsp_pkt = ftm_tdscdma_calv3_radio_setup(cmd_ptr);
    break;

  case FTM_INT_CALV3_FEEDBACK_SETUP:
    rsp_pkt = ftm_tdscdma_calv3_feedback_setup(cmd_ptr);
    break;
 
  case FTM_GET_MULTIPLE_RX_LEVEL_DBM:
    ftm_tdscdma_get_multiple_rx_level_dbm( device,
                                           cmd_ptr->rf_params.ftm_rf_factory_data.get_multiple_rx_level_dbm_data.select_rx_chain,
                                           &rsp_pkt);
    break;

  case FTM_RF_MEAS_TX_PARAMS:
    ftm_tdscdma_fbrx_meas_tx_params( device,
                                     cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.num_averages,
                                     cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.capture_offset,
                                     cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_meas_tx_params.tx_measurement_config,
                                     &rsp_pkt );
    break;

  case FTM_FBRX_IQ_CAPTURE_TRIGGER:
    ftm_tdscdma_fbrx_iq_capture_trigger( device,
                                         cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_trigger.sample_size,
                                         cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_trigger.fbrx_chain,
                                         &rsp_pkt );
    break;

  case FTM_FBRX_IQ_CAPTURE_GET_SAMPLES:
    ftm_tdscdma_fbrx_iq_capture_get_samples( device,
                                              cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_source,
                                              cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_offset,
                                              cmd_ptr->rf_params.ftm_rf_factory_data.ftm_rf_fbrx_test_cmd.ftm_rf_fbrx_iq_capture_get_samples.sample_size,
                                              &rsp_pkt );
    break;

  case FTM_TDSCDMA_TRIGGER_PORT_SWITCH:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_asm_update_params.error_code = 
      ftm_tdscdma_trigger_asm_update(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_asm_update_params.asm_port_state);
    break;

  case FTM_SET_TUNER_TUNE_CODE_OVERRIDE:
    cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.result = 
	  ftm_tdscdma_set_tuner_tune_code_override( device,
                                                    (uint8)cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.override_flag,
                                                    (void*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.config_data),
                                                    (uint8*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.nv_data),
                                                    (uint8*)&(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tuner_tunecode_override_data.tuner_id));
    break;

  case FTM_INT_CALV3_XPT_IQ_CAPTURE:
    rsp_pkt = ftm_tdscdma_cal_xpt_iq_capture(cmd_ptr);
    break;

  case FTM_TDSCDMA_SET_RX_LIN_STATE:
      ftm_tdscdma_set_rx_lin_state(cmd_ptr->rf_params.ftm_rf_factory_data.rx_lin_state);
  break;

  case FTM_SET_ALT_PATH_OVERRIDE_INDEX:
    ftm_tdscdma_set_alt_path(cmd_ptr->rf_params.ftm_rf_factory_data.ftm_tdscdma_alt_path_oride_params);
    break;

  case FTM_TDSCDMA_SET_DEVICES:
  	ftm_tdscdma_set_devices(cmd_ptr);
  break;

  default:
    FTM_MSG_HIGH("The cmd is not supported ...", 0, 0, 0);
    break;
  }
  return rsp_pkt;
} /* end ftm_tdscdma_dispatch */

#endif /* FTM_HAS_TDSCDMA */
#endif /* FEATURE_FACTORY_TESTMODE */
