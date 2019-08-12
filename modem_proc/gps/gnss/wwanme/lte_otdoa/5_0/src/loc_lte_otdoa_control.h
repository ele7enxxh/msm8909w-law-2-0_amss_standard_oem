#ifndef LOC_LTE_OTDOA_CONTROL_H
#define LOC_LTE_OTDOA_CONTROL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Location LTE OTDOA Control Module Header File

GENERAL DESCRIPTION
This file contains API definitions between Location LTE OTDOA Control module
files.

===========================================================================
 
  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/lte_otdoa/5_0/src/loc_lte_otdoa_control.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "loc_lte_otdoa_common_defs.h"
#include "sm_loc_lte_otdoa.h"
#include "limits.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
/* min. number of TOA measurments necessary to generate an OTDOA report */
#define LLOC_MIN_NUM_OF_TOA_MEAS_FOR_OTDOA_RPT   2

/* if we choose the center timestamp as a projection point for OTDOA report,
 * define this macro as 1. If last timestamp is used for projection, declare
 * the macro as 0 */
#define LLOC_CENTER_PROJ_POINT_FOR_OTDOA      1

/* distance equivalent of Ts (Ts * speed of light) */
#define LLOC_TS_TO_METERS                     9.75

/* Maximum number of unique EARFCNs in assitance data (to support inter-freq) */
#define LLOC_MAX_FREQ_LAYERS           SM_OTDOA_NEIGHBOUR_FREQ_INFO_ELEMENTS

/* As we have maximum 72 ngbrs and 1 reference cell, TOA DB size should be 73 */
#define LLOC_MAX_CELL_SEARCHES         LOC_LTE_OTDOA_MAX_NGBR_CELLS+1

/* Masks for bit-mapped validity field for RSTD DB */
#define LLOC_RSTD_DB_CELL_MEAS_MASK           1U << 0
#define LLOC_RSTD_DB_SFN_MASK                 1U << 1
#define LLOC_RSTD_DB_GLOBAL_CELL_ID_MASK      1U << 2
#define LLOC_RSTD_DB_EARFCN_MASK              1U << 3
#define LLOC_RSTD_DB_RSTD_VAL_MASK            1U << 4
#define LLOC_RSTD_DB_MEAS_QUALITY_MASK        1U << 5
#define LLOC_RSTD_MEAS_QUAL_NUM_SAMPLES_MASK  1U << 0

/* maximum num of MNC/MCC digits in global cell ID */
#define LLOC_MAX_MNC_MCC_DIGITS               3

/* PRS and RSTD log packet version IDs */
#define LLOC_PRS_TOA_LOG_VERSION              13
#define LLOC_PRS_RSTD_LOG_VERSION             7
#define LLOC_PRS_ASSIST_DATA_LOG_VERSION      9

/* The expected RSTD and uncertainity values sent in assistance data has
 * a resolution of 3Ts units */
#define LLOC_RSTD_RESOLUTION                  3

/* Shift values to multiply and divide */
#define LLOC_SCALE_BY_32                     5
#define LLOC_SCALE_BY_64                     6
#define LLOC_SCALE_BY_128                    7
#define LLOC_SCALE_BY_256                    8
#define LLOC_SCALE_BY_512                    9
#define LLOC_SCALE_BY_1024                   10
#define LLOC_SCALE_BY_2048                   11
#define LLOC_SCALE_BY_2                      1

/* max RSTD and unc. values */
#define LLOC_MAX_RSTD                         16383
#define LLOC_MAX_RSTD_UNC                     1023

/* max. config index */
#define LLOC_MAX_CONFIG_INDEX                 2399

/* Size of PRS subframe offset-EARFCN look-up table */
#define LLOC_PRS_SF_OFFSET_LUT_SIZE      10

/*
 * PRS pipeline delay. PRS peak with 0 transmission delay that should occur at 1024 (win center),
 * occurs at ~1095 (71 Ts delay). Increase the RSTD unc. of ngbr cells by 30 (3*Ts) units
 * to compensate for the delay with additional room of 20 Ts
 */
#define LLOC_PRS_PIPELINE_DELAY               30

/* Timeout before MP task should send Meas reports */
#define LLOC_MP_SANITY_TIMEOUT                200

/* Timeout before ML1 task should send update cell to Ctrl */
#define LLOC_ML1_SANITY_UPDATE_CELLS_TIMEOUT  200

/* Timeout before SM task should send message to Control */
#define LLOC_SM_SANITY_TIMEOUT        200

/* Master timers for an OTDOA session. OTDOA should be back to INACTIVE
 * state at the end of this timeout*/
#define LLOC_SM_MISSION_MODE_TIMER    100000    /* 100secs */
#define LLOC_SM_PRS_DEBUG_MODE_TIMER  1200000   /* 20mins */

#define TIMER_VALID(x) ( (x == LTE_OTDOA_CONTROL_TIMER_ID_SM_RPT) ||\
                         (x == LTE_OTDOA_CONTROL_TIMER_ID_ML1_SANITY) ||\
                         (x == LTE_OTDOA_CONTROL_TIMER_ID_MP_SANITY) ||\
                         (x == LTE_OTDOA_CONTROL_TIMER_ID_SM_SANITY) )

/* Max PCI range 0 to 503 */
#define LLOC_MAX_PCI        503

/* Log types for AssitData log */
#define LLOC_AD_FROM_LOC_SERVER   0
#define LLOC_AD_UPDATE_TO_ML1     1
#define LLOC_AD_TO_ML1_AFTER_SERV_CELL_UPDATE 2
#define LLOC_AD_TO_ML1_PRS_SCANNER 3
#define LLOC_AD_TO_ML1_AFTER_MAX_SRCH_SIZE 4
#define LLOC_AD_TO_ML1_PARTIAL_UPDATE 5

/* Mask to log LTE supported bands in AD log packet */
#define LLOC_AD_LOG_LTE_BANDS_INFO_VALID  (1<<0)

/* Num of consecutive meas. based on AntPortConfig */
#define LLOC_NUM_CONSECUTIVE_MEAS_2_ANT  2
#define LLOC_NUM_CONSECUTIVE_MEAS_4_ANT  4

/* Minimum PRS configuration index for Tprs=1280ms */
#define LLOC_1280_MS_TPRS     1120

/* Threshold limit to determine weak cell */
#define LLOC_WEAK_CELL_THRES_LIMIT  3.0

/* Max number of unique EARFCN values to identify */
#define LLOC_MAX_UNIQUE_EARFCNS      3

/* Min CER value */
#define LLOC_MIN_CER  LLOMP_MIN_UINT32_CER

/* Min noise floor to avoid divide by zero computation */
#define LLOC_MIN_NF 0.00000001  // 1e-8

/* Min PRS Gain Bias value */
#define LLOC_MIN_HW_PRS_GAIN_BIAS    10

/* Increase FA threshold by 3dB during pre-discovery phase (i.e. when serving cell not in AD).
 * Scaled by a factor of 10 */
#define LLOC_ADJUST_FA_THRESH_PRE_DISC_PHASE  30

#define LLOC_MAX_LTE_FREQ_BANDS   41
#define LLOC_LTE_BANDS_MASK_LEN   64
#define LLOC_LTE_BANDS_ARRAY_LEN   4
#define LLOC_MEAS_CNT_FOR_PRS_OCCASIONS 3

/* Max value for PRS SF offset */
#define LLOC_MAX_PRS_SF_OFFSET  1279

#define LLOC_MAX_MOD_6_LEVEL      6

/* PRS IC Offend Threholds in dB */
#define LLOC_PRS_IC_OFFEND_TH_1_4_MHZ 20.9126
#define LLOC_PRS_IC_OFFEND_TH_3_MHZ 24.8991
#define LLOC_PRS_IC_OFFEND_TH_5_MHZ 27.1189
#define LLOC_PRS_IC_OFFEND_TH_10_MHZ 30.1301
#define LLOC_PRS_IC_OFFEND_TH_15_MHZ 31.8863
#define LLOC_PRS_IC_OFFEND_TH_20_MHZ 33.1369

