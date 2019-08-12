#ifndef WL1DRXENTITY_H
#define WL1DRXENTITY_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               WL1 DRX Entity

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2007 by Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1drxentity.h#1 $
  $DateTime: 2016/12/13 08:00:12 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/16   am      Fail PICH wakeup if IRAT searches are going on.
08/01/16   gsk     Dont consider CTCH entity if its already blocked by SRCH/BPLMN
06/16/16   skk     F3 Reduction.
02/24/15    ap     Initial check-in for WL1 QSH framework 
07/03/15   skk     Delay Sleep abort if PICH and BCCH are waking up with in few ms and DRX decides to wakeup for BCCH. 
07/03/15   skk     Trigger sleep through regular code path at the end of BPLMN.
11/24/14   gsk     SRCHZZ wakeup cleanup changes
11/20/14   rs      Fixed compiler warning
11/19/14   rs      Code changes for requesting lock with new proc during BCH decode
11/19/14   rs      New macro for waiting for 2 slots 
11/12/14   skk     Reset entity_delayed flag if wakeup is cancelled.
10/28/14   kr      externing a function
09/05/14   rs      Block PICH when Inter freq reselection is triggered 
09/03/14   skk     Unfreze sample server if PICH doesn't have valid schedule while some other entity is going to sleep.
05/08/14   rs/gsk  code changes to handle CTCH-BPLMN collisions
03/06/14   gsk     Ignore PICH wakeups during interf reselection
01/16/14   srk     Delay cphy_setup_cmd when sccpch drop is in progress.
01/07/14   gsk     Externing API to query if CTCH is blocked by search
01/03/14   rs      Returning the correct value of next entity waking up
01/03/13   gsk     Reverting the changes to delay cphy setup until L1M wakeup
12/16/13   as      Porting G2W TA from Triton to Dime.
08/22/13   kr      changing return type for delayed entity function
09/12/13   jd      Searcher redesign
12/19/13   rs      drx_entity_next_wakeup_time() function declaration added to get 
                   the next entity wakeup time.
08/23/13   dm      Change minimum sleep duration for CTCH to account for 
                   PICH/CTCH collision.
06/29/13   pv      API for resetting the DPC sync counter.
02/11/13   gsk/sr  EDRX Full Sleep initial check in.
09/08/12   pv      Extern for the RPUSH wrapper.
08/29/12   pv      New APIs for PICH timeline opts.
07/16/12   pv      Externed a variable.
07/13/11   pv      Offline PICH/Parallel Reacq fixes.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   pv      New API needed for Offline PICH/Parallel Reacq.
06/11/12   vs      MBMS feature cleanup.
06/08/12   gv      Feature HS RACH Initial code check in
03/01/11   scm     Give SRCH ability to force CTCH to stay sleeping indefinitely.
08/11/10   scm     drx_entity_wake_any_delayed_entities() returns boolean.
04/08/09   scm     Handle waking an entity right after waking from sleep.
10/03/08   scm     Add ability to drop CTCH because of a sleep abort.
10/03/08   hk      Bringing in 7k Mailine fixes
06/11/08   scm     Extern drx_entity_is_pich_suspended_or_inactive().
05/07/08   scm     Featurize MICH code with FEATURE_MODEM_MBMS.
04/24/08   scm     Eliminate FEATURE_WCDMA_DRX_ENTITY_USE_REX_TIMER define.
04/08/08   scm     PICH/MICH "smallest circle" enhancements.
03/11/08   scm     Move ALL_ENTITIES_ASLEEP define from wl1drxentity.c to .h.
08/17/07   scm     Extern drx_entity_get_cell_info() utility function.
06/13/07   scm     Tweak drx_entity_time_to_next_wakeup_usec() prototype.
                   Extern drx_entity_wake_any_delayed_entities().
