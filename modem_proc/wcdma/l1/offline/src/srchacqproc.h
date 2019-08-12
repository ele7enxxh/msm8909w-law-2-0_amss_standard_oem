#ifndef SRCHACQPROC_H
#define SRCHACQPROC_H

/*=============================================================================
   INCLUDED FILES
  ===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "msg.h"
#include "mdspsync.h"
#include "l1const.h"
#include "wl1m.h"
#include "l1pos.h"
#include "l1msetup.h"
#include "l1utils.h"
#include "dltri.h"
#include "demoddrv.h"
#include "dlstates.h"
#include "srchmeas.h"
#include "srchinterf.h"
#include "drx.h"
#include "srchhho.h"

#ifdef FEATURE_G2W_EVENTS_SUPPORTED
#include "event.h"
#endif /* FEATURE_G2W_EVENTS_SUPPORTED */

#include "rfm.h"


#ifdef FEATURE_RF_AFC
#include "clkrtc.h"
#endif

#include "wl1trm.h"

#ifdef FEATURE_WCDMA_COEXISTENCE_SW
#include "wl1cxm.h"
#endif /* FEATURE_WCDMA_COEXISTENCE_SW */

#include "dlcellcfg.h"
#include "srchcr.h"
#include "wl1srchdlif.h"
#include "dlacqpullin.h"
#include "time_svc.h"
#include "rrccspdb.h"
#include "cmapi.h"
#include "dlnschmgr.h"
#include "srchset.h"
#include "srchacq.h"
#include "srchsched.h"

/*=============================================================================
   PREPROCESSOR MACROS
  ===========================================================================*/
/* The total number of times to attempt a PN search in HHO or X2W acq cases */
#define SRCHACQPROC_MAX_PN_ATTEMPTS 4 

/* Total number of times to attempt pullin before declaring failure */
#define SRCHACQPROC_MAX_PULLIN_ATTEMPTS 4

/* Indicates an invalid frequency hypothesis index */
#define SRCHACQ_NO_HYPO_INDEX 0xff 

/*--------------------------------------------------------------------------
                          SPLIT ACQUSITION DECLARATIONS
--------------------------------------------------------------------------*/

/* These are different steps that are needed to complete the ACQ. These are 
primary steps and 
we may/may not release TRM in between any of these steps */

/*function ptr typedef for  primary step and pre-post actions */
typedef void (* srchacq_prim_step_proc_fptr)(srchacq_step_enum_type step);
typedef void (* srchacq_pre_post_action_proc_fptr)(void);

/*Different pre and post actions. Pls note that the sequence is important 
here as the pre
post bmask will be processed from left to right. */
typedef enum
{
 SRCHACQ_PRE_POST_ACTION_COMMON_PREPROC,
 SRCHACQ_PRE_POST_ACTION_RF_TUNE,
 SRCHACQ_PRE_POST_ACTION_PN_SRCH_REACQ,
 SRCHACQ_PRE_POST_ACTION_CELL_CFG,
 SRCHACQ_PRE_POST_ACTION_TRIAGE,
 SRCHACQ_PRE_POST_ACTION_CELL_DECFG,
 SRCHACQ_PRE_POST_ACTION_SAMP_SERV_UNFREEZ,
 SRCHACQ_PRE_POST_ACTION_LOCK_REL_ALT_CLI,
 SRCHACQ_PRE_POST_ACTION_LOCK_REL,
 SRCHACQ_PRE_POST_ACTION_LOCK_REQ,
 SRCHACQ_PRE_POST_ACTION_CHECK_DRX_EXPIRY_OR_LOCK_REQ,
 SRCHACQ_PRE_POST_ACTION_ACQ_RPT_FAIL,
 SRCHACQ_PRE_POST_ACTION_ACQ_RPT_SUCC,
 SRCHACQ_PRE_POST_ACTION_RF_TUNE_FOR_FET_CFG,
 
 SRCHACQ_PRE_POST_ACTION_MAX
}srchacq_pre_post_action_enum_type;

/* This is how each primary step will look like */
typedef struct
{
  /*bmask actions will be processed from right to left */
  uint16  pre_action_bmask;

  srchacq_step_enum_type primary_step;
  
  /*lock duration needed for this step +Preaction */
  uint16 duration_ms;
  /* array of 2 - will be picked up based on SUCCEES/FAILURE of
  Current step */
  uint16 post_actions_bmask[2];
  
}srchacq_schedule_block_struct_type;

/*Different scheduler states -signify what are we processing now -PRE/PRIM/
POST action*/
typedef enum
{
  SRCHACQ_SPLIT_STATE_INACTIVE,
  SRCHACQ_SPLIT_STATE_PREACTION,
  SRCHACQ_SPLIT_STATE_POSTACTION,
  SRCHACQ_SPLIT_STATE_PRIM_STEP,  
  SRCHACQ_SPLIT_STATE_MAX
}srchacq_split_state_enum_type;

