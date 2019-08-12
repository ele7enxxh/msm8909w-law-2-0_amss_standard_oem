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
#include "cpr_smem.h"
#include "CoreVerify.h"
#ifdef WINSIM
#define SMEM_CPR_CONFIG 1024  /* TODO: fix whatever smem will do for the PC build*/
#define smem_alloc(x,y) (0)
#define smem_get_addr(x,y) (0)
#else
#include "smem.h"
#include "stringl/stringl.h"
#endif
#include "cpr_enablement.h"
#include "cpr_open_loop.h"
#include "cpr_image.h"
#include <string.h>

#define __ELEMENT_OFFSET(type, element) \
    (uint32)(&((type *)0x0)->element)

#define SMEM_ALIGN_SZ_64BIT(VALUE) ( ((VALUE)+7) & ~0x00000007)

extern cpr_open_loop_rail_t cpr_open_loop_rail_root;
extern cpr_closed_loop_rail_t cpr_closed_loop_rail_root;

void cpr_externalize_state(void)
{
    const uint32 size_of_externalized_data = 0x400;     //Reserve 1K for now
    cpr_externalized_state_header* extern_data = (cpr_externalized_state_header*) smem_alloc(SMEM_CPR_CONFIG, size_of_externalized_data);
    
    CORE_VERIFY_PTR(extern_data);

    const uint8* end_of_buffer = (uint8*)extern_data + size_of_externalized_data;
    extern_data->version = 1;
    extern_data->rail_info_count = 0;
    
    uint8* buffer_pointer = (uint8*)extern_data;
    buffer_pointer += sizeof(cpr_externalized_state_header);
    buffer_pointer = (uint8*)SMEM_ALIGN_SZ_64BIT((uintnt)buffer_pointer);
    
    CORE_VERIFY((buffer_pointer - (uint8*)extern_data)<size_of_externalized_data);
    
    cpr_open_loop_rail_t open_loop_config = cpr_open_loop_rail_root;
    
    while(open_loop_config)
    {
        cpr_externalized_state_rail_index_t* rail_info_header = &extern_data->rail_info_index[extern_data->rail_info_count];

        extern_data->rail_info_count++;
        CORE_VERIFY(extern_data->rail_info_count<=CPR_SMEM_MAX_RAILS);

        rail_info_header->rail_id = open_loop_config->rail_id;
        rail_info_header->enablement = CPR_EXTERNALIZED_STATE_ENABLED_OPEN_LOOP;
        rail_info_header->voltage_config_count = open_loop_config->enablement_config->supported_level_count;
        
        cpr_externalized_state_open_loop_level_t* open_loop_level_config = (cpr_externalized_state_open_loop_level_t*)buffer_pointer;   

        //Make sure we've got enough space to fit the config. 
        CORE_VERIFY((uint8*)&open_loop_level_config[rail_info_header->voltage_config_count]<end_of_buffer);
        buffer_pointer = (uint8*)&open_loop_level_config[rail_info_header->voltage_config_count];
        buffer_pointer = (uint8*)SMEM_ALIGN_SZ_64BIT((uintnt)buffer_pointer);
        
        rail_info_header->voltage_config_offset = (uint8*)open_loop_level_config - (uint8*)extern_data;
        
        for(int i=0; i<rail_info_header->voltage_config_count; i++)
        {
            cpr_voltage_mode_t voltage_mode = open_loop_config->enablement_config->supported_level[i].voltage_mode;
            open_loop_level_config[i].voltage_mode = voltage_mode;
            open_loop_level_config[i].voltage = cpr_image_get_rail_mode_voltage(open_loop_config->voltage_control_handle, voltage_mode);
        }

        open_loop_config = open_loop_config->next_link;
    }

    cpr_closed_loop_rail_t closed_loop_config = cpr_closed_loop_rail_root;
    
    while(closed_loop_config)
    {
        cpr_externalized_state_rail_index_t* rail_info_header = &extern_data->rail_info_index[extern_data->rail_info_count];

        extern_data->rail_info_count++;
        CORE_VERIFY(extern_data->rail_info_count<=CPR_SMEM_MAX_RAILS);

        rail_info_header->rail_id = closed_loop_config->rail_config->rail_id;
        rail_info_header->enablement = CPR_EXTERNALIZED_STATE_ENABLED_CLOSED_LOOP;
        rail_info_header->voltage_config_count = closed_loop_config->target_params_count;
        
        cpr_externalized_state_closed_loop_level_t* closed_loop_level_config = (cpr_externalized_state_closed_loop_level_t*)buffer_pointer;   
        //Make sure we've got enough space to fit the config.
        
        CORE_VERIFY((uint8*)&closed_loop_level_config[rail_info_header->voltage_config_count]<end_of_buffer);
        buffer_pointer = (uint8*)&closed_loop_level_config[rail_info_header->voltage_config_count];
        buffer_pointer = (uint8*)SMEM_ALIGN_SZ_64BIT((uintnt)buffer_pointer);
        
        rail_info_header->voltage_config_offset = (uint8*)closed_loop_level_config - (uint8*)extern_data;
        
        for(int i=0; i<rail_info_header->voltage_config_count; i++)
        {
            cpr_corner_params_t* corner_params = &closed_loop_config->target_params[i];
            cpr_voltage_mode_t voltage_mode = corner_params->voltage_mode;
            closed_loop_level_config[i].voltage = cpr_image_get_rail_mode_voltage(closed_loop_config->voltage_control_handle, voltage_mode);
            cpr_image_memscpy(&closed_loop_level_config[i].corner_params, sizeof(cpr_corner_params_t), corner_params, sizeof(cpr_corner_params_t));
        }
        
        closed_loop_config = closed_loop_config->next_link;
    }
}
extern void cpr_enable_init_fn_open_loop_from_smem(cpr_rail_id_t rail_id, const cpr_externalized_state_open_loop_level_t* open_loop_levels, uint32 open_loop_level_count, cpr_enablement_stage_t enablement_stage);
extern void cpr_enable_init_fn_closed_loop_from_smem(cpr_rail_id_t rail_id, const cpr_externalized_state_closed_loop_level_t* closed_loop_levels, uint32 closed_loop_level_count, cpr_enablement_stage_t enablement_stage);


