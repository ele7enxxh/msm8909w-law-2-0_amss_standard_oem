#ifndef TDSL1DRXENTITY_H
#define TDSL1DRXENTITY_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               WL1 DRX Entity

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2007 by Qualcomm Technologies, Incorporated. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdrxentity.h#1 $
  $DateTime: 2016/12/13 07:58:19 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/11   weis    Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "tdsdrxmgr.h"


/* The different commands processed by the channel state machines. */
typedef enum
{
  TDSDRXENTITY_REGISTER_CMD = 0,
  TDSDRXENTITY_DEREGISTER_CMD,
  TDSDRXENTITY_SRCH_WAKEUP_RSP,
  TDSDRXENTITY_DEMOD_WAKEUP_RSP,
  TDSDRXENTITY_DEMOD_TO_SLEEP_RSP,
  TDSDRXENTITY_TIMER_EXPIRED_CMD,
  TDSDRXENTITY_SCHEDULE_UPDATE_CMD,
  TDSDRXENTITY_SUSPEND_CMD,
  TDSDRXENTITY_RESUME_CMD,
  TDSDRXENTITY_NUM_CMDS
} tdsdrxentity_cmd_id_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   local_cmd_hdr;

  tdsdrxmgr_entity_handle_type  handle;
  tdsdrxentity_cmd_id_type  cmd_id;

  boolean                 channel_awake;
} tdsdrxentity_register_cmd_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   local_cmd_hdr;

  tdsdrxmgr_entity_handle_type  handle;
  tdsdrxentity_cmd_id_type  cmd_id;
} tdsdrxentity_deregister_cmd_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type  local_cmd_hdr;

  tdsdrxmgr_entity_handle_type    handle;
  tdsdrxentity_cmd_id_type        cmd_id;

  tdssrch_wakeup_set_struct_type  pscs_woken_up;
} tdsdrxentity_srch_awake_cmd_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   local_cmd_hdr;

  tdsdrxmgr_entity_handle_type  handle;
  tdsdrxentity_cmd_id_type  cmd_id;

  boolean                 status;
} tdsdrxentity_srch_to_sleep_cmd_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   local_cmd_hdr;

  tdsdrxmgr_entity_handle_type  handle;
  tdsdrxentity_cmd_id_type  cmd_id;

  boolean                 status;
} tdsdrxentity_demod_cmd_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   local_cmd_hdr;

  tdsdrxmgr_entity_handle_type  handle;
  tdsdrxentity_cmd_id_type  cmd_id;
} tdsdrxentity_timer_expired_cmd_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   local_cmd_hdr;

  tdsdrxmgr_entity_handle_type  handle;
  tdsdrxentity_cmd_id_type  cmd_id;
} tdsdrxentity_schedule_update_cmd_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   local_cmd_hdr;

  tdsdrxmgr_entity_handle_type  handle;
  tdsdrxentity_cmd_id_type  cmd_id;
} tdsdrxentity_suspend_cmd_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   local_cmd_hdr;

  tdsdrxmgr_entity_handle_type  handle;
  tdsdrxentity_cmd_id_type  cmd_id;
} tdsdrxentity_resume_cmd_type;

typedef union
{
  struct
  {
    /* Need this to put command on tdsl1m_local_cmd_q. */
    tdsl1def_l1_local_cmd_hdr_type            local_cmd_hdr;

    tdsdrxmgr_entity_handle_type           handle;
    tdsdrxentity_cmd_id_type           cmd_id;
  } header;

  tdsdrxentity_register_cmd_type              register_cmd;
  tdsdrxentity_deregister_cmd_type            deregister_cmd;
  tdsdrxentity_srch_awake_cmd_type            srch_awake_cmd;
  tdsdrxentity_srch_to_sleep_cmd_type  srch_to_sleep_cmd;
  tdsdrxentity_demod_cmd_type                 demod_cmd;
  tdsdrxentity_timer_expired_cmd_type         timer_expired_cmd;
  tdsdrxentity_schedule_update_cmd_type       schedule_update_cmd;
  tdsdrxentity_suspend_cmd_type               suspend_cmd;
  tdsdrxentity_resume_cmd_type                resume_cmd;
} tdsdrxentity_cmd_type;

/* Whenever an entity handle is in a state other than INACTIVE or ASLEEP, the
** corresponding bit in this mask shall be set to one.  In other words, if
** awake, set the bit, otherwise clear the bit.  So when this mask is 0x00,
** all channels are asleep, and we may put the phone to sleep. */
#define TDSDRXENTITY_ALL_ENTITIES_ASLEEP  0x00
/* This is the maximum number of logical entities that may be activated by RRC
** at one time.  They all may not be physically active simultaneously.  Some
** may be sleeping.  But all active from RRC's perspective. */
#define TDSDRXENTITY_NUM_ACTIVE_LOGICAL_ENTITIES  ((int16)TDSDRXMGR_ENTITY_NUM_TYPES)

