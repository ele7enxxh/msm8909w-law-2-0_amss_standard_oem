#ifndef WL1_DLCARRCFG_H
#define WL1_DLCARRCFG_H
/*===========================================================================
                 WCDMA L1 DL CARRIER CONFIGURATION MANGER

GENERAL DESCRIPTION
  This file contains the code for all operations that manipulate the receive
  carrier

Copyright (c) 2007 by Qualcomm Technologies, Inc.  All Rights Reserved.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1dlcarrcfg.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $ 
 
when        who      what, where, why
--------    ---      --------------------------------------------------------- 
06/17/16    sl       Add 40 bpg delay for RF warm up while freezing EDRX scheduling
02/24/15    ap       Initial check-in for WL1 QSH framework 
09/09/15    sr       Changes to add SCCH order module as part of DCH cleanup
01/26/15    cjz      QXDM command support for DC-HSUPA activation via fake hs-scch order 
01/23/15    sr       Account for DC cell for mcvs update for DC to SC
01/08/15    sr       Changes to ignore SCCH orders during connected mode cleanup01/05/15    sks      Changes for combined PRx/DRx tune.
12/22/14    kr       do combined wakeup for dsds
10/09/14    mit      Fix for incorrect num_carr value returned incase of any carr cfg actions pending
08/21/14    rs       Fixed compiler warnings
08/06/14    sl       Changes to ignore SCCH orders before QTA based on timer approach
08/04/14    sad      RxD changes for DB-MC in case of SCCH order
07/30/14    qj       Fix compilation warning
07/18/14    sad      RF api changes and inter freq tune changes
06/23/14    sad      Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag
05/26/14    nd       Added support for LTA with DC-HSUPA
05/19/14    srk      Changes to ignore SCCH order prior to QTA.
03/17/14    mit      Changes to accommodate dynamic cell_db_idx allocation
05/13/14    ymu      Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/11/14    sr       Changes to introduce voting mechanism to ignore CPC SCCH order
03/17/14    jd       Moving the search related scch order changes to scheduler files.
04/11/14    yw       DCUPA: RF retune code cleanup
03/11/14    sad      SCCH order state machine and RF retune code
03/06/14    sad      Fix to clear RF DB when we release secondary band
02/28/14    sad      SCCH order optimization check in
04/04/14    abs      Mods to support secondary carrier Rx freq/pwr reporting
02/12/14    sad      DB-3C API check in
02/05/14    pr       Added support for DCHSUPA Finger Switching Ops.
02/05/14    sad      L1-RRC interface change and compaction cphy request
01/31/14    sad      Changes to gracefully handle SCCH orders sent by n/w in SC mode
01/16/14    sad      Fix for KW error
01/06/14    sr       Suspending periodic ops before CPHY req processing
12/30/13    pr       SCCH order State machine to avoid concurrencies.
12/17/13    jd       Changes to pass carrier idx to CME during scch activation and de-activation.
12/03/13    sad      Check to detect 3C to SC reconfiguration
12/03/13    pr       Clear Prev HS cell drop in setup cmd and drop HS cells during carrier swap.
11/25/13    pr       Notify CME to drop QICE on enabled cell, when dropping non HS secondary RL.
11/14/13    pr       Provide a new api based on carrier cfg for fetching the RL TTO during 
                     E-DL RL add.
10/29/13    sad      Inital check in for Secondary ASET update changes
10/28/13    sad      Fix to calculate num_carr in case of primary only reconfig
10/17/13    bs       Preliminary 3C Support
08/19/13    sad      Support to ignore SCCH order when CPHY processing is in progress
08/12/13    sad      Fix to drop HS when serving PSC and ordered DC cell's PN
                     position differ's by more than 1 BPG
07/15/13    zr       Removing old API so pending delete composite set can be used
06/27/13    sad      Added support for secondary Tx RL failure/Phychan estb. failure
06/18/13    kcm      Support for TxD detection on Secondary carrier.Also added
                     changes to backup the old DC srch cell ptr in the case of
                     DC to DC repointings
05/21/13    sad      Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/20/13    kcm/hrk  First cut

===========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "srchset.h"
#ifdef FEATURE_WPLT_ENABLE_DC_MODE
#error code not present
#endif

/** ----------------------------------------------
 *  CONSTANT / DEFINE DECLARATIONS
 *  ---------------------------------------------- */
