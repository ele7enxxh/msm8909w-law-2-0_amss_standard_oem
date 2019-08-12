/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            F T M   W C D M A   C A L V 3

GENERAL DESCRIPTION
  This is the FTM RF file which contains implementations for WCDMA CalV3

Copyright (c) 2009 - 2015 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/src/ftm_wcdma_calv3.c#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $


when       who     what, where, why 
--------   ---     ----------------------------------------------------------
09/17/15   kma     Added auto pin support for Pin cal
09/16/15   vs      fix WCDMA Cal failures on SSKU card (on 8996 MTP) due to negative M-Line compression
03/17/15   ak      Calculate the LNA offsets correctly when NBEE is disabled.
02/09/15   ak      Calculate the DVGA offsets correctly when NBEE is disabled and for Serial Cal
02/02/15   xs      Add support of disable NBEE() mode in ftm_wcdma_calv3_radio_setup API
12/10/14   vs      Cleanup of F3s printed during CAL
11/13/14   vbh     Add checks for concurrency manager before tune
09/13/14   dj      Fixed fb_read conversion to dBm*10 in TX_MEAS_FB_PWR
10/08/14    ka     Replace rfc with device api for retrieving device pointer
09/23/14   kai     Add WCDMA DRDS Cal support
09/23/14   vbh     Cleanup CW by using arguments of the correct data type
09/03/14   cd      Remove legacy Tx override fields from data module
8/22/14    vbh     Added device parameter in Set_Tx API
07/25/14   aro     Added support for TX_MEAS_FB_PWR action in TxMeas
07/02/14   kr/kai  Non-0dB Pin SW implementation to support -ve pin offsets values
06/27/14   ac      change to enable NBEE cal 
06/26/14   gh      Sleep PRx in tear_down and do enter_mode in radio_setup if cmd is rx/rxtx
06/16/14   vs/kai  Non-0dB Pin SW implementation 
06/04/14   kai     Avoid using script based tune for dual carrier tune
05/20/14   kai     Reduce RF Cal retune time
05/16/14   aro     Added support to perform Tx Power meter read
05/16/14   kai     Added XPT IQ capture offloading to RF thread
05/01/14   vws     Fix to pass the correct pa_state to apply accurate pin compensation  
04/24/14   vbh     Removed direct calls to rfm_wcdma APIs for tune
03/24/14    aa     Remove hardcoded value for fbrx status check
03/06/14   zhw     Normalized Pin Cal support
02/13/14    id     Updates needed for FBRx Cal API to match FBRx capture API
02/05/14   dw      Remove set CW in the Cal sequence
02/04/14  ska     Added Pin cal support
02/04/14   jmf     Adding xpt_mode as arg for ftm_wcdma_xpt_cal_config
02/04/14   kai     Add ASSERT for tx_override called before radio_setup done
02/04/14   kai     Clean up legacy function calling
01/30/14   jmf     Update WCDMA Feedback Setup and support for FBRx type XPT IQ caps
01/13/14    id     Add support for FB Gain Meas in TX Meas Opcode
01/09/14   dw      Fix false trigger by zero out IQ gain during FTM set Tx ON for Cal sweep
01/08/14   cac     Update apply_pin_alignment to reflect change in the ftm common API.
12/13/13   tks     Updated calls to rfdevice tx tune api
12/12/13   zhh     correct msg error category
12/11/13   cd      Pin alignment uses RFLM Tx override info for xPT cal
11/25/13   kai     Conditionally hook up RFLM-TxAGC override
11/14/13   kma     Added pin calibration support
10/22/13   tks     Set tx off in radio setup before tuning to a new uplink
                   frequency 
09/24/13   dw      Obsolete rfwcdma_mdsp_get_lna_offset() reference
09/16/13   dw      Added valid RxLM buffer index to rfm_enter_mode call
09/10/13   aa      Enable/Disable NBEE RXCAL on the fly
09/09/13   aro     Compilation error fix
08/14/13   aro     Support for DC calibration in RX_MEASURE OpCode
08/27/13   dj      Moved the feedback setup handling code to ftm_wcdma_ctl
                   Clear use_fb_path_flag during radio tear down
08/23/13   vs      Added support for 3 carrier
08/21/13   dj      Fix a klocwork error
08/21/13   dj      Moved feedback_setup to be run from the wcdma command dispatch
08/19/13   dj      Support for fb_setup_action 
                   FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_FBAGC
08/16/13   jmf     [xPT] Update internal data structure for xpt capture info
08/15/13   ka      [xPT] Set QPOET into APT mode before retune during W xPT Cal
08/08/13   dj      Added error messages for unsupported fb_set_up actions
08/07/13   kai     [xPT] Move modem bus clk voting into radio setup
08/05/13   kr      replace memcpy with memscpy
08/02/13   dj      Updated feedback setup to use fb_state instead of pa_state
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/12/13   aa      fixed off-target build error
07/10/13   aa      RX calibration optimization using NBEE
06/26/13   kai     Get current Tx channel info from rfwcdma_mc_state in apply_pin_alignment
06/21/13   jmf     [xPT] ET delay implementation for Cal V4
06/04/13   kai     Added clear the DPD map if we are starting calibration
05/28/13   kai     Added DPD enable flag as setting WTR for Tx
05/24/13   rsr     Cal V4 not applying Delay value. Hardcode to use from NV for now. 
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/09/13   kai     Added turning off DRx in tear down
05/07/13   kai     Fix DRx enable in radio setup
05/02/13   cd      Apply Pin alignment during Tx override
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/29/13   cd      Send Therm read to data module for current Tx channel
04/24/13   cd      Add support for Therm read as part of Tx Meas Opcode
04/24/13    dj     Added support for Feedback Setup
04/23/13   cri     Add reference channel to tx override
04/22/13   jmf     Implement xPT processing Enum Type
04/22/13   kai     Removed hard coded DRx enable flag from retune api
04/18/13   kai     Changed Tx agc override api
04/11/13   cri     Use XPT mode in power lookup for uniqueness
04/09/13   cri     Add init and deinit for cal data module
04/09/12   cri     Update set current override API to data module
04/04/13   ka      WCDMA state machine updates
04/03/13   kai     xpt data module hookups for cal 
03/29/13   aro     Updated Tx Override OpCode to support modular payload
03/28/13   kai     Added xpt opcode support for WCDMA
03/21/13   kai     Removed asm_config in retune
03/20/13   kai     Added asm_config_tx after tx_tune_chan
03/06/13   kcj     Correct the order of range and q current setting in tx sweep
03/05/13   kai     Added Cal v4 band info convertiono function
03/01/13   kai     Comment out non-using xPT functions 
02/14/13   ac      cal v3 fix and cleanup to call common api for hdet
02/12/13   jmf     Update SAPT IQ capture fn params to use updated capture fn
02/01/13   adk     Fixed compilation issues with TDET
01/30/13   ka      Updated FTM interfaces to logical device
01/28/13   kai     Remove delay before tx on in radio_setup
01/23/13   kai     Fix tx_measure and rx_measure
01/21/13   kai     Fix off target error
01/21/13   kai     Added calv3_tx_measure and calv3_rx_measure
01/18/13   kai     Fix radio setup crash
01/18/13   aro     Deleted nikel-XPT specific code
12/21/12   aa      KW warning fixes
12/06/12   aki     Added RX threshold aka RX wait functionality
12/06/12   ka      Turn on tx before programming tx modem registers
11/26/12   kai     Added wcdma tx override operation support
11/16/12   kai     Fix warnings 
11/15/12   kai     Added radio setup operation
11/05/12   aro     Re-organized cal v3 data structure to be more modular
10/15/12   kai     Changed QFE1510 HDET APIs
08/02/12   kai     Added feature FEATURE_RF_HAS_QFE1510_HDET for QFE1510 HDET
10/08/12   ac      enabling drx rf cal
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
08/31/21   ac      disable diveristy by default
07/18/12   aro     Renamed the XPT type definitions
07/18/12   aro     Moved the XPT result pushing to common XPT framework
07/18/12   aro     Added Error Code and SubCopde fields in XPT results
07/17/12   aro     Updated XPT Opcode payload
07/13/12   kai     Added Qtuner feature on including qfe_cmn_intf.h
07/10/12   kai     Add support for Qtuner FTM command
06/22/12   anr     EPT Calibration Enhancements
06/21/12   tks     Changed the device function call to new format 
06/14/12   anr     Moved EPT specific FTM functionality to this file.
06/07/12   shb     Added DPD support in WCDMA TX tune API - disabled by 
                   default
06/04/12   ks      Add FTM-PC interface for XPT
03/10/12   ks      Hookup Cal v3 ET Enable/Disable actions to device
01/12/12   aro     Converted Tx flag and Rx flag to device flag
12/21/11   dw      Do not tune TxPLL is FTM tune if Tx is not enabled
11/16/11    gh     Combine PRx and DRx DVGA/LNA measurements
10/12/11    dj     Fix for cal V3 support for 5 LNA ranges
10/11/11    gh     Change LNA range limit to 5
09/28/11    sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and FEATURE_RF_WCDMA_LM_RFM_INTERFACE. 
09/06/11    gh     Add support for dual carrier cal. Optimize retune segment
08/31/11    gh     Fix re-tune segment for callbox segment overrun
08/26/11    gh     Time reduction for measure_tx
                   Take input as PA state instead of range per requirement
08/25/11    gh     Fix OFT build error
08/19/11    gh     Initial release
===========================================================================*/

/*===========================================================================
 *        Include Files                                                     *
===========================================================================*/
#include "diagcmd.h"
#include "ftmdiag.h"
#include "ftm_msg.h"
#include "ftm.h"
#include "ftm_common.h"
#include "ftm_common_control.h"
#include "rfcom.h"
#include "rfdevice_intf_cmd.h"
#include "rfdevice_type_defs.h"
#include "rfm_device_types.h"
#include "rfcommon_math.h"
#include "ftm_rf_cmd.h"
#include "bsp.h"
#include "rfm.h"
#include "rfm_ftm.h"
#include "rfumts.h"
#include "rfwcdma_msm.h"
#include "rfcommon_core_txlin.h"
#include "rfcommon_core_txlin_types.h"
#include "rfwcdma_core_temp_comp.h"

#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_xpt.h"
#include "ftm_common_data.h"
#include "ftm_wcdma_ctl.h"
#include "ftm_wcdma_dispatch.h"
#include "ftm_wcdma_calv3.h"
#include "ftm_common_fbrx.h"
#include "rfdevice_wcdma_intf.h"
#include "rfwcdma_core_util.h"
#include "rfm_wcdma.h"
#include "wfw_sw_intf.h"
#include "modem_mem.h"

#include "rfdevice_cmn_intf.h"
#include "rfnv_wcdma.h"
#include "rfwcdma_msm.h"
#include "mcpm_api.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rfcommon_mc.h"
#include "rfcommon_mdsp.h"
#include "ftm_common_xpt.h"
#include "rfdevice_wcdma_asm_intf.h"
#include "rfdevice_hdet_wcdma_intf.h"
#include "rfdevice_hdet_cmn_intf.h"
#include "ftm_calibration_data_module.h"
#include "rfwcdma_core_xpt.h"
#include "rfc_wcdma_data.h"
#include "rfcommon_autopin_api.h"
/* memscpy */
#include "stringl.h"
#include "rfcommon_fbrx_types.h"
#include "ftm_common_concurrency_manager.h"
#include <math.h>

#define EPT_MAX_NUM_RGI 20
#define EPT_SAMPLES (9*1024)
#define CACHE_LINE_SIZE 32
#define IMOD(_size, _mod) ((((_size) + (_mod) - 1) / (_mod)) * (_mod)) 
#define EPT_SAMPLE_BUFLEN (EPT_MAX_NUM_RGI *(IMOD(EPT_SAMPLES*2,CACHE_LINE_SIZE) + IMOD(EPT_SAMPLES*4,CACHE_LINE_SIZE)))

/*
static void *ept_buffer = NULL;
static void *ept_fw_addr = 0;
static void *ept_buffer_backup = NULL;
static boolean call_apply_dpd = FALSE;*/

/*===========================================================================
 *        Definitions and Variables/Data Structures                         *
===========================================================================*/
#define  FTM_WCDMA_CALV3_SLAVE_MASK        0x0000000f
#define  FTM_WCDMA_CALV3_SLAVE_ENABLE_VAL  0x00000001
#define  FTM_WCDMA_CALV3_SLAVE_DISABLE_VAL 0x0000000f
#define  FTM_DVGA_GAIN_OFFSET_MAX      511
#define  FTM_DVGA_GAIN_OFFSET_MIN     -512
#define FTM_EPT_IQ_CAPTURE_BUFF_SIZE_MAX 1048576
/*----------------------------------------------------------------------------*/
static boolean ftm_wcdma_calv3_dbg_msg_flag = FALSE; /*!< Flag for enabling  
WCDMA CalV3 debug messages */

/*----------------------------------------------------------------------------*/
static ftm_mode_id_type ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_MAX;
/*!< ToDo: This is a place holder for retune segment to use. Remove this
           when common framework adds support for band info when doing retune
*/

/*----------------------------------------------------------------------------*/
static timetick_type prof_start_time = 0; /*<! Time profile variable  */

/*----------------------------------------------------------------------------*/
static rftx_pdm_limit_type ftm_wcdma_calv3_pdm_limits; /*!< Tx PDM min and max 
limits */  

 
/*----------------------------------------------------------------------------*/
static boolean wcdma_calv3_in_progress = FALSE; 

/*----------------------------------------------------------------------------*/
extern ftm_pkt_data_type ftm_pkt_data; /*!< Current FTM command */  

/*----------------------------------------------------------------------------*/
extern ftm_rf_mode_type ftm_current_rf_mode; /*!< To track the current RF mode */

/*----------------------------------------------------------------------------*/
extern boolean ftm_wcdma_div_status[]; /*!< To track the current diversity status */

/*----------------------------------------------------------------------------*/
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode; /*!< Current W mode */

/*----------------------------------------------------------------------------*/
extern ftm_lm_buf_type ftm_wcdma_rxlm_buffer[]; /*!< RXLM buffer */

extern boolean ftm_wcdma_tx_on_status; /*!< FTM Tx status */
extern ftm_lm_buf_type ftm_wcdma_xpt_fb_path_buffer;

timetick_type prof_rxcal_func_1;

uint8 ftm_num_nbee_read = FTM_NUM_OF_AVG_NBEE_READ ;
uint16 ftm_wcdma_rx_nbee_settling_time = FTM_WCDMA_CALV3_RX_NBEE_SETTLING_TIME ;
uint16 nbee_delay_read = FTM_NBEE_DELAY_READ ;

/*===========================================================================
 *        Local Functions, Variables/Data Structures                        *
===========================================================================*/

/*----------------------------------------------------------------------------*/
uint16 ftm_wcdma_calv3_prx_drx_tune_wait = 500; /*!< Place holder for wait time */

boolean ftm_wcdma_calv3_radio_setup_done = FALSE; /*!< Flag indicates radio setup done or not */

LOCAL void ftm_wcdma_calv3_tune_band_chan(ftm_mode_id_type mode,
                                          uint16 chan,
                                          uint16 dc_chan_offset,
                                          boolean drx_enable_flag);

LOCAL void ftm_wcdma_calv3_get_dvga_lna_gain_offset(uint32 rx_flag, 
                                                    byte lna_range,
                                                    word expected_agc_val,
                                                    ftm_calibration_rx_meas_data_type* result_buffer);

LOCAL void ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr(uint32 rx_flag, 
                                                    byte lna_range,
                                                    word expected_agc_val,
                                                    ftm_calibration_rx_meas_data_type* result_buffer);

LOCAL void ftm_wcdma_calv3_set_current_mode(rfcom_wcdma_band_type band);

