/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             P M    NPA   S E R V I C E S

GENERAL DESCRIPTION
  This file contains initialization functions for NPA

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010-2013           by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/src/pm_npa.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/26/13   rk      Remove code for VS.
01/22/13   vk      Added suppressible request support
03/01/2011 umr     Use Scalar request for VS and NCP.
12/14/2010 umr     NPA Transaction support.
09/26/10   umr     Add CXO scalar resource support.
06/03/10   umr     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"

#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"
#include "pm_npa_device_vs.h"
#include "pm_npa.h"
#include "pm_target_information.h"
#include "pm_utils.h"
#include "pm_sw_event.h"


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_npa_init                                EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the NPA for PMIC.

    It does the following:
    * Initializes the PMIC NPA Node and resources.

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
pm_npa_init (void)
{
} /* end of pm_modem_proc_npa_init() */

/*===========================================================================

FUNCTION pm_npa_resource_set

DESCRIPTION
    This function handles all the client requests.

    It does the following:
     Gets the user data from resource handle and passes requests
     the Device NPA Resources.

INPUT PARAMETERS
  Resource- The resource that is being requested
  Client-   Client requesting the resource
  mode_id-  Mode to which the Resource needs to be set.

RETURN VALUE
  State.

DEPENDENCIES
  NPA Framework should be available in the build being compiled for

SIDE EFFECTS
  NONE.

===========================================================================*/
npa_resource_state
pm_npa_process_rsrc (npa_client_handle client,
                     npa_resource *resource,
                     npa_resource_state mode_id,
                     uint32 index, uint32 dep_idx)
{
  pm_npa_pam_client_cfg_type *pam_data;
  pam_data = (pm_npa_pam_client_cfg_type *) resource->definition->data;

  npa_pass_request_attributes( client, NPA_DEPENDENCY( resource, dep_idx ) );

  switch (pam_data[index].rail_type)
  {
    case PM_NPA_VREG_SMPS:
      {
        pm_npa_smps_kvps *vreg_data;
        vreg_data = (pm_npa_smps_kvps *)pam_data[index].rail_data;
        vreg_data = &vreg_data[mode_id];

        npa_issue_vector_request( NPA_DEPENDENCY( resource, dep_idx ),
               sizeof (pm_npa_smps_kvps) / sizeof( npa_resource_state ),
                (npa_resource_state *)vreg_data );
      }
      break;

    case PM_NPA_VREG_LDO:
      {
        pm_npa_ldo_kvps *vreg_data;
        vreg_data = (pm_npa_ldo_kvps *)pam_data[index].rail_data;
        vreg_data = &vreg_data[mode_id];
        npa_issue_vector_request( NPA_DEPENDENCY( resource, dep_idx ),
               sizeof (pm_npa_ldo_kvps) / sizeof( npa_resource_state ),
                (npa_resource_state *)vreg_data );
      }
      break;

    case PM_NPA_VREG_VS:
      {
        pm_npa_vs_kvps *vreg_data;
        vreg_data = (pm_npa_vs_kvps *)pam_data[index].rail_data;
        vreg_data = &vreg_data[mode_id];

        npa_issue_vector_request( NPA_DEPENDENCY( resource, dep_idx ),
            sizeof (pm_npa_vs_kvps) / sizeof( npa_resource_state ),
            (npa_resource_state *)vreg_data );
      }
      break;

    case PM_NPA_CLK_BUFF:
      {
        pm_npa_clk_buff_kvps *cxo_data;
        cxo_data = (pm_npa_clk_buff_kvps *)pam_data[index].rail_data;
        cxo_data = &cxo_data[mode_id];

        npa_issue_vector_request( NPA_DEPENDENCY( resource, dep_idx ),
            sizeof (pm_npa_clk_buff_kvps) / sizeof( npa_resource_state ),
            (npa_resource_state *)cxo_data );
      }
      break;

    default:
      return 0;
  }
  return mode_id;
}

npa_resource_state
pm_npa_resource_set (npa_resource      *resource,
                     npa_client_handle  client,
                     npa_resource_state mode_id)
{
  uint32 i, dep;
  npa_resource_state mode_id_req;

  if (client->type == NPA_CLIENT_INITIALIZE)
  {
    /*
     * Initial state of resources to be set if any, Apart from device initial
     * states
     */
  } else
  {
    /* Use the aggregation corresponding to the current client->type.
     * The overall agg isn't important here as much as the individual
     * REQUIRED and SUPPRESSIBLE aggregations, because we have 2 client
     * handles to the dependency and it will aggregate across these anyway.
     */
    if (client->type == NPA_CLIENT_SUPPRESSIBLE)
    {
      dep = 1;
      mode_id_req = NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource);
    }
    else /* required */
    {
      dep = 0;
      mode_id_req = NPA_RESOURCE_REQUIRED_REQUEST(resource);
    }

    for (i = 0; dep < resource->node->dependency_count; i++, dep+=2)
      {
      pm_npa_process_rsrc(client, resource, mode_id_req, i, dep);
      }
    }

  return mode_id;
}

