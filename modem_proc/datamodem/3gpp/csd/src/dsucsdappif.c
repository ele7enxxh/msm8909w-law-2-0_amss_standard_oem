/*===========================================================================

             U M T S   C I R C U I T - S W I T C H E D   D A T A

                  A P P L I C A T I O N   I N T E R F A C E


DESCRIPTION
  This file contains the implementation of the interfaces for UMTS 
  Circuit-Switched Data applications. The interfaces support only (external) 
  Video Telephony calls now. They do not support external Asynchronous or
  Synchronous CS Data calls yet.

EXTERNALIZED FUNCTIONS
  ds_ucsd_app_register_client()
    API for registering CS Data Application callback functions

  ds_ucsd_app_deregister_client()
    API for deregistering CS Data Application callback functions
  
  ds_ucsd_app_originate_call()
    API for originating a CS Data call
    
  ds_ucsd_app_confirm_call()
    API for confirming CS Data call setup

  ds_ucsd_app_answer_call()
    API for answering a CS Data call
    
  ds_ucsd_app_end_call()
    API for ending a CS Data call

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds_ucsd_app_register_callback_functions() should be called before
  before a CS Data call is made.

   Copyright (c) 2003 - 2014 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdappif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/12   ND      Memory reduction effort in 3gpp.
09/29/11   SA      Added support for TD-SCDMA.
04/01/11   TTV     Support for Q6 free floating.
09/29/09   SA      Fixed KW errors.
07/09/09   BS      Fixed offtarget lint medium and low's.
03/04/09   SA      AU level CMI modifications.
09/01/08   SA      Added modifications for CMI defeaturization.
04/01/08   SA      Added routine ds_ucsd_api_get_calltype().
03/11/08   AR      Rework dial string handling for IDL remoted interface 
06/27/07   AR      Add multiple client support for unified UCSD API.
03/28/07   AR      More Lint error corrections.
02/26/07   AR      Replace banned APIs with safer routines.
03/28/07   AR      Added FEATURE_UUS wrapping for EDGE only build.
12/14/06   AR      Add deregister interface for WindowsMobile client. 
11/15/06   AR      Export UCSD API for WindowsMobile access.
09/01/06   AR      Added support for multiprocessor build.
02/15/06   AR      Remove obsolete call orig callback.
02/09/06   AR      Lint corrections.
11/21/05   AR      Accomodate ds_ucsd_get_instid() interface change.
09/15/05   AR      Add support for service change & fallback.
06/16/05   ar      Add support for asynchronous call setup response. 
05/20/05   ar      Added call_progress_cb to callback registration.
11/12/04   dwp     Add support for VT two stage dialing via User to User IE.
11/03/04   dwp     Add support for sending reject cause for incoming call.
07/16/04   ar      Added supplementary services dial modifer to API.
03/24/04   ar      Added routine ds_ucsd_lookup_cm_callid()
03/16/04   ar      Preserve passed dial string in local buffer.
08/13/03   dwp     Add callback for Call Confirmation for VT.
06/20/03   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#include <stringl/stringl.h>
#include "data_msg.h"
#include "dsucsdappif_apis.h" 


#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
#include "err.h"
#include "cm.h"
#include "dstask.h"
#include "dsat_v.h"
#include "dsucsdi.h"
#include "dsucsdhdlr.h"
#include "dsucsdappifi.h"




/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

/* Macro to test for permitted call types for event subscription */
#define DSUCSD_API_CALL_TYPES_SUBSCRIBE_MASK \
  ( (uint8)DS_UCSD_ASYNC_CSD_CALL | \
    (uint8)DS_UCSD_SYNC_CSD_CALL | \
    (uint8)DS_UCSD_VIDEO_TELEPHONY_CALL )

#define DSUCSD_API_CALL_TYPES_SCUDIF_MASK \
  ( (uint8)DSUCSD_API_CALL_TYPES_SUBSCRIBE_MASK | \
    (uint8)DS_UCSD_VOICE_CALL )


/* Macro to test for valid call type within mask (TRUE indicates error) */
#define VALIDATE_CALL_TYPE( call_type, mask ) \
      (IS_CALL_TYPE(call_type, ~mask) || \
       ((uint8)DS_UCSD_UNKNOWN_CALL == (uint8)call_type))

/* Macro to test for valid client ID range (TRUE indicates error) */
#define VALIDATE_CLIENT_ID( client_id ) \
      ((DSUCSD_INVALID_CLIENT_ID >= client_id) || \
       (DSUCSD_MAX_CLIENT_ID < client_id))

/*---------------------------------------------------------------------------
  API client registration information
---------------------------------------------------------------------------*/
static ds_ucsd_client_type  ds_ucsd_app_clients[DSUCSD_API_MAX_CLIENTS];

/* Declare a task critical section for serialized code execution. Used
 * in place of TASKLOCK()/TASKFREE(). */
static rex_crit_sect_type ds_ucsd_app_client_crit_sect;

/* Indicate client support has been initialized */
static boolean ds_ucsd_app_client_init_done = FALSE;

/* Flag for reading from NV whether the incoming call will be routed
 * to internal app via CSD API */

/* Call Manager callID to UMTS CS Data call instance table.  Provides
** direct lookup for CM event handlers.
*/

/*---------------------------------------------------------------------------
  Dial string from CS Data applicaton
---------------------------------------------------------------------------*/
#define MAX_DIAL_STRING_CHARS  CM_MAX_CALLED_INFO_CHARS
static uint8 ds_ucsd_app_dial_string[MAX_DIAL_STRING_CHARS+1] = {0};   

/*---------------------------------------------------------------------------
  User to User (UUS1) H.323 address, used in VT 2 stage dialing
---------------------------------------------------------------------------*/
static uint8 ds_ucsd_uus1_addr[MAX_USER_USER_DATA_1_LENGTH + 1] = {0};

/*---------------------------------------------------------------------------
  Default data path information for call service change   
---------------------------------------------------------------------------*/
static ds_ucsd_dpath_info_type  ds_ucsd_app_default_dpath_info = {NULL};


/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS_UCSD_LOOKUP_CM_CALLID

