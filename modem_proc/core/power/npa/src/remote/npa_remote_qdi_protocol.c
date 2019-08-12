/*==============================================================================
@file npa_remote_qdi_protocol.c

NPA QDI Remote protocol

Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/power/npa/src/remote/npa_remote_qdi_protocol.c#1 $
==============================================================================*/
#include <qurt.h>
#include "CoreVerify.h"
#include "CoreHeap.h"
#include "npa.h"
#include "npa_resource.h"
#include "npa_remote_resource.h"
#include "npa_remote_qdi_protocol.h"

#define NPA_QDI_REMOTE_PROTOCOL_NAME "/protocol/qurt/qdi"

typedef struct npa_qdi_remote_resource_data
{
  char name[NPA_MAX_STR_LEN + 1];
  char units[NPA_QDI_REMOTE_MAX_RESOURCE_UNITS_LEN + 1];
} npa_qdi_remote_resource_data;

static int npa_qdi_drv_hdl;

static npa_resource_state 
npa_qdi_remote_update_fcn( npa_resource *resource, npa_client* client )
{
  int ret;
  npa_resource_state state;

  CORE_VERIFY_PTR( resource );
  CORE_VERIFY_PTR( client );

  state = NPA_PENDING_REQUEST(client).state;

  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE:
    {
      ret = qurt_qdi_handle_invoke( (int)client->resource_data,
                                    NPA_QDI_ISSUE_SCALAR_REQUEST, state );
      CORE_VERIFY( ret == 0 );
      break;
    }
  case NPA_CLIENT_LIMIT_MAX:
    {
      ret = qurt_qdi_handle_invoke( (int)client->resource_data,
                                    NPA_QDI_ISSUE_LIMIT_MAX_REQUEST, state );
      CORE_VERIFY( ret == 0 );
      state = resource->request_state;
      break;
    }
  default:
    {
      CORE_UNIMPLEMENTED();
    }
  }
  return state;
}

static unsigned int 
npa_qdi_remote_create_client_ex_fcn(npa_client   *client,
                                    unsigned int  client_value, 
                                    void         *client_ref)
{
  int ret, remote_handle;

  CORE_VERIFY_PTR(client);
  
  ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl, 
                               NPA_QDI_CREATE_SYNC_CLIENT_EX, 
                                client->resource_name,
                                client->name, client->type,
                                client_value, client_ref,
                                &remote_handle );
  if ( ret != 0 )
  { 
    return NPA_ERROR;
  }
  
  client->resource_data = (npa_user_data)remote_handle;
  return NPA_SUCCESS;
}

static void npa_qdi_remote_destroy_client_fcn( npa_client *client )
{
  CORE_VERIFY_PTR(client);
  qurt_qdi_close( (int)client->resource_data );
}

const npa_resource_plugin npa_qdi_remote_plugin =
{
  npa_qdi_remote_update_fcn,
  NPA_CLIENT_REQUIRED|NPA_CLIENT_SUPPRESSIBLE|NPA_CLIENT_LIMIT_MAX,
  NULL,
  npa_qdi_remote_destroy_client_fcn,
  npa_qdi_remote_create_client_ex_fcn,
  NULL
};

static npa_resource_state 
npa_qdi_remote_driver_fcn( npa_resource      *resource,
                          npa_client_handle    client,
                          npa_resource_state   state)
{
  CORE_VERIFY_PTR( resource );
  CORE_VERIFY_PTR( client );

  if ( client->type == NPA_CLIENT_INITIALIZE )
  {
    if(0 == strncmp(NPA_QDI_REMOTE_PROTOCOL_NAME,
                    resource->definition->name,
                    sizeof(NPA_QDI_REMOTE_PROTOCOL_NAME)))
    {
      npa_qdi_drv_hdl = qurt_qdi_open("/dev/npa");
      CORE_VERIFY( npa_qdi_drv_hdl > 0 );
    }
    else
    {
      /* Create a query handle to be able to query the remote resource
         when requested */
      int remote_handle;
      CORE_VERIFY( 0 == qurt_qdi_handle_invoke( npa_qdi_drv_hdl, 
                                   NPA_QDI_CREATE_QUERY_HANDLE, 
                                   resource->definition->name,
                                                &remote_handle ) );
      resource->definition->data = (npa_user_data)remote_handle;
      resource->active_plugin = &npa_qdi_remote_plugin;
    }
  }
  
  return state;
}

