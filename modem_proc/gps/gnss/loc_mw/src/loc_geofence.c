/*============================================================================
 @file loc_geofence.c

 loc MW geofencing module implementation

 GENERAL DESCRIPTION

 This file contains the loc middleware geofencing module implementation.

 Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary

 Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
 All Rights Reserved.
 QCA Proprietary and Confidential.
 
 Copyright (c) 2015 Qualcomm Techonologies, Inc.  
 Qualcomm Techonologies Confidential and Proprietary. All Rights Reserved. 
 
 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_geofence.c#1 $
 $DateTime: 2016/12/13 07:59:45 $
 $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
05/22/15    ssu   Resolve a bug in setting inside dwell type.
04/02/15    ssu   Batch Dwell notification for geofence.
01/26/15    ssu   Adding geofence custom responsiveness.
11/12/14   sj     Make changes for GF Config items. 
04/03/14   ssu      Additional Fix details parameters for Geofence Breach reports
 03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
 12/12/11   nks     Initial version

=============================================================================*/

#include "customer.h"
#include "comdef.h"

#include "msg.h"
#include "loc_geofence.h"
#include "gfc_qmi_api.h"
#include "location_service_v02.h"
#include "loc_api_internal.h"
#include "loc_qmi_shim.h"
#include "loc_pd.h"
#include "gnss_calendar_util.h"

/** the maximum number of outstanding requests per geofence.
 */
#define MAX_NUM_CONCURRENT_REQ (1)

/**
 * Structure tying a client to a particular transaction id
 * identifies the client to whom the response for the
 * transaction should be sent.
 */

typedef struct
{
  /** loc MW client who issued the request, if the handle
     is set to LOC_CLIENT_HANDLE_INVALID then this
     entry is availabe to use.
  */
  int locClientHandle;

  /** identifies if the txn ID is valid, a valid txn Id indicates
      that a request has been sent to gfc but the response has
     not been receievd.
   */
  boolean txnId_valid;

  /** txnId of the request, txnId is used for start and edit
   * geofence requests.
   * */
  uint32 txnId;

}locGeofenceClientReqStructT;



/**==================================GLOBAL VARIABLES========================================================*/

/** global array to store outstanding delete geofence requests */
static locGeofenceClientReqStructT g_deleteReqClientList[MAX_NUM_CONCURRENT_REQ];

/** global array to store outstanding add geofence requests */
static locGeofenceClientReqStructT g_addReqClientList[MAX_NUM_CONCURRENT_REQ];

/** global array to store outstanding edit geofence requests */
static locGeofenceClientReqStructT g_editReqClientList[MAX_NUM_CONCURRENT_REQ];

/** global array to store outstanding query geofence requests */
static locGeofenceClientReqStructT g_queryReqClientList[MAX_NUM_CONCURRENT_REQ];

/** global array to store outstanding geofence set engine
 *  config requests */
static locGeofenceClientReqStructT g_setEngineConfigReqClientList[MAX_NUM_CONCURRENT_REQ];

/** global array to store outstanding geofence get engine config
 *  requests */
static locGeofenceClientReqStructT g_getEngineConfigReqClientList[MAX_NUM_CONCURRENT_REQ];


/**==================================INTERNAL FUNCTION DEFINITIONS==============================================*/

/** locGeofenceResetState
 *  reset the state of all the geofence global variables */

static void locGeofenceResetState(void)
{
  uint32 i = 0;
  for( i=0; i<MAX_NUM_CONCURRENT_REQ;i++)
  {
    g_addReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_addReqClientList[i].txnId_valid = 0;
    g_deleteReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_deleteReqClientList[i].txnId_valid = 0;
    g_editReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_editReqClientList[i].txnId_valid = 0;
    g_queryReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_queryReqClientList[i].txnId_valid = 0;
    g_setEngineConfigReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_setEngineConfigReqClientList[i].txnId_valid = 0;
    g_getEngineConfigReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_getEngineConfigReqClientList[i].txnId_valid = 0;
  }
}

/** addTxnIdToClientReq
 *  checks if there is a free entry in the client req list and
 *  adds a txn Id to it.
 *  @param [in] pReqClient : pointer to the request client
 *         list where txn id has to be added
 *  @param [in] : locClientHandle : handle of the request sender
 *  @param [in] : transactionId   : txn id to be populated in
 *         the request list.
 *  @return TRUE -- The handle/txn Id were stored in the table.
 *          FALSE -- The handle/txn Id could not be stored in
 *          the table.
 */

static boolean addTxnIdToClientReq(
  locGeofenceClientReqStructT *pReqClient,
  int locClientHandle, uint32 transactionId)
{
  uint32 i = 0;

  if(NULL == pReqClient)
  {
    return FALSE;
  }

  /** check if there is a entry free in the request table */
  for( i=0; i<MAX_NUM_CONCURRENT_REQ ; i++)
  {
    if(LOC_CLIENT_HANDLE_INVALID == pReqClient[i].locClientHandle)
    {
      /** found an empty slot */
      pReqClient[i].locClientHandle = locClientHandle;
      /** store the transaction id */
      pReqClient[i].txnId_valid = TRUE;
      pReqClient[i].txnId = transactionId;
      break;
    }
  }

  /** if max requests exceed, return error */
  if(i >= MAX_NUM_CONCURRENT_REQ)
  {
    LOC_MSG_ERROR("addTxnIdToClientReq : max concurrent requests exceeded \n",
                    0,0,0);
    return FALSE;
  }

  return TRUE;
}

/** clearTxnIdFromClientReq
 *  Clears the entry corresponding to the txnd ID in the
 *  clientReqList
 *  @param [in] pReqClient : pointer to the request client list
 *         where an entry has to be cleared.
 *  @param [in] : locClientHandle : handle of the request sender
 *  @param [in] : transactionId   : txn id
 *  @return TRUE -- entry was cleared.
 *          FALSE -- entry could not be cleared.
 */

