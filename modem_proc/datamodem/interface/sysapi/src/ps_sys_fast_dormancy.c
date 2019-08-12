
/******************************************************************************
  @file    ps_sys_fast_dormancy.c
  @brief   

  DESCRIPTION
  This file defines functions, structs used for fast dormancy feature 
  implementation. Also Mode handlers will communicate start, stop Fast 
  Dormancy to PS through sys conf API's and they will be handled here.
  Also takes call on releasing the signalling connection

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ps_sys_fast_dormancy.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
 
  
when          who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/14   tyk       Created module
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/


#include "ps_sys_fast_dormancy.h"
#include "ps_sys_eventi.h"
#include "ps_ifacei.h"
#include "ds_Utils_DebugMsg.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           LOCAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* global structure to hold fast dormancy related information */

static ps_sys_fast_dormancy_info_s fast_dormancy_info;

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/



/*===========================================================================

                                      FUNCTIONS

===========================================================================*/


/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_START_TIMER()

DESCRIPTION
  This function starts/restarts Fast Dormancy Timer with specific timer value

PARAMETERS
timer_type:  which timer has to be started

RETURN VALUE
0 -  Success
-1 - Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int ps_sys_fast_dormancy_start_timer
(
   ps_sys_fast_dormancy_enum_timer_type    timer_type
)
{
   int     ret_val = -1;
  /* ----------------------------------------------------- */
  if (timer_type > PS_SYS_FAST_DORMANCY_TIMER_MAX)
  {
    LOG_MSG_INVALID_INPUT_1("ps_sys_fast_dormancy_start_timer():Invalid args "
                                                   "timer_num %d", timer_type);
    return -1;
  }

  if(fast_dormancy_info.timer_info[timer_type].timer_value > 0)
  { 
    if((ps_timer_start((fast_dormancy_info.timer_info[timer_type].timer_hdl), 
                (fast_dormancy_info.timer_info[timer_type].timer_value))) != 0)
    {
      ret_val = 0;
    }
  }
  LOG_MSG_INFO1_3("ps_sys_fast_dormancy_start_timer():Timer_type: %d,"
                   "timer_value: %d, ret_val: %d", timer_type, 
                   (fast_dormancy_info.timer_info[timer_type].timer_value),
                   ret_val);
  return ret_val; 
}/* ps_sys_fast_dormancy_start_timer */


/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_CLR_TIMER()

DESCRIPTION
  This function clears Fast Dormancy Timer 

PARAMETERS
Timer_type :  Timer on which operation has to be performed


 RETURN VALUE
 None

 DEPENDENCIES
 None
  
SIDE EFFECTS
None
===========================================================================*/
static void ps_sys_fast_dormancy_clr_timer
(
   ps_sys_fast_dormancy_enum_timer_type    timer_type
)
{
  /*--------------------------------------------------------------- */
  if (timer_type > PS_SYS_FAST_DORMANCY_TIMER_MAX)
  {
    LOG_MSG_INVALID_INPUT_1("ps_sys_fast_dormancy_clr_timer():"
                             "Invalid args, timer_num %d", timer_type);
    return;
  }

  /*cancelling the Timer  */
  if( (ps_timer_cancel(fast_dormancy_info.timer_info[timer_type].timer_hdl)
                                                                       != 0))
  {
    LOG_MSG_INFO1_1("ps_sys_fast_dormancy_clr_timer(): Success, timer: %d,",
                                                                  timer_type);
  }  
  return;
}/* ps_sys_fast_dormancy_clr_timer */

/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_IS_TIMER_RUNNING()

DESCRIPTION
  This function gives information on whether timer is running or not currently

PARAMETERS
None

RETURN VALUE
TRUE -  If running
FALSE - if Not

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 ps_sys_fast_dormancy_is_timer_running
(
  ps_sys_fast_dormancy_enum_timer_type    timer_type
)
{
  uint8             ret_val = FALSE;
  /*------------------------------------------------------------ */
  if (timer_type > PS_SYS_FAST_DORMANCY_TIMER_MAX)
  {
    LOG_MSG_INVALID_INPUT_1("ps_sys_fast_dormancy_is_timer_running():"
                                        "Invalid args, timer_num %d", timer_type);
    return ret_val;
  }
  
  ret_val = ps_timer_is_running(
                             fast_dormancy_info.timer_info[timer_type].timer_hdl);

  LOG_MSG_INFO1_2("ps_sys_fast_dormancy_is_timer_running(): timer: %d,"
                                                "ret_val: %d",timer_type, ret_val);
  return ret_val;
}/* ps_sys_fast_dormancy_is_timer_running */

/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_START()

DESCRIPTION
  This function processes start fast dormancy command and starts timer_1. 
  It updates statistics if Timer_1 is already running  else (first trigger)
  Initializes IFACE Statistics before starting Timer

PARAMETERS
  event_info_p:contains timer values 

