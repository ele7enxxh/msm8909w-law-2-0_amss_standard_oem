
/*============================================================================
  @file:       loc_qmi_shim.c

  OVERVIEW:    This module is adapts the loc-api to QCSI framework. This
               file glues loc-api enabling it to be a provider of the 
               QMI-Loc Service. It has a few sections, any changes to the 
               file should be done in the appropriate sections to maintain 
               readability of the file. 

               UTILITY FUNCTIONS
                 All utility functions that are needed shall go here
                   
               MESSAGE CONVERTORS QMI --> Loc-Api
                 All functions that are used to convert QMI to Loc-Api data
                 structures shall be added to this section.
                   
               MESSAGE CONVERTORS Loc-Api --> QMI
                 All functions that are used to convert Loc-Api to QMI data
                 structures shall be added to this section.

               LOC-API CALLBACKS
                 All functions that are registered with the Loc-API shall be 
                 defined in this section
 
               MESSAGE HANDLERS 
                 All the messages handlers should be added to this section. 
                
               QCSI CALLBACKS
                 All functions that are registered with QCSI framework shall be
                 defined in this section
                
               EXTERNALIZED Functions
                 All functions that are exposed through the header file shall be
                 implemented here.
  
                
  DEPENDENCIES: QCSI Framework
 
                Copyright (c) 2011-2012, 2013 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
                All Rights Reserved. 
                Qualcomm Confidential and Proprietary
                Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_qmi_shim.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  01/21/16     yh  LocWifi cleanup
  10/06/15     yh  QZSS
  04/06/15     sj  LB 2.0 integration
  05/28/15     jv  Disabled QMI_LOC_[SET/GET]_SBAS_CONFIG_REQ_V02
  05/21/15     jv  Support for SBAS EPH & ALM Deletion
  11/07/14   ssu   GM LOWI Integration
  02/19/15     sj  Added reqId to status indications sent back by DBT
  11/06/14     ah  Fixed QMI_LOC_GET_LOW_POWER_MODE Indication failure
  04/15/14     yh  XTRA version check added
  03/10/14     ah  Added support for Set Premium Services Cfg & Get Avail WWAN Pos
  01/24/14     ah  Added GLO support for Get/Set NMEA Type Requests in QMI-LOC
  02/25/13    ssu  Adding a new WWAN type used for AGNSS Emergency connections.
  01/28/12      mj   Update sm_RegisterApiNotifier to include client thread id
  2011-03-08  spn  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"


/* Module Header include */
#include "loc_qmi_shim.h"

/* Non-GPS Includes */
#include "qmi_csi_target_ext.h"
#include "qmi_csi.h"
#include "qmi_sap.h"
#include "qmi_idl_lib.h"
#include "mqcsi_log.h"
#include "msg.h"
#include "queue.h"
#include "qmi_si.h"
#include "location_service_v02.h"
#include "location_service_impl_v02.h"
#include "common_v01.h"


/* GPS Includes */
#include "aries_os_api.h"
#include "loc_api_2.h"
#include "loc_task.h"
#include "loc_client.h"
#include "loc_api_internal.h"
#include "gnss_calendar_util.h"
#include "loc_sdp.h"
#include "loc_slim.h"
#include "loc_geofence.h"
#include "cgps_api.h"
#include "gnss_cqsvc.h"
#include "loc_pd.h"
#include "loc_batching.h"
#include "loc_aon.h"
#include "loc_gtp.h"
#include "secapi.h"
//#include "secapi_util.h"
#include "loc_wle.h"
#include "loc_pa.h"
#include "loc_nv_efs.h"
/* PDAPI - Include PDAPI to convert newly added APIs to PDAPI directly */
#include "pdapi.h"
#ifdef __GRIFFON__
#include "mgp_api.h"
#endif /* __GRIFFON__ */
#include "gnss_common.h"

/*****************************************************************************
 * Preprocessor Definitions and Constants
 * **************************************************************************/

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/* -----------------------------------------------------------------------*//**
@brief
  Signal that is used by the QMI Framework to notify the task in case there 
  are pending messages for the task to handle. 
*//* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_SIGNAL             C_OS_FLAG_QMI_EVENT



/* -----------------------------------------------------------------------*//**
@brief
  The Max Valid value of SUPL CertID that can be accepted
  Valid Values of SUPL CertID is [0..9]
*//* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_MAX_SUPL_CERT_ID      (9)

/* ------------------------------------------------------------------------*//**
@brief
  The number of Satellites in GPS or GLONASS constellation
*//* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_GNSS_CONSTELLATION_SIZE  (32)

/* ------------------------------------------------------------------------*//** 
@brief 
  Used for translating GPS SV IDs. GPS SV ID Range is [1..32]
*//* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_GPS_SV_OFFSET            (1)

/* ------------------------------------------------------------------------*//** 
@brief 
  Used for translating GLONASS SV IDs. GLONASS SV ID Range is [65..96]
 *//* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_GLO_SV_OFFSET            (65)
/** @brief Supported TBF interval for 20Hz */
#define LOC_QMI_SHIM_INTERVAL_20HZ_MS (50)
/** @brief Supported TBF interval for 10Hz */
#define LOC_QMI_SHIM_INTERVAL_10HZ_MS (100)
/** @brief Supported TBF interval for 5Hz */
#define LOC_QMI_SHIM_INTERVAL_5HZ_MS (200)
/** @brief Supported TBF interval for 2Hz */
#define LOC_QMI_SHIM_INTERVAL_2HZ_MS (500)
/** @brief Supported TBF interval for 1Hz */
#define LOC_QMI_SHIM_INTERVAL_1HZ_MS (1000)

/* -----------------------------------------------------------------------*//**
@brief
  Message Macros to be used in this module.
*//* ------------------------------------------------------------------------*/
#define SHIM_MSG_LOW( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str,  a, b, c )
#define SHIM_MSG_MED( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a, b, c )
#define SHIM_MSG_HIGH( str, a, b, c )                                         \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c )
#define SHIM_MSG_ERROR( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a, b, c )
#define SHIM_ERR_FATAL( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a, b, c )

/* -----------------------------------------------------------------------*//**
 @brief 
  Macro definition to verify if IOCTL Status is valid 
  
@param[in]  i_IoctlStatus - is an ioctl status that needs to be verified
*//* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_IS_LOC_IOCTL_STATUS_VALID(status)                        \
      ( ( ( (status) == LOC_API_SUCCESS ) ||                                \
          ( (status) == LOC_API_GENERAL_FAILURE ) ||                        \
          ( (status) == LOC_API_UNSUPPORTED ) ||                            \
          ( (status) == LOC_API_INVALID_HANDLE ) ||                         \
          ( (status) == LOC_API_INVALID_PARAMETER ) ||                      \
          ( (status) == LOC_API_PHONE_OFFLINE ) ||                          \
          ( (status) == LOC_API_ENGINE_BUSY ) ||                            \
          ( (status) == LOC_API_TIMEOUT ) ||                                \
          ( (status) == LOC_API_XTRA_VERSION_CHECK_FAILURE)) ? ( TRUE ) : ( FALSE ) ) \


/* -----------------------------------------------------------------------*//**
 @brief 
  Macro definition to set the value of a simple data type and its validity
  to TRUE. Simple macro-utility to update the QMI-Loc data structures for 
  simple data-types.
  
@param[in]  variable    the variable which needs to be set
@param[in]  value       the value that it needs to be set to
*//* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_VARIABLE_VALID(variable) variable ## _valid
#define LOC_QMI_SHIM_SET_VALUE(variable, value)                               \
           LOC_QMI_SHIM_VARIABLE_VALID(variable) = TRUE;                      \
           (variable)= (value)


/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
 @brief 
  The information required for handling messages is same as that of indication.
  Hence reusing the same structure. 
*//* ------------------------------------------------------------------------*/
typedef locQmiShimIndInfoStructT locQmiShimMsgInfoStructT;

/* -----------------------------------------------------------------------*//**
@brief
  Context information for each client is stored in this structure. Elements of
  this type will be stored in a queue, where context information for all 
  clients will be stored. This element also stores the mapping of handles as 
  allocated by the QMI-Loc Service and the Loc-API. An instance of this 
  structure is created whenever a new client connects, and destroyed when a 
  client deregisters. This structure is used as the connection handle that 
  will be given to the QCSI-Framework for it to identify a client to the 
  service provider.
*//* ------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                  z_Link; /*< Data Structure required by the queue 
                                           library to add this as part of a 
                                           queue */
  void                        *p_QmiClientHandle;/*< Handle identifying the 
                                           client of QMI-Loc to identify the  
                                           client */
  loc_client_handle_type       z_LocClientHandle; /*< Handle provided by 
                                           loc-api when a new new client 
                                           registers with loc-api, as a result 
                                           of a client connecting to QMI-Loc */
  uint64                       t_EventMask; /*< Save the event mask to filter 
                                            the events from loc-api. As 
                                            QMI-Loc has more granularity */
  uint32                       q_CltRev; /*< The Client Revision, this is 
                                             indicated by the client */
  uint8                        u_SessionId; /*< An identifier of the current 
                                             session as specified by the 
                                             client */
  uint32                       q_fixID; /*< Running counter of fixes generated 
                                            in a session */
  qmiLocConfigIncludeAltitudeAssumedInGnssSvInfoEnumT_v02
                               configAltitudeAssumed; /* Configuration to choose if 
                                                         Altitude asssumed is needed in
                                                         SV report for this client */
  boolean                      isBatchingClient; /*< to mark if this is a batching client */
} locQmiShimCltInfoStructT;

/* -----------------------------------------------------------------------*//**
@brief
  Union of all indications that can be sent out as a result of the callback 
  from loc-api. 

  Note: The indication structures are in alphabetical order, it allows for easy
        search, and remove redundancies. This order needs to be maintained.
        
*//* ------------------------------------------------------------------------*/
typedef union 
{
  qmiLocDeleteAssistDataIndMsgT_v02               z_DeleteAssistDataInd;
  qmiLocEventEngineStateIndMsgT_v02               z_EngStateInd;
  qmiLocEventFixSessionStateIndMsgT_v02           z_FixSessStateInd;  
  qmiLocEventGnssSvInfoIndMsgT_v02                z_GnssSvInfoInd;
  qmiLocEventNmeaIndMsgT_v02                      z_NmeaInd;    
  qmiLocEventNiNotifyVerifyReqIndMsgT_v02         z_NiNotifyVeryReqInd;
  qmiLocEventInjectPositionReqIndMsgT_v02         z_PosInjReqInd;
  qmiLocEventPositionReportIndMsgT_v02            z_PosRepInd;
  qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02  z_OrbitsInjReqInd;
  qmiLocEventInjectTimeReqIndMsgT_v02             z_TimeInjReqInd;
  qmiLocEventWifiReqIndMsgT_v02                   z_WifiReqInd;
  qmiLocGetEngineLockIndMsgT_v02                  z_GEtEngineLockInd;
  qmiLocGetFixCriteriaIndMsgT_v02                 z_GetFixCriteriaInd;
  qmiLocGetLowPowerModeIndMsgT_v02                z_GetLowPowerInd;
  qmiLocGetNmeaTypesIndMsgT_v02                   z_GetNmeaTypesInd;
  qmiLocGetOperationModeIndMsgT_v02               z_GetOperModeInd;
  qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02   z_XtraSourceInd;
  qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02 z_GetXtraValidityInd;
  qmiLocGetProtocolConfigParametersIndMsgT_v02    z_GetProtConfigParamInd;
  qmiLocGetSbasConfigIndMsgT_v02                  z_GetSbasInd;
  qmiLocGetServerIndMsgT_v02                      z_GetServerInd;
  qmiLocGetServiceRevisionIndMsgT_v02             z_GetSvcRevInd;
  qmiLocGetXtraTSessionControlIndMsgT_v02         z_GetXtraSessionCtrlInd;
  qmiLocGetPositionEngineConfigParametersIndMsgT_v02 z_GetPeConfigParamInd;
  qmiLocInformLocationServerConnStatusIndMsgT_v02 z_ConnStatusInd;
  qmiLocInjectPositionIndMsgT_v02                 z_InjPosInd;
  qmiLocInjectPredictedOrbitsDataIndMsgT_v02      z_InjPredOrbitsInd;
  qmiLocInjectUtcTimeIndMsgT_v02                  z_InjUtcTimeInd;
  qmiLocInjectWifiPositionIndMsgT_v02             z_InjWifiPosInd;
  qmiLocNiUserRespIndMsgT_v02                     z_NiUsrRespInd;
  qmiLocNotifyWifiStatusIndMsgT_v02               z_NotifyWifiStatusInd;
  qmiLocSetEngineLockIndMsgT_v02                  z_SetEngineLockInd;
  qmiLocSetLowPowerModeIndMsgT_v02                z_SetLowPowerInd;  
  qmiLocSetOperationModeIndMsgT_v02               z_SetOperModeInd;
  qmiLocSetProtocolConfigParametersIndMsgT_v02    z_SetProtConfigParamInd;
  qmiLocSetSbasConfigIndMsgT_v02                  z_SetSbasInd;
  qmiLocSetServerIndMsgT_v02                      z_SetServerInd;
  qmiLocSetXtraTSessionControlIndMsgT_v02         z_XtratSessCtrlInd;
  qmiLocSetPositionEngineConfigParametersIndMsgT_v02 z_SetPeConfigParamInd;
  qmiLocInjectSuplCertificateIndMsgT_v02          z_InjectSuplCertInd;
  qmiLocDeleteSuplCertificateIndMsgT_v02          z_DeleteSuplCertInd;
  qmiLocGetBestAvailablePositionIndMsgT_v02       z_GetBestAvailablePositionInd;
  qmiLocSecureGetAvailablePositionIndMsgT_v02     z_SecureGetAvailablePositionInd;
  qmiLocInjectGSMCellInfoIndMsgT_v02              z_InjectGSMCellInfoInd;
  qmiLocInjectNetworkInitiatedMessageIndMsgT_v02  z_InjectNIMessageInd;
  qmiLocWWANOutOfServiceNotificationIndMsgT_v02   z_WWANOutofServiceNotificationInd;
  qmiLocSetGNSSConstRepConfigIndMsgT_v02          z_GNSSConfig;
  qmiLocInjectTimeZoneInfoIndMsgT_v02             z_InjectTZInfoInd;
  qmiLocInjectApCacheDataIndMsgT_v02              z_InjectApCacheDataInd;
  qmiLocInjectApDoNotCacheDataReqMsgT_v02         z_InjectNoApCacheDataInd;
} locQmiShimIndUnionT; 

/* -----------------------------------------------------------------------*//**
 @brief 
  Structure with information that is required for queuing up an indication 
  that needs to be serialized behind the current line of execution. 
*//* ------------------------------------------------------------------------*/
typedef struct
{
  q_link_type               z_Link;    /*< Structure required by the queue 
                                       library */
  locQmiShimCltInfoStructT *pz_CltInfo; /*< The client to which the indication
                                       needs to be sent */
  uint32                    q_IndId;   /*< Indication ID as allocated by the 
                                       service definition */
  locQmiShimIndUnionT       z_IndInfo; /*< The message itself */
  uint32                    q_IndLen;  /*< Length of the message */
} locQmiShimQueuedIndStructT;

/* -----------------------------------------------------------------------*//**
@brief
  THIS STRUCTURE SHOULD BE INSTANTIATED ONLY ONCE. This is the service handle 
  that will be given to the framework to identify the service. And contains 
  the state information of the shim layer. 
*//* ------------------------------------------------------------------------*/
typedef struct
{
  q_type                 z_ClientQ; /**< Information about all the clients that
                               have connected to the service. This is queue 
                               of type locQmiShimCltInfoStructT */
  void*                  p_ServiceHandle;  /**< Handle that the framework
                               provides to represent this service connection */

  qmi_sap_client_handle  z_QSAPUserHandle; /**< QSAP user handle to register with 
                                QSAP.*/

  q_type                 z_IndFreeQ; /**< Queue of Free ind buffers to be used
                               to store indications that have to be sent out
                               after current execution path is completed. A 
                               buffer in this queue is ready to be used, and 
                               need not be memset'ed after q_get() from here */
  q_type                 z_IndQ; /**< Queue of indications that need to sent 
                               out after current execution path is completed */
} locQmiShimInfoStructT;


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle GetBatchSize Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the get batch size request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locGetBatchSizeReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                  qmi_req_handle              z_ReqHandle,
                                  locQmiShimMsgInfoStructT    *pz_MsgInfo);

/* -----------------------------------------------------------------------*//**
@brief
  Untility function to send Response mesasge to Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the response message
@param[in]   resp_value            Response status being sent to control point
@param[in]   e_QmiErr              QMI Error type being sent to control point


@retval    TRUE           Sending Response to control point success
@retval    FALSE          Sending Response to control point fail
*//* ------------------------------------------------------------------------*/
static boolean locBatchingOperationSendResp(locQmiShimCltInfoStructT    * const pz_CltInfo,
                                            qmi_req_handle              z_ReqHandle,
                                            locQmiShimMsgInfoStructT    *pz_MsgInfo,
                                            boolean                     resp_value,
                                            qmi_error_type_v01          e_QmiErr);


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle StartBatching Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pStartBatchingReq     Request message from the control point


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locStartBatchingReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                   qmi_req_handle               z_ReqHandle,
                                   locQmiShimMsgInfoStructT     *pz_MsgInfo);


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle ReadFromBatch Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the ReadFromBatch request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locReadFromBatchReq(locQmiShimCltInfoStructT     *const pz_CltInfo,
                                   qmi_req_handle               z_ReqHandle,
                                   locQmiShimMsgInfoStructT     *pz_MsgInfo);


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle StopBatching Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopBatching request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locStopBatchingReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                  qmi_req_handle              z_ReqHandle,
                                  locQmiShimMsgInfoStructT    *pz_MsgInfo); 


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle ReleaseBatch Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the ReleaseBatch request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locReleaseBatchReq(locQmiShimCltInfoStructT      *const pz_CltInfo,
                                  qmi_req_handle                z_ReqHandle,
                                  locQmiShimMsgInfoStructT      *pz_MsgInfo); 


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle a Start DBT session from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StartDBT request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locStartDbtReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                  qmi_req_handle              z_ReqHandle,
                                  locQmiShimMsgInfoStructT    *pz_MsgInfo);

/* -----------------------------------------------------------------------*//**
@brief
  Function to handle Stop a DBT session from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopDBT request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locStopDbtReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                  qmi_req_handle              z_ReqHandle,
                                  locQmiShimMsgInfoStructT    *pz_MsgInfo);



/* -----------------------------------------------------------------------*//**
@brief
  Function to retrieve the Aon configuration 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopDBT request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locQueryAonConfigReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                    qmi_req_handle              z_ReqHandle,
                                    locQmiShimMsgInfoStructT    *pz_MsgInfo);

/*============================================================================
 * Definitions required to declare a function table of Convertors to convert 
 * from QMI-Loc to Loc-Api
 * =========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
 Forward-Function Declarations of Converters. Detailed description of functions
 are provided along with function-definitions later. 
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetEventRegMask(
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocSetFixCriteria(
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocInjectPredictedOrbitsData(
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocInjectUtcTime(
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocInjectPosition(
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocSetEngineLock(
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocSetNmeaTypes (
                const void *pz_Msg, loc_ioctl_data_u_type *pz_IoctlInfo );
static boolean locQmiShimConvToLocDeleteAssistData (
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocXtraTSessCtrl ( 
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocSetOperationMode (
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocNiUserResponse (
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocSetSbasConfig (
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocSetLowPowerMode (
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocInjectSuplCertificate (
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocDeleteSuplCertificate (
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimConvToLocSetGNSSConfig(
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);

static boolean locQmiShimDecryptSecureAvailablePositionDataBlob(
                const void * pz_EncryptedMsg,void * pz_DecryptedMsg);
static boolean locQmiShimConvToLocGetSecureAvailablePosition(
                const void * pz_Msg,loc_ioctl_data_u_type * pz_IoctlInfo);
static boolean locQmiShimPopulateLocGetSecureAvailablePositionParam(
                const uint8 *src,uint32 src_length,qmiLocGetAvailablePositionReqParamMsgT_v02 *pz_DecodedStartReq);

/* -----------------------------------------------------------------------*//**
@brief
  Function prototype for the converter function from QMI-Loc to Loc-Api

@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format   

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
typedef boolean (*locQmiShimConvToLocFuncT)
(
  const void                   *pz_Msg,
  loc_ioctl_data_u_type        *pz_IoctlInfo 
);


/* -----------------------------------------------------------------------*//**
@brief
  structure definition of functions table and information to convert incoming 
  message to Loc-API ioctl  
*//* ------------------------------------------------------------------------*/
typedef struct
{
  uint32                    q_qmiLocMsgId;/*< This is the message id of 
                                              incoming QMI-Loc Msg */
  loc_ioctl_e_type          e_LocMwMsgId;/*< This is the for Loc-Api 
                                             Equivalent Message ID */
  locQmiShimConvToLocFuncT  pz_ConvFunc; /*< Pointer to a converter function */
} locQmiShimConvToLocIoctlStructT; 

/* Externing as including sm_api.h causes a lot of dependencies */
extern boolean sm_RegisterApiNotifier( void (*sm_ApiInitNotifierCb)(void), os_ThreadIdEnumType clientThreadId);


/*****************************************************************************
 * Global Data Definitions
 * *************************************************************************/

/*****************************************************************************
 * Static Variable Definitions
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  Memory allocated, as loc-api expects a pointer for which memory is allocated
  by the caller. 
  
  This information SHALL NOT BE EXPOSED OUT OF THIS FILE  

  NOTE: 2000 is selected since "QMI_LOC_MAX_SUPL_CERT_LENGTH_V02" is the max 
        buffer length that is needed. 
        There are other commands that use this buffer
*//* ------------------------------------------------------------------------*/
#define   LOC_QMI_SHIM_MAX_CHAR_BUF_LEN      (2000)
static uint8 pu_LocQmiShimCharBuf[LOC_QMI_SHIM_MAX_CHAR_BUF_LEN];
/* -----------------------------------------------------------------------*//**
@brief
  Declaration of lookup table for Converters that would convert incoming
  QMI-Loc data into data that is suitable for Loc-Api. This function table
  is used for IOCTLs that are exposed in loc-api.h. For Extended IOCTLs this 
  table is not referred

  This information SHALL NOT BE EXPOSED OUT OF THIS FILE  

@warning   QMI_LOC_SET_SERVER_REQ_V02 --> Is not handled in this table, 
                             because one QMI-Request can trigger 
                             one-of-the-four Loc-Api-Ioctls
@warning   QMI_LOC_GET_SERVER_REQ_V02 --> Is not handled in this table, 
                             because one QMI-Request can trigger 
                             one-of-the-four Loc-Api-Ioctls
@warning   QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02 -->  Is not 
                             handled in this table, because one QMI-Request can 
                             trigger more than one Loc-Api-IOCTL
@see       locQmiShimConvToLocGetServer(), locQmiShimConvToLocSetServer(),
           locQmiShimConvToLocInformLocationServerConnStatus()
*//* ------------------------------------------------------------------------*/
static locQmiShimConvToLocIoctlStructT pz_LocQmiShimConvToLoc[] = 
{
  { QMI_LOC_START_REQ_V02, 
    LOC_IOCTL_SET_FIX_CRITERIA, 
    locQmiShimConvToLocSetFixCriteria },
   
  { QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02,
    LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA,
    locQmiShimConvToLocInjectPredictedOrbitsData },

  { QMI_LOC_INJECT_UTC_TIME_REQ_V02,
    LOC_IOCTL_INJECT_UTC_TIME,
    locQmiShimConvToLocInjectUtcTime },

  { QMI_LOC_INJECT_POSITION_REQ_V02,
    LOC_IOCTL_INJECT_POSITION,
    locQmiShimConvToLocInjectPosition },

  { QMI_LOC_SET_ENGINE_LOCK_REQ_V02,
    LOC_IOCTL_SET_ENGINE_LOCK,
    locQmiShimConvToLocSetEngineLock },

  { QMI_LOC_GET_ENGINE_LOCK_REQ_V02,
    LOC_IOCTL_GET_ENGINE_LOCK,
    NULL },

  { QMI_LOC_SET_NMEA_TYPES_REQ_V02,
    LOC_IOCTL_SET_NMEA_TYPES,
    locQmiShimConvToLocSetNmeaTypes },

  { QMI_LOC_DELETE_ASSIST_DATA_REQ_V02,
    LOC_IOCTL_DELETE_ASSIST_DATA,
    locQmiShimConvToLocDeleteAssistData }, 

  { QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02,
    LOC_IOCTL_SET_XTRA_T_SESSION_CONTROL,
    locQmiShimConvToLocXtraTSessCtrl },

  { QMI_LOC_NI_USER_RESPONSE_REQ_V02,
    LOC_IOCTL_INFORM_NI_USER_RESPONSE,
    locQmiShimConvToLocNiUserResponse },

  { QMI_LOC_GET_NMEA_TYPES_REQ_V02,
    LOC_IOCTL_GET_NMEA_TYPES,
    NULL },

  { QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02,
    LOC_IOCTL_GET_XTRA_T_SESSION_CONTROL,
    NULL },

  { QMI_LOC_GET_FIX_CRITERIA_REQ_V02,
    LOC_IOCTL_GET_FIX_CRITERIA,
    NULL },

  { QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02,
    LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE,
    NULL },

  { QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02,
    LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY,
    NULL },

  { QMI_LOC_SET_SBAS_CONFIG_REQ_V02,
    LOC_IOCTL_SET_SBAS_CONFIG,
    locQmiShimConvToLocSetSbasConfig },

  { QMI_LOC_GET_SBAS_CONFIG_REQ_V02,
    LOC_IOCTL_GET_SBAS_CONFIG,
    NULL },

  { QMI_LOC_GET_LOW_POWER_MODE_REQ_V02,
    LOC_IOCTL_GET_ON_DEMAND_LPM,  
    NULL },
    
  { QMI_LOC_SET_LOW_POWER_MODE_REQ_V02,
    LOC_IOCTL_SET_ON_DEMAND_LPM,  
    locQmiShimConvToLocSetLowPowerMode },

  { QMI_LOC_REG_EVENTS_REQ_V02,
    LOC_IOCTL_SET_EVENT_REG_MASK, 
    locQmiShimConvToLocSetEventRegMask },

  { QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02,
    LOC_IOCTL_GET_EVENT_REG_MASK,
    NULL },

  { QMI_LOC_SET_OPERATION_MODE_REQ_V02,
    LOC_IOCTL_SET_OPERATION_MODE,
    locQmiShimConvToLocSetOperationMode }, 

  
  { QMI_LOC_GET_OPERATION_MODE_REQ_V02,
    LOC_IOCTL_GET_OPERATION_MODE,
    NULL }, 
  { QMI_LOC_INJECT_SUPL_CERTIFICATE_REQ_V02,
    LOC_IOCTL_ACCESS_EFS_DATA,
    locQmiShimConvToLocInjectSuplCertificate },
    
  { QMI_LOC_DELETE_SUPL_CERTIFICATE_REQ_V02,
    LOC_IOCTL_ACCESS_EFS_DATA,
    locQmiShimConvToLocDeleteSuplCertificate },

  { QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_V02,
    LOC_IOCTL_SET_GNSS_CONSTELL_REPORT_CONFIG,
    locQmiShimConvToLocSetGNSSConfig },
};

/* -----------------------------------------------------------------------*//**
@brief
  A Single instance of the module context information. This instance is also
  provided as a service cookie to the QCSI framework. 

  This information SHALL NOT BE EXPOSED OUT OF THIS FILE  
*//* ------------------------------------------------------------------------*/
static locQmiShimInfoStructT z_LocQmiShim;


/* -----------------------------------------------------------------------*//**
@brief
  Mapping of masks from QMI-Loc Event Masks to Loc-API Event Masks. This is a 
  singleton, there is no need to define another instance of this
*//* ------------------------------------------------------------------------*/
static struct 
{
  uint64        t_QmiMask;
  uint64        t_LocMask;
} z_LocQmiShimMaskMap[]=                             /* Mask mapping */
{
  { QMI_LOC_EVENT_MASK_POSITION_REPORT_V02, 
    LOC_EVENT_PARSED_POSITION_REPORT              },
  
  { QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02, 
    LOC_EVENT_SATELLITE_REPORT                    },
    
  { QMI_LOC_EVENT_MASK_NMEA_V02, 
    LOC_EVENT_NMEA_POSITION_REPORT                },
    
  { QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ_V02, 
    LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST            },
    
  { QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02, 
    LOC_EVENT_ASSISTANCE_DATA_REQUEST             },
    
  { QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02, 
    LOC_EVENT_ASSISTANCE_DATA_REQUEST             },
    
  { QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02, 
    LOC_EVENT_ASSISTANCE_DATA_REQUEST             },
    
  { QMI_LOC_EVENT_MASK_ENGINE_STATE_V02, 
    LOC_EVENT_STATUS_REPORT                       },

  { QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02, 
    LOC_EVENT_STATUS_REPORT                       },
    
  { QMI_LOC_EVENT_MASK_WIFI_REQ_V02, 
    LOC_EVENT_WPS_NEEDED_REQUEST                  },

  { QMI_LOC_EVENT_MASK_LOCATION_SERVER_CONNECTION_REQ_V02,
    LOC_EVENT_LOCATION_SERVER_REQUEST             },

  { QMI_LOC_EVENT_MASK_SENSOR_STREAMING_READY_STATUS_V02,
    LOC_EVENT_SENSOR_STREAMING_READY_STATUS       },

  { QMI_LOC_EVENT_MASK_TIME_SYNC_REQ_V02,
    LOC_EVENT_TIME_SYNC_REQ                       },

  { QMI_LOC_EVENT_MASK_SET_SPI_STREAMING_REPORT_V02,
    LOC_EVENT_SPI_STREAMING_REPORT                },
    
  { QMI_LOC_EVENT_MASK_GNSS_MEASUREMENT_REPORT_V02,
    LOC_EVENT_GNSS_MEASUREMENT_REPORT 			  },

  { QMI_LOC_EVENT_MASK_GNSS_SV_POLYNOMIAL_REPORT_V02,
	LOC_EVENT_SV_POLY_REPORT 			          },

};

/* -----------------------------------------------------------------------*//**
@brief
  
   Mapping of masks from QMI-Loc Assistance Masks to PDAPI Assistance Masks. 
   These are specific to GnssData Masks.

   This is a singleton, there is no need to define another instance of this
*//* ------------------------------------------------------------------------*/
static struct
{
  uint32        q_QmiMask;
  uint32        q_PdapiMask;
}  z_LocQmiShimDeleteCellDbDataMaskMap[] =
  {
      { QMI_LOC_MASK_DELETE_CELLDB_POS_V02,
        PDSM_PA_DELETE_CELLDB_MASK_POS_INFO },
      { QMI_LOC_MASK_DELETE_CELLDB_LATEST_GPS_POS_V02,
        PDSM_PA_DELETE_CELLDB_MASK_LATEST_GPS_POS_INFO },
      { QMI_LOC_MASK_DELETE_CELLDB_OTA_POS_V02,
        PDSM_PA_DELETE_CELLDB_MASK_OTA_POS_INFO },
      { QMI_LOC_MASK_DELETE_CELLDB_EXT_REF_POS_V02, 
        PDSM_PA_DELETE_CELLDB_MASK_EXT_REF_POS_INFO },
      { QMI_LOC_MASK_DELETE_CELLDB_TIMETAG_V02,
        PDSM_PA_DELETE_CELLDB_MASK_TIMETAG_INFO },     
      { QMI_LOC_MASK_DELETE_CELLDB_CELLID_V02,
        PDSM_PA_DELETE_CELLDB_MASK_CELLID_INFO },
      { QMI_LOC_MASK_DELETE_CELLDB_CACHED_CELLID_V02,
        PDSM_PA_DELETE_CELLDB_MASK_CACHED_CELLID_INFO },
      { QMI_LOC_MASK_DELETE_CELLDB_LAST_SRV_CELL_V02,
        PDSM_PA_DELETE_CELLDB_MASK_LAST_SRV_CELL_INFO },
      { QMI_LOC_MASK_DELETE_CELLDB_CUR_SRV_CELL_V02,
        PDSM_PA_DELETE_CELLDB_MASK_CUR_SRV_CELL_INFO },
      { QMI_LOC_MASK_DELETE_CELLDB_NEIGHBOR_INFO_V02,
        PDSM_PA_DELETE_CELLDB_MASK_NEIGHBOR_INFO_INFO },
  };


/* -----------------------------------------------------------------------*//**
@brief
  Mapping of masks from QMI-Loc Assistance Masks to Loc-API Assistance Masks. 
  These are specific to GnssData Masks.
  
  This is a singleton, there is no need to define another instance of this
*//* ------------------------------------------------------------------------*/
static struct 
{
  uint64        t_QmiMask;
  uint64        q_LocMask;
} z_LocQmiShimDeleteGnssDataMaskMap[]=                  /* Mask mapping */
  {
      { QMI_LOC_MASK_DELETE_GPS_SVDIR_V02,
        LOC_ASSIST_DATA_SVDIR },
      { QMI_LOC_MASK_DELETE_GPS_SVSTEER_V02,
        LOC_ASSIST_DATA_SVSTEER},
      { QMI_LOC_MASK_DELETE_GPS_TIME_V02,
        LOC_ASSIST_DATA_TIME_GPS },
      { QMI_LOC_MASK_DELETE_GPS_ALM_CORR_V02,
        LOC_ASSIST_DATA_ALM_CORR_DELETE },
      { QMI_LOC_MASK_DELETE_GLO_SVDIR_V02,
        LOC_ASSIST_DATA_SVDIR_GLO },
      { QMI_LOC_MASK_DELETE_GLO_SVSTEER_V02,
        LOC_ASSIST_DATA_SVSTEER_GLO },
      { QMI_LOC_MASK_DELETE_GLO_TIME_V02,
        LOC_ASSIST_DATA_TIME_GLO },
      { QMI_LOC_MASK_DELETE_GLO_ALM_CORR_V02,
        LOC_ASSIST_DATA_ALM_CORR_GLO_DELETE },
      { QMI_LOC_MASK_DELETE_SBAS_SVDIR_V02,
        LOC_ASSIST_DATA_SVDIR_SBAS },
      { QMI_LOC_MASK_DELETE_SBAS_SVSTEER_V02,
        LOC_ASSIST_DATA_SVSTEER_SBAS },
      { QMI_LOC_MASK_DELETE_POSITION_V02,
        LOC_ASSIST_DATA_POS },
      { QMI_LOC_MASK_DELETE_TIME_V02,
        LOC_ASSIST_DATA_TIME },
      { QMI_LOC_MASK_DELETE_IONO_V02,
        LOC_ASSIST_DATA_IONO },
      { QMI_LOC_MASK_DELETE_UTC_V02,
        LOC_ASSIST_DATA_UTC },
      { QMI_LOC_MASK_DELETE_HEALTH_V02,
        LOC_ASSIST_DATA_HEALTH },
      { QMI_LOC_MASK_DELETE_SADATA_V02,
        LOC_ASSIST_DATA_SADATA },
      { QMI_LOC_MASK_DELETE_RTI_V02,
        LOC_ASSIST_DATA_RTI },
      { QMI_LOC_MASK_DELETE_SV_NO_EXIST_V02,
        LOC_ASSIST_DATA_SV_NO_EXIST },
      { QMI_LOC_MASK_DELETE_FREQ_BIAS_EST_V02,
        LOC_ASSIST_DATA_FREQ_BIAS_EST_DELETE },
      { QMI_LOC_MASK_DELETE_BDS_SVDIR_V02,
        LOC_ASSIST_DATA_SVDIR_BDS },
      { QMI_LOC_MASK_DELETE_BDS_SVSTEER_V02,
        LOC_ASSIST_DATA_SVSTEER_BDS },
      { QMI_LOC_MASK_DELETE_BDS_TIME_V02,
        LOC_ASSIST_DATA_TIME_BDS },
      { QMI_LOC_MASK_DELETE_BDS_ALM_CORR_V02,
        LOC_ASSIST_DATA_ALM_CORR_BDS_DELETE },
      { QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_GPS_V02,
        LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GPS },
      { QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_GLO_V02,
        LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GLO },
      { QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_BDS_V02,
        LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_BDS },
      { QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_GAL_V02,
        LOC_ASSIST_DATA_GNSS_SV_BLACKLIST_GAL },
      { QMI_LOC_MASK_DELETE_GAL_SVDIR_V02,
        LOC_ASSIST_DATA_SVDIR_GAL },
      { QMI_LOC_MASK_DELETE_GAL_SVSTEER_V02,
        LOC_ASSIST_DATA_SVSTEER_GAL },
      { QMI_LOC_MASK_DELETE_GAL_TIME_V02,
        LOC_ASSIST_DATA_TIME_GAL },
      { QMI_LOC_MASK_DELETE_GAL_ALM_CORR_V02,
        LOC_ASSIST_DATA_ALM_CORR_GAL_DELETE }
  };


/* -----------------------------------------------------------------------*//**
@brief
  Memory allocation for a pool of buffers, to be used by indications that are 
  queued up for later processing. 
*//* ------------------------------------------------------------------------*/
#define  LOC_QMI_SHIM_IND_BUFFER_POOL_MAX             2
static locQmiShimQueuedIndStructT  
                             z_IndBufferPool[LOC_QMI_SHIM_IND_BUFFER_POOL_MAX];

/* -----------------------------------------------------------------------*//**
@brief
  All SUPL certificates will be injected in /SUPL/ folder, and will be named
  Cert0, Cert1, .. Cert9. The common prefix for all Certificates is defined here

  Note, this prefix must be kept in sync with the definition in tm_common.h
*//* ------------------------------------------------------------------------*/
static char const * const  pz_LocQmiShimCertLocnPrefix =  "/SUPL/Cert" ;
static char const * const  pz_LocQmiShimCertDirLocn    =  "/SUPL/" ;


/*****************************************************************************
 * Internal/Static Function Declarations and Definitions
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to get min interval NV item. 
*/
/* ------------------------------------------------------------------------*/
uint32 locQmiShimReadNvMinInterval(void)
{
   uint32  q_SupportedMinInterval = GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT; //default value;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* read the default NV's */
  if (FALSE == locNvEfsRead(
           GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL, 
           (void *)&q_SupportedMinInterval, sizeof(uint32) ))
  {
    q_SupportedMinInterval = GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT; //default value
    SHIM_MSG_HIGH("NHZ Could not read nv for SpportedMinInterval, defaulting it to %u ms", 
                   q_SupportedMinInterval, 0, 0 );
  }
  else
  {
    SHIM_MSG_HIGH("NHZ Read nv for SpportedMinInterval Success %u ms", q_SupportedMinInterval, 0, 0 );
  }

   return q_SupportedMinInterval;
}
/*============================================================================
 *                    U T I L I T Y    F U N C T I O N S 
 * =========================================================================*/

/**
 * @brief Select closest supported time interval for TBF
 *
 * The method selects appropriate time interval in milliseconds for time between
 * fixes. All intervals are rounded up to the next supported value if they are
 * lower, than 1000ms.
 *
 * Values over 1000ms are rounded up to the next integer second interval.
 *
 * When NHz feature is enabled, the following values are allowed:
 * - 250 milliseconds - 4Hz rate;
 * - 500 milliseconds - 2Hz rate;
 * - N*1000 millseconds - 1Hz rate and lower with one second boundaries.
 *
 * @param[in] q_IntervalMs Time interval in milliseconds to round up.
 *
 * @return Time interval in milliseconds supported by engine.
 */
static uint32 locQmiShimMatchFixInterval
(
  uint32 q_IntervalMs
)
{
  /**
   * @brief Table of supported intervals.
   *
   * This table has to be monotonic with growing values up to 1000ms. When
   * NHz feature is not enabled, only 1000ms is supported.
   */
  static const uint32 pq_IntervalsMs[] =
  {
#ifdef FEATURE_LOCTECH_NHZ
    LOC_QMI_SHIM_INTERVAL_20HZ_MS,
    LOC_QMI_SHIM_INTERVAL_10HZ_MS,
    LOC_QMI_SHIM_INTERVAL_5HZ_MS,
    LOC_QMI_SHIM_INTERVAL_2HZ_MS,
#endif /* FEATURE_LOCTECH_NHZ */
    LOC_QMI_SHIM_INTERVAL_1HZ_MS
  };

  static boolean v_LocNvInit = FALSE;
  static uint32  q_SupportedMinInterval = LOC_QMI_SHIM_INTERVAL_1HZ_MS; // 1 Sec
  uint32 q_RetVal = q_IntervalMs;

  /* Only read NV supported min interval once */
  if(FALSE == v_LocNvInit)
  {
     if(TRUE == locNvEfsInit())
     {
        q_SupportedMinInterval = locQmiShimReadNvMinInterval();
        v_LocNvInit = TRUE;
     }
     else
     {
        SHIM_MSG_ERROR("locQmiShimMatchFixInterval: locNvEfsInit Failed!",0,0,0);
     }
  }

  /* Restrict the interval to the MIN supported value */
  if ( q_RetVal < q_SupportedMinInterval )
  {
    q_RetVal = q_SupportedMinInterval;
  }


  if (q_IntervalMs >= LOC_QMI_SHIM_INTERVAL_1HZ_MS)
  {
    if (0 != q_IntervalMs % LOC_QMI_SHIM_INTERVAL_1HZ_MS)
    {
      q_RetVal = (q_IntervalMs / LOC_QMI_SHIM_INTERVAL_1HZ_MS + 1) *
                 LOC_QMI_SHIM_INTERVAL_1HZ_MS;
    }
  }
  else
  {
    uint32 q_Idx = 0;

    for (q_Idx = ARR_SIZE(pq_IntervalsMs); q_Idx ; q_Idx--)
    {
       if( q_RetVal >= pq_IntervalsMs[q_Idx-1] )
       {
          q_RetVal = pq_IntervalsMs[q_Idx-1];
          break;
       }
    }
  }

  SHIM_MSG_MED("locQmiShimMatchFixInterval: input=%ums output=%ums",
               q_IntervalMs,
               q_RetVal,
               0);

   return q_RetVal;
}

/* For Griffon*/
#ifdef __GRIFFON__
#include "qmi_csi_common.h"
#include "upos.h"
extern qmi_csi_xport_ops_type udp_ops;
static qmi_csi_os_params g_z_OsParams;
static UPOS_thread_t upos_thread ;
static void read_udp_and_signal()
{
	fd_set fds;

	printf("created thread: in read_udp_and_signal\n");
	/* This loop calls a blocking select to read from the UDP port that was
           provided to QCSI during service registration */
	while(1)
	{
		//Wait for a signal from the mother task to resume
		UPOS_ThreadWait(&upos_thread);
        fds = g_z_OsParams.fds;
	    printf("read_udp_and_signal: waiting for fd signal\n");
	    select(g_z_OsParams.max_fd+1, &fds, NULL, NULL, NULL);

            // signal the "mother" thread
	    printf("read_udp_and_signal: signaling the main thread\n");
	    UPOS_ThreadSignalTask(&upos_thread,C_OS_FLAG_QMI_EVENT);
	}
}
#endif /* __GRIFFON__ */

/* -----------------------------------------------------------------------*//**
@brief
  A helper utility to ensure the validity of a boolean. Booleans are usually
  represented in uint8 where "0x00" is FALSE and "0x01" is TRUE. However 
  over the wire it is possible that an 8-bit unsigned integer whose absolute 
  value does not represent a boolean is sent in. This function is provided to 
  check if the 8-bit unsigned integer is a valid-boolean representation or not
  
@param[in]   u_Value           The value which needs to be validated

@retval      TRUE              If value is a boolean
@retval      FALSE             If value is not a boolean

@warning 
  This check is done for incoming messages.
*//* ------------------------------------------------------------------------*/
static GNSS_INLINE boolean locQmiShimIsValidBoolean(uint8  u_Value)
{
  /* Assumption: TRUE is "1" and FALSE is "0", ie the valid values
     are less than "2" */
  return (u_Value < 2);
}


/* -----------------------------------------------------------------------*//**
@brief
  A helper utility to memcpy from one array to the other, bounded by 
  max length
  
@param[in]   q_SrcLen          The length to copy
@param[in]   p_Src             The source buffer to copy from
@param[in]   q_DstLenMax       The max length that can be written
@param[out]  p_Dst             The destination buffer for the copy

@retval      The number of bytes copied to the destination
*//* ------------------------------------------------------------------------*/
#define locQmiShimMemcpy(q_SrcLen,p_Src,q_DstLenMax,p_Dst)                   \
              __locQmiShimMemcpy(q_SrcLen,p_Src,q_DstLenMax,p_Dst,__LINE__)

static uint32 __locQmiShimMemcpy
( 
  uint32          q_SrcLen,
  const void*     p_Src,
  uint32          q_DstLenMax,
  void*           p_Dst,
  uint32          line
)
{
  uint32 q_Length = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Bound the copy based on Max Length */
  if (q_SrcLen > q_DstLenMax)
  {
    /* Log that truncation happened */
    SHIM_MSG_MED("locQmiShimMemcpy: src buf len %d truncate to %d on line %d",\
                  q_SrcLen, q_DstLenMax, line);
  }

  q_Length = memscpy ( p_Dst, q_DstLenMax, p_Src, q_SrcLen );

  return q_Length;
}


/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to the queue library. This function is intended to 
  see if the  ShimCltInfo represented by "item_ptr" holds information for a 
  QMI-Client represented by "p_compareVal" which is also a ShimCltInfo 
  structure.
  
@param[in]   p_ItemPtr         A potential item of z_LocQmiShim.z_ClientQ
@param[in]   p_CompareVal      Qmi-Client handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*//* ------------------------------------------------------------------------*/
static int locQmiShimCompareConnectionInfoHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (p_ItemPtr == p_CompareVal) ? 1 : 0 );
}

/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to the queue library. This function is intended to 
  see if the  ShimCltInfo represented by "item_ptr" holds information for a 
  QMI-Client represented by "p_compareVal" which is 
  ShimCltInfo->p_QmiClientHandle
  
@param[in]   p_ItemPtr         A potential item of z_LocQmiShim.z_ClientQ
@param[in]   p_CompareVal      Qmi-Client handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*//* ------------------------------------------------------------------------*/
static int locQmiShimCompareQmiClientHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
  locQmiShimCltInfoStructT *pz_CltInfo = (locQmiShimCltInfoStructT*)p_ItemPtr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (pz_CltInfo->p_QmiClientHandle == p_CompareVal) ? 1 : 0 );
}


/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to the queue library. This function is intended to 
  see if the  ShimCltInfo represented by "item_ptr" holds information for a 
  Loc-Api Client Handle represented by "p_compareVal".
  
@param[in]   p_ItemPtr         A potential item of z_LocQmiShim.z_ClientQ
@param[in]   p_CompareVal      Loc-Api Client handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*//* ------------------------------------------------------------------------*/
static int locQmiShimCompareLocApiHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
  locQmiShimCltInfoStructT *pz_Item = (locQmiShimCltInfoStructT *) p_ItemPtr;
  loc_client_handle_type   z_locHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  z_locHandle = (loc_client_handle_type)p_CompareVal;
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (pz_Item->z_LocClientHandle == z_locHandle) ? 1 : 0 );
}

/* -----------------------------------------------------------------------*//**
@brief
  Function registered with TM. This function is called when TM is initialized 
  and we are notified. On receipt of this callback the QMI-LOC Service needs 
  to be initialized.

  But this function will queue an command to loc-middleware task, and 
  initialize the QMI-LOC service as part of processing the message.
  
*//* ------------------------------------------------------------------------*/
static void locQmiShimInitNotifierCb
(
  void
)
{
  loc_cmd_s_type  z_LocMwCmd; 
  boolean b_Retval = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* The only information required is the CMD Type */
  z_LocMwCmd.cmd_type = LOC_CMD_TYPE_INIT_QMI_LOC;

  b_Retval = loc_middleware_queue_cmd(&z_LocMwCmd);

  if ( FALSE == b_Retval )
  {
    SHIM_ERR_FATAL("Cannot initialize the QMI-LOC Service",0,0,0);
  }

}


/* -----------------------------------------------------------------------*//**
@brief
  A helper utility to be used by the QmiShim module when it needs to generate
  an indication in response to an incoming message. A buffer will be allocated
  and then queued. And a IPC will be queued to the Service Owner ie Loc-MW-Task


  An example would be "Get-Service-Revision", following will be the logic it 
  would be handled.

  1. QMI_ULP_GET_SERVICE_REVISION_REQ is received
  2. As part of the processing of request, an indication is queued and an IPC 
     is sent to loc-mw. 
  3. In the current execution path we send the response back. 
  4. As part of the "processing" of the IPC, we send the indication later on.

@param[in]   pz_IndInfo        Information regarding the indication that needs
                               to be sent out.
@param[out]  pe_QmiErr         if Failure, this will be populated to 
                               identify the reason for error
                               
@retval    TRUE          Indication was successfully queued
@retval    FALSE         Indication was not queued successfully 

@see locQmiShimProcessIpc(), locQmiShimProcessSerializeInd()
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimSerializeInd
( 
  const locQmiShimQueuedIndStructT    *pz_IndInfo,
  qmi_error_type_v01                  *pe_QmiErr
)  
{
  boolean v_RetVal = FALSE;
  locQmiShimQueuedIndStructT *pz_Ind = NULL;  
  loc_cmd_s_type  z_LocMwCmd; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IndInfo is filled up properly */

  pz_Ind = q_get( &z_LocQmiShim.z_IndFreeQ );
  memset ( &z_LocMwCmd, 0 , sizeof(z_LocMwCmd ) );

  if ( NULL != pz_Ind )
  {
    /* Copy the infocmtion, the Msg is same as the z_IndInfo. Do not copy
       over q_link */
    pz_Ind->q_IndId = pz_IndInfo->q_IndId;
    pz_Ind->q_IndLen = pz_IndInfo->q_IndLen;
    pz_Ind->pz_CltInfo = pz_IndInfo->pz_CltInfo;
    pz_Ind->z_IndInfo = pz_IndInfo->z_IndInfo;
    
    /* The only information required is the CMD Type */
    z_LocMwCmd.cmd_type = LOC_CMD_TYPE_SERIALIZE_IND;

    v_RetVal = loc_middleware_queue_cmd(&z_LocMwCmd);

    /* Queue up either into the IndQ or the IndFreeQ depending on the result
       of trying to queue a command to Loc MW. Its ok if the Queueing of the 
       Indication fails at this point, when the Above command is processed it
       will return immediately as nothing was found in the IndQ */
    if ( TRUE == v_RetVal )
    {
      q_put ( &z_LocQmiShim.z_IndQ, 
              &pz_Ind->z_Link );
    }
    else
    {
      memset ( pz_Ind, 0, sizeof(*pz_Ind) );
      (void) q_link ( pz_Ind, &pz_Ind->z_Link );
      q_put ( &z_LocQmiShim.z_IndFreeQ, &pz_Ind->z_Link );
      SHIM_MSG_ERROR( "Could not queue to loc-mw task, not serializing ind:0x%x",
                      pz_IndInfo->q_IndId,0,0);
      (*pe_QmiErr) = QMI_ERR_INTERNAL_V01;
    }
  }
  else
  {
    SHIM_MSG_ERROR( "No empty buffers available, not serializing ind:0x%x",
                    pz_IndInfo->q_IndId,0,0);
    (*pe_QmiErr) = QMI_ERR_NO_MEMORY_V01;
  }
  return v_RetVal; 
}

/*============================================================================
 *                    U T I L I T Y    F U N C T I O N S FOR SECURE AVAILABLE POSITION
 * =========================================================================*/

int16 bsti_endian = 1;  // Byte swap test integer
#define is_bigendian() ( (*(char*)&bsti_endian) == 0 )


/**
In-place swapping of bytes to match endianness of hardware
@param[in/out] *object : memory to swap in-place
@param[in]     _size   : length in bytes
*/
void swapbytes(void *_object, size_t _size)
{
  uint8 *start, *end;
  if(NULL == _object)
  {
    return;
  }
  if (!is_bigendian())
  {
    for (start = (uint8 *)_object, end = start + _size - 1; start < end; ++start, --end)
    {
      unsigned char swap = *start;
      * start = *end;
      * end = swap;
    }
  }
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Unswap the Float data to match the system Endianess
  
@param[in]   p_floatData      char data Pointer      

@retval    Float   Float data after swapping of bytes
*//* ------------------------------------------------------------------------*/

float FloatUnSwap(const unsigned char* p_floatData)
{
    union
    {
      float swappedFloatData;
      byte b[4];
    } data1, data2;

    data1.b[0] = p_floatData[0];
    data1.b[1] = p_floatData[1];
    data1.b[2] = p_floatData[2];
    data1.b[3] = p_floatData[3];

    data2.b[0] = data1.b[3];
    data2.b[1] = data1.b[2];
    data2.b[2] = data1.b[1];
    data2.b[3] = data1.b[0];

	return data2.swappedFloatData;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Unswap the Double data to match the system Endianess
  
@param[in]   p_doubleData      char data Pointer      

@retval    Doble	Double data after swapping of bytes
*//* ------------------------------------------------------------------------*/

double DoubleUnSwap(const unsigned char* p_doubleData)
{
    union
    {
      double swappedDoubleData;
      byte b[8];
    } data1, data2;

    data1.b[0] = p_doubleData[0];
    data1.b[1] = p_doubleData[1];
    data1.b[2] = p_doubleData[2];
    data1.b[3] = p_doubleData[3];
    data1.b[4] = p_doubleData[4];
    data1.b[5] = p_doubleData[5];
    data1.b[6] = p_doubleData[6];
    data1.b[7] = p_doubleData[7];

    data2.b[0] = data1.b[7];
    data2.b[1] = data1.b[6];
    data2.b[2] = data1.b[5];
    data2.b[3] = data1.b[4];
    data2.b[4] = data1.b[3];
    data2.b[5] = data1.b[2];
    data2.b[6] = data1.b[1];
    data2.b[7] = data1.b[0];

    return data2.swappedDoubleData;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to return the size of the data type enum used in Opaque data Blob
  
@param[in]   dataType          Enum data Type

@retval    Size                 Size of Data Type
*//* ------------------------------------------------------------------------*/

size_t sizeofDataType(qmiLocSecureMessageDataTypeEnumT_v02 dataType)
{
  size_t size= 0;
  switch (dataType)
  {
    case eQMI_LOC_SECURE_DATA_TYPE_BYTE_8_V02 :
      size = sizeof(byte);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_CHAR_8_V02 :
      size = sizeof(char);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_CHAR_8_V02 :
      size = sizeof(char);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_INT_8_V02 :
      size = sizeof(int8);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_8_V02 :
      size = sizeof(uint8);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_INT_16_V02 :
      size = sizeof(int16);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_16_V02 :
      size = sizeof(uint16);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_INT_32_V02 :
      size = sizeof(int32);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02 :
      size = sizeof(uint32);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_INT_64_V02 :
      size = sizeof(int64);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_64_V02 :
      size = sizeof(uint64);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_BOOL_8_V02 :
      size = sizeof(boolean);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_DOUBLE_64_V02 :
      size = sizeof(double);
      break;
    case eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02:
      size = sizeof(float);
      break;
    default :
      size = 0;
      break;
  }
  return size;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Extract Value from the given TLV data Blob based on the data type
  
@param[OUT]  dest                Dest buffer with decoded value
@param[in]   Paramsize           Length of  parameter
@param[in]   type                Type of Start Fix parameter
@param[in]   src               	 Data blob 


@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/

boolean ExtractValue(void *p_destBuff, size_t Paramsize, qmiLocSecureMessageDataTypeEnumT_v02 type, const uint8 *p_srcBuff)
{
  size_t size = 0;
  size_t DataTypeSize = sizeofDataType(type);
  if (NULL == p_srcBuff)
  {
    SHIM_MSG_ERROR("\np_srcBuff is NULL",0,0,0);
    return FALSE;
  }
  if (DataTypeSize != Paramsize)
  {
    SHIM_MSG_ERROR("\nExtractValue: Invalid Data Type Size ",0,0,0);
    return FALSE;
  }
  switch (type)
  {
    case eQMI_LOC_SECURE_DATA_TYPE_BYTE_8_V02:
    {
      byte *temp = (byte *)p_destBuff;
      *temp = Extract1Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_CHAR_8_V02:
    {
      char *temp = (char *)p_destBuff;
      *temp = Extract1Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_CHAR_8_V02:
    {
      char *temp = (char *)p_destBuff;
      *temp = Extract1Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_INT_8_V02:
    {
      int8 *temp = (int8 *)p_destBuff;
      *temp = Extract1Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_8_V02:
    {
      uint8 *temp = (uint8 *)p_destBuff;
      *temp = Extract1Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_INT_16_V02:
    {
      int16 *temp = (int16 *)p_destBuff;
      *temp = Extract2Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_16_V02:
    {
      uint16 *temp = (uint16 *)p_destBuff;
      *temp = Extract2Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_INT_32_V02:
    {
      int32 *temp = (int32 *)p_destBuff;
      *temp = Extract4Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02:
    {
      uint32 *temp = (uint32 *)p_destBuff;
      *temp = Extract4Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_INT_64_V02:
    {
      int64 *temp1 = (int64 *)p_destBuff;
      int64 temp = 0;
      const unsigned char *t1 = NULL;
      t1 = p_srcBuff + 4;
      temp = (((temp & 0x0000000000000000) | (Extract4Bytes(p_srcBuff))) << 32) | (Extract4Bytes(t1));
      *temp1 = temp;
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_64_V02:
    {
      uint64 *temp1 = (uint64 *)p_destBuff;
      uint64 temp = 0;
      const unsigned char *t1 = NULL;
      t1 = p_srcBuff + 4;
      temp = (((temp & 0x0000000000000000) | (Extract4Bytes(p_srcBuff))) << 32) | (Extract4Bytes(t1));
      *temp1 = temp;
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_BOOL_8_V02:
    {
      boolean *temp = (boolean*)p_destBuff;
      *temp = Extract4Bytes(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_DOUBLE_64_V02:
    {
      double *temp = (double *)p_destBuff;
      *temp = DoubleUnSwap(p_srcBuff);
      break;
    }
    case eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02:
    {
      float *temp = (float *)p_destBuff;
      *temp = FloatUnSwap(p_srcBuff);
      break;
    }
    default :
      return FALSE;
}
  return TRUE;
}

/* =========================================================================*/

/*============================================================================
 *                    M E S S A G E    C O N V E R T O R S 
 *                         QMI Loc --> LOC-Api
 * =========================================================================*/

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert NI Notification Type from QMI-Loc to Loc-Api 
  
@param[in]    e_QmiNiNotification               Loc-API IOCTL Format 
@param[out]   e_LocNiNotification               Qmi-Loc Format of Message

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocNiNotificationType
(
  qmiLocNiNotifyVerifyEnumT_v02       e_QmiNiNotification,
  loc_ni_notify_verify_e_type         *e_LocNiNotification
)
{
  boolean v_RetVal = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  switch ( e_QmiNiNotification )
  {
    case eQMI_LOC_NI_USER_NO_NOTIFY_NO_VERIFY_V02:
    {
      (*e_LocNiNotification) = 
                              LOC_NI_USER_NO_NOTIFY_NO_VERIFY;
    }
    break;
    case eQMI_LOC_NI_USER_NOTIFY_ONLY_V02:
    {
      (*e_LocNiNotification) = LOC_NI_USER_NOTIFY_ONLY;
    }
    break;
    case eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02:
    {
      (*e_LocNiNotification) = 
                        LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
    }
    break;
    case eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02:
    {
      (*e_LocNiNotification) = 
                    LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
    }
    break;
    case eQMI_LOC_NI_USER_NOTIFY_VERIFY_PRIVACY_OVERRIDE_V02:
    {
      
      (*e_LocNiNotification) = 
                    LOC_NI_USER_PRIVACY_OVERRIDE;
    }
    break;
    
    
  
    default:
    {
      v_RetVal = FALSE;
      SHIM_MSG_ERROR( "Invalid Option: %d", 
                      e_QmiNiNotification, 0, 0 );
    }  
    break;
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert NI Format Type from QMI-Loc to Loc-Api 
  
@param[in]    e_QmiNiFormatType             Loc-API IOCTL Format 
@param[out]   q_LocFormatType               Qmi-Loc Format of Message

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocNiFormatType
(
  qmiLocNiSuplFormatEnumType_v02      e_QmiNiFormatType,
  unsigned char                      *q_LocFormatType
)
{
  boolean v_RetVal = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
     
  switch ( e_QmiNiFormatType )
  {
    case eQMI_LOC_NI_SUPL_FORMAT_LOGICAL_NAME_V02:
      (*q_LocFormatType) = 
                  LOC_NI_SUPL_FORMAT_LOGICAL_NAME;
      break;
    case eQMI_LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS_V02:
      (*q_LocFormatType) = 
                  LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS;
      break;
    case eQMI_LOC_NI_SUPL_FORMAT_MSISDN_V02:
      (*q_LocFormatType) = 
                  LOC_NI_SUPL_FORMAT_MSISDN;
      break;
    case eQMI_LOC_NI_SUPL_FORMAT_URL_V02:
      (*q_LocFormatType) = 
                  LOC_NI_SUPL_FORMAT_URL;
      break;
    case eQMI_LOC_NI_SUPL_FORMAT_SIP_URL_V02:
      (*q_LocFormatType) = 
                  LOC_NI_SUPL_FORMAT_SIP_URL;
      break;
    case eQMI_LOC_NI_SUPL_FORMAT_MIN_V02:
      (*q_LocFormatType) = 
                  LOC_NI_SUPL_FORMAT_MIN;
      break;
    case eQMI_LOC_NI_SUPL_FORMAT_MDN_V02:
      (*q_LocFormatType) = 
                  LOC_NI_SUPL_FORMAT_MDN;
      break;
    case eQMI_LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY_V02:
      (*q_LocFormatType) =
                  LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY;
      break;
    case eQMI_LOC_NI_SUPL_FORMAT_OSS_UNKNOWN_V02:
      /* Unfortunate to do it, but loc-api needs an unsigned char */
      (*q_LocFormatType) = 
                  (unsigned char)LOC_NI_SUPL_FORMAT_OSS_UNKNOWN;
      break;
      
    default:
    {
      v_RetVal = FALSE;
      SHIM_MSG_ERROR( "Invalid Option: %d", 
                      e_QmiNiFormatType, 
                      0, 0 );
    }  
    break;
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert NI Data Coding Scheme from QMI-Loc to Loc-Api
  
@param[in]    e_QmiNiDataCodingScheme             Qmi-Loc Format of Message
@param[out]   e_LocDataCodingScheme               Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocNiDataCodingScheme
(
  qmiLocNiDataCodingSchemeEnumT_v02    e_QmiNiDataCodingScheme,
  loc_ni_datacoding_scheme_e_type     *e_LocDataCodingScheme
)
{
  boolean v_RetVal = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (e_QmiNiDataCodingScheme )
  { 
    case eQMI_LOC_NI_SS_GERMAN_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_GERMAN;
      break;
    case eQMI_LOC_NI_SS_ENGLISH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_ENGLISH;
      break;
    case eQMI_LOC_NI_SS_ITALIAN_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_ITALIAN;
      break;
    case eQMI_LOC_NI_SS_FRENCH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_FRENCH;
      break;
    case eQMI_LOC_NI_SS_SPANISH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_SPANISH;
      break;
    case eQMI_LOC_NI_SS_DUTCH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_DUTCH;
      break;
    case eQMI_LOC_NI_SS_SWEDISH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_SWEDISH;
      break;
    case eQMI_LOC_NI_SS_DANISH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_DANISH;
      break;
    case eQMI_LOC_NI_SS_PORTUGUESE_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_PORTUGUESE;
      break;
    case eQMI_LOC_NI_SS_FINNISH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_FINNISH;
      break;
    case eQMI_LOC_NI_SS_NORWEGIAN_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_NORWEGIAN;
      break;
    case eQMI_LOC_NI_SS_GREEK_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_GREEK;
      break;
    case eQMI_LOC_NI_SS_TURKISH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_TURKISH;
      break;
    case eQMI_LOC_NI_SS_HUNGARIAN_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_HUNGARIAN;
      break;
    case eQMI_LOC_NI_SS_POLISH_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_POLISH;
      break;
    case eQMI_LOC_NI_SS_LANGUAGE_UNSPEC_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SS_LANGUAGE_UNSPEC;
      break;
    case eQMI_LOC_NI_SUPL_UTF8_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SUPL_UTF8;
      break;
    case eQMI_LOC_NI_SUPL_UCS2_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SUPL_UCS2;
      break;
    case eQMI_LOC_NI_SUPL_GSM_DEFAULT_V02:
      (*e_LocDataCodingScheme) = 
                  LOC_NI_SUPL_GSM_DEFAULT;
      break;

    default:
    {
      v_RetVal = FALSE;
      SHIM_MSG_ERROR( "Invalid Option: %d", 
                      e_QmiNiDataCodingScheme, 0, 0 );
    }  
    break;
  }

  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert NI Vx Notify Verify Message Qmi-Loc to Loc-Api
  
@param[in]   pz_QmiLoc             Loc-API IOCTL Format 
@param[out]  pz_NiVx               Qmi-Loc Format of Message

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocNiVxNotifyVerify
(
  const qmiLocNiVxNotifyVerifyStructT_v02          *pz_QmiLoc,
  loc_ni_vx_notify_verify_req_s_type               *pz_NiVx
)
{
  boolean v_RetVal = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( TRUE == v_RetVal ) 
  {
    switch ( pz_QmiLoc->posMode)
    {
      case eQMI_LOC_NI_VX_MS_ASSISTED_ONLY_V02:
      {
        pz_NiVx->pos_mode = 
                                LOC_VX_MS_ASSISTED_ONLY;
      }
      break;
      case eQMI_LOC_NI_VX_MS_BASED_ONLY_V02:
      {
        pz_NiVx->pos_mode = LOC_VX_MS_BASED_ONLY;
      }
      break;
      case eQMI_LOC_NI_VX_MS_ASSISTED_PREFERRED_MS_BASED_ALLOWED_V02:
      {
        pz_NiVx->pos_mode = 
               LOC_VX_MS_ASSISTED_PREF_MSBASED_ALLWD;
      }
      break;
      case eQMI_LOC_NI_VX_MS_BASED_PREFERRED_MS_ASSISTED_ALLOWED_V02:
      {
        pz_NiVx->pos_mode = 
               LOC_VX_MS_BASED_PREF_ASSISTED_ALLWD;
      }
      break;
  
      default:
      {
        v_RetVal = FALSE;
      }  
      break;
    }          
  }
  
  if ( TRUE == v_RetVal ) 
  {
    switch ( pz_QmiLoc->encodingScheme )
    {
      case eQMI_LOC_NI_VX_OCTET_V02:
      {
        pz_NiVx->encoding_scheme = 
                                LOC_NI_VX_OCTET;
      }
      break;
      case eQMI_LOC_NI_VX_EXN_PROTOCOL_MSG_V02:
      {
        pz_NiVx->encoding_scheme = LOC_NI_VX_EXN_PROTOCOL_MSG;
      }
      break;
      case eQMI_LOC_NI_VX_ASCII_V02:
      {
        pz_NiVx->encoding_scheme = 
               LOC_NI_VX_ASCII;
      }
      break;
      case eQMI_LOC_NI_VX_IA5_V02:
      {
        pz_NiVx->encoding_scheme = 
               LOC_NI_VX_IA5;
      }
      break;
      case eQMI_LOC_NI_VX_UNICODE_V02:
      {
        pz_NiVx->encoding_scheme = 
               LOC_NI_VX_UNICODE;
      }
      break;
      case eQMI_LOC_NI_VX_SHIFT_JIS_V02:
      {
        pz_NiVx->encoding_scheme = 
               LOC_NI_VX_SHIFT_JIS;
      }
      break;
      case eQMI_LOC_NI_VX_KOREAN_V02:
      {
        pz_NiVx->encoding_scheme = 
               LOC_NI_VX_KOREAN;
      }
      break;
      case eQMI_LOC_NI_VX_LATIN_HEBREW_V02:
      {
        pz_NiVx->encoding_scheme = 
               LOC_NI_VX_LATIN_HEBREW;
      }
      break;
      case eQMI_LOC_NI_VX_LATIN_V02:
      {
        pz_NiVx->encoding_scheme = 
               LOC_NI_VX_LATIN;
      }
      break;
      case eQMI_LOC_NI_VX_GSM_V02:
      {
        pz_NiVx->encoding_scheme = 
               LOC_NI_VX_GSM;
      }
      break;
  
      default:
      {
        v_RetVal = FALSE;
      }  
      break;
    }          
  }
  
  if ( TRUE == locQmiShimIsValidBoolean(pz_QmiLoc->posQosIncl) )
  {
    pz_NiVx->pos_qos_incl       = pz_QmiLoc->posQosIncl;
  }
  else
  {
    v_RetVal = FALSE;
  }
  
  if ( TRUE == v_RetVal )
  {

    pz_NiVx->requester_id.requester_id_length = (unsigned char ) locQmiShimMemcpy(
                      pz_QmiLoc->requestorId_len,
                      (void*)pz_QmiLoc->requestorId,
                      LOC_NI_MAX_REQUESTOR_ID_LENGTH,
                      (void*)pz_NiVx->requester_id.requester_id );
                      

    pz_NiVx->pos_qos            = pz_QmiLoc->posQos;
    pz_NiVx->num_fixes          = pz_QmiLoc->numFixes;
    pz_NiVx->tbf                = pz_QmiLoc->timeBetweenFixes;
    pz_NiVx->user_resp_timer_val= pz_QmiLoc->userRespTimerInSeconds;
  }
  
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert RegEventMask from QMI-Loc to Loc-Api
  
@param[in]   pz_Msg               Qmi-Loc Format of Reg Event Mask
@param[out]  pz_IoctlInfo         Loc-Api Format of Reg Event Mask

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully

@warning   This is an extended IOCTL introduced for QMI-LOC, and hence the 
           "reserved" data field in the ioctl is used. This function data-ptr
           and data-size accordingly.
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetEventRegMask
(
  const void * pz_Msg,
  loc_ioctl_data_u_type * pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  loc_event_mask_type z_LocMsg = 0; /* It is a uint64 */
  uint8 ctr = 0;
  const qmiLocRegEventsReqMsgT_v02  *pz_QmiLoc = 
                         (qmiLocRegEventsReqMsgT_v02*) pz_Msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
  for ( ctr = 0;
        ctr < ( ( sizeof( z_LocQmiShimMaskMap ) ) / 
                          ( sizeof( z_LocQmiShimMaskMap[0] ) ) ) ;
        ctr ++ )
  {
    if ( z_LocQmiShimMaskMap[ctr].t_QmiMask & pz_QmiLoc->eventRegMask )
    {
      z_LocMsg |= z_LocQmiShimMaskMap[ctr].t_LocMask;
    }
  }

  /* Include the IOCTL REPORT */      
  pz_IoctlInfo->event_reg_mask = ( z_LocMsg | 
                                   LOC_EVENT_IOCTL_REPORT  );

  return v_RetVal; 
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to fill Set Fix Criteria from Start Req Mesage
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetFixCriteria
(
  const void                      *pz_Msg,
  loc_ioctl_data_u_type           *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  
  const qmiLocStartReqMsgT_v02    *pz_StartReq = 
                                  (qmiLocStartReqMsgT_v02*)pz_Msg;  
  loc_fix_criteria_s_type         *pz_IoctlData = 
                                  &(pz_IoctlInfo->fix_criteria);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  /* Set validity mask to 0 */
  pz_IoctlData->valid_mask = 0;

  if ( pz_StartReq->fixRecurrence_valid )
  {
    pz_IoctlData->valid_mask |= LOC_FIX_CRIT_VALID_RECURRENCE_TYPE;
    switch ( pz_StartReq->fixRecurrence )
    {
      case eQMI_LOC_RECURRENCE_PERIODIC_V02:
        pz_IoctlData->recurrence_type = LOC_PERIODIC_FIX;
        break;
      case eQMI_LOC_RECURRENCE_SINGLE_V02:
        pz_IoctlData->recurrence_type = LOC_SINGLE_FIX;
        break;
      default:
        /* Invalid input, rejecting */
         v_RetVal = FALSE;
        break;
    }
  }

  if ( ( TRUE == v_RetVal ) &&
       ( pz_StartReq->horizontalAccuracyLevel_valid ) )
  {
    pz_IoctlData->valid_mask |= LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY;

    if ( eQMI_LOC_ACCURACY_LOW_V02 == pz_StartReq->horizontalAccuracyLevel )
    {
      pz_IoctlData->preferred_accuracy = LOC_ACCURACY_LOW; /* in meters */
    }
    else if ( eQMI_LOC_ACCURACY_MED_V02 == 
                                    pz_StartReq->horizontalAccuracyLevel )
    {
      pz_IoctlData->preferred_accuracy = LOC_ACCURACY_MED; /* in meters */
    }
    else if ( eQMI_LOC_ACCURACY_HIGH_V02 == 
                                    pz_StartReq->horizontalAccuracyLevel )
    {
      pz_IoctlData->preferred_accuracy = LOC_ACCURACY_HIGH; /* in meters */
    }
    else
    {
      /* Invalid input, rejecting */
       v_RetVal = FALSE;
    }
  }

  /* There are other fields in the fix criteria, but are not exposed 
     through QMI and hence not filled, assuming the defaults will be used by 
     Loc-MW. */ 

  if ( TRUE == v_RetVal )
  {
    /*  The control point must explicitly set this field to OFF if it
        does not wish to receive intermediate position reports*/
    pz_IoctlData->valid_mask |= 
              LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED;
    pz_IoctlData->intermediate_pos_report_enabled = TRUE;

    if( TRUE == pz_StartReq->intermediateReportState_valid && 
        eQMI_LOC_INTERMEDIATE_REPORTS_OFF_V02 ==
                 pz_StartReq->intermediateReportState)
    {
      pz_IoctlData->intermediate_pos_report_enabled = FALSE;
    }
  }

  if ( ( TRUE == v_RetVal ) &&
       ( pz_StartReq->minInterval_valid ) )
  {
    pz_IoctlData->valid_mask |= LOC_FIX_CRIT_VALID_MIN_INTERVAL;
    pz_IoctlData->min_interval = locQmiShimMatchFixInterval(
                                                     pz_StartReq->minInterval);
                                                    /* in milliseconds */
  }

  /* copy application Id*/
  if ( ( TRUE == v_RetVal ) &&
       ( pz_StartReq->applicationId_valid ) )
  {
    /* set the application valid mask*/
    pz_IoctlData->valid_mask |= LOC_FIX_CRIT_VALID_APPLICATION_ID;

    /* copy the application Id to fix criteria structure*/
    locQmiShimMemcpy(sizeof(pz_StartReq->applicationId),
                     (void*)&pz_StartReq->applicationId, //source
                     sizeof( pz_IoctlData->application_id),
                     (void *)&pz_IoctlData->application_id); //dst

    
  }
  
  /* Set session timeout*/
  if ( TRUE == v_RetVal )
  {
    pz_IoctlData->valid_mask |= LOC_FIX_CRIT_VALID_PREFERRED_RESPONSE_TIME;
    if( pz_StartReq->positionReportTimeout_valid )
    {
      if ( LOC_MIN_RESPONSE_TIME > pz_StartReq->positionReportTimeout ||
           LOC_DEFAULT_RESPONSE_TIME < pz_StartReq->positionReportTimeout)
      {
        LOC_MSG_ERROR("locQmiShimConvToLocSetFixCriteria invalid positionReportTimeout=%u",
                     pz_StartReq->positionReportTimeout, 0,0);
        v_RetVal = FALSE;
      }
      else
      {
        pz_IoctlData->preferred_response_time = pz_StartReq->positionReportTimeout;
      }
    }
    else
    {
      pz_IoctlData->preferred_response_time = LOC_DEFAULT_RESPONSE_TIME; /* 255 seconds */
    }
  }

  /* between intermediate position reports elapsed time must longer than the interval time.*/
  pz_IoctlData->min_intermediate_interval = 0;
  if ( ( TRUE == v_RetVal ) &&
       ( pz_StartReq->minIntermediatePositionReportInterval_valid ) )
  {
    pz_IoctlData->min_intermediate_interval = 
        pz_StartReq->minIntermediatePositionReportInterval;
    LOC_MSG_MED("locQmiShimConvToLocSetFixCriteria min_intermediate_interval=%u",
                 pz_StartReq->minIntermediatePositionReportInterval, 0,0);
  }

  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to decode the data blob of GetAvailablePosition Request in Internal Structure.
  
@param[in]   src                Input Data Blob
@param[in]   src_length             Length of Data Blob

@param[out]  pz_DecodedGetAvailablePosReq         GetAvailable Position request parameters after decoding

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/

static boolean locQmiShimPopulateLocGetSecureAvailablePositionParam(
                const uint8 *p_srcDataBlob,uint32 src_length,qmiLocGetAvailablePositionReqParamMsgT_v02 *pz_DecodedGetAvailablePosReq  )
{
  boolean v_RetVal = TRUE;
  qmiLocSecureGetAvailablePosParameterIDEnumT_v02 paramId;
  qmiLocSecureMessageDataTypeEnumT_v02 paramType;
  uint8 num_of_data_type = 0;
  uint32 paramBloblength = 0;
  uint32 byte_count = 0;
  SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam: data_length passed in GetAvailablePositionReq param %d ",src_length,0, 0);

  for(byte_count = 0; byte_count < src_length; byte_count += paramBloblength)
  {
    uint32 cur_byteCount = byte_count;
    size_t DataTypeSize;
    paramBloblength = 0;

    // Verify if Parametr , Type and Length data is present to decode the data blob for this parameter
    if ((cur_byteCount + sizeof(qmiLocSecureGetAvailablePosParameterIDEnumT_v02)
      +sizeof(qmiLocSecureMessageDataTypeEnumT_v02)
      +sizeof(uint8)) >= src_length)
    {
      SHIM_MSG_MED("\nDecodePositionRptIndicationParam: Data Blob doesnt have Parametr, Type and Length data to decode",0,0,0);
      return FALSE;
    }
    // Decode Parameter Blob
    paramId = (qmiLocSecureGetAvailablePosParameterIDEnumT_v02) (Extract4Bytes(p_srcDataBlob));
    p_srcDataBlob = p_srcDataBlob + sizeof(qmiLocSecureGetAvailablePosParameterIDEnumT_v02);
    SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam: ParamID %d ",paramId,0, 0);
    paramBloblength += sizeof(qmiLocSecureGetAvailablePosParameterIDEnumT_v02);

    // Decode data Type Blob
    paramType = (qmiLocSecureMessageDataTypeEnumT_v02)(Extract4Bytes(p_srcDataBlob));
    p_srcDataBlob = p_srcDataBlob + sizeof(qmiLocSecureMessageDataTypeEnumT_v02);
    SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam: Param Data Type %d ",paramType,0, 0);
    paramBloblength += sizeof(qmiLocSecureMessageDataTypeEnumT_v02);

    // Length of Data Type
    num_of_data_type = Extract1Bytes(p_srcDataBlob); 
    p_srcDataBlob = p_srcDataBlob + sizeof(uint8);
    SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam:Param Data Type Number %d ",num_of_data_type,0, 0);
    paramBloblength += sizeof(uint8);

    DataTypeSize = sizeofDataType(paramType);

    // Verify if data is enough to decode the data blob for this parameter
    if ((cur_byteCount + paramBloblength
      + (DataTypeSize  *num_of_data_type ) ) > src_length)
    {
      SHIM_MSG_ERROR("locQmiShimPopulateLocGetSecureAvailablePositionParam : Data Blob doesnt have enough data to decode",0,0,0);
      return FALSE;
    }

    if(num_of_data_type == DATA_TYPE_LENGTH_ZERO)
    {
      // This Type Field is not valid.
      // can Ignore it and continue
      // generally it should not happen, because if Field is not valid, Client should not put the param ID in the data buffer
      continue;
    }
    SHIM_MSG_MED("locQmiShimPopulateLocGetSecureAvailablePositionParam :Processing param ID %d ", paramId,0,0);
    switch(paramId)
    {
      case eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_TRANSACTION_ID_V02 :
        if(num_of_data_type == DATA_TYPE_LENGTH_ONE)
        {
          v_RetVal =ExtractValue(&pz_DecodedGetAvailablePosReq->transactionId,
            sizeof(pz_DecodedGetAvailablePosReq->transactionId), paramType, p_srcDataBlob);
          SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam: transactionId value %d ",
            pz_DecodedGetAvailablePosReq->transactionId,0, 0);
        }
        else
        {
          v_RetVal =  FALSE;
        }
        break;
      case eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_NONCE_V02:
        if(num_of_data_type == DATA_TYPE_LENGTH_ONE)
        { 
          v_RetVal =ExtractValue(&pz_DecodedGetAvailablePosReq->nonce, sizeof(pz_DecodedGetAvailablePosReq->nonce), paramType, p_srcDataBlob);
          pz_DecodedGetAvailablePosReq->nonce_valid = TRUE;
          SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam: nonce value %016llX ",pz_DecodedGetAvailablePosReq->nonce,0, 0);
        }
        else
        {
          v_RetVal =  FALSE;
        }
        break;
      case eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_SOURCE_V02:
        if(num_of_data_type == DATA_TYPE_LENGTH_ONE)
        {
          uint32 p_tempSrcDataBlob = 0;
          v_RetVal =ExtractValue(&p_tempSrcDataBlob, sizeof(pz_DecodedGetAvailablePosReq->locPositionSource), paramType, p_srcDataBlob);
          pz_DecodedGetAvailablePosReq->locPositionSource_valid = TRUE;
          if(p_tempSrcDataBlob == eQMI_LOC_POS_SRC_GNSS_V02)
          {
            pz_DecodedGetAvailablePosReq->locPositionSource= eQMI_LOC_POS_SRC_GNSS_V02;
          }
          else if(p_tempSrcDataBlob == 0x02)
          {
            pz_DecodedGetAvailablePosReq->locPositionSource= 0x02;
          }
          else
          {
            //Wrong Input Data
            v_RetVal =  FALSE;
          }
          SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam: locPositionSource %d ",pz_DecodedGetAvailablePosReq->locPositionSource,0, 0);
        }
        else
        {
          v_RetVal =  FALSE;
        }
        break;
      case eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_REPORT_DATA_SECURITY_MODE_V02:
        if(num_of_data_type == DATA_TYPE_LENGTH_ONE)
        {
          uint32 p_tempSrcDataBlob = 0;
          v_RetVal =ExtractValue(&p_tempSrcDataBlob, sizeof(pz_DecodedGetAvailablePosReq->locPositionRptDataMode), paramType, p_srcDataBlob);
          pz_DecodedGetAvailablePosReq->locPositionRptDataMode_valid= TRUE;
          if(p_tempSrcDataBlob == eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED_V02)
          {
            pz_DecodedGetAvailablePosReq->locPositionRptDataMode= eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED_V02;
          }
          else if(p_tempSrcDataBlob == eQMI_LOC_SECURE_LOC_DATA_UNENCRYPTED_V02)
          {
            pz_DecodedGetAvailablePosReq->locPositionRptDataMode= eQMI_LOC_SECURE_LOC_DATA_UNENCRYPTED_V02;
          }
          else
          {
            //Wrong Input Data
            v_RetVal =  FALSE;
          }
          SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam: locPositionRptDataMode value %d ",pz_DecodedGetAvailablePosReq->locPositionRptDataMode,0, 0);
        }
        else
        {
          v_RetVal =  FALSE;
        }
        break;
      case eQMI_LOC_SECURE_GET_AVAILABLE_POS_PARAM_REPORT_DATA_TIME_PROPAGATION_V02:
        if(num_of_data_type == DATA_TYPE_LENGTH_ONE)
        {
          uint32 p_tempSrcDataBlob = 0;
          v_RetVal =ExtractValue(&p_tempSrcDataBlob, sizeof(pz_DecodedGetAvailablePosReq->locPositionRptDataTimePropogation), paramType, p_srcDataBlob);

          pz_DecodedGetAvailablePosReq->locPositionRptDataTimePropogation_valid= TRUE;
          if(p_tempSrcDataBlob == eQMI_LOC_POS_REPORT_PROPAGATED_TO_CURRENT_UTC_V02)
          {
            pz_DecodedGetAvailablePosReq->locPositionRptDataTimePropogation= eQMI_LOC_POS_REPORT_PROPAGATED_TO_CURRENT_UTC_V02;
          }
          else
          {
            //Wrong Input Data
            v_RetVal =  FALSE;
          }
          SHIM_MSG_MED( "locQmiShimPopulateLocGetSecureAvailablePositionParam: locPositionRptDataTimePropogation value %d ",pz_DecodedGetAvailablePosReq->locPositionRptDataTimePropogation,0, 0);
        }
        else
        {
          v_RetVal =  FALSE;
        }
        break;
      default:
      {
        v_RetVal =  FALSE;
      }
        break;
    }
    if (v_RetVal == FALSE)
    {
      return v_RetVal;
    }
    else
    {
      paramBloblength += (DataTypeSize * num_of_data_type);
      if (num_of_data_type == DATA_TYPE_LENGTH_ONE)
      {
        p_srcDataBlob = p_srcDataBlob + DataTypeSize;
      }
      else
      {
        // p_srcDataBlob increment is taken care while extracting data from array
        ;
      }
    }
}

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  function to Decrypt  the Data Blob, using Secure API
  
@param[in]   pz_EncryptedMsg           Qmi-Loc Format of Message having Encrypted Packet
@param[out]  pz_DecryptedMsg         Decrypted Data Blob 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/


static boolean locQmiShimDecryptSecureAvailablePositionDataBlob(const void * pz_EncryptedMsg,void * pz_DecryptedMsg)
{
  boolean v_RetVal = TRUE;
  secerrno_enum_type status = E_SUCCESS;

  qmiLocSecureGetAvailablePositionReqMsgT_v02    *pz_EncGetAvailablePosReq = 
    (qmiLocSecureGetAvailablePositionReqMsgT_v02*)pz_EncryptedMsg;  

  qmiLocSecureGetAvailablePositionReqMsgT_v02    *pz_DecGetAvailablePosReq = 
    (qmiLocSecureGetAvailablePositionReqMsgT_v02*)pz_DecryptedMsg;  

#if 0
  SHIM_MSG_MED("[DEBUG] Before Encryption Data Length %d", pz_EncGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len,0, 0);
    for(uint32 j = 0 ;j<pz_EncGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len ;j++)
      SHIM_MSG_MED("[DEBUG] Before Encryption Data %d",  pz_EncGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData[j], 0,0);
#endif

    // Decrypt this
    //pz_GetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData
  SHIM_MSG_ERROR( "locQmiShimDecryptSecureAvailablePositionDataBlob Secure Message Decryption  ", 0, 0, 0 );
  do
  {
    status = secapi_secure_channel_init( );
    if (status != E_SUCCESS && status != E_ALREADY_DONE)
    {
      SHIM_MSG_MED("secure channel init failed", 0, 0, 0);
      v_RetVal = FALSE;
      break;
    }
    // Initialize the Output data length
    pz_DecGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len =
      pz_EncGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len;
    status = secapi_authenticate_decrypt_message( SC_SSID_TZ,
      SC_CID_TLOC,(uint8*)pz_EncGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData,
      pz_EncGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len,
      (uint8 *)pz_DecGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData,
      &pz_DecGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len);

  if (E_SUCCESS != status)
    {
      SHIM_MSG_MED("authenticate and decrypt for Get Secure Location Message failed. Status %d", status, 0, 0);
      v_RetVal = FALSE;
      break;
    }
    SHIM_MSG_MED("authenticate and decrypt for Get Secure Location Message Passed. qmilocSecureGetAvailablePositionRequestData_len %d", pz_DecGetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len, 0, 0);  
  } while (0);

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  function to  Parse the Data Blob, verify and  fill in the internal structure for Get Secure Available Position
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocGetSecureAvailablePosition
(
  const void                      *pz_Msg,
  loc_ioctl_data_u_type           *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSecureGetAvailablePositionReqMsgT_v02    *pz_GetAvailablePosReq = 
   (qmiLocSecureGetAvailablePositionReqMsgT_v02*)pz_Msg;  
  qmiLocGetAvailablePositionReqParamMsgT_v02   *pz_IoctlData = 
   &(pz_IoctlInfo->z_GetAvailablePosReq);

  qmiLocGetAvailablePositionReqParamMsgT_v02 *pz_DecodedGetAvailablePosReq = 
    (qmiLocGetAvailablePositionReqParamMsgT_v02*) os_MemAlloc( 
    sizeof(qmiLocGetAvailablePositionReqParamMsgT_v02),OS_MEM_SCOPE_TASK );
  if(pz_DecodedGetAvailablePosReq == NULL)
  {
    SHIM_MSG_ERROR( "locQmiShimConvToLocGetSecureAvailablePosition: memory Allocation failed",0, 0, 0);
    v_RetVal =  FALSE;
    return v_RetVal;
  }
  memset((void*)pz_DecodedGetAvailablePosReq, 0, sizeof(qmiLocGetAvailablePositionReqParamMsgT_v02));
  SHIM_MSG_MED( "locQmiShimConvToLocGetSecureAvailablePosition: pz_GetAvailablePosReq->secureLocDataMode = %d",
     pz_GetAvailablePosReq->secureLocDataMode, 0, 0);

  #if 0  
   // Enable only for Debugging
   // Message is in TLV format
   //   PARAMETER TYPE LENGTH VALUE  ...........PARAMETER TYPE LENGTH  VALUE

   const uint8 *SSdata = pz_GetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData;
  
    for(uint32 count = 0; count < pz_GetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len ; count++)
    {
    
       SHIM_MSG_MED( "locQmiShimConvToLocGetSecureAvailablePosition data:  0x%x ",*SSdata,0, 0);
       SSdata++;
    }
  #endif
  v_RetVal = locQmiShimPopulateLocGetSecureAvailablePositionParam(&pz_GetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData[0],
               pz_GetAvailablePosReq->qmilocSecureGetAvailablePositionRequestData_len,pz_DecodedGetAvailablePosReq);

  if(( TRUE == v_RetVal ) && !pz_DecodedGetAvailablePosReq->nonce_valid && (pz_GetAvailablePosReq->secureLocDataMode == eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED_V02 ))
  {
    // Start param data blob doesnt have Nonce or Session ID.
    // return a Error Response
    SHIM_MSG_MED( "locQmiShimConvToLocSetSecureFixCriteria: Secure Start param data blob doesnt have Nonce or Session ID.",0,0, 0);
    v_RetVal = FALSE;
  }

  if(( TRUE == v_RetVal ) )
  {
    pz_IoctlData->transactionId = pz_DecodedGetAvailablePosReq->transactionId;
  }

  if ( ( TRUE == v_RetVal ) && pz_DecodedGetAvailablePosReq->nonce_valid )
  {
    pz_IoctlData->nonce = pz_DecodedGetAvailablePosReq->nonce;
    pz_IoctlData->nonce_valid = TRUE;
  }

  if ( ( TRUE == v_RetVal ) && ( pz_DecodedGetAvailablePosReq->locPositionSource_valid ) )
  {
    pz_IoctlData->locPositionSource = pz_DecodedGetAvailablePosReq->locPositionSource;
    pz_IoctlData->locPositionSource_valid = TRUE;
  }
  if ( ( TRUE == v_RetVal ) && ( pz_DecodedGetAvailablePosReq->locPositionRptDataMode_valid ) )
  {
    pz_IoctlData->locPositionRptDataMode = pz_DecodedGetAvailablePosReq->locPositionRptDataMode;
    pz_IoctlData->locPositionRptDataMode_valid = TRUE;
  }
  if ( ( TRUE == v_RetVal ) && ( pz_DecodedGetAvailablePosReq->locPositionRptDataTimePropogation_valid ) )
  {
    pz_IoctlData->locPositionRptDataTimePropogation = pz_DecodedGetAvailablePosReq->locPositionRptDataTimePropogation;
    pz_IoctlData->locPositionRptDataTimePropogation_valid = TRUE;
  }
  if( TRUE == v_RetVal )
  {
    SHIM_MSG_MED( "locQmiShimConvToLocGetSecureAvailablePosition : *********Secure Get Available request param. ***********",0,0, 0); 
    SHIM_MSG_MED( "locQmiShimConvToLocGetSecureAvailablePosition : transactionId %d ",pz_IoctlData->transactionId,0, 0); 
   // SHIM_MSG_MED( "locQmiShimConvToLocGetSecureAvailablePosition : nonce_valid %d  nonce %d ",pz_IoctlData->nonce_valid,pz_IoctlData->nonce, 0); 
    SHIM_MSG_MED( "locQmiShimConvToLocGetSecureAvailablePosition : locPositionSource_valid %d  locPositionSource %d ",pz_IoctlData->locPositionSource_valid,pz_IoctlData->locPositionSource, 0); 
    SHIM_MSG_MED( "locQmiShimConvToLocGetSecureAvailablePosition : locPositionRptDataMode_valid %d  locPositionRptDataMode %d ",pz_IoctlData->locPositionRptDataMode_valid,pz_IoctlData->locPositionRptDataMode, 0); 
    SHIM_MSG_MED( "locQmiShimConvToLocGetSecureAvailablePosition : locPositionRptDataTimePropogation_valid %d  locPositionRptDataTimePropogation %d ",pz_IoctlData->locPositionRptDataTimePropogation_valid,pz_IoctlData->locPositionRptDataTimePropogation, 0); 
  }
  if (NULL != pz_DecodedGetAvailablePosReq)
  {
    os_MemFree((void **)&pz_DecodedGetAvailablePosReq);
  }
  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Predicted Orbits Data
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocInjectPredictedOrbitsData
(
  const void                                         *pz_Msg,
  loc_ioctl_data_u_type                              *pz_IoctlInfo
)
{
  boolean v_RetVal = FALSE;
  
  const qmiLocInjectPredictedOrbitsDataReqMsgT_v02  *pz_OrbitsData = 
                    (qmiLocInjectPredictedOrbitsDataReqMsgT_v02*)pz_Msg;
  loc_predicted_orbits_data_s_type   *pz_IoctlData =  
                                  &(pz_IoctlInfo->predicted_orbits_data);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed */

  /* Injection can be accepted only if data is less than what was defined */
  if ( pz_OrbitsData->partData_len <= 
          QMI_LOC_MAX_PREDICTED_ORBITS_PART_LEN_V02 )
  {
    /* Setting to XTRA by default, since this is the only option, and if 
       the TLV is not given we have to set to default */
    pz_IoctlData->format_type = LOC_PREDICTED_ORBITS_XTRA;
    pz_IoctlData->total_size  = pz_OrbitsData->totalSize;
    /* Assigning uint16 to uint8 */
    pz_IoctlData->total_parts = (uint8) pz_OrbitsData->totalParts;
    pz_IoctlData->part        = (uint8) pz_OrbitsData->partNum;
    pz_IoctlData->part_len    = (uint16) pz_OrbitsData->partData_len;

    /* Using Global Memory: 
       Had to do this because of the way loc-api was written, the structure
       does not allocate memory. The assumption is that once this function 
       returns this Buffer can be used */
    pz_IoctlData->data_ptr = pu_LocQmiShimCharBuf;
    locQmiShimMemcpy(
          (sizeof(char) * pz_OrbitsData->partData_len ),
          pz_OrbitsData->partData,
          LOC_QMI_SHIM_MAX_CHAR_BUF_LEN,
          pu_LocQmiShimCharBuf );

    v_RetVal = TRUE;
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Injected Utc Time
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocInjectUtcTime
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo
)
{
  boolean v_RetVal = FALSE;
  const qmiLocInjectUtcTimeReqMsgT_v02        *pz_UtcData = 
                      (qmiLocInjectUtcTimeReqMsgT_v02*) pz_Msg;
  
  loc_assist_data_time_s_type  *pz_IoctlData = 
                       &(pz_IoctlInfo->assistance_data_time);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed */

  pz_IoctlData->time_utc = pz_UtcData->timeUtc;
  pz_IoctlData->uncertainty = pz_UtcData->timeUnc;

  v_RetVal = TRUE;
  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert from qmiLocReliabilityEnumT_v02 to
  pdsm_pd_reliability_e_type

@param[out]  pe_Target            points to target PDAPI data type
@param[in]   reliability          Qmi-Loc Format of reliability indicator

@retval    TRUE  if the conversion was successful 
@retval    FALSE  if the conversion failed
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToPdsmPdReliability
( 
  pdsm_pd_reliability_e_type * const pe_Target,
  const qmiLocReliabilityEnumT_v02 reliability
)
{
  boolean result = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if(NULL == pe_Target)
    {
      result = FALSE;
      break;
    }

    switch(reliability)
    {
    case eQMI_LOC_RELIABILITY_NOT_SET_V02:
      *pe_Target = PDSM_POSITION_RELIABILITY_NOT_SET;
      break;
    case eQMI_LOC_RELIABILITY_VERY_LOW_V02:
      *pe_Target = PDSM_POSITION_RELIABILITY_VERY_LOW;
      break;
    case eQMI_LOC_RELIABILITY_LOW_V02:
      *pe_Target = PDSM_POSITION_RELIABILITY_LOW;
      break;
    case eQMI_LOC_RELIABILITY_MEDIUM_V02:
      *pe_Target = PDSM_POSITION_RELIABILITY_MEDIUM;
      break;
    case eQMI_LOC_RELIABILITY_HIGH_V02:
      *pe_Target = PDSM_POSITION_RELIABILITY_HIGH;
      break;
    default:
      result = FALSE;
      break;
    }
  } while(0);

  if(TRUE != result)
  {
    SHIM_MSG_ERROR("locQmiShimConvToPdsmPdReliability failed",0,0,0);
  }
  return result;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert from qmiLocAltSrcEnumT_v02 to
  pdsm_pd_altitude_source_e_type
  
@param[out]  pe_Target    points to target PDAPI data type
@param[in]   src          Qmi-Loc Format of source of altitude info

@retval    TRUE if the conversion was successful 
@retval    FALSE if the conversion failed
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToPdsmPdAltitudeSource
(
  pdsm_pd_altitude_source_e_type * const pe_Target,
  const qmiLocAltSrcEnumT_v02 src
)
{
  boolean result = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if(NULL == pe_Target)
    {
      result = FALSE;
      break;
    }

    switch(src)
    {
    case eQMI_LOC_ALT_SRC_UNKNOWN_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_UNKNOWN;
      break;
    case eQMI_LOC_ALT_SRC_GPS_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_GPS;
      break;
    case eQMI_LOC_ALT_SRC_CELL_ID_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_CID;
      break;
    case eQMI_LOC_ALT_SRC_ENHANCED_CELL_ID_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_ECID;
      break;
    case eQMI_LOC_ALT_SRC_WIFI_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_WIFI;
      break;
    case eQMI_LOC_ALT_SRC_TERRESTRIAL_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_TERRESTRIAL;
      break;
    case eQMI_LOC_ALT_SRC_TERRESTRIAL_HYBRID_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_TERRESTRIAL_HYBRID;
      break;
    case eQMI_LOC_ALT_SRC_ALTITUDE_DATABASE_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_ALTITUDE_DATABASE;
      break;
    case eQMI_LOC_ALT_SRC_BAROMETRIC_ALTIMETER_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_BAROMETRIC_ALTIMETER;
      break;
    case eQMI_LOC_ALT_SRC_OTHER_V02:
      *pe_Target = PDSM_ALTITUDE_SOURCE_OTHER;
      break;
    default:
      result = FALSE;
      break;
    }
  } while(0);

  if(TRUE != result)
  {
    SHIM_MSG_ERROR("locQmiShimConvToPdsmPdAltitudeSource failed",0,0,0);
  }
  return result;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert from qmiLocAltSrcLinkageEnumT_v02 to
  pdsm_pd_altitude_horizontal_linkage_e_type
  
@param[out]  pe_Target        points to target PDAPI data type
@param[in]   linkage          Qmi-Loc Format of relationship between altitude 
                              and 2D location info

@retval    TRUE if the conversion was successful 
@retval    FALSE if the conversion failed
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToPdsmPdAltitudeHorLinkage
(
  pdsm_pd_altitude_horizontal_linkage_e_type * const pe_Target,
  const qmiLocAltSrcLinkageEnumT_v02 linkage
)
{
  boolean result = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  do
  {
    if(NULL == pe_Target)
    {
      result = FALSE;
      break;
    }

    switch(linkage)
    {
    case eQMI_LOC_ALT_SRC_LINKAGE_NOT_SPECIFIED_V02:
      *pe_Target = PDSM_ALT_HOR_LINK_NOT_SPECIFIED;
      break;
    case eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INTERDEPENDENT_V02:
      *pe_Target = PDSM_ALT_HOR_LINK_FULLY_INTERDEPENDENT;
      break;
    case eQMI_LOC_ALT_SRC_LINKAGE_DEPENDS_ON_LAT_LONG_V02:
      *pe_Target = PDSM_ALT_HOR_LINK_ALT_DEP_LAT_LONG;
      break;
    case eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INDEPENDENT_V02:
      *pe_Target = PDSM_ALT_HOR_LINK_FULLY_INDEPENDENT;
      break;
    default:
      result = FALSE;
      break;
    }
  } while(0);

  if(TRUE != result)
  {
    SHIM_MSG_ERROR("locQmiShimConvToPdsmPdAltitudeHorLinkage failed",0,0,0);
  }
  return result;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert from qmiLocAltSrcUncertaintyCoverageEnumT_v02 to
  pdsm_pd_altitude_unc_coverage_e_type
  
@param[out]  pe_Target        points to target PDAPI data type
@param[in]   coverage          Qmi-Loc Format of coverage of altitude info

@retval    TRUE if the conversion was successful 
@retval    FALSE if the conversion failed
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToPdsmPdAltitudeUncCoverage
(
  pdsm_pd_altitude_unc_coverage_e_type * const pe_Target,
  const qmiLocAltSrcUncertaintyCoverageEnumT_v02 coverage
)
{
  boolean result = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  do
  {
    if(NULL == pe_Target)
    {
      result = FALSE;
      break;
    }

    switch(coverage)
    {
    case eQMI_LOC_ALT_UNCERTAINTY_NOT_SPECIFIED_V02:
      *pe_Target = PDSM_ALT_COVERAGE_NOT_SPECIFIED;
      break;
    case eQMI_LOC_ALT_UNCERTAINTY_POINT_V02:
      *pe_Target = PDSM_ALT_COVERAGE_POINT_UNC;
      break;
    case eQMI_LOC_ALT_UNCERTAINTY_FULL_V02:
      *pe_Target = PDSM_ALT_COVERAGE_FULL_UNC;
      break;
    default:
      result = FALSE;
      break;
    }
  } while(0);

  if(TRUE != result)
  {
    SHIM_MSG_ERROR("locQmiShimConvToPdsmPdAltitudeUncCoverage failed",0,0,0);
  }
  return result;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Injected Position
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocInjectPosition
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocInjectPositionReqMsgT_v02    *pz_PosData = 
                      (qmiLocInjectPositionReqMsgT_v02*) pz_Msg;
  
  loc_assist_data_pos_s_type *pz_IoctlData = 
                       &(pz_IoctlInfo->assistance_data_position);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Not doing check of inputs as this is called from within the module,
   and this will be called with proper parameters 
   Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  do
  {
    pz_IoctlData->valid_mask = 0;
  

    if ( TRUE == pz_PosData->latitude_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_LATITUDE;
      pz_IoctlData->latitude = pz_PosData->latitude;
    }

    if ( TRUE == pz_PosData->longitude_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_LONGITUDE;
      pz_IoctlData->longitude = pz_PosData->longitude;
    }
   
    if ( TRUE == pz_PosData->timestampUtc_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_TIMESTAMP_UTC;
      pz_IoctlData->timestamp_utc = pz_PosData->timestampUtc;
    }
  
    if ( TRUE == pz_PosData->altitudeWrtEllipsoid_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_ALTITUDE_WRT_ELLIPSOID;
      pz_IoctlData->altitude_wrt_ellipsoid = pz_PosData->altitudeWrtEllipsoid;
    }
  
    if ( TRUE == pz_PosData->altitudeWrtMeanSeaLevel_valid )
    {
      pz_IoctlData->valid_mask |= 
                            LOC_ASSIST_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL;
      pz_IoctlData->altitude_wrt_mean_sea_level = 
                            pz_PosData->altitudeWrtMeanSeaLevel;
    }
  
    if ( TRUE == pz_PosData->horUncCircular_valid )
    {
      if(pz_PosData->horUncCircular > 0.0)
      {
        pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_HOR_UNC_CIRCULAR;
        pz_IoctlData->hor_unc_circular = pz_PosData->horUncCircular;

        /* Parse confidence only when circular unc is set */
        if ( TRUE == pz_PosData->horConfidence_valid )
        {
          pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_CONFIDENCE_HORIZONTAL;
          pz_IoctlData->confidence_horizontal = pz_PosData->horConfidence;
        }
      }
      else
      {
        SHIM_MSG_ERROR("Invalid HorUncCircular = %f ",pz_PosData->horUncCircular,0,0);
        v_RetVal = FALSE;
        break;
      }
    }
  
    if ( TRUE == pz_PosData->vertUnc_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_VERT_UNC;
      pz_IoctlData->vert_unc = pz_PosData->vertUnc;
    }
  
    if ( TRUE == pz_PosData->vertConfidence_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_CONFIDENCE_VERTICAL;
      pz_IoctlData->confidence_vertical= pz_PosData->vertConfidence;
    }
  
    if ( TRUE == pz_PosData->timestampAge_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_TIMESTAMP_AGE;
      pz_IoctlData->timestamp_age = pz_PosData->timestampAge;
    }
  
    /* horReliability, verReliability, AltSourceInfo - 
              Loc Api 2.0 Upgrades */

    if ( TRUE == pz_PosData->horReliability_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_H_RELIABILITY_IND;
      if(TRUE != locQmiShimConvToPdsmPdReliability(
          &pz_IoctlData->hor_reliability, pz_PosData->horReliability))
      {
        v_RetVal = FALSE;
        break;
      }
    }

    if ( TRUE == pz_PosData->vertReliability_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_V_RELIABILITY_IND;
      if(TRUE != locQmiShimConvToPdsmPdReliability(
          &pz_IoctlData->vert_reliability, pz_PosData->vertReliability))
      {
        v_RetVal = FALSE;
        break;
      }
    }

    if ( TRUE == pz_PosData->altSourceInfo_valid )
    {
      pz_IoctlData->valid_mask |= (LOC_ASSIST_POS_VALID_ALTITUDE_SOURCE |
                                   LOC_ASSIST_POS_VALID_ALT_H_LINKAGE | 
                                   LOC_ASSIST_POS_VALID_ALT_UNC_COVERAGE);

      if(TRUE != locQmiShimConvToPdsmPdAltitudeSource(
           &pz_IoctlData->altitude_source, pz_PosData->altSourceInfo.source))
      {
        v_RetVal = FALSE;
        break;
      }

      if(TRUE != locQmiShimConvToPdsmPdAltitudeHorLinkage(
           &pz_IoctlData->altitude_hor_linkage, pz_PosData->altSourceInfo.linkage))
      {
        v_RetVal = FALSE;
        break;
      }

      if(TRUE != locQmiShimConvToPdsmPdAltitudeUncCoverage(
           &pz_IoctlData->altitude_unc_coverage, pz_PosData->altSourceInfo.coverage))
      {
        v_RetVal = FALSE;
        break;
      }
    }

    if ( TRUE == pz_PosData->positionSrc_valid )
    {
      pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_POSITION_SRC;
      switch ( pz_PosData->positionSrc )
      {
        case eQMI_LOC_POSITION_SRC_GNSS_V02:
          pz_IoctlData->position_source = 
                        PDSM_PD_EXTERN_COARSE_POS_SOURCE_GPS;
          break;
        case eQMI_LOC_POSITION_SRC_CELLID_V02:
          pz_IoctlData->position_source = 
                        PDSM_PD_EXTERN_COARSE_POS_SOURCE_CELLID;
          break;
        case eQMI_LOC_POSITION_SRC_ENH_CELLID_V02:
          pz_IoctlData->position_source = 
                        PDSM_PD_EXTERN_COARSE_POS_SOURCE_ENH_CELLID;
          break;
        case eQMI_LOC_POSITION_SRC_WIFI_V02:
          pz_IoctlData->position_source = 
                        PDSM_PD_EXTERN_COARSE_POS_SOURCE_WIFI;
          break;
        case eQMI_LOC_POSITION_SRC_TERRESTRIAL_V02:
          pz_IoctlData->position_source = 
                        PDSM_PD_EXTERN_COARSE_POS_SOURCE_TERRESTRIAL;
          break;
        case eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID_V02:
          pz_IoctlData->position_source = 
                        PDSM_PD_EXTERN_COARSE_POS_SOURCE_GPS_TERRESTRIAL_HYBRID;
          break;
        case eQMI_LOC_POSITION_SRC_OTHER_V02:
          pz_IoctlData->position_source = PDSM_PD_EXTERN_COARSE_POS_SOURCE_OTHER;
          break;
        default:
          v_RetVal = FALSE;
          break;
      }
    }

    if ( FALSE == v_RetVal )
    {      
      break;
    }
    
    if ( TRUE == pz_PosData->rawHorUncCircular_valid )
    {
      if(pz_PosData->rawHorUncCircular > 0.0)
      {
        pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_RAW_HOR_UNC_CIRCULAR;
        pz_IoctlData->raw_hor_unc_circular = pz_PosData->rawHorUncCircular;
    
        /* Parse raw confidence only when raw circular unc is set */
        if ( TRUE == pz_PosData->rawHorConfidence_valid )
        {
          pz_IoctlData->valid_mask |= LOC_ASSIST_POS_VALID_RAW_CONFIDENCE_HORIZONTAL;
          pz_IoctlData->raw_confidence_horizontal = pz_PosData->rawHorConfidence;
        }
      }
      else
      {
        SHIM_MSG_ERROR("Invalid rawHorUncCircular = %f ",pz_PosData->rawHorUncCircular,0,0);
        v_RetVal = FALSE;
        break;
      }
    }

  } while(0);

  if(TRUE != v_RetVal)
  {
      SHIM_MSG_ERROR("locQmiShimConvToLocInjectPosition failed",0,0,0);
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Injected Engine Lock Request
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetEngineLock
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSetEngineLockReqMsgT_v02  *pz_EngLock = 
                      (qmiLocSetEngineLockReqMsgT_v02*) pz_Msg;
  
  loc_lock_e_type  *pz_IoctlData = 
                       &(pz_IoctlInfo->engine_lock);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */
  

  switch ( pz_EngLock->lockType )
  {
    case eQMI_LOC_LOCK_NONE_V02:
      (*pz_IoctlData) = LOC_LOCK_NONE;
      break;
    case eQMI_LOC_LOCK_MI_V02:
      (*pz_IoctlData) = LOC_LOCK_MI;
      break;
    case eQMI_LOC_LOCK_MT_V02:
      (*pz_IoctlData) = LOC_LOCK_MT;
      break;
    case eQMI_LOC_LOCK_ALL_V02:
      (*pz_IoctlData) = LOC_LOCK_ALL;
      break;
    default:
      SHIM_MSG_ERROR("Invalid Input: %d", pz_EngLock->lockType, 0, 0 );
      v_RetVal = FALSE;
      break;
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Injected Set Nmea Type request
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetNmeaTypes
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSetNmeaTypesReqMsgT_v02  *pz_NmeaType = 
                      (qmiLocSetNmeaTypesReqMsgT_v02*) pz_Msg;
  
  loc_nmea_sentence_type *pz_IoctlData = &(pz_IoctlInfo->nmea_types);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */
  

  if ( QMI_LOC_NMEA_MASK_GGA_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GGA;
  }
  if ( QMI_LOC_NMEA_MASK_RMC_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_RMC;
  }
  if ( QMI_LOC_NMEA_MASK_GSV_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GSV;
  }
  if ( QMI_LOC_NMEA_MASK_GSA_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GSA;
  }
  if ( QMI_LOC_NMEA_MASK_VTG_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_VTG;
  }
  if ( QMI_LOC_NMEA_MASK_PQXFI_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_PQXFI;
  }
  if ( QMI_LOC_NMEA_MASK_GLGSV_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GLGSV;
  }
  if ( QMI_LOC_NMEA_MASK_GNGSA_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GNGSA;
  }
  if ( QMI_LOC_NMEA_MASK_GNGNS_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GNGNS;
  }
  if ( QMI_LOC_NMEA_MASK_GARMC_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GARMC;
  }
  if ( QMI_LOC_NMEA_MASK_GAGSV_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GAGSV;
  }
  if ( QMI_LOC_NMEA_MASK_GAGSA_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GAGSA;
  }
  if ( QMI_LOC_NMEA_MASK_GAVTG_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GAVTG;
  }
  if ( QMI_LOC_NMEA_MASK_PSTIS_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_PSTIS;
  }
  if ( QMI_LOC_NMEA_MASK_GAGGA_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_GAGGA;
  }
  if ( QMI_LOC_NMEA_MASK_PQGSA_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_PQGSA;
  }
  if ( QMI_LOC_NMEA_MASK_PQGSV_V02 & (pz_NmeaType->nmeaSentenceType) )
  {
    (*pz_IoctlData) |= LOC_NMEA_MASK_PQGSV;
  }

  return v_RetVal; 
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert delete assistance data request
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocDeleteAssistData
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocDeleteAssistDataReqMsgT_v02  *pz_DelAssist = 
                      (qmiLocDeleteAssistDataReqMsgT_v02*) pz_Msg;
  
  loc_assist_data_delete_s_type *pz_IoctlData = 
                            &(pz_IoctlInfo->assist_data_delete);

  uint8 u_Cntr;
  uint8 u_MaxLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  memset ( pz_IoctlData, 0, sizeof (*pz_IoctlData) );

    if ( TRUE == locQmiShimIsValidBoolean(pz_DelAssist->deleteAllFlag) )
  {
    if ( TRUE == pz_DelAssist->deleteAllFlag )
    {
      pz_IoctlData->type = LOC_ASSIST_DATA_ALL;

      pz_IoctlData->cell_db = LOC_CELLDB_DELETE_ALL;
      pz_IoctlData->clock_info = LOC_CLOCK_INFO_DELETE_ALL;
    }
    else
    {
      /* Loop Through each constellation and 
          (a) Ensure validity
          (b) Clean information for LocAPI Call */

      if ( TRUE == pz_DelAssist->deleteSvInfoList_valid )
      {
        uint8 u_SvCounter; 
        qmiLocSvSystemEnumT_v02 e_CurrSystem;
        const qmiLocDeleteSvInfoStructT_v02 *pz_CurrSv;
        
        for ( u_SvCounter =0;
              u_SvCounter < pz_DelAssist->deleteSvInfoList_len;
              u_SvCounter ++ )
        {
          pz_CurrSv = &(pz_DelAssist->deleteSvInfoList[u_SvCounter]);
          if ( ( pz_CurrSv->gnssSvId >= PDSM_PD_SV_ID_GPS_MIN ) && 
               ( pz_CurrSv->gnssSvId <= PDSM_PD_SV_ID_GPS_MAX ) )
          {
            e_CurrSystem = eQMI_LOC_SV_SYSTEM_GPS_V02;
          }
          else if ( ( pz_CurrSv->gnssSvId >= PDSM_PD_SV_ID_SBAS_MIN ) && 
               ( pz_CurrSv->gnssSvId <= PDSM_PD_SV_ID_SBAS_MAX ) )
          {
            e_CurrSystem = eQMI_LOC_SV_SYSTEM_SBAS_V02;
          }
          else if ( ( pz_CurrSv->gnssSvId >= PDSM_PD_SV_ID_GLO_MIN ) && 
               ( pz_CurrSv->gnssSvId <= PDSM_PD_SV_ID_GLO_MAX ) )             
          {
            e_CurrSystem = eQMI_LOC_SV_SYSTEM_GLONASS_V02;
          }
          else if ( ( pz_CurrSv->gnssSvId >= PDSM_PD_SV_ID_GAL_MIN ) && 
               ( pz_CurrSv->gnssSvId <= PDSM_PD_SV_ID_GAL_MAX ) )             
          {
            e_CurrSystem = eQMI_LOC_SV_SYSTEM_GALILEO_V02;
          }
          else
          {
            v_RetVal = FALSE;
            break;
          }

          /* Validate that the system is set correct */
          if ( e_CurrSystem != pz_CurrSv->system )
          {
            v_RetVal = FALSE; 
            break;
          }

          /* Clean info for GPS and GNSS */
          if ( eQMI_LOC_SV_SYSTEM_GPS_V02== e_CurrSystem )
          {
            if ( QMI_LOC_MASK_DELETE_EPHEMERIS_V02 & pz_CurrSv->deleteSvInfoMask )
            {
              pz_IoctlData->type |= LOC_ASSIST_DATA_EPH;
            }
            if ( QMI_LOC_MASK_DELETE_ALMANAC_V02 & pz_CurrSv->deleteSvInfoMask )
            {
              pz_IoctlData->type |= LOC_ASSIST_DATA_ALM;
            }
          }
          else if ( eQMI_LOC_SV_SYSTEM_GLONASS_V02== e_CurrSystem )
          {
            if ( QMI_LOC_MASK_DELETE_EPHEMERIS_V02 & pz_CurrSv->deleteSvInfoMask )
            {
              pz_IoctlData->type |= LOC_ASSIST_DATA_EPH_GLO;
            }
            if ( QMI_LOC_MASK_DELETE_ALMANAC_V02 & pz_CurrSv->deleteSvInfoMask )
            {
              pz_IoctlData->type |= LOC_ASSIST_DATA_ALM_GLO;
            }
          }
          else if ( eQMI_LOC_SV_SYSTEM_GALILEO_V02 == e_CurrSystem )
          {
            if ( QMI_LOC_MASK_DELETE_EPHEMERIS_V02 & pz_CurrSv->deleteSvInfoMask )
            {
              pz_IoctlData->type |= LOC_ASSIST_DATA_EPH_GAL;
            }
            if ( QMI_LOC_MASK_DELETE_ALMANAC_V02 & pz_CurrSv->deleteSvInfoMask )
            {
              pz_IoctlData->type |= LOC_ASSIST_DATA_ALM_GAL;
            }
          }
          else if ( eQMI_LOC_SV_SYSTEM_SBAS_V02 == e_CurrSystem )
          {
            if ( QMI_LOC_MASK_DELETE_EPHEMERIS_V02 & pz_CurrSv->deleteSvInfoMask )
            {
              pz_IoctlData->type |= LOC_ASSIST_DATA_EPH_SBAS;
            }
            if ( QMI_LOC_MASK_DELETE_ALMANAC_V02 & pz_CurrSv->deleteSvInfoMask )
            {
              pz_IoctlData->type |= LOC_ASSIST_DATA_ALM_SBAS;
            }
          }

        }// end of for loop
      }// end of if ( TRUE == pz_DelAssist->deleteSvInfoList_valid )

      /* Process BDS specific delete SV info*/
      if ( TRUE == pz_DelAssist->deleteBdsSvInfoList_valid )
      {
        uint8 u_SvCounter; 
        const qmiLocDeleteBDSSvInfoStructT_v02 *pz_BdsSv;
        
        SHIM_MSG_MED( "BDS SV delete request",
                      0, 0, 0 );

        for ( u_SvCounter =0;
              u_SvCounter < pz_DelAssist->deleteBdsSvInfoList_len;
              u_SvCounter ++ )
        {
          pz_BdsSv = &(pz_DelAssist->deleteBdsSvInfoList[u_SvCounter]);
          /* Check the range of BDS satellites */
          if ( ( pz_BdsSv->gnssSvId < PDSM_PD_SV_ID_BDS_MIN ) || 
               ( pz_BdsSv->gnssSvId > PDSM_PD_SV_ID_BDS_MAX ) )
          {
            v_RetVal = FALSE;
            break;
          }
          /* Glean info for BDS */
          if ( QMI_LOC_MASK_DELETE_EPHEMERIS_V02 & pz_BdsSv->deleteSvInfoMask )
          {
            pz_IoctlData->type |= LOC_ASSIST_DATA_EPH_BDS;
          }
          if ( QMI_LOC_MASK_DELETE_ALMANAC_V02 & pz_BdsSv->deleteSvInfoMask )
          {
            pz_IoctlData->type |= LOC_ASSIST_DATA_ALM_BDS;
          }
        }// end for loop 

      }// end of if ( TRUE == pz_DelAssist->deleteBdsSvInfoList_valid )

   /* Process GAL specific delete SV info*/
      if ( TRUE == pz_DelAssist->deleteGalSvInfoList_valid )
      {
        uint8 u_SvCounter; 
        const qmiLocDeleteGALSvInfoStructT_v02 *pz_GalSv;
        
        SHIM_MSG_MED( "Gal SV delete request",
                      0, 0, 0 );

        for ( u_SvCounter =0;
              u_SvCounter < pz_DelAssist->deleteGalSvInfoList_len;
              u_SvCounter ++ )
        {
          pz_GalSv = &(pz_DelAssist->deleteGalSvInfoList[u_SvCounter]);
          /* Check the range of GAL satellites */
          if ( ( pz_GalSv->gnssSvId < PDSM_PD_SV_ID_GAL_MIN ) || 
               ( pz_GalSv->gnssSvId > PDSM_PD_SV_ID_GAL_MAX ) )
          {
            v_RetVal = FALSE;
            break;
          }
          /* Clean info for GAL */
          if ( QMI_LOC_MASK_DELETE_EPHEMERIS_V02 & pz_GalSv->deleteSvInfoMask )
          {
            pz_IoctlData->type |= LOC_ASSIST_DATA_EPH_GAL;
          }
          if ( QMI_LOC_MASK_DELETE_ALMANAC_V02 & pz_GalSv->deleteSvInfoMask )
          {
            pz_IoctlData->type |= LOC_ASSIST_DATA_ALM_GAL;
          }
        }// end for loop 

      }// end of if ( TRUE == pz_DelAssist->deleteGalSvInfoList_valid )

      /* Covert GNSS Delete Assistance Data Mask */
      if ( TRUE == pz_DelAssist->deleteGnssDataMask_valid )
      {
        u_MaxLen = ( sizeof ( z_LocQmiShimDeleteGnssDataMaskMap ) ) / 
                      ( sizeof ( z_LocQmiShimDeleteGnssDataMaskMap[0] ) ) ;

         for (  u_Cntr = 0;
                u_Cntr < u_MaxLen ;
                u_Cntr ++ )
        {
          if ( z_LocQmiShimDeleteGnssDataMaskMap[u_Cntr].t_QmiMask &
                 pz_DelAssist->deleteGnssDataMask )
          {
            pz_IoctlData->type |=   
                      z_LocQmiShimDeleteGnssDataMaskMap[u_Cntr].q_LocMask;
          }
        }
      }
      /* Convert CellDb Delete Assistance Mask */
      if ( TRUE == pz_DelAssist->deleteCellDbDataMask_valid )
      {
        pz_IoctlData->type |= LOC_ASSIST_DATA_CELLDB_INFO;
                        
        u_MaxLen = ( sizeof ( z_QmiPdsmCellDBMaskMap ) ) / 
                      ( sizeof ( z_QmiPdsmCellDBMaskMap[0] ) ) ;

         for (  u_Cntr = 0;
                u_Cntr < u_MaxLen ;
                u_Cntr ++ )
        {
          if ( z_QmiPdsmCellDBMaskMap[u_Cntr].q_qmi_mask &
                 pz_DelAssist->deleteCellDbDataMask )
          {
            pz_IoctlData->cell_db |=   
                      z_QmiPdsmCellDBMaskMap[u_Cntr].q_pdapi_mask;
          }        
        }
      }

     /* Convert Clock Info Delete Assistance Mask */
     if ( TRUE == pz_DelAssist->deleteClockInfoMask_valid)
     {
       pz_IoctlData->type |= LOC_ASSIST_DATA_CLOCK_INFO;
                       
       u_MaxLen = ( sizeof ( z_QmiPdsmClockInfoMaskMap) ) / 
                     ( sizeof ( z_QmiPdsmClockInfoMaskMap[0] ) ) ;
     
        for (  u_Cntr = 0;
               u_Cntr < u_MaxLen ;
               u_Cntr ++ )
       {
         if ( z_QmiPdsmClockInfoMaskMap[u_Cntr].q_qmi_mask &
                pz_DelAssist->deleteClockInfoMask )
         {
           pz_IoctlData->clock_info |=   
                     z_QmiPdsmClockInfoMaskMap[u_Cntr].q_pdapi_mask;
         }        
       }
     }

    }
  }
  else
  {
    v_RetVal = FALSE;
  }
 
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Set XTRAT Session Control
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocXtraTSessCtrl
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSetXtraTSessionControlReqMsgT_v02  *pz_SetXtraT = 
                      (qmiLocSetXtraTSessionControlReqMsgT_v02*) pz_Msg;
  
  uint8  *pz_IoctlData = &(pz_IoctlInfo->xtra_t_session_control);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  /* Note: Not doing a check for boolean, as non-zero numbers are valid */
  (*pz_IoctlData) = pz_SetXtraT->xtraTSessionControl;

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Set Operation Mode
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully


@warning   This is an extended IOCTL introduced for QMI-LOC, and hence the 
           "reserved" data field in the ioctl is used. This function data-ptr
           and data-size accordingly.

*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetOperationMode
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSetOperationModeReqMsgT_v02     *pz_SetOpMode = 
                      (qmiLocSetOperationModeReqMsgT_v02*) pz_Msg; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */


  switch ( pz_SetOpMode->operationMode )
  {
    /* range check the operation mode enum */
    case eQMI_LOC_OPER_MODE_DEFAULT_V02:
    case eQMI_LOC_OPER_MODE_MSB_V02:
    case eQMI_LOC_OPER_MODE_MSA_V02:
    case eQMI_LOC_OPER_MODE_STANDALONE_V02:
    case eQMI_LOC_OPER_MODE_CELL_ID_V02:
    case eQMI_LOC_OPER_MODE_WWAN_V02:
      pz_IoctlInfo->operation_mode.operationMode = pz_SetOpMode->operationMode;
      if( TRUE == pz_SetOpMode->minInterval_valid )
      {
        pz_IoctlInfo->operation_mode.minInterval_valid = pz_SetOpMode->minInterval_valid;
        pz_IoctlInfo->operation_mode.minInterval = locQmiShimMatchFixInterval(pz_SetOpMode->minInterval);
      }
      else
      {
        pz_IoctlInfo->operation_mode.minInterval_valid = FALSE;
        pz_IoctlInfo->operation_mode.minInterval = 0;
      }
      break;
    default:
      v_RetVal=FALSE;
      SHIM_MSG_ERROR( "Invalid Option:%d",
                      pz_SetOpMode->operationMode, 0, 0 );
      break;
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Set Server Address. This is not part of the IOCTL
  Converter table, as one QMI-Request can trigger one-of-the-four LOC-Api
  IOCTLs
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 
@param[out]  pe_LocIoctlType      Loc-Ioctl that has to be triggered

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully

@warning  This function is not part of the function table.
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetServer
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo,
  loc_ioctl_e_type                            *pe_LocIoctlType
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSetServerReqMsgT_v02      *pz_SetServer = 
                      (qmiLocSetServerReqMsgT_v02*) pz_Msg;
  loc_server_info_s_type                *pz_LocServerInfo =  
                                              &pz_IoctlInfo->server_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Not doing check of inputs as this is called from within the module,
   and this will be called with proper parameters 
   Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  switch ( pz_SetServer->serverType )
  {
    case eQMI_LOC_SERVER_TYPE_CDMA_PDE_V02:
    {
      (*pe_LocIoctlType) = LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR;
    }
    break;
    case eQMI_LOC_SERVER_TYPE_CDMA_MPC_V02:
    {
      (*pe_LocIoctlType) = LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR;
    }
    break;
    case eQMI_LOC_SERVER_TYPE_UMTS_SLP_V02:
    {
      (*pe_LocIoctlType) = LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR;
    }
    break;
    case eQMI_LOC_SERVER_TYPE_CUSTOM_PDE_V02:
    {
      (*pe_LocIoctlType) = LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR;
    }
    break;

    case QMILOCSERVERTYPEENUMT_MIN_ENUM_VAL_V02:
    case QMILOCSERVERTYPEENUMT_MAX_ENUM_VAL_V02:
    default:
      SHIM_MSG_ERROR( "Invalid Server Type: %d",
                      pz_SetServer->serverType, 0, 0);
      break;                
  }

  /* Priority given to IPV4 Address if both are given */
  if ( TRUE == pz_SetServer->ipv4Addr_valid )
  {
    pz_LocServerInfo->addr_type = LOC_SERVER_ADDR_IPV4;
    pz_LocServerInfo->addr_info.ipv4.addr = pz_SetServer->ipv4Addr.addr;
    pz_LocServerInfo->addr_info.ipv4.port = pz_SetServer->ipv4Addr.port;
  }
  else if ( TRUE == pz_SetServer->urlAddr_valid )
  {
    pz_LocServerInfo->addr_type = LOC_SERVER_ADDR_URL;

    /* Bounded Buffer Copy of incoming string. */
    pz_LocServerInfo->addr_info.url.length = (uint16)GNSS_STRLCPY ( 
                              (char*)pz_LocServerInfo->addr_info.url.addr,
                              (char*)pz_SetServer->urlAddr,
                              LOC_API_MAX_SERVER_ADDR_LENGTH ) ; 
  }
  else
  {
    SHIM_MSG_ERROR( "Neither URL nor IP address provided", 0, 0 , 0 );
    v_RetVal = FALSE;
  }
 
return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Get Server Address. This is not part of the IOCTL
  Converter table, as one QMI-Request can trigger one-of-the-four LOC-Api
  IOCTLs
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 
@param[out]  pe_LocIoctlType      Loc-Ioctl that has to be triggered

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully

@warning  This function is not part of the function table 
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocGetServer
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo,
  loc_ioctl_e_type                            *pe_LocIoctlType
)
{
  boolean v_RetVal = TRUE;
  const qmiLocGetServerReqMsgT_v02      *pz_GetServer = 
                      (qmiLocGetServerReqMsgT_v02*) pz_Msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Not doing check of inputs as this is called from within the module,
   and this will be called with proper parameters 
   Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  switch ( pz_GetServer->serverType )
  {
    case eQMI_LOC_SERVER_TYPE_CDMA_PDE_V02:
    {
      (*pe_LocIoctlType) = LOC_IOCTL_GET_CDMA_PDE_SERVER_ADDR;
    }
    break;
    case eQMI_LOC_SERVER_TYPE_CDMA_MPC_V02:
    {
      (*pe_LocIoctlType) = LOC_IOCTL_GET_CDMA_MPC_SERVER_ADDR;
    }
    break;
    case eQMI_LOC_SERVER_TYPE_UMTS_SLP_V02:
    {
      (*pe_LocIoctlType) = LOC_IOCTL_GET_UMTS_SLP_SERVER_ADDR;
    }
    break;
    case eQMI_LOC_SERVER_TYPE_CUSTOM_PDE_V02:
    {
      (*pe_LocIoctlType) = LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR;
    }
    break;

    case QMILOCSERVERTYPEENUMT_MAX_ENUM_VAL_V02:
    case QMILOCSERVERTYPEENUMT_MIN_ENUM_VAL_V02:
    default:
      SHIM_MSG_ERROR( "Invalid Server Type: %d",
                      pz_GetServer->serverType, 0, 0);
      break;                
  }

  /* No way to tell Loc-API if we need IPV4 or URL */
 
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Ni User Response. 
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocNiUserResponse 
(
  const void                  * pz_Msg,
  loc_ioctl_data_u_type       * pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocNiUserRespReqMsgT_v02   *pz_UserResp = 
                      (qmiLocNiUserRespReqMsgT_v02*) pz_Msg;
  loc_user_verify_s_type *pz_Loc = &pz_IoctlInfo->user_verify_resp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Not doing check of inputs as this is called from within the module,
   and this will be called with proper parameters 
   Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  
  /* Its implied that only one of the optional fields will be valid, 
     if this assumption is negated we need to update logic here */
  if ( TRUE == pz_UserResp->NiVxPayload_valid )
  {
    v_RetVal = locQmiShimConvToLocNiVxNotifyVerify( 
                                &pz_UserResp->NiVxPayload,
                                &(pz_Loc->ni_event_pass_back.payload.vx_req ));

    if ( TRUE == v_RetVal )
    {
      v_RetVal = locQmiShimConvToLocNiNotificationType ( 
           pz_UserResp->notificationType,
           &(pz_Loc->ni_event_pass_back.payload.vx_req.notification_priv_type) ); 
    }

    pz_Loc->ni_event_pass_back.event =
                      LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ;
  }
  else if ( TRUE == pz_UserResp->NiSuplPayload_valid )
  {
    loc_ni_supl_notify_verify_req_s_type *pz_Supl = 
                                    &pz_Loc->ni_event_pass_back.payload.supl_req;
    const qmiLocNiSuplNotifyVerifyStructT_v02  *pz_QmiLoc = 
                                           &pz_UserResp->NiSuplPayload;

    v_RetVal = locQmiShimConvToLocNiNotificationType ( 
                            pz_UserResp->notificationType,
                            &pz_Supl->notification_priv_type ); 
    pz_Supl->flags = 0;                        
    if ( TRUE == v_RetVal )
    {
      /* Copy from Qmi-Loc to Loc-Api capped by the size of Loc-Api, if 
         session ID length changes in Qmi-Loc, then the size has to be 
         increased in Loc-Api.. ie define another const. */
      if ( pz_QmiLoc->valid_flags & QMI_LOC_SUPL_SESSION_ID_MASK_V02 )
      {
        pz_Supl->supl_slp_session_id.presence = TRUE;
        memscpy ( pz_Supl->supl_slp_session_id.session_id, sizeof(pz_Supl->supl_slp_session_id.session_id),
                 pz_QmiLoc->suplSessionId, sizeof(pz_QmiLoc->suplSessionId)); 
        
        if ( pz_QmiLoc->valid_flags & QMI_LOC_SUPL_SERVER_INFO_MASK_V02 )
        {
          if ( pz_QmiLoc->suplServerInfo.suplServerAddrTypeMask & 
                      QMI_LOC_SERVER_ADDR_TYPE_IPV4_MASK_V02 ) 
          {
            pz_Supl->supl_slp_session_id.slp_address.addr_type =
                               LOC_SERVER_ADDR_IPV4;
            pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv4.addr =
                                  pz_QmiLoc->suplServerInfo.ipv4Addr.addr;
            pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv4.port = 
                                  pz_QmiLoc->suplServerInfo.ipv4Addr.port;
          }
          else if ( pz_QmiLoc->suplServerInfo.suplServerAddrTypeMask & 
                      QMI_LOC_SERVER_ADDR_TYPE_IPV6_MASK_V02 ) 
          {
            pz_Supl->supl_slp_session_id.slp_address.addr_type =
                               LOC_SERVER_ADDR_IPV6;
            memscpy ( pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv6.addr,
                      sizeof(pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv6.addr), 
                     pz_QmiLoc->suplServerInfo.ipv6Addr.addr,
                      sizeof(pz_QmiLoc->suplServerInfo.ipv6Addr.addr));
            
            pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv6.port = 
                                  pz_QmiLoc->suplServerInfo.ipv6Addr.port;
          }
          else if ( pz_QmiLoc->suplServerInfo.suplServerAddrTypeMask & 
                      QMI_LOC_SERVER_ADDR_TYPE_URL_MASK_V02 ) 
          {
            size_t suplServerurl_len = 0;

            pz_Supl->supl_slp_session_id.slp_address.addr_type =
                             LOC_SERVER_ADDR_URL;

            /* Bounded Buffer Copy. Length of src string is returned */
            suplServerurl_len = GNSS_STRLCPY ( 
               (char*)pz_Supl->supl_slp_session_id.slp_address.addr_info.url.addr,
               (char*)pz_QmiLoc->suplServerInfo.urlAddr,
                LOC_API_MAX_SERVER_ADDR_LENGTH );

            /* Check if copy was truncated. Adjust length accordingly */
            if ( suplServerurl_len >= LOC_API_MAX_SERVER_ADDR_LENGTH )
            {
              pz_Supl->supl_slp_session_id.slp_address.addr_info.url.length = \
                       LOC_API_MAX_SERVER_ADDR_LENGTH - 1;
              SHIM_MSG_MED("Rcvd SUPL srvr URL too long: %d, truncated",      \
                            suplServerurl_len, 0, 0);
            }
            else
              pz_Supl->supl_slp_session_id.slp_address.addr_info.url.length = \
                       suplServerurl_len;
          }
        }
      }

      /* No flag required on inbound */
      if ( pz_QmiLoc->valid_flags & QMI_LOC_SUPL_HASH_MASK_V02 )
      {
        memscpy ( pz_Supl->supl_hash, sizeof(pz_Supl->supl_hash),
                  pz_QmiLoc->suplHash, sizeof(pz_QmiLoc->suplHash));
      }

      if ( pz_QmiLoc->valid_flags & QMI_LOC_SUPL_DATA_CODING_SCHEME_MASK_V02 )
      {
        v_RetVal = locQmiShimConvToLocNiDataCodingScheme ( 
                          pz_QmiLoc->dataCodingScheme,
                          &pz_Supl->datacoding_scheme );

        if( TRUE == v_RetVal )
        {
          pz_Supl->flags |= PDSM_LCS_ENCODING_TYPE_PRESENT;
        }
      }
    }


    if ( TRUE == v_RetVal )
    {
      if ( pz_QmiLoc->valid_flags & QMI_LOC_SUPL_POS_METHOD_MASK_V02 )
      {
        switch ( pz_QmiLoc->posMethod )
        {
          case eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_AGPS_SETASSISTED;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_AGPS_SETBASED;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_PREF_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_AGPS_SETASSISTED_PREF;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_PREF_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_AGPS_SETBASED_PREF;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_AUTONOMOUS_GPS_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_AUTONOMOUS_GPS;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_AFLT_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_AFLT;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_ECID_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_ECID;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_EOTD_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_EOTD;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_OTDOA_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_OTDOA;
            break;
          case eQMI_LOC_NI_SUPL_POSMETHOD_NO_POSITION_V02:
            pz_Supl->pos_method = 
                    LOC_NI_POSMETHOD_NO_POSITION;
            break;
            
          default:
          {
            v_RetVal = FALSE;
            SHIM_MSG_ERROR( "Invalid Option: %d", 
                            pz_QmiLoc->posMethod, 0, 0 );
          }  
          break;
        }
      }
      else
      {
        v_RetVal = FALSE;
        SHIM_MSG_ERROR( "Invalid Message, Pos Method not specified: ", 
                        0, 0, 0 );
      }
    }

    if ( (  TRUE == v_RetVal ) &&
         ( pz_QmiLoc->valid_flags & QMI_LOC_SUPL_REQUESTOR_ID_MASK_V02 ) )
    {
      v_RetVal = locQmiShimConvToLocNiFormatType(
              pz_QmiLoc->requestorId.formatType,
              (&pz_Supl->requestor_id.data_coding_scheme ) );

      if ( TRUE == v_RetVal )
      {
        pz_Supl->requestor_id.string_len = (unsigned char) locQmiShimMemcpy ( 
                    pz_QmiLoc->requestorId.formattedString_len,
                    (void*)pz_QmiLoc->requestorId.formattedString,
                    LOC_NI_MAX_CLIENT_NAME_LENGTH,
                    (void*)pz_Supl->requestor_id.requestor_id_string );

        pz_Supl->flags |= PDSM_LCS_REQUESTOR_ID_PRESENT;
      }
    }

    if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiLoc->valid_flags & QMI_LOC_SUPL_CLIENT_NAME_MASK_V02 ) )
    {

      v_RetVal = locQmiShimConvToLocNiFormatType(
              pz_QmiLoc->clientName.formatType,
              &pz_Supl->client_name.data_coding_scheme );

      if ( TRUE == v_RetVal )
      {
        pz_Supl->client_name.string_len = (unsigned char)locQmiShimMemcpy ( 
                    pz_QmiLoc->clientName.formattedString_len,
                    (void*)pz_QmiLoc->clientName.formattedString,
                    LOC_NI_MAX_CLIENT_NAME_LENGTH,
                    (void*)pz_Supl->client_name.client_name_string );

        pz_Supl->flags |= PDSM_LCS_CLIENT_NAME_PRESENT;
      }
    }

    if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiLoc->valid_flags & QMI_LOC_SUPL_QOP_MASK_V02 ) )
    {

      pz_Supl->flags |= PDSM_LCS_SUPL_QOP_VALID;
      
      /* No mask present in Loc-API so assuming it is always provided */
      if ( pz_QmiLoc->suplQop.validMask & 
                  QMI_LOC_NI_SUPL_MASK_QOP_HORZ_ACC_VALID_V02 )
      {
        pz_Supl->supl_qop.horacc = pz_QmiLoc->suplQop.horizontalAccuracy;
      }
      else
      {
        SHIM_MSG_ERROR( "Invalid Message, Hor Acc not specified: ", 
                        0, 0, 0 );
        v_RetVal = FALSE;
      }
      
      if ( pz_QmiLoc->suplQop.validMask & 
                  QMI_LOC_NI_SUPL_MASK_QOP_MAXAGE_VALID_V02 )
      {
        pz_Supl->supl_qop.maxLocAge = pz_QmiLoc->suplQop.maxLocAge;
        pz_Supl->supl_qop.bit_mask   |=  PDSM_LCS_SUPL_QOP_MAXAGE_VALID;
      }
      if ( pz_QmiLoc->suplQop.validMask & 
                  QMI_LOC_NI_SUPL_MASK_QOP_DELAY_VALID_V02 )
      {
        pz_Supl->supl_qop.delay = pz_QmiLoc->suplQop.delay;
        pz_Supl->supl_qop.bit_mask   |=  PDSM_LCS_SUPL_QOP_DELAY_VALID;
      }

      if ( pz_QmiLoc->suplQop.validMask & 
                  QMI_LOC_NI_SUPL_MASK_QOP_VER_ACC_VALID_V02 )
      {
        pz_Supl->supl_qop.veracc = pz_QmiLoc->suplQop.verticalAccuracy;
        pz_Supl->supl_qop.bit_mask   |=  PDSM_LCS_SUPL_QOP_VERACC_VALID;
      }
    }

    if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiLoc->valid_flags & 
                  QMI_LOC_SUPL_USER_RESP_TIMER_MASK_V02 ) )
    {
      pz_Supl->user_response_timer = pz_QmiLoc->userResponseTimer;
    }

    if( ( TRUE == v_RetVal ) &&
        (TRUE == pz_UserResp->NiSuplVer2ExtPayload_valid ) )
    {
      /** copy the SUPL extensions to Loc API response payload */

      memscpy( &(pz_Supl->supl_ver2_ext), sizeof(pz_Supl->supl_ver2_ext),
              &(pz_UserResp->NiSuplVer2ExtPayload), sizeof(pz_UserResp->NiSuplVer2ExtPayload));

      pz_Supl->supl_ver2_ext_valid = TRUE;
      SHIM_MSG_LOW("locQmiShimConvToLocNiUserResponse: SUPL2 extensions present"
                    "gnssType= %u, triggertype=%d, supported netwprks =%u\n",
                    pz_Supl->supl_ver2_ext.gnssType, pz_Supl->supl_ver2_ext.triggerType,
                    pz_Supl->supl_ver2_ext.supportedNetworksMask); 
    }

    if( ( TRUE == v_RetVal ) &&
        (TRUE == pz_UserResp->suplEmergencyNotification_valid ) )
    {
      /** copy the SUPL extensions to Loc API response payload */

      memscpy( &(pz_Supl->supl_emergency_notification), sizeof(pz_Supl->supl_emergency_notification),
              &(pz_UserResp->suplEmergencyNotification), sizeof(pz_UserResp->suplEmergencyNotification));

      pz_Supl->supl_emergency_valid = TRUE;
      SHIM_MSG_HIGH("locQmiShimConvToLocNiUserResponse: SUPL emergency present"
                   "eslpUrl= %c\n", 
                   pz_Supl->supl_emergency_notification.eslpUrl[0],0,0); 
    }

    pz_Loc->ni_event_pass_back.event =
                      LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ;
  }
  else if ( TRUE == pz_UserResp->NiUmtsCpPayload_valid)
  {
    loc_ni_umts_cp_notify_verify_req_s_type  *pz_Umts = 
                                        &pz_Loc->ni_event_pass_back.payload.umts_cp_req;
    const qmiLocNiUmtsCpNotifyVerifyStructT_v02  *pz_QmiLoc = 
                                         &pz_UserResp->NiUmtsCpPayload;

    
    v_RetVal = locQmiShimConvToLocNiNotificationType ( 
                              pz_UserResp->notificationType,
                              &pz_Umts->notification_priv_type );
    pz_Umts->flags = 0;
    if  ( TRUE == v_RetVal )
    {
      if ( pz_QmiLoc->valid_flags & QMI_LOC_UMTS_CP_INVOKE_ID_MASK_V02 )
      {
        pz_Umts->invoke_id = pz_QmiLoc->invokeId;
      }
      else
      {
        v_RetVal = FALSE;
        SHIM_MSG_ERROR( "Invalid Message, Invoke ID not specified: ", 
                        0, 0, 0 );
      }
    }

    if ( ( TRUE == v_RetVal ) && 
         ( pz_QmiLoc->valid_flags & QMI_LOC_UMTS_CP_DATA_CODING_SCHEME_MASK_V02 ) )
    {
      v_RetVal = locQmiShimConvToLocNiDataCodingScheme ( 
                      pz_QmiLoc->dataCodingScheme,
                      &pz_Umts->datacoding_scheme );
      
      if( TRUE == v_RetVal )
      {
        pz_Umts->flags |= PDSM_LCS_ENCODING_TYPE_PRESENT;
      }
    }

    if ( TRUE == v_RetVal )
    {

      if ( pz_QmiLoc->valid_flags & QMI_LOC_UMTS_CP_NOTIFICATION_TEXT_MASK_V02 )
      {
        pz_Umts->notification_length = (unsigned char)locQmiShimMemcpy(
                pz_QmiLoc->notificationText_len,
                (void*)pz_QmiLoc->notificationText,
                LOC_NI_MAX_CLIENT_NAME_LENGTH,
                (void*)pz_Umts->notification_text );
        pz_Umts->flags |= PDSM_LCS_CLIENT_NAME_PRESENT;
      }

      if ( pz_QmiLoc->valid_flags & QMI_LOC_UMTS_CP_CLIENT_ADDRESS_MASK_V02)
      {
        pz_Umts->ext_client_address_data.ext_client_address_len = 
                (unsigned char) locQmiShimMemcpy(
                  pz_QmiLoc->clientAddress_len,
                  (void*)pz_QmiLoc->clientAddress,
                  LOC_NI_MAX_EXT_CLIENT_ADDRESS,
                  (void*)pz_Umts->ext_client_address_data.ext_client_address );

        pz_Umts->flags |= PDSM_LCS_CLIENT_EXTADDR_PRESENT;
        
      }
      
      if ( pz_QmiLoc->valid_flags & QMI_LOC_UMTS_CP_LOCATION_TYPE_MASK_V02)
      {
        switch ( pz_QmiLoc->locationType )
        {

          case eQMI_LOC_NI_LOCATIONTYPE_CURRENT_LOCATION_V02:
            pz_Umts->location_type = 
                    LOC_NI_LOCATIONTYPE_CURRENT_LOCATION;
            break;
          case eQMI_LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION_V02:
            pz_Umts->location_type = 
                 LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION;
            break;
          case eQMI_LOC_NI_LOCATIONTYPE_INITIAL_LOCATION_V02:
            pz_Umts->location_type =
                    LOC_NI_LOCATIONTYPE_INITIAL_LOCATION;
            break;
            
          default:
          {
            v_RetVal  = FALSE;
            SHIM_MSG_ERROR( "Invalid Option: %d", 
                            pz_QmiLoc->locationType, 0, 0 );
          }  
          break;
        }
        /* No flag for location type */
      }
      else
      {
        v_RetVal = FALSE;
        SHIM_MSG_ERROR( "Invalid Message, Location Type not specified: ", 
                        0, 0, 0 );
      }
    }

    if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiLoc->valid_flags & QMI_LOC_UMTS_CP_REQUESTOR_ID_MASK_V02 ) )
    {

      loc_ni_datacoding_scheme_e_type  e_RequestorIdDataCodingScheme = LOC_NI_SUPL_ENCODING_UNKNOWN;
      if ( pz_QmiLoc->requestorId.codedString_len <= sizeof(pz_QmiLoc->requestorId.codedString) )
      {
        pz_Umts->requestor_id.string_len = (unsigned char)locQmiShimMemcpy(
           pz_QmiLoc->requestorId.codedString_len,
           (void *)pz_QmiLoc->requestorId.codedString,
           LOC_NI_MAX_REQUESTOR_ID_LENGTH,
           (void *)pz_Umts->requestor_id.requestor_id_string);

        v_RetVal = locQmiShimConvToLocNiDataCodingScheme(
           pz_QmiLoc->requestorId.dataCodingScheme,
           &e_RequestorIdDataCodingScheme);

        /* Unfortunate but the data coding scheme in requestor ID is a char */
        pz_Umts->requestor_id.data_coding_scheme =
           (unsigned char)e_RequestorIdDataCodingScheme;
        pz_Umts->flags |= PDSM_LCS_REQUESTOR_ID_PRESENT;
      }
      else
      {
        v_RetVal = FALSE;
        SHIM_MSG_ERROR("Invalid codedString_len: %d in requestor ID for NI N/V",
                       pz_QmiLoc->requestorId.codedString_len, 0, 0);
      }
    }

    if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiLoc->valid_flags & QMI_LOC_UMTS_CP_CODEWORD_STRING_MASK_V02) )
    { 
    
      loc_ni_datacoding_scheme_e_type  e_RequestorIdDataCodingScheme = LOC_NI_SUPL_ENCODING_UNKNOWN;
      
      
      pz_Umts->codeword_string.string_len = (unsigned char) locQmiShimMemcpy ( 
                  pz_QmiLoc->codewordString.codedString_len,
                  (void*)pz_QmiLoc->codewordString.codedString,
                  LOC_NI_CODEWORD_LENGTH,
                  (void*)pz_Umts->codeword_string.lcs_codeword_string );
      
      v_RetVal = locQmiShimConvToLocNiDataCodingScheme ( 
                        pz_QmiLoc->codewordString.dataCodingScheme,
                        &e_RequestorIdDataCodingScheme);
      /* Unfortunate but the data coding scheme in codeword is a char */
      pz_Umts->codeword_string.data_coding_scheme = 
                        (unsigned char)e_RequestorIdDataCodingScheme;
      pz_Umts->flags |= PDSM_LCS_CODEWORD_PRESENT ;
    }

    if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiLoc->valid_flags & QMI_LOC_UMTS_CP_SERVICE_TYPE_MASK_V02 ) )
    {
      pz_Umts->service_type_id.lcs_service_type_id = 
                      pz_QmiLoc->lcsServiceTypeId;
      pz_Umts->flags |= PDSM_LCS_SERVICE_TYPE_ID_PRESENT;
      
      pz_Umts->user_response_timer = pz_QmiLoc->userResponseTimer;
    }
    pz_Loc->ni_event_pass_back.event =
                      LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ;
  }
  else if ( TRUE == pz_UserResp->NiVxServiceInteractionPayload_valid)
  {
    const qmiLocNiVxServiceInteractionStructT_v02* pz_QmiNiVxSvc = 
                                &pz_UserResp->NiVxServiceInteractionPayload;
    loc_ni_vx_service_interaction_req_s_type *pz_LocNiVxSvc = 
                   &pz_Loc->ni_event_pass_back.payload.service_interaction_req;
    v_RetVal = locQmiShimConvToLocNiVxNotifyVerify(
                                            &pz_QmiNiVxSvc->niVxReq,
                                            &(pz_LocNiVxSvc->ni_vx_req ) );
    
    if ( TRUE == v_RetVal )
    {
      v_RetVal = locQmiShimConvToLocNiNotificationType ( 
         pz_UserResp->notificationType,
         &(pz_LocNiVxSvc->ni_vx_req.notification_priv_type) ); 
    }
    
    pz_Loc->ni_event_pass_back.event =
                      LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ;
    
    
    switch ( pz_QmiNiVxSvc->serviceInteractionType )
    {
      case eQMI_LOC_NI_SERVICE_INTERACTION_ONGOING_NI_INCOMING_MO_V02:
         pz_LocNiVxSvc->service_interation_type = 
                LOC_NI_SERVICE_INTERACTION_ONGOING_NI_INCOMING_MO;
         break;   
      default:
      {
        v_RetVal = FALSE;
        SHIM_MSG_ERROR( "Invalid Option: %d", 
                        pz_QmiNiVxSvc->serviceInteractionType, 0, 0 );
      }  
      break;
    }
  }

  if ( TRUE == v_RetVal )
  {
    switch ( pz_UserResp->userResp )
    {
      case eQMI_LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT_V02:
       pz_Loc->user_resp = LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT;
       break;
      case eQMI_LOC_NI_LCS_NOTIFY_VERIFY_DENY_V02:
        pz_Loc->user_resp = LOC_NI_LCS_NOTIFY_VERIFY_DENY;
        break;
      case eQMI_LOC_NI_LCS_NOTIFY_VERIFY_NORESP_V02:
        pz_Loc->user_resp = LOC_NI_LCS_NOTIFY_VERIFY_NORESP;
        break;
      default:
        v_RetVal = FALSE;
        break;
    }
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Sbas Config
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetSbasConfig
( 
  const void                                   *pz_Msg,
  loc_ioctl_data_u_type                        *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSetSbasConfigReqMsgT_v02   *pz_Sbas = 
                      (qmiLocSetSbasConfigReqMsgT_v02*) pz_Msg;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed */

  if ( TRUE == locQmiShimIsValidBoolean(pz_Sbas->sbasConfig) )
  {
    if ( TRUE == pz_Sbas->sbasConfig )
    {
      pz_IoctlInfo->sbas_mode = TRUE;
    }
    else
    {
      pz_IoctlInfo->sbas_mode = FALSE;
    }
  }    
  else
  {
    v_RetVal = FALSE;
  }

  return v_RetVal; 
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Low Power Mode 
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetLowPowerMode
( 
  const void                                   *pz_Msg,
  loc_ioctl_data_u_type                        *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSetLowPowerModeReqMsgT_v02     *pz_Lpm = 
                      (qmiLocSetLowPowerModeReqMsgT_v02*) pz_Msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed */


  if ( TRUE == locQmiShimIsValidBoolean(pz_Lpm->lowPowerMode) )
  {  
    if ( FALSE == pz_Lpm->lowPowerMode )
    {
      pz_IoctlInfo->on_demand_lpm = FALSE;
    }
    else
    {
      pz_IoctlInfo->on_demand_lpm = TRUE;
    }
  }
  else
  {
    v_RetVal = FALSE;
  }
  
  return v_RetVal; 
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Inject Supl Certificate
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocInjectSuplCertificate
( 
  const void                                   *pz_Msg,
  loc_ioctl_data_u_type                        *pz_IoctlInfo
)
{
  const   qmiLocInjectSuplCertificateReqMsgT_v02 *pz_CertData = 
                      (qmiLocInjectSuplCertificateReqMsgT_v02*) pz_Msg;

  loc_efs_data_s_type *pz_Loc = &(pz_IoctlInfo->efs_data);
  int                  i_CharsWritten = 0;
  uint8                u_ErrCode       = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed */

  do
  {

    if ( QMI_LOC_MAX_SUPL_CERT_LENGTH_V02 < pz_CertData->suplCertData_len )
    {
      /* Return Error the Cert Length is greater than 
         what the Interface accepts */
      u_ErrCode = 1;
      break;
    }

    if ( LOC_QMI_SHIM_MAX_SUPL_CERT_ID < pz_CertData->suplCertId )
    {
      /* The interface allows only 0-9 */
      u_ErrCode = 5;
      break;
    }
     /* Using Global Memory: 
        Had to do this because of the way loc-api was written, the structure
        does not allocate memory. The assumption is that once this function 
        returns this Buffer can be used */
    pz_Loc->data_ptr = pu_LocQmiShimCharBuf;

    locQmiShimMemcpy (
              pz_CertData->suplCertData_len,
              pz_CertData->suplCertData, 
              sizeof(pu_LocQmiShimCharBuf),
              (void*)pu_LocQmiShimCharBuf);
    
    /* Write the prefix to the file name */
    i_CharsWritten = GNSS_STRLCPY ( (char*)pz_Loc->filename,
                                    pz_LocQmiShimCertLocnPrefix,
                                    LOC_EFS_MAX_PATH_LEN_BYTES);

    if( i_CharsWritten > 0 &&
        i_CharsWritten < (LOC_EFS_MAX_PATH_LEN_BYTES - 1) )
    {
      /* Add the numerical suffix */
      pz_Loc->filename[i_CharsWritten] = '0' + pz_CertData->suplCertId;
      /* Null terminate the filename */
      pz_Loc->filename[i_CharsWritten + 1] = 0; 
    }
    else
    {
      u_ErrCode = 10;
      break;
    }
    
    pz_Loc->operation   = LOC_FS_CREATE_WRITE_FILE;
    pz_Loc->part        = 1;
    pz_Loc->part_len    = pz_CertData->suplCertData_len;
    pz_Loc->reserved    = 0;
    pz_Loc->total_parts = 1;
    pz_Loc->total_size  = pz_CertData->suplCertData_len;
    return TRUE;
  }while(0);
  
  SHIM_MSG_ERROR("locQmiShimConvToLocInjectSuplCertificate failed. errcode %d", 
                  u_ErrCode, 0, 0);
  return FALSE;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Delete Supl Cert
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocDeleteSuplCertificate
( 
  const void                                   *pz_Msg,
  loc_ioctl_data_u_type                        *pz_IoctlInfo
)
{
  uint8   u_ErrCode       = 0;
  int32   i_CharsWritten  = 0;
  
  const qmiLocDeleteSuplCertificateReqMsgT_v02 *pz_DeleteData = 
                      (qmiLocDeleteSuplCertificateReqMsgT_v02*) pz_Msg; 
  loc_efs_data_s_type *pz_Loc = &(pz_IoctlInfo->efs_data);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed */
  do 
  {
    if ( ( TRUE == pz_DeleteData->suplCertId_valid ) &&
         ( LOC_QMI_SHIM_MAX_SUPL_CERT_ID < pz_DeleteData->suplCertId ) )
    {
      /* The interface allows only 0-9 */
      u_ErrCode = 1;
      break;
    }

    if ( TRUE == pz_DeleteData->suplCertId_valid )
    {
      /* Delete the certificate specified */
      i_CharsWritten = GNSS_STRLCPY ( (char*)pz_Loc->filename, 
                                     pz_LocQmiShimCertLocnPrefix,
                                     LOC_EFS_MAX_PATH_LEN_BYTES);

      if( i_CharsWritten > 0 &&
          i_CharsWritten < (LOC_EFS_MAX_PATH_LEN_BYTES - 1) )
      {
        /* Add the numerical suffix */
        pz_Loc->filename[i_CharsWritten] = '0' + pz_DeleteData->suplCertId;
        /* Null terminate the filename */
        pz_Loc->filename[i_CharsWritten + 1] = 0; 
        pz_Loc->operation = LOC_FS_DELETE_FILE;
      }
      else
      {
        u_ErrCode = 5;
        break;
      }
    }
    else
    {
      /* If suplCertId is not valid, delete all certificates */
      pz_Loc->operation = LOC_FS_PURGE_DIR;

      /* Provide loc api with the dir from which to delete all certificates */
      (void) GNSS_STRLCPY ( (char*)pz_Loc->filename, 
                            pz_LocQmiShimCertDirLocn,
                            LOC_EFS_MAX_PATH_LEN_BYTES);
    }
    /* Init the rest of the struct */
    pz_Loc->part        = 0;
    pz_Loc->part_len    = 0;
    pz_Loc->reserved    = 0;
    pz_Loc->total_parts = 0;
    pz_Loc->total_size  = 0;
    pz_Loc->data_ptr    = NULL;
    
    return TRUE;
  }while(0);

  SHIM_MSG_ERROR("locQmiShimConvToLocDeleteSuplCertificate failed. errcode %d", 
                  u_ErrCode, 0, 0);
  return FALSE;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert from pdsm_extended_time_src_e_type to
  qmiLocTimeSourceEnumT_v02
  
@param[out]  pe_Target        points to target Qmi-Loc data type
@param[in]   ext_time_source          PDAPI format of time source indicator

@retval    TRUE if the conversion was successful 
@retval    FALSE if the conversion failed
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocTimeSource
(
  qmiLocTimeSourceEnumT_v02 * const pe_Target,
  const pdsm_extended_time_src_e_type e_ExtTimeSrc
)
{
  boolean v_RetVal = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if(NULL == pe_Target)
    {
      v_RetVal = FALSE;
      break;
    }

    switch(e_ExtTimeSrc)
    {
    case PDSM_GNSS_TIME_SRC_RESERVED: /* Fall Through */
    case PDSM_GNSS_TIME_SRC_INVALID:
      *pe_Target = eQMI_LOC_TIME_SRC_INVALID_V02;
      break;
    case PDSM_GNSS_TIME_SRC_UNKNOWN:
      *pe_Target = eQMI_LOC_TIME_SRC_UNKNOWN_V02;
      break;
    case PDSM_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER:
      *pe_Target = eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER_V02;
      break;
    case PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING:
      *pe_Target = eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING_V02;
      break;
    case PDSM_GNSS_TIME_SRC_EXTERNAL_INPUT:
      *pe_Target = eQMI_LOC_TIME_SRC_EXTERNAL_INPUT_V02;
      break;
    case PDSM_GNSS_TIME_SRC_GPS_TOW_DECODE:
      *pe_Target = eQMI_LOC_TIME_SRC_TOW_DECODE_V02;
      break;
    case PDSM_GNSS_TIME_SRC_TOW_CONFIRMED:
      *pe_Target = eQMI_LOC_TIME_SRC_TOW_CONFIRMED_V02;
      break;
    case PDSM_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED:
      *pe_Target = eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V02;
      break;
    case PDSM_GNSS_TIME_SRC_NAV_SOLUTION:
      *pe_Target = eQMI_LOC_TIME_SRC_NAV_SOLUTION_V02;
      break;
    case PDSM_GNSS_TIME_SRC_SOLVE_FOR_TIME:
      *pe_Target = eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME_V02;
      break;
    case PDSM_GNSS_TIME_SRC_GLO_TOW_DECODE:
      *pe_Target = eQMI_LOC_TIME_SRC_GLO_TOW_DECODE_V02 ;
      break;
    case PDSM_GNSS_TIME_SRC_TIME_TRANSFORM:
      *pe_Target = eQMI_LOC_TIME_SRC_TIME_TRANSFORM_V02 ;
      break;
    case PDSM_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING:
      *pe_Target = eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V02 ;
      break;
    case PDSM_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING:
      *pe_Target = eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V02 ;
      break;
      
    default:
      v_RetVal = FALSE;
      break;
    }
  } while(0);

  if(TRUE != v_RetVal)
  {
    SHIM_MSG_ERROR("locQmiShimConvToLocTimeSource failed:%d",
                    e_ExtTimeSrc, 0, 0);
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert sensor data usage indicator from PDAPI to Qmi-Loc

  PDAPI does not define constants, and loc-mw just copies the PDAPI value
  over to loc-api structures. 

  As Per PDAPI:
      0x0001 – Accelerometer
      0x0002 – Gyro

@param[out]  pq_Target      points to target Qmi-Loc data type 
@param[in]   usage          PDAPI format of sensor data usage indicator

@retval    TRUE if the conversion was successful 
@retval    FALSE if the conversion failed
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSensorUsage
(
  const uint16                       w_Source,
  qmiLocSensorUsageMaskT_v02 * const pz_Target
)
{
  boolean v_RetVal = TRUE;
  uint8   u_Cntr = 0;

  struct 
  {
    uint16                      w_SrcMask;
    qmiLocSensorUsageMaskT_v02  z_TgtMask;
  }z_SensorUsageMaskMap[] =
      {
        { 0x0001, 
          QMI_LOC_SENSOR_MASK_USED_ACCEL_V02 } ,
        { 0x0002, 
          QMI_LOC_SENSOR_MASK_USED_GYRO_V02 }
      };
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if(NULL == pz_Target)
    {
      v_RetVal = FALSE;
      break;
    }

    ( *pz_Target) = 0;
    for ( u_Cntr = 0;
      u_Cntr < ( sizeof(z_SensorUsageMaskMap) / sizeof (z_SensorUsageMaskMap[0]) );
      u_Cntr ++ )
   {
      if ( z_SensorUsageMaskMap[u_Cntr].w_SrcMask & w_Source )
      {
        (*pz_Target) |= z_SensorUsageMaskMap[u_Cntr].z_TgtMask;
      }
   }

  } while(0);

  if(TRUE != v_RetVal)
  {
    SHIM_MSG_ERROR("locQmiShimConvToLocSensorUsage failed:0x%x",
                    (pz_Target),0,0);
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert sensor aiding indicator from PDAPI to Qmi-Loc

  PDAPI does not define constants, and loc-mw just copies the PDAPI value
  over to loc-api structures. 

  As Per PDAPI:
     0x0001 – Heading aided with sensor data
     0x0002 – Speed aided with sensor data
     0x0004 – Position aided with sensor data
     0x0008 – Velocity aided with sensor data
  
@param[out]  z_Target      Points to target Qmi-Loc data type   
@param[in]   w_Source       PDAPI format of sensor aiding indicator

@retval    TRUE if the conversion was successful 
@retval    FALSE if the conversion failed
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSensorAid
(
  const uint16                       w_Source,
  qmiLocSensorAidedMaskT_v02 * const pz_Target
)
{
  boolean v_RetVal = TRUE;
  uint8   u_Cntr = 0;

  struct 
  {
    uint16                      w_SrcMask;
    qmiLocSensorAidedMaskT_v02  z_TgtMask; 
   
  }z_SensorAidingMaskMap[] =
      {
        { 0x0001, 
          QMI_LOC_SENSOR_AIDED_MASK_HEADING_V02 } ,
        { 0x0002, 
          QMI_LOC_SENSOR_AIDED_MASK_SPEED_V02 } ,
        { 0x0004, 
          QMI_LOC_SENSOR_AIDED_MASK_POSITION_V02 } ,
        { 0x0008, 
          QMI_LOC_SENSOR_AIDED_MASK_VELOCITY_V02 } 
      };
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if(NULL == pz_Target)
    {
      v_RetVal = FALSE;
      break;
    }

    ( *pz_Target) = 0;
    
    for ( u_Cntr = 0;
          u_Cntr < ( sizeof(z_SensorAidingMaskMap) / sizeof (z_SensorAidingMaskMap[0]) );
          u_Cntr ++ )
   {
      if ( z_SensorAidingMaskMap[u_Cntr].w_SrcMask & w_Source )
      {
        (*pz_Target) |= z_SensorAidingMaskMap[u_Cntr].z_TgtMask;
      }
   }
  } while(0);

  if(TRUE != v_RetVal)
  {
    SHIM_MSG_ERROR( "locQmiShimConvToLocSensorAid failed: 0x%x",
                    (pz_Target),0,0);
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert from pdsm_pd_reliability_e_type to 
  qmiLocReliabilityEnumT_v02
  
@param[out]  pe_Target      points to target Qmi-Loc data type   
@param[in]   reliability          PDAPI format of reliability indicator

@retval    TRUE if the conversion was successful 
@retval    FALSE if the conversion failed
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocReliability
(
  qmiLocReliabilityEnumT_v02 * const pe_Target,
  const pdsm_pd_reliability_e_type reliability
)
{
  boolean result = TRUE;
  do
  {
    if(NULL == pe_Target)
    {
      result = FALSE;
      break;
    }

    switch(reliability)
    {
    case PDSM_POSITION_RELIABILITY_NOT_SET:
      *pe_Target = eQMI_LOC_RELIABILITY_NOT_SET_V02;
      break;
    case PDSM_POSITION_RELIABILITY_VERY_LOW:
      *pe_Target = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
      break;
    case PDSM_POSITION_RELIABILITY_LOW:
      *pe_Target = eQMI_LOC_RELIABILITY_LOW_V02;
      break;
    case PDSM_POSITION_RELIABILITY_MEDIUM:
      *pe_Target = eQMI_LOC_RELIABILITY_MEDIUM_V02;
      break;
    case PDSM_POSITION_RELIABILITY_HIGH:
      *pe_Target = eQMI_LOC_RELIABILITY_HIGH_V02;
      break;
    default:
      result = FALSE;
      break;
    }
  } while(0);

  if(TRUE != result)
  {
    SHIM_MSG_ERROR("locQmiShimConvToLocReliability failed",0,0,0);
  }
  return result;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Inform Location Server Connection Status. This is
  not part of the IOCTL Converter table, as one QMI-Request can trigger 
  one-of-the-two LOC-Api
  IOCTLs
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 
@param[out]  pe_LocIoctlType      Loc-Ioctl that has to be triggered

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully

@warning  This function is not part of the function table 
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocInformLocationServerConnStatus
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo,
  loc_ioctl_e_type                            *pe_LocIoctlType
)
{
  boolean v_RetVal = TRUE;
  const qmiLocInformLocationServerConnStatusReqMsgT_v02  *pz_ConnStatus = 
                     (qmiLocInformLocationServerConnStatusReqMsgT_v02*) pz_Msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  switch ( pz_ConnStatus->requestType )
  {
    case eQMI_LOC_SERVER_REQUEST_OPEN_V02:
    {
      loc_server_multi_open_status_s_type *pz_LocOpenStatus = 
                                    &pz_IoctlInfo->multi_conn_open_status;
      pz_LocOpenStatus->conn_handle = 
                   (loc_server_connection_handle)pz_ConnStatus->connHandle;
      switch ( pz_ConnStatus->statusType )
      {
        case eQMI_LOC_SERVER_REQ_STATUS_SUCCESS_V02:
          pz_LocOpenStatus->open_status = LOC_SERVER_OPEN_SUCCESS;
          break;
          
        case eQMI_LOC_SERVER_REQ_STATUS_FAILURE_V02:
          pz_LocOpenStatus->open_status = LOC_SERVER_OPEN_FAIL;
          break;
          
        default:
          v_RetVal = FALSE;
          break;
      } 

      if ( TRUE == pz_ConnStatus->apnProfile_valid )
      {
            (void)GNSS_STRLCPY ( pz_LocOpenStatus->apn_name,
                            pz_ConnStatus->apnProfile.apnName,
                            LOC_MAX_APN_STRING_LEN );
        
        switch ( pz_ConnStatus->apnProfile.pdnType )
        {
          case eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV4_V02:
            pz_LocOpenStatus->pdp_type = LOC_SERVER_PDP_IP;
            break;
          case eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV6_V02:
            pz_LocOpenStatus->pdp_type = LOC_SERVER_PDP_IPV6;
            break;
          case eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV4V6_V02:
            pz_LocOpenStatus->pdp_type = LOC_SERVER_PDP_IPV4V6;
            break;
          case eQMI_LOC_APN_PROFILE_PDN_TYPE_PPP_V02:
            pz_LocOpenStatus->pdp_type = LOC_SERVER_PDP_PPP;
            break;
          default:
            v_RetVal = FALSE;
            break;
        }
      }
      (*pe_LocIoctlType) = LOC_IOCTL_INFORM_SERVER_MULTI_OPEN_STATUS;
    }
    break;

    case eQMI_LOC_SERVER_REQUEST_CLOSE_V02:
    {
      loc_server_close_status_s_type *pz_LocCloseStatus = 
                                    &pz_IoctlInfo->conn_close_status;
      pz_LocCloseStatus->conn_handle = 
                   (loc_server_connection_handle)pz_ConnStatus->connHandle;
      switch ( pz_ConnStatus->statusType )
      {
        case eQMI_LOC_SERVER_REQ_STATUS_SUCCESS_V02:
          pz_LocCloseStatus->close_status = LOC_SERVER_CLOSE_SUCCESS;
          break;
          
        case eQMI_LOC_SERVER_REQ_STATUS_FAILURE_V02:
          pz_LocCloseStatus->close_status = LOC_SERVER_CLOSE_FAIL;
          break;
          
        default:
          v_RetVal = FALSE;
          break;
      } 
     (*pe_LocIoctlType) = LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS;
    }
    break;

    default:
      v_RetVal = FALSE;
      break;
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function To decrypt and decode the Secure Best Available Position Parameter
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pe_LocIoctl          The IOCTL that needs to be executed
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   The IOCTL can be issued 
@retval    FALSE                  Something went wrong, IOCTL cannot be issued
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSecureAvailablePosition 
(
  const locQmiShimMsgInfoStructT         *pz_MsgInfo,
  loc_ioctl_e_type                       *pe_LocIoctl,
  loc_ioctl_data_u_type                  *pz_IoctlInfo
)
{
  boolean v_RetVal = FALSE;
  
  //Decode the data, Then  forward, after proper typecasting
  qmiLocSecureGetAvailablePositionReqMsgT_v02 *q_getEncryptedAvailablePosReq,*q_getAvailablePosReq;
    qmiLocSecureGetAvailablePositionReqMsgT_v02 getDecryptedAvailablePosReq;

  if ( pz_MsgInfo->q_MsgLen > sizeof(qmiLocSecureGetAvailablePositionReqMsgT_v02) )
   {
      return v_RetVal;
   }
   (*pe_LocIoctl) = LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC;
   v_RetVal = TRUE;
   q_getEncryptedAvailablePosReq = (qmiLocSecureGetAvailablePositionReqMsgT_v02 *)pz_MsgInfo->p_Msg;

   SHIM_MSG_ERROR( "Secure Message   : %d   ", q_getEncryptedAvailablePosReq->secureLocDataMode, 0, 0 );
   if(q_getEncryptedAvailablePosReq->secureLocDataMode == eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED_V02)
   {
     //Decrypt the payload
     v_RetVal = locQmiShimDecryptSecureAvailablePositionDataBlob(q_getEncryptedAvailablePosReq,&getDecryptedAvailablePosReq);
     SHIM_MSG_ERROR( "locQmiShimDecryptSecureAvailablePositionDataBlob : %d   ", v_RetVal, 0, 0 );
     if(v_RetVal)
     q_getAvailablePosReq= &getDecryptedAvailablePosReq;
   }
   else
   {
     // No need to Decrypt, Data is un-encrypted
     q_getAvailablePosReq = q_getEncryptedAvailablePosReq;
   }
   // Decode the payload and populate the pz_IoctlInfo structure.
   if(v_RetVal)
   {
     v_RetVal = locQmiShimConvToLocGetSecureAvailablePosition(q_getAvailablePosReq,pz_IoctlInfo);
   }
   if(v_RetVal)
     SHIM_MSG_ERROR( "Secure Message Decoded Transaction ID  : %d   ", pz_IoctlInfo->z_GetAvailablePosReq.transactionId, 0, 0 );
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function for all new LocApi IOCTLs. ( Post LocApi 1.0 ). The new
  IOCTLs use the structures of QMI-LOC, so the only operation to do is to copy 
  the incoming message into the Loc API structure
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pe_LocIoctl          The IOCTL that needs to be executed
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   The IOCTL can be issued 
@retval    FALSE                  Something went wrong, IOCTL cannot be issued
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimCopyToLoc 
(
  const locQmiShimMsgInfoStructT         *pz_MsgInfo,
  loc_ioctl_e_type                       *pe_LocIoctl,
  loc_ioctl_data_u_type                  *pz_IoctlInfo
)
{
  boolean v_RetVal = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /* Not doing check of inputs as this is called from within the module,
    and this will be called with proper parameters 
    Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  /* Poor Mans exception Handling */
  do 
  {
    if ( pz_MsgInfo->q_MsgLen > sizeof(*pz_IoctlInfo) )
    {
      break;
    }

    switch ( pz_MsgInfo->q_Id )
    {
      case QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
        (*pe_LocIoctl) = LOC_IOCTL_SET_PE_CONFIG;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
        (*pe_LocIoctl) = LOC_IOCTL_GET_PE_CONFIG;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02:
       (*pe_LocIoctl) = LOC_IOCTL_GET_BEST_AVAILABLE_POSITION;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ_V02:
       (*pe_LocIoctl) = LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_SECURE_GET_AVAILABLE_POSITION_REQ_V02:
       (*pe_LocIoctl) = LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_INJECT_GSM_CELL_INFO_REQ_V02:
        (*pe_LocIoctl) = LOC_IOCTL_INJECT_GSM_CELL_INFO_REQ;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_INJECT_NETWORK_INITIATED_MESSAGE_REQ_V02:
        (*pe_LocIoctl) = LOC_IOCTL_INJECT_NI_MESSAGE_REQ;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_WWAN_OUT_OF_SERVICE_NOTIFICATION_REQ_V02:
        (*pe_LocIoctl) = LOC_IOCTL_WWAN_OOS_NOTIFICATION_REQ;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_REQ_V02:
       (*pe_LocIoctl) = LOC_IOCTL_SET_PREMIUM_SERVICES_CONFIG;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_SET_XTRA_VERSION_CHECK_REQ_V02:
        (*pe_LocIoctl) = LOC_IOCTL_SET_XTRA_VERSION_CHECK;
        v_RetVal = TRUE;
        break;
      case QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02:
        (*pe_LocIoctl) = LOC_IOCTL_DELETE_GNSS_SERVICE_DATA;
        v_RetVal = TRUE;
        break;

      default:
        v_RetVal = FALSE;
        break;
    }

    if ( TRUE == v_RetVal )
    {
      /* The length will be enough, as the same structure 
         is what the QCSI decodes data into */
      memscpy( pz_IoctlInfo, sizeof(*pz_IoctlInfo),
               pz_MsgInfo->p_Msg,
               pz_MsgInfo->q_MsgLen );
    }
    else
    { 
      /* Could not handle the message */
      v_RetVal = FALSE;
      break;
    }

  } while(0);
  
 return v_RetVal;
}


/*============================================================================
 *                    M E S S A G E    C O N V E R T O R S 
 *                         LOC-Api --> QMI
 * =========================================================================*/

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert NI Notification Type from Loc-Api to QMI-Loc
  
@param[in]   e_LocNiNotification               Loc-API Format of Message
@param[out]  e_QmiNiNotification               Qmi-Loc IOCTL Format 

@retval    LOC_API_SUCCESS        Converted successfully
@retval    <Anything Else>        DId not convert successfully
*//* ------------------------------------------------------------------------*/
static int32 locQmiShimConvToQmiNiNotificationType
(
  loc_ni_notify_verify_e_type         e_LocNiNotification,
  qmiLocNiNotifyVerifyEnumT_v02      *e_QmiNiNotification
)
{
  int32 i_RetVal = LOC_API_SUCCESS; /*< This return value means that the ioctl 
                      callback was handled successfully */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( e_LocNiNotification )
  {
    case LOC_NI_USER_NO_NOTIFY_NO_VERIFY:
    {
      (*e_QmiNiNotification) = 
                              eQMI_LOC_NI_USER_NO_NOTIFY_NO_VERIFY_V02;
    }
    break;
    case LOC_NI_USER_NOTIFY_ONLY:
    {
      (*e_QmiNiNotification) = eQMI_LOC_NI_USER_NOTIFY_ONLY_V02;
    }
    break;
    case LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
    {
      (*e_QmiNiNotification) = 
                        eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02;
    }
    break;
    case LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
    {
      (*e_QmiNiNotification) = 
                    eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02;
    }
    break;
    
    case LOC_NI_USER_PRIVACY_OVERRIDE:
    {
      (*e_QmiNiNotification) = 
                    eQMI_LOC_NI_USER_NOTIFY_VERIFY_PRIVACY_OVERRIDE_V02;
    }
    break;
    
    default:
    {
      i_RetVal = LOC_API_INVALID_PARAMETER;
    }  
    break;
  }

  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert NI Format Type from Loc-Api to QMI-Loc
  
@param[in]   q_LocFormatType               Loc-API Format of Message
@param[out]  e_QmiNiFormatType             Qmi-Loc IOCTL Format 

@retval    LOC_API_SUCCESS        Converted successfully
@retval    <Anything Else>        DId not convert successfully
*//* ------------------------------------------------------------------------*/
static int32 locQmiShimConvToQmiNiFormatType
(
  uint32                              q_LocFormatType,
  qmiLocNiSuplFormatEnumType_v02     *e_QmiNiFormatType
)
{
  int32 i_RetVal = LOC_API_SUCCESS; /*< This return value means that the ioctl 
                      callback was handled successfully */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( q_LocFormatType )
  {
    case LOC_NI_SUPL_FORMAT_LOGICAL_NAME:
      (*e_QmiNiFormatType) = 
                  eQMI_LOC_NI_SUPL_FORMAT_LOGICAL_NAME_V02;
      break;
    case LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS:
      (*e_QmiNiFormatType) = 
                  eQMI_LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS_V02;
      break;
    case LOC_NI_SUPL_FORMAT_MSISDN:
      (*e_QmiNiFormatType) = 
                  eQMI_LOC_NI_SUPL_FORMAT_MSISDN_V02;
      break;
    case LOC_NI_SUPL_FORMAT_URL:
      (*e_QmiNiFormatType) = 
                  eQMI_LOC_NI_SUPL_FORMAT_URL_V02;
      break;
    case LOC_NI_SUPL_FORMAT_SIP_URL:
      (*e_QmiNiFormatType) = 
                  eQMI_LOC_NI_SUPL_FORMAT_SIP_URL_V02;
      break;
    case LOC_NI_SUPL_FORMAT_MIN:
      (*e_QmiNiFormatType) = 
                  eQMI_LOC_NI_SUPL_FORMAT_MIN_V02;
      break;
    case LOC_NI_SUPL_FORMAT_MDN:
      (*e_QmiNiFormatType) = 
                  eQMI_LOC_NI_SUPL_FORMAT_MDN_V02;
      break;
    case LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY:
      (*e_QmiNiFormatType) =
                  eQMI_LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY_V02;
      break;
    case LOC_NI_SUPL_FORMAT_OSS_UNKNOWN:
      (*e_QmiNiFormatType) = 
                  eQMI_LOC_NI_SUPL_FORMAT_OSS_UNKNOWN_V02;
      break;
      
    default:
    {
      i_RetVal = LOC_API_INVALID_PARAMETER;
      SHIM_MSG_ERROR( "Invalid Option: %d", 
                      q_LocFormatType, 
                      0, 0 );
    }  
    break;
  }

  return i_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert NI Data Coding Scheme from Loc-Api to QMI-Loc
  
@param[in]   e_LocDataCodingScheme               Loc-API Format of Message
@param[out]  e_QmiNiDataCodingScheme             Qmi-Loc IOCTL Format 

@retval    LOC_API_SUCCESS        Converted successfully
@retval    <Anything Else>        DId not convert successfully
*//* ------------------------------------------------------------------------*/
static int32 locQmiShimConvToQmiNiDataCodingScheme
(
  loc_ni_datacoding_scheme_e_type      e_LocDataCodingScheme,
  qmiLocNiDataCodingSchemeEnumT_v02   *e_QmiNiDataCodingScheme
)
{
  int32 i_RetVal = LOC_API_SUCCESS; /*< This return value means that the ioctl 
                      callback was handled successfully */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (e_LocDataCodingScheme )
  { 
    case LOC_NI_SS_GERMAN:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_GERMAN_V02;
      break;
    case LOC_NI_SS_ENGLISH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_ENGLISH_V02;
      break;
    case LOC_NI_SS_ITALIAN:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_ITALIAN_V02;
      break;
    case LOC_NI_SS_FRENCH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_FRENCH_V02;
      break;
    case LOC_NI_SS_SPANISH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_SPANISH_V02;
      break;
    case LOC_NI_SS_DUTCH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_SWEDISH_V02;
      break;
    case LOC_NI_SS_SWEDISH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_SWEDISH_V02;
      break;
    case LOC_NI_SS_DANISH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_DANISH_V02;
      break;
    case LOC_NI_SS_PORTUGUESE:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_PORTUGUESE_V02;
      break;
    case LOC_NI_SS_FINNISH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_FINNISH_V02;
      break;
    case LOC_NI_SS_NORWEGIAN:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_GREEK_V02;
      break;
    case LOC_NI_SS_GREEK:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_GREEK_V02;
      break;
    case LOC_NI_SS_TURKISH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_TURKISH_V02;
      break;
    case LOC_NI_SS_HUNGARIAN:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_HUNGARIAN_V02;
      break;
    case LOC_NI_SS_POLISH:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_POLISH_V02;
      break;
    case LOC_NI_SS_LANGUAGE_UNSPEC:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SS_LANGUAGE_UNSPEC_V02;
      break;
    case LOC_NI_SUPL_UTF8:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SUPL_UTF8_V02;
      break;
    case LOC_NI_SUPL_UCS2:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SUPL_UCS2_V02;
      break;
    case LOC_NI_SUPL_GSM_DEFAULT:
      (*e_QmiNiDataCodingScheme) = 
                  eQMI_LOC_NI_SUPL_GSM_DEFAULT_V02;
      break;

    /* There are some enums that are not translated */
    default:
    {
      i_RetVal = LOC_API_INVALID_PARAMETER;
      SHIM_MSG_ERROR( "Invalid Option: %d", 
                      e_LocDataCodingScheme, 0, 0 );
    }  
    break;
  }

  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Loc-Api Event Status to QMi-LOC format
  
@param[in]   q_Status                 Loc-API Format of Message
@param[out]  pe_QmiStatus             Qmi-Loc IOCTL Format 

@retval    LOC_API_SUCCESS        Converted successfully
@retval    <Anything Else>        DId not convert successfully
*//* ------------------------------------------------------------------------*/
static int32 locQmiShimConvToQmiEventStatus
(
  int32                                q_Status,
  qmiLocStatusEnumT_v02               *pe_QmiStatus
)
{
  int32 i_RetVal = LOC_API_SUCCESS; /*< This return value means that the ioctl 
                      callback was handled successfully */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do Conversion of the IOCTL Status */
  switch ( q_Status ) 
  {
    case LOC_API_SUCCESS:
      (*pe_QmiStatus) = eQMI_LOC_SUCCESS_V02;
      break;
    case LOC_API_GENERAL_FAILURE:
      (*pe_QmiStatus) = eQMI_LOC_GENERAL_FAILURE_V02;
      break;
    case LOC_API_UNSUPPORTED:
      (*pe_QmiStatus) = eQMI_LOC_UNSUPPORTED_V02;
      break;
    case LOC_API_INVALID_HANDLE:
    case LOC_API_INVALID_PARAMETER:
      (*pe_QmiStatus) = eQMI_LOC_INVALID_PARAMETER_V02;
      break;
    case LOC_API_ENGINE_BUSY:
      (*pe_QmiStatus) = eQMI_LOC_ENGINE_BUSY_V02;
      break;
    case LOC_API_PHONE_OFFLINE:
      (*pe_QmiStatus) = eQMI_LOC_PHONE_OFFLINE_V02;
      break;
    case LOC_API_TIMEOUT:
      (*pe_QmiStatus) = eQMI_LOC_TIMEOUT_V02;
      break;
    case LOC_API_XTRA_VERSION_CHECK_FAILURE:
      (*pe_QmiStatus) = eQMI_LOC_XTRA_VERSION_CHECK_FAILURE_V02;
      break;
    default:
      SHIM_MSG_ERROR( "Invalid IOCTL Status %d Exec-path should not reach here",
                      q_Status, 0, 0 );
      i_RetVal = LOC_API_INVALID_PARAMETER;
      break;
  }


  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Loc-Api status to QMI Response format
  
@param [in] q_Status : Loc-API Format of Message
@retval     QMI Response Error 

*//* ------------------------------------------------------------------------*/
static qmi_error_type_v01 locQmiShimConvToQmiError 
(
  int32 q_Status
)

{
  qmi_error_type_v01 qmiErr; 

  /* Do Conversion of the IOCTL Status */
  switch ( q_Status ) 
  {
    case LOC_API_SUCCESS:
      qmiErr = QMI_ERR_NONE_V01;
      break;
    case LOC_API_GENERAL_FAILURE:
      qmiErr = QMI_ERR_GENERAL_V01;
      break;
    case LOC_API_UNSUPPORTED:
      qmiErr = QMI_ERR_NOT_SUPPORTED_V01;
      break;
    case LOC_API_INVALID_HANDLE:
       qmiErr = QMI_ERR_INTERNAL_V01;
      break;
    case LOC_API_INVALID_PARAMETER:
      qmiErr = QMI_ERR_MALFORMED_MSG_V01;
      break;
    case LOC_API_ENGINE_BUSY:
      qmiErr = QMI_ERR_DEVICE_IN_USE_V01;
      break;
    case LOC_API_PHONE_OFFLINE:
      qmiErr = QMI_ERR_NO_RADIO_V01;
      break;
    case LOC_API_TIMEOUT:
      qmiErr = QMI_ERR_INTERNAL_V01;
      break;
    default:
      SHIM_MSG_ERROR( "Invalid IOCTL Status %d Exec-path should not reach here",
                      q_Status, 0, 0 );
      qmiErr = QMI_ERR_INTERNAL_V01;
      break;
  }

  return qmiErr;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert NI Vx Notify Verify Message Loc-Api to QMI-Loc
  
@param[in]   pz_NiVx               Qmi-Loc Format of Message
@param[out]  pz_QmiLoc             Loc-API IOCTL Format 

@retval    LOC_API_SUCCESS        Converted successfully
@retval    <Anything Else>        DId not convert successfully
*//* ------------------------------------------------------------------------*/
static int32 locQmiShimConvToQmiNiVxNotifyVerify
(
  const loc_ni_vx_notify_verify_req_s_type   *pz_NiVx,
  qmiLocNiVxNotifyVerifyStructT_v02          *pz_QmiLoc
)
{
  int32 i_RetVal = LOC_API_SUCCESS; /*< This return value means that the ioctl 
                      callback was handled successfully */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  if ( LOC_API_SUCCESS == i_RetVal ) 
  {
    switch ( pz_NiVx->pos_mode)
    {
      case LOC_VX_MS_ASSISTED_ONLY:
      {
        pz_QmiLoc->posMode = 
                                eQMI_LOC_NI_VX_MS_ASSISTED_ONLY_V02;
      }
      break;
      case LOC_VX_MS_BASED_ONLY:
      {
        pz_QmiLoc->posMode = eQMI_LOC_NI_VX_MS_BASED_ONLY_V02;
      }
      break;
      case LOC_VX_MS_ASSISTED_PREF_MSBASED_ALLWD:
      {
        pz_QmiLoc->posMode = 
               eQMI_LOC_NI_VX_MS_ASSISTED_PREFERRED_MS_BASED_ALLOWED_V02;
      }
      break;
      case LOC_VX_MS_BASED_PREF_ASSISTED_ALLWD:
      {
        pz_QmiLoc->posMode = 
               eQMI_LOC_NI_VX_MS_BASED_PREFERRED_MS_ASSISTED_ALLOWED_V02;
      }
      break;
  
      default:
      {
        i_RetVal = LOC_API_INVALID_PARAMETER;
      }  
      break;
    }          
  }
  
  if ( LOC_API_SUCCESS == i_RetVal ) 
  {
    switch ( pz_NiVx->encoding_scheme)
    {
      case LOC_NI_VX_OCTET:
      {
        pz_QmiLoc->encodingScheme = 
                                eQMI_LOC_NI_VX_OCTET_V02;
      }
      break;
      case LOC_NI_VX_EXN_PROTOCOL_MSG:
      {
        pz_QmiLoc->encodingScheme = eQMI_LOC_NI_VX_EXN_PROTOCOL_MSG_V02;
      }
      break;
      case LOC_NI_VX_ASCII:
      {
        pz_QmiLoc->encodingScheme = 
               eQMI_LOC_NI_VX_ASCII_V02;
      }
      break;
      case LOC_NI_VX_IA5:
      {
        pz_QmiLoc->encodingScheme = 
               eQMI_LOC_NI_VX_IA5_V02;
      }
      break;
      case LOC_NI_VX_UNICODE:
      {
        pz_QmiLoc->encodingScheme = 
               eQMI_LOC_NI_VX_UNICODE_V02;
      }
      break;
      case LOC_NI_VX_SHIFT_JIS:
      {
        pz_QmiLoc->encodingScheme = 
               eQMI_LOC_NI_VX_SHIFT_JIS_V02;
      }
      break;
      case LOC_NI_VX_KOREAN:
      {
        pz_QmiLoc->encodingScheme = 
               eQMI_LOC_NI_VX_KOREAN_V02;
      }
      break;
      case LOC_NI_VX_LATIN_HEBREW:
      {
        pz_QmiLoc->encodingScheme = 
               eQMI_LOC_NI_VX_LATIN_HEBREW_V02;
      }
      break;
      case LOC_NI_VX_LATIN:
      {
        pz_QmiLoc->encodingScheme = 
               eQMI_LOC_NI_VX_LATIN_V02;
      }
      break;
      case LOC_NI_VX_GSM:
      {
        pz_QmiLoc->encodingScheme = 
               eQMI_LOC_NI_VX_GSM_V02;
      }
      break;
  
      default:
      {
        i_RetVal = LOC_API_INVALID_PARAMETER;
      }  
      break;
    }          
  }
  
  
  if ( LOC_API_SUCCESS == i_RetVal )
  {

    pz_QmiLoc->requestorId_len = locQmiShimMemcpy(
                                  pz_NiVx->requester_id.requester_id_length,
                                  (void*)pz_NiVx->requester_id.requester_id,
                                  QMI_LOC_NI_MAX_REQUESTOR_ID_LENGTH_V02,
                                  (void*)pz_QmiLoc->requestorId );

    pz_QmiLoc->posQosIncl       = pz_NiVx->pos_qos_incl;
    pz_QmiLoc->posQos           = pz_NiVx->pos_qos;
    pz_QmiLoc->numFixes         = pz_NiVx->num_fixes;
    pz_QmiLoc->timeBetweenFixes = pz_NiVx->tbf;
    pz_QmiLoc->userRespTimerInSeconds = pz_NiVx->user_resp_timer_val;
  }
  
  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Fix Criteria from Loc-Api to Qmi-Loc
  
@param[in]   pz_LocFixCrit         Loc-Api Format of Message
@param[out]  pz_QmiLocFixCrit             Qmi-Loc Format 

@retval    TRUE        Converted successfully
@retval    FALSE       DId not convert successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToQmiFixCriteria
(
  const loc_fix_criteria_s_type        *pz_LocFixCrit,
  qmiLocGetFixCriteriaIndMsgT_v02      *pz_QmiLocFixCrit
)
{
  boolean v_RetVal = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  if ( pz_LocFixCrit->valid_mask & LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY )
  {
    pz_QmiLocFixCrit->horizontalAccuracyLevel_valid = TRUE;

    if ( LOC_ACCURACY_HIGH == pz_LocFixCrit->preferred_accuracy )
    {
      pz_QmiLocFixCrit->horizontalAccuracyLevel =  eQMI_LOC_ACCURACY_LOW_V02; 
    }
    else if ( LOC_ACCURACY_MED == pz_LocFixCrit->preferred_accuracy )
    {
        pz_QmiLocFixCrit->horizontalAccuracyLevel =  eQMI_LOC_ACCURACY_MED_V02; 
    }
    else if ( LOC_ACCURACY_LOW == pz_LocFixCrit->preferred_accuracy )
    {
        pz_QmiLocFixCrit->horizontalAccuracyLevel =  eQMI_LOC_ACCURACY_HIGH_V02; 
    }
    else
    {
      /* Invalid input, rejecting */
       v_RetVal = FALSE;
    }
  }

  
  pz_QmiLocFixCrit->intermediateReportState_valid = TRUE;
  if ( ( TRUE == v_RetVal ) &&
       ( pz_LocFixCrit->valid_mask & 
                LOC_FIX_CRIT_VALID_INTERMEDIATE_POS_REPORT_ENABLED )  )
  {
    pz_QmiLocFixCrit->intermediateReportState = 
                                eQMI_LOC_INTERMEDIATE_REPORTS_ON_V02;
  }
  else
  {
    pz_QmiLocFixCrit->intermediateReportState = 
                                eQMI_LOC_INTERMEDIATE_REPORTS_OFF_V02;
  }

  if ( ( TRUE == v_RetVal ) &&
       ( pz_LocFixCrit->valid_mask & LOC_FIX_CRIT_VALID_MIN_INTERVAL ) )
  {
    LOC_QMI_SHIM_SET_VALUE ( pz_QmiLocFixCrit->minInterval,
                             pz_LocFixCrit->min_interval );
  }

  /* Copy aplication Id */
   if ( ( TRUE == v_RetVal ) &&
       ( pz_LocFixCrit->valid_mask & LOC_FIX_CRIT_VALID_APPLICATION_ID ) )
  {
     /*set the application_id valid bit*/
    pz_QmiLocFixCrit->applicationId_valid = TRUE;

    /*copy application Id to the QMI LOC application Id field*/
    locQmiShimMemcpy(sizeof(pz_LocFixCrit->application_id),
                     (void *)&pz_LocFixCrit->application_id, //src
                     sizeof(pz_QmiLocFixCrit->applicationId),
                     (void *)&pz_QmiLocFixCrit->applicationId); //dst
  }

  /* Other fields in QMI-Loc Are not supported yet */
  
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert RegEventMask from Loc-Api to QMI-Loc
  
@param[in]   pt_LocRegMask        Loc-Api Format of Reg Event Mask
@param[out]  pt_QmiRegMask        QMI-Loc Format of Reg Event Mask

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static int32 locQmiShimConvToQmiEventRegMask
(
  const loc_event_mask_type  * pt_LocRegMask,
  uint64                     * pt_QmiRegMask
)
{
  int32 i_RetVal = LOC_API_SUCCESS;
  uint8 ctr = 0;
  uint64  t_RegMask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  (*pt_QmiRegMask ) = 0;

  for ( ctr = 0;
        ctr < ( ( sizeof( z_LocQmiShimMaskMap ) ) / 
                          ( sizeof( z_LocQmiShimMaskMap[0] ) ) ) ;
        ctr ++ )
  {
    if ( z_LocQmiShimMaskMap[ctr].t_LocMask & (*pt_LocRegMask) )
    {
      t_RegMask |= z_LocQmiShimMaskMap[ctr].t_QmiMask;
    }
  }

  (*pt_QmiRegMask) = t_RegMask;
  
  return i_RetVal; 
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert IOCTL reports from Loc-Api to 
  QMI LOC Set Protocol Config indication 
  
@param [in]   e_IoctlEvent          Event identifier
@param [out]  pz_IndInfo            Structure to be filled in with indication 
@param [in]  e_IndStatus           indication status                        

@retval  TRUE: if conversion succeeded
         FALSE: if ioctl wasn't found 

*//* ------------------------------------------------------------------------*/

static boolean  locQmiShimConvIoctlReportToSetProtocolConfig(
       loc_ioctl_e_type                       e_IoctlEvent,
       locQmiShimMsgInfoStructT              *pz_IndInfo,
       qmiLocStatusEnumT_v02                  e_IndStatus)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
  qmiLocSetProtocolConfigParametersIndMsgT_v02 *pz_QmiLocSetProtInd =
    (qmiLocSetProtocolConfigParametersIndMsgT_v02*)pz_IndInfo->p_Msg;

  pz_QmiLocSetProtInd->status = e_IndStatus;

  if (eQMI_LOC_SUCCESS_V02 != e_IndStatus )
  {
    pz_QmiLocSetProtInd->failedProtocolConfigParamMask_valid = TRUE;
    /* Since muxing is not supported there will ever only
       be one failedProtocol per indication. Setting the appropriate
       failedProtocolMask here. */

    switch( e_IoctlEvent )
    {
      case LOC_IOCTL_SET_SUPL_SECURITY:
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_SECURITY_V02;
        break;
      }

      case LOC_IOCTL_SET_VX_CONFIG :
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_VX_VERSION_V02;
        break;
      }

      case LOC_IOCTL_SET_SUPL_VERSION :
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_VERSION_V02;
        break;
      }

      case LOC_IOCTL_SET_LPP_CONFIG :
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_LPP_CONFIG_V02;
        break;
      }

      case LOC_IOCTL_SET_ASSISTED_GLO_CONFIG :
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_ASSISTED_GLONASS_PROTOCOL_V02;
        break;
      }

      case LOC_IOCTL_SET_EMERGENCY_PROTOCOL :
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_EMERGENCY_PROTOCOL_V02;
        break;
      }
      case LOC_IOCTL_SET_SUPL_HASH_ALGO :
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_HASH_ALGO_V02;
        break;
      }

      case LOC_IOCTL_SET_SUPL_TLS_VERSION :
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_TLS_VERSION_V02;
        break;
      }

      case LOC_IOCTL_SET_WIFI_SCAN_INJECT_TIMEOUT :
      {
        pz_QmiLocSetProtInd->failedProtocolConfigParamMask = 
          QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_WIFI_SCAN_INJECT_TIMEOUT_V02;
        break;
      }

      default :
      {
        /* Should never be hit*/
        SHIM_MSG_ERROR("locQmiShimConvSetProtocolConfigIoctlReport :"
                       "Error converting Set Protocol config indication",
                       0,0,0);
        return FALSE;
      }
    }
  }

  pz_IndInfo->q_Id = QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02;
  pz_IndInfo->q_MsgLen = sizeof(*pz_QmiLocSetProtInd);

  return TRUE;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Encode  LOC API position report parameters in PTLV data format to populate the data blob of Get Secure Location API
  
@param [in]   param               Parameter type getting Encoded
@param [in]  dataType            Data Type used for Encoding
@param [in]  length                 length of Data type
@param [in]  value                  Data Value to be encoded
@param [in]  value_size           data Size to be encoded
@param [OUT]   pz_dstEncodedgetAvailablePosRsp         Destination buffer to hold Encoded Data

@retval  size_t  Size of data getting encoded and populated in the Destinantion buffer

*//* ------------------------------------------------------------------------*/


size_t populateGetAvailableLocationPosRpt(unsigned char * pz_dstEncodedgetAvailablePosRsp,qmiLocSecureGetAvailablePositionIndParamIDEnumT_v02 param, 
  qmiLocSecureMessageDataTypeEnumT_v02 dataType, uint8 length, void *value,
  size_t value_size)
{

  int k = 0;
  unsigned char * start = pz_dstEncodedgetAvailablePosRsp;
  unsigned char * valstart = NULL;
  uint32 uint32_PlaceHolder;
  uint8 uint8_PlaceHolder;
  uint32 array_count = 0;
  size_t valueSize ;

  /* Ensure that NULL pointers are not passed in */
  if( NULL == pz_dstEncodedgetAvailablePosRsp || 
      NULL == value )
  {
    return 0;
  }        
  
  /***Encode in PTLV Format****/

  /* Encode PARAM ID  -- 4 bytes  */
  uint32_PlaceHolder = Swap4Bytes(param);
  memscpy(pz_dstEncodedgetAvailablePosRsp,sizeof (qmiLocSecureGetAvailablePositionIndParamIDEnumT_v02),
    &uint32_PlaceHolder, sizeof (qmiLocSecureGetAvailablePositionIndParamIDEnumT_v02));
  pz_dstEncodedgetAvailablePosRsp += sizeof (qmiLocSecureGetAvailablePositionIndParamIDEnumT_v02);

  /* Encode TYPE  -- 4 bytes  */
  uint32_PlaceHolder = Swap4Bytes(dataType);
  memscpy(pz_dstEncodedgetAvailablePosRsp,sizeof(qmiLocSecureMessageDataTypeEnumT_v02),
    &uint32_PlaceHolder, sizeof (qmiLocSecureMessageDataTypeEnumT_v02));
  pz_dstEncodedgetAvailablePosRsp += sizeof (qmiLocSecureMessageDataTypeEnumT_v02);

  /* Encode Length  -- 1 bytes */
  uint8_PlaceHolder = (length);
  memscpy(pz_dstEncodedgetAvailablePosRsp,sizeof(length),
    &uint8_PlaceHolder, sizeof( length));
  pz_dstEncodedgetAvailablePosRsp += sizeof (length);

  /*Encode the data part */
  valueSize = sizeofDataType(dataType);
  if ((valueSize*length) != value_size)
  {
    SHIM_MSG_ERROR( "populateGetAvailableLocationPosRpt:  Invalid param value length for ID %d", param, 0, 0 );
    //reset the dst pointer to start 
    pz_dstEncodedgetAvailablePosRsp  = start;
    return 0;
  }

  valstart = (unsigned char *)value;
  while (array_count < length)
  {
    swapbytes(valstart, valueSize);
    valstart = valstart + valueSize;
    array_count++;
  }
  memscpy(pz_dstEncodedgetAvailablePosRsp,value_size,value,(value_size));
  pz_dstEncodedgetAvailablePosRsp += value_size;
  return  (pz_dstEncodedgetAvailablePosRsp - start);

}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Encode  IOCTL reports from Loc-Api to Get Available Secure Position data blob
  
@param [in]   z_GetAvailablePosInd          Input QMI formatted data with result
@param [out]   pz_GetAvailablePosition     Encoded Payload of the event
@param [in]  e_IndStatus           indication status                        

@retval  TRUE: if conversion succeeded
         FALSE: if ioctl wasn't found 

*//* ------------------------------------------------------------------------*/

static boolean locQmiShimEncodeIoctlReportToGetAvailableGnssPos(
  qmiLocGetAvailablePositionIndParamMsgT_v02 z_GetAvailablePosInd,
  qmiLocSecureGetAvailablePositionIndMsgT_v02* pz_GetAvailablePosition,
  qmiLocStatusEnumT_v02 e_IndStatus
)
{
  qmiLocGetAvailablePositionIndParamMsgT_v02 *pz_GetAvailPosIndRptMsg;
  secerrno_enum_type status = E_SUCCESS;
  boolean e_IndStatusAvailPos = TRUE;
  uint8* output_encoded_msg_ptr = NULL;
  uint32 output_encoded_msg_len;

  /* Encode the Payload */
  uint32 rc;

  SHIM_MSG_MED( "Secure Message payload ", 0, 0, 0 );
  SHIM_MSG_MED( "z_GetAvailablePosInd.TransactionID  %d  ", z_GetAvailablePosInd.transactionId, 0, 0 );
  SHIM_MSG_MED( "z_GetAvailablePosInd Data Mode %d  ", z_GetAvailablePosInd.get_secure_available_pos_data_mode, 0, 0 );

  do{
    pz_GetAvailPosIndRptMsg = 
     (qmiLocGetAvailablePositionIndParamMsgT_v02 *)os_MemAlloc(sizeof(qmiLocGetAvailablePositionIndParamMsgT_v02),OS_MEM_SCOPE_TASK);
    if(NULL == pz_GetAvailPosIndRptMsg)
    {
      SHIM_MSG_ERROR( "pz_GetAvailPosIndRptMsg Memory Allocation failed",0,0, 0 );
      e_IndStatusAvailPos = FALSE;
      break;
    }
    memset((void*)pz_GetAvailPosIndRptMsg, 0, sizeof(qmiLocGetAvailablePositionIndParamMsgT_v02));
    /* make a local copy of the QMI structured response to be set back to client */
    locQmiShimMemcpy(sizeof (qmiLocGetAvailablePositionIndParamMsgT_v02),&z_GetAvailablePosInd,
      sizeof (qmiLocGetAvailablePositionIndParamMsgT_v02),pz_GetAvailPosIndRptMsg);
    /* Copy the status to the Data Blob before encoding */
    pz_GetAvailPosIndRptMsg->status = e_IndStatus;

    /* Encode the Position in PTLV format
       data is in z_GetAvailablePosInd
       Output Encoded data needs to be in pz_GetAvailablePosition */
    {
      unsigned char *pz_dstEncodedgetAvailablePosRsp = NULL ;
      unsigned char *pz_dstEncodedgetAvailablePosRsp_Cpy = NULL;
      size_t size_perMsg = 0;
      uint32 qmilocSecureGetAvailablePositionInd_len = 0;
      pz_dstEncodedgetAvailablePosRsp = 
        (unsigned char *)os_MemAlloc(sizeof(pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd),OS_MEM_SCOPE_TASK);
      if(pz_dstEncodedgetAvailablePosRsp == NULL)
      {
        SHIM_MSG_ERROR( "qmilocSecureGetAvailablePositionInd_len Memory Allocation failed",0,0, 0 );
        e_IndStatusAvailPos = FALSE;
        break;
      }
      memset(pz_dstEncodedgetAvailablePosRsp, 0x0,
        ((size_t)QMI_LOC_SECURE_GET_AVAILABLE_POSITION_IND_ENCRYPTED_MAX_V02 * sizeof(uint8)));
      pz_dstEncodedgetAvailablePosRsp_Cpy = pz_dstEncodedgetAvailablePosRsp;
      if(pz_GetAvailPosIndRptMsg->transactionId_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_TRANSACTION_ID_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->transactionId, sizeof(pz_GetAvailPosIndRptMsg->transactionId));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
  
      if(pz_GetAvailPosIndRptMsg->nonce_valid)
      {
        size_perMsg = populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
         eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_NONCE_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_64_V02, 1,
         (void *)&pz_GetAvailPosIndRptMsg->nonce, sizeof(pz_GetAvailPosIndRptMsg->nonce));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->latitude_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_LATITUDE_V02, eQMI_LOC_SECURE_DATA_TYPE_DOUBLE_64_V02, 1, 
          (void *)&pz_GetAvailPosIndRptMsg->latitude, sizeof(pz_GetAvailPosIndRptMsg->latitude));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->longitude_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_LONGITUDE_V02, eQMI_LOC_SECURE_DATA_TYPE_DOUBLE_64_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->longitude, sizeof(pz_GetAvailPosIndRptMsg->longitude));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horUncCircular_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HORIZ_UNC_CIRCULAR_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->horUncCircular, sizeof(pz_GetAvailPosIndRptMsg->horUncCircular));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->altitudeWrtEllipsoid_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_ALTITUDE_WRT_ELP_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->altitudeWrtEllipsoid, sizeof(pz_GetAvailPosIndRptMsg->altitudeWrtEllipsoid));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->vertUnc_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_VERTICAL_UNC_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->vertUnc, sizeof(pz_GetAvailPosIndRptMsg->vertUnc));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->timestampUtc_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_TIME_STAMP_UTC_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_64_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->timestampUtc, sizeof(pz_GetAvailPosIndRptMsg->timestampUtc));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->timeUnc_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_TIME_UNC_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->timeUnc, sizeof(pz_GetAvailPosIndRptMsg->timeUnc));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horUncEllipseSemiMinor_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HORIZ_UNC_ELP_SEMIMINOR_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->horUncEllipseSemiMinor, sizeof(pz_GetAvailPosIndRptMsg->horUncEllipseSemiMinor));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horUncEllipseSemiMajor_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HORIZ_UNC_ELP_SEMIMAJOR_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->horUncEllipseSemiMajor, sizeof(pz_GetAvailPosIndRptMsg->horUncEllipseSemiMajor));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horUncEllipseOrientAzimuth_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HORIZ_UNC_ELP_ORIENT_AZIMUTH_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->horUncEllipseOrientAzimuth, sizeof(pz_GetAvailPosIndRptMsg->horUncEllipseOrientAzimuth));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horCircularConfidence_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HORIZ_CONFIDENCE_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_8_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->horCircularConfidence, sizeof(pz_GetAvailPosIndRptMsg->horCircularConfidence));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horEllipticalConfidence_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HORIZ_ELLIP_CONFIDENCE_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_8_V02,
          1, (void *)&pz_GetAvailPosIndRptMsg->horEllipticalConfidence, sizeof(pz_GetAvailPosIndRptMsg->horEllipticalConfidence));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horReliability_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HORIZ_RELIABILITY_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->horReliability, sizeof(pz_GetAvailPosIndRptMsg->horReliability));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horSpeed_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_SPEED_HORIZ_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->horSpeed, sizeof(pz_GetAvailPosIndRptMsg->horSpeed));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->horSpeedUnc_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_SPEED_HORIZ_UNC_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->horSpeedUnc, sizeof(pz_GetAvailPosIndRptMsg->horSpeedUnc));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->altitudeWrtMeanSeaLevel_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_ALTITUDE_WRT_MSA_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->altitudeWrtMeanSeaLevel, sizeof(pz_GetAvailPosIndRptMsg->altitudeWrtMeanSeaLevel));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->vertConfidence_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_VERTICAL_CONF_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_8_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->vertConfidence, sizeof(pz_GetAvailPosIndRptMsg->vertConfidence));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->vertReliability_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_VERT_RELIABILITY_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->vertReliability, sizeof(pz_GetAvailPosIndRptMsg->vertReliability));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->vertSpeed_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_SPEED_VERTICAL_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->vertSpeed, sizeof(pz_GetAvailPosIndRptMsg->vertSpeed));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->vertSpeedUnc_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_SPEED_VERTICAL_UNC_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->vertSpeedUnc, sizeof(pz_GetAvailPosIndRptMsg->vertSpeedUnc));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->heading_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HEADING_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->heading, sizeof(pz_GetAvailPosIndRptMsg->heading));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->headingUnc_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HEADING_UNC_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1, 
          (void *)&pz_GetAvailPosIndRptMsg->headingUnc, sizeof(pz_GetAvailPosIndRptMsg->headingUnc));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->magneticDeviation_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_MAGNETIC_DEVIATION_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1, 
          (void *)&pz_GetAvailPosIndRptMsg->magneticDeviation, sizeof(pz_GetAvailPosIndRptMsg->magneticDeviation));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->technologyMask_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_TECH_MASK_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02, 1, 
          (void *)&pz_GetAvailPosIndRptMsg->technologyMask, sizeof(pz_GetAvailPosIndRptMsg->technologyMask));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->DOP_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_PDOP_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1, 
          (void *)&pz_GetAvailPosIndRptMsg->DOP.PDOP, sizeof(pz_GetAvailPosIndRptMsg->DOP.PDOP));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_HDOP_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1, 
          (void *)&pz_GetAvailPosIndRptMsg->DOP.HDOP, sizeof(pz_GetAvailPosIndRptMsg->DOP.HDOP));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_VDOP_V02, eQMI_LOC_SECURE_DATA_TYPE_FLOAT_32_V02, 1, 
          (void *)&pz_GetAvailPosIndRptMsg->DOP.VDOP, sizeof(pz_GetAvailPosIndRptMsg->DOP.VDOP));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->gpsTime_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_GPS_WEEK_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_16_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->gpsTime.gpsWeek, sizeof(pz_GetAvailPosIndRptMsg->gpsTime.gpsWeek));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp, 
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_GPS_TIMEOFWEEKMS_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->gpsTime.gpsTimeOfWeekMs, sizeof(pz_GetAvailPosIndRptMsg->gpsTime.gpsTimeOfWeekMs));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->timeSrc_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_TIME_SRC_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->timeSrc, sizeof(pz_GetAvailPosIndRptMsg->timeSrc));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->sensorDataUsage_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_SENSOR_USAGE_MASK_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->sensorDataUsage.usageMask, sizeof(pz_GetAvailPosIndRptMsg->sensorDataUsage.usageMask));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_AIDING_IND_MASK_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_32_V02, 1,
          (void *)&pz_GetAvailPosIndRptMsg->sensorDataUsage.aidingIndicatorMask, sizeof(pz_GetAvailPosIndRptMsg->sensorDataUsage.aidingIndicatorMask));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      if(pz_GetAvailPosIndRptMsg->gnssSvUsedList_valid){
        size_perMsg =populateGetAvailableLocationPosRpt(pz_dstEncodedgetAvailablePosRsp,
          eQMI_LOC_SECURE_GET_AVAILABLE_POS_REP_PARAM_SV_USED_V02, eQMI_LOC_SECURE_DATA_TYPE_UNSIGNED_INT_16_V02,
          pz_GetAvailPosIndRptMsg->gnssSvUsedList_len, (void *)&pz_GetAvailPosIndRptMsg->gnssSvUsedList[0], 
          (sizeof(pz_GetAvailPosIndRptMsg->gnssSvUsedList[0])*pz_GetAvailPosIndRptMsg->gnssSvUsedList_len));
        qmilocSecureGetAvailablePositionInd_len += size_perMsg;
        pz_dstEncodedgetAvailablePosRsp += size_perMsg;
      }
      // Check if data Blob needs to be Encrypted
      if(z_GetAvailablePosInd.get_secure_available_pos_data_mode == eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED_V02)
      {
        SHIM_MSG_MED( "locQmiShimEncodeIoctlReportToGetAvailableGnssPos Calling Secure API for Encryption",0, 0, 0 );
        output_encoded_msg_ptr = (unsigned char *)os_MemAlloc((qmilocSecureGetAvailablePositionInd_len + 100),OS_MEM_SCOPE_TASK);
        if(output_encoded_msg_ptr == NULL)
        {
          SHIM_MSG_ERROR( "qmilocSecureGetAvailablePositionInd_len Memory Allocation failed",0,0, 0 );
          e_IndStatusAvailPos = FALSE;
          if (NULL != pz_dstEncodedgetAvailablePosRsp)
          {
            os_MemFree((void **)&pz_dstEncodedgetAvailablePosRsp);
          }
          break;
        }
        /* Set outPut Message length to be 100 Bytes greater. Set value will have the proper Length */
        output_encoded_msg_len = qmilocSecureGetAvailablePositionInd_len + SECAPI_ENCRYPTION_MSG_HEADER;
        /* call Encryption Functions */
        status = secapi_secure_message( SC_SSID_TZ,
          SC_CID_TLOC, &pz_dstEncodedgetAvailablePosRsp_Cpy[0],
          qmilocSecureGetAvailablePositionInd_len,
          output_encoded_msg_ptr,
          &output_encoded_msg_len );

        if(status == E_SUCCESS)
        {
          pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_valid = TRUE;
          pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_len = output_encoded_msg_len;
          SHIM_MSG_MED( "qmilocSecureGetAvailablePositionInd_len before Encryption %d  After %d",qmilocSecureGetAvailablePositionInd_len, output_encoded_msg_len, 0 );
          locQmiShimMemcpy(pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_len,(void *)output_encoded_msg_ptr,
          pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_len,(void *)&pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd[0]);
        }
        else
        {
          pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_valid = FALSE;
          SHIM_MSG_MED( "secapi_secure_message Encryption failed status %d", status, 0, 0 );
          e_IndStatusAvailPos = FALSE;
        }
      }
      else
      {
        pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_valid = TRUE;
        pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_len = qmilocSecureGetAvailablePositionInd_len;
        SHIM_MSG_MED( "No Encryption qmilocSecureGetAvailablePositionInd_len %d", qmilocSecureGetAvailablePositionInd_len, 0, 0 );
        locQmiShimMemcpy(pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_len,(void *)pz_dstEncodedgetAvailablePosRsp_Cpy,
        pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd_len,(void *)&pz_GetAvailablePosition->qmilocSecureGetAvailablePositionInd[0]);
      }
      /* Reset the pz_dstEncodedgetAvailablePosRsp to start of the buffer */
      pz_dstEncodedgetAvailablePosRsp = pz_dstEncodedgetAvailablePosRsp_Cpy;
      if (NULL != pz_dstEncodedgetAvailablePosRsp)
      {
        os_MemFree((void **)&pz_dstEncodedgetAvailablePosRsp);
      }
    }
  }while(0);
  if (NULL != output_encoded_msg_ptr)
  {
    os_MemFree((void **)&output_encoded_msg_ptr);
  }
  if (NULL != pz_GetAvailPosIndRptMsg)
  {
    os_MemFree((void **)&pz_GetAvailPosIndRptMsg);
  }
  return e_IndStatusAvailPos;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to convert IOCTL reports from Loc-Api to 
  Get Protocol Config indication 
  
@param [in]   e_IoctlEvent          Event identifier
@param [in]   pz_IoctlEventPayload  Payload of the event
@param [out]  pz_IndInfo            Structure to be filled in with indication 
@param [in]  e_IndStatus           indication status                        

@retval  TRUE: if conversion succeeded
         FALSE: if ioctl wasn't found 

*//* ------------------------------------------------------------------------*/

static boolean  locQmiShimConvIoctlReportToGetProtocolConfig(
       loc_ioctl_e_type                       e_IoctlEvent,
       const loc_ioctl_callback_data_u_type  *pz_IoctlEventPayload,
       locQmiShimMsgInfoStructT              *pz_IndInfo,
       qmiLocStatusEnumT_v02                  e_IndStatus)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  qmiLocGetProtocolConfigParametersIndMsgT_v02 *pz_QmiLocGetProtInd =
    (qmiLocGetProtocolConfigParametersIndMsgT_v02*)pz_IndInfo->p_Msg;

  pz_QmiLocGetProtInd->status = e_IndStatus;

  pz_QmiLocGetProtInd->suplSecurity_valid = FALSE;
  pz_QmiLocGetProtInd->vxVersion_valid = FALSE;
  pz_QmiLocGetProtInd->suplVersion_valid = FALSE;
  pz_QmiLocGetProtInd->lppConfig_valid = FALSE;
  pz_QmiLocGetProtInd->assistedGlonassProtocolMask_valid = FALSE;

  if( eQMI_LOC_SUCCESS_V02 == e_IndStatus )
  {
    switch(  e_IoctlEvent  )
    {
      case LOC_IOCTL_GET_SUPL_SECURITY:
      {
        LOC_QMI_SHIM_SET_VALUE(pz_QmiLocGetProtInd->suplSecurity,
                               pz_IoctlEventPayload->supl_security );
        break;
      }

      case LOC_IOCTL_GET_VX_CONFIG :
      {
        LOC_QMI_SHIM_SET_VALUE( pz_QmiLocGetProtInd->vxVersion,
                                pz_IoctlEventPayload->vx_config );
        break;
      }

      case LOC_IOCTL_GET_SUPL_VERSION :
      {
        qmiLocSuplVersionEnumT_v02 e_Version;
        uint32  q_LocVersion = pz_IoctlEventPayload->supl_version;

        /* Interested in major version only */
        q_LocVersion &= (0x00FF0000); //- Mask out everything except for major rev.
        q_LocVersion >>= 16; //-- Right shift the interesting bits to LSB
        e_Version = ( q_LocVersion == (uint32)eQMI_LOC_SUPL_VERSION_2_0_V02)?
          eQMI_LOC_SUPL_VERSION_2_0_V02:eQMI_LOC_SUPL_VERSION_1_0_V02;
        LOC_QMI_SHIM_SET_VALUE( pz_QmiLocGetProtInd->suplVersion,
                                e_Version );
        break;
      }

      case LOC_IOCTL_GET_LPP_CONFIG :
      {
        LOC_QMI_SHIM_SET_VALUE( pz_QmiLocGetProtInd->lppConfig,
                                pz_IoctlEventPayload->lpp_config );
        break;
      }

      case LOC_IOCTL_GET_ASSISTED_GLO_CONFIG:
      {
        LOC_QMI_SHIM_SET_VALUE(
          pz_QmiLocGetProtInd->assistedGlonassProtocolMask,
          pz_IoctlEventPayload->assisted_glo_config );
        break;
      }

      case LOC_IOCTL_GET_EMERGENCY_PROTOCOL:
      {
        LOC_QMI_SHIM_SET_VALUE(
          pz_QmiLocGetProtInd->emergencyProtocol,
          pz_IoctlEventPayload->emergency_protocol );
        break;
      }
      case LOC_IOCTL_GET_SUPL_HASH_ALGO:
      {
        LOC_QMI_SHIM_SET_VALUE(
          pz_QmiLocGetProtInd->suplHashAlgo,
          pz_IoctlEventPayload->supl_hash_algo );
        break;
      }

      case LOC_IOCTL_GET_SUPL_TLS_VERSION:
      {
        LOC_QMI_SHIM_SET_VALUE(
          pz_QmiLocGetProtInd->suplTlsVersion,
          pz_IoctlEventPayload->supl_tls_version );
        break;
      }

      case LOC_IOCTL_GET_WIFI_SCAN_INJECT_TIMEOUT:
      {
        LOC_QMI_SHIM_SET_VALUE(
          pz_QmiLocGetProtInd->wifiScanInjectTimeout,
          pz_IoctlEventPayload->wifi_scan_inject_timeout );
        break;
      }

      default :
      {
        /* Should never be hit*/
        SHIM_MSG_ERROR("locQmiShimConvGetProtocolConfigIoctlReport :"
                       "Error converting Get Protocol config indication",
                       0,0,0);
        return FALSE;
      }
    }
  }

  pz_IndInfo->q_Id = QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_IND_V02;
  pz_IndInfo->q_MsgLen = sizeof(*pz_QmiLocGetProtInd);
  return TRUE;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle IOCTL Event reports from Loc-Api 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   e_IoctlEvent          Event identifier
@param[in]   i_IoctlStatus         Status of the IOCTL 
@param[in]   pz_IoctlEventPayload  Payload of the event
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS        Indication handled
@retval    <Anything Else>        Indication could not be handled. Defined in 
                                  loc_api.h
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleIoctlReport
(
  locQmiShimCltInfoStructT              *pz_CltInfo,
  loc_ioctl_e_type                       e_IoctlEvent,
  int32                                  i_IoctlStatus,
  const loc_ioctl_callback_data_u_type  *pz_IoctlEventPayload,
  locQmiShimMsgInfoStructT              *pz_IndInfo,
  boolean                               *pb_SendInd
)
{
  int32 i_RetVal = LOC_API_SUCCESS; /*< This return value means that the ioctl 
                      callback was handled successfully */
  qmiLocStatusEnumT_v02 e_IndStatus = eQMI_LOC_SUCCESS_V02;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */



  i_RetVal  = locQmiShimConvToQmiEventStatus ( i_IoctlStatus,
                                               &e_IndStatus );
  SHIM_MSG_LOW( "Rcvd IOCTL Report for Loc-IOCTL:%d Loc-Status:%d QMI-Status:%d",
                 e_IoctlEvent, i_IoctlStatus, e_IndStatus );
  
  if ( LOC_API_SUCCESS == i_RetVal )
  {
    switch ( e_IoctlEvent ) 
    {
      case LOC_IOCTL_SET_FIX_CRITERIA:
      {
        boolean b_IndFailure = FALSE;
        
        /* This ioctl is triggered as a result of QMI-Loc Start Request 
           message. And when this succeeds loc_start_fix, needs to be 
           triggered */
        if ( LOC_API_SUCCESS == i_IoctlStatus )
        {
          if ( LOC_API_SUCCESS != 
                       loc_start_fix(pz_CltInfo->z_LocClientHandle) )
          {
            b_IndFailure = TRUE;
          }
          else
          {
            SHIM_MSG_ERROR( "Set Fix Criteria succeeds",
                            0, 0, 0);
          }
        }
        else
        {
          
          SHIM_MSG_ERROR( "Set Fix Criteria failed, could not start fix",
                          0, 0, 0);
          b_IndFailure = TRUE;
        }

       if ( b_IndFailure )
       {
         qmiLocEventPositionReportIndMsgT_v02 *pz_PosInd =
           (qmiLocEventPositionReportIndMsgT_v02* )pz_IndInfo->p_Msg;
            
         /* Manufacture a Position Report with Session Failure */

         pz_PosInd->sessionStatus = 
           eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02;

         pz_IndInfo->q_Id = QMI_LOC_EVENT_POSITION_REPORT_IND_V02;
         pz_IndInfo->q_MsgLen = sizeof(*pz_PosInd);
         (*pb_SendInd) = TRUE;
         /* Ioctl Callback is handled succesfully no need to change 
            the retVal */
       }
      }
      break;

      case LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA:
      {
        qmiLocInjectPredictedOrbitsDataIndMsgT_v02 *pz_InjPredOrbits  =
              (qmiLocInjectPredictedOrbitsDataIndMsgT_v02* )pz_IndInfo->p_Msg;
        const uint16 *pz_Data = 
                            &pz_IoctlEventPayload->w_PredOrbitsPartNum;
        pz_InjPredOrbits->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_InjPredOrbits);        

        LOC_QMI_SHIM_SET_VALUE ( pz_InjPredOrbits->partNum,
                                 (*pz_Data) );
        
        i_RetVal = LOC_API_SUCCESS;
        (*pb_SendInd) = TRUE;
      }
      break;
      

      case LOC_IOCTL_INJECT_UTC_TIME:
      {
        qmiLocInjectUtcTimeIndMsgT_v02 *pz_InjUtcInd =
                (qmiLocInjectUtcTimeIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_InjUtcInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_INJECT_UTC_TIME_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_InjUtcInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_INJECT_POSITION:
      {
        qmiLocInjectPositionIndMsgT_v02 *pz_InjPosInd =
                (qmiLocInjectPositionIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_InjPosInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_INJECT_POSITION_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_InjPosInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_SET_ENGINE_LOCK:
      {
        qmiLocSetEngineLockIndMsgT_v02 *pz_SetEngLockInd =
                (qmiLocSetEngineLockIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_SetEngLockInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_ENGINE_LOCK_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetEngLockInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_ENGINE_LOCK:
      {
        qmiLocGetEngineLockIndMsgT_v02 *pz_GetEngLockInd =
                (qmiLocGetEngineLockIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_GetEngLockInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_GET_ENGINE_LOCK_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetEngLockInd);
        (*pb_SendInd) = TRUE;

        switch ( pz_IoctlEventPayload->engine_lock )
        {
          case LOC_LOCK_NONE:
            LOC_QMI_SHIM_SET_VALUE( pz_GetEngLockInd->lockType,
                                    eQMI_LOC_LOCK_NONE_V02 );
            break;
          case LOC_LOCK_MI:
            LOC_QMI_SHIM_SET_VALUE( pz_GetEngLockInd->lockType,
                                    eQMI_LOC_LOCK_MI_V02 );
            break;
          case LOC_LOCK_MT:
            LOC_QMI_SHIM_SET_VALUE( pz_GetEngLockInd->lockType,
                                    eQMI_LOC_LOCK_MT_V02 );
            break;
          case LOC_LOCK_ALL:
            LOC_QMI_SHIM_SET_VALUE( pz_GetEngLockInd->lockType,
                                    eQMI_LOC_LOCK_ALL_V02 );
            break;
          default:
            SHIM_MSG_ERROR( "Invalid Lock Type: %d", 
                            pz_IoctlEventPayload->engine_lock, 0, 0 );
            break;
        }
      }
      break;

      case LOC_IOCTL_SET_NMEA_TYPES:
      {
        qmiLocSetNmeaTypesIndMsgT_v02 *pz_SetNmeaTypesInd =
                (qmiLocSetNmeaTypesIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_SetNmeaTypesInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_NMEA_TYPES_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetNmeaTypesInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_DELETE_ASSIST_DATA:
      {
        qmiLocDeleteAssistDataIndMsgT_v02 *pz_DelAssistInd =
                (qmiLocDeleteAssistDataIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_DelAssistInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_DELETE_ASSIST_DATA_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_DelAssistInd);
        (*pb_SendInd) = TRUE;
      }
      break;
      
      case LOC_IOCTL_SET_XTRA_T_SESSION_CONTROL:
      {
        qmiLocSetXtraTSessionControlIndMsgT_v02 *pz_XtraTInd =
                (qmiLocSetXtraTSessionControlIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_XtraTInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_XtraTInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      /* Follow Through */
      case  LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR:
      case  LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR:
      case  LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR:
      case  LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR:
      {
        qmiLocSetServerIndMsgT_v02   *pz_SetServerInd =
                (qmiLocSetServerIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_SetServerInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_SERVER_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetServerInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      /* Follow Through */
      case  LOC_IOCTL_GET_CDMA_PDE_SERVER_ADDR:
      case  LOC_IOCTL_GET_CDMA_MPC_SERVER_ADDR:
      case  LOC_IOCTL_GET_UMTS_SLP_SERVER_ADDR:
      case  LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR:
      {
        qmiLocGetServerIndMsgT_v02   *pz_GetServerInd =
                (qmiLocGetServerIndMsgT_v02* )pz_IndInfo->p_Msg;
        const loc_server_info_s_type  *pz_SrvrInfo = 
                                  &(pz_IoctlEventPayload->server_addr);
        
        pz_GetServerInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_GET_SERVER_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetServerInd);
        (*pb_SendInd) = TRUE;

        if ( LOC_IOCTL_GET_CDMA_PDE_SERVER_ADDR == e_IoctlEvent ) 
        {
          pz_GetServerInd->serverType = eQMI_LOC_SERVER_TYPE_CDMA_PDE_V02;
        }
        else if ( LOC_IOCTL_GET_CDMA_MPC_SERVER_ADDR == e_IoctlEvent )
        {
          pz_GetServerInd->serverType = eQMI_LOC_SERVER_TYPE_CDMA_MPC_V02;
        }
        else if ( LOC_IOCTL_GET_UMTS_SLP_SERVER_ADDR == e_IoctlEvent )
        {
          pz_GetServerInd->serverType = eQMI_LOC_SERVER_TYPE_UMTS_SLP_V02;
        }
        else if ( LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR == e_IoctlEvent ) 
        {
          pz_GetServerInd->serverType = eQMI_LOC_SERVER_TYPE_CUSTOM_PDE_V02;
        }

        if ( LOC_SERVER_ADDR_IPV4 == pz_SrvrInfo->addr_type )
        {
          pz_GetServerInd->ipv4Addr_valid = TRUE;
          pz_GetServerInd->ipv4Addr.addr = pz_SrvrInfo->addr_info.ipv4.addr;
          pz_GetServerInd->ipv4Addr.port = pz_SrvrInfo->addr_info.ipv4.port;
        }
        else if ( LOC_SERVER_ADDR_URL == pz_SrvrInfo->addr_type )
        {
          pz_GetServerInd->urlAddr_valid = TRUE;
          /* Bounded Buffer Copy, will get truncated if buffer allocated is not 
             enough */
          (void) GNSS_STRLCPY( (char*)pz_GetServerInd->urlAddr,
                       (char*)pz_SrvrInfo->addr_info.url.addr,
                       QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1 );
        }
      }
      break;
      
      case LOC_IOCTL_INFORM_NI_USER_RESPONSE:
      {
        qmiLocNiUserRespIndMsgT_v02  *pz_UserRespInd =
                (qmiLocNiUserRespIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_UserRespInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_NI_USER_RESPONSE_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_UserRespInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_NMEA_TYPES:
      {
        qmiLocGetNmeaTypesIndMsgT_v02    *pz_Ind =
                (qmiLocGetNmeaTypesIndMsgT_v02* )pz_IndInfo->p_Msg;
        const loc_nmea_sentence_type  *pz_NmeaType = 
                                  &(pz_IoctlEventPayload->nmea_types);

        pz_Ind->status = e_IndStatus;
        pz_Ind->nmeaSentenceType_valid = TRUE;
        pz_Ind->nmeaSentenceType = 0;
        
        if ( LOC_NMEA_MASK_GGA & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GGA_V02 ;
        }
        if ( LOC_NMEA_MASK_RMC & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_RMC_V02 ;
        }
        if ( LOC_NMEA_MASK_GSV & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GSV_V02 ;
        }
        if ( LOC_NMEA_MASK_GSA & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GSA_V02 ;
        }
        if ( LOC_NMEA_MASK_VTG & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_VTG_V02 ;
        }
        if ( LOC_NMEA_MASK_PQXFI & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_PQXFI_V02;
        }
        if ( LOC_NMEA_MASK_GLGSV & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GLGSV_V02;
        }
        if ( LOC_NMEA_MASK_GNGSA & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GNGSA_V02;
        }
        if ( LOC_NMEA_MASK_GNGNS & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GNGNS_V02;
        }
        if ( LOC_NMEA_MASK_GARMC  & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GARMC_V02;
        }
        if ( LOC_NMEA_MASK_GAGSV & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GAGSV_V02;
        }
        if ( LOC_NMEA_MASK_GAGSA & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GAGSA_V02;
        }
        if ( LOC_NMEA_MASK_GAVTG & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GAVTG_V02;
        }
        if ( LOC_NMEA_MASK_PSTIS & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_PSTIS_V02;
        }
        if ( LOC_NMEA_MASK_GAGGA & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_GAGGA_V02;
        }
        if ( LOC_NMEA_MASK_PQGSA & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_PQGSA_V02;
        }
        if ( LOC_NMEA_MASK_PQGSV & (*pz_NmeaType) )
        {
          (pz_Ind->nmeaSentenceType) |= QMI_LOC_NMEA_MASK_PQGSV_V02;
        }

        pz_IndInfo->q_Id = QMI_LOC_GET_NMEA_TYPES_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_Ind);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_XTRA_T_SESSION_CONTROL:
      {
        qmiLocGetXtraTSessionControlIndMsgT_v02  *pz_Xtrat =
                (qmiLocGetXtraTSessionControlIndMsgT_v02* )pz_IndInfo->p_Msg;
      
        LOC_QMI_SHIM_SET_VALUE ( pz_Xtrat->xtraTSessionControl,
                                 pz_IoctlEventPayload->xtra_t_session_control );
        
        pz_Xtrat->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_GET_XTRA_T_SESSION_CONTROL_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_Xtrat);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_FIX_CRITERIA:
      {  
         qmiLocGetFixCriteriaIndMsgT_v02   *pz_QmiFixCrit = 
                    ( qmiLocGetFixCriteriaIndMsgT_v02* )pz_IndInfo->p_Msg;
         const loc_fix_criteria_s_type  *pz_LocFixCrit = 
                ( loc_fix_criteria_s_type*) &pz_IoctlEventPayload->fix_criteria;

         pz_QmiFixCrit->status = e_IndStatus;
         pz_IndInfo->q_Id = QMI_LOC_GET_FIX_CRITERIA_IND_V02;
         pz_IndInfo->q_MsgLen = sizeof(*pz_QmiFixCrit);       
         (*pb_SendInd) = locQmiShimConvToQmiFixCriteria ( pz_LocFixCrit,
                                                          pz_QmiFixCrit ) ;
      }
      break;

      case LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE:
      {
        qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02  *pz_XtraSource =
             (qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02* )pz_IndInfo->p_Msg;
        const loc_predicted_orbits_data_source_s_type *pz_LocXtra = 
                             &pz_IoctlEventPayload->predicted_orbits_data_source;

        pz_XtraSource->status = e_IndStatus;

        //copy the payload only if the query call was successful
        if (eQMI_LOC_SUCCESS_V02 == e_IndStatus )
        {
          uint8  u_SrvCntr = 0;
          pz_XtraSource->allowedSizes.maxFileSizeInBytes = 
            pz_LocXtra->max_file_size;
          pz_XtraSource->allowedSizes.maxPartSize =
            pz_LocXtra->max_part_size;

          pz_XtraSource->serverList_valid = TRUE;
          pz_XtraSource->serverList.serverList_len = 
            MIN( LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS, 
                 QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02 );
          for ( u_SrvCntr = 0;
                u_SrvCntr < pz_XtraSource->serverList.serverList_len;
                u_SrvCntr ++ )
          {
            (void)GNSS_STRLCPY ( (char*)pz_XtraSource->serverList.serverList[u_SrvCntr].serverUrl,
                                 (char*)pz_LocXtra->servers[u_SrvCntr],
                                 QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1 );
          }

        }
        
        pz_IndInfo->q_Id = QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_XtraSource);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY:
      {
        qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02 *pz_XtraValidity =
          (qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02* )pz_IndInfo->p_Msg;
        const loc_predicted_orbits_data_validity_report_s_type *pz_LocXtra = 
                           &pz_IoctlEventPayload->predicted_orbits_data_validity;

        pz_XtraValidity->validityInfo_valid = TRUE;
        pz_XtraValidity->validityInfo.durationHours = 
                                        pz_LocXtra->valid_duration_hrs;
        pz_XtraValidity->validityInfo.startTimeInUTC =
                                        pz_LocXtra->start_time_utc;
        
        pz_XtraValidity->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_XtraValidity);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_SBAS_CONFIG:
      {
        qmiLocGetSbasConfigIndMsgT_v02  *pz_GetSbas =
                (qmiLocGetSbasConfigIndMsgT_v02* )pz_IndInfo->p_Msg;

        if ( eQMI_LOC_SUCCESS_V02 == e_IndStatus )
        {
          if ( TRUE == pz_IoctlEventPayload->sbas_mode )
          {
            LOC_QMI_SHIM_SET_VALUE(pz_GetSbas->sbasConfig,TRUE);
          }
          else
          {
            LOC_QMI_SHIM_SET_VALUE(pz_GetSbas->sbasConfig,FALSE);
          }
        }  
        pz_GetSbas->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_GET_SBAS_CONFIG_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetSbas);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_SET_SBAS_CONFIG:
      {
        qmiLocSetSbasConfigIndMsgT_v02  *pz_SetSbas =
                (qmiLocSetSbasConfigIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_SetSbas->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_SBAS_CONFIG_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetSbas);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS:
      case LOC_IOCTL_INFORM_SERVER_MULTI_OPEN_STATUS:
      {
        qmiLocInformLocationServerConnStatusIndMsgT_v02  *pz_ConnStat =
          (qmiLocInformLocationServerConnStatusIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_ConnStat->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_ConnStat);
        (*pb_SendInd) = TRUE;
      }
      break;

      
      case LOC_IOCTL_SET_ON_DEMAND_LPM:
      {
        qmiLocSetLowPowerModeIndMsgT_v02 *pz_SetLpmInd =
                (qmiLocSetLowPowerModeIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_SetLpmInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_LOW_POWER_MODE_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetLpmInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_ON_DEMAND_LPM:
      {
        qmiLocGetLowPowerModeIndMsgT_v02 *pz_GetLpmInd =
                (qmiLocGetLowPowerModeIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_GetLpmInd->status = e_IndStatus;
        
        if ( eQMI_LOC_SUCCESS_V02 == e_IndStatus )
        {
          if (pz_IoctlEventPayload->on_demand_lpm)
          {
            LOC_QMI_SHIM_SET_VALUE(pz_GetLpmInd->lowPowerMode,TRUE);
          }
          else
          {
            LOC_QMI_SHIM_SET_VALUE(pz_GetLpmInd->lowPowerMode,FALSE);
          }
        }

        pz_IndInfo->q_Id = QMI_LOC_GET_LOW_POWER_MODE_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetLpmInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_SET_EVENT_REG_MASK:
        /** Nothing to do, there is no indication that needs to be given.
            Added this case statement to supress it from going to default case
            and print a wrong message */
        break;

      case LOC_IOCTL_GET_EVENT_REG_MASK:
      {
        qmiLocGetRegisteredEventsIndMsgT_v02 *pz_GetRegMask =
                (qmiLocGetRegisteredEventsIndMsgT_v02* )pz_IndInfo->p_Msg;
        const loc_event_mask_type* pz_Data = 
                            &pz_IoctlEventPayload->event_reg_mask;
        pz_GetRegMask->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_GET_REGISTERED_EVENTS_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetRegMask);        

        i_RetVal = locQmiShimConvToQmiEventRegMask(
                       pz_Data, &pz_GetRegMask->eventRegMask);
        if ( LOC_API_SUCCESS == i_RetVal )
        {
          pz_GetRegMask->eventRegMask_valid = TRUE;
          (*pb_SendInd) = TRUE;
        }
      }
      break;

     case LOC_IOCTL_SET_OPERATION_MODE:                
      {
        qmiLocSetOperationModeIndMsgT_v02  *pz_SetOpMode =
                (qmiLocSetOperationModeIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_SetOpMode->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_OPERATION_MODE_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetOpMode);
        (*pb_SendInd) = TRUE;
      }
      break;
      
      case LOC_IOCTL_GET_OPERATION_MODE:
      {
        qmiLocGetOperationModeIndMsgT_v02  *pz_GetOpMode =
                (qmiLocGetOperationModeIndMsgT_v02* )pz_IndInfo->p_Msg;
        const qmiLocOperationModeEnumT_v02* pz_Data = 
                            &pz_IoctlEventPayload->operation_mode;

        switch ( *pz_Data )
        {
          /* range check the operation mode */
          case eQMI_LOC_OPER_MODE_DEFAULT_V02:
          case eQMI_LOC_OPER_MODE_MSB_V02:
          case eQMI_LOC_OPER_MODE_MSA_V02:
          case eQMI_LOC_OPER_MODE_STANDALONE_V02:
          case eQMI_LOC_OPER_MODE_CELL_ID_V02:
          case eQMI_LOC_OPER_MODE_WWAN_V02:
            LOC_QMI_SHIM_SET_VALUE( pz_GetOpMode->operationMode,
                                    *pz_Data );
            break;
                  
          default:
            i_RetVal = LOC_API_INVALID_PARAMETER;
            SHIM_MSG_ERROR( "Invalid Option:%d",
                            (*pz_Data), 0, 0 );
            break;
        }

        pz_GetOpMode->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_GET_OPERATION_MODE_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetOpMode);

        /* Note: Send only if i_RetVal == SUCCESS */
        (*pb_SendInd) = (LOC_API_SUCCESS == i_RetVal );
      }
      break;

      case LOC_IOCTL_ACCESS_EFS_DATA:
      {
          loc_fs_operation_e_type e_FileOperation = 
                 pz_IoctlEventPayload->z_AccessEfsData.efs_operation;
          locQmiShimIndUnionT  *pz_Resp =
                  (locQmiShimIndUnionT* )pz_IndInfo->p_Msg;
          switch ( e_FileOperation )
          {
            case LOC_FS_CREATE_WRITE_FILE:
                pz_Resp->z_InjectSuplCertInd.status = e_IndStatus;
                pz_IndInfo->q_Id = QMI_LOC_INJECT_SUPL_CERTIFICATE_IND_V02;
                pz_IndInfo->q_MsgLen = sizeof(pz_Resp->z_InjectSuplCertInd);
              break;
            case LOC_FS_DELETE_FILE:
            case LOC_FS_PURGE_DIR:
              pz_Resp->z_DeleteSuplCertInd.status = e_IndStatus;
              pz_IndInfo->q_Id = QMI_LOC_DELETE_SUPL_CERTIFICATE_IND_V02;
              pz_IndInfo->q_MsgLen = sizeof(pz_Resp->z_DeleteSuplCertInd);
              break;
            default:
              i_RetVal = LOC_API_INVALID_PARAMETER;
              SHIM_MSG_ERROR( "Invalid File Operation:%d",
                              (e_FileOperation), 0, 0 );
              break;
          }          
          (*pb_SendInd) = (LOC_API_SUCCESS == i_RetVal );
      }
      break;


      /* Set Protocol config messages */
      case LOC_IOCTL_SET_SUPL_SECURITY:
      case LOC_IOCTL_SET_VX_CONFIG:
      case LOC_IOCTL_SET_SUPL_VERSION:  
      case LOC_IOCTL_SET_LPP_CONFIG:
      case LOC_IOCTL_SET_ASSISTED_GLO_CONFIG:
      case LOC_IOCTL_SET_EMERGENCY_PROTOCOL:
      case LOC_IOCTL_SET_SUPL_HASH_ALGO:
      case LOC_IOCTL_SET_SUPL_TLS_VERSION:
      case LOC_IOCTL_SET_WIFI_SCAN_INJECT_TIMEOUT:
      {
        
       (*pb_SendInd) =
          locQmiShimConvIoctlReportToSetProtocolConfig(e_IoctlEvent,
                                                       pz_IndInfo,
                                                       e_IndStatus);
       break;
      }

      /* Get Protocol config messages*/
      case LOC_IOCTL_GET_SUPL_VERSION:
      case LOC_IOCTL_GET_LPP_CONFIG:
      case LOC_IOCTL_GET_VX_CONFIG:
      case LOC_IOCTL_GET_SUPL_SECURITY:
      case LOC_IOCTL_GET_ASSISTED_GLO_CONFIG:
      case LOC_IOCTL_GET_EMERGENCY_PROTOCOL:
      case LOC_IOCTL_GET_SUPL_HASH_ALGO:
      case LOC_IOCTL_GET_SUPL_TLS_VERSION:
      case LOC_IOCTL_GET_WIFI_SCAN_INJECT_TIMEOUT:
      {
   
        (*pb_SendInd) = 
          locQmiShimConvIoctlReportToGetProtocolConfig(e_IoctlEvent,
                                                       pz_IoctlEventPayload,
                                                       pz_IndInfo,
                                                       e_IndStatus);
          break;
      }

      case LOC_IOCTL_SET_PE_CONFIG:
      {
        qmiLocSetPositionEngineConfigParametersIndMsgT_v02 *pz_SetPeCfg =
           (qmiLocSetPositionEngineConfigParametersIndMsgT_v02* )pz_IndInfo->p_Msg;
        
        const qmiLocSetPositionEngineConfigParametersIndMsgT_v02* pz_Data = 
                            &pz_IoctlEventPayload->z_SetPeConfigInd;
        /* Copy over the Payload */
        (*pz_SetPeCfg ) = (*pz_Data);

        /* Now lets set the status overwriting what was copied before,
           Since the LocMw does not set the status */
        pz_SetPeCfg->status = e_IndStatus;
        pz_IndInfo->q_Id = 
                QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetPeCfg);
        (*pb_SendInd) = TRUE;
      }
      break;        

      case LOC_IOCTL_GET_PE_CONFIG:
      {
        qmiLocGetPositionEngineConfigParametersIndMsgT_v02 *pz_GetPeCfg =
           (qmiLocGetPositionEngineConfigParametersIndMsgT_v02* )pz_IndInfo->p_Msg;
        
        const qmiLocGetPositionEngineConfigParametersIndMsgT_v02* pz_Data = 
                            &pz_IoctlEventPayload->z_GetPeConfigInd;
        /* Copy over the Payload */
        (*pz_GetPeCfg ) = (*pz_Data);
      
        /* Now lets set the status overwriting what was copied before,
           Since the LocMw does not set the status */
        pz_GetPeCfg->status = e_IndStatus;
        pz_IndInfo->q_Id = 
                QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetPeCfg);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_BEST_AVAILABLE_POSITION:
      {
        qmiLocGetBestAvailablePositionIndMsgT_v02 *pz_GetBestAvailablePosition =
          (qmiLocGetBestAvailablePositionIndMsgT_v02 *)pz_IndInfo->p_Msg;

        /* copy the payload from ioctl report */
        *pz_GetBestAvailablePosition = pz_IoctlEventPayload->bap_ind;
        /* set the status now */
        pz_GetBestAvailablePosition->status = e_IndStatus;

        pz_IndInfo->q_Id = QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetBestAvailablePosition);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION:
      {
        qmiLocGetAvailWwanPositionIndMsgT_v02 *pz_GetAvailableWwanPosition =
          (qmiLocGetAvailWwanPositionIndMsgT_v02 *)pz_IndInfo->p_Msg;

        /* copy the payload from ioctl report */
        *pz_GetAvailableWwanPosition = pz_IoctlEventPayload->z_GetAvailWwanPosInd;
        /* set the status now */
        pz_GetAvailableWwanPosition->status = e_IndStatus;

        pz_IndInfo->q_Id = QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetAvailableWwanPosition);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC:
      {
        int32 i_RetVal =  TRUE;
        qmiLocSecureGetAvailablePositionIndMsgT_v02 *pz_GetAvailablePosition =
          (qmiLocSecureGetAvailablePositionIndMsgT_v02 *)pz_IndInfo->p_Msg;

        // Encode the value from pz_IoctlEventPayload->z_GetAvailGnssPosInd
        // Encrypt if required
        // Then Copy the data to pz_GetAvailablePosition
        // encode and Encrypt  the data
        i_RetVal = locQmiShimEncodeIoctlReportToGetAvailableGnssPos(pz_IoctlEventPayload->z_GetAvailablePosInd,
           pz_GetAvailablePosition,e_IndStatus);

        /* copy the payload from ioctl report */
       //*pz_GetAvailablePosition = pz_IoctlEventPayload->z_GetAvailGnssPosInd;

        /* set the status now */
        if(i_RetVal && (eQMI_LOC_SUCCESS_V02 == e_IndStatus))
        {
          pz_GetAvailablePosition->sessionStatus= (qmiLocSessionStatusEnumT_v02)e_IndStatus;
        }
        else
        {
          pz_GetAvailablePosition->sessionStatus= i_RetVal;
        }
        pz_GetAvailablePosition->secureLocDataMode= 
          pz_IoctlEventPayload->z_GetAvailablePosInd.get_secure_available_pos_data_mode;
        pz_IndInfo->q_Id = QMI_LOC_SECURE_GET_AVAILABLE_POSITION_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_GetAvailablePosition);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_INJECT_GSM_CELL_INFO_REQ:
      {
        qmiLocInjectGSMCellInfoIndMsgT_v02 *pz_InjGSMCellInfoInd =
                (qmiLocInjectGSMCellInfoIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_InjGSMCellInfoInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_INJECT_GSM_CELL_INFO_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_InjGSMCellInfoInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_INJECT_NI_MESSAGE_REQ:
      {
        qmiLocInjectNetworkInitiatedMessageIndMsgT_v02 *pz_InjNIMsgInd =
                (qmiLocInjectNetworkInitiatedMessageIndMsgT_v02 * )pz_IndInfo->p_Msg;
        pz_InjNIMsgInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_INJECT_NETWORK_INITIATED_MESSAGE_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_InjNIMsgInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_WWAN_OOS_NOTIFICATION_REQ:
      {
        qmiLocWWANOutOfServiceNotificationIndMsgT_v02 *pz_WWANOutOfServiceInd =
                (qmiLocWWANOutOfServiceNotificationIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_WWANOutOfServiceInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_WWAN_OUT_OF_SERVICE_NOTIFICATION_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_WWANOutOfServiceInd);
        (*pb_SendInd) = TRUE;
      }
      break;
      
      case LOC_IOCTL_SET_PREMIUM_SERVICES_CONFIG:
      {
        qmiLocSetPremiumServicesCfgIndMsgT_v02 *pz_SetPremSvcsCfgInd =
                (qmiLocSetPremiumServicesCfgIndMsgT_v02* )pz_IndInfo->p_Msg;

        /* copy the payload from ioctl report */
        *pz_SetPremSvcsCfgInd = pz_IoctlEventPayload->z_SetPremSvcsCfgInd;
        /* set the status now */
        pz_SetPremSvcsCfgInd->status = e_IndStatus;

        pz_IndInfo->q_Id = QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetPremSvcsCfgInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_SET_XTRA_VERSION_CHECK:
      {
        qmiLocSetXtraVersionCheckIndMsgT_v02 *pz_SetXtraVersionCheckInd =
                (qmiLocSetXtraVersionCheckIndMsgT_v02* )pz_IndInfo->p_Msg; 

        pz_SetXtraVersionCheckInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_XTRA_VERSION_CHECK_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetXtraVersionCheckInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_SET_GNSS_CONSTELL_REPORT_CONFIG:
      {
        qmiLocSetGNSSConstRepConfigIndMsgT_v02 *pz_SetGNSSConfigInd =
                (qmiLocSetGNSSConstRepConfigIndMsgT_v02* )pz_IndInfo->p_Msg;
        SHIM_MSG_MED("Processing LOC_IOCTL_SET_GNSS_CONSTELL_REPORT_CONFIG Indicate back",0,0,0);
        pz_SetGNSSConfigInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_SetGNSSConfigInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      case LOC_IOCTL_DELETE_GNSS_SERVICE_DATA:
      {
        qmiLocDeleteGNSSServiceDataIndMsgT_v02 *pz_DelGNSSDataInd =
                (qmiLocDeleteGNSSServiceDataIndMsgT_v02* )pz_IndInfo->p_Msg;
        pz_DelGNSSDataInd->status = e_IndStatus;
        pz_IndInfo->q_Id = QMI_LOC_DELETE_GNSS_SERVICE_DATA_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(*pz_DelGNSSDataInd);
        (*pb_SendInd) = TRUE;
      }
      break;

      default: 
        SHIM_MSG_ERROR( "Cannot Handle Ioctl Status for IOCTL:0x%x",
                        e_IoctlEvent,0,0);
        break;
    }
  }

  return i_RetVal;
}/* locQmiHandleIoctlReport */

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Parsed Position Event reports from Loc-Api 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_ParsedPositionRpt  Parsed position report from Loc-Api
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS         Indication handled
@retval    LOC_API_GENERAL_FAILURE Indication could not be handled. Defined in 
                                   loc_api.h
                                  
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleParsedPositionReport
(
  locQmiShimCltInfoStructT              *pz_CltInfo,
  const loc_parsed_position_s_type      *pz_ParsedPositionRpt,
  locQmiShimMsgInfoStructT              *pz_IndInfo,
  boolean                               *pb_SendInd
)
{
  int32 i_RetVal = LOC_API_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 != 
         ( pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 ) )
  {
    SHIM_MSG_MED( "Client did not register for this report:%d, suppressing",
                  QMI_LOC_EVENT_POSITION_REPORT_IND_V02,
                  0,
                  0 );
    /* i_RetVal is not changed as we handled the callback successfully */
  }
  else
  { 
    
    qmiLocEventPositionReportIndMsgT_v02 *pz_PosInd =
            (qmiLocEventPositionReportIndMsgT_v02* )pz_IndInfo->p_Msg;
    /* Translate the Indication */

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_SESSION_STATUS )
    {
      switch(pz_ParsedPositionRpt->session_status )
      {
        case LOC_SESS_STATUS_SUCCESS:
          pz_PosInd->sessionStatus = eQMI_LOC_SESS_STATUS_SUCCESS_V02;
          break;
        case LOC_SESS_STATUS_BAD_PARAMETER:
          pz_PosInd->sessionStatus = eQMI_LOC_SESS_STATUS_BAD_PARAMETER_V02;
          break;
        case LOC_SESS_STATUS_IN_PROGESS:
          pz_PosInd->sessionStatus = eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02;
          break;
        case LOC_SESS_STATUS_PHONE_OFFLINE:
          pz_PosInd->sessionStatus = eQMI_LOC_SESS_STATUS_PHONE_OFFLINE_V02;
          break;
        case LOC_SESS_STATUS_TIMEOUT:
          pz_PosInd->sessionStatus = eQMI_LOC_SESS_STATUS_TIMEOUT_V02;
          break;
        case LOC_SESS_STATUS_ENGINE_LOCKED:
          pz_PosInd->sessionStatus = eQMI_LOC_SESS_STATUS_ENGINE_LOCKED_V02;
          break;
          
        case LOC_SESS_STATUS_GENERAL_FAILURE:
        default:
          pz_PosInd->sessionStatus = eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02;
          break;        
      }
    }
    /* Since this is a mandatory TLV, we have to set it */
    else
    {
      pz_PosInd->sessionStatus = eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02;
    }

    /* Relay the Session ID */
    pz_PosInd->sessionId = pz_CltInfo->u_SessionId;

    /* Update the fix session ID and relay it */
    pz_CltInfo->q_fixID++;
    LOC_QMI_SHIM_SET_VALUE(pz_PosInd->fixId, pz_CltInfo->q_fixID);
    
    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_TIMESTAMP_UTC )
    {
      
      uint16 w_GpsWeek;
      uint32 q_GpsToWMs;

      /* We need to fill in GPS time as well, calculate from UTC and 
         fill in, only if LEAP Seconds is also provided */
      if ( ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_LEAP_SECONDS ) &&
           ( TRUE == gnss_ConvertUtcMstoGpsTime(
                           pz_ParsedPositionRpt->timestamp_utc,
                           pz_ParsedPositionRpt->leap_seconds,
                           &w_GpsWeek,
                           &q_GpsToWMs ) ) )
      {
        /* Since it is a complex TLV assign separately */
        pz_PosInd->gpsTime_valid = TRUE;
        pz_PosInd->gpsTime.gpsWeek = w_GpsWeek;
        pz_PosInd->gpsTime.gpsTimeOfWeekMs = q_GpsToWMs;
      }

      LOC_QMI_SHIM_SET_VALUE( pz_PosInd->timestampUtc, 
                              pz_ParsedPositionRpt->timestamp_utc );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_LEAP_SECONDS )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->leapSeconds,
                               pz_ParsedPositionRpt->leap_seconds );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_TIME_UNC )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->timeUnc,
                               pz_ParsedPositionRpt->time_unc );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_LATITUDE )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->latitude,
                               pz_ParsedPositionRpt->latitude );
      
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_LONGITUDE )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->longitude,
                               pz_ParsedPositionRpt->longitude );
    }

    if ( pz_ParsedPositionRpt->valid_mask & 
                     LOC_POS_VALID_ALTITUDE_WRT_ELLIPSOID )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->altitudeWrtEllipsoid,
                               pz_ParsedPositionRpt->altitude_wrt_ellipsoid );
    }

    if ( pz_ParsedPositionRpt->valid_mask & 
                     LOC_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->altitudeWrtMeanSeaLevel,
                            pz_ParsedPositionRpt->altitude_wrt_mean_sea_level);
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_SPEED_HORIZONTAL )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->speedHorizontal,
                               pz_ParsedPositionRpt->speed_horizontal );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_SPEED_VERTICAL )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->speedVertical,
                               pz_ParsedPositionRpt->speed_vertical );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_HEADING )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->heading,
                               pz_ParsedPositionRpt->heading);
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_HOR_UNC_CIRCULAR )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->horUncCircular,
                               pz_ParsedPositionRpt->hor_unc_circular);
    }

    if ( pz_ParsedPositionRpt->valid_mask & 
                      LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MAJ )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->horUncEllipseSemiMajor,
                            pz_ParsedPositionRpt->hor_unc_ellipse_semi_major );
    }

    if ( pz_ParsedPositionRpt->valid_mask & 
                     LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MIN )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->horUncEllipseSemiMinor,
                            pz_ParsedPositionRpt->hor_unc_ellipse_semi_minor );
    }

    if ( pz_ParsedPositionRpt->valid_mask & 
                     LOC_POS_VALID_HOR_UNC_ELLI_ORIENT_AZIMUTH )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->horUncEllipseOrientAzimuth,
                        pz_ParsedPositionRpt->hor_unc_ellipse_orient_azimuth );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_VERTICAL_UNC )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->vertUnc,
                               pz_ParsedPositionRpt->vert_unc );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_SPEED_UNC )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->speedUnc,
                               pz_ParsedPositionRpt->speed_unc );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_HEADING_UNC )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->headingUnc,
                               pz_ParsedPositionRpt->heading_unc );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_CONFIDENCE_HORIZONTAL )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->horConfidence,
                               pz_ParsedPositionRpt->confidence_horizontal);
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_CONFIDENCE_VERTICAL )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->vertConfidence,
                               pz_ParsedPositionRpt->confidence_vertical );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_MAGNETIC_VARIATION )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->magneticDeviation,
                               pz_ParsedPositionRpt->magnetic_deviation );
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_TECHNOLOGY_MASK )
    {
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->technologyMask,
                               pz_ParsedPositionRpt->technology_mask );
    }

    if ( (pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_SENSOR_AIDING_IND) && 
         (pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_SENSOR_DATA_USAGE) )
    {
      boolean b_Ok = TRUE;
      qmiLocSensorUsageIndicatorStructT_v02 z_Usage;
      
      memset(&z_Usage, 0, sizeof(z_Usage));
      if(TRUE != locQmiShimConvToLocSensorAid(
                    pz_ParsedPositionRpt->sensor_aiding_indicator,
                    &(z_Usage.aidingIndicatorMask) ) )
      {
        b_Ok = FALSE;
      }
      if(TRUE != locQmiShimConvToLocSensorUsage(
                    pz_ParsedPositionRpt->sensor_data_usage_mask,                    
                    &(z_Usage.usageMask) ) )
      {
        b_Ok = FALSE;
      }
      if(TRUE == b_Ok)
      {
        LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->sensorDataUsage, z_Usage);
      }
      else
      {
        i_RetVal = LOC_API_GENERAL_FAILURE;
      }
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_EXT_TIME_SOURCE )
    {
      qmiLocTimeSourceEnumT_v02 z_Src = eQMI_LOC_TIME_SRC_INVALID_V02;
      if(TRUE != locQmiShimConvToLocTimeSource(&z_Src,
            pz_ParsedPositionRpt->extended_time_source ))
      {
        i_RetVal = LOC_API_GENERAL_FAILURE;
      }
      else
      {
        LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->timeSrc, z_Src);
      }
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_DOP )
    {
      qmiLocDOPStructT_v02 z_Dop;
      memset(&z_Dop, 0, sizeof(z_Dop));
      z_Dop.PDOP = pz_ParsedPositionRpt->position_dop;
      z_Dop.HDOP = pz_ParsedPositionRpt->hor_dop;
      z_Dop.VDOP = pz_ParsedPositionRpt->vert_dop;
      LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->DOP, z_Dop);
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_H_RELIABILITY_IND )
    {
      qmiLocReliabilityEnumT_v02 z_Reliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
      if(TRUE != locQmiShimConvToLocReliability(&z_Reliability, 
                   pz_ParsedPositionRpt->hor_reliability))
      {
        i_RetVal = LOC_API_GENERAL_FAILURE;
      }
      else
      {
        LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->horReliability, z_Reliability);
      }
    }

    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_V_RELIABILITY_IND )
    {
      qmiLocReliabilityEnumT_v02 z_Reliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
      if(TRUE != locQmiShimConvToLocReliability(&z_Reliability,
                   pz_ParsedPositionRpt->vert_reliability))
      {
        i_RetVal = LOC_API_GENERAL_FAILURE;
      }
      else
      {
        LOC_QMI_SHIM_SET_VALUE ( pz_PosInd->vertReliability, z_Reliability);
      }
    }

   if( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_SV_USED_MASK )
    {
      uint32_t i = 0, q_SvListIdx = 0;

      /** convert the sv used info to gnssSvUsedList */

      /* Populate the GPS satellite info */
      for (i = 0; (i < PDSM_N_ACTIVE_GPS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
      {
        /* Check whether GPS SVs were used in calculating position */
        if(pz_ParsedPositionRpt->z_NavSvInfo.z_GpsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
        {
          pz_PosInd->gnssSvUsedList[q_SvListIdx++] = 
            pz_ParsedPositionRpt->z_NavSvInfo.z_GpsSvInfo[i].w_SvId;
        }
      }
      /* Populate the GLO satellite info */
      for (i = 0; (i < PDSM_N_ACTIVE_GLO_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
      {
        /* Check whether GLO SVs were used in calculating position */
        if(pz_ParsedPositionRpt->z_NavSvInfo.z_GloSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
        {
          pz_PosInd->gnssSvUsedList[q_SvListIdx++] = 
            pz_ParsedPositionRpt->z_NavSvInfo.z_GloSvInfo[i].w_SvId;
        }
      }

      /* Populate the QZSS satellite info */
      for (i = 0; (i < PDSM_N_ACTIVE_QZSS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
      {
        /* Check whether QZSS SVs were used in calculating position */
        if(pz_ParsedPositionRpt->z_NavSvInfo.z_QzssSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
        {
          pz_PosInd->gnssSvUsedList[q_SvListIdx++] = 
            pz_ParsedPositionRpt->z_NavSvInfo.z_QzssSvInfo[i].w_SvId;
        }
      }

      /* Populate the BDS satellite info */
      for (i = 0; (i < PDSM_N_ACTIVE_BDS_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
      {
        /* Check whether GLO SVs were used in calculating position */
        if(pz_ParsedPositionRpt->z_NavSvInfo.z_BdsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
        {
          pz_PosInd->gnssSvUsedList[q_SvListIdx++] = 
            pz_ParsedPositionRpt->z_NavSvInfo.z_BdsSvInfo[i].w_SvId;
        }
      }

      /* Populate the GAL satellite info */
      for (i = 0; (i < PDSM_N_ACTIVE_GAL_CHAN) && (q_SvListIdx < QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02); i++) 
      {
        /* Check whether GAL SVs were used in calculating position */
        if(pz_ParsedPositionRpt->z_NavSvInfo.z_GalSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
        {
          pz_PosInd->gnssSvUsedList[q_SvListIdx++] = 
            pz_ParsedPositionRpt->z_NavSvInfo.z_GalSvInfo[i].w_SvId;
        }
      }

      /** set the valid flag only if the list is non-empty */
      if(q_SvListIdx > 0 )
      {
        pz_PosInd->gnssSvUsedList_len = q_SvListIdx ;
        pz_PosInd->gnssSvUsedList_valid = TRUE;
      }
    }

    /* always populate altitude assumed in position report */
    if ( pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_ALTITUDE_ASSUMED_MASK )
    {
      LOC_QMI_SHIM_SET_VALUE(pz_PosInd->altitudeAssumed,
                             pz_ParsedPositionRpt->altitude_assumed);
    }

   if (pz_ParsedPositionRpt->valid_mask & LOC_POS_VALID_SPEED_ENU)
   {
      pz_PosInd->velEnu_valid = TRUE;
      memscpy(pz_PosInd->velEnu,sizeof(pz_PosInd->velEnu),
              pz_ParsedPositionRpt->f_VelocityENUMps, sizeof(pz_ParsedPositionRpt->f_VelocityENUMps));
      pz_PosInd->velUncEnu_valid = TRUE;
      memscpy(pz_PosInd->velUncEnu,sizeof(pz_PosInd->velUncEnu),
              pz_ParsedPositionRpt->f_VelocityUncENUMps, sizeof(pz_ParsedPositionRpt->f_VelocityUncENUMps));
   }

    pz_IndInfo->q_Id = QMI_LOC_EVENT_POSITION_REPORT_IND_V02;
    pz_IndInfo->q_MsgLen = sizeof ( *pz_PosInd );
    (*pb_SendInd) = TRUE;

    /* send final position report time to ME */
    if(TRUE == *pb_SendInd && 
       eQMI_LOC_SESS_STATUS_SUCCESS_V02 == pz_ParsedPositionRpt->session_status )
    {
       uint64 t_positionReportTime = cgps_TimeTickGetMsec();
       gnss_UpdateQmiPosRptTime(pz_PosInd->gpsTime.gpsWeek, pz_PosInd->gpsTime.gpsTimeOfWeekMs, t_positionReportTime);
       SHIM_MSG_MED( "locQmiHandleParsedPositionReport gpsWeek=%u, gpsTimeOfWeekMs=%u,position report time=%u,",
                      pz_PosInd->gpsTime.gpsWeek,
                      pz_PosInd->gpsTime.gpsTimeOfWeekMs,
                      t_positionReportTime );

     }
  }
        
  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Satellite reports from Loc-Api 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_SatelliteRpt       Satellite report from Loc-Api
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS         Indication handled
@retval    LOC_API_GENERAL_FAILURE Indication could not be handled. Defined in 
                                   loc_api.h
                                  
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleSatelliteReport
(
  const locQmiShimCltInfoStructT        *pz_CltInfo,
  const loc_gnss_info_s_type            *pz_SatelliteRpt,
  locQmiShimMsgInfoStructT              *pz_IndInfo,
  boolean                               *pb_SendInd
)
{
  int32 i_RetVal = LOC_API_SUCCESS; 
  uint8 u_Cntr, u_CurrSv = 0;
  boolean b_Continue = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02 != 
        ( pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02 ) )
  {
    SHIM_MSG_MED( "Client did not register for this report:%d, suppressing",
                  QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02,
                  0,
                  0 );
    /* i_RetVal is not changed as we handled the callback successfully */
  }
  else
  {
    
    qmiLocEventGnssSvInfoIndMsgT_v02 *pz_GnssSvInd =
            (qmiLocEventGnssSvInfoIndMsgT_v02* )pz_IndInfo->p_Msg;
    
    /* SV report based on PD final position report have
       LOC_GNSS_INFO_VALID_ALTITUDE_ASSUMED bit always set. Send this SV report
       based on client config only */
    if ( pz_SatelliteRpt->valid_mask & LOC_GNSS_INFO_VALID_ALTITUDE_ASSUMED )
    {
      /* check the client config for extra SV report with  Altitude Assumed */
      if (eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_DISABLED_V02 == pz_CltInfo->configAltitudeAssumed)
      {
        (*pb_SendInd) = FALSE; /* block the SV event indication to client*/
        return LOC_API_GENERAL_FAILURE;
      } // else ENABLED. So continue and send the report.
      
      /* Mandatory TLV and hence not using the MACRO */
      pz_GnssSvInd->altitudeAssumed = pz_SatelliteRpt->altitude_assumed;
    }
    /* else -->
       SV report based on PD measurement report doesn't have
       LOC_GNSS_INFO_VALID_ALTITUDE_ASSUMED bit set at all. Always send this SV report.
       Default pz_GnssSvInd->altitudeAssumed is FALSE, from memset. */

    if ( ( pz_SatelliteRpt->valid_mask & LOC_GNSS_INFO_VALID_SV_COUNT ) && 
         ( pz_SatelliteRpt->valid_mask & LOC_GNSS_INFO_VALID_SV_LIST ) )
    {
      uint16 w_SvCntToReport = 0;

      /* Cap the max to report */
      w_SvCntToReport = 
          ( pz_SatelliteRpt->sv_count < QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02 ) ?
          pz_SatelliteRpt->sv_count :  QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02;
      
      {
        pz_GnssSvInd->svList_valid = TRUE;

        /* Loop through allowed count of SV-Infos from Loc-Mw */
        for ( u_Cntr = 0, u_CurrSv = 0; 
              u_Cntr < w_SvCntToReport; 
              u_Cntr++ )
        {
          /* Initialize */
          b_Continue = FALSE;
          memset ( &(pz_GnssSvInd->svList[u_CurrSv]), 
                   0, 
                   sizeof(pz_GnssSvInd->svList[u_CurrSv]) );
          
          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                                  LOC_SV_INFO_VALID_SYSTEM )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                        QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02;

            switch ( pz_SatelliteRpt->sv_list[u_Cntr].system )
            {
              case LOC_SV_SYSTEM_GPS:
                pz_GnssSvInd->svList[u_CurrSv].system = 
                                                eQMI_LOC_SV_SYSTEM_GPS_V02;
                break;
              
              case LOC_SV_SYSTEM_GALILEO:
                pz_GnssSvInd->svList[u_CurrSv].system = 
                                                eQMI_LOC_SV_SYSTEM_GALILEO_V02;
                break;

              case LOC_SV_SYSTEM_SBAS:
                pz_GnssSvInd->svList[u_CurrSv].system = 
                                                eQMI_LOC_SV_SYSTEM_SBAS_V02;
                break;
              
              case LOC_SV_SYSTEM_COMPASS:
                pz_GnssSvInd->svList[u_CurrSv].system = 
                                                eQMI_LOC_SV_SYSTEM_COMPASS_V02;
                break;
              
              case LOC_SV_SYSTEM_GLONASS:
                pz_GnssSvInd->svList[u_CurrSv].system = 
                                                eQMI_LOC_SV_SYSTEM_GLONASS_V02;
                break;

              case LOC_SV_SYSTEM_BDS:
                pz_GnssSvInd->svList[u_CurrSv].system = (qmiLocSvSystemEnumT_v02)eQMI_LOC_SV_SYSTEM_BDS_V02;
                break;

              case LOC_SV_SYSTEM_QZSS:
                pz_GnssSvInd->svList[u_CurrSv].system = (qmiLocSvSystemEnumT_v02)eQMI_LOC_SV_SYSTEM_QZSS_V02;
                break;

              default:
                /* Continue to next SV */
                SHIM_MSG_ERROR( "Invalid SV System %d",
                                pz_SatelliteRpt->sv_list[u_Cntr].system,
                                0, 0 );
                /* This structure will be initialized again at start of loop */
                b_Continue = TRUE;
                break;
            }
          }

          /* Go to next SV */
          if ( TRUE == b_Continue )
          {
            continue;
          }  
          
          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                                  LOC_SV_INFO_VALID_PRN )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                      QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02;
            pz_GnssSvInd->svList[u_CurrSv].gnssSvId= 
                                          pz_SatelliteRpt->sv_list[u_Cntr].prn;
          }

          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                              LOC_SV_INFO_VALID_HEALTH_STATUS )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                  QMI_LOC_SV_INFO_MASK_VALID_HEALTH_STATUS_V02;
            pz_GnssSvInd->svList[u_CurrSv].healthStatus = 
                                pz_SatelliteRpt->sv_list[u_Cntr].health_status;
          }

          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                             LOC_SV_INFO_VALID_PROCESS_STATUS )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                 QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02;
            switch ( pz_SatelliteRpt->sv_list[u_Cntr].process_status )
            {
              case LOC_SV_STATUS_IDLE:
                pz_GnssSvInd->svList[u_CurrSv].svStatus = 
                                                  eQMI_LOC_SV_STATUS_IDLE_V02;
                break;
              
              case LOC_SV_STATUS_SEARCH:
                pz_GnssSvInd->svList[u_CurrSv].svStatus = 
                                                  eQMI_LOC_SV_STATUS_SEARCH_V02;
                break;

              case LOC_SV_STATUS_TRACK:
                pz_GnssSvInd->svList[u_CurrSv].svStatus = 
                                                  eQMI_LOC_SV_STATUS_TRACK_V02;
                break;
              default:
                /* Continue to next SV */
                SHIM_MSG_ERROR( "Invalid SV System %d",
                                pz_SatelliteRpt->sv_list[u_Cntr].system,
                                0, 0 );
                /* This structure will be initialized again at start of loop */
                b_Continue = TRUE;
                break;
            }
          }

          /* Go to next SV */
          if ( TRUE == b_Continue )
          {
            continue;
          }  

          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                                  LOC_SV_INFO_VALID_HAS_EPH )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                    QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
            if(pz_SatelliteRpt->sv_list[u_Cntr].has_eph)
            {
              pz_GnssSvInd->svList[u_CurrSv].svInfoMask |= 
                                      QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02;
            }
          }

          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                                  LOC_SV_INFO_VALID_HAS_ALM )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                    QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02;
            if(pz_SatelliteRpt->sv_list[u_Cntr].has_alm)
            {
              pz_GnssSvInd->svList[u_CurrSv].svInfoMask |= 
                                      QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02;
            }
          }

          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                                  LOC_SV_INFO_VALID_ELEVATION )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                    QMI_LOC_SV_INFO_MASK_VALID_ELEVATION_V02;
            pz_GnssSvInd->svList[u_CurrSv].elevation = 
                                    pz_SatelliteRpt->sv_list[u_Cntr].elevation;
          }

          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                                  LOC_SV_INFO_VALID_AZIMUTH )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                      QMI_LOC_SV_INFO_MASK_VALID_AZIMUTH_V02;
            pz_GnssSvInd->svList[u_CurrSv].azimuth= 
                                    pz_SatelliteRpt->sv_list[u_Cntr].azimuth;
          }

          if ( pz_SatelliteRpt->sv_list[u_Cntr].valid_mask & 
                                                  LOC_SV_INFO_VALID_SNR )
          {
            pz_GnssSvInd->svList[u_CurrSv].validMask |= 
                                      QMI_LOC_SV_INFO_MASK_VALID_SNR_V02;
            pz_GnssSvInd->svList[u_CurrSv].snr= 
                                    pz_SatelliteRpt->sv_list[u_Cntr].snr;
          }

          /* since we were able to fill in this SV, lets go to the next
             SV in the indication that is being sent out */
          u_CurrSv++;
        }     

        /* Set the length to the amount that we were able to copy 
           successfully */
        pz_GnssSvInd->svList_len = u_CurrSv;
      }      
    }
    
    /* Send the indication only if there is atleast one SV-Report that
       needs to be sent out */
    if ( 0 != u_CurrSv  )
    {
      (*pb_SendInd) = TRUE;
      pz_IndInfo->q_Id = QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02;
      pz_IndInfo->q_MsgLen = sizeof(*pz_GnssSvInd);
    }
  }
  return i_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Convert Injected GNSS Constellation configuration message
  
@param[in]   pz_Msg               Qmi-Loc Format of Message
@param[out]  pz_IoctlInfo         Loc-API IOCTL Format 

@retval    TRUE                   Message successfully translated
@retval    FALSE                  Message could not be translated successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimConvToLocSetGNSSConfig
(
  const void                                  *pz_Msg,
  loc_ioctl_data_u_type                       *pz_IoctlInfo
)
{
  boolean v_RetVal = TRUE;
  const qmiLocSetGNSSConstRepConfigReqMsgT_v02  *pz_GNSSConfig = 
                      (qmiLocSetGNSSConstRepConfigReqMsgT_v02*) pz_Msg;
  
  qmiLocSetGNSSConstRepConfigReqMsgT_v02  *pz_IoctlData = 
                       &(pz_IoctlInfo->z_GNSS_config);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters 
     Also assumes that the pz_IoctlInfo is already memset'ed to "0" */

  MSG_MED("locQmiShimConvToLocSetGNSSConfig:Meas Rep Valid:%d,SV Poly Rep Valid:%d",pz_GNSSConfig->measReportConfig_valid,
                                        pz_GNSSConfig->svPolyReportConfig_valid,0);

  MSG_MED("locQmiShimConvToLocSetGNSSConfig:Meas Rep Constell:%d,SV Poly Rep Constell:%d",pz_GNSSConfig->measReportConfig,
                                       pz_GNSSConfig->svPolyReportConfig,0);

  if(pz_GNSSConfig->measReportConfig_valid)
  {
    pz_IoctlData->measReportConfig_valid = TRUE;
    pz_IoctlData->measReportConfig = 0;
    if(pz_GNSSConfig->measReportConfig & eQMI_SYSTEM_GPS_V02)
        pz_IoctlData->measReportConfig |= PDSM_SYSTEM_GPS;
    if(pz_GNSSConfig->measReportConfig & eQMI_SYSTEM_GLO_V02)
        pz_IoctlData->measReportConfig |= PDSM_SYSTEM_GLO;
    if(pz_GNSSConfig->measReportConfig & eQMI_SYSTEM_BDS_V02)
        pz_IoctlData->measReportConfig |= PDSM_SYSTEM_BDS;
    if(pz_GNSSConfig->measReportConfig & eQMI_SYSTEM_GAL_V02)
        pz_IoctlData->measReportConfig |= PDSM_SYSTEM_GAL;
    if(pz_GNSSConfig->measReportConfig & eQMI_SYSTEM_QZSS_V02)
        pz_IoctlData->measReportConfig |= PDSM_SYSTEM_QZSS;
  }
  if(pz_GNSSConfig->svPolyReportConfig_valid)
  {
    pz_IoctlData->svPolyReportConfig_valid = TRUE;
    pz_IoctlData->svPolyReportConfig = 0;    
    if(pz_GNSSConfig->svPolyReportConfig & eQMI_SYSTEM_GPS_V02)
        pz_IoctlData->svPolyReportConfig |= PDSM_SYSTEM_GPS;
    if(pz_GNSSConfig->svPolyReportConfig & eQMI_SYSTEM_GLO_V02)
        pz_IoctlData->svPolyReportConfig |= PDSM_SYSTEM_GLO;
    if(pz_GNSSConfig->svPolyReportConfig & eQMI_SYSTEM_BDS_V02)
        pz_IoctlData->svPolyReportConfig |= PDSM_SYSTEM_BDS;
    if(pz_GNSSConfig->svPolyReportConfig & eQMI_SYSTEM_GAL_V02)
        pz_IoctlData->svPolyReportConfig |= PDSM_SYSTEM_GAL;
    if(pz_GNSSConfig->svPolyReportConfig & eQMI_SYSTEM_QZSS_V02)
        pz_IoctlData->svPolyReportConfig |= PDSM_SYSTEM_QZSS;
  }
  /* If received with none of the config enabled, retrun error, to prevent un-initizlied config application */
  if(!pz_GNSSConfig->measReportConfig_valid && !pz_GNSSConfig->svPolyReportConfig_valid)
  {
    v_RetVal = FALSE;
    SHIM_MSG_ERROR("locQmiShimConvToLocSetGNSSConfig: Received Unsupported configuration",0,0,0);
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Mapping of masks from PD SV system enum to QMI-Loc enum.
  This is a singleton, there is no need to define another instance of this
*//* ------------------------------------------------------------------------*/
static struct 
{
  uint64        t_QmiEnum;
  uint64        t_PdEnum;
}z_QmiPdsmSVEnumMap[]=
{
   { eQMI_LOC_SV_SYSTEM_GPS_V02,
   PD_LOC_SV_SYSTEM_GPS },
   { eQMI_LOC_SV_SYSTEM_GALILEO_V02,
   PD_LOC_SV_SYSTEM_GALILEO },
   { eQMI_LOC_SV_SYSTEM_SBAS_V02,
   PD_LOC_SV_SYSTEM_SBAS },
   { eQMI_LOC_SV_SYSTEM_COMPASS_V02, 
   PD_LOC_SV_SYSTEM_COMPASS },
   { eQMI_LOC_SV_SYSTEM_GLONASS_V02,
   PD_LOC_SV_SYSTEM_GLONASS },
   { eQMI_LOC_SV_SYSTEM_BDS_V02,
   PD_LOC_SV_SYSTEM_BDS },
   { eQMI_LOC_SV_SYSTEM_QZSS_V02,
   PD_LOC_SV_SYSTEM_QZSS },
};


/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Satellite measurement reports from Loc-Api 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_rawClkMeasRpt       Satellite measurement report from Loc-Api
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS         Indication handled
@retval    LOC_API_GENERAL_FAILURE Indication could not be handled. Defined in 
                                   loc_api.h                                  
*//* ------------------------------------------------------------------------*/

static int32 locQmiHandleGnssSVMeasReport
(
  const locQmiShimCltInfoStructT        *pz_CltInfo,
  const loc_gnss_meas_s_type            *pz_rawClkMeasRpt,
  locQmiShimMsgInfoStructT              *pz_IndInfo,
  boolean                               *pb_SendInd
)
{

  int32 i_RetVal = LOC_API_SUCCESS; 
  uint8 u_Cntr, u_CurrSv = 0, i, u_MapSize;
  boolean b_Continue = FALSE;
  qmiLocSVMeasurementStructT_v02 * p_dstSvMeas;
  const pdsm_LocSVMeasurementStructType * p_SrcSVMeas;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
#ifdef FEATURE_GNSS_RAW_MEAS_OUTPUT_DEBUG

  SHIM_MSG_MED( "RawMeasurementReport:%d",
               QMI_LOC_EVENT_MASK_GNSS_MEASUREMENT_REPORT_V02,
               0,
               0 );

#endif

  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_GNSS_MEASUREMENT_REPORT_V02 != 
        ( pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_GNSS_MEASUREMENT_REPORT_V02 ) )
  {
    SHIM_MSG_MED( "Client did not register for this report:%d, suppressing",
                  QMI_LOC_EVENT_MASK_GNSS_MEASUREMENT_REPORT_V02,
                  0,
                  0 );
    /* i_RetVal is not changed as we handled the callback successfully */
  }
  else
  {
    
    qmiLocEventGnssSvMeasInfoIndMsgT_v02 *pz_GnssSvMeasInfo =
            (qmiLocEventGnssSvMeasInfoIndMsgT_v02* )pz_IndInfo->p_Msg;

    /* Clear-off destination */
    memset(pz_GnssSvMeasInfo, 0, sizeof(qmiLocEventGnssSvMeasInfoIndMsgT_v02));

    pz_GnssSvMeasInfo->seqNum = pz_rawClkMeasRpt->z_NavGnssMeas.u_seqNum;
    pz_GnssSvMeasInfo->maxMessageNum = pz_rawClkMeasRpt->z_NavGnssMeas.u_maxMessageNum;

    /* Populate leap second information */

    if(pz_rawClkMeasRpt->z_NavGnssMeas.b_LeapSecValid)
    {
      pz_GnssSvMeasInfo->leapSecondInfo_valid = TRUE;
      pz_GnssSvMeasInfo->leapSecondInfo.leapSec = pz_rawClkMeasRpt->z_NavGnssMeas.z_LeapSec.u_leapSec;
      pz_GnssSvMeasInfo->leapSecondInfo.leapSecUnc= pz_rawClkMeasRpt->z_NavGnssMeas.z_LeapSec.u_leapSecUnc;
    }

    /* Populate clock frequency information */

    if(pz_rawClkMeasRpt->z_NavGnssMeas.b_clockFreqValid)
    {
      pz_GnssSvMeasInfo->rcvrClockFrequencyInfo_valid = pz_rawClkMeasRpt->z_NavGnssMeas.b_clockFreqValid;
      pz_GnssSvMeasInfo->rcvrClockFrequencyInfo.clockDrift = pz_rawClkMeasRpt->z_NavGnssMeas.z_clockFreq.f_clockDrift;
      pz_GnssSvMeasInfo->rcvrClockFrequencyInfo.clockDriftUnc= pz_rawClkMeasRpt->z_NavGnssMeas.z_clockFreq.f_clockDriftUnc;  
      pz_GnssSvMeasInfo->rcvrClockFrequencyInfo.sourceOfFreq = (qmiLocSourceofFreqEnumT_v02)pz_rawClkMeasRpt->z_NavGnssMeas.z_clockFreq.e_sourceOfFreq;
    }

    /* Populate Inter-system time biases */

    if(pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsGloInterSystemBias.u_validMask)
    {
      pz_GnssSvMeasInfo->gpsGloInterSystemBias_valid = TRUE;
      pz_GnssSvMeasInfo->gpsGloInterSystemBias.validMask = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsGloInterSystemBias.u_validMask;
      pz_GnssSvMeasInfo->gpsGloInterSystemBias.timeBias = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsGloInterSystemBias.f_sys1Sys2TB;
      pz_GnssSvMeasInfo->gpsGloInterSystemBias.timeBiasUnc = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsGloInterSystemBias.f_sys1Sys2TBUnc;
    }

    if(pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsBdsInterSystemBias.u_validMask)
    {
      pz_GnssSvMeasInfo->gpsBdsInterSystemBias_valid = TRUE;
      pz_GnssSvMeasInfo->gpsBdsInterSystemBias.validMask = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsBdsInterSystemBias.u_validMask;
      pz_GnssSvMeasInfo->gpsBdsInterSystemBias.timeBias = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsBdsInterSystemBias.f_sys1Sys2TB;
      pz_GnssSvMeasInfo->gpsBdsInterSystemBias.timeBiasUnc = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsBdsInterSystemBias.f_sys1Sys2TBUnc;
    }

    if(pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsGalInterSystemBias.u_validMask)
    {
      pz_GnssSvMeasInfo->gpsGalInterSystemBias_valid = TRUE;
      pz_GnssSvMeasInfo->gpsGalInterSystemBias.validMask = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsGalInterSystemBias.u_validMask;
      pz_GnssSvMeasInfo->gpsGalInterSystemBias.timeBias = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsGalInterSystemBias.f_sys1Sys2TB;
      pz_GnssSvMeasInfo->gpsGalInterSystemBias.timeBiasUnc = pz_rawClkMeasRpt->z_NavGnssMeas.z_gpsGalInterSystemBias.f_sys1Sys2TBUnc;  
    }

    if(pz_rawClkMeasRpt->z_NavGnssMeas.z_bdsGloInterSystemBias.u_validMask)
    {
      pz_GnssSvMeasInfo->bdsGloInterSystemBias_valid = TRUE;
      pz_GnssSvMeasInfo->bdsGloInterSystemBias.validMask = pz_rawClkMeasRpt->z_NavGnssMeas.z_bdsGloInterSystemBias.u_validMask;
      pz_GnssSvMeasInfo->bdsGloInterSystemBias.timeBias = pz_rawClkMeasRpt->z_NavGnssMeas.z_bdsGloInterSystemBias.f_sys1Sys2TB;
      pz_GnssSvMeasInfo->bdsGloInterSystemBias.timeBiasUnc = pz_rawClkMeasRpt->z_NavGnssMeas.z_bdsGloInterSystemBias.f_sys1Sys2TBUnc;  
    }

    if(pz_rawClkMeasRpt->z_NavGnssMeas.z_galGloInterSystemBias.u_validMask)
    {
      pz_GnssSvMeasInfo->galGloInterSystemBias_valid = TRUE;
      pz_GnssSvMeasInfo->galGloInterSystemBias.validMask = pz_rawClkMeasRpt->z_NavGnssMeas.z_galGloInterSystemBias.u_validMask;
      pz_GnssSvMeasInfo->galGloInterSystemBias.timeBias = pz_rawClkMeasRpt->z_NavGnssMeas.z_galGloInterSystemBias.f_sys1Sys2TB;
      pz_GnssSvMeasInfo->galGloInterSystemBias.timeBiasUnc = pz_rawClkMeasRpt->z_NavGnssMeas.z_galGloInterSystemBias.f_sys1Sys2TBUnc;  
    }

    if(pz_rawClkMeasRpt->z_NavGnssMeas.z_galBdsInterSystemBias.u_validMask)
    {
      pz_GnssSvMeasInfo->galBdsInterSystemBias_valid = TRUE;
      pz_GnssSvMeasInfo->galBdsInterSystemBias.validMask = pz_rawClkMeasRpt->z_NavGnssMeas.z_galBdsInterSystemBias.u_validMask;
      pz_GnssSvMeasInfo->galBdsInterSystemBias.timeBias = pz_rawClkMeasRpt->z_NavGnssMeas.z_galBdsInterSystemBias.f_sys1Sys2TB;
      pz_GnssSvMeasInfo->galBdsInterSystemBias.timeBiasUnc = pz_rawClkMeasRpt->z_NavGnssMeas.z_galBdsInterSystemBias.f_sys1Sys2TBUnc;  
    }

    if(pz_rawClkMeasRpt->z_NavGnssMeas.b_GnssMeasValid)
    {
      u_MapSize = sizeof(z_QmiPdsmSVEnumMap)/sizeof(z_QmiPdsmSVEnumMap[0]);
      for (  u_Cntr = 0; u_Cntr < u_MapSize; u_Cntr++ )
      {
         if( z_QmiPdsmSVEnumMap[u_Cntr].t_PdEnum == pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.e_system )
         {
            pz_GnssSvMeasInfo->system = (qmiLocSvSystemEnumT_v02)z_QmiPdsmSVEnumMap[u_Cntr].t_QmiEnum;
            break;
         }
      }

      /* Populate System clock information */
      if(pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.b_isSystemTimeValid)
      {
        const pdsm_LocSystemTimeStructType * p_src = &pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.z_systemTime;

        if(pz_GnssSvMeasInfo->system == eQMI_LOC_SV_SYSTEM_GLONASS_V02)
        {
          qmiLocGloTimeStructT_v02 * p_dst = &pz_GnssSvMeasInfo->gloTime;
          p_dst->gloFourYear = p_src->u_gloFourYear;
          p_dst->gloDays = p_src->w_gloDays;
          p_dst->gloMsec = p_src->q_systemMsec;
          p_dst->gloClkTimeBias = p_src->f_systemClkTimeBias;
          p_dst->gloClkTimeUncMs = p_src->f_systemClkTimeUncMs;
          pz_GnssSvMeasInfo->gloTime_valid = TRUE;
        }
        else
        {
          qmiLocGnssTimeStructT_v02 * p_dst = &pz_GnssSvMeasInfo->systemTime;
          const pdsm_LocSystemTimeStructType * p_src = &pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.z_systemTime;  
          p_dst->system = pz_GnssSvMeasInfo->system;
          p_dst->systemWeek = p_src->w_systemWeek;
          p_dst->systemMsec = p_src->q_systemMsec;
          p_dst->systemClkTimeBias = p_src->f_systemClkTimeBias;
          p_dst->systemClkTimeUncMs = p_src->f_systemClkTimeUncMs;
          pz_GnssSvMeasInfo->systemTime_valid = TRUE;
        }
        {
          qmiLocGnssTimeExtStructT_v02 * p_dst = &pz_GnssSvMeasInfo->systemTimeExt;    
          p_dst->refFCount = p_src->q_refFCount;
          p_dst->systemRtc_valid = p_src->b_SystemRtcValid;
          p_dst->systemRtcMs = p_src->t_systemRtcMs;    

          {
             qmiLocTimeSourceEnumT_v02 z_Src = eQMI_LOC_TIME_SRC_INVALID_V02;
             if(TRUE != locQmiShimConvToLocTimeSource(&z_Src, p_src->e_SourceOfTime ))
             {
               p_dst->sourceOfTime = eQMI_LOC_TIME_SRC_INVALID_V02;
             }
             else
             {
               p_dst->sourceOfTime = z_Src;
             }
          }
          pz_GnssSvMeasInfo->systemTimeExt_valid = TRUE;
        }
      }


      /* Populate SV Measurements */

      MSG_MED("RAW_CLK_QMI_SHIM:Fcount:%d,NumSv%d",pz_GnssSvMeasInfo->systemTimeExt.refFCount,pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.u_NumSvs,0);

      if(pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.u_NumSvs != 0)
      {
        pz_GnssSvMeasInfo->svMeasurement_len = pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.u_NumSvs;
        pz_GnssSvMeasInfo->svMeasurement_valid = TRUE;

        for(i=0;i<pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.u_NumSvs;i++)
        {
          p_dstSvMeas = &pz_GnssSvMeasInfo->svMeasurement[i];
          p_SrcSVMeas = &pz_rawClkMeasRpt->z_NavGnssMeas.z_GnssMeas.z_svMeasurement[i];

          p_dstSvMeas->gnssSvId = p_SrcSVMeas->w_gnssSvId;
          p_dstSvMeas->gloFrequency = p_SrcSVMeas->u_gloFrequency;
          p_dstSvMeas->svStatus = (qmiLocSvStatusEnumT_v02)p_SrcSVMeas->e_svStatus;  
          p_dstSvMeas->measurementStatus = p_SrcSVMeas->t_measurementStatus;
          p_dstSvMeas->validMeasStatusMask = (uint64_t)p_SrcSVMeas->t_measurementStatusValid;
          //MSG_MED("RAW_CLK_QMI_SHIM:SVID:%d,Incoming Mstatvalid:%llu",p_dstSvMeas->gnssSvId,p_SrcSVMeas->t_measurementStatusValid,0);
          //MSG_MED("RAW_CLK_QMI_SHIM:Outgoing Mstatvalid:%llu",p_dstSvMeas->validMeasStatusMask,0,0);

          p_dstSvMeas->CNo = p_SrcSVMeas->w_CNo;
          p_dstSvMeas->gloRfLoss = p_SrcSVMeas->w_gloRfLoss;      
          if(p_SrcSVMeas->b_lossOfLockValid)
          {
            p_dstSvMeas->validMask |= QMI_LOC_SV_LOSSOFLOCK_VALID_V02;
            p_dstSvMeas->lossOfLock = p_SrcSVMeas->b_lossOfLock;
          }

          p_dstSvMeas->measLatency = p_SrcSVMeas->x_measLatency;

          p_dstSvMeas->svTimeSpeed.svTimeMs = p_SrcSVMeas->z_svTimeSpeed.q_svMs;
          p_dstSvMeas->svTimeSpeed.svTimeSubMs = p_SrcSVMeas->z_svTimeSpeed.f_svSubMs;
          p_dstSvMeas->svTimeSpeed.svTimeUncMs = p_SrcSVMeas->z_svTimeSpeed.f_svTimeUncMs;
          p_dstSvMeas->svTimeSpeed.dopplerShift = p_SrcSVMeas->z_svTimeSpeed.f_dopplerShift;
          p_dstSvMeas->svTimeSpeed.dopplerShiftUnc = p_SrcSVMeas->z_svTimeSpeed.f_dopplerShiftUnc;

          if(p_SrcSVMeas->b_dopplerAccelValid)
          {
            p_dstSvMeas->svTimeSpeed.dopplerAccel_valid = TRUE;
            p_dstSvMeas->svTimeSpeed.dopplerAccel = p_SrcSVMeas->f_dopplerAccel;
          }

          if(p_SrcSVMeas->b_multipathEstValid)
          {
            p_dstSvMeas->validMask |= QMI_LOC_SV_MULTIPATH_EST_VALID_V02;
            p_dstSvMeas->multipathEstimate = p_SrcSVMeas->f_multipathEstimate;
          }
        
          if(p_SrcSVMeas->b_fineSpeedValid)
          {
            p_dstSvMeas->validMask |= QMI_LOC_SV_FINE_SPEED_VALID_V02;
            p_dstSvMeas->fineSpeed= p_SrcSVMeas->f_fineSpeed;        
          }

          if(p_SrcSVMeas->b_fineSpeedUncValid)
          {
            p_dstSvMeas->validMask |= QMI_LOC_SV_FINE_SPEED_UNC_VALID_V02;
            p_dstSvMeas->fineSpeedUnc= p_SrcSVMeas->f_fineSpeedUnc;      
          }

          if(p_SrcSVMeas->b_carrierPhaseValid)
          {
            p_dstSvMeas->validMask |= QMI_LOC_SV_CARRIER_PHASE_VALID_V02;
            p_dstSvMeas->carrierPhase= p_SrcSVMeas->d_carrierPhase;
          }

          /* AI: Add valid field in QMI */
          if(p_SrcSVMeas->b_cycleSlipCountValid)
          {
            p_dstSvMeas->validMask |= QMI_LOC_SV_CYCLESLIP_COUNT_VALID_V02;
             p_dstSvMeas->cycleSlipCount= p_SrcSVMeas->u_cycleSlipCount;
          }

          if(p_SrcSVMeas->b_svDirectionValid)
          {
            p_dstSvMeas->validMask |= QMI_LOC_SV_SV_DIRECTION_VALID_V02;
            p_dstSvMeas->svAzimuth= p_SrcSVMeas->f_svAzimuth;    
            p_dstSvMeas->svElevation= p_SrcSVMeas->f_svElevation;
          }
          p_dstSvMeas->svInfoMask= p_SrcSVMeas->e_svInfoMask;

          if(p_SrcSVMeas->b_healthStatus_valid)
          {
            p_dstSvMeas->validMask |= QMI_LOC_SV_HEALTH_VALID_V02;
            if(p_SrcSVMeas->u_healthStatus)
            {
              p_dstSvMeas->healthStatus = 1;  // Healthy
            }
            else
            {
              p_dstSvMeas->healthStatus = 0;  //Bad
            }
          }

        //MSG_MED("RAW_CLK_QMI_SHIM:SVID%d,mstat:%u",p_dstSvMeas->gnssSvId,p_dstSvMeas->measurementStatus,0);
        }
      }
      else
      {
        pz_GnssSvMeasInfo->svMeasurement_valid = FALSE;
      }
    }

    (*pb_SendInd) = TRUE;
    pz_IndInfo->q_Id = QMI_LOC_EVENT_GNSS_MEASUREMENT_REPORT_IND_V02;
    pz_IndInfo->q_MsgLen = sizeof(*pz_GnssSvMeasInfo);
  }
  return i_RetVal;
}

#define LOC_SV_POLY_INIT      0   /* No component of pe_SvPoly is valid */
#define LOC_SV_POLY_COEFF     1   /* SV position in orbit coeffients are filled */
#define LOC_SV_POLY_IONO      (1<<1)   /* Iono and its rate are good */
#define LOC_SV_POLY_TROPO     (1<<2)   /* Tropo estimate is good */
#define LOC_SV_POLY_ELEV      (1<<3)   /* Elevation and its rate are good */
#define LOC_SV_POLY_SRC_ALM_CORR (1<<4) /* Polynomials based upon XTRA */
#define LOC_SV_POLY_SBAS_IONO (1<<5)    /* SBAS Iono and its rate are good */
#define LOC_SV_POLY_GLO_STR4 (1<<6)    /* GLO String4 has been received */

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Satellite polynomial reports from Loc-Api 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_SVPolyRpt       Satellite polynomial report from Loc-Api
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS         Indication handled
@retval    LOC_API_GENERAL_FAILURE Indication could not be handled. Defined in 
                                   loc_api.h                                  
*//* ------------------------------------------------------------------------*/

static int32 locQmiHandleSVPolyReport
(
  const locQmiShimCltInfoStructT        *pz_CltInfo,
  const loc_sv_poly_s_type            *pz_SVPolyRpt,
  locQmiShimMsgInfoStructT              *pz_IndInfo,
  boolean                               *pb_SendInd
)
{

  int32 i_RetVal = LOC_API_SUCCESS; 
  uint8 u_Cntr, u_CurrSv = 0, i;
  boolean b_Continue = FALSE;
  qmiLocEventGnssSvPolyIndMsgT_v02 * p_dstSvPoly;
  const pdsm_pd_ext_SvPoly_s_type * p_SrcSVPoly;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

#ifdef FEATURE_GNSS_RAW_SVPOLY_OUTPUT_DEBUG

  SHIM_MSG_MED( "SV Polynomail Report:%d",
                 QMI_LOC_EVENT_MASK_GNSS_SV_POLYNOMIAL_REPORT_V02,
                 0,
                 0 );

#endif
  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_GNSS_SV_POLYNOMIAL_REPORT_V02 != 
        ( pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_GNSS_SV_POLYNOMIAL_REPORT_V02 ) )
  {
    SHIM_MSG_MED( "Client did not register for this report:%d, suppressing",
                  QMI_LOC_EVENT_MASK_GNSS_SV_POLYNOMIAL_REPORT_V02,
                  0,
                  0 );
    /* i_RetVal is not changed as we handled the callback successfully */
  }
  else
  {
    qmiLocEventGnssSvPolyIndMsgT_v02 *pz_SvPolyInfo =
            (qmiLocEventGnssSvPolyIndMsgT_v02* )pz_IndInfo->p_Msg;

  memset(pz_SvPolyInfo, 0, sizeof(qmiLocEventGnssSvPolyIndMsgT_v02));
#ifdef FEATURE_GNSS_RAW_SVPOLY_OUTPUT_DEBUG
  SHIM_MSG_MED( "SV Poly for SV:[%d]",
          pz_SVPolyRpt->z_SvPoly.u_Sv,
          0,
          0 );
#endif

  if(pz_SVPolyRpt->z_SvPoly.w_SvPolyFlags & LOC_SV_POLY_COEFF)
  {

    pz_SvPolyInfo->gnssSvId = pz_SVPolyRpt->z_SvPoly.u_Sv;

    pz_SvPolyInfo->gloFrequency_valid = TRUE;
    pz_SvPolyInfo->gloFrequency  = pz_SVPolyRpt->z_SvPoly.b_FreqNum;

    pz_SvPolyInfo->svPolyFlagValid = (QMI_LOC_SV_POLY_SRC_ALM_CORR_VALID_V02 | QMI_LOC_SV_POLY_GLO_STR4_VALID_V02);

    if(LOC_SV_POLY_SRC_ALM_CORR & pz_SVPolyRpt->z_SvPoly.w_SvPolyFlags)
      pz_SvPolyInfo->svPolyFlags |= QMI_LOC_SV_POLY_SRC_ALM_CORR_V02;
    if(LOC_SV_POLY_GLO_STR4 & pz_SVPolyRpt->z_SvPoly.w_SvPolyFlags)
      pz_SvPolyInfo->svPolyFlags |= QMI_LOC_SV_POLY_GLO_STR4_V02;


    pz_SvPolyInfo->IODE_valid = TRUE;
    pz_SvPolyInfo->IODE = pz_SVPolyRpt->z_SvPoly.u_Iode;

    pz_SvPolyInfo->enhancedIOD_valid = FALSE;
    pz_SvPolyInfo->enhancedIOD = 0;

    pz_SvPolyInfo->T0 = pz_SVPolyRpt->z_SvPoly.d_T0;
    
    pz_SvPolyInfo->polyCoeffXYZ0_valid = TRUE;
    for(i=0;i<QMI_LOC_SV_POLY_XYZ_0_TH_ORDER_COEFF_SIZE_V02;i++)
    {
      pz_SvPolyInfo->polyCoeffXYZ0[i] = pz_SVPolyRpt->z_SvPoly.pd_xyz0[i];
    }

    pz_SvPolyInfo->polyCoefXYZN_valid = TRUE;
    for(i=0;i<QMI_LOC_SV_POLY_XYZ_N_TH_ORDER_COEFF_SIZE_V02;i++)
    {
      pz_SvPolyInfo->polyCoefXYZN[i] = pz_SVPolyRpt->z_SvPoly.pd_xyzN[i];
    }

    pz_SvPolyInfo->polyCoefClockBias_valid = TRUE;
    for(i=0;i<QMI_LOC_SV_POLY_SV_CLKBIAS_COEFF_SIZE_V02;i++)
    {
      pz_SvPolyInfo->polyCoefClockBias[i] = pz_SVPolyRpt->z_SvPoly.pf_other[i];
    }

    pz_SvPolyInfo->svPosUnc_valid = TRUE;
    pz_SvPolyInfo->svPosUnc = pz_SVPolyRpt->z_SvPoly.f_PosUnc;

    if(pz_SVPolyRpt->z_SvPoly.w_SvPolyFlags & LOC_SV_POLY_IONO)
    {
      pz_SvPolyInfo->ionoDelay_valid = TRUE;
      pz_SvPolyInfo->ionoDelay = pz_SVPolyRpt->z_SvPoly.f_IonoDelay;

      pz_SvPolyInfo->ionoDot_valid = TRUE;
      pz_SvPolyInfo->ionoDot = pz_SVPolyRpt->z_SvPoly.f_IonoDot;
    }
    

    if(pz_SVPolyRpt->z_SvPoly.w_SvPolyFlags & LOC_SV_POLY_SBAS_IONO)
    {
      pz_SvPolyInfo->sbasIonoDelay_valid = TRUE;
      pz_SvPolyInfo->sbasIonoDelay = pz_SVPolyRpt->z_SvPoly.f_sbasIonoDelay;

      pz_SvPolyInfo->sbasIonoDot_valid = TRUE;
      pz_SvPolyInfo->sbasIonoDot = pz_SVPolyRpt->z_SvPoly.f_sbasIonoDot;
    }

    if(pz_SVPolyRpt->z_SvPoly.w_SvPolyFlags & LOC_SV_POLY_TROPO)
    {
      pz_SvPolyInfo->tropoDelay_valid = TRUE;
      pz_SvPolyInfo->tropoDelay = pz_SVPolyRpt->z_SvPoly.f_TropoDelay;
    }

    if(pz_SVPolyRpt->z_SvPoly.w_SvPolyFlags & LOC_SV_POLY_ELEV)
    {
      pz_SvPolyInfo->elevation_valid = TRUE;
      pz_SvPolyInfo->elevation = pz_SVPolyRpt->z_SvPoly.f_Elevation;

      pz_SvPolyInfo->elevationDot_valid = TRUE;
      pz_SvPolyInfo->elevationDot = pz_SVPolyRpt->z_SvPoly.f_ElevDot;

      pz_SvPolyInfo->elenationUnc_valid = TRUE;
      pz_SvPolyInfo->elenationUnc = pz_SVPolyRpt->z_SvPoly.f_ElevUnc;
    }


    pz_SvPolyInfo->velCoef_valid = TRUE;
    for(i=0;i<QMI_LOC_SV_POLY_VELOCITY_COEF_SIZE_V02;i++)
    {
      pz_SvPolyInfo->velCoef[i] = pz_SVPolyRpt->z_SvPoly.d_VelCoef[i];
    }

        (*pb_SendInd) = TRUE;
        pz_IndInfo->q_Id = QMI_LOC_EVENT_SV_POLYNOMIAL_REPORT_IND_V02;
        pz_IndInfo->q_MsgLen = sizeof(qmiLocEventGnssSvPolyIndMsgT_v02);
  }
  else
  {
    (*pb_SendInd) = FALSE;
    i_RetVal = LOC_API_GENERAL_FAILURE;
  }
  }
  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle NMEA Sentences from Loc-Api 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_NmeaRpt            NMEA Sentence from Loc-Api
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS         Indication handled
@retval    LOC_API_GENERAL_FAILURE Indication could not be handled. Defined in 
                                   loc_api.h
                                  
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleNmeaPosReport
(
  const locQmiShimCltInfoStructT        *pz_CltInfo,
  const loc_nmea_report_s_type          *pz_NmeaRpt,
  locQmiShimMsgInfoStructT              *pz_IndInfo,
  boolean                               *pb_SendInd
)
{
  int32 i_RetVal = LOC_API_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_NMEA_V02!= 
      ( pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_NMEA_V02 ) )
  {
    SHIM_MSG_MED( "Client did not register for this report:%d, suppressing",
                  QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02,
                  0,
                  0 );
    /* i_RetVal is not changed as we handled the callback successfully */
  }
  else
  {
    qmiLocEventNmeaIndMsgT_v02 *pz_NmeaInd = 
         (qmiLocEventNmeaIndMsgT_v02*) pz_IndInfo->p_Msg;
    
    /* Buffer-Size bounded Copy from Source to destination, and 
       gauranteed NULL-termination, if sentences are truncated then 
       buffer size needs to be increased */
    (void)GNSS_STRLCPY ( (char*)pz_NmeaInd->nmea, 
                  (char*)pz_NmeaRpt->nmea_sentences, 
                  sizeof(pz_NmeaInd->nmea) );

    (*pb_SendInd) = TRUE;
    pz_IndInfo->q_Id = QMI_LOC_EVENT_NMEA_IND_V02;
    pz_IndInfo->q_MsgLen = sizeof(*pz_NmeaInd);
  }

  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Notify Verify Request from Loc-Api 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_NiVerReq           NMEA Sentence from Loc-Api
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS         Indication handled
@retval    LOC_API_GENERAL_FAILURE Indication could not be handled. Defined in 
                                   loc_api.h
                                  
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleNiNotifyVerifyRequest
(
  const locQmiShimCltInfoStructT        *pz_CltInfo,
  const loc_ni_event_s_type             *pz_NiVerReq,
  locQmiShimMsgInfoStructT              *pz_IndInfo,
  boolean                               *pb_SendInd
)
{
  int32 i_RetVal = LOC_API_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ_V02 != 
      ( pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ_V02 ) )
  {
    SHIM_MSG_MED( "Client did not register for this report:%d, suppressing",
                  QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ_V02,
                  0,
                  0 );
    /* i_RetVal is not changed as we handled the callback successfully */
  }
  else
  {
    
    qmiLocEventNiNotifyVerifyReqIndMsgT_v02  *pz_NiRequest = 
         (qmiLocEventNiNotifyVerifyReqIndMsgT_v02*) pz_IndInfo->p_Msg;
    
    switch ( pz_NiVerReq->event )
    {
      case LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ:
      {
        const loc_ni_vx_notify_verify_req_s_type *pz_Vx = 
                                              &pz_NiVerReq->payload.vx_req;
        qmiLocNiVxNotifyVerifyStructT_v02 *pz_QmiLoc = 
                                               &pz_NiRequest->NiVxInd;
        i_RetVal = locQmiShimConvToQmiNiVxNotifyVerify (
                                pz_Vx,
                                pz_QmiLoc );

        if ( LOC_API_SUCCESS == i_RetVal )
        {
           i_RetVal = locQmiShimConvToQmiNiNotificationType ( 
                                 pz_Vx->notification_priv_type,
                                 &pz_NiRequest->notificationType ); 
        }
                                
        if ( LOC_API_SUCCESS == i_RetVal )
        {
          pz_NiRequest->NiVxInd_valid = TRUE;
        }
      }
      break;
      case LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ:
      {
        const loc_ni_supl_notify_verify_req_s_type *pz_Supl = 
                                              &pz_NiVerReq->payload.supl_req;
        qmiLocNiSuplNotifyVerifyStructT_v02  *pz_QmiLoc = 
                                               &pz_NiRequest->NiSuplInd;

        i_RetVal = locQmiShimConvToQmiNiNotificationType ( 
                                pz_Supl->notification_priv_type,
                                &pz_NiRequest->notificationType ); 

        pz_QmiLoc->valid_flags = 0;
        if ( LOC_API_SUCCESS == i_RetVal )
        {
          /* There is no flag for this, look into the presence field */
          if ( TRUE == pz_Supl->supl_slp_session_id.presence )
          {
            pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_SERVER_INFO_MASK_V02;
            pz_QmiLoc->suplServerInfo.suplServerAddrTypeMask = 0;
            if ( pz_Supl->supl_slp_session_id.slp_address.addr_type ==
                                 LOC_SERVER_ADDR_IPV4 ) 
            {
              pz_QmiLoc->suplServerInfo.suplServerAddrTypeMask |= 
                        QMI_LOC_SERVER_ADDR_TYPE_IPV4_MASK_V02;
              pz_QmiLoc->suplServerInfo.ipv4Addr.addr =
                    pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv4.addr;
              pz_QmiLoc->suplServerInfo.ipv4Addr.port = 
                    pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv4.port;
            }
            else if ( pz_Supl->supl_slp_session_id.slp_address.addr_type ==
                                 LOC_SERVER_ADDR_IPV6 )
            {
              
              pz_QmiLoc->suplServerInfo.suplServerAddrTypeMask |= 
                        QMI_LOC_SERVER_ADDR_TYPE_IPV6_MASK_V02;
              memscpy (pz_QmiLoc->suplServerInfo.ipv6Addr.addr,
			  	       sizeof(pz_QmiLoc->suplServerInfo.ipv6Addr.addr),
                       pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv6.addr,
                       sizeof(pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv6.addr));
              
              pz_QmiLoc->suplServerInfo.ipv6Addr.port = 
                    pz_Supl->supl_slp_session_id.slp_address.addr_info.ipv6.port;
            }
            else if ( pz_Supl->supl_slp_session_id.slp_address.addr_type ==
                                 LOC_SERVER_ADDR_URL )
            {
              pz_QmiLoc->suplServerInfo.suplServerAddrTypeMask |= 
                        QMI_LOC_SERVER_ADDR_TYPE_URL_MASK_V02;

              /* Bounded Buffer Copy */
              (void) GNSS_STRLCPY ( 
                 (char*)pz_QmiLoc->suplServerInfo.urlAddr,
                 (char*)pz_Supl->supl_slp_session_id.slp_address.addr_info.url.addr,
                  QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 );
            }
            
            /* Copy from Loc-Api to QMI-Loc capped by the size of QMI-Loc, if 
               session ID length changes in Loc-Api, then the size has to be 
               increased in QMI-Loc.. ie define another const. */
            pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_SESSION_ID_MASK_V02;
            memscpy (pz_QmiLoc->suplSessionId, sizeof(pz_QmiLoc->suplSessionId),
                     pz_Supl->supl_slp_session_id.session_id,
                     sizeof(pz_Supl->supl_slp_session_id.session_id)); 
          }
        }
        
        /* No flag set from PDSM */
        pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_HASH_MASK_V02;
        memscpy (pz_QmiLoc->suplHash, sizeof(pz_QmiLoc->suplHash),
                 pz_Supl->supl_hash, sizeof(pz_Supl->supl_hash));

        
        if ( PDSM_LCS_ENCODING_TYPE_PRESENT & pz_Supl->flags )
        {
          i_RetVal = locQmiShimConvToQmiNiDataCodingScheme ( 
                            pz_Supl->datacoding_scheme,
                            &pz_QmiLoc->dataCodingScheme );
          if ( LOC_API_SUCCESS == i_RetVal )
          {
            pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_DATA_CODING_SCHEME_MASK_V02;
          }
        }         

        
        if ( LOC_API_SUCCESS == i_RetVal )
        {
          switch ( pz_Supl->pos_method )
          {
            case LOC_NI_POSMETHOD_AGPS_SETASSISTED:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_V02;
              break;
            case LOC_NI_POSMETHOD_AGPS_SETBASED:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_V02;
              break;
            case LOC_NI_POSMETHOD_AGPS_SETASSISTED_PREF:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_PREF_V02;
              break;
            case LOC_NI_POSMETHOD_AGPS_SETBASED_PREF:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_PREF_V02;
              break;
            case LOC_NI_POSMETHOD_AUTONOMOUS_GPS:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_AUTONOMOUS_GPS_V02;
              break;
            case LOC_NI_POSMETHOD_AFLT:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_AFLT_V02;
              break;
            case LOC_NI_POSMETHOD_ECID:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_ECID_V02;
              break;
            case LOC_NI_POSMETHOD_EOTD:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_EOTD_V02;
              break;
            case LOC_NI_POSMETHOD_OTDOA:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_OTDOA_V02;
              break;
            case LOC_NI_POSMETHOD_NO_POSITION:
              pz_QmiLoc->posMethod = 
                      eQMI_LOC_NI_SUPL_POSMETHOD_NO_POSITION_V02;
              break;
              
            case LOC_NI_POSMETHOD_MAX:
            default:
            {
              i_RetVal = LOC_API_INVALID_PARAMETER;
              SHIM_MSG_ERROR( "Invalid Option: %d", 
                              pz_Supl->pos_method, 0, 0 );
            }  
            break;
          }
          if ( LOC_API_SUCCESS == i_RetVal )
          {
            pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_POS_METHOD_MASK_V02;
          }
        }

        if ( LOC_API_SUCCESS == i_RetVal )
        {

          if  ( PDSM_LCS_REQUESTOR_ID_PRESENT & pz_Supl->flags )
          {
            i_RetVal = locQmiShimConvToQmiNiFormatType(
                    pz_Supl->requestor_id.data_coding_scheme,
                    &pz_QmiLoc->requestorId.formatType );

            if ( LOC_API_SUCCESS == i_RetVal )
            {
              pz_QmiLoc->requestorId.formattedString_len = locQmiShimMemcpy ( 
                          pz_Supl->requestor_id.string_len,
                          (void*)pz_Supl->requestor_id.requestor_id_string,
                          QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02,
                          (void*)pz_QmiLoc->requestorId.formattedString );

              pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_REQUESTOR_ID_MASK_V02;
            }
          }
        }

        if ( LOC_API_SUCCESS == i_RetVal )
        {
                 
          if  ( PDSM_LCS_CLIENT_NAME_PRESENT & pz_Supl->flags )
          {

            i_RetVal = locQmiShimConvToQmiNiFormatType(
                    pz_Supl->client_name.data_coding_scheme,
                    &pz_QmiLoc->clientName.formatType );

            if ( LOC_API_SUCCESS == i_RetVal ) 
            {
              pz_QmiLoc->clientName.formattedString_len = locQmiShimMemcpy ( 
                          pz_Supl->client_name.string_len,
                          (void*)pz_Supl->client_name.client_name_string,
                          QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02,
                          (void*)pz_QmiLoc->clientName.formattedString );

              pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_CLIENT_NAME_MASK_V02;
            }
          }
        }

        if ( ( LOC_API_SUCCESS == i_RetVal )  &&
             ( PDSM_LCS_SUPL_QOP_VALID & pz_Supl->flags ) )
        {

          pz_QmiLoc->suplQop.horizontalAccuracy = pz_Supl->supl_qop.horacc;
          pz_QmiLoc->suplQop.validMask   |=  
            QMI_LOC_NI_SUPL_MASK_QOP_HORZ_ACC_VALID_V02;

          if ( pz_Supl->supl_qop.bit_mask & PDSM_LCS_SUPL_QOP_MAXAGE_VALID )
          {
            pz_QmiLoc->suplQop.maxLocAge = pz_Supl->supl_qop.maxLocAge;
            pz_QmiLoc->suplQop.validMask   |=  
              QMI_LOC_NI_SUPL_MASK_QOP_MAXAGE_VALID_V02;
          }
          if ( pz_Supl->supl_qop.bit_mask & PDSM_LCS_SUPL_QOP_DELAY_VALID )
          {
            pz_QmiLoc->suplQop.delay = pz_Supl->supl_qop.delay;
            pz_QmiLoc->suplQop.validMask   |=  
              QMI_LOC_NI_SUPL_MASK_QOP_DELAY_VALID_V02;
          }

          if ( pz_Supl->supl_qop.bit_mask & PDSM_LCS_SUPL_QOP_VERACC_VALID )
          {
            pz_QmiLoc->suplQop.verticalAccuracy = pz_Supl->supl_qop.veracc;
            pz_QmiLoc->suplQop.validMask   |=  
              QMI_LOC_NI_SUPL_MASK_QOP_VER_ACC_VALID_V02;
          }
          pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_QOP_MASK_V02;
        }

        if ( LOC_API_SUCCESS == i_RetVal )
        {
          pz_QmiLoc->userResponseTimer = pz_Supl->user_response_timer;
          pz_QmiLoc->valid_flags |= QMI_LOC_SUPL_USER_RESP_TIMER_MASK_V02;
        }
        
        if ( (LOC_API_SUCCESS == i_RetVal) &&
             ( TRUE == pz_Supl->supl_ver2_ext_valid ))
        {
          /** copy the supl version 2 extension payload */
          memscpy(&(pz_NiRequest->NiSuplVer2ExtInd), sizeof(pz_NiRequest->NiSuplVer2ExtInd),
                 &( pz_Supl->supl_ver2_ext), sizeof(pz_Supl->supl_ver2_ext));

          pz_NiRequest->NiSuplVer2ExtInd_valid = TRUE;

          SHIM_MSG_LOW("Notify Request: supl extensions,trigger type = %d,"
                        "supported Networks = %u, gnss type = %u\n", 
                        pz_NiRequest->NiSuplVer2ExtInd.triggerType,
                        pz_NiRequest->NiSuplVer2ExtInd.supportedNetworksMask,
                        pz_NiRequest->NiSuplVer2ExtInd.gnssType);
        }

        if ( (LOC_API_SUCCESS == i_RetVal) &&
             ( TRUE == pz_Supl->supl_emergency_valid ))
        {
          /** copy the supl version 2 extension payload */
          memscpy(&(pz_NiRequest->suplEmergencyNotification), sizeof(pz_NiRequest->suplEmergencyNotification),
                 &( pz_Supl->supl_emergency_notification), sizeof(pz_Supl->supl_emergency_notification));

          pz_NiRequest->suplEmergencyNotification_valid = TRUE;

          SHIM_MSG_HIGH("Notify Request: supl emergency notification URL [0] = %c\n", 
                        pz_NiRequest->suplEmergencyNotification.eslpUrl[0],
                        0, 0);
        }

        if ( LOC_API_SUCCESS == i_RetVal )
        {
          pz_NiRequest->NiSuplInd_valid = TRUE;
        }

      }
      break;
      case LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ:
      {
        const loc_ni_umts_cp_notify_verify_req_s_type  *pz_Umts = 
                                            &pz_NiVerReq->payload.umts_cp_req;
        qmiLocNiUmtsCpNotifyVerifyStructT_v02  *pz_QmiLoc = 
                                             &pz_NiRequest->NiUmtsCpInd;

        /* Initialize the flags, we will use PDAPI masks as LocAPI flags
           are not used anywhere */
        pz_QmiLoc->valid_flags = 0;
        
        i_RetVal = locQmiShimConvToQmiNiNotificationType ( 
                                pz_Umts->notification_priv_type,
                                &pz_NiRequest->notificationType );

        pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_INVOKE_ID_MASK_V02;
        pz_QmiLoc->invokeId = pz_Umts->invoke_id;

        if ( ( LOC_API_SUCCESS == i_RetVal ) &&
             ( pz_Umts->flags & PDSM_LCS_ENCODING_TYPE_PRESENT ) )
        {
          i_RetVal = locQmiShimConvToQmiNiDataCodingScheme ( 
                            pz_Umts->datacoding_scheme,
                            &pz_QmiLoc->dataCodingScheme );
           if ( LOC_API_SUCCESS == i_RetVal )
           {
             pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_DATA_CODING_SCHEME_MASK_V02;
           }
        }

        if (  LOC_API_SUCCESS == i_RetVal )
        {
          if ( pz_Umts->flags & PDSM_LCS_CLIENT_NAME_PRESENT )
          {

            
            pz_QmiLoc->notificationText_len = locQmiShimMemcpy(
                    pz_Umts->notification_length,
                    (void*)pz_Umts->notification_text,
                    QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02,
                    (void*)pz_QmiLoc->notificationText );
            pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_NOTIFICATION_TEXT_MASK_V02;
          }
                            
          if ( pz_Umts->flags & PDSM_LCS_CLIENT_EXTADDR_PRESENT )
          {
          
            pz_QmiLoc->clientAddress_len = 
                    locQmiShimMemcpy(
                      pz_Umts->ext_client_address_data.ext_client_address_len,
                      (void*)pz_Umts->ext_client_address_data.ext_client_address,
                      QMI_LOC_NI_MAX_EXT_CLIENT_ADDRESS_V02,
                      (void*)pz_QmiLoc->clientAddress );
            pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_CLIENT_ADDRESS_MASK_V02;
          }

          /* No flags for location type */
          switch ( pz_Umts->location_type )
          {

            case LOC_NI_LOCATIONTYPE_CURRENT_LOCATION:
              pz_QmiLoc->locationType = 
                      eQMI_LOC_NI_LOCATIONTYPE_CURRENT_LOCATION_V02;
              break;
            case LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION:
              pz_QmiLoc->locationType = 
                   eQMI_LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION_V02;
              break;
            case LOC_NI_LOCATIONTYPE_INITIAL_LOCATION:
              pz_QmiLoc->locationType =
                      eQMI_LOC_NI_LOCATIONTYPE_INITIAL_LOCATION_V02;
              break;
              
            default:
            {
              i_RetVal = LOC_API_INVALID_PARAMETER;
              SHIM_MSG_ERROR( "Invalid Option: %d", 
                              pz_Umts->location_type, 0, 0 );
            }  
            break;
          }
          if ( LOC_API_SUCCESS == i_RetVal ) 
          {
            pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_LOCATION_TYPE_MASK_V02;
          }
        }

        if ( ( LOC_API_SUCCESS == i_RetVal )  &&
             ( pz_Umts->flags & PDSM_LCS_REQUESTOR_ID_PRESENT  ) )
        {
          
          pz_QmiLoc->requestorId.codedString_len = locQmiShimMemcpy ( 
                      pz_Umts->requestor_id.string_len,
                      (void*)pz_Umts->requestor_id.requestor_id_string,
                      QMI_LOC_NI_CODEWORD_MAX_LENGTH_V02,
                      (void*)pz_QmiLoc->requestorId.codedString );

          /* Had to typecast to enum as that is what is present in loc-api */
          i_RetVal = locQmiShimConvToQmiNiDataCodingScheme ( 
                  (loc_ni_datacoding_scheme_e_type)pz_Umts->requestor_id.data_coding_scheme,
                  &pz_QmiLoc->requestorId.dataCodingScheme );
          if ( LOC_API_SUCCESS == i_RetVal ) 
          {
            pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_REQUESTOR_ID_MASK_V02;
          }
        }

        if ( ( LOC_API_SUCCESS == i_RetVal ) &&
             ( pz_Umts->flags & PDSM_LCS_CODEWORD_PRESENT  ) )              
        {
          pz_QmiLoc->codewordString.codedString_len = locQmiShimMemcpy ( 
                      pz_Umts->codeword_string.string_len,
                      (void*)pz_Umts->codeword_string.lcs_codeword_string,
                      QMI_LOC_NI_CODEWORD_MAX_LENGTH_V02,
                      (void*)pz_QmiLoc->codewordString.codedString );

          /* Unfortunate: Have to type cast bcoz of Loc-Api */
          i_RetVal = locQmiShimConvToQmiNiDataCodingScheme ( 
             (loc_ni_datacoding_scheme_e_type)pz_Umts->codeword_string.data_coding_scheme,
             &pz_QmiLoc->codewordString.dataCodingScheme );

          if ( LOC_API_SUCCESS == i_RetVal ) 
          {
            pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_CODEWORD_STRING_MASK_V02;
          }
        }
        
        if ( ( LOC_API_SUCCESS == i_RetVal ) &&
             ( pz_Umts->flags & PDSM_LCS_SERVICE_TYPE_ID_PRESENT ) )
        {
          pz_QmiLoc->lcsServiceTypeId = 
                          pz_Umts->service_type_id.lcs_service_type_id;
          
          pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_SERVICE_TYPE_MASK_V02;
          
        }

        pz_QmiLoc->userResponseTimer = pz_Umts->user_response_timer;
        pz_QmiLoc->valid_flags |= QMI_LOC_UMTS_CP_USER_RESP_TIMER_MASK_V02;
        
        if ( LOC_API_SUCCESS == i_RetVal ) 
        {
          pz_NiRequest->NiUmtsCpInd_valid = TRUE;
        }
      }
      break;
      case LOC_NI_EVENT_VX_SERVICE_INTERACTION_REQ:
      {
        const loc_ni_vx_service_interaction_req_s_type  *pz_VxSvc = 
                                &pz_NiVerReq->payload.service_interaction_req;
        qmiLocNiVxServiceInteractionStructT_v02  *pz_QmiLoc = 
                                      &pz_NiRequest->NiVxServiceInteractionInd;


        i_RetVal = locQmiShimConvToQmiNiVxNotifyVerify (
                                &pz_VxSvc->ni_vx_req,
                                &pz_QmiLoc->niVxReq );

        switch ( pz_VxSvc->service_interation_type )
        {
          case LOC_NI_SERVICE_INTERACTION_ONGOING_NI_INCOMING_MO:
             pz_QmiLoc->serviceInteractionType = 
                    eQMI_LOC_NI_SERVICE_INTERACTION_ONGOING_NI_INCOMING_MO_V02;
             break;   
          default:
          {
            i_RetVal = LOC_API_INVALID_PARAMETER;
            SHIM_MSG_ERROR( "Invalid Option: %d", 
                            pz_VxSvc->service_interation_type, 0, 0 );
          }  
          break;
        }
        
        if ( LOC_API_SUCCESS == i_RetVal )
        {
          pz_NiRequest->NiVxServiceInteractionInd_valid = TRUE;
        }
      }
      break;
      default:
      {
        SHIM_MSG_ERROR( "Not supported : %d" ,
                        pz_NiVerReq->event, 0, 0 );
        i_RetVal = LOC_API_INVALID_PARAMETER;
      }
      break;      
    }

    if ( LOC_API_SUCCESS == i_RetVal )
    {
      pz_IndInfo->q_Id = QMI_LOC_EVENT_NI_NOTIFY_VERIFY_REQ_IND_V02;
      pz_IndInfo->q_MsgLen = sizeof(*pz_NiRequest);
      (*pb_SendInd) = TRUE;
        
    }
  }

  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Assistance data request from Loc-Api. 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_SatelliteRpt       Satellite report from Loc-Api
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS        Indication handled
@retval    <Anything Else>        Indication could not be handled. Defined in 
                                  loc_api.h
                                  
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleAssistanceDataRequest
(
  const locQmiShimCltInfoStructT              *pz_CltInfo,
  const loc_assist_data_request_s_type        *pz_AssistData,
  locQmiShimMsgInfoStructT                    *pz_IndInfo,
  boolean                                     *pb_SendInd
)
{
  int32 i_RetVal = LOC_API_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */


  switch ( pz_AssistData->event )
  {
    case LOC_ASSIST_DATA_POSITION_INJECTION_REQ:
      {
        if ( pz_CltInfo->t_EventMask & 
                  QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02 )
        {
          qmiLocEventInjectPositionReqIndMsgT_v02 *pz_PosInjInd = 
               (qmiLocEventInjectPositionReqIndMsgT_v02*)pz_IndInfo->p_Msg;
          const loc_pos_inj_request_s_type* pz_LocPosInj = 
                                   &(pz_AssistData->payload.pos_injection);
          pz_PosInjInd->horUncCircular = 
                                (float)pz_LocPosInj->position_uncertainty;
          pz_PosInjInd->latitude = pz_LocPosInj->latitude;
          pz_PosInjInd->longitude = pz_LocPosInj->longitude;
          pz_PosInjInd->timestampUtc = pz_LocPosInj->timestamp;
          
          pz_IndInfo->q_Id = QMI_LOC_EVENT_INJECT_POSITION_REQ_IND_V02;
          pz_IndInfo->q_MsgLen = sizeof(*pz_PosInjInd);
          (*pb_SendInd) = TRUE;
        }
      }
    break;

    case LOC_ASSIST_DATA_TIME_REQ:
      {
        if ( pz_CltInfo->t_EventMask & 
                  QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02 )
        {
          uint32 q_NumServers = 0;
          uint32 q_Counter;
          qmiLocEventInjectTimeReqIndMsgT_v02 *pz_TimeInjInd = 
               (qmiLocEventInjectTimeReqIndMsgT_v02*) pz_IndInfo->p_Msg;
          const loc_time_download_source_s_type* pz_LocTimeInj = 
                                   &(pz_AssistData->payload.time_download );
          pz_TimeInjInd->timeServerInfo_valid = TRUE;
          pz_TimeInjInd->timeServerInfo.delayThreshold = 
                                  pz_LocTimeInj->delay_threshold;

          for ( q_Counter = 0;
                q_Counter < LOC_API_MAX_NUM_NTP_SERVERS;
                q_Counter ++ )
          {
            /* Copy only if the information is provided */
            if ( NULL != pz_LocTimeInj->servers[q_Counter] )
            {
              (void)GNSS_STRLCPY( 
                (char*)(&pz_TimeInjInd->timeServerInfo.timeServerList[q_NumServers]),
                (char*)(pz_LocTimeInj->servers[q_Counter]),
                sizeof(char) * ( QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02+1 ) );

              q_NumServers++;
            }
          }

          pz_TimeInjInd->timeServerInfo.timeServerList_len = q_NumServers;
          
          pz_IndInfo->q_Id = QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02;
          pz_IndInfo->q_MsgLen = sizeof(*pz_TimeInjInd);
          (*pb_SendInd) = TRUE;
        }
      }
    break;
    case LOC_ASSIST_DATA_PREDICTED_ORBITS_REQ:
      {
        if ( pz_CltInfo->t_EventMask & 
                  QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02 )
        {
          uint8 u_ServerCnt = 0;
          qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02 *pz_OrbInjInd = 
           (qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02*) pz_IndInfo->p_Msg;
          const loc_predicted_orbits_data_source_s_type* pz_LocOrbitsInj = 
                                  &(pz_AssistData->payload.data_download );
          pz_OrbInjInd->allowedSizes.maxFileSizeInBytes = 
                                   pz_LocOrbitsInj->max_file_size;
          pz_OrbInjInd->allowedSizes.maxPartSize = 
                                   pz_LocOrbitsInj->max_part_size;

          pz_OrbInjInd->serverList_valid = TRUE;
          pz_OrbInjInd->serverList.serverList_len = 0;
          for ( u_ServerCnt = 0;
              ( ( u_ServerCnt < QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02 ) && 
                ( u_ServerCnt < LOC_API_MAX_NUM_PREDICTED_ORBITS_SERVERS ) );
                u_ServerCnt ++ )
          {
            (void)GNSS_STRLCPY( 
              (char*)pz_OrbInjInd->serverList.serverList[u_ServerCnt].serverUrl,
              (char*)pz_LocOrbitsInj->servers[u_ServerCnt],
              sizeof(char) * ( QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02+1 ) );
            
            pz_OrbInjInd->serverList.serverList_len++;
          }

          (*pb_SendInd) = TRUE;
          pz_IndInfo->q_Id = QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02;
          pz_IndInfo->q_MsgLen = sizeof(*pz_OrbInjInd);
        }
      }
    break;

    default:
      {
        i_RetVal = LOC_API_GENERAL_FAILURE;
        SHIM_MSG_ERROR ( "Invalid Event %d",
                         pz_AssistData->event,
                         0, 0 );
      }
    break;
  }
  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Misc Status Reports from Loc-Api. 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_StatusReport       Status Report 
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS        Indication handled
@retval    <Anything Else>        Indication could not be handled. Defined in 
                                  loc_api.h
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleStatusReport
(
  const locQmiShimCltInfoStructT              *pz_CltInfo,
  const loc_status_event_s_type               *pz_StatusReport,
  locQmiShimMsgInfoStructT                    *pz_IndInfo,
  boolean                                     *pb_SendInd
)
{
  int32 i_RetVal = LOC_API_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  (*pb_SendInd) = TRUE;

  switch ( pz_StatusReport->event )
  {
    case LOC_STATUS_EVENT_ENGINE_STATE:
      {
        if ( pz_CltInfo->t_EventMask & 
             QMI_LOC_EVENT_MASK_ENGINE_STATE_V02 )
        {
          qmiLocEventEngineStateIndMsgT_v02 *pz_EngStatInd = 
               (qmiLocEventEngineStateIndMsgT_v02*) pz_IndInfo->p_Msg;
          
          switch ( pz_StatusReport->payload.engine_state )
          {
            
            case LOC_ENGINE_STATE_ON:
              pz_EngStatInd->engineState = eQMI_LOC_ENGINE_STATE_ON_V02;
              break;
            case LOC_ENGINE_STATE_OFF:
              pz_EngStatInd->engineState = eQMI_LOC_ENGINE_STATE_OFF_V02;
              break;
            default:
              i_RetVal = LOC_API_GENERAL_FAILURE;
             (*pb_SendInd) = FALSE;              
              SHIM_MSG_ERROR ( "Invalid Engine State %d",
                               pz_StatusReport->payload.engine_state,
                               0, 0 );
              break;
          }
          pz_IndInfo->q_Id = QMI_LOC_EVENT_ENGINE_STATE_IND_V02;
          pz_IndInfo->q_MsgLen = sizeof(*pz_EngStatInd);
        }
        else
        {
          (*pb_SendInd) = FALSE;
          /* No need to update i_RetVal as handled successfully */
        }
      }
      break;
    case LOC_STATUS_EVENT_FIX_SESSION_STATE:
      {
        if ( pz_CltInfo->t_EventMask & 
            QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02 )
        {
          qmiLocEventFixSessionStateIndMsgT_v02 *pz_FixSessState = 
                (qmiLocEventFixSessionStateIndMsgT_v02 *) pz_IndInfo->p_Msg;

          switch ( pz_StatusReport->payload.fix_session_state )
          {
            case LOC_FIX_SESSION_STATE_BEGIN:
              pz_FixSessState->sessionState = 
                                  eQMI_LOC_FIX_SESSION_STARTED_V02;
              break;
            case LOC_FIX_SESSION_STATE_END:
              pz_FixSessState->sessionState = 
                                  eQMI_LOC_FIX_SESSION_FINISHED_V02;
              break;
            default:
              (*pb_SendInd) = FALSE;
              i_RetVal = LOC_API_GENERAL_FAILURE;
              SHIM_MSG_ERROR ( "Invalid Fix Session State %d",
                               pz_StatusReport->payload.fix_session_state,
                               0, 0 );
              break;

          }

          pz_IndInfo->q_Id = QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02;
          pz_IndInfo->q_MsgLen = sizeof(*pz_FixSessState);
        }
        else
        {
          (*pb_SendInd) = FALSE;
          /* No need to update i_RetVal as handled successfully */
        }
      }
      break;

      default:
        {
          (*pb_SendInd) = FALSE;
          i_RetVal = LOC_API_GENERAL_FAILURE;
          SHIM_MSG_ERROR ( "Invalid Event %d",
                           pz_StatusReport->event,
                           0, 0 );
        }
      break;
  }
  /* Send the indication only if we could translate */

  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Wifi Position Needed request from Loc-Api. 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_StatusReport       Status Report 
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS        Indication handled
@retval    <Anything Else>        Indication could not be handled. Defined in 
                                  loc_api.h
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleWpsNeededRequest
(
  const locQmiShimCltInfoStructT              *pz_CltInfo,
  const pdsm_pd_qwip_data_s_type              *pz_pdQwip,
  locQmiShimMsgInfoStructT                    *pz_IndInfo,
  boolean                                     *pb_SendInd
)
{
  int32 l_RetVal = LOC_API_SUCCESS; 
  
  qmiLocEventWifiReqIndMsgT_v02 *pz_WifiReqInd = 
       (qmiLocEventWifiReqIndMsgT_v02*) pz_IndInfo->p_Msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
  pz_IndInfo->q_Id = QMI_LOC_EVENT_WIFI_REQ_IND_V02;
  pz_IndInfo->q_MsgLen = sizeof(*pz_WifiReqInd);

  switch (pz_pdQwip->request_type)
  {
    case PDSM_PD_QWIP_START_PERIODIC_HI_FREQ_FIXES:
       pz_WifiReqInd->requestType = 
                              eQMI_LOC_WIFI_START_PERIODIC_HI_FREQ_FIXES_V02;
      break;
    case PDSM_PD_QWIP_START_PERIODIC_KEEP_WARM:
      pz_WifiReqInd->requestType = eQMI_LOC_WIFI_START_PERIODIC_KEEP_WARM_V02;
      break;
    case PDSM_PD_QWIP_STOP_PERIODIC_FIXES:
      pz_WifiReqInd->requestType = eQMI_LOC_WIFI_STOP_PERIODIC_FIXES_V02;
      break;
    case PDSM_PD_QWIP_SUSPEND:
      pz_WifiReqInd->requestType = eQMI_LOC_WIFI_STOP_PERIODIC_FIXES_V02;
      break;

    default:
    {
      SHIM_MSG_ERROR( "locQmiHandleWpsNeededRequest invalid request_type: %d", pz_pdQwip->request_type,0,0);
      (*pb_SendInd) = FALSE;
      l_RetVal = LOC_API_INVALID_PARAMETER;
    }
    break;
  }

  if ( LOC_API_SUCCESS == l_RetVal )
  {
    LOC_QMI_SHIM_SET_VALUE ( pz_WifiReqInd->tbfInMs,
                             pz_pdQwip->tbf_ms );
    (*pb_SendInd) = TRUE;
  }
  return l_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to handle Location Server Request From Loc-Api
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pz_StatusReport       Status Report 
@param[out]  pz_IndInfo            Structure to be filled in with indication 
                                   info.
@param[out]  pb_SendInd:TRUE       Ind Info has been filled successfully.
@param[out]  pb_SendInd:FALSE      Ind Info has not been filled successfully.

@retval    LOC_API_SUCCESS        Indication handled
@retval    <Anything Else>        Indication could not be handled. Defined in 
                                  loc_api.h
*//* ------------------------------------------------------------------------*/
static int32 locQmiHandleLocationServerRequest
(
  const locQmiShimCltInfoStructT              *pz_CltInfo,
  const loc_server_request_s_type             *pz_SrvrReq,
  locQmiShimMsgInfoStructT                    *pz_IndInfo,
  boolean                                     *pb_SendInd
)
{
  int32 i_RetVal = LOC_API_SUCCESS; 
  
  qmiLocEventLocationServerConnectionReqIndMsgT_v02 *pz_ConReqInd = 
       (qmiLocEventLocationServerConnectionReqIndMsgT_v02*) pz_IndInfo->p_Msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  if ( pz_CltInfo->t_EventMask & 
       QMI_LOC_EVENT_MASK_LOCATION_SERVER_CONNECTION_REQ_V02 )
  {

    switch ( pz_SrvrReq->event )
    {
      case LOC_SERVER_REQUEST_MULTI_OPEN:
      {
        pz_ConReqInd->requestType = eQMI_LOC_SERVER_REQUEST_OPEN_V02;
        pz_ConReqInd->connHandle = 
                          (uint32) pz_SrvrReq->payload.multi_open_req.conn_handle;
        // The connection type is already converted to the appropriate QMI_LOC type.
        pz_ConReqInd->wwanType = pz_SrvrReq->payload.multi_open_req.connection_type;
      }
      break;
      
      case LOC_SERVER_REQUEST_CLOSE:
      {
        pz_ConReqInd->requestType = eQMI_LOC_SERVER_REQUEST_CLOSE_V02;
        pz_ConReqInd->connHandle = 
                          (uint32) pz_SrvrReq->payload.close_req.conn_handle;
      }
      break;
        
      default:
        i_RetVal = LOC_API_INVALID_PARAMETER;
        break;
    }

    if ( LOC_API_SUCCESS == i_RetVal )
    {
      (*pb_SendInd) = TRUE;
      pz_IndInfo->q_Id = QMI_LOC_EVENT_LOCATION_SERVER_CONNECTION_REQ_IND_V02;
      pz_IndInfo->q_MsgLen = sizeof(*pz_ConReqInd);
    }
  }
  else
  {
    (*pb_SendInd) = FALSE;
    SHIM_MSG_ERROR( "Client Did not register for Location Server Request",
                    0, 0, 0);
  }


  return i_RetVal;
}

/*============================================================================
 *                    L O C - A P I    C A L L B A C K S 
 * =========================================================================*/

/* -----------------------------------------------------------------------*//**
@brief
  Function registered with Loc-Api, which will be called to notify the shim of 
  events from loc-api
  
@param[in]   z_LocHandle           Loc-Client Handle
@param[in]   e_LocEvent            Event identifier
@param[in]   pz_LocEventPayload    Payload of the event

@retval    LOC_API_SUCCESS        Indication handled
@retval    <Anything Else>        Indication could not be handled. Defined in 
                                  loc_api.h
                                  
*//* ------------------------------------------------------------------------*/
static int32 locQmiShimEventCb
(
  loc_client_handle_type           z_LocHandle,
  loc_event_mask_type              e_LocEvent,
  const loc_event_payload_u_type*  pz_LocEventPayload
)
{
  int32                       i_RetVal = LOC_API_GENERAL_FAILURE;
  qmi_csi_error               e_QmiErr;
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;
  locQmiShimMsgInfoStructT    z_IndInfo;
  locQmiShimIndUnionT         z_IndUnion;
  boolean                     b_SendInd = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pz_CltInfo = q_linear_search(  &z_LocQmiShim.z_ClientQ,
                                 locQmiShimCompareLocApiHandle,
                                 (void*)z_LocHandle );

  /* If the client was found */
  if ( ( NULL != pz_CltInfo ) &&
       ( NULL != pz_LocEventPayload ) )
  {
    memset ( &z_IndUnion, 0, sizeof(z_IndUnion) );
    memset ( &z_IndInfo, 0, sizeof(z_IndInfo) );

    /* Set the pointers */
    z_IndInfo.p_Msg = &z_IndUnion;
    
    switch ( e_LocEvent ) 
    {
      case LOC_EVENT_IOCTL_REPORT:
        {
          if ( LOC_QMI_SHIM_IS_LOC_IOCTL_STATUS_VALID(
                     pz_LocEventPayload->ioctl_report.status) )
          {
            i_RetVal = locQmiHandleIoctlReport ( 
                                     pz_CltInfo,
                                     pz_LocEventPayload->ioctl_report.type,
                                     pz_LocEventPayload->ioctl_report.status,
                                     &(pz_LocEventPayload->ioctl_report.data),
                                     &z_IndInfo,
                                     &b_SendInd );
          }
          else
          {
            SHIM_MSG_ERROR( "Invalid IOCTL Status:%d",
                             pz_LocEventPayload->ioctl_report.status,0,0);
            i_RetVal = LOC_API_INVALID_PARAMETER;
          }
        }
        break;
      case LOC_EVENT_PARSED_POSITION_REPORT:
        {
          i_RetVal = locQmiHandleParsedPositionReport (
                                 pz_CltInfo,
                                 &pz_LocEventPayload->parsed_location_report,
                                 &z_IndInfo,
                                 &b_SendInd );
        }
        break;

      case LOC_EVENT_SATELLITE_REPORT:
        {
          i_RetVal = locQmiHandleSatelliteReport (
                                  pz_CltInfo,
                                  &pz_LocEventPayload->gnss_report,
                                  &z_IndInfo,
                                  &b_SendInd );
        }
      break;

      case LOC_EVENT_NMEA_1HZ_REPORT:
      case LOC_EVENT_NMEA_POSITION_REPORT:
        {
          i_RetVal = locQmiHandleNmeaPosReport (
                                  pz_CltInfo,
                                  &pz_LocEventPayload->nmea_report,
                                  &z_IndInfo,
                                  &b_SendInd );
        }
      break;

      case LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST:
        {
          i_RetVal = locQmiHandleNiNotifyVerifyRequest (
                              pz_CltInfo,
                              &pz_LocEventPayload->ni_request,
                              &z_IndInfo,
                              &b_SendInd );
        }
      break;
      
      case LOC_EVENT_ASSISTANCE_DATA_REQUEST:
        {
          i_RetVal = locQmiHandleAssistanceDataRequest (
                                  pz_CltInfo,
                                  &pz_LocEventPayload->assist_data_request,
                                  &z_IndInfo,
                                  &b_SendInd );
        }
      break;

      case LOC_EVENT_STATUS_REPORT:
        {
          i_RetVal = locQmiHandleStatusReport (
                                  pz_CltInfo,
                                  &pz_LocEventPayload->status_report,
                                  &z_IndInfo,
                                  &b_SendInd );
            
        }
      break;

      case LOC_EVENT_WPS_NEEDED_REQUEST:
      {
        i_RetVal = locQmiHandleWpsNeededRequest (
                                  pz_CltInfo,
                                  &pz_LocEventPayload->z_pdQwip,
                                  &z_IndInfo,
                                  &b_SendInd );
           
      }
      break;

      case LOC_EVENT_LOCATION_SERVER_REQUEST:
      {
        i_RetVal = locQmiHandleLocationServerRequest (
                                  pz_CltInfo,
                                  &pz_LocEventPayload->loc_server_request,
                                  &z_IndInfo,
                                  &b_SendInd );
                                    
      }
      break;

	  case LOC_EVENT_GNSS_MEASUREMENT_REPORT:
      {
        i_RetVal = locQmiHandleGnssSVMeasReport (
                                  pz_CltInfo,
                                  &pz_LocEventPayload->gnss_meas_report,
                                  &z_IndInfo,
                                  &b_SendInd );

      }
      break;

	  case LOC_EVENT_SV_POLY_REPORT:
      {
        i_RetVal = locQmiHandleSVPolyReport (
                                  pz_CltInfo,
                                  &pz_LocEventPayload->sv_poly_report,
                                  &z_IndInfo,
                                  &b_SendInd );
                                    
      }
      break;

      default:
        SHIM_MSG_ERROR ( " Unhandled Event %d ",
                         e_LocEvent, 0, 0 );
        i_RetVal = LOC_API_UNSUPPORTED;
        break;
    }
  }
  else
  {
    i_RetVal = LOC_API_INVALID_PARAMETER;
  }

  
  if ( b_SendInd ) 
  {
    
    e_QmiErr = mqcsi_send_ind ( 
                                    pz_CltInfo->p_QmiClientHandle,
                               z_IndInfo.q_Id,
                               z_IndInfo.p_Msg,
                               z_IndInfo.q_MsgLen) ;

    if ( QMI_CSI_NO_ERR != e_QmiErr )
    {
      /* Could not send an event out, some error in QCSI framework */
      SHIM_MSG_ERROR( "locQmiShimEventCb: send QMI_IND 0x00%x failed, e_QmiErr:%d",
                      z_IndInfo.q_Id, e_QmiErr, 0);
      
    }
    else
    {
      SHIM_MSG_MED( "locQmiShimEventCb: Send QMI_IND 0x00%x succeed ",
                      z_IndInfo.q_Id, 0, 0);
    }
  }
  else
  {
    /* Could not send an event out, some error in translation */
    SHIM_MSG_ERROR( "locQmiShimEventCb: Not Send Indication for event 0x%x ",
                    e_LocEvent,
                    0,
                    0 );
  }

  return i_RetVal;
}



/*============================================================================
 *                       M E S S A G E    H A N D L E R S 
 * =========================================================================*/

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle all the messages that convert to an ioctl call
  for loc-api

  Messages handled in this function are:
    QMI_LOC_START_REQ_V02,
    QMI_LOC_INJECT_UTC_TIME_REQ_V02,
  
  
@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pe_QmiErr             if Failure, this will be populated to 
                                      identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimIoctlHdlr
(
  locQmiShimCltInfoStructT               *pz_CltInfo,
  const locQmiShimMsgInfoStructT         *pz_MsgInfo,
  qmi_error_type_v01                     *pe_QmiErr
)
{
  boolean                     v_RetVal = FALSE;
  /* Setting to an unused value to intialize it */
  loc_ioctl_e_type            e_LocIoctlType = LOC_IOCTL_THIRD_PARTY_START_INDEX;
  loc_ioctl_data_u_type       z_IoctlInfo = {0};
  qmiLocStartReqMsgT_v02 *p_startReq = NULL;
  
  uint32  q_Cntr = 0;
  boolean b_FoundIndex = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  SHIM_MSG_MED("locQmiShimIoctlHdlr: QMI MsgID 0x%x", pz_MsgInfo->q_Id, 0 ,0); 
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  /* Find the entry in the Table */
  for ( q_Cntr = 0;
      q_Cntr < ( sizeof(pz_LocQmiShimConvToLoc) / sizeof (pz_LocQmiShimConvToLoc[0]) );
      q_Cntr++ )
  {
    if ( pz_MsgInfo->q_Id == pz_LocQmiShimConvToLoc[q_Cntr].q_qmiLocMsgId )
    {
      b_FoundIndex = TRUE;
      break;
    }
  }

  SHIM_MSG_HIGH("locQmiShimIoctlHdlr: foundIdx %d sizeof(pz_LocQmiShimConvToLoc) %d sizeof (pz_LocQmiShimConvToLoc[0]) %d",
            b_FoundIndex, sizeof(pz_LocQmiShimConvToLoc), sizeof (pz_LocQmiShimConvToLoc[0]) );

  /* If found do the conversion */
  if ( ( sizeof(pz_LocQmiShimConvToLoc) / sizeof (pz_LocQmiShimConvToLoc[0]) > q_Cntr ) && 
       ( TRUE ==  b_FoundIndex ) )
  {

    if ( NULL != pz_LocQmiShimConvToLoc[q_Cntr].pz_ConvFunc )
    {
      v_RetVal = pz_LocQmiShimConvToLoc[q_Cntr].pz_ConvFunc ( 
                                         pz_MsgInfo->p_Msg,
                                         &z_IoctlInfo );
    }  
    else
    { 
      /* Since there is no function to be provided, The LOC-IOCTL 
         needs to be called with no parameters */     
      v_RetVal = TRUE;
    }
    
    e_LocIoctlType = pz_LocQmiShimConvToLoc[q_Cntr].e_LocMwMsgId;

    /*** Special handling of QMI-Loc Messages go here ***/
    
    /* If it is the Event Mask Message, cache the qmi-event-mask for
       future use to filter events when generated from loc-api */
    if ( ( TRUE == v_RetVal ) &&
         ( QMI_LOC_REG_EVENTS_REQ_V02 == pz_MsgInfo->q_Id ) )
    {
      pz_CltInfo->t_EventMask = 
               ((qmiLocRegEventsReqMsgT_v02*)pz_MsgInfo->p_Msg)->eventRegMask;
      /* Check if we need to enable the batch breach notification in GM */
      if(pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_BREACH_NOTIFICATION_V02) 
      {
        locGeofenceHandleBatchBreachReq(TRUE);
      }

    }

    /* If it is the Start Request, cache the session ID to be used when
       relaying the position Reports. 

       NOTE: We do not clean this up, when we get stop, as a new start will 
             overwrite the old session ID. Session ID is a mandatory field */
    else if ( ( TRUE == v_RetVal ) &&
         ( QMI_LOC_START_REQ_V02 == pz_MsgInfo->q_Id ) )
    {
      pz_CltInfo->u_SessionId = 
        ((qmiLocStartReqMsgT_v02*)pz_MsgInfo->p_Msg)->sessionId;
        
      /* Reset the fix ID to zero whenever there is a start request */
      pz_CltInfo->q_fixID = 0;
        
      p_startReq = ((qmiLocStartReqMsgT_v02 *)pz_MsgInfo->p_Msg);
        
      /* check altitude assumed configuration from client */
      if ( p_startReq->configAltitudeAssumed_valid )
      {
        pz_CltInfo->configAltitudeAssumed = p_startReq->configAltitudeAssumed;
      }
      else
      {
          /* Set default. Altitude assumed is ENABLED in SV as per QMI-LOC */
        pz_CltInfo->configAltitudeAssumed = eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_ENABLED_V02;
      }
    }

    else if ( (TRUE == v_RetVal) &&
              (QMI_LOC_SET_OPERATION_MODE_REQ_V02 == pz_MsgInfo->q_Id) )
    {
      
      const qmiLocSetOperationModeReqMsgT_v02     *pz_SetOpMode = 
                          (qmiLocSetOperationModeReqMsgT_v02*) pz_MsgInfo->p_Msg; 

      if(pz_SetOpMode == NULL)
      {        
        SHIM_MSG_MED("locQmiShimIoctlHdlr ERROR: Null pointer while sending operation mode", 0, 0, 0);
  }
      else
      {
        /*Send the operation mode to GF*/
        locGeofenceSetOperationMode(pz_SetOpMode->operationMode);
      }
    }
    
  }
  /*** Handling of messages which are not in the table go here ***/
  else if ( QMI_LOC_SET_SERVER_REQ_V02 == pz_MsgInfo->q_Id )
  {
    v_RetVal = locQmiShimConvToLocSetServer ( pz_MsgInfo->p_Msg,
                                     &z_IoctlInfo,
                                     &e_LocIoctlType );
  }
  else if ( QMI_LOC_GET_SERVER_REQ_V02 == pz_MsgInfo->q_Id )
  {
    v_RetVal = locQmiShimConvToLocGetServer ( pz_MsgInfo->p_Msg,
                                     &z_IoctlInfo,
                                     &e_LocIoctlType );
  }
  else if ( QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02 == 
                                                          pz_MsgInfo->q_Id )
  {
    v_RetVal = locQmiShimConvToLocInformLocationServerConnStatus ( 
                                     pz_MsgInfo->p_Msg,
                                     &z_IoctlInfo,
                                     &e_LocIoctlType );
  }
   else if ( QMI_LOC_SECURE_GET_AVAILABLE_POSITION_REQ_V02 == 
                                                          pz_MsgInfo->q_Id )
  {
    v_RetVal = locQmiShimConvToLocSecureAvailablePosition ( 
                                     pz_MsgInfo,
                                     &e_LocIoctlType,
                                    &z_IoctlInfo  );

  }
  else
  {
    SHIM_MSG_LOW("locQmiShimIoctlHdlr: newer IOCTLs", 0, 0,0);

    /* See if it is one of the newer IOCTLs */
    v_RetVal = locQmiShimCopyToLoc( pz_MsgInfo,
                                    &e_LocIoctlType,
                                    &z_IoctlInfo );

    if ( FALSE == v_RetVal )
    {
      SHIM_MSG_ERROR( "locQmiShimIoctlHdlr: locQmiShimCopyToLoc failed for Msg 0x%x",
                      pz_MsgInfo->q_Id, 0, 0);
    }
  }

  /* If conversion succeeds call the IOCTL */
  if ( TRUE == v_RetVal )
  {
    
    if ( LOC_API_SUCCESS == loc_ioctl ( 
                   pz_CltInfo->z_LocClientHandle,
                   e_LocIoctlType,
                   &z_IoctlInfo ) )
    {
      v_RetVal = TRUE;
    }
    else
    {
      SHIM_MSG_ERROR( "locQmiShimIoctlHdlr: loc_ioctl returned error %d",
                      e_LocIoctlType, 0, 0);
      /* If conversion fails then it is because of invalid input arguments,
         or arguments that are not recognized, there is no other reason to 
         fail */
      (*pe_QmiErr) = QMI_ERR_MALFORMED_MSG_V01;
      v_RetVal = FALSE;
    }
      
    /**
      * No indication need to be sent for this message.  Since this is an
      * IOCTL an indication will be sent if IOCTL Callback is called with
      * the result.
      */

  }
  else
  {
    SHIM_MSG_ERROR( "locQmiShimIoctlHdlr:  Could not Handle QMI request 0x%x",
                    pz_MsgInfo->q_Id, 0, 0);
    (*pe_QmiErr) = QMI_ERR_MALFORMED_MSG_V01;
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle the stop-request from the client
  
@param[in]  pz_CltInfo            Client for whom this message is intended
@param[in]  p_MsgInfo             Information regarding to the message
@param[out] pe_QmiErr             if Failure, this will be populated to 
                                  identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimStopReqHdlr
(
  const locQmiShimCltInfoStructT         *pz_CltInfo,
  const locQmiShimMsgInfoStructT         *pz_MsgInfo,
  qmi_error_type_v01                     *pe_QmiErr
)
{
  boolean                     v_RetVal = TRUE;
  int32                       i_LocApiErr = LOC_API_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
  
    /* Forward call to loc-api */
  i_LocApiErr = loc_stop_fix ( pz_CltInfo->z_LocClientHandle );

  if ( LOC_API_SUCCESS != i_LocApiErr)
  {
    v_RetVal = FALSE;

    switch ( i_LocApiErr )
    {
       case LOC_API_INVALID_HANDLE:
         (*pe_QmiErr) = QMI_ERR_INVALID_HANDLE_V01;
         break;

         /* For all other values relay INTERNAL error */
       default:
         (*pe_QmiErr) = QMI_ERR_INTERNAL_V01;
         break;
    }
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to store and handle the client revision that is given 
  by the application
  
@param[out]  pz_CltInfo            Client for whom this message is intended
@param[in]   p_MsgInfo             Information regarding to the message
@param[out]  pe_QmiErr             if Failure, this will be populated to 
                                   identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleClientRevision
(
  locQmiShimCltInfoStructT               *pz_CltInfo,
  const locQmiShimMsgInfoStructT         *pz_MsgInfo, 
  qmi_error_type_v01                     *pe_QmiErr
)
{
  boolean                     v_RetVal = TRUE;
  const qmiLocInformClientRevisionReqMsgT_v02* pz_Clt = pz_MsgInfo->p_Msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  /* Store the revision of the client revision  */
  pz_CltInfo->q_CltRev = pz_Clt->revision;

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler for the get service revision req_v02 API from the 
  application. 
  
@param[out]  pz_CltInfo            Client for whom this message is intended
@param[in]   p_MsgInfo             Information regarding to the message
@param[out]  pe_QmiErr             if Failure, this will be populated to 
                                   identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleGetServRevisionReq_v02
(
  locQmiShimCltInfoStructT               *pz_CltInfo,
  const locQmiShimMsgInfoStructT         *pz_MsgInfo,
  qmi_error_type_v01                     *pe_QmiErr
)
{
  locQmiShimQueuedIndStructT              z_IndInfo;
  qmiLocGetServiceRevisionIndMsgT_v02*    p_GetSvcRevInd = 
                                         &z_IndInfo.z_IndInfo.z_GetSvcRevInd;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */


  z_IndInfo.pz_CltInfo = pz_CltInfo;
  z_IndInfo.q_IndId    = QMI_LOC_GET_SERVICE_REVISION_IND_V02;
  z_IndInfo.q_IndLen   = sizeof ( z_IndInfo.z_IndInfo.z_GetSvcRevInd );

  p_GetSvcRevInd->status   = eQMI_LOC_SUCCESS_V02;
  p_GetSvcRevInd->revision = LOC_V02_IDL_MINOR_VERS;
  
#ifdef __GRIFFON__
  p_GetSvcRevInd->gnssMeFWVerString_valid   =
                    mgp_GetGNSSSWVersion( 
                        C_MGP_VER_STR_ME, 
                        p_GetSvcRevInd->gnssMeFWVerString, 
                        sizeof(p_GetSvcRevInd->gnssMeFWVerString ));
  
  p_GetSvcRevInd->gnssHostSWVerString_valid =
                    mgp_GetGNSSSWVersion( 
                        C_MGP_VER_STR_HOST, 
                        p_GetSvcRevInd->gnssHostSWVerString,
                        sizeof(p_GetSvcRevInd->gnssHostSWVerString ));
  
  p_GetSvcRevInd->gnssSWVerString_valid     =
                    mgp_GetGNSSSWVersion( 
                        C_MGP_VER_STR_FULL, 
                        p_GetSvcRevInd->gnssSWVerString,
                        sizeof(p_GetSvcRevInd->gnssSWVerString ));
#else
  p_GetSvcRevInd->gnssMeFWVerString_valid   = FALSE;
  p_GetSvcRevInd->gnssHostSWVerString_valid = FALSE;
  p_GetSvcRevInd->gnssSWVerString_valid     = FALSE;
#endif /* __GRIFFON__ */

  /* Queue up the indication that needs to be sent */
  return locQmiShimSerializeInd(&z_IndInfo, pe_QmiErr );

}


static boolean locQmiShimHandleInjectTimeZoneInfoReq
(
  locQmiShimCltInfoStructT        * const pz_CltInfo, 
  const locQmiShimMsgInfoStructT  *pz_MsgInfo, 
  qmi_error_type_v01              *pe_QmiErr,
  qmiLocGenRespMsgT_v02           *pz_QmiResp
)
{
  locQmiShimQueuedIndStructT  z_IndInfo = { 0 };
  boolean v_RetVal = FALSE;

  v_RetVal = loc_geofence_inject_timezone_info(
                 &z_IndInfo.z_IndInfo.z_InjectTZInfoInd,
                 (qmiLocInjectTimeZoneInfoReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                 pz_QmiResp);

  if(v_RetVal)
  {
    MSG_MED("Send Indication to QMI Client for TimeZone info", 0, 0, 0);
      
    z_IndInfo.pz_CltInfo = pz_CltInfo;
    z_IndInfo.q_IndId    = QMI_LOC_INJECT_TIME_ZONE_INFO_IND_V02;
    z_IndInfo.q_IndLen   = sizeof ( z_IndInfo.z_IndInfo.z_InjectTZInfoInd );
  
    /* Queue up the indication that needs to be sent, 
    pe_QmiErr will be set correctly when this returns */
    v_RetVal =  locQmiShimSerializeInd(&z_IndInfo, pe_QmiErr );
  }
  return v_RetVal;
}
  

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle all new APIs added after Loc-Api 1.x. The 
  expectation from these APIs is that no translation is required and they 
  will handle the QMI-LOC data types directly.
  
@param[out]  pz_CltInfo            Client for whom this message is intended
@param[in]   p_MsgInfo             Information regarding to the message
@param[out]  pe_QmiErr             if Failure, this will be populated to 
                                   identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimRouteMessages
(
  locQmiShimCltInfoStructT               * const pz_CltInfo,
  const locQmiShimMsgInfoStructT         *pz_MsgInfo, 
  qmi_error_type_v01                     *pe_QmiErr
)
{
  boolean                     v_RetVal = TRUE;
  qmiLocGenRespMsgT_v02       z_QmiResp; 
  loc_client_info_s_type * const pz_LocClientPtr = 
                        loc_client_find_handle (pz_CltInfo->z_LocClientHandle);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  memset ( &z_QmiResp, 0, sizeof(z_QmiResp) );

  switch ( pz_MsgInfo->q_Id )
  {
    case QMI_LOC_SET_SPI_STATUS_REQ_V02:
      {
        (void) loc_qmiloc_set_spi_status_req(
                         pz_LocClientPtr,
                         (qmiLocSetSpiStatusReqMsgT_v02*)pz_MsgInfo->p_Msg,
                         &z_QmiResp );
    
      }
    break;

    case QMI_LOC_INJECT_SENSOR_DATA_REQ_V02:
      {
        (void) loc_qmiloc_inject_sensor_data_req(
                         pz_LocClientPtr,
                         (qmiLocInjectSensorDataReqMsgT_v02*)pz_MsgInfo->p_Msg,
                         &z_QmiResp );
      }
    break;

    case QMI_LOC_INJECT_TIME_SYNC_DATA_REQ_V02:
      {
        (void) loc_qmiloc_inject_time_sync_data_req(
                         pz_LocClientPtr,
                         (qmiLocInjectTimeSyncDataReqMsgT_v02*)pz_MsgInfo->p_Msg,
                         &z_QmiResp );
      }
    break;

    case QMI_LOC_SET_CRADLE_MOUNT_CONFIG_REQ_V02:
      {
        (void) loc_qmiloc_set_cradle_mount_config_req (
                      pz_LocClientPtr,
                     (qmiLocSetCradleMountConfigReqMsgT_v02*)pz_MsgInfo->p_Msg,
                     &z_QmiResp );
      }
    break;

    case QMI_LOC_GET_CRADLE_MOUNT_CONFIG_REQ_V02:
      {
        (void) loc_qmiloc_get_cradle_mount_config_req (
                                    pz_LocClientPtr,
                                   &z_QmiResp );
      }
    break;

    case QMI_LOC_SET_EXTERNAL_POWER_CONFIG_REQ_V02:
      {
        cgps_BatteryChargerStatusEnumType e_ExtChargerStatus;
        qmiLocSetExternalPowerConfigReqMsgT_v02 *p_ConfigMsg;
        qmiLocExternalPowerConfigEnumT_v02 externalPowerState;

        (void) loc_qmiloc_set_external_power_config_req(
                  pz_LocClientPtr,
                  (qmiLocSetExternalPowerConfigReqMsgT_v02*)pz_MsgInfo->p_Msg,
                  &z_QmiResp );

        /* Set the Charger status in CGPS Api */
        /* Translate the QMI Loc charger status to internal version */
        p_ConfigMsg = 
          (qmiLocSetExternalPowerConfigReqMsgT_v02*)pz_MsgInfo->p_Msg;
        externalPowerState = p_ConfigMsg->externalPowerState;
        switch ( externalPowerState )
        {
          case eQMI_LOC_EXTERNAL_POWER_NOT_CONNECTED_V02:
            e_ExtChargerStatus = CGPS_BATTERY_CHARGER_NOT_CONNECTED;
            break;
          case eQMI_LOC_EXTERNAL_POWER_CONNECTED_V02:
            /* Assume it is connected and charged */
            e_ExtChargerStatus = CGPS_BATTERY_CHARGER_CONNECTED_CHARGED;
            break;

          case eQMI_LOC_EXTERNAL_POWER_UNKNOWN_V02:
          default:
            /* Default to charger state unknown */
            e_ExtChargerStatus = CGPS_BATTERY_CHARGER_STATE_UNKNOWN;
            break;
        }
        cgps_SetExternalChargerStatus( e_ExtChargerStatus );
      }
    break;

    case QMI_LOC_GET_EXTERNAL_POWER_CONFIG_REQ_V02:
      {
        (void) loc_qmiloc_get_external_power_config_req(
                  pz_LocClientPtr,
                  &z_QmiResp );
      }
    break;
  case QMI_LOC_SET_SENSOR_CONTROL_CONFIG_REQ_V02:
    {
      (void) loc_qmiloc_set_sensor_control_config_req(
                                              pz_LocClientPtr,
                                              (qmiLocSetSensorControlConfigReqMsgT_v02*)pz_MsgInfo->p_Msg,
                                              &z_QmiResp );
      (void) loc_slim_set_sensor_control_config_req((qmiLocSetSensorControlConfigReqMsgT_v02*)pz_MsgInfo->p_Msg);

      break;
    }
  
  case QMI_LOC_GET_SENSOR_CONTROL_CONFIG_REQ_V02:
    {
      (void) loc_qmiloc_get_sensor_control_config_req(
                                              pz_LocClientPtr,
                                              &z_QmiResp );
      break;
    }
  
  case QMI_LOC_SET_SENSOR_PROPERTIES_REQ_V02:
    {
      (void) loc_qmiloc_set_sensor_properties_req(
                                          pz_LocClientPtr,
                                          (qmiLocSetSensorPropertiesReqMsgT_v02*)pz_MsgInfo->p_Msg,
                                          &z_QmiResp );

      break;
    }
  
  case QMI_LOC_GET_SENSOR_PROPERTIES_REQ_V02:
    {
      (void) loc_qmiloc_get_sensor_properties_req(
                                          pz_LocClientPtr,
                                          (qmiLocGetSensorPropertiesReqMsgT_v02*)pz_MsgInfo->p_Msg,
                                          &z_QmiResp );
      break;
    }
  
  case QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02:
    {
      (void) loc_qmiloc_set_sensor_performance_control_config_req(
                                                          pz_LocClientPtr,
                                                          (qmiLocSetSensorPerformanceControlConfigReqMsgT_v02*)pz_MsgInfo->p_Msg,
                                                          &z_QmiResp );
      break;
    }
  
  case QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02:
    {

      (void) loc_qmiloc_get_sensor_performance_control_config_req(
                                                          pz_LocClientPtr,
                                                          &z_QmiResp );
      break;
    }
    case QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ_V02:
      {
        v_RetVal = locGeofenceAdd(
          pz_CltInfo->z_LocClientHandle,
          (qmiLocAddCircularGeofenceReqMsgT_v02 *) pz_MsgInfo->p_Msg,
           &z_QmiResp);
        break;
      }

    case QMI_LOC_DELETE_GEOFENCE_REQ_V02:
      {
        v_RetVal = locGeofenceDelete(
          pz_CltInfo->z_LocClientHandle,
          (qmiLocDeleteGeofenceReqMsgT_v02 *) pz_MsgInfo->p_Msg,
           &z_QmiResp);
        break;
      }
   
    case QMI_LOC_EDIT_GEOFENCE_REQ_V02:
      {
        v_RetVal = locGeofenceEdit(
          pz_CltInfo->z_LocClientHandle,
          (qmiLocEditGeofenceReqMsgT_v02 *) pz_MsgInfo->p_Msg,
           &z_QmiResp);
        break;
      } 
    
    case QMI_LOC_QUERY_GEOFENCE_REQ_V02:
      {
        v_RetVal = locGeofenceQuery(
          pz_CltInfo->z_LocClientHandle,
          (qmiLocQueryGeofenceReqMsgT_v02 *) pz_MsgInfo->p_Msg,
           &z_QmiResp);
        break;
      } 
    case QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_REQ_V02:
      {
        v_RetVal = locGeofenceSetEngineConfig(
        pz_CltInfo->z_LocClientHandle,
        (qmiLocSetGeofenceEngineConfigReqMsgT_v02 *) pz_MsgInfo->p_Msg,
         &z_QmiResp);
        break;
      } 

    case QMI_LOC_PEDOMETER_REPORT_REQ_V02:
      {
        v_RetVal = 
          loc_slim_pedometer_report_req(
            pz_LocClientPtr,
            (qmiLocPedometerReportReqMsgT_v02 *)pz_MsgInfo->p_Msg,
            &z_QmiResp );
        break;
      }
    
    case QMI_LOC_INJECT_MOTION_DATA_REQ_V02:
      {
        v_RetVal = 
          loc_slim_inject_motion_data_req(
            pz_LocClientPtr,
            (qmiLocInjectMotionDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
            &z_QmiResp);
        break;
      }

     case QMI_LOC_INJECT_VEHICLE_SENSOR_DATA_REQ_V02:
      {
        (void)loc_slim_inject_vehicle_sensor_data_req(pz_LocClientPtr,
                                                     (qmiLocInjectVehicleSensorDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                                     &z_QmiResp);
        break;
      }

     case QMI_LOC_INJECT_WIFI_AP_DATA_REQ_V02:
      {
        v_RetVal = 
          loc_pd_process_inject_wifi_ap_data_req(
             pz_LocClientPtr,
            (qmiLocInjectWifiApDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
            &z_QmiResp);
        break;
      }

    case QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_REQ_V02:
      (void) loc_gtp_InjectGtpClientDownloadedDataReq(pz_LocClientPtr, 
                                                      (qmiLocInjectGtpClientDownloadedDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                                      &z_QmiResp );
      break;

    case QMI_LOC_GDT_UPLOAD_BEGIN_STATUS_REQ_V02:
      loc_gdt_UploadBeginStatusReq(pz_LocClientPtr, 
                                   (qmiLocGdtUploadBeginStatusReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                   &z_QmiResp);
      break;

    case QMI_LOC_GDT_UPLOAD_END_REQ_V02:
      loc_gdt_UploadEndStatusReq(pz_LocClientPtr, 
                                 (qmiLocGdtUploadEndReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                 &z_QmiResp);
      break;

    case QMI_LOC_INJECT_TIME_ZONE_INFO_REQ_V02:
      v_RetVal = locQmiShimHandleInjectTimeZoneInfoReq(
                                  pz_CltInfo, 
                                  pz_MsgInfo, 
                                  pe_QmiErr,
                                  &z_QmiResp);

      break;
      

    case QMI_LOC_INJECT_APCACHE_DATA_REQ_V02:
      loc_wle_InjectApCacheDataReq(pz_LocClientPtr, 
                                   (qmiLocInjectApCacheDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                   &z_QmiResp);
      break;

    case QMI_LOC_INJECT_APDONOTCACHE_DATA_REQ_V02:
      loc_wle_InjectNoApCacheDataReq(pz_LocClientPtr, 
                                     (qmiLocInjectApDoNotCacheDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                     &z_QmiResp);
      break;

    default:
      SHIM_MSG_ERROR( "This message:0x:%x has no known handler",
                      pz_MsgInfo->q_Id, 0, 0);
      v_RetVal = FALSE;
      break;
  }

 /* Set this anyways, this will be read only if v_RetVal = TRUE, to 
    send out the response to the client */
 (*pe_QmiErr) = z_QmiResp.resp.error;

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to convert the Set Protocol Config request to 
  appropriate LOC IOCTL
  
   
@param[in|out]  pz_CltInfo        Client for whom this message is intended
@param[in]      p_MsgInfo         Information regarding to the message

@param[out]     pe_QmiErr         if Failure, this will be populated to 
                                  identify the reason for error

@retval    TRUE                   Message successfully handled. It means that 
                                  there is atleast one Loc-IOCTL that got
                                  accepted and an indication will be generated.
@retval    FALSE                  Message could not be handled successfully*//* ------------------------------------------------------------------------*/

static boolean locQmiShimHandleSetProtocolConfigReq
(
  locQmiShimCltInfoStructT               *pz_CltInfo,
  const locQmiShimMsgInfoStructT         *pz_MsgInfo, 
  qmi_error_type_v01                     *pe_QmiErr
)
{
  boolean v_RetVal = FALSE;
  const qmiLocSetProtocolConfigParametersReqMsgT_v02 *pz_QmiLocSetProtReq =
             (qmiLocSetProtocolConfigParametersReqMsgT_v02*)pz_MsgInfo->p_Msg;
  uint8 u_CntTlv = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  u_CntTlv += ((pz_QmiLocSetProtReq->suplSecurity_valid)?1:0);
  u_CntTlv += ((pz_QmiLocSetProtReq->vxVersion_valid)?1:0);
  u_CntTlv += ((pz_QmiLocSetProtReq->suplVersion_valid)?1:0);
  u_CntTlv += ((pz_QmiLocSetProtReq->lppConfig_valid)?1:0);
  u_CntTlv += ((pz_QmiLocSetProtReq->assistedGlonassProtocolMask_valid)?1:0);
  u_CntTlv += ((pz_QmiLocSetProtReq->emergencyProtocol_valid)?1:0);
  u_CntTlv += ((pz_QmiLocSetProtReq->suplHashAlgo_valid)?1:0);
  u_CntTlv += ((pz_QmiLocSetProtReq->suplTlsVersion_valid)?1:0);
  u_CntTlv += ((pz_QmiLocSetProtReq->wifiScanInjectTimeout_valid)?1:0);
  /* If more than one TLV is set return NOT SUPPORTED */
  do
  {
    if ( u_CntTlv > 1 )
    {
      (*pe_QmiErr) = QMI_ERR_NOT_SUPPORTED_V01;
      SHIM_MSG_ERROR(" locQmiShimHandleSetProtocolConfig : "
                     "More than one configuration specified",0,0,0);
      v_RetVal = FALSE;
      break;
    }
  
    /* If no TLV is specified return success and queue a success indication as well*/
    if( 0 == u_CntTlv )
    {
  
      locQmiShimQueuedIndStructT              z_IndInfo;
      qmiLocSetProtocolConfigParametersIndMsgT_v02 *p_SetProtocolConfigInd = 
                                           &z_IndInfo.z_IndInfo.z_SetProtConfigParamInd;
    
      z_IndInfo.pz_CltInfo = pz_CltInfo;
      z_IndInfo.q_IndId    = QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02;
      z_IndInfo.q_IndLen   = sizeof ( z_IndInfo.z_IndInfo.z_SetProtConfigParamInd );
      p_SetProtocolConfigInd->status   = eQMI_LOC_SUCCESS_V02;
  
      /* Queue up the indication that needs to be sent, 
      pe_QmiErr will be set correctly when this returns */
      v_RetVal =  locQmiShimSerializeInd(&z_IndInfo, pe_QmiErr );
      break;
    }
  
    /* one and only one TLV is specified */
    else
    {
      loc_ioctl_data_u_type   z_IoctlInfo;
      loc_ioctl_e_type e_IoctlType = 0;

      memset( &z_IoctlInfo, 0, sizeof(z_IoctlInfo) );
        
      if ( TRUE == pz_QmiLocSetProtReq->suplSecurity_valid )
      {
        /* zero --> FALSE and non-zero --> TRUE */
        z_IoctlInfo.supl_security = 
              (0 == pz_QmiLocSetProtReq->suplSecurity ) ? FALSE:TRUE;
    
        /* The only parameter that needs to be checked here is the suplSecurity, 
           We need to ensure that it is indeed a boolean */
        if ( FALSE == locQmiShimIsValidBoolean(pz_QmiLocSetProtReq->suplSecurity))
        {
          (*pe_QmiErr) = QMI_ERR_MALFORMED_MSG_V01;
           v_RetVal = FALSE;
           break;
        }
  
        e_IoctlType = LOC_IOCTL_SET_SUPL_SECURITY;
      }  
     
      else if ( TRUE == pz_QmiLocSetProtReq->vxVersion_valid )
      {
        z_IoctlInfo.vx_config =
                    pz_QmiLocSetProtReq->vxVersion;
        e_IoctlType = LOC_IOCTL_SET_VX_CONFIG;
      }    
      
      else if ( TRUE == pz_QmiLocSetProtReq->suplVersion_valid )
      {
         
        /* The enums match the version number and hence typecast and assignment,
           Refer to definition of supl version for the translation */
        z_IoctlInfo.supl_version =  
            (eQMI_LOC_SUPL_VERSION_2_0_V02 == pz_QmiLocSetProtReq->suplVersion)?
            (0x00020000):(0x00010000);
  
        e_IoctlType = LOC_IOCTL_SET_SUPL_VERSION;
      }    
     
      else if ( TRUE == pz_QmiLocSetProtReq->lppConfig_valid )
      {
       
        z_IoctlInfo.lpp_config = pz_QmiLocSetProtReq->lppConfig;
        e_IoctlType = LOC_IOCTL_SET_LPP_CONFIG;
      }    
      
      else if ( TRUE == pz_QmiLocSetProtReq->assistedGlonassProtocolMask_valid )
      {
        z_IoctlInfo.assisted_glo_config = 
          pz_QmiLocSetProtReq->assistedGlonassProtocolMask;
        e_IoctlType = LOC_IOCTL_SET_ASSISTED_GLO_CONFIG;
      }

      else if ( TRUE == pz_QmiLocSetProtReq->emergencyProtocol_valid )
      {
        z_IoctlInfo.emergency_protocol =  pz_QmiLocSetProtReq->emergencyProtocol;
        e_IoctlType = LOC_IOCTL_SET_EMERGENCY_PROTOCOL;
      }
      else if ( TRUE == pz_QmiLocSetProtReq->suplHashAlgo_valid )
      {
        z_IoctlInfo.supl_hash_algo = pz_QmiLocSetProtReq->suplHashAlgo;
        e_IoctlType = LOC_IOCTL_SET_SUPL_HASH_ALGO;
      }

      else if ( TRUE == pz_QmiLocSetProtReq->suplTlsVersion_valid )
      {
        z_IoctlInfo.supl_tls_version = pz_QmiLocSetProtReq->suplTlsVersion;
        e_IoctlType = LOC_IOCTL_SET_SUPL_TLS_VERSION;
      }

      else if ( TRUE == pz_QmiLocSetProtReq->wifiScanInjectTimeout_valid )
      {
        z_IoctlInfo.wifi_scan_inject_timeout = pz_QmiLocSetProtReq->wifiScanInjectTimeout;
        e_IoctlType = LOC_IOCTL_SET_WIFI_SCAN_INJECT_TIMEOUT;
      }

      *pe_QmiErr = locQmiShimConvToQmiError(
        loc_ioctl (pz_CltInfo->z_LocClientHandle,
                   e_IoctlType,
                   &(z_IoctlInfo ) ) );

      if( QMI_ERR_NONE_V01 != *pe_QmiErr )
      {
        SHIM_MSG_ERROR("Set protocol config ioctl %d failed",e_IoctlType,0,0);
        v_RetVal = FALSE;
        break;
      }
      else
      {
        /* In case of no-error, the information will be recvd as part of 
               Loc-IOCTL Callback */

        v_RetVal = TRUE;
      }
    }    
     
  }while(0);

  return v_RetVal;
  
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle the Get Protocol Config Param and convert it
  to appropriate LOC IOCTL .
  
@param[in|out]  pz_CltInfo        Client for whom this message is intended
@param[in]      p_MsgInfo         Information regarding to the message
@param[out]     pe_QmiErr         if Failure, this will be populated to 
                                  identify the reason for error

@retval    TRUE                   Message successfully handled. It means that 
                                  there is atleast one Loc-IOCTL that got
                                  accepted and an indication will be generated.
@retval    FALSE                  Message could not be handled successfully
*//* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleGetProtocolConfigReq
(
  locQmiShimCltInfoStructT               *pz_CltInfo,
  const locQmiShimMsgInfoStructT         *pz_MsgInfo, 
  qmi_error_type_v01                     *pe_QmiErr
)
{
 
  const qmiLocGetProtocolConfigParametersReqMsgT_v02 *pz_QmiLocGetProtReq =
             (qmiLocGetProtocolConfigParametersReqMsgT_v02*)pz_MsgInfo->p_Msg;

  loc_ioctl_e_type e_IoctlType = 0;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */


  /* If more than one flag is set return NOT SUPPORTED */
  if ( ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_SECURITY_V02 ) &&
       ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_VX_VERSION_V02 ) &&
       ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_VERSION_V02 )&&
       ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_LPP_CONFIG_V02 ) &&
       ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_ASSISTED_GLONASS_PROTOCOL_V02 ) &&
       ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_EMERGENCY_PROTOCOL_V02 ) &&
       ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_HASH_ALGO_V02 ) &&
       ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_TLS_VERSION_V02 ) &&
       ( pz_QmiLocGetProtReq->getProtocolConfigParamMask != 
                      QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_WIFI_SCAN_INJECT_TIMEOUT_V02 ) )
  {
    (*pe_QmiErr) = QMI_ERR_NOT_SUPPORTED_V01;

    return FALSE;
  }

  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_SECURITY_V02 )
  {
     e_IoctlType = LOC_IOCTL_GET_SUPL_SECURITY; 
  }    
 
  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_VX_VERSION_V02 )
  {
     e_IoctlType = LOC_IOCTL_GET_VX_CONFIG;
  }    
  
  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_VERSION_V02 )
  {
    e_IoctlType = LOC_IOCTL_GET_SUPL_VERSION;
  }

  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_LPP_CONFIG_V02 )
  {
    e_IoctlType =  LOC_IOCTL_GET_LPP_CONFIG;
  }

  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_ASSISTED_GLONASS_PROTOCOL_V02 )
  {
    e_IoctlType =  LOC_IOCTL_GET_ASSISTED_GLO_CONFIG;
  }

  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_EMERGENCY_PROTOCOL_V02 )
  {
    e_IoctlType =  LOC_IOCTL_GET_EMERGENCY_PROTOCOL;
  }
  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_HASH_ALGO_V02 )
  {
    e_IoctlType =  LOC_IOCTL_GET_SUPL_HASH_ALGO;
  }

  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_TLS_VERSION_V02 )
  {
    e_IoctlType =  LOC_IOCTL_GET_SUPL_TLS_VERSION;
  }

  if ( pz_QmiLocGetProtReq->getProtocolConfigParamMask & 
           QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_WIFI_SCAN_INJECT_TIMEOUT_V02 )
  {
    e_IoctlType =  LOC_IOCTL_GET_WIFI_SCAN_INJECT_TIMEOUT;
  }

  *pe_QmiErr = locQmiShimConvToQmiError( 
    loc_ioctl ( 
      pz_CltInfo->z_LocClientHandle,
      e_IoctlType,
      NULL ) );
  
  SHIM_MSG_HIGH(" locQmiShimHandleGetProtocolConfig: ioctl %d qmi error %d",
                    e_IoctlType ,*pe_QmiErr, 0); 
  return ((*pe_QmiErr == QMI_ERR_NONE_V01) ? TRUE : FALSE);
}

/*============================================================================
 *                       Q C S I    C A L L B A C K S 
 * =========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Callback function provided to QCSI, which notify the shim layer of a 
  new client connecting. 
  
@param[in]   pz_ClientHandle   Handle of client that is connecting allocated by
                               QCSI
@param[in]   pz_ServiceCookie  Handle of service, as provided by SHIM layer to
                               QCSI during registration of service. 
@param[out]  pz_ConHandle      Connection handle allocated by shim given back
                               given to QCSI for reference. 

@retval    QMI_CSI_CB_NO_ERR        Client Connect was handled with success 
@retval    QMI_CSI_CB_CONN_REFUSED  If client connect cannot be accepted
@retval    QMI_CSI_CB_INTERNAL_ERR  For all other errors

@warning
  A new client handle is accepted only if it is not present in the client 
  queue.
*//* ------------------------------------------------------------------------*/
static qmi_csi_cb_error locQmiShimClientConnectCb
(
  void                     *p_ClientHandle,
  void                     *pz_ServiceCookie,
  void                    **pz_ConHandle
)

{
  locQmiShimCltInfoStructT *pz_CltInfo;
  qmi_csi_cb_error e_RetVal = QMI_CSI_CB_INTERNAL_ERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if the Client Handle is Non-Null, and is not a duplicate that 
     has already been used */
  if ( ( NULL != p_ClientHandle ) &&
       ( pz_ServiceCookie == &z_LocQmiShim ) &&
       ( NULL == q_linear_search( &z_LocQmiShim.z_ClientQ,
                                   locQmiShimCompareQmiClientHandle,
                                   p_ClientHandle ) ) )
  {
    pz_CltInfo = 
       (locQmiShimCltInfoStructT*) os_MemAlloc( 
                           sizeof(locQmiShimCltInfoStructT), 
                           OS_MEM_SCOPE_TASK );
    
    if ( pz_CltInfo != NULL ) 
    {
      /* Memset to "0" for first use, it might have garbage and cause 
         trouble */
      memset ( pz_CltInfo,
               0,
               sizeof(*pz_CltInfo) );
      
      /* Initialize the link, return value is a pointer to the link, 
         not needed */
      (void) q_link ( pz_CltInfo, &pz_CltInfo->z_Link );

      /* Store the client handle allocated by QCSI */
      pz_CltInfo->p_QmiClientHandle = p_ClientHandle;

      /* explicityly initialize the client as non-batching client */
      pz_CltInfo->isBatchingClient = FALSE;
      
      /* Trigger Loc Open with a IOCTL Event Registration Mask, Event 
         Registration Mask is expected to be updated by client later on */
      pz_CltInfo->z_LocClientHandle = loc_open2( locQmiShimEventCb );
      
      if ( pz_CltInfo->z_LocClientHandle == LOC_CLIENT_HANDLE_INVALID )
      {
        SHIM_MSG_ERROR( " Could not accept a new client loc_open() ",
                        0, 0, 0 );

        /* Free the memory in case of error */
        os_MemFree( (void**) &pz_CltInfo );

        /* Connection is refused because of Loc-Api error */
        e_RetVal = QMI_CSI_CB_CONN_REFUSED;
      }
      else
      {
        /* Add it to the queue of clients */
        q_put( &z_LocQmiShim.z_ClientQ, 
               &pz_CltInfo->z_Link );

        /* Give the Client Info as a handle to the QCSI Framework */
        (*pz_ConHandle) = pz_CltInfo;
        
        e_RetVal = QMI_CSI_CB_NO_ERR;
      }
    } /* if pz_CltInfo == NULL */
    else
    {
      SHIM_MSG_ERROR( " Could not allocate memory for a new client",
                      0, 0, 0 );
    }
  }
  else
  {
    SHIM_MSG_ERROR( " Invalid input parameters rejecting the connect ",
                      0, 0, 0 );
  }
  return e_RetVal;
}



/* -----------------------------------------------------------------------*//**
@brief
  Callback function provided to QCSI, which notifies the shim layer of an  
  existing client disconnecting. At this point loc-close is called to 
  release the client handle with loc-api
  
@param[in]   pz_ConHandle     QMI-Handle of client that is Disconnecting. 
@param[in]   pz_ServiceCookie  Handle of service, as provided by SHIM layer to
                               QCSI during registration of service. 

*//* ------------------------------------------------------------------------*/
static void  locQmiShimClientDisconnectCb
(
  void                      *pz_ConHandle,
  void                      *pz_ServiceCookie
)
{
  locQmiShimCltInfoStructT *pz_CltInfo = NULL;
  SHIM_MSG_HIGH("locQmiShimClientDisconnectCb : client disconnect, conn handle %p",pz_ConHandle, 0, 0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL != pz_ConHandle )
  {
    /* Find the Connection Handle in the queue */
    pz_CltInfo = 
         (locQmiShimCltInfoStructT*) q_linear_search( &z_LocQmiShim.z_ClientQ,
                                        locQmiShimCompareConnectionInfoHandle,
                                        pz_ConHandle );
    if ( NULL != pz_CltInfo )
    {

      SHIM_MSG_HIGH("locQmiShimClientDisconnectCb : client info %p, loc client handle %d",
		pz_CltInfo, pz_CltInfo->z_LocClientHandle, 0);

      /* stop any batching requests from this client */
      /* check if this is batching client and do cleanup */
	  if (TRUE == pz_CltInfo->isBatchingClient)
      {
        loc_client_info_s_type *const client_ptr = 
		  loc_client_find_handle(pz_CltInfo->z_LocClientHandle);

        if( NULL != client_ptr)
        {
          // force stop all AON requests
		  locAonForceStopAll( pz_CltInfo->z_LocClientHandle);

          /* free the buffer and reset client info */
          (void)loc_batching_free_and_reset(client_ptr);

		  pz_CltInfo->isBatchingClient = FALSE;
          SHIM_MSG_HIGH("locQmiShimClientDisconnectCb : cleaned up batching state", 0, 0, 0);
        }

    }

        /* If the client registered for Geofence indication, purge
        geofences. ASSUMPTION: Only one client shall register for
        geofences and when this client deregisters or is killed, 
        all geofences in the system shall be deleted */
      if(( pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION_V02) ||
         ( pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_BREACH_NOTIFICATION_V02))
      {
        /* no error checking done, errors are logged inside the
        function */
        locGeofencePurge();
      }

	  if ( LOC_API_SUCCESS != loc_close(pz_CltInfo->z_LocClientHandle) )
      {
        SHIM_MSG_ERROR("loc_close returned error",0, 0, 0);
      }

      /* Remove from Queue */
      if ( TRUE != q_delete_ext( &z_LocQmiShim.z_ClientQ,
                            &pz_CltInfo->z_Link ) )
      {
        SHIM_MSG_ERROR("Could not delete the client info from queue ",0, 0, 0);
      }

      /* Free the memory in case of error */
      os_MemFree( (void**) &pz_CltInfo );
      
      /* nothing extra to be done to mark isBatchingClient */
    }
  }
}


/* -----------------------------------------------------------------------*//**
@brief
   This callback is invoked when the infrastructure receives an incoming 
   message. The infrastructure decodes the data and gives it to the services 

@param[in]  pz_ConHandle           Service handle used by the infrastructure
                                   to communicate to each individual service.
@param[in]  z_ReqHandle            Opaque handle provided by the infrastructure
                                   to specify a particular transaction and
                                   message.

@param[in]  i_MsgId                Message Id pertaining to a particular
                                   message. 
@param[in]  pz_Msg                 C struct with the decoded data.
@param[in]  i_MsgLen               Length of the c struct.                                   
@param[in]  pz_ServiceCookie         Service specific data.Service cookie is 
                                   registered with the infrastructure during 
                                   service registration(qmi_csi_register).                     


@retval    QMI_CSI_NO_ERR          Success
@retval    QMI_CSI_CB_INTERNAL_ERR Error, Could not handle the message
*//* ------------------------------------------------------------------------*/
static qmi_csi_cb_error locQmiShimHandleRequestCb
(
  void                    *pz_ConHandle,
  qmi_req_handle           z_ReqHandle,
  uint32                   q_MsgId,
  void                    *pz_Msg,
  uint32                   q_MsgLen,
  void                    *pz_ServiceCookie
  )
{
  locQmiShimMsgInfoStructT        z_MsgInfo;
  qmiLocGenRespMsgT_v02           z_QmiLocResp;
  qmi_csi_cb_error                rc = QMI_CSI_CB_INTERNAL_ERR;
  boolean                         v_RetVal = FALSE;
  locQmiShimCltInfoStructT*       pz_CltInfo = NULL;
  qmi_error_type_v01              e_QmiErr = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NULL pz_ConHandle is handled */
  pz_CltInfo = q_linear_search(  &z_LocQmiShim.z_ClientQ,
                                 locQmiShimCompareConnectionInfoHandle,
                                 pz_ConHandle );

  SHIM_MSG_MED("locQmiShimHandleRequestCb: received QMI MsgID:0x00%x pz_CltInfo 0x%x z_ReqHandle 0x%x",q_MsgId, pz_CltInfo, z_ReqHandle);
  SHIM_MSG_MED("Received q_MsgLen:%d pz_Msg 0x%x pz_ServiceCookie 0x%x",q_MsgLen, pz_Msg, pz_ServiceCookie);
  if ( ( NULL != pz_CltInfo ) &&
       ( NULL != z_ReqHandle ) &&
       ( ( ( 0 == q_MsgLen ) && ( NULL == pz_Msg ) ) ||
         ( ( 0 < q_MsgLen ) && ( NULL != pz_Msg ) ) ) &&
       ( pz_ServiceCookie == &z_LocQmiShim ) )
  {  
    z_MsgInfo.q_Id = q_MsgId;
    z_MsgInfo.p_Msg = pz_Msg;
    z_MsgInfo.q_MsgLen = q_MsgLen;
    
    /* completely handle all AON Requests here. Send immediate Resp, send Ind after processing */

    if ( QMI_LOC_GET_BATCH_SIZE_REQ_V02 == q_MsgId )
    {
      v_RetVal = locGetBatchSizeReq(pz_CltInfo,
                                    z_ReqHandle,
                                    &z_MsgInfo);

      rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
      return rc;
    }
    else if ( QMI_LOC_READ_FROM_BATCH_REQ_V02 == q_MsgId )
    {
      v_RetVal = locReadFromBatchReq(pz_CltInfo,
                                     z_ReqHandle,
                                     &z_MsgInfo);

      rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
      return rc;

    }
    else if ( QMI_LOC_RELEASE_BATCH_REQ_V02 == q_MsgId )
    {
      v_RetVal = locReleaseBatchReq(pz_CltInfo,
                                    z_ReqHandle,
                                    &z_MsgInfo);

      rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
      return rc;
    }
    else if ( QMI_LOC_STOP_BATCHING_REQ_V02 == q_MsgId )
    {
      v_RetVal = locStopBatchingReq(pz_CltInfo,
                                    z_ReqHandle,
                                    &z_MsgInfo);

      rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
      return rc;

    }
    /* If it is the Start Batching Request, check the batching parameters inputed by client. */
    else if ( (QMI_LOC_START_BATCHING_REQ_V02 == q_MsgId) )
    {
      v_RetVal = locStartBatchingReq(pz_CltInfo,
                                     z_ReqHandle,
                                     &z_MsgInfo);

      SHIM_MSG_MED("locQmiShimHandleRequestCb: "
                   "locStartBatchingReq returned %d", v_RetVal, 0, 0);
      rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
      return rc;
    }
    else if ( QMI_LOC_START_DBT_REQ_V02 == q_MsgId )
    {
      v_RetVal = locStartDbtReq(pz_CltInfo,
                                z_ReqHandle,
                                &z_MsgInfo);

      rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
      return rc;
    }
    else if ( QMI_LOC_STOP_DBT_REQ_V02 == q_MsgId )
    {
      v_RetVal = locStopDbtReq(pz_CltInfo,
                               z_ReqHandle,
                               &z_MsgInfo);

      rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
      return rc;
    }
    else if ( QMI_LOC_QUERY_AON_CONFIG_REQ_V02 == q_MsgId )
    {
      v_RetVal = locQueryAonConfigReq(pz_CltInfo,
                                      z_ReqHandle,
                                      &z_MsgInfo);

      rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
      return rc;
    }

    /* handle all other Requests here */
    switch (q_MsgId)
    {
      /* Multiple Case Statements handled by the same function */
       case QMI_LOC_SET_ENGINE_LOCK_REQ_V02:
       case QMI_LOC_GET_ENGINE_LOCK_REQ_V02:
       case QMI_LOC_SET_NMEA_TYPES_REQ_V02:
       case QMI_LOC_DELETE_ASSIST_DATA_REQ_V02:
       case QMI_LOC_NI_USER_RESPONSE_REQ_V02:
       case QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02:
       case QMI_LOC_SET_OPERATION_MODE_REQ_V02:
       case QMI_LOC_GET_OPERATION_MODE_REQ_V02:
       case QMI_LOC_SET_SERVER_REQ_V02:
       case QMI_LOC_GET_SERVER_REQ_V02:
       case QMI_LOC_INJECT_POSITION_REQ_V02:
       case QMI_LOC_INJECT_UTC_TIME_REQ_V02:
       case QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02:
       case QMI_LOC_START_REQ_V02:
       case QMI_LOC_REG_EVENTS_REQ_V02:
       case QMI_LOC_GET_NMEA_TYPES_REQ_V02:
       case QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02:
       case QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02:
       case QMI_LOC_GET_FIX_CRITERIA_REQ_V02:
       case QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02:
       case QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02:
#if 0 /*Not supported*/
       case QMI_LOC_SET_SBAS_CONFIG_REQ_V02:
       case QMI_LOC_GET_SBAS_CONFIG_REQ_V02:
#endif
       case QMI_LOC_SET_LOW_POWER_MODE_REQ_V02:
       case QMI_LOC_GET_LOW_POWER_MODE_REQ_V02:
       case QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02:
       case QMI_LOC_INJECT_SUPL_CERTIFICATE_REQ_V02:
       case QMI_LOC_DELETE_SUPL_CERTIFICATE_REQ_V02:
       case QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
       case QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
       case QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02:
       case QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ_V02:
       case QMI_LOC_INJECT_GSM_CELL_INFO_REQ_V02:
       case QMI_LOC_INJECT_NETWORK_INITIATED_MESSAGE_REQ_V02:
       case QMI_LOC_WWAN_OUT_OF_SERVICE_NOTIFICATION_REQ_V02:
       case QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_REQ_V02:
       case QMI_LOC_SET_XTRA_VERSION_CHECK_REQ_V02:
       case QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_V02:
       case QMI_LOC_SECURE_GET_AVAILABLE_POSITION_REQ_V02:
       case QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02:
         v_RetVal = locQmiShimIoctlHdlr(pz_CltInfo,
                                        &z_MsgInfo,
                                        &e_QmiErr);
         break;

       case QMI_LOC_INJECT_WIFI_POSITION_REQ_V02:
       case QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02:
       case QMI_LOC_NOTIFY_WIFI_ATTACHMENT_STATUS_REQ_V02:
       case QMI_LOC_NOTIFY_WIFI_ENABLED_STATUS_REQ_V02:
       {
         /* Set up global WIFI module structure with QMI LOC specific info */
         v_RetVal = locWifi_ProcessQmiRequest(pz_CltInfo->z_LocClientHandle,
                                             q_MsgId, pz_Msg, q_MsgLen);
         if (FALSE == v_RetVal)
         {
           e_QmiErr = QMI_ERR_MALFORMED_MSG_V01;
         }
       }
       break; 

       case QMI_LOC_STOP_REQ_V02:
         v_RetVal = locQmiShimStopReqHdlr(pz_CltInfo,
                                          &z_MsgInfo,
                                          &e_QmiErr);
         break;


       case QMI_LOC_INFORM_CLIENT_REVISION_REQ_V02:
         v_RetVal = locQmiShimHandleClientRevision(pz_CltInfo,
                                                   &z_MsgInfo,
                                                   &e_QmiErr);
         break;

       case QMI_LOC_GET_SERVICE_REVISION_REQ_V02:
         v_RetVal = locQmiShimHandleGetServRevisionReq_v02(pz_CltInfo,
                                                           &z_MsgInfo,
                                                           &e_QmiErr);
         break;

         /* Multiple Case Statements handled by the same function */
       case QMI_LOC_SET_SPI_STATUS_REQ_V02:
       case QMI_LOC_INJECT_SENSOR_DATA_REQ_V02:
       case QMI_LOC_INJECT_TIME_SYNC_DATA_REQ_V02:
       case QMI_LOC_SET_CRADLE_MOUNT_CONFIG_REQ_V02:
       case QMI_LOC_GET_CRADLE_MOUNT_CONFIG_REQ_V02:
       case QMI_LOC_SET_EXTERNAL_POWER_CONFIG_REQ_V02:
       case QMI_LOC_GET_EXTERNAL_POWER_CONFIG_REQ_V02:
       case QMI_LOC_SET_SENSOR_CONTROL_CONFIG_REQ_V02:
       case QMI_LOC_GET_SENSOR_CONTROL_CONFIG_REQ_V02:
       case QMI_LOC_SET_SENSOR_PROPERTIES_REQ_V02:
       case QMI_LOC_GET_SENSOR_PROPERTIES_REQ_V02:
       case QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02:
       case QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02:
       case QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ_V02:
       case QMI_LOC_DELETE_GEOFENCE_REQ_V02:
       case QMI_LOC_EDIT_GEOFENCE_REQ_V02:
       case QMI_LOC_QUERY_GEOFENCE_REQ_V02:
       case QMI_LOC_PEDOMETER_REPORT_REQ_V02:
       case QMI_LOC_INJECT_MOTION_DATA_REQ_V02:
       case QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_REQ_V02:
       case QMI_LOC_INJECT_VEHICLE_SENSOR_DATA_REQ_V02:
       case QMI_LOC_INJECT_WIFI_AP_DATA_REQ_V02:
       case QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_REQ_V02:
       case QMI_LOC_GDT_UPLOAD_BEGIN_STATUS_REQ_V02:
       case QMI_LOC_GDT_UPLOAD_END_REQ_V02:
       case QMI_LOC_INJECT_TIME_ZONE_INFO_REQ_V02:
       case QMI_LOC_INJECT_APCACHE_DATA_REQ_V02:
       case QMI_LOC_INJECT_APDONOTCACHE_DATA_REQ_V02:
         v_RetVal = locQmiShimRouteMessages(pz_CltInfo,
                                            &z_MsgInfo,
                                            &e_QmiErr);
         break;

       case QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02:
         v_RetVal = locQmiShimHandleSetProtocolConfigReq(
            pz_CltInfo,
            &z_MsgInfo,
            &e_QmiErr);
         break;


       case QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02:
         v_RetVal = locQmiShimHandleGetProtocolConfigReq(
            pz_CltInfo,
            &z_MsgInfo,
            &e_QmiErr);
         break;

       default:
         SHIM_MSG_ERROR("locQmiShimHandleRequestCb: Cannot handle this message: %d",
                        q_MsgId, 0, 0);
         e_QmiErr = QMI_ERR_NOT_SUPPORTED_V01;
         break;
    }

    /* Fill and send the response back to the client */
    
    if ( TRUE == v_RetVal )
    {
      z_QmiLocResp.resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    {
      z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    }
    z_QmiLocResp.resp.error  = e_QmiErr;

    /* Msg Id for Req and Resp are the same, hence sending response to the 
       incoming message id */
    if ( QMI_CSI_NO_ERR != mqcsi_send_rsp ( 
                                pz_CltInfo->p_QmiClientHandle,
                                z_ReqHandle, 
                                q_MsgId,  
                                &z_QmiLocResp, 
                                sizeof(z_QmiLocResp) ) )
    {
      SHIM_MSG_ERROR( "locQmiShimHandleRequestCb: Send QMI_RESP 0x00%x failed",
                      q_MsgId, 0, 0 );
      v_RetVal = FALSE;
    }
    else
    {
      SHIM_MSG_MED( "locQmiShimHandleRequestCb: Send QMI_RESP 0x00%x succeed,error:%d,result:%d ",
                     q_MsgId, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);
    }
  }
  else
  {
    SHIM_MSG_ERROR( "locQmiShimHandleRequestCb: Invalid Connection Info Handle from QCSI",
                    0, 0, 0 );
  }

  rc = ( v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR );
  
  return rc;
}/* handle_req */


/* -----------------------------------------------------------------------*//**
@brief
  Function to process the Serialize Ind message. This message is queued from
  the shim module, when there are indications that need to be sent out.
*//* ------------------------------------------------------------------------*/
static void  locQmiShimProcessSerializeInd
(
  void
)
{
  locQmiShimQueuedIndStructT *pz_Ind = NULL;
  qmi_csi_error               e_QmiErr;
  locQmiShimCltInfoStructT    *pz_CltInfo = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pz_Ind = q_get ( &z_LocQmiShim.z_IndQ );
  
  while ( NULL != pz_Ind )
  {
  
    /* The client may have disconnected so better to check if it is still 
       active  */
    pz_CltInfo = q_linear_search(  &z_LocQmiShim.z_ClientQ,     
                                   locQmiShimCompareConnectionInfoHandle,
                                   pz_Ind->pz_CltInfo );
  
    if ( NULL != pz_CltInfo )
    {
      e_QmiErr = mqcsi_send_ind ( 
                                      pz_CltInfo->p_QmiClientHandle,
                                      pz_Ind->q_IndId,
                                      &pz_Ind->z_IndInfo,
                                      pz_Ind->q_IndLen ) ;
      
      if ( QMI_CSI_NO_ERR != e_QmiErr )
      {
      /* Could not send an event out, some error in QCSI framework */
        SHIM_MSG_ERROR( "locQmiShimProcessSerializeInd: send QMI_IND 0x00%x failed, e_QmiErr:%d",
                        pz_Ind->q_IndId, e_QmiErr, 0 );
        
      }
      else
      {
        SHIM_MSG_MED( "locQmiShimProcessSerializeInd: Send QMI_IND 0x00%x succeed",
                      pz_Ind->q_IndId, 0, 0);
      }
    }
  
    /* Initialize and put the buffer back to the free-Q */
    memset ( pz_Ind, 0, sizeof(*pz_Ind) );
    (void)q_link ( pz_Ind, &pz_Ind->z_Link );
    q_put ( &z_LocQmiShim.z_IndFreeQ, &pz_Ind->z_Link );
  
    /* Get the next item to be processed */
    pz_Ind = q_get ( &z_LocQmiShim.z_IndQ );      
  }

}


/* -----------------------------------------------------------------------*//**
@brief
  Function to process the InitQmiLoc Message. This function checks with SM
  which service is enabled, QMI-LOC/QMI-PDS/etc. and if QMI-LOC is enabled, 
  registers the QMI-LOC service with QCSI.
*//* ------------------------------------------------------------------------*/
static void  locQmiShimProcessInitQmiLoc
(
  void
)
{
#ifndef __GRIFFON__
  qmi_csi_os_params   z_OsParams;
#endif /* __GRIFFON__ */
  qmi_csi_error                rc = QMI_CSI_INTERNAL_ERR;
  qmi_idl_service_object_type  z_QmiLocServiceObject = NULL;
  uint32_t                     q_SvcId;
  pdsm_QMIServiceConfig        qmi_config;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( FALSE == pdsm_read_qmi_services_config(&qmi_config) )
  {
    SHIM_ERR_FATAL("Could not read QMI Config!!", 0, 0, 0);
    return;
  }
  
  if( qmi_config.enableMask & PDSM_QMI_CONFIG_ENABLE_LOC )
  {
    loc_middleware_data.qmi_loc_enabled = TRUE;
  }
  else
  {
    /* Do not register with QMI framework, as QMI-LOC disabled in NV */
    SHIM_MSG_HIGH("QMI-LOC Disabled!!", 0, 0, 0);
    return;
  }
#ifdef __GRIFFON__
  /* start the UDP transport */
  qmi_csi_xport_start(&udp_ops, NULL);

#else /* __GRIFFON__ */
  /* Get the TCB for the task in which initialization of shim was called */
  z_OsParams.tcb = os_Self();
  z_OsParams.sig = LOC_QMI_SHIM_SIGNAL;
#endif /* __GRIFFON__ */

  z_QmiLocServiceObject =  loc_get_service_object_v02();

  if ( QMI_IDL_LIB_NO_ERR == qmi_idl_get_service_id( z_QmiLocServiceObject,
                                     &q_SvcId ) )
  {

    /* Register with QMI SI for logging of supported interfaces */
    qmi_si_register_object( z_QmiLocServiceObject, 
                            0,
                            loc_get_service_impl_v02() );

#ifdef __GRIFFON__
    printf("QMI-LOC trying to register with QCSI\n");
#endif /* __GRIFFON__ */
    /* Register the service with QCSI Framework */
    rc = mqcsi_register (  q_SvcId,
                            LOC_V02_IDL_MAJOR_VERS,
                            LOC_V02_IDL_MINOR_VERS,
                            z_QmiLocServiceObject, 
                            locQmiShimClientConnectCb, 
                            locQmiShimClientDisconnectCb, 
                            locQmiShimHandleRequestCb, 
                            (void*)&z_LocQmiShim, 
#ifdef __GRIFFON__
                            &g_z_OsParams,
#else /* __GRIFFON__ */
                            &z_OsParams, 
#endif /* __GRIFFON__ */
                            &(z_LocQmiShim.p_ServiceHandle));


    if(QMI_CSI_NO_ERR == rc )
    {
      qmi_sap_error sapErr;
      SHIM_MSG_HIGH("QMI-LOC Registered Successfully with QCSI",0,0,0);

      /** Register the service with QSAP*/
      sapErr = qmi_sap_register(z_QmiLocServiceObject,
                                NULL,
                                &(z_LocQmiShim.z_QSAPUserHandle));

      if( QMI_SAP_NO_ERR == sapErr )
      {
        SHIM_MSG_HIGH("QMI-LOC Registered Successfully with QSAP",0,0,0);
      }
      else
      {
        SHIM_MSG_ERROR( "Was not able to register successfully with QSAP Error:%d",
                        sapErr, 0, 0 );
      }


#ifdef __GRIFFON__
      printf("QMI-LOC Registered Successfully with QCSI\n");

      // create a thread to read this UDP socket
      if(OK == UPOS_ThreadCreate(&upos_thread, UPOS_FindTask(NULL),
    		   read_udp_and_signal,1, 1, 8192, "uposThread"))
      {
    	  printf("udp xport_open: created UPOS thread\n");
    	  //Activate the Thread
          UPOS_ThreadActivate(&upos_thread);
      }
      else
      {
    	printf("udp xport_open: Could not create UPOS thread\n");
      }
#endif /* __GRIFFON__ */
    }
    else
    {
#ifdef __GRIFFON__
      printf("Was not able to register successfully with QCSI Error:%d\n",rc);
#endif /* __GRIFFON__ */
      SHIM_MSG_ERROR( "Was not able to register successfully with QCSI Error:%d",
                      rc, 0, 0 );
    }
  }
  else
  {
#ifdef __GRIFFON__
    printf("Was not able to get Service ID: %d\n",rc);
#endif /* __GRIFFON__ */
    SHIM_MSG_ERROR( "Was not able to get Service ID",
                    0, 0, 0 );
  }
}


/*****************************************************************************
 *****************************************************************************
 *
 *                   Externalized Function Definitions
 *
 *****************************************************************************
 * ***************************************************************************/
 
/* -----------------------------------------------------------------------*//**
@brief
 This function needs to be called to initialize the QMI-Loc module. It
 initializes the internal data structures of the module and registers with
 SM to receive a callback letting QMI-Loc know when PDAPI is initialized. 

 No other QMI related initialization, specifically no registration of QMI-Loc
 service with the QMI framework is performed here.

@retval  TRUE  - Successful initialization of the module
@retval  FALSE - Module initialization was not successful

@warning
  This function does not gaurd itself against being called multiple times. 
  Once called it can be called again only if "cleanup" is called. 
*//* ------------------------------------------------------------------------*/
boolean locQmiShimInit 
( 
  void 
)
{
  boolean             v_RetVal = TRUE;
  uint8               u_Cntr; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the Service Context Information */
  memset ( &z_LocQmiShim, 
            0,
            sizeof(z_LocQmiShim ) );


  /* Initialize the queue which will hold all handles of clients and maintain 
     the mapping of QMI-Client-handles and the loc-api-client handles */
  (void) q_init(&z_LocQmiShim.z_ClientQ);

  /* Initialize the queues which would manage the Indications that have to 
     be serialized */
  (void) q_init(&z_LocQmiShim.z_IndFreeQ);
  (void) q_init(&z_LocQmiShim.z_IndQ);

  /* Initialize and add buffers to free Q */
  for ( u_Cntr = 0;
        u_Cntr < LOC_QMI_SHIM_IND_BUFFER_POOL_MAX;
        u_Cntr++ )
  {
    memset ( &z_IndBufferPool[u_Cntr], 
             0, 
             sizeof(z_IndBufferPool[u_Cntr]) );
    
    (void) q_link ( &z_IndBufferPool[u_Cntr], 
                    &z_IndBufferPool[u_Cntr].z_Link );

    /* Add it to the Free Queue of Ind buffers */
    q_put( &z_LocQmiShim.z_IndFreeQ, 
           &z_IndBufferPool[u_Cntr].z_Link );
                    
  }

  if ( FALSE == sm_RegisterApiNotifier(locQmiShimInitNotifierCb, THREAD_ID_LOC_MIDDLEWARE) )
  {
    SHIM_ERR_FATAL("Could not register Callback", 0, 0, 0 );
  }
  
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to reset and disconnect the shim layer from the 
  QMI-Loc Service. After this function call, the loc-middleware task will need 
  to call locQmiShimInit() to be able to communicate over the QMI-framework. 
*//* ------------------------------------------------------------------------*/
void locQmiShimCleanup 
( 
  void 
)
{
  locQmiShimCltInfoStructT *pz_CltInfo = NULL;
  qmi_csi_error                 rc = QMI_CSI_INTERNAL_ERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  Continue Stopping only if we have a valid service handle */
  if ( z_LocQmiShim.p_ServiceHandle != NULL )
  {
    /** Unregister with QSAP */
    if( z_LocQmiShim.z_QSAPUserHandle )
    {
      if(QMI_SAP_NO_ERR != qmi_sap_deregister(z_LocQmiShim.z_QSAPUserHandle))
      {
        SHIM_MSG_ERROR("Could not deregister the service with QSAP",
                       0, 0, 0); 
      }
    }

    /* Unregister with framework, so no more messages arrive */
    rc = mqcsi_unregister (  z_LocQmiShim.p_ServiceHandle );
    if ( rc != QMI_CSI_NO_ERR )
    {
      SHIM_MSG_ERROR( "Could not deregister the service successfully",
                      0, 0, 0); 
    }

     /* Reset to initial values */
    memset ( &z_LocQmiShim.p_ServiceHandle,
             0,
             sizeof(z_LocQmiShim.p_ServiceHandle ) );

    /* For each client call the disconnect callback. The same actions need to 
       be taken. Hence q_check() instead of q_get() */
    pz_CltInfo = 
      ( locQmiShimCltInfoStructT* )q_check(&z_LocQmiShim.z_ClientQ);
    while ( NULL != pz_CltInfo ) 
    {

      /* Trigger the function that handles disconnect client callback, 
         as at this point the same functionality needs to be performed 
         NOTE: Memory is freed inside this call */
      locQmiShimClientDisconnectCb( (void*) pz_CltInfo,
                                    &z_LocQmiShim );

      /* Get the next connection handle. 
         NOTE: it is q_check() not q_get() */
      pz_CltInfo = 
        ( locQmiShimCltInfoStructT* )q_check(&z_LocQmiShim.z_ClientQ);
    }

    if ( 0 != q_cnt(&z_LocQmiShim.z_ClientQ) )
    {
      SHIM_MSG_ERROR( "Cleanup was not proper, still some clients dangling ",
                      0, 0, 0); 
    }

    /* Destroy the queue. It is expected to call Init() again before using this 
       module */
    q_destroy(&z_LocQmiShim.z_ClientQ);

  }
}

/* -----------------------------------------------------------------------*//**
@brief
  This function is called by the Service owner indicating that there is a 
  QMI-Event that is queued up in QCSI that needs to be handled.
*//* ------------------------------------------------------------------------*/
void locQmiShimHandleEvent
( 
  void 
)
{
#ifndef __GRIFFON__
  qmi_csi_os_params   z_OsParams;
#endif /* __GRIFFON__ */
  qmi_csi_error       e_Err;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the TCB for the task in which initialization of shim was called 
     Assumption is that this function will be called in the service provider's
     context, and hence os_Self() should return the service provider's 
     context. If called in someone else's context, qmi_csi_handle_event() 
     should return an error */

#ifndef __GRIFFON__
  z_OsParams.tcb = os_Self();
  z_OsParams.sig = LOC_QMI_SHIM_SIGNAL;
#endif /* __GRIFFON__ */

  SHIM_MSG_HIGH("Handle QMI Event", 0, 0,0);
  e_Err = mqcsi_handle_event ( z_LocQmiShim.p_ServiceHandle,
#ifdef __GRIFFON__
                               &g_z_OsParams
#else /* __GRIFFON__ */
                               &z_OsParams
#endif /* __GRIFFON__ */
                               );

  if ( QMI_CSI_NO_ERR != e_Err )
  {
    SHIM_MSG_ERROR( "Could not handle the event properly Err:%d ",
                    e_Err, 0, 0); 
  }
#ifdef __GRIFFON__

  //Unblock the "select" thread, ok to "select" the UDP socket now 
  UPOS_ThreadActivate(&upos_thread);
#endif  /* __GRIFFON__ */
}

/* -----------------------------------------------------------------------*//**
@brief
  This function is called by the Service owner in the Service owner task 
  context. This will be triggered from the QMI-Shim when QMI-Shim has an 
  indication to be serialized and queues up a OS-IPC to the Service Owner. 
*//* ------------------------------------------------------------------------*/
void locQmiShimProcessIpc 
( 
  loc_cmd_s_type * const pz_Cmd 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch ( pz_Cmd->cmd_type )
  {
    case LOC_CMD_TYPE_SERIALIZE_IND:
    {
      locQmiShimProcessSerializeInd();
    };
    break;


    case LOC_CMD_TYPE_INIT_QMI_LOC:
    {
      locQmiShimProcessInitQmiLoc();
    }
    break;


    default:
      SHIM_MSG_ERROR("Invalid IPC:%d to handle", pz_Cmd->cmd_type, 0, 0 );
  }
}

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the module that intends to send out an 
  indication to the client of the QMI-LOC, Service. 

@return TRUE    The indication was successfully sent over QCSI
@return FALSE   The indication was not successfully sent over QCSI

@warning
  This function assumes that it is called in the context of the service 
  providers task. ( Loc-MW task )
*//* ------------------------------------------------------------------------*/
boolean locQmiShimSendInd
(
  loc_client_handle_type                  z_LocClientHandle,
  const locQmiShimIndInfoStructT         *pz_IndInfo
)
{
  boolean v_RetVal = TRUE;
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;
  qmi_csi_error               e_QmiErr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pz_CltInfo = q_linear_search(  &z_LocQmiShim.z_ClientQ,     
                                 locQmiShimCompareLocApiHandle,
                                 (void*)z_LocClientHandle );
  if ( (  NULL != pz_CltInfo ) &&
       ( NULL != pz_IndInfo ) )
  {
    
    e_QmiErr = mqcsi_send_ind ( 
                                    pz_CltInfo->p_QmiClientHandle,
                                    pz_IndInfo->q_Id,
                                    pz_IndInfo->p_Msg,
                                    pz_IndInfo->q_MsgLen );
    
    if ( QMI_CSI_NO_ERR != e_QmiErr )
    {
      /* Could not send an event out, some error in QCSI framework */
      SHIM_MSG_ERROR( "locQmiShimSendInd: send QMI_IND 0x00%x failed, e_QmiErr:%d",
                        pz_IndInfo->q_Id, 
                        e_QmiErr, 
                        0 );
      v_RetVal = FALSE;
    }
    else
    {
      SHIM_MSG_MED( "locQmiShimSendInd: Send QMI_IND 0x00%x succeed ",
                     pz_IndInfo->q_Id, 0, 0);
    }
  }
  else
  {
    SHIM_MSG_ERROR("Could not find the qmi-client",0,0,0);
    v_RetVal = FALSE;
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the module that intends to broadcast an 
  indication to multiple clients of the QMI-LOC, Service. 

@return TRUE    The indication was successfully sent over QCSI
@return FALSE   The indication was not successfully sent over QCSI

@warning
  This function assumes that it is called in the context of the service 
  providers task. ( Loc-MW task )
*//* ------------------------------------------------------------------------*/
boolean locQmiShimBroadcastInd
(
  const uint64                           t_EventMask,
  const locQmiShimIndInfoStructT * const pz_IndInfo
)
{
  boolean v_RetVal = TRUE;
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;
  uint32 error_code = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL != pz_IndInfo)
  {
    SHIM_MSG_HIGH( "Shim BroadCast Event:%lu, msg_ID:%d, len:%d",
                  (uint32) (t_EventMask & 0x00000000FFFFFFFF), pz_IndInfo->q_Id , pz_IndInfo->q_MsgLen);
    for( pz_CltInfo = q_check(&z_LocQmiShim.z_ClientQ); 
         pz_CltInfo != NULL; 
         pz_CltInfo = q_next(&(z_LocQmiShim.z_ClientQ),&(pz_CltInfo->z_Link)))
    {
      MSG_HIGH("locQmiShimBroadcastInd z_LocClientHandle %d t_EventMask %d %d",
                pz_CltInfo->z_LocClientHandle,
                (uint32) (pz_CltInfo->t_EventMask & 0x00000000FFFFFFFF),
                pz_CltInfo->isBatchingClient);
	    
      if(0 != (t_EventMask & pz_CltInfo->t_EventMask) )
      {
        /* this client has registered for something matching this event 
           (mask) */
        if(TRUE == locQmiShimSendInd ( pz_CltInfo->z_LocClientHandle, 
                                       pz_IndInfo))
        {
          SHIM_MSG_MED( "BroadCast Ind:Clt:%u, Event:%d",
                        pz_CltInfo , t_EventMask,0);
        }
        else
        {
          SHIM_MSG_MED ( "Could Not BroadCast Ind:Clt:%u, Event:%d",
                        pz_CltInfo , t_EventMask,0);
          error_code = 1;
        }
      }
    }
  }
  else
  {
    SHIM_MSG_LOW ( "Could Not BroadCast Ind:Clt:%u, Event:%d",
                  pz_CltInfo , t_EventMask,0);
    error_code = 2;
  }

  if(0 != error_code)
  {
      v_RetVal = FALSE;
      SHIM_MSG_ERROR("locQmiShimIndInfoStructT error: %u",error_code,0,0);
  }
  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Untility function to send Response mesasge to Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the response message
@param[out]  pResp                 Pointer to send the response
@param[in]   resp_value            Response status being sent to control point
@param[in]   e_QmiErr              QMI Error type being sent to control point


@retval    TRUE           Sending Response to control point success
@retval    FALSE          Sending Response to control point fail
*//* ------------------------------------------------------------------------*/
static boolean locBatchingOperationSendResp(locQmiShimCltInfoStructT    * const pz_CltInfo,
                                            qmi_req_handle              z_ReqHandle,
                                            locQmiShimMsgInfoStructT    *pz_MsgInfo,
                                            boolean                     resp_value,
                                            qmi_error_type_v01          e_QmiErr)
{
  boolean v_RetVal = TRUE;
  qmiLocGenRespMsgT_v02           z_QmiLocResp;
  
  memset(&z_QmiLocResp, 0, sizeof(z_QmiLocResp));
  
  /* Fill and send the response back to the client */
  if ( TRUE == resp_value )
  {
    z_QmiLocResp.resp.result = QMI_RESULT_SUCCESS_V01;
    z_QmiLocResp.resp.error = QMI_ERR_NONE_V01;
  }
  else
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error  = e_QmiErr;
  }

  /* Msg Id for Req and Resp are the same, hence sending response to the 
     incoming message id */
  if ( QMI_CSI_NO_ERR != mqcsi_send_rsp(
        pz_CltInfo->p_QmiClientHandle,
        z_ReqHandle,
        pz_MsgInfo->q_Id,
        &z_QmiLocResp,
        sizeof(qmiLocGenRespMsgT_v02)) )
  {
    SHIM_MSG_ERROR("locBatchingOperationSendResp: Send QMI_RESP 0x00%x failed",
                   pz_MsgInfo->q_Id, 0, 0);
    v_RetVal = FALSE;
  }
  else
  {
    SHIM_MSG_MED( "locBatchingOperationSendResp: Send QMI_RESP 0x00%x succeed,error:%d,result:%d ",
                   pz_MsgInfo->q_Id, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);
  }
  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle GetBatchSize Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the get batch size request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locGetBatchSizeReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                  qmi_req_handle              z_ReqHandle,
                                  locQmiShimMsgInfoStructT    *pz_MsgInfo)
{
  qmiLocGetBatchSizeIndMsgT_v02 getBatchSizeInd;
  boolean v_RetVal = TRUE;
  boolean resp_result = TRUE; /* default TRUE */
  qmi_error_type_v01 resp_err = QMI_ERR_NONE_V01; /* default NO Error */

  loc_client_info_s_type * const client_ptr = 
                        loc_client_find_handle (pz_CltInfo->z_LocClientHandle);

  qmiLocGetBatchSizeReqMsgT_v02* pGetBatchSizeReq = pz_MsgInfo->p_Msg;
  
  if ( (NULL == pGetBatchSizeReq) || (NULL == client_ptr) )
  {
    SHIM_MSG_ERROR("locGetBatchSizeReq: NULL param(s)\n",
                  0, 0, 0);

    resp_result = FALSE;
    resp_err = QMI_ERR_INTERNAL_V01;
  }
  else if ( TRUE == client_ptr->batching_info.is_batching_client )
  {
    /* check if memory is already allocated for this client */
    resp_result = FALSE;
    resp_err = QMI_ERR_DEVICE_IN_USE_V01;
  }
  else if ( pGetBatchSizeReq->batchSize <= 0 )
  {
    /* check if batch size requested is valid */
    resp_result = FALSE;
    resp_err = QMI_ERR_INVALID_ARG_V01;
  }
  
  /* send response first */
  v_RetVal = locBatchingOperationSendResp(pz_CltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          resp_result,
                                          resp_err); 
  
  /* process the request and send indication only if RESP is SUCCESS */
  if ( (TRUE == resp_result) && (TRUE == v_RetVal) )
  {
	locQmiShimIndInfoStructT  shimIndInfo;

    memset(&getBatchSizeInd, 0, sizeof(getBatchSizeInd));

    locGetBatchSizeHandleAllocate(client_ptr,
                                  &getBatchSizeInd,
                                  pGetBatchSizeReq->batchSize,
                                  pGetBatchSizeReq->transactionId);

    /* mark the shim client as batching or non-batching client based on the status */
    if (eQMI_LOC_SUCCESS_V02 == getBatchSizeInd.status)
    {
      pz_CltInfo->isBatchingClient = TRUE;
    }
    else
    {
      pz_CltInfo->isBatchingClient = FALSE;
    }
    
	/* send indication */
	shimIndInfo.p_Msg = &getBatchSizeInd;
	shimIndInfo.q_MsgLen = sizeof(getBatchSizeInd);
	shimIndInfo.q_Id = QMI_LOC_GET_BATCH_SIZE_IND_V02;

 	v_RetVal = locQmiShimSendInd(client_ptr->client_handle,
                           &shimIndInfo);
  }

  return v_RetVal; 
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to raise event to control point when the live position is available
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   batching_element      Batched position report sent to control point


@retval    TRUE           event reporting success
@retval    FALSE          event reporting fail
*//* ------------------------------------------------------------------------*/
boolean loc_batching_report_live_position_event(loc_client_info_s_type                 *const client_ptr,
                                                const qmiLocBatchedReportStructT_v02   *batching_element)
{ 
  qmiLocEventLiveBatchedPositionReportIndMsgT_v02 liveBatchedPosReportInd;
  locQmiShimIndInfoStructT shimIndInfo;
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;

  pz_CltInfo = q_linear_search(&z_LocQmiShim.z_ClientQ,
                               locQmiShimCompareLocApiHandle,
                               (void*)client_ptr->client_handle);
  if ( NULL == pz_CltInfo )
  {
    SHIM_MSG_ERROR("loc_batching_report_live_position_event: Could not find the qmi-client",0,0,0);
    return FALSE;
  }

  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_LIVE_BATCHED_POSITION_REPORT_V02 !=
          (pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_LIVE_BATCHED_POSITION_REPORT_V02) )
  {
    SHIM_MSG_MED("Client did not register for this report:%d, suppressing",
                 QMI_LOC_EVENT_MASK_LIVE_BATCHED_POSITION_REPORT_V02,
                 0,
                 0);
    return FALSE; 
  }

  /* reset the meomory */
  memset(&liveBatchedPosReportInd, 0, sizeof(liveBatchedPosReportInd));

  /* fill the position report into the indication */
  liveBatchedPosReportInd.liveBatchedReport = (*batching_element);

  shimIndInfo.p_Msg = &liveBatchedPosReportInd;
  shimIndInfo.q_MsgLen = sizeof(liveBatchedPosReportInd);
  shimIndInfo.q_Id = QMI_LOC_EVENT_LIVE_BATCHED_POSITION_REPORT_IND_V02;

  SHIM_MSG_LOW("loc_batching_report_live_position_event Ind:Clt:%u",
                 QMI_LOC_EVENT_LIVE_BATCHED_POSITION_REPORT_IND_V02, 0, 0);

  return locQmiShimSendInd(client_ptr->client_handle,
                           &shimIndInfo) ;

}


/* -----------------------------------------------------------------------*//**
@brief
  Function to raise event to control point the batch is full
  
@param[in]   client_ptr            Client Info Handle


@retval    TRUE           event reporting success
@retval    FALSE          event reporting fail
*//* ------------------------------------------------------------------------*/
boolean loc_batching_report_batch_full_event(loc_client_info_s_type    *const client_ptr)
{
  qmiLocEventBatchFullIndMsgT_v02 batchFullNotifyInd;
  locQmiShimIndInfoStructT shimIndInfo;
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;

  pz_CltInfo = q_linear_search(&z_LocQmiShim.z_ClientQ,
                               locQmiShimCompareLocApiHandle,
                               (void*)client_ptr->client_handle);
  if ( NULL == pz_CltInfo )
  {
    SHIM_MSG_ERROR("loc_batching_report_batch_full_event: Could not find the qmi-client",0,0,0);
    return FALSE;
  }

  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_BATCH_FULL_NOTIFICATION_V02 !=
          (pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_BATCH_FULL_NOTIFICATION_V02) )
  {
    SHIM_MSG_MED("Client did not register for this report:%d, suppressing",
                 QMI_LOC_EVENT_MASK_BATCH_FULL_NOTIFICATION_V02,
                 0,
                 0);
    return FALSE; 
  }

  /* client registered for QMI_LOC_EVENT_MASK_BATCH_FULL_V02 event  */
  batchFullNotifyInd.batchCount = client_ptr->batching_info.max_array_size; /* size of batch to be notified */

  shimIndInfo.p_Msg = &batchFullNotifyInd;
  shimIndInfo.q_MsgLen = sizeof(batchFullNotifyInd);
  shimIndInfo.q_Id = QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND_V02;

  SHIM_MSG_LOW("loc_batching_report_batch_full_event Ind:Clt:%u, Count:%u",
                 QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND_V02, batchFullNotifyInd.batchCount, 0);

  return locQmiShimSendInd(client_ptr->client_handle,
                           &shimIndInfo) ;
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to send read from batch Indication to control point.
  
@param[in]   client_ptr            Client Info Handle 
@param[out]  p_readFromBatchInd    Read From Batch Indication message to control point
@param[in]   request_status        Status from lifo buffer pull operation
@param[in]   reqTransactionId      Transaction Id in request message by control point 

@retval    TRUE           locQmiShimSendInd is success
@retval    FALSE          locQmiShimSendInd is failed
                                  
*//* ------------------------------------------------------------------------*/
static boolean loc_send_read_from_batch_ind(loc_client_info_s_type               *const client_ptr,
                                     qmiLocReadFromBatchIndMsgT_v02       *const p_readFromBatchInd,
                                     boolean                              request_status,
                                     uint32                               reqTransactionId)
{
  locQmiShimIndInfoStructT              shimIndInfo;

  /* invalid read size, return failure */
  if(request_status == FALSE)
  {
    p_readFromBatchInd->status = eQMI_LOC_INVALID_PARAMETER_V02;
  }
  else
  {
    p_readFromBatchInd->status = eQMI_LOC_SUCCESS_V02;
  }

  /* update transaction id in indication - mandatory field */
  p_readFromBatchInd->transactionId = reqTransactionId; 

  shimIndInfo.p_Msg = p_readFromBatchInd;
  shimIndInfo.q_MsgLen = sizeof(*p_readFromBatchInd);
  shimIndInfo.q_Id = QMI_LOC_READ_FROM_BATCH_IND_V02;

  LOC_MSG_MED("loc_send_read_from_batch_ind: calling locQmiShimSendInd", 0, 0, 0);

  /* send indication */
  return locQmiShimSendInd(client_ptr->client_handle,
                           &shimIndInfo);
}


/* -----------------------------------------------------------------------*//**
@brief
  Function to handle ReadFromBatch Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the ReadFromBatch request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locReadFromBatchReq(locQmiShimCltInfoStructT     *const pz_CltInfo,
                                   qmi_req_handle               z_ReqHandle,
                                   locQmiShimMsgInfoStructT     *pz_MsgInfo)
{
  qmiLocReadFromBatchIndMsgT_v02 readFromBatchInd;
  boolean v_RetVal = TRUE;
  boolean ret_val = FALSE;
  boolean resp_result = TRUE; /* default TRUE */
  qmi_error_type_v01 resp_err = QMI_ERR_NONE_V01; /* default NO Error */
  
  loc_client_info_s_type *const client_ptr =
     loc_client_find_handle(pz_CltInfo->z_LocClientHandle);

  qmiLocReadFromBatchReqMsgT_v02 *pReadFromBatchReq = pz_MsgInfo->p_Msg;

  if ( (NULL == pReadFromBatchReq) || (NULL == client_ptr) )
  {
    SHIM_MSG_ERROR("locReadFromBatchReq: NULL param(s)\n",
                  0, 0, 0);

    resp_result = FALSE;
    resp_err = QMI_ERR_INTERNAL_V01;
  }

  /* check if this is really a batching client */
  else if ( FALSE == client_ptr->batching_info.is_batching_client )
  {
    resp_result = FALSE;
    resp_err = QMI_ERR_INVALID_HANDLE_V01;
  }
  
  /* send response first */
  v_RetVal = locBatchingOperationSendResp(pz_CltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          resp_result,
                                          resp_err); 

  /* process the request and send indication only if RESP is SUCCESS */
  if ( (TRUE == resp_result) && (TRUE == v_RetVal) )
  {
    memset(&readFromBatchInd, 0, sizeof(readFromBatchInd));

    ret_val = loc_handle_batching_read(client_ptr, pReadFromBatchReq->numberOfEntries, &readFromBatchInd);

    /* Queue up the indication that needs to be sent */
    v_RetVal = loc_send_read_from_batch_ind(client_ptr, &readFromBatchInd, ret_val, pReadFromBatchReq->transactionId);
  }

  return v_RetVal; 
}

/* -----------------------------------------------------------------------*//**
@brief
  Utility function to send release batch Indication to control point.
  
@param[in]   client_ptr            Client Info Handle 
@param[out]  p_releaseBatchInd     Release Batch Indication message to control point
@param[in]   request_status        Status from buffer free operation
@param[in]   reqTransactionId      Transaction Id in request message by control point 


@retval    TRUE           locQmiShimSendInd is success
@retval    FALSE          locQmiShimSendInd is failed
                                  
*//* ------------------------------------------------------------------------*/
static boolean loc_send_release_batch_ind(loc_client_info_s_type           *const client_ptr,
                                   qmiLocReleaseBatchIndMsgT_v02    *const p_releaseBatchInd,
                                   boolean                          request_status,
                                   uint32                           reqTransactionId)
{
  locQmiShimIndInfoStructT              shimIndInfo;

  /* invalid read size, return failure */
  if(request_status == FALSE)
  {
    p_releaseBatchInd->status = eQMI_LOC_GENERAL_FAILURE_V02;
  }
  else
  {
    p_releaseBatchInd->status = eQMI_LOC_SUCCESS_V02;
  }

  /* update transaction id in indication - mandatory field */
  p_releaseBatchInd->transactionId = reqTransactionId;

  shimIndInfo.p_Msg = p_releaseBatchInd; 
  shimIndInfo.q_MsgLen = sizeof(*p_releaseBatchInd);
  shimIndInfo.q_Id = QMI_LOC_RELEASE_BATCH_IND_V02;

  LOC_MSG_MED("loc_send_release_batch_ind: calling locQmiShimSendInd", 0, 0, 0);

  /* send indication */
  return locQmiShimSendInd(client_ptr->client_handle,
                           &shimIndInfo);
}



/* -----------------------------------------------------------------------*//**
@brief
  Function to handle ReleaseBatch Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the ReleaseBatch request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locReleaseBatchReq(locQmiShimCltInfoStructT      *const pz_CltInfo,
                                  qmi_req_handle                z_ReqHandle,
                                  locQmiShimMsgInfoStructT      *pz_MsgInfo)
{
  qmiLocReleaseBatchIndMsgT_v02 releaseBatchInd;
  boolean v_RetVal = TRUE;
  boolean ret_val = FALSE;
  boolean resp_result = TRUE; /* default TRUE */
  qmi_error_type_v01 resp_err = QMI_ERR_NONE_V01; /* default NO Error */
  
  loc_client_info_s_type *const client_ptr =
     loc_client_find_handle(pz_CltInfo->z_LocClientHandle);

  qmiLocReleaseBatchReqMsgT_v02 *pReleaseBatchReq = pz_MsgInfo->p_Msg;

  if ( (NULL == pReleaseBatchReq) || (NULL == client_ptr) )
  {
    SHIM_MSG_ERROR("locReleaseBatchReq: NULL param(s)\n",
                  0, 0, 0);

    resp_result = FALSE;
    resp_err = QMI_ERR_INTERNAL_V01;
  }

  /* check if memory is already allocated for this client */
  else if ( FALSE == client_ptr->batching_info.is_batching_client )
  {
    resp_result = FALSE;
    resp_err = QMI_ERR_INVALID_HANDLE_V01;
  }
  else if(TRUE == locAonCheckIfLocClientPresent(pz_CltInfo->z_LocClientHandle, eLOC_AON_REQ_BATCH))
  {
    SHIM_MSG_ERROR("locReleaseBatchReq: call QMI_LOC_STOP_BATCHING before RELEASE_BATCHING\n",
                  0, 0, 0);
    resp_result = FALSE;
    resp_err = QMI_ERR_INVALID_OPERATION_V01;
  }

  /* send response first */
  v_RetVal = locBatchingOperationSendResp(pz_CltInfo,
                                          z_ReqHandle,
                                          pz_MsgInfo,
                                          resp_result,
                                          resp_err);

  /* process the request and send indication only if RESP is SUCCESS */
  if ( (TRUE == resp_result) && (TRUE == v_RetVal) )
  {
    memset(&releaseBatchInd, 0, sizeof(releaseBatchInd));
    
    /* free the buffer and reset client info */
    ret_val = loc_batching_free_and_reset(client_ptr);
    
    if ( TRUE == ret_val )
    {
      /* mark the shim client as non-batching client */
      pz_CltInfo->isBatchingClient = FALSE;
    }

    /* Queue up the indication that needs to be sent */
    v_RetVal = loc_send_release_batch_ind(client_ptr, &releaseBatchInd, ret_val, pReleaseBatchReq->transactionId);
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to handle StartBatching Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   pStartBatchingReq     Request message from the control point


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locStartBatchingReq(locQmiShimCltInfoStructT     *const pz_CltInfo,
                                   qmi_req_handle               z_ReqHandle,
                                   locQmiShimMsgInfoStructT     *pz_MsgInfo)
{
  boolean v_RetVal = FALSE; 
  qmiLocGenRespMsgT_v02  z_QmiLocResp;
  qmiLocStartBatchingReqMsgT_v02* pStartBatchingReq = NULL;
  
  loc_client_info_s_type * const client_ptr = 
                        loc_client_find_handle (pz_CltInfo->z_LocClientHandle);

  if(NULL == pz_MsgInfo)
  {
   SHIM_MSG_ERROR("locStartBatchingReq: Error invalid invalid msg info 0x%x",
                    pz_MsgInfo, 0, 0);
   return v_RetVal;
  }
  else if(NULL == client_ptr)
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error =  QMI_ERR_INTERNAL_V01;
    SHIM_MSG_ERROR("locStartBatchingReq: Error invalid client info 0x%x",
                    client_ptr, 0,0);
  }

  else if ( TRUE != client_ptr->batching_info.is_batching_client )
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error =  QMI_ERR_INVALID_OPERATION_V01;
    SHIM_MSG_ERROR("locStartBatchingReq: Not a batching client",
                    0,0,0);
  }

 else
  {
	pStartBatchingReq = (qmiLocStartBatchingReqMsgT_v02*)pz_MsgInfo->p_Msg;
    /* forward the request to AoN*/
    locAonBatchStartReq(pz_CltInfo->z_LocClientHandle,
                        pStartBatchingReq,
                        &z_QmiLocResp);
  }

  /* send response */
  v_RetVal = (QMI_CSI_NO_ERR == 
              mqcsi_send_rsp( pz_CltInfo->p_QmiClientHandle,
                              z_ReqHandle,
                              pz_MsgInfo->q_Id,
                              &z_QmiLocResp,
                              sizeof(qmiLocGenRespMsgT_v02)) );

  if(TRUE != v_RetVal)
  {
    SHIM_MSG_ERROR("locStartBatchingReq: Send QMI_RESP 0x00%x failed",
                    0,0,0);
  }
  else
  {
    SHIM_MSG_MED( "locStartBatchingReq: Send QMI_RESP 0x00%x succeed,error:%d,result:%d ",
                   pz_MsgInfo->q_Id, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to handle StopBatching Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopBatching request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locStopBatchingReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                  qmi_req_handle              z_ReqHandle,
                                  locQmiShimMsgInfoStructT    *pz_MsgInfo)
{
  boolean v_RetVal = FALSE;
  qmiLocGenRespMsgT_v02  z_QmiLocResp;
  qmiLocStopBatchingReqMsgT_v02 *pStopBatchingReq = 
    (qmiLocStopBatchingReqMsgT_v02 *)pz_MsgInfo->p_Msg;

  loc_client_info_s_type *const client_ptr =
     loc_client_find_handle(pz_CltInfo->z_LocClientHandle);

  if (NULL == client_ptr) 
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error = QMI_ERR_INTERNAL_V01;
    SHIM_MSG_ERROR("locStopBatchingReq: could not find client info",
                   0, 0, 0);
  }
  /* check if this client is a batching client */
  else if ( FALSE == client_ptr->batching_info.is_batching_client )
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error = QMI_ERR_INVALID_OPERATION_V01;
    SHIM_MSG_ERROR("locStopBatchingReq: NOT a batching client", 0, 0, 0);
  }
  else
  {
     /* forward the request to AoN*/
    locAonBatchStopReq(pz_CltInfo->z_LocClientHandle,
                       pStopBatchingReq,
                       &z_QmiLocResp);
  }

   /* send response */
  v_RetVal = (QMI_CSI_NO_ERR == 
              mqcsi_send_rsp( pz_CltInfo->p_QmiClientHandle,
                              z_ReqHandle,
                              pz_MsgInfo->q_Id,
                              &z_QmiLocResp,
                              sizeof(qmiLocGenRespMsgT_v02)) );

  /* Send a success indication if the response was a success
     if the the response was a failure no need to send an indication*/
  if ( (TRUE == v_RetVal) && 
       (QMI_RESULT_SUCCESS_V01 == z_QmiLocResp.resp.result ))
  {
    locQmiShimIndInfoStructT  shimIndInfo;
    qmiLocStopBatchingIndMsgT_v02 stopBatchingInd;
    stopBatchingInd.status = eQMI_LOC_SUCCESS_V02;
    stopBatchingInd.transactionId = pStopBatchingReq->transactionId;
    stopBatchingInd.requestId_valid = pStopBatchingReq->requestId_valid;
    stopBatchingInd.requestId = pStopBatchingReq->requestId;
    shimIndInfo.p_Msg = &stopBatchingInd;
    shimIndInfo.q_MsgLen = sizeof(stopBatchingInd);
    shimIndInfo.q_Id = QMI_LOC_STOP_BATCHING_IND_V02;

    SHIM_MSG_MED("locStopBatchingReq: Send QMI_RESP 0x00%x succeed,error:%d,result:%d ",
                    pz_MsgInfo->q_Id,z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);
    /* send indication */
    v_RetVal = locQmiShimSendInd(pz_CltInfo->z_LocClientHandle,
                                 &shimIndInfo);
    if ( FALSE == v_RetVal )
    {
      SHIM_MSG_ERROR("locStopBatchingReq: error sending indication",
                    0,0,0);
    }
  }
  else if(TRUE != v_RetVal)
  {
    SHIM_MSG_ERROR("locStopBatchingReq: Send QMI_RESP 0x00%x failed",
                    pz_MsgInfo->q_Id,0,0);
  }

  return v_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Function to send start batching indication to the control point 
  
@param[in]   locClientHandle       Loc MW client handle 
@param[in]   status                Indication status
                                   
@retval    TRUE           success
@retval    FALSE          failure
*//* ------------------------------------------------------------------------*/
boolean locSendStartBatchingInd(
  loc_client_handle_type locClientHandle,
  uint32 locReqId,
  qmiLocStatusEnumT_v02 status)
{
  boolean v_RetVal = FALSE;
  loc_client_info_s_type *client_ptr = loc_client_find_handle(locClientHandle);
  if(NULL != client_ptr)
  {
	locQmiShimIndInfoStructT  shimIndInfo;
    qmiLocStartBatchingIndMsgT_v02 startBatchingInd;
    startBatchingInd.status = status;
	if( locReqId != 0)
	{
	  startBatchingInd.requestId_valid = TRUE;
	  startBatchingInd.requestId = locReqId;
	}
    shimIndInfo.p_Msg = &startBatchingInd;
    shimIndInfo.q_MsgLen = sizeof(startBatchingInd);
    shimIndInfo.q_Id = QMI_LOC_START_BATCHING_IND_V02;
	
    /* send indication */
    v_RetVal = locQmiShimSendInd(locClientHandle, &shimIndInfo);
  }
  else
  {
    SHIM_MSG_ERROR("locSendStartBatchingInd: no client info for client: %d, reqId: %d",
                    locClientHandle,locReqId,0);
  }
  return v_RetVal;
}
                               
/* -----------------------------------------------------------------------*//**
@brief
  Function to handle a Start DBT session from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StartDBT request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locStartDbtReq(locQmiShimCltInfoStructT     *const pz_CltInfo,
                              qmi_req_handle               z_ReqHandle,
                              locQmiShimMsgInfoStructT     *pz_MsgInfo)
{
  boolean v_RetVal = FALSE; 
  qmiLocGenRespMsgT_v02  z_QmiLocResp;
  qmiLocStartDbtReqMsgT_v02* pStartDbtReq = NULL;

  if( NULL == pz_MsgInfo)
  {
	LOC_MSG_ERROR("locStartDbtReq: NULL QmiShimMsgInfo param",0,0,0);
	return FALSE;
  }

  pStartDbtReq = (qmiLocStartDbtReqMsgT_v02*)pz_MsgInfo->p_Msg;
  /* forward the request to AoN*/
  locAonDbtStartReq(pz_CltInfo->z_LocClientHandle,
                        pStartDbtReq,
                        &z_QmiLocResp);  

  /* send response */
  v_RetVal = (QMI_CSI_NO_ERR == 
              mqcsi_send_rsp( pz_CltInfo->p_QmiClientHandle,
                              z_ReqHandle,
                              pz_MsgInfo->q_Id,
                              &z_QmiLocResp,
                              sizeof(qmiLocGenRespMsgT_v02)) );

  if(TRUE != v_RetVal)
  {
    SHIM_MSG_ERROR( "locStartDbtReq: Send QMI_RESP 0x00%x failed",
                     pz_MsgInfo->q_Id, 0, 0 );
  }
  else
  {
    SHIM_MSG_MED( "locStartDbtReq: Send QMI_RESP 0x00%x succeed,error:%d,result:%d ",
                   pz_MsgInfo->q_Id, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to handle Stop a DBT session from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopDBT request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locStopDbtReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                  qmi_req_handle              z_ReqHandle,
                                  locQmiShimMsgInfoStructT    *pz_MsgInfo)
{
  boolean v_RetVal = FALSE;
  qmiLocGenRespMsgT_v02  z_QmiLocResp;
  qmiLocStopDbtReqMsgT_v02 *pStopDbtReq = NULL;

  if( NULL == pz_MsgInfo)
  {
	LOC_MSG_ERROR("locAonDbtStopReq: NULL QmiShimMsgInfo param",0,0,0);
	return FALSE;
  }

   pStopDbtReq = (qmiLocStopDbtReqMsgT_v02 *)pz_MsgInfo->p_Msg;

  /* forward the request to AoN*/
  locAonDbtStopReq(pz_CltInfo->z_LocClientHandle,
                       pStopDbtReq,
                       &z_QmiLocResp);  

   /* send response */
  v_RetVal = (QMI_CSI_NO_ERR == 
              mqcsi_send_rsp( pz_CltInfo->p_QmiClientHandle,
                              z_ReqHandle,
                              pz_MsgInfo->q_Id,
                              &z_QmiLocResp,
                              sizeof(qmiLocGenRespMsgT_v02)) );

  /* Send a success indication if the response was a success
     if the the response was a failure no need to send an indication*/
  if ( (TRUE == v_RetVal) && 
       (QMI_RESULT_SUCCESS_V01 == z_QmiLocResp.resp.result ))
  {
    locQmiShimIndInfoStructT  shimIndInfo;
    qmiLocStopDbtIndMsgT_v02 stopDbtInd;
    stopDbtInd.status = eQMI_LOC_SUCCESS_V02;
    stopDbtInd.reqId_valid = TRUE;
    stopDbtInd.reqId = pStopDbtReq->reqId;
    shimIndInfo.p_Msg = &stopDbtInd;
    shimIndInfo.q_MsgLen = sizeof(stopDbtInd);
    shimIndInfo.q_Id = QMI_LOC_STOP_DBT_IND_V02;

    SHIM_MSG_MED("locAonDbtStopReq: Send QMI_RESP 0x00%x succeed,error:%d,result:%d",
                    pz_MsgInfo->q_Id,z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);

    /* send indication */
    v_RetVal = locQmiShimSendInd(pz_CltInfo->z_LocClientHandle,
                                 &shimIndInfo);
    if ( TRUE != v_RetVal )
    {
      SHIM_MSG_ERROR("locAonDbtStopReq: error sending indication",
                    0,0,0);
    }
    
  }
  else if(TRUE != v_RetVal)
  {
    SHIM_MSG_ERROR("locAonDbtStopReq: Send QMI_RESP 0x00%x failed",
                    pz_MsgInfo->q_Id,0,0);
  }

  return v_RetVal;
}
/* -----------------------------------------------------------------------*//**
@brief
  Function to handle QueryAonConfig Request from Control point 
  
@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                   to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the StopBatching request


@retval    TRUE           request command handle success
@retval    FALSE          request command handle failure
*//* ------------------------------------------------------------------------*/
static boolean locQueryAonConfigReq(locQmiShimCltInfoStructT    *const pz_CltInfo,
                                    qmi_req_handle              z_ReqHandle,
                                    locQmiShimMsgInfoStructT    *pz_MsgInfo)
{
  boolean v_RetVal = FALSE;
  
  qmiLocGenRespMsgT_v02  z_QmiLocResp = {0};
 
  qmiLocQueryAonConfigReqMsgT_v02 *pQueryAonConfigReq = 
    (qmiLocQueryAonConfigReqMsgT_v02 *)pz_MsgInfo->p_Msg;

  loc_client_info_s_type *const client_ptr =
     loc_client_find_handle(pz_CltInfo->z_LocClientHandle);
  
  SHIM_MSG_MED("locQueryAonConfigReq : got request to query Aon Config",0,0,0);

  if (NULL == client_ptr) 
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error = QMI_ERR_INTERNAL_V01;
    SHIM_MSG_ERROR("locQueryAonConfigReq: could not find client info",
                   0, 0, 0);
  }
 
  /* send response */
  v_RetVal = (QMI_CSI_NO_ERR == 
              mqcsi_send_rsp( pz_CltInfo->p_QmiClientHandle,
                              z_ReqHandle,
                              pz_MsgInfo->q_Id,
                              &z_QmiLocResp,
                              sizeof(qmiLocGenRespMsgT_v02)) );

  /* Send a success indication if the response was a success
     if the the response was a failure no need to send an indication*/
  if ( (TRUE == v_RetVal) && 
       (QMI_RESULT_SUCCESS_V01 == z_QmiLocResp.resp.result ))
  {
    locQmiShimIndInfoStructT  shimIndInfo;
    qmiLocQueryAonConfigIndMsgT_v02 aonConfigInd;
    
    aonConfigInd.status = eQMI_LOC_SUCCESS_V02;
    aonConfigInd.transactionId_valid = TRUE;
    aonConfigInd.transactionId = pQueryAonConfigReq->transactionId;
    aonConfigInd.aonCapability_valid = TRUE;
    aonConfigInd.aonCapability = (
        QMI_LOC_MASK_AON_AUTO_BATCHING_SUPPORTED_V02 |
        QMI_LOC_MASK_AON_DISTANCE_BASED_BATCHING_SUPPORTED_V02 |
        QMI_LOC_MASK_AON_TIME_BASED_BATCHING_SUPPORTED_V02 |
        QMI_LOC_MASK_AON_DISTANCE_BASED_TRACKING_SUPPORTED_V02 |
      QMI_LOC_MASK_AON_UPDATE_TBF_SUPPORTED_V02);

    SHIM_MSG_MED("locQueryAonConfigReq : returning Aon Config = %d, transaction id = %d",
                  aonConfigInd.aonCapability ,aonConfigInd.transactionId,0);

    shimIndInfo.p_Msg = &aonConfigInd;
    shimIndInfo.q_MsgLen = sizeof(aonConfigInd);
    shimIndInfo.q_Id = QMI_LOC_QUERY_AON_CONFIG_IND_V02;
    /* send indication */
    v_RetVal = locQmiShimSendInd(pz_CltInfo->z_LocClientHandle,
                                 &shimIndInfo);
    if ( TRUE != v_RetVal )
    {
        SHIM_MSG_ERROR("locQueryAonConfigReq: error sending indication",
                    0,0,0);
    }
  }
  else if(TRUE != v_RetVal)
  {
    SHIM_MSG_ERROR("locQueryAonConfigReq: error sending response",
                    0,0,0);
  }

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to send batching status event indication to batching clients that
  registered for it
  
@param[in]   locClientHandle       Loc MW client handle 
@param[in]   status                Indication status
                                   
@retval    TRUE           success
@retval    FALSE          failure
*//* ------------------------------------------------------------------------*/
boolean locSendBatchingStatusInd(
  loc_client_handle_type locClientHandle,
  qmiLocBatchingStatusEnumT_v02 status)
{
  qmiLocEventBatchingStatusIndMsgT_v02 batchStatusInd = {0};
  locQmiShimIndInfoStructT shimIndInfo = {0};
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;

  pz_CltInfo = q_linear_search(&z_LocQmiShim.z_ClientQ,
                               locQmiShimCompareLocApiHandle,
                               (void*)locClientHandle);
  if ( NULL == pz_CltInfo )
  {
    SHIM_MSG_ERROR("locSendBatchingStatusInd: Could not find the qmi-client",0,0,0);
    return FALSE;
  }

  /* Send report only if the client registered */
  if ( QMI_LOC_EVENT_MASK_BATCHING_STATUS_V02 !=
          (pz_CltInfo->t_EventMask & QMI_LOC_EVENT_MASK_BATCHING_STATUS_V02) )
  {
    SHIM_MSG_MED("locSendBatchingStatusInd: Client did not register for this report:%d, suppressing",
                 QMI_LOC_EVENT_MASK_BATCHING_STATUS_V02,
                 0,
                 0);
    return FALSE; 
  }



  /* fill the position report into the indication */
  batchStatusInd.batchingStatus = status;

  shimIndInfo.p_Msg = &batchStatusInd;
  shimIndInfo.q_MsgLen = sizeof(batchStatusInd);
  shimIndInfo.q_Id = QMI_LOC_EVENT_BATCHING_STATUS_IND_V02;

  SHIM_MSG_LOW("locSendBatchingStatusInd Ind:Clt:%u", QMI_LOC_EVENT_BATCHING_STATUS_IND_V02, 0, 0);

  return locQmiShimSendInd(locClientHandle, &shimIndInfo) ;
}

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC Indication for commands
---------------------------------------------------------------------------*/
boolean locQmi_ProcessInd
(
  loc_client_handle_type       l_ClientHandle,
  uint32                       q_MsgID,
  void*                        p_IndData,
  qmiLocStatusEnumT_v02        e_IndStatus
)
{
   locQmiShimIndInfoStructT  z_IndInfo = {0};
   locQmiShimIndUnionT      *p_IndUnion = (locQmiShimIndUnionT *)loc_calloc(sizeof(locQmiShimIndUnionT));
   locQmiShimCltInfoStructT *pz_CltInfo = NULL;
   boolean                   v_RetVal = FALSE;

   if(NULL == p_IndUnion)
   {
      SHIM_MSG_HIGH("locQmi_ProcessWifiInd: loc_calloc failed", 0,0,0);
      return v_RetVal;
   }

   do
   {
      /* Set the pointer to a union of all possible Indication types */
      z_IndInfo.p_Msg = p_IndUnion;

      SHIM_MSG_HIGH("locQmi_ProcessWifiInd: QMI_LOC Msg=0x00%, ClientHandle=%d, IndStatus=%d", q_MsgID, l_ClientHandle, e_IndStatus);

      pz_CltInfo = (locQmiShimCltInfoStructT *)q_linear_search(&z_LocQmiShim.z_ClientQ, locQmiShimCompareLocApiHandle, (void*)l_ClientHandle);
      if (NULL == pz_CltInfo)
      {
         SHIM_MSG_ERROR("locQmi_ProcessWifiInd: Invalid Client Info for Handle=%d", l_ClientHandle, 0, 0);
         break;
      }

      v_RetVal = TRUE;

      /* Process all WIFI commands here */
      switch (q_MsgID)
      {
         case QMI_LOC_INJECT_WIFI_POSITION_REQ_V02:
         {
            qmiLocInjectWifiPositionIndMsgT_v02* p_InjWifiPosInd =
            (qmiLocInjectWifiPositionIndMsgT_v02*)z_IndInfo.p_Msg;

            z_IndInfo.q_Id             = QMI_LOC_INJECT_WIFI_POSITION_IND_V02;
            z_IndInfo.q_MsgLen         = sizeof(*p_InjWifiPosInd);
            p_InjWifiPosInd->status = e_IndStatus;
            break;
         }

         case QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02:
         {
            qmiLocNotifyWifiStatusIndMsgT_v02* p_NotifyWifiStatusInd =
            (qmiLocNotifyWifiStatusIndMsgT_v02*)z_IndInfo.p_Msg;

            z_IndInfo.q_Id           = QMI_LOC_NOTIFY_WIFI_STATUS_IND_V02;
            z_IndInfo.q_MsgLen       = sizeof(*p_NotifyWifiStatusInd);
            p_NotifyWifiStatusInd->status = e_IndStatus;
            break;
         }

         case QMI_LOC_NOTIFY_WIFI_ATTACHMENT_STATUS_REQ_V02:
         {
            qmiLocNotifyWifiAttachmentStatusIndMsgT_v02* p_NotifyWifiAttachmentInd =
            (qmiLocNotifyWifiAttachmentStatusIndMsgT_v02*)z_IndInfo.p_Msg;

            z_IndInfo.q_Id             = QMI_LOC_NOTIFY_WIFI_ATTACHMENT_STATUS_IND_V02;
            z_IndInfo.q_MsgLen         = sizeof(*p_NotifyWifiAttachmentInd);
            p_NotifyWifiAttachmentInd->status = e_IndStatus;
            break;
         }

         case QMI_LOC_NOTIFY_WIFI_ENABLED_STATUS_REQ_V02:
         {
            qmiLocNotifyWifiEnabledStatusIndMsgT_v02* p_NotifyWifiEnabledInd =
            (qmiLocNotifyWifiEnabledStatusIndMsgT_v02*)z_IndInfo.p_Msg;

            z_IndInfo.q_Id       = QMI_LOC_NOTIFY_WIFI_ENABLED_STATUS_IND_V02;
            z_IndInfo.q_MsgLen   = sizeof(*p_NotifyWifiEnabledInd);
            p_NotifyWifiEnabledInd->status = e_IndStatus;
            break;
         }

         default:
         {
            SHIM_MSG_ERROR("locQmi_ProcessWifiInd: Invalid QmiMsgId=0x%x", q_MsgID, 0, 0);
            v_RetVal = FALSE;
         }
      }
   }while(0);

   if(TRUE == v_RetVal)
   {
      v_RetVal = locQmiShimSendInd(pz_CltInfo->z_LocClientHandle, &z_IndInfo);
   }

   loc_free(p_IndUnion);

   return v_RetVal;
}

