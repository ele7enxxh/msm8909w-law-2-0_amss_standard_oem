/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   DBM   M A N A G E R

GENERAL DESCRIPTION
  This module contains functions used by the call manager in processing
  Data Burst messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmxdbm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/12   sk      Fixing the CW and KW warnings
11/30/11   jh      Added feature for dynamic rat acq order change
01/10/12   gm      Mainlining HDR API change integration
01/06/12   gm      Data synchronization changes for freefloating platform
04/11/11   am      Cleaning stubs and runnning qtf SUTE
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/16/11   xs      Mainline KDDI high psist requirement
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
07/14/10   mh      Fixed compile warnings
12/08/09   fj      Added lte_band_pref in cmph_force_pref_on_the_fly().
09/01/09   sv      Moving from clk API to time API
02/27/09   rm      Adding header files of other MOBs
12/17/08   jd      Fixed Lint errors
12/13/07   sk/ak   Added DCM.
08/07/07   ic      Lint clean up
06/22/07   sk      DDTM rewrite.
06/18/07   cl      Fix the issue for dequeing DOS Message
06/14/07   pk/cl   Added support for power suspend
04/30/07   jqi     Added EMPA support for MT DBM over HDR.
04/23/07   jqi     DOS feature rework for dual processor arch.
03/14/07   ic      In cmdbm_forward_msg_to_hdrmc() featurized mo_dos.route
                   field access under FEATURE_HDR_EMPA
01/23/07   pk      Controlling cmph_mo_sms_end under CDMA features
12/14/06   ic      FEATURE_HDR_EMPA tune up, restoring original fields in
                   cm_dbm_info_s_type and cm_dbm_data_block_s_type
12/01/06   ic      Fixed NULL ptr bug in cmdbm_forward_msg_to_mc()
11/20/06   ic      Mainlined FEATURE_HDR_EMPA
11/16/06   pk      Lint Fixes
11/16/06   ic      Added support for FEATURE_HDR_EMPA
10/26/06   sk      Fixed resend of already sent DBM when SD sends ok to orig.
05/12/05   sk      Fixed cmdbm_client_cmd_check() to allow DBM messages when
                   there is no 1x service on MAIN instance but there is HDR
                   service on hybrid instance.
03/10/06   pk      Added support for high PSIST
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic       Lint cleanup
11/08/05   sk       Separated CM reports and commands.
11/03/05   ic       Added CM_ASSERT() for ptr function parameters and lint
                    comments for 613 Lint error
10/25/05   pk      Fixed RVCT compiler warnings
10/20/05   sk      Fixed CM_HDR_DOS_HEADER_SIZE to be in line with the
                   standards. A HDR team member let me know of this change.
                   Fixed GPS DBM over HDR DOS issue.
                   Improved SDB success rate by using base station IDM to
                   determine which protocol should be used to send SDB
                   messages.
10/09/05   ic      Included directly dsm.h
09/12/05   pk      Modified call to cmph_force_pref_on_the_fly
09/12/05   ic      Fixed lint and compiler warnings
08/15/05   pk      Added support for priority Queues for origination
                    parameters
08/04/05   ic      Featurization changes in cmdbm_client_cmd_proc_next() so
                   that CM actually sends DBM when FEATURE_HDR_DOS is off and
                   we're in hybrid operation
06/16/05   ic      Include hdrmc.h only if FEATURE_HDR_DOS is defined
06/15/05   ic      DOS fixes so DBM go over HDR both while in HDR mode only
                    and while in HDR hybrid operation
06/10/05   ic      Updated comment in cmdbm_forward_msg_to_hdrmc()
04/18/05   ic      Merged in third installment of DOS changes
03/13/05   ic      Merged in second installment of DOS changes
03/04/05   ic      Merged in DataOverSignaling (DOS) support
02/01/05   dk      Added WLAN Support.
11/11/04   sj      Rearranged featurization for DDTM_CNTL.
11/15/04   ic      Lint cleanup with CM_DEBUG on
09/14/04   ws      Remove calls to srch_rude_wakeup.
08/20/04   sj      Added support for PRL Preference.
08/04/04   ic      CM Lint clean up - cmxdbm.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/15/04   sj      Changed pref_reason to ORIG_START_CS.
07/14/04   ws      Fixed compiler warnings with the addition of modifying info_structs
                    in event generations.
07/12/04   ic      In DBM command replaced client field with client_id field
                   to make it consistent with the rest of CM objects
06/30/04   ic      Fixed obvious RVCT 2.1 compiler warnings (Jaguar build)
05/27/04   ic      In cmdbm_client_cmd_proc() call cmdbm_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/11/04   ws      Do not check for overall call state in MO DBM.
04/23/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.28 and MSMSHARED_CM.01.00.10.00.31
03/24/04   ic      Featurization bug fixes.
03/18/04   sj      Fixed compile errors on unit test.
03/02/04   ic      Fixed compile error (wrong number of arguments to
                    cmph_force_pref_on_the_fly())
02/12/04   ic      Fixed compile error when FEATURE_GPSONE is turned off
02/06/04   ws      Initial jaguar/mainline merge.
09/18/03   vt      Added functions to update DDTM status.
08/14/03   vt      Added DDTM.
08/14/03   az      Added support to return overflow size for MO SDB.
04/28/03   AT      Lint cleanup.
02/27/03   ws      Fixes for MO SDB timeout problems.
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
10/31/02   vt      Mainlined feature SD20.
10/29/2002 ws      Fixed a typo in cmd_type in previous revision.
10/25/2002 ws      Corrected compile problem when referring to cmd_type in CM_FUNC_START
10/04/2002 az      Changed the implementation to use internal queue API
07/09/2002 az      Initial Release.

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
     !defined(FEATURE_MMODE_REMOVE_1X))
#include "comdef.h"
#include "cm.h"
#include "cm_v.h"
#include "cmi.h"
#include "time_svc.h"

#include "cmxdbm.h"
#include "cmdbg.h"
#include "cmph.h"
#include "cmclient.h"

#ifdef FEATURE_HDR_DOS
#include "hdrcp_msg.h"
#include "dsm.h"
#include "cmssidm.h"
#endif

#ifdef FEATURE_JCDMA
#include "cmcall.h"
#endif

#ifdef FEATURE_DEDICATED_C2K_MODE
#include "cmxcall.h"
#endif

#ifdef CM_DEBUG
#error code not present
#endif

#include "cmefs.h"
#include "cmtaski.h"
#include "cm_msgr_msg.h"
#include "cmclnup.h"
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                  GLOBALS                                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*
** 749: local enum constant not referenced
** This is fine, we want it for consistency
*/
/*lint -esym(749,CMDBM_MAX)*/

