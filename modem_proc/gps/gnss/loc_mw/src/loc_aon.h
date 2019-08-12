/*============================================================================
 @file loc_aon.h

 loc MW AlwaysOn service module

 GENERAL DESCRIPTION

 This file defines the AlwaysOn service module in loc middleware.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2014 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary
 Copyright (c) 2015 Qualcomm Technologies, Inc.
 Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_aon.h#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 04/06/15   sj      LB 2.0 integration
 09/22/14   sj      Initial version

 =============================================================================*/

#ifndef _LOC_AON_H_
#define _LOC_AON_H_
#include "aries_os_api.h"
#include "location_service_v02.h"

//Min itnerval b/w fixes that are batched
#define LOC_AON_BATCHING_MINIMUM_MININTERVAL (1000)

typedef enum 
{
  eLOC_AON_SUCCESS = 0,
  eLOC_AON_ERR_START_PENDING = 1,
  eLOC_AON_ERR_AON_REQ_NOT_FOUND = 2
}locAonStatusEnumT;

typedef enum 
{
  eLOC_AON_REQ_DBT,
  eLOC_AON_REQ_BATCH
}locAonReqTypeEnumT;

/** locAonForceStopAll
* Stop all batching requests from the specified client 
* @return : none.
*/
extern void locAonForceStopAll(  
  loc_client_handle_type locClientHandle);

/** locAonInit
 * initialize loc MW AlwaysOn service module and register the callbacks with GM
 * module.
 * @return : TRUE ; loc AlwaysOn module was initialized correctly
 *         : FALSE; loc AlwaysOn module could not be initialized.
 */
extern boolean locAonInit(void);

/** locAonBatchStartReq
* Start batching request, stops the existing request if the same client already
  has batching ongoing. Restarts batching with new params.
* @return None
*/

extern void locAonBatchStartReq(  
  loc_client_handle_type locClientHandle,
  const qmiLocStartBatchingReqMsgT_v02 * const pStartBatchingReq,
  qmiLocGenRespMsgT_v02 * const pResp);

/** locAonBatchStopReq
* Stop batching request.
* @return None
*/
extern void locAonBatchStopReq(  
  loc_client_handle_type locClientHandle,
  const qmiLocStopBatchingReqMsgT_v02 * const pStopBatchingReq,
  qmiLocGenRespMsgT_v02 * const pResp);

/** locAonDbtStartReq
* Starts DBT request.
* @return None
*/
extern boolean locAonDbtStartReq( 
  loc_client_handle_type locClientHandle,
  const qmiLocStartDbtReqMsgT_v02 * const pStartDbtReq,
  qmiLocGenRespMsgT_v02 * const pResp);

/** locAonDbtStopReq
* Stop DBT request.
* @return None
*/
extern boolean locAonDbtStopReq( 
  loc_client_handle_type locClientHandle,
  const qmiLocStopDbtReqMsgT_v02 * const pStopDbtReq,
  qmiLocGenRespMsgT_v02 * const pResp);

/** locAonIpcHandler
* Forwards the AON IPC's to aon code
* @return None
*/
extern boolean locAonIpcHandler(os_IpcMsgType* p_msg );

/** locAonTimerHandler
* Hnadles timer expirys set by AON 
* @return None 
*/
extern void locAonTimerHandler(uint32 timerId, uint32 timerParam);

/** locAonCheckIfLocClientPresent
* Determine if the given locClientHandle is present in the 
* request queue 
* @return : TRUE: Present
*           FALSE: Absent
*/
boolean locAonCheckIfLocClientPresent(
  loc_client_handle_type locClientHandle,
  locAonReqTypeEnumT reqType);

#endif // _LOC_AON_H_
