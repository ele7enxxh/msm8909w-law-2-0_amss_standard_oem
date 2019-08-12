/*============================================================================
 @file loc_geofence.h

 loc MW geofencing modules

 GENERAL DESCRIPTION

 This file defines the geofencing module in loc middleware.

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

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_geofence.h#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 12/12/11   nks     Initial version

 =============================================================================*/

#ifndef _LOC_GEOFENCE_H_
#define _LOC_GEOFENCE_H_
#include "aries_os_api.h"
#include "location_service_v02.h"
#include "loc_client.h"
/* ---------------------------------------------------------------------------
 * ..._req called by shim layer to process requests from QMI
 * ..._handler called by loc_task to process IPC from another task
 * ---------------------------------------------------------------------------*/


/** locGeofenceHandleNiInd
 *  Handle the NI geofence indication sent by GFC QMI module. This
 *  indication should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */
extern void locGeofenceHandleNiInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceHandleGenAlertInd
 *  Handle the geofence general alert indication sent by GFC QMI
 *  module. This indication should be sent as an event to the
 *  client.
 *  @param [in] pIpcMsg
 * */
extern void locGeofenceHandleGenAlertInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceHandleBreachInd
 *  Handle the breach indication sent by GFC QMI module. This
 *  indication should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */
extern void locGeofenceHandleBreachInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceHandleProximityInd
 *  Handle the Proximity indication sent by GFC QMI module. This
 *  indicatio should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */

extern void locGeofenceHandleProximityInd(const os_IpcMsgType* const pIpcMsg);

/** handleLocGeofenceAddInd
 *  Handle the add indication sent by GFC QMI module. This
 *  should be invoked when: a) loc_geofence module had sent an
 *  add geofence request previously OR b) A NI geofence was
 *  added.
 *  @param [in] pIpcMsg : pointer to the IPC message
 * */
extern void locGeofenceHandleAddInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceHandleDeleteInd
 *  Handle the delete indication sent by GFC QMI module. This
 *  should be invoked when loc_geofence module had sent an
 *  delete geofence request previously.
 *  @param [in] pIpcMsg
 * */
extern void locGeofenceHandleDeleteInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceHandleEditInd
 *  Handle the edit indication sent by GFC QMI module. This
 *  should be invoked when loc_geofence module had sent an edit
 *  geofence request previously.
 *  @param [in] pIpcMsg
 * */
extern void locGeofenceHandleEditInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceHandleQueryInd
 *  Handle the query indication sent by GFC QMI module. This
 *  should be invoked when loc_geofence module had sent a query
 *  geofence request previously.
 *  @param [in] pIpcMsg
 * */
extern void locGeofenceHandleQueryInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceHandleSetEngineConfigInd
 *  Handle the set engine configuration indication sent by GFC
 *  QMI module. This indication should be sent as an event to
 *  the client.
 *  @param [in] pIpcMsg
 * */
extern void locGeofenceHandleSetEngineConfigInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceInit
 * initialize loc MW goefence module and register the callbacks with the gfc_qmi
 * module.
 * @return : TRUE ; loc geofence module was initialized correctly
 *         : FALSE; loc geofence module could not be initialized.
 */
extern boolean locGeofenceInit(void);

/** locGeofenceAdd
 * process an add geofence request 
 * @param [in] locClientHandle : loc MW client handle 
 * @param [in] addGeofenceReq :  add geofence request 
 * @param [out] pResp        : QMI LOC response payload 
 * @return : TRUE -- GFC_QMI accepted the operation to add the 
 *           geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           add the geofence.
 */
extern boolean locGeofenceAdd( 
  int locClientHandle,  
  const qmiLocAddCircularGeofenceReqMsgT_v02* const pAddGeofenceReq,
  qmiLocGenRespMsgT_v02 * const pResp);

/** locGeofenceDelete
 * process a delete geofence request 
 * @param [in] locClientHandle : loc MW client handle 
 * @param [in] deleteGeofenceReq :  delete geofence request 
 * @param [out] pResp            : QMI LOC response payload 
 * @return : TRUE -- GFC_QMI accepted the operation to delete 
 *           the geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           delete the geofence.
 */
