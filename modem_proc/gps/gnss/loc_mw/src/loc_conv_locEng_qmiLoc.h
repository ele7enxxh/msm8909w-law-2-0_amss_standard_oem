/******************************************************************************
  @file:  loc_conv_locEng_qmiLoc.h
  @brief: Function headers for translation between locEng and qmiLoc

  DESCRIPTION
  This module defines the functions that handle the conversions between locEng
  and qmiLoc messages. locEng messages are IDL based and intended to be used
  within the GNSS engine.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2013-2015 QUALCOMM Atheros Incorporated.
  All Rights Reserved. QUALCOMM Atheros Proprietary and Confidential.
  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
04/06/15   sj       LB 2.0 integration
01/30/15   sj       Updated position transalation for DBT 
03/10/14   ah       Added support for Get Avail WWAN Pos
10/22/12   ns       First revision

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_conv_locEng_qmiLoc.h#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/
#ifndef LOC_CONV_LOC_ENG_QMI_LOC_H
#define LOC_CONV_LOC_ENG_QMI_LOC_H

#include "comdef.h" 
#include "location_service_v02.h"
#include "locEng_sm_common_msg_v01.h"
#include "aon_api.h"
#include "loc_api_2.h"


/**  convertLocEngReliability
 
@brief : convert reliability from loc Eng common pos format to 
       QMI_LOC format.
@param [in]   srcRel :   Reliability in loc Eng Pos format 
@param [out]  pDstRel :  Reliability in QMI_LOC format

@retval TRUE  Reliability successfully translated
@retval FALSE Reliability was not translated successfully
---------------------------------------------------------------------*/ 

extern boolean convertLocEngReliability(
   locEngReliabilityEnumT_v01 srcRel,
   qmiLocReliabilityEnumT_v02 *pDstRel );

/**  convertLocEngTimeSrc
 
@brief : convert Time source from loc Eng common pos format to 
       QMI_LOC format.
@param [in]   srcTimeSrc :   Time Source in loc Eng Pos format 
@param [out]  pDstTimeSrc :  Time Source in QMI_LOC format

@retval TRUE  Time Source successfully translated
@retval FALSE Time Source was not translated successfully
---------------------------------------------------------------------*/ 

extern boolean convertLocEngTimeSrc(
   locEngTimeSourceEnumT_v01 srcTimeSrc,
   qmiLocTimeSourceEnumT_v02 *pDstTimeSrc );

/**  convertLocEngSensorUsage
 
@brief : convert sensor from loc Eng common format to QMI_LOC
       format.
@param [in]   pSrcSensorUsage : Sensor Usage in loc Eng Pos 
       format
@param [out] pDstSensorUsage :  Sensor Usage in QMI_LOC format

@retval TRUE  Sensor Usage successfully translated
@retval FALSE Sensor Usage was not translated successfully
---------------------------------------------------------------------*/ 

extern boolean convertLocEngSensorUsage(
   const locEngSensorUsageIndicatorStructT_v01 *pSrcSensorUsage,
   qmiLocSensorUsageIndicatorStructT_v02 *pDstSensorUsage);

/**   convertLocEngPosToBapPos
 
@brief : convert loc Eng common position to QMI_LOC Best available
         position.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position
@param [out]  pBapPosition    :  Pointer to BAP position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

extern boolean convertLocEngPosToBapPos
(
  const locEngPositionReportMsgT_v01        *pLocEngPosition,
  qmiLocGetBestAvailablePositionIndMsgT_v02 *pBapPosition
);

/**   convertLocEngPosToAvailWwanPos
 
@brief : convert loc Eng common position to QMI_LOC Available WWAN
         position.
@param [in]   pLocEngPosition : Pointer to Loc Eng position
@param [out]  pAvWwanPosition : Pointer to Available WWAN position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

extern boolean convertLocEngPosToAvailWwanPos
(
  const locEngPositionReportMsgT_v01    *pLocEngPosition,
  qmiLocGetAvailWwanPositionIndMsgT_v02 *pAvWwanPosition
);

/**   convertLocEngPosToAvailablePosition
 
@brief : convert loc Eng common position to QMI_LOC Available GNSS
         position.
@param [in]   pLocEngPosition : Pointer to Loc Eng position
@param [out]  pAvailablePosition : Pointer to Available GNSS position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully

---------------------------------------------------------------------*/

extern boolean convertLocEngPosToAvailablePosition
(
  const locEngPositionReportMsgT_v01    *pLocEngPosition,
  qmiLocGetAvailablePositionIndParamMsgT_v02 *pAvailablePosition
);

