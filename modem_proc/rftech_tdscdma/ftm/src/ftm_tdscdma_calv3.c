/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            F T M   T D S C D M A   C A L V 3

GENERAL DESCRIPTION
  This is the FTM RF file which contains implementations for TDSCDMA CalV3

Copyright (c) 2009 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/src/ftm_tdscdma_calv3.c#1 $
  $DateTime: 2016/12/13 07:59:50 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/17/15   kma     Added auto pin support for Pin cal
11/20/15   ych     Added FTM exit mode in tear-down of radio
01/06/15   jak     keep div status consistent in radio setup and tear down  
09/15/14   dj      Use only the abstracted API for FB PWR read 
09/13/14   dj      Fixed fb power read for TX_MEAS_FB_PWR
10/08/14    ka     Replace rfc with device api for retrieving device pointer
09/28/14   jyu     Fixed bug for DR-DSDS
09/21/14   jyu     Added RFcal support for DR-DSDS 
09/03/14   cd      Remove legacy Tx override fields from data module
08/18/14   whz     Fixed an invalid band parameter
08/05/14   ms      Added support for TX_MEAS_FB_PWR action in TxMeas
07/17/14   ms      Sawless RX support
07/11/14   jmf     Added support for CONFIGURE_RXLM_FOR_FBAGC
07/10/14   qzh     Add support for set_mode node is not present scenario when RF cal
07/09/14   ych     Added XPT IQ capture offloading to RF thread
07/02/14   kr/kai  Non-0dB Pin SW implementation to support -ve pin offsets values
06/16/14   vs/kai  Non-0dB Pin SW implementation
05/14/14   kb      Use correct band type to enable HDET in FTM mode. 
03/24/14    aa     Remove hardcoded value for fbrx status check
03/21/14   zg      Removed rfc_common_get_cmn_device_object() from TDSCDMA client. 
02/14/14   jhe     Clear DPD map and delay map at the begining of calibration.
02/13/14    id     Updates needed for FBRx Cal API to match FBRx capture API
02/10/14   jhe     Removed the pin/pout debug F3s
01/31/14   jmf     Added support for FBRx Cal type captures and feedback setup
01/13/14    id     Add support for FB Gain Meas in TX Meas Opcode
01/08/14   cac     Update apply_pin_alignment to reflect change in the ftm common API.
01/02/14   zg      Fixed a bug in ftm_tdscdma_calv3_rx_measure() w.r.t device input.
12/25/13   jps     Change Tx override handlers to use RFLM Tx override for xPT Cal
12/15/13   jps     Fixes for RFLM based TXAGC override
12/04/13   kb      [XPT] Clear pin map at the begining of calibration.
10/23/13   qzh     Fix KW warnings
09/19/13   kb      Fix BOLT compiler warning.
09/17/13   dj      If feedback action contains only WTR config command, call it
                   in ISR context
09/05/13   kb      1. Remove redundant IQ capture code
                   2. Remove B40 hardcoding from Tx_Measure API
08/28/13   kb      Fix Klocwork warnings.
08/26/13   ych     Replace memcpy() with memscpy().
08/22/13   kb      [XPT] Read Therm ADC value during Tx_Measure to write to NV.
08/22/13   kb      [XPT] Support for Pin and Pout for DPD cal v4
08/21/13   dj      Moved feedback_setup to be run from the tdscdma command dispatch
08/19/13   kb      [XPT] Support for cal_config, iq_capture and waveform switching.
08/08/13   dj      Added error messages for unsupported fb_set_up actions
08/02/13   dj      Updated feedback setup to use fb_state instead of pa_state
07/23/13   ych     Opcodes calv4 support
06/26/13   jyu     Added support to remove the requirement to specify band info
                   when it is not ready
06/18/13   ms      Change default band to B34
03/21/13   ms      Tuner TDET support
03/05/13   jyu     Fixed a bug to enable composite cal on diversity chain
02/08/13   jyu     Removed hard-coded device info
12/06/12   aki     Added RX threshold aka RX wait functionality
12/04/12   ms      APT support
11/05/12   aro     Re-organized cal v3 data structure to be more modular
07/31/12   qzh     Rfdevice interface change to use rfm_device instead of path
03/26/12   hy      TD-SCDMA Cal time optimization
01/18/12   hy      Fixed compiler errors due to common struct change
11/16/11   hy      Initial TDSCDMA V3 Cal
===========================================================================*/

/*===========================================================================
 *        Include Files                                                     *
===========================================================================*/
#include <stringl/stringl.h>

#include "rfa_variation.h"
#include "diagcmd.h"
#include "ftmdiag.h"
#include "ftm_msg.h"
#include "ftm.h"
#include "ftm_common.h"
#include "ftm_common_control.h"
#include "ftm_common_fbrx.h"
#include "rfcom.h"
#include "rfdevice_intf_cmd.h"
#include "rfdevice_type_defs.h"
#include "rfm_device_types.h"
#include "rfcommon_math.h"
#include "ftm_rf_cmd.h"
#include "rfm.h"
#include "rfm_ftm.h"
#include "rfcommon_data.h"

#include "ftm_common_calibration_v3.h"
#include "ftm_common_data.h"
#include "ftm_tdscdma_ctl.h"
#include "ftm_tdscdma_dispatch.h"
#include "ftm_tdscdma_calv3.h"
#include "rfdevice_tdscdma_intf.h"
#include "rf_tdscdma_core_util.h"
#include "rfdevice_hdet_tdscdma_intf.h"
#include "rfcommon_mc.h"
#include "rfcommon_core_txlin_types.h"
#include "rfc_card_tdscdma.h"
#include "rfcommon_fbrx_types.h"
#include "rf_tdscdma_core_xpt.h"
#include "rflm_api_fbrx_fw_hal.h"
#include "rfdevice_hdet_cmn_intf.h"
#include "rfcommon_autopin_api.h"

#include <math.h>

/*===========================================================================
 *        Definitions and Variables/Data Structures                         *
===========================================================================*/
#define  FTM_TDSCDMA_CALV3_SLAVE_MASK        0x0000000f
#define  FTM_TDSCDMA_CALV3_SLAVE_ENABLE_VAL  0x00000001
#define  FTM_TDSCDMA_CALV3_SLAVE_DISABLE_VAL 0x0000000f

/*-----------------------------------------------------------------------------*/
static rftx_pdm_limit_type ftm_tdscdma_calv3_pdm_limits;  /*!< Tx PDM min and max limits */

extern rfcom_tdscdma_band_type ftm_curr_tdscdma_mode; /*!< Current TD mode */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static boolean ftm_tdscdma_calv3_dbg_msg_flag = TRUE; /*!< Flag for enabling
TDSCDMA CalV3 debug messages */

/*----------------------------------------------------------------------------*/
extern boolean ftm_tdscdma_ctl_dbg_msg_flag; /*!< Flag for enabling
TDSCDMA FTM debug messages. CalV3 may override this flag */

/*----------------------------------------------------------------------------*/
extern ftm_mode_id_type ftm_tdscdma_current_mode_id;

/*----------------------------------------------------------------------------*/
static timetick_type ftm_tdscdma_prof_start_time = 0; /*<! Time profile variable  */

extern rftx_pdm_limit_type ftm_tdscdma_pdm_limits;

/*----------------------------------------------------------------------------*/
extern ftm_pkt_data_type ftm_pkt_data; /*!< Current FTM command */

extern ftm_rf_mode_type ftm_current_rf_mode; /*!< To track the current RF mode */

/*----------------------------------------------------------------------------*/
extern boolean ftm_tdscdma_div_status; /*!< To track the current diversity status */

extern uint16 ftm_tdscdma_current_chan; /*!< To track the current channel- */

extern boolean ftm_tdscdma_tx_on_status;
/*----------------------------------------------------------------------------*/
//extern ftm_lm_buf_type ftm_tdscdma_rxlm_buffer[]; /*!< RXLM buffer */

extern uint8 rf_tdscdma_core_temp_comp_scale_therm(uint16 therm_raw_value, boolean save);

/*===========================================================================
 *        Local Functions, Variables/Data Structures                        *
===========================================================================*/

/*----------------------------------------------------------------------------*/
uint16 ftm_tdscdma_calv3_prx_drx_tune_wait = 500; /*!< Place holder for wait time */

LOCAL boolean drx_enable_flag = FALSE; /* Enable DRx indicator */


LOCAL void ftm_tdscdma_calv3_tune_band_chan(ftm_mode_id_type mode,
                                          uint16 chan,
                                          uint16 dc_chan_offset,
                                          boolean drx_enable_flag);

LOCAL void ftm_tdscdma_calv3_set_current_mode(rfcom_tdscdma_band_type band);

LOCAL rfcom_tdscdma_band_type ftm_tdscdma_calv4_convert_band(uint16 band);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_TDSCDMA_CALV3_SWEEP_CONFIGURE_RADIO
  Configure Radio for Calibration Sweep

  @details
  Based on the RF Configuration paylaod, this function will configure the radio.
  All radio tuning actions will be done under FTM Task context by sending
  Radio tune command to FTM Task Q.

  @param rf_cfg
  Pointer to the data structure containing RF Configuration parameter:
      device_master: Bit mask indicating which devices/chain to config
      device_slave:  Not used here
      rf_mode:       RF Mode
      channel:       UL channel

 @return
 boolean that indicates whether there had been an error or not
