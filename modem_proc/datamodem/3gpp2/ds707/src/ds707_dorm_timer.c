/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S
                  D O R M A N T  T I M E R  C O N T R O L
                       
GENERAL DESCRIPTION
  This file contains functions used to handle the dormant timer.  This 
  functionality is also known as the holddown timer.  It prevents a pkt call
  from re-originiating out of dormancy for a time specified by the base
  station.

EXTERNALIZED FUNCTIONS
  DS707_DORM_TIMER_PARSE_SOCM
    Called when a service-option control message is received, requiring the
    dormant timer value to be changed.

  DS707_DORM_TIMER_ENABLE_
    Called to enable/disable dormant timer functionality.
    
  DS707_DORM_TIMER_PROCESS_CMD
    Processes commands specific to the dormant timer.    

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_dorm_timer_init() at power-up.

   Copyright (c) 2002 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_dorm_timer.c_v   1.3   15 Nov 2002 21:36:08   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_dorm_timer.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/21/15   sc      Reset the dorm_timer_running flag after HOLD DOWN Timer expiry
11/02/11   msh     Support changes to post SOCM PARSE cmd to DS task 
08/30/11   msh     Handle case when SOCM is received while dorm timer is running 
03/15/11   mg      Global variable cleanup
10/14/10   sa      Adding debug messages before ASSERT.
10/13/10   op      Migrated to MSG 2.0 macros
09/13/10   op      Changed DS707_CBACK_MSG macro to use MSG 2.0 format
03/13/08   psng    Fixed off target medium and low lint errors.
08/13/05   ms      Removed the code that updates Dorm timer val 
                   at wrong places
03/08/05   vrk/gr  Holddown timer 50ms issue removed
04/24/04   ak      Lint fixes.
04/09/04   ak      Make event callback take a const.
02/04/04   ak      Updated to use pkt mgr event mechanisms.
09/10/03   ak      Fix for JCDMA dorm timer value.
11/15/02   ak      Updated file header comments.
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_1X

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "err.h"
#include "ds3gtimer.h"
#include "ds707.h"
#include "ds707_cback.h"
#include "ds707_pkt_mgr.h"
#include "ds707_dorm_timer.h"
#include "dstask.h"
#include "dstaski.h"
#include "event.h"
#include "msg.h"
#include "data_msg.h"
#include "snm.h"
#include "ds707_roaming.h"
#include "data_err_suppr.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
  Mask to get the DORM_CNTL bits.
---------------------------------------------------------------------------*/
#define DORM_TIMERI_CNTL_MASK           0xE0

/*---------------------------------------------------------------------------
  Values of the DORM_CNTL field in the Packet Data Dormant Timer Control
  service option control message.
---------------------------------------------------------------------------*/
#define DORM_TIMERI_CNTL_DISABLE        0x0
#define DORM_TIMERI_CNTL_SET_TIMER      0x20

/*--------------------------------------------------------------------------
  Default Dormant Timer value (in milliseconds).
---------------------------------------------------------------------------*/
#define DORM_TIMERI_TIMER_DEFAULT     0
/*--------------------------------------------------------------------------
  DEFAULT_DORM_TIME:
  JCDMA requirement: Change default timer to 5secs. Note, this is in 
  violation of IS-707.
---------------------------------------------------------------------------*/
#define DORM_TIMERI_JCDMA_TIMER_DEFAULT     5000

/*---------------------------------------------------------------------------
  Packet Data Dormant Timer value (in milliseconds).
---------------------------------------------------------------------------*/
LOCAL word dorm_timeri_val = DORM_TIMERI_TIMER_DEFAULT;

/*---------------------------------------------------------------------------
  Enables/disables the holddown timer
---------------------------------------------------------------------------*/
LOCAL boolean dorm_timeri_enabled = TRUE;

/*---------------------------------------------------------------------------
  State of the timer
---------------------------------------------------------------------------*/
LOCAL boolean dorm_timeri_is_running = FALSE;

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DORM_TIMERI_START

DESCRIPTION   Starts the dormant timer.

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timeri_start
(
  boolean               use_hdown_timer,
  sys_sys_mode_e_type   curr_network
)
{
  ds707_cback_event_info_type   event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dorm_timeri_is_running == FALSE)
  {
    if ((dorm_timeri_val > 0)           && 
        (dorm_timeri_enabled == TRUE)   &&
        (use_hdown_timer == TRUE)       &&
        (curr_network    == SYS_SYS_MODE_CDMA)
       )
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Start holddown timer %d msecs",dorm_timeri_val);
      ds3gi_start_timer(DS3G_TIMER_HOLDDOWN, dorm_timeri_val);

      dorm_timeri_is_running = TRUE;

      ds707_cback_post_event(  
                              DS707_CBACK_HOLDDOWN_TIMER_STARTED_EV,
                              &event_info
                           );
    }
  }
} /* ds707_dorm_timeri_start() */

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_STOP

