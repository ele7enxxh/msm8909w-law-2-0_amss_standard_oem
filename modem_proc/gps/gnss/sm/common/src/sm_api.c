
/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Session Manager API

GENERAL DESCRIPTION
  This module makes up the Position Determination Session Manager of the subscriber
  station software.

  The Session Manager API file is used by MGP module to interface with external
  world


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/sm_api.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/09/15   rh      Keep sm_UpdateCellIDInfo() API for SMART even with CM CID change 
07/03/15   jv      Added QZSS Support
06/04/15   wtr     Added initializer (of Unc's to high values) for 
                   sm_GnssNavPosition
05/09/14   jv      Galio GNSS Support
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos) 
01/28/13   mj      Update sm_RegisterApiNotifier to include new parm clientId
01/12/13   sk      ECID for LPP API changes
03/14/12   ss      Reset Location Service Changes
02/27/12   rh      Added sm_SendEngineErrorRecoveryReport() API 
05/18/11   rc      Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
02/12/10   vp      Added Sv polynomial function for OEM DRE enablement
06/10/09   gk      Added Get UTC offset from NV
01/12/09   atien   XTRA-T support

09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
04/28/08   gk      On-Demand Changes
08/01/07   rw      Add GPS report time info for XTRA feature
12/01/06   jw      Made changes to support modified struct sm_GpsMeasRptStructType
04/04/06   sk      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */

#include "smdbg.h"      /* Interface to SM debug services */
#include "gps_common.h"
#include "sm_api.h"     /* Internal interface to pdsm.c */
#include "aries_os_api.h"
#include "aries_os_globals.h"
#ifndef ARIES_DLF_CONVERT
   #include "tm_api.h"
#endif /* End - !ARIES_DLF_CONVERT */

#include "sm_util.h"
#include "gnss_common.h"
#include "location_common.h"
#include "sm_nv_efs.h"


/*===========================================================================
FUNCTION sm_send_robust_message

DESCRIPTION
  Function to send an IPC message to SM threads

RETURN VALUE
 ipc_msg_ptr = If IPC message was send successfully

DEPENDENCIES
  None

=============================================================================*/
/* KGC TODO: change return type for sm_send_message. the returned
   os_IpcMsgType is only being used to determine if a message was
   successfully allocated and sent. it is dangerous to access the message
   after it has been queued for another thread.

   Return type may be changed to bool. Also, in the current implementation,
   only action taken on the return is to emit an F3, which is a repeat of the
   one already being done in sm_send_message.

*/
os_IpcMsgType *sm_send_robust_message
(
uint32 msg_id,
uint32 size,
uint32 thread,
const void *data
)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   boolean status;
   MSG_LOW("SM_API: Sending msg=%d,size=%d,thread=%d",msg_id,size,thread);

   /* All messages to SM are dynamic, so just allocate non-static buffer.
   */
   ipc_msg_ptr = os_RobustIpcCreate(size,IPC_ALLOCATION_DYNAMIC,thread);

   if(ipc_msg_ptr != NULL)
   {
      /* Copy out the data message portion.
      */
      if(data != NULL)
      {
         memscpy( ipc_msg_ptr->p_Data, size, data, size);
      }
      ipc_msg_ptr->q_MsgId = msg_id;

      /* Check if IPC is able to send the message to thread.
      */
      status    = os_IpcSend(ipc_msg_ptr, thread);

      if(status == FALSE)
      {
         MSG_ERROR("SM_API: Error in sending msg_id=%d to thread=%d",msg_id,thread,0);
         (void)os_IpcDelete(ipc_msg_ptr);
         ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
         MSG_LOW("SM_API:sending msg_id=%d to thread=%d",msg_id,thread,0);
      } /* end else status */
   } /* end ipc_msg_ptr */

   return ipc_msg_ptr;
} /* End sm_send_message() */

/*===========================================================================
FUNCTION sm_send_message

DESCRIPTION
  Function to send an IPC message to SM threads

RETURN VALUE
 ipc_msg_ptr = If IPC message was send successfully

DEPENDENCIES
  None

=============================================================================*/
/* KGC TODO: change return type for sm_send_message. the returned
   os_IpcMsgType is only being used to determine if a message was
   successfully allocated and sent. it is dangerous to access the message
   after it has been queued for another thread.

   Return type may be changed to bool. Also, in the current implementation,
   only action taken on the return is to emit an F3, which is a repeat of the
   one already being done in sm_send_message.

*/
os_IpcMsgType *sm_send_message
(
uint32 msg_id,
uint32 size,
uint32 thread,
const void *data
)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   boolean status;
   MSG_LOW("SM_API: Sending msg=%d,size=%d,thread=%d",msg_id,size,thread);

   /* All messages to SM are dynamic, so just allocate non-static buffer.
   */
   ipc_msg_ptr = os_IpcCreate(size,IPC_ALLOCATION_DYNAMIC,thread);

   if(ipc_msg_ptr != NULL)
   {
      /* Copy out the data message portion.
      */
      if(data != NULL)
      {
         memscpy( ipc_msg_ptr->p_Data, size, data, size);
      }
      ipc_msg_ptr->q_MsgId = msg_id;

      /* Check if IPC is able to send the message to thread.
      */
      status    = os_IpcSend(ipc_msg_ptr, thread);

      if(status == FALSE)
      {
         MSG_ERROR("SM_API: Error in sending msg_id=%d to thread=%d",msg_id,thread,0);
         (void)os_IpcDelete(ipc_msg_ptr);
         ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
         MSG_LOW("SM_API:sending msg_id=%d to thread=%d",msg_id,thread,0);
      } /* end else status */
   } /* end ipc_msg_ptr */

   return ipc_msg_ptr;
} /* End sm_send_message() */
#ifdef FEATURE_GPSONE_PPM
/*===========================================================================

FUNCTION PDSM_SRCH_MSG

DESCRIPTION
  This function is used to allow AFLT engine to work like 6800. The PPM measurement
  is converted from old format to MGP report format before sending a message to
  SM. Instead of using old Rex signals, this function call LM API which sends
  an IPC to SM.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void pdsm_srch_msg( pdsm_srch_msg_u_type  *msg_ptr )
{
   uint32 msg_id;
   uint32 size;
   os_IpcMsgType   *ipc_msg_ptr  = NULL;

   MSG_HIGH("SM_API: SRCH =>SM, PPM report results...",0,0,0 );

   if( msg_ptr != NULL )
   {
      /* Convert the PPM data first. For now since all the data is in
      ** IS801 format no need to convert....
      */
      //sm_convert_ppm( msg_ptr, &(sm_data->z_RequestData.z_PpmRpt) );

      /* Set the msg id and size an invoke a common SM routine to sen msg.
      */
      msg_id      = (uint32)ARIES_MSG_ID_AFLT_LM_REPORT_PPM_MEAS_IS801;
      size        = sizeof(srch_ppm_rpt_type);
      /*MRAWAL: need to write a function to copy pdsm_srch_ppm_rsp_type to
      sm_PpmRptStructType*/

      /* Send the MSG to SM LM thread
      */
      ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)&msg_ptr->ppm_rsp.srch_rpt);

      if(ipc_msg_ptr != NULL)
      {
         MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
      }
   }
   else
   {
      MSG_ERROR("SM_API: Null pointers,i/p=%d,sm=%d",msg_ptr,ipc_msg_ptr,0);
   } /* End sm_data */

   /* Put the buffer back onto free q, right after IPC msg was sent out.
   */
   if(msg_ptr != NULL)
   {
      (void)q_link( msg_ptr, &msg_ptr->hdr.link );
      q_put( &pdsm_srch_msg_free_q, &msg_ptr->hdr.link );
   }
} /* End pdsm_srch_msg() */
#endif /* FEATURE_GPSONE_PPM */

