/*============================================================================
  FILE:         node_definition_tcm.c

  OVERVIEW:     This file provides the NPA node definition for the
                /core/cpu/tcm node. This node is used to control if
                target will do power collapse with tcm retention or
                no retention.

  DEPENDENCIES: None

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/npa_nodes/node_definition_tcm.c#1 $
$DateTime: 2016/12/13 07:59:23 $
============================================================================*/

#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "sleep_lpr.h"
#include "CoreVerify.h"

/* ==================================================================
              INTERNAL FUNCTION DECLARATIONS
   ================================================================== */

/**
 * @brief sleep_tcm_driver
 *
 * Driver function for the tcm node. This function gets called
 * every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return: New state of the resource.
 */
static npa_resource_state sleep_tcm_driver ( npa_resource *resource,
                                             npa_client *client,
                                             npa_resource_state state )
{
  /* Get the TCM resources's client handle to the sleep LPR node. */
  npa_client_handle tcm_lpr_client = NPA_DEPENDENCY( resource, 0 );

  /* At init time, register the LPR with the sleep LPR node */
  if( client->type == NPA_CLIENT_INITIALIZE )
  {
    sleep_define_lpr_str( "tcm", tcm_lpr_client );
  }

  if ( state )
  {
    /* Disable noret LPRM. Only ret LPRM is enabled */
    npa_issue_required_request( tcm_lpr_client, ~SLEEP_LPRM_NUM(0) );
  }
  else
  {
    /* TCM Memory can be powered OFF */
    npa_issue_required_request( tcm_lpr_client, SLEEP_ALL_LPRMS );
  }

  return state;
}

/* ==================================================================
                        NODE DEFINITION
   ================================================================== */

/**
 * @brief sleep_tcm_node_dependency
 *
 * The dependency array for the tcm node.  This node depends
 * on the sleep LPR node, as it makes requests to that node when the state
 * of this node changes.
 */
static npa_node_dependency sleep_tcm_node_dependency[1] =
{
  {
    SLEEP_LPR_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/**
 * @brief sleep_tcm_resource
 *
 * Resource definition for the /core/cpu/tcm resource.
 */
static npa_resource_definition sleep_tcm_resource[] =
{
  {
    "/core/cpu/tcm",      /* Name */
    "on/off",             /* Units */
    1,                    /* Max State */
    &npa_binary_plugin,   /* Plugin */
    NPA_RESOURCE_DEFAULT, /* Attributes */
    NULL,                 /* User Data */
  }
};

/**
 * @brief sleep_tcm_node
 *
 * Node definition for the /core/cpu/tcm resource.
 */
npa_node_definition sleep_tcm_node =
{
  "/node/core/cpu/tcm",                 /* Name */
  sleep_tcm_driver,                     /* Driver_fcn */
  NPA_NODE_DEFAULT,                     /* Attributes */
  NULL,                                 /* Data */
  NPA_ARRAY(sleep_tcm_node_dependency), /* Dependencies */
  NPA_ARRAY(sleep_tcm_resource)         /* Resources */
};
