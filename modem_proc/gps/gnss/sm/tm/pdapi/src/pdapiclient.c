/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Position Determination Session Manager Client Module


GENERAL DESCRIPTION
  This module contains the Position Determination Client Interface Block

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/pdapi/src/pdapiclient.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/12   ss      Added support to send error in PA callback
02/01/13   mj      Modify pdsm_set_ready_flag to add IPC calls when TM ready
06/11/12   rk      Added new 'pdsm_client_pa_reg_ex' function for clients to register for 
                   all PA events. Also enhanced 'pdsm_client_pa_event_ntfy' to notify the
                   clients used extended function to register in the first place.
01/24/10   rb      add critical section around access to pdapi client map
10/25/09   gk      Sensors client
07/30/09   gk      ODP 2.0 support
01/02/09   atien   Support XTRA-T
07/28/08   gk      Fix klocwork errors
11/13/07   gk      fix link list corruption
08/01/07   rw      Add support for XTRA feature
07/20/07   gk      Notify verify send default response only if no client is registered, moved pdapi_init here
04/04/07   gk      Added some missing cmd call back fucntiosn
02/14/07   gk      lint
12/06/06   gk      store the LCS client id when lcs_reg is called.
12/04/06   gk      Modified LCS client response function
11/30/06   gk      Updated BETWEEN macro to PDSM_BETWEEN to avoid a conflict
11/16/06   gk      Updated LCS response
11/08/06   gk      PDAPI for Converged GPS

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"    /* Definition for basic types and macros */

#include "pdapi.h"        /* External interface to pd.c */
#include "pdapiclient.h"  /* Interface to this file */
#include "pdapidbg.h"     /* Interface to PDSM debug services */
#include "err.h"          /* Interface to error log services */
#include "msg.h"          /* Interface to F3 messages services */
#include "queue.h"        /* Interface to queue services */
#include "tm_api.h"       /* interface to tm */
#include "aries_os_api.h"

#ifdef FEATURE_TM_SECURITY
#include "tm_api.h"
#endif /* FEATURE_TM_SECURITY */


/* unconditional acceptance of SUPL_INIT by pdsm */
#define PDSM_SM_NV_GNSS_OEM_FEATURE_MASK_4_SUPL_NI_NOTIF_LOOPBK  0x00000008  
#define PDSM_SM_NV_GNSS_OEM_FEATURE_MASK_5_CP_SS_NOTIF_LOOPBK    0x00000010  

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
                             FREE PDSMCLIENT QUEUE
---------------------------------------------------------------------------*/

/* Static pool of Clients
*/
static pdsm_client_s_type  pdsmclient_arr[ PDSMCLIENT_POOL_SIZE ];

/* Queue to manage free pdsmclient objects. Even though the clients types are
** static it is better to use Free Q's for Flexibility. It allows us to statically
** less number of buffers than the client types supported.
*/
static q_type            pdsmclient_free_q;

static  pdsm_client_id_type     ni_client_id;

/*---------------------------------------------------------------------------
                             CLIENT ID TO PTR MAP LIST
---------------------------------------------------------------------------*/

/* This maptable maps client types and thier handles. The handles are returned to
** the clients when pdsm_client_init( ) is called. The table also stores the allocated
** client object pointer. In case pdsm_client_init( ) gets called more than once for
** the same client type, the buffer pointed by this pointer is re-assigned.
*/

typedef struct pdsm_client_map_s {

  pdsm_client_type_e_type client_type;
      /* Client Type */

  pdsm_client_id_type     client_id;
  /* Client Id (Handle for Client's) */

  pdsm_client_s_type      *client_ptr;
  /* Pointer to client object */

  pdsm_pd_info_s_type     *info_ptr;
  /* This is the info buffer used for reporting events back to clients. */

} pdsm_client_map_s_type;

/* Allocating Separate info buffer for all the clients. Need to
** check if this is really required. Keep it this way till testing.
*/
static pdsm_pd_info_s_type  info_ptr[ PDSMCLIENT_POOL_SIZE ];

/* The handles can either simple indexes to this table or they can
** be arbitrart number's. For simplicity they can be index's to avoid
** searching the maptable for entries.
*/

static pdsm_client_map_s_type pdsmclient_map_arr[ PDSMCLIENT_POOL_SIZE ] =
{
  { PDSM_CLIENT_TYPE_BROWSER,  (pdsm_client_id_type) 9909, NULL, info_ptr+0 },
  { PDSM_CLIENT_TYPE_UI,       (pdsm_client_id_type) 1290, NULL, info_ptr+1 },
  { PDSM_CLIENT_TYPE_PDA,      (pdsm_client_id_type) 3491, NULL, info_ptr+2 },
  { PDSM_CLIENT_TYPE_DM,       (pdsm_client_id_type) 1909, NULL, info_ptr+3 },
  { PDSM_CLIENT_TYPE_NI,       (pdsm_client_id_type) 7001, NULL, info_ptr+4 },
  { PDSM_CLIENT_TYPE_BREW,     (pdsm_client_id_type) 1009, NULL, info_ptr+5 },
  { PDSM_CLIENT_TYPE_JAVA,     (pdsm_client_id_type) 5688, NULL, info_ptr+6 },
  { PDSM_CLIENT_TYPE_TEST1,    (pdsm_client_id_type) 7541, NULL, info_ptr+7 },
  { PDSM_CLIENT_TYPE_TEST2,    (pdsm_client_id_type) 1001, NULL, info_ptr+8 },
  { PDSM_CLIENT_TYPE_OEM,      (pdsm_client_id_type) 9001, NULL, info_ptr+9 },
  { PDSM_CLIENT_TYPE_TEST3,    (pdsm_client_id_type) 2001, NULL, info_ptr+10 },
  { PDSM_CLIENT_TYPE_XTRA,     (pdsm_client_id_type) 8000, NULL, info_ptr+11 },
  { PDSM_CLIENT_TYPE_ATCOP,    (pdsm_client_id_type) 5001, NULL, info_ptr+12 },
  { PDSM_CLIENT_TYPE_XTRA_T,   (pdsm_client_id_type) 8001, NULL, info_ptr+13 },
  { PDSM_CLIENT_TYPE_WIPER,    (pdsm_client_id_type) 6001, NULL, info_ptr+14 },
  { PDSM_CLIENT_TYPE_ODP,      (pdsm_client_id_type) 3333, NULL, info_ptr+15 },
  { PDSM_CLIENT_TYPE_SENSORS,  (pdsm_client_id_type) 7877, NULL, info_ptr+16 },
};


/*---------------------------------------------------------------------------
                                 CLIENT LIST
---------------------------------------------------------------------------*/
/* Client list object (singular). The client list
** object is responsible for maintaining a list of
** PDSM clients and for forwarding PD events to them.
*/
static struct {
  q_type  queue;
      /* Q item to manage the client list */
} client_list;


/* Macro to get a pointer to the client list object.
**
** NOTE! This macro should only be used within the
** client list object scope.
*/
#define CLIENT_LIST_PTR() ( &client_list )


/* Macro to get a pointer the queue element within
** the client list object. NOTE! This macro should
** only be used within the client list object scope.
*/
#define CLIENT_LIST_Q_PTR() ( &(CLIENT_LIST_PTR()->queue) )



/* <EJECT> */
/*===========================================================================
                          FUNCTION PROTOTYPES
===========================================================================*/

static void pdsmclient_list_add( pdsm_client_s_type *client_ptr );
static void pdsmclient_list_del( pdsm_client_s_type *client_ptr );

static os_MutexBlockType z_pdapi_client_mutex;


/* Globals */
static boolean b_pdapi_ready_flag = FALSE;
static uint32  q_pdapi_gnss_oem_feature_mask = 0;

