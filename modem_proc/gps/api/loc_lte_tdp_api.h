#ifndef LOC_LTE_TDP_API_H
#define LOC_LTE_TDP_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Location LTE TDP API Header File

GENERAL DESCRIPTION
This file contains API definitions between Location LTE TDP module and
other modules outside GNSS.

Copyright (c) 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/api/loc_lte_tdp_api.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "comdef.h"
#include "cgps_api.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/



/*--------------------------------------------------------------------------
 * Op CRS Type Declarations
 *-----------------------------------------------------------------------*/

typedef enum
{
  LOC_LTE_OPCRS_MEAS_SUCCESS = 0,
  LOC_LTE_OPCRS_MEAS_ERROR,
  LOC_LTE_OPCRS_MEAS_MAX
}
loc_lte_tdp_OpCrsMeasStatusTypeVal;

typedef uint8 loc_lte_tdp_OpCrsMeasStatusType;

typedef struct
{
  uint16  w_Sfn;
  boolean b_Valid;
  uint64  t_VsrcFromFW[2]; 
  uint32  q_OstmrFromFW[2];
  uint32  q_OstmrPhFromFW[2];
  uint64  t_RefDlTime; 
} 
loc_lte_tdp_OpCrsVsrcInfoType;

/*--------------------------------------------------------------------------
 * WB CRS Type Declarations
 *--------------------------------------------------------------------------
*/

typedef enum
{
  LOC_LTE_WBCRS_MEAS_SUCCESS = 0,
  LOC_LTE_WBCRS_MEAS_ERROR,
  LOC_LTE_WBCRS_MEAS_MAX
}
loc_lte_tdp_WbCrsMeasStatusTypeVal;

typedef uint8 loc_lte_tdp_WbCrsMeasStatusType;

typedef enum
{
  /* 1.4MHz bandwidth */
  LOC_LTE_TDP_BW_6 = 0,
  /* 3MHz bandwidth */
  LOC_LTE_TDP_BW_15,
  /* 5MHz bandwidth */
  LOC_LTE_TDP_BW_25,
  /* 10MHz bandwidth */
  LOC_LTE_TDP_BW_50,
  /* 15MHz bandwidth */
  LOC_LTE_TDP_BW_75,
  /* 20MHz bandwidth */
  LOC_LTE_TDP_BW_100,
  /* Unknown Bandwidth */
  LOC_LTE_TDP_BW_UNKNOWN
} loc_lte_tdp_WbCrsBandwidthVal;

typedef uint8 loc_lte_tdp_WbCrsBandwidthType;

typedef struct
{
  uint16  w_Sfn;
  boolean b_Valid;
  uint64  t_VsrcFromFW[2]; 
  uint32  q_OstmrFromFW[2];
  uint32  q_OstmrPhFromFW[2];
  uint64  t_RefDlTime; 
} 
loc_lte_tdp_WbCrsVsrcInfoType;

/* Reference Time structure for Timetags */
typedef struct 
{
  /* Ref Time */
  boolean                  b_RefTimeValid;
  /* Ref Time Validity */
  uint64                   t_RefTime;
} lte_LL1_ref_time_s;

typedef struct
{
  boolean b_Sib8Valid;   /* SIB8 "cdma-EUTRA-Synchronization" info. 
                              TRUE: LTE NW is synchronized.
                              FALSE: LTE NW is NOT synchronized */

  uint64  t_Sib8CdmaSystemTime; /* The CDMA2000 system time corresponding to 
                           the SFN boundary at or after the ending 
                           boundary of the SI-Window in which this Time 
                           Transfer was taken. The size is 52 bits and 
                           the unit is [CDMA2000 chips] */

  boolean b_Sib16Valid; /* SIB16 System Time info*/

  uint64  t_Sib16GpsMsecs; /* The Full gps time at the SFN boundary in units of msecs */

  uint16  w_LeapSeconds; /* Number of leap seconds between UTC time and GPS time */

  uint16 w_Subfn;        /* Sub Frame Number of this WbCrs measurement */

  boolean b_OstmrVsrcValid; /* Mark if the next field is valid or not */
  loc_lte_tdp_WbCrsVsrcInfoType z_RefVsrcInfo[3];
  loc_lte_tdp_WbCrsVsrcInfoType z_SFNVsrcInfo[3];

  lte_LL1_ref_time_s  z_RefTimeInfo;

}
loc_lte_tdp_WbCrsTimeInfoType;


