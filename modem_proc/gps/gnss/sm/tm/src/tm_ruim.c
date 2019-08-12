/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  RUIM interface of Position Determination Module

GENERAL DESCRIPTION
  This module makes up the RUIM interface of Position Determination subscriber
  station software.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 
 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_ruim.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/04/16   mj      Add changes to check sub id sanity at call back function
11/25/15   mc      Removed debug message in tm_ruim_event_cb()
10/20/15   rh      Register only on MCFG sub refresh, not slot 
07/08/15   rh      Changed NV refresh to be done on one specifc sub 
11/11/14   rh      Added DSDS support for SIM hot swap 
06/24/14   rh      Add SIM hot swap support 
03/14/15   rh      Added support for MultiSIM NV  
05/13/14   mj      Add featurization to fix SMART compile/link issues
03/04/14   mj      Remove rcevt_wait_name as its leading to a crash waiting on it.
12/23/13   mj	      Remove FEATURE_DUAL_SIM and FEATURE_TRIPLE_SIM
09/27/13   mj      TSTS Support: Open tertiary mmgsdi session for triple SIM
03/03/10   ns      DSDS Supprt
10/22/08   ank     Invoke GSDI API to read CDMA service table only after client reg is complete. 
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_GPSONE
#include "err.h"
#include "tm_common.h" 
#include "tm_ruim.h"
#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdisessionlib.h"
#else /* FEATURE_MMGSDI_SESSION_LIB */
#include "gsdi_exp.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#include "nvruimi.h"
#include "aries_os_api.h"
#include "tm_data.h"
#ifdef FEATURE_UIM_SUPPORT_LBS
#include "tm_1x_up.h"
#endif /*FEATURE_UIM_SUPPORT_LBS*/
#include "tm_vx.h"
#include "tm_cm_iface.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

mmgsdi_client_id_type tm_ruim_gsdi_client_id = (mmgsdi_client_id_type)0;
boolean tm_ruim_client_id_registered = FALSE;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/* TRUE if received at least one SIM inserted event during E911 */
boolean tm_ruim_got_card_inserted_evt = FALSE; 
mmgsdi_event_data_type tm_ruim_cached_card_inserted_event;

/* TRUE if received at least one 1x subscription ready event during E911 */
boolean tm_ruim_got_sub_ready_evt = FALSE; 
mmgsdi_event_data_type tm_ruim_cached_sub_ready_event;

/* TRUE if received at least one 1x session close event during E911 */
boolean tm_ruim_got_session_close_evt = FALSE; 
mmgsdi_event_data_type tm_ruim_cached_session_close_event;

/* we don't need a full cached event queue, only need order information
   for sub_ready & session_close, sim_inserted can always replay at the end */
boolean tm_ruim_last_cached_event_is_session_close = FALSE;

/* MCFG refresh reference ID passed in to GNSS callback */
uint32 tm_ruim_mcfg_refresh_reference_id = 0;
#endif  /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_MMGSDI_SESSION_LIB
mmgsdi_session_id_type tm_ruim_gsdi_session_id = (mmgsdi_session_id_type)0;
boolean tm_ruim_session_id_allocated = FALSE; 
#else

typedef enum 
{
  TM_RUIM_MMGSDI_CDMA_RUIM_OP_NONE = 0,
  TM_RUIM_MMGSDI_CDMA_RUIM_OP_WRITE,  /* RUIM write operation requested to MMGSDI */
  TM_RUIM_MMGSDI_CDMA_RUIM_OP_READ,   /* RUIM read operation requested to MMGSDI */
  TM_RUIM_MMGSDI_CDAM_RUIM_OP_MAX = 0xFF
}tm_ruim_mmgsdi_cdma_ruim_op_e_type;

boolean tm_ruim_pending_read_cdma_svc_table = FALSE;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*-------------------------------------------------------------------------
  Local variables
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
LOCAL void tm_ruim_cmd_cb
( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

LOCAL void tm_ruim_event_cb
(
  const mmgsdi_event_data_type *event
);

/*====================================================================

FUNCTION tm_mmgsdi_open_session

DESCRIPTION
  Internal function which calls mmgsdi_session_open for the specified
  session type, and generates an error if unsuccessful.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI. 
  Call after client registration with MMGSDI is complete, and CM
  has a valid client ID.

RETURN VALUE
  none

SIDE EFFECTS
  Request to open a session enqueued with MMGSDI.
  ERROR if unsuccessful.

=====================================================================*/
LOCAL void tm_mmgsdi_open_session(
  mmgsdi_session_type_enum_type tm_mmgsdi_session_type
)
{
   mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;

   mmgsdi_status = mmgsdi_session_open(tm_ruim_gsdi_client_id, 
                                       tm_mmgsdi_session_type,
                                       0,
                                       tm_ruim_event_cb,
                                       FALSE,
                                       tm_ruim_cmd_cb, 
                                       (mmgsdi_client_data_type) tm_mmgsdi_session_type);
   /* This should not happen, raise an error */
   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
     MSG_ERROR( "MMGSDI session open failed: Type %d STATUS %d",
                   tm_mmgsdi_session_type, mmgsdi_status, 0 );
   }
} /* tm_mmgsdi_open_session */

#else //FEATURE_MMGSDI_SESSION_LIB

