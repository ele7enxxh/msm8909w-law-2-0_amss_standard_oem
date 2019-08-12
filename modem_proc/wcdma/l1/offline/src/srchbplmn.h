#ifndef SRCH_BPLMN_H
#define SRCH_BPLMN_H
/*===========================================================================

   S R C H    B P L M N    S T A T E   R E L A T E D   D E C L A R A T I O N S

DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


REFERENCES
  WCDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2000 to 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchbplmn.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/15   bj      Removed the full ACQ code implementation after split ACQ PN Resume failure.
09/08/14   bj      DRX_TIMER_EXPIRY cmd handling in ACQ state for spllit acq case. 
08/23/14   bj      Added split acq across DRX support in BPLMN state.
08/21/14   da      Fixed compiler warnings
04/16/14   bj      BPLMN split ACQ code changes
09/12/13   jd      Searcher redesign
09/19/11   kcm     Added a postcall back function 
                   srchbplmn_resume_on_serving_cell_cb
03/13/09   sv      Corrected a typo. 
12/09/08   sv      Mainlined FEATURE_WCDMA_OFFLINE_SEARCHER, FEATURE_RUMI_BRINGUP 
                   and FEATURE_WCDMA_DL_ENHANCED.
10/26/07   sv      Added searcher support for FEATURE_WCDMA_DL_ENHANCED.
11/24/05   nd/gv   Code cleanup
11/08.05   nd/gv   Checkin the code onto mainline.
11/07/05   nd      initial creation for the feature W to W BPLMN srch

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*--------------------------------------------------------------------------
                          BPLMN SEARCH SUBSTATES

--------------------------------------------------------------------------*/

typedef enum
{
  SRCH_BPLMN_INACTIVE,
  SRCH_BPLMN_ACQ_STEP1,
  SRCH_BPLMN_ACQ_STEP_NASTT,
  SRCH_BPLMN_ACQ_STEP_PN,
  SRCH_BPLMN_ACQ_KNOWN_POS_PN,
  SRCH_BPLMN_ACQ_PULLIN,
  SRCH_BPLMN_ACQ_STTD,
  SRCH_BPLMN_NUM_STATES
}srch_bplmn_state_enum_type;

extern srch_bplmn_state_enum_type srchbplmn_substate ;

/* Structure to save any serving cell related information for restoring once BPLMN
   search is over */
typedef struct
{
  /* position from this finger will be taken by searcher for reacq search */
  uint8 fing_to_strongest_path;
  /* serving cell frequecny */
  uint16 serv_cell_freq;
  /* detected set count for the serving cell */
  uint8 serv_freq_uset_cnt;
  /*save the SYNC_N set information...count*/
  uint8 serv_freq_sync_nset_cnt;
}srchbplmn_saved_info_struct_type;

/* step3 peaks from BPLMN searches are stored in this structure*/
typedef struct 
{
  uint32  max_eng;
  uint32  max_pos;
}srchbplmn_cell_peak_struct_type;

#ifdef FEATURE_WCDMA_L1_SPLIT_ACQ_ACROSS_BPLMN_CYCLES

/* Structure to save NASTT detected pilot in case of split acq bplmn, 
    we would use this info in next drx cycle*/
typedef struct
{
  /* acquistion request frequecny */
  uint16 saved_acq_req_freq;
  uint16  saved_nastt_pilot_cnt;
  srch_pilot_peak_struct_type  saved_nastt_pilots[SRCH_MAX_NASTT_PILOTS];
  timetick_type sp_acq_res_start_timeticks;

}srchbplmn_split_acq_resume_struct_type;

typedef struct
{
  uint32 bplmn_sp_acq_cnt;
  uint32 bplmn_sp_acq_resume_cnt;
  uint32 bplmn_sp_acq_resume_pass_cnt;
  uint32 bplmn_sp_acq_resume_pn_fail_cnt;
  uint32 bplmn_sp_acq_resume_pull_fail_cnt;

}srchbplmn_split_acq_resume_stat_struct_type;

#endif

/*This global variable is defined to check whether the BPLMN acq 
  was successful or not. This flag is checked when trying to establish 
  physical channel to demodulate SIBs. If this is not set to TRUE then 
  a CNF is sent to RRC with a failure for the setup request.*/