LOCAL rfcom_wcdma_band_type ftm_wcdma_calv4_convert_band(uint16 band);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_SWEEP_CONFIGURE_RADIO
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
boolean ftm_wcdma_calv3_sweep_configure_radio
(
  ftm_cal_instr_payload_config_radio_type const *rf_cfg
)
{
  ftm_wcdma_cal_sweep_configure_radio_req_type cfg_req;

  /* ToDo: Get the dbg flag from common data structure */
  ftm_wcdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  ftm_wcdma_calv3_set_cal_flag(TRUE);

  prof_start_time = timetick_get();

  /* Check for NULL ptr */
  if(rf_cfg == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_configure_radio: NULL rf_cfg ptr!");
    return FALSE;
  }
  
  /* Create radio request for config_radio */
  cfg_req.cmd = DIAG_SUBSYS_CMD_F;
  cfg_req.sub_sys = FTM;
  cfg_req.mode = FTM_WCDMA_C;
  cfg_req.wcdma_cmd = FTM_INT_WCDMA_CALV3_CONFIGURE_RADIO;
  cfg_req.req_len = 0;
  cfg_req.rsp_len = 0;
  cfg_req.device_master_mask = rf_cfg->device_master;
  cfg_req.device_slave = rf_cfg->device_slave;
  cfg_req.band = rf_cfg->rf_mode;
  cfg_req.channel = rf_cfg->channel;
  cfg_req.dc_channel_offset = rf_cfg->dc_channel_offset;

  /* Queue up the FTM command */
  ftm_common_send_radio_command( &cfg_req,
                                sizeof(ftm_wcdma_cal_sweep_configure_radio_req_type) );

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_6(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_configure_radio: mode = %d, chan = %d, device_master = %d, device_slave = %d, DC_chan_offset = %d, elapse time: %d", 
              rf_cfg->rf_mode,
              rf_cfg->channel,
              rf_cfg->device_master,
              rf_cfg->device_slave,
              rf_cfg->dc_channel_offset,
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return TRUE;
} /* ftm_wcdma_calv3_sweep_configure_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_SWEEP_RETUNE_RADIO
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
boolean ftm_wcdma_calv3_sweep_retune_radio
(
   ftm_cal_instr_payload_retune_radio_type const *rf_retune
)
{
  /* ToDo: Retune input needs to have band data such that single
     sweep for all bands can be done. Need common framework change to update */

  prof_start_time = timetick_get();

  /* Check if input ptr is null */
  if(rf_retune == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_retune_radio: NULL rf_retune ptr!");
    return FALSE;
  }

  /* Reset PDM to lowest - Need to do this first before tune to chan since it can cause
     callbox segment overrun
  */
  ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_wcdma_calv3_pdm_limits.min_pdm+1);

  ftm_wcdma_calv3_tune_band_chan(ftm_wcdma_calv3_current_mode, /* place holder untill framework update */
                                 rf_retune->channel,
                                 rf_retune->dc_channel_offset,
                                 ftm_wcdma_div_status[RFM_DEVICE_1]                          
                                 );

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_5(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_retune_radio: chan = %d, device_master = %d,"
              " div_status = %d, DC_chan_offset = %d, elapse time %d", 
              rf_retune->channel,
              rf_retune->device_master,
              ftm_wcdma_div_status[RFM_DEVICE_1],
              rf_retune->dc_channel_offset,
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return TRUE;
} /* ftm_wcdma_calv3_sweep_retune_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_SWEEP_CONFIGURE_RX
  Configure Rx for Calibration Sweep
 
  @details
  Configure Rx for calibration based on input parameters. cal_action field in
  the input param is a bit mask that indicates the LNA state. For WCDMA only 
  ONE action is supported per segment. This function will return FALSE if 
  there is more than one cal_action bits.

  rx_flag field in the input param is a bit mask that indicates which
  rf device(s) to be used. Only bits 0 and 1 are used for W where both
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
boolean ftm_wcdma_calv3_sweep_configure_rx
(
   ftm_cal_instr_payload_config_rx_type const *rx_config
)
{
  byte lna_range;

  prof_start_time = timetick_get();

  /* Check if input is NULL */
  if(rx_config == NULL)
  { 
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_configure_rx: NULL input ptr!");
    return FALSE;
  }

  /* Check if there is only one cal action
     WCDMA supports only one Rx Cal action per segment, 
     e.g. 1 LNA state per segment
  */
  if( rf_count_one_bits((uint32)rx_config->cal_action) != 1 )
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_configure_rx: More than one cal action!");
    return FALSE;
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
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_configure_rx: Invalid LNA range!");
    return FALSE;
  }

  /* Setup for PRx */
  if(rx_config->device_flag & DEVICE_FLAG_0)
  {
    /* Check if it is DVGA cal */
    if(rx_config->cal_action == RX_ACTION_LNA0_CAL)
    {
      ftm_wcdma_get_dvga_gain_offset_setup(RFM_DEVICE_0);
    }
    else
    {
      ftm_wcdma_get_lna_offset_setup(RFM_DEVICE_0, lna_range);
    }
  }

  /* Setup for DRx */
  if(rx_config->device_flag & DEVICE_FLAG_1)
  {
    /* Check if it is DVGA cal */
    if(rx_config->cal_action == RX_ACTION_LNA0_CAL)
    {
        ftm_wcdma_get_dvga_gain_offset_setup(RFM_DEVICE_1);
    }
    else
    {
      ftm_wcdma_get_lna_offset_setup(RFM_DEVICE_1, lna_range);
    }
  }

  /* Setup for DRx */
  if(rx_config->device_flag & DEVICE_FLAG_2)
  {
    /* Check if it is DVGA cal */
    if(rx_config->cal_action == RX_ACTION_LNA0_CAL)
    {
        ftm_wcdma_get_dvga_gain_offset_setup(RFM_DEVICE_2);
    }
    else
    {
      ftm_wcdma_get_lna_offset_setup(RFM_DEVICE_2, lna_range);
    }
  }

  /* Setup for DRx */
  if(rx_config->device_flag & DEVICE_FLAG_3)
  {
    /* Check if it is DVGA cal */
    if(rx_config->cal_action == RX_ACTION_LNA0_CAL)
    {
        ftm_wcdma_get_dvga_gain_offset_setup(RFM_DEVICE_3);
    }
    else
    {
      ftm_wcdma_get_lna_offset_setup(RFM_DEVICE_3, lna_range);
    }
  }

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_4(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_configure_rx: rx_flag (devices) = %d, cal_action = %d (which is lna_range %d), elapse time %d", 
              rx_config->device_flag,
              rx_config->cal_action,
              lna_range,
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return TRUE;

} /* ftm_wcdma_calv3_sweep_configure_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_SWEEP_CONFIGURE_TX
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
boolean ftm_wcdma_calv3_sweep_configure_tx
(
   ftm_cal_instr_payload_config_tx_type const *tx_config
)
{  
  ftm_xpt_txagc_override_type ovr_params;

  /* Start of profile */
  prof_start_time = timetick_get();

  /* Check if input is NULL ptr */
  if(tx_config == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_configure_tx: NULL input ptr!");
    return FALSE;
  }
  /* Check if it is device_0 */
  else if(tx_config->device_flag != (ftm_calibration_device_flag_mask_type)DEVICE_FLAG_0)
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_configure_tx: Invalid Tx device!");
    return FALSE;
  }

  /* Program TxAGC override parameters  */

  ovr_params.device = RFM_DEVICE_0;
  ovr_params.xpt_mode = RFCOMMON_MDSP_XPT_MODE_APT;
  ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT;
  ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
  ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
  ovr_params.rgi = tx_config->rgi;
  ovr_params.smps_bias = tx_config->pa_ctl_struct.pa_bias;
  ovr_params.pa_state = tx_config->pa_state;
  ovr_params.pa_curr = tx_config->pa_ctl_struct.quiscent_current;
  ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;

  ftm_wcdma_xpt_override_rf_settings( &ovr_params );

  //debug
      FTM_MSG_3(FTM_HIGH,
              "pa_bias:%d, q_current:%d, q_current_en:%d", 
              tx_config->pa_ctl_struct.pa_bias,
              tx_config->pa_ctl_struct.quiscent_current,
              tx_config->pa_ctl_struct.quiscent_current_en);
          
  if(tx_config->cal_action & TX_ACTION_FB_RX_ENABLE)
  {
    ftm_wcdma_set_et_path( tx_config->pa_state, TRUE );
  }
  if(tx_config->cal_action & TX_ACTION_FB_RX_DISABLE)
  {
    ftm_wcdma_set_et_path( tx_config->pa_state, FALSE );
  }

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_4(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_configure_tx: pa_bias = %d, pa_state = %d, pa_range = %d, rgi = %d, elapse time %d", 
              tx_config->pa_bias,
              tx_config->pa_state,
              tx_config->rgi,
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return TRUE;
  
} /* ftm_wcdma_calv3_sweep_configure_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_SWEEP_MEASURE_RX
  Measure Rx for calibration sweep
 
  @details
  Measure Rx for calibration sweep and populate results (DVGA or LNA offsets)
  in to the input result buffer.

  @param rx_meas
  Pointer to the data structure containing Rx configuration parameter:
 
  cal_action: Bit mask corresponding to LNA state
     BIT[0]: Corresponds to gain state 0 -> get DVGA offset
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
boolean ftm_wcdma_calv3_sweep_measure_rx
(
  ftm_cal_instr_payload_meas_rx_type const *rx_meas,
  ftm_calibration_rx_result_type *rx_result
)
{

  boolean success_flag = TRUE;           /* Function success flag               */
  uint32  cal_action_mask = 0xFFFFFFFF;  /* Mask that indicates which LNA state */

  uint8   rx_meas_count = 0;             /* Total measurement result count      */
  uint16  rx_meas_size_count = 0;        /* Total measurement result size       */

  uint32  rx_flag;                       /* Rx device(s) for measurement        */
  byte    lna_range = 0;                 /* LNA range                           */

  prof_start_time = timetick_get();

  /* Check for NULL ptr(s) */
  if((rx_meas == NULL) || (rx_result == NULL) || (rx_result->rx_buffer == NULL))
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_measure_rx: NULL ptr(s)!");
    return FALSE;
  }

  /* Get LNA gain state for storing result(s) */
  cal_action_mask = rf_extract_first_one_bit_mask( (uint32)rx_meas->cal_action );

  /* Check for valid cal action */
  if( (cal_action_mask != (uint32)rx_meas->cal_action) ||  /* Check if there is only one action */
      (cal_action_mask > (uint32)RX_ACTION_LNA5_CAL)       /* Check if LNA state is valid */
    )
  {
    FTM_MSG_2(FTM_ERROR, "ftm_wcdma_calv3_sweep_measure_rx: Invalid cal_action: cal_action_mask = %d, cal_action = %d",
              cal_action_mask,rx_meas->cal_action);
    return FALSE;
  }

  /* Get the Rx device information */
  rx_flag = (uint32)rx_meas->device_flag;

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

  /* Check for potential buffer overflow */
  if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
  {
    rx_meas_count++;
  }
  if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
  {
    rx_meas_count++;
  }

  if( rx_result->max_rx_buffer_len < 
      (rx_meas_size_count + rx_meas_count*sizeof(ftm_calibration_rx_meas_data_type)) )
  {
    success_flag = FALSE;
  }

  /* If no potential buffer overflow detected then proceed with Rx meas */
  else
  {
    if( ftm_wcdma_rxcal_opt_enabled() )
    {
      if(IS_FTM_IN_WCAL_MODE())
      {

        ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr(rx_flag,
                                                 lna_range,
                                                 (word)rx_meas->exp_rxagc,
                                                 (ftm_calibration_rx_meas_data_type*)(rx_result->rx_buffer)
                           );
      }
      else
      {
      ftm_wcdma_calv3_get_dvga_lna_gain_offset(rx_flag,
                                               lna_range,
                                               (word)rx_meas->exp_rxagc,
                                               (ftm_calibration_rx_meas_data_type*)(rx_result->rx_buffer)
                         );

      }
    }else
    {

    ftm_wcdma_calv3_get_dvga_lna_gain_offset(rx_flag,
                                             lna_range,
                                             (word)rx_meas->exp_rxagc,
                                             (ftm_calibration_rx_meas_data_type*)(rx_result->rx_buffer)
											 );
    }
  }

  /* If measurements go through then update meas struct */
  if(success_flag)
  {
    /* Update the total number of rx measurements taken during this segment. */
    rx_result->num_of_rx_reads = rx_meas_count;
  }
  else
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_measure_rx: Measurement buffer overflow!");
  }

  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_1(FTM_HIGH, 
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_measure_rx: elapse time %d",
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return success_flag;
  
} /* ftm_wcdma_calv3_sweep_measure_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM_WCDMA_CALV3_SWEEP_MEASURE_TX
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
boolean ftm_wcdma_calv3_sweep_measure_tx
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
  rfdevice_wcdma_hdet_read_type hdet_output={0};

  prof_start_time = timetick_get();

  /* Check for NULL ptr(s) */
  if( (hdet_meas == NULL) || (tx_result == NULL) ||(tx_result->tx_buffer == NULL) )
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_measure_tx: NULL input ptr(s)!");
    return FALSE;
  }

  /* Check if device of interest is valid */
  if(hdet_meas->device_flag != DEVICE_FLAG_0)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_calv3_sweep_measure_tx: Invalid Device: %d!",hdet_meas->device_flag);
    return FALSE;
  }

  /* Check for potential buffer overflow */
  if ( tx_result->max_tx_buffer_len < sizeof(ftm_calibration_tx_meas_data_type) )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_wcdma_calv3_sweep_measure_tx: Insufficient buffer size %d!",tx_result->max_tx_buffer_len);
    return FALSE;
  }  

  /* Check for div-by-0 */
  if(hdet_meas->hdet_sample == 0)
    {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_measure_tx: hdet_sample input is 0!");
    return FALSE;
  }
  /* Get HDET measurements */
  for(hdet_idx = 0; hdet_idx < hdet_meas->hdet_sample; hdet_idx++)
    {
      /* Do up to 3 tries per HDET read */
      local_read_count = 0;
      do
      {
       rfdevice_hdet_wcdma_read_hdet( RFM_DEVICE_0,
                                      ftm_curr_wcdma_mode,
                                         FALSE, 
                                      &hdet_output );

       if ( hdet_output.hdet_quality == TRUE )
       {
         temp_hdet_val = hdet_output.hdet_reading;
       }
       else
       {
         temp_hdet_val = -1;
       }

        local_read_count++;
    }while ((temp_hdet_val == 0xFFFF)
               && (local_read_count < 3));

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

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_4(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_measure_tx: tx_flag = %d, HDET samples = %d, avg_hdet = %d, elapse time %d", 
              hdet_meas->device_flag,
              hdet_meas->hdet_sample,
              hdet_accm,
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return success_flag;

} /* ftm_wcdma_calv3_sweep_measure_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_SWEEP_TEAR_DOWN_RADIO
  Tear Down Radio for calibration sweep
 
  @details
  This function will tear down all radios that are tuned to WCDMA mode to
  sleep state.
 
 @return
 boolean that indicates whether there had been an error or not
*/
boolean ftm_wcdma_calv3_sweep_tear_down_radio(void)
{
  rfm_wcdma_rx_sleep_params sleep_params;
  prof_start_time = timetick_get();

  rfwcdma_mc_set_use_fb_path_flag(FALSE); /* Clear FB path flag */
  ftm_wcdma_calv3_set_cal_flag(FALSE);    /* Clear wcdma_calv3 flag */
  ftm_wcdma_set_tx(RFM_DEVICE_0,FALSE);                /* Disable Tx */

  /* Reset PDM to lowest */
  ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_wcdma_calv3_pdm_limits.min_pdm+1);

  /* Sleep Rx on Dev2,3 */
  ftm_wcdma_set_secondary_chain(RFM_DEVICE_3, FALSE);
  memset(&sleep_params, 0, sizeof(rfm_wcdma_rx_sleep_params)); 
  sleep_params.api_params[RFM_DEVICE_2].is_valid = TRUE;
  sleep_params.api_params[RFM_DEVICE_2].rxlm_handle = ftm_wcdma_rxlm_buffer[RFM_DEVICE_2].buf_idx;
  sleep_params.api_params[RFM_DEVICE_2].skip_agc_disable = FALSE; 
  rfm_wcdma_rx_sleep(&sleep_params,NULL,NULL);

  /* Sleep Rx on Dev0,1 */
  ftm_wcdma_set_secondary_chain(RFM_DEVICE_1, FALSE);
  memset(&sleep_params, 0, sizeof(rfm_wcdma_rx_sleep_params)); 
  sleep_params.api_params[RFM_DEVICE_0].is_valid = TRUE;
  sleep_params.api_params[RFM_DEVICE_0].rxlm_handle = ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx;
  sleep_params.api_params[RFM_DEVICE_0].skip_agc_disable = FALSE; 
  rfm_wcdma_rx_sleep(&sleep_params,NULL,NULL);

  /* Tear down the data module */
  ftm_cal_data_teardown_data_module();
  /* Tear down the delay table */
  ftm_cal_data_teardown_delay_map();
  /* Tear down the pin table */
  ftm_cal_data_teardown_pin_map();

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_1(FTM_HIGH, "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_tear_down_radio: elapse time %d",
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return TRUE;
} /* ftm_wcdma_calv3_sweep_tear_down_radio */

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
boolean ftm_wcdma_calv3_sweep_radio_setup
(
  ftm_cal_radio_setup_parameter_type const *radio_setup
)
{
  /* Config radio request */
  ftm_cal_radio_setup_req_type rs_req;

  /* ToDo: Get the dbg flag from common data structure */
  ftm_wcdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  prof_start_time = timetick_get();

  ftm_wcdma_calv3_set_cal_flag(TRUE);

  /* Check for NULL ptr */
  if(radio_setup == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_radio_setup: NULL radio_setup ptr!");
    return FALSE;
  } /* if ( radio_setup == NULL ) */
  
  /* Create Request Packet for radio setup */
  rs_req.cmd = DIAG_SUBSYS_CMD_F;
  rs_req.sub_sys = FTM;
  rs_req.mode = FTM_WCDMA_C;
  rs_req.cdma_cmd = FTM_INT_CALV3_RADIO_SETUP;

  memscpy( &(rs_req.rs_param), sizeof(ftm_cal_radio_setup_parameter_type), radio_setup, 
           sizeof(ftm_cal_radio_setup_parameter_type) );

  /* Queue up the FTM command */
  ftm_common_send_radio_command( &rs_req,
                                 sizeof(ftm_cal_radio_setup_req_type) );

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_2(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_radio_setup: mode = %d, elapse time: %d", 
              rs_req.mode,
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return TRUE;
} /* ftm_wcdma_calv3_sweep_radio_setup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Feedback Setup
 
  @details
  This function will be called from cal v3 common framework to perform
  feedback setup operation.
 
  @param feedback_setup
  Pointer to the data structure containing radio setup unpacked parameter
 
  @return
  Flag indicating the status of feedback setup operation
*/
boolean
ftm_wcdma_calv3_sweep_feedback_setup
(
  ftm_cal_feedback_setup_parameter_type const *feedback_setup
)
{
   /* Config radio request */
  ftm_cal_radio_setup_req_type rs_req;

  /* ToDo: Get the dbg flag from common data structure */
  ftm_wcdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  prof_start_time = timetick_get();

  /* Check for NULL ptr */
  if(feedback_setup == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_feedback_setup: NULL radio_setup ptr!");
    return FALSE;
  } /* if ( feedback_setup == NULL ) */
  
  /* Create Request Packet for radio setup */
  rs_req.cmd = DIAG_SUBSYS_CMD_F;
  rs_req.sub_sys = FTM;
  rs_req.mode = FTM_WCDMA_C;
  rs_req.cdma_cmd = FTM_INT_CALV3_FEEDBACK_SETUP;

  memscpy( &(rs_req.rs_param), sizeof(ftm_cal_feedback_setup_parameter_type), feedback_setup, 
           sizeof(ftm_cal_feedback_setup_parameter_type) );

    /* Queue up the FTM command */
    ftm_common_send_radio_command( &rs_req,
                                   sizeof(ftm_cal_feedback_setup_req_type) );

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_2(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_feedback_setup: mode = %d, elapse time: %d", 
              rs_req.mode,
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  return TRUE;
}/* ftm_wcdma_calv3_sweep_feedback_setup */

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
ftm_wcdma_calv3_sweep_xpt_iq_capture
(
  ftm_cal_xpt_instr_payload_iq_capture_type const *payload
)
{
   /* Config XPT IQ capture request */
  ftm_cal_xpt_instr_req_type xpt_req;

  /* ToDo: Get the dbg flag from common data structure */
  ftm_wcdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  prof_start_time = timetick_get();

  /* Check for NULL ptr */
  if(payload == NULL)
  {
    FTM_MSG(FTM_ERROR, "ftm_wcdma_calv3_sweep_xpt_iq_capture: NULL payload ptr!");
    return FALSE;
  } /* if ( payload == NULL ) */
  
  /* Create Request Packet for radio setup */
  xpt_req.cmd = DIAG_SUBSYS_CMD_F;
  xpt_req.sub_sys = FTM;
  xpt_req.mode = FTM_WCDMA_C;
  xpt_req.cdma_cmd = FTM_INT_CALV3_XPT_IQ_CAPTURE;

  memscpy( &(xpt_req.xpt_param), 
           sizeof(ftm_cal_xpt_instr_payload_iq_capture_type), 
           payload, 
           sizeof(ftm_cal_xpt_instr_payload_iq_capture_type) );

  /* Queue up the FTM command */
  ftm_common_send_radio_command( &xpt_req,
                                 sizeof(ftm_cal_xpt_instr_req_type) );

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_2(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_xpt_iq_capture: cmd = %d, elapse time: %d", 
              xpt_req.cdma_cmd,
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
}

  return TRUE;
}/* ftm_wcdma_calv3_sweep_xpt_iq_capture */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Feedback Setup
 
  @details
  This function will be called from cal v3 common framework to perform
  feedback setup operation.
 
  @param feedback_setup
  Pointer to the data structure containing radio setup unpacked parameter
 
  @return
  ftm_rsp_pkt_type
*/
ftm_rsp_pkt_type
ftm_wcdma_calv3_feedback_setup(void *ftm_req_data)
{
  ftm_cal_feedback_setup_parameter_type const *feedback_setup = NULL;
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */

  /* Parameter validation */
  if ( ftm_req_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_feedback_setup: NULL argument" );
    return ftm_rsp_data;
  } /* if ( ftm_req_data == NULL ) */

  /* Get the pointer to radio setup param */
  feedback_setup = &(((ftm_cal_feedback_setup_req_type *)ftm_req_data)->fb_param);

  ftm_wcdma_feedback_setup( feedback_setup->primary_device,
                            feedback_setup->feedback_device,
                            feedback_setup->feedback_setup_action,
                            feedback_setup->configure_wtr.fb_state,
                            (boolean)feedback_setup->configure_wtr.enable_mode,
                            (boolean)feedback_setup->configure_rxlm_for_fbagc.enable_mode,
                            (boolean)feedback_setup->configure_rxlm_for_fbagc.enable_mode );

  return ftm_rsp_data;
}

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
ftm_wcdma_calv3_apply_pin_alignment
(
  rfm_device_enum_type device,
  uint16 ref_chan,
  ftm_cal_data_dpd_map_row_type *input_override_row,
  ftm_cal_data_dpd_map_row_type *output_override_row
)
{
  /* Pointer to NV data */
  rfnv_wcdma_nv_tbl_type *nv_tbl;

  /* Pin compensation */
  uint16 ref_lin_therm_adc;
  int16 pin_comp_offset;
  int16 pout_comp_offset;
  tx_lin_vs_temp_vs_freq_comp_param_type temp_vs_freq_params;
  uint32 curr_chan;
  boolean pin_offset_calibrated;

  /* Obtain NV information */
  nv_tbl = rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_convert_band_rfcom_to_rfnv(
                                                        ftm_curr_wcdma_mode) );
  curr_chan = ftm_common_data_get_curr_channel();

  /* Get the reference linearizer Therm ADC value from data module */
  ftm_cal_data_get_therm_adc( ref_chan, &ref_lin_therm_adc );

  /* Initialize input data for the function to compute offset */
  temp_vs_freq_params.freq_params.tx_freq = 
    rfwcdma_core_get_freq_from_ul_arfcn( rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx );
  temp_vs_freq_params.freq_params.ref_lin_tx_freq = 
    rfwcdma_core_get_freq_from_ul_arfcn( ref_chan );

  temp_vs_freq_params.temp_params.temp_read = 
    rfwcdma_core_temp_comp_get_scaled_therm_read( ref_lin_therm_adc );
  temp_vs_freq_params.temp_params.ref_lin_temp_adc = 
    rfwcdma_core_temp_comp_get_scaled_therm_read( ref_lin_therm_adc );

  temp_vs_freq_params.pa_state = input_override_row->curr_tx_override.pa_state;
  temp_vs_freq_params.ref_xpt_pin_offset = 0;

  if( input_override_row->curr_tx_override.xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET )
  {
   temp_vs_freq_params.ref_xpt_pin_offset = 
     (int32)rfwcdma_core_xpt_get_cfg_param( ftm_curr_wcdma_mode, XPT_CFG_CMN_REF_ET_PIN_OFFSET );
  }
  else if( input_override_row->curr_tx_override.xpt_mode  == RFCOMMON_MDSP_XPT_MODE_EPT )
  {
   temp_vs_freq_params.ref_xpt_pin_offset = 
     (int32)rfwcdma_core_xpt_get_cfg_param( ftm_curr_wcdma_mode, XPT_CFG_CMN_REF_EPT_PIN_OFFSET );
  }

  pin_offset_calibrated = rfcommon_core_txlin_has_pin_cal_offset(&(nv_tbl->lin_temp_comp_data),
	  input_override_row->curr_tx_override.pa_state);

  /*Apply Pin cal during FComp sweep if Pin NVs exist*/
  
	if ( pin_offset_calibrated == TRUE )
	{
	  pin_comp_offset = ftm_cal_data_get_pin_cal_offset(ref_chan, &(nv_tbl->lin_temp_comp_data),
		input_override_row->curr_tx_override.pa_state, 	 
		(uint32)curr_chan );
	   FTM_MSG_2( FTM_HIGH, "ftm_wcdma_calv3_apply_pin_alignment: "
                       "Pin comp offset %d, Chan %d",pin_comp_offset,curr_chan);
	}
	else
	{

    temp_vs_freq_params.auto_pin_enable = rfcommon_autopin_is_enabled(RFM_IMT_MODE);

  /* Calculate Pin alignment */
  rfcommon_core_txlin_get_temp_compensation( 
         RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3,
         &pin_comp_offset,
         &pout_comp_offset,
         &temp_vs_freq_params,
         &nv_tbl->lin_temp_comp_data );
	}
	
  /* Apply Pin alignment to the override row */
  ftm_cal_data_apply_pin_alignment( ref_chan, 
                                    input_override_row->curr_tx_override.pa_state, 
                                    input_override_row,
                                    output_override_row,
                                    pin_comp_offset );

  FTM_MSG_5( FTM_HIGH, "ftm_wcdma_calv3_apply_pin_alignment: "
                       "Band %d, Ref Chan %d, Ref ADC %d, Curr Chan %d, PA State %d",
             ftm_curr_wcdma_mode, ref_chan, ref_lin_therm_adc, 
             rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx, 
             input_override_row->curr_tx_override.pa_state );
  FTM_MSG_3( FTM_HIGH, "ftm_wcdma_calv3_apply_pin_alignment: "
                       "Pin Comp %d dB10, Input RGI %d, Input IQ Gain %d",
             pin_comp_offset, input_override_row->curr_tx_override.rgi,
             input_override_row->curr_tx_override.iq_gain );
  FTM_MSG_2( FTM_HIGH, "ftm_wcdma_calv3_apply_pin_alignment: "
                       "Output RGI %d, Output IQ Gain %d",
             output_override_row->curr_tx_override.rgi,
             output_override_row->curr_tx_override.iq_gain );

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Tx Override
 
  @details
  This function will be called from cal v3 common framework to perform
  Tx Override operation. Tx Override involves operation such as overriding
  several RF and MSM Tx parameters such as PA State, RGI, DPD, PA Bias,
  quiscent current and more.
 
  @param tx_override
  Pointer to the data structure containing Tx Override unpacked parameter
 
  @return
  Flag indicating the status of Tx Override operation
*/
boolean
ftm_wcdma_calv3_sweep_tx_override
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
  ftm_cal_data_dpd_map_row_type override_row;
  ftm_cal_data_dpd_map_row_type override_row_before_pin;
  rfcom_band_type_u rfm_band;

  memset(&override_row,0,sizeof(ftm_cal_data_dpd_map_row_type));

  /* Radio setup must be done before txagc override */
  ASSERT(ftm_wcdma_calv3_radio_setup_done);

  /* ToDo: Get the dbg flag from common data structure */
  ftm_wcdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  prof_start_time = timetick_get();

  /* Init value to 0 */
  memset( &ovr_params, 0x0, sizeof(ftm_xpt_txagc_override_type) );
  memset( &curr_ovr_params, 0, sizeof(rfcommon_core_xpt_tx_override_type) );

  /* Parameter validation */
  if ( tx_override == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_sweep_tx_override: NULL argument" );
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
          ovr_params.rgi = FTM_COMMON_XPT_TX_OVR_RGI_INVALID;
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
          ovr_params.smps_bias = FTM_COMMON_XPT_TX_OVR_SMPS_BIAS_INVALID;
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
          ovr_params.pa_state = FTM_COMMON_XPT_TX_OVR_PA_STATE_INVALID;
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
          ovr_params.pa_curr = FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID;
          curr_ovr_params.pa_curr = FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID;
        } /* if !( ( param_mask & ( 1 << TX_OVR_QUISCENT_CURRENT ) ) != 0 ) */

        if ( ( param_mask & ( 1 << TX_OVR_DELAY ) ) != 0 )
        {
          ovr_params.delay_action_type = 
            tx_override->tx_param[device].power_ovr.individual_param.delay.ovr_action;
          ovr_params.delay = 
            tx_override->tx_param[device].power_ovr.individual_param.delay.delay_val;
          curr_ovr_params.delay_action_type = 
            tx_override->tx_param[device].power_ovr.individual_param.delay.ovr_action;
          curr_ovr_params.et_delay = 
            tx_override->tx_param[device].power_ovr.individual_param.delay.delay_val;
        }
        else
        {
          ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_NV;
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
        memscpy( (void*)&override_row, sizeof(ftm_cal_data_dpd_map_row_type),
                (void*)&override_row_before_pin, 
                sizeof(ftm_cal_data_dpd_map_row_type) );

        FTM_MSG_2( FTM_HIGH,"Ovr by power current row delay action %d, delay val %d", 
                   override_row.curr_tx_override.delay_action_type,
                   override_row.curr_tx_override.et_delay );

        /* If the Pin alignment flag is set, then apply Pin 
        alignment to the override values before proceeding. If Pin is 
        not required, the values obtained from the data module will 
        be used */
        if ( tx_override->flag == 
                              FTM_CALIBRATION_TX_OVR_APPLY_PIN_ALIGNMENT )
        {
          ftm_wcdma_calv3_apply_pin_alignment( 
                 device,
                 tx_override->
                        tx_param[device].power_ovr.abs_power.ref_channel,
                 &override_row_before_pin,
                 &override_row );

          if ( tx_override->debug_mode_en == TRUE )
          {
            FTM_MSG_2( FTM_MED, "ftm_wcdma_calv3_sweep_tx_override: "
                                "Applied Pin alignment to Tx override for "
                                "Power %d XPT Mode %d",
                       tx_override->
                        tx_param[device].power_ovr.abs_power.power_dbm10,
                       override_row_before_pin.curr_tx_override.xpt_mode);
          }
        }/* if( tx_override->flag == 
                              FTM_CALIBRATION_TX_OVR_APPLY_PIN_ALIGNMENT */

        /* Verify we found a row of data */
        if(ret_val == FALSE)
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,
             "ftm_wcdma_calv3_sweep_tx_override: Could not find power %d in data module",
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
        FTM_MSG_1( FTM_ERROR, "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_tx_override: "
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
      ret_val &= ftm_wcdma_xpt_set_txagc_override_params( device,
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
      rfm_band.wcdma_band = ftm_curr_wcdma_mode;
      ret_val &= rfm_ftm_apply_xpt_tx_agc_override( RFM_IMT_MODE, 
                                                    device,
                                                    rfm_band );
    }
    else
    {
    ret_val &= ftm_wcdma_xpt_override_txagc(device,
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
     if ( ftm_wcdma_calv3_dbg_msg_flag )
     {
       FTM_MSG_5( FTM_HIGH, "ftm_wcdma_calv3_sweep_tx_override: Dev %d "
                  "Tx Param 0x%x, PA %d, RGI %d, Bias %d", 
                  device, 
                  param_mask,
                  curr_ovr_params.pa_state,
                  curr_ovr_params.rgi,
                  curr_ovr_params.papm_bias );
     }
  } /* while ( dev_mask != 0 ) */

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_3( FTM_HIGH, "WCDMA CalV3 DBG - ftm_wcdma_calv3_sweep_tx_override: "
               "[Status %d] [DevMask %d] [Elapse time: %d]", 
               ret_val, tx_override->device_mask, timetick_get_elapsed( prof_start_time, T_USEC ) );
  }

  return ret_val;

} /* ftm_wcdma_calv3_sweep_tx_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_CONFIGURE_RADIO
  Configures radio for calibration sweep.
 
  @details
  Configures RF based on passed in parameters to prepare for sweep.
  ftm_wcdma_calv3_sweep_configure_radio (which executes in ISR context)
  populates the required fields and pushes cmd to ftm. ftm will then
  pick up the cmd and execute this function in ftm context.

  @param ftm_req_data
  Pointer to ftm_wcdma_cal_sweep_configure_radio_req_type:
     mode:
       RF mode
     channel
       RF channel
     device_master_mask
       8-bit parameter
       For W we only use the first bit which indicates Tx device 0
     device_slave
       32-bit parameter
       Each master bit has usage of 4 bits of the slave_device.
       If the 4-bit val is 0xF then it is don't-care
       For W, we only use the last four bits: 0x1 - DRx enabled, 0xF - disabled 
 
 @return
 ftm_rsp_pkt_type
*/
ftm_rsp_pkt_type ftm_wcdma_calv3_configure_radio(void *ftm_req_data)
{
  boolean drx_enable_flag = FALSE; /* Enable DRx indicator */

  /* Create response Packets */
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};

  /* Pointer to config information */
  ftm_wcdma_cal_sweep_configure_radio_req_type* header = NULL;

  prof_start_time = timetick_get();

  /* Get Request Packet */
  header = (ftm_wcdma_cal_sweep_configure_radio_req_type*)ftm_req_data;

  /* Print debug msg if enabled: Function start time tick */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_4(FTM_HIGH,
              "WCDMA CAlV3 DBG - ftm_wcdma_calv3_configure_radio: mode %d, band, %d chan %d, device_slave %d",
              header->mode,
              header->band,
              header->channel,
              header->device_slave
              );
  }

  /* Check if DRx is required */
  if((header->device_slave & FTM_WCDMA_CALV3_SLAVE_MASK) == FTM_WCDMA_CALV3_SLAVE_ENABLE_VAL )
  {
    drx_enable_flag = TRUE;
  }

  /* Tune Band and Chan */
  ftm_wcdma_calv3_tune_band_chan((ftm_mode_id_type)header->band,
                                 header->channel,
                                 header->dc_channel_offset,
                                 drx_enable_flag
                                 );

  /* Set Lowest PA State  */
  ftm_wcdma_set_pa_range(rfwcdma_core_util_get_pa_range_from_nv(0));
  
  /* Get PDM limit and set PDM to lowest + 1 */
  /*! @todo Need to get the rfm_device info from the caller function */
  rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0,ftm_curr_wcdma_mode,RFDEVICE_GET_TX_AGC_LIMITS, &ftm_wcdma_calv3_pdm_limits);
  ftm_wcdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_wcdma_calv3_pdm_limits.min_pdm+1);
  
  /* Set TX ON */
  ftm_wcdma_set_tx(RFM_DEVICE_0,TRUE);

  /* Populate a "No Response" packet */
  ftm_rsp_data.cmd = FTM_RSP_NO_RSP;
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_3(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_configure_radio, start_tick = %d, stop_tick = %d, elapse = %d us", 
              prof_start_time,
              timetick_get(),
              timetick_get_elapsed( prof_start_time, T_USEC ));
  }

  return ftm_rsp_data;

} /* ftm_wcdma_calv3_configure_radio */

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
ftm_rsp_pkt_type ftm_wcdma_calv3_radio_setup(void *ftm_req_data)
{
  rfm_device_enum_type mdev; /* Master device */
  rfm_device_enum_type sdev; /* Slave device */
  ftm_cal_radio_setup_mdevice_state_type mdev_state; /* Master device State */
  ftm_cal_radio_setup_sdevice_state_type sdev_state; /* Slave device State */
  rfcom_wcdma_band_type band; /* WCDMA Band */
  uint8 num_of_chan;
  uint16 tune_ul_channel_list[RFCOM_MAX_CARRIERS] = {0,0,0}; // uplink channel list
  boolean ret_val = TRUE; /* Status of radio setup */
  ftm_cal_radio_setup_parameter_type const *radio_setup = NULL; /* RS param */
  /* Populate a "No Response" packet */
  ftm_rsp_pkt_type ftm_rsp_data = {FTM_RSP_NO_RSP, 0, NULL, FALSE};
  ftm_xpt_txagc_override_type ovr_params;  
  ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents ftmdiag_diag_free() from being called */
  ftm_wcdma_calv3_radio_setup_done = FALSE; /* reset the radio_setup_done flag in the beginning */
  /* ToDo: Get the dbg flag from common data structure */
  ftm_wcdma_calv3_dbg_msg_flag = (ftm_common_data_get_cal_v3_ptr())->cal_config.debug_mode_en;

  prof_start_time = timetick_get();    
    
  /* Parameter validation */
  if ( ftm_req_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_radio_setup: NULL argument" );
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
  band = ftm_wcdma_calv4_convert_band(radio_setup->band);
  
  if (radio_setup->flag == FTM_CALIBRATION_NBEE_RXCAL_ENABLED )
  {
   FTM_MSG_1(FTM_HIGH, "WCDMA CalV3 DBG - ftm_wcdma_calv3_radio_setup NBEE enabled %d", radio_setup->flag );          
   ftm_wcdma_enable_NBEE();
  }
  else
  {
	ftm_wcdma_disable_NBEE();
	FTM_MSG_1(FTM_HIGH, "WCDMA CalV3 DBG - ftm_wcdma_calv3_radio_setup NBEE DISABLED %d", radio_setup->flag );
  }  
  if(num_of_chan==1)
  {
    tune_ul_channel_list[0] = radio_setup->chan_list[0].channel;
  }
  else 
  {
    if(num_of_chan==2)
    {
    tune_ul_channel_list[0] = radio_setup->chan_list[0].channel;
    tune_ul_channel_list[1] = radio_setup->chan_list[1].channel;
    }
    else if(num_of_chan==3)
    {
    tune_ul_channel_list[0] = radio_setup->chan_list[0].channel;
    tune_ul_channel_list[1] = radio_setup->chan_list[1].channel;
    tune_ul_channel_list[2] = radio_setup->chan_list[2].channel;
    }
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_wcdma_calv3_radio_setup: Unsupported"
                          " number of channels %d", num_of_chan);
      return ftm_rsp_data;
    }
  }

  /*----------------------------------------------------------------------*/
  /* MASTER_DEVICE_DISABLED */
  /*----------------------------------------------------------------------*/

  if ( mdev_state == MASTER_DEVICE_DISABLED )
  {
    /* Tear Down RF */ 
    if( mdev < RFM_DEVICE_4  )
    {
      ftm_rfmode_exit(mdev, FTM_STATE_PARK);
      /* Tear down the data module when master device disabled */
      ftm_cal_data_teardown_data_module();
      /* Tear down the ET delay table when master device disabled */
      ftm_cal_data_teardown_delay_map();
      /* Tear down the pin table when master device disabled */
      ftm_cal_data_teardown_pin_map();
    }
    else
    {
     FTM_MSG(FTM_ERROR, "Wrong device index");
    }

  } /* if ( radio_setup->mdevice_state == MASTER_DEVICE_DISABLED ) */

  /* Clear the DPD map, pin map and ET delay table if we are starting calibration */
  if ( mdev_state == MASTER_DEVICE_RXTX  )
  {
    ftm_cal_data_teardown_data_module();
    ftm_cal_data_teardown_delay_map();
    ftm_cal_data_teardown_pin_map();
  }
  /*----------------------------------------------------------------------*/
  /* MASTER_DEVICE_RX || MASTER_DEVICE_RXTX */
  /*----------------------------------------------------------------------*/

  if ( ( mdev_state == MASTER_DEVICE_RX ) ||
       ( mdev_state == MASTER_DEVICE_RXTX ) )
  {
    /* Do enter for RX or RXTX cmd */
    if( mdev < RFM_DEVICE_4  )
    {
      if(mdev == RFM_DEVICE_2)
      {
        /* Put Transceiever Rx and Tx to low power mode */
        rfdevice_wcdma_rx_disable(RFM_DEVICE_0,
                                  rfwcdma_mc_state.car_path_state[0][0].curr_band_rx,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  NULL);
        rfdevice_wcdma_rx_disable(RFM_DEVICE_1,
                                  rfwcdma_mc_state.car_path_state[1][0].curr_band_rx,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  NULL);
      }
      /* Enter FTM to WCDMA Mode */
      ftm_rfmode_enter( mdev, FTM_STATE_WCDMA );
      /* Enter WCDMA Mode */
      rfm_enter_mode( mdev, RFCOM_WCDMA_MODE, 
                      NULL, NULL, ftm_wcdma_rxlm_buffer[mdev].buf_idx);
      rfm_enable_rx(mdev, NULL, 0); 
    }
    else
    {
      FTM_MSG(FTM_ERROR, "Wrong device index");
    }

    ftm_wcdma_calv3_set_current_mode(band);

    // Check if TX on then turn off TX
    if(ftm_wcdma_tx_on_status == TRUE)
    {
      ftm_wcdma_set_tx(RFM_DEVICE_0,FALSE);
    }

    if(num_of_chan == 1)
    {
      /* Rx PLL */
      if( mdev < RFM_DEVICE_4 )
      {
        ftm_wcdma_tune_to_chan( mdev, tune_ul_channel_list[0]);
      }
      else
      {
         FTM_MSG(FTM_ERROR, "Wrong device index");
      }
    }
    if((num_of_chan == 2)||(num_of_chan == 3))
    {
      /* Rx PLL */
      if( mdev < RFM_DEVICE_4 )
      {
       ftm_wcdma_tune_to_multi_chan( mdev, tune_ul_channel_list);
      }
      else
      {
         FTM_MSG(FTM_ERROR, "Wrong device index");
      }

    }

    if(mdev_state == MASTER_DEVICE_RXTX)
    {
      /* Perform Tx Setup, if the state requested is RxTx. */
      /* Set TX ON */
      ftm_wcdma_set_tx(mdev, TRUE);        
    }
  } /* if ( ( mdev_state == MASTER_DEVICE_RX ) ||
          ( mdev_state == MASTER_DEVICE_RXTX ) ) */

  /*----------------------------------------------------------------------*/
  /* MASTER_DEVICE_RETUNE */
  /*----------------------------------------------------------------------*/
  if ( mdev_state == MASTER_DEVICE_RETUNE )
  {
    /* Skip enter_mode if already done so */
    if (   (ftm_current_rf_mode != FTM_DB_RF_WCDMA_IMT)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_1900MHZ_A)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_1900MHZ_B)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC3)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC4)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_800MHZ)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC8)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC9)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC11)
        && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC19))
    {
      /* Enter FTM to WCDMA Mode */
      if( mdev < RFM_DEVICE_4 )
      {
        ftm_rfmode_enter( mdev, FTM_STATE_WCDMA );
        /* Enter WCDMA Mode */
        rfm_enter_mode( mdev, RFCOM_WCDMA_MODE, 
                        NULL, NULL, ftm_wcdma_rxlm_buffer[mdev].buf_idx);
        rfm_enable_rx(mdev, NULL, 0); 
      }
      else
      {
         FTM_MSG(FTM_ERROR, "Wrong device index");
      }
    }

    /* Reset PDM to lowest */ 
    if(ftm_wcdma_tx_on_status == TRUE)
    {
      ovr_params.device = RFM_DEVICE_0;
      ovr_params.xpt_mode = RFCOMMON_MDSP_XPT_MODE_APT;
      ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
      ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
      ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
      ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID; 
      ovr_params.rgi = (int16)ftm_wcdma_calv3_pdm_limits.min_pdm+1;
      ovr_params.smps_bias = 2000;
      ovr_params.pa_state = FTM_COMMON_XPT_TX_OVR_PA_STATE_INVALID;
      ovr_params.pa_curr = FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID;
      ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
      ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
      ftm_wcdma_xpt_override_rf_settings( &ovr_params );
    }

    ftm_wcdma_calv3_set_current_mode(band);

    /*----------------------------------------------------------------------*/
    /* SLAVE_DEVICE_RXDIV */
    /*----------------------------------------------------------------------*/
    if( sdev_state == SLAVE_DEVICE_RXDIV )
    {
      if(ftm_wcdma_div_status[sdev] == FALSE)
      {
        /* Call enter mode directly to avoid retune */
        ftm_rfmode_enter(sdev, FTM_STATE_WCDMA);
        rfm_enter_mode(sdev, 
                       RFCOM_WCDMA_MODE, 
                       NULL, NULL, 
                       ftm_wcdma_rxlm_buffer[sdev].buf_idx);
        ftm_wcdma_div_status[sdev] = TRUE;
        /* Print debug msg if enabled */
        if(ftm_wcdma_calv3_dbg_msg_flag)
        {
           FTM_MSG_1(FTM_HIGH,"WCDMA CalV3 DBG - ftm_wcdma_calv3_radio_setup:"
                              " enabled DRx on device %d", sdev);
        }
      }
    }

    /*----------------------------------------------------------------------*/
    /* SLAVE_DEVICE_RXDIV */
    /*----------------------------------------------------------------------*/
    if( sdev_state == SLAVE_DEVICE_DISABLED )
    {
      if(ftm_wcdma_div_status[sdev] == TRUE)
      {
        ftm_wcdma_set_secondary_chain(sdev, FALSE);
      }
    }

    if(num_of_chan == 1)
    {
      /* Rx PLL */
      if( mdev < RFM_DEVICE_4 )
      {
        ftm_wcdma_tune_to_chan( mdev, tune_ul_channel_list[0]);
      }
      else
      {
        FTM_MSG(FTM_ERROR, "Wrong device index");
      }
    }
    if(num_of_chan == 2)
    {
      /* Rx PLL */
      if( mdev < RFM_DEVICE_4 )
      {
        ftm_wcdma_tune_to_multi_chan( mdev, tune_ul_channel_list);
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
    if(ftm_wcdma_div_status[sdev] == FALSE)
    {
      ftm_wcdma_set_secondary_chain(sdev, TRUE);
    }
  } 

  /*----------------------------------------------------------------------*/
  /* SLAVE_DEVICE_RXDIV */
  /*----------------------------------------------------------------------*/
  if( sdev_state == SLAVE_DEVICE_DISABLED )
  {
    if(ftm_wcdma_div_status[sdev] == TRUE)
    {
      ftm_wcdma_set_secondary_chain(sdev, FALSE);
    }
  } 

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_9( FTM_HIGH, "WCDMA CalV3 DBG - ftm_wcdma_calv3_radio_setup: [Status %d] "
               "Dev[%d,%d] State[%d,%d] Num[%d] Band [%d] Channel[%d %d]", 
               ret_val, mdev, sdev, mdev_state, sdev_state, num_of_chan, band, 
               tune_ul_channel_list[0], tune_ul_channel_list[1]);
    FTM_MSG_1(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_radio_setup: elapse time: %d", 
              timetick_get_elapsed( prof_start_time, T_USEC )
              );
  }

  ftm_wcdma_calv3_radio_setup_done = TRUE; /* set the radio_setup_done flag */
  
  /* Return Status */
  return ftm_rsp_data;

} /* ftm_wcdma_calv3_radio_setup */


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
ftm_wcdma_calv3_rx_measure
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
  int16 lna_offset[2] = {0,0}; /* LNA Offset Value */

  /* Parameter validation */
  if ( ( rx_measure == NULL ) || ( cal_result == NULL ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_rx_measure: NULL argument" );
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
      /* RX_MEAS_LNA_OFFSET */
   if( ( rx_measure->rx_meas_param[device].rx_meas_action & 
             ( 1 << RX_MEAS_LNA_OFFSET ) ) != 0 )
  {        
   if(ftm_wcdma_rxcal_opt_enabled())
   {
    if ( IS_FTM_IN_WCAL_MODE() == TRUE )
    {
        ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr
                            (device+1, // rx_flag start from 1
                             rx_measure->rx_meas_param[device].lna_offset.lna_state,
                             (word)rx_measure->rx_meas_param[device].lna_offset.expected_exagc,
                             (ftm_calibration_rx_meas_data_type*)(lna_offset)
                             );
    }
    else 
    {
        ftm_wcdma_calv3_get_dvga_lna_gain_offset
                            (device+1, // rx_flag start from 1
                             rx_measure->rx_meas_param[device].lna_offset.lna_state,
                             (word)rx_measure->rx_meas_param[device].lna_offset.expected_exagc,
                             (ftm_calibration_rx_meas_data_type*)(lna_offset)
                             );
    }
  }
 else
  {
      ftm_wcdma_calv3_get_dvga_lna_gain_offset
                          (dev_mask,
                           rx_measure->rx_meas_param[device].lna_offset.lna_state,
                           (word)rx_measure->rx_meas_param[device].lna_offset.expected_exagc,
                           (ftm_calibration_rx_meas_data_type*)(lna_offset)
                           );
  }

    } /* if( ( rx_measure->rx_meas_param[device].rx_meas_action & 
             ( 1 << RX_MEAS_LNA_OFFSET ) ) != 0 ) */

      /* RX_MEAS_DC_LNA_OFFSET */
      if( ( rx_measure->rx_meas_param[device].rx_meas_action & 
       ( 1 << RX_MEAS_DC_LNA_OFFSET ) ) != 0 )
      {
   if(ftm_wcdma_rxcal_opt_enabled())
   {
        if ( IS_FTM_IN_WCAL_MODE() == TRUE )
        {
          ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr
                              (device+1, // rx_flag start from 1
                               rx_measure->rx_meas_param[device].dc_lna_offset.lna_state,
                               (word)rx_measure->rx_meas_param[device].dc_lna_offset.expected_exagc,
                               (ftm_calibration_rx_meas_data_type*)(lna_offset)
                               );
        }
        else 
        {
          ftm_wcdma_calv3_get_dvga_lna_gain_offset
                                (device+1, // rx_flag start from 1
                                 rx_measure->rx_meas_param[device].dc_lna_offset.lna_state,
                                 (word)rx_measure->rx_meas_param[device].dc_lna_offset.expected_exagc,
                                 (ftm_calibration_rx_meas_data_type*)(lna_offset)
                                 );
        }
      }else
      {
        ftm_wcdma_calv3_get_dvga_lna_gain_offset
                            (dev_mask,
                             rx_measure->rx_meas_param[device].dc_lna_offset.lna_state,
                             (word)rx_measure->rx_meas_param[device].dc_lna_offset.expected_exagc,
                             (ftm_calibration_rx_meas_data_type*)(lna_offset)
                             );
       }       

      } /* if( ( rx_measure->rx_meas_param[device].rx_meas_action & 
             ( 1 << RX_MEAS_LNA_OFFSET ) ) != 0 ) */

            /* Populate device specific results */
            cal_result->rx_meas_param[device].rx_meas_action = 
              rx_measure->rx_meas_param[device].rx_meas_action;
      cal_result->rx_meas_param[device].lna_offset.lna_state = 
                          rx_measure->rx_meas_param[device].lna_offset.lna_state;
      cal_result->rx_meas_param[device].lna_offset.power_mode = 
                          rx_measure->rx_meas_param[device].lna_offset.power_mode;
      cal_result->rx_meas_param[device].lna_offset.lna_offset = lna_offset[0]; 
     
      cal_result->rx_meas_param[device].dc_lna_offset.lna_state = 
                          rx_measure->rx_meas_param[device].dc_lna_offset.lna_state;
      cal_result->rx_meas_param[device].dc_lna_offset.power_mode = 
                          rx_measure->rx_meas_param[device].dc_lna_offset.power_mode;
      cal_result->rx_meas_param[device].dc_lna_offset.lna_offset = lna_offset[0];

      /* Update OpCode to set the processed bit to zero. */
      dev_temp = dev_temp & (~dev_mask);
      
      /* For the current unprocessed Opcode, extract the bit mask for the first
      enabled cal action. The first bit will be from LSB. */
      dev_mask = rf_extract_first_one_bit_mask( dev_temp );

      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_2(FTM_HIGH, 
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_rx_measure: device %d, lna_offset %d",
                  device, lna_offset[0] );
      }   /* if(ftm_wcdma_calv3_dbg_msg_flag) */
    } /* if ( device < RFM_DEVICE_4 ) */
    else
    {
      ret_val = FALSE;
      FTM_MSG_1( FTM_ERROR, "ftm_wcdma_calv3_rx_measure: Dev %d "
                 "not valid", device );
      break;
    } /* if ! ( device < RFM_DEVICE_4 ) */
  } /* while ( dev_mask != 0 ) */

  /* Populate common results */
  cal_result->device_mask = rx_measure->device_mask;

  return ret_val;

} /* ftm_wcdma_calv3_rx_measure */

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
ftm_wcdma_calv3_tx_measure
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
  rfdevice_wcdma_hdet_read_type hdet_output={0};

  /* Parameter validation */
  if ( ( tx_measure == NULL ) || ( cal_result == NULL ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_tx_measure: NULL argument" );
    return FALSE;
  } /* if ( ( rx_measure == NULL ) || ( cal_result == NULL ) ) */
    
  prof_start_time = timetick_get();
    
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
      FTM_MSG_1(FTM_ERROR, "ftm_wcdma_calv3_tx_measure: Invalid Device: %d!",device);
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
          FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_tx_measure: hdet_sample input is 0");
          return FALSE;
        }

        /* Get HDET measurements */
        for( hdet_idx = 0; hdet_idx < hdet_sample; hdet_idx++)
        {
          /* Do up to 3 tries per HDET read */
          local_read_count = 0;
          do
          {
            rfdevice_hdet_wcdma_read_hdet( RFM_DEVICE_0,
                                           ftm_curr_wcdma_mode,
                                         FALSE, 
                                           &hdet_output );

            if ( hdet_output.hdet_quality == TRUE )
            {
              temp_hdet_val = hdet_output.hdet_reading;
            }
            else
            {
              temp_hdet_val = -1;
            }

            local_read_count++;
          }while ( ((temp_hdet_val <= 0) || temp_hdet_val == 0xFFFF)
                   && (local_read_count < 3));
      
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
        rfdevice_wcdma_tx_cmd_dispatch( 
              device,
              rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx,
              RFDEVICE_GET_THERMISTER_VALUE,
              &therm_read );

        /* Populate results data structure */ 
        cal_result->tx_meas_param[device].lin_therm.therm_read = therm_read;

        /* Send the Therm value to the data module for storage with respect to 
        channel */
        ftm_cal_data_set_therm_adc( 
                 rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx, 
                 &cal_result->tx_meas_param[device].lin_therm.therm_read );

        if ( tx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_1( FTM_MED, "WCDMA CalV3 DBG - ftm_wcdma_calv3_tx_measure: "
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
        fb_meas_inp.xpt_mode = 
          (rfcommon_mdsp_xpt_mode_enum_type)tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.xpt_mode;
        fb_meas_inp.gain_comp = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.gain_comp;
        fb_meas_inp.fbrx_gain_state = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.fb_gain_state;
        fb_meas_inp.ubias = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.ubias;
        fb_meas_inp.scale_rx = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.rx_scale;
        fb_meas_inp.store_capture_data = (boolean)tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.debug_mode_en;
        fb_meas_inp.tech = RFCOM_WCDMA_MODE;
        fb_meas_inp.band.wcdma_band = rfwcdma_mc_state.car_path_state[0][0].curr_band_tx;

        /* Call common FB Gain Measurement function */
        fb_meas_res = ftm_common_fbrx_capture_and_process(fb_meas_inp);

        if ( ftm_wcdma_calv3_dbg_msg_flag == TRUE )
        {
          FTM_MSG_8( FTM_MED, "WCDMA CalV3 DBG - FB Gain Cal Res: Status %d , Gain = %d,"
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
          FTM_MSG_1( FTM_ERROR, "WCDMA CalV3 DBG - ftm_wcdma_calv3_tx_measure: Dev %d "
                                "FB Gain Measurement Failed", device );
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_stat = 
            fb_meas_res.status;
        }

        if ( tx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_1( FTM_MED, "WCDMA CalV3 DBG - ftm_wcdma_calv3_tx_measure: "
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
          FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_tx_measure: fb_power_samples input is 0");
          return FALSE;
        }

        hdet_obj = ftm_common_get_tuner_object(RFM_DEVICE_0, RFDEVICE_HDET);
        rfdevice_hdet_cmn_set_gain_state( hdet_obj, 
                                          NULL, 
                                          RFDEVICE_EXECUTE_IMMEDIATE,
                                          tx_measure->tx_meas_param[device].fb_power.fp_power_params.fb_gain_state, 
                                          0 );

        /* Get HDET measurements */
        for( fb_power_samples_idx = 0; 
             fb_power_samples_idx < fb_power_samples; 
             fb_power_samples_idx++)
        {
          /* Do up to 3 tries per HDET read */
          local_read_count = 0;
          do
          {
            rfdevice_hdet_wcdma_read_hdet( RFM_DEVICE_0,
                                           ftm_curr_wcdma_mode,
                                           FALSE, 
                                           &hdet_output );

            if ( hdet_output.hdet_quality == TRUE )
            {
              temp_hdet_val = hdet_output.hdet_reading;
            }
            else
            {
              FTM_MSG_1( FTM_ERROR, "ftm_wcdma_calv3_tx_measure: Bad HDET Reading - Index %d",
                         fb_power_samples_idx );
              temp_hdet_val = -1;
            }

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
      if ( ftm_wcdma_calv3_dbg_msg_flag == TRUE )
      {
        FTM_MSG_3( FTM_HIGH, "WCDMA CalV3 DBG - ftm_wcdma_calv3_tx_measure:"
                   " Dev %d, tx_meas_action %d, hdet_read %d", device, 
                   tx_measure->tx_meas_param[device].tx_meas_action,
                   cal_result->tx_meas_param[device].hdet.hdet_read );        
      }

    } /* if (device == RFM_DEVICE_0) */
  } /* while ( dev_mask != 0 ) */

  /* Populate common results */
  cal_result->device_mask = tx_measure->device_mask;

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_3(FTM_HIGH,
              "WCDMA CalV3 DBG - ftm_wcdma_calv3_tx_measure: [Status %d] "
             "DevMask %d tmTime[%d]us", 
             ret_val, tx_measure->device_mask,
             timetick_get_elapsed( prof_start_time, T_USEC )
             );
  }

  return ret_val;

} /* ftm_wcdma_calv3_tx_measure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_TUNE_BAND_CHAN
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
LOCAL void ftm_wcdma_calv3_tune_band_chan(ftm_mode_id_type mode,
                                          uint16 chan,
                                          uint16 dc_chan_offset,
                                          boolean drx_enable_flag)
{
  /* ToDo:
     Add possible usage of CB for tune_to_multi_chan instead of hardcode wait
 */

  uint16 tune_channel_list[3] = {chan,0,0};
  rfcom_multi_carrier_hspa_id_type multi_carrier_type = RFCOM_SINGLE_CARRIER;
  ftm_rf_mode_type ftm_prev_rf_mode = ftm_current_rf_mode;
  rfcom_wcdma_band_type band =0;
  rfc_rx_path_type rx_path = 0; 
  ftm_concurrency_manager_params_type wcdma_cc_params;

  /* Transceiver tx tune parameters */
  rfdevice_wcdma_tx_tune_param_type tx_params; 
  tx_params.band       = RFCOM_BAND_INVALID;
  tx_params.bw         = RFDEV_WCDMA_BW_INVALID; 
  tx_params.dpd_state  = RFDEVICE_DPD_NUM; 

  memset(&wcdma_cc_params, 0, sizeof(ftm_concurrency_manager_params_type));

  /* Skip enter_mode if already done so */
  if (   (ftm_current_rf_mode != FTM_DB_RF_WCDMA_IMT)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_1900MHZ_A)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_1900MHZ_B)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC3)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC4)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_800MHZ)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC8)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC9)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC11)
      && (ftm_current_rf_mode != FTM_DB_RF_WCDMA_BC19))
  {
    ftm_rfmode_enter(RFM_DEVICE_0,FTM_STATE_WCDMA);
    rfm_enter_mode(RFCOM_TRANSCEIVER_0, RFCOM_WCDMA_MODE, NULL, NULL, ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx);
    rfm_enable_rx(RFCOM_TRANSCEIVER_0, NULL, NULL);    
  }

  /* Check if Dual Carrier setting is to be used */
  if(dc_chan_offset != 0)
  {
    multi_carrier_type = RFCOM_DUAL_CARRIER;
  }

  /* ToDo: Remove this when common framework supports band info */
  ftm_wcdma_calv3_current_mode = mode;

  /* Setup for PRx */
  switch(mode)
  {
    case FTM_PHONE_MODE_WCDMA_IMT:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_IMT;
      ftm_curr_wcdma_mode = RFCOM_BAND_IMT;
      tune_channel_list[0] += BSP_RF_WCDMA_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    case FTM_PHONE_MODE_WCDMA_1900A:
    case FTM_PHONE_MODE_WCDMA_1900B:
      ftm_current_rf_mode = (mode == FTM_PHONE_MODE_WCDMA_1900A) ? FTM_DB_RF_WCDMA_1900MHZ_A : FTM_DB_RF_WCDMA_1900MHZ_B;
      ftm_curr_wcdma_mode = RFCOM_BAND_1900;
      tune_channel_list[0] += BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    case FTM_PHONE_MODE_WCDMA_BC3:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC3;
      ftm_curr_wcdma_mode = RFCOM_BAND_BC3;
      tune_channel_list[0] += BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    case FTM_PHONE_MODE_WCDMA_BC4:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC4;
      ftm_curr_wcdma_mode = RFCOM_BAND_BC4;
      tune_channel_list[0] += BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    case FTM_PHONE_MODE_WCDMA_800:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_800MHZ;
      ftm_curr_wcdma_mode = RFCOM_BAND_800;
      tune_channel_list[0] += BSP_RF_WCDMA_800_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    case FTM_PHONE_MODE_WCDMA_BC8:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC8;
      ftm_curr_wcdma_mode = RFCOM_BAND_BC8;
      tune_channel_list[0] += BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    case FTM_PHONE_MODE_WCDMA_BC9:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC9;
      ftm_curr_wcdma_mode = RFCOM_BAND_BC9;
      tune_channel_list[0] += BSP_RF_WCDMA_BC9_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    case FTM_PHONE_MODE_WCDMA_BC11:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC11;
      ftm_curr_wcdma_mode = RFCOM_BAND_BC11;
      tune_channel_list[0] += BSP_RF_WCDMA_BC11_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    case FTM_PHONE_MODE_WCDMA_BC19:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC19;
      ftm_curr_wcdma_mode = RFCOM_BAND_BC19;
      tune_channel_list[0] += BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET;
      break;

    default:
      break;
  }

  /* Setup for DRx */
  tune_channel_list[1] = (dc_chan_offset == 0) ? 0 : tune_channel_list[0] + dc_chan_offset;

  rx_path = RFC_PRX_PATH;
  band = rfwcdma_core_convert_chan_to_band(tune_channel_list[0]);

  wcdma_cc_params.rfm_dev = RFM_DEVICE_0;
  wcdma_cc_params.band.wcdma_band = band;
  wcdma_cc_params.prx_drx =rx_path;
  wcdma_cc_params.tech = RFM_IMT_MODE; 

  if(ftm_concurrency_manager_validate_operation(&wcdma_cc_params))
  {/* Rx PLL */
  rfm_tune_to_multi_chan (RFM_DEVICE_0,
                          tune_channel_list,
                          multi_carrier_type, 
                          RFM_NUM_TUNES, 
                          NULL, 
                          NULL,
                          ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx
                          ); 
  }
  else
  {
     FTM_MSG_2( FTM_ERROR, "ftm_wcdma_calv3_tune_band_chan: Invalid concurreny scenario- device:%d, band %d ", RFM_DEVICE_0 , band );
     return; 
  }

  /* If there is a band change or discrepency between current diversity
     status then check if diversity needs to be enabled */
  if( (ftm_prev_rf_mode != ftm_current_rf_mode) ||
      (drx_enable_flag != ftm_wcdma_div_status[RFM_DEVICE_1])
    )
  {
    if(drx_enable_flag)
    {
      ftm_wcdma_set_secondary_chain(RFM_DEVICE_1,ON);

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG(FTM_HIGH, "WCDMA CalV3 DBG - ftm_wcdma_calv3_tune_band_chan: Enable DRx");
      }
    }

    else
    {
      ftm_wcdma_set_secondary_chain(RFM_DEVICE_1,OFF);

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG(FTM_HIGH, "WCDMA CalV3 DBG - ftm_wcdma_calv3_tune_band_chan: Disable DRx");
      }
    }
  }

  if(ftm_wcdma_div_status[RFM_DEVICE_1] == TRUE)
  {
    /* Wait for PRx AGC to settle. */
    DALSYS_BusyWait(ftm_wcdma_calv3_prx_drx_tune_wait);
    wcdma_cc_params.rfm_dev = RFM_DEVICE_1;

    if(ftm_concurrency_manager_validate_operation(&wcdma_cc_params))
    {
    rfm_tune_to_multi_chan (RFM_DEVICE_1,
                            tune_channel_list,
                            multi_carrier_type, 
                            RFM_NUM_TUNES, 
                            NULL, 
                            NULL,                          
                            ftm_wcdma_rxlm_buffer[RFM_DEVICE_1].buf_idx
                            );  
    }
    else
    {
      FTM_MSG_2( FTM_ERROR, "ftm_wcdma_calv3_tune_band_chan: Invalid concurreny scenario- device:%d, band %d ", RFM_DEVICE_1 , band );
      return; 
    }
  }   

  if(ftm_wcdma_tx_on_status == TRUE)
  {
    /* Tx PLL */
    if(rfwcdma_core_xpt_is_enabled(ftm_curr_wcdma_mode))
    {
      tx_params.band      = ftm_curr_wcdma_mode;
      tx_params.bw        = RFDEV_WCDMA_BW_1X; 
      tx_params.channel[0]= chan; 
      tx_params.dpd_state = RFDEVICE_DPD_ENABLED; 

      rfdevice_wcdma_tx_set_channel(RFM_DEVICE_0, RFCOM_WCDMA_MODE, 
                                    ftm_curr_wcdma_mode, tx_params, 
                                    RFDEVICE_EXECUTE_IMMEDIATE, NULL); 

      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG(FTM_HIGH,"ftm_wcdma_calv3_tune_band_chan DPD enabled.");
      }
    }
    else
    {
      tx_params.band      = ftm_curr_wcdma_mode;
      tx_params.bw        = RFDEV_WCDMA_BW_1X; 
      tx_params.channel[0]= chan; 
      tx_params.dpd_state = RFDEVICE_DPD_DISABLED;

      rfdevice_wcdma_tx_set_channel(RFM_DEVICE_0, RFCOM_WCDMA_MODE, 
                                    ftm_curr_wcdma_mode, tx_params, 
                                    RFDEVICE_EXECUTE_IMMEDIATE, NULL); 
    }
  }

  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
    FTM_MSG_5(FTM_HIGH,
              "WCDMA CAlV3 DBG - ftm_wcdma_calv3_tune_band_chan: mode %d, multi_carrier_type %d, chan_list[0] %d, chan_list[1] %d ftm_wcdma_div_status %d",
              mode,
              multi_carrier_type,
              tune_channel_list[0],
              tune_channel_list[1],
              ftm_wcdma_div_status[RFM_DEVICE_1]
              );
  }

} /* ftm_wcdma_calv3_tune_band_chan */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_GET_DVGA_LNA_GAIN_OFFSET
  This functions gets DVGA or LNA gain offsets for PRx and/or DRx
 
  @details
  This functions gets DVGA or LNA gain offsets for PRx and/or DRx based on the
  input flags and the populates results in to passed-in array pointer.
 
  Notes and Assumptions: 
    - There can only be one LNA range per segment so there can only be one exp_agc
    - If PRx and DRx readings are to be done together then the first in the result
      buffer is always PRx. This is based on the CalV3 common framework design.

  @param rx_flag
 	 Indicator as to which Rx chains to be measured. Interpretation based on
	 ftm_calibration_rx_flag_mask_enum_type

  @param lna_range
 	 0 indicates DVGA gain offset(s) to be measured
	 1-5 indicates the LNA range to be used for LNA offset measurement(s)

  @param expected_agc_val
     Expected AGC value

  @param result_buffer
     Result buffer to populate measurements
 */