DESCRIPTION   Stops the dormant timer.

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timeri_stop(void)
{
  ds707_cback_event_info_type   event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                  "Handle stop holddown timer, dorm_timeri_is_running %d", 
                  dorm_timeri_is_running);

  if (dorm_timeri_is_running == TRUE)
  {
    dorm_timeri_is_running = FALSE;
    ds3gi_stop_timer(DS3G_TIMER_HOLDDOWN);

    ds707_cback_post_event(DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV,
                           &event_info);
  }
} /* ds707_dorm_timeri_stop() */

/*===========================================================================
FUNCTION      DS707_DORM_TIMERI_RESET_VAL

DESCRIPTION   This function resets the dormant timer to it's default value.
              If a rex timer is running, it is not affected.

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timeri_reset_val(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dorm_timeri_val = (ds707_roaming_is_curr_mode_jcdma() == TRUE)? 
                     DORM_TIMERI_JCDMA_TIMER_DEFAULT:
                     DORM_TIMERI_TIMER_DEFAULT;
} /* ds707_dorm_timeri_reset_val() */

/*===========================================================================
FUNCTION       DS707_DORM_TIMERI_EVENT_CBACK

DESCRIPTION    Callback when various phone events occur.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_dorm_timeri_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                  "dorm timer event cback: event %d, nest_level %d", 
                  event, ds707_cback_get_nest_level());

  switch(event)
  {
    case DS707_CBACK_SID_CHANGED_EV:
      ds707_dorm_timeri_reset_val();
      ds707_dorm_timeri_stop();
      break;

    case DS707_CBACK_PHYS_LINK_UP_EV:
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "phy link UP - enable & stop hdown timer");

      ds707_dorm_timer_enable(TRUE);
      ds707_dorm_timeri_stop();
      break;

    case DS707_CBACK_PHYS_LINK_DOWN_EV:
      if (event_info_ptr->going_dormant == TRUE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "phy link DOWN dorm - start hdown timer");

        /*lint -save -e730
         *             lint doesn't like the !(var) below
         */
        ds707_dorm_timeri_start(
                                 !(event_info_ptr->failed_orig),
                                 event_info_ptr->curr_network
                               );
        /*lint -restore */
      }
      else /* going NULL */
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "phy link DOWN null - stop, enable & reset hdown timer");
        
        ds707_dorm_timeri_stop();
        ds707_dorm_timer_enable(TRUE);
        ds707_dorm_timeri_reset_val();
      }
      break;

    case DS707_CBACK_ABORT_SESSION_EV:
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "phy link ABORT - stop, enable & reset hdown timer");
      
      ds707_dorm_timeri_stop();
      ds707_dorm_timer_enable(TRUE);
      ds707_dorm_timeri_reset_val();
      break;

    case DS707_CBACK_NEW_NET_EV:
      if(ds707_pkt_is_handoff_from_1x_to_hdr(
         event_info_ptr->curr_network, event_info_ptr->new_network))
      {      
        ds707_dorm_timeri_stop();
      }      
      break;

    default:
      DS_FORCE_ASSERT();
      break;
  }

} /* ds707_dorm_timeri_event_cback() */

