/*! \file
*  
*  \brief  pm_npa_device_smps.c ----This file contains init functions for the RPM PMIC NPA
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

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/src/pm_npa_device_smps.c#1 $
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/22/13   vk      Added suppressible request support
03/07/12   hs      Use DAl APIs to replace memory allocation. 
02/12/12   wra     KVP based device setting initial file 
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_npa_device_smps.h"
#include "pm_malloc.h" // pm_malloc
#include "npa_resource.h"

// SMPS Create and Destroy Client Callbacks
void pm_npa_smps_kvps_create_client_fcn(npa_client *client)
{
    // Allocate storage space because NPA discards the data otherwise.
    pm_malloc(sizeof(pm_npa_smps_kvps),(void **)&client->resource_data);

    DALSYS_memset(client->resource_data, 0, sizeof(pm_npa_smps_kvps));
}

void pm_npa_smps_kvps_destroy_client_fcn(npa_client *client)
{
    // Clean up our allocated storage space.
    DALSYS_Free(client->resource_data);
}

static void
pm_npa_smps_kvps_aggregate(pm_npa_smps_kvps *kvps, pm_npa_smps_kvps *request)
{
        // Aggregate Software Enable
        kvps->sw_enable_value           = MAX(kvps->sw_enable_value,       request->sw_enable_value);

        // Aggregate Software Mode Enable
        kvps->smps_sw_mode_value        = MAX(kvps->smps_sw_mode_value,       request->smps_sw_mode_value);

        // Aggregate ORed values of Pin Control Enable
        kvps->pin_ctrl_enable_value     = kvps->pin_ctrl_enable_value | request->pin_ctrl_enable_value;

        // Aggregate ORed values of Pin Control Power Mode
        kvps->pin_ctrl_power_mode_value = kvps->pin_ctrl_power_mode_value | request->pin_ctrl_power_mode_value;

        // Aggregate Maximum Voltage
        kvps->uv_value                  = MAX(kvps->uv_value,       request->uv_value);

        // Aggregate Summation of Current
        kvps->current_value             = kvps->current_value + request->current_value;

        // The following line will only be true if there is a frequency reason 
        // other than "PM_NPA_FREQ_REASON_NONE"
        if(request->frequency_reason_value > kvps->frequency_reason_value)
        {
            // If the frequency reason takes priority then set the frequency
            // to what ever the client needs.
            kvps->frequency_reason_value = request->frequency_reason_value;
            kvps->frequency_value = request->frequency_value;            
        }
        else
        {
            // If the frequency reason is NONE, then a simple max frequency aggregation works
            if(kvps->frequency_reason_value == PM_NPA_FREQ_REASON_NONE)
            {
                if( 0 != request->frequency_value)
                {
                // Aggregate Maximum Frequency, the enum sequence is from the lowest to highest since the HW is mapped so
                kvps->frequency_value           = MIN(kvps->frequency_value,       request->frequency_value);
            }
        }
        }

        // Aggregate ORed values of Frequency Reason
        kvps->frequency_reason_value    =  kvps->frequency_reason_value | request->frequency_reason_value;

        // Aggregate Quiet Mode
        kvps->follow_quiet_mode_value   = MAX(kvps->follow_quiet_mode_value, request->follow_quiet_mode_value);

        // Aggregate Headroom Voltage
        kvps->head_room_voltage_value = MAX(kvps->head_room_voltage_value, request->head_room_voltage_value);

        kvps->bypass_allowed_value = MAX(kvps->bypass_allowed_value, request->bypass_allowed_value);
        
        kvps->corner_value = MAX(kvps->corner_value, request->corner_value);
		
        kvps->floor_value = MAX(kvps->floor_value, request->floor_value);

        kvps->volt_level_value = MAX(kvps->volt_level_value, request->volt_level_value);

        kvps->volt_level_floor_value = MAX(kvps->volt_level_floor_value, request->volt_level_floor_value);
}

npa_resource_state pm_npa_smps_kvps_update_fcn(npa_resource *resource, npa_client_handle client)
{
  npa_client_handle active_client = resource->clients;
  pm_npa_smps_kvps      *kvps, *reqd_kvps, *client_req;
  npa_client_type req_type = client->type;

  client_req = (pm_npa_smps_kvps *)client->resource_data;
  DALSYS_memcpy(client_req, NPA_PENDING_REQUEST(client).pointer.vector, sizeof(pm_npa_smps_kvps));
  NPA_PENDING_REQUEST(client).pointer.vector = (npa_resource_state *)client_req;

  kvps = (pm_npa_smps_kvps *)resource->state_vector;
  DALSYS_memcpy(kvps, client_req, sizeof(pm_npa_smps_kvps));

  /* Reqd request Only*/
  reqd_kvps = (pm_npa_smps_kvps *)resource->required_state_vector;
  if (req_type == NPA_CLIENT_VECTOR)
  {
    DALSYS_memcpy(reqd_kvps, client_req, sizeof(pm_npa_smps_kvps));
  }

  // Loop through all of the clients, aggregating state (starting with the values from the current client).
  while (active_client)
  {
    // Find the saved request for this client.
    pm_npa_smps_kvps *request = active_client->resource_data;

    // If this is the current client, we've already started with his values.
    if (client == active_client)
    {
        active_client = active_client->next;
      continue;
    }

    /* Aggregate for both Suppressable and Required */
    pm_npa_smps_kvps_aggregate (kvps, request);

    if (req_type == NPA_CLIENT_VECTOR && active_client->type == NPA_CLIENT_VECTOR)
    {
      /* Aggregate only Required */
      pm_npa_smps_kvps_aggregate (reqd_kvps, request);
    }

    active_client = active_client->next;
  }
    
    return kvps->uv_value; // Return the voltage for quick perusal in logs; "real state" is in resource->state_vector.
}

