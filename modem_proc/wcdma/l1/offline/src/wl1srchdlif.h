/*===========================================================================
FILE: wl1srchdlif.c


GENERAL DESCRIPTION
  This file contains code responsible for event evaluations for MBMS cells.


Copyright (c) 2010-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1srchdlif.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/16   sl      Avoid decrementing the cell_cfg_count twice for one config removal from txd queue
03/21/16   rn      API to clear all TXD active and pending transactions
08/07/15   sr      Changes to init srchdlif db at one time init
07/13/15   jk      Doubling srch_in_asu variable per sub_id.
07/13/15   vn      Debuggability enhancements for handling QTA, while cell is under config/deconfig.
07/08/15   vn     Added Searcher debug code 
06/18/15   da      Added mutex protection for srch cell config/deconfig and txd detection
02/20/15   mk      Introduce new API to check whether any cell in triage set undergoing config.
07/03/14   sks     Changes for FCDC
06/09/14   da      Created a new api to get the current hs serving cell for a given carrier
05/08/14   rs/pv   FR19940 FCDC code changes
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/24/14   jg      Define new API to return only HS cells to CME.
02/14/14   rsr     Added Fix to avoid CME/Triage Race condition where cell gets 
                   configured in ISR and passes CME validation check for QSET addition,
                   but fingers are not yet assigned on the cell.Fixes issue with 
                   previous checkin.
01/24/14   rsr     Added Fix to avoid CME/Triage Race condition where cell gets 
                   configured in ISR and passes CME validation check for QSET addition,
                   but fingers are not yet assigned on the cell.
01/02/14   km      Split cfg_decfg_ongoing into 2 functions
12/06/13   ar      Extern srch_in_asu 
10/29/13   sr/gsk  During cell reselection, delaying CPHY setup cnf for NBCH drop untill
                   serv cell is configured and nbr cell is deconfigured,
09/23/13   as      Porting Dual Sim from Triton to Dime.
09/12/13   jd      Searcher redesign 
07/15/13   zr      Removing old API so pending delete composite set can be used
06/18/13   kcm     Changes to cleanup DC srch cell ptrs during transition from
                   DC to SC.
05/16/13   zr      Notify carrier config when ASET update completes
05/03/13   kcm     Change the srch retry txd det cnt to 4 so as to limit the 
                   number of txd detection tries to 5. 
04/04/13   pr      Changes for deciding Retry TxD Count
03/27/13   pr      SW Changes for TxD command done from FW.
02/15/13   pr      Ensure that TD Detect is not sent to FW for 
                   cell which are under deconfiguration.
02/13/13   zr      Added cell config callback for CME to config info struct
01/31/13   ag      Mainlining FEATURE_WCDMA_RESCHEDULE_TXD_DET
01/28/13   pr      Sequentialize TD Detect for Dime.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/13/12   zr      Add support for periodic TD determination.
03/10/12   sv      Externed the API to reschedule TxD detection on a cell.
09/09/11   rgn     Expand the number of parallel cfg requests 
02/08/12   vs      Feature cleanup.
08/17/11   rgn     Increased the array size of cfg/decfg structure
07/05/11   vsr     Mainlining Genesis CME features
06/14/11   vsr     Disable SCHIC for voice calls
05/13/11   sv      Added support for periodic TxD determination.
01/13/11   sv      Added support for QICE ASET cell registration
10/19/10   sv      Added changes to support full search for DC and QICE support.
10/07/10    vb      Added field to srch cell config API to pass info of
                    demod cfg comit to FW needed or not
05/05/10   mc      Removed Taxis featurization for SSC and SCPICH
03/30/10   rvs     Remove FEATURE_WCDMA_CME_CONTROLLER.
02/26/10   rvs     Fixes for Mustang.
01/08/09   hrk/rgn Added misc interface to configure certain cell params 
12/09/09   sv      Extern the global to override TD detection.
11/06/09   mr      Handle MIMO S_CPICH power Imbalance from RRC
09/24/09   rvs     Include wl1qicectrli.h instead of wl1qicectrl.h.
07/20/09   rvs     Include the wl1qicectrl.h header file.
07/15/09   rgn     Enhanced existing s-cpich support  
07/16/09   hk      Reorganized the code to remove INC/SRC violations
07/15/09   rvs     Edited feature flags to include
                   FEATURE_WCDMA_QICE_CONTROLLER.
04/29/09   rvs     Moved reg/dereg CME txd change functions to here from srch.h.
01/19/09   sv      Increasing the max number of parallel queues to 3.
12/03/08   rgn     Added support for Tx Diversity detection procedure 
10/01/08   sv      Added "srch_is_cell_in_deconfig" API.
09/26/08   vb      externed srch_check_if_any_cfg_decfg_in_progress
09/25/08   sv      Code cleanup for SRCH-DL i\f.
09/14/08   uk      Added Config/Deconfig reasons for srch wakeup/slp in DRX
09/12/08   nd      changes for cell deconfig during state cleanup 

===========================================================================*/

