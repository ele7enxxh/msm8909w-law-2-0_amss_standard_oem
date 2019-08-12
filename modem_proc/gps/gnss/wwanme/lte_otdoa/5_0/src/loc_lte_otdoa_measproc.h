#ifndef LOC_LTE_OTDOA_MEASPROC_H
#define LOC_LTE_OTDOA_MEASPROC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        Location LTE OTDOA Measurements Processing Module Header File

GENERAL DESCRIPTION
This file contains API definitions between Location LTE OTDOA Measurements
Processing module files.

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/lte_otdoa/5_0/src/loc_lte_otdoa_measproc.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "loc_lte_otdoa_common_defs.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

/* There is a scaling factor for CER. It is 2^(N-4) where N=12 */
#define LLOMP_CER_SCALING_FACTOR    256.0

/* factor for converting integer to float (2^31) */
#define LLOMP_TWO_POW_31    2147483648

/* Invalid CER index. Used to detect error conditions */
#define LLOMP_CER_BAD_INDEX         0xFFFF

/* mask to extract only the least significant 12-bits from unpacked CER element */
#define LLOMP_EXTRACT_LS_12_BITS    0xFFF

/* Peak should be atleast 15dB above the noise floor. Hence, peak should be
 * atleast ( NoiseFloor * 10^(15/10) ) or above. 10^(1.5) is 31.62278 */
#define LLOMP_NOISE_FLOOR_THRES     31.62278

/* Early Peaks should be atleast 8 dB below the Peak value. Hence, Early peak should be
 * atleast ( PeakVal * 10^(-8/10) ) or above. 10^(-0.8) is 0.1585 */
#define LLOMP_EARLY_PEAK_THRES     0.1585

/* Early peak thresholds */
/* Early Peaks should be above N dB below the Peak value.
   Thresholds are defined as ( PeakVal * 10^(-N/10) ) or above.
   for ex 8 dB below the peak 10^(-0.8) is 0.1584893 */
/* 3 dB below peak value */
#define LLOMP_THRES_NEG_3_DB    0.5011872
/* 6 dB below peak value */
#define LLOMP_THRES_NEG_6_DB    0.2511886
/* 8 dB below peak value */
#define LLOMP_THRES_NEG_8_DB    0.1584893
/* 9 dB below peak value */
#define LLOMP_THRES_NEG_9_DB    0.1258925

/* Noise floor thresholds */
/* Good Peaks should have SNR N dB above the Noise floor
   SNR = 10log10(S/N) and SNR > 14 dB ---> 10log10(S/N) > 14 or S/N > 10^(1.4)
   for ex 14 dB above the peak 10^(1.4) is 25.1189 */
/* 8 dB below peak value */
#define LLOMP_THRES_14_DB       25.1189

/* 13.5 dB = 10^1.35 */
#define LLOMP_NOISE_FLOOR_THRESHOLD (22.387211385683401)

/* -14 dB = 10^(-1.4) */
#define LLOMP_PEAK_ENERGY_THRESHOLD              (0.039810717055350)
#define LLOMP_NORMALIZED_MASK_SAMPLE_MEDIAN      0.0016
#define LLOMP_MEDIAN_RATIO_SAMPLE_LENGTH         40
#define LLOMP_EEAP_MEDIAN_RATIO_MASK_SIZE        50
#define LLOMP_MEDIAN_RATIO_THRESHOLD             0.4

#define LLOC_TS_IN_US           0.0325520833

/* For a cell with 0 delay wrt serving cell, its peak is found at 1097
 * instead of 1024. This offset will take care of this. This is mainly due to the processing
 * delay. Note: disable hyp offset */
#define LLOMP_HYP_OFFSET  0

/* PI Value */
#define LLOMP_PI_VALUE                          3.14159265
#define LLOMP_LOG10_2                           0.30103

#define LLOMP_EAP_MASK_SIZE                     200
#define LLOMP_NOISE_FLOOR_EST_MEDIAN_SIZE       200
#define LLOMP_EEAP_NOISE_FLOOR_EST_MEDIAN_SIZE  256

/* CER range provided by FW can be in the range 0 to 2^32 - 1. But min. value is set to 1 to
 * avoid divide-by-0 errors */
#define LLOMP_ZERO_CER_INT_FORMAT               LLOMP_MIN_UINT32_CER
#define LLOMP_ZERO_CER_12UFL_FORMAT             0xF80