/**
  @brief   This function sends the GPS Measurements obtained by ME to SM.

  This function sends the Receiver State change event report to
  SM

  @return
  @sideeffects None
  @see
*/
void sm_ReportRcvrStateChange(
                             uint32 q_GpsRtcMs,
                             /**< GNSS RTC msec at which this report was generated.  */
                             gps_RcvrStateEnumType e_CurRcvrState
                             /**< new receiver state */
                             )
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;
   sm_RcvrStateChangeStructType  state;

   MSG_LOW("SM_API: MGP=>SM state change RTC=%d, state=%d",q_GpsRtcMs,e_CurRcvrState,0);

   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_GENERAL_STATUS;
   size        = sizeof(sm_RcvrStateChangeStructType);

   /* Init the state and RTC value in local variable for IPC send
   */
   state.e_CurrentRcvrState = e_CurRcvrState;
   state.q_GpsRtcMs = q_GpsRtcMs;
   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)&state);

   if(ipc_msg_ptr != NULL)
   {
      MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }
} /* End sm_ReportRcvrStateChange() */

/**
 * This function sends the GNSS Measurements obtained by ME to
 * SM. PS Measurements are sent out periodically for NMEA
 * sentence generation and for diagnostic reasons (periodicity
 * is set by SM). They are also sent when measurement processing
 * is complete in 1x MS-Assisted mode of operation. The
 * measurement status report indicates the reason for sending
 * the measurements.
 * @param e_MeasRptReason
 * @param meas_ptr
 */
void sm_ReportGnssMeasInfo
(
gnss_MeasRptReasonEnumType e_MeasRptReason,
const gnss_MeasBlkStructType *meas_ptr
)
{
   sm_GpsMeasRptStructType z_SmMeas, *sm_meas_ptr = &z_SmMeas;
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_LOW("SM_API: Meas update for SM",0,0,0);

   /* Copy the GPS Meas Blk into SM meas structure */
   sm_meas_ptr->e_MeasRptReason = e_MeasRptReason;

   sm_meas_ptr->z_MeasBlk = *meas_ptr;

   msg_id = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS;

   size = sizeof(sm_GpsMeasRptStructType);

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)sm_meas_ptr);

   if(ipc_msg_ptr != NULL)
   {
      MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }
   return;
} /* end sm_ReportGnssMeasInfo() */

/*=============================================================================

FUNCTION
  sm_ReportSVPolynomialGNSS

DESCRIPTION
  This function sends the GNSS SV Poly report by PE to SM.
  GPS FIX/NO-FIX reports are sent periodically once MGP is turned ON.


DEPENDENCIES
  None.

PARAMETERS
  p_GnssSvPolyRpt - Pointer to the structure containing the GPS SV Poly data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportSVPolynomialGNSS(const gnss_SvPoly *p_GnssSvPolyRpt)
/**< Pointer to the structure containing the GNSS Fix data */
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_LOW("SM_API: SV Poly report for SM",0,0,0);

   /* send sv poly to LM */
   msg_id      = (uint32)MSG_ID_MGP_LM_REPORT_SV_POLY;
   size        = sizeof(gnss_SvPoly);

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_GnssSvPolyRpt);

   if(ipc_msg_ptr != NULL)
   {
      MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }

} /* end sm_ ReportSVPolynomialGPS */