static boolean clearTxnIdFromClientReq(
  locGeofenceClientReqStructT *pReqClient,
  int locClientHandle, uint32 transactionId)
{
  uint32 i = 0;

  if(NULL == pReqClient)
  {
    return FALSE;
  }

  /** find the entry in the request table */
  for( i=0; i<MAX_NUM_CONCURRENT_REQ ; i++)
  {
    if(transactionId == pReqClient[i].txnId &&
       locClientHandle == pReqClient[i].locClientHandle )
    {
      /** found the entry, invalidate it  */
      pReqClient[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID ;
      pReqClient[i].txnId_valid = FALSE;
      break;
    }
  }

  /** if max requests exceed, return error */
  if(i >= MAX_NUM_CONCURRENT_REQ)
  {
    LOC_MSG_ERROR("clearTxnIdFromClientReq : Did not find the entry "
                  "handle = %d, txnID = %u \n", locClientHandle,transactionId,0);
    return FALSE;
  }

  return TRUE;
}

/** sendIndicationByTxnId
 *  Matches the transaction Id and sends an indication to
 *  the corresponding client
 *  @param [in/out] pReqClient -- pointer to client/request
 *         table. The table entry is cleared after the
 *         indication is sent.
 *  @param [in] transactionId
 *  @param[in]  pShimIndication -- pointer to the SHIM
 *        indication to send out
 *  @return -- TRUE : If the indication was sent to SHIM
 *          -- FALSE : If indication could not be sent   */

boolean sendIndicationByTxnId (
  locGeofenceClientReqStructT * const pClientReqInfo,
  uint32 transactionId,
  const locQmiShimIndInfoStructT * const pShimIndication )
{
  uint32 i = 0;
  /** sanity check the input */
  if(NULL == pClientReqInfo || NULL == pShimIndication)
  {
    LOC_MSG_ERROR("sendIndicationByTxnId: Null input params\n", 0,0,0);
    return FALSE;
  }
  /** traverse the client list and send edit indications to the
   *  clients who issued the corresponding request
   */
  for(i = 0; i < MAX_NUM_CONCURRENT_REQ; i++)
  {
    /** if txnId matches the indication txnInd send the
     *  indication back to to the corresponding client */
    if((LOC_CLIENT_HANDLE_INVALID != pClientReqInfo[i].locClientHandle) &&
       (pClientReqInfo[i].txnId_valid) &&
       (pClientReqInfo[i].txnId == transactionId ))
    {
      if( FALSE == locQmiShimSendInd(pClientReqInfo[i].locClientHandle,
                                     pShimIndication) )
      {
            LOC_MSG_ERROR("sendIndicationByTxnId: could not send Ind %u"
                         "for client %d\n", pShimIndication->q_Id,
                         pClientReqInfo[i].locClientHandle,0);
      }

      /** invalidate the entry */
      pClientReqInfo[i].txnId_valid = FALSE;
      pClientReqInfo[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;

      /** done; break out of the for loop */
      break;
    }
  }

  if(i >= MAX_NUM_CONCURRENT_REQ)
  {
    LOC_MSG_ERROR("sendIndicationByTxnId: did not find txn ID %u for client %d\n",
                 transactionId, pClientReqInfo[i].locClientHandle, 0);
    return FALSE;
  }
  return TRUE;
}


/** locGeofenceGenerateIPCtolocMW
 *  Generate an IPC to loc MW
 *  @param [in] msgId -- loc MW IPC message id
 *  @param [in] payloadSize -- payload size
 *  @param [in] pPayload --  pointer to the payload
 *  @return -- TRUE is IPC was sent successfully
 *          -- FALSE otherwise.
 **/
static boolean locGeofenceGenerateIPCtolocMW(
  loc_middleware_msg_id_e_type msgId,
  size_t payloadSize, const void * const pPayload )
{
  os_IpcMsgType * const pIpcMsg =
  os_IpcCreate(payloadSize, IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_LOC_MIDDLEWARE);
  if(NULL != pIpcMsg )
  {
    /** copy the payload*/
    (void)memscpy(pIpcMsg->p_Data, payloadSize, pPayload, payloadSize);

    pIpcMsg->q_MsgId = msgId;
    return(os_IpcSend(pIpcMsg, THREAD_ID_LOC_MIDDLEWARE) );
  }
  return FALSE;
}

/******************* CONVERSION FUNCTIONS BETWEEN QMI_LOC and GFC*************************************************/

/**  convertGfcResultToQmiLocStatus
 *   Convert a GFC QMI operation result to QMI_LOC status
 *   @param [in] returnCode -- from gfc_qmi
 *   @return status -- in qmi_loc format
 *    */
static qmiLocStatusEnumT_v02 convertGfcResultToQmiLocStatus(
  gfc_qmi_operation_result_e_type returnCode)
{
  qmiLocStatusEnumT_v02 status;
  switch(returnCode)
  {
    case GFC_QMI_ERROR_NONE:
      {
        status = eQMI_LOC_SUCCESS_V02;
        break;
      }
    case GFC_QMI_ERROR_INSUFFICIENT_MEMORY:
      {
        status = eQMI_LOC_INSUFFICIENT_MEMORY_V02;
        break;
      }
    case GFC_QMI_ERROR_GEOFENCE_NOT_FOUND:
    case GFC_QMI_ERROR_INVALID_PARAMETER:
    case GFC_QMI_ERROR_MANDATORY_PARAMETER_UNSPECIFIED:
      {
        status = eQMI_LOC_INVALID_PARAMETER_V02;
        break;
      }
    case GFC_QMI_ERROR_MAX_GEOFENCE_PROGRAMED:
      {
        status = eQMI_LOC_MAX_GEOFENCE_PROGRAMMED_V02;
        break;
      }
    case GFC_QMI_ERROR_COMMUNICATION_FAILURE:
    case  GFC_QMI_ERROR_GENERAL:
    default:
      {
        status = eQMI_LOC_GENERAL_FAILURE_V02;
        break;
      }
  }
  return status;
}

/** convertGfcStatusToQmiLocResp
 *  @param [in] status : GFC operation status
 *  @param [in] returnCode : GFC detailed return code
 *  @param [out] pResp:  QMI_LOC response type
 *  @return -- TRUE : If GFC status contained no error
 *          -- FALSE : If GFC status contained an error
 * */

static boolean convertGfcStatusToQmiLocResp(
  gfc_qmi_operation_status_e_type status,
  gfc_qmi_operation_result_e_type returnCode,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  boolean b_Retval;
  if(GFC_QMI_OPERATION_STATUS_FAILED == status )
  {
	pResp->resp.result = QMI_RESULT_FAILURE_V01;
	switch(returnCode)
	{
	  case GFC_QMI_ERROR_NONE:
	    {
	       pResp->resp.error = QMI_ERR_NONE_V01;
	       break;
	    }
	  case GFC_QMI_ERROR_INSUFFICIENT_MEMORY:
	      {
	    	pResp->resp.error = QMI_ERR_NO_MEMORY_V01;
	        break;
	      }
	    case GFC_QMI_ERROR_GEOFENCE_NOT_FOUND:
	    case GFC_QMI_ERROR_INVALID_PARAMETER:
	    case GFC_QMI_ERROR_MANDATORY_PARAMETER_UNSPECIFIED:
	      {
	    	pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
	        break;
	      }
	    case GFC_QMI_ERROR_COMMUNICATION_FAILURE:
	    case  GFC_QMI_ERROR_GENERAL:
	    default:
	      {
	    	pResp->resp.error = QMI_ERR_GENERAL_V01;
	        break;
	      }
	  }

   /** The GFC status contained an error   */
    b_Retval = FALSE;
  }
  else //PENDING OR SUCCESS
  {
	pResp->resp.error = 0;
    pResp->resp.result = QMI_RESULT_SUCCESS_V01;

    /** the GFC status contained no error */
    b_Retval = TRUE;
  }
  return b_Retval;
}

/** convertAddCircularGeofenceReqToGfc
 *  convert an add circular geofence request from QMI_LOC to GFC
 *  format
 *  @param [in] pAddGeofenceReq : QMI LOC add request
 *  @param [out]  pGfcAddGeofenceReq : GFC add geofence request
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertAddCircularGeofenceReqToGfc (
  const qmiLocAddCircularGeofenceReqMsgT_v02* const pAddGeofenceReq,
  gfc_qmi_add_geofence_req_s_type* const pGfcAddGeofenceReq)
{
  gfc_qmi_geofence_circular_target_area_req_s_type *pCirc_target_area = NULL;

  if(NULL == pAddGeofenceReq || NULL == pGfcAddGeofenceReq )
  {
    LOC_MSG_ERROR("convertAddCircularGeofenceReqToGfc: NULL param(s)\n",
                 0,0,0);
    return FALSE;
  }

  /** copy the mandatory fields */

  /** convert the breach mask*/
  if(pAddGeofenceReq->breachMask & QMI_LOC_GEOFENCE_BREACH_ENTERING_MASK_V02 )
  {
    pGfcAddGeofenceReq->breach_event_req_mask |=
    GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_ENTERING_MASK;
  }
  if(pAddGeofenceReq->breachMask & QMI_LOC_GEOFENCE_BREACH_LEAVING_MASK_V02 )
  {
    pGfcAddGeofenceReq->breach_event_req_mask |=
    GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_LEAVING_MASK;
  }

  /** convert the target area */
  pGfcAddGeofenceReq->target_area.target_shape_type =
  GFC_QMI_GEOFENCE_TARGET_SHAPE_CIRCULAR;

  pCirc_target_area =
  &(pGfcAddGeofenceReq->target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area);

  /** copy the circular geofence area parameters */
  pCirc_target_area->latitude_d = pAddGeofenceReq->circularGeofenceArgs.latitude;
  pCirc_target_area->longitude_d = pAddGeofenceReq->circularGeofenceArgs.longitude;
  pCirc_target_area->radius_m = pAddGeofenceReq->circularGeofenceArgs.radius;

  /** copy the transaction Id */
  pGfcAddGeofenceReq->transaction_id = pAddGeofenceReq->transactionId;

  if( TRUE == pAddGeofenceReq->includePosition)
  {
    pGfcAddGeofenceReq->opt_feature_req_mask |=
    GFC_QMI_GEOFENCE_OPT_FEATURE_REQ_POS_REQ_ON_BREACH_MASK;
  }

  /** copy the optional fields */

  /** copy confidence */
  if(pAddGeofenceReq->confidence_valid)
  {
    pGfcAddGeofenceReq->valid_opt_param_req_mask |=
    GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_CONFIDENCE_MASK;
    switch(pAddGeofenceReq->confidence)
    {
      case eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02:
        pGfcAddGeofenceReq->confidence = GFC_QMI_CONFIDENCE_LOW;
        break;
      case eQMI_LOC_GEOFENCE_CONFIDENCE_MED_V02:
        pGfcAddGeofenceReq->confidence = GFC_QMI_CONFIDENCE_MEDIUM;
        break;
      case eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH_V02:
        pGfcAddGeofenceReq->confidence = GFC_QMI_CONFIDENCE_HIGH;
        break;
      default:
        //error return with an error
        return FALSE;
    }
  }

  /** Copy the custom responsiveness value if present. If the
   *  custom responsiveness value is set then consider only
   *  this value and disregard the other responsiveness setting */
  if(pAddGeofenceReq->customResponsivenessValue_valid)
  {
    /** Old HLOS/AP will send only responsiveness as High, Medium
        and Low. While new HLOS/AP will send both responsiveness
        set to High, Medium or Low and also set the value in custom
        responsiveness. This is for backward compatibility to take
        care of older modems which wont have custom responsiveness
        support*/
    pGfcAddGeofenceReq->valid_opt_param_req_mask |= 
    GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_RESPONSIVENESS_MASK;
    pGfcAddGeofenceReq->q_custom_responsiveness_value = pAddGeofenceReq->customResponsivenessValue;
    pGfcAddGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_CUSTOM;
  }
  else if(pAddGeofenceReq->responsiveness_valid)
  {
    /** Consider the other responsiveness setting   */
    pGfcAddGeofenceReq->valid_opt_param_req_mask |= 
    GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_RESPONSIVENESS_MASK;
    switch(pAddGeofenceReq->responsiveness)
    {
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_LOW_V02:
        pGfcAddGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_LOW;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_MED_V02:
        pGfcAddGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_MEDIUM;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_HIGH_V02:
        pGfcAddGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_HIGH;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_ULTRA_HIGH_V02:
        pGfcAddGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_ULTRA_HIGH;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_CUSTOM_V02:
        pGfcAddGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_CUSTOM;
        LOC_MSG_ERROR("Custom resp value not specified\n",
                 0,0,0);
        return FALSE;
      default:
        //error return with an error
        return FALSE;
    }
  }

  /* Copy the dwell type in the add request. If the dwell type is valid only then the dwell indication
     is sent . If dwell type is valid and dwell time is not set , then it is an error. */
  if(pAddGeofenceReq->dwellTypeMask_valid) 
  {
    /* Only INSIDE dwell is enabled */
    if(pAddGeofenceReq->dwellTypeMask&QMI_LOC_GEOFENCE_DWELL_TYPE_INSIDE_MASK_V02) 
    {
      pGfcAddGeofenceReq->q_dwell_type_mask |= GFC_QMI_GEOFENCE_DWELL_TYPE_INSIDE_MASK;
    }
    /* Only OUTSIDE dwell is enabled */
    if(pAddGeofenceReq->dwellTypeMask&QMI_LOC_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK_V02) 
    {
      pGfcAddGeofenceReq->q_dwell_type_mask |= GFC_QMI_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK;
    }
    /* Both INSIDE and OUTSIDE dwell are enabled */
    if((pAddGeofenceReq->dwellTypeMask&QMI_LOC_GEOFENCE_DWELL_TYPE_INSIDE_MASK_V02)&&
       (pAddGeofenceReq->dwellTypeMask&QMI_LOC_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK_V02))
    {
      pGfcAddGeofenceReq->q_dwell_type_mask |= GFC_QMI_GEOFENCE_DWELL_TYPE_INSIDE_MASK|GFC_QMI_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK;
    }    
  }
  else
  {
    pGfcAddGeofenceReq->q_dwell_type_mask |= GFC_QMI_GEOFENCE_DWELL_TYPE_NONE_MASK;
  }

  /* Copy the dwell time in secs. If the dwell time is not specified then its an error.
     Depending on the dwell type mask the notifications are sent. */
  if(pAddGeofenceReq->dwellTime_valid)
  {
    pGfcAddGeofenceReq->q_dwell_time_s = pAddGeofenceReq->dwellTime;
  }
  else
  {
    if(pAddGeofenceReq->dwellTypeMask_valid)
    {
      /* Return with an error */
      LOC_MSG_ERROR("convertAddCircularGeofenceReqToGfc: No dwell time specified.  (s)\n",
                   0,0,0);
      return FALSE;
    }
  }

  /** duration is INFINITY and start time is NOW */
  pGfcAddGeofenceReq->duration_s = 0xFFFFFFFF;
  pGfcAddGeofenceReq->start_time_s = 0;
  pGfcAddGeofenceReq->valid_opt_param_req_mask |=
  (GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_START_TIME_MASK |
   GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_DURATION_MASK);

  return TRUE;
}

/** convertGfcQueryRespToLocInd
 *  convert a Query response from GFC to QMI LOC format
 *  @param [in] pGfcQueryInd : GFC query response
 *  @param [out]pQueryInd : QMI LOC query indication
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertGfcQueryRespToLocInd(
  const gfc_qmi_query_geofence_rsp_s_type* const pGfcQueryInd,
  qmiLocQueryGeofenceIndMsgT_v02* const pQueryInd)
{
  if(NULL == pQueryInd || NULL == pGfcQueryInd )
  {
    LOC_MSG_ERROR("convertGfcQueryRespToLocInd: NULL param(s)\n",
                 0,0,0);
    return FALSE;
  }

  /** Geofence ID and Txn Id should always be valid */
  pQueryInd->geofenceId_valid = 1;
  pQueryInd->geofenceId = pGfcQueryInd->geofence_id;

  pQueryInd->transactionId_valid = 1;
  pQueryInd->transactionId = pGfcQueryInd->transaction_id;

  pQueryInd->status =
        convertGfcResultToQmiLocStatus(pGfcQueryInd->return_code);
   /** copy the fields only if status was successful */
   if(eQMI_LOC_SUCCESS_V02 == pQueryInd->status)
   {
     /** convert circular geofence args */
     if(GFC_QMI_GEOFENCE_TARGET_SHAPE_CIRCULAR ==
        pGfcQueryInd->target_area.target_shape_type)
     {
       pQueryInd->circularGeofenceArgs_valid = 1;

       pQueryInd->circularGeofenceArgs.latitude =
         pGfcQueryInd->target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.latitude_d;

       pQueryInd->circularGeofenceArgs.longitude =
         pGfcQueryInd->target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.longitude_d;

       pQueryInd->circularGeofenceArgs.radius =
         pGfcQueryInd->target_area.gfc_qmi_geofence_target_area_req_u.circ_target_area.radius_m;
     }

     /** convert geofence origin */
     switch(pGfcQueryInd->geofence_origin)
     {
       case GFC_QMI_GEOFENCE_ORIGIN_NI:
         {
           pQueryInd->geofenceOrigin =
             eQMI_LOC_GEOFENCE_ORIGIN_NETWORK_V02;
           break;
         }
       case GFC_QMI_GEOFENCE_ORIGIN_MO:
         {
           pQueryInd->geofenceOrigin =
             eQMI_LOC_GEOFENCE_ORIGIN_DEVICE_V02;
           break;
         }
       default:
         {
           LOC_MSG_ERROR("convertGfcQueryRespToLocInd: invalid geofence origin\n",
                        0,0,0);
           return FALSE;
         }
     }
     pQueryInd->geofenceOrigin_valid = 1;

     /** convert position w.r.t geofence   */
     pQueryInd->posWrtGeofence_valid = 1;
     switch(pGfcQueryInd->pos_status)
     {
       case GFC_QMI_POS_INSIDE :
         {
           pQueryInd->posWrtGeofence = eQMI_LOC_GEOFENCE_POSITION_INSIDE_V02;
           break;
         }
       case GFC_QMI_POS_OUTSIDE:
         {
           pQueryInd->posWrtGeofence = eQMI_LOC_GEOFENCE_POSITION_OUTSIDE_V02;
           break;
         }
       case GFC_QMI_POS_UNKNOWN :
         {
           LOC_MSG_HIGH("convertGfcQueryRespToLocInd: unknown pos w.r.t geofence\n",
                        0,0,0);
           pQueryInd->posWrtGeofence_valid = 0;
           break;
         }

       default:
         {
           LOC_MSG_ERROR("convertGfcQueryRespToLocInd: invalid pos w.r.t geofence\n",
                        0,0,0);
           return FALSE;
         }
     }

     /** TBD : convert geofence state */
   }

   return TRUE;
}

/** convertEditGeofenceReqToGfc
 *  convert an edit geofence request from QMI_LOC to GFC format
 *  @param [in] pEditGeofenceReq : QMI LOC Edit request ptr
 *  @param [in] pGfcEditGeofenceReq : GFC edit geofence request
 *         ptr
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertEditGeofenceReqToGfc (
  const qmiLocEditGeofenceReqMsgT_v02* const pEditGeofenceReq,
  gfc_qmi_edit_geofence_req_s_type* const pGfcEditGeofenceReq)
{

  if(NULL == pEditGeofenceReq || NULL == pGfcEditGeofenceReq )
  {
    LOC_MSG_ERROR("convertEditGeofenceReqToGfc: NULL param(s)\n",
                 0,0,0);
    return FALSE;
  }

  /** convert mandatory parameters */

  pGfcEditGeofenceReq->geofence_id = pEditGeofenceReq->geofenceId;

  pGfcEditGeofenceReq->transaction_id = pEditGeofenceReq->transactionId;

  if(pEditGeofenceReq->geofenceState_valid)
  {
    /** Duration is always set to infinity */
    pGfcEditGeofenceReq->duration_s = 0xFFFFFFFF;
    switch(pEditGeofenceReq->geofenceState)
    {
      case eQMI_LOC_GEOFENCE_STATE_ACTIVE_V02:
        {
          pGfcEditGeofenceReq->start_time_s = 0;
          break;
        }
      case eQMI_LOC_GEOFENCE_STATE_SUSPEND_V02:
        {
          pGfcEditGeofenceReq->start_time_s = 0xFFFFFFFF;
          break;
        }
      default :
        {
          return FALSE;
        }
    }
    pGfcEditGeofenceReq->edit_param_mask |=
    (GFC_QMI_EDIT_GEOFENCE_PARAM_START_TIME_MASK |
     GFC_QMI_EDIT_GEOFENCE_PARAM_DURATION_MASK);
  }

  if(pEditGeofenceReq->breachMask_valid)
  {
    if(pEditGeofenceReq->breachMask & QMI_LOC_GEOFENCE_BREACH_ENTERING_MASK_V02 )
    {
      pGfcEditGeofenceReq->breach_event_req_mask |=
      GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_ENTERING_MASK;
    }
    if(pEditGeofenceReq->breachMask & QMI_LOC_GEOFENCE_BREACH_LEAVING_MASK_V02 )
    {
      pGfcEditGeofenceReq->breach_event_req_mask |=
      GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_LEAVING_MASK;
    }
    pGfcEditGeofenceReq->edit_param_mask |=
    GFC_QMI_EDIT_GEOFENCE_PARAM_BREACH_EVENT_TYPE_MASK;
  }
  if(pEditGeofenceReq->responsiveness_valid)
  {
    switch(pEditGeofenceReq->responsiveness)
    {
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_LOW_V02:
        pGfcEditGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_LOW;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_MED_V02:
        pGfcEditGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_MEDIUM;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_HIGH_V02:
        pGfcEditGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_HIGH;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_ULTRA_HIGH_V02:
        pGfcEditGeofenceReq->responsiveness = GFC_QMI_RESPONSIVENESS_ULTRA_HIGH;
        break;
      default:
        {
          return FALSE;
        }
      }
    pGfcEditGeofenceReq->edit_param_mask |= GFC_QMI_EDIT_GEOFENCE_PARAM_RESPONSIVENESS_MASK;
  }

  return TRUE;
}

