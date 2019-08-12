/*==============================================================================
  FILE:         node_definition_cpu_vdd.c

  OVERVIEW:     This file provides the NPA node definition for the
                /core/cpu/vdd node.
 
  DEPENDENCIES: None

                Copyright (c) 2011-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/npa_nodes/node_definition_cpu_vdd.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "sleep_lpr.h"

/*==============================================================================
                       INTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
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
     sleepLPR_define( "cpu_vdd", client_handle );
  }

  if ( 0x1 == state )
  {
    /* SLEEP_LPRM_NUM(0) -- "pc_noret" 
     * SLEEP_LPRM_NUM(1) -- "pc_l2_noret"
     * SLEEP_LPRM_NUM(2) -- "apcr_rail_lpm"
     *
     * correspond to the LPRMs in the "cpu_vdd" lpr xml file that 
     * allow cpu_vdd power rail to be collapsed. 
     * Disable above modes when clients request this resource */
    npa_issue_required_request( client_handle, 
                                ~( SLEEP_LPRM_NUM(0) | 
                                   SLEEP_LPRM_NUM(1) |
                                   SLEEP_LPRM_NUM(2) ) );
  }
  else  if ( 0x2 == state )
  {
    /* SLEEP_LPRM_NUM(0) -- "pc_noret" 
     * SLEEP_LPRM_NUM(1) -- "pc_l2_noret"
     * SLEEP_LPRM_NUM(2) -- "apcr_rail_lpm"
     * SLEEP_LPRM_NUM(3) -- "apcr_pll_lpm"
     * SLEEP_LPRM_NUM(4) -- "apcr"
     * SLEEP_LPRM_NUM(5) -- "clk_gate"
     *
     * correspond to the LPRMs in the "cpu_vdd" lpr xml file that 
     * allow cpu_vdd power rail to be collapsed as well those that
     * allow BHS to be turned off
     * Disable above modes when clients request this resource */
    npa_issue_required_request( client_handle, SLEEP_LPRM_NUM(5));
  }
  else
  {
    /* Issue a request to the sleep node to enable this node's LPRMs
     * during sleep. */
    npa_issue_required_request( client_handle, SLEEP_ALL_LPRMS);
  }

  return state;
}

/*==============================================================================
                             NODE DEFINITION
 =============================================================================*/
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
    "level",              /* Units */
    0x2,                  /* Max State */
    &npa_max_plugin,      /* Plugin */
    NPA_RESOURCE_DEFAULT, /* Attributes */
    NULL,                 /* User Data */
    NULL,                 /* Query Function */
    NULL                  /* Link query function */
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

