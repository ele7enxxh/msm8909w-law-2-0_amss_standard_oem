#ifndef TDSL1DRXMANAGER_H
#define TDSL1DRXMANAGER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               WL1 DRX Manager

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdrxmgr.h#1 $
  $DateTime: 2016/12/13 07:58:19 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/11   weis    Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsl1def.h"
#include "msg.h"
#include "tdssrchzz.h"
#include "tfw_sw_intf_msg.h"
#include "mcaltds_srch.h"

/* When Channel Manager registers a channel with DRX Manager, DRX Manager
** returns this handle.  Used for future identification of this logical
** entity or channel. */
typedef int16  tdsdrxmgr_entity_handle_type;

#define TDSDRXMGR_ENTITY_HANDLE_INVALID  (-1)

/* Channel Manager uses this type to identify the logical entity it is
** registering with DRX Manager. */
typedef enum
{
  TDSDRXMGR_ENTITY_INVALID = 0,

  /* These channels can have schedules associated with them.  There can only
  ** be one instance of these at a time, i.e. can't have 2 PICH's at once.
  ** It is OK to have a PICH and a MICH at once, etc. */
  TDSDRXMGR_ENTITY_PICH, /* = 1 */
  TDSDRXMGR_ENTITY_BCCH_INT, /* = 2 */
  TDSDRXMGR_ENTITY_BCCH_EXT, /* = 3 */
  TDSDRXMGR_ENTITY_FACH,     /* = 4 */
  TDSDRXMGR_ENTITY_DCH,      /* = 5 */
  /* This one doesn't really have a schedule.  It sleeps indefinitely until
  ** a page is detected, but there can still only be one at a time. */
  TDSDRXMGR_ENTITY_PCCH,     /* = 6 */

  /* Special type for DRX Manager, not really a channel, but gives DRX Manager
  ** a way to wake the UE for sleep aborts. */
  TDSDRXMGR_ENTITY_DRX_MANAGER, /* = 7 */

  TDSDRXMGR_ENTITY_ONE_INSTANCE_ONLY_MAX_TYPE = TDSDRXMGR_ENTITY_DRX_MANAGER,

  /* Channels with no schedule (100% active) like PCCH, DTCH, etc.  Let's just
  ** say we can only have 3 at once for now. */
  TDSDRXMGR_ENTITY_ALWAYS_ACTIVE,  /* = 8 */
  TDSDRXMGR_ENTITY_ALWAYS_ACTIVE_0 = TDSDRXMGR_ENTITY_ALWAYS_ACTIVE,
  TDSDRXMGR_ENTITY_ALWAYS_ACTIVE_1,
  TDSDRXMGR_ENTITY_ALWAYS_ACTIVE_2,

  TDSDRXMGR_ENTITY_NUM_TYPES
} tdsdrxmgr_entity_type;

/* These are bitmasks corresponding to tdsdrxmgr_entity_type just above. */
#define TDSDRXMGR_ENTITY_MASK_INVALID          (0x00000001UL << (int16)TDSDRXMGR_ENTITY_INVALID)
#define TDSDRXMGR_ENTITY_MASK_PICH             (0x00000001UL << (int16)TDSDRXMGR_ENTITY_PICH)
#define TDSDRXMGR_ENTITY_MASK_BCCH_INT         (0x00000001UL << (int16)TDSDRXMGR_ENTITY_BCCH_INT)
#define TDSDRXMGR_ENTITY_MASK_BCCH_EXT         (0x00000001UL << (int16)TDSDRXMGR_ENTITY_BCCH_EXT)
#define TDSDRXMGR_ENTITY_MASK_PCCH             (0x00000001UL << (int16)TDSDRXMGR_ENTITY_PCCH)
#define TDSDRXMGR_ENTITY_MASK_DRX_MANAGER      (0x00000001UL << (int16)TDSDRXMGR_ENTITY_DRX_MANAGER)
#define TDSDRXMGR_ENTITY_MASK_ALWAYS_ACTIVE    (0x00000001UL << (int16)TDSDRXMGR_ENTITY_ALWAYS_ACTIVE)
#define TDSDRXMGR_ENTITY_MASK_ALWAYS_ACTIVE_0  (0x00000001UL << (int16)TDSDRXMGR_ENTITY_ALWAYS_ACTIVE_0)
#define TDSDRXMGR_ENTITY_MASK_ALWAYS_ACTIVE_1  (0x00000001UL << (int16)TDSDRXMGR_ENTITY_ALWAYS_ACTIVE_1)
#define TDSDRXMGR_ENTITY_MASK_ALWAYS_ACTIVE_2  (0x00000001UL << (int16)TDSDRXMGR_ENTITY_ALWAYS_ACTIVE_2)