LOCAL void ftm_wcdma_calv3_get_dvga_lna_gain_offset
(uint32 rx_flag, byte lna_range, word expected_agc_val, ftm_calibration_rx_meas_data_type* result_buffer)
{
  uint8 result_count = 0;        /* Result index counter */
  int2 num_of_agc_reads;         /* AGC read counter */
  int2 prx_offset = 0;           /* Calculated DVGA/LNA offset for PRx*/
  int2 drx_offset = 0;           /* Calculated DVGA/LNA offset for DRx*/
  int2 agc_read_val_prx = 0;     /* PRx AGC accumulation */
  int2 agc_read_val_drx = 0;     /* DRx AGC accumulation */
  int2 agc_read_val_dev2 = 0;     /* RFM_DEVICE_2 AGC accumulation */
  int2 agc_read_val_dev3 = 0;     /* RFM_DEVICE_3 AGC accumulation */

  /* DVGA gain offset operations */
  if (lna_range == (byte)FTM_LNA_RANGE_0)
  {
    for (num_of_agc_reads = 0; num_of_agc_reads < FTM_NUM_OF_AVG_RX_AGC_READ; num_of_agc_reads++)
    {
      if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
      {
        agc_read_val_prx += rfm_get_rx_raw_agc_val(RFM_DEVICE_0);
      }
      if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
      {
        agc_read_val_drx += rfm_get_rx_raw_agc_val(RFM_DEVICE_1);
      }
      if((rx_flag & DEVICE_FLAG_2) == DEVICE_FLAG_2)
      {
        agc_read_val_dev2 += rfm_get_receive_agc_car1_val(RFM_DEVICE_2, RFCOM_SERVINGCELL_RXAGC_RAW, RFCOM_RXAGC_IN_DB10);
      }
      if((rx_flag & DEVICE_FLAG_3) == DEVICE_FLAG_3)
      {
        agc_read_val_dev3 += rfm_get_receive_agc_car1_val(RFM_DEVICE_3, RFCOM_SERVINGCELL_RXAGC_RAW, RFCOM_RXAGC_IN_DB10);
      }
      DALSYS_BusyWait(100);
    }
    
    /* Compute and set DVGA gain offset for PRx */
    if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
    {
      agc_read_val_prx = agc_read_val_prx/FTM_NUM_OF_AVG_RX_AGC_READ;
      prx_offset = (agc_read_val_prx - ((int2) expected_agc_val)  - rfwcdma_msm_get_dvga_offset(RFM_DEVICE_0));

      /* Range checking */
      prx_offset = RF_SATURATE(prx_offset, FTM_DVGA_GAIN_OFFSET_MIN, FTM_DVGA_GAIN_OFFSET_MAX);

      /* Set dvga gain offset */
      ftm_wcdma_set_dvga_gain_offset(RFM_DEVICE_0, prx_offset );

      /* Write result buffer */
      result_buffer[result_count++] = (ftm_calibration_rx_meas_data_type)prx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_3(FTM_HIGH,
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_get_dvga_lna_gain_offset: Rx chain = %d, Exp Rx_AGC = %d, DVGA offset = %d", 
                  DEVICE_FLAG_0, (int16)expected_agc_val, prx_offset
				  );
      }
    }

    /* Compute and set DVGA gain offset for DRx */
    if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
    {
      agc_read_val_drx = agc_read_val_drx/FTM_NUM_OF_AVG_RX_AGC_READ;
      drx_offset = (agc_read_val_drx - ((int2) expected_agc_val) - rfwcdma_msm_get_dvga_offset(RFM_DEVICE_1));

      /* Range checking */
      drx_offset = RF_SATURATE(drx_offset, FTM_DVGA_GAIN_OFFSET_MIN, FTM_DVGA_GAIN_OFFSET_MAX);

      /* Set dvga gain offset */
      ftm_wcdma_set_dvga_gain_offset(RFM_DEVICE_1, drx_offset );

      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)drx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_3(FTM_HIGH,
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_get_dvga_lna_gain_offset: Rx chain = %d, Exp Rx_AGC = %d, DVGA offset = %d", 
                  DEVICE_FLAG_1, (int16)expected_agc_val, drx_offset
				  );
      }
    }

    /* Compute and set DVGA gain offset for PRx second carrier path */
    if((rx_flag & DEVICE_FLAG_2) == DEVICE_FLAG_2)
    {
      agc_read_val_dev2 = agc_read_val_dev2/FTM_NUM_OF_AVG_RX_AGC_READ;
      drx_offset = (agc_read_val_dev2 - ((int2) expected_agc_val) - rfwcdma_msm_get_dvga_offset(RFM_DEVICE_2));

      /* Range checking */
      drx_offset = RF_SATURATE(drx_offset, FTM_DVGA_GAIN_OFFSET_MIN, FTM_DVGA_GAIN_OFFSET_MAX);

      /* Set dvga gain offset */
      ftm_wcdma_set_dvga_gain_offset(RFM_DEVICE_2, drx_offset );

      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)drx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_3(FTM_HIGH,
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_get_dvga_lna_gain_offset: Rx chain = %d, Exp Rx_AGC = %d, DVGA offset = %d", 
                  DEVICE_FLAG_2, (int16)expected_agc_val, drx_offset
				  );
      }
    }

    /* Compute and set DVGA gain offset for DRx */
    if((rx_flag & DEVICE_FLAG_3) == DEVICE_FLAG_3)
    {
      agc_read_val_dev3 = agc_read_val_dev3/FTM_NUM_OF_AVG_RX_AGC_READ;
      drx_offset = (agc_read_val_dev3 - ((int2) expected_agc_val) - rfwcdma_msm_get_dvga_offset(RFM_DEVICE_3));

      /* Range checking */
      drx_offset = RF_SATURATE(drx_offset, FTM_DVGA_GAIN_OFFSET_MIN, FTM_DVGA_GAIN_OFFSET_MAX);

      /* Set dvga gain offset */
      ftm_wcdma_set_dvga_gain_offset(RFM_DEVICE_3, drx_offset );

      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)drx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_3(FTM_HIGH,
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_get_dvga_lna_gain_offset: Rx chain = %d, Exp Rx_AGC = %d, DVGA offset = %d", 
                  DEVICE_FLAG_3, (int16)expected_agc_val, drx_offset
				  );
      }
    }
  } /* DVGA gain offset operations */

  /* LNA offset operations */
  else if (lna_range < (byte)FTM_LNA_RANGE_MAX)
  {
    for (num_of_agc_reads = 0; num_of_agc_reads < FTM_NUM_OF_AVG_RX_AGC_READ; num_of_agc_reads++)
    {
      if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
      {
        agc_read_val_prx += rfwcdma_mdsp_get_raw_rx_agc(RFM_DEVICE_0); 
      }
      if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
      {
        agc_read_val_drx += rfwcdma_mdsp_get_raw_rx_agc(RFM_DEVICE_1); 
      }
      if((rx_flag & DEVICE_FLAG_2) == DEVICE_FLAG_2)
      {
        agc_read_val_dev2 += rfwcdma_mdsp_get_raw_rx_agc(RFM_DEVICE_2); 
      }
      if((rx_flag & DEVICE_FLAG_3) == DEVICE_FLAG_3)
      {
        agc_read_val_dev3 += rfwcdma_mdsp_get_raw_rx_agc(RFM_DEVICE_3); 
      }
      DALSYS_BusyWait(100);
    }

    /* Compute and set LNA offset for PRx */
    if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
    {
      agc_read_val_prx = agc_read_val_prx/FTM_NUM_OF_AVG_RX_AGC_READ;
      prx_offset = (((int2)expected_agc_val) - agc_read_val_prx) + rfwcdma_msm_get_dvga_offset(RFM_DEVICE_0);

       /* Set LNA offset */
      ftm_wcdma_set_lna_offset( RFM_DEVICE_0, lna_range , prx_offset);

      /* Write result buffer */
      result_buffer[result_count++] = (ftm_calibration_rx_meas_data_type)prx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_4(FTM_HIGH,
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_get_dvga_lna_gain_offset: Rx chain = %d, LNA range = %d, Exp Rx_AGC = %d, LNA offset = %d", 
                  DEVICE_FLAG_0, lna_range, (int16)expected_agc_val, result_buffer[result_count-1]
                  );
      }
    }

    /* Compute and set LNA offset for DRx */
    if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
    {
      agc_read_val_drx = agc_read_val_drx/FTM_NUM_OF_AVG_RX_AGC_READ;
      drx_offset = (((int2)expected_agc_val) - agc_read_val_drx)+ rfwcdma_msm_get_dvga_offset(RFM_DEVICE_1);

       /* Set LNA offset */
      ftm_wcdma_set_lna_offset( RFM_DEVICE_1, lna_range , drx_offset);

      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)drx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_4(FTM_HIGH,
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_get_dvga_lna_gain_offset: Rx chain = %d, LNA range = %d, Exp Rx_AGC = %d, LNA offset = %d", 
                  DEVICE_FLAG_1, lna_range, (int16)expected_agc_val, result_buffer[result_count]
                  );
      }
    }

    /* Compute and set LNA offset for device 2 */
    if((rx_flag & DEVICE_FLAG_2) == DEVICE_FLAG_2)
    {
      agc_read_val_dev2 = agc_read_val_dev2/FTM_NUM_OF_AVG_RX_AGC_READ;
      drx_offset = (((int2)expected_agc_val) - agc_read_val_dev2) + rfwcdma_msm_get_dvga_offset(RFM_DEVICE_2);

       /* Set LNA offset */
      ftm_wcdma_set_lna_offset( RFM_DEVICE_2, lna_range , drx_offset);

      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)drx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_4(FTM_HIGH,
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_get_dvga_lna_gain_offset: Rx chain = %d, LNA range = %d, Exp Rx_AGC = %d, LNA offset = %d", 
                  DEVICE_FLAG_2, lna_range, (int16)expected_agc_val, result_buffer[result_count]
                  );
      }
    }

    /* Compute and set LNA offset for DRx */
    if((rx_flag & DEVICE_FLAG_3) == DEVICE_FLAG_3)
    {
      agc_read_val_dev3 = agc_read_val_dev3/FTM_NUM_OF_AVG_RX_AGC_READ;
      drx_offset = (((int2)expected_agc_val) - agc_read_val_dev3) + rfwcdma_msm_get_dvga_offset(RFM_DEVICE_3);

       /* Set LNA offset */
      ftm_wcdma_set_lna_offset( RFM_DEVICE_3, lna_range , drx_offset);

      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)drx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
        FTM_MSG_4(FTM_HIGH,
                  "WCDMA CalV3 DBG - ftm_wcdma_calv3_get_dvga_lna_gain_offset: Rx chain = %d, LNA range = %d, Exp Rx_AGC = %d, LNA offset = %d", 
                  DEVICE_FLAG_3, lna_range, (int16)expected_agc_val, result_buffer[result_count]
                  );
      }
    }
  } /* LNA offset operations */
} /* ftm_wcdma_calv3_get_dvga_lna_gain_offset */




