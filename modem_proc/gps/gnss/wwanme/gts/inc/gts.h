/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Global Time Services Module

GENERAL DESCRIPTION
This file is the main file of Global Time Service module

Copyright (c) 2014-2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/gts/inc/gts.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "msg.h"
#include "gts_loc_api.h"
#include "sys.h"
#include "gps_types.h"
#include "gts_task.h"
#include "wwanme_common_defs.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

#ifndef ABS_GTS
#define ABS_GTS(a)          (((a) < 0) ? (-(a)) : (a))
#endif

/* Time related paramaters */
#define GTS_GNSSRTC_MS_MASK        0x7fff
#define GTS_GNSSRTC_MS_ROLLOVER    0x8000
#define GTS_GNSSRTC_MS_SHFT         (17)

/* GPS CHIPS related definitions */
#define GTS_GCX1_PER_MSEC           1023
#define GTS_GCX32_PER_MSEC          (32*GTS_GCX1_PER_MSEC)
#define GTS_GCX80_PER_MSEC          (80*GTS_GCX1_PER_MSEC)

/*Define the bits and masks for GNSSRTC (Gen8)*/
#define GTS_GNSSRTC_CNT_CX80_MASK        (0x1FFFF)

/* 16:7  Chip count */
#define GTS_GNSSRTC_CHIP_CNT_MASK        (0x1FF80)
#define GTS_GNSSRTC_CHIP_CNT_SHIFT       (7)

/* 6:0  subChip x 80 sample count */
#define GTS_GNSSRTC_SAMPLE_CNT_MASK      (0x7F)

#define GTS_GNSSPHASE_PER_CX80           4294967296

/* 64 bit Dal Time - Lower 24 bits are Ustmr, so higher 40 are MSB */
#define GTS_USTMR_MSB_SHIFT               (24)

/* USTMR Constants for Bolt */
#define GTS_USTMR_MASK                   (0xFFFFFF)

/* USTMR Clock speed = 19.2 MHz */
#define GTS_USTMR_CLOCK_FREQ_HZ          19200000

/* Ms to USTMR conversion */
#define GTS_USTMR_PER_MS           19200

/* USTMR Rolls over every 0x8000000 counts */
#define GTS_USTMR_ROLLOVER_COUNT         16777216

/* USTMR Rollover midpoint */
#define GTS_USTMR_HALF_ROLLOVER_COUNT    8388608

/* USTMR Rolls over every 873 ms (16777216/19200)  */
#define GTS_USTMR_ROLLOVER_MS            873.813

/* Number of CDMA chips in 1 week.
 * 1.2288 Mcps * 3600 * 24 * 7 */
#define GTS_NUM_OF_SECS_PER_WEEK_CX1   743178240000

/* No of ms in a week 7*24*60*60*1000 */
#define GTS_NUM_OF_MSEC_IN_ONE_WEEK  604800000

/* OSTMR Clock Speed */
#define GTS_OSTMR_PER_MS 30720.0

/* OSTMR Phase Conversion factor */
#define GTS_OSTMR_PHASE_FACTOR  4294967296.0

#define GTS_1MS_IN_LTE_TS   GTS_OSTMR_PER_MS
#define GTS_11MS_IN_LTE_TS   337920 /* 11 * 30720 */
#define GTS_OSTMR_ROLLOVER  0x40000000

#define GTS_MAX_NO_OF_LATCHES 8
#define GTS_USTMR_PER_OSTMR   0.625   /* 19.2 Mhz/30.72 Mhz */

#define GTS_MAX_USTMR_PROPAGATION_MS    86400000 /* 24 hours in ms */

/* Define the Network clock drift to be 0.05ppm */
#define GTS_NETWORK_CLOCK_DRIFT 0.05e-6

/* Max Time between two LTE time - Just shy of OSTMR Rollover */
#define GTS_MAX_LTE_DIFF_MS    34000

/* Minimum Uncertainity for HAT mode to be enabled - 100 Us*/
#define GTS_MIN_UNC_FOR_HAT_EN_MS    0.1

/* Time uncertainity due to sleep clock latching for HAT */
#define GTS_TIME_UNC_PER_LATCH_MS    0.002

/* Uncertainity for HAT for Low to High XO Mode switches (per second) */
#define GTS_TIME_UNC_MODE_SWITCH_MS    0.001

/* 1 ppb */
#define GTS_UNC_1024_PPB                ((FLT)1e6*1024*1024)

/* Max error in Freq Est of XO Clock in PPB (17 PPM) */
#define GTS_FREQ_EST_ERR_BOUND_1024_PPB  17*1024*1024

/* Default Uncertainity if there is not enough motion data available */
#define GTS_DEFAULT_WWAN_TUNC_MS     0.1

/* LTE Time Unc based on MAR of 10 Km (33us) + Base Station Clock error (5us) */
#define GTS_LTE_MAR_TUNC_MS          0.038

