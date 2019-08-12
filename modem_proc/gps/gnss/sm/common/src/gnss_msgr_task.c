/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GNSS Message Router

GENERAL DESCRIPTION

This module contains implemenation of GNSS Message Router


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.

  Copyright (c) 2016 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/gnss_msgr_task.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/29/16   kpn     Use of subscription ID while handling NAS DL message for LPP CP
05/08/14   rk      Fixed LPP UP & CP KW errors show-up on BO.1.0/DPM.1.0 PL.
02/08/13   ssu      Move to the DOG Heartbeat mechanism on DIME and beyond. 
11/13/12   rh      Added new NAS error code support for send failure case
04/06/12   rh      Initial creation of file.
======================================================================*/

#include "customer.h"
#include "comdef.h"

#include "task.h"
#include "sys.h"
#include "msg.h"
#include "queue.h"
#include "dsm_pool.h"
#include "msgr.h"
#include "msgr_rex.h"
#include "emm_gps_if_msg.h"
#include "lte_nas_common.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "gps_types.h"
#include "gps_common.h"

#include "gnss_msgr.h"
#include "tm_prtl_iface.h"


/****************** Constants/Enums/Macros ************************************/

typedef enum
{
  GNSS_MSGR_TIMER_ID_DOG = C_USR_MSG_ID_FIRST+1,
} gnss_msgr_ipc_message_type;


/* Create TECH MODULE MACRO FOR GNSS MSGR TASK */
#define MSGR_GPS_TM   MSGR_TECH_MODULE(MSGR_TECH_GPS, THREAD_ID_GNSS_MSGR)

#define GNSS_MSGR_NTOHL(x) \
  ((uint32)((((uint32)(x) & 0x000000FFU) << 24) | \
  (((uint32)(x) & 0x0000FF00U) <<  8) | \
  (((uint32)(x) & 0x00FF0000U) >>  8) | \
  (((uint32)(x) & 0xFF000000U) >> 24)))

#define GNSS_MSGR_HTONL(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
        (((uint32)(x) & 0x0000ff00U) <<  8) | \
        (((uint32)(x) & 0x00ff0000U) >>  8) | \
        (((uint32)(x) & 0xff000000U) >> 24)))


/****************** Variables ************************************************/
static boolean u_gnss_msgr_task_online;

/* Variable to indicate if MSGR is successfully created */
static boolean  gnss_msgr_is_client_created = FALSE;

/* Variable to indicate if NASS MSGR is successfully registered */
static boolean gnss_msgr_is_nas_msg_registered = FALSE;

/* MSGR RECEIVE QUEUE SIZE */
#define GNSS_MSGR_RCV_QUEUE_SIZE  10

typedef struct
{
  q_link_type  qlink;

} gnss_msgr_rcv_msg_hdr_type;

/* Messages which can be received over MSGR queue. We can receive messages over MSGR 
 * NAS only for now
 */
typedef union
{
  /* Generic MSGR header
   */
  msgr_hdr_struct_type                           hdr;

#ifdef FEATURE_LTE

  /* NAS Downlink LPP Message. This is from NAS->GNSS
   */
  lte_nas_emm_dl_generic_transport_s_type        nas_dl_lpp;

  /* NAS CNF Message for Uplink Sending. This is from NAS->GNSS
   */
  lte_nas_emm_ul_generic_transport_cnf_type      nas_ul_cnf;

  /* NAS Failure Message. This is from NAS->GNSS
   */
  emm_irat_failure_ind_type                      nas_failure_ind;

#endif /* FEATURE_LTE */

} gnss_msgr_rcv_msg_msgs_type;


/* Format of messages received over MSGR */
typedef struct
{
  gnss_msgr_rcv_msg_hdr_type  msg_hdr;
  gnss_msgr_rcv_msg_msgs_type msg;
} gnss_msgr_rcv_msg_type;


typedef struct
{
  gnss_msgr_rcv_msg_type   q_items[ GNSS_MSGR_RCV_QUEUE_SIZE ];
  q_type                   msg_q;
  q_type                   free_q;
} gnss_msgr_rcv_buffer_type;

/* Buffer to receive messages over MSGR */
static gnss_msgr_rcv_buffer_type  gnss_msgr_rcv;


/****************** Function Definitions***************************************/


/*===========================================================================

FUNCTION       GNSS_MSGR_INIT_QUEUE
 
DESCRIPTION    This function initializes the receive queue 
 
DEPENDENCIES   None. 
 
RETURN VALUE   None. 
 
SIDE EFFECTS   None. 
 
===========================================================================*/
static void gnss_msgr_init_queue( void )
{
   uint8 i;
    /* Initialize the Command Queue, and the Command Free Queue */
  (void) q_init( &gnss_msgr_rcv.msg_q );
  (void) q_init( &gnss_msgr_rcv.free_q );

  /* Link the Command Queue Items onto the Command Free Queue */
  for (i=0; i<GNSS_MSGR_RCV_QUEUE_SIZE; i++)
  {
    q_put( &gnss_msgr_rcv.free_q,
           q_link( &gnss_msgr_rcv.q_items[i],
           &gnss_msgr_rcv.q_items[i].msg_hdr.qlink ) );
  }/* init_queue */

}