#define LLOC_ABS_VAL(x) ((x) < 0 ? -(x) : (x))

/* Size of 4Tx antenna port and EARFCN look-up table */
#define LLOC_4TX_ANT_PORT_FREQ_LUT_SIZE      10

/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/* enum to identify the type of searches for reference cell */
typedef enum
{
  /* srch 4 hypothesis each with maximum window size */
  LLOC_MAX_SRCH_SIZE,
  /* srch only 1 hypothesis with maximum window size */
  LLOC_MIN_SRCH_SIZE
}
lloc_SrchSizeType;

/* enum to identify search sizes (RSTD unc.) for all the cells */
typedef enum
{
  LLOC_SERV_IS_AIDING_REF,
  LLOC_SERV_IS_NGBR,
  LLOC_SRCH_NOT_STARTED,
  LLOC_MAX_WIN_SIZE_FOR_ALL_CELLS,
  LLOC_REF_CELL_TIMING_AVAIL
}
lloc_SrchStrategyType;

/* Error codes for Ctrl task */
typedef enum
{
  LLOC_NO_ERR = 1,
  LLOC_ERR_NULL_SHARED_MEM,
  LLOC_ERR_ML1_MAX_CELLS,
  LLOC_ERR_ML1_INVALID_PRS_SEQ_NUM,
  LLOC_ERR_ML1_NO_CELLS,
  LLOC_ERR_ML1_GENERAL_ERROR,
  LLOC_ERR_ML1_NULL_SERVING_CELL_INFO,
  LLOC_ERR_ML1_INVALID_SERVING_CELL_ID,
  LLOC_ERR_ML1_INVALID_CELL_MEASURED,
  LLOC_ERR_SM_BAD_ASSIST_DATA,
  LLOC_ERR_MP_RPT_INVALID_HYP,
  LLOC_ERR_MP_RPT_NULL,
  LLOC_ERR_MP_NO_BUF,
  LLOC_ERR_MP_GENERAL_ERROR,
  LLOC_ERR_MP_INVALID_CELL_PROCESSED,
  LLOC_ERR_MAX
}
lloc_ErrorType;

/* DIAG test commands mimicking commands from SM */
typedef enum
{
  LLOC_TEST_SM_UPDATE_TIMEOUT_CMD,         /* 0 */
  LLOC_TEST_SM_REQ_MEAS_RPT_CMD,           /* 1 */
  LLOC_TEST_SM_STOP_SESS_CMD               /* 2 */
}
lloc_SmTestSubCmdEnumType;


/********************* DB for TOA info *********************/
/* cell type */
typedef enum
{
  /* this cell is a ref. cell given by server in aiding data */
  LLOC_AIDING_REF_CELL = 1,
  /* this cell is a serving cell */
  LLOC_SERVING_CELL,
  /* this cell is purely a ngbr cell */
  LLOC_NGBR_CELL
}
lloc_CellType;

/* Signal strength type of the cell */
typedef enum
{
  LLOC_STRONG_CELL = 0,
  LLOC_WEAK_CELL,
  LLOC_VERY_WEAK_CELL,
  LLOC_STRENGTH_NOT_CLASSIFIED
}
lloc_SignalStrengthType;

/* Index to store the 1st and last valid TOA for a cell */
typedef enum
{
  LLOC_TOA_FIRST = 0,
  LLOC_TOA_LAST,
  LLOC_TOA_BEST,
  LLOC_TOA_MEAS_DB_MAX
}
lloc_ToaMeasDbIndexType;

typedef struct
{
  /* Time of arrival meas. of this cell based on the EAP. Units of Ts */
  int64 t_Toa;

  /* Sub-sample interpolated peak location. Units of Ts/256 */
  uint8 u_FractionalTs;

  /* PRS occasion sequence number (from FW shared mem) */
  uint32 q_OccasionSeqNum;

  /* Status to mark if the measurement processing was success or not */
  loc_lte_otdoa_PrsMeasProcErrorType e_Status;

  /* Validity flag */
  boolean b_valid;
}
lloc_ToaMeasDbType;

/* DB to store time of arrival (TOA) info for cells sent by MeasProc task */
typedef struct
{
  /* Validity of the data. Will always be set to 1 for now */
  boolean b_Valid;

  /* cell type */
  lloc_CellType e_CellType;

  /* physical cell ID */
  uint16 w_CellId;

  /* LTE downlink time at the SFN boundary of the PRS occasion. Units of Ts */
  int64 t_DlTimestamp;

  /* Time of arrival meas. of this cell based on the EAP. Units of Ts. This will be the best (earliest)
   * arriving TOA for the cell */
  int64 t_Toa;

  /* Sub-sample interpolated peak location. Units of Ts/256 */
  uint8 u_FractionalTs;

  /* A metric for measurement uncertainty based on Cramer-Rao lower bound
   * calculation, but modified by the "double-hump" uncertainty for search
   * windows that are off by more than the cyclic prefix width from nominal.
   * Units of Ts */
  FLT f_EapUnc;

  /* Total unc. (meas. unc. + time unc.) */
  FLT f_TotalUnc;

  /* LTE system BW in enum */
  loc_lte_otdoa_PrsBandwidthType e_LteSysBw;

  /* VSRC OSTMR diff */
  FLT f_VsrcOstmrDiff;

  /* Adj to TOA for inter-freq cases with mixed BW */
  int8 b_ToaAdjMixedBw;

  /* signal-to-noise ratio for meas. cell using EAP CER */
  uint16 w_Snr;

  /* signal-to-noise ratio for meas. cell using max peak */
  uint16 w_MaxPeakSnr;

  /* FW signal-to-noise ratio for meas. cell */
  int16 x_FwSnrRx0;

  /* FW signal-to-noise ratio for meas. cell */
  int16 x_FwSnrRx1;

  /* This field specifies the E-UTRA carrier frequency of the RSTD reference cell */
  uint32 w_Earfcn;

  /* the maximum CER (Channel Energy Response) value for the
   * measured cell across multiple hypothesis */
  FLT f_MaxCer;

  /* the CER value corresponding to the TOA of the EAP (earliest arriving path) */
  FLT f_EapCer;

  /* System frame number associated with measurement of cell. Range [0,1023]. */
  uint16 w_Sfn;

  /* Offset from EAP to center of search window. May be positive or negative.
   * Units of Ts. */
  int32 l_WinCenterAndToaDiff;

  /* Offset from Max. CER to center of search window. May be positive or negative.
   * Units of Ts. */
  int32 l_WinCenterAndMaxCerDiff;

  /* Search window size. Max. value 2047 (Ts units) */
  uint16 w_SrchWinSize;

  /* Status to mark if the measurement processing was success or not */
  loc_lte_otdoa_PrsMeasProcErrorType e_Status;

  /* Peak position offset from SFN boundary. This includes PeakIndex and all time-related corrections */
  int16 x_PeakOffsetFromSfn;

  /* Accumulated TTL Adjustment */
  int16 x_TtlAdj;

  /* Accumulated COM (center of mass) Adjustment (Fixed Arithmetic Q6 format) */
  int32 l_ComAdj;

  /* Mark if the Meas is NC or not */
  boolean b_NCMeas;

  /* Current NC Level */
  uint8 u_NCLevel;

  /* Mark which symbol offset was used in the final measurements and which symbol offset to be
   * used for NC/Remeas State */
  int8 b_SymOffsetForRemeasPhase;

  /* Log the mesaurement status of the cell */
  lloc_MeasStatusType e_MeasState;

  /* Signal strength type of the cell */
  lloc_SignalStrengthType e_SignalStrength;

  /* Accumulated MSTMR adjustment, in Ts units, Q0 format. */
  int64 t_MstmrAdj;

  /* PRS occasion type */
  lloc_PrsOccasionType e_PrsOccasionType;

  /* PRS occasion sequence number (from FW shared mem) */
  uint32 q_OccasionSeqNum;

  /* Noise floor associated with the meas. */
  FLT f_NoiseFloor;

  /* Flag to indicate if the TOA meas report is for raw CER or for final NC combined
   * CER, for b_NCMeas enabled cells */
  boolean b_NcCombinedMeas;

  /* PRS Gain Bias */
  int8 b_PrsGainBias;

  /* Flag to indicate if the meas. was done when a stronger offending
   * cell (same mod-6 value) was muted, and also the PCI of the offending
   * cell */
  boolean b_MeasWhenOffendingCellMuted;
  uint16 w_OffendingCellPci;
  uint32 w_OffendingCellEarfcn;

  uint8 u_ML1SchedFlags;
  FLT f_GlobalSNRdB;

  /* Carrier used for the measuremnt of this cell. It could be PCC, SCC1 or SCC2.
   * Range: 0 to 'LTE_LL1_CARRIER_COUNT-1'. As of now, 'LTE_LL1_CARRIER_COUNT' is defined as '3'
   * in 'modem_proc\fw_lte\api\intf_common.h' */
  uint8 u_Carrier; 

  /* PRS frequency meas. type. Info about whether the current cell was an intra-frequency meas.,
   * or inter-frequency meas. */
  loc_lte_otdoa_FreqMeasInfoType e_MeasType;

  /* OSTMR-VSRC info at reference instance (when RF was turned ON) */
  loc_lte_otdoa_OstmrVsrcInfoType z_RefVsrcInfo;

  /* OSTMR-VSRC info at start of a PRS occasion */
  loc_lte_otdoa_OstmrVsrcInfoType z_PrsOccVsrcInfo;

  /* Num of hyp and best hyp index */
  uint8 u_NumHypPerCell;
  uint8 u_BestHypIdx;

  /* Peak index (including sub-Ts) within CER window */
  FLT f_PeakIndex;

  /* Structure to store FFT residual signal detection for every hypothesis */
  lloc_FftResidualSignalDetectionType z_FftResidualSigDet[LOC_LTE_OTDOA_MAX_HYPO_PER_CELL];

  /* Structure to store the 1st and last valid TOA of a cell. This will be used to determine RSTD for
   * inter-freq cells. */
  lloc_ToaMeasDbType z_ToaMeasDb[LLOC_TOA_MEAS_DB_MAX];
}
lloc_ToaDbPerCellType;