/*----------------------------------------------------------------------------*/
/*!
  @brief
  FUNCTION FTM_WCDMA_CALV3_GET_DVGA_LNA_GAIN_OFFSET_NBPWR
  This functions gets DVGA or LNA gain offsets for PRx and/or DRx
 
  @details
  This functions gets DVGA or LNA gain offsets for PRx and/or DRx based on the
  input flags and the populates results in to passed-in array pointer.
 
  Notes and Assumptions: 
    - There can only be one LNA range per segment so there can only be one exp_agc
    - If PRx and DRx readings are to be done together then the first in the result
      buffer is always PRx. This is based on the CalV3 common framework design.

  @param rx_flag
 	 Indicator as to which Rx chains to be measured. Interpretation based on
	 ftm_calibration_rx_flag_mask_enum_type

  @param lna_range
 	 0 indicates DVGA gain offset(s) to be measured
	 1-5 indicates the LNA range to be used for LNA offset measurement(s)

  @param expected_agc_val
     Expected AGC value

  @param result_buffer
     Result buffer to populate measurements
 */

LOCAL void ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr
(uint32 rx_flag, byte lna_range, word expected_agc_val, ftm_calibration_rx_meas_data_type* result_buffer)
{
  uint8 result_count = 0;        /* Result index counter */
  int2 num_of_agc_reads;         /* AGC read counter */
  static int2 prx_dvga_offset = 0;      /* Calculated DVGA offset for PRx*/
  static int2 drx_dvga_offset = 0;      /* Calculated DVGA offset for DRx*/
  static int2 dvga_offset_dev2 = 0;     /* Calculated DVGA offset for RFM_DEVICE_2*/
  int2 dvga_offset_dev3 = 0;     /* Calculated DVGA offset for RFM_DEVICE_3*/
  int2 prx_offset = 0;           /* Calculated LNA offset for PRx*/
  int2 drx_offset = 0;           /* Calculated LNA offset for DRx*/
  int2 prx_offset_dev2 = 0;      /* Calculated LNA offset for RFM_DEVICE_2*/
  int2 prx_offset_dev3 = 0;      /* Calculated LNA offset for RFM_DEVICE_3*/
  uint32 agc_read_val_prx = 0;     /* PRx AGC accumulation */
  uint32 agc_read_val_drx = 0;     /* DRx AGC accumulation */
  uint2 agc_read_val_dev2 = 0;     /* RFM_DEVICE_2 AGC accumulation */
  uint2 agc_read_val_dev3 = 0;     /* RFM_DEVICE_3 AGC accumulation */
  int2 zero_db_gain = rfwcdma_msm_get_zero_db_digital_gain_10dBm() ;

 

  if (lna_range == (byte)FTM_LNA_RANGE_0)
  {
    for (num_of_agc_reads = 0; num_of_agc_reads < ftm_num_nbee_read; num_of_agc_reads++)
    {
      if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
      {
        agc_read_val_prx += rfwcdma_msm_read_nbpwr(RFM_DEVICE_0 ,  ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx );
      }
      if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
      {

        agc_read_val_drx += rfwcdma_msm_read_nbpwr(RFM_DEVICE_1 ,  ftm_wcdma_rxlm_buffer[RFM_DEVICE_1].buf_idx );
      }
      if((rx_flag & DEVICE_FLAG_2) == DEVICE_FLAG_2)
      {
        agc_read_val_dev2 += rfwcdma_msm_read_nbpwr(RFM_DEVICE_2 ,  ftm_wcdma_rxlm_buffer[RFM_DEVICE_2].buf_idx );
      }
      if((rx_flag & DEVICE_FLAG_3) == DEVICE_FLAG_3)
      {
        agc_read_val_dev3 +=rfwcdma_msm_read_nbpwr(RFM_DEVICE_3 ,  ftm_wcdma_rxlm_buffer[RFM_DEVICE_3].buf_idx );
      }
      if( num_of_agc_reads != ftm_num_nbee_read) DALSYS_BusyWait(nbee_delay_read);
    }
    
    /* Compute and set DVGA gain offset for PRx */
    if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
    {
      agc_read_val_prx = agc_read_val_prx/ftm_num_nbee_read;

      prx_dvga_offset = (   rfwcdma_msm_convert_raw_nbee_to_10dBm(agc_read_val_prx) - zero_db_gain - ((int2) expected_agc_val)) - rfwcdma_msm_get_dvga_offset(RFM_DEVICE_0);
       
      /* Range checking */
      prx_dvga_offset = RF_SATURATE(prx_dvga_offset, FTM_DVGA_GAIN_OFFSET_MIN, FTM_DVGA_GAIN_OFFSET_MAX);

      /* Write result buffer */
      result_buffer[result_count++] = (ftm_calibration_rx_meas_data_type)prx_dvga_offset;

	   /* Print debug msg if enabled */
       if(ftm_wcdma_calv3_dbg_msg_flag)
       {
        FTM_MSG_5(FTM_HIGH,
                  "ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr: Rx chain = %d, agc_read_val_prx =%d, zero_db_gain =%d, Exp Rx_AGC = %d, PRX DVGA offset = %d", 
                  DEVICE_FLAG_0, agc_read_val_prx, zero_db_gain, (int16)expected_agc_val, prx_dvga_offset);
       }
      }

    /* Compute and set DVGA gain offset for DRx */
    if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
    {
      agc_read_val_drx = agc_read_val_drx/ftm_num_nbee_read;

      drx_dvga_offset = ( rfwcdma_msm_convert_raw_nbee_to_10dBm(agc_read_val_drx) - zero_db_gain - ((int2) expected_agc_val)) - rfwcdma_msm_get_dvga_offset(RFM_DEVICE_1);

      /* Range checking */
      drx_dvga_offset = RF_SATURATE(drx_dvga_offset, FTM_DVGA_GAIN_OFFSET_MIN, FTM_DVGA_GAIN_OFFSET_MAX);

      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)drx_dvga_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
	 FTM_MSG_5(FTM_HIGH,
		   "ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr: Rx chain = %d, agc_read_val_drx =%d, zero_db_gain =%d, Exp Rx_AGC = %d, DRX DVGA offset = %d", 
		   DEVICE_FLAG_1, agc_read_val_drx, zero_db_gain, (int16)expected_agc_val, drx_dvga_offset);
      }


    }

    /* Compute and set DVGA gain offset for PRx second carrier path */
    if((rx_flag & DEVICE_FLAG_2) == DEVICE_FLAG_2)
    {
      agc_read_val_dev2 = agc_read_val_dev2/ftm_num_nbee_read;

      dvga_offset_dev2 = ( rfwcdma_msm_convert_raw_nbee_to_10dBm(agc_read_val_dev2) - zero_db_gain - ((int2) expected_agc_val)) - rfwcdma_msm_get_dvga_offset(RFM_DEVICE_2);

      /* Range checking */
      dvga_offset_dev2 = RF_SATURATE(dvga_offset_dev2, FTM_DVGA_GAIN_OFFSET_MIN, FTM_DVGA_GAIN_OFFSET_MAX);

      /* Set dvga gain offset */
      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)dvga_offset_dev2;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
	FTM_MSG_5(FTM_HIGH,
  		  "ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr: Rx chain = %d, agc_read_val_drx =%d, zero_db_gain =%d, Exp Rx_AGC = %d, DRX DVGA offset = %d", 
		  DEVICE_FLAG_2, agc_read_val_dev2, zero_db_gain, (int16)expected_agc_val, dvga_offset_dev2);
	  }

    }

    /* Compute and set DVGA gain offset for DRx */
    if((rx_flag & DEVICE_FLAG_3) == DEVICE_FLAG_3)
    {
      agc_read_val_dev3 = agc_read_val_dev3/ftm_num_nbee_read;

      dvga_offset_dev3 = ( rfwcdma_msm_convert_raw_nbee_to_10dBm(agc_read_val_dev3) - zero_db_gain - ((int2) expected_agc_val)) - rfwcdma_msm_get_dvga_offset(RFM_DEVICE_3);

      /* Range checking */
      dvga_offset_dev3 = RF_SATURATE(dvga_offset_dev3, FTM_DVGA_GAIN_OFFSET_MIN, FTM_DVGA_GAIN_OFFSET_MAX);

      /* Set dvga gain offset */
      ftm_wcdma_set_dvga_gain_offset(RFM_DEVICE_3, dvga_offset_dev3 );

      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)dvga_offset_dev3;

     /* Print debug msg if enabled */
     if(ftm_wcdma_calv3_dbg_msg_flag)
     {
        FTM_MSG_5(FTM_HIGH,
        	  "ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr: Rx chain = %d, agc_read_val_drx =%d, zero_db_gain =%d, Exp Rx_AGC = %d, DRX DVGA offset = %d", 
		  DEVICE_FLAG_3, agc_read_val_dev3, zero_db_gain, (int16)expected_agc_val, dvga_offset_dev3);
	 }

    }
  } /* DVGA gain offset operations */

  /* LNA offset operations */
  else if (lna_range < (byte)FTM_LNA_RANGE_MAX)
  {
    for (num_of_agc_reads = 0; num_of_agc_reads < ftm_num_nbee_read; num_of_agc_reads++)
    {
      if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
      {

        agc_read_val_prx +=  rfwcdma_msm_read_nbpwr(RFM_DEVICE_0 ,  ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx ); 
      }
      if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
      {
        agc_read_val_drx += rfwcdma_msm_read_nbpwr(RFM_DEVICE_1 ,  ftm_wcdma_rxlm_buffer[RFM_DEVICE_1].buf_idx );
      }
      if((rx_flag & DEVICE_FLAG_2) == DEVICE_FLAG_2)
      {
        agc_read_val_dev2 += rfwcdma_msm_read_nbpwr(RFM_DEVICE_2 ,  ftm_wcdma_rxlm_buffer[RFM_DEVICE_2].buf_idx ); 
      }
      if((rx_flag & DEVICE_FLAG_3) == DEVICE_FLAG_3)
      {
        agc_read_val_dev3 += rfwcdma_msm_read_nbpwr(RFM_DEVICE_3 ,  ftm_wcdma_rxlm_buffer[RFM_DEVICE_3].buf_idx ); 
      }
     if( num_of_agc_reads != ftm_num_nbee_read)  DALSYS_BusyWait(nbee_delay_read);
    }

    /* Compute and set LNA offset for PRx */
    if((rx_flag & DEVICE_FLAG_0) == DEVICE_FLAG_0)
    {

      agc_read_val_prx = agc_read_val_prx/ftm_num_nbee_read;
      
      prx_offset = (( (int2)expected_agc_val) + prx_dvga_offset + zero_db_gain - rfwcdma_msm_convert_raw_nbee_to_10dBm(agc_read_val_prx) ) +  rfwcdma_msm_get_dvga_offset(RFM_DEVICE_0);

      /* Write result buffer */
      result_buffer[result_count++] = (ftm_calibration_rx_meas_data_type)prx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {

      FTM_MSG_8(FTM_HIGH,
                "ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr: Rx chain = %d, LNA range = %d, expected_agc_val = %d"
                "prx_dvga_offset = %d,  zero_db_gain =%d, agc_read_val_prx = %d, LNA offset = %d, result_buffer =%d",
                DEVICE_FLAG_0, 
                lna_range, 
                (int2)expected_agc_val,  
                prx_dvga_offset,  
                zero_db_gain,  
                agc_read_val_prx, 
                prx_offset ,  
                result_buffer[result_count-1]);
     }	  
    }

    /* Compute and set LNA offset for DRx */
    if((rx_flag & DEVICE_FLAG_1) == DEVICE_FLAG_1)
    {
      agc_read_val_drx = agc_read_val_drx/ftm_num_nbee_read;

      drx_offset = (( (int2)expected_agc_val) + drx_dvga_offset + zero_db_gain - rfwcdma_msm_convert_raw_nbee_to_10dBm(agc_read_val_drx) ) +  rfwcdma_msm_get_dvga_offset(RFM_DEVICE_1);
      
      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)drx_offset;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
	FTM_MSG_8(FTM_HIGH,
		  "ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr: Rx chain = %d, LNA range = %d, expected_agc_val = %d"
		  "prx_dvga_offset = %d,  zero_db_gain =%d, agc_read_val_prx = %d, LNA offset = %d, result_buffer =%d",
		  DEVICE_FLAG_1, 
		  lna_range, 
		  (int2)expected_agc_val,  
		  prx_dvga_offset,	
		  zero_db_gain,  
		  agc_read_val_drx, 
		  drx_offset ,	
		  result_buffer[result_count-1]);
      }	

    }

    /* Compute and set LNA offset for device 2 */
    if((rx_flag & DEVICE_FLAG_2) == DEVICE_FLAG_2)
    {
      agc_read_val_dev2 = agc_read_val_dev2/ftm_num_nbee_read;

      prx_offset_dev2 = (( (int2)expected_agc_val) + dvga_offset_dev2 + zero_db_gain - rfwcdma_msm_convert_raw_nbee_to_10dBm(agc_read_val_dev2) ) +  rfwcdma_msm_get_dvga_offset(RFM_DEVICE_2);
   
      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)prx_offset_dev2;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
	FTM_MSG_8(FTM_HIGH,
		  "ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr: Rx chain = %d, LNA range = %d, expected_agc_val = %d"
		  "prx_dvga_offset = %d,  zero_db_gain =%d, agc_read_val_prx = %d, LNA offset = %d, result_buffer =%d",
		  DEVICE_FLAG_2, 
		  lna_range, 
		  (int2)expected_agc_val,  
		  prx_dvga_offset,	
		  zero_db_gain,  
		  agc_read_val_dev2, 
		  prx_offset_dev2 ,	
		  result_buffer[result_count-1]);
      }	
    }

    /* Compute and set LNA offset for DRx */
    if((rx_flag & DEVICE_FLAG_3) == DEVICE_FLAG_3)
    {
      agc_read_val_dev3 = agc_read_val_dev3/ftm_num_nbee_read;
      prx_offset_dev3 = (( (int2)expected_agc_val) + dvga_offset_dev3 + zero_db_gain - rfwcdma_msm_convert_raw_nbee_to_10dBm(agc_read_val_dev3) ) +  rfwcdma_msm_get_dvga_offset(RFM_DEVICE_3);
      
      /* Write result buffer */
      result_buffer[result_count] = (ftm_calibration_rx_meas_data_type)prx_offset_dev3;

      /* Print debug msg if enabled */
      if(ftm_wcdma_calv3_dbg_msg_flag)
      {
	 FTM_MSG_8(FTM_HIGH,
		   "ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr: Rx chain = %d, LNA range = %d, expected_agc_val = %d"
		   "prx_dvga_offset = %d,  zero_db_gain =%d, agc_read_val_prx = %d, LNA offset = %d, result_buffer =%d",
		   DEVICE_FLAG_3, 
		   lna_range, 
		   (int2)expected_agc_val,	
		   prx_dvga_offset,  
		   zero_db_gain,  
		   agc_read_val_dev3, 
		   prx_offset_dev3 , 
		   result_buffer[result_count-1]);
      } 

    }
  } /* LNA offset operations */
} /* ftm_wcdma_calv3_get_dvga_lna_gain_offset_nbpwr */




