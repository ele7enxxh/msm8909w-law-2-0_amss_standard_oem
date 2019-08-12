/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S E N S O R _ L O C A T I O N _ I N T E R F A C E _ M A N A G E R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the slim service Data structures.

  Copyright (c) 2014-2015 Qualcomm Atheros, Inc. 
  All rights reserved.
  Qualcomm Atheros Proprietary and Confidential.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/slim/src/sensor_location_interface_manager_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.3 
   It was generated on: Wed Mar 11 2015 (Spin 1)
   From IDL File: sensor_location_interface_manager_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "sensor_location_interface_manager_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t qmiSlimServiceDataHeaderT_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceDataHeaderT_v01, provider),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceDataHeaderT_v01, slimService),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceDataHeaderT_v01, timeSource),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceDataHeaderT_v01, time),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceDataHeaderT_v01, flags),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmiSlimGenRespMsgT_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiSlimGenRespMsgT_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimGenRespMsgT_v01, slimError) - QMI_IDL_OFFSET8(qmiSlimGenRespMsgT_v01, slimError_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimGenRespMsgT_v01, slimError)
};

static const uint8_t qmiSlimServiceErrorIndMsgT_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceErrorIndMsgT_v01, slimService),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceErrorIndMsgT_v01, slimError)
};

static const uint8_t qmiSlimDisableServiceReqMsgT_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimDisableServiceReqMsgT_v01, slimService)
};

static const uint8_t qmiSlimRegisterReqMsgT_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimRegisterReqMsgT_v01, clientType)
};

/* 
 * qmiSlimUnregisterReqMsgT is empty
 * static const uint8_t qmiSlimUnregisterReqMsgT_data_v01[] = {
 * };
 */
  
static const uint8_t qmiSlimServiceUpdateIndMsgT_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceUpdateIndMsgT_v01, slimService),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimServiceUpdateIndMsgT_v01, providerFlags)
};

static const uint8_t qmiSlimGetTimeUpdateReqMsgT_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateReqMsgT_v01, time)
};

static const uint8_t qmiSlimGetTimeUpdateRespMsgT_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, slimError) - QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, slimError_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, slimError),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, rxTime) - QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, rxTime_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, rxTime),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, txTime) - QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, txTime_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimGetTimeUpdateRespMsgT_v01, txTime)
};

static const uint8_t qmiSlimEnableSensorDataReqMsgT_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, slimService),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, providerFlags) - QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, providerFlags_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, providerFlags),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, sampleCount) - QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, sampleCount_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, sampleCount),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, reportRate) - QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, reportRate_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableSensorDataReqMsgT_v01, reportRate)
};

static const uint8_t qmiSlimEnablePedometerReqMsgT_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, providerFlags) - QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, providerFlags_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, providerFlags),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, reportPeriod) - QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, reportPeriod_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, reportPeriod),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, resetStepCount) - QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, resetStepCount_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, resetStepCount),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, stepCountThreshold) - QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, stepCountThreshold_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, stepCountThreshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, sampleRate) - QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, sampleRate_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, sampleRate),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, dutyCycleOnPercent) - QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, dutyCycleOnPercent_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnablePedometerReqMsgT_v01, dutyCycleOnPercent)
};

static const uint8_t qmiSlimEnableQmdDataReqMsgT_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, slimService),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, providerFlags) - QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, providerFlags_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, providerFlags),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, reportPeriod) - QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, reportPeriod_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, reportPeriod),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, varThreshold) - QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, varThreshold_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, varThreshold),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, varWinLen) - QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, varWinLen_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableQmdDataReqMsgT_v01, varWinLen)
};

static const uint8_t qmiSlimEnableVehicleDataReqMsgT_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableVehicleDataReqMsgT_v01, slimService),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimEnableVehicleDataReqMsgT_v01, providerFlags) - QMI_IDL_OFFSET8(qmiSlimEnableVehicleDataReqMsgT_v01, providerFlags_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimEnableVehicleDataReqMsgT_v01, providerFlags)
};

static const uint8_t qmiSlimSensorDataIndMsgT_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiSlimSensorDataIndMsgT_v01, header),
  QMI_IDL_TYPE88(0, 0),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimSensorDataIndMsgT_v01, sampleTimeOffsets),
  QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET8(qmiSlimSensorDataIndMsgT_v01, sampleTimeOffsets) - QMI_IDL_OFFSET8(qmiSlimSensorDataIndMsgT_v01, sampleTimeOffsets_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesX) - QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesX_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimSensorDataIndMsgT_v01, samplesX),
  QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesX) - QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesX_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesY) - QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesY_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimSensorDataIndMsgT_v01, samplesY),
  QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesY) - QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesY_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesZ) - QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesZ_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimSensorDataIndMsgT_v01, samplesZ),
  QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesZ) - QMI_IDL_OFFSET16RELATIVE(qmiSlimSensorDataIndMsgT_v01, samplesZ_len)
};

