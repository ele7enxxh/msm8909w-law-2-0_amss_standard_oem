#ifndef WWANME_COMMON_DEFS_H
#define WWANME_COMMON_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Location LTE OTDOA Common Definitions Header File

GENERAL DESCRIPTION
This file contains common definitions shared between all Location LTE OTDOA
module files.

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/common/inc/wwanme_common_defs.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "gps_types.h"
#include "custcgps.h"

#include "msg.h"
#include <math.h>
#include "queue.h"
#include "log.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"


/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

#define WME_MSG_LOW(a,b,c,d)            MSG_3(MSG_SSID_MGPME, MSG_MASK_5, a,b,c,d)
#define WME_MSG_MED(a,b,c,d)            MSG_3(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d)
#define WME_MSG_MED_4(a,b,c,d,e)        MSG_4(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e)
#define WME_MSG_MED_5(a,b,c,d,e,f)      MSG_5(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f)
#define WME_MSG_MED_6(a,b,c,d,e,f,g)    MSG_6(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f,g)
#define WME_MSG_MED_7(a,b,c,d,e,f,g,h)    MSG_7(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f,g,h)
#define WME_MSG_MED_8(a,b,c,d,e,f,g,h,i)    MSG_8(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f,g,h,i)
#define WME_MSG_HIGH(a,b,c,d)           MSG_3(MSG_SSID_MGPME, MSG_MASK_7, a,b,c,d)
#define WME_MSG_ERROR(a,b,c,d)          MSG_3(MSG_SSID_MGPME, MSG_LEGACY_ERROR, a,b,c,d)
#define WME_MSG_FATAL(a,b,c,d)          MSG_3(MSG_SSID_MGPME, MSG_LEGACY_FATAL, a,b,c,d)

#define WME_MSG_SPRINTF_1(a,b);         MSG_SPRINTF_1( MSG_SSID_MGPME, MSG_LEGACY_HIGH, a, b);

/*--------------------------------------------------------------------------
 * LTE Related Constants
 *-----------------------------------------------------------------------*/

#define LLO_MSG_LOW(a,b,c,d)            MSG_3(MSG_SSID_MGPME, MSG_MASK_5, a,b,c,d)
#define LLO_MSG_MED(a,b,c,d)            MSG_3(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d)
#define LLO_MSG_MED_4(a,b,c,d,e)        MSG_4(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e)
#define LLO_MSG_MED_5(a,b,c,d,e,f)      MSG_5(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f)
#define LLO_MSG_MED_6(a,b,c,d,e,f,g)    MSG_6(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f,g)
#define LLO_MSG_MED_7(a,b,c,d,e,f,g,h)    MSG_7(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f,g,h)
#define LLO_MSG_MED_8(a,b,c,d,e,f,g,h,i)    MSG_8(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f,g,h,i)
#define LLO_MSG_HIGH(a,b,c,d)           MSG_3(MSG_SSID_MGPME, MSG_MASK_7, a,b,c,d)
#define LLO_MSG_ERROR(a,b,c,d)          MSG_3(MSG_SSID_MGPME, MSG_LEGACY_ERROR, a,b,c,d)
#define LLO_MSG_FATAL(a,b,c,d)          MSG_3(MSG_SSID_MGPME, MSG_LEGACY_FATAL, a,b,c,d)
#define LLO_COMPARE_CELLS(a,b,c,d)      ((a == b) && (c == d))

#define LLO_MSG_SPRINTF_1(a,b);         MSG_SPRINTF_1( MSG_SSID_MGPME, MSG_LEGACY_HIGH, a, b);

/* 1/(15000*2048) secs */
#define LLOC_TS_UNIT       3.25520833e-8

#define LLOC_US_PER_TS      0.0325520833
#define LLOC_US_PER_3TS     0.09765625
#define LLOC_US_TO_MS       0.001
#define LLOC_MS_PER_TS      LLOC_US_PER_TS*LLOC_US_TO_MS

