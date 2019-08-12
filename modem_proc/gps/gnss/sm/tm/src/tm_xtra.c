/*===========================================================================

                  TM - XTRA Core Module

DESCRIPTION
  This file contains implementations for TM XTRA Core.

  Responsible for handling XTRA related functionality. 

  

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*
Copyright (c) 2013-2014 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_xtra.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/15   skm     Xtra Version check fix  
05/12/14   ah      Added support for Set XTRA Version Check
12/06/13   ah      Support for non-deferrable timers
09/24/13   rh      Allow one-part XTRA injection for XTRA3c.bin
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/01/08   rw      Add gpsOneXTRA events
06/21/07   rw      Initial Release
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"   /* Definition for basic types and macros */
#include "customer.h"

/*---------------------------------------------------------------------------

   This section contain code which is only built when 
   FEATURE_GPSONE_XTRA is defined.
   
----------------------------------------------------------------------------*/
#ifdef FEATURE_CGPS_XTRA

#include "time_svc.h"
#include  <stdio.h>
#include  <stdlib.h>
#include "gps_common.h"
#include "tm_cm_iface.h"
#include "tm_common.h"
#include "tm_nv.h"

#include "lm_api.h"
#include "tm_api.h"
#include "tm_xtra.h"

#include "tm_data.h"
#include "mgp_api.h"

#include "sm_dm_event.h"

#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#endif

/*------------------------- Global Variables ------------------------------------*/
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
tm_xtra_state_type*  p_xtra_state_info_ptr = NULL;  /* TM XTRA state information */
#else
tm_xtra_state_type  z_xtra_state_info;  /* TM XTRA state information */
#endif /*FEATURE_CGPS_XTRA_DYNAMIC_ALLOC*/

#ifdef FEATURE_CGPS_XTRA_DEBUG
/* Used for debugging */
static char pchAsciiOutput[XTRA_TEMP_ASCII_STRING_SIZE];
#endif /* FEATURE_CGPS_XTRA_DEBUG */

#ifdef FEATURE_GNSS_XTRA3 
/* since tm_decode_xtra3_data.h is not shiped, declare it here */
void tm_xtra3_set_download_url(uint8 **Xtra3Url_ptr, uint32 NumUrl, uint32 MaxUrlLen);
#endif /* FEATURE_GNSS_XTRA3 */

/*===========================================================================
=============================================================================

   XTRA Module functions
   
=============================================================================
===========================================================================*/


/*===========================================================================
FUNCTION tm_xtra_req_allow

DESCRIPTION
Allow XTRA data requests.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
static void tm_xtra_req_allow
(
  void
)
{
  tm_xtra_state_type *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return;
  }
  
  xtra_state_ptr->time_of_throttle_end = 0;
  xtra_state_ptr->dl_attempts = TM_XTRA_INIT_RETRY_ATTEMPTS;
  xtra_state_ptr->continue_blocking = FALSE;
  TM_XTRA_MSG_MED("XTRA data requests unblocked",0, 0,0); 
   
  return;
}

/*===========================================================================
FUNCTION tm_xtra_is_req_blocked

DESCRIPTION
Check XTRA data request throttling status based on timestamp.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
Throttle status
===========================================================================*/
static boolean tm_xtra_is_req_blocked
(
  void
)
{
  uint32 now;
  tm_xtra_state_type *xtra_state_ptr =  tm_xtra_get_state_ptr();
  

  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return TRUE;
  }
  
  now = time_get_secs();

  TM_XTRA_MSG_MED("now %lu, end %lu ",now,xtra_state_ptr->time_of_throttle_end,0 );
  
  if (now >= xtra_state_ptr->time_of_throttle_end)
  {
    TM_XTRA_MSG_MED("tm_xtra_is_req_blocked == FALSE",0,0,0);
    return FALSE;
  }
  
  TM_XTRA_MSG_MED("tm_xtra_is_req_blocked == TRUE",0,0,0);
  return TRUE;
}


/*===========================================================================
FUNCTION tm_xtra_req_block

DESCRIPTION
Block XTRA requests until retry timeout.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
static void tm_xtra_req_block
(
  void
)
{
  uint32 time_between_attempts = 0; 
  tm_xtra_state_type *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return;
  }

  xtra_state_ptr->dl_attempts++;
  if (xtra_state_ptr->dl_attempts <= xtra_state_ptr->num_download_attempts)
  {  
     time_between_attempts = xtra_state_ptr->time_between_attempts*(TM_XTRA_MSEC_IN_MIN/TM_XTRA_SEC_TO_MSEC);       
     TM_XTRA_MSG_MED("XTRA BLOCKED for %lu seconds, attempt = %d",time_between_attempts, xtra_state_ptr->dl_attempts,xtra_state_ptr->num_download_attempts);
  }
  else
  {
    time_between_attempts = TM_XTRA_LAST_RETRY_TIMEOUT; //seconds
    TM_XTRA_MSG_MED("XTRA BLOCKED for %lu seconds, attempt = %d",time_between_attempts, xtra_state_ptr->dl_attempts,0);
  }
  
  xtra_state_ptr->time_of_throttle_end = time_get_secs() + time_between_attempts;

  return;
}

/*===========================================================================
FUNCTION tm_xtra_init_state_ptr

DESCRIPTION
Dynamically allocate memory for the XTRA state pointer

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
Memory allocation status
===========================================================================*/
boolean tm_xtra_init_state_ptr(void)
{
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if(p_xtra_state_info_ptr != NULL)
  {
    TM_XTRA_MSG_MED("XTRA state pointer mem allocation alredy done",0,0,0);   
    return TRUE;
  }
  p_xtra_state_info_ptr = (tm_xtra_state_type*)os_MemAlloc(sizeof(tm_xtra_state_type), OS_MEM_SCOPE_TASK);
  memset((void *)p_xtra_state_info_ptr, 0x0, sizeof(tm_xtra_state_type));

  if(p_xtra_state_info_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("XTRA state pointer mem allocation failed",0,0,0);	  
	return FALSE;
  }
  else
  {
    TM_XTRA_MSG_MED("XTRA state pointer mem allocation successful",0,0,0);    
	return TRUE;
  }
#else
  return TRUE;   
#endif /*FEATURE_CGPS_XTRA_DYNAMIC_ALLOC*/
}

/*===========================================================================
FUNCTION tm_xtra_init

DESCRIPTION
Initialize the XTRA module

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
void tm_xtra_init
(
  void
)
{
  /* Local Variables */
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

    
  if(xtra_state_ptr == NULL)
  {
	MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
	return;
  }

  /*----------------------------------------------------------------------
     Initialize all XTRA session data
  ----------------------------------------------------------------------*/  

  /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if (xtra_state_ptr->xtraTempBuf != NULL)
  {
    TM_XTRA_MSG_ERROR("XTRA buffer ptr not NULL, possible memory leak!",0,0,0);
    xtra_state_ptr->xtraTempBuf = NULL;
  }
#else
  memset((void *)xtra_state_ptr->xtraTempBuf, 
         0x0,
         TM_XTRA_MAX_FILE_SIZE);
#endif

  /* Used to manage data read into XtraTempBuf */
  xtra_state_ptr->q_XtraDataLen = 0;

  /* GPS time structure */
  memset((void*)&xtra_state_ptr->gps_time,
         0x0,
         sizeof(gps_TimeStructType) );

  /*----------------------------------------------------------------------
     XTRA NV item local settings
  ----------------------------------------------------------------------*/  

  /* Number of hours between automatic downloads [hours] */
  if(xtra_state_ptr->download_interval < TM_XTRA_MIN_DOWNLOAD_INTERVAL
     || xtra_state_ptr->download_interval > TM_XTRA_MAX_DOWNLOAD_INTERVAL)
  {
    /* Set Default value */
    xtra_state_ptr->download_interval 
                   = TM_XTRA_DEFAULT_DOWNLOAD_INTERVAL;
  }

  /* Number of download attempts (0...10) */
  if (xtra_state_ptr->num_download_attempts > TM_XTRA_MAX_NUM_DOWNLOAD_ATTEMPTS )
  {
    /* Set Default value */
    xtra_state_ptr->num_download_attempts 
                   = TM_XTRA_DEFAULT_NUM_DOWNLOAD_ATTEMPTS;
  }

  /* Number of minutes between download attempts */
  if(xtra_state_ptr->time_between_attempts < TM_XTRA_MIN_TIME_BETWEEN_ATTEMPTS 
     || xtra_state_ptr->time_between_attempts > TM_XTRA_MAX_TIME_BETWEEN_ATTEMPTS)
  {
    /* Set Default value */
    xtra_state_ptr->time_between_attempts 
                   = TM_XTRA_DEFAULT_TIME_BETWEEN_ATTEMPTS;    
  }

  /* One-way delay failover threshold */
  if(xtra_state_ptr->time_info_oneway_delay_thresh < TM_XTRA_MIN_TIME_INFO_DELAY_THRESH 
     || xtra_state_ptr->time_info_oneway_delay_thresh > TM_XTRA_MAX_TIME_INFO_DELAY_THRESH)
  {
    xtra_state_ptr->time_info_oneway_delay_thresh
                 = TM_XTRA_DEFAULT_TIME_INFO_DELAY_THRESH;
  }

  /* Time uncertainty threshold */
  if(xtra_state_ptr->time_info_uncertainty_thresh < TM_XTRA_MIN_TIME_INFO_UNC_THRESH 
     || xtra_state_ptr->time_info_uncertainty_thresh > TM_XTRA_MAX_TIME_INFO_UNC_THRESH)
  {
    xtra_state_ptr->time_info_uncertainty_thresh
                 = TM_XTRA_DEFAULT_TIME_INFO_UNC_THRESH;
  }

  /*----------------------------------------------------------------------
     Initialize TM-XTRA module session information 
  ----------------------------------------------------------------------*/
  /* data/time injection handler state */
  xtra_state_ptr->e_state = TM_XTRA_STATE_IDLE;

  /* Number of dload request retry attempts */
  xtra_state_ptr->retry_attempts =  TM_XTRA_INIT_RETRY_ATTEMPTS;

  /* Allow On Demand requests */
  tm_xtra_req_allow();
 
  /* Source of Time Information request */
  xtra_state_ptr->time_info_req_src = GPS_TIME_CHECK_SOURCE_NONE;

  /* XTRA Version Check */
  xtra_state_ptr->version_check = PDSM_PA_XTRA_VERSION_CHECK_DISABLED;

  /*----------------------------------------------------------------------
     Initialize TM-XTRA module Timers if not already created
  ----------------------------------------------------------------------*/
  if(xtra_state_ptr->auto_dload_timer == NULL)
  {
    xtra_state_ptr->auto_dload_timer =
        os_TimerCreateInCgpsTimerGroup( TM_XTRA_TIMER_ID_AUTO_DLOAD,
                                        (uint32)THREAD_ID_SM_TM );
  }
  if(xtra_state_ptr->retry_dload_timer == NULL)
  {
    xtra_state_ptr->retry_dload_timer =
        os_TimerCreateInCgpsTimerGroup( TM_XTRA_TIMER_ID_DLOAD_REQ_RETRY,
                                        (uint32)THREAD_ID_SM_TM );
  }

  /* Start the Auto download timer */
  if(xtra_state_ptr->auto_download_enabled)
  {
    /* Start the timer */
    (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                        TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                        0);
  }

} /* End tm_xtra_init()*/

