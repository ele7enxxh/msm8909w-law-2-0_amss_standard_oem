#ifndef CGPS_API_H
#define CGPS_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C-GPS API Header File

GENERAL DESCRIPTION
  This file contains common defines, structures and enum definitions that are
  used both within and outside C-GPS code.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 
  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/api/cgps_api.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/10   dsn     Added support for DSDS Sclk Time-transfer.
06/10/09   gk      Added Get UTC offset from NV
12/24/08   jlp     Added support for gps restriction functionality (for LCU/camera).
10/04/08   bp      changes for supporting QDSP6
09/20/07   va      Create wrapper for clk_regime_cgps_freq_offset_ppt
09/13/07   va      Move XO Featurization outside the library
07/31/07   br      Added support for 1x Allow GPS interface.
05/05/07   br      Added support for RF Info interface. Fixed lint warnings.
04/19/07   ld      Intermediate check-in for UMTS Time-tagging
04/11/07   df      Add RF API to report GP linearity changes
03/15/07   br      Removed unused definitions. Included gps_task.h.
02/16/07   br      Added PPM - 1x SRCH command interface.
02/13/07   br      Removed GPS RTC related definitions.
02/09/07   ld      Initial Time-tag API definition.
02/08/07   br      Renamed some definitions in 1x SRCH interface.
02/06/07   ld      Time-Transfer structure changes
02/05/07   br      Added more 1x SRCH interface changes.
01/31/07   br      Added some 1x SRCH interface changes.
01/04/07   ld      Added LPF Bypass parameter to BC Config structure.
12/19/06   br      Added proper featurization and more support for 1x interface.
11/17/06   br      Added 1x SRCH interface.
11/07/06   br      Initial implementation.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


#include "comdef.h"

#ifdef __arm__
#define GPS_INLINE static __inline
#else
#define GPS_INLINE  INLINE
#endif

#include "cai.h"
#include "qw.h"
#include "timetick.h"
#include "rex.h"
#include "lte_as.h"
#include "sys.h"

/*****************************************************************************
 * Definitions used by C-GPS to interface with MemArb
 *****************************************************************************
*/
typedef struct
{
  uint32 q_UniqueToken; /* Token assigned to request returned by Allocate */
  uint32 q_BaseAddress; /* Base address of block that got released or acquired */
  uint32 q_Size;        /* Size of block that got released or acquired */
}cgps_MemArbNotifyInfoType;

/*****************************************************************************
 * Definitions used by API between RF Drivers and C-GPS
 *****************************************************************************
*/

/* Define values for no band and channel */
#define CGPS_NO_TUNE_BAND 0xFF
#define CGPS_NO_TUNE_CHAN 0xFFFF

/* Define system time error value */
#define CGPS_SYSTEM_TIME_ERROR_HI        0
#define CGPS_SYSTEM_TIME_ERROR_LO        0

/* Define invalid Ref PN */
#define CGPS_INVALID_REF_PN   0xFFFF

/* Define max number of sets of latched RTCs per each 1x Time Transfer Data
   report */
#define CGPS_MAX_TT_SAMPLES_PER_REPORT  8

/* Define */
#define CGPS_LTE_RXTX_MAX_RESULTS (32)

typedef struct {
  uint8 u_LpfLength;
  uint8 u_GainStep;
  uint8 u_BlankEn;
  uint8 u_BlankPol;
  uint8 u_QuantSel;
  uint8 u_QuantInv;
  uint8 u_QuantLev;
  uint8 u_Rate;
  uint8 u_LpfBypass;
}
cgps_BcConfigType;

typedef struct
{
  uint8 u_LpfBypass;
}
cgps_BcConfig2Type;

typedef struct
{
  uint8 u_EqCoef0;
  uint8 u_EqCoef1;
  uint8 u_EqCoef2;
  uint8 u_EqCoef3;
}
cgps_BcEqCoefType;

typedef enum
{
  BLANK_ON,
  BLANK_OFF,
  BLANK_FORCE_OFF
}
cgps_RfSwBlankCmdEnum;

typedef struct {
  boolean u_SwapIQ;
  boolean u_InvertQ;
  boolean u_InvertI;
}
cgps_IQControlType;

typedef struct {
  int16   x_MeanI;
  int16   x_MeanQ;
  uint16  w_AmpI;
  uint16  w_AmpQ;
  uint32  q_GpsRtc;
  boolean u_SampleCorrupted;
}
cgps_RfDcGainType;

typedef struct {
  int32   l_AccumMeanI;         /* 24 bit */
  int32   l_AccumMeanQ;         /* 24 bit */
  uint32  q_SamplesAccumulated;
  uint32  q_GpsRtc;
}
cgps_RfAccumulatedDcMeanType;

typedef struct {
  int16   x_MeanI;
  int16   x_MeanQ;
}
cgps_RfDcType;

/* cgps_RfLinearityEnum specifies which GPS RF linearity setting
   is in use. */
typedef enum
{
  LINEARITY_HI,
  LINEARITY_HI_AWS,
  LINEARITY_LO
}cgps_RfLinearityEnum;

typedef struct
{
  /* Either LINEARITY_LO or LINEARITY_HI. (See above) */
  cgps_RfLinearityEnum  e_RfLinearity;

  /* Absolute carrier phase (arbitrary reference) in units of
     cycles.

     A practical but not necessarily mandatory way to implement this is to
     report 0.0 cycles for LINEARITY_LO and somewhere in the range
     of 0.0 to 1.0 cycles for LINEARITY_HI mode.

     Note that for data demodualtion correction it is only the difference
     that actually matters. */
  float f_CarrierPhase;
}
cgps_RfLinearityType;

typedef struct
{
  uint8 u_ImmBlank;
  uint8 u_QuantLev;
  uint8 u_NfMode;
  uint8 u_DDCMode;
  uint8 u_DemuxMode;
  uint8 u_Init;
  uint32 q_IfFreq;
} cgps_RfBcAuxConfigType;

typedef struct
{
  uint8 u_NotchPole;
  uint8 u_Mode;
  uint32 q_NotchFreq;
} cgps_RfBcAuxNotchConfigType;

typedef struct
{
  int16 x_MeanI;
  int16 x_MeanQ;
} cgps_RfBcAuxMeanStatusType;

typedef struct
{
  int16 x_MeanI;
  int16 x_MeanQ;
} cgps_RfRpMeanMonType;

typedef struct
{
  uint8 u_RsMode;
  uint8 u_DdcMode;
  uint8 u_QuantLev;
  uint8 u_LpfBypass;
  uint8 u_AuxInSel;
  uint8 u_Init;
} cgps_RfRpConfigType;

typedef struct
{
  uint8 u_LowCoef[11];
  uint8 u_HighCoef[11];
  uint8 u_FloorCount;
} cgps_RfRpResamplerMapType;

typedef enum
{
  GPS_RF_ASIC,
  GPS_RF_GAIN_ADJUST,
  GPS_RF_DC_DAC_IQ,
  GPS_RF_VCO_TUNING_CODE
} cgps_RfStatusReportEnum;

/*****************************************************************************
 * Definitions used by APIs between 1x SRCH task and C-GPS, and
 * EVDO module and C-GPS
 *****************************************************************************
*/

/* Data structure for active pilot set information */
typedef struct
{
  uint16  w_PN;
  int8    b_Ecio;

}
cgps_PilotType;

/* Data structure for PER/FER estimate */
typedef struct
{
  /* Number of packets sampled for PER measurement */
  uint16  w_MeasPkts;

  /* Number of errors in sample */
  uint16  w_ErrPkts;

}
cgps_FERType;


/*****************************************************************************
 * Definitions used by API between 1x SRCH task and C-GPS
 *****************************************************************************
*/

/* Sync80 Data Structure */
typedef struct
{
  /* Current 1x serving frequency band associated to Ref PN */
  uint8   u_Band;

  /* Current 1x serving frequency channel associated to Ref PN */
  uint16  w_Channel;

  /* Rx AGC value (in dBm units) of receive chain at the last tune to home
     band/channel */
  uint8   u_RxAgc;

  /* 1x Search Window Center in Chipx8 units */
  uint32  q_WinCenter;
}
cgps_1xPPMOpGrantDataType;

/* Sync80 Data Structure */
typedef struct
{
  /* 1x system time in qword format */
  qword   t_SystemTime;

  /* Current value of the slow clock counter */
  uint32  q_SclkTime;

  /* CDMA RTC value latched at the last Sync80 tick (in CDMA Chipx8) */
  uint32  q_CdmaRtc;

  /* Current 1x Ref PN offset */
  uint16  w_RefPN;

  /* Current 1x serving frequency band associated to Ref PN */
  uint8   u_Band;

  /* Current 1x serving frequency channel associated to Ref PN */
  uint16  w_Channel;

  /* Current master finger position, where master fingers is
   * assigned to Ref PN Offset */
  int32   l_MasterFingerPosition;

  /* Rx AGC value of receive chain at the last tune to home band/channel */
  uint8   u_RxAgc;

  /* Mobile Station Transmit Offset validity */
  /* Flag is TRUE if 1x system is currently transmitting, i.e., in
   * Traffic Channel state and PA is on AND not fading (Ignore Access State) */
  boolean u_MstoValid;

  /* CDMA RTC phase value latched at the last Sync80 tick (in CDMA Chipx2^35) */
  uint32  q_CdmaRtcSync80Phase;

  /* Difference between CDMA RTC and VSRC STMR (in CDMA Chipx8192 units) */
  uint32  q_CdmaRtcVsrcStmrDiff;
}
cgps_1xSync80DataType;

/* Time Transfer sets of CDMA+GNSS RTCs latched at the same time structure */
typedef PACKED struct
{
  /* CDMA RTC count value (in CDMA Chipx8) */
  uint32  q_CdmaRtc;

  /* CDMA RTC phase value (in CDMA Chipx2^35) */
  uint32  q_CdmaRtcPhase;

  /* GNSS RTC value (in GPS Chipx80) */
  uint32  q_GnssRtc;

  /* GNSS Phase value (in sub-GPS Chipx80) */
  uint32  q_GnssPhase;

  /* GNSS Sample Count status */
  uint32  q_SampleCount;    
}
cgps_1xTTLatchedDataType;

