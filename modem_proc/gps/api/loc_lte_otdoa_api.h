#ifndef LOC_LTE_OTDOA_API_H
#define LOC_LTE_OTDOA_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Location LTE OTDOA API Header File

GENERAL DESCRIPTION
This file contains API definitions between Location LTE OTDOA module and
other modules outside GNSS.

Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
Qualcomm Proprietary 
 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/api/loc_lte_otdoa_api.h#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "comdef.h"
#include "intf_prs_sm.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

/* maximum number of searches possible per PRS occasion */
#ifdef FEATURE_GNSS_OTDOA_30
  #define LOC_LTE_OTDOA_MAX_HYPO_SRCH_PER_OCCASION    8
#else /* Default to OTDOA 3.1 */
  /* Get the Hyps Per Batch/Occ from FW */
  #define LOC_LTE_OTDOA_MAX_HYPO_SRCH_PER_BATCH       LTE_LL1_PRS_NUM_CELL_MAX
  #define LOC_LTE_OTDOA_MAX_HYPO_SRCH_PER_OCCASION    LTE_LL1_PRS_NUM_CELL_MAX
#endif 

/* Maximum number of LTE carriers supported i.e. PCC, SCC1 and SCC2 */
#define LLOC_MAX_LTE_CARRIERS                         3

/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
typedef enum
{
  /* error because GPS told ML1 to stop */
  LOC_LTE_OTDOA_ERROR_GPS_ABORT = 0,

  /* error because ML1 ran out of PRS occassions */
  LOC_LTE_OTDOA_ERROR_NO_PRS_OCCASIONS,

  /* error because ML1 had a configuration error */
  LOC_LTE_OTDOA_ERROR_CONFIG,

  /* error because ML1 cannot detect the reference cell */
  LOC_LTE_OTDOA_ERROR_NO_REF_CELL,

  /* error because ML1 dosen't have a serving cell (no LTE service) */
  LOC_LTE_OTDOA_ERROR_NO_SERV_CELL,

  /* error because reference cell is unsupported (inter-freq) */
  LOC_LTE_OTDOA_ERROR_UNSUPPORTED_REF_CELL,

  /* undefined error when cause is unknown */
  LOC_LTE_OTDOA_ERROR_UNDEFINED
}
loc_lte_otdoa_PrsErrorInfoType;

/* LTE support bandwidths */
typedef enum
{
  /* 1.4MHz bandwidth */
  LLOC_PRS_BW_6 = 0,
  /* 3MHz bandwidth */
  LLOC_PRS_BW_15,
  /* 5MHz bandwidth */
  LLOC_PRS_BW_25,
  /* 10MHz bandwidth */
  LLOC_PRS_BW_50,
  /* 15MHz bandwidth */
  LLOC_PRS_BW_75,
  /* 20MHz bandwidth */
  LLOC_PRS_BW_100,
  /* Unknown Bandwidth and total num. of BW types */
  LLOC_PRS_BW_UNKNOWN,
  LLOC_PRS_BW_TOTAL = LLOC_PRS_BW_UNKNOWN
}
loc_lte_otdoa_PrsBandwidthType;

/* PRS antenna port config */
typedef enum
{
  LLOC_PRS_ANT_PORT_CONFIG_1_2 = 0,  /* 1-or-2-ports */
  LLOC_PRS_ANT_PORT_CONFIG_4,        /* 4-ports */
  LLOC_PRS_ANT_PORT_CONFIG_MAX,      /* Maximum antenna port config */
}
lloc_PrsAntPortConfigType;

typedef struct
{
  /* enum of type 'loc_lte_otdoa_PrsErrorInfoType' */
  uint8 e_Error;
}
loc_lte_otdoa_MeasErrorInfoType;