/*===========================================================================
FUNCTION tm_xtra_reset_data_buffer

DESCRIPTION
This functions reset XTRA data download buffer if SET_DATA command fails.
It'll reset XTRA's state to IDLE as well

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_reset_data_buffer(void)
{
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

  if (xtra_state_ptr == NULL)
  {
	MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
	return;
  }

  /*----------------------------------------------------------------------
     Clear the variables used to load data from XTRA.BIN
  ----------------------------------------------------------------------*/  
  /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if (xtra_state_ptr->xtraTempBuf != NULL)
  {
  os_MemFree((void**)&xtra_state_ptr->xtraTempBuf);
  xtra_state_ptr->xtraTempBuf = NULL;
  }
#else
  memset((void *)xtra_state_ptr->xtraTempBuf, 
         0x0,
         TM_XTRA_MAX_FILE_SIZE);
#endif

  /* Used to manage data read into XtraTempBuf */
  xtra_state_ptr->q_XtraDataLen = 0;

  /*----------------------------------------------------------------------
     Initialize XTRA Session state
  ----------------------------------------------------------------------*/  
  xtra_state_ptr->e_state = TM_XTRA_STATE_IDLE;
}

/*===========================================================================
FUNCTION tm_xtra_abort_and_recover

DESCRIPTION
This is the cleanup function for TM-XTRA module.  It will initialize the 
current state and variables.  Then triggers a status event to the client, if
the end_status is valid

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
void tm_xtra_abort_and_recover
(
  pdsm_xtra_status_e_type end_status
)
{
  /* Local Variables */
  tm_xtra_event_payload_u_type  status_event_payload;
  
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

  TM_XTRA_MSG_ERROR("Abort and Recover (end_status=%d)",end_status,0,0);

  if(xtra_state_ptr == NULL)
  {
	MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
	return;
  }

  /*----------------------------------------------------------------------
     Clear the variables used to load data from XTRA.BIN
  ----------------------------------------------------------------------*/  
  /* continue blocking flag is set in in case XTRA file was rejected because of stale content */
  if (xtra_state_ptr->continue_blocking == FALSE)
  { 
     tm_xtra_req_allow();
  }
  else
  {
     xtra_state_ptr->continue_blocking = FALSE;
  }
  
  /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  if (xtra_state_ptr->xtraTempBuf != NULL)
  {
  os_MemFree((void**)&xtra_state_ptr->xtraTempBuf);
  xtra_state_ptr->xtraTempBuf = NULL;
  }
#else
  memset((void *)xtra_state_ptr->xtraTempBuf, 
         0x0,
         TM_XTRA_MAX_FILE_SIZE);
#endif

  /* Used to manage data read into XtraTempBuf */
  xtra_state_ptr->q_XtraDataLen = 0;

  /*----------------------------------------------------------------------
     Initialize XTRA Session state
  ----------------------------------------------------------------------*/  
  xtra_state_ptr->e_state = TM_XTRA_STATE_IDLE;

  /* Number of dload request retry attempts */
  xtra_state_ptr->retry_attempts =  TM_XTRA_INIT_RETRY_ATTEMPTS;

  /* Source of Time Information request */
  xtra_state_ptr->time_info_req_src = GPS_TIME_CHECK_SOURCE_NONE;

  /*----------------------------------------------------------------------
     Stop Timers
  ----------------------------------------------------------------------*/  
  (void)os_TimerStop(xtra_state_ptr->auto_dload_timer);
  (void)os_TimerStop(xtra_state_ptr->retry_dload_timer);
  
  /*----------------------------------------------------------------------
     Send the Status event to the XTRA clients
  ----------------------------------------------------------------------*/
  if( end_status != XTRA_DOWNLOAD_STATUS_MAX )
  {
    status_event_payload.status_payload.status = end_status;
    
    (void)tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, status_event_payload);

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_end_dload(end_status);
  }

  /* let's restart auto-download timer if enabled */
    /* Start the Auto download timer */
  if(xtra_state_ptr->auto_download_enabled && xtra_state_ptr->auto_dload_timer)
  {
    /* Start the timer */
    (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                        TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                        0);
  }
#ifdef FEATURE_CGPS_XTRA_T
  /* will not trigger XTRA-T if XTRA is triggered by AGPS(SUPL/V2) */
  if (tm_core_info.session_info.req_xtra_trig_by_agps == FALSE)
  {
    /* Let TLE know a XTRA download session completes. TLE will trigger
       a XTRA-T upload/download session.
     */
    (void) tle_RequestSessionByXtra(tm_core_is_standalone_active());
  }
  else
  {
    /* reset the "XTRA triggered by AGPS" flag */
    tm_core_info.session_info.req_xtra_trig_by_agps = FALSE;
  }
#endif  
  return;  
} /* End tm_xtra_abort_and_recover()*/


/*===========================================================================
FUNCTION tm_xtra_translate_end_status

DESCRIPTION
Handle the abort and recover request from TM-Core.  Only certain abort reasons
should end TM-XTRA module processing.  For example, E911 call origination
or TM-Core task stop.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_xtra_status_e_type tm_xtra_translate_end_status
(
  const tm_sess_stop_reason_e_type reason
)
{
  /* Local Variables */
  pdsm_xtra_status_e_type ret_val = XTRA_DOWNLOAD_STATUS_MAX;

  /* Translate the TM-Core stop reason to an XTRA stop reason */
  switch(reason)
  {
    case TM_STOP_REASON_E911:
      ret_val = XTRA_DOWNLOAD_STATUS_END_E911;
      break;

    case TM_STOP_REASON_TASK_OFFLINE:
    case TM_STOP_REASON_TASK_STOP:
      ret_val = XTRA_DOWNLOAD_STATUS_FAILURE;
      break;

    case TM_STOP_REASON_ON_RESET_LOCATION_SERVICE:
        ret_val = XTRA_DOWNLOAD_STATUS_END_RESET_LOCATION_SERVICE;
        break;      

    default:
      TM_XTRA_MSG_MED("TM-Core stop reason (%d), not valid for XTRA", reason,0,0);
      break;  
  }

  return ret_val;
} /* End tm_xtra_translate_end_status() */


/*===========================================================================
FUNCTION tm_xtra_handle_core_abort_recover

DESCRIPTION
Handle the abort and recover request from TM-Core.  Only certain abort reasons
should end TM-XTRA module processing.  For example, E911 call origination
or TM-Core task stop.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
void tm_xtra_handle_core_abort_recover
(
  const tm_sess_stop_reason_e_type abort_reason
)
{
  /* Local Variables */
  pdsm_xtra_status_e_type xtra_end_status = XTRA_DOWNLOAD_STATUS_MAX;

  /* Translate the abort reason from TM-Core */
  xtra_end_status = tm_xtra_translate_end_status(abort_reason);

  /* Abort the XTRA session, only if this is a valid XTRA end reason */
  if(xtra_end_status != XTRA_DOWNLOAD_STATUS_MAX)
  {
    tm_xtra_abort_and_recover(xtra_end_status);
  }

  return;
} /* End tm_xtra_handle_core_abort_recover() */


/*===========================================================================
FUNCTION tm_xtra_is_enable

DESCRIPTION
Check if the XTRA feature is enabled

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_is_enable
(
  void
)
{
  /* Local Variable */
  tm_xtra_state_type *xtra_info_ptr = tm_xtra_get_state_ptr();
  
  if(xtra_info_ptr == NULL)
  {
    return FALSE;
  }
  
  return xtra_info_ptr->xtra_enabled;
} /* End tm_xtra_is_enable()*/

/*===========================================================================
FUNCTION tm_xtra_is_time_info_enable

DESCRIPTION
Check if the XTRA time info feature is enabled

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_is_time_info_enable
(
  void
)
{
  /* Local Variable */
  tm_xtra_state_type *xtra_info_ptr = tm_xtra_get_state_ptr();
  
  if(xtra_info_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }
  
  return xtra_info_ptr->time_info_enabled;
} /* End tm_xtra_is_time_info_enable()*/


/*===========================================================================

FUNCTION   tm_xtra_update_sess_state

DESCRIPTION
  This function is used by TM-XTRA module to update the current session state.

RETURN VALUE
  TRUE - State transition is successful
  FALSE - otherwise
============================================================================*/
boolean tm_xtra_update_sess_state
(
  tm_xtra_state_e_type new_state
)
{
  /* Local Variables */
  boolean                      state_tran = FALSE;
  tm_xtra_state_e_type         curr_state = TM_XTRA_STATE_IDLE;
  
  tm_xtra_state_type           *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }

  /*------------------------------------------------------------------------
     Check the TM-JGPS Core session state transition
  -------------------------------------------------------------------------*/
 
  /* Current Session State */
  curr_state = xtra_state_ptr->e_state;
  
  if( curr_state == new_state )
  {
    TM_XTRA_MSG_MED("current state [%d], is same as new state [%d]",
                    curr_state, 
                    new_state,
                    0 );
    return TRUE;
  }
  
  /* Determine if the session state can be modified*/
  switch(new_state)
  {  
    case TM_XTRA_STATE_IDLE:
      if(   curr_state == TM_XTRA_STATE_CLIENT_REQ
         || curr_state == TM_XTRA_STATE_WAIT_DATA
         || curr_state == TM_XTRA_STATE_INJECT_DATA )
      {
        state_tran = TRUE;
#ifdef FEATURE_CGPS_XTRA_T
        TM_XTRA_MSG_HIGH("Notify TLE XTRA Session Stop",0,0,0);
        tle_XTRAEventNotify(TLE_EVENT_XTRA_STOP);
#endif
      }
      break;
    case TM_XTRA_STATE_CLIENT_REQ:
      if( curr_state == TM_XTRA_STATE_IDLE )
      {
        state_tran = TRUE;
#ifdef FEATURE_CGPS_XTRA_T
        TM_XTRA_MSG_HIGH("Notify TLE XTRA Session Start",0,0,0);
        tle_XTRAEventNotify(TLE_EVENT_XTRA_START);
#endif
      }
      break;
    case TM_XTRA_STATE_WAIT_DATA:
      if( curr_state == TM_XTRA_STATE_CLIENT_REQ )
      {
        state_tran = TRUE;
      }
      break;
    case TM_XTRA_STATE_INJECT_DATA:
      if( curr_state == TM_XTRA_STATE_WAIT_DATA ||
          curr_state == TM_XTRA_STATE_CLIENT_REQ )
      {
        state_tran = TRUE;
      }
      break;
  
    default:
      state_tran = FALSE;
      break;    
  }

  /*------------------------------------------------------------------------
     Update the TM-JGPS Core session state
  -------------------------------------------------------------------------*/
  if(state_tran)
  {
    xtra_state_ptr->e_state = new_state;
    TM_XTRA_MSG_MED("state transition [%d] -> [%d]",
                    curr_state, 
                    xtra_state_ptr->e_state,
                    0 );
  }
  else
  {
    TM_XTRA_MSG_MED( "state transition failed cur state [%d], req state [%d]",
                      curr_state, 
                      new_state,
                      0 );
  }
  
  return state_tran;

} /* tm_xtra_update_sess_state */


