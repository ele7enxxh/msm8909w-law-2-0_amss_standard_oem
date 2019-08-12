#ifndef __SDP_LOC_ENG_API_H__
#define __SDP_LOC_ENG_API_H__

/*============================================================================
  @file sdp_loc_eng_api.h

            API between SDP and LOC_ENG

GENERAL DESCRIPTION

  This file defines the API between other LocEng tasks (LocMW, Lbs, SM) and 
  the Sensor Data Processor module/task.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sdp/inc/sdp_loc_eng_api.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/13   vr      Support for injecting vehicle gyro and odometry data
10/15/12   mk      Initial version

=============================================================================*/

#define SDP_LOC_ENG_API_VERSION 1

/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "locEng_sm_common_msg_v01.h"

/* ---------------------------------------------------------------------------
 * Data type definitions
 * ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ---------------------------------------------------------------------------*/


/**
 * Called from SDP task; controls how pedometer data is
 * injected into the engine
 * 
 * @param [in] p_ind  the QMI indication 
 * 
 * @return 0 if operation is successful
 */
uint32 sdp_locEngPedometerControlInd(const locEngEventPedometerControlIndMsgT_v01 * const p_ind);


/** 
 *  Called from SDP task; controls how motion data is injected
 *  into the engine
 * 
 * @param [in] p_ind  the QMI indication 
 * 
 * @return 0 if operation is successful
 */
uint32 sdp_locEngMotionDataControlInd (const locEngEventMotionDataControlIndMsgT_v01 * const p_ind);

/**
 * called from LocMW or Lbs task to handle a QMI command to set navigation config.
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.
 *  
 * @param p_req  LocEng request including all client-specific data 
 *
 * @return eLOC_ENG_SUCCESS_V01         If the request has passed parameter 
 *                                      checking and an IPC was sent to
 *                                      SDP task for further processing
 *         eLOC_ENG_GENERAL_FAILURE_V01 If the request failed parameter checking
 *                                      and no IPC was sent to SDP
 *         eLOC_ENG_UNSUPPORTED_V01     If the API is unsupported
 */
locEngStatusEnumT_v01 sdp_locEngSetNavigationConfigReqProxy(locEngSetNavigationConfigReqMsgT_v01 * const pz_DataReq);

/**
 * called from LocMW or Lbs task to handle a QMI command to get navigation config.
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.
 *  
 * @param p_req  LocEng request including all client-specific data 
 *
 * @return eLOC_ENG_SUCCESS_V01         If the request has passed parameter 
 *                                      checking and an IPC was sent to
 *                                      SDP task for further processing
 *         eLOC_ENG_GENERAL_FAILURE_V01 If the request failed parameter checking
 *                                      and no IPC was sent to SDP
 *         eLOC_ENG_UNSUPPORTED_V01     If the API is unsupported
 */
locEngStatusEnumT_v01 sdp_locEngGetNavigationConfigReqProxy(locEngGetNavigationConfigReqMsgT_v01 * const pz_DataReq);

/**
 * called from LocMW or Lbs task to handle a QMI command to inject vehicle sensor data.
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.
 *  
 * @param p_req  LocEng request including all client-specific data 
 *
 * @return eLOC_ENG_SUCCESS_V01         If the request has passed parameter 
 *                                      checking and an IPC was sent to
 *                                      SDP task for further processing
 *         eLOC_ENG_GENERAL_FAILURE_V01 If the request failed parameter checking
 *                                      and no IPC was sent to SDP
 *         eLOC_ENG_UNSUPPORTED_V01     If the API is unsupported
 */
locEngStatusEnumT_v01 sdp_locEngInjectVehicleSensorDataReqProxy(locEngInjectVehicleSensorDataReqMsgT_v01 * const pz_DataReq);

#endif  /* __SDP_LOC_ENG_API_H__ */
