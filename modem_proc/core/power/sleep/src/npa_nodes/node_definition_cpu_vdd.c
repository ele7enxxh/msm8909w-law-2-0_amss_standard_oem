/*============================================================================
  FILE:         node_definition_cpu_vdd.c

  OVERVIEW:     This file provides the NPA node definition for the
                /core/cpu/vdd node.

  DEPENDENCIES: None

                Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "sleep_lpr.h"

/* ==================================================================
                     FORWARD DECLARATIONS
   ================================================================== */

/*
 * Pull in the LPR & init function associated with this node.
*/
extern void cpu_vdd_lpr_init( void );

/* ==================================================================
              INTERNAL FUNCTION DECLARATIONS
   ================================================================== */

/**
 *
 * @brief sleep_cpu_vdd_driver
 *
 * Driver function for the cpu_vdd node.  This function gets called
 * every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return: New state of the resource.
 *
*/
static npa_resource_state sleep_cpu_vdd_driver ( npa_resource *resource,
                                                 npa_client *client,
                                                 npa_resource_state state )
{
  /* Get the modem_core_rail node's client handle to the sleep LPR node. */
  npa_client_handle client_handle = NPA_DEPENDENCY( resource, 0 );

  /* At init time, register the LPR with the sleep LPR node, and request
   * a bit mask to use for the MODEM_CORE_RAIL LPRMs from the sleep LPR node. */
  if( client->type == NPA_CLIENT_INITIALIZE )
  {
     sleep_define_lpr_str( "cpu_vdd", client_handle );
  }

  if ( state & 0x1 )
  {
    /* Client request of 0x1 disables all LPRMs for cpu_vdd
     * This vote should override other less restrictive requests. */
    npa_complete_request( client_handle );
  }
  else if ( state & 0x2 )
  {
    /* Client request of 0x2 *only* enables BHS based PC LPRM for cpu_vdd */
    /* LPRM order in lpr_definiton_cpu_vdd.xml is as follows: */
    /* SLEEP_LPRM_NUM(0) == pc_l2_tcm_noret */
    /* SLEEP_LPRM_NUM(1) == pc_l2_noret     */
    /* SLEEP_LPRM_NUM(2) == pc_l2_tcm_ret   */
    /* SLEEP_LPRM_NUM(3) == bhs_l2_tcm_ret  */
    npa_issue_required_request( client_handle, SLEEP_LPRM_NUM( 3 ) );
  }
  else
  {
    /* Request sleep node to enable ALL LPRMs for cpu_vdd */
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
 * The dependency array for the cpu_vdd node.  This node depends
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
 * @brief sleep_cpu_vdd_resource
 *
 * Resource definition for the /core/cpu/vdd resource.
 *
*/
static npa_resource_definition sleep_cpu_vdd_resource[] =
{
  {
    "/core/cpu/vdd",      /* Name */
    "bitmask",            /* Units */
    0x3,                  /* Max State */
    &npa_or_plugin,       /* Plugin */
    NPA_RESOURCE_DEFAULT, /* Attributes */
    NULL,                 /* User Data */
    NULL                  /* Query Function */
  }
};

/**
 *
 * @brief sleep_cpu_vdd_node
 *
 * Node definition for the /core/cpu/vdd resource.
 *
*/
npa_node_definition sleep_cpu_vdd_node =
{
  "/node/core/cpu/vdd",              /* Name */
  sleep_cpu_vdd_driver,              /* Driver_fcn */
  NPA_NODE_DEFAULT,                  /* Attributes */
  NULL,                              /* Data */
  NPA_ARRAY(sleep_lpr_dependency),   /* Dependencies */
  NPA_ARRAY(sleep_cpu_vdd_resource)  /* Resources */
};
