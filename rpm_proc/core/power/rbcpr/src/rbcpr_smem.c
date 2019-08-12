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
#include "rpm_config.h"


#include "CoreVerify.h"
#include "Chipinfo.h"


#define __ELEMENT_OFFSET(type, element) \
    (uint32)(&((type *)0x0)->element)
#define RPM_CSR_CONFIGURE_MSGRAM_ACCESS HWIO_RPM_PAGE_SELECT_OUT(0x0)
#define RPM_CSR_RECONFIGURE_MSGRAM_ACCESS HWIO_RPM_PAGE_SELECT_OUT(0x2)
	
boolean rbcpr_internalize_state(void)
{
    uint32 extern_data_size;
    const rbcpr_externalized_state* extern_data = (const rbcpr_externalized_state*) smem_get_addr(SMEM_CPR_CONFIG, &extern_data_size);
    RPM_CSR_RECONFIGURE_MSGRAM_ACCESS;
    if(!extern_data)
	{
	    RPM_CSR_CONFIGURE_MSGRAM_ACCESS;
        return false;
	}
    
    if(extern_data->version != 2)
	{
	    RPM_CSR_CONFIGURE_MSGRAM_ACCESS;
        return false;
	}	

	CORE_VERIFY(extern_data_size == __ELEMENT_OFFSET(rbcpr_externalized_state, rail_data[extern_data->number_of_rails]));
 
    for(int i=0; i<extern_data->number_of_rails; i++)
    {
        const rbcpr_rail_externalized_state_v1* extern_rail_data = &extern_data->rail_data[i];
        
        if(extern_rail_data->enablement == RBCPR_DISABLED)
            continue;

        int rbcpr_rail_index;
        bool rail_found = false;
        
        for(rbcpr_rail_index=0; rbcpr_rail_index<rbcpr_bsp_data.num_rails; rbcpr_rail_index++)
        {
            if(!strcmp(rbcpr_bsp_data.rails[rbcpr_rail_index].rail_const_info->rail_name, extern_rail_data->rail_name))
            {
                rail_found = true;
                break;
            }
        }
        
        if(!rail_found)
            continue;
        
        int railway_rail_id = rail_id(extern_rail_data->rail_name);
        
        for(int j=RAILWAY_SVS_LOW; j<RAILWAY_CORNERS_COUNT; j++)
        {
            railway_set_corner_voltage(railway_rail_id, (railway_corner)j, extern_rail_data->railway_voltages[j]);
        }

        rbcpr_bsp_rail_params_type* bsp_data = (rbcpr_bsp_rail_params_type*) malloc(sizeof(rbcpr_bsp_rail_params_type));
        CORE_VERIFY(bsp_data);
        memset(bsp_data, 0, sizeof(rbcpr_bsp_rail_params_type));

        bsp_data->corners_to_settle_at = NULL;
        bsp_data->number_of_corners_to_settle_at = 0;
        bsp_data->rbcpr_enablement = extern_rail_data->enablement;
        
        CORE_VERIFY(extern_rail_data->corner_params_count <= MAX_RBCPR_LEVELS);
        bsp_data->number_of_target_params = extern_rail_data->corner_params_count;
        bsp_data->target_params = (rbcpr_corner_params_and_fuse_type*) malloc(sizeof(rbcpr_corner_params_and_fuse_type) * bsp_data->number_of_target_params);
        CORE_VERIFY(bsp_data->target_params);
        for(int j=0; j<bsp_data->number_of_target_params; j++)
        {
            memcpy(&bsp_data->target_params[j].corner_params, &extern_rail_data->corner_params[j], sizeof(rbcpr_corner_params_type));
        }

        if(bsp_data->rbcpr_enablement != RBCPR_DISABLED)
        {
            bsp_data->sbl_init_done = true;
        }

        rbcpr_bsp_data.rails[rbcpr_rail_index].bsp_data = bsp_data;
    }

	RPM_CSR_CONFIGURE_MSGRAM_ACCESS;
    return true;
    }