LOCAL void tm_ruim_gsdi_client_id_reg_cb
( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

LOCAL void tm_ruim_gsdi_pin_event_cb_func 
(
  gsdi_sim_events_T event            /* Event ID              */
);



LOCAL void tm_ruim_mmgsdi_read_transparent_cb (mmgsdi_return_enum_type status, 
                                        mmgsdi_cnf_enum_type    cnf, 
                                        const mmgsdi_cnf_type        *cnf_ptr);



#endif //FEATURE_MMGSDI_SESSION_LIB

/*===========================================================================

FUNCTION  TM_RUIM_IPC_SEND

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_ruim_ipc_send 
( 
  uint32  q_MsgId,	
  uint32  q_DestThreadId,
  uint8   data_sz,
  uint8   data[]
)
{
  os_IpcMsgType              *ipc_msg_ptr;

  ipc_msg_ptr = os_IpcCreate(data_sz, IPC_ALLOCATION_DYNAMIC, q_DestThreadId);

  if (ipc_msg_ptr == NULL)
  {
    MSG_ERROR("Failed to create IPC mesage of sz %d",data_sz,0,0); 
    return FALSE;
  }

  ipc_msg_ptr->q_MsgId          = q_MsgId;
  ipc_msg_ptr->q_DestThreadId   = q_DestThreadId;
  ipc_msg_ptr->q_Size           = 0;

  memscpy((void*) ipc_msg_ptr->p_Data, data_sz, (void*) data, data_sz);

  if (!os_IpcSend(ipc_msg_ptr, q_DestThreadId))
  {
    MSG_ERROR("Failed to send IPC mesage %d to Thread %d",
                 q_MsgId, q_DestThreadId, 0);     
    (void)os_IpcDelete(ipc_msg_ptr);   
    return FALSE;
  }

  return TRUE;
}


#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================

FUNCTION  tm_gsdi_handle_lbs_svc_available

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_gsdi_handle_lbs_svc_available(void)
{

  #ifdef FEATURE_UIM_SUPPORT_LBS
  nvruim_lbs_init_support(TRUE);

  #if 0 /* Not reading UIM NV as temporary workaround for CR625878 */
  /* re-Read LBS param's from RUIM card */
  tm_core_ruim_read(); // Dynamic mode
  #if defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)
    tm_1x_up_ruim_read();
  #endif /* FEATURE_CGPS_USES_CDMA && !FEATURE_GNSS_LOW_MEMORY*/

  #if defined(FEATURE_GPSONE_VX_LCS_AGENT) && !defined(FEATURE_GNSS_LOW_MEMORY)
    tm_vx_ruim_read();
  #endif /* FEATURE_GPSONE_VX_LCS_AGENT  & !FEATURE_GNSS_LOW_MEMORY */

  #ifdef FEATURE_CGPS_XTRA
    tm_xtra_ruim_init_and_read();
  #endif /* FEATURE_CGPS_XTRA */
  #endif /* CR625878 workaround */
  
  #endif /*FEATURE_UIM_SUPPORT_LBS*/
}


/*===========================================================================
FUNCTION tm_ruim_is_1x_session_id

DESCRIPTION
  This function check whether a session_id is MMGSDI_1X_PROV_PRI_SESSION
  
DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE if session_id is 1x MMGSDI_1X_PROV_PRI_SESSION, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean tm_ruim_is_1x_session_id
(
  mmgsdi_session_id_type    session_id
)
{
#ifdef FEATURE_UIM_SUPPORT_LBS
  mmgsdi_session_id_type      *tm_mmgsdi_ruim_session_id_ptr = NULL; 

  /* If Dual SIM, use the mmgsdi session info table */
  if((tm_mmgsdi_get_session_id_ptr( MMGSDI_1X_PROV_PRI_SESSION,
      &tm_mmgsdi_ruim_session_id_ptr)) &&
      (*tm_mmgsdi_ruim_session_id_ptr == session_id))
  {
    return TRUE;
  }
#endif /* FEATURE_UIM_SUPPORT_LBS */

  return FALSE;
}

/*===========================================================================
FUNCTION tm_ruim_is_session_id_dd_subs

DESCRIPTION
  This function check whether a session_id is current DD subscription
  (Designated Data)
  
DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE if session_id is DD subs, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean tm_ruim_is_session_id_dd_subs
(
  mmgsdi_session_id_type    session_id
)
{
  sys_modem_as_id_e_type     dd_subs_id, sess_sub_id = SYS_MODEM_AS_ID_NONE;

  /* get SUB ID for this session_id */
  if (tm_mmgsdi_get_subscription_id(session_id, &sess_sub_id) == FALSE)
  {
    MSG_HIGH( "SessID=0x%x no SUBS ID!", session_id, 0, 0);
    return FALSE;
  } 

  dd_subs_id = tm_core_get_current_dd_subs_id();

  MSG_MED( "SessID=0x%x DD_SUB=%d SESS_SUB=%d",
                session_id, (int)dd_subs_id, (int)sess_sub_id );

  if (sess_sub_id == dd_subs_id && sess_sub_id != SYS_MODEM_AS_ID_NONE)
  {
    return TRUE;
  }

  return FALSE;
}


