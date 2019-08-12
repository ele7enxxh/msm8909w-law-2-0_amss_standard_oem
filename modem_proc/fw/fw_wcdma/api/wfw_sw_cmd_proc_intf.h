/*============================================================================*/
/*!
  @file

  @brief
  WCDMA FW-SW command processing interface definition header file
*/
/*============================================================================*/

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_sw_cmd_proc_intf.h#1 $ 
$DateTime: 2016/12/13 08:00:08 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/18/14   SK      Enable DR-DSDS support.
04/29/13   DG      SWI+API changes to separate sample capture and processing.
04/12/13   CA      Added support for Dual Band Dual Carrier to RxAGC
01/31/13   IM      Adding new parameter, wfwXptParams, to sample capture command structure
01/11/13   IM      Adding new ET TXLM config cmd.
12/03/12   MG      HS-RACH API changes: 1)Add availCellSignatureMask into RACH cfg.
                   2)Change JUDC DL and UL channel action from Enum type to UWord8.
07/31/12   IM      Added capture type member to SAPT capture command structure
07/05/12   MG      Added EPT calibration support
07/05/12   MM      Offline PICH feature added.
06/26/12   TL      Support SW RF commands
06/07/12   PT      Extended the action type for RACH cfg command to include HS_RACH_ENABLE action
05/24/12   SS      Added HS-RACH feature
03/01/12   NG      Support SAPT feature
01/13/12   PT      Support HS-FACH
10/28/11   KT      changed reserved bits WfwSWCmdTxEulCfgStruct to support carrierId
10/26/11   WB      Support RxD mode when coming back to W in W2X CM/Idle
10/07/11   RP      support ARD feature
08/13/11   WB      Fix bitwidth of x2wMode field
07/19/11   NB      DAC calibration support for WFW
07/17/11   WB      Add interface to support step1/NASTT in L2W connected
07/13/11   WB      Add X2W mode for L2W idle reconf
06/09/11   GS      Removed feature FEATURE_WCDMA_MDSP_MSG_BASED_SW_INTF_SUPPORT
                   and related definition from interface files.
05/04/11   WB      Combine X2W_MODE and RFTUNE commands
04/28/11   YL      logging interface for QICE and cleanup
04/26/11   WB      Update interface for W2X idle/CM: move rf/rxlm buffer indices to sync cmd interface
                   and remove from async
04/26/11   FT      Updated TxLM related ENUM
04/22/11   WX      Add command for IQ sample capture for XO calibration
03/30/11   WB      Add enum for WFW_START_MODE_G2W
03/18/11   WB      Support Low/Med/High clock in SRCH
02/16/11   WX      Add interface support for RX/TX LM
02/02/11   NC      Support high/medium/low offline clock speed
01/07/11   HK      Changed the command ID to WFW_CPC_CFG_CMD_ID from WFW_MOD_DTX_CFG_CMD_ID
12/15/10   WB      Define WFW_IRAT_X2W_MODE_CMD_ID and extend WfwG2WTypeEnum (CR 266395)
10/22/10   EM      CPC DTX: Add DTX command support
09/27/10   WB      W2L Connected Changes, add new TGMP
03/01/10   NC      Add RF Tune Command
09/09/09   FT      Add numSfnCorrectedCFNRollover for MIMO CQI type CFN rollover
08/17/09   EM      Add DC bitfield support for HS Uplink dual carrier
06/11/09   NB      Modify FACH cfg command interface for FACH mode G meas
05/20/09   NC      Merge with Taxis CS2 SWI
05/20/09   NB      Add SS freeze/ unfreeze command
05/20/09   NC      AGC command DC comments
03/27/09   WM      Add QICE command interface
12/11/08   NC      Fix for UL CM - expand slot format to 5 bit
12/09/08   NB      IRAT command to send G2W search type mode
11/14/08   NB      CM command interface
11/14/08   NC      Add enum for RPP mode
10/02/08   FT      Add support for HS MIMO UL
09/06/08   NC      MDSP halt command
09/04/08   WX      Add sleep support
06/09/08   PK      Added support for simultaneous int+msg based intf with SW
03/14/08   NC      Add placeholder for EUL TX cfg command
02/15/08   GS      Add compressed mode cfg command
01/02/08   NC      Add status dump command
11/27/07   GS      Added message Q based support for SW interface
11/08/07   NC      AIs from TX SWI review
10/15/07   NC      Restructure HS SWI
09/21/07   NC      Include TX related configuration commands and updates from
                   interface review
===========================================================================*/

#ifndef WFW_SW_CMD_PROC_INTF_H
#define WFW_SW_CMD_PROC_INTF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "wfw_qice_swi.h"
#include "wfw_rf_intf.h"
#include "fw_rf_common_intf.h"
#include "wfw_rxagc_intf.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/***********************/
/* Various command Ids */
/***********************/

/* HS-FACH support enabled.*/
#define WFW_DEMOD_HS_RACH_ENABLED