04/04/07   scm     Extern drx_entity_time_to_next_wakeup_usec().
03/31/07   scm     Switch some srchzz_xxx API's to srch_xxx.
03/28/07   scm     Support new entity suspend and resume commands.
03/23/07   scm     Remove extern to drx_entity_find_psc_handle().
03/19/07   scm     Implement drx_manager_get_current_entity_mask().
03/03/07   scm     Add extern to drx_entity_get_sleep_info().
02/22/07   scm     Remove extern to drx_timeout_timer.
02/15/07   scm     Modify drx_srch_wakeup_done() parameter list.
01/24/07   scm     Initial support of PICH with new DRX Manager.
01/15/07   scm     Support schedule update indication from Channel Manager.
11/16/06   scm     Additional DRX manager functionality.
11/10/06   scm     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "wl1drxmanager.h"


/* The different commands processed by the channel state machines. */
typedef enum
{
  DRX_ENTITY_REGISTER_CMD = 0,
  DRX_ENTITY_DEREGISTER_CMD,
  DRX_ENTITY_SRCH_WAKEUP_RSP,
  DRX_ENTITY_SRCH_TO_SLEEP_RSP,
  DRX_ENTITY_DEMOD_WAKEUP_RSP,
  DRX_ENTITY_DEMOD_TO_SLEEP_RSP,
  DRX_ENTITY_TIMER_EXPIRED_CMD,
  DRX_ENTITY_SCHEDULE_UPDATE_CMD,
  DRX_ENTITY_SUSPEND_CMD,
  DRX_ENTITY_RESUME_CMD,
  DRX_ENTITY_NUM_CMDS
} drx_entity_cmd_id_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   local_cmd_hdr;

  drx_entity_handle_type  handle;
  drx_entity_cmd_id_type  cmd_id;

  boolean                 channel_awake;
} drx_register_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   local_cmd_hdr;

  drx_entity_handle_type  handle;
  drx_entity_cmd_id_type  cmd_id;
} drx_deregister_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type        local_cmd_hdr;

  drx_entity_handle_type       handle;
  drx_entity_cmd_id_type       cmd_id;

  srch_wakeup_set_struct_type  psc_woken_up;
  
} drx_srch_awake_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   local_cmd_hdr;

  drx_entity_handle_type  handle;
  drx_entity_cmd_id_type  cmd_id;

  boolean                 status;
} drx_entity_srch_to_sleep_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   local_cmd_hdr;

  drx_entity_handle_type  handle;
  drx_entity_cmd_id_type  cmd_id;

  boolean                 status;
} drx_demod_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   local_cmd_hdr;

  drx_entity_handle_type  handle;
  drx_entity_cmd_id_type  cmd_id;
} drx_timer_expired_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   local_cmd_hdr;

  drx_entity_handle_type  handle;
  drx_entity_cmd_id_type  cmd_id;
} drx_schedule_update_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   local_cmd_hdr;

  drx_entity_handle_type  handle;
  drx_entity_cmd_id_type  cmd_id;
} drx_suspend_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   local_cmd_hdr;

  drx_entity_handle_type  handle;
  drx_entity_cmd_id_type  cmd_id;
} drx_resume_cmd_type;

typedef union
{
  struct
  {
    /* Need this to put command on l1_local_cmd_q. */
    l1_local_cmd_hdr_type            local_cmd_hdr;

    drx_entity_handle_type           handle;
    drx_entity_cmd_id_type           cmd_id;
  } header;

  drx_register_cmd_type              register_cmd;
  drx_deregister_cmd_type            deregister_cmd;
  drx_srch_awake_cmd_type            srch_awake_cmd;
  drx_entity_srch_to_sleep_cmd_type  srch_to_sleep_cmd;
  drx_demod_cmd_type                 demod_cmd;
  drx_timer_expired_cmd_type         timer_expired_cmd;
  drx_schedule_update_cmd_type       schedule_update_cmd;
  drx_suspend_cmd_type               suspend_cmd;
  drx_resume_cmd_type                resume_cmd;
} drx_entity_cmd_type;