RETURN VALUE
0 -  Success
-1 - Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_sys_fast_dormancy_start
(
  void* event_info_p
)
{ 
  ps_sys_fast_dormancy_timer_values_s       *timer_values_p = NULL;
  /* -------------------------------------------------------------------------*/
  if (NULL == event_info_p )
  {
    LOG_MSG_INVALID_INPUT_1("ps_sys_fast_dormancy_start(): Invalid args: "
                                             "event_info_p 0x%p", event_info_p);
    return -1;
  }
  
  timer_values_p = (ps_sys_fast_dormancy_timer_values_s*)event_info_p;

  LOG_MSG_INFO1_2("ps_sys_fast_dormancy_start():timer_1 value: %d, timer_2 value: %d", 
                         timer_values_p->timer_value_1, timer_values_p->timer_value_2);

  /* cache the Timer values passed in event_info ptr */
  fast_dormancy_info.timer_info[PS_SYS_FAST_DORMANCY_TIMER_1].timer_value = 
                                                timer_values_p->timer_value_1;
  fast_dormancy_info.timer_info[PS_SYS_FAST_DORMANCY_TIMER_2].timer_value = 
                                                timer_values_p->timer_value_2;

  /*Trigger to start FD can be received even when FD Timers are already running.
   If overall timer is already running update the already existing iface stats.
   and clear agressive timer which will be started when dropped indication is received*/
  if(ps_sys_fast_dormancy_is_timer_running(PS_SYS_FAST_DORMANCY_TIMER_1)
                                                                     == TRUE)
  {
    if(0 != ps_iface_fast_dormancy_update_3gpp_iface_stats())
    {
      LOG_MSG_INFO1_0("ps_sys_fast_dormancy_start_cmd_hdlr():" 
                                                 "update stats failed");
      ps_sys_fast_dormancy_stop(); 
      return -1;
    }
  }
  else
  {
    /* Define/Allocate ps timers for Fast dormancy */
    /* check if timers are already allocated. Allocate if timer handles are not valid */
    if((fast_dormancy_info.timer_info[PS_SYS_FAST_DORMANCY_TIMER_1].timer_hdl 
                                                 == PS_TIMER_INVALID_HANDLE)&&
      (fast_dormancy_info.timer_info[PS_SYS_FAST_DORMANCY_TIMER_1].timer_value
                                                                          > 0))
    {
       fast_dormancy_info.timer_info[PS_SYS_FAST_DORMANCY_TIMER_1].timer_hdl = 
                           ps_timer_alloc( ps_sys_fast_dormancy_timer_expiry_cb, 
                            (void *)PS_SYS_FAST_DORMANCY_TIMER_1);
    }
     /* for timer_2 */
    if((fast_dormancy_info.timer_info[PS_SYS_FAST_DORMANCY_TIMER_2].timer_hdl 
                                                 == PS_TIMER_INVALID_HANDLE)&&
      (fast_dormancy_info.timer_info[PS_SYS_FAST_DORMANCY_TIMER_2].timer_value
                                                                          > 0))
    {
      fast_dormancy_info.timer_info[PS_SYS_FAST_DORMANCY_TIMER_2].timer_hdl = 
                          ps_timer_alloc( ps_sys_fast_dormancy_timer_expiry_cb, 
                         (void *)PS_SYS_FAST_DORMANCY_TIMER_2);
    }

    /* It is First Trigger to start FD so Init IFACE Stats */
    if (0 != ps_iface_fast_dormancy_init_iface_stats(UMTS_IFACE))
    {
      LOG_MSG_INFO1_0("ps_sys_fast_dormancy_start_cmd_hdlr():" 
                                                    "Stats tbl Init Failed");
      /*  freeing the allocated memory if any as Initializing the Tbl failed */
      ps_iface_fast_dormancy_free_iface_stats_tbl();
      return -1;
    }
  }

  /* Start overall FD Timer. */
  if ( 0 != ps_sys_fast_dormancy_start_timer(PS_SYS_FAST_DORMANCY_TIMER_1))
  {
    ps_sys_fast_dormancy_stop(); 
    return -1;
  }
   /* indicate PS to report packet dropped indication  */
  ps_iface_set_first_powersave_packet_drop_reported(FALSE);
   
  /* clear agressive FD Timer as it should be started after dropped Indication */
  ps_sys_fast_dormancy_clr_timer(PS_SYS_FAST_DORMANCY_TIMER_2);

  return 0;
}/* ps_sys_fast_dormancy_start */

/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_STOP()

DESCRIPTION
  This function clears Fast Dormancy Timers and also fress the memory allocated
  for all IFACE Stats

PARAMETERS
None

RETURN VALUE
 None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sys_fast_dormancy_stop() 
{
  /*---------------------------------------------------------------------- */
  /* clear overalL FD Timer */
  ps_sys_fast_dormancy_clr_timer(PS_SYS_FAST_DORMANCY_TIMER_1);

  /* clear FD agressive timer  */
  ps_sys_fast_dormancy_clr_timer(PS_SYS_FAST_DORMANCY_TIMER_2);

  /* Freeing the dynamic memory allocated  */
  ps_iface_fast_dormancy_free_iface_stats_tbl();

  fast_dormancy_info.pkt_dropd_ind_rcvd = FALSE;

  LOG_MSG_INFO1_0("ps_sys_fast_dormancy_stop(): Stopped");
  return;
} /* ps_sys_fast_dormancy_stop*/

