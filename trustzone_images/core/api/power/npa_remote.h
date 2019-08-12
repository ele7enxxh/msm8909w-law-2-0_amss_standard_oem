#ifndef NPA_REMOTE_H
#define NPA_REMOTE_H
/*!
  @file
    npa_remote.h

  @brief
    This file contains the NPA remote external data structures and APIs.
*/

/*==============================================================================
  npa_remote.h

  NPA Remote API 

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/npa_remote.h#1 $ */

/** \mainpage NPA Remote API
 *
 *  - @ref remote Overall Remote Interface
 *    - @ref client Access NPA Clients remtotely
 *    - @ref event Acess NPA Events remotely
 *    - @ref query Access NPA Queries remotely
 *  - @ref resource API for defining remote resources
 *  - @ref protocol API for defining remote protocols
 *
 */


/** \defgroup remote NPA Remote API
 *
 * NPA Remote API is the interface to allow NPA resources to be accessed 
 * across processors.
 *
 * 
 */
/*@{*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Defines
 * -------------------------------------------------------------------------*/
/** \name NPA Remote version info
 *
 * @{
 */
#define NPA_REMOTE_VERSION_MAJOR   1 

#define NPA_REMOTE_VERSION_MINOR   1
/* TODO: Figure out how to define this via the build system */
#define NPA_REMOTE_VERSION_BUILD   0
/*@}*/

/** NPA remote max string buffer length */
#define NPA_REMOTE_MAX_STR_LEN    63


/*----------------------------------------------------------------------------
 * Enumerations
 * -------------------------------------------------------------------------*/

/**
   NPA remote function return status.
*/
typedef enum
{
  NPA_REMOTE_PROTOCOL_FAILURE = -1,
  NPA_REMOTE_SUCCESS = 0,
  NPA_REMOTE_FAILURE = 1,
} npa_remote_status;

/** NPA Remote client work models */
typedef enum
{
  NPA_REMOTE_CLIENT_RESERVED1  , /**< -- Reserved client type */
  NPA_REMOTE_CLIENT_RESERVED2  , /**< -- Reserved client type */
  NPA_REMOTE_CLIENT_CUSTOM1    , /**< -- Custom client types - 
                                         can be customized by resource */
  NPA_REMOTE_CLIENT_CUSTOM2    , /**< -- can be customized by resource */
  NPA_REMOTE_CLIENT_CUSTOM3    , /**< -- can be customized by resource */
  NPA_REMOTE_CLIENT_CUSTOM4    , /**< -- can be customized by resource */
  NPA_REMOTE_CLIENT_REQUIRED   , /**< -- Request must honor request */
  NPA_REMOTE_CLIENT_ISOCHRONOUS, /**< -- Placeholder for isochronous work model */
  NPA_REMOTE_CLIENT_IMPULSE    , /**< -- Poke the resource - node
                                  *  defines the impulse response */
  NPA_REMOTE_CLIENT_LIMIT_MAX  , /**< -- Set maximum value for the resource */
  NPA_REMOTE_CLIENT_VECTOR     , /**< -- Vector request */

  NPA_REMOTE_CLIENT_SIZE = 0x7fffffff /**< -- Force enum to 32 bits */

} npa_remote_client_type;


/** NPA Remote event types */
typedef enum
{
  NPA_REMOTE_EVENT_RESERVED,           /**< -- Reserved event */
  NPA_REMOTE_EVENT_HI_WATERMARK,       /**< -- Resource passed hi watermark */
  NPA_REMOTE_EVENT_LO_WATERMARK,       /**< -- Resource passed lo watermark */
  NPA_REMOTE_EVENT_CHANGE,             /**< -- Resource changed state */
  NPA_REMOTE_NUM_EVENT_TYPES,          /**< -- Size of enum */

  NPA_REMOTE_EVENT_SIZE = 0x7fffffff   /**< -- Force enum to 32 bits */
} npa_remote_event_type;
/*~ SENTINEL npa_remote_event_type.NPA_REMOTE_NUM_EVENT_TYPES */