/*---------------------------------------------------------------------------
                                  PDSM CLIENT
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION pdsm_set_ready_flag

DESCRIPTION
This function is used to set a global flag which should be set to TRUE when
PDAPI has been initialized and is ready to accept calls from PDAPI clients.

This function was added because on some targets this was causing crashes if
a client called pdsm_client_init() before TM was PDAPI was ready.

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void pdsm_set_ready_flag(void)
{
  os_IpcMsgType       *ipc_msg_ptr = NULL;
  tm_api_notifier_type    *ipc_data_ptr = NULL;
  boolean ipc_Status = FALSE;
  uint8 errCode = 0;

  /* Set the ready flag */
  b_pdapi_ready_flag = TRUE;
    
  
  ipc_msg_ptr = os_IpcCreate(sizeof(tm_api_notifier_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
    
  if (ipc_msg_ptr)
  {
  ipc_data_ptr = (tm_api_notifier_type*)ipc_msg_ptr->p_Data; 
    
  ipc_data_ptr->pNotifierCb = NULL;
  ipc_data_ptr->clientIndex = 0;
  ipc_data_ptr->broadcast = TRUE;
      
  ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_API_REGISTRATION;
  ipc_Status  = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
  if (ipc_Status == FALSE)
  {
    ipc_Status = os_IpcDelete(ipc_msg_ptr);
    ipc_msg_ptr = NULL;
    return;
  } 
  }
  else
  { /* ipc_msg_ptr == NULL */
    errCode = 1;
  }

  if (errCode > 0)
  {
    MSG_ERROR("pdsm_set_ready_flag failed with error code = %d", errCode, 0, 0);
  }
      
  return;
}

/*===========================================================================

FUNCTION pdsm_get_ready_flag

DESCRIPTION
This function is used to get a global flag which should be set to TRUE when
PDAPI has been initialized and is ready to accept calls from PDAPI clients.

DEPENDENCIES
none

RETURN VALUE
  TRUE: if TM is initialized
  FALSE: if TM is not intialized

SIDE EFFECTS
none

===========================================================================*/
boolean pdsm_get_ready_flag(void)
{
  return b_pdapi_ready_flag;
}



/*===========================================================================

FUNCTION pdsm_set_gnss_oem_feature_mask

DESCRIPTION
This function is used to get a copy of the NV item "OEM feture mask"

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/
void pdsm_set_gnss_oem_feature_mask(uint32 q_gnss_oem_feature_mask)
{
  q_pdapi_gnss_oem_feature_mask = q_gnss_oem_feature_mask;
}

/*===========================================================================

FUNCTION pdsm_client_init

DESCRIPTION
  Initializing a client object.

  This function must be called exactly once on a client object before
  it is being used, in any way, place, or form.
  In case this function is called more than once, then same client data structure
  gets reallocated.

  Note that in order to get notified of PD events a client still needs
  to do the following:
  1. Register its callback functions and specific events with each functional group
  2. Activate its client object with pd_client_act()

DEPENDENCIES
  none

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/

pdsm_client_id_type pdsm_client_init(
    pdsm_client_type_e_type           client_type
            /* Client type */
)
{
  uint8 i;
  pdsm_client_s_type  *pd_client_ptr;
  pdsm_client_id_type pd_client;
  boolean             re_init = FALSE;

  /*----------------------------------------------------------------------*/

  PDSM_FUNC_START( "pdsm_client_init()", 0,0,0 );

  /*
  ** Ascertain if a client object is already allocated for it. If yes,
  ** use the previously allocated object. Otherwise, allocate a new one.
  */

  pd_client_ptr     = NULL;
  pd_client         = -1;                        /* Initialize it to invalid */

  /* this global flag is set by TM when it is done initializing */
  if (b_pdapi_ready_flag == FALSE)
  {
    PDSM_FUNC_START("PDAPI is not initialized yet",0,0,0);
    return PDSM_CLIENT_TYPE_NONE;
  }

  os_MutexLock(&z_pdapi_client_mutex);

  for (i = 0; i < ARR_SIZE(pdsmclient_map_arr); i++)
  {
    if ( pdsmclient_map_arr[i].client_type == client_type )
    {
      if ( pdsmclient_map_arr[i].client_ptr == NULL )
      {
         pdsmclient_map_arr[i].client_ptr =
              (pdsm_client_s_type *) q_get(&pdsmclient_free_q);
      }
      else
      {
        /* This is the case of client_init() getting called
        ** multiple times.
        */
        re_init = TRUE;
      }

      pd_client     = pdsmclient_map_arr[i].client_id;
      pd_client_ptr = pdsmclient_map_arr[i].client_ptr;
      break;

    } /* End if client types match */

  } /* End for all possible client entries */

  if(pd_client_ptr == NULL)
  {
    /* Return error */
    pd_client = (pdsm_client_id_type)-1;
  }
  else
  {
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Mark this object as initialized. */
    pd_client_ptr->init_mask = (pdsm_pd_event_type)PDSM_INIT_MASK;


    /* Initially client is inactive. */
    if ( (pd_client_ptr->is_active) && re_init )
    {
      /* If this is the case of multiple client_init() and without
      ** client having called client_deact() we should delete the active
      ** client otherwise the link list will be corrupted.
      */
      pdsmclient_list_del( pd_client_ptr );
    }

    pd_client_ptr->is_active = FALSE;

    /* Initialize the link element*/
    (void) q_link( pd_client_ptr, &pd_client_ptr->link );

    /* Set client ID to the address of this client object */
    pd_client_ptr->client          = pd_client_ptr;

    pd_client_ptr->client_id       = pd_client;

    /* Initialize Event masks. */
    pd_client_ptr->pd_event_mask   = 0;

    /* Initially set all callback function pointers
    ** to point to NULL
    */
    pd_client_ptr->pd_event_func   = NULL;

    pd_client_ptr->pd_cmd_err_func = NULL;

    pd_client_ptr->lcs_event_mask = 0;

    pd_client_ptr->lcs_event_func = NULL;

    pd_client_ptr->lcs_cmd_err_func = NULL;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return pd_client;

} /* End pdsm_client_init( ) */


/*===========================================================================

FUNCTION pdsm_client_release

DESCRIPTION
  Release a client object.

  This function should be called if a client doesn't need PDSM anymore.
  This call will release the client object. That object can be reused for
  another client. If the client needs to use PDSM again, it should call
  pdsm_client_init function to establish connection.

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_release(

 pdsm_client_id_type               client_id
        /* Client ID pointer*/
)
{
  uint8 i;
  pdsm_client_s_type        *pd_client_ptr;
  pdsm_client_status_e_type status;

  /*---------------------------------------------------------------------*/

  PDSM_FUNC_START( "pdsm_client_release()", 0,0,0 );

  status = PDSM_CLIENT_ERR_CLIENT_ID;

  /*
  ** Acertain if the client object is already released. If yes, return
  ** failure. Otherwise, put it on the queue.
  */

  os_MutexLock(&z_pdapi_client_mutex);

  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( pdsmclient_map_arr[i].client_id == client_id )
    {
      pd_client_ptr = pdsmclient_map_arr[i].client_ptr;

      if (pd_client_ptr == NULL)
      {
        status =  PDSM_CLIENT_ERR_CLIENT_ID;
        break;
      }
      else if ( pd_client_ptr->is_active == TRUE )
      {
        /* Client is active, need to deactivate before releasing */
        status = pdsm_client_deact( client_id );
        if ( status != PDSM_CLIENT_OK )
        {
          break;
        }
      }
      /* Everything is fine, go ahead and release the client.
      */
      q_put( &pdsmclient_free_q, &pdsmclient_map_arr[i].client_ptr->link );
      pdsmclient_map_arr[i].client_ptr = NULL;

      status = PDSM_CLIENT_OK;
      break;
    }
  } /* End for all the client entries */

  os_MutexUnlock(&z_pdapi_client_mutex);
  return  status;

} /* End pdsm_client_release( ) */


/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_get_client_ptr

DESCRIPTION
  Returns a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object

SIDE EFFECTS
  None

===========================================================================*/
pdsm_client_s_type* pdsmclient_get_client_ptr(

  pdsm_client_id_type client_id
    /* Client Id */
)
{
  uint8 i;
  pdsm_client_s_type* client_type = NULL;
  /*---------------------------------------------------------------------*/

  os_MutexLock(&z_pdapi_client_mutex);

  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_id == pdsmclient_map_arr[i].client_id )
    {
      client_type = pdsmclient_map_arr[i].client_ptr;
      break;
    }
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_type;

} /* End pdsmclient_get_client_ptr() */

/*===========================================================================

FUNCTION pdsmclient_get_client_ptr

DESCRIPTION
  checks if it is a valid client handle. The client should have already called
  pd_client_init() with the client type to obtain a valid client id.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object

SIDE EFFECTS
  None

===========================================================================*/

boolean pdsmclient_valid_client_id(

  pdsm_client_id_type client_id
    /* Client Id */
)
{
  boolean ret_val = FALSE;
  uint8 i;

  os_MutexLock(&z_pdapi_client_mutex);

  /* Need to go through all the map entries and check if this
  ** is a valid client id.
  */
  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_id == pdsmclient_map_arr[i].client_id )
    {
      if ( pdsmclient_map_arr[i].client_ptr != NULL )
      {
        ret_val = TRUE;
        break;
      } /* End if client ptr is initialized */
    } /* End if client id's match */
  } /* For all the possible clients' */

  os_MutexUnlock(&z_pdapi_client_mutex);

  return ret_val;

} /* End pdsmclient_valid_client_id() */

/*===========================================================================

FUNCTION pdsmclient_get_client_type

DESCRIPTION
  checks if it is a valid client handle. If the client is valid, the corsp
  client type will be returned to caller

DEPENDENCIES
  None

RETURN VALUE
  Client type for the client ptr

SIDE EFFECTS
  None

===========================================================================*/
pdsm_client_type_e_type pdsmclient_get_client_type_map(
  pdsm_client_id_type client_id
    /* Client Id */
)
{
  uint8 i;
  pdsm_client_type_e_type client_type = PDSM_CLIENT_TYPE_NONE;

  os_MutexLock(&z_pdapi_client_mutex);

  /* Need to go through all the map entries and check if this
  ** is a valid client id.
  */
  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_id == pdsmclient_map_arr[i].client_id )
    {
      client_type = pdsmclient_map_arr[i].client_type;
      break;
    } /* End if client id's match */
  } /* For all the possible clients' */

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_type;
} /* End pdsmclient_get_client_type() */
/*===========================================================================

FUNCTION pdsmclient_get_client_type

DESCRIPTION
  This function provides Client ID based on client type

DEPENDENCIES
  None

RETURN VALUE
  Client type for the client ptr

SIDE EFFECTS
  None

===========================================================================*/
pdsm_client_id_type pdsmclient_get_client_id_map(
  pdsm_client_type_e_type client_type
    /* Client Id */
)
{
  uint8 i;
  pdsm_client_id_type client_id = (pdsm_client_id_type)PDSM_CLIENT_TYPE_NONE;

  os_MutexLock(&z_pdapi_client_mutex);

  /* Need to go through all the map entries and check if this
  ** is a valid client id.
  */
  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_type == pdsmclient_map_arr[i].client_type )
    {
      client_id = pdsmclient_map_arr[i].client_id;
      break;
    } /* End if client id's match */
  } /* For all the possible clients' */

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_id;
} /* End pdsmclient_get_client_id_map() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_get_client_id

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
pdsm_client_id_type pdsmclient_get_client_id(

  pdsm_client_s_type *client_ptr
    /* Client object pointer */
)
{
  uint8 i;
  pdsm_client_id_type client_id = 0;

  os_MutexLock(&z_pdapi_client_mutex);

  /*---------------------------------------------------------------------*/

  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_ptr == pdsmclient_map_arr[i].client_ptr )
    {
      client_id = pdsmclient_map_arr[i].client_id;
      break;
    }
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_id;

} /* End pdsmclient_get_client_id() */


/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_act

DESCRIPTION
  Activate the client object, so that the client will be notified
  of Session Manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Add the client object to the client list.

