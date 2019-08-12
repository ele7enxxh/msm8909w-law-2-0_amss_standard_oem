#ifndef WL1API_H 
#define WL1API_H

/*==========================================================================
             WCDMA L1 Public header file

DESCRIPTION
  This file contains definition, declaration required for to be exposed
  from WCDMA L1 as Public.

  Copyright (c) 2011 - 2011 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/wl1api.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
-------    ---    ---------------------------------------------------------- 
09/16/15   sr     Initial checkin for FR 26957 - TxAGC logging
06/23/14   gsk    Compilation error 
04/14/14   cc     Create an API for RF to call idle W2X search cfg to WFW in FTM mode
03/17/14   mk     Removed featurization FEATURE_WCDMA_BOLT_2_0 
01/09/14   pr     Code changes for UL Compression Feature
10/02/13   gp     Add dummy dpcch indication for ul cfg dpcch cmd
09/29/13   as     Continuing port of Dual Sim from Triton to Dime.
07/29/13   sks    Inclusion of QDSS profile points for timelining.
07/24/13   jd      Changes for Dual band Dual carrier
06/06/13   pr     Changed rxlmDynGroupMask type from uint8 to uint32
05/22/13   oh     Adding DC-HSUPA framework support
05/23/13   geg    WSW decoupling (except for component of QCHAT)
04/25/13   jd     Removing Obsolete params for Wcdma sample capture  
04/17/13   pr     Add support for RF cal to WFW
04/04/13   at     Cleanup FEATURE_WCDMA_TRITON_MCAL_API featurization
02/04/13   jd     Update WCDMA xpT IQ capture API to support passing buffer ids for capture and results.  
02/10/13   pv     Fix the parameters for AGC cmd for TRITON.
02/05/13   pv     Fixed compile issue. 
02/05/13   pv     Featurize out FR 2001 for Triton. 
02/05/13   pv     Support for FR 2001.
02/05/13   ms     Added support for HS RxLM
01/17/13   kr     externed a function need to make a voice request.
01/17/13   at     Added ET Txlm cfg cmd to turn on Tx on chain0 in ftm mode
01/02/13   pr     Made mdsp_sync_send_rf_cfg_cmd , mdsp_sync_send_rf_init_cmd and
                        mdsp_sync_iq_sample_capture_cmd as public API's for RF.
10/09/12   sj     Added rxlm buf idx for div chain in send rxagc cmd
07/24/12   geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/24/12   mmr    Added mdsp_sync_send_iq_cap_cmd()
04/05/12   gsk    Adding wl1_check_state_for_delayed_gstmr_int 
03/09/12   uk     Adding API wl1_is_camped_in_idle 
11/30/11   rc     Added mdsp_sync_send_lpm_agc_cmd for DSR support
09/07/11   kcm    Added function MDSP_SYNC_SEND_DAC_CAL for Tx DAC Cal
07/13/11   mk     Exposed the mdsp_sync_send_sample_server_cmd declaration 
                  outside of W
05/31/11   geg    Changes for RRC and L2 api files clean-up wcdma dependencies
04/27/11   mc     Exposed mdsp APIs for CMI compliance
04/27/11   oh     Move the func. prototypes for TxLM Cfg cmd to wl1api.h 
04/26/11   mc     Replaced MCAL API with WHL1 API for querying freqcommon
04/21/11   mc     Created this WL1 public API

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "sys_type.h"
#include "ds3g_ext.h"

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define W_ARFCN_T ARFCN_T

/*------------------------------------------------------------*/
/*  Defintions used by the sleep controller SW                */
/*------------------------------------------------------------*/

#define ONE_FRAME_IN_CX8           (307200)

/* ---------------------------------------------------------- */
/* Definition to query HS-DPCCH beta and feedback information */
/* ---------------------------------------------------------- */

#define HS_DPCCH_GET_MEAS_FEEDBACK_INFO(dpcch_ptr, field) (dpcch_ptr)->meas_feedback_info.field
#define HS_DPCCH_GET_BETA_HS_INFO(dpcch_ptr, field) (dpcch_ptr)->beta_hs_info.field

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/*--------------------------------------------------------------------------
                         TYPEDEF: CALLBACK_FN

This defines the callback_fn type which is a pointer to a function which
takes no parameters and returns a void.
--------------------------------------------------------------------------*/
typedef void (*post_cb_fn)(uint16 status);
typedef void (*pre_cb_fn)(void);