/* Min value of CER to be considered for noise floor. It should be 2^-18 or -54dB. This value in 
 * 12UFL is (255/2^26) or 0xD7F. This value in uint64 format is 0x1FE0 */
#define LLOMP_MIN_CER_FOR_NF_CALC               64

#define LOC_LTE_OTDOA_PRS_OCCASION_MEAS_LOG_VERSION     13

/* 12-bit CER vectors will be packed into 32-bit words */
#define LOC_LTE_OTDOA_PACKED_CER                      (0U<<0)

/* 12-bit CER vectors will occupy full 32-bit word */
#define LOC_LTE_OTDOA_UNPACKED_CER                    (1U<<0)

#define LOC_LTE_OTDOA_MAX_SNR_DB                    20.0

/* 11.11/LLOC_TS_IN_US i.e 341.299 */
#define LOC_LTE_OTDOA_ALIAS_POS_CP                  341
/* 22.22/LLOC_TS_IN_US i.e. 682.598 */
#define LOC_LTE_OTDOA_ALIAS_POS_EXTCP               683

/* Minimum SNR i.e. 0dB */
#define LLOMP_MIN_SNR           1.0

/* Range of average power (sigma squared) of PRS tones used for cancelling residual signal 
 * (error due to FFT truncation) */
#define LLOMP_PRS_TONES_POWER_RANGE_1 40
#define LLOMP_PRS_TONES_POWER_RANGE_2 60

/* Rollover value of LTE OSTMR. Same as 'LTE_MAX_OSTMR_COUNT' from
 * <mpss>\modem_proc\lte\ML1\l1_common\inc\lte_ml1_comdef.h */
#define LLOMP_OSTMR_ROLLOVER  0x40000000
#define LLOMP_TWO_POW_32 4294967296.0
#define LLOMP_TWO_POW_10 1024.0

/* Max tolerable diff of 'DL timestamp and total adjustments' between 2 CER vectors used
 * for NC integration */
#define LLOMP_MAX_NC_TOA_CORR_DIFF    20

/* Max. time difference between the 1st raw meas. for NC and current time instance (in Ts units). If
 * there is an entry in NC DB with time diff greater than this, then free the NC buffer. The set value is 
 * 2.5 secs in Ts units (2.5 * 30.72MHz) */
#define LLOMP_NC_INTEGRATION_MAX_TIMEDIFF 76800000

/* Scaling factor to determin the side band */
#define LLOMP_SCALE_FOR_SIDEBAND    3.0

/* Max number of SNR values to be used in measurement unc. table */
#define LLOMP_UNC_TABLE_MAX_SNR   40

/* Scaling factor for TTL for ATLAS modems */
#define LLOMP_TTL_SCALING_ATLAS 3.0

/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef enum
{
  LLOMP_DYN_MEM_CER_QUEUE,
  LLOMP_DYN_MEM_NC_INT
} llomp_DynMemModuleType;

/* LTE support bandwidths in RBs */
typedef enum
{
  /* 1.4MHz bandwidth */
  LLOMP_BW_6_RB = 6,
  /* 3MHz bandwidth */
  LLOMP_BW_15_RB = 15,
  /* 5MHz bandwidth */
  LLOMP_BW_25_RB = 25,
  /* 10MHz bandwidth */
  LLOMP_BW_50_RB = 50,
  /* 15MHz bandwidth */
  LLOMP_BW_75_RB = 75,
  /* 20MHz bandwidth */
  LLOMP_BW_100_RB = 100
}
llomp_LteBwRbType;

typedef enum
{
  LLOMP_UNKNOWN_MASK,
  LLOMP_NORMAL_MASK,
  LLOMP_RAISED_MASK,
  LLOMP_MAX_MASK
} llomp_EapMaskType;

typedef struct
{
  uint16  w_ArrPeakIndex[LOC_LTE_OTDOA_UNPACKED_PRS_CELL_SIZE_MAX];
          /* Array of peak indeices in CER vector */
  uint16  w_ArrCandidatePeakIndex[LLOMP_EAP_MASK_SIZE];
          /* Array of candidate EAP peaks indices */
  uint32  q_NoiseFloor; 
  uint64  t_SumCerEnergy;
  FLT     f_PeakEnergyThreshold; /* LLOMP_PEAK_ENERGY_THRESHOLD dB from the peak CER */
  FLT     f_NoiseFloorThreshold; /* LLOMP_NOISE_FLOOR_THRESHOLD above the noise floor */
  FLT     f_NormailizedMaskSampleMedian;
  uint16  w_NumValidCerTaps;
  uint16  w_MaskLen;
  uint16  w_PeakCount;
  uint16  w_MaxPeakCerIndex;
  uint16  w_CandidatePeakCount;
  uint16  w_EapIndex;
  uint8   u_MedianRatioSampleLength;
  FLT     f_MedianCompRatioThr;
  boolean b_EapValid;
  llomp_EapMaskType e_MaskType;
}llomp_EnhancedEapStatusType;