/* GSM Time Unc based on MAR of 10 Km (33us) + 3 symbols wide unc. */
#define GTS_GSM_MAR_TUNC_MS          0.044077

/* TDS Time Unc based on MAR of 10 Km (33us) + Base Station Clock error (5us) */
#define GTS_TDS_MAR_TUNC_MS          0.038

/* WCDMA Time Unc based on MAR of 10 Km (33us) + Base Station Clock error (5us) */
#define GTS_WCDMA_MAR_TUNC_MS          0.038

/* Air speed model for uncertainty propagation in meters/second */
/* #define LOC_PROP_AIR_SPEED_MPS      ((FLT)(250.0)) ~ approx 1us/s*/
#define GTS_PROP_AIR_SPEED_TUNC_MS  0.01e-6

/* Max buffer size of Diag commands */
#define GTS_MAX_DIAG_CMD_SIZE   36

/* Max Time diff in MS between FCount Propagations QTime and ME Est */
#define GTS_MAX_FCOUNT_PROP_DIFF_MS     100

/* FT State and corresponding UNC Definitions (ppm) */
/* Based on the current FT state, XO Unc (USTMR/QTime) Unc will vary */
/* Following are the Max UNC constants for few of the FT States, For all 
   other FT States, The UNC value will be the Coarse Cal UNC */   
#define GTS_FT_STATE_COARSE_CAL   5
#define GTS_COARSE_CAL_UNC        17e-6

#define GTS_FT_STATE_IFC          6
#define GTS_IFC_UNC               11e-6

#define GTS_FT_STATE_FINE_CAL     7
#define GTS_FINE_CAL_UNC          10e-6

#define GTS_FT_STATE_FT1          8
#define GTS_FT1_UNC               9e-6

#define GTS_FT_STATE_FT2          11
#define GTS_FT2_UNC               3e-6

/* Define the Max Ustmr clock drift to be 17 ppm which is coarse Cal UNC */
#define GTS_USTMR_CLOCK_DRIFT     GTS_COARSE_CAL_UNC

/* Used to indicate some calculation failed and hence default value was used */
#define GTS_ERROR_DIFF_MS        4294967296.0

/* GSM frame timing */
#define GTS_GSM_FRAMES_PER_HYPERFRAME 2715648L // 26 * 51 * 2048
#define GTS_GSM_MSECS_PER_HYPERFRAME  12533760L // GSM_HYPERFRAME * 120 / 26
#define GTS_GSM_MSECS_PER_FRAME       4.61538462f // 120 / 26
#define GTS_GSM_NUM_SYMBOLS_PER_FRAME     5000
#define GTS_GSM_NUM_SYMBOLS_PER_SLOT      625
#define GTS_GSM_SYMBOL_LEN_IN_USEC        0.923076923f // 120 / (5 * 26)
#define GTS_GSM_MAX_DIFF_MS    GTS_GSM_MSECS_PER_HYPERFRAME //Max time between 2 GSM time instances 

/* WCDMA frame timing */
#define GTS_WCDMA_MSECS_PER_FRAME   10L
#define GTS_WCDMA_MAX_SFN_COUNT           4096L
#define GTS_WCDMA_MSECS_PER_MAX_SFN       40960L // 10 * 4096
#define GTS_WCDMA_CHIPX8_PER_MSEC         30720L
#define GTS_WCDMA_CHIPX8_PER_FRAME  307200L // 10 * 30720
#define GTS_WCDMA_MAX_DIFF_MS    GTS_WCDMA_MSECS_PER_MAX_SFN //Max time between 2 WCDMA time instances

/* TDS frame timing */
#define GTS_TDS_CX8_PER_MSEC          10240L
#define GTS_TDS_CX8_PER_SUB_FRAME     51200L
#define GTS_TDS_MSEC_PER_SUB_FRAME    5
#define GTS_TDS_MAX_SUB_FRAME_NUM     8192
#define GTS_TDS_ROLLOVER_MS           40960 // 5 * 8192
#define GTS_TDS_ROLLOVER_Cx8    419430400L // 10240 * 40960
#define GTS_TDS_MAX_DIFF_MS     GTS_TDS_ROLLOVER_MS //Max time between 2 TDS time instances

/* ----------------------------------------------------- *
 * IMPORTANT: The routines below are useful given the    *
 *            following preconditions:                   *
 *  - A must be in the range [0..M-1]                    *
 *  - B must be in the range [0..M-1]                    *
 *  - RESULT will be in the range [0..M-1]               *
 *     but requires a datatype in the range [0..2M-2]    *
 *     to prevent overflow.                              *
 *                                                       *
 *  If these conditions are not true, you can NOT use    *
 *  these functions!!                                    *
 * ----------------------------------------------------- */

