/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *  Copyright (c) 2015 Qualcomm Technologies, Inc. 
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *----------------------------------------------------------------------------
 */
#ifndef TDP_COMMON_H
#define TDP_COMMON_H
/*=============================================================================

                             GPS Common Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for the entire GPS module (essentially the shared structures).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/tdp_common.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-2-14   hs  First version. Added support for TDP feature.




=============================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "gnss_common.h"
#include "loc_wwan_me_api.h"


/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/
#define NUM_MAX_TDP_MEAS     (24)  /* Max # of TDP meas in a TDP Upload meas block */

/*=============================================================================

       Typedefs

=============================================================================*/

/* Typedef struct for all the response/request from Searcher to PDSM.
*/

/*=============================================================================
   TDP Clock Structure
=============================================================================*/
typedef  struct
{
   /* Boolean to indicate System Rtc is valid */
   boolean     b_SystemRtcValid;

   /* System Rtc */
   uint64      t_SystemRtcMs;  

   /* GPS Time  */
   gps_TimeStructType z_GpsTime; 

} tdp_ClockStructType;


/*=============================================================================
   TDP Base Station Location
=============================================================================*/
typedef struct
{
  /* ECEF Co-ordinates of the Base Station */
  DBL d_BsEcef[3];

  /* Base Station Hepe */
  FLT f_BsHorUnc63Meters;

  /* Base Station 1 sigma Alt Unc */
  FLT f_BsAltUnc68Meters;

  /* Forward Link Correction of the Base station in meters */
  FLT f_FlcMeters; 

  /* Forward Link Correction Uncertainty in meters */
  FLT f_FlcUnc68Meters; 
  
}tdp_BsLocStruct;


/*=============================================================================
   TDP Base station Structure that includes the BS location struct and 
   the Lte Cell Info
=============================================================================*/
typedef struct
{
   /* LTE Cell Info */
   loc_wwan_me_LteCellInfoType z_CellInfo;

   /* BS Location data */
   tdp_BsLocStruct z_BsLocStruct;

}tdp_BsStructType;


/*=============================================================================
   TDP TOA Struct
=============================================================================*/
typedef struct
{
  /* Local clock counter in Msec when CRS measurement was acquired */
  uint32 q_Msec;

  /* Local clock SubMsec when CRS measurement was acquired (units of msec) */
  FLT    f_SubMsec;

} tdp_ToaStructType;


/*=============================================================================
   TDP LTE Measurement Structure
=============================================================================*/
typedef struct
{
  /*! TOA of the LTE measurement */
  tdp_ToaStructType z_Toa;

/*! Meas uncertainity in meters */
  uint16 w_MeasUncMeters;

  /*! Physical cell ID */
  uint16 w_PhyCellId;

  /*! Avg Reference signal - received power [0...97] in dBm, Q7 format */
  uint8 u_RsrpAvg;
  
} tdp_LteMeasType;


/*=============================================================================
   TDP Measurement structure includes the LTE meas and BS data
=============================================================================*/
typedef struct
{
  tdp_LteMeasType z_LteMeas;
  tdp_BsStructType z_BsStruct;
} tdp_MeasStructType;


/*=============================================================================
   TDP Measurement Block (for positioning)
=============================================================================*/
typedef struct
{
  /*! Number of Cell measurements being reported */
  uint8 u_NumOfCellMeas; 

  /* Terrain Altitude */
  FLT f_TerrainAltMeters;

  /* Terrain Alt Uncertainty */
  FLT f_TerrainAltUnc68Meters;

  /* TDP Clock Structure */
  tdp_ClockStructType z_TdpClock; 

  /* TDP Meas Array */
  tdp_MeasStructType z_TdpMeas[N_ACTIVE_TDP_CHAN];

} tdp_MeasBlkStructType;

/*=============================================================================
   Structs used for TDP upload
=============================================================================*/




typedef struct 
{
  struct 
  {
     uint8 b_IsMccValid : 1;
     uint8 b_IsMncValid : 1;
     uint8 b_IsTacValid : 1;    
     uint8 b_IsCellIdValid : 1;
  } z_CellIdMask;
	
  uint16 w_PhyId;  /* Physical Id 0-503 */
  uint32 q_Earfcn; /* Downlink center frequency - Integer (0..262143) */
  uint16 w_Mcc; /* Layer 1 region ID */
  uint16 w_Mnc; /* Layer 2 region ID */
  uint16 w_Tac; /* Layer 3 region ID */
  uint32 q_CellId; /* Layer 4 region ID */

} tdp_LteCellIdStructType;

/* LTE supported bandwidths */
typedef enum
{
  /* 1.4MHz bandwidth */
  TDP_CRS_BW_6 = 0,
  /* 3MHz bandwidth */
  TDP_CRS_BW_15,
  /* 5MHz bandwidth */
  TDP_CRS_BW_25,
  /* 10MHz bandwidth */
  TDP_CRS_BW_50,
  /* 15MHz bandwidth */
  TDP_CRS_BW_75,
  /* 20MHz bandwidth */
  TDP_CRS_BW_100,
  /* Unknown Bandwidth */
  TDP_CRS_BW_UNKNOWN
} tdp_LteSystemBandwidthEnumTypeVal;
typedef uint8 tdp_LteSystemBandwidthEnumType;