DESCRIPTION
   This function performs lookup in the call table, ds_ucsd_call_tbl[],
   to find the Call Manager call ID which correspons to the passed CS
   Data internal call ID. 
        
DEPENDENCIES
   None
  
RETURN VALUE
   CM call ID or CM_CALL_ID_INVALID.
    
SIDE EFFECTS
   None   
===========================================================================*/
uint8 ds_ucsd_lookup_cm_callid
(
  const uint8 inst_id
)
{
   uint8 index;                                       /* Call table index  */
   uint8 cm_call_id = CM_CALL_ID_INVALID;             /* Call ID to return */
   ds_ucsd_call_tbl_entry_type * ds_ucsd_call_tbl_ptr;

/*-------------------------------------------------------------------------*/

   /* Find entry in the call table with matching CS callID */
   for ( index = 0; 
         index < MAX_UCSD_CALL_TBL_ENTRIES; 
         index++ ) 
   {
     ds_ucsd_call_tbl_ptr = ds_ucsd_get_call_tbl(index);
     if (( NULL != ds_ucsd_call_tbl_ptr ) &&
         ( inst_id == ds_ucsd_call_tbl_ptr->inst_id ))
      {
        /* Found target entry in the call table */
        cm_call_id = ds_ucsd_call_tbl_ptr->cm_call_id;
        break;
      }
   }

   if ( index >= MAX_UCSD_CALL_TBL_ENTRIES ) 
   {
     /* Did not find target call_id */
     DATA_MSG1_ERROR( "Cannot find CS call instance: %d", inst_id );
   }
   return ( cm_call_id );
} /* ds_ucsd_lookup_cm_callid() */


/*===========================================================================

FUNCTION DS_UCSD_APP_GET_DPATH_INFO

DESCRIPTION
  This function returns the client data path inforation.
        
DEPENDENCIES
   None
  
RETURN VALUE
   Pointer to data path structure.
    
SIDE EFFECTS
   None   
===========================================================================*/
ds_ucsd_dpath_info_type*  ds_ucsd_app_get_dpath_info( void )
{
  return &ds_ucsd_app_default_dpath_info;
}


/*===========================================================================

FUNCTION DS_UCSD_APP_INIT

DESCRIPTION
  This function initializes the data structures for the client API.
  It should be called once during powerup.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_app_init( void )
{
  /* Setup the client registration tracking */
  memset( (void*)ds_ucsd_app_clients, 0x0, sizeof(ds_ucsd_app_clients) );
  
  /* Initialize task critical section */
  memset(&ds_ucsd_app_client_crit_sect, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect( &ds_ucsd_app_client_crit_sect );

  ds_ucsd_app_client_init_done = TRUE;
} /* ds_ucsd_app_init() */



/*===========================================================================

FUNCTION DS_UCSD_APP_ALLOCATE_CLIENT

DESCRIPTION
  This function attempts to allocate a free record from the client
  table.  If free record is found, the record is updated indicate in
  use and the reference is returned to the caller along with succcess
  return code.  If no free record is found, failure return code is returned. 
  
  A critical section is used here and in release function to ensure
  multiple threads do not impact each other.

DEPENDENCIES
  ds_ucsd_app_init() must have been previously invoked.
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_app_allocate_client
(
  ds_ucsd_client_type  **client_pptr
)
{
  uint8 index;

  if( NULL == client_pptr )
  {
    DATA_MSG0_ERROR("NULL client pointer specified");
    return FALSE;
  }
  
  *client_pptr = NULL;

  /* Enter critical section to ensure serialized exection. */
  rex_enter_crit_sect( &ds_ucsd_app_client_crit_sect );
  
  /* Traverse client table for available record */
  for( index=0; index<DSUCSD_API_MAX_CLIENTS; index++ )
  {
    /* Check for free record */
    if( FALSE == ds_ucsd_app_clients[index].in_use )
    {
      ds_ucsd_app_clients[index].in_use = TRUE;
      ds_ucsd_app_clients[index].client_id = index;
      *client_pptr = &ds_ucsd_app_clients[index];
      DATA_MSG1_MED( "Allocated UCSD client: %d",index);
      break;
    }
  }

  /* Allow waiting task to proceed. */
  rex_leave_crit_sect( &ds_ucsd_app_client_crit_sect );

  return (NULL == *client_pptr)? FALSE : TRUE;
} /* ds_ucsd_app_allocate_client() */



/*===========================================================================

FUNCTION DS_UCSD_APP_FIND_CLIENT

DESCRIPTION
  This function attempts to find the specified client identifier in
  the client table for matching call types.  If found, the record
  pointer is returned to the caller if a pointer was passed (NULL
  permitted).

DEPENDENCIES
  ds_ucsd_app_init() must have been previously invoked.
  
RETURN VALUE
  TRUE if client record was found, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_app_find_client
(
  const ds_ucsd_client_id_type client_id,
  const ds_ucsd_client_type  **client_pptr
)
{
  uint8 index;
  boolean status = FALSE;

  /* Validate client identifier */
  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR("Invalid client identifier specified: %d",client_id);
    return FALSE;
  }
  
  /* Traverse client table for specified  record */
  for( index=0; index<DSUCSD_API_MAX_CLIENTS; index++ )
  {
    /* Check for matching allocated record */
    if( (client_id == ds_ucsd_app_clients[index].client_id) &&
        (TRUE == ds_ucsd_app_clients[index].in_use) )
    {
      /* Set client record pointer if specified */
      if( NULL != client_pptr )
      {
        *client_pptr = &ds_ucsd_app_clients[index];
      }
      status = TRUE;
      break;
    }
  }

  return status;
} /* ds_ucsd_app_find_client() */



