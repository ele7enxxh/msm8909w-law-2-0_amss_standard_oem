/*==============================================================================

FILE:      npa_plugins.c

DESCRIPTION: NPA standard plugin library

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2009-2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/npa/src/npa_plugins.c#3 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/14/10   sg      Added suppressible requests
07/07/10   sg      Added OR plugin
04/20/10   sg      Added impulse and no_client plugins
10/20/09   sg      Remap NPA_MAX_STATE requests to actual resource max state
08/03/09   sg      Added Identity plugin
05/01/09   sg      Initial import

==============================================================================*/
#include "npa_resource.h"

#ifdef WINSIM
#include "npa_offline.h"
#endif /* WINSIM */

#include "CoreVerify.h"

extern int rpm_npa_get_client(const npa_client* client);

static unsigned int _internal_index( npa_client_handle client )
{
  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:     return NPA_REQUIRED_INDEX;
  case NPA_CLIENT_SUPPRESSIBLE: return NPA_SUPPRESSIBLE_INDEX;
  default: CORE_VERIFY( 0 );
  }
  return 0;
}

npa_resource_state npa_min_update_fcn( npa_resource      *resource,
                                              npa_client_handle  client)
{
  npa_resource_state   request, active_min, pending_min;
  unsigned int         index;

  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE: 
    index = _internal_index( client );
    pending_min = active_min = resource->internal_state[index];

    NPA_REMAP_MAX_REQUEST( resource, client );
    request = NPA_PENDING_REQUEST(client).state;

    if ( ( request != 0 ) &&
         ( ( pending_min == 0 ) || request < pending_min ) )
    {
      pending_min = request;
    } 
    else if ( (active_min == NPA_ACTIVE_REQUEST(client).state) || 
              (request == 0) )
    {
      npa_client_handle active = resource->clients;
      pending_min = request;
    
      /* Scan the pending list for a new minimum */
      while ( NULL != active )
      {
        if ( active != client && client->type == active->type ) 
        {
          request = NPA_ACTIVE_REQUEST(active).state;
          if ( ( request != 0 ) &&
               ( ( pending_min == 0 ) || request < pending_min ) )
          {
            pending_min = request;
          }
        }
        active = active->next;
      }
    }
    resource->internal_state[index] = pending_min;
  default:
    break;
  }

  if ( 0 == NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource) )
  {
    return NPA_RESOURCE_REQUIRED_REQUEST(resource);
  }
  else if ( 0 == NPA_RESOURCE_REQUIRED_REQUEST(resource) )
  {
    return NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource);
  }
  else 
  {
    return MIN( NPA_RESOURCE_REQUIRED_REQUEST(resource),
                NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource) );
  }
}

npa_resource_state npa_max_update_fcn( npa_resource      *resource,
                                              npa_client_handle  client )
{
  npa_resource_state   request, active_max, pending_max;
  unsigned int index;

  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE:
    index = _internal_index( client );
    pending_max = active_max = resource->internal_state[index];
  
    NPA_REMAP_MAX_REQUEST( resource, client );
    request = NPA_PENDING_REQUEST(client).state;
    if ( request > pending_max )
    {
      pending_max = request;
    } 
    else if ( active_max == NPA_ACTIVE_REQUEST(client).state )
    {
      npa_client_handle active = resource->clients;
      pending_max = NPA_PENDING_REQUEST(client).state;
    
      /* Scan the pending list for a new maximum */
      while ( NULL != active )
      {
        if ( client != active && client->type == active->type )
        {
          request = NPA_ACTIVE_REQUEST(active).state;
          if ( request > pending_max )
          {
            pending_max = request;
          }
        }
        active = active->next;
      }
    }
    resource->internal_state[index] = pending_max;
    break;
  default:
    break;
  }

  return MAX( NPA_RESOURCE_REQUIRED_REQUEST(resource), 
              NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource ) );
}

npa_resource_state npa_sum_update_fcn( npa_resource      *resource,
                                              npa_client_handle  client )
{
  unsigned int index;

  if ( NULL != client )
  {
    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      index = _internal_index( client );
      NPA_REMAP_MAX_REQUEST( resource, client );
      resource->internal_state[index] -= NPA_ACTIVE_REQUEST(client).state;
      resource->internal_state[index] += NPA_PENDING_REQUEST(client).state;
      break;
    default:
      break;
    }
  }
  return 
    NPA_RESOURCE_REQUIRED_REQUEST(resource) + NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource );
}

