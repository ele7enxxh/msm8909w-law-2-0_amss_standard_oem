#ifndef DLBCCHMGR_H
#define DLBCCHMGR_H

/*===========================================================================
                 D L B C C H M G R . H

GENERAL DESCRIPTION
  This file contains the declarations and definitions of dlbcchmgr needed
  across modules.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dlbcchmgr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
04/01/14    km      Rxd/ACQ changes
12/16/13    as      Porting G2W TA from Triton to Dime.
02/20/13    hk      Mainlined FEATURE_WCDMA_DRX_MANAGER
10/09/12    sr      Changes to initiate schedule update after receiving every BCH block
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/23/12    kcm     L/G to W handover optimizations
05/15/12    hk      Support for Inter SIB sleep for SIB7 read in EDRX-FACH state
01/26/12    vs      Mainlining of the features FEATURE_PCCPCH_SETUP_DECODE_ENHANCEMENT
                    & DL_BCH_CRC_STATUS_TYPE
05/25/09    hk      Fixed compiler warnings
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/22/08    hk      Added funtion declarations to manage sttd info in
                    dl_bcch_cfg_db, Removed the macro DL_BCCH_GET_STTD_INFO
01/16/08    vp      Added support for PCCPCH weak indication
01/15/08    vp      Added code comments, Merged dynamic and static db in to a 
                    single config db, Added support for "Serving cell change abort procedure"
11/06/07    vp      Moved DRX handle from dlchmgr to channel specific managers
                    and added shared channel support in sccpchmgr.
09/06/07    sv      Renamed dl_bcch_is_conc_bch_svc_on_same_cell to dl_bcch_is_bch_svc_on_cell.
08/23/07    vsr     moved in variables, dl_acq_bch_decode_done_cmd() and made include file
                    changes related to removal of dlcmd.* files
08/21/07    scm     Move inter-SIB sleep support from dlstates.[ch] to here.
04/25/07    vp/vsr  Lint Fixes
04/05/07    vb      externed function to provide cell info on which nsib is read
03/30/07    vb      Added support to handle cocnurrent bch svc on the same cell
03/02/07    vp      Added support to send setup_done to l1m from bcchmgr
02/21/07    vp      Initial version
===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "l1const.h"
#include "dlchmgr.h"
#include "dlphch.h"
#include "wl1drxmanager.h"

/*Macro to identify the number of TTI hypothesis that is run to identify the 
pccpch TTI boundary*/
#define DL_BCCH_MAX_ALLOWED_CFN_PER_PCCPCH 2

/*Macro to identify the maximum number of PCCPCH allowed at a time*/
#define DL_BCCH_MAX_ALLOWED_PCCPCH 2

/*Macro to identify the bcch manager cfg db size*/
#define DL_BCCH_CFG_DB_SIZE (DL_BCCH_MAX_ALLOWED_PCCPCH)

/*Macro to identify the bcch manager static db size*/
#define DL_BCCH_STAT_DB_SIZE (DL_BCCH_SETUP_TYPE_TOTAL_NUM)

/*The value of this macro determines the BLER window size. If this value 
becomes larger than 64, then the variable bler_running_status in bcch config 
db.should be changed.*/
#define DL_BCCH_BLER_STATE_MC_WINDOW_SIZE 8

#define DL_BCCH_BLER_STATE_MC_FAIL_THRESHOLD 3

#define DL_BCCH_PCCPCH_FAIL_CNT_TRIG 3

/*Enum to index in to the bcch manager static db*/
typedef enum {
DL_BCCH_SETUP_TYPE_SFN_WRC=0,
DL_BCCH_SETUP_TYPE_ACQ_BCH,
DL_BCCH_SETUP_TYPE_BCH_S_SIB_READ,
DL_BCCH_SETUP_TYPE_BCH_N_SIB_READ,
DL_BCCH_SETUP_TYPE_BCH_X2W_HHO,
DL_BCCH_SETUP_TYPE_TOTAL_NUM
} dl_bcch_static_db_idx_enum_type;