/*===========================================================================

FUNCTION DS_UCSD_APP_RELEASE_CLIENT

DESCRIPTION
  This function clears the registration information from the client
  table based on the client identifier.  If specified client is not
  found, error status is returned.

  A critical section is used here and in allocate function to ensure
  multiple threads do not impact each other.

DEPENDENCIES
  ds_ucsd_app_init() must have been previously invoked.
  
RETURN VALUE
  TRUE is operation successful, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_app_release_client
(
  const ds_ucsd_client_id_type client_id
)
{
  boolean status = FALSE;
  const ds_ucsd_client_type  *client_ptr = NULL;
  
  /* Enter critical section to ensure serialized exection. */
  rex_enter_crit_sect( &ds_ucsd_app_client_crit_sect );

  /* Find the matching allocated record for supported call types */
  status = ds_ucsd_app_find_client( client_id, &client_ptr );
  if( status )
  {
    /* Clear client record */
    memset( (void*)client_ptr, 0x0, sizeof(ds_ucsd_client_type) );
    DATA_MSG1_MED( "Released UCSD client: %d",client_id);
  }

  /* Allow waiting task to proceed. */
  rex_leave_crit_sect( &ds_ucsd_app_client_crit_sect );
  
  return status;
} /* ds_ucsd_app_release_client() */



/*===========================================================================

FUNCTION DS_UCSD_APP_NOTIFY_CLIENTS_BY_TYPE

DESCRIPTION
  This function traverses the client table to find those clients
  registered for a given call type.  For each record match, the
  registered callback function is invoked to post the event to the
  client.

DEPENDENCIES
  ds_ucsd_app_init() must have been previously invoked.
  
RETURN VALUE
  TRUE if one or more clients were notified, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_app_notify_clients_by_type
(
  const uint8                     call_type,
  const ds_ucsd_call_event_type  *event_ptr
)
{
  uint8 index;
  boolean status = FALSE;

  /* Validate parameters */
  if( NULL == event_ptr )
  {
    DATA_MSG0_ERROR( "Unexpected NULL event pointer");
    return FALSE;
  }

  if( !IS_CALL_TYPE(call_type,DSUCSD_API_CALL_TYPES_SUBSCRIBE_MASK) )
  {
    DATA_MSG2_ERROR("Invalid call type specified: %02X   permitted: %02X",
               call_type, DSUCSD_API_CALL_TYPES_SUBSCRIBE_MASK);
    return FALSE;
  }

  /* Check for internal routing enabled */
  if( !ds_ucsd_is_incoming_internal_call_enabled() )
  {
   DATA_MSG0_ERROR("Internal routing disabled, ignoring event"); 
    return FALSE;
  }

  
  /* Traverse client table for matching records */
  for( index=0; index<DSUCSD_API_MAX_CLIENTS; index++ )
  {
    ds_ucsd_client_type *c_ptr = &ds_ucsd_app_clients[index];
    
    /* Check for matching call type */
    if( (TRUE == c_ptr->in_use) &&
        IS_CALL_TYPE(call_type, c_ptr->info.call_types) )
    {
      /* Check for registered callback function */
      if( NULL != c_ptr->info.call_event_cb )
      {
        /* Notify this client of event */
        c_ptr->info.call_event_cb( event_ptr, c_ptr->info.user_info_ptr );
        status = TRUE;

        DATA_MSG2_MED( "Posted UCSD event: evt=%d clnt=%d",
                event_ptr->call_event, c_ptr->client_id);
      }
    }
  }

  return status;
} /* ds_ucsd_app_notify_clients_by_type() */


/*===========================================================================

FUNCTION DS_UCSD_APP_NOTIFY_CLIENT

DESCRIPTION
  This function find a specified client in client table and on match,
  the registered callback function is invoked to post the event to the
  client.

DEPENDENCIES
  ds_ucsd_app_init() must have been previously invoked.
  
RETURN VALUE
  TRUE if one or more clients were notified, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_app_notify_client
(
  const ds_ucsd_client_id_type    client_id,
  const ds_ucsd_call_event_type  *event_ptr
)
{
  boolean status = FALSE;
  const ds_ucsd_client_type  *client_ptr = NULL;

  /* Validate parameters */
  if( NULL == event_ptr )
  {
    DATA_MSG0_ERROR( "Unexpected NULL event pointer");
    return FALSE;
  }

  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR( "Invalid client identifier specified: %d",client_id);
    return FALSE;
  }

  /* Check for internal routing enabled */
  if( !ds_ucsd_is_incoming_internal_call_enabled() )
  {
    DATA_MSG0_ERROR( "Internal routing disabled, ignoring event"); 
    return FALSE;
  }
  
  /* Find the matching allocated record for supported call types */
  status = ds_ucsd_app_find_client( client_id, &client_ptr );
  if( status )
  {
    /* Check for registered callback function */
    if( NULL != client_ptr->info.call_event_cb )
    {
      /* Notify this client of event */
      client_ptr->info.call_event_cb( event_ptr,
                                      client_ptr->info.user_info_ptr );
      DATA_MSG2_MED("Posted UCSD event: evt=%d clnt=%d",
              event_ptr->call_event, client_id);
    }
  }
  
  return status;
} /* ds_ucsd_app_notify_client() */



/*===========================================================================

FUNCTION DS_UCSD_APP_INIT_DATAPATH

DESCRIPTION
  This function initializes a passed datapath structure based on the
  specified call instance.  Datapath is either via SIO/SMD or
  watermark queues.  It is an error condition to use watermarks with
  the remoted interface.

DEPENDENCIES
  None

RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_app_init_datapath
(
  ds_ucsd_call_tbl_entry_type  *call_info_ptr,
  ds_ucsd_datapath_type        *datapath_ptr
)
{
  if( (NULL == call_info_ptr) ||
      (NULL == datapath_ptr) )
  {
    DATA_MSG0_ERROR("Unexpected NULL pointers specified");
    return;
  }
  

  /* Initialize the datapath structure based on call info. */
  if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
  {
    /* Datapath via SIO (SMD) */
    datapath_ptr->mode = DS_UCSD_DPATH_MODE_SIO;
    datapath_ptr->info.sio_info.sio_port =
      ds3g_siolib_map_ds_port( call_info_ptr->sio_port_id );
  }
  else
  {
    /* Datapath via watermark queues */
#ifdef FEATURE_EXPORT_DSUCSDAPPIF_APIS
    /* Not supported on remoted API builds */
    DATA_MSG0_FATAL("Invalid datapath configuration for build");
    memset((void*)datapath_ptr, 0x0, sizeof(ds_ucsd_datapath_type));
#else
    datapath_ptr->mode = DS_UCSD_DPATH_MODE_WM;
    datapath_ptr->info.wm_info = &call_info_ptr->dpath_params;
#endif    
  }
  datapath_ptr->chan_info = &call_info_ptr->chan_info;

  return;
} /* ds_ucsd_app_init_datapath() */