/**
  @brief  This function sends the GPS Fix report calculated by
          PE to SM. GPS FIX/NO-FIX reports are sent periodically
          once MGP is turned ON

  @return
  @sideeffects None
  @see

*/
void sm_ReportGnssFixInfo(const sm_GnssFixRptStructType *p_GnssFixRpt)
/**< Pointer to the structure containing the GPS Fix data */
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_LOW("SM_API: Fix report for SM",0,0,0);
   if (NULL == p_GnssFixRpt)
   {
     MSG_ERROR("SM_API: Null pointer access in sm_ReportGnssFixInfo()",0,0,0);
     return;
   }


   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_FIX;
   size        = sizeof(*p_GnssFixRpt); /* sizeof(sm_GnssFixRptStructType) */

   /* Send the MSG to SM LM thread
   */
   /* If fix is not valid or it is full GNSS Fix use legacy IPC create API */
   if ((TRUE == p_GnssFixRpt->z_NavPos.z_PosFlags.b_NHzGnssUsedFixRpt) || 
       (TRUE == p_GnssFixRpt->z_NavPos.z_PosFlags.b_PVAPropagationUsedFixRpt) )
   {     
     ipc_msg_ptr = sm_send_robust_message(msg_id, size, (uint32)THREAD_ID_SM_LM, 
       (void *)p_GnssFixRpt);
   }
   else
   {
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_GnssFixRpt);
   }

   if(ipc_msg_ptr != NULL)
   {
      MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }

} /* end sm_ReportGnssFixInfo */

/**
  @brief  This function sends the GNSS position estimate
          maintained by PE to SM. PE responds using this API
          when SM requests position estimate using
          mgp_ReqPosEstimate.

* @return      void

  @sideeffects None
  @see sm_ReportGnssFixInfo

*/
void sm_ReportGnssPosEstimate(const sm_GnssFixRptStructType *p_GnssFixRpt)
/**< Pointer to the structure containing the GPS Fix data */
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_LOW("SM_API: Fix Estimate report for SM",0,0,0);

   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE;
   size        = sizeof(*p_GnssFixRpt); /* sizeof(sm_GnssFixRptStructType) */

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_GnssFixRpt);

   if(ipc_msg_ptr != NULL)
   {
      MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }
} /* end sm_ReportGnssPosEstimate */

/*=============================================================================

FUNCTION
  sm_ReportUtcInfo

DESCRIPTION
  This function sends the UTC data decoded by PE off the air to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_UtcInfo - Pointer to the structure containing the UTC info

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportUtcInfo(const sm_UtcInfoRptStructType *p_UtcInfo)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_HIGH("SM_API: UTC data update...",0,0,0);

   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_UTC_INFO;
   size        = sizeof(sm_UtcInfoRptStructType);

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_UtcInfo);

   if(ipc_msg_ptr != NULL)
   {
      MSG_HIGH("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }
}

/**
* @brief  PE indicates to SM the status of current assistance
*         data and wish-list using this API

  @return
  @sideeffects None
  @see

*/
void sm_ReportGnssAssistDataStatus( const sm_GnssAssistDataStatusStructType *p_AssistDataInfo)
{
   sm_InternalAssistDataStatusStructType internalAssistData;
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_LOW("SM_API: Assitance data status update for SM...",0,0,0);

   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS;
   size        = sizeof(sm_InternalAssistDataStatusStructType);

   /* this translation is temporary and will eventually go away as the underlying SM code
      is changed */
   memset(&internalAssistData, 0, sizeof(internalAssistData));
   if(sm_translate_gnss_assist_data_to_internal_assist_data(&internalAssistData, p_AssistDataInfo))
   {
      /* Send the MSG to SM LM thread */
      ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)&internalAssistData);

      if(ipc_msg_ptr != NULL)
      {
         MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
      }
   }
   else
   {
      MSG_ERROR("SM_API: translation from gnss assist data to internal assist data failed \n",0,0,0);
   }

} /* end sm_ReportGnssAssistDataStatus */

/*=============================================================================

FUNCTION
  sm_ReportEvent

DESCRIPTION
  This function sends an event notification from PE to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportEvent - Report event parameters.
  Contains the following fields:
     e_EventType - type of event
     q_EventData - data related to the event
     q_EventTime - time stamp related to event(GPS RTC)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportEvent(const sm_ReportEventStructType* p_ReportEvent)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_LOW("SM_API: Report event to SM...",0,0,0);

   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_EVENT;
   size        = sizeof(sm_ReportEventStructType);

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_ReportEvent);

   if(ipc_msg_ptr != NULL)
   {
      MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }

}

/*=============================================================================

FUNCTION
  sm_ReportRfInfo

DESCRIPTION
  This function sends RF Info data from PGI to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportRfInfo - Pointer to the structure containing RF Info data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportRfInfo(const sm_ReportRfInfoStructType * p_ReportRfInfo)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_LOW("SM_API: Report RF Info to SM...",0,0,0);

   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_RF_INFO;
   size        = sizeof(sm_ReportRfInfoStructType);

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_ReportRfInfo);

   if(ipc_msg_ptr != NULL)
   {
      MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }

}

/*=============================================================================

FUNCTION
  sm_StoreFtcalTTData

DESCRIPTION
  This function sends TT data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  cgps_FtcalTTStruct - TT data for Fast TCal

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_StoreFtcalTTData (cgps_FtcalTTStruct* p_TTData)
{
  tm_api_store_ftcal_tt_data(p_TTData);
}

/*=============================================================================

FUNCTION
  sm_StoreFtcalPpmData

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  cgps_FtcalPpmStruct - PPM data for Fast TCal

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_StoreFtcalPpmData (cgps_FtcalPpmStruct* p_PpmData)
{
  tm_api_store_ftcal_ppm_data(p_PpmData);
}

/*=============================================================================

FUNCTION
  sm_ReportWBIQInfo

DESCRIPTION
  This function sends Wide Band IQ Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportWBIQInfo - Pointer to the structure containing WBIQ params

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportWBIQInfo (const sm_ReportWBIQInfoStructType * p_ReportWBIQInfo)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_LOW("SM_API: Report WBIQ Info to SM...",0,0,0);

  msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS;
  size        = sizeof(sm_ReportWBIQInfoStructType);

  /* Send the MSG to SM LM thread
  */
  ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_ReportWBIQInfo);

  if (ipc_msg_ptr != NULL)
  {
    MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }

}