/*  This keeps track of the status of each DBM. Initially the status is
**  set to CMDBM_NONE.
*/
typedef enum
{

  CMDBM_NONE,

  CMDBM_WAIT_SESSION_CLOSE, /* Waiting for HDR session to close */
  CMDBM_FORCED_PREF, /* Forced the preference */
  CMDBM_HOLD_ORIG,   /* Received Hold Orig from MC */
  CMDBM_MSG_SENT,    /* Sent the DBM to MC */

  CMDBM_MAX
} cmdbm_msg_status_type;


/*  This structure stores the information associated with each DBM.
*/

typedef struct
{
  q_link_type           link;
    /* Q linking element

       This element has to be the first
       element in this data structure
       because of removal of self_ptr
       in q_link_type
     */

  boolean valid;                            /* Is this DBM valid */
  cm_dbm_cmd_s_type cm_dbm_cmd;             /* DBM contents */
  cmdbm_msg_status_type is_cmd_sent_to_MC;  /* Status of sending of DBM */
} cmdbm_cmd_queue_type;

static cmdbm_cmd_queue_type cm_dbm_cmd_queue[CM_DBM_CMD_HOLD_QUEUE_SIZE];

/*  Stores the DBM that CM is currently processing
 */
/* in_CS for all tasks. */
static cmdbm_cmd_queue_type curr_dbm_obj;

/*  This keeps track if the preference has been forced to send DBM.
*/
static boolean is_pref_forced;

/* Queues to manage CM DBM pool
*/
typedef q_type   cmdbm_buf_q_type;

/* DBM Queue
*/
static cmdbm_buf_q_type      cmdbm_client_dbm_q;

/* Free Queue
*/
static cmdbm_buf_q_type      cmdbm_free_buf_q;


/*  This function is called during linear search of the queue to find
 *  DBM's whose l2_ack_wait_time has expired
*/
static int cmdbm_l2_ack_wait_time_compare( void* item_ptr, void* compare_val );


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               FUNCTION DEFINITIONS                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION cmdbm_free_buf_q_init

DESCRIPTION
  Initialize the CMDBM free Buffer queue object.

  This function must be called before the CM free Buffer queue is used,
  in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmdbm_free_buf_q_init( void )
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cmdbm_free_buf_q_init(), free commands=%d",
                 ARR_SIZE(cm_dbm_cmd_queue) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CMDBM free buffer queue.
  */
  (void) q_init( &cmdbm_free_buf_q );

  /* Link and place each of the CMDBM buffer locations
  ** into the free buffer queue.
  */
  for ( i=0; i < ARR_SIZE(cm_dbm_cmd_queue); i++ )
  {
    (void) q_link( &cm_dbm_cmd_queue[i], &cm_dbm_cmd_queue[i].link );
    q_put( &cmdbm_free_buf_q, &cm_dbm_cmd_queue[i].link );
  }

  /* Our free queue is ready at this point.
  */

}

/*===========================================================================

FUNCTION cmdbm_free_buf_q_get

DESCRIPTION
  Dequeue a buffer from the head of the CMDBM free buffer queue.

DEPENDENCIES
  cmdbm_free_buf_q_init() must have already been called.

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cmdbm_cmd_queue_type * cmdbm_free_buf_q_get( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Dequeue a command buffer
  ** from the free Buffer Q.
  */
  return (cmdbm_cmd_queue_type *) q_get( &cmdbm_free_buf_q );

}


/*===========================================================================

FUNCTION cmdbm_free_buf_q_put

DESCRIPTION
  Enqueue a command buffer onto the CM free DBM queue.

DEPENDENCIES
  cmdbm_free_buf_q_init() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmdbm_free_buf_q_put(

    cmdbm_cmd_queue_type * cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{

  CM_ASSERT( cmd_ptr != NULL );
  CM_MSG_LOW_1("START cmdbm_free_buf_q_put(), cmd_type=%d",cmd_ptr->cm_dbm_cmd.cmd );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** onto the CM free command Q.
  */
  q_put( &cmdbm_free_buf_q, &cmd_ptr->link );

}

/*===========================================================================

FUNCTION cmdbm_client_dbm_q_init

DESCRIPTION
  Initialize the client DBM queue.

  This function must be called before the client DBM queue is used in
  any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmdbm_client_dbm_q_init( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) q_init( &cmdbm_client_dbm_q );
}

/*===========================================================================

FUNCTION cmdbm_client_dbm_q_put

DESCRIPTION
  Enqueue a command buffer onto the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cmdbm_client_dbm_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmdbm_client_dbm_q_put(

    cmdbm_cmd_queue_type    *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr           != NULL);
  CM_MSG_LOW_1("START cmdbm_client_dbm_q_put(), cmd_type=%d",
                 cmd_ptr->cm_dbm_cmd.cmd );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Queue the command in the CM client DBM queue
  */
  q_put( &cmdbm_client_dbm_q, &cmd_ptr->link );
}

/*===========================================================================

FUNCTION cmdbm_client_dbm_q_delete

DESCRIPTION
  Removes a DBM from the DBM queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cmdbm_client_dbm_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmdbm_client_dbm_q_delete(

    cmdbm_cmd_queue_type    *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*  Delete the command in the CM client DBM queue
  */
  q_delete( &cmdbm_client_dbm_q, &cmd_ptr->link );
}


/*===========================================================================

FUNCTION cmdbm_client_dbm_q_get

DESCRIPTION
  Dequeue a command buffer from the client DBM queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cmdbm_client_dbm_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cmdbm_cmd_queue_type * cmdbm_client_dbm_q_get( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get next command from the CM client command queue.
  */
  return ((cmdbm_cmd_queue_type *) q_get( &cmdbm_client_dbm_q ));

}



/*===========================================================================

FUNCTION cmdbm_get_queue_count

DESCRIPTION
  Number of items in the supplied queue.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static int cmdbm_get_queue_count(q_type q)
{

  return (q_cnt(&q));
}

/*===========================================================================

FUNCTION cmdbm_copy_dbm_cmd

DESCRIPTION
  Copies the DBM command found on the CM command queue to the free queue
  buffer. Before this function is called all the necessary error checks have
  been performed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful in buffering.

SIDE EFFECTS
  None

===========================================================================*/

static boolean cmdbm_copy_dbm_cmd
(
    cmdbm_cmd_queue_type *client_cmd_free_buf,
    const cm_dbm_cmd_s_type *dbm_cmd_ptr
)
{

  if ((dbm_cmd_ptr != NULL) && (client_cmd_free_buf != NULL))
  {
    client_cmd_free_buf->valid             = TRUE;
    client_cmd_free_buf->is_cmd_sent_to_MC = CMDBM_NONE;

    /* Populate the dbm cmd buffer
    */
    *(&(client_cmd_free_buf->cm_dbm_cmd)) = *dbm_cmd_ptr;

    client_cmd_free_buf->cm_dbm_cmd.l2_ack_wait_time =
                     dbm_cmd_ptr->l2_ack_wait_time + time_get_uptime_secs();

    return TRUE;
  }

  return FALSE;

}

