/*!
  @file

  @brief
  WCDMA FW modulator interface definitions header file
*/

/*===========================================================================

  Copyright (c) 2007-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_mod_intf.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14   IM      Change number of MPR bins from 7 to 8
09/30/13   IM      Adding couple of members for AGC frame logging
04/29/13   DG      Remove obsolete definitions.
04/22/13   IM      Adding txAgcOnlyDisable flag to WfwTxAgcAsyncWriteStruct
03/18/13   IM      Adding more fields to txAsyncReadBuf for AGC frame logging
03/16/13   CA      Added  QTF Antenna Tuner Support
03/14/13   IM      Changing WfwXptDpdEpdtIndStruct from union to struct
01/28/13   IM      New linearizer table + txAgcAsync API changes
12/03/12   MG      Remove LO override variables as RTR ETFCI127 workaround is removed.
                   Remove r99DataReadyCtr as EUL/R99 concurrent HW encoding bug is fixed.
11/21/12   IM      APT/EPT/ET API changes
10/02/12   IM      EUL FTM beta override
09/28/12   IM      Merged in NikeL EPT code and performed clean-up
08/02/12   IM      Linearizer table changes
07/10/12   PT      Added WFW_TX_R99_INVALID_INIT_POWER to indicate invalid value for txDipPlusCmDelta
07/05/12   MG      Added EPT calibration support
06/26/12   PT      Added UL HS-DPCCH pending config flag
06/07/12   FT      Added EPT (SAPT) support
05/24/12   SS      Added HS-RACH feature
04/27/12   NC      Frame Early Termination & DCCH Early Detection Feature Updates
04/11/12   FT      Added TX power based GRFC trigger support.
04/02/12   FT      Added TX linearizer version support
                   Added WTR phase compensation support
03/09/12   FT      Added support for HSPA linearizer support to use alternate HSPA linearizer
02/23/12   FT      Added QPA support
02/08/12   FT      Added LO override support for HSUPA throughput issue
02/02/12   GS      Compiler warning fix
11/18/11   IM      Expanded Eul AsyncWrite and Eul/R99 AsyncRead structures to support DC on UL
11/15/11   FT      Added comments due to revised FEATURE_WFW_TX_DUAL_LIN_TABLE optimized cache flush
10/19/11   FT      Changed r99 data ready flag from boolean to counter
10/17/11   FT      Added and defined FEATURE_WFW_TX_DUAL_LIN_TABLE
10/04/11   FT      Removed paFreqTempComp, Added dual TX linearizer support
08/10/11   FT      Added r99EncoderOverrunCnt to inform WL1 about R99 encoder overrun state
07/20/11   FT      Added support for EUL-R99 encode workaround and enhanced EUL error code reporting
04/05/11   EM      CR 281966 Adjustable TxAGC SSBI action time for reducing peak evm in RACH
04/05/11   EM      APT changes to allow only temperature compensation in computing SMPS
03/21/11   EM      CR 278732, support AICH_ACK and MSG_DONE for FMO
01/27/11   MG      Add TX/PA Rise/Fall Delays
01/26/11   NC      Fix sign of txAgcValue field for log packets
12/20/10   NC      CR 266788: DC with ack/nack repetition > 1 not working.
                   Separate carrier repetition in demback and UL reporting
12/15/10   NC      CR 263100: RACH extended logging information
11/30/10   EM      Change DTX CQI Timer to signed value for infinite timer
11/23/10   FT      Add support for preReconfigIndicator
10/22/10   EM      CPC DTX: Add DTX interface structure and DUAL FIQ interface
10/19/10   EM      Add Support for FDPCH Outage SIR feature
10/07/10   EM      Add APT Changes
09/28/10   EM      Add MIMO Logging status bit
09/28/10   NC      Add APT interface
09/27/10   EM      Add DTX interface structure and DUAL FIQ interface
07/07/10   NC      Make BetaEc Q8 to handle R8 feature
05/21/10   EM      Add ILPC Per Slot Logging Structure
05/06/10   EM      Add lutParamStruct to mprbin fields
04/22/10   EM      Add logging of Tx linearizer LUT index
12/08/09   EM      Add DC bit field to indicate HS DPCCH logging status
08/28/09   FT      Added txPdm logging
08/17/09   EM      Update Uplink DC second carrier define name
07/27/09   FT      Added timerHysteresisDelay
07/20/09   FT      Added paThresholdOffset and hdetTriggerPeriod
06/29/09   FT      Added EMPR interface and DTX bin for txAHsGainQ10[] and txMtplBin[].
07/16/09   NC      Support legacy RTR in addition to RTR8600
07/15/09   EM      Update TxAGC LUT structure for RTR8600
05/20/09   NC      Merge with Taxis CS2 SWI
03/30/09   FT      Swap ACK and NACK enum in wfwDemodHsResEnum for legacy PLT ACK-NACK override scripts
                   Add txCnt to WfwTxAgcHdetAsyncReadStruct
03/25/09   NC      CLTD logging interface changes
03/03/09   FT      Add EUL Rate Matching logging support
01/30/09   FT      Add MPR logging WfwTxMprAsyncReadStruct
12/29/08   NC      Swap definition for CM Gap and Down command for logging
12/15/08   FT      Add txAgcIqGainUnitySetPoint to interface
12/03/08   FT      Change WFW_MOD_NUM_SUPPORTED_MPR_BINS to 7 to match RF init
11/14/08   NC      Add in enum for TPC & FBI logging definitions
11/06/08   RK      Defined a new ENUM for EUL TTI type
11/05/08   FT      Added TXAGC store per slot over a frame for L1
10/28/08   FT      Merged changes from MIMO dev branch, which include:
                   Add MIMO CQI type to support deltaCQI+1 power
                   Use 1 MIMO PCI override buffer instead of 2
10/28/08   FT      ILPC logging fix. Add disable control of TXAGC SBI.
10/13/08   FT      HS logging fix
10/10/08   RK      Changed the comments on eulFIQOffset format
10/07/08   FT      HDET support in DCH
10/02/08   FT      Added HS MIMO UL support
09/12/08   PK      Expanded ACK/NACK Enum for MIMO
09/06/08   NC      Per TPC combiner group NTIO for rejection threshold calc
08/25/08   NC      Include addl interface vars for power control
07/22/08   NC      Move ACK/NACK/DTX definitions to SWI and fix UWord8 bug
07/18/08   RK      Added a name to the EUL param structure in the union
                   WfwTxEulDBParamStruct
07/15/08   RK      Added betaEdMin term to the EUL Async Write Struct
06/27/08   NC      Move btfDelay to TX AGC SWI
06/19/08   NC      Add HS TX CQI, HS TX ACK/NACK and TPC override interface
06/11/08   RK      Added support for PDU preparation delay in SW and cleaned
                   up the the EUL DB strcuture & async write structure
05/14/08   NC      Add SSBI PDM packet
04/28/08   RK    Added new interface for EUL UL
03/27/08   NC      Add mapped PA range to SWI
03/14/08   NC      HS TX and AGC SWI updates
03/12/08   NC      MPR bin based backoff
11/08/07   NC      AIs from TX SWI review
11/02/07   NC      Added TX AGC PA async variables
10/26/07   NC      Added FBI & AV related async variables
10/19/07   NC      Added additional RACH interface variables
10/15/07   NC      Remove db idx from read interface
10/03/07   NC      Clean up HS logging interface
10/01/07   NC      Remove dependencies on wfw_common.h/qdsp6.h
09/27/07   NC      Addl Updates from interface review
09/21/07   NC      Updates from interface review
08/10/07   NC      Created file for TX async interface definitions
===========================================================================*/

#ifndef WFW_MOD_INTF_H
#define WFW_MOD_INTF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "wfw_demod_intf.h"
#include "fw_dpd_intf.h"

#include "rfcmd_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Public constants and enums */
/* -------------------------- */

/*! @brief Enable Dual Tx linearization support */
#define FEATURE_WFW_TX_DUAL_LIN_TABLE

/*! @brief Max number of TPC Combiners */
#define WFW_TC_NUM_MAX_TPC_COMBINERS 6

/*! @brief Max number of PA states */
#define WFW_MOD_NUM_MAX_PA_STATES 4

/*! @brief Number of MPR bins for max power backoff */
#define WFW_MOD_NUM_SUPPORTED_MPR_BINS 8

/*! @brief Number of HS log packets */
#define WFW_MOD_NUM_HS_LOG_PACKETS 16
/*! @brief Number of HS TX interface buffers */
#define WFW_MOD_NUM_HS_TX_BUF 2

/*! @brief Number of RACH preamble log packets */
#define WFW_MOD_NUM_RACH_LOG_PACKETS 2

/*! @brief Maximum size of TX HS ACK override buffer */
#define WFW_TX_HS_ACK_ORIDE_BUF_SIZE 32
/*! @brief Maximum size of TX HS CQI override buffer */
#define WFW_TX_HS_CQI_ORIDE_BUF_SIZE 32

/*! @brief HDET number of SBI transaction groups */
#define WFW_TX_AGC_HDET_NUM_SBI_GROUP 5
/*! @brief HDET max number of SBI per transaction group */
#define WFW_TX_AGC_HDET_MAX_SBI_PER_GROUP 4
/*! @brief Maximum number of SSBI to use for TX AGC configuration */
#define WFW_TX_AGC_MAX_NUM_SBI 5
/*! @brief Step size of TX AGC - db10 */
#define WFW_TX_AGC_NUM_STEP_PER_DB 10

/*! @brief Number of supported EDPDCH channels */
#define WFW_MOD_NUM_SUPPORTED_EDPDCH 4

/*! @brief Identifier to disable EUL FIQ */
#define WFW_TX_EUL_FIQ_DISABLE 0xFFFF

/*! @brief CQI Timer Infinity */
#define WFW_TX_CPC_DTX_CQI_TIMER_INFINITY 0xFFF

/*! @brief Indicating invalid value for txDipPlusCmDelta in r99 async buffer */
#define WFW_TX_R99_INVALID_INIT_POWER  0x7FFF

/*! @brief Q factor for beta Ec - 8 for EMPR, 11 otherwise */
#define WFW_MOD_BETA_EC_Q_FACTOR 8

/*! @brief Number of DPD tablet set (AMAM+AMPM) supported for EPT */
#define WFW_TX_NUM_EPT_DPD_TABLES_SUPPORTED 4

/*! @brief Maximum number of sample captures for EPT calibration */
#define WFW_TX_NUM_MAX_EPT_CAL_CAPTURES_SUPPORTED 20

/* !@brief MAX possible EDCH resources allocated to the cell*/
#define HS_RACH_MAX_NUM_EDCH_RSRCS 32

/* !@brief MAX possible QPOET writes */
#define WFW_QPOET_MAX_WRITES 5

#define WFW_PA_CURRENT_MAX_WRITES 5

/* !@brief MAX number of PA states that could be in EPT or ET */
#define WFW_TX_MAX_NUM_XPT_STATES_SUPPORTED 2

/*! @brief TX linearization type */
typedef enum
{
  WFW_TX_LIN_LUT_R99=0,
  WFW_TX_LIN_LUT_HS
} wfwTxLinLutTypeEnum;


/*! @brief HS ACK/NACK shared queue values */
typedef enum
{
  WFW_DEMOD_HS_RES_DTX = -1,
  WFW_DEMOD_HS_RES_ACK,
  WFW_DEMOD_HS_RES_NACK,
  WFW_DEMOD_HS_RES_ACK_ACK,        /* 25.212 table 15B, MIMO */
  WFW_DEMOD_HS_RES_ACK_NACK,
  WFW_DEMOD_HS_RES_NACK_ACK,
  WFW_DEMOD_HS_RES_NACK_NACK,
  WFW_DEMOD_HS_RES_PREAMBLE,
  WFW_DEMOD_HS_RES_POSTAMBLE,

  WFW_DEMOD_HS_RES_NUM_RESULTS
} wfwDemodHsResEnum;

/*! @brief TX HS Data Types */
typedef enum
{
  WFW_TX_HS_DTX_DATA_TYPE_IDX = 0,
  WFW_TX_HS_ACK_DATA_TYPE_IDX,
  WFW_TX_HS_NACK_DATA_TYPE_IDX,
  WFW_TX_HS_CQI_DATA_TYPE_IDX,
  WFW_TX_HS_ACK_ACK_DATA_TYPE_IDX,
  WFW_TX_HS_ACK_NACK_DATA_TYPE_IDX,
  WFW_TX_HS_NACK_ACK_DATA_TYPE_IDX,
  WFW_TX_HS_NACK_NACK_DATA_TYPE_IDX,
  WFW_TX_HS_MIMO_CQI_DATA_TYPE_IDX,
  WFW_TX_HS_NUM_DATA_TYPE
} wfwTxHsBetaIdxEnum;