*/
boolean ftm_tdscdma_calv3_sweep_configure_radio
(
  ftm_cal_instr_payload_config_radio_type const *rf_cfg
)
{
  ftm_tdscdma_cal_sweep_configure_radio_req_type cfg_req;

  ftm_tdscdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;
  ftm_tdscdma_ctl_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Check for NULL ptr */
  if(rf_cfg == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_configure_radio: NULL rf_cfg ptr!");
    return FALSE;
  }

  /* Create radio request for config_radio */
  cfg_req.cmd = DIAG_SUBSYS_CMD_F;
  cfg_req.sub_sys = FTM;
  cfg_req.mode = FTM_TDSCDMA_C;
  cfg_req.tdscdma_cmd = FTM_TDSCDMA_CALV3_CONFIGURE_RADIO;
  cfg_req.req_len = 0;
  cfg_req.rsp_len = 0;
  cfg_req.device_master_mask = rf_cfg->device_master;
  cfg_req.device_slave = rf_cfg->device_slave;
  cfg_req.band = rf_cfg->rf_mode;
  cfg_req.channel = rf_cfg->channel;
  cfg_req.dc_channel_offset = rf_cfg->dc_channel_offset;

  /* Queue up the FTM command */
  ftm_common_send_radio_command( &cfg_req,
                                sizeof(ftm_tdscdma_cal_sweep_configure_radio_req_type) );

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_6(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_configure_radio: mode = %d, chan = %d, device_master = %d, device_slave = %d, DC_chan_offset = %d, elapse time: %d",
              rf_cfg->rf_mode,
              rf_cfg->channel,
              rf_cfg->device_master,
              rf_cfg->device_slave,
              rf_cfg->dc_channel_offset,
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  return TRUE;
} /* ftm_tdscdma_calv3_sweep_configure_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_TDSCDMA_CALV3_SWEEP_RETUNE_RADIO
  Retune Radio for calibration based on input parameters

  @details
  Retune Radio for calibration based on input parameters

  @param rf_retune
  Pointer to the data structure containing RF retune parameter:
      primary chain
      ul channel

 @return
 boolean that indicates whether there had been an error or not
*/
boolean ftm_tdscdma_calv3_sweep_retune_radio
(
   ftm_cal_instr_payload_retune_radio_type const *rf_retune
)
{
   ftm_tdscdma_cal_sweep_retune_radio_req_type retune_req;

   ftm_tdscdma_prof_start_time = timetick_get();

   /* Check for NULL ptr */
   if(rf_retune == NULL)
   {
     FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_retune_radio: NULL rf_cfg ptr!");
     return FALSE;
   }

   /* Create radio request for retune_radio */
   retune_req.cmd = DIAG_SUBSYS_CMD_F;
   retune_req.sub_sys = FTM;
   retune_req.mode = FTM_TDSCDMA_C;
   retune_req.tdscdma_cmd = FTM_TDSCDMA_CALV3_RETUNE_RADIO;
   retune_req.req_len = 0;
   retune_req.rsp_len = 0;
   retune_req.device_master_mask = rf_retune->device_master;
   retune_req.channel = rf_retune->channel;
   retune_req.dc_channel_offset = rf_retune->dc_channel_offset;

   /* Queue up the FTM command */
   ftm_common_send_radio_command( &retune_req,
                                 sizeof(ftm_tdscdma_cal_sweep_retune_radio_req_type) );

   /* Print debug msg if enabled */
   if(ftm_tdscdma_calv3_dbg_msg_flag)
   {
     FTM_MSG_4(FTM_HIGH,
               "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_retune_radio:chan = %d, device_master = %d, DC_chan_offset = %d, elapse time: %d",
               rf_retune->channel,
               rf_retune->device_master,
               rf_retune->dc_channel_offset,
               timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
               );
   }

   return TRUE;
} /* ftm_tdscdma_calv3_sweep_retune_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_TDSCDMA_CALV3_SWEEP_TEAR_DOWN_RADIO
  Tear Down Radio for calibration sweep

  @details
  This function will tear down all radios that are tuned to TDSCDMA mode to
  sleep state.

 @return
 boolean that indicates whether there had been an error or not
*/
boolean ftm_tdscdma_calv3_sweep_tear_down_radio(void)
{

  ftm_tdscdma_cal_sweep_tear_radio_req_type tear_down_req;

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Create radio request for retune_radio */
  tear_down_req.cmd = DIAG_SUBSYS_CMD_F;
  tear_down_req.sub_sys = FTM;
  tear_down_req.mode = FTM_TDSCDMA_C;
  tear_down_req.tdscdma_cmd = FTM_TDSCDMA_CALV3_TEAR_DOWN_RADIO;
  tear_down_req.req_len = 0;
  tear_down_req.rsp_len = 0;

  /* Queue up the FTM command */
  ftm_common_send_radio_command( &tear_down_req,
                                sizeof(ftm_tdscdma_cal_sweep_tear_radio_req_type) );

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_1(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_tear_down_radio:elapse time: %d",
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  ftm_tdscdma_ctl_dbg_msg_flag = TRUE;//after tear down, re-enable the flag.

  return TRUE;
} /* ftm_tdscdma_calv3_sweep_tear_down_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_TDSCDMA_CALV3_SWEEP_CONFIGURE_RX
  Configure Rx for Calibration Sweep

  @details
  Configure Rx for calibration based on input parameters. cal_action field in
  the input param is a bit mask that indicates the LNA state. For TDSCDMA only
  ONE action is supported per segment. This function will return FALSE if
  there is more than one cal_action bits.

  rx_flag field in the input param is a bit mask that indicates which
  rf device(s) to be used. Only bits 0 and 1 are used for TDS where both
  bits can be enabled for a given segment.

  @param rx_config
  Pointer to the data structure containing RF config parameter:
      cal_action: 0 - 5 indicates LNA 0 - LNA 5 calibration respectively
      power_mode: used by cdma for linearity only
      exp_rxagc: Expected RxAGC
      rx_flag: Rx Flag, B[3:0] indicates Rx device

 @return
 boolean that indicates whether there had been an error or not
*/
boolean ftm_tdscdma_calv3_sweep_configure_rx
(
   ftm_cal_instr_payload_config_rx_type const *rx_config
)
{
  byte lna_range;
  byte rx_lin_state;

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Check if input is NULL */
  if(rx_config == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_configure_rx: NULL input ptr!");
    return FALSE;
  }

  /* Check if there is only one cal action
     TDSCDMA supports only one Rx Cal action per segment,
     e.g. 1 LNA state per segment
  */
  if( rf_count_one_bits((uint32)rx_config->cal_action) != 1 )
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_configure_rx: More than one cal action!");
    return FALSE;
  }

  switch(rx_config->power_mode)
  {
    case 1:
    rx_lin_state = (byte)FTM_RX_LIN_STATE_HIGH;
    break;

    case 0:
    default:
    rx_lin_state = (byte)FTM_RX_LIN_STATE_LOW;
    break;
  }

  /* Get the LNA range from cal_action */
  switch(rx_config->cal_action)
  {
    case RX_ACTION_LNA0_CAL:
      lna_range = (byte)FTM_LNA_RANGE_0;
      break;

    case RX_ACTION_LNA1_CAL:
      lna_range = (byte)FTM_LNA_RANGE_1;
      break;

    case RX_ACTION_LNA2_CAL:
      lna_range = (byte)FTM_LNA_RANGE_2;
      break;

    case RX_ACTION_LNA3_CAL:
      lna_range = (byte)FTM_LNA_RANGE_3;
      break;

    case RX_ACTION_LNA4_CAL:
      lna_range = (byte)FTM_LNA_RANGE_4;
      break;

    case RX_ACTION_LNA5_CAL:
      lna_range = (byte)FTM_LNA_RANGE_5;
      break;

    default:
      lna_range = (byte)FTM_LNA_RANGE_MAX;
      break;
  }

  /* Check if LNA range is valid */
  if (lna_range == (byte)FTM_LNA_RANGE_MAX)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_configure_rx: Invalid LNA range!");
    return FALSE;
  }

  /*TDSCDMA specific, set to Rx state. this will tear down Tx, if previously in Tx state.*/
  ftm_tdscdma_set_rx(TRUE);

  /* Setup for PRx */
  if(rx_config->device_flag & DEVICE_FLAG_0)
  {
    /* Reset the indexed LNA Offset to zero */
    ftm_tdscdma_set_lna_offset( FTM_RECEIVE_CHAIN_0, lna_range, 0);

    /* Set the LNA range to the current index*/
    ftm_tdscdma_set_lna_range( FTM_RECEIVE_CHAIN_0, lna_range, rx_lin_state );
  }

  if(rx_config->device_flag & DEVICE_FLAG_2)
  {
    /* Reset the indexed LNA Offset to zero */
    ftm_tdscdma_set_lna_offset( FTM_RECEIVE_CHAIN_2, lna_range, 0);

    /* Set the LNA range to the current index*/
    ftm_tdscdma_set_lna_range( FTM_RECEIVE_CHAIN_2, lna_range, rx_lin_state );
  }

  /* Setup for DRx */
  if(rx_config->device_flag & DEVICE_FLAG_1)
  {
    /* Reset the indexed LNA Offset to zero */
    ftm_tdscdma_set_lna_offset( FTM_RECEIVE_CHAIN_1, lna_range, 0);

    /* Set the LNA range to the current index*/
    ftm_tdscdma_set_lna_range( FTM_RECEIVE_CHAIN_1, lna_range, rx_lin_state );
  }

  if(rx_config->device_flag & DEVICE_FLAG_3)
  {
    /* Reset the indexed LNA Offset to zero */
    ftm_tdscdma_set_lna_offset( FTM_RECEIVE_CHAIN_3, lna_range, 0);

    /* Set the LNA range to the current index*/
    ftm_tdscdma_set_lna_range( FTM_RECEIVE_CHAIN_3, lna_range, rx_lin_state );
  }

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_4(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_configure_rx: rx_flag (devices) = 0x%x, cal_action = %d (which is lna_range %d), elapse time %d",
              rx_config->device_flag,
              rx_config->cal_action,
              lna_range,
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  return TRUE;

} /* ftm_tdscdma_calv3_sweep_configure_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_TDSCDMA_CALV3_SWEEP_CONFIGURE_TX
  Configure Tx for Calibration Sweep

  @details
  Configure Tx for calibration sweep based on input parameters.

  @param tx_config
  Pointer to the data structure containing Tx measurement parameter:
     pa_state: PA State to which the TxAGC is to be overridden during Tx Measurement
     rgi: RTR Gain Index to be overridden to
     pa_bias: PA Bias value to be applied
     tx_flag: Bit mask corresponding to Tx Device to be used
              i.e. B[0] : RFM Device 0 calibration

 @return
 boolean that indicates whether there had been an error or not
*/
boolean ftm_tdscdma_calv3_sweep_configure_tx
(
   ftm_cal_instr_payload_config_tx_type const *tx_config
)
{
  uint8 pa_range;

  /* Start of profile */
  ftm_tdscdma_prof_start_time = timetick_get();

  /* Check if input is NULL ptr */
  if(tx_config == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_configure_tx: NULL input ptr!");
    return FALSE;
  }
  /* Check if it is device_0 */
  else if(tx_config->device_flag != (ftm_calibration_device_flag_mask_type)DEVICE_FLAG_0)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_configure_tx: Invalid Tx device!");
    return FALSE;
  }
  else if (tx_config->pa_state == 255)//invalid
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_configure_tx: Invalid pa_state!");
    return FALSE;
  }

  /*TDSCDMA specific, set to Tx state. this will tear down Rx, if previously in Rx state.*/
  ftm_tdscdma_set_tx(TRUE);



  /* Set SMPS bias override flag and set PA bias value */
  ftm_tdscdma_set_smps_pa_bias_override(TRUE);
  ftm_tdscdma_set_smps_pa_bias_val((uint16)tx_config->pa_bias);


  /* Set PA Range
     Input is PA state. Convert it to PA range first and then set PA range
  */
  pa_range = rf_tdscdma_core_util_get_pa_range_from_nv(tx_config->pa_state);
  ftm_tdscdma_set_pa_range(pa_range);

  /* Set PDM */
  ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)tx_config->rgi);

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_5(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_configure_tx: pa_bias = %d, pa_state = %d, pa_range = %d, rgi = %d, elapse time %d",
              tx_config->pa_bias,
              tx_config->pa_state,
              pa_range,
              tx_config->rgi,
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  return TRUE;

} /* ftm_tdscdma_calv3_sweep_configure_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_TDSCDMA_CALV3_SWEEP_MEASURE_RX
  Measure Rx for calibration sweep

  @details
  Measure Rx for calibration sweep and populate results (LNA offsets)
  in to the input result buffer.

  @param rx_meas
  Pointer to the data structure containing Rx configuration parameter:

  cal_action: Bit mask corresponding to LNA state
     BIT[0]: Corresponds to gain state 0 -> get LNA0 offset
     BIT[x]: Corresponds to gain state x -> get LNA offset
  power_mode: Used by cdma only
  exp_rxagc: Expected RxAGC
  rx_flag:
     Bit mask corresponding to device
     BIT[0]: Corresponds to primary chain
     BIT[1]: Corresponds to secondary chain

  @param rx_result
  Result buffer where Rx measurement results are to be populated in.
  Number of measurements is also populated here when completed.
  Contains max_rx_buffer_len field which indicates the size of the result
  buffer. This is used for checking against buffer overflow.

 @return
 boolean that indicates whether there had been an error or not
*/
boolean ftm_tdscdma_calv3_sweep_measure_rx
(
  ftm_cal_instr_payload_meas_rx_type const *rx_meas,
  ftm_calibration_rx_result_type *rx_result
)
{

  boolean success_flag = TRUE;           /* Function success flag               */
  uint32  cal_action_mask = 0xFFFFFFFF;  /* Mask that indicates which LNA state */

  uint8   rx_meas_count = 0;             /* Total measurement result count      */
  uint16  rx_meas_size_count = 0;        /* Total measurement result size       */

  ftm_receive_chain_handle_type device;  /* Rx chain of interest                */
  uint32  rx_flag;                       /* Rx device(s) for measurement        */
  uint32  rx_flag_mask = 0xFFFFFFFF;     /* Mask for looping through Rx devices */

  byte    lna_range = 0;                 /* LNA range                           */
  byte    rx_lin_state = 0;                 /* HL/LL state                          */

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Check for NULL ptr(s) */
  if((rx_meas == NULL) || (rx_result == NULL) || (rx_result->rx_buffer == NULL))
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_measure_rx: NULL ptr(s)!");
    return FALSE;
  }

  /* Get LNA gain state for storing result(s) */
  cal_action_mask = rf_extract_first_one_bit_mask( (uint32)rx_meas->cal_action );

  /* Check for valid cal action */
  if( (cal_action_mask != (uint32)rx_meas->cal_action) ||  /* Check if there is only one action */
      (cal_action_mask > (uint32)RX_ACTION_LNA5_CAL)       /* Check if LNA state is valid */
    )
  {
    FTM_MSG_2(FTM_ERROR, "ftm_tdscdma_calv3_sweep_measure_rx: Invalid cal_action: cal_action_mask = %d, cal_action = %d",
              cal_action_mask,rx_meas->cal_action);
    return FALSE;
  }

  /* Get the Rx device information */
  rx_flag = (uint32)rx_meas->device_flag;


  /* Get the HL/LL from rx_lin_action */
  switch(rx_meas->power_mode)
  {
    case 1:
    rx_lin_state = (byte)FTM_RX_LIN_STATE_HIGH;
    break;

    case 0:
    default:
    rx_lin_state = (byte)FTM_RX_LIN_STATE_LOW;
    break;
  }


  /* Get the LNA range from cal_action */
  switch(rx_meas->cal_action)
  {
   case RX_ACTION_LNA0_CAL:
     lna_range = (byte)FTM_LNA_RANGE_0;
     break;

   case RX_ACTION_LNA1_CAL:
     lna_range = (byte)FTM_LNA_RANGE_1;
     break;

   case RX_ACTION_LNA2_CAL:
     lna_range = (byte)FTM_LNA_RANGE_2;
     break;

   case RX_ACTION_LNA3_CAL:
     lna_range = (byte)FTM_LNA_RANGE_3;
     break;

   case RX_ACTION_LNA4_CAL:
     lna_range = (byte)FTM_LNA_RANGE_4;
     break;

   case RX_ACTION_LNA5_CAL:
     lna_range = (byte)FTM_LNA_RANGE_5;
     break;

   default:
     lna_range = (byte)FTM_LNA_RANGE_MAX;
     break;
  }

  /* Loop through all device(s) of interest */
  while ( rx_flag_mask != 0 )
  {
    /* For the current unprocessed Rx Flag. The first bit will be from LSB. */
    rx_flag_mask = rf_extract_first_one_bit_mask( rx_flag );

    /* Determine RFM device based on the given flag */
    switch ( rx_flag_mask )
    {

    case DEVICE_FLAG_0:
      device = FTM_RECEIVE_CHAIN_0;
      break;

    case DEVICE_FLAG_1:
      device = FTM_RECEIVE_CHAIN_1;
      break;

    default:
      device = FTM_RECEIVE_CHAIN_INVALID;
      break;
    }

    /* If the device is not Invalid, Get Rx measurement */
    if ( device != FTM_RECEIVE_CHAIN_INVALID )
    {
      /* Check the rx measurement counter against the maximum allowed
         number of measurements */
      if ( rx_result->max_rx_buffer_len >=
           (rx_meas_size_count + sizeof(ftm_calibration_rx_meas_data_type)) )
      {
        /* Get Rx measurements */

        {
          /* Store LNA offset result */
          ((ftm_calibration_rx_meas_data_type*)(rx_result->rx_buffer))[rx_meas_count] =
            (ftm_calibration_rx_meas_data_type)ftm_tdscdma_get_lna_offset(device,lna_range,(word)rx_meas->exp_rxagc,rx_lin_state);

          /* Print debug msg if enabled */
          if(ftm_tdscdma_calv3_dbg_msg_flag)
          {
            FTM_MSG_4(FTM_HIGH,
                      "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_measure_rx: Rx chain = %d, LNA range = %d, Exp Rx_AGC = %d, LNA offset = %d",
                      device,
                      lna_range,
                      rx_meas->exp_rxagc,
                      ((ftm_calibration_rx_meas_data_type*)(rx_result->rx_buffer))[rx_meas_count]
                      );
          }

        } /* LNA cal */

        /* Increment the rx_meas counter */
        rx_meas_size_count += sizeof(ftm_calibration_rx_meas_data_type);

        /* Increment the measurement counter */
        rx_meas_count ++;
      }

      else
      {
        /* Buffer Overflow. Break loop immediately */
        success_flag = FALSE;
        break;
      }
    } /* if ( device != RFM_INVALID_DEVICE ) */

    /* Update Rx Flag to set the processed bit to zero. */
    rx_flag = rx_flag & (~rx_flag_mask);

  } /* while ( rx_flag_mask != 0 ) */


  /* If measurements go through then update meas struct */
  if(success_flag)
  {
    /* Update the total number of rx measurements taken during this segment. */
    rx_result->num_of_rx_reads = rx_meas_count;
  }
  else
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_measure_rx: Measurement buffer overflow!");
  }

  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_1(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_measure_rx: elapse time %d",
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  return success_flag;

} /* ftm_tdscdma_calv3_sweep_measure_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM_TDSCDMA_CALV3_SWEEP_MEASURE_TX
  Perform an hdet read bases on passed-in parameter.

  @details
  Perform an hdet read bases on passed-in parameter.

  @param hdet_meas
  Pointer to the data structure containing Tx measurement settings:
     hdet_sample:       Number of HDET Samples to be taken for averaging
     lpm_hdet_sample:   Used in CDMA only
     tx_flag:           Which chain of interest

  @param tx_result
  Pointer to data structure to populate measurement results
     tx_buffer:         Array to populate HDET results
     num_of_tx_reads:   Number of HDET reads
     max_tx_buffer_len: Size of the tx_buffer allocated

  Result buffer where Tx measurement result is to be populated in.
  Number of measurements is also populated here when completed.
  Contains max_tx_buffer_len field which indicates the size of the result
  buffer. This is used for checking against buffer overflow.

 @return
 boolean that indicates whether there had been an error or not
*/
boolean ftm_tdscdma_calv3_sweep_measure_tx
(
  ftm_cal_instr_payload_meas_hdet_type const *hdet_meas,
  ftm_calibration_tx_result_type *tx_result

)
{
  boolean success_flag = TRUE;  /* Return value           */
  uint16  hdet_accm = 0;        /* HDET read accumuulator */
  uint16  temp_hdet_val  = 0;   /* temp HDET read         */
  uint8   hdet_idx = 0;         /* read index             */
  uint8   local_read_count;     /* read counter           */
  uint8   tx_meas_count = 0;    /* Tx measurement count   */
  const int8 hdet_max_num_retries = 1; /*this value used to 3. use one to avoid isr overrun. return an err if failed*/

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Check for NULL ptr(s) */
  if( (hdet_meas == NULL) || (tx_result == NULL) ||(tx_result->tx_buffer == NULL) )
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_measure_tx: NULL input ptr(s)!");
    return FALSE;
  }

  /* Check if device of interest is valid */
  if(hdet_meas->device_flag != DEVICE_FLAG_0)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_tdscdma_calv3_sweep_measure_tx: Invalid Device: %d!",hdet_meas->device_flag);
    return FALSE;
  }

  /* Check for potential buffer overflow */
  if ( tx_result->max_tx_buffer_len < sizeof(ftm_calibration_tx_meas_data_type) )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_tdscdma_calv3_sweep_measure_tx: Insufficient buffer size %d!",tx_result->max_tx_buffer_len);
    return FALSE;
  }

  /* Check for div-by-0 */
  if(hdet_meas->hdet_sample == 0)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_measure_tx: hdet_sample input is 0!");
    return FALSE;
  }


  /* Get HDET measurements */
  for(hdet_idx = 0; hdet_idx < hdet_meas->hdet_sample; hdet_idx++)
  {
    /* Do up to 3 tries per HDET read */
    local_read_count = 0;
    do
    {
      if(is_wtr_hdet)
      {
        rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                         RFCOM_TDSCDMA_MODE,
                                         RFCOM_BAND_TDSCDMA_ANY_BAND,
                                         RFDEVICE_GET_HDET_VALUE,
                                         &temp_hdet_val);
      }
      else
      {
        (void)rfdevice_hdet_tdscdma_tx_hdet_read(RFM_DEVICE_0,
                                                 RFCOM_BAND_TDSCDMA_ANY_BAND,
                                                 FALSE,
                                                 &temp_hdet_val,
                                                 NULL,
                                                 RFDEVICE_EXECUTE_IMMEDIATE);
      }
      local_read_count++;

    }while ( ((temp_hdet_val <= 0) || temp_hdet_val == 0xFFFF)
             && (local_read_count < hdet_max_num_retries));

    /* Add to accum */
    hdet_accm += temp_hdet_val;
  }

  /* Take avg */
  hdet_accm = hdet_accm / hdet_meas->hdet_sample;

  /* Populate result */
  ((ftm_calibration_tx_meas_data_type*)(tx_result->tx_buffer))[tx_meas_count] = hdet_accm;
  tx_meas_count++;

  /* Update number of Tx measurements */
  tx_result->num_of_tx_reads = tx_meas_count;

  if (hdet_accm == 0xFFFF)//invalid
  {
    FTM_MSG_1(FTM_ERROR, "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_measure_tx: hdet %d failed to converge.", hdet_accm);
  }

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_4(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_measure_tx: tx_flag = %d, HDET samples = %d, avg_hdet = %d, elapse time %d",
              hdet_meas->device_flag,
              hdet_meas->hdet_sample,
              hdet_accm,
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  return success_flag;

} /* ftm_tdscdma_calv3_sweep_measure_tx */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_TDSCDMA_CALV3_CONFIGURE_RADIO
  Configures radio for calibration sweep.

  @details
  Configures RF based on passed in parameters to prepare for sweep.
  ftm_tdscdma_calv3_sweep_configure_radio (which executes in ISR context)
  populates the required fields and pushes cmd to ftm. ftm will then
  pick up the cmd and execute this function in ftm context.

  @param ftm_req_data
  Pointer to ftm_tdscdma_cal_sweep_configure_radio_req_type:
     mode:
       RF mode
     channel
       RF channel
     device_master_mask
       8-bit parameter
       For TDS we only use the first bit which indicates Tx device 0
     device_slave
       32-bit parameter
       Each master bit has usage of 4 bits of the slave_device.
       If the 4-bit val is 0xF then it is don't-care
       For TDS, we only use the last four bits: 0x1 - DRx enabled, 0xF - disabled

 @return
 ftm_rsp_pkt_type
