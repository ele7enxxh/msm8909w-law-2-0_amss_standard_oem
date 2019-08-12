#ifndef LOC_LTE_OTDOA_COMMON_DEFS_H
#define LOC_LTE_OTDOA_COMMON_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Location LTE OTDOA Common Definitions Header File

GENERAL DESCRIPTION
This file contains common definitions shared between all Location LTE OTDOA
module files.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wwanme/lte_otdoa/5_0/src/loc_lte_otdoa_common_defs.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "wwanme_common_defs.h"

#include "loc_lte_otdoa_api.h"
#include "loc_lte_otdoa_gnss_api.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* Macro to enable debug messaging */
//#define OTDOA_DEBUG

/* maximum neighbor cells that can be searched */
#define LOC_LTE_OTDOA_MAX_NGBR_CELLS            72
#define LOC_LTE_OTDOA_MAX_NGBR_CELLS_PER_FREQ   SM_LOC_LTE_OTDOA_MAX_NGBR_CELLS
#define LOC_LTE_OTDOA_MAX_NGBR_RSTD_RPT         72 /* 24 */

/* maximum num .of hypothesis that can be searched for a cell (based on exp. RSTD and unc. */
#define LOC_LTE_OTDOA_MAX_HYPO_PER_CELL            4

#define FEATURE_LOC_LTE_OTDOA_DYNAMIC_MEM          1

#ifdef FEATURE_LOC_LTE_OTDOA_DYNAMIC_MEM
/* Buffers enough to handle atleast 3 measurements */
#define LOC_LTE_OTDOA_CMD_QUEUE_SIZE               3
#define LOC_LTE_OTDOA_DYN_MEM_QUEUE_SIZE           LOC_LTE_OTDOA_MAX_HYPO_SRCH_PER_OCCASION*2
#else
/* Buffers enough to handle atleast two occasions */
#define LOC_LTE_OTDOA_CMD_QUEUE_SIZE            LOC_LTE_OTDOA_MAX_HYPO_SRCH_PER_OCCASION*2
#endif

#define LOC_LTE_OTDOA_IFFT_SIZE_256             256
#define LOC_LTE_OTDOA_IFFT_SIZE_512             512
#define LOC_LTE_OTDOA_IFFT_SIZE_1024            1024
#define LOC_LTE_OTDOA_IFFT_SIZE_2048            2048
#define LOC_LTE_OTDOA_IFFT_SIZE                 2048
#define LOC_LTE_OTDOA_FIND_MIN(a,b)             (((a)<(b))?(a):(b))
#define LOC_LTE_OTDOA_FIND_MAX(a,b)             (((a)>(b))?(a):(b))
#define LOC_LTE_OTDOA_UNPACKED_PRS_CELL_SIZE_MAX    LOC_LTE_OTDOA_IFFT_SIZE
#define LOC_LTE_OTDOA_PRS_NUM_OF_SUBFRAMES_MAX   6
#define LOC_LTE_OTDOA_SYMBOL_SIZE               LOC_LTE_OTDOA_IFFT_SIZE
#define LOC_LTE_OTDOA_ZERO_EXP_RSTD             8192
#define LLOC_DEFAULT_1HYP_EXP_RSTD_UNC          335
#define LLOC_DEFAULT_ALL_HYP_EXP_RSTD_UNC       1023

/* 160ms / 1Ts = 0.16 * 2048 * 15000 */
#define LLOC_T_PRS_160_MS_IN_TS               4915200

/* 1ms / 1Ts = 0.001 * 2048 * 15000 */
#define LLOC_T_PRS_1_MS_IN_TS                 30720

/* Bit masks for logging. Use 3 MSBs to indicate modem type */
#define LLOC_NIKEL_MODEM                       (0<<29)
#define LLOC_DIME_MODEM                        (1<<29)
#define LLOC_BOLT_MODEM                        (2<<29)
#define LLOC_TRITON_MODEM                      (3<<29)
#define LLOC_TABASCO_MODEM                     (4<<29)
#define LLOC_THOR_MODEM                        (5<<29)
#define LLOC_ATLAS_MODEM                       (6<<29)
#define LLOC_JOLOKIA_MODEM                     (7<<29)
#define LLOC_WIN_ADJ_VALID                     (1U<<28)
#define LLOC_WIN_ADJ_INVALID                   (0U<<28)
#define LLOC_MAX_PEAK_SNR_VALID                (1U<<27)
#define LLOC_NC_MEASUREMENT_LOG                (1U<<26)
#define LLOC_MEAS_STATUS_VALID                 (1U<<25)
#define LLOC_SIGNAL_STRENGTH_VALID             (1U<<24)
#define LLOC_PETL_VALID                        (1U<<23)
#define LLOC_PETL_INVALID                      (0U<<23)
#define LLOC_REFTIME_VALID                     (1U<<20)
#define LLOC_VSRC_OSTMR_DIFF_VALID             (1U<<18)
#define LLOC_RSTD_EARLY_RPT                    (1U<<0)
#define LLOC_TOA_ADJ_MIXED_BW_VALID            (1U<<17)
#define LLOC_REF_TOA_FOR_RSTD_VALID            (1U<<1)