/* Defines for inflation rate calculation
 * 1Ts/sec = 9.7588 m/s which is close to 9 m/s rate in AFLT.
 * Ts = 1/(2048 * 15000) = 1/(30720000) */
#define LLOC_INFLATION_RATE_TS_PER_SEC        1.0
#define LLOC_TS_PER_SEC                       30720000
#define LLOC_TS_PER_USEC                      30.72

/* Mts/Ts = 299792458 / (2048 * 150000) */
#define LLOC_MTS_PER_TS                       9.7588690

#define MIN_PHYSICAL_ID                         0
#define MAX_PHYSICAL_ID                         503
#define UNKNOWN_PHYSICAL_ID                     0xFFFF
#define PHYSICAL_ID_OK(x)   (  ( (x) >= ((MIN_PHYSICAL_ID)) ) && ( x <= MAX_PHYSICAL_ID ) )

#define MIN_EARFCN                              0
#define MAX_EARFCN                              262143
#define UNKNOWN_EARFCN                          0xFFFFFFFF
#define EARFCN_OK(x)        (  ( (x) >= ((MIN_EARFCN)) ) && ( x <= MAX_EARFCN ) )

#define MAX_RSRP                                 97
#define RSRP_OK(x)                               ( (x) <= MAX_RSRP )

#define MAX_RSRQ                                 34
#define RSRQ_OK(x)                               ( (x) <= MAX_RSRQ )

#define MAX_RXTX_TIMEDIFF                        4095
#define RXTX_TIMEDIFF_OK(x)                      ( (x) <= MAX_RXTX_TIMEDIFF )

/*--------------------------------------------------------------------------
 * General Constants Definitions
 *-----------------------------------------------------------------------*/

#define MSEC_TO_SEC                             0.001f
#define NSEC_TO_MSEC                            1e-6f
#define MSEC_TO_NSEC                            1e6

/*--------------------------------------------------------------------------
 * Timer Id definitions
 *-----------------------------------------------------------------------*/
/* LTE OTDOA Timers */
#define LTE_OTDOA_CONTROL_TIMER_ID_SM_RPT      1   /* 00001 */
#define LTE_OTDOA_CONTROL_TIMER_ID_ML1_SANITY  2   /* 00010 */
#define LTE_OTDOA_CONTROL_TIMER_ID_MP_SANITY   4   /* 00100 */
#define LTE_OTDOA_CONTROL_TIMER_ID_SM_SANITY   8   /* 01000 */
#define LTE_OTDOA_CONTROL_ALL_TIMERS           31  /* 11111 */

/* Total timers in OTDOA task */
#define LTE_OTDOA_CONTROL_TIMER_CNT             5


/* WWAN ME Config NV Bit definitions */
/* Disable LTE Time transfer when this bit is set*/
#define WWANME_CONFIG_DISABLE_LTE_TIME_TRANSFER     (1 << 0)
/* NV 73547 bit 1 to indicate if meas in connected mode will be time stamped or not.
Default behavior will be only meas in idle mode will be stored and time stamped. */
#define WWANME_CONFIG_ACCEPT_CONNECTED_MODE_TDP_MEAS     (1 << 1)
#define WWANME_CONFIG_SET_CONNECTED_VSRC_THRESHOLD       (1 << 2)
/* OTDOA Bits */
#define WWANME_CONFIG_OTDOA_ENABLE_PRSIC           (1 << 3)


/*--------------------------------------------------------------------------
 * WWAN ME Meas Proc Task Message Definitions
 *-----------------------------------------------------------------------*/