/* Routine for (A + B) mod M and, (A - B) mod M */
#define GTS_ADD_MOD( a, b, m ) ( ( (a) + (b) < (m) ) ? ( (a) + (b) ) : ( (a) + (b) - (m) ) )
#define GTS_SUB_MOD( a, b, m ) ( ( (a) >= (b) ) ? ( (a) - (b) ) : ( (m) + (a) - (b) ) )

/* Routine for adding or subtracting 2 GSM frame numbers */
#define GTS_ADD_FN_GSM( a, b ) GTS_ADD_MOD( (a), (b), GTS_GSM_FRAMES_PER_HYPERFRAME )
#define GTS_SUB_FN_GSM( a, b ) GTS_SUB_MOD( (a), (b), GTS_GSM_FRAMES_PER_HYPERFRAME )
#define GTS_SUB_FN_WCDMA( a, b ) GTS_SUB_MOD( (a), (b), GTS_WCDMA_MAX_SFN_COUNT )
#define GTS_SUB_FN_TDS( a, b ) GTS_SUB_MOD( (a), (b), GTS_TDS_ROLLOVER_Cx8 )

#define GTS_MSG_LOW(a,b,c,d)            MSG_3(MSG_SSID_MGPME, MSG_MASK_5, a,b,c,d)
#define GTS_MSG_MED(a,b,c,d)            MSG_3(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d)
#define GTS_MSG_0(a)                    MSG(MSG_SSID_MGPME, MSG_MASK_6, a)
#define GTS_MSG_1(a,b)                  MSG_1(MSG_SSID_MGPME, MSG_MASK_6, a,b)
#define GTS_MSG_2(a,b,c)                MSG_2(MSG_SSID_MGPME, MSG_MASK_6, a,b,c)
#define GTS_MSG_3(a,b,c,d)              MSG_3(MSG_SSID_MGPME, MSG_MASK_5, a,b,c,d)
#define GTS_MSG_4(a,b,c,d,e)            MSG_4(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e)
#define GTS_MSG_5(a,b,c,d,e,f)          MSG_5(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f)
#define GTS_MSG_6(a,b,c,d,e,f,g)        MSG_6(MSG_SSID_MGPME, MSG_MASK_6, a,b,c,d,e,f,g)
#define GTS_MSG_HIGH(a,b,c,d)           MSG_3(MSG_SSID_MGPME, MSG_MASK_7, a,b,c,d)
#define GTS_MSG_ERROR(a,b,c,d)          MSG_3(MSG_SSID_MGPME, MSG_LEGACY_ERROR, a,b,c,d)
#define GTS_MSG_FATAL(a,b,c,d)          MSG_3(MSG_SSID_MGPME, MSG_LEGACY_FATAL, a,b,c,d)
#define GTS_MSG_SPRINTF_1(a,b);         MSG_SPRINTF_1( MSG_SSID_MGPME, MSG_LEGACY_HIGH, a, b);

/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/* Max Time Source elements */
/* These are also used to reference the DB array */
typedef enum
{
  GNSS_PE,
  GNSS_ME,
  LTE_ML1,
  FREQEST,
  GSM_L1_SUB1,
  GSM_L1_SUB2,
  GSM_L1_SUB3,
  WCDMA_L1_SUB1,
  WCDMA_L1_SUB2,
  TDSCDMA_L1,
  CDMA_M1X,
  MAX_SRC
} gts_TimeSrcModuleType;

typedef enum
{
  /* Device state is not known */
  GTS_MOTION_STATE_UNKNOWN      = 0,
  /* Device state is Stationary */
  GTS_MOTION_STATE_STATIONARY   = 1,
  /* Device state is In Motion */
  GTS_MOTION_STATE_IN_MOTION    = 2,
  /* Out of Range */
  GTS_MOTION_STATE_MAX = GTS_MOTION_STATE_IN_MOTION
} gts_MotionType;

/* Common Time Transfer Data Structure */
typedef struct
{
  /* Indicate if the latch was successful */
  boolean b_Latch;
  /* GPS RTC value latched at the same time as CDMA RTC (in GPS Chipx32) */
  U32  q_GnssRtc;
  /* GNSS RTC sample and chip counts @ Time transfer latch */
  U32  q_SampleCount;
  /* GNSS RTC Phase count @ Time transfer latch */
  U32  q_GnssPhase;
  /* Universal STMR value latched at the same time as the first latched
     CDMA/GNSS RTC set (in 19.2MHz untis) */
  U32  q_Ustmr;
} gts_LatchType;

/* IPC Message Definitions */

/* Registration Message Info Type */
typedef struct
{
  gts_ClientType e_ClientInfo;
  gts_TimeEstCbType p_CbFunction;
} gts_RegisterMsgType;

/* High Accuracy Time Maintenance Mode Enable/Disable Message */
typedef struct
{
  boolean b_HighAccuMode;
} gts_HATModeMsgType;

typedef struct
{
  gts_MotionType e_MotionState;
  gts_QtimeType z_QtimeInfo;
} gts_MotionRptMsgType;