#define LOC_LTE_OTDOA_MAX_NON_COH_CELLS         20

#define LOC_LTE_OTDOA_NULL_NC_LEVEL             0
#define LOC_LTE_OTDOA_MIN_NC_LEVEL              1
#define LOC_LTE_OTDOA_MISSION_MODE_NC_LEVEL     2

#define LLOC_RESERVED_FIELDS_CNT                8

#define LOC_LTE_TDP_MAX_UE_RX_ANT               2

#define LOC_LTE_TDP_OPCRS_SERV_CER_MAX_SIZE     512
#define LOC_LTE_TDP_OPCRS_NGBR_CER_MAX_SIZE     32

#define LOC_LTE_TDP_NUM_SUBFRAMES_PER_SYSFRAME  10

#define LOC_LTE_TDP_CONVERT_NSEC_TO_K_CONST_C    8.4

#define LOC_LTE_TDP_CONVERT_NSEC_TO_K_CONST_LOGN_ONE_PLUS_X    0.112435

#define LOC_LTE_TDP_MAX_K                       (uint8)63

#define LOC_LTE_TDP_MAX_MEAS_AGE_MSECS           5000

#define UNKNOWN_TOTMS                            0xFFFF

#define UNKNOWN_RXTX_TIMEDIFF                    0xFFFF

#define LOC_LTE_TDP_SMALL_FLOAT                  (1e-10)

#define LOC_LTE_TDP_3SGIMA_TO_1SIGMA_1D_SCALE_FACTOR      (1/2.57)

#define LLOMP_MIN_UINT32_CER                    1

#define LLOC_SRCH_SIZE_BUFFER                   300

/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/* Error codes reported by Meas Proc Task to Ctrl task */
typedef enum
{
  LLOMP_NO_ERR = 1,
  LLOMP_NO_ERR_NO_PEAK,
  LLOMP_ERR_PEAK_INDEX_OUT_OF_BOUNDS,
  LLOMP_ERR_INVALID_CELLID,
  LLOMP_ERR_INVALID_HYP,
  LLOMP_ERR_NULL_HYP_SEQ_NUM,
  LLOMP_ERR_INVALID_HYP_SEQ_NUM,
  LLOMP_ERR_NULL_CER_MEMORY,
  LLOMP_ERR_PARTIAL_MEAS,
  LLOMP_ERR_NO_MEAS,
  LLOMP_ERR_GENERAL,
  LLOMP_ERR_MAX
}
loc_lte_otdoa_PrsMeasProcErrorType;

/* PRS number of downlink subframes */
typedef enum
{
  LLOC_PRS_1_SF = 0,  /* 1 PRS subframe in PRS occasion */
  LLOC_PRS_2_SF,      /* 2 PRS subframe in PRS occasion */
  LLOC_PRS_4_SF,      /* 4 PRS subframe in PRS occasion */
  LLOC_PRS_6_SF,      /* 6 PRS subframe in PRS occasion */
  LLOC_PRS_MAX_SF     /* Maximum PRS subframes */
}
loc_lte_otdoa_PrsDlSubframesType;

/* Measurement status of a cell */
typedef enum
{
  /* cell info sent to ML1 */
  LLOC_NOT_MEASURED = 1,
  /* cell meas. received from ML1 */
  LLOC_MEASURED,
  /* cell meas. processed by MP task */
  LLOC_PROCESSED,
  /* cell meas are remeasured */
  LLOC_REMEAS,
  /* cell meas are in Non Coherent State*/
  LLOC_NONCOH,
  /* assistance data error. Cell not searched */
  LLOC_ERR_NOT_SEARCHED
}
lloc_MeasStatusType;

/* PRS occasion type */
typedef enum
{
  LLOC_EVEN_PRS_OCCASION,
  LLOC_ODD_PRS_OCCASION,
  LLOC_UNKNOWN_PRS_OCCASION,
  LLOC_ALL_OCCASION_TYPE_BITMASK = 3,
}
lloc_PrsOccasionType;

/* OTDOA SW type */
typedef enum
{
  LOC_LTE_OTDOA_1_0,
  LOC_LTE_OTDOA_2_0,
  LOC_LTE_OTDOA_3_0,
  LOC_LTE_OTDOA_3_1,
  LOC_LTE_OTDOA_4_0,
  LOC_LTE_OTDOA_5_0,
}
lloc_OtdoaLocSwType;