*/
ftm_rsp_pkt_type ftm_tdscdma_calv3_configure_radio(void *ftm_req_data)
{
  /* Create response Packets */
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};

  /* Pointer to config information */
  ftm_tdscdma_cal_sweep_configure_radio_req_type* header = NULL;

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Get Request Packet */
  header = (ftm_tdscdma_cal_sweep_configure_radio_req_type*)ftm_req_data;

  /* Print debug msg if enabled: Function start time tick */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_4(FTM_HIGH,
              "TDSCDMA CAlV3 DBG - ftm_tdscdma_calv3_configure_radio: mode %d, band, %d chan %d, dc_channel_offset %d",
              header->mode,
              header->band,
              header->channel,
              header->dc_channel_offset
              );
  }

  drx_enable_flag = FALSE; /* Enable DRx indicator */

  /* Check if DRx is required */
  if((header->device_slave & FTM_TDSCDMA_CALV3_SLAVE_MASK) == FTM_TDSCDMA_CALV3_SLAVE_ENABLE_VAL )
  {
    drx_enable_flag = TRUE;
  }

  /* Tune Band and Chan */
  ftm_tdscdma_calv3_tune_band_chan((ftm_mode_id_type)header->band,
                                 header->channel,
                                 header->dc_channel_offset,
                                 drx_enable_flag
                                 );

  /* Set Lowest PA State  */
  ftm_tdscdma_set_pa_range(rf_tdscdma_core_util_get_pa_range_from_nv(0));

  /* Get PDM limit and set PDM to lowest + 1 */
  ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_tdscdma_pdm_limits.min_pdm+1);

  /* Populate a "No Response" packet */
  ftm_rsp_data.cmd = FTM_RSP_NO_RSP;
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_3(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_configure_radio, start_tick = %d, stop_tick = %d, elapse = %d us",
              ftm_tdscdma_prof_start_time,
              timetick_get(),
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC ));
  }

  return ftm_rsp_data;

} /* ftm_tdscdma_calv3_configure_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_INT_CALV3_RADIO_SETUP
  Perform Radio Setup to given state

  @details
  This function will be called from cal v3 common framework to perform
  radio setup operation.All radio setup operation will be done under FTM
  Task context by sending radio setup command to FTM Task Q.

  @param radio_setup
  Pointer to the data structure containing radio setup unpacked parameter

  @return
  Flag indicating the status of radio setup operation
*/
boolean ftm_tdscdma_calv3_sweep_radio_setup
(
  ftm_cal_radio_setup_parameter_type const *radio_setup
)
{
  /* Config radio request */
  ftm_cal_radio_setup_req_type rs_req;

  /* ToDo: Get the dbg flag from common data structure */
  ftm_tdscdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Check for NULL ptr */
  if(radio_setup == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_radio_setup: NULL radio_setup ptr!");
    return FALSE;
  } /* if ( radio_setup == NULL ) */

  /* Create Request Packet for radio setup */
  rs_req.cmd = DIAG_SUBSYS_CMD_F;
  rs_req.sub_sys = FTM;
  rs_req.mode = FTM_TDSCDMA_C;
  rs_req.cdma_cmd = FTM_INT_CALV3_RADIO_SETUP;

  memscpy( &(rs_req.rs_param),
           sizeof(ftm_cal_radio_setup_parameter_type),
		   radio_setup,
		   sizeof(ftm_cal_radio_setup_parameter_type) );

  /* Queue up the FTM command */
  ftm_common_send_radio_command( &rs_req,
                                 sizeof(ftm_cal_radio_setup_req_type) );

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_2(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_radio_setup: mode = %d, elapse time: %d",
              rs_req.mode,
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  return TRUE;
} /* ftm_tdscdma_calv3_sweep_radio_setup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Radio Setup to given state

  @details
  This function will be called from cal v3 common framework to perform
  radio setup operation. Radio setup involves operation such as first
  radio setung, channel retuning, feedback setup, and tx waveform
  configuration. Thus all these cases should be handled by this function.

  @param rs_req
  Pointer to the data structure containing radio setup request data

  @return
  ftm_rsp_pkt_type
*/
ftm_rsp_pkt_type ftm_tdscdma_calv3_radio_setup(void *ftm_req_data)
{
  rfm_device_enum_type mdev; /* Master device */
  rfm_device_enum_type sdev; /* Slave device */
  ftm_cal_radio_setup_mdevice_state_type mdev_state; /* Master device State */
  ftm_cal_radio_setup_sdevice_state_type sdev_state; /* Slave device State */
  rfcom_tdscdma_band_type band = RFCOM_BAND_TDSCDMA_INVALID; /* TDSCDMA Band */
  uint8 num_of_chan = 0;
  int16 tune_channel = 0;
  boolean ret_val = TRUE; /* Status of radio setup */
  ftm_cal_radio_setup_parameter_type const *radio_setup = NULL; /* RS param */
  /* Populate a "No Response" packet */
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */

  /* Parameter validation */
  if ( ftm_req_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_radio_setup: NULL argument" );
    return ftm_rsp_data;
  } /* if ( ftm_req_data == NULL ) */

  /* Get the pointer to radio setup param */
  radio_setup = &(((ftm_cal_radio_setup_req_type*)ftm_req_data)->rs_param);

  /* Extract Radio Setup parameter */
  mdev = radio_setup->mdevice;
  sdev = radio_setup->sdevice;
  mdev_state = radio_setup->mdevice_state;
  sdev_state = radio_setup->sdevice_state;
  num_of_chan = radio_setup->num_of_chan;

  /* Add support for DR-DSDS RFcal */
  ftm_tdscdma_current_pri_device = mdev;
  ftm_tdscdma_current_sec_device = sdev;

  if(mdev_state == MASTER_DEVICE_RXTX)
  {
    ftm_tdscdma_current_tx_device = mdev;
  }

	FTM_MSG_3(FTM_HIGH, "TDSCDMA updated pri_device %d, sec_device %d, tx_device %d", 
            ftm_tdscdma_current_pri_device,
            ftm_tdscdma_current_sec_device,
            ftm_tdscdma_current_tx_device);
   
  band = ftm_tdscdma_calv4_convert_band(radio_setup->band);

  if(num_of_chan==1)
  {
    tune_channel = radio_setup->chan_list[0].channel;
  }
  else
  {
  	FTM_MSG(FTM_ERROR, "TDSCDMA can't have dual-carrier");
  }

  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_8( FTM_HIGH, "ftm_tdscdma_calv3_radio_setup: [Status %d] "
               "Dev[%d,%d] State[%d,%d] Num[%d] Band [%d] Channel[%d %d]",
               ret_val, mdev, sdev, mdev_state, sdev_state, num_of_chan, band,
               tune_channel);
  }

  /*----------------------------------------------------------------------*/
  /* MASTER_DEVICE_DISABLED */
  /*----------------------------------------------------------------------*/

  if ( mdev_state == MASTER_DEVICE_DISABLED )
  {
    /* Tear Down RF */
    if( mdev < RFM_MAX_DEVICES  )
    {
      ftm_rfmode_exit(mdev, 0);
      /* Tear down the data module when master device disabled */
      ftm_cal_data_teardown_data_module();
      /* Tear down the delay map when master device disabled */
      ftm_cal_data_teardown_delay_map();
      /* Tear down the pin map when master device disabled */
      ftm_cal_data_teardown_pin_map();
    }
    else
    {
     FTM_MSG(FTM_ERROR, "Wrong device index");
    }

  } /* if ( radio_setup->mdevice_state == MASTER_DEVICE_DISABLED ) */
  
  /* Clear the DPD map, delay map and pin map when starting calibration */
  if ( mdev_state == MASTER_DEVICE_RXTX  )
  {
    ftm_cal_data_teardown_data_module();
    ftm_cal_data_teardown_delay_map();
    ftm_cal_data_teardown_pin_map();
  }

  if ( ( mdev_state == MASTER_DEVICE_RX ) ||
       ( mdev_state == MASTER_DEVICE_RXTX ) )
  {
    if (   (ftm_current_rf_mode != FTM_DB_RF_TDSCDMA_B34)
        && (ftm_current_rf_mode != FTM_DB_RF_TDSCDMA_B39)
        && (ftm_current_rf_mode != FTM_DB_RF_TDSCDMA_B40))
    {

      if( mdev < RFM_MAX_DEVICES  )
      {
        ftm_tdscdma_calv3_set_current_mode(band);

      /* Enter FTM to TDSCDMA Mode */
      ftm_rfmode_enter( mdev, FTM_STATE_TDSCDMA );
      /* Enter TDSCDMA Mode */
      rfm_enter_mode( mdev, RFCOM_TDSCDMA_MODE,
                        NULL, NULL, 0);
      }
      else
      {
        FTM_MSG(FTM_ERROR, "Wrong device index");
      }
    }
      else
      {
        ftm_tdscdma_calv3_set_current_mode(band);
      }

    /*----------------------------------------------------------------------*/
    /* SLAVE_DEVICE_RXDIV */
    /*----------------------------------------------------------------------*/
    if( sdev_state == SLAVE_DEVICE_RXDIV )
    {
      if(ftm_tdscdma_div_status == FALSE)
      {
        ftm_tdscdma_set_secondary_chain(TRUE);
      }
    }
    /*----------------------------------------------------------------------*/
    /* SLAVE_DEVICE_RXDIV */
    /*----------------------------------------------------------------------*/
    if( sdev_state == SLAVE_DEVICE_DISABLED )
    {
      if(ftm_tdscdma_div_status == TRUE)
      {
        ftm_tdscdma_set_secondary_chain(FALSE);
      }
    }


    if(num_of_chan == 1)
    {
      /* Rx PLL */
      if( mdev < (rfm_device_enum_type)FTM_RECEIVE_CHAIN_INVALID )
      {
		ftm_tdscdma_tune_to_chan((ftm_receive_chain_handle_type)mdev, tune_channel);
      }
      else
      {
         FTM_MSG(FTM_ERROR, "Wrong device index");
      }
    }

    if(mdev_state == MASTER_DEVICE_RX)
    {
      // Check if TX on then turn off TX

      if(ftm_tdscdma_tx_on_status == TRUE)
      {
        ftm_tdscdma_set_tx(FALSE);
      }
    }
    else
    {
    	rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0, RFCOM_TDSCDMA_MODE, ftm_tdscdma_current_mode, RFDEVICE_GET_TX_AGC_LIMITS, &ftm_tdscdma_calv3_pdm_limits);
      ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_tdscdma_calv3_pdm_limits.min_pdm+1);

    	ftm_tdscdma_set_rx(FALSE);
    	ftm_tdscdma_set_tx(TRUE);
    }
  } /* if ( ( mdev_state == MASTER_DEVICE_RX ) ||
          ( mdev_state == MASTER_DEVICE_RXTX ) ) */

  /*----------------------------------------------------------------------*/
  /* MASTER_DEVICE_RETUNE */
  /*----------------------------------------------------------------------*/
  if ( mdev_state == MASTER_DEVICE_RETUNE )
  {
    /* Reset PDM to lowest - Need to do this first before tune to chan since it can cause
       callbox segment overrun
    */
    ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_tdscdma_pdm_limits.min_pdm+1);

    /* Skip enter_mode if already done so */
	if ( (ftm_current_rf_mode != FTM_DB_RF_TDSCDMA_B34)
        && (ftm_current_rf_mode != FTM_DB_RF_TDSCDMA_B39)
        && (ftm_current_rf_mode != FTM_DB_RF_TDSCDMA_B40))
    {
      ftm_tdscdma_calv3_set_current_mode(band); 

      /* Enter FTM to TDSCDMA Mode */
      if( mdev < RFM_MAX_DEVICES )
      {
      ftm_rfmode_enter( mdev, FTM_STATE_TDSCDMA );
      /* Enter TDSCDMA Mode */
      rfm_enter_mode( mdev, RFCOM_TDSCDMA_MODE,
                      NULL, NULL, 0);
      }
      else
      {
         FTM_MSG(FTM_ERROR, "Wrong device index");
      }
    }
    else
    {
      ftm_tdscdma_calv3_set_current_mode(band); // Why is this?
    }
    /*----------------------------------------------------------------------*/
    /* SLAVE_DEVICE_RXDIV */
    /*----------------------------------------------------------------------*/
    if( sdev_state == SLAVE_DEVICE_RXDIV )
    {
      if(ftm_tdscdma_div_status == FALSE)
      {
        ftm_tdscdma_set_secondary_chain(TRUE);
      }
    }
    /*----------------------------------------------------------------------*/
    /* SLAVE_DEVICE_RXDIV */
    /*----------------------------------------------------------------------*/
    if( sdev_state == SLAVE_DEVICE_DISABLED )
    {
      if(ftm_tdscdma_div_status == TRUE)
      {
        ftm_tdscdma_set_secondary_chain(FALSE);
      }
    }

    if(num_of_chan == 1)
    {
      /* Rx PLL */
      if( mdev < (rfm_device_enum_type)FTM_RECEIVE_CHAIN_INVALID )
      {
       	ftm_tdscdma_tune_to_chan( mdev, tune_channel);
				ftm_tdscdma_set_tx(TRUE);
      }
      else
      {
      FTM_MSG(FTM_ERROR, "Wrong device index");
      }
    }
  } /* if ( mdev_state == MASTER_DEVICE_RETUNE ) */

  /*----------------------------------------------------------------------*/
  /* SLAVE_DEVICE_RXDIV */
  /*----------------------------------------------------------------------*/
  if( sdev_state == SLAVE_DEVICE_RXDIV )
  {
    if(ftm_tdscdma_div_status == FALSE)
      {
      ftm_tdscdma_set_secondary_chain(TRUE);
      }
  }

  /*----------------------------------------------------------------------*/
  /* SLAVE_DEVICE_RXDIV */
  /*----------------------------------------------------------------------*/
  if( sdev_state == SLAVE_DEVICE_DISABLED )
  {
    if(ftm_tdscdma_div_status == TRUE)
    {
      ftm_tdscdma_set_secondary_chain(FALSE);
    }
  }

  switch(radio_setup->wfm_type)
  {
     case TX_WFM_DEFAULT:
       ftm_tdscdma_switch_tx_waveform((byte)0);
       break;

     case TX_WFM_DPD:
       ftm_tdscdma_switch_tx_waveform((byte)1);
       break;

     default:
       FTM_MSG(FTM_ERROR, "TDSCDMA Does not support the waveform type");
  }

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_8( FTM_HIGH, "ftm_tdscdma_calv3_radio_setup: [Status %d] "
               "Dev[%d,%d] State[%d,%d] Num[%d] Band [%d] Channel[%d %d]",
               ret_val, mdev, sdev, mdev_state, sdev_state, num_of_chan, band,
               tune_channel);
  }

  /* Return Status */
  return ftm_rsp_data;

} /* ftm_tdscdma_calv3_radio_setup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Apply Pin alignment during xPT cal when requested

  @details
  This function applies Pin alignment to the RGIs that must be used during
  RF calibration. Based on the action flag to do Pin alignment, this function
  takes as input the override values that would be used for the current RF
  calibration measurement. These override values are adjusted based on static
  NV Pin compensation values to give new override values.

  The new override values are returned to the Tx override functionality,
  which includes RGI and IQ gain adjustment

  @param device
  Current device for which calibration is running

  @param ref_chan
  Reference linearizer channel with respect to which Pin alignment must be
  computed

  @param input_override_row
  Pointer to row containing input override values

  @param output_override_row
  Pointer to row containing Pin aligned override values

  @return
  Flag indicating the status of Pin alignment calculation result
*/
static
boolean
ftm_tdscdma_calv3_apply_pin_alignment
(
  rfm_device_enum_type device,
  uint32 ref_chan,
  ftm_cal_data_dpd_map_row_type *input_override_row,
  ftm_cal_data_dpd_map_row_type *output_override_row
)
{
  /* Pointer to NV data */
  rfnv_tdscdma_tx_tbl_type *nv_tbl = NULL;

//  uint16 curr_chan_tx;

  /* Pin compensation */
  uint16 ref_lin_therm_adc = 0;
  int16 pin_comp_offset = 0;
  int16 pout_comp_offset = 0;
  tx_lin_vs_temp_vs_freq_comp_param_type temp_vs_freq_params = {0};

  /* Obtain NV information */
  nv_tbl = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);

  /* Get the reference linearizer Therm ADC value from data module */
  ftm_cal_data_get_therm_adc( ref_chan, &ref_lin_therm_adc );

  /* Initialize input data for the function to compute offset */

  temp_vs_freq_params.freq_params.tx_freq =
    rf_tdscdma_core_util_get_freq_from_uarfcn(ftm_tdscdma_current_chan, ftm_tdscdma_current_mode);
  temp_vs_freq_params.freq_params.ref_lin_tx_freq =
    rf_tdscdma_core_util_get_freq_from_uarfcn(ref_chan, ftm_tdscdma_current_mode);

  temp_vs_freq_params.temp_params.temp_read =
    rf_tdscdma_core_temp_comp_scale_therm( ref_lin_therm_adc, FALSE );
  temp_vs_freq_params.temp_params.ref_lin_temp_adc =
    rf_tdscdma_core_temp_comp_scale_therm( ref_lin_therm_adc, FALSE );

  temp_vs_freq_params.pa_state = input_override_row->curr_tx_override.pa_state;
  temp_vs_freq_params.ref_xpt_pin_offset = 0;

  if( input_override_row->curr_tx_override.xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET )
  {
   temp_vs_freq_params.ref_xpt_pin_offset = 
     (int32)rf_tdscdma_core_xpt_get_cfg_param( ftm_tdscdma_current_mode, XPT_CFG_CMN_REF_ET_PIN_OFFSET );
  }
  else if( input_override_row->curr_tx_override.xpt_mode == RFCOMMON_MDSP_XPT_MODE_EPT )
  {
   temp_vs_freq_params.ref_xpt_pin_offset = 
     (int32)rf_tdscdma_core_xpt_get_cfg_param( ftm_tdscdma_current_mode, XPT_CFG_CMN_REF_EPT_PIN_OFFSET );
  }
  /* Calculate Pin alignment */

  if(nv_tbl != NULL)
  {
    temp_vs_freq_params.auto_pin_enable = rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE);
    rfcommon_core_txlin_get_temp_compensation(
           RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3,
           &pin_comp_offset,
           &pout_comp_offset,
           &temp_vs_freq_params,
           &nv_tbl->lin_temp_comp_data );

  }
  else
  {
    FTM_MSG(FTM_HIGH, "ftm_tdscdma_calv3_apply_pin_alignment: No NV table returned");
  }

  /* Apply Pin alignment to the override row */
  ftm_cal_data_apply_pin_alignment( ref_chan,
                                    input_override_row->curr_tx_override.pa_state,
                                    input_override_row,
                                    output_override_row,
                                    pin_comp_offset );

  FTM_MSG_4( FTM_HIGH, "ftm_tdscdma_calv3_apply_pin_alignment: "
                       "Ref Chan %d, Ref ADC %d, Curr Chan %d, PA State %d",
             ref_chan, ref_lin_therm_adc,
             ftm_tdscdma_current_chan,
             input_override_row->curr_tx_override.pa_state );
  FTM_MSG_3( FTM_HIGH, "ftm_tdscdma_calv3_apply_pin_alignment: "
                       "Pin Comp %d dB10, Input RGI %d, Input IQ Gain %d",
             pin_comp_offset, input_override_row->curr_tx_override.rgi,
             input_override_row->curr_tx_override.iq_gain );
  FTM_MSG_2( FTM_HIGH, "ftm_tdscdma_calv3_apply_pin_alignment: "
                       "Output RGI %d, Output IQ Gain %d",
             output_override_row->curr_tx_override.rgi,
             output_override_row->curr_tx_override.iq_gain );

  return TRUE;
} /* ftm_tdscdma_calv3_apply_pin_alignment */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Tx Override

  @details
  This function will be called from cal v3 common framework to perform
  Tx Override operation. Tx Override involves operation such as overriding
  several RF and MSM Tx parameters.

  @param tx_override
  Pointer to the data structure containing Tx Override unpacked parameter

  @return
  Flag indicating the status of Tx Override operation