typedef enum
{

  /* IPC 0... C_USR_MSG_ID_FIRST-1 is reserved by OS */
  C_LOC_LTE_OTDOA_MEASPROC_MSG_FIRST = C_USR_MSG_ID_FIRST,  
  
  /* ----------------------LTE OTDOA Messages---------------------- */
  C_LOC_LTE_OTDOA_MEASPROC_START_MEAS_PROCESSING,     /* 11 */
  C_LOC_LTE_OTDOA_MEASPROC_MEAS_RESULTS_AVAIL,        /* 12 */
  C_LOC_LTE_OTDOA_MEASPROC_STOP_MEAS_PROCESSING,      /* 13 */
  C_LOC_LTE_OTDOA_MEASPROC_FLUSH_IPC_QUEUE,           /* 14 */
  C_LOC_LTE_OTDOA_MEASPROC_RESET_NC_DATABASE,         /* 15 */
  C_LOC_LTE_OTDOA_MEASPROC_ADJUST_FA_THRESHOLD,       /* 16 */
  C_LOC_LTE_OTDOA_MEASPROC_MSG_LAST,
  C_LOC_LTE_OTDOA_MEASPROC_MSG_MAX = 99,

  /* ----------------------LTE TDP Messages---------------------- */
  C_LLOMP_TDP_MSG_FIRST = C_LOC_LTE_OTDOA_MEASPROC_MSG_MAX,  
  C_LLOMP_TDP_COMPUTE_EAP,   /* 100 : msg sent by OC to OMP task to compute Eap */
  C_LLOMP_TDP_MSG_LAST,                               /* 101 */
  C_LLOMP_TDP_MSG_MAX = 200
} wwanme_measproc_IpcMsgEnumType;


/*--------------------------------------------------------------------------
 * WWAN ME Control Task Message Definitions
 *-----------------------------------------------------------------------*/
typedef enum
{
  /* IPC 0... C_USR_MSG_ID_FIRST-1 is reserved by OS */
  C_LOC_LTE_OTDOA_CONTROL_MSG_FIRST = C_USR_MSG_ID_FIRST,  

  /* ----------------------LTE OTDOA Messages---------------------- */
  /* OTDOA-ML1 interface */
  C_LOC_LTE_OTDOA_CONTROL_ML1_MEAS_ERROR, /* 11 */
  C_LOC_LTE_OTDOA_CONTROL_ML1_MEAS_RESULTS_AVAIL,   /* 12 */
  C_LOC_LTE_OTDOA_CONTROL_ML1_UPDATE_SERVING_CELL,  /* 13 */

  /* OTDOA-SM interface */
  C_LOC_LTE_OTDOA_CONTROL_SM_START_SESS_CMD,        /* 14 */
  C_LOC_LTE_OTDOA_CONTROL_SM_STOP_SESS_CMD,         /* 15 */
  C_LOC_LTE_OTDOA_CONTROL_SM_REQ_MEAS_RPT_CMD,      /* 16 */
  C_LOC_LTE_OTDOA_CONTROL_SM_UPDATE_TIMEOUT,        /* 17 */
  C_LOC_LTE_OTDOA_CONTROL_SM_UPDATE_FTA_DATA,       /* 18 */
  C_LOC_LTE_OTDOA_CONTROL_SM_OTA_RESET_CMD,         /* 19 */

  /* OTDOA-DIAG interface */
  C_LOC_LTE_OTDOA_CONTROL_DIAG_PKT_RECEIVED,        /* 20 */

  /* MeasProc-Ctrl task interface */
  C_LOC_LTE_OTDOA_CONTROL_MP_MEAS_PROC_COMPLETE,   /* 21 */
  C_LOC_LTE_OTDOA_CONTROL_MP_HYPOTHESES_COMPLETE,  /* 22 */
  C_LOC_LTE_OTDOA_CONTROL_MP_STOP_CMD_ACK,         /* 23 */
  C_LOC_LTE_OTDOA_CONTROL_MSG_LAST,                /* 24 */

  C_LOC_LTE_OTDOA_CONTROL_MSG_MAX = 99, 

  /* ----------------------LTE TDP Messages---------------------- */
  C_LOC_LTE_TDP_CONTROL_MSG_FIRST = C_LOC_LTE_OTDOA_CONTROL_MSG_MAX,
  /* ML1-OC interface */
  C_LLOC_TDP_ML1_OPCRS_MEAS_RESULTS_AVAIL,   //100

  /* TLM->OC interface */
  C_LLOC_TDP_UPLOAD_ENABLE,   /* 101: TDP Upload enable sent by TLM task */
  C_LLOC_TDP_UPLOAD_DISABLE,  /* 102: TDP Upload disable sent by TLM task */         
  C_LLOC_TDP_MEASBLK_REQ,     /* 103: TDP meas blk request from TLM task */

  /* OMP -> OC interface */
  C_LLOC_TDP_EAP_CALC_COMPLETE_0, /* 104: Obsolete -TDP Meas response from OMP task */

  /* ME -> OC interface */
  C_LOC_LTE_TDP_GPSFIX_TIME_UPDATE, /* 105: Time Update message from MC Task */
  C_LOC_LTE_TDP_UPLOAD_CONFIG, /* 106: TDP Upload Config from TLM task */
  C_LLOC_TDP_EAP_CALC_COMPLETE, /* 107: TDP Meas response from OMP task */

  C_LOC_LTE_TDP_CONTROL_MSG_LAST, /* 108 */
  C_LLOC_TDP_MSG_MAX = 200,

  C_LOC_GTS_MSG_FIRST = C_LLOC_TDP_MSG_MAX,
  C_LOC_GTS_MSG_FREQ_ERR_EST,
  C_LOC_GTS_MSG_LTE_TIME,
  C_LOC_GTS_MSG_GPS_POS_REP,
  C_LOC_GTS_MSG_GPS_TIME_REP,
  C_LOC_GTS_MSG_REGISTER,
  C_LOC_GTS_MSG_GPS_TIME_REQ,
  C_LOC_GTS_MSG_HIGH_ACCU_MODE,
  C_LOC_GTS_MSG_SENSOR_REP,
  C_LOC_GTS_MSG_FEST_HAT_STATUS,
  C_LOC_GTS_MSG_DIAG_CMDS,
  C_LOC_GTS_MSG_GSM_TIME,
  C_LOC_GTS_MSG_WCDMA_TIME,
  C_LOC_GTS_MSG_TDSCDMA_TIME,
  C_LOC_GTS_MSG_LAST,
  C_LOC_GTS_MSG_MAX = 300
}
wwanme_control_IpcMsgEnumType;