#define TDSDRXMGR_ENTITY_ALWAYS_ACTIVE_MASKS  (TDSDRXMGR_ENTITY_MASK_ALWAYS_ACTIVE_0 | \
                                         TDSDRXMGR_ENTITY_MASK_ALWAYS_ACTIVE_1 | \
                                         TDSDRXMGR_ENTITY_MASK_ALWAYS_ACTIVE_2)

/* How to convert an entity type to an entity mask. */
#define TDSDRXMGR_ENTITY_TYPE_TO_MASK(type)  (0x00000001UL << (int16)type)

/* When determining sleep duration, start off assuming a big, bogus value.
** Biggest positive number that fits in a uint32. */
#define TDSDRXMGR_LARGE_INVALID_SLEEP_TIME  0x7FFFFFFF

/* Number of microseconds in a frame. */
#define TDSDRXMGR_USEC_PER_FRAME  10000

/****************************************************************************
** When DRX Manager asks Channel Manager for activity information about a
** logical entity, these are the possible answers:
** - TDSDRXMGR_AWAKE_INDEFINITELY
**   Channel has no foreseeable period of inactivity, like DTCH for example.
** - TDSDRXMGR_ASLEEP_INDEFINITELY
**   Channel has no foreseeable period of activity.  Its schedule indicates
**   an indefinite period of sleep.
** - TDSDRXMGR_AWAKE_NOW
**   Channel should be active right now, for a finite duration.
** - TDSDRXMGR_ASLEEP_NOW
**   Channel can sleep right now, for a finite duration.
****************************************************************************/
typedef enum
{
  TDSDRXMGR_AWAKE_INDEFINITELY,
  TDSDRXMGR_ASLEEP_INDEFINITELY,
  TDSDRXMGR_AWAKE_NOW,
  TDSDRXMGR_ASLEEP_NOW,
  TDSDRXMGR_SLEEP_NUM_TYPES
} tdsdrxmgr_sleep_type;

/****************************************************************************
** When DRX Manager asks Channel Manager for activity information about a
** logical entity, Channel Manager returns this structure:
** - sleep_type
**   Channel activity state at the current time, i.e. right now.
** - target_time
**   Future time at which the channel activity state changes.  If state is
**   INDEFINITELY, "target_time" is not applicable.
****************************************************************************/
typedef struct
{
  tdsdrxmgr_sleep_type          sleep_type;
  tdsl1_time_info_struct_type   target_time;
} tdsdrxmgr_sleep_struct_type;

/****************************************************************************
** DRX Manager invokes these callbacks in Channel Manager when it needs to
** perform one of the following:
** - Wake up a logical entity (or channel).
** - Put a logical entity (or channel) to sleep.
** - Get scheduling information on a logical entity (or channel).
****************************************************************************/