#define WL1_DL_CARRCFG_CELL_DECFG_REQ       0x01

#define WL1_DL_CARRCFG_RF_CFG_REQ           0x02

#define WL1_DL_SRCH_MACRO_DIV_UPD_REQ       0x04 

#define WL1_DL_CARRCFG_CELL_CFG_REQ         0x08

#define WL1_DL_CARRCFG_TRIAGE_REQ           0x10

#define WL1_DL_CARRCFG_SEC_UL_FING_SWITCH_REQ    0x20

#define WL1_DL_CARRCFG_CXM_REQ              0x40

#define WL1_DL_CARRCFG_QICE_NBGR_CELL_DECFG_REQ  0x80

#define WL1_DL_CARR_MSG_HIGH                MSG_HIGH

#define WL1_DL_CARR_MSG_ERROR               MSG_ERROR

#define WL1_DL_CARRCFG_INVALID_NUM_CARRS    0xFF

#define WL1_DL_CARRCFG_INVALID_FREQ         0xFFFF

#define WL1_DL_CARRCFG_INVALID_PSC          0xFFFF

/* Do not change spec defined */
#define MAX_SEC_ASET_CELLS     0x04

#define WL1_DL_MAX_SCCH_ORD_COMBS     0x6

#define WL1_DL_1ST_SEC_SERVING_RL_ACT (1 << 0)

#define WL1_DL_2ND_SEC_SERVING_RL_ACT (1 << 1)

#define WL1_DL_SEC_UPLINK_ACT (1 << 4)

#define WL1_DL_GET_SCCH_ORD_ACT_STATUS_BMSK(val)   wl1_dl_scch_ord_table[val]

#define WL1_DL_CARRCFG_IS_NUM_CARRS_INVALID(carr_idx)   ((carr_idx == 0) || (carr_idx > WL1_MAX_NUM_CARRIER_IDX))

#define WL1_DL_CARRCFG_ACTIONS_IS_CELL_DECFG_OPS_INCL() \
        (wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk & WL1_DL_CARRCFG_CELL_DECFG_REQ ? TRUE : FALSE)

#define WL1_DL_CARRCFG_ACTIONS_IS_RF_CFG_OPS_INCL() \
        ( wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk & WL1_DL_CARRCFG_RF_CFG_REQ ? TRUE : FALSE)

#define WL1_DL_CARRCFG_ACTIONS_IS_SRCH_MACRO_DIV_OPS_INCL() \
        ( wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk & WL1_DL_SRCH_MACRO_DIV_UPD_REQ ? TRUE : FALSE)

#define WL1_DL_CARRCFG_ACTIONS_IS_SRCH_CELL_CFG_OPS_INCL() \
        ( wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk & WL1_DL_CARRCFG_CELL_CFG_REQ ? TRUE : FALSE)

#define WL1_DL_CARRCFG_ACTIONS_IS_SEC_UL_FING_SWITCH_OPS_INCL() \
        ( wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk & WL1_DL_CARRCFG_SEC_UL_FING_SWITCH_REQ ? TRUE : FALSE)

#define WL1_DL_CARRCFG_ACTIONS_IS_TRIAGE_OPS_INCL() \
        ( wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk & WL1_DL_CARRCFG_TRIAGE_REQ ? TRUE : FALSE)

#define WL1_DL_CARRCFG_ACTIONS_IS_CXM_OPS_INCL() \
        ( wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk & WL1_DL_CARRCFG_CXM_REQ ? TRUE : FALSE)

#define WL1_DL_CARRCFG_ACTIONS_IS_QICE_NBGR_CELL_DECFG_OPS_INCL() \
        (wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk & WL1_DL_CARRCFG_QICE_NBGR_CELL_DECFG_REQ ? TRUE : FALSE)

#define WL1_DL_CARRCFG_ALL_ACTIONS_DONE() \
        ((wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk == 0) ? TRUE : FALSE)