/*=============================================================================

FUNCTION
  sm_ReportNBIQInfo

DESCRIPTION
  This function sends Narrow Band IQ Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportNBIQInfo - Pointer to the structure containing NBIQ raw IQ data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportNBIQInfo (const sm_ReportNBIQInfoStructType * p_ReportNBIQInfo)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_LOW("SM_API: Report NBIQ Info to SM...",0,0,0);

  msg_id      = (uint32)SM_MSG_ID_MGP_LM_REPORT_NBIQ_PARAMS;
  size        = sizeof(sm_ReportNBIQInfoStructType);

  /* Send the MSG to SM LM thread
  */
  ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_ReportNBIQInfo);

  if (ipc_msg_ptr != NULL)
  {
    MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }

}

/*=============================================================================

FUNCTION
  sm_ReportGNSSRfStatusInfo

DESCRIPTION
  This function sends GNSS RF status Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportGNSSRfInfo - Pointer to the structure containing GNSS RF status
  information 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGNSSRfStatusInfo (const sm_ReportGNSSRfStatusInfoStructType * p_ReportGNSSRfInfo)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_LOW("SM_API: Report GNSS RF Status Info to SM...",0,0,0);

  msg_id      = (uint32)SM_MSG_ID_MGP_LM_REPORT_GNSS_RF_STATUS_INFO;
  size        = sizeof(sm_ReportGNSSRfStatusInfoStructType);

  /* Send the MSG to SM LM thread
  */
  ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_ReportGNSSRfInfo);

  if (ipc_msg_ptr != NULL)
  {
    MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }

}

/*=============================================================================

FUNCTION
  sm_RequestSAData

DESCRIPTION
  This function allows MGP to request SA data from the network

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_RequestSAData(void)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_LOW("SM_API: Request SA Data to SM for SV Mask ...",0,0,0);

   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REQ_SA_DATA;
   size        = 0;

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)NULL);

   if(ipc_msg_ptr != NULL)
   {
      MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }
}

#ifdef FEATURE_CGPS_UMTS_CELLDB
/*=============================================================================

FUNCTION
  sm_UpdateCellIDInfo

DESCRIPTION
  This function sends the latest Cell ID information to SM. This information
  is maintained in MGP and is updated to SM whenever there is a cell change
  or when new Cell ID is received from RR/RRC.
 
  Latest Cell ID changes will come from Call Manager instead of MGP, but this
  API can be used to inject Cell ID change to SM (like in SMART)
 
DEPENDENCIES
  None.

PARAMETERS
  p_CellInfoCached - Pointer to the Cached Cell ID information.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_UpdateCellIDInfo( const cgps_ActiveRATEnumType e_TriggerRAT, const cgps_CellInfoCachedStruct * const p_CellInfoCached )
{
  tm_api_update_cell_info(e_TriggerRAT, p_CellInfoCached);
}
#endif /* FEATURE_CGPS_UMTS_CELLDB */

//TBD
// Aries41 merge. Following 2 functions needs to be removed once mc_msg is integrated.
void tm_api_update_pos_punc_tunc ( const sm_OnDemandPosPuncTuncStruct* const p_PosInfo )
{
}
/*=============================================================================

FUNCTION
  sm_UpdatePosPuncTunc

DESCRIPTION
  This function sends the latest Position and its associated PUNC/TUNC to SM for
  On-demand positioning.

DEPENDENCIES
  None.

PARAMETERS
  p_PosPuncTunc - Pointer to the data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_UpdatePosPuncTunc( const sm_OnDemandPosPuncTuncStruct* const p_PosPuncTunc )
{
   tm_api_update_pos_punc_tunc(p_PosPuncTunc);
}

/*=============================================================================

FUNCTION
  sm_UpdateGpsState

DESCRIPTION
  This function sends the latest Position and its associated PUNC/TUNC to SM for
  On-demand positioning and diag pos or GPS status report.

DEPENDENCIES
  None.

PARAMETERS
  p_PosInfo - Pointer to the best available position fix info.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_UpdateGpsState( const sm_PositionReportStructType* const p_PosInfo )
{
  tm_api_update_gps_state_info(p_PosInfo);
}

#ifdef FEATURE_CGPS_XTRA_T
/*=============================================================================

FUNCTION
  sm_ReportCellData

DESCRIPTION
  This function sends cell data from MGP to SM.

DEPENDENCIES
  None.

PARAMETERS
  sm_ReportCellDataStructType - Pointer to the structure containing cell data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportCellData(const sm_ReportCellDataStructType* const p_ReportCellData)
{
   MSG_ERROR("new XTRA-T implementation should not be here!", 0,0,0);
}
#endif /* FEATURE_CGPS_XTRA_T */


/*=============================================================================

FUNCTION
  sm_GetNvGpsUtcOffset

DESCRIPTION
  This function gets UTC offset configured in the NV.

DEPENDENCIES
  None.
PARAMETERS
  p_UtcOffset - Pointer to the UTC offset value
RETURN VALUE
  None.
SIDE EFFECTS
  None.

=============================================================================*/
void  sm_GetNvGpsUtcOffset(uint8 *p_UtcOffset)
{
  tm_api_report_nv_utc_offset(p_UtcOffset);
}

/**
  @brief  Delivers estimate of current time to SM. PE calls this
          API in response to a request from SM via
          mgp_ReqGnssTime (to be defined by MGP).

  @return
  @sideeffects None
  @see

*/
void sm_ReportGnssTime(const sm_ReportGnssTimeStructType *p_ReportGnssTime)/**< gnss time struct */
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;
   sm_ReportGpsTimeStructType gpsTime = {0};

   /* we translate to older gps struct so that internal code doesn't have to change just yet.
   Also all layers below SM API use only GPS time*/
   if(sm_translate_gnss_time_to_gps_time(&gpsTime, p_ReportGnssTime))
   {
      MSG_LOW("sm_ReportGnssInfo: Report GPS time to SM...",0,0,0);

      msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME;
      size        = sizeof(gpsTime); /* sizeof(sm_ReportGpsTimeStructType) */

      /* Send the MSG to SM LM thread
      */
      ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)&gpsTime);

      if(ipc_msg_ptr != NULL)
      {
         MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
      }
   }
   else
   {
      MSG_HIGH("SM_API: failed when translating gnss time to internal gps time ",0,0,0);
   }
} /* end sm_ReportGnssTime */