*/
boolean
ftm_tdscdma_calv3_sweep_tx_override
(
  ftm_cal_tx_override_parameter_type const *tx_override
)
{

  uint8 dev_mask = 0xFF; /*  */
  uint8 dev_temp = 0xFF; /* Variable to store the Device. This will be modified
  in the while loop. */
  rfm_device_enum_type device; /* RFM Device */
  ftm_xpt_txagc_override_type ovr_params; /* Tx Override Params */
  rfcommon_core_xpt_tx_override_type curr_ovr_params;
  boolean ret_val = TRUE;
  ftm_cal_tx_override_type_mask_type override_type = 0;
  ftm_cal_tx_ovr_power_ovr_type power_ovr_type = 0;
  ftm_cal_tx_ovr_power_param_type param_mask = 0;
  ftm_cal_data_dpd_map_row_type override_row = {0};
  ftm_cal_data_dpd_map_row_type override_row_before_pin;
  rfcom_band_type_u rfm_band;
  //ftm_cdma_data_status_type *dev_ptr;

  memset( &curr_ovr_params, 0, sizeof(rfcommon_core_xpt_tx_override_type) );

  /* ToDo: Get the dbg flag from common data structure */
  ftm_tdscdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Parameter validation */
  if ( tx_override == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_sweep_tx_override: NULL argument" );
    return FALSE;
  } /* if ( tx_override == NULL ) */

  /* Temporarily store the Device mask */
  dev_temp = tx_override->device_mask;

  /* For the current unprocessed Opcode, extract the bit mask for the first
  enabled cal action. The first bit will be from LSB. */
  dev_mask = rf_extract_first_one_bit_mask( dev_temp );

  /* Loop until, the OpCode Mask comes out to be zero */
  while ( dev_mask != 0 )
  {

    /* Find the bit location and this location will correspond to
    RFM device */
    device = (rfm_device_enum_type)(
                        rf_extract_first_one_bit_lsb_location(dev_mask) );

    /* Populate Device */
    ovr_params.device = device;

    if( device >= FTM_CAL_V3_TX_MAX_DEV)
    {
     FTM_MSG( FTM_ERROR, "Device index out of range" );
     return FALSE;
    }

    /* Populate Device */
    ovr_params.device = device;
    ovr_params.xpt_mode = tx_override->tx_param[device].xpt_mode;
    curr_ovr_params.xpt_mode = tx_override->tx_param[device].xpt_mode;
    override_type = tx_override->tx_param[device].override_type;

    if ( ( override_type & ( 1 << TX_OVR_POWER ) ) != 0 )
    {
      power_ovr_type = tx_override->tx_param[device].power_ovr.power_ovr_type;
      if ( power_ovr_type == TX_OVR_POWER_IND_PARAM )
      {
        param_mask =
          tx_override->tx_param[device].power_ovr.individual_param.param_mask;

        /* Populate IQ Gain if the entry is valid */
        if ( ( param_mask & ( 1 << TX_OVR_IQ_GAIN ) ) != 0 )
        {
          ovr_params.iq_gain_action_type =
             tx_override->tx_param[device].power_ovr.individual_param.iq_gain.ovr_action;
          ovr_params.iq_gain =
            tx_override->tx_param[device].power_ovr.individual_param.iq_gain.iq_gain;
          curr_ovr_params.iq_gain_action_type = 
               tx_override->tx_param[device].power_ovr.individual_param.iq_gain.ovr_action;
          curr_ovr_params.iq_gain = 
              tx_override->tx_param[device].power_ovr.individual_param.iq_gain.iq_gain;
        } /* if ( ( param_mask & ( 1 << TX_OVR_IQ_GAIN ) ) != 0 ) */
        else
        {
          ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
          ovr_params.iq_gain = 0;
          curr_ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
          curr_ovr_params.iq_gain = 0;
        } /* if ! ( ( param_mask & ( 1 << TX_OVR_IQ_GAIN ) ) != 0 ) */

        /* Populate Envelope Scale if the entry is valid */
        if ( ( param_mask & ( 1 << TX_OVR_ENV_SCALE ) ) != 0 )
        {
          ovr_params.env_scale_action_type =
            tx_override->tx_param[device].power_ovr.individual_param.env_scale.ovr_action;
          ovr_params.env_scale =
            (uint16)tx_override->tx_param[device].power_ovr.individual_param.env_scale.env_scale;
          curr_ovr_params.env_scale_action_type = 
              tx_override->tx_param[device].power_ovr.individual_param.env_scale.ovr_action;
          curr_ovr_params.env_scale = 
              (uint16)tx_override->tx_param[device].power_ovr.individual_param.env_scale.env_scale;
        } /* if ( ( param_mask & ( 1 << TX_OVR_ENV_SCALE ) ) != 0 ) */
        else
        {
          ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
          ovr_params.env_scale = 0;
          curr_ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
          curr_ovr_params.env_scale = 0;
        } /* if ! ( ( param_mask & ( 1 << TX_OVR_ENV_SCALE ) ) != 0 ) */

        /* Populate PA Bias if the entry is valid */
        if ( ( param_mask & ( 1 << TX_OVR_RGI ) ) != 0 )
        {
          ovr_params.rgi =
            tx_override->tx_param[device].power_ovr.individual_param.rgi.gain_index;
          curr_ovr_params.rgi = 
            tx_override->tx_param[device].power_ovr.individual_param.rgi.gain_index;
        } /* if ( ( param_mask & ( 1 << TX_OVR_RGI ) ) != 0 ) */
        else
        {
          ovr_params.rgi = -1;
          curr_ovr_params.rgi = FTM_COMMON_XPT_TX_OVR_RGI_INVALID;
        } /* if ! ( ( param_mask & ( 1 << TX_OVR_RGI ) ) != 0 ) */

        /* Populate XPT if the entry is valid */
        if ( ( param_mask & ( 1 << TX_OVR_PA_BIAS ) ) != 0 )
        {
          ovr_params.smps_bias =
            tx_override->tx_param[device].power_ovr.individual_param.bias.pa_bias;
          curr_ovr_params.papm_bias = 
            tx_override->tx_param[device].power_ovr.individual_param.bias.pa_bias;
        } /* if ( ( param_mask & ( 1 << TX_OVR_PA_BIAS ) ) != 0 ) */
        else
        {
          ovr_params.smps_bias = -1;
          curr_ovr_params.papm_bias = FTM_COMMON_XPT_TX_OVR_SMPS_BIAS_INVALID;
        } /* if ! ( ( param_mask & ( 1 << TX_OVR_PA_BIAS ) ) != 0 ) */

        /* Populate PA State if the entry is valid */
        if ( ( param_mask & ( 1 << TX_OVR_PA_STATE ) ) != 0 )
        {
          ovr_params.pa_state =
            tx_override->tx_param[device].power_ovr.individual_param.pa_state.gain_state;
          curr_ovr_params.pa_state = 
            tx_override->tx_param[device].power_ovr.individual_param.pa_state.gain_state;
        } /* if ( ( param_mask & ( 1 << TX_OVR_PA_STATE ) ) != 0 ) */
        else
        {
          ovr_params.pa_state = -1;
          ovr_params.pa_state = FTM_COMMON_XPT_TX_OVR_PA_STATE_INVALID;
        } /* if ! ( ( param_mask & ( 1 << TX_OVR_PA_STATE ) ) != 0 ) */

        /* Populate PA Current if the entry is valid */
        if ( ( param_mask & ( 1 << TX_OVR_QUISCENT_CURRENT ) ) != 0 )
        {
          ovr_params.pa_curr =
            tx_override->tx_param[device].power_ovr.individual_param.current.quiscent_current;
          curr_ovr_params.pa_curr = 
            tx_override->tx_param[device].power_ovr.individual_param.current.quiscent_current;
        } /* if ( ( param_mask & ( 1 << TX_OVR_QUISCENT_CURRENT ) ) != 0 ) */
        else
        {
          ovr_params.pa_curr = -1;
          curr_ovr_params.pa_curr = FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID;
        } /* if !( ( param_mask & ( 1 << TX_OVR_QUISCENT_CURRENT ) ) != 0 ) */


        /* Skip delay value programming for now */
        #if 0
        if ( ( param_mask & ( 1 << TX_OVR_DELAY ) ) != 0 )
        {
          ovr_params.delay_action_type = tx_override->tx_param[device].power_ovr.individual_param.delay.ovr_action;
          ovr_params.delay = tx_override->tx_param[device].power_ovr.individual_param.delay.delay_val;
          curr_ovr_params.delay_action_type = 
            tx_override->tx_param[device].power_ovr.
                                     individual_param.delay.ovr_action;
          curr_ovr_params.et_delay = 
            tx_override->tx_param[device].power_ovr.
                                     individual_param.delay.delay_val;
        }
        else
        #endif
        {
        ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_NV;
        /* Initialize to zero since we are skipping for now */
        ovr_params.delay = 0;
          curr_ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_NV;
          curr_ovr_params.et_delay = 0;
        }

        /* Else override by current settings */
        curr_ovr_params.amam_addr.mem_bank_id = -1;
        curr_ovr_params.amam_addr.start_offset = -1;
        curr_ovr_params.amam_addr.tbl_size = 0;
        curr_ovr_params.ampm_addr.mem_bank_id = -1;
        curr_ovr_params.ampm_addr.start_offset = -1;
        curr_ovr_params.ampm_addr.tbl_size = 0;
        curr_ovr_params.detrough_addr.mem_bank_id = -1;
        curr_ovr_params.detrough_addr.start_offset = -1;
        curr_ovr_params.detrough_addr.tbl_size = 0;

        memscpy( (void*)&override_row.curr_tx_override,
                 sizeof(rfcommon_core_xpt_tx_override_type),
                 (void*)&curr_ovr_params,
                 sizeof(rfcommon_core_xpt_tx_override_type) );

        /* Set the current override */
        /* Set the current row as invalid so when it gets pushed by
           IQ capture it will go into the next available row */
        ftm_cal_data_set_current_override_row(FTM_COMMON_XPT_INV_OVR_ROW);
      }
      else if ( power_ovr_type == TX_OVR_POWER_ABS )
      {
        ret_val &= ftm_cal_data_get_row_data_by_power(
           tx_override->tx_param[device].power_ovr.abs_power.ref_channel,
                                                     &override_row_before_pin,
                                                     tx_override->tx_param[device].power_ovr.abs_power.power_dbm10,
                                                     curr_ovr_params.xpt_mode);

        FTM_MSG_1( FTM_HIGH, "Absolite Tx Power override power_dbm10 %d",
                   tx_override->tx_param[device].power_ovr.abs_power.power_dbm10 );

        /* Copy over the data module values to the final override row */
        memscpy( (void*)&override_row,
			     sizeof(ftm_cal_data_dpd_map_row_type),
                 (void*)&override_row_before_pin,
                 sizeof(ftm_cal_data_dpd_map_row_type) );

        FTM_MSG_2( FTM_HIGH,"Ovr by power row delay action %d, delay val %d", 
                       override_row.curr_tx_override.delay_action_type,
                       override_row.curr_tx_override.et_delay );

        /* If the Pin alignment flag is set, then apply Pin
        alignment to the override values before proceeding. If Pin is
        not required, the values obtained from the data module will
        be used */
        if ( tx_override->flag ==
                              FTM_CALIBRATION_TX_OVR_APPLY_PIN_ALIGNMENT )
        {
          ftm_tdscdma_calv3_apply_pin_alignment(device,
                                                tx_override->
                                                tx_param[device].power_ovr.abs_power.ref_channel,
                                                &override_row_before_pin,
                                                &override_row );

          if ( tx_override->debug_mode_en == TRUE )
          {
            FTM_MSG_1( FTM_MED, "ftm_tdscdma_calv3_sweep_tx_override: "
                                "Applied Pin alignment to Tx override for "
                                "Power %d",
                       tx_override->
                       tx_param[device].power_ovr.abs_power.power_dbm10 );
          }
        }/* if( tx_override->flag ==
                              FTM_CALIBRATION_TX_OVR_APPLY_PIN_ALIGNMENT */


        /* Verify we found a row of data */
        if(ret_val == FALSE)
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,
             "ftm_tdscdma_calv3_sweep_tx_override: Could not find power %d in data module",
             tx_override->tx_param[device].power_ovr.abs_power.power_dbm10);
          return FALSE;
        }

        /* Set the current override */
        /* Set with the row retrieved from get_row_by_power so that
           when it's pushed by IQ capture it will use this row to overwrite */
        ftm_cal_data_set_current_override_row(override_row.row_idx);
      }
      else
      {
        FTM_MSG_1( FTM_ERROR, "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_tx_override: "
                   "Invalid Power Override Type %d", power_ovr_type );
      }
    } /* if ( ( override_type & ( 1 << TX_OVR_POWER ) ) != 0 ) */

    if ( ( override_type & ( 1 << TX_OVR_APPLY_DPD ) ) != 0 )
    {
      FTM_MSG( FTM_ERROR, "Apply DPD to be implemented" );
    } /* if ( ( override_type & ( 1 << TX_OVR_APPLY_DPD ) ) != 0 ) */

    /* Override TxAGC */
    if ( rfcommon_debug_flags.use_rflm_txagc_override )
    {
      /* Process override params */
      ret_val &= ftm_tdscdma_xpt_process_override_txagc( device,
                   override_row.curr_tx_override.xpt_mode,
                   override_row.curr_tx_override.amam_addr.mem_bank_id,
                   override_row.curr_tx_override.amam_addr.start_offset,
                   override_row.curr_tx_override.amam_addr.tbl_size,
                   override_row.curr_tx_override.ampm_addr.mem_bank_id,
                   override_row.curr_tx_override.ampm_addr.start_offset,
                   override_row.curr_tx_override.ampm_addr.tbl_size,
                   override_row.curr_tx_override.detrough_addr.mem_bank_id,
                   override_row.curr_tx_override.detrough_addr.start_offset,
                   override_row.curr_tx_override.detrough_addr.tbl_size,
                   override_row.curr_tx_override.iq_gain_action_type,
                   override_row.curr_tx_override.iq_gain,
                   override_row.curr_tx_override.env_scale_action_type,
                   override_row.curr_tx_override.env_scale,
                   override_row.curr_tx_override.rgi,
                   override_row.curr_tx_override.papm_bias,
                   override_row.curr_tx_override.pa_state,
                   override_row.curr_tx_override.pa_curr,
                   override_row.curr_tx_override.delay_action_type,
                   override_row.curr_tx_override.et_delay );

      /* Apply the Tx override through RFLM */
      rfm_band.tdscdma_band = ftm_curr_tdscdma_mode;
      ret_val &= rfm_ftm_apply_xpt_tx_agc_override( RFM_TDSCDMA_MODE, device,
                                                    rfm_band );
    }
    else
    {
    ret_val &= ftm_tdscdma_xpt_override_txagc(device,
                   override_row.curr_tx_override.xpt_mode,
                   override_row.curr_tx_override.amam_addr.mem_bank_id,
                   override_row.curr_tx_override.amam_addr.start_offset,
                   override_row.curr_tx_override.amam_addr.tbl_size,
                   override_row.curr_tx_override.ampm_addr.mem_bank_id,
                   override_row.curr_tx_override.ampm_addr.start_offset,
                   override_row.curr_tx_override.ampm_addr.tbl_size,
                   override_row.curr_tx_override.detrough_addr.mem_bank_id,
                   override_row.curr_tx_override.detrough_addr.start_offset,
                   override_row.curr_tx_override.detrough_addr.tbl_size,
                   override_row.curr_tx_override.iq_gain_action_type,
                   override_row.curr_tx_override.iq_gain,
                   override_row.curr_tx_override.env_scale_action_type,
                   override_row.curr_tx_override.env_scale,
                   override_row.curr_tx_override.rgi,
                   override_row.curr_tx_override.papm_bias,
                   override_row.curr_tx_override.pa_state,
                   override_row.curr_tx_override.pa_curr,
                   override_row.curr_tx_override.delay_action_type,
                   override_row.curr_tx_override.et_delay );
    }

    /* Update OpCode to set the processed bit to zero. */
    dev_temp = dev_temp & (~dev_mask);

    /* For the current unprocessed Opcode, extract the bit mask for the first
    enabled cal action. The first bit will be from LSB. */
    dev_mask = rf_extract_first_one_bit_mask( dev_temp );

    /* Print Extra Debug message */
    if ( tx_override->debug_mode_en == TRUE )
    {
      FTM_MSG_5( FTM_HIGH, "ftm_tdscdma_calv3_sweep_tx_override: Dev %d "
                 "Tx Param 0x%x, PA %d, RGI %d, Bias %d", 
                 device, 
                 param_mask,
                 curr_ovr_params.pa_state,
                 curr_ovr_params.rgi,
                 curr_ovr_params.papm_bias );
    }

  } /* while ( dev_mask != 0 ) */

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_3( FTM_HIGH, "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_tx_override: "
               "[Status %d] [DevMask %d] [Elapse time: %d]",
               ret_val, tx_override->device_mask, timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC ) );
  }

  return ret_val;

} /* ftm_tdscdma_calv3_sweep_tx_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Tx Measurement

  @details
  This function will be called from cal v3 common framework to perform
  Tx measurement operation. Tx measurement involves operation of sampling HDET
  reads

  @param tx_measure
  Pointer to the data structure containing Tx meas unpacked parameter

  @param cal_result
  Pointer to the data structure to which the results is supposed
  to be populated

  @return
  Flag indicating the status of Tx Measurement operation
*/
boolean
ftm_tdscdma_calv3_tx_measure
(
  ftm_cal_tx_meas_parameter_type const *tx_measure,
  ftm_cal_tx_meas_result_data_type *cal_result
)
{

  uint8 dev_mask = 0xFF; /*  */
  uint8 dev_temp = 0xFF; /* Variable to store the Device. This will be modified
  in the while loop. */
  rfm_device_enum_type device; /* RFM Device */
  boolean ret_val = TRUE;
  ftm_cal_tx_meas_hdet_sample_type hdet_sample = 0;
  uint8 fb_power_samples = 0;
  uint32  hdet_accm = 0;        /* HDET read accumuulator */
  uint16  temp_hdet_val  = 0;   /* temp HDET read         */
  uint8   hdet_idx = 0;         /* read index             */
  uint8   fb_power_samples_idx = 0;         /* read index             */
  uint8   local_read_count;     /* read counter           */
  uint16  therm_read = 0;       /* Therm read             */
  ftm_common_fbrx_capture_input_type fb_meas_inp;
  ftm_common_fbrx_capture_result_type fb_meas_res;
  uint8 log_ind = 0;
  rfcom_tdscdma_band_type cur_band;

  const uint8 hdet_max_num_retries = 1; //accoding to TDSweepTx

  /* Parameter validation */
  if ( ( tx_measure == NULL ) || ( cal_result == NULL ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_tx_measure: NULL argument" );
    return FALSE;
  } /* if ( ( rx_measure == NULL ) || ( cal_result == NULL ) ) */

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Temporarily store the Device mask */
  dev_temp = tx_measure->device_mask;

  /* For the current unprocessed Opcode, extract the bit mask for the first
  enabled cal action. The first bit will be from LSB. */
  dev_mask = (uint8)rf_extract_first_one_bit_mask( dev_temp );

  /* Loop until, the OpCode Mask comes out to be zero */
  while ( dev_mask != 0 )
  {

    /* Find the bit location and this location will correspond to
    RFM device */
    device = (rfm_device_enum_type)(
                        rf_extract_first_one_bit_lsb_location(dev_mask) );

    /* Check if device of interest is valid */
    if(device != RFM_DEVICE_0)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_tdscdma_calv3_tx_measure: Invalid Device: %d!",device);
    }
    else
    {

      /*----------------------------------------------------------------------*/
      /*  TX_MEAS_HDET */
      /*----------------------------------------------------------------------*/

      /* Perform LNA Offset measurment */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action &
           ( 1 << TX_MEAS_HDET ) ) != 0 )
      {
        hdet_sample = tx_measure->tx_meas_param[device].hdet.hdet_sample;

        /* Check for div-by-0 */
        if( hdet_sample == 0)
        {
          FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_tx_measure: hdet_sample input is 0");
          return FALSE;
        }

        cur_band = ftm_curr_tdscdma_mode;

        /* Get HDET measurements */
        for( hdet_idx = 0; hdet_idx < hdet_sample; hdet_idx++)
        {
          /* Do up to 3 tries per HDET read */
          local_read_count = 0;
          do
          {
			if(is_wtr_hdet)
          	{
				rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                         RFCOM_TDSCDMA_MODE,
                                         cur_band,
                                         RFDEVICE_GET_HDET_VALUE,
                                         &temp_hdet_val);
          	}
       	  	else
       	  	{
            	(void)rfdevice_hdet_tdscdma_tx_hdet_read(RFM_DEVICE_0,
                                         cur_band,
                                         FALSE,
                                         &temp_hdet_val,
                                         NULL,
                                         RFDEVICE_EXECUTE_IMMEDIATE
                                         );
      	  	}
            local_read_count++;
          }while ( ((temp_hdet_val <= 0) || temp_hdet_val == 0xFFFF)
                   && (local_read_count < hdet_max_num_retries));

          /* Add to accum */
            hdet_accm += temp_hdet_val;
          }

        /* Take avg */
        hdet_accm = hdet_accm / hdet_sample;

        /* Populate result */
        cal_result->tx_meas_param[device].hdet.hdet_read = (uint16)hdet_accm;

      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action &
           ( 1 << TX_MEAS_HDET ) ) != 0 ) */

       /*----------------------------------------------------------------------*/
      /*  TX_MEAS_LINEARIZER_THERM */
      /*----------------------------------------------------------------------*/

      /* Perform Therm ADC measurement */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action &
           ( 1 << TX_MEAS_LINEARIZER_THERM ) ) != 0 )
      {
        /* Trigger Therm Read in FTM */
        rfdevice_tdscdma_tx_cmd_dispatch(
              device,
              RFCOM_TDSCDMA_MODE,
              ftm_tdscdma_current_mode,
              RFDEVICE_GET_THERMISTER_VALUE,
              &therm_read );

        /* Populate results data structure */
        cal_result->tx_meas_param[device].lin_therm.therm_read = therm_read;

        /* Send the Therm value to the data module for storage with respect to
        channel */

        ftm_cal_data_set_therm_adc(
                 ftm_tdscdma_current_chan,
                 &cal_result->tx_meas_param[device].lin_therm.therm_read );

        if ( tx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_1( FTM_MED, "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_tx_measure: "
                               "Therm Read %d",
                     cal_result->tx_meas_param[device].lin_therm.therm_read );
        } /* if ( tx_measure->debug_mode_en == TRUE ) */

      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action &
           ( 1 << TX_MEAS_LINEARIZER_THERM ) ) != 0 ) */

      /*----------------------------------------------------------------------*/
      /*  TX_MEAS_FB_GAIN */
      /*----------------------------------------------------------------------*/

      /* Perform FB Gain measurement */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_FB_GAIN ) ) != 0 )
      {
        /* Fetch Input Params for FBRx capture */
        fb_meas_inp.tx_device_id = device;
        fb_meas_inp.xpt_mode = (rfcommon_mdsp_xpt_mode_enum_type)tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.xpt_mode;
        fb_meas_inp.gain_comp = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.gain_comp;
        fb_meas_inp.fbrx_gain_state = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.fb_gain_state;
        fb_meas_inp.ubias = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.ubias;
        fb_meas_inp.scale_rx = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.rx_scale;
        fb_meas_inp.store_capture_data = (boolean)tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.debug_mode_en;
        fb_meas_inp.tech = RFCOM_TDSCDMA_MODE;
        fb_meas_inp.band.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;

        /* Call common FB Gain Measurement function */
        /* The first FBRx capture is mostlty bad, it's discarded for now by capturing twice */
        fb_meas_inp.store_capture_data = FALSE;
        fb_meas_res = ftm_common_fbrx_capture_and_process(fb_meas_inp);
        fb_meas_inp.store_capture_data = (boolean)tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.debug_mode_en;
        fb_meas_res = ftm_common_fbrx_capture_and_process(fb_meas_inp);

        if ( tx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_8( FTM_MED, "TDSCDMA CalV3 DBG - FB Gain Cal Res: Status %d , Gain = %d,"
                              "LOFT = %d, RSB = %d, GI = %d, PI = %d, LS_ERR = %d, XCORR_LOG[0] = %d", 
                     fb_meas_res.status, 
                     fb_meas_res.gain, fb_meas_res.loft, fb_meas_res.rsb, 
                     fb_meas_res.gain_imbalance, fb_meas_res.phase_imbalance, 
                     fb_meas_res.ls_err, fb_meas_res.xcorr_log[0] );
        }

        /* Check if FB Gain Measurement was a success */
        if ( fb_meas_res.status == RFCOMMON_FBRX_SUCCESS )
        {
          /* Populate results data structure */ 
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_gain_state = fb_meas_inp.fbrx_gain_state;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fbrx_mode = ftm_common_fbrx_get_mode();
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_stat = fb_meas_res.status;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_gain_val = fb_meas_res.gain;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_loft = fb_meas_res.loft;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_rsb = fb_meas_res.rsb;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_gain_imb = fb_meas_res.gain_imbalance;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_phase_imb = fb_meas_res.phase_imbalance;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_ls_err = fb_meas_res.ls_err;
          for( log_ind = 0; log_ind < RFLM_FBRX_XCORR_LOG_SIZE; log_ind++ )
          {
            cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_xcorr_log[log_ind] = fb_meas_res.xcorr_log[log_ind];
          }
        }
        else
        {
          FTM_MSG_1( FTM_ERROR, "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_tx_measure: Dev %d "
                                "FB Gain Measurement Failed", device );
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_stat = 
            fb_meas_res.status;
        }

        if ( tx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_1( FTM_MED, "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_tx_measure: "
                               "FB Gain Measurement Status = %d", 
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_stat );
        } /* if ( tx_measure->debug_mode_en == TRUE ) */

      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
         ( 1 << TX_MEAS_FB_GAIN ) ) != 0 ) */


      /*----------------------------------------------------------------------*/
      /*  TX_MEAS_FB_PWR */
      /*----------------------------------------------------------------------*/

      /* Perform LNA Offset measurment */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_FB_PWR ) ) != 0 )
      {
        void *hdet_obj = NULL;
        fb_power_samples = 
            tx_measure->tx_meas_param[device].fb_power.fp_power_params.fb_power_samples;

        /* Check for div-by-0 */
        if( fb_power_samples == 0)
        {
          FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_tx_measure: fb_power_samples input is 0");
          return FALSE;
        }

        hdet_obj = ftm_common_get_tuner_object(RFM_DEVICE_0, RFDEVICE_HDET);
        rfdevice_hdet_cmn_set_gain_state( hdet_obj, 
                                          NULL, 
                                          RFDEVICE_EXECUTE_IMMEDIATE,
                                          tx_measure->tx_meas_param[device].fb_power.fp_power_params.fb_gain_state, 
                                          0 );

        cur_band = ftm_curr_tdscdma_mode;

        /* Get HDET measurements */
        for( fb_power_samples_idx = 0; 
             fb_power_samples_idx < fb_power_samples; 
             fb_power_samples_idx++)
        {
          /* Do up to 3 tries per HDET read */
          local_read_count = 0;
          do
          {

            (void)rfdevice_hdet_tdscdma_tx_hdet_read(RFM_DEVICE_0,
                                         cur_band,
                                         FALSE,
                                         &temp_hdet_val,
                                         NULL,
                                         RFDEVICE_EXECUTE_IMMEDIATE
                                         );

            local_read_count++;
          }while ( ((temp_hdet_val <= 0) || temp_hdet_val == 0xFFFF)
                   && (local_read_count < 3));
      
          /* Add to accum */
          hdet_accm += temp_hdet_val;
        }
           
        /* Take avg */
        hdet_accm = hdet_accm / fb_power_samples;

        /* Populate result */
        cal_result->tx_meas_param[device].fb_power_meas.fb_pwr_meas_res = 
                                                      (int16)(100 * log10((float)hdet_accm));

      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_FB_PWR ) ) != 0 ) */

      /* Populate Tx Meas action */
      cal_result->tx_meas_param[device].tx_meas_action =
                          tx_measure->tx_meas_param[device].tx_meas_action;

      /* Update OpCode to set the processed bit to zero. */
      dev_temp = dev_temp & (~dev_mask);

      /* For the current unprocessed Opcode, extract the bit mask for the first
      enabled cal action. The first bit will be from LSB. */
      dev_mask = (uint8)rf_extract_first_one_bit_mask( dev_temp );

      /* Print Extra Debug message */
      if ( ftm_tdscdma_calv3_dbg_msg_flag == TRUE )
      {
        FTM_MSG_3( FTM_HIGH, "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_tx_measure:"
                   " Dev %d, tx_meas_action %d, hdet_read %d", device,
                   tx_measure->tx_meas_param[device].tx_meas_action,
                   cal_result->tx_meas_param[device].hdet.hdet_read );
      }

    } /* if (device == RFM_DEVICE_0) */
  } /* while ( dev_mask != 0 ) */

  /* Populate common results */
  cal_result->device_mask = tx_measure->device_mask;

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_3(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_tx_measure: [Status %d] "
             "DevMask %d tmTime[%d]us",
             ret_val, tx_measure->device_mask,
             timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
             );
  }

  return ret_val;

} /* ftm_tdscdma_calv3_tx_measure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Rx Measurement

  @details
  This function will be called from cal v3 common framework to perform
  Rx measurement operation. Rx measurement involves operation of sampling RxAGC
  to compute the offset between the expected and actual Rx AGC

  @param rx_measure
  Pointer to the data structure containing Rx meas unpacked parameter

  @param cal_result
  Pointer to the data structure to which the results is supposed
  to be populated

  @return
  Flag indicating the status of Rx Measurement operation
*/
boolean
ftm_tdscdma_calv3_rx_measure
(
  ftm_cal_rx_meas_parameter_type const *rx_measure,
  ftm_cal_rx_meas_result_data_type *cal_result
)
{
  uint8 dev_mask = 0xFF; /*  */
  uint8 dev_temp = 0xFF; /* Variable to store the Device. This will be modified
                            in the while loop. */
  rfm_device_enum_type device; /* RFM Device */
  boolean ret_val = TRUE;
  int16 lna_offset; /* LNA Offset Value */

  /* Parameter validation */
  if ( ( rx_measure == NULL ) || ( cal_result == NULL ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_rx_measure: NULL argument" );
    return FALSE;
  } /* if ( ( rx_measure == NULL ) || ( cal_result == NULL ) ) */

  /* Temporarily store the Device mask */
  dev_temp = rx_measure->device_mask;

  /* For the current unprocessed Opcode, extract the bit mask for the first
  enabled cal action. The first bit will be from LSB. */
  dev_mask = rf_extract_first_one_bit_mask( dev_temp );

  /* Loop until, the OpCode Mask comes out to be zero */
  while ( dev_mask != 0 )
  {
    /* Find the bit location and this location will correspond to
    RFM device */
    device = (rfm_device_enum_type)(
                        rf_extract_first_one_bit_lsb_location(dev_mask) );

    /* Check if the device is valid */
    if ( device < FTM_CAL_V3_RX_MAX_DEV )
    {
      lna_offset = (ftm_calibration_rx_meas_data_type)ftm_tdscdma_get_lna_offset
                          (device,
                           rx_measure->rx_meas_param[device].lna_offset.lna_state,
                           (word)rx_measure->rx_meas_param[device].lna_offset.expected_exagc,
                           rx_measure->rx_meas_param[device].lna_offset.power_mode
                           );

      /* Populate device specific results */
      cal_result->rx_meas_param[device].rx_meas_action =
                          rx_measure->rx_meas_param[device].rx_meas_action;
      cal_result->rx_meas_param[device].lna_offset.lna_state =
                          rx_measure->rx_meas_param[device].lna_offset.lna_state;
      cal_result->rx_meas_param[device].lna_offset.power_mode =
                          rx_measure->rx_meas_param[device].lna_offset.power_mode;
      cal_result->rx_meas_param[device].lna_offset.lna_offset = lna_offset;


      /* Update OpCode to set the processed bit to zero. */
      dev_temp = dev_temp & (~dev_mask);

      /* For the current unprocessed Opcode, extract the bit mask for the first
      enabled cal action. The first bit will be from LSB. */
      dev_mask = rf_extract_first_one_bit_mask( dev_temp );

      if(ftm_tdscdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_2(FTM_HIGH,
                  "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_rx_measure: device %d, lna_offset %d",
                  device, lna_offset );
      }
    } /* if ( device < RFM_MAX_DEVICES ) */
    else
    {
      ret_val = FALSE;
      FTM_MSG_1( FTM_ERROR, "ftm_tdscdma_calv3_rx_measure: Dev %d "
                 "not valid", device );
      break;
    } /* if ! ( device < RFM_MAX_DEVICES ) */
  } /* while ( dev_mask != 0 ) */

  /* Populate common results */
  cal_result->device_mask = rx_measure->device_mask;

  return ret_val;

} /* ftm_tdscdma_calv3_rx_measure */

/*----------------------------------------------------------------------------*/
/*!
  FUNCTION FTM_TDSCDMA_CALV3_RETUNE_RADIO

  @brief
  Retune TDSCDMA Radio for Calibration

  @details
  This function retunes TDSCDMA radio to new channel. Based on the enabled master
  devices list, all master devices and their associated devices will be
  retuned to a new TDSCDMA channel. This function sets the Tx power to the lowest
  possible to avoid false trigger to the calibration equipment.

  HDET state will not be altered as HDET will be enabled till the end of the
  calibration sweep.

  Since this command will be sent by embedded side itself, the response
  packet will be created in such a way that no response is sent back to DIAG.

  @param *ftm_req_data
  FTM Request Data to retune radio

  @return
  Response packet for the command indicating that there is no response to DIAG.
*/
ftm_rsp_pkt_type
ftm_tdscdma_calv3_retune_radio
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_tdscdma_cal_sweep_retune_radio_req_type* header =
    (ftm_tdscdma_cal_sweep_retune_radio_req_type*)ftm_req_data;

  /* Create response Packets */
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};

  rfcom_tdscdma_band_type target_rfcom_band = rf_tdscdma_core_util_get_band_from_uarfcn(header->channel);
  ftm_mode_id_type target_ftm_band = ftm_tdscdma_util_convert_band_rfcom_to_ftm(target_rfcom_band);

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Print debug msg if enabled: Function start time tick */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_4(FTM_HIGH,
              "TDSCDMA CAlV3 DBG - ftm_tdscdma_calv3_retune_radio: mode %d, band, %d chan %d, dc_channel_offset %d",
              header->mode,
              target_ftm_band,
              header->channel,
              header->dc_channel_offset
              );
  }
  /*tune to chan will tear down both rx and tx, thus no need to set lowest pdm*/

  ftm_tdscdma_calv3_tune_band_chan(target_ftm_band,
                                 header->channel,
                                 header->dc_channel_offset,
                                 drx_enable_flag
                                 );

  /* Populate a "No Response" packet */
  ftm_rsp_data.cmd = FTM_RSP_NO_RSP;
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_3(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_retune_radio, start_tick = %d, stop_tick = %d, elapse = %d us",
              ftm_tdscdma_prof_start_time,
              timetick_get(),
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC ));
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_tdscdma_calv3_retune_radio */