/* Time Transfer Data Structure */
typedef struct
{
  /* 1x system time in qword format */
  qword   t_SystemTime;

  /* Current value of the slow clock counter */
  uint32  q_SclkTime;

  /* CDMA RTC value latched at the last Sync80 tick (in CDMA Chipx8) */
  uint32  q_CdmaRtcSync80;

  /* Current 1x Ref PN offset */
  uint16  w_RefPN;

  /* Current 1x serving frequency band associated to Ref PN */
  uint8   u_Band;

  /* Current 1x serving frequency channel associated to Ref PN */
  uint16  w_Channel;

  /* Current master finger position, where master fingers is
   * assigned to Ref PN Offset */
  int32   l_MasterFingerPosition;

  /* Maximum Antenna Range */
  uint32  q_Mar;

  /* Best estimate of the current 1x system time uncertainty */
  uint32  q_SystemTimeUncertaintyEstimate;

  /* CDMA RTC value latched at the same time as GPS RTC (in CDMA Chipx32) */
  uint32  q_CdmaRtcLatched;

  /* GPS RTC value latched at the same time as CDMA RTC (in GPS Chipx32) */
  uint32  q_GpsRtcLatched;
  uint32  q_GnssRtcLatched;

  /* LPM RTC value latched at the same time as the first latched CDMA/GNSS RTC
     set (in 19.2MHz untis) */
  uint32  q_LPMRtcLatched;

  /* CDMA RTC phase value latched at the last Sync80 tick (in CDMA Chipx2^35) */
  uint32  q_CdmaRtcSync80Phase;

  /* Universal STMR value latched at the same time as the first latched
     CDMA/GNSS RTC set (in 19.2MHz untis) */
  uint32  q_UnivStmrLatched;

  /* CDMA RTC STMR control register read at the same time as the first latched
     CDMA/GNSS RTC set (in CDMA Chipx2^35 units) */
  uint32  q_CdmaRtcFcw;

  /* Number of sets of latched RTCs per 1x Time Transfer Data report.
     u_NumTTSamples should be <= CGPS_MAX_TT_SAMPLES_PER_REPORT
     u_NumTTSamples == 0 presents error condition
   */
  uint8   u_NumTTSamples;

  /* Sets of CDMA+GNSS RTCs latched at the same time */
  cgps_1xTTLatchedDataType z_LatchedData[ CGPS_MAX_TT_SAMPLES_PER_REPORT ];

  /* Difference between CDMA RTC and VSRC STMR (in CDMA Chipx8192 units) */
  uint32  q_CdmaRtcVsrcStmrDiff;
}
cgps_1xTTDataType;


/* Data structure to latch Slow Clock with System Time */
typedef struct
{
  /* 1x System time in qword format */
  qword   t_SystemTimeT1;

  /* CDMA RTC value exactly when System Time (T1) was obtained */
  uint32  q_CdmaRtcAtT1;

  /* Value of the Slow clock counter latched at the same time(T2) as CDMA RTC */
  uint64  q_SclkRtcAtT2;

  /* CDMA RTC value latched at the same time(T2) as Slow Clock counter */
  uint32  q_CdmaRtcAtT2;

 /* Reacquisition slew */
   uint32  q_ReacqSlew;

 /* Frequency Error Estimate of the slow clock */
   int32  q_SclkFee;

  /* Best estimate of the current 1x system time uncertainty */
  uint32  q_SystemTimeUncertaintyEstimate;

  /* Maximum Antenna Range */
  uint32  q_Mar;

}
cgps_1xSclkTTDataType;

typedef enum
{
  CDMA_TTR,
  LTE_TTR,
  REG_TTG,
  LTE_TTG,  
  GPS_USTMR
}
cgps_TimeReqType;

/* Common Time Transfer Data Structure */
typedef struct
{
  /* Indicate if the latch was successful */
  boolean b_Latch;  
  /* GPS RTC value latched at the same time as CDMA RTC (in GPS Chipx32) */
  uint32  q_GnssRtc;
  /* GNSS RTC sample and chip counts @ Time transfer latch */
  uint32  q_SampleCount;
  /* GNSS RTC Phase count @ Time transfer latch */
  uint32  q_GnssPhase;
  /* LPM RTC value latched at the same time as the first latched CDMA/GNSS RTC
     set (in 19.2MHz untis) */
  uint32  q_LPMRtc;
  /* Universal STMR value latched at the same time as the first latched
     CDMA/GNSS RTC set (in 19.2MHz untis) */
  uint32  q_UnivStmr;
  /* WWWAN Specific Info */
  /* CDMA Time */
  uint32  q_CdmaRtc;
  uint32  q_CdmaPhase;
  /* LTE Time */
  uint32  q_Ostmr;
  uint32  q_OstmrPhase;
  /* Mark if Dal Systime is valid or not */
  uint64  t_RefTime;
  boolean b_RefTimeValid;  
}
cgps_ComTimeType;

typedef enum
{
  TT_INVALID,
  TT_VALID,
  TT_OLD
}
cgps_1xTtValidEnum;

/* Data structure containing all necessary data in the case 1x clocks/RTC
   operation is interrupted for PPM module */
typedef struct
{
  /* TRUE if 1x clocks/RTC will be interrupted follwing this command */
  boolean   b_1xClockInterruption;

  /* Quality of included Sync80 Data */
  cgps_1xTtValidEnum  e_1xTtValid;

/* Sync80 Data Structure */
  cgps_1xSync80DataType  z_Sync80Data;
}
cgps_1xPpmOpStopsDataType;

/* Data structure containing all necessary data in the case 1x clocks/RTC
   operation is interrupted for GPS module */
typedef struct
{
  /* TRUE if 1x clocks/RTC will be interrupted follwing this command */
  boolean   b_1xClockInterruption;

  /* Quality of included Time Transfer Data */
  cgps_1xTtValidEnum  e_1xTtValid;

/* Time Transfer Structure */
  cgps_1xTTDataType  z_TTData;
}
cgps_1xTTOpStopsDataType;


/* 1x RF Info defionitions and structures */

/* Maximum size of the Active Set that would be used for reporting 1x RF Info
*/
#define CGPS_1X_RF_INFO_ASET_MAX_SIZE  6

typedef struct
{
  /* Tx Power */
  word              b_TxPower;

  /* PER/FER data */
  cgps_FERType      z_FER;

  /* Received Signal Strength Indication */
  int8              b_RSSI;

  /* Number of PNs */
  uint8             u_PilotCnt;

  /* Active Pilot Set information */
  cgps_PilotType    z_Pilot[ CGPS_1X_RF_INFO_ASET_MAX_SIZE ];

  /* Serving frequency channel */
  uint16            w_Channel;

}
cgps_1xRfInfoType;


/* Maximum size of the Active Set (per IS-95 Appendex 2, Table D-2) */
#define CGPS_1X_ASET_MAX_SIZE  CAI_N6M

/* Maximum size of the (pC+C) Set.
   Note: maximum size of the real Candidates by spec (per IS-95 Appendex 2,
   Table D-2) is CAI_N7M. */
#define CGPS_1X_CSET_MAX_SIZE (CAI_N7M + 5)

/* Maximum size of the Neighbor Set (per IS-95 Appendex 2, Table D-2) */
#define CGPS_1X_NSET_MAX_SIZE  CAI_N8M

/* Struct to hold the pertinent data to pass to the GPS subsystem */
typedef struct
{
  uint16 w_PilotPN;
  uint32 q_WinSize; /* 1x search window size in Chipx8 units */
  uint8  u_Band;
  uint16 w_Channel;
}
cgps_CdmaSectType;

/* Ref PN info structure */
typedef struct
{
  uint16 w_PilotPN;
  uint8  u_Band;
  uint16 w_Channel;
}
cgps_RefPNType;

/* Active and Candidate set collection of sectors */
typedef struct
{
  uint8              u_ASetSectCnt;
  uint8              u_CSetSectCnt;
  cgps_CdmaSectType  z_ASatSectors[ CGPS_1X_ASET_MAX_SIZE ];
  cgps_CdmaSectType  z_CSatSectors[ CGPS_1X_CSET_MAX_SIZE ];
}
cgps_ACSetType;

/* The Neighbor set collection of sectors */
typedef struct
{
  uint8              u_SectCnt;
  cgps_CdmaSectType  sectors[ CGPS_1X_NSET_MAX_SIZE ];
}
cgps_NSetType;


/*****************************************************************************
 * Definitions used by API between EVDO module and C-GPS
 *****************************************************************************
*/

/* EVDO RF Info defionitions and structures */

/* Maximum size of the Active Set that would be used for reporting 1x RF Info
*/
#define CGPS_EVDO_RF_INFO_ASET_MAX_SIZE  6

typedef struct
{
  /* Tx Power */
  int16             b_TxPower;

  /* PER/FER data */
  cgps_FERType      z_PER;

  /* Received Signal Strength Indication */
  uint8             b_RSSI;

  /* Number of PNs */
  uint8             u_PilotCnt;

  /* Active Pilot Set information */
  cgps_PilotType    z_Pilot[ CGPS_EVDO_RF_INFO_ASET_MAX_SIZE ];

  /* Serving frequency channel */
  uint16            w_Channel;

}
cgps_EVDORfInfoType;


/*#include "rrcdata.h"*/  /* For rrc_state_e_type */
#include "sys_type.h" /* For ARFCN_T */

/*****************************************************************************
 * Definitions used by API between UMTS L1 and C-GPS
 *****************************************************************************
*/

/* Update types passed to LSM */
typedef enum
{
  CGPS_TIMETAG_NONE,
  CGPS_TIMETAG_REQUEST,
  CGPS_TIMETAG_PRE_CELL_CHANGE,
  CGPS_TIMETAG_POST_CELL_CHANGE,
  CGPS_TIMETAG_MAX
} cgps_CellUpdateEnumType;