/*=============================================================================
FUNCTION
  sm_ReportPrescDwellResult

DESCRIPTION
  This function gets called when a presc dwell log is available.

DEPENDENCIES
  None.

PARAMETERS
  p_PrescDwellResult - Pointer to the structure containing presc dwell result

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportPrescDwellResult( const sm_PrescDwellLogType *p_PrescDwellResult )
{
  /* Display a few values for quick F3 confirmation */
  MSG_LOW("SM_API: Presc Dwell result Flags %u CNo %u PostD %u",
            p_PrescDwellResult->z_PrescDwellResult.u_Flags,
            p_PrescDwellResult->z_PrescDwellResult.w_CnoDb,
            p_PrescDwellResult->z_PrescDwellResult.w_PostDetN);
}

/*=============================================================================

FUNCTION
  sm_ReportDemodSoftDecisions

DESCRIPTION
  This function gets called when demod soft-decision log is available.

DEPENDENCIES
  None.

PARAMETERS
  p_DemodResult - Pointer to the structure containing demod soft decision data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDemodSoftDecisions( const log_gps_demod_soft_decisions_type *p_DemodResult )
{
 /* Display a few values for quick F3 confirmation */
  MSG_HIGH("SM_API: Demod soft Sv %u DemodId %u BitId %u",
            p_DemodResult->w_Sv, p_DemodResult->w_DemodId, p_DemodResult->q_BitId);
}


/*=============================================================================

FUNCTION
  sm_EnableSensorLPM

DESCRIPTION
  This function turns on sensor low power mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_EnableSensorLPM( void )
{
  //tm_api_subscribe_sensor_data( (uint32)TM_SENSOR_DATA_MOTION_LOW_POWER );
}

/*=============================================================================

FUNCTION
  sm_DisableSensorLPM

DESCRIPTION
  This function turns off sensor low power mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_DisableSensorLPM( void )
{
 // tm_api_unsubscribe_sensor_data( (uint32)TM_SENSOR_DATA_MOTION_LOW_POWER );
}

/*=============================================================================

FUNCTION
  sm_EnableSensorFPM

DESCRIPTION
  This function turns on sensor low power mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_EnableSensorFPM( void )
{
  uint32 data = (uint32)( TM_SENSOR_DATA_VEL_1HZ_1SEC |
                          TM_SENSOR_DATA_JERK_1HZ_1SEC |
                          TM_SENSOR_DATA_MOTION_1HZ |
                          TM_SENSOR_DATA_VEL_1HZ_100MS |
                          TM_SENSOR_DATA_JERK_1HZ_100MS );
  (void)data;
 // tm_api_subscribe_sensor_data( data );
}


/*=============================================================================

FUNCTION
  sm_DisableSensorFPM

DESCRIPTION
  This function turns off sensor low power mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_DisableSensorFPM( void )
{
  uint32 data = (uint32)( TM_SENSOR_DATA_VEL_1HZ_1SEC |
                          TM_SENSOR_DATA_JERK_1HZ_1SEC |
                          TM_SENSOR_DATA_MOTION_1HZ |
                          TM_SENSOR_DATA_VEL_1HZ_100MS |
                          TM_SENSOR_DATA_JERK_1HZ_100MS );
  (void)data;
  //tm_api_unsubscribe_sensor_data( data );
}


/**
* @brief  Indicate to SM that it should request external SPI information

  @return
  @sideeffects None
  @see

*/
void sm_XspiStartInjection(void)
{
   MSG_LOW("SM_API: requesting XSPI injection", 0, 0, 0);
   tm_api_xspi_start_injection();
}

/**
* @brief  Indicate to SM that it should close the
          SPI information injection stream

  @return
  @sideeffects None
  @see

*/
void sm_XspiStopInjection(void)
{
   MSG_LOW("SM_API: requesting XSPI injection termination", 0, 0, 0);
   tm_api_xspi_stop_injection();
}

/*=============================================================================

FUNCTION
  sm_ReportDrSyncPulseDescriptor

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDrSyncPulseDescriptor(gnss_DRSyncPulseDescType* pDrSyncPulseDesc)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_HIGH("sm_ReportDrSyncPulseDescriptor()",0,0,0);

  msg_id = (uint32)SM_MSG_ID_MGP_LM_DRSYNC_PULSE_DESC;
  size   = sizeof(gnss_DRSyncPulseDescType);

  /* Send the MSG to SM LM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size,
    (uint32)THREAD_ID_SM_LM, (void *)pDrSyncPulseDesc);

  if(ipc_msg_ptr != NULL)
  {
    MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }
}

/*=============================================================================

FUNCTION
  sm_ReportDrSyncConfig

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDrSyncConfig(gnss_DRSyncConfigType* pDrSyncConfig)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_HIGH("sm_ReportDrSyncConfig()",0,0,0);

  msg_id = (uint32)SM_MSG_ID_MGP_LM_DRSYNC_CONFIG;
  size   = sizeof(gnss_DRSyncConfigType);

  /* Send the MSG to SM LM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size,
    (uint32)THREAD_ID_SM_LM, (void *)pDrSyncConfig);

  if(ipc_msg_ptr != NULL)
  {
  }
}

/*=============================================================================

FUNCTION
  sm_ReportDebugNMEA

DESCRIPTION
  Accepts formatted proprietary NMEA string for debug purpose.
  NMEA string is sent as IPC to the LM task by this API

DEPENDENCIES
  None.

PARAMETERS
  sm_DebugNMEA

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDebugNMEA(sm_DebugNMEA* pDebugNMEA)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_LOW("sm_ReportDebugNMEA()",0,0,0);

  msg_id = (uint32)SM_MSG_ID_MGP_LM_DEBUG_NMEA;
  size   = sizeof(sm_DebugNMEA);

  /* Send the MSG to SM LM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size,
    (uint32)THREAD_ID_SM_LM, (void *)pDebugNMEA);

  if(ipc_msg_ptr != NULL)
  {
    MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }
}

/*=============================================================================

FUNCTION
  sm_RegisterApiNotifierCb

DESCRIPTION
  Called by potential clients to register their callbacks. The callbacks are
  used to notify the clients that the API/Engine is now ready to accept clients

  If the API/Engine is ready to accept clients, the callback will be called
  inline.  The caller should be ready to handle this and queue an IPC to itself.

DEPENDENCIES
  None.

PARAMETERS
  pNotifierCb - The callback of the client to be registered
  clientThreadId - ThreadId used to index the cb into an array

RETURN VALUE
  TRUE:  registration has succeeded, or if Engine is ready the callback is
         called immediately;
  FALSE:  registration failed

SIDE EFFECTS
  None.

=============================================================================*/
boolean sm_RegisterApiNotifier(sm_ApiInitNotifierCb pNotifierCb, os_ThreadIdEnumType clientThreadId)
{
  return tm_api_reg_task_init_notifier(pNotifierCb, clientThreadId);
}

