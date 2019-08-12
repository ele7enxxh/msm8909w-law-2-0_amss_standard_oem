#ifndef WL1DRXMANAGER_H
#define WL1DRXMANAGER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               WL1 DRX Manager

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1drxmanager.h#1 $
  $DateTime: 2016/12/13 08:00:12 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/11/15   rs      Triggering FW sleep before handling ext cmd in early RF shutdown cases
11/24/14   gsk     SRCHZZ wakeup cleanup changes
06/10/14   skk     Recapture entity timer just before programming SLPC for non PICH entities. 
09/01/14   kr/skk  DSDS:call rf tune to multichain in gts fail case
09/10/14   skk     If sleep time is invalid in inter f resel, make it 4.5ms.
06/13/14   mk      Changing the return type in the extern of function from void to boolean.
06/10/14   sks     Recapture entity timer just before programming SLPC for EDRX. 
05/22/14   gsk     Removing DUAL_SIM featurization
03/24/14   rs      Back out CR 606459
01/30/14   rs      Externed wl1_g2w_ta_sleep_mode_switch for use across differnt files
12/16/13   as      Porting G2W TA from Triton to Dime.
10/29/13   sr/gsk  During cell reselection, delaying CPHY setup cnf for NBCH drop untill
                   serv cell is configured and nbr cell is deconfigured,
12/19/13   rs      Removed function header. 
07/15/13   gsk     FR 3672: CTCH enhancement changes
02/11/13   gsk/sr  EDRX Full Sleep initial check in.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12   gv      Feature HS RACH Initial code check in
03/14/11   scm     Remove "#ifdef T_MSM6290" check.
09/21/10   scm     Support CTCH DRX lengths greater than 4096 frames.
03/25/09   scm     Add FEATURE_WCDMA_NO_CTCH_IF_ANOTHER_ENTITY_ACTIVE definition.
02/18/09   prk     Changed the declaration of Inline functions to pass offline builds.
10/03/08   hk      Bringing in 7k Mailine fixes
04/24/08   scm     Eliminate use of WL1_STRx() macros.
04/08/08   scm     Remove extern to drx_entity_reset_all_entities().
02/27/08   scm     Extern drx_timeout_timer for use in wl1drxentity.c.
01/18/08   uk      Added new entity mask DRX_ENTITY_MASK_DRX_MANAGER corresponding
                   to the entity DRX_MANAGER
01/04/08   scm     Add new type: DRX_MANAGER_PSC_SCAN_DONE_RSP.
12/18/07   scm     Extern drx_manager_psc_scan_done().
12/12/07   scm     Handle DL to sleep timeout with local command, pretending
                   DL is asleep and put SRCH to sleep anyway.
                   Move "drx_manager_entity_handle" reset to
                   drx_entity_inactive_entry().
10/09/07   scm     Add MICH end to sleep_struct so DRX knows MICH read window.
08/15/07   scm     Implement drx_manager_release_drx_mode().
05/22/07   scm     No longer default all messages to ERROR.
04/28/07   scm     Change DRX_ENTITY_BCCH_S(N) to DRX_ENTITY_BCCH_INT(EXT).
04/18/07   scm     Modify next_wakeup_time() to also return usec to next wake.
04/18/07   scm     Print hw_sw_cfn_diff also when bad TLM SFN detected.
04/04/07   scm     Extern drx_manager_wake_from_sleep() for true sleep support.
03/31/07   scm     Switch some srchzz_xxx API's to srch_xxx.
03/28/07   scm     Support new entity suspend and resume commands.
03/23/07   scm     Add handle parameter to serialize_srchzz_wakeup_cells().
03/21/07   scm     Fix bug in entity mask definitions.
03/19/07   scm     Implement drx_manager_get_current_entity_mask().
03/17/07   scm     Add DRX_ENTITY_MTCH_3 to drx_entity_type.
03/16/07   scm     Add DRX_ENTITY_PCCH to drx_entity_type.
03/03/07   scm     Implement drx_manager_next_wakeup_time().
03/02/07   scm     Fix lint issue.
03/01/07   scm     Statically allocate DRX handles based upon drx_entity_type.
02/22/07   scm     Supporting new SRCH interface.
02/20/07   scm     Modify drx_srch_asleep_done() parameter list.
02/15/07   scm     Modify drx_srch_wakeup_done() parameter list.
02/14/07   scm     Some preliminary sleep abort support.
01/24/07   scm     Initial support of PICH with new DRX Manager.
12/22/06   scm     Tweak drx_manager_sleep_struct_type and its comment a bit.
12/18/06   scm     More cleanup and comments.
12/13/06   scm     Cleanup, comments, name changes.
11/16/06   scm     Additional DRX manager functionality.
11/10/06   scm     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "l1def.h"
#include "msg.h"
#include "srchzz.h"
#include "tlm.h"