===========================================================================*/
pdsm_client_status_e_type pdsm_client_act(

  pdsm_client_id_type          client_id
    /* Client ID */
)
{

  pdsm_client_s_type          *client_ptr;


#ifdef FEATURE_TM_SECURITY
  os_IpcMsgType     *ipc_msg_ptr;
#endif /* FEATURE_TM_SECURITY */

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  PDSM_FUNC_START("pdsm_client_act(). client_id=%d", client_id, 0, 0); 


  /* If client is NOT already active, add the client to the client list
  ** and indicate that client is now active,
  */
  if( !client_ptr->is_active )
  {
    pdsmclient_list_add( client_ptr );
    client_ptr->is_active = TRUE;

#ifdef FEATURE_TM_SECURITY
    /* Secuirty Layer: Set subsig to PDSM to update the first random number
    ** if the client has registered for security events.
    */
    if( (client_ptr->pd_event_func != NULL)
      &&(client_ptr->pd_event_mask & PDSM_PD_EVENT_SECURITY))
    {
      ipc_msg_ptr = os_IpcCreate(16, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

      if (ipc_msg_ptr != NULL)
      {
        ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_SEC_MSG_ID_CLNT_ACTIVATED;
        ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
        ipc_msg_ptr->q_Size           = 16;

        if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
        {
          MSG_ERROR("Failed to send IPC mesage",0,0,0);
          (void)os_IpcDelete(ipc_msg_ptr);
        }
      } /* End ipc_msg_ptr */
      else
      {
        MSG_ERROR("Failed to create IPC mesage",0,0,0);
      } /* End ipc_msg_ptr */

    } /* end if */
#endif /* FEATURE_TM_SECURITY */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, we are trying to activate a client that is already active.
  ** This is not a problem except that it might indicate client confusion.
  */
  else
  {
    PDSM_MSG_HIGH( "Client already active! client_id=%p",client_ptr,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return PDSM_CLIENT_OK;

} /* End pdsm_client_act( ) */


/* <EJECT> */
/*===========================================================================

FUNCTION pd_client_deact

DESCRIPTION
  Deactivate a client object, so that the client will NOT be notified
  of Session Manager events (for which it is registered). Before releasing a
  client object the client needs to be deactivated using this function.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Remove the client object from the client list.

===========================================================================*/
pdsm_client_status_e_type pdsm_client_deact(

    pdsm_client_id_type          client_id
    /* Client ID */
)
{

  pdsm_client_s_type          *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  PDSM_FUNC_START( "pdsm_client_deact() client_id=%p",
                client_id,0,0 );

  /* If client is active, delete the client from the client list
  ** and indicate that client is NOT active,
  */
  if( client_ptr->is_active )
  {
    pdsmclient_list_del( client_ptr );
    client_ptr->is_active = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, we are trying to deactivate a client that is already inactive.
  */
  else
  {
    PDSM_MSG_HIGH( "Client already inactive! client_id=%p",client_ptr,0,0 );
  }

  return PDSM_CLIENT_OK;

} /* End pdsm_client_deact() */



/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_event_ntfy

DESCRIPTION
  Notify a client of a specified PD event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsm_client_event_ntfy(

  const pdsm_client_s_type    *client_ptr,
      /* pointer to a client struct */

  pdsm_pd_event_type          pd_event,
      /* notify client of this PD event */

  const pdsm_pd_info_s_type   *pd_info_ptr
      /* pointer to pdsm state struct */
)
{
  if (client_ptr == NULL || pd_info_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK)
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If callback function exists and corresponding
  ** event bit is on, notify the client of the PD event.
  */

  if( (client_ptr->pd_event_func != NULL) &&
      (client_ptr->pd_event_mask & pd_event) )
  {
    client_ptr->pd_event_func( client_ptr->pd_data_ptr, pd_event, pd_info_ptr );
  }

} /* End pdsm_client_event_ntfy() */

#ifdef FEATURE_VECTORONE
/*===========================================================================

FUNCTION pdsm_client_or_event_ntfy

DESCRIPTION
  Notify a client of a specified OR event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsm_client_or_event_ntfy(

  const pdsm_client_s_type    *client_ptr,
      /* pointer to a client struct */

  pdsm_or_event_type          or_event,
      /* notify client of this OR event */

  const pdsm_or_info_s_type   *or_info_ptr
      /* pointer to pdsm state struct */
)
{
  if (client_ptr == NULL || or_info_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }

  PDSM_MSG_HIGH("pdsm_client_or_event_ntfy",0,0,0);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If callback function exists and corresponding
  ** event bit is on, notify the client of the PD event.
  */

  if( (client_ptr->or_event_func != NULL) &&
      (client_ptr->or_event_mask & or_event) )
  {
    PDSM_MSG_HIGH("calling or_event_func, event=%d",or_event,0,0);
    client_ptr->or_event_func( client_ptr->or_data_ptr, or_event, or_info_ptr );
  }

} /* End pdsm_client_or_event_ntfy() */
#endif

/*===========================================================================

FUNCTION pdsm_client_pa_event_ntfy

DESCRIPTION
  Notify a client of a specified PA event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsm_client_pa_event_ntfy(

  const pdsm_client_s_type    *client_ptr,
      /* pointer to a client struct */

  pdsm_pa_event_type          pa_event,
      /* notify client of this PD event */

  const pdsm_pa_info_s_type   *pa_info_ptr
      /* pointer to pdsm state struct */
)
{
  if (client_ptr == NULL || pa_info_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If callback function exists and corresponding
  ** event bit is on, notify the client of the PD event.
  */

  if( (client_ptr->pa_event_func != NULL) &&
      (client_ptr->pa_event_mask & pa_event) )
  {
    client_ptr->pa_event_func( client_ptr->pa_data_ptr, pa_event, pa_info_ptr );
  }

  // Notify client registered with extended all PA event interface
  if ( client_ptr->pa_event_func_ex != NULL )
  {
    client_ptr->pa_event_func_ex( client_ptr->pa_data_ptr, pa_info_ptr->pa_event, pa_info_ptr, PDSM_PA_CMD_ERR_NOERR );
  }

} /* End pdsm_client_pa_event_ntfy() */

/*===========================================================================

FUNCTION pdsm_client_pa_event_ntfy_ex

DESCRIPTION
  Notify a client of a specified PA event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsm_client_pa_event_ntfy_ex(

  const pdsm_client_s_type    *client_ptr,
      /* pointer to a client struct */

  pdsm_pa_event_type          pa_event,
      /* notify client of this PD event */

  const pdsm_pa_info_s_type   *pa_info_ptr,
  /* pointer to pdsm state struct */

  pdsm_pa_cmd_err_e_type   pa_cmd_err  
      /*CMD Error*/
)
{
  if (client_ptr == NULL || pa_info_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }


  // Notify client registered with extended all PA event interface
  if ( client_ptr->pa_event_func_ex != NULL )
  {
    client_ptr->pa_event_func_ex( client_ptr->pa_data_ptr, pa_info_ptr->pa_event, pa_info_ptr, pa_cmd_err );
  }

} /* End pdsm_client_pa_event_ntfy() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_ext_status_event_ntfy

DESCRIPTION
  Notify a client of a specified extended status event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsm_client_ext_status_event_ntfy(

  const pdsm_client_s_type    *client_ptr,
      /* pointer to a client struct */

  pdsm_ext_status_event_type          ext_status_event,
      /* notify client of this PD event */

  const pdsm_ext_status_info_s_type   *ext_status_info_ptr
      /* pointer to pdsm state struct */
)
{
  if (client_ptr == NULL || ext_status_info_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If callback function exists and corresponding
  ** event bit is on, notify the client of the PD event.
  */

  if( (client_ptr->ext_status_event_func != NULL) &&
      (client_ptr->ext_status_event_mask & ext_status_event) )
  {
    client_ptr->ext_status_event_func( client_ptr->ext_status_data_ptr, ext_status_event, ext_status_info_ptr );
  }

} /* End pdsm_client_ext_status_event_ntfy() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_pd_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified PD command error code if the client is
  registered to be notified of OTHER clients PD command errors and the
  client is NOT the one originally sending this PD command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsm_client_pd_cmd_err_ntfy(

  const pdsm_client_s_type         *client_ptr,
      /* pointer to a client struct */

  const pdsm_pd_cmd_s_type         *pd_cmd_ptr,
      /* pointer to pd command struct */

  pdsm_pd_cmd_err_e_type           pd_cmd_err
      /* command error code */
)
{
  if (client_ptr == NULL || pd_cmd_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(pd_cmd_err, PDSM_PD_CMD_ERR_NONE, PDSM_PD_CMD_ERR_MAX) );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != pd_cmd_ptr->client )
  {
    if( client_ptr->pd_cmd_err_func != NULL )
    {
      client_ptr->pd_cmd_err_func( pd_cmd_ptr->client_id,
                                     pd_cmd_ptr->cmd,
                                     pd_cmd_err );

    } /* End if no cmd err func is installed */
  } /* End if client ID's don't match */

} /* End of pdsm_client_cmd_err_ntfy */

#ifdef FEATURE_VECTORONE
/*===========================================================================

FUNCTION pdsm_client_or_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified PD command error code if the client is
  registered to be notified of OTHER clients PD command errors and the
  client is NOT the one originally sending this PD command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsm_client_or_cmd_err_ntfy(

  const pdsm_client_s_type         *client_ptr,
      /* pointer to a client struct */

  const pdsm_or_cmd_s_type         *or_cmd_ptr,
      /* pointer to pd command struct */

  pdsm_or_cmd_err_e_type           or_cmd_err
      /* command error code */
)
{
  if (client_ptr == NULL || or_cmd_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(or_cmd_err, PDSM_OR_CMD_ERR_NONE, PDSM_OR_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != or_cmd_ptr->client )
  {
    if( client_ptr->or_cmd_err_func != NULL )
    {
      PDSM_MSG_HIGH("cmd_err_ntfy %d", or_cmd_err,0,0);
      client_ptr->or_cmd_err_func( or_cmd_ptr->client_id,
                                     or_cmd_ptr->cmd,
                                     or_cmd_err );

    } /* End if no cmd err func is installed */
  } /* End if client ID's don't match */
} /* End of pdsm_client_or_cmd_err_ntfy */
#endif

/*===========================================================================

FUNCTION pdsm_client_pa_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified Parameter command error code.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsm_client_pa_cmd_err_ntfy(

  const pdsm_client_s_type         *client_ptr,
      /* pointer to a client struct */

  const pdsm_pa_cmd_s_type         *pa_cmd_ptr,
      /* pointer to pa command struct */

  pdsm_pa_cmd_err_e_type           pa_cmd_err
      /* command error code */
)
{
  if (client_ptr == NULL || pa_cmd_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(pa_cmd_err, PDSM_PA_CMD_ERR_NONE, PDSM_PA_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != pa_cmd_ptr->client )
  {
    if( client_ptr->pa_cmd_err_func != NULL )
    {
      client_ptr->pa_cmd_err_func( pa_cmd_ptr->client_id,
                                     pa_cmd_ptr->cmd,
                                     pa_cmd_err );

    } /* End if no cmd err func is installed */
  } /* End if client ID's don't match */
} /* End of pdsm_client_cmd_err_ntfy */

/*===========================================================================

FUNCTION pdsm_client_lcs_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified PD command error code if the client is
  registered to be notified of OTHER clients PD command errors and the
  client is NOT the one originally sending this PD command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsm_client_lcs_cmd_err_ntfy(

  const pdsm_client_s_type         *client_ptr,
      /* pointer to a client struct */

  const pdsm_lcs_cmd_s_type         *lcs_cmd_ptr,
      /* pointer to pd command struct */

  pdsm_lcs_cmd_err_e_type           lcs_cmd_err
      /* command error code */
)
{
  if (client_ptr == NULL || lcs_cmd_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(lcs_cmd_err, PDSM_LCS_CMD_ERR_NONE, PDSM_LCS_CMD_ERR_MAX) );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != lcs_cmd_ptr->client )
  {
    if( client_ptr->lcs_cmd_err_func != NULL )
    {
      client_ptr->lcs_cmd_err_func( lcs_cmd_ptr->client_id,
                                     lcs_cmd_ptr->cmd,
                                     lcs_cmd_err );

    } /* End if no cmd err func is installed */
  } /* End if client ID's don't match */
} /* End of pdsm_client_cmd_err_ntfy */


