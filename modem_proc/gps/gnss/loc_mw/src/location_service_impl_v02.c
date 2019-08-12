/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              L O C A T I O N _ S E R V I C E _ I M P L _ V 0 2  . C

GENERAL DESCRIPTION
  This is the file which defines the loc service Data structures.

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/location_service_impl_v02.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.5
   It was generated on: Thu Jul  9 2015 (Spin 0)
   From IDL File: location_service_v02.idl */

#include "qmi_si.h"
#include "location_service_v02.h"

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v02 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_supported_msgs_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmi_get_supported_msgs_resp_impl_v02,
  NULL
};

static const qmi_implemented_optionals qmi_get_supported_fields_req_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmi_get_supported_fields_resp_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* request_fields */
    QMI_SI_TLV_SET(0x11)   |  /* response_fields */
    QMI_SI_TLV_SET(0x12)      /* indication_fields */
};

static const qmi_implemented_optionals qmi_get_supported_fields_resp_impl_v02 =
{
  1, /* number of elements in array */
  qmi_get_supported_fields_resp_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_supported_fields_reqresp_impl_tuple_v02 = 
{
  &qmi_get_supported_fields_req_impl_v02,
  &qmi_get_supported_fields_resp_impl_v02,
  NULL
};

static const qmi_implemented_optionals qmiLocInformClientRevisionReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocGenRespMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inform_client_revision_reqresp_impl_tuple_v02 = 
{
  &qmiLocInformClientRevisionReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  NULL
};

static const qmi_implemented_optionals qmiLocRegEventsReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_reg_events_reqresp_impl_tuple_v02 = 
{
  &qmiLocRegEventsReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  NULL
};

static const uint32_t qmiLocStartReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* fixRecurrence */
    QMI_SI_TLV_SET(0x11)   |  /* horizontalAccuracyLevel */
    QMI_SI_TLV_SET(0x12)   |  /* intermediateReportState */
    QMI_SI_TLV_SET(0x13)   |  /* minInterval */
    QMI_SI_TLV_SET(0x14)   |  /* applicationId */
    QMI_SI_TLV_SET(0x15)   |  /* configAltitudeAssumed */
    QMI_SI_TLV_SET(0x16)      /* minIntermediatePositionReportInterval */
};

static const qmi_implemented_optionals qmiLocStartReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocStartReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_start_reqresp_impl_tuple_v02 = 
{
  &qmiLocStartReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  NULL
};

static const qmi_implemented_optionals qmiLocStopReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_stop_reqresp_impl_tuple_v02 = 
{
  &qmiLocStopReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  NULL
};

static const uint32_t qmiLocEventPositionReportIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* latitude */
    QMI_SI_TLV_SET(0x11)   |  /* longitude */
    QMI_SI_TLV_SET(0x12)   |  /* horUncCircular */
    QMI_SI_TLV_SET(0x13)   |  /* horUncEllipseSemiMinor */
    QMI_SI_TLV_SET(0x14)   |  /* horUncEllipseSemiMajor */
    QMI_SI_TLV_SET(0x15)   |  /* horUncEllipseOrientAzimuth */
    QMI_SI_TLV_SET(0x16)   |  /* horConfidence */
    QMI_SI_TLV_SET(0x17)   |  /* horReliability */
    QMI_SI_TLV_SET(0x18)   |  /* speedHorizontal */
    QMI_SI_TLV_SET(0x19)   |  /* speedUnc */
    QMI_SI_TLV_SET(0x1A)   |  /* altitudeWrtEllipsoid */
    QMI_SI_TLV_SET(0x1B)   |  /* altitudeWrtMeanSeaLevel */
    QMI_SI_TLV_SET(0x1C)   |  /* vertUnc */
    QMI_SI_TLV_SET(0x1D)   |  /* vertConfidence */
    QMI_SI_TLV_SET(0x1E)   |  /* vertReliability */
    QMI_SI_TLV_SET(0x1F)   |  /* speedVertical */
    QMI_SI_TLV_SET(0x20)   |  /* heading */
    QMI_SI_TLV_SET(0x21)   |  /* headingUnc */
    QMI_SI_TLV_SET(0x22)   |  /* magneticDeviation */
    QMI_SI_TLV_SET(0x23)   |  /* technologyMask */
    QMI_SI_TLV_SET(0x24)   |  /* DOP */
    QMI_SI_TLV_SET(0x25)   |  /* timestampUtc */
    QMI_SI_TLV_SET(0x26)   |  /* leapSeconds */
    QMI_SI_TLV_SET(0x27)   |  /* gpsTime */
    QMI_SI_TLV_SET(0x28)   |  /* timeUnc */
    QMI_SI_TLV_SET(0x29)   |  /* timeSrc */
    QMI_SI_TLV_SET(0x2A)   |  /* sensorDataUsage */
    QMI_SI_TLV_SET(0x2B)   |  /* fixId */
    QMI_SI_TLV_SET(0x2C)   |  /* gnssSvUsedList */
    QMI_SI_TLV_SET(0x2D)      /* altitudeAssumed */
};

static const qmi_implemented_optionals qmiLocEventPositionReportIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventPositionReportIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_position_report_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventPositionReportIndMsgT_impl_v02
};

static const uint32_t qmiLocEventGnssSvInfoIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* svList */
};

static const qmi_implemented_optionals qmiLocEventGnssSvInfoIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventGnssSvInfoIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_gnss_sv_info_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGnssSvInfoIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventNmeaIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_nmea_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventNmeaIndMsgT_impl_v02
};

static const uint32_t qmiLocEventNiNotifyVerifyReqIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* NiVxInd */
    QMI_SI_TLV_SET(0x11)   |  /* NiSuplInd */
    QMI_SI_TLV_SET(0x12)   |  /* NiUmtsCpInd */
    QMI_SI_TLV_SET(0x13)   |  /* NiVxServiceInteractionInd */
    QMI_SI_TLV_SET(0x14)   |  /* NiSuplVer2ExtInd */
    QMI_SI_TLV_SET(0x15)      /* suplEmergencyNotification */
};

static const qmi_implemented_optionals qmiLocEventNiNotifyVerifyReqIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventNiNotifyVerifyReqIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_ni_notify_verify_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventNiNotifyVerifyReqIndMsgT_impl_v02
};

static const uint32_t qmiLocEventInjectTimeReqIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* timeServerInfo */
};

static const qmi_implemented_optionals qmiLocEventInjectTimeReqIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventInjectTimeReqIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_inject_time_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventInjectTimeReqIndMsgT_impl_v02
};

static const uint32_t qmiLocEventInjectPredictedOrbitsReqIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* serverList */
};

static const qmi_implemented_optionals qmiLocEventInjectPredictedOrbitsReqIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventInjectPredictedOrbitsReqIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_inject_predicted_orbits_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventInjectPredictedOrbitsReqIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventInjectPositionReqIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_inject_position_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventInjectPositionReqIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventEngineStateIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_engine_state_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventEngineStateIndMsgT_impl_v02
};

static const uint32_t qmiLocEventFixSessionStateIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* sessionId */
};

static const qmi_implemented_optionals qmiLocEventFixSessionStateIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventFixSessionStateIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_fix_session_state_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventFixSessionStateIndMsgT_impl_v02
};

static const uint32_t qmiLocEventWifiReqIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* tbfInMs */
};

static const qmi_implemented_optionals qmiLocEventWifiReqIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventWifiReqIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_wifi_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventWifiReqIndMsgT_impl_v02
};

static const uint32_t qmiLocEventSensorStreamingReadyStatusIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* accelReady */
    QMI_SI_TLV_SET(0x11)   |  /* gyroReady */
    QMI_SI_TLV_SET(0x12)   |  /* accelTemperatureReady */
    QMI_SI_TLV_SET(0x13)   |  /* gyroTemperatureReady */
    QMI_SI_TLV_SET(0x14)   |  /* calibratedMagReady */
    QMI_SI_TLV_SET(0x15)      /* uncalibratedMagReady */
};

static const qmi_implemented_optionals qmiLocEventSensorStreamingReadyStatusIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventSensorStreamingReadyStatusIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_sensor_streaming_ready_status_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventSensorStreamingReadyStatusIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventTimeSyncReqIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_time_sync_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventTimeSyncReqIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventSetSpiStreamingReportIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_set_spi_streaming_report_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventSetSpiStreamingReportIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventLocationServerConnectionReqIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_location_server_connection_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventLocationServerConnectionReqIndMsgT_impl_v02
};

static const uint32_t qmiLocGetServiceRevisionIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* gnssMeFWVerString */
    QMI_SI_TLV_SET(0x11)   |  /* gnssHostSWVerString */
    QMI_SI_TLV_SET(0x12)      /* gnssSWVerString */
};

static const qmi_implemented_optionals qmiLocGetServiceRevisionIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetServiceRevisionIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_service_revision_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetServiceRevisionIndMsgT_impl_v02
};

static const uint32_t qmiLocGetFixCriteriaIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* horizontalAccuracyLevel */
    QMI_SI_TLV_SET(0x11)   |  /* intermediateReportState */
    QMI_SI_TLV_SET(0x12)   |  /* minInterval */
    QMI_SI_TLV_SET(0x13)      /* applicationId */
};

static const qmi_implemented_optionals qmiLocGetFixCriteriaIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetFixCriteriaIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_fix_criteria_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetFixCriteriaIndMsgT_impl_v02
};

