/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E
                      Reply object related functions

GENERAL DESCRIPTION
  This module implements the methods that act on reply object.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmreply.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/12   aj/cl   SC SVLTE SO 33 suspend changes
04/11/11   am      Cleaning stubs and runnning qtf SUTE
04/05/11   xs      Unblock DS packet state command when waiting for DS confirmation
                   for LPM/PWROFF command
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
09/22/10   cl      Extend MC_MAX_REPLY to 30 seconds
08/31/09   fj      Added support LTE UE initiated call funtionality.
09/01/09   sv      Moving from clk API to time API
08/07/07   ic      Lint clean up
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
11/15/04   ic      Lint cleanup with CM_DEBUG on
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
06/21/04   ic      cmcall_is_waiting_for_reply() cleanup
05/06/04   ic      Fixed incorrect assertion in cm_reply_check()
02/06/04   ws      Initial jaguar/mainline merge.
04/28/03   AT      Lint cleanup.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/06/03   prk     Added support for new end call API
                   (FEATURE_CONC_SRVC_NEW_API).
01/21/03   ws      Updated copyright information for 2003
08/09/02   vt      Initial version.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "customer.h"
#include "cmdbg.h"
#include "cmreply.h"
#include "time_svc.h"

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cm_reply_init

DESCRIPTION
  Initialize a CM reply object.

  This function must be called on a CM reply object before it being used,
  in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_reply_init(
    cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object to be initialized */
)
{
  CM_ASSERT( reply_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  reply_ptr->cmd_ptr = NULL;
}



/*===========================================================================

FUNCTION cm_reply_set

DESCRIPTION
  Setup necessary conditions for putting a CM command on a wait
  for reply from MC.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_reply_set(
    cm_reply_s_type    *reply_ptr,
        /* Pointer to a reply object */

    cm_cmd_type      *cmd_ptr,
        /* Pointer to a CM command waiting for a reply from MC */
    cm_name_type       mc_rpt_id,
        /* Indicate the MC report we are waiting for */

    dword              reply_time
        /* Indicate an uptime upper bound for how long we should wait
        ** for the reply to come */
)
{
  CM_ASSERT( reply_ptr != NULL );
  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( INRANGE((int)mc_rpt_id,
                     (int)CM_MO_CALL_CONF,
                     (int)(CM_NUM_CM_COMMANDS-1)) );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2( "Waiting for MC replay mc_rpt_id=%d, uptime=%d",
              mc_rpt_id, time_get_uptime_secs());

  /* We better not overwrite a command currently
  ** waiting for a reply.
  */
  CM_ASSERT( reply_ptr->cmd_ptr == NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  reply_ptr->cmd_ptr      = cmd_ptr;

  reply_ptr->cm_mc_rpt    = mc_rpt_id;

  reply_ptr->uptime       = time_get_uptime_secs() + reply_time;

}



/*===========================================================================

FUNCTION cm_reply_get

DESCRIPTION
  Get a pointer to the CM command waiting for a replay from MC.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  A pointer to the CM command waiting for a replay from MC.
  If no command is waiting for reply, return NULL.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_type  *cm_reply_get(
    cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object */
)
{
  cm_cmd_type  *cmd_ptr = NULL;
      /* Point at CM command waiting for reply (if any) */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(reply_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = reply_ptr->cmd_ptr;

  /* Indicate that we are no longer waiting for a reply
  ** and return a pointer to CM command that was waiting
  ** on the reply (if any).
  */
  reply_ptr->cmd_ptr = NULL;
  return cmd_ptr;

}



/*===========================================================================

FUNCTION cm_reply_check

DESCRIPTION
  Check whether a specified mc report is the reply for which
  we are waiting.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if the given mc report matches the replay for which
  we are waiting, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_reply_check(

    const cm_reply_s_type    *reply_ptr,
        /* Pointer to a reply object */

    cm_name_type       cm_mc_rpt_id
        /* Are we waiting for this mc report */

)
{
  CM_ASSERT( reply_ptr != NULL );
  CM_ASSERT( BETWEEN((int)cm_mc_rpt_id, (int)CM_RPT_GW_BEGIN, (int)CM_NUM_CM_COMMANDS) );

  /* Make sure we either NOT waiting for a reply,
  ** or we are waiting for something legal.
  */

  CM_ASSERT( (reply_ptr->cmd_ptr == NULL) ||
             INRANGE((int)reply_ptr->cm_mc_rpt,
                     (int)CM_MO_CALL_CONF,
                     (int)(CM_NUM_CM_COMMANDS-1)) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are waiting for a reply and the mc-via-ui
  ** command matches the reply for which we are waiting,
  ** return TRUE.
  */
  if( reply_ptr->cmd_ptr != NULL )
  {
    CM_MSG_HIGH_2( "MC reply: Waiting for:%d recvd=%d",reply_ptr->cm_mc_rpt,
                   cm_mc_rpt_id );
    return( reply_ptr->cm_mc_rpt == cm_mc_rpt_id );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else, return FALSE.
  */
  return FALSE;

}


/*===========================================================================

FUNCTION cm_reply_is_waiting

DESCRIPTION
  Check whether we are currently waiting for a reply.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if we are currently waiting for a rely, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_reply_is_waiting(

    const cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object */
)
{

  CM_ASSERT( reply_ptr != NULL );

  /* Make sure we either NOT waiting for a reply,
  ** or we are waiting for something legal.
  */
  CM_ASSERT( reply_ptr->cmd_ptr == NULL ||
             INRANGE((int)reply_ptr->cm_mc_rpt,
                     (int)CM_MO_CALL_CONF,
                     (int)(CM_NUM_CM_COMMANDS-1)));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return boolean indicating whether we are
  ** currently waiting on a reply.
  */
  return( reply_ptr->cmd_ptr != NULL );

}



/*===========================================================================

FUNCTION cm_reply_too_long

DESCRIPTION
  Check whether are waiting for too long on a reply

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if are waiting for too long on a reply, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_reply_too_long(

    const cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object */
)
{

  CM_ASSERT( reply_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are waiting for a reply command
  ** and we are waiting for too long, return TRUE.
  */
  if( reply_ptr->cmd_ptr != NULL &&
      time_get_uptime_secs() > reply_ptr->uptime )
  {
    CM_MSG_HIGH_3( "MC reply: Wait time expired cm_mc_rpt=%d, curr_uptime=%d, wait_until=%d",
                   reply_ptr->cm_mc_rpt,
                   time_get_uptime_secs(),
                   reply_ptr->uptime );

    return TRUE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, return FALSE.
  */
  return FALSE;

}



/*===========================================================================

FUNCTION cmcall_is_waiting_for_reply

DESCRIPTION
  Check whether the call object is currently waiting for a reply.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call object is currently waiting for a rely, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_waiting_for_reply
(
    cm_call_id_type call_id
        /* Call id */
)
{
  cmcall_s_type *call_ptr = cmcall_ptr( call_id );
  return( ( call_ptr == NULL ) ? FALSE : cm_reply_is_waiting( &call_ptr->reply ) );

} /* cmcall_is_waiting_for_reply() */



/*===========================================================================

FUNCTION cm_reply_should_cmd_wait

DESCRIPTION
  This function determines if a new command can be processed or is it waiting
  for a reply from MC.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if current system is waiting for response from MC. False otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_reply_should_cmd_wait
(
    const cm_cmd_type    *cmd_ptr
        /* Pointer to a command object */
)
{
  int                             i;

  boolean                         cmd_wait;
  cmph_s_type                     *ph_ptr      = cmph_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( cmd_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (ph_ptr->is_hold_user_act)
  {
    CM_MSG_HIGH_1(" HOLD USER ACTIVITY status %d", ph_ptr->is_hold_user_act);
    return TRUE;
  }

  cmd_wait = FALSE;
  if (CMD_TYPE(cmd_ptr)== CM_CMD_TYPE_PH ) 
  {
    if( cmph_is_waiting_for_reply() )
    {
      cmd_wait = TRUE;
    }

    /* unblock ph command CM_PH_CMD_PACKET_STATE when waiting 
    ** for DS confirmation
    */
    if( PH_CMD_PTR(cmd_ptr)->cmd == CM_PH_CMD_PACKET_STATE &&
       cm_reply_check(&ph_ptr->reply, CM_DATA_PACKET_STATE_RPT))
    { 
      cmd_wait = FALSE;
    }
    
  } /* if ( cmd_ptr->cmd_type == CM_CMD_TYPE_PH ) */
  else if (CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL   &&
            CALL_CMD_PTR(cmd_ptr)->info.call_id != CM_CALL_ID_INVALID)
  {
    if( CALL_CMD_PTR(cmd_ptr)->cmd != CM_CALL_CMD_END )
    {
      if( (cmcall_ptr( CALL_CMD_PTR(cmd_ptr)->info.call_id ) != NULL) &&
        cmcall_is_waiting_for_reply( CALL_CMD_PTR(cmd_ptr)->info.call_id ) )
      {
        cmd_wait = TRUE;
      }
    } /* if ( cmd_ptr->cmd.call.cmd != CM_CALL_CMD_END ) */
    else
    {

      for( i = 0; i < CALL_CMD_PTR(cmd_ptr)->info.num_end_call_ids; i++)
      {
        if( (cmcall_ptr( CALL_CMD_PTR(cmd_ptr)->info.end_params[i].call_id ) != NULL) &&
          cmcall_is_waiting_for_reply( CALL_CMD_PTR(cmd_ptr)->info.end_params[i].call_id ) )
        {
          cmd_wait = TRUE;
          break;
        }
      } /* for( i = 0; i < cmd_info_ptr->num_end_call_ids; i++) */
    } /* if( call_cmd_ptr->cmd != CM_CALL_CMD_END ) */

  } /* else if ( cmd_ptr->cmd_type == CM_CMD_TYPE_CALL ) */

  return cmd_wait;
} /* cm_reply_should_cmd_wait() */


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#ifdef FEATURE_OTASP

/*===========================================================================

FUNCTION cm_commit_reply_set

DESCRIPTION
  Setup necessary conditions to wait for MC's OTASP commit status command.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cm_commit_reply_set(

    cm_reply_s_type    *reply_ptr,
        /* Pointer to a reply object */

    cm_cmd_type      *cmd_ptr,
        /* Pointer to a CM command waiting for a reply from MC */

    cm_name_type       mc_rpt_id,
        /* Indicate the MC report we are waiting for */

    dword              reply_time
        /* Indicate an uptime upper bound for how long we should wait
        ** for the reply to come */
)
{

  CM_ASSERT( reply_ptr != NULL );
  CM_ASSERT( INRANGE((int)mc_rpt_id,
                     (int)CM_MO_CALL_CONF,
                     (int)(CM_NUM_CM_COMMANDS-1)) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  reply_ptr->cmd_ptr      = cmd_ptr;
  reply_ptr->cm_mc_rpt    = mc_rpt_id;
  reply_ptr->uptime       = time_get_uptime_secs() + reply_time;

  CM_MSG_HIGH_3( "MC reply: Set mc_rpt_id=%d, curr_uptime=%d, wait_until=%d",
                 mc_rpt_id,
                 time_get_uptime_secs(),
                 reply_ptr->uptime );

}



/*===========================================================================

FUNCTION cm_commit_reply_check

DESCRIPTION
  Check whether a specified mc report is the reply for which
  we are waiting: OTASP commit status command.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if the given mc report matches the replay for which
  we are waiting, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_commit_reply_check(

    const cm_reply_s_type    *reply_ptr,
        /* Pointer to a reply object */

    cm_name_type       cm_mc_rpt_id
        /* Are we waiting for this mc report */
)
{

  CM_ASSERT( reply_ptr != NULL );
  CM_ASSERT( INRANGE((int)cm_mc_rpt_id,
                     (int)CM_MO_CALL_CONF,
                     (int)(CM_NUM_CM_COMMANDS-1)) );

  /* Make sure we either NOT waiting for a reply,
  ** or we are waiting for something legal.
  */
  CM_ASSERT( INRANGE((int)reply_ptr->cm_mc_rpt,
                     (int)CM_MO_CALL_CONF,
                     (int)(CM_NUM_CM_COMMANDS-1)) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are waiting for a reply and the mc-via-ui
  ** command matches the reply for which we are waiting,
  ** return TRUE.
  */
  if( reply_ptr->cm_mc_rpt  == cm_mc_rpt_id )
  {
    CM_MSG_HIGH_1( "Got MC reply cm_mc_rpt=%d", cm_mc_rpt_id);
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, return FALSE.
  */
  return FALSE;

}



/*===========================================================================

FUNCTION cm_commit_reply_too_long

DESCRIPTION
  Check whether are waiting for too long on a reply

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if are waiting for too long on a reply, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_commit_reply_too_long(

    const cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object */
)
{

  CM_ASSERT( reply_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are waiting for a reply command
  ** and we are waiting for too long, return TRUE.
  */
  if( time_get_uptime_secs() > reply_ptr->uptime )
  {
    CM_MSG_HIGH_1( "Waiting for too long on MC reply cm_mc_rpt=%d",
                   reply_ptr->cm_mc_rpt );

    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, return FALSE.
  */
  return FALSE;

}
#endif /* FEATURE_OTASP */
#endif /* FEATURE_MMODE_CDMA_800 | FEATURE_MMODE_CDMA_1900 */
