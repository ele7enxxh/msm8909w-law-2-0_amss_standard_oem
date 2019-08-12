/*==============================================================================

    S E N S O R S   D A T A  P R O C E S S O R <-> P E    I N T E R F A C E

DESCRIPTION

   Defines the sensors sdp interface exposed to GPS PE

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sdp/inc/sdp_pe_iface.h#1 $
$DateTime: 2016/12/13 07:59:45 $ 
$Author: mplcsds1 $ 


when         who     what, where, why
----------   ---     -----------------------------------------------------------
03/29/11     mk      renamed from lbs_sdp_pe.h to sdp_pe_iface.h 
11/03/10     asj     created.
==============================================================================*/

#ifndef _LBS_SDP_PE_H
#define _LBS_SDP_PE_H

#include "comdef.h"
#include "gps_types.h"
#include "locEng_sm_common_msg_v01.h"


/***********   Timing information ************************/
typedef struct 
{
  uint32   q_TimeTickVal;  /* estimate  of timetick_get()       */
  FLT      f_UncMs;        /* Uncertainty of the estimate  (ms) */
} lbs_SdpPeTimeInfo_t;

/***********   Velocity information ************************/
typedef struct 
{
  FLT f_VelEast;        /* East velocity - m/s                    */
  FLT f_VelNorth;       /* North velocity - m/s                   */
  FLT f_VelVer;         /* vertical velocity   - m/s              */

  FLT f_VelNorthUnc;    /* North velocity uncertainty  - m/s      */
  FLT f_VelEastUnc;     /* Horizontal velocity uncertainty -  m/s */
  FLT f_VelVertUnc;     /* Vertical velocity uncertainty   - m/s  */
} lbs_SdpPeVelInfo_t;

/***********   Static Odds information ************************/
typedef struct 
{
  boolean b_KfStateStatic;  /* 0 => KF is in dynamic or unknown state           */
                          /* 1 => KF is in static state                       */

  /* static odds and its components */
  FLT f_OddsStatic;   
  FLT f_OddsStaticVelHeve;
  FLT f_OddsStaticCarrier; 
  FLT f_OddsStaticDynOffset; 
  FLT f_OddsStaticForAWhile;
  FLT f_OddsStaticNonGnssObservation; 

}lbs_SdpPeStaticInfo_t;

/***********   Position Uncertainty information ************************/
typedef struct 
{
  /* position unc components */
  FLT f_PosUncMetersLatLonAlt[3];

}sdp_PePosInfo_t;


/***********   IPC payload *********************/

/* this is an older packet and is deprecated. It is retained
   here only for backwards compatibility. This would help with
   older data collects during playback. See sdp_PeInfoPacket_t
*/

typedef struct 
{
  /* time of application of all information to follow                  */
  uint16     w_GpsWeek;        /* gps_week                */
  uint32     q_GpsMsInWeek;    /* gps millisecond in week */

  /* What is valid   */
  boolean b_TimeInfoValid;
  boolean b_StaticInfoValid;
  boolean b_VelInfoValid;  

  /* The information elements themselves */
  lbs_SdpPeStaticInfo_t     z_StaticInfo;
  lbs_SdpPeTimeInfo_t       z_TimeInfo;
  lbs_SdpPeVelInfo_t        z_VelInfo;

} lbs_SdpPeInfoPacket_t;

typedef struct 
{
  /* time of application of all information to follow                  */
  uint16     w_GpsWeek;        /* gps_week                */
  uint32     q_GpsMsInWeek;    /* gps millisecond in week */

  /* What is valid   */
  boolean b_TimeInfoValid;
  boolean b_StaticInfoValid;
  boolean b_VelInfoValid;  
  boolean b_PosInfoValid;  

  /* The information elements themselves */
  lbs_SdpPeStaticInfo_t     z_StaticInfo;
  lbs_SdpPeTimeInfo_t       z_TimeInfo;
  lbs_SdpPeVelInfo_t        z_VelInfo;
  sdp_PePosInfo_t           z_PosInfo;

} sdp_PeInfoPacket_t;