static const uint32_t qmiLocNiUserRespReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* NiVxPayload */
    QMI_SI_TLV_SET(0x11)   |  /* NiSuplPayload */
    QMI_SI_TLV_SET(0x12)   |  /* NiUmtsCpPayload */
    QMI_SI_TLV_SET(0x13)   |  /* NiVxServiceInteractionPayload */
    QMI_SI_TLV_SET(0x14)   |  /* NiSuplVer2ExtPayload */
    QMI_SI_TLV_SET(0x15)      /* suplEmergencyNotification */
};

static const qmi_implemented_optionals qmiLocNiUserRespReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocNiUserRespReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocNiUserRespIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_ni_useronse_reqresp_impl_tuple_v02 = 
{
  &qmiLocNiUserRespReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocNiUserRespIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectPredictedOrbitsDataReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* formatType */
};

static const qmi_implemented_optionals qmiLocInjectPredictedOrbitsDataReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectPredictedOrbitsDataReqMsgT_impl_array_v02
};
static const uint32_t qmiLocInjectPredictedOrbitsDataIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* partNum */
};

static const qmi_implemented_optionals qmiLocInjectPredictedOrbitsDataIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectPredictedOrbitsDataIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_predicted_orbits_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectPredictedOrbitsDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectPredictedOrbitsDataIndMsgT_impl_v02
};

static const uint32_t qmiLocGetPredictedOrbitsDataSourceIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* allowedSizes */
    QMI_SI_TLV_SET(0x11)      /* serverList */
};

static const qmi_implemented_optionals qmiLocGetPredictedOrbitsDataSourceIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetPredictedOrbitsDataSourceIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_predicted_orbits_data_source_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetPredictedOrbitsDataSourceIndMsgT_impl_v02
};

static const uint32_t qmiLocGetPredictedOrbitsDataValidityIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* validityInfo */
};

static const qmi_implemented_optionals qmiLocGetPredictedOrbitsDataValidityIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetPredictedOrbitsDataValidityIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_predicted_orbits_data_validity_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetPredictedOrbitsDataValidityIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectUtcTimeReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectUtcTimeIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_utc_time_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectUtcTimeReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectUtcTimeIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectPositionReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* latitude */
    QMI_SI_TLV_SET(0x11)   |  /* longitude */
    QMI_SI_TLV_SET(0x12)   |  /* horUncCircular */
    QMI_SI_TLV_SET(0x13)   |  /* horConfidence */
    QMI_SI_TLV_SET(0x14)   |  /* horReliability */
    QMI_SI_TLV_SET(0x15)   |  /* altitudeWrtEllipsoid */
    QMI_SI_TLV_SET(0x16)   |  /* altitudeWrtMeanSeaLevel */
    QMI_SI_TLV_SET(0x17)   |  /* vertUnc */
    QMI_SI_TLV_SET(0x18)   |  /* vertConfidence */
    QMI_SI_TLV_SET(0x19)   |  /* vertReliability */
    QMI_SI_TLV_SET(0x1A)   |  /* altSourceInfo */
    QMI_SI_TLV_SET(0x1B)   |  /* timestampUtc */
    QMI_SI_TLV_SET(0x1C)   |  /* timestampAge */
    QMI_SI_TLV_SET(0x1D)   |  /* positionSrc */
    QMI_SI_TLV_SET(0x1E)   |  /* rawHorUncCircular */
    QMI_SI_TLV_SET(0x1F)      /* rawHorConfidence */
};

static const qmi_implemented_optionals qmiLocInjectPositionReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectPositionReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocInjectPositionIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_position_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectPositionReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectPositionIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetEngineLockReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetEngineLockIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_engine_lock_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetEngineLockReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetEngineLockIndMsgT_impl_v02
};

static const uint32_t qmiLocGetEngineLockIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* lockType */
};

static const qmi_implemented_optionals qmiLocGetEngineLockIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetEngineLockIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_engine_lock_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetEngineLockIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetSbasConfigReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetSbasConfigIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_sbas_config_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetSbasConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetSbasConfigIndMsgT_impl_v02
};

static const uint32_t qmiLocGetSbasConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* sbasConfig */
};

static const qmi_implemented_optionals qmiLocGetSbasConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetSbasConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_sbas_config_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetSbasConfigIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetNmeaTypesReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetNmeaTypesIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_nmea_types_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetNmeaTypesReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetNmeaTypesIndMsgT_impl_v02
};

static const uint32_t qmiLocGetNmeaTypesIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* nmeaSentenceType */
};

static const qmi_implemented_optionals qmiLocGetNmeaTypesIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetNmeaTypesIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_nmea_types_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetNmeaTypesIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetLowPowerModeReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetLowPowerModeIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_low_power_mode_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetLowPowerModeReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetLowPowerModeIndMsgT_impl_v02
};

static const uint32_t qmiLocGetLowPowerModeIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* lowPowerMode */
};

static const qmi_implemented_optionals qmiLocGetLowPowerModeIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetLowPowerModeIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_low_power_mode_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetLowPowerModeIndMsgT_impl_v02
};

static const uint32_t qmiLocSetServerReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ipv4Addr */
    QMI_SI_TLV_SET(0x11)   |  /* ipv6Addr */
    QMI_SI_TLV_SET(0x12)      /* urlAddr */
};

static const qmi_implemented_optionals qmiLocSetServerReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetServerReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocSetServerIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_server_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetServerReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetServerIndMsgT_impl_v02
};

static const uint32_t qmiLocGetServerReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* serverAddrTypeMask */
};

static const qmi_implemented_optionals qmiLocGetServerReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetServerReqMsgT_impl_array_v02
};
static const uint32_t qmiLocGetServerIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ipv4Addr */
    QMI_SI_TLV_SET(0x11)   |  /* ipv6Addr */
    QMI_SI_TLV_SET(0x12)      /* urlAddr */
};

static const qmi_implemented_optionals qmiLocGetServerIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetServerIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_server_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetServerReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetServerIndMsgT_impl_v02
};

static const uint32_t qmiLocDeleteAssistDataReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* deleteSvInfoList */
    QMI_SI_TLV_SET(0x11)   |  /* deleteGnssDataMask */
    QMI_SI_TLV_SET(0x12)   |  /* deleteCellDbDataMask */
    QMI_SI_TLV_SET(0x13)   |  /* deleteClockInfoMask */
    QMI_SI_TLV_SET(0x14)   |  /* deleteBdsSvInfoList */
    QMI_SI_TLV_SET(0x15)      /* deleteGalSvInfoList */
};

static const qmi_implemented_optionals qmiLocDeleteAssistDataReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocDeleteAssistDataReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocDeleteAssistDataIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_delete_assist_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocDeleteAssistDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocDeleteAssistDataIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetXtraTSessionControlReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetXtraTSessionControlIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_xtra_t_session_control_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetXtraTSessionControlReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetXtraTSessionControlIndMsgT_impl_v02
};

static const uint32_t qmiLocGetXtraTSessionControlIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* xtraTSessionControl */
};

static const qmi_implemented_optionals qmiLocGetXtraTSessionControlIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetXtraTSessionControlIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_xtra_t_session_control_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetXtraTSessionControlIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectWifiPositionReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* wifiFixTime */
    QMI_SI_TLV_SET(0x11)   |  /* wifiFixPosition */
    QMI_SI_TLV_SET(0x12)   |  /* apInfo */
    QMI_SI_TLV_SET(0x13)   |  /* horizontalReliability */
    QMI_SI_TLV_SET(0x14)   |  /* rawHepe */
    QMI_SI_TLV_SET(0x15)      /* wifiApSsidInfo */
};

static const qmi_implemented_optionals qmiLocInjectWifiPositionReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectWifiPositionReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocInjectWifiPositionIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_wifi_position_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectWifiPositionReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectWifiPositionIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocNotifyWifiStatusReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocNotifyWifiStatusIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_notify_wifi_status_reqresp_impl_tuple_v02 = 
{
  &qmiLocNotifyWifiStatusReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocNotifyWifiStatusIndMsgT_impl_v02
};

static const uint32_t qmiLocGetRegisteredEventsIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* eventRegMask */
};

static const qmi_implemented_optionals qmiLocGetRegisteredEventsIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetRegisteredEventsIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_registered_events_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetRegisteredEventsIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetOperationModeReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetOperationModeIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_operation_mode_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetOperationModeReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetOperationModeIndMsgT_impl_v02
};

static const uint32_t qmiLocGetOperationModeIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* operationMode */
};

static const qmi_implemented_optionals qmiLocGetOperationModeIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetOperationModeIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_operation_mode_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetOperationModeIndMsgT_impl_v02
};

static const uint32_t qmiLocSetSpiStatusReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* confidenceStationary */
};

static const qmi_implemented_optionals qmiLocSetSpiStatusReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetSpiStatusReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocSetSpiStatusIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_spi_status_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetSpiStatusReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetSpiStatusIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectSensorDataReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* opaqueIdentifier */
    QMI_SI_TLV_SET(0x11)   |  /* threeAxisAccelData */
    QMI_SI_TLV_SET(0x12)   |  /* threeAxisGyroData */
    QMI_SI_TLV_SET(0x13)   |  /* threeAxisAccelDataTimeSource */
    QMI_SI_TLV_SET(0x14)   |  /* threeAxisGyroDataTimeSource */
    QMI_SI_TLV_SET(0x15)   |  /* accelTemperatureData */
    QMI_SI_TLV_SET(0x16)   |  /* gyroTemperatureData */
    QMI_SI_TLV_SET(0x17)   |  /* threeAxisMagData */
    QMI_SI_TLV_SET(0x18)      /* threeAxisMagDataTimeSource */
};