/*Structure that holds dynamic (run-time) information pertaining to 
a BCCH channel*/
typedef struct {
/*Whether the bcch dynamic entry is in use*/
boolean in_use;

/*Index for the BCCH channel in to the DlChMgr cfg db*/
dlchmgr_cfg_db_idx_type chmgr_cfg_db_idx;

/*reslam cfg db idx*/
dlchmgr_cfg_db_idx_type chmgr_reslam_cfg_db_idx;

/*Index for the BCCH channel in to the bcch manager static db*/
dl_bcch_static_db_idx_enum_type static_db_idx;

/*Whether the ccr state machine is finished for the bcch*/
boolean crc_statemc_finished;

/*sttd is used or not in this cell*/
boolean sttd_used;

/*rpt_cnt gives the number of CRC status commands received so 
far for the bcch*/
uint16 rpt_cnt;

/*pass_cnt gives the number of passed CRC status commands received so far.*/
uint16 pass_cnt[DL_BCCH_MAX_ALLOWED_CFN_PER_PCCPCH];

/*the computed sfn_wrc diff for the bcch*/
tlm_tfn_struct_type  sfn_wrc_diff;

/*the result of the bcch crc state machine*/
boolean crc_status;

/*drx handle associated with the bcch*/
drx_entity_handle_type drx_handle;

/*total blocks received so far in the running window*/
uint16 bler_state_mc_total_blks;

/*total erroneous blocks received in the running window*/
uint16 bler_state_mc_err_blks;

/*bler window current position; This gives the oldest position which has to be 
removed from the cummulative statistics*/
uint16 bler_window_curr_index;

/*This variable holds the previous "DL_BCCH_BLER_STATE_MC_WINDOW_SIZE" 
number of bler status.Note that DL_BCCH_BLER_STATE_MC_WINDOW_SIZE cannot be 
larger than 64 as long as bler_running_status size is only 64 bits.*/
uint64 bler_running_status;
} dl_bcch_dyn_db_struct_type;

/*Structure that holds the semi-static information pertaining to a type of bcch*/
typedef struct {
  /*run bler measurement for this for this pccpch*/
  boolean run_bler_meas;

  /*Whether to invoke schedule update*/
  boolean schedule_update_needed;

  /*Whether sfn_wrc computation is needed*/
  boolean sfn_wrc_diff_needed;

  /*Whether BCH has to be turned ON to MAC*/
  boolean turn_bch_on_to_mac;

  /*Whether ALM has to be notified to change the alignment*/
  boolean notify_alm;

  /*Whether dual CFN setup is needed in mDSP to do a TTI hypothesis*/
  boolean dual_cfn_setup;

  /*Whether a channel reslam is needed to correct the CFN in STMR*/
  boolean reslam_needed;

  /*drop the hwch*/
  boolean drop_hwch_needed;

  /*macro that holds the minimum pass count needed for a bcch*/
  uint16 min_pass_cnt;

  /*macro that holds the maximum report count allowed for a bcch*/
  uint16 max_rpt_cnt;

  /*Type of entity to be used in registration with DRX*/
  drx_entity_type drx_type;

  /*Call back functions to be registered with DRX*/
  channel_cb_struct_type *drx_cb_functions;
} dl_bcch_static_db_struct_type;

/*Structure that holds the both the dynamic and semi-static information 
pertaining to sccpch*/
typedef struct {
  dl_bcch_static_db_struct_type stat_db;

  dl_bcch_dyn_db_struct_type dyn_db;
} dl_bcch_cfg_struct_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: DL_ACQ_STTD_DONE_CMD_TYPE

This struture contains the parameter for the DL_ACQ_STTD_DONE_CMD.
--------------------------------------------------------------------------*/
typedef struct {
  l1_local_cmd_hdr_type hdr;

  /* the status of the STTD detection procedure, TRUE if successful */
  boolean status;

} dl_acq_bch_decode_done_cmd_type;


