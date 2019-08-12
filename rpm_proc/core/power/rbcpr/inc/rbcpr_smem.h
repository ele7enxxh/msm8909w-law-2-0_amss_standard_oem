/*===========================================================================
                              rbcpr_smem.h

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

#include "rbcpr.h"

#define MAX_RBCPR_LEVELS 7
#define MAX_RAIL_NAME_LENGTH    0x10

typedef struct
{
    rbcpr_enablement_type       enablement;
    char                        rail_name[MAX_RAIL_NAME_LENGTH];
    uint32                      railway_voltages[RAILWAY_CORNERS_COUNT];
    uint32                      corner_params_count;
    rbcpr_corner_params_type    corner_params[MAX_RBCPR_LEVELS];
} rbcpr_rail_externalized_state_v1;

typedef struct
{
    uint32 version;
    uint32 number_of_rails;
    rbcpr_rail_externalized_state_v1 rail_data[];
} rbcpr_externalized_state;

