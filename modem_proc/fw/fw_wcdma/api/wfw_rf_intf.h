#ifndef RFW_RF_INTF_H
#define RFW_RF_INTF_H

/*! @brief
  @file

  @brief
  This module contains the definitions of the WCDMA RF-MDSP interface.
*/

/*===========================================================================

  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
04/25/13   CA      Add support for Dual Band Dual Carrier
07/25/12   TL      checked in changes made by RF-SW team.
07/02/12   TL      fixed CSim MDSP build failure. 
05/15/12   ac      initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                        CONSTANT DEFINITIONS

===========================================================================*/

#define WFW_RF_MOD_NUM_MAX_PA_STATES   4
#define WFW_RF_NUM_PA_ON               4
#define WFW_RF_NUM_PA_OFF              4
#define WFW_RF_NUM_TX_ON               2
#define WFW_RF_NUM_TX_OFF              2
#define WFW_RF_RX_LNA_NUM_TRANS                4
#define WFW_RF_LPM_NUM_TRANS                   4
#define WFW_RF_RX_LNA_NUM_STATES               6
#define WFW_RF_NUM_DEVICES                     2
#define WFW_RF_NUM_TRANS_PA_RANGE_MAX          5
#define WFW_RF_NUM_TRANS_TX_GAIN_SBI_MAX       4
#define WFW_RF_NUM_TRANS_TX_SMPS_MAX           (WFW_QPOET_MAX_WRITES)
#define WFW_RF_NUM_TRANS_PA_CURRENT_MAX        (WFW_PA_CURRENT_MAX_WRITES)
#define WFW_RF_NUM_TRANS_WRITE_HDET_MAX        8          /* estimate */
#define WFW_RF_NUM_TRANS_READ_THERM_MAX        0          /* currently not implemented */

/*! @brief WCDMA FW Tx PLL control maximum number of transactions */
#define WFW_RF_NUM_TRANS_TX_PLL_MAX    4

typedef enum
{
  WFW_RF_RX_INIT,                  
  WFW_RF_RX_SLEEP,     
  WFW_RF_RX_TUNE, 
  WFW_RF_RX_SELECT_BW,  
  WFW_RF_RX_SET_BAND,           
  WFW_RF_TX_INIT,  
  WFW_RF_TX_SET_BAND,
  WFW_RF_TX_TUNE,
  WFW_RF_TX_DISABLE,
  WFW_RF_READ_THERM, 
  WFW_RF_WRITE_HDET,
  WFW_RF_SET_TX_PLL,
  WFW_RF_SET_TX_ON,
  WFW_RF_SET_TX_OFF,
  WFW_RF_SET_PA_ON,
  WFW_RF_SET_PA_OFF,
  WFW_RF_SET_TX_AGC_PA_UP,
  WFW_RF_SET_TX_AGC_PA_DOWN,
  WFW_RF_SET_TX_AGC_NO_PA_UPDATE,
  WFW_RF_EVENT_NUM,
  WFW_RF_EVENT_MAX=WFW_RF_EVENT_NUM
} WfwSwCmdRfEventType;

typedef enum
{
  WFW_RF_PA_UP,
  WFW_RF_PA_DOWN,
  WFW_RF_NO_PA_RANGE_UPDATE,
  WFW_RF_TXAGC_EVENT_NUM,
  WFW_RF_TXAGC_EVENT_MAX = WFW_RF_TXAGC_EVENT_NUM
}WfwSwTxAgcEventType;

typedef enum
{
  WFW_RF_TX_GAIN_TASK,
  WFW_RF_PA_CURR_TASK,
  WFW_RF_PA_BIAS_TASK,
  WFW_RF_PA_RANGE_TASK,
  WFW_RF_TXAGC_TASK_NUM,
  WFW_RF_TXAGC_TASK_MAX = WFW_RF_TXAGC_TASK_NUM
}WfwSwTxAgcTaskType;

typedef enum
{
  WFW_RF_RFFE_TASK,
  WFW_RF_GRFC_TASK,
  WFW_RF_SSBI_TASK,
  WFW_RF_TASK_TYPE_NUM,
  WFW_RF_TASK_TYPE_MAX=WFW_RF_TASK_TYPE_NUM,
  WFW_RF_DUMMY_TASK = 0x7fffffff /* Ensure sizeof(WfwSwCmdRfTaskType) == 4 on both VP and WFW */
} WfwSwCmdRfTaskType;

/*! @brief WCDMA FW RF Rx LPM control Type (ON/OFF) */
typedef enum
{
  WFW_RF_RX_LPM_ON,
  WFW_RF_RX_LPM_OFF,
  WFW_RF_RX_LPM_CTL_NUM,
  WFW_RF_RX_LPM_CTL_MAX = WFW_RF_RX_LPM_CTL_NUM
} WfwSwRxLpmCtlType;

/*! @brief WCDMA FW RF Tx PLL control Type (ON/OFF) */
typedef enum
{
  WFW_RF_TX_PLL_ON,
  WFW_RF_TX_PLL_OFF,
  WFW_RF_TX_PLL_CTL_NUM,
  WFW_RF_TX_PLL_CTL_MAX = WFW_RF_TX_PLL_CTL_NUM
} WfwSwTxPllCtlType;

/*! @brief WCDMA FW TxAGC Task Info Type */
typedef struct
{
  WfwSwCmdRfTaskType smpsType;
  WfwSwCmdRfTaskType paCurrentType;
  WfwSwCmdRfTaskType txGainType;
  WfwSwCmdRfTaskType paRangeType;
  UWord32 numTxGain;
  UWord32 numPaCurrent;
  UWord32 numSmps;   
  UWord32 numPaRange;
  UWord8 txAgcTaskIdx[WFW_RF_TXAGC_TASK_NUM][WFW_RF_TXAGC_EVENT_NUM];  
} WfwRfTxAgcTaskInfoType;      