/** convertMotionStateInfoConfigReqToGfc
 *  convert set geofence engine config request structure for motion state Info
 *  from QMI_LOC to GFC format
 *  @param [in] pEditGeofenceReq : QMI LOC Edit request ptr
 *  @param [in] pGfcEditGeofenceReq : GFC edit geofence request
 *         ptr
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/
static boolean convertMotionStateInfoConfigReqToGfc(
  const qmiLocSetGeofenceEngineConfigReqMsgT_v02* const pSetGeofenceEngineConfigReq,
  gfc_qmi_set_geofence_engine_config_req_s_type* const pGfcSetGeofenceEngineConfigReq)
{
  uint32  q_index  = 0;

  gm_motion_state_speed_config               *p_gm_ms_speed_config = NULL;

  if((pSetGeofenceEngineConfigReq == NULL) || (pGfcSetGeofenceEngineConfigReq == NULL))
  {
    LOC_MSG_ERROR("convertSetGeofenceEngineConfigReqToGfc: NULL param(s)\n",
                 0,0,0);
    return FALSE;
  }

  p_gm_ms_speed_config = &pGfcSetGeofenceEngineConfigReq->q_gm_ms_speed_config;

  for( q_index = 0;
       ((q_index < pSetGeofenceEngineConfigReq->motionStateInfo_len) &&
        (q_index < QMI_LOC_GEOFENCE_MAX_MOTION_STATES_V02));
       q_index++ )
  {
    const qmiLocGeofenceMotionStateConfigStructT_v02 * const p_motionStateInfoQmi =
                             &pSetGeofenceEngineConfigReq->motionStateInfo[q_index];

    if(eQMI_LOC_GEOFENCE_MOTION_STATE_WALK_V02 == p_motionStateInfoQmi->motionState)
    {
      p_gm_ms_speed_config->gm_ms_walk_speed = p_motionStateInfoQmi->motionStateSpeed;
      p_gm_ms_speed_config->b_walking_valid = TRUE;
    }
    else if(eQMI_LOC_GEOFENCE_MOTION_STATE_RUN_V02 == p_motionStateInfoQmi->motionState)
    {
      p_gm_ms_speed_config->gm_ms_run_speed = p_motionStateInfoQmi->motionStateSpeed;
      p_gm_ms_speed_config->b_running_valid = TRUE;
    }
    else
    {
      /* Configuration of anything other than Walk & Run not supported */
      MSG_ERROR("Unsupported Motion State value %d", p_motionStateInfoQmi->motionState, 0, 0);
    }
  }

  return TRUE;
}

/** convertSetGeofenceEngineConfigReqToGfc
 *  convert set geofence engine config request from QMI_LOC to
 *  GFC format
 *  @param [in] pEditGeofenceReq : QMI LOC Edit request ptr
 *  @param [in] pGfcEditGeofenceReq : GFC edit geofence request
 *         ptr
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertSetGeofenceEngineConfigReqToGfc (
  const qmiLocSetGeofenceEngineConfigReqMsgT_v02* const pSetGeofenceEngineConfigReq,
  gfc_qmi_set_geofence_engine_config_req_s_type* const pGfcSetGeofenceEngineConfigReq)
{
  boolean b_retval = FALSE;

  if((NULL == pSetGeofenceEngineConfigReq) ||
     (NULL == pGfcSetGeofenceEngineConfigReq ))
  {
    LOC_MSG_ERROR("convertSetGeofenceEngineConfigReqToGfc: NULL param(s)\n",
                 0,0,0);
    return FALSE;
  }
  /** convert mandatory parameters */
  pGfcSetGeofenceEngineConfigReq->transaction_id = pSetGeofenceEngineConfigReq->transactionId;

  /* Convert optional parameters */
  if(pSetGeofenceEngineConfigReq->maxGeofences_valid)
  {
    b_retval = TRUE;
    pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_MAX_GF_MASK;
    pGfcSetGeofenceEngineConfigReq->q_max_gf = pSetGeofenceEngineConfigReq->maxGeofences;

  }
  else if(pSetGeofenceEngineConfigReq->gnssUnavailableIndicationTimeout_valid)
  {
    b_retval = TRUE;
    pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_GNSS_UNAVAIL_TIMEOUT_MASK;
    pGfcSetGeofenceEngineConfigReq->gnss_unavailable_indication_timeout = pSetGeofenceEngineConfigReq->gnssUnavailableIndicationTimeout;
  }
  else if(pSetGeofenceEngineConfigReq->enableCpiUsage_valid)
  {
    b_retval = TRUE;
    pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_ENABLE_CPI_USAGE_MASK;
    pGfcSetGeofenceEngineConfigReq->q_gm_enable_position_injection_usage = pSetGeofenceEngineConfigReq->enableCpiUsage;
  }
  else if(pSetGeofenceEngineConfigReq->enableMotionDetectionSources_valid)
  {
    /* Start by setting all the sources to disabled , then enable based on the setting */
    pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= (GFC_QMI_GM_ENGINE_CONF_ENABLE_SENSORS_FOR_MOTION_DETECTION_MASK |
                                                                      GFC_QMI_GM_ENGINE_CONF_ENABLE_WIFI_FOR_MOTION_DETECTION_MASK |
                                                                      GFC_QMI_GM_ENGINE_CONF_ENABLE_WWAN_FOR_MOTION_DETECTION_MASK);
    pGfcSetGeofenceEngineConfigReq->q_gm_enable_sensor_motion_detection = FALSE;
    pGfcSetGeofenceEngineConfigReq->q_gm_enable_wifi_motion_detection = FALSE;
    pGfcSetGeofenceEngineConfigReq->q_gm_enable_wwan_motion_detection = FALSE;

    if(pSetGeofenceEngineConfigReq->enableMotionDetectionSources&QMI_LOC_MOTION_DETECTION_SOURCE_SENSORS_V02)
    {
      pGfcSetGeofenceEngineConfigReq->q_gm_enable_sensor_motion_detection = TRUE;
    }
    if(pSetGeofenceEngineConfigReq->enableMotionDetectionSources&QMI_LOC_MOTION_DETECTION_SOURCE_WIFI_V02)
    {
      pGfcSetGeofenceEngineConfigReq->q_gm_enable_wifi_motion_detection = TRUE;
    }
    if(pSetGeofenceEngineConfigReq->enableMotionDetectionSources&QMI_LOC_MOTION_DETECTION_SOURCE_WWAN_V02)
    {
      pGfcSetGeofenceEngineConfigReq->q_gm_enable_wwan_motion_detection = TRUE;
    }
    b_retval = TRUE;
  }
  else if(pSetGeofenceEngineConfigReq->gnssPositionSessionTimeout_valid)
  {
    pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_GNSS_POS_QOS_SESS_TIMEOUT_MASK;
    pGfcSetGeofenceEngineConfigReq->gnss_pos_qos_sess_timeout = pSetGeofenceEngineConfigReq->gnssPositionSessionTimeout;
    b_retval = TRUE;
  }
  else if(pSetGeofenceEngineConfigReq->gnssPositionMaxPuncAcceptable_valid)
  {
    pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_GNSS_MAX_POS_UNC_ACCEPTED_MASK;
    pGfcSetGeofenceEngineConfigReq->q_gm_gnss_max_pos_unc_accepted =  pSetGeofenceEngineConfigReq->gnssPositionMaxPuncAcceptable;
    b_retval = TRUE;
  }
  else if(pSetGeofenceEngineConfigReq->mediumResponsivenessValue_valid)
  {
    pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_MED_RESP_BACKOFF_MASK;
    pGfcSetGeofenceEngineConfigReq->q_gm_med_resp_backoff =  pSetGeofenceEngineConfigReq->mediumResponsivenessValue;
    b_retval = TRUE;
  }
  else if(pSetGeofenceEngineConfigReq->chalGnssEnvMinCpiWaitInterval_valid)
  {
    pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_CPI_REQUEST_RATE_CONFIG_MASK;
    pGfcSetGeofenceEngineConfigReq->q_gm_cpi_request_rate =  pSetGeofenceEngineConfigReq->chalGnssEnvMinCpiWaitInterval;
    b_retval = TRUE;
  }
  else if(pSetGeofenceEngineConfigReq->motionStateInfo_valid)
  {
    if (convertMotionStateInfoConfigReqToGfc(pSetGeofenceEngineConfigReq,
                                             pGfcSetGeofenceEngineConfigReq))
    {
      pGfcSetGeofenceEngineConfigReq->t_gfc_qmi_gm_engine_conf_mask |= GFC_QMI_GM_ENGINE_CONF_MS_SPEED_MASK;
      b_retval = TRUE;
    }
  }

  return b_retval;
}