npa_resource_state
pm_npa_resource_set_vdd_uvol(npa_resource      *resource,
                             npa_client_handle  client,
                             npa_resource_state req_uv)
{
  uint32 i, dep, state_changed = FALSE, pm_mx_corners_idx;
  uint32 quantized_uv = req_uv;

  static pm_pwr_resource_info_type* mx_info = NULL;
  static uint32* pm_mx_corners_lst = NULL;
  static uint32 pm_mx_corners_count = 0;
  static uint32 prev_reqd_quantized_uv = 0, prev_supp_quantized_uv = 0;

  if (client->type == NPA_CLIENT_INITIALIZE)
  {
    /*
     * Initial state of resources to be set if any, Apart from device initial
     * states
     */
  }
  else
  {
    //Grab corner data if we haven't yet
    if(mx_info == NULL)
    {
        mx_info = (pm_pwr_resource_info_type*)pm_target_information_get_specific_info(PM_PROP_MX);
    }

    if(mx_info != NULL)
    {
        pm_mx_corners_count = *((uint32*)mx_info->data1);
        pm_mx_corners_lst = (uint32*)mx_info->data2;

        if (client->type == NPA_CLIENT_SUPPRESSIBLE)
        {
          dep = 1;
          pm_mx_corners_idx = pm_utils_get_upper_idx(NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource),
                                                     pm_mx_corners_lst, pm_mx_corners_count);
          quantized_uv = pm_mx_corners_lst[pm_mx_corners_idx];

          if (quantized_uv != prev_supp_quantized_uv)
          {
            prev_supp_quantized_uv = quantized_uv;
            state_changed = TRUE;
          }
        }
        else /* required */
        {
          dep = 0;
          pm_mx_corners_idx = pm_utils_get_upper_idx(NPA_RESOURCE_REQUIRED_REQUEST(resource),
                                                     pm_mx_corners_lst, pm_mx_corners_count);
          quantized_uv = pm_mx_corners_lst[pm_mx_corners_idx];

          if (quantized_uv != prev_reqd_quantized_uv)
          {
            prev_reqd_quantized_uv = quantized_uv;
            state_changed = TRUE;
          }
        }

        if (state_changed)
        {
            for (i = 0; dep < resource->node->dependency_count; i++, dep += 2)
            {
                pm_npa_pam_client_cfg_type *pam_data = (pm_npa_pam_client_cfg_type *)resource->definition->data;

                if(mx_info->resource_type == RPM_SMPS_A_REQ || mx_info->resource_type == RPM_SMPS_B_REQ)
                {
                    pm_npa_smps_kvps *vreg_data;

                    vreg_data = (pm_npa_smps_kvps *)pam_data[i].rail_data;
                    vreg_data = &vreg_data[0];
                    vreg_data->uv_value = quantized_uv;

                    pm_sw_event(PMIC_SET_VDD_MEM_UVOL, 1, vreg_data->uv_value);

                    npa_pass_request_attributes( client, NPA_DEPENDENCY( resource, dep ) );
                    npa_issue_vector_request(NPA_DEPENDENCY(resource, dep),
                                             sizeof(pm_npa_smps_kvps) / sizeof(npa_resource_state),
                                             (npa_resource_state *)vreg_data);
                }
                else if(mx_info->resource_type == RPM_LDO_A_REQ || mx_info->resource_type == RPM_LDO_B_REQ)
                {
                    pm_npa_ldo_kvps *vreg_data;

                    vreg_data = (pm_npa_ldo_kvps *)pam_data[i].rail_data;
                    vreg_data = &vreg_data[0];
                    vreg_data->uv_value = quantized_uv;

                    pm_sw_event(PMIC_SET_VDD_MEM_UVOL, 1, vreg_data->uv_value);

                    npa_pass_request_attributes( client, NPA_DEPENDENCY( resource, dep ) );
                    npa_issue_vector_request( NPA_DEPENDENCY( resource, dep),
                                            sizeof (pm_npa_ldo_kvps) / sizeof( npa_resource_state ),
                                            (npa_resource_state *)vreg_data);
                }
            }
        }
    }
  }

  return req_uv;
}
