/*===========================================================================

  railway_config.h - data related to platform-specific customization

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RAILWAY_CONFIG_H
#define RAILWAY_CONFIG_H

#include "railway.h"
#include "rpm.h"
#include "railway_internal.h"
#include "pmapp_npa.h"

typedef void (*railway_static_cb_type)(uint32);

typedef enum
{
    RAILWAY_RAIL_TYPE_MX = 0,  // Shall be >= logical rails
    RAILWAY_RAIL_TYPE_LOGIC,   // Shall be <= Mx rail
    RAILWAY_RAIL_TYPE_EBI,     // Shall be == Mx rail if votes > rentention
                               // Otherwise, shall be in retention voltage
    RAILWAY_RAIL_TYPES_COUNT,
} railway_rail_type_t;

typedef struct
{
    const char        *vreg_name;

    uint32            vreg_num;

    uint32            pmic_step_size;
   
    railway_rail_type_t   rail_type;

    railway_corner    initial_corner; // Which corner to vote for out of the box (no votes received yet).

    bool              supports_explicit_voltage_requests;

    rpm_resource_type vreg_type;

    uint32            supported_corners_count;
    railway_corner    *supported_corners;   //List of corners that this rail supports

    const uint32      *default_uvs; // Default voltage lookup table.
    
    //Workaround for Mx droop problems. See CR 525065
    railway_aggregation_workaround_fn  aggregation_workaround;
} railway_rail_config_t;

typedef struct
{
    int                    num_rails;
    railway_rail_config_t *rails;
} railway_config_data_t;

extern const railway_config_data_t * const RAILWAY_CONFIG_DATA;

#endif // RPM_CONFIG_H