/*===========================================================================
FUNCTION      DS707_DORM_TIMERI_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a SOCM
              which contains dormant timer info.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timeri_parse_socm
(
  ds707_dorm_timer_socm_msg_type  *dorm_socm_msg   /* SOCM with Dorm Timer info  */
)
{
  if(NULL == dorm_socm_msg)
  {
    DATA_ERR_FATAL("dorm_socm_msg is NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    Service option control message for Packet Data Dormant Timer Control.
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG2(MSG_LEGACY_MED, "Rx'ed Dormant Timer SO Ctl Msg %d. "
                  "dorm_timeri_is_running %d",
                  (dorm_socm_msg->cntl & DORM_TIMERI_CNTL_MASK),
                  dorm_timeri_is_running);

  if( (dorm_socm_msg->cntl & DORM_TIMERI_CNTL_MASK) == 
                                                   DORM_TIMERI_CNTL_DISABLE )
  {
    /*-----------------------------------------------------------------------
      Disable BS/MSC control of the dormant timer i.e. reset the timer to
      its default value.
    -----------------------------------------------------------------------*/
    dorm_timeri_val = (ds707_roaming_is_curr_mode_jcdma() == TRUE)? DORM_TIMERI_JCDMA_TIMER_DEFAULT:
                                       DORM_TIMERI_TIMER_DEFAULT;
  }
  else if( (dorm_socm_msg->cntl & DORM_TIMERI_CNTL_MASK) == 
                                                 DORM_TIMERI_CNTL_SET_TIMER )
  {
    if( dorm_socm_msg->dorm_time == 0 )
    {
      /*---------------------------------------------------------------------
       The dormant timer value is set to zero => disable BS/MSC control of
       the dormant timer i.e. reset the timer to its default value.
      ---------------------------------------------------------------------*/
      dorm_timeri_val = (ds707_roaming_is_curr_mode_jcdma() == TRUE)? DORM_TIMERI_JCDMA_TIMER_DEFAULT:
                                         DORM_TIMERI_TIMER_DEFAULT;
    }
    else
    {

      if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
      {
        dorm_timeri_val = (word)(dorm_socm_msg->dorm_time * 100);
      }
      else
      {
        /*---------------------------------------------------------------------
        Set the dormant timer value to the greater of the current value, or
        the value specified in the service option control message.
        --------------------------------------------------------------------*/
        dorm_timeri_val = MAX(dorm_timeri_val, 
                              (word)(dorm_socm_msg->dorm_time * 100));
      }
    }
    if (dorm_timeri_is_running == TRUE)
    {
      ds707_dorm_timeri_stop();
      ds707_dorm_timeri_start(TRUE, SYS_SYS_MODE_CDMA);
    }
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED,"Dorm Timer value :%d", dorm_timeri_val);
} /* ds707_dorm_timeri_parse_socm() */

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_ENABLE

DESCRIPTION   Enable or disable dormancy timer

DEPENDENCIES  If multiple calls of this function are made, principle used
              is last caller wins.

              Input TRUE  - use the holddown timer when going dormant.
                    FALSE - do not use the holddown timer when dormant.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timer_enable
(
  boolean is_enabled
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dorm_timeri_enabled = is_enabled;
} /*  ds707_dorm_timer_enable() */

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_INIT

DESCRIPTION   Called at startup

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timer_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_cback_register(
                            DS707_CBACK_SID_CHANGED_EV,
                            ds707_dorm_timeri_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg SID_CHNG_EV cback",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_UP_EV,
                            ds707_dorm_timeri_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_UP_EV cback",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_DOWN_EV,
                            ds707_dorm_timeri_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_DOWN_EV cback",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_ABORT_SESSION_EV,
                            ds707_dorm_timeri_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg ABORT_SESSION_EV cback",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_NEW_NET_EV,
                            ds707_dorm_timeri_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg NEW_NET_EV cback",0,0,0);
  }

} /* ds707_dorm_timer_init() */

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_PROCESS_CMD

DESCRIPTION   Processes the dormant timer commands.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timer_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  ds707_cback_event_info_type   event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL == ds_cmd_ptr)
  {
    DATA_ERR_FATAL("ds_cmd_ptr is NULL");
    return;
  }

  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_PKT_HOLDDOWN_TIMER_EXPIRED:
      if (dorm_timeri_val > 0)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Holddown timer expired");
        event_report(EVENT_DORMANT_TIMER_EXPIRED);
      }
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "reset holddown timer running flag");
      dorm_timeri_is_running = FALSE;

      ds707_cback_post_event(DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV, 
                             &event_info);
      break;

    case DS_CMD_707_PARSE_DORM_TIMER_SOCM:
      ds707_dorm_timeri_parse_socm(
           (ds707_dorm_timer_socm_msg_type *)(ds_cmd_ptr->cmd_payload_ptr));
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown cmd %d", 
                      ds_cmd_ptr->hdr.cmd_id);
      break;
  }
} /* ds707_dorm_timer_process_cmd() */


/*===========================================================================
FUNCTION      DS707_DORM_TIMER_PARSE_SOCM

DESCRIPTION   This function does a quick error check on service option 
              control message, and then posts this message to DS, so
              it's processed in DS task.

DEPENDENCIES  None

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_dorm_timer_parse_socm
(
  ds707_dorm_timer_socm_msg_type  *dorm_socm_msg /* SOCM with DORM TIMER info */
)
{
  ds_cmd_type                     *cmd_ptr       = NULL;         
  ds707_dorm_timer_socm_msg_type  *dorm_socm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == dorm_socm_msg )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DORM_TIMER SOCM - invalid socm msg ptr");
    return(SNM_UNSPEC);
  }
  else if(((dorm_socm_msg->cntl & DORM_TIMERI_CNTL_MASK) == DORM_TIMERI_CNTL_DISABLE)    ||
     ((dorm_socm_msg->cntl & DORM_TIMERI_CNTL_MASK) == DORM_TIMERI_CNTL_SET_TIMER) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "DORM_TIMER SOCM - post msg to DS");

    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_epzid_type));
    if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
    {
      ASSERT(0);
      return(SNM_INVALID_MSG_TYP);
    }

    else
    {
      cmd_ptr->hdr.cmd_id = DS_CMD_707_PARSE_DORM_TIMER_SOCM;
      dorm_socm_ptr 
                = (ds707_dorm_timer_socm_msg_type *)cmd_ptr->cmd_payload_ptr;

      dorm_socm_ptr->cntl     = dorm_socm_msg->cntl;
      dorm_socm_ptr->dorm_time = dorm_socm_msg->dorm_time;
      ds_put_cmd( cmd_ptr );
    }
    return(SNM_VALID);
  }
  else
  {
    return(SNM_INVALID_MSG_TYP);
  }
} /* ds707_dorm_timer_parse_socm() */

#endif /* FEATURE_DATA_IS707 */