/** 
 *
 * Note: All the public resource queries return a type of
 * npa_resource_state
 * 
 */
enum {
  NPA_REMOTE_QUERY_CURRENT_STATE, /**< -- Query the current state of the resource */
  NPA_REMOTE_QUERY_ACTIVE_MAX,    /**< -- Query the active maximum state supported 
                                          by the resource. The active max may be lower 
                                          than the resource maximum due to limit_max 
                                          request */
  NPA_REMOTE_QUERY_RESOURCE_MAX,  /**< -- Query the absolute max of the resource.  This 
                                          may be greater than the resource's active max
                                          if there are active limit_max requests */
  NPA_REMOTE_MAX_PUBLIC_QUERY     = 1023, /**< -- End public resource query space */
  NPA_REMOTE_REOURCE_AUTHOR_QUERY_START = NPA_REMOTE_MAX_PUBLIC_QUERY, /**< -- Internal Resource Requests */
  NPA_REMOTE_QUERY_RESOURCE_ATTRIBUTES,   /**< -- Get the resource attribute vector */
  NPA_REMOTE_QUERY_NODE_ATTRIBUTES, /**< -- Get the node attribute vector */
  NPA_REMOTE_MAX_RESOURCE_AUTHOR_QUERY = 2047,
  NPA_REMOTE_QUERY_SIZE = 0x7fffffff /**< -- Force enum to 32 bits */

};


/** Query Status */
typedef enum {
  NPA_REMOTE_QUERY_PROTOCOL_FAILURE = -1, /**< -- query failed at remote protocol level */
  NPA_REMOTE_QUERY_SUCCESS = 0,/**< -- query was successful */
  NPA_REMOTE_QUERY_UNSUPPORTED_QUERY_ID,/**< -- query type is unsupported */
  NPA_REMOTE_QUERY_UNKNOWN_RESOURCE, /**< -- remote resource is unknown */

  NPA_REMOTE_QUERY_STATUS_SIZE = 0x7fffffff /**< -- Force enum to 32 bits */
} npa_remote_query_status;

/*----------------------------------------------------------------------------
 * Types/Typedefs
 * -------------------------------------------------------------------------*/

/* Would like to use stdint.h, but WM doesn't support it */
#ifdef FEATURE_WINCE
typedef          int  int32_t;
typedef unsigned int  uint32_t;
#else
#include <stdint.h>
#endif

/** Work request types */
typedef uint32_t npa_remote_resource_state; 
typedef  int32_t npa_remote_resource_state_delta; 
typedef uint32_t npa_remote_resource_time;

/** Handle to the client */
typedef void * npa_remote_client_handle; 

/** Event structure */
typedef void * npa_remote_event_handle; 

typedef struct npa_remote_async_client_callback_data 
{ 
  npa_remote_client_handle   handle; 
  npa_remote_resource_state  state; 
} npa_remote_async_client_data;

/** Event data structure - passed to npa_remote_event callback */
typedef struct npa_remote_event_data
{
  npa_remote_resource_state        state;
  npa_remote_resource_state_delta  headroom;
  char                             resource_name[64];
} npa_remote_event_data;

typedef npa_remote_status ( * npa_remote_callback_type )( void     *context,
                                                          uint32_t  event_type, 
                                                          int32_t  *data,       
                                                          uint32_t  data_size); /*~ PARAM IN data VARRAY LENGTH data_size */
/*~ CALLBACK npa_remote_callback_type 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/** Query handle to the resource */
typedef void * npa_remote_query_handle; 

/** Query Type */
typedef enum
{
   NPA_REMOTE_QUERY_TYPE_STATE,
   NPA_REMOTE_QUERY_TYPE_VALUE,
   NPA_REMOTE_QUERY_TYPE_NAME,

   NPA_REMOTE_QUERY_NUM_TYPES
} npa_remote_query_type_enum;