/* DB type to store time of arrival (TOA) info for all cells sent by MeasProc task. */
typedef struct
{
  /* num. of meas.*/
  uint16 w_Count;

  /* num. of successful meas. (no error) */
  uint16 w_NumSuccessMeas;

  lloc_ToaDbPerCellType z_CellInfo[ LLOC_MAX_CELL_SEARCHES ];
}
lloc_ToaDbType;


/********************* diag packet *********************/
/* Diag packet */
typedef struct
{
  uint8 * p_DiagPkt;
  uint16  w_PktLength;
}
loc_lte_otdoa_control_DiagPktMsgType;


/********************* interface struct between Ctrl and MP task *********************/
/* info sent by MeasProc task after it completes PRS peak processing of all cells
 * info sent by MeasProc task after it completes PRS peak processing of all hyp in one cell */
typedef struct
{
  /* PRS Meas Result Sequence Number */
  uint32 q_OccasionSeqNum;

  /* TOA info of 1 cell */
  lloc_ToaDbPerCellType z_ToaInfo;
}
loc_lte_otdoa_control_PrsMeasProcCompleteInfoType;

/* Info sent by MeasProc task after it completes processing of one
   hypotheses */
typedef struct
{
  loc_lte_otdoa_MeasBufType * p_MeasBuf;

  /* Status to mark if the measurement processing was success or not */
  loc_lte_otdoa_PrsMeasProcErrorType e_Status;
} loc_lte_otdoa_control_PrsHypothesesCompleteInfoType;

/* Info sent by MeasProc task after it completes Stop measurements
   processing command */
typedef struct
{
  /* Status to mark if the stop processing was success or not */
  loc_lte_otdoa_PrsMeasProcErrorType e_Status;
}
loc_lte_otdoa_control_StopMeasProcAckInfoType;


/********************* DB for aiding data *********************/
typedef enum
{
  LLOC_AD_RESET_AFTER_PARTIAL_UPDATE,
  LLOC_AD_RESET_AFTER_ONE_ROUND_COMPLETE,
  LLOC_AD_RESET_AFTER_SERVING_CELL_UPDATE
}
lloc_AdParamType;

/* PRS muting repeat enum */
typedef enum
{
  LLOC_PRS_MUTING_REPEAT_2 = 0,  /* Repeat every 2 occasions  */
  LLOC_PRS_MUTING_REPEAT_4,      /* Repeat every 4 occasions  */
  LLOC_PRS_MUTING_REPEAT_8,      /* Repeat every 8 occasions  */
  LLOC_PRS_MUTING_REPEAT_16,     /* Repeat every 16 occasions  */
  LLOC_PRS_MUTING_REPEAT_MAX,    /* Maximum */
}
lloc_MutingRepeatType;

/* muting pattern type */
typedef struct
{
  /* index will be either 2, 4, 8 or 16. This represents the number of
   * bits valid in muting pattern */
  lloc_MutingRepeatType e_MutingIndex;
  uint16 w_MutingPattern;
}
lloc_MutingPatternForMl1Type;

typedef struct
{
  boolean b_MutingPresent;
  uint8 b_MutingIndex;
  uint8 b_Pattern[4];
}
lloc_MutingPatternFromSmType;

typedef struct
{
  uint8 u_Mcc[3];
  uint8 u_Mnc[3];
  uint32 q_CellIdentity;
  uint8 u_NumOfMncDigits;
  uint8 u_NumOfCellIdBits;
}
lloc_GlobalCellIdType;

typedef struct
{
  /* Symbol offset index */
  int8 b_SymbolOffsetIndex;

  /* Prune window size */
  uint16 w_PruneWinSize;

  /* Prune window start index */
  uint16 w_PruneWinStartIndex;
}
lloc_HypothesisElement;

typedef struct
{
  uint8 u_NumHypotheses;

  /* hypotheses */
  lloc_HypothesisElement z_HypoElem[LOC_LTE_OTDOA_MAX_HYPO_PER_CELL];
}
lloc_HypothesisPerCellType;

