/*============================================================================
  FILE:         node_definition_rpm_sync.c

  OVERVIEW:     This file provides the NPA node definition for the
                /core/cpu/rpm_sync node.

  DEPENDENCIES: None

                Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "sleep_lpr.h"

/* ==================================================================
              INTERNAL FUNCTION DECLARATIONS
   ================================================================== */

/**
 *
 * @brief sleep_rpm_sync_driver
 *
 * Driver function for the rpm_sync node.  This function gets called
 * every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return: New state of the resource.
 *
*/
static npa_resource_state sleep_rpm_sync_driver ( npa_resource *resource,
                                                  npa_client *client,
                                                  npa_resource_state state )
{
  /* Get the node's client handle to the sleep LPR node. */
  npa_client_handle client_handle = NPA_DEPENDENCY( resource, 0 );

  /* At init time, register the LPR with the sleep LPR node, and request
   * the rpm LPR */
  if( client->type == NPA_CLIENT_INITIALIZE )
  {
     sleep_define_lpr_str( "rpm", client_handle );
  }

  if ( state )
  {
    /* Request "/sleep/lpr"" node to disable rpm.sync_only LPRM for rpm LPR */
    /* RPM LPRM 0 : "sync"      */ 
    /* RPM LPRM 1 : "sync_only" */
    npa_issue_required_request( client_handle, ~SLEEP_LPRM_NUM( 1 ) );
  }
  else
  {
    /* Aggregated zero request enables all the LPRMs for rpm */
    npa_issue_required_request( client_handle, SLEEP_ALL_LPRMS );
  }

  return state;
}

/* ==================================================================
                        NODE DEFINITION
   ================================================================== */

/**
 *
 * @brief sleep_lpr_dependency
 *
 * The dependency array for the rpm_sync node.  This node depends
 * on the sleep LPR node, as it makes requests to that node when the state
 * of this node changes.
 *
*/
static npa_node_dependency sleep_lpr_dependency[1] =
{
  {
    SLEEP_LPR_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/**
 *
 * @brief sleep_rpm_sync_resource
 *
 * Resource definition for the /core/cpu/rpm_sync resource.
 *
*/
static npa_resource_definition sleep_rpm_sync_resource[] =
{
  {
    "/core/cpu/rpm_sync", /* Name */
    "disable/enable",     /* Units */
    0x1,                  /* Max State */
    &npa_max_plugin,      /* Plugin */
    NPA_RESOURCE_DEFAULT, /* Attributes */
    NULL,                 /* User Data */
    NULL                  /* Query Function */
  }
};

/**
 *
 * @brief sleep_rpm_sync_node
 *
 * Node definition for the /core/cpu/rpm_sync resource.
 *
*/
npa_node_definition sleep_rpm_sync_node =
{
  "/node/core/cpu/rpm_sync",         /* Name */
  sleep_rpm_sync_driver,             /* Driver_fcn */
  NPA_NODE_DEFAULT,                  /* Attributes */
  NULL,                              /* Data */
  NPA_ARRAY(sleep_lpr_dependency),   /* Dependencies */
  NPA_ARRAY(sleep_rpm_sync_resource) /* Resources */
};