/*! @brief EUL TTI type Enum */
typedef enum
{
  WFW_EUL_10MS_TTI = 0,
  WFW_EUL_2MS_TTI
} wfwTxEulTTIEnum;

/*! @brief EUL FIQ identifier, PreFIQ = -1, FIQ = +1*/
typedef enum
{
  WFW_EUL_PRE_FIQ = -1,
  WFW_EUL_BAD_FIQ,
  WFW_EUL_FIQ
} wfwTxEulFiqEum;

/*! @brief EAGCH SW FIQ request identifier */
typedef enum
{
  WFW_SW_EAGCH_RSLT_REQ_FIQ = -1,
  WFW_SW_EAGCH_RSLT_NO_REQ_FIQ
} wfwSwEagchFiqEnum;

/*! @brief TX DPCCH TPC Log enum */
typedef enum
{
  WFW_TX_DPCCH_TPC_LOG_REJECT = 0,
  WFW_TX_DPCCH_TPC_LOG_UP,
  WFW_TX_DPCCH_TPC_LOG_CM_GAP,
  WFW_TX_DPCCH_TPC_LOG_DOWN
} wfwTxDpcchTpcLogEnum;

/*! @brief TX DPCCH FBI Log enum */
typedef enum
{
  WFW_TX_DPCCH_FBI_ZERO = 0,
  WFW_TX_DPCCH_FBI_ONE,
  WFW_TX_DPCCH_FBI_INV,
  WFW_TX_DPCCH_FBI_CM_GAP
} wfwTxDpcchFbiLogEnum;

/*! @brief APT SMPS mode of operation */
typedef enum
{
  WFW_TX_APT_MODE_DISABLE=0,
  WFW_TX_APT_MODE_TX_PDM,
  WFW_TX_APT_MODE_GPIO,
  WFW_TX_XPT_MODE_QPOET
} wfwTxSmpsModeEnum;

/*! @brief EUL PDU DTX reason code */
typedef enum
{
  WFW_TX_EUL_NO_TX_THIS_TTI=0,
  WFW_TX_EUL_PDU_DONE_DELAYED,
  WFW_TX_EUL_PARAM_DELAYED,
  WFW_TX_EUL_ENCODE_SKIPPED_DUE_TO_ACTIVE_R99_ENCODE,
  WFW_TX_EUL_ENCODE_TRASHED_DUE_TO_R99_ENCODE
} wfwTxEulErrorCode;

/*! @brief TX linearization version */
typedef enum
{
  WFW_TX_LIN_LUT_LEGACY_INTF=0,
  WFW_TX_LIN_LUT_MULTI_SSBI_INTF,
  WFW_TX_LIN_LUT_MULTI_SSBI_INTF_EAPT
} wfwTxLinLutVerEnum;

/* Structure definitions */
/* --------------------- */

/*! @name Structure definition for carrier specific async UL DPCCH SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW TX DPCCH MDSP async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*! @name FPC async write vars - DPCH frame boundary latched */
  /*!@{*/
  /* NOTE: Do NOT insert any variable between fpcTargetEbNt and fpcTargetEbNtWindUp, and
           they should be of the SAME size */
  /*! @brief Inner threshold for forward power control, default 256 */
  UWord16 fpcTargetEbNt;
  /*! @brief Inner threshold for forward power control for windup detection, default 0,*/
  UWord16 fpcTargetEbNtWindUp;
  /*! SIRT floor value */
  UWord16 fpcTargetFloor;

  /*! @brief Adjustment to targetEbNt for compressed mode, default 1024 */
  UWord16 fpcCmEbNtAdjust;
  /*! @brief Indicator to force a balanced FPC command */
  Word8   fpcForcePowerBalance;
  /*! @brief Data aided power estimation feature for FPC processing */
  Word8   fpcDapeEn;
  /*!@}*/

  /*! @brief Initial Power + compressed mode delta in dB10,
      Range -512->512 */
  Word16  txDipPlusCmDelta;
  /*! @brief MPR bin for r99 (no-HS) (Range 0->5) */
  UWord8  txMtplBin;
  /*!@}*/

  /*! @name FBI async write vars */
  /*!@{*/
  /*! @brief Per-slot FBI open loop override mode */
  UWord8  fbiOrideOpenLoop;
  /*! @brief Per-slot FBI override BER in Q16 */
  UWord16 fbiOrideBerThresh;
  /*! @brief Per-slot FBI bit in open loop debug mode for TX */
  UWord16 fbiOridePerSlot;
  /*! @brief Per-slot FBI bit in open loop debug mode for AV */
  UWord16 fbiOridePerSlotInternalChain;
  /*!@}*/

  /*! @brief Initial Backoff - Use to calculate initial DPCCH power for 2nd UL.
      Range: 0,10,20,...70 dB10 => UWord8 & 0xFFFF is invalid... */
  Word16 initialDpcchBackoffSec;   /* Only for secondary */

  /*! @name TPC async write vars - DPCH frame boundary latched */
  /*!@{*/

  /*! @name TPC common async write vars - DPCH frame boundary latched */
  /*!@{*/
  /*! @brief Specifies TPC algorithm, 0->TPC alg 1, 1->TPC alg 2, default 0 */
  Word8  tpcAlgorithm;
  /*! @brief TX TPC step size in dB10 (Range 10, 20, 30), default 10 */
  Word8  tpcGainAdjStepSize;

  /*! @brief TPC override flag - if true, use tpc decision from tpcOrideDecHist */
  Word8  tpcOrideFlag;
  /*! @brief CLTD init mode skip InitCnt flag */
  Word8  initCntSkipFlag;
  /*! @brief When non-zero, HS TPC group will override the normal ULTPC decision */
  Word8 hsOnlyULTPC;
  /*! @brief TPC override histogram - BIT(1:0) = slot0 TPC command:
      0=reject, 1=UP, 2,3=DOWN */
  UWord32 tpcOrideDecHist;
  /*! @brief Up threshold used for TPC algorithm 1 for each PC group, default 0 */
  Word32 tpcUpThreshPCA1[WFW_TC_NUM_MAX_TPC_COMBINERS];
  /*! @brief Down threshold used for TPC algorithm 1 for each PC group, default 0 */
  Word32 tpcDownThreshPCA1[WFW_TC_NUM_MAX_TPC_COMBINERS];
  /*! @brief Up threshold used for TPC algorithm 2 for each PC group, default 0 */
  Word32 tpcUpThreshPCA2[WFW_TC_NUM_MAX_TPC_COMBINERS];
  /*! @brief Down threshold used for TPC algorithm 2 for each PC group, default 0 */
  Word32 tpcDownThreshPCA2[WFW_TC_NUM_MAX_TPC_COMBINERS];
  /*! @brief Outage DL TPC SIR */
  Word32 fdpchOutageSirTh;
  /*! @brief indicator for UL DCCH present */
  Word32 dcchPresent;
  /*!@}*/

  /*! @brief out-of-sync indication to control transmission per carrier
      if both carriers are out of syncm turn off PA. Otherwise, DTX baseband */
  Word8 outOfSync;

  /*! @brief If there is RLF in second carrier, put channels into dormant mode.
      SW shall reset this flag once it receives RF retune FIQ from FW */
  UWord8 dormantModeSecFlag;

  /*! @brief If this set tune Tx back to 10 MHz after an LTA gap.
      SW shall reset this flag once it receives RF retune FIQ from FW */
  UWord8 secActiveFlag;

  /*! @brief TX_PDA_CTRL_WD_Cc: base value of Phase Delta Accumulator -> nominal 0 */
  Word32 txPdaCtrlBase;

  /*! @brief TX_PDA_CTRL_WD_OFFSET_Cc -> nominal values C0(-): 182955, C1(+): 341333
      round(2^22*frequency_offset(=2.5e6)/sampling_rate(=30.72e6)) = 341333 for C1 */
  Word32 txPdaCtrlOffset;

} WfwTxDpcchAsyncWriteStruct;

/*! @name Structure definition for async UL DPCCH SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW TX DPCCH MDSP async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*! @name General TX async write vars - DPCH frame boundary latched */
  /*!@{*/
  /*! @brief Beta C Gain, Range 1-15 */
  UWord8  txBetaCGain;
  /*! @brief Beta D Gain, Range 0-15 */
  UWord8  txBetaDGain;

  /* Workaround for Anritsu not supporting DC-HSUPA SCCH order
     Set DC-HSUPA order based on DC-HSDPA order if this variable is set */
  UWord8 dcHsdpaOrderImpliesDcHsupaHack;

} WfwTxDpcchCommonAsyncWriteStruct;


/* Struct of bitmasks (one bit per slot) indicating various ILPC gating or modifying factors */
typedef struct
{
  /*! Bitmask indicating which slots within a frame, FOR ANY REASON, did not attempt a ULTPC decision based on received 
    TPC, thereby forcing zero change in DPCCH power for a slot.
    This mask is a superset of other bitmasks here that indicate specific reasons such as DSDx or FET gaps.
    Future features that gate ULTPC should also be rolled into this bitmask indication.
    This bitmask does not get set for late-cell slot delays or HS-only TPC mode, since these conditions modify but do       
    not prevent a ULTPC decision */
  UWord16 noDecisionBitmaskULTPC;

  /*! Bitmask indicating which slots within a frame, FOR ANY REASON, did not attempt a DLTPC decision based on received 
    DPCCH.  This mask is a superset of other bitmasks here that indicate specific reasons such as CM or DSDx gaps.
    Future features that gate DLTPC should also be rolled into this bitmask indication.                                     
    This bitmask does get set for cases where the DLTPC decision is "canned" rather than based on received DPCCH.           
    This bitmask does not get set for late-cell slot delays or HS-only TPC mode, since these conditions modify but do       
    not prevent a DLTPC decision */
  UWord16 noDecisionBitmaskDLTPC;

  /*! Bitmask indicating which slots within a frame skipped ULTPC processing (i.e. forced no decision) due to a DSDx-type gap */
  UWord16 dsdxGapBitmaskULTPC;
  /*! Bitmask indicating which slots within a frame skipped DLTPC (i.e. forced no decision) due to a DSDx-type gap */
  UWord16 dsdxGapBitmaskDLTPC;
  /*! Bitmask indicating which slots within a frame had no ULTPC decision because they were one of the first 4 slots of the 5-slot PCA2 period */
  UWord16 pca2BitmaskULTPC;
  /*! Bitmask indicating which slots within a frame delayed UL- and DLTPC processing due to one or more late cells */
  UWord16 slotDelayBitmask;
  /*! Bitmask indicating which slots within a frame skipped ULTPC processing (i.e. forced no decision) due to a FET gap */
  UWord16 fetGapBitmaskULTPC;
  /*! Bitmask indicating which slots used DAPE for DLTPC processing */
  UWord16 dapeBitmask;
  /*! Bitmask indicating for which slots within a frame HS-only TPC mode was enabled */
  UWord16 hsOnlyTpcBitmask;
  /*! Bitmask indicating when application of ULTPC decision wasn't possible due to MTPL constraint */
  UWord16 mtplConstraintBitmask;
  /*! Bitmask indicating when application of ULTPC decision wasn't possible due to min power constraint */
  UWord16 minPwrConstraintBitmask;
  /*! Bitmask indicating when application of ULTPC decision wasn't possible due to DPCCH pwr ratio constraint */
  UWord16 pwrRatioConstraintBitmask;
} WfwTxIlpcGatingBmsksStruct;

/* ILPC logging struct */
typedef struct
{
  /* Indicates which indices of Per-TPC-group arrays should be logged by SW */
  UWord8 logTpcGrpBmsk;

  /* Per-TPC-group logging */
  Word16 scaledTpcPerGrp[6];
  Word16 scaledDpPerTpcGrp[6];
  Word16 scaledNtIoPerTpcGrp[6];

  /*! @brief Accumed NtIo */
  Word16 sirEstimateA;
  /*! @brief SIR Estimate B, default 1, not used in Genesis */
  Word16 sirEstimateB;
  /*! @brief SIR Estimate C, default 2, not used in Genesis */
  Word16 sirEstimateC;
  /*! @brief Filtered TPC bit EsIo */
  Word16 sirEstimateD;
  /*! @brief 2-bit FPC code used for DL power control */
  Word16 fpcDecision;
  /*! @brief Target SIR DL power control */
  Word16 sirTarget;
} WfwTxIlpcSlotLogStruct;