typedef struct
{
  void *p_WbCrsMeasData;  /* Pointer to shared memory block storing WbCrs measurement */

  loc_lte_tdp_WbCrsMeasStatusType  e_MeasStatus; /* Measurement Status. Enum of type 'loc_lte_WbCrs_MeasStatusType' */

  uint8 u_CarrierIdx;     /* 0=PCC, 1=SCC_0, and so on */

  uint8 e_DlBandwidth; /* LTE DL bandwidth of the measurement. Enum of type 'loc_lte_tdp_WbCrsBandwidthType' */

  uint8 u_SignalPower;  /* Range (-140 dBm to -44 dBm) with 1 dBm 
                                                   resolution. */

  uint8  u_SignalQuality; /* Reference signal received quality.
                                                   For PRS measurements, the signalQuality will 
                                                   be RSRQ as defined in LPP. Range (-19.5 dB 
                                                   to -3dB with 0.5 dB resolution. The same 
                                                   values and encoding will be used for CRS 
                                                   measurements. */

  boolean w_RxTxTimeDiffValid; //--- only valid in connected mode- do we need this or just use u_lte_state below to determine validity?

  uint16 w_RxTxTimeDiff;   /* Rx-Tx time difference, as defined in LPP. 
                                                   Range (2 TS to 20472 TS)
                                                   The same values and encoding will be used 
                                                   for PRS and CRS measurements. */

  loc_lte_tdp_WbCrsTimeInfoType z_TimeInfo; /* LTE related timing parameters */

  uint8 u_lte_state; /*State of LTE at the time of the measurement*/

} loc_lte_tdp_WbCrsMeasInfoType; 


/*--------------------------------------------------------------------------
 * Op CRS Type Declarations
 *--------------------------------------------------------------------------
*/

typedef struct
{
  boolean b_Sib8Valid;   /* SIB8 "cdma-EUTRA-Synchronization" 
                            info. 
                            TRUE: LTE NW is synchronized.
                            FALSE: LTE NW is NOT synchronized */

  uint64  t_Sib8CdmaSystemTime; /* The CDMA2000 system time corresponding to 
                                   the SFN boundary at or after the ending 
                                   boundary of the SI-Window in which this Time 
                                   Transfer was taken. The size is 52 bits and 
                                   the unit is [CDMA2000 chips] */

  boolean b_Sib16Valid; /* SIB16 System Time info*/

  uint64  t_Sib16GpsMsecs; /* The Full gps time at the SFN boundary in units of 
                              msecs */

  uint16  w_LeapSeconds; /* Number of leap seconds between UTC time and GPS 
                            time */


  uint16 w_Sfn;            /* System Frame Number of this OpCRS measurement */

  uint8 u_SubFn;          /* Subframe number of this OpCRS measurement */            

  uint32  q_OStmrSfBoundary;  /* O-STMR value at the boundary of the subframe 
                                 in which CRS measurement was made */

  uint32  q_UstmrLatched; /* Universal STMR value latched at the same time as 
                             O_STMR (in 19.2MHz units) */

  uint32  q_OstmrLatched; /* OSTMR value latched at the same time as USTMR */

  uint32  q_OstmrPhaseLatched; /* OSTMR Phase value latched at the same time as USTMR */

  uint32  q_UstmrFwDLSubFN;     /* Universal STMR value at DL SF according to FW 
                                (in 19.2MHz units) */

  uint32  q_OstmrFwDLSubFN; /* OSTMR value at DL Sub Fn using VSTMR API on USTMR from FW */

  uint32  q_OstmrPhFwDLSubFN; /* OSTMR phase value at DL Sub Fn using VSTMR API on USTMR from FW */

  uint32  q_UstmrFwDLSubFnNoAdj;     /* Universal STMR value at DL SF according to FW 
                                (in 19.2MHz units) */

  uint32  q_OstmrFwDLSubFnNoAdj; /* OSTMR value at DL Sub Fn as per FW */

  boolean b_OstmrVsrcValid; /* Mark if the next field is valid or not */
  loc_lte_tdp_OpCrsVsrcInfoType z_RefVsrcInfo[3];
  loc_lte_tdp_OpCrsVsrcInfoType z_SFNVsrcInfo[3];

  cgps_RefInfoType  z_RefTimeInfo;

  /* For Unit Testing purpose */
  boolean b_LatchTTr;
  cgps_LteTTLatchedDataType z_LatchedData[8];
  uint32 q_UstmrLatchTTr[8];
} 
loc_lte_tdp_OpcrsTimeInfoType;  


