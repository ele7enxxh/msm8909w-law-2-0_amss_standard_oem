

#ifndef WL1ULMC_H
#define WL1ULMC_H

//#ifdef FEATURE_WCDMA_DC_HSUPA

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      WL1 UPLINK MULTI-CARRIER MANAGER                   

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2013 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



--------   ---     ----------------------------------------------------------
08/08/14    sks    MCPM driver code cleanup.
06/24/14    pkg    Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag.
06/23/14    yw     Ensure no 2 UL DPCCH config command is sent to FW in the same CFN
06/08/14    ar     Stop blocking QTA when Sec PCP remaining frame is only 2 left 
05/27/14    yw     QTA DC-HSUPA initial checkin: Block QTA during secondary PCP and SRB
05/21/13    ymu    Remove Prep init for BI 2.0
05/13/14    ymu    Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
10/10/13    yw     Fix for SRB delay incorrectly applied during secondary carreir activation
08/07/13    yw      Fix call drop issue during DC to SC recfg
07/11/13    ymu    Support for secondary carrier OOS and recfg.
07/02/13    yw     Send Sec UL DPCCH disable cfg command from enc_get_data ISR.
06/25/13    yw     define macro IS_SEC_CARR_UL_CFGD_OR_ACTIVE
06/05/13    ar     CR470879 : Checked in BOLT Compilation changes.
                   Removed redundant enumeration for carrier index
05/22/13    oh     Adding DC-HSUPA feature framework support. 
11/20/12   saik    Created file with support for basic functionality


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "rex.h"
#include "task.h"
#include "msm.h"
#include "mdsputil.h"
#include "mdspsync.h"
#include "stmr.h"
#include "seq_v.h"
#include "l1mathutil.h"
#include "l1def.h"
#include "l1msetup.h"
#include "l1internalmeas.h"

#include "ulstates.h"
#include "ulcmd.h"
#include "ul.h"

/* Renamed enc.h to wenc.h*/
#include "wenc.h"
#include "./wenci.h"

#include "ullog.h"
#include "dlchmgr.h"
#include "rfm.h"
#include "wsrch.h"

#include "wl1rfmif.h"
#include "ran.h"

#include "srchfach.h"

#if !defined(FEATURE_DIAG_NO_EVENTS)
#include "event.h"
#include "event_defs.h"
#endif
#include "l1cmmgr.h"
#include "demoddrv.h"

#if (defined (FEATURE_WCDMA_PLT) && !defined (FEATURE_MSM_FFA_CONFIG) && !defined(FEATURE_WPLT_RXTYPE_NVCONFIG))
#error code not present
#endif

#ifdef FEATURE_WCDMA_L1_TM_CIPHER_SUPPORTED
#include "msm.h"
#endif

#include "dldemlog_v.h"

#ifndef FEATURE_WCDMA_PLT
#include "rrcscmgr.h"
#else
#error code not present
#endif

#include "hscfg.h"
#include "enchs.h"

#include "euledpch.h"
#include "eulcfg.h"
#include "eulsg.h"
#include "edlcfg.h"
#include "wsrchi.h"
#include "mdspasync.h"

#include "l1macif.h"

/* Includes declaration for winddown_detected */
#include "dlolpc.h"

#ifdef FEATURE_QCHAT
#include "l1_qchatupk.h"
#endif /* FEATURE_QCHAT */

#include "npa.h"

#include "tlm.h"

#ifdef FEATURE_WCDMA_HS_FACH_DRX
#include "wl1uledrx.h"
#endif

#include "wl1xlm.h"

#include "DDIChipInfo.h"
#include "DALDeviceId.h"

#ifdef FEATURE_WCDMA_HS_RACH
#include "wl1ulchmgr.h"
#include "wl1hsrachcntrlr.h"
#include "mcalwcdma_hsrach.h"
#include "dltrchmeas.h"
#include "srchcr.h"
#endif /*FEATURE_WCDMA_HS_RACH*/


rex_crit_sect_type   wl1_ul_mcmgr_mutex;

#define WL1ULMCMGR_MUTEXLOCK() REX_ISR_LOCK(&wl1_ul_mcmgr_mutex)
#define WL1ULMCMGR_MUTEXFREE() REX_ISR_UNLOCK(&wl1_ul_mcmgr_mutex)
#define IS_SEC_CARR_UL_CFGD_OR_ACTIVE() (ul_dpch_carrier_state[WL1_SEC_UL_CARR_IDX] != UL_MC_DPCH_STATE_INIT)

