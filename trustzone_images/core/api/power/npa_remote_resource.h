#ifndef NPA_REMOTE_RESOURCE_H
#define NPA_REMOTE_RESOURCE_H

/*!
  @file
    npa_remote_resource.h

  @brief
    This file contains the NPA remote resource external data structures and
    APIs.
*/
/*==============================================================================
  npa_remote_resource.h

  NPA Remote Resource API 

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/npa_remote_resource.h#1 $ */

/** \defgroup resource NPA Remote Resource
 *
 * This file contains the NPA remote resource external data structures and
 * APIs.
 *
 */
/*@{*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "npa_resource.h"
#include "npa_remote_protocol.h"
#include "npa_extension.h"

/** 
 * If defining a remote node/resource using these driver/update 
 * functions, the resource must include the
 * npa_remote_resource_extension 
 */ 
typedef struct npa_remote_resource_extension
{
  npa_extension_header        header;
  char                       *remote_resource_name;
  char                       *protocol_type;
  npa_remote_client_handle    client;   /**< -- Initialize as NULL */
  npa_remote_protocol        *protocol; /**< --  Initialize as NULL */
} npa_remote_resource_extension;




/** Definition of a basic remote resource */
typedef struct npa_remote_resource_definition
{
  char                      *local_resource_name;
  char                      *remote_resource_name;
  char                      *protocol_type;
  const npa_resource_plugin *plugin;
  npa_resource_driver_fcn    driver_fcn; 
  const char                *units; /**< -- Ideally would be queried from remote resource */         
  npa_resource_state         max;           
  unsigned int               attributes;
} npa_remote_resource_definition;


/**
   @brief Define a local resource that uses a remote resource.

   This function will allocate memory for and construct all of 
   the necessary NPA structures needed to create a remoted 
   resource and then invoked npa_remote_define_node. 

   The local resource will not be created until the remote resource
   is accessible. The define_event can be used to determine when the
   local resource is available.

   Note: If the remote resource is not remotely accessible (does not
   have define NPA_RESOURCE_REMOTE_ACCESS_ALLOWED attrbiute), the
   local resource will never be created.

   Note: If the local resource is doing remote aggregatation (every
   local request is forwarded to the remote resource), the initial
   state must be zero.  

   @param[in] resource_defn : definition of the remote resource

   @param[in] initial_state : initial state of the local 
         resource

   @param[in] define_event : optional event to be triggered when
   local resource is defined 

   @return None 
  
   @sideeffect  None

   @dependencies None
 
   @ingroup resource
 */
void npa_remote_define_resource( npa_remote_resource_definition *remote_defn,
                                 npa_resource_state              initial_state,
                                 DALSYSEventHandle               define_event );

/**

   @brief Initialize the remote resource interface 
    
   @return None 

   @sideeffect  None

   @dependencies None
 
   @ingroup resource    

*/
void npa_remote_resource_init( void );


/**

   @brief Define a local node that uses a remote node.

   Currently only simple nodes (single resource) are supported. The
   resource must include the npa_remote_resource_extension as part of
   the resource's user_data.

   The local node/resource will not be created until the remote node
   is accessible. The define_event can be used to determine when the
   local node is available.

   Note: If the remote resource is not remotely accessible (does not
   have define NPA_RESOURCE_REMOTE_ACCESS_ALLOWED attrbiute), the
   local resource will never be created.

   Note: If the local resource is doing remote aggregataion (every
   local request is forwarded to the remote resource), the initial
   state must be zero.  

   @param[in] node : definition of the local nodename of the 
   resource that is to be used 

   @param[in] initial_state : initial state of the local 
         resource

   @param[in] define_event : optional event to be triggered when 
   local resource is defined 

   @return None

   @sideeffect  None

   @dependencies None
 
   @ingroup resource    
*/
void npa_remote_define_node( npa_node_definition *node, 
                             npa_resource_state   initial_state[],
                             DALSYSEventHandle    define_event );




/**
   @brief Driver function to use for a resource that locally 
   aggregates requests and then sends a single, aggregated 
   request to the remote node. 

   This is a standard driver function

   @param[in] resource : pointer to local resource

   @param[in] client : client current issuing request

   @param[in] state : aggregated request state 
    
   @return npa_resource_state  
    
   @sideeffect  None

   @dependencies None

   @ingroup resource        
*/
npa_resource_state 
npa_remote_resource_local_aggregation_driver_fcn( 
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state 
  );

/**
   @brief Driver function to use for a resource that locally 
   aggregates vector requests and then sends a single, aggregated 
   vector request to the remote node. 

   This is a standard driver function

   @param[in] resource : pointer to local resource

   @param[in] client : client current issuing request

   @param[in] state : aggregated request state 
    
   @return npa_resource_state  
    
   @sideeffect  None

   @dependencies None

   @ingroup resource        
*/
npa_resource_state 
npa_remote_resource_local_aggregation_vector_driver_fcn (
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state 
  );

/**
   @brief Driver function to use for a resource that remotely 
          aggregates requests.

   If using this driver function, it is expected that the
   remote_aggregation update function/plugins isbeing used

   This is a standard driver function.

   @param[in] resource : pointer to local resource

   @param[in] client : client current issuing request

   @param[in] state : aggregated request state 
    
   @return npa_resource_state  
    
   @sideeffect  None

   @dependencies None
 
   @ingroup resource     
*/
npa_resource_state 
npa_remote_resource_remote_aggregation_driver_fcn (
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state 
  );

/**
   @brief Update function to use for a resource that remotely 
          aggregates requests.

   This function forwards each local request to the remote resource

   This is a standard update function.

   @param[in] resource : pointer to local resource

   @param[in] client : client current issuing request 
    
   @return npa_resource_state 

   @sideeffect  None

   @dependencies None
 
   @ingroup resource     
*/
npa_resource_state 
npa_remote_resource_remote_aggregation_update_fcn ( 
  struct npa_resource *resource,
  npa_client_handle    client
  );

/**
   @brief Function invoked on local client create. 

   These functions create remote client handles to the resource 
   that is being proxied locally.  The remote client handle is 
   stored in the local client's resource_data field .

   @param[in] client : client 
   @param[in] client_data : arbitrary client data argument, is ignored
   @param[in] client_ptr : arbitrary client ptr argument, is ignored 
    
   @return 0 if success, non-zero if failure
    
   @sideeffect will attempt to create remote client and will bind to
   local client using client->resource_data field

   @dependencies None
 
   @ingroup resource      
*/
unsigned int npa_remote_resource_create_client_fcn( npa_client  *client,
                                                    unsigned int client_data, 
                                                    void        *client_ptr );

/**
   @brief Function invoked on local client destroy 

   These functions destroy remote client handles to the resource
   that is being proxied locally.  The remote client handle is 
   stored in the local client's resource_data field 

   @param[in] client : client 
    
   @return None 
    
   @sideeffect  None

   @dependencies None
 
   @ingroup resource      
*/
void npa_remote_resource_destroy_client_fcn( npa_client *client );

/** 
    Standard plugin to use for resource using remote aggregation
    
    Note: It is currently an error to try to create a local client of
    a type unsupported by the remote resource. It is hoped that a
    future version will relax this condition and simply return a NULL
    client handle
*/
extern const npa_resource_plugin npa_remote_aggregation_plugin;

/**@} /group resource */

#endif /* NPA_REMOTE_RESOURCE_H */