/* PLE Pedestrian Detection information */
typedef struct 
{
  /* time of application of all information to follow                  */
  uint16     w_GpsWeek;        /* gps_week                */
  uint32     q_GpsMsInWeek;    /* gps millisecond in week */

  /* PLE pedestrian detection flag */
  boolean b_PedestrianDetected;

} sdp_PleInfoPacket_t;

#define SDP_RESET_MASK_INT_CALIBRATION  0x0001
#define SDP_RESET_MASK_QMI_FSM          0x0002

typedef struct 
{
  /* Time of reset. If GpsTime is not avaible, GpsWeek is specified as 
     Unknown (0xffff) */
  uint16     w_GpsWeek;        /* gps_week                */
  uint32     q_GpsMsInWeek;    /* gps millisecond in week */

  /* What to reset */
  uint16  w_ResetMask;

} sdp_ResetRequest_t;

/* SDP integration mode. */
enum sdp_IntegrationMode
{
  SDP_INT_MODE_STATIC  = 0, /* Static calibration mode. Assume no rotation */
  SDP_INT_MODE_DYNAMIC = 1, /* Regular (dynamic) update mode */
  SDP_INT_MODE_RESET   = 2  /* Reset sensor bias mode */
};

/***********   Sensor Bias Corrections information ************************/
typedef struct
{
  U64 t_TimeTickMsec;        /* Timestamp (Msec) when the biases were set*/      
  U16 w_GpsWeek;             /* GPS time when the biases were set */
  U32 q_GpsMsInWeek;
  U8  u_BiasCorrId;          /* Bias Correction ID.*/
  
  DBL d_AccelBiasCorr[3];    /* KF estimate of acceleration bias correction. 
                                Units: m/s^2*/
  DBL d_AccelBiasUnc[3];     /* Acceleration bias uncertainties in sensor 
                                frame, m/s2*/
  DBL d_AccelScaleCorr[3];   /* Acceleration scale factor corrections */
  DBL d_AccelScaleUnc[3];    /* Acceleration scale factor uncertainties */
  
  DBL d_GyroBiasCorr[3];     /* KF estimate of gyroscope bias correction. 
                                Units: radian/s */
  DBL d_GyroBiasUnc[3];      /* Gyroscope bias uncertainties in sensor 
                                frame, rad/s */
  DBL d_GyroScaleCorr[3];    /* Gyroscope scale factor corrections */
  DBL d_GyroScaleUnc[3];     /* Gyroscope scale factor uncertainties */

  U8  u_IntegrationMode;     /* Integration mode. 0 = static; 1 = dynamic.
                                2 = reset see sdp_IntegrationMode above.  */
} sdp_PeSensorBiasCorr;


/***********   Integrated Sensor Data Request ************************/
typedef struct 
{
  U64    t_TimeTickMsec;    /* Timestamp (msec) of the request message */ 

  U64    t_TimeTickMsStart; /* TimeStamp (Msec) at the start of the 
                               integration interval*/
  U16    w_GpsWeek;         /* GPS time when the biases were set */
  U32    q_GpsMsInWeek;
  U32    q_IntegrationIntervalMsec; /* Default 1000: Msec (1 Hz) */
} sdp_IntSensorMeasRequest;

/*
  sdp_PeConfigQueryValidityUnionType is analogue of mgp_ConfigQueryValidityUnionType 
  This structure captures the validity of various 
  configurable values that can be presented to PE. This is used communicate the
  result of an asycn set / query request from SDP. 

  The validity of values queried or set are specified using 
  the u_Validity union memeber.

  See also:
  1. mgp_SdpQueryConfigStructType for details on each element
  2. mgp_GetConfigDataMultiple() for usage

  the z_Flags data memeber provides human friendly flags that can be
  set / queried easily and individually.

  the t_Masks member provides a uint64 style bitmask for different values.
  NOTE: due to the way bit fields work, the first bit in z_Flags is the MSB
  of t_Masks (ie. b_headingFilterMode_valid should be checked as 
  if(t_Masks & (1<<63)) 

  NOTE: we DO NOT reuse the mgp_ConfigQueryValidityUnionType because
  doing so can cause implicit IPC dependencies. Defining a separate 
  struct ensures that PE can modify it's messages independently of SDP
  (modularity)
*/