void cpr_enable_init_fn_read_config_from_smem(cpr_rail_id_t rail_id, const struct cpr_enablement_versioned_rail_config_t* rail_enablement_config, cpr_enablement_stage_t enablement_stage)
{
#ifndef WINSIM  /* c99 in VS2013 is limited */
    uint32 extern_data_size;
    const cpr_externalized_state_header* extern_data = (const cpr_externalized_state_header*) smem_get_addr(SMEM_CPR_CONFIG, &extern_data_size);

	cpr_image_prepare_to_access_smem();
    CORE_VERIFY_PTR(extern_data);
    
    CORE_VERIFY(extern_data->version == 1);
    
    const cpr_externalized_state_rail_index_t* rail_header = NULL;
    
    for(int i=0; i<extern_data->rail_info_count; i++)
    {
        if(extern_data->rail_info_index[i].rail_id == rail_id)
        {
            rail_header = &extern_data->rail_info_index[i];
            break;
        }
    }
    
    CORE_VERIFY(rail_header);
    
    if(rail_header->enablement == CPR_EXTERNALIZED_STATE_ENABLED_OPEN_LOOP)
    {
        //Copy data from SMEM to the stack
        const uint32 open_loop_level_count = rail_header->voltage_config_count;
        cpr_externalized_state_open_loop_level_t open_loop_levels[open_loop_level_count];
        
        cpr_image_memscpy(open_loop_levels, sizeof(open_loop_levels),
            &((const uint8*)extern_data)[rail_header->voltage_config_offset], 
            open_loop_level_count * sizeof(cpr_externalized_state_open_loop_level_t));

        cpr_image_smem_access_complete();

        cpr_enable_init_fn_open_loop_from_smem(rail_id, open_loop_levels, open_loop_level_count, enablement_stage);
        return;
    }
    
    CORE_VERIFY(rail_header->enablement == CPR_EXTERNALIZED_STATE_ENABLED_CLOSED_LOOP);

    //Copy data from SMEM to the stack
    const uint32 closed_loop_level_count = rail_header->voltage_config_count;
    cpr_externalized_state_closed_loop_level_t closed_loop_levels[closed_loop_level_count];

    cpr_image_memscpy(closed_loop_levels, sizeof(closed_loop_levels),
        &((const uint8*)extern_data)[rail_header->voltage_config_offset], 
        closed_loop_level_count * sizeof(cpr_externalized_state_closed_loop_level_t));

	cpr_image_smem_access_complete();

    cpr_enable_init_fn_closed_loop_from_smem(rail_id, closed_loop_levels, closed_loop_level_count, enablement_stage);
#endif
}

const cpr_enablement_init_params_t CPR_ENABLE_READ_CONFIG_FROM_SMEM =
{
    .enablement_type        = CPR_ENABLEMENT_READ_CONFIG_FROM_SMEM_ID,
    .enablement_fn          = cpr_enable_init_fn_read_config_from_smem,
};