npa_resource_state npa_binary_update_fcn( npa_resource      *resource,
                                                 npa_client_handle  client )
{
  unsigned int index;

  if ( NULL != client )
  {
    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      index = _internal_index( client );
      resource->internal_state[index+2] -= (NPA_ACTIVE_REQUEST(client).state  == 0) ? 0 : 1;
      resource->internal_state[index+2] += (NPA_PENDING_REQUEST(client).state == 0) ? 0 : 1;
      resource->internal_state[index] = 
        ( resource->internal_state[index+2] ) ? resource->active_max : 0;
      break;
    default:
      break;
    }
  }
  

  return MAX( NPA_RESOURCE_REQUIRED_REQUEST(resource),
              NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource) );
}

npa_resource_state npa_or_update_fcn( npa_resource       *resource,
                                             npa_client_handle   client)
{
  if ( NULL != client )
  {
    npa_resource_state request = NPA_PENDING_REQUEST(client).state;
    npa_client_handle  active  = resource->clients;
    unsigned int       index   = _internal_index( client );

    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      while ( NULL != active )
      {
        if ( ( active != client ) && ( client->type == active->type ) )
        {
          request |= NPA_ACTIVE_REQUEST(active).state;
        }
        active = active->next;
      }
      break;
    default:
      break;
    }
    resource->internal_state[index] = request;
  }
  
  return 
    (NPA_RESOURCE_REQUIRED_REQUEST(resource) | NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource));
}

npa_resource_state npa_bitwise_update_fcn( npa_resource       *resource,
                                           npa_client_handle   client)
{
  
  if ( NULL != client )
  {
    npa_resource_state request = NPA_PENDING_REQUEST(client).state;
    npa_client_handle  active  = resource->clients;
    unsigned int       index   = _internal_index( client );
    int client_id =-1;
	
    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      while ( NULL != active )
      {
        if ( ( active != client ) && ( client->type == active->type ) )
        {
		  client_id = rpm_npa_get_client(active);
		  
          if(NPA_ACTIVE_REQUEST(active).state)
		  {
		    request |= (1 << client_id);
		  }
		  else
		  {
		    request &= ~(1 << client_id);
		  }
        }
        active = active->next;
      }
      break;
    default:
      break;
    }
    resource->internal_state[index] = request;
  return NPA_PENDING_REQUEST(client).state;	
  }
  
  return 
    (NPA_RESOURCE_REQUIRED_REQUEST(resource));    
}

npa_resource_state npa_identity_update_fcn( npa_resource      *resource,
                                                   npa_client_handle  client )
{
  if( resource->definition->attributes & NPA_RESOURCE_VECTOR_STATE )
  {
    unsigned int vector_size;

    CORE_VERIFY( NPA_PENDING_REQUEST(client).state <= resource->definition->max );      
    resource->active_max = NPA_PENDING_REQUEST(client).state;
    
    resource->request_state = NPA_PENDING_REQUEST(client).state;
    vector_size = resource->request_state * sizeof( npa_resource_state );
      
    /* Should use strlcpy() instead if available */
    memcpy( resource->state_vector, NPA_PENDING_REQUEST(client).pointer.vector, vector_size );
      
    return resource->request_state;
  }
  else
  {
    return NPA_PENDING_REQUEST(client).state;
  }
}

npa_resource_state npa_always_on_update_fcn( npa_resource      *resource,
                                                    npa_client_handle  client )
{
  return resource->active_max;
}

npa_resource_state npa_impulse_update_fcn( npa_resource      *resource,
                                                  npa_client_handle  client )
{
  return resource->request_state;
}

npa_resource_state npa_no_client_update_fcn( npa_resource      *resource,
                                                    npa_client_handle  client )
{
  /* This function should never be invoked */
  CORE_VERIFY(0);
  return 0;
}


/*======================================================================
  Default Plugins 
======================================================================*/
const npa_resource_plugin npa_binary_plugin =
{
  npa_binary_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_min_plugin = 
{
  npa_min_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_max_plugin = 
{
  npa_max_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_sum_plugin = 
{
  npa_sum_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_identity_plugin = 
{
  npa_identity_update_fcn,
  0xffffffff,                   /* All client types supported */
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_always_on_plugin = 
{
  npa_always_on_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};


const npa_resource_plugin npa_impulse_plugin = 
{
  npa_impulse_update_fcn,
  /* Supported client types */
  NPA_CLIENT_IMPULSE,
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_or_plugin = 
{
  npa_or_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_bitwise_update_plugin = 
{
  npa_bitwise_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

/* Resource supports no clients - used for markers */
static void npa_no_client_create_fcn ( npa_client *client )
{
  client->resource = NULL; /* Signal that this client can't be created */
  client->resource_data = 0;    /* Set error code */
}

const npa_resource_plugin npa_no_client_plugin =
{
  npa_no_client_update_fcn,
  /* Supported client types */
  0,
  npa_no_client_create_fcn,     /* Create client function */
  NULL                          /* Destroy client function */
};