/* Position Estimate Message */
typedef struct
{
  gps_ClockStructType z_GpsTimeEst;

  /* Latched Information */
  gts_LatchType z_LatchInfo;
  gts_QtimeType z_QtimeInfo;
} gts_PosEstMsgType;

/* Time Estimate Message from ME */
typedef struct
{
  mgp_TimeEstPutSourceIDEnum e_MESrc;
  gps_ClockStructType z_GpsTimeEst;

  /* Qtime Information */
  gts_QtimeType z_QtimeInfo;

  /* FCount at Qtime */
  U32 q_FCountQtime;
} gts_METimeEstMsgType;

/* Diag Command Msg */
typedef struct
{
  U8 u_DiagData[GTS_MAX_DIAG_CMD_SIZE];
  U8 u_Size;
} gts_DiagCmdMsgType;

/* Time Storage Structures */
typedef struct
{
  /* DL frequency */
  U32 q_Freq;
  /* Cell ID */
  U16 w_CellId;

  U64 t_DlRefTime;
  U16 w_Sfn;

  /* Clocked @ 30.72MHz */
  U32 q_Ostmr;
  FLT  f_OstmrBias;

  gts_QtimeType z_Qtime;

  /* Determine if Default UNC is already included */
  boolean b_DefaultUncAdded;
} gts_LteInfoDBType;

typedef struct
{
  /* Cell Information */
  /* Base Station Identification code */
  U8 u_Bsic;
  /* Control Channel Frequency */
  ARFCN_T z_Arfcn;

  /* Time Information */
  /* Frame Number 0..(26L*51L*2048L-1)  ~4.615ms frames */
  U32 q_FrameNum;
  /* Time Slot Number 0..7 */
  U8 u_TimeSlot;
  /* QSymbol Count (sub fn) */
  U32 q_QSymCnt;

  /* GSM State */
  U8 u_GL1State;

  /* Reference Time ( QTIME ) */
  gts_QtimeType z_QtimeInfo;

  /* Determine if Default UNC is already included */
  boolean b_DefaultUncAdded;
} gts_GsmInfoDBType;

typedef struct
{
  /* Cell Information */
  /* DL frequency. */
  U16  w_Freq;

  /* Cell Param ID */
  U16  w_CellId;

  /* Time Information */
  /* 64-bits value in TDS chipx8 units at status dump, clocked @ 10.24 MHz */
  U64  q_TdsTime;

  /* 30-bits value in TDS sub chipx8 units at status dump */
  U32  q_TdsPhase;

  /* TDS L1 State 0 – Paging, 1- Traffic*/
  U8 u_TDSL1State;

  /* Reference Time ( QTIME ) */
  gts_QtimeType z_QtimeInfo;

  /* Determine if Default UNC is already included */
  boolean b_DefaultUncAdded;
} gts_TdsInfoDBType;

typedef struct
{
  /* Cell Information */
  /* DL frequency and Primary scrambling code */
  U16  w_Freq;
  U16  w_PSC;

  /* Time Information */
  /* 10.3.6.75  System Frame Number  0..4095 10ms frames */
  U16  w_SysFn;

  /* 0..307200 Chipx8 per frame */
  U32  q_CX8Num;

  /* UMTS STMR Information including Fractional bits*/
  U64 t_UmtsStmr;

  /* GSM State */
  U8 u_WL1State;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;

  /* Determine if Default UNC is already included */
  boolean b_DefaultUncAdded;
} gts_WcdmaInfoDBType;

/* Time Storage Structures */
/* ME Time Storage Structure */
typedef struct
{
  /* GPS RTC @ the Time estimate */
  U32 q_GpsRtc;

  /* Source from which Time estimate was rcvd */
  mgp_TimeEstPutSourceIDEnum e_TimeEstSrc;

  /* Freq Bias information */
  gps_FreqStructType z_Freq;
} gts_METimeType;

/* Position Storage structure */
typedef struct
{
  /* Freq Bias information */
  gps_FreqStructType z_Freq;
} gts_PosEstDbType;

typedef struct
{
  /* Indicates if the mobile moved since the last update */
  boolean b_Moved;
  /* Qtime at the earliest instance when motion was detected */
  gts_QtimeType z_FirstMoveTime;

  /* Type of movement detected at the last sensor input */
  gts_MotionType e_LastMoveType;
  /* Qtime at the last instance when motion was detected */
  gts_QtimeType z_LastMoveTime;
} gts_MotionInfoType;

typedef struct
{
  U64 t_PrevQtime;    /* Time at the last temp Update */
  U64 t_CurrQtime;    /* Time at the current temp Update */
  DBL d_CurrFreqEst;      /* Freq Est based on Temperature at Prev and Curr Time */
  FLT f_FreqEstUncMs; /* Time Uncertainity estimate based on temp table */
}gts_FreqEstDBType;