/*! DAPE logging info structure */
typedef struct
{
  /*! @brief bitmask of slots determined to be NULL */
  UWord16 nullDecBmsk;
  /*! @brief bitmask of slots determined to be SID  */
  UWord16 sidDecBmsk;
  /*! @brief bitmask of slots determined to be FULL */
  UWord16 fullDecBmsk;
  /*! @brief bitmask of slots determined to be DCCH */
  UWord16 dcchDecBmsk;
  /*! @brief classifier 1 decision: 0=NULL, 1=SID, 2=FULL */
  UWord8 dtchClass1Dec;
  /*! @brief DP only power estimate (in Q8)*/
  Word16 dpPower[15];
  /*! @brief noise estimate from NtIo */
  Word16 dpNoise[15];
  /*! @brief data only power estimate (in Q8) */
  Word16 dataPower[15];
  /*! @brief data* only noise estimate */
  Word16 dataNoise[15];
  /*! @brief number of data bits used */
  UWord8 numDataBits[15];
  /*! @brief tpc decision */
  UWord8 tpcCmdIdx[15];
}WfwTxDapeLoggingStruct;


/*! @brief WCDMA FW UL DPCCH MDSP Carrier specific async write variables - READ ONLY BY L1 */
typedef struct
{
  /*! @name FPC async read vars - DPCH frame boundary latched */
  /*!@{*/
  /*! @brief Bitmask of FPC decisions over previous frame, each slot is 2-bit s.t.:
      0->invalid CM gap, 1->up decision, 2->down decision, 3->invalid fingers OOL.
      BIT(29:28)->slot0, BIT(27:26)->slot1 ... BIT(1:0)->slot14 */
  UWord32 fingLockFpcBitValidHist;
  /*! @brief Bitmask of FPC decisions over previous frame based on fpcTargetEbNt,
      0->down, CM gap, invalid, 1->up.  BIT(14) = slot0, ... BIT(0) = slot14 */
  UWord16 fpcOverallDecisionHist;
  /*! @brief Bitmask of FPC decisions over previous frame based on fpcTargetEbNtWindUp,
      0->down, CM gap, invalid, 1->up.  BIT(14) = slot0, ... BIT(0) = slot14 */
  UWord16 fpcOverallDecisionWindupHist;

  /*! @brief Bitmask of valid FPC slots over previous frame,
      0->slot in CM gap, 1->slot not in CM gap,
      BIT(14) = slot0, ... BIT(0) = slot14 */
  UWord16 fpcBitValidHist;
  /*! @brief CPICH energy weighted NT/IO */
  UWord32 fpcWtNtIoRd;
  /*! @brief CPICH amplitude weighted dedicated pilot amplitude */
  UWord32 fpcWtTrafficAmpRd;
  /*! @brief Squared, filtered TPC sum for HS serving cell's TPC group */
  UWord32 fpcHsFiltTpcEnrgyRd;
  /*! @brief Per TPC combiner group Wt NTIO */
  UWord32 fpcTpcWtNtIoRd[WFW_TC_NUM_MAX_TPC_COMBINERS];
  /*! @brief Per TPC combiner group weighted EsIo */
  UWord32 fpcTpcWtEsIoRd[WFW_TC_NUM_MAX_TPC_COMBINERS];
  /*!@}*/

 /*! @brief R99 encode DTX count, indicate delayed ERAMA R99 data write condition */
  UWord32 r99DataDTXCnt;
  /*! @brief R99 encoder error count, indicate encoder overrun condition */
  UWord32 r99EncoderOverrunCnt;

  /*! @name TPC async read vars - DPCH frame boundary latched */
  /*!@{*/
  /*! @brief TX TPC Accum */
  Word16  tpcAdjAccumRd;
  /*! @brief bitfield of up vs non-up commands */
  UWord16 tpcDecLogHist;
  /*! @brief Bitmask of Overall TPC decisions over previous frame, each slot is 2-bit s.t.:
      0->reject(no change), 1->UP, 2->CM_GAP, 3->DOWN.
      BIT(29:28)->slot0, BIT(27:26)->slot1 ... BIT(1:0)->slot14 */
  UWord32 tpcOverallDecisionHist;
  /* @brief Per-TPC combiner decision histogram, same definition as overall TPC decision */
  UWord32 tpcDecisionHist[WFW_TC_NUM_MAX_TPC_COMBINERS];
  /*! @brief bitfield of up vs non-up commands */
  UWord8  tpcNumRejectedTpcCmdHist[WFW_TC_NUM_MAX_TPC_COMBINERS];

  /*! @name TFC PL async read vars - DPCH frame boundary latched */
  /*!@{*/
  /*! @brief TFC PL headroom value for each slot of previous frame ->
      maxTxLimit (no backoff) - txTpcAccum, Range -512->512 */
  Word16   tfcPlHeadroomValue[15];
  /*! @brief TFC PL beta HS state for each slot of previous frame:
      0->DTX, 1->ACK, 2->NACK, 3->CQI, Range 0-3 */
  Word8   tfcPlHsTxPowerLevel[15];

  /*! @brief DPCCH power per carrier for the last 3 slots - latched once per subframe */
  Word16 dpcchPowerValueRd[3];

  /*!@}*/

  /*! @name FBI async read vars - DPCH frame boundary latched */
  /*!@{*/
  /*! @brief Bitmask of Overall FBI decisions over previous frame, each slot is 2-bit s.t.:
      0->rejected CM gap, 1->up decision, 2->down decision, 3->reserved.
      BIT(29:28)->slot0, BIT(27:26)->slot1 ... BIT(1:0)->slot14 */
  UWord32 fbiDecisionhist;

  /*! @brief Number of Slot in frame that has lower outage SIR */
  Word32 fdpchOutageSlotNum;
  /*! @brief Number of Slot that is skipped due to DTX
       SW use 15 - tpcDtxGapSkipCount in calculation */
  Word32 tpcDTXGapSkipCount;

  /*! @brief Bitmasks indicating per-slot gating of UL/DLTPC */
  WfwTxIlpcGatingBmsksStruct ilpcGatingBmsks;

  /*!@}*/

  /*! @brief ILPC per slot logging */
  WfwTxIlpcSlotLogStruct ilpcSlotLogging[15];

  /*! @brief DAPE per slot logging */
  WfwTxDapeLoggingStruct dapeSlotLogging;

  /* Indicates Pending DPCCH Cfg*/
  Word8 judcDpcchPendingCfg;

} WfwTxDpcchAsyncReadStruct;

/*! @brief WCDMA FW UL DPCCH MDSP async write variables - READ ONLY BY L1 */
typedef struct
{
  /*! @brief TXAGC value for each slot of the previous frame */
  Word16  txAgcValueStore[15];
  /*! MPR values in dB10 units for each slot of previous frame */
  UWord8  mprValueStore[15];

  /*! @brief TX Frame Strobe in Cx8 unit */
  Word32 txTimeTxDacFrBpg;

  /* Currently used for SIM only */
  Word16 reserved1;

} WfwTxDpcchCommonAsyncReadStruct;

/*!@}*/

/*! @name Structure definition for async UL HS DPCCH SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW UL HS DPCCH MDSP async write variables - READ ONLY BY L1 */
typedef struct
{
  /* Word 0 */
  /*! @brief Beta Info for ACK slot */
  /*!  Bits 0-5 BetaHs.  Bits 6-11 BetaD/BetaC enum. */
  UWord32 betaAckSlot : 12;       /* Bit 0-11*/
  /*!  @brief Least signif 6 bits are BetaHs.  Most signif 6 bits are BetaD/BetaC enum. */
  UWord32 betaCqiSlot : 12;       /* Bit 12-23 */
  /*! @brief Currently only slot format 0 supported. 4C or 3C+MIMO require format 1*/
  UWord32 hsDpcchSlotFormat : 1;  /* Bit 24 */
  UWord32 reserved_0 : 7;         /* Bit 25-31 */

  /* Word 1 */
  /*! @brief TX AGC Value at start of ACK slot */
  Word32  txAgcValueAck : 10;    /* Bit 0-9 */
  /*! @brief PA Range at start of ACK slot */
  UWord32 paRangeAck : 2;        /* Bit 10-11 */
  /*! @brief TX AGC Value at start of CQI slot */
  Word32  txAgcValueCqi : 10;    /* Bit 12-21 */
  /*! @brief PA Range at start of CQI slot */
  UWord32 paRangeCqi : 2;        /* Bit 22-23 */
  /*! @brief MIMO status bit for logging cqi */
  UWord32 mimoEnaStatus : 1;     /* Bit 24 */
  UWord32 reserved_1 : 7;        /* Bit 25-31 */

  /* Word 2 */
  /*! @brief Bitmask indicating which secondary cells are active. Bit 0 is 1st
    secondary cell, bit 1 is 2nd secondary cell, etc. */
  UWord32 hsSecCarActiveBMask : 3;       /* Bit 0-2 */
  /*! @brief For 3C mode: =0 for 1st CQI report, =1 for 2nd CQI report, =2 for no report (DTX).
  Only 3C uses a 2nd CQI report */
  UWord32 hsCqiReport : 2;               /* Bit 3-4 */
  /*! @brief Bitmask indicating which serving cells' CQI are reported this subframe, e.g. for 3C w/o MIMO,
    1st report has C0 valid only, 2nd report has CQ1 and CQ2 valid */
  UWord32 hsCqiValidBmsk : 4;             /* Bit 5-8 */
  /*! @brief Selected Receiver type: Rake, QICE, or override */
  UWord32 cqiRcvrType : 2;            /* Bit 9-10 */
  UWord32 reserved_2 : 21;                /* Bit 11-31 */

  /*! @brief ACK/NACK status for each serving cell, 1-DTX, 2-ACK, 3-NACK.  The only valid entries in
    this array are those for primary serving cell and active secondary cells (see hsSecCarActiveBMask) */
  UWord8 ackNackStatus[WFW_DEMOD_NUM_MAX_CARRIERS];

  /*! @brief CQI value sent on UL.  Depending on config this may be a 5-bit or an 8-bit CQI value */
  UWord8 cqiNum[WFW_DEMOD_NUM_MAX_CARRIERS];

  /* TX blanking for Multi-sim HSPA*/
  UWord32 txBlankBmsk          : 30; /* Bits 29-0 */
  UWord32 reserved             : 2;  /* Bit 31-30 */

} WfwTxHsLogStruct;

/*! @brief WCDMA FW UL HS DPCCH MDSP async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*! @name HS TX async write vars -
      latched at first full subframe after DPCH frame boundary */
  /*!@{*/

  /*! @brief AHs (BetaHs/BetaC) Gain x15 for HS data type. Enu please check wfwTxHsBetaIdxEnum defined in this file.
      Range (0-38) in Q10. Index: 0-DTX, 1-ACK, 2-NACK, 3-CQI, 4-ACKACK, 5-ACKNACK, 6-NACKACK, 7-NACKNACK, 8-MIMOCQI */
  UWord16 txAHsGainQ10[WFW_TX_HS_NUM_DATA_TYPE];

  /*! @brief MPR bin for all HS data types, index 0 = DTX, in the order:
      DTX, ACK, NACK, CQI, ACKACK, ACKNACK, NACKACK, NACKNACK, MIMOCQI (We need DTX as mprBin of DTX does not have to be 0) */
  UWord8 txMtplBin[WFW_TX_HS_NUM_DATA_TYPE];

  /*! @brief CQI override feedback cycle repetition count */
  Word16  cqiOrideRepeatMax;
  /*! @brief CQI override flag */
  UWord8  cqiOrideFlag;
  /*! @brief CQI override pattern length (Range 0->31) */
  UWord8  cqiOridePatternLen[WFW_DEMOD_NUM_MAX_CARRIERS];

  /*! @brief CQI A override flag */
  UWord8  mimoCqiTypeAOrideFlag;
  /*! @brief CQI A override pattern length (Range 0->31) */
  UWord8  mimoCqiTypeAOridePatternLen;
  /*! @brief CQI B override flag */
  UWord8  mimoCqiTypeBOrideFlag;
  /*! @brief CQI B override pattern length (Range 0->31) */
  UWord8  mimoCqiTypeBOridePatternLen;
  /*! @brief PCI override flag */
  UWord8  mimoPciOrideFlag;
  /*! @brief PCI override pattern length (Range 0->31) */
  UWord8  mimoPciOridePatternLen;

  /*! @brief CQI pattern buffer, each element repeated for cqiOrideRepeatMax
      feedback cycles */
  UWord8  cqiOrideNumBuf[WFW_DEMOD_NUM_MAX_CARRIERS][WFW_TX_HS_CQI_ORIDE_BUF_SIZE];
  UWord8  mimoCqiTypeANumBuf[WFW_TX_HS_CQI_ORIDE_BUF_SIZE];
  UWord8  mimoCqiTypeBNumBuf[WFW_TX_HS_CQI_ORIDE_BUF_SIZE];
  UWord8  mimoPciNumBuf[WFW_TX_HS_CQI_ORIDE_BUF_SIZE];
  /*! @brief ACK/NACK override flag */
  UWord8  ackOrideFlag;
  /*! @brief ACK override pattern buffer length (Range 0->31) */
  UWord8  ackOridePatternLen;
  /*! @brief ACK override pattern buffer */
  Word8   ackOrideBuf[WFW_DEMOD_NUM_MAX_CARRIERS][WFW_TX_HS_ACK_ORIDE_BUF_SIZE];
  /*!@}*/

} WfwTxHsDpcchAsyncWriteStruct;

