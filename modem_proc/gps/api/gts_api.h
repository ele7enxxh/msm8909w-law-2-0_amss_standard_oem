#ifndef GTS_API_H
#define GTS_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Global Time Services Header File

GENERAL DESCRIPTION
This file contains API definitions between Global Time Service module and
other modules outside GNSS.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/api/gts_api.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "comdef.h"
#include "sys_type.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef enum  
{
  GTS_CLIENT_INVALID,
  GTS_CLIENT_GNSS_ME,
  GTS_CLIENT_MAX = GTS_CLIENT_GNSS_ME
} gts_ClientType;


typedef enum
{
  /* Default - No valid Time Source */
  GTS_TIME_SRC_NONE,
  
  /* WWAN Based Time */
  GTS_TIME_SRC_LTE,
  GTS_TIME_SRC_WCDMA,
  GTS_TIME_SRC_CDMA,
  GTS_TIME_SRC_GSM,
  GTS_TIME_SRC_TDSCDMA,

  /* Time from GPS Satellites */
  GTS_TIME_SRC_GPS,
  
  /* Other Clock Source */
  GTS_TIME_SRC_FEST,
  GTS_TIME_SRC_SLOWCLK,
  GTS_TIME_SRC_EXTINJ,
  GTS_TIME_SRC_OTHER,
  GTS_TIME_SRC_MAX = GTS_TIME_SRC_OTHER
} gts_TimeSrcType;

/* Reference Time structure for Timetags */
typedef struct 
{
  /* Qtime is Valid */
  boolean b_QtimeValid;
  /* Qtime or 64 bit Dal Time @ Time Estimate in units of 19.2 Mhz */
  uint64  t_Qtime;
  /* Bias if applicable in Qtime units */
  float   f_QtimeBias;
} gts_QtimeType;

typedef struct
{
  uint32  q_VsrcFromFW[2]; 
  uint32  q_VsrcPhFromFW[2]; 
  uint32  q_OstmrFromFW[2];
  uint32  q_OstmrPhFromFW[2];
  uint64  t_RefDlTime;
} gts_VsrcInfoType;

typedef struct 
{
  uint32  q_Freq;    /* DL frequency - Same as lte_earfcn_t*/ 
  uint16  w_CellId;  /* Physical Cell ID */

  /* SIB8 TRUE: LTE NW is synchronized and supports 1x. FALSE: LTE NW is NOT synchronized */
  boolean b_Sib8Valid;   
  /* The CDMA2000 system time corresponding to the SFN boundary at or after the ending boundary of the SI-Window in which this Time Transfer was taken. The size is 52 bits and the unit is [CDMA2000 chips] */
  uint64  t_Sib8CdmaSystemTime; 
  
  boolean b_Sib16Valid; /* SIB16 System Time info*/
  uint64  t_Sib16GpsMsecs; /* The Abs GPS time at the SFN boundary in of 10 msecs */
  uint16  w_LeapSeconds; /* Number of leap seconds between UTC time and GPS time */
  
  uint16  w_Sfn;             /* System Frame Number at which data was collected */
  uint32  q_OStmrSfBoundary;  /* OSTMR at the boundary of the subframe */

  /* Latch Information */
  uint32  q_UstmrLatched;   /* USTMR Latched in 19.2 Mhz Units */
  uint32  q_OstmrLatched;   /* OSTMR Latched in 30.2 Mhz Units ( Ts ) */
  uint32  q_OstmrPhLatched; /* OSTMR Latched in 30.2 Mhz Units ( sub Ts )*/

  /* VSRC Information for PCC */
  /* Mark if the next field is valid or not */
  boolean b_OstmrVsrcValid; 
  gts_VsrcInfoType z_RefVsrcInfo;  
  gts_VsrcInfoType z_CurrVsrcInfo;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
}gts_LteTimeInfoType; 


typedef struct
{
  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  uint8 u_SubInfo;
  /* Cell Information */
  /* Base Station Identification code */
  uint8 u_BSIC;
  /* Control Channel Frequency */
  ARFCN_T z_Arfcn;

  /* Time Information */
  /* Frame Number 0..(26L*51L*2048L-1)  ~4.615ms frames */
  uint32 q_FrameNum;
  /* Time Slot Number 0..7 */
  uint8 u_TimeSlot;
  /* QSymbol Count (sub fn) */
  uint32 q_QSymCnt;

  /* GSM State */
  uint8 u_GL1State;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
} gts_GsmTimeInfoType;


typedef struct
{
  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  uint8 u_SubInfo;

  /* Cell Information */
  /* DL frequency and Primary scrambling code */
  uint16  w_Freq;
  uint16  w_PSC;

  /* Time Information */
  /* 10.3.6.75  System Frame Number  0..4095 10ms frames */
  uint16  w_SysFn;

  /* 0..307200 Chipx8 per frame */
  uint32  q_CX8Num;

  /* UMTS STMR Information including Fractional bits*/
  uint64 t_UmtsStmr;

  /* WCDMA State */
  uint8 u_WL1State;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
}gts_WcdmaTimeInfoType;