/*===========================================================================

FUNCTION pdsm_client_ext_status_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified ext_statuscommand error code if the client is
  registered to be notified of OTHER clients PD command errors and the
  client is NOT the one originally sending this PD command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsm_client_ext_status_cmd_err_ntfy(

  const pdsm_client_s_type         *client_ptr,
      /* pointer to a client struct */

  const pdsm_ext_pos_cmd_s_type         *ext_status_cmd_ptr,
      /* pointer to pd command struct */

  pdsm_ext_pos_cmd_err_e_type           ext_status_cmd_err
      /* command error code */
)
{
  if (client_ptr == NULL || ext_status_cmd_ptr == NULL || client_ptr->init_mask != PDSM_INIT_MASK )
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(ext_status_cmd_err, PDSM_EXT_POS_CMD_ERR_NONE, PDSM_EXT_POS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != ext_status_cmd_ptr->client )
  {
    if( client_ptr->ext_status_cmd_err_func != NULL )
    {
      client_ptr->ext_status_cmd_err_func( ext_status_cmd_ptr->client_id,
                                     ext_status_cmd_ptr->cmd,
                                     ext_status_cmd_err );

    } /* End if no cmd err func is installed */
  } /* End if client ID's don't match */
} /* End of pdsm_client_cmd_err_ntfy */

/*---------------------------------------------------------------------------
                                CLIENT LIST
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION pdsmclient_list_init

DESCRIPTION
  Initializes the client list.

  This function must be called before using the client list is used in any
  way, place or form.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pdsmclient_list_init( void )
{

  PDSM_FUNC_START( "pdsmclient_list_init()",0,0,0 );

  /* Initialize client queue.
  */
  (void) q_init( CLIENT_LIST_Q_PTR() );
}

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_first

DESCRIPTION
  Return a pointer to first client in the client list.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  Return a pointer to first client in the client list.
  If client list is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static pdsm_client_s_type *pdsmclient_list_first( void )
{
  PDSM_FUNC_START( "pdsmclient_list_first()",0,0,0 );

  return( (pdsm_client_s_type*) q_check(CLIENT_LIST_Q_PTR()) );
}



/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_next

DESCRIPTION
  Return a pointer to the next client in the client list.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().
  A lock of z_pdapi_client_mutex is needed before calling this function.

RETURN VALUE
  A pointer to the next client in the client list.
  If the end of the client list is reached, a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static pdsm_client_s_type *pdsmclient_list_next( pdsm_client_s_type *client_ptr )
{
  q_link_type  *link_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (client_ptr == NULL)
  {
    PDSM_MSG_HIGH("Null Pointers", 0,0,0);
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get pointer to next client
  ** (i.e. the link with in the client).
  */
  link_ptr = (q_link_type*) q_next( CLIENT_LIST_Q_PTR(), &client_ptr->link );

  /* If this is not the end of the client list, return a pointer
  ** to the client object containing the link element.
  */
/* Else, we have reached the end of the client list -
  ** return NULL.
  */
  return (pdsm_client_s_type *) link_ptr;

} /* End pdsmclient_list_next() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_add

DESCRIPTION
  Add a client to the client list.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsmclient_list_add( pdsm_client_s_type *client_ptr )
{

  PDSM_FUNC_START( "pdsmclient_list_add()",0,0,0 );

  /* Make sure this client is NOT already in the client list.
  */
  if(client_ptr == NULL || client_ptr->link.next_ptr != NULL )
  {
    PDSM_MSG_HIGH("Non-Null pointer", 0,0,0);
    return;
  }

#ifndef FEATURE_Q_SINGLE_LINK
  if( client_ptr->link.prev_ptr != NULL )
  {
    PDSM_MSG_HIGH("Non-Null pointer", 0,0,0);
    return;
  }
#endif

#ifndef __GRIFFON__
#ifndef FEATURE_Q_NO_SELF_QPTR
  if( client_ptr->link.q_ptr != NULL )
  {
    PDSM_MSG_HIGH(Non-Null Pointer,0,0,0);
    return;
  }
#endif
#endif  // __GRIFFON__

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);
  q_put( CLIENT_LIST_Q_PTR(), &client_ptr->link );
  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_add() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_del