/*=============================================================================

FUNCTION
  sm_ReportXtraInjDone

DESCRIPTION
  Indication from CD that xtra injection completed successfully. 

DEPENDENCIES
  None.

PARAMETERS
   None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportXtraInjDone( void )
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_LOW("sm_ReportXtraInjDone",0,0,0); 

  msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_XTRA_INJ_DONE;
  size   = 0;

  /* Send the MSG to SM LM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size, 
    (uint32)THREAD_ID_SM_LM, NULL);

  if(ipc_msg_ptr != NULL)
  {
    MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }
}

/*=============================================================================

FUNCTION
  sm_ReportResetLocationServiceDone

DESCRIPTION
  Indication from MGP that location service reset id completed.

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportResetLocationServiceDone(pdsm_pd_ext_reset_location_service_status_e_type status)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_LOW("sm_ReportResetLocationServiceDone",0,0,0); 

  msg_id = (uint32)SM_MSG_ID_MGP_RESET_LOCATION_SERVICE_DONE;
  size   = sizeof(pdsm_pd_ext_reset_location_service_status_e_type);

  /* Send the MSG to SM LM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size, 
    (uint32)THREAD_ID_SM_LM, (void*)&status);

  if(ipc_msg_ptr != NULL)
  {
    MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }
}

/*=============================================================================

FUNCTION
  sm_SendEngineErrorRecoveryReport

DESCRIPTION
  ME will call this API to send engine error recovery report to SM so it'll be
  forwarded to QMI

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_SendEngineErrorRecoveryReport( gnss_engine_err_recovery_rpt *pErrReport )
{
  tm_api_send_engine_error_recovery_report(pErrReport);
}


/*=============================================================================

FUNCTION
  sm_ReportGpsEphemeris

DESCRIPTION
  This function sends a GPS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Prn    - PRN of ephemeride being returned
  pz_GpsEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGpsEphemeris( const uint16 w_Prn, const gps_EphStructType *pz_GpsEph )
{
	os_IpcMsgType   *ipc_msg_ptr  = NULL;
	uint32          msg_id        = 0;
	uint32          size          = 0;
	sm_GpsEphemeris z_GpsEph;

	MSG_LOW("sm_ReportGpsEphemeris",0,0,0); 

	if( NULL == pz_GpsEph)
	{
		MSG_LOW("sm_ReportGpsEphemeris - Invalid Parameter",0,0,0); 
        return;
	}

	z_GpsEph.w_Prn = w_Prn;
	z_GpsEph.z_GpsEphemeris = *pz_GpsEph;

	msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_GPS_EPHEMERIS;
	size   = sizeof(sm_GpsEphemeris);

	/* Send the MSG to SM LM thread */
	ipc_msg_ptr = sm_send_message(msg_id, size, 
	  (uint32)THREAD_ID_SM_LM, (void *)&z_GpsEph);

	if(ipc_msg_ptr != NULL)
	{
	  MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
	}
}

/*=============================================================================

FUNCTION
  sm_ReportQzssEphemeris

DESCRIPTION
  This function sends a QZSS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  u_Prn    - PRN of ephemeride being returned
  pz_QzssEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportQzssEphemeris( const uint8 u_Prn, const gps_EphStructType *pz_QzssEph )
{
	os_IpcMsgType   *ipc_msg_ptr  = NULL;
	uint32          msg_id        = 0;
	uint32          size          = 0;
	sm_GpsEphemeris z_GpsEph; /*QZSS uses same format as in GPS eph*/

    if(NULL == pz_QzssEph)
    {
        return;
    }

	z_GpsEph.w_Prn = u_Prn;
	z_GpsEph.z_GpsEphemeris = *pz_QzssEph;

	MSG_LOW("sm_ReportQzssEphemeris",0,0,0); 

	msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_QZSS_EPHEMERIS;
	size   = sizeof(sm_GpsEphemeris);

	/* Send the MSG to SM LM thread */
	ipc_msg_ptr = sm_send_message(msg_id, size, 
	  (uint32)THREAD_ID_SM_LM, (void *)&z_GpsEph);

	if(ipc_msg_ptr != NULL)
	{
	  MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
	}
}