typedef enum {
/*! @brief In case SW app is on Q6 or has a OS compatiable with Q6 OS to send and
    receive messages then following command Ids can be used to communicate */
/*!@{*/
  WFW_SW_INTF_BUF_SHARE_MSG_ID      = 1,
  WFW_SW_INTF_BUF_SHARE_DONE_MSG_ID = 2,
  WFW_SW_INTF_BUF_ACT_REQ_MSG_ID    = 3,
  WFW_SW_INTF_BUF_ACT_DONE_MSG_ID   = 4,
  WFW_PROC_SLEEP_DONE_MSG_ID        = 5,
  WFW_PROC_WAKEUP_DONE_MSG_ID       = 6,
/*!@}*/

/*! @brief Set this to first command Id */
#define WFW_CMD_ID_BASE            WFW_DEMOD_CFG_CMD_ID

/*! @name Command Id for DEMOD module */
/*!@{*/
#define WFW_DEMOD_BASE_ID          WFW_CMD_ID_BASE /*!< @brief Demod sync cmd base Id */
  WFW_DEMOD_CFG_CMD_ID =    10, /*!< @brief Demod cfg cmd Id */
#define WFW_DEMOD_LAST_ID          WFW_DEMOD_CFG_CMD_ID /*!< @brief Demod sync cmd last Id */
/*!@}*/

/*! @name Command Id for SRCH module */
/*!@{*/
#define WFW_SRCH_CMD_BASE_ID       WFW_SRCH_STEP1_CMD_ID
  WFW_SRCH_STEP1_CMD_ID =   11,
  WFW_SRCH_STEP2_CMD_ID =   12,
  WFW_SRCH_STEP3_CMD_ID =   13,
  WFW_SRCH_PN_CMD_ID    =   14,
  WFW_SRCH_NASTT_CMD_ID =   15,
  WFW_SRCH_OTDOA_CMD_ID =   16,
  WFW_SRCH_ABORT_CMD_ID =   17,
#define WFW_SRCH_LAST_ID           WFW_SRCH_ABORT_CMD_ID
/*!@}*/

/*! @name Command Id for RxAGC module */
/*!@{*/
#define WFW_RXAGC_CMD_BASE_ID      WFW_RXAGC_CMD_ID
  WFW_RXAGC_CMD_ID =         18,
#define WFW_RXAGC_LAST_ID =        WFW_RXAGC_CMD_ID
/*!@}*/

/*! @name Command Id for Demback module */
/*!@{*/
  WFW_CCTRCH_DIRTY_BIT_UPDATE_ID =    19,
/*!@}*/

/*! @name Command Id for modulator module */
/*!@{*/
#define WFW_MOD_BASE_ID            WFW_MOD_RACH_CFG_CMD_ID /*!< @brief RACH sync cmd base Id*/
  WFW_MOD_RACH_CFG_CMD_ID     = 20, /*!< @brief TX RACH cfg cmd Id */
  WFW_MOD_DPCCH_CFG_CMD_ID    = 21, /*!< @brief TX r99 DPCCH cfg cmd Id */
  WFW_MOD_HS_DPCCH_CFG_CMD_ID = 22, /*!< @brief TX HS DPCCH cfg cmd Id */
  WFW_MOD_EUL_CFG_CMD_ID      = 23, /*!< @brief TX EUL cfg cmd Id */
#define WFW_MOD_LAST_ID            WFW_MOD_EUL_CFG_CMD_ID /*!< @brief RACH sync cmd last Id */
/*!@}*/

/*! @name Command Id for TX Timing Config module */
/*!@{*/
#define WFW_TX_TIMING_BASE_ID      WFW_TX_TIMING_CFG_CMD_ID /*!< @brief TX timing sync cmd base Id*/
  WFW_TX_TIMING_CFG_CMD_ID = 24, /*!< @brief TX timing cfg cmd Id */
#define WFW_TX_TIMING_LAST_ID      WFW_TX_TIMING_CFG_CMD_ID /*!< @brief TX timing sync cmd last Id */
/*!@}*/

/*! @name Command Id for status dump */
  WFW_DEMOD_STATUS_DUMP_CMD_ID =    25, /*!< @brief Demod status dump cmd Id */

/*! @name Command Id for compressed mode cfg */
  WFW_CM_CFG_CMD_ID            =    26,

/*! @name Command Id for MDSP Panic */
  WFW_MDSP_PANIC_ID            =    27,

/*! @name Command Id for sleep command */
  WFW_SLEEP_CMD_ID             =    28,

/*! @name Command Id for CMSRCH module */
/*!@{*/
#define WFW_CMSRCH_CMD_BASE_ID           WFW_CMSRCH_STEP1_SRCH_CMD_ID /* !< @brief CM search commands base Id */
  WFW_CMSRCH_STEP1_SRCH_CMD_ID =    29, /* !< @brief CM step1 srch command Id */
  WFW_CMSRCH_PN_SRCH_CMD_ID    =    30, /* !< @brief CM list srch command Id */
#define WFW_CMSRCH_LAST_CMD_ID           WFW_CMSRCH_PN_SRCH_CMD_ID
  WFW_START_FACH_MEAS_CMD_ID   =    31, /* !< @brief CM FACH measurement command Id */

  WFW_X2W_STOP_RF_CMD_ID       =    32, /* !< @brief G2W stop rf command Id */
#define WFW_G2W_STOP_RF_CMD_ID            WFW_X2W_STOP_RF_CMD_ID

/*! @name Command Id for start_wfw command */
  WFW_START_WFW_CMD_ID =    33,

/*! @brief WCDMA FW stop command Id */
  WFW_STOP_CMD_ID =         34,

/*! @brief Interrat command */
  WFW_IRAT_X2W_MODE_CMD_ID =          35, /* !< @brief G2W mode command Id */
#define WFW_IRAT_G2W_MODE_CMD_ID           WFW_IRAT_X2W_MODE_CMD_ID

/*! @brief Command Id for QICE module */
  WFW_QICE_CFG_CMD_ID =               36, /* !< @brief QICE cfg cmd Id */

/*! @brief Freeze/ Unfreeze sample server command */
  WFW_FREEZE_SAMPLE_SERVER_CFG_CMD_ID =  37, /* !< @brief freeze SS cfg command Id */

/*! @brief Command Id for QICE module */
  WFW_CLK_CFG_CMD_ID =                38, /* !< @brief clock configuration cmd Id */

/*! @brief Command Id for DTX manager */
  WFW_CPC_CFG_CMD_ID =                39,  /* !< @brief DTX configuration cmd Id */
#define WFW_MOD_DTX_CFG_CMD_ID        WFW_CPC_CFG_CMD_ID

/*! @brief RF tuning command */
  WFW_RFTUNE_CMD_ID =                 40, /* !< @brief RF tuning command Id */

/*! @brief TxLM config command */
  WFW_TXLM_CFG_CMD_ID =               41, /* !< @brief TX LM cfg command Id */

/*! @brief TxLM update command */
  WFW_TXLM_UPDATE_CMD_ID =            42, /* !< @brief TX LM update command Id */

/*! @brief W2X Idle mode config command */
  WFW_W2X_IDLE_CFG_CMD_ID =           43, /* !< @brief RF tuning command Id */

/*! @brief W IQ Sample Capture command */
  WFW_IQ_CAPTURE_CMD_ID =             44,  /* !< @brief IQ capture command Id */

/*! @brief X2W IRAT command to confogire resources and tune to W */
  WFW_IRAT_X2W_MODE_CFG_CMD_ID =      45,  /* !< @brief X2W mode and RF tune command Id */

/*! @brief WCDMA DAC calibration command */
  WFW_TX_DAC_CAL_CMD_ID =             46,  /* !< @brief DAC calibration command Id */

/*! @brief Command Id for HS demback reset */
  WFW_HS_DEMBACK_TRESET_CMD_ID =      47,

/*! @brief W S-APT IQ Sample Capture command */
  WFW_SAPT_IQ_CAPTURE_CMD_ID =        48,  /* !< @brief IQ capture command Id */

/*! @brief WCDMA Joint DL UL Config Command */
  WFW_JUDC_CMD_ID =                   49,

/*! @brief W EPT calibration apply DPD command */
  WFW_EPT_CAL_APPLY_DPD_CMD_ID =      50,  /* !< @brief EPT cal DPD apply command Id */

/*! @brief W EPT calibration sample buffer command */
  WFW_EPT_CAL_SAMPLE_BUFFER_CFG_CMD_ID = 51,  /* !< @brief EPT cal sample buffer command Id */
/*! @brief RF command interface */
  WFW_RF_INIT_CMD_ID =                   52,  /* !< @brief RF SW command to init dynamic event */
  WFW_RF_CFG_CMD_ID =                    53,  /* !< @brief RF SW command to send static event */

/*! @brief TxLM config command */
  WFW_TXLM_ET_CFG_CMD_ID =               54, /* !< @brief TX LM ET cfg command Id */

/*! @brief RF dynamic update command */
  WFW_RF_ADC_TQ_CONFIG_CMD_ID =          55,  /* !< @brief RF dynamic update command Id */

/*! @brief RF update command */
  WFW_RF_UPDATE_CMD_ID =                 56,  /* !< @brief RF update command Id */

/*! @brief RxLM dynamic update command */
  WFW_RXLM_DYN_UPDATE_CMD_ID =           57,  /* !< @brief RxLM dynamic update command Id */

  WFW_COEX_CFG_CMD_ID =                  58,  /* !< @brief Coex (DSDA) command Id */

  WFW_QTA_CFG_CMD_ID =                   59,

  WFW_RFTUNE_STEP1_CMD_ID =              60,

/*! @name Command Id for srch_cycle command */
  WFW_SRCH_CYCLE_CMD_ID =                61,

  WFW_MEAS_CLEANUP_CMD_ID =              62,  /* !< @brief signal that L2W multi-gaps processing completed. Time to free L1S MEAS RESERVE */

/*! @brief WCDMA Antenna Switch command */
  WFW_ANTENNA_SWITCH_CMD_ID =            63  /* !< @brief Antenna switch command Id */

#define WFW_CMD_ID_LAST                      WFW_ANTENNA_SWITCH_CMD_ID

} __attribute__ ((packed)) WfwSwCmdEnum;


/**********************/
/* Misc status values */
/**********************/

/*!@name WCDMA FW command proc status */
/*!@{*/
#define WFW_CMD_STATUS_GOOD            0xFF /*!< @brief Sync command status good */
#define WFW_CMD_STATUS_INTF_NOT_READY  1    /*!< @brief Sync interface not ready */
#define WFW_CMD_STATUS_INTF_BUSY       2    /*!< @brief interface busy */
#define WFW_CMD_STATUS_INVALID_CMD     3    /*!< @brief Invalid sync command */
#define WFW_CMD_STATUS_INVALID_PARAMS  4    /*!< @brief Invalid parameters */
#define WFW_CMD_STATUS_INTERNAL_ERR    5    /*!< @brief internal processing error */
/*!@}*/

/*! @brief Define immediate RF tune time */
#define WFW_IRAT_RF_TUNE_TIME_IMM    0x7FFFFFFF
#define WFW_SS_UPDATE_TIME_IMM       0xFFFF

#define WFW_COEX_UNKNOWN_FREQUENCY_ID   (0xFFFFFFF0)
#define WFW_COEX_UNKNOWN_CHANNEL_ID     (0xFF)

/* Structure definitions */
/* --------------------- */

/*! @brief RACH configuration command enum */
typedef enum
{
  /*! RACH is enabled */
  WFW_CMD_RACH_ACTION_ENABLE = 0,
  /*! RACH is aborted */
  WFW_CMD_RACH_ACTION_ABORT,
  /*! HS-RACH is enabled */
  WFW_CMD_HS_RACH_ACTION_ENABLE,
  WFW_CMD_RACH_NUM_ACTION
} __attribute__ ((packed)) WfwCmdRachActionTypeEnum;