/*
  Supported cyclix prefix modes
  Defined in 36.211 Table 6.12-1 for normal and extended mode.
*/
typedef enum
{
  /* Normal CP */
  LOC_LTE_OTDOA_CP_MODE_NORMAL = 0,

  /* Extended CP */
  LOC_LTE_OTDOA_CP_MODE_EXTENDED,

  /* Long CP for MBSFN */
  LOC_LTE_OTDOA_CP_MODE_EXTENDED_MBSFN,

  LOC_LTE_OTDOA_CP_MODE_MAX
}
loc_lte_otdoa_CpModeType;

typedef enum
{
  /* used by ML1 to provide serving cell info at start of PRS meas. */
  LOC_LTE_OTDOA_SERVING_CELL_AT_SESSION_START = 0,

  /* used by ML1 to indicate that seving cell has changed during PRS occasion */
  LOC_LTE_OTDOA_SERVING_CELL_SWITCH
}
loc_lte_otdoa_ServingCellContextType;

typedef struct
{
  /* Physical cell id */
  uint16 w_PhyCellId;

  /* E-ARFCN */
  uint32 w_Earfcn;

  /* PRS bandwidth in RBs */
  uint8 u_Bandwidth;

  /* Cyclic prefix. Enum of type 'loc_lte_otdoa_CpModeType' */
  uint8 e_CpMode;

  /* System Frame Number (SFN) of the last PRS occasion */
  uint16 w_Sfn;

  /* Enum to indicate when this info is passed to OTDOA SW by ML1.
   * Type 'loc_lte_otdoa_ServingCellContextType' */
  uint8 e_Context;

  /* BW of serving cell in enum. Type 'loc_lte_otdoa_PrsBandwidthType' */
  uint8 e_ServingBw;

  /* Num of Tx antennas for serving cell. Type 'lloc_PrsAntPortConfigType' */
  uint8 e_NumTxAnt;
}
loc_lte_otdoa_ServingCellInfoType;

/* Criteria is Offending special meas. */
#define LLOC_ML1_CRITERIA_IS_OFFENDING_SPECIAL_MEAS   0x01

/* Criteria is PRS Offending special meas. */
#define LLOC_ML1_CRITERIA_IS_PRSIC_OFFENDING_SPECIAL_MEAS   0x2

/* Structure to send OSTMR and VSRC values. We have array of [2] for Rx0 and Rx1
 * antennas */
typedef struct
{
  boolean  b_Valid;
  uint16  w_Sfn;
  uint64  t_VsrcFromFW[2]; 
  uint32  q_OstmrFromFW[2];
  uint32  q_OstmrPhFromFW[2];
  uint64  t_RefDlTime;
} 
loc_lte_otdoa_OstmrVsrcInfoType;

typedef enum
{
  LOC_LTE_OTDOA_INTRA_FREQ,
  LOC_LTE_OTDOA_INTER_FREQ_MG,
  LOC_LTE_OTDOA_INTER_FREQ_CA,
  LOC_LTE_OTDOA_INTER_FREQ_GAPLESS,
  LOC_LTE_OTDOA_INTER_FREQ_MAX
}
loc_lte_otdoa_FreqMeasInfoType;

typedef struct
{
  /* Physical cell id */
  uint16 w_PhyCellId;

  /* number of hypothesis reqd. for this particular cell */
  uint8 u_NumHypPerCell;

  /* Hypothesis sequence number. Range - 1 to 'u_NumHypPerCell' */
  uint8 u_HypSeqNum;

  /* PRS BW used to measure this cell. Type 'loc_lte_otdoa_PrsBandwidthType' */
  uint8 e_PrsBw;

  /* Bitmask to specify different scheduling criteria used */
  uint8 u_SchedulingCriteriaBitMask;

  /* Carrier used for the measuremnt of this cell. It could be PCC, SCC1 or SCC2.
   * Range: 0 to 'LTE_LL1_CARRIER_COUNT-1'. As of now, 'LTE_LL1_CARRIER_COUNT' is defined as '3'
   * in 'modem_proc\fw_lte\api\intf_common.h' */
  uint8 u_Carrier; 

  /* PRS frequency meas. type. Based on enum 'loc_lte_otdoa_FreqMeasInfoType'.
   * Info about whether the current cell was an intra-frequency meas., or inter-frequency meas. */
  uint8 e_MeasType;

  /* LTE system bandwidth of the serving cell (it could be the sys. BW of the PCC or the SCC1/SCC2 cell
   * based on the carrier used for PRS measurement). Type: 'loc_lte_otdoa_PrsBandwidthType' */
  uint8 e_LteSysBw;
}
loc_lte_otdoa_PrsMeasHypoSrchInfo;