static const qmi_implemented_optionals qmiLocInjectSensorDataReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectSensorDataReqMsgT_impl_array_v02
};
static const uint32_t qmiLocInjectSensorDataIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* opaqueIdentifier */
    QMI_SI_TLV_SET(0x11)   |  /* threeAxisAccelSamplesAccepted */
    QMI_SI_TLV_SET(0x12)   |  /* threeAxisGyroSamplesAccepted */
    QMI_SI_TLV_SET(0x13)   |  /* accelTemperatureSamplesAccepted */
    QMI_SI_TLV_SET(0x14)   |  /* gyroTemperatureSamplesAccepted */
    QMI_SI_TLV_SET(0x15)      /* threeAxisMagSamplesAccepted */
};

static const qmi_implemented_optionals qmiLocInjectSensorDataIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectSensorDataIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_sensor_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectSensorDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectSensorDataIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectTimeSyncDataReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectTimeSyncDataIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_time_sync_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectTimeSyncDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectTimeSyncDataIndMsgT_impl_v02
};

static const uint32_t qmiLocSetCradleMountConfigReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* confidenceCradleMountState */
};

static const qmi_implemented_optionals qmiLocSetCradleMountConfigReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetCradleMountConfigReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocSetCradleMountConfigIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_cradle_mount_config_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetCradleMountConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetCradleMountConfigIndMsgT_impl_v02
};

static const uint32_t qmiLocGetCradleMountConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cradleMountState */
    QMI_SI_TLV_SET(0x11)      /* confidenceCradleMountState */
};

static const qmi_implemented_optionals qmiLocGetCradleMountConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetCradleMountConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_cradle_mount_config_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetCradleMountConfigIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetExternalPowerConfigReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetExternalPowerConfigIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_external_power_config_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetExternalPowerConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetExternalPowerConfigIndMsgT_impl_v02
};

static const uint32_t qmiLocGetExternalPowerConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* externalPowerState */
};

static const qmi_implemented_optionals qmiLocGetExternalPowerConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetExternalPowerConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_external_power_config_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetExternalPowerConfigIndMsgT_impl_v02
};

static const uint32_t qmiLocInformLocationServerConnStatusReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* apnProfile */
};

static const qmi_implemented_optionals qmiLocInformLocationServerConnStatusReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInformLocationServerConnStatusReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocInformLocationServerConnStatusIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inform_location_server_conn_status_reqresp_impl_tuple_v02 = 
{
  &qmiLocInformLocationServerConnStatusReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInformLocationServerConnStatusIndMsgT_impl_v02
};

static const uint32_t qmiLocSetProtocolConfigParametersReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* suplSecurity */
    QMI_SI_TLV_SET(0x11)   |  /* vxVersion */
    QMI_SI_TLV_SET(0x12)   |  /* suplVersion */
    QMI_SI_TLV_SET(0x13)   |  /* lppConfig */
    QMI_SI_TLV_SET(0x14)   |  /* assistedGlonassProtocolMask */
    QMI_SI_TLV_SET(0x15)   |  /* suplHashAlgo */
    QMI_SI_TLV_SET(0x16)   |  /* suplTlsVersion */
    QMI_SI_TLV_SET(0x17)   |  /* emergencyProtocol */
    QMI_SI_TLV_SET(0x18)      /* wifiScanInjectTimeout */
};

static const qmi_implemented_optionals qmiLocSetProtocolConfigParametersReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetProtocolConfigParametersReqMsgT_impl_array_v02
};
static const uint32_t qmiLocSetProtocolConfigParametersIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* failedProtocolConfigParamMask */
};

static const qmi_implemented_optionals qmiLocSetProtocolConfigParametersIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetProtocolConfigParametersIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_set_protocol_config_parameters_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetProtocolConfigParametersReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetProtocolConfigParametersIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGetProtocolConfigParametersReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocGetProtocolConfigParametersIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* suplSecurity */
    QMI_SI_TLV_SET(0x11)   |  /* vxVersion */
    QMI_SI_TLV_SET(0x12)   |  /* suplVersion */
    QMI_SI_TLV_SET(0x13)   |  /* lppConfig */
    QMI_SI_TLV_SET(0x14)   |  /* assistedGlonassProtocolMask */
    QMI_SI_TLV_SET(0x15)   |  /* suplHashAlgo */
    QMI_SI_TLV_SET(0x16)   |  /* suplTlsVersion */
    QMI_SI_TLV_SET(0x17)   |  /* emergencyProtocol */
    QMI_SI_TLV_SET(0x18)      /* wifiScanInjectTimeout */
};

static const qmi_implemented_optionals qmiLocGetProtocolConfigParametersIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetProtocolConfigParametersIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_protocol_config_parameters_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetProtocolConfigParametersReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetProtocolConfigParametersIndMsgT_impl_v02
};

static const uint32_t qmiLocSetSensorControlConfigReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sensorsUsage */
    QMI_SI_TLV_SET(0x11)      /* sensorProvider */
};

static const qmi_implemented_optionals qmiLocSetSensorControlConfigReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetSensorControlConfigReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocSetSensorControlConfigIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_sensor_control_config_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetSensorControlConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetSensorControlConfigIndMsgT_impl_v02
};

static const uint32_t qmiLocGetSensorControlConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sensorsUsage */
    QMI_SI_TLV_SET(0x11)      /* sensorProvider */
};

static const qmi_implemented_optionals qmiLocGetSensorControlConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetSensorControlConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_sensor_control_config_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetSensorControlConfigIndMsgT_impl_v02
};

static const uint32_t qmiLocSetSensorPropertiesReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* gyroBiasVarianceRandomWalk */
    QMI_SI_TLV_SET(0x11)   |  /* velocityRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x12)   |  /* accelerationRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x13)   |  /* angleRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x14)   |  /* rateRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x15)   |  /* vehicleDataUse */
    QMI_SI_TLV_SET(0x16)   |  /* vehicleVelocityRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x17)   |  /* vehicleAccelRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x18)   |  /* vehicleAngleRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x19)   |  /* vehicleAngularRateRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x1A)   |  /* vehicleOdometryScaleFactorRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x1B)      /* vehicleOdometryVariance */
};

static const qmi_implemented_optionals qmiLocSetSensorPropertiesReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetSensorPropertiesReqMsgT_impl_array_v02
};
static const uint32_t qmiLocSetSensorPropertiesIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* failedSensorPropertiesMask */
};

static const qmi_implemented_optionals qmiLocSetSensorPropertiesIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetSensorPropertiesIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_set_sensor_properties_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetSensorPropertiesReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetSensorPropertiesIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGetSensorPropertiesReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocGetSensorPropertiesIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* gyroBiasVarianceRandomWalk */
    QMI_SI_TLV_SET(0x11)   |  /* velocityRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x12)   |  /* accelerationRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x13)   |  /* angleRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x14)   |  /* rateRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x15)   |  /* vehicleDataUse */
    QMI_SI_TLV_SET(0x16)   |  /* vehicleVelocityRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x17)   |  /* vehicleAccelRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x18)   |  /* vehicleAngleRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x19)   |  /* vehicleAngularRateRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x1A)   |  /* vehicleOdometryScaleFactorRandomWalkSpectralDensity */
    QMI_SI_TLV_SET(0x1B)      /* vehicleOdometryVariance */
};

static const qmi_implemented_optionals qmiLocGetSensorPropertiesIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetSensorPropertiesIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_sensor_properties_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetSensorPropertiesReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetSensorPropertiesIndMsgT_impl_v02
};

static const uint32_t qmiLocSetSensorPerformanceControlConfigReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* performanceControlMode */
    QMI_SI_TLV_SET(0x11)   |  /* accelSamplingSpec */
    QMI_SI_TLV_SET(0x12)   |  /* gyroSamplingSpec */
    QMI_SI_TLV_SET(0x13)   |  /* algorithmConfig */
    QMI_SI_TLV_SET(0x14)   |  /* accelSamplingSpecHigh */
    QMI_SI_TLV_SET(0x15)      /* gyroSamplingSpecHigh */
};

static const qmi_implemented_optionals qmiLocSetSensorPerformanceControlConfigReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetSensorPerformanceControlConfigReqMsgT_impl_array_v02
};
static const uint32_t qmiLocSetSensorPerformanceControlConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* failedConfiguration */
};

static const qmi_implemented_optionals qmiLocSetSensorPerformanceControlConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetSensorPerformanceControlConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_set_sensor_performance_control_configuration_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetSensorPerformanceControlConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetSensorPerformanceControlConfigIndMsgT_impl_v02
};

static const uint32_t qmiLocGetSensorPerformanceControlConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* performanceControlMode */
    QMI_SI_TLV_SET(0x11)   |  /* accelSamplingSpec */
    QMI_SI_TLV_SET(0x12)   |  /* gyroSamplingSpec */
    QMI_SI_TLV_SET(0x13)   |  /* algorithmConfig */
    QMI_SI_TLV_SET(0x14)   |  /* accelSamplingSpecHigh */
    QMI_SI_TLV_SET(0x15)      /* gyroSamplingSpecHigh */
};

static const qmi_implemented_optionals qmiLocGetSensorPerformanceControlConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetSensorPerformanceControlConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_sensor_performance_control_configuration_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetSensorPerformanceControlConfigIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectSuplCertificateReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectSuplCertificateIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_supl_certificate_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectSuplCertificateReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectSuplCertificateIndMsgT_impl_v02
};

static const uint32_t qmiLocDeleteSuplCertificateReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* suplCertId */
};