/*! @brief CM config command Time Gap measurement purpose (TGMP) enum */
typedef enum
{
  /*! CM No measurement (debug only) */
  WFW_CM_TGMP_NO_MEAS,
  /*! CM interfrequency measurements */
  WFW_CM_TGMP_INTER_FREQ,
  /*! For W2G GSM measurements */
  WFW_CM_TGMP_W2G_GSM_MEAS,
  /*! For W2L LTE measurements */
  WFW_CM_TGMP_W2L_LTE_MEAS
} __attribute__ ((packed)) WfwCmTgmpEnum;

/*! @brief enum for QTA gap start/stop command*/
typedef enum
{
  /*! start gap */
  WFW_QTA_CMD_START_TYPE = 0,
  /*! stop gap*/
  WFW_QTA_CMD_STOP_TYPE
} __attribute__ ((packed)) WfwQtaCmdTypeEnum;

/* @brief Start WFW command enum */
typedef enum
{
  /*! Start WCDMA FW in X2W mode */
  WFW_START_MODE_X2W,
  /*! Start WCDMA FW in G2W mode (To be removed later) */
  WFW_START_MODE_G2W = WFW_START_MODE_X2W,
  /*! Start complete WCDMA FW */
  WFW_START_MODE_WCDMA,
  /*! Number of Start Mode */
  WFW_NUM_START_MODE
} __attribute__ ((packed)) WfwCmdStartWfwCfgEnum;

/*! @brief Start WFW Command: UE Type enum */
typedef enum
{
  /*! Start WFW in Single Sim */
  WFW_START_UE_TYPE_SINGLE_SIM,
  /*! Start WFW in DSDS */
  WFW_START_UE_TYPE_DUAL_SIM_DUAL_STANDBY,
  /*! Start WFW in DSDA */
  WFW_START_UE_TYPE_DUAL_SIM_DUAL_ACTIVE,
} __attribute__ ((packed)) WfwStartCmdUeTypeEnum;

/* @brief IRAT X2W mode command enum */
typedef enum
{
  /*! IRAT INIT mode */
  WFW_IRAT_INIT_MODE = 0,
  /*! IRAT G2W Idle Mode */
  WFW_IRAT_G2W_IDLE_MODE = 1<<0,
  /*! IRAT L2W Idle Mode */
  WFW_IRAT_L2W_IDLE_MODE = 1<<1,
  /*! IRAT L2W Idle Reconf Mode */
  WFW_IRAT_L2W_IDLE_RECONF_MODE = 1<<2,
  /*! IRAT G2W Dedicated Mode */
  WFW_IRAT_G2W_DEDICATED_MODE = 1<<3,
  /*! IRAT L2W Dedicated Mode */
  WFW_IRAT_L2W_DEDICATED_MODE = 1<<4,
  /*! IRAT L2W Dedicated Mode for Cell ID (step1//NASTT) */
  WFW_IRAT_L2W_DEDICATED_CELL_ID_MODE = 1<<5,
  /*! WFW waiting for X2W command from SW driver */
  WFW_IRAT_WAITING_FOR_X2W_CMD = 1<<6
} __attribute__ ((packed)) WfwX2WTypeEnum;

/*! TO BE REMOVED WHEN SW CHANGES ARE DONE */
typedef WfwX2WTypeEnum WfwG2WTypeEnum;


/*! @brief TG dl frame structure type A/B enum */
typedef enum
{
  /*! CM ITP first TPC command in the gap */
  WFW_TG_DL_FRAME_STRUCTURE_TYPE_A = 0,
  /*! CM ITP most recent computed delta i */
  WFW_TG_DL_FRAME_STRUCTURE_TYPE_B
} __attribute__ ((packed)) WfwTgDlFrameStructureTypeEnum;


/*! @brief TG itp tpc cmd gap/delta last enum */
typedef enum
{
  /*! CM ITP first TPC command in the gap */
  WFW_TG_ITP_TPC_CMD_GAP = 0,
  /*! CM ITP most recent computed delta i */
  WFW_TG_ITP_DELTA_LAST
} __attribute__ ((packed)) WfwTgItpTypeEnum;


/*! @brief TG rpp normal/fast enum */
typedef enum
{
  /*! CM RPP normal mode */
  WFW_TG_RPP_NORMAL = 0,
  /*! CM RPP fast recovery mode */
  WFW_TG_RPP_FAST
} __attribute__ ((packed)) WfwTgRppTypeEnum;

/*! @brief W2X idle cfg mode for other RAT */
typedef enum
{
  /*! Indicates we will do W2G idle */
  WFW_W2X_IDLE_GSM_MODE,
  /*! Indicates we will do W2L idle */
  WFW_W2X_IDLE_LTE_MODE
} __attribute__ ((packed)) WfwW2xIdleCfgModeEnum;

/*! @brief Txlm Cfg state */
typedef enum
{
  /*! TxLM config cmd for disabling */
  WFW_TX_TXLM_CFG_DISABLED_STATE = 0,
  /*! TxLM config cmd for enabling SC */
  WFW_TX_TXLM_CFG_ENABLED_STATE,
  /*! TxLM config cmd for enabling DC */
  WFW_TX_TXLM_CFG_ENABLED_DCUPA_STATE
} __attribute__ ((packed)) WfwTxlmCfgSTateEnum;

/*! @brief Step1 cmd type used for L2W connected step1/NASTT */
typedef enum
{
  /*! Default step1 in 1 cmd */
  WFW_SRCH_STEP1_CMD_TYPE_DEFAULT,
  /*! First subtask for multiple commands step1 */
  WFW_SRCH_STEP1_CMD_TYPE_FIRST_SUBTASK,
  /*! Middle subtask for multiple commands step1 */
  WFW_SRCH_STEP1_CMD_TYPE_MIDDLE_SUBTASK,
  /*! Last subtask for multiple commands step1 */
  WFW_SRCH_STEP1_CMD_TYPE_LAST_SUBTASK
} __attribute__ ((packed)) WfwSrchStep1CmdTypeEnum;

/*! @brief DC-HSUPA waitOnScchOrderSec types */
typedef enum
{
  /*! SC case - ignore waitOnScchOrderSec variable */
  WFW_TX_DCHSUPA_WAIT_SCCH_ORDER_NOOP = 0,
  /*! Wait on SCCH order for DC-HSUPA activation */
  WFW_TX_DCHSUPA_WAIT_SCCH_ORDER_TRUE,
  /*! Enable and activate DC-HSUPA (HHO recovery) */
  WFW_TX_DCHSUPA_WAIT_SCCH_ORDER_FALSE
} __attribute__ ((packed)) WfwTxDcHsupaWaitScchOrderTypeEnum;
typedef enum
{
  /*! Issued by WL1 - used with Tab */
  WFW_COEX_CMD_UPDATE_PRIORITY_TABLE = 0,
  /*! Issued by WL1 - used for freqTab*/
  WFW_COEX_CMD_UPDATE_FREQ_ID_TABLE,
  /* Issued by WL1 indicating that de-sense scenario is a possibility */
  /* Used for desenseScenarioActive*/
  WFW_COEX_CMD_UPDATE_DESENSE,
  /*! Used to update channel Id for DR-DSDS */ 
  WFW_COEX_CMD_UPDATE_CHANNEL_ID,
  /*! NOT Issued by WL1, used for VP testing */
  WFW_COEX_CMD_RX_BLANK
} __attribute__ ((packed)) WfwCoexCmdTypeEnum;

