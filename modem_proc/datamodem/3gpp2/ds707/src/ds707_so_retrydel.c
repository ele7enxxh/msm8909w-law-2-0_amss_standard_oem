/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       S O  R E T R Y  D E L A Y  

GENERAL DESCRIPTION
  This file contains functions that are used to implement the retry delay
  processing that is part of IS2000.  Retry delays apply to all pkt 
  service options.
  
EXTERNALIZED FUNCTIONS
  DS707_SO_RETRYDEL_INIT
    Called at startup.  Initializes delay to zero (i.e., not
    delayed).
    
  DS707_SO_RETRYDEL_SET_DELAY
    Sets the delay for packet calls.  The delay is stored as the time in
    the future at which the phone is allowed to re-orig with a packet
    data calls.
    
  DS707_SO_RETRYDEL_CLR_DELAY
    Clears the retry delay for packet data calls.  The delay is set back
    to zero.
    
  DS707_SO_RETRYDEL_CLR_ALL_DELAYS
    Clears retry delay for packet data calls.
    
  DS707_SO_RETRYDEL_SO_IS_DELAYED
    Returns TRUE or FALSE, depending on if packet data calls are 
    delayed.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_so_retrydel_init() at startup.

 Copyright (c) 2001 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_retrydel.c_v   1.2   21 Jan 2003 22:24:42   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_so_retrydel.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
03/09/09   sa         AU level CMI modifications.
03/07/07   sq/yy      In ds707_so_retrydel_is_delayed function, if retrydel still
                      active, send DS_707_RETRY_DELAY_ORIG_CMD to DS, which will 
                      disable iface flow and start retry delay timer.
01/10/07   sq         Send DS_707_RETRY_DELAY_ORIG_CMD to DS when get 
                      retry delay from BS
04/11/03   ak         Updated with better msg when receiving retry delay.
01/13/03   ak         Updated to now have retry delay apply to all service
                      options, instead of being per service option.
11/17/02   ak         Updated file header comments.
03/30/01   ak         Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "time_svc.h"
#include "time_tod.h"
#include "msg.h"
#include "data_msg.h"
#include "qw.h"
#include "dstask_v.h"
#include "ds707_so_retrydel.h"


/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
LOCAL qword ds_retry_exp_time;       /* when retry delay ends (absolute)   */

/*===========================================================================
                   INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_INIT
 
DESCRIPTION   Initializes the retry delay.  Called only at powerup.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qw_set(ds_retry_exp_time, 0, 0);
} /* ds707_so_retrydel_init() */

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_SET_DELAY
 
DESCRIPTION   Sets the delay for all pkt data calls.  If the delay is
              infinite, then the expiration_time is set to all F's (hex).

DEPENDENCIES  None.  

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_set_delay
(
  boolean        delay_infinite,            /* is the delay infinite       */
  uint32         delay                      /* the delay in msec from now  */
)
{
  qword    curr_time = {0,0};               /* curr time since epoch       */
  qword    ep_delay  = {0,0};               /* delay in msecs from epoch   */
  ds_cmd_type *cmd_ptr;                     /* DS Task cmd buf             */
  ds707_retry_delay_orig_type  *retry_delay_orig_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the delay is infinite, then set the expiration time to the max
    possible.  Else add the delay to the current time, and store it away
  -------------------------------------------------------------------------*/
  if (delay_infinite == TRUE)
  {
    qw_set(ds_retry_exp_time,0xFFFF,0xFFFF);
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Pkt SOs retry delay is infinite");
  }
  else
  {
    time_get_ms(curr_time);
    qw_set(ep_delay,0,delay);
    qw_add(ds_retry_exp_time, curr_time, ep_delay);
    DATA_IS707_MSG3(MSG_LEGACY_MED, "Pkt SOs retry delay is for %d msec. "
                    "Pkt delayed till %d curr %d", 
                    delay, ds_retry_exp_time[0], curr_time[0]);
  }

  /* post retry delay comd to DS task which will disable iface flow */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_retry_delay_orig_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_707_RETRY_DELAY_ORIG;

  retry_delay_orig_ptr 
                  = (ds707_retry_delay_orig_type *)cmd_ptr->cmd_payload_ptr;
  retry_delay_orig_ptr->delay_infinite = delay_infinite;
  retry_delay_orig_ptr->delay = delay;

  ds_put_cmd( cmd_ptr );
} /* ds707_so_retrydel_set_delay() */

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_CLR_DELAY
 
DESCRIPTION   Clears the retry delay for pkt data so's.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_clr_delay(void)
{
  ds_cmd_type                  *cmd_ptr              = NULL;
  ds707_retry_delay_orig_type  *retry_delay_orig_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qw_set(ds_retry_exp_time, 0, 0);
  DATA_IS707_MSG0(MSG_LEGACY_MED, "Pkt Data Retry delay cleared");

  /* post retry delay comd to DS task which will enable iface flow */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_retry_delay_orig_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_707_RETRY_DELAY_ORIG;

  retry_delay_orig_ptr 
                  = (ds707_retry_delay_orig_type *)cmd_ptr->cmd_payload_ptr;
  retry_delay_orig_ptr->delay_infinite = FALSE;
  retry_delay_orig_ptr->delay = 0; /*delay =0 means cancel delay*/

  ds_put_cmd( cmd_ptr );
} /* ds707_so_retrydel_clr_all_delays() */

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_IS_DELAYED
 
DESCRIPTION   Returns TRUE or FALSE, indicating if packet data calls are 
              being blocked/delayed by the base station.

DEPENDENCIES  None.

RETURN VALUE  TRUE = cannot originate pkt data right now, since the base
                     station has delayed it.
              FALSE= can originate pkt data now.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_retrydel_is_delayed(void)
{
  qword    curr_time = {0,0};               /* curr time since epoch       */
  qword	ds_retry_diff_time = {0,0}; 
  ds_cmd_type *cmd_ptr;                     /* DS Task cmd buf             */
  ds707_retry_delay_orig_type  *retry_delay_orig_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  time_get_ms(curr_time);
  if (qw_cmp(ds_retry_exp_time,curr_time) > 0)
  {
    DATA_IS707_MSG3(MSG_LEGACY_MED, 
                    "Pkt Data is delayed. delay %d, curr %d, left %d ms",
                    ds_retry_exp_time[0], curr_time[0], ds_retry_diff_time[0]);

    /* post retry delay comd to DS task which will disable iface flow */
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_retry_delay_orig_type));
    if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
    {
      ASSERT(0);
      return FALSE;
    }

    retry_delay_orig_ptr 
                    = (ds707_retry_delay_orig_type *)cmd_ptr->cmd_payload_ptr;

    if ((ds_retry_exp_time[0] == 0xFFFF) && (ds_retry_exp_time[1] == 0xFFFF))	
    {	
      retry_delay_orig_ptr->delay_infinite = TRUE;
    }
    else
    {
      retry_delay_orig_ptr->delay_infinite = FALSE;
      qw_sub(ds_retry_diff_time, ds_retry_exp_time, curr_time);
      retry_delay_orig_ptr->delay = ds_retry_diff_time[0];
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_RETRY_DELAY_ORIG;
    ds_put_cmd( cmd_ptr );
  
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_so_retrydel_is_delayed() */

#endif /* FEATURE_DATA_IS707 */