/*===========================================================================

FUNCTION cmdbm_put_in_dbm_buf_q

DESCRIPTION
  Copies the DBM command found on the CM command queue to the DBM queue.
  Before this function is called all the necessary error checks have been
  performed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful in buffering.
  FALSE - If out of buffering space.

SIDE EFFECTS
  None

===========================================================================*/

static boolean cmdbm_put_in_dbm_buf_q( const cm_dbm_cmd_s_type * dbm_cmd_ptr )
{
    cmdbm_cmd_queue_type * client_cmd_free_buf;
    boolean copy_status = FALSE;

    /*  Check if we are already out of buffers
    */
    if (cmdbm_get_queue_count(cmdbm_client_dbm_q) >=
                                              CM_DBM_CMD_HOLD_QUEUE_SIZE )
    {
      return FALSE;
    }

    /*  Get a free DBM buffer from the free queue.
    */
    client_cmd_free_buf = cmdbm_free_buf_q_get();

    if (client_cmd_free_buf == NULL)
    {
      /*  We are out of free buffers
      */
      return FALSE;
    }

    /*  Load the DBM object with the incoming DBM command.
    */
    copy_status = cmdbm_copy_dbm_cmd( client_cmd_free_buf, dbm_cmd_ptr);

    if (copy_status)
    {
      /*  Copy was successful. Put the received DBM on the queue.
      */
      cmdbm_client_dbm_q_put(client_cmd_free_buf);
    }
    else
    {
      /*  Copy was unsuccessful. Put the free DBM buffer back on
       *  the free queue
      */
      cmdbm_free_buf_q_put(client_cmd_free_buf);
    }

    return copy_status;

}


/*===========================================================================

FUNCTION cmdbm_event

DESCRIPTION
  Notify the client list of a specified dbm event.

DEPENDENCIES
  dbm object must have already been initialized with
  cmdbm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmdbm_event(
    cm_dbm_info_s_type         *dbm_info_ptr,
        /* pointer to a dbm object.  */

    cm_dbm_event_e_type         dbm_event
        /* notify client list of this dbm event */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dbm_info_ptr != NULL );
  CM_ASSERT( BETWEEN(dbm_event, CM_DBM_EVENT_NONE, CM_DBM_EVENT_MAX) );
  CM_MSG_HIGH_1( "dbm event=%d",dbm_event);

  /*---------------------------------
    Now notify the clients of this
    event.
   *------------------------------ */

  cmclient_list_dbm_event_ntfy(dbm_event, dbm_info_ptr);

}



/*===========================================================================

FUNCTION cmdbm_client_cmd_check

DESCRIPTION
  Check for DBM command parameter errors and whether a specified DBM command
  is allowed in the current state of the call/phone.

DEPENDENCIES
  None

RETURN VALUE
  CM_DBM_CMD_ERR_NONE if command is allowed in the current state
  of the call/phone, specific cm_dbm_cmd_err_e_type otherwise.

SIDE EFFECTS
  none
===========================================================================*/

