/*============================================================================
  FILE:         le_types.h

  OVERVIEW:     LE types shared between le_iface.h, and transitional pipes
                in MGP, PE & NF from LE to NF.

  DEPENDENCIES: NA
 
                Copyright (c) 2015 QUALCOMM Atheros, Inc.
                All Rights Reserved.
                Qualcomm Atheros Confidential and Proprietary
                
                Copyright (c) 2015 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/le/inc/le_types.h#1 $ 
   $DateTime: 2016/12/13 07:59:45 $ 
   $Author: mplcsds1 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-24  wtr  new file split from le_iface.h

============================================================================*/

#ifndef _LE_TYPES_H
#define _LE_TYPES_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "com_dtypes.h" // for uint64
#include "gps_types.h" // for FLT
#include "wl_common.h" // for wl_PosReqSrcType
#include "pdapi.h" // for pdsm_pd_reliability_e_type

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define LE_MAX_WIFI_COV_MEAS 50

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// LE wifi fix control inputs
typedef struct
{
  wl_PosReqSrcType e_PosReqSource; // where to send the result
  struct
  {
    uint8 v_ForceWifiSource : 1;  // send best WiFi fix, even if better fix 
                                  // exists – TBD support in 2nd phase */
    // 7 bits remain
  } u_Ctrl; // extra control bits
 }
le_WileFixControlFlagType; // Used in IPC - change with IPC rev. only

// Measurement and matching AD for one Wifi Access Point.
typedef struct
{
  uint64 t_MacAddress; /** Lower 48-bits active.  WLM & LE can both use this for
                           tracking & logging. LE/WiLE only uses this for 
                           logging. */
  FLT    f_RssiDbm; /** in dBm.  
                        (Careful to convert from LOWI-LP format of 0.5dBm steps,
                        possibly signed.) 
                        Used to cull the weak ones – no weighted averaging yet 
                        */
  int32  l_Age_ms;  /** Age of this specific measurement, in milliseconds, 
                        vs. the time in the header.
                        Negative values, on input are invalid, as age is time
                        before now, and shall be treated as unknown age.
                        See also note below in header.*/
  FLT    f_LatDeg;  /** latitude, WGS-84, in degrees, south negative
                        Note that this can have resolution of
                        2^6 * 1/2^23 = 1/2^17 degrees, which is ~0.76 meters.
                        Longitutde below can be 1.5m resolution.
                        This matches Wifi AD (GTP) server provided accuracy, 
                        and is still precise enough for coverage area.  
                        Switching to DBL, or fixed-point uint32, is recommended
                        for future higher accuracy applicatons, like 
                        Wifi-Ranging. */
  FLT    f_LonDeg;   // longitude, WGS-84, in degrees
  FLT    f_Mar_m;   /** Maximum Antenna Range (meters)
                        Used as MAR = HEPE * 2, which implies a radius % 
                        confidence of about 98% of users being within MAR meters
                        of the specified Lat/Lon. */
} 
// Used in IPC - change with IPC rev. only
le_WifiCovMeasAndAdType; // Wifi Coverage-area Measurement (with AD)

// control & result - for pass through to ALE & use below
typedef struct
{
  le_WileFixControlFlagType z_Control;
  wl_ResultFlagsType        z_Result; // from WLM + added flags from WILE
} le_WifiFixCtrlResultType; // Used in IPC - change with IPC rev. only

// measurement set
typedef struct
{
  le_WifiFixCtrlResultType  z_CtrlResult;
                           
  /** Location-standard Timestamp of Measurements overall, with individual
      measurement ages vs. this timestamp in the array below. 
      See also cgps_TimeTickGetMsec(). */
  uint64                    t_TimeTickMeas_ms; 

  uint8                     u_cMeas; // how many in array below are valid
  le_WifiCovMeasAndAdType   z_Meas[LE_MAX_WIFI_COV_MEAS];
}
le_WifiCovFixRequestType; // Used in IPC - change with IPC rev. only

typedef enum
{
  CONS_UNCHECKED,   // unable to check consistency
  CONS_CONSISTENT,  // checked, and consistent
  CONS_INCONSISTENT // checked, and inconsistent
}
le_ConsistencyCheckResultType;

typedef struct
{
  uint8 u_PosInfoSrc; /* Source of the Position Estimate */
  /* populate with cgps_PosInfosrcType */
  boolean u_PosInfoValid; /* Validity indicator for the Position Information */
  uint16  w_GpsWeek;      /* GPS Week time reference for the Position */
  uint32  q_GpsMsec;      /* GPS Msec time reference for the Position */
  /* Lalitude, Longitude in Radians and Altitude in meters */
  DBL     d_PosLLa[3];    
  FLT     f_PosUnc99;       /* Horizontal Uncertainty in meters (circular) */
  FLT     f_AltUnc68;       /* Altitude Uncertainty in meters */
  pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/
  uint64  t_TimeTickMsec; /* Timetick time reference for the Position. */
  /* Should always be valid and used if the GPS time stamp is not valid*/
} ale_CellDBInputPosStructType; 

typedef struct
{
  /** Latitude in float has resolution of 2^6 * 1/2^23 = 1/2^17 degrees, 
      which is ~0.76 meters.
      Longitutde is 2x this or 1.5m resolution.
      This is just sufficient given Wifi AD (GTP) server provided accuracy is
      30-100m, and may improve toward 5-10m.
      Switching to DBL, or fixed-point uint32, is recommended for future higher 
      accuracy applicatons, like Wifi-Ranging.

      HEPE == Horizontal Estimated Position Error == RSS of east/north and/or 
      along/cross 1-sigma values 63%ile when circular underlying */

  FLT f_LatDeg; // latitude, WGS-84, in degrees, south negative
  FLT f_LonDeg; // longitude, WGS-84, in degrees, west negative
  FLT f_Hepe_m; // HEPE, meters
  uint64 t_TimeTick_ms; // time of validity, local clock time tick, msec
}
le_WifiCovLocType;

// An enumeration for type of ALE use of WifiCov
typedef enum
{
  ALE_WCL_REJECTED_VS_OTHER = 0,
  ALE_WCL_REJECTED_ALG_ERROR,
  ALE_WCL_ACCEPTED_NOT_BEST,
  ALE_WCL_ACCEPTED_AND_BEST
} ale_WclResultEnumType;

#endif  // _LE_TYPES_H