/* Whenever an entity handle is in a state other than INACTIVE or ASLEEP, the
** corresponding bit in this mask shall be set to one.  In other words, if
** awake, set the bit, otherwise clear the bit.  So when this mask is 0x00,
** all channels are asleep, and we may put the phone to sleep. */
#define ALL_ENTITIES_ASLEEP  0x00

#ifdef FEATURE_DUAL_SIM
/* This is the minimum time required for UE sleep before we will initiate
** full sleep by calling dl_prepare_for_sleep(). */
#define DRX_MINIMUM_UE_SLEEP_TIME_USEC            50000
#endif

#define TWO_SLOTS_IN_USEC        1332 

extern uint32  drx_handles_awake_mask;

extern boolean wl1drxentity_bypass_qxt_notf_nxt_cycle;

extern boolean wl1drxentity_pended_srch_pich_entity_drop;

extern boolean wl1drxentity_modify_activity_fail_at_wakeup;

#define DRX_FORCE_ENTITY_TO_SLEEP()         (wl1drxentity_modify_activity_fail_at_wakeup)
#define DRX_SET_FORCE_ENTITY_TO_SLEEP(val)  (wl1drxentity_modify_activity_fail_at_wakeup = val)

#ifdef FEATURE_DUAL_SIM
extern boolean widle_g2w_ta_rel_res_aftr_resel_fail;
#endif

/* Reduce messaging between RF wakeup and PICH setup.
   SCCPCH state transitions will be printed only if UE is paged.
   1) Avoid PICH entity state change prints while waking up.
   2) Avoid printing SCCPCH state changes if UE is not paged.
   3) Print only -> AWAKE or -> ASLEEP transitions in DCH and FACH for any entity.
   4) Print all state transitions for EDRX.  */
#define WL1_DRX_STATE_CHANGE_MSG3(str, a, b, c) \
  if (((!((a == (drx_entity_handle_type)DRX_ENTITY_PICH) && \
          (c != DRX_ENTITY_INACTIVE)                     && \
          DRX_ENTITY_IS_WAKING_UP()) \
       ) && \
       (!((a == (drx_entity_handle_type)DRX_ENTITY_PCCH) && \
          (drx_rxd_pch.page_received_this_drx == FALSE)) \
       ) && \
       (!(((l1m_state == L1M_DCH) || (l1m_state == L1M_FACH)) && \
          (c != DRX_ENTITY_ASLEEP) && (c != DRX_ENTITY_AWAKE)) \
       ) \
      ) || \
      (a == (drx_entity_handle_type)DRX_ENTITY_EDRX) \
     ) \
  { \
    WL1_MSG3(ERROR,str,a,b,c); \
  } \

/* SRCH can block CTCH activity if it wants to, like for inter-RAT searches.
   Similarly BPLMN start blocks CTCH and BPLMN stop unblocks CTCH 
** Maintain that state here. */
typedef enum
{
  DRX_ENTITY_CTCH_NORMAL_BEHAVIOR    = 0,
  DRX_ENTITY_CTCH_DROPPING_FOR_SRCH  = 0x1,
  DRX_ENTITY_CTCH_DISABLED_BY_SRCH   = 0x2,
  DRX_ENTITY_CTCH_DROPPING_FOR_BPLMN = 0x4,
  DRX_ENTITY_CTCH_DISABLED_BY_BPLMN  = 0x8        
} drx_entity_ctch_disabled_type;

extern drx_entity_ctch_disabled_type  drx_entity_disable_ctch_for_other_activities;

/*===========================================================================
FUNCTION     DRX_ENTITY_TIME_TO_NEXT_WAKEUP_USEC

DESCRIPTION
  This function is called when all channels are sleeping or inactive.  It
  returns the time in microseconds until the next entity wakes up.  It also
  returns the handle of that entity, so the caller can know why we are
  waking up.

DEPENDENCIES
  None.

PARAMETERS
  handle_waking_up - Parameter used to return the entity handle we will
                     be waking up for.

RETURN VALUE
  int32 - Microseconds until next wakeup.
===========================================================================*/
extern int32 drx_entity_time_to_next_wakeup_usec
(
  drx_entity_handle_type *handle_waking_up
);

