/*----------------------------------------------------------------------------
 *  Copyright (c) 2016 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
    Copyright (c) 2016 Qualcomm Technologies, Inc. 
    Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 *----------------------------------------------------------------------------
 */
#ifndef _SM_LOC_LTE_OTDOA_H_
#define _SM_LOC_LTE_OTDOA_H_


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Session Manager PLLTE->OTDOA API

GENERAL DESCRIPTION
  This module makes up the Position Determination API between Session Mananger
  and Position Location LTE_OTODA manager software.

  The Session Manager API file is used by SM and PLLTE module to interface with
  OTDOA world


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/inc/sm_loc_lte_otdoa.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/27/14   rk      Added LPP RSTD >24 measurement processing support
05/07/14   rk      Enhanced earfcn to support extended value for LTE.
05/18/11   rc    Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
02/12/10   vp      Added Sv polynomial function for OEM DRE enablement
06/10/09   gk      Added Get UTC offset from NV
01/12/09   atien   XTRA-T support

09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
04/28/08   gk      On-Demand Changes
08/01/07   rw      Add GPS report time info for XTRA feature
12/01/06   jw      Made changes to support modified struct sm_GpsMeasRptStructType
04/04/06   sk      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "gnss_common.h"
#include "aries_os_api.h"




#define SM_LOC_LTE_PRTL_TYPE_UMTS_UP 8 // this value is used as u_sess_type

#define SM_LOC_LTE_MODE_UE_ASSISTED  2 // this value is used for q_operation_mode
#define SM_LOC_OTDOA_MODE 7 // this value is used for q_operation_mode

#define SM_OTDOA_NEIGHBOUR_FREQ_INFO_ELEMENTS 3
#define SM_LOC_LTE_OTDOA_MAX_NGBR_CELLS        24
#define SM_LOC_LTE_OTDOA_MAX_HYPOTHESIS        3
#define SM_LOC_LTE_FTA_MAX_CELLS               16
#define SM_PRS_INFO_PRS_MUTINGINFO_R9_PO2_R9 1
#define SM_PRS_INFO_PRS_MUTINGINFO_R9_PO4_R9 2
#define SM_PRS_INFO_PRS_MUTINGINFO_R9_PO8_R9 3
#define SM_PRS_INFO_PRS_MUTINGINFO_R9_PO16_R9 4

/**************************************************************/
/*                                                            */
/*  ECGI                                                      */
/*                                                            */
/**************************************************************/

typedef struct {
   uint8 mcc[3];
   uint8 mnc[3];
   uint32 cellidentity;
   uint8 num_mnc_digits;
   uint8 cell_id_numbits;
} sm_ECGI;

typedef struct sm_PRS_Info_prs_MutingInfo_r9 {
   uint8 t;
   union {
      /* t = 1 */
      uint8 po2_r9[4];
      /* t = 2 */
      uint8 po4_r9[4];
      /* t = 3 */
      uint8 po8_r9[4];
      /* t = 4 */
      uint8 po16_r9[4];
   } u;
} sm_PRS_Info_prs_MutingInfo_r9;
/**************************************************************/
/*                                                            */
/*  PRS_Info                                                  */
/*                                                            */
/**************************************************************/

typedef struct {
   struct {
      unsigned prs_MutingInfo_r9Present : 1;
   } m;
   uint32 prs_Bandwidth;
   uint16 prs_ConfigurationIndex;
   uint32 numDL_Frames;
   sm_PRS_Info_prs_MutingInfo_r9 prs_MutingInfo_r9;
} sm_PRS_Info;

/**************************************************************/
/*                                                            */
/*  sm_OTDOA_NeighbourCellInfoElement                            */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_NeighbourCellInfoElement {
   struct {
      unsigned cellGlobalIdPresent : 1;
      unsigned earfcnPresent : 1;
      unsigned cpLengthPresent : 1;
      unsigned prsInfoPresent : 1;
      unsigned antennaPortConfigPresent : 1;
      unsigned slotNumberOffsetPresent : 1;
      unsigned prs_SubframeOffsetPresent : 1;
   } m;
   uint16 physCellId;
   sm_ECGI cellGlobalId;
   lte_earfcn_t earfcn; /* (0..262143) */
   uint32 cpLength;
   sm_PRS_Info prsInfo;
   uint32 antennaPortConfig;
   uint8 slotNumberOffset;
   uint16 prs_SubframeOffset;
   uint16 expectedRSTD;
   uint16 expectedRSTD_Uncertainty;
} sm_OTDOA_NeighbourCellInfoElement;


