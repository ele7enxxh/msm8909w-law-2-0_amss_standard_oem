/*==============================================================================

  FILE:      npa.c

  DESCRIPTION: NPA framework implementation

  INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
  ==============================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/rpm.bf/2.1.1/core/power/npa/src/npa.c#1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------
  08/31/10   sg      Changed the callback arguments to resources_avail
  08/25/10   sg      Added support for enable/disable node
  08/20/10   sg      Added support for NPA_TRIGGER_THRESHOLD_EVENT
  08/04/10   sg      Added support for NPA_NODE_NO_LOCK
  08/03/10   sg      Fix double counting of resources in npa_resources_available
  06/23/10   tau     Featurize the async init in npa_init with !FEATURE_WINCE_OAL.
  05/07/10   sg      Extended Client Creation/Vector Request
  04/18/10   sg      Raw callback interface
  03/10/10   yz      Async event triggering
  02/23/10   sg      Factored out target configuration data
  02/08/10   sg      Removed ulog DAL, use direct interface
  02/05/10   yz      updated watermark trigger function to handle boundary values
  02/04/10   sg      Handle non-zero initialization properly
  01/13/10   sg      Properly initialize headroom at resource construction time
  12/21/09   ajf     Change to using newer ULog API
  12/08/09   sg      Added Query API
  10/20/09   sg      Don't try to track & reissue async requests
  09/08/09   sg      Changed variable length arrays to be enumerated 
  instead of NULL terminated 
  08/31/09   sg      Improved logging/added sequence number
  08/13/09   sg      Always commit requests, even if they're short-circuited
  08/12/09   sg      Improved error checking & refactored resource creation
  08/03/09   sg      Added compound node support
  05/01/09   sg      Initial import
  ==============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "npa_init.h"
#include "npa_resource.h"
#include "npa_extension.h"
#include "npa_config.h"
#include "npa_log.h"

#ifdef WINSIM
#include "npa_offline.h"
#endif /* WINSIM */

#include "ULogFront.h"

#include "DALSys.h"

#include <string.h>

#include "CoreVerify.h"
#include "CorePool.h"
#include "CoreWorkContainer.h"
#include "CoreWorkQueue.h"

#include "rpm_log.h"
#include "coredump.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define NPA_RESOURCE_OFF ((npa_resource_state)0)

#define NPA_TRIGGER_RESOURCE_AVAILABLE  NPA_TRIGGER_RESERVED_EVENT

#define NPA_SEQUENCE_INCREMENT 0x100
#define NPA_REDUNDANT_REQUEST  0x1

#ifndef MIN
#define MIN( a, b ) ((a)<(b)) ? (a) : (b)
#endif

#if defined(FEATURE_WINCE) || defined(__ARMCC_VERSION)
#define NPA_INLINE __inline
#else
#define NPA_INLINE inline
#endif

#define NPA_LIST_ADD( list, elem )                              \
  npa_list_add( (npa_list **)(list), (npa_list *)(elem) )
#define NPA_LIST_REMOVE( list, elem )                           \
  npa_list_remove( (npa_list **)(list), (npa_list *)(elem) )

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct npa_list
{
  struct npa_list *next;
} npa_list;

typedef struct npa_link
{
  struct npa_link *next;
  const char      *name;
  npa_resource    *resource;
} npa_link;

/* Single struct for holding all the event trigger data */
typedef struct npa_event_trigger
{
  npa_resource             *resource;
  npa_resource_state        old_state;
  npa_resource_state        new_state;
  npa_resource_state_delta  old_headroom;
  npa_resource_state_delta  new_headroom;
} npa_event_trigger;

typedef struct npa_continuation
{
  npa_callback_event callback;

  union
  {
    struct npa_alias_continuation
    {
      const char        *resource_name;
      const char        *alias_name;
    } alias;

    struct npa_node_continuation
    {
      npa_node_definition *definition;
      unsigned int         dependency;
    } node;

    struct npa_resources_available_continuation
    {
      const char  **resources;
      unsigned int  num_resources;
      unsigned int  index;
    } resources;

    struct npa_client_request_continuation
    {
      DALSYSEventHandle    enqueue_event; /* DAL Event Handle for enquing
                                           * async requests */
      npa_resource_state  *vector;
    } client;

  } data;
} npa_continuation;

typedef struct npa_struct
{
  /* memory pools */
  CorePoolType resource_pool;
  CorePoolType client_pool;
  CorePoolType event_pool;
  CorePoolType link_pool;

/* Global list of registered resources
 * 
 * TODO: Perhaps this should be a hash table indexed on resource name?
 * Depends on the number of resources and lookups that happen... */
  npa_link     *resources;
  npa_link     *placeholders;

  DALSYSSyncHandle graph_lock;
  DALSYSSyncHandle event_lock;

  ULogHandle     log;

  /* Workloop for async requests */
  DALSYSWorkLoopHandle async_request_handle;

  /* Workloop for npa event processing */
  CoreContainerWorkLoopHandle event_handle;

  /* Queue for async npa event processing */
  CoreWorkContainerHandle event_queue_handle;
  
} npa_struct;

/* alias some of the pools */
#define continuation_pool event_pool
CORE_STATIC_VERIFY( sizeof( npa_continuation ) <= sizeof( npa_event ) );

#define callback_pool link_pool
CORE_STATIC_VERIFY( sizeof( npa_callback_event ) <= sizeof( npa_link ) );

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/* NPA data structure */
static npa_struct npa;

/* Statically define the marker structures */
static npa_resource_definition marker_resource_definition = 
{
  "marker resource",
  "none",
  NPA_RESOURCE_DEFAULT,
  &npa_no_client_plugin,
  /* Rest is defined as 0/NULL */
};
static npa_node_definition marker_node_definition =
{
  "marker resource",
  NULL,
  NPA_NODE_DEFAULT,
  NULL,
  NPA_EMPTY_ARRAY,
  1,
  &marker_resource_definition
  /* Rest is defined as 0/NULL */
};

static npa_resource marker_resource =
{
  &marker_resource_definition,
  0,
  &marker_node_definition,
  NULL,
  NULL, 
  &npa_no_client_plugin,
  /* Rest is defined as 0/NULL */
};
/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
#ifndef FEATURE_SYNC_REQUESTS_ONLY
static void npa_enqueue_async_request( npa_client_handle client,
                                       int new_request );
#endif

static void npa_list_add( npa_list **list, npa_list *elem )
{
  /* assert( NULL != *list ); */
  /* assert( NULL != elem ); */
  elem->next = *list;
  *list = elem;
}

static void npa_list_remove( npa_list **list, npa_list *elem )
{
  npa_list *prev, *next;

  /* assert( NULL != *list ); */
  /* assert( NULL != elem ); */

  if ( *list == elem )
  {
    *list = elem->next;
    return;
  }

  prev = *list;
  next = prev->next;
  while ( NULL != next )
  {
    if ( next == elem )
    {
      prev->next = next->next;
      return;
    }
    prev = next;
    next = next->next;
  }  

  CORE_VERIFY( 0 );
}

/* Note: With the addition of the NPA_NODE_NO_LOCK attribute, it's
 * possible for this to be invoked with a NULL handle.  I can't find
 * any documentation confirming this, but from inspection, these
 * DALSYS functions will not error if invoked with a NULL handle so
 * I'm not adding a NULL check here.  */
static NPA_INLINE void npa_lock( DALSYSSyncHandle lock )
{
  DALSYS_SyncEnter(lock);
}

static NPA_INLINE void npa_unlock( DALSYSSyncHandle lock )
{
  DALSYS_SyncLeave(lock);
}

static NPA_INLINE int npa_resource_has_attribute( 
  npa_resource           *resource,
  npa_resource_attribute  attribute )
{
  return (resource->definition->attributes & attribute);
}

static NPA_INLINE int npa_node_has_attribute( 
  npa_node_definition    *node,
  npa_node_attribute  attribute )
{
  return (node->attributes & attribute);
}

static NPA_INLINE int npa_is_placeholder_resource( npa_resource *resource )
{
  return ( NULL == resource->definition );
}

static NPA_INLINE int npa_is_link_valid( npa_link *link )
{
  return ( ( NULL != link ) && 
           (!npa_is_placeholder_resource( link->resource ) ) );
}

static npa_link *npa_find_resource_link( const char *resource_name )
{
  npa_link *link = npa.resources;
  if ( NULL == resource_name ) {return NULL;}

  while ( NULL != link )
  {
    if ( 0 == strncmp(resource_name, link->name, NPA_MAX_STR_LEN + 1) )
    {
      return link;
    }
    link = link->next;
  }
  return NULL;
}

static NPA_INLINE 
void npa_trigger_callback_event( npa_callback_event *cb_event, 
                                 unsigned int        event_type, 
                                 void               *data,       
                                 unsigned int        data_size )
{
  CORE_VERIFY_PTR( cb_event );
  cb_event->callback( cb_event->context, event_type, data, data_size );
}




/* This is the cooked interface - it masks out the missing
 * resources.  Most requests should be using this interface */
static npa_resource *npa_find_resource( const char *resource_name )
{
  npa_link *link = npa_find_resource_link( resource_name );
  if ( npa_is_link_valid( link ) )
  {
    return link->resource;
  }
  return NULL;
}


static NPA_INLINE void npa_destroy_placeholder_link( npa_link *placeholder_link )
{
  npa_resource *resource;
  npa_event    *event;

  if ( NULL == placeholder_link ) { return; }

  resource = placeholder_link->resource;
  CORE_VERIFY( npa_is_placeholder_resource( resource ) );

  event = resource->events;

  while ( NULL != event )
  {
    npa_event *dead_event;

    CORE_VERIFY( NPA_TRIGGER_RESOURCE_AVAILABLE == event->trigger_type );

    npa_trigger_callback_event( &event->callback,
                                0, 
                                (void *)placeholder_link->name, 
                                strlen( placeholder_link->name ) );
    dead_event = event;
    event = event->next;
    Core_PoolFree( &npa.event_pool, dead_event );
  }

  CORE_DAL_VERIFY( DALSYS_DestroyObject( resource->node_lock ) );

  Core_PoolFree( &npa.resource_pool, resource );
  Core_PoolFree( &npa.link_pool, placeholder_link );
}