#define IS_SEC_CARR_UL_ACTIVE() ((ul_dpch_carrier_state[WL1_SEC_UL_CARR_IDX] == UL_MC_DPCH_STATE_TX)\
                                  || (ul_dpch_carrier_state[WL1_SEC_UL_CARR_IDX] == UL_MC_DPCH_STATE_PCP_WAIT))
                                    
#define IS_SEC_CARR_UL_IN_PCP() (ul_dpch_carrier_state[WL1_SEC_UL_CARR_IDX] == UL_MC_DPCH_STATE_PCP_WAIT)
#define IS_SEC_CARR_UL_IN_TX() (ul_dpch_carrier_state[WL1_SEC_UL_CARR_IDX] == UL_MC_DPCH_STATE_TX)


                                    
#define IS_SEC_CARR_UL_WAIT_FOR_SCCH_ORDER() (ul_dpch_carrier_state[WL1_SEC_UL_CARR_IDX] == UL_MC_DPCH_SCCH_ORDER_WAIT)                         

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
#define CFNS_QTA_ALLOWED_PCP 2
#define CFNS_QTA_ALLOWED_SRB 1
#endif



l1_ul_dpch_cfg_struct_type  sync_primary_uplink_carrier_dpch_cfg;

extern const uint8 ul_dpcch_slot_fmt_a_idx[L1_UL_MAX_DPCCH_SLOT_FMT];

extern const uint8 ul_dpcch_slot_fmt_b_idx[L1_UL_MAX_DPCCH_SLOT_FMT];

typedef enum
{
  /*  DPCH state when in non-DCH state */
  UL_MC_DPCH_STATE_INIT,

  /* DPCH cfg information from RRC received,
  ** but waiting for SCCH order from n/w to activate
  ** DPCH transmission
  */
  UL_MC_DPCH_STATE_CONFG_WAIT_FOR_ACTV, 
  /* DPCH activation order received, waiting for
  ** DL SW to request DPCH Tx enable, based on sync
  ** procedures.
  */
  UL_MC_DPCH_STATE_ACTV_WAIT_FOR_TX_ENABLE,
  /* DPCH is in PCP wait state, no RAB or SRB data tx allowed,
  ** UL sync procedure in progress.
  */
  UL_MC_DPCH_STATE_PCP_WAIT,

  /* DPCH regular transmission ongoing
  ** both RAB and SRB data can be txed.
  */
  UL_MC_DPCH_STATE_TX,
  /* Carrier is out of sync, Tx suspended until in sync declared */
  UL_MC_DPCH_STATE_TX_SUSPEND,
  /* State for secondary/dependent carrier to halt any Tx until
  ** primary carrier sync achieved. Specifically used in inter/IRAT HHO 
  ** fail and resume cases.
  ** Forced sync A ????
  ** Intra-f HHO with sync procedure ????
  ** 
  */ 
  UL_MC_DPCH_SCCH_ORDER_WAIT,
  /* */
  UL_MC_NUM_DPCH_STATES
} ul_mc_dpch_state_enum_type;


typedef struct
{
  l1_ul_dpch_cfg_struct_type  uplink_dpch_cfg;
  boolean maintain_dpcch_init_tx_pwr;
  boolean pending_state_transition;
  boolean pending_recfg_param_write_to_wfw;
  boolean pending_tx_resume;
  boolean pending_tx_suspend;
  ul_mc_dpch_state_enum_type pending_state;
  ul_mc_dpch_state_enum_type state_at_suspend;
  int16 dpcch_init_tx_pwr_current;
  int16 dpcch_init_tx_pwr_previous;
  /* DPCCH slot format */
  uint8 ul_dpcch_slot_fmt;
  uint8 ul_dpcch_slot_fmt_nm;
  /* CM delta */
  uint32 cm_delta;
  uint32 Npilot_c;
  uint32 Npilot_n;
  uint32 Nslots_c;
  /* Beta params */
  uint32 Beta_c;
  uint32 Beta_d;
  /* Enable carrier state machine */
  boolean enable_state_mc;
  /* Wait for prim sync*/
  boolean wait_for_prim_sync;
  /* Activate DPCH pending */
  boolean activation_pending;
}uplink_carrier_cfg_struct_type;

uplink_carrier_cfg_struct_type uplink_carrier_cfg[WL1_MAX_UL_CARR_IDX];

l1_cm_ul_param_struct_type ul_mc_cm_params[WL1_MAX_UL_CARR_IDX];