/*===========================================================================
FUNCTION tm_xtra_is_download_in_progress

DESCRIPTION
Determine if the current download is in progress

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_is_download_in_progress
(
  void
)
{
  /* Local Variables */
  boolean             ret_val = FALSE;
  tm_xtra_state_type  *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }

  if(    xtra_state_ptr->e_state == TM_XTRA_STATE_WAIT_DATA
      || xtra_state_ptr->e_state == TM_XTRA_STATE_INJECT_DATA )
  {
    ret_val = TRUE;
  }

  return ret_val; 
} /* End tm_xtra_is_download_in_progress()*/


/*===========================================================================
FUNCTION tm_xtra_is_download_req_pending

DESCRIPTION
Determine if a download requst is pending

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_is_download_req_pending
(
  void
)
{
  /* Local Variables */
  boolean             ret_val = FALSE;
  tm_xtra_state_type  *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }

  if( xtra_state_ptr->e_state == TM_XTRA_STATE_CLIENT_REQ )
  {
    ret_val = TRUE;
  }

  return ret_val;
} /* End tm_xtra_is_download_req_pending()*/

/*===========================================================================

FUNCTION tm_xtra_handle_auto_dload_timer_expiry

DESCRIPTION
  This function handles the automatic download timer expiration. 
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_xtra_handle_auto_dload_timer_expiry
(
  void
)
{
   tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();
    
   if(xtra_state_ptr == NULL)
   {
      MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
      return;
   }
    
   /*check if time of last download is non-zero (valid)
   If an XTRA file has never been downloaded, then don't do anything.
   The very first download should be done manually or during a positioning
   session before the automatic downloading functionality kicks in to 
   prevent the phone trying to retrieve XTRA data as soon as it is turned 
   on for the first time*/
  if(tm_xtra_data_valid())
  {
    (void)tm_xtra_data_validity_check( GPS_TIME_CHECK_SOURCE_PERIODIC );
  }
  else
  {
     if(xtra_state_ptr != NULL)
     {
       /* Re-start the auto dload timer */
       (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                           TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                           0);  
     }
  }
  
} /* End tm_xtra_handle_auto_dload_timer_expiry()*/

/*===========================================================================

FUNCTION tm_xtra_handle_dload_req_retry_timer_expiry

DESCRIPTION
  This function handles the automatic download timer expiration. 
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_xtra_handle_dload_req_retry_timer_expiry
(
  void
)
{
  /* Local Variables */
  boolean                       ret_val = FALSE;
  tm_xtra_event_payload_u_type  event_payload;
  tm_xtra_state_type            *xtra_state_ptr =  tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return;
  }

  /* Update the retry attempts counter */
  xtra_state_ptr->retry_attempts++;

  /* If the download attempt exceeds some threshold terminate the session */
  if(xtra_state_ptr->retry_attempts >= xtra_state_ptr->num_download_attempts)
  {
    tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_RETRY_EXCEEDED);
  }
  else
  {
    /*--------------------------------------------------------------------
       Trigger the download request processing
    --------------------------------------------------------------------*/
    TM_XTRA_MSG_LOW("Processing initiate_xtra_download command. ",0,0,0);
  
    event_payload.data_dload_payload.force_data_dload = TRUE;
      
    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, 
                                     event_payload );
      
    /*--------------------------------------------------------------------
       Based on the trigger event and current state determine if timer 
       should be re-started.
    --------------------------------------------------------------------*/
    if(ret_val)
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_RETRY);
      
      if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
      }
    }
    else if ( tm_xtra_is_download_in_progress() 
             || tm_xtra_is_download_req_pending() )
    {
      TM_XTRA_MSG_MED("Download currently in progress or pending - restart dload retry timer (state=%d)",
                       xtra_state_ptr->e_state,0,0);

      /* Start the recover timer */
      (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                          (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                          0);
    }
    else
    {
      /* Abort and recover the session */
      tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);
    } /* end else */
  } /* end else */

  return;
} /* End tm_xtra_handle_dload_req_retry_timer_expiry()*/


/*===========================================================================

FUNCTION tm_xtra_handle_timer_expiry

DESCRIPTION
  This function is called by TM-core to inform XTRA module about timer
  expiry event.
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_xtra_handle_timer_expiry(const os_TimerExpiryType *timer_data)
{
  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;
  
  if(timer_param != NULL)
  {
    MSG_HIGH("timerId 0x%x expired", timer_param->q_TimerId, 0, 0);
    
    switch(timer_param->q_TimerId)
    {
      case TM_XTRA_TIMER_ID_AUTO_DLOAD:
        tm_xtra_handle_auto_dload_timer_expiry();
      break;
      
      case TM_XTRA_TIMER_ID_DLOAD_REQ_RETRY:
        tm_xtra_handle_dload_req_retry_timer_expiry();
      break;
      
      default:
        MSG_ERROR("Uknown timer expired", 0, 0, 0);
      break;
    }
  }
  else
  {
    MSG_ERROR("Can not proess null timer msg", 0, 0, 0);  
  }
} /* End tm_xtra_handle_timer_expiry() */


/*===========================================================================
FUNCTION tm_xtra_randomize_and_set_urls

DESCRIPTION
Randomize order of input URLs and output.  This is for load-balancing 
purposes to aid XTRA clients accessing different XTRA servers.  
(Assumption that XTRA clients do NOT perform internal load-balancing
techniques)


INPUT PARAMS
U8 **InUrl_ptr

OUTPUT PARAMS
U8 **OutUrl_ptr

RETURN VALUE 
void
===========================================================================*/
void tm_xtra_randomize_and_set_urls
(
  uint8 **InUrl_ptr,
  uint8 **OutUrl_ptr
)
{
  /* Local Variables */
  uint8 i, k, indOut = 0;
  uint8 initUrl = 0;

  qword current_time;

  /* Check for null pointers */
  if ((InUrl_ptr == NULL) || (OutUrl_ptr == NULL))
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter %lx %lx",
                     InUrl_ptr,OutUrl_ptr,0);
    return;
  }

  /* Get current system time
  */
  (void) time_get(current_time);
  
  /* Generate a random integer btwn 0 and 2 */
  initUrl = (uint8)(current_time[0] % TM_XTRA_MAX_NUMBER_URLS);

  for ( k = 0; k < TM_XTRA_MAX_NUMBER_URLS; k++ )
  {
    if ( InUrl_ptr[initUrl] != NULL && InUrl_ptr[initUrl][0] != '\0' )
    {
      for (i = 0; i < TM_XTRA_MAX_SERVER_URL_SIZE; i++)
      {
        OutUrl_ptr[indOut][i] = InUrl_ptr[initUrl][i];

        if ( InUrl_ptr[initUrl][i] == '\0' )
          break;
      }

      indOut++;
    }

    initUrl++;
    initUrl %= TM_XTRA_MAX_NUMBER_URLS;
  }

  /* Fill in remaining output URLs with NULL terminated strings */
  for ( k = indOut; k < TM_XTRA_MAX_NUMBER_URLS; k++ )
  {
    OutUrl_ptr[k][0] = '\0';
  }

  return;
} /* End tm_xtra_randomize_and_set_urls()*/