/* Aiding data of PRS info per cell */
typedef struct
{
  /* validity fields */
  boolean b_GlobalIdPresent;
  boolean b_EarfcnPresent;
  boolean b_CpLengthPresent;
  boolean b_PrsInfoPresent;
  boolean b_AntPortConfigPresent;
  boolean b_SlotNumOffsetPresent;
  boolean b_SubFrameOffsetPresent;

  /* physical cell ID  Range ~ 0 to 503 */
  uint16 w_PhyCellId;

  /* Num of consecutive meas. to be done */
  uint8 u_NumOfConsecutiveMeas;

  /* Cell type - ref. or ngbr or serving cell */
  lloc_CellType e_CellType;

  /* This field specifies the ECGI, the globally unique identity of a cell in E-UTRA,
   * of the RSTD reference cell. The target shall provide this IE if it knows the ECGI
   * of the RSTD reference cell. */
  lloc_GlobalCellIdType z_GlobalCellId;

  /* This field specifies the E-UTRA carrier frequency of the reference cell. We should store
   * 2 copies of EARFCN in case we need to change the EARFCN value based on the serving cell
   * EARFCN if they both are on same frequency. For example, EARFCNs 5900 and 8750 have the same
   * frequency. When reporting RSTD meas., we must the report the same EARFCN that was sent in AD. */
  uint32 w_Earfcn;
  uint32 w_EarfcnFromAd;

  /* Antenna port config */
  lloc_PrsAntPortConfigType e_AntennaPortConfig;

  /* cyclic prefix length */
  loc_lte_otdoa_CpModeType e_CpLength;

  /* PRS bandwidth in assistance from Loc server and value used by ML1. In most cases, it will be the same
   * but there is 1 corner case when PRS BW is > system BW of serving cell (PCC or SCC based on IF) */
  loc_lte_otdoa_PrsBandwidthType e_PrsBandwidth;
  loc_lte_otdoa_PrsBandwidthType e_PrsBwUsed;
  
  /* Configuration index */
  uint16 w_PrsConfigIndex;

  /* Number of downlink subframes from Loc server and value used by ML1. In most cases, it will be the same
   * but there are cases with I/F and MG. LTE ML1 will modify Nprs as needed if it doesnt fit within gaps */
  loc_lte_otdoa_PrsDlSubframesType e_NumDlFrames;
  loc_lte_otdoa_PrsDlSubframesType e_NprsUsed;

  /* Muting pattern */
  lloc_MutingPatternForMl1Type z_MutingPatternToMl1;
  lloc_MutingPatternFromSmType z_MutingPatternFromSm;

  /* RSTD info */
  lloc_AidingRstdInfoType z_AidingRstdInfo;

  /* Number of hypotheses */
  lloc_HypothesisPerCellType z_Hypo;

  /* cell search status */
  lloc_MeasStatusType e_MeasStatus;

  /* NC Level */
  uint8 u_NcLevelEven;
  uint8 u_NcLevelOdd;

  /* Which hypothesis to use during NC or Remeas State */
  uint8 u_HypNum;

  /* Counters for num of times measured and processed */
  uint8 u_NumOfTimesMeasured;
  uint8 u_NumOfTimesProcessed;

  /* Signal strength type of the cell */
  lloc_SignalStrengthType e_SignalStrength;

  /* Max of MaxPeakCer among multiple meas. */
  FLT f_MaxPeakCer;

  /* Copy of above Max of MaxPeakCer. It will be used for CER saturation detection algm */
  uint32 q_MaxPeakCerCopy;

  /* Noise floor associated with Max of MaxPeakCer among multiple meas. */
  FLT f_NoiseFloor;

  /* Is strong enough to cause x-corr  */
  boolean b_IsOffendingCell;

  /* Is PRS enough to cause x-corr  */
  boolean b_IsPrsIcOffendingCell;

  /* SNR associated with PRS IC*/
  FLT f_GlobalSNRdB;

  /* Peak Found or Not */
  boolean b_PeakFound;

  /* PRS Gain Bias */
  int8 b_PrsGainBias;

  /* Max of MaxPeakSnr for the cell among multiple measurements. In dB scaled by 100 */
  uint16 w_MaxPeakSnr;

  /* Bit mask to track if a cell has been processed by MP for both odd and PRS occasions */
  uint8  u_OccTypeBitMaskForProc;

  /* Bit mask to track if NC meas. has been done for a cell for both odd and PRS occasions */
  uint8  u_OccTypeBitMaskForNcMeas;

  /* Flag to indicate of diversity in meas. is possible based on muting pattern */
  boolean b_DiversityInMeas;

  /* Flag to force recalculating PRS gain bias and signal strength and send intermediate update
   * to ML1 */
  boolean b_UpdateAdToMl1;

  /* Flag to indicate end of disc. phase for a cell. It is reset everytime after we receive diversity meas. */
  boolean b_DiscPhaseDone;
}
lloc_PrsAidingDataDbPerCell;

/* master DB with all relevant PRS info cells */
typedef struct
{
  /* session info sent by SM layer */
  uint8 e_OpMode;
  uint16 e_SessType;

  /* validity fields */
  boolean b_RefCellInfoPresent;
  boolean b_NgbrCellInfoPresent;

  /* Number of frequency layers present in assitance data. Max 3 */
  uint16 w_NumFreqElems;

  /* master DB with all the aiding data info */
  uint16 w_NumNgbrs;
  lloc_PrsAidingDataDbPerCell z_RefCell;
  lloc_PrsAidingDataDbPerCell z_NgbrCell[LOC_LTE_OTDOA_MAX_NGBR_CELLS];

  /* counters to chk if MP has processed all measured hypothesis */
  uint16 w_NumHypMeasured;
  uint16 w_NumHypProcessed;
}
lloc_PrsAidingDataDbType;


/********************* DB for OTDOA meas. *********************/
/* Meas. quality definition. All fields are encoded.  */
typedef struct
{
  /* Bit-mapped validity.
   * Bit 0 represents validity of u_NumSamples */
  uint8 u_Valid;

  /* error resolution. Range ~ 0 to 3.
   * This field specifies the resolution R used in 'u_ErrorValue' field.
   * The encoding on two bits is as follows:
   * '00'      5  meters
   * '01'      10 meters
   * '10'      20 meters
   * '11'      30 meters.
   */
  uint8 u_ErrorResolution;

  /* error value. Range ~ 0 to 31.
   * This field specifies the target device’s best estimate of the uncertainty
   * of the OTDOA (or TOA) measurement.
   * The encoding on five bits is as follows:
   *   '00000' 0     to   (R*1-1) meters
   *   '00001' R*1   to   (R*2-1) meters
   *   '00010' R*2   to   (R*3-1) meters
   *   .... (and so on)
   *   '11111' R*31   meters or more;
   * where R is the resolution defined by error-Resolution field.
   * Eg., R=20m corresponds to 0-19m, 20-39,..., 620+ m.
   */
  uint8 u_ErrorValue;

  /* num. of samples used for estimation. Range ~ 0 to 7
   * If the error-Value field provides the sample uncertainty of the OTDOA (or TOA) measurement,
   * this field specifies how many measurements have been used by the target device to determine
   * this (i.e., sample size). Following 3 bit encoding is used:
   *   ‘000’   Not the baseline metric
   *   '001'   5-9
   *   '010'   10-14
   *   '011'   15-24
   *   '100'   25-34
   *   '101'   35-44
   *   '110'   45-54
   *   '111'   55 or more.
   * In case of the value ‘000’, the error-Value field contains the target device’s best estimate
   * of the uncertainty of the OTDOA (or TOA) measurement not based on the baseline metric.
   * E.g., other measurements such as signal-to-noise-ratio or signal strength can be utilized to
   * estimate the error-Value. If this field is absent, the value of this field is ‘000’.
   */
  uint8 u_ErrorNumSamples;
}
lloc_RstdMeasQualityType;

/* TDOA info */
typedef struct
{
  /* TDOA value between current cell and reference */
  int64 r_TdoaTs;

  /* TDOA value with decimal precision */
  FLT f_TdoaTs;

  /* TDOA value uncertainity */
  FLT f_TdoaUnc;
}
lloc_TdoaInfoType;

/* RSTD info for cells */
typedef struct
{
  /* Bit-mapped validity.
   * Bit 0 represents validity of this cell meas.
   * Bit 1 represents validity of q_SFN
   * Bit 2 represents validity of z_GlobalCellId
   * Bit 3 represents validity of w_Earfcn
   * Bit 4 represents validity of w_Rstd
   * Bit 5 represents validity of z_MeasQual */
  uint32 u_Valid;

  /* This field specifies the subframe number of the RSTD reference cell containing the
   * starting subframe of the PRS occasion during which the most recent neighbour cell
   * RSTD measurement was performed. Range ~ 0 to 1023 */
  uint16 w_Sfn;

  /* physical cell ID  Range ~ 0 to 503 */
  uint16 w_PhyCellId;

  /* This field specifies the ECGI, the globally unique identity of a cell in E-UTRA,
   * of the measured cell */
  lloc_GlobalCellIdType z_GlobalCellId;

  /* This field specifies the E-UTRA carrier frequency of the measured cell */
  uint32 w_Earfcn;

  /* RSTD value for neighbor cells. Ts units. Range ~ 0 to 12711 */
  uint16 w_Rstd;

  /* Time of arrival of the earliest arriving path (EAP). Units of Ts */
  int64 t_EapToa;

  /* measured signal to noise ratio. Unitless */
  uint16 w_EapSnr;
  uint16 w_MaxPeakSnr;

  /* Meas quality (uncertainity.) */
  lloc_RstdMeasQualityType z_MeasQuality;

  /* TDOA info */
  lloc_TdoaInfoType z_TdoaInfo;

  /* Meas status of the cell that was used for final report */
  lloc_MeasStatusType e_MeasState;

  /* Meas status of the cell that was used for final report */
  lloc_SignalStrengthType e_SignalStrength;

  /* PRS occasion sequence number (from FW shared mem) */
  uint32 q_OccasionSeqNum;

  /* For inter-freq cells, ref cell TOA, other than the earliest arriving, might be used for RSTD.
   * Store the occasion seq num from which ref cell TOA is picked up */
  lloc_ToaMeasDbIndexType e_RefToaIndexForRstd;
  uint32 q_RefCellOccasionSeqNum;
}
lloc_RstdInfoType;