/* When Channel Manager registers a channel with DRX Manager, DRX Manager
** returns this handle.  Used for future identification of this logical
** entity or channel. */
typedef int16  drx_entity_handle_type;

#define DRX_ENTITY_HANDLE_INVALID  (-1)

extern rex_crit_sect_type wl1drxmgr_wake_srch_buffr_mutex;

/* Channel Manager uses this type to identify the logical entity it is
** registering with DRX Manager. */
typedef enum
{
  DRX_ENTITY_INVALID = 0,

  /* These channels can have schedules associated with them.  There can only
  ** be one instance of these at a time, i.e. can't have 2 PICH's at once.
  ** It is OK to have a PICH and a MICH at once, etc. */
  DRX_ENTITY_PICH,
  DRX_ENTITY_MICH,
  DRX_ENTITY_CTCH,
  DRX_ENTITY_BCCH_INT,
  DRX_ENTITY_BCCH_EXT,
  DRX_ENTITY_MCCH,

  /* This one doesn't really have a schedule.  It sleeps indefinitely until
  ** a page is detected, but there can still only be one at a time. */
  DRX_ENTITY_PCCH,
  DRX_ENTITY_AICH,
  DRX_ENTITY_FDPCH,

  /* Entity for FACH-DRX */
  DRX_ENTITY_EDRX,

  /* Special type for DRX Manager, not really a channel, but gives DRX Manager
  ** a way to wake the UE for sleep aborts. */
  DRX_ENTITY_DRX_MANAGER,

  /* Entity for L1M */
  DRX_ENTITY_L1M,

  /* Entity that is used for doing SRCH and PICH at different times.
  ** Targeted for G2W Tune Away for now - can be used for others as well like TS TS.*/
  DRX_ENTITY_SPLIT_SRCH,

  DRX_ENTITY_ONE_INSTANCE_ONLY_MAX_TYPE ,

  /* We can have 4 MTCH channels active at once. */
  DRX_ENTITY_MTCH,
  DRX_ENTITY_MTCH_0 = DRX_ENTITY_MTCH,
  DRX_ENTITY_MTCH_1,
  DRX_ENTITY_MTCH_2,
  DRX_ENTITY_MTCH_3,

  /* Channels with no schedule (100% active) like PCCH, DTCH, etc.  Let's just
  ** say we can only have 3 at once for now. */
  DRX_ENTITY_ALWAYS_ACTIVE,
  DRX_ENTITY_ALWAYS_ACTIVE_0 = DRX_ENTITY_ALWAYS_ACTIVE,
  DRX_ENTITY_ALWAYS_ACTIVE_1,
  DRX_ENTITY_ALWAYS_ACTIVE_2,

  DRX_ENTITY_NUM_TYPES
} drx_entity_type;

/* These are bitmasks corresponding to drx_entity_type just above. */
#define DRX_ENTITY_MASK_INVALID          (0x00000001UL << (int16)DRX_ENTITY_INVALID)
#define DRX_ENTITY_MASK_PICH             (0x00000001UL << (int16)DRX_ENTITY_PICH)
#define DRX_ENTITY_MASK_MICH             (0x00000001UL << (int16)DRX_ENTITY_MICH)
#define DRX_ENTITY_MASK_CTCH             (0x00000001UL << (int16)DRX_ENTITY_CTCH)
#define DRX_ENTITY_MASK_BCCH_INT         (0x00000001UL << (int16)DRX_ENTITY_BCCH_INT)
#define DRX_ENTITY_MASK_BCCH_EXT         (0x00000001UL << (int16)DRX_ENTITY_BCCH_EXT)
#define DRX_ENTITY_MASK_MCCH             (0x00000001UL << (int16)DRX_ENTITY_MCCH)
#define DRX_ENTITY_MASK_PCCH             (0x00000001UL << (int16)DRX_ENTITY_PCCH)
#define DRX_ENTITY_MASK_DRX_MANAGER      (0x00000001UL << (int16)DRX_ENTITY_DRX_MANAGER)
#define DRX_ENTITY_MASK_MTCH             (0x00000001UL << (int16)DRX_ENTITY_MTCH)
#define DRX_ENTITY_MASK_MTCH_0           (0x00000001UL << (int16)DRX_ENTITY_MTCH_0)
#define DRX_ENTITY_MASK_MTCH_1           (0x00000001UL << (int16)DRX_ENTITY_MTCH_1)
#define DRX_ENTITY_MASK_MTCH_2           (0x00000001UL << (int16)DRX_ENTITY_MTCH_2)
#define DRX_ENTITY_MASK_MTCH_3           (0x00000001UL << (int16)DRX_ENTITY_MTCH_3)
#define DRX_ENTITY_MASK_ALWAYS_ACTIVE    (0x00000001UL << (int16)DRX_ENTITY_ALWAYS_ACTIVE)
#define DRX_ENTITY_MASK_ALWAYS_ACTIVE_0  (0x00000001UL << (int16)DRX_ENTITY_ALWAYS_ACTIVE_0)
#define DRX_ENTITY_MASK_ALWAYS_ACTIVE_1  (0x00000001UL << (int16)DRX_ENTITY_ALWAYS_ACTIVE_1)
#define DRX_ENTITY_MASK_ALWAYS_ACTIVE_2  (0x00000001UL << (int16)DRX_ENTITY_ALWAYS_ACTIVE_2)
#define DRX_ENTITY_MASK_EDRX             (0x00000001UL << (int16)DRX_ENTITY_EDRX)
#define DRX_ENTITY_MASK_SPLIT_SRCH       (0x00000001UL << (int16)DRX_ENTITY_SPLIT_SRCH)
#define DRX_ENTITY_MASK_L1M              (0x00000001UL << (int16)DRX_ENTITY_L1M)