static cm_dbm_cmd_err_e_type cmdbm_client_cmd_check(
    cm_dbm_cmd_s_type          *dbm_cmd_ptr
        /* Pointer to a CM command */
)
{
  cmph_s_type               *ph_ptr      = cmph_ptr();
  /* Pointer for phone object */

  cmss_s_type               *ss_ptr      = cmss_ptr();
  /* Pointer for ss object */

  cm_dbm_cmd_err_e_type   cmd_err        = CM_DBM_CMD_ERR_NOERR;
  /* Initialize command error to NO-ERROR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dbm_cmd_ptr != NULL );
  CM_ASSERT( dbm_cmd_ptr->cmd_type == CM_CMD_TYPE_DBM );
  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_1( "cmdbm_client_cmd_check, cmd=%d", dbm_cmd_ptr->cmd);

  /* check cmd is already in error or not */

  if (dbm_cmd_ptr->cmd_err != CM_DBM_CMD_ERR_NOERR)
  {
    return dbm_cmd_ptr->cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  Check if the number of bytes is less than or equal to
   *  CM_DBM_MAX_PAYLOAD_SIZE. If yes than proceed further.
   *  If not send back a failure report to the interested clients
  */
  if (dbm_cmd_ptr->info.num_bytes > CM_DBM_MAX_PAYLOAD_SIZE)
  {
    /*  The supplied payload is more than maximum size supported
    */
    return CM_DBM_CMD_ERR_OTHER;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether command is possible.
  */
  switch ( dbm_cmd_ptr->cmd )
  {
    case CM_DBM_CMD_SEND_MSG:

      if ( dbm_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        /* Invalid client ID */
        cmd_err  = CM_DBM_CMD_ERR_CLIENT_ID_P;
        CM_ERR_0( "CM_DBM_CMD_ERR_CLIENT_ID_P");
      }
      else if ( (dbm_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
      {
        /* Invalid client ID */
        cmd_err  = CM_DBM_CMD_ERR_CLIENT_ID_P;
        CM_ERR_0( "CM_DBM_CMD_ERR_CLIENT_ID_P");
      }
      else if ( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE ||
                cmclnup_is_ongoing_subsc(SYS_MODEM_AS_ID_1_MASK) )
      {
        cmd_err = CM_DBM_CMD_ERR_OFFLINE_S;
        CM_ERR_0( "CM_DBM_CMD_ERR_OFFLINE_S");
      }
      else if ( ph_ptr->cdma_lock_mode == CM_CDMA_LOCK_MODE_ON )
      {
        /* CDMA lock is On */
        cmd_err = CM_DBM_CMD_ERR_CDMA_LOCK_S;
        CM_ERR_0( "CM_DBM_CMD_ERR_CDMA_LOCK_S");
      }
      /* Check if a DO call is in origination or connected, if it is then
      ** if the ddtm mask is set to block MO DBM return error
      */
      /*lint -e655 */
      /* 655: bit-wise operation uses (compatible) enum's */
      else if( cm_is_jcdma_enable() && (cmcall_count_on_ss(CM_SS_HDR) != 0 ) &&
            ( ( ph_ptr->cur_ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM ) ||
          ( ( ph_ptr->ddtm_pref == CM_DDTM_PREF_ON )&&
            ( ph_ptr->ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM )
          )
        )
       )
      {
        cmd_err = CM_DBM_CMD_ERR_DDTM;
        CM_ERR_0( "CM_DBM_CMD_ERR_DDTM");
      }
      /*lint +e655 */
      else
      {
        if(ss_ptr->info.sys_mode == SYS_SYS_MODE_NO_SRV &&
           ss_ptr->info.hdr_srv_status == SYS_SRV_STATUS_NO_SRV)
        {
          cmd_err = CM_DBM_CMD_ERR_NO_SRV_S;
          CM_ERR_0( "CM_DBM_CMD_ERR_NO_SRV_S");
        }
        else if (ss_ptr->info.sys_mode == SYS_SYS_MODE_AMPS)
        {
          cmd_err = CM_DBM_CMD_ERR_NOT_ALLOWED_IN_AMPS_S;
          CM_ERR_0( "CM_DBM_CMD_ERR_NOT_ALLOWED_IN_AMPS_S");
        }
      }
      break;

    default:
      CM_ERR_1("Unexpected dbm_cmd_ptr->cmd %d", dbm_cmd_ptr->cmd);
      break;
  }

  /* check cmd is already in error or not */
  if (cmd_err != CM_DBM_CMD_ERR_NOERR)
  {
    return cmd_err;
  }


  /*  No erors were discovered. Copy the sent command locally in a queue.
  */

  if (!cmdbm_put_in_dbm_buf_q(dbm_cmd_ptr))
  {
    /*  Not Successful in copying. We might be out of queue buffers.
    */

    cmd_err = CM_DBM_CMD_ERR_NO_BUF_L;
  }

  return cmd_err;
}

/*===========================================================================

FUNCTION cmdbm_client_cmd_err

DESCRIPTION
  Notify clients of a specified dbm command error.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmdbm_client_cmd_err(
    const cm_dbm_cmd_s_type       *dbm_cmd_ptr,
        /* Pointer to a CM command */
    cm_dbm_cmd_err_e_type    cmd_err
        /* Indicate the command error */
)
{

  cm_dbm_info_s_type          info;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dbm_cmd_ptr != NULL );
  CM_ASSERT( dbm_cmd_ptr->cmd_type == CM_CMD_TYPE_DBM );
  CM_ASSERT( BETWEEN( cmd_err, CM_DBM_CMD_ERR_NONE, CM_DBM_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the buffer
  */
  memset( &info, 0, sizeof(cm_dbm_info_s_type) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_3("START cmdbm_client_cmd_err() dbm_cmd=%d, cmd_err=%d, client=%p",
                  dbm_cmd_ptr->cmd, cmd_err, dbm_cmd_ptr->client_id );

  /*---------------------------------
    If commanding client supplied a callback function,
    notify this client of command error status.
   *------------------------------ */

  if(dbm_cmd_ptr->cmd_cb_func != NULL)
  {
    dbm_cmd_ptr->cmd_cb_func(dbm_cmd_ptr->data_block_ptr,
                            dbm_cmd_ptr->cmd,
                            cmd_err);
  }
  else
  {
    /*  Check if the number of bytes is less than or equal to
     *  CM_DBM_MAX_PAYLOAD_SIZE. If yes than proceed further.
     *  If not send back a failure report to the interested clients
    */
    if (dbm_cmd_ptr->info.num_bytes > CM_DBM_MAX_PAYLOAD_SIZE)
    {
      /*  The supplied payload is more than maximum size supported
      */
      info.burst_type = dbm_cmd_ptr->burst_type;
      info.mo_status = CM_DBM_MO_ACCESS_TOO_LARGE;
      info.user_data = dbm_cmd_ptr->data_block_ptr;
      info.burst_overflow_size =
        dbm_cmd_ptr->info.num_bytes - CM_DBM_MAX_PAYLOAD_SIZE;
    }

    if (cmd_err == CM_DBM_CMD_ERR_NO_BUF_L)
    {
      /*  Not Successful in copying. We are out of queue buffers.
       *  Send back an event indicating CM_DBM_MO_OUT_OF_RESOURCES to AEE
       */

      info.burst_type = dbm_cmd_ptr->burst_type;
      info.mo_status = CM_DBM_MO_OUT_OF_RESOURCES;
      info.user_data = dbm_cmd_ptr->data_block_ptr;
    }
    else
    {
      info.burst_type = dbm_cmd_ptr->burst_type;
      info.mo_status =   CM_DBM_MO_OTHER;
      info.user_data = dbm_cmd_ptr->data_block_ptr;
    }

    cmph_update_ddtm_status( cmph_ptr() );        /* update the DDTM satus */
    cmdbm_event(&info, CM_DBM_EVENT_MO_STATUS);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------
    If error status is other than NO ERROR,
    notify the client list of command error, as well.
   *------------------------------ */

  if(cmd_err != CM_DBM_CMD_ERR_NOERR)
  {
    CM_ERR_3( "DBM cmd err, cmd=%d, err=%d, client=%ld",
            dbm_cmd_ptr->cmd, cmd_err, dbm_cmd_ptr->client_id );

    cmclient_list_dbm_cmd_err_ntfy( cmd_err, dbm_cmd_ptr );
  }
}


/*===========================================================================

FUNCTION cmdbm_forward_msg_to_mc

DESCRIPTION
  Forwards a DBM message to MC.

DEPENDENCIES
  MC must be in a state where sending a DBM message is possible (AC, at least).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void cmdbm_forward_msg_to_mc(cm_dbm_cmd_info_s_type  *msg, uint8 burst_type)
{
  mc_msg_type *mc_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( msg != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  mc_ptr = cm_mc_get_buf_else_err_fatal();
  mc_ptr->hdr.cmd            = MC_MO_DBM_F;
  mc_ptr->mo_dbm.dbm_buf_ptr = (void *) msg;
  mc_ptr->mo_dbm.ack_req     = TRUE;
  mc_ptr->mo_dbm.burst_type  = burst_type;

  if (curr_dbm_obj.valid)
  {
      CM_MSG_LOW_0("DBM, is_cmd_sent_to_MC set");
      curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_MSG_SENT;
  }

  cm_mc_send_cmd( mc_ptr );
}

#ifdef FEATURE_HDR_DOS
/*===========================================================================

FUNCTION cmdbm_forward_msg_to_hdrmc

DESCRIPTION
  Forwards a DBM message to HDRMC.

DEPENDENCIES
  HDRMC must be in a state where sending a DBM message is possible (AC, at least).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void cmdbm_forward_msg_to_hdrmc(cm_dbm_cmd_info_s_type  * msg)
{
  /*lint -save -e713 -e732 -e734  */
  byte                sdb_ctl_byte = 0x0;
  dsm_item_type       *dsm_item_ptr, *temp_item_ptr;
  int16               buf_len, remain_len;
  uint8               *buf;

  hdrcp_msg_req_u     *hdrcp_req;

  #define             CM_HDR_DOS_HEADER_SIZE 2

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(msg != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH_2("DBM (rlp_flow %x) (rout %x) to HDRMC",
              msg->rlp_flow, msg->route);

  /* Allocate DoS header size + 1 byte for HMP header (1 byte protocol ID)
  */
  temp_item_ptr = dsm_offset_new_buffer(DSM_DS_SMALL_ITEM_POOL,
                                        CM_HDR_DOS_HEADER_SIZE + 1);
  if ( temp_item_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  dsm_item_ptr = temp_item_ptr;

  /* Package the data as the DSM item as HDRDOS likes it that way
  */
  buf        = msg->payload;
  buf_len    = msg->num_bytes;
  remain_len = buf_len;
  while( remain_len > 0 )
  {
    temp_item_ptr->used = temp_item_ptr->size < remain_len ? /*lint !e737 */
                          temp_item_ptr->size : remain_len;  /*lint !e737 */

    memscpy( temp_item_ptr->data_ptr, (size_t)(temp_item_ptr->size - temp_item_ptr->used),
             &buf[buf_len - remain_len], temp_item_ptr->used );

    remain_len -= temp_item_ptr->used;
    if (remain_len <= 0 )
    {
      break;
    }
    temp_item_ptr->pkt_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
    if ( temp_item_ptr->pkt_ptr == NULL )
    {
      CM_ERR_FATAL_0( "Out of DSM items");
    }
    temp_item_ptr = temp_item_ptr->pkt_ptr;
  }

  /* Get the SDB Control Byte out as it is no longer needed.
  */
  (void) dsm_pullup( &dsm_item_ptr, &sdb_ctl_byte, sizeof(sdb_ctl_byte) );

  hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));

  hdrcp_req->mo_dos.dos_buf_ptr = (long) dsm_item_ptr;
  hdrcp_req->mo_dos.ack_req = TRUE;
  hdrcp_req->mo_dos.rlp_flow = msg->rlp_flow;
  hdrcp_req->mo_dos.route = msg->route;

  if (curr_dbm_obj.valid)
  {
    CM_MSG_MED_0("is_cmd_sent_to_MC set");
    curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_MSG_SENT;
  }

  CM_MSG_HIGH_0("send HDR_CP_SEND_DATA_OVER_SIGNALING_REQ");
  (void) cm_msgr_send( HDR_CP_SEND_DATA_OVER_SIGNALING_REQ, MSGR_MM_CM,
                       (msgr_hdr_struct_type*)hdrcp_req, sizeof(hdrcp_msg_req_u) );

  cm_mem_free(hdrcp_req);


  /*lint -restore */
}
#endif /* FEATURE_HDR_DOS */

/*===========================================================================

FUNCTION cmdbm_init

DESCRIPTION
  Initializing the DBM object.

  This function must be called before the DBM object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbm_init( void )
{
  uint8 counter;

  /*  Initialize the current DBM object
  */
  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access curr_dbm_obj */
  curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_NONE;
  curr_dbm_obj.valid = FALSE;
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  /* Initialize the force preference flag
  */
  is_pref_forced = FALSE;


  /* Initialize all DBM entries to invalid
  */
  for (counter = 0; counter < CM_DBM_CMD_HOLD_QUEUE_SIZE; counter++)
  {
    cm_dbm_cmd_queue[counter].valid = FALSE;
    cm_dbm_cmd_queue[counter].is_cmd_sent_to_MC = CMDBM_NONE;
  }

  /* Initialize the free queue
  */
  cmdbm_free_buf_q_init();

  /* Initialize the DBM buf queue
  */
  cmdbm_client_dbm_q_init();

}

/*===========================================================================

FUNCTION cmdbm_load_next_dbm

DESCRIPTION
  Copy the next DBM command found in the cm_dbm_cmd_queue buffer
  into curr_dbm_obj.

DEPENDENCIES
  dbm object must have already been initialized with
  cmdbm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmdbm_load_next_dbm(void)
{

  cmdbm_cmd_queue_type * temp_dbm_obj;

  if (curr_dbm_obj.valid)
  {
    /*  Current DBM is valid. Don't get the next one until
     *  this dbm access is completed.
    */
    return;
  }

  /*  There are no more outstanding DBM requests
  */
  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access curr_dbm_obj */
  curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_NONE;
  curr_dbm_obj.valid = FALSE;
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  temp_dbm_obj = cmdbm_client_dbm_q_get();

  if (temp_dbm_obj == NULL)
  {
    return;
  }

  /*  Obtained DBM Object is not null
  */

  /* Populate the dbm cmd info data structure from command queue.
  */
  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access curr_dbm_obj */
  curr_dbm_obj = *temp_dbm_obj;
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  /*  Return the DBM buf to the free queue
  */
  cmdbm_free_buf_q_put(temp_dbm_obj);

}


/*===========================================================================

FUNCTION cmdbm_client_cmd_proc_next

DESCRIPTION
  Processes next DBM commands found in the cm_dbm_cmd_queue buffer.

DEPENDENCIES
  dbm object must have already been initialized with
  cmdbm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmdbm_client_cmd_proc_next(void)
{

  cmss_s_type           *ss_ptr               = cmss_ptr();
  cmph_s_type           *ph_ptr               = cmph_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!curr_dbm_obj.valid)
  {
    /*  Get the next available DBM to send
    */

    cmdbm_load_next_dbm();
  }

  if ( ( curr_dbm_obj.is_cmd_sent_to_MC == CMDBM_NONE
         || curr_dbm_obj.is_cmd_sent_to_MC == CMDBM_WAIT_SESSION_CLOSE )
       && (curr_dbm_obj.valid) )
  {

    /*  When control reaches this point, there is atleast one DBM message
     *  to process.
     *  The current DBM message is not sent to MC even once.
     *  Hence send it now.
     */

    switch (curr_dbm_obj.cm_dbm_cmd.cmd)
    {
       case CM_DBM_CMD_SEND_MSG:

        #ifndef FEATURE_MMODE_REMOVE_1X
        #ifdef FEATURE_HDR_DOS
        if( curr_dbm_obj.cm_dbm_cmd.burst_type == CAI_SHORT_DATA_BURST &&
            cmssidm_get_base_station_idm() == SYS_SYS_MODE_HDR )
        {
          CM_MSG_HIGH_0("HDRMC ready for DBM, send ...");
          cmdbm_forward_msg_to_hdrmc(&curr_dbm_obj.cm_dbm_cmd.info);
        }
        else
        #endif /* FEATURE_HDR_DOS */
        #endif /* CAI_SHORT_DATA_BURST */
        if(ss_ptr->info.sys_mode != SYS_SYS_MODE_CDMA)
        {
          CM_MSG_HIGH_0("DBM dropped!");
        }
        else
        #ifdef FEATURE_DEDICATED_C2K_MODE
        if( ss_ptr->is_hdr_session_open )
        {
          CM_MSG_HIGH_0("DCM: Closing HDR session for DBM");
          cmph_send_hdrmc_session_close_cmd();
          cmxcall_end_call( cmcall_is_hdr_call(),
                            CM_CALL_END_HDR_RELEASED_BY_CM,
                            CAI_REL_NORMAL );
          /* Mark that the DBM is in progess. */
          curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_WAIT_SESSION_CLOSE;
        }
        else
        #endif
        if (cmcall_overall_call_state() == CM_OVERALL_CALL_STATE_IDLE)
        {

          /*---------------------------------
            If the MC is ready, forward it now;
            otherwise, force the mode to make it ready.
           *-------------------------------*/

          CM_MSG_HIGH_0("MC not ready for DBM, force ...");
          (void) cmph_force_pref_on_the_fly(
                                      CM_SS_MAIN,
                                      ph_ptr,
                                      SD_SS_PREF_REAS_ORIG_START_CS,
                                      CM_ACT_TYPE_SMS_CALL,
                                      CM_PREF_TERM_CM_MO_SMS,
                                      CM_MODE_PREF_NO_CHANGE,
                                      CM_BAND_PREF_NO_CHANGE,
                                      SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                      CM_BAND_PREF_NO_CHANGE,
                                      CM_PRL_PREF_NO_CHANGE,
                                      CM_ROAM_PREF_NO_CHANGE,
                                      CM_HYBR_PREF_NO_CHANGE,
                                      ph_ptr->main_stack_info.pref_info.plmn,
                                      CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                      CM_OTASP_ACT_CODE_NONE,
                                      ph_ptr->main_stack_info.pref_info.acq_order_pref,
                                      CM_NETWORK_SEL_MODE_PREF_NONE,
                                      (cm_act_id_type) &curr_dbm_obj,
                                      CM_ACT_UPDATE_REAS_ACT_START,
                                      TRUE,
                                      cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                                      CM_DEFAULT_VALUE,
                                      ph_ptr->main_stack_info.pref_info.csg_id,
                                      ph_ptr->main_stack_info.pref_info.csg_rat);

          curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_FORCED_PREF;
          is_pref_forced = TRUE;

        }
        else
        {
          CM_MSG_HIGH_0("MC ready for DBM, send ...");
          cmdbm_forward_msg_to_mc(&curr_dbm_obj.cm_dbm_cmd.info,
                                   curr_dbm_obj.cm_dbm_cmd.burst_type);
        }

        cmph_update_ddtm_status( cmph_ptr() );       /* update DDTM status */
        break;

      default:
        CM_ERR_1("Invalid command recd %d", (uint8) curr_dbm_obj.cm_dbm_cmd.cmd);
        break;
    } /* switch */

  }
}