LOCAL void ftm_wcdma_calv3_set_current_mode(rfcom_wcdma_band_type band)
{
 ftm_curr_wcdma_mode = band;

 switch(band)
  {
    case RFCOM_BAND_IMT:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_IMT;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_IMT;
      break;

    case RFCOM_BAND_1900:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_1900MHZ_A;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_1900A;
      break;

    case RFCOM_BAND_BC3:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC3;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_BC3;
      break;

    case RFCOM_BAND_BC4:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC4;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_BC4;
      break;

    case RFCOM_BAND_800:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_800MHZ;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_800;
      break;

    case RFCOM_BAND_BC8:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC8;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_BC8;
      break;

    case RFCOM_BAND_BC9:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC9;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_BC9;
      break;

    case RFCOM_BAND_BC11:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC11;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_BC11;
      break;

    case RFCOM_BAND_BC19:
      ftm_current_rf_mode = FTM_DB_RF_WCDMA_BC19;
      ftm_wcdma_calv3_current_mode = FTM_PHONE_MODE_WCDMA_BC19;
      break;

    default:
      break;
  }
} /* ftm_wcdma_calv3_set_current_mode */

#if 0
boolean ftm_wcdma_ept_config( ftm_cal_xpt_instr_payload_ept_config_type const *payload )
{
  /* Initialize EPT config params */
  int32 exp_scale = 16526664;
  int16 search_center = 29;
  int16 search_width =10;
  uint8 exp_scale_q_fac = 29;
  uint8 lin_tx_mag = 102;
  uint8 pm_order = 2;

  rfnv_wcdma_nv_tbl_type *nv_tbl;

  boolean ret_val = TRUE;
  boolean pick_from_nv = FALSE;
  
  mcpm_request_type wmcpmdrv_req;
  mcpm_request_parms_type wmcpmdrv_request_parms;

  nv_tbl = rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode) );

  /* Populate the EPT config parameters from NV*/
  if( pick_from_nv )
  {
    exp_scale = (int32) nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_EXP_SCALE];
    search_center = (int16) nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_SEARCH_CENTER];
    search_width = (int16) nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_SEARCH_WIDTH];
    exp_scale_q_fac = (uint8) nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_EXP_SCALE_Q_FACT];
    lin_tx_mag = (uint8) nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_LIN_TX_MAG];
    pm_order = (uint8) nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_PM_ORDER];
  }

  /* Set up the RxLM buffer */
  /* allocate the chain 1 buffer so we can populate it with SAPT config
         if it hasn't been allocated in FTM yet */
  if(!ftm_wcdma_rxlm_buffer[RF_PATH_1].is_buf_allocated)
  {
    ftm_wcdma_allocate_rxlm(FTM_RECEIVE_CHAIN_1);
  }
  rfwcdma_msm_set_et_state_rxlm(ftm_wcdma_rxlm_buffer[RF_PATH_1].buf_idx);


  /* Setup offline clock to 144MHz for EPT Sample capture */
  memset(&wmcpmdrv_request_parms, 0x0, sizeof(mcpm_request_parms_type));
  wmcpmdrv_req = MCPM_WCDMA_PARMS_UPDATE_REQ;
  wmcpmdrv_request_parms.rf_bandwidth = MCPM_WCDMA;
  wmcpmdrv_request_parms.update_info |= MCPM_RF_BANDWIDTH_UPDATE;
  wmcpmdrv_request_parms.update_info |= MCPM_MODEM_PERF_MODE_UPDATE;
  wmcpmdrv_request_parms.modem_perf_mode = MCPM_MODEM_WCDMA_HIGH_PERF_MODE;
  MCPM_Config_Modem(wmcpmdrv_req, &wmcpmdrv_request_parms);
  
  FTM_MSG(FTM_HIGH, "In ftm_wcdma_ept_config() , calling mDSP function " );
  rfwcdma_mdsp_ept_config(exp_scale, 
    search_center,
    search_width,
    exp_scale_q_fac,
    lin_tx_mag,
    pm_order);

  return ret_val;
}
#endif