static const uint8_t qmiSlimPedometerDataIndMsgT_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, header),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, timeInterval),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepCount),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepConfidence) - QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepConfidence_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepCountUncertainty) - QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepCountUncertainty_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepCountUncertainty),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepRate) - QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepRate_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepRate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, strideLength) - QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, strideLength_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, strideLength),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, strideLengthUncertainty) - QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, strideLengthUncertainty_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, strideLengthUncertainty),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepCountFractional) - QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepCountFractional_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, stepCountFractional),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, reportType) - QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, reportType_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimPedometerDataIndMsgT_v01, reportType)
};

static const uint8_t qmiSlimQmdDataIndMsgT_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiSlimQmdDataIndMsgT_v01, header),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimQmdDataIndMsgT_v01, motionState)
};

static const uint8_t qmiSlimVehicleSensorDataIndMsgT_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiSlimVehicleSensorDataIndMsgT_v01, header),
  QMI_IDL_TYPE88(0, 0),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimVehicleSensorDataIndMsgT_v01, sampleTimeOffset),
  QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET8(qmiSlimVehicleSensorDataIndMsgT_v01, sampleTimeOffset) - QMI_IDL_OFFSET8(qmiSlimVehicleSensorDataIndMsgT_v01, sampleTimeOffset_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleX) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleX_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimVehicleSensorDataIndMsgT_v01, sampleX),
  QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleX) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleX_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleY) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleY_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimVehicleSensorDataIndMsgT_v01, sampleY),
  QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleY) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleY_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleZ) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleZ_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimVehicleSensorDataIndMsgT_v01, sampleZ),
  QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleZ) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleSensorDataIndMsgT_v01, sampleZ_len)
};

static const uint8_t qmiSlimVehicleOdometryDataIndMsgT_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiSlimVehicleOdometryDataIndMsgT_v01, header),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiSlimVehicleOdometryDataIndMsgT_v01, odometryFlags),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledBase),

  0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiSlimVehicleOdometryDataIndMsgT_v01, sampleTimeOffset),
  QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET8(qmiSlimVehicleOdometryDataIndMsgT_v01, sampleTimeOffset) - QMI_IDL_OFFSET8(qmiSlimVehicleOdometryDataIndMsgT_v01, sampleTimeOffset_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledAvg) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledAvg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledAvg),
  QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledAvg) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledAvg_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledLeft) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledLeft_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledLeft),
  QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledLeft) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledLeft_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledRight) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledRight_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledRight),
  QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01,
  QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledRight) - QMI_IDL_OFFSET16RELATIVE(qmiSlimVehicleOdometryDataIndMsgT_v01, distanceTravelledRight_len)
};