/*===========================================================================
FUNCTION tm_ruim_cmd_cb

DESCRIPTION
  This function handles the MMGSDI client ID registration command callback.  
  It will assign the global client ID value which will be used in mmgsdi 
  function calls

ARGUMENTS
  

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void tm_ruim_cmd_cb
( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  os_IpcMsgType              *ipc_msg_ptr;

  if (status == MMGSDI_SUCCESS)
  {
    if (cnf == MMGSDI_CLIENT_ID_AND_EVT_REG_CNF)
    {
      tm_ruim_gsdi_client_id = cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;
      MSG_MED("GSDI LBS Client ID 0x%x", tm_ruim_gsdi_client_id, 0, 0);
      tm_ruim_client_id_registered = TRUE;

      ipc_msg_ptr = os_IpcCreate(0, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

      if (ipc_msg_ptr != NULL)
      {
        ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_MMGSDI_CLIENT_REG;
        ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
        ipc_msg_ptr->q_Size           = 0;

        if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
        {
          MSG_ERROR("Failed to send IPC mesage",0,0,0);     
          (void)os_IpcDelete(ipc_msg_ptr);   
          return;
        }
        return; /*lint !e506 */
      } /* End ipc_msg_ptr */
      else
      {
        MSG_ERROR("Failed to create IPC mesage",0,0,0); 
        return;
      } /* End ipc_msg_ptr */
    }
    else if (cnf == MMGSDI_SESSION_OPEN_CNF) 
    {
      /* Set session id for the right session */
      /* If Dual SIM, use the mmgsdi session info table */
      tm_mmgsdi_set_session_id( 
      (mmgsdi_session_type_enum_type)cnf_ptr->session_open_cnf.response_header.client_data,
      cnf_ptr->session_open_cnf.session_id);
      MSG_MED("GSDI LBS Open Session ID 0x%x (hdr id 0x%x) ", 
              cnf_ptr->session_open_cnf.session_id,
              cnf_ptr->session_open_cnf.response_header.session_id, 
              0);
    }
    else if (cnf == MMGSDI_SRV_AVAILABLE_CNF) 
    {
      if (cnf_ptr->srv_available_cnf.srv_available == TRUE && 
          /* Check session id for RUIM*/   
          tm_ruim_is_1x_session_id(cnf_ptr->srv_available_cnf.response_header.session_id))
      {
          ipc_msg_ptr = os_IpcCreate(0, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

          if (ipc_msg_ptr != NULL)
          {
            ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_MMGSDI_LBS_SRV_AVAILABLE;
            ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
            ipc_msg_ptr->q_Size           = 0;

            if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
            {
              MSG_ERROR("Failed to send IPC mesage",0,0,0);     
              (void)os_IpcDelete(ipc_msg_ptr);   
              return;
            }
            return; /*lint !e506 */
          } /* End ipc_msg_ptr */
          else
          {
            MSG_ERROR("Failed to create IPC mesage",0,0,0); 
            return;
          } /* End ipc_msg_ptr */
      }
      else
      {
        MSG_ERROR("LBS Svc not available",0,0,0);
      }
    }
    else
    {
      MSG_HIGH("cnf %d not handled", cnf,0,0);
    }
  }
  else
  {
    MSG_ERROR("Status =%d",status,0,0);
  }
} /* tm_ruim_cmd_cb */


/*===========================================================================
FUNCTION  tm_ruim_event_cb_card_inserted_evt

DESCRIPTION
  Handle MMGSDI_CARD_INSERTED_EVT Callback

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
LOCAL boolean tm_ruim_event_cb_card_inserted_evt
(
    const mmgsdi_event_data_type *event
)
{
  if (event == NULL)
  {
    MSG_ERROR("Null SIM inserted evt!", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  if (tm_cm_is_phone_in_e911_state())
  {
    // save the event (in case we need any other info), we'll keep only the latest such event
    MSG_MED("SIM inserted evt cached during E911", 0, 0, 0);
    tm_ruim_got_card_inserted_evt = TRUE;
    tm_ruim_cached_card_inserted_event = *event;  
    return FALSE; 
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  if (tm_ruim_ipc_send( TM_CORE_MSG_ID_SIM_CARD_INSERTED, THREAD_ID_SM_TM, 0, NULL) != TRUE)
  {
    MSG_ERROR(" Failed to send IPC for SIM inserted evt!", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================
FUNCTION  tm_ruim_event_cb_sub_ready_evt

DESCRIPTION
  Handle MMGSDI_SUBSCRIPTION_READY_EVT Callback

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
LOCAL boolean tm_ruim_event_cb_sub_ready_evt
(
    const mmgsdi_event_data_type *event
)
{
  if (event == NULL)
  {
    MSG_ERROR("Null Subs Ready evt!", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_UIM_SUPPORT_LBS
  if (tm_ruim_is_1x_session_id(event->session_id))
  {
#ifdef FEATURE_MODEM_CONFIG_REFRESH
    if (tm_cm_is_phone_in_e911_state())
    {
      MSG_MED("Sub_Ready evt cached during E911", 0, 0, 0);
      tm_ruim_got_sub_ready_evt = TRUE;
      // save the event, we'll keep only the latest such event (for 1x sub ready)
      tm_ruim_cached_sub_ready_event = *event;
      tm_ruim_last_cached_event_is_session_close = FALSE;
      return FALSE; 
    }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

    if (tm_ruim_ipc_send( TM_CORE_MSG_ID_SIM_SUBSCRIPTION_READY, THREAD_ID_SM_TM, 
                          sizeof(event->data.subscription_ready), 
                          (void *)&event->data.subscription_ready) != TRUE)
    {
      MSG_ERROR(" Failed to send IPC for Subs Ready evt!", 0, 0, 0);
      return FALSE;
    }
  }
#endif /* FEATURE_UIM_SUPPORT_LBS */

  return TRUE;
}


/*===========================================================================
FUNCTION  tm_ruim_event_cb_session_changed_evt

DESCRIPTION
  Handle MMGSDI_SESSION_CHANGED_EVT Callback
  (session activated or non-activated switch)
  Usually SIM removal and insertion will trigger this event (active<=>inactive)
 
DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
LOCAL boolean tm_ruim_event_cb_session_changed_evt
(
    const mmgsdi_event_data_type *event
)
{
  if (event == NULL)
  {
    MSG_ERROR("Null Session Changed evt!", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_UIM_SUPPORT_LBS
  /* We will only care about SessionChanged event on DD subs,
     should terminate DD sub session upon this event */
  if (tm_ruim_is_session_id_dd_subs(event->session_id))
  {
    if (tm_ruim_ipc_send( TM_CORE_MSG_ID_SIM_SESSION_CHANGED, THREAD_ID_SM_TM, 0, NULL) != TRUE)
    {
      MSG_ERROR(" Failed to send IPC for Session Changed evt!", 0, 0, 0);
      return FALSE;
    }
  }
#endif /* FEATURE_UIM_SUPPORT_LBS */

  return TRUE;
}

