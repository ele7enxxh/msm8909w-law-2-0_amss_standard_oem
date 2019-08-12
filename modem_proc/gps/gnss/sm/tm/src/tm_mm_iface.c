/*===========================================================================

                            TM_MM_IFACE

DESCRIPTION


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_mm_iface.c#1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  05/30/07    lt      Mods. to make TM's RRC and MM iface symmetrical to CM and RR. 
  05/30/07    lt      Initial check in.
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"

#ifdef FEATURE_CGPS_UMTS_CP

#include "mm_lsm.h"

#include "err.h"
#include "event.h"
#include "tm_common.h"
#include "tm_data.h"


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

FUNCTION tm_mm_iface_send_event

DESCRIPTION
  This function is used by MM to send events and/or data to CGPS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_mm_iface_send_event( mm_cgps_event_data_s_type*  p_mm_event_data )
{
  os_IpcMsgType              *ipc_msg_ptr = NULL;

  ipc_msg_ptr = os_IpcCreate(sizeof(mm_cgps_event_data_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr == NULL)
  {
    MSG_ERROR( "IPC message allocation failed",0,0,0 );
    return FALSE;
  }
  else 
  {

    ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_MM;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(mm_cgps_event_data_s_type);

    /* Note for MM, p_mm_event_data->u.z_mm_cgps_msg_data.p_data is NULL, 
       the only information needed to be passed through IPC is in 
       p_mm_event_data->e_event_type  */

    memscpy( (uint8 *)ipc_msg_ptr->p_Data, 
            ipc_msg_ptr->q_Size,
            (uint8 *)p_mm_event_data, 
            ipc_msg_ptr->q_Size ); 

    if ( os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM) == FALSE )
    {
      MSG_ERROR("Error in sending MM msg to TM thread",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      ipc_msg_ptr = NULL;
    } /* End status */
    else
    {
      MSG_HIGH("sending MM msg to TM thread", 0,0,0);
    } /* end else status */

  }

  return FALSE;
}


/*===========================================================================

FUNCTION tm_mm_iface_proc_event

DESCRIPTION
  This function is used by TM to process an MM Event Message.  This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_mm_iface_proc_event( void *p_msg )
{
  prtl_mm_event_s_type  z_tm_mm_event;
  if( p_msg == NULL )
  {
    MSG_ERROR( "TM_RRC_IFACE: Invalid NULL pointer in proc_event", 0, 0, 0 );
    return;
  }

  z_tm_mm_event.mm_event_data_ptr = (mm_cgps_event_data_s_type *)p_msg; 

  MSG_MED( "TM_RRC_IFACE: tm_rr_iface_proc_event called", 0, 0, 0 );

  tm_core_mm_notify_event( &z_tm_mm_event );
}



/*===========================================================================

FUNCTION tm_mm_iface_init

DESCRIPTION
  This function initializes the TM-MM IFACE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_mm_iface_init( void )
{
  /* Register an event handler with RRC */
  #ifndef FEATURE_CGPS_NO_UMTS_TDS
  if ( mm_register_cgps_event_cb( tm_mm_iface_send_event ) == FALSE )
  {
    MSG_ERROR("tm_rrc_iface: event_cb reg. failed", 0, 0, 0);
  }
  #endif

}

#endif  /* FEATURE_CGPS_UMTS_CP */

