/*!
  @file
  geran_msg.h

  @brief
  Definitions of GERAN UMIDs and message definitions

*/

/*=============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_geran/api/geran_msg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

=============================================================================*/

#ifndef GERAN_MSG_H
#define GERAN_MSG_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/
#include "msgr.h"
#include "gfw_sw_intf.h"
/*lint -save -e123 */
#include "cxm_intf_types.h"
/*lint -restore */ 
/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/


#define MSGR_MODULE_FW  0x00
#define MSGR_MODULE_FW2 0x01
#define MSGR_MODULE_FW3 0x04

#define MSGR_GERAN_FW   MSGR_TECH_MODULE( MSGR_TECH_GERAN, MSGR_MODULE_FW)
#define MSGR_GERAN_FW2  MSGR_TECH_MODULE( MSGR_TECH_GERAN, MSGR_MODULE_FW2)
#define MSGR_GERAN_FW3  MSGR_TECH_MODULE( MSGR_TECH_GERAN, MSGR_MODULE_FW3)

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*-----------------------------------------------------------------------*/


/*! @brief Asynchronous generic Command/Response messages

*/
typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

  GfwAsyncCmd  hostCmdType; //!< Host Command

  uint32       swFrameNo;   //!< GSM Hyper fn

  uint16        hwFrameNo;   //!< Frame count from GSTMR HW

  uint16       cmdSeqNum;   //!< Command Sequence number

  uint8        cmdBuffIdx;  //!< Index to host command buffer

} GfwHostAsyncCmd;

typedef struct
{
	msgr_hdr_s  hdr;       //!< MSGR header

	uint16      cmdSeqNum; //!< Payload

} GfwHostAsyncRsp;


/*! @brief Asynchronous sleep Command/Response messages

*/
typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

  GfwSleepCmdType  cmd; //!< Host Command

} GfwHostSleepCmd;

typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header
  boolean decodeDone;  //!< TRUE indicates that early decode has complete
} GfwHostAsyncSleepReadyCmd;

typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

  GfwSleepStateStruct gfwSleepState;

} GfwHostSleepRsp;


/*! @brief Asynchronous app mode config Command/Response messages

*/
typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

  GfwAppModeType  mode;

  uint32  rxlmBufIndex;     /*index for rxlm*/

  GfwDeviceModeType devMode;             /*Device Mode SS, DSDS and DSDA*/

  uint8 channel_id0;					 /* CxM channel	Id*/

} GfwHostAppModeConfigCmd;

typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

  GfwAppModeType  mode;

  uint32  rxlmBufIndex;     /*index for rxlm*/

} GfwHostAppModeConfigRsp;


/*! @brief Asynchronous WTR config Command/Response messages

*/
typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

  uint32  rxlmBufIndex;     //! < RXLM bufer index
  
  uint8 channel_id;					 /* CxM channel	Id*/

  uint8 deviceID;

} GfwHostWtrConfigCmd;

typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

} GfwHostWtrConfigRsp;


/*! @brief Asynchronous enter mode Command/Response messages

*/
typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

  GfwAppModeType mode;
  uint32 rxlmBufIndex;  //
  uint32 rxlmDrxBufIndex;
  uint8 deviceID;
  GfwEnterModeType enterMode;          // Primary / Secondary

} GfwHostRfmEnterModeCmd;

typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

} GfwHostRfmEnterModeRsp;

/*! @brief Asynchronous exit DRX mode Command/Response messages

*/
typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

} GfwHostRfmExitModeCmd;

typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header

} GfwHostRfmExitModeRsp;

/*! @brief Asynchronous immediate Command/Response messages

*/
typedef struct
{
	msgr_hdr_s   hdr;         //!< MSGR header

	uint32       swFrameNo;   //!< GSM Hyper fn

#ifndef FEATURE_UMPLT_GERAN
	uint8        hwFrameNo;   //!< Frame count from GSTMR HW
#else
	uint16        hwFrameNo;   //!< Frame count from GSTMR HW
#endif

	uint16       cmdSeqNum;   //!< Command Sequence number

	uint8        cmdBuffIdx;  //!< Index to host command buffer

} GfwHostAsyncImmediateCmd;

typedef struct
{
	msgr_hdr_s  hdr;       //!< MSGR header

	uint16      cmdSeqNum; //!< Payload

} GfwHostAsyncImmediateRsp;


