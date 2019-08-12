/*===========================================================================

                  MGP PE Sensors integration API

DESCRIPTION
  This file contains the interface for injecting inertial sensors data to PE. 

  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

 This program is confidential and a trade secret of Qualcomm Technologies, Inc.  The
 receipt or possession of this program does not convey any rights to
 reproduce or disclose its contents or to manufacture, use or sell anything
 that this program describes in whole or in part, without the express written
 consent of Qualcomm Technologies, Inc.  The recipient and/or possessor of this program
 shall not reproduce or adapt or disclose or use this program except as
 expressly allowed by a written authorization from Qualcomm Technologies, Inc.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/mgp/inc/mgp_pe_sensors_api.h#1 $  
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:45 $ 
===========================================================================*/

#ifndef MGP_PE_SENSORS_H
#define MGP_PE_SENSORS_H

#include "gps_types.h"

/* macros and parameters */
#define MGP_PE_SENSORS_MAX_INT CGPS_MAX_UINT32

/* NHC velocity variance */
#define NF_NHC_VELOCITY_CONSTRAINT_VARIANCE_MIN_M2_S2 (0.0f)    /* units m^2/s^2 */
#define NF_NHC_VELOCITY_CONSTRAINT_VARIANCE_MAX_M2_S2 (1.0E8f)
#define NF_NHC_VELOCITY_CONSTRAINT_VARIANCE_DEFAULT_M2_S2 (25.0f)

/* enums */
/* CRD-1 may communicate state as follows to PE */
typedef enum {
  MGP_PE_SENSORS_CRADLE_ROTATION_STATE_NOT_ROTATING= 0,
  MGP_PE_SENSORS_CRADLE_ROTATION_STATE_ROTATING,
  MGP_PE_SENSORS_CRADLE_ROTATION_STATE_UNKNOWN,

  MGP_PE_SENSORS_CRADLE_ROTATION_STATE_LAST_RESERVED = MGP_PE_SENSORS_MAX_INT

}
mgp_PeSensorsCradleRotationStateType;


/* output of mounted-state detector may be communicated as one of the following */
typedef enum {
  MGP_PE_SENSORS_MOUNTED_STATE_UNMOUNTED = 0,
  MGP_PE_SENSORS_MOUNTED_STATE_MOUNTED,
  MGP_PE_SENSORS_MOUNTED_STATE_UNKNOWN,

  /* ADD any other state above this line */
  MGP_PE_SENSORS_MOUNTED_STATE_LAST_RESERVED = MGP_PE_SENSORS_MAX_INT
} 
mgp_PeSensorsMountedStateType;

/* following settings are exposed by PE sensors module.
   For each setting, a corresponding member of the union
   mgp_PeSensorsConfigData holds the appropriate value */
typedef enum {
  MGP_PE_SENSORS_CONFIG_FIRST = 0, 
  MGP_PE_SENSORS_CONFIG_HF_MODE = MGP_PE_SENSORS_CONFIG_FIRST,
  MGP_PE_SENSORS_CONFIG_NHC_ENABLE,
  MGP_PE_SENSORS_CONFIG_YAW_GYRO_BIAS,

  /* ADD all new config items above this line */
  MGP_MGP_PE_SENSORS_CONFIG_LAST,
  MGP_MGP_PE_SENSORS_CONFIG_MAX_RESERVED = MGP_PE_SENSORS_MAX_INT
} 
mgp_PeSensorsConfigType;

/* Source of the mounted state indicator: internal/external */
typedef enum {
  MGP_PE_SENSORS_MOUNTED_STATE_SOURCE_INTERNAL = 0,
  MGP_PE_SENSORS_MOUNTED_STATE_SOURCE_EXTERNAL,

  /* ADD any other state above this line */
  MGP_PE_SENSORS_MOUNTED_STATE_SOURCE_LAST_RESERVED = MGP_PE_SENSORS_MAX_INT
} 
mgp_PeSensorsMountedStateSourceType;

/* structs */

/* generic structure to hold value and uncertainty
   of a single precision scalar random variable */
typedef struct {
  FLT f_Value;        /* unc and value should have same units */
  FLT f_Uncertainty;
} mgp_PeSensorsFltValUnc;


