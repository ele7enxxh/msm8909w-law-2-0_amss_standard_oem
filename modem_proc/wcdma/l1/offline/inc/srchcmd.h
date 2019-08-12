#ifndef SRCH_CMD_H
#define SRCH_CMD_H

/*===========================================================================

                         Search Command Handlers

GENERAL DESCRIPTION
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2000-2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchcmd.h_v   1.16   19 May 2002 03:34:10   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchcmd.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
12/10/14    vn      Added struct content for SRCH CONFIG CPC DRX local command.
11/17/14    psr     Removed check_cell_barred, moved to srchcrutils
05/21/14    jk      Moved the periodic measurements handling from srch_ops_cmd to a 
                    new local command.
04/16/14    bj      Remove duplicate code in split ACQ code
07/01/14    amj     Remove old ASET update variables and APIs
12/16/13    as      Porting G2W TA from Triton to Dime.
12/16/13    bj      split ACQ feature Porting from Triton to Dime.
11/21/13    jd      FET 2.0 Searcher two queue approach.
01/20/12    zr      Mainlined FEATURE_L1_CELL_RESELECTION, FEATURE_SLEEP, FEATURE_FREQ_SCAN, 
                    and FEATURE_T_BARRED_SUPPORT
03/12/11    sa      Fixed issues in W2L code.
01/31/11    sa\nd   Moved processing of INTER-RAT periodic measurements from 
                    ISR to task context.
06/01/09    sv      Mainlined the search abort code.
05/26/09    sv      Added prototype for function "srch_abort_srch_done".
05/15/09    sv      Modified prototype of function "srchacq_step1_srch_cmd"
05/08/09    sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
04/22/09    stk     Compiler warning fixes
03/26/09    sv      Changed prototype for srchacq_step1_srch_cmd.
12/09/08    sv      Mainlined FEATURE_WCDMA_OFFLINE_SEARCHER, FEATURE_RUMI_BRINGUP 
                    and FEATURE_WCDMA_DL_ENHANCED.
10/26/07    sv      Added searcher support for FEATURE_WCDMA_DL_ENHANCED.
09/26/07    nd      Added the structure for new local cmd SRCH_FS_PROC_DONE
07/16/07    nd      added extern for srchfach_process_immediate_interf_meas_req()
03/29/07    vip     Added the SFN_SFN difference value to OFF/Tm sructure
06/02/05    m       Changed step1 srch req function prototype
05/04/05   sk/sh    Added T-barred Feature support. Handling CELL_BAR_REQ 
                    from l1m
04/07/05    m       Added support for FEATURE_OPTIMIZE_SEARCH_FOR_HHO
04/21/04    cpe     srchfach_process_immediate_meas_req is changed to include
                    meas quantity for RACH
05/18/02    sh      Added Local command to start evaluation for cell
                    reselection.
05/18/02    sh      Cell Reselection related changes.
                    Changed structure srch_cphy_aset_cmd_type.
11/16/01    cpe     Changed the return value of srch_measurement_parm_update
                    from void to boolean
08/30/01    wjn     Added the function prototype "srch_fs_req_cmd()".
08/08/01    cpe     Added srchfach_process_immediate_meas_req() prototype
05/13/01    wjn     Added srch_sfn_cfn_diff_cmd_type. 
                    Deleted srch_nbch_sfn_info_type.
                    Added the function prototype "srch_sfn_cfn_diff_update()".
04/19/01    sh      Added l1_local_cmd_hdr_type to srch_nbch_sfn_info_type
                    and srch_cphy_aset_cmd_type.
03/17/01    wjn     Deleted command handler for RRC ASET_UPDATE_CMD which is
                    obsoleted by combination of RRC CELL_TRANSITION_CMD/
                    CPHY_SETUP_CMD and l1 local command SRCH_CPHY_ASET_CMD.
02/14/01    wjn     Added the prototype of function "srch_cphy_aset_update"
                    handling the local command SRCH_CPHY_ASET_CMD".
02/05/01    wjn     Deleted the prototype of the function handling 
                    CPHY_IMMEDIATE_CELL_EVAL_REQ which is no longer required
                    by RRC.
07/05/00    wjn     Created file.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "l1rrcif.h"
#include "l1def.h"
#include "wsrch.h"
#include "srchset.h"
#include "tlm.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define SRCH_COPY_SFN_WRC_DIFF(source, dest)         \
  do {         \
    if (!((source).sfn_wrc_valid))         \
    {         \
      (dest).sfn_wrc_valid = FALSE;         \
    }         \
    else         \
    {         \
      (dest).sfn_wrc_valid = TRUE;         \
      TLM_COPY_TLM_TFN((source).sfn_wrc_diff, (dest).sfn_wrc_diff);         \
    }         \
  } while(0)

/*--------------------------------------------------------------------------
                        SEARCH DONE COMMAND

This command must be sent as a parameter of funtion "l1_srch_proc_srch_done".
--------------------------------------------------------------------------*/