/* Reference Time structure for Timetags */
typedef struct 
{
  /* Ref Time */
  boolean                  b_RefTimeValid;
  /* Ref Time Validity */
  uint64                   t_RefTime;
} cgps_RefInfoType;


/*
 * From GSM/GPRS L1 -to- LSM
*/

/* This structure provides FN, time slot and symbol count for the reference cell */
typedef struct {

   /* Frame Number 0..(26L*51L*2048L-1)  ~4.615ms frames */
   uint32     fn;

   /* Time Slot Number 0..7 */
   uint8      time_slot;

   /* QSymbol Count (sub fn) */
   uint32     q_symbol_count;

} cgps_GsmCellTimeType;


/* Cell Information <bsic,freq> */
typedef struct {

   /* Base Station Identification code */
   uint8 bsic;

   /* Control Channel Frequency */
   ARFCN_T arfcn;
} cgps_GsmCellInfoType;


/* Structure provides information specific to the cell, identity and timing */
typedef struct {

  /* Always Valid, when cell information is provided */
  cgps_GsmCellInfoType  cell_info;     /* Control Channel-Freq, and BSIC */

  /* Always Valid, when cell information is provided */
  cgps_GsmCellTimeType  cell_time;

} cgps_GsmCellTimingType;


/* Structure carries cell timing information to LSM */
typedef struct {

  /* GSM Layer 1 Main States, ie L1_NULL_MODE... */
  uint8                    l1_state;

  /* Indicates whether this message is the result of a cell change or update
     request.  */
  cgps_CellUpdateEnumType  update_type;

  /* TRUE if the time-tag is based on the sleep timer */
  boolean                  sleep_timetag;

  /* TRUE if the GPS timer is valid */
  boolean                  gps_timer_valid;

  /* 32-bit GPS timer value. */
  uint32                   gps_timer;

  /* Set FALSE if not in service or timing not available */
  boolean                  cell_valid;

  /* Cell information */
  cgps_GsmCellTimingType   cell;

  /* Subscription id */
  uint8                    subs_id;

  /* Reference info for timetag, This will be filled by GPS APIs */
  cgps_RefInfoType         ref_info;
} cgps_GsmCellTimingStructType;


/* For the time being, provide WCDMA timing on the reference cell only. */
#define CGPS_WCDMA_CELLS_TIME_INFO_MAX  1


/* Structure provides WCDMA time snapshot of SFN and PN position of a
** specific cell. */
typedef struct
{
  /* 10.3.6.75  System Frame Number  0..4095 10ms frames */
  uint16  sfn;

  /* 0..307200 Chipx8 per frame */
  uint32  pn_pos_chipx8;

} cgps_WcdmaCellTimeType;

/* Cell identified by DL frequency and primary scrambling code. */
typedef struct
{
  /* DL frequency. */
  uint16  freq;

 /* Primary scrambling code. */
  uint16  psc;

} cgps_WcdmaCellInfoType;

/* Structure provides cell identity and timing information. */
typedef struct
{
  /* Identifies cell by DL frequency and primary scrambling code. */
  cgps_WcdmaCellInfoType   cell_info;

  /* WCDMA time snapshot of SFN and PN position. */
  cgps_WcdmaCellTimeType   cell_time;

} cgps_WcdmaCellTimingType;


/* Structure used to pass data with LSM_CMD_WCDMA_CELL_INFO_UPDATE command
** from WL1 to LSM.  Provides a GPS and WCDMA time snapshot for timetag. */
typedef struct
{
  /* What state RRC thinks the phone is in. */
  uint8         ue_state;

  /* Was this message caused by a WCDMA cell change or an LSM request? */
  cgps_CellUpdateEnumType  update_type;

  /* Was this time-tag based on the sleep timer? */
  boolean                  sleep_timetag;

  /* This applies only to the PRE timetag during DRX (cell reselection).  Tell
  ** LSM how many sequential failed DRX reacquisition attempts we've had prior
  ** to this timetag.  Failed reacquisitions mean reduced timetag accuracy. */
  uint16                   num_failed_reacquisitions;

  /* If available, provides GPS timer value for timetag. */
  boolean                  gps_timer_valid;
  uint32                   gps_timer;

  /* WCDMA cell timing information provided in this message.  Reference cell
  ** data is always at offset zero. */
  uint8                    num_cells;
  cgps_WcdmaCellTimingType cells[CGPS_WCDMA_CELLS_TIME_INFO_MAX];

  /* Reference info for timetag, This will be filled by GPS APIs */
  cgps_RefInfoType         ref_info;

  /* Subscription id for the received timetag */
  sys_modem_as_id_e_type subs_id;
} cgps_WcdmaCellTimingStructType;


/* For the time being, provide TDSCDMA timing on the reference cell only. */
#define CGPS_TDSCDMA_CELLS_TIME_INFO_MAX  1


/* Structure provides TDSCDMA time snapshot of Sub Frame Number and PN position of a
** specific cell. */
typedef struct
{
  /* 64-bits value in TDS chipx8 units at status dump, clocked @ 10.24 MHz */
  uint64  q_TdsTime;

  /* 30-bits value in TDS sub chipx8 units at status dump */
  uint32  q_TdsPhase;

} cgps_TDScdmaCellTimeType;


/* Cell identified by DL frequency and cell parameters id. */
typedef struct
{
  /* DL frequency. */
  uint16  freq;

 /* Cell parameters ID. */
  uint8  cpid;

} cgps_TDScdmaCellInfoType;

/* Structure provides cell identity and timing information. */
typedef struct
{
  /* Identifies cell by DL frequency and cell parameters ID. */
  cgps_TDScdmaCellInfoType   cell_info;

  /* TDSCDMA time snapshot of SubFN and PN position. */
  cgps_TDScdmaCellTimeType   cell_time;

} cgps_TDScdmaCellTimingType;


/* Structure used to pass data with LSM_CMD_TDSCDMA_CELL_INFO_UPDATE command
** from TDSCDMA L1 to LSM.  Provides a GPS and TD-SCDMA time snapshot for timetag. */
typedef struct
{
  /* What state RRC thinks the phone is in. */
  uint8         ue_state;

  /* Was this message caused by a TD-SCDMA cell change or an LSM request? */
  cgps_CellUpdateEnumType  update_type;

  /* Was this time-tag based on the sleep timer? */
  boolean                  sleep_timetag;

  /* This applies only to the PRE timetag during DRX (cell reselection).  Tell
  ** LSM how many sequential failed DRX reacquisition attempts we've had prior
  ** to this timetag.  Failed reacquisitions mean reduced timetag accuracy. */
  uint16                   num_failed_reacquisitions;

  /* If available, provides GPS timer value for timetag. */
  boolean                  gps_timer_valid;
  uint32                   gps_timer;
  uint32                   lpm_count;

  /* Raw 32-bit Universal STMR value at status dump during time tag */
  uint32  ustmr_count;

  /* TDSCDMA cell timing information provided in this message.  Reference cell
  ** data is always at offset zero. */
  uint8                    num_cells;
  cgps_TDScdmaCellTimingType cells[CGPS_TDSCDMA_CELLS_TIME_INFO_MAX];

  /* Reference info for timetag, This will be filled by GPS APIs */
  cgps_RefInfoType         ref_info;

} cgps_TDScdmaCellTimingStructType;


/* For the time being, provide LTE timing on the reference cell only. */
#define CGPS_LTE_CELLS_TIME_INFO_MAX  1


/* Structure provides LTE time snapshot of a specific cell. */
typedef struct
{
  /* 64-bits, no rollover, chipx8 units, clocked @ 30.72MHz */
  uint64  lte_rtc;

} cgps_LteCellTimeType;

/* Cell identified by DL frequency and cell_id */
typedef struct
{
  /* DL frequency */ 
  lte_earfcn_t  freq;

 /* Cell ID */
  uint16  cell_id;

} cgps_LteCellInfoType;

/* Structure provides cell identity and timing information. */
typedef struct
{
  /* Identifies cell by DL frequency and primary scrambling code. */
  cgps_LteCellInfoType   cell_info;

  /* LTE time snapshot*/
  cgps_LteCellTimeType   cell_time;

} cgps_LteCellTimingType;

/* Structure used to pass time-tag info from ML1 to GNSS.
   Provides a GPS and WCDMA time snapshot for timetag. */
typedef struct
{
  /* What state ML1 thinks the phone is in. */
  uint8         ue_state;

  /* Was this message caused by a LTE cell change or a GNSS request? */
  cgps_CellUpdateEnumType  update_type;

  /* Was this time-tag based on the sleep timer? */
  boolean                  sleep_timetag;

  /* This applies only to the PRE timetag during DRX (cell reselection).  Tell
  ** GNSS how many sequential failed DRX reacquisition attempts we've had prior
  ** to this timetag.  Failed reacquisitions mean reduced timetag accuracy. */
  uint16                   num_failed_reacquisitions;

  /* If available, provides GPS timer value for timetag. */
  boolean                  gps_timer_valid;
  uint32                   gps_timer;

  /* LTE cell timing information provided in this message.  Reference cell
  ** data is always at offset zero. */
  uint8                    num_cells;
  cgps_LteCellTimingType cells[CGPS_LTE_CELLS_TIME_INFO_MAX];

  /* USTMR count. Resolution of 19.2 MHz */
  uint32         ustmr_count;

  /* Reference info for timetag, This will be filled by GPS APIs */
  cgps_RefInfoType         ref_info;  
} cgps_LteCellTimingStructType;