extern boolean locGeofenceDelete( 
  int locClientHandle,
  const qmiLocDeleteGeofenceReqMsgT_v02* const pDeleteGeofenceReq,
  qmiLocGenRespMsgT_v02 * const pResp);

/** locGeofenceEdit
 * process an edit geofence request 
 * @param [in] locClientHandle : loc MW client handle 
 * @param [in] pEditGeofenceReq :  Edit geofence request ptr 
 * @param [out] pResp          : QMI LOC response payload 
 * @return : TRUE -- GFC_QMI accepted the operation to edit 
 *           the geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           edit the geofence.
 */
extern boolean locGeofenceEdit(
   int locClientHandle,
   const qmiLocEditGeofenceReqMsgT_v02* const pEditGeofenceReq,
   qmiLocGenRespMsgT_v02 * const pResp);

/** locGeofenceQuery
 * process an edit geofence request 
 * @param [in] locClientHandle : loc MW client handle 
 * @param [in] pQueryGeofenceReq :  Query geofence request ptr 
 * @param [out] pResp            : QMI LOC response payload 
 * @return : TRUE -- GFC_QMI accepted the operation to query 
 *           the geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           query the geofence.
 */
extern boolean locGeofenceQuery(
   int locClientHandle,
   const qmiLocQueryGeofenceReqMsgT_v02* const pQueryGeofenceReq,
   qmiLocGenRespMsgT_v02 * const pResp);

/** locGeofencePurge
 * Purge all the geofences from the system. This 
 * function is called when the geofence client deregisters 
 * or the service deInitializes for any reasons 
 * The assumption is that only one geofence client is ever 
 * present in the system.
 *  
 */
extern void locGeofencePurge(void);


/** locGeofenceSetEngineConfig
 * process a set geofence engine config request. 
 * @param [in] locClientHandle : loc MW client handle 
 * @param [in] pQueryGeofenceReq :  Set geofence engine config 
 *        request ptr
 * @param [out] pResp            : QMI LOC response payload 
 * @return : TRUE -- GFC_QMI accepted the operation to set 
 *           the geofence engine configuration .\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           set the geofence engine configuration.
 */
extern boolean locGeofenceSetEngineConfig(
   int locClientHandle,
   const qmiLocSetGeofenceEngineConfigReqMsgT_v02* const pSetGeofenceEngineConfigReq,
   qmiLocGenRespMsgT_v02 * const pResp);

/** locGeofenceHandleBatchBreachInd
 *  Handle the breach indication sent by GFC QMI module. This
 *  indicatio should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleBatchBreachInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceHandleBatchBreachReq
 *  Handle the breach request sent by the control point. 
 *  
 *  @param [in] bEnable : Enable / Disable batch breach reports 
 * */

void locGeofenceHandleBatchBreachReq(boolean bEnable);

/** locGeofenceHandleBatchDwellInd
 *  Handle the breach indication sent by GFC QMI module. This
 *  indicatio should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleBatchDwellInd(const os_IpcMsgType* const pIpcMsg);

/** locGeofenceSetOperationMode
 *  Handle the operation mode request sent by control point
 *  
 *  @param [in] bEnable : Enable / Disable batch breach reports 
 * */
void locGeofenceSetOperationMode(const qmiLocOperationModeEnumT_v02    op_mode);

/** locGeofenceHandleGetTzReq
 *  Handle the TimeZone request from the geofence engine
 *  
 * */
void locGeofenceHandleGetTzReq(void);

/** locGeofenceSendTzIndResp
 *  Send Indication for ReqInject TimeZone info from the qmi control point
 *  
 * */
void locGeofenceSendTzIndResp(os_IpcMsgType* p_msg);

/** loc_geofence_inject_timezone_info
 *  Handle the TimeZone request from the geofence engine
 *  
 * */
boolean loc_geofence_inject_timezone_info
(
  qmiLocInjectTimeZoneInfoIndMsgT_v02 *pz_InjectTZInfoInd,
  const qmiLocInjectTimeZoneInfoReqMsgT_v02 *const cpz_Req,
  qmiLocGenRespMsgT_v02 *const pz_Resp
);

#endif // _LOC_GEOFENCE_H_