typedef struct
{
  l1_local_cmd_hdr_type hdr;
    /* generic command header */
  srch_queue_enum_type queue;
  /*  scheduler queue  */
  srch_intraf_queue_enum_type intraf_queue;
    /* LPQ or HPQ */
}
srch_done_cmd_type;


/*--------------------------------------------------------------------------
                      SEARCH OPERATION COMMAND

This command must be sent as a parameter of funtion "l1_srch_proc_op_cmd".
--------------------------------------------------------------------------*/

typedef struct
{
  l1_local_cmd_hdr_type hdr;
    /* generic command header */
  srch_ops_code_struct_type   ops_code;
    /* Specifies search operations to be triggered  */
}
srch_ops_cmd_type;

typedef struct
{
  l1_local_cmd_hdr_type hdr;
    /* generic command header */
  srch_periodic_meas_struct_type   periodic_rpt;
    /* Specifies search operations to be triggered  */
}
srch_periodic_meas_cmd_type;

typedef struct
{
  l1_local_cmd_hdr_type hdr;
  l1_interrat_periodic_meas_struct_type   inter_rat_meas;
}
srch_interrat_periodic_meas_cmd_type;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
typedef struct
{
  l1_local_cmd_hdr_type hdr;
  l1_interrat_periodic_meas_struct_type   inter_rat_meas;
}
srch_eutra_periodic_meas_cmd_type;
#endif

/*--------------------------------------------------------------------------
                      SEARCH LOST_SRCH COMMAND

This command must be sent as a parameter of funtion "srch_proc_lost_srch".
--------------------------------------------------------------------------*/

typedef struct
{
  l1_local_cmd_hdr_type hdr;
    /* generic command header */
  srch_lost_srch_struct_type   lost_srch;
   /* Specifies which type of search is lost  */
}
srch_lost_srch_cmd_type;



/*--------------------------------------------------------------------------
                      SFN-CFN OBSERVED TIME DIFFERENCE COMMAND
--------------------------------------------------------------------------*/
typedef struct
{
  l1_local_cmd_hdr_type hdr;
    /* Generic command header */
  boolean status;

  uint16 off;
    /* Which is (SFN - CFN_Tx) mod 256, defined in 5.1.9 of TS25.215 */
  uint32 tm;
    /* Which is ((T_UETx - T0) - T_RxSFN), defined in 5.1.9 of TS25.215 */
#ifdef FEATURE_WCDMA_CNT_C_SFN_DIFF
  /* Difference between the SFNs of serving and a neighbor cell */
  uint32 sfn_sfn_diff;
#endif    
}
srch_sfn_cfn_diff_cmd_type;