typedef struct {
  struct {
    unsigned cellGlobalIdPresent                : 1; /* 1=present, 0=not present */
    unsigned systemFrameNumberPresent           : 1; /* 1=present, 0=not present */
    unsigned rsrp_ResultPresent                 : 1; /* 1=present, 0=not present */
    unsigned rsrq_ResultPresent                 : 1; /* 1=present, 0=not present */
    unsigned ue_RxTxTimeDiffPresent             : 1; /* 1=present, 0=not present */
  } m; /* BitMask indicating presence of data */

  uint16 w_physCellId;        /* phys cell ID */

  /* Global Cell ID data */
  uint8 mcc[3];               /* MCC always 3 digits */
  uint8 mnc[3];               /* MNC  can be less than 3 digits */
  uint8 num_mnc_digits;       /* number of MNC digits */

  uint32 cell_id_num_bits;    /* number of bits in the cell id */
  uint32 cellIdEutra;         /* Global cell ID */

  /* Measurement data */
  uint16 arfcnEUTRA;          /* EUTRA arfcn */
  uint32  systemFrameNumber;  /* frame number */
  uint8  rsrp_Result;         /* rsrp result */
  uint8  rsrq_Result;         /* rsrq result */
  uint16 ue_RxTxTimeDiff;     /* rxtx time diff */

} cgps_LteRxtxCellMeasType;

typedef enum{
  CGPS_MSGR_LOC_WWAN_LTE_RXTX,
  CGPS_MSGR_MSG_MAX

}cgps_LocWwanMsgType;

typedef struct{

  #ifdef FEATURE_GNSS_LEAN_BUILD
  /* For the lean-build, we use an ARMCC compiler, which is having compilation
     issues with the union below. Since there is only one element, I declared
     it directly here. This structure will not be used in Lean Build
  */
  uint32 lte_rxtx_status;
  #else
  union{
    uint32 lte_rxtx_status;
  };
  #endif

}cgps_LocWwanMsgDataType;

typedef struct {
  struct {
    unsigned servingCellMeasuredResultsPresent  : 1; /* 1=present, 0=not present */
  } m; /* BitMask indicating presence of data */

  cgps_LteRxtxCellMeasType servingCellMeasuredResults;
  uint32 n; /* number of neighbor cell measurements */
  cgps_LteRxtxCellMeasType neighborCellMeasuredResults[CGPS_LTE_RXTX_MAX_RESULTS];

} cgps_LteRxtxMeasType;

typedef enum {
  CGPS_ENTER_SARF_MODE = 0x00,
  CGPS_EXIT_SARF_MODE  = 0x01,
  CGPS_SARF_MODE_MAX,
} cgps_sarf_mode_switch_enum_type;


typedef enum 
{
  CGPS_XO_CAL_PASS = 0x01, /* Success: only 1 tone is found in valid ranges (both C/No and Freq ranges). */
  CGPS_XO_CAL_NO_TONE = 0x02, /* Fail because No tone is found in valid ranges. */
  CGPS_XO_CAL_MULTI_TONES = 0x04, /* Fail because 2 or more tones are found in valid ranges. */
  CGPS_XO_CAL_JAMMER_OUTSIDE_RANGE = 0x08, /* Fail because extremely strong jammer outside of valid freq range */
  CGPS_XO_CAL_DATA_COLLECT_TIMEOUT = 0x10, /* Fail because of IQ data collection timeout. */
  CGPS_XO_CAL_OTHER_ERROR = 0x20, /* Fail due to MISC error. */
} cgps_xocal_resp_enum_type;

typedef struct
{
  cgps_xocal_resp_enum_type    e_Status;/* PASS if valid peak is found, else send the error type. */
  int32                l_Freq; /* Freq error in hrppb */
  uint32               q_Cno;  /* CNo in 10s of dBHz */
} cgps_xocal_results_type;

typedef struct
{
  int32 r ;
  int32 i ;
}cgps_CplxS32_Ftm_type;

#define  CGPS_IQ_SAMPLE_CNT_FTM  512

typedef void (*cgps_mode_switch_callback)
(
  /* TRUE if switch suceeded */
  boolean               success
);

typedef void (cgps_SARF_Cb)
(
  /* TRUE if IQ samples are valid, FALSE otherwise */
  boolean               success
);

typedef void (cgps_XOCAL_Cb)
(
  /* PASS if valid peak is found, else send the error type. */
  cgps_xocal_resp_enum_type    e_Status,

  /* Freq error in hrppb */
  int32                l_Freq  
);

typedef void (cgps_XOCAL_results_Cb)
(
  cgps_xocal_results_type    z_XocalResults
);

/*****************************************************************************
 * Definitions used by API between LTE module and C-GPS
 *****************************************************************************
*/

#define CGPS_MAX_LTE_TT_SAMPLES_PER_REPORT  8

typedef enum {
  CGPS_LTE_TT_RPT_STATUS_VALID,
  CGPS_LTE_TT_RPT_STATUS_NO_SIB8,
  CGPS_LTE_TT_RPT_STATUS_ABORTED,
  CGPS_LTE_TT_RPT_STATUS_ERROR
}
cgps_LteTTRptStatusType;

/* LTE timing advance structure */
typedef struct
{
  /* Validity of timing advance data */
  boolean b_TaValid;

  /* Timing advance (Rx-Tx diff) in Ts units */
  int16 x_TaData;
  
  /* Serving cell physical ID. Range ~ 0 to 503 */
  uint16 w_ServPhyCellId;
}
cgps_TaDataType;

/* LTE Time Transfer sets of O_STMR+GNSS RTCs latched at the same time structure */
typedef struct
{
  /* O_STMR count value (in CDMA Chipx8) */
  uint32  q_OStmr;

  /* O_STMR phase value (in CDMA Chipx2^35) */
  uint32  q_OStmrPhase;

  /* GNSS RTC value (in GPS Chipx80) */
  uint32  q_GnssRtc;

  /* GNSS Phase value (in sub-GPS Chipx80) */
  uint32  q_GnssPhase;

  /* GNSS Sample Count status */
  uint32  q_SampleCount;  
}
cgps_LteTTLatchedDataType;

/* LTE Time Transfer Report data Structure */
typedef struct
{
  /* Request ID of the LTE Time Transfer request that triggered this report */
  uint8   u_ReqID;

  /* Report status */
  cgps_LteTTRptStatusType e_RptStatus;

  /* Physical cell ID of the LTE serving cell [Range ~ 0 to 503] */
  uint16  w_PhyCellId;

  /* This field specifies the E-UTRA carrier frequency of the LTE serving cell */
  lte_earfcn_t  w_Earfcn;

  /* SIB8 "cdma-EUTRA-Synchronisation" info:
     TRUE if LTE NW is synchronized. Otherwise FALSE. */
  boolean b_Sib8CdmaSynchronisation;

  /* The CDMA2000 system time corresponding to the SFN
                              boundary at or after the ending boundary of the
     SI-Window in which this Time Transfer was taken. The size
     is 52 bits and the unit is [CDMA2000 chips]
  */
  uint64  t_Sib8CdmaSystemTime;

  /* SIB16 System Time info*/
  boolean b_Sib16Valid;

  /* The Coordinated universal system time corresponding to the SFN
                              boundary at or after the ending boundary of the
     SI-Window in which this Time Transfer was taken. The size
     is 34 bits and the unit is [1 ms]
  */
  uint64  t_Sib16GPSTime;

  /* Number of leap seconds between UTC time and GPS time */
  uint16  w_LeapSeconds;

  /* LTE SFN system time of the subframe in which TTr was acquired
     in the unit of subframe */
  uint16  w_Sfn;

  /* O-STMR value at the boundary of the subframe in which TTr was acquired */
  uint32  q_OStmrSfBoundary;

  /* Best estimate of the current LTE system time uncertainty */
  uint32  q_SystemTimeUncertaintyEstimate;

  /* Current value of the slow clock counter */
  uint32  q_SclkTime;

  /* LPM RTC value latched at the same time as the first latched O_STMR/GNSS RTC
     set (in 19.2MHz untis) */
  uint32  q_LpmRtcLatched;

  /* Universal STMR value latched at the same time as the first latched
     O_STMR/GNSS RTC set (in 19.2MHz untis) */
  uint32  q_UnivStmrLatched;

  /* O_STMR control register read at the same time as the first latched
     O_STMR/GNSS RTC set (in Ts/2^30 units) */
  uint32  q_OStmrFcw;

  /* Mark if the next two fields are valid or not */
  boolean b_OstmrVsrcValid;

  /* OSTMR From FW */
  uint32 q_OstmrFromFW[2];

  /* VSRC From FW */
  uint32 q_VsrcFromFW[2];

  /* VSRC From FW */
  uint32 q_VsrcCurrT2BYT1M1[3];

  /* Difference between O_STMR and VSRC STMR (in TBD units) */
  uint32  q_OStmrVsrcStmrDiff;

  /* Number of sets of latched RTCs/STMRs per LTE Time Transfer Data report.
     u_NumTTSamples should be <= CGPS_MAX_LTE_TT_SAMPLES_PER_REPORT
     u_NumTTSamples == 0 presents error condition
   */
  uint8   u_NumTTSamples;

  /* Timing advance data to predict SIB8 TTr uncertainity */
  cgps_TaDataType z_TaData;

  /* Sets of O_STMR + GNSS RTCs latched at the same time */
  cgps_LteTTLatchedDataType z_LatchedData[ CGPS_MAX_LTE_TT_SAMPLES_PER_REPORT ];

}
cgps_LteTTRptType;


/*
 *****************************************************************************
 * Definitions used by API between gps restriction and LCU/Camera
 *****************************************************************************
*/

typedef enum
{
  CGPS_RSTRCT_LEVEL_NONE,             /* No gps restrictions */
  CGPS_RSTRCT_LEVEL_BUS,              /* Restrict GPS from using the bus */

  CGPS_RSTRCT_LEVEL_LAST_PLUS_1       /* Marks invalid restrictions. THIS MUST
                                         REMAIN LAST IN THE LIST */
} cgps_rstrctLevelType;

typedef uint32 CGPS_RSTRCT_ID;


typedef struct
{
  void (* cgps_rstrctLevel)(const cgps_rstrctLevelType);
  void (* cgps_rstrctRequest)(const cgps_rstrctLevelType);
} cgps_rstrctHandlerType;

/*****************************************************************************
 * API functions for C-GPS capabilities
 *****************************************************************************
*/

/*
 ******************************************************************************
 * Function cgps_XOSupported
 *
 * Description:
 *
 *  This function is a peek on C-GPS capability for XO Support
 *
 * Parameters:
 *
 *  None.
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if XO is supported
 *  FALSE if XO is not supported and VCTCXO is supported.
 *
 ******************************************************************************
*/
extern boolean cgps_XoSupported( void );