typedef enum
{
  /* Antenna Switching, should be highest priority activity */
  WFW_COEX_ACTIVITY_ANTENNA_SWITCH = 0,
  /*! PLL tune + CGAGC converging - Should protect 1 slot starting from RxAGC start cmd */
  WFW_COEX_ACTIVITY_AGC_CONVERGENCE,
  /*! Reading Page from SCCPCH (right after PICH) - MT call*/
  WFW_COEX_ACTIVITY_R99_SCCPCH_DECODE_PCH,
  /*! Decoding PICH - should protect PI bits at the least*/
  WFW_COEX_ACTIVITY_PICH_DECODE,
  /*! Should protect 1 slot interval beginning from gap start (CGAGC protection) */
  WFW_COEX_ACTIVITY_W2W_CM_GAP_ENTRY,
  /*! Should protect 1 slot interval beginning from gap end (CGAGC protection) */
  WFW_COEX_ACTIVITY_W2W_CM_GAP_EXIT,
   /*! Should protect 1 slot interval beginning from gap end (CGAGC protection) */
  WFW_COEX_ACTIVITY_W2G_CM_GAP_EXIT,
  /*! Should protect 1 slot interval beginning from gap start (CGAGC protection) */
  WFW_COEX_ACTIVITY_FMO_GAP_ENTRY,
  /*! Should protect 1 slot interval beginning from gap end (CGAGC protection) */
  WFW_COEX_ACTIVITY_FMO_GAP_EXIT,
  /*! TIER 1 Tx activity*/
  WFW_COEX_ACTIVITY_TIER_1_TX,
  /*! RACH Message Tx */ /*snaravar --->  giving more priority to RACH related activities */
  WFW_COEX_ACTIVITY_RACH_MSG_TX,
  /*! RACH preamble Tx */
  WFW_COEX_ACTIVITY_RACH_PREAMBLE_TX,
  /*! AICH decode during RACH procedure */
  WFW_COEX_ACTIVITY_AICH_DECODE,
  /*! Searcher Background Rx Activity */
  WFW_COEX_ACTIVITY_SEARCHER,
  /*! DL DPDCH being received*/
  WFW_COEX_ACTIVITY_R99_DPDCH_DECODE,
  /*! PCCPCH being received with RSC ID 0*/
  WFW_COEX_ACTIVITY_R99_PCCPCH_0_DECODE,
  /*! PCCH being received with RSC ID 1*/
  WFW_COEX_ACTIVITY_R99_PCCPCH_1_DECODE,
  /*! Reading RRC conn. setup from SCCPCH (right after RACH) - MO call*/
  /* First SCCPCH Channel priority */
  WFW_COEX_ACTIVITY_R99_SCCPCH_DECODE_FACH0,
  /* Second SCCPCH Channel Priority */
  WFW_COEX_ACTIVITY_R99_SCCPCH_DECODE_FACH1,
  /*! HS decode ongoing in RAKE*/
  WFW_COEX_ACTIVITY_HS_DECODE_RAKE,
  /*! HS decode ongoing in QICE*/
  WFW_COEX_ACTIVITY_HS_DECODE_QICE,
  /*! R99 UL DPCCH active*/
  WFW_COEX_ACTIVITY_R99_DPCCH_TX,
  /*! This should cover the CM gap from start to end */
  /*! Does not protect anything, but needed for inferring freqID*/
  WFW_COEX_ACTIVITY_W2W_CM_GAP_DURATION,
  /*! Cannot think of an use case for this..nothing to protect from W side */
  WFW_COEX_ACTIVITY_W2G_CM_GAP_ENTRY,
  /*! This should cover the FMO gap from start to end */
  /*! Does not protect anything, but needed for inferring freqID*/
  WFW_COEX_ACTIVITY_FMO_GAP_DURATION,
  /*! Max*/
  WFW_COEX_ACTIVITY_CODES_MAX
} __attribute__ ((packed)) WfwCoexActivityCodesEnum;

typedef enum
{
  /*! Index for home frequency for RX */
  WFW_COEX_RX_HOME_FREQ_ID = 0,
  /*! Index for interfreq CM (W2W) for RX */
  WFW_COEX_RX_INTERFREQ_CM_FREQ_ID,
  /*! Index for interfreq  FMO gap for RX */
  WFW_COEX_RX_INTERFREQ_FMO_FREQ_ID,
  WFW_COEX_RX_FREQ_ID_MAX
} __attribute__ ((packed)) WfwCoexRxFreqIdEnum;

typedef enum
{
  /*! Index for home frequency for TX */
  WFW_COEX_TX_HOME_FREQ_ID = 0,
  /*! Index for interfreq CM (W2W) for RX */
  WFW_COEX_TX_INTERFREQ_CM_FREQ_ID,
  /*! Index for interfreq  FMO gap for RX */
  WFW_COEX_TX_INTERFREQ_FMO_FREQ_ID,
  WFW_COEX_TX_FREQ_ID_MAX
} __attribute__ ((packed)) WfwCoexTxFreqIdEnum;

/**********************************************/
/* Various command info definition structures */
/**********************************************/

/* Searcher sync command */
/* --------------------- */

/* @brief PN command sync interface */
typedef struct
{
  /*! List search command on Low priority queue */
 UWord16 srchLQen:1;
  /*! List search command on High priority queue */
 UWord16 srchHQen:1;
 /*! whether the PN search needs to be treated as overlapped search or not. */
 UWord16 overLappedSearch:1;
  /*! Queue in which the current PN search came on, either 0 or 1 */
 UWord16 pnQ:1;
 UWord16 :9;
 /* The bits below are valid only in G2W cell reconfirmation search, at
 all other times it must be set to 0 */
 /*! Stage 1 search in G2W */
 UWord16 stage1Srch:1;
 /*! Stage 2 search in G2W */
 UWord16 stage2Srch:1;
 /*! Stage 2 Direct search in X2W */
 UWord16 stage2DirectSrch:1;
} WfwSrchListCmdRspInterfaceStruct;


typedef struct
{
  /* Word 0 */
 UWord16 srchLQen:1;
 /* Bit 1 */
 UWord16 srchHQen:1;
 /* Bit 13:2 reserved */
 UWord16 :12;
 /* Bit 14 */
 UWord16 srchLastSubTask:1;
 /* Bit 15 */
 UWord16 srchFirstSubTask:1;

} WfwSrchStep123CmdRspInterfaceStruct;

typedef struct
{
 /*! Type of step1 cmd */
 WfwSrchStep1CmdTypeEnum cmdType;
} WfwSrchStep1CmdRspInterfaceStruct;

typedef union
{
 WfwSrchStep1CmdRspInterfaceStruct   srchStep1CmdRsp;
 WfwSrchStep123CmdRspInterfaceStruct srch123CmdRsp;
 WfwSrchListCmdRspInterfaceStruct    srchListCmdRsp;

} WfwSrchCmdCfgStruct;

/* CM Searcher sync command */
/* ------------------------- */

/*! @brief CM PN command */
typedef struct
{
 /* Word 0 */
 /*! CM Queue select bit (0 or 1) */
 UWord16 qsel:1;
 /* Bit 15:1 reserved */
 UWord16 :15;
} WfwCmSrchListCmdRspInterfaceStruct;

/*! @brief CM Step1 command */
typedef struct
{
  /* Word 0 */
  /*! CM Queue select bit (0 or 1) */
  UWord16 qsel:1;
  /* Bit 13:1 reserved */
  UWord16 :13;
  /*! Last task of CM step1 non-coherent accumulation */
  UWord16 srchLastSubTask:1;
  /*! First task of CM step1 non-coherent accumulation */
  UWord16 srchFirstSubTask:1;
} WfwCmSrchStep1CmdRspInterfaceStruct;

/*! @brief Union of CM Step1/ PN command structures */
typedef union
{
  /*! Step1 command struct */
  WfwCmSrchStep1CmdRspInterfaceStruct cmSrchStep1CmdRsp;
  /*! PN Command struct */
  WfwCmSrchListCmdRspInterfaceStruct  cmSrchListCmdRsp;

} WfwCmSrchCmdCfgUnion;


/* RxAGC sync command */
/* ------------------------- */

/* RxAGC command word constant definitions */
#define WFW_RXAGC_CMD_START     2
#define WFW_RXAGC_CMD_STOP      1
#define WFW_RXAGC_CMD_NOACTION  0

/*! @brief RxAGC Command Word Bitfield */
typedef struct
{
  /*! @brief AGC start-stop control for Antenna 0 Carrier 0 */
  UWord32 a0c0 : 2;  /* Bits 1:0 */
  /*! @brief AGC start-stop control for Antenna 1 Carrier 0 */
  UWord32 a1c0 : 2;  /* Bits 3:2 */
  /*! @brief AGC start-stop control for Antenna 0 Carrier 1 */
  UWord32 a0c1 : 2;  /* Bits 5:4 */
  /*! @brief AGC start-stop control for Antenna 1 Carrier 1 */
  UWord32 a1c1 : 2;  /* Bits 7:6 */
  /*! @brief AGC start-stop control for Antenna 0 Carrier 2 */
  UWord32 a0c2 : 2;  /* Bits 9:8 */
  /*! @brief AGC start-stop control for Antenna 1 Carrier 2 */
  UWord32 a1c2 : 2;  /* Bits 11:10 */
  /*! @brief AGC start-stop control for Antenna 0 Carrier 3 */
  UWord32 a0c3 : 2;  /* Bits 13:12 */
  /*! @brief AGC start-stop control for Antenna 1 Carrier 3 */
  UWord32 a1c3 : 2;  /* Bits 15:14 */
  /*! @brief reserved */
  UWord32      : 16; /* Bits 31:16 */
} WfwRxAgcCmdWordStruct;

