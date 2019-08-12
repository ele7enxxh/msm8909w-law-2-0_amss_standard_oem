/*! \file
 *  
 *  \brief  pm_rpm_proc_npa.c ----This file contains initialization functions for NPA
 *  \details This file contains initialization functions for NPA
 *          and node definitions.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: MSM8974 branch 1.0 ver 8_21_2012 - Not Approved
 *    PMIC code generation NPA Device Setting Value Version: MSM8974 branch 1.0 ver 8_21_2012 - Not Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/src/pm_rpm_npa.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/23/13   hs      Fixed the naming convention in \config.
02/27/13   hs      Code refactoring.
01/29/13   aks     Adding support for Boost as separate peripheral 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "comdef.h"
#include <err.h>
#include "rpmserver.h"
#include "npa.h"

#include "pm_npa.h"
#include "pm_rpm_npa_device.h"
#include "pm_rpm_npa.h"
#include "pm_npa_device.h"
#include "pm_target_information.h"
/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

pm_rpm_keys_to_dependent_map keys_to_dependent_map[14] =
{
   {PM_NPA_KEY_SOFTWARE_ENABLE, PM_RPM_NPA_INT_CLIENT_SW_ENABLE_DEPENDENT, 4},
   {PM_NPA_KEY_LDO_SOFTWARE_MODE, PM_RPM_NPA_INT_CLIENT_SW_MODE_DEPENDENT, 4},
   {PM_NPA_KEY_SMPS_SOFTWARE_MODE, PM_RPM_NPA_INT_CLIENT_SW_MODE_DEPENDENT, 4},
   {PM_NPA_KEY_PIN_CTRL_ENABLE, PM_RPM_NPA_INT_CLIENT_PIN_CONTROL_ENABLE_DEPENDENT, 4},
   {PM_NPA_KEY_PIN_CTRL_POWER_MODE, PM_RPM_NPA_INT_CLIENT_PIN_CONTROL_MODE_DEPENDENT, 4},
   {PM_NPA_KEY_CURRENT, PM_RPM_NPA_INT_CLIENT_CURRENT_DEPENDENT, 4},
   {PM_NPA_KEY_MICRO_VOLT, PM_RPM_NPA_INT_CLIENT_VOLTAGE_DEPENDENT, 4},
   {PM_NPA_KEY_FREQUENCY, PM_RPM_NPA_INT_CLIENT_FREQ_DEPENDENT, 4},
   {PM_NPA_KEY_FREQUENCY_REASON, PM_RPM_NPA_INT_CLIENT_FREQ_REASON_DEPENDENT, 4},
   {PM_NPA_KEY_FOLLOW_QUIET_MODE, PM_RPM_NPA_INT_CLIENT_QUIET_MODE_DEPENDENT, 4},
   {PM_NPA_KEY_HEAD_ROOM, PM_RPM_NPA_INT_CLIENT_HEADROOM_DEPENDENT, 4},
   {PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, PM_RPM_NPA_INT_CLIENT_BYPASS_EN_DEPENDENT, 4},
   {PM_NPA_KEY_BYPASS_ALLOWED_KEY, PM_RPM_NPA_INT_CLIENT_CLK_PIN_CONTROL_ENABLE_DEPENDENT, 4},
   {PM_NPA_KEY_CORNER_LEVEL_KEY, PM_RPM_NPA_INT_CLIENT_CORNER_LEVEL_DEPENDENT, 4},
};

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
uint32                              num_of_nodes = 0;
npa_node_definition                *pmic_npa_rpm_nodes = NULL;
npa_resource_definition            *pmic_npa_rpm_resources = NULL;    

void pm_rpm_proc_npa_register_resources (void);

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION pm_rpm_proc_npa_init                                EXTERNAL FUNCTION

DESCRIPTION
This function initializes the NPA for PMIC.

It does the following:
* It initializes the PMIC NPA Node and resources.

INPUT PARAMETERS
None.

RETURN VALUE
None.

DEPENDENCIES
NPA Framework should be available in the build being compiled for

SIDE EFFECTS
NONE.

===========================================================================*/
void
pm_rpm_proc_npa_init (void)
{
   unsigned i;
   unsigned int_resource;
   pm_rpm_local_resource_client_dependency *internal_client_dependecy_array;
   pm_rpm_resource_client_dependency_info *internal_client_dependecy_info;
   unsigned client_type = 0;
   unsigned client_size = 0;
 
   uint32                             *pamNodeRscCount = NULL;
   pm_npa_node_resource_info          *pamNodeRscInfo = NULL;
   pm_npa_node_resource_info          *nodeRscArray = NULL;
   pm_npa_node_resource_info          *nodeRsc = NULL;

   /* Init RPM device trans-apply */
   pm_rpm_proc_npa_register_resources();

   // Get the PAM info
   pamNodeRscInfo = (pm_npa_node_resource_info*)pm_target_information_get_specific_info(PM_PROP_PAM_NODE_RSRCS);
   pamNodeRscCount = (uint32*)pm_target_information_get_specific_info(PM_PROP_PAM_NODE_NUM);

   if(pamNodeRscInfo && pamNodeRscCount)
   {
       nodeRscArray = (pm_npa_node_resource_info*)(pamNodeRscInfo);
       num_of_nodes = *((uint32*)(pamNodeRscCount));

       if(num_of_nodes && nodeRscArray)
       {
           pm_malloc(sizeof(npa_node_definition)*num_of_nodes, (void**)&pmic_npa_rpm_nodes);
           pm_malloc(sizeof(npa_resource_definition)*num_of_nodes, (void**)&pmic_npa_rpm_resources);

           // construct the PMIC RPM resources
           for(i = 0; i<num_of_nodes; i++)
           {
               nodeRsc = &nodeRscArray[i];

               // construct the PMIC RPM resources
               pmic_npa_rpm_resources[i].name = nodeRsc->group_name;
               pmic_npa_rpm_resources[i].units = "ModeID";
               pmic_npa_rpm_resources[i].max = nodeRsc->max_mode;
               pmic_npa_rpm_resources[i].plugin = &npa_identity_plugin;
               pmic_npa_rpm_resources[i].attributes = nodeRsc->resource_attributes;
               pmic_npa_rpm_resources[i].data = nodeRsc->resource_data;

               // construct the PMIC RPM Nodes
               pmic_npa_rpm_nodes[i].name = nodeRsc->node_name;
               pmic_npa_rpm_nodes[i].driver_fcn = pm_rpm_npa_resource_set;
               pmic_npa_rpm_nodes[i].attributes = nodeRsc->node_attributes;
               pmic_npa_rpm_nodes[i].data = nodeRsc->node_data;
               pmic_npa_rpm_nodes[i].dependency_count = 0;
               pmic_npa_rpm_nodes[i].dependencies = nodeRsc->node_dependencies;
               pmic_npa_rpm_nodes[i].resource_count = 1;
               pmic_npa_rpm_nodes[i].resources = NULL;


           }
       }
       else
       {
           ERR_FATAL( "PMIC-RPM PAM initialization failed! ", 0, 0, 0 ); // we should never get here
       }
   }
   
   // Create the client handles
   for (i = 0; i < num_of_nodes; i++)
   {
       internal_client_dependecy_info = (pm_rpm_resource_client_dependency_info*)pmic_npa_rpm_nodes[i].data;
       internal_client_dependecy_array = internal_client_dependecy_info->dependencies;
       for(int_resource = 0; int_resource < internal_client_dependecy_info->dependency_count; int_resource++ )
       {
          client_type = internal_client_dependecy_array[int_resource].client_type;

          // register internal clients based on the type of the clients.
         if(client_type == PM_RPM_NPA_CLIENT_SMPS_REQ) 
         {
             client_size = sizeof(pm_npa_smps_int_rep);
         }
         else if(client_type == PM_RPM_NPA_CLIENT_LDO_REQ)
         {
             client_size = sizeof(pm_npa_ldo_int_rep);
         }
         else if(client_type == PM_RPM_NPA_CLIENT_VS_REQ)
         {
             client_size = sizeof(pm_npa_vs_int_rep);
         }
         else if(client_type == PM_RPM_NPA_CLIENT_CLK_BUFFER_REQ)
         {
             client_size = sizeof(pm_npa_clk_buffer_int_rep);
         }
		 else if(client_type == PM_RPM_NPA_CLIENT_BOOST_REQ )
		 {
		     client_size = sizeof(pm_npa_boost_int_rep);
		 }
		 else if(client_type == PM_RPM_NPA_CLIENT_BOOST_BYP_REQ )
		 {
		     client_size = sizeof(pm_npa_boost_byp_int_rep);
		 }
         if(client_size != 0)
         {
           rpm_register_internal_client_type(internal_client_dependecy_array[int_resource].resource_type,
                                             internal_client_dependecy_array[int_resource].resource_id,
                                               client_type,
                                               client_size);

           internal_client_dependecy_array[int_resource].internal_client_index =
           rpm_create_client(internal_client_dependecy_array[int_resource].resource_type,
                             internal_client_dependecy_array[int_resource].resource_id,
                                               client_type);
         }
       }      
   }
   
   for (i = 0; i < num_of_nodes; i++)
   {
      pmic_npa_rpm_nodes[i].resources = &pmic_npa_rpm_resources[i];
      //pmic_npa_rpm_nodes[i].resource_count = NPA_ARRAY_SIZE(pmic_npa_rpm_resources);
      npa_define_node (pmic_npa_rpm_nodes + i, NULL, NULL);
   }
} /* end of pm_rpm_proc_npa_init() */


