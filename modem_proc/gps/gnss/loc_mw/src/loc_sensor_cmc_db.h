/*============================================================================
 @file loc_sensor_cmc_db.h

 loc MW geofencing modules

 GENERAL DESCRIPTION

 This file defines the loc sensor client in loc middleware.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_sensor_cmc_db.h#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 3/31/14     ss     Supporting floats for speed
 11/29/12   ss     Initial version

 =============================================================================*/

#ifndef _LOC_SENSOR_CMC_DB_H_
#define _LOC_SENSOR_CMC_DB_H_

#include "comdef.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT
#include "sns_sam_distance_bound_v01.h"


typedef struct
{
  uint32 cmc_db_indication_type;
  
  union
  {
    sns_sam_distance_bound_enable_resp_msg_v01 cmc_db_start_resp_ind;
    sns_sam_distance_bound_disable_resp_msg_v01 cmc_db_stop_resp_ind;
    sns_sam_distance_bound_error_ind_msg_v01     cmc_db_err_ind;
    sns_sam_distance_bound_set_bound_resp_msg_v01  cmc_db_set_bound_resp_ind;
    sns_sam_distance_bound_get_report_resp_msg_v01  cmc_db_get_report_resp_ind;    
    sns_sam_distance_bound_report_ind_msg_v01 cmc_db_report_ind;
  }u;
} loc_geofence_cmc_db_ind;

typedef struct
{
  float cmc_db_unknwon_speed_bound;
  float cmc_db_stationary_speed_bound;
  float cmc_db_in_motion_speed_bound;
  float cmc_db_fiddle_speed_bound;
  float cmc_db_pedestrian_speed_bound;
  float cmc_db_vehicle_speed_bound;
  float cmc_db_walk_speed_bound;
  float cmc_db_run_speed_bound;
} loc_geofence_cmc_db_state_speed_bound;

/** loc_geofence_sensor_start_cmc_db
 *    Handles CMC Start request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_start_cmc_db(loc_geofence_cmc_db_state_speed_bound *p_speed_bounds);

/** loc_geofence_sensor_stop_cmc_db
 *    Handles CMC Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_cmc_db(void);

/** loc_geofence_sensor_cmc_db_set_bound
 *    Handles CMC Set Bound request from Geofence module. 
 *  @param [in] distance_to_check
 * */
boolean loc_geofence_sensor_cmc_db_set_bound(float distance_to_check);

/** loc_geofence_sensor_cmc_db_get_report
 *    Handles CMC Get Report request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_cmc_db_get_report(void);
#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
#endif // _LOC_SENSOR_AMD_H_