/*! @brief Asynchronous set tx band Command/Response messages

*/
typedef struct
{
	msgr_hdr_s   hdr;         //!< MSGR header

} GfwHostRfmSetTxBandCmd;

typedef struct
{
	msgr_hdr_s   hdr;         //!< MSGR header

} GfwHostRfmSetTxBandRsp;


/*! @brief Asynchronous thermistor read Command/Response messages

*/
typedef struct
{
	msgr_hdr_s   hdr;         //!< MSGR header

} GfwHostRfmThermReadCmd;

typedef struct
{
	msgr_hdr_s   hdr;         //!< MSGR header

	uint32       payload;     //!< Generic payload

} GfwHostRfmThermReadRsp;


/*! @brief G2W Command mode

*/
typedef enum
{
  G2W_START,
  G2W_IN_PROGRESS,            //!< Used internally in GFW
  G2W_END,
  G2W_STOPPED                 //!< Used internally in GFW
} GfwG2WTaMode;

/*! @brief Asynchronous G2W Command

*/
typedef struct
{
    msgr_hdr_s    hdr;         //!< MSGR header
    GfwG2WTaMode  mode;        //!< G2W Mode START/END
} GfwHostG2WTaCmd;

/*! @brief Asynchronous exit command message

*/
typedef struct
{
	msgr_hdr_s   hdr;         //!< MSGR header

} GfwHostAsyncExitCmd;

/*! @brief Asynchronous update deviceMode command message

*/
typedef struct
{
   msgr_hdr_s        hdr;         //!< MSGR header
        GfwDeviceModeType devMode;     //!< device mode SS, DSDS, DSDA & TSTS
} GfwHostAsyncDeviceModeCmd;
//! @brief Asynchronous WLAN COEX command/response messages
typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header
  cxm_active_config_v01_s  message;     //! < RXLM bufer index
} GfwWlanCxmConfigCmd;

typedef struct
{
  msgr_hdr_s   hdr;         //!< MSGR header
  cxm_wcn_txfrmdnl_stats_v01_s  response;
} GfwWlanCxmConfigRsp;

/*! @brief Asynchronous update ccs TX status command message*/
typedef struct
{
  msgr_hdr_s  hdr;       //!< MSGR header

  uint16      ccsTxStatus; //!< Payload

} GfwAsyncCcsTxStatusCmd;