static void npa_cleanup_placeholders( void )
{
  npa_link *placeholder;

  while ( NULL != npa.placeholders )
  {
    npa_lock( npa.graph_lock );
    placeholder = npa.placeholders;
    npa.placeholders = placeholder->next;
    npa_unlock( npa.graph_lock );

    npa_destroy_placeholder_link( placeholder );
  } 
}

static void npa_link_resource( const char *name, npa_resource *resource )
{
  npa_link *placeholder;
  npa_link *link;

  CORE_VERIFY_PTR( resource );
  CORE_VERIFY_PTR( name );

  link = Core_PoolAlloc( &npa.link_pool );
  CORE_VERIFY_PTR( link );

  /* Initialize link */
  link->name     = name;
  link->resource = resource;

  npa_lock( npa.graph_lock );

  placeholder = npa_find_resource_link( name );

  CORE_LOG_VERIFY( !npa_is_link_valid( placeholder ),
                   NPA_LOG_CREATE( 
                     ULOG_RT_PRINTF_2( npa.log, 
                                       "\tFAILED resource \"%s\" already exists."
                                       "Defined by node %s", 
                                       name,
                                       placeholder->resource->node->name ) ) );

  /* Insert real link into global list */
  NPA_LIST_ADD( &npa.resources, link );

  if ( NULL != placeholder )
  {
    NPA_LIST_REMOVE( &npa.resources, placeholder );
    NPA_LIST_ADD( &npa.placeholders, placeholder );
  }

  npa_unlock( npa.graph_lock );

  return;
}

static npa_resource *npa_new_placeholder_resource( const char *resource_name )
{
  npa_resource *resource = Core_PoolAlloc( &npa.resource_pool );

  CORE_VERIFY_PTR( resource );

  /* Clear the structure out */
  memset( resource, 0, sizeof( npa_resource ) );

  resource->definition = NULL;
  CORE_DAL_VERIFY( DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, 
                                     &resource->node_lock, 
                                     NULL) );
  resource->event_lock = npa.event_lock;

  npa_link_resource( resource_name, resource );
  return resource;
}

/* Return the placeholder resource or NULL if the real resource exists  */
static npa_resource *npa_get_placeholder_resource( const char *resource_name )
{
  npa_resource *placeholder;
  npa_link     *link;

  CORE_VERIFY_PTR( resource_name );

  npa_lock( npa.graph_lock );

  link = npa_find_resource_link( resource_name );
  
  if ( npa_is_link_valid( link ) )
  {
    /* No placeholder - have real resource */
    placeholder = NULL;
  }
  else if ( NULL == link )
  {
    /* No link: create new placeholder */
    placeholder = npa_new_placeholder_resource( resource_name );
    CORE_VERIFY( NULL != placeholder );
  }
  else
  {
    /* Existing placeholder */
    placeholder = link->resource;
    CORE_VERIFY( NULL != placeholder );
  }

  npa_unlock( npa.graph_lock );
  return placeholder;
}

/* Clients are not reentrant - will break if they are reentered */
static NPA_INLINE void npa_request_serviced( npa_client_handle client )
{
  client->index ^= 1;

  /* Request processed - pending request has become active request
   *
   * If resource has not made a copy of the data to a different
   * location clear out vector so we don't retain reference to invalid
   * data */
  if ( ( NPA_CLIENT_VECTOR == client->type ) &&
       ( NULL != client->continuation ) &&
       ( NPA_ACTIVE_REQUEST(client).pointer.vector == 
         client->continuation->data.client.vector ) )
  {
    NPA_ACTIVE_REQUEST(client).pointer.vector = NULL; 
  }
}

/* These functions must be invoked from within a lock */
static NPA_INLINE void npa_invoke_update_function( npa_resource      *resource,
                                                   npa_client_handle  client )
{
  resource->request_state = 
    resource->active_plugin->update_fcn( resource, client );
}

static DALResult npa_async_event_handler( void *pData )
{
  npa_event_data     event_data;
  npa_event         *event;
  npa_event_type     change_event    = NPA_EVENT_RESERVED;
  npa_event_type     threshold_event = NPA_EVENT_RESERVED;
  npa_event_type     watermark_event = NPA_EVENT_RESERVED;
  npa_event_trigger *trigger = (npa_event_trigger *)pData;

  CORE_VERIFY_PTR( trigger );
  CORE_VERIFY_PTR( trigger->resource );

  npa_lock( trigger->resource->event_lock ); 

  event = trigger->resource->events;

  /* Initialize npa_event_data structure */
  event_data.resource_name = trigger->resource->definition->name;
  event_data.state         = trigger->new_state;
  event_data.headroom      = trigger->new_headroom;

  if ( trigger->new_state != trigger->old_state )
  {
    change_event = NPA_EVENT_CHANGE;
  }

  /* To trigger watermark events */
  if ( trigger->new_headroom != trigger->old_headroom ) 
  {
    watermark_event =  
      ( trigger->old_headroom > trigger->new_headroom ) ?
      NPA_EVENT_LO_WATERMARK : NPA_EVENT_HI_WATERMARK;
  }

  while ( NULL != event )
  {
    switch( event->trigger_type )
    {
    case NPA_TRIGGER_CHANGE_EVENT:
      /* To trigger change events */
      if ( NPA_EVENT_RESERVED != change_event ) 
      {
        npa_trigger_callback_event( &event->callback,
                                    NPA_EVENT_CHANGE, 
                                    (void *)&event_data, 
                                    sizeof( npa_event_data ) );
      }
      break;

    case NPA_TRIGGER_WATERMARK_EVENT:
      if ( event->lo.watermark <= event->hi.watermark ) 
      {
        switch (watermark_event)
        {
        case NPA_EVENT_HI_WATERMARK:
          if ( trigger->old_headroom < event->hi.watermark &&
               event->hi.watermark <= trigger->new_headroom )
          {
            npa_trigger_callback_event( &event->callback,
                                        watermark_event,
                                        &event_data, 
                                        sizeof( npa_event_data ) );
          }
          break;
        case NPA_EVENT_LO_WATERMARK:
          if ( trigger->old_headroom > event->lo.watermark &&
               event->lo.watermark >= trigger->new_headroom )
          {
            npa_trigger_callback_event( &event->callback,
                                        watermark_event,
                                        &event_data, 
                                        sizeof( npa_event_data ) );
          }
          break;
          
        default: 
          break;
        }
      }
      break;
    
    case NPA_TRIGGER_THRESHOLD_EVENT:
      if ( ( NPA_EVENT_RESERVED != change_event ) &&
           ( event->lo.threshold <= event->hi.threshold ) )
      {
        threshold_event = NPA_EVENT_RESERVED;
        if ( ( event_data.state < event->lo.threshold ) &&
             ( trigger->old_state >= event->lo.threshold ) )
        {
          threshold_event = NPA_EVENT_THRESHOLD_LO;
        }
        else if ( ( event_data.state > event->hi.threshold ) &&
                  ( trigger->old_state <= event->hi.threshold ) )
        {
          threshold_event = NPA_EVENT_THRESHOLD_HI;
        }
        else if ( ( ( event->lo.threshold <= event_data.state ) &&
                    ( event_data.state <= event->hi.threshold ) ) &&
                  ( (  trigger->old_state < event->lo.threshold  ) ||
                    ( trigger->old_state > event->hi.threshold ) ) )
        {
          threshold_event = NPA_EVENT_THRESHOLD_NOMINAL;
        }
          
        if ( NPA_EVENT_RESERVED != threshold_event )
        {
          npa_trigger_callback_event( &event->callback,
                                      threshold_event,
                                      &event_data, 
                                      sizeof( npa_event_data ) );
        }
      }
      break;
    default:
      CORE_VERIFY( 0 );
    }
    event = event->next;
  } 

  npa_unlock( trigger->resource->event_lock ); 

  return DAL_SUCCESS;
}

static NPA_INLINE void npa_update_resource_state( 
  npa_resource            *resource,
  npa_resource_state       new_state )
{
  npa_resource_state       old_state    = resource->active_state;
  npa_resource_state_delta old_headroom = resource->active_headroom;

  resource->active_state    = new_state;
  resource->active_headroom = resource->request_state - resource->active_max;

  if ( NULL != resource->events &&
       ( (resource->active_state    != old_state) || 
         (resource->active_headroom != old_headroom) ) )
  {
    npa_event_trigger *trigger;
    CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.event_queue_handle, (void **)&trigger) );

    trigger->resource     = resource;
    trigger->old_state    = old_state;
    trigger->new_state    = resource->active_state;
    trigger->old_headroom = old_headroom;
    trigger->new_headroom = resource->active_headroom;

    CORE_DAL_VERIFY( Core_WorkQueuePut(npa.event_queue_handle, trigger) );
  }
}

static NPA_INLINE void npa_invoke_driver_function( npa_resource *resource,
                                                   npa_client   *client )
{
  npa_resource_state new_state = 
    ( npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) ) ?
    resource->request_state : MIN( resource->request_state, 
                                   resource->active_max );

  /* TODO: make updates only on deltas be optional */
  if ( npa_resource_has_attribute( resource, 
                                   NPA_RESOURCE_DRIVER_UNCONDITIONAL ) ||
       ( new_state != resource->active_state ) )
  {
    new_state = resource->node->driver_fcn( resource, 
                                            client, 
                                            new_state );
  }

  /* Do this unconditionally - active max may have changed without
   * hitting driver function/changing resource state */
  npa_update_resource_state( resource, new_state );
}