/* All Possible Time Source Modules */
typedef union
{
  gts_LteInfoDBType z_LteInfo;
  gts_GsmInfoDBType z_GsmInfo;
  gts_WcdmaInfoDBType z_WcdmaInfo;
  gts_TdsInfoDBType z_TdsInfo;
  gts_FreqEstDBType z_FreqEstInfo;
  gts_METimeType z_MeTimeInfo;
  gts_PosEstDbType z_PePosInfo;
} gts_InputInfoType;

/* Time Storage Database Element Definition */
typedef struct
{
  /* Element has valid information */
  boolean b_Valid;

  /* Source Module from which Time was recieved */
  gts_TimeSrcModuleType e_SrcModule;

  /* Input information for propagation */
  gts_InputInfoType z_InputInfo;

  /* Absolute GPS Time */
  gts_TimeType z_GPSTime;
} gts_TimeDbElementType;

/* Time Storage Database Definition */
typedef struct
{
  /* QTIME at the last Time info update */
  U64 t_LastUpdateTime;
  /* Module which provided the last Time update */
  gts_TimeSrcModuleType e_LastUpdateModule;
  /* Elements with Time information */
  gts_TimeDbElementType z_TimeDbElement[MAX_SRC];
} gts_TimeDbType;

/* Serving System Information */
typedef struct
{
  sys_sys_mode_e_type   e_SysMode;
  sys_srv_status_e_type e_SrvStatus;
} gts_SrvSysInfoType;

typedef struct
{
  /* High Accuracy Time Update Enabled/Disabled */
  U16 b_HATModeEn : 1;
  /* High Accuracy Time Update Status - Enabled/Disabled */
  U16 b_HATStatus : 1;
  /* Use Punc based Tunc model */
  U16 b_UsePuncBasedTunc : 1;
  /* Enabling/Disabling DB Dump at Position Report */
  U16 b_EnDbDump : 1;
  /* When doing Dump Db log, should it be Raw Db or Prop Db*/
  U16 b_RawDbDump : 1;
  /* Determines if ME should use GTS Time for Regular sessions */
  U16 b_ForceUseGtsTime : 1;
  /* Determines if ME should use FT State based USTMR Unc */
  U16 b_UseFTStateUstmrUnc : 1;  
  U16 : 8;
} gts_FlagsStructType;

/* Request Time Info structure */
typedef struct
{
  U8 b_InUse;
  gts_ClientType e_ClientInfo;
  gts_TimeEstCbType p_CbFunction;
  U8 b_ContUpdates;
} gts_ClientsDbType;

/* GTS Paramaters */
typedef struct
{
  gts_FlagsStructType z_Flags;
  /* Diag based Input Source Blocking */
  U32 q_BlockSrc;

  /* GTS Clients with callback function */
  gts_ClientsDbType z_GtsClients[GTS_CLIENT_MAX];
  gts_MotionInfoType z_MotionInfo;
  gts_SrvSysInfoType z_SrvSys[3][2];

  /* USTMR Unc Ms based on FT State */
  FLT f_UstmrUncMs;
} gts_ParamsType;

/* Log Packet Definition */
#ifdef T_WINNT
#error code not present
#endif

/* Common Time Transfer Data Structure */
typedef PACKED struct PACKED_POST
{
  /* Indicate if the latch was successful */
  U8 b_Latch;
  /* GPS RTC value latched at the same time as CDMA RTC (in GPS Chipx32) */
  U32  q_GnssRtc;
  /* GNSS RTC sample and chip counts @ Time transfer latch */
  U32  q_SampleCount;
  /* GNSS RTC Phase count @ Time transfer latch */
  U32  q_GnssPhase;
  /* Universal STMR value latched at the same time as the first latched
     CDMA/GNSS RTC set (in 19.2MHz untis) */
  U32  q_Ustmr;
} gts_LatchLogType;

/* Reference Time structure for Timetags */
typedef PACKED struct PACKED_POST
{
  /* Ref Time */
  U8  b_QtimeValid;
  /* Ref Time Validity */
  U64 t_Qtime;
  U32 f_QtimeBias;
} gts_QtimeLogType;

typedef PACKED struct PACKED_POST
{
  uint16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;     /* GPS msec at reference tick [msecs] */
  uint32 f_ClkTimeBias; /* Clock bias [msecs] */
  uint32 f_ClkTimeUncMs;/* 1 sided max time bias uncertainty [msecs] */
} gts_GpsTimeLogType;

typedef PACKED struct PACKED_POST
{
  uint32 f_ClkFreqBias;  /* Clock frequency bias. [m/s] */
  uint32 f_ClkFreqUnc; /* 1 sided max freq bias uncertainty [m/s] */
} gts_GpsFreqLogType;