/*! @brief     */
enum
{
  /*! GL1 uses this message to send async commands to GFW */
  MSGR_DEFINE_UMID( GERAN, FW, CMD, ASYNC,  0x00, GfwHostAsyncCmd),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, APP_MODE_CONFIG,  0x01, GfwHostAppModeConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, APP_MODE_CONFIG,  0x02, GfwHostAppModeConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, RFM_ENTER_MODE,  0x03, GfwHostRfmEnterModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, RFM_ENTER_MODE,  0x04, GfwHostRfmEnterModeRsp),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, ASYNC,  0x05, GfwHostAsyncRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, SLEEP,  0x06, GfwHostSleepCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, SLEEP,  0x07, GfwHostSleepRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, SLEEP_READY,  0x08, GfwHostAsyncSleepReadyCmd),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, ASYNC_IMMEDIATE,  0x09, GfwHostAsyncImmediateCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, ASYNC_IMMEDIATE,  0x0A, GfwHostAsyncImmediateRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, RFM_SET_TX_BAND,  0x0B, GfwHostRfmSetTxBandCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, RFM_SET_TX_BAND,  0x0C, GfwHostRfmSetTxBandRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, RFM_THERM_READ,  0x0D, GfwHostRfmThermReadCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, RFM_THERM_READ,  0x0E, GfwHostRfmThermReadRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, ASYNC_EXIT,  0x0F, GfwHostAsyncExitCmd), 
  MSGR_DEFINE_UMID( GERAN, FW, CMD, G2W_TA,  0x10, GfwHostG2WTaCmd),  
  MSGR_DEFINE_UMID( GERAN, FW, CMD, RFM_EXIT_MODE,  0x11, GfwHostRfmExitModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, RFM_EXIT_MODE,  0x12, GfwHostRfmExitModeRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, DEVICE_MODE,  0x13, GfwHostAsyncDeviceModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, WTR_CFG,  0x14, GfwHostWtrConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, WTR_CFG,  0x15, GfwHostWtrConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, WLAN_CXM_POLICY,  0x16, GfwWlanCxmConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW, RSP, WLAN_CXM_POLICY,  0x17, GfwWlanCxmConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW, CMD, CCS_TX_STATUS,  0x18, GfwAsyncCcsTxStatusCmd),

  /*! GL1 uses this message to send async commands to GFW second instance */
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, ASYNC,  0x00, GfwHostAsyncCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, APP_MODE_CONFIG,  0x01, GfwHostAppModeConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, APP_MODE_CONFIG,  0x02, GfwHostAppModeConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, RFM_ENTER_MODE,  0x03, GfwHostRfmEnterModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, RFM_ENTER_MODE,  0x04, GfwHostRfmEnterModeRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, ASYNC,  0x05, GfwHostAsyncRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, SLEEP,  0x06, GfwHostSleepCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, SLEEP,  0x07, GfwHostSleepRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, SLEEP_READY,  0x08, GfwHostAsyncSleepReadyCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, ASYNC_IMMEDIATE,  0x09, GfwHostAsyncImmediateCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, ASYNC_IMMEDIATE,  0x0A, GfwHostAsyncImmediateRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, RFM_SET_TX_BAND,  0x0B, GfwHostRfmSetTxBandCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, RFM_SET_TX_BAND,  0x0C, GfwHostRfmSetTxBandRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, RFM_THERM_READ,  0x0D, GfwHostRfmThermReadCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, RFM_THERM_READ,  0x0E, GfwHostRfmThermReadRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, ASYNC_EXIT,  0x0F, GfwHostAsyncExitCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, G2W_TA,  0x10, GfwHostG2WTaCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, RFM_EXIT_MODE,  0x11, GfwHostRfmExitModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, RFM_EXIT_MODE,  0x12, GfwHostRfmExitModeRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, DEVICE_MODE,  0x13, GfwHostAsyncDeviceModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, WTR_CFG,  0x14, GfwHostWtrConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, WTR_CFG,  0x15, GfwHostWtrConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, WLAN_CXM_POLICY,  0x16, GfwWlanCxmConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW2, RSP, WLAN_CXM_POLICY,  0x17, GfwWlanCxmConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW2, CMD, CCS_TX_STATUS,  0x18, GfwAsyncCcsTxStatusCmd),

  /*! GL1 uses this message to send async commands to GFW third instance */
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, ASYNC,  0x00, GfwHostAsyncCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, APP_MODE_CONFIG,  0x01, GfwHostAppModeConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, APP_MODE_CONFIG,  0x02, GfwHostAppModeConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, RFM_ENTER_MODE,  0x03, GfwHostRfmEnterModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, RFM_ENTER_MODE,  0x04, GfwHostRfmEnterModeRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, ASYNC,  0x05, GfwHostAsyncRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, SLEEP,  0x06, GfwHostSleepCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, SLEEP,  0x07, GfwHostSleepRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, SLEEP_READY,  0x08, GfwHostAsyncSleepReadyCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, ASYNC_IMMEDIATE,  0x09, GfwHostAsyncImmediateCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, ASYNC_IMMEDIATE,  0x0A, GfwHostAsyncImmediateRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, RFM_SET_TX_BAND,  0x0B, GfwHostRfmSetTxBandCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, RFM_SET_TX_BAND,  0x0C, GfwHostRfmSetTxBandRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, RFM_THERM_READ,  0x0D, GfwHostRfmThermReadCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, RFM_THERM_READ,  0x0E, GfwHostRfmThermReadRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, ASYNC_EXIT,  0x0F, GfwHostAsyncExitCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, G2W_TA,  0x10, GfwHostG2WTaCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, RFM_EXIT_MODE,  0x11, GfwHostRfmExitModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, RFM_EXIT_MODE,  0x12, GfwHostRfmExitModeRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, DEVICE_MODE,  0x13, GfwHostAsyncDeviceModeCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, WTR_CFG,  0x14, GfwHostWtrConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, WTR_CFG,  0x15, GfwHostWtrConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, WLAN_CXM_POLICY,  0x16, GfwWlanCxmConfigCmd),
  MSGR_DEFINE_UMID( GERAN, FW3, RSP, WLAN_CXM_POLICY,  0x17, GfwWlanCxmConfigRsp),
  MSGR_DEFINE_UMID( GERAN, FW3, CMD, CCS_TX_STATUS,  0x18, GfwAsyncCcsTxStatusCmd),

};


#endif /* GERAN_MSG_H */