DESCRIPTION
  Delete a client from the client list.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pdsmclient_list_del( pdsm_client_s_type *client_ptr )
{

  if( client_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /* Make sure this client is indeed in the client list.
  */
  if( client_ptr->link.next_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

#ifndef FEATURE_Q_SINGLE_LINK
  if( client_ptr->link.prev_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
#endif


#ifndef __GRIFFON__
#ifndef FEATURE_Q_NO_SELF_QPTR
  if( client_ptr->link.q_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
#endif
#endif  // __GRIFFON__
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Delete the client from the client list.
  */
  os_MutexLock(&z_pdapi_client_mutex);
//
//#ifdef FEATURE_Q_NO_SELF_QPTR
//  q_delete(CLIENT_LIST_Q_PTR(), (q_link_type *) &client_ptr->link );
//#else
//  q_delete( (q_link_type *) &client_ptr->link );
//
//#endif
//

#if defined(__GRIFFON__) || defined(FEATURE_Q_NO_SELF_QPTR)
  q_delete(CLIENT_LIST_Q_PTR(), (q_link_type *) &client_ptr->link );
#else
  q_delete( (q_link_type *) &client_ptr->link );

#endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the client's link item to make sure its next and prev
  ** pointers are no longer pointing at other clients.
  */
  (void) q_link( client_ptr, &client_ptr->link );

  os_MutexUnlock(&z_pdapi_client_mutex);
} /* End pdsmclient_list_del() */


/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_pd_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD event. Only the
  registered clients other than those which were active are informed about
  this event. The active client are passed through list_ptr.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void pdsmclient_reg_list_pd_event_ntfy(

  pdsm_pd_event_type           pd_event,
      /* notify clients of this PD event */

  const pdsm_pd_info_s_type    *pd_info_ptr,
      /* pointer to PD information struct */

  pdsm_client_s_type           **list_ptr
      /* List of clients which are active and would be informed separately */
)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  boolean found = FALSE;
  uint8 i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( pd_event != (pdsm_pd_event_type)PDSM_PD_EVENT_MASK );
  if( pd_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */

  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    /* Compare the registred client with the active client */
    for ( i=0; i < PDSMCLIENT_POOL_CHECK; i++ )
    {
        if ( client_ptr == list_ptr[i] )
        {
            /* This registered client was active and is already informed, so
            ** skip him.
            */
            found = TRUE;
            break;
        }
    } /* End for all the Active Client's */

    if ( found == FALSE )
    {
        /* This is a registered client so inform this client about a pd_event */
        pdsm_client_event_ntfy( client_ptr, pd_event, pd_info_ptr );
    }


    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);
} /* End pdsm_reg_list_event_ntfy() */



/*===========================================================================

FUNCTION pdsmclient_list_pd_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_pd_event_ntfy(

  pdsm_pd_event_type   pd_event,
      /* notify clients of this PD event */

  const pdsm_pd_info_s_type    *pd_info_ptr
      /* pointer to PD information struct */
)
{
  pdsm_client_s_type *client_ptr , *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( pd_event != (pdsm_pd_event_type)PDSM_PD_EVENT_MASK );
  if( pd_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */

  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_event_ntfy( client_ptr, pd_event, pd_info_ptr );

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_event_ntfy() */

/*===========================================================================

FUNCTION pdsmclient_list_pa_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PA event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_pa_event_ntfy(

  pdsm_pa_event_type   pa_event,
      /* notify clients of this PD event */

  const pdsm_pa_info_s_type    *pa_info_ptr
      /* pointer to PD information struct */
)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( pa_event != PDSM_PA_EVENT_MASK );
  if( pa_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_pa_event_ntfy( client_ptr, pa_event, pa_info_ptr );

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_event_ntfy() */

/*===========================================================================

FUNCTION pdsmclient_list_pa_event_ntfy_ex

DESCRIPTION
  Notify all the clients in the client list of a specified PA event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_pa_event_ntfy_ex(

  pdsm_pa_event_type   pa_event,
      /* notify clients of this PD event */

  const pdsm_pa_info_s_type    *pa_info_ptr,
      /* pointer to PD information struct */

  pdsm_pa_cmd_err_e_type   pa_cmd_err
    /*CMD Error*/

)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( pa_event != PDSM_PA_EVENT_MASK );
  if( pa_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_pa_event_ntfy_ex( client_ptr, pa_event, pa_info_ptr,pa_cmd_err );

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_event_ntfy() */


/*===========================================================================

FUNCTION pdsmclient_list_ext_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified ext status event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_ext_status_event_ntfy(

  pdsm_ext_status_event_type   ext_status_event,
      /* notify clients of this PD event */

  const pdsm_ext_status_info_s_type    *ext_status_info_ptr
      /* pointer to PD information struct */
)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( ext_status_event != PDSM_EXT_STATUS_EVENT_MASK );
  if( ext_status_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_ext_status_event_ntfy( client_ptr, ext_status_event, ext_status_info_ptr );
    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);
} /* End pdsmclient_list_event_ntfy() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_pd_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PDSM command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_pd_cmd_err_ntfy(

  pdsm_pd_cmd_err_e_type      pd_cmd_err,
      /* PD command error code */

  const pdsm_pd_cmd_s_type    *pd_cmd_ptr
      /* pointer to PD command struct */

)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pd_cmd_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(pd_cmd_err, PDSM_PD_CMD_ERR_NONE, PDSM_PD_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_pd_cmd_err_ntfy( client_ptr,
                                 pd_cmd_ptr,
                                 pd_cmd_err );

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_pd_cmd_err_ntfy() */

#ifdef FEATURE_VECTORONE

/*===========================================================================

FUNCTION pdsmclient_list_or_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified OR event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_or_event_ntfy(

  pdsm_or_event_type   or_event,
      /* notify clients of this OR event */

  const pdsm_or_info_s_type    *or_info_ptr
      /* pointer to OR information struct */
)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( or_event != PDSM_OR_EVENT_MASK );
  if( or_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_or_event_ntfy( client_ptr, or_event, or_info_ptr );

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_or_list_event_ntfy() */

/*===========================================================================

FUNCTION pdsmclient_reg_list_or_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD event. Only the
  registered clients other than those which were active are informed about
  this event. The active client are passed through list_ptr.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void pdsmclient_reg_list_or_event_ntfy(

  pdsm_or_event_type           or_event,
      /* notify clients of this OR event */

  const pdsm_or_info_s_type    *or_info_ptr,
      /* pointer to OR information struct */

  pdsm_client_s_type           **list_ptr
      /* List of clients which are active and would be informed separately */
)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  boolean found = FALSE;
  uint8 i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PDSM_ASSERT( or_event != PDSM_OR_EVENT_MASK );
  if( or_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    /* Compare the registred client with the active client */
    for ( i=0; i < PDSM_OR_CLIENT_POOL_SIZE; i++ )
    {
        if ( client_ptr == list_ptr[i] )
        {
            /* This registered client was active and is already informed, so
            ** skip him.
            */
            found = TRUE;
            break;
        }
    } /* End for all the Active Client's */

    if ( found == FALSE )
    {
        /* This is a registered client so inform this client about a or_event */
        pdsm_client_or_event_ntfy( client_ptr, or_event, or_info_ptr );
    }

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }/* End for all the registered client list */

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsm_reg_list_event_ntfy() */

/*===========================================================================

FUNCTION pdsmclient_list_or_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PDSM command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_or_cmd_err_ntfy(

  pdsm_or_cmd_err_e_type      or_cmd_err,
      /* PD command error code */

  const pdsm_or_cmd_s_type    *or_cmd_ptr
      /* pointer to PD command struct */

)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( or_cmd_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(or_cmd_err, PDSM_OR_CMD_ERR_NONE, PDSM_OR_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    PDSM_MSG_HIGH("cmd err=%d", or_cmd_err,0,0);
    pdsm_client_or_cmd_err_ntfy( client_ptr,
                                 or_cmd_ptr,
                                 or_cmd_err );
    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_or_cmd_err_ntfy() */
#endif

/*===========================================================================

FUNCTION pdsmclient_list_pd_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PDSM command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_pa_cmd_err_ntfy(

  pdsm_pa_cmd_err_e_type      pa_cmd_err,
      /* PD command error code */

  const pdsm_pa_cmd_s_type    *pa_cmd_ptr
      /* pointer to PD command struct */

)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pa_cmd_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(pa_cmd_err, PDSM_PA_CMD_ERR_NONE, PDSM_PA_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_pa_cmd_err_ntfy( client_ptr,
                                 pa_cmd_ptr,
                                 pa_cmd_err );
    /* Set the current client to the next client in list */
    client_ptr = next_ptr;

  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_pd_cmd_err_ntfy() */

/*===========================================================================

FUNCTION pdsmclient_list_lcs_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PDSM command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_lcs_cmd_err_ntfy(

  pdsm_lcs_cmd_err_e_type      lcs_cmd_err,
      /* PD command error code */

  const pdsm_lcs_cmd_s_type    *lcs_cmd_ptr
      /* pointer to PD command struct */

)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( lcs_cmd_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(lcs_cmd_err, PDSM_LCS_CMD_ERR_NONE, PDSM_LCS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_lcs_cmd_err_ntfy( client_ptr,
                                 lcs_cmd_ptr,
                                 lcs_cmd_err );
    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_lcs_cmd_err_ntfy() */


/*===========================================================================

FUNCTION pdsmclient_list_ext_status_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PDSM command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_ext_status_cmd_err_ntfy(

  pdsm_ext_pos_cmd_err_e_type      ext_status_cmd_err,
      /* PD command error code */

  const pdsm_ext_pos_cmd_s_type    *ext_status_cmd_ptr
      /* pointer to PD command struct */

)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ext_status_cmd_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return;
  }
  PDSM_ASSERT( PDSM_BETWEEN(ext_status_cmd_err, PDSM_EXT_POS_CMD_ERR_NONE, PDSM_EXT_POS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    pdsm_client_ext_status_cmd_err_ntfy( client_ptr,
                                 ext_status_cmd_ptr,
                                 ext_status_cmd_err );
    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

} /* End pdsmclient_list_ext_status_cmd_err_ntfy() */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_pd_reg

DESCRIPTION
  This function allows a client to register the following PD-related
  callbacks and events with PDSM:

   1. Callback function to receive registered PD events
   2. Register and de-register PD events
   3. Callback function to receive other clients' PD-command errors.

   If a client registers a callback function to receive PD event
   notification, it also needs to register one or more PD events it is
   interested in receiving.

  If a client registers a callback function to receive other clients'
  pd-command errors, it will receive notifications on all possible
  pd-command errors caused by other clients.The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' pd-command errors, it can set pd_cmd_err_func = NULL.
  However, if the pd_event_func is set to NULL, all registered PD events
  are also de-registered.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_pd_reg(

  pdsm_client_id_type           client_id,
      /* client id */

  void                          *client_data_ptr,

  pdsm_pd_event_f_type          *pd_event_func,
      /* Pointer to a callback function to notify the client of PD
      ** events */

  pdsm_client_event_reg_e_type  event_reg_type,

  pdsm_pd_event_type            pd_event_mask,
      /* register for this event mask */

  pdsm_pd_cmd_err_f_type        *pd_cmd_err_func
    /* pointer to a callback function to notify the client of PD
    ** command errors of OTHER clients */
)
{
  boolean reg_flag;
  struct pdsm_client_s  *client_ptr;

#ifdef FEATURE_TM_SECURITY
  os_IpcMsgType     *ipc_msg_ptr;
#endif /* FEATURE_TM_SECURITY */

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  if( pd_event_func == NULL )
  {

   /* deregister all of client's PD events */
   client_ptr->pd_event_mask = 0;

   /* update PD event and other clients' command error callback functions */

   client_ptr->pd_event_func = pd_event_func;
   client_ptr->pd_cmd_err_func = pd_cmd_err_func;
   return PDSM_CLIENT_OK;

  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->pd_event_mask) & (~PDSM_PD_EVENT_MASK) )
  {
      return PDSM_CLIENT_ERR_EVENT_MASK;
  }

  switch(event_reg_type)
  {
     case PDSM_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case PDSM_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return PDSM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  PDSM_MSG_HIGH("pdsm_client_pd_reg(). CDPtr %p", client_data_ptr, 0, 0); 

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;

  if (reg_flag == TRUE)
  {
    client_ptr->pd_event_mask = pd_event_mask;

    /* update PD event and other clients' command error callback functions */
    client_ptr->pd_event_func = pd_event_func;
    client_ptr->pd_cmd_err_func = pd_cmd_err_func;

#ifdef FEATURE_TM_SECURITY
    /* Security Layer: If client is already active, set subsig to PDSM
    ** to update the first random number otherwise wait until the client
    ** is active before sending the first randm number challenge
    */
    if( (client_ptr->is_active)
      &&(client_ptr->pd_event_mask & PDSM_PD_EVENT_SECURITY))
    {
      ipc_msg_ptr = os_IpcCreate(16, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

      if (ipc_msg_ptr != NULL)
      {
        ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_SEC_MSG_ID_CLNT_ACTIVATED;
        ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
        ipc_msg_ptr->q_Size           = 16;

        if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
        {
          MSG_ERROR("Failed to send IPC mesage",0,0,0);
          (void)os_IpcDelete(ipc_msg_ptr);
        }
      } /* End ipc_msg_ptr */
      else
      {
        MSG_ERROR("Failed to create IPC mesage",0,0,0);
      } /* End ipc_msg_ptr */
    }
#endif /* FEATURE_TM_SECURITY */

  }
  else
  {
    /* the client wants to de-register, just clear the event bit nask.
    */
    client_ptr->pd_event_mask &= ~pd_event_mask;
  }

  return PDSM_CLIENT_OK;

}/* End of pdsm_client_pd_reg( ) */

#ifdef FEATURE_VECTORONE
/*===========================================================================

FUNCTION pdsm_client_or_reg

DESCRIPTION
  This function allows a client to register the following OR-related
  callbacks and events with PDSM:

   1. Callback function to receive registered OR events
   2. Register and de-register OR events
   3. Callback function to receive other clients' OR-command errors.

   If a client registers a callback function to receive OR event
   notification, it also needs to register one or more OR events it is
   interested in receiving.

  If a client registers a callback function to receive other clients'
  OR-command errors, it will receive notifications on all possible
  OR-command errors caused by other clients.The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' OR-command errors, it can set pd_cmd_err_func = NULL.
  However, if the pd_event_func is set to NULL, all registered OR events
  are also de-registered.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_or_reg(

  pdsm_client_id_type           client_id,
      /* client id */
  void                          *client_data_ptr,

  pdsm_or_event_f_type          *or_event_func,
      /* Pointer to a callback function to notify the client of PD
      ** events */

  pdsm_client_event_reg_e_type  event_reg_type,

  pdsm_or_event_type            or_event_mask,
      /* register for this event mask */

  pdsm_or_cmd_err_f_type        *or_cmd_err_func
    /* pointer to a callback function to notify the client of PD
    ** command errors of OTHER clients */
)
{
  boolean reg_flag;
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  if( or_event_func == NULL )
  {

   /* deregister all of client's OR events */
   client_ptr->or_event_mask = NULL;

   /* update OR event and other clients' command error callback functions */

   client_ptr->or_event_func = or_event_func;
   client_ptr->or_cmd_err_func = or_cmd_err_func;

   return PDSM_CLIENT_OK;

  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->or_event_mask) & (~PDSM_OR_EVENT_MASK) )
  {
      return PDSM_CLIENT_ERR_EVENT_MASK;
  }

  switch(event_reg_type)
  {
     case PDSM_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case PDSM_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return PDSM_CLIENT_ERR_EVENT_REG_TYPE;
             break;
  }
  /* use the latest client data pointer.
  */
  client_ptr->or_data_ptr = client_data_ptr;

  if (reg_flag == TRUE)
  {
    client_ptr->or_event_mask = or_event_mask;

    /* update OR event and other clients' command error callback functions */
    client_ptr->or_event_func = or_event_func;
    client_ptr->or_cmd_err_func = or_cmd_err_func;
  }
  else
  {
    /* the client wants to de-register, just clear the event bit nask.
    */
    client_ptr->or_event_mask &= ~or_event_mask;
  }

  return PDSM_CLIENT_OK;

}/* End of pdsm_client_or_reg( ) */
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_pa_reg

DESCRIPTION
  This function allows a client to register the following Parameter-related
  callbacks and events with PDSM:

   1. Callback function to receive registered Parameter related Events
   2. Register and de-register PA events.

   If a client registers a callback function to receive PA event
   notification, it also needs to register one or more PA events it is
   interested in receiving.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_pa_reg(

  pdsm_client_id_type              client_id,
      /* client id */
  void                             *client_data_ptr,

  pdsm_pa_event_f_type             *pa_event_func,
      /* Pointer to a callback function to notify the client of PD
      ** events */

  pdsm_client_event_reg_e_type     event_reg_type,

  pdsm_pa_event_type               pa_event_mask,
      /* register for this event mask  */

  pdsm_pa_cmd_err_f_type           *pa_cmd_err_func
    /* pointer to a callback function to notify the client of PD
    ** command errors of OTHER clients */
)
{
  boolean reg_flag;
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  if( pa_event_func == NULL )
  {
   /* deregister all of client's PD events */
   client_ptr->pa_event_mask = 0;

   /* update PD event and other clients' command error callback functions */

   client_ptr->pa_event_func = pa_event_func;
   client_ptr->pa_cmd_err_func = pa_cmd_err_func;

   return PDSM_CLIENT_OK;

  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->pa_event_mask) & ~(PDSM_PA_EVENT_MASK) )
  {
    return PDSM_CLIENT_ERR_EVENT_MASK;
  }

  switch(event_reg_type)
  {
     case PDSM_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case PDSM_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return PDSM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  /* use the latest client data pointer.
  */
  client_ptr->pa_data_ptr = client_data_ptr;

  if ( reg_flag == TRUE)
  {
    /* In case of registration init the fields.
    */
    client_ptr->pa_event_mask = pa_event_mask;
    /* update PD event and other clients' command error callback functions */
    client_ptr->pa_event_func = pa_event_func;
    client_ptr->pa_cmd_err_func = pa_cmd_err_func;
  }
  else
  {
    /* If client wants to deregister, just clear his event mask.
    */
/*lint -e{502} as client_ptr does not negative number, this is safe */
    client_ptr->pa_event_mask &= ~pa_event_mask;
  }

  return PDSM_CLIENT_OK;

} /* End pdsm_client_pa_reg() */

/* <EJECT> */

/*===========================================================================

FUNCTION pdsm_client_pa_reg_ex

DESCRIPTION
  This function allows a client to register the following Parameter-related
  callbacks with PDSM:

   1. Callback function to receive Parameter events, all PA events will be
      reported, client will not be able to selectively register certain
      events
   2. Register and de-register Parameter events
   3. Callback function to receive other clients' PD-command errors.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  If client calls this function to register PA events, all PA events 
  will be reported via the registered callback function "pa_event_func_ex", 
  including all PA operations up to PDSM_PA_LPP_CONFIG_INFO. 

  If client also calls legacy pdsm_client_pa_reg() function to register for the 
  old bitmask-style PA event reporting, client will get event via the registered
  callback function "pa_event_func" only for PA operation up to 
  PDSM_PA_LPP_CONFIG_INFO (client will get event twice in this case, client
  needs to properly handle such case).
 
  If client register using both methods (pdsm_client_pa_reg() & pdsm_client_pa_reg_ex())
  it should also call both legacy & new interface to deregister completely.
===========================================================================*/
pdsm_client_status_e_type pdsm_client_pa_reg_ex(

    pdsm_client_id_type              client_id,
        /* Requesting client */
    void                             *client_data_ptr,

    pdsm_pa_event_ex_f_type          *pa_event_func_ex,
        /* Pointer to a callback function to notify the client of PA
        ** events */
    /*~ PARAM pa_event_func_ex POINTER */
    pdsm_client_event_reg_e_type     event_reg_type,

    pdsm_pa_cmd_err_f_type           *pa_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM pa_cmd_err_func POINTER */ 
)
{
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (NULL == client_ptr) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  if ( NULL == pa_event_func_ex ) // considered as deregister
  {
    /* update PD event and other clients' command error callback functions */
    client_ptr->pa_event_func_ex = pa_event_func_ex;
    client_ptr->pa_cmd_err_func_ex = pa_cmd_err_func;

    return PDSM_CLIENT_OK;
  }


  switch (event_reg_type)
  {
  case PDSM_CLIENT_EVENT_REG:
    // In case of registration init the fields.
    client_ptr->pa_event_func_ex = pa_event_func_ex;
    client_ptr->pa_cmd_err_func_ex = pa_cmd_err_func;
    break;

  case PDSM_CLIENT_EVENT_DEREG:
    // If client wants to deregister, just clear the callback functions
    client_ptr->pa_event_func_ex = NULL;
    client_ptr->pa_cmd_err_func_ex = NULL;
    break;

  default: 
    return PDSM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  // use the latest client data pointer.
  client_ptr->pa_data_ptr_ex = client_data_ptr;

  return PDSM_CLIENT_OK;
}
/*~ FUNCTION pdsm_client_pa_reg_ex */

/*===========================================================================

FUNCTION pdsm_client_lcs_reg

DESCRIPTION
  This function allows a client to register the following UI-related
  callbacks and events with PDSM:

   1. Callback function to receive registered LCS related Events
   2. Register and de-register LCS events.

   If a client registers a callback function to receive LCS event
   notification, it also needs to register one or more LCS events it is
   interested in receiving.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/

pdsm_client_status_e_type pdsm_client_lcs_reg(

    pdsm_client_id_type           client_id,
        /* Requesting client */
    void                          *client_data_ptr,

    pdsm_lcs_event_f_type          *lcs_event_func,
        /* Pointer to a callback function to notify the client of PD
        ** events */

    pdsm_client_event_reg_e_type  event_reg_type,

    pdsm_lcs_event_type            event_mask,
        /* register from this event (including) */

    pdsm_lcs_cmd_err_f_type        *lcs_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
)
{
  boolean reg_flag;
  struct pdsm_client_s  *client_ptr;

  client_ptr = pdsmclient_get_client_ptr( client_id );

  ni_client_id = client_id;

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  if( lcs_event_func == NULL )
  {
   /* deregister all of client's PD events */
   client_ptr->lcs_event_mask = 0;

   /* update PD event and other clients' command error callback functions */

   client_ptr->lcs_event_func = lcs_event_func;
   client_ptr->lcs_cmd_err_func = lcs_cmd_err_func;

   return PDSM_CLIENT_OK;

  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->lcs_event_mask) & ~(PDSM_LCS_EVENT_MASK) )
  {
    return PDSM_CLIENT_ERR_EVENT_MASK;
  }

  switch(event_reg_type)
  {
     case PDSM_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case PDSM_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return PDSM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  /* use the latest client data pointer.
  */
  client_ptr->lcs_data_ptr = client_data_ptr;

  if ( reg_flag == TRUE)
  {
    /* In case of registration init the fields.
    */
    client_ptr->lcs_event_mask = event_mask;
    /* update PD event and other clients' command error callback functions */
    client_ptr->lcs_event_func = lcs_event_func;
    client_ptr->lcs_cmd_err_func = lcs_cmd_err_func;
  }
  else
  {
    /* If client wants to deregister, just clear his event mask.
    */
/*lint -e{502} as client_ptr does not negative number, this is safe */
    client_ptr->lcs_event_mask &= ~event_mask;
  }

  return PDSM_CLIENT_OK;


}
/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_ext_status_reg

DESCRIPTION

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/

pdsm_client_status_e_type pdsm_client_ext_status_reg(

    pdsm_client_id_type           client_id,
        /* Requesting client */
    void                          *client_data_ptr,

    pdsm_ext_status_event_f_type          *ext_status_event_func,
        /* Pointer to a callback function to notify the client of PD
        ** events */

    pdsm_client_event_reg_e_type     event_reg_type,

    pdsm_ext_status_event_type               event_mask,
    pdsm_ext_status_cmd_err_f_type        *ext_status_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM ext_status_cmd_err_func POINTER */
)
{
  boolean reg_flag;
  struct pdsm_client_s  *client_ptr;


  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  if( ext_status_event_func == NULL )
  {
   /* deregister all of client's PD events */
   client_ptr->ext_status_event_mask = 0;

   /* update PD event and other clients' command error callback functions */

   client_ptr->ext_status_event_func = ext_status_event_func;


   return PDSM_CLIENT_OK;

  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->ext_status_event_mask) & ~(PDSM_EXT_STATUS_EVENT_MASK) )
  {
    return PDSM_CLIENT_ERR_EVENT_MASK;
  }

  switch(event_reg_type)
  {
     case PDSM_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case PDSM_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return PDSM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  /* use the latest client data pointer.
  */
  client_ptr->ext_status_data_ptr = client_data_ptr;

  if ( reg_flag == TRUE)
  {
    /* In case of registration init the fields.
    */
    client_ptr->ext_status_event_mask = event_mask;
    /* update PD event and other clients' command error callback functions */
    client_ptr->ext_status_event_func = ext_status_event_func;
    client_ptr->ext_status_cmd_err = ext_status_cmd_err_func;

  }
  else
  {
    /* If client wants to deregister, just clear his event mask.
    */
/*lint -e{502} as client_ptr does not negative number, this is safe */
    client_ptr->ext_status_event_mask &= ~event_mask;
  }

  return PDSM_CLIENT_OK;


}

/*===========================================================================

FUNCTION PDSM_NOTIFICATION_VERIFICATION_REQUEST

DESCRIPTION
  This function is used to  send a Notification verification request to a
  registered LCS client.

RETURN VALUE
  None

DEPENDENCIES
  Called by LCS Agent.

===========================================================================*/

boolean  pdsm_lcs_agent_client_event_ntfy
(
  pdsm_lcs_event_type      lcs_event,             /* Type of LCS event */
  pdsm_lcs_info_s_type     *lcs_info_ptr            /* data to be sent to LCS client */
)
{
  pdsm_client_s_type *client_ptr, *next_ptr;
  boolean      response_sent = FALSE;



   if( lcs_info_ptr == NULL )
   {
     PDSM_MSG_HIGH("Null pointer", 0,0,0);
     return FALSE;
   }


   /* unconditional loopback if NV item is so provisioned */
   if ( lcs_info_ptr->lcs_cb_cmd_type == PDSM_LCS_CMD_TYPE_SUPL_REQ )
   {
     if (q_pdapi_gnss_oem_feature_mask & PDSM_SM_NV_GNSS_OEM_FEATURE_MASK_4_SUPL_NI_NOTIF_LOOPBK)
     {
       pdsm_lcs_notification_resp_s_type  z_resp_info, *resp_info_ptr = &z_resp_info;
       boolean u_ret;

       resp_info_ptr->resp = PDSM_LCS_NOTIFY_VERIFY_ACCEPT;

       resp_info_ptr->lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_SUPL_REQ;
       memscpy ( &resp_info_ptr->lcs_info.supl_req, sizeof(resp_info_ptr->lcs_info.supl_req), &lcs_info_ptr->lcs_info.supl_req, sizeof(lcs_info_ptr->lcs_info.supl_req) );
       u_ret = pdsm_lcs_agent_client_response (NULL, NULL, resp_info_ptr, lcs_info_ptr->client_id); 
       if ( u_ret == TRUE )
       {
         MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Loopback SUPL NotifVerif for clientID: %u", lcs_info_ptr->client_id);
         return TRUE;
       }
       else
       {
         MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Loopback SUPL NotifVerif failure: ret=%u", u_ret);
         return FALSE;
       }
     }
   }
   else if ( lcs_info_ptr->lcs_cb_cmd_type == PDSM_LCS_CMD_TYPE_UMTS_CP_REQ )
   {
     if ( q_pdapi_gnss_oem_feature_mask & PDSM_SM_NV_GNSS_OEM_FEATURE_MASK_5_CP_SS_NOTIF_LOOPBK )
     {
       pdsm_lcs_notification_resp_s_type  z_resp_info, *resp_info_ptr = &z_resp_info;
       boolean u_ret;
       resp_info_ptr->resp = PDSM_LCS_NOTIFY_VERIFY_ACCEPT;

       resp_info_ptr->lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_UMTS_CP_REQ;
       memscpy ( &resp_info_ptr->lcs_info.umts_cp_req, sizeof(resp_info_ptr->lcs_info.umts_cp_req), &lcs_info_ptr->lcs_info.umts_cp_req, sizeof(lcs_info_ptr->lcs_info.umts_cp_req) );
       u_ret = pdsm_lcs_agent_client_response (NULL, NULL, resp_info_ptr, lcs_info_ptr->client_id);
       if ( u_ret == TRUE )
       {
         MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Loopback CP NotifVerif for clientID: %u", lcs_info_ptr->client_id);
         return TRUE;
       }
       else
       {
         MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Loopback CP NotifVerif failure: ret=%u", u_ret);
         return FALSE;
       }
     }
   }


   os_MutexLock(&z_pdapi_client_mutex);

   client_ptr = pdsmclient_list_first();

   while (client_ptr != NULL)
   {
     /* Save the next client list ptr */
     next_ptr = pdsmclient_list_next(client_ptr);

     /* call the LCS registered call back function */
     if ( (client_ptr->lcs_event_func != NULL) &&
         (client_ptr->lcs_event_mask & lcs_event) )
     {
       client_ptr->lcs_event_func(client_ptr->lcs_data_ptr, lcs_event, lcs_info_ptr );
       response_sent = TRUE;
     }
     /* Set the current client to the next client in list */
     client_ptr = next_ptr;
   }
   if ( response_sent == FALSE ) /* if not registered, use this to test MT */
   {
      if ( lcs_info_ptr->lcs_cb_cmd_type == PDSM_LCS_CMD_TYPE_SUPL_REQ )
      {
        if ( lcs_info_ptr->lcs_info.supl_req.notification_priv_type == PDSM_LCS_NO_NOTIFY_NO_VERIFY )
        {
            pdsm_lcs_notification_resp_s_type  z_resp_info, *resp_info_ptr = &z_resp_info;

            resp_info_ptr->resp = PDSM_LCS_NOTIFY_VERIFY_ACCEPT;

            resp_info_ptr->lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_SUPL_REQ;
            memscpy ( &resp_info_ptr->lcs_info.supl_req, sizeof(resp_info_ptr->lcs_info.supl_req), &lcs_info_ptr->lcs_info.supl_req, sizeof(lcs_info_ptr->lcs_info.supl_req) );
            (void)pdsm_lcs_agent_client_response (NULL, NULL, resp_info_ptr, lcs_info_ptr->client_id);
        }
      }
      else if ( lcs_info_ptr->lcs_cb_cmd_type == PDSM_LCS_CMD_TYPE_UMTS_CP_REQ )
      {
        if ( lcs_info_ptr->lcs_info.umts_cp_req.notification_priv_type == PDSM_LCS_USER_NOTIFY_ONLY )
        {
            pdsm_lcs_notification_resp_s_type  z_resp_info, *resp_info_ptr = &z_resp_info;

            resp_info_ptr->resp = PDSM_LCS_NOTIFY_VERIFY_ACCEPT;

            resp_info_ptr->lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_UMTS_CP_REQ;
            memscpy ( &resp_info_ptr->lcs_info.umts_cp_req, sizeof(resp_info_ptr->lcs_info.umts_cp_req), &lcs_info_ptr->lcs_info.umts_cp_req, sizeof(lcs_info_ptr->lcs_info.umts_cp_req) );
            (void)pdsm_lcs_agent_client_response (NULL, NULL, resp_info_ptr, lcs_info_ptr->client_id);
        }
      }
   }

   os_MutexUnlock(&z_pdapi_client_mutex);

   return TRUE;

} /* End pdsm_notification_verification_request( ) */



/*===========================================================================

FUNCTION PDAPI_INIT

DESCRIPTION
  Initialize the Position Determination API - called from the PD task

RETURN VALUE
  None

DEPENDENCIES
  Called only once from PDSM_TASK or lsm_task

===========================================================================*/

void pdapi_init( void )
{
  /* Now initialize different queue's. When P-FIFO is used a new queue will be used,
  ** which stores the request's in P-FIFO manner.
  */

  (void)os_MutexInit(&z_pdapi_client_mutex, MUTEX_DATA_ONLY_CONTEXT);

  /* Initialize the Client free q */
  pdsmclient_free_q_init( );

  /* Init the client list Q, Queue to maintain list of active clients.
  */
  pdsmclient_q_init();

} /* End pdapi_init() */

/*===========================================================================

FUNCTION pdsm_client_xtra_reg

DESCRIPTION
This function allows a client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_xtra_reg
(
  pdsm_client_id_type           client_id,
  /* Requesting client */
  void                          *client_data_ptr,

  pdsm_xtra_event_f_type          *xtra_event_func,
  /* Pointer to a callback function to notify the client of PD
  ** events */

  pdsm_client_event_reg_e_type  event_reg_type,
  /* Register or deregister events */

  pdsm_xtra_event_type            event_mask,
  /* register from this event (including) */

  pdsm_xtra_cmd_err_f_type        *xtra_cmd_err_func
  /* pointer to a callback function to notify the client of XTRA
  ** command errors of OTHER clients */
)
{
#ifdef FEATURE_CGPS_XTRA
  boolean reg_flag;
  struct pdsm_client_s  *client_ptr;

  /* Only XTRA client can register for events */
  if (pdsmclient_get_client_type_map(client_id) != PDSM_CLIENT_TYPE_XTRA)
  {
    MSG_ERROR("XTRA: invalid client id %d", client_id, 0, 0);
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  if( xtra_event_func == NULL )
  {
    /* deregister all of client's XTRA events */
    client_ptr->xtra_event_mask = PDSM_XTRA_EVENT_NONE;

    /* update event and other clients' command error callback functions */
    client_ptr->xtra_event_func = xtra_event_func;
    client_ptr->xtra_cmd_err_func = xtra_cmd_err_func;

    return PDSM_CLIENT_OK;
  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->xtra_event_mask) & ~(PDSM_XTRA_EVENT_MASK) )
  {
    return PDSM_CLIENT_ERR_EVENT_MASK;
  }

  /* Determine if this request if for registering or deregistering */
  switch(event_reg_type)
  {
  case PDSM_CLIENT_EVENT_REG:
    reg_flag=TRUE;
    break;

  case PDSM_CLIENT_EVENT_DEREG:
    reg_flag=FALSE;
    break;

  default:
    return PDSM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  /* use the latest client data pointer */
  client_ptr->xtra_data_ptr = client_data_ptr;

  if ( reg_flag == TRUE)
  {
    /* In case of registration init the fields */
    client_ptr->xtra_event_mask = event_mask;

    /* update XTRA event and other client command error callback functions */
    client_ptr->xtra_event_func = xtra_event_func;
    client_ptr->xtra_cmd_err_func = xtra_cmd_err_func;
  }
  else
  {
    /* If client wants to deregister, just clear his event mask */
/*lint -e{502} as client_ptr does not negative number, this is safe */
    client_ptr->xtra_event_mask &= ~event_mask;
  }


  return PDSM_CLIENT_OK;
#else
  return PDSM_CLIENT_ERR_CLIENT_INIT_FAIL;
#endif /* FEATURE_CGPS_XTRA */
} /* pdsm_client_xtra_reg */



/*===========================================================================

FUNCTION pdsm_client_xtra_t_reg

DESCRIPTION
This function allows a client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_xtra_t_reg
(
  pdsm_client_id_type           client_id,
  /* Requesting client */
  void                          *client_data_ptr,

  pdsm_xtra_t_event_f_type      *xtra_t_event_func,
  /* Pointer to a callback function to notify the client of PD
  ** events */

  pdsm_client_event_reg_e_type  event_reg_type,
  /* Register or deregister events */

  pdsm_xtra_t_event_type        xtra_t_event_mask,
  /* register from this event (including) */

  pdsm_xtra_t_cmd_err_f_type    *xtra_t_cmd_err_func
  /* pointer to a callback function to notify the client of XTRA-T
  ** command errors of OTHER clients */
)
{
#ifdef FEATURE_CGPS_XTRA_T
  boolean reg_flag;
  struct pdsm_client_s  *client_ptr;

  /* Only XTRA-T client can register for events */
  if (pdsmclient_get_client_type_map(client_id) != PDSM_CLIENT_TYPE_XTRA_T)
  {
    MSG_ERROR("XTRA-T: invalid client id %d", client_id, 0, 0);
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr = pdsmclient_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_INIT_MASK) )
  {
    return PDSM_CLIENT_ERR_CLIENT_ID;
  }

  if( xtra_t_event_func == NULL )
  {
    /* deregister all of client's XTRA-T events */
    client_ptr->xtra_t_event_mask = PDSM_XTRA_T_EVENT_NONE;

    /* update event and other clients' command error callback functions */
    client_ptr->xtra_t_event_func = xtra_t_event_func;
    client_ptr->xtra_t_cmd_err_func = xtra_t_cmd_err_func;

    return PDSM_CLIENT_OK;
  }

  /* Check if any of the registered events is valid */
  if ( (client_ptr->xtra_t_event_mask) & ~(PDSM_XTRA_T_EVENT_MASK) )
  {
    return PDSM_CLIENT_ERR_EVENT_MASK;
  }

  /* Determine if this request if for registering or deregistering */
  switch(event_reg_type)
  {
  case PDSM_CLIENT_EVENT_REG:
    reg_flag=TRUE;
    break;

  case PDSM_CLIENT_EVENT_DEREG:
    reg_flag=FALSE;
    break;

  default:
    return PDSM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  /* use the latest client data pointer */
  client_ptr->xtra_t_data_ptr = client_data_ptr;

  if ( reg_flag == TRUE)
  {
    /* In case of registration init the fields */
    client_ptr->xtra_t_event_mask = xtra_t_event_mask;

    /* update XTRA-T event and other client command error callback functions */
    client_ptr->xtra_t_event_func = xtra_t_event_func;
    client_ptr->xtra_t_cmd_err_func = xtra_t_cmd_err_func;
  }
  else
  {
    /* If client wants to deregister, just clear his event mask */
    client_ptr->xtra_t_event_mask &= ~xtra_t_event_mask;
  }

  return PDSM_CLIENT_OK;
#else
  return PDSM_CLIENT_ERR_CLIENT_INIT_FAIL;
#endif /* FEATURE_CGPS_XTRA_T */
} /* pdsm_client_xtra_t_reg */