/* info sent by Ctrl task to start PRS peak processing */
typedef struct
{
  boolean b_NCEnabled;
  boolean b_NCLoggingEnabled;
  uint8 u_ValidNCLevel;
  uint32 q_LppSessionCtr;
}
loc_lte_otdoa_measproc_StartPrsMeasProcInfoType;

typedef struct
{
  /* Adjust value of FA threshold in DB, scaled-up by a factor of 10.
   * For example, -3.5dB will be -35. Range: -12.7dB to 12.8dB */
  int8 b_AdjDb;
}
loc_lte_otdoa_measproc_AdjustFaThreshInfoType;

typedef struct
{
  uint8 u_NumHypotheses;
  uint8 u_HypothesesSeqNum;
  loc_lte_otdoa_MeasBufType * p_MeasBuf;
}
loc_lte_otdoa_measproc_MeasResultsAvailInfoType;

/* info sent by Ctrl task to abort PRS peak processing */
typedef struct
{
  int16 dummy;
}
loc_lte_otdoa_measproc_StopPrsMeasProcInfoType;

typedef struct
{
  uint32 q_SequenceNum;
}
loc_lte_otdoa_measproc_FlushIpcQueueInfoType;

typedef struct
{
  boolean b_FreeMem;
}
loc_lte_otdoa_measproc_ResetNcDbType;

/* Processed results from processing CER vector of a hypothesis */
typedef struct
{
  /* Final CER value for this Hypotheses including EAP */
  FLT f_EapCer;

  /* Final CER Pos for this Hypotheses including EAP */
  uint16 w_EapCerPos;

  /* Final CER Pos for this Hypotheses including EAP */
  FLT f_EapCerPosFine;

  /* Max CER value for this Hypotheses */
  FLT f_MaxPeakCer;

  /* Index value for the max CER  */
  uint16 w_MaxPeakCerPos;

  FLT f_MedianCerEnergy;

  /*  EAP Parameters */
  /* Location */
  int16 x_Loc[2];

  /* Fine Location */
  FLT f_LocFine[2];

  /* Meas Quality */
  FLT f_PeakQual[2];

  /* Fine Peak Position in us */
  FLT f_PeakOffset[2];
}
llomp_EapInfoType;

/* Processed results from processing CER vector of a hypothesis */
typedef struct
{
  /* Physical Cell ID corresponding to the CER vector. Range: 0~503  */
  uint16 w_CellID;

  /* Cer information for the hypothesis */
  llomp_EapInfoType           z_EapInfo;

  llomp_EnhancedEapStatusType z_EnhancedEapInfo;

  /* Time of Arrival in Ts units based on the starting position of this
     occasion */
  int64 t_Toa;

  /* Hypotheses for this Cell */
  uint8 u_HypSeqNum;

  /* Window size for this Hypothesis */
  uint16 w_WinPruneSize;

  /* Window start index for this Hypothesis */
  uint16 w_WinStartIdx;

  /* Symbol Offset  */
  int8 b_SymOffset;

  /* Predicted Uncertainity Value for this Hypothesis in Ts  */
  FLT f_PredUnc;

  /* Signal to Noise Ratio for this Hypothesis using EAP CER*/
  FLT f_Snr;

  /* Signal to Noise Ratio for this Hypothesis using max peak */
  FLT f_MaxPeakSnr;

  /* S/S+N reported from FW for this Hypothesis in Q31 format */
  /* Chain 0 */
  uint32 q_FwSSNRx0;
  /* Chain 1*/
  uint32 q_FwSSNRx1;

  /*! the number of the prs subframe per occasion */
  loc_lte_otdoa_PrsDlSubframesType e_NumDlFrames;

  /* Cell Carrier frequency */
  uint32 w_Earfcn;

  /* Mark whether this hypotheses should be reported to MP Task */
  loc_lte_otdoa_PrsMeasProcErrorType e_ErrStatus;

  /* TTL Adjustment in Ts units */
  int16 x_TtlAdj;

  /* Accumulated COM (center of mass) Adjustment (Fixed Arithmetic Q6 format) */
  int32 l_ComAdj;

  /* MSTMR adjustment, in Ts units, Q0 format. */
  int64 t_MstmrAdj;

  /* Indicate if the Meas is NC Meas */
  boolean b_NCMeas;

  /* Non Coherent Integration Level */
  uint8 u_NCLevel;

  /* PRS occasion type */
  lloc_PrsOccasionType e_PrsOccasionType;

  /* Noise floor associated with the meas. */
  FLT f_NoiseFloor;

  /* PRS Gain Bias */
  int8 b_PrsGainBias;

  /* Flag to indicate if the meas. was done when a stronger offending
   * cell (same mod-6 value) was muted, and also the PCI of the offending
   * cell */
  boolean b_MeasWhenOffendingCellMuted;
  uint16 w_OffendingCellPci;
  uint32 w_OffendingCellEarfcn;

  uint8 u_ML1SchedFlags;

  /* Variables to store the CER energy for sigma^2=40 and 60 resp. (used for logging
   * purposes for residual signal cancellation from FFT). Valid only for TA modem */
  lloc_FftResidualSignalDetectionType z_FftResidualSigDet;

  /* TOA adjustment for NC2 CER vector */
  int16 x_ToaAdjForNc;
}
llomp_HypResultsDb;

