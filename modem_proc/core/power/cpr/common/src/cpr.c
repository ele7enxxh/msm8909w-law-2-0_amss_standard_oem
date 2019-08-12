/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_fuses.h"
#include "cpr_enablement.h"
#include "CoreVerify.h"

static void cpr_init_rail(cpr_rail_id_t rail_id, const cpr_enablement_versioned_rail_config_t* rail_enablement_config, cpr_enablement_stage_t enablement_stage)
{
    CORE_VERIFY_PTR(rail_enablement_config);
    CORE_VERIFY_PTR(rail_enablement_config->enablement_init_params);
    CORE_VERIFY_PTR(rail_enablement_config->enablement_init_params->enablement_fn);

    rail_enablement_config->enablement_init_params->enablement_fn(rail_id, rail_enablement_config, enablement_stage);
}

static bool cpr_init_initial_voltage_config_done;

void cpr_init_initial_voltage_config(void)
{   
    if(cpr_init_initial_voltage_config_done)
        return;
        
    cpr_init_initial_voltage_config_done = true;
    
    cpr_fuses_init();

    const uint32 number_of_rails = cpr_enablement_number_of_rails();
    for(int i = 0; i<number_of_rails; i++)
    {
        cpr_rail_id_t rail_id;
        const cpr_enablement_versioned_rail_config_t* rail_enablement_config = NULL;
        cpr_enablement_rail_info(i, &rail_id, &rail_enablement_config);
        cpr_enablement_from_xml(rail_id ,(cpr_enablement_versioned_rail_config_t**)&rail_enablement_config);
        cpr_init_rail(rail_id, rail_enablement_config, CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG);
    }
}

void cpr_init_closed_loop_init(void)
{
    //Make sure the initial voltage config has been done.
    CORE_VERIFY(cpr_init_initial_voltage_config_done);

    static bool cpr_init_closed_loop_init_done;
    if(cpr_init_closed_loop_init_done)
        return;
        
    cpr_init_closed_loop_init_done = true;
    
    const uint32 number_of_rails = cpr_enablement_number_of_rails();
    for(int i = 0; i<number_of_rails; i++)
    {
        cpr_rail_id_t rail_id;
        const cpr_enablement_versioned_rail_config_t* rail_enablement_config = NULL;
        cpr_enablement_rail_info(i, &rail_id, &rail_enablement_config);

        cpr_init_rail(rail_id, rail_enablement_config, CPR_ENABLEMENT_STAGE_INIT_CLOSED_LOOP);
    }
}