/* error types for RSTD meas */
typedef enum
{
  LLOC_RSTD_NO_ERROR = 0,
  LLOC_RSTD_ERROR_ASSIST_DATA_MISSING,
  LLOC_RSTD_ERROR_NO_REF_CELL,
  LLOC_RSTD_ERROR_NO_NGBR_CELLS,
  LLOC_RSTD_ERROR_PARTIAL_NGBR_CELLS,
  LLOC_RSTD_ERROR_UNDEFINED
}
lloc_RstdMeasErrorType;

/* DB to store the OTDOA meas. generated from TOA DB */
typedef struct
{
  /* Number of elements in measurement neighbor list Range 1 to 24 */
  uint8 u_NumNeighborMeasurements;

  /* reference cell meas. */
  lloc_RstdInfoType z_RefCellMeasurements;

  /* neighbor cell meas. */
  lloc_RstdInfoType z_NgbrCellMeasurements[LOC_LTE_OTDOA_MAX_NGBR_RSTD_RPT];

  /* error status */
  lloc_RstdMeasErrorType e_ErrorInfo;
}
lloc_OtdoaMeasRptDbType;

typedef struct
{
  /* RSTD Information for MP task usage */
  lloc_AidingRstdInfoType z_RstdInfo;

  /* PRS BW */
  loc_lte_otdoa_PrsBandwidthType e_PrsBandwidth;
  loc_lte_otdoa_PrsBandwidthType e_PrsBwAd;

  /* Cyclic Prefix */
  loc_lte_otdoa_CpModeType e_CpLength;

  /* Meas State */
  lloc_MeasStatusType e_MeasState;

  /* Signal strength */
  lloc_SignalStrengthType e_SignalStrength;

  /* NC Level */
  uint8 u_NCLevel;

  /* PRS occasion type */
  lloc_PrsOccasionType e_PrsOccasionType;

  /* PRS occasion type */
  lloc_PrsAntPortConfigType e_NumTxAnt;

  /* MaxPeak CER used to calculate the PRS gain bias. It will be used for CER saturation
   * detection algorithm */
  uint32 q_MaxPeakCerForGain;
}
lloc_MPInfoPerHypType;

typedef struct
{
  lloc_MPInfoPerHypType z_MpInfoPerHyp[LOC_LTE_OTDOA_MAX_HYPO_SRCH_PER_BATCH];
}
lloc_MPInfoType;

typedef enum
{
  /* OTDOA session is not active */
  LLOC_OTDOA_NOT_ACTIVE = 0,
  /* OTDOA session has started and start request has been sent to LTE ML1.
   * Waiting for ServingCell info from LTE ML1 before sending the 1st assitance data */
  LLOC_OTDOA_SESS_STARTED,
  /* OTDOA session has stopped. Stop has been sent to LTE ML1 and MP.
   * Waiting for Ack from MP */
  LLOC_OTDOA_SESS_STOPPED,
  /* OTDOA session is in progress. Measurements are being sent by ML1 and being processed
   * by MP */
  LLOC_OTDOA_SESS_IN_PROGRESS
}
lloc_StateType;

typedef struct
{
  /* Defragmentation logic */
  boolean u_RecievedDataValid;
  boolean u_PacketReceivingInProgress;
  uint32  q_ReceivedDataTransferId;
  uint32  q_ReceivedPacketsCount;
  uint32  q_TotalNumberOfPackets;
  uint32  q_TotalDataLength;
  uint32  q_ReceivedDataLength;

  uint32  q_ReceivedData[ LOC_LTE_OTDOA_MAX_DIAG_MSG / 4 ];
}
lloc_DiagDataType;

/* Structure to identify the LTE downlink frequency from EARFCN */
typedef struct
{
  /* Band number */
  uint16 w_BandNum;

  /* Lower frequency value in MHz, scaled by 10 */
  uint32 q_FreqLow;

  /* EARFCN range of an LTE band */
  uint32 q_EarfcnLow;
  uint32 q_EarfcnHigh;
}
lloc_EarfcnFreqConvType;

typedef struct
{
  boolean b_Valid;
  lloc_PrsAidingDataDbPerCell* p_Cell;
}
lloc_WorstOffendingCellType;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

/* structure for PRS Override sub command */
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

  /* Boolean to override PRS parameters */
  uint8   b_OverrideData;

  uint8   q_Data[LOC_LTE_OTDOA_MAX_DIAG_MSG_DATA_LENGTH / 4 ];
}
lloc_DiagPrsOverrideCmdType;

/* Structure to store parameters for FFT residual signal detection. Valid only for TA */
typedef PACKED struct PACKED_POST
{
  FLT f_FaThresUpdate;
  uint32 q_NoiseFloor;
  FLT f_Sigma40;
  FLT f_Sigma60;
}
lloc_FftResidualSignalDetectionLogType;