#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */



/*===========================================================================

FUNCTION DS_UCSD_API_REGISTER_CLIENT

DESCRIPTION
  This function is used to register the application client with the UCSD
  subtask. It should be called by the application task at initialization.

  Parameters:
    call_types     - Bitmap of UCSD call types for which client subscribes
                     for call control notification events.
                     Fax and voice calls are not supported at this time.

    call_event_cb  - Client callback function to receive call control
                     notification events.

    user_info_ptr  - Client context value, provided with call control
                     notification events.

  The caller is returned a client ID, which must be checked to determine
  the status of registration.

DEPENDENCIES
  UCSD client support must have been previouly initialized.
  
RETURN VALUE
  Valid client ID on successful registration, DSUCSD_INVALID_CLIENT_ID
  on registration failure.
    
SIDE EFFECTS
   None

===========================================================================*/
/* ARGSUSED */
ds_ucsd_client_id_type ds_ucsd_api_register_client
(
  uint8   call_types,                             /* Bitmap of call types  */
  ds_ucsd_call_event_cb_type call_event_cb,       /* Event callback        */
  void  *user_info_ptr                            /* Client context value  */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  ds_ucsd_client_id_type client_id = DSUCSD_INVALID_CLIENT_ID;
  ds_ucsd_client_type   *client_ptr = NULL;
  boolean status = FALSE;

  /* Wait until UCSD client support has been initialized */
  if( !ds_ucsd_app_client_init_done )
  {
    DATA_MSG0_FATAL("Registration request before UCSD client support init");
    return DSUCSD_INVALID_CLIENT_ID;
  }
  
  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  /* Check for valid call_types bitmask */
  if( VALIDATE_CALL_TYPE(call_types, DSUCSD_API_CALL_TYPES_SUBSCRIBE_MASK) )
  {
    DATA_MSG2_ERROR("Invalid call types bitmask specified: %02X   permitted: %02X",
               call_types, DSUCSD_API_CALL_TYPES_SUBSCRIBE_MASK);
    return client_id;
  }

  /* Check for valid event callback */
  if( NULL == call_event_cb )
  {
    DATA_MSG0_ERROR( "NULL call event callback specified");
    return client_id;
  }

  /*-------------------------------------------------------------
    Register client with UCSD.
  --------------------------------------------------------------*/
  status = ds_ucsd_app_allocate_client( &client_ptr );
  if( FALSE == status )
  {
    DATA_MSG0_ERROR( "ds_ucsd_api_register_client returning error status");
  }
  else
  {
    /* Retrieve the client identifier for caller */
    if( NULL != client_ptr )
    {
      client_id = client_ptr->client_id;

      /* Preserve client specified parameters */
      client_ptr->info.call_types = call_types;
      client_ptr->info.call_event_cb = call_event_cb;
      client_ptr->info.user_info_ptr = user_info_ptr;

      MSG_MED("Registered UCSD client[%d]:  types=0x%02X cb=0x%p",
              client_id, call_types, call_event_cb );
    }
    else
    {
      DATA_MSG0_ERROR("Unexpected NULL client record");
    }
  }
  
  return client_id;
#else
  return DSUCSD_INVALID_CLIENT_ID ;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
} /* ds_ucsd_api_register_client() */



/*===========================================================================

FUNCTION DS_UCSD_API_DEREGISTER_CLIENT

DESCRIPTION
  This function is used to deregister the application client from the UCSD
  subtask. It should be called by the application task at shutdown.

  Parameters:
    client_id    - Client identifier returned on API registration.

DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  TRUE on sucessful operation, FALSE otherwise.
    
SIDE EFFECTS
   None

===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_api_deregister_client
(
  ds_ucsd_client_id_type client_id                   /* Client identifier  */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  boolean status = FALSE;

  DATA_MSG1_LOW("Invoked ds_ucsd_api_deregister_client(%d)",client_id);
  
  status = ds_ucsd_app_release_client( client_id );
  if( FALSE == status )
  {
    DATA_MSG0_ERROR("ds_ucsd_api_deregister_client returning error status");
  }
  else
  {
    DATA_MSG1_MED("Deregistered UCSD client[%d]",client_id);
  }
  
  return status;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
} /* ds_ucsd_api_deregister_client() */