typedef struct
{
  /* Generic command header */
  l1_local_cmd_hdr_type hdr;

  /* PSC for which the results are applicable */
  uint16 psc;

  /* freq */
  uint16 freq;

  /*sfn_wrc diff computation was a success or failure*/
  boolean status;

  /*if the status flag above is true, then sfn_wrc_diff holds the difference 
     between SFN of the cell and WRC*/
/*  tlm_tfn_struct_type sfn_wrc_diff;*/
} srch_sfn_wrc_diff_cmd_done_type;

typedef enum
{
  /* While processing local command check for wake up time init */
  SRCH_CPC_DRX_INIT_WAKEUP,

  /* While processing local command check for dl drx update schedule */
  SRCH_CPC_DRX_UPDATE_DL_SCHEDULE,
  
  /* MAX number of enum */
  SRCH_CPC_DRX_PROCESS_MAX  
 }srchcpcdrx_config_cmd_enum_type;

/*--------------------------------------------------------------------------
                      PHYCHAN_SETUP ASET UPDATE COMMAND
--------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT
typedef struct
{
  l1_local_cmd_hdr_type hdr;
  /*result of the processing done TRUE=PASS , FALSE=FAIL */
  boolean status;
  uint32 mdsp_freq_scan_variance;
  uint32 mdsp_freq_scan_variance32;

}srch_fs_proc_done_cmd_type;
#endif

typedef struct
{
  l1_local_cmd_hdr_type hdr;
  /*result of the processing done TRUE=PASS , FALSE=FAIL */
  boolean status;
}srch_acq_split_action_done_cmd_type;

/*Local commad structure for SRCH CPC DRX start or DL DRX Scheduler call from searcher*/
typedef struct
{
  l1_local_cmd_hdr_type hdr;
  /**/
  srchcpcdrx_config_cmd_enum_type  cmd_type;
}srchcpcdrx_config_cmd_type;


/*===========================================================================
                          FUNCTION PROTOTYPES
                          
The followings are prototypes of functions exported to external modules.
===========================================================================*/