#if 0
boolean ftm_wcdma_ept_do_iq_capture( ftm_cal_xpt_instr_payload_iq_capture_type const
                                  *iq_capture )
{

  uint32 rx_chain = 1;
  uint32 tx_chain = 0;
  uint32 do_processing = 1;
  boolean ret_val = TRUE;
  const rfc_common_logical_device_params_type* logical_dev = NULL;
  rf_common_xpt_buffer_id_type buffer_ids;
  boolean capture_done = FALSE;
  boolean processing_done = FALSE;
  uint8 iq_buffer_index = -1;
  uint8 dpd_buffer_index = -1;
  mcpm_mcvsrequest_parms_type params;
  /* uint32 size = IMOD(EPT_SAMPLE_BUFLEN,4*1024); */
  
  /* Switch TxLM settings ( DP_CFG , IQ_Gain, ENV_SCALE ) to EPT specific settings */
  rfwcdma_mdsp_set_ept_specific_params(TRUE); 
  rfwcdma_msm_set_dpd_state(TRUE); 

 /*if( ept_buffer==NULL ) 
  {
    ept_buffer = modem_mem_alloc( (size + (1 << alignment) - 1),MODEM_MEM_CLIENT_RFA);
    ept_buffer = modem_mem_alloc(size, MODEM_MEM_CLIENT_RFA);
    ept_buffer_backup = ept_buffer;
    ept_buffer = (int)(ept_buffer) & (~(0xFFF));
    ept_buffer = (int)ept_buffer + 4096;
    ept_fw_addr = blast_lookup_physaddr((uint32)ept_buffer);
   ept_fw_addr = (((int) ept_buffer) + (1 << alignment) - 1) & (~0 << alignment);
    
    if( ept_buffer_backup!=NULL)
    {
      FTM_MSG(FTM_HIGH, "In ftm_wcdma_ept_do_iq_capture() , Memory allocation successful " );
      FTM_MSG_2(FTM_HIGH, "Mem_Size:%d , Mem_Address:%d ",size, ept_buffer);
    }
    else
    {
      FTM_MSG(FTM_HIGH, "In ftm_wcdma_ept_do_iq_capture() , Memory allocation FAILURE " );
    }
  }
  else
  {
    FTM_MSG(FTM_HIGH, "In ftm_wcdma_ept_do_iq_capture() , Memory already allocated" );
 }

 if( ept_buffer_backup !=NULL)
  {
   FTM_MSG_3(FTM_HIGH, "In ftm_wcdma_ept_do_iq_capture(),Num_samp:%d, FTrig:%d, LTrig:%d ", 
           iq_capture->ept_iq_capture_num_samples, iq_capture->ept_iq_capture_first_trigger, 
           iq_capture->ept_iq_capture_last_trigger );
  if( iq_capture->ept_iq_capture_first_trigger ==1)
  {
    rfwcdma_mdsp_tlb_mem_map(size, (uint32*)ept_buffer );
  } */

   /* Turn on WTR Feedback path */
  logical_dev = rfc_common_get_logical_device_params ( RFM_DEVICE_0 );
  if( logical_dev!=NULL )
  {
	 ret_val = rfdevice_cmn_set_et_path( logical_dev->cmn_device, RFM_IMT_MODE, RFM_PA_GAIN_STATE_3,TRUE);
  }
  if( ret_val == TRUE )
  {
    ret_val &= rfcommon_xpt_is_capture_done();
    if( ret_val == FALSE)
    {
      FTM_MSG( FTM_ERROR, "ftm_wcdma_ept_do_iq_capture: IQ capture already "
                 "in progress" );
    }

    /* Monitor processing only if it was requested */
    if (do_processing == 1)
    {
      ret_val &= rfcommon_xpt_is_processing_done();
      if( ret_val == FALSE)
      {
        FTM_MSG( FTM_ERROR, "ftm_wcdma_ept_do_iq_capture: IQ processing already "
                   "in progress" );
      }
    }
  }

  /* Set capture and processing flags */
  if( ret_val == TRUE )
  {
    ret_val &= rfcommon_xpt_set_capture_in_progress();
    if (do_processing == 1)
    {
      ret_val &= rfcommon_xpt_set_processing_in_progress();
    }
    if (iq_capture->first_trigger == 1)
    {
      /* Initialize buffer pool for xpt captures */
      rfcommon_core_xpt_init_buffers();
    }
    /* Get buffer ids, TODO: need to pass in do_processing flag to not get dpd result buffer */
    ret_val &= rfcommon_core_xpt_get_buffers(&buffer_ids);
  }
  if ( ret_val == TRUE)
  {
    if ((do_processing==1) && (iq_capture->first_trigger==1))
    {
      rfcommon_mdsp_setup_dpd_proc_params(140,50,40,240,20,80,20,1,1,iq_capture->num_samples);
    }
    /* send the CX bump up */
    if (iq_capture->first_trigger==1)
    {
      /* Setup offline clock to 144MHz for EPT Sample capture */
      params.mcvs_req_type = MCVS_FULL_REQUEST;
      params.modem_clk_bus_KHz = 144000;
      params.mcvs_update_info = MCPM_MCVS_CLKBUS_UPDATE;
      MCPM_MCVSConfig_Modem(MCPM_WCDMA_TECH, &params);
      DALSYS_BusyWait(10000); /* Hard-coded delay before first capture to have enough settling time */
    }
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "Offline speed returned by MCPM:%d", MCPMDRV_Get_Modem_Offline_Clock(0x1));
  
  rfwcdma_mdsp_send_sapt_iq_cap_cmd( ftm_wcdma_rxlm_buffer[RF_PATH_1].buf_idx, 
                                       rx_chain,
                                       tx_chain,
                                       iq_capture->num_samples,
                                       iq_capture->first_trigger,
                                       iq_capture->last_trigger,
                                       do_processing,
                                       buffer_ids.capt_buf_id,
                                       buffer_ids.res_buf_id );
    if (iq_capture->last_trigger==1)
    {
      DALSYS_BusyWait(10000); /* Hard-coded delay after last capture to have enough settling time */
      MCPM_MCVSConfig_Release(MCPM_WCDMA_TECH);
    }
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_ept_do_iq_capture: Buffer allocation failed!" );
  }
  call_apply_dpd = TRUE;

  if( (ret_val == TRUE) )
  {
    capture_done = rfcommon_xpt_wait_for_capture_done( 500, 10000, &iq_buffer_index );
    if( capture_done == FALSE )
    {
      FTM_MSG( FTM_ERROR, "ftm_wcdma_ept_do_iq_capture: Sample "
                 "capture did not complete" );
      ret_val = FALSE;
    }
    else
    {
      /* REMOVE ABOVE EXTERN when removing this */
      /* Log samples in "OLD format" as old structure is still functional */
      rfcommon_mdsp_extract_sapt_iq_cap_data( iq_capture->num_samples );
    }
  }

  /* Wait for processing done */
  if( (ret_val == TRUE) &&
      (do_processing == 1) )
  {
    processing_done = rfcommon_xpt_wait_for_processing_done( 500, 10000, &dpd_buffer_index );
    if ( processing_done == FALSE)
    {
      FTM_MSG( FTM_ERROR, "ftm_wcdma_ept_do_iq_capture: "
               "Processing did not complete" );
      ret_val = FALSE;
    }
  }

  /* release capture buffer, returns false now, TODO: implement deallocation of buffer_id and return true */
  rfcommon_core_xpt_release_capture_buffer(iq_buffer_index);
  /* release results buffer, returns false now, TODO: implement deallocation of buffer_id and return true */
  rfcommon_core_xpt_release_results_buffer(dpd_buffer_index );
  if ( (iq_capture->last_trigger == 1) )
  {
    /* release all buffer ids */
    rfcommon_core_xpt_free_all_buffers();
  }

 /* else
  {
    FTM_MSG(FTM_HIGH," Memory allocation failed , not calling mdSP functions ");
    call_apply_dpd = FALSE;
  } */
   
  return TRUE;
}
#endif

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
ftm_wcdma_calv3_sweep_rx_agc_threshold_reached
(
  ftm_cal_polling_instr_payload_type const *polling_payload,
  void const *event_payload
)
{
  ftm_cal_polling_event_1_payload_type *event_pl = (ftm_cal_polling_event_1_payload_type*)event_payload;

  /* Validation */
  if ( polling_payload == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: NULL Payload" );
    return FALSE;
  } /* if ( polling_payload == NULL ) */

  /* Validation */
  if ( event_payload == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: NULL Event Payload" );
    return FALSE;
  } /* if ( event_payload == NULL ) */

  /* Validation */
  if (    SIGNAL_RISING  != event_pl->direction  
       && SIGNAL_FALLING != event_pl->direction )
  {
    FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: Invalid direction argument" );
    return FALSE;
  } /* if ( SIGNAL_RISING ... ) */

  {
    int16 rx_agc_data_dbm10;  
    int16 rx_level_agc;

    MSG_3(MSG_SSID_FTM, FTM_LOW, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: Reading AGC from device %d (RFM %d), threshold %d dBm10", 
          event_pl->primary_device, ftm_calibration_dev_flag_to_rfm_dev(event_pl->primary_device), event_pl->threshold);

    /* Call RFM function to get current RX AGC*/  
    if( FALSE == ftm_wcdma_get_rx_level_dbm(ftm_calibration_dev_flag_to_rfm_dev(event_pl->primary_device), &rx_level_agc, &rx_agc_data_dbm10))
    {
      FTM_MSG( FTM_ERROR, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: Failed to read RX AGC" );
      return FALSE;
    }

    MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: Current RX reading: %d dBm10", rx_agc_data_dbm10);

    if( SIGNAL_RISING == event_pl->direction )
    {
      MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: Waiting signal to rise above or equal %d dBm10",
            event_pl->threshold);
      if( event_pl->threshold <= rx_agc_data_dbm10 )
      {      
        MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: Rising signal threshold %d dBm10 reached.",
              event_pl->threshold);
        return TRUE;
      }
    }
    else 
    {
      MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: Waiting signal to fall below or equal %d dBm10",
            event_pl->threshold);
      if( event_pl->threshold >= rx_agc_data_dbm10 )
      {
        MSG_1(MSG_SSID_FTM, FTM_LOW, "ftm_wcdma_calv3_sweep_rx_agc_threshold_reached: Falling signal threshold %d dBm10 reached",
              event_pl->threshold);
        return TRUE;
      }
    }
  }

  return FALSE;
} /* ftm_wcdma_calv3_sweep_rx_agc_threshold_reached */