/* Structure with the results of a cell after doing multi
   timing hypothesis computation */
typedef struct
{
  FLT f_FinalPeakQual;         /* Final Peak value */
  FLT f_FinalPeakOffset;       /* Final TOA value in us */
  FLT f_MeasError;             // Measurement error in LLOC_TS_UNIT
  uint8 u_MeasErrResolution;       // Measurement error Resolution (meters) {5, 10, 20, 30}
  uint8 u_ResDbIdx;
}
llomp_FinalCellResultsType;

typedef struct
{
  int64 r_DlTime;
  uint16 w_CellID;
  uint32 w_Earfcn;
  uint8 u_NCLevel;
  uint16 w_WinPruneSize;
  int16 x_NcCerAdj;
  int32 l_DlPlusAdjBaseCer;
  lloc_PrsOccasionType e_PrsOccasionType;
  int8 u_GainBias;
  uint8 b_PRSICApplied;
  uint64 t_IntCer[LOC_LTE_OTDOA_IFFT_SIZE];
}
llomp_NonCohResType;

typedef struct
{
  boolean b_InUse;
  boolean b_MemAllocated;
  llomp_NonCohResType *p_Ptr;
}
llomp_NonCohDynMemDbPtrType;

typedef struct
{
  uint16 w_NumMemAllocated;
  llomp_NonCohDynMemDbPtrType z_PtrDb[LOC_LTE_OTDOA_MAX_NON_COH_CELLS];
}
llomp_NonCohDbType;

typedef struct
{
  /* Error status of LLOMP task */
  loc_lte_otdoa_PrsMeasProcErrorType e_ErrorStatus;

  /* Number of Hyp with errors */
  uint8 u_NumErrHyp;

  /* Hyp seq no that is being handled by MP */
  uint8 u_HypSeqNum;

  /* Cell id being processed */
  uint16 w_CellId;

  /* EARFCN of cell being processed */
  uint32 w_Earfcn;

  /* Total Num of Hyp for this Cell */
  uint8 u_NumHyp;

  /* Occasion no that is being handled by MP */
  uint32 q_OccasionSeqNum;

  /* Cyclic Prefix */
  loc_lte_otdoa_CpModeType e_CpLength;

  /* flag to indicate if CER is packed or unpacked in shared memory */
  boolean b_FlagPackedCer;

  /* Buffer to store Results */
  llomp_HypResultsDb z_HypResDb[LOC_LTE_OTDOA_MAX_HYPO_PER_CELL];

  /* Enable or disable Non Coherent Integration */
  boolean b_NonCoh;
  boolean b_NCLoggingEnabled;
  boolean u_ValidNCLevel;

  /* Session counter */
  uint32 q_LppSessionCtr;

  /* False alarm threshold adjust value given by Control module. In real units, not in dB */
  FLT f_FaThreshAdj;

  /* False alarm threshold adjust value as a workaround for FFT residual signal. It is calculated for every hypothesis */
  FLT f_FaThreshAdjFftResd;

  /* Counters for dynamic mem alloc debug */
  uint32 q_AllocMemCtr;
  uint32 q_FreeMemCtr;
  uint32 q_AllocMemFailCtr;
}
llomp_ParamType;