static NPA_INLINE 
void npa_destroy_continuation( npa_continuation *continuation )
{
  if ( NULL != continuation )
  {
    Core_PoolFree( &npa.continuation_pool, continuation );
  }
}

/*======================================================================
  Constructors
  ======================================================================*/
static npa_continuation *npa_new_continuation(
  npa_callback  callback,
  npa_user_data context )
{
  npa_continuation *continuation = Core_PoolAlloc( &npa.continuation_pool );
  CORE_VERIFY_PTR( continuation );

  memset( continuation, 0, sizeof( npa_continuation ) );
  continuation->callback.callback = callback;
  continuation->callback.context  = context;

  return continuation;
}

static npa_resource *npa_new_resource( 
  npa_resource_definition *definition,
  npa_node_definition     *node_definition,
  DALSYSSyncHandle         node_lock )
{
  npa_resource        *resource;

  CORE_VERIFY_PTR( definition );
  CORE_VERIFY_PTR( node_definition );
  CORE_VERIFY_PTR( resource = Core_PoolAlloc( &npa.resource_pool ) );

  /* Clear the structure out */
  memset( resource, 0, sizeof( npa_resource ) );

  resource->definition    = definition;
  resource->active_plugin = definition->plugin;
  resource->active_max    = definition->max;
  resource->node          = node_definition;
  resource->node_lock     = node_lock;
  /* TODO: perhaps want to exchange this with a unique lock when an
     event is created against this resource */
  resource->event_lock    = npa.event_lock; 
  
  /* Create the state vector if necessary */
  if ( npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    unsigned int vector_size = definition->max * sizeof( npa_resource_state );

    definition->attributes |= NPA_RESOURCE_DRIVER_UNCONDITIONAL;
    CORE_DAL_VERIFY( DALSYS_Malloc( vector_size, (void **)&resource->state_vector ) );
    memset( resource->state_vector, 0, vector_size );
  }

  /* If they have the legacy create funcion, they can't have a an
   * extended function, and vice-versa */
  CORE_LOG_VERIFY( !( ( NULL != definition->plugin->create_client_fcn ) && 
                      ( NULL != definition->plugin->create_client_ex_fcn ) ),
                   NPA_LOG_CREATE( 
                     ULOG_RT_PRINTF_1( npa.log, 
                                       "\tFAILED resource %s plugin defined both "
                                       "create_client_fcn and create_client_ex_fcn", 
                                       definition->name ) ) );
  /* Ideally, I'd like to do this:
     if ( NULL != definition->plugin->create_client_fcn )
     {
     definition->plugin->create_client_ex_fcn = npa_plugin_legacy_create_client_ex;
     }      
     but this can fail if plugin is const. 
  */
  return resource;
}

static npa_client_handle npa_new_client( const char       *resource_name,
                                         const char       *client_name, 
                                         npa_client_type   client_type,
                                         unsigned int      client_value,
                                         void             *client_ref,
                                         void (*request_fcn)( 
                                           npa_client_handle client,
                                           int  new_request ) )
{
  npa_link      *link;
  npa_resource  *resource;
  npa_client    *client;

  CORE_VERIFY_PTR( client_name );
  CORE_VERIFY( strlen( client_name ) < 64 );

  link = npa_find_resource_link( resource_name );

  if ( !npa_is_link_valid( link ) )
  {
    /* TODO: need to create a reference copy of the resource_name here */
    NPA_LOG_CREATE( 
      ULOG_RT_PRINTF_2( npa.log, 
                        "\tFAILED npa_new_client \"%s\": resource \"%s\" does not exist", 
                        client_name, 
                        resource_name ) );
    return NULL; 
  }

  resource      = link->resource;
  resource_name = link->name;

  if ( !(resource->active_plugin->supported_clients & client_type) ) 
  { 
    NPA_LOG_CREATE(
      ULOG_RT_PRINTF_3( npa.log, 
                        "\tFAILED npa_new_client \"%s\": resource %s does not support client type %s", 
                        client_name, 
                        resource_name, 
                        npa_client_type_name(client_type)) );
    return NULL;
  }

  client = Core_PoolAlloc( &npa.client_pool );
  CORE_VERIFY_PTR( client );

  memset( client, 0, sizeof( npa_client ) );

  client->name          = client_name;
  client->resource_name = resource_name;
  client->resource      = resource;
  client->type          = client_type;
  client->issue_request = request_fcn;

#ifndef FEATURE_SYNC_REQUESTS_ONLY
  if ( npa_enqueue_async_request == request_fcn )
  {
    client->continuation = npa_new_continuation(NULL, NULL);
    if ( NULL == client->continuation )
    {
      Core_PoolFree( &npa.client_pool, client );
      return NULL;
    }
  }
#endif

  npa_lock( resource->node_lock );

  /* Add client to list */
  if ( npa_resource_has_attribute( resource, 
                                   NPA_RESOURCE_SINGLE_CLIENT ) &&
       resource->clients != NULL )
  {
    NPA_LOG_CREATE( 
      ULOG_RT_PRINTF_2( npa.log, 
                        "\tFAILED npa_new_client \"%s\": "
                        "resource \"%s\" only supports single client and already has one"
                        "(error_code: %d)",
                        client_name, 
                        resource_name ) );
    goto client_error;
  }

  /* Call client create function if defined */
  if ( NULL != resource->active_plugin->create_client_ex_fcn )
  {
    unsigned int code = 
      resource->active_plugin->create_client_ex_fcn( client,
                                                     client_value,
                                                     client_ref );

    if ( 0 != code )
    {
      NPA_LOG_CREATE( 
        ULOG_RT_PRINTF_3( npa.log, 
                          "\tFAILED npa_new_client \"%s\": "
                          "resource \"%s\" failed client create "
                          "(error_code: %d)",
                          client_name, 
                          resource_name,
                          code ) );
      
      goto client_error;
    } 
  }
  else if ( NULL != resource->active_plugin->create_client_fcn )
  {
    /* Should I verify extended args are 0/NULL?
     * Perhaps log a warning message? */
    client->resource->definition->plugin->create_client_fcn( client );
  }
  
  /* add client to client list */
  NPA_LIST_ADD( &resource->clients, client );

  npa_unlock( resource->node_lock );
  return client;


  /* Error handling path - free up everything and return NULL */
client_error:
  npa_unlock( resource->node_lock );
  npa_destroy_continuation( client->continuation );
  Core_PoolFree( &npa.client_pool, client );
  return NULL;
}

static npa_event *npa_new_event( 
  const char      *resource_name,
  const char      *event_name,
  npa_event_trigger_type trigger_type,
  npa_callback     callback,
  void            *context )
{
  npa_event    *event;
  npa_resource *resource;
  npa_link     *link = npa_find_resource_link( resource_name );

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_create_event "
                      "(resource: \"%s\") (event: \"%s\") (type: \"%s\")",
                      resource_name, event_name,
                      npa_event_type_name( trigger_type ) ) );
  
  if ( ( NULL == link ) ||
       ( npa_is_placeholder_resource( link->resource ) && 
         NPA_TRIGGER_RESOURCE_AVAILABLE != trigger_type ) )
  {
    /* TODO: need to create a reference copy of the resource_name here */
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_2( npa.log, 
                        "\tFAILED npa_new_event \"%s\": resource \"%s\" does not exist", 
                        event_name, 
                        resource_name ) );
    return NULL; 
  }

  if ( NULL == npa.event_queue_handle )
  {
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_1( npa.log, 
                        "\tFAILED npa_new_event \"%s\": "
                        "NPA target configuration does not support events",
                        event_name ) );
    return NULL; 
  }

  resource      = link->resource;
  resource_name = link->name;

  /* Can't create events against markers */
  if( resource == &marker_resource ) 
  { 
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_2( npa.log,
                        " Can't create event \"%s\" against marker resource \"%s\"",
                        event_name, resource_name ) );
    return NULL; 
  }

  /* How should the error be handled? Currently it's fatal... */
  CORE_VERIFY_PTR( resource );
  CORE_VERIFY( strlen( event_name ) < 64 );
  CORE_VERIFY_PTR( callback );

  event = Core_PoolAlloc( &npa.event_pool );
  CORE_VERIFY_PTR( event );

  memset( event, 0, sizeof( npa_event ) );

  event->name          = event_name;
  event->resource_name = resource_name;
  event->resource      = resource;
  event->trigger_type  = trigger_type;
  event->lo.watermark  = 0x1;

  event->callback.callback = callback;
  event->callback.context  = context;

  npa_lock( resource->event_lock );
  NPA_LIST_ADD( &resource->events, event );
  npa_unlock( resource->event_lock );

  return event;
}

static NPA_INLINE void npa_update_resource( npa_resource      *resource,
                                            npa_client_handle  client ) 
{
  npa_lock( resource->node_lock );
  
  client->sequence = (resource->sequence += NPA_SEQUENCE_INCREMENT);

  npa_invoke_update_function( resource, client );
  npa_invoke_driver_function( resource, client );

  npa_unlock( resource->node_lock );

  return;
}

static NPA_INLINE void npa_client_complete_request( npa_client *client )
{
  npa_request_serviced( client );

  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_4( npa.log,
                      "\trequest complete "
                      "(handle: 0x%08x) (sequence: 0x%08x) "
                      "(request state:%d) (active state:%d)",
                      client, 
                      client->sequence,
                      client->resource->request_state,
                      client->resource->active_state ) );

  /* Invoke Async Client Completion Event */
  if ( (NULL != client->continuation) && 
       ( NULL != client->continuation->callback.callback ) )
  {
    npa_trigger_callback_event( &client->continuation->callback,
                                0,
                                client, 
                                sizeof( npa_client_handle ) );
  }

}

static void npa_process_request( npa_client_handle client )
{
  CORE_VERIFY( NULL != client );
  CORE_VERIFY( NULL != client->resource );

  npa_update_resource( client->resource, client );
  npa_client_complete_request( client );
}