/*===========================================================================
FUNCTION     DRX_ENTITY_WAKEUP_CB_TYPE

DESCRIPTION
  This callback is invoked by DRX Manager to wake up a channel (or logical
  entity) because its schedule indicates there is data to be received.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel to wake up.

RETURN VALUE
  None.
===========================================================================*/
typedef void (*tdsdrxmgr_entity_wakeup_cb_type)(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_TO_SLEEP_CB_TYPE

DESCRIPTION
  This callback is invoked by DRX Manager to put a channel (or logical entity)
  to sleep because its schedule indicates there is no data to be received
  for awhile.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel to put to sleep.

RETURN VALUE
  None.
===========================================================================*/
typedef void (*tdsdrxmgr_entity_to_sleep_cb_type)(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     NEXT_FRAME_WANTED_CB_TYPE

DESCRIPTION
  This callback is invoked by DRX Manager to find out when a channel
  (or logical entity) wants to be active in the future, according to its
  schedule.  It also indicates the duration of that activity.

  It is possible for a channel to be always active or always inactive for
  an indefinite period of time.

DEPENDENCIES
  None.

PARAMETERS
  logical_entity_handle - Uniquely identifies channel of interest.
  sleep_info_ptr        - Return pointer to structure providing channel
                          scheduling information to the caller.
RETURN VALUE
  TRUE  - Call successful.
  FALSE - Call failed.  For example, invalid logical_entity_handle.
===========================================================================*/
typedef boolean (*tdsdrxmgr_next_frame_wanted_cb_type)
(
  tdsdrxmgr_entity_handle_type          entity_handle,
  tdsdrxmgr_sleep_struct_type  *sleep_info_ptr
);

typedef struct
{
  tdsdrxmgr_entity_wakeup_cb_type      entity_wakeup_cb;
  tdsdrxmgr_entity_to_sleep_cb_type    entity_to_sleep_cb;
  tdsdrxmgr_next_frame_wanted_cb_type  next_frame_wanted_cb;
} tdsdrxmgr_channel_cb_struct_type;

/* The different commands processed by DRX Manager. */
typedef enum
{
  TDSDRXMGR_SRCH_TO_SLEEP_RSP = 0,
  TDSDRXMGR_PSC_SCAN_DONE_RSP,
  TDSDRXMGR_NUM_CMDS
} tdsdrxmgr_cmd_id_type;

typedef struct
{
  /* Need this to put command on tdsl1m_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type    local_cmd_hdr;

  tdsdrxmgr_cmd_id_type  cmd_id;
} tdsdrxmgr_srch_to_sleep_cmd_type;

typedef union
{
  struct
  {
    /* Need this to put command on tdsl1m_local_cmd_q. */
    tdsl1def_l1_local_cmd_hdr_type             local_cmd_hdr;

    tdsdrxmgr_cmd_id_type           cmd_id;
  } header;

  tdsdrxmgr_srch_to_sleep_cmd_type  srch_to_sleep_cmd;
} tdsdrxmgr_cmd_type;

/* Timer to detect timeouts on responses from SRCH and Channel Manager. */
extern timer_type tdsdrxmgr_timeout_timer;

/* DRX entity handle returned by tdsdrxmgr_register_entity().  This particular entity
** does not correspond to a channel, like most do.  Instead it is used to wake
** the UE from sleep on RRC command reception, i.e. sleep abort. */
extern tdsdrxmgr_entity_handle_type  tdsdrxmgr_entity_handle;

extern boolean tdsdrxmgr_sleep_pending_for_rx_time_adj;

/*===========================================================================
FUNCTION     DRX_MANAGER_WAKE_FROM_SLEEP

DESCRIPTION
  This function is called once RF has woken up following a sleep abort, i.e.
  an RRC command received while the phone is asleep.

  RRC is probably setting up a physical channel now.  So the goal now is to
  wake SRCH to start tracking all cells that we put to sleep earlier.  For
  now, we only support the serving cell here.

  The way we do this is create a logical entity with DRX Manager.  It doesn't
  correspond to a channel like it usually does.  This time it's just a means
  to wake up DRX Manager and start tracking cells.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_wake_from_sleep(void);

/*===========================================================================
FUNCTION     DRX_MANAGER_RELEASE_DRX_MODE

DESCRIPTION
  This function is called by drx_rel().  DRX mode operation has been stopped.
  If a sleep abort happened not long ago, then the TDSDRXMGR_ENTITY_DRX_MANAGER
  logical entity is active with DRX Manager.  This entity's sole
  responsibility is to prevent sleep for awhile after a sleep abort.

  Not that DRX mode has stopped, we don't need TDSDRXMGR_ENTITY_DRX_MANAGER
  registered anymore.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_release_drx_mode(void);

/*===========================================================================
FUNCTION     DRX_MANAGER_PUT_SRCH_TO_SLEEP

DESCRIPTION
  This function is called when all channels are asleep or dropped, so DRX
  Manager can start putting the UE to sleep.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_put_srch_to_sleep(void);

/*===========================================================================
FUNCTION     DRX_MANAGER_SERIALIZE_SRCH_WAKEUP_CELLS

DESCRIPTION
  This function serializes calls to srch_wakeup_cells().  In other words,
  if two channels want to wake SRCH at about the same time, make sure we
  get a SRCH response to the first one before calling srch_wakeup_cells()
  again on the second one.

DEPENDENCIES
  None.

PARAMETERS
  handle         - Handle identifying this logical entity.
  wakeup_set_ptr - Pointer to cells to wake up.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_serialize_srch_wakeup_cells
(
  tdsdrxmgr_entity_handle_type    handle,
  tdssrch_wakeup_set_struct_type *wakeup_set_ptr
);

/*===========================================================================
FUNCTION     tdsdrxmgr_will_go_to_sleep

DESCRIPTION
  this function is called by SRCH/Measurement module when lte measurement is done, 
  and TFW is in IRAT state

  if drx mgr will put UE to sleep, srch don't need change TFW state back to TRACKING;
  else, srch has to change TFW state to TRACKING.

  BPLMN is not taken into accout in this function, TFW state transition will be 
  handled after receiving RRC BPLMN srch request.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  --- drx mgr will put UE to sleep
  FALSE --- drx mgr will NOT put UE to sleep.
===========================================================================*/
extern boolean tdsdrxmgr_will_go_to_sleep(void);

/*===========================================================================
FUNCTION     DRX_MANAGER_START_SLEEP_CYCLE

DESCRIPTION
  When this function is called, all channels are sleeping, and now SRCH is
  finally sleeping too.  This function checks on a few things, like for BPLMN
  scan, PSC scan, and a few others.  If there is nothing else to do, then we
  start powering down the hardware by calling tdsl1sleep_go_to_sleep().

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_start_sleep_cycle(void);


/*===========================================================================
FUNCTION     DRX_MANAGER_PROCESS_CMD

DESCRIPTION
  This function handles commands directed to DRX Manager.  There is a
  different routine that handles commands directed to individual channel
  entity state machines.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - Pointer to DRX Manager command structure.

RETURN VALUE
  TRUE means success.  FALSE otherwise.
===========================================================================*/
extern boolean tdsdrxmgr_process_cmd(const tdsdrxmgr_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION     DRX_REGISTER_ENTITY

DESCRIPTION
  This function is called by Channel Manager when RRC sets up a channel with
  the TDSL1_CPHY_SETUP_REQ command.  Channel Manager passes in all information
  needed by DRX Manager to manage this channel according to its activity
  schedule.

  Shortly after this function is called, DRX Manager will invoke the
  next_frame_wanted() callback function to retrieve schedule information on
  this channel.

  Normally channel_awake will be set to TRUE because a channel is coming up
  immediately.  This is not so for PICH, which generally starts out sleeping.

  Channel Manager may set channel_awake to TRUE before the physical channel
  is actually brought up.  If so, Channel Manager must reflect a channel
  schedule of constant activity, so DRX Manager does not attempt to put the
  channel to sleep.

DEPENDENCIES
  None.

PARAMETERS
  logical_entity_type - Channel type (PICH, MTCH, etc.) identifying this channel.
  dl_uarfcn           - DL channel number of cell carrying this channel
                        (maps to DL frequency of this cell).
  psc                 - Primary scrambling code of cell carrying this channel.
  channel_awake     --- TRUE means this channel is already active or in the
                     \  process of becoming active.
                      \ FALSE means this channel is currently sleeping because
                        its schedule is in an idle period.
  cb_functions        - Pointer to structure of callback functions.

RETURN VALUE
  tdsdrxmgr_entity_handle_type - Unique logical entity (or channel) identifier.
===========================================================================*/
extern tdsdrxmgr_entity_handle_type tdsdrxmgr_register_entity
(
  tdsdrxmgr_entity_type    entity_type,
  uint16                   uarfcn,
  uint16                   cpid,
  boolean                  channel_awake,
  tdsdrxmgr_channel_cb_struct_type  *cb_functions
);

/*===========================================================================
FUNCTION     DRX_DEREGISTER_ENTITY

DESCRIPTION
  Channel Manager calls this DRX Manager function when DRX Manager no longer
  needs to manage a channel and its schedule.  The typical scenario for this
  is when RRC drops the channel.

  There may be other scenarios as well, like if a channel's schedule has an
  indefinite period of inactivity.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel shutting down.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_deregister_entity(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_SUSPEND_ENTITY

DESCRIPTION
  Channel Manager calls this DRX Manager function to suspend an entity's
  state machine.  Channel Manager is up to something like a reconfig, etc.,
  and wants to make sure DRX Manager doesn't disturb it while this is
  happening.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies the entity being suspended.

RETURN VALUE
  boolean - TRUE on success.  FALSE on failure.
===========================================================================*/
extern boolean tdsdrxmgr_suspend_entity(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_RESUME_ENTITY

DESCRIPTION
  Channel Manager calls this DRX Manager function to resume an entity's
  state machine.  This function should only be called sometime after
  tdsdrxmgr_suspend_entity() is called.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies the entity being resumed.

RETURN VALUE
  boolean - TRUE on success.  FALSE on failure.
===========================================================================*/
extern boolean tdsdrxmgr_resume_entity(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_SCHEDULE_UPDATE

DESCRIPTION
  Channel Manager calls this DRX Manager function when the schedule of a
  logical entity (or channel) changes.  Upper software layers, like RRC, BMC,
  etc., may change the schedule of a channel over time.

  This function simply alerts DRX Manager of a schedule change, passing in
  no schedule information to DRX Manager.  DRX Manager must then invoke the
  next_frame_wanted() callback to actually retrieve the most up-to-date
  schedule information.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel with a new schedule.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_entity_schedule_update(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_MANAGER_NEXT_WAKEUP_TIME

DESCRIPTION
  This function is called by SRCH to ask DRX Manager when one or more channels
  are going to wake up next.  SRCH passes in a bitmask of channels for DRX
  Manager to check.  This function will return the wakeup time of the channel
  that wants to wake up the soonest.

  Some bitmasks are TDSDRXMGR_ENTITY_MASK_PICH, TDSDRXMGR_ENTITY_MASK_MTCH, etc.

  Also the macro TDSDRXMGR_ENTITY_TYPE_TO_MASK(type) can be used to convert the
  enumerated type "tdsdrxmgr_entity_type" into a bitmask.

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
extern int32 tdsdrxmgr_next_wakeup_time
(
  tdsdrxmgr_entity_handle_type *next_wakeup_handle,
  tdsdrxmgr_sleep_struct_type  *next_wakeup_time
);

/*===========================================================================
FUNCTION     DRX_MANAGER_GET_CURRENT_ENTITY_MASK

DESCRIPTION
  SRCH (or whoever) can call this function to find out what entity types are
  currently awake.  This function returns a bitmask of the entity "types" that
  are awake, not entity "handles".

  For single instance channels (PICH, MCCH, etc.), the type and the handle are
  equal.  For multiple instance channels (MTCH, etc.), the type and the
  handle(s) may differ.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  uint32 - Bitmask of entity types currently awake.
===========================================================================*/
extern uint32 tdsdrxmgr_get_current_entity_mask(void);
/*===========================================================================
FUNCTION     DRX_ENTITY_WAKEUP

DESCRIPTION
  calls channel manager wake-up callback function to wake up this entity.  
  This happens when the entity awake time is arrival.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel to be woken up.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_entity_wakeup(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_WAKEUP_DONE

DESCRIPTION
  Channel Manager calls this DRX Manager function upon channel wakeup
  completion.  This happens in response to DRX Manager waking up a channel
  by invoking the channel wakeup callback.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel being woken up.
  status - TRUE means channel wakeup successful.
         \ FALSE means hannel wakeup failed.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_entity_wakeup_done
(
  tdsdrxmgr_entity_handle_type handle,
  boolean                status
);
/*===========================================================================
FUNCTION     DRX_ENTITY_SLEEP

DESCRIPTION
  calls channel manager sleep callback function to put this entity to sleep.
  This happens when the entity asleep time is arrival.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel to be asleep.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_entity_sleep(tdsdrxmgr_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_ASLEEP_DONE

DESCRIPTION
  Channel Manager calls this DRX Manager function once a channel has been put
  to sleep.  This happens in response to DRX Manager putting a channel to sleep
  by invoking the channel to sleep callback.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies channel being put to sleep.
  status - TRUE means channel wakeup successful.
         \ FALSE means hannel wakeup failed.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_entity_asleep_done
(
  tdsdrxmgr_entity_handle_type handle,
  boolean                status
);

/*===========================================================================
FUNCTION     DRX_SRCH_WAKEUP_DONE

DESCRIPTION
  SRCH invokes this function in response to DRX Manager calling
  srch_wakeup_cells().  DRX Manager calls this in the process of waking up
  a logical entity.  It starts by telling SRCH to start tracking the cell(s)
  of interest.  SRCH calls this function after the initial search completes.

DEPENDENCIES
  None.

PARAMETERS
  pscs_woken_up_ptr - Pointer to list of cell structs that just woke up.
  num_pscs          - Number of PSC's in the above list.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_srch_wakeup_done
(
  const tdssrch_wakeup_set_struct_type  *pscs_woken_up_ptr
);

/*===========================================================================
FUNCTION     DRX_SRCH_ASLEEP_DONE

DESCRIPTION
  SRCH invokes this function in response to DRX Manager calling
  tdssrchzz_wait_on_srch_sleep ().  DRX Manager calls this function once all logical
  entities are sleeping, and DRX Manager wants to put the phone to sleep.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_srch_asleep_done(void);


/*===========================================================================
FUNCTION     tdsdrxmgr_proc_state_cfg_rsp

DESCRIPTION:
  - handle state_cfg_rsp from FW
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================*/
extern void tdsdrxmgr_proc_state_cfg_rsp(tfw_state_t state);

/*===========================================================================
FUNCTION     DRX_MANAGER_INIT

DESCRIPTION
  DRX Manager initializations performed whenever WCDMA is activated.

  Must be called ONE TIME, at power-up only.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsdrxmgr_init(void);

/*------------------------------------------------------------------------------------------------*/ 
#ifdef FEATURE_TDSCDMA_MSS_PWR_TEST

typedef enum
{
    TDSDRXMGR_PWR_TEST_NV_OFF = 0, /*NV control OFF*/
	TDSDRXMGR_PWR_TEST_NV_SKIP_DRX,
	TDSDRXMGR_PWR_TEST_NV_NUM_TYPES

} tdsdrxmgr_pwr_test_nv_type;

typedef enum
{
    TDSDRXMGR_PWR_TEST_FSM_NV_OFF = 0, /*UE power on with NV_OFF*/
    TDSDRXMGR_PWR_TEST_FSM_OFF,
    TDSDRXMGR_PWR_TEST_FSM_ON,
    TDSDRXMGR_PWR_TEST_FSM_NUM_TYPES

} tdsdrxmgr_pwr_test_fsm_type;

#define TDSDRXMGR_PWR_TEST_DRX_WIN_BIT_ARRAY_NUM 1     /*unit with 16bits*/
#define TDSDRXMGR_PWR_TEST_PAGE_DRX_WINDOW_SIZE (TDSDRXMGR_PWR_TEST_DRX_WIN_BIT_ARRAY_NUM*16)
#define TDSDRXMGR_PWR_TEST_PAGE_SUCCESS_RATE 5 /*1/5 = 20%*/ 


/* define events to triger FSM ON/OFF 
 * could be NV, could be environment parameters*/
typedef enum
{
    TDSDRXMGR_PWR_TEST_EVTS_INIT = 0, //-> Init to check 
   	/*events need to reset the mux_bit*/
    TDSDRXMGR_PWR_TEST_EVTS_DRX_SETUP,     /*Drx length is of 640ms/1.28 do the DRX events*/
   	TDSDRXMGR_PWR_TEST_EVTS_DRX_REL,
	TDSDRXMGR_PWR_TEST_EVTS_SLEEP_ABORT,  /*Need to add in Rude wkup scenario*/
    /*following events only on/off the bit, and off the FSM
	  not reset the mux bitmap*/
	TDSDRXMGR_PWR_TEST_EVTS_IRAT_OFF,
	TDSDRXMGR_PWR_TEST_EVTS_IRAT_ON,
	TDSDRXMGR_PWR_TEST_EVTS_PG_RATE_MEET,
	TDSDRXMGR_PWR_TEST_EVTS_PG_RATE_NOT_MEET,
#ifdef FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
    TDSDRXMGR_PWR_TEST_EVTS_PCCPCH_SNR_MEET,  /*SNR meet start paging detect*/
    TDSDRXMGR_PWR_TEST_EVTS_PCCPCH_NOT_MEET,  /*Not reset paging pattern if detected. stop detecting, if not ready*/
    TDSDRXMGR_PWR_TEST_EVTS_PCH_PATTEN_READY, /*PCH pattern detected*/
    TDSDRXMGR_PWR_TEST_EVTS_PCH_PATTEN_NO_SKIP, /*PCH pattern canot skip, not send ready to PWR_TST FSM*/
#endif
    TDSDRXMGR_PWR_TEST_EVTS_FSM_NUM_TYPES

} tdsdrxmgr_pwr_test_fsm_events_type;


/*----------------------------------------------------------------------------*/ 

#ifdef FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
#define TDSDRXMGR_PCH_DETECT_PATT_WIN_SIZE 4
/*try 8 times, if all same as pging pattern = 1, no need detecting*/
#define TDSDRXMGR_PCH_DETECT_PATT_MAX_DET_CYCLE 8

/*Seperate the PCH pattern detection state machine*/
typedef enum
{
    TDSDRXMGR_PCH_DETECT_PATT_OFF = 0, /*Not detected PCH pattern yet*/
	TDSDRXMGR_PCH_DETECT_PATT_DETECTING, /*all other condition meet and find the PI = 0 to 1 transition*/
    TDSDRXMGR_PCH_DETECT_PATT_DETECTED,
    TDSDRXMGR_PCH_DETECT_PATT_NUM_TYPES

} tdsdrxmgr_pch_det_patt_state_type;


/*PCH pattern detection events*/
typedef enum
{
    TDSDRXMGR_PCH_DETECT_PATT_EVTS_PI_TRIG,       /*PI = 0 to PI = 1 transition trigger*/
    TDSDRXMGR_PCH_DETECT_PATT_EVTS_PCH_CRC_FAIL, 
    TDSDRXMGR_PCH_DETECT_PATT_EVTS_RRC_FAIL,       /*RRC return false, need restart detect*/
    TDSDRXMGR_PCH_DETECT_PATT_EVTS_RRC_SUCC,       /*RRC return true, keep continue*/

	/*for Pwr test OFF, reset the paging detect procedure*/
    TDSDRXMGR_PCH_DETECT_PATT_EVTS_PWR_TEST_OFF,  /*Power test mode off by other conditions*/
    TDSDRXMGR_PCH_DETECT_PATT_EVTS_NUM_TYPES
} tdsdrxmgr_pch_det_patt_events_type;

void tdsdrxmgr_pch_det_patt_updt_skip_cnt(void);

#endif //FEATURE_TDSCDMA_PAGE_PATTERN_DETECT

/*----------------------------------------------------------------------------*/ 


/*power test parameters
 * which affect the skip decision*/
typedef struct
{
    boolean last_pch_crc_pass;
} tdsdrxmgr_pwr_tst_sched_param_type;

/*scheduling decision*/
typedef struct
{
    boolean skip_coming_pich;
    boolean skip_pch;

} tdsdrxmgr_pwr_tst_sched_deci_type;

typedef struct
{
    uint16 nv_alfa;   /*NV read 0~10, mulitple 10 when init*/
	int16 nv_thresh_hold;  /*NV read threshold*/
    int16 pre_sample;

} tdsdrxmgr_pch_det_patt_snr_filter_type;


typedef struct
{
	tdsdrxmgr_pwr_test_fsm_type fsm_state;
#ifdef FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
    tdsdrxmgr_pch_det_patt_state_type pch_det_state;
    uint32 kick_pg_det_bit;   /*events bit need to check before start pging detectin*/
    uint8 pg_det_nv_enable;
	uint8 pch_det_win_cnt;
	uint8 last_pi_value;     /*record the pi value, detect pi = 0 to 1*/
    uint8 pg_patt_value;
	uint8 pg_protect_value; /*protect value for reduce skip*/
	uint8 max_pg_det_1_cycle;
    uint8 pg_patt_skip_cnt;
	/*in Micro wakeup reset cnt to (D-1) as made decision not skip this drx,
	  but in PICH dec isr will call scheduler again, if PI = 1, will skip PCH which is not expected
    add this flag to handle unexpected PCH skip*/
	boolean not_skip;        
	tdsdrxmgr_pch_det_patt_snr_filter_type filter_info;
#endif
    uint8 pg_rate;
	tdsdrxmgr_pwr_tst_sched_param_type sched_param;	 /*schedule paramters*/
	tdsdrxmgr_pwr_tst_sched_deci_type sched_deci_info; /*schedule decision*/
	/*define the events bitmap for checking incase 
	 * need severl events together to turn on the FSM*/
	uint32 fsm_evts_mux_bit;
	uint32 fsm_evts_ck_bit;
	uint32 drx_cycle_cnt;
	uint16 pich_true_array[TDSDRXMGR_PWR_TEST_DRX_WIN_BIT_ARRAY_NUM];

} tdsdrxmgr_pwr_tst_info_type;


#ifdef FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
/*===========================================================================
FUNCTION     tdsdrxmgr_pch_det_patt_state_updt

DESCRIPTION  paging pattern detection events update


DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/

void tdsdrxmgr_pch_det_patt_state_updt(tdsdrxmgr_pch_det_patt_events_type trig_evt);

#endif //FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
/*===========================================================================
FUNCTION     

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
void tdsdrxmgr_pwr_test_fsm_init(void);
/*===========================================================================
FUNCTION     

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/

void tdsdrxmgr_pwr_test_fsm_state_update(tdsdrxmgr_pwr_test_fsm_events_type trig_evt);
/*===========================================================================
FUNCTION     

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
boolean tdsdrxmgr_pwr_test_scheduler( tdsdrxmgr_pwr_tst_sched_param_type *sched_in_ptr, tdsdrxmgr_pwr_tst_sched_deci_type *sched_out_ptr);


/*===========================================================================
FUNCTION     

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
void tdsdrxmgr_pwr_test_paging_rate_updt_fsm(uint8 pi);

/*===========================================================================
  FUNCTION     

  DESCRIPTION

  DEPENDENCIES
  None.

  PARAMETERS
  None.

  RETURN VALUE
  None.
  ===========================================================================*/
void tdsdrxmgr_pch_det_patt_snr_filter(int16 curr_sample);


#endif //FEATURE_TDSCDMA_MSS_PWR_TEST

#endif  /* #ifndef TDSL1DRXMANAGER_H */