typedef struct
{
  FLT f_Value[LOC_LTE_OTDOA_MAX_HYPO_PER_CELL];
  uint8 u_Cnt;
}
llomp_vectorType;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

/* cell-specific PRS Meas descriptor */
typedef PACKED struct PACKED_POST
{
  /* Physical Cell ID corresponding to the CER vector.
   * Range: 0~503
   */
  uint16  w_CerCellId;

  /* Symbol Tap Offset */
  int8 b_SymbolOffset;

  /* Start Tap Index in Ts [0-2048] */
  uint16 w_WinStartIdx;

  /* PRS Search Window Prune Size in Ts [0-2048]*/
  uint16 w_WinPruneSize;

  /* S/S+N Ratio reported from FW for this Hypothesis in Q31 format */
  /* Chain 0 */
  uint32 q_AvgSSNRx0;
  /* Chain 1*/
  uint32 q_AvgSSNRx1;

  /* The number of the prs subframe per occasion used by FW */
  uint8 e_NumDlFrames;

  /* Cell Carrier frequency - based on uint32 */
  uint32 w_Earfcn;

  /* PRS Gain Bias */
  int8 b_PrsGainBias;

  /* Flag to indicate if the meas. was done when a stronger offending
   * cell (same mod-6 value) was muted, and also the PCI of the offending
   * cell */
  boolean b_MeasWhenOffendingCellMuted;
  uint16 w_OffendingCellPci;
  uint32 w_OffendingCellEarfcn;

  uint8 u_ML1SchedFlags;

  /* PRS BW */
  uint8 u_PrsBw;
}
loc_lte_otdoa_PrsCellDescLog;

/* occasion-specific PRS Meas descriptor */
typedef PACKED struct PACKED_POST
{
  /* LTE downlink timeStamp aligned to frame boundary of PRS Occasion,
   * time read from 32-bit VSRC sample counter.
   * (alternatively 64-bit DL Link counter)
   */
  int64  r_DlTimestamp;

  /* Accumulated TTL Adjustment */
  int16  x_TtlAdj;

  /* cell ID of the serving sector in this PRS occasion */
  uint16  w_ServCellId;

  /* PRS Meas Result Sequence Number. This will be used
   * by GNSS SW to check if the data they read have been
   * overwritten or not.
   */
  uint32 q_OccasionSeqNum;

  /* The number of hypothesis in this PRS occasion, including other cells in this PRS occasion */
  uint8 u_NumHypInOccasion;

  /* Header containing relevant info for PRS measurement vector */
  loc_lte_otdoa_PrsCellDescLog z_CellInfo;

  /* LTE system BW in RBs */
  uint8 u_LteSysBw;

  /* Accumulated COM (center of mass) Adjustment (Fixed Arithmetic Q6 format) */
  int32 l_ComAdj;

  /* Accumulated MSTMR adjustment, in Ts units, Q0 format. */
  int64 t_MstmrAdj;

  /* SFN of the measured PRS occasion */
  uint16 w_Sfn;

  /* NC related info */
  uint8 u_NCLevel;
  uint8 e_MeasState;
  uint8 e_PrsOccasionType;

  /* Carrier used for the measuremnt of this cell. It could be PCC, SCC1 or SCC2.
   * Range: 0 to 'LTE_LL1_CARRIER_COUNT-1'. As of now, 'LTE_LL1_CARRIER_COUNT' is defined as '3'
   * in 'modem_proc\fw_lte\api\intf_common.h' */
  uint8 u_Carrier; 

  /* PRS frequency meas. type. Info about whether the current cell was an intra-frequency meas.,
   * or inter-frequency meas. Of type 'loc_lte_otdoa_FreqMeasInfoType' */
  uint8 e_MeasType;

  /* OSTMR-VSRC info at reference instance (when RF was turned ON). We have array of 3, for
   * each carrier. I.e. PCC, SCC1, SCC2 */
  llomp_OstmrVsrcInfoTypeLog z_RefVsrcInfo[LLOC_MAX_LTE_CARRIERS];

  /* OSTMR-VSRC info at start of a PRS occasion. We have array of 3, for
   * each carrier. I.e. PCC, SCC1, SCC2 */
  llomp_OstmrVsrcInfoTypeLog z_PrsOccVsrcInfo[LLOC_MAX_LTE_CARRIERS];
}
loc_lte_otdoa_PrsMeasOccasionDescLog;