typedef struct
{
  /* Subscription Information – For MultiSim Support */
  uint8 u_SubInfo;
  /* Cell Information */
  /* DL frequency. */
  uint16  w_Freq;

  /* Cell Param ID */
  uint16  w_CellId;

  /* Time Information */
  /* 64-bits value in TDS chipx8 units at status dump, clocked @ 10.24 MHz */
  uint64  q_TdsTime;

  /* 30-bits value in TDS sub chipx8 units at status dump */
  uint32  q_TdsPhase;

  /* TDS L1 State 0 – Paging, 1- Traffic*/
  uint8 u_TDSL1State;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
}  gts_TdscdmaTimeInfoType;

typedef struct
{
  /* If the time below is valid or not */
  boolean b_Valid;

  /* Time Source */
  gts_TimeSrcType e_TimeSrc; 

  /* Absolute GPS Time related information */
  uint16  w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  uint32  q_GpsMsec;     /* GPS msec at reference tick [msecs] */
  float   f_ClkTimeBias; /* Clock bias [msecs]. Constraint to >-0.5 and <0.5 */
  float   f_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
  
  /* GPS Clock Specific Fields */
  boolean b_FCountValid; /* Mark if FCount below is valid */
  uint32  q_GpsFCount;    /* GPS FCount at the Time Estimate */
} gts_TimeType;

/* Callback Function for recieving Time updates */
typedef void ( * gts_TimeEstCbType )( const gts_TimeType * );

/* Request Time Info structure */
typedef struct
{
  gts_ClientType e_ClientInfo;
  boolean b_ContUpdates;
}gts_ReqTimeInfoType;

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/
/*
 ******************************************************************************
 * Function gts_ReqGpsTime
 *
 * Description:
 *
 *  This function requests for the latest time from GTS module 
 *
 * Parameters:
 *
 *  pz_ReqTimeInfo - Pointer related to requesting time info like client info
 *  and continous updates
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if the request was sent successfully, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_ReqGpsTime( gts_ReqTimeInfoType* pz_ReqTimeInfo );

/*
 ******************************************************************************
 * Function gts_LteTimeUpdate
 *
 * Description:
 *
 *  This function sends current LTE Time to GTS module using IPC
 *
 * Parameters:
 *
 *  pz_LteTime - Pointer to the LTE Time structure 
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

void gts_LteTimeUpdate( const gts_LteTimeInfoType* pz_LteTime );

/*
 ******************************************************************************
 * Function gts_GsmTimeUpdate
 *
 * Description:
 *
 *  This function sends current GSM Time to GTS module using IPC
 *
 * Parameters:
 *
 *  pz_GsmTime - Pointer to the GSM Time structure 
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

void gts_GsmTimeUpdate( const gts_GsmTimeInfoType* pz_GsmTime );

/*
 ******************************************************************************
 * Function gts_WcdmaTimeUpdate
 *
 * Description:
 *
 *  This function sends current WCDMA Time to GTS module using IPC
 *
 * Parameters:
 *
 *  pz_WcdmaTime - Pointer to the WCDMA Time structure 
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

void gts_WcdmaTimeUpdate( const gts_WcdmaTimeInfoType* pz_WcdmaTime );

/*
 ******************************************************************************
 * Function gts_TdscdmaTimeUpdate
 *
 * Description:
 *
 *  This function sends current TDS Time to GTS module using IPC
 *
 * Parameters:
 *
 *  pz_TdscdmaTime - Pointer to the TDS Time structure 
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

void gts_TdscdmaTimeUpdate( const gts_TdscdmaTimeInfoType* pz_TdscdmaTime );

/*
 ******************************************************************************
 * Function gts_RegForGtsTime
 *
 * Description:
 *
 *  This function allows clients to register a callback function to GTS for
 *  sending Time updates
 *
 * Parameters:
 *
 *  e_ClientInfo - Information of which client is registering
 *  p_CbFunction - Callback function which will be invoked by GTS to send
 *  time information to the client
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if registration was successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_RegForGtsTime( gts_ClientType e_ClientInfo, gts_TimeEstCbType p_CbFunction ); 

/*
 ******************************************************************************
 * gts_GetQtime
 *
 * Function description:
 *
 * This function returns the Qtime @ the calling instant
 *
 * Parameters:
 *  pz_Qtime - Qtime structure ( Validity,Time and Bias ) is updated
 *
 * Return value:
 *  None, but callers should check pz_Qtime->b_QtimeValid which will be TRUE 
 *  if the get time was successful, FALSE otherwise
 *
 ******************************************************************************
*/
void gts_GetQtime ( gts_QtimeType* pz_Qtime );

#endif /* GTS_API_H */