/*! @brief RxAGC Command Word Union */
typedef union
{
  /*! @brief RxAGC Command Word */
  UWord32 val;
  /*! @brief RxAGC Command Word Bitfield */
  WfwRxAgcCmdWordStruct f;
} WfwRxAgcCmdWordUnion;

/*! @brief RxAGC Command */
typedef struct
{
  /*! @brief RxAGC command word */
  WfwRxAgcCmdWordUnion cmdWord;

  /*! @brief RxLM buffer indices per AGC */
  UWord8 rxlmBufIndex[WFW_RXAGC_MAX_ANTENNAS][WFW_RXAGC_MAX_CARRIERS];

  /*! @brief start/stop mode indicator for DSR update
       0: no action
       1: stop AGC first and then start AGC immediately */
  UWord8 stopStartFlag;

  /*! @brief Total Time needed for the completion of SSBI execution + PLL tune time
     SSBI includes two parts
     1 - AutoB group of SSBIs (happen before PLL settling)
     2 - AutoA group of SSBIs (happen in parallel with PLL settling)
     This time is execTime(AutoB) + max(execTime(AutoA), PLL_settling_time)
     This time is ONLY used if rfWarmupFlag is set */
  UWord16 rfWarmupTime;
  /*! @brief Flag to indicate the AGC start during warmup */
  UWord8 rfWarmupFlag;

  /*! @ brief Rx calibration flag
       0: Normal mode
       1: Rx Cal, do not start AGC power accumulations */
  UWord8 rxCalFlag;

  /*! @ brief NBR feature enabling
       0: disabled
       1: enabled - nbr should be running as part of rxagc start cmd processing */
  UWord8 nbrEnFlag;                 

  /*! @ brief rxlmBufIndex for modified G chain */
  UWord8 nbrRxlmBufIndex;

} WfwRxAgcCmdCfgStruct;

/*! @brief RxLM Dynamic Update Command */
typedef struct
{
  /*! @brief RxLM buffer index per AGC. */
  UWord8 rxlmBufIndex[WFW_RXAGC_MAX_ANTENNAS][WFW_RXAGC_MAX_CARRIERS];
  /*! @brief RxLM dynamic group bitmask. */
  UWord32 rxlmDynGroupMask;
  /*! @brief Antenna bitmask, 1 = apply dynamic RxLM updates to Rx chain.
    Bit 1  Rx1
    Bit 0  Rx0 */
  UWord32 rxAntennaMask;
} WfwRxlmDynUpdateCmdStruct;

/* startIQ capture command */
/* --------------------- */

typedef struct
{
  UWord32 rxlmBufferId:8;
  UWord32 chain:2;
  UWord32 carrier:2;
  UWord32 :4;
  UWord32 numSamples:16;
  UWord32 startTime;
} WfwStartIqCaptureCmdStruct;

/* start S-APT IQ capture command */
/* ---------------------------- */

/* We use this for ET, EPT, and Delay */
/* Rx and Tx */

typedef struct
{
  /*! @ brief Specify sample capture parameters, including buffer ID's,
   *  response sequence ID */
  fw_rf_capture_param_t captureParam;
} WfwStartSaptIqCaptureCmdStruct;

/* Apply DPD for EPT calibration */
/* ---------------------------- */
typedef struct
{
  /*! @ brief DPD index -> range: 0 to (WFW_TX_NUM_MAX_EPT_CAL_CAPTURES_SUPPORTED-1) */
  UWord8 dpdIdx;
} WfwEptCalApplyDpdCmdStruct;

/* EPT Sample Buffer cfg command */
/* ---------------------------- */
typedef struct
{
  /*! @ brief Memory size that needs to be allocated in bytes */
  UWord32 memSize;
  /*! @ brief Address where the samples are stored. */
  UWord32 memAddress;
} WfwEptSampleBufferCfgCmdStruct;

/* Start WCDMA FW */
/* --------------------- */
/*! @brief Start WFW command strucuture */
typedef struct
{
  /*! Start either G2W threads or start all threads */
  WfwCmdStartWfwCfgEnum startMode;
  /*! @brief WFW Start Cmd UE Type */
  WfwStartCmdUeTypeEnum ueType;
} WfwStartWfwCmdStruct;

typedef struct
{
  /*! X2W is either idle mode or dedicated mode, G2W or L2W */
  WfwX2WTypeEnum x2wMode;
} WfwX2wModeCmdStruct;

typedef struct
{
  /* Word 0 */
  /*! RF action time (cx8) - Timed Range 0:307199, for immediate time use   0x7FFF_FFFF*/
  UWord32 rfActionTime;
  /* Word 1 */
  /*! X2W is either idle mode or dedicated mode, G2W or L2W (from WfwX2WTypeEnum) */
  UWord16 x2wMode : 7;              /* Bits 6:0 */
  /*! @brief RxLM buffer index */
  UWord16 rxlmBufIndex : 5;         /* Bits 11:7 */
  /*! @brief FTM mode ON bit to make sure we tune independently of TQ release by other RAT */
  UWord16 ftmMode : 1;              /* Bit 12 */
  /*! Reserved */
  UWord16 : 3;                      /* Bits 15-13 */

} WfwX2wModeCfgCmdStruct;

/*! UNUSED, TO BE REMOVED WHEN SW CHANGES ARE DONE */
typedef struct
{
  /*! G2W is either idle mode or dedicated mode */
  WfwG2WTypeEnum g2wMode;
} WfwG2wModeCmdStruct;

/*! @brief SS Freeze config command structure */
typedef struct
{
  /* Bit to indicate to freeze SS */
  UWord16 ssFreeze:1;
  /* Bit to indicate to UNfreeze SS */
  UWord16 ssUnfreeze:1;
  /* chain0 = 1, means apply freeze/unfreeze action to chain 0 */
  UWord16 chain0:1;
  /* chain1 = 1, means apply freeze/unfreeze action to chain 1 */
  UWord16 chain1:1;
  /* 0: No action; 1: send SSBI to power down the RTR */
  UWord16 rfPwrUpdateFlag:1;
  UWord16 unused:11;
  /* action Time for freeze/unfreeze (within one frame in cx1 unit, 0-38399) */
  UWord16 actionTime;
} WfwSsFreezeCfgCmdStruct;

/*! @brief RF tuning command structure */
typedef struct
{
  /*! gapStartTime and gapEndTime (WSTMR) – for IDLE, gapEndTime should be set to 0xFFFF_FFFF; and WFW will ignore it. */
  UWord32 gapStartTime;
  UWord32 gapEndTime;
  /*! RF action time (cx8) - Timed Range 0:307199, for immediate time use
    0x7FFF_FFFF*/
  UWord32 rfActionTime;
  /*! @brief RxLM buffer index */
  UWord16 rxlmBufIndex:8;
  /*! @brief FTM mode ON bit */
  UWord16 ftmMode:1;               /* Bits 8 */
  /*! @brief RF TUNE ON bit */
  UWord16 rfTuneOn:1;              /* Bits 9 - TRUE: execute RF script; FALSE: do NOT execute RF script */
  /*! Reserved */
  UWord16 reserved:6;              /* Bits 15:10 */

} WfwRftuneCmdStruct;

/*! @brief RF tune plus Step1 command structure */
typedef struct
{
  /*! gapStartTime and gapEndTime (WSTMR) – for IDLE, gapEndTime should be set to 0xFFFF_FFFF; and WFW will ignore it. */
  UWord32 gapStartTime;
  UWord32 gapEndTime;
  /*! RF action time (cx8) - Timed Range 0:307199, for immediate time use
    0x7FFF_FFFF*/
  UWord32 rfActionTime;
  /*! @brief RxLM buffer index */
  UWord16 rxlmBufIndex:8;          /* Bits 7:0 */
  /*! @brief FTM mode ON bit */
  UWord16 ftmMode:1;               /* Bits 8 */
  /*! @brief RF TUNE ON bit */
  UWord16 rfTuneOn:1;              /* Bits 9 - TRUE: execute RF script; FALSE: do NOT execute RF script */
  /*! Reserved */
  UWord16 reserved:6;              /* Bits 15:10 */
  WfwSrchStep1CmdTypeEnum cmdType;
} WfwRftuneStep1CmdStruct;

/* Demod sync command */
/* ------------------ */

/*! @brief Demod resource cfg update command information structure */
typedef struct
{

  /*! @brief Update bit for demod resource to be configured. If 1 then
   cfg update is requested. LSB is resource 0 update bit */
  UWord32 cfgUpdateBmsk;
} WfwSWCmdDemodRscCfgStruct;

