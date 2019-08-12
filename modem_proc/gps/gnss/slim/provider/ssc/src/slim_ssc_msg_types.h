#ifndef SLIM_SSC_MSG_TYPES_H
#define SLIM_SSC_MSG_TYPES_H
/**
@file
@brief SSC Provider-specific messages

  Header containing the message types that SSC provider can route through
  SLIM owning task.

@dependencies
  - FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT -- Enable pedestrian alignment support

@ingroup slim_SSCProvider
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
			   
			   Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
			   All Rights Reserved.
			   Qualcomm Technologies, Inc. Confidential and Proprietary  
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/ssc/src/slim_ssc_msg_types.h#1 $ */

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "slim_ssc_provider_data.h"
#include "sns_sam_distance_bound_v01.h"
#include "sns_sam_qmd_v01.h"
#include "sns_sam_ped_v01.h"
#ifdef FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT
#include "sns_sam_ped_alignment_v01.h"
#endif /* FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT */
#include "sns_sam_smd_v01.h"
#include "sns_smgr_api_v01.h"
#include "sns_time_api_v02.h"

//! @addtogroup slim_SSCProvider
//! @{

/*----------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/
/*!
@brief SSC provider message identifiers

This enumeration contains all SSC provider-specific message identifiers.
*/
typedef enum
{
  eSLIM_SSC_MSG_ID_DB_ENABLE_RESP,
  eSLIM_SSC_MSG_ID_DB_DISABLE_RESP,
  eSLIM_SSC_MSG_ID_DB_IND,
  eSLIM_SSC_MSG_ID_DB_SET_RESP,
  eSLIM_SSC_MSG_ID_DB_GET_RESP,

  eSLIM_SSC_MSG_ID_PED_ALIGNMENT_ENABLE_RESP,
  eSLIM_SSC_MSG_ID_PED_ALIGNMENT_DISABLE_RESP,
  eSLIM_SSC_MSG_ID_PED_ALIGNMENT_IND,

  eSLIM_SSC_MSG_ID_PEDOMETER_ENABLE_RESP,
  eSLIM_SSC_MSG_ID_PEDOMETER_DISABLE_RESP,
  eSLIM_SSC_MSG_ID_PEDOMETER_RESET_RESP,
  eSLIM_SSC_MSG_ID_PEDOMETER_IND,

  eSLIM_SSC_MSG_ID_QMD_ENABLE_RESP,
  eSLIM_SSC_MSG_ID_QMD_DISABLE_RESP,
  eSLIM_SSC_MSG_ID_QMD_IND,

  eSLIM_SSC_MSG_ID_SMGR_RESP,
  eSLIM_SSC_MSG_ID_SMGR_IND,

  eSLIM_SSC_MSG_ID_SMD_ENABLE_RESP,
  eSLIM_SSC_MSG_ID_SMD_DISABLE_RESP,
  eSLIM_SSC_MSG_ID_SMD_IND,

  eSLIM_SSC_MSG_ID_TIME_GET_RESP,

  eSLIM_SSC_MSG_ID_NOTIFICATION,

  //! @cond
  eSLIM_SSC_MSG_ID_MAX                  = 2147483647 /* Force 32bit */
  //! @endcond
} slim_SscMsgIdEnumType;

/*! Struct for SSC response IPC header */
typedef struct
{
  slimErrorEnumT e_SlimError;
  /**< SLIM error code */

  int32 l_TxnId;
  /**< SLIM transaction id */
} slim_SscRespHeaderStructT;

/*! Struct for SSC indication IPC header */
typedef struct
{
  slimErrorEnumT e_SlimError;
  /**< SLIM error code */

  slimClientHandleT p_Handle;
  /**< SLIM client handle */
} slim_SscIndHeaderStructT;


/*! Struct for SSC indication IPC header */
typedef struct
{
  slimErrorEnumT e_SlimError;
  /**< SLIM error code */

//  slimClientHandleT p_Handle;
  /**< SLIM client handle */
} slim_SscNotificationHeaderStructT;