/*scheduler substate allow us to keep scheduler in different states and stop 
scheduling further*/
typedef enum
{
  /*split ACQ not active */
  SRCHACQ_SPLIT_SUBSTATE_INACTIVE,
  /*split ACQ procedure is frozen for some break in context activity */
  SRCHACQ_SPLIT_SUBSTATE_FREEZE,
   /*split ACQ procedure is running normal */
  SRCHACQ_SPLIT_SUBSTATE_NORMAL,
  /*split ACQ procedure is waiting on TRM Resource/lock */
  SRCHACQ_SPLIT_SUBSTATE_WAITING_ON_RESOURCE,  
  /* PRE action has failed and we cannot continue further */
  SRCHACQ_SPLIT_SUBSTATE_FAILED,
  
  SRCHACQ_SPLIT_SUBSTATE_MAX
}srchacq_split_substate_enum_type;

/* This is tha split ACQ database and it has all the information regarding 
the split ACQ funtionality*/
typedef struct
{
  /* state of the scheduler */
  srchacq_split_state_enum_type curr_state; 

  srchacq_split_substate_enum_type substate;
  /* store the stage of ACQ where we are currently */
  uint16 prim_step_idx;
  uint16 pre_post_step_idx;
  uint16 prim_data;
  boolean prim_step_result;
  boolean no_resource_failure;
  /* schedule to be followed-*/
  srchacq_schedule_block_struct_type *schedule_ptr;
    /* Primary step processors */
  srchacq_prim_step_proc_fptr prim_step_handler;
  /* Pre/Post action step processors*/
  srchacq_pre_post_action_proc_fptr *prepost_action_handler;
  
}srchacq_split_db_struct_type;

extern srchacq_split_db_struct_type srchacq_split_db;



#define ACTION_BMASK(str) (1 << SRCHACQ_PRE_POST_ACTION_##str)

#define SRCHACQ_IS_SPLIT_ACQ_ACTIVE() (srchacq_split_db.curr_state != SRCHACQ_SPLIT_STATE_INACTIVE)
#define SRCHACQ_IS_WAITING_ON_RESOURCE()(SRCHACQ_IS_SPLIT_ACQ_ACTIVE() && (srchacq_split_db.substate == SRCHACQ_SPLIT_SUBSTATE_WAITING_ON_RESOURCE))
#define SRCHACQ_IS_SPLIT_ACQ_ACTIVE_FOR_SRCHSCHED_ACQ_CLIENT() (SRCHACQ_IS_SPLIT_ACQ_ACTIVE() && (SRCHSCHED_ACQ_CLIENT == srchsched_get_curr_client()))
#define SRCHACQ_IS_SPLIT_ACQ_ACTIVE_FOR_SRCHSCHED_BPLMN_CLIENT() (SRCHACQ_IS_SPLIT_ACQ_ACTIVE() && (SRCHSCHED_BPLMN_CLIENT == srchsched_get_curr_client()))
#define SRCHACQ_IS_SPLIT_ACQ_ACTIVE_FOR_SRCHSCHED_CONFIG_MODE()(SRCHACQ_IS_SPLIT_ACQ_ACTIVE() && (SRCHSCHED_CONFIG_MODE == srchsched_get_curr_mode()))

#define SRCHACQ_SET_CURR_STEP_RESULT(rslt) (srchacq_split_db.prim_step_result = rslt)
           
#define SRCHACQ_GET_POST_ACTION_BMASK(curr_step,result)              \
           (result)? srchacq_split_db.schedule_ptr[curr_step].post_actions_bmask[1]:  \
               srchacq_split_db.schedule_ptr[curr_step].post_actions_bmask[0]


#define SRCHACQ_IS_SPLIT_ACQ_COMPLETE_AND_SUCCESSFUL(p_idx)                                     \
 (((srchacq_split_db.schedule_ptr[p_idx].primary_step == SRCHACQ_STEP_PCCPCH_SETUP_CONT)||        \
   (srchacq_split_db.schedule_ptr[p_idx].primary_step == SRCHACQ_STEP_PCCPCH_SETUP_AFTER_GAP))    \
   &&(srchacq_split_db.curr_state == SRCHACQ_SPLIT_STATE_POSTACTION)                            \
   &&(srchacq_split_db.prim_step_result == TRUE))


#define SRCHACQ_SPLIT_SCHEDULE_PRE_POST_ACTION(pre_post_idx)            \
do{                                                                     \
 ASSERT(srchacq_split_db.prepost_action_handler[pre_post_idx]!=NULL);   \
 srchacq_split_db.prepost_action_handler[pre_post_idx]();               \
}while(0)               