/**************************************************************/
/*                                                            */
/*  sm_OTDOA_NeighbourFreqInfo                                   */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_NeighbourFreqInfo {
   uint32 n;
   sm_OTDOA_NeighbourCellInfoElement neighbor_cell_elem[SM_LOC_LTE_OTDOA_MAX_NGBR_CELLS];
} sm_OTDOA_NeighbourFreqInfo;


/**************************************************************/
/*                                                            */
/*  sm_OTDOA_NeighbourCellInfoList                               */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_NeighbourCellInfoList {
   uint32 n;
   sm_OTDOA_NeighbourFreqInfo neighbor_freq_elem[SM_OTDOA_NEIGHBOUR_FREQ_INFO_ELEMENTS];
} sm_OTDOA_NeighbourCellInfoList;

/**************************************************************/
/*                                                            */
/*  sm_OTDOA_ReferenceCellInfo                                   */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_ReferenceCellInfo {
   struct {
      unsigned cellGlobalIdPresent : 1;
      unsigned earfcnRefPresent : 1;
      unsigned antennaPortConfigPresent : 1;
      unsigned prsInfoPresent : 1;
   } m;
   uint16 physCellId;
   sm_ECGI cellGlobalId;
   lte_earfcn_t earfcnRef;          /* (0..262143) */
   uint32 antennaPortConfig;
   uint32 cpLength;
   sm_PRS_Info prsInfo;
} sm_OTDOA_ReferenceCellInfo;

/* Assistance Data provided by the network */

typedef struct sm_OTDOA_AssistanceData {
   struct {
      unsigned otdoa_ReferenceCellInfoPresent : 1;
      unsigned otdoa_NeighbourCellInfoPresent : 1;
   } m;
   sm_OTDOA_ReferenceCellInfo otdoa_ReferenceCellInfo;
   sm_OTDOA_NeighbourCellInfoList otdoa_NeighbourCellInfo;
} sm_OTDOA_AssistanceData;


/**************************************************************/
/*                                                            */
/*  OTDOA_MeasQuality                                         */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_MeasQuality {
   struct {
      unsigned error_NumSamplesPresent : 1;
   } m;
   uint32 error_Resolution;
   uint32 error_Value;
   uint32 error_NumSamples;
} sm_OTDOA_MeasQuality;


/**************************************************************/
/*                                                            */
/*  NeighbourMeasurementElement                               */
/*                                                            */
/**************************************************************/

typedef struct sm_NeighbourMeasurementElement {
   struct {
      unsigned cellGlobalIdNeighbourPresent : 1;
      unsigned earfcnNeighbourPresent : 1;
   } m;
   uint16 physCellIdNeighbor;
   sm_ECGI cellGlobalIdNeighbour;
   lte_earfcn_t earfcnNeighbour;
   uint16 rstd;
   sm_OTDOA_MeasQuality rstd_Quality;
} sm_NeighbourMeasurementElement;


/**************************************************************/
/*                                                            */
/*  NeighbourMeasurementList                                  */
/*                                                            */
/**************************************************************/

typedef struct sm_NeighbourMeasurementList {
   uint32 n;
   sm_NeighbourMeasurementElement elem[SM_LOC_LTE_OTDOA_MAX_NGBR_CELLS];
} sm_NeighbourMeasurementList;

/**************************************************************/
/*                                                            */
/*  OTDOA_SignalMeasurementInformation                        */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_SignalMeasurementInformation {
   struct {
      unsigned cellGlobalIdRefPresent : 1;
      unsigned earfcnRefPresent : 1;
      unsigned referenceQualityPresent : 1;
   } m;
   uint32 systemFrameNumber;
   uint16 physCellIdRef;
   sm_ECGI cellGlobalIdRef;
   lte_earfcn_t earfcnRef;          /* (0..262143) */
   sm_OTDOA_MeasQuality referenceQuality;
   sm_NeighbourMeasurementList neighbourMeasurementList;
} sm_OTDOA_SignalMeasurementInformation;

/* measurement information provided to the network */


/**************************************************************/
/*                                                            */
/*  OTDOA_TargetDeviceErrorCauses_cause                       */
/*                                                            */
/**************************************************************/

typedef enum {
   SM_OTDOA_cause_undefined = 0,
   SM_OTDOA_cause_assistance_data_missing = 1,
   SM_OTDOA_cause_unableToMeasureReferenceCell = 2,
   SM_OTDOA_cause_unableToMeasureAnyNeighbourCell = 3,
   SM_OTDOA_cause_attemptedButUnableToMeasureSomeNeighbourCells = 4
} sm_OTDOA_Error;