/*----------------------------------------------------------------------------*/
boolean
ftm_wcdma_calv3_sweep_polling_event_init
(
  void
)
{
  boolean ret_val = TRUE;

  ret_val &= ftm_calibration_polling_register_event( RFM_IMT_MODE,
                                           POLLING_EVENT_RXAGC_THRES_REACHED,
                                           ftm_wcdma_calv3_sweep_rx_agc_threshold_reached );

  FTM_MSG( FTM_MED, "ftm_wcdma_calv3_sweep_polling_event_init: Events registered" );

  return ret_val;

} /* ftm_wcdma_calv3_sweep_polling_event_init */

LOCAL rfcom_wcdma_band_type ftm_wcdma_calv4_convert_band(uint16 band)
{
 rfcom_wcdma_band_type ret_band = RFCOM_BAND_INVALID;

 switch(band)
  {
    case WCDMA_FTM_BAND_1:
      ret_band = RFCOM_BAND_IMT;
      break;

    case WCDMA_FTM_BAND_2:
      ret_band = RFCOM_BAND_1900;
      break;

    case WCDMA_FTM_BAND_3:
      ret_band = RFCOM_BAND_BC3;
      break;

    case WCDMA_FTM_BAND_4:
      ret_band = RFCOM_BAND_BC4;
      break;

    case WCDMA_FTM_BAND_5:
      ret_band = RFCOM_BAND_800;
      break;

    case WCDMA_FTM_BAND_8:
      ret_band = RFCOM_BAND_BC8;
      break;

    case WCDMA_FTM_BAND_9:
      ret_band = RFCOM_BAND_BC9;
      break;

    case WCDMA_FTM_BAND_11:
      ret_band = RFCOM_BAND_BC11;
      break;

    case WCDMA_FTM_BAND_19:
      ret_band = RFCOM_BAND_BC19;
      break;

    default:
      ret_band = RFCOM_BAND_INVALID;
      break;
  }
 return ret_band;
} /* ftm_wcdma_calv4_convert_band */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure XPT

  @details
  This function will call RFM function in order to configure FW for
  XPT calibration Mode

  @param payload
  Payload associated with the XPT configuration
  
  @return
  Flag indicating the status of the XPT configure action