/*=============================================================================

FUNCTION
  sm_ReportGloEphemeris

DESCRIPTION
  This function sends a GLO ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_SlotId  - Slot ID of ephemeride being returned
  pz_GloEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGloEphemeris( const uint16 w_SlotId, const glo_EphStructType *pz_GloEph )
{
	os_IpcMsgType   *ipc_msg_ptr  = NULL;
	uint32          msg_id        = 0;
	uint32          size          = 0;
	sm_GloEphemeris z_GloEph;

	if( NULL == pz_GloEph)
	{
		MSG_LOW("sm_ReportGloEphemeris - Invalid Parameter",0,0,0); 
        return;
	}

	z_GloEph.w_SlotId = w_SlotId;
	z_GloEph.z_GloEphemeris = *pz_GloEph;

	MSG_LOW("sm_ReportGloEphemeris",0,0,0); 

	msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_GLO_EPHEMERIS;
	size   = sizeof(sm_GloEphemeris);

	/* Send the MSG to SM LM thread */
	ipc_msg_ptr = sm_send_message(msg_id, size, 
	  (uint32)THREAD_ID_SM_LM, (void *)&z_GloEph);

	if(ipc_msg_ptr != NULL)
	{
	  MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
	}
}


/*=============================================================================

FUNCTION
  sm_DpoStatusReport

DESCRIPTION
  This function sends a GLO ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERS
  Information about DPO disengagement from MGP to SM . 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_DpoStatusReport( sm_DpoStatusReportStructType *p_DpoStatusReport )
{
  // Send the DPO status report to TM using IPC . 
  tm_api_send_dpo_status_report(p_DpoStatusReport);
}

/*
******************************************************************************
* sm_SvSframePut
*
* Function description:
*
*    This function is called by PE to provide a decoded subframe to PE.
*
* Parameters:
*
*  p_SvSframe -  Pointer to SV subframe structure
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_SvSubframePut( const gnss_SvSubframeStruct *p_SvSubframe )
{
	os_IpcMsgType   *ipc_msg_ptr  = NULL;
	uint32          msg_id        = 0;
	uint32          size          = 0;

	MSG_LOW("sm_SvSubframePut",0,0,0); 

	msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_GPS_SUBFRAME;
	size   = sizeof(gnss_SvSubframeStruct);

	/* Send the MSG to SM LM thread */
	ipc_msg_ptr = sm_send_message(msg_id, size, 
	  (uint32)THREAD_ID_SM_LM, (void *)p_SvSubframe);

	if(ipc_msg_ptr != NULL)
	{
	  MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
	}
}

/*
******************************************************************************
* sm_GloStringPut
*
* Function description:
*
*  This function is called by PE to provide a decoded GLONASS string to PE.
*
* Parameters:
*
*  pz_Str -  input, Pointer to a GLONASS string
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_GloStringPut( const gnss_GloStringStruct* pz_Str )
{
	os_IpcMsgType   *ipc_msg_ptr  = NULL;
	uint32          msg_id        = 0;
	uint32          size          = 0;

	MSG_LOW("sm_GloStringPut",0,0,0); 

	msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_GLO_STRING;
	size   = sizeof(gnss_GloStringStruct);

	/* Send the MSG to SM LM thread */
	ipc_msg_ptr = sm_send_message(msg_id, size, 
	  (uint32)THREAD_ID_SM_LM, (void *)pz_Str);

	if(ipc_msg_ptr != NULL)
	{
	  MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
	}
}


/*
******************************************************************************
* sm_SvGalPagePut
*
* Function description:
*
*    This function is called by PE to provide a decoded Galileo Page to PE.
*
* Parameters:
*
*  p_SvSframe -  Pointer to SV GAL Page structure
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_SvGalPagePut( const gnss_GalPageStruct *p_SvGalPage)
{
	os_IpcMsgType   *ipc_msg_ptr  = NULL;
	uint32          msg_id        = 0;
	uint32          size          = 0;

    if( NULL != p_SvGalPage)
    {
	    MSG_LOW("sm_SvGalPagePut",0,0,0);
        msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_GAL_PAGE;
	    size   = sizeof(gnss_GalPageStruct);

	    /* Send the MSG to SM LM thread */
	    ipc_msg_ptr = sm_send_message(msg_id, size, 
	                                 (uint32)THREAD_ID_SM_LM,
                                     (void *)p_SvGalPage);

	    if(ipc_msg_ptr != NULL)
	    {
	        MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
	    }
        else
        {
            MSG_LOW("SM_API: IPC msg NOT send to SM, id = %d",(uint32)SM_MSG_ID_MGP_LM_REPORT_GAL_PAGE,0,0);
        }
    }
    else
    {
        MSG_LOW("sm_SvGalPagePut - Invalid Parameter",0,0,0);
    }
}


/*===========================================================================
  sm_loc_lte_ecid_recv_measurements

  Description:

     This function is used by LOC_LTE_ECID module to send the ECID measurements
 Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_ecid_recv_measurements(gnss_lpp_ecid_meas_info_type *p_ecid_measurement_info)
{

  return (tm_loc_lte_ecid_recv_measurements(p_ecid_measurement_info));
}

/*=============================================================================

FUNCTION
  sm_ReportBdsEphemeris

DESCRIPTION
  This function sends a BDS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Sv      - SV ID of ephemeride being returned
  pz_BdsEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportBdsEphemeris( const uint16 w_Sv, const bds_EphStructType *pz_BdsEph )
{
	os_IpcMsgType   *ipc_msg_ptr  = NULL;
	uint32          msg_id        = 0;
	uint32          size          = 0;
	sm_BdsEphemeris z_BdsEph;
    
	if( NULL == pz_BdsEph)
	{
		MSG_LOW("sm_ReportGalEphemeris - Invalid Parameter",0,0,0); 
        return;
	}

	z_BdsEph.w_Prn = w_Sv;
	z_BdsEph.z_BdsEphemeris = *pz_BdsEph;

	MSG_LOW("sm_ReportBdsEphemeris",0,0,0); 

	msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_BDS_EPHEMERIS;
	size   = sizeof(sm_BdsEphemeris);

	/* Send the MSG to SM LM thread */
	ipc_msg_ptr = sm_send_message(msg_id, size, 
	  (uint32)THREAD_ID_SM_LM, (void *)&z_BdsEph);

	if(ipc_msg_ptr != NULL)
	{
	  MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
	}
}