static void npa_terminate_request( npa_client_handle client )
{
  /* TODO: May want to use a complete_request function pointer */
  if ( NPA_CLIENT_LIMIT_MAX == client->type ) 
  {
    npa_issue_limit_max_request( client, 0 );
  }
  else 
  {
    NPA_PENDING_REQUEST(client).state = 0;
    client->issue_request( client, NPA_ACTIVE_REQUEST(client).state != 0 );
  }
}

static void npa_issue_sync_request( npa_client_handle client,
                                    int new_request )
{ 
  if ( new_request )
  {
    npa_process_request( client );
  }
  else 
  {
    client->sequence |= NPA_REDUNDANT_REQUEST;
    npa_client_complete_request( client );
  }
}

#ifndef FEATURE_SYNC_REQUESTS_ONLY
/* This function runs in the async client service workloop */
static DALResult npa_issue_async_request( DALSYSEventHandle event,
                                          void *arg )
{
  npa_client_handle client = (npa_client_handle)arg;

  npa_process_request(client);
  return DAL_SUCCESS;
}

static void npa_enqueue_async_request( npa_client_handle client,
                                       int new_request )
{
  if ( new_request )
  {
    CORE_DAL_VERIFY( DALSYS_EventCtrl(client->continuation->data.client.enqueue_event,
                                      DALSYS_EVENT_CTRL_TRIGGER ));
  }
  else
  {
    /* Complete synchronously */
    npa_client_complete_request( client );
  }
}
#endif



static void npa_create_node( npa_node_definition *definition,
                             npa_callback_event  *callback )
{
  npa_client               client;
  npa_node_dependency     *dependency;
  npa_resource_definition *resource_def;
  DALSYSSyncHandle         node_lock;
  unsigned int             i;

  CORE_VERIFY_PTR( callback );
  CORE_VERIFY_PTR( definition );
  CORE_VERIFY_PTR( &definition->resources[0] );

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_2( npa.log, 
                      "npa_create_node "
                      "(handle: 0x%08x) "
                      "(node: \"%s\")",
                      definition,
                      definition->name ) );

  /* Create resource lock */
  node_lock = NULL;
  if ( !npa_node_has_attribute(definition, NPA_NODE_NO_LOCK ) )
  {
    CORE_DAL_VERIFY( DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, 
                                       &node_lock, 
                                       NULL) );
  }

  /* Create dependencies (if necessary) */
  for ( i = 0, dependency = &definition->dependencies[i];
        i < definition->dependency_count;
        dependency = &definition->dependencies[++i] )
  {
    dependency->handle = NULL;
    if ( NPA_NO_CLIENT == dependency->client_type ) 
    {
      continue;
    }
    
    CORE_VERIFY_PTR ( 
      dependency->handle = npa_create_sync_client( dependency->name, 
                                                   definition->name, 
                                                   dependency->client_type ) );
  }


  /* Create resources */
  for ( i = 0, resource_def = &definition->resources[i]; 
        i < definition->resource_count;
        resource_def = &definition->resources[++i] )
  {
    npa_link *placeholder = npa_find_resource_link( resource_def->name );

    CORE_VERIFY_PTR( placeholder );
    CORE_VERIFY_PTR( resource_def->handle = npa_new_resource( resource_def, 
                                                              definition, 
                                                              node_lock ) );    
    /* copy the request over from the placeholder */
    resource_def->handle->request_state = placeholder->resource->request_state;

    /* Initialize the index for this resource */
    resource_def->handle->index = i;
  }

  /* Create initialization client */
  memset( &client, 0, sizeof( npa_client ) );
  client.type = NPA_CLIENT_INITIALIZE;

  for ( i = 0, resource_def = &definition->resources[i]; 
        i < definition->resource_count;
        resource_def = &definition->resources[++i] )
  {
    /* Initialize resource */
    npa_resource *resource = resource_def->handle;

    npa_update_resource_state( resource,
                               resource->node->driver_fcn(
                                 resource, 
                                 &client, 
                                 resource->request_state ) );
    
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_4( npa.log, 
                        "\tcreated resource "
                        "(resource: \"%s\") "
                        "(handle: 0x%08x) "
                        "(initial state: %d) "
                        "(active state: %d)",
                        resource_def->name,
                        resource,
                        resource->request_state,
                        resource->active_state ) );

    /* Clear out request state so it doesn't interfere with client
     * requests */
    resource->request_state = 0;
  }

  for ( i = 0, resource_def = &definition->resources[i]; 
        i < definition->resource_count;
        resource_def = &definition->resources[++i] )
  {
    /* Add resource to the list of resources */
    npa_link_resource( resource_def->name, resource_def->handle );
  }
  
  /* Invoke the node define callback */
  if ( NULL != callback->callback )
  {
    npa_trigger_callback_event( callback,
                                0,
                                (void *)definition->name, 
                                strlen( definition->name ) );
  }

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_2( npa.log, 
                      "\tend npa_create_node "
                      "(handle: 0x%08x) "
                      "(node: \"%s\")",
                      definition,
                      definition->name ) );
}

static void npa_define_node_continuation(
  void         *user_data, 
  unsigned int  cb_event_type,
  void         *cb_data, 
  unsigned int  size )
{
  npa_node_definition *node;
  npa_continuation    *continuation =  (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );
  
  node = continuation->data.node.definition;

  /* Find next unavailable resource */
  while ( ( continuation->data.node.dependency < node->dependency_count ) &&
          ( NULL != npa_find_resource( 
            node->dependencies[continuation->data.node.dependency].name ) ) )
  {
    continuation->data.node.dependency++;
  }

  if ( continuation->data.node.dependency == node->dependency_count )
  {
    npa_create_node( node, &continuation->callback );
    npa_destroy_continuation( continuation );
  }
  else 
  {
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_3( npa.log, "\tnode waiting on resource "
                        "(handle: 0x%08x) "
                        "(node: \"%s\") "
                        "(resource: \"%s\")",
                        node,
                        node->name,
                        node->dependencies[continuation->data.node.dependency].name ) );


    npa_resource_available_cb ( 
      node->dependencies[continuation->data.node.dependency++].name, 
      npa_define_node_continuation,
      continuation );
  }
}

static void npa_alias_resource_continuation( 
  void         *user_data, 
  unsigned int  cb_event_type,
  void         *cb_data, 
  unsigned int  size )
{
  npa_continuation *continuation = (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );

  CORE_VERIFY( 0 == strncmp( continuation->data.alias.resource_name, 
                            (const char *)cb_data, 
                            NPA_MAX_STR_LEN + 1 ) );

  npa_link_resource( continuation->data.alias.alias_name, 
                     npa_find_resource( continuation->data.alias.resource_name ) );

  if ( NULL != continuation->callback.callback ) 
  {
    npa_trigger_callback_event( &continuation->callback,
                                0,
                                (void *)continuation->data.alias.alias_name, 
                                strlen( continuation->data.alias.alias_name ) );
  }

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_2( npa.log, 
                      "\tend npa_alias_resource "
                      "(resource: \"%s\") "
                      "(alias: \"%s\")", 
                      continuation->data.alias.resource_name, 
                      continuation->data.alias.alias_name ) );

  npa_destroy_continuation( continuation );
}

static void npa_init_pool( CorePoolType          *pool, 
                           const npa_pool_config *config,
                           unsigned int           element_size )
{
  CorePoolConfigType  pool_config;

  CORE_VERIFY_PTR( pool );
  CORE_VERIFY_PTR( config );

  pool_config.size       = element_size;
  pool_config.initial    = config->initial;
  pool_config.subsequent = config->subsequent;

  Core_PoolConfig( pool, &pool_config );
}

static void npa_init_workloops( const npa_config *config_data )
{
  if( config_data->async_event_workloop.num_events )
  {
    CORE_DAL_VERIFY( 
      Core_WorkQueueCreate( &npa.event_queue_handle, 
                            npa_async_event_handler,
                            config_data->event_queue_allocation.initial,
                            config_data->event_queue_allocation.subsequent, 
                            sizeof(npa_event_trigger) ) );

#ifdef FEATURE_SYNC_EVENTS_ONLY
    CORE_VERIFY( 0xffffffff == config_data->async_event_workloop.priority );
#else
    if ( 0xffffffff != config_data->async_event_workloop.priority )
    {
      CORE_DAL_VERIFY( 
        Core_RegisterContainerWorkLoop( config_data->async_event_workloop.priority, 
                                        &npa.event_handle ) );
      
      CORE_DAL_VERIFY( 
        Core_AddContainerToWorkLoop( npa.event_queue_handle, npa.event_handle ) );
    }
#endif
  }

  
#ifdef FEATURE_SYNC_REQUESTS_ONLY
    CORE_VERIFY( 0xffffffff == config_data->async_event_workloop.priority );
#else
  if( config_data->async_request_workloop.num_events )
  {
    /* Create async request Workloop */
    CORE_DAL_VERIFY( 
      DALSYS_RegisterWorkLoop( config_data->async_request_workloop.priority, 
                               config_data->async_request_workloop.num_events,
                               &npa.async_request_handle,
                               NULL ) );
  }
#endif
}

/*======================================================================
  Public Functions
  ======================================================================*/
/* External target-specific function  */
extern void npa_target_init( void );

#ifdef NPA_SELF_TEST
static int npa_self_test = 1;
extern void npa_test( void );
#endif  /* NPA_SELF_TEST */

