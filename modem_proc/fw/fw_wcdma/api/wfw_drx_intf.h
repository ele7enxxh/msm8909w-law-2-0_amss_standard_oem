/*
  FILE wfw_drx_intf.h

  DRX interface.
*/

/*===========================================================================

  Copyright (c) 2009-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_drx_intf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/12   NC      Frame Early Termination & DCCH Early Detection Feature Updates
03/27/12   YL      added EDRX support
03/15/12   NC      Frame Early Termination & DCCH Early Detection Feature
01/26/12   EM      Expand SCCH order rece bit to handle CPC and DC scch order state
01/07/11   HK      Drx interface initial check-in
===========================================================================*/

#ifndef __WFW_DRX_INTF_H__
#define __WFW_DRX_INTF_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/* MAX SBI's used for DRX  */
#define WFW_CPC_MAX_PWR_SBI_NUM          8

/* MAX GRFC's used for DRX  */
#define WFW_CPC_MAX_PWR_GRFC_NUM         8

/* MAX log pkt buffers  */
#define WFW_CPC_DRX_MAX_LOG_PKT_BUF_LEN  15

/* MAX log pkt buffers  */
#define WFW_E_DRX_MAX_LOG_PKT_BUF_LEN  15

/* MULTISIM LOGGING REQ by SW */
#define WFW_DRXCTRL_MULTISIM_FIRST_SLOT_MASK     0x000003FF
#define WFW_DRXCTRL_MULTISIM_SECOND_SLOT_MASK    0x000FFC00
#define WFW_DRXCTRL_MULTISIM_THIRD_SLOT_MASK     0x3FF00000

/*! @brief WCDMA CPC DRX async write variables - WRITE ONLY BY L1  */
typedef struct
{
  /*! @brief inactivity threshold in subframes, 0..512 */
  UWord16 inactivityThreshold;
  /*! @brief cycle Length in subframes, 4..20 */
  UWord8 cycleLength;
  /*! @brief dtxDrxOffset should be a copy of CPC DTX parameters */
  UWord8 dtxDrxOffset;
  /*! @brief enable clk gating Feature for Dback/DEMSS during DRX*/
  UWord8 enableCpcLite;
} WfwCpcDrxCfgParamAsyncWriteStruct;


/*! @brief WCDMA CPC DRX rf plf - WRITE ONLY BY L1  */
typedef struct
{
  /* ssbi transaction ids*/
  UWord32 pwrOptsOnSsbiId[WFW_CPC_MAX_PWR_SBI_NUM];
  /* ssbi transaction ids*/
  UWord32 pwrOptsOffSsbiId[WFW_CPC_MAX_PWR_SBI_NUM];
  /* GRFC's transaction ids*/
  UWord32 pwrOptsOnGrfc[WFW_CPC_MAX_PWR_GRFC_NUM];
  /* GRFC transaction ids*/
  UWord32 pwrOptsOffGrfc[WFW_CPC_MAX_PWR_GRFC_NUM];
  /* Current power opts are prim/div pll and bbrx*/
  /* Num ssbi transactions to enable different blocks in the receive chain  */
  UWord8 numOnSsbiTrans;
  /* Num ssbi transactions to disable different blocks in the receive chain */
  UWord8 numOffSsbiTrans;
  /* Num grfc transactions to enable different blocks in the receive chain */
  UWord8 numOnGrfcTrans;
  /* Num grfc transactions to disable different blocks in the receive chain */
  UWord8 numOffGrfcTrans;
}WfwCpcDrxPwrOptsCfgAsyncWriteStruct;

/*! @brief WCDMA CPC DRX rx gating async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*flag to indicate sw doesnt need any data from the receive chain*/
  UWord8 gateRxActivity;
  /*flag to indicate SW has enabled Searcher and Demback for Reception*/
  UWord8 swPowerUpDone;
}WfwCpcDrxRxActivityCtlStruct;

/*! @brief WCDMA FET async params - WRITE ONLY BY L1 */
typedef struct
{
  /*! TX power level threshold to disable FET - db10 units (-70->32.3dBm) = (-512->511dB10) */
  Word16 maxTxPower;
  /*! Flag to indicate FET enabled */
  UWord8 enable;
  /*! Flag to indicate FET enabled if FR detected */
  UWord8 frEnable;
  /*! Flag to indicate FET TPC decode */
  UWord8 tpcDecodeEnable;
  /*! Flag to indicate FET mirror search enabled */
  UWord8 mirrorSrchEnable;
  /*! Flag to indicate gating FET if UL DCCH present */
  UWord8 ulSigGateEnable;
  /*! Enable early decode at 8ms */
  UWord8 dec8MsEnable;
  /*! Once a disagreement is seen within the 5-slot period, skip TPC demod for remainder of 5-slot period */
  UWord8 pca2FetEnhEnable;
  /*! Reduced rate TPC demod */
  UWord8 reducedRateTPCDemodEnable;
  /*! AMR mute operation state aware FET */
  UWord8 mosFetEnable;
} WfwFetDrxAsyncWriteStruct;