#define SRCHACQ_GET_RESOURCE_DURATION() (0xFF) //FIXME with the WRM API
#define SRCHACQ_SPLIT_STATE_CHANGE(new_state)                  \
do{                                                            \
 MSG_FATAL("SPLIT_ACQ STATE CHANGE %d --> %d", srchacq_split_db.curr_state,new_state,0);  \
 srchacq_split_db.curr_state = new_state;                      \
 srchacq_split_db.pre_post_step_idx = 0;                       \
}while(0);

/*=============================================================================
   PUBLIC METHODS
  ===========================================================================*/
  
extern const uint16 srchacqproc_map[SRCHACQPROC_MAX];

/*=============================================================================
  FUNCTION
    SRCHACQPROC_INIT
  
  DESCRIPTION
    This function initializes the acqusition sequencer.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchacqproc_init(void);

/*=============================================================================
  FUNCTION
    SRCHACQPROC_HANDLE_ACQ_REQ_CMD
  
  DESCRIPTION
    When RRC sends L1 an acquisition command, the procedure to be performed is
    not yet known. This function determines the procedure to perform and
    initiates the first step with the scheduler.
  
  DEPENDENCIES
    Assumes the acquisition module has already been initialized via
    srchacq_init
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchacqproc_handle_acq_req_cmd(l1_acq_cmd_type* cmd);

/*=============================================================================
  FUNCTION
    SRCHACQPROC_START_PROC
  
  DESCRIPTION
    This function starts the specified high-level acquisition procedure. It is
    a non-blocking call, since all procedures involve scheduling searches with
    firmware. 
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the specified sequence was able to be started, FALSE otherwise.
  
  SIDE EFFECTS
    None 
=============================================================================*/
boolean srchacqproc_start_proc(srchacqproc_enum_type seq);

/*=============================================================================
   EXTERNAL MODULE CALLBACKS
  ===========================================================================*/

/*=============================================================================
  FUNCTION
    SRCHACQPROC_HANDLE_STEP1_NASTT_DONE
  
  DESCRIPTION
    This function is called by the scheduler module when a Step 1/NAS-TT
    command has been completed. Status information is available as a
    parameter, and the actual search results can be queried from MCAL.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None 
=============================================================================*/
void srchacqproc_handle_step1_nastt_done(srchsched_step1_nastt_status_struct_type* results);

/*=============================================================================
  FUNCTION
    SRCHACQPROC_HANDLE_PN_DONE
  
  DESCRIPTION
    This function is called by the scheduler module when a scheduled PN
    command has been completed. Status information is available as a
    parameter, and the actual search results can be queried from MCAL.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None 
=============================================================================*/
void srchacqproc_handle_pn_done(srch_done_cmd_type* cmd);

/*=============================================================================
  FUNCTION
    SRCHACQPROC_HANDLE_PULLIN_DONE
  
  DESCRIPTION
    This function is called from downlink when the scheduled pull-in procedure 
    completes.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchacqproc_handle_pullin_done(dl_acq_pullin_done_cmd_type* cmd);

/*=============================================================================
  FUNCTION
    SRCHACQPROC_HANDLE_FRAME_AMBIG_RES_DONE
  
  DESCRIPTION
    This function is called from downlink when the scheduled frame ambiguity 
    resolution procedure completes.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchacqproc_handle_frame_ambig_res_done(boolean success, boolean sttd);

/*=============================================================================
  FUNCTION
    SRCHACQPROC_HANDLE_CELL_DECFG_DONE
  
  DESCRIPTION
    If frame ambiguity resolution fails, the cell that was attempted must be
    deconfigured before the step can be considered completed. Cell
    deconfiguration is a firmware operation, so firmware would be busy if a
    search was to be scheduled before deconfiguration completes. So,
    srchacq_handle_frame_ambig_res_done will schedule the cell deconfiguration
    on failure, and downlink will call this function once the cell
    deconfiguration is done.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchacqproc_handle_cell_decfg_done(void);

void srchacqproc_handle_hho_cell_cfg(void);
void srchacqproc_update_step1_nastt_status(srchsched_step1_nastt_status_struct_type* results);

extern void srchacqproc_start_step(srchacq_step_enum_type step);


extern void srchacqproc_handle_proc_failure(void);
extern void srchacqproc_handle_proc_success(void);




/*Split ACQ function declaration*/
extern void srchacqproc_split_db_activate
(
  srchacq_schedule_block_struct_type *schedule_ptr,
  srchacq_prim_step_proc_fptr prim_step_handler,
  srchacq_pre_post_action_proc_fptr *prepost_action_handler
 );

extern void srchacqproc_split_db_reset(void);
extern void srchacqproc_split_scheduler(void);
extern void srchacqproc_action_done_resume_scheduler(boolean result);
extern void srchacqproc_idle_resume_acq_success(void);

#ifdef FEATURE_DUAL_SIM
extern void srchacqproc_resume_processing_successful_sttd_done(void);
#endif

#endif /* !SRCHACQPROC_H */