#define DRX_ENTITY_MTCH_MASKS  (DRX_ENTITY_MASK_MTCH_0 | DRX_ENTITY_MASK_MTCH_1 | \
                                DRX_ENTITY_MASK_MTCH_2 | DRX_ENTITY_MASK_MTCH_3)

#define DRX_ENTITY_ALWAYS_ACTIVE_MASKS  (DRX_ENTITY_MASK_ALWAYS_ACTIVE_0 | \
                                         DRX_ENTITY_MASK_ALWAYS_ACTIVE_1 | \
                                         DRX_ENTITY_MASK_ALWAYS_ACTIVE_2)

/* How to convert an entity type to an entity mask. */
#define DRX_ENTITY_TYPE_TO_MASK(type)  (0x00000001UL << (int16)type)

/* How to convert a single bit set in a mask to an entity handle.  This macro
** does not support multiple bits set in the mask, so don't do it. */
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif 
drx_entity_handle_type DRX_MASK_TO_ENTITY_HANDLE(uint32 bit_mask)
{
  drx_entity_handle_type  handle = 0;


  /* Count the shifts (bit positions) to shift the bit out.  The bit mask
  ** should never be 0, but handle it just in case. */
  while ((bit_mask != DRX_ENTITY_MASK_INVALID) && (bit_mask != 0))
  {
    /* Increment to the next type and shift bit one step closer to out. */
    handle++;
    bit_mask >>= 1;
  }

  return handle;
}

/* When determining sleep duration, start off assuming a big, bogus value.
** Biggest positive number that fits in a uint32. */
#define LARGE_INVALID_SLEEP_TIME  0x7FFFFFFF

/* Number of microseconds in a frame. */
#define USEC_PER_FRAME  10000

/****************************************************************************
** When DRX Manager asks Channel Manager for activity information about a
** logical entity, these are the possible answers:
** - DRX_MANAGER_AWAKE_INDEFINITELY
**   Channel has no foreseeable period of inactivity, like DTCH for example.
** - DRX_MANAGER_ASLEEP_INDEFINITELY
**   Channel has no foreseeable period of activity.  Its schedule indicates
**   an indefinite period of sleep.
** - DRX_MANAGER_AWAKE_NOW
**   Channel should be active right now, for a finite duration.
** - DRX_MANAGER_ASLEEP_NOW
**   Channel can sleep right now, for a finite duration.
****************************************************************************/
typedef enum
{
  DRX_MANAGER_AWAKE_INDEFINITELY,
  DRX_MANAGER_ASLEEP_INDEFINITELY,
  DRX_MANAGER_AWAKE_NOW,
  DRX_MANAGER_ASLEEP_NOW,
  DRX_MANAGER_SLEEP_NUM_TYPES
} drx_manager_sleep_type;