typedef union {
  struct {
    boolean b_headingFilterMode_valid;
    boolean b_VelocityConstraintVariance_valid;
    boolean b_GyroBiasRandomWalk_valid;
    boolean b_PositioningMethod_valid;
    boolean b_MaximumDeadReckoningTime_valid;
    boolean b_InsFilterCrossCheckSigmaLowThreshold_valid;
    boolean b_InsFilterCrossCheckSigmaHighThreshold_valid;
  } z_Flags;

  uint64 t_Masks;
}
sdp_PeConfigQueryValidityUnionType;

/*
  sdp_PeQueryConfigStructType is the analogue of mgp_SdpQueryConfigStructType 
  This struct captures the various configurable values that
  can be presented to PE. This is different from the pe_ConfigData struct
  one instance of pe_ConfigData can only represnt / hold one datum at a
  given time. mgp_SdpQueryConfigStructType, on the other hand, is intended to
  hold multiple fields simultaneously. this provides the ability to QUERY
  OR SET MORE THAN ONE VALUE AT A TIME. The validity of values queried or 
  set are specified using the u_Validity union memeber.

  See also:
  1. pe_ConfigValidityUnionType for details on each element
  2. pe_GetConfigDataMultiple() for usage

  NOTE: we DO NOT reuse the mgp_SdpQueryConfigStructType because
  doing so can cause implicit IPC dependencies. Defining a separate 
  struct ensures that PE can modify it's messages independently of SDP
  (modularity)
*/
typedef struct 
{
  /* which of the following fields contain valid data */
  sdp_PeConfigQueryValidityUnionType z_Validity;
  
  int8    b_headingFilterMode;
  FLT     f_VelocityConstraintVariance;
  FLT     f_GyroBiasRandomWalk;
  U32     q_PositioningMethod;
  U32     q_MaximumDeadReckoningTime;
  uint8   u_InsFilterCrossCheckSigmaLowThreshold;
  uint8   u_InsFilterCrossCheckSigmaHighThreshold;
  /* add other data types here */


} sdp_PeQueryConfigStructType;
/*
 ******************************************************************************
 * Function lbs_SdpUpdateGnssInfo(lbs_SdpPeInfoPacket_t *gnssInfo)
 *
 * Description:
 *
 *  This function is used by the GNSS engine to update the sensors processing engine
 *  about various GNSS computed information elements
 *
 * Parameters: 
 *     pointer to sdp_pe_infoPacket_t defined in this file
 *     the memory pointed to by this pointer may be freed on return
 *
 * Return value: 
 *      FALSE => an error occured
 *      TRUE  => otherwise
 *
 ******************************************************************************
*/
boolean lbs_SdpUpdateGnssInfo(const lbs_SdpPeInfoPacket_t *p_GnssInfo);

/*
 ******************************************************************************
 * Function lbs_SdpHandlePeUpdate(lbs_SdpPeInfoPacket_t *gnssInfo)
 *
 * Description:
 *
 *  This function is used by the GNSS engine to update the sensors processing engine
 *  about various GNSS computed information elements
 *  This function is called in the context of the LBS task.
 *
 * Parameters: 
 *     pointer to sdp_pe_infoPacket_t defined in this file
 *     the memory pointed to by this pointer may be freed on return from this function. 
 *
 * Return value: 
 *      FALSE => an error occured
 *      TRUE  => otherwise
 *
 ******************************************************************************
*/
boolean lbs_SdpHandlePeUpdate(const lbs_SdpPeInfoPacket_t *p_GnssInfo);


/*
 ******************************************************************************
  FUNCTION      sdp_SensorBiasCorrPut()

  DESCRIPTION 	This function is used by the GNSS engine to update the sensors 
                processing engine about the sensor bias corrections/errors 
                estimated by the GILE KF. 
 
  DEPENDENCIES  None

  PARAMETERS    p_GnssSensorBiasCorr: Pointer to the sensor bias corrections. 

  RETURN VALUE  FALSE: an error occured
                TRUE:  otherwise 

  SIDE EFFECTS  None

***************************************************************************** */
boolean sdp_SensorBiasCorrPut(const sdp_PeSensorBiasCorr *p_GnssSensorBiasCorr);


