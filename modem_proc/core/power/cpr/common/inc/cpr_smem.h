/*===========================================================================
                              cpr_smem.h

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

#include "cpr_closed_loop.h"

#define CPR_SMEM_MAX_RAILS      8

typedef enum
{
    CPR_EXTERNALIZED_STATE_ENABLED_OPEN_LOOP,
    CPR_EXTERNALIZED_STATE_ENABLED_CLOSED_LOOP,
    CPR_EXTERNALIZED_STATE_NO_CONFIG_AVAILABLE,
} cpr_extrnalized_state_rail_enablement_t;

typedef struct
{
    uint32                                      rail_id; // Actually a cpr_rail_id_t
    uint32                                      enablement; // cpr_extrnalized_state_rail_enablement_t
    uint32                                      voltage_config_count;
    uint32                                      voltage_config_offset;
} cpr_externalized_state_rail_index_t;

typedef struct
{
    cpr_corner_params_t         corner_params;  
    uint32                      voltage;
} cpr_externalized_state_closed_loop_level_t;

typedef struct
{
    cpr_voltage_mode_t          voltage_mode;  
    uint32                      voltage;
} cpr_externalized_state_open_loop_level_t;

typedef struct
{
    uint32                                  version;
    cpr_externalized_state_rail_index_t     rail_info_index[CPR_SMEM_MAX_RAILS];
    uint32                                  rail_info_count;
} cpr_externalized_state_header;