/****************************************************************************
** When DRX Manager asks Channel Manager for activity information about a
** logical entity, Channel Manager returns this structure:
** - sleep_type
**   Channel activity state at the current time, i.e. right now.
** - target_time
**   Future time at which the channel activity state changes.  If state is
**   INDEFINITELY, "target_time" is not applicable.
** - mich_mod_period_end_time
**   Applies only to MICH.  Ignored/NA for all other channels.  For MICH, it
**   points to the last frame of the modification period to read MICH
**   (plus one).  So it points to the first frame of the next modification
**   period.  So target_time to mich_mod_period_end_time identifies the window
**   to read MICH.  Since MICH is transmitted continuously throughout the
**   modification period, DRX Manager can read MICH anytime in the window.
****************************************************************************/
typedef struct
{
  drx_manager_sleep_type        sleep_type;
  tlm_extended_sfn_struct_type  target_time;
  tlm_tfn_struct_type           mich_mod_period_end_time;
} drx_manager_sleep_struct_type;

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
typedef void (*drx_entity_wakeup_cb_type)(drx_entity_handle_type handle);

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
typedef void (*drx_entity_to_sleep_cb_type)(drx_entity_handle_type handle);

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
typedef boolean (*drx_next_frame_wanted_cb_type)
(
  drx_entity_handle_type          entity_handle,
  drx_manager_sleep_struct_type  *sleep_info_ptr
);

typedef struct
{
  drx_entity_wakeup_cb_type      entity_wakeup_cb;
  drx_entity_to_sleep_cb_type    entity_to_sleep_cb;
  drx_next_frame_wanted_cb_type  next_frame_wanted_cb;
} channel_cb_struct_type;

/* The different commands processed by DRX Manager. */
typedef enum
{
  DRX_MANAGER_SRCH_TO_SLEEP_RSP = 0,
  DRX_MANAGER_PSC_SCAN_DONE_RSP,
  DRX_MANAGER_DL_TO_SLEEP_TIMED_OUT,
  DRX_MANAGER_NUM_CMDS
} drx_manager_cmd_id_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type    local_cmd_hdr;

  drx_manager_cmd_id_type  cmd_id;
} drx_manager_srch_to_sleep_cmd_type;

typedef union
{
  struct
  {
    /* Need this to put command on l1_local_cmd_q. */
    l1_local_cmd_hdr_type             local_cmd_hdr;

    drx_manager_cmd_id_type           cmd_id;
  } header;

  drx_manager_srch_to_sleep_cmd_type  srch_to_sleep_cmd;
} drx_manager_cmd_type;

/* Special handling for PICH requires that we keep special track of
** whether PICH is physically active or not. */
extern boolean drx_manager_pich_awake;

/* Timer to detect timeouts on responses from SRCH and Channel Manager. */
extern timer_type drx_timeout_timer;

/* DRX entity handle returned by drx_register_entity().  This particular entity
** does not correspond to a channel, like most do.  Instead it is used to wake
** the UE from sleep on RRC command reception, i.e. sleep abort. */
extern drx_entity_handle_type  drx_manager_entity_handle;

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
extern void drx_manager_wake_from_sleep(void);

/*===========================================================================
FUNCTION     DRX_MANAGER_RELEASE_DRX_MODE

DESCRIPTION
  This function is called by drx_rel().  DRX mode operation has been stopped.
  If a sleep abort happened not long ago, then the DRX_ENTITY_DRX_MANAGER
  logical entity is active with DRX Manager.  This entity's sole
  responsibility is to prevent sleep for awhile after a sleep abort.

  Not that DRX mode has stopped, we don't need DRX_ENTITY_DRX_MANAGER
  registered anymore.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_manager_release_drx_mode(void);

/*===========================================================================
FUNCTION     DRX_MANAGER_UPDATE_TIME_TO_NEXT_WAKEUP

DESCRIPTION
  This function checks whether time to next wakeup is invalid. If invalid
  changes it based on drx cycle len.

DEPENDENCIES
  None.

PARAMETERS
  int32 time_to_next_wakeup_usec
  uint32 msec_to_sleep

RETURN VALUE
  None.
===========================================================================*/

extern void drx_manager_update_time_to_next_wakeup(int32 *time_to_next_wakeup_usec, uint32 *msec_to_sleep);

/*===========================================================================
FUNCTION     DRX_MANAGER_UPDATE_TIME_TO_NEXT_WAKEUP

DESCRIPTION
  This function checks whether time to next wakeup is invalid. If invalid
  changes it based on drx cycle len.

DEPENDENCIES
  None.

PARAMETERS
  int32 time_to_next_wakeup_usec
  uint32 msec_to_sleep

RETURN VALUE
  None.
===========================================================================*/
extern void drx_manager_update_time_to_next_wakeup(int32 *time_to_next_wakeup_usec, uint32 *msec_to_sleep);