/*
 ******************************************************************************
  FUNCTION      sdp_HandleSensorBiasCorr()

  DESCRIPTION 	This function is handles the sensor bias corrections/errors 
                update by the GNSS engine (GILE KF). 
 
  DEPENDENCIES  None

  PARAMETERS    p_GnssSensorBiasCorr: Pointer to the received sensor bias 
                          corrections. 

  RETURN VALUE  None

  SIDE EFFECTS  None

***************************************************************************** */
void sdp_HandleSensorBiasCorr(const sdp_PeSensorBiasCorr *p_GnssSensorBiasCorr);



/*
 ******************************************************************************
  FUNCTION      sdp_IntSensorMeasRequest()

  DESCRIPTION 	This function is used by the GNSS engine to send the request for 
                integrated sensor data for a time-interval specified in the 
                request. 
 
  DEPENDENCIES  None

  PARAMETERS    p_Request: Pointer to the integrated sensor data request. 

  RETURN VALUE  FALSE: an error occured
                TRUE:  otherwise 

  SIDE EFFECTS  None

***************************************************************************** */
boolean sdp_IntSensorMeasRequestPut(const sdp_IntSensorMeasRequest *p_Request);


/*
 ******************************************************************************
  FUNCTION      sdp_HandleIntSensorMeasRequest()

  DESCRIPTION 	This function is handles the integrated sensor data request from 
                the GNSS engine (GILE KF). 
 
  DEPENDENCIES  None

  PARAMETERS    p_Request: Pointer to the received integrated sensor data request.

  RETURN VALUE  None

  SIDE EFFECTS  None

***************************************************************************** */
void sdp_HandleIntSensorMeasRequest(const sdp_IntSensorMeasRequest *p_Request);

/* -------------------------------------------------------------------------
* sdp_PeConfigQueryResponse
*
* Description: 
*  This function is called by PE in response to the SDP message to query the configuration
*  settings within PE/NF. The result is an IPC to the SDP task from PE. 
*
* Parameters:
*  pz_ConfigResp - structure specifying settings queried. The response
*             mimics the structure of the query. The bit masks / flags
*             indicate which value is to be queried.
*             In the response message, the flags indicate which values
*             are "filled in".
*             TRUE => valid/requested, FALSE otherwise
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* See also:   locEngGetNavigationConfigRespMsgT_v01
* -----------------------------------------------------------------------*/
boolean sdp_PeConfigQueryResponse(const locEngGetNavigationConfigRespMsgT_v01 *pz_ConfigResp);

/* -------------------------------------------------------------------------
* sdp_PeConfigSetResponse
*
* Description: 
*  This function is called by PE in response to the async message from SDP 
*  to set the configuration settings within PE/NF. This is method allows 
*  multiple items can be set at once.
* 
*  The response is an IPC that indicates which values were successfully applied.
*
* Parameters:
*  pz_ConfigResp - structure specifying result of settings sent from the SDP. 
*             The response mimics the structure of the request. The bit masks / flags
*             indicate which values were successfully applied. SDP can compare 
*             against the requested mask to determine which values passed / failed.
*
*             FALSE => invalid / failed, 
*             TRUE => value needs to be set / successfully applied
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* See also:   locEngSetNavigationConfigRespMsgT_v01
* -----------------------------------------------------------------------*/
boolean sdp_PeConfigSetResponse(const locEngSetNavigationConfigRespMsgT_v01 *pz_ConfigResp);

/* -------------------------------------------------------------------------
* sdp_TranslateSdpPeInfoPacket
*
* Description: 
*  Conversion routine to translate an older "update from PE" IPC to the 
*  newer format, which includes HEPE as well.
*
* Parameters:
*  p_Dest - new message payload with data corresponding to p_Src
*  p_Src  - older data to be converted
*
* Return value: 
*  FALSE => conversion failed
*  TRUE => successfully converted
*
* See also:   sdp_PeInfoPacket_t
* -----------------------------------------------------------------------*/
boolean sdp_TranslateSdpPeInfoPacket(sdp_PeInfoPacket_t *p_Dest, const lbs_SdpPeInfoPacket_t* p_Src);