/*! @brief WCDMA CPC DRX rx gating async write variables - WRITE ONLY BY L1 */
typedef struct
{
  /*! Frame early termination DRX async write params */
  WfwFetDrxAsyncWriteStruct           fet;
  /*CPC DRX cfg async params*/
  WfwCpcDrxCfgParamAsyncWriteStruct   cfg;
  /*CPC DRX power opts params*/
  WfwCpcDrxPwrOptsCfgAsyncWriteStruct pwrOptsCfg;
  /*CPC DRX rx activity gating */
  WfwCpcDrxRxActivityCtlStruct        swRxCtl;
}WfwCpcDrxAsyncWriteStruct;

/*! @brief WCDMA CPC DRX Events slot logging  */
typedef struct
{
  /*Sw requested reception this slot*/
  UWord16 swReqReception:1;
  /*F-DPCH was received this slot*/
  UWord16 fdpchReception:1;
  /*scch was received this slot*/
  UWord16 scchReception:1;
  /*CM was active this slot*/
  UWord16 cmActive:1;
  /*TC warming up this slot*/
  UWord16 tcWarmup:1;
  /*AGC warming up this slot*/
  UWord16 agcWarmup:1;
  /*tc rake enabled this slot*/
  UWord16 tcRakeEnabled:1;
  /*EQ enabled this slot*/
  UWord16 eqEnabled:1;
  /*RX chain state for this slot
    0 - Rx chain was off
    1 - RX chain was turned on but waiting for warmup to complete
    2 - RX chain turned on and ready to receive data
    3 - Unused*/
  UWord16 rxChainOn:2;
  /* Small gap in this slot */
  UWord16 smallGap:1;
  /* Debug mode in this slot */
  UWord16 debugMode:1;
  UWord16 reserved:4;
}WfwCpcDrxSlotLevelEventsLogRec;

/*! @brief WCDMA CPC DRX Events subframe logging  */
typedef struct
{
  /*scch order received this subframe 
    0 - No Scch Order
    1 - CPC Scch Order
    2 - DC Scch Order
    3 - Unused */
  UWord32 scchOrdReceived:2;
  /*scch demod enabled - whether scch demod was enabled during this subframe*/
  UWord32 scchDemodEnabled:1;
  /*SCCH CRC state 0 - indicates failure 1 - indicates pass */
  UWord32 scchCrcStatus:1;
  /*log record subframe number of the scch max = 1280*/
  UWord32 scchSubfrNum: 11;
  /*Current value of inactivity counter*/
  UWord32 inactCntrVal: 10;
  /*! @brief UNUSED */
  UWord32 reserved : 7;
  /*slot level logging for this subframe*/
  WfwCpcDrxSlotLevelEventsLogRec slotLevelInfo[3];

  UWord32 multiSimGapInd;
}WfwCpcDrxLoggingRecStruct;

/*! @brief WCDMA CPC DRX logging : This should be SW R/W area  */
typedef struct
{
  /*write idx pointing to the current entry where the DRX log record is being updated by fw*/
  UWord32 wrPtr;
  /*! @brief 0x422c log packet support */
  WfwCpcDrxLoggingRecStruct logpkt[WFW_CPC_DRX_MAX_LOG_PKT_BUF_LEN];
} WfwCpcDrxLogAsyncReadStruct;

//ifdef FEATURE_WFW_E_DRX
/*! @brief WCDMA eDRX async write variables - 
      WRITE ONLY BY L1
      FW reads it and clear after latching
      YAN NOTE: will there be any race condition? */
typedef struct
{
  /* indicate pending Rx event: 1: ON; -1: off; 0: no event
     set 35 bpgs ahead of action time: 
   */
  Word16  eDrxRxEventPending;
  /* subframe number to start Rx Event; -1: immediate */
  Word16 eDrxRxEventStart;
  /* indicate HSSCCH ON or off event pending */
  Word16  eDrxHsEventPending;
  /* subframe number to start HS Event */
  Word16 eDrxHsEventStart; 
}WfwEdrxAsyncWriteStruct;

/*! @brief WCDMA eDRX Events subframe logging  */
typedef struct
{
  /*scch order received this subframe 
    0 - No Scch Order
    1 - CPC Scch Order
    2 - DC Scch Order
    3 - Unused */
  UWord32 scchOrdReceived:2;
  /*scch demod enabled - whether scch demod was enabled during this subframe*/
  UWord32 scchDemodEnabled:1;
  /*SCCH CRC state 0 - indicates failure 1 - indicates pass */
  UWord32 scchCrcStatus:1;
  /*log record subframe number of the scch max = 1280*/
  UWord32 scchSubfrNum: 11;
  /*! @brief UNUSED */
  UWord32 reserved : 17;
}WfwEdrxLoggingRecStruct;

/*! @brief WCDMA E DRX logging : This should be SW R/W area  */
typedef struct
{
  /*write idx pointing to the current entry where the DRX log record is being updated by fw*/
  UWord32 wrPtr;
  /*! @brief 0x422c log packet support */
  WfwEdrxLoggingRecStruct logpkt[WFW_E_DRX_MAX_LOG_PKT_BUF_LEN];
} WfwEdrxLogAsyncReadStruct;
//end FEATURE_WFW_E_DRX

/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                         FUNCTION PROTOTYPES

===========================================================================*/

#endif /* __WFW_DRX_INTF_H__ */
