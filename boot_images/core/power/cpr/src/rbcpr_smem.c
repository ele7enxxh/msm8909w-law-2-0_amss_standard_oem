/*===========================================================================
                              rbcpr_smem.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rbcpr_smem.h"

#include "CoreVerify.h"
#include "smem.h"
#include "stringl/stringl.h"

#define __ELEMENT_OFFSET(type, element) \
    (uint32)(&((type *)0x0)->element)

void rbcpr_externalize_state(void)
{
    const uint32 size_of_externalized_data = __ELEMENT_OFFSET(rbcpr_externalized_state, rail_data[rbcpr_bsp_data.num_rails]);
    rbcpr_externalized_state* extern_data = (rbcpr_externalized_state*) smem_alloc(SMEM_CPR_CONFIG, size_of_externalized_data);
    CORE_VERIFY_PTR(extern_data);
    extern_data->version = 2;
    extern_data->number_of_rails = rbcpr_bsp_data.num_rails;
    for(int i=0; i<rbcpr_bsp_data.num_rails; i++)
    {
        rbcpr_rail_externalized_state_v1* rail_data = &extern_data->rail_data[i];
        const rbcpr_bsp_rail_info* rail = &rbcpr_bsp_data.rails[i];

        strlcpy(rail_data->rail_name, rail->rail_const_info->rail_name, MAX_RAIL_NAME_LENGTH);
        rail_data->enablement = rail->bsp_data->rbcpr_enablement;
        if(rail->bsp_data->rbcpr_enablement == RBCPR_DISABLED)
            continue;

        for(int i=0; i<RAILWAY_CORNERS_COUNT; i++)
        {
            rail_data->railway_voltages[i] = railway_get_corner_voltage(rail->railway_rail_id, (railway_corner)i);
        }

        CORE_VERIFY(rail->bsp_data->number_of_target_params<=MAX_RBCPR_LEVELS);
        
        rail_data->corner_params_count = rail->bsp_data->number_of_target_params;
        
        for(int i=0; i<rail->bsp_data->number_of_target_params; i++)
        {
			memscpy(&rail_data->corner_params[i], sizeof(rbcpr_corner_params_type), &rail->bsp_data->target_params[i].corner_params, sizeof(rbcpr_corner_params_type));
			/*
	    memscpy(&rail_data->corner_params[i], sizeof(rbcpr_corner_params_type), &rail->bsp_data->target_params[i], sizeof(rbcpr_corner_params_type));
		*/
        }
    }
}
