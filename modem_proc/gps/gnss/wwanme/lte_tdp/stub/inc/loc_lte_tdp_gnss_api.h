#ifndef LOC_LTE_TDP_GNSS_API_H
#define LOC_LTE_TDP_GNSS_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Location LTE OTDOA - GNSS API Header File

GENERAL DESCRIPTION
This file contains API definitions between Location LTE OTDOA module and
other GNSS modules.

===========================================================================

 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/lte_tdp/stub/inc/loc_lte_tdp_gnss_api.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "comdef.h"
#include "loc_lte_otdoa_gnss_api.h"


/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

/* TDP Diag cmds will start from 100 */
#define LOC_LTE_TDP_DIAG_SUBCMD_START             100

/* Sub command to enable OpCrs meas collection */
#define LOC_LTE_TDP_DIAG_ENABLE_OPCRS_SUBCMD      LOC_LTE_TDP_DIAG_SUBCMD_START

/* Sub command to enable OpCrs meas collection */
#define LOC_LTE_TDP_DIAG_DISABLE_OPCRS_SUBCMD     101


/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef struct 
{
  uint8 u_Reserved;
}loc_lte_tdp_CmdType;

typedef struct
{
  uint32  q_GnssRtc; /* GNSS RTC value latched at the same time as USTMR */
  uint32  q_GnssPhase; /* GNSS RTC Phase count at Latch */
  uint32  q_SampleCount;   /* GNSS RTC sample and chip counts @ at Latch */
  uint32  q_Ustmr;   /* USTMR latch in 19.2MHz untis) */
} loc_lte_tdp_LatchedDataType;

/* CRS Time stamping related information */
/* Complete GPS Time information at either GPS Fix or CRS Meas */
typedef struct
{
  uint8   b_Valid;   /* GPS Time validity flag TRUE: valid, FALSE: invalid */
  uint16  w_GpsWeek; /* GPS Week estimate */ 
  uint32  q_GpsMsec; /* GPS Ms estimate */ 
  FLT     f_ClkTimeUnc1SigmaMs; /* Time Uncertainity Estimate at the GPS Time */ 
  FLT     f_ClkTimeBias;  /* GPS Sub Ms or Bias at the GPS Time */ 
  uint32  q_FCountEst; /* GPS FCount at Time estimate */

  /* Latched Values */
  uint32  q_GnssRtc; /* GNSS RTC value latched at the same time as USTMR */
  uint32  q_GnssPhase; /* GNSS RTC Phase count at Latch */
  uint32  q_SampleCount;   /* GNSS RTC sample and chip counts @ at Latch */
  uint32  q_Ustmr;   /* USTMR latch in 19.2MHz untis) */
  uint64  t_RefTime; /* DAL System time captured close to the HW latch */

  loc_lte_tdp_LatchedDataType z_LatchedData[8];
} loc_lte_tdp_GpsFixTimeInfoType;  


typedef struct
{
  uint16 w_GpsWeek;
  uint32 q_GpsMsec;
} loc_lte_tdp_GpsTimeStructType;


/* TDP upload config */
typedef struct
{
  FLT f_MeasTunc1SigmaThresholdMeters; /* Tunc threshold to be applied to meas beyond which
                           measurement will not be reported to TLE */
} loc_lte_tdp_UploadConfigStructType;

/*--------------------------------------------------------------------------
 * Function Prototypes
 *-----------------------------------------------------------------------*/

/*
 ******************************************************************************
 * Function: loc_lte_tdp_upload_enable
 *
 * Description:
 *  This function is used by TLE to send TDP Upload enable cmd to OC task
 *
 * Parameters: pz_TdpUploadCmd - Pointer to loc_lte_tdp_CmdType
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_upload_enable( loc_lte_tdp_CmdType *pz_TdpUploadCmd );

/*
 ******************************************************************************
 * Function: loc_lte_tdp_upload_disable
 *
 * Description:
 *  This function is used by TLE to send TDP Upload disable cmd to OC task
 *
 * Parameters: pz_TdpUploadCmd - Pointer to loc_lte_tdp_CmdType
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_upload_disable( loc_lte_tdp_CmdType *pz_TdpUploadCmd );


/*
 ******************************************************************************
 * Function: loc_lte_tdp_upload_req_meas
 *
 * Description:
 *  This function is used by TLE to send TDP Upload disable cmd to OC task
 *
 * Parameters: pz_TdpUploadCmd - Pointer to loc_lte_tdp_CmdType
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_upload_req_meas( loc_lte_tdp_GpsTimeStructType *pz_GpsTime );

/*
 ******************************************************************************
 * Function: loc_lte_tdp_ClockUpdate
 *
 * Description:
 *  This function is used by MC to send clock update after GPS Fix
 *
 * Parameters:
 *  pz_GnssClock - GPS Time information (Week, Ms, Unc, Bias, FCount)
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_ClockUpdate( gnss_ClockStructType *pz_GnssClock, 
                              cgps_ComTimeType z_LatchInfo[] );

/*
 ******************************************************************************
 * Function: loc_lte_tdp_send_upload_config
 *
 * Description:
 *  This function is used by TLE to send TDP upload config params to WWAN ME
 *
 * Parameters:
 *  pz_Config - pointer to TDP Upload Config structure
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
void loc_lte_tdp_send_upload_config( loc_lte_tdp_UploadConfigStructType *pz_Config );

/*
 ******************************************************************************
 * Function: loc_lte_tdp_ProcControlApiMsg
 *
 * Description:
 *  Wrapper for the TDP Control Message processor
 *
 * Parameters:
 *  None
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void loc_lte_tdp_ProcControlApiMsg ( const os_IpcMsgType *const p_Msg );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_ProcMeasprocCustomMsg
 *
 * Description:
 *  Wrapper for the OMP task Message processor for processing TDP related msgs
 *
 * Parameters: p_Msg - Pointer to the IPC payload
 *  
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void loc_lte_tdp_ProcMeasprocCustomMsg ( const os_IpcMsgType *const p_Msg );

/*
 ******************************************************************************
 * Function: loc_lte_tdp_lloc_ProcDiagPkt
 *
 * Description:
 *  Wrapper for Lower Level TDP commands
 *
 * Parameters: pz_DiagCmd - Pointer to Diag cmd
 *  
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void loc_lte_tdp_lloc_ProcDiagPkt(loc_lte_otdoa_DiagCmdType *pz_DiagCmd);

/*
 ******************************************************************************
 * Function: loc_lte_tdp_lloc_HandleOtdoaSessionStateChange
 *
 * Description: Wrapper to Function to handle OTDOA session start and stop
 *
 * Parameters: b_IsSessStart - bool to indicate if E911 session is ongoing
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
void loc_lte_tdp_lloc_HandleOtdoaSessionStateChange(boolean b_IsSessStart);

#endif