/*===========================================================================
FUNCTION     DRX_MANAGER_START_UE_SLEEP

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
extern void drx_manager_start_ue_sleep(void);

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
  wakeup_set_info - Struct for the cell to wake up.
  

RETURN VALUE
  None.
===========================================================================*/
extern void drx_manager_serialize_srch_wakeup_cells
(
  drx_entity_handle_type       handle,
  srch_wakeup_set_struct_type  wakeup_set_info
);

/*===========================================================================
FUNCTION     WL1DRXMGR_CLEAR_HNDLE_FROM_WKUP_SRCH_BUF

DESCRIPTION
  This function clears the entity that wakes up from the 
   wkup_srch_buf.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
boolean wl1drxmgr_clear_hndle_from_wkup_srch_buf(drx_entity_handle_type handle);

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
extern boolean drx_manager_process_cmd(const drx_manager_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION     DRX_REGISTER_ENTITY

DESCRIPTION
  This function is called by Channel Manager when RRC sets up a channel with
  the CPHY_SETUP_REQ command.  Channel Manager passes in all information
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
  drx_entity_handle_type - Unique logical entity (or channel) identifier.
===========================================================================*/
extern drx_entity_handle_type drx_register_entity
(
  drx_entity_type          entity_type,
  uint16                   dl_uarfcn,
  uint16                   psc,
  boolean                  channel_awake,
  channel_cb_struct_type  *cb_functions
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
extern void drx_deregister_entity(drx_entity_handle_type handle);

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
extern boolean drx_suspend_entity(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_RESUME_ENTITY

DESCRIPTION
  Channel Manager calls this DRX Manager function to resume an entity's
  state machine.  This function should only be called sometime after
  drx_suspend_entity() is called.

DEPENDENCIES
  None.

PARAMETERS
  handle - Uniquely identifies the entity being resumed.

RETURN VALUE
  boolean - TRUE on success.  FALSE on failure.
===========================================================================*/
extern boolean drx_resume_entity(drx_entity_handle_type handle);

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
extern void drx_entity_schedule_update(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION     DRX_ENTITY_SCHED_UPD_FOR_SKIP_CTCH_SFN

DESCRIPTION 
  This schedule update doesnt post a local command. 
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
extern void drx_entity_sched_upd_for_skip_ctch_sfn(drx_entity_handle_type handle);

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
extern uint32 drx_manager_get_current_entity_mask(void);

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
extern void drx_entity_wakeup_done
(
  drx_entity_handle_type handle,
  boolean                status
);

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
extern void drx_entity_asleep_done
(
  drx_entity_handle_type handle,
  boolean                status
);

/*===========================================================================
FUNCTION     DRX_DL_TO_SLEEP_DONE

DESCRIPTION
  Channel Manager calls this DRX Manager function once it has completely put
  itself to sleep.  This means it has dropped all physical channels in
  preparation for DRX Manager's putting the UE to sleep.

DEPENDENCIES
  None.

PARAMETERS
  status - TRUE means Putting Channel Manager (DL) to sleep succeeded.
           FALSE means Putting Channel Manager (DL) to sleep failed.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_dl_to_sleep_done(boolean status);

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
  psc_woken_up_info - Cell struct that just woke up.
  

RETURN VALUE
  None.
===========================================================================*/
extern void drx_srch_wakeup_done
(
  const srch_wakeup_set_struct_type  psc_woken_up_info
);

/*===========================================================================
FUNCTION     DRX_SRCH_ASLEEP_DONE

DESCRIPTION
  SRCH invokes this function in response to DRX Manager calling
  srchzz_all_cells_asleep().  DRX Manager calls this function once all logical
  entities are sleeping, and DRX Manager wants to put the phone to sleep.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_srch_asleep_done(void);

/*===========================================================================
FUNCTION     DRX_MANAGER_PSC_SCAN_DONE

DESCRIPTION
  This function should get called when a PSC scan ends.  It will start UE
  sleep, if there's enough time to sleep before the next entity wakes up.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void drx_manager_psc_scan_done(void);

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
extern void drx_manager_init(void);
/*===========================================================================
FUNCTION    DRX_MANAGER_ALLOW_SLEEP

DESCRIPTION
  This function checks if we need to allow sleep even though sleep time is invalid.
  This might happen during G2W TA mode in BCH or inter f resel in PCH_SLEEP. As there
  wont be any valid entity at that point of time we will have invalid sleep time. Idea
  is that RRC will send us a ext cmd and take us out of this state.
  
  scenarios. 
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Boolean - TRUE if sleep can be allowed, FALSE otherwise.
===========================================================================*/
extern boolean drx_manager_allow_sleep(void);

#endif  /* #ifndef WL1DRXMANAGER_H */