void npa_init( void )
{
  /* If the Log doesn't exist, we're not initialized
   *
   * TODO: Should I worry about a race condition on init?
   */
  if ( NULL == npa.graph_lock )           
  {
    const npa_config *npa_config_data = npa_target_config();

    CORE_VERIFY_PTR( npa_config_data );
   
    /* Create NPA locks */
    CORE_DAL_VERIFY( DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, 
                                       &npa.graph_lock, NULL ) );
    CORE_DAL_VERIFY( DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, 
                                       &npa.event_lock, NULL ) );
    
    ///* Initialize logging system */
    //CORE_DAL_VERIFY( ULogFront_RealTimeInit(&npa.log,
    //                                        "NPA Log",
    //                                        npa_config_data->log_buffer_size,
    //                                        ULOG_MEMORY_LOCAL,
    //                                        ULOG_LOCK_OS ) );

    /* Initialize memory pools */
    npa_init_pool( &npa.resource_pool, 
                   &npa_config_data->resource_allocation, 
                   sizeof(npa_resource) );

    npa_init_pool( &npa.client_pool, 
                   &npa_config_data->client_allocation,
                   sizeof(npa_client) );

    npa_init_pool( &npa.event_pool,
                   &npa_config_data->event_allocation,
                   sizeof(npa_event) );

    npa_init_pool( &npa.link_pool,
                   &npa_config_data->link_allocation,
                   sizeof(npa_link) );

    /* TODO: Perhaps this should be featurized separately  */
#ifndef FEATURE_WINCE_OAL
    npa_init_workloops( npa_config_data );
#endif /* !FEATURE_WINCE_OAL */

    /* Initialize target nodes */
    npa_target_init();

    /* This is done better in RPM target init code, but 'npa' is currently
     * static so it has to go here for now. */
    rpm_core_dump.npa_state = &npa;

#ifdef NPA_SELF_TEST
    if ( npa_self_test ) {npa_test();}
#endif

  }

  return;
}

void npa_define_node_cb( npa_node_definition *definition,
                         npa_resource_state   initial_state[],
                         npa_callback         callback,
                         npa_user_data        context )
{
  npa_continuation *continuation;
  unsigned int      count = 0;

  CORE_LOG_VERIFY_PTR( definition,
                       NPA_LOG_CREATE (
                         ULOG_RT_PRINTF_0( 
                           npa.log,
                           "\tFAILED npa_define_node definition is NULL" ) ) );
  
  CORE_LOG_VERIFY( (NULL != definition->resources) && 
                   (definition->resource_count > 0),
                   NPA_LOG_CREATE (
                     ULOG_RT_PRINTF_1( 
                       npa.log,
                       "\tFAILED npa_define_node \"%s\" has no resourcees",
                       definition->name ) ) );

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_4( npa.log, "npa_define_node "
                      "(handle: 0x%08x) "
                      "(node: \"%s\") " 
                      "(resource count: %d) "
                      "(dependency count: %d)", 
                      definition,
                      definition->name,
                      definition->resource_count,
                      definition->dependency_count) );

  for ( count = 0; count < definition->resource_count; count++ )
  {
    npa_resource *placeholder = 
      npa_get_placeholder_resource( definition->resources[count].name );

    /* If there's not a placeholder, soemthing by that name
     * (resource/alias/marker) must have been previously defined. Name
     * collisions are bad... */
    CORE_LOG_VERIFY_PTR( placeholder,
                         NPA_LOG_CREATE (
                           ULOG_RT_PRINTF_1( npa.log,
                                             "\tFAILED resource %s already defined",
                                             definition->resources[count].name ) ) );

    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_4( npa.log, "\tnode resource "
                        "(handle: 0x%08x) "
                        "(node: \"%s\") "
                        "(resource: \"%s\") "
                        "(units: \"%s\")",
                        definition,
                        definition->name,
                        definition->resources[count].name,
                        definition->resources[count].units ) );

    /* Store the requested initial state in the placeholder */
    placeholder->request_state = (initial_state) ? initial_state[count] : 0;
  }

  for ( count = 0; count < definition->dependency_count; count++ )
  {
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_3( npa.log, "\tnode dependency "
                        "(handle: 0x%08x) "
                        "(node: \"%s\") "
                        "(dependency: \"%s\")",
                        definition,
                        definition->name,
                        definition->dependencies[count].name ) );
  }

  continuation = npa_new_continuation( callback, context );
  continuation->data.node.definition = definition;
  continuation->data.node.dependency = 0;
  
  /* Trigger the continuation directly for the first time */
  npa_define_node_continuation( continuation, 0, NULL, 0 );

  /* Invoke any resource available callbacks */
  npa_cleanup_placeholders();

  return;

}

void npa_alias_resource_cb( const char    *resource_name, 
                            const char    *alias_name,
                            npa_callback   callback,
                            npa_user_data  context )
{
  npa_continuation *continuation;

  CORE_LOG_VERIFY_PTR( resource_name,
                       NPA_LOG_CREATE (
                         ULOG_RT_PRINTF_0( 
                           npa.log,
                           "\tFAILED npa_alias_resource resource_name is NULL" ) ) );

  CORE_LOG_VERIFY_PTR( alias_name,
                       NPA_LOG_CREATE (
                         ULOG_RT_PRINTF_0( 
                           npa.log,
                           "\tFAILED npa_alias_resource alias name is NULL" ) ) );

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_2( npa.log, 
                      "npa_alias_resource "
                      "(resource: \"%s\") (alias: \"%s\")", 
                      resource_name, alias_name ) );

  continuation = npa_new_continuation( callback, context );
  continuation->data.alias.resource_name = resource_name;
  continuation->data.alias.alias_name    = alias_name;

  npa_resource_available_cb ( resource_name,
                              npa_alias_resource_continuation,
                              continuation );

  /* Invoke any resource available callbacks */
  npa_cleanup_placeholders();
}

/* Verify that the node the user passed in as the same identity as the
 * one in the graph */
static int npa_node_equal( npa_node_definition *user_node,
                           npa_node_definition *graph_node,
                           unsigned int attr_mask )
{
  unsigned int i;

  CORE_VERIFY_PTR( user_node );
  CORE_VERIFY_PTR( graph_node );
  
  if ( ( 0 != strncmp( user_node->name, graph_node->name, NPA_MAX_STR_LEN + 1 ) ) ||
       ( ( user_node->attributes & attr_mask ) != 
         ( graph_node->attributes & attr_mask ) ) ||
       ( user_node->dependency_count != graph_node->dependency_count ) ||
       ( user_node->resource_count != graph_node->resource_count ) )
  {
    return 0;
  }

  for( i = 0; i < user_node->dependency_count; i++ )
  {
    if ( 0 != strncmp( user_node->dependencies[i].name, 
                       graph_node->dependencies[i].name, 
                       NPA_MAX_STR_LEN + 1 ) )
    {
      return 0;
    }
  }

  for( i = 0; i < user_node->resource_count; i++ )
  {
    if ( 0 != strncmp( user_node->resources[i].name, 
                       graph_node->resources[i].name,
                       NPA_MAX_STR_LEN + 1 ) )
    {
      return 0;
    }
  }
  return 1;
}

static void *npa_copy_buffer( const void *src_buffer, size_t length )
{
  void *new_buffer;

  CORE_DAL_VERIFY( DALSYS_Malloc( length, (void **) &new_buffer ) );
  memcpy( new_buffer, src_buffer, length );

  return new_buffer;
}

static NPA_INLINE const char *npa_copy_name( const char * name )
{
  return (const char *)npa_copy_buffer( name, strlen(name)+1 );
}

/* Pass through query function for disabled resources.  This can be
 * used as a flag to determine if the resource is disabled. */
static 
npa_query_status npa_disabled_query_fcn( npa_resource    *resource, 
                                         unsigned int     id, 
                                         npa_query_type  *result )
{
  return NPA_QUERY_UNSUPPORTED_QUERY_ID;
}

static npa_node_definition *npa_copy_node( npa_node_definition *user_node )
{
  unsigned int i;
  npa_node_definition *node;

  node = (npa_node_definition *)
    npa_copy_buffer( user_node, sizeof( npa_node_definition ) );
  CORE_VERIFY_PTR(node);
    
  node->dependencies = ( npa_node_dependency *)
    npa_copy_buffer( user_node->dependencies,
                     node->dependency_count * sizeof( npa_node_dependency ) );
  CORE_VERIFY_PTR(node->dependencies);
                     
  node->resources = ( npa_resource_definition *)
    npa_copy_buffer( user_node->resources,
                     node->resource_count * sizeof( npa_resource_definition ) );
  CORE_VERIFY_PTR(node->resources);
  
  node->name = npa_copy_name( node->name );
  for ( i = 0; i < node->dependency_count; i++ )
  {
    node->dependencies[i].name = 
      npa_copy_name( user_node->dependencies[i].name );
  }
  for ( i = 0; i < node->resource_count; i++ )
  {
    node->resources[i].name = npa_copy_name( user_node->resources[i].name );
  }

  return node;
}