/*===========================================================================

FUNCTION pdsmclient_free_q_init

DESCRIPTION
  This is used to initialize the free q for Client object's.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request statuss

SIDE EFFECTS
  none

===========================================================================*/

void pdsmclient_free_q_init( void )
{
  uint8 i;

  os_MutexLock(&z_pdapi_client_mutex);

  (void)q_init( &pdsmclient_free_q );

  for( i=0; i < ARR_SIZE(pdsmclient_arr); i++ )
  {
    (void) q_link( &pdsmclient_arr[i], &pdsmclient_arr[i].link );
    q_put( &pdsmclient_free_q, &pdsmclient_arr[i].link );
  } /* End for client array element's */

  os_MutexUnlock(&z_pdapi_client_mutex);
} /* End pdsmclient_free_q_init() */


/*===========================================================================

FUNCTION pdsmclient_q_init

DESCRIPTION
  This is used to initialize the client object q.

DEPENDENCIES

RETURN VALUE
  request statuss

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_q_init( void )
{
  PDSM_FUNC_START(" pdsmclient_list_init()",0,0,0);

  /* Init the Client Queue.
  */
  os_MutexLock(&z_pdapi_client_mutex);
  (void)q_init( CLIENT_LIST_Q_PTR() );
  os_MutexUnlock(&z_pdapi_client_mutex);
}