/*===========================================================================
=============================================================================

   TM-XTRA: PD API Requst Check and Process 
   
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION tm_xtra_check_set_data_para

DESCRIPTION
Check the input parameters for XTRA set data client request.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_xtra_cmd_err_e_type tm_xtra_check_set_data_para
(
  const pdsm_xtra_cmd_s_type *req_ptr
)
{
  /* Local Variables */
  pdsm_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_XTRA_CMD_ERR_NOERR;

   
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }     
  /*----------------------------------------------------------------------
      Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  }   
  /*-----------------------------------------------------------------------
     Check if exceeding max local buffer size 
  -----------------------------------------------------------------------*/
  else if (req_ptr->info.xtra_data_len > TM_XTRA_MAX_FILE_SIZE)
  {
    TM_XTRA_MSG_ERROR("Exceeding max buffer size %ld", 
                      req_ptr->info.xtra_data_len, 0, 0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_NO_BUF_L;
  }
  /*-----------------------------------------------------------------------
     Check the xtra data ptr
  -----------------------------------------------------------------------*/
  else if (req_ptr->info.xtra_data_ptr == NULL )
  {
    TM_XTRA_MSG_ERROR("NULL ptr from XTRA client", 0, 0, 0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*-----------------------------------------------------------------------
     Verify the part number
  -----------------------------------------------------------------------*/
  else if (req_ptr->info.part_number > req_ptr->info.total_parts)
  {
    TM_XTRA_MSG_ERROR("part_number %d > total_parts %d",
                      req_ptr->info.part_number, 
                      req_ptr->info.total_parts, 0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*-----------------------------------------------------------------------
     Check the XTRA client Download status
  -----------------------------------------------------------------------*/
  else if( (req_ptr->info.part_number < req_ptr->info.total_parts )
     && (PDSM_XTRA_STATUS_FAILURE == req_ptr->info.xtra_dc_status) )
  {
    TM_XTRA_MSG_ERROR("Failed to Recv complete XTRA file "
                      "(part num %d < total %d, status: %d",
                      req_ptr->info.part_number, 
                      req_ptr->info.total_parts, 
                      req_ptr->info.xtra_dc_status);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     If assisted (MS-B dload, MS-A) mode operation is ongoing, block
     client request
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_core_is_xtra_operation_allowed() )
  {
    TM_XTRA_MSG_ERROR("XTRA: Assisted GPS session in progress",0,0,0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY;
  }
  
  return xtra_cmd_err;

} /* End tm_xtra_check_set_data_para() */


/*===========================================================================
FUNCTION tm_xtra_check_initiate_download_para

DESCRIPTION
Check the input parameters for XTRA check init download parameters.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_xtra_cmd_err_e_type tm_xtra_check_initiate_download_para
(
  const pdsm_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  } 
  /*----------------------------------------------------------------------
     If assisted (MS-B dload, MS-A) mode operation is ongoing, block
     client request
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_core_is_xtra_operation_allowed() )
  {
    TM_XTRA_MSG_ERROR("XTRA: Assisted GPS session in progress",0,0,0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY;
  } 

  return xtra_cmd_err;
} /* End tm_xtra_check_initiate_download_para() */


/*===========================================================================
FUNCTION tm_xtra_check_auto_download_para

DESCRIPTION
Check the input parameters for XTRA check auto download parameters.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_xtra_cmd_err_e_type tm_xtra_check_auto_download_para
(
  const pdsm_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_XTRA_CMD_ERR_NOERR;

  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  } 
  /*-----------------------------------------------------------------------
     Check Auto Download interval
  -----------------------------------------------------------------------*/
  else if (   (req_ptr->info.download_interval < TM_XTRA_MIN_DOWNLOAD_INTERVAL) 
           || (req_ptr->info.download_interval > TM_XTRA_MAX_DOWNLOAD_INTERVAL) )
  {
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE;
  }


  return xtra_cmd_err;

} /* End tm_xtra_check_auto_download_para() */

/*===========================================================================
FUNCTION tm_xtra_check_query_data_validity_para

DESCRIPTION
Check the input parameters for XTRA check data validy request parameters.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_xtra_cmd_err_e_type tm_xtra_check_query_data_validity_para
(
  const pdsm_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature is disabled.",0,0,0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  } 

  return xtra_cmd_err;
} /* End tm_xtra_check_query_data_validity_para() */

/*===========================================================================
FUNCTION tm_xtra_copy_data

DESCRIPTION
This function is called by PD API at client's context. Be sure only to perform
simple operations in this function.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_xtra_cmd_err_e_type tm_xtra_copy_data(uint8 part_num, byte *xtra_part_ptr, uint32 xtra_part_length)
{
  /* Call this macro to obtain gpsOneXTRA tag pointer */
  tm_xtra_state_type *p_xtra_state_info = tm_xtra_get_state_ptr();

  if(p_xtra_state_info == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }

  // check XTRA data pointer
  if(xtra_part_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Received NULL XTRA data pointer in set_data",0,0,0);
    return PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }
  
  if (part_num == 1 && p_xtra_state_info->q_XtraDataLen != 0)
  {
    TM_XTRA_MSG_ERROR("Received XTRA download 1st part while buffer not empty!",0,0,0);
    p_xtra_state_info->q_XtraDataLen = 0; //Reset to 0, recovery from bad case
  }

  // check if we've exceeded the local buffer size
  if ( (p_xtra_state_info->q_XtraDataLen + xtra_part_length)
                                                > TM_XTRA_MAX_FILE_SIZE )
  {
    TM_XTRA_MSG_ERROR("Total data %d exceeds max allowed size %d",
      p_xtra_state_info->q_XtraDataLen,
      TM_XTRA_MAX_FILE_SIZE,0);
    /*Since this function doesn't have any recovery mechanism, we need to somehow indicate TM_XTRA
      that a buffer space issue has occured and session abort is needed. We set the XTRADataLen to 
      the MAX file size +1 so that when tm_xtra_process_set_data_cmd() is called, it will abort the
      session and recover*/
    p_xtra_state_info->q_XtraDataLen = TM_XTRA_MAX_FILE_SIZE + 1;

    return PDSM_XTRA_CMD_ERR_NO_BUF_L;
  }

#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
  /* Only allocate if this is first call to copy data */
  if (p_xtra_state_info->xtraTempBuf == NULL)
  {
    if (part_num == 1 && p_xtra_state_info->q_XtraDataLen == 0)
    {
      /*  Allocate temporary buffer for gpsOneXTRA data */
      p_xtra_state_info->xtraTempBuf = (uint8*)os_MemAlloc(TM_XTRA_MAX_FILE_SIZE, OS_MEM_SCOPE_TASK);
      
      if (p_xtra_state_info->xtraTempBuf == NULL)
      {
        TM_XTRA_MSG_ERROR("os_MemAlloc failed - out of memory",0,0,0);
        return PDSM_XTRA_CMD_ERR_NO_BUF_L;
      }
    }
    else
    {
      TM_XTRA_MSG_ERROR("Unexpected part # (%d) or data length (%d)",
        part_num, p_xtra_state_info->q_XtraDataLen,0);
      return PDSM_XTRA_CMD_ERR_NO_BUF_L;
    }
  }
#endif

  // copy the client data into the xtra buffer
  memscpy( p_xtra_state_info->xtraTempBuf + p_xtra_state_info->q_XtraDataLen, 
    xtra_part_length, xtra_part_ptr, xtra_part_length);

  // total amount of data received from client thus far
  p_xtra_state_info->q_XtraDataLen += xtra_part_length;   

  return PDSM_XTRA_CMD_ERR_NOERR;
}

/*===========================================================================
FUNCTION tm_xtra_check_query_data_validity_para

DESCRIPTION
Check the input parameters for XTRA check data validy request parameters.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_xtra_cmd_err_e_type tm_xtra_check_time_info_inject_para
(
  const pdsm_xtra_cmd_s_type *req_ptr
)
{
  /* Local Varaibles*/
  pdsm_xtra_cmd_err_e_type  xtra_cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  
  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (req_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("null ptr", 0, 0, 0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_INVALID_PARAM;
  }
  /*----------------------------------------------------------------------
     Determine if NV item enables XTRA feature
  ----------------------------------------------------------------------*/
  else if ( FALSE == tm_xtra_is_time_info_enable() )
  {
    TM_XTRA_MSG_ERROR("XTRA: feature xtra timeinfo is disabled.",0,0,0);
    xtra_cmd_err = PDSM_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
  }

  return xtra_cmd_err;
} /* End tm_xtra_check_time_info_inject_para() */


/*===========================================================================
FUNCTION tm_xtra_process_set_data_cmd

DESCRIPTION
Process the set data command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_set_data_cmd
(
  pdsm_xtra_cmd_s_type *cmd_ptr
)
{
   /* Local Variables */
   boolean                    ret_val = FALSE;
   tm_xtra_event_payload_u_type  event_payload;
   qword d_CurrentTime = {0}, d_TimeDiff = {0}, d_TimeDiffMs = {0};

   /* Call this macro to obtain gpsOneXTRA tag pointer */
   tm_xtra_state_type *p_xtra_state_info = tm_xtra_get_state_ptr();

   if(p_xtra_state_info == NULL)
   {
     MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  

     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;    
     (void)tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, event_payload);

     return FALSE;
   }

  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
   if (cmd_ptr == NULL)
   {
     TM_XTRA_MSG_ERROR("Null cmd_ptr",0,0,0);

     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;    
     (void)tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, event_payload);

     return FALSE;
   }

   /*-----------------------------------------------------------------------
      Process the incoming gpsOneXTRA data from client
   -----------------------------------------------------------------------*/
   do{
     /* Update the session state, to client request, if this is the first 
     ** XTRA data file part
     */
     if(p_xtra_state_info->e_state == TM_XTRA_STATE_IDLE)
     {
       /* Update the session state, IDLE->CLIENT_REQ should succeed */
       (void)tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ);
     }
     else if ( p_xtra_state_info->e_state == TM_XTRA_STATE_INJECT_DATA )
     {
       (void)time_get(d_CurrentTime);
       qw_sub(d_TimeDiff,d_CurrentTime,p_xtra_state_info->time_of_last_download);
       gps_total_msec(d_TimeDiff,d_TimeDiffMs);
       if ( (p_xtra_state_info->time_of_last_download > 0) && 
          ((d_TimeDiffMs[0]) > 120000) ) 
       {
         //allow xtra injection if its been more than 2 minutes since last 
         //time of download
         TM_XTRA_MSG_MED("tm_xtra_process_set_data_cmd: Xtra injection already going on. ",0,0,0);
         /* Clear the earlier xtra injection request status */
         /* Clear the time of last download */
         qw_set(p_xtra_state_info->time_of_last_download, 0, 0);
         (void)tm_xtra_update_sess_state(TM_XTRA_STATE_IDLE);
         /* Number of dload request retry attempts */
         p_xtra_state_info->retry_attempts =  TM_XTRA_INIT_RETRY_ATTEMPTS;

         /* Source of Time Information request */
         p_xtra_state_info->time_info_req_src = GPS_TIME_CHECK_SOURCE_NONE;

         /*----------------------------------------------------------------------
         Stop Timers
         ----------------------------------------------------------------------*/  
         (void)os_TimerStop(p_xtra_state_info->auto_dload_timer);
         (void)os_TimerStop(p_xtra_state_info->retry_dload_timer);
         if(p_xtra_state_info->auto_download_enabled 
            && p_xtra_state_info->auto_dload_timer)
         {
           /* Start the timer */
           (void)os_TimerStart(p_xtra_state_info->auto_dload_timer,
                        TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                        0);
         }
         if(!tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ))
         {
            TM_XTRA_MSG_ERROR("tm_xtra_process_set_data_cmd: Unable to change xtra inj state after time-out. ",0,0,0);
            tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);  // -> IDLE
            break;
         }
       }
       else
       {
         TM_XTRA_MSG_MED("Injecting data, process data rejected (state:%d)", p_xtra_state_info->e_state, 0, 0);
           event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_GPS_BUSY;    
           (void)tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, event_payload);
         /* Send gpsOneXTRA dload DM event */
         sm_report_event_xtra_end_dload(XTRA_DOWNLOAD_STATUS_GPS_BUSY);
         break;
       }
     }
     
     TM_XTRA_MSG_HIGH( "Processing set_xtra_data command. status=%d, part=%d, total parts=%d",
                       cmd_ptr->info.xtra_dc_status,
                       cmd_ptr->info.part_number,
                       cmd_ptr->info.total_parts);
  
     // Clear the wait data timer
     (void)os_TimerStop(p_xtra_state_info->retry_dload_timer);  
  
     // make sure file was downloaded successfully
     if (cmd_ptr->info.xtra_dc_status != PDSM_XTRA_STATUS_SUCCESS)
     {
       TM_XTRA_MSG_MED(" ERROR - client cannot download data successfully. Abort ",0,0,0);
       tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);  // -> IDLE
       break;
     }
     
     // check XTRA data pointer
     if(cmd_ptr->info.xtra_data_ptr == NULL)
     {
       TM_XTRA_MSG_ERROR("Received NULL XTRA data pointer in set_data",0,0,0);
       tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);
       break;
     }

     // check if we've exceeded the local buffer size
     if ( p_xtra_state_info->q_XtraDataLen > TM_XTRA_MAX_FILE_SIZE )
     {
       TM_XTRA_MSG_ERROR("Total data %d exceeds max allowed size %d",
                          p_xtra_state_info->q_XtraDataLen,
                          TM_XTRA_MAX_FILE_SIZE,0);
       
       tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_BAD_BUFFER_LENGTH);
       break;
     }

     // if we have all of the parts, process the data now
     if (cmd_ptr->info.part_number == cmd_ptr->info.total_parts)
     {
       /* Update the session state, inject data state */
       if( !tm_xtra_update_sess_state(TM_XTRA_STATE_INJECT_DATA) )
       {
         tm_xtra_abort_and_recover(XTRA_DOWNLOAD_STATUS_FAILURE);  // -> IDLE
         break;
       }
 
       /* Request GPS time, once GPS time is available process the XTRA data */
       ret_val = tm_xtra_data_validity_check(GPS_TIME_CHECK_SOURCE_DATA_INJECT);
       
     }
     else
     {
       /* Send status event to XTRA client */
       event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
    
       (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, event_payload );
       
       /* Update the session state, wait for additional data */
       if( !tm_xtra_update_sess_state(TM_XTRA_STATE_WAIT_DATA) )
       {
         break;
       }
     }

     ret_val = TRUE;
   } while (0);

   return ret_val;
} /* End tm_xtra_process_set_data_cmd() */