/* log packet for the outputs of EAP module */
typedef PACKED struct PACKED_POST
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  uint8 u_Version;

  /* Mask for reserved fields */
  uint32 q_Mask;

  /* Time of arrival of the earliest arriving path (EAP). Units of Ts */
  int64 t_EapToa;

  /* Physical cell ID of cell used in PRS measurement Range ~ 0 to 503*/
  uint16  w_PhyCellId;

  /* Sub-sample interpolated peak location. Units of Ts/256 */
  uint8 u_FractionalTs;

  /* A metric for measurement uncertainty based on Cramer-Rao lower
   * bound calculation, but modified by the "double-hump" uncertainty for
   * search windows that are off by more than the cyclic prefix width from nominal.
   * Units of Ts */
  FLT f_MeasurementUnc;

  /* the maximum CER (Channel Energy Response) value for the
   * measured cell across multiple hypothesis */
  FLT f_MaxCer;

  /* the CER value corresponding to the TOA of the EAP (earliest arriving path) */
  FLT f_EapCer;

  /* Cell type - ref. or ngbr or serving cell */
  uint8 e_CellType;

  /* System frame number associated with measurement of cell. Range ~ 0 to 1023 */
  uint16 w_Sfn;

  /* Offset from EAP to center of search window. May be positive or negative.
   * Units of Ts. */
  int32 l_WinCenterAndToaDiff;

  /* Offset from Max. CER to center of search window. May be positive or negative.
   * Units of Ts. */
  int32 l_WinCenterAndMaxCerDiff;

  /* Search window size. Max. value 2047 (Ts units) */
  uint16 w_SrchWinSize;

  /* measured signal to noise ratio (using EAP CER). Unitless */
  uint16 w_EapSnr;

  /* FW reported signal to noise ratio in dB * 100 */
  int16 x_FwSnrRx0;
  int16 x_FwSnrRx1;

  /* Peak position offset from SFN boundary (this includes PeakIndex including all corrections) */
  int16 x_PeakOffsetFromSfn;

  /* Accumulated TTL Adjustment */
  int16 x_TtlAdj;

  /* LTE system BW in RBs */
  uint8 u_LteSysBw;

  /* Accumulated COM (center of mass) Adjustment (Fixed Arithmetic Q6 format) */
  int32 l_ComAdj;

  /* Accumulated MSTMR adjustment, in Ts units, Q0 format. */
  int64 t_MstmrAdj;

  /* EARFCN of the cell - based on uint32 */
  uint32 w_Earfcn;

  /* PRS occasion seq number and Session counter */
  uint32 q_OccasionSeqNum;
  uint32 q_LppSessionCtr;

  /* NC related */
  uint8 u_NCLevel;
  uint8 e_PrsOccasionType;
  uint8 e_SignalStrength;
  uint8 e_MeasState;

  /* Enum of type 'loc_lte_otdoa_PrsMeasProcErrorType' */
  uint8 e_ToaErrorStatus;

  /* MaxPeakSnr */
  uint16 w_MaxPeakSnr;

  /* PRS Gain Bias */
  int8 b_PrsGainBias;

  /* Noise floor associated with the meas. */
  FLT f_NoiseFloor;

  /* Flag to indicate if the meas. was done when a stronger offending
   * cell (same mod-6 value) was muted, and also the PCI of the offending
   * cell */
  boolean b_MeasWhenOffendingCellMuted;
  uint16 w_OffendingCellPci;
  uint32 w_OffendingCellEarfcn;

  uint8 u_ML1SchedFlags;
  FLT f_GlobalSNRdB;

  /* Carrier used for the measuremnt of this cell. It could be PCC, SCC1 or SCC2.
   * Range: 0 to 'LTE_LL1_CARRIER_COUNT-1'. As of now, 'LTE_LL1_CARRIER_COUNT' is defined as '3'
   * in 'modem_proc\fw_lte\api\intf_common.h' */
  uint8 u_Carrier; 

  /* PRS frequency meas. type. Info about whether the current cell was an intra-frequency meas.,
   * or inter-frequency meas. Of type 'loc_lte_otdoa_FreqMeasInfoType' */
  uint8 e_MeasType;

  /* OSTMR-VSRC info at reference instance (when RF was turned ON) */
  llomp_OstmrVsrcInfoTypeLog z_RefVsrcInfo;

  /* OSTMR-VSRC info at start of a PRS occasion */
  llomp_OstmrVsrcInfoTypeLog z_PrsOccVsrcInfo;

  /* Num hypothesis and best hyp */
  uint8 u_NumHypPerCell;
  uint8 u_BestHypIdx;

  /* DL ref time and PeakIndex within window (in Ts units) */
  int64 t_DlRefTime;
  FLT f_PeakIndex;

  /* Data for FFT residual signal detection. Valid only for TA modem */
  lloc_FftResidualSignalDetectionLogType z_FftResidualSigDet[LOC_LTE_OTDOA_MAX_HYPO_PER_CELL];

  /* Reserved fields */
  uint16 w_Reserved[LLOC_RESERVED_FIELDS_CNT];
}
lloc_PrsToaMeasLog;

/* global cell ID definition */
typedef PACKED struct PACKED_POST
{
  uint8 u_Mcc[3];
  uint8 u_Mnc[3];
  uint32 q_CellIdentity;
  uint8 u_NumOfMncDigits;
  uint8 u_NumOfCellIdBits;
}
lloc_LogGlobalCellIdType;

/* Meas. quality definition. All fields are encoded.  */
typedef PACKED struct PACKED_POST
{
  /* Bit-mapped validity.
   * Bit 0 represents validity of u_NumSamples */
  uint8 u_Valid;

  /* error resolution. Range ~ 0 to 3.
   * This field specifies the resolution R used in 'u_ErrorValue' field.
   * The encoding on two bits is as follows:
   * '00'      5  meters
   * '01'      10 meters
   * '10'      20 meters
   * '11'      30 meters.
   */
  uint8 u_ErrorResolution;

  /* error value. Range ~ 0 to 31.
   * This field specifies the target device’s best estimate of the uncertainty
   * of the OTDOA (or TOA) measurement.
   * The encoding on five bits is as follows:
   *   '00000' 0     to   (R*1-1) meters
   *   '00001' R*1   to   (R*2-1) meters
   *   '00010' R*2   to   (R*3-1) meters
   *   .... (and so on)
   *   '11111' R*31   meters or more;
   * where R is the resolution defined by error-Resolution field.
   * Eg., R=20m corresponds to 0-19m, 20-39,..., 620+ m.
   */
  uint8 u_ErrorValue;

  /* num. of samples used for estimation. Range ~ 0 to 7
   * If the error-Value field provides the sample uncertainty of the OTDOA (or TOA) measurement,
   * this field specifies how many measurements have been used by the target device to determine
   * this (i.e., sample size). Following 3 bit encoding is used:
   *   ‘000’   Not the baseline metric
   *   '001'   5-9
   *   '010'   10-14
   *   '011'   15-24
   *   '100'   25-34
   *   '101'   35-44
   *   '110'   45-54
   *   '111'   55 or more.
   * In case of the value ‘000’, the error-Value field contains the target device’s best estimate
   * of the uncertainty of the OTDOA (or TOA) measurement not based on the baseline metric.
   * E.g., other measurements such as signal-to-noise-ratio or signal strength can be utilized to
   * estimate the error-Value. If this field is absent, the value of this field is ‘000’.
   */
  uint8 u_ErrorNumSamples;
}
lloc_LogMeasQualityType;

/* RSTD info for cells */
typedef PACKED struct PACKED_POST
{
  /* Bit-mapped validity.
   * Bit 0 represents validity of this cell meas.
   * Bit 1 represents validity of q_SFN
   * Bit 2 represents validity of z_GlobalCellId
   * Bit 3 represents validity of w_Earfcn
   * Bit 4 represents validity of w_Rstd
   * Bit 5 represents validity of z_MeasQual */
  uint32 u_Valid;

  /* This field specifies the subframe number of the RSTD reference cell containing the
   * starting subframe of the PRS occasion during which the most recent neighbour cell
   * RSTD measurement was performed. Range ~ 0 to 1023 */
  uint16 w_Sfn;

  /* physical cell ID  Range ~ 0 to 503 */
  uint16 w_PhyCellId;

  /* This field specifies the ECGI, the globally unique identity of a cell in E-UTRA,
   * of the measured cell */
  lloc_LogGlobalCellIdType z_GlobalCellId;

  /* This field specifies the E-UTRA carrier frequency of the measured cell.
   * based on uint32 */
  uint32 w_Earfcn;

  /* RSTD value for neighbor cells. Ts units. Range ~ 0 to 12711 */
  uint16 w_Rstd;

  /* TDOA value with decimal precision */
  FLT f_TdoaTs;

  /* Time of arrival of the earliest arriving path (EAP). Units of Ts */
  int64 t_EapToa;

  /* measured signal to noise ratio. Unitless */
  uint16 w_EapSnr;
  uint16 w_MaxPeakSnr;

  /* Meas quality (uncertainity.) */
  lloc_LogMeasQualityType z_MeasQuality;

  /* PRS occasion seq number and Session counter */
  uint32 q_OccasionSeqNum;

  /* NC related */
  uint8 e_SignalStrength;
  uint8 e_MeasState;

  /* Reserved */
  uint16 w_Reserved[LLOC_RESERVED_FIELDS_CNT];
}
lloc_LogRstdInfoType;