/*! @brief WCDMA FW UL HS DPCCH MDSP async write variables - WRITE ONLY BY L1 */
typedef union
{
  struct
  {
    /* Word 0*/

    /*! @brief Happy Bit 1->UE is content with the granted Tr/pilot ratio
    0->UE not content */
    UWord32 happyBit : 1;           /* bit 0 */
    /*! @brief Retransmission Sequence Number (RSN) is sent to NodeB to
        indicate the redundancy version of each HARQ transmission, when using
        incremental redundancy */
    UWord32 rsn : 2;                /* bit 1-2 */
    /*! @brief  E TFCI to be encoded by the FW for the following TTI */
    UWord32 etfci : 7;              /* bit 3-9 */
    /*! @brief This configuration is used to map the data sent on different
        E-DPDCH to the appropriate I & Q channels */
    UWord32 edpdchConfig : 2;       /* bit 10-11 */
    /*! @brief This field indicates the total number of E-DPDCH channels
        with SF2 configured for the following TTI*/
    UWord32 numSf2 : 2;             /* bit 12-13 */
    UWord32 reserved1 : 2;          /* bit 14-15 */
    /*! @brief ERAM bank index */
    UWord32 eramBankIdx : 4;        /* Bit 16-19 */
    /*! @brief number of  valid slots per TTI */
    UWord32 numTxSlotsPerTTI : 4;   /* Bit 20-23 */
    /*! @brief ctl Bank Selection
        Specifies which bank to use for EUL ERAM CTRL and COPY ENG
        memories. 0: Bank0 1: Bank1 */
    UWord32 ctlBank : 1;            /* bit 24 */
    /*! @brief 1-> 16Qam , 0->QPSK */
    UWord32 modulationType : 1;     /* bit 25 */
    UWord32 copyEngEn : 1;          /* bit 26 */
    UWord32 reserved2 : 5;          /* bit 27-31 */

    /* Word 1*/

    /*! @brief  The SF of E-DPDCHc */
    UWord32 edpdchSf0 : 3;          /* bit 0-2 */
    /*! @brief  The OVSF code number for E-DPDCHc */
    UWord32 edpchOvsfnum0 : 8;     /* bit 3-10 */
    /*! @brief 1-> 16Qam , 0->QPSK */
    UWord32 reserved3 : 4;          /* bit 11-14 */
    /*! @brief  1 -> indicates the channel number
        in this particular location is enabled */
    UWord32 D0 : 1;                   /* bit 15 */
    /*! @brief  The SF of E-DPDCHc */
    UWord32 edpdchSf1 : 3;          /* bit 16-18 */
    /*! @brief  The OVSF code number for E-DPDCHc */
    UWord32 edpchOvsfnum1 : 8;     /* bit 19-26*/
    /*! @brief 1-> 16Qam , 0->QPSK */
    UWord32 reserved4 : 4;         /* bit 11-14 */
    /*! @brief  1 -> indicates the channel number
        in this particular location is enabled */
    UWord32 D1 : 1;                /* bit 15 */

    /* Word 2*/

    /*! @brief  The SF of E-DPDCHc */
    UWord32 edpdchSf2 : 3;         /* bit 0-2 */
    /*! @brief  The OVSF code number for E-DPDCHc */
    UWord32 edpchOvsfnum2 : 8;     /* bit 3-10 */
    /*! @brief 1-> 16Qam , 0->QPSK */
    UWord32 reserved5 : 4;         /* bit 11-14 */
    /*! @brief  1 -> indicates the channel number
        in this particular location is enabled */
    UWord32 D2 : 1;                /* bit 15 */
    /*! @brief  The SF of E-DPDCHc */
    UWord32 edpdchSf3 : 3;         /* bit 16-18 */
    /*! @brief  The OVSF code number for E-DPDCHc */
    UWord32 edpchOvsfnum3 : 8;     /* bit 19-26*/
    /*! @brief 1-> 16Qam , 0->QPSK */
    UWord32 reserved6 : 4;         /* bit 11-14 */
    /*! @brief  1 -> indicates the channel number
        in this particular location is enabled */
    UWord32 D3 : 1;                /* bit 15 */

    /* Word.3*/
    /*! @brief  total number of buts into rate matching block */
    UWord32 Nej ;              /* bit 0-31 */

    /* Word 4*/
    /*! @brief  The total number of bits available for the E-DCH transmission
        per TTI with transport format j (this is post rate matching */
    UWord32 NeDataj ;          /* bit 0-31 */

    /* Word 5*/
    /*! @brief Gain factor for E-DPCCH, Q8 for EMPR7 */
    UWord32 betaEc : 16;            /* bit 0-15 */
    /*! @brief  TGain factor for E-DPDCH channels */
    UWord32 betaEd : 16;            /* bit 16-31 */

    /* Word 6*/
    /*! @brief Gain factor for E-DPDCH channels with SF=2 */
    UWord32 betaEdSF2 : 16;          /* bit 0-15 */

  }paramStruct;

  Vect32 v[7];

}WfwTxEulDBParamStruct;

/*! @brief WCDMA FW EUL MPR bin arrangement for EMPR Reselection LUT */
typedef union
{
  struct
  {
    /*! @brief MPR bin for all HS data types */
    UWord32 mprBinDtx:3;      /* bit 0-2 */
    UWord32 mprBinAck:3;      /* bit 3-5 */
    UWord32 mprBinNack:3;     /* bit 6-8 */
    UWord32 mprBinCqi:3;      /* bit 9-11 */
    UWord32 mprBinAckAck:3;   /* bit 12-14 */
    UWord32 mprBinNackNack:3; /* bit 15-17 */
    UWord32 mprBinMimoCqi:3;  /* bit 18-20 */
    UWord32 mprBinAckNack:3;  /* bit 21-23 */
    UWord32 mprBinNackAck:3;  /* bit 24-26 */
    UWord32 reserved:5;       /* bit 27-31 */
  }lutParamStruct;
  UWord32 lutWord;            /* Composite word */
} WfwTxEmprLutStruct;

/*! @brief Carrier Specific WCDMA FW EUL MDSP async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*! @brief This flag is set for the current TTI by the SW when it is done
      writing the params for rm calculation*/
  UWord32 edpchWriteCompleteCnt;
  /*! @brief This flag is set for the current TTI by the A2 when it is done
      preparing the pdu for the given TTI - A2 will invalidate the entire
      32-byte cache line */
  UWord32 pduDoneCnt __attribute__((aligned(32)));
  /*! @brief Double buffered EUL parameter structure.
      Makes sure the next part of the structure starts at the next
      cache boundary leaving 28 bytes after pduConeCnt free and not
      affected by cache invalidation by A2 */
  WfwTxEulDBParamStruct edpchParamBuf[2] __attribute__((aligned(32)));

  /*! @brief index into the edpchparamBuf */
  UWord8 edpchParamBufIdx;

  /*! @brief Double buffered EUL EMPR lookup table. Length = WFW_TX_BETAED_QTABLE_LENGTH_LONG = 33. */
  WfwTxEmprLutStruct eMprLut[2][33];
  /*! @brief eMprLut format indicator: 0 => WFW_TX_BETAED_QTABLE_LENGTH_SHORT, 1 = WFW_TX_BETAED_QTABLE_LENGTH_LONG */
  UWord8 eMprParamTableFmtIndicator;
  /*! @brief eMprLut active index */
  UWord8 eMprParamTableIdx;

  /*! @brief Indicator for AGCH FIQ SW Request(-1), No Request (0)*/
  Word8 eagchFiqSwReqIndicator;

  /*@brief min beta Ed allowed in case the EDPDCH power needs to be lowered
    in Q7 format */
  UWord16 betaEdMin;
  /*! @brief set to 1 when betaEd rescaling function is not to be called
      from Tx AGC */
  UWord8 pwrCompDisable;
} WfwTxEULAsyncWriteStruct;

/*! @brief WCDMA FW EUL R9-MPR bin arrangement for EMPR Reselection LUT */
typedef struct
{
  /*! second moment */
  float secondMoment;
  /*! fourth moment */
  float fourthMoment_x_9;
  /*! sixth moment */
  float sixthMoment;
} WfwTxDcEmprMomentStruct;

/*! @brief WCDMA FW EUL MDSP async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*! @brief this variable contains the BPG cnt in a 2msec TTI from the DPCH
      frame boundary when the RM proc and beta norm thread would be called */
  UWord8 rmTriggerTime2msOverride;
  /*! @brief this variable contains the BPG cnt in a 10msec TTI from the DPCH
      frame boundary when the RM proc and beta norm thread would be called */
  UWord8 rmTriggerTime10msOverride;

  /*! @brief set to 1 if EMPR algorithm is used, otherwise use legacy power compression */
  UWord8 emprEnable;

  /*! @brief Counter to sequence PreFIQ and FIQ, indication from SW to FW to error tracking */
  UWord8 eulDLDualFiqCount;

  /*! @brief this variable contains the modulator Slot number and bpg number
    in MSB and LSB resp.at which the fw should send the Pre-FIQ interrupt
    b[0]=modSlotCnt b[1]=modBpgCnt */
  UWord16 eulDLPreFIQOffset;

  /*! @brief this variable contains the modulator Slot number and bpg number
    in MSB and LSB resp.at which the fw should send the FIQ interrupt
    b[0]=modSlotCnt b[1]=modBpgCnt */
  UWord16 eulDLFIQOffset;

  /*! @brief compressed mode mask that would apply for UL channels based
      on compressed slot occurance */
  UWord16 cmGenericSlotBitMask0;
  UWord16 cmGenericSlotBitMask1;

  /*! @brief if there is a retransmission that needs to happen of the data
      that was previously a part of compressed slot those particular slots
      in the retransmission are also DTXed */
  UWord16 edchPseudoCMSlotBitMask0;
  UWord16 edchPseudoCMSlotBitMask1;

  /*! @brief Double buffered EUL EMPR lookup table. Length = WFW_TX_HS_NUM_DATA_TYPE = 9. */
  WfwTxDcEmprMomentStruct dcEmprMomentPri[2][WFW_TX_HS_NUM_DATA_TYPE];

  /*! @brief Double buffered EUL EMPR lookup table. Length = 1 (betaHs = 0 for secondary. */
  WfwTxDcEmprMomentStruct dcEmprMomentSec[2];

  /* passed down from SW */
  UWord8 subFrameNum;
  /*! @brief ping-pong index into the above dcEmprMomentPri[] and dcEmprMomentSec[] */
  UWord8 txDcEmprMomentIdx;

  Word16 ccsDelayRampUp;

  Word16 ccsDelayRampDownHighEtfci;

  Word16 ccsDelayRampDownLowEtfci;

  Word16 etfciThreshold;

  UWord16 timestampT1Value;
} WfwTxEULCommonAsyncWriteStruct;

