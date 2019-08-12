/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/



#include "rbcpr_stats.h"
#include "railway.h"
#include "time_service.h"
#include "image_layout.h"
#include "CoreVerify.h"
#include "comdef.h"
#include "rbcpr.h"

static rbcpr_stats_header_type* rbcpr_stats_header;
extern rbcpr_bsp_type rbcpr_bsp_data;

rbcpr_rail_stats_footer_type* rail_footer(uint32 rail_id);
rbcpr_stats_corner_type* rail_corner(uint32 rail_id, uint32 corner_index);
rbcpr_rail_stats_header_type* rail_header(uint32 rail_id);

static rbcpr_stats_header_type* stats_ptr(void)
{
    return rbcpr_stats_header;
}

static rbcpr_stats_corner_type* rail_corner(uint32 rail_id, uint32 corner_index)
{
    CORE_VERIFY(rail_id<rbcpr_bsp_data.num_rails);
    CORE_VERIFY(corner_index<rbcpr_bsp_data.rails[rail_id].bsp_data->number_of_target_params);
    rbcpr_stats_corner_type* corner_start = (rbcpr_stats_corner_type*)((char*)rail_header(rail_id)+sizeof(rbcpr_rail_stats_header_type));
    return &corner_start[corner_index];
}

static rbcpr_rail_stats_footer_type* rail_footer(uint32 rail_id)
{
    CORE_VERIFY(rail_id<rbcpr_bsp_data.num_rails);
    if(rbcpr_bsp_data.rails[rail_id].bsp_data->number_of_target_params>0)
    {
        rbcpr_stats_corner_type* last_corner = rail_corner(rail_id,
            rbcpr_bsp_data.rails[rail_id].bsp_data->number_of_target_params-1);
        return (rbcpr_rail_stats_footer_type*)((char*)last_corner+sizeof(rbcpr_stats_corner_type));
    } else {
        return (rbcpr_rail_stats_footer_type*)((char*)rail_header(rail_id)+sizeof(rbcpr_rail_stats_header_type));
    }
}

static rbcpr_rail_stats_header_type* rail_header(uint32 rail_id)
{
    CORE_VERIFY(rail_id<rbcpr_bsp_data.num_rails);
    if(rail_id==0)
        return (rbcpr_rail_stats_header_type*)((char*)stats_ptr()+sizeof(rbcpr_stats_header_type));
    else return (rbcpr_rail_stats_header_type*)((char*)rail_footer(rail_id-1)+sizeof(rbcpr_rail_stats_footer_type));
}


static uint32 size_of_stats(void)
{
    if(rbcpr_bsp_data.num_rails==0)
    {
        return sizeof(rbcpr_stats_header_type);
    }

    char* byte_past_the_end = (char*)rail_footer(rbcpr_bsp_data.num_rails-1)+sizeof(rbcpr_rail_stats_footer_type);
    return byte_past_the_end - (char*)stats_ptr();
}

void rbcpr_stats_init(void)
{
    uint32 number_of_rails = rbcpr_bsp_data.num_rails;
	rbcpr_stats_header = (rbcpr_stats_header_type*)rpm_image_section_array[RPM_CPR_STATS].section_base;
    uint32 stats_size = size_of_stats();
	assert(&((char*)stats_ptr())[stats_size]<=rpm_image_section_array[RPM_CPR_STATS].section_end);

    //Mem fill this thing to clear it.
    memset((void*)rbcpr_stats_header, 0, stats_size);

    stats_ptr()->count = number_of_rails;
    
    for(int i=0; i<number_of_rails; i++)
        {
        rbcpr_rail_stats_header_type* header = rail_header(i);
        header->num_corners = rbcpr_bsp_data.rails[i].bsp_data->number_of_target_params;
        header->num_latest_recommends = RBCPR_LATEST_RECOMMENDS;
        }

    stats_ptr()->status++;
}

void rbcpr_stats_set_fuse_microvolts(uint32 rail_id, uint32 corner_index, int32 value)
{
    rail_corner(rail_id, corner_index)->fuse_microvolts=value;
    stats_ptr()->status++;
}

void rbcpr_stats_set_corner_voltage(uint32 rail_id, uint32 corner_index, uint32 uv)
{
    rbcpr_stats_corner_type* corner_ptr = rail_corner(rail_id, corner_index);
    corner_ptr->programmed_microvolts = uv;

    //Shuffle down the recommended list.
    memmove(&corner_ptr->recommended[1], &corner_ptr->recommended[0], 
            sizeof(rbcpr_stats_corner_recommendation)*(RBCPR_LATEST_RECOMMENDS-1));
    corner_ptr->recommended[0].microvolts=uv;
    corner_ptr->recommended[0].timestamp=time_service_now();
    
    stats_ptr()->status++;
}

void rbcpr_stats_increment_isr_counter(uint32 rail_id, uint32 corner_index)
{
    rail_corner(rail_id, corner_index)->isr_counter++;
    stats_ptr()->status++;
}

void rbcpr_stats_increment_min_counter(uint32 rail_id, uint32 corner_index)
{
    rail_corner(rail_id, corner_index)->min_counter++;
    stats_ptr()->status++;
}

void rbcpr_stats_increment_max_counter(uint32 rail_id, uint32 corner_index)
{
    rail_corner(rail_id, corner_index)->max_counter++;
}

void rbcpr_stats_set_railway_settings(uint32 rail_id, const railway_settings* settings)
{
    rbcpr_rail_stats_footer_type* footer = rail_footer(rail_id);
    footer->current_corner = settings->mode;
    footer->railway_microvolts = settings->microvolts;
    stats_ptr()->status++;
}