typedef struct npa_remote_query_type 
{
  npa_remote_query_type_enum   type;
  union
  {
    npa_remote_resource_state  state;      /*~ CASE NPA_REMOTE_QUERY_TYPE_STATE npa_remote_query_type.data.state */
    uint32_t                   value;      /*~ CASE NPA_REMOTE_QUERY_TYPE_VALUE npa_remote_query_type.data.value */
    char                      *name;       /*~ CASE NPA_REMOTE_QUERY_TYPE_NAME npa_remote_query_type.data.name */
                                           /*~ FIELD npa_remote_query_type.data.name STRING */
                                           /*~ DEFAULT npa_remote_query_type.data.void */
  } data;                                  /*~ FIELD npa_remote_query_type.data DISC npa_remote_query_type.type */
} npa_remote_query_type;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**

   @brief Initialize the remote interface.
 
   Request initialization of the remote protocol at a particular
   version number.  The remote protocol will invoke the callback when
   the protocol is initialized and avaialble for use.  

   The remote protocol should not be used before the callback is
   received.  Behavior will be undefined in this case.

   If the remote protocol does not support the requested version,
   NPA_REMOTE_FAILURE will be returned. In this case, the callback
   will never be invoked.

   @param[in] major : major protocol number requested
   @param[in] minor : minor protocol number requested
   @param[in] build : build version number requested
   @param[in] callback : callback to invoke when remote protocol is fully initialized
   @param[in] context : context argument to be passed back to callback when callback is invoked
 
   @return npa_remote_status
 
   @sideeffects  will initialize remote protocol

   @dependencies None
 
   @ingroup remote   
  
**/
npa_remote_status
npa_remote_init( uint32_t                  major,
                 uint32_t                  minor,
                 uint32_t                  build,
                 npa_remote_callback_type  callback,
                 void                     *context );
/*~ FUNCTION npa_remote_init
    CLNTNAME npa_rpc_remote_init 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/*----------------------------------------------------------------------------
 * NPA Client API
 * -------------------------------------------------------------------------*/


/**

   @brief Creates a synchronous client of a resource.

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

   @param[in] resource_name : name of the resource that is to be used
   @param[in] client_name : name of the client that's being created
   @param[in] client_type : the work model that this client will use for
   work requests
   @param[out] handle : Handle to new client, NULL if client could not be created

   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client   
*/
npa_remote_status
npa_remote_create_sync_client( const char               *resource_name, /*~ PARAM IN resource_name STRING */
                               const char               *client_name,   /*~ PARAM IN client_name STRING */
                               npa_remote_client_type    client_type,
                               npa_remote_client_handle *handle );      /*~ PARAM OUT handle POINTER */ 
/*~ FUNCTION npa_remote_create_sync_client
    CLNTNAME npa_rpc_remote_create_sync_client
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Creates an asynchronous client of a resource.

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

   @param[in] resource_name : name of the resource that is to be used
   @param[in] client_name : name of the client that's being created 
   @param[in] client_type : the work model that this client will use for
   work requests
   @param[in] callback : callabck to be invoked when work request is complete
   @param[out] handle :  Handle to new client, NULL if client could not be
   created
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client   

*/
npa_remote_status
npa_remote_create_async_client( const char               *resource_name, /*~ PARAM IN resource_name STRING */
                                const char               *client_name,   /*~ PARAM IN client_name STRING */
                                npa_remote_client_type    client_type,
                                npa_remote_callback_type  callback,
                                void                     *context, 
                                npa_remote_client_handle *handle );      /*~ PARAM OUT handle POINTER */ 