/*===========================================================================

FUNCTION DS_UCSD_API_ORIGINATE_CALL

DESCRIPTION
  This function is used to originate a CS Data call. It sends the 
  DS_UCSD_APP_ORIG_CALL_CMD command to the UCSD subtask. It 
  should be called by a CS Data application. 
       
  Parameters:
    client_id    - Client identifier returned on API registration.

    call_mode    - Specify single or dual mode call.  Applicable only
                   for builds supporting multimedia fallback (SCUDIF).

    speed        - Fixed network data rate, per +CBST AT command

    name         - (A)synchronous name flag, per +CBST AT command

    connection_element - Connection element, per +CBST AT command

    waiur        - Wanted air interface user rate, per +CHSN AT command

    dial_string  - NULL terminated dial string, without dialing modifiers

    dial_string_len - Length of dial string

    modifiers    - Dialing modifiers

    uus_info     - User-to-user signaling, for VT call 2stage dialing
    
DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  CS Data call instance ID on success, DSUCSD_INVALID_INST_ID on failure.
    
SIDE EFFECTS
  May allocate an entry in the CS Data call table. 

===========================================================================*/
/* ARGSUSED */
uint8 ds_ucsd_api_originate_call
( 
  ds_ucsd_client_id_type client_id,       /* Client identifier             */
  uint8                call_mode,         /* Single/dual mode flag         */
  uint8                speed,             /* Network data rate             */
  uint8                name,              /* Synchronous or Asynchronous   */
  uint8                connection_element,/* Transparent or Non-Transparent*/
  uint8                waiur,             /* Wanted air interface rate     */
  const uint8         *dial_string,       /* Dialing string                */
  uint8                dial_string_len,   /* Dial string length            */
  ds_ucsd_dial_modifier_info_type *modifiers,/* SupSrv dial modifiers      */
  ds_ucsd_uus_info_type *uus_info         /* User to User IE               */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  ds_cmd_type                     *cmd_ptr = NULL;  /* DS command pointer  */
  ds_ucsd_app_orig_call_cmd_type  *data_ptr = NULL;
  uint8             inst_id;              /* CSData call instance          */
/*-------------------------------------------------------------------------*/
   
  DATA_MSG1_LOW("Invoked ds_ucsd_api_originate_call(%d)",client_id);
      
  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR( "Invalid client identifier specified: %d",client_id);
    return DSUCSD_INVALID_INST_ID;
  }
  if( VALIDATE_CALL_MODE( call_mode ) )
  {
    DATA_MSG1_ERROR("Invalid call mode specified: %d",call_mode);
    return DSUCSD_INVALID_INST_ID;
  }
  if( TRUE !=
      ds_ucsd_validate_cbst_values( speed, name, connection_element ) )
  {
    MSG_ERROR ("Unsupported call configuraiton specified: %d %d %d",
               speed, name, connection_element);
    return DSUCSD_INVALID_INST_ID;
  }
  if( TRUE !=
      ds_ucsd_validate_chsn_values( connection_element, waiur ) )
  {
    DATA_MSG1_ERROR("Unsupported WAIUR for current mode specified: %d",
               waiur);
    return DSUCSD_INVALID_INST_ID;
  }
  if( NULL == dial_string )
  {
   DATA_MSG0_ERROR("Unexpected NULL dialing string param");
    return DSUCSD_INVALID_INST_ID;
  }
  if( NULL == modifiers )
  {
    DATA_MSG0_ERROR( "Unexpected NULL dialing modifiers param");
    return DSUCSD_INVALID_INST_ID;
  }
  if( NULL == uus_info )
  {
    DATA_MSG0_ERROR( "Unexpected NULL user2user signalling param");
    return DSUCSD_INVALID_INST_ID;
  }
  if( MAX_DIAL_STRING_CHARS < dial_string_len )
  {
    DATA_MSG2_ERROR("Dialing string exceeds max length: %d<%d",
              MAX_DIAL_STRING_CHARS,dial_string_len);
    return DSUCSD_INVALID_INST_ID;
  }

  /*-------------------------------------------------------------------------
    Allocate CS Data call instance ID.
  -------------------------------------------------------------------------*/
  inst_id = ds_ucsd_get_instid( NULL );

  /*-------------------------------------------------------------------------
    Send the DS_UCSD_APP_ORIG_CALL_CMD command to the UCSD subtask.
  -------------------------------------------------------------------------*/
  if ( inst_id != DSUCSD_INVALID_INST_ID )
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_ucsd_app_orig_call_cmd_type));
    if ( (cmd_ptr != NULL) && (cmd_ptr->cmd_payload_ptr != NULL) )
    {
      uint16 len = MIN(dial_string_len, (sizeof(ds_ucsd_app_dial_string)-1) );

      DATA_MSG0_HIGH( "UCSD Application originates a call");

      /* Preserve passed dial string */
     memscpy( (char*)ds_ucsd_app_dial_string,
              sizeof(ds_ucsd_app_dial_string),
              (const char*)dial_string,
              dial_string_len );
      ds_ucsd_app_dial_string[len] = 0;
      
      /*---------------------------------------------------------------------
        If present, preserve the passed H.323 address used in VT 2 stage 
        dialing (UUS1).
      ---------------------------------------------------------------------*/
      if(uus_info->present == TRUE)
      {
        /*-------------------------------------------------------------------
          Note, we use < and not <= for MAX_USER_USER_DATA_1_LENGTH since we 
          are using User-user specific protocol discriminator, and are using
          the first octet of the user data to specifiy the H.323 address 
          type. e.g. email, url, h.322.
        -------------------------------------------------------------------*/
        if( uus_info->data_len < MAX_USER_USER_DATA_1_LENGTH &&
            uus_info->data_len != 0)
        {
          /*-----------------------------------------------------------------
            Leave the first octet empty, since we'll set it later for what
            the H.323 addr id type is. See ds_csd_init_orig_uus_info().
          -----------------------------------------------------------------*/
          (void)memscpy( (void*)&ds_ucsd_uus1_addr[1],
                         (sizeof(ds_ucsd_uus1_addr)-1),
                        (const void*)uus_info->uus_data,
                        (sizeof(ds_ucsd_uus1_addr)-1) );
          uus_info->uus_data = ds_ucsd_uus1_addr;
          
          /*-----------------------------------------------------------------
            Since we're adding an octet for the address id, add one to the
            lenght of the user data.
            e.g uus_data = "joe@e.com"
                uus_data_len = 9 (don't need to have a NULL)
            However, it will be sent as "1joe@x.com" for email address type
            so uus_info.data_len needs to have one added to it.
          -----------------------------------------------------------------*/
          uus_info->data_len++;
        }
        else
        {
           uus_info->present = FALSE;
           ds_ucsd_uus1_addr[0] = 0;
           DATA_MSG2_HIGH( "UUS1 invalid payload size, 0<(size=%d)<=%d", 
                     uus_info->data_len, MAX_USER_USER_DATA_1_LENGTH-1);
        }
      } /* UUS present */

      data_ptr = (ds_ucsd_app_orig_call_cmd_type*)cmd_ptr->cmd_payload_ptr;
      memset(data_ptr, 0x0, sizeof(ds_ucsd_app_orig_call_cmd_type));

      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_UCSD_APP_ORIG_CALL;

      data_ptr->client_id     = client_id;
      data_ptr->inst_id       = inst_id;
      data_ptr->call_mode     = call_mode;
      data_ptr->speed         = speed;
      data_ptr->name          = name;
      data_ptr->ce            = connection_element;
      data_ptr->waiur         = waiur;
      data_ptr->dial_string   = ds_ucsd_app_dial_string;
      data_ptr->modifiers     = *modifiers;
      data_ptr->uus_info      = *uus_info;

      ds_put_cmd( cmd_ptr );
    }
    else
    {
      ASSERT(0);
      return DSUCSD_INVALID_INST_ID;
    }
  }
  else
  {
   DATA_MSG0_ERROR( "Cannot originate call: no call instance");
  }

  return ( inst_id );