static const qmi_implemented_optionals qmiLocDeleteSuplCertificateReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocDeleteSuplCertificateReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocDeleteSuplCertificateIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_delete_supl_certificate_reqresp_impl_tuple_v02 = 
{
  &qmiLocDeleteSuplCertificateReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocDeleteSuplCertificateIndMsgT_impl_v02
};

static const uint32_t qmiLocSetPositionEngineConfigParametersReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* injectedPositionControl */
    QMI_SI_TLV_SET(0x11)   |  /* filterSvUsage */
    QMI_SI_TLV_SET(0x12)   |  /* storeAssistData */
    QMI_SI_TLV_SET(0x13)      /* enableFasterTTFF */
};

static const qmi_implemented_optionals qmiLocSetPositionEngineConfigParametersReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetPositionEngineConfigParametersReqMsgT_impl_array_v02
};
static const uint32_t qmiLocSetPositionEngineConfigParametersIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* failedPositionEngineConfigParamMask */
};

static const qmi_implemented_optionals qmiLocSetPositionEngineConfigParametersIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetPositionEngineConfigParametersIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_set_position_engine_config_parameters_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetPositionEngineConfigParametersReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetPositionEngineConfigParametersIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGetPositionEngineConfigParametersReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocGetPositionEngineConfigParametersIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* injectedPositionControl */
    QMI_SI_TLV_SET(0x11)   |  /* filterSvUsage */
    QMI_SI_TLV_SET(0x12)   |  /* storeAssistData */
    QMI_SI_TLV_SET(0x13)      /* enableFasterTTFF */
};

static const qmi_implemented_optionals qmiLocGetPositionEngineConfigParametersIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetPositionEngineConfigParametersIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_position_engine_config_parameters_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetPositionEngineConfigParametersReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetPositionEngineConfigParametersIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventNiGeofenceNotificationIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_ni_geofence_notification_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventNiGeofenceNotificationIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventGeofenceGenAlertIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_geofence_gen_alert_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGeofenceGenAlertIndMsgT_impl_v02
};

static const uint32_t qmiLocEventGeofenceBreachIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geofencePosition */
    QMI_SI_TLV_SET(0x11)      /* breachConfidence */
};

static const qmi_implemented_optionals qmiLocEventGeofenceBreachIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventGeofenceBreachIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_geofence_breach_notification_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGeofenceBreachIndMsgT_impl_v02
};

static const uint32_t qmiLocAddCircularGeofenceReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* responsiveness */
    QMI_SI_TLV_SET(0x11)   |  /* confidence */
    QMI_SI_TLV_SET(0x12)   |  /* customResponsivenessValue */
    QMI_SI_TLV_SET(0x13)   |  /* dwellTime */
    QMI_SI_TLV_SET(0x14)      /* dwellTypeMask */
};

static const qmi_implemented_optionals qmiLocAddCircularGeofenceReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocAddCircularGeofenceReqMsgT_impl_array_v02
};
static const uint32_t qmiLocAddCircularGeofenceIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* transactionId */
    QMI_SI_TLV_SET(0x11)      /* geofenceId */
};

static const qmi_implemented_optionals qmiLocAddCircularGeofenceIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocAddCircularGeofenceIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_add_circular_geofence_reqresp_impl_tuple_v02 = 
{
  &qmiLocAddCircularGeofenceReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocAddCircularGeofenceIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocDeleteGeofenceReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocDeleteGeofenceIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geofenceId */
    QMI_SI_TLV_SET(0x11)      /* transactionId */
};

static const qmi_implemented_optionals qmiLocDeleteGeofenceIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocDeleteGeofenceIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_delete_geofence_reqresp_impl_tuple_v02 = 
{
  &qmiLocDeleteGeofenceReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocDeleteGeofenceIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocQueryGeofenceReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocQueryGeofenceIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geofenceId */
    QMI_SI_TLV_SET(0x11)   |  /* transactionId */
    QMI_SI_TLV_SET(0x12)   |  /* geofenceOrigin */
    QMI_SI_TLV_SET(0x13)   |  /* posWrtGeofence */
    QMI_SI_TLV_SET(0x14)   |  /* circularGeofenceArgs */
    QMI_SI_TLV_SET(0x15)      /* geofenceState */
};

static const qmi_implemented_optionals qmiLocQueryGeofenceIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocQueryGeofenceIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_query_geofence_reqresp_impl_tuple_v02 = 
{
  &qmiLocQueryGeofenceReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocQueryGeofenceIndMsgT_impl_v02
};

static const uint32_t qmiLocEditGeofenceReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geofenceState */
    QMI_SI_TLV_SET(0x11)   |  /* breachMask */
    QMI_SI_TLV_SET(0x12)      /* responsiveness */
};

static const qmi_implemented_optionals qmiLocEditGeofenceReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEditGeofenceReqMsgT_impl_array_v02
};
static const uint32_t qmiLocEditGeofenceIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geofenceId */
    QMI_SI_TLV_SET(0x11)   |  /* transactionId */
    QMI_SI_TLV_SET(0x12)      /* failedParams */
};

static const qmi_implemented_optionals qmiLocEditGeofenceIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEditGeofenceIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_edit_geofence_reqresp_impl_tuple_v02 = 
{
  &qmiLocEditGeofenceReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocEditGeofenceIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGetBestAvailablePositionReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocGetBestAvailablePositionIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* transactionId */
    QMI_SI_TLV_SET(0x11)   |  /* latitude */
    QMI_SI_TLV_SET(0x12)   |  /* longitude */
    QMI_SI_TLV_SET(0x13)   |  /* horUncCircular */
    QMI_SI_TLV_SET(0x14)   |  /* altitudeWrtEllipsoid */
    QMI_SI_TLV_SET(0x15)   |  /* vertUnc */
    QMI_SI_TLV_SET(0x16)   |  /* timestampUtc */
    QMI_SI_TLV_SET(0x17)   |  /* timeUnc */
    QMI_SI_TLV_SET(0x18)   |  /* horUncEllipseSemiMinor */
    QMI_SI_TLV_SET(0x19)   |  /* horUncEllipseSemiMajor */
    QMI_SI_TLV_SET(0x1A)   |  /* horUncEllipseOrientAzimuth */
    QMI_SI_TLV_SET(0x1B)   |  /* horCircularConfidence */
    QMI_SI_TLV_SET(0x1C)   |  /* horEllipticalConfidence */
    QMI_SI_TLV_SET(0x1D)   |  /* horReliability */
    QMI_SI_TLV_SET(0x1E)   |  /* horSpeed */
    QMI_SI_TLV_SET(0x1F)   |  /* horSpeedUnc */
    QMI_SI_TLV_SET(0x20)   |  /* altitudeWrtMeanSeaLevel */
    QMI_SI_TLV_SET(0x21)   |  /* vertConfidence */
    QMI_SI_TLV_SET(0x22)   |  /* vertReliability */
    QMI_SI_TLV_SET(0x23)   |  /* vertSpeed */
    QMI_SI_TLV_SET(0x24)   |  /* vertSpeedUnc */
    QMI_SI_TLV_SET(0x25)   |  /* heading */
    QMI_SI_TLV_SET(0x26)   |  /* headingUnc */
    QMI_SI_TLV_SET(0x27)   |  /* magneticDeviation */
    QMI_SI_TLV_SET(0x28)   |  /* technologyMask */
    QMI_SI_TLV_SET(0x29)   |  /* DOP */
    QMI_SI_TLV_SET(0x2A)   |  /* gpsTime */
    QMI_SI_TLV_SET(0x2B)   |  /* timeSrc */
    QMI_SI_TLV_SET(0x2C)   |  /* sensorDataUsage */
    QMI_SI_TLV_SET(0x2D)      /* gnssSvUsedList */
};

static const qmi_implemented_optionals qmiLocGetBestAvailablePositionIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetBestAvailablePositionIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_best_available_position_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetBestAvailablePositionReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetBestAvailablePositionIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectMotionDataReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectMotionDataIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_motion_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectMotionDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectMotionDataIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGetNiGeofenceIdListReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocGetNiGeofenceIdListIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* transactionId */
    QMI_SI_TLV_SET(0x11)      /* niGeofenceIdList */
};

static const qmi_implemented_optionals qmiLocGetNiGeofenceIdListIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetNiGeofenceIdListIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_ni_geofence_id_list_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetNiGeofenceIdListReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetNiGeofenceIdListIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectGSMCellInfoReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* timingAdvance */
};

static const qmi_implemented_optionals qmiLocInjectGSMCellInfoReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectGSMCellInfoReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocInjectGSMCellInfoIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_gsm_cell_info_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectGSMCellInfoReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectGSMCellInfoIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectNetworkInitiatedMessageReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectNetworkInitiatedMessageIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_network_initiated_message_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectNetworkInitiatedMessageReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectNetworkInitiatedMessageIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocWWANOutOfServiceNotificationIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_wwan_out_of_service_notification_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocWWANOutOfServiceNotificationIndMsgT_impl_v02
};

static const uint32_t qmiLocEventPedometerControlIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* resetStepCount */
    QMI_SI_TLV_SET(0x11)      /* stepCountThreshold */
};

static const qmi_implemented_optionals qmiLocEventPedometerControlIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventPedometerControlIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_pedometer_control_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventPedometerControlIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventMotionDataControlIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_motion_data_control_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventMotionDataControlIndMsgT_impl_v02
};

static const uint32_t qmiLocPedometerReportReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* stepConfidence */
    QMI_SI_TLV_SET(0x11)   |  /* stepCountUncertainty */
    QMI_SI_TLV_SET(0x12)      /* stepRate */
};