/* Enum type declaration */
/* --------------------- */
typedef enum {
AGC_CMD_NOOP,
AGC_CMD_DISABLE,
AGC_CMD_ENABLE
}agc_cfg_action_enum_type;

/* this enum type is for DC-HSUPA*/
typedef enum
{
	/* No operation */
	UL_MC_WAIT_SCCH_ORDER_NOOP,
	/* Wait for SCCH order*/
	UL_MC_WAIT_SCCH_ORDER_TRUE,
	/*Do not wait for SCCH order*/
	UL_MC_WAIT_SCCH_ORDER_FALSE,
} wlulcm_wait_ScchOrderSec_type;

/* This enum is used exclusively for specifying an argument into 
   mdsp_send_idle_mode_w2x_cfg_cmd().  This enum should mirror the enum
   in the sw/fw interface file used for the mode field such that the 
   numbers match. */
typedef enum
{
  MDSP_SYNC_W2X_IDLE_CMD_W2G,
  MDSP_SYNC_W2X_IDLE_CMD_W2L,
  MDSP_SYNC_W2X_INVALID_CMD = 0xFF
} mdsp_sync_w2x_idle_cmd_mode_enum_type;

/* Structure type declaration */
/* -------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* defines for the TrCh ref type bit field */
#define L1_DL_TRCH_REFTYPE_SBCH_BF 0x01
#define L1_DL_TRCH_REFTYPE_NBCH_BF 0x02
#define L1_DL_TRCH_REFTYPE_DCH_BF  0x04
#define L1_DL_TRCH_REFTYPE_DSCH_BF 0x08
#define L1_DL_TRCH_REFTYPE_PCH_BF  0x10
#define L1_DL_TRCH_REFTYPE_FACH_BF 0x20

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_SIM_WCDMA_TUNEAWAY)
/*===========================================================================
FUNCTION     wl1_dsds_allow_geran_xomgr_update

DESCRIPTION
  This is a query function used by GERAN stack on other SUB to determine if
  it can update XOMGR with the frequency error. If not, W will maintain the 
  frequency error.

DEPENDENCIES
  GERAN should use this API only in DSDS mode.

PARAMETERS
  Duration for which GERAN has TRM lock, if not known GERAN should populate this 
  param with the value 0xFFFFFFFF

RETURN VALUE  
  TRUE - Allow GERAN to update frequency error
  FALSE - Do not allow GERAN to update frequency error

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_dsds_allow_geran_xomgr_update(uint32 lock_held_ms);
#endif /* #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_SIM_WCDMA_TUNEAWAY) */

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_AGC_CMD

