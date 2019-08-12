/*============================================================================
 @file loc_sensor_amd.h

 loc MW geofencing modules

 GENERAL DESCRIPTION

 This file defines the loc sensor client in loc middleware.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2015 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_sensor_iod.h#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 =============================================================================*/

#ifndef _LOC_SENSOR_IOD_H_
#define _LOC_SENSOR_IOD_H_

#include "comdef.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT
#include "sns_sam_iod_v01.h"


typedef struct
{
  uint32 iod_indication_type;
  
  union
  {
    sns_sam_iod_enable_resp_msg_v01     iod_enable_resp;
    sns_sam_iod_disable_resp_msg_v01    iod_disable_resp;
    //iod_configure_resp_msg_v01  iod_config_resp;
    sns_sam_iod_update_resp_msg_v01     iod_update_resp;
    sns_sam_iod_get_report_resp_msg_v01 iod_report_resp;
    //iod_version_resp_msg_v01          iod_version_resp;
    sns_sam_iod_report_ind_msg_v01      iod_ind;

  }u;
} loc_geofence_iod_ind;


/** loc_geofence_start_iod
 *    Handles IOD Start request from Geofence module. 
 *  @param [in] sunset_ts: UTC timestamp for sunset in secs from epoch
 *  @param [in] sunrise_ts: UTC timestamp for sunrise in secs from epoch
 *  
 * */
boolean loc_geofence_sensor_start_iod
(
  uint32 utc_ts,
  uint32 sunrise_ts,
  uint32 sunset_ts
);

/** loc_geofence_stop_iod
 *    Handles IOD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_iod
(
  uint8 u_instance_id_valid,
  uint8 u_instance_id
);

/** loc_geofence_configure_iod
 *    Handles IOD Configure request from Geofence module. 
 *  @param [in] p_config_iod
 * */
boolean loc_geofence_configure_iod(sns_sam_iod_update_req_msg_v01 *p_config_iod);

/** loc_geofence_sensor_start_iod_autocal
 *    Handles IOD AutoCal Start request from Geofence module. We start the 
 *    AutoCal only once in a boot cycle. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_iod_autocal
(
  uint32  q_utc_time_secs,
  uint32  q_sunrisetime_sec,
  uint32  q_sunsettime_sec
);
#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
#endif // _LOC_SENSOR_IOD_H_

