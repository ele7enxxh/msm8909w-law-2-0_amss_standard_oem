
/*!
  @file
  wwan_coex_spur_list.c

  @brief
  This file implements the spur calculations of LIMTSMGR COEX module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/src/wwan_coex_spur_list.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/04/14   ag      Added TTL markers for profiling
10/27/14   jm      KW Fixes
09/24/14   sg      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "cxm.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_i.h"
#include "lmtsmgr_msgr.h"
#include "wwan_coex.h"
#include "wwcoex_conflict_table.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/

#ifndef MCS_LIMITS_LOW_MEMORY

extern  uint16                     wwcoex_spur_info_idx;
extern  cxm_spur_info_s            wwcoex_spur_info[WWCOEX_MAX_SPUR_TABLE_SIZE];

#endif /* MCS_LIMITS_LOW_MEMORY */


/*=============================================================================

                            STATIC INPUT

=============================================================================*/

#ifndef MCS_LIMITS_LOW_MEMORY

/* RF spur details are stored in this array */
wwcoex_rf_spur_entry_type  rf_spur_table[WWCOEX_MAX_RF_SPUR_ENTRIES] =
{
  
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    0,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    0,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -610,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    0,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0},  /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    0,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -4, 0},  /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,              /* CA1 band (sys band) */
    0,               /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,              /* CA1 band (sys band) */
    0,               /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,              /* CA1 band (sys band) */
    0,               /* CA2 band (sys band) */
    -530,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0},   /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,              /* CA1 band (sys band) */
    0,               /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -10, 0},  /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,              /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -500,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, 0, 0},    /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,              /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -740,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -1, 0},   /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,              /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -820,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -2, 0},   /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,              /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -470,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, -2, 0},   /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,              /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -790,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, -4, 0},   /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,              /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -750,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -6, 0},   /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,              /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -600,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0},  /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,              /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -5, 0},  /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,              /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0},   /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,              /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -760,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -10, 0},  /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -780,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -600,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -550,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -590,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    80,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    84,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    84,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    84,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    84,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    87,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -480,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    87,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -770,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -1, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    87,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -820,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    87,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 9, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    81,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -740,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    81,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    81,               /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -580,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    81,               /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -750,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -740,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -620,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -540,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -9, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -520,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    80,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    84,              /* CA2 band (sys band) */
    -740,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    84,              /* CA2 band (sys band) */
    -530,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    87,              /* CA2 band (sys band) */
    -470,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    87,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    87,              /* CA2 band (sys band) */
    -740,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    81,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    81,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    81,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    81,              /* CA2 band (sys band) */
    -720,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    163,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -9, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    163,              /* CA2 band (sys band) */
    -750,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -9, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    163,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 13, -11, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -440,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, -1, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -760,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 7, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -760,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 9, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -750,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -830,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 13, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    167,              /* CA2 band (sys band) */
    -600,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -1, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    167,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 13, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    167,              /* CA2 band (sys band) */
    -540,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    167,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 9, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    163,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    163,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    163,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -450,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -1, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -770,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    168,              /* CA2 band (sys band) */
    -750,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    167,              /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    167,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    167,              /* CA2 band (sys band) */
    -520,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    167,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -760,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -600,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -550,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -590,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    120,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    122,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -440,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    122,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    122,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -620,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    122,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    122,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 7, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    122,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    122,               /* CA1 band (sys band) */
    48,              /* CA2 band (sys band) */
    -760,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -600,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -11, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -550,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -540,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -610,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    126,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -620,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    126,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -620,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    126,               /* CA1 band (sys band) */
    47,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    126,               /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -580,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 1, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    126,               /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -800,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    126,               /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -750,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    123,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    123,               /* CA1 band (sys band) */
    44,              /* CA2 band (sys band) */
    -750,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    123,               /* CA1 band (sys band) */
    43,              /* CA2 band (sys band) */
    -550,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -550,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -520,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -530,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -590,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -560,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -540,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 13, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -520,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -9, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -610,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    120,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    122,              /* CA2 band (sys band) */
    -460,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    122,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -1, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    122,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    122,              /* CA2 band (sys band) */
    -720,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 9, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    122,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    122,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    122,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -600,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -11, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -550,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -540,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -710,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -12, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    147,              /* CA2 band (sys band) */
    -600,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    126,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    126,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    126,              /* CA2 band (sys band) */
    -760,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    126,              /* CA2 band (sys band) */
    -530,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    126,              /* CA2 band (sys band) */
    -850,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 7, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    126,              /* CA2 band (sys band) */
    -770,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    123,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    123,              /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    123,              /* CA2 band (sys band) */
    -520,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -550,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -520,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    139,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 5, -10, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -570,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -840,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 7, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -720,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -450,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -740,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -1, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 7, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -510,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -560,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -830,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -9, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -560,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -690,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -770,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 9, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -720,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -730,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -720,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -520,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 3, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -840,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 7, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    157,              /* CA2 band (sys band) */
    -720,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -450,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 2, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -1, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -740,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 7, -3, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    158,              /* CA2 band (sys band) */
    -670,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -510,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -650,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 10, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -680,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 6, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -560,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    48,               /* CA1 band (sys band) */
    159,              /* CA2 band (sys band) */
    -830,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -9, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -550,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -4, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -630,            /* Spur level (units: dBm*10) */
    2,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 4, -2, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -660,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 8, -5, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -770,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 9, -6, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -720,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 11, -7, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    0,               /* TX band (sys band) */
    47,               /* CA1 band (sys band) */
    160,              /* CA2 band (sys band) */
    -720,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {0, 12, -8, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    168,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    0,              /* CA2 band (sys band) */
    -760,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {2, -3, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    167,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    0,              /* CA2 band (sys band) */
    -530,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {2, -4, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    147,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    0,              /* CA2 band (sys band) */
    -640,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {4, -2, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    157,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    0,              /* CA2 band (sys band) */
    -800,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {2, -5, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    158,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    0,              /* CA2 band (sys band) */
    -700,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {2, -3, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    159,               /* TX band (sys band) */
    44,               /* CA1 band (sys band) */
    0,              /* CA2 band (sys band) */
    -530,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {2, -4, 0, 0}, /* Spur Coeffecients */
  },
  {
    0,               /* Band mask (generated run time) */
    159,               /* TX band (sys band) */
    43,               /* CA1 band (sys band) */
    0,              /* CA2 band (sys band) */
    -810,            /* Spur level (units: dBm*10) */
    1,               /* Spur type (victim vco) */
    0,               /* Notch depth (k value) */
    {4, -10, 0, 0}, /* Spur Coeffecients */
  },
};

#endif /* MCS_LIMITS_LOW_MEMORY */


/*=============================================================================

                                FUNCTIONS

=============================================================================*/

#ifndef MCS_LIMITS_LOW_MEMORY

/*=============================================================================

  FUNCTION:  wwcoex_get_lo_freq

=============================================================================*/
/*!
    @brief
    Uses the XO error and returns the LO frequency
 
    @return
    TRUE
*/
/*===========================================================================*/
static void wwcoex_get_lo_freq
(
  cxm_tech_type   tech,
  uint32          freq,    /* Units: KHz */
  uint32         *lo_freq, /* Units: Hz  */
  uint16         *xo_unc   /* Units: Parts per billion (ppb) */
)
{
  int32  xo_err;
  float  freq_corr;

  if (!LMTSMGR_IS_TECH_VALID(tech))
  {
    LMTSMGR_MSG_1(FATAL, "ERROR:wwcoex_get_lo_freq tech input invalid %d",tech);
    *lo_freq = freq*1000;
    *xo_unc = 0;
    return;
  }

  /* Retrieve the XO error provided by XO manager */
  xo_err  = lmtsmgr.tech_state[tech].currList.xo_error;
  *xo_unc = lmtsmgr.tech_state[tech].currList.xo_uncertainty; 

  /* The XO error is in PPB units. The Frequency is in KHz.
     Convert the XO error from PPB to KHz */
  freq_corr = (float)(((float)xo_err*(float)freq)/1000000);

  /* Add the XO error to frequency to get the corrected LO frequency */
  *lo_freq = (freq*1000) + (int)(freq_corr);
    
  return;
}

/*=============================================================================

  FUNCTION:  wwcoex_get_spur_coeff

=============================================================================*/
/*!
    @brief
    Searches the list for the band combination and provides the spur coeff
 
    @return
    Spur coeffecients if the band combination is found.
    Spur coeffecients are 8 bits each and they are packed to 32 bits.
*/
/*===========================================================================*/
static boolean wwcoex_get_spur_coeff
(
  uint32                      vband,
  uint32                      aband,
  uint8                       vvco,
  uint8                       avco,
  cxm_tech_link_direction     direction,
  wwcoex_spur_entry_type     *spur_coeff
)
{
  uint32 i, start_idx, end_idx, srch_idx, v_idx = WWCOEX_MAX_COEFF, a_idx = WWCOEX_MAX_COEFF;
  uint64 arr_entry_val, mask = 0;
  boolean found=FALSE;
  uint32 rf_entry_count = 0;

  TTLMARKER(LMTSMGR_GET_SPUR_COEFF_IN);
  
  /* Create the mask based on the agressor and victim vco */
  if((direction == CXM_LNK_DRCTN_UL) &&
     (vvco == 1))
  {
    v_idx = WWCOEX_CA1_N;
    a_idx = WWCOEX_TX_M;
    
    mask = (aband&0xFFFF) | 
           ((vband&0xFFFF) << 16);
  }

  else if((direction == CXM_LNK_DRCTN_UL) &&
          (vvco == 2))
  {
    v_idx = WWCOEX_CA2_P;
    a_idx = WWCOEX_TX_M;
    
    mask = (aband&0xFFFF) | 
           (((uint64)vband&0xFFFF) << 32);
  }

  else if((direction == CXM_LNK_DRCTN_DL) &&
          (vvco == 1))
  {

    v_idx = WWCOEX_CA1_N;
    a_idx = WWCOEX_CA2_P;
    
    mask = (((uint64)aband&0xFFFF)<<32) | 
           ((vband&0xFFFF) << 16);
  }
  
  else if((direction == CXM_LNK_DRCTN_DL) &&
          (vvco == 2))
  {
    v_idx = WWCOEX_CA2_P;
    a_idx = WWCOEX_CA1_N;
    
    mask = ((aband&0xFFFF)<<16) | 
           (((uint64)vband&0xFFFF) << 32);
  }

  start_idx = 1;
  end_idx = WWCOEX_MAX_RF_SPUR_ENTRIES - 1;
  
  while (1)
  {
    if ((end_idx < start_idx) ||
        (start_idx == 0))
    {
      /* could not find the val in the list */
      break;
    }
  
    srch_idx = start_idx + ((end_idx - start_idx) >> 1);

    arr_entry_val = rf_spur_table[srch_idx].band_mask;
    
    if (mask == arr_entry_val)
    {
      /* Found it */
      found = TRUE;
      break;
    }
    else if (mask > arr_entry_val)
    {
      start_idx = srch_idx + 1;
    }
    else if (mask < arr_entry_val)
    {
      end_idx = srch_idx - 1;
    }
  } 

  if(found == TRUE)
  {
    /* Reset found and now search for matching band mask
       along with matching victim VCO */
    found = FALSE;

    /* If index is lower than 6 then set it to 6 to avoid array
       overflow */
    if(srch_idx < WWCOEX_MAX_CONCURRENT_SPURS)
    {
      srch_idx = WWCOEX_MAX_CONCURRENT_SPURS;
    }
    
    /* The table could have multie entries that match the mask.
       So even after one entry is found, the adjacent entries are 
       searched to check for matches
       Rhe max concurrent entries supported is 6 (MACRO) */
    for(i = (srch_idx-WWCOEX_MAX_CONCURRENT_SPURS);
        i < (srch_idx+WWCOEX_MAX_CONCURRENT_SPURS);
        i++)
    {
      /* Check if index is valid for both the arrays */
      if ((i >= WWCOEX_MAX_RF_SPUR_ENTRIES) ||
          (rf_entry_count >= WWCOEX_MAX_CONCURRENT_SPURS))
      {
        break;
      }

      /* check if mask matches */
      if(rf_spur_table[i].band_mask != mask)
      {
        continue;
      }

      /* Mask matched. Check if the spur type matches victim VCO */
      if( (rf_spur_table[i].spur_type == vvco) && 
          (a_idx < WWCOEX_MAX_COEFF) &&
          (v_idx < WWCOEX_MAX_COEFF) )
      {
        /* This entry matches the spur criteria. Copy the results */
        found = TRUE;
        
        spur_coeff->rf_entry[rf_entry_count].a_coeff = 
            rf_spur_table[i].coeff[a_idx];
        spur_coeff->rf_entry[rf_entry_count].v_coeff = 
            rf_spur_table[i].coeff[v_idx];
        spur_coeff->rf_entry[rf_entry_count].spur_level = 
            rf_spur_table[i].spur_level;
        spur_coeff->rf_entry[rf_entry_count].spur_type = 
            rf_spur_table[i].spur_type;
        spur_coeff->rf_entry[rf_entry_count].notch_depth = 
            rf_spur_table[i].notch_depth;
        
        rf_entry_count++;
      }
    }
  }
  
  spur_coeff->count = rf_entry_count;
  
  TTLMARKER(LMTSMGR_GET_SPUR_COEFF_OUT);
  return found;
  
}

/*=============================================================================

  FUNCTION:  wwcoex_gen_spur_band_mask

=============================================================================*/
/*!
    @brief
    Generate a unique spur band mask for a given band combination
 
    @details
      Mask of aggr and victim bands:
        1st lower order 8 bits indicate Tx band,
        2nd lower order 8 bits indicate CA1 band,
        3rd lower order 8 bits indicate CA2 band,
        If a coeffecient does not exist the band is 0
    @return
     void

*/
/*===========================================================================*/
static void wwcoex_gen_spur_band_mask()
{
  uint32 i,j;
  wwcoex_rf_spur_entry_type temp_entry;

  /* Create the band mask */
  for(i=0;i<WWCOEX_MAX_RF_SPUR_ENTRIES;i++)
  {
    rf_spur_table[i].band_mask = 
      (rf_spur_table[i].tx_band) | 
      (rf_spur_table[i].ca1_band << 16) |
      ((uint64)rf_spur_table[i].ca2_band << 32);
  }

  /* Sort the list in the order of band mask */
  for (i=0;i<WWCOEX_MAX_RF_SPUR_ENTRIES-1; i++)
  {
    for (j=i+1; j<WWCOEX_MAX_RF_SPUR_ENTRIES; j++)
    {
      if (rf_spur_table[i].band_mask > rf_spur_table[j].band_mask)
      {
        temp_entry = rf_spur_table[i];
        rf_spur_table[i] = rf_spur_table[j];
        rf_spur_table[j] = temp_entry;
      }
    }
  }
}

#endif /* MCS_LIMITS_LOW_MEMORY */

/*=============================================================================

  FUNCTION:  wwcoex_spur_compare_freqs

=============================================================================*/
/*!
    @brief
    Compare frequencies to check for spur issues between the frequencies
 
    @return
    TRUE
*/
/*===========================================================================*/
boolean wwcoex_spur_compare_freqs
(
  wwcoex_spur_freqcompare_in*   compare_in,
  wwcoex_spur_freqcompare_out*  compare_out
)
{

#ifndef MCS_LIMITS_LOW_MEMORY
  
  cxm_tech_link_info_s *a_info, *v_info;
  wwcoex_spur_entry_type spur_coeff;
  cxm_tech_type a_tech, v_tech;
  uint32  a_lo_freq, v_lo_freq, i, v_freq_id, spur_count = 0;
  int64   a_spur_freq, v_spur_freq, spur_freq;
  boolean found = FALSE;
  uint16 spur_idx, a_xo_unc, v_xo_unc, handle = WWCOEX_SPUR_HANDLE_NONE;

  TTLMARKER(LMTSMGR_SPUR_COMP_IN);
  
  /* Check if input is valid */
  if (compare_in == NULL || compare_out == NULL)
  {
    TTLMARKER(LMTSMGR_SPUR_COMP_ERR1);
    return FALSE;
  }

  /* Setup default action and handle */
  compare_out->action = ACTION_NONE;
  compare_out->handle = WWCOEX_SPUR_HANDLE_NONE;

  /* Initialize the input to get spur coeff */
  a_tech = compare_in->atech;
  v_tech = compare_in->vtech;
  a_info = &compare_in->atfreqinfo.link_info;
  v_info = &compare_in->vtfreqinfo.link_info;

  /* If the link is not of serving cell type then 
     return spur unknown */
  if((v_info->type != CXM_LNK_TYPE_UNUSED) ||
     (a_info->type != CXM_LNK_TYPE_UNUSED))
  {
    compare_out->action = ACTION_SPUR_UNKNOWN;
    compare_out->handle = WWCOEX_SPUR_HANDLE_UNKNOWN;
    TTLMARKER(LMTSMGR_SPUR_COMP_ERR2);
    return TRUE;
  }

  /* Get spur coeffecients and calculate spur */
  found = wwcoex_get_spur_coeff(v_info->band,
                                a_info->band,
                                compare_in->vtech_vco,
                                compare_in->atech_vco,
                                a_info->direction,
                                &spur_coeff
                               );
   
  /* If the band combination has valid coeffecients then 
     proceed with computing spur */
  if (found == TRUE)
  {
    TTLMARKER(LMTSMGR_SPUR_FOUND_IN);
    spur_idx = wwcoex_spur_info_idx;

    wwcoex_get_lo_freq(a_tech,a_info->frequency, &a_lo_freq, &a_xo_unc);
    wwcoex_get_lo_freq(v_tech,v_info->frequency, &v_lo_freq, &v_xo_unc);
    v_freq_id = compare_in->vtfreqinfo.freqid;

    /* check for array overflow */
    if(spur_coeff.count > WWCOEX_MAX_CONCURRENT_SPURS)
    {
      spur_coeff.count = WWCOEX_MAX_CONCURRENT_SPURS;
    }
    
    for(i=0;i<spur_coeff.count;i++)
    {
      /* Use the RFC equation and get the spur frequency 
         m*txlo + n*ca1_rxlo + p*ca2_rxlo = victim_lo ± (victim BW/2) */
      a_spur_freq = (int64)((int64)(spur_coeff.rf_entry[i].a_coeff)*(int64)a_lo_freq);
      v_spur_freq = (int64)((int64)(spur_coeff.rf_entry[i].v_coeff)*(int64)v_lo_freq);
      spur_freq   = (int64)(a_spur_freq + v_spur_freq);

      if((spur_freq >= (v_lo_freq-(v_info->bandwidth/2))) &&
         (spur_freq <= (v_lo_freq+(v_info->bandwidth/2))))
      {
        /* Spur exists.Copy the result to the array. Increase the count */
        wwcoex_spur_info[spur_idx].notch_depth = 
          spur_coeff.rf_entry[i].notch_depth;
        wwcoex_spur_info[spur_idx].spur_level = 
          (spur_coeff.rf_entry[i].spur_level*10);
        wwcoex_spur_info[spur_idx].spur_type = 
          spur_coeff.rf_entry[i].spur_type;
        wwcoex_spur_info[spur_idx].spur_freq_offset = 
          (int32)(spur_freq-v_lo_freq);
        wwcoex_spur_info[spur_idx].victim_freq_id = 
          v_freq_id;
        wwcoex_spur_info[spur_idx].spur_freq_uncertainty = 
          v_xo_unc;
        
        spur_count++;
        spur_idx = (spur_idx+1)%WWCOEX_MAX_SPUR_TABLE_SIZE;
      }
    }
    TTLMARKER(LMTSMGR_SPUR_FOUND_OUT);
  }

  /* If spurs are detected in the victim bandwidth then store the handle
     handle has 2 parts - 12 bits of index. 4 bits of count
     Ex: 2 spurs are detected and stored at index 0 then the handle = 0x2 */
  if(spur_count > 0)
  {
    handle = (wwcoex_spur_info_idx & 0xFFF)<<4;
    handle = handle | (spur_count & 0xF);
    wwcoex_spur_info_idx = spur_idx;
    compare_out->action = ACTION_SPUR;
  }

  /* Store the generated handle in the return structure */
  compare_out->handle = handle;

  LMTSMGR_MSG_6(HIGH, "wwcoex_spur_compare_freqs: Agg band: %d Agg VCO: %d "
                      "Vic band: %d Vic VCO: %d, Entry found: %d, handle: %d",
                       a_info->band, compare_in->atech_vco,
                       v_info->band, compare_in->vtech_vco, found, handle);
  
  TTLMARKER(LMTSMGR_SPUR_COMP_OUT);
  
  return found;
  
#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(compare_in);
  WWCOEX_DUMMY_CHECK(compare_out);

  return FALSE;

#endif /* MCS_LIMITS_LOW_MEMORY */

}


/*=============================================================================

  FUNCTION:  wwan_coex_spur_list_init

=============================================================================*/
/*!
    @brief
    Initialize spur related entries
 
    @details
      Initialize spur related entries
      
    @return
     void

*/
/*===========================================================================*/
void wwan_coex_spur_list_init()
{

#ifndef MCS_LIMITS_LOW_MEMORY
  
  /* Initialize tech band mask */
  wwcoex_gen_spur_band_mask();

  /* Memset handles */
  memset((void *)wwcoex_spur_info,0,sizeof(wwcoex_spur_info));

  /* Copy the EFS contents to globals */
  wwcoex_init_spur_sim(0);

  return;

#endif /* MCS_LIMITS_LOW_MEMORY */
}