/*----------------------------------------------------------------------------*/
/*!
  FUNCTION FTM_TDSCDMA_CALV3_TEAR_DOWN_RADIO

  @brief
  Tear down TDSCDMA Radio after Calibration

  @details
  This function tears down CDMA radio to SLEEP state. This function
  also disabled HDET at the end of tear down.

  Since this command will be sent by embedded side itself, the response
  packet will be created in such a way that no response is sent back to DIAG.

  @param *ftm_req_data
  FTM Request Data to tear down radio

  @return
  Response packet for the command indicating that there is no response to DIAG.
*/
ftm_rsp_pkt_type
ftm_tdscdma_calv3_tear_down_radio
(
  void  *ftm_req_data
)
{
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};

  ftm_tdscdma_prof_start_time = timetick_get();

  /*turn off Rx*/
  ftm_tdscdma_set_rx(FALSE);
  /*turn off Tx*/
  ftm_tdscdma_set_tx(FALSE);

  /* Set Lowest PA State  */
  ftm_tdscdma_set_pa_range(rf_tdscdma_core_util_get_pa_range_from_nv(0));

  /* Get PDM limit and set PDM to lowest + 1 */
  ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_tdscdma_pdm_limits.min_pdm+1);

  ftm_rfmode_exit(RFM_DEVICE_0, FTM_STATE_PARK);

  /* Clear div status if it has been set */
  if(ftm_tdscdma_div_status == TRUE)
  {
    ftm_tdscdma_set_secondary_chain(FALSE);
  }  

  /* Populate a "No Response" packet */
  ftm_rsp_data.cmd = FTM_RSP_NO_RSP;
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_3(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_tear_down_radio, start_tick = %d, stop_tick = %d, elapse = %d us",
              ftm_tdscdma_prof_start_time,
              timetick_get(),
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC ));
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_tdscdma_calv3_tear_down_radio */