/* time stamp structure. Sensor data injected in to PE
   must be time stamped with GPS time
*/
typedef struct {
  uint16 w_GpsWeek;
  uint32 q_GpsMsec;
} mgp_PeSensorsTimeStamp;

/* structure of yaw gyro samples.
   Each sample has an associated gps millisecond.
   Measurement uncertainties are specified OOB */
typedef struct {
  mgp_PeSensorsTimeStamp z_TimeStamp;    /* time stamp associated with this packet. Week number is a minor overhead
                                           but is important for detecting the difference between roll-overs and a 
                                           late / stray packet
                                        */
  mgp_PeSensorsFltValUnc z_YawGyroRate; /* yaw gyro rate sample in radians / sec */                  
                                        /* uncertainty associated with yaw gyro rate in rad / sec */ 

  mgp_PeSensorsFltValUnc z_YawGyroRateBias;  /* computed gyro rate bias in radians / sec */                        
                                             /* uncertainty associated with yaw gyro rate bias in radians / sec */ 
} mgp_PeSensorsYawGyroSample;


/* cradle rotation data */
typedef struct {
  mgp_PeSensorsTimeStamp                 z_TimeStamp;  /* time-stamp of cradle rotation */
  mgp_PeSensorsCradleRotationStateType   e_State;      /* state of cradle rotation */
  mgp_PeSensorsFltValUnc                 z_Angle;      /* angle of rotation in radians */
  FLT                                 f_Confidence;  /* confidence in rotation decision */
}
mgp_PeSensorsCradleRotationState;

typedef struct {
  mgp_PeSensorsTimeStamp           z_TimeStamp;    /* time-stamp of this mounted state estimate */
  mgp_PeSensorsMountedStateType    e_State;        /* one of mgp_PeSensorsMountedState */
  mgp_PeSensorsMountedStateSourceType e_Source;   /* source (internal/external) for the mounted state */
  FLT                           f_Confidence;  /* associated confidence in mounted state call */
  FLT                           f_VelocityConstraintVariance; 
} 
mgp_PeSensorsMountedState;

/* output of mounted-state detector may be communicated as one of the following */
typedef enum {
  MGP_PE_SENSORS_SDP_STOP = 0,
  MGP_PE_SENSORS_GVECTOR_RESET
} 
mgp_PeSensorsStateIndicatorEnumType;

typedef struct {
  mgp_PeSensorsTimeStamp               z_TimeStamp;    /* time-stamp of this mounted state estimate */
  mgp_PeSensorsStateIndicatorEnumType  e_State;        /* one of mgp_PeSensorsMountedState */
}
mgp_PeSensorsStateStructType;

/* Indicate if the integrated sensor measurement is regular (periodic) update 
   from sensor data processor, or in response to a GNSS engine request. */
typedef enum 
{
  MGP_PE_SENSOR_MEAS_INVALID = 0,      /* 0: Invalid sensor meas */
  MGP_PE_SENSOR_MEAS_PERIODIC_UPDATE,  /* 1: Periodic sensor meas */
  MGP_PE_SENSOR_MEAS_RESPONSE          /* 2: On-demand sensor meas */
} mgp_PeIntSensorMeasFlag; 

/* This data structure contains integrated sensor measurements with 
   timestamp, uncertainty and validity information */