/*=============================================================================

FUNCTION
  sm_ReportGalEphemeris

DESCRIPTION
  This function sends a GAL ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Sv      - SV ID of ephemeride being returned
  pz_GalEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGalEphemeris( const uint16 w_Sv, const gal_EphStructType *pz_GalEph )
{
	os_IpcMsgType   *ipc_msg_ptr  = NULL;
	uint32          msg_id        = 0;
	uint32          size          = 0;
	sm_GalEphemeris z_GalEph;

    if( NULL == pz_GalEph)
	{
		MSG_LOW("sm_ReportGalEphemeris - Invalid Parameter",0,0,0); 
        return;
	}

	z_GalEph.w_Prn = w_Sv;
	z_GalEph.z_GalEphemeris = *pz_GalEph;

	MSG_LOW("sm_ReportGalEphemeris",0,0,0); 

	msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_GAL_EPHEMERIS;
	size   = sizeof(sm_GalEphemeris);

	/* Send the MSG to SM LM thread */
	ipc_msg_ptr = sm_send_message(msg_id, size, 
	  (uint32)THREAD_ID_SM_LM, (void *)&z_GalEph);

	if(ipc_msg_ptr != NULL)
	{
	  MSG_LOW("SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
	}
}

/*===========================================================================
  sm_ReportWifiFixResponse

  Description:

     This function is used by WiLE to provide Wifi fix response to SM
 Parameters:
   sm_GnssFixRptStructType: Pointer to Position Report
                                            Note that this may be non Wifi position 
                                            if there was a better
                                            position available in ALE.
    wifi_ResultFlagsType: Wifi Results Flag
          The result flag indicates any error during Wifi pos fix calculation
          like no modem cache, no measurements, etc. It also indicates 
          whether a better non Wifi fix was sent instead. 
    sm_WifiPosReqSrcType: Wifi Position fix source (originiator)
  Return value: 
   VOID
=============================================================================*/
void sm_ReportWifiFixResponse(const sm_GnssFixRptStructType* pz_PosFixRpt,
                              wl_ResultFlagsType z_wifi_result,
                              wl_PosReqSrcType e_wifi_pos_src)
{
  tm_handle_wifi_fix_response(pz_PosFixRpt, z_wifi_result, e_wifi_pos_src);
}

/*===========================================================================
  sm_InitGnssNavPosition

  Description:

     Initalizes a sm_GnssNavPosition safely

 Parameters:

   pz_Pos:  pointer to the struct to initialize

  Return value: 
    boolean: true if successfully initialized

=============================================================================*/
boolean sm_InitGnssNavPosition(sm_GnssNavPosition * pz_Pos)
{
  if (NULL == pz_Pos)
  {
    return FALSE;
  }

  // zero most things - including the valid flags
  memset(pz_Pos, 0, sizeof(*pz_Pos));

  // but set error estiamtes, aka uncertainties, high

  // position 
  pz_Pos->f_ErrorEllipse[1] = LOC_UNKNOWN_UNC; 
  pz_Pos->f_ErrorEllipse[2] = LOC_UNKNOWN_UNC;
  pz_Pos->f_Punc3dMeters = LOC_UNKNOWN_UNC;  
  pz_Pos->f_PuncLatLonMeters[0] = LOC_UNKNOWN_UNC;
  pz_Pos->f_PuncLatLonMeters[1] = LOC_UNKNOWN_UNC;

  // altitude
  pz_Pos->f_FilteredAltUnc = LOC_ALT_MAX_UNC_METERS;
  pz_Pos->f_PuncVertMeters = LOC_ALT_MAX_UNC_METERS;

  // speed
  pz_Pos->f_Vunc3dMps = LOC_MAX_SIGMA_MPS;
  pz_Pos->f_VuncEastNorthMps[0] = LOC_MAX_SIGMA_MPS;
  pz_Pos->f_VuncEastNorthMps[1] = LOC_MAX_SIGMA_MPS;
  pz_Pos->f_VuncVertMps = LOC_MAX_SIGMA_MPS;

  // heading
  pz_Pos->f_GnssHeadingUncRad = (FLT)C_PI;
  pz_Pos->f_HeadingUncRad     = (FLT)C_PI;

  // clock
  pz_Pos->f_ClockBiasUncMs       = (FLT)C_MAX_CLK_TIME_UNC;

  pz_Pos->f_ClockDriftRateUncMps = (FLT)C_MAX_CLK_FREQ_UNC;

  // times
  pz_Pos->w_GpsWeek = C_GPS_WEEK_UNKNOWN;
  pz_Pos->u_GloNumFourYear = C_GLO_FOUR_YEAR_UNKNOWN;
  pz_Pos->w_GloNumDaysInFourYear = C_GLO_DAY_UNKNOWN;

  return TRUE;
};

#ifdef FEATURE_LOCTECH_NHZ
/*===========================================================================
  sm_NhzConfigPut

  Description:

     Get the NHz config from SM

 Parameters:

   pq_NhzConfig:  pointer to the config

  Return value: 
    None

=============================================================================*/
void sm_NhzConfigPut(uint32 * pq_NhzConfig)
{
  if(NULL == pq_NhzConfig)
  {
    MSG_ERROR("Null pointer access in sm_NhzConfigPut()", 0, 0, 0);
    return;
  }

  if (!sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_NHZ_CONFIG, sizeof(uint32), pq_NhzConfig) )
  {
    *pq_NhzConfig = GNSS_NV_EFS_SM_NHZ_CONFIG_DEFAULT;
  }
}

/*===========================================================================
  sm_NhzDefaultConfigPut

  Description:  Get the NHz default config from SM
  
  Parameters :  None

  Return value: Default config

=============================================================================*/
uint32 sm_NhzDefaultConfigPut()
{
  return(GNSS_NV_EFS_SM_NHZ_CONFIG_DEFAULT);
}
#endif 