static npa_query_status 
npa_qdi_remote_query_fcn( npa_resource   *resource,
                         unsigned int         query_id, 
                         npa_query_type      *query_result)
{
  int ret;

  CORE_VERIFY_PTR( resource );
  CORE_VERIFY_PTR( query_result );

  if ( query_id == NPA_QUERY_REMOTE_RESOURCE_AVAILABLE )
  {
    /* The Kernel/RootPD boots first, so all of its resources are available */
    npa_remote_resource_available( query_result->data.reference );
    return NPA_QUERY_SUCCESS;
  }

  ret = qurt_qdi_handle_invoke( (int)resource->definition->data,
                               NPA_QDI_QUERY,
                                query_id, query_result );
  CORE_VERIFY( ret >= 0 );
  
  return (npa_query_status)ret;
}

static npa_resource_definition npa_qdi_remote_protocol_node_resource[] = 
{ 
  {
    NPA_QDI_REMOTE_PROTOCOL_NAME,
    "na",
    0,
    &npa_no_client_plugin,
    NPA_RESOURCE_DEFAULT,
    NULL,
    npa_qdi_remote_query_fcn
  }
};

static npa_node_definition npa_qdi_remote_protocol_node = 
{ 
  "/node/protocol/qurt/qdi",
  npa_qdi_remote_driver_fcn, 
  NPA_NODE_DEFAULT,
  NULL,
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(npa_qdi_remote_protocol_node_resource)
};

void npa_qdi_remote_define_proxy_resources(void)
{
  int pid, ret;
  unsigned count = 0;
  npa_remote_resource_definition *resources = NULL;
  npa_qdi_remote_resource_data *resource_data = NULL;

  /* The driver handle should already be initialized */
  CORE_VERIFY( npa_qdi_drv_hdl > 0 );
  
  pid = qurt_getpid();
  CORE_VERIFY( pid > 0 && pid <= 32 );
  
  /* Since the kernel PD boots first, we assume that all kernel resources have
     been created and published */
  ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl,
                               NPA_QDI_GET_PUBLISHED_RESOURCE_COUNT,
                                pid, &count );
  CORE_VERIFY( ret == 0 );
  
  if(count > 0)
  {
    int i;
    resources = (npa_remote_resource_definition *)
                Core_Malloc(sizeof(npa_remote_resource_definition) * count);
    CORE_VERIFY_PTR(resources);
    resource_data = (npa_qdi_remote_resource_data *)
                    Core_Malloc(sizeof(npa_qdi_remote_resource_data) * count);
    CORE_VERIFY_PTR(resource_data);
    
    for(i = 0; i < count; i++)
    {
      resources[i].local_resource_name = resource_data[i].name;
      resources[i].remote_resource_name = resource_data[i].name;
      resources[i].protocol_type = NPA_QDI_REMOTE_PROTOCOL_NAME;
      resources[i].plugin = &npa_qdi_remote_plugin;
      resources[i].units = resource_data[i].units;
    }
  
    ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl,
                                 NPA_QDI_GET_PUBLISHED_RESOURCE_DEFINITION,
                                  pid, count, resources );
    CORE_VERIFY( ret == 0 );
    
    for(i = 0; i < count; i++)
    {
      npa_remote_define_resource_cb(&resources[i], 0, NULL, NULL);
    }
  }
}

void npa_qdi_remote_init(void)
{
  npa_define_node_cb(&npa_qdi_remote_protocol_node, NULL, NULL, NULL);
  npa_qdi_remote_define_proxy_resources();
}