/*! @brief Structure definition for DPD (EPT) and detrough (ET) table indices - WRITE ONLY BY RFSW */
typedef struct
{
  /*! @brief AMAM and AMPM (DPD) indices for EPT and ET */
  rfcmd_xpt_table_info_t amamInd[64];
  rfcmd_xpt_table_info_t ampmInd[64];
  /*! @brief EPDT (detrough) indices for ET only */
  rfcmd_xpt_table_info_t epdtInd[64];
} WfwXptDpdEpdtIndStruct;

/* changed eptDpdLut --> xptDpdEpdtLut */
/*! @brief Structure definition for dynamic async TX EPT DPD SW-FW write interface - WRITE ONLY BY RFSW */
typedef struct
{
 /*! @brief LUT that contains DPD mapping for PA range eptPaState, index 0 */
  WfwXptDpdEpdtIndStruct xptDpdEpdtLUT0[WFW_TX_MAX_NUM_XPT_STATES_SUPPORTED];
  /*! @brief LUT that contains DPD mapping for PA range eptPaState, index 1 */
  WfwXptDpdEpdtIndStruct xptDpdEpdtLUT1[WFW_TX_MAX_NUM_XPT_STATES_SUPPORTED];
  /*! @brief LUT that contains DPD mapping for PA range eptPaState, index 0, when HSPA linearizer override is used */
  WfwXptDpdEpdtIndStruct xptDpdEpdtHsLUT0[WFW_TX_MAX_NUM_XPT_STATES_SUPPORTED];
  /*! @brief LUT that contains DPD mapping for PA range eptPaState, index 1, when HSPA linearizer override is used */
  WfwXptDpdEpdtIndStruct xptDpdEpdtHsLUT1[WFW_TX_MAX_NUM_XPT_STATES_SUPPORTED];
} WfwEptDpdDynamicAsyncWriteStruct;

/*! @brief Structure definition for static async TX EPT DPD SW-FW write interface - WRITE ONLY BY RFSW */
typedef struct
{
  /*! @brief PA state (0/1/2/3) of which EPT is enabled, this also corresponds to the linearizer chosen for EPT */
  Word8 eptPaState;
} WfwEptDpdStaticAsyncWriteStruct;

/*! @brief Structure definition for async TX EPT DPD SW-FW write interface - WRITE ONLY BY RFSW */
typedef struct
{
  /*! @brief Static EPT async param, which is fixed during a call */
  WfwEptDpdStaticAsyncWriteStruct staticParam;
  /*! @brief Dynamic EPT async param, which can get updated everytime linearizer gets updated */
  WfwEptDpdDynamicAsyncWriteStruct dynamicParam;
} WfwEptDpdAsyncWriteStruct;

/*! @brief Structure definition for async TX EPT DPD SW-FW read interface - READ ONLY BY RFSW */
typedef struct
{
  /*! @brief Sample capture index -> range: -1 to (WFW_TX_NUM_MAX_EPT_CAL_CAPTURES_SUPPORTED-1), -1 if sample capture isn't ready */
  Word8 calSampleCaptureIndex;
  /*! @brief Indication of whether calibration database is ready */
  UWord8 calDpdDatabaseReady;
} WfwEptDpdAsyncReadStruct;

/*! @brief WCDMA FW UL HS DPCCH MDSP async write variables - READ ONLY BY L1 */
typedef struct
{
  /*! @brief HS Log structure */
  WfwTxHsLogStruct hsLog[WFW_MOD_NUM_HS_TX_BUF][WFW_MOD_NUM_HS_LOG_PACKETS];

  /*! @brief  Subframe that is logged at the time of EAGCH decode done in HS RACH */
  /*! @brief  Helps SW determine the first active HS DPCCH subframe # */
  UWord16 hsRachHspdschSfnAtEagchDecDone;

  /*! @brief HS-DPCCH pending config flag */
  UWord8 hsDpcchPendingCfg;

 /*! @brief HS DPCCH final subframe where the channel is terminated
      (Range 0-1279) */
  UWord16 finalSfn;

} WfwTxHsDpcchAsyncReadStruct;
/*!@}*/

/*! @brief CPC DTX EUL related vars updated per subframe */
typedef struct
{
    /*! @brief EUL transmission pending indicator: 1 = Has EUL Tx on cpcEdpchPending, 0 = no EUL Tx */
    UWord32 cpcEdpchPending : 1;
    /*! @brief EUL transmission start subframe number of next EUL transmission */
    UWord32 cpcEdpchNextStartSubframe : 15;
    UWord32 cpcEdpchCntr : 16;
    /*! @brief protect EDCH during DSDA and when CPC dtx is enabled */
    UWord8  cpcEdpchProtect;
   /*! @brief Not to protect EDCH during DSDA and when CPC dtx is enabled if last t within 40 ms */
    UWord8  dsdaeEdchHighPriorityTimer;
} WfwTxDtxEdpchParamStruct;

/*! @name Structure definition for async TX PLL SW-FW interface */
/*!@{*/
/*! @brief Structure definition for async TX PLL SW-FW write interface - WRITE ONLY BY L1 */
typedef struct
{
  /*! @brief TX PLL ON SBI control word */
  UWord32 sbiTurnOnTxPll;
  /*! @brief TX PLL OFF SBI control word */
  UWord32 sbiTurnOffTxPll;
  /*! @brief TX PLL Force ON override (1 = Disable power saving in CPC DTX Tx PLL) */
  UWord8 forceTxPllOn;
  /*! @brief BPG number to turn on BPG */
  UWord8 txPllTurnOnBpg;
} WfwDtxTxPllAsyncWriteStruct;
/*!@}*/

/*! @name Structure definition for async UL CPC DTX interface */
/*!@{*/
/*! @brief WCDMA CPC DTX async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /* Word 1 */
  /*! @brief DTX DRX offset in subframes, 0..159 */
  UWord32 dtxDrxOffset : 8;
  /*! @brief DTX mode 1 cycle length in subframes, max = 20 */
  UWord32 cycle1Length : 8;
  /*! @brief DTX mode 2 cycle length in subframes, max = 160 */
  UWord32 cycle2Length : 8;
  /*! @brief DTX mode 1 burst length in subframes: 1,2,5 */
  UWord32 burst1Length : 4;
  /*! @brief DTX mode 2 burst length in subframes: 1,2,5 */
  UWord32 burst2Length : 4;

  /* Word 2 */
  /*! @brief DTX short preamble length in slots, always 2 */
  UWord32 shortPreambleLength : 4;
  /*! @brief DTX long preamble length in slots: 2,4,15 */
  UWord32 longPreambleLength : 4;
  /*! @brief DTX Inactivity threshold for Cycle 2 in subframes, max = 256*5 */
  UWord32 cycle2InactivityThreshold : 12;
  /*! @brief DTX CQI timer in subframes: 1,2,4,...,512,inf (represented by WFW_TX_CPC_DTX_CQI_TIMER_INFINITY)*/
  Word32 cqiTimer : 12;

  /* Word 3, EUL related interface variable, updated per subframe for each carrier */
  WfwTxDtxEdpchParamStruct cpcEdpchParamWord[WFW_NUM_MAX_UPLINK_CARRIERS];


  /*! @brief CM filler enable for next frame, enable for each UL carrier */
  UWord8 cmFillNextFrame[WFW_NUM_MAX_UPLINK_CARRIERS];

  /* @brief Indicate an upcomfing DTX reconfig, FW uses this to switch to INACTIVE state at ensuing frame */
  UWord8 preReconfigIndicator;

  /*! @brief Enable PA control due to DTX states (default = 0 = disable) */
  UWord8 enableDtxPaCtrl;


  /*! @brief SW indicate FW the action CFN from where CPC DTX on sec carrier will be activated
      Set to -1 initially by SW, this is to be checked only if pri carrier is already CPC active and
      sec carrier is getting activated, if both pri and sec carriers were active at the time when request to start
      CPC action cfn, then this is not to be ignored and FW will use the sync interface action CFN */
  Word32 cpcDTxActionCfnSec;


  /*! @brief Structure to control TX PLL for power saving in DTX gap */
  WfwDtxTxPllAsyncWriteStruct txPllCtrl;


} WfwTxDtxAsyncWriteStruct;
/*!@}*/

/*! @brief DTX inactivity structure for logging */
typedef struct
{
  /* @brief inactivity count down cycle, from inact threshold to 0 */
  UWord16 inactCntDownCycle : 14;
  /* @brief DTX cycle state, cycle 1 = 0, cycle 2 = 1 */
  UWord16 cycleState : 1;
  /* @brief DTX active state, active = 1, inactive = 0 */
  UWord16 dtxActiveState : 1;
  /* @brief DTX DSDA event state for 5 subframes */
  UWord8 dtxDsdaMacInactEdch : 1;
  UWord8 reserved : 7;
} WfwTcDtxInactiveLogStruct;

/*! @brief DTX event queue type, supports number of bits in WfwTcDtxEventQueueBits
           Bit order
           0 Burst, Preamble/Postamble due to Bursts
           1 EDCH transmission
           2 Preamble/Postamble due to EDCH transmission
           3 HS ACK/NACK, Preamble/Postamble due to HS ACK/NACK
           4 HS CQI transmission
           5 Preamble/Postamble due to CQI transmission
           6 CM filler
           7 CM gap
*/
typedef UWord8 wfwTcDtxEventQueueType;

/*! @brief Number of subframes logged for DTX inactivity, 2 frames */
#define WFW_TX_CPC_INACT_LOG_LENGTH 10

/*! @brief DTX logging packet structure */
typedef struct
{
  /* @brief UL CFN number for log packet */
  UWord16 ulCfn;
  /* @brief ianctivity state and ctr for 10 subframes */
  WfwTcDtxInactiveLogStruct inactState[WFW_TX_CPC_INACT_LOG_LENGTH];
  /* @brief DTX event state for 15 slots */
  wfwTcDtxEventQueueType dtxEngineState[15];
} WfwTxDtxLoggingStruct;

/*! @brief DTX logging packet structure */
typedef struct
{
  /* @brief HS SCCH ORDER CNTR */
  UWord16 cntr;
  /*! @brief HS SCCH ORDER TYPE */
  UWord16 type;
  /*! @brief HS SCCH ORDER ACTION SFN */
  Word16 sfn;
} WfwTxDtxScchOrderStruct;


/*! @name Structure definition for async UL CPC DTX interface */
/*!@{*/
/*! @brief WCDMA CPC DTX async read variables - READ ONLY BY L1 */
typedef struct
{
  /*! @brief 0x422c log packet support */
  WfwTxDtxLoggingStruct logpkt[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief hs scch order */
  WfwTxDtxScchOrderStruct hsScchOrder;

} WfwTxDtxAsyncReadStruct;
/*!@}*/

/*! @name Structure definition for async UL RACH SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW UL RACH MDSP async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*! @name RACH async write vars - latched at start of RACH procedure */
  /*!@{*/
  /*! @brief Beta gain used for RACH preamble, default 0 */
  Word16 pmblBetaGain;
  /*! @brief Gain factor adjustment between preamble and message in dB10,
      (Range -512->512), default 0 */
  Word16 msgPmblPowerOffset;
  /*! @brief MPR bin for RACH preamble state (Range 0->5) */
  UWord8 pmblMtplBin;
  /*! @brief MPR bin for RACH message state (Range 0->5) */
  UWord8 msgMtplBin;
  /*! @brief BetaC gain used for RACH message, default 0 */
  Word16 msgBetaCGain;
  /*! @brief BetaD gain used for RACH message, default 0 */
  Word16 msgBetaDGain;
  /*! set 1 for rach preamle, 2 for message and 3 for other(DPCCH) */
  UWord8 regDumpCrashCallType;
  /*! Bpg count at wich crash the UE while Rach preamble or message */
  UWord8 regDumpCrashBpg;
  /*! Number of preammbles at wich UE need to crash */
  UWord8 regDumpCrashNumPreambles;
  /*!@}*/
} WfwTxRachAsyncWriteStruct;

