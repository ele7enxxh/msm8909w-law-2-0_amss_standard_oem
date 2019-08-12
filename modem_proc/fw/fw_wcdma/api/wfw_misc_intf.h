/*!
  @file

  @brief
  WCDMA FW miscellaneous interface definitions header file
*/

/*===========================================================================

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_misc_intf.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/18/14   SK      Enable DR-DSDS support.
10/11/11   WX      Add RAFT interafce support
08/15/11   WX      Add xoSlamFlag in XO write interface for freq slamming during ACQ
08/03/11   WX      Add interface for CGAGC_SCHEME + SBI transaction time
07/22/11   WX      Remove common RF header file to unblock CSIM
07/15/11   WX      Modify GRFC buffer to match unified interface
05/26/11   WX      Correct comments for XO freq error unit 
05/20/11   WB      Add PLL settling time in cx1 to misc async write interface
04/26/11   WX      Add interface support for NikeL XO
04/26/11   WB      Update interface for W2X idle/CM: move rf/rxlm buffer indices to sync cmd interface
                   and remove from async
04/15/11   WB      Add deltaRxf struct for W2W tune-away 
04/07/11   WB      Increase number of RXF_REGS in order to support RAFT SPI 
04/07/11   WX      Change SBI packet data to structure
03/18/11   WX      Add RxLM buffer index for home frequency
07/07/10   NC      Fix units of txToRxFreqRatio from Q13 to Q15
05/26/10   NC      Increase max number of SBIs from 24 to 128
05/24/10   NC      Relocate and rename mdspFwAntClkCfg
05/21/10   WB      Antenna clock control (MDSP_FW_ANT_CLK_CFG) for IRAT 
05/06/10   EM      Add sbiCtrlParamStruct to sbi fields
03/12/10   WB      Increase num lna states for CM programming
08/10/09   PK      Enhanced TD detection support
06/01/09   EM      Add New SW interface for Tx DAC MN:D Counter
04/21/09   EM      Add New SW interface for MN:D Counter
11/14/08   NB      Add RF interface for CM
06/27/08   NC      Add singlePathCond to misc interface
06/06/08   NC      Fixed interface to match LCU
06/05/08   NC      Added file to hold STMR, VCTCXO, misc interfaces
===========================================================================*/

#ifndef WFW_MISC_INTF_H
#define WFW_MISC_INTF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "wfw_rxagc_intf.h"
#include "wfw_sw_cmd_proc_intf.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Public constants and enums */
/* -------------------------- */
#define WFW_NUM_LNA_STATES                 6
#define WFW_MAX_NUM_CM_SBI_TRANS         128
#define WFW_MAX_NUM_RXF_REGS              33
#define WFW_NUM_CM_INTERFREQ               4

/*! TX TO RX Freq Ratio Q factor */
#define WFW_TX_TO_RX_FREQ_RATIO_Q_FACTOR 15

/*! Maximum number of GRFC transactions */
#define RFMEAS_MAX_NUM_OF_GRFC  44

/*! Define feature for W2W tuning for RxD on CM gap exit*/
#define FEATURE_WFW_CM_W2W_RXD_SBI

/*! Number of Log Entries per WFW CXM Log Packet */
#define WFW_COEX_MGR_NUM_LOG_ENTRY_PER_PACKET   (30)
/*! Number of Log Buffer for WFW CXM Log Packet */
#define WFW_COEX_MGR_LOG_PACKET_BUF_LEN         (2)
/*! Indicate Invalid Log Packet Buffer Index */
#define WFW_COEX_MGR_LOG_PACKET_BUF_INDEX_INVALID (0xFF)

/*! Logging Definition in "Metric 1" column to indicate Primary/Secondary/Both 
    carrier blanking info */
#define WFW_COEX_MGR_LOG_PRIMARY_CARRIER_BLANK      (1)
#define WFW_COEX_MGR_LOG_SECONDARY_CARRIER_BLANK    (2)
#define WFW_COEX_MGR_LOG_BOTH_CARRIER_BLANK         (3)

/*! Number of Carriers for which AGC values are reported */
#define WFW_AGC_POWER_NUM_CARRIERS              (3)

/*! Number of Entries per WFW AGC Power Log Packet */
#define WFW_AGC_POWER_NUM_LOG_ENTRY             (30)

