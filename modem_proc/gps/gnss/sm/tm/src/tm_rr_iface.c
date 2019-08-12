/*===========================================================================

                            TM_RR_IFACE

DESCRIPTION


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_rr_iface.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
11/25/15    mc      Removed debug message inside tm_rr_iface_ue_capabilities_get()
09/11/14    mj      Update RR interface tm_rr_iface_send_event to handle asid
05/08/07    kgj     Initial Release
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"

#if defined (FEATURE_CGPS_UMTS_CP_GSM) && !defined (FEATURE_GNSS_LOW_MEMORY)

#include "rr_lsm_if.h"
#include "tm_rr_iface.h"
#include "tm_prtl_iface.h"
#include "tm_umts_cp_gsm_api.h"
#include "tm_common.h"
#include "tm_data.h"
#include "aries_os_api.h"
#include "err.h"
#include "gsclassmark.h"

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GLOBAL DATA DECLARATIONS                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DECLARATIONS                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DEFINITIONS                           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION tm_rr_iface_send_event

DESCRIPTION
  This function is used by GSM RR to send events and/or data to CGPS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void tm_rr_iface_send_event(
        const rr_cgps_event_data_type * event_data )
{
  prtl_rr_event_s_type    *rr_msg_ptr;
  os_IpcMsgType           *ipc_msg_ptr;
  boolean                 ipc_send_ok;

  MSG_MED( "TM_RR_IFACE: tm_rr_iface_send_event called", 0, 0, 0);

  /*
   *  Attempt to allocate an IPC Message
   *  IF unable to allocate THEN
   *    output an error message and return (or crash with a fatal)
   */
  ipc_msg_ptr = os_IpcCreate( sizeof(*rr_msg_ptr),
                              IPC_ALLOCATION_DYNAMIC,
                              (uint32) THREAD_ID_SM_TM );
  if( ipc_msg_ptr == NULL )
  {
    MSG_ERROR( "TM_RR_IFACE: Can't allocate Event IPC Message", 0, 0, 0);
    return;
  }

  /*
   *  Copy the event information from the incoming RR message to
   *  the IPC message.  In the process, convert the event message format
   *  from that which is known by RR to one used by TM.  This takes a little
   *  extra code, but it allows RR and TM to be a bit more decoupled and
   *  allows us to avoid including RR header files inside TM header files 
   *  and creating a rats nest of header dependencies.
   */
  rr_msg_ptr = (prtl_rr_event_s_type *) ipc_msg_ptr->p_Data;

  switch( event_data->event_id )
  {
    case RR_CGPS_APDU_MSG_EVENT:
      rr_msg_ptr->event_id                  = PRTL_RR_APDU_MSG_EVENT;
      rr_msg_ptr->event_payload.apdu.flags  = event_data->u.apdu.flags;
      rr_msg_ptr->event_payload.apdu.id     = event_data->u.apdu.id;
	  rr_msg_ptr->as_id						= event_data->as_id;
      
      rr_msg_ptr->event_payload.apdu.length =
	  	      memscpy( rr_msg_ptr->event_payload.apdu.data,
	  	             sizeof(rr_msg_ptr->event_payload.apdu.data),
              event_data->u.apdu.p_data,
              event_data->u.apdu.length );
      break;

    case RR_CGPS_APDU_ABORT_EVENT:
      rr_msg_ptr->event_id = PRTL_RR_APDU_ABORT_EVENT;
      break;

    case RR_CGPS_SESSION_ABORT_EVENT:
      rr_msg_ptr->event_id = PRTL_RR_SESSION_ABORT_EVENT;
      break;

    default:
      MSG_ERROR( "TM_RR_IFACE: Invalid RR->TM event_id=%d", event_data->event_id, 0, 0 );
      return;
  }

  /*
   *  Send the IPC Message to TM Core.  TM Core will a handler to handle
   *  this message when it is received.  The handler will reside in this
   *  (tm_rr_iface.c) file.
   */
  ipc_msg_ptr->q_MsgId = TM_CORE_RR_MSG_ID_EVENT_NTFY;
  
  ipc_send_ok = os_IpcSend( ipc_msg_ptr, (uint32) THREAD_ID_SM_TM );

  /*
   *  IF the IPC send failed for some reason THEN
   *    spit out an error message for debugging
   *    Delete the IPC message to avoid memory leaks
   */
  if( !ipc_send_ok )
  {
    (void)os_IpcDelete( ipc_msg_ptr );
    
    MSG_ERROR( "TM_RR_IFACE: Error sending RR message to TM thread", 0, 0, 0 );
  }
  else
  {
    MSG_HIGH( "TM_RR_IFACE: Sending RR Event %d to TM thread with as_id %d", rr_msg_ptr->event_id, (uint8)rr_msg_ptr->as_id, 0 );
  }
}


/*===========================================================================

FUNCTION tm_rr_iface_proc_event

DESCRIPTION
  This function is used by TM to process an RR Event Message.  This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_rr_iface_proc_event( void * p_msg )
{
  MSG_MED( "TM_RR_IFACE: tm_rr_iface_proc_event called", 0, 0, 0 );

  if( p_msg == NULL )
  {
    MSG_ERROR( "TM_RR_IFACE: Invalid NULL pointer in proc_event", 0, 0, 0 );
    return;
  }

  tm_core_rr_notify_event( (prtl_rr_event_s_type *) p_msg );
}

/*===========================================================================

FUNCTION tm_rr_iface_ue_capabilities_get

DESCRIPTION
  This function handles requests to query for UE positioning capabilities

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_rr_iface_ue_capabilities_get
(
  gsclassmark_cgps_pos_capability_struct_type   *p_pos_capability
)
{
  #ifdef FEATURE_CGPS_UMTS_CP_GSM
  (void)tm_umts_cp_gsm_ue_capabilities_get( p_pos_capability );
  #endif /* FEATURE_CGPS_UMTS_CP_GSM */
}

/*===========================================================================

FUNCTION tm_rr_iface_init

DESCRIPTION
  This function initializes the TM-RR IFACE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_rr_iface_init( void )
{
  MSG_MED( "TM_RR_IFACE: tm_rr_iface_init called", 0, 0, 0 );

  /*
   *  Register an event handler with RR
   */
  #ifndef FEATURE_CGPS_NO_UMTS_TDS
  rr_register_cgps_event_cb( tm_rr_iface_send_event );
  #endif

  /*
   *  Register a UE Positioning Capability Callback with GSCLASSMARK
   *  (see umts\services\utils\gsclassmark.h)
   */
  gsclassmark_register_cgps_cb( tm_rr_iface_ue_capabilities_get );
}

#endif  /* FEATURE_CGPS_UMTS_CP_GSM && ! FEATURE_GNSS_LOW_MEMORY */