#ifndef WL1_SRCH_DL_IF_H
#define WL1_SRCH_DL_IF_H

#include "wcdma_variation.h"
#include "srchset.h"
#include "dlcellcfg.h"
#include "wl1qicectrli.h"
#include "wl1dlcarrcfg.h"

/* max no of cells that can be scheduled for the txd detection */
#ifdef FEATURE_WCDMA_DC_HSDPA
#define SRCH_DL_CFG_DECFG_SET_MAX 22
#else
#define SRCH_DL_CFG_DECFG_SET_MAX 16
#endif
/* how many max parallel req for txd detection we can send to demod. 
 * theoriticall we can do more but limiting now so that when the result comes
 * back we dont use too many local cmds 
 */
/* Increasing the max number of parallel queues to 3 for the case when -
 * SRCH registers a cell config(1), then starts TxD detection.
 * TxD detection returns TD different from what we have programmed, so 
 * registers cell config again(2). At this time, if an ASU happens,
 * thats a third cell config(3). 
 */
#define SRCH_DL_MAX_PARALLEL_REQ   SRCH_DL_CFG_DECFG_SET_MAX

#define SRCH_NUM_CELL_DECONFIG_MAX SRCH_DL_CFG_DECFG_SET_MAX

#define SRCH_RETRY_TXD_DET_CNT_MAX      4

/* Timer to check if a cell sent for config/deconfig is stuck */
#define SRCH_CELL_STUCK_TIMER 20

/** THis stores the function callback internally within searcher. Whenever we break in context during CELL_CONFIG/
 CELL_DECONFIG we store the remaining processing in the post processor. Whenever DL comes back successfully,
 after a CELL_CONFIG/CELL_DECONFIG, post processor will do that processing  */
typedef void (*SRCH_CELL_CFG_DECFG_POST_PROCESSOR)(void);

typedef void (*SRCH_CME_NGHBR_CELL_DECONFIG_CB_FUNC_TYPE) (uint16  cell_bmsk);
typedef void (*SRCH_CME_NGHBR_CELL_CONFIG_CB_FUNC_TYPE)   (uint16 cell_bmsk);

/*Callback function which will be used by FACH module to call the DL func once FMO is completed. It is to handle
   CPHY_SETUP handling colliding with FMO*/
typedef void (*SRCHFACH_FMO_DONE_CB_FUNC)(void);