uint16 pcp_frm_cnt[WL1_MAX_UL_CARR_IDX];
uint16 srb_frm_cnt[WL1_MAX_UL_CARR_IDX];

uint8 uplink_dpch_tx_enable_pending_bmsk;
uint8 uplink_dpch_tx_disable_pending_bmsk;
uint8 uplink_dpch_tx_enabled[WL1_MAX_UL_CARR_IDX];

typedef enum
{
  /* Non-primary carrier DPCH config received from RRC */
  WL1ULMCMGR_ACTION_CONFIG_DPCCH,
  /* HHO fail and resume case, non-primary carrier to
  ** wait in this state until primary carrier resumes sync
  */
  WL1ULMCMGR_ACTION_MOVE_TO_SCCH_ORDER_WAIT,
  /* Once primary carrier achieves DL sync, use this action
  ** to move non-primary carrier to the appropriate state
  */
  WL1ULMCMGR_ACTION_ACTIVATE_WITHOUT_ORDER,
  /* Non-primary carrier DPCH activated */
  WL1ULMCMGR_ACTION_ACTIVATE_DPCCH,
  /* Non-primary carrier DPCH disabled */
  WL1ULMCMGR_ACTION_DISABLE_DPCCH,
  /*HS SCCH order rcvd. to deactivate Sec UL*/
  WL1ULMCMGR_ACTION_DEACTIVATE_DPCCH,
  /* Reconfig DPCH */
  WL1ULMCMGR_ACTION_RECONFIG_DPCCH,
  /* Notify DL DPCH/F-DPCH sync status as in sync */
  WL1ULMCMGR_ACTION_IN_SYNC_IND_ENABLE_DPCCH_TX,
  /* Notify DL DPCH/F-DPCH sync status as out of sync */
  WL1ULMCMGR_ACTION_OUT_OF_SYNC_IND_DISABLE_DPCCH_TX,
  /* Execute UL Tx state machine for non-primary carrier */
  WL1ULMCMGR_ACTION_EXECUTE_UL_MC_STATE_MACHINE,
  /* Perform pending non-primary carrier DPCH state transition */
  WL1ULMCMGR_ACTION_EXECUTE_PENDING_ACTIONS,
  /* Write beta params to WFW */
  WL1ULMCMGR_ACTION_WRITE_WFW_BETA_PARAMS,
  /* Default no op */
  WL1ULMCMGR_ACTION_NO_ACTION,
  WL1ULMCMGR_NUM_ACTIONS
}wlulmcmgr_action_type;



/* DPCH state for primary tracked independently */
extern ul_mc_dpch_state_enum_type ul_dpch_carrier_state[WL1_MAX_UL_CARR_IDX];

typedef struct
{
  l1_ul_dpch_cfg_struct_type* dpch_cfg;
  l1_e_sec_dpcch_info_struct_type* sec_dpcch_cfg;
} wl1ulmcmgr_params_type;

typedef struct
{
  wl1_ul_carr_id_enum_type carr_idx;
  wlulmcmgr_action_type action_type; 
  wl1ulmcmgr_params_type params;
}wl1ulmcmgr_intf_type;

typedef enum
{
  /* Default no op, no Sec UL DPCCH cfg cmd to send to MDSP */
  WL1ULMCMGR_MDSP_CFG_NOOP,

  /* Allow sending enable Sec UL DPCCH cfg cmd to MDSP */
  WL1ULMCMGR_MDSP_CFG_ENA,

  /* Allow sending disable Sec UL DPCCH cfg cmd to MDSP */
  WL1ULMCMGR_MDSP_CFG_DIS,

  /* Indicate to Maint evt handler that Sec UL DPCCH enable cmd has been sent in the current CFN */
  WL1ULMCMGR_MDSP_ENA_CMD_SENT,

  /* Indicate to Maint evt handler that Sec UL DPCCH disable cmd has been sent in the current CFN*/
  WL1ULMCMGR_MDSP_DIS_CMD_SENT

}wl1ulmcmgr_mdsp_cfg_cmd_type;

extern wl1ulmcmgr_mdsp_cfg_cmd_type ul_sec_cfg_cmd_pending;



