/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GM WWAN Motion Detector

GENERAL DESCRIPTION
  This module is used for motion detection using WWAN

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/gm/src/gm_motion_sensor_wwan.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/14   ssu   Initial creation of file.
======================================================================*/

#ifndef GM_MOTION_SENSOR_WWAN_H
#define GM_MOTION_SENSOR_WWAN_H

#include "gm_motion_sensor.h"


#define GM_MOTION_SENSOR_WWAN_NUM_RSP_HISTORY_MAX 10

#define GM_MOTION_SENSOR_WWAN_CELL_MEAS_INFO_MAX 50

/* The classified speeds which are the outputs of the WWAN motion classifier */
typedef enum
{
  GM_MOTION_SENSOR_WWAN_MOTION_STATE_LOW_SPEED,
  GM_MOTION_SENSOR_WWAN_MOTION_STATE_HIGH_SPEED,
  GM_MOTION_SENSOR_WWAN_MOTION_STATE_UNDEFINED = GM_MOTION_SENSOR_WWAN_MOTION_STATE_HIGH_SPEED,
} gm_motion_sensor_wwan_motion_state;

/* The per cell information used by GM for motion classification */
typedef struct
{
  boolean  active_set;    /* TRUE if this cell belongs to the active set. */
  uint16   freq;          /* Frequency of this cell.                      */
  uint16   pri_scr_code;  /* Primary scrambling code of this cell.        */
  uint8    ec_no_val;    /* Ec/No of the cell. */
  uint8    rsp_val;      /* Received Signal Code Power (RSCP) of the cell.  */
  boolean  delta_rsp_included; /* TRUE if delta RSCP is included */
  int16    delta_rsp;  /* only valid when the above field is TRUE */
  uint8    rsrq_val;         /* Received signal code quality */
}gm_motion_sensor_wwan_per_cell_info_s_type;

/* The WWAN measurements for all the cells obtained by GM */
typedef struct
{
  uint8 num_cells;
  gm_motion_sensor_wwan_per_cell_info_s_type cell_info[GM_MOTION_SENSOR_WWAN_CELL_MEAS_INFO_MAX];
}gm_motion_sensor_wwan_cell_meas_update_s_type;


void gm_motion_sensor_wwan_motion_learn(gm_motion_sensor_wwan_motion_state e_motion_state);

/*===========================================================================
gm_motion_sensor_wwan_motion_detector_reset

Description:

   This function is used to reset the WWAN motion classification algorithm. 

Parameters:
  void

Return value:
  void
=============================================================================*/
void gm_motion_sensor_wwan_motion_detector_reset(void);

/*===========================================================================
gm_motion_sensor_wwan_handle_mrl_request_timer_expiry

Description:

   This function handles motion sensor WWAN MRL request timer expiry

Parameters:
  void

Return value:
  void
=============================================================================*/
void gm_motion_sensor_wwan_handle_meas_request_timer_expiry(void);

/*===========================================================================
gm_motion_sensor_wwan_handle_backoff_timer_expiry

Description:

   This function handles motion sensor WWAN Backoff timer expiry

Parameters:
  void

Return value:
  void
=============================================================================*/
void gm_motion_sensor_wwan_handle_backoff_timer_expiry(void);


/*===========================================================================
gm_motion_sensor_wwan_request_cell_meas

Description:

   This function is used to request WWAN cell measurements based on the RAT

Parameters:
  void

Return value:
  void
=============================================================================*/
void gm_motion_sensor_wwan_request_cell_meas(void);

/*===========================================================================
gm_motion_sensor_wwan_handle_first_geofence_add

Description:

  This function is used to by WWAN sensor to handle the first geofence added in the
   system.

Parameters:
  void

Return value:
  void
=============================================================================*/
void gm_motion_sensor_wwan_handle_first_geofence_add(void);

/*===========================================================================
gm_motion_sensor_wwan_handle_last_geofence_del

Description:

   This function is used to by WWAN sensor to handle the last geofence deleted in the
   system.

Parameters:
  void

Return value:
  void
=============================================================================*/
void gm_motion_sensor_wwan_handle_last_geofence_del(void);

/*===========================================================================
gm_motion_sensor_wwan_start

Description:

   This function handles motion detection start

Parameters:
  void

Return value:
  void
=============================================================================*/
boolean gm_motion_sensor_wwan_start(float distance_to_check);

/*===========================================================================
gm_motion_sensor_wwan_get_accum_distance

Description:

   This function retrieves distance accumlated

Parameters:
  void

Return value:
  void
=============================================================================*/
float gm_motion_sensor_wwan_get_accum_distance(void);

/*===========================================================================
gm_motion_sensor_wwan_handle_wcdma_cell_meas_update

Description:

   This function handles WCDMA Cell measurement update

Parameters:
  void

Return value:
  void
=============================================================================*/
void gm_motion_sensor_wwan_handle_wcdma_cell_meas_update(wl1_lsm_cell_meas_update_type *cell_meas_update_ptr);

/*===========================================================================
gm_motion_sensor_wwan_handle_lte_cell_meas_update

Description:

   This function handles LTE Cell measurement update

Parameters:
  void

Return value:
  void
=============================================================================*/
void gm_motion_sensor_wwan_handle_lte_cell_meas_update(lte_ml1_gps_mrl_cell_meas_update_t *cell_meas_update_ptr);

/*===========================================================================
gm_motion_sensor_wwan_init

Description:

   This function does wwan motion sensor initialization

Parameters:
  void

Return value:
  void
=============================================================================*/
boolean gm_motion_sensor_wwan_init(void);

#endif // GM_MOTION_SENSOR_WWAN_H