/*! \} */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_TDSCDMA_CALV3_TUNE_BAND_CHAN
  Tune Radio for given channel and channel offset.

  @details
  Tune Radio for given channel and channel offset. If channel offset is
  non-zero then dual-carrier mode is used.

  @param mode
     RF mode

  @param chan
     UL channel to tune to

  @param dc_chan_offset
     Channel offset for dual-carrier

  @param drx_enable_flag
     Flag for whether DRx needs to be enabled or not
 */
LOCAL void ftm_tdscdma_calv3_tune_band_chan(ftm_mode_id_type mode,
                                          uint16 chan,
                                          uint16 dc_chan_offset,
                                          boolean drx_enable_flag)
{
  // Configure Diversity
  if (drx_enable_flag)
  {
    ftm_tdscdma_set_secondary_chain(TRUE);
  }

  /* Skip enter_mode if already done so.
  must do this check otherwise  ftm_tdscdma_set_mode() will always
  tune to a default chan for each and every freq*/
  if ( ftm_tdscdma_current_mode_id != mode)
  {
    ftm_tdscdma_set_mode(mode);
  }
  else //already in TDS mode, ensure both rx and tx are off.
  {
    /* Turn off Tx */
    ftm_tdscdma_set_tx(FALSE);
    /* Turn off Rx */
    ftm_tdscdma_set_rx(FALSE);
  }

  ftm_tdscdma_tune_to_chan(FTM_RECEIVE_CHAIN_0, chan);

  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_2(FTM_HIGH,
              "TDSCDMA CAlV3 DBG - ftm_tdscdma_calv3_tune_band_chan: mode %d, chan %d",
              mode,
              chan
              );
  }

} /* ftm_tdscdma_calv3_tune_band_chan */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RX AGC Threshold Reached event callback

  @details
  This function will call RFM function to check if RX AGC is reached given
  threshold

  @param polling_payload
  Pointer to the polling payload

  @return
  Flag indicating the if the event is met or not,

  @retval TRUE
  Flag indicating the event is met

  @retval FALSE
  Flag indicating the event is not yet met