typedef enum
{
  WFW_CGAGC_1_STEP  = 0, 
  WFW_CGAGC_3_STEP,  
  WFW_CGAGC_NUM_SCHEMES
} wfwCgagcSchemeEnum;

/*! Number of BPGs for CGAGC settling time */
#define WFW_CGAGC_1_STEP_SETTLING_TIME   3
#define WFW_CGAGC_3_STEP_SETTLING_TIME   9    

#define WFW_NUM_TIMELINE_CHECKS 32 /* Must be a multiple of 4. An increase requires an update to Apex log packet 0x41C4*/

/* Structure definitions */
/* --------------------- */

/*! @brief WCDMA FW misc XO async write information structure */
typedef struct
{
  /*! Total frequency error  (applied to STMR, Tx/Rx resampler) in Q6 Hz */
  Word32 totalFreqError;
  /*! Inverse of the downlink frequency to calculate ppm, 2^33/Downlink_frequency, Q19, per band */
  UWord32 invDLOFreq[2];
  /*! Cx8 TTL Accum threshold - default (13333*10/6) since runs per 6-BPG */
  UWord16 cx8TtlAccumThresh;
  /*! TX to RX frequency ratio in Q15 - default 0x8000 */
  UWord16 txToRxFreqRatio;
  /*! (TC)XO HW Freq hypothesis, units of 7.324/16 Hz - default 0 */
  Word16 xoFreqHypothesisHw;
  /*! Weighted frequency bias - default 0 */
  Word16 wtFreqBias;
  /*! Flag indicating FW to use (TC)XO HW Freq hypothesis - default 0 */
  UWord8 xoUseFreqHypothesis;
  /*! Flag indicating (TC)XO frozen - default 0 */
  UWord8 xoFreezeFlag;
  /*! Flag indicating spectral inversion - default 0 */
  UWord8 spectralInversion;
  /*! Flag indicating slamming of the XO init frequency error */
  UWord8 xoSlamFlag;
} WfwMiscXoAsyncWriteStruct;

/*! @brief WCDMA FW misc XO async read information structure */
typedef struct
{
  /*! Total frequency error (outerloop + innerloop) in Q6 Hz */
  Word32 totalFreqError;
  /*! Rpush value for GPS (filtered value of total freq error), in Q6 Hz */
  Word32 xoRPushValue;
  /*! (TC)XO Current Frame Adj Accum, in 7.32/256 Hz */
  Word32 xoAdjAccumCurr;
  /*! (TC)XO Previous Frame Adj Accum, in 7.32/256 Hz */
  Word32 xoAdjAccumPrev;
  /*! (TC)XO Unbiased weighted freq common, in 7.32/256 Hz */
  Word16 xoUnBiasedWtFreqSum;
  /*! (TC)XO inner loop freq common, in 7.32/16 Hz */
  Word16 xoFreqCommon;
} WfwMiscXoAsyncReadStruct;

/*! @brief WCDMA FW misc DL async write information structure */
typedef struct
{
  /*! Threshold for TD detection */
  Word32 tdDetThresh;
  /*! Flag indicating single path condition */
  UWord8 singlePathCond;
  /*! UNUSED, TO BE REMOVED WHEN SW CHANGES ARE DONE: Antenna clock configuration to be written to MDSP_FW_ANT_CLK_CFG */
  UWord32 mdspFwAntClkCfg;
} WfwMiscDlAsyncWriteStruct;


/*! @brief SW to FW Misc VRT control parameters */
typedef struct
{
  /* Set to 1 to force the VRT-to-PCCPCH BPG offset to be the value specified
  in the activationOffset member of this structure. This will occur when the VRT is established, which is the
  only time that this parameter is referenced.  Should be set to 0 by default */
  UWord8 forceActivationOffset;

  /* Set to 1 to disable FW VRT tracking, primarily for simulation purposes.
  Should be set to 0 by default. */
  UWord8 disableVrtTracking;

  /* If forceActivationOffset = 1, VRT-to-PCCPCH offset will be forced to this value. Valid range is 0-9 BPGs */
  UWord16 activationOffset;
} WfwVrtCtrlStruct;