#define IS_OS_CONTROL_MSG(x)  ( ( (x) >= C_OS_MSG_ID_FIRST) && \
                          ( (x) < C_USR_MSG_ID_FIRST) ) 


#define IS_LTE_OTDOA_CONTROL_MSG(x)  ( ( (x) > C_LOC_LTE_OTDOA_CONTROL_MSG_FIRST) && \
                          ( (x) < C_LOC_LTE_OTDOA_CONTROL_MSG_LAST) ) 


#define IS_LTE_TDP_CONTROL_MSG(x)  ( ( (x) > C_LOC_LTE_TDP_CONTROL_MSG_FIRST) && \
                           ( (x) < C_LOC_LTE_TDP_CONTROL_MSG_LAST) ) 

#define IS_GTS_MSG(x)  ( ( (x) > C_LOC_GTS_MSG_FIRST) && \
                           ( (x) < C_LOC_GTS_MSG_LAST) ) 


#define IS_LTE_OTDOA_MEASPROC_MSG(x)  ( ( (x) > C_LOC_LTE_OTDOA_MEASPROC_MSG_FIRST) && \
                          ( (x) < C_LOC_LTE_OTDOA_MEASPROC_MSG_LAST) ) 


#define IS_LTE_TDP_MEASPROC_MSG(x)  ( ( (x) > C_LLOMP_TDP_MSG_FIRST) && \
                           ( (x) < C_LLOMP_TDP_MSG_LAST) ) 


#endif /* WWANME_COMMON_DEFS_H */