#define WL1_DL_CARRCFG_CLEAR_RF_CFG_OPS_INCL() \
        wl1_dl_carrcfg_cfg_db.internal_db.cfg_actions_bmsk &= (~WL1_DL_CARRCFG_RF_CFG_REQ)

#define WL1_DL_GET_MAX_VAL(a, b) \
        (a >= b) ? (a) : (b)

#define WL1_DL_SET_HS_DROP_PER_MC(a, b) \
        if ((wl1_dl_carrcfg_cfg_db.ord_db.num_carrs == 2 ) && (wl1_dl_carrcfg_cfg_db.curr_db.num_carrs == 3)) b = TRUE; \
        if ((wl1_dl_carrcfg_cfg_db.ord_db.num_carrs == 1 ) && (wl1_dl_carrcfg_cfg_db.curr_db.num_carrs == 2 || wl1_dl_carrcfg_cfg_db.curr_db.num_carrs == 3)) a = TRUE

#ifdef FEATURE_WCDMA_DC_HSDPA

#define WL1_DL_CARRCFG_INVALID_SCCH_ORD 0xFF

#define WL1_DL_CARRCFG_IS_SCCH_ORD_TYPE_FOR_DC_OPERATION(val) (((val == 1) && (wl1_dl_carrcfg_cfg_db.curr_db.num_carrs > 1)) ? TRUE : FALSE)

/*Remove after deletion of old event function*/
#define WL1_DL_CARRCFG_DC_STOP_EVENT        0x02

#define WL1_DL_CARRCFG_DC_START_EVENT       0x01

#define WL1_DL_CARRCFG_DC_NO_ORDER_EVENT    0x00

#define WL1_DL_CARRCFG_DC_CONTINUE_EVENT    0x03

#define WL1_DL_CARRCFG_DC_HSDPA_EVENT_SHIFT 0

#define WL1_DL_CARRCFG_NO_ORDER_EVENT    0x00

#define WL1_DL_CARRCFG_START_EVENT       0x01

#define WL1_DL_CARRCFG_STOP_EVENT        0x02

#define WL1_DL_CARRCFG_CONTINUE_EVENT    0x03

#define WL1_DL_CARRCFG_1ST_SEC_EVENT_SHIFT 0

#define WL1_DL_CARRCFG_2ND_SEC_EVENT_SHIFT 2

#define WL1_DL_CARRCFG_3RD_SEC_EVENT_SHIFT 4

#define WL1_DL_CARRCFG_DC_HSUPA_EVENT_SHIFT 6

#endif

#define SCCH_ORD_INACTIVE 0x00

#define SCCH_ORD_HSDPA_ACTION_IN_PROGRESS 0x01

#define SCCH_ORD_DCUPA_ACTION_IN_PROGRESS 0x02

#define SCCH_ORD_IN_PROGRESS 0x04

#define WL1_DL_CARRCFG_MAX_NUM_CME_NBGR_PER_CARR WL1_CME_MAX_NUM_NGHBRS_PER_CARRIER
/** ----------------------------------------------
 *  STRUCTURE DEFINITIONS
 *  ---------------------------------------------- */

typedef srch_aset_struct_type wl1_dl_carrcfg_srch_aset_cfg_struct_type;

typedef void WL1_CARRCFG_DUAL_BAND_RXD_ACT_DEACT_CB_FUNC_TYPE(boolean);

typedef void WL1_DL_CARRCFG_INFORM_CME_UPON_SCCH_ORD_RECEPTION_CB_FUNC_TYPE(uint8 );

extern WL1_DL_CARRCFG_INFORM_CME_UPON_SCCH_ORD_RECEPTION_CB_FUNC_TYPE* wl1_dl_carrcfg_inform_cme_upon_scch_ord_reception_cb;

extern WL1_CARRCFG_DUAL_BAND_RXD_ACT_DEACT_CB_FUNC_TYPE* wl1_dl_carrcfg_dual_band_rxd_act_deact_cb;

/**This struct holds the params needed to process the various 
 * steps of carrier configuration */
typedef struct
{
  uint8 num_carrs;
  wl1_dl_carrcfg_srch_aset_cfg_struct_type per_carr_srch_cfg_info[WL1_MAX_NUM_CARRIER_IDX];
}wl1_dl_carrcfg_cfg_param_struct;