/*****************************************************************************
 * API functions between RF Drivers and C-GPS
 *****************************************************************************
*/

/*
 ******************************************************************************
 * Function cgps_RfDesignIfOffsetPut
 *
 * Description:
 *
 *  This function is called by the RF driver to supply the current Design IF
 *  Offset. The function shall be dispatched whenever the GPS RF is switched
 *  on or when the GPS PLL is reprogramed.
 *
 * Parameters:
 *
 *  Offset in Hz.
 *  Note that this is the theoretical offset that would occur when dF/F = 0.
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfDesignIfOffsetPut( double );

/*
 ******************************************************************************
 * Function cgps_RfBcConfigPut
 *
 * Description:
 *
 *  Sets the static controls that are specific to a given GPS RF chain.
 *  Function must be called as part of GPS RF turn on processing.
 *
 * Parameters:
 *
 *  Use the same bit field format specified in BC_CONFIG register definition:
 *    BC_LPFLENGTH - First LPF selection (8/16 MHz)
 *    BC_GAINSTEP  - 6dB gain steps
 *    BC_BLANKEN   - Blank enable / disable
 *    BC_BLANKPOL  - HW blank polarity
 *    BC_QUANTSEL  - Quantization format - ADC order
 *    BC_QUANTINV  - Quantization format - MSB inversion
 *    BC_QUANTLEV  - Quantization format - ADC type
 *    BC_RATE      - Sample rate (8/16 MHz)
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfBcConfigPut( cgps_BcConfigType * );

/*
 ******************************************************************************
 * Function cgps_RfBcConfig2Put
 *
 * Description:
 *
 *  Sets the static controls that are specific to a given GPS RF chain.
 *  Function must be called as part of GPS RF turn on processing.
 *
 * Parameters:
 *
 *  Use the same bit field format specified in BC_CONFIG2 register definition.
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfBcConfig2Put( cgps_BcConfig2Type * );

/*
 ******************************************************************************
 * Function cgps_RfBcEqCoefPut
 *
 * Description:
 *
 *  Sets the baseband converter equalizer settings
 *
 * Parameters:
 *
 *  Use the same bit field format specified in BC_EQCOEF register definitions.
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfBcEqCoefPut( cgps_BcEqCoefType * );

/*
 ******************************************************************************
 * Function cgps_RfBcNFramePut
 *
 * Description:
 *
 *  Sets the BC_NFRAME value
 *
 * Parameters:
 *
 *  Use the same bit field format specified in the BC_MODE register
 *  definition
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfBcNFramePut( uint8 u_Val );

/*
 ******************************************************************************
 * Function cgps_RfRpMeanMonGet
 *
 * Description:
 *
 *  Retrieves the mean estimates from the resampler processor
 *
 * Parameters:
 *
 *  Pointer to output structure
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfRpMeanMonGet( cgps_RfRpMeanMonType *);

/*
 ******************************************************************************
 * Function cgps_RfRpConfigPut
 *
 * Description:
 *
 *  Sets configuration parameters for the resampler processor
 *
 * Parameters:
 *
 *  Pointer to configuration structure
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfRpConfigPut( cgps_RfRpConfigType *);

/*
 ******************************************************************************
 * Function cgps_RfRpCarrierPhasePut
 *
 * Description:
 *
 *  Programs the resampler processor to compensate for phase shift
 *
 * Parameters:
 *
 *  Phase shift in +- degrees
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfRpCarrierPhasePut( float f_PhaseShift );

/*
 ******************************************************************************
 * Function cgps_RfRpResamplerMapPut
 *
 * Description:
 *
 *  Sets the resampler processor high / low phase maps
 *
 * Parameters:
 *
 *  Phase map high / low coefficients and floor count
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfRpResamplerMapPut( cgps_RfRpResamplerMapType * );

/*
 ******************************************************************************
 * Function cgps_RfRpFreqPut
 *
 * Description:
 *
 *  Sets the resampler processor coarse rotator downconversion
 *  frequency
 *
 * Parameters:
 *
 *  Rotator frequency
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfRpFreqPut( float f_Freq );

/*
 ******************************************************************************
 * Function cgps_RfRpGroupDelayPut
 *
 * Description:
 *
 *  Sets the RF group delay compensation value in the resampler
 *  processor
 *
 * Parameters:
 *
 *  Group delay value in nanoseconds
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfRpGroupDelayPut( float f_GroupDelayNs );

/*
 ******************************************************************************
 * Function cgps_RfSwBlankPut
 *
 * Description:
 *
 *  Starts or stops the SW blank function in the GPS receiver. The RF driver
 *  shall issue a start blank followed by a stop blank to bracket a time
 *  period in which undesired signals might be presented to the GPS receive
 *  path.
 *  Note that the GPS RF shall force the blank function to stop when the
 *  GPS RF is being turned off.
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfSwBlankPut( cgps_RfSwBlankCmdEnum );

/*
 ******************************************************************************
 * Function cgps_RfIQControlPut
 *
 * Description:
 *
 *  This function commands the GPS receiver to perform supplementary
 *  modifications to IQ. These modifications allow coarse phase shifts
 *  (quanta of 90 degrees) or spectral inversion to be commanded by the
 *  RF driver.
 *  Note that the GPS RF shall initialize the IQ control when the GPS RF is
 *  being turned on.
 *
 * Parameters:
 *
 *  u_SwapIQ:  TRUE = Swap IQ
 *  u_InvertQ: TRUE = Invert Q
 *  u_InvertI: TRUE = Invert I
 *
 * Dependencies:
 *
 *  Operations to shared variables and registers are performed using an
 *  interrupt lock method.
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfIQControlPut( cgps_IQControlType * );

/*
 ******************************************************************************
 * Function cgps_RfDcGainGet
 *
 * Description:
 *
 *  This function samples the current mean (DC) and amp (Gain) offsets being
 *  measured by the GPS receiver. The RF driver shall use these measurements
 *  to perform initialization coarse tuning of any RF/ADC controls.
 *  Note that these reports are derived from measurements accumulated over
 *  the previous 1 ms frame.
 *
 * Parameters:
 *
 *  Pointer to output structure
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Output structure contains measured DC and Gain offsets, current GPS RTC
 *  value, and info if blanikng is currently turned on.
 *
 ******************************************************************************
*/
extern void cgps_RfDcGainGet( cgps_RfDcGainType * );

/*
 ******************************************************************************
 * Function cgps_RfAccumulatedDcGainGet
 *
 * Description:
 *
 *  This function returns the accumulated DC offset since the last time this
 * function was called. The DC offset is accumulated at a 50Hz rate. The
 * function returns the accumulated offset and number of samples
 * accumulated. The accumulator is cleared after the values are
 * returned.
 *
 *  The accumulated offset can be used to derive an average mean over a
 *  period of time.
 *
 * Parameters:
 *
 *  Pointer to output structure
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Output structure contains measured accumulated DC offset and number of
 *  samples accumulated.
 *
 ******************************************************************************
*/
extern void cgps_RfAccumulatedDcMeanGet( cgps_RfAccumulatedDcMeanType * );

/*
 ******************************************************************************
 * Function cgps_RfDcOffsetPut
 *
 * Description:
 *
 *  This function writes to the DC loop control registers. Its intended to
 *  restore a previously saved DC compensation value. Was introduced for
 *  for SHDR feature. RF drivers will typically call this.
 *
 * Parameters:
 *
 *  Pointer to I/Q DC compensation structure
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
void cgps_RfDcOffsetPut( cgps_RfDcType * p_DcOffset );


/*
 ******************************************************************************
 * Function cgps_RfLinearityPut
 *
 * Description:
 *
 *  This function allows the RF driver to report the RF linearity setting
 *  to the GPS receiver. The GPS receiver uses this information in diagnostic
 *  logs. Additionally this function also passes carrier phase offset
 *  information that allows the carrier phase discontinuity to be removed
 *  from the data demodulation coherent sums. (The approprate GPS RTC time
 *  reference information for this correction is derived within this function).
 *
 *  The RF driver should dispatch this function whenever the GPS RF linearity
 *  is programmed. (ie - At RF initialization and when the linearity is
 *  switched).
 *
 * Parameters:
 *
 *  Pointer to cgps_RfLinearityType that captures new linearity setting and
 *  revised carrier phase offset.
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
extern void cgps_RfLinearityPut( const cgps_RfLinearityType * );

/*
 ******************************************************************************
 * Function cgps_RfSynthModifyPut
 *
 * Description:
 *
 *  This function allows the RF driver to report the RF synthesizer
 *  modification events to the GPS engine. The problems encountered with
 *  the RTR6285 and Ramsis RF solutions will likely require that the RF Driver
 *  reprogram the GPS VCO to address. A side effect of programming the GPS
 *  synthesizer is that a 180 degree carrier phase ambiguity may be imposed.
 *  The GPS SW can takes steps to accommodate the carrier phase ambiguity, but
 *  needs to know the GPS RTC at the moment of modification.
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
 *  None
 *
 ******************************************************************************
*/
extern void cgps_RfSynthModifyPut( void );