/*! @brief WCDMA FW misc async read information structure */
typedef struct
{
  /*! Misc XO SW R/W interfaces */
  WfwMiscXoAsyncWriteStruct xo;
  /*! Misc DL SW R/W interfaces */
  WfwMiscDlAsyncWriteStruct dl;

  /* VRT control */
  WfwVrtCtrlStruct vrtCtrl;

	UWord32 x2wRfStartupTimeCx8;

  /*! PLL settling time in cx8 */
  UWord32 rfPllSettlingTimeCx8;
  /*! CGAGC settling scheme */
  wfwCgagcSchemeEnum cgagcScheme;

  /*!RF preload_offset in cx8: tune away, nbr; must be less than num Cx8's in a frame */
  UWord32 rfMeasPreloadOffsetTimeCx8;

  /*!RF preload_offset in cx8: tune back, home; must be less than num Cx8's in a frame */
  UWord32 rfHomePreloadOffsetTimeCx8;

  /*! Flag to bypass undoAGC in RxAGC */
  UWord32 disableUndoAGC;

} WfwMiscAsyncWriteStruct;


/*! @brief Logged info related to FW timeline checks */
typedef struct
{
  /* Timestamps */
  UWord32 wstmr;
  UWord32 ustmr;
  UWord32 pcycles;
  /* Only valid if UL-DPCCH is enabled */
  UWord8  cfn;

  union
  {
  uint8 event[WFW_NUM_TIMELINE_CHECKS];
  uint32 fourEvents[WFW_NUM_TIMELINE_CHECKS/4];
  };
} WfwTimelineCheckReadStruct;

/*! @brief WCDMA FW misc async read information structure */
typedef struct
{
  /*! Misc XO SW RO interfaces */
  WfwMiscXoAsyncReadStruct xo;

/*! Logged info related to FW timeline checks */
  WfwTimelineCheckReadStruct timeChecks;
} WfwMiscAsyncReadStruct;

/*! @brief WCDMA FW W2W transition register list */
typedef struct
{
  /*! @brief Array of register addresses */
  UWord32 regAddr[WFW_MAX_NUM_RXF_REGS];
} WfwCmRxFRegisterListStruct;

/*! @brief WCDMA FW W2W transition AGC parameters list */
typedef struct
{
  /*! Init AGC value per LNA state before starting CGAGC processing */
  Word32 ffagcValueCmInit[WFW_NUM_LNA_STATES];
  /*! Fall threshold from transition from LNA state 2 to state 1 */
  Word32 ffLnaFall1;
  /*! Fall threshold from transition from LNA state 1 to state 0 */
  Word32 ffLnaFall2;
  /*! Rise threshold from transition from LNA state 2 to state 3 */
  Word32 ffLnaRise1;
  /*! Rise threshold from transition from LNA state 3 to state 4 */
  Word32 ffLnaRise2;
  /*! Rise threshold from transition from LNA state 4 to state 5 */
  Word32 ffLnaRise3;
  /*! Rise thresholds for FGAGC for each LNA state */
  Word32 lnaRise[WFW_NUM_LNA_STATES-1];
  /*! Fall thresholds for FGAGC for each LNA state */
  Word32 lnaFall[WFW_NUM_LNA_STATES-1];
  /*! Linearizer underflow (LNA 1 bypassed). */
  Word16 agcValMin[WFW_NUM_LNA_STATES];
  /*! LNA Gain Offset for state 0-4 */
  Word16 lnaOffset[WFW_NUM_LNA_STATES];
  /*! Specifies the VGA Gain offset to get the final
     rxAgcValue in the gain range of the DVGA */
  Word16 agcDelta;
  /*! LNA state information, holds the address and data for each LNA gain state */
  WfwRfLnaInfoType rxLnaStateInfoPacket;
} WfwCmRfCgagcCtrlParamsStruct;

/*! @brief Union for SBI_HW_REQ programmed value, 
RF drivers fill the struct, FW programs the UWord32 */
typedef union
{
  /*! 32 bit value to be programmed to MDSP_SBI_HW_REQ */
  UWord32 sbiCtrlValue;
  struct
  {
    /*! Data field (Bits 0-7) */
    UWord8 data;
    /*! SBI Address field (Bits 8 - 15) */
    UWord8 addr;
    /*! SBI slave ID field (Bits 16- 23) */
    UWord8 slave_id;
    /*! Control fifo field (Bits 24-31) */
    UWord8 ctrl_fifo;    
  }sbiCtrlParamStruct;
} WfwSbiCtrlUnion;