/*===========================================================================
FUNCTION tm_xtra_process_initiate_download_cmd

DESCRIPTION
Process the initiate download command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_initiate_download_cmd
(
  pdsm_xtra_cmd_s_type *cmd_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  tm_xtra_event_payload_u_type  event_payload;

  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();
  
  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  

    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    
    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
                                 event_payload );

    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Received NULL cmd_ptr pointer parameter",0,0,0);
    
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    
    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
                                     event_payload );
  }
  else
  {
    /*--------------------------------------------------------------------
      Trigger the download request processing
    --------------------------------------------------------------------*/
    TM_XTRA_MSG_LOW("Processing initiate_xtra_download command. ",0,0,0);

    event_payload.data_dload_payload.force_data_dload = TRUE;
    
    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, 
                                     event_payload );
    
    /* Update the session state */
    if(ret_val)
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_XTRA_CLIENT_REQ);

      if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
      }
    }
    
  } /* end else */

  return ret_val;

} /* End tm_xtra_process_initiate_download_cmd() */


/*===========================================================================
FUNCTION tm_xtra_process_set_auto_download_cmd

DESCRIPTION
Process the set auto download command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_set_auto_download_cmd
(
  pdsm_xtra_cmd_s_type *cmd_ptr
)
{
   /* Local Variables */
   boolean                       ret_val = FALSE;
   tm_xtra_event_payload_u_type  event_payload;
   nv_item_type                  nv_write_buffer;

   tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

   if(xtra_state_ptr == NULL)
   {
      MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  

      event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;

      (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
         event_payload );

      return FALSE;
   }

   /*-----------------------------------------------------------------------
   Check input parameters
   -----------------------------------------------------------------------*/
   if (cmd_ptr == NULL)
   {
      TM_XTRA_MSG_ERROR("Received NULL cmd_ptr pointer parameter",0,0,0);

      event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;

      ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
         event_payload );
   }
   else
   {
      /*--------------------------------------------------------------------
      Set the auto download parameters
      --------------------------------------------------------------------*/
      TM_XTRA_MSG_LOW("Processing set_auto_download command. ",0,0,0);

      /* set download interval */
      xtra_state_ptr->download_interval  
         = cmd_ptr->info.download_interval;

      nv_write_buffer.gps1_xtra_download_interval 
         = cmd_ptr->info.download_interval;

      (void)tm_core_nv_write( NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I, 
         (void*)&nv_write_buffer );

      /* enable/disable auto downloads */
      xtra_state_ptr->auto_download_enabled
         = cmd_ptr->info.enable_auto_download;

      nv_write_buffer.gps1_xtra_auto_download_enabled 
         = cmd_ptr->info.enable_auto_download;

      (void)tm_core_nv_write( NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I, 
         (void*)&nv_write_buffer );


      /* Start the Auto download timer */
      if(xtra_state_ptr->auto_download_enabled)
      {
         /* Start the timer */
         (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
            TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
            0);
      }
      else
      {
         /* Stop the timer */
         (void)os_TimerStop(xtra_state_ptr->auto_dload_timer);
      }

      event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;

      (void) tm_xtra_trigger_event(TM_XTRA_EVENT_TYPE_STATUS, event_payload);
      ret_val =  TRUE;
   } /* end else */

   return ret_val;
} /* End tm_xtra_process_set_auto_download_cmd() */


/*===========================================================================
FUNCTION tm_xtra_process_query_data_validity_cmd

DESCRIPTION
Process the query data validity command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_query_data_validity_cmd
(
  pdsm_xtra_cmd_s_type *cmd_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  tm_xtra_event_payload_u_type  event_payload;

  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Received NULL cmd_ptr pointer parameter",0,0,0);

    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;

    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
                                     event_payload );
  }
  else
  {
    /*--------------------------------------------------------------------
      Trigger the download request processing
    --------------------------------------------------------------------*/
    TM_XTRA_MSG_LOW("Processing query_data_validity command. ",0,0,0);

    event_payload.data_query_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
    
    ret_val = tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_QUERY_DATA_VALID, 
                                     event_payload );       
  } /* end else */

  return ret_val;    
} /* End tm_xtra_process_query_data_validity_cmd() */


/*===========================================================================
FUNCTION tm_xtra_process_inject_time_info

DESCRIPTION
Process the inject time info command request from XTRA client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_process_inject_time_info
(
  pdsm_xtra_cmd_s_type *cmd_ptr
)
{
  /* Local Variables */
  tm_xtra_event_payload_u_type  event_payload;
  lm_request_info_s_type    lm_info_req_param;
  pdsm_xtra_cmd_info_s_type *p_xtra_info = NULL;
  uint16 w_GpsWeek;
  uint32 u_GpsMsec;
  boolean ret_val = FALSE;
  
  p_xtra_info = &cmd_ptr->info;

  /*-----------------------------------------------------------------------
     Check input parameters
  -----------------------------------------------------------------------*/
  if (cmd_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("Null cmd_ptr",0,0,0);
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );
    return FALSE;
  }

  if (p_xtra_info->cmd_err != PDSM_XTRA_CMD_ERR_NOERR)
  {
    TM_XTRA_MSG_ERROR("Time injection command error found", 0, 0, 0);
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
    (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );
    return FALSE;
  }

  TM_XTRA_MSG_MED("Processing inject_time_info command. force %d",
                  p_xtra_info->b_ForceFlag,0,0);
  w_GpsWeek = (uint16) ( p_xtra_info->d_TimeMsec / ( TM_XTRA_GPS_SECONDS_IN_WEEK * 1000.0) );
  u_GpsMsec = (uint32) ( p_xtra_info->d_TimeMsec - w_GpsWeek* ( TM_XTRA_GPS_SECONDS_IN_WEEK * 1000.0));
  
  if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
  {
    char u_string[100];
    (void) snprintf(u_string, sizeof(u_string), "=TM XTRA= Time injection: %0.3lf(Week: %d,Msec: %ld), unc %0.3f, utc %d",
                                          p_xtra_info->d_TimeMsec,
                                          w_GpsWeek,
                                          u_GpsMsec,
                                          p_xtra_info->f_TimeUncMsec, 
                                          p_xtra_info->b_RefToUtcTime);
    MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", u_string);
  }

  if (!p_xtra_info->b_ForceFlag && (w_GpsWeek < TM_XTRA_MIN_ACCEPTABLE_GPS_WEEK_FOR_INJECTION) )
  {
     TM_XTRA_MSG_ERROR("Invalid GPS week (%u) in XTRA time injection",w_GpsWeek,0,0);
     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
     (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );
     return FALSE;
  }
  
  
  lm_info_req_param.e_req_type = LM_REQUEST_SLOW_CLK_TIME_INJECT;

  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.t_Milliseconds = (uint64) p_xtra_info->d_TimeMsec; 
  /*Increase the uncertainty to 3.5sec if less than 3.5sec is injected in-order to prevent pattern-match in MGP*/
  if(p_xtra_info->f_TimeUncMsec < TM_XTRA_MIN_INJ_TIME_UNC)
  {
    lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.q_Uncertainty = (uint32)TM_XTRA_MIN_INJ_TIME_UNC;
  }
  else
  {
    lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.q_Uncertainty = (uint32)p_xtra_info->f_TimeUncMsec;
  }
  
  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.e_TimeSource = MGP_SLOW_CLK_SRC_XTRA; 
  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.b_TimeIsGps = !p_xtra_info->b_RefToUtcTime; 
  lm_info_req_param.z_request.z_slow_clk_inject_request.e_source = 
        p_xtra_info->b_RefToUtcTime ? GNSS_SLOW_CLOCK_SRC_UTC 
                                    : GNSS_SLOW_CLOCK_SRC_GPS;
  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.u_DiscontinuityCnt = 0; 
  lm_info_req_param.z_request.z_slow_clk_inject_request.z_slow_clk_time.b_ForceFlag = p_xtra_info->b_ForceFlag; 

  /* Inject time to MGP through LM */
  ret_val = lm_request(&lm_info_req_param);

  if(ret_val)
  {
     TM_XTRA_MSG_MED("Time injection to LM/MGP successful",0,0,0); 
     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
     (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );
     /*Now Request the time from MGP*/
     (void) tm_xtra_data_validity_check(GPS_TIME_CHECK_SOURCE_TIME_INJECT);
  }
  else
  {
     TM_XTRA_MSG_ERROR("Time injection to LM/MGP failed", 0, 0, 0);  
     event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_FAILURE;
     (void) tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS,event_payload );
  }

  return ret_val;
} /* End tm_xtra_process_inject_time_info() */


/*===========================================================================
=============================================================================

   TM-XTRA: PD API Event Handlers
   
=============================================================================
===========================================================================*/


/*===========================================================================
FUNCTION tm_xtra_get_status_event_info

DESCRIPTION
Fill the status event parameters to be deliverd to the XTRA clients

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_status_event_info
(
  pdsm_xtra_info_s_type          *info_ptr,
  tm_xtra_status_payload_s_type  *payload_ptr
)
{

  /* Local Variables */
  boolean ret_val = FALSE;

  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (info_ptr == NULL)
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
  }
  else
  {  
    /*-----------------------------------------------------------------------
        Fill in the valid XTRA information for the STATUS event
    -----------------------------------------------------------------------*/
    info_ptr->xtra_event = PDSM_XTRA_EVENT_STATUS;
    
    /* Set the valid bits flag in download_info struct */
    info_ptr->xtra_union.xtra_download_info.flags 
      = XTRA_DOWNLOAD_INFO_FLAGS_STATUS;
  
    /* set status value */
    info_ptr->xtra_union.xtra_download_info.status = payload_ptr->status;

    ret_val = TRUE;
  }
  
  return ret_val;
} /* End tm_xtra_get_status_event_info() */