static const qmi_implemented_optionals qmiLocPedometerReportReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocPedometerReportReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocPedometerReportIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_pedometer_report_reqresp_impl_tuple_v02 = 
{
  &qmiLocPedometerReportReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocPedometerReportIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectWCDMACellInfoReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* freq */
    QMI_SI_TLV_SET(0x11)      /* psc */
};

static const qmi_implemented_optionals qmiLocInjectWCDMACellInfoReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectWCDMACellInfoReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocInjectWCDMACellInfoIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_wcdma_cell_info_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectWCDMACellInfoReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectWCDMACellInfoIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectTDSCDMACellInfoReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* freq */
};

static const qmi_implemented_optionals qmiLocInjectTDSCDMACellInfoReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectTDSCDMACellInfoReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocInjectTDSCDMACellInfoIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_tdscdma_cell_info_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectTDSCDMACellInfoReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectTDSCDMACellInfoIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectSubscriberIDReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* preferredIMSI */
    QMI_SI_TLV_SET(0x11)      /* preferredMSISDN */
};

static const qmi_implemented_optionals qmiLocInjectSubscriberIDReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectSubscriberIDReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocInjectSubscriberIDIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_subscriber_id_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectSubscriberIDReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectSubscriberIDIndMsgT_impl_v02
};

static const uint32_t qmiLocSetGeofenceEngineConfigReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* gnssUnavailableIndicationTimeout */
    QMI_SI_TLV_SET(0x11)   |  /* maxGeofences */
    QMI_SI_TLV_SET(0x12)   |  /* enableMotionDetectionSources */
    QMI_SI_TLV_SET(0x13)   |  /* enableCpiUsage */
    QMI_SI_TLV_SET(0x14)   |  /* gnssPositionSessionTimeout */
    QMI_SI_TLV_SET(0x15)   |  /* gnssPositionMaxPuncAcceptable */
    QMI_SI_TLV_SET(0x16)   |  /* mediumResponsivenessValue */
    QMI_SI_TLV_SET(0x17)   |  /* chalGnssEnvMinCpiWaitInterval */
    QMI_SI_TLV_SET(0x18)      /* motionStateInfo */
};

static const qmi_implemented_optionals qmiLocSetGeofenceEngineConfigReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetGeofenceEngineConfigReqMsgT_impl_array_v02
};
static const uint32_t qmiLocSetGeofenceEngineConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* transactionId */
};

static const qmi_implemented_optionals qmiLocSetGeofenceEngineConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetGeofenceEngineConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_set_geofence_engine_config_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetGeofenceEngineConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetGeofenceEngineConfigIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGetGeofenceEngineConfigReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocGetGeofenceEngineConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* transactionId */
    QMI_SI_TLV_SET(0x11)   |  /* gnssUnavailableIndicationTimeout */
    QMI_SI_TLV_SET(0x12)   |  /* maxGeofences */
    QMI_SI_TLV_SET(0x13)   |  /* enabledMotionDetectionSources */
    QMI_SI_TLV_SET(0x14)      /* enabledCpiUsage */
};

static const qmi_implemented_optionals qmiLocGetGeofenceEngineConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetGeofenceEngineConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_geofence_engine_config_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetGeofenceEngineConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetGeofenceEngineConfigIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGetBatchSizeReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocGetBatchSizeIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_get_batch_size_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetBatchSizeReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetBatchSizeIndMsgT_impl_v02
};

static const uint32_t qmiLocStartBatchingReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* minInterval */
    QMI_SI_TLV_SET(0x11)   |  /* horizontalAccuracyLevel */
    QMI_SI_TLV_SET(0x12)   |  /* fixSessionTimeout */
    QMI_SI_TLV_SET(0x13)   |  /* minDistance */
    QMI_SI_TLV_SET(0x14)   |  /* batchAllPos */
    QMI_SI_TLV_SET(0x15)      /* requestId */
};

static const qmi_implemented_optionals qmiLocStartBatchingReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocStartBatchingReqMsgT_impl_array_v02
};
static const uint32_t qmiLocStartBatchingIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* requestId */
};

static const qmi_implemented_optionals qmiLocStartBatchingIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocStartBatchingIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_start_batching_reqresp_impl_tuple_v02 = 
{
  &qmiLocStartBatchingReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocStartBatchingIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventBatchFullIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_batch_full_notification_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventBatchFullIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventLiveBatchedPositionReportIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_live_batched_position_report_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventLiveBatchedPositionReportIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocReadFromBatchReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocReadFromBatchIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* numberOfEntries */
    QMI_SI_TLV_SET(0x11)      /* batchedReportList */
};

static const qmi_implemented_optionals qmiLocReadFromBatchIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocReadFromBatchIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_read_from_batch_reqresp_impl_tuple_v02 = 
{
  &qmiLocReadFromBatchReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocReadFromBatchIndMsgT_impl_v02
};

static const uint32_t qmiLocStopBatchingReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* requestId */
};

static const qmi_implemented_optionals qmiLocStopBatchingReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocStopBatchingReqMsgT_impl_array_v02
};
static const uint32_t qmiLocStopBatchingIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* requestId */
};

static const qmi_implemented_optionals qmiLocStopBatchingIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocStopBatchingIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_stop_batching_reqresp_impl_tuple_v02 = 
{
  &qmiLocStopBatchingReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocStopBatchingIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocReleaseBatchReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocReleaseBatchIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_release_batch_reqresp_impl_tuple_v02 = 
{
  &qmiLocReleaseBatchReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocReleaseBatchIndMsgT_impl_v02
};

static const qmi_implemented_optionals_tuple qmi_loc_event_inject_wifi_ap_data_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals qmiLocInjectWifiApDataReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectWifiApDataIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_wifi_ap_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectWifiApDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectWifiApDataIndMsgT_impl_v02
};

static const uint32_t qmiLocNotifyWifiAttachmentStatusReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* accessPointMacAddress */
    QMI_SI_TLV_SET(0x11)      /* wifiApSsid */
};

static const qmi_implemented_optionals qmiLocNotifyWifiAttachmentStatusReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocNotifyWifiAttachmentStatusReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocNotifyWifiAttachmentStatusIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_notify_wifi_attachment_status_reqresp_impl_tuple_v02 = 
{
  &qmiLocNotifyWifiAttachmentStatusReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocNotifyWifiAttachmentStatusIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocNotifyWifiEnabledStatusReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocNotifyWifiEnabledStatusIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_notify_wifi_enabled_status_reqresp_impl_tuple_v02 = 
{
  &qmiLocNotifyWifiEnabledStatusReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocNotifyWifiEnabledStatusIndMsgT_impl_v02
};

static const uint32_t qmiLocEventGeofenceBatchedBreachIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geofenceIdContinuousList */
    QMI_SI_TLV_SET(0x11)   |  /* geofenceIdDiscreteList */
    QMI_SI_TLV_SET(0x12)   |  /* geofencePosition */
    QMI_SI_TLV_SET(0x13)   |  /* breachConfidence */
    QMI_SI_TLV_SET(0x14)   |  /* headingUnc */
    QMI_SI_TLV_SET(0x15)   |  /* vertUnc */
    QMI_SI_TLV_SET(0x16)   |  /* speedUnc */
    QMI_SI_TLV_SET(0x17)   |  /* horConfidence */
    QMI_SI_TLV_SET(0x18)   |  /* vertConfidence */
    QMI_SI_TLV_SET(0x19)   |  /* DOP */
    QMI_SI_TLV_SET(0x1A)      /* gnssSvUsedList */
};

static const qmi_implemented_optionals qmiLocEventGeofenceBatchedBreachIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventGeofenceBatchedBreachIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_geofence_batched_breach_notification_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGeofenceBatchedBreachIndMsgT_impl_v02
};

static const uint32_t qmiLocEventVehicleDataReadyIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* vehicleAccelReadyStatus */
    QMI_SI_TLV_SET(0x11)   |  /* vehicleAngularRateReadyStatus */
    QMI_SI_TLV_SET(0x12)      /* vehicleOdometryReadyStatus */
};

static const qmi_implemented_optionals qmiLocEventVehicleDataReadyIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventVehicleDataReadyIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_vehicle_data_ready_status_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventVehicleDataReadyIndMsgT_impl_v02
};

static const uint32_t qmiLocInjectVehicleSensorDataReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* accelData */
    QMI_SI_TLV_SET(0x11)   |  /* angRotationData */
    QMI_SI_TLV_SET(0x12)   |  /* odometryData */
    QMI_SI_TLV_SET(0x13)      /* changeInTimeScales */
};