/** convertGfcSetEngineConfigRespToLocInd
 *  convert a set engine config response from GFC to QMI LOC
 *  format
 *  @param [in] pGfcQueryInd : GFC query response
 *  @param [out]pQueryInd : QMI LOC query indication
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertGfcSetEngineConfigRespToLocInd(
  const gfc_qmi_set_geofence_engine_config_rsp_s_type* const pGfcSetEngineConfigInd,
  qmiLocSetGeofenceEngineConfigIndMsgT_v02* const pSetEngineConfigInd)
{
  if(NULL == pSetEngineConfigInd || NULL == pGfcSetEngineConfigInd )
  {
    LOC_MSG_ERROR("convertGfcSetEngineConfigRespToLocInd: NULL param(s)\n",
                 0,0,0);
    return FALSE;
  }
  pSetEngineConfigInd->transactionId_valid = 1;
  pSetEngineConfigInd->transactionId = pGfcSetEngineConfigInd->transaction_id;

  pSetEngineConfigInd->status =
        convertGfcResultToQmiLocStatus(pGfcSetEngineConfigInd->return_code);
  return TRUE;
}

/** convertBreachRespToLocInd
 *  convert an breach event from GFC to QMI  LOC format
 *  @param [in] pGfcBreachInd : GFC breach indication
 *  @param [out]  pBreach : QMI LOC breach indication
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertBreachRespToLocInd(
  const gfc_qmi_breach_geofence_rsp_s_type* const pGfcBreachInd,
  qmiLocEventGeofenceBreachIndMsgT_v02 * const pBreachInd)
{
  if( NULL == pGfcBreachInd || NULL == pBreachInd)
  {
    LOC_MSG_ERROR("convertBreachRespToQmiInd: NULL parameters\n", 0,0,0);
    return FALSE;
  }

  if(pGfcBreachInd->origin != GFC_QMI_GEOFENCE_ORIGIN_MO)
  {
    LOC_MSG_ERROR("convertBreachRespToQmiInd: unknown origin\n", 0,0,0);
    return FALSE;
  }
  switch(pGfcBreachInd->breach_type)
  {
    case GFC_QMI_BREACH_GEOFENCE_TYPE_ENTERING:
      pBreachInd->breachType = eQMI_LOC_GEOFENCE_BREACH_TYPE_ENTERING_V02;
      break;

    case GFC_QMI_BREACH_GEOFENCE_TYPE_LEAVING:
      pBreachInd->breachType = eQMI_LOC_GEOFENCE_BREACH_TYPE_LEAVING_V02;
      break;

    default:
      LOC_MSG_ERROR("convertBreachRespToQmiInd: Unknown breach type\n", 0,0,0);
      return FALSE;
      break;
  }

  if(pGfcBreachInd->confidence_rep_mask & GFC_QMI_CONFIDENCE_REPORT_ON_BREACH_TRUE)
  {
    pBreachInd->breachConfidence_valid = 1;
    switch(pGfcBreachInd->confidence)
    {
      case GFC_QMI_CONFIDENCE_LOW:
        pBreachInd->breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02;
        break;
      case GFC_QMI_CONFIDENCE_MEDIUM:
        pBreachInd->breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_MED_V02;
        break;
      case GFC_QMI_CONFIDENCE_HIGH:
        pBreachInd->breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH_V02;
        break;
      default:
       LOC_MSG_ERROR("convertBreachRespToLocInd: Unknown breach confidence type\n", 0,0,0);
       return FALSE;
    }
  }

  pBreachInd->geofenceId = pGfcBreachInd->geofence_id;
  if(pGfcBreachInd->pos_rep_mask & GFC_QMI_POS_REPORT_ON_BREACH_TRUE)
  {
    uint64_t timeInSec;
    pBreachInd->geofencePosition_valid = 1;
    //TBD: convert position
    pBreachInd->geofencePosition.latitude = pGfcBreachInd->pos_fix.latitude;
    pBreachInd->geofencePosition.longitude = pGfcBreachInd->pos_fix.longitude;

    pBreachInd->geofencePosition.horUncEllipseSemiMajor =
      pGfcBreachInd->pos_fix.loc_uncrtnty_a;

    pBreachInd->geofencePosition.horUncEllipseSemiMinor =
      pGfcBreachInd->pos_fix.loc_uncrtnty_p;

    pBreachInd->geofencePosition.horUncEllipseOrientAzimuth =
      pGfcBreachInd->pos_fix.loc_uncrtnty_ang;

    gnss_GetUnixEpochFromUTC(&(pGfcBreachInd->pos_fix.utc_time),
                             &timeInSec);
    pBreachInd->geofencePosition.timestampUtc = (timeInSec*1000);

    if(pGfcBreachInd->pos_fix.height_incl)
    {
       pBreachInd->geofencePosition.altitudeWrtEllipsoid_valid = 1;
        pBreachInd->geofencePosition.altitudeWrtEllipsoid =
          pGfcBreachInd->pos_fix.height;
    }
    if(pGfcBreachInd->pos_fix.height_uncrtnty_incl)
    {
      pBreachInd->geofencePosition.vertUnc_valid = 1;
      pBreachInd->geofencePosition.vertUnc =
        pGfcBreachInd->pos_fix.loc_uncrtnty_v;
    }
    if(pGfcBreachInd->pos_fix.velocity_incl)
    {
      pBreachInd->geofencePosition.speedHorizontal_valid = 1;
      pBreachInd->geofencePosition.speedHorizontal =
        pGfcBreachInd->pos_fix.velocity_hor;
      //TBD: No GFC mask for vertical velocity so assuming it is
      //valid when velocity_incl is set
      pBreachInd->geofencePosition.speedVertical_valid = 1;
      pBreachInd->geofencePosition.speedVertical =
        pGfcBreachInd->pos_fix.velocity_ver;
    }
    if(pGfcBreachInd->pos_fix.heading_valid)
    {
      pBreachInd->geofencePosition.heading_valid = 1;
      pBreachInd->geofencePosition.heading = pGfcBreachInd->pos_fix.heading;
    }

  }

  return TRUE;
}

/** convGfcLocationInfoToLoc
 *  Convert the location information from GFC to QMI LOC format
 *  @param [in] pGfcPosInfo : GFC location info
 *  @param [out]  pLocPosInfo : QMI LOC position info
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convGfcLocationInfoToLoc(
  const qfc_qmi_pos_data_rsp_s_type* const pGfcPosInfo,
  qmiLocGeofencePositionStructT_v02 * const pLocPosInfo)
{
  uint64_t timeInSec = 0;

  if((NULL == pGfcPosInfo)||
     (NULL == pLocPosInfo))
  {
    return FALSE;
  }
  pLocPosInfo->latitude = pGfcPosInfo->latitude;
  pLocPosInfo->longitude = pGfcPosInfo->longitude;

  pLocPosInfo->horUncEllipseSemiMajor =
    pGfcPosInfo->loc_uncrtnty_a;
  pLocPosInfo->horUncEllipseSemiMinor =
    pGfcPosInfo->loc_uncrtnty_p;

  pLocPosInfo->horUncEllipseOrientAzimuth =
    pGfcPosInfo->loc_uncrtnty_ang;

  gnss_GetUnixEpochFromUTC(&(pGfcPosInfo->utc_time),
                           &timeInSec);
  pLocPosInfo->timestampUtc = (timeInSec*1000);

  if(pGfcPosInfo->height_incl)
  {
     pLocPosInfo->altitudeWrtEllipsoid_valid = 1;
      pLocPosInfo->altitudeWrtEllipsoid =
        pGfcPosInfo->height;
  }
  if(pGfcPosInfo->height_uncrtnty_incl)
  {
    pLocPosInfo->vertUnc_valid = 1;
    pLocPosInfo->vertUnc =
      pGfcPosInfo->loc_uncrtnty_v;
  }
  if(pGfcPosInfo->velocity_incl)
  {
    pLocPosInfo->speedHorizontal_valid = 1;
    pLocPosInfo->speedHorizontal =
      pGfcPosInfo->velocity_hor;
    //TBD: No GFC mask for vertical velocity so assuming it is
    //valid when velocity_incl is set
    pLocPosInfo->speedVertical_valid = 1;
    pLocPosInfo->speedVertical =
      pGfcPosInfo->velocity_ver;
  }
  if(pGfcPosInfo->heading_valid)
  {
    pLocPosInfo->heading_valid = 1;
    pLocPosInfo->heading = pGfcPosInfo->heading;
  }
  return TRUE;
}


/** convertBatchBreachRespToLocInd
 *  convert a batch breach event from GFC to QMI  LOC format
 *  @param [in] pGfcBreachInd : GFC batch breach indication
 *  @param [out]  pBreach : QMI LOC batch breach indication
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertBatchBreachRespToLocInd(
  const gfc_qmi_batch_breach_geofence_rsp_s_type* const pGfcBreachInd,
  qmiLocEventGeofenceBatchedBreachIndMsgT_v02 * const pBreachInd)
{
  uint32 i = 0;
  gfc_qmi_cont_gid_s_type *pCont = NULL;

  if( NULL == pGfcBreachInd || NULL == pBreachInd)
  {
    LOC_MSG_ERROR("convertBatchBreachRespToQmiInd: NULL parameters\n", 0,0,0);
    return FALSE;
  }

  if(pGfcBreachInd->origin != GFC_QMI_GEOFENCE_ORIGIN_MO)
  {
    LOC_MSG_ERROR("convertBreachRespToQmiInd: unknown origin\n", 0,0,0);
    return FALSE;
  }
  switch(pGfcBreachInd->breach_type)
  {
    case GFC_QMI_BREACH_GEOFENCE_TYPE_ENTERING:
      pBreachInd->breachType = eQMI_LOC_GEOFENCE_BREACH_TYPE_ENTERING_V02;
      break;

    case GFC_QMI_BREACH_GEOFENCE_TYPE_LEAVING:
      pBreachInd->breachType = eQMI_LOC_GEOFENCE_BREACH_TYPE_LEAVING_V02;
      break;

    default:
      LOC_MSG_ERROR("convertBatchBreachRespToQmiInd: Unknown breach type\n", 0,0,0);
      return FALSE;
      break;
  }

  if(pGfcBreachInd->confidence_rep_mask & GFC_QMI_CONFIDENCE_REPORT_ON_BREACH_TRUE)
  {
    pBreachInd->breachConfidence_valid = 1;
    switch(pGfcBreachInd->confidence)
    {
      case GFC_QMI_CONFIDENCE_LOW:
        pBreachInd->breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02;
        break;
      case GFC_QMI_CONFIDENCE_MEDIUM:
        pBreachInd->breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_MED_V02;
        break;
      case GFC_QMI_CONFIDENCE_HIGH:
        pBreachInd->breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH_V02;
        break;
      default:
       LOC_MSG_ERROR("convertBreachRespToLocInd: Unknown breach confidence type\n", 0,0,0);
       return FALSE;
    }
  }
  if(pGfcBreachInd->pos_rep_mask & GFC_QMI_POS_REPORT_ON_BREACH_TRUE)
  {
    uint64_t timeInSec;
    pBreachInd->geofencePosition_valid = 1;
    //TBD: convert position
    pBreachInd->geofencePosition.latitude = pGfcBreachInd->pos_fix.latitude;
    pBreachInd->geofencePosition.longitude = pGfcBreachInd->pos_fix.longitude;

    pBreachInd->geofencePosition.horUncEllipseSemiMajor =
      pGfcBreachInd->pos_fix.loc_uncrtnty_a;

    pBreachInd->geofencePosition.horUncEllipseSemiMinor =
      pGfcBreachInd->pos_fix.loc_uncrtnty_p;

    pBreachInd->geofencePosition.horUncEllipseOrientAzimuth =
      pGfcBreachInd->pos_fix.loc_uncrtnty_ang;

    gnss_GetUnixEpochFromUTC(&(pGfcBreachInd->pos_fix.utc_time),
                             &timeInSec);
    pBreachInd->geofencePosition.timestampUtc = (timeInSec*1000);

    if(pGfcBreachInd->pos_fix.height_incl)
    {
       pBreachInd->geofencePosition.altitudeWrtEllipsoid_valid = 1;
        pBreachInd->geofencePosition.altitudeWrtEllipsoid =
          pGfcBreachInd->pos_fix.height;
    }
    if(pGfcBreachInd->pos_fix.height_uncrtnty_incl)
    {
      pBreachInd->geofencePosition.vertUnc_valid = 1;
      pBreachInd->geofencePosition.vertUnc =
        pGfcBreachInd->pos_fix.loc_uncrtnty_v;
    }
    if(pGfcBreachInd->pos_fix.velocity_incl)
    {
      pBreachInd->geofencePosition.speedHorizontal_valid = 1;
      pBreachInd->geofencePosition.speedHorizontal =
        pGfcBreachInd->pos_fix.velocity_hor;
      //TBD: No GFC mask for vertical velocity so assuming it is
      //valid when velocity_incl is set
      pBreachInd->geofencePosition.speedVertical_valid = 1;
      pBreachInd->geofencePosition.speedVertical =
        pGfcBreachInd->pos_fix.velocity_ver;
    }
    if(pGfcBreachInd->pos_fix.heading_valid)
    {
      pBreachInd->geofencePosition.heading_valid = 1;
      pBreachInd->geofencePosition.heading = pGfcBreachInd->pos_fix.heading;
      /*  Heading uncertainty in degrees */
      pBreachInd->headingUnc = pGfcBreachInd->pos_fix.headingUnc;
      pBreachInd->headingUnc_valid = 1;
    }
    /* Vertical uncertainty in meters.
       It is already populated in pBreachInd->geofencePosition.vertUnc .
       So we disable this value for now . */
    pBreachInd->vertUnc = pGfcBreachInd->pos_fix.loc_uncrtnty_v;
    pBreachInd->vertUnc_valid = 0;

    /* 3-D Speed uncertainty in m/s */
    pBreachInd->speedUnc = pGfcBreachInd->pos_fix.speedUnc;
    pBreachInd->speedUnc_valid = 1;

    /* Horizontal uncertainty confidence in percentage */
    pBreachInd->horConfidence = pGfcBreachInd->pos_fix.horConfidence;
    pBreachInd->horConfidence_valid = 1;

    /* Vertical uncertainty confidence in percentage */
    pBreachInd->vertConfidence = pGfcBreachInd->pos_fix.vertConfidence;
    pBreachInd->vertConfidence_valid = 1;

    //Dilution of precision
    memscpy(&pBreachInd->DOP, sizeof(pBreachInd->DOP), &pGfcBreachInd->pos_fix.DOP, sizeof(pGfcBreachInd->pos_fix.DOP));
    pBreachInd->DOP_valid = 1;

    /* Number of SVs used for calculating this position */
    pBreachInd->gnssSvUsedList_len = pGfcBreachInd->pos_fix.gnssSvUsedList_len;

    /* SV  used info */
    memscpy(&pBreachInd->gnssSvUsedList, sizeof(pBreachInd->gnssSvUsedList),
            &pGfcBreachInd->pos_fix.gnssSvUsedList, sizeof(pGfcBreachInd->pos_fix.gnssSvUsedList));
    pBreachInd->gnssSvUsedList_valid = 1;
  }
  // Collect the discrete GIDs from the report
  if(pGfcBreachInd->q_discrete_list_len > 0)
  {
    pBreachInd->geofenceIdDiscreteList_len = pGfcBreachInd->q_discrete_list_len;
    pBreachInd->geofenceIdDiscreteList_valid = TRUE;
    for ( i = 0; i < pBreachInd->geofenceIdDiscreteList_len; i++)
    {
      if(i < QMI_LOC_MAX_GEOFENCE_ID_DISCRETE_LIST_LENGTH_V02)
      {
        pBreachInd->geofenceIdDiscreteList[i] = pGfcBreachInd->data[i];
      }
    }
  }

  // Collect the continuous GIDs from the report
  if(pGfcBreachInd->q_cont_list_len > 0)
  {
    pBreachInd->geofenceIdContinuousList_len = pGfcBreachInd->q_cont_list_len;
    pBreachInd->geofenceIdContinuousList_valid = TRUE;
    pCont = (gfc_qmi_cont_gid_s_type*)&pGfcBreachInd->data[pGfcBreachInd->q_discrete_list_len];
    for (i = 0; i < pBreachInd->geofenceIdContinuousList_len; i++)
    {
      if(i < QMI_LOC_MAX_GEOFENCE_ID_DISCRETE_LIST_LENGTH_V02)
      {
        pBreachInd->geofenceIdContinuousList[i].idLow = pCont->gid_low;
        pBreachInd->geofenceIdContinuousList[i].idHigh = pCont->gid_high;
      }
      pCont++;
    }
  }
  return TRUE;
}