/*! @brief WCDMA FW RACH logging interface - READ ONLY BY L1 */
typedef struct
{
  /* Word 1 */
  /*! @brief Beta HW value programmed for preamble */
  UWord32 hwPreamble : 16;       /* Bit 0-15 */
  /*! @brief AI Result: 0->Silence, 1->ACK(MSG DONE), 2->NACK, 3->ACK(AICH ACK), 4->ABORTED */
  UWord32 aiResult : 3;          /* Bit 16-18 */
  /*! @brief PA Range, 0->3 */
  UWord32 paRange : 2;           /* Bit 19-20 */
  /*! @brief Indicates if this index is valid for this RACH preamble */
  UWord32 valid : 1;             /* Bit 21 */
  /*! @brief TX AGC Value for this preamble */
  UWord32 txAgcValue : 10;       /* Bit 22-31 */

  /* Word 2 */
  /*! @brief AI Sum */
  Word32  aiSum;

  /* Word 3 */
  /*! @brief RSSI for fingers contributing to AI */
  UWord32  aiRssi;

  /* Word 4 */
  /*! @brief Bitmask of fingers contributing to AI detection -
             NC AI: is this needed? */
  UWord32  aiDemodBegun;

  /* Word 5 */
  /*! @brief Bitmask of fingers in lock for AI detection
             NC AI: is this needed? */
  UWord32  aiDemodFingLock;

} WfwTxRachLogStruct;

/*! @brief WCDMA FW UL RACH MDSP async read variables - READ ONLY BY L1 */
typedef struct
{
  /*! @brief TX Time upon RACH exit procedure */
  Word32 timeStatusDump;
  /*! @brief Power setting of last preamble or message in dB10 (Range -512->512) */
  /*  holds DPCCH Inital Power in case of a successful HS RACH procedure */
  Word16 lastPowerSetting;
  /*! @brief RACH status, 0->Max retrans reached, 1->Message success, 2->NACK received, 3->ACK Received */
  UWord8 status;
  /*! @brief Signature used for last trasmitted preamble (Range 0->15) */
  UWord8 lastSignature;
  /*! @brief Access slot in which last preamble was trasmitted (Range 0->12) */
  UWord8 lastAccessSlot;
  /*! @brief Total number of preambles transmitted during the procedure */
  UWord8 numAccessAttempts;
  /*! @brief PN count of last preamble sent on AICH channel */
  Word32 aichPnCnt;
  /*! @brief TX STMR time before last preamble (BIT 0:18 = subFrameCx8, BIT 19-30 = frame) */
  UWord32 txTime;
  /*! @brief TX STMR time at timing slam (BIT 0:18 = subFrameCx8, BIT 19-30 = frame) */
  UWord32 initTxTime;
  /*! @brief Ref finger position in cx8 units before preamble */
  UWord32 refFingPosCx8;
  /*! @brief TX time tracking accumulation since last timing slam in Cx8 */
  Word16  txTtAccumCx8;


  /*! @brief RACH Log structure for final two preamble attempts */
  WfwTxRachLogStruct log[WFW_MOD_NUM_RACH_LOG_PACKETS];    /*lint !e123 */

  /*! @brief E-AICH results */
  /* Decoded EAI metric */
  UWord16 eAiDecodeMetricMaxEnergy;
  /* AI/EAI decoded energy for each signiture */
  /* Even Indices are from AI signiture and odd inices from E-AI signitures*/
  Word32 eAiDecodeEnergy[32];
  /* EAI symbols */
  Word16 eAiDecodeInputSyms[32];

  /*! @brief eAiEdchRsrcIdx, 0-31*/
  UWord8 eAiEdchRsrcIdx;
  /*! @brief eAiDecodeStatus maps to the following enum */
  /*  WFW_RACH_EAICH_NOT_ATTEMPTED =0, ( in case of AI silence)
      WFW_RACH_EAICH_NOT_CONFIGURED_AND_NACK = 1, (in case of AI = -1 and HS RACH not cfg'ed)
      WFW_RACH_EAICH_DEFAULT_RSRC_IDX = 2, ( in case of AI = 1)
      WFW_RACH_EAICH_CONFIGURED_DEC_RESULT_PLUS_ONE = 3, (in case of AI = -1, HS RACH Cfg'ed)
      WFW_RACH_EAICH_CONFIGURED_DEC_RESULT_MINUS_ONE = 4, in case of AI = -1, HS RACH Cfg'ed)
      WFW_RACH_EAICH_ABORT = 5*/
  UWord8 eAiDecodeStatus;
   /*! @brief eAiSignature, 0-15, 255 if invalid*/
  UWord8 eAiSignature;
  /*! @brief eAiSuccAiPnCnt, 20n, 20n+10 n = 0..7*/
  UWord8 eAiSuccPnCnt;
  /*! @brief hsRachOverallDecStatus maps to the following enum */
  /*  WFW_RACH_OVERALL_SILENCE = 0,
      WFW_RACH_OVERALL_NACK = 1,
      WFW_RACH_OVERALL_ACK = 2,
      WFW_RACH_OVERALL_ABORT = 3*/
  UWord8 hsRachOverallDecStatus;

} WfwTxRachAsyncReadStruct;
/*!@}*/

/*! @name Structure definition for async TX AGC SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW TX Linerizer LUT structure - WRITE ONLY BY L1 */
typedef union
{
  struct
  {
    /* Word 0 */

    /*! @brief Setting for sbi register 4, BIT 0:6 */
    UWord32 phaseComp : 7;
    /*! @brief IQ attenuation factor in dB10, 0..-6.3dB, for temp-freq compensation, BIT 7-12 */
    UWord32 iqTempCompScalFac : 6;
    /*! @brief SMPS voltage setting, BIT 13-21. Do not move this. */
    UWord32 smpsVal : 9;
    /*! @brief TX AGC Value in db10 for these settings, BIT 22-31. Do not move this. */
    UWord32 txAgcValue : 10;

    /* Word 1 */

    /*! @brief Setting for sbi register 0:3, BIT 0:31 */
    UWord8 sbi03[4];

  } eptStruct;

  struct
  {

    /* Word 0 */

    /*! @brief Setting for sbi register 4, BIT 0:6 */
    UWord32 phaseComp : 7;
    /*! @brief Reserved */
    UWord32 reserved : 6;
    /*! @brief SMPS voltage setting, BIT 13-21. Do not move this. */
    UWord32 smpsVal : 9;
    /*! @brief TX AGC Value in db10 for these settings, BIT 22-31. Do not move this. */
    UWord32 txAgcValue : 10;

    /* Word 1 */

    /*! @brief Setting for sbi register 0:3, BIT 0:31 */
    UWord8 sbi03[4];

    /* Words 2 & 3 & 4*/

    /*! @brief Settings for SMPS on QPOET */
    UWord8 smpsValQpoet[WFW_QPOET_MAX_WRITES];
    /*! @brief IQ attenuation factor in dB10, 0..-25.5dB */
    UWord8 iqGainAdjFac;
    /*! @brief PA current settings */
    UWord8 paCurrent[WFW_PA_CURRENT_MAX_WRITES];

  } xptStruct;

  /*! @brief full linearizer LUT word */
  UWord32 word[5];

} WfwTxLinLutStruct;

/*! @brief AGC MPR bin structure for phase 2 backoff */
typedef struct
{
  /*! @brief Beta backoff for MPR bin in Q15, 1.0 is 32767 */
  UWord16 betaBackoff;
  /*! @brief MPR values for MPR bin in dB10, i.e. 0. 0.5dB, 1dB */
  UWord8  mprVal;
  /*! @brief TX AGC Offset for MPR bin, adj in txPdm, in dB10
      (Range -128->127 -> -10.7dB->10.6dB) */
  Word8 txAgcOffset;
} WfwTxAgcMprWriteStruct;

/*! @brief Structure to control QPA */
typedef struct
{
  /*! @brief PA Control SSBI target ID */
  UWord8 paCtrlSbiSel;
  /*! @brief SBI address word to turn on/off PA */
  UWord8 paOnOffSbiAddr;
  /*! @brief SBI address words to control PA range */
  UWord8 paRangeCtrlAddr[2];
  /*! @brief SBI data word to turn on PA */
  UWord8 paOnSbiData;
  /*! @brief SBI data word to turn off PA */
  UWord8 paOffSbiData;
  /*! @brief paRangeCtrlData[0] is for highest PA gain, paRangeCtrlData[3] is for lowest PA gain, 2 words per PA range control */
  UWord8 paRangeCtrlData[WFW_MOD_NUM_MAX_PA_STATES][2];
} WfwTxAgcQPAWriteStruct;

/*! @brief Structure to control TX power based GRFC trigger */
typedef struct
{
  /*! @brief Enable flag to trigger GRFC(s) based on TX power levels */
  Word8 trigGrfcEnabled;
  /*! @brief GRFC ID(s) to be triggered */
  Word8 trigGrfc[3];
  /*! @brief TX power level in dB10 to assert trigger */
  Word16 onThresholds[3];
  /*! @brief TX power level in dB10 to deassert trigger */
  Word16 offThresholds[3];
} WfwTxAgcTxPwrTriggerStruct;