#else
  return DSUCSD_INVALID_INST_ID;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
} /* ds_ucsd_api_originate_call() */



/*===========================================================================

FUNCTION DS_UCSD_API_CONFIRM_CALL

DESCRIPTION
   This function is used to confirm an incoming CS Data call. It sends the
   DS_UCSD_APP_CONFIRM_CALL_CMD command to the UCSD subtask.  It
   should be called by a CS Data application.

  Parameters:
    client_id    - Client identifier returned on API registration.

    inst_id      - Allocated CS Data call instance ID.
    
    call_type    - Specify preferred mode of a dual mode call. Applicable only
                   for builds supporting multimedia fallback (SCUDIF).
    
    result_params_ptr - Indicate if setup is rejected and cause value.
        
DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  TRUE on sucessful operation, FALSE otherwise.
    
SIDE EFFECTS
  If call_type changed relative to setup event, preferred bearer order will
  be changed in network signaling.

===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_api_confirm_call
( 
  ds_ucsd_client_id_type     client_id,        /* Client identifier        */
  uint8                      inst_id,          /* CSData call instance     */
  ds_ucsd_call_type          call_type,        /* Type of call             */
  ds_ucsd_answer_param_type *result_params_ptr /* Result parameter info    */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  ds_cmd_type                        *cmd_ptr = NULL; /* DS command pointer*/
  ds_ucsd_app_confirm_call_cmd_type  *data_ptr = NULL;

  DATA_MSG1_LOW("Invoked ds_ucsd_api_confirm_call(%d)",client_id);
      
  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR("Invalid client identifier specified: %d",client_id);
    return FALSE;
  }
  if( VALIDATE_INST_ID( inst_id ) )
  {
    DATA_MSG1_ERROR("Invalid call instance identifier specified: %d",inst_id);
    return FALSE;
  }
  if( VALIDATE_CALL_TYPE(call_type, DSUCSD_API_CALL_TYPES_SCUDIF_MASK) )
  {
    DATA_MSG2_ERROR("Invalid call type bitmask specified: %02X   permitted: %02X",
               call_type, DSUCSD_API_CALL_TYPES_SCUDIF_MASK);
    return FALSE;
  }
  if( NULL == result_params_ptr )
  {
    DATA_MSG0_ERROR("Unexpected NULL result parameter specified");
    return FALSE;
  }

  
  /*-------------------------------------------------------------------------
    Send the DS_UCSD_APP_CONFIRM_CALL_CMD command to the UCSD subtask.
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_ucsd_app_confirm_call_cmd_type));
  if ( (cmd_ptr != NULL) && (cmd_ptr->cmd_payload_ptr != NULL) )
  {
    DATA_MSG0_HIGH( "UCSD Application confirms a call" );

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_UCSD_APP_CONFIRM_CALL;
    data_ptr = (ds_ucsd_app_confirm_call_cmd_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->client_id      = client_id;
    data_ptr->inst_id        = inst_id;
    data_ptr->call_type      = call_type;
    data_ptr->result_params  = *result_params_ptr;
    ds_put_cmd( cmd_ptr );
  }
  else
  {
    ASSERT(0);
    return FALSE;
  }

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
} /* ds_ucsd_api_confirm_call() */



/*===========================================================================

FUNCTION DS_UCSD_API_ANSWER_CALL

DESCRIPTION
  This function is used to answer a CS Data call. It sends the 
  DS_UCSD_APP_ANSWER_CALL_CMD command to the UCSD subtask. 
  It should be called by a CS Data application.

  Parameters:
    client_id    - Client identifier returned on API registration.

    inst_id      - Allocated CS Data call instance ID.
    
    answer_params_ptr - Indicate if call answer is rejected and cause value.

DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  TRUE on sucessful operation, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_api_answer_call
( 
  ds_ucsd_client_id_type     client_id,        /* Client identifier        */
  uint8                      inst_id,          /* CSData call instance     */  
  ds_ucsd_answer_param_type *answer_params_ptr /* Answer parameter info    */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  ds_cmd_type                       *cmd_ptr = NULL; /* DS command pointer */
  ds_ucsd_app_answer_call_cmd_type  *data_ptr = NULL;

/*-------------------------------------------------------------------------*/
   
  DATA_MSG1_LOW("Invoked ds_ucsd_api_answer_call(%d)",client_id);

  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR("Invalid client identifier specified: %d",client_id);
    return FALSE;
  }
  if( VALIDATE_INST_ID( inst_id ) )
  {
    DATA_MSG1_ERROR( "Invalid call instance identifier specified: %d",inst_id);
    return FALSE;
  }
  if( NULL == answer_params_ptr )
  {
    DATA_MSG0_ERROR( "Unexpected NULL answer parameter specified");
    return FALSE;
  }
   

  /*-------------------------------------------------------------------------
    Send the DS_UCSD_APP_ANSWER_CALL_CMD command to the UCSD subtask.
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_ucsd_app_answer_call_cmd_type ));
  if ( (cmd_ptr != NULL) && (cmd_ptr->cmd_payload_ptr != NULL) )
  {
    DATA_MSG0_HIGH( "UCSD Application answers a call");

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_UCSD_APP_ANSWER_CALL;
    data_ptr = (ds_ucsd_app_answer_call_cmd_type*)cmd_ptr->cmd_payload_ptr;

    data_ptr->client_id     = client_id;
    data_ptr->inst_id       = inst_id;
    data_ptr->answer_params = *answer_params_ptr;

    ds_put_cmd( cmd_ptr );
  }
  else
  {
    ASSERT(0)
    return FALSE;;
  }

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
} /* ds_ucsd_api_answer_call() */