typedef struct
{
  struct 
  {
    uint8 b_IsSib8Valid : 1;
    uint8 b_IsSib16Valid : 1;
  }z_ConfigMask;

  tdp_LteSystemBandwidthEnumType e_CellSystemBw; /* LTE DL bandwidth of 
                                                      the measurement */

  boolean b_CyclicPrefix; /* FALSE: normal
                          TRUE : extended */

  boolean b_AntennaPorts; /* FALSE: 1 or 2 transmit antennas
                             TRUE : 4 transmit antennas */

  uint64  t_Sib8CdmaSystemTime; /* The CDMA2000 system time corresponding to 
                                   the SFN boundary at or after the ending 
                                   boundary of the SI-Window in which this Time 
                                   Transfer was taken. The size is 52 bits and 
                                   the unit is [CDMA2000 chips] */

  uint64  t_Sib16FullGpsMsecs; /* The Full gps time at the SFN boundary in
                                   units of msecs */

  uint16 w_LeapSeconds;         /* UTC leap second */
  

} tdp_ServingCellConfigType;


typedef enum
{
  TDM_MEASTYPE_PRS = 0,
  TDP_MEASTYPE_OPCRS,
  TDP_MEASTYPE_UKCRS 
} tdp_RangingMeasEnumTypeVal;
typedef uint8 tdp_RangingMeasEnumType;


typedef struct 
{
   struct 
   {
     uint8 b_IsSignalPowerValid : 1;
     uint8 b_IsSignalQualityValid : 1;
     uint8 b_IsRxTxTimeDiffValid : 1;
     uint8 b_IsTotMsValid : 1;
     uint8 b_IsServing : 1;
   } z_MeasMask;

   uint8 u_SignalPower; /* Range (-140 dBm to -44 dBm) with 1 dBm 
                           resolution. Refer to GTP the ICD. */

   uint8 u_SignalQuality; /* Reference signal received quality.
                           For PRS measurements, the signalQuality will 
                           be RSRQ as defined in LPP. Range (-19.5 dB 
                           to -3dB with 0.5 dB resolution. The same 
                           values and encoding will be used for CRS 
                           measurements. Refer to GTP the ICD.*/

   uint16 w_RxTxTimeDiff; /* Rx-Tx time difference, as defined in 
                            LPP. Range (2 TS to 20472 TS). The same 
                            values and encoding will be used for PRS 
                            and CRS measurements. Refer to GTP the ICD.*/

   tdp_RangingMeasEnumType e_RangingMeasType; /* PRS, CRS, UKCRS */

   tdp_LteSystemBandwidthEnumType e_RangingMeasBW; /* LTE DL BW */

   boolean b_FineClockSyncFlag; /* TRUE: LTE-GNSS Time sync applied */
 
   uint16 w_TotMs; /* Offset of the observed signal’s time of 
                   transmission from start of LTE frame structure in 
                   units of ms. Value is calculated as: (System Frame 
                   Number) * 10 +  sub_frame_offset_within_frame.
                   Required to be present for the serving cell.                
                   Optionally present for neighboring cells. When
                   present for neighboring cells, the value shall apply 
                   to the neighbor’s */

   int16 x_rToaMs; /* TOA measurements are relative to the GPS time in
                    the position estimate for this lteTdpUploadData
                    instance. Ms component of rToa Range (~ -32.7 sec
                    to 32.7 sec ) */   

   int32 l_rToaNs; /* TOA measurements are relative to the GPS time in 
                    the position estimate for this lteTdpUploadData                
                    instance. Ns component of rToa Range (-0.5 ms to 
                    0.5 ms) */

   uint8 u_rToaMeasUncUnPropK; /* K-value encoded measurement uncertainty, 
                              computed based on signal strength, multipath, 
                              etc.*/

   uint8 u_rToaClockUncPropK; /* K-value encoded clock Uncertainty. If the 
                             fineClockSyncFlag is set to 1, then this  
                             field represents the clock uncertainty wrt 
                             the GPS time of fix. Else, this represents 
                             the relative clock uncertainty growth from 
                             the first measurement of the block. */

} tdp_UploadMeasStructType;

typedef struct
{
  tdp_LteCellIdStructType z_CellId;

  tdp_UploadMeasStructType z_Meas;

} tdp_MeasAndCellDataStructType;

typedef struct 
{
  tdp_ServingCellConfigType z_ServingCellConfig; /* Serving cell cfg */

  uint8 u_NumTdpMeas;/* Number of measurements in the scanlist below */

  tdp_MeasAndCellDataStructType z_MeasAndCellData[NUM_MAX_TDP_MEAS]; 
                                                      /* Max 24 meas */

} tdp_MeasAndCellDataBlkStructType;

typedef enum
{
  TDP_SCAN_METHOD_ONE = 0,
  TDP_SCAN_METHOD_MAX = 0xFFFF
}tdp_ScanSelectionMethodEnumType;

typedef struct
{
  boolean b_Valid;         /* TRUE, if the configuration is valid */
  boolean b_UploadControl; /* 0: Disable TDP upload for subsequent transactions 
                              1: Enable TDP upload for subsequent transactions 
                                (default)*/

  uint16 w_MaxNumScanListPerCell; /* Maximum number scanlists 
                                    (instances of lteTdpUploadData) that may 
                                    be uploaded per serving cell record*/

  uint16 w_GridSizeDeg;  /* Recommended spacing between scanlist collection 
                            points in the unit of 0.00001 degree of latitude
                            and longitude.*/

  uint16 w_MinScanIntervalUploadSec; /* Recommended minimum time between scanlists
                                        for upload in seconds*/

  uint16 w_MaxPosUncUpload; /* Maximum position uncertainty for scanlists for 
                               uploads in meters */

  uint8 u_ScanSelectionParameter; /* A parameter for a cost function calculation,
                                     used for selection among collected scans 
                                     within a grid segment.*/

  tdp_ScanSelectionMethodEnumType e_ScanSelectionMethod; 
} tdp_UploadControlConfigType;


#endif /* TDP_COMMON_H */