/*===========================================================================

FUNCTION cmdbm_client_cmd_proc

DESCRIPTION
  Processes DBM commands found on the CM command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  May force the mode of the phone if necessary.

===========================================================================*/

void cmdbm_client_cmd_proc(
      cm_dbm_cmd_s_type  *dbm_cmd_ptr
      )
{
  cm_dbm_cmd_err_e_type   cmd_err;

  CM_ASSERT(dbm_cmd_ptr != NULL);
  CM_ASSERT(dbm_cmd_ptr->cmd_type == CM_CMD_TYPE_DBM);

  cmd_err = cmdbm_client_cmd_check(dbm_cmd_ptr);

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmdbm_client_cmd_err(dbm_cmd_ptr, cmd_err);
  if( cmd_err != CM_DBM_CMD_ERR_NOERR )
  {
    return;
  }

  cmdbm_client_cmd_proc_next();
}


/*===========================================================================

FUNCTION cmdbm_mt_msg_cdma

DESCRIPTION
  This function is to be called by CM whenever a MT DBM is received from MC.
  This function will generate a DBM event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void cmdbm_mt_msg_cdma(  uint8* payload,
                                uint8  num_bytes,
                                uint8  burst_type,
                                cm_dbm_source_e_type  burst_source
                 )
{
  cm_dbm_info_s_type dbm_info;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(payload != NULL);
  CM_ASSERT(num_bytes > 0);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Reset the buffer.
  */
  memset( &dbm_info, 0, sizeof(cm_dbm_info_s_type) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dbm_info.num_bytes    = num_bytes;
  dbm_info.payload      = payload;
  dbm_info.burst_type   = burst_type;
  dbm_info.burst_source = burst_source;

  cmdbm_event(&dbm_info, CM_DBM_EVENT_MT_MSG);
}