typedef struct
{
  void *p_OpcrsMeasData;  /* Pointer to shared memory block storing OpCRS
                             measurement */

  loc_lte_tdp_OpCrsMeasStatusType  e_MeasStatus; /* Measurement Status. Enum of type 
                                                    'loc_lte_opcrs_MeasStatusType' */

  uint32 q_OccasionSeqNum; /* Unique identifier sent by LTE FW along with 
                              measurement */

  uint16 w_PhyCellId;      /* Physical Cell ID of the measurement */

  uint8  u_CellType;       /* 0=Serving, 1=Nbr */

  uint16 w_Sfn;            /* System Frame Number of this OpCRS measurement */

  uint8 u_SubFn;          /* Subframe number of this OpCRS measurement */

  uint8 u_CarrierIdx;     /* 0=PCC, 1=SCC_0, and so on */

  uint8 e_DlBandwidth; /* LTE DL bandwidth of the measurement. Enum of type 
                                                'loc_lte_tdp_OpCrsBandwidthType' */

  uint32 q_Earfcn;  /* Downlink center frequency - Integer (0..262143)*/

  uint8 u_SignalPower;  /* Range (-140 dBm to -44 dBm) with 1 dBm 
                           resolution. */

  uint8  u_SignalQuality; /* Reference signal received quality.
                           For PRS measurements, the signalQuality will 
                           be RSRQ as defined in LPP. Range (-19.5 dB 
                           to -3dB with 0.5 dB resolution. The same 
                           values and encoding will be used for CRS 
                           measurements. */

  uint16 w_RxTxTimeDiff;   /* Rx-Tx time difference, as defined in LPP. 
                           Range (2 TS to 20472 TS)
                           The same values and encoding will be used 
                           for PRS and CRS measurements. */

  boolean b_CyclicPrefix; /* FALSE: normal
                             TRUE : extended */

  boolean b_AntennaPorts;	  /* FALSE: 1 or 2 transmit antennas
                             TRUE : 4 transmit antennas */

  loc_lte_tdp_OpcrsTimeInfoType z_TimeInfo; /*LTE related timing parameters*/

  uint8 u_lte_state; /*State of LTE at the time of the measurement*/

} loc_lte_tdp_OpcrsMeasInfoType;



/*--------------------------------------------------------------------------
 * WB CRS Function Definitions
 *--------------------------------------------------------------------------
*/

/*
 ******************************************************************************
 * Function: loc_lte_tdp_WbCrsMeasAvail
 *
 * Description:
 *  This API is called by ML1 to send TDP WB CRS meas to OC task
 *
 * Parameters: p_MeasResultsInfo - Pointer to WB Crs Meas
 *   
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_WbCrsMeasAvail(loc_lte_tdp_WbCrsMeasInfoType *pz_MeasResultsInfo);


/*--------------------------------------------------------------------------
 * OpCRS Function Definitions
 *--------------------------------------------------------------------------
*/

/*
 ******************************************************************************
 * Function: loc_lte_tdp_MeasResultsAvail
 *
 * Description:
 *  This API is called by ML1 to send TDP OpCRS meas to OC task
 *
 * Parameters: p_MeasResultsInfo - Pointer to OpCrs Meas
 *   
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_MeasResultsAvail(loc_lte_tdp_OpcrsMeasInfoType *pz_MeasResultsInfo);

#endif /* #ifndef LOC_LTE_TDP_API_H */