/*===========================================================================

FUNCTION       GNSS_MSGR_GET_CLIENT_ID

DESCRIPTION    This function retuns the msgr client id.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static msgr_client_t*  gnss_msgr_get_client_id( void )
{
  // MSGR client ID
  static msgr_client_t gnss_msgr_client_id;

  return &gnss_msgr_client_id;

} /*  gnss_msgr_client_id */


/*===========================================================================

FUNCTION       GNSS_MSGR_GET_QUEUE_ID

DESCRIPTION    This function retuns the msgr queue id.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static msgr_id_t*  gnss_msgr_get_queue_id( void )
{
  // MSGR Queue ID
  static msgr_id_t gnss_msgr_queue_id;

  return &gnss_msgr_queue_id;

} /*  gnss_msgr_get_queue_id */


/*===========================================================================
FUNCTION       gnss_msgr_setup_client
 
DESCRIPTION    This function setups the GNSS client with the MSGR.
 
DEPENDENCIES   Queues must be initialized.
 
RETURN VALUE   Boolean: TRUE for success, FALSE for failure
 
SIDE EFFECTS   None. 
 
===========================================================================*/
static boolean gnss_msgr_setup_client( void )
{
  errno_enum_type  estatus;
  msgr_client_t* gnss_msgr_client_id_ptr = gnss_msgr_get_client_id();

  /*-----------------------------------------------------------------------*/
  if (gnss_msgr_client_id_ptr == NULL )
  {
    MSG_ERROR("GNSS MSGR: Failed to get client ID", 0, 0, 0);
    return FALSE;
  }

  estatus = msgr_client_create( gnss_msgr_client_id_ptr );
  if ( estatus != E_SUCCESS )
  {
    MSG_ERROR("GNSS MSGR: Failed to create GNSS MSGR client error=%d", (uint32)estatus, 0, 0);
    return FALSE;
  }

  estatus = msgr_client_add_rex_q ( gnss_msgr_client_id_ptr,
                                    os_Self(),  // will return own TCB
                                    C_OS_FLAG_MSGR_MSG,  // OSAPI will convert this to IPC
                                    &gnss_msgr_rcv.msg_q,
                                    &gnss_msgr_rcv.free_q,
                                    offsetof( gnss_msgr_rcv_msg_type, msg ),
                                    MSGR_NO_OFFSET,
                                    (uint16)sizeof(gnss_msgr_rcv_msg_type ),
                                    gnss_msgr_get_queue_id() );

  if ( estatus != E_SUCCESS )
  {
    MSG_ERROR("GNSS MSGR: Failed to add rex_q for GNSS MSGR error=%d", (uint32)estatus, 0, 0);

    msgr_client_delete( gnss_msgr_client_id_ptr );
    if ( estatus != E_SUCCESS )
    {
      MSG_ERROR("GNSS MSGR: Failed to delete GNSS MSGR client error=%d", (uint32)estatus, 0, 0);
    }

    return FALSE;
  }
  else
  {
    gnss_msgr_is_client_created = TRUE;

    return TRUE;
  }

} /* setup_client */


/*===========================================================================

FUNCTION      GNSS_MSGR_MODULE_INIT

DESCRIPTION   This function initializes GNSS MSGR module

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None

===========================================================================*/
static void gnss_msgr_module_init(void)
{
  /* Initialize the Queues */
  gnss_msgr_init_queue();

  /* Create GNSS MSGR client */
  if (gnss_msgr_setup_client() == TRUE)
  {
    MSG_MED("GNSS MSGR: client successfully created",0,0,0);
  }
  else
  {
    MSG_ERROR("GNSS MSGR: client creation failed!",0,0,0);
  }
}


/*===========================================================================
FUNCTION        gnss_msgr_start_up

DESCRIPTION     This function sets up IPC and creates timers.
  
RETURN VALUE

DEPENDENCIES 
 
=============================================================================*/
static void gnss_msgr_start_up(void)
{
  (void)os_IpcStart((uint32)THREAD_ID_GNSS_MSGR);
  
  /* Create wdog timer first.
  */
#ifndef __GRIFFON__
/* Create wdog timer first.
  */

  /* Register for DOG heartbeat monitoring */
  os_DogHeartbeatRegister();
  u_gnss_msgr_task_online = TRUE;
#endif  /* __GRIFFON__ */

} /* End gnss_msgr_start_up() */