npa_resource_plugin pm_npa_smps_kvps_plugin =
{
    pm_npa_smps_kvps_update_fcn,
    NPA_CLIENT_VECTOR | NPA_CLIENT_SUPPRESSIBLE_VECTOR,
    pm_npa_smps_kvps_create_client_fcn,
    pm_npa_smps_kvps_destroy_client_fcn,
};

/**
 * pm_npa_smps_kvps_init
 *
 * See pm_npa_vreg.h
 */
pm_err_flag_type
pm_npa_smps_kvps_init (pm_npa_smps_kvps *kvps)
{
  if (kvps == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  kvps->sw_enable_key = PM_NPA_KEY_SOFTWARE_ENABLE;
  kvps->sw_enable_size = 4;
  kvps->sw_enable_value = 0;

  kvps->smps_sw_mode_key = PM_NPA_KEY_SMPS_SOFTWARE_MODE;
  kvps->smps_sw_mode_size = 4;
  kvps->smps_sw_mode_value = 0;

  kvps->pin_ctrl_enable_key = PM_NPA_KEY_PIN_CTRL_ENABLE;
  kvps->pin_ctrl_enable_size = 4;
  kvps->pin_ctrl_enable_value = 0;

  kvps->pin_ctrl_power_mode_key = PM_NPA_KEY_PIN_CTRL_POWER_MODE;
  kvps->pin_ctrl_power_mode_size = 4;
  kvps->pin_ctrl_power_mode_value = 0;

  kvps->uv_key = PM_NPA_KEY_MICRO_VOLT;
  kvps->uv_size = 4;
  kvps->uv_value = 0;

  kvps->current_key = PM_NPA_KEY_CURRENT;
  kvps->current_size = 4;
  kvps->current_value = 0;

  kvps->frequency_key = PM_NPA_KEY_FREQUENCY;
  kvps->frequency_size = 4;
  kvps->frequency_value = 0;

  kvps->frequency_reason_key = PM_NPA_KEY_FREQUENCY_REASON;
  kvps->frequency_reason_size = 4;
  kvps->frequency_reason_value = 0;

  kvps->follow_quiet_mode_key = PM_NPA_KEY_FOLLOW_QUIET_MODE;
  kvps->follow_quiet_mode_size = 4;
  kvps->follow_quiet_mode_value = 0;

  kvps->head_room_voltage_key = PM_NPA_KEY_HEAD_ROOM;
  kvps->head_room_voltage_size = 4;
  kvps->head_room_voltage_value = 0;

  kvps->bypass_allowed_key = PM_NPA_KEY_BYPASS_ALLOWED_KEY;
  kvps->bypass_allowed_size = 4;
  kvps->bypass_allowed_value = 0;

  kvps->corner_key = PM_NPA_KEY_CORNER_LEVEL_KEY;
  kvps->corner_size = 4;
  kvps->corner_value = 0;

  kvps->floor_key = PM_NPA_KEY_ACTIVE_FLOOR;
  kvps->floor_size = 4;
  kvps->floor_value = 0;

  kvps->volt_level_key = PM_NPA_KEY_CORNER_VOLT_LEVEL;
  kvps->volt_level_size = 4;
  kvps->volt_level_value = 0;

  kvps->volt_level_floor_key = PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR;
  kvps->volt_level_floor_size = 4;
  kvps->volt_level_floor_value = 0;

  return PM_ERR_FLAG__SUCCESS;
}

// Helper function to retrieve the SMPS KVPS plugin
npa_resource_plugin *pm_npa_get_smps_kvps_plugin(void)
{
    return(&pm_npa_smps_kvps_plugin);
}