/*~ FUNCTION npa_remote_create_async_client
    CLNTNAME npa_rpc_remote_create_async_client
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**

   @brief Destroy the client.  

   This will cancel the client request (if any) and free the client
   structure. The client handle can not be used to issue requests after
   being destroyed

   @param[in] client : handle to client being destroyed
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client   

*/
npa_remote_status
npa_remote_destroy_client( npa_remote_client_handle client ); /*~ PARAM IN client POINTER */
/*~ FUNCTION npa_remote_destroy_client
    CLNTNAME npa_rpc_remote_destroy_client 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Issue the REQUIRED client work request

   Resource state should be updated to account for new request. 
   
   The client must be of REQUIRED work model
  
   @param[in] client : handle to client being used
   @param[in] state : requested resource state
   @param[out] new_state : ptr to return new resource state 
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client   
*/
npa_remote_status
npa_remote_issue_required_request( npa_remote_client_handle   client,      /*~ PARAM IN client POINTER */
                                   npa_remote_resource_state  state,
                                   npa_remote_resource_state *new_state ); /*~ PARAM OUT new_state POINTER */ 
/*~ FUNCTION npa_remote_issue_required_request
    CLNTNAME npa_rpc_remote_issue_required_request 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Issue an IMPULSE work request 

   The client must be of IMPULSE work model

   The resource defines the impulse response

   Resource state should be updated to account for new request

   @param[in] client : handle to client being used
   @param[out] new_state : ptr to return new resource state 
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client   
*/
npa_remote_status
npa_remote_issue_impulse_request( npa_remote_client_handle client,        /*~ PARAM IN client POINTER */
                                  npa_remote_resource_state *new_state ); /*~ PARAM OUT new_state POINTER */
/*~ FUNCTION npa_remote_issue_impulse_request
    CLNTNAME npa_rpc_remote_issue_impulse_request 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Issue the VECTOR client work request

   Resource state should be updated to account for new request. 
   
   The vector pointermust remain valid for the duration of the
   requrest, but not beyond.  If the resource needs the data to
   persist, it is the repsonsibility of the resource to copy the data.

   The interpretation of the vector elements is defined by the
   resource.
   
   The client must be of VECTOR work model
  
   @param [in] client : handle to client being used
   @param [in] num_elems : number of elements in the vector
   @param [in] vector : array of num_elems
   @param [out] new_state : ptr to return new resource state
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client   
*/
npa_remote_status
npa_remote_issue_vector_request( npa_remote_client_handle   client,      /*~ PARAM IN client POINTER */
                                 uint32_t                   num_elems,
                                 npa_remote_resource_state *vector,      /*~ PARAM IN vector POINTER */
                                 npa_remote_resource_state *new_state ); /*~ PARAM OUT new_state POINTER */ 
/*~ FUNCTION npa_remote_issue_vector_request
    CLNTNAME npa_rpc_remote_issue_vector_request 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Issue an ISOCHRONOUS work request 

   The client must be of ISOCHRONOUS work model

   The client must issue npa_remote_complete_request() when the work is completed
   
   Resource state should be updated to accound for new work
   Note: The hint may be overriden/ignored by the resource

   @param[in] client : handle to client being used
   @param[in] deadline : the interval (in ms) when the work must be completed
   @param[in] level_hint : a hint as to what an appropriate level might
   be. May be overridden by resource
   @param[out] new_state : ptr to return new resource state 
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client  
*/
npa_remote_status
npa_remote_issue_isoc_request( npa_remote_client_handle   client,      /*~ PARAM IN client POINTER */
                               npa_remote_resource_time   deadline,
                               npa_remote_resource_state  level_hint,
                               npa_remote_resource_state *new_state ); /*~ PARAM OUT new_state POINTER */

/*~ FUNCTION npa_remote_issue_isoc_request
    CLNTNAME npa_rpc_remote_issue_isoc_request 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Issue the LIMIT_MAX  request

   If there are multiple concurrent max requests, the minimum of the
   requests will be used.

   The client must be of LIMIT_MAX work model.

   Note: The behavior of concurrent LIMIT_MAX and FIXED request or
   LIMIT_MAX < LIMIT_MIN requests is undefined
 
   @param[in] client : handle to client being used
   @param[in] max : maximum state of resource
   @param[out] new_state : ptr to return new resource state 
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client  
*/
npa_remote_status
npa_remote_issue_limit_max_request( npa_remote_client_handle   client,       /*~ PARAM IN client POINTER */
                                    npa_remote_resource_state  max,
                                    npa_remote_resource_state *new_state ); /*~ PARAM OUT new_state POINTER */