extern uint32  tdsdrxentity_handles_awake_mask;

extern boolean tdsdrxentity_pich_wakeup_late_flag;

/*===========================================================================
FUNCTION     DRX_ENTITY_PROCESS_CHNL_SETUP_EVENT

DESCRIPTION
   This function is called when the CHNL_SETUP_EVENT is firing, it checks 
   every waiting_on_dl_awake entity's setup time.
   if current sub-fn is just right setup time for an entiy, 
      call dl mgr callback to wake up this entity.
   otherwise
      do nothing for this entity, 
      continue to check the next waiting_on_dl_awake entity;

DEPENDENCIES
  None.

PARAMETERS
  void
RETURN VALUE
  void
===========================================================================*/
extern void tdsdrxentity_process_chnl_setup_event(void);

/*===========================================================================
FUNCTION     TDSDRXENTITY_PROCESS_CMD

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
extern boolean tdsdrxentity_process_cmd(const tdsdrxentity_cmd_type  *cmd_ptr);

/*===========================================================================
FUNCTION     TDSDRXENTITY_ALLOCATE_HANDLE

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
           TDSDRXMGR_ENTITY_HANDLE_INVALID on failure.
===========================================================================*/
extern tdsdrxmgr_entity_handle_type tdsdrxentity_allocate_handle
(
  tdsdrxmgr_entity_type    entity_type,
  uint16                   uarfcn,
  uint16                   cpid,
  tdsdrxmgr_channel_cb_struct_type  *cb_functions
);

/*===========================================================================
FUNCTION     TDSDRXENTITY_WAKE_ANY_DELAYED_ENTITIES

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
  None.
===========================================================================*/
extern boolean tdsdrxentity_wake_any_delayed_entities(void);


/*===========================================================================
FUNCTION     TDSDRXENTITY_SET_TARGET_ENTITY

DESCRIPTION
  When DRX Manager puts the UE to sleep, it calls this function to set the
  entity handle that we will wake up for in the future.  This would be the
  first entity to wake up of all sleeping entities.  Of course, usually
  it's just PICH.

DEPENDENCIES
  None.

PARAMETERS
  tdsdrxmgr_entity_handle_type handle_waking_up - Entity waking next.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxentity_set_target_entity(tdsdrxmgr_entity_handle_type handle_waking_up);

/*===========================================================================
FUNCTION     TDSDRXENTITY_SET_TARGET_ENTITY

DESCRIPTION
  get target drx entity

DEPENDENCIES
  None.

PARAMETERS
  void.

RETURN VALUE
  None.
===========================================================================*/
extern tdsdrxmgr_entity_handle_type tdsdrxentity_get_target_entity(void);

/*===========================================================================
FUNCTION     TDSDRXENTITY_WAKE_TARGET_ENTITY

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
extern void tdsdrxentity_wake_target_entity(void);

/*===========================================================================
FUNCTION     TDSDRXENTITY_FIND_TYPE_HANDLE

DESCRIPTION
  This function returns the entity handle based on its type.  This function
  should only be used on entities that can only have a single instance, like
  PICH, MCCH, etc.  It should not be used on MTCH for example.

DEPENDENCIES
  None.

PARAMETERS
  entity_type - i.e. TDSDRXMGR_ENTITY_PICH, TDSDRXMGR_ENTITY_CTCH, etc.

RETURN VALUE
  tdsdrxmgr_entity_handle_type - Handle identifying this logical entity.
===========================================================================*/
extern tdsdrxmgr_entity_handle_type tdsdrxentity_find_type_handle
(
  tdsdrxmgr_entity_type entity_type
);