/*===========================================================================

FUNCTION PSMCLIENT_CLIENT_INFO_ALLOC

DESCRIPTION
  This is used to allocate the info buffer for client. All the clients have
  a statically allocated buffer for reporting client info. This buffer is reused
  whenever any new info is required to be passed to the user. Currently all the
  active clients have their own client id field set when the data is send back,
  so we require a per client buffer.

DEPENDENCIES

RETURN VALUE
  request statuss

SIDE EFFECTS
  none

===========================================================================*/
pdsm_pd_info_s_type *pdsmclient_info_alloc( pdsm_client_s_type   *client_ptr )
{
  uint8 i;
  pdsm_pd_info_s_type* client_type_ptr = NULL;

  /*---------------------------------------------------------------------*/

  os_MutexLock(&z_pdapi_client_mutex);
  for ( i = 0; i < ARR_SIZE( pdsmclient_map_arr ); i++ )
  {
    if ( client_ptr == pdsmclient_map_arr[i].client_ptr )
    {
      client_type_ptr = pdsmclient_map_arr[i].info_ptr;
      break;
    }
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return client_type_ptr;
} /* End pdsmclient_info_alloc() */



/*===========================================================================
FUNCTION pdsm_dummy_ni_event_cb
DESCRIPTION
   This function is used to as a dummy NI event call back when there is no
   registered real NI event call back in the case where direct loop back for
   NI trigger is provisioned.
RETURN VALUE
     void
DEPENDENDENCIES
===========================================================================*/
void pdsm_dummy_ni_event_cb(  
  void *user_data,
  pdsm_lcs_event_type  lcs_event,
  const pdsm_lcs_info_s_type *lcs_info_ptr
  )
{
  return;
}

/*===========================================================================
FUNCTION pdsm_ni_client_id_get
DESCRIPTION
   This function is used to get the pdapi client id for the NI requests
RETURN VALUE
     void
DEPENDENDENCIES
===========================================================================*/
pdsm_client_id_type pdsm_ni_client_id_get (void)
{
  if (ni_client_id)
  {
     return ni_client_id;
  }
  else
  {
    if ( ( q_pdapi_gnss_oem_feature_mask & 
           PDSM_SM_NV_GNSS_OEM_FEATURE_MASK_4_SUPL_NI_NOTIF_LOOPBK ) ||
         ( q_pdapi_gnss_oem_feature_mask &
           PDSM_SM_NV_GNSS_OEM_FEATURE_MASK_5_CP_SS_NOTIF_LOOPBK )
       )
    {

      pdsm_client_id_type dummy_client_id = 0;
      pdsm_client_status_e_type   status = 0;

      dummy_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_NI);

      if (dummy_client_id == -1)
      {
        PDSM_MSG_HIGH("dummy pdsm_client_init for NI failed", 0,0,0);
      }
      else
      {
        status= pdsm_client_lcs_reg(dummy_client_id,
                                      NULL,
                                      pdsm_dummy_ni_event_cb,
                                      PDSM_CLIENT_EVENT_REG,
                                      PDSM_LCS_EVENT_MASK,
                                      NULL
                                      );
        if(status!= PDSM_CLIENT_OK)
        {
           PDSM_MSG_HIGH ("dummy pdsm_client_lcs_reg failed: %d", status, 0, 0);
        }

      }

      if (ni_client_id)
      {
        PDSM_MSG_MED("dummy pdsm_client_init ok: %d", ni_client_id,0,0);
        return ni_client_id;
      }
      else
      {
        PDSM_MSG_HIGH("dummy NI pdsm_client attempt failed: %d, %d ", dummy_client_id,status,0);
        return -1;
      }

    }

    else /* no special direct loopback provisioned */
    {
      return -1;
    }

  } /* NI client not registered */
}