/*~ FUNCTION npa_remote_issue_limit_max_request
    CLNTNAME npa_rpc_remote_issue_limit_max_request
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Complete the work request

   This is used to notify the resource that the work request is
   complete and the client no longer needs the resource

   Any request state that has been learned will persist
   
   Resource state should be updated to account for completed work

   @param[in] client : handle to client being used
   @param[out] new_state : ptr to return new resource state 
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client 
*/
npa_remote_status
npa_remote_complete_request( npa_remote_client_handle   client,      /*~ PARAM IN client POINTER */
                             npa_remote_resource_state *new_state ); /*~ PARAM OUT new_state POINTER */
/*~ FUNCTION npa_remote_complete_request
    CLNTNAME npa_rpc_remote_complete_request 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Cancel the work request

   This can be used by any work model to terminate the work request.
   This internally invokes npa_remote_complete_request and then clears any
   adapted state

   This function is used when future client requests will be
   qualitatively different than previous requests made via this client

   Resource state should be updated to account for terminated work

   @param[in] client : handle to client being used
   @param[out] new_state : ptr to return new resource state 
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup client 
*/
npa_remote_status
npa_remote_cancel_request( npa_remote_client_handle client,        /*~ PARAM IN client POINTER */
                           npa_remote_resource_state *new_state ); /*~ PARAM OUT new_state POINTER */
/*~ FUNCTION npa_remote_cancel_request
    CLNTNAME npa_rpc_remote_cancel_request 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/*============================================================================
  NPA Event API

  NPA Events are invoked asynchronously to the resource update and will be 
  invoked in a serial manner to avoid requiring event handlers for a single 
  resource to be reentrant. 

  If the same event function is registered with multiple resources, there is 
  no guarantee that the event function will not be invoked from multiple 
  threads and it is the responsibility of the person registering the event 
  handler to deal with reentrancy 
============================================================================*/


/**
   @brief Create a change event handler with the given resource

   This will return a NULL handle if the resource does not exist or
   the event handler could not be created.

   The event_name must be < 64 characters in length.

   The event_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   If the DAL Event is a callback, the arguments will be:
   ( user_data, event_type, npa_remote_event_data *, sizeof( npa_remote_event_data *) )

   @param[in] resource_name : name of the resource that is to be used
   @param[in] event_name : name of the event handler that's being created 
   @param[in] callback :
   @param[in} context :
   @param[out] handle : New event handle, NULL if the event handle could not be created
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup event
 
*/
npa_remote_status
npa_remote_create_change_event( const char               *resource_name, /*~ PARAM IN resource_name STRING */
                                const char               *event_name,    /*~ PARAM IN event_name STRING */
                                npa_remote_callback_type  callback,
                                void                     *context,
                                npa_remote_event_handle  *handle );      /*~ PARAM OUT handle POINTER */       

/*~ FUNCTION npa_remote_create_change_event
    CLNTNAME npa_rpc_remote_create_change_event 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Create a watermark event handler with the given resource

   The event handler will not be invoked until the watermarks have
   been set

   The event_name must be < 64 characters in length.

   The event_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   If the DAL Event is a callback, the arguments will be:
   ( user_data, event_type, npa_remote_event_data *, sizeof( npa_remote_event_data *) )

   @param[in] resource_name : name of the resource that is to be used
   @param[in] event_name : name of the event handler that's being created 
   @param[in] callback :
   @param[in} context :
   @param[out] handle : New event handle, NULL if the event handle could not be created
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup event
*/
npa_remote_status
npa_remote_create_watermark_event( const char        *resource_name,    /*~ PARAM IN resource_name STRING */
                                   const char        *event_name,       /*~ PARAM IN event_name STRING */
                                   npa_remote_callback_type callback,
                                   void                     *context,
                                   npa_remote_event_handle  *handle );  /*~ PARAM OUT handle POINTER */ 