/** convertBatchDwellRespToLocInd
 *  convert a batch dwell event from GFC to QMI  LOC format
 *  @param [in] pGfcDwellInd : GFC batch dwell indication
 *  @param [out]  pDwell : QMI LOC batch dwell indication
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertBatchDwellRespToLocInd(
  const gfc_qmi_batch_dwell_geofence_rsp_s_type* const pGfcDwellInd,
  qmiLocEventGeofenceBatchedDwellIndMsgT_v02 * const pDwellInd)
{
  uint32 i = 0;
  gfc_qmi_cont_gid_s_type *pCont = NULL;

  if( NULL == pGfcDwellInd || NULL == pDwellInd)
  {
    LOC_MSG_ERROR("convertBatchDwellRespToQmiInd: NULL parameters\n", 0,0,0);
    return FALSE;
  }

  if(pGfcDwellInd->origin != GFC_QMI_GEOFENCE_ORIGIN_MO)
  {
    LOC_MSG_ERROR("convertDwellRespToQmiInd: unknown origin\n", 0,0,0);
    return FALSE;
  }
  switch(pGfcDwellInd->dwell_type)
  {
    case GFC_QMI_GEOFENCE_DWELL_TYPE_INSIDE_MASK:
      pDwellInd->dwellType = eQMI_LOC_GEOFENCE_DWELL_TYPE_INSIDE_V02;
      break;

    case GFC_QMI_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK:
      pDwellInd->dwellType = eQMI_LOC_GEOFENCE_DWELL_TYPE_OUTSIDE_V02;
      break;

    default:
      LOC_MSG_ERROR("convertBatchDwellRespToQmiInd: Unknown dwell type\n", 0,0,0);
      return FALSE;
      break;
  }

  if(pGfcDwellInd->pos_rep_mask & GFC_QMI_POS_REPORT_ON_BREACH_TRUE)
  {
    /* Copy the location information from GFC to LOC API format */
    convGfcLocationInfoToLoc(&pGfcDwellInd->pos_fix, &pDwellInd->geofencePosition);
    pDwellInd->geofencePosition_valid = 1;
    if(pGfcDwellInd->pos_fix.heading_valid)
    {
      /*  Heading uncertainty in degrees */
      pDwellInd->headingUnc = pGfcDwellInd->pos_fix.headingUnc;
      pDwellInd->headingUnc_valid = 1;
    }
    /* Vertical uncertainty in meters.
       It is already populated in pDwellInd->geofencePosition.vertUnc .
       So we disable this value for now . */
    pDwellInd->vertUnc = pGfcDwellInd->pos_fix.loc_uncrtnty_v;
    pDwellInd->vertUnc_valid = 0;

    /* 3-D Speed uncertainty in m/s */
    pDwellInd->speedUnc = pGfcDwellInd->pos_fix.speedUnc;
    pDwellInd->speedUnc_valid = 1;

    /* Horizontal uncertainty confidence in percentage */
    pDwellInd->horConfidence = pGfcDwellInd->pos_fix.horConfidence;
    pDwellInd->horConfidence_valid = 1;

    /* Vertical uncertainty confidence in percentage */
    pDwellInd->vertConfidence = pGfcDwellInd->pos_fix.vertConfidence;
    pDwellInd->vertConfidence_valid = 1;

    //Dilution of precision
    memscpy(&pDwellInd->DOP, sizeof(pDwellInd->DOP), &pGfcDwellInd->pos_fix.DOP, sizeof(pGfcDwellInd->pos_fix.DOP));
    pDwellInd->DOP_valid = 1;

    /* Number of SVs used for calculating this position */
    pDwellInd->gnssSvUsedList_len = pGfcDwellInd->pos_fix.gnssSvUsedList_len;

    /* SV  used info */
    memscpy(&pDwellInd->gnssSvUsedList, sizeof(pDwellInd->gnssSvUsedList),
            &pGfcDwellInd->pos_fix.gnssSvUsedList, sizeof(pGfcDwellInd->pos_fix.gnssSvUsedList));
    pDwellInd->gnssSvUsedList_valid = 1;
  }
  // Collect the discrete GIDs from the report
  if(pGfcDwellInd->q_discrete_list_len > 0)
  {
    pDwellInd->geofenceIdDiscreteList_len = pGfcDwellInd->q_discrete_list_len;
    pDwellInd->geofenceIdDiscreteList_valid = TRUE;
    for ( i = 0; i < pDwellInd->geofenceIdDiscreteList_len; i++)
    {
      if(i < QMI_LOC_MAX_GEOFENCE_ID_DISCRETE_LIST_LENGTH_V02)
      {
        pDwellInd->geofenceIdDiscreteList[i] = pGfcDwellInd->data[i];
      }
    }
  }

  // Collect the continuous GIDs from the report
  if(pGfcDwellInd->q_cont_list_len > 0)
  {
    pDwellInd->geofenceIdContinuousList_len = pGfcDwellInd->q_cont_list_len;
    pDwellInd->geofenceIdContinuousList_valid = TRUE;
    pCont = (gfc_qmi_cont_gid_s_type*)&pGfcDwellInd->data[pGfcDwellInd->q_discrete_list_len];
    for (i = 0; i < pDwellInd->geofenceIdContinuousList_len; i++)
    {
      if(i < QMI_LOC_MAX_GEOFENCE_ID_DISCRETE_LIST_LENGTH_V02)
      {
        pDwellInd->geofenceIdContinuousList[i].idLow = pCont->gid_low;
        pDwellInd->geofenceIdContinuousList[i].idHigh = pCont->gid_high;
      }
      pCont++;
    }
  }
  return TRUE;
}


/** convertProximityRespToLocInd
 *  convert an proximity event from GFC to QMI  LOC format
 *  @param [in] pGfcBreachInd : GFC breach indication
 *  @param [out]  pBreach : QMI LOC breach indication
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertProximityRespToLocInd(
  const gfc_qmi_proximity_geofence_rsp_s_type* const pGfcProximityInd,
  qmiLocEventGeofenceProximityIndMsgT_v02 * const pProximityInd)
{
  if( NULL == pGfcProximityInd || NULL == pProximityInd)
  {
    LOC_MSG_ERROR("convertProximityRespToLocInd: NULL parameters\n", 0,0,0);
    return FALSE;
  }

  switch(pGfcProximityInd->proximity_type)
  {
    case GFC_QMI_PROXIMITY_IND_TYPE_IN:
      pProximityInd->proximityType = eQMI_LOC_GEOFENCE_PROXIMITY_TYPE_IN_V02;
      break;

    case GFC_QMI_PROXIMITY_IND_TYPE_OUT:
      pProximityInd->proximityType = eQMI_LOC_GEOFENCE_PROXIMITY_TYPE_OUT_V02;
      break;

    default:
      LOC_MSG_ERROR("convertProximityRespToLocInd: Unknown proximity type\n", 0,0,0);
      return FALSE;
      break;
  }
   pProximityInd->geofenceId = pGfcProximityInd->geofence_id;
  if(pGfcProximityInd->context_id_valid)
  {
    pProximityInd->contextId_valid = 1;
    pProximityInd->contextId = pGfcProximityInd->context_id;
  }

  return TRUE;
}


/*********************************** CALLBACKS invoked by GFC ******************************************/

/** locGeofenceAddRespCb
 *  Add response callback called in SM context. The callback
 *  checks if the origin is NI or MO, converts the payload from
 *  GFC to QMI LOC format, and generates the appropriate IPC msg
 *  to loc MW.
 *  @param [in] addGeofenceInd
 *   */