void npa_enable_node( npa_node_definition *user_node, 
                      npa_resource_state default_state[] )
{
  npa_resource *resource;
  npa_node_definition *graph_node;
  CORE_VERIFY_PTR( user_node );
  
  resource = npa_find_resource( user_node->resources[0].name );
  if ( NULL == resource )
  {
    /* Enabling node for the first time - copy and construct it */
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_1( npa.log, 
                        "npa_enable_node - initial construction (node: \"%s\")",
                        user_node->name ) );
    CORE_LOG_VERIFY( 
      !npa_node_has_attribute( user_node, NPA_NODE_NO_LOCK ),
      NPA_LOG_CREATE (
        ULOG_RT_PRINTF_1( npa.log, 
                          "\tFAILED node \"%s\" has NO_LOCK attribute",
                          user_node->name ) ) ); 
    
    /* Copy the node and add the DSIABLEABLE attribute */
    graph_node = npa_copy_node( user_node );
    graph_node->attributes |= NPA_NODE_DISABLEABLE;

    npa_define_node_cb( graph_node, default_state, 
                        NULL, NULL );
  }
  else
  {
    /* Enabling disabled node...
     *
     * At least - it has better have been disabled.  I don't currently
     * have am easy way to valiate that the node is in a disabled state */
    unsigned int i;
    npa_client client;

    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_1( npa.log, 
                        "npa_enable_node - reenable (node: \"%s\")",
                        user_node->name ) );

    memset( &client, 0, sizeof( npa_client ) );
    client.type = NPA_CLIENT_INITIALIZE;

    graph_node = resource->node;
    CORE_VERIFY( npa_node_equal( user_node, graph_node, ~NPA_NODE_DISABLEABLE ) );

    if ( npa_disabled_query_fcn == resource->definition->query_fcn )
    {
      graph_node->driver_fcn = user_node->driver_fcn;
      graph_node->data       = user_node->data;

      for ( i = 0; i < graph_node->resource_count; i++ )
      {
        npa_resource *resource = graph_node->resources[i].handle;
        npa_resource_state enable_state = resource->request_state;

        graph_node->resources[i].data = user_node->resources[i].data;

        graph_node->resources[i].query_fcn = user_node->resources[i].query_fcn;
        graph_node->resources[i].plugin = user_node->resources[i].plugin;
        
        if ( NULL == resource->clients )
        { 
          enable_state = (NULL == default_state) ? 0 : default_state[i];
        }
        
        npa_update_resource_state( resource,
                                   resource->node->driver_fcn(
                                     resource, 
                                     &client, 
                                     enable_state ) );
        NPA_LOG_CREATE (
          ULOG_RT_PRINTF_1( npa.log, 
                            "\tenable resource (resource: \"%s\")",
                            graph_node->resources[i].name ) );
      }
      npa_unlock( resource->node_lock );
    }
    else 
    {
      NPA_LOG_CREATE (
        ULOG_RT_PRINTF_0( npa.log, 
                          "\tWARNING - enabling node that is not disabled" ) );
    }
  }
  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_1( npa.log, 
                      "\tend npa_enable_node (node: \"%s\")",
                      user_node->name ) );
}

void npa_disable_node( npa_node_definition *user_node )
{
  unsigned int i;
  npa_resource *resource;
  npa_node_definition *graph_node;

  CORE_VERIFY_PTR( user_node );
  CORE_VERIFY_PTR( 
    resource = npa_find_resource( user_node->resources[0].name ) );

  graph_node = resource->node;

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_1( npa.log, 
                      "npa_disable_node (node: \"%s\")",
                      graph_node->name ) );

  CORE_LOG_VERIFY( npa_node_has_attribute( graph_node, NPA_NODE_DISABLEABLE ),
                   NPA_LOG_CREATE (
                     ULOG_RT_PRINTF_1( npa.log, 
                                       "\tFAILED npa_disable_node \"%s\""
                                       "does not have NPA_NODE_DISABLEABLE "
                                       "attribute defined",
                                       graph_node->name ) ) );

  CORE_VERIFY( npa_node_equal( user_node, graph_node, ~NPA_NODE_DISABLEABLE ) );
  CORE_VERIFY( !npa_node_has_attribute( graph_node, NPA_NODE_NO_LOCK ) );

  npa_lock( resource->node_lock );

  for ( i = 0; i < graph_node->resource_count; i++ )
  {
    CORE_LOG_VERIFY( NULL == graph_node->resources[i].query_fcn,
                     NPA_LOG_CREATE (
                       ULOG_RT_PRINTF_1( 
                         npa.log,
                         "\tFAILED: can't disable resource \"%s\" "
                         "it has non-NULL query function",
                         graph_node->resources[i].name ) ) );

    graph_node->resources[i].query_fcn = npa_disabled_query_fcn;

    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_1( npa.log, 
                        "\tdisable resource (resource: \"%s\")",
                        graph_node->resources[i].name ) );
  }
  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_1( npa.log, 
                      "\tend npa_disable_node (node: \"%s\")",
                      graph_node->name ) );
}


void npa_define_marker( const char *marker_name )
{
  ULOG_RT_PRINTF_1( npa.log, "npa_define_marker (marker: \"%s\") ",
                      marker_name);
  npa_link_resource( marker_name, &marker_resource );

  /* Invoke any resource available callbacks */
  npa_cleanup_placeholders();
}

void npa_assign_resource_state( npa_resource       *resource,
                                npa_resource_state  new_state )
{
  CORE_VERIFY_PTR( resource );

  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_3( npa.log, 
                      "npa_assign_resource_state "
                      "(handle: 0x%08x) "
                      "(resource: \"%s\") "
                      "(active state: %d)", 
                      resource,
                      resource->definition->name,
                      new_state ) );

  npa_lock( resource->node_lock );
  npa_update_resource_state( resource, new_state );
  npa_unlock( resource->node_lock );
}

void npa_change_resource_plugin( const char                *resource_name,
                                 const npa_resource_plugin *plugin )
{
  npa_client   *client;
  npa_resource *resource = npa_find_resource( resource_name );

  CORE_VERIFY_PTR( resource );

  /* If the plugin is NULL, restore the definition */
  if ( NULL == plugin )
  {
    plugin = resource->definition->plugin;
  }

  /* Make sure all the original supported clients continue to be supported */
  CORE_VERIFY( 
    ( resource->definition->plugin->supported_clients & 
      plugin->supported_clients ) == 
    resource->definition->plugin->supported_clients );

  client = resource->clients;

  npa_lock( resource->node_lock );
  while ( NULL != client )
  {
    /* plugin is being replaced - call the old plugin's client destroy
     * function */
    if ( NULL != resource->active_plugin->destroy_client_fcn )
    {
      resource->active_plugin->destroy_client_fcn( client );
    }
    /* plugin is being replaced - call the new plugins's client create
     * function */
    if ( NULL != plugin->create_client_fcn )
    {
      plugin->create_client_fcn( client );
      CORE_VERIFY( client->resource == resource );
    }

    client = client->next;
  }

  resource->active_plugin = plugin;
  npa_unlock( resource->node_lock );
}

void npa_issue_limit_max_request( npa_client_handle   client,
                                  npa_resource_state  max )
{
  npa_resource       *resource;
  npa_resource_state  active_max;

  CORE_VERIFY_PTR( client );

  resource = client->resource;
  CORE_VERIFY_PTR( resource );

  active_max = resource->definition->max;

  NPA_PENDING_REQUEST(client).state = max;
  
  npa_lock( resource->node_lock );
  if ( ( max != 0 ) && ( max < resource->active_max ) )
  {
    resource->active_max = max;
  }
  /* TODO: Reinvesitgate this logic when requests are more than double
   * buffered */
  else if ( NPA_ACTIVE_REQUEST(client).state == resource->active_max )
  {
    /* There are ways to optimize this path, but it would involve
     * making the limit max requests be even more of a special case.
     * Not bothering with that unless this becomes a performance
     * problem */
    npa_client *max_client = resource->clients;

    while ( NULL != max_client )
    {
      if ( NPA_CLIENT_LIMIT_MAX == max_client->type )
      {
        npa_resource_state req = (max_client == client) ? 
          max : NPA_ACTIVE_REQUEST(max_client).state;

        if ( 0 != req && req < active_max )
        {
          active_max = req;
        }
      }
      max_client = max_client->next;
    }
    resource->active_max = active_max;
  }
  npa_unlock( resource->node_lock );

  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_5( npa.log,
                      "npa_issue_limit_max_request "
                      "(handle: 0x%08x) "
                      "(client: \"%s\") "
                      "(max: %d) "
                      "(resource: \"%s\") "
                      "(active_max: %d)",
                      client,
                      client->name, 
                      max, 
                      resource->definition->name,
                      active_max ) );

  client->issue_request( client, 
                         ( NPA_PENDING_REQUEST(client).state != 
                           NPA_ACTIVE_REQUEST(client).state ) );

  return;
}

void npa_issue_scalar_request( npa_client_handle   client, 
                               npa_resource_state  state )
{

  CORE_VERIFY_PTR( client );
  /* assert( npa_client_is_registered( resource, client ) */

  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_4( npa.log,
                      "npa_issue_required_request "
                      "(handle: 0x%08x) "
                      "(client: \"%s\") "
                      "(request: %u) "
                      "(resource: \"%s\")",
                      client,
                      client->name, 
                      state, 
                      client->resource_name ) );

  NPA_PENDING_REQUEST(client).state = state;

/*   NEXT_REQUEST(client).start_time = timetick_get(); */
  
  client->issue_request( client, NPA_ACTIVE_REQUEST(client).state != state );

  return;
}



/* Modify the existing resource request */
void npa_modify_required_request( npa_client_handle         client, 
                                  npa_resource_state_delta  delta )
{
  CORE_VERIFY_PTR( client );
  /* assert( npa_client_is_registered( resource, client ) */
  
  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_4( npa.log,
                      "npa_modify_request "
                      "(handle: 0x%08x) "
                      "(client: \"%s\") "
                      "(delta: %d) "
                      "(resource: \"%s\")",
                      client,
                      client->name, 
                      delta, 
                      client->resource_name ) );

  NPA_PENDING_REQUEST(client).state = NPA_ACTIVE_REQUEST(client).state + delta;
/*   NPA_PENDING_REQUEST(client).start_time = timetick_get(); */
  client->issue_request( client, delta != 0 );

  return;
}
void npa_issue_impulse_request( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );

  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_issue_impulse_request "
                      "(handle: 0x%08x)"
                      "(client: \"%s\") "
                      "(resource: \"%s\")",
                      client,
                      client->name, 
                      client->resource_name ) );

  client->issue_request( client, 1 );
}


void npa_issue_isoc_request( npa_client_handle   client,
                             npa_resource_time   deadline,
                             npa_resource_state  level_hint )
{
  CORE_UNIMPLEMENTED();
}

void npa_issue_vector_request( npa_client_handle   client, 
                               unsigned int        num_elems, 
                               npa_resource_state *vector )
{
  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( vector );

  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_5( npa.log,
                      "npa_issue_vector_request "
                      "(handle: 0x%08x)"
                      "(client: \"%s\") "
                      "(resource: \"%s\")"
                      "(num_elems: %d ) "
                      "(vector: 0x%08x)",
                      client, 
                      client->name, 
                      client->resource_name,
                      num_elems,
                      vector ) );

  NPA_PENDING_REQUEST(client).state = num_elems;
  NPA_PENDING_REQUEST(client).pointer.vector = vector;

