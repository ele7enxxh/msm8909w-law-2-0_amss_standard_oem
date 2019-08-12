#ifndef __LBS_SDP_H__
#define __LBS_SDP_H__

/*============================================================================
  @file lbs_sdp.h

                       LBS Sensor Data Processor API

GENERAL DESCRIPTION

  This file defines the API betwen the LBS task and the Sensor Data Processor
  (plugin) module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sdp/inc/lbs_sdp.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/10   asj     Added Util function
11/10/10   asj     Added SDP util functions, update enums
11/03/10   kgj     Original

=============================================================================*/


/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "comdef.h"   /* Definition for basic types and macros */
#include "customer.h"
#include "queue.h"
#include "aries_os_api.h"
#include "lsmp_api.h" /* For premium service */

#include "sdp_loc_eng_api.h"
#include "slim_client_types.h"

/* ---------------------------------------------------------------------------
 * Data type definitions
 * ---------------------------------------------------------------------------*/
typedef enum
{
  e_LBS_SDP_CFG_GYRO_DATA_VARIANCE,
  e_LBS_SDP_CFG_VIRGO_GRAVITY_AVERAGING_LENGTH,
  e_LBS_SDP_CFG_GYRO_RANDOM_WALK_VARIANCE,
  e_LBS_SDP_CFG_VELOCITY_RANDOM_WALK_SPECTRAL_DENSITY,
  e_LBS_SDP_CFG_ACCELERATION_RANDOM_WALK_SPECTRAL_DENSITY,
  e_LBS_SDP_CFG_ANGLE_RANDOM_WALK_SPECTRAL_DENSITY,
  e_LBS_SDP_CFG_RATE_RANDOM_WALK_SPECTRAL_DENSITY,
  e_LBS_SDP_CFG_SENSOR_ALGORITHM_MASK,
  e_LBS_SDP_CFG_MAXPLUS1
} lbs_SdpQmiCfg_e;

typedef union
{
  float   f_gyroDataVariance;
  float   f_gyroRandomWalkVariance;
  uint32  q_virgoGravityAveragingLength;
  float   f_velocityRandomWalkSpectralDensity;
  float   f_accelerationRandomWalkSpectralDensity;
  float   f_angleRandomWalkSpectralDensity;
  float   f_rateRandomWalkSpectralDensity;
  uint32  q_sensorAlgorithmMask;
} lbs_SdpQmiCfg_u;

typedef struct
{
  lbs_SdpQmiCfg_e   cfgType;
  lbs_SdpQmiCfg_u   cfgValue;
} lbs_SdpQmiCfg_t;

typedef enum {
  SDP_GNSS_STATE_ENGINE_UNKNOWN,
  SDP_GNSS_STATE_ENGINE_ON,
  SDP_GNSS_STATE_ENGINE_OFF
} sdp_GnssStateType;

// HACK: Remove this once QMI team provides a proper enum
typedef enum {
  QMI_INTERFACE_UNKNOWN = 0,
  QMI_INTERFACE_PDS = 6,
  QMI_INTERFACE_LOC = 16
} qmi_InterfaceType;

/* Struct to hold fix rate information from sent from  SM/TM */
typedef struct {
  uint32 num_fixes; /* Number of fixes - > 1 => AppTrack */
  uint32 time_between_fixes_ms; /* Time between fix (msec) */
}sdp_FixRateType;

/* consts used for determing if the session type is tracking or not */
#define SDP_MIN_NUM_FIXES_FOR_TRACKING (1)
#define SDP_MAX_TBF_MSEC_FOR_TRACKING  (5000)

/* ---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 *                          qmipds_sdpInit
 * ---------------------------------------------------------------------------*/
/**
@brief This function initializes the LBS task's SDP module
@param b_ForceInit: TRUE if the SDP init should happen regardless if it is 
                    already initialized 
@return TRUE if SDP init successful, else FALSE
*/
boolean qmipds_sdpInit( boolean b_ForceInit );

/* ---------------------------------------------------------------------------
 *                          qmipds_sdpStop
 * ---------------------------------------------------------------------------*/
/**
@brief This function stops SDP module processing
*/
void qmipds_sdpStop( void );

/* ---------------------------------------------------------------------------
 *                          qmipds_validateCfg
 * ---------------------------------------------------------------------------*/
/**
@brief This function checks validity of "QMI-configurable" parameters

@param pz_cfgTbl: Array (table) of configurable parameters
@param n_tblEntries: Number of parameters in the array (ie. the array length)

@return TRUE if parameters are valid, else FALSE
*/
boolean qmipds_validateCfg( const lbs_SdpQmiCfg_t * pz_cfgTbl,
                            int                     n_tblEntries );
/* ---------------------------------------------------------------------------
 *                          qmipds_cfgSdp
 * ---------------------------------------------------------------------------*/