/*===========================================================================
FUNCTION tm_xtra_get_data_download_event_info

DESCRIPTION
Fill the download request event parameters to be deliverd to the XTRA clients

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_data_download_event_info
(
  pdsm_xtra_info_s_type              *info_ptr,
  tm_xtra_data_dload_payload_s_type  *payload_ptr
)
{
  /* Local Variables */
  boolean              ret_val = FALSE;
  uint8 *InUrl_ptr[3], *OutUrl_ptr[3];
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  
  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (info_ptr == NULL)
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
    return ret_val;
  }

  
  do{
    /*-----------------------------------------------------------------------
        Check current XTRA module state
    -----------------------------------------------------------------------*/
    if ( FALSE == tm_xtra_is_enable() )
    {
      TM_XTRA_MSG_MED("Feature disabled",0,0,0);
      break;
    }
    else if ( tm_xtra_is_download_in_progress() )
    {
      TM_XTRA_MSG_MED("Trigger download.  Download currently in progress - no download request sent to client. Force %d",
                       payload_ptr->force_data_dload,0,0);
      break;
    }
    else if ( tm_xtra_is_download_req_pending() )
    {
      /* Do not force the download request if already pending */
      if (  FALSE == payload_ptr->force_data_dload )
      {
        MSG_MED("Trigger download.  Download already pending - no download request sent to client. Force %d",
                 payload_ptr->force_data_dload,0,0);
        break;
      }
    }

    
    /*-----------------------------------------------------------------------
        Client is allowed start download
        Fill in the valid XTRA information for the DOWNLOAD_REQ event
    -----------------------------------------------------------------------*/
    TM_XTRA_MSG_HIGH("Trigger download - sending data download request to client. Force %d",
                      payload_ptr->force_data_dload,0,0);

    /* Send the Download request event */
    info_ptr->xtra_event = PDSM_XTRA_EVENT_DOWNLOAD_REQ;


    /*-----------------------------------------------------------------------
       Populate the payload for download reqest
    -----------------------------------------------------------------------*/
    /* Set command */
    info_ptr->xtra_union.xtra_download_info.command = XTRA_COMMAND_RETRIEVE_FILE;

    /* Set the valid bits flag in download_info struct */
    info_ptr->xtra_union.xtra_download_info.flags 
                                =   (XTRA_DOWNLOAD_INFO_FLAGS_COMMAND) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_SERVER1) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_SERVER2) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_SERVER3) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_MAX_FILE_PART_SIZE) \
                                  | (XTRA_DOWNLOAD_INFO_FLAGS_MAX_FILE_SIZE);

    /* Set status value */
    info_ptr->xtra_union.xtra_download_info.status = XTRA_DOWNLOAD_STATUS_MAX;

    /* Randomize and set server URLs */
    InUrl_ptr[0] = (uint8 *)xtra_state_ptr->primary_data_server_url;
    InUrl_ptr[1] = (uint8 *)xtra_state_ptr->secondary_data_server_url;
    InUrl_ptr[2] = (uint8 *)xtra_state_ptr->tertiary_data_server_url;
  
    OutUrl_ptr[0] = (uint8 *)info_ptr->xtra_union.xtra_download_info.xtra_server_primary;
    OutUrl_ptr[1] = (uint8 *)info_ptr->xtra_union.xtra_download_info.xtra_server_secondary;
    OutUrl_ptr[2] = (uint8 *)info_ptr->xtra_union.xtra_download_info.xtra_server_tertiary;
  
    tm_xtra_randomize_and_set_urls(InUrl_ptr, OutUrl_ptr);
         
#ifdef FEATURE_GNSS_XTRA3 
    tm_xtra3_set_download_url(OutUrl_ptr, sizeof(OutUrl_ptr)/sizeof(OutUrl_ptr[0]), TM_XTRA_MAX_SERVER_URL_SIZE);
#endif /* FEATURE_GNSS_XTRA3 */
                 
    /* Set file block transfer information */
    info_ptr->xtra_union.xtra_download_info.maxFilePartSize = TM_XTRA_MAX_FILE_PART_SIZE;
    info_ptr->xtra_union.xtra_download_info.maxFileSize = TM_XTRA_MAX_FILE_SIZE;


    /*-----------------------------------------------------------------------
        Get XTRA time and duration validy info from Almanac corrections data
    -----------------------------------------------------------------------*/
    if(FALSE == tm_xtra_set_xtra_time_and_validity(&info_ptr->xtra_union.xtra_download_info))
    {
      ret_val = FALSE;
      TM_XTRA_MSG_ERROR("set_xtra_time_and_validity failed",0,0,0);
      break;
    }

 
#ifdef FEATURE_CGPS_XTRA_DEBUG
    /* Debug message: Print out the parameters sent to the client application. */
    snprintf(pchAsciiOutput, 
             sizeof(pchAsciiOutput), 
             "Sending XTRA download request. URLs:%s,%s,%s\n",
             info_ptr->xtra_union.xtra_download_info.xtra_server_primary,
             info_ptr->xtra_union.xtra_download_info.xtra_server_secondary,
             info_ptr->xtra_union.xtra_download_info.xtra_server_tertiary);
    TM_XTRA_MSG_LOW(pchAsciiOutput,0,0,0);

#endif /* FEATURE_CGPS_XTRA_DEBUG */

  
    ret_val = TRUE;
    
  } while (0);
  
  return ret_val;
} /* End tm_xtra_get_data_download_event_info() */


/*===========================================================================
FUNCTION tm_xtra_get_time_download_event_info

DESCRIPTION
Fill the time info download request event parameters to be deliverd to 
the XTRA clients

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_time_download_event_info
(
  pdsm_xtra_info_s_type               *info_ptr,
  tm_xtra_time_dload_payload_s_type   *payload_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  uint8 *InUrl_ptr[3], *OutUrl_ptr[3];
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  
  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return FALSE;
  }

  
  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (info_ptr == NULL)
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
    return ret_val;
  }

  
  do{
    /*-----------------------------------------------------------------------
        Check current XTRA module state
    -----------------------------------------------------------------------*/
    if (    !tm_xtra_is_enable() 
         || !tm_xtra_is_time_info_enable() )
    {
      TM_XTRA_MSG_MED("Feature disabled",0,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
        Client is allowed start download
        Fill in the valid XTRA information for the TIME_REQ event
    -----------------------------------------------------------------------*/
    TM_XTRA_MSG_HIGH("Trigger download - sending time info request to client. Force %d",
                      payload_ptr->force_time_info_dload,0,0);

    /* Send the time info request event */
    info_ptr->xtra_event = PDSM_XTRA_EVENT_TIME_REQ;

    /*-----------------------------------------------------------------------
       Populate the payload for download reqest
    -----------------------------------------------------------------------*/
    /* Set command */
    info_ptr->xtra_union.xtra_time_info.command = XTRA_COMMAND_RETRIEVE_TIME_INFO;

    /* Randomize and set server URLs */
    InUrl_ptr[0] = (uint8 *)xtra_state_ptr->primary_sntp_server_url;
    InUrl_ptr[1] = (uint8 *)xtra_state_ptr->secondary_sntp_server_url;
    InUrl_ptr[2] = (uint8 *)xtra_state_ptr->tertiary_sntp_server_url;

    OutUrl_ptr[0] = (uint8 *)info_ptr->xtra_union.xtra_time_info.xtra_server_primary;
    OutUrl_ptr[1] = (uint8 *)info_ptr->xtra_union.xtra_time_info.xtra_server_secondary;
    OutUrl_ptr[2] = (uint8 *)info_ptr->xtra_union.xtra_time_info.xtra_server_tertiary;

    tm_xtra_randomize_and_set_urls(InUrl_ptr, OutUrl_ptr);
    
    /* Set one-way delay failover threshold */
    info_ptr->xtra_union.xtra_time_info.oneway_delay_failover_thresh
                             = xtra_state_ptr->time_info_oneway_delay_thresh;

#ifdef FEATURE_CGPS_XTRA_DEBUG
    /* Debug message: Print out the parameters sent to the client application. */
    snprintf(pchAsciiOutput, 
             sizeof(pchAsciiOutput),
             "Request time info - Thresh=%ld, URLs:%s,%s,%s\n",
             info_ptr->xtra_union.xtra_time_info.oneway_delay_failover_thresh,
             info_ptr->xtra_union.xtra_time_info.xtra_server_primary,
             info_ptr->xtra_union.xtra_time_info.xtra_server_secondary,
             info_ptr->xtra_union.xtra_time_info.xtra_server_tertiary);
    TM_XTRA_MSG_LOW(pchAsciiOutput,0,0,0);
#endif /* FEATURE_CGPS_XTRA_DEBUG */

  
    ret_val = TRUE;
    
  } while (0);
    
  return ret_val;
} /* End tm_xtra_get_time_download_event_info() */


/*===========================================================================
FUNCTION tm_xtra_get_query_data_valid_event_info

DESCRIPTION
Fill the query valid request info to be sent to XTRA clients

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_get_query_data_valid_event_info
(
  pdsm_xtra_info_s_type               *info_ptr,
  tm_xtra_data_query_payload_s_type   *payload_ptr
)
{
  /* Local Variables */
  boolean              ret_val = FALSE;

  /*-----------------------------------------------------------------------
      Check input parameters
  -----------------------------------------------------------------------*/
  if (info_ptr == NULL)
  {
    TM_XTRA_MSG_HIGH("Received NULL pointer parameter",0,0,0);
    return ret_val;
  }


  do
  {
    /*-----------------------------------------------------------------------
        Check current XTRA module state
    -----------------------------------------------------------------------*/
    if ( !tm_xtra_is_enable() )
    {
      TM_XTRA_MSG_MED("Feature disabled",0,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
        Client is allowed start download
        Fill in the valid XTRA information for the TIME_REQ event
    -----------------------------------------------------------------------*/
    TM_XTRA_MSG_HIGH("Trigger query data request - sending info to client(status:%d)",
                      payload_ptr->status,0,0);

    /* Send the time info request event */
    info_ptr->xtra_event = PDSM_XTRA_EVENT_STATUS;


    /*-----------------------------------------------------------------------
       Populate the payload for query data validity request
    -----------------------------------------------------------------------*/
    /* Set the valid bits flag in download_info struct */
    info_ptr->xtra_union.xtra_download_info.flags 
                               = (XTRA_DOWNLOAD_INFO_FLAGS_STATUS) \
                               | (XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK) \
                               | (XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS) \
                               | (XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION);

    /* Set status value */
    info_ptr->xtra_union.xtra_download_info.status = payload_ptr->status;

    /* Initialize the URL data */
    memset((void*)info_ptr->xtra_union.xtra_time_info.xtra_server_primary,
           0x0,
           TM_XTRA_MAX_SERVER_URL_SIZE);

    memset((void*)info_ptr->xtra_union.xtra_time_info.xtra_server_secondary,
           0x0,
           TM_XTRA_MAX_SERVER_URL_SIZE);

    memset((void*)info_ptr->xtra_union.xtra_time_info.xtra_server_tertiary,
           0x0,
           TM_XTRA_MAX_SERVER_URL_SIZE);

    /* Set file block transfer information */
    info_ptr->xtra_union.xtra_download_info.maxFilePartSize = TM_XTRA_NULL_FILE_SIZE;
    info_ptr->xtra_union.xtra_download_info.maxFileSize = TM_XTRA_NULL_FILE_SIZE;


    /*-----------------------------------------------------------------------
        Get XTRA time and duration validy info from Almanac corrections data
    -----------------------------------------------------------------------*/
    if(FALSE == tm_xtra_set_xtra_time_and_validity(&info_ptr->xtra_union.xtra_download_info))
    {
       ret_val = FALSE;
       TM_XTRA_MSG_ERROR("set_xtra_time_and_validity failed",0,0,0);
       break;
    }
  
    ret_val = TRUE;

  } while (0);

  return ret_val;
} /* End tm_xtra_get_query_data_valid_event_info()*/


/*===========================================================================
FUNCTION tm_xtra_trigger_event

DESCRIPTION
Send the specified XTRA event to the client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_trigger_event
(
  tm_xtra_event_e_type          xtra_event,
  tm_xtra_event_payload_u_type  event_payload
)
{
  /* Local variables */
  boolean                ret_val = TRUE;
  pdsm_xtra_info_s_type  u_xtra_info = {0};

  /*-----------------------------------------------------------------------
      Process the incoming XTRA event request
  -----------------------------------------------------------------------*/
  switch ( xtra_event )
  { 
    case TM_XTRA_EVENT_TYPE_STATUS:
      ret_val = tm_xtra_get_status_event_info( &u_xtra_info, 
                                               &event_payload.status_payload);
      break;
    case TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD:

      ret_val = FALSE;
      /*Forced downloads are exempt from blocking */
      
      if ( (tm_xtra_is_req_blocked() == FALSE) || (event_payload.data_dload_payload.force_data_dload == TRUE) )
      {
      /* During A-GPS session do not send XTRA download request */
      if( tm_core_is_xtra_operation_allowed() )
      {
        ret_val = tm_xtra_get_data_download_event_info( &u_xtra_info, 
                                                        &event_payload.data_dload_payload );
           TM_XTRA_MSG_HIGH("Sending XTRA Download Request, force(%d)", event_payload.data_dload_payload.force_data_dload,0,0);
           /* Throttle On Demand data requests. Unblock on successful injection or retry timer */
           if( (ret_val == TRUE) && (event_payload.data_dload_payload.force_data_dload==FALSE) )
           {
              tm_xtra_req_block();
           }   
      }
      else
      {
        TM_XTRA_MSG_ERROR("Active A-GPS session, download event is blocked",0,0,0);
         }
      }
      else
      {
          TM_XTRA_MSG_HIGH("XTRA Download Request Blocked", 0,0,0);
      }
      
      break;
      
    case TM_XTRA_EVENT_TYPE_REQ_TIME: 
      ret_val = tm_xtra_get_time_download_event_info( &u_xtra_info,
                                                      &event_payload.time_dload_payload );
      break;
    case TM_XTRA_EVENT_TYPE_REQ_QUERY_DATA_VALID:
      ret_val = tm_xtra_get_query_data_valid_event_info( &u_xtra_info,
                                                         &event_payload.data_query_payload );    
      break;
    default:
      TM_XTRA_MSG_ERROR("TM xtra event (%d) not handled", xtra_event,0,0);
      ret_val = FALSE;
      break;
  }

  /* Only for known XTRA events and valid parameters, 
  ** send request to XTRA client 
  */
  if (ret_val)
  {
    TM_XTRA_MSG_MED( "Sending event %d", u_xtra_info.xtra_event,0,0);

    tm_pdapi_xtra_event_callback(u_xtra_info.xtra_event, &u_xtra_info);
  }

  return ret_val;
  
} /* End tm_xtra_trigger_event() */