/*===========================================================================
FUNCTION  tm_ruim_event_cb_session_close_evt

DESCRIPTION
  Handle MMGSDI_SESSION_CLOSE_EVT Callback
 
DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
LOCAL boolean tm_ruim_event_cb_session_close_evt
(
    const mmgsdi_event_data_type *event
)
{
  if (event == NULL)
  {
    MSG_ERROR("Null Session Closed evt!", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_UIM_SUPPORT_LBS
  /* We only care about SessionClosed event on DD subs, 1x needs to reopen session? */
  if (tm_ruim_is_1x_session_id(event->session_id))
  {
#ifdef FEATURE_MODEM_CONFIG_REFRESH
    if (tm_cm_is_phone_in_e911_state())
    {
      /* save the event, we'll keep only the latest such event (for 1x session closed)
         during E911 session, should have only one such event since no new 1x session will be opened */
      MSG_MED("Session_Close evt cached during E911", 0, 0, 0);
      tm_ruim_got_session_close_evt = TRUE;
      tm_ruim_cached_session_close_event = *event;
      tm_ruim_last_cached_event_is_session_close = TRUE;
      return FALSE; 
    }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

    if (tm_ruim_ipc_send( TM_CORE_MSG_ID_SIM_SESSION_CLOSED, THREAD_ID_SM_TM, 
                          sizeof(event->data.session_close), (void *)&event->data.session_close) != TRUE)
    {
      MSG_ERROR(" Failed to send IPC for Session Closed evt!", 0, 0, 0);
      return FALSE;
    }
  }
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  else
  {
    tm_mmgsdi_close_session_id(event->session_id); // for non-1x session close event, just reset it
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#endif /* FEATURE_UIM_SUPPORT_LBS */

  return TRUE;
}


/*===========================================================================
FUNCTION  tm_ruim_event_cb 

DESCRIPTION
  GSDI event CB function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in LBS command buffer

===========================================================================*/
LOCAL void tm_ruim_event_cb
(
    const mmgsdi_event_data_type *event
)
{
  if (event==NULL)
  {
    MSG_ERROR("tm_ruim_event_cb: NULL ptr!",0,0,0);
    return;
  }

  MSG_MED("tm_ruim_event_cb called with event: %d session_id 0x%x", event->evt, event->session_id, 0);

  /* Filter events of interest to LBS */
  switch (event->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      MSG_MED("SIM inserted slot=%d aids=%d",
          event->data.card_inserted.slot, event->data.card_inserted.num_aids_avail, 0);
      (void)tm_ruim_event_cb_card_inserted_evt(event);
      break;

    case MMGSDI_SUBSCRIPTION_READY_EVT:
      tm_mmgsdi_set_app_type(event->session_id, event->data.subscription_ready.app_info.app_data.app_type);
      MSG_MED("Sub App type set SessionID 0x%x, app type %d (slot %d)", event->session_id,
             event->data.subscription_ready.app_info.app_data.app_type,
             event->data.subscription_ready.app_info.slot);
      (void)tm_ruim_event_cb_sub_ready_evt(event);
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      MSG_MED("Session Changed SessionID=0x%x slot=%d active=%d",
            event->session_id, event->data.session_changed.app_info.slot, (uint32)event->data.session_changed.activated);
      /* if activated==FALSE, session is closed */
      (void)tm_ruim_event_cb_session_changed_evt(event);
      break;

    case MMGSDI_SESSION_CLOSE_EVT:
      MSG_MED("Session Closed SessionID=0x%x slot=%d cause=%d",
            event->session_id, event->data.session_close.slot, event->data.session_close.cause);
      (void)tm_ruim_event_cb_session_close_evt(event);
      break;

    case MMGSDI_CARD_ERROR_EVT:   /* will get CARD_ERROR for SIM removal */
      MSG_MED("SIM error slot=%d", (uint32)event->data.card_error.slot, 0, 0);
      break;
    case MMGSDI_CARD_REMOVED_EVT: /* usually won't get this for SIM removal */
      MSG_MED("SIM removed slot=%d", (uint32)event->data.card_removed.slot, 0, 0);
      break;

    default:
      break;
  }
} /* tm_ruim_event_cb */


/*===========================================================================

FUNCTION tm_gsdi_handle_sim_card_inserted_evt
                                        
DESCRIPTION
  This function issues a read to the CDMA service table to see if 3GPD is 
  supported on the R-UIM.  It is called during initialization.
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/
void tm_gsdi_handle_sim_card_inserted_evt()
{

#ifdef FEATURE_UIM_SUPPORT_LBS

   /* Make sure client_id is registered */
  if(tm_ruim_client_id_registered == TRUE)
  {
    /* Open new session for 1x */
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    if (tm_mmgsdi_get_session_id_status(MMGSDI_1X_PROV_PRI_SESSION) == FALSE) 
    {
      /* Open session for 1x default provisioning app */
      tm_mmgsdi_open_session(MMGSDI_1X_PROV_PRI_SESSION);
    }
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
    
    /* FIXME: Do we need to re-open these GW sessions for SIM hot swap (maybe for reading IMSI?) 
       Most likely session just got into inactivated state, won't be closed during SIM swap */
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
    if (tm_mmgsdi_get_session_id_status(MMGSDI_GW_PROV_PRI_SESSION) == FALSE) 
    {
      /* Open session for GW default provisioning app if not already opened */
      tm_mmgsdi_open_session(MMGSDI_GW_PROV_PRI_SESSION);
    }

    if (tm_mmgsdi_get_session_id_status(MMGSDI_GW_PROV_SEC_SESSION) == FALSE) 
    {
      /* Open session for GW secondary provisioning app */
      tm_mmgsdi_open_session(MMGSDI_GW_PROV_SEC_SESSION);
    }

    if (tm_mmgsdi_get_session_id_status(MMGSDI_GW_PROV_TER_SESSION) == FALSE) 
    {
      /* Open session for GW 3rd provisioning app */
      tm_mmgsdi_open_session(MMGSDI_GW_PROV_TER_SESSION);
    }
#endif  /* #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */
    
  }
  else
  {
    MSG_ERROR("Client_id not registered",0,0,0);
  }
#endif //FEATURE_UIM_SUPPORT_LBS

  return;
} /* tm_gsdi_handle_sim_card_inserted_evt */