/**
@brief This function sets 1 or more SDP "QMI-configurable" parameters

@param pz_cfgTbl: Array (table) of configurable parameters
@param n_tblEntries: Number of parameters in the array (ie. the array length)

@return TRUE if no errors, else FALSE
*/
boolean qmipds_cfgSdp( const lbs_SdpQmiCfg_t   *pz_cfgTbl,
                       int                     n_tblEntries );

/* ---------------------------------------------------------------------------
 *                          qmipds_sdpHandleIPC
 * ---------------------------------------------------------------------------*/
/**
@brief This function handles IPC messages sent to the SDP module from other
       tasks

@param p_ipcMsg      : pointer to the ipc message
*/
void qmipds_sdpHandleIPC( const os_IpcMsgType * p_ipcMsg );

/**
 * called from SM/LM to indicate a change of GNSS state. Issues an IPC msg
 * to SDP task.
 *
 * @param e_newGnssState Current GNSS state
 *
 * @return 0 if ipc has been sent successfully
 */
uint32 sdp_gnss_state_change_notification_proxy(sdp_GnssStateType e_newState);

/**
 * called from LBS/LocMW task to indicate the currently
 * available QMI interface. Issues an IPC msg to SDP task.
 *
 * @param uint32 QMI Service Id
 *
 * @return 0 if ipc has been sent successfully
 */
uint32 sdp_qmi_interface_change_notification_proxy(qmi_InterfaceType e_ServiceId);

/**
 * called from SM/TM to indicate number of fixes and time 
 * between fixes. Issues an IPC msg to SDP task. 
 *
 * @param q_NumFixes            Number of fixes.
 * @param q_TimeBetweenFixes_ms Time between fixes in milliseconds.
 *
 * @return 0 if ipc has been sent successfully
 */
uint32 sdp_send_fix_rate_info_proxy
(
  uint32 q_NumFixes,
  uint32 q_TimeBetweenFixes_ms
);

/**
 * called from SM/TM to indicate premium service config
 * for SAP feature. Issues an IPC msg to SDP task.
 *
 * @param premium service config
 *
 * @return 0 if ipc has been sent successfully
 */
uint32 sdp_send_premium_svc_cfg_proxy(lsmp_premium_services_param_type q_PremiumSvcCfg);

/**
 * sdp_SendPedometerControlRequest: Function used by NF or SM to send pedometer control 
 *     request to turn on/off the pedometer with desired configuration. 
 *
 * @param pz_PedometerReq: pedometer control request
 *
 * @return TRUE if the request has been successfully processed 
 */
boolean sdp_SendPedometerControlRequest(const locEngEventPedometerControlIndMsgT_v01 *pz_PedometerReq);

/**
 * sdp_SendMotionDataControlRequest: Function used by NF or SM to send motion data control 
 *     request to turn on/off the motion data streaming with desired configuration. 
 *
 * @param pz_MotionDataReq: motion data control request
 *
 * @return TRUE if the request has been successfully processed 
 */
boolean sdp_SendMotionDataControlRequest(const locEngEventMotionDataControlIndMsgT_v01 *pz_MotionDataReq);

/**
 * sdp_SendPedestrianAlignmentControlRequest: Function used by NF or SM to send pedestrian
 * alignment control request to turn on/off the pedestrian alignment with desired configuration.
 *
 * @param pz_PedAlignmentReq: pedestrian alignment control request
 *
 * @return TRUE if the request has been successfully processed 
 */
boolean sdp_SendPedestrianAlignmentControlRequest(
  const slimEnablePedestrianAlignmentRequestStructT *pz_PedAlignmentReq);

/*=============================================================================

  FUNCTION      sdp_SetAdditionalAccelDelay()

  DESCRIPTION   Add additional delay to accel measurements. 

  DEPENDENCIES  sdpl_AdditionalAccelDelayMs

  PARAMETERS    l_DelayMs: additional accel delay in ms

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void sdp_SetAdditionalAccelDelay(int32 l_DelayMs);

/*=============================================================================

  FUNCTION      sdp_SetAdditionalGyroDelay()

  DESCRIPTION   Add additional delay to gyro measurements. 

  DEPENDENCIES  sdpl_AdditionalGyroDelayMs

  PARAMETERS    l_DelayMs: additional gyro delay in ms

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void sdp_SetAdditionalGyroDelay(int32 l_DelayMs);

/*=============================================================================
  
  FUNCTION      sdp_InjectLsmpPremiumServiceMsg()

  DESCRIPTION   Inject the LSMP message for premium service to NF

  DEPENDENCIES  NONE

  PARAMETERS    pz_Data: pointer to the msg data

  RETURN VALUE  TRUE: message to SDP queued successfully
                FALSE: handling failed

  SIDE EFFECTS  None

=============================================================================*/
boolean sdp_InjectLsmpPremiumServiceMsg( void *pz_Data );

#endif  /* __LBS_SDP_H__ */