npa_resource_state
pm_rpm_npa_resource_set (npa_resource      *resource,
                         npa_client_handle  client,
                         npa_resource_state mode_id)
{
   unsigned int_resource;
   pm_rpm_local_resource_client_dependency *internal_client_dependecy_array;
   pm_rpm_resource_client_dependency_info *internal_client_dependecy_info;
   
   if (client->type == NPA_CLIENT_INITIALIZE)
   {
      /*
      * Initial state of resources to be set if any, Apart from device initial
      * states
      */
   }
   else
   {
      internal_client_dependecy_info = (pm_rpm_resource_client_dependency_info*) resource->node->data;
      internal_client_dependecy_array = internal_client_dependecy_info->dependencies;
      for(int_resource = 0; int_resource < internal_client_dependecy_info->dependency_count; int_resource++ )
      {
         pm_rpm_npa_process_rsrc (resource, &internal_client_dependecy_array[int_resource], mode_id, int_resource);
      }
   }
   return mode_id;
}

npa_resource_state
pm_rpm_npa_process_rsrc (npa_resource *resource,
                         pm_rpm_local_resource_client_dependency *dep_info,
                         npa_resource_state mode_id,
                         uint32 index)
{
   pm_npa_pam_client_cfg_type *pam_data;
   //pm_rpm_local_resource_client_dependency *int_client_dep;
   pam_data = (pm_npa_pam_client_cfg_type *) resource->definition->data;
   
   switch (pam_data[index].rail_type)
   {
      case PM_NPA_VREG_SMPS:
      {
         pm_npa_smps_int_rep *vreg_data;
         vreg_data = (pm_npa_smps_int_rep *)pam_data[index].rail_data;
         vreg_data = &vreg_data[mode_id];
         
         rpm_issue_request(dep_info->resource_type,
         dep_info->resource_id,
         dep_info->internal_client_index,
         sizeof(pm_npa_smps_int_rep), (void*)vreg_data);
      }
      break;
      
	  case PM_NPA_VREG_BOOST:
      {
         pm_npa_boost_int_rep *vreg_data;
         vreg_data = (pm_npa_boost_int_rep *)pam_data[index].rail_data;
         vreg_data = &vreg_data[mode_id];
         
         rpm_issue_request(dep_info->resource_type,
         dep_info->resource_id,
         dep_info->internal_client_index,
         sizeof(pm_npa_boost_int_rep), (void*)vreg_data);
      }
      break;
      
      case PM_NPA_VREG_LDO:
      {
         pm_npa_ldo_int_rep *vreg_data;
         vreg_data = (pm_npa_ldo_int_rep *)pam_data[index].rail_data;
         vreg_data = &vreg_data[mode_id];
         
         rpm_issue_request(dep_info->resource_type,
         dep_info->resource_id,
         dep_info->internal_client_index,
         sizeof(pm_npa_ldo_int_rep), (void*)vreg_data);
      }
      break;
      
      case PM_NPA_VREG_VS:
      {
         pm_npa_vs_int_rep *vreg_data;
         vreg_data = (pm_npa_vs_int_rep *)pam_data[index].rail_data;
         vreg_data = &vreg_data[mode_id];
         
         rpm_issue_request(dep_info->resource_type,
         dep_info->resource_id,
         dep_info->internal_client_index,
         sizeof(pm_npa_vs_int_rep), (void*)vreg_data);
      }
      break;
      
      case PM_NPA_VREG_NCP:
      {
         // NO NCP on badger
         //pm_npa_ncp_int_rep *vreg_data;
         //vreg_data = (pm_npa_ncp_int_rep *)pam_data[index].rail_data;
         //vreg_data = &vreg_data[mode_id];
         
         //rpm_issue_request(dep_info->resource_type,
         //    dep_info->resource_id,
         //    dep_info->internal_client_index,
         //    sizeof(pm_npa_ncp_int_rep), (void*)vreg_data);
      }
      break;
      
      case PM_NPA_CLK_BUFF:
      {
         pm_npa_clk_buffer_int_rep *vreg_data;
         vreg_data = (pm_npa_clk_buffer_int_rep *)pam_data[index].rail_data;
         vreg_data = &vreg_data[mode_id];
         
         rpm_issue_request(dep_info->resource_type,
         dep_info->resource_id,
         dep_info->internal_client_index,
         sizeof(pm_npa_clk_buffer_int_rep), (void*)vreg_data);
      }
      break;
      
	  case PM_NPA_VREG_BOOST_BYP:
      {
         pm_npa_boost_byp_int_rep *vreg_data;
         vreg_data = (pm_npa_boost_byp_int_rep *)pam_data[index].rail_data;
         vreg_data = &vreg_data[mode_id];
         
         rpm_issue_request(dep_info->resource_type,
         dep_info->resource_id,
         dep_info->internal_client_index,
         sizeof(pm_npa_boost_byp_int_rep), (void*)vreg_data);
      }
      break;
      
      default:
      return 0;
   }
   return mode_id;
}