/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_PKT_DROPPED_IND()

DESCRIPTION
  This function handles packet dropped indication from Framework 
  and starts Fast Dormancy Timer_2. Also update statistics table 
  with current counts

PARAMETERS
iface_ptr: IFACE on which packet drop occured

RETURN VALUE
 None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sys_fast_dormancy_pkt_dropped_ind
(
  ps_iface_type       *iface_ptr    /* for later use or enhancement */
)
{
 /* --------------------------------------------------------------------- */
  LOG_MSG_INFO1_0("ps_sys_fast_dormancy_pkt_dropped_ind(): Received");
 
  /* Start agressive timer only if overall timer is running */
  if(ps_sys_fast_dormancy_is_timer_running(PS_SYS_FAST_DORMANCY_TIMER_1) 
                                                                      == TRUE)
  {
    if(fast_dormancy_info.pkt_dropd_ind_rcvd == FALSE)
    {
      fast_dormancy_info.pkt_dropd_ind_rcvd = TRUE;

      /* Updating the statistics before Timer_2 start  */
      if ( 0 != ps_iface_fast_dormancy_update_3gpp_iface_stats())
      {
        LOG_MSG_INFO1_0("ps_sys_fast_dormancy_start_timer():"
                                                     " Stats Update failed");
        ps_sys_fast_dormancy_stop(); 
        return ;
      }
    }

    if ( 0 == ps_sys_fast_dormancy_start_timer(PS_SYS_FAST_DORMANCY_TIMER_2))
    {
      LOG_MSG_INFO1_0("ps_sys_fast_dormancy_pkt_dropped_ind():"
                                                     " start Timer Success");
    }
  }
  return;
}/* ps_sys_fast_dormancy_pkt_dropped_ind  */


/*===========================================================================
FUNCTION PS_SYS_FAST_DORMANCY_TIMER_EXPIRY_CB()

DESCRIPTION
  This function will get called when timer is expired.
  checks for data activity and calls mode handler  IOCTL to release
  signalling connection if activity is not present

PARAMETERS
cb_data:  which timer has expired

RETURN VALUE
 None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sys_fast_dormancy_timer_expiry_cb
(
  void*  cb_data /* callback data contains the timer identity here */
) 
{  
  ps_sys_fast_dormancy_enum_timer_type   timer_type;
  int                                    stop_fast_dormancy = 0;
  int16                                  ps_errno;
  /* --------------------------------------------------------------- */
  
  timer_type = (ps_sys_fast_dormancy_enum_timer_type) cb_data;

  if (timer_type > PS_SYS_FAST_DORMANCY_TIMER_2)
  {
    LOG_MSG_INVALID_INPUT_1("ps_sys_fast_dormancy_timer_expiry_cb():"
                                     "Invalid args Timer_type %d", timer_type);
    return;
  }

  LOG_MSG_INFO1_1("ps_sys_fast_dormancy_timer_expiry_cb():timer_type: %d",
                                                                   timer_type);

  if ((timer_type == PS_SYS_FAST_DORMANCY_TIMER_1) &&
      (ps_sys_fast_dormancy_is_timer_running(PS_SYS_FAST_DORMANCY_TIMER_2) 
                                                                   == TRUE) ) 
  {
     ps_sys_fast_dormancy_stop(); 
     return;
  }

  if ( 0 == ps_iface_fast_dormancy_check_data_activity(&stop_fast_dormancy))
  {
    LOG_MSG_INFO1_0("ps_sys_fast_dormancy_timer_expiry_cb():"
                                                    "Data Activity present");
    if (0 == stop_fast_dormancy)
    {
      /* there is valid data activity, stop fast dormancy*/
      ps_sys_fast_dormancy_stop();
    }
    else
    {
      ps_sys_fast_dormancy_start_timer(PS_SYS_FAST_DORMANCY_TIMER_2);
    }
  }
  else
  {
    LOG_MSG_INFO1_0("ps_sys_fast_dormancy_timer_expiry_cb():"
                                    "No Data Activity, Release Connection");
    if ( 0 != ps_sys_ioctl( PS_SYS_TECH_3GPP, 
                            PS_SYS_IOCTL_RELEASE_SIGNALLING_CONNECTION,
                            NULL, 
                            &ps_errno) )
    {
      LOG_MSG_ERROR_0("Failed SYS IOCTL RELEASE_SIGNALLING_CONNECTION");
    }

    /*if this expiry is because of Timer_2 then don't stop fast dormancy 
      algorithm. Continue Timer_1 to run till expiry. This will help 
      in releasing the signalling connection if first request was ignored 
      by NW in case of data inactivity */
    if (timer_type == PS_SYS_FAST_DORMANCY_TIMER_1)
    {
      ps_sys_fast_dormancy_stop();
    }
  }
 return;
}/* ps_sys_fast_dormancy_timer_expiry_cb */


