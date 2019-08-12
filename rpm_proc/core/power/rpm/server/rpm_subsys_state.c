/*===========================================================================

  sussys_state.c - resource used to share sub system state information within the RPM

  Copyright (c) 2013 - 2015 Qualcomm Technologies Incorporated
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rpm_subsys_state.h"
#include "comdef.h"
#include <stdlib.h>
#include "time_service.h"
#include "rpmserver.h"
#include "rpm_config.h"
#include "CoreVerify.h"


/*=========================================================================
      Prototypes
==========================================================================*/
npa_resource_state subsys_apply
(
  struct npa_resource *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState
);

typedef struct
{
  npa_resource_definition Resource;
  npa_node_definition     Node;
} NPAResourceType;

typedef struct
{
  NPAResourceType         Subsys_State_Node;
} StateResourceType;
/*=========================================================================
      External references
==========================================================================*/

extern int rpm_npa_get_client(const npa_client* client);

/*=========================================================================
      Data
==========================================================================*/
rpm_npa_adapter state_adapter;

// Capture sub system state 
unsigned int current_subsys_state;

/*
 * Define the subsystem power collapse State resources.
 */
 
StateResourceType State_Resource =
{
  .Subsys_State_Node =
  {
    /* State Resource */
    .Resource =
    {
      "/server/sys_pc_state",                                             /* Name of resource */
      "stat",                                                         /* Units for this resource */
      0xF,                                                            /* Maximum state */
      &npa_bitwise_update_plugin,                                     /* Plugin */
      NPA_RESOURCE_REMOTE_ACCESS_ALLOWED|NPA_RESOURCE_DRIVER_UNCONDITIONAL, /* Resource Attributes */
    },

    /* sub system state Node */
    .Node =
    {
      "/node/server/sys_pc_state",                                  /* Name */
      subsys_apply,                                                 /* Driver function */
      NPA_RESOURCE_DEFAULT,                                         /* Attributes */
      NULL,                                                         /* Data */
      NPA_EMPTY_ARRAY,                                              /* Dep array */
      1,
      &State_Resource.Subsys_State_Node.Resource                   /* Resource Array */
    }
  },
};

/*===========================================================================

  FUNCTION      subsys_apply

  DESCRIPTION   This function is used for update state variable.
                Currently, Clk_bimc code will need current sub system state info. 

  PARAMETERS    pResource [in] - State Resource
                nState [in]    - Request state.

  DEPENDENCIES  None.

  RETURN VALUE  New subsystem state

  SIDE EFFECTS  None.

===========================================================================*/

npa_resource_state subsys_apply
(
  struct npa_resource *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState
)
{
  int client_id = -1;
  
  if(hClient->type != NPA_CLIENT_REQUIRED)
  {
    return NPA_NO_CLIENT;
  }
  
  client_id = rpm_npa_get_client(hClient);
  
  CORE_VERIFY(client_id != -1);
  CORE_VERIFY(client_id < rpm->num_ees);
  
  if(nState)
  {
	current_subsys_state |= (1 << client_id); 
  }
  else
  {
	current_subsys_state &= ~(1 << client_id); 
  }
	
  return (npa_resource_state)current_subsys_state;
}

/*===========================================================================

  FUNCTION      rpm_subsys_pc_init

  DESCRIPTION   This function is used setup State node 

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None

  SIDE EFFECTS  None.

===========================================================================*/
void rpm_subsys_pc_init(void)
{
  npa_resource_state  nInitialRequest;
  state_adapter = rpm_create_npa_adapter(RPM_SUBSYS_STATE_REQ, 1); 
  
  rpm_associate_npa_resource(state_adapter, 0, State_Resource.Subsys_State_Node.Resource.name);
  
  nInitialRequest = 0;
  npa_define_node(&State_Resource.Subsys_State_Node.Node, &nInitialRequest, NULL);
  
}