typedef PACKED struct PACKED_POST
{
  uint32  q_RefFCount;  /* FCount value at reference tick */
  uint8   u_SystemRtcValid;
  uint64  t_SystemRtcMs;    /* System RTC value at clock validity */
  uint32  q_NumClockResets; /* Cumulative # of Clock resets. */
  gts_GpsTimeLogType z_Time;   /* Time at w_RefFCount */
  gts_GpsFreqLogType z_Freq;   /* Freq at w_RefFCount */
} gts_GpsClockLogType;

typedef PACKED struct PACKED_POST
{
  gts_GpsClockLogType z_GpsTimeEstLog;

  /* Latched Information */
  gts_LatchLogType z_LatchInfoLog;
  gts_QtimeLogType z_QtimeInfoLog;
} gts_PosEstMsgLogType;

typedef PACKED struct PACKED_POST
{
  U32  q_VsrcFromFW[2];
  U32  q_VsrcPhFromFW[2];
  U32  q_OstmrFromFW[2];
  U32  q_OstmrPhFromFW[2];
  U64  t_RefDlTime;
} gts_VsrcInfoLogType;

typedef PACKED struct PACKED_POST
{
  U32  q_Freq;    /* DL frequency - Same as lte_earfcn_t*/
  U16  w_CellId;  /* Physical Cell ID */

  /* SIB8 TRUE: LTE NW is synchronized and supports 1x. FALSE: LTE NW is NOT synchronized */
  U8 b_Sib8Valid;
  /* The CDMA2000 system time corresponding to the SFN boundary at or after the ending boundary of the SI-Window in which this Time Transfer was taken. The size is 52 bits and the unit is [CDMA2000 chips] */
  U64  t_Sib8CdmaSystemTime;

  U8 b_Sib16Valid; /* SIB16 System Time info*/
  U64  t_Sib16GpsMsecs; /* The Abs GPS time at the SFN boundary in of 10 msecs */
  U16  w_LeapSeconds; /* Number of leap seconds between UTC time and GPS time */

  U16  w_Sfn;             /* System Frame Number at which data was collected */
  U32  q_OStmrSfBoundary;  /* OSTMR at the boundary of the subframe */

  /* Latch Information */
  U32  q_UstmrLatched;   /* USTMR Latched in 19.2 Mhz Units */
  U32  q_OstmrLatched;   /* OSTMR Latched in 30.2 Mhz Units (Ts) */
  U32  q_OstmrPhLatched; /* OSTMR Latched in 30.2 Mhz Units (sub Ts)*/

  /* VSRC Information for PCC */
  /* Mark if the next field is valid or not */
  U8 b_OstmrVsrcValid;
  gts_VsrcInfoLogType z_RefVsrcInfo;
  gts_VsrcInfoLogType z_CurrVsrcInfo;

  /* Reference Time (QTIME) */
  gts_QtimeLogType  z_QtimeInfo;
} gts_LteTimeInfoLogType;

/* ARFCN type, includes band if quadband is supported */
typedef enum
{
  GTS_SYS_BAND_EGSM_900,
  GTS_SYS_BAND_PGSM_900,
  GTS_SYS_BAND_PCS_1900,
  GTS_SYS_BAND_DCS_1800,
  GTS_SYS_BAND_CELL_850,
  GTS_INVALID_BAND,
  GTS_SYS_BAND_NONE = 0xFF  
} gts_GsmBandLogType;

typedef PACKED struct PACKED_POST
{
  U16 w_BandNum;
  U8 e_BandType; // type 'gts_GsmBandLogType'
} gts_GsmArfcnLogType;

typedef PACKED struct PACKED_POST
{
  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  U8 u_SubInfo;

  /* Cell Information. Base Station Identification code and freq */
  U8 u_Bsic;
  gts_GsmArfcnLogType z_Arfcn;

  /* Time Information */
  /* Frame Number 0..(26L*51L*2048L-1)  ~4.615ms frames */
  U32 q_FrameNum;
  /* Time Slot Number 0..7 */
  U8 u_TimeSlot;
  /* QSymbol Count (sub fn) */
  U32 q_QSymCnt;

  /* GSM State */
  U8 u_GL1State;

  /* Reference Time (QTIME) */
  gts_QtimeLogType  z_QtimeInfo;
} gts_GsmTimeInfoLogType;

typedef PACKED struct PACKED_POST
{
  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  U8 u_SubInfo;

  /* Cell Information */
  /* DL frequency and Primary scrambling code */
  U16  w_Freq;
  U16  w_PSC;

  /* Time Information */
  /* 10.3.6.75  System Frame Number  0..4095 10ms frames */
  U16  w_SysFn;

  /* 0..307200 Chipx8 per frame */
  U32  q_CX8Num;

  /* UMTS STMR Information including Fractional bits*/
  U64 t_UmtsStmr;

  /* GSM State */
  U8 u_WL1State;

  /* Reference Time (QTIME) */
  gts_QtimeLogType  z_QtimeInfo;
} gts_WcdmaTimeInfoLogType;

