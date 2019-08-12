/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             PM NPA TEST SERVICES

GENERAL DESCRIPTION
  This file contains initialization function definition for PMIC NPA tests.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/test/pm_npa_test.c#1 $
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/14   mr      Resolved pm_npa_test_init() failure. (CR-699978)
04/22/14   pxm     Fixed Compilation warnings in pm_npa_test_init. Set variables as global to avoid duplicate.
04/19/13   kt      Added pm_npa_test_init function.
09/26/10   umr     Add HKADC mode and group test case.
03/31/10   umr     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <err.h>
#include "npa.h"
#include "npa_resource.h"
#include "pm_npa.h"
#include "pmapp_npa.h"
#include "pm_target_information.h"
#include "pm_malloc.h"
#include "pm_npa_test.h"


/*===========================================================================

                GLOBAL VARIABLES

===========================================================================*/
/* Following variables are defined as extern inside pm_npa_test.h */
char                        *rsrc_test_name = "/pmic/tst_cl/";
pm_npa_node_resource_info   *node_rsrc_arr  = NULL;
uint32                      num_of_test_nodes  = 0;

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
void pm_npa_test_init (void)
{
    unsigned i;

    pm_npa_node_resource_info          *node_rsrc = NULL;
    static npa_node_definition         *pm_npa_test_nodes = NULL;
    static npa_resource_definition     *pm_npa_test_resources = NULL;
    static boolean                      pm_npa_test_initialized = FALSE;
    char                               *node_test_name = "/node/pmic/tst_cl/";
    uint32                             *node_num_ptr = NULL;

    if (pm_npa_test_initialized)
    {
       return;
    }

    // Get the PAM info
    node_rsrc_arr = (pm_npa_node_resource_info*)pm_target_information_get_specific_info(PM_PROP_PAM_NODE_RSRCS);
    node_num_ptr = (uint32*)pm_target_information_get_specific_info(PM_PROP_PAM_NODE_NUM);

    if (node_num_ptr != NULL)
    {
       num_of_test_nodes = *node_num_ptr;
    }

    if(num_of_test_nodes && node_rsrc_arr)
    {
        pm_malloc(sizeof(npa_node_definition)*num_of_test_nodes, (void**)&pm_npa_test_nodes);
        pm_malloc(sizeof(npa_resource_definition)*num_of_test_nodes, (void**)&pm_npa_test_resources);

            for(i = 0; i<num_of_test_nodes; i++)
            {
                node_rsrc = &node_rsrc_arr[i];

                /* Construct the PMIC RPM test resources */

                /* Dynamically allocate memory for test resource name and then copy the actual
                   resource name to the memory by changing the initial fixed part (to rsrc_test_name) */
                pm_malloc((sizeof(char)*(strlen(node_rsrc->group_name)+1)), (void**)&pm_npa_test_resources[i].name);

                /* Change the initial fixed part of the actual resource name by copying the
                   rsrc_test_name first to the allocated memory */
                strlcpy((char*)pm_npa_test_resources[i].name, rsrc_test_name, (strlen(node_rsrc->group_name) + 1));

                /* Then copy the non-fixed part of the actual resource name to the allocated memory starting
                   from after the fixed part till the end based on the actual resource name length */
                strlcpy((char*)(pm_npa_test_resources[i].name + strlen(rsrc_test_name)),
                        (node_rsrc->group_name + strlen(rsrc_test_name)),
                        (strlen(node_rsrc->group_name) - strlen(rsrc_test_name) + 1));

                pm_npa_test_resources[i].units = "ModeID";
                pm_npa_test_resources[i].max = node_rsrc->max_mode;
                pm_npa_test_resources[i].attributes = node_rsrc->resource_attributes;
                if ((pm_npa_test_resources[i].attributes & NPA_RESOURCE_SINGLE_CLIENT) ==
                    NPA_RESOURCE_SINGLE_CLIENT)
                {
                  pm_npa_test_resources[i].plugin = &npa_identity_plugin;
                }
                else
                {
                  pm_npa_test_resources[i].plugin = &npa_max_plugin;
                }
                pm_npa_test_resources[i].data = node_rsrc->resource_data;
                pm_npa_test_resources[i].query_fcn = 0;

                /* Construct the PMIC RPM test nodes */

                /* Dynamically allocate memory for test node name and then copy the actual
                   node name to the memory by changing the initial fixed part (to node_test_name) */
                pm_malloc((sizeof(char)*(strlen(node_rsrc->node_name)+1)), (void**)&pm_npa_test_nodes[i].name);

                /* Change the initial fixed part of the actual node name by copying the
                   node_test_name first to the allocated memory */
                strlcpy((char*)pm_npa_test_nodes[i].name, node_test_name, (strlen(node_rsrc->node_name) + 1));

                /* Then copy the non-fixed part of the actual node name to the allocated memory starting
                   from after the fixed part till the end based on the actual node name length*/
                strlcpy((char*)(pm_npa_test_nodes[i].name + strlen(node_test_name)),
                        (node_rsrc->node_name + strlen(node_test_name)),
                        (strlen(node_rsrc->node_name) - strlen(node_test_name) + 1));

                pm_npa_test_nodes[i].driver_fcn = pm_npa_resource_set;
                pm_npa_test_nodes[i].attributes = node_rsrc->node_attributes;
                pm_npa_test_nodes[i].data = node_rsrc->node_data;
                pm_npa_test_nodes[i].dependencies = node_rsrc->node_dependencies;
                pm_npa_test_nodes[i].dependency_count = node_rsrc->dependency_count;
                pm_npa_test_nodes[i].resource_count = 1;
                pm_npa_test_nodes[i].resources = NULL;
            }
        }
        else
        {
            ERR_FATAL( "PMIC PAM test initialization failed! ", 0, 0, 0 ); // we should never get here
        }

    for (i = 0; i < num_of_test_nodes; i++)
    {
       pm_npa_test_nodes[i].resources = &pm_npa_test_resources[i];
       npa_define_node (pm_npa_test_nodes + i, NULL, NULL);
    }

    pm_npa_test_initialized = TRUE;

} /* end of pm_npa_test_init() */