/*===========================================================================
FUNCTION tm_xtra_start_process_data_info

DESCRIPTION
Start the data injection to MGP, first process the gpsOneXTRA 
file

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_start_process_data_info
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  pdsm_xtra_status_e_type       ret_val = XTRA_DOWNLOAD_STATUS_FAILURE;

  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return;
  }
  
  /*-----------------------------------------------------------------------
    Process and the inject the gpsOneXTRA data
  -----------------------------------------------------------------------*/

  /* Process and inject the gpsOneXTRA data file */
  ret_val = tm_xtra_process_and_inject_data( xtra_state_ptr->xtraTempBuf, 
                                             xtra_state_ptr->q_XtraDataLen);

  /*-----------------------------------------------------------------------
     Send the gpsOneXTRA data download status to client
  -----------------------------------------------------------------------*/
  if (XTRA_DOWNLOAD_STATUS_SUCCESS == ret_val)
  {
    /* Memory buffer to store XTRA.BIN data from XTRA Client */
#ifdef FEATURE_CGPS_XTRA_DYNAMIC_ALLOC
    if (xtra_state_ptr->xtraTempBuf != NULL)
    {
    os_MemFree((void**)&xtra_state_ptr->xtraTempBuf);
    xtra_state_ptr->xtraTempBuf = NULL;
    }
#else
    memset((void*)xtra_state_ptr->xtraTempBuf,
           0x0,
           TM_XTRA_MAX_FILE_SIZE);
#endif
    
    /* Initialize the data length for next file injection */
    xtra_state_ptr->q_XtraDataLen = 0;

    /* Save time of last download */
    (void)time_get(xtra_state_ptr->time_of_last_download);

    /* Send Ind to CD since Xtra injection has completed*/
    lm_send_xtra_done_msg();

#ifdef FEATURE_CGPS_XTRA_T
    /* will not trigger XTRA-T if XTRA is triggered by AGPS(SUPL/V2 etc) */
    if (tm_core_info.session_info.req_xtra_trig_by_agps == FALSE)
    {
      /* Let TLE know a XTRA download session completes. TLE will trigger
         a XTRA-T upload/download session.
       */
      (void) tle_RequestSessionByXtra(tm_core_is_standalone_active());
    }
    else
    {
      /* reset the "XTRA triggered by AGPS" flag */
      tm_core_info.session_info.req_xtra_trig_by_agps = FALSE;
    }
#endif

    tm_xtra_req_allow(); 
  }
  else
  {
    /* Cleanup TM-XTRA: Error scenario, data download failed */
    xtra_state_ptr->continue_blocking = TRUE;
    tm_xtra_abort_and_recover(ret_val);
  }

  return;

} /* End tm_xtra_start_process_data_info() */


/*===========================================================================
FUNCTION tm_xtra_check_time_uncertainty

DESCRIPTION
Check the current time uncertainty and fire the time info request if 
necessary

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
boolean - if time was requested from XTRA
===========================================================================*/
boolean tm_xtra_check_time_uncertainty
(
  void
)
{
  /* Local Variables */
  tm_xtra_event_payload_u_type  event_payload;

  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Only perform the check if the NV item is enabled
  -----------------------------------------------------------------------*/
  if( !tm_xtra_is_time_info_enable() )
  {
    TM_XTRA_MSG_ERROR("Time info feature is disabled",0,0,0);
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
    If the GPS time is unknown or time uncertainty is great than a threshold,
    send a time info request to the client
  -----------------------------------------------------------------------*/
  if((xtra_state_ptr->gps_time.w_GpsWeek == C_GPS_WEEK_UNKNOWN) ||  
   (xtra_state_ptr->gps_time.f_ClkTimeUncMs 
            >= xtra_state_ptr->time_info_uncertainty_thresh) )
  {
    /*Do not send another XTRA time request if we are already processing one*/  
    if(!(xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_TIME_INJECT))
    {
      /*---------------------------------------------------------------------
         Send the gpsOneXTRA time info download request
      ---------------------------------------------------------------------*/
      event_payload.time_dload_payload.force_time_info_dload = TRUE;
  
      (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_TIME, 
                                   event_payload );

      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION);

      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }

} /* End tm_xtra_check_time_uncertainty() */

/*===========================================================================
FUNCTION tm_xtra_start_process_time_info

DESCRIPTION
Start the time info injection to MGP, first process the gpsOneXTRA 
time info

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_start_process_time_info
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  tm_xtra_data_check_result_e_type  ret_val = TM_XTRA_DATA_CHECK_ERROR;
  tm_xtra_event_payload_u_type      event_payload;
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return;
  } 
  
  /*-----------------------------------------------------------------------
    Check time uncertainty, may send time info request to clients
  -----------------------------------------------------------------------*/  
  if(!tm_xtra_check_time_uncertainty())
  {
    ret_val = tm_xtra_data_check_handler(GPS_TIME_CHECK_SOURCE_TIME_INJECT);

    /*-----------------------------------------------------------------------
      Send the XTRA download request to client if data expired
    -----------------------------------------------------------------------*/
    if (ret_val == TM_XTRA_DATA_CHECK_ERROR)
    {
      TM_XTRA_MSG_HIGH("XTRA data invalid/expired at time inject, request data download",0,0,0);

      event_payload.data_dload_payload.force_data_dload = FALSE;

      /* Send the download request event, update the session state */
      if( TRUE ==
               tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, event_payload ) 
        )
      {
        /* Send gpsOneXTRA dload DM event */
        sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_FIX_REQUEST);
        
        if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
        {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
        }
      }
    }
  }

  return;
} /* End tm_xtra_start_process_time_info() */



/*===========================================================================
FUNCTION tm_xtra_start_process_auto_dload_check

DESCRIPTION
Check the automatic periodic download parameters to determine if a new 
gpsOneXTRA data download is required.  Send data download request
event to client if necessary.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_start_process_auto_dload_check
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  tm_xtra_data_check_result_e_type  ret_val = TM_XTRA_DATA_CHECK_ERROR;
  tm_xtra_event_payload_u_type      event_payload;
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return;
  }

  /*-----------------------------------------------------------------------
    Process and the inject the gpsOneXTRA data
  -----------------------------------------------------------------------*/

  /* Check the header validity */
  ret_val = tm_xtra_data_check_handler( source );
      
  /*-----------------------------------------------------------------------
    Send the XTRA download request to client if time expired
  -----------------------------------------------------------------------*/
  if (ret_val == TM_XTRA_DATA_CHECK_ERROR)
  {
    TM_XTRA_MSG_HIGH("Automatic download period expired, request data download",0,0,0);
    
    event_payload.data_dload_payload.force_data_dload = FALSE;

    /* Send the download request event, update the session state */
    if( TRUE ==
           tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, event_payload ) 
      )
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_AUTO);

      if( !tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        TM_XTRA_MSG_ERROR("Automatic download period expired, session state update failed",0,0,0);
      }
      else
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
          (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
          0);
      }
    }
  }
  else if(ret_val == TM_XTRA_DATA_CHECK_INVALID_TIME)
  {
    /*---------------------------------------------------------------------
    Send the gpsOneXTRA time info download request
    ---------------------------------------------------------------------*/
    event_payload.time_dload_payload.force_time_info_dload = TRUE;

    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_TIME, 
                                 event_payload );  

    /* Start the automatic download timer */
    (void)os_TimerStart(xtra_state_ptr->auto_dload_timer,
                        TM_XTRA_AUTO_DLOAD_CHECK_TIME, 
                        0);

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION);
  }

  return;

} /* End tm_xtra_start_process_auto_dload_check()*/