/*===========================================================================

FUNCTION tm_gsdi_sim_handle_subscription_ready_evt
                                        
DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_sim_handle_subscription_ready_evt(void* p_data)
{
#ifdef FEATURE_UIM_SUPPORT_LBS

  mmgsdi_session_id_type   *tm_mmgsdi_ruim_session_id_ptr = NULL; 
  mmgsdi_session_id_type tm_1x_prov_pri_session_id = (mmgsdi_session_id_type)MMGSDI_INVALID_SESSION_ID;

  tm_ruim_sub_event_info_s_type   *info_ptr;
  MSG_HIGH("tm_gsdi_sim_handle_subscription_ready_evt",0,0,0);
  
  if (p_data == NULL)
  {
    MSG_ERROR("TM_RUIM: Null pointer to function ", 0,0,0); 
    return;
  }
  info_ptr = (tm_ruim_sub_event_info_s_type*)p_data;

  /* If Dual SIM, use the mmgsdi session info table */
  if(tm_mmgsdi_get_session_id_ptr( MMGSDI_1X_PROV_PRI_SESSION,
        &tm_mmgsdi_ruim_session_id_ptr) == TRUE)
  {
    tm_1x_prov_pri_session_id = *tm_mmgsdi_ruim_session_id_ptr;
  }

  if(tm_mmgsdi_get_session_id_status(MMGSDI_1X_PROV_PRI_SESSION) == TRUE)  
  {
    if(info_ptr->subscription_ready.app_info.app_data.app_type == MMGSDI_APP_CSIM)
    {
#ifdef FEATURE_MMGSDI_SESSION_LIB    
        if ( MMGSDI_SUCCESS == mmgsdi_session_is_service_available(
                                                    tm_1x_prov_pri_session_id,
                                                    MMGSDI_CSIM_SRV_LBS, 
                                                    tm_ruim_cmd_cb,
                                                    NULL 
           ))
        {
          MSG_HIGH("Succesfully posted the Session Is Svc Available for CSIM card to MMGSDI",0,0,0);
        }
        else
        {
          MSG_ERROR (" MMGSDI rejected Session Is Svc Available request for CSIM card ",0,0,0);
        }
#endif /* FEATURE_MMGSDI_SESSION_LIB*/		
    }
    else if(info_ptr->subscription_ready.app_info.app_data.app_type == MMGSDI_APP_RUIM)
    {
#ifdef FEATURE_MMGSDI_SESSION_LIB    
        if ( MMGSDI_SUCCESS == mmgsdi_session_is_service_available(
                                                    tm_1x_prov_pri_session_id,
                                                    MMGSDI_CDMA_SRV_LBS,  
                                                    tm_ruim_cmd_cb,
                                                    NULL
           ))
        {
          MSG_HIGH("Succesfully posted  Session Is Svc Available for RUIM card to MMGSDI",0,0,0);
        }
        else
        {
          MSG_ERROR ("MMGSDI rejected the Session Is Svc Available request for RUIM card",0,0,0);
        }
#endif /* FEATURE_MMGSDI_SESSION_LIB*/			
    }
  }
  else
  {
    MSG_ERROR (" Session id not yet allocated",0,0,0);
  }
#endif //FEATURE_UIM_SUPPORT_LBS
} /* tm_gsdi_sim_handle_subscription_ready_evt */

/*===========================================================================

FUNCTION tm_ruim_abort_non_e911_agps_sessioni
                                        
DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
LOCAL void tm_ruim_abort_non_e911_agps_session(void)
{
  boolean is_in_e911 = tm_cm_is_phone_in_e911_state(); 

  /* phone may be outside E911 state, if location session is started as E911
     let it continue */
  if ( is_in_e911 || tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911 )
  {
    /* If it's in E911, let E911 session run till end */
    MSG_MED("tm_ruim_abort_non_e911_agps_session: e911=%d handle=%d!",
            is_in_e911, tm_core_info.session_info.active_sess_handle, 0);
  }
  else
  {
    /* Do not abort the stand alone sesison. */
    if(tm_core_session_active() && (tm_core_is_standalone_active() == FALSE) )
    {
      MSG_MED("tm_ruim_abort_non_e911_agps_session",0,0,0);
      /* It is not a stand alone session but an AGPS session , abort it.  */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR);
    }
  }

} /* tm_ruim_abort_non_e911_agps_session */


/*===========================================================================

FUNCTION tm_gsdi_handle_session_changed
                                        
DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_handle_session_changed_evt(void)
{
  MSG_HIGH("tm_gsdi_handle_session_changed_evt",0,0,0);

  /* abort non-E911 AGPS session */
  tm_ruim_abort_non_e911_agps_session();

} /* tm_gsdi_handle_session_changed_evt */


/*===========================================================================

FUNCTION tm_gsdi_handle_session_closed_evt
                                        
DESCRIPTION 
 
  Handle 1x MMGSDI session close event
  (abort AGPS session and re-open 1x MMGSDI session)
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_handle_session_closed_evt(void* p_data)
{
  tm_ruim_close_event_info_s_type   *info_ptr;

  if (p_data == NULL)
  {
    MSG_ERROR("TM_RUIM: Null pointer to function ", 0,0,0); 
    return;
  }
  info_ptr = (tm_ruim_close_event_info_s_type*)p_data;

  MSG_HIGH("tm_gsdi_handle_session_closed_evt (sessionID=0x%x)",
           info_ptr->session_close.session_id,0,0);

  /* abort non-E911 AGPS session if session_id is DD sub */
  if (tm_ruim_is_session_id_dd_subs(info_ptr->session_close.session_id))
  {
    tm_ruim_abort_non_e911_agps_session();
  }

  /* reset this session ID from TM table */
  tm_mmgsdi_close_session_id(info_ptr->session_close.session_id);

  /* Make sure client_id is registered */
  if(tm_ruim_client_id_registered == TRUE)
  {
    /* Open new session */
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    /* Open session for 1x default provisioning app */

    tm_mmgsdi_open_session(MMGSDI_1X_PROV_PRI_SESSION);
    #endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  }
  else
  {
    MSG_MED("ClientId not registered",0,0,0);
  }
} /* tm_gsdi_handle_session_closed_evt */


