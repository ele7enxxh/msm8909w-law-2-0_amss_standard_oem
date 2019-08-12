/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#include "rbcpr.h"
#include "HAL_rbcpr.h"

typedef struct
{
    uint32                  microvolts;
    uint64                  timestamp;
} rbcpr_stats_corner_recommendation;

typedef struct
{
    // The fuse voltage (which is a offset)
    int32                     fuse_microvolts;
    // The programmed voltage to railway
    uint32                    programmed_microvolts;
    // The # of interrupts
    uint32                    isr_counter;
    // The # of exception events - VDDCX voltage hitting MIN
    uint32                    min_counter;
    // The # of exception events - VDDCX voltage hitting MAX
    uint32                    max_counter;
    // The latest recommended voltages with timestamp
    rbcpr_stats_corner_recommendation recommended[RBCPR_LATEST_RECOMMENDS];
} rbcpr_stats_corner_type;

typedef struct
{
    // The # of corners supported (matched with HAL_RBCPR_CORNERS_COUNT)
    uint32                    num_corners;
    // The # of latest recommendations (matched with RBCPR_LATEST_RECOMMENDS)
    uint32                    num_latest_recommends;
} rbcpr_rail_stats_header_type;

typedef struct
{
    // The current active corner
    railway_corner            current_corner;
    // The current active railway voltage
    uint32                    railway_microvolts;
    // Whether we're off-corner
    uint32                    off_corner;
    // The target quotient margin in mV
    uint32                    margin;
} rbcpr_rail_stats_footer_type;

typedef struct
{
    // Number of rails that we have CPR Stats for
    uint32                    count;
    // The counter gets incremented whenever there is an update
    uint32                    status;
} rbcpr_stats_header_type;

/*

//CPR Stats header
//rail 0
////header
////corners0
////corners1
////corners2
////corners3
////footer
//rail 1
////header
////corners0
////corners1
////corners2
////footer

*/


void rbcpr_stats_init(void);
void rbcpr_stats_set_fuse_microvolts(uint32 rail_id, uint32 corner_index, int32 value);
void rbcpr_stats_set_corner_voltage(uint32 rail_id, uint32 corner_index, uint32 uv);
void rbcpr_stats_increment_isr_counter(uint32 rail_id, uint32 corner_index);
void rbcpr_stats_increment_min_counter(uint32 rail_id, uint32 corner_index);
void rbcpr_stats_increment_max_counter(uint32 rail_id, uint32 corner_index);
void rbcpr_stats_set_railway_settings(uint32 rail_id, const railway_settings* settings);

