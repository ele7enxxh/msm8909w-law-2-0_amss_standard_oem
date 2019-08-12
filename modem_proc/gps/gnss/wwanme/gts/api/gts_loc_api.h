#ifndef GTS_LOC_API_H
#define GTS_LOC_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Global Time Services Location Tech Header File

GENERAL DESCRIPTION
This file contains API definitions between Global Time Service module between
other Location Tech Modules.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/gts/api/gts_loc_api.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "gts_api.h"
#include "gnss_common.h"
#include "mgp_api.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
/* GTS Freq Est Structure */
typedef struct
{
  /* Time at the last temp Update */
  uint64 t_PrevQtime;    
  /* Time at the current temp Update */
  uint64 t_CurrQtime;    
  /* Freq Est based on Temp in 2^10 PPB */
  int32  l_FreqEst;      
  /* Time Uncertainity estimate based on temp table in Ms/Second */
  float  f_FreqEstUncMs; 
  /* FT Calibration state */
  uint8  u_FtCalState;
} gts_FreqEstInfoType;

/* GTS Freq Est Structure */
typedef struct
{
  boolean b_Enable;
} gts_HATStatusMsgType;


/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/
/*
 ******************************************************************************
 * Function gts_GetGpsTimeEst
 *
 * Description:
 *
 *  This function reads the current time from GTS module. This time could be 
 *  from any source like XO or LTE or Timetag or Time transfer. 
 *
 * Parameters:
 *
 *  pz_GpsTimeEst - Pointer to the GPS Time Estimate from GTS
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if Time was filled in correctly, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_GetGpsTimeEst( gps_ClockStructType* pz_GpsTimeEst );

/*
 ******************************************************************************
 * Function gts_PutGpsTimeEst
 *
 * Description:
 *
 *  This function creates a ME GPS Time estimate message and sends it GTS 
 *  module
 *
 * Parameters:
 *
 *  pz_GpsTimeEst - GPS Time Estimate information
 *  e_MESrc - ME Time source that provided this time
 *  pz_Qtime - Qtime at the Time Estimate instance
 *  q_FCount - FCount at the Time Estimate instance
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if Time was filled in correctly, FALSE otherwise
 *
 ******************************************************************************
*/
void gts_PutGpsTimeEst( const gps_ClockStructType* pz_GpsTimeEst,
                        mgp_TimeEstPutSourceIDEnum e_MESrc,
                        const gts_QtimeType* pz_Qtime,
                        U32 q_FCount);

/*
 ******************************************************************************
 * Function gts_SetHighAccuracyMode
 *
 * Description:
 *
 *  This function Indicates that Background positioning is enabled and need  
 *  high accuracy time with a increased power consumption. This will enable
 *  Time maintenance using Slowclock and Temperature compensation
 *
 * Parameters:
 *
 *  b_HighAccuracy - TRUE/FALSE to enable/Disable high accuracy time tracking
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_SetHighAccuracyMode( boolean b_HighAccuracy );

/*
 ******************************************************************************
 * Function gts_FreqEstUpdate
 *
 * Description:
 *
 *  Freq Est module will use this function to inject Freq Est and corresponding
 *  slow clock time to GTS
 *
 * Parameters:
 *
 *  pz_FreqEstInfo - Freq Estimate Information
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_FreqEstUpdate ( gts_FreqEstInfoType* pz_FreqEstInfo );

/*
 ******************************************************************************
 * Function gts_HATStatusUpdate
 *
 * Description:
 *
 *  Freq Est module will use this function to indicate GTS if HAT Enable/Disable
 *  was successful and what is the current status.
 *
 * Parameters:
 *
 *  b_Enable - Enable/Disable Status
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_HATStatusUpdate ( boolean b_Enable );

/*
 ******************************************************************************
 * Function gts_PutGpsPosEst
 *
 * Description:
 *
 *  Position Engine uses this api to inject GPS Time for a position fix
 *
 * Parameters:
 *
 *  pz_GpsTimeEst - Time Estimate from Position Fix
 *  pz_GpsPosEst - Position Estimate Information
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_PutGpsPosEst ( const gps_ClockStructType* pz_GpsTimeEst);

/*
 ******************************************************************************
 * Function gts_MotionInfoUpdate
 *
 * Description:
 *
 *  SLIM will use this function to inject motion data into GTS
 *
 * Parameters:
 *
 *  pz_MotionData - Motion Data Information
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_MotionInfoUpdate ( const slimInjectMotionDataIndMsgT* pz_MotionData );

/*
 ******************************************************************************
 * Function gts_ForceUseGtsTime
 *
 * Description:
 *
 *  Used by MC to determine if it should use GTS time instead of requesting
 *  Time tag or Time Transfer
 *
 * Parameters:
 *
 *  None 
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE - If GTS Time should be used by MC for regular GPS Sessions
 *
 ******************************************************************************
*/
boolean gts_ForceUseGtsTime ();

/*
 ******************************************************************************
 * Function gts_MapMESrcToGtsSrc
 *
 * Description:
 *
 *  ME Source into GTS Time source
 *
 * Parameters:
 *
 *  e_MESrc - ME Time Source Type
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  GTS Source Type
 *
 ******************************************************************************
*/
gts_TimeSrcType gts_MapMESrcToGtsSrc ( mgp_TimeEstPutSourceIDEnum e_MESrc );

/*
 ******************************************************************************
 * Function gts_MapGtsSrcToMESrc
 *
 * Description:
 *
 *  Converts GTS Source into ME Time source
 *
 * Parameters:
 *
 *  e_GtsSrc - GTS Time Source Type
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  ME Time Source Type
 *
 ******************************************************************************
*/
mgp_TimeEstPutSourceIDEnum gts_MapGtsSrcToMESrc( gts_TimeSrcType e_GtsSrc );

/*
 ******************************************************************************
 * Function gts_ProcDiagCmd
 *
 * Description:
 *
 *  Create and Send an IPC message to GTS module with the Diag command info 
 *
 * Parameters:
 *
 *  p_Data - Diag Command Packet Data
 *  u_Length - Length of the Diag Packet Data
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_ProcDiagCmd( U8* p_Data, U8 u_Length );

/*
 ******************************************************************************
 * Function gts_CalcUstmrAtFCount
 *
 * Description:
 *
 *  Generic Handler Function which will update Qtime at any provided FCount
 *
 * Parameters:
 *
 *  pz_Qtime - Qtime structure that will be updated
 *  q_FCount - Frame Count at which Qtime is estimated
 * 
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_CalcUstmrAtFCount( gts_QtimeType* pz_Qtime, U32 q_FCount );

#endif /* GTS_LOC_API_H */