/*===========================================================================

FUNCTION DS_UCSD_API_END_CALL

DESCRIPTION
  This function is used to end a CS Data call. It sends the 
  DS_UCSD_APP_END_CALL_CMD command to the UCSD subtask. 
  It should be called by a CS Data application.
        
  Parameters:
    client_id    - Client identifier returned on API registration.

    inst_id      - Allocated CS Data call instance ID.

DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  TRUE on sucessful operation, FALSE otherwise.
    
SIDE EFFECTS
   None

===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_api_end_call
( 
  ds_ucsd_client_id_type     client_id,        /* Client identifier        */
  uint8                      inst_id           /* CSData call instance     */  
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  ds_cmd_type                    *cmd_ptr = NULL;  /* DS command pointer   */
  ds_ucsd_app_end_call_cmd_type  *data_ptr = NULL;

/*-------------------------------------------------------------------------*/
   
  DATA_MSG1_LOW( "Invoked ds_ucsd_api_end_call(%d)",client_id);

  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR( "Invalid client identifier specified: %d",client_id);
    return FALSE;
  }
  if( VALIDATE_INST_ID( inst_id ) )
  {
    DATA_MSG1_ERROR( "Invalid call instance identifier specified: %d",inst_id);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Send the DS_UCSD_APP_END_CALL_CMD command to the UCSD subtask.
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_ucsd_app_end_call_cmd_type));
  if ( (cmd_ptr != NULL) && (cmd_ptr->cmd_payload_ptr != NULL ) )
  {
    DATA_MSG0_HIGH( "UCSD Application ends a call");

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_UCSD_APP_END_CALL;
    data_ptr = (ds_ucsd_app_end_call_cmd_type*)cmd_ptr->cmd_payload_ptr;

    data_ptr->client_id = client_id;
    data_ptr->inst_id = inst_id;

    ds_put_cmd( cmd_ptr );
  }
  else
  {
    ASSERT(0)
    return FALSE;
  }

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

} /* ds_ucsd_api_end_call() */



/*===========================================================================

FUNCTION DS_UCSD_API_MODIFY_CALL

DESCRIPTION
  This function is used to modify a CS Data call to a new call type.
  It sends the DS_UCSD_APP_MODIFY_CALL_CMD command to the UCSD
  subtask.  It should be called by a CS Data application.  Applicable
  only for builds supporting multimedia fallback (SCUDIF).

  Parameters:
    client_id    - Client identifier returned on API registration.

    inst_id      - Allocated CS Data call instance ID.

    msg_type     - Specify command or response.

    new_call_type - Indicate new call type for originating modify request.

    accept       - Indicate accept/reject for incoming modify request.
    
DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  TRUE on sucessful operation, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_api_modify_call
( 
  ds_ucsd_client_id_type     client_id,        /* Client identifier        */
  uint8                      inst_id,          /* CSData call instance     */  
  ds_ucsd_msg_type           msg_type,         /* Command or Response      */
  ds_ucsd_call_type          new_call_type,    /* New type for call        */
  boolean                    accept            /* Respose indication       */
)
{

#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
} /* ds_ucsd_api_modify_call() */


/*===========================================================================

FUNCTION DS_UCSD_API_LOOKUP_CM_CALLID

DESCRIPTION
  This function performs lookup in the CS Data call table to find the
  Call Manager call ID which corresponds to the CS Data call instance
  ID.
        
  Parameters:
    inst_id      - Allocated CS Data call instance ID.

DEPENDENCIES
  None
  
RETURN VALUE
  Mapped CM call ID on success, CM_CALL_ID_INVALID on failure.
    
SIDE EFFECTS
  None   
===========================================================================*/
/* ARGSUSED */
uint8 ds_ucsd_api_lookup_cm_callid
(
  const uint8 inst_id                              /* CSData call instance */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  DATA_MSG1_LOW("Invoked ds_ucsd_api_lookup_cm_callid(%d)",inst_id);
  
  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  if( VALIDATE_INST_ID( inst_id ) )
  {
    DATA_MSG1_ERROR("Invalid call instance identifier specified: %d",inst_id);
    return CM_CALL_ID_INVALID;
  }
  
  /* Invoke the base function */
  return ds_ucsd_lookup_cm_callid( inst_id );
#else
  return CM_CALL_ID_INVALID;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

} /* ds_ucsd_api_lookup_cm_callid() */
  


/*===========================================================================

FUNCTION DS_UCSD_API_SET_RLP

DESCRIPTION
  This function changes the Radio Link Protocol (RLP) parameters for
  the Modem processor.  Changes are system-wide and affect all future
  non-transparent CS Data calls.
        
  Parameters:
    client_id    - Client identifier returned on API registration.

    rlp_params_ptr - Pointer to RLP and data compression parameters.
    
DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  TRUE on sucessful operation, FALSE otherwise.
    
SIDE EFFECTS
  None   
===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_api_set_rlp
(
  ds_ucsd_client_id_type      client_id,       /* Client identifier        */
  const ds_ucsd_nt_info_type *rlp_params_ptr   /* RLP parameters pointer   */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  boolean result = TRUE;
  
  DATA_MSG1_LOW( "Invoked ds_ucsd_api_set_rlp(%d)",client_id);
  
  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR( "Invalid client identifier specified: %d",client_id);
    return FALSE;
  }
  if( NULL == rlp_params_ptr )
  {
    DATA_MSG0_ERROR( "Unexpected NULL RLP parameter specified");
    return FALSE;
  }

  /* Send RLP parameters to ATCOP for update */
  if( DSAT_OK != dsat_change_rlp_params( rlp_params_ptr ) )
  {
    DATA_MSG0_ERROR( "Failed to update RLP parameter set");
    result = FALSE;
  }

  return result;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
} /* ds_ucsd_api_set_rlp() */