/*
 ******************************************************************************
 * Function cgps_RfGroupDelayPut
 *
 * Description:
 *   This function allows the RF driver to indicate group delay
 *   variations to the GPS core. The group delay specified is
 *   the delta from nominal.
 *
 * Parameters:
 *
 *  l_GroupDelayns - Group Delay in nanoseconds
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
extern void cgps_RfGroupDelayPut( int32 l_GroupDelayns );

/*
 ******************************************************************************
 * Function cgps_RfBcAuxConfigPut
 *
 * Description:
 *   This function allows the RF driver to modify the
 *   Auxilliary baseband converter register
 *
 * Parameters:
 *
 * p_Config - Pointer to BC Aux config structure
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
extern void cgps_RfBcAuxConfigPut( cgps_RfBcAuxConfigType *p_Config );

/*
 ******************************************************************************
 * Function cgps_RfBcAuxNotchConfigPut
 *
 * Description:
 *   This function allows the RF driver to modify the
 *   Auxilliary baseband converter notch filger
 *   registers
 *
 * Parameters:
 *
 * p_Config - Pointer to BC Aux notch config structure
 * u_NotchIndex - Selects which notch filter to configure
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
extern void cgps_RfBcAuxNotchConfigPut( cgps_RfBcAuxNotchConfigType *p_Config, uint8 u_NotchIndex );

/*
 ******************************************************************************
 * Function cgps_RfBcAuxMeanStatusGet
 *
 * Description:
 *   This function allows the RF driver to retrieve the
 *   Auxilliary baseband converter mean status register
 *   values
 *
 * Parameters:
 *
 * p_Status - Pointer to structure in which to store the
 * mean status register values
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
extern void cgps_RfBcAuxMeanStatusGet( cgps_RfBcAuxMeanStatusType *p_Status );

/*****************************************************************************
 * API functions between 1x SRCH and C-GPS
 *****************************************************************************
*/

/*
 ******************************************************************************
 * Function srch_ppm_init
 *
 * Description:
 *
 *  This function is a good place to initialize all variables,
 *  structures, arrays, etc that are related to SRCHPPM that
 *  need to have some initial values. This function should ONLY
 *  be called ONCE at power up.
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
 *  None
 *
 ******************************************************************************
*/
extern void srch_ppm_init( void );

/*
 ******************************************************************************
 * Function srch_ppm_srch4_active
 *
 * Description:
 *
 *  This function returns if PPM SRCH4 is active or not
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
 *  None
 *
 ******************************************************************************
*/
extern boolean srch_ppm_srch4_active( void );

/*=========================================================================
FUNCTION: SRCH_PPM_1X_ACTIVE

DESCRIPTION : Return if 1x service is active according to PPM

DEPENDENCIES :   None.

RETURN VALUE :   Status of 1x service in PPM.

SIDE EFFECTS :   None.

==========================================================================*/
extern boolean srch_ppm_1x_active( void );

/*===========================================================================

FUNCTION SRCH_GPS_HANDLER

DESCRIPTION    This function is called by stm_driver as its driver_f function.

DEPENDENCIES   None

RETURN VALUE   TRUE if an event was processed, FALSE if not

SIDE EFFECTS   None

===========================================================================*/

extern boolean srch_gps_handler ( void );


/*===========================================================================

FUNCTION SRCH_GPS_SIGS

DESCRIPTION  This function is called by stm_driver as its driver_f function.

DEPENDENCIES   None

RETURN VALUE   TRUE if an event was processed, FALSE if not

SIDE EFFECTS   None

===========================================================================*/

extern rex_sigs_type srch_gps_sigs ( void );


/*
 ******************************************************************************
 * Function cgps_1xPPMOperationGranted
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to inform PPM module that it
 *  can start PPM searching.
 *
 * Parameters:
 *
 *  1x serving frequency band class and channel, current 1x Rx Agc.
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
extern void cgps_1xPPMOperationGranted( const cgps_1xPPMOpGrantDataType * );

/*
 ******************************************************************************
 * Function cgps_1xPPMOperationNotGranted
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to inform PPM module that 1x
 *  modem is not demodulating incoming 1x signal on the serving frequency
 *  (for any reason, such as: modem goes to sleep, looses the system, etc.).
 *  In response to this, PPM module should immediately stop any PPM searching
 *  and discard all search results obtained after this instance.
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
 *  None
 *
 ******************************************************************************
*/
extern void cgps_1xPPMOperationNotGranted( void );

/*
 ******************************************************************************
 * Function cgps_1xPpmOperationStops
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to inform PPM module that 1x
 *  modem is not demodulating incoming 1x signal on the serving frequency
 *  (for any reason, such as: modem goes to sleep, looses the system, etc.).
 *  In response to this, PPM module should immediately stop any PPM searching
 *  and discard all search results obtained after this instance.
 *
 * Parameters:
 *
 *  Information if 1x clocks/RTC would be interrupted after this command
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
extern void cgps_1xPpmOperationStops( const cgps_1xPpmOpStopsDataType * );

/*
 ******************************************************************************
 * Function cgps_1xSendSync80Data
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass Sync80 Data packet to
 *  PPM module.
 *
 * Parameters:
 *
 *  Sync80 Data structure.
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
extern void cgps_1xSendSync80Data( const cgps_1xSync80DataType * );

/*
 ******************************************************************************
 * Function cgps_1xSendUnsolicitedNeighborListUpdate
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass Neighbor List Data packet
 *  to PPM module.
 *
 * Parameters:
 *
 *  Neighbor List Data structure.
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
extern void cgps_1xSendUnsolicitedNeighborListUpdate( const cgps_NSetType * );

/*
 ******************************************************************************
 * Function cgps_1xSendSrchWindowCenter
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass Search Window Center
 *  to PPM module.
 *
 * Parameters:
 *
 *  Search Window Center in Chipx8 units.
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
extern void cgps_1xSendSrchWindowCenter( uint32 );

/*
 ******************************************************************************
 * Function cgps_1xSendRefPN
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass Reference PN
 *  to PPM module.
 *
 * Parameters:
 *
 *  Reference PN offset and serving frequency.
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
extern void cgps_1xSendRefPN( const cgps_RefPNType * );

/*
 ******************************************************************************
 * Function cgps_1xSendActiveCandidateSet
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass Active and Candidate sets
 *  Data packet to PPM module.
 *
 * Parameters:
 *
 *  Active and Candidate sets structure.
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
extern void cgps_1xSendActiveCandidateSet( const cgps_ACSetType * );

/*
 ******************************************************************************
 * Function cgps_1xSendSystemTimeUncertaintyEstimate
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass System Time Uncertainty
 *  Estimate to PGI module.
 *
 * Parameters:
 *
 *  System Time Uncertainty Estimate in microseconds units.
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
extern void cgps_1xSendSystemTimeUncertaintyEstimate( uint32 );

/*
 ******************************************************************************
 * Function cgps_1xDisableTTDataRecordingAck
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to inform PGI module that Time
 *  Transfer Data Recording is disabled.
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
 *  None
 *
 ******************************************************************************
*/
extern void cgps_1xDisableTTDataRecordingAck( void );

/*
 ******************************************************************************
 * Function cgps_1xSendTTData
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass Time Transfer Data packet
 *  to PGI module.
 *
 * Parameters:
 *
 *  Time Transfer Data structure.
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
extern void cgps_1xSendTTData ( const cgps_1xTTDataType * );


/*
 ******************************************************************************
 * Function cgps_1xSendSclkTTData
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to provide a Slow Clock Time
 *  Transfer data packet to the PGI module.
 *
 *
 * Parameters:
 *
 *  Data structure which contains information with which Time can be transferred
 *  to the Slow Clock.
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
extern void cgps_1xSendSclkTTData( const cgps_1xSclkTTDataType *p_SclkTTData );

/*
 ******************************************************************************
 * Function cgps_1xTTOperationStops
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass information that the Time
 *  Transfer operation will be temproarily stopped (i.e. 1x clocks and RTC
 *  could stop) after this message.
 *  This message is sent to PGI module.
 *
 * Parameters:
 *
 *  Information if 1x clocks/RTC would be interrupted after this command
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
extern void cgps_1xTTOperationStops( const cgps_1xTTOpStopsDataType * );

/*
 ******************************************************************************
 * Function cgps_1xSendRfInfo
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to pass RF Info packet
 *  to PGI module.
 *
 * Parameters:
 *
 *  RF Info structure.
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
extern void cgps_1xSendRfInfo( const cgps_1xRfInfoType * );

/*
 ******************************************************************************
 * Function cgps_1xGPSAllowedAck
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to inform PGI module that GPS
 *  searching can start now.
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
 *  None
 *
 ******************************************************************************
*/
extern void cgps_1xGPSAllowedAck( void );


/*
 ******************************************************************************
 * Function cgps_EVDOSendRfInfo
 *
 * Description:
 *
 *  This function is called by EVDO module to pass RF Info packet
 *  to PGI module.
 *
 * Parameters:
 *
 *  RF Info structure.
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
extern void cgps_EVDOSendRfInfo( const cgps_EVDORfInfoType * );


/*
 ******************************************************************************
 * Function cgps_EVDOGPSAllowedAck
 *
 * Description:
 *
 *  This function is called by EVDO SRCH task to inform PGI module that GPS
 *  searching can start now.
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
 *  None
 *
 ******************************************************************************
*/
extern void cgps_EVDOGPSAllowedAck( void );

typedef struct
{
  /* Reference Oscillator Offset Update Timestamp */
  timetick_type      z_RefOscUpdateTimestamp;

  /* Reference Oscillator Offset */
  int32              l_RefOscOffset;

  /* Reference Oscillator Offset Rate */
  int32              l_RefOscOffsetRate;
} cgps_RefOscOffsetType;

/*
 ******************************************************************************
 * Function cgps_EstimatedReferenceOscillatorOffsetPut
 *
 * Description:
 *
 *  This function is called to provide CGPS with the pertinent reference
 *  oscillator offset parameters such as drift and drift rate
 *
 * Parameters:
 *
 * p_Offset - Pointer to Reference Oscillator Offset Structure
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
void cgps_EstimatedReferenceOscillatorOffsetPut( cgps_RefOscOffsetType *p_Offset );

/*
 ******************************************************************************
 * Function cgps_ClkRegimeFreqOffsetPpt
 *
 * Description:
 *
 *  This function calls the appropriate clk fuction for QSC targets,
 *  empty function otherwise.
 *
 * Parameters:
 *
 *  l_Offset - Freq Offset in ppt ( parts per trillion )
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
void cgps_ClkRegimeFreqOffsetPpt(int32 l_Offset);

/*****************************************************************************
 * API functions between UMTS L1 and C-GPS
 *****************************************************************************
*/