/**************************************************************/
/*                                                            */
/*  OTDOA_ProvideLocationInformation                          */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_LocationInformation {
   struct {
      unsigned otdoaSignalMeasurementInformationPresent : 1;
      unsigned otdoa_ErrorPresent : 1;
   } m;
   sm_OTDOA_SignalMeasurementInformation otdoaSignalMeasurementInformation;
   sm_OTDOA_Error otdoa_Error;
} sm_OTDOA_ProvideLocationInformation;

#if 0 /* for future use if needed */
/**************************************************************/
/*                                                            */
/*  CellGlobalIdGERAN                                         */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_CellGlobalIdGERAN {
   sm_ECGI cellIdentityGERANi;
   uint32 locationAreaCode;
} sm_FTA_CellGlobalIdGERAN;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_gSM                                    */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_NetworkTime_cellID_gSM {
   struct {
      unsigned cellGlobalIdGERANPresent : 1;
   } m;
   uint16 bcchCarrier;
   uint8 bsic;
   sm_FTA_CellGlobalIdGERAN cellGlobalIdGERAN;
} sm_FTA_NetworkTime_cellID_gSM;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_uTRA_mode_fdd                          */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_NetworkTime_cellID_uTRA_mode_fdd {
   uint16 primary_CPICH_Info;
} sm_FTA_NetworkTime_cellID_uTRA_mode_fdd;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_uTRA_mode_tdd                          */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_NetworkTime_cellID_uTRA_mode_tdd {
   uint8 cellParameters;
} sm_FTA_NetworkTime_cellID_uTRA_mode_tdd;


/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_uTRA_mode                              */
/*                                                            */
/**************************************************************/

/* Choice tag constants */

#define SM_FTA_NETWORKTIME_CELL_ID_UTRA_MODE_FDD 1
#define SM_FTA_NETWORKTIME_CELL_ID_UTRA_MODE_TDD 2

typedef struct NetworkTime_cellID_uTRA_mode {
   int t;
   union {
      /* t = 1 */
      sm_FTA_NetworkTime_cellID_uTRA_mode_fdd fdd;
      /* t = 2 */
      sm_FTA_NetworkTime_cellID_uTRA_mode_tdd tdd;
   } u;
} sm_FTA_NetworkTime_cellID_uTRA_mode;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_uTRA                                   */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_NetworkTime_cellID_uTRA {
   struct {
      unsigned cellGlobalIdUTRAPresent : 1;
   } m;
   sm_FTA_NetworkTime_cellID_uTRA_mode mode;
   sm_ECGI cellGlobalIdUTRA;
   uint16 uarfcn;
} sm_FTA_NetworkTime_cellID_uTRA;
#endif 

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_eUTRA                                  */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_NetworkTime_cellID_eUTRA {
   struct {
      unsigned cellGlobalIdEUTRAPresent : 1;
   } m;
   uint16 physCellId;
   sm_ECGI cellGlobalIdEUTRA;
   lte_earfcn_t earfcn; /* (0..262143) */
} sm_FTA_NetworkTime_cellID_eUTRA;

#define SM_FTA_NETWORKTIME_CELLID_EUTRA      1
#if 0
#define SM_FTA_NETWORKTIME_CELLID_UTRA       2
#define SM_FTA_NETWORKTIME_CELLID_GSM        3
#endif
typedef struct sm_NetworkTime_cellID {
   int t;
   union {
      /* t = 1 */
      sm_FTA_NetworkTime_cellID_eUTRA eUTRA;
      /* t = 2 */
      #if 0 /* Only eUTRA for now */
      sm_FTA_NetworkTime_cellID_uTRA uTRA;
      /* t = 3 */
      sm_FTA_NetworkTime_cellID_gSM gSM;
      #endif
   } u;
} sm_FTA_NetworkTime_cellID;
/**************************************************************/
/*                                                            */
/*  NetworkTime                                               */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_NetworkTime {
   struct {
      unsigned frameDriftPresent : 1;
   } m;
   uint16 secondsFromFrameStructureStart;
   uint32 fractionalSecondsFromFrameStructureStart;
   uint8 frameDrift;
   sm_FTA_NetworkTime_cellID cellID;
} sm_FTA_NetworkTime;

/**************************************************************/
/*                                                            */
/*  GNSS_ReferenceTimeForOneCell_bsAlign                      */
/*                                                            */
/**************************************************************/

typedef enum {
   sm_FTA_ReferenceTimeForOneCell_bsAlign_true_ = 0
} sm_FTA_ReferenceTimeForOneCell_bsAlign_Root;

/**************************************************************/
/*                                                            */
/*  GNSS_ReferenceTimeForOneCell                              */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_ReferenceTimeForOneCell {
   struct {
      unsigned bsAlignPresent : 1;
   } m;
   sm_FTA_NetworkTime networkTime;
   uint8 referenceTimeUnc;
   uint32 bsAlign;
} sm_FTA_ReferenceTimeForOneCell;