#ifndef FEATURE_SYNC_REQUESTS_ONLY
  if ( npa_enqueue_async_request == client->issue_request )
  {
    client->continuation->data.client.vector = vector; 
    CORE_VERIFY_PTR( client->continuation );
    /* Save the stack pointer so we can check the client's version for
     * validity */
    client->issue_request( client, 1 );
  }
  else
#endif
  {
    client->issue_request( client, 1 );
    if ( NPA_ACTIVE_REQUEST(client).pointer.vector == vector )
    {
      NPA_ACTIVE_REQUEST(client).pointer.vector = NULL; 
    }
  }
}

void npa_issue_internal_request( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );

  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_issue_internal_request "
                      "(handle: 0x%08x)"
                      "(client: \"%s\") "
                      "(resource: \"%s\")",
                      client, 
                      client->name, 
                      client->resource_name ) );

  client->issue_request( client, 1 );
}


void npa_complete_request( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );
  
  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_complete_request "
                      "(handle: 0x%08x) "
                      "(client: \"%s\") "
                      "(resource: \"%s\") ",
                      client, 
                      client->name, 
                      client->resource_name ) );

  npa_terminate_request( client );
}


void npa_cancel_request( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );

  NPA_LOG_REQUEST (
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_cancel_request "
                      "(handle: 0x%08x) "
                      "(client: \"%s\") "
                      "(resource: \"%s\")",
                      client,
                      client->name, 
                      client->resource_name ) );

  npa_terminate_request( client );

  /* Execute the plugin cancel function (if defined)
   *
   * Note: This function is executed in a locked context.  The lock is
   * outside the check for the plugin's cancel function being non-NULL
   * to avoid a race condition on changing plugins */
  npa_lock( client->resource->node_lock );

  if ( NULL != client->resource->active_plugin->cancel_client_fcn )
  {
    client->resource->active_plugin->cancel_client_fcn( client );
  }
  npa_unlock( client->resource->node_lock );
}


npa_client_handle npa_create_sync_client_ex( const char       *resource_name,
                                             const char       *client_name, 
                                             npa_client_type   client_type,
                                             unsigned int      client_value,
                                             void             *client_ref )

{
  npa_client *client;

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_create_sync_client (resource: \"%s\") "
                      "(client: \"%s\") (client_type: %s)",
                      resource_name,
                      client_name, 
                      npa_client_type_name(client_type) ) );

  client = npa_new_client( resource_name, 
                           client_name, 
                           client_type,
                           client_value,
                           client_ref,                            
                           npa_issue_sync_request );

  if ( NULL != client )
  { 
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_4( npa.log,
                        "\tend npa_create_sync_client "
                        "(handle: 0x%08x) "
                        "(resource: \"%s\") "
                        "(client: \"%s\") "
                        "(client_type: %s) ",
                        client, 
                        client->resource_name,
                        client->name, 
                        npa_client_type_name(client_type) ) );
  }
  else
  {
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_3( npa.log,
                        "\tFAILED npa_create_sync_client (resource: \"%s\") "
                        "(client: \"%s\") (client_type: %s)",
                        resource_name,
                        client_name, 
                        npa_client_type_name(client_type) ) );
  }
  
  return client;
}

#ifndef FEATURE_SYNC_REQUESTS_ONLY
npa_client_handle npa_create_async_client_cb_ex( const char      *resource_name,
                                                 const char      *client_name, 
                                                 npa_client_type  client_type,
                                                 npa_callback     callback,
                                                 void            *context,
                                                 unsigned int     client_value,
                                                 void             *client_ref )

{
  npa_client *client;

  if ( NULL == npa.async_request_handle )
  {
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_3( npa.log,
                        "FAILED npa_create_async_client (resource: \"%s\") "
                        "(client: \"%s\") (client_type: %s):"
                        "NPA target configuration does not support async clients",
                        resource_name,
                        client_name, 
                        npa_client_type_name(client_type) ) );
    
    return NULL;
  }
  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_create_async_client (resource: \"%s\") "
                      "(client: \"%s\") (client_type: %s)",
                      resource_name,
                      client_name, 
                      npa_client_type_name(client_type) ) );

  client = npa_new_client( resource_name, 
                           client_name, 
                           client_type,
                           client_value,
                           client_ref,
                           npa_enqueue_async_request );

  if ( NULL != client )
  {    
    npa_continuation *continuation = client->continuation;

    CORE_DAL_VERIFY( DALSYS_EventCreate( DALSYS_EVENT_ATTR_WORKLOOP_EVENT, 
                                         &continuation->data.client.enqueue_event,
                                         NULL ) );
    
    /* TODO: Need sync object?  Can ignore? */
    CORE_DAL_VERIFY( DALSYS_AddEventToWorkLoop( npa.async_request_handle,
                                                npa_issue_async_request,
                                                client,
                                                continuation->data.client.enqueue_event,
                                                NULL ) );
    
    /* Set up callback */
    continuation->callback.callback = callback;
    continuation->callback.context  = context;

    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_4( npa.log,
                        "\tend npa_create_async_client "
                        "(handle: 0x%08x) "
                        "(resource: \"%s\") "
                        "(client: \"%s\") "
                        "(client_type: %s) ",
                        client, 
                        client->resource_name,
                        client->name, 
                        npa_client_type_name(client_type) ) );
  }
  else
  {
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_3( npa.log,
                        "\tFAILED npa_create_async_client (resource: \"%s\") "
                        "(client: \"%s\") (client_type: %s)",
                        resource_name,
                        client_name, 
                        npa_client_type_name(client_type) ) );
  }
  return client;
}
#endif

void npa_destroy_client( npa_client_handle client )
{
  npa_resource *resource;

  if( NULL == client) {return;}

  resource = client->resource;
  CORE_VERIFY_PTR( resource );

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_destroy_client "
                      "(handle: 0x%08x) "
                      "(client: \"%s\") "
                      "(resource: \"%s\")",
                      client,
                      0,
                      0 ) );

  /* Handle this request synchronously */
  client->issue_request = npa_issue_sync_request;
  
  /* Be sure to complete any pending work request */
  npa_complete_request( client );

  /* Remove client from inactive list */
  npa_lock( resource->node_lock );
  NPA_LIST_REMOVE( &resource->clients, client );
  npa_unlock( resource->node_lock );

  if ( NULL != client->continuation )
  {
    npa_continuation *continuation = client->continuation;

#ifndef FEATURE_SYNC_REQUESTS_ONLY
    if ( NULL != event )
    {
      CORE_DAL_VERIFY( DALSYS_DeleteEventFromWorkLoop( npa.async_request_handle, 
                                                       event ) );
      CORE_DAL_VERIFY( DALSYS_DestroyObject( event ) );
    }
#endif
    npa_destroy_continuation( continuation );
  }

  if ( NULL != resource->active_plugin->destroy_client_fcn )
  {
    resource->active_plugin->destroy_client_fcn( client );
  }

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_3( npa.log,
                      "\tend npa_destroy_client "
                      "(handle: 0x%08x) (client: \"%s\") (resource: \"%s\")",
                      client, client_name, resource_name ) );

  Core_PoolFree( &npa.client_pool, client );
}

npa_event_handle npa_create_event_cb( const char   *resource_name, 
                                      const char   *event_name,
                                      npa_event_trigger_type trigger_type,
                                      npa_callback  callback,
                                      void         *context )
{
  return npa_new_event( resource_name, event_name, 
                        trigger_type,
                        callback, context ); 
}

void npa_set_event_watermarks( npa_event_handle         event,
                               npa_resource_state_delta hi_watermark,
                               npa_resource_state_delta lo_watermark )
{
  npa_event_data  event_data;
  npa_resource   *resource;

  CORE_VERIFY_PTR( event );
  CORE_VERIFY( NPA_TRIGGER_WATERMARK_EVENT == event->trigger_type );

  resource = event->resource;
  CORE_VERIFY_PTR( resource );

  /* Initialize npa_event_data structure */
  npa_lock( resource->node_lock );
  event_data.resource_name = resource->definition->name;
  event_data.state         = resource->active_state;
  event_data.headroom      = resource->active_headroom;
  npa_unlock( resource->node_lock );

  /* Trigger the event if it's active */
  npa_lock( resource->event_lock );

  event->hi.watermark = hi_watermark;
  event->lo.watermark = lo_watermark;

  if ( lo_watermark <= hi_watermark ) 
  {
    if ( event_data.headroom < lo_watermark ) 
    {
      npa_trigger_callback_event( &event->callback,
                                  NPA_EVENT_LO_WATERMARK,
                                  &event_data, 
                                  sizeof( npa_event_data ) );
    }
    else if ( event_data.headroom > hi_watermark )
    {
      npa_trigger_callback_event( &event->callback,
                                  NPA_EVENT_HI_WATERMARK,
                                  &event_data, 
                                  sizeof( npa_event_data ) );
    }
  }
  npa_unlock( resource->event_lock );
}