/******************************************************************************
                      COMMAND: DL_BCH_CRC_STATUS_CMD
******************************************************************************/
/*--------------------------------------------------------------------------
                  STRUCTURE: DL_TRBLK_CRC_STATUS_TYPE

This structure contains parameters for the DL_TRBLK_CRC_STATUS_CMD.
--------------------------------------------------------------------------*/



typedef struct {
  l1_local_cmd_hdr_type hdr;

  /* which CCTrCH table index is being reported on */
  uint8 cctrch_idx;

  /* set to TRUE if this frame's BCH transport block passed it's CRC check. */
  boolean pass;

  /* energy seen by decoder */
  uint32 eng;

  /* SFN as decoded by DL Dec from the BCH header */
  uint16 sfn;

  /* CFN at the instant of SFN decoding */
  uint16 cfn;

  /* tm as calcualted from finger position */
  uint16 tm;

  /* combiner channel number */
  uint8 hwch;

  /* physical channel type */
  l1_dl_phychan_enum_type phch;

  /* physical channel operation reason type */
  dl_phychan_action_reason_enum_type reason;

} dl_bch_crc_status_type;


/******************************************************************************
                      COMMAND: DL_BCH_BLER_STATUS_CMD
******************************************************************************/
/*--------------------------------------------------------------------------
                  STRUCTURE: DL_TRBLK_BLER_STATUS_TYPE

This structure contains parameters for the DL_TRBLK_BLER_STATUS_CMD.
--------------------------------------------------------------------------*/
typedef struct {
  l1_local_cmd_hdr_type hdr;

  /* which CCTrCH table index is being reported on */
  uint8 cctrch_idx;
} dl_bcch_weak_ind_cmd_type;

extern dl_bcch_cfg_struct_type dl_bcch_cfg_db[DL_BCCH_CFG_DB_SIZE];

extern channel_cb_struct_type  dl_bcch_drx_cb_functions;

/* This structure holds the reponse functions for the ACQ BCH*/
extern dlchmgr_resp_func_cb_struct_type dl_bcch_acq_bch_resp_func_cb;

/* This structure holds the reponse functions for the SFN_WRC BCH*/
extern dlchmgr_resp_func_cb_struct_type dl_bcch_sfn_wrc_resp_func_cb;

/* This structure holds the reponse functions for the S_BCH setup by RRC*/
extern dlchmgr_resp_func_cb_struct_type dl_bcch_sib_s_resp_func_cb;

/* This structure holds the reponse functions for the N_BCH setup by RRC*/
extern dlchmgr_resp_func_cb_struct_type dl_bcch_sib_n_resp_func_cb;

/* This structure holds the reponse functions for the X2W HHO setup*/
extern dlchmgr_resp_func_cb_struct_type dl_bcch_x2w_hho_resp_func_cb;

extern dlchmgr_idx_mgmt_func_struct_type dl_bcch_idx_mgmt_func;

extern dlchmgr_op_cb_func_struct_type dl_bcch_op_cb_func;

#ifdef FEATURE_DUAL_SIM
/* SIB schedule received from RRC to do inter-SIB sleep.  Points to an array
** of 4096 bits, or 512 bytes.  A 1 bit at a particular SFN offset into the
** bit array means we should read the SIB data at that SFN. */
extern uint8  *sib_sfn_bit_mask;
#endif