/* Type Table */
static const qmi_idl_type_table_entry  slim_type_table_v01[] = {
  {sizeof(qmiSlimServiceDataHeaderT_v01), qmiSlimServiceDataHeaderT_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry slim_message_table_v01[] = {
  {sizeof(qmiSlimGenRespMsgT_v01), qmiSlimGenRespMsgT_data_v01},
  {sizeof(qmiSlimServiceErrorIndMsgT_v01), qmiSlimServiceErrorIndMsgT_data_v01},
  {sizeof(qmiSlimDisableServiceReqMsgT_v01), qmiSlimDisableServiceReqMsgT_data_v01},
  {sizeof(qmiSlimRegisterReqMsgT_v01), qmiSlimRegisterReqMsgT_data_v01},
  {sizeof(qmiSlimUnregisterReqMsgT_v01), 0},
  {sizeof(qmiSlimServiceUpdateIndMsgT_v01), qmiSlimServiceUpdateIndMsgT_data_v01},
  {sizeof(qmiSlimGetTimeUpdateReqMsgT_v01), qmiSlimGetTimeUpdateReqMsgT_data_v01},
  {sizeof(qmiSlimGetTimeUpdateRespMsgT_v01), qmiSlimGetTimeUpdateRespMsgT_data_v01},
  {sizeof(qmiSlimEnableSensorDataReqMsgT_v01), qmiSlimEnableSensorDataReqMsgT_data_v01},
  {sizeof(qmiSlimEnablePedometerReqMsgT_v01), qmiSlimEnablePedometerReqMsgT_data_v01},
  {sizeof(qmiSlimEnableQmdDataReqMsgT_v01), qmiSlimEnableQmdDataReqMsgT_data_v01},
  {sizeof(qmiSlimEnableVehicleDataReqMsgT_v01), qmiSlimEnableVehicleDataReqMsgT_data_v01},
  {sizeof(qmiSlimSensorDataIndMsgT_v01), qmiSlimSensorDataIndMsgT_data_v01},
  {sizeof(qmiSlimPedometerDataIndMsgT_v01), qmiSlimPedometerDataIndMsgT_data_v01},
  {sizeof(qmiSlimQmdDataIndMsgT_v01), qmiSlimQmdDataIndMsgT_data_v01},
  {sizeof(qmiSlimVehicleSensorDataIndMsgT_v01), qmiSlimVehicleSensorDataIndMsgT_data_v01},
  {sizeof(qmiSlimVehicleOdometryDataIndMsgT_v01), qmiSlimVehicleOdometryDataIndMsgT_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object slim_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *slim_qmi_idl_type_table_object_referenced_tables_v01[] =
{&slim_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object slim_qmi_idl_type_table_object_v01 = {
  sizeof(slim_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(slim_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  slim_type_table_v01,
  slim_message_table_v01,
  slim_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry slim_service_command_messages_v01[] = {
  {QMI_SLIM_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_SLIM_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_SLIM_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_SLIM_UNREGISTER_REQ_V01, QMI_IDL_TYPE16(0, 4), 0},
  {QMI_SLIM_GET_TIME_UPDATE_REQ_V01, QMI_IDL_TYPE16(0, 6), 11},
  {QMI_SLIM_DISABLE_SERVICE_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {QMI_SLIM_ENABLE_SENSOR_DATA_REQ_V01, QMI_IDL_TYPE16(0, 8), 28},
  {QMI_SLIM_ENABLE_PEDOMETER_REQ_V01, QMI_IDL_TYPE16(0, 9), 40},
  {QMI_SLIM_ENABLE_QMD_DATA_REQ_V01, QMI_IDL_TYPE16(0, 10), 39},
  {QMI_SLIM_ENABLE_VEHICLE_DATA_REQ_V01, QMI_IDL_TYPE16(0, 11), 18}
};

static const qmi_idl_service_message_table_entry slim_service_response_messages_v01[] = {
  {QMI_SLIM_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_SLIM_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_SLIM_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_SLIM_UNREGISTER_RESP_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_SLIM_GET_TIME_UPDATE_RESP_V01, QMI_IDL_TYPE16(0, 7), 36},
  {QMI_SLIM_DISABLE_SERVICE_RESP_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_SLIM_ENABLE_SENSOR_DATA_RESP_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_SLIM_ENABLE_PEDOMETER_RESP_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_SLIM_ENABLE_QMD_DATA_RESP_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_SLIM_ENABLE_VEHICLE_DATA_RESP_V01, QMI_IDL_TYPE16(0, 0), 14}
};

static const qmi_idl_service_message_table_entry slim_service_indication_messages_v01[] = {
  {QMI_SLIM_SERVICE_UPDATE_IND_V01, QMI_IDL_TYPE16(0, 5), 18},
  {QMI_SLIM_SERVICE_ERROR_IND_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_SLIM_SENSOR_DATA_IND_V01, QMI_IDL_TYPE16(0, 12), 847},
  {QMI_SLIM_PEDOMETER_DATA_IND_V01, QMI_IDL_TYPE16(0, 13), 91},
  {QMI_SLIM_QMD_DATA_IND_V01, QMI_IDL_TYPE16(0, 14), 38},
  {QMI_SLIM_VEHICLE_ODOMETRY_DATA_IND_V01, QMI_IDL_TYPE16(0, 16), 1105},
  {QMI_SLIM_VEHICLE_SENSOR_DATA_IND_V01, QMI_IDL_TYPE16(0, 15), 1087}
};

/*Service Object*/
struct qmi_idl_service_object slim_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x37,
  8204,
  { sizeof(slim_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(slim_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(slim_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { slim_service_command_messages_v01, slim_service_response_messages_v01, slim_service_indication_messages_v01},
  &slim_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type slim_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SLIM_V01_IDL_MAJOR_VERS != idl_maj_version || SLIM_V01_IDL_MINOR_VERS != idl_min_version 
       || SLIM_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&slim_qmi_idl_service_object_v01;
}