/*! @brief WCDMA FW TX AGC MDSP async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*! @name General TX AGC & PA async write vars
      to be set up at initialization/configuration time */
  /*!@{*/
  /*! BTF Delay in Cx8 (nominally around 40 chips) */
  Word16  btfDelay;
  /*! @brief Specifies time in cx16 between early tx time and VGA gain SBI */
  Word16  txAgcUpdateAgcTime;
  /*! @brief Specifies time in cx16 between early tx time and TXAGC SBI valid value 0-614400 */
  Word32  txAgcUpdateSsbiTime;
  /*! @brief Specifies time after early TX strobe PA is turned on/off in Cx8,
      (Range 1->2048), default 1
      paUpdateTime[0]: PA_POWER_DOWN_TIME
      paUpdateTime[1]: PA_POWER_UP_TIME */
  UWord16 paPowerUpdateTime[2];
  /*! @brief Specifies delay between early TX strobe and when PA state is switched up or down in Cx8,
      (Range 1->2048), default 1
      paUpdateTime[0][0]: PA_RANGE 3,2,1->0
      paUpdateTime[0][1]: PA_RANGE 3,2->1
      paUpdateTime[0][2]: PA_RANGE 3->2
      paUpdateTime[1][0]: PA_RANGE 0->1
      paUpdateTime[1][1]: PA_RANGE 0,1->2
      paUpdateTime[1][2]: PA_RANGE 0,1,2->3 */
  UWord16 paUpdateTime[2][WFW_MOD_NUM_MAX_PA_STATES - 1];
  /*! @brief Specifies slot worth of per-BPG PA compensation at transition points in dB10,
      (Range -24->24), default all zero
      paComp[0][0][n]: PA_RANGE 3,2,1->0
      paComp[0][1][n]: PA_RANGE 3,2->1
      paComp[0][2][n]: PA_RANGE 3->2
      paComp[1][0][n]: PA_RANGE 0->1
      paComp[1][1][n]: PA_RANGE 0,1->2
      paComp[1][2][n]: PA_RANGE 0,1,2->3 */
  Word8  paComp[2][(WFW_MOD_NUM_MAX_PA_STATES - 1)][10];

  /*! @brief Structure of rise and fall thresholds for PA state
      paRiseFall[0] = Rise | Fall <= Check for going up to PA 1 (Fall should be -ve max)
      paRiseFall[1] = Rise | Fall <= Check for going up to PA 2 (Fall should be -ve max)
      paRiseFall[2] = Rise | Fall <= Check for going up to PA 3 (Fall should be -ve max)
      paRiseFall[3] = Rise | Fall <= Check for going dn to PA 0 (Rise should be +ve max)
      paRiseFall[4] = Rise | Fall <= Check for going dn to PA 1 (Rise should be +ve max)
      paRiseFall[5] = Rise | Fall <= Check for going dn to PA 2 (Rise should be +ve max) */
  Vect32  paRiseFall[2 * (WFW_MOD_NUM_MAX_PA_STATES - 1)];

  /*! @brief PA switch point reduction per MPR value, MPR=0,0.5,1.0,...,2.5, in dB12 */
  UWord8 paThreshholdOffset[WFW_MOD_NUM_SUPPORTED_MPR_BINS-1];

  /*! @brief Per-PA rot angle array */
  Word16  paRotAngle[WFW_MOD_NUM_MAX_PA_STATES];

  /*! @brief Per-PA Temperature compensation array */
  Word8   paSMPSComp[WFW_MOD_NUM_MAX_PA_STATES];
  /*! @brief Remapped PA state */
  Word8   paRangeMapped[WFW_MOD_NUM_MAX_PA_STATES];
  /*! @brief TX AGC PA Volt level change flag */
  Word8   paVoltLevelChangeUpdate;
  /*! @brief TX AGC PDM SSBI packet */
  Word32  txAgcSsbiPdmPacket;
  /*! @brief TX AGC SSBI array */
  Word32  txAgcSsbiPacket[WFW_TX_AGC_MAX_NUM_SBI];
  /*! @brief TX AGC IQ gain unit set point for APT */
  Word16  txAgcIqGainUnitySetPoint;
  /*! @brief TX AGC IQ gain unit set point for EPT */
  Word16  txAgcIqGainUnitySetPointEpt;
  /*! @brief TX AGC IQ gain unit set point for ET */
  Word16  txAgcIqGainUnitySetPointEt;
  /*! @brief TX AGC ENV scale unit set point for EPT */
  Word16  txAgcEnvScaleUnitySetPointEpt;
  /*! @brief TX AGC ENV scale unit set point for ET */
  Word16  txAgcEnvScaleUnitySetPointEt;
  /*! @brief DC-HSUPA TX AGC IQ gain unit set point for APT */
  Word16  txAgcIqGainUnitySetPointDcupa;
  /*! @brief DC-HSUPA TX AGC IQ gain unit set point for EPT */
  Word16  txAgcIqGainUnitySetPointEptDcupa;
  /*! @brief DC-HSUPA TX AGC IQ gain unit set point for ET */
  Word16  txAgcIqGainUnitySetPointEtDcupa;
  /*! @brief DC-HSUPA TX AGC ENV scale unit set point for EPT */
  Word16  txAgcEnvScaleUnitySetPointEptDcupa;
  /*! @brief DC-HSUPA TX AGC ENV scale unit set point for ET */
  Word16  txAgcEnvScaleUnitySetPointEtDcupa;
  /*! @brief TX AGC SBI Disable flag */
  Word8   txAgcSsbiDisabled;
  /*! @brief TX AGC disable flag */
  Word8   txAgcOnlyDisabled;
  /*! @brief TX AGC SBI select - chooses between PDM SSBI and SSBI interface (Replaced by txLinLutVer to support WTR) */
  Word8   txAgcSsbiSel;
  /*! @brief TX linearizer data interface version number
      Version 0 = Use TxPDM legacy interface
      Version 1 = Use Tx multi SSBI interface
      Version 2 = Use Tx multi SSBI interface to support EAPT and WTR phase compensation */
  Word8   txLinLutVer;
  /*! @brief TX AGC Num SBI - valid if SSBI interface selected */
  Word8   txAgcNumSsbi;
  /*! @brief SMPS Mode: 0-disabled (default), 1-TX PDM, 2-GPIO->GRFC */
  Word8   smpsMode;
  /*! @brief WTR phase compensation enable flag */
  Word8   phaseCompEnabled;

  /* ---------------------------------------------------------------------------*/
  /* Special NOTE: Do NOT insert any elements from txLinLutSize0 to txMprStruct */
  /*! @brief TX AGC PDM size (Range 0,2,4,8,16,32,64), set 0 */
  Word8   txLinLutSize0[WFW_MOD_NUM_MAX_PA_STATES];
  /*! @brief TX AGC PDM size (Range 0,2,4,8,16,32,64), set 1 */
  Word8   txLinLutSize1[WFW_MOD_NUM_MAX_PA_STATES];
  /*! @brief Size of TX LIN when HSPA is in use, max size 16, set 0 */
  Word8   txHsLinLutSize0;
  /*! @brief Size of TX LIN when HSPA is in use, max size 16, set 1 */
  Word8   txHsLinLutSize1;
  /*! @brief PA range (before remapping) of PA range which FW should use the txHsLinLUT instead of txLinLUT, -1=disable */
  Word8   txHsOverridePaRange;
  /*! @brief TXAGC trigger level in dB10 which FW should start override (in addition to matching PA range) during HSPA */
  Word16  txHsOverrideStartLevel;
  /*! @brief TXAGC trigger level in dB10 which FW should stop override (in addition to matching PA range) during HSPA */
  Word16  txHsOverrideStopLevel;
  /*! @brief TX AGC PDM linearizer LUT for PA state 0-3, set 0 */
  WfwTxLinLutStruct  txLinLut0[WFW_MOD_NUM_MAX_PA_STATES][64];
  /*! @brief TX AGC PDM linearizer LUT for PA state 0-3, set 1 */
  WfwTxLinLutStruct  txLinLut1[WFW_MOD_NUM_MAX_PA_STATES][64];
  /*! @brief TX AGC PDM linearizer LUT in HSPA and paRange = txHsOverridePaRange, set 0 */
  WfwTxLinLutStruct  txHsLinLut0[16];
  /*! @brief TX AGC PDM linearizer LUT in HSPA and paRange = txHsOverridePaRange, set 1 */
  WfwTxLinLutStruct  txHsLinLut1[16];
  /*! @brief TX AGC PDM linearizer LUT set index for FW to pick up, updated at frame boundary */
  Word8   txLinLutIdx;
  /*! @brief TX AGC PDM linearizer LUT set index (for highest PA state), picked up at frame boundary */
  Word8  txLinLutMaxPaIdx;
  /*! @brief PA range (before remapping) of PA range which FW should use txLinLutMaxPaIdx instead of txLinLutIdx */
  Word8  txLinLutMaxPaState;

  /*! @brief Structure for MPR bin based backoff */
  WfwTxAgcMprWriteStruct txMprStruct[WFW_MOD_NUM_SUPPORTED_MPR_BINS];
  /*!@}*/
  /* End Special NOTE */
  /* ---------------- */

  /*! @brief Per carrier freq comp in dB10 for DC to be applied in baseband.
      Needed since the linearizer is built for LO frequency (range -10->10 dB10) */
  Word16 freqComp[WFW_NUM_MAX_UPLINK_CARRIERS][WFW_MOD_NUM_MAX_PA_STATES];

  /*! @brief Global variables for linearizer LUT */

  /*! @brief Tracking mode: 0 = APT; 1 = EPT; 2 = ET */
  UWord8 xptMode[WFW_MOD_NUM_MAX_PA_STATES];

  /*! @brief Slave ID */
  UWord8 qpoetSlaveId;

  /*! @brief Qpoet addresses for ET */
  UWord8 qpoetAddrEt[WFW_QPOET_MAX_WRITES];

  /*! @brief Qpoet addresses for EPT */
  UWord8 qpoetAddrEpt[WFW_QPOET_MAX_WRITES];

  /*! @brief Delay value */
  Word32 delayValue;

  /*! @name General TX AGC & PA async write vars
      RACH: Latched before preamble/message, DPCCH: Latched at frame boundary */
  /*!@{*/
  /*! @brief Upper bound for TX power in dB10 (Range -512->511) */
  Word16 txGainUpperLimit;
  /*! @brief Lower bound for TX power in dB10 (Range -512->511) */
  Word16 txGainLowerLimit;
  /*! @brief Flag to freeze internal TX AGC Adjust value (Range 0,1) */
  UWord8 txFreezeAccum[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief Set to 1 to force PA off, 0 for normal operation, default 0 */
  Word8  paForceOff;

  /*! @brief Specifies override value for TX AGC adjust value in dB10 (Range -512->512) */
  Word16 txGainAdjWr[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief Flag to override internal TX AGC adjust value (Range 0,1) */
  UWord8 txGainAdjOverride[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief Flag to override internal PA Range value, (Range 0,1), default 0 */
  UWord8 paRangeOride;
  /*! @brief Specifies override value for PA Range, (Range 0->3), default 0 */
  UWord8 paRangeWr;
  /*!@}*/

  /*! @name QPA async write vars
      Support for Qualcomm Power Amplifier control */
  /*!@{*/

  /*! @brief Enable/Disable QPA support */
  UWord8 enableQpaSupport;

  /*! @brief Enable/Disable QTF Tuner Support */
  UWord8 enableQtfTunerSupport;

  /*! @brief Control word for QPA on/off, QPA PA Range controls */
  WfwTxAgcQPAWriteStruct qpaControl;

  /*! @brief  TX power level trigger support */
  WfwTxAgcTxPwrTriggerStruct txPwrTrig;

  /*! @name HDET async write vars
      FW cache invalidates as necessary when HDET is to be triggered */
  /*!@{*/

  /*! @brief SBI to be sent out per group */
  UWord32 hdetSbi[WFW_TX_AGC_HDET_NUM_SBI_GROUP][WFW_TX_AGC_HDET_MAX_SBI_PER_GROUP];
  /*! @brief Number of SBIs per group, Range 0->4 */
  UWord8  hdetNumSbi[WFW_TX_AGC_HDET_NUM_SBI_GROUP];

  /*! @brief Indicator to HDET that ADC is busy */
  UWord8 hdetAdcBusyFlagFromArm;
  /*! @brief Global disable flag for HDET */
  UWord8 hdetDisableFlag;

  /*! @brief HDET trigger power (default = 15dBm) */
  Word16 hdetTriggerLevel;

  /*! @brief HDET trigger period in BPG, 10ms=150, 100ms=1500, etc. */
  UWord16 hdetTriggerPeriod;

  /*! @brief Timer hysteresis delay in BPG */
  UWord16 timerHysteresisDelay;

  /* ----------------------------------------------------------------------------------- */
  /* Special NOTE: Do not modify these 4 and keep them at the very last of the interface */
  /*! @brief Replaces UMTS_AGC_TX/PA_ON_RISE/FALL_DELAY registers
      programmed by RFSW in legacy targets as TX_VALID is obsolete in NikeL. */
  /*! @brief To turn on RTR */
  UWord16 txOnRiseDelay;
  /*! @brief To turn off RTR */
  UWord16 txOnFallDelay;
  /*! @brief To turn on PA */
  UWord16 paOnRiseDelay;
  /*! @brief To turn off PA */
  UWord16 paOnFallDelay;
  /*! @brief pa quiscent current update */
  Word32  txAgcUpdatePaCurrentTime;
  /*! END Special NOTE */
  /* ----------------- */

  /*!@}*/

} WfwTxAgcAsyncWriteStruct;

/*! @brief WCDMA FW TX AGC MDSP HDET async read variables - READ ONLY BY L1 */
typedef struct
{
  /*! @brief Tx Time when HDET is triggered */
  UWord32 txCnt;
  /*! @brief TX Gain value when HDET is triggered */
  Word16 bufferedTxGain;
  /*! @brief Beta C value when HDET is triggered */
  Word16 betaCValue;
  /*! @brief Beta D value when HDET is triggered */
  Word16 betaDValue;
  /*! @brief Beta Hs value when HDET is triggered */
  Word16 betaHsValue;
  /*! @brief Beta RACH Preamble value when HDET is triggered */
  Word16 betaPmblValue;
  /*! @brief Beta Ec value when HDET is triggered */
  Word16 betaEcValue[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief Beta Ed value when HDET is triggered */
  Word16 betaEdValue[WFW_NUM_MAX_UPLINK_CARRIERS][WFW_MOD_NUM_SUPPORTED_EDPDCH];
  /*! @brief Max power value when HDET is triggered */
  Word16 mtpl;
  /*! @brief TxPdm when HDET is triggered */
  Word16 txPdmHdet;
  /*! @brief Indicator that MDSP is using ADC for HDET */
  UWord8 convStartedFlag;
  /*! @brief Indicator if HDET finishes to completion or aborted */
  UWord8 hdetInvalidFlag;
} WfwTxAgcHdetAsyncReadStruct;


/*! @brief WCDMA FW TX AGC MDSP async read variables - READ ONLY BY L1 */
typedef struct
{
  /*! @name General TX AGC async read vars
      RACH: Latched before preamble/message, DPCCH: Latched at frame boundary */
  /*!@{*/
  /*! @brief Max TX power, Range -512->512 */
  Word16 txGainUpperLimitRd;
  /*! @brief Rate Adj value */
  Word16 txRateAdjRd;
  /*! @brief TX AGC Value */
  Word16 txAgcValueRd;
  /*! @brief DPCCH power per carrier - to be removed */
  Word16 dpcchPowerValueRd[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief PA Range */
  UWord8 paRangeRd;
  /*! @brief PA disabled indicator */
  UWord8 paDisabled;
  /*! @brief TX PDM value */
  Word16 txPdmRd;
  /*! @brief TX Lin LUT idx */
  UWord16 txLinLutMatchIdxRd;
  /*! @brief TX Lin LUT idx */
  UWord16 txSmpsLutMatchIdxRd;
  /*! @brief TX SMPS value */
  Word16 txSmpsValueRd;
  /*! @brief PA current */
  UWord32 paCurrentRd;
  /*! @brief Winning index */
  Word32 winIdx;
  /*! @brief SMPS winning index */
  Word32 smpsWinIdx;
  /*! @brief Envelope scale */
  Word32 envScale;
  /*! @brief Delay value */
  Word32 delayValue;
  /*! @brief EPDT HW Base */
  uint16 epdtHwBase;
   /*! @brief IQ gain written to HW */
  Word32 iqGain;
  /*!@}*/

  /*! @brief Current active Tx LUT type */
  wfwTxLinLutTypeEnum activeTxLutTypeRd;

  /*! @name HDET async read vars */
  WfwTxAgcHdetAsyncReadStruct hdet;

} WfwTxAgcAsyncReadStruct;


/*! @brief EUL rate matching logging async read variables - READ ONLY BY L1 */
typedef struct
{
  /*! @brief Rate matching systematic bit Eini parameter */
  UWord32 eulRmSysEini;
  /*! @brief Rate matching systematic bit E+E- parameter */
  UWord32 eulRmSysEplusEminus;
  /*! @brief Rate matching parity1 bit Eini parameter */
  UWord32 eulRmP1Eini;
  /*! @brief Rate matching parity1 bit E+ parameter */
  UWord32 eulRmP1Eplus;
  /*! @brief Rate matching parity1 bit E- parameter */
  UWord32 eulRmP1Eminus;
  /*! @brief Rate matching parity2 bit Eini parameter */
  UWord32 eulRmP2Eini;
  /*! @brief Rate matching parity2 bit E+E- parameter */
  UWord32 eulRmP2EplusEminus;
  /*! @brief Rate matching redundancy version */
  UWord16 eulRmRv;
} WfwTxEULRateMatchLogStruct;


/*! @brief WCDMA FW EUL MDSP carrier specific async read variables - READ ONLY BY L1 */
typedef struct
{
  /*! @this flag is set by FW when sending out a DTX in the scenarios
      that SW is not able to complete the PDU calculation */
  UWord32 paramDTXCnt;
  UWord32 pduDoneDTXCnt;
  UWord32 pduOverdueDelayCnt;

  /*! @brief EUL TTI error code  */
  Word8 eulTTIErrorCode;

  /* Indicates Pending EUL TX Cfg*/
  UWord8 judcEulTxPendingCfg;

  /*! @brief Tx blanking on/off for this TTI */
  UWord8 isTxBlanked;

  /*! @brief EUL rate matching log packet */
  WfwTxEULRateMatchLogStruct rmLogPacket;

}WfwTxEULAsyncReadStruct;

/*! @brief WCDMA FW EUL MDSP async read variables (common to both carriers) - READ ONLY BY L1 */
typedef struct
{
  /*! @brief Counter than count out-of-sync EUL FIQ, used for dual FIQ sequencing */
  Word8 eulConsecutiveFiqMissCnt;

  /*! @brief Indicator for PreFIQ (-1) or FIQ (+1) */
  Word8 eulFiqIndicator;
}WfwTxEULCommonAsyncReadStruct;

typedef struct
{
  /*! fixed-point format Q20 value of DpcchPowerRatio used by MPR computation */
  UWord32 linDpcchPowerRatioSqQ20;
  /*! Worst case HsDataType */
  UWord32 wcHsDataType:  8;
  /*! DC values of mprVal */
  UWord32 mprVals:       8;
  /* subFrameNum passed down from SW */
  UWord32 subFrameNum:   8;
  UWord32            :   8;
}WfwTxDcMprStruct;

/*! @brief MPR information async read variables - READ ONLY BY L1 */
typedef struct
{
  /*! @brief HW BetaC programmed. 5 - for 5 subframes / frame */
  Word16 betaGainDpcchRd[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief HW BetaD programmed */
  Word16 betaGainDpdchRd;
  /*! @brief HW BetaHs programmed */
  Word16 betaGainHsDpcchRd;
  /*! @brief HW BetaEc programmed */
  Word16 betaGainEdpcchRd[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief HW BetaEd programmed */
  Word16 betaGainEdpdchRd[WFW_NUM_MAX_UPLINK_CARRIERS][WFW_MOD_NUM_SUPPORTED_EDPDCH];
  /*! @brief MPR Beta scaling value */
  Word16 mprBetaBackOffRd;
  /*! @brief MPR bin */
  UWord8 mprBinValRd;
  /*! @brief MPR reduction value */
  UWord8 mprReductionValRd;
  /*! @brief MPR txagc compensation value */
  Word8  txAgcOffsetRd;
  /*! @brief MPR calculation for DC */
  WfwTxDcMprStruct dcMprReductionRd[15];
} WfwTxMprAsyncReadStruct;

/*! @brief Tx Beta and EUL Params Override async write variables in FTM Mode */
typedef struct
{
  /*! @brief HW BetaC programmed */
  UWord32 betaGain0;
  /*! @brief HW BetaD programmed */
  UWord32 betaGain1;
  /*! @brief HW EDPCCH Params programmed */
  UWord32 eulParamsEdpcch;
  /*! @brief HW BetaEc programmed */
  UWord32 eulBetaGainEdpcch;
  /*! @brief HW EDPDCH Config programmed */
  UWord32 eulConfigEdpdch;
  /*! @brief HW EDPDCH Params programmed */
  UWord32 eulParamsEdpdch[WFW_MOD_NUM_SUPPORTED_EDPDCH];
  /*! @brief HW BetaEd programmed */
  UWord32 eulBetaGainEdpdch[WFW_MOD_NUM_SUPPORTED_EDPDCH];
  /*! @brief BetaGain override flag - if true, use above override values */
  Word8  enable;
} WfwTxBetaOverrideAsyncWriteStruct;

/*! @name Structure definition for async TX SW-FW interface */
/*!@{*/
/*! @brief Structure definition for async TX SW-FW write interface - WRITE ONLY BY L1 */
typedef struct
{
  /*! @brief TX AGC async write variables */
  WfwTxAgcAsyncWriteStruct agc;
  /*! @brief TX RACH async write variables */
  WfwTxRachAsyncWriteStruct rach;
  /*! @brief TX r99 DPCCH async write variables - common */
  WfwTxDpcchCommonAsyncWriteStruct r99Common;
  /*! @brief TX r99 DPCCH async write variables - per carrier */
  WfwTxDpcchAsyncWriteStruct r99[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief TX HS DPCCH async write variables */
  WfwTxHsDpcchAsyncWriteStruct hs;
  /*! @brief EUL async write variables - common */
  WfwTxEULCommonAsyncWriteStruct eulCommon;
  /*! @brief EUL async write variables - per carrier */
  WfwTxEULAsyncWriteStruct eul[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief CPC DTX async write variables */
  WfwTxDtxAsyncWriteStruct dtx;
  /*! @brief EPT DPD async write variables */
  WfwEptDpdAsyncWriteStruct ept;

 /*! @brief Tx Beta and EUL Params Override async write variables in FTM Mode */
  WfwTxBetaOverrideAsyncWriteStruct betaOverride[WFW_NUM_MAX_UPLINK_CARRIERS];

} WfwTxAsyncWriteStruct;

/*! @brief Structure definition for async TX SW-FW read interface - READ ONLY BY L1 */
typedef struct
{
  /*! @brief TX AGC async read variables */
  WfwTxAgcAsyncReadStruct agc;
  /*! @brief TX RACH async read variables */
  WfwTxRachAsyncReadStruct rach;
  /*! @brief TX r99 DPCCH async read variables - common */
  WfwTxDpcchCommonAsyncReadStruct r99Common;
  /*! @brief TX r99 DPCCH async read variables - per carrier */
  WfwTxDpcchAsyncReadStruct r99[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief TX HS DPCCH async read variables */
  WfwTxHsDpcchAsyncReadStruct hs;
  /*! @brief EUL async read variables - common */
  WfwTxEULCommonAsyncReadStruct eulCommon;
  /*! @brief EUL async read variables - per carrier */
  WfwTxEULAsyncReadStruct eul[WFW_NUM_MAX_UPLINK_CARRIERS];
  /*! @brief MPR async read variables */
  WfwTxMprAsyncReadStruct mpr;
  /*! @brief DTX async read variables */
  WfwTxDtxAsyncReadStruct dtx;
  /*! @brief EPT async read variables */
  WfwEptDpdAsyncReadStruct ept;
} WfwTxAsyncReadStruct;

/*!@ HS RACH structures*/
typedef struct
{
  /*! Is RGCH Enabled in this EDCH resource, 0->NOT Enabled, 1->enabled */
  UWord32 rgchEna:1;
  /*! RGCH Signature */
  UWord32 rgchSig:6;
  UWord32 :1;
  /*! HICH Signature */
  UWord32 hichSig:6;
  UWord32 :2;
  /*! HICH/RGCH Ovsf Code */
  UWord32 hiRgchOvsfCode:8;
  /*! UL Scrambling code type */
  UWord32 ulScrCodeType:1;
  UWord32 :7;
  /*! UL SCR code */
  UWord32 ulScrCode;
  /*! s_offset, 0-9 */
  UWord32 sOffset:4;
  /*! fdpch Ovsf Code */
  UWord32 fdpchOvsfCode:8;
  UWord32 :20;
} WfwEdchRsrcIdxToChanParamStruct;

typedef struct
{
  /*! array of  WfwEdchRsrcIdxToChanParamStruct structures */
  WfwEdchRsrcIdxToChanParamStruct wfwEdchChanMap[HS_RACH_MAX_NUM_EDCH_RSRCS];
  /*! Total num of EDCH resources allocated to this cell, 1-32 */
  UWord8 numEdchRsrcIndices;
} WfwEdchRsrcChanMappingStruct;

/*! @name Structure definition for async HS-RACH SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW HS RACH async write variables - READ ONLY BY L1 */
typedef struct
{

  /*! @brief F-DPCH TTO in BPGs */
  UWord8 fdpchTto;               /* Bit 0-7 */
  /*! @brief AGCH from F-DPCH offset in BPGs */
  UWord8 agchFromFdpchOffset;      /* Bit 0-7 */
  /*! @brief HICH from F-DPCH offset in BPGs */
  UWord8 hichFromFdpchOffset;      /* Bit 0-7 */
  /*! @brief RGCH from F-DPCH offset in BPGs */
  UWord8 rgchFromFdpchOffset;      /* Bit 0-7 */
  /*! @brief STMR Time @ AICH decode */
  UWord32 stmrTimeAtAichDecodeStart;  /* Bit 0-7 */
  /*! @brief STMR Time @ which F-DPCH goes active in chips */
  UWord32 stmrTimeAtFdpchFrmbry;   /* Bit 0-7 */
  /*! @brief STMR Time @ which AGCH goes active in chips */
  UWord32 stmrTimeAtAgchFrmbry;   /* Bit 0-7 */
  /*! @brief STMR Time @ which HICH goes active in chips */
  UWord32 stmrTimeAtHichFrmbry;   /* Bit 0-7 */
  /*! @brief STMR Time @ which RGCH goes active in chips */
  UWord32 stmrTimeAtRgchFrmbry;   /* Bit 0-7 */
  /*! @brief STMR Time @ which UL DPCCH goes active in chips */
  UWord32 stmrTimeAtdpcchFrmbry;  /* Bit 0-7 */
  /*! @brief UL DPCCH from Ref Timing offset in BPGs */
  UWord8 dpcchFromRefTimingOffset;      /* Bit 0-7 */
  /*! @brief S-Offset valid range 0-9 */
  UWord8 sOffset: 4;             /* Bit 0-3*/
  UWord8 : 4;                    /* Bit 4-7*/
  /*! @brief Tx TimeStamp at FIQ interrupt */
  /*! Tx Time is stored in following format
      UWord32 subBpgCx8 : 11;  Cx8 count within a bpg [0..2047]
      UWord32 subFrameBpg : 8; BPG count within a frame [0..149]
      UWord32 frameNum : 12;   Frame number [0..4095]
      UWord32 : 1;
  */
  UWord32  txTimeAtFIQIntrpt;
  /*! @brief Tx TimeStamp at PreFIQ interrupt */
  UWord32  txTimeAtPreFIQIntrpt;
} WfwHsRachChanTimingLogStruct;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* WFW_MOD_INTF_H */