/**This struct holds the params internal for performing 
 * various carrier configuration  steps*/ 
typedef struct
{
  uint8 cfg_actions_bmsk;
  uint16 cell_decfg_bmsk;
  uint16 prev_cfg_hs_cell_bmsk;
}wl1_dl_carrcfg_internal_param_db_type;

/**This struct holds the carrcfg ordered db and current db 
 * parameters. It also holds the current rf tune database **/
typedef struct
{
  wl1_dl_carrcfg_cfg_param_struct ord_db;
  wl1_dl_carrcfg_cfg_param_struct curr_db;
  wl1_dl_carrcfg_internal_param_db_type internal_db;
  wl1_rf_config_struct_type rf_db[WL1_MAX_NUM_BAND];
}wl1_dl_carrcfg_cfg_db_type;

#ifdef FEATURE_WCDMA_DC_HSDPA
/**this struct contains the information of the scch ord, i.e 
 * ord type and ord */
typedef struct
{
  uint8 order_type;
  uint8 order;
}wl1_dl_carrcfg_hs_scch_ord_type;

/**this struct database contains the information of the scch 
 * ord */
typedef struct
{
  wl1_dl_carrcfg_hs_scch_ord_type current_scch_ord;
  wl1_dl_carrcfg_hs_scch_ord_type saved_scch_ord;
  uint8 scch_ord_state;
  boolean cphy_pending_on_scch_comp;
  boolean cphy_idle_pending_on_scch_comp;
  uint8 scch_ord_vote_reg_client;
}wl1_dl_carrcfg_hs_scch_ord_db_type;

typedef enum
{
  /** Client for L1m */
  WL1_DL_CARRCFG_L1M_CLIENT,
  /** Client for CPC DRX cfg */
  WL1_DL_CARRCFG_CPC_DRX_CFG_CLIENT,
  /** Client for CPC DTX cfg */
  WL1_DL_CARRCFG_CPC_DTX_CFG_CLIENT,
  /** Client for QTA CPC */
  WL1_DL_CARRCFG_QTA_CPC_CLIENT,
  /** Client for QTA CPC */
  WL1_DL_CARRCFG_QTA_MC_CLIENT,
  /** Client for connected state cleanup */
  WL1_DL_CARRCFG_CONN_STATE_CLEANUP_CLIENT,

  WL1_DL_CARRCFG_MAX_CLIENT
}wl1_dl_carrcfg_scch_ord_ctrl_client_enum_type;
#endif /*FEATURE_WCDMA_DC_HSDPA*/

#define WL1_DL_CARRCFG_IGNORE_SCCH_ORD_MC  WFW_DEMOD_IGNORE_SCCH_ORD_MC
#define WL1_DL_CARRCFG_IGNORE_SCCH_ORD_CPC  WFW_DEMOD_IGNORE_SCCH_ORD_CPC

extern uint8 const wl1_dl_scch_ord_table[WL1_DL_MAX_SCCH_ORD_COMBS];

/** ----------------------------------------------
 *  FUNCTION PROTOTYPES 
 *  ---------------------------------------------- */

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_init