typedef enum
{
  SRCHDLIF_REASON_NONE,

  SRCHDLIF_CONFIG_REASON_ACQ,
  SRCHDLIF_CONFIG_REASON_BPLMN_ACQ,
  SRCHDLIF_CONFIG_REASON_ASET_UPD,
  SRCHDLIF_CONFIG_REASON_RESTORE_SERV_CELL,
  SRCHDLIF_CONFIG_REASON_NBCH_DECODE,
  SRCHDLIF_CONFIG_REASON_NBCH_DONE,
  SRCHDLIF_CONFIG_REASON_REACQ,  
  SRCHDLIF_CONFIG_REASON_TXD_DET,
  SRCHDLIF_CONFIG_REASON_SCPICH_MIMO_MODE_REQ,
  SRCHDLIF_CONFIG_REASON_QICE_NBR,
  SRCHDLIF_CONFIG_REASON_QICE_ASET,

  SRCHDLIF_DECONFIG_REASON_ACQ_PULLIN_DONE,
  SRCHDLIF_DECONFIG_REASON_ACQ_STTD_DONE,
  SRCHDLIF_DECONFIG_REASON_ASET_UPD,
  SRCHDLIF_DECONFIG_REASON_NBCH_DECODE,
  SRCHDLIF_DECONFIG_REASON_NBCH_DONE,
  SRCHDLIF_DECONFIG_REASON_CELL_TRANS_DONE,
  SRCHDLIF_DECONFIG_REASON_TXD_DONE,
  
  SRCHDLIF_DECONFIG_REASON_DEREG_ASET_ACQ,
  SRCHDLIF_DECONFIG_REASON_DEREG_ASET_VERIFY_ACQ,
  SRCHDLIF_DECONFIG_REASON_DEREG_ASET_BPLMN_ACQ,
  SRCHDLIF_DECONFIG_REASON_DEREG_ASET_BPLMN_SERV_CELL,
  SRCHDLIF_DECONFIG_REASON_DEREG_ASET_BPLMN_BPLMN_CELL,
  SRCHDLIF_DECONFIG_REASON_DEREG_ASET_HHO_SAVE,
  SRCHDLIF_DECONFIG_REASON_DEREG_STATE_TRANS,
  SRCHDLIF_DECONFIG_REASON_SLEEP,
  SRCHDLIF_DECONFIG_REASON_SCPICH_MIMO_MODE_REQ, 
  SRCHDLIF_DECONFIG_REASON_QICE_NBR,
  SRCHDLIF_DECONFIG_REASON_QICE_ASET,
  SRCHDLIF_CONFIG_REASON_STTD_RECONFIG,
  SRCHDLIF_CONFIG_CELL_PARAMS
#ifdef FEATURE_WCDMA_DC_HSDPA
  , SRCHDLIF_CONFIG_REASON_DC_CONFIG
#endif
}srchdlif_cfg_reason_enum_type;

/* Reasons for the cell parameters configuration*/
typedef enum
{
  SRCHDLIF_CELL_CONFIG_SCPICH,
  SRCHDLIF_CELL_CONFIG_SSC,
  SRCH_CELL_CONFIG_PARAMS_MAX
}srch_cell_params_cfg_enum_type;

#define SRCHDLIF_IS_DECONFIG_REASON_DREG_ASET(reason)                 (  \
(reason == SRCHDLIF_DECONFIG_REASON_DEREG_ASET_ACQ) ||                      \
(reason == SRCHDLIF_DECONFIG_REASON_DEREG_ASET_VERIFY_ACQ) ||          \
(reason == SRCHDLIF_DECONFIG_REASON_DEREG_ASET_BPLMN_ACQ) ||          \
(reason == SRCHDLIF_DECONFIG_REASON_DEREG_ASET_BPLMN_SERV_CELL) ||          \
(reason == SRCHDLIF_DECONFIG_REASON_DEREG_ASET_BPLMN_BPLMN_CELL) ||          \
(reason == SRCHDLIF_DECONFIG_REASON_DEREG_ASET_HHO_SAVE)            \
)

typedef struct
{
  boolean active;
  
  boolean cfg_pending;

  uint16 cell_mask;

  srchdlif_cfg_reason_enum_type reason;

  uint16 start_ts;

  uint8 pend_action_bmask;
  uint8 txd_max_retry_cnt;
  /* The reason field disambiguates this union */
  union
  {
    /* Searcher cell configuration or deconfiguration done */
    SRCH_CELL_CFG_DECFG_POST_PROCESSOR srch;

    /* CME cell configuration done */
    SRCH_CME_NGHBR_CELL_CONFIG_CB_FUNC_TYPE cme_cfg;

    /* CME cell deconfiguration done */
    SRCH_CME_NGHBR_CELL_DECONFIG_CB_FUNC_TYPE cme_decfg;
  } post_processing_cb;
  
}srchdlif_per_config_info_struct_type;