/*===========================================================================

FUNCTION tm_gsdi_handle_client_reg_cb
                                        
DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_handle_client_reg_cb(void)
{
    MSG_HIGH("tm_gsdi_handle_client_reg_cb",0,0,0);

    /* Open new session */
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    /* Open session for 1x default provisioning app */
    tm_mmgsdi_open_session(MMGSDI_1X_PROV_PRI_SESSION);
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )

    /* Open session for default provisioning app */
    tm_mmgsdi_open_session(MMGSDI_GW_PROV_PRI_SESSION);
    /* Open session for default provisioning app */

    tm_mmgsdi_open_session(MMGSDI_GW_PROV_SEC_SESSION);
	/* Open session for default provisioning app */
    tm_mmgsdi_open_session(MMGSDI_GW_PROV_TER_SESSION);
#endif  /* #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */
}

#else /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
FUNCTION tm_ruim_gsdi_client_id_reg_cb

DESCRIPTION
  This function handles the MMGSDI client ID registration command callback.  
  It will assign the global client ID value which will be used in mmgsdi 
  function calls

ARGUMENTS
  

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void tm_ruim_gsdi_client_id_reg_cb
( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  if (status != MMGSDI_SUCCESS || cnf != MMGSDI_CLIENT_ID_REG_CNF)
  {
    return;
  }

  tm_ruim_gsdi_client_id = cnf_ptr->client_id_reg_cnf.response_header.client_id;
  MSG_MED("GSDI LBS Client ID 0x%x", tm_ruim_gsdi_client_id, 0, 0);
  tm_ruim_client_id_registered = TRUE;

  /* If reading of CDMA SVC Table is pending, read it now */
  if(tm_ruim_pending_read_cdma_svc_table == TRUE)
  {
    /* Send an IPC to TM to reading the CDMA Svc Table */
    if(tm_ruim_ipc_send( TM_CORE_MSG_ID_MMGSDI_CMD_READ_CDMA_SVC_TBL, 
                         THREAD_ID_SM_TM, 0, NULL))
    {
      tm_ruim_pending_read_cdma_svc_table = FALSE;
    }
  }
} /* tm_ruim_gsdi_client_id_reg_cb */

/*===========================================================================
FUNCTION tm_ruim_mmgsdi_read_cmda_svc_table

DESCRIPTION
  This function issues a read command to MMGSDI API to read the CDMA Svc tbl.
  Called by tm_task in response to IPC TM_CORE_MSG_ID_MMGSDI_CMD_READ_CDMA_SVC_TBL.

ARGUMENTS
  

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tm_ruim_mmgsdi_read_cmda_svc_table(void)
{
  /* Issue a read command to read the SVC TABLE through MMGSDI API */
  if ( MMGSDI_SUCCESS == 
           mmgsdi_read_transparent(
                 tm_ruim_gsdi_client_id,
                 MMGSDI_SLOT_AUTOMATIC,
                 MMGSDI_CDMA_SVC_TABLE, 
                 0, 0, 
                 (mmgsdi_callback_type) tm_ruim_mmgsdi_read_transparent_cb,
                 (uint32)TM_RUIM_MMGSDI_CDMA_RUIM_OP_READ))
  {
    MSG_HIGH("Succesfully posted the CDMA SVC TABLE read to MMGSDI",0,0,0);
  }
  else
  {
    MSG_ERROR (" MMGSDI rejected the CDMA SVC TABLE read request",0,0,0);
  }
}

/*===========================================================================
FUNCTION  GSDI_PIN_EVENT_CB_FUNC 

DESCRIPTION
  GSDI PIN status callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in LBS command buffer

===========================================================================*/
LOCAL void tm_ruim_gsdi_pin_event_cb_func 
(
  gsdi_sim_events_T             event            /* Event ID              */
)
{
  os_IpcMsgType              *ipc_msg_ptr;

  MSG_MED("tm_ruim_gsdi_pin_event_cb_func called with event: %d", event, 0, 0);

  /* Filter events of interest to LBS */
  switch (event)
  {
    case GSDI_SIM_INIT_COMPLETED:
      ipc_msg_ptr = os_IpcCreate(0, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

      if (ipc_msg_ptr != NULL)
      {
        ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_SIM_INIT_COMPLETED;
        ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
        ipc_msg_ptr->q_Size           = 0;

        if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
        {
          MSG_ERROR("Failed to send IPC mesage",0,0,0);     
          (void)os_IpcDelete(ipc_msg_ptr);   
          return;
        }
        return; /*lint !e506 */
      } /* End ipc_msg_ptr */
      else
      {
        MSG_ERROR("Failed to create IPC mesage",0,0,0); 

      } /* End ipc_msg_ptr */

      break;
    default:
      MSG_HIGH("Unsupported GSDI pin event: %d", event, 0, 0);
      /* Ignore event */
      break;
  }
}  /* gsdi_pin_event_cb_func */


/*===========================================================================

FUNCTION tm_gsdi_sim_init_handler
                                        
DESCRIPTION
  This function issues a read to the CDMA service table to see if 3GPD is 
  supported on the R-UIM.  It is called during initialization.
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/
void tm_gsdi_sim_init_handler()
{
  /* Make sure client_id is registered */
  if(tm_ruim_client_id_registered == TRUE)
  {
    /* Issue a read command to read the SVC TABLE through MMGSDI API */
    tm_ruim_mmgsdi_read_cmda_svc_table(void);
  }
  else
  {
    MSG_HIGH("Client_id not registered yet, Keep CDMA SVC TABLE read pending",0,0,0);
    tm_ruim_pending_read_cdma_svc_table = TRUE;
  }
  return;
} /* tm_ruim_check_cdma_svc_table */

