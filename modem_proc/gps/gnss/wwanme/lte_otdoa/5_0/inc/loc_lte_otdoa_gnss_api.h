#ifndef LOC_LTE_OTDOA_GNSS_API_H
#define LOC_LTE_OTDOA_GNSS_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Location LTE OTDOA - GNSS API Header File

GENERAL DESCRIPTION
This file contains API definitions between Location LTE OTDOA module and
other GNSS modules.

===========================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Qualcomm Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/lte_otdoa/5_0/inc/loc_lte_otdoa_gnss_api.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "comdef.h"
#include "sm_loc_lte_otdoa.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
/* Different session types. Enum value defined by SM layer */
#define LOC_LTE_OTDOA_UP_SESSION 8 /* user plane */
#define LOC_LTE_OTDOA_CP_SESSION 0x200  /* control plane */

/* Different operation modes. Enum value defined by SM layer */
#define LOC_LTE_OTDOA_OP_MODE_OTDOA_GPS_HYBRID 2 /* hybrid mode */
#define LOC_LTE_OTDOA_OP_MODE_OTDOA_ONLY_UP 7        /* OTDOA only */
#define LOC_LTE_OTDOA_OP_MODE_OTDOA_ONLY_CP 16   /* OTDOA only for CP */

typedef struct
{
  uint8 u_Reason;
  uint16 e_SessType;
}
loc_lte_otdoa_StopSessCmdType;

typedef struct
{
  uint8 u_Reserved;
}
loc_lte_otdoa_OtaResetCmdType;

typedef struct
{
  gnss_ClockStructType *p_Clock;
  uint16 e_SessType;
  boolean b_IsEarlyRptReq;
}
loc_lte_otdoa_ReqMeasRptCmdType;


/* Testmode where we control LTE OTDOA operation */
#define LOC_LTE_OTDOA_DIAG_INJECT_ASSIST_DATA_SUBCMD      1
#define LOC_LTE_OTDOA_DIAG_START_SESSION_SUBCMD           2

/* DIAG commands to mimic cmds sent by SM */
#define LOC_LTE_OTDOA_DIAG_SM_TEST_SUBCMD                 6

/* Sub commands for enabling/disabling PRS logger mode (cont. search
 * without NC */
#define LOC_LTE_OTDOA_DIAG_ENABLE_PRS_LOGGER_SUBCMD       7
#define LOC_LTE_OTDOA_DIAG_DISABLE_PRS_LOGGER_SUBCMD      8

/* Sub commands for enabling/disabling PRS scanning. In this mode,
 * all 504 possible physical cell IDs belonging to EARFCN of
 * serving cell will be searched */
#define LOC_LTE_OTDOA_DIAG_ENABLE_PRS_SCAN_SUBCMD          9
#define LOC_LTE_OTDOA_DIAG_DISABLE_PRS_SCAN_SUBCMD         10

/* Sub command to override PRS paramters */
#define LOC_LTE_OTDOA_DIAG_OVERRIDE_PRS_PARAMS_SUBCMD      11

/* Sub command to enable NC Integration */
#define LOC_LTE_OTDOA_DIAG_ENABLE_NC_INT_SUBCMD      12
#define LOC_LTE_OTDOA_DIAG_DISABLE_NC_INT_SUBCMD      13

/* Sub commands to force aiding ref cell to be ref cell in RSTD report.
 * Similar to OTA reset command */
#define LOC_LTE_OTDOA_DIAG_FORCE_AIDING_REF_CELL      14
#define LOC_LTE_OTDOA_DIAG_DISABLE_AIDING_REF_CELL     15

/* Sub command to modify QOS timers value */
#define LOC_LTE_OTDOA_DIAG_MODIFY_QOS_TIMER       16

/* Reserve 17, 18 for OPCRS */

/* Sub command to enable/disable computing offending cells after finishing disc phase */
#define LOC_LTE_OTDOA_DIAG_ENABLE_OFFENDING_CELLS_COMPUTE   19
#define LOC_LTE_OTDOA_DIAG_DISABLE_OFFENDING_CELLS_COMPUTE  20

/* Sub command to enable/disable computing PRS Gain Bias for cells after finishing disc phase */
#define LOC_LTE_OTDOA_DIAG_ENABLE_PRS_GAIN_BIAS_COMPUTE     21
#define LOC_LTE_OTDOA_DIAG_DISABLE_PRS_GAIN_BIAS_COMPUTE    22

/* Sub command to adjust the FA threshold */
#define LOC_LTE_OTDOA_DIAG_ADJUST_FA_THRESHOLD     23

/* Sub command to adjust the num of hyps per batch, num of hyps per occ */
#define LOC_LTE_OTDOA_DIAG_HYPS_PER_BATCH          24
#define LOC_LTE_OTDOA_DIAG_HYPS_PER_OCC            25

/* Sub command to enable/disable computing offending cells after finishing disc phase */
#define LOC_LTE_OTDOA_DIAG_ENABLE_PRSIC_OFFENDING_COMPUTE   26
#define LOC_LTE_OTDOA_DIAG_DISABLE_PRSIC_OFFENDING_COMPUTE  27