/*===========================================================================

FUNCTION DS_UCSD_API_GET_RLP

DESCRIPTION
  This function queries the Radio Link Protocol (RLP) parameters from
  the Modem processor.  Parameters apply to non-transparent CS Data calls.
        
  Parameters:
    client_id    - Client identifier returned on API registration.

    rlp_params_ptr - Pointer to RLP and data compression parameters.
    
DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  TRUE on sucessful operation, FALSE otherwise.
    
SIDE EFFECTS
  None   
===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_api_get_rlp
(
  ds_ucsd_client_id_type     client_id,        /* Client identifier        */
  ds_ucsd_rlp_sets_type     *rlp_params_ptr    /* RLP parameters pointer   */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  boolean result = TRUE;
  
  DATA_MSG1_LOW( "Invoked ds_ucsd_api_get_rlp(%d)",client_id);
  
  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR( "Invalid client identifier specified: %d",client_id);
    return FALSE;
  }
  if( NULL == rlp_params_ptr )
  {
    DATA_MSG0_ERROR( "Unexpected NULL RLP parameter specified");
    return FALSE;
  }

  /* Request RLP parameters from ATCOP */
  if( DSAT_OK != dsat_get_rlp_params( rlp_params_ptr ) )
  {
    DATA_MSG0_ERROR( "Failed to get RLP parameter sets");
    result = FALSE;
  }

  return result;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

} /* ds_ucsd_api_get_rlp() */



/*===========================================================================

FUNCTION DS_UCSD_API_GET_CALLSTATS

DESCRIPTION

  This function queries the data traffic statistics for the specified
  CS Data call.  Statistics are tracked only for those calls using SIO
  datapath.  If call is inactive, the last statistics are returned. 
        
  Parameters:
    client_id    - Client identifier returned on API registration.

    inst_id      - Allocated CS Data call instance ID.
    
    callstats_ptr - Pointer to call statistics information.
    
DEPENDENCIES
  ds_ucsd_api_register_client() must have already been called.
  
RETURN VALUE
  TRUE on sucessful operation, FALSE otherwise.
    
SIDE EFFECTS
  None   
===========================================================================*/
/* ARGSUSED */
boolean ds_ucsd_api_get_callstats
(
  ds_ucsd_client_id_type     client_id,        /* Client identifier        */
  uint8                      inst_id,          /* CSData call instance     */  
  ds_ucsd_call_stats_type   *callstats_ptr     /* Call statistics pointer  */
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  ds_ucsd_last_call_info_type ds_ucsd_last_call_info;
  boolean result = TRUE;
  uint32 rx_data_counter=0, tx_data_counter=0;

  DATA_MSG1_LOW( "Invoked ds_ucsd_api_get_callstats(%d)",client_id);
  
  memscpy(&ds_ucsd_last_call_info, sizeof(ds_ucsd_last_call_info_type),
          ds_ucsd_get_last_call_info(), sizeof(ds_ucsd_last_call_info_type));
  /*-------------------------------------------------------------
    Validate interface parameters
  --------------------------------------------------------------*/
  if( VALIDATE_CLIENT_ID( client_id ) )
  {
    DATA_MSG1_ERROR( "Invalid client identifier specified: %d",client_id);
    return FALSE;
  }
  if( VALIDATE_INST_ID( inst_id ) )
  {
    DATA_MSG1_ERROR( "Invalid call instance identifier specified: %d",inst_id);
    return FALSE;
  }
  if( NULL == callstats_ptr )
  {
    DATA_MSG0_ERROR( "Unexpected NULL call stats parameter specified");
    return FALSE;
  }

  /* Query for active call stats */
  if( TRUE == ds_ucsd_get_data_counters( inst_id,
                                         &rx_data_counter,
                                         &tx_data_counter ))
  {
    callstats_ptr->tx_data_counter = tx_data_counter;
    callstats_ptr->rx_data_counter = rx_data_counter;
    callstats_ptr->call_active = TRUE;
    callstats_ptr->present = TRUE;
  }
  else
  {
    /* Check last connected call information */
    if( (TRUE == ds_ucsd_last_call_info.present) &&
        (inst_id == ds_ucsd_last_call_info.inst_id) )
    {
      /* Matched instance ID, return call stats */
      callstats_ptr->tx_data_counter = ds_ucsd_last_call_info.tx_data_counter;
      callstats_ptr->rx_data_counter = ds_ucsd_last_call_info.rx_data_counter;
      callstats_ptr->call_active = FALSE;
      callstats_ptr->present = TRUE;
      MSG_MED("ds_ucsd_last_call_info: tx=%d rx=%d active=%d",
              callstats_ptr->tx_data_counter,
              callstats_ptr->rx_data_counter,
              callstats_ptr->call_active );
    }
    else
    {
      callstats_ptr->present = FALSE;
      result = FALSE;
    }
  }

  return result;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
} /* ds_ucsd_api_get_callstats() */

/*===========================================================================

FUNCTION DS_UCSD_API_GET_CALLTYPE

DESCRIPTION

  This function queries the UCSD stack call type for the specified 
  Call Manager callID. If the call instance mapped to the callID is 
  present, the UCSD call type is returned. If call instance cannot be 
  found, the call type will be DS_UCSD_UNKNOWN_CALL and return code 
  will be FALSE.
           
DEPENDENCIES 
   None 
  
RETURN VALUE 
   TRUE on successful operation, FALSE otherwise. 
    
SIDE EFFECTS 
   None
===========================================================================*/ 
/* ARGSUSED */
boolean ds_ucsd_api_get_calltype
( 
  const uint8    cm_call_id,             /* Call Manager call ID */ 
  uint8*         call_type               /* UCSD call type */ 
)
{
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
  uint8 index = 0;
  ds_ucsd_call_tbl_entry_type *call_info_ptr= NULL;

  DATA_MSG1_LOW( "Invoked ds_ucsd_api_get_calltype with cm_call_id(%d)", cm_call_id);

  if( NULL == call_type )
  {
    DATA_MSG0_ERROR( "Unexpected NULL call type parameter specified");
    return FALSE;
  }
  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID(cm_call_id ))) )
  {
    DATA_MSG1_ERROR( "Cannot find call instance for CM call ID: %d",
               cm_call_id);
    *call_type = (uint8)DS_UCSD_UNKNOWN_CALL;
    return FALSE;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );

  /* The call type for dual mode(SCUDIF) will be updated by adding VOICE 
   * fallback if requested. So it has to be cleared before reporting the 
   * data call type.
   */
  *call_type = (uint8)((uint8)call_info_ptr->call_type & (~(uint8)DS_UCSD_VOICE_CALL));

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */
}/* ds_ucsd_api_get_calltype */