/*! @brief Demod configuration update information for all its resources */
typedef struct
{
  WfwSWCmdDemodRscCfgStruct cellCfg;     /*!< @brief cell configuration update info */
  WfwSWCmdDemodRscCfgStruct fingerCfg;   /*!< @brief finger configuration update info */
  WfwSWCmdDemodRscCfgStruct channelCfg;  /*!< @brief channel configuration update info */
  WfwSWCmdDemodRscCfgStruct combinerCfg; /*!< @brief combiner configuration update info */
  WfwSWCmdDemodRscCfgStruct eqCfg;       /*!< @brief Equalizer configuration update info */
  WfwSWCmdDemodRscCfgStruct commonCfg;   /*!< @brief Common configuration update info */
  WfwSWCmdDemodRscCfgStruct ardCfg;      /*!< @brief Equalizer configuration update info */
} WfwSWCmdDemodCfgStruct;

/* TX Timing command */
/* ----------------- */

/*! @brief Modulator timing slam command information structure */
typedef struct
{
  /* Word 1 */

  /*! @brief DL Reference channel offset (Range 4, -30, -50) */
  Word32 dlRefOffset;

  /* Word 2 */

  /*! @brief TX PN Position to slam to in cx8 (Range 0-307199) */
  UWord32 txPnPositionCx8:19;
  /*! @brief TX Frame Count (Range 0-4095) */
  UWord32 txTti : 13;

} WfwSWCmdTxTimingCfgStruct;

/* Mod Config command */
/* ------------------ */

/*! @brief Uplink r99 DPCCH config command information structure */
typedef struct
{
  /*! @brief Enable bit for DPCCH, range 0,1 */
  UWord32 en : 1;
  /*! Dummy Dpcch command to trigger DL in-sync Detection */
  UWord32 inSyncDetect: 1;
  UWord32 reserved1 : 5;
  /*! @brief Specifies FPC algorithm 0->DPC Mode 0, 1->DPC Mode 1 */
  UWord32 dpcMode : 1;
  /*! @brief Slot format, range 0-17 */
  UWord32 slotFormat : 5;
  /*! @brief Wait for SCCH orders to enable secondary UL carrier
      WfwTxDcHsupaWaitScchOrderTypeEnum - 0:noop, 1: true, 2:false */
  UWord32 waitOnScchOrderSec : 2;
  /*! @brief For DC->SC reconfig, disable only secondary carrier
      this bit is relevant only if en = 1. en = 0 disables both. */
  UWord32 disableSecCarrier : 1;
  /*! @brief Uncoded TFCI bits, range 10 bits */
  UWord32 uncodedTfciBits : 16;
} WfwSWCmdTxDpcchCfgStruct;

/*! @brief Uplink RACH config command information structure */
typedef struct
{
  /* Word 1 */

  /*! @brief Action, 0->Enable RACH procedure, 1->Abort RACH procedure, 2->Enable HS-RACH procedure  */
  UWord32 action : 2;
  /*! @brief hsRachEaiCfged, 0->EAI for HS RACH proc is NOT cfg'ed, 1->EAI for HS RACH proc is cfg'ed*/
  UWord32 hsRachEaiCfged : 1;
  /*! @ (hsRachY +1) denotes the number of EDCH resources allocated for this cell, 0-31 */
  UWord32 hsRachY : 5;

  /*! @brief Initial sub-channel number (Range 0-11) */
  UWord32 initSubChannel : 8;
  /*! @brief Bitmask of available sub-channels (12-bit) */
  UWord32 availSubChannelMask : 16;

  /* Word 2 */

  /*! @brief Uncoded TFCI bits (10 bits) */
  UWord32 uncodedTfciBits : 16;
  /*! @brief Bitmask of available signatures (16-bit) */
  UWord32 availSignatureMask : 16;

  /* Word 3 */

  /*! @brief TX power at start of first preamble (Range -512->512) */
  Word32 initTxPower : 16;
  /*! @brief Preamble power step size in db10 (Range 12->96) */
  Word32  pmblPowerStepSize : 8;
  /*! @brief Max number of preamble retransmissions before returning error (Range 1->64) */
  Word32  pmblRetransMax : 8;

  /* Word 4 */

  /*! @brief Step size between last preamble and message (Range -120->120) */
  Word32 msgPowerStepSize : 16;
  /*! @brief TTI of RACH message, 0->10ms TTI, 1->20ms TTI */
  Word32  msgTti : 8;
  /*! @brief AICH Tx Timing Indicator, received from RRC (Range 0,1),
      0->3 SC offset, 1->4 SC offset */
  Word32  aiTxTimingInd : 8;

  /* Word 5 */

  /*! @brief Bitmask of all available signatures in cell (16-bit)
             Used for computing SigIdx in HS-RACH  */
  UWord32 availCellSignatureMask : 16;
  UWord32 : 16;
} WfwSWCmdRachCfgStruct;

/*! @brief TX HS config command information structure */
typedef struct
{
  /* Word 0 */

  /*! @brief HS TX Action - 0->Disable, 1->Enable,
    2->Reconfig, 3->Seamless Reconfig - no gaps between finalSubframe and startSubframe */
  UWord32 action : 2;
  /*! @brief HS N/ACK Preamble Mode Enable (Range 0,1) */
  UWord32 ackPmblEn : 1;
  /*! @brief HS CQI Feedback Factor (Range 1,2,4,5,10,20,40,80) */
  UWord32 cqiFbackFactor : 8;
  /*! @brief HS CQI repetition factor (Range 1-4) */
  UWord32 cqiRep : 3;
  /*! @brief HS N/ACK repetition factor (Range 1-4) */
  UWord32 ackRep : 3;
  /*! @brief Offset in BPG between DPCCH frame boundary and first HS-DPCCH
      subframe contained in that frame (Range 0-29) */
  UWord32 dpcchOffset : 8;
  /*! @brief HS MIMO enum representation the ratio M_cqi_type_A/M_cqi */
  UWord32 hsMimoNcqiTypeAMcqiRatio : 4;
  /*! @brief HS MIMO is enabled flag */
  UWord32 hsMimoIsEnabled : 1;
  /*! @brief UNUSED */
  UWord32 reserved_0 : 2;

  /* Word 1 */

  /*! @brief HS DB Idx for control table and log packet. This is same as index
      passed in UL HS-DPCCH cfg: 0-1 = primary serving cell, 2-3=secondary
      serving cell, etc. */
  UWord32 dbIdx : 3;
  /*! @brief Initial HS DPCCH global subframe number, valid if enable or reslam
      (Range 0-1279) */
  UWord32 initGsfn : 11;
  /*! @brief Final HS DPCCH global subframe number, valid if reslam or disable
      (Range 0-1279) */
  UWord32 finalGsfn : 11;
  /*! @brief Number of subframes to be corrected at CFN rollover to compute MIMO CQI type */
  UWord32 numSfnCorrectedCFNRollover : 3;
  /*! @brief SECONDARY_CELL_ENABLED.  0->SC, 1->DC, 2->3C */
  UWord32 hsSecondaryCellEnabled : 2;
  /*! @brief UNUSED */
  UWord32 reserved_1 : 2;

} WfwSWCmdTxHsDpcchCfgStruct;



/*! @brief TX EUL config command information structure */
typedef struct
{
  /* Word 0 */

  /*! @brief EUL TX action 0 -> Disable , 1 -> Enable
      2 -> Reconfiguration (reconfig involves TTI change/IR change ONLY)
  */
  UWord32 action : 2;
  /*! @brief This field indicates the retransmission scheme employed:
     IR combining or chase combining.
     0 -> Chase Combining. The UE uses only E- DCH RV index 0,
     regardless of RSN value.
     1 -> IR combining. The UE uses an RV index indicated in
     Table 16 of 25.212, based on the RSN value
  */
  UWord32 ir : 1;
  /*! @brief  tti = 0-> 10ms, 1 -> 2ms */
  UWord32 tti : 1;
  /* Starting subframe #, when the data will first be sent on the EUL
     channels with the new configuration.
     Note: for 10ms TTI, the starting subframe number is CFN*5
  */
  UWord32 startingSubframe : 11;
  UWord32 finalSubframe : 11;

  /*! @brief This field indicates whether the carrier is configured or not.
  */
  UWord32 dirtyBitPri : 1;

  /*! @brief UNUSED */
  UWord32 reserved1 : 5;

  /* Word 1 */

  /*! @brief EUL TX action 0 -> Disable , 1 -> Enable */
  UWord32 actionSec : 1;

  /* Starting subframe #, when the data will first be sent on the EUL
     channels with the new configuration.
     Note: for 10ms TTI, the starting subframe number is CFN*5
  */
  UWord32 startingSubframeSec : 11;
  UWord32 finalSubframeSec : 11;

  /*! @brief This field indicates whether the carrier is configured or not.
  */
  UWord32 dirtyBitSec : 1;

  /*! @brief UNUSED */
  UWord32 reserved2 : 8;

} WfwSWCmdTxEulCfgStruct;