DESCRIPTION     This function initializes the databases of carrcfg

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void wl1_dl_carrcfg_init(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_init_curr_cfg_db

DESCRIPTION     This function initializes the current config database

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void wl1_dl_carrcfg_init_curr_cfg_db(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_execute_cfg_sequence_steps

DESCRIPTION     This function execute various  steps of carrier configuration

DEPENDENCIES    None

RETURN VALUE    TRUE - If all the configuration steps are not done and there is 
                a break in context 
                FALSE - If all the configuration steps are done 

SIDE EFFECTS    None
===========================================================================*/
extern boolean wl1_dl_carrcfg_execute_cfg_sequence_steps(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_prepare_ord_cfg_db

DESCRIPTION     This function prepares the carrier cfg ordered database based 
                on the cphy setup req. 

DEPENDENCIES    None

RETURN VALUE    None 
 
SIDE EFFECTS    None
===========================================================================*/
extern void wl1_dl_carrcfg_prepare_ord_cfg_db(uint8);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_perform_rf_tune_to_channel

DESCRIPTION     This function performs the tuning of rf.

DEPENDENCIES    None

RETURN VALUE    None 
 
SIDE EFFECTS    None
===========================================================================*/
extern void wl1_dl_carrcfg_perform_rf_tune_to_channel(uint16 );

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_get_asu_rl_drop_bmsk

DESCRIPTION     This function retuns the rl drop bmsk for performing 
                Active set update.

DEPENDENCIES    ASU is included in cphy setup req.

RETURN VALUE    Uint16 bmsk 
 
SIDE EFFECTS    None
===========================================================================*/
extern uint16 wl1_dl_carrcfg_get_asu_rl_drop_bmsk(uint8);

/*===========================================================================
FUNCTION        wl1_carrcfg_notify_l1m_prev_hs_cfg_cell_drop_done

DESCRIPTION     This function notifies the l1msetup after performing the 
                cell deconfiguration of previous hs config. 

DEPENDENCIES    Cphy setup request with HS repointing 

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_carrcfg_notify_l1m_prev_hs_cfg_cell_drop_done(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_perform_prev_hs_cfg_cell_drop

DESCRIPTION     This function performs the cell deconfiguration procedure 
                of previous hs config.
 
DEPENDENCIES    Cphy setup request with HS repointing 

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_perform_prev_hs_cfg_cell_drop(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_init_ord_cfg_db

DESCRIPTION     This function inits the ord cfg database.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void wl1_dl_carrcfg_init_ord_cfg_db(void);

#ifdef FEATURE_WCDMA_DC_HSDPA

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_process_scch_act_srch_done

DESCRIPTION     This function will do the resume rxd, cme, heartbeat searches 
                after scch activation searches are done 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_process_scch_act_srch_done( void );

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_register_dbdc_rxd_act_deact_cb

DESCRIPTION     This function registers dbdc callback for rxd

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
===========================================================================*/
extern void wl1_dl_carrcfg_register_dual_band_rxd_act_deact_cb(WL1_CARRCFG_DUAL_BAND_RXD_ACT_DEACT_CB_FUNC_TYPE rxd_cb);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_register_dbdc_cme_act_deact_cb

DESCRIPTION     This function registers dbdc callback for cme

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
===========================================================================*/
extern void wl1_dl_carrcfg_register_scch_ord_cme_cb(WL1_DL_CARRCFG_INFORM_CME_UPON_SCCH_ORD_RECEPTION_CB_FUNC_TYPE cme_cb);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_process_rxfe_int

DESCRIPTION     This function 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/

extern void wl1_dl_carrcfg_rxfe_start_int_handler( void );
/*===========================================================================
FUNCTION wl1_dl_carrcfg_hs_scch_ord_handler

DESCRIPTION
  This function handles HS-SCCH orders indicated by firmware.
  Order would be interpretted based on the below table (spec: 25.212)
 
  Xodt,1  Xodt,2  Xodt,3  = 0 0 1(ITS a ORD for DC HSDPA/DC HSUPA enable/disable)
 
   A- Activate           D- Deactivate
  -----------------------------------------------------------
  Xord,2    |      Xord, 3    =     DC HSUPA    |  DC HSDPA
  -----------------------------------------------------------
     0               0                D               D
 
     0               1                D               A
 
     1               1                A               A
 
     1               0               UNUSED COMBINATION
 
  -----------------------------------------------------------
 
 DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_dl_carrcfg_hs_scch_ord_handler( void );

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_init_curr_scch_ord_db

DESCRIPTION     This function initializes the curr scch ord db
 
DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_init_curr_scch_ord_db(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_init_saved_scch_ord_db

DESCRIPTION     This function initializes the saved scch ord db

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_init_saved_scch_ord_db(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_schedule_txd_det_dc_cell

DESCRIPTION     This function schedules the TxD detection for the DC-HS cell
 
DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_schedule_txd_det_dc_hsdpa_cell(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_vote_scch_ord_ignore_state

DESCRIPTION     This function controls the voting mechanism for scch orders ignore by different clients
                         L1m client requests to ignore all SCCH orders
                         CPC DTX client requests to ignore only CPC SCCH orders
                         CPC DRX client requests to ignore only CPC SCCH orders
                         QTA CPC requests to ignore only CPC SCCH orders
                         QTA MC requests to ignore only MC SCCH orders

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_vote_scch_ord_ignore_state(
  wl1_dl_carrcfg_scch_ord_ctrl_client_enum_type client, boolean ignore_scch_ord);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_cntrl_scch_order_notify_cb

DESCRIPTION     This is call back event which fires exactly one frame before QTA start
                        We vote to ignore multi carrier SCCH order as a part of this function.
                        This event will be fired only DCHSDPA is configured but not DCHSUPA.

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_cntrl_scch_order_notify_cb();

#endif /* FEATURE_WCDMA_DC_HSDPA */

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*===========================================================================
FUNCTION        wl1_dl_carrcfg_rf_tx_retune_ops

DESCRIPTION     This function performs procedures for activating/deactivating 
                DC HSUPA based on the SCCH order 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_rf_tx_retune_ops(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_rf_tx_retune_ops

DESCRIPTION     This function performs procedures for activating/deactivating 
                       DC-HSUPA on secondary carrier after LTA

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/

extern void wl1_dl_carrcfg_rf_tx_retune_ops_after_lta(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_get_dc_hsupa_act_status

DESCRIPTION     API to read secondary carrier UL status from SCCH order
                bitmask

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/

extern boolean wl1_dl_carrcfg_get_dc_hsupa_act_status(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_rf_tx_retune_and_resume_cphy_ops

DESCRIPTION     This function performs procedures for retuning the rf and  
                informing cphy 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_rf_tx_retune_and_resume_cphy_ops(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_upd_curr_scch_ord_upon_sec_deact

DESCRIPTION     This function updates the scch order data base upon the 
                RL failure/Phychan establishment failure on secondary carrier 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_upd_curr_scch_ord_upon_sec_deact(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_is_spl_case_dc_hsupa_aset_upd

DESCRIPTION     This function determines whether current DC serving cell is not part of the new DC ASET and the 
                      new DC serving cell is not part of the old DC ASET. 

DEPENDENCIES    None

RETURN VALUE    TRUE if this special case is satisfied

SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_carrcfg_is_spl_case_dc_hsupa_aset_upd(void);
#endif /* FEATURE_WCDMA_DC_HSUPA */

/*===========================================================================
FUNCTION        wl1_dl_is_hs_implicit_drop_needed

DESCRIPTION     This function schedules an HS drop when PN position of serving PSC and ordered DC psc differs by 
                        more than 1BPG

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_is_hs_implicit_drop_needed(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_dbdc_cleanup

DESCRIPTION     This function cleans up dbdc related params
 
DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_dual_band_cleanup(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_get_rl_tto

DESCRIPTION     This function returns the TTO for a specific cell idx and carr idx

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern uint16 wl1_dl_carrcfg_get_rl_tto(uint16 carr_idx, uint8 cell_idx);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_get_cell_decfg_bmsk

DESCRIPTION     This function returns the cell decfg bmsk when a Carrier id is passed

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern uint16 wl1_dl_carrcfg_get_cell_decfg_bmsk(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_reset_cfg_db_for_hs_implicit_drop

DESCRIPTION     This function sets the cell decfg bmsk to the prev hs cell mask and clears the prev
                hs cell bmsk

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/

extern void wl1_dl_carrcfg_reset_cfg_db_for_hs_implicit_drop(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_scch_order_ops

DESCRIPTION     This function returns the status of whether a CPHY suspend is needed based on the SCCH order
                          operation being performed

DEPENDENCIES    None

RETURN VALUE    TRUE if CPHY can start
 
SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_carrcfg_scch_order_ops(void);
/*===========================================================================
FUNCTION        wl1_dl_carrcfg_post_scch_handling_done

DESCRIPTION     This function posts a local command once SCCH order handling is done. The local command 
                          unfreezes the cphy if needed

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_post_scch_handling_done(void);
/*===========================================================================
FUNCTION        wl1_dl_carrcfg_perform_scch_order_done_post_ops

DESCRIPTION     This function gets processed when L1_SCCH_ORD_DONE_CMD is posted once SCCH order
                          handling is done. It unsuspends cphy if needed.
DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_perform_scch_order_done_post_ops(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_is_scch_order_proc_ongoing_event

DESCRIPTION     This function is an event handler which checks if SCCH order is received during CPHY 
                         setup is in progress.

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_is_scch_order_proc_ongoing_event(uint8 evt);

/*===========================================================================
FUNCTION        wl1_dl_carr_upd_curr_scch_ord_based_on_bmsk

DESCRIPTION    The function updates the current SCCH order based on the bmsk

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carr_upd_curr_scch_ord_based_on_bmsk(uint8 );

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_get_curr_scch_ord

DESCRIPTION    This fucntion returns the current SCCH order

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern uint8 wl1_dl_carrcfg_get_curr_scch_ord(void);

/*===========================================================================
FUNCTION        WL1_DL_GET_CURR_NUM_CARR

DESCRIPTION     This function return the valid number of carrier/s based on which
                data base is valid (i.e curr_db or ord_db)

DEPENDENCIES    curr_db or ord_db to be updated accordingly

RETURN VALUE    number of carriers (default value is 1)

SIDE EFFECTS    None
==========================================================================*/
extern uint8 wl1_dl_get_curr_num_carr(void);

/*===========================================================================
FUNCTION        wl1_dl_compute_ord_num_carrs

DESCRIPTION    Computes the order number of carrier

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern uint8 wl1_dl_compute_ord_num_carrs(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_execute_sec_ul_fing_switch_ops

DESCRIPTION     This function executes triage operations if needed

DEPENDENCIES    None

RETURN VALUE    TRUE if there is a) No Need to perform finger switching ops
                                                 b) Switch ops is needed and is successfully performed.
                          FALSE if fing decfg is needed and a break in context will happen
 
SIDE EFFECTS    None
===========================================================================*/
extern boolean wl1_dl_carrcfg_execute_sec_ul_fing_switch_ops(void);

/*===========================================================================
FUNCTION        wl1_dl_carr_upd_curr_scch_ord_based_on_bmsk

DESCRIPTION    The function updates the current SCCH order based on the bmsk

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carr_upd_curr_scch_ord_based_on_bmsk(uint8 );

/*===========================================================================
FUNCTION        wl1_dl_upd_rf_config_db_to_single_band

DESCRIPTION    The function sets RF DB To single carrier mode

DEPENDENCIES    None

RETURN VALUE    boolean

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_upd_rf_config_db_to_single_band(uint16 );

/*===========================================================================
FUNCTION        WL1_DL_GET_ORD_NUM_CARR

DESCRIPTION    

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern uint8 wl1_dl_get_ord_num_carr(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_get_curr_db_act_status_frm_carr_idx

DESCRIPTION    The function returns the activation status of the carr_idx passed

DEPENDENCIES    None

RETURN VALUE    boolean
 
SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_carrcfg_get_curr_act_status_frm_carr_idx(uint8 );

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_get_ord_db_act_status_frm_carr_idx

DESCRIPTION    The function returns the activation status of the carr_idx passed

DEPENDENCIES    None

RETURN VALUE    boolean
 
SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_carrcfg_get_ord_act_status_frm_carr_idx(uint8 );

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_is_cell_part_of_ord_aset

DESCRIPTION     The function checks the given psc, freq is present in ordered db
                This api is provided for usage by qicecntrl ONLY, 
                when wl1_cme_free_cell_db_idx() is called.

DEPENDENCIES    None

RETURN VALUE    TRUE if present
                FALSE otherwise

SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_carrcfg_is_cell_part_of_ord_aset(uint16 psc, uint16 freq);

/*===========================================================================
FUNCTION     wl1_dl_carrcfg_init_dual_band_rf_db

DESCRIPTION
  This function initialises the RF DB for the second band

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_dl_carrcfg_init_dual_band_rf_db(void);

/*===========================================================================
FUNCTION     wl1_dl_carrcfg_tune_to_channel

DESCRIPTION
  This function is used to tune RF to the desired freq

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_dl_carrcfg_tune_to_channel(uint32 , rfm_device_enum_type ,uint8 ,MDSP_WCDMA_L1_EVENT_CB_FUNC_TYPE,wl1_rfm_wakeup_chain_type);

/*===========================================================================
FUNCTION     wl1_dl_get_curr_tuned_chan

DESCRIPTION
  This function is returns the current freq for the passed band

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 wl1_dl_get_curr_tuned_chan(uint8 );

/*===========================================================================
FUNCTION     wl1_dl_get_curr_tuned_chan

DESCRIPTION
  This function is returns the current freq for the passed band

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 wl1_dl_get_curr_tune_chan_rx_freq_kHz(uint8 );

/*===========================================================================
FUNCTION     wl1_dl_rf_tune_to_single_carrier

DESCRIPTION
  Tunes RF to single 5Mhz band 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_dl_rf_tune_to_single_carrier(uint16 ,uint32, rfm_device_enum_type ,uint8 ,MDSP_WCDMA_L1_EVENT_CB_FUNC_TYPE,wl1_rfm_wakeup_chain_type );

/*===========================================================================
FUNCTION  wl1_dl_carrcfg_get_ca_freq

DESCRIPTION
  This function returns the CA home freq.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE


SIDE EFFECTS
  None
===========================================================================*/
extern uint16 wl1_dl_carrcfg_get_ca_freq(void);

/*===========================================================================
FUNCTION     wl1_dl_set_curr_tune_chan_rx_freq_kHz

DESCRIPTION
  This function sets the current tuned freq

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_dl_set_curr_tune_chan_rx_freq_kHz(uint32 freq, uint8 band);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_is_carrier_aggregation_active_in_curr_cfg_db

DESCRIPTION     This function determines if carrier aggregation is on for current cfg

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    None
==========================================================================*/
extern boolean wl1_dl_carrcfg_is_carrier_aggregation_active_in_curr_cfg_db(void);

/*===========================================================================
FUNCTION     wl1_dl_carrcfg_get_per_carr_prev_hs_cells

DESCRIPTION
  This function returns num cells present in prev_hs_cell_bmask.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 wl1_dl_carrcfg_get_per_carr_prev_hs_cells(uint8 carr_id);

/*===========================================================================
FUNCTION     wl1_dl_carrcfg_is_scch_ord_in_prog

DESCRIPTION
  This function checks if SCCH order is in progress.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_dl_carrcfg_is_scch_ord_in_prog(void);

/*===========================================================================
FUNCTION     wl1_dl_carrcfg_set_scch_order_state

DESCRIPTION
  This function sets SCCH order state

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  wl1_dl_carrcfg_set_scch_order_state(uint8 ord_state);

/*===========================================================================
FUNCTION     wl1_dl_carrcfg_resume_cfg_sequence_steps

DESCRIPTION
  This function post a local command to resume carrier cfg operations

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_dl_carrcfg_resume_cfg_sequence_steps(void);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION        wl1_dl_carrcfg_trigger_dcupa_scch_order_thru_qxdm

DESCRIPTION     This function activates secondary carrier with a fake scch order,
                being invoked by a qxdm command

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void wl1_dl_carrcfg_trigger_dcupa_scch_order_thru_qxdm(void);

/*===========================================================================
FUNCTION        wl1_dl_carrcfg_rf_tx_retune_ops_for_qxdm

DESCRIPTION     This function performs procedures for activating/deactivating 
                DC HSUPA based on the fake SCCH order 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
void wl1_dl_carrcfg_rf_tx_retune_ops_for_qxdm(void);
#endif /* FEATURE_WCDMA_DC_HSUPA */
/*===========================================================================
FUNCTION     WL1_DL_CARRCFG_IS_INTERF_CELL_RESEL_IN_PROG

DESCRIPTION
  This function checks if any inter-frequency reselection is in progress

DEPENDENCIES
  This function should only be used when CPHY is in progress

PARAMETERS
  None.

RETURN VALUE
  TRUE if Inter-frequency reselection is in progresss
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean wl1_dl_carrcfg_is_interf_cell_resel_in_prog();
#endif
