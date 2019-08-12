/*===========================================================================

  Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
#ifndef RBCPR_H
#define RBCPR_H

#include <stdbool.h>
#include <comdef.h>
#include <DALSysTypes.h>
//#include "HAL_cpr_v2.h"
#include "HAL_cpr.h"
//#include "Clock.h"
#include "cpr_fuses.h"
#include "cpr_enablement.h"
#include "cpr_target_quotients.h"
#include "cpr_closed_loop.h"

#define MPSS_CORNERS_COUNT 10

typedef enum
{
    REF_CLK,
    AHB_CLK,
    CPR_CLK_COUNT
} cpr_clock_id;

typedef struct
{
  //ClockIdType clk_id;
  int clk_id;
  bool        enabled;
} cpr_clock_state;

//typedef struct cpr_image_closed_loop_rail_s* cpr_image_closed_loop_rail_t;

typedef struct cpr_image_closed_loop_rail_s
{
    cpr_clock_state     clock[CPR_CLK_COUNT];
    HAL_cpr_dump_t      hal_core_dump;
    int8                corner_lookup_index[MPSS_CORNERS_COUNT];
    void*               task;
} cpr_image_closed_loop_rail_s;

boolean rbcpr_internalize_state(void);

cpr_corner_params_t* cpr_corner_params(cpr_closed_loop_rail_t rail, cpr_voltage_mode_t level);
uint32 rbcpr_corner_index(cpr_closed_loop_rail_t rail, int corner);


#endif /* RBCPR_H */