/*===========================================================================
FUNCTION        dl_bcch_process_bch_crc_status_cmd

DESCRIPTION     This is the local command handler for the CRC status command.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_bcch_process_bch_crc_status_cmd(dl_bch_crc_status_type *cmd);

/*===========================================================================
FUNCTION        dl_bch_status_rpt

DESCRIPTION     This function posts a CRC status command to L1M.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_bch_status_rpt (uint8 cctrch_table_index, boolean crc_status,
  uint32 eng, uint16 sfn, uint8 cfn);

/*===========================================================================
FUNCTION        dl_bcch_reslam_cb

DESCRIPTION     This function handles all the post-reslam operations.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_bcch_reslam_cb(uint8 cctrch_idx, l1_dl_phychan_enum_type phch_type,
  dl_phch_cfg_handle_type phch_handle, boolean status);

/*===========================================================================
FUNCTION        dl_bcch_dealloc_dyn_db_idx

DESCRIPTION     This function frees entry in the bcch manager dynamic db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_bcch_dealloc_cfg_db_idx(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_alloc_dyn_db_idx

DESCRIPTION     This function finds a free entry in the bcch manager dynamic 
                db and allocates it to the caller.

DEPENDENCIES    None

RETURN VALUE
                cfg-db-idx: The allotted index in bcch manager dynamic db.

SIDE EFFECTS    None.
===========================================================================*/
extern dl_ch_spec_cfg_db_idx_type dl_bcch_alloc_cfg_db_idx(dlchmgr_cfg_db_idx_type chmgr_cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_pre_add_cb

DESCRIPTION     This function does any processing needed before the addition 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All pre-add operations were successful.
                FALSE: At least one pre-add operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_bcch_pre_add_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx,
  l1_dl_phychan_db_struct_type *phychan_db);

/*===========================================================================
FUNCTION        dl_bcch_post_add_cb

DESCRIPTION     This function does any processing needed after the addition 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All post-add operations were successful.
                FALSE: At least one post-add operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_bcch_post_add_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_init

DESCRIPTION     This function initializes the bcch manager. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_bcch_init( void );

/*===========================================================================
FUNCTION        L1M_CPHY_SIB_SCHED_CHANGE_IND

DESCRIPTION
  We got the CPHY_SIB_SCHED_CHANGE_IND primitive from RRC.  
  This means WL1 is supposed to change schedules for receiving data on the 
  N_PCCPCH. This schedule consists of a bitmask indicating which SIB sfns
  are required.

PARAMETERS
  l1_ext_cmd_type *cmd_ptr - Pointer to CPHY_SIB_SCHED_CHANGE_IND command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void l1m_cphy_sib_sched_change_ind(const l1_ext_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION        DL_SIB_SAVE_CHAN

DESCRIPTION
  This function is called when we start processing a CPHY_SETUP_REQ for
  NBCH carrying PCCPCH_N from RRC.  It saves away the inter-SIB sleep
  schedule passed down from RRC.

PARAMETERS
  setup - Current CPHY_SETUP_REQ command from RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dl_sib_save_chan(const l1_setup_cmd_type* setup);

/*===========================================================================
FUNCTION   DL_BCCH_NEXT_FRAME_WANTED_CB

DESCRIPTION
  DRX Manager invokes this callback routine to find out how long a BCCH wants
  to be active or sleeping, whichever the case may be.  So this function
  effectively implements the scheduling of inter-SIB sleep.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE on success (always returns TRUE).
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dl_bcch_next_frame_wanted_cb
(
  drx_entity_handle_type          entity_handle,
  drx_manager_sleep_struct_type  *sleep_info_ptr
);

/*===========================================================================
FUNCTION        dl_bcch_pre_drop_cb

DESCRIPTION     This function does any processing needed before the drop 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All pre-drop operations were successful.
                FALSE: At least one pre-drop operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_bcch_pre_drop_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_crc_state_mc_post_process

DESCRIPTION     This function finds whether a BCCH channel is setup on the 
                passed psc and frequency.

DEPENDENCIES    None

RETURN VALUE
                TRUE: BCCH channel is present on the cell
                FALSE: BCCH channel is not present on the cell

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_bcch_is_bch_svc_on_cell(uint16 freq, uint16 psc);

/*===========================================================================
FUNCTION        dl_bcch_get_cell_info_for_n_sib_read

DESCRIPTION     This function finds the BCCH setup by RRC for neighbor SIB 
                read and return the corresponding cell pointer 

DEPENDENCIES    None

RETURN VALUE
                cell pointer: cell pointer corresponding to the BCCH setup
                by RRC for neighbor SIB read

SIDE EFFECTS    None
===========================================================================*/
extern srch_cell_struct_type*  dl_bcch_get_cell_info_for_n_sib_read(void);