extern boolean srchbplmn_acq_status;
extern boolean srch_drx_tmr_expired;
/*===========================================================================

FUNCTION        SRCHBPLMN_FS_REQ_CMD

DESCRIPTION
  Freq scan for BPLMN searches

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srchbplmn_fs_req_cmd(l1_freq_scan_cmd_type *cmd);


/*===========================================================================

FUNCTION SRCHBPLMN_ACQ_REQ_CMD

DESCRIPTION
  This function first check if there is any step of neighbor search command
  to be issued following an ASET (HPQ) search done. if yes, the function
  issues the search command to DSP. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchbplmn_acq_req_cmd(l1_acq_cmd_type *cmd);


/*===========================================================================

FUNCTION SRCHBPLMN_SAVE_SERVING_CELL_INFO

DESCRIPTION
  save the serving cell parameters before starting the BPLMN searches

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchbplmn_save_serving_cell_info(void);


/*===========================================================================

FUNCTION SRCHBPLMN_RESTORE_SERVING_CELL_INFO

DESCRIPTION
  restore the serving cell parameters after BPLMN searching is over.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchbplmn_restore_serving_cell_info(void);


/*===========================================================================

FUNCTION SRCHBPLMN_ACQ_PULLIN_DONE

DESCRIPTION
  This function is called by Demod module once pull-in operation is finished.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchbplmn_acq_pullin_done( dl_acq_pullin_done_cmd_type* cmd );

/*===========================================================================

FUNCTION SRCHBPLMN_PROC_SRCH_OPS

DESCRIPTION
  This function processes "srch_ops_cmd" in BPLMN state

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchbplmn_proc_srch_ops(srch_ops_cmd_type  *cmd_ptr);

/*===========================================================================

FUNCTION       SRCHBPLMN_PROC_SRCH_DONE

DESCRIPTION
  This function starts the processing of step1/2/3 or list search .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchbplmn_proc_srch_done( srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION      SRCHBPLMN_ADD_PCCPCH_N

DESCRIPTION
  This function is called to set up the PCCPCH on the BPLMN cell just acquired. The 
  PCCPCH is setup to resolve TTI ambiguity first and then reading SIBs for upper layers.
  It will be setup using concurrent PCCPCH_N with reason SFN_SFN_diff. The reason to use
  the SFN_SFN_DIFF as the reason to set up the neighbor PCCPCH channel was because
  it will allow to use the existing code without much modification.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicating if the channel was added successfully or not.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchbplmn_add_pccpch_n
(
  dl_phychan_add_type* phch_cmd,
  /* Freq of the cell on which the PCCPCH will be setup*/
  uint16 freq,
  /*the cell on which the PCCPCH will be setup*/
  srch_cell_struct_type* nghbr_cell_ptr
);

extern void srchbplmn_dereg_aset_postproc_resume_go_to_sleep(void);

/*===========================================================================

FUNCTION srchbplmn_resume_on_serving_cell_cb

DESCRIPTION
  This post call back function is executed to tune the RF to the serving cell 
  frequency and inform DRX that the BPLMN searching is done.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchbplmn_resume_on_serving_cell_cb(void);

void srchbplmn_handle_step1_nastt_done(srchsched_step1_nastt_status_struct_type* results);
void srchbplmn_handle_pn_done(srch_done_cmd_type* srch_cmd);
void srchbplmn_handle_acq_pn_done(void);
extern void srch_handle_drx_expiry_cmd(void);

#ifdef FEATURE_WCDMA_L1_SPLIT_ACQ_ACROSS_BPLMN_CYCLES

extern void srchbplmn_reset_split_acq_resume_db(void);

#endif

/*===========================================================================

FUNCTION SRCHBPLMN_ACQ_RPT

DESCRIPTION
  This function report acqusition result to RRC. acq_status "TRUE" indicates
  that acquisition succeeded; and acq_status "FALSE" indicates the acqusition
  failed.

DEPENDENCIES
  assumed that .FEATURE_BARRED_CELL_DURING_CELL_SELECTION is always defined

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchbplmn_acq_rpt (boolean acq_status);

#endif