/* ----------------------------------------------------------------------------
   FUNCTION:      TM_RUIM_MMGSDI_READ_TRANSPARENT_CB

   DESCRIPTION:
     This function provides internal callback function pointer to internal
     processing that MMGSDI does not care for the callback information returned

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
/* ARGSUSED */
void tm_ruim_mmgsdi_read_transparent_cb 
(
  mmgsdi_return_enum_type   status, 
  mmgsdi_cnf_enum_type      cnf, 
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  tm_ruim_cdma_svc_table cdma_svc_tbl;

  (void) cnf;
  /* Handle Error conditions */
  if ( cnf_ptr->read_cnf.response_header.client_data !=
                                  TM_RUIM_MMGSDI_CDMA_RUIM_OP_READ )
  {
    MSG_ERROR("tm_ruim_mmgsdi_read_transparent_cb can not handle opearation %d", 
             cnf_ptr->read_cnf.response_header.client_data,0,0);
    return;
  }

  if( status != MMGSDI_SUCCESS )
  {
    MSG_HIGH("Could Not Read CDMA Service Table!",0,0,0);
    return; 
  }

  if( cnf_ptr->read_cnf.read_data.data_len > UIM_CDMA_SVC_TABLE_SIZE ||
      cnf_ptr->read_cnf.read_data.data_len <= 0)
  {
    MSG_ERROR("Error: Length of SVC TABLE %d",
                 cnf_ptr->read_cnf.read_data.data_len, 0, 0);
    return;
  }

  /* Copy the read data and send to TM task via IPC */
  MSG_HIGH(" MMGSDI_CDMA_RUIM_OP_READ call back: CDMA service table read",0,0,0);
  memset((void*) &cdma_svc_tbl, 0, sizeof(cdma_svc_tbl));

  cdma_svc_tbl.data_len = memscpy( (void*) cdma_svc_tbl.data, sizeof(cdma_svc_tbl.data),
  	       (void*) cnf_ptr->read_cnf.read_data.data_ptr, cnf_ptr->read_cnf.read_data.data_len );

  /* Send an IPC to TM to begin reading the NV */
  (void) tm_ruim_ipc_send(
           TM_CORE_MSG_ID_MMGSDI_CDMA_SVC_INITIALIZED, 
           THREAD_ID_SM_TM,
           (uint8) sizeof(cdma_svc_tbl),
           (void*) &cdma_svc_tbl);

  return;

} /* tm_ruim_mmgsdi_read_transparent_cb */

 /*===========================================================================

FUNCTION TM_RUIM_CDMA_SVC_INIT_REPORT

DESCRIPTION
  Accept status reports from R-UIM and prints out message for CDMA SVC table 
  check. This function is called by tm_task in reponse to IPC
  TM_CORE_MSG_ID_MMGSDI_CDMA_SVC_INITIALIZED


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_ruim_cdma_svc_init_report(void *p_Data)
{ 
  uim_svc_table_return_type support;
  boolean support_lbs = FALSE;
  tm_ruim_cdma_svc_table *cdma_svc_tbl = (tm_ruim_cdma_svc_table*) (p_Data);

  if(p_Data == NULL)
  {
    MSG_ERROR("Null CDMA Svc Table passed", 0, 0, 0);
    return;
  }
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_LBS,
                                               cdma_svc_tbl->data);

  if(support.allocated && support.activated)
  {
    support_lbs = TRUE;
  }
  else
  {
    support_lbs = FALSE;
  }

  nvruim_lbs_init_support(support_lbs);

#if 0 /*Workaround for CR625878 */
  if(support.allocated && support.activated)
  {
    /* re-Read LBS param's from RUIM card */
    (void) tm_core_ruim_read(); // Dynamic mode
    tm_1x_up_ruim_read();

#ifdef FEATURE_GPSONE_VX_LCS_AGENT
    tm_vx_ruim_read();
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */

#ifdef FEATURE_CGPS_XTRA
    tm_xtra_ruim_init_and_read();
#endif /* FEATURE_CGPS_XTRA */

  }
#endif /*Workaround for CR625878 */
  return; 
} /* tm_ruim_cdma_svc_init_report */
#endif /* FEATURE_MMGSDI_SESSION_LIB */


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION  TM_RUIM_MCFG_REFRESH_CB

DESCRIPTION
  Client task is expected to copy over the p_info and put the refresh update into 
its own command queue 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean tm_ruim_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info)
{
  uint8 sub = 0;

  if (p_info == NULL)
  {
    MSG_ERROR("GNSS MCFG_REFRESH got NULL ptr!", 0, 0, 0);
    return FALSE;
  }

  MSG_MED("GNSS MCFG_REFRESH got type=%d index=%d slot=%d", 
          (uint32)p_info->type, (uint32)p_info->slot_index, (uint32)p_info->sub_index);

  // save reference ID passed in by MCFG REFRESH, need it when reporting DONE
  tm_ruim_mcfg_refresh_reference_id = p_info->reference_id;

  sub = (uint8)p_info->sub_index; /* value 0,1 or 2 */

  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > tm_core_info.config_info.max_valid_subs_id) )
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NV Refresh Invalid sub %u. Abort Refresh on this SUB", sub);
    return FALSE;
  }
  
  /* send IPC to let TM refresh carrier-specific NV asynchronously, don't block MCFG_REFRESH */
  if (tm_ruim_ipc_send( TM_CORE_MSG_ID_REFRESH_CARRIER_SPECIFIC_NV, 
                       THREAD_ID_SM_TM, sizeof(uint8), &sub) != TRUE)
  {
    MSG_ERROR("Failed to send IPC for carrier specific NV refresh!", 0, 0, 0);
	return FALSE;
  }

//#ifndef FEATURE_MMGSDI_SESSION_LIB
#if 0   /* FIXME: Do we need this, FEATURE_MMGSDI_SESSION_LIB is always defined now */
  /* Send an IPC to TM to reading the CDMA Svc Table in TM task context,
     this will trigger NV-RUIM re-read asynchronously/eventually */
  if (tm_ruim_ipc_send( TM_CORE_MSG_ID_MMGSDI_CMD_READ_CDMA_SVC_TBL, 
                       THREAD_ID_SM_TM, 0, NULL) != TRUE)
  {
    MSG_ERROR(" GNSS MCFG_REFRESH failed to send IPC for CDMA_SVC_TBL read!", 0, 0, 0);
  }