static const qmi_implemented_optionals qmiLocInjectVehicleSensorDataReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocInjectVehicleSensorDataReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocInjectVehicleSensorDataIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_vehicle_sensor_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectVehicleSensorDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectVehicleSensorDataIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGetAvailWwanPositionReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocGetAvailWwanPositionIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* transactionId */
    QMI_SI_TLV_SET(0x11)   |  /* latitude */
    QMI_SI_TLV_SET(0x12)   |  /* longitude */
    QMI_SI_TLV_SET(0x13)   |  /* horUncCircular */
    QMI_SI_TLV_SET(0x14)   |  /* altitudeWrtEllipsoid */
    QMI_SI_TLV_SET(0x15)   |  /* vertUnc */
    QMI_SI_TLV_SET(0x16)   |  /* timestampUtc */
    QMI_SI_TLV_SET(0x17)   |  /* timeUnc */
    QMI_SI_TLV_SET(0x18)   |  /* horUncEllipseSemiMinor */
    QMI_SI_TLV_SET(0x19)   |  /* horUncEllipseSemiMajor */
    QMI_SI_TLV_SET(0x1A)   |  /* horUncEllipseOrientAzimuth */
    QMI_SI_TLV_SET(0x1B)   |  /* horCircularConfidence */
    QMI_SI_TLV_SET(0x1C)   |  /* horEllipticalConfidence */
    QMI_SI_TLV_SET(0x1D)   |  /* horReliability */
    QMI_SI_TLV_SET(0x1E)   |  /* altitudeWrtMeanSeaLevel */
    QMI_SI_TLV_SET(0x1F)   |  /* vertConfidence */
    QMI_SI_TLV_SET(0x20)   |  /* vertReliability */
    QMI_SI_TLV_SET(0x21)   |  /* gpsTime */
    QMI_SI_TLV_SET(0x22)      /* timeSrc */
};

static const qmi_implemented_optionals qmiLocGetAvailWwanPositionIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocGetAvailWwanPositionIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_get_available_wwan_position_reqresp_impl_tuple_v02 = 
{
  &qmiLocGetAvailWwanPositionReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGetAvailWwanPositionIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetPremiumServicesCfgReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetPremiumServicesCfgIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_premium_services_config_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetPremiumServicesCfgReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetPremiumServicesCfgIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSetXtraVersionCheckReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocSetXtraVersionCheckIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_xtra_version_check_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetXtraVersionCheckReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetXtraVersionCheckIndMsgT_impl_v02
};

static const uint32_t qmiLocEventGnssSvMeasInfoIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* rcvrClockFrequencyInfo */
    QMI_SI_TLV_SET(0x11)   |  /* leapSecondInfo */
    QMI_SI_TLV_SET(0x12)   |  /* gpsGloInterSystemBias */
    QMI_SI_TLV_SET(0x13)   |  /* gpsBdsInterSystemBias */
    QMI_SI_TLV_SET(0x14)   |  /* gpsGalInterSystemBias */
    QMI_SI_TLV_SET(0x15)   |  /* bdsGloInterSystemBias */
    QMI_SI_TLV_SET(0x16)   |  /* galGloInterSystemBias */
    QMI_SI_TLV_SET(0x17)   |  /* galBdsInterSystemBias */
    QMI_SI_TLV_SET(0x18)   |  /* systemTime */
    QMI_SI_TLV_SET(0x19)   |  /* gloTime */
    QMI_SI_TLV_SET(0x1A)   |  /* systemTimeExt */
    QMI_SI_TLV_SET(0x1B)      /* svMeasurement */
};

static const qmi_implemented_optionals qmiLocEventGnssSvMeasInfoIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventGnssSvMeasInfoIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_gnss_measurement_report_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGnssSvMeasInfoIndMsgT_impl_v02
};

static const uint32_t qmiLocEventGnssSvPolyIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* polyCoeffXYZ0 */
    QMI_SI_TLV_SET(0x11)   |  /* polyCoefXYZN */
    QMI_SI_TLV_SET(0x12)   |  /* polyCoefClockBias */
    QMI_SI_TLV_SET(0x13)   |  /* gloFrequency */
    QMI_SI_TLV_SET(0x14)   |  /* IODE */
    QMI_SI_TLV_SET(0x15)   |  /* enhancedIOD */
    QMI_SI_TLV_SET(0x16)   |  /* svPosUnc */
    QMI_SI_TLV_SET(0x17)   |  /* ionoDelay */
    QMI_SI_TLV_SET(0x18)   |  /* ionoDot */
    QMI_SI_TLV_SET(0x19)   |  /* sbasIonoDelay */
    QMI_SI_TLV_SET(0x1A)   |  /* sbasIonoDot */
    QMI_SI_TLV_SET(0x1B)   |  /* tropoDelay */
    QMI_SI_TLV_SET(0x1C)   |  /* elevation */
    QMI_SI_TLV_SET(0x1D)   |  /* elevationDot */
    QMI_SI_TLV_SET(0x1E)   |  /* elenationUnc */
    QMI_SI_TLV_SET(0x1F)      /* velCoef */
};

static const qmi_implemented_optionals qmiLocEventGnssSvPolyIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventGnssSvPolyIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_sv_polynomial_report_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGnssSvPolyIndMsgT_impl_v02
};

static const uint32_t qmiLocSetGNSSConstRepConfigReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* measReportConfig */
    QMI_SI_TLV_SET(0x11)      /* svPolyReportConfig */
};

static const qmi_implemented_optionals qmiLocSetGNSSConstRepConfigReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetGNSSConstRepConfigReqMsgT_impl_array_v02
};
static const qmi_implemented_optionals qmiLocSetGNSSConstRepConfigIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_set_gnss_constell_report_config_impl_tuple_v02 = 
{
  &qmiLocSetGNSSConstRepConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetGNSSConstRepConfigIndMsgT_impl_v02
};

static const uint32_t qmiLocAddGeofenceContextReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geofenceId */
    QMI_SI_TLV_SET(0x11)   |  /* wifiApSsidInfo */
    QMI_SI_TLV_SET(0x12)   |  /* wifiApMacAddressList */
    QMI_SI_TLV_SET(0x13)   |  /* tdsCdmaCellIDList */
    QMI_SI_TLV_SET(0x14)   |  /* wcdmaCellIDList */
    QMI_SI_TLV_SET(0x15)   |  /* gsmCellIDList */
    QMI_SI_TLV_SET(0x16)      /* iBeaconList */
};

static const qmi_implemented_optionals qmiLocAddGeofenceContextReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocAddGeofenceContextReqMsgT_impl_array_v02
};
static const uint32_t qmiLocAddGeofenceContextIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* transactionId */
    QMI_SI_TLV_SET(0x11)   |  /* geofenceId */
    QMI_SI_TLV_SET(0x12)      /* contextId */
};

static const qmi_implemented_optionals qmiLocAddGeofenceContextIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocAddGeofenceContextIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_add_geofence_context_reqresp_impl_tuple_v02 = 
{
  &qmiLocAddGeofenceContextReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocAddGeofenceContextIndMsgT_impl_v02
};

static const uint32_t qmiLocSetGeofenceEngineContextReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* utcTimeOfDay */
    QMI_SI_TLV_SET(0x11)      /* temperature */
};

static const qmi_implemented_optionals qmiLocSetGeofenceEngineContextReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetGeofenceEngineContextReqMsgT_impl_array_v02
};
static const uint32_t qmiLocSetGeofenceEngineContextIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* transactionId */
};

static const qmi_implemented_optionals qmiLocSetGeofenceEngineContextIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSetGeofenceEngineContextIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_set_geofence_engine_context_reqresp_impl_tuple_v02 = 
{
  &qmiLocSetGeofenceEngineContextReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSetGeofenceEngineContextIndMsgT_impl_v02
};

static const uint32_t qmiLocDeleteGeofenceContextReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* contextId */
};

static const qmi_implemented_optionals qmiLocDeleteGeofenceContextReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocDeleteGeofenceContextReqMsgT_impl_array_v02
};
static const uint32_t qmiLocDeleteGeofenceContextIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* transactionId */
    QMI_SI_TLV_SET(0x11)   |  /* geofenceId */
    QMI_SI_TLV_SET(0x12)      /* contextId */
};

static const qmi_implemented_optionals qmiLocDeleteGeofenceContextIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocDeleteGeofenceContextIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_delete_geofence_context_reqresp_impl_tuple_v02 = 
{
  &qmiLocDeleteGeofenceContextReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocDeleteGeofenceContextIndMsgT_impl_v02
};

static const uint32_t qmiLocEventGeofenceProximityIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* contextId */
};

static const qmi_implemented_optionals qmiLocEventGeofenceProximityIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventGeofenceProximityIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_geofence_proximity_notification_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGeofenceProximityIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectGtpClientDownloadedDataReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectGtpClientDownloadedDataIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_gtp_client_downloaded_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectGtpClientDownloadedDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectGtpClientDownloadedDataIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGdtUploadBeginStatusReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocGdtUploadBeginStatusIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_gdt_upload_begin_status_reqresp_impl_tuple_v02 = 
{
  &qmiLocGdtUploadBeginStatusReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGdtUploadBeginStatusIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocGdtUploadEndReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocGdtUploadEndIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_gdt_upload_end_reqresp_impl_tuple_v02 = 
{
  &qmiLocGdtUploadEndReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocGdtUploadEndIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventGdtUploadBeginStatusReqIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_gdt_upload_begin_status_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGdtUploadBeginStatusReqIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventGdtUploadEndReqIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_gdt_upload_end_reqresp_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGdtUploadEndReqIndMsgT_impl_v02
};

static const uint32_t qmiLocStartDbtReqMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* maxLatency */
    QMI_SI_TLV_SET(0x11)      /* usageType */
};

static const qmi_implemented_optionals qmiLocStartDbtReqMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocStartDbtReqMsgT_impl_array_v02
};
static const uint32_t qmiLocStartDbtIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* reqId */
};

static const qmi_implemented_optionals qmiLocStartDbtIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocStartDbtIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_start_dbt_reqresp_impl_tuple_v02 = 
{
  &qmiLocStartDbtReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocStartDbtIndMsgT_impl_v02
};