/*!
  @brief
  GRFC data structure for passing nonSBI script to FW.
  Warining:Note that the start and stop logic in the interface
  is not tested.
*/
typedef struct
{
  /*! @brief GRFC engine Id to be activated/de-activated */
  UWord16 grfc_id;
  /*! @brief start time offset for activate*/
  UWord16 start_delta;
  /*! @brief stop time offset for de-activate */
  UWord16 stop_delta;
  /*! @brief type of action.Activate/deactivate */
  UWord8 start_logic;
  /*! @brief type of action.Activate/deactivate */
  UWord8 stop_logic;
} rfmeasGrfcType;

#define WFW_MAX_NUM_OF_ANT_TUNE_SPI_DEV 4
/*!
  @brief
  Antenna tuner SPI information Data structure.
*/
typedef struct
{
  UWord8 cs_n[WFW_MAX_NUM_OF_ANT_TUNE_SPI_DEV];             /* CS_N for each according codeword */
  UWord32 tune_info[WFW_MAX_NUM_OF_ANT_TUNE_SPI_DEV];       /* array of total max of 4 devices */
} rfmeasSpiAntTunerParamType; 

/*! @brief WCDMA FW W2W transition SBI/GRFC programming struct */
typedef struct
{ 
  union
  {
    struct
    {
      /*! GRFC ctrl value buffer */
      rfmeasGrfcType grfc_buffer[RFMEAS_MAX_NUM_OF_GRFC];   
      /*! Number of GRFC transactions */
      UWord8 numGrfcTransactions;
      /*! Bit Mask for GRFC's to be written */
    };
    struct
    {
      /*! Bit Mask for GRFC's to be written */
      UWord32 grfcModeCtrlMask;
      /*! GRFC values to be written (0 or 1) to GRFC's specified by bit Mask */
      UWord32 grfcModeCtrlData;
    };
  };
  /*! Values to be written to SBI HW REQ register */
  WfwSbiCtrlUnion sbiCtrl[WFW_MAX_NUM_CM_SBI_TRANS];
  /*! Number of SBI transactions */
  UWord16 numSbiTransactions;
  /*! Number of SBI transactions for Diversity chain */
  UWord16 numSbiTransactionsDiv;

  /* SPI transaction */
  rfmeasSpiAntTunerParamType spiAntTunerParamType;

} WfwCmRfSbiGrfcStruct;

/*! @brief WCDMA FW Rx frontend register programming struct */
typedef struct
{
  /*! Number of Rx Frontend registers to program */
  UWord16 numRxFRegisters;
  /*! Values to be programmed */
  UWord32 rxfRegVal[WFW_MAX_NUM_RXF_REGS];
  /*! Addresses to be programmed */
  UWord32 rxfRegAddr[WFW_MAX_NUM_RXF_REGS];
} WfwCmRfRxfDeltaRegisterStruct;

/*! @brief XO parameters for the neighboring W frequency */
typedef struct
{
  /*! Total frequency error  (applied to STMR, Tx/Rx resampler) in Q6 Hz */
  Word32 totalFreqError;
  /*! inverse of the downlink frequency to calculate ppm, 2^33/Downlink_frequency, Q19 */
  UWord32 invDLOFreq;
}WfwCmXoParamsStruct;

/*! @brief WCDMA FW W2W transition programming structure per band */
typedef struct
{
  /*! AGC params */
  WfwCmRfCgagcCtrlParamsStruct agcCtrlParams;
  /*! SBI/GRFC params */
//  WfwCmRfSbiGrfcStruct sbigrfc;  obsolete with Dime, replaced with Rf Scripts in Common FW Area
  /*! XO params for neighbor frequency */
  WfwCmXoParamsStruct xo;
} WfwW2WRfBandInfoStruct;

/*! @brief WCDMA FW async params for programming band transitions */
typedef struct
{
  /*! Delta RxFe HW registers to be programmed */
  WfwCmRfRxfDeltaRegisterStruct deltaRxf[WFW_NUM_CM_INTERFREQ];
  /*! RF tuning information for 3 bands */
  WfwW2WRfBandInfoStruct bandInfo[WFW_NUM_CM_INTERFREQ];
} WfwCmRFControlInfoAsyncWriteStruct;