#endif /* FEATURE_MMGSDI_SESSION_LIB */

  return TRUE;
}


/*===========================================================================

FUNCTION  TM_RUIM_MCFG_REFRESH_REGISTER

DESCRIPTION
  This function is used to register with MCFG to receive refresh update through
  registered callback function for MCFG switching
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void tm_ruim_mcfg_refresh_register(void)
{
  mcfg_refresh_registration_s_type gnss_mcfg_refresh = 
  {
    MCFG_REFRESH_ORDER_50,        /**< Refresh order ID registed for refresh */
    MCFG_REFRESH_INDEX_MASK_NONE, /**< Mask for slot(s) to recieve refresh update */
    MCFG_REFRESH_INDEX_MASK_ALL,  /**< Mask for sub(s) to recieve refresh update */
    0,                            /**< Reserved. */
    tm_ruim_mcfg_refresh_cb       /**< Callback function to receive refresh update */
  };

  boolean status = mcfg_refresh_register(&gnss_mcfg_refresh);

  if (status == TRUE)
  {
    MSG_MED("GNSS MCFG_REFRESH registration success", 0, 0, 0);
  }
  else
  {
    MSG_ERROR("GNSS MCFG_REFRESH registration fail!", 0, 0, 0);
  }
}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================

FUNCTION  TM_RUIM_CARRIER_NV_REFRESH_DONE

DESCRIPTION
  Called by TM task to notify TM-RUIM that Carrier-specific NV refresh done
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_ruim_carrier_nv_refresh_done ( void )
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* This function is used to notify MCFG task that registered client has
  completed refresh operation. The client is expected to return the same
  reference id passed through registered callback */
  (void)mcfg_refresh_done(tm_ruim_mcfg_refresh_reference_id);  
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
}

/*===========================================================================

FUNCTION  TM_RUIM_HANDLE_POST_E911

DESCRIPTION
  TM RUIM handling after E911 session
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_ruim_handle_post_e911 ( void )
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* to replay all the cached MMGSDI events */

  mmgsdi_events_enum_type cached_events[2];
  uint8 i;

  /* recontruct cached event order */
  if (tm_ruim_last_cached_event_is_session_close == TRUE)
  {
    cached_events[0] = MMGSDI_SESSION_CLOSE_EVT;
    /* skip Sub Ready event since 1x Session Close came later, 1x sub ready is not relevant anymore */
    tm_ruim_got_sub_ready_evt = FALSE;  
    tm_ruim_last_cached_event_is_session_close = FALSE; // reset to initial value
  }
  else
  {
    cached_events[0] = MMGSDI_SUBSCRIPTION_READY_EVT;  /* 1x Sub Ready event came, session should remain open */
  }
  cached_events[1] = MMGSDI_CARD_INSERTED_EVT;  /* backup: to open all remaining not-open sessions */

  // replay up to 3 cached events
  for (i = 0; i < sizeof(cached_events)/sizeof(cached_events[0]); i++)
  {
    switch (cached_events[i])
    {
      case MMGSDI_CARD_INSERTED_EVT:   /* for Card Inserted event */
        if (tm_ruim_got_card_inserted_evt == TRUE)
        {
          if (tm_ruim_event_cb_card_inserted_evt(&tm_ruim_cached_card_inserted_event) == TRUE)
          {
            tm_ruim_got_card_inserted_evt = FALSE;  // have replayed SIM inserted event
          }
        }
        break;

      case MMGSDI_SUBSCRIPTION_READY_EVT:   /* for 1x Sub Ready event */
        if (tm_ruim_got_sub_ready_evt == TRUE)
        {
          if (tm_ruim_event_cb_sub_ready_evt(&tm_ruim_cached_sub_ready_event) == TRUE)
          {
            tm_ruim_got_sub_ready_evt = FALSE;  // have replayed Sub Ready event
          }
        }
        break;

      case MMGSDI_SESSION_CLOSE_EVT:  /* for 1x Session Close event */
        if (tm_ruim_got_session_close_evt == TRUE)
        {
          if (tm_ruim_event_cb_session_close_evt(&tm_ruim_cached_session_close_event) == TRUE)
          {
            tm_ruim_got_session_close_evt = FALSE;  // have replayed Session Close event
          }
        }
        break;
      default: /* All other everts */
        /* Do Nothing */
        break;
    } /* switch */
  } /* for */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
}


/*===========================================================================

FUNCTION  TM_RUIM_GSDI_INIT

DESCRIPTION
  GSDI (generic SIM driver interface) Initialization function. This function does the following:
  - Registers the client call back function for GSDI events,
  - Register as a client of MMGSDI.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_ruim_gsdi_init ( void )
{
#ifdef FEATURE_MMGSDI_SESSION_LIB

  /* Register for MMGSDI client ID:
   */
  if ( MMGSDI_SUCCESS != mmgsdi_client_id_and_evt_reg( tm_ruim_event_cb,
                                                       tm_ruim_cmd_cb, 
                                                       0 ) )
  {
    MSG_ERROR("MMGSDI client_id_and_event registration failed", 0, 0, 0);
  }
#else

  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  /* Register the GSDI event callback function */
  gsdi_status = gsdi_reg_callback ( tm_ruim_gsdi_pin_event_cb_func );

  /* This should not happen, raise an error */
  if (gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR( "Could not register PIN event callback function with GSDI", 
               0, 0, 0 );
  }

  /* Register for MMGSDI client ID:
  */
  if ( MMGSDI_SUCCESS != mmgsdi_client_id_reg( tm_ruim_gsdi_client_id_reg_cb, 0 ) )
  {
    MSG_ERROR("MMGSDI client registration failed", 0, 0, 0);
  }
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  tm_ruim_mcfg_refresh_register();
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  return;
}/* tm_ruim_gsdi_init */


#endif /* FEATURE_GPSONE */