/*~ FUNCTION npa_remote_create_watermark_event
    CLNTNAME npa_rpc_remote_create_watermark_event 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Set the watermarks for triggering the event handler.

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
   
   @param[in] remote_event : handle of event being manipulated
   @param[in] hi_watermark : hi watermark setting
   @param[in] lo_watermark : lo watermark setting
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup event 

*/
npa_remote_status
npa_remote_set_event_watermarks( npa_remote_event_handle         remote_event,   /*~ PARAM IN event POINTER */
                                 npa_remote_resource_state_delta hi_watermark,
                                 npa_remote_resource_state_delta lo_watermark );
/*~ FUNCTION npa_remote_set_event_watermarks
    CLNTNAME npa_rpc_remote_set_event_watermarks 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Destroy the event handler
    
   After invocation, the event handle is invalidated and must not be
   reused

   @param[in] remote_event : handle of event being destroyed
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup event 
*/
npa_remote_status
npa_remote_destroy_event_handle( npa_remote_event_handle remote_event ); /*~ PARAM IN event POINTER */
/*~ FUNCTION npa_remote_destroy_event_handle
    CLNTNAME npa_rpc_remote_destroy_event_handle 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/*============================================================================
  NPA Query API

  NPA Queries are used to query the underlying resource state
============================================================================*/

/**
   @brief Create a query handle to a resource 
   
   @param[in] resource_name : name of resource being queried

   @param[out] query_handle : Query handle to resource
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup query 
*/
npa_remote_status 
npa_remote_create_query_handle( const char              *resource_name,  /*~ PARAM IN resource_name STRING */
                                npa_remote_query_handle *query_handle ); /*~ PARAM OUT query_handle POINTER */
/*~ FUNCTION npa_remote_create_query_handle
    CLNTNAME npa_rpc_remote_create_query_handle 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Destroy the query handle

   Only query handles that have been explicitly created via
   npa_remote_create_query_handle need to be explicitly destroyed

   @param[in] remote_query : Query handle to destroy
 
   @return npa_remote_status
 
   @sideeffects   None

   @dependencies None
 
   @ingroup query 
*/
npa_remote_status 
npa_remote_destroy_query_handle( npa_remote_query_handle remote_query ); /*~ PARAM IN query POINTER */
/*~ FUNCTION npa_remote_destroy_query_handle
    CLNTNAME npa_rpc_remote_destroy_query_handle 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */


/**
   @brief Query the current state of the resource

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param[in] query : query handle to resource being queried
   @param[in] query_id : type of query being issued
   @param[in,out] query_result : value from the query

   @return success/failure of query (npa_remote_query_status)
 
   @sideeffects   None

   @dependencies None
 
   @ingroup query 

*/
npa_remote_query_status 
npa_remote_query( npa_remote_query_handle remote_handle,  /*~ PARAM IN query POINTER */
                  uint32_t                query_id, 
                  npa_remote_query_type  *query_result ); /*~ PARAM INOUT query_result POINTER */
/*~ FUNCTION npa_remote_query
    CLNTNAME npa_rpc_remote_query 
    ONERROR return NPA_REMOTE_QUERY_PROTOCOL_FAILURE */


/** @brief Query the current state of the named resource

   This is a convenience function that wraps the

   h = npa_remote_create_query_handle(...);
   npa_remote_query(h, ...);
   npa_remote_destroy_query_handle(h);

   sequence. If you are making mulitple queries to the same resource,
   calling this function multiple times will have a poorer performance
   relative to managing the query handle directly.

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param[in] resource_name : name of resource being queried
   @param[in] query_id : type of query being issued
   @param[in,out] query_result : value from the query

   @return success/failure of query (npa_remote_query_status)
 
   @sideeffects   None

   @dependencies None
 
   @ingroup query 
*/
npa_remote_query_status 
npa_remote_query_by_name( const char             *resource_name,  /*~ PARAM IN resource_name STRING */
                          uint32_t                query_id, 
                          npa_remote_query_type  *query_result ); /*~ PARAM INOUT query_result POINTER */