/*! @brief WFW CXM Log Packet: Log Entry per 0.5 slot */
typedef struct
{
    /*! USTMR Start Time */
    UWord32         ustmr;
    /*! WSTMR Start Time in CX1 */
    UWord32         wstmr;
    /*! Priority of Rx Activity Report */
    UWord16         activityReportRx;
    /*! Priority of Tx Activity Report */
    UWord16         activityReportTx;    
    /*! USTMR at Rx Blank Start */
    UWord32         rxBlankStart;
    /*! USTMR at Rx Blank End */
    UWord32         rxBlankEnd;
    /*! USTMR at Tx Blank Start */  
    UWord32         txBlankStart;
    /*! USTMR at Tx Blank End */
    UWord32         txBlankEnd;
    /*! USTMR at R99 Null WCB Start */
    UWord32         r99NullWCBStart;
    /*! USTMR at R99 Null WCB End */
    UWord32         r99NullWCBEnd;
    /*! USTMR when TX IQ Nulling Start */
    UWord32         txIQBlankingStart;
    /*! USTMR when TX IQ Nulling End */
    UWord32         txIQBlankingEnd;
    /*! USTMR when TX PA OFF */
    UWord32         txPAOff;
    /*! USTMR when TX PA ON */
    UWord32         txPAOn;
    /*! Rx Frequency ID */
    UWord32         rxFreqID;
    /*! Tx Frequency ID */
    UWord32         txFreqID;
    /*! Metric 1 for debugging */
    UWord32         metric1;
    /*! Metric 2 for debugging */
    UWord32         metric2; 
}WfwCoexMgrLogEntry;

/*! @brief WFW CXM Log Packet: Log Packet every WCDMA Frame */
typedef struct
{
    /*! CFN for the Log Packet */
    UWord16             cfn;
    /*! SFN for the Log Packet*/
    UWord16             sfn;
    /*! USTMR at CFN start */
    UWord32             cfnStartUstmr;
    /*! WSTMR at CFN start in CX1 */
    UWord32             cfnStartWstmr;
    /*! 30 Log entries; Each for 0.5 slot */
    WfwCoexMgrLogEntry  logEntry[WFW_COEX_MGR_NUM_LOG_ENTRY_PER_PACKET];
}WfwCoexMgrLogPacket;

/*! @brief WFW CXM Log Packet: FW-SW Interface Buffer */
typedef struct
{
    UWord8 bufIdx;
    /*! Blanking scenario indicates if Tx/Rx blanking actually happened in current log packet*/
    UWord8 blankingScenario[WFW_COEX_MGR_LOG_PACKET_BUF_LEN];
    WfwCoexMgrLogPacket wfwCxmLogPacket[WFW_COEX_MGR_LOG_PACKET_BUF_LEN];
}WfwCoexMgrLogAsyncReadStruct;

/*! @brief RXAGC Power Report Log Packet: FW-SW Interface Buffer */
typedef struct
{
    UWord32 wrIdx;
    Word16 agcValInst[WFW_AGC_POWER_NUM_CARRIERS];
    Word16 agcValue[WFW_AGC_POWER_NUM_CARRIERS][WFW_AGC_POWER_NUM_LOG_ENTRY]; 
}WfwAgcPowerLogAsyncReadStruct;

/*! @brief WFW CXM Async Cmd Buffer for Priority Table and Freq Table: FW-SW Interface Buffer */
typedef struct
{
    /*! @brief Flag to indicate CxM Params to be read from Async Memory */
    UWord8 asyncCxmParamsReadFlag;
    WfwCoexPriorityTable asyncPriorityTable;
    WfwCoexFreqIDTableStruct asyncFreqTable;
    WfwCoexChannelIDStruct channelID;    
}WfwCoexMgrParamsAsyncWriteStruct;

/*! @brief WFW Async Buffer for Sample Server Information */
typedef struct
{
    /* Virtual address of start of sample servers */
    UWord32 ssBaseAddr[WFW_RXAGC_MAX_ANTENNAS][WFW_RXAGC_MAX_CARRIERS];
}WfwSSAsyncRdStruct;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* WFW_MISC_INTF_H */