/* information database of the cells which are undergoing txd detection */
typedef struct
{
  /* holds the ID of the current transaction with DL */
  uint8 curr_trans_id;
  
  /* Num cells for which config\de-config\TxD is requested */
  uint8  cell_cnt;

  srch_cell_struct_type *cell_ptr[SRCH_DL_CFG_DECFG_SET_MAX];

  /* Counter indicating the number of CELL_CFG/DECONFIG/TxD that are currently 
   * issued and havent received an ACK from DL yet
   */
  uint16 cfg_count;
  
  /* Flag to track whether any configuration is in progress */
  boolean cfg_in_progress;

  srchdlif_per_config_info_struct_type per_cfg_info[SRCH_DL_MAX_PARALLEL_REQ];
} srch_dl_if_struct_type;

typedef struct
{
  /*cell db idx*/
  uint8 cell_db_idx;
  /*activate or deactivate s-cpich mode*/
  boolean activate;
  /*sec cpich ovsf*/
  uint8 sec_cpich_ovsf_mimo_mode;
  /*sec cpich scr*/
  uint8 sec_cpich_scr_code_mimo_mode;
  /*function pointer to be called on*/
  SRCH_CELL_CFG_DECFG_POST_PROCESSOR srch_post_processor;
  /* MIMO S_CPICH Imbalance Scale */
  uint16 s_cpich_imb_scale;
} srchdlif_cell_cfg_scpich_params;

typedef struct
{
  /*cell db idx*/
  uint8 cell_db_idx;
  /*Secondary scrambling code*/
  uint8 ssc;
  /*function pointer to be called on*/
  SRCH_CELL_CFG_DECFG_POST_PROCESSOR srch_post_processor;
} srchdlif_cell_cfg_ssc_params;

typedef union
{
  srchdlif_cell_cfg_scpich_params cell_scpich_params;
  srchdlif_cell_cfg_ssc_params cell_ssc_params;
} srchdlif_cell_config_params_type;

extern srch_dl_if_struct_type srch_txd_det_info;
extern srch_dl_if_struct_type srch_config_cell_info;
extern srch_dl_if_struct_type srch_deconfig_cell_info;

/* Global used to override TD detection */
extern boolean wl1_avoid_td_det;

/* Flag to indicate whether an active set update procedure is in progress */
extern boolean srch_in_asu;


extern void srchdlif_init(void);

extern void srch_deconfigure_cells_with_dl
(
  uint16 srch_cell_dealloc_mask,
  SRCH_CELL_CFG_DECFG_POST_PROCESSOR srch_post_processor,
  srchdlif_cfg_reason_enum_type deconfig_reason
);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION srchbplmn_deconfigure_aset_cells

DESCRIPTION:

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchbplmn_deconfigure_aset_cells(SRCH_CELL_CFG_DECFG_POST_PROCESSOR decfg_done_cb);
#endif

/*===========================================================================

FUNCTION srch_schedule_txd_detection

DESCRIPTION:
  configure diffrent freq hypothesis to FW for step1
  'curr_rotator' will be passed if we get some data from tcxo etc. It
  will be used at the start of the acq search

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void srch_schedule_txd_detection(srch_cell_struct_type *cell_ptr, uint8 num_retry_count);

extern void srch_configure_cells_with_dl
(
  uint16 srch_cell_alloc_mask,
  SRCH_CELL_CFG_DECFG_POST_PROCESSOR srch_post_processor,
  srchdlif_cfg_reason_enum_type config_reason,
  boolean demod_commit_needed
);

extern boolean srch_check_if_aset_dereg_in_progress(void);

extern boolean srch_add_config_cells(srch_cell_struct_type * cell_ptr);

extern boolean srch_add_deconfig_cells(srch_cell_struct_type * cell_ptr);
extern boolean srch_check_any_cfg_ongoing(void);
extern boolean srch_check_any_decfg_ongoing(void);
extern boolean srch_check_if_any_cfg_decfg_in_progress(void);

/*===========================================================================

FUNCTION SRCH_IS_CELL_IN_DECONFIG

DESCRIPTION:
   This function checks whether any cell is currently set to be deconfigured.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if given cell pointer is present in deconfig cells DB
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_is_cell_in_deconfig(srch_cell_struct_type * cell_ptr);

/*===========================================================================

FUNCTION SRCH_IS_CELL_IN_CONFIG

DESCRIPTION:
   This function checks whether any cell is currently set to be configured.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if given cell pointer is present in config cells DB
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_is_cell_in_config_for_cme(uint16 freq, uint16 scr_code);


/*===========================================================================

FUNCTION srch_register_cme_txd_change_cb

DESCRIPTION:
  Function used by CME module to register txd change callback

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_register_cme_txd_change_cb(
  /* Callback function pointer */
  SRCH_CME_TXD_CHANGE_CB_FUNC_TYPE *srch_cme_cb);