static const uint32_t qmiLocEventDbtPositionReportIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* headingUnc */
    QMI_SI_TLV_SET(0x11)   |  /* speedUnc */
    QMI_SI_TLV_SET(0x12)   |  /* horConfidence */
    QMI_SI_TLV_SET(0x13)   |  /* vertConfidence */
    QMI_SI_TLV_SET(0x14)   |  /* DOP */
    QMI_SI_TLV_SET(0x15)   |  /* gnssSvUsedList */
    QMI_SI_TLV_SET(0x16)      /* positionSrc */
};

static const qmi_implemented_optionals qmiLocEventDbtPositionReportIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventDbtPositionReportIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_dbt_position_report_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventDbtPositionReportIndMsgT_impl_v02
};

static const uint32_t qmiLocEventDbtSessionStatusIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* reqId */
};

static const qmi_implemented_optionals qmiLocEventDbtSessionStatusIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventDbtSessionStatusIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_dbt_session_status_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventDbtSessionStatusIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocStopDbtReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocStopDbtIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* reqId */
};

static const qmi_implemented_optionals qmiLocStopDbtIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocStopDbtIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_stop_dbt_reqresp_impl_tuple_v02 = 
{
  &qmiLocStopDbtReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocStopDbtIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocSecureGetAvailablePositionReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocSecureGetAvailablePositionIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* qmilocSecureGetAvailablePositionInd */
};

static const qmi_implemented_optionals qmiLocSecureGetAvailablePositionIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocSecureGetAvailablePositionIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_secure_get_available_position_reqresp_impl_tuple_v02 = 
{
  &qmiLocSecureGetAvailablePositionReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocSecureGetAvailablePositionIndMsgT_impl_v02
};

static const uint32_t qmiLocEventGeofenceBatchedDwellIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geofenceIdContinuousList */
    QMI_SI_TLV_SET(0x11)   |  /* geofenceIdDiscreteList */
    QMI_SI_TLV_SET(0x12)   |  /* geofencePosition */
    QMI_SI_TLV_SET(0x13)   |  /* headingUnc */
    QMI_SI_TLV_SET(0x14)   |  /* vertUnc */
    QMI_SI_TLV_SET(0x15)   |  /* speedUnc */
    QMI_SI_TLV_SET(0x16)   |  /* horConfidence */
    QMI_SI_TLV_SET(0x17)   |  /* vertConfidence */
    QMI_SI_TLV_SET(0x18)   |  /* DOP */
    QMI_SI_TLV_SET(0x19)      /* gnssSvUsedList */
};

static const qmi_implemented_optionals qmiLocEventGeofenceBatchedDwellIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocEventGeofenceBatchedDwellIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_event_geofence_batched_dwell_notification_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGeofenceBatchedDwellIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventGetTimeZoneReqIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_get_time_zone_info_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventGetTimeZoneReqIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectTimeZoneInfoReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectTimeZoneInfoIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_time_zone_info_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectTimeZoneInfoReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectTimeZoneInfoIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectApCacheDataReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectApCacheDataIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_apcache_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectApCacheDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectApCacheDataIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocInjectApDoNotCacheDataReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmiLocInjectApDoNotCacheDataIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_inject_apdonotcache_data_reqresp_impl_tuple_v02 = 
{
  &qmiLocInjectApDoNotCacheDataReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocInjectApDoNotCacheDataIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocEventBatchingStatusIndMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_loc_event_batching_status_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &qmiLocEventBatchingStatusIndMsgT_impl_v02
};

static const qmi_implemented_optionals qmiLocQueryAonConfigReqMsgT_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmiLocQueryAonConfigIndMsgT_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* transactionId */
    QMI_SI_TLV_SET(0x11)      /* aonCapability */
};