*/
boolean
ftm_wcdma_cal_xpt_configure
(
  ftm_cal_xpt_instr_payload_ept_config_type const *payload
)
{
  boolean ret_val = TRUE;
  uint8 device;
  uint8 device_fb;

  /* Start of profile */
  prof_start_time = timetick_get();

  /* NULL pointer check */
  if ( payload == NULL )
  {
    FTM_MSG( FTM_LOW, "ftm_wcdma_cal_xpt_configure : NULL Payload" );
    return FALSE;
  } /* if ( payload == NULL ) */
  else
  {
    device = payload->primary_device;
    device_fb = payload->feedback_device;
    
    /* Initialize buffer pool for xpt captures */
    rfcommon_core_xpt_init_buffers();

    /* Perform EPT Configure */
    ret_val &= ftm_wcdma_xpt_cal_config( device, 
                                         device_fb,
                                         payload->xpt_mode,
                                         TRUE );
   
    /* Initialize data module for the current channel */
    ret_val &= ftm_cal_data_init_cur_channel_map(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);      
     
  } /* if ! ( payload == NULL ) */

  FTM_MSG_2( FTM_LOW, "ftm_wcdma_cal_xpt_configure : [Status %d] Profile %d us ",
             ret_val, timetick_get_elapsed( prof_start_time, T_USEC ) );

  /* Return */
  return ret_val;

} /* ftm_wcdma_cal_xpt_configure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  XPT IQ Capture

  @details
  This function will call RFM function in order to send a command to FW to
  perform XPT IQ Capture

  @param payload
  Payload associated with the XPT IQ Capture
  
  @return
  Flag indicating the status of the XPT IQ Capture action
*/
ftm_rsp_pkt_type
ftm_wcdma_cal_xpt_iq_capture
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
  prof_start_time = timetick_get();

  /* Request Packet */
  payload = &(((ftm_cal_xpt_instr_req_type*)ftm_req_data)->xpt_param);
  /* NULL pointer check */
  if ( payload == NULL )
  {
    FTM_MSG( FTM_LOW, "ftm_wcdma_cal_xpt_iq_capture : NULL Payload" );
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
          FTM_MSG_3(FTM_MED,"ftm_wcdma_cal_xpt_iq_capture: Push to DPD Map - Chan: %d, CaptBuf: %d, ResBuf: %d",
                                            rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
                                            buffer_ids.capt_buf_id,
                                            buffer_ids.res_buf_id);
          /* Create a new data module table using the current channel */
          ret_val &= ftm_cal_data_push_cur_override_to_dpd_map(
                                            rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
                                            buffer_ids.capt_buf_id,
                                            buffer_ids.res_buf_id );
        }

        if (ftm_wcdma_xpt_fb_path_buffer.is_buf_allocated == TRUE)
        {
            samp_rate = rfwcdma_mc_get_samp_rate((uint8)ftm_wcdma_xpt_fb_path_buffer.buf_idx); 
        }
        else
        {
            FTM_MSG(FTM_ERROR,"ftm_wcdma_cal_xpt_iq_capture: rxlm has not been allocated yet");
            ret_val = FALSE;
        }

        /* Send command to perform IQ capture */
        iq_cap_status =  ftm_wcdma_xpt_iq_capture(payload->primary_device,
                                                  payload->feedback_device,
                                                  payload->capture_type,
                                                  payload->num_samples,
                                                  payload->first_trigger,
                                                  payload->last_trigger,
                                                  ftm_calibration_xpt_map_proc_type(payload->proc_flag),
                                                  buffer_ids,
                                                  &samp_rate);

        ret_val &= iq_cap_status;

        /* if xpt_iq_capture failed, unvote modem bus clk immediately;
           otherwise, modem bus clk will be unvoted at capture done response
           when last_trig is true*/
        if ( iq_cap_status == FALSE )
        {
          /* FBRx Mode IQ Captures use different call flow to setup WTR and LM */
          if ( payload->capture_type == XPT_CAPTURE_FBRX )
          {
            // do nothing
            FTM_MSG(FTM_HIGH, "ftm_wcdma_cal_xpt_iq_capture: FBRx type IQ Capture failed,"
                                 " use feedback_setup api to turn off FB device");
          }
          else
          {
          ftm_wcdma_xpt_cal_config( payload->primary_device, 
                                    payload->feedback_device,
                                      (uint8)FTM_XPT_CONFIG_EPT,
                                    FALSE );
          }
        }

      } /* if ( buff_alloc == TRUE ) */
      else
      {
        ret_val = FALSE;
        FTM_MSG( FTM_ERROR, "ftm_wcdma_cal_xpt_iq_capture: Buffer could not "
                            "be allocated" );
      } /* if ! ( buff_alloc == TRUE ) */

    } /* if ( rfcommon_xpt_is_capture_done() == TRUE ) */
    else
    {
      FTM_MSG( FTM_ERROR, "ftm_wcdma_cal_xpt_iq_capture: Previous IQ capture"
                          "is not complete" );
    } /* if ! ( rfcommon_xpt_is_capture_done() == TRUE ) */
  } /* if ! ( payload == NULL ) */

  /* Print debug msg if enabled */
  if(ftm_wcdma_calv3_dbg_msg_flag)
  {
  FTM_MSG_2( FTM_LOW, "ftm_wcdma_cal_xpt_iq_capture : [Status %d] Profile %d us ",
             ret_val, timetick_get_elapsed( prof_start_time, T_USEC ) );
  }
  /* Return */
  return ftm_rsp_data;
} /* ftm_wcdma_cal_xpt_iq_capture */


/*----------------------------------------------------------------------------*/
void ftm_wcdma_calv3_set_cal_flag(boolean cal_flag)
{
  wcdma_calv3_in_progress = cal_flag;
  FTM_MSG_1(FTM_HIGH, "setting calv3 flag to %d", wcdma_calv3_in_progress );
}



/*----------------------------------------------------------------------------*/
boolean ftm_wcdma_calv3_get_cal_flag(void)
{
  //FTM_MSG_1(FTM_HIGH, "ftm_wcdma_calv3_get_cal_flag %d", wcdma_calv3_in_progress );
  return wcdma_calv3_in_progress ;
}


/*!
  @brief
  Returns rx settling time for agc or NBEE
 
  @details
  This function returns settling time for RXAGC in case of normal calibration and
  NBEE settling time in the case of optimized RXCAL
 
  @return
  Settling time in usec
*/

uint16 ftm_wcdma_calv3_get_rx_settling_time(void)
{

  if( ftm_wcdma_rxcal_opt_enabled() )
    return ftm_wcdma_rx_nbee_settling_time;
  else
    return FTM_WCDMA_CALV3_RX_AGC_SETTLING_TIME;

}