typedef struct
{
  boolean b_Valid;
  uint16 w_PhyCellId;
  uint32 w_Earfcn;
}
loc_lte_otdoa_OffendingCellInfoType;

typedef struct
{
  /* Pointer to the shared memory block where the PRS occasion meas. are stored */
  void *p_PrsMeasData;

  /* Sequence number of the data block. Used for sanity chk */
  uint32 q_OccasionSeqNum;

  /* total number of hypothesis searched in this PRS occasion */
  uint8 u_NumHyp;

  /* System Frame Number (SFN) of this PRS occasion */
  uint16 w_Sfn;

  /* Info of a strong offending cell, if it was muted in a PRS occasion */
  loc_lte_otdoa_OffendingCellInfoType z_OffendingCell;

  /* Info. on each of the hypothesis in this PRS occasion */
#ifdef FEATURE_GNSS_OTDOA_30  
  loc_lte_otdoa_PrsMeasHypoSrchInfo z_HypoInfo[LOC_LTE_OTDOA_MAX_HYPO_SRCH_PER_OCCASION];
#else /* for OTDOA.3.1 */
  loc_lte_otdoa_PrsMeasHypoSrchInfo z_HypoInfo[LOC_LTE_OTDOA_MAX_HYPO_SRCH_PER_BATCH];
#endif
  /* OSTMR-VSRC info at reference instance (when RF was turned ON). We have array of 3, for
   * each carrier. I.e. PCC, SCC1, SCC2 */
  loc_lte_otdoa_OstmrVsrcInfoType z_RefVsrcInfo[LLOC_MAX_LTE_CARRIERS];

  /* OSTMR-VSRC info at start of a PRS occasion. We have array of 3, for
   * each carrier. I.e. PCC, SCC1, SCC2 */
  loc_lte_otdoa_OstmrVsrcInfoType z_PrsOccVsrcInfo[LLOC_MAX_LTE_CARRIERS];
}
loc_lte_otdoa_PrsOccasionMeasInfoType;

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/
/*
 ******************************************************************************
 * Function: loc_lte_otdoa_MeasError
 *
 * Description:
 *  This function informs LSW (loc software) if there are any error during PRS meas.
 *
 * Parameters:
 *  Complete status.
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_MeasError (loc_lte_otdoa_MeasErrorInfoType *p_MeasErrorInfo);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_MeasResultsAvail
 *
 * Description:
 *  This function informs LSW (loc software) that measurement results are
 *  available in shared memory
 *
 * Parameters:
 *  Sequence number of the available OTDOA measurements
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_MeasResultsAvail (loc_lte_otdoa_PrsOccasionMeasInfoType *p_MeasResultsInfo);

/*
 ******************************************************************************
 * Function: loc_lte_otdoa_UpdateServingCell
 *
 * Description:
 *  This function informs LSW (loc software) about the serving cell information. This
 *  will also be used by ML1 for informing about a cell switch (change of serving cell)
 *
 * Parameters:
 *  Downlink system information of the serving cell
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None.
 *
 ******************************************************************************
 */
extern void loc_lte_otdoa_UpdateServingCell (loc_lte_otdoa_ServingCellInfoType *p_ServingCellInfo);

#endif /* LOC_LTE_OTDOA_API_H */