static void locGeofenceAddRespCb(
  const gfc_qmi_add_geofence_rsp_s_type * const pAddGeofenceInd)
{
  void *pPayload = NULL;
  size_t payloadSize = 0;
  loc_middleware_msg_id_e_type msgId;
  boolean sendIPCFlag = FALSE;

  if(NULL != pAddGeofenceInd)
  {
    /** check if message is for NI geofence add */

    if(GFC_QMI_GEOFENCE_ORIGIN_NI == pAddGeofenceInd->origin)
    {
      if(GFC_QMI_ERROR_NONE == pAddGeofenceInd->return_code)
      {
        /** convert to QMI LOC NI Geofence Add Indication */
        qmiLocEventNiGeofenceNotificationIndMsgT_v02 geofenceNiInd;

        memset(&geofenceNiInd, 0, sizeof(geofenceNiInd));

        geofenceNiInd.geofenceId =  pAddGeofenceInd->geofence_id;
        geofenceNiInd.operationType = eQMI_LOC_NI_GEOFENCE_ADDED_V02;

        /** send IPC to loc Mw for NI geofence add indication */
        pPayload = (void *)&geofenceNiInd;
        msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_NI_IND;
        payloadSize = sizeof(geofenceNiInd);
        sendIPCFlag = TRUE;
      }
      else
      {
        LOC_MSG_ERROR("locGeofenceAddRespCb: NI geofence return code err %d\n",
                      pAddGeofenceInd->return_code, 0, 0);
      }
    }
    /** check if origin is MO */
    else if(GFC_QMI_GEOFENCE_ORIGIN_MO == pAddGeofenceInd->origin)
    {
      /** convert to QMI_LOC Add indication */
      qmiLocAddCircularGeofenceIndMsgT_v02 geofenceAddInd;
      memset(&geofenceAddInd, 0, sizeof(geofenceAddInd));

      /** populate status and txn Id */
      geofenceAddInd.status =
        convertGfcResultToQmiLocStatus(pAddGeofenceInd->return_code);

      geofenceAddInd.transactionId_valid = 1;
      geofenceAddInd.transactionId = pAddGeofenceInd->transaction_id;

      /** if status is success, populate geofence Id */
      if(eQMI_LOC_SUCCESS_V02 == geofenceAddInd.status )
      {
        geofenceAddInd.geofenceId_valid = 1;
        geofenceAddInd.geofenceId = pAddGeofenceInd->geofence_id;
      }

      /** send IPC to loc MW for MO geofence add indication */
      pPayload = (void *)&geofenceAddInd;
      msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_ADD_IND;
      payloadSize = sizeof(geofenceAddInd);
      sendIPCFlag = TRUE;
    }
    else
    {
      LOC_MSG_ERROR("locGeofenceAddRespCb :Unknown Origin %d\n",
                   pAddGeofenceInd->origin, 0, 0);
    }

    if( TRUE == sendIPCFlag)
    {
      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(msgId, payloadSize,pPayload ))
      {
        LOC_MSG_ERROR("locGeofenceAddRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }

  return;
}

/** locGeofenceDeleteRespCb
 *  Delete indication callback called in SM context. The
 *  callback checks if the origin is NI or MO, converts the
 *  payload from GFC to QMI LOC format, and generates the
 *  appropriate IPC msg to loc MW.
 *  @param [in] deleteGeofenceInd
 *   */

static void locGeofenceDeleteRespCb(
  const gfc_qmi_delete_geofence_rsp_s_type * const pDeleteGeofenceInd)
{
  void *pPayload = NULL;
  size_t payloadSize = 0;
  loc_middleware_msg_id_e_type msgId;
  boolean sendIPCFlag = FALSE;

  if(NULL != pDeleteGeofenceInd)
  {
    /** check if message is because a NI originated geofence was
     *  deleted */
    if(GFC_QMI_GEOFENCE_ORIGIN_NI == pDeleteGeofenceInd->origin)
    {
      if(GFC_QMI_ERROR_NONE == pDeleteGeofenceInd->return_code)
      {
        /** convert to QMI LOC NI Geofence Delete Indication */
        qmiLocEventNiGeofenceNotificationIndMsgT_v02 geofenceNiInd;

        memset(&geofenceNiInd, 0, sizeof(geofenceNiInd));

        geofenceNiInd.geofenceId =  pDeleteGeofenceInd->geofence_id;
        geofenceNiInd.operationType = eQMI_LOC_NI_GEOFENCE_DELETED_V02;

        /** send IPC to loc Mw for NI geofence delete indication */
        pPayload = (void *)&geofenceNiInd;
        msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_NI_IND;
        payloadSize = sizeof(geofenceNiInd);
        sendIPCFlag = TRUE;
      }
      else
      {
        LOC_MSG_ERROR("locGeofenceDeleteRespCb : NI geofence return code err %d\n",
                   pDeleteGeofenceInd->return_code, 0, 0);
      }
    }

    /** check if an MO geofence was deleted */
    else if(GFC_QMI_GEOFENCE_ORIGIN_MO == pDeleteGeofenceInd->origin)
    {
      /** loc MW always sets duration to infinity so the only
       *  reason handled is if MO client deleted the geofence */
      if( GFC_QMI_GEOFENCE_DEL_REASON_MO_CLIENT_REQ ==
          pDeleteGeofenceInd->del_reason)
      {
        /** convert to QMI_LOC delete indication */
        qmiLocDeleteGeofenceIndMsgT_v02 geofenceDelInd;
        memset(&geofenceDelInd, 0, sizeof(geofenceDelInd));

        /** populate status and txn Id */
        geofenceDelInd.status =
          convertGfcResultToQmiLocStatus(pDeleteGeofenceInd->return_code);

        geofenceDelInd.geofenceId_valid = 1;
        geofenceDelInd.geofenceId = pDeleteGeofenceInd->geofence_id;

        geofenceDelInd.transactionId_valid = 1;
        geofenceDelInd.transactionId = pDeleteGeofenceInd->transaction_id;


        /** send IPC to loc MW for MO geofence delete indication */
        pPayload = (void *)&geofenceDelInd;
        msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_DELETE_IND;
        payloadSize = sizeof(geofenceDelInd);
        sendIPCFlag = TRUE;
      }
      else
      {
        LOC_MSG_ERROR("locGeofenceDeleteRespCb :Unknown delete reason %d\n",
                      pDeleteGeofenceInd->del_reason, 0, 0);
      }
    }
    else
    {
      LOC_MSG_ERROR("locGeofenceDeleteRespCb :Unknown delete origin %d\n",
                      pDeleteGeofenceInd->origin, 0, 0);
    }

    if( TRUE == sendIPCFlag)
    {
      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(msgId, payloadSize,pPayload ))
      {
        LOC_MSG_ERROR("locGeofenceDeleteRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceDeleteRespCb: NULL payload\n",
                 0,0,0);
  }

  return;
}

/** locGeofenceEditRespCb
 *  Edit indication callback called in SM context. The callback
 *  converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGfcEditGeofenceInd
 */

static void locGeofenceEditRespCb(
  const gfc_qmi_edit_geofence_rsp_s_type * const pGfcEditGeofenceInd)
{
  void *pPayload = NULL;
  size_t payloadSize = 0;
  loc_middleware_msg_id_e_type msgId;

  if(NULL != pGfcEditGeofenceInd)
  {
    /** convert to QMI_LOC edit indication */
    qmiLocEditGeofenceIndMsgT_v02 geofenceEditInd;
    memset(&geofenceEditInd, 0, sizeof(geofenceEditInd));

    /** populate status and txn Id */
    geofenceEditInd.status =
      convertGfcResultToQmiLocStatus(pGfcEditGeofenceInd->return_code);

    /** geofence Id and tx ID are always valid as per GFC header */
    geofenceEditInd.geofenceId_valid = 1;
    geofenceEditInd.geofenceId = pGfcEditGeofenceInd->geofence_id;

    geofenceEditInd.transactionId_valid = 1;
    geofenceEditInd.transactionId = pGfcEditGeofenceInd->transaction_id;

    /** TBD: GFC doesn't set the failed params right now */
    geofenceEditInd.failedParams_valid = 0;

    /** send IPC to loc MW for geofence edit indication */
    pPayload = (void *)&geofenceEditInd;
    msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_EDIT_IND;
    payloadSize = sizeof(geofenceEditInd);

    /** Generate an IPC to loc MW*/
    if(FALSE == locGeofenceGenerateIPCtolocMW(msgId, payloadSize,pPayload ))
    {
      LOC_MSG_ERROR("locGeofenceEditRespCb: could not send IPC to loc MW\n",
                   0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceEditRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}

/** locGeofenceQueryRespCb
 *  Query indication callback called in SM context. The callback
 *  converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGfcQueryGeofenceInd
 */

static void locGeofenceQueryRespCb(
  const gfc_qmi_query_geofence_rsp_s_type * const pGfcQueryGeofenceInd)
{
  void *pPayload = NULL;
  size_t payloadSize = 0;
  loc_middleware_msg_id_e_type msgId;

  if(NULL != pGfcQueryGeofenceInd)
  {
    /** convert to QMI_LOC query indication */
    qmiLocQueryGeofenceIndMsgT_v02 geofenceQueryInd;
    memset(&geofenceQueryInd, 0, sizeof(geofenceQueryInd));

    /** convert Query response to QMI_LOC */
    if(TRUE == convertGfcQueryRespToLocInd(pGfcQueryGeofenceInd,
                                           &geofenceQueryInd ))
    {
      /** send IPC to loc MW for MO geofence query indication */
      pPayload = (void *)&geofenceQueryInd;
      msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_QUERY_IND;
      payloadSize = sizeof(geofenceQueryInd);

      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(msgId, payloadSize,pPayload ))
      {
        LOC_MSG_ERROR("locGeofenceQueryRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceQueryRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}

/** locGeofencePurgeRespCb Purge indication callback called in
 *  SM context. No IPC is generated since this is an internal
 *  function, called only when a geofence client disconnects.
 *  @param [in]  gfcResult : result of the purge operation
 */

static void locGeofencePurgeRespCb(
  const  gfc_qmi_operation_result_e_type gfcResult)
{
  LOC_MSG_MED("locGeofencePurgeRespCb: Purge result = %d\n",
                 gfcResult,0,0);

  if(GFC_QMI_ERROR_NONE != gfcResult )
  {
    LOC_MSG_ERROR("locGeofencePurgeRespCb: Purge failed error = %d\n",
                 gfcResult,0,0);
  }
  return;
}

/** locGeofenceGenAlertRespCb
 *  General Alert indication callback called in SM context. The
 *  callback converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGeofenceGenAlertInd
 */
static void locGeofenceGenAlertRespCb(
  const gfc_qmi_general_notify_rsp_e_type geofenceGenAlertInd)
{
  qmiLocEventGeofenceGenAlertIndMsgT_v02 genAlertInd;
  memset(&genAlertInd, 0, sizeof(genAlertInd));

  /** convert a general alert */
  switch(geofenceGenAlertInd)
  {
    case GFC_QMI_GENERAL_NOTIFY_GPS_OFF:
    {
        genAlertInd.geofenceAlert =
            eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE_V02;
        break;
    }

    case GFC_QMI_GENERAL_NOTIFY_GPS_ON:
    {
      genAlertInd.geofenceAlert =
          eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_AVAILABLE_V02;
      break;
    }

    case GFC_QMI_GENERAL_NOTIFY_OOS:
    {
        genAlertInd.geofenceAlert =
            eQMI_LOC_GEOFENCE_GEN_ALERT_OOS_V02;
        break;
    }

    case GFC_QMI_GENERAL_NOTIFY_TIMESTAMP_INVALID:
    {
        genAlertInd.geofenceAlert =
            eQMI_LOC_GEOFENCE_GEN_ALERT_TIME_INVALID_V02;
        break;
    }
    default:
    {
        LOC_MSG_ERROR("locGeofenceGenAlertRespCb: unknown alert type\n",
                0,0,0);
        return;
    }

  }

    /** Generate an IPC to loc MW*/
  if(FALSE == locGeofenceGenerateIPCtolocMW(
    LM_MIDDLEWARE_MSG_ID_GEOFENCE_GEN_ALERT_IND,
    sizeof(genAlertInd),
    (void *)&genAlertInd))
  {
    LOC_MSG_ERROR("locGeofenceGenAlertRespCb: could not send IPC to loc MW\n",
                  0,0,0);
  }
  return;
}

/** locGeofenceBreachRespCb
 *  Breach indication callback called in SM context. The
 *  callback converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGeofenceBreachInd
 */

static void locGeofenceBreachRespCb(
  const gfc_qmi_breach_geofence_rsp_s_type * const pGeofenceBreachInd)
{
  if(NULL != pGeofenceBreachInd)
  {
    qmiLocEventGeofenceBreachIndMsgT_v02 breachInd;
    memset(&breachInd, 0, sizeof(breachInd));
    if(TRUE == convertBreachRespToLocInd(pGeofenceBreachInd, &breachInd))
    {
      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(
        LM_MIDDLEWARE_MSG_ID_GEOFENCE_BREACH_IND,
        sizeof(breachInd),
        (void *)&breachInd))
      {
        LOC_MSG_ERROR("locGeofenceBreahcRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceBreahcRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}


/** locGeofenceProximityRespCb
 *  Proximity  indication callback called in SM context. The
 *  callback converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGeofenceBreachInd
 */

static void locGeofenceProximityRespCb(
  const gfc_qmi_proximity_geofence_rsp_s_type * const pGeofenceProximityInd)
{
  if(NULL != pGeofenceProximityInd)
  {
    qmiLocEventGeofenceProximityIndMsgT_v02 proxInd;
    memset(&proxInd, 0, sizeof(proxInd));
    if(TRUE == convertProximityRespToLocInd(pGeofenceProximityInd, &proxInd))
    {
      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(
        LM_MIDDLEWARE_MSG_ID_GEOFENCE_PROXIMITY_IND,
        sizeof(proxInd),
        (void *)&proxInd))
      {
        LOC_MSG_ERROR("locGeofenceProximityRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceProximityRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}


/** locGeofenceGetTimeZoneCb 
 *  This function is a callback registered with GFC QMI to send an IPC to loc mw
 *  when the GF engine needs a time zone injection
 *
 *  Handle the TimeZone request from the geofence engine. Send an IPC to LocMW.
 *  
 * */
static void locGeofenceGetTimeZoneCb(void)
{
  qmiLocEventGetTimeZoneReqIndMsgT_v02 tzInd = { 0 };

  tzInd.status = eQMI_LOC_SUCCESS_V02;

  MSG_MED("Req QMI Client for TimeZone info", 0, 0, 0);
  if(FALSE == locGeofenceGenerateIPCtolocMW(
        LM_MIDDLEWARE_MSG_ID_TZ_REQ_IND, 
        sizeof(tzInd),
        (void *)&tzInd))
  {
    LOC_MSG_ERROR("locGeofenceGetTimeZoneCb: could not send IPC to loc MW\n",
                   0,0,0);
  }
  
}


/** locGeofenceSetEngineConfigRespCb
 *  Set geofence engine config indication callback called in SM
 *  context. The callback converts the payload from GFC to QMI
 *  LOC format, and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGfcSetEngineConfigInd
 */

static void locGeofenceSetEngineConfigRespCb(
  const gfc_qmi_set_geofence_engine_config_rsp_s_type * const pGfcSetEngineConfigInd)
{
  void *pPayload = NULL;
  size_t payloadSize = 0;
  loc_middleware_msg_id_e_type msgId;

  if(NULL != pGfcSetEngineConfigInd)
  {
    /** convert to QMI_LOC Set engine config indication */
    qmiLocSetGeofenceEngineConfigIndMsgT_v02 geofenceSetEngineConfigInd;
    memset(&geofenceSetEngineConfigInd, 0, sizeof(geofenceSetEngineConfigInd));

    /** convert set engine config response to QMI_LOC */
    if(TRUE == convertGfcSetEngineConfigRespToLocInd(pGfcSetEngineConfigInd,
                                                     &geofenceSetEngineConfigInd ))
    {
      /** send IPC to loc MW for set engine config indication */
      pPayload = (void *)&geofenceSetEngineConfigInd;
      msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_SET_ENGINE_CONFIG_IND;
      payloadSize = sizeof(geofenceSetEngineConfigInd);

      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(msgId, payloadSize,pPayload ))
      {
        LOC_MSG_ERROR("locGeofenceSetEngineConfigRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceSetEngineConfigRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}

/** locGeofenceBatchBreachRespCb
 *  Breach indication callback called in SM context. The
 *  callback converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGeofenceBatchBreachInd
 */

static void locGeofenceBatchBreachRespCb(
  const gfc_qmi_batch_breach_geofence_rsp_s_type * const pGeofenceBatchBreachInd)
{
  if(NULL != pGeofenceBatchBreachInd)
  {
    qmiLocEventGeofenceBatchedBreachIndMsgT_v02 breachInd;
    memset(&breachInd, 0, sizeof(breachInd));
    if(TRUE == convertBatchBreachRespToLocInd(pGeofenceBatchBreachInd, &breachInd))
    {
      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(
        LM_MIDDLEWARE_MSG_ID_GEOFENCE_BATCH_BREACH_IND,
        sizeof(breachInd),
        (void *)&breachInd))
      {
        LOC_MSG_ERROR("locGeofenceBreahcRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceBreahcRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}


/** locGeofenceBatchDwellRespCb
 *  Breach indication callback called in SM context. The
 *  callback converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGeofenceBatchDwellInd
 */

static void locGeofenceBatchDwellRespCb(
  const gfc_qmi_batch_dwell_geofence_rsp_s_type * const pGeofenceBatchDwellInd)
{
  if(NULL != pGeofenceBatchDwellInd)
  {
    qmiLocEventGeofenceBatchedDwellIndMsgT_v02 dwellInd = {0};
    if(TRUE == convertBatchDwellRespToLocInd(pGeofenceBatchDwellInd, &dwellInd))
    {
      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(
        LM_MIDDLEWARE_MSG_ID_GEOFENCE_BATCH_DWELL_IND,
        sizeof(dwellInd),
        (void *)&dwellInd))
      {
        LOC_MSG_ERROR("locGeofenceDwellRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceDwellRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}

/**============================= FUNCTION DEFINITIONS=====================================================*/

/** locGeofenceHandleAddInd
 *  Handle the add indication sent by GFC QMI module. This
 *  should be invoked when: a) loc_geofence module had sent an
 *  add geofence request previously.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleAddInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocAddCircularGeofenceIndMsgT_v02)))
  {
    locQmiShimIndInfoStructT shimIndInfo;
    qmiLocAddCircularGeofenceIndMsgT_v02 geofenceAddInd;

    shimIndInfo.q_MsgLen = memscpy(&geofenceAddInd, sizeof(geofenceAddInd),
                                   pIpcMsg->p_Data, pIpcMsg->q_Size);

    /** send indication to the SHIM */
    shimIndInfo.p_Msg = &geofenceAddInd;
    shimIndInfo.q_Id = QMI_LOC_ADD_CIRCULAR_GEOFENCE_IND_V02;
    (void)sendIndicationByTxnId(g_addReqClientList,
                                geofenceAddInd.transactionId,
                                &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleAddInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceHandleDeleteInd
 *  Handle the delete indication sent by GFC QMI module. This
 *  should be invoked when loc_geofence module had sent an
 *  delete geofence request previously.
 *  @param [in] pIpcMsg
 */

void locGeofenceHandleDeleteInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocDeleteGeofenceIndMsgT_v02)))
  {
    locQmiShimIndInfoStructT shimIndInfo;
    qmiLocDeleteGeofenceIndMsgT_v02 geofenceDeleteInd;
    shimIndInfo.q_MsgLen = memscpy(&geofenceDeleteInd, sizeof(geofenceDeleteInd),
                                   pIpcMsg->p_Data, pIpcMsg->q_Size);

    /** send indication to the SHIM */
    shimIndInfo.p_Msg = &geofenceDeleteInd;
    shimIndInfo.q_Id = QMI_LOC_DELETE_GEOFENCE_IND_V02;
    (void)sendIndicationByTxnId(g_deleteReqClientList,
                                geofenceDeleteInd.transactionId,
                                &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleDeleteInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceHandleEditInd
 *  Handle the edit indication sent by GFC QMI module. This
 *  should be invoked when loc_geofence module had sent an edit
 *  geofence request previously.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleEditInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) && (pIpcMsg->q_Size == sizeof(qmiLocEditGeofenceIndMsgT_v02)))
  {
    qmiLocEditGeofenceIndMsgT_v02 geofenceEditInd;

    locQmiShimIndInfoStructT shimIndInfo;

    shimIndInfo.q_MsgLen = memscpy(&geofenceEditInd, sizeof(geofenceEditInd),
                                   pIpcMsg->p_Data, pIpcMsg->q_Size);

    /** send edit indication to the SHIM */
    shimIndInfo.p_Msg = &geofenceEditInd;
    shimIndInfo.q_Id = QMI_LOC_EDIT_GEOFENCE_IND_V02;
    (void)sendIndicationByTxnId(g_editReqClientList,
                                geofenceEditInd.transactionId,
                                &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleEditInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceHandleQueryInd
 *  Handle the query indication sent by GFC QMI module. This
 *  should be invoked when loc_geofence module had sent a query
 *  geofence request previously.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleQueryInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) && (pIpcMsg->q_Size == sizeof(qmiLocQueryGeofenceIndMsgT_v02)))
  {
     qmiLocQueryGeofenceIndMsgT_v02 geofenceQueryInd;
     locQmiShimIndInfoStructT shimIndInfo;

    shimIndInfo.q_MsgLen = memscpy(&geofenceQueryInd, sizeof(geofenceQueryInd),
                                    pIpcMsg->p_Data, pIpcMsg->q_Size);


    /** send edit indication to the SHIM */
    shimIndInfo.p_Msg = &geofenceQueryInd;
    shimIndInfo.q_Id = QMI_LOC_QUERY_GEOFENCE_IND_V02;
    (void)sendIndicationByTxnId(g_queryReqClientList,
                                geofenceQueryInd.transactionId,
                                &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleQueryInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceHandleNiInd
 *  Handle the NI geofence indication sent by GFC QMI module. This
 *  indication should be sent as an event to the client. For NI
 *  add, add the the entry to global list, for NI delete update
 *  the deleteReqInfo in the global list.
 *  @param [in] pIpcMsg
 * */
void locGeofenceHandleNiInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocEventNiGeofenceNotificationIndMsgT_v02)))
  {
    qmiLocEventNiGeofenceNotificationIndMsgT_v02 geofenceNiInd;
    locQmiShimIndInfoStructT shimIndInfo;

    shimIndInfo.q_MsgLen = memscpy(&geofenceNiInd, sizeof(geofenceNiInd),
                                   pIpcMsg->p_Data, pIpcMsg->q_Size);

    /** send the event to all the clients who registered */
    shimIndInfo.p_Msg = &geofenceNiInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_NI_GEOFENCE_NOTIFICATION_IND_V02;
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_NI_GEOFENCE_NOTIFICATION_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("locGeofenceHandleNiInd: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleNiInd: Invalid IPC message\n", 0,0,0);
  }
  return;
}

/** locGeofenceHandleGenAlertInd
 *  Handle the geofence general alert indication sent by GFC QMI
 *  module. This indication should be sent as an event to the
 *  client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleGenAlertInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocEventGeofenceGenAlertIndMsgT_v02)))
  {
    qmiLocEventGeofenceGenAlertIndMsgT_v02 genAlertInd;
    locQmiShimIndInfoStructT shimIndInfo;

    shimIndInfo.q_MsgLen = memscpy(&genAlertInd, sizeof(genAlertInd),
                                   pIpcMsg->p_Data, pIpcMsg->q_Size);

    // TBD: send a unicast message
    shimIndInfo.p_Msg = &genAlertInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_GEOFENCE_GEN_ALERT_IND_V02;
    // Broadcast to all registered clients for now
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("locGeofenceHandleGenAlertInd: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleGenAlertInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceHandleBreachInd
 *  Handle the breach indication sent by GFC QMI module. This
 *  indicatio should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleBreachInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocEventGeofenceBreachIndMsgT_v02)))
  {
    qmiLocEventGeofenceBreachIndMsgT_v02 breachInd;
    locQmiShimIndInfoStructT shimIndInfo;

    shimIndInfo.q_MsgLen = memscpy(&breachInd, sizeof(breachInd), pIpcMsg->p_Data, pIpcMsg->q_Size);

    shimIndInfo.p_Msg = &breachInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_GEOFENCE_BREACH_NOTIFICATION_IND_V02;

    /** Broadcast to all registered clients */
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("locGeofenceHandleBreachInd: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleBreachInd: Invalid IPC message\n", 0,0,0);
  }
  return;
}

/** locGeofenceHandleProximityInd
 *  Handle the Proximity indication sent by GFC QMI module. This
 *  indicatio should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleProximityInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocEventGeofenceProximityIndMsgT_v02)))
  {
    qmiLocEventGeofenceProximityIndMsgT_v02 proxInd;
    locQmiShimIndInfoStructT shimIndInfo;

    shimIndInfo.q_MsgLen = memscpy(&proxInd, sizeof(proxInd), pIpcMsg->p_Data, pIpcMsg->q_Size);

    shimIndInfo.p_Msg = &proxInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_GEOFENCE_PROXIMITY_NOTIFICATION_IND_V02;

    /** Broadcast to all registered clients */
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_GEOFENCE_PROXIMITY_NOTIFICATION_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("locGeofenceHandleProximityInd: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleProximityInd: Invalid IPC message\n", 0,0,0);
  }
  return;
}

/** locGeofenceHandleGetTzReq
 *  Handle the TimeZone request from the geofence engine in LocMW context
 *  by sending the TIME_ZONE_REQ to QMI clients/control points.
 *  
 * */
void locGeofenceHandleGetTzReq(void)
{
  qmiLocEventGetTimeZoneReqIndMsgT_v02 tzInd = { 0 };
  locQmiShimIndInfoStructT shimIndInfo; 
  
  tzInd.status = eQMI_LOC_SUCCESS_V02;

  shimIndInfo.p_Msg = &tzInd;
  shimIndInfo.q_Id  = QMI_LOC_EVENT_GET_TIME_ZONE_INFO_IND_V02;
  shimIndInfo.q_MsgLen = sizeof(tzInd);
  
  /** Broadcast to all registered clients */
  if(FALSE == locQmiShimBroadcastInd(
     QMI_LOC_EVENT_MASK_GET_TIME_ZONE_REQ_V02,
     &shimIndInfo))
  {
    LOC_MSG_ERROR("locGeofenceHandleGetTzReq: Could not send indication\n",
                   0,0,0);
  }
}

/** locGeofenceHandleBatchBreachInd
 *  Handle the breach indication sent by GFC QMI module. This
 *  indicatio should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleBatchBreachInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocEventGeofenceBatchedBreachIndMsgT_v02)))
  {
    qmiLocEventGeofenceBatchedBreachIndMsgT_v02 breachInd;
    locQmiShimIndInfoStructT shimIndInfo;

    shimIndInfo.q_MsgLen = memscpy(&breachInd, sizeof(breachInd), pIpcMsg->p_Data, pIpcMsg->q_Size);

    shimIndInfo.p_Msg = &breachInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_GEOFENCE_BATCHED_BREACH_NOTIFICATION_IND_V02;

    /** Broadcast to all registered clients */
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_BREACH_NOTIFICATION_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("locGeofenceHandleBatchBreachInd: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleBatchBreachInd: Invalid IPC message\n", 0,0,0);
  }
  return;
}


/** locGeofenceHandleBatchDwellInd
 *  Handle the breach indication sent by GFC QMI module. This
 *  indicatio should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleBatchDwellInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocEventGeofenceBatchedDwellIndMsgT_v02)))
  {
    qmiLocEventGeofenceBatchedDwellIndMsgT_v02 dwellInd;
    locQmiShimIndInfoStructT shimIndInfo;

    shimIndInfo.q_MsgLen = memscpy(&dwellInd, sizeof(dwellInd), pIpcMsg->p_Data, pIpcMsg->q_Size);

    shimIndInfo.p_Msg = &dwellInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_GEOFENCE_BATCHED_DWELL_NOTIFICATION_IND_V02;

    /** Broadcast to all registered clients */
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_DWELL_NOTIFICATION_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("locGeofenceHandleBatchDwellInd: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleBatchDwellInd: Invalid IPC message\n", 0,0,0);
  }
  return;
}



/** locGeofenceHandleSetEngineConfigInd
 *  Handle the set engine configuration indication sent by GFC
 *  QMI module. This will be invoked when: a) loc_geofence
 *  module had sent a set engine configuration request
 *  previously.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleSetEngineConfigInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocSetGeofenceEngineConfigIndMsgT_v02)))
  {
    locQmiShimIndInfoStructT shimIndInfo;
    qmiLocSetGeofenceEngineConfigIndMsgT_v02 geofenceSetEngineConfigInd;

    shimIndInfo.q_MsgLen = memscpy(&geofenceSetEngineConfigInd, sizeof(geofenceSetEngineConfigInd),
		pIpcMsg->p_Data, pIpcMsg->q_Size);

    /** send indication to the SHIM */
    shimIndInfo.p_Msg = &geofenceSetEngineConfigInd;
    shimIndInfo.q_Id = QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_IND_V02;
    (void)sendIndicationByTxnId(g_setEngineConfigReqClientList,
                                geofenceSetEngineConfigInd.transactionId,
                                &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleAddInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceInit
 * initialize loc MW goefence module and register the callbacks with the gfc_qmi
 * module.
 * @return : TRUE ; loc geofence module was initialized correctly
 *         : FALSE; loc geofence module could not be initialized.
 */
boolean locGeofenceInit(void)
{
  gfc_qmi_cb_rsp_s_type locGeofenceCallbacks = {
    locGeofenceAddRespCb,
    locGeofenceDeleteRespCb,
    locGeofencePurgeRespCb,
    locGeofenceEditRespCb,
    locGeofenceQueryRespCb,
    locGeofenceBreachRespCb,
    locGeofenceGenAlertRespCb,
    NULL,
    locGeofenceSetEngineConfigRespCb,
    locGeofenceBatchBreachRespCb,
    locGeofenceProximityRespCb,
    locGeofenceBatchDwellRespCb,    
    locGeofenceGetTimeZoneCb
  };
  /** reset the global variable state */
  locGeofenceResetState();
  /** register the callbacks with gfc_qmi*/
  return( gfc_qmi_register(&locGeofenceCallbacks));
}

/** locGeofenceAdd
 * process an add geofence request
 * @param [in] locClientHandle : loc MW client handle
 * @param [in] addGeofenceReq :  add geofence request
 * @return : TRUE -- GFC_QMI accepted the operation to add the
 *           geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           add the geofence.
 */

boolean locGeofenceAdd(
  int locClientHandle,
  const qmiLocAddCircularGeofenceReqMsgT_v02* const pAddGeofenceReq,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  gfc_qmi_add_geofence_req_s_type gfcAddGeofenceReq;
  gfc_qmi_operation_result_e_type returnCode;
  gfc_qmi_operation_status_e_type status;

  memset(&gfcAddGeofenceReq, 0, sizeof(gfcAddGeofenceReq));

  /** convert to GFC_QMI format */
  if( FALSE == convertAddCircularGeofenceReqToGfc(
     pAddGeofenceReq, &gfcAddGeofenceReq))
  {
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
    LOC_MSG_ERROR("locGeofenceAdd: Invalid params\n", 0,0,0);
    return FALSE ;
  }

  /** If txn Id cannot be added send error back   */
  if(TRUE != addTxnIdToClientReq(g_addReqClientList,
                                 locClientHandle,
                                 pAddGeofenceReq->transactionId))
  {
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
    return FALSE;
  }

  /** call gfc api to add the geofence */
  status = gfc_qmi_add_geofence(&gfcAddGeofenceReq, &returnCode);

  if( TRUE != convertGfcStatusToQmiLocResp(status, returnCode, pResp))
  {
    LOC_MSG_ERROR("locGeofenceAdd: gfc error \n", 0,0,0);
    (void)clearTxnIdFromClientReq(g_addReqClientList,
                                 locClientHandle,
                                 pAddGeofenceReq->transactionId);
    return FALSE;
  }


  return TRUE;
}

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
boolean locGeofenceDelete(
  int locClientHandle,
  const qmiLocDeleteGeofenceReqMsgT_v02* const pDeleteGeofenceReq,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  gfc_qmi_operation_result_e_type returnCode;
  gfc_qmi_operation_status_e_type status;

  gfc_qmi_delete_geofence_req_s_type gfc_delete_req;

  /** sanity check the input */
  if(NULL == pDeleteGeofenceReq )
  {
    /** invalid parameters */
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
    LOC_MSG_ERROR("locGeofenceDelete: Invalid params\n", 0,0,0);
    return FALSE;
  }

  /** If txn Id cannot be added send error back   */
  if( TRUE != addTxnIdToClientReq( g_deleteReqClientList,
                                   locClientHandle,
                                   pDeleteGeofenceReq->transactionId))
  {
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
    return FALSE;
  }

  /** send the request to gfc client */
  memset(&gfc_delete_req, 0, sizeof(gfc_delete_req));

  gfc_delete_req.geofence_id = pDeleteGeofenceReq->geofenceId;
  gfc_delete_req.transaction_id = pDeleteGeofenceReq->transactionId;

  /** call gfc api to delete the geofence */
  status = gfc_qmi_delete_geofence(&gfc_delete_req, &returnCode);

  /** convert delete status to response and fill in the
   *  response code for QMI_LOC Gen resp */
  if(TRUE != convertGfcStatusToQmiLocResp(status, returnCode, pResp))
  {
    LOC_MSG_ERROR("locGeofenceDelete: gfc error \n", 0,0,0);
    (void)clearTxnIdFromClientReq(g_deleteReqClientList,
                                  locClientHandle,
                                  pDeleteGeofenceReq->transactionId);
    return FALSE;
  }

  return TRUE;
}

/** locGeofenceEdit
 * process an edit geofence request
 * @param [in] locClientHandle : loc MW client handle
 * @param [in] pEditGeofenceReq :  Edit geofence request ptr
 * @param [out] pResp            : QMI LOC response payload
 * @return : TRUE -- GFC_QMI accepted the operation to edit
 *           the geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           edit the geofence.
 */
boolean locGeofenceEdit(
  int locClientHandle,
  const qmiLocEditGeofenceReqMsgT_v02* const pEditGeofenceReq,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  gfc_qmi_operation_result_e_type returnCode;
  gfc_qmi_operation_status_e_type status;
  gfc_qmi_edit_geofence_req_s_type gfcEditReq;

  memset(&gfcEditReq, 0, sizeof(gfcEditReq));

  /** convert edit request to GFC fromat from QMI_LOC */
  if(FALSE == convertEditGeofenceReqToGfc(
    pEditGeofenceReq, &gfcEditReq))
  {
    /** invalid parameters */
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
    LOC_MSG_ERROR("locGeofenceEdit: Invalid params\n", 0,0,0);
    return FALSE;
  }

  /** If txn Id cannot be added send error back   */
  if(TRUE != addTxnIdToClientReq(g_editReqClientList,
                                 locClientHandle,
                                 pEditGeofenceReq->transactionId))
  {
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error =  QMI_ERR_DEVICE_IN_USE_V01;
    return FALSE;
  }

  /** call gfc api to edit the geofence */
  status = gfc_qmi_edit_geofence(&gfcEditReq, &returnCode);

  /** convert status to response and fill in the response code for
   *  QMI_LOC Gen resp */
  if( TRUE != convertGfcStatusToQmiLocResp(status, returnCode, pResp))
  {
    LOC_MSG_ERROR("locGeofenceEdit:gfc edit error\n",0,0,0);
    /**clear the entry from client req list */
    (void)clearTxnIdFromClientReq(g_editReqClientList,
                                  locClientHandle,
                                  pEditGeofenceReq->transactionId);
    return FALSE;
  }

  return TRUE;
}

/** locGeofenceQuery
 * process a query geofence request
 * @param [in] locClientHandle : loc MW client handle
 * @param [in] pQueryGeofenceReq :  Query geofence request ptr
 * @param [out] pResp            : QMI LOC response payload
 * @return : TRUE -- GFC_QMI accepted the operation to query
 *           the geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           query the geofence.
 */
boolean locGeofenceQuery(
  int locClientHandle,
  const qmiLocQueryGeofenceReqMsgT_v02* const pQueryGeofenceReq,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  gfc_qmi_operation_result_e_type returnCode;
  gfc_qmi_operation_status_e_type status;
  gfc_qmi_query_geofence_req_s_type gfcQueryReq;

  memset(&gfcQueryReq, 0, sizeof(gfcQueryReq));

  /** convert query request to GFC format from QMI_LOC */
  if(NULL == pQueryGeofenceReq)
  {
    /** invalid parameters */
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
    LOC_MSG_ERROR("locGeofenceQuery: Invalid params\n", 0,0,0);
    return FALSE;
  }

  /** If txn Id cannot be added send error back   */
  if(TRUE != addTxnIdToClientReq(g_queryReqClientList,
                                  locClientHandle,
                                  pQueryGeofenceReq->transactionId))
  {
     pResp->resp.result =  QMI_RESULT_FAILURE_V01;
     pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
     return FALSE;
  }

  gfcQueryReq.geofence_id = pQueryGeofenceReq->geofenceId;
  gfcQueryReq.transaction_id = pQueryGeofenceReq->transactionId;

  /** call gfc api to edit the geofence */
  status = gfc_qmi_query_geofence(&gfcQueryReq, &returnCode);

  /** convert status to response and fill in the response code for
   *  QMI_LOC Gen resp */
  if(TRUE != convertGfcStatusToQmiLocResp(status, returnCode, pResp))
  {
    LOC_MSG_ERROR("locGeofenceQuery:gfc query error\n",0,0,0);
    /**clear the entry from client req list */
    (void)clearTxnIdFromClientReq(g_queryReqClientList,
                                  locClientHandle,
                                  pQueryGeofenceReq->transactionId);
    return FALSE;
  }
  return TRUE;
}


/** locGeofenceSetEngineConfig
 * process a Set Geofence Engine config request.
 * @param [in] locClientHandle : loc MW client handle
 * @param [in] pQueryGeofenceReq :  Query geofence request ptr
 * @param [out] pResp            : QMI LOC response payload
 * @return : TRUE -- GFC_QMI accepted the operation to query
 *           the geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           query the geofence.
 */
boolean locGeofenceSetEngineConfig(
  int locClientHandle,
  const qmiLocSetGeofenceEngineConfigReqMsgT_v02* const pSetGeofenceEngineConfigReq,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  gfc_qmi_operation_result_e_type returnCode;
  /* The return value from this api */
  boolean retVal = FALSE;
  uint8 u_CntTlv = 0;
  gfc_qmi_operation_status_e_type status;
  gfc_qmi_set_geofence_engine_config_req_s_type gfcSetGeofenceEngineConfigReq;

  memset(&gfcSetGeofenceEngineConfigReq, 0, sizeof(gfcSetGeofenceEngineConfigReq));

  u_CntTlv += ((pSetGeofenceEngineConfigReq->enableCpiUsage_valid)?1:0);
  u_CntTlv += ((pSetGeofenceEngineConfigReq->enableMotionDetectionSources_valid)?1:0);
  u_CntTlv += ((pSetGeofenceEngineConfigReq->gnssPositionSessionTimeout_valid)?1:0);
  u_CntTlv += ((pSetGeofenceEngineConfigReq->gnssUnavailableIndicationTimeout_valid)?1:0);
  u_CntTlv += ((pSetGeofenceEngineConfigReq->maxGeofences_valid)?1:0);
  do
  {
    if ( u_CntTlv > 1 )
    {
      /** invalid parameters */
      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
      LOC_MSG_ERROR(" locGeofenceSetEngineConfig : "
                     "More than one configuration specified",0,0,0);
      retVal = FALSE;
      break;
    }

    /** convert the request to GFC format from QMI_LOC */
    if(FALSE == convertSetGeofenceEngineConfigReqToGfc(pSetGeofenceEngineConfigReq, &gfcSetGeofenceEngineConfigReq))
    {
      /** invalid parameters */
      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
      LOC_MSG_ERROR("locGeofenceSetEngineConfig: Invalid params\n", 0,0,0);
      retVal = FALSE;
      break;
    }
    /** If txn Id cannot be added send error back   */
    if (TRUE != addTxnIdToClientReq(g_setEngineConfigReqClientList,
                                    locClientHandle,
                                    pSetGeofenceEngineConfigReq->transactionId))
    {
      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
      retVal = FALSE;
      break;
    }

    /** call gfc api to set the geofence engine config   */
    status = gfc_qmi_set_geofence_engine_config(&gfcSetGeofenceEngineConfigReq, &returnCode);

    /** convert status to response and fill in the response code for
       *  QMI_LOC Gen resp */
    if (TRUE != convertGfcStatusToQmiLocResp(status, returnCode, pResp))
    {
      LOC_MSG_ERROR("locGeofenceSetEngineConfig:set engine config error\n",0,0,0);
      /**clear the entry from client req list */
      (void)clearTxnIdFromClientReq(g_setEngineConfigReqClientList,
                                    locClientHandle,
                                    pSetGeofenceEngineConfigReq->transactionId);
      retVal = FALSE;
      break;
    }
    retVal = TRUE;
  }while(0);

  return retVal;

}


/** locGeofencePurge
 * Purge all the geofences from the system. This
 * function is called when the geofence client deregisters
 * or the service deInitializes for any reasons
 * The assumption is that only one geofence client is ever
 * present in the system.
 *
 */

void locGeofencePurge(void)
{
  gfc_qmi_operation_result_e_type returnCode;
  gfc_qmi_operation_status_e_type  status =
    gfc_qmi_purge_geofence(&returnCode);

  if( GFC_QMI_OPERATION_STATUS_FAILED == status)
  {
    /** Log an error, nothing else can be done */
    LOC_MSG_ERROR("locGeofencePurge: failed return code = %d\n",
                  returnCode,0, 0);
  }

  /** reset the global variable state */
  locGeofenceResetState();

  return;
}



/** locGeofenceHandleBatchBreachReq
 *  Handle the batch breach request sent by the control point.
 *
 *  @param [in] bEnable : Enable / Disable batch breach reports
 * */

void locGeofenceHandleBatchBreachReq(boolean bEnable)
{
  gfc_qmi_handle_batch_breach_req(bEnable);
}


/** locGeofenceSetOperationMode
 *  Handle the operation mode request sent by control point
 *
 *  @param [in] operation mode
 * */

void locGeofenceSetOperationMode(const qmiLocOperationModeEnumT_v02    op_mode)
{
  pdsm_pd_session_operation_e_type pd_operation_mode = PDSM_SESSION_OPERATION_MIN;

  pd_operation_mode = loc_pd_xlate_fix_mode_from_locapi_to_pdapi (op_mode);

  gfc_qmi_handle_operation_mode_req(pd_operation_mode);


}


/** loc_geofence_inject_timezone_info
 *  Handle the inject TimeZone request from the QMI control point by injecting
 *  the TIME_ZONE into GFC_QMI and onto the geofence engine.
 **/
boolean loc_geofence_inject_timezone_info
(
  qmiLocInjectTimeZoneInfoIndMsgT_v02 *pz_InjectTZInfoInd,
  const qmiLocInjectTimeZoneInfoReqMsgT_v02 *const cpz_Req,
  qmiLocGenRespMsgT_v02 *const pz_Resp
)
{
  if ((NULL == cpz_Req) || (NULL == pz_Resp) || (NULL == pz_InjectTZInfoInd))
  {
    /* this null pointer reference shall never happen
       the error code indicates pResp is not filled */
    return FALSE;
  }


  LOC_MSG_HIGH("loc_geofence_inject_timezone_info.", 0, 0, 0);

  gfc_qmi_handle_inject_time_zone(
      cpz_Req->timeUtc / 1000, /* convert ms to sec */
      cpz_Req->timeZone.rawOffset,
      cpz_Req->timeZone.dstOffset);
    
  memset(pz_Resp, 0, sizeof(*pz_Resp));
  pz_Resp->resp.error = QMI_ERR_NONE_V01;
  pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;

  pz_InjectTZInfoInd->status   = eQMI_LOC_SUCCESS_V02;
  return TRUE;
}

#if 0
/** locGeofenceSendTzIndResp
 *  Send Indication for ReqInject TimeZone info from the qmi control point
 *  
 * */
void locGeofenceSendTzIndResp(os_IpcMsgType* p_msg)
{
  locQmiShimIndInfoStructT  shimIndInfo = { 0 }; 
  injectTzIndication        *p_tzInd    = (injectTzIndication*) p_msg->p_Data;
  qmiLocInjectTimeZoneInfoIndMsgT_v02 injectTzInfoInd;
  

  if(p_msg == NULL)
  {
    LOC_MSG_ERROR("locGeofenceSendTzIndResp NULL Ptr!!", 0, 0, 0);
    return;
  }

  if(p_msg->q_MsgId != LM_MIDDLEWARE_MSG_ID_INJECT_TZ_IND)
  {
    LOC_MSG_ERROR("locGeofenceSendTzIndResp NULL Ptr!!", 0, 0, 0);
    return;
  }

  shimIndInfo.p_Msg = &injectTzInfoInd;
  shimIndInfo.q_Id  = QMI_LOC_INJECT_TIME_ZONE_INFO_IND_V02;
  injectTzInfoInd.status = p_tzInd->e_Status;

  shimIndInfo.q_MsgLen = sizeof(injectTzInfoInd);
	
  /** Broadcast to client that injected the time zone info */
  if(FALSE == locQmiShimSendInd(p_tzInd->locClientHandle,
                                &shimIndInfo))
  {
    LOC_MSG_ERROR("locGeofenceSendTzIndResp: Could not send indication\n",
                   0,0,0);
  }
  
}
#endif