/*
 ******************************************************************************
 * Function cgps_StartGpsTimer
 *
 * Description:
 *
 *  This function is used by UMTS L1 to start the GPS Clocks and the GPSRTC.
 *  This is necessary because the Time-tag can be performed when GPS is
 *  in-active (as in Pre-Cell change and Post-Cell change time-tags). The
 *  clocks will be ON till GPS receives the CellInfoUpdate message. If
 *  the Time-tag is for Pre Cell change, clocks are kept ON till the
 *  Post Cell change message is received (or till a timeout occurs).
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
 *  None
 *
 ******************************************************************************
*/
void cgps_StartGpsTimer( void );

/*
 ******************************************************************************
 * Function cgps_LatchGpsTimer
 *
 * Description:
 *
 *  This function latches the GPSRTC and returns the latched value. To perform
 *  a Time-tag, UMTS L1 shall latch both the Network Timers, and GPS RTC
 *  from an interrupt locked context to obtain a dependable association.
 *  The latched values are sent to GPS in the CellInfoUpdate message.
 *  Return value of this function indicates the validity of GPSRTC latched value
 *
 * Parameters:
 *
 *  p_GpsTimer - Pointer to the location where GPSRTC is stored
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if GPSRTC latched value is valid.
 *
 ******************************************************************************
*/
boolean cgps_LatchGpsTimer( uint32 *p_GpsTimer );

/*
 ******************************************************************************
 * Function cgps_WL1_LatchGpsTimer
 *
 * Description:
 *
 *  This function is used by UMTS L1 to latch GPS time. The latched GPS time
 *  is stored and later retrieved in cgps_WL1_GetGpsTimer. This change was done
 *  when MDM9K started to see 17us delay between latch and Getgpstime calls
 *  from WL1.
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
 *  None
 *
 ******************************************************************************
*/
void cgps_WL1_LatchGpsTimer( void );

/*
 ******************************************************************************
 * Function cgps_WL1_GetGpsTimer
 *
 * Description:
 *
 *  This function is used by UMTS L1 to get the latched GPS time. The latched
 *  GPS time is returned along with the validity.
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
 *  TRUE/FALSE - Latched GPS time is valid/invalid
 *
 ******************************************************************************
*/
boolean cgps_WL1_GetGpsTimer( uint32 *p_GpsTimer );

/*
 ******************************************************************************
 * Function cgps_GsmCellInfoUpdate
 *
 * Description:
 *
 *  This function sends the Time-tag information to GPS module. UMTS L1 performs
 *  Time-tags based on request from GPS or when Cell/RAT change happens.
 *  Time-tag contains latched Network time and GPS RTC. It also contains
 *  locally unique cell identifiers (for the active cell from which network
 *  time was derived).
 *
 * Parameters:
 *
 *  p_TimeTag - Pointer to the Time-Tag structure
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if the message could be sent successfully to CGPS.
 *
 ******************************************************************************
*/
boolean cgps_GsmCellInfoUpdate( cgps_GsmCellTimingStructType *p_TimeTag );

/*
 ******************************************************************************
 * Function cgps_GetRefInfo
 *
 * Description:
 *
 *  This function gets the Ref Info. This includes System timer @ any instant. 
 *  WWAN L1 shall latch both the Network Timers, and Ref Time from an interrupt 
 *  locked context to obtain a dependable association. The latched values are 
 *  sent to GPS in the CellInfoUpdate message. Return value of this function 
 *  indicates the validity of Ref Time
 *
 * Parameters: 
 *
 *  p_RefInfo - Pointer to the location where Reference Info is stored
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
void cgps_GetRefInfo( cgps_RefInfoType *p_RefInfo );

/*
 ******************************************************************************
 * Function cgps_WcdmaCellInfoUpdate
 *
 * Description:
 *
 *  This function sends the Time-tag information to GPS module. UMTS L1 performs
 *  Time-tags based on request from GPS or when Cell/RAT change happens.
 *  Time-tag contains latched Network time and GPS RTC. It also contains
 *  locally unique cell identifiers (for the active cell from which network
 *  time was derived).
 *
 * Parameters:
 *
 *  p_TimeTag - Pointer to the Time-Tag structure
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if the message could be sent successfully to CGPS.
 *
 ******************************************************************************
*/
boolean cgps_WcdmaCellInfoUpdate( cgps_WcdmaCellTimingStructType *p_TimeTag );

/*
 ******************************************************************************
 * Function cgps_LteCellInfoUpdate
 *
 * Description:
 *
 *  This function sends the Time-tag information to GPS module. LTE L1 performs
 *  Time-tags based on request from GPS or when Cell/RAT change happens.
 *  Time-tag contains latched Network time and GPS RTC. It also contains
 *  locally unique cell identifiers (for the active cell from which network
 *  time was derived).
 *
 * Parameters:
 *
 *  p_TimeTag - Pointer to the Time-Tag structure
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if the message could be sent successfully to GNSS.
 *
 ******************************************************************************
*/
boolean cgps_LteCellInfoUpdate( cgps_LteCellTimingStructType *p_TimeTag );

/*
 ******************************************************************************
 * Function cgps_TDScdmaCellInfoUpdate
 *
 * Description:
 *
 *  This function sends the Time-tag information to GPS module.
 *
 * Parameters:
 *
 *  p_TimeTag - Pointer to the Time-Tag structure
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if the message could be sent successfully to CGPS.
 *
 ******************************************************************************
*/
boolean cgps_TDScdmaCellInfoUpdate( cgps_TDScdmaCellTimingStructType *p_TimeTag );


/*
 ******************************************************************************
 * cgps_GetGpsRtcMs
 *
 * Function description:
 *
 *    cgps_GetGpsRtcMs obtains the current value of the Millisecond counter
 *    (extended to 32-bits). This will have an offset from the real hardware GPSRTC
 *    for DPO modes. In DPO, GPSRTC will have discontinuities (turned off to save power).
 *    But the ExternalGpsRtcMs will be adjusted to include the OFF_TIME so that
 *    external clients are not affected by DPO.
 *
 * Parameters:
 *
 *  p_GpsRtcMs - Pointer for the variable where GpsRtcMs will be stored
 *
 * Return value:
 *
 *  TRUE if the GpsRtc is valid (i.e. GPS Clock is running).
 *
 * ******************************************************************************
*/
boolean cgps_GetGpsRtcMs( uint32 *p_GpsRtcMs );


/*****************************************************************************
 * Support for old lsm interface for WL1
 *****************************************************************************
*/

typedef cgps_CellUpdateEnumType         wl1_cell_update_e_type;
typedef cgps_WcdmaCellTimeType          wl1_cell_time_type;
typedef cgps_WcdmaCellInfoType          wl1_wcdma_cell_info_type;
typedef cgps_WcdmaCellTimingType        wl1_cell_timing_type;
typedef cgps_WcdmaCellTimingStructType  wl1_cell_timing_update_type;

#define WL1_TIMETAG_NONE              CGPS_TIMETAG_NONE
#define WL1_TIMETAG_REQUEST           CGPS_TIMETAG_REQUEST
#define WL1_TIMETAG_PRE_CELL_CHANGE   CGPS_TIMETAG_PRE_CELL_CHANGE
#define WL1_TIMETAG_POST_CELL_CHANGE  CGPS_TIMETAG_POST_CELL_CHANGE
#define WL1_TIMETAG_REF_CELL_CHANGE   CGPS_TIMETAG_REF_CELL_CHANGE

#define WL1_LSM_CELLS_TIME_INFO_MAX   CGPS_WCDMA_CELLS_TIME_INFO_MAX

GPS_INLINE void lsm_wl1_latch_gps_timer( void  )
{
  cgps_WL1_LatchGpsTimer();
}

GPS_INLINE boolean lsm_wl1_get_gps_timer( wl1_cell_update_e_type update_type,
                                        uint32 *gps_timer )
{
  return cgps_WL1_GetGpsTimer(gps_timer);
}

GPS_INLINE void lsm_start_gps_timer( void )
{
  cgps_StartGpsTimer();
}

GPS_INLINE boolean lsm_wcdma_cell_info_update( wl1_cell_timing_update_type *cell_info )
{
  return cgps_WcdmaCellInfoUpdate(cell_info);
}

/*
 ******************************************************************************
 * Function cgps_ModeSwitch
 *
 * Description:
 *
 *  This function is used by external modules to set GPS in SARF mode or exit SARF
 * mode. Call-back is called with SUCCESS or FAILURE depending on whether the
 * switch actually happpened.
 *
 * Parameters:
 *
 *  e_new_state - Desired MGP state.
 *  p_cb        - Pointer to CB function.
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None.
 *
 ******************************************************************************
*/
void cgps_ModeSwitch( cgps_sarf_mode_switch_enum_type e_NewState, cgps_mode_switch_callback p_Cb );

/*
 ******************************************************************************
 * Function cgps_SigProcMemSize
 *
 * Description:
 *
 * This function performs check on the buffer size of the signal processing
 * memory meu_SigProcMem[].
 *
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
 *  Size of the the buffer meu_SigProcMem.
 *
 ******************************************************************************
*/
uint32 cgps_SigProcMemSize( void );

/*
 ******************************************************************************
 * Function cgps_StartIqCollect
 *
 * Description:
 *
 *  This function is used by external modules to trigger an IQ collect.
 * Returns TRUE if the request is accepted, else returns FALSE.
 *
 * Parameters:
 *
 *  p_Buffer - Pointer to memory that must contain IQ samples.
 *  p_cb     - Pointer to CB function. Called in GPS task context
 *             on finishing sample collect.
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Boolean - TRUE if Iq collect will be processed by GPS, FALSE otherwise.
 *            (eg., FALSE if mode-switch is not applied).
 *
 ******************************************************************************
*/
boolean cgps_StartIqCollect( cgps_CplxS32_Ftm_type *p_Buffer, cgps_SARF_Cb p_Cb );