/* RSTD info */
typedef struct
{
  uint8 u_SlotNumOffset;
  uint16 w_PrsSubframeOffset;
  uint16 w_ExpectedRstd;
  uint16 w_ExpectedRstdUnc;
}
lloc_AidingRstdInfoType;

/* cell-specific PRS Meas descriptor */
typedef struct
{
  /* Physical Cell ID corresponding to the CER vector. Range: 0~503  */
  uint16  w_CellID;

  /* Symbol Tap Offset */
  int8 b_SymOffset;

  /* Start Tap Index in Ts [0-2048] */
  uint16 w_WinStartIdx;

  /* PRS Search Window Prune Size in Ts [0-2048]*/
  uint16 w_WinPruneSize;

  /* S/S+N data for Rx 0 in Q31 format */
  uint32 q_AvgSSNRx0;

  /* S/S+N data for Rx 1 in Q31 format*/
  uint32 q_AvgSSNRx1;

  /*! the number of the prs subframe per occasion */
  loc_lte_otdoa_PrsDlSubframesType e_NumDlFrames;

  /* Cell Carrier frequency */
  uint32 w_Earfcn;

  /* Memory offset for raw CER vectors */
  uint16 w_MemOffset;

  /* PRS Gain Bias */
  int8 b_PrsGainBias;

  /* MaxPeak CER used to calculate the PRS gain bias. It will be used for CER saturation
   * detection algorithm */
  uint32 q_MaxPeakCerForGain;
}
loc_lte_otdoa_CellDescType;

typedef struct
{
  /*  LTE downlink timeStamp aligned to frame boundary of PRS Occasion,
      time read from 32-bit VSRC sample counter.
      (alternatively 64-bit DL Link counter)
  */
  int64  r_DlTimestamp;

  /* Accumulated TTL Adjustment */
  int16  x_TtlAdj[LOC_LTE_OTDOA_PRS_NUM_OF_SUBFRAMES_MAX];

  /* Accumulated COM (center of mass) Adjustment (Fixed Arithmetic Q6 format) */
  int32 l_ComAdj[LOC_LTE_OTDOA_PRS_NUM_OF_SUBFRAMES_MAX];

  /* Accumulated MSTMR adjustment, in Ts units, Q0 format. */
  int64 t_MstmrAdj[LOC_LTE_OTDOA_PRS_NUM_OF_SUBFRAMES_MAX];
  int64 t_MstmrAdjOffsetLogging[LOC_LTE_OTDOA_PRS_NUM_OF_SUBFRAMES_MAX];

  /* cell ID of the serving sector in this PRS occasion */
  uint16  w_ServCellID;

  /* Cyclic Prefix */
  loc_lte_otdoa_CpModeType e_CpLength;

  /* PRS occasion type */
  lloc_PrsAntPortConfigType e_NumTxAnt;

  /* PRS Bandwidth */
  loc_lte_otdoa_PrsBandwidthType e_PrsBandwidth;

  /* LTE system BW of primary carrier (PCC) and system BW used for PRS */
  loc_lte_otdoa_PrsBandwidthType e_LteSysBwForPcc;
  loc_lte_otdoa_PrsBandwidthType e_LteSysBwUsedForPrs;
  loc_lte_otdoa_PrsBandwidthType e_LteSysBwForCalib;
  
  /* RSTD Information */
  lloc_AidingRstdInfoType z_RstdInfo;

  /* sequence num. of this PRS occasion */
  uint32 q_OccasionSeqNum;

  /* num. of cells in this PRS occasion */
  uint8 u_NumCell;

  /* Header containing relevant info for PRS measurement vector */
  loc_lte_otdoa_CellDescType z_CellInfo;

  /* SFN value during the occasion when this cell was measured */
  uint16 w_Sfn;

  /* Indicate if the Meas is NC Meas */
  boolean b_NCMeas;

  /* NC Level */
  uint8 u_NCLevel;

  /* Meas status */
  lloc_MeasStatusType e_MeasState;

  /* PRS occasion type */
  lloc_PrsOccasionType e_PrsOccasionType;

  /* Flag to indicate if the meas. was done when a stronger offending
   * cell (same mod-6 value) was muted, and also the PCI of the offending
   * cell */
  boolean b_MeasWhenOffendingCellMuted;
  uint16 w_OffendingCellPci;
  uint32 w_OffendingCellEarfcn;

  uint8 u_ML1SchedFlags;

  /* OSTMR-VSRC info at reference instance (when RF was turned ON). We have array of 3, for
   * each carrier. I.e. PCC, SCC1, SCC2 */
  loc_lte_otdoa_OstmrVsrcInfoType z_RefVsrcInfo[LLOC_MAX_LTE_CARRIERS];

  /* OSTMR-VSRC info at start of a PRS occasion. We have array of 3, for
   * each carrier. I.e. PCC, SCC1, SCC2 */
  loc_lte_otdoa_OstmrVsrcInfoType z_PrsOccVsrcInfo[LLOC_MAX_LTE_CARRIERS];

  /* Carrier used for the measuremnt of this cell. It could be PCC, SCC1 or SCC2.
   * Range: 0 to 'LTE_LL1_CARRIER_COUNT-1'. As of now, 'LTE_LL1_CARRIER_COUNT' is defined as '3'
   * in 'modem_proc\fw_lte\api\intf_common.h' */
  uint8 u_Carrier;

  /* PRS frequency meas. type. Info about whether the current cell was an intra-frequency meas.,
   * or inter-frequency meas. */
  loc_lte_otdoa_FreqMeasInfoType e_MeasType;

  /*! SF Info Debug parameters */
  #ifdef PRS_SFINO_DBG_LOGGING
  int64 dl_subframe_ref_time; ///< ref time at start of DL subframe
  int64 dl_subfn_ref_time_raw;///< ref time without mstmr correction
  int64 dl_subframe_ref_time_offline;
                              ///< ref time at start of DL subframe until
                              ///< sample recording is ON = dl_subframe_ref_time
                              ///< Once sample recording is frozen,
                              ///< dl_subframe_ref_time_offline is also frozen.

  int64 dl_subframe_ext_mstmr;///< unwrapped MSTMR count at DL subframe
  int64 dl_subframe_rtc;      ///< ref time without TTL correction and mstmr correction
  uint32 dl_subframe_cnt;     ///< subframe counter
  int32 dl_subframe_mstmr;    ///< O_STMR MSTMR count at start of DL subframe
  uint32 dl_sf_univ_stmr_cnt; ///< UNIV_STMR MSTMR count at start of DL subframe
  int32 dl_subframe_sr_addr;  ///< SR addr at start of DL subframe
  int32 ul_subframe_mstmr;    ///< O_STMR MSTMR count at start of UL subframe
  uint32 ul_sf_univ_stmr_cnt; ///< UNIV_STMR MSTMR count at start of UL subframe
  int16 subframe_num;         ///< subframe number
  int16 frame_num;            ///< radio frame number
  uint16 subframe_frame_num;  ///< bits 0~3  = subframe number
  #endif //PRS_SFINO_DBG_LOGGING

  /* Individual Cell Data */
  uint32 q_Cer[ LOC_LTE_OTDOA_UNPACKED_PRS_CELL_SIZE_MAX ];
}
loc_lte_otdoa_MeasResultsType;