/*===========================================================================

FUNCTION srch_deregister_cme_txd_change_cb

DESCRIPTION:
  Function used by CME module to de-register txd change callback

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_deregister_cme_txd_change_cb(void);

/*===========================================================================

FUNCTION srch_register_cme_srch_cell_info_cb

DESCRIPTION:
  Function used by CME module to register search info callback

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_register_cme_srch_cell_info_cb(
  /* Callback function pointer */
  SRCH_CME_CELL_INFO_CB_FUNC_TYPE *srch_cme_cb, boolean call_cb_now);

/*===========================================================================

FUNCTION srch_deregister_cme_srch_cell_info_cb

DESCRIPTION:
  Function used by CME module to de-register search info callback

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_deregister_cme_srch_cell_info_cb(void);

/*===========================================================================

FUNCTION srch_scan_cme_cell_info_hs

DESCRIPTION:
  Scans all serving cells and sends cell ptrs to CME module when invoked

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_scan_cme_cell_info_hs(void);

/*===========================================================================

FUNCTION srch_scan_cme_cell_info

DESCRIPTION:
  Scans all relevant srch sets and sends cell ptrs to CME module every triage cycle

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_scan_cme_cell_info(void);

/*===========================================================================

FUNCTION srch_cme_configure_cme_non_aset_cell

DESCRIPTION:
  Function call from CME module to initiate QICE cell setup with FW.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: if QICE cell setup initiated
  FALSE: otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_cme_configure_cme_non_aset_cell(uint16 psc, uint16 freq);

/*===========================================================================

FUNCTION srch_cme_deconfigure_cme_non_aset_cell

DESCRIPTION:
  Function call from CME module to initiate deconfiguration of QICE cell setup 
  with FW.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_cme_deconfigure_cme_non_aset_cell(uint16 cell_bmsk);

/*===========================================================================

FUNCTION srch_cme_configure_cme_aset_cell

DESCRIPTION:
  Function call from CME module to initiate QICE ASET cell setup with FW.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: if QICE cell setup initiated
  FALSE: otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_cme_configure_cme_aset_cell(uint16 psc, uint16 freq);

/*===========================================================================

FUNCTION srch_cme_deconfigure_cme_aset_cell

DESCRIPTION:
  Function call from CME module to initiate deconfiguration of QICE ASET cell 
  setup with FW.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_cme_deconfigure_cme_aset_cell(uint16 cell_bmsk);
/*===========================================================================

FUNCTION srch_cme_indicate_qice_status

DESCRIPTION:
  Function call from CME module to indicate status of QICE operation.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_cme_set_qice_status(uint8 carr_idx, boolean qice_stat);

/*===========================================================================

FUNCTION srch_cme_get_qice_status

DESCRIPTION:
  Returns the status of QICE operation.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_cme_get_qice_status(uint8 carr_idx);


/*===========================================================================

FUNCTION srch_configure_scpich_cell_mimo_mode

DESCRIPTION:
	This function configures a cell for s-cpich mimo mode operation
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_configure_scpich_cell_mimo_mode(
  /*cell db idx*/
  uint8 cell_db_idx,
  /*activate or deactivate s-cpich mode*/
  boolean activate,
  /*sec cpich ovsf*/
  uint8 sec_cpich_ovsf_mimo_mode,
  /*sec cpich scr*/
  uint8 sec_cpich_scr_code_mimo_mode,
  /*function pointer to be called on*/
  SRCH_CELL_CFG_DECFG_POST_PROCESSOR srch_post_processor,
  /* MIMO S_CPICH Imbalance Scale */
  uint16 s_cpich_imb_scale);

