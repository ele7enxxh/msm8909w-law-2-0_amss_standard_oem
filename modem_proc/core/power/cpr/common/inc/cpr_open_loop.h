/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_H
#define CPR_H

#include "cpr_defs.h"
#include "cpr_enablement.h"

void cpr_init(void);

//To do, move to another folder.

typedef struct cpr_open_loop_rail_s* cpr_open_loop_rail_t;

typedef struct cpr_open_loop_rail_s
{
    cpr_rail_id_t                                   rail_id;
    cpr_image_rail_voltage_control_handle_t         voltage_control_handle;
    const cpr_enablement_versioned_rail_config_t*   enablement_config;
    cpr_open_loop_rail_t                            next_link;
} cpr_open_loop_rail_s;


#endif //CPR_H