/**   convertLocEngStatus
 
@brief : convert locEng status to QMI_LOC status.
@param [in]  locEngStatus :  Status in locEng format
 
@retval  corresponding qmiLoc status
---------------------------------------------------------------------*/ 

extern qmiLocStatusEnumT_v02 convertLocEngStatus( 
   locEngStatusEnumT_v01 locEngStatus);


/**   convertLocEngEventPedometerControlInd
 
@brief : convert locEng Pedometer Control event to QMI_LOC. 
@param [in]   pLocEngEventPedometerControl :  Pointer to Loc Eng 
              Pedometer Control event payload.
@param [out]  pEventPedometerControl :  Pointer to QMI_LOC 
       Pedometer control event payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/ 

extern boolean convertLocEngEventPedometerControlInd( 
   const locEngEventPedometerControlIndMsgT_v01 *pLocEngEventPedometerControl,
   qmiLocEventPedometerControlIndMsgT_v02 *pEventPedometerControl);

/**   convertLocEngEventMotionDataControlInd
 
@brief : convert locEng Motion Data Control event to QMI_LOC. 
@param [in]   pLocEngEventMotionControl :  Pointer to Loc Eng 
              Motion Data Control event payload.
@param [out]  pEventMotionControl :  Pointer to QMI_LOC Motiona 
       Data Control event payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/ 

extern boolean convertLocEngEventMotionDataControlInd( 
   const locEngEventMotionDataControlIndMsgT_v01 *pLocEngEventMotionControl,
   qmiLocEventMotionDataControlIndMsgT_v02 *pEventMotionControl);

/** convertQmiLocPedomterReportReq
@brief : convert QMI LOC pedometer report request to Loc Eng. 
@param [in]   pQmiLocPedometerReportReq :  Pointer to QMI LOC 
              pedometer report request payload.
@param [out]  pPedometerReportReq :  Pointer to Loc Eng 
       pedometer report request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/ 

extern boolean convertQmiLocPedomterReportReq( 
   const qmiLocPedometerReportReqMsgT_v02 *pQmiLocPedometerReportReq,
   locEngPedometerReportReqMsgT_v01 *pPedometerReportReq);



/** convertQmiLocInjectMotionDataReq
@brief : convert QMI LOC inject motion data request to Loc Eng. 
@param [in]   pQmiLocMotionDataReq :  Pointer to QMI LOC 
              inject motion data request payload.
@param [out]  pMotionDataReq :  Pointer to Loc Eng inject motion
       data request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/ 

extern boolean convertQmiLocInjectMotionDataReq( 
   const qmiLocInjectMotionDataReqMsgT_v02 *pQmiLocMotionDataReq,
   locEngInjectMotionDataReqMsgT_v01 *pMotionDataReq);


/** convertToLocEngInjectVehicleSensorDataReq
@brief : convert QMI LOC vehicle sensor inject data 
       request to Loc Eng.
@param [in]   pQmiLocVehicleSensorReq :  Pointer to QMI LOC 
              vehicle sensor inject data request payload.
@param [out]  pLocEngVehicleSensorReq :  Pointer to Loc Eng 
       vehicle sensor inject data request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

extern boolean convertToLocEngInjectVehicleSensorDataReq(
  const qmiLocInjectVehicleSensorDataReqMsgT_v02 *pQmiLocVehicleSensorReq,
  locEngInjectVehicleSensorDataReqMsgT_v01 *pLocEngVehicleSensorReq);


/** convertTolocEngSetNavigationConfigReq
@brief : convert QMI LOC set sensor properties data 
       request to Loc Eng Set Navigation Config type.
@param [in]   pQmiLocSetSensorPropertiesReq :  Pointer to QMI 
              LOC set sensor properties data request payload.
@param [out]  pLocEngSetNavConfigReq :  Pointer to Loc Eng Set 
       Navigation config data request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

extern boolean convertTolocEngSetNavigationConfigReq(
  const qmiLocSetSensorPropertiesReqMsgT_v02 *pQmiLocSetSensorPropertiesReq,
  locEngSetNavigationConfigReqMsgT_v01 *pLocEngSetNavConfigReq);


/** convertTolocEngGetNavigationConfigReq
@brief : convert QMI LOC get sensor properties data request to 
       Loc Eng get Navigation Config type.
@param [in]   pQmiLocGtSensorPropertiesReq :  Pointer to QMI 
              LOC get sensor properties data request payload.
@param [out]  pLocEngGetNavConfigReq :  Pointer to Loc Eng Get 
       Navigation config data request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

extern boolean convertTolocEngGetNavigationConfigReq(
  const qmiLocGetSensorPropertiesReqMsgT_v02 *pQmiLocGetSensorPropertiesReq,
  locEngGetNavigationConfigReqMsgT_v01 *pLocEngGetNavConfigReq);



/**  convertFromlocEngGetNavigationConfigResp
@brief : convert locEng Get Navigation Config Resp to QMI_LOC.
@param [in]   pLocEngEventMotionControl :  Pointer to Loc Eng 
       Motion Data Control event payload.
@param [out]  pEventMotionControl :  Pointer to QMI_LOC Motion 
       Data Control event payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

extern boolean convertFromlocEngGetNavigationConfigResp(
  const locEngGetNavigationConfigRespMsgT_v01 *pLocEngGetNavConfigResp,
  qmiLocGetSensorPropertiesIndMsgT_v02 *pQmiLocGetSensorPropertiesInd,
  qmiLocSensorPropertiesMaskT_v02 sensorPropertiesMask);


/**   convertLocEngStatusToQmiError
 
@brief : convert loc Eng status to QMI error .
@param [in]  locEngStatus :  Status in locEng format
 
@retval  corresponding QMI error
------------------------------------------------------------------*/

