/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GM_IOD_BASED_BACKOFF

GENERAL DESCRIPTION
  This module implmenets the Geofence module IOD Based Backoff algorithm

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/gm/src/gm_iod_based_backoff.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/15   sj      Initial creation of file.
======================================================================*/

#include "gm_gps_env_based_backoff.h"
#include "gm_motion_sensor.h"

/*
#define GM_EBEE_IOD_BACKOFF_TRIGGER_TIMER_ID_LAST    (GM_EBEE_BACKOFF_TRIGGER_TIMER_ID_FIRST + 20)
*/
#define GM_EBEE_IOD_BKOFF_UPDATE_WIFI_SET_TIMER  (GM_EBEE_IOD_BACKOFF_TRIGGER_TIMER_ID_FIRST)
#define GM_EBEE_IOD_BKOFF_DEFERRED_PROC_TIMER    (GM_EBEE_IOD_BACKOFF_TRIGGER_TIMER_ID_FIRST+1)
/* A total of 20 timers available */

/****************** Function Declarations ***************************************/

/*===========================================================================

FUNCTION gm_iod_based_backoff_is_needed

DESCRIPTION
  This function checks if the backoff is needed based on IOD algorithm

DEPENDENCIES

RETURN VALUE
  gm_backoff_predictor_result_enum_type: Predictor Result Enum type

SIDE EFFECTS

===========================================================================*/
gm_backoff_predictor_result_enum_type gm_iod_based_backoff_is_needed
(
  sm_gm_geofence_info_type *p_geofence
);


/*===========================================================================

FUNCTION gm_iod_based_backoff_calculate_backoff

DESCRIPTION
  This function calculates the backoff based on IOD sensor output

DEPENDENCIES

RETURN VALUE
  boolean: whether a backoff is calculated and filled in the param passed or not

SIDE EFFECTS

===========================================================================*/
boolean gm_iod_based_backoff_calculate_backoff
(
 sm_gm_geofence_info_type *p_geofence,
 uint32 *p_backoff
);

/*===========================================================================

FUNCTION gm_iod_based_backoff_predictor_init

DESCRIPTION
  This function initializes and enables the IOD algorithm

DEPENDENCIES

RETURN VALUE
  TRUE:

SIDE EFFECTS

===========================================================================*/
boolean gm_iod_based_backoff_predictor_init(void);


/*===========================================================================

FUNCTION gm_iod_based_backoff_handle_iod_indication

DESCRIPTION
  This function handles the IOD results from IOD algorithm

DEPENDENCIES

RETURN VALUE
  TRUE:

SIDE EFFECTS

===========================================================================*/
void gm_iod_based_backoff_handle_iod_indication(gm_iod_ind_info *p_als_iod_ind);

/*===========================================================================

FUNCTION gm_iod_based_backoff_exit_iod_backoff

DESCRIPTION
  This function handles the IOD results from IOD algorithm

DEPENDENCIES

RETURN VALUE
  TRUE:

SIDE EFFECTS

===========================================================================*/
void gm_iod_based_backoff_exit_iod_backoff(void);