#define WfwSWCmdCpcCfgStruct WfwSWCmdTxDtxCfgStruct
#define cpcCfg  dtxCfg
/* @brief CPC DTX/DRX config command information structure */
typedef struct
{
  /* Word 0 */
  /*! @brief CPC DTX type 0 -> DTX, 1 -> DRX */
  UWord32 cpcType : 1;
  /*! @brief CPC action type 0 -> disable, 1 -> enable, 2 -> reconfig */
  UWord32 action  : 2;
  /*! @brief CPC actionCfn */
  UWord32 actionCfn : 8;
  /*! @brief Wait for SCCH orders */
  UWord32 waitOnScchOrder : 1;
  /*! @brief UNUSED */
  UWord32 reserved : 20;
} WfwSWCmdCpcCfgStruct;

/* Demback sync command */
/* -------------------- */

/*! @brief demback related configuration update information */
typedef struct
{
  UWord16 cctrchIdBitMask; /*!< @brief CCTrCh Id bit mask that need update */
  UWord16 interruptReqd : 1;       /*!< @brief Bit 0 */
  UWord16 : 15;                    /*!< @brief Bits 1:15 */
} WfwDembackDirtyBitStruct;

/* Compressed mode cfg sync command */
/* -------------------------------- */


/*! @brief CM cfg command structure */
typedef struct
{
  /*! @brief reference channel number */
  UWord8  chanNum;
  /*! @brief start cfn in PCCPCH/SCCPCH/DPCH channel */
  UWord8  startCfn;
  /*! @brief Start slot of gap in the next frame */
  Word8  firstSlot;
  /*! @brief Duration of gap in term of #slot */
  UWord8  gapLenSlots;
  /*! @brief gap measurement purpose */
  WfwCmTgmpEnum tgmp;
  /*! @brief RxLM buf index, away freq */
  UWord8  rxlmSetupBufIndex;
  /*! @brief RxLM buf index, home frequency */
  UWord8  rxlmHomeBufIndex;
  /*! @brief RxLM buf index, home frequency, diversity chain */
  UWord8  rxlmHomeBufIndexDivChain;
  /*! @brief WTR index, away frequency */
  UWord8  wtrIndex;
  /*! @brief DL frame structure type A or B */
  WfwTgDlFrameStructureTypeEnum dlFrType;
  /*! @brief Initial Transmit Power mode */
  WfwTgItpTypeEnum  itp;
  /*! @brief Recovery Period Power control mode */
  WfwTgRppTypeEnum  rpp;
  /*! @brief UL CM method, SF2 or HLS */
  WfwCmCfgCmMethodEnum ulMethod;
  /*! @brief DL CM method, SF2 or HLS */
  WfwCmCfgCmMethodEnum dlMethod;

  /*! Bit 19:0 Action Time in W STMR counter, bit19:8 12 bit frame num (0~4095), bit7:0, bpg num (0~149) */
  UWord32 stmrActionTime       : 20;
  /* Bit 20-31, Reserved */
  UWord32                      : 12;   

} WfwCmCfgCmdStruct;


/*! @brief FACH command structure */
typedef WfwCmCfgCmdStruct WfwFachMeasCmdCfgStruct;


#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
/*! @brief Ant switching command structure */
typedef struct
{
  /*! Demod config update command info */
  WfwSWCmdDemodCfgStruct      demodCfg;

} WfwAntSwitchCfgCmdStruct;
#endif


/* clk cfg sync command */
/* -------------------------------- */
/*! @brief MSS clock state */

#define WFW_SRCH_CLK_STATE_LEVEL_5 384

#define WFW_SRCH_CLK_STATE_LEVEL_DEFAULT  WFW_SRCH_CLK_STATE_LEVEL_5 
/*! @brief clk cfg command structure */
typedef struct
{
  /*! @Brief MSS/SRCH clock mode 
    Level       MHz
    ----------------------
      0          19
      1         115
      2         144
      3         230
      4         288
      5         384
      6         576
      7         691
  */
  UWord16  srchClkMode;
} WfwSrchClockCmdCfgStruct;

/*! @brief clk cfg command structure */
typedef struct
{
  /*! @Brief Per slot Srch Cycles */
  UWord32 srchCycles;
} WfwSrchCycleCmdCfgStruct;

/*! @brief sleep command structure */
typedef struct
{
  /*! @brief RxLM buffer index */
  UWord32 bufIndex;
}WfwSleepCmdStruct;

/*! @brief W2X idle config command structure */
typedef struct
{
  /*! @brief RF script buffer index */
  UWord16  : 2;               /* Bits 1:0 */
  /*! @brief RxLM buffer index */
  UWord16 rxlmHomeBufIndex : 5;             /* Bits 6:2 */
  /*! @brief RxLM buffer index, diversity chain */
  UWord16 rxlmHomeBufIndexDivChain : 5;     /* Bits 11:7 */
  /*! @brief tech mode specifier for other RAT (WfwW2xIdleCfgModeEnum: 0 for GSM / 1 for LTE) */
  UWord16 mode : 1;                         /* Bit 12 */
  UWord16 : 3;                              /* Bits 15:13 */
}WfwW2XIdleCfgCmdStruct;

/*! @brief DAC calibration command structure */
typedef struct
{
  /*! @brief Whether to do dac cal on ant 0 */
  UWord16 dacCalAnt0: 1;       /* Bit 0 */
  /*! @brief Whether to do dac cal on ant 1 */
  UWord16 dacCalAnt1: 1;       /* Bit 1 */
  UWord16 : 14;                /* Bits 15:2 */
}WfwTxDacCalCfgCmdStruct;



/*! @brief Tx LM config command structure */
typedef struct
{
  /*! @brief TxLM buffer index */
  UWord32 bufIndex;
  /*! @brief TxLM mode - extended to support DC-HSUPA:
      WfwTxlmCfgStateEnum - 0: disable, 1: enable, 2: enable DC */
  UWord32 mode;
}WfwTxLmCfgCmdStruct;

/*! @brief Tx LM update command structure */
typedef struct
{
  /*! @brief TxLM buffer index */
  UWord32 bufIndex;
}WfwTxLmUpdateCmdStruct;

/***********************************************/
/* Various response info definition structures */
/***********************************************/

/*! @brief Channel cfg update command information structure */
typedef struct
{
  /*! @brief Status for WCDMA FW sync command */
  UWord16 status;
} WfwSWRspMiscStatusStruct;
/* JUDC Command - DL component*/

typedef struct
{
  /* channel configuration update info */
  WfwSWCmdDemodRscCfgStruct judcChanCfg;
  /* combiner configuration update info */
  WfwSWCmdDemodRscCfgStruct judcCombCfg;

  /* JUDC DL channel action (WfwDemodJUDCActionTypeEnum)
     0 - disable, 1 - enable, 2 - seamless transition */
  UWord8 hsRachDLChansAction;

} WfwSWCmdJudcDlCfgStruct;

/* JUDC Command - UL component*/
typedef struct
{
  /* UL Channel Config*/
  /* DPCCH config command info */
  WfwSWCmdTxDpcchCfgStruct judcDpcchCfg;
  /* EUL config command info */
  /* In enable cmd, FW expexts startingSubframe = 1 (TTI = 2ms) and
                               startingSubframe = 5 (TTI = 10ms) */
  WfwSWCmdTxEulCfgStruct judcTxEulCfg;

  /* Indicates Update to DPCCH Cfg */
  UWord8 judcDpcchCfgUpdate:1;
  /* Indicates Update to EUL TX Cfg */
  UWord8 judcEulTxCfgUpdate:1;
  UWord8 :6;  /* Reserved bits 2-7 */

  /* JUDC UL channel action (WfwDemodJUDCActionTypeEnum)
     0 - disable, 1 - enable, 2 - seamless transition */
  UWord8 hsRachULChansAction;

} WfwSWCmdJudcUlCfgStruct;

/* JUDC Command*/
typedef struct
{

  /* DL Channel/Combiner Config */
  WfwSWCmdJudcDlCfgStruct judcDlCfg;
  /* UL Channel Config*/
  WfwSWCmdJudcUlCfgStruct judcUlCfg;

}WfwSWCmdJointUlDlCfgStruct;