/* Sub command to enable/disable computing of RSTD for IF cells based on best ref cell TOA */
#define LOC_LTE_OTDOA_DIAG_ENABLE_VAR_REF_TOA_FOR_IF_NGBR   28
#define LOC_LTE_OTDOA_DIAG_DISABLE_VAR_REF_TOA_FOR_IF_NGBR  29

/* */
#define LOC_LTE_OTDOA_MAX_DIAG_MSG   4000
#define LOC_LTE_OTDOA_MAX_DIAG_MSG_DATA_LENGTH   2000

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

typedef PACKED struct PACKED_POST
{
  /* Command code */
  uint8   u_CmdCode;

  /* Subcommand code (overload of u_SubsysId) */
  uint8   u_GpsControl;

  /* Length of data payload (overload of w_SubsysCmdCode) */
  uint16  w_DataLength;

  /* CGPS Command Code (overload of u_CgpsCmdCode) */
  uint8   u_GpsSubcmd;

  /* Request Message Version Number */
  uint8   u_Version;
  int32   l_ClientId;

  /* Complete data set length that needs to be transfered in all packets
   * (irrelevant if q_TotalPacketsNumber == 1) */
  uint32  q_TotalDataLength;

  /* Current packet number (irrelevant if q_TotalPacketsNumber == 1);
   * first packet number == 0 */
  uint32  q_PacketNumber;

  /* Total numer of packtes needed to transfer the complete data set */
  uint32  q_TotalNumberOfPackets;

  /* Data length of the u_PacketData field */
  uint32  q_PacketDataLength;

  uint32  q_PacketData[ LOC_LTE_OTDOA_MAX_DIAG_MSG_DATA_LENGTH / 4 ];
}
loc_lte_otdoa_DiagCmdType;

typedef PACKED struct PACKED_POST
{
  /* Command code */
  uint8   u_CmdCode;

  /* Subcommand code (overload of u_SubsysId) */
  uint8   u_GpsControl;

  /* Length of data payload (overload of w_SubsysCmdCode) */
  uint16  w_DataLength;

  /* CGPS Command Code (overload of u_CgpsCmdCode) */
  uint8   u_GpsSubcmd;

  /* Command that has to be injected */
  uint8   u_InjectCmdCode;

  /* Boolean to indicate to use Internal Data or not */
  uint8   b_ExternalData;

  uint8   q_Data[LOC_LTE_OTDOA_MAX_DIAG_MSG_DATA_LENGTH / 4 ];
}
loc_lte_otdoa_DiagCmdSmTestType;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

typedef struct
{
  sm_OTDOA_AssistanceData *p_OtdoaAssistanceData;
  uint8 e_OpMode;
  uint16 e_SessType;
}
loc_lte_otdoa_SmStartCmdInfoType;

typedef struct
{
  /* Timeout in milli seconds */
  uint32 q_Timeout;
  uint16 e_SessType;
}
loc_lte_otdoa_UpdateTimoutInfoType;

typedef struct
{
  sm_OTDOA_FTA_Information *p_FtaAidingData;
  uint16 e_SessType;
}
loc_lte_otdoa_FtaAidingDataInfoType;

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_StartSessCmd
 *
 * Description:
 *  This function will start the OTDOA meas. process. It will be sent by SM
 *
 * Parameters:
 *  Assitance data, operation mode and session type.
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_StartSessCmd
(
  loc_lte_otdoa_SmStartCmdInfoType *p_SmStartCmdInfo
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_StopSessCmd
 *
 * Description:
 *  This function will stop the OTDOA meas. process. It will be sent by SM
 *
 * Parameters:
 *  Reason for stop command.
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_StopSessCmd
(
  loc_lte_otdoa_StopSessCmdType *p_StopSessCmd
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_ReqMeasRptCmd
 *
 * Description:
 *  This function is used by SM to request OTDOA measurements from GPS SW
 *
 * Parameters:
 *  Meas. type
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_ReqMeasRptCmd
(
  loc_lte_otdoa_ReqMeasRptCmdType * p_ReqMeasRptCmd
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_ProcDiagCmd
 *
 * Description:
 *  This function is used by DIAG task to pass received Loc LTE OTDOA packet
 *
 * Parameters:
 *  DIAG packet
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_ProcDiagCmd
(
  uint8 * p_DiagPkt, uint16 w_PktLength
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_UpdateTimeout
 *
 * Description:
 *  This function is used by SM to send the timeout info
 *
 * Parameters:
 *  Timeout value and session type
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_UpdateTimeout
(
  loc_lte_otdoa_UpdateTimoutInfoType *p_UpdateTimoutInfo
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_UpdateFtaAidingData
 *
 * Description:
 *  This function is used by SM to send the fine time assistance info
 *
 * Parameters:
 *  Fine time assitance and session type
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_UpdateFtaAidingData
(
  loc_lte_otdoa_FtaAidingDataInfoType *p_FtaAidingDataInfo
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_OtaReset
 *
 * Description:
 *  This function is used by SM to send the OTA reset to LOC_LTE_OTDOA
 *
 * Parameters:
 *  None
 *
 * Dependencies:
 *  None.
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_OtaReset
(
   loc_lte_otdoa_OtaResetCmdType *p_OtaResetInfo
);

#endif /* LOC_LTE_OTDOA_GNSS_API_H */