/*===========================================================================

FUNCTION SRCH_PROC_SRCH_DONE

DESCRIPTION
  This function processes "Search_Done" Signal. It is called by L1 manager 
  when it parses "Search_Done" signal from the local command queue. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_proc_srch_done( srch_done_cmd_type *cmd_ptr );

/*===========================================================================

FUNCTION SRCH_ABORT_SRCH_DONE

DESCRIPTION
  This function processes "Abort_Done" Signal. It is called by L1 manager 
  when it parses "Abort_Done" signal from the local queue. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_abort_srch_done(void);

/*===========================================================================

FUNCTION SRCH_PROC_SRCH_OPS

DESCRIPTION
  This function processes "Search Operation Command" from local command queue. 
  A search operation command is sent to local queue when "srch_heartbeat_isr()
  detect that a search operation is due. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_proc_srch_ops
( 
  srch_ops_cmd_type *cmd_ptr
);



/*===========================================================================

FUNCTION SRCH_PROC_LOST_SRCH

DESCRIPTION
  This function processes "Lost Search Command" from local command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_proc_lost_srch
( 
  srch_lost_srch_cmd_type *cmd_ptr
);



/*===========================================================================

FUNCTION SRCH_SFN_CFN_DIFF_UPDATE

DESCRIPTION
  This function processes the SRCH_SFN_CFN_OBSVED_DIFF_CMD .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void srch_sfn_cfn_diff_update
(
  srch_sfn_wrc_diff_cmd_done_type *cmd
);

/*===========================================================================

FUNCTION        SRCH_FS_REQ_CMD

DESCRIPTION
  This function performs frequency scan requested by RRC.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void srch_fs_req_cmd (l1_freq_scan_cmd_type *cmd);

/*===========================================================================

FUNCTION      SRCH_ACQ_REQ_CMD

DESCRIPTION  This function starts WCDMA acquisition.

             The function implementationis in srchacq.c.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_acq_req_cmd( l1_acq_cmd_type *cmd );

/*===========================================================================

FUNCTION SRCH_CELL_PARM_UPDATE

DESCRIPTION
  This function update search control parameters based on information conveyed 
  by CPHY_CELL_PARM_REQ. 

  The function implementation is in srch.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_cell_parm_update ( l1_cell_parm_cmd_type *cmd_ptr );



/*===========================================================================

FUNCTION SRCH_MEASUREMENT_PARM_UPDATE  

DESCRIPTION
  This function updates measurement control parmmeters based on 
  CPHY_MEASUREMENT_REQ. 
  
  The function implementation is in srch.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean srch_measurement_parm_update
( 
  l1_meas_cmd_type *cmd_ptr
);



/*===========================================================================

FUNCTION SRCH_CELL_SELECT_EVAL 

DESCRIPTION
  This function processes CPHY_CELL_SELECTION_REQ from RRC, and starts cell 
  selection searches on the current WCDMA frequency. 
  
  The function impleentation is in srhbch.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_cell_selection_eval
( 
  l1_cell_selec_cmd_type *cmd_ptr
);

#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif

/*===========================================================================

FUNCTION SRCH_IMMEDIATE_CELL_RESELECT

DESCRIPTION
  This function performs immediate cell reselection based on RRC reuqest 
  via CPHY_IMMEDIATE_RESELECTION_REQ.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_immediate_cell_reselect
( 
  l1_immediate_reselec_cmd_type *cmd_ptr
);


/*===========================================================================

FUNCTION SRCH_PROC_CELL_RESELECT_RSP

DESCRIPTION
  This function processes cell reselection based on RRC's response 
  CPHY_CELL_RESELECTION_RSP.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_proc_cell_reselect_rsp
( 
  l1_cell_reselect_rsp_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION SRCHCR_CELL_BAR_REQ

DESCRIPTION
  This function processes cell bar req from RRC
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srchcr_cell_bar_req 
( 
   l1_cell_bar_cmd_type *cmd
);

/*===========================================================================
  FUNCTION srchacq_request_resource

DESCRIPTION
    This function is called to request TRM lock. Duration for the lock is 
    calculated inside the function and depends on the state of execution.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_request_resource(void);

/*===========================================================================
  FUNCTION srchacq_release_resource

DESCRIPTION
    This function is called to release TRM lock

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_release_resource(void);

/*===========================================================================
  FUNCTION srchacqproc_resource_avalble_cb

DESCRIPTION
    This function is executed whenever TRM lock is granted by the WRM .WRM 
    calls a searcher CB which in turn call this function so that scheduler
    can be resumed again.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacqproc_resource_avalble_cb(boolean success);

/*===========================================================================
  FUNCTION srchacq_setup_pccpch_tti_hypo

DESCRIPTION
    This function is called once pullin is successful.This sets up the PCCPCH
    for TTI hypothesis

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_setup_pccpch_tti_hypo(void);

/*===========================================================================
  FUNCTION srchacq_rf_tune_proc

DESCRIPTION
    This function is called to to tune to ACQ frequency during acq start and also
    when TRM is acquired in between ACQ steps.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_rf_tune_proc(void);

/*===========================================================================
  FUNCTION srchacq_is_split_acq_mode_active

DESCRIPTION
    This function returns true if the split ACQ is in active state i.e. current
    ACQ_REQ is in progress with split mode.

DEPENDENCIES
    None.

RETURN VALUE
    TRUE -split ACQ active
    FALSE - split ACQ not active .

SIDE EFFECTS
    None.
===========================================================================*/
extern boolean srchacq_is_split_acq_mode_active(void);

/*===========================================================================
  FUNCTION srchacqproc_action_done_resume_scheduler

DESCRIPTION
    This function resumes the scheduler. This will be triggered whenever any action is
    done or TRM resource is available.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacqproc_action_done_resume_scheduler(boolean result);

#endif