/*===========================================================================
FUNCTION tm_xtra_start_process_per_fix_check

DESCRIPTION
Check the automatic periodic download parameters to determine if a new 
gpsOneXTRA data download is required.  Send data download request
event to client if necessary.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_start_process_per_fix_check
(
  const tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  tm_xtra_data_check_result_e_type  ret_val = TM_XTRA_DATA_CHECK_ERROR;
  tm_xtra_event_payload_u_type      event_payload;
  
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();

  /*-----------------------------------------------------------------------
    Check the state ptr
  -----------------------------------------------------------------------*/  
  if (xtra_state_ptr == NULL)
  {
    TM_XTRA_MSG_ERROR("NULL pointer error",0,0,0);
    return;
  }

  /*-----------------------------------------------------------------------
    Process and the inject the gpsOneXTRA data
  -----------------------------------------------------------------------*/
  /* Check the header validity */
  ret_val = tm_xtra_data_check_handler( source );
  
  /*-----------------------------------------------------------------------
    Send the XTRA download request to client if time expired
  -----------------------------------------------------------------------*/
  if (ret_val == TM_XTRA_DATA_CHECK_ERROR)
  {
    TM_XTRA_MSG_HIGH("XTRA data invalid/expired, request data download",0,0,0);
    
    event_payload.data_dload_payload.force_data_dload = FALSE;
    
    /* Send the download request event, update the session state */
    if( TRUE ==
            tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_DATA_DOWNLOAD, event_payload ) 
      )
    {
      /* Send gpsOneXTRA dload DM event */
      sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_FIX_REQUEST);
 
      if( tm_xtra_update_sess_state(TM_XTRA_STATE_CLIENT_REQ) )
      {
        /* Start the recover timer */
        (void)os_TimerStart(xtra_state_ptr->retry_dload_timer,
                            (xtra_state_ptr->time_between_attempts*TM_XTRA_MSEC_IN_MIN), 
                            0);
      }
    }
  }
  else if(ret_val == TM_XTRA_DATA_CHECK_INVALID_TIME)
  {
    /*---------------------------------------------------------------------
    Send the gpsOneXTRA time info download request
    ---------------------------------------------------------------------*/
    event_payload.time_dload_payload.force_time_info_dload = TRUE;

    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_REQ_TIME, 
                                 event_payload );  

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_start_dload(XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION);
  }
#ifdef FEATURE_CGPS_XTRA_T
  else if (ret_val == TM_XTRA_DATA_CHECK_SUCCESS)
  {
    MSG_HIGH("XTRA data/time valid. Ask TLE for Pos Unc for XTRA-T if necessary",0,0,0);
    MSG_HIGH("Request TLE reports cell position unc",0,0,0);
    (void) tle_GetTerrPosStatus();
  }
#endif /* FEATURE_CGPS_XTRA_T */

  return;

} /* End tm_xtra_start_process_per_fix_check()*/


/*===========================================================================
FUNCTION tm_xtra_gps_time_report_handler

DESCRIPTION
Handle the GPS time info from MGPS

Starts the following processes:
1) gpsOneXTRA data or time from clients
2) automatic download file time check
3) GPS fix request to check data validity

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
void tm_xtra_gps_time_report_handler
(
  gps_TimeStructType     *p_gps_time
)
{
  /* Local Variables */
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  
  if(xtra_state_ptr == NULL)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);  
    return;
  }
  
  /*-----------------------------------------------------------------------
    Check input parameters
  -----------------------------------------------------------------------*/  
  if (p_gps_time == NULL)
  {
    TM_XTRA_MSG_ERROR("p_gps_time pointer is NULL",0,0,0);
    return;
  }

  /*-----------------------------------------------------------------------
      Process data validity check request
  -----------------------------------------------------------------------*/
  /* Save the GPS time from MGP */
  xtra_state_ptr->gps_time = *p_gps_time;

  /*-----------------------------------------------------------------------
      Process data validity check request
  -----------------------------------------------------------------------*/
  /* Data injection check request */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_DATA_INJECT )
  {
    tm_xtra_start_process_data_info(GPS_TIME_CHECK_SOURCE_DATA_INJECT);
  }

  /* Periodic data validity check request */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_PERIODIC )
  {
    tm_xtra_start_process_auto_dload_check(GPS_TIME_CHECK_SOURCE_PERIODIC);
  }
      
  /* Per fix data validity check request */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_PER_FIX )
  {
    tm_xtra_start_process_per_fix_check(GPS_TIME_CHECK_SOURCE_PER_FIX);
  }

  /* Time injection check request */
  if ( xtra_state_ptr->time_info_req_src & GPS_TIME_CHECK_SOURCE_TIME_INJECT )
  {
    tm_xtra_start_process_time_info(GPS_TIME_CHECK_SOURCE_TIME_INJECT);
  }

  
  /*-----------------------------------------------------------------------
     Reset the time info req mask
  -----------------------------------------------------------------------*/
  xtra_state_ptr->time_info_req_src = GPS_TIME_CHECK_SOURCE_NONE;
  
  return;
} /* End tm_xtra_gps_time_report_handler()*/


/*===========================================================================
FUNCTION tm_xtra_data_validity_check

DESCRIPTION
Request MGP for GPS time, must wait for response from MGP before validating
the gpsOneXTRA data/time. Once the MGP provides the GPS time, TM-Core will
call tm_xtra_gps_time_report_handler().


INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
boolean tm_xtra_data_validity_check
(
  tm_xtra_data_check_source_e_type     source
)
{
  /* Local Variables */
  boolean                   ret_val = FALSE;
  lm_request_info_s_type    lm_info_req_param;

  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
    
  /* Determine if gpsOneXTRA is enabled
  */
  if( tm_xtra_is_enable() )
  {

    /* Save the current time info request source */
    xtra_state_ptr->time_info_req_src |= source;

    if(xtra_state_ptr->time_info_req_src != GPS_TIME_CHECK_SOURCE_NONE)
    {
      /*---------------------------------------------------------------------
        Request the current GPS time from MGP, place TM-XTRA in wait state
      ---------------------------------------------------------------------*/
      lm_info_req_param.e_req_type = LM_REQUEST_GNSS_TIME;
    
      lm_info_req_param.z_request.z_gps_time_request.e_time_chk_type = source; 
     
      /* Send request to LM->MGP */
      ret_val = lm_request(&lm_info_req_param);
    }
    else
    {
      /* Time info request already sent to MGP */
      ret_val = TRUE;
    }
  }
  
  return ret_val;
} /* End tm_xtra_data_validity_check() */


/*===========================================================================
FUNCTION tm_xtra_inj_done

DESCRIPTION
This function will mark as an indication from CD of the completion of Xtra 
injection. Once this message is received, we send a trigger out to the 
client indication "Xtra injection completed" also stop the retry download 
timer and change the state of the system to TM_XTRA_STATE_IDLE
 
INPUT PARAMS
pdsm_xtra_status_e_type - Status of xtra injection

OUTPUT PARAMS
None

RETURN VALUE 
None
===========================================================================*/
void tm_xtra_inj_done(void)
{
  tm_xtra_event_payload_u_type  event_payload;
  tm_xtra_state_type   *xtra_state_ptr =  tm_xtra_get_state_ptr();
  // Only if SM is in the INJECT state then reset its state on 
  //receiving this ACK. 
  /* Clear the time of last download */
  qw_set(xtra_state_ptr->time_of_last_download, 0, 0);

  if(xtra_state_ptr->e_state == TM_XTRA_STATE_INJECT_DATA)
  {
    /*----------------------------------------------------------------------
     Stop Timers
    ----------------------------------------------------------------------*/  
    (void)os_TimerStop(xtra_state_ptr->retry_dload_timer);

    /* Update the session state */
    (void) tm_xtra_update_sess_state(TM_XTRA_STATE_IDLE);
    /* Send the status event to the client */
    event_payload.status_payload.status = XTRA_DOWNLOAD_STATUS_SUCCESS;
    
    (void)tm_xtra_trigger_event( TM_XTRA_EVENT_TYPE_STATUS, 
                                 event_payload );

    /* Send gpsOneXTRA dload DM event */
    sm_report_event_xtra_end_dload(XTRA_DOWNLOAD_STATUS_SUCCESS);
  }
}

/*===========================================================================
FUNCTION tm_xtra_invoke_xtrat

DESCRIPTION
This function: 
  1- Verifies to see if it is ok to initaite an XTRA-t session
  2- If it is ok to start an XTRA-T session, it initaites a PUNC based
     XTRA-T session (If PUNC > 25 Km) XTRA-T session will be initiated
 
INPUT PARAMS
e_cmd_err - Status of xtra injection

OUTPUT PARAMS
None

RETURN VALUE 
 TRUE  - If the request for XTRA-T session is done
 FALSE - Otherwise
===========================================================================*/
boolean tm_xtra_invoke_xtrat(pdsm_xtra_cmd_err_e_type e_cmd_err)
{
  boolean b_result = FALSE;
#ifdef FEATURE_CGPS_XTRA_T
  if((e_cmd_err != PDSM_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY))
  {
    MSG_MED("Initiate XTRA-T session.",
            0, 0, 0);
    b_result = tle_GetTerrPosStatus();
  }
  else
  {
    MSG_HIGH("Can not initiate XTRA-T session while, Assisted GPS session in progress",
             0, 0, 0);
  }
#endif /* FEATURE_CGPS_XTRA_T */
  return b_result;
}

/*===========================================================================

FUNCTION tm_xtra_update_version_check

DESCRIPTION
  This function is called by TM-PDAPI to inform XTRA module about XTRA version
  check type for verification purposes.

DEPENDENCIES

RETURN VALUE
  TRUE  - Version check is successful
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_xtra_update_version_check
(
  pdsm_pa_xtra_ver_check_e_type xtra_version_check
)
{
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

  if (NULL == xtra_state_ptr)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);
    return FALSE;
  }

  /* Set the global variable for XTRA version check */
  xtra_state_ptr->version_check = xtra_version_check;

  return TRUE;
}
/*===========================================================================

FUNCTION tm_xtra_get_version_check

DESCRIPTION
  This function is called by TM-PDAPI to get XTRA version check enabled parameter
  for LBS.

DEPENDENCIES

RETURN VALUE
  pdsm_pa_xtra_ver_check_e_type
    PDSM_PA_XTRA_VERSION_CHECK_DISABLED, XTRA Version Check Disabled
    PDSM_PA_XTRA_VERSION_CHECK_AUTO,     XTRA Version Check Automatic 
    PDSM_PA_XTRA_VERSION_CHECK_XTRA2,    XTRA Version Check with XTRA2 format 
    PDSM_PA_XTRA_VERSION_CHECK_XTRA3,    XTRA Version Check with XTRA3 format
    PDSM_PA_XTRA_VERSION_CHECK_MAX,      XTRA feature Disabled 

SIDE EFFECTS
  None

===========================================================================*/
pdsm_pa_xtra_ver_check_e_type tm_xtra_get_version_check ()
{
  tm_xtra_state_type *xtra_state_ptr = tm_xtra_get_state_ptr();

  if (NULL == xtra_state_ptr)
  {
    MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);
    return PDSM_PA_XTRA_VERSION_CHECK_MAX;
  }

  /* return the global variable for XTRA version check */
  return (xtra_state_ptr->version_check);
}
#endif /* FEATURE_CGPS_XTRA */