/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_cfg_validate(wl1_ul_carr_id_enum_type carr_idx,ul_phychan_add_type* ul_phychan_setup);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean wl1ulmcmgr_add_validate(wl1_ul_carr_id_enum_type carr_idx,ul_phychan_add_type* ul_phychan_setup);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean wl1ulmcmgr_drop_validate(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern ul_mc_dpch_state_enum_type wl1ulmcmgr_get_carr_state(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern int16 wl1ulmcmgr_get_ul_carr_dpcch_init_pwr(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern int16 wl1ulcmmgr_get_prim_ul_carr_tx_pwr(void);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES
  Note: Do not call this function directly, use wl1ulmcmgr_perform_dpcch_init_pwr_ops

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_determine_dpcch_pwr_maint(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES
  Note: Do not call this function directly, use wl1ulmcmgr_perform_dpcch_init_pwr_ops

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_calc_ul_dpcch_init_tx_pwr(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES


RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_perform_dpcch_init_pwr_ops(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_write_wfw_dpcch_slot_fmt(wl1_ul_carr_id_enum_type carr_idx);


/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean wl1ulmcmgr_pcp_period_status(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_dpcch_st_mc_routine(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_set_ul_dpcch_state(wl1_ul_carr_id_enum_type carr_idx, ul_mc_dpch_state_enum_type next_state);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_set_async_dpcch_ctrl(wl1_ul_carr_id_enum_type carr_idx ,boolean disable_tx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1_disable_ul_dpcch(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean wl1ulmcmgr_write_hw_wfw_dpcch_cfg_params(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean wl1ulmcmgr_enable_ul_dpcch(wl1_ul_carr_id_enum_type carr_idx);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean wl1ulmcmgr_req_ops(wl1ulmcmgr_intf_type* wl1ulmcmgr_intf_params);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_write_beta_params_to_mdsp(void);
#endif /* FEATURE_WCDMA_DC_HSUPA */

#if 0
/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean wl1ulmcmgr_compute_cm_slot_fmt(wl1_ul_carr_id_enum_type carr_idx);
#endif

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean wl1ulmcmgr_config_dpch(wl1ulmcmgr_intf_type* wl1ulmccfg_param);

/*=========================================================================
FUNCTION 

DESCRIPTION

This is executed in the 46th bpg of UL DPCCH frame (50th bpg of corresponding 
DL F-DPCH/DPCH frame) by the downlink sync control functionality.
The event is the RX_GEN_DL_MAINT_EVENT triggered on the RX GEN STMR evt
timeline. This timeline is aligned to DL F-DPCH/DPCH channel timing.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void ul_pa_mgr_notify_tx_disable(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


This is executed in the 46th bpg of UL DPCCH frame (50th bpg of corresponding 
DL F-DPCH/DPCH frame) by the downlink sync control functionality.
The event is the RX_GEN_DL_MAINT_EVENT triggered on the RX GEN STMR evt
timeline. This timeline is aligned to DL F-DPCH/DPCH channel timing.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void ul_pa_mgr_notify_tx_enable(wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================
FUNCTION 

DESCRIPTION


This is executed in Slot 2 (0-14 slot range), of the UL TX timeline(in DCH).
It is called from TX_GEN_ENC_GET_DATA event.

It is also executed by ul_pa_mgr_notify_tx_disable(). Please refer to that 
function for timing information.

DEPENDENCIES



RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void ul_pa_mgr_proc_pending_notf(wl1_ul_carr_id_enum_type carr_idx);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_dch_cleanup(void);

/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_dch_init(void);


/*=========================================================================
FUNCTION 

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

=========================================================================*/
extern void wl1ulmcmgr_init(void);

/*=========================================================================
FUNCTION wl1ulmcmgr_send_sec_ul_dpcch_sync_cmd()

DESCRIPTION  This function send FW the DPCCH_CFG cmd for secondary carrier


DEPENDENCIES  None

RETURN VALUE None

SIDE EFFECTS
  None.

=========================================================================*/

extern void wl1ulmcmgr_send_sec_ul_dpcch_sync_cmd();

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*=========================================================================
FUNCTION wl1ulmcmgr_sec_carr_pcp_srb_delay()

DESCRIPTION  This function is called be demod to check if sec UL is in PCP or SRB delay


DEPENDENCIES  None

RETURN VALUE boolean
TRUE:  Sec UL in PCP
FALSE:

SIDE EFFECTS
  None.

=========================================================================*/
boolean wl1ulmcmgr_sec_carr_in_pcp_srb_delay(void);
#endif /* defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA) */


#endif /*FEATURE_WCDMA_DC_HSUPA*/


#endif /*end of WL1ULMC_H*/

