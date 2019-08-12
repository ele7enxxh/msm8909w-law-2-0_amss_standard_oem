/*============================================================================
 @file loc_sensor_ped.h

 loc MW geofencing modules

 GENERAL DESCRIPTION

 This file defines the loc sensor client in loc middleware.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_sensor_ped.h#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/29/12   ss     Initial version

 =============================================================================*/

#ifndef _LOC_SENSOR_PED_H_
#define _LOC_SENSOR_PED_H_

#include "comdef.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT
#include "sns_sam_ped_v01.h"

typedef struct
{
  uint32 ped_indication_type;
  
  union
  {
    sns_sam_ped_enable_resp_msg_v01 ped_start_resp_ind;
    sns_sam_ped_disable_resp_msg_v01 ped_stop_resp_ind;
    sns_sam_ped_error_ind_msg_v01     ped_err_ind;
    sns_sam_ped_report_ind_msg_v01 ped_report_ind;
  }u;
} loc_geofence_ped_ind;

/** loc_geofence_sensor_start_ped
 *    Handles PED Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_ped(uint32 report_period);

/** loc_geofence_sensor_stop_ped
 *    Handles PED Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_ped(void);
#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
#endif // _LOC_SENSOR_PED_H_