*/
static boolean
ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached
(
  ftm_cal_polling_instr_payload_type const *polling_payload,
  void const *event_payload
)
{
  ftm_cal_polling_event_1_payload_type *event_pl = (ftm_cal_polling_event_1_payload_type*)event_payload;

  /* Validation */
  if ( polling_payload == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: NULL Payload" );
    return FALSE;
  } /* if ( polling_payload == NULL ) */

  /* Validation */
  if ( event_payload == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: NULL Event Payload" );
    return FALSE;
  } /* if ( event_payload == NULL ) */

  /* Validation */
  if (    SIGNAL_RISING  != event_pl->direction
       && SIGNAL_FALLING != event_pl->direction )
  {
    FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: Invalid direction argument" );
    return FALSE;
  } /* if ( SIGNAL_RISING ... ) */

  {
    int16 rx_agc_data_dbm10;
    int16 rx_level_agc;

    MSG_3(MSG_SSID_FTM, FTM_LOW, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: Reading AGC from device %d (RFM %d), threshold %d dBm10",
          event_pl->primary_device, ftm_calibration_dev_flag_to_rfm_dev(event_pl->primary_device), event_pl->threshold);

    /* Call RFM function to get current RX AGC*/
    if( FALSE == ftm_tdscdma_get_rx_level_dbm(ftm_calibration_dev_flag_to_rfm_dev(event_pl->primary_device), &rx_level_agc, &rx_agc_data_dbm10))
    {
      FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: Failed to read RX AGC" );
      return FALSE;
    }

    MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: Current RX reading: %d dBm10", rx_agc_data_dbm10);

    if( SIGNAL_RISING == event_pl->direction )
    {
      MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: Waiting signal to rise above or equal %d dBm10", event_pl->threshold);
      if( event_pl->threshold <= rx_agc_data_dbm10 )
      {
        MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: Rising signal threshold %d dBm10 reached.", event_pl->threshold);
        return TRUE;
      }
    }
    else
    {
      MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: Waiting signal to fall below or equal %d dBm10", event_pl->threshold);
      if( event_pl->threshold >= rx_agc_data_dbm10 )
      {
        MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached: Falling signal threshold %d dBm10 reached", event_pl->threshold);
        return TRUE;
      }
    }
  }

  return FALSE;
} /* ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached */


/*----------------------------------------------------------------------------*/
boolean
ftm_tdscdma_calv3_sweep_polling_event_init
(
  void
)
{
  boolean ret_val = TRUE;

  ret_val &= ftm_calibration_polling_register_event( RFM_TDSCDMA_MODE,
                                           POLLING_EVENT_RXAGC_THRES_REACHED,
                                           ftm_tdscdma_calv3_sweep_rx_agc_threshold_reached );

  FTM_MSG( FTM_MED, "ftm_tdscdma_calv3_sweep_polling_event_init: Events registered" );

  return ret_val;

} /* ftm_tdscdma_calv3_sweep_polling_event_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure XPT

  @details
  This function will call FTM function in order to configure FW for
  XPT calibration Mode

  @param payload
  Payload associated with the XPT configuration

  @return
  Flag indicating the status of the XPT configure action
*/
boolean
ftm_tdscdma_cal_xpt_configure
(
  ftm_cal_xpt_instr_payload_ept_config_type const *payload
)
{
  boolean ret_val = TRUE;
  uint8 device;
  uint8 device_fb;
  //uint8 xpt_mode;
  /* Start of profile */
  ftm_tdscdma_prof_start_time = timetick_get();

  /* NULL pointer check */
  if ( payload == NULL )
  {
    FTM_MSG( FTM_LOW, "ftm_tdscdma_cal_xpt_configure : NULL Payload" );
    return FALSE;
  } /* if ( payload == NULL ) */
  else
  {
    device = payload->primary_device;
    device_fb = payload->feedback_device;
    //xpt_mode = payload->xpt_mode;

    /* Initialize buffer pool for xpt captures */
    rfcommon_core_xpt_init_buffers();

    /* Perform EPT Configure */
    ret_val &= ftm_tdscdma_xpt_cal_config(device,
                                         device_fb,
                                         payload->xpt_mode,
                                         TRUE );

    /* Initialize data module for the current channel */
    ret_val &= ftm_cal_data_init_cur_channel_map(ftm_tdscdma_current_chan);

  } /* if ! ( payload == NULL ) */

  FTM_MSG_2( FTM_LOW, "ftm_tdscdma_cal_xpt_configure : [Status %d] Profile %d us ",
             ret_val, timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC ) );

  /* Return */
  return ret_val;

} /* ftm_tdscdma_cal_xpt_configure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  IQ Capture

  @details
  This function will call FTM function in order to send a command to FW to
  perform XPT IQ Capture

  @param payload
  ftm required data associated with the XPT IQ Capture

  @return
  Rsp packet type of the XPT IQ Capture action
*/
ftm_rsp_pkt_type
ftm_tdscdma_cal_xpt_iq_capture
(
  void  *ftm_req_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_xpt_instr_payload_iq_capture_type const *payload = NULL; /* RS param */
  rf_common_xpt_buffer_id_type buffer_ids; /* buffer ids for capture and dpd
                                              results */
  boolean buff_alloc = TRUE; /* flag indicating if the buffer is allocated */
  boolean iq_cap_status;
  uint32 samp_rate = 0;

  /* Populate a "No Response" packet */
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */

  /* Start of profile */
  ftm_tdscdma_prof_start_time = timetick_get();

  /* Request Packet */
  payload = &(((ftm_cal_xpt_instr_req_type*)ftm_req_data)->xpt_param);
  /* NULL pointer check */
  if ( payload == NULL )
  {
    FTM_MSG( FTM_LOW, "ftm_tdscdma_cal_xpt_iq_capture : NULL Payload" );
    ret_val = FALSE;
  } /* if ( payload == NULL ) */
  else
  {
    /* Send IQ Capture command only if previous capture is done */
    if ( rfcommon_xpt_is_capture_done() == TRUE )
    {
      /* Indicate that the IQ capture is in process */
      ret_val &= rfcommon_xpt_set_capture_in_progress();

      /* Set capture and processing flags */
      ret_val &= rfcommon_xpt_set_processing_in_progress();

      /* Get buffer and results container ID */
      buff_alloc = rfcommon_core_xpt_get_buffers( &buffer_ids );

      /* Start IQ capture */
      if ( buff_alloc == TRUE )
      {
         /* Only update DPD map if capture type is DPD */
        if ((rfcommon_xpt_capture_type)payload->capture_type < XPT_CAPTURE_DELAY)
        {

          FTM_MSG_4(FTM_MED,"ftm_tdscdma_cal_xpt_iq_capture: Push to DPD Map - Chan: %d, CapType: %d, CaptBuf: %d, ResBuf: %d",
                                              ftm_tdscdma_current_chan,
                                              (rfcommon_xpt_capture_type)payload->capture_type,
                                              buffer_ids.capt_buf_id,
                                              buffer_ids.res_buf_id);

          /* Create a new data module table using the current channel */
          ret_val &= ftm_cal_data_push_cur_override_to_dpd_map(ftm_tdscdma_current_chan,
                                                               buffer_ids.capt_buf_id,
                                                               buffer_ids.res_buf_id);

        }


        /* Send command to perform IQ capture */
        FTM_MSG_8(FTM_MED, "ftm_tdscdma_cal_xpt_iq_capture: primary_device - %d, feedback_device - %d, capture_type - %d, num_samples - %d, first_trigger - %d, last_trigger - %d, proc_flag - %d, samp_rate - %d",
                  payload->primary_device,
                  payload->feedback_device,
                  payload->capture_type,
                  payload->num_samples,
                  payload->first_trigger,
                  payload->last_trigger,
                  ftm_calibration_xpt_map_proc_type(payload->proc_flag),
                  &samp_rate);
        iq_cap_status =  ftm_tdscdma_xpt_iq_capture(payload->primary_device,
                                                    payload->feedback_device,
                                                    payload->capture_type,
                                                    payload->num_samples,
                                                    payload->first_trigger,
                                                    payload->last_trigger,
                                                    ftm_calibration_xpt_map_proc_type(payload->proc_flag),
                                                    buffer_ids,
                                                    &samp_rate);

        ret_val &= iq_cap_status;
        if (payload->capture_type == XPT_CAPTURE_DELAY)
        {
          FTM_MSG_2(FTM_MED, "ftm_tdscdma_cal_xpt_iq_capture: Set Samp Rate in Data Module - Chan: %d, SampRate: %d",
                    ftm_tdscdma_current_chan, samp_rate );

          /*Delay processing currently not supported in TDS*/
          //ret_val &= ftm_cal_data_delay_capt_set_samp_rate(ftm_tdscdma_current_chan, samp_rate);
        }

        /* if xpt_iq_capture failed, unvote modem bus clk immediately;
           otherwise, modem bus clk will be unvoted at capture done response
           when last_trig is true*/
        if ( iq_cap_status == FALSE )
        {
          /* If FBRx IQ Capture failed for FBRx then do nothing, use Feedback Setup API to explicitly deconfig */
          if (payload->capture_type == XPT_CAPTURE_FBRX)
          {
            // do nothing
            FTM_MSG(FTM_HIGH, "ftm_tdscdma_cal_xpt_iq_capture: FBRx type IQ Capture failed,"
                                 " use feedback_setup api to turn off FB device");
          }
          else
          {
          ftm_tdscdma_xpt_cal_config(payload->primary_device,
                                     payload->feedback_device,
                                       (uint8)FTM_XPT_CONFIG_EPT,
                                     FALSE );
        }
        }

      } /* if ( buff_alloc == TRUE ) */
      else
      {
        ret_val = FALSE;
        FTM_MSG( FTM_ERROR, "ftm_tdscdma_cal_xpt_iq_capture: Buffer could not "
                            "be allocated" );
      } /* if ! ( buff_alloc == TRUE ) */

    } /* if ( rfcommon_xpt_is_capture_done() == TRUE ) */
    else
    {
      FTM_MSG( FTM_ERROR, "ftm_tdscdma_cal_xpt_iq_capture: Previous IQ capture"
                          "is not complete" );
    } /* if ! ( rfcommon_xpt_is_capture_done() == TRUE ) */
  } /* if ! ( payload == NULL ) */

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_2( FTM_LOW, "ftm_tdscdma_cal_xpt_iq_capture : [Status %d] Profile %d us ",
               ret_val, timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC ) );
  }
  /* Return */
  return ftm_rsp_data;
} /* ftm_tdscdma_cal_xpt_iq_capture */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_INT_CALV3_FEEDBACK_SETUP

  @details
  This function will be called from cal v3 common framework to perform
  feedback setup operation.

  @param feedback_setup
  Pointer to the data structure containing radio setup unpacked parameter

  @return
  Flag indicating the status of feedback setup operation