/*===========================================================================

FUNCTION cmdbm_mt_msg_hdr

DESCRIPTION
  This function is to be called by CM whenever a MT DBM is received from HDR
  CP. This function will generate a DBM event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void cmdbm_mt_msg_hdr(

  void                  *payload,
    /* Pointer to the dsm item.
    */

  uint8                 num_bytes,
    /* Size of the dsm item
    */

  uint8                 burst_type,
    /* The burst type.
    */

  cm_dbm_source_e_type  burst_source,
    /* Indicate over which the burst is received.
    */

  uint8                rlp_flow,
    /* Corresponding Link Flow number for the DoS packet
    ** (FEATURE_HDR_EMPA)
    */

  byte                 route
    /* Route on which the DoS packet should go
    ** (FEATURE_HDR_EMPA)
    */

)
{
  cm_dbm_info_s_type dbm_info;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(payload != NULL);
  CM_ASSERT(num_bytes > 0);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Reset the buffer.
  */
  memset( &dbm_info, 0, sizeof(cm_dbm_info_s_type) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dbm_info.dos_num_bytes             = num_bytes;
  dbm_info.dos_payload               = payload;
  dbm_info.burst_type                = burst_type;
  dbm_info.burst_source              = burst_source;
  dbm_info.rlp_flow                  = rlp_flow;
  dbm_info.route                     = route;

  cmdbm_event(&dbm_info, CM_DBM_EVENT_MT_MSG);
}



/*===========================================================================

FUNCTION cmdbm_mc_rpt_proc

DESCRIPTION
  Process MC reports specific to DBM.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbm_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{
  /*lint -e{826} keep it suppressed for general / specific walks as well */
  cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;
      /* Pointer to MC report */

  cm_dbm_info_s_type      info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_mc_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the buffer.
  */
  memset( &info, 0, sizeof(cm_dbm_info_s_type) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(cm_mc_rpt_ptr->hdr.cmd)
  {

    case CM_MO_DBM_STATUS_F:
      #ifndef FEATURE_MMODE_REMOVE_1X
      CM_MSG_LOW_0("Got CM_MO_DBM_STATUS_F");
      if(CAI_POSITION_DET == cm_mc_rpt_ptr->mo_dbm_status.msg_type ||
         CAI_SHORT_DATA_BURST == cm_mc_rpt_ptr->mo_dbm_status.msg_type )
      {
        if(cm_mc_rpt_ptr->mo_dbm_status.status == CM_DBM_MO_HOLD_ORIG)
        {
          if (curr_dbm_obj.valid)
          {
            curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_HOLD_ORIG;
            CM_MSG_HIGH_0("HOLDORIG, waiting for OK from MC");
          }
        }
        else
        {

          if (curr_dbm_obj.valid &&
              (curr_dbm_obj.is_cmd_sent_to_MC == CMDBM_MSG_SENT))
          {

            info.burst_type = curr_dbm_obj.cm_dbm_cmd.burst_type;
            info.mo_status  =
                 (cm_dbm_mo_status_e_type) (cm_mc_rpt_ptr->mo_dbm_status.status);
            info.user_data  =
                 curr_dbm_obj.cm_dbm_cmd.data_block_ptr;
            info.burst_overflow_size =
                   cm_mc_rpt_ptr->mo_dbm_status.burst_overflow_size;


            CM_MSG_HIGH_2("status %d for msg (id %x). Notifying Client.",
                    info.mo_status, info.user_data);

            rex_enter_crit_sect(cmtask_crit_sect_ptr());
            /* Serialize data access curr_dbm_obj */
            curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_NONE;
            curr_dbm_obj.valid = FALSE;
            rex_leave_crit_sect(cmtask_crit_sect_ptr());

            cmph_update_ddtm_status( cmph_ptr() );/* update the DDTM satus */
            cmdbm_event(&info, CM_DBM_EVENT_MO_STATUS);
          }

          if (is_pref_forced)
          {
            if (cmdbm_get_queue_count(cmdbm_client_dbm_q) <= 0)
            {
              /*  Reset the flag
              */
              is_pref_forced = FALSE;

              /*------------------------------------------
               If we forced the mode to send this message,
               unforce it now.
              *------------------------------------------*/
              CM_MSG_LOW_0("Unforce mode after DBM msg sent ...");
              #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
              cmph_mo_sms_end( (dword)0, (cm_act_id_type)&curr_dbm_obj );
              #endif
            }
            else
            {
              /* There are more messages on the queue ......
              ** Let's increase the timer to make sure the other messages get through
              ** without timing out.
              */
              cmph_update_orig_mode_uptime(CM_SS_MAIN,
                                           (cm_act_id_type) &curr_dbm_obj );

            }
          }

          /*  Process the next command on the cmdbm_client_dbm_q
          */
          cmdbm_client_cmd_proc_next();

        }
      }
      #endif
      /*-----------------------------------------
        else, this is not a DBM msg (PD or SDB),
        ignore.
       *----------------------------------------- */

      break;

    case CM_MT_DBM_DATA_F:

      if(cm_mc_rpt_ptr->mt_dbm_data.burst_source == CM_DBM_FROM_1X)
      {
        cmdbm_mt_msg_cdma( cm_mc_rpt_ptr->mt_dbm_data.payload,
                      cm_mc_rpt_ptr->mt_dbm_data.num_bytes,
                      cm_mc_rpt_ptr->mt_dbm_data.burst_type,
                      cm_mc_rpt_ptr->mt_dbm_data.burst_source);
      }
      else if(cm_mc_rpt_ptr->mt_dbm_data.burst_source == CM_DBM_FROM_HDR)
      {
        cmdbm_mt_msg_hdr( cm_mc_rpt_ptr->mt_dbm_data.dos_payload,
                          cm_mc_rpt_ptr->mt_dbm_data.num_bytes,
                          cm_mc_rpt_ptr->mt_dbm_data.burst_type,
                          cm_mc_rpt_ptr->mt_dbm_data.burst_source,
                          cm_mc_rpt_ptr->mt_dbm_data.rlp_flow,
                          cm_mc_rpt_ptr->mt_dbm_data.route);
      }
      else
      {
        CM_ERR_1("Bad burst_source = %d",
                cm_mc_rpt_ptr->mt_dbm_data.burst_source);
      }
      break;

    #ifdef FEATURE_DEDICATED_C2K_MODE
    /* This piece of code assumes that cmss_mc_rpt_proc() is called before
    ** this function. That assumption is necessary, since
    ** ss_ptr->is_hdr_session_open needs to be FALSE when
    ** cmdbm_client_cmd_proc_next() is called.
    */
    case CM_HDR_SESSION_OPEN_FAILED:
    case CM_HDR_SESSION_CLOSED:

      cmdbm_client_cmd_proc_next();

      break;
    #endif

    default:
      break;
  }
}



/*===========================================================================

FUNCTION cmdbm_sd_rpt_proc

DESCRIPTION
  Process System determination reports specific to DBM.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbm_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{
  /*lint -e{826} keep it suppressed for general / specific walks as well */
  const cm_sd_rpt_u_type         *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD report */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sd_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -e{826} keep it suppressed for general / specific walks as well */
  switch( sd_rpt_ptr->hdr.cmd )
  {
    case CM_OK_TO_ORIG_F:

      if (curr_dbm_obj.valid
          && curr_dbm_obj.is_cmd_sent_to_MC != CMDBM_MSG_SENT)
      {
        CM_MSG_HIGH_0("OK_TO_ORIG. Sending blocked DBM now");
        if( sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode == SD_MODE_HDR )
        {
          #ifdef FEATURE_HDR_DOS

          cmdbm_forward_msg_to_hdrmc(&curr_dbm_obj.cm_dbm_cmd.info);

          #endif /* FEATURE_HDR_DOS */
        }
        else
        #ifdef FEATURE_DEDICATED_C2K_MODE
        if( !cmss_ptr()->is_hdr_session_open )
        #endif
        {
          cmdbm_forward_msg_to_mc(&curr_dbm_obj.cm_dbm_cmd.info,
                                curr_dbm_obj.cm_dbm_cmd.burst_type);
        }

      }
      break;

    default:
      break;
  }
} /* cmdbm_sd_rpt_proc() */