typedef PACKED struct PACKED_POST
{
  /* Subscription Information – For MultiSim Support */
  U8 u_SubInfo;
  /* Cell Information */
  /* DL frequency. */
  U16  w_Freq;

  /* Cell Param ID */
  U16  w_CellId;

  /* Time Information */
  /* 64-bits value in TDS chipx8 units at status dump, clocked @ 10.24 MHz */
  U64  q_TdsTime;

  /* 30-bits value in TDS sub chipx8 units at status dump */
  U32  q_TdsPhase;

  /* TDS L1 State 0 – Paging, 1- Traffic*/
  U8 u_TDSL1State;

  /* Reference Time (QTIME) */
  gts_QtimeLogType  z_QtimeInfo;
} gts_TdsTimeInfoLogType;

/* GTS Freq Est Structure */
typedef PACKED struct PACKED_POST
{
  U64 t_PrevQtime;  /* Time at the last temp Update */
  U64 t_CurrQtime; /* Time at the current temp Update */
  S32  l_FreqEst;   /* Freq Est based on Temperature at Prev and Curr Time */
  U32  f_FreqEstUncMs; /* Time Uncertainity estimate based on temp table */
  U8  u_FtCalState;  /* FT Calibration state */
} gts_FreqEstInfoLogType;

/* Time Estimate Message from ME */
typedef PACKED struct PACKED_POST
{
  U8 e_MESrc;
  gts_GpsClockLogType z_GpsTimeEst;

  /* Latched Information */
  gts_QtimeLogType z_QtimeInfoLog;
  U32 q_FCount;
} gts_METimeEstMsgLogType;

/* GTS Time Input Log Type */
typedef PACKED union PACKED_POST
{
  gts_PosEstMsgLogType z_PosEstMsgLog;
  gts_METimeEstMsgLogType z_METimeEstMsgLog;
  gts_LteTimeInfoLogType z_LteTimeInfoLog;
  gts_GsmTimeInfoLogType z_GsmTimeInfoLog;
  gts_WcdmaTimeInfoLogType z_WcdmaTimeInfoLog;
  gts_TdsTimeInfoLogType z_TdsTimeInfoLog;
  gts_FreqEstInfoLogType z_FreqEstInfoLog;
} gts_ModuleInfoLogType;

/* GTS Input Information Log Packet */
typedef PACKED struct PACKED_POST
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  uint8 u_Version;

  U8 e_TimeModuleSource; // of enum type 'gts_TimeSrcModuleType'
  gts_ModuleInfoLogType z_TimeInputLog;
} gts_TimeInputLogType;

/* GTS Time Update Log Packet */
typedef PACKED struct PACKED_POST
{
  /* If the time below is valid or not */
  U8 b_Valid;

  /* Time Source */
  U8 e_TimeSrc;

  /* Absolute GPS Time related information */
  U16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  U32 q_GpsMsec;     /* GPS msec at reference tick [msecs] */
  U32 f_ClkTimeBias; /* Clock bias [msecs]. Constraint to >-0.5 and <0.5 */
  U32 f_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */

  /* Reference Time (QTIME) */
  gts_QtimeLogType  z_QtimeLogInfo;

  /* GPS Clock Specific Fields */
  U8  b_FCountValid; /* Mark if FCount below is valid */
  U32 q_GpsFCount;    /* GPS FCount at the Time Estimate */
}gts_TimeLogType;

typedef PACKED struct PACKED_POST
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  uint8 u_Version;

  U8 e_ModSrc;
  gts_TimeLogType z_PrevTime;
  gts_TimeLogType z_CurrTime;
}gts_TimeUpdateLogType;

typedef PACKED struct PACKED_POST
{
  /* Source Module */
  U8  e_SrcMod;
  /* Source Module Time */
  gts_TimeLogType z_GpsTime;

  /* Time Difference with GPS Ref Time */
  U8  b_TimeDiffValid;
  U64 d_TimeDiffMs;
}gts_DbTimeLogType;

typedef PACKED struct PACKED_POST
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  uint8 u_Version;

  /* 0x0 - Raw Time Dump
     0x1 - Time Dump propagated to recent GPS Fix */
  U8 u_Flags;
  /* Reference Time from GPS Fix */
  gts_TimeLogType z_GpsRefTime;
  gts_DbTimeLogType z_ModSrcTime[MAX_SRC];
}gts_DbLogType;

#ifdef T_WINNT
#error code not present
#endif

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/