/*===========================================================================

FUNCTION srch_handle_scpich_cell_mimo_mode_cfg_done

DESCRIPTION: 
  This function handles the completion of s-cpich mimo mode config  
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_handle_scpich_cell_mimo_mode_cfg_done(
  /*notify id*/
  uint8 notify_id, 
  /*config status*/
  boolean status);

/*===========================================================================

FUNCTION srch_configure_cell_params

DESCRIPTION:
  Interface function to the outside world to configure the cell parameters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_configure_cell_params(
  /*Reason for the cell parameter configuration*/
  srch_cell_params_cfg_enum_type cell_param_cfg,
  /*Parameters for the cell configuration*/
  srchdlif_cell_config_params_type *cell_params);

/*===========================================================================

FUNCTION srch_is_txd_det_pending

DESCRIPTION:
  Interface function to check if txd detection is pending on a cell.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if TxD detection is pending on a cell.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_is_txd_det_pending(uint16 cell_mask);

/*===========================================================================
FUNCTION
  SRCH_PERFORM_ASET_UPDATE

DESCRIPTION
  Perform the set management component of an ASET update procedure.

DEPENDENCIES
  An ASET update must not already be in progress, as indicated by
  srch_in_asu.

RETURN VALUE
  TRUE if the ASET update is complete when the function returns, FALSE if a
  search must be issued.

SIDE EFFECTS
  The ASET will be updated with the new cells, and a PN search will be
  issued if there are timing unknown cells in the new ASET.
===========================================================================*/
extern boolean srch_perform_aset_update(wl1_dl_carrcfg_cfg_param_struct* ord_cfg, wl1_dl_carrcfg_cfg_param_struct* curr_cfg);

/*===========================================================================
FUNCTION
  SRCH_NOTIFY_CARRCFG_UPON_ASET_SRCH_DONE

DESCRIPTION
  Post a local command to carrier config when an ASET update PN search is
  complete, indicating that searcher's component of the ASET update
  procedure is complete.

DEPENDENCIES
  An ASET update must be in progress, as indicated by srch_in_asu.

RETURN VALUE
  None

SIDE EFFECTS
  Will issue an ERR_FATAL crash if the command could not be posted
===========================================================================*/
extern void srch_notify_carrcfg_upon_aset_srch_done(void);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================

FUNCTION   SRCH_DC_MEAS_CONFIGURATION

DESCRIPTION
  This Function will perform all the basic operation which needs to be done
  while configuring/deconfiguring DC.

DEPENDENCIES
  None.

PARAMETERS
  If the measurement is enabled or disabled on DC carrier
 
RETURN VALUE
  None

SIDE EFFECTS
  Will update some flags, cell pointer, measurement database etc.
===========================================================================*/
void srch_dc_meas_configuration(boolean meas_enable);
#endif

/*===========================================================================
FUNCTION srch_get_serv_hs_cell

DESCRIPTION:
  This function returns the HS search cell pointer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern srch_cell_struct_type* srch_get_serv_hs_cell(uint8 carr_idx);

/*===========================================================================
FUNCTION   SRCH_CHK_AND_REG_SRCH_TO_SLP_CB

DESCRIPTION
  This Function will check if any cell is under decfg and will register a callback with cell cfg module

DEPENDENCIES
  None.

PARAMETERS
 
RETURN VALUE
  Boolean

SIDE EFFECTS
===========================================================================*/
extern boolean srch_chk_and_reg_srch_to_slp_cb(void);

/*===========================================================================
FUNCTION   SRCH_CHECK_IF_CELL_STUCK

DESCRIPTION
  This Function will check if any cell is under configuration or deconfiguration 
  for more than SRCH_CELL_STUCK_TIMER

DEPENDENCIES
  None.

PARAMETERS
 
RETURN VALUE
  Boolean

SIDE EFFECTS
===========================================================================*/
extern void srch_check_if_cell_stuck(void);

/*===========================================================================

FUNCTION srch_clear_txd_active_pending_transactions

DESCRIPTION:
  Clears the TD Detection Cell Queue if the Cell is under deconfiguration

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void srch_clear_txd_active_pending_transactions(srch_cell_struct_type *cell_ptr);

#endif