DESCRIPTION     This function sends the AGC command to the mDSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean mdsp_sync_send_rxagc_cmd
(
  /* AGC command config parameters */
  void *agc_cfg,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_RXCAL

DESCRIPTION     This function sends the RXCAL command to FW

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean mdsp_sync_send_rxcal_cmd
(
     /* 00: No Action; 01: disable; 10: Enable */
  agc_cfg_action_enum_type configure_agc0_carr0,

  /* 00: No Action; 01: disable; 10: Enable */
  agc_cfg_action_enum_type configure_agc0_carr1,

  /* 00: No Action; 01: disable; 10: Enable */
  agc_cfg_action_enum_type configure_agc1_carr0,

  /* 00: No Action; 01: disable; 10: Enable */
  agc_cfg_action_enum_type configure_agc1_carr1,

  /* RxLM buffer index */
  uint32 buf_idx,

  /* RxLM buffer index for diversity */
  uint32 buf_idx_rxd,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb,

  uint16 rfWarmupTime,

  boolean rfWarmupFlag
);

/*===========================================================================

FUNCTION demoddrv_retrieve_freqcommon

DESCRIPTION
  This function retrieves averaged out frequency error

DEPENDENCIES
  None.

RETURN VALUE
  int16 freq common error read from fw

SIDE EFFECTS
  None

===========================================================================*/
extern int16 demoddrv_retrieve_freqcommon(void);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_MOD_TIMING_CMD

DESCRIPTION     This function will send the "set modulator timing relation"
                command to the mDSP.

                Note that two parameters are pre_cb, and post_cb which are
                function pointers to a callback that will be called
                immediately prior to sending the command and immediately
                after the command done interrupt is called.  As these can
                be called in interrupt context they are expected to be very
                skinny functions.  If substantial pre-processing needs to be
                done it should be done at task context prior to calling
                mdsp_sync_send_mod_timing_cmd().  If substantial
                post-processing needs to be done then the post_cb function
                should send a command to the L1 local queue and that command
                handler should do the post-processing.  Look at mdsp_sync.h
                for definitions of the pre_cb_fn and post_cb_fn types.  The
                post-processing callback takes the command status as returned
                by the mDSP as a parameter.


DEPENDENCIES    This will fail if the L1 sync command queue is full, however
                that is probably considered to be a fatal error.

RETURN VALUE    TRUE if the command was sent, FALSE otherwise.  Only reason
                that the command won't sent is that the mDSP command queue
                is full which should never happen.

SIDE EFFECTS    None

===========================================================================*/
boolean mdsp_sync_send_mod_timing_cmd
(
  /* slam or slew to the channel timing */
  boolean slam,

  /* which hw channel contains the downlink physical channel that the uplink
     channel whose timing relation is being set derives it's timing from */
  uint8 phch,

  /* Absolute Tx PN position (DL DPCH timing + T0) */
  uint32 txPnPositioncx8,

  /* the value of the TTI counter must be set to when this command takes
     effect */
  uint8 tti,

  /* DL reference offset */
  int8 dl_ref_offset,

  /* the post callback function, may be NULL */
  post_cb_fn post_cb,

  /* the pre callback function, may be NULL */
  pre_cb_fn pre_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_UL_DPCCH_CFG_CMD

DESCRIPTION     This function sends the Uplink DPCCH config command to the
                mDSP.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    The mDSP Uplink DPCCH config command will be sent to the mDSP.

===========================================================================*/
boolean mdsp_sync_send_ul_dpcch_cfg_cmd
(
  /* enable or disable DPCCH processing */
  boolean ena,

  /* dummy or real dpcch cfg */
  boolean inSyncDetect,
  
  /* slot format to use */
  uint16 slot_format,

  /* the uncoded TFCI bits */
  uint16 uncoded_tfci,

  /* Waitforscchorder */
  wlulcm_wait_ScchOrderSec_type waitforscchorder,

  /* Disable secondary carrier */
  boolean disableseccarrier,

  /* the pre-command callback function */
  pre_cb_fn pre_cb,

  /* the post-commadn callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_TXLM_CFG_CMD

DESCRIPTION     This function sends the TxLM config command to
                the mDSP. Invoked from RF SW after auto gen code setup is done,
                during enable. For disable cmd is invoked from WL1.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    The mDSP EUL EDPCH config will be sent to the mDSP.

===========================================================================*/
extern boolean mdsp_sync_send_txlm_cfg_cmd(
  /*TxLM mode 1: enable, 0:disable*/
  boolean mode_enable,
  /* TxLM buffer index */
  uint32 buf_idx,
   /* the pre-command callback function */
  pre_cb_fn pre_cb,
  /* the post-commadn callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_ET_TXLM_CFG_CMD

DESCRIPTION     This function sends the ET TxLM config command to
                the mDSP. Invoked from RF SW after auto gen code setup is done,
                during enable. For disable cmd is invoked from WL1.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    The mDSP EUL EDPCH config will be sent to the mDSP.

===========================================================================*/
extern boolean mdsp_sync_send_et_txlm_cfg_cmd(
  /*TxLM mode 1: enable, 0:disable*/
  boolean mode_enable,
  /* TxLM buffer index */
  uint32 buf_idx,
   /* the pre-command callback function */
  pre_cb_fn pre_cb,
  /* the post-commadn callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_TXLM_UPDATE_CMD

DESCRIPTION     This function sends the TxLM config command to
                the mDSP. Invoked from RF SW for dynamic updates.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    The mDSP EUL EDPCH config will be sent to the mDSP.

===========================================================================*/
extern boolean mdsp_sync_send_txlm_update_cmd
(
  /* TxLM buffer index */
  uint32 buf_idx,
  /* the pre-command callback function */
  pre_cb_fn pre_cb,
  /* the post-commadn callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_DAC_CAL

DESCRIPTION     This function sends the command to the mDSP to start   
                Tx DAC cal. Invoked from FTM (Factory Test Mode) of RF during
                Tx Calibration of UE.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    None

===========================================================================*/
extern boolean mdsp_sync_send_tx_dac_cal
(
  /* Whether to do dac cal on ant 0 */
  boolean dac_cal_ant0,

  /* Whether to do dac cal on ant 1 */
  boolean dac_cal_ant1,

  /* the pre-command callback function */
  pre_cb_fn pre_cb,

  /* the post-commadn callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_SAMPLE_SERVER_CMD

DESCRIPTION     This function formats and sends a sample server freeze\unfreeze 
                command to the mDSP by way of the mDSP command queue.

                Note the caller does not have to worry about putting the
                command buffer back on the free q.  That is handled by
                the L1 sync module.

DEPENDENCIES    None

RETURN VALUE    TRUE if the command is sent, FALSE otherwise.  The function
                can return FALSE if there are no free command buffers available.

SIDE EFFECTS    A command will be either placed on the mDSP command queue
                or will be immediately sent to the mDSP.

===========================================================================*/

extern boolean mdsp_sync_send_sample_server_cmd
(
  /* Indicates whether to freeze SS */
  boolean freeze,
  /* Indicates whether to unfreeze SS */
  boolean unfreeze,
  /* Indicates whether to apply freeze/unfreeze action to chain 0 */
  boolean chain0,
  /* Indicates whether to apply freeze/unfreeze action to chain 1 */
  boolean chain1,
  /* Indicates the action to be taken with RTR after sample server cmd action time.*/
  boolean rfPwrUpdateFlag,
  /* Indicates the STMR Cx1 value for the action time for the freeze/unfreeze.*/
  uint16 actionTime,
  /* pre-command callback */
  pre_cb_fn pre_cb,
  /* post-command callback */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_LPM_AGC_CMD

DESCRIPTION     

DEPENDENCIES    None

RETURN VALUE    

SIDE EFFECTS    

===========================================================================*/
extern void mdsp_sync_send_lpm_agc_cmd(uint8 rxlm_buf_idx,
                                       uint8 rxlm_buf_idx_rxd,
                                       uint8 linearity_mode,
                                       pre_cb_fn pre_cb,
                                       post_cb_fn post_cb);

/*===========================================================================

FUNCTION        WL1_IS_CAMPED_IN_IDLE

DESCRIPTION     

DEPENDENCIES    None

RETURN VALUE    
       True - If L1m state is PCH or PCH_SLEEP
       False - Otherwise
 
SIDE EFFECTS    

===========================================================================*/
extern boolean wl1_is_camped_in_idle(void);

/*===========================================================================

FUNCTION      wl1_check_state_for_delayed_gstmr_int

DESCRIPTION
  This function controls weather the gstmr delayed error message can be printed 
  or not. If returns TURE error message can be printed else skipped.
  
DEPENDENCIES
  None

RETURN VALUE
   TRUE or FALSE

SIDE EFFECTS

===========================================================================*/
extern boolean wl1_check_state_for_delayed_gstmr_int(void); 
/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_IQ_CAP_CMD

DESCRIPTION     This function sends the start IQ capture command to the mDSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean mdsp_sync_send_iq_cap_cmd
(
  /* RxLm Buffer Id */
  uint32 rxlm_buffer_id,

  /* Rx Chain to start the capture on */
  uint32 chain,

  /* Time to begin the capture */
  uint32 start_time,

  /* Carrier information */
  uint16 carrier,

  /* Number of samples to capture */
  uint16 num_samples,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);
/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_RF_CFG_CMD

DESCRIPTION     This function sends the RF CFG command to the mDSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean mdsp_sync_send_rf_cfg_cmd
(
  uint32 event_id,

  uint32 offset,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);
/*===========================================================================

FUNCTION        MDSP_SYNC_IQ_SAMPLE_CAPTURE_CMD

DESCRIPTION     This function sends the IQ Sample Capture command to
                the mDSP. Invoked from RF SW for dynamic updates.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    

===========================================================================*/

extern boolean mdsp_sync_iq_sample_capture_cmd(
  /* flag to do initialization */
  uint32 first_trig,

  /* flag to do de-initialization */
  uint32 last_trig,

  /* sequence id of command sent to common FW */
  uint32 seq_id,

  /* capture buffer id for use by common FW */
  uint8  capt_buf_id,

  /* results buffer id for use by common FW */
  uint8  res_buf_id,

  /* the pre-command callback function */
  pre_cb_fn pre_cb,

  /* the post-commadn callback function */
  post_cb_fn post_cb);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_RF_INIT_CMD

DESCRIPTION     This function sends the RF CFG command to the mDSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean mdsp_sync_send_rf_init_cmd
(
  uint32 event_id,

  uint32 offset,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);
/*===========================================================================
FUNCTION     WMCPMDRV_RF_FTM_MODE_REQ

DESCRIPTION
Wrapper function for honoring voice requestfrom RF to mcpm when in FTM mode.

PARAMETERS
TRUE or FALSE

RETURN VALUE
None

SIDE EFFECTS
None.
===========================================================================*/
extern void wmcpmdrv_rf_ftm_mode_req(boolean request);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*===========================================================================
FUNCTION     WL1TRM_CAN_GSM_DISABLE_FW_APP_WCDMA
 
DESCRIPTION
  GL1 calls this function to check whether or not it may disable FW_APP_WCDMA.
 
DEPENDENCIES
  None.
 
PARAMETERS
  None.
 
RETURN VALUE
  TRUE  - GSM may disable FW_APP_WCDMA.
  FALSE - GSM may NOT disable FW_APP_WCDMA.
===========================================================================*/
extern boolean wl1trm_can_gsm_disable_fw_app_wcdma(void);
#endif /* #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA) */

/*===========================================================================

FUNCTION        mdsp_sync_send_rxlm_dyn_update_cmd

DESCRIPTION     This function sends the RF rxlm dynamic update command to the mDSP
                for ici coeff to switch from r99 to HS
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean mdsp_sync_send_rxlm_dyn_update_cmd
(
  uint8 rxlmBufferId[],

  uint32 rxlmDynGroupMask,

  uint32 rxAntennaMask,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        wl1_get_tlm_tx_sys_time_cx8_qty

DESCRIPTION     This function returns the cx8 qty of Tx system time

DEPENDENCIES

RETURN VALUE    Int32

SIDE EFFECTS

===========================================================================*/
extern int32 wl1_get_tlm_tx_sys_time_cx8_qty(void);

typedef enum
{
  /* PICH ISR */ 
  WL1_PICH_ISR,

  /* PICH Setup Done */ 
  WL1_PICH_SETUP_DONE,

  /* Before DRX(Wakeup) request to MCPM */ 
  WL1_BEF_MCPM_DRX_REQ,

  /* After DRX(Wakeup) request to MCPM */ 
  WL1_AFT_MCPM_DRX_REQ,

  /* Before W2G request to MCPM */ 
  WL1_BEF_MCPM_W2G_REQ,

  /* After W2G request to MCPM */ 
  WL1_AFT_MCPM_W2G_REQ,

  /* Before MCPM request for all other clients at sleep */
  WL1_BEF_MCPM_OTHER_REQ,

  /* Before DRX relinquish at sleep */ 
  WL1_BEF_MCPM_DRX_REL,

  /* After DRX relinquish at sleep */ 
  WL1_AFT_MCPM_DRX_REL,

  /* Before Reacq start */ 
  WL1_BEF_REACQ,

  /* Search WakeUp Done */ 
  WL1_SRCH_AWAKE,

  /* DL Awake Done */ 
  WL1_DL_AWAKE,

  /* DL ready for Sleep */ 
  WL1_DL_READY_FOR_SLEEP,

  /* Search Ready for Sleep */ 
  WL1_SRCH_READY_FOR_SLEEP,

WL1_BEF_RGS_UPDATE,

WL1_AFT_RGS_UPDATE,

WL1_BEF_SRCH_ENTITY_DROP,

WL1_AFT_SRCH_ENTITY_DROP,

  /* After AGC settling done */ 
  WL1_AFT_AGC,

  /* Start of ols */ 
  WL1_BEF_OLS,

  /* Start of MICRO WakeUp */ 
  WL1_BEF_MWU,

  /* End of MICRO WakeUp */ 
  WL1_AFT_MWU,

  /* Start of Sleep Start Int */ 
  WL1_BEF_GTS,

  /* End of Slp Start Int */ 
  WL1_AFT_GTS,

  /* End of Sleep Drv Setup */ 
  WL1_SLP_DRV_STP_FINISH,

  /* Before RF wakeup */ 
  WL1_BEF_RF_ON,

WL1_AFT_RF_ON,

WL1_BEF_RFM_PWR_CLK_ON,

WL1_AFT_RFM_PWR_CLK_ON,

  /* Before RF Off */ 
  WL1_BEF_RF_OFF,

WL1_BEF_FW_SLP,

WL1_AFT_FW_SLP,
  
WL1_FAST_CELL_DECFG_CONFIGURED,

WL1_FAST_CELL_DECFG_ST,  

 WL1_FAST_CELL_DECFG_END, 
  
 WL1_FAST_CELL_DECFG_DONE_WAIT,

 WL1_EDRX_HS_TEARDOWN_START,

 WL1_EDRX_HS_TEARDOWN_END,

 WL1_EDRX_HS_ADD_START,

 WL1_EDRX_HS_ADD_END,

 WL1_EDRX_RX_BURST_START,

 WL1_RF_WKUP_VREG_ON,

 WL1_RF_WKUP_HAL_BUS_EN,

 WL1_RF_WKUP_CORE_CONFIG_DONE,

 WL1_RF_WKUP_RFC_COMMON_WKUP,

 WL1_RF_WKUP_CORE_POWER_ON,

 WL1_RF_WKUP_INIT_CMDS_DONE,

 WL1_RF_WKUP_MEAS_SCRIPTS_BUILT,

 WL1_RF_WKUP_ASM_WAKEUP_DONE,

 WL1_RF_WKUP_TUNER_INIT_DONE,

 WL1_RF_WKUP_BEF_AGC_CMD,
 WL1_WFW_START_CMD_DONE,

 WL1_RF_WKUP_GRFC_SCRIPTS_DONE,

 WL1_RF_WKUP_TUNER_RX_PROGRAM_DONE,

 WL1_MCPM_STMR_BLOCK_CB_START,

 WL1_MCPM_STMR_BLOCK_CB_END,

 WL1_RFM_PWR_CLOCK_ON_START,

 WL1_RFM_PWR_CLOCK_ON_END,

 WL1_NUM_POINTS
} wl1_prof_name_type;


extern void drx_pwr_strobe_toggle_alt_gpio(wl1_prof_name_type sender);

/*===========================================================================
FUNCTION        MDSP_SEND_FTM_IDLE_MODE_W2X_CFG_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function is used for RF FTM to call W2X inter-rat searches 
                from idle modes.
                

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/

void mdsp_send_ftm_idle_mode_w2x_cfg_cmd(
  /* the rxlm buffer index used to tune back the primary Rx chain to WCDMA at
     the end of an idle w2x search */
  uint32 rxlm_home_buf_idx,
  /* the rxlm buffer index used to tune back the diversity Rx chain to WCDMA at
     the end of an idle w2x search */
  uint32 rxlm_home_rxd_buf_idx,
  /* determines if the search is w2l or w2g */
  mdsp_sync_w2x_idle_cmd_mode_enum_type mode,
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);

/*===========================================================================
FUNCTION wl1_stmr_get_current_rx_tl_time

DESCRIPTION
  This function returns the tick count on RX TL 

DEPENDENCIES
  None

RETURN VALUE
  uint32 representing TICK Count on Rx TL

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 wl1_stmr_get_current_rx_tl_time(void);

/*===========================================================================
FUNCTION wl1_l1m_cphy_setup_in_progress

DESCRIPTION
  This function returns the status of l1m_cphy_setup_in_progress

DEPENDENCIES
  None

RETURN VALUE
  boolean value of l1m_cphy_setup_in_progress

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_l1m_cphy_setup_in_progress(void);

/*===========================================================================
FUNCTION wl1_config_modem_power_stats_tx

DESCRIPTION
  This function enables tx duration logging

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_config_modem_power_stats_tx(boolean start_stop);

/*===========================================================================
FUNCTION wl1_log_tx_duration

DESCRIPTION
  This function fills the tx duration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_get_modem_power_stats(uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX]);

#endif /* WL1API_H */