/*
 ******************************************************************************
 * Function: gts_LatchGpsClock
 *
 * Description:
 *  This function calls the Time Latching Function which in turn captures
 *  USTMR and GPS RTC
 *
 * Parameters:
 *  z_LatchInfo - GPS and USTMR is latched and updated in this structure
 *  z_QtimeInfo - Qtime captured close to the latch
 *
 * Dependencies:
 *  The capture is done blocking interrupts
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void gts_LatchGpsClock( gts_LatchType* pz_LatchInfo, gts_QtimeType* pz_QtimeInfo);

/*
 ******************************************************************************
 * Function gts_InputSrcDisabled
 *
 * Description:
 *
 *  Returns current status of Diag based blocking input source from input modules
 *
 * Parameters:
 *
 *  e_ModSrc - Source which should be checked
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Returns the TRUE if the source should be blocked, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_InputSrcDisabled( gts_TimeSrcModuleType e_ModSrc );

/*
 ******************************************************************************
 * Function gts_GetGtsTimeNow
 *
 * Description:
 *
 *  This function reads the current time from GTS module. The possible sources
 *  are ME (GPS Fix, Timetag, Timetransfer etc) or FreqEst based time or
 *  LTE based Time
 *
 * Parameters:
 *
 *  pz_TimeEstMsg - Pointer to the GTS Time structure - This will be updated with
 *  Time information
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
boolean gts_GetGtsTimeNow( gts_TimeType* pz_TimeEst );

/*
 ******************************************************************************
 * Function gts_ProcFreqEstMsg
 *
 * Description:
 *
 *  This function will use the Freq Estimate Error to propagate last known
 *  GPS Fix time which was based on Freq Est to current Qtime/Slow Clock Time
 *
 * Parameters:
 *
 *  pz_FreqEst - Freq Estimate Information
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
void gts_ProcFreqEstMsg( gts_FreqEstInfoType *pz_FreqEst );

/*
 ******************************************************************************
 * Function gts_ProcPosEstMsg
 *
 * Description:
 *
 *  This function handles the Position Est Message from PE which has GPS Time
 *  from Position Fix
 *
 * Parameters:
 *
 *  pz_PosEst - Position Estimate Message ( includes Time as well )
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
void gts_ProcPosEstMsg( gts_PosEstMsgType *pz_PosEst );

/*
 ******************************************************************************
 * Function gts_ProcTimeEstRptMsg
 *
 * Description:
 *
 *  This function handles the Time estimate from Measurement Engine
 *
 * Parameters:
 *
 *  pz_TimeEstMsg - Time Estimate Report Message
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
void gts_ProcTimeEstRptMsg( gts_METimeEstMsgType *pz_TimeEstMsg );

/*
 ******************************************************************************
 * Function gts_ProcTimeEstReqMsg
 *
 * Description:
 *
 *  This function handles the Time estimate request message from any client. 
 *  Time estimate is inturn sent to the client using the call back function 
 *  which they have registered during Init
 *
 * Parameters:
 *
 *  pz_TimeEst - Time Estimate Report Message
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
void gts_ProcTimeEstReqMsg();

/*
 ******************************************************************************
 * Function gts_ProcMotionRptMsg
 *
 * Description:
 *
 *  Handle the motion data from Sensors ( sent by SLIM module )
 *  Motion data is mainly used to restrict the uncertainity of WWAN Time
 *
 * Parameters:
 *
 *  pz_MotionRptMsg - Motion data Msg
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
void gts_ProcMotionRptMsg ( const gts_MotionRptMsgType* pz_MotionRptMsg );

/*
 ******************************************************************************
 * Function gts_ProcHATModeMsg
 *
 * Description:
 *
 *  This function handles the High Accuracy Mode Msg which indicates that
 *  Background positioning is enabled and need  high accuracy time with a
 *  increased power consumption. This will enable Time maintenance using
 *  Slowclock and Temperature compensation
 *
 * Parameters:
 *
 *  pz_HAModeMsg - High Accuracy Mode Msg
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
void gts_ProcHATModeMsg ( const gts_HATModeMsgType* pz_HAModeMsg );

/*
 ******************************************************************************
 * Function gts_ProcHATStatusMsg
 *
 * Description:
 *
 *  This function handles the High Accuracy Status Msg which indicates that
 *  high accuracy time updates are enabled in FREQEST module
 *
 * Parameters:
 *
 *  pz_HATStatus - High Accuracy Status Msg
 *
 * Dependencies:
 *
 *  Client should have already registered with GTS 
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_ProcHATStatusMsg ( const gts_HATStatusMsgType* pz_HATStatus );

/*
 ******************************************************************************
 * Function gts_ProcDiagCmdMsg
 *
 * Description:
 *
 *  Handle the diag commands IPC
 *
 * Parameters:
 *   pz_DiagMsg - Pointer to the Diag command Message
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
void gts_ProcDiagCmdMsg( gts_DiagCmdMsgType* pz_DiagMsg );

void gts_ProcLteTimeMsg( const gts_LteTimeInfoType *pz_LteTime );
void gts_ProcWcdmaTimeMsg( const gts_WcdmaTimeInfoType *pz_WcdmaTime );
void gts_ProcGsmTimeMsg( const gts_GsmTimeInfoType *pz_GsmTime );
void gts_ProcTdsTimeMsg( const gts_TdscdmaTimeInfoType *pz_TdsTime );