/* QTA command struct */
typedef struct
{

  UWord16 : 2;            
  /*! @brief RxLM buf index and handle index, home frequency */
  UWord16 rxlmHomeBufIndex : 5;
  /*! @brief RxLM buf index, home frequency, diversity chain */
  UWord16 rxlmHomeBufIndexDivChain : 5;
  /*! @brief Reference phch index for Quick Tune Away start - not used currently */
  UWord16 refPhChIdx : 2;
  /*! @brief if HARQ buffers need to be flushed */
  UWord16 softBufFlush : 1;
  /*! @brief should be one of WfwQtaCmdTypeEnum*/
  UWord16 cmdType : 1;
  /*! @brief should be '1' if tuneback is not needed, else '0' */
  /* To be set only when sending QTA_STOP command */
  UWord16 isEmptyQtaGap : 1;
  /*! @brief should be '1' only for CxM Based QTA, else '0' for SW QTA */
  UWord16 useCxMBasedQta : 1;

} WfwQtaCfgCmdStruct;


/*! This is used to simulate RxBlank from VP*/
typedef struct
{
  /*---|----------------WSTMR FRAME0-------------------------------|-----FRAME1---*/
  /*---|<------desenseStart-----><----desenseCycle1----><----desenseCycle2------->*/
  /*                             <--desenseDur-->       <--desenseDur-->          */
  /*! offset from WSTMR frame 0 in cx1 units*/
  Word32 startCx1;
  /*! length of cycle in cx1 units*/
  Word32 cycleCx1;
  /*! duration of desense in the desense cycle in cx1 units*/
  Word32 durationCx1;

} WfwCoexBlankInfo;

typedef struct
{
  /*! Table of priorities for various activities*/
  UWord8  priorityTbl[WFW_COEX_ACTIVITY_CODES_MAX];
  /*! Tier 1 Priority for W */
  UWord8  priorityTier1;
  /*! Tier 2 Priority for W */
  UWord8  priorityTier2;
} WfwCoexPriorityTable;

typedef struct
{
  /*! Freq Ids for Rx (Home and Away)*/
  UWord32  rxFreqID[WFW_COEX_RX_FREQ_ID_MAX];
  /*! Freq Ids for Tx (Home and Away)*/
  UWord32  txFreqID[WFW_COEX_TX_FREQ_ID_MAX];
  /*! Rx Freq Ids for multi carriers */
  UWord32  multiCarrRxFreqID;
  /*! Tx Freq Ids for multi carriers */
  UWord32  multiCarrTxFreqID;
} WfwCoexFreqIDTableStruct;

typedef struct
{
  /* Channel ID information */
  UWord8 channelIDRx; 
  UWord8 channelIDTx;
} WfwCoexChannelIDStruct;

typedef struct
{
  /*! frequency ID table updates for de-sense gaps */
  WfwCoexFreqIDTableStruct    freqIdTab;
  /*! Priority table updates for de-sense gaps */
  WfwCoexPriorityTable        prioTab;
  /*! Update desense scenario transition*/
  UWord8                      desenseScenarioActive;
  /*! For VP Testing*/
  WfwCoexBlankInfo            blankInfo;
  /*! Type of DSDA command */
  WfwCoexCmdTypeEnum          cmdType;
  /*! Channel ID for Tx and Rx */
  WfwCoexChannelIDStruct      channelID;
} WfwCxmParamCfgCmdStruct;

/*****************************************/
/* Command/Response definition structure */
/*****************************************/

/*! @brief Union of various command structures */
typedef union
{
  /*! Search command info */
  WfwSrchCmdCfgStruct         srchCfg;
  /*! RxAGC command info */
  WfwRxAgcCmdCfgStruct        rxAgcCfg;
  /*! WFW start command info */
  WfwStartWfwCmdStruct        wfwStartCfg;
  /*! Demod config update command info */
  WfwSWCmdDemodCfgStruct      demodCfg;
  /*! Demback CCTrCh update command info  */
  WfwDembackDirtyBitStruct    dembackCctrchDirtyBitMsg;
  /*! TX Timing config command info */
  WfwSWCmdTxTimingCfgStruct   txTiming;
  /*! DPCCH config command info */
  WfwSWCmdTxDpcchCfgStruct    dpcchCfg;
  /*! RACH config command info */
  WfwSWCmdRachCfgStruct       rachCfg;
  /*! HS DPCCH config command info */
  WfwSWCmdTxHsDpcchCfgStruct  hsCfg;
  /*! EUL config command info */
  WfwSWCmdTxEulCfgStruct      txEulCfg;
  /*! Compressed mode cfg command info */
  WfwCmCfgCmdStruct           cmCfg;
  /*! Compressed mode search command info */
  WfwCmSrchCmdCfgUnion        cmSrchCfg;
  /*! FACH meas command info */
  WfwFachMeasCmdCfgStruct     fachMeasCfg;
  /*! IRAT X2W mode command */
  union
  {
    WfwX2wModeCmdStruct         x2wModeCfg;
    WfwG2wModeCmdStruct         g2wModeCfg;
  };
  /*! IRAT X2W mode cfg command */
  WfwX2wModeCfgCmdStruct        iratX2wModeCfg;
  /*! Freeze/ Unfreeze command */
  WfwSsFreezeCfgCmdStruct     ssFreezeCfg;
  /*! Srch clock config command info */
  WfwSrchClockCmdCfgStruct srchClkCfg;
  /*! Srch Cycle command info */
  WfwSrchCycleCmdCfgStruct srchCycleCfg;
  /*! CPC DTX/DRX config command info */
  WfwSWCmdCpcCfgStruct         cpcCfg;
  /*! RF tuning command info */
  WfwRftuneCmdStruct          rfTuneCfg;
  /*! Sleep command info */
  WfwSleepCmdStruct           sleepCfg;
  /*! W2X idle config command info */
  WfwW2XIdleCfgCmdStruct      w2xIdleCfg;
  /*! @brief TX dac cal cfg command */
  WfwTxDacCalCfgCmdStruct     txDacCalCfg;
  /*! TXLM config command info */
  WfwTxLmCfgCmdStruct         txLmCfg;
  /*! TxLM update command info */
  WfwTxLmUpdateCmdStruct      txLmUpdateCfg;
  /*! TXLM ET config command info */
  WfwTxLmCfgCmdStruct         txLmEtCfg;
  /*! Start IQ capture command info  */
  WfwStartIqCaptureCmdStruct  startIQCfg;
  /*! S-APT sample capture trigger command info  */
  WfwStartSaptIqCaptureCmdStruct  startSaptIQCfg;
  /*! EPT Calibration DPD apply command info */
  WfwEptCalApplyDpdCmdStruct applyDpdCfg;
  /*! EPT Calibration sample buffer cfg command info */
  WfwEptSampleBufferCfgCmdStruct sampleBufferCfg;
  /*! Joint UL DL Cfg commmand */
  WfwSWCmdJointUlDlCfgStruct jointUlDlCfg;
  /*! Rf init and cfg command */
  WfwSwCmdRfEventStruct       rfCmd;
  /*! RxLM dynamic update command info */
  WfwRxlmDynUpdateCmdStruct   rxlmDynUpdateCmdCfg;
  /*! QTA command */
  WfwQtaCfgCmdStruct          qtaCfg;
  /*! CXM Param Config command */
  WfwCxmParamCfgCmdStruct     cxmCfg;
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  /*! Antenna switching command info */
  WfwAntSwitchCfgCmdStruct    antSwitchCfg;
#endif
  /*! Rf Tune plus Step1 command */
  WfwRftuneStep1CmdStruct     rfTuneStep1Cfg;
  WfwSwCmdRfAdcTqConfigStruct rfAdcTqConfig;
} WfwSwCmdStruct;

/*! @brief Union of various response structures */
typedef union
{
  /*! @brief Cell cfg command info */
  WfwSWRspMiscStatusStruct statusRsp;
} WfwSwRspStruct;

/*! @brief Sync command/response structures */
typedef struct
{
  /*! @brief Command Id of this command */
  WfwSwCmdEnum cmdId;
  UWord8 :8;

  /*! @brief Union of various cmd response buffers */
  union
  {
    WfwSwCmdStruct cmd; /*!< @brief WCDMA FW sync command */
    WfwSwRspStruct rsp; /*!< @brief WCDMA FW response to sync command */
  } cmdRspBuf;
} WfwSWCmdRspStruct;

/*===========================================================================

                    EXTERNAL VARIBLE DECLARATION

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* WFW_SW_CMD_PROC_INTF_H */