extern qmi_error_type_v01 convertLocEngStatusToQmiError(
  locEngStatusEnumT_v01 locEngStatus);


/** convertQmiLocToLocEngWiFiApDataMask
@brief : convert QMI wifi AP device type enum to Loc Eng. 
@param [in]   apDataMask :  QMI LOC type mask.
@param [out]  pConvertedType :  Pointer to Loc Eng type.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/
// Routine to translate QMI-LOC WiFi AP data mask to LocEng type.
extern boolean convertQmiLocToLocEngWiFiApDataMask(qmiLocWifiApDataMaskT_v02   apDataMask,
                                                   locEngWifiMaskT_v01         *pConvertedType);


/** convertQmiLocToLocEngWiFiDeviceTypeEnum
@brief : convert QMI wifi AP device type enum to Loc Eng. 
@param [in]   deviceType :  QMI LOC type enum.
@param [out]  pConvertedType :  Pointer to Loc Eng type.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/
// Routine to translate QMI-LOC WiFi device type enum to LocEng type.
extern boolean convertQmiLocToLocEngWiFiDeviceTypeEnum(qmiLocWifiApDataDeviceTypeEnumT_v02  deviceType,
                                                       locEngWifiApDeviceTypeEnumT_v01      *pConvertedType);


/** convertQmiLocToLocEngWiFiRTDTypeEnum
@brief : convert QMI wifi AP RTD type enum to Loc Eng. 
@param [in]   rtdType :  QMI LOC type enum.
@param [out]  pConvertedType :  Pointer to Loc Eng type.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/
// Routine to translate QMI-LOC WiFi RTD type enum to LocEng type.
extern boolean convertQmiLocToLocEngWiFiRTDTypeEnum(qmiLocWifiApDataRtdUnitTypeEnumT_v02  rtdType,
                                                    locEngWifiApRtdUnitTypeEnumT_v01      *pConvertedType);


/** convertQmiLocInjectWifiApDataReq
@brief : convert QMI wifi AP scan data request to Loc Eng. 
@param [in]   pQmiLocWifiApDataReq :  Pointer to QMI LOC 
              inject wifi AP data listpayload.
@param [out]  pMotionDataReq :  Pointer to Loc Eng inject wifi 
       AP data listpayload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/
extern boolean convertQmiLocInjectWifiApDataReq(const qmiLocWifiApDataStructT_v02  *pQmiLocWifiApDataReq,
                                                locEngWifiApInfoStructT_v01        *pWifiApDataReq,
                                                uint32                             numberOfAps);

/**   convertLocEngPosToBatchPos
 
@brief : convert loc Eng common position to QMI_LOC batched
         position structure.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position
@param [out]  pBatchedPosition    :  Pointer to BAP position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

extern boolean convertLocEngPosToBatchedPos
(
  const locEngPositionReportMsgT_v01       *pLocEngPosition,
  qmiLocBatchedReportStructT_v02     *const pBatchedPosition
);

/**   convertLocEngPosToDbtPosInd
 
@brief : convert loc Eng common position to QMI_LOC DBT
         position structure.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position 
@param [in]   positionType    :  Origin or Tracking Position 
@param [in]   locReqId        :  Request Id associated 
@param [out]  pDbtPositionInd :  Pointer to DBT position 
                                 Indication

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

extern boolean convertLocEngPosToDbtPosInd
(
  const locEngPositionReportMsgT_v01            *pLocEngPosition,
  aonPositionType                               positionType,
  uint32                                        locReqId,
  qmiLocEventDbtPositionReportIndMsgT_v02       *const pDbtPositionInd
  
);


#endif //LOC_CONV_LOC_ENG_QMI_LOC_H