/*===========================================================================
FUNCTION     DRX_ENTITY_PROCESS_CMD

DESCRIPTION
  This function routes incoming commands to the proper logical entity state
  machine for processing and possible state change.  There are 3 main types
  of commands handled here:
  (1) DEMOD commands and responses
  (2) SRCH responses
  (3) Activity or inactivity timer expiration.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - Command to this logical entity's state machine.

RETURN VALUE
  TRUE on success.  FALSE on failure.
===========================================================================*/
extern boolean drx_entity_process_cmd(const drx_entity_cmd_type  *cmd_ptr);

/*===========================================================================
FUNCTION     DRX_ENTITY_ALLOCATE_HANDLE

DESCRIPTION
  This function attempts to allocate a handle for a new logical entity.  If
  it successfully allocates a handle, it initializes the data associated
  with that handle.

DEPENDENCIES
  None.

PARAMETERS
  logical_entity_type - Channel type (PICH, MTCH, etc.) identifying this channel.
  dl_uarfcn           - DL channel number of cell carrying this channel
                        (maps to DL frequency of this cell).
  psc                 - Primary scrambling code of cell carrying this channel.
  cb_functions        - Pointer to structure of callback functions.

RETURN VALUE
  handle - Uniquely identifies this channel (or logical entity).  Returns
           DRX_ENTITY_HANDLE_INVALID on failure.
===========================================================================*/
extern drx_entity_handle_type drx_entity_allocate_handle
(
  drx_entity_type          entity_type,
  uint16                   dl_uarfcn,
  uint16                   psc,
  channel_cb_struct_type  *cb_functions
);