/*
 ******************************************************************************
 * Function cgps_StartIqCollectForXoCal
 *
 * Description:
 *
 *  This function is used by external modules to trigger an IQ collect for XO calibration. 
 * Returns TRUE if the request is accepted, else returns FALSE.
 *
 * Parameters: 
 *
 *  p_cb     - Pointer to CB function. Called in GPS task context
 *             on finishing sample collect.
 *  l_Power  - Power set at the test box
 *  l_FreqOffset - Freq offset from GPS L1
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  uint8 - TRUE if Iq collect will be processed by GPS, FALSE otherwise.
 *            (eg., FALSE if mode-switch is not applied).
 *
 ******************************************************************************
*/
uint8 cgps_StartIqCollectForXoCal ( cgps_XOCAL_Cb p_Cb, int32 l_Power, int32 l_FreqOffset );

/*
 ******************************************************************************
 * Function cgps_StartIqForXoCal
 *
 * Description:
 *
 *  This function is used by external modules to trigger an IQ collect for XO calibration. 
 * Returns TRUE if the request is accepted, else returns FALSE.
 *
 * Parameters: 
 *
 *  p_cb     - Pointer to CB function. Called in GPS task context
 *             on finishing sample collect.
 *  l_Power  - Power set at the test box
 *  l_FreqOffset - Freq offset from GPS L1
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  uint8 - TRUE if Iq collect will be processed by GPS, FALSE otherwise.
 *            (eg., FALSE if mode-switch is not applied).
 *
 ******************************************************************************
*/
uint8 cgps_StartIqForXoCal ( cgps_XOCAL_results_Cb p_Cb, int32 l_Power, int32 l_FreqOffset );

/******************************************************************************
 * cgps_GetNvUtcOffset
 *
 * Description:
  * Return the NV item for the GPS UTC offset.
 *
 * Parameters:
 *
 *  None.
 *
 * Dependencies:
 *
 * Return value:
 *
 *  Uint8 - u_GpsOffset.
 *
 *
 ******************************************************************************
*/
uint8 cgps_GetNvUtcOffset( void );


/*
 ******************************************************************************
 * cgps_GetNvUtcOffset
 *
 * Description:
  * Return the NV item for the GPS UTC offset.
 *
 * Parameters:
 *
 *  None.
 *
 * Dependencies:
 *
 * Return value:
 *
 *  Uint8 - u_GpsOffset.
 *
 *
 ******************************************************************************
*/
uint8 cgps_GetNvUtcOffset( void );


/** This is a Function pointer for when cgps has acquired or needs to release
    the Resource.

    Parameters:

*/
typedef void * (*cgps_MemArbCBType)
(
 void *p_CTxt,
 uint32 q_Param,
 void *p_Payload,
 uint32 q_PayloadSize
);


/*
 ******************************************************************************
 * Function cgps_MemArbInit
 *
 * Description:
 *
 *  This function is called to initialize all the necessary structures before
 *  requesting for IMEM resource.  This function only needs to be called once.
 *
 * Parameters:
 *  p_PreReleaseCB - The callback to invoke indicating that the previously acquired
 *                   memory is about to be released.
 *  p_PostAcqureCB - The callback to invoke indicating that the memory has been
 *                   acquired.
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Non Zero - Handle to IMEM, Zero - Failed to init.
 *
 ******************************************************************************
*/
void *cgps_MemArbInit
(
  cgps_MemArbCBType p_PreReleaseCB,
  cgps_MemArbCBType p_PostAcqureCB
);


/*
 ******************************************************************************
 * Function cgps_MemArbDeRegisterForResource
 *
 * Description:
 *
 *  This function is called to De Register for use of a resource from RM.
 *
 * Parameters:
 *  p_GpsHandle - Handle to IMEM object
 *  q_UniqueToken - The unique token passed back by the MemArb module
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  True success; else failure
 *
 ******************************************************************************
*/
boolean cgps_MemArbDeRegisterForResource
(
  void *p_GpsHandle,
  uint32 q_UniqueToken
);


/*
 ******************************************************************************
 * Function cgps_MemArbRegisterForResource
 *
 * Description:
 *
 *  This function is called to register for use of a resource from MemArb.
 *
 * Parameters:
 *  p_GpsHandle - Handle to IMEM object
 *  q_SourceBaseAddress - Base address of internal GPS buffer
 *  q_IMemSize - Size of the internal buffer
 *  p_UniqueToken - The unique token passed back by the MemArb module
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  True success; else failure
 *
 ******************************************************************************
*/
boolean cgps_MemArbRegisterForResource
(
  void *p_GpsHandle,
  uint32 q_SourceBaseAddress,
  uint32 q_IMemSize,
  uint32 *p_UniqueToken /* Output */
);


/*
 ******************************************************************************
 * Function cgps_MemArbConvertNotifyInfo
 *
 * Description:
 *
 *  This a helper function to convert MemArb Notify info to GPS data structure
 *
 * Parameters:
 *  p_MemArbInfo - Info from Mem Arb
 *  p_GpsMemArbInfo - GPS Mem Arb Info
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
void cgps_MemArbConvertNotifyInfo
(
  const void *p_MemArbInfo,
  cgps_MemArbNotifyInfoType *p_GpsMemArbInfo /* Output */
);


 /******************************************************************************
 * Function cgps_MinGpsWeekNumberGet
 *
 * Description:
 *
 *  Retrieves the Minimum GPS Week Number that is stored in the NV database
 *
 * Parameters:
 *
 *  Pointer to output variable
 *
 * Dependencies:
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void cgps_MinGpsWeekNumberGet( uint16 *p_MinGpsWeekNumber );

/*
 ******************************************************************************
 * Function cgps_XoEstimationState
 *
 * Description:
 *
 *  This function starts or stops XO frequency estimation loop
 *
 * Parameters:
 *
 *  u_Enable : TRUE  : set start frequency estimation sig
 *             FALSE : set stop frequenmcy estimation sig
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
void cgps_XoEstimationState( boolean u_Enable );
/*
 ******************************************************************************
 *
 * cgps_GetChargerStatus( void )
 *
 * Function description:
 *  This function obtains the Battery Charger status. It checks whether a charger
 *  is connected or not, and if the phone is still charging or not.
 *  Possible return values are
 *  CGPS_BATTERY_CHARGER_STATE_UNKNOWN
 *  CGPS_BATTERY_CHARGER_NOT_CONNECTED
 *  CGPS_BATTERY_CHARGER_CONNECTED_NOT_CHARGED
 *  CGPS_BATTERY_CHARGER_CONNECTED_CHARGED
 *
 * Parameters:
 *
 * Return value: cgps_BatteryChargerStatus
 *
 *
 ******************************************************************************
*/

typedef enum
{
  CGPS_BATTERY_CHARGER_STATE_UNKNOWN,
  CGPS_BATTERY_CHARGER_NOT_CONNECTED,
  CGPS_BATTERY_CHARGER_CONNECTED_NOT_CHARGED,
  CGPS_BATTERY_CHARGER_CONNECTED_CHARGED,
  CGPS_BATTERY_CHARGER_STATE_MAX

} cgps_BatteryChargerStatusEnumType;


cgps_BatteryChargerStatusEnumType cgps_GetChargerStatus( void );

/*
 ******************************************************************************
 * cgps_StartFrontEndCalibration
 *
 * Function description:
 *
 *  cgps_StartFrontEndCalibration performs some initialization so that the
 *    RF GPS front-end calibration can start.
 *
 * Parameters:
 *
 *  void
 *
 * Return value:
 *
 *  boolean - TRUE if successful
 *
 ******************************************************************************
*/
boolean cgps_StartFrontEndCalibration( void );

/*
 ******************************************************************************
 * cgps_StopFrontEndCalibration
 *
 * Function description:
 *
 *  cgps_StopFrontEndCalibration performs some deinitialization after the
 *    RF GPS front-end calibration is complete.
 *
 * Parameters:
 *
 *  void
 *
 * Return value:
 *
 *  boolean - TRUE if successful
 *
 ******************************************************************************
*/
boolean cgps_StopFrontEndCalibration( void );

/******************************************************************************
 * Function cgps_MinGpsWeekNumberGet
 *
 * Description:
 *
 *  Retrieves the Minimum GPS Week Number that is stored in the NV database
 *
 * Parameters:
 *
 *  Pointer to output variable
 *
 * Dependencies:
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void cgps_MinGpsWeekNumberGet( uint16 *p_MinGpsWeekNumber );

/*
 ******************************************************************************
 * Function cgps_LteTTRpt
 *
 * Description:
 *
 *  This function is called by LTE module to pass Time Transfer Report packet
 *  to PGI module.
 *
 * Parameters:
 *
 *  Time Transfer Report structure.
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
extern void cgps_LteTTRpt ( const cgps_LteTTRptType * );

/*
 ******************************************************************************
 *
 * cgps_SetExternalChargerStatus( void )
 *
 * Function description:
 *  This function sets a static indication of the external charger status.
 *  The Charger status may be used by other functions to enable / disable functions
 *  (for example, the receiver may be configured to disable low power mode of operation
 *  when it is connected to charger to give better accuracy)
 *
 * Parameters:
 *  e_ExtChargerStatus -- Indicates whether the receiver is connected to a charger
 *
 * Return value: None
 *
 *
 ******************************************************************************
*/

void cgps_SetExternalChargerStatus( cgps_BatteryChargerStatusEnumType e_ExtChargerStatus );

/*
 ******************************************************************************
 * Function cgps_GetGpsTime
 *
 * Description:
 * Provide a time API to Tcxo-Mgr for XO aging feature.
 *
 * Parameters:
 * None
 *
 * Dependencies:
 * None
 *
 * Return value:
 *
 *  uint64: GPS time in seconds, 0 indicates time is not known.
 *
 ******************************************************************************
*/
uint64 cgps_GetGpsTime(void);


/*
 ******************************************************************************
 * Function cgps_GetTimeInfo
 *
 * Description:
 * Api for latching Reference Time information for WWANs during Time related 
 * operations like time tag and time transfer
 *  
 * Parameters:
 * pz_ComTime - Common Time structure
 * e_TimeType - Type of operation
 *
 * Dependencies:
 * None
 *
 * Return value: 
 * None
 *
 ******************************************************************************
*/
void cgps_GetTimeInfo
(
  cgps_ComTimeType *pz_ComTime,
  cgps_TimeReqType e_TimeReqType
);


#endif /* CGPS_API_H */