typedef struct
{
  WfwSwCmdRfTaskType txOnOffType;
  UWord32 numTxOn;
  UWord32 numTxOff;  
} WfwRfTxOnOffTaskInfoType;      // rfwcdma_mdsp_event_task_type;

typedef struct
{
  WfwSwCmdRfTaskType paOnOffType;
   UWord32 numPaOn;
   UWord32 numPaOff; 
} WfwRfPaOnOffTaskInfoType;      // rfwcdma_mdsp_event_task_type;

typedef struct
{
   WfwSwCmdRfTaskType rxAgcType;
   UWord32 numLnaState;
} WfwRfRxAgcTaskInfoType;      // rfwcdma_mdsp_event_task_type;

/*! @brief WCDMA FW RF Tx PLL control task info type */
typedef struct
{
   /*! @brief Tx PLL control bus protocol type */
   WfwSwCmdRfTaskType TxPllType;
   /*! @brief Tx PLL control number of transactions*/
   UWord32 numTxPll;
} WfwTxPllTaskInfoType;      // rfwcdma_mdsp_event_task_type;

typedef struct
{
  WfwSwCmdRfEventType eventId; 
  UWord32 smEventOffset;
} WfwSwCmdRfEventStruct;

/*! @brief Single SBI transaction.  */
typedef struct
{
  UWord8 sbiBus;
  UWord8 sbiAddr;
  UWord8 sbiData;
} WfwRfSbiPacketType;

/*! @brief SBI transaction information per LNA state */
typedef struct
{
  /* Number of SBI transactions */
  /* 1 - WFW_RF_RX_LNA_NUM_TRANS */
  UWord8 numTrans;
  WfwRfSbiPacketType trans[WFW_RF_RX_LNA_NUM_TRANS];
} WfwRfLnaTranType;

/*! @brief SBI transaction information for all LNA states */
typedef struct
{
  WfwRfLnaTranType lnaState[WFW_RF_RX_LNA_NUM_STATES];
} WfwRfLnaStateType;

/*! @brief LNA update information for a single antenna across all devices */
typedef struct
{
  /* Number of RF devices */
  /* 1 - WFW_RF_NUM_DEVICE */
  UWord8 numDevices;
  WfwRfLnaStateType lna[WFW_RF_NUM_DEVICES];
} WfwRfLnaInfoType;

/*! @brief SBI transaction information for LPM control */
typedef struct
{
  /* Number of SBI transactions */
  UWord8 numTrans;
  WfwRfSbiPacketType trans[WFW_RF_LPM_NUM_TRANS];
} WfwRfLpmTranType;

/*! @brief Information for LPM states */
typedef struct
{
  WfwRfLpmTranType lpmState[WFW_RF_RX_LPM_CTL_NUM];
} WfwRfLpmCtlStateType;

/*! @brief LNA update information for a single carrier across all devices */
typedef struct
{
  /* Number of RF devices */
  /* 1 - WFW_RF_NUM_DEVICE */
  UWord8 numDevices;
  WfwRfLpmCtlStateType lpm[WFW_RF_NUM_DEVICES];
} WfwRfLpmInfoType;

typedef struct
{
  WfwRfTxAgcTaskInfoType txAgcTaskInfo;
  WfwRfPaOnOffTaskInfoType paOnOffTaskInfo;
  WfwRfTxOnOffTaskInfoType txOnOffTaskInfo;
  WfwRfRxAgcTaskInfoType  rxAgcTaskInfo;
  /*! @brief WCDMA FW RF Tx PLL control task info type */
  WfwTxPllTaskInfoType  TxPllTaskInfo;
  UWord16 smEventOffsets[WFW_RF_EVENT_MAX]; /*array of offsets for the events*/
  /*! LPM control data for primary antenna */
  WfwRfLpmInfoType lpmCtlData;
  /*! LPM control data for diversity antenna */
  WfwRfLpmInfoType lpmCtlDataRxd;
  /*! LNA state update information for primary antenna */
  WfwRfLnaInfoType lnaStateData;
  /*! LNA state update information for diversity antenna */
  WfwRfLnaInfoType lnaStateDataRxd;
  UWord32 paStateData[WFW_RF_MOD_NUM_MAX_PA_STATES][WFW_RF_NUM_TRANS_PA_RANGE_MAX];
  UWord16 paOnData[WFW_RF_NUM_PA_ON];
  UWord16 paOffData[WFW_RF_NUM_PA_OFF];
  UWord16 txOnData[WFW_RF_NUM_TX_ON];
  UWord16 txOffdata[WFW_RF_NUM_TX_OFF];
  /*! @brief WCDMA FW RF Tx PLL data array for on/off control*/
  UWord16 txPllData[WFW_RF_TX_PLL_CTL_NUM][WFW_RF_NUM_TRANS_TX_PLL_MAX];
  UWord16 wfwRfeventSize[WFW_RF_EVENT_MAX];
	     
} WfwSwCmdRfSharedMemoryType;

/*! @brief specify adc_index for TQ locking purpose */
typedef enum 
{
    WFW_ADC_INDEX_0 = 0,
    WFW_ADC_INDEX_1,
    WFW_ADC_INDEX_2,
    WFW_ADC_INDEX_3,
    WFW_ADC_INDEX_MAX = 0xFFFFFFFF      /* make sure it's 32-bit */
} WfwSwCmdRfAdcIndexType;

typedef struct
{
  WfwSwCmdRfAdcIndexType primAdcIndex; 
  WfwSwCmdRfAdcIndexType secAdcIndex; 
} WfwSwCmdRfAdcTqConfigStruct;


#endif /* RFW_RF_INTF_H */