/*===========================================================================
FUNCTION     TDSDRXENTITY_WHICH_TYPE

DESCRIPTION
  This function returns the entity type based on its handle.

DEPENDENCIES
  None.

PARAMETERS
  handle - Handle identifying this logical entity.

RETURN VALUE
  tdsdrxmgr_entity_type - i.e. TDSDRXMGR_ENTITY_PICH, TDSDRXMGR_ENTITY_CTCH, etc.
===========================================================================*/
extern tdsdrxmgr_entity_type tdsdrxentity_which_type(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     TDSDRXENTITY_GET_SLEEP_INFO

DESCRIPTION
  This function returns a pointer to the sleep information structure
  corresponding to the passed-in logical entity handle.

  Note that it doesn't check if this handle has actually been allocated.
  That is, only use this function on handles returned by
  tdsdrxentity_allocate_handle();

DEPENDENCIES
  None.

PARAMETERS
  handle - Handle identifying this logical entity.

RETURN VALUE
  tdsdrxmgr_sleep_struct_type* - Pointer to sleep info.
===========================================================================*/
extern tdsdrxmgr_sleep_struct_type *tdsdrxentity_get_sleep_info
(
  tdsdrxmgr_entity_handle_type handle
);

/*===========================================================================
FUNCTION     TDSDRXENTITY_GET_SLEEP_INFO

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
extern boolean tdsdrxentity_get_cell_info
(
  tdsdrxmgr_entity_handle_type handle,
  tdstlm_cell_info_struct_type *cell_info_ptr
);

/*===========================================================================
FUNCTION     DRX_ENTITY_GET_CB_FUNCTION

DESCRIPTION
  This function returns the call back function of the passed-in 
  logical entity handle.

DEPENDENCIES
  None.

PARAMETERS
  handle - Handle identifying this logical entity.

RETURN VALUE
  uint16 - state.
===========================================================================*/
extern tdsdrxmgr_channel_cb_struct_type* tdsdrxentity_get_cb_function
(
  tdsdrxmgr_entity_handle_type     handle
);


/*===========================================================================
FUNCTION     tdsdrxentity_get_entity_status

DESCRIPTION
  This function output the status of inputted entity, 
  if the entity is alseep , this function also output the number of sub-frames 
  from now to awake time.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if this entity is configured.
  FALSE otherwise, it is not configured.
===========================================================================*/
extern boolean tdsdrxentity_get_entity_status
(
  tdsdrxmgr_entity_handle_type handle,
  boolean* awake, 
  uint16* num_sub_fn_to_awake
);

/*===========================================================================
FUNCTION     tdsdrxentity_get_bcch_status

DESCRIPTION
  This function output the status of BCCH_EXT, 
  if it is alseep , this function also output the number of sub-frames 
  from now to awake time.
  if it's awake, output the number of sub-frames from now to asleep time;
  if bcch is not configured, return FALSE

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if this entity is configured.
  FALSE otherwise, it is not configured.
===========================================================================*/
extern boolean tdsdrxentity_get_bcch_status
(
  boolean* asleep, boolean* awake, 
  uint16* num_sub_fn_left
);


/*===========================================================================
FUNCTION     TDSDRXENTITY_IS_PICH_SUSPENDED_OR_INACTIVE

DESCRIPTION
  This function pretty much just returns whether or not PICH is operational
  in WL1.  If TDSDRXMGR_ENTITY_PICH is suspended or not registered with DRX, this
  function returns TRUE.  Otherwise it returns FALSE.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if PICH is suspended or inactive.
  FALSE otherwise, i.e. PICH is awake or asleep.
===========================================================================*/
extern boolean tdsdrxentity_is_pich_suspended_or_inactive(void);

/*===========================================================================
FUNCTION     tdsdrxentity_is_pich_target_entity

DESCRIPTION
  This function check and return whether tdsdrxentity_target_handle is PICH or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if PICH is the target entity at UE wake-up.
  FALSE otherwise, i.e. PICH is not the target entity at UE wake-up.
===========================================================================*/
extern boolean tdsdrxentity_is_pich_target_entity(void);


#ifdef FEATURE_TDSCDMA_DSDS
/*===========================================================================
FUNCTION     tdsdrxentity_is_pcch_target_entity

DESCRIPTION
  This function check and return whether tdsdrxentity_target_handle is PCCH or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if PCCH is the target entity at UE wake-up.
  FALSE otherwise, i.e. PCCH is not the target entity at UE wake-up.
===========================================================================*/
extern boolean tdsdrxentity_is_pcch_target_entity(void);


/*===========================================================================
FUNCTION     tdsdrxentity_is_bcch_registered

DESCRIPTION
  This function check and return whether BCCH_EXT is registered or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
===========================================================================*/
extern boolean tdsdrxentity_is_bcch_registered(void);
#endif
#if defined(FEATURE_TDSCDMA_DSDS) || defined(FEATURE_TDSCDMA_MSS_PWR_TEST)
/*===========================================================================
FUNCTION     tdsdrxentity_schedule_update_for_wakeup_cancel

DESCRIPTION
  This function update the schedule info of input handle due to wake-up cancel.

DEPENDENCIES


PARAMETERS


RETURN VALUE

===========================================================================*/
extern uint32 tdsdrxentity_schedule_update_for_wakeup_cancel
(
  uint32 cx8_to_wakeup,
  tdsl1_time_info_struct_type  old_wake_up_time,
  tdsl1_time_info_struct_type* new_wake_up_time
);
#endif

/*===========================================================================
FUNCTION     TDSDRXENTITY_RESET_ALL_ENTITIES

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
extern void tdsdrxentity_reset_all_entities(void);

/*===========================================================================
FUNCTION     TDSDRXENTITY_INIT

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
extern void tdsdrxentity_init(void);

#endif  /* #ifndef TDSL1DRXENTITY_H */