/*! Struct for SSC enable distance bound response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC enable distance bound response payload is valid */

  sns_sam_distance_bound_enable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscEnableDbRespMsgT;

/*! Struct for SSC disable distance bound response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC disable distance bound response payload is valid */

  sns_sam_distance_bound_disable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscDisableDbRespMsgT;

/*! Struct for SSC set distance bound response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC set distance bound response payload is valid */

  sns_sam_distance_bound_set_bound_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscSetDbRespMsgT;

/*! Struct for SSC get distance bound response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC get distance bound response payload is valid */

  sns_sam_distance_bound_get_report_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscGetDbRespMsgT;

/*! Struct for SSC distance bound indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_IndData_valid;
  /**< TRUE if SSC indication payload is valid */

  sns_sam_distance_bound_report_ind_msg_v01 z_IndData;
  /**< Indication data */
} slim_SscDbIndMsgT;

/*! Union for distance bound messages. */
typedef union 
{
  slim_SscEnableDbRespMsgT z_EnableRespMsg;
  /**< Enable response */

  slim_SscDisableDbRespMsgT z_DisableRespMsg;
  /**< Disable response */

  slim_SscSetDbRespMsgT z_SetRespMsg;
  /**< Set response */

  slim_SscGetDbRespMsgT z_GetRespMsg;
  /**< Get response */

  slim_SscDbIndMsgT z_IndMsg;
  /**< Indication */
} slim_SscDistanceBoundMsgUnionType;

/*! Struct for SSC enable pedometer response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC enable pedometer response payload is valid */

  sns_sam_ped_enable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscEnablePedRespMsgT;

/*! Struct for SSC disable pedometer response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC disable pedometer response payload is valid */

  sns_sam_ped_disable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscDisablePedRespMsgT;

/*! Struct for SSC reset pedometer response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC reset pedometer response payload is valid */

  sns_sam_ped_reset_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscResetPedRespMsgT;

/*! Struct for SSC pedometer indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_IndData_valid;
  /**< TRUE if SSC indication payload is valid */

  sns_sam_ped_report_ind_msg_v01 z_IndData;
  /**< Indication data */
} slim_SscPedIndMsgT;

/*! Union for pedometer messages. */
typedef union 
{
  slim_SscEnablePedRespMsgT z_EnableRespMsg;
  /**< Enable response */

  slim_SscDisablePedRespMsgT z_DisableRespMsg;
  /**< Disable response */

  slim_SscResetPedRespMsgT z_ResetRespMsg;
  /**< Reset response */

  slim_SscPedIndMsgT z_IndMsg;
  /**< Indication */
} slim_SscPedometerMsgUnionType;

#ifdef FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT
/*! Struct for SSC enable ped alignment response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC enable ped alignment response payload is valid */

  sns_sam_ped_alignment_enable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscEnablePedAlignmentRespMsgT;

/*! Struct for SSC disable ped alignment response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC disable ped alignment response payload is valid */

  sns_sam_ped_alignment_disable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscDisablePedAlignmentRespMsgT;

/*! Struct for SSC ped alignment indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_IndData_valid;
  /**< TRUE if SSC indication payload is valid */

  sns_sam_ped_alignment_report_ind_msg_v01 z_IndData;
  /**< Indication data */
} slim_SscPedAlignmentIndMsgT;

/*! Union for ped alignment messages. */
typedef union 
{
  slim_SscEnablePedAlignmentRespMsgT z_EnableRespMsg;
  /**< Enable response */

  slim_SscDisablePedAlignmentRespMsgT z_DisableRespMsg;
  /**< Disable response */

  slim_SscPedAlignmentIndMsgT z_IndMsg;
  /**< Indication */
} slim_SscPedAlignmentMsgUnionType;
#endif /* FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT */

/*! Struct for SSC enable AMD/RMD response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  slim_SscQmdConnectionType e_QmdType;
  /**< Type of the connection. */

  boolean b_RespData_valid;
  /**< TRUE if SSC enable AMD/RMD response payload is valid */

  sns_sam_qmd_enable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscEnableQmdRespMsgT;

/*! Struct for SSC disable AMD/RMD response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  slim_SscQmdConnectionType e_QmdType;
  /**< Type of the connection. */

  boolean b_RespData_valid;
  /**< TRUE if SSC disable AMD/RMD response payload is valid */

  sns_sam_qmd_disable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscDisableQmdRespMsgT;

/*! Struct for SSC AMD/RMD indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscIndHeaderStructT z_IndHeader;
  /**< Indication header */

  slim_SscQmdConnectionType e_QmdType;
  /**< Type of the connection. */

  boolean b_IndData_valid;
  /**< TRUE if SSC indication payload is valid */

  sns_sam_qmd_report_ind_msg_v01 z_IndData;
  /**< Indication data */
} slim_SscQmdIndMsgT;

/*! Union for QMD messages. */
typedef union 
{
  slim_SscEnableQmdRespMsgT z_EnableRespMsg;
  /**< Enable response */

  slim_SscDisableQmdRespMsgT z_DisableRespMsg;
  /**< Disable response */

  slim_SscQmdIndMsgT z_IndMsg;
  /**< Indication */
} slim_SscQmdMsgUnionType;

/*! Struct for SSC enable SMD response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC enable SMD response payload is valid */

  sns_sam_smd_enable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscEnableSmdRespMsgT;

/*! Struct for SSC disable SMD response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC disable SMD response payload is valid */

  sns_sam_smd_disable_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscDisableSmdRespMsgT;

/*! Struct for SSC SMD indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_IndData_valid;
  /**< TRUE if SSC indication payload is valid */

  sns_sam_smd_report_ind_msg_v01 z_IndData;
  /**< Indication data */
} slim_SscSmdIndMsgT;

/*! Union for SMD messages. */
typedef union 
{
  slim_SscEnableSmdRespMsgT z_EnableRespMsg;
  /**< Enable response */

  slim_SscDisableSmdRespMsgT z_DisableRespMsg;
  /**< Disable response */

  slim_SscSmdIndMsgT z_IndMsg;
  /**< Indication */
} slim_SscSmdMsgUnionType;

/*! Struct for SSC SMGR request response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC SMGR response payload is valid */

  sns_smgr_buffering_resp_msg_v01 z_RespData;
  /**< Response data */
} slim_SscSmgrRespMsgT;

/*! Struct for SSC SMGR data indication */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscIndHeaderStructT z_IndHeader;
  /**< Indication header */

  boolean b_IndData_valid;
  /**< TRUE if SSC indication payload is valid */

  sns_smgr_buffering_ind_msg_v01 z_IndData;
  /**< Indication data */
} slim_SscSmgrIndMsgT;

/*! Union for SMGR messages. */
typedef union 
{
  slim_SscSmgrRespMsgT z_RespMsg;
  /**< Response */

  slim_SscSmgrIndMsgT z_IndMsg;
  /**< Indication */
} slim_SscSmgrMsgUnionType;

/*! Struct for SSC time request response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */
  
  slim_SscRespHeaderStructT z_RespHeader;
  /**< Response header */

  boolean b_RespData_valid;
  /**< TRUE if SSC time response payload is valid */

  sns_time_timestamp_resp_msg_v02 z_RespData;
  /**< Response data */
} slim_SscTimeRespMsgT;

/*! Struct for SSC time request response */
typedef struct
{
  slim_ProviderEnumType e_Provider;
  slimAvailableServiceMaskT q_ServiceMask;
  /**< Response data */
} slim_SscNotificationPayloadStructT;


/*! Struct for SSC time request response */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  /**< Modem millisecond timetick when the IPC was sent. */

  slim_SscNotificationHeaderStructT z_NotificationHeader;
  /**< Response header */

  boolean b_NotificationData_valid;
  /**< TRUE if SSC time response payload is valid */

  slim_SscNotificationPayloadStructT z_NotificationData;
  /**< Response data */
} slim_SscNotificationMsgT;
//! @}

#endif /* SLIM_SSC_MSG_TYPES_H */