/*===========================================================================
FUNCTION        GNSS_MSGR_HANDLE_TASK_OFFLINE

DESCRIPTION     This function is used to handle TASK_OFFLINE IPC

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None 
 
=============================================================================*/
static void gnss_msgr_handle_task_offline( void )
{
  u_gnss_msgr_task_online = FALSE;

  MSG_HIGH("GNSS MSGR: TASK OFFLINE received",0,0,0);

}


/*===========================================================================
FUNCTION        GNSS_MSGR_HANDLE_TASK_STOP

DESCRIPTION     This function is used to handle Stop IPC

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None 
 
=============================================================================*/
static void gnss_msgr_handle_task_stop( void )
{
  MSG_HIGH("GNSS MSGR: TASK STOP received",0,0,0);

  /* Deregister with the Dog Heartbeat before stopping task */
  os_DogHeartbeatDeregister();

  os_TaskStopAck();
}


/*===========================================================================
FUNCTION        GNSS_MSGR_SEND_NAS_DL_EVENT

DESCRIPTION     This function is used to send NAS download event to TM

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None 
 
=============================================================================*/
static boolean gnss_msgr_send_nas_dl_event(uint32 msg_count)
{
  boolean retVal = TRUE;

  os_IpcMsgType                  *ipc_msg_ptr;

  // let TM core send out NAS msg ready event to LPP CP module

  /* Create a message with no payload */
  ipc_msg_ptr = os_IpcCreate(sizeof(uint32), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_NAS_DL_DATA_EVENT;    
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    // 4 bytes prtl_lpp_cp_event_s_type.lpp_cp_event_data for msg_count
    memscpy((void*)&(ipc_msg_ptr->p_Data), sizeof(msg_count), (void*)&(msg_count), sizeof(msg_count));
    
    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("GNSS MSGR: Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      retVal = FALSE;
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("GNSS MSGR: Failed to create IPC mesage",0,0,0); 
    retVal = FALSE;
  } /* End ipc_msg_ptr */

  return retVal;
}


#ifdef FEATURE_LTE

/*===========================================================================
FUNCTION        GNSS_MSGR_SEND_NAS_FAILURE_EVENT

DESCRIPTION     This function is used to send NAS failure event to TM

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None 
 
=============================================================================*/
static boolean gnss_msgr_send_nas_failure_event(uint32 error_code)
{
  boolean retVal = TRUE;

  os_IpcMsgType                  *ipc_msg_ptr;

  // let TM core send out NAS failure event to LPP CP module

  /* Create a message with 4-byte payload */
  ipc_msg_ptr = os_IpcCreate(sizeof(uint32), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_NAS_FAILURE_EVENT;    
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    // 4 bytes prtl_lpp_cp_event_s_type.lpp_cp_event_data for msg_count
    memscpy((void*)&(ipc_msg_ptr->p_Data), sizeof(error_code), (void*)&(error_code), sizeof(error_code));

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("GNSS MSGR: Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      retVal = FALSE;
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("GNSS MSGR: Failed to create IPC mesage",0,0,0); 
    retVal = FALSE;
  } /* End ipc_msg_ptr */

  return retVal;
}

#endif /* FEATURE_LTE */

/*===========================================================================
FUNCTION        GNSS_MSGR_HANDLE_MSGR_MSGS

DESCRIPTION     This function is used to handle MSGR router messages

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None 
 
=============================================================================*/
static void gnss_msgr_handle_msgr_msg(void)
{
  uint32 msg_cnt = (uint32)q_cnt(&gnss_msgr_rcv.msg_q);

  if (q_check(&gnss_msgr_rcv.msg_q) != NULL)
  {
    MSG_MED("GNSS MSGR: Got MSGR message(cnt=%d), sending NAS_DL_EVENT...", msg_cnt,0,0);

    (void)gnss_msgr_send_nas_dl_event(msg_cnt);
  }
}


/*===========================================================================
FUNCTION      GNSS_MSGR_PROC

DESCRIPTION   GNSS MSGR task main IPC processing routine
  
RETURN VALUE

DEPENDENCIES 
 
=============================================================================*/
void gnss_msgr_proc(void)
{
  os_IpcMsgType         *ipc_msg_ptr;
  
  for(;;)
  {
    ipc_msg_ptr = os_IpcReceive();
    if( NULL == ipc_msg_ptr )
    {
      MSG_ERROR("GNSS MSGR: NULL pointer rcvd from os_IpcReceive()", 0, 0, 0);
      continue;
    }
  
    if (u_gnss_msgr_task_online)
    {
      switch (ipc_msg_ptr->q_MsgId)
      {    
        /*Handle task maintenance messages*/
        case C_OS_MSG_ID_OFFLINE: 
          gnss_msgr_handle_task_offline();
          break;
      
        case C_OS_MSG_ID_STOP: 
          gnss_msgr_handle_task_stop();
          break;

        case C_OS_MSG_ID_MSGR_MSG:
          gnss_msgr_handle_msgr_msg();
          break;

        case C_OS_MSG_ID_DOG_HB:
        {
          // Respond to the dog heart beat. Pat the dog. 
          os_DogHeartbeatReport();
          break;
        }

        default:
          MSG_ERROR("GNSS MSGR: Unknown IPC MSG ID",0,0,0);
          break;
      }/* End switch MsgId */
    }
    (void)os_IpcDelete(ipc_msg_ptr); 

  }/* End receive */

}/* End gnss_msgr_Proc() */


/*===========================================================================
FUNCTION      gnss_msgr_get_ready_flag

DESCRIPTION   Return whether GNSS MSGR task is ready or not 
 
RETURN VALUE

DEPENDENCIES
=============================================================================*/
boolean gnss_msgr_get_ready_flag(void)
{
  return u_gnss_msgr_task_online;
}


/*===========================================================================
FUNCTION      gnss_msgr_task

DESCRIPTION   GNSS MSGR TASK entry point 
 
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void gnss_msgr_task(
  dword dummy                     /* Rex requirement, not used */
)
{
  (void)dummy;
    
  MSG_HIGH("GNSS MSGR: Starting GNSS MSGR task (ThreadID=%d)...", THREAD_ID_GNSS_MSGR,0,0);

  os_TaskReadyAck();
  os_TaskStopRegister();

  /* Set up IPC and timers */
  gnss_msgr_start_up();
  
  /* GM module initialization */
  gnss_msgr_module_init();

  gnss_msgr_proc(); 

} /* End tm_Task() */


/*===========================================================================

FUNCTION       gnss_msgr_send_msg

DESCRIPTION    This function initializes the header and sends the message to
               the message router.

DEPENDENCIES   None.

RETURN VALUE   Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS   None.

===========================================================================*/
boolean gnss_msgr_send_msg
(
  msgr_hdr_struct_type*     msg_hdr_ptr,
  msgr_umid_type            umid,
  int32                     size
)
{
  errno_enum_type  estatus;

  /*-----------------------------------------------------------------------*/

  if ( msg_hdr_ptr == NULL )
  {
    MSG_ERROR("GNSS MSGR: msg_hdr_ptr is NULL!", 0, 0, 0);
    return FALSE;
  }

  /* Initialize the message header */
  msgr_init_hdr(msg_hdr_ptr, MSGR_GPS_TM, umid);

  estatus = msgr_send( msg_hdr_ptr, size );

  if (estatus == E_SUCCESS )
  {
    MSG_HIGH("GNSS MSGR: sent msg with umid=%d size=%d",umid,size,0);
    return TRUE;
  }
  else
  {
    MSG_ERROR("GNSS MSGR: Failed to send msg with umid=%d size=%d",umid,size,0);
    return FALSE;
  }
}


/*=========================================================================
FUNCTION       gnss_msgr_register_nas_msgs
 
DESCRIPTION    This function registers a message that GNSS is interested in 
               collecting from the message router
 
DEPENDENCIES   GNSS MSGR client setup with the MSGR must have been completed.
 
RETURN VALUE   Boolean: TRUE for success, FALSE for failure
 
SIDE EFFECTS   None 
 
===========================================================================*/
boolean gnss_msgr_register_nas_msgs( void )
{
#ifdef FEATURE_LTE

  errno_enum_type  estatus;
  msgr_client_t*   gnss_msgr_client_id_ptr = gnss_msgr_get_client_id();

  if (gnss_msgr_is_client_created != TRUE)
  {
    MSG_ERROR("GNSS MSGR: Can't registering for NAS MSGR messages since GNSS client not created!",0,0,0);
    return FALSE;
  }

  if (gnss_msgr_is_nas_msg_registered == TRUE)
  {
    MSG_ERROR("GNSS MSGR: NAS MSGR messages already registered!",0,0,0);
    return FALSE;
  }

  MSG_HIGH("GNSS MSGR: Registering for NAS MSGR messages",0,0,0);

  estatus = msgr_register( MSGR_GPS_TM,
                           gnss_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND ); // downlink NAS msg
  if ( estatus != E_SUCCESS )
  {
    MSG_ERROR("GNSS MSGR: Failed to register NAS DL message error=%d", (uint32)estatus, 0, 0);
    return FALSE;
  }

  /* now trying to register for UL confirmation */
  estatus = msgr_register( MSGR_GPS_TM,
                           gnss_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF ); // success confirmation for uplink NAS msg

  if ( estatus != E_SUCCESS )
  {
    // de-register the first one
    (void)msgr_deregister( MSGR_GPS_TM,                            
                           gnss_msgr_client_id_ptr,
                           NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND ); 

    MSG_ERROR("GNSS MSGR: Failed to register NAS UL CNF message error=%d", (uint32)estatus, 0, 0);
    return FALSE;
  }

  /* now trying to register failure indication */
  estatus = msgr_register( MSGR_GPS_TM,
                           gnss_msgr_client_id_ptr,
                           MSGR_ID_REX,
                           NAS_EMM_GENERIC_NAS_TRANSPORT_FAILURE_IND ); // failure for NAS transport

  if ( estatus != E_SUCCESS )
  {
    // de-register the first two messages
    (void)msgr_deregister( MSGR_GPS_TM,                            
                           gnss_msgr_client_id_ptr,
                           NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND ); 

    (void)msgr_deregister( MSGR_GPS_TM,                            
                           gnss_msgr_client_id_ptr,
                           NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF ); 

    MSG_ERROR("GNSS MSGR: Failed to register NAS failure IND message error=%d", (uint32)estatus, 0, 0);
    return FALSE;
  }

  gnss_msgr_is_nas_msg_registered = TRUE;

  return TRUE;

#else  /* FEATURE_LTE */

  MSG_HIGH("GNSS MSGR: LTE not enabled, can't register for NAS MSGR messages",0,0,0);

  return FALSE;

#endif /* FEATURE_LTE */

} /* gnss_msgr_register_nas_msgs */


/*===========================================================================
FUNCTION      gnss_msgr_deregister_nas_msgs

DESCRIPTION   Deregister NAS's MSGR messages
 
RETURN VALUE  Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS  None 
 
=============================================================================*/
boolean gnss_msgr_deregister_nas_msgs(void)
{
#ifdef FEATURE_LTE

  errno_enum_type  estatus;
  msgr_client_t*   gnss_msgr_client_id_ptr = gnss_msgr_get_client_id();
  boolean bRet = TRUE;

  if (gnss_msgr_is_client_created != TRUE)
  {
    MSG_ERROR("GNSS MSGR: Can't deregistering for NAS MSGR messages since GNSS client not created!",0,0,0);
    return FALSE;
  }

  if (gnss_msgr_is_nas_msg_registered != TRUE)
  {
    MSG_ERROR("GNSS MSGR: NAS MSGR messages not registered!",0,0,0);
    return FALSE;
  }

  MSG_HIGH("GNSS MSGR: Deregistering for NAS MSGR messages",0,0,0);

  estatus = msgr_deregister( MSGR_GPS_TM,                            
                             gnss_msgr_client_id_ptr,
                             NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND ); // downlink NAS msg
  if ( estatus != E_SUCCESS )
  {
    MSG_ERROR("GNSS MSGR: Failed to deregister NAS DL message error=%d", (uint32)estatus, 0, 0);
    bRet = FALSE;
  }

  estatus = msgr_deregister( MSGR_GPS_TM,
                             gnss_msgr_client_id_ptr,
                             NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF ); // confirmation for uplink NAS msg
  if ( estatus != E_SUCCESS )
  {
    MSG_ERROR("GNSS MSGR: Failed to deregister NAS UL CNF message error=%d", (uint32)estatus, 0, 0);
    bRet = FALSE;
  }

  estatus = msgr_deregister( MSGR_GPS_TM,
                             gnss_msgr_client_id_ptr,
                             NAS_EMM_GENERIC_NAS_TRANSPORT_FAILURE_IND ); // NAS failure indication
  if ( estatus != E_SUCCESS )
  {
    MSG_ERROR("GNSS MSGR: Failed to deregister NAS failure IND message error=%d", (uint32)estatus, 0, 0);
    bRet = FALSE;
  }

  gnss_msgr_is_nas_msg_registered = FALSE;

  return bRet;

#else /* FEATURE_LTE */

  gnss_msgr_is_nas_msg_registered = FALSE;

  return FALSE;

#endif /* FEATURE_LTE */

}

/*===========================================================================
FUNCTION       GNSS_MSGR_SEND_NAS_MSG

DESCRIPTION    Send MSGR message to NAS
 
RETURN VALUE   Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS   None
=============================================================================*/
boolean gnss_msgr_send_nas_msg(
  uint8 *pkt_ptr, 
  uint32 pkt_size, 
  uint32 nas_id,
  uint16 trans_id,
  uint8 inst_id
)
{
#ifdef FEATURE_LTE

  errno_enum_type  estatus;
  lte_nas_emm_ul_generic_transport_s_type ul_nas_msg;
  dsm_item_type *lpp_dsm_ptr = NULL,*nasid_dsm_ptr = NULL;
  msgr_attach_struct_type *lpp_att_ptr = NULL, *nasid_att_ptr = NULL;  
  uint32 num_bytes = 0;
  uint32 nasid_be, nasid_bytes = sizeof(nasid_be); // 4 byte NAS ID (Big Endian)
  uint32 nas_transid; // 4 byte NAS/RRC trans ID

  if (gnss_msgr_is_client_created != TRUE)
  {
    MSG_ERROR("GNSS MSGR: Can't send NAS MSGR message since GNSS client not created!",0,0,0);
    return FALSE;
  }

  if (pkt_ptr == NULL || pkt_size == 0 || pkt_size > 65535) // 16 bit len, up to 65535
  {
    MSG_ERROR("GNSS MSGR: invalid NAS MSGR message(size=%d), can't send!",pkt_size,0,0);
    return FALSE;
  }
  
  memset(&ul_nas_msg, 0, sizeof(ul_nas_msg));

  //MSGR_PRIORITY_HIGH=0x7F MSGR_PRIORITY_LOW=0x00 NAS is using LOW priority?
  msgr_init_hdr_attach((msgr_hdr_s *)(&(ul_nas_msg.msg_hdr)),
                        MSGR_GPS_TM, NAS_EMM_UL_GENERIC_NAS_TRANSPORT_REQ, MSGR_PRIORITY_HIGH, 2);

  msgr_set_hdr_inst((msgr_hdr_s *)(&(ul_nas_msg.msg_hdr)), inst_id);

  lpp_att_ptr = msgr_get_attach(&(ul_nas_msg.msg_hdr), 0);
  nasid_att_ptr = msgr_get_attach(&(ul_nas_msg.msg_hdr), 1);
  if ((lpp_att_ptr == NULL) || (nasid_att_ptr == NULL))
  {
    MSG_ERROR("GNSS MSGR: NULL MSGR attachment ptr! LPP=0x%x NASID=0x%x",lpp_att_ptr,nasid_att_ptr,0);
    return FALSE;
  }

  MSG_HIGH("GNSS MSGR: NAS_EMM_UL_GENERIC_NAS_TRANSPORT_REQ payload (DSM size=%d)",pkt_size,0,0);

  /* Pushdown DSM */
  num_bytes = dsm_pushdown(&lpp_dsm_ptr, (void *)(pkt_ptr), (uint16)(pkt_size), 
                           DSM_DS_POOL_SIZE(pkt_size));
  if (num_bytes != pkt_size || lpp_dsm_ptr == NULL)
  {
    MSG_ERROR("GNSS MSGR: Only %d bytes pushed to DSM 0x%x (%d payload)", num_bytes, lpp_dsm_ptr, pkt_size);
    dsm_free_packet(&lpp_dsm_ptr); // can take NULL pointer
    return FALSE;
  }
  else
  {
    /* Set DSM attach */
    msgr_set_dsm_attach(lpp_att_ptr, lpp_dsm_ptr);
  }

  // NAS ID is 4-byte
  nasid_be = GNSS_MSGR_HTONL(nas_id); // convert from host(little) endian to network(big) endian
  num_bytes = dsm_pushdown(&nasid_dsm_ptr, (void *)(&nasid_be), (uint16)(nasid_bytes), 
                           DSM_DS_POOL_SIZE(nasid_bytes));
  if (num_bytes != nasid_bytes || nasid_dsm_ptr == NULL)
  {
    MSG_ERROR("GNSS MSGR: Only %d bytes pushed to DSM 0x%x (%d payload)", num_bytes, nasid_dsm_ptr, nasid_bytes);
    dsm_free_packet(&lpp_dsm_ptr); // can take NULL pointer
    dsm_free_packet(&nasid_dsm_ptr); // can take NULL pointer
    return FALSE;
  }
  else
  {
    /* Set DSM attach */
    msgr_set_dsm_attach(nasid_att_ptr, nasid_dsm_ptr);
  }

  /*
  typedef enum
  {
    LPP_MSG_CONTAINER = 1,
    LOCATION_SERVICE_MSG_CONTAINER = 2,
  }lte_nas_generic_msg_container_type;
  */
  /* NAS/RRC TransactionID: MSB1 is 0x01 (NAS_LTE) MSB2 is 0x04(LTE_NAS_GPS), last two bytes are local */
  nas_transid = ((uint32)NAS_LTE << 24) | ((uint32)LTE_NAS_GPS << 16) | (uint32)trans_id;
  ul_nas_msg.generic_msg_container_type = LPP_MSG_CONTAINER;
  ul_nas_msg.trans_id = nas_transid;

  estatus = msgr_send((msgr_hdr_s *)&ul_nas_msg, sizeof(lte_nas_emm_ul_generic_transport_s_type));

#ifndef FEATURE_SMART_GNSS_MSG_ROUTER
  if (estatus != E_SUCCESS )
  {
    MSG_ERROR("GNSS MSGR: Failed to send NAS msg with NASID=%d size=%d transid=0x%x",nas_id,pkt_size,nas_transid);
    dsm_free_packet(&lpp_dsm_ptr); // can take NULL pointer
    dsm_free_packet(&nasid_dsm_ptr); // can take NULL pointer
    return FALSE;
  }
#else
  /* Below are off-target testing code for SMART */
  {
    extern boolean smart_lpp_cp_send_nas_msg(uint8 *pkt_ptr, uint32 pkt_size, uint32 nas_id, uint8 inst_id);

    // for SMART, send packet to LPP CP mini server
    smart_lpp_cp_send_nas_msg(pkt_ptr, pkt_size, nas_id, inst_id);

    // for SMART simulation, always free the DSM packet (otherwise memory leak)
    dsm_free_packet(&lpp_dsm_ptr); // can take NULL pointer
    dsm_free_packet(&nasid_dsm_ptr); // can take NULL pointer
  }
#endif

  MSG_HIGH("GNSS MSGR: sent NAS msg with NASID=%d size=%d trans_id=0x%x",nas_id,pkt_size,nas_transid);
  return TRUE;

#else /* FEATURE_LTE */

  MSG_HIGH("GNSS MSGR: LTE not enabled, can't send NAS MSGR messages",0,0,0);

  return FALSE;

#endif /* FEATURE_LTE */

}


#ifndef FEATURE_SMART_GNSS_MSG_ROUTER

/*===========================================================================
FUNCTION       GNSS_MSGR_RECEIVE_NAS_MSG

DESCRIPTION    Receive NAS MSGR message from Queue
 
RETURN VALUE   Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS   None
=============================================================================*/
boolean gnss_msgr_receive_nas_msg(
  uint8 *pkt_buf, 
  uint32 buf_size, 
  uint32 *pkt_size,
  uint32 *nas_id, 
  uint8 *inst_id
)
{
#ifdef FEATURE_LTE

  boolean bRet = TRUE;
  gnss_msgr_rcv_msg_type* msg_ptr = NULL;
  dsm_item_type           *lpp_dsm_ptr = NULL, *nasid_dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *lpp_att_ptr = NULL,*nasid_att_ptr = NULL;         /* Message attachment pointer */
  uint32 dsm_packet_length = 0; /*Used to calculate the whole length of UL message*/
  uint32 nasid_bytes = 0;
  uint8 num_attach = 0;
  uint16 dsm_pull_bytes = 0;

  if (gnss_msgr_is_client_created != TRUE)
  {
    MSG_ERROR("GNSS MSGR: Can't send NAS MSGR message since GNSS client not created!",0,0,0);
    return FALSE;
  }

  if (pkt_buf == NULL || buf_size == 0 || pkt_size == NULL || nas_id == NULL || inst_id == NULL) 
  {
    MSG_ERROR("GNSS MSGR: invalid parameter (buf_size=%d)!",buf_size,0,0);
    return FALSE;
  }

  *pkt_size = 0; // set it to 0 first
  *nas_id = 0; 
  *inst_id = 0;

  // receive MSGR message from Queue
  msg_ptr = (gnss_msgr_rcv_msg_type*)q_get(&gnss_msgr_rcv.msg_q);
  if (msg_ptr == NULL)
  {
    MSG_HIGH("GNSS MSGR: No NAS message available!!",0,0,0);
    return FALSE;
  }

  //NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND
  //NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF
  if (msg_ptr->msg.hdr.id == NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND)
  {  
    MSG_HIGH("GNSS MSGR: received DL_GENERIC_NAS_TRANSPORT msg",0,0,0);

    do
    {
      num_attach = msgr_get_num_attach(&msg_ptr->msg.hdr);
      MSG_LOW("GNSS MSGR: No of attach items for pull up = %d",num_attach,0,0);

      *inst_id = (uint8) MSGR_GET_INST(&msg_ptr->msg.hdr);

      if (num_attach != 2 ||
          (lpp_att_ptr = msgr_get_attach(&msg_ptr->msg.hdr, 0)) == NULL ||
          (nasid_att_ptr = msgr_get_attach(&msg_ptr->msg.hdr, 1)) == NULL)
      {
        MSG_ERROR("GNSS MSGR: wrong attachments! num=%d LPP=0x%x NASID=0x%x", 
          num_attach, lpp_att_ptr, nasid_att_ptr);
        bRet = FALSE;
        break;
      }

      // get DSM attachments
      msgr_get_dsm_attach(lpp_att_ptr, &lpp_dsm_ptr);
      msgr_get_dsm_attach(nasid_att_ptr, &nasid_dsm_ptr);
      if (lpp_dsm_ptr == NULL || nasid_dsm_ptr == NULL)
      {  
        MSG_ERROR("GNSS MSGR: NULL DSM ptr! LPP=0x%x NASID=0x%x!",lpp_dsm_ptr,nasid_dsm_ptr,0);
        bRet = FALSE;
        break;
      }

      /*Check the length for Generic Message Container*/
      dsm_packet_length  = dsm_length_packet(lpp_dsm_ptr);
      if (dsm_packet_length > buf_size)
      {
        MSG_ERROR("GNSS MSGR: buf size %d too small (LPP msg size %d)", buf_size, dsm_packet_length,0);
        bRet = FALSE;
        break;
      }

      MSG_HIGH("GNSS MSGR: Length of LPP Message = %d",dsm_packet_length,0,0);
      dsm_pull_bytes = dsm_pullup(&lpp_dsm_ptr, (void *)pkt_buf, dsm_packet_length);
      if (dsm_pull_bytes != dsm_packet_length || lpp_dsm_ptr != NULL)
      {
        // last byte is pulled, DSM will be returned to free queue
        MSG_ERROR("GNSS MSGR: LPP DSM 0x%x only %d bytes pulled (length=%d)",
          lpp_dsm_ptr, dsm_pull_bytes, dsm_packet_length);    
        bRet = FALSE;
        break;
      }
      else
      {
        *pkt_size = (uint32)dsm_pull_bytes; // save LPP payload length
      }

      nasid_bytes = dsm_length_packet(nasid_dsm_ptr); 
      if (nasid_bytes != sizeof(uint32))  // NAS should send 4 byte NAS ID (RRC Correlation ID)
      {
        MSG_ERROR("GNSS MSGR: received wrong size %d for NAS ID for LPP",nasid_bytes,0,0);
        bRet = FALSE; 
        break;
      }

      dsm_pull_bytes = dsm_pullup(&nasid_dsm_ptr, (void *)nas_id, nasid_bytes);
      if (dsm_pull_bytes != nasid_bytes || nasid_dsm_ptr != NULL)
      {
        // last byte is pulled, DSM will be returned to free queue
        MSG_ERROR("GNSS MSGR: NASID DSM 0x%x only %d bytes pulled (length=%d)",
          nasid_dsm_ptr, dsm_pull_bytes, nasid_bytes);    
        bRet = FALSE;
        break;
      }
      else
      {
        *nas_id = GNSS_MSGR_NTOHL(*nas_id); // NAS ID is in network/big endian order
      }

      MSG_LOW("GNSS MSGR: received NAS ID %d (size %d)", *nas_id, nasid_bytes,0);
    }while(0);

    if (bRet == FALSE)
    {
      dsm_free_packet(&lpp_dsm_ptr);  // can take NULL pointer, free DSM in case of error 
      dsm_free_packet(&nasid_dsm_ptr);
    }
  }
  else if (msg_ptr->msg.hdr.id == NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF)
  { 
    MSG_HIGH("GNSS MSGR: received NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF msg TrasID=0x%x",
             msg_ptr->msg.nas_ul_cnf.trans_id, 0, 0);
  }
  else if (msg_ptr->msg.hdr.id == NAS_EMM_GENERIC_NAS_TRANSPORT_FAILURE_IND)
  {
    MSG_HIGH("GNSS MSGR: received NAS_EMM_GENERIC_NAS_TRANSPORT_FAILURE_IND msg TrasID=0x%x Failure=%d",
             msg_ptr->msg.nas_failure_ind.trans_id, (uint32)msg_ptr->msg.nas_failure_ind.cause_type, 0);

    // send TM_CORE_MSG_ID_NAS_FAILURE_EVENT to LPP CP module
    (void)gnss_msgr_send_nas_failure_event((uint32)msg_ptr->msg.nas_failure_ind.cause_type);
  }
  else
  {  
    MSG_ERROR("GNSS MSGR: received unknown message from NAS module, discarding it",0,0,0);
    bRet = FALSE;
  }
  
  if (msg_ptr != NULL)
  {
    // return item to free queue
    q_put( &gnss_msgr_rcv.free_q, &msg_ptr->msg_hdr.qlink );
  }

  return bRet;

#else /* FEATURE_LTE */

  MSG_HIGH("GNSS MSGR: LTE not enabled, can't receive NAS MSGR messages",0,0,0);

  return FALSE;

#endif /* FEATURE_LTE */

}


#else /* FEATURE_SMART_GNSS_MSG_ROUTER */

/* Below are off-target testing code for SMART */
extern boolean smart_lpp_cp_receive_nas_msg(uint8 *pkt_buf, uint32 buf_size, uint32 *pkt_size, uint32 *nas_id, uint8 *inst_id);

/* stub function for SMART only, to read canned date from file or from mini LPP server */
boolean gnss_msgr_receive_nas_msg(
  uint8 *pkt_buf, 
  uint32 buf_size, 
  uint32 *pkt_size,
  uint32 *nas_id,
  uint8 *inst_id
)
{
  return smart_lpp_cp_receive_nas_msg(pkt_buf, buf_size, pkt_size, nas_id, inst_id);
}

#endif /* FEATURE_SMART_GNSS_MSG_ROUTER */