static const qmi_implemented_optionals qmiLocQueryAonConfigIndMsgT_impl_v02 =
{
  1, /* number of elements in array */
  qmiLocQueryAonConfigIndMsgT_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_loc_query_aon_config_reqresp_impl_tuple_v02 = 
{
  &qmiLocQueryAonConfigReqMsgT_impl_v02,
  &qmiLocGenRespMsgT_impl_v02,
  &qmiLocQueryAonConfigIndMsgT_impl_v02
};

static const qmi_implemented_optionals_tuple * const loc_service_implemented_array_v02[] =
{
    NULL,                                                /* 0x0000 */
    NULL,                                                /* 0x0001 */
    NULL,                                                /* 0x0002 */
    NULL,                                                /* 0x0003 */
    NULL,                                                /* 0x0004 */
    NULL,                                                /* 0x0005 */
    NULL,                                                /* 0x0006 */
    NULL,                                                /* 0x0007 */
    NULL,                                                /* 0x0008 */
    NULL,                                                /* 0x0009 */
    NULL,                                                /* 0x000A */
    NULL,                                                /* 0x000B */
    NULL,                                                /* 0x000C */
    NULL,                                                /* 0x000D */
    NULL,                                                /* 0x000E */
    NULL,                                                /* 0x000F */
    NULL,                                                /* 0x0010 */
    NULL,                                                /* 0x0011 */
    NULL,                                                /* 0x0012 */
    NULL,                                                /* 0x0013 */
    NULL,                                                /* 0x0014 */
    NULL,                                                /* 0x0015 */
    NULL,                                                /* 0x0016 */
    NULL,                                                /* 0x0017 */
    NULL,                                                /* 0x0018 */
    NULL,                                                /* 0x0019 */
    NULL,                                                /* 0x001A */
    NULL,                                                /* 0x001B */
    NULL,                                                /* 0x001C */
    NULL,                                                /* 0x001D */
    &qmi_loc_get_supported_msgs_reqresp_impl_tuple_v02,  /* 0x001E */
    &qmi_loc_get_supported_fields_reqresp_impl_tuple_v02, /* 0x001F */
    &qmi_loc_inform_client_revision_reqresp_impl_tuple_v02, /* 0x0020 */
    &qmi_loc_reg_events_reqresp_impl_tuple_v02,          /* 0x0021 */
    &qmi_loc_start_reqresp_impl_tuple_v02,               /* 0x0022 */
    &qmi_loc_stop_reqresp_impl_tuple_v02,                /* 0x0023 */
    &qmi_loc_event_position_report_impl_tuple_v02,       /* 0x0024 */
    &qmi_loc_event_gnss_sv_info_impl_tuple_v02,          /* 0x0025 */
    &qmi_loc_event_nmea_impl_tuple_v02,                  /* 0x0026 */
    &qmi_loc_event_ni_notify_verify_reqresp_impl_tuple_v02, /* 0x0027 */
    &qmi_loc_event_inject_time_reqresp_impl_tuple_v02,   /* 0x0028 */
    &qmi_loc_event_inject_predicted_orbits_reqresp_impl_tuple_v02, /* 0x0029 */
    &qmi_loc_event_inject_position_reqresp_impl_tuple_v02, /* 0x002A */
    &qmi_loc_event_engine_state_impl_tuple_v02,          /* 0x002B */
    &qmi_loc_event_fix_session_state_impl_tuple_v02,     /* 0x002C */
    &qmi_loc_event_wifi_reqresp_impl_tuple_v02,          /* 0x002D */
    &qmi_loc_event_sensor_streaming_ready_status_impl_tuple_v02, /* 0x002E */
    &qmi_loc_event_time_sync_reqresp_impl_tuple_v02,     /* 0x002F */
    &qmi_loc_event_set_spi_streaming_report_impl_tuple_v02, /* 0x0030 */
    &qmi_loc_event_location_server_connection_reqresp_impl_tuple_v02, /* 0x0031 */
    &qmi_loc_get_service_revision_reqresp_impl_tuple_v02, /* 0x0032 */
    &qmi_loc_get_fix_criteria_reqresp_impl_tuple_v02,    /* 0x0033 */
    &qmi_loc_ni_useronse_reqresp_impl_tuple_v02,         /* 0x0034 */
    &qmi_loc_inject_predicted_orbits_data_reqresp_impl_tuple_v02, /* 0x0035 */
    &qmi_loc_get_predicted_orbits_data_source_reqresp_impl_tuple_v02, /* 0x0036 */
    &qmi_loc_get_predicted_orbits_data_validity_reqresp_impl_tuple_v02, /* 0x0037 */
    &qmi_loc_inject_utc_time_reqresp_impl_tuple_v02,     /* 0x0038 */
    &qmi_loc_inject_position_reqresp_impl_tuple_v02,     /* 0x0039 */
    &qmi_loc_set_engine_lock_reqresp_impl_tuple_v02,     /* 0x003A */
    &qmi_loc_get_engine_lock_reqresp_impl_tuple_v02,     /* 0x003B */
    &qmi_loc_set_sbas_config_reqresp_impl_tuple_v02,     /* 0x003C */
    &qmi_loc_get_sbas_config_reqresp_impl_tuple_v02,     /* 0x003D */
    &qmi_loc_set_nmea_types_reqresp_impl_tuple_v02,      /* 0x003E */
    &qmi_loc_get_nmea_types_reqresp_impl_tuple_v02,      /* 0x003F */
    &qmi_loc_set_low_power_mode_reqresp_impl_tuple_v02,  /* 0x0040 */
    &qmi_loc_get_low_power_mode_reqresp_impl_tuple_v02,  /* 0x0041 */
    &qmi_loc_set_server_reqresp_impl_tuple_v02,          /* 0x0042 */
    &qmi_loc_get_server_reqresp_impl_tuple_v02,          /* 0x0043 */
    &qmi_loc_delete_assist_data_reqresp_impl_tuple_v02,  /* 0x0044 */
    &qmi_loc_set_xtra_t_session_control_reqresp_impl_tuple_v02, /* 0x0045 */
    &qmi_loc_get_xtra_t_session_control_reqresp_impl_tuple_v02, /* 0x0046 */
    &qmi_loc_inject_wifi_position_reqresp_impl_tuple_v02, /* 0x0047 */
    &qmi_loc_notify_wifi_status_reqresp_impl_tuple_v02,  /* 0x0048 */
    &qmi_loc_get_registered_events_reqresp_impl_tuple_v02, /* 0x0049 */
    &qmi_loc_set_operation_mode_reqresp_impl_tuple_v02,  /* 0x004A */
    &qmi_loc_get_operation_mode_reqresp_impl_tuple_v02,  /* 0x004B */
    &qmi_loc_set_spi_status_reqresp_impl_tuple_v02,      /* 0x004C */
    &qmi_loc_inject_sensor_data_reqresp_impl_tuple_v02,  /* 0x004D */
    &qmi_loc_inject_time_sync_data_reqresp_impl_tuple_v02, /* 0x004E */
    &qmi_loc_set_cradle_mount_config_reqresp_impl_tuple_v02, /* 0x004F */
    &qmi_loc_get_cradle_mount_config_reqresp_impl_tuple_v02, /* 0x0050 */
    &qmi_loc_set_external_power_config_reqresp_impl_tuple_v02, /* 0x0051 */
    &qmi_loc_get_external_power_config_reqresp_impl_tuple_v02, /* 0x0052 */
    &qmi_loc_inform_location_server_conn_status_reqresp_impl_tuple_v02, /* 0x0053 */
    &qmi_loc_set_protocol_config_parameters_reqresp_impl_tuple_v02, /* 0x0054 */
    &qmi_loc_get_protocol_config_parameters_reqresp_impl_tuple_v02, /* 0x0055 */
    &qmi_loc_set_sensor_control_config_reqresp_impl_tuple_v02, /* 0x0056 */
    &qmi_loc_get_sensor_control_config_reqresp_impl_tuple_v02, /* 0x0057 */
    &qmi_loc_set_sensor_properties_reqresp_impl_tuple_v02, /* 0x0058 */
    &qmi_loc_get_sensor_properties_reqresp_impl_tuple_v02, /* 0x0059 */
    &qmi_loc_set_sensor_performance_control_configuration_reqresp_impl_tuple_v02, /* 0x005A */
    &qmi_loc_get_sensor_performance_control_configuration_reqresp_impl_tuple_v02, /* 0x005B */
    &qmi_loc_inject_supl_certificate_reqresp_impl_tuple_v02, /* 0x005C */
    &qmi_loc_delete_supl_certificate_reqresp_impl_tuple_v02, /* 0x005D */
    &qmi_loc_set_position_engine_config_parameters_reqresp_impl_tuple_v02, /* 0x005E */
    &qmi_loc_get_position_engine_config_parameters_reqresp_impl_tuple_v02, /* 0x005F */
    &qmi_loc_event_ni_geofence_notification_impl_tuple_v02, /* 0x0060 */
    &qmi_loc_event_geofence_gen_alert_impl_tuple_v02,    /* 0x0061 */
    &qmi_loc_event_geofence_breach_notification_impl_tuple_v02, /* 0x0062 */
    &qmi_loc_add_circular_geofence_reqresp_impl_tuple_v02, /* 0x0063 */
    &qmi_loc_delete_geofence_reqresp_impl_tuple_v02,     /* 0x0064 */
    &qmi_loc_query_geofence_reqresp_impl_tuple_v02,      /* 0x0065 */
    &qmi_loc_edit_geofence_reqresp_impl_tuple_v02,       /* 0x0066 */
    &qmi_loc_get_best_available_position_reqresp_impl_tuple_v02, /* 0x0067 */
    &qmi_loc_inject_motion_data_reqresp_impl_tuple_v02,  /* 0x0068 */
    &qmi_loc_get_ni_geofence_id_list_reqresp_impl_tuple_v02, /* 0x0069 */
    &qmi_loc_inject_gsm_cell_info_reqresp_impl_tuple_v02, /* 0x006A */
    &qmi_loc_inject_network_initiated_message_reqresp_impl_tuple_v02, /* 0x006B */
    &qmi_loc_wwan_out_of_service_notification_reqresp_impl_tuple_v02, /* 0x006C */
    &qmi_loc_event_pedometer_control_impl_tuple_v02,     /* 0x006D */
    &qmi_loc_event_motion_data_control_impl_tuple_v02,   /* 0x006E */
    &qmi_loc_pedometer_report_reqresp_impl_tuple_v02,    /* 0x006F */
    &qmi_loc_inject_wcdma_cell_info_reqresp_impl_tuple_v02, /* 0x0070 */
    &qmi_loc_inject_tdscdma_cell_info_reqresp_impl_tuple_v02, /* 0x0071 */
    &qmi_loc_inject_subscriber_id_reqresp_impl_tuple_v02, /* 0x0072 */
    &qmi_loc_set_geofence_engine_config_reqresp_impl_tuple_v02, /* 0x0073 */
    &qmi_loc_get_geofence_engine_config_reqresp_impl_tuple_v02, /* 0x0074 */
    &qmi_loc_get_batch_size_reqresp_impl_tuple_v02,      /* 0x0075 */
    &qmi_loc_start_batching_reqresp_impl_tuple_v02,      /* 0x0076 */
    &qmi_loc_event_batch_full_notification_impl_tuple_v02, /* 0x0077 */
    &qmi_loc_event_live_batched_position_report_impl_tuple_v02, /* 0x0078 */
    &qmi_loc_read_from_batch_reqresp_impl_tuple_v02,     /* 0x0079 */
    &qmi_loc_stop_batching_reqresp_impl_tuple_v02,       /* 0x007A */
    &qmi_loc_release_batch_reqresp_impl_tuple_v02,       /* 0x007B */
    &qmi_loc_event_inject_wifi_ap_data_reqresp_impl_tuple_v02, /* 0x007C */
    &qmi_loc_inject_wifi_ap_data_reqresp_impl_tuple_v02, /* 0x007D */
    &qmi_loc_notify_wifi_attachment_status_reqresp_impl_tuple_v02, /* 0x007E */
    &qmi_loc_notify_wifi_enabled_status_reqresp_impl_tuple_v02, /* 0x007F */
    &qmi_loc_event_geofence_batched_breach_notification_impl_tuple_v02, /* 0x0080 */
    &qmi_loc_event_vehicle_data_ready_status_impl_tuple_v02, /* 0x0081 */
    &qmi_loc_inject_vehicle_sensor_data_reqresp_impl_tuple_v02, /* 0x0082 */
    &qmi_loc_get_available_wwan_position_reqresp_impl_tuple_v02, /* 0x0083 */
    &qmi_loc_set_premium_services_config_reqresp_impl_tuple_v02, /* 0x0084 */
    &qmi_loc_set_xtra_version_check_reqresp_impl_tuple_v02, /* 0x0085 */
    &qmi_loc_event_gnss_measurement_report_impl_tuple_v02, /* 0x0086 */
    &qmi_loc_event_sv_polynomial_report_impl_tuple_v02,  /* 0x0087 */
    &qmi_loc_set_gnss_constell_report_config_impl_tuple_v02, /* 0x0088 */
    &qmi_loc_add_geofence_context_reqresp_impl_tuple_v02, /* 0x0089 */
    &qmi_loc_set_geofence_engine_context_reqresp_impl_tuple_v02, /* 0x008A */
    &qmi_loc_delete_geofence_context_reqresp_impl_tuple_v02, /* 0x008B */
    &qmi_loc_event_geofence_proximity_notification_impl_tuple_v02, /* 0x008C */
    &qmi_loc_inject_gtp_client_downloaded_data_reqresp_impl_tuple_v02, /* 0x008D */
    &qmi_loc_gdt_upload_begin_status_reqresp_impl_tuple_v02, /* 0x008E */
    &qmi_loc_gdt_upload_end_reqresp_impl_tuple_v02,      /* 0x008F */
    &qmi_loc_event_gdt_upload_begin_status_reqresp_impl_tuple_v02, /* 0x0090 */
    &qmi_loc_event_gdt_upload_end_reqresp_impl_tuple_v02, /* 0x0091 */
    &qmi_loc_start_dbt_reqresp_impl_tuple_v02,           /* 0x0092 */
    &qmi_loc_event_dbt_position_report_impl_tuple_v02,   /* 0x0093 */
    &qmi_loc_event_dbt_session_status_impl_tuple_v02,    /* 0x0094 */
    &qmi_loc_stop_dbt_reqresp_impl_tuple_v02,            /* 0x0095 */
    &qmi_loc_secure_get_available_position_reqresp_impl_tuple_v02, /* 0x0096 */
    &qmi_loc_event_geofence_batched_dwell_notification_impl_tuple_v02, /* 0x0097 */
    &qmi_loc_event_get_time_zone_info_impl_tuple_v02,    /* 0x0098 */
    &qmi_loc_inject_time_zone_info_reqresp_impl_tuple_v02, /* 0x0099 */
    &qmi_loc_inject_apcache_data_reqresp_impl_tuple_v02, /* 0x009A */
    &qmi_loc_inject_apdonotcache_data_reqresp_impl_tuple_v02, /* 0x009B */
    &qmi_loc_event_batching_status_impl_tuple_v02,       /* 0x009C */
    &qmi_loc_query_aon_config_reqresp_impl_tuple_v02     /* 0x009D */
};

static const qmi_implemented_messages loc_service_implemented_v02 =
{
  0x009D,  /* maximum message ID in this service */
  loc_service_implemented_array_v02
};

const qmi_implemented_messages *loc_get_service_impl_v02 (void)
{
  return &loc_service_implemented_v02;
}