/* RSTD meas. log packet */
typedef PACKED struct PACKED_POST
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  uint8 u_Version;

  /* Number of elements in measurement neighbor list Range 1 to 24 */
  uint8 u_NumNeighborMeasurements;

  /* reference cell meas. */
  lloc_LogRstdInfoType z_RefCellMeasurements;

  /* neighbor cell meas. */
  lloc_LogRstdInfoType z_NgbrCellMeasurements[LOC_LTE_OTDOA_MAX_NGBR_RSTD_RPT];

  /* error status */
  uint8 e_ErrorInfo;

  /* time-to-fix */
  uint32 q_Ttf;

  /* Config data */
  uint32 q_ConfigData;

  /* Session counter */
  uint32 q_LppSessionCtr;

  /* Reserved */
  uint16 w_Reserved[LLOC_RESERVED_FIELDS_CNT];
}
lloc_PrsRstdMeasLog;

/* log packet structure for storing hypothesis parameters */
typedef PACKED struct PACKED_POST
{
  /* Symbol offset index */
  int8 b_SymbolOffsetIndex;

  /* Prune window size */
  uint16 w_PruneWinSize;

  /* Prune window start index */
  uint16 w_PruneWinStartIndex;
}
lloc_LogHypoParamsType;

/* log packet structure for assitance data per cell */
typedef PACKED struct PACKED_POST
{
  /* physical cell ID  Range ~ 0 to 503 */
  uint16 w_PhyCellId;

  /* This field specifies the E-UTRA carrier frequency of the RSTD reference cell.
   * based on uint32 */
  uint32 w_Earfcn;

  /* Antenna port config */
  uint8 e_AntennaPortConfig;

  /* PRS bandwidth */
  uint8 e_PrsBandwidth;

  /* cyclic prefix length */
  uint8 e_CpLength;

  /* Configuration index */
  uint16 w_PrsConfigIndex;

  /* Number of downlink subframes */
  uint8 e_NumDlFrames;

  /* Muting pattern */
  /* index will be either 2, 4, 8 or 16. This represents the number of
   * bits valid in muting pattern */
  boolean b_MutingValid;
  uint8 e_MutingIndex;
  uint16 w_MutingPattern;

  /* expected RSTD */
  uint16 w_ExpectedRstd;

  /* exptected RSTD unc. */
  uint16 w_ExpectedRstdUnc;

  /* Slot number and Subframe offset (for inter-freq) */
  uint8 u_SlotNumOffset;
  uint16 w_PrsSubframeOffset;

  /* number of hypothesis */
  uint8 b_NumHyp;

  /* hypothesis parameters */
  lloc_LogHypoParamsType z_Hyp[ LOC_LTE_OTDOA_MAX_HYPO_PER_CELL ];

  /* Num of consecutive meas. to be done */
  uint8 u_NumOfConsecutiveMeas;

  /* Global cell ID */
  lloc_LogGlobalCellIdType z_GlobalCellId;

  /* Is strong enough to cause x-corr */
  boolean b_IsOffendingCell;

  /* PRS Gain Bias */
  int8 b_PrsGainBias;

  /* Is PRS enough to cause x-corr  */
  uint8 b_IsPrsIcOffendingCell;

  /* SNR associated with PRS IC*/
  FLT f_GlobalSNRdB;

  /* Reserved */
  uint16 w_Reserved[4];
}
lloc_LogAssistDataPerCell;

/* log packet for logging assistance data sent to ML1 */
typedef PACKED struct PACKED_POST
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  uint8 u_Version;

  /* Number of unique EARFCNs present in assitance data. Max 3 */
  uint16 w_NumFreqElems;

  /* Number of elements in measurement neighbor list Range 1 to 72 */
  uint8 u_NumNgbrs;

  /* session info sent by SM layer */
  uint8 e_OpMode;
  uint16 e_SessType;

  /* assistance data for reference cell */
  lloc_LogAssistDataPerCell z_RefCell;

  /* assistance data for ngbrs */
  lloc_LogAssistDataPerCell z_NgbrCell[ LOC_LTE_OTDOA_MAX_NGBR_CELLS ];

  /* Report type */
  uint8 e_LogType;

  /* Session counter */
  uint32 q_LppSessionCtr;

  /* Reserved */
  uint32 q_Mask;
  uint16 w_Reserved[LLOC_RESERVED_FIELDS_CNT];
}
lloc_PrsAssistDataLog;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

/* structure for PRS Override sub command */
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

  /* Non Coherent Integration Level */
  uint8   u_NCLevel;

  /* Intermediate Non Coherent logging enabled */
  uint8   b_NCLoggingEnabled;

  /* Enable NC for all the cells */
  uint8   b_EnableNcForAllCells;

  uint8   q_Data[LOC_LTE_OTDOA_MAX_DIAG_MSG_DATA_LENGTH / 4 ];
}
lloc_DiagNCCmdType;

typedef enum
{
  /* This is normal more of PRS operation. All cells will be searched only once. */
  LLOC_NORMAL_MODE,
  /* In this mode, all cells will be searched continuosly i.e. if we have reference cell 'R' and ngbr cells
   * 'A' 'B' 'C' and 'D', search pattern will be R A B C D R A B C D R A B C D...and so on (based on muting pattern).
   * Once all the cells have been measured, non-coherant integration will start for semi-weak cells */
  LLOC_CONT_SRCH_MODE,
  /* In this mode, all 503 physical cell IDs will be searched starting from #0. Assitance data is generated
   * internally */
  LLOC_PRS_SCAN_MODE,
  /* Similar to continuous search, but without non-coherant integration */
  LLOC_PRS_LOGGER_MODE
}
lloc_PrsSrchModeType;

typedef struct
{
  /* Flag to indicate whether to use PRS params given by server or
   * override them */
  boolean b_OverridePrsBw;
  boolean b_OverrideNprs;
  boolean b_OverrideIprs;

  /* PRS bandwidth */
  loc_lte_otdoa_PrsBandwidthType e_PrsBandwidth;

  /* Number of downlink subframes */
  loc_lte_otdoa_PrsDlSubframesType e_NumDlFrames;

  /* Configuration index */
  uint16 w_PrsConfigIndex;
}
lloc_PrsParamsType;

typedef struct
{
  uint32  w_Earfcn;
  FLT           f_EarfcnNoiseFloor;
  uint32        u_CodeIsolationLinear;
  loc_lte_otdoa_PrsBandwidthType  e_MaxPrsBandwidth;
}
lloc_NfXcorrPerEarfcn;

typedef struct
{
  uint32 q_OccasionSeqNum;
  uint8 u_NumOfMeas;
  uint8 u_NumOfMeasProcessed;
}
lloc_PrsOccasionMeasCntType;

/* Look-up table with mapping for 4Tx cells and EARFCN */
typedef struct
{
  boolean b_4TxAntPortCell;
  uint32 q_Earfcn;
} lloc_4TxAntPortFreqElemType;

typedef struct
{
  uint16 w_Cnt;
  lloc_4TxAntPortFreqElemType z_Table[LLOC_4TX_ANT_PORT_FREQ_LUT_SIZE];
} lloc_4TxAntPortAndFreqLutType;

/* Look-up table to remove multiple PRS subframe offsets of same EARFCN */
typedef struct
{
  uint16 w_Count;
  uint16 w_PrsSfOffset[LLOC_PRS_SF_OFFSET_LUT_SIZE];
  uint32 q_Earfcn[LLOC_PRS_SF_OFFSET_LUT_SIZE];
}
lloc_PrsSfOffsetLutType;

typedef struct
{
  uint16 w_PrsConfigIndex;
  loc_lte_otdoa_PrsDlSubframesType e_NumDlFrames;
  loc_lte_otdoa_PrsBandwidthType e_PrsBandwidth;
  lloc_PrsAntPortConfigType e_AntennaPortConfig;
} lloc_PrsConfigForScannerType;