/* -------------------------------------------------------------------------
* sdp_HandlePeUpdate
*
* Description: 
*  Handles a "fix update" from PE. this carries timing information, static
*  odds and HEPE information from PE to SDP.
*
* Parameters:
*  p_GnssInfo - new information from PE
*
* Return value: 
*  FALSE => handling failed
*  TRUE => message successfully handled
*
* See also:   sdp_PeInfoPacket_t
* -----------------------------------------------------------------------*/
boolean sdp_HandlePeUpdate(const sdp_PeInfoPacket_t *p_GnssInfo);

/* -------------------------------------------------------------------------
* sdp_HandlePeUpdate
*
* Description: 
*  Sends a "fix update" IPC to SDP task. This method is called from PE. 
*  this carries timing information, static
*  odds and HEPE information from PE to SDP.
*
* Parameters:
*  p_GnssInfo - new information from PE
*
* Return value: 
*  TRUE => message to SDP queued successfully 
*  FALSE => handling failed
*
* See also:   sdp_PeInfoPacket_t
* -----------------------------------------------------------------------*/
boolean sdp_UpdateGnssInfo(const sdp_PeInfoPacket_t *p_GnssInfo);

/* -------------------------------------------------------------------------
* sdp_UpdatePleInfo
*
* Description:
*  Sends a PLE info IPC to SDP task. This method is called from PLE and
*  carries the pedestrian mode information.
*
* Parameters:
*  pz_PleInfo - information from PLE
*
* Return value:
*  TRUE => message to SDP queued successfully
*  FALSE => handling failed
* -----------------------------------------------------------------------*/
boolean sdp_UpdatePleInfo(const sdp_PleInfoPacket_t *pz_PleInfo);

/* -------------------------------------------------------------------------
* sdp_SendResetReq
*
* Description:
*  This function is used by the GNSS engine/PE to reset SDP. The reset mask 
*   in the request specifies to calibrate,
*    SDP_RESET_MASK_INT_CALIBRATION -- SDP internal calibration parameters 
*    SDP_RESET_MASK_QMI_FSM -- SDP QMI state-machine  
*
* Parameters:
*     pz_ResetReq -- SDP reset request (GpsWeek and GpsMsec in the request
*                    are for logging only. If Gps time not available, then 
*                    specify Unknown GpsWeek). 
*
* Return value:
*      FALSE => an error occured
*      TRUE  => otherwise
*
* See also:   sdp_ResetRequest_t
* -----------------------------------------------------------------------*/
boolean sdp_SendResetReq(const sdp_ResetRequest_t *pz_ResetReq);

/* -------------------------------------------------------------------------
* sdp_HandleResetReq
*
* Description:
*  Handles the SDP Reset request from PE. 
*
* Parameters:
*  pz_ResetReq - SDP Reset Request from PE
*
* Return value:
*  TRUE => message to SDP queued successfully
*  FALSE => handling failed
*
* See also:   sdp_SendResetReq, sdp_ResetRequest_t
* -----------------------------------------------------------------------*/
boolean sdp_HandleResetReq(const sdp_ResetRequest_t *pz_ResetReq);

/* -------------------------------------------------------------------------
* sdp_GetMsiAndXmsiReq
*
* Description:
*  Sends the MSI and XMSI state request from PE to SDP. 
*
* Parameters:
*  None
*
* Return value:
*  TRUE => message to SDP queued successfully
*  FALSE => handling failed
*
* -----------------------------------------------------------------------*/
boolean sdp_GetMsiAndXmsiReq( void);

/* -------------------------------------------------------------------------
* sdp_HandleMsiAndXmsiReq
*
* Description:
*  Handles the MSI and XMSI state request from PE. 
*
* Parameters:
*  None
*
* Return value:
*  TRUE => message to SDP queued successfully
*  FALSE => handling failed
*
* -----------------------------------------------------------------------*/
boolean sdp_HandleMsiAndXmsiReq( void);

#endif /* _LBS_SDP_PE_H */
