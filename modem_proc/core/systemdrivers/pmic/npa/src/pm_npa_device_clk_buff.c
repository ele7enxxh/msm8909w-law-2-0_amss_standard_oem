/*! \file
*  
*  \brief  pm_npa_device_clk_buff.c ----This file contains init functions for the RPM PMIC NPA
*          driver
*  \details 
*  
*  
*  &copy; Copyright 2012-2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/src/pm_npa_device_clk_buff.c#1 $
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/22/13   vk      Added suppressible request support
03/07/12   hs      Use DAl APIs to replace memory allocation. 
02/12/12   wra     KVP based device setting initial file 
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_npa_device_clk_buff.h"
#include "pm_malloc.h" // pm_malloc
#include "pm_sw_event.h"

// Clock Buffer Create and Destroy Client Callbacks
void pm_npa_clk_buff_kvps_create_client_fcn(npa_client *client)
{
    // Allocate storage space because NPA discards the data otherwise.
    pm_malloc(sizeof(pm_npa_clk_buff_kvps),(void **)&client->resource_data);

    DALSYS_memset(client->resource_data, 0, sizeof(pm_npa_clk_buff_kvps));
}

void
pm_npa_clk_buff_kvps_destroy_client_fcn(npa_client *client)
{
    // Clean up our allocated storage space.
    DALSYS_Free(client->resource_data);
}

static void
pm_npa_clk_buff_kvps_aggregate(pm_npa_clk_buff_kvps *kvps, pm_npa_clk_buff_kvps *request)
{
  // Aggregate  Software Enable
  kvps->sw_enable_value       = MAX(kvps->sw_enable_value,       request->sw_enable_value);

  kvps->pin_ctrl_enable_value = MAX(kvps->pin_ctrl_enable_value, request->pin_ctrl_enable_value);
}

npa_resource_state
pm_npa_clk_buff_kvps_update_fcn(npa_resource *resource, npa_client_handle client)
{
    npa_client_handle active_client = resource->clients;
  pm_npa_clk_buff_kvps *kvps, *reqd_kvps, *client_req;
  npa_client_type req_type = client->type;

    // Firstly, save the client's request so that we don't lose it later.
    client_req = (pm_npa_clk_buff_kvps *)client->resource_data;
    DALSYS_memcpy(client_req, NPA_PENDING_REQUEST(client).pointer.vector, sizeof(pm_npa_clk_buff_kvps));
    NPA_PENDING_REQUEST(client).pointer.vector = (npa_resource_state *)client_req;

    // Loop through all of the clients, aggregating state (starting from scratch).
    kvps = (pm_npa_clk_buff_kvps *)resource->state_vector;
    DALSYS_memcpy(kvps, client_req, sizeof(pm_npa_clk_buff_kvps));

  /* Reqd request Only*/
  reqd_kvps = (pm_npa_clk_buff_kvps *)resource->required_state_vector;
  if (req_type == NPA_CLIENT_VECTOR)
  {
    DALSYS_memcpy(reqd_kvps, client_req, sizeof(pm_npa_clk_buff_kvps));
  }

    while(active_client)
    {
        // Find the saved request for this client.
        pm_npa_clk_buff_kvps *request = active_client->resource_data;

        // If this is the current client, we have already started with his values.
        if(client == active_client)
        {
            active_client = active_client->next;
            continue;
        }
    /* Aggregate for both Suppressable and Required */
    pm_npa_clk_buff_kvps_aggregate (kvps, request);
    pm_sw_event(PMIC_CLK_BUF_AGG1, 3, 0, kvps->sw_enable_value, kvps->pin_ctrl_enable_value); 

    if (req_type == NPA_CLIENT_VECTOR && active_client->type == NPA_CLIENT_VECTOR)
    {
      /* Aggregate only Required */
      pm_npa_clk_buff_kvps_aggregate (reqd_kvps, request);
      pm_sw_event(PMIC_CLK_BUF_AGG2, 3, 0, kvps->sw_enable_value, kvps->pin_ctrl_enable_value);
    }

        active_client = active_client->next;
    }

    return kvps->sw_enable_value; // Return the sw enable for quick perusal in logs; "real state" is in resource->state_vector.
                                //return sizeof(pm_npa_clk_buff_kvps);
}

npa_resource_plugin pm_npa_clk_buff_kvps_plugin =
{
    pm_npa_clk_buff_kvps_update_fcn,
    NPA_CLIENT_VECTOR | NPA_CLIENT_SUPPRESSIBLE_VECTOR,
    pm_npa_clk_buff_kvps_create_client_fcn,
    pm_npa_clk_buff_kvps_destroy_client_fcn,
};

pm_err_flag_type
pm_npa_clk_buff_kvps_init (pm_npa_clk_buff_kvps *kvps)
{
  if (kvps == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  kvps->sw_enable_key = PM_NPA_KEY_SOFTWARE_ENABLE;
  kvps->sw_enable_size = 4;
  kvps->sw_enable_value = 0;

  kvps->pin_ctrl_enable_key = PM_NPA_KEY_PIN_CTRL_ENABLE;
  kvps->pin_ctrl_enable_size = 4;
  kvps->pin_ctrl_enable_value = 0;

  return PM_ERR_FLAG__SUCCESS;
} 

npa_resource_plugin *pm_npa_get_clk_buff_kvps_plugin(void)
{
    return(&pm_npa_clk_buff_kvps_plugin);
}


