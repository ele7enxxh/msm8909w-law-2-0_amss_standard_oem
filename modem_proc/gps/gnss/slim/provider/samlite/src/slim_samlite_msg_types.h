#ifndef SLIM_SAMLITE_MSG_TYPES_H
#define SLIM_SAMLITE_MSG_TYPES_H
/**
  @file

  Header containing the message types that SAMLite provider can route through
  SLIM owning task.

  @ingroup slim_SAMLProvider
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/samlite/src/slim_samlite_msg_types.h#1 $ */

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "saml_client_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_SAMLProvider
//! @{

/*----------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/
/**
@brief SAML Provider callback message identifiers

These message identifiers are used for routing SAML messages from SAML task
context into SLIM context.
*/
typedef enum
{
  eSLIM_SAMLITE_MSG_ID_IND_SERVICE_STATUS = 0,
  /*!< Service status indication

    @sa slim_SAMLiteInjectServiceStatusEventIndMsgT
  */
  eSLIM_SAMLITE_MSG_ID_RESPONSE_MOTION_DATA,
  /*!< Motion service response

    @sa slim_SAMLiteRespMsgT
  */
  eSLIM_SAMLITE_MSG_ID_RESPONSE_PEDOMETER,
  /*!< Pedometer service response

    @sa slim_SAMLiteRespMsgT
  */
  eSLIM_SAMLITE_MSG_ID_RESPONSE_PED_ALIGNMENT,
  /*!< Pedestrial alignment service response

    @sa slim_SAMLitePedAlignmentRespMsgT
  */
  eSLIM_SAMLITE_MSG_ID_IND_MOTION_DATA,
  /*!< Motion data indication

    @sa slim_SAMLiteInjectMotionDataIndMsgT
  */
  eSLIM_SAMLITE_MSG_ID_IND_PEDOMETER,
  /*!< Motion data

    @sa slim_SAMLiteInjectPedometerDataIndMsgT
  */
  eSLIM_SAMLITE_MSG_ID_IND_PED_ALIGNMENT,
  /*!< Pedestrian alignment data

    @sa slim_SAMLiteInjectPedestrianAlignmentDataIndMsgT
  */

  //! @cond
  eSLIM_SAMLITE_MSG_ID_MAX                  = 2147483647 /* Force 32bit */
  //! @endcond
} slim_SAMLiteMsgIdEnumType;

/*! Struct for SAMLite response IPC header */
typedef struct
{
  saml_ErrorEnumT e_MsgError;
  /**< SAMLite error code */

  int32 l_TxnId;
  /**< Client transaction ID set in request */
} slim_SAMLiteRespHeaderStructT;

/*! Struct for SAMLite indication IPC header */
typedef struct
{
  saml_ErrorEnumT e_MsgError;
  /**< SAMLite error code */
} slim_SAMLiteIndHeaderStructT;

/*! Struct for SAMLite response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SAMLiteRespHeaderStructT z_RespHeader;
  /**< Response header */

  /* No payload */
} slim_SAMLiteRespMsgT;

/*! Struct for SAMLite ped alignment response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SAMLiteRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SAMLite ped alignment response payload is valid */

  saml_PedestrianAlignmentEnableResponseStructT z_RespData;
  /**< Response data */
} slim_SAMLitePedAlignmentRespMsgT;

/*!
@brief SAML pedometer data message
*/
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slim_SAMLiteIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_PedometerData_valid;
  /**< TRUE if SAMLite pedometer data payload is valid */

  saml_PedometerDataStructT z_PedometerData;
  /**< SAMLite pedometer data payload */

} slim_SAMLiteInjectPedometerDataIndMsgT;

/*!
@brief SAML motion data message
*/
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slim_SAMLiteIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_MotionData_valid;
  /**< TRUE if SAMLite motion data payload is valid */

  saml_MotionDataStructT z_MotionData;
  /**< SAMLite motion data payload */

} slim_SAMLiteInjectMotionDataIndMsgT;

/*!
@brief SAML pedestrian alignment data message
*/
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slim_SAMLiteIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_PedestrianAlignmentData_valid;
  /**< TRUE if SAMLite pedestrian alignment data payload is valid */

  saml_PedestrianAlignmentDataStructT z_PedestrianAlignmentData;
  /**< SAMLite pedestrian alignment data payload */

  saml_PedAlignmentInputTypeEnumT e_Algorithm;
  /**< SAMLite pedestrian alignment algorithm type */

} slim_SAMLiteInjectPedestrianAlignmentDataIndMsgT;

/*!
@brief SAML service status message
*/
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slim_SAMLiteIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_ServiceStatusEvent_valid;
  /**< TRUE if SAMLite service status event payload is valid */

  saml_ServiceStatusEventStructT z_ServiceStatusEvent;
  /**< SAMLite service status event payload */

} slim_SAMLiteInjectServiceStatusEventIndMsgT;

//! @}

#ifdef __cplusplus
}
#endif


#endif /* SLIM_SAMLITE_MSG_TYPES_H */