/* PRS occasion meas. log packet */
typedef PACKED struct PACKED_POST
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  uint8 u_Version;

  /* Hypothesis seq num of this cell and Num of hyp for this cell */
  uint8 u_HypothesesSeqNum;
  uint8 u_NumHypForCell;

  /* configuration vector */
  uint32 q_ConfigData;

  /* PRS Occasion Meas Descriptor */
  loc_lte_otdoa_PrsMeasOccasionDescLog z_OccasionInfo;

  /* Session counter */
  uint32 q_LppSessionCtr;

  /* Reserved fields */
  uint16 w_Reserved[LLOC_RESERVED_FIELDS_CNT];

  /* Individual Cell Data */
  uint8 u_RawCer[1];
}
loc_lte_otdoa_PrsOccasionMeasLog;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_measproc_ProcCustomMsg
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
extern void loc_lte_otdoa_measproc_ProcCustomMsg ( const os_IpcMsgType *const p_Msg );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_measproc_Init
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
extern void loc_lte_otdoa_measproc_Init( void );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_measproc_StartPrsMeasProcessing
 *
 * Description:
 *  API called by Ctrl task to initiate PRS meas. processing operation
 *
 * Parameters:
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
extern void loc_lte_otdoa_measproc_StartPrsMeasProcessing \
        ( loc_lte_otdoa_measproc_StartPrsMeasProcInfoType *p_StartPrsMeasProcInfo );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_measproc_MeasResultsAvail
 *
 * Description:
 *  API called by Ctrl task to start PRS meas. processing of the acquired
 *  search measurements results.
 *
 * Parameters:
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
extern void loc_lte_otdoa_measproc_MeasResultsAvail\
        ( loc_lte_otdoa_measproc_MeasResultsAvailInfoType *p_MeasResultsAvailInfo );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_measproc_StopPrsMeasProcessing
 *
 * Description:
 *  API called by Ctrl task to abort PRS meas. processing
 *
 * Parameters:
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
extern void loc_lte_otdoa_measproc_StopPrsMeasProcessing\
        ( loc_lte_otdoa_measproc_StopPrsMeasProcInfoType *p_StopPrsMeasProcInfo );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_measproc_FlushIpcQueue
 *
 * Description:
 *  API called by MeasProc task by itself to check if there is anything else
 *  waiting on the IPC queue before continuing with lower priority activities.
 *
 * Parameters:
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
extern void loc_lte_otdoa_measproc_FlushIpcQueue\
        ( loc_lte_otdoa_measproc_FlushIpcQueueInfoType *p_FlushIpcQueueInfo );

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_measproc_ProcMeasResultsAvail
 *
 * Description:
 *
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
extern void loc_lte_otdoa_measproc_ProcMeasResultsAvail
(
  const loc_lte_otdoa_measproc_MeasResultsAvailInfoType * p_MeasResults
);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_CalcFalseThres
 *
 * Description:
 *  Based on the N_PRS value, determine the False Alarm threshold values
 *
 * Parameters:
 *  e_DLFrames - N_PRS (Num DL Frames) Value
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  f_FalseThres - False threshold value in float
 *
 ******************************************************************************
 */

extern FLT loc_lte_otdoa_CalcFalseThres
(
   loc_lte_otdoa_PrsBandwidthType e_PrsBandwidth,
   loc_lte_otdoa_PrsDlSubframesType e_DLFrames,
   uint8 u_NcLevel
);

extern void loc_lte_otdoa_measproc_NcResetDb
(
   loc_lte_otdoa_measproc_ResetNcDbType *p_NcResetDbInfo
);

extern void loc_lte_otdoa_measproc_AdjustFaThresh
(
  loc_lte_otdoa_measproc_AdjustFaThreshInfoType *p_AdjustFaThreshInfo
);

extern uint8 llomp_GetPerBwResolutionScale
(
  loc_lte_otdoa_PrsBandwidthType u_PrsBw
);

extern uint16 llomp_GetPerBwMinWindowSize
(
  loc_lte_otdoa_PrsBandwidthType u_PrsBw
);

extern uint16 llomp_GetPerBwIfftSize
(
  loc_lte_otdoa_PrsBandwidthType u_PrsBw
);

extern uint8 llomp_GetPerBwShiftValue
(
  loc_lte_otdoa_PrsBandwidthType u_PrsBw
);

extern boolean llomp_MemAlloc( void **p_MemPtr, size_t w_MemSize, llomp_DynMemModuleType e_Module );
extern void llomp_MemFree( void **p_MemPtrToPtr, llomp_DynMemModuleType e_Module );

#endif /* LOC_LTE_OTDOA_MEASPROC_H */