void npa_set_event_thresholds( npa_event_handle   event,
                               npa_resource_state lo_threshold,
                               npa_resource_state hi_threshold )
{
  npa_event_data  event_data;
  npa_event_type  event_type;
  npa_resource   *resource;

  CORE_VERIFY_PTR( event );
  CORE_VERIFY( NPA_TRIGGER_THRESHOLD_EVENT == event->trigger_type );

  resource = event->resource;
  CORE_VERIFY_PTR( resource );

  /* Initialize npa_event_data structure */
  npa_lock( resource->node_lock );
  event_data.resource_name = resource->definition->name;
  event_data.state         = resource->active_state;
  event_data.headroom      = resource->active_headroom;
  npa_unlock( resource->node_lock );

  /* Trigger the event if it's active */
  npa_lock( resource->event_lock );

  event->hi.threshold = hi_threshold;
  event->lo.threshold = lo_threshold;

  if ( lo_threshold <= hi_threshold )
  {
    if ( event_data.state < lo_threshold )
    {
      event_type = NPA_EVENT_THRESHOLD_LO;
    }
    else if ( hi_threshold < event_data.state  )
    {
      event_type = NPA_EVENT_THRESHOLD_HI;
    }
    else
    {
      event_type = NPA_EVENT_THRESHOLD_NOMINAL;
    }

    npa_trigger_callback_event( &event->callback,
                                event_type,
                                &event_data, 
                                sizeof( npa_event_data ) );
  }
  npa_unlock( resource->event_lock );
}

void npa_destroy_event_handle( npa_event_handle event )
{
  npa_resource    *resource;

  CORE_VERIFY_PTR( event );
  resource = event->resource;

  CORE_VERIFY_PTR( resource );

  NPA_LOG_CREATE (
    ULOG_RT_PRINTF_2( npa.log,
                      "npa_destroy_event_handle "
                      "(event: \"%s\") (resource: \"%s\")",
                      event->name, resource->definition->name ) );

  /* Remove from element from the list */
  npa_lock( resource->event_lock );
  NPA_LIST_REMOVE( &resource->events, event );
  npa_unlock( resource->event_lock );

  Core_PoolFree( &npa.event_pool, event );
}


void npa_process_event( void )
{
  Core_WorkElementProcess( npa.event_queue_handle );
}

unsigned int npa_num_pending_events( void )
{
  return Core_WorkQueueLength( npa.event_queue_handle );
}

npa_query_handle npa_create_query_handle( const char * resource_name )
{
  CORE_VERIFY_PTR( resource_name );

  /* The query handle internally is just an alias to the
   * resource. Since it's just an alias, there's no real allocation
   * necessary.
   *
   * But the API exposes it as handle */
  return (npa_query_handle)npa_find_resource( resource_name );
}

void npa_destroy_query_handle( npa_query_handle query )
{
  return;
}

npa_query_status npa_query( npa_query_handle handle, 
                            uint32           id, 
                            npa_query_type  *result )
{
  npa_query_status  status  = NPA_QUERY_UNSUPPORTED_QUERY_ID;
  npa_resource     *resource = (npa_resource *) handle;

  /* Validate resource */
  if ( NULL == resource ) { return NPA_QUERY_UNKNOWN_RESOURCE; }
  if ( resource == &marker_resource ) { return status; }
  
  /* Give the resource a chance to handle the query */
  if ( NULL != resource->definition->query_fcn )
  {
    status = resource->definition->query_fcn( resource, 
                                              id, 
                                              result );
  }

  /* If the query has not been handled yet, process it */
  if ( NPA_QUERY_UNSUPPORTED_QUERY_ID == status )
  {
    status = NPA_QUERY_SUCCESS;


    switch ( id )
    {
    case NPA_QUERY_CURRENT_STATE:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->active_state;
      break;
    case NPA_QUERY_ACTIVE_MAX:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->active_max;
      break;
    case NPA_QUERY_RESOURCE_MAX:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->definition->max;
      break;
    case NPA_QUERY_RESOURCE_ATTRIBUTES:
      result->type = NPA_QUERY_TYPE_VALUE;
      result->data.state = resource->definition->attributes;
      break;
    case NPA_QUERY_NODE_ATTRIBUTES:
      result->type = NPA_QUERY_TYPE_VALUE;
      result->data.state = resource->node->attributes;
      break;
    default:
      result->type = NPA_QUERY_TYPE_REFERENCE;
      result->data.reference = NULL;
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
    }
  }

  return status;
}

npa_query_status npa_query_by_name( const char      *resource_name, 
                                    uint32           query_id, 
                                    npa_query_type  *query_result )
{
  return npa_query( (npa_query_handle)npa_find_resource( resource_name ), 
                    query_id, 
                    query_result );
}

npa_query_status npa_query_by_client( npa_client_handle client, 
                                      uint32            query_id, 
                                      npa_query_type  *query_result )
{
  CORE_VERIFY_PTR( client );
  return npa_query( (npa_query_handle)client->resource,
                    query_id, 
                    query_result );
}

npa_query_status npa_query_by_event( npa_event_handle event, 
                                     uint32           query_id, 
                                     npa_query_type  *query_result )
{
  CORE_VERIFY_PTR( event );
  return npa_query( (npa_query_handle)event->resource,
                    query_id, 
                    query_result );
}

npa_resource *npa_query_get_resource( npa_query_handle query_handle )
{
  return (npa_resource *)query_handle;
}

npa_query_status npa_query_resource_available( const char *resource_name )
{
  if ( npa_find_resource( resource_name ) )
  {
    return NPA_QUERY_SUCCESS;
  }
  else
  {
    return NPA_QUERY_UNKNOWN_RESOURCE;
  }
}

npa_user_data npa_find_extension( npa_extension_header *extension, 
                                  unsigned int          extension_id  )
{
  while ( ( NULL != extension ) && 
          ( extension_id != extension->extension_id ) )
  {
    extension = (npa_extension_header *)extension->next_extension;
  }
  return extension;
}

void npa_resources_available_continuation(   
  void         *user_data, 
  unsigned int  cb_event_type,
  void         *cb_data, 
  unsigned int  size )
{
  npa_continuation    *continuation =  (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );

  while ( ( continuation->data.resources.index < continuation->data.resources.num_resources ) &&
          ( NULL != npa_find_resource( 
            continuation->data.resources.resources[continuation->data.resources.index] ) ) )
  {
    continuation->data.resources.index++;
  }

  if ( continuation->data.resources.index == continuation->data.resources.num_resources )
  {
    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_2( npa.log, "  npa_resources_available complete "
                        "(callback: 0x%08x) "
                        "(context: 0x%08x) ",
                        continuation->callback.callback,
                        continuation->callback.context ) );

    npa_trigger_callback_event( &continuation->callback, 
                                cb_event_type, 
                                (void *)continuation->data.resources.resources, 
                                continuation->data.resources.num_resources );
    npa_destroy_continuation( continuation );
  }
  else 
  {
    const char *resource =  
      continuation->data.resources.resources[continuation->data.resources.index++];

    NPA_LOG_CREATE (
      ULOG_RT_PRINTF_3( npa.log, "  npa_resources_available waiting on resource "
                        "(resource: \"%s\") "
                        "(callback: 0x%08x) "
                        "(context: 0x%08x) ",
                        resource,
                        continuation->callback.callback,
                        continuation->callback.context ) );


    npa_resource_available_cb( resource, 
                               npa_resources_available_continuation,
                               continuation );
  }

}

void npa_resources_available_cb( unsigned int    num_resources,
                                 const char     *resources[],
                                 npa_callback    callback,
                                 void           *context )
{
  npa_continuation *continuation;
 
  CORE_VERIFY( ( 0 == num_resources ) || ( NULL != resources ) );
  CORE_VERIFY_PTR( callback );
  continuation = npa_new_continuation( callback, context );

  /* Initialize this continuation's state */
  continuation->data.resources.num_resources = num_resources;
  continuation->data.resources.resources     = resources;
  continuation->data.resources.index         = 0;

  /* Trigger the continuation directly for the first time */
  npa_resources_available_continuation( continuation, 0, NULL, 0 );
  return;
}

void npa_resource_available_cb( const char     *resource_name,
                                npa_callback    callback,
                                npa_user_data   context )
{
  npa_resource *placeholder;
  CORE_VERIFY_PTR( resource_name );
  CORE_VERIFY_PTR( callback );

  npa_lock( npa.graph_lock );

  placeholder = npa_get_placeholder_resource( resource_name );

  if ( NULL != placeholder )
  {
    CORE_VERIFY_PTR( npa_new_event( resource_name,
                                    "Resource Available Callback",
                                    NPA_TRIGGER_RESOURCE_AVAILABLE,
                                    callback, context ) );
    npa_unlock( npa.graph_lock );
  }
  else 
  {
    npa_callback_event cb_event;
    cb_event.callback = callback;
    cb_event.context = context;

    npa_unlock( npa.graph_lock );
    /* Trigger the event outside the lock */
    npa_trigger_callback_event( &cb_event,      
                                0, 
                                (void *)resource_name, 
                                strlen( resource_name ) );
  }
  return;
}

/***********************************************************************
 * DAL Helper Functions
 *
 * Callback to trigger DAL event support from general callback
 *    mechanism. I would like to move these out of NPA proper and into
 *    a support library.
 *
 *********************************************************************** */
/* Utility function for triggering DAL Events on top of callback interface */
void npa_dal_event_callback( void         *context,
                             unsigned int  event_type, 
                             void         *data,       
                             unsigned int  data_size )
{
  DALSYSEventHandle dal_event = (DALSYSEventHandle)context;
  CORE_VERIFY_PTR( dal_event );
  CORE_DAL_VERIFY( DALSYS_EventCtrlEx( dal_event,
                                       DALSYS_EVENT_CTRL_TRIGGER,
                                       event_type,
                                       (int *)data, 
                                       data_size ) );
  return;
}

void npa_resource_lock( npa_resource *resource )
{
  CORE_VERIFY_PTR( resource );

  npa_lock( resource->node_lock );
  ULOG_RT_PRINTF_2( npa.log, "npa_resource_lock "
                    "(resource: \"%s\") (sequence: 0x%08x)",
                    resource->definition->name, resource->sequence );
}

void npa_resource_unlock( npa_resource *resource )
{
  CORE_VERIFY_PTR( resource );
  npa_unlock( resource->node_lock );

  ULOG_RT_PRINTF_1( npa.log, "npa_resource_unlock "
                    "(resource: \"%s\")", resource->definition->name );
}