/*===========================================================================

FUNCTION cmdbm_l2_ack_wait_time_compare
DESCRIPTION
  Scans through the queue of DBM commands and deletes those DBM's whose
  l2_ack timer has expired.

DEPENDENCIES
  Client command queue must have already been initialized with
  cmdbm_client_dbm_q_init().

RETURN VALUE

  1 => if atleast one DBM is removed from the queue
  0 => if no DBM was removed

SIDE EFFECTS
  None

===========================================================================*/

static int cmdbm_l2_ack_wait_time_compare(void * dbm_obj, void * curr_timer_val)
/*lint -esym(715, curr_timer_val)*/
/*lint -esym(818, curr_timer_val)
** Can't be const as then q_linear_search() won't compile with
** <argument 2 to 'q_linear_search'>: implicit cast of pointer to non-equal
** pointer
*/
{
  cmdbm_cmd_queue_type *curr_scan_dbm = NULL;
  cm_dbm_info_s_type   info;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(dbm_obj           != NULL);
  CM_ASSERT(curr_timer_val    != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the buffer.
  */
  memset( &info, 0, sizeof(cm_dbm_info_s_type) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  curr_scan_dbm = (cmdbm_cmd_queue_type *) dbm_obj;

  if (curr_scan_dbm->valid &&
      curr_scan_dbm->cm_dbm_cmd.l2_ack_wait_time <= time_get_uptime_secs())
  {

    /* This DBM's timer has expired. Report back a failure.
    */
    info.burst_type = curr_scan_dbm->cm_dbm_cmd.burst_type;
    info.mo_status  = CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT;
    info.user_data  = curr_scan_dbm->cm_dbm_cmd.data_block_ptr;

    cmph_update_ddtm_status( cmph_ptr() );       /* update the DDTM satus */
    cmdbm_event(&info, CM_DBM_EVENT_MO_STATUS);


    /*  Remove this DBM object from the DBM buf queue
    **  and place it on the free queue
    */
    curr_scan_dbm->is_cmd_sent_to_MC = CMDBM_NONE;
    curr_scan_dbm->valid = FALSE;

    cmdbm_client_dbm_q_delete(curr_scan_dbm);
    cmdbm_free_buf_q_put(curr_scan_dbm);

    return 1;
  }
  return 0;
}
/*lint +esym(715, curr_timer_val) */
/*lint +esym(818, curr_timer_val) */


/*===========================================================================

FUNCTION cmdbm_timer_proc

DESCRIPTION

  This function is called at each timer event (after each second). It checks
  the DBM queue to see if any DBM's timer has expired. If it has then that
  DBM is removed from the queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmdbm_timer_proc(
    cm_timer_event_type    timer_event
        /* Indicate specific timer event.
        **
        ** Note that currently timer event is not being used
        */
)
/*lint -esym(715,timer_event) */
{
  cm_dbm_info_s_type info;
  byte counter = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the buffer.
  */
  memset( &info, 0, sizeof(cm_dbm_info_s_type) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (counter = 0; counter <
                 cmdbm_get_queue_count(cmdbm_client_dbm_q); counter++)
  {
    if ( (q_linear_search(&cmdbm_client_dbm_q,
                          cmdbm_l2_ack_wait_time_compare,
                          NULL)) == NULL)
    {
      break;
    }

  } /* for counter */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (curr_dbm_obj.valid &&
      curr_dbm_obj.cm_dbm_cmd.l2_ack_wait_time <= time_get_uptime_secs())
  {
    CM_ERR_0("DBM curr obj Timer expired");

    /* This DBM's timer has expired. Report back a failure.
     */
    info.burst_type = curr_dbm_obj.cm_dbm_cmd.burst_type;
    info.mo_status  = CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT;
    info.user_data  = curr_dbm_obj.cm_dbm_cmd.data_block_ptr;

    /*  Remove this DBM object from the DBM buf queue
     *  and place it on the free queue
    */
    rex_enter_crit_sect(cmtask_crit_sect_ptr());
    /* Serialize data access curr_dbm_obj */
    curr_dbm_obj.is_cmd_sent_to_MC = CMDBM_NONE;
    curr_dbm_obj.valid = FALSE;
    rex_leave_crit_sect(cmtask_crit_sect_ptr());

    cmph_update_ddtm_status( cmph_ptr() );        /* update the DDTM satus */
    cmdbm_event(&info, CM_DBM_EVENT_MO_STATUS);


    /*  Unforce the mode if there are no more DBM commands on the queue
    */
    if (is_pref_forced)
    {
      if (cmdbm_get_queue_count(cmdbm_client_dbm_q) <= 0)
      {
        /*  Reset the flag
        */
        is_pref_forced = FALSE;

        /*------------------------------------------
         If we forced the mode to send this message,
         unforce it now.
        *------------------------------------------*/
        CM_MSG_LOW_0("Unforce mode after DBM msg sent ...");
        #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
        cmph_mo_sms_end( (dword)0, (cm_act_id_type)&curr_dbm_obj );
        #endif
      }
    }
    cmdbm_client_cmd_proc_next();
  }
}
/*lint +esym(715,timer_event) */


/*===========================================================================

FUNCTION cmxdbm_is_mo_dbm_in_progress

DESCRIPTION
 Query the mo dbm in progress status

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmxdbm_is_mo_dbm_in_progress( void )
{
  /* If the current dbm object is not valid, then there are no pending dbms.
  ** If the current dbm object is valid and its status is none which means
  ** there is a dbm but no action has yet taken on it. So, if you started
  ** procesing it, meaning not 'none' then tell that dbm is in progress.
  */
  boolean is_dbm_valid;
  cmdbm_msg_status_type is_cmd_sent_to_MC;

  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access curr_dbm_obj */
  is_cmd_sent_to_MC = curr_dbm_obj.is_cmd_sent_to_MC;
  is_dbm_valid = curr_dbm_obj.valid;
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  if ( is_dbm_valid && is_cmd_sent_to_MC != CMDBM_NONE )
  {
    return TRUE;
  }
  return FALSE;

} /* cmxdbm_is_mo_dbm_in_progress() */

/*===========================================================================
FUNCTION cmxdbm_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmxdbm_dem_is_apps_wakeup_from_suspend_event(

    cm_dbm_event_e_type   dbm_event
        /* Call event */
)
{
  /*lint -esym(715, dbm_event)*/
  #if !defined(FEATURE_APPS_POWER_COLLAPSE)
  /* If Apps power collapse is not defined, return ture
  */
  return TRUE;
  #else

  switch ( dbm_event )
  {
    case CM_DBM_EVENT_MT_MSG:

      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      return FALSE;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif
} /* cmxdbm_dem_is_apps_wakeup_from_suspend_event */
/*lint +esym(715, dbm_event)*/

#endif /* #if ((defined(FEATURE_MMODE_CDMA_800) ||
        **      defined(FEATURE_MMODE_CDMA_1900)) && \
        **     !defined(FEATURE_MMODE_REMOVE_1X))*/