typedef struct
{
 U64 t_TimeTickMsUpdate;        /* Timestamp (Msec) when the data arrives to 
                                   NF task */      
 U64 t_TimeTickMsStart;         /* TimeStamp (Msec) at the start of the 
                                   integration interval */
 mgp_PeSensorsTimeStamp z_IntStartTime; /* Integration start time */

 U32 q_IntegrationIntervalMsec; /* Sensor measurement integration interval */
 mgp_PeIntSensorMeasFlag e_IntSensorMeasFlag; /* Indicate if the integrated 
                                    sensor measurement is regular update from 
                                    SDP, or in response to a GILE request. */
 
 U8  u_BiasId;                  /* ID of the most recent bias correction packet
                                   that has been applied to this sensor meas */
 U8  u_Mode;                    /* Static or Dynamic: indicates the property of 
                                   this sensor data packet. Applicability of the
                                   data for static/dynamic mode could be different*/ 

 DBL d_ImuAccel[3];             /* Accelerations in IMU frame. Units: m/s^2 */
 DBL d_AverageGyro[3];          /* Average Gyro measurement over the integration 
                                   interval. Units: radian/sec */
 /* We need 3 different variances for 3 axis, as physical sensors for different axis 
 could be different. */
 FLT f_AccelVar[3];             /* Acceleration variance (3-D). Units: (m/s^2)^2 */
 FLT f_IntegratedGyroVar[3];    /* Delta Rotation variance (3-D). Units: (rad/s)^2 */

 FLT f_AccelNoise;              /* Acceleration process noise. Units: (m/s^2)^2 */
 FLT f_VelocityNoise[3];        /* Velocity process noise. Units: (m/s)^2 */
 FLT f_GyroNoise;               /* Angular velocity process noise. Units: (rad/s)^2 */
 FLT f_AngleNoise[3];           /* Angle process noise. Units: (rad)^2 */

 DBL d_IntegratedQuaternion[4]; /* We may also want to put the data type for 
                                   Quaternion. Units: None */
 DBL d_Temperature;             /* Temperature of the sensor board. Units: Degree C*/

 boolean b_ZmdDecision;         /* ZMD decision: TRUE if zero-motion, FALSE if non-
                                   stationary. */
} mgp_PeIntSensorMeasStruct;

/***********   Sensor Biases to be sent to GILE ******************/
typedef struct
{
  FLT f_GyroBias[3];    /* Current best estimate of Gyroscope Bias  in (radians)/(seconds)*/
  FLT f_AccelBias[3];     /* Current best estimate of Accelerometer Bias in (meters)/(seconds^2) */

} mgp_PeSensorBiasStruct;


/* methods */

/*
 ******************************************************************************
 * mgp_PeSensorsInjectYawGyroMeasurements
 *
 * Function description:
 *
 * Inject Yaw gyro measurements to MGP
 *
 * Parameters:
 *   p_Samples: Array of mgp_PeSensorsYawGyroSample representing time-stampped
 *              yaw gyro rates
 *   q_NumSamples: number of samples in array
 *   p_CradleState: cradle rotation state
 *
 * Return value: TRUE is successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean mgp_PeSensorsInjectYawGyroMeasurements(const mgp_PeSensorsYawGyroSample *p_Samples, 
                                               uint32 q_NumSamples, 
                                               const mgp_PeSensorsCradleRotationState  *p_CradleState);

/*
 ******************************************************************************
 * mgp_PeSensorsSetMountedSate
 *
 * Function description:
 *
 * Notify PE of mounted state events
 *
 * Parameters:
 *   p_State: Current Mounted state
 *
 * Return value: TRUE is successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean mgp_PeSensorsSetMountedSate(const mgp_PeSensorsMountedState *p_State);

/*
 ******************************************************************************
 * mgp_PeSensorsSetState
 *
 * Function description:
 *   Notify PE of sensor state events, such as Gravity-vector resets, 
 *    SDP stop, etc. In response, NF may decide to reset OTF Gyro Cal 
 *    or any other neccessary action. 
 *
 * Parameters:
 *   pz_State: Current sensor state event
 *
 * Return value: TRUE is successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean mgp_PeSensorsSetState(const mgp_PeSensorsStateStructType *pz_State);

/*
 ******************************************************************************
 * mgp_PeIntSensorMeasPut
 *
 * Function description:
 *   This function is used to pass pre-processed and integrated Sensor Data, 
 *   associated integration time information, and other associated information 
 *   (validity bit-mask, etc) to GNSS engine.
 *
 * Parameters:
 *   pz_SensorMeas: Pointer to the integrated sensor measurement packet. 
 *
 * Return value: TRUE is successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean mgp_PeIntSensorMeasPut(const mgp_PeIntSensorMeasStruct *pz_SensorMeas);

/******************************************************************************
 * mgp_PeSensorBiasPut
 *
 * Function description:
 *   This function is used inject to sensor model data to the GNSS Engine.
 *
 * Parameters:
 *   pz_SensorMeas: Pointer to the sensor data. 
 *
 * Return value: TRUE is successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean mgp_PeSensorBiasPut(const mgp_PeSensorBiasStruct *pz_SensorBias);

#endif /* MGP_PE_SENSORS_H */