/* Memory type */
typedef enum
{
  LLOC_MEMORY_MIN = 0,
  LLOC_MEMORY_HEAP,
  LLOC_MEMORY_QUEUE,
  LLOC_MEMORY_MAX,
}
lloc_otdoa_MemoryType;

typedef struct
{
  q_link_type                   z_Link;
  loc_lte_otdoa_MeasResultsType z_Meas;
  boolean                       b_InUse;
  lloc_otdoa_MemoryType         e_MemoryType;
}
loc_lte_otdoa_MeasBufType;

typedef struct
{
  uint32                        q_NumElemsAllocated;
  loc_lte_otdoa_MeasBufType *   p_PrsMeasResultsDynMemBuf;
}
loc_lte_otdoa_PrsMeasResultsDynMemBufType;

/* Structure to store parameters for FFT residual signal detection. Valid only for TA */
typedef struct
{
  FLT f_FaThresUpdate;
  uint32 q_NoiseFloor;
  FLT f_Sigma40;
  FLT f_Sigma60;
}
lloc_FftResidualSignalDetectionType;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

/* Structure to send OSTMR and VSRC values. We have array of [2] for Rx0 and Rx1
 * antennas */
typedef PACKED struct PACKED_POST
{
  boolean  b_Valid;
  uint16  w_Sfn;
  uint64  t_VsrcFromFW[2];
  uint32  q_OstmrFromFW[2];
  uint32  q_OstmrPhFromFW[2];
  uint64  t_RefDlTime;
}
llomp_OstmrVsrcInfoTypeLog;

typedef struct
{
  /* PRS Bandwidth */
  loc_lte_otdoa_PrsBandwidthType e_PrsBandwidth;

  /* Number Of RBs */
  uint8 u_NumRBs;

  /* Max Nprs Allowed */
  loc_lte_otdoa_PrsDlSubframesType e_MaxNprsPerBw;

  /* IFFT Size */
  uint16 w_IfftSize;

  /* Resolution Scale */
  uint8 u_ResolutionScale;
  uint8 u_ShiftValue;

  /* Min Window Size */
  uint16 w_MinWindowSize;
}
loc_lte_otdoa_LookUpPerPrsBwType;

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

#endif /* LOC_LTE_OTDOA_COMMON_DEFS_H */
