#ifndef NPA_REMOTE_PROTOCOL_H
#define NPA_REMOTE_PROTOCOL_H

/*==============================================================================
  @file npa_remote_protocol.h

  NPA Remote Protocol API 

  This is not intended for normal use.  Users should generally be
  defining local resources using npa_remote_resource.h and those
  functions will use the protocol.

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "npa_remote.h"
#include "npa_resource.h"
/** \defgroup protocol NPA Remote Protocol
 *
 * This file contains the NPA remote protocol external data 
 * structures and APIs. 
 *
 */
/*@{*/


/* Internal protocol state */
typedef enum
{
  NPA_REMOTE_PROTOCOL_UNINITIALIZED,
  NPA_REMOTE_PROTOCOL_INITIALIZING,
  NPA_REMOTE_PROTOCOL_INITIALIZED
} npa_remote_protocol_state;

enum
{
  NPA_QUERY_REMOTE_PROTOCOL = NPA_QUERY_RESERVED_END+1
};

typedef struct npa_remote_protocol *npa_remote_protocol_handle;

/* Vtable for remote protocol */
typedef struct npa_remote_protocol_vtable
{
/**

   @brief remote_init - Initialize the remote interface
 
   Request initialization of the remote protocol at a particular
   version number.  The remote protocol will invoke the callback when
   the protocol is initialized and avaialble for use.  

   The remote protocol should not be used before the callback is
   received.  Behavior will be undefined in this case.

   If the remote protocol does not support the requested version,
   NPA_REMOTE_FAILURE will be returned. In this case, the callback
   will never be invoked.

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param major : major protocol number requested
   @param minor : minor protocol number requested
   @param build : build version number requested
   @param callback : callback to invoke when remote protocol is fully 
   initialized
   @param context : context argument to be passed back to callback 
   when callback is invoked
 
   @return npa_remote_status

**/
  npa_remote_status
  (*remote_init)( npa_remote_protocol_handle  protocol,
                  uint32_t                    major,
                  uint32_t                    minor,
                  uint32_t                    build,
                  npa_remote_callback_type    callback,
                  void                       *context );

/*----------------------------------------------------------------------------
 * NPA Client API
 * -------------------------------------------------------------------------*/


/**

   @brief create_sync_client - Creates a synchronous client of a
   resource

   Part of NPA Client API

   Create a synchronous client of the given resource with the given
   work model.  The new client will not be created with have an
   initial request. Work requests made via syncronous clients will be
   fully processed before returning.

   If the named resource does not exist or it exists but doesn't
   support the requested client_type, a NULL handle will be
   returned. Callers are expected to check for NULL handles.

   A client must never attempt to use a NULL handle to update a
   resource.

   The client_name must be < 64 characters in length.

   The client_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param resource_name : name of the resource that is to be used
   @param client_name : name of the client that's being created
   @param client_type : the work model that this client will use for
   work requests
   @param handle : ptr to return new client handle, NULL if client
   could not be created
 
   @return npa_remote_status

*/
  npa_remote_status
  (*create_sync_client)( npa_remote_protocol_handle  protocol,
                         const char                 *resource_name,
                         const char                 *client_name,   
                         npa_remote_client_type      client_type,
                         npa_remote_client_handle   *handle );   

/**
   @brief create_async_client - Creates an asynchronous client of
   a resource

   Part of NPA Client API

   Create an asynchronous client of the given resource with the given
   work model.  Requests made via an asynchronous client will be
   processed asynchronously. The new client will not be created with an
   initial request.

   If the named resource does not exist or it exists but doesn't
   support the requested client_type, a NULL handle will be
   returned. Callers are expected to check for NULL handles.
   
   A client must never attempt to use a NULL handle to update a
   resource.

   The client_name must be < 64 characters in length.

   The client_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   If non-NULL, the callback will be triggered when the asynchronous
   request has completed. The arguments will be (user_arg, 0,
   client_handle, sizeof(client_handle) );

   Restrictions: 

   In general, the same client can not have multiple simultaneous
   requests issued.  It is the responsibility of the user to make sure
   the previous request has completed before issuing a new request via
   the same client.
   
   It is hoped that both these restrictions will be removed in future
   versions.
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param resource_name : name of the resource that is to be used
   @param client_name : name of the client that's being created 
   @param client_type : the work model that this client will use for
   work requests
   @param callback : callabck to be invoked when work request is complete
   @param context : context to be passed back in callback invocation
   @param handle : ptr to return new client handle, NULL if client
   could not be created

   @return npa_remote_status

*/
  npa_remote_status
  (*create_async_client)( npa_remote_protocol_handle  protocol,
                          const char                 *resource_name,
                          const char                 *client_name,
                          npa_remote_client_type      client_type,
                          npa_remote_callback_type    callback,
                          void                       *context, 
                          npa_remote_client_handle   *handle );


/**

   @brief destroy_client - Destroy the client.  

   Part of NPA Client API

   This will cancel the client request (if any) and free the client
   structure. The client handle can not be used to issue requests after
   being destroyed
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param client : handle to client being destroyed
    
   @return npa_remote_status 

*/
  npa_remote_status
  (*destroy_client)( npa_remote_protocol_handle protocol,
                     npa_remote_client_handle   client );


/**
   @brief issue_required_request - Issue the REQUIRED client work
   request

   Part of NPA Client API

   Resource state should be updated to account for new request. 
   
   The client must be of REQUIRED work model
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param client : handle to client being used
   @param state : requested resource state 
   @param new_state : ptr to return new resource state  
    
   @return npa_remote_status 

*/
  npa_remote_status
  (*issue_required_request)( npa_remote_protocol_handle protocol,
                             npa_remote_client_handle   client,
                             npa_remote_resource_state  state,
                             npa_remote_resource_state *new_state );


/**
   @brief issue_vector_request - Issue the VECTOR client work
   request

   Part of NPA Client API

   Resource state should be updated to account for new request. 
   
   The client must be of VECTOR work model
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param client : handle to client being used
   @param num_elems : number of elements in the vector
   @param vector : array of num_elems 
   @param new_state : ptr to return new resource state  ???
    
   @return npa_remote_status  

*/
  npa_remote_status
  (*issue_vector_request)( npa_remote_protocol_handle  protocol,
                           npa_remote_client_handle    client_handle, 
                           uint32_t                    num_elems,
                           npa_remote_resource_state  *vector,
                           npa_remote_resource_state  *new_vector);

  
/**
   @brief issue_impulse_request - Issue an IMPULSE work request 

   Part of NPA Client API

   The client must be of IMPULSE work model

   The resource defines the impulse response

   Resource state should be updated to account for new request
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param client : handle to client being used 
   @param new_state : ptr to return new resource state  
    
   @return npa_remote_status  

*/
  npa_remote_status
  (*issue_impulse_request)( npa_remote_protocol_handle protocol,
                            npa_remote_client_handle   client,
                            npa_remote_resource_state *new_state );


/**
   @brief issue_isoc_request -  Issue an ISOCHRONOUS work request 

   Part of NPA Client API

   The client must be of ISOCHRONOUS work model

   The client must issue complete_request() when the work is completed
   
   Resource state should be updated to accound for new work
   Note: The hint may be overriden/ignored by the resource
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param client : handle to client being used
   @param deadline : the interval (in ms) when the work must be completed
   @param level_hint : a hint as to what an appropriate level might
   be. May be overridden by resource 
   @param new_state : ptr to return new resource state  
    
   @return npa_remote_status  

*/
  npa_remote_status
  (*issue_isoc_request)( npa_remote_protocol_handle protocol,
                         npa_remote_client_handle   client,
                         npa_remote_resource_time   deadline,
                         npa_remote_resource_state  level_hint,
                         npa_remote_resource_state *new_state );

/**
   @brief issue_limit_max_request - Issue the LIMIT_MAX  request

   Part of NPA Client API

   If there are multiple concurrent max requests, the minimum of the
   requests will be used.

   The client must be of LIMIT_MAX work model.

   Note: The behavior of concurrent LIMIT_MAX and FIXED request or
   LIMIT_MAX < LIMIT_MIN requests is undefined
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param client : handle to client being used
   @param max : maximum state of resource 
   @param new_state : ptr to return new resource state  
    
   @return npa_remote_status  

*/
  npa_remote_status
  (*issue_limit_max_request)( npa_remote_protocol_handle protocol,
                              npa_remote_client_handle   client,
                              npa_remote_resource_state  max,
                              npa_remote_resource_state *new_state );


/**
   @brief complete_request - Complete the work request

   Part of NPA Client API

   This is used to notify the resource that the work request is
   complete and the client no longer needs the resource

   Any request state that has been learned will persist
   
   Resource state should be updated to account for completed work
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param client : handle to client being used 
   @param new_state : ptr to return new resource state  
    
   @return npa_remote_status  

*/
  npa_remote_status
  (*complete_request)( npa_remote_protocol_handle protocol,
                       npa_remote_client_handle   client,
                       npa_remote_resource_state *new_state );

/**
   @brief cancel_request - Cancel the work request

   Part of NPA Client API

   This can be used by any work model to terminate the work request.
   This internally invokes npa_remote_complete_request and then clears any
   adapted state

   This function is used when future client requests will be
   qualitatively different than previous requests made via this client

   Resource state should be updated to account for terminated work
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param client : handle to client being used 
   @param new_state : ptr to return new resource state 
    
   @return npa_remote_status  

*/
  npa_remote_status
  (*cancel_request)( npa_remote_protocol_handle protocol,
                     npa_remote_client_handle   client,
                     npa_remote_resource_state *new_state );



/**
   @brief create_change_event - Create a change event handler with
   the given resource

   Part of NPA Event API

   This will return a NULL handle if the resource does not exist or
   the event handler could not be created.

   The event_name must be < 64 characters in length.

   The event_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   If the DAL Event is a callback, the arguments will be:
   ( user_data, event_type, npa_remote_event_data *, sizeof( npa_remote_event_data *) )
    
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param resource_name : name of the resource that is to be used
   @param event_name : name of the event handler that's being created 
   @param callback : callback to be invoked when event is triggered
   @param context : context to be passed back in callback invocation
   @param handle : ptr to return new event handle, NULL if the event handle 
   could not be created
    
   @return npa_remote_status 

*/
  npa_remote_status
  (*create_change_event)( npa_remote_protocol_handle protocol,
                          const char                *resource_name,
                          const char                *event_name,
                          npa_remote_callback_type   callback,
                          void                      *context,
                          npa_remote_event_handle   *handle );

/**
   @brief npa_remote_create_watermark_event- Create a watermark event handler
   with the given resource

   Part of NPA Event API

   The event handler will not be invoked until the watermarks have
   been set

   The event_name must be < 64 characters in length.

   The event_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   If the DAL Event is a callback, the arguments will be:
   ( user_data, event_type, npa_remote_event_data *, sizeof( npa_remote_event_data *) )
   
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param resource_name : name of the resource that is to be used
   @param event_name : name of the event handler that's being created 
   @param callback : callback to be invoked when event is triggered
   @param context : context to be passed back in callback invocation
   @param handle : ptr to return new event handle, NULL if the event handle 
   could not be created

   @return npa_remote_status 

*/
  npa_remote_status
  (*create_watermark_event)( npa_remote_protocol_handle protocol,
                             const char                *resource_name,
                             const char                *event_name,
                             npa_remote_callback_type   callback,
                             void                      *context,
                             npa_remote_event_handle   *handle ); 


/**
   @brief set_event_watermarks - Set the watermarks for triggering
   the event handler.

   Part of NPA Event API

   Watermarks are defined as the value relative to the resource maximum

   For example: If the caller wanted to be notified when the resource
   state rose past qthe hi watermark of (max-20) units or dropped below
   the low watermark of (max-40) units, the hi and lo watermarks would
   be set to -20 and -40, respectively
   
   No watermark event will be triggered until the watermarks have been
   set.

   The event may be triggered during this function if the resource is
   below the lo watermark or above the hi watermark when this function
   is invoked
   
   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param remote_event : handle of event being manipulated
   @param hi_watermark : hi watermark setting
   @param lo_watermark : lo watermark setting

   @return npa_remote_status 

*/
  npa_remote_status
  (*set_event_watermarks)( npa_remote_protocol_handle      protocol,
                           npa_remote_event_handle         remote_event,
                           npa_remote_resource_state_delta hi_watermark,
                           npa_remote_resource_state_delta lo_watermark );


/**
   @brief destroy_event_handle - Destroy the event handler

   Part of NPA Event API

   After invocation, the event handle is invalidated and must not be
   reused

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param remote_event : handle of event being destroyed

   @return npa_remote_status 

*/
  npa_remote_status
  (*destroy_event_handle)( npa_remote_protocol_handle protocol,
                           npa_remote_event_handle    remote_event );


/**
   @brief create_query_handle - Create a query handle to a resource 

   Part of NPA Query API

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param resource_name : name of resource being queried
   @param query_handle : ptr to return new query handle, NULL if the 
   query handle could not be created

   @return npa_remote_status

*/
  npa_remote_status 
  (*create_query_handle)( npa_remote_protocol_handle protocol,
                          const char                *resource_name, 
                          npa_remote_query_handle   *query_handle );

/**
   @brief destroy_query_handle - Destroy the query handle

   Part of NPA Query API

   Only query handles that have been explicitly created via
   npa_remote_create_query_handle need to be explicitly destroyed

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param remote_query : query handle to destroy

   @return npa_remote_status

*/
  npa_remote_status 
  (*destroy_query_handle)( npa_remote_protocol_handle protocol,
                           npa_remote_query_handle    remote_query );


/**
   @brief query - Query the current state of the resource

   Part of NPA Query API

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param remote_handle : query handle to resource being queried
   @param query_id : type of query being issued
   @param query_result : value from the query

   @return success/failure of query (npa_remote_query_status)

*/
  npa_remote_query_status 
  (*query)( npa_remote_protocol_handle protocol,
            npa_remote_query_handle    remote_handle,
            uint32_t                   query_id, 
            npa_remote_query_type     *query_result );


/** 
   @brief query_by_name - Query the current state of the named
   resource

   Part of NPA Query API

   This is a convenience function that wraps the

   h = create_query_handle(...);
   npa_remote_query(h, ...);
   npa_remote_destroy_query_handle(h);

   sequence. If you are making mulitple queries to the same resource,
   calling this function multiple times will have a poorer performance
   relative to managing the query handle directly.

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param resource_name : name of resource being queried
   @param query_id : type of query being issued
   @param query_result : value from the query

   @return success/failure of query (npa_remote_query_status)

*/
  npa_remote_query_status 
  (*query_by_name)( npa_remote_protocol_handle protocol,
                    const char                *resource_name, 
                    uint32_t                   query_id, 
                    npa_remote_query_type     *query_result ); 
/**
   @brief query_by_client - Query a resource via a client to that resource

   Part of NPA Query API

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param remote_handle : client of resource being queried
   @param query_id : type of query being issued
   @param query_result : value from the query

   @return success/failure of query (npa_remote_query_status)

*/
  npa_remote_query_status 
  (*query_by_client)( npa_remote_protocol_handle protocol,
                      npa_remote_client_handle   remote_handle,  
                      uint32_t                   query_id, 
                      npa_remote_query_type     *query_result ); 

/**
   @brief query_by_event - Query a resource via an event of that resource

   Part of NPA Query API

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param remote_handle : event of resource being queried
   @param query_id : type of query being issued
   @param query_result : value from the query

   @return success/failure of query (npa_remote_query_status)

*/
  npa_remote_query_status 
  (*query_by_event)( npa_remote_protocol_handle protocol,
                     npa_remote_event_handle    remote_handle, 
                     uint32_t                   query_id, 
                     npa_remote_query_type     *query_result );

/**
   
   @brief resource_available - Notify when resource is available
   to be used

   Part of NPA Query API

   Register an event to be triggered (once) when the given resource is
   available and ready for use.  The event may be fired during
   function execution if the resource is available when the function
   is invoked.  

   @param protocol : handle to the remote protocol, against which
   a request is being issued
   @param resource_name : name of the resource that's being querried 
   @param callback : callback to invoke when resource is available
   @param context : context to be passed back in callback invocation

   @return npa_remote_status

*/
  npa_remote_status 
  (*resource_available)( npa_remote_protocol_handle protocol,
                         const char               *resource_name, 
                         npa_remote_callback_type  callback,
                         void                     *context );
} npa_remote_protocol_vtable;

/* Remote Protocol Definition */
typedef struct npa_remote_protocol
{
  const char *name;
  struct
  { 
    uint32_t  major;
    uint32_t  minor;
    uint32_t  build;
  } version;

  npa_remote_protocol_state   state;        /* Default to uninitialized */
  
  void  *data;                  /* Extension data */
  
  npa_remote_protocol_vtable *vtable;    /* Vtable pointer */
} npa_remote_protocol; 


/**
   
   @brief npa_define_remote_protocol - Add a remote protocol to the graph

   Register a remote protocol 

   @param npa_remote_protocol : protocol structure to be registered
*/
void npa_define_remote_protocol( npa_remote_protocol *protocol );

/**@} /group protocol */
#endif /* NPA_REMOTE_PROTOCOL_H */