*/
boolean ftm_tdscdma_calv3_sweep_feedback_setup
(
  ftm_cal_feedback_setup_parameter_type const *feedback_setup
)
{
  /* Config radio request */
  ftm_cal_feedback_setup_req_type fb_req;

  /* ToDo: Get the dbg flag from common data structure */
  ftm_tdscdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Check for NULL ptr */
  if(feedback_setup == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_radio_setup: NULL radio_setup ptr!");
    return FALSE;
  } /* if ( radio_setup == NULL ) */

  /* Create Request Packet for radio setup */
  fb_req.cmd = DIAG_SUBSYS_CMD_F;
  fb_req.sub_sys = FTM;
  fb_req.mode = FTM_TDSCDMA_C;
  fb_req.cdma_cmd = FTM_INT_CALV3_FEEDBACK_SETUP;

  memscpy( &(fb_req.fb_param),
	      sizeof(ftm_cal_feedback_setup_parameter_type),
	      feedback_setup,
        sizeof(ftm_cal_feedback_setup_parameter_type) );

  /* If this segment contains only a WTR config command, call it in ISR context */
  if ( feedback_setup->feedback_setup_action == FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR )
  {
    (void)ftm_tdscdma_calv3_feedback_setup( (void *)(&fb_req) );
  }
  else
  {
    /* Queue up the FTM command */
    ftm_common_send_radio_command( &fb_req,
                                   sizeof(ftm_cal_feedback_setup_req_type) );
  }

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_2(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_radio_setup: mode = %d, elapse time: %d",
              fb_req.mode,
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  return TRUE;
} /* ftm_tdscdma_calv3_sweep_feedback_setup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform XPT sample capture
 
  @details
  This function will be called from cal v3 common framework to perform
  XPT sample capture.
 
  @param payload
  Payload associated with the XPT IQ Capture
 
  @return
  Flag indicating the status of the XPT IQ Capture action
*/
boolean
ftm_tdscdma_calv3_sweep_xpt_iq_capture
(
  ftm_cal_xpt_instr_payload_iq_capture_type const *payload
)
{
   /* Config XPT IQ capture request */
  ftm_cal_xpt_instr_req_type xpt_req;

  /* ToDo: Get the dbg flag from common data structure */
  ftm_tdscdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  ftm_tdscdma_prof_start_time = timetick_get();

  /* Check for NULL ptr */
  if(payload == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_sweep_xpt_iq_capture: NULL payload ptr!");
    return FALSE;
  } /* if ( payload == NULL ) */
  
  /* Create Request Packet for radio setup */
  xpt_req.cmd = DIAG_SUBSYS_CMD_F;
  xpt_req.sub_sys = FTM;
  xpt_req.mode = FTM_TDSCDMA_C;
  xpt_req.cdma_cmd = FTM_INT_CALV3_XPT_IQ_CAPTURE;

  memscpy( &(xpt_req.xpt_param), 
           sizeof(ftm_cal_xpt_instr_payload_iq_capture_type), 
           payload, 
           sizeof(ftm_cal_xpt_instr_payload_iq_capture_type) );

  /* Queue up the FTM command */
  ftm_common_send_radio_command( &xpt_req,
                                 sizeof(ftm_cal_xpt_instr_req_type) );

  /* Print debug msg if enabled */
  if(ftm_tdscdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_2(FTM_HIGH,
              "TDSCDMA CalV3 DBG - ftm_tdscdma_calv3_sweep_xpt_iq_capture: cmd = %d, elapse time: %d", 
              xpt_req.cdma_cmd,
              timetick_get_elapsed( ftm_tdscdma_prof_start_time, T_USEC )
              );
  }

  return TRUE;
}/* ftm_tdscdma_calv3_sweep_xpt_iq_capture */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Feedback Setup

  @details
  This function will be called from cal v3 common framework to perform
  feedback setup operation.

  @param ftm_req_data
  Pointer to the data structure containing radio setup unpacked parameter

  @return
  ftm_rsp_pkt_type
*/

ftm_rsp_pkt_type ftm_tdscdma_calv3_feedback_setup(void *ftm_req_data)
{
  const rfc_common_logical_device_params_type *cmn_logical_dev = NULL;
  ftm_cal_feedback_setup_parameter_type const *feedback_setup = NULL;
  rfdevice_rxtx_common_class* device_ptr = NULL;
  rfdevice_trx_tdscdma_tx** tds_device_ptr = NULL;
  lm_handle_type fbrx_lm_handle;
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  /* Populate a "No Response" packet */
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */

  /* Parameter validation */
  if ( ftm_req_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_feedback_setup: NULL argument" );
    return ftm_rsp_data;
  } /* if ( ftm_req_data == NULL ) */

  /* Get the pointer to feedback setup param */
  feedback_setup = &(((ftm_cal_feedback_setup_req_type*)ftm_req_data)->fb_param);

  if ( feedback_setup->feedback_setup_action ==
       FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR )
  {
    /* Set internal flag to enable or disable Tx Self Cal */
    if ( feedback_setup->configure_wtr.enable_mode != 0 )
    {
       ftm_tdscdma_config_tx_selfcal(TRUE);
    }
    else
    {
       ftm_tdscdma_config_tx_selfcal(FALSE);
    }

    /* Get pointer to the common logical device */
    cmn_logical_dev = rfc_common_get_logical_device_params(
       (rfm_device_enum_type)feedback_setup->primary_device );

    if (NULL != cmn_logical_dev)
    {
      rfdevice_cmn_set_et_path(cmn_logical_dev->cmn_device,
                     RFM_TDSCDMA_MODE,
                     feedback_setup->configure_wtr.fb_state,
                     feedback_setup->configure_wtr.enable_mode);
    }
    else
    {
      FTM_MSG( FTM_ERROR, "ftm_tdscdma_calv3_feedback_setup: Could not get logical device" );
    }

    //device_ptr = rfc_common_get_cmn_device_object ( 0 );
    tds_device_ptr = (rfdevice_trx_tdscdma_tx**) rfc_tdscdma_get_rf_device_obj(
                                (rfm_device_enum_type)feedback_setup->primary_device, 
                                RFC_CONFIG_TX, 
                                RFCOM_BAND_TDSCDMA_B34, 
                                RFDEVICE_TRANSCEIVER);

    if (tds_device_ptr != NULL)
    {
      device_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(tds_device_ptr[0]);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "rfc_tdscdma_get_rf_device_obj failure. Null pointer detected. Tx band: %d", 
            RFCOM_BAND_TDSCDMA_B34);
      return ftm_rsp_data;
    }

    if (NULL != device_ptr)
    {
      if (feedback_setup->configure_wtr.enable_mode == TRUE)
      {
        rfcommon_fbrx_mc_enable_rf_dev(device_ptr, feedback_setup->configure_wtr.fb_state);
      }
      else
      {
        rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
      }
    }
    else
    {
      FTM_MSG(FTM_ERROR, "ftm_tdscdma_calv3_feedback_setup: Could not get RFC rxtx common device");
    }
  }

  if ( feedback_setup->feedback_setup_action &
            FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_XPT )
  {
    FTM_MSG( FTM_ERROR, "FEEDBACK_SETUP_ACTION CONFIGURE_RXLM_FOR_XPT not supported in Tech: TDSCDMA" );
  }

  if ( feedback_setup->feedback_setup_action &
            FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_FBAGC )
  {
    fbrx_lm_handle = rfcommon_fbrx_mc_get_tech_handle( RFM_TDSCDMA_MODE );

    ret_value = rfcommon_fbrx_msm_update_static_settings(fbrx_lm_handle, RFCOMMON_FBRX_TDS);

    if (ret_value == RFCOMMON_FBRX_SUCCESS) 
    {
      ret_value = rfcommon_fbrx_enable( fbrx_lm_handle );

      if (ret_value == RFCOMMON_FBRX_SUCCESS) 
      {
        FTM_MSG(FTM_LOW,"FEEDBACK_SETUP_ACTION re-enabled FBRX");
      }
    }
  }

  return ftm_rsp_data;
} /*ftm_tdscdma_calv3_feedback_setup*/

LOCAL rfcom_tdscdma_band_type ftm_tdscdma_calv4_convert_band(uint16 band)
{
 rfcom_tdscdma_band_type ret_band = RFCOM_BAND_TDSCDMA_INVALID;

 switch(band)
  {
    case TDSCDMA_FTM_BAND_B34:
      ret_band = RFCOM_BAND_TDSCDMA_B34;
      break;

    case TDSCDMA_FTM_BAND_B39:
      ret_band = RFCOM_BAND_TDSCDMA_B39;
      break;

    case TDSCDMA_FTM_BAND_B40:
      ret_band = RFCOM_BAND_TDSCDMA_B40;
      break;

    default:
      ret_band = RFCOM_BAND_TDSCDMA_INVALID;
      break;
  }
 return ret_band;
} /* ftm_tdscdma_calv4_convert_band */

LOCAL void ftm_tdscdma_calv3_set_current_mode(rfcom_tdscdma_band_type band)
{

 switch(band)
 {
   case RFCOM_BAND_TDSCDMA_B34:
         ftm_current_rf_mode = FTM_DB_RF_TDSCDMA_B34;
         ftm_curr_tdscdma_mode = ftm_tdscdma_util_convert_band_ftm_to_rfcom(FTM_PHONE_MODE_TDSCDMA_B34);
         ftm_tdscdma_current_mode = RFCOM_BAND_TDSCDMA_B34;
         ftm_tdscdma_current_mode_id = FTM_PHONE_MODE_TDSCDMA_B34;
         if (ftm_tdscdma_current_pri_device == RFM_INVALID_DEVICE)
         {
         ftm_tdscdma_current_pri_device = RFM_DEVICE_0;
         ftm_tdscdma_current_sec_device = RFM_DEVICE_1;
           ftm_tdscdma_current_tx_device = RFM_DEVICE_0;
         }

	 break;

   case RFCOM_BAND_TDSCDMA_B39:
         ftm_current_rf_mode = FTM_DB_RF_TDSCDMA_B39;
         ftm_curr_tdscdma_mode = ftm_tdscdma_util_convert_band_ftm_to_rfcom(FTM_PHONE_MODE_TDSCDMA_B39);
         ftm_tdscdma_current_mode = RFCOM_BAND_TDSCDMA_B39;
         ftm_tdscdma_current_mode_id = FTM_PHONE_MODE_TDSCDMA_B39;
         if (ftm_tdscdma_current_pri_device == RFM_INVALID_DEVICE)
         {
         ftm_tdscdma_current_pri_device = RFM_DEVICE_0;
         ftm_tdscdma_current_sec_device = RFM_DEVICE_1;
           ftm_tdscdma_current_tx_device = RFM_DEVICE_0;
         }

	 break;

   case RFCOM_BAND_TDSCDMA_B40:
         ftm_current_rf_mode = FTM_DB_RF_TDSCDMA_B40;
         ftm_curr_tdscdma_mode = ftm_tdscdma_util_convert_band_ftm_to_rfcom(FTM_PHONE_MODE_TDSCDMA_B40);
         ftm_tdscdma_current_mode = RFCOM_BAND_TDSCDMA_B40;
         ftm_tdscdma_current_mode_id = FTM_PHONE_MODE_TDSCDMA_B40;
      if (ftm_tdscdma_current_pri_device == RFM_INVALID_DEVICE)
      {
         ftm_tdscdma_current_pri_device = RFM_DEVICE_0;
         ftm_tdscdma_current_sec_device = RFM_DEVICE_1;   
        ftm_tdscdma_current_tx_device = RFM_DEVICE_0;
      } 
	 break;

   default:
	 break;
 }

}