/*===========================================================================

FUNCTION PDSM_XTRA_CLIENT_REQUEST

DESCRIPTION
This function is used to send a request to a registered XTRA client.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/

boolean pdsm_xtra_client_request
(
 pdsm_xtra_event_type      xtra_event,        /* Type of XTRA event */
 pdsm_xtra_info_s_type     *xtra_info_ptr     /* data to be sent to XTRA client */
)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( xtra_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("Null pointer", 0,0,0);
    return FALSE;
  }

  os_MutexLock(&z_pdapi_client_mutex);

  /* Traverse the client list and find the XTRA client
  ** send the event to it.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    /* call the XTRA registered call back function */
    if ( (client_ptr->xtra_event_func != NULL) &&
         (client_ptr->xtra_event_mask & xtra_event) )
    {
      /* Set the client ID */
      xtra_info_ptr->client_id = client_ptr->client_id;

      client_ptr->xtra_event_func(client_ptr->xtra_data_ptr, xtra_event, xtra_info_ptr);
    }

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  os_MutexUnlock(&z_pdapi_client_mutex);

  return TRUE;

} /* End pdsm_notification_verification_request( ) */


/*===========================================================================

FUNCTION PDSM_XTRA_T_CLIENT_REQUEST

DESCRIPTION
This function is used to send a request to a registered XTRA-T client.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/
boolean pdsm_xtra_t_client_request
(
 pdsm_xtra_t_event_type      xtra_t_event,        /* Type of XTRA-T event */
 pdsm_xtra_t_info_s_type     *xtra_t_info_ptr     /* data to be sent to XTRA-T client */
)
{
  pdsm_client_s_type *client_ptr, *next_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( xtra_t_info_ptr == NULL )
  {
    PDSM_MSG_HIGH("xtra_t_info_ptr Null", 0,0,0);
    return FALSE;
  }


  /* Traverse the client list and find the XTRA-T client
  ** send the event to it.
  */
  client_ptr = pdsmclient_list_first();

  while (client_ptr != NULL)
  {
    /* Save the next client list ptr */
    next_ptr = pdsmclient_list_next(client_ptr);

    /* call the XTRA-T registered call back function */
    if ( (client_ptr->xtra_t_event_func != NULL) &&
         (client_ptr->xtra_t_event_mask & xtra_t_event) )
    {
      /* Set the client ID */
      xtra_t_info_ptr->client_id = client_ptr->client_id;

      client_ptr->xtra_t_event_func(client_ptr->xtra_data_ptr, xtra_t_event, xtra_t_info_ptr);
    }

    /* Set the current client to the next client in list */
    client_ptr = next_ptr;
  }

  return TRUE;

} /* pdsm_xtra_t_client_request */