/* control task parameters type */
typedef struct
{
  /* Error status of LLOC task */
  lloc_ErrorType e_ErrorStatus;
  loc_lte_otdoa_PrsMeasProcErrorType e_MPErrorStatus;

  /* Current state of Control task */
  lloc_StateType e_State;

  /* Previous state of Control task */
  lloc_StateType e_PrevState;

  /* Diag variables */
  boolean u_DiagInjectedAssistData;

  uint8 u_TimerMask;
  uint32 q_SMRptTimeout;

  /* serving cell info */
  loc_lte_otdoa_ServingCellInfoType z_ServingCellInfo;

  /* Basic aiding ref cell info */
  uint16 w_AidingRefCellId;
  uint32 w_AidingRefEarfcn;
  uint16 w_AidingRefIprs;

  /* search strategy */
  lloc_SrchStrategyType e_SrchStrategy;

  /* time stamp when start command received */
  qword t_StartTime;

  /* time stamp when report is sent to SM */
  qword t_RptTime;

  /* time-to-fix */
  uint32 q_Ttf;

  /* PRS search mode type */
  lloc_PrsSrchModeType e_PrsSrchMode;

  /* Cell ID index used only in PRS scanning mode */
  uint16 w_PrsScanCellIdCtr;

  /* New PRS parameters for override */
  lloc_PrsParamsType z_PrsParams;

  /* Boolean flag to force reporting aiding ref cell as ref cell in RSTP rpt */
  boolean b_ReportAidingRefCellAsRef;

  /* Enable or disable Non Coherent Integration */
  boolean b_NonCoh;
  boolean b_NCLoggingEnabled;

  /* Allowed NC Level */
  uint8 u_ValidNCLevel;

  /* Received UpdateTimeout cmd from SM (for OTDOA-only session) */
  boolean b_RxedUpdateTimeoutCmd;

  /* Adjust value for QoS timer in 100ms unit */
  int8 x_QosTimerAdj;

  /* Flag to indicate that TOA measurments from MeasProc has to be ignored since there was a serving
   * cell update */
  boolean b_IgnoreToaMeasFromMp;

  /* For serving cell update case, ignore the TOA uptil this occ seq number when the b_IgnoreToaMeasFromMp 
   * flag is set */
  uint32 q_IgnoreUptoOccasionSeqNum;

  /* Enable NC for all the cells */
  boolean b_EnableNcForAllCells;

  /* LPP Session counter */
  uint32 q_LppSessionCtr;

  /* Slow clk during PRS occasion type */
  qword t_PrsOccasionTime;

  /* Max. Tprs among all cells in AD */
  uint16 w_Tprs;

  /* List of Noise Floor and BW/code-isolation per EARFCN group */
  lloc_NfXcorrPerEarfcn z_NfXcorrPerEarfcn[LLOC_MAX_UNIQUE_EARFCNS];

  FLT f_PrsICGlobalNf;

  /* Enable/Disable offending cells computation */
  boolean b_IdentifyOffendingCells;

  /* Enable/Disable PRS Gain Bias computation */
  boolean b_ComputePrsGainBias;

  /* Num of Hyps Per Batch */
  uint8 u_HypsPerBatch;

  /* Num of Hyps Per Occ */
  uint8 u_HypsPerOcc;

  /* Keep track of number of meas. in a PRS occasion for 3 occasions */
  uint8 u_PrsOccMeasCntIdx;
  lloc_PrsOccasionMeasCntType z_PrsOccMeasCnt[LLOC_MEAS_CNT_FOR_PRS_OCCASIONS];

  /* Flag to skip 1st PRS occasion after handover */
  boolean b_SkipPrsOccasionAfterHo;

  /* Enable/Disable PRS IC offending cells computation */
  boolean b_IdentifyPRSICOffendingCells;

  /* Enable/Disable using of best ref cell TOA for RSTD computation for IF ngbr cells */
  boolean b_UseVarRefToaForIfNgbr;

  /* OTDOA SW version type */
  lloc_OtdoaLocSwType e_LocSwVer;

  /* Latest occasion sequence number from FW shared memory */
  uint32 q_CurrOccasionSeqNum;

  /* OTDOA SW version type */
  boolean b_4TxAntPortCell;
  lloc_4TxAntPortAndFreqLutType z_4TxAntDb;

  /* Look-up table to store PRS subframe offset for every EARFCN */
  lloc_PrsSfOffsetLutType z_PrsSfOffsetLut;

  /* Key AD parameters for PRS scanner */
  lloc_PrsConfigForScannerType z_PrsConfigScanner;

  /* Bitmask for LTE supported bands */
  uint64 t_LteSupportedBands[LLOC_LTE_BANDS_ARRAY_LEN];
}
lloc_ParamType;

typedef struct
{
  uint32 q_TimerId;
  os_TimerMsgType* p_Timer;
  boolean b_Status;
}
lloc_TimersType;

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/
 /*
  ******************************************************************************
  * Function: lloc_SendStop
  *
  * Description:
  *  Process transition to new state and stop ML1/MP as requested
  *
  * Parameters:
  *  b_ML1Stop - Indicate to send stop to ML1
  *  b_MPStop  - Indicate to send stop to MP
  *  e_State   - State to transition to
  *
  * Dependencies:
  *  None
  *
  * Return value:
  *  None.
  *
  ******************************************************************************
 */
void lloc_SendStop( boolean b_ML1Stop, boolean b_MPStop, lloc_StateType e_State );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_control_Init
 *
 * Description:
 *  Initialize all necessary parameters before the session begins
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
void loc_lte_otdoa_control_Init( void );


/*
 ******************************************************************************
 * Function: lloc_CreateTimers
 *
 * Description:
 *  Message processor
 *
 * Parameters:
 *  None
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  FALSE in case of any error
 *
 ******************************************************************************
 */
boolean lloc_CreateTimers(void);

/*
 ******************************************************************************
 * Function: lloc_ProcTimerExpiry
 *
 * Description:
 *  Process timer expiry for Control Task
 *
 * Parameters:
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
void lloc_ProcTimerExpiry(const uint32);

/*
 ******************************************************************************
 * Function: lloc_CopyAidingDataFromSm
 *
 * Description:
 *  Module to copy the aiding data sent by SM layer into local DB
 *
 * Parameters:
 *  Pointer to aiding data sent by ML1, pointer to local aiding data DB
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void lloc_CopyAidingDataFromSm
(
  const loc_lte_otdoa_SmStartCmdInfoType * p_SmStartCmdInfo
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_control_ProcApiMsg
 *
 * Description:
 *  Message processor
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
void loc_lte_otdoa_control_ProcApiMsg(const os_IpcMsgType *const p_Msg);

/*
 ******************************************************************************
 * Function: lloc_StopTimer
 *
 * Description:
 *  Stop a timer
 *
 * Parameters:
 *  None
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  FALSE in case of any error
 *
 ******************************************************************************
 */
boolean lloc_StopTimer( uint32 q_TimerId );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_control_PrsMeasProcComplete
 *
 * Description:
 *  API called by MeasProc task when it completes PRS meas. processing
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
extern void loc_lte_otdoa_control_PrsMeasProcComplete
(
  loc_lte_otdoa_control_PrsMeasProcCompleteInfoType *p_PrsMeasProcCompleteInfo
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_control_PrsHypothesesComplete
 *
 * Description:
 *  API called by MeasProc task when it completes PRS meas. processing of one
 *  hypotheses.
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
extern void loc_lte_otdoa_control_PrsHypothesesComplete
(
  loc_lte_otdoa_control_PrsHypothesesCompleteInfoType *p_PrsHypothesesCompleteInfo
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_control_StopMeasProcAck
 *
 * Description:
 *  API called by MeasProc task when it completes Stopo measurement
 *  processing command.
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
extern void loc_lte_otdoa_control_StopMeasProcAck
(
  loc_lte_otdoa_control_StopMeasProcAckInfoType *p_StopMeasProcAckInfo
);

extern boolean lloc_AidingDataSanityCheck( boolean b_InitialCheck );
extern void lloc_LogAidingData( uint8 u_LogType );
uint8 lloc_ConvertLteBwEnumToRb( loc_lte_otdoa_PrsBandwidthType e_EnumBw );
extern void lloc_GetLteSupportedBands( void ); 

#endif /* LOC_LTE_OTDOA_CONTROL_H */