/*~ FUNCTION npa_remote_query_by_name
    CLNTNAME npa_rpc_remote_query_by_name 
    ONERROR return NPA_REMOTE_QUERY_PROTOCOL_FAILURE */


/**
   @brief Query a resource via a client to that resource

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param[in] client : client of resource being queried
   @param[in] query_id : type of query being issued
   @param[in,out] query_result : value from the query

   @return success/failure of query (npa_remote_query_status)
 
   @sideeffects   None

   @dependencies None
 
   @ingroup query  
*/
npa_remote_query_status 
npa_remote_query_by_client( npa_remote_client_handle  remote_handle,  /*~ PARAM IN client POINTER */
                            uint32_t                  query_id, 
                            npa_remote_query_type    *query_result ); /*~ PARAM INOUT query_result POINTER */
/*~ FUNCTION npa_remote_query_by_client
    CLNTNAME npa_rpc_remote_query_by_client 
    ONERROR return NPA_REMOTE_QUERY_PROTOCOL_FAILURE */


/**
   @brief Query a resource via an event of that resource

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param[in] event : event of resource being queried
   @param[in] query_id : type of query being issued
   @param[in,out] query_result : value from the query

   @return success/failure of query (npa_remote_query_status)
 
   @sideeffects   None

   @dependencies None
 
   @ingroup query 
*/
npa_remote_query_status 
npa_remote_query_by_event( npa_remote_event_handle   remote_handle,  /*~ PARAM IN event POINTER */
                           uint32_t                  query_id, 
                           npa_remote_query_type    *query_result ); /*~ PARAM INOUT query_result POINTER */
/*~ FUNCTION npa_remote_query_by_event
    CLNTNAME npa_rpc_remote_query_by_event 
    ONERROR return NPA_REMOTE_QUERY_PROTOCOL_FAILURE */


/**
   
   @brief Notify when resource is available to be used

   Register an event to be triggered (once) when the given resource is
   available and ready for use.  The event may be fired during
   function execution if the resource is available when the function
   is invoked.  

   @param[in] resource_name : name of the resource that's being queried
   @param[in] callback :callback to invoke when resource is available
   @param[in] context : 
 
   @return npa_remote_status 
 
   @sideeffects   None

   @dependencies None
 
   @ingroup query  
*/

npa_remote_status 
npa_remote_resource_available( const char               *resource_name, /*~ PARAM IN resource_name STRING */
                               npa_remote_callback_type  callback,
                               void                     *context );
/*~ FUNCTION npa_remote_resource_available
    CLNTNAME npa_rpc_remote_resource_available 
    ONERROR return NPA_REMOTE_PROTOCOL_FAILURE */

/*@} /group remote */

/** 
 * \defgroup client NPA Remote Client 
 *  
 * \ingroup remote
 */

/** 
 * \defgroup query NPA Remote Query 
 *  
 *  NPA Queries are used to query the underlying resource state.
 *  
 * \ingroup remote
 */

/** 
 *  \defgroup event NPA Remote Event
 *  
 *  NPA Events are invoked asynchronously to the resource update and will be 
 *  invoked in a serial manner to avoid requiring event handlers for a single
 *  resource to be reentrant. 
 *
 *  If the same event function is registered with multiple resources, there is 
 *  no guarantee that the event function will not be invoked from multiple 
 *  threads and it is the responsibility of the person registering the event 
 *  handler to deal with reentrancy  
 *  
 * \ingroup remote
 */

#endif /* NPA_REMOTE_H */