/**************************************************************/
/*                                                            */
/*  GNSS_ReferenceTime_gnss_ReferenceTimeForCells             */
/*                                                            */
/**************************************************************/

typedef struct sm_FTA_FTA_Information {
   uint32 n;
   sm_FTA_ReferenceTimeForOneCell elem[SM_LOC_LTE_FTA_MAX_CELLS];
} sm_FTA_ReferenceTimeForCells;

typedef struct sm_OTDOA_FTA_Information
{
  uint8 referenceTimeUnc;
  sm_FTA_ReferenceTimeForCells fta_ReferenceTimeForCells;
}sm_OTDOA_FTA_Information;


typedef enum
{
  SM_LOC_LTE_OTDOA_MSG_ID_FIRST = C_USR_MSG_ID_FIRST,
  SM_LOC_LTE_OTDOA_MSG_ID_START_CMD,
  SM_LOC_LTE_OTDOA_MSG_ID_STOP_CMD,
  SM_LOC_LTE_OTDOA_MSG_ID_GET_MEAS_CMD,
} sm_loc_lte_otdoa_msg_id_type;


typedef struct
{
  sm_OTDOA_AssistanceData *p_otdoa_ad;
  uint32 q_op_mode;
  uint32 u_sess_type;
} sm_loc_lte_otdoa_start_cmd_type;


typedef struct
{
  gnss_ClockStructType *p_clock;
  uint32 u_sess_type;
} sm_loc_lte_otdoa_get_measurements_type;


typedef struct
{
  uint8 u_reason;
  uint32 u_sess_type;
} sm_loc_lte_otdoa_stop_cmd_type;


typedef struct
{
  uint8 u_SeqNum;         /* Measurement report sequence number starting from 1 */
  uint8 u_MaxMessageNum;  /* Max measurement report expected where u_MaxMessageNum >= u_SeqNum always */
  sm_OTDOA_ProvideLocationInformation  *p_meas_info;
  uint32 u_sess_type;
  boolean earlyMeasurement;
} sm_loc_lte_otdoa_meas_info_type;

/*===========================================================================
  sm_loc_lte_otoda_start_cmd

  Description:

     This function is used by SM to send a start to LOC_LTE_OTDOA module. This
     will send an IPC to the LOC_LTE_OTODA module

  Parameters: 
    sm_OTDOA_AssistanceData - pointer to the AD provided by the network
    timeout - value given by the network

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_start_cmd(const sm_OTDOA_AssistanceData *p_otdoa_ad, uint32 q_operation_mode, 
                                   uint32 u_sess_type);



/*===========================================================================
  sm_loc_lte_otoda_get_measurements

  Description:

     This function is used by SM to request OTDOA measurements from LOC_LTE_OTDOA module.
     This GNSS time stamps is sent if the OTDOA measurements are to be time aligned,
     NULL if no time alignment is needed and the current measurements are to be returned.

  Parameters: 
    gnss_TimeStructType *p_gnss_clock - gnss time if time alignment needed, else NULL

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_get_measurements(gnss_ClockStructType *p_gnss_clock, uint32 u_sess_type,
                                          boolean earlyMeasurement);



/*===========================================================================
  sm_pl_lte_otoda_stop_cmd

  Description:

     This function is used by SM to STOP LOC_LTE_OTDOA module

  Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_stop_cmd(uint8 u_reason, uint32 u_sess_type);


/*===========================================================================
  sm_loc_lte_otoda_sm_recv_measurements

  Description:

     This function is used by LOC_LTE_OTDOA module to send the OTDOA measurements

  Parameters:
    measurements information

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_recv_measurements(sm_loc_lte_otdoa_meas_info_type *p_otdoa_measurement_info);


/*===========================================================================
  sm_loc_lte_otoda_update_timeout

  Description:

     This function is used by LOC_LTE_OTDOA module to send the OTDOA measurements

  Parameters:
    measurements information

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_update_timeout(uint32 q_timeout,  uint32 u_sess_type );


/*===========================================================================
  sm_loc_lte_fta_assistance_data

  Description:

     This function is used by LOC_LTE_OTDOA module to send the OTDOA measurements

  Parameters:
    measurements information

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_fta_assitance_data(sm_OTDOA_FTA_Information *p_fta_ad,  uint32 u_sess_type );

/*===========================================================================
  sm_loc_lte_ota_reset

  Description:

     This function is used to inform LOC_LTE_OTDOA module of OTA reset msg

  Parameters:
    None

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_ota_reset(void);

#endif