/*===========================================================================
FUNCTION        dl_bcch_post_drop_cb

DESCRIPTION     This function does any processing needed after the drop 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All post-drop operations were successful.
                FALSE: At least one post-drop operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_bcch_post_drop_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_get_cell_info_for_n_sib_read

DESCRIPTION     This function processes the ACQ BCH decode done command.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_acq_bch_decode_done_cmd (dl_acq_bch_decode_done_cmd_type *acq_bch_decode_done);

/*===========================================================================
FUNCTION        dl_bcch_pre_resume_cb

DESCRIPTION     This function does any processing needed before the resumption 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All pre-resumption operations were successful.
                FALSE: At least one pre-resumption operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_bcch_pre_resume_cb(drx_entity_handle_type entity_handle);

/*===========================================================================
FUNCTION        dl_bcch_post_resume_cb

DESCRIPTION     This function does any processing needed after the resumption 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All post-resumption operations were successful.
                FALSE: At least one post-resumption operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_bcch_post_resume_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_post_suspend_cb

DESCRIPTION     This function does any processing needed after the suspension 
                of the channel.

DEPENDENCIES    None

RETURN VALUE
                TRUE: All post-suspension operations were successful.
                FALSE: At least one post-suspension operation failed.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_bcch_post_suspend_cb(dl_ch_spec_cfg_db_idx_type cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_send_acq_bch_decode_done_cmd

DESCRIPTION     This function posts ACQ BCH decode done command.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_bcch_send_acq_bch_decode_done_cmd(dlchmgr_cfg_db_idx_type chmgr_cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_send_acq_bch_decode_done_cmd

DESCRIPTION     This function posts SFN WRC diff done command.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_bcch_send_srch_sfn_wrc_diff_done_cmd(dlchmgr_cfg_db_idx_type chmgr_cfg_db_idx);

/*===========================================================================
FUNCTION        dl_bcch_bler_stats_cb

DESCRIPTION     This function posts a local command with all the details 
                received from decoder. The local command handler shall process 
                the data in task context

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_bcch_send_pccpch_weak_indication(dl_bcch_weak_ind_cmd_type *cmd);

/*===========================================================================
FUNCTION        dl_bcch_run_bler_state_machine

DESCRIPTION     This function does all the post-processing after the CRC 
                state machine is over.

DEPENDENCIES    The value of DL_BCCH_BLER_STATE_MC_WINDOW_SIZE should be 
                less than 64. If it becomes larger than 64, then bler_running_status 
                in bcch config db shuold be changed to accomodate that.

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_bcch_run_bler_state_machine(uint8 cctrch_idx, uint8 trch_idx, uint8 rece_cfn,
  uint8 num_err_blk, uint8 num_total_blk);
/*===========================================================================
FUNCTION        dl_bcch_update_sttd_info

DESCRIPTION     This function updates the sttd information for a BCCH channel 
                in the BCCH Mgr Cfg db.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_bcch_update_sttd_info(dl_ch_spec_cfg_db_idx_type ch_spec_cfg_db_idx, boolean sttd_info);

/*===========================================================================
FUNCTION        dl_bcch_get_sttd_info

DESCRIPTION     This function returns the sttd information for a BCCH channel 
                from the BCCH Mgr Cfg db.

DEPENDENCIES    None

RETURN VALUE
                TRUE: STTD enabled on the BCCH.
                FALSE: STTD not enabled on the BCCH.

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_bcch_get_sttd_info(uint8 cctrch_idx);

extern void dl_bcch_handle_wrong_cfn(void);

#ifdef FEATURE_DUAL_SIM
void dl_bcch_reset_sib_mask(void);
#endif

#endif