/*===========================================================================
FUNCTION     DRX_ENTITY_WAKE_ANY_DELAYED_ENTITIES

DESCRIPTION
  There are currently two ways an entity that wants to wake up is not
  allowed to wake from sleep state:

  (1) A Background PLMN search is active.  It may be on a different frequency,
      so we can't allow the sleeping entity to wake on the serving frequency.
  (2) mDSP and RF are not yet active.  We power down during true sleep.  If an
      entity wants to wake while we are in true sleep, it must wait a bit.

  This function is called to kick any delayed entities back to life because
  WL1 is now in a state to support entities going active.  If no entities have
  been delayed, then this function takes no action.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
 Mask having all the entities delayed 
===========================================================================*/
extern uint32 drx_entity_wake_any_delayed_entities(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_CHECK_FOR_CTCH_DROP

DESCRIPTION
  This function is called during a sleep abort caused by RRC command reception
  in WL1.  With FEATURE_WCDMA_NO_CTCH_IF_ANOTHER_ENTITY_ACTIVE feature, CTCH
  cannot be active if any other entity is active.  So this function will
  initiate the CTCH drop caused by a sleep abort.

  Without FEATURE_WCDMA_NO_CTCH_IF_ANOTHER_ENTITY_ACTIVE feature, this
  function does nothing.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if we have started CTCH drop for sleep abort.
          \ FALSE if we have not.
===========================================================================*/
extern boolean drx_entity_check_for_ctch_drop(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_SET_TARGET_ENTITY

DESCRIPTION
  When DRX Manager puts the UE to sleep, it calls this function to set the
  entity handle that we will wake up for in the future.  This would be the
  first entity to wake up of all sleeping entities.  Of course, usually
  it's just PICH.

DEPENDENCIES
  None.

PARAMETERS
  drx_entity_handle_type handle_waking_up - Entity waking next.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_set_target_entity(drx_entity_handle_type handle_waking_up);

/*===========================================================================
FUNCTION     DRX_ENTITY_GET_TARGET_ENTITY

DESCRIPTION
  This function returns the next entity waking  up.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  drx_entity_handle_type handle_waking_up - Entity waking next.
===========================================================================*/
extern drx_entity_handle_type drx_entity_get_target_entity(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_WAKE_TARGET_ENTITY

DESCRIPTION
  UE has just woken up from true sleep.  Since RF is on now, etc., we can
  actually wake up channel(s).  This function wakes the entity/channel that
  wants to be awake next.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_wake_target_entity(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_FIND_TYPE_HANDLE

DESCRIPTION
  This function returns the entity handle based on its type.  This function
  should only be used on entities that can only have a single instance, like
  PICH, MCCH, etc.  It should not be used on MTCH for example.

DEPENDENCIES
  None.

PARAMETERS
  entity_type - i.e. DRX_ENTITY_PICH, DRX_ENTITY_CTCH, etc.

RETURN VALUE
  drx_entity_handle_type - Handle identifying this logical entity.
===========================================================================*/
extern drx_entity_handle_type drx_entity_find_type_handle
(
  drx_entity_type entity_type
);

/*===========================================================================
FUNCTION     DRX_ENTITY_WHICH_TYPE

DESCRIPTION
  This function returns the entity type based on its handle.

DEPENDENCIES
  None.

PARAMETERS
  handle - Handle identifying this logical entity.

RETURN VALUE
  drx_entity_type - i.e. DRX_ENTITY_PICH, DRX_ENTITY_CTCH, etc.
===========================================================================*/
extern drx_entity_type drx_entity_which_type(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_GET_SLEEP_INFO

DESCRIPTION
  This function returns a pointer to the sleep information structure
  corresponding to the passed-in logical entity handle.

  Note that it doesn't check if this handle has actually been allocated.
  That is, only use this function on handles returned by
  drx_entity_allocate_handle();

DEPENDENCIES
  None.

PARAMETERS
  handle - Handle identifying this logical entity.

RETURN VALUE
  drx_manager_sleep_struct_type* - Pointer to sleep info.
===========================================================================*/
extern drx_manager_sleep_struct_type *drx_entity_get_sleep_info
(
  drx_entity_handle_type handle
);

/*===========================================================================
FUNCTION     DRX_ENTITY_GET_SLEEP_INFO

DESCRIPTION
  This function returns a pointer to the cell information structure
  corresponding to the passed-in logical entity handle.  This structure is
  filled with the primary scrambling code and frequency associated with the
  passed-in logical entity handle.

DEPENDENCIES
  None.

PARAMETERS
  handle - Handle identifying this logical entity.

RETURN VALUE
  *cell_info_ptr - Pointer to cell info.
===========================================================================*/
extern boolean drx_entity_get_cell_info
(
  drx_entity_handle_type     handle,
  tlm_cell_info_struct_type *cell_info_ptr
);

/*===========================================================================
FUNCTION     DRX_ENTITY_IS_PICH_SUSPENDED_OR_INACTIVE

DESCRIPTION
  This function pretty much just returns whether or not PICH is operational
  in WL1.  If DRX_ENTITY_PICH is suspended or not registered with DRX, this
  function returns TRUE.  Otherwise it returns FALSE.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if PICH is suspended or inactive.
  FALSE otherwise, i.e. PICH is awake or asleep.
===========================================================================*/
extern boolean drx_entity_is_pich_suspended_or_inactive(void);


/*===========================================================================
FUNCTION     DRX_ENTITY_IS_CTCH_UP

DESCRIPTION
  SRCH calls this function to check if CTCH is awake.  If so, we must drop it
  before SRCH can do stuff like inter-RAT neighbor searches.  So this function
  starts CTCH sleep and tells SRCH to wait for completion before moving on.

  If CTCH not currently awake, set flag so it won't wake up later.  Also tell
  SRCH that CTCH not awake.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - CTCH currently awake, and sleep procedure started.
  FALSE - CTCH not currently awake.
===========================================================================*/
extern boolean drx_entity_is_ctch_up(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_UNBLOCK_CTCH

DESCRIPTION
  SRCH no longer needs CTCH to stay sleeping, so resume normal CTCH behavior.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_unblock_ctch(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_RESET_ALL_ENTITIES

DESCRIPTION
  Called when WCDMA L1 is shutting down.  Makes sure all timers are stopped,
  and logical entity database gets reset.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_reset_all_entities(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_INIT

DESCRIPTION
  Logical entity database initializations performed whenever WCDMA is
  activated.  Resets all database entries and allocates a timer for each
  entry in the database.

  Must be called ONE TIME, at power-up only.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_init(void);

/*===========================================================================
FUNCTION     drx_entity_is_entity_only_active

DESCRIPTION
  This function returns the number of active entities.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
boolean drx_entity_is_entity_only_active(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION     drx_entity_is_entity_awake

DESCRIPTION
  This function is used to verify if entity is in 'AWAKE' state

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern boolean drx_entity_is_entity_awake(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION     drx_return_drx_entity_type_for_nsch

DESCRIPTION
  Returns the DRX entity type corresponding to the Phy chan type passed as argument.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  DRX entity type
===========================================================================*/
extern drx_entity_type drx_return_drx_entity_type_for_nsch(l1_dl_phychan_enum_type phch_type);

/*===========================================================================
FUNCTION     DRX_PICH_OPT_ENTITY_OPS

DESCRIPTION
  This function performs the operations for PICH opts.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_pich_opt_entity_ops(void);

/*===========================================================================
FUNCTION     DRX_PROCESS_DPC_SYNC_PROC_OPS

DESCRIPTION
  This function performs synchronization between processing
  in different HW threads.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_process_dpc_sync_proc_ops(void);

/*===========================================================================
FUNCTION     GET_PICH_FREQ

DESCRIPTION
  This function gets the frequency for the registered PICH entity.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern uint16 get_pich_freq(void);

/*===========================================================================
FUNCTION     GET_PICH_PSC

DESCRIPTION
  This function gets the psc for the registered PICH entity.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern uint16 get_pich_psc(void);

/*===========================================================================
FUNCTION     WL1DRXENTITY_PERF_RFS_UPDATE_RPUSH_WRAPPER

DESCRIPTION
  This function queues up RGS update and RPUSH in DPC task.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/

extern void wl1drxentity_perf_rgs_update_rpush_wrapper(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_VERIFY_TARGET_ENEITY_PICH

DESCRIPTION
  This function checks if the target entity waking up is PICH.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if target entity waking up is PICH, FALSE otherwise.
===========================================================================*/
extern boolean drx_entity_verify_target_entity_pich(void);

/*===========================================================================
FUNCTION     DRX_RESET_DPC_SYNC_COUNTER

DESCRIPTION
  This function resets the DPC sync counter used to synchronize parallel threads 
  during wakeup.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_reset_dpc_sync_counter(void);

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*===========================================================================
FUNCTION     DRX_ENTITY_CLR_TIMER_FOR_EDRX

DESCRIPTION
  Just a wrapper function to the proper subsystem when clearing and stopping
  the timer associated with a logical entity.

DEPENDENCIES
  None.

PARAMETERS
  Handle - Logical entity whose timer to clear/stop.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_clr_timer_for_edrx(drx_entity_handle_type handle);
#endif /*FEATURE_WCDMA_HS_FACH_DRX*/

/*===========================================================================
FUNCTION     DRX_ENTITY_TO_BE_AWAKE

DESCRIPTION
  Just a wrapper function to to know which Handle/entity to be awake.

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
Handle to be awakew next
===========================================================================*/
extern uint8 drx_entity_to_be_awake(void);

/*===========================================================================
FUNCTION     DRX_RETURN_CTCH_MIN_SLEEP_DURATION

DESCRIPTION
  API which returns the minimum sleep duration for CTCH. 
  It is max(4, 2 x CTCH/PICH collision time).

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
max(4, 2 x CTCH/PICH collision time)
===========================================================================*/
extern uint8 drx_return_ctch_min_sleep_duration(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION     DRX_ENTITY_WAKE_TARGET_ENTITY

DESCRIPTION
  UE has just woken up from true sleep.  Since RF is on now, etc., we can
  actually wake up channel(s).  This function wakes the entity/channel that
  wants to be awake next.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_wake_entity(drx_entity_handle_type handle);
/*===========================================================================
FUNCTION     DRX_ENTITY_PUT_ENTITY_TO_SLEEP

DESCRIPTION
  API puts the entity to sleep by changing entity
  state to DRX_ENTITY_ASLEEP.

DEPENDENCIES
  None.

PARAMETERS
drx_entity_handle_type handle

RETURN VALUE
TRUE - entity put to sleep successfully
FALSE - entity not put to sleep.
===========================================================================*/
extern boolean drx_entity_put_entity_to_sleep(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_GET_SLEEP_TYPE

DESCRIPTION
  API will return the current sleep type of the handle.

DEPENDENCIES
  None.

PARAMETERS
 drx_entity_handle_type

RETURN VALUE
drx_manager_sleep_type
===========================================================================*/
extern drx_manager_sleep_type drx_entity_get_sleep_type(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_CHECK_ENTITY_ASLEEP

DESCRIPTION
  API checks whether the entity is asleep or not.

DEPENDENCIES
  None.

PARAMETERS
 drx_entity_handle_type

RETURN VALUE
TRUE - entity state is asleep
FALSE - entity state is not asleep
===========================================================================*/
extern boolean drx_entity_check_entity_asleep(drx_entity_handle_type handle);
#endif

/*===========================================================================
FUNCTION     DRX_ENTITY_NEXT_WAKEUP_TIME

DESCRIPTION
  This function is called by SRCH to ask DRX Manager when one or more channels
  are going to wake up next.  SRCH passes in a bitmask of channels for DRX
  Manager to check.  This function will return the wakeup time of the channel
  that wants to wake up the soonest.

  Some bitmasks are DRX_ENTITY_MASK_PICH, DRX_ENTITY_MASK_MTCH, etc.

  Also the macro DRX_ENTITY_TYPE_TO_MASK(type) can be used to convert the
  enumerated type "drx_entity_type" into a bitmask.

  At the moment, this function only supports serving cell channels.
  TODO - add support for non serving cell channels.

DEPENDENCIES
  None.

PARAMETERS
  entity_bit_mask                - Bitmask of channels to check.
  next_wakeup (output paramater) - Absolute time (SFN/chipx8) of next wakeup.

RETURN VALUE
  int32 - Duration until next wakeup in microseconds.
===========================================================================*/
extern int32 drx_entity_next_wakeup_time
(
  uint32                          entity_bit_mask,
  drx_manager_sleep_struct_type  *next_wakeup,
  int16 *entity_wakingup_soonest
);

/*===========================================================================
FUNCTION     DRX_ENTITY_CHECK_IF_CTCH_BLOCKED_BY_SRCH

DESCRIPTION 
  API checks if CTCH is blocked by search. 

DEPENDENCIES
  None.

PARAMETERS 
  None 

RETURN VALUE 
  TRUE -- If CTCH is blocked by search
  FALSE -- otherwise

===========================================================================*/
extern boolean drx_entity_check_if_ctch_blocked_by_srch(void);

/*===========================================================================
FUNCTION     drx_is_entity_awake

DESCRIPTION
  API which checks drx_handles_awake_mask and returns if an entity is awake

DEPENDENCIES
  None.

PARAMETERS
  Entity in handle mask format

RETURN VALUE
  TRUE  -- Entity is not asleep
  FALSE -- Entity is asleep
===========================================================================*/
extern boolean drx_is_entity_awake(uint32 handle_mask);

/*===========================================================================
FUNCTION     DRX_ENTITY_ADJUST_ENTITY_SCHEDULE

DESCRIPTION
  API avoids the next wakeup of PICH/SPLIT SRCH entities if its not needed

DEPENDENCIES
  None.

PARAMETERS
  Entity in handle mask format

RETURN VALUE
  TRUE  -- Entity is not asleep
  FALSE -- Entity is asleep
===========================================================================*/
void drx_entity_adjust_entity_schedule(drx_entity_handle_type *next_wkup_entity_handle, int32 *time_to_next_wakeup_usec);

/*===========================================================================
FUNCTION     DRX_ENTITY_POST_ENTITY_PROCESS_CMD

DESCRIPTION
  API is used by drx manager to post an entity process command to drx entity module

DEPENDENCIES
  None.

PARAMETERS
  Command info

RETURN VALUE
  None
===========================================================================*/
extern void drx_entity_post_entity_process_cmd(drx_entity_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION     DRX_CLR_PICH_ENTITY_TMR_IF_RESEL_IN_PROG

DESCRIPTION
  API used to clear the PICH entity timer when Inter Freq reselection is
  triggered 

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
void drx_clr_pich_entity_tmr_if_resel_in_prog(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_RESET_ENTITY_DELAYED

DESCRIPTION
  This function is used to handle special cases where the entity_delayed flag
  for an entity needs to be cleared in an asynchronous manner.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel shutting down.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_reset_entity_delayed(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION     drx_entity_is_close_to_sleep

DESCRIPTION
  API to return TRUE or FALSE based on PO distance when cnf to rrc is sent

  triggered 

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
extern boolean drx_entity_is_close_to_sleep(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_CHK_BCH_COLLISION_WITH_ANY_ENTITY

DESCRIPTION
  This function checks if BCH wakeup is within 50 ms window of PICH or 
  split search

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE/FALSE
===========================================================================*/
extern boolean drx_entity_chk_bch_collision_with_any_entity(void);

/*===========================================================================
FUNCTION     DRX_ENTITY_SET_ENTITY_TIMER

DESCRIPTION
  This WRAPPER API is used to set the entity wakeup time for the entity handle
  passed to this function.

DEPENDENCIES
  None.

PARAMETERS
  uint32 duration

RETURN VALUE
  None
===========================================================================*/
extern void drx_entity_set_entity_timer(drx_entity_handle_type handle, uint32 sleep_duration);

/*===========================================================================
FUNCTION     DRX_ENTITY_TRIGGER_SLEEP

DESCRIPTION
  This function is called when a channel goes to sleep or is deactivated.
  It checks to see if all channels are now sleeping or inactive.  If so, the
  UE may be put to sleep to save power.

DEPENDENCIES
  None.

PARAMETERS
  is_r99_handle - TRUE if the channel just put to sleep is R99.
                \ FALSE if channel is MBMS (MCCH or MTCH).
RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_trigger_sleep( boolean is_r99_handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_GET_TIME_TILL_ENTITY_WAKEUP

DESCRIPTION
  This API is used to get time left till entity wakeup in sleep abort scenarios.

DEPENDENCIES
  None.

PARAMETERS
  uint32 duration

RETURN VALUE
  None
===========================================================================*/
extern int32 drx_entity_get_time_till_entity_wakeup(drx_entity_handle_type handle);
/*===========================================================================
FUNCTION     DRX_ENTITY_CHECK_FOR_SLEEP

DESCRIPTION
  This function is called when a channel goes to sleep or is deactivated.
  It checks to see if all channels are now sleeping or inactive.  If so, the
  UE may be put to sleep to save power.

DEPENDENCIES
  None.

PARAMETERS
  is_r99_handle - TRUE if the channel just put to sleep is R99.
                \ FALSE if channel is MBMS (MCCH or MTCH).
RETURN VALUE
  None.
===========================================================================*/
extern void drx_entity_check_for_sleep(boolean is_r99_handle);

#ifdef FEATURE_QSH_DUMP
extern void wl1_drx_entity_get_debug_info(wl1_drx_sleep_mini_debug_info_struct_type* obj);
#endif

/*=========================================================================
FUNCTION    drx_entity_adjust_ctch_schedule

DESCRIPTION
   This function updates CTCH schedule and clears timer if blocked by SRCH/BPLMN

PARAMETERS
     Entity handle and SUB ID

DEPENDENCIES
     None

RETURN VALUE
     reserve/request duration in frames
=========================================================================*/
extern void drx_entity_adjust_ctch_schedule(void);

#endif  /* #ifndef WL1DRXENTITY_H */
