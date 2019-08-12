/*!
  @file
  rf_tdscdma_msg.h

  @brief
  Message router UMIDs header for RF TD-SCDMA subsystem. This file defines
  interfaces in the RF task.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/tdscdma/rf_tdscdma_msg.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/14   jyu     Separate RF WAKEUP_STEP0 into 0p1 and 0p2 for wakeup timeline optimization
11/10/14   jyu     Add message field to indicate if use_sw_tune_in_acq and to support 
                   CMAPI type 4 (for RxD override)
07/30/14   qzh     Enable FTM NS new intf and remove the dependency
07/17/14   ms      Sawless RX support
04/24/14   ych     Added dynamic tuner script for ifreq
04/24/14   ms      Added is_dsds in enter and enable_rx request structure
02/23/14   jyu     Added support for better handling RF exit_mode
                   in coordination of FW resource release
02/22/14   jyu     Added support for RF wakeup in parallel with MCPM
11/20/13   qzh     Featurized FTM NS new interface to disable it on BO 0.1
10/18/13   ych     Added support for TX_CHAIN NO
08/30/13   qzh     Update Non-signaling R4&HSUPA code with new TL1-RFSW interface 
                   and function partition
07/26/13   ms      AsDiv support
07/17/13   ms      Support for update tune code
06/01/13   nsh     Support Get Multi Synth for FTM RF Mode
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
02/19/13   jyu     Changed the number of IFREQ measurement to align with
                   RF-FW interface
12/12/12   zg      Added IQ capture support
08/23/12   ms      Added 3 RFA_TDSCDMA_MSG IDs
08/23/12   ms      Updated interface for primary and secondary device
02/14/12   ycl     Add Power scan state to enable RxLNA swpt mode updates.
09/23/11   jhe     ifreq/irat msg update
06/22/11   jhe     Update the interface
05/03/11   xw      Initial version

===========================================================================*/

#ifndef RF_TDSCDMA_MSG_H
#define RF_TDSCDMA_MSG_H

/*===========================================================================

                  INCLUDE FILES

===========================================================================*/
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#define FEATURE_RF_TL1_FTM_NEW_INTF
#endif

#include "appmgr_msg.h"
#include "msgr.h"
#include "msgr_types.h"
#include "rfa_msgr.h"
#include "rfcom.h"
#include "lm_types.h"


/*===========================================================================

             EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-----------------------------------------------------------------------*/
/*    MSG ID'S                                                           */
/*-----------------------------------------------------------------------*/

/*! @brief ID for RF to enter TD-SCDMA mode of operation REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_ENTER_MODE			    0x01

/*! @brief ID for RF to exit TD-SCDMA mode of operation REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_EXIT_MODE			    0x02

/*! @brief ID for RF to enable RX REQ and CNF (this includes tuning RX device)
 */
#define RFA_TDSCDMA_MSG_ID_ENABLE_RX       	        0x03

/*! @brief ID for RF to enable TX REQ and CNF (this includes tuning TX device)
 */
#define RFA_TDSCDMA_MSG_ID_ENABLE_TX        	    0x04

/*! @brief ID for RF to disable TX REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_DISABLE_TX        	    0x05

/*! @brief ID for RF to go to sleep REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_SLEEP            	    0x06

/*! @brief ID for RF to wake up REQ and CNF
 */ 
#define RFA_TDSCDMA_MSG_ID_WAKEUP                   0x07

/*! @brief ID to set the network Tx power limit REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_SET_TX_PWR_LIMIT 	    0x08

/*! @brief ID to query the RF warmup time REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_GET_RF_WARMUP_TIME       0x09

/*! @brief ID for RF to update AGC, LM and SSBI buffers for BHO REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_UPDATE_BHO     	        0x0A

/*! @brief ID for RF to swap buffers after BHO REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_SWITCH_BUF     	        0x0B

/*! @brief ID to request TDSCDMA neighbor list for measurement
 */
#define RFA_TDSCDMA_MSG_ID_UPDATE_TDS_NBR        	0x0C

/*! @brief ID to request GSM neighbor list for measurement
 */
#define RFA_TDSCDMA_MSG_ID_UPDATE_GSM_NBR    		0x0D

/*! @brief ID to get UE max tx power
 */
#define RFA_TDSCDMA_MSG_ID_GET_MAX_TX_PWR   		0x0E

/*! @brief ID to update therm backoff
 */
#define RFA_TDSCDMA_MSG_ID_THERM_BACKOFF     		0x0F

/*! @brief ID to init tx REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_INIT_TX      	        0x10

/*! @brief ID to set SG-TDS power limit REQ (note: this msg is for MCS)
 */
#define RFA_TDSCDMA_MSG_ID_SET_COEX_TX_PWR_LIMIT    0x11

//TODO - get_rxagc and get_agc_log most likely not needed, to delete
/*! @brief ID to query the receive AGC value REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_GET_RXAGC_VAL  	        0x12

/*! @brief ID to query a single or multiple AGC log REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_GET_AGC_LOG     	        0x13

/*! @brief ID to query a single or multiple AGC log REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_FW_RX_TX_HDET_RSP        0x14

/*! @brief ID to query a single or multiple AGC log REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_FW_RX_TX_TEMP_RSP        0x15

/*! @brief ID to query a single or multiple AGC log REQ and CNF
 */
#define RFA_TDSCDMA_MSG_ID_FW_STATE_CFG_RSP	        0x16

/*! @brief ID for IQ capture command and CNF
 */
#define RFA_TDSCDMA_MSG_ID_FW_IQ_CAPTURE_RSP        0x17

#define RFA_TDSCDMA_MSG_ID_FW_FTM_TX_RSP            0x18

#define RFA_TDSCDMA_MSG_ID_FW_FTM_RX_RSP            0x19

/*! @brief ID for TUNER temp comp
 */
#define RFA_TDSCDMA_MSG_ID_FW_RX_TX_TUNER_TEMP_RSP  0x1A

#define RFA_TDSCDMA_MSG_ID_FW_RX_TX_RF_DYNAMIC_UPDATE_RSP  0x1B


#define RFA_TDSCDMA_MSG_ID_FW_GET_MULTI_SYNTH_RSP   0x1C

#define RFA_TDSCDMA_MSG_ID_UPDATE_TUNER   0x1D
#define RFA_TDSCDMA_MSG_ID_ANT_SW                   0x1E

#define RFA_TDSCDMA_MSG_ID_ANT_SW_ABORT             0x1F

#define RFA_TDSCDMA_MSG_ID_GET_RF_TXCHAIN_NO       0x20

#define RFA_TDSCDMA_MSG_ID_DYNAMIC_SCRIPT           0x21

/*! @brief ID for RF to override rxd (dedicated for CMAPI type 4)
 */
#define RFA_TDSCDMA_MSG_ID_RXD_OVERRIDE   	        0x22

/*! Represents the ID of the last command - is the same as the command whose 
    ID is the max of all commands supported (if new msg added, need to update this)
*/ 
#define RFA_TDSCDMA_MSG_MAX_ID                      0x23

/****************************************************************************
                RF Task TD-SCDMA messages
****************************************************************************/
/*! @brief TD-SCDMA subsystem message UMIDs the RF task sends. 
*/
enum
{
  /*! Enter TD-SCDMA mode message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, ENTER_MODE, RFA_TDSCDMA_MSG_ID_ENTER_MODE, rfa_tdscdma_enter_mode_cnf_s),

  /*! Exit TD-SCDMA mode message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, EXIT_MODE, RFA_TDSCDMA_MSG_ID_EXIT_MODE, rfa_tdscdma_exit_mode_cnf_s),

  /*! Enable RX message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, ENABLE_RX, RFA_TDSCDMA_MSG_ID_ENABLE_RX, rfa_tdscdma_enable_rx_cnf_s),

  /*! Init TX message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, INIT_TX, RFA_TDSCDMA_MSG_ID_INIT_TX, rfa_tdscdma_init_tx_cnf_s),

  /*! Enable TX message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, ENABLE_TX, RFA_TDSCDMA_MSG_ID_ENABLE_TX, rfa_tdscdma_enable_tx_cnf_s),

  /*! Disable TX message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, DISABLE_TX, RFA_TDSCDMA_MSG_ID_DISABLE_TX, rfa_tdscdma_disable_tx_cnf_s),

  /*! RF sleep message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, SLEEP, RFA_TDSCDMA_MSG_ID_SLEEP, rfa_tdscdma_sleep_cnf_s),

  /*! RF wakeup message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, WAKEUP, RFA_TDSCDMA_MSG_ID_WAKEUP, rfa_tdscdma_wakeup_cnf_s),

  /*! RF set tx power limit message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, SET_TX_PWR_LIMIT, RFA_TDSCDMA_MSG_ID_SET_TX_PWR_LIMIT, rfa_tdscdma_set_tx_pwr_limit_cnf_s),

  /*! RF get RF warmup time message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, GET_RF_WARMUP_TIME, RFA_TDSCDMA_MSG_ID_GET_RF_WARMUP_TIME, rfa_tdscdma_get_rf_warmup_time_cnf_s),

  /*! RF get RF txchain no message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, GET_RF_TXCHAIN_NO, RFA_TDSCDMA_MSG_ID_GET_RF_TXCHAIN_NO, rfa_tdscdma_get_rf_txchain_no_cnf_s),  

  /*! RF update buffers for BHO message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, UPDATE_BHO, RFA_TDSCDMA_MSG_ID_UPDATE_BHO, rfa_tdscdma_update_bho_cnf_s),

  /*! RF swap buffers*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, SWITCH_BUF, RFA_TDSCDMA_MSG_ID_SWITCH_BUF, rfa_tdscdma_switch_buf_cnf_s),

  /*! RF update SSBI and GRFC buffers for up to 8 TDSCDMA neighbors message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, UPDATE_TDS_NBR, RFA_TDSCDMA_MSG_ID_UPDATE_TDS_NBR, rfa_tdscdma_update_tds_nbr_cnf_s),

  /*! RF update SSBI and GRFC buffers for up to 32 GSM neighbors message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, UPDATE_GSM_NBR, RFA_TDSCDMA_MSG_ID_UPDATE_GSM_NBR, rfa_tdscdma_update_gsm_nbr_cnf_s),

  /*! RF get UE max tx power message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, GET_MAX_TX_PWR, RFA_TDSCDMA_MSG_ID_GET_MAX_TX_PWR, rfa_tdscdma_get_max_tx_pwr_cnf_s),

  /*! RF update therm backoff message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, THERM_BACKOFF, RFA_TDSCDMA_MSG_ID_THERM_BACKOFF, rfa_tdscdma_therm_backoff_cnf_s),

  //TODO - get_rxagc and get_agc_log most likely not needed, to delete
  /*! RF receive AGC value message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, GET_RXAGC_VAL, RFA_TDSCDMA_MSG_ID_GET_RXAGC_VAL, rfa_tdscdma_get_rxagc_val_cnf_s),

  /*! RF get agc log message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, GET_AGC_LOG, RFA_TDSCDMA_MSG_ID_GET_AGC_LOG, rfa_tdscdma_get_agc_log_cnf_s),

  /*! RF get timer trigger for tuner message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, UPDATE_TUNER, RFA_TDSCDMA_MSG_ID_UPDATE_TUNER, rfa_tdscdma_update_tuner_cnf_s),

   /*! RF ant sw log message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, ANT_SW, RFA_TDSCDMA_MSG_ID_ANT_SW, rfa_tdscdma_ant_sw_cnf_s),
  
   /*! Dynamic tuner script for ifreq*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, DYNAMIC_SCRIPT, RFA_TDSCDMA_MSG_ID_DYNAMIC_SCRIPT, rfa_tdscdma_dynamic_script_cnf_s),

  /*! RxD Override message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, CNF, RXD_OVERRIDE, RFA_TDSCDMA_MSG_ID_RXD_OVERRIDE, rfa_tdscdma_rxd_override_cnf_s),

};

/*! @brief TD-SCDMA subsystem message UMIDs the RF task receives. 
*/
enum
{
  /*! Enter TD-SCDMA mode message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, ENTER_MODE, RFA_TDSCDMA_MSG_ID_ENTER_MODE, rfa_tdscdma_enter_mode_req_s),

  /*! Exit TD-SCDMA mode message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, EXIT_MODE, RFA_TDSCDMA_MSG_ID_EXIT_MODE, rfa_tdscdma_exit_mode_req_s),

  /*! Enable RX message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, ENABLE_RX, RFA_TDSCDMA_MSG_ID_ENABLE_RX, rfa_tdscdma_enable_rx_req_s),

  /*! Init TX message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, INIT_TX, RFA_TDSCDMA_MSG_ID_INIT_TX, rfa_tdscdma_init_tx_req_s),

  /*! Enable TX message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, ENABLE_TX, RFA_TDSCDMA_MSG_ID_ENABLE_TX, rfa_tdscdma_enable_tx_req_s),

  /*! Disable TX message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, DISABLE_TX, RFA_TDSCDMA_MSG_ID_DISABLE_TX, rfa_tdscdma_disable_tx_req_s),

  /*! RF sleep message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, SLEEP, RFA_TDSCDMA_MSG_ID_SLEEP, rfa_tdscdma_sleep_req_s),

  /*! RF wakeup message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, WAKEUP, RFA_TDSCDMA_MSG_ID_WAKEUP, rfa_tdscdma_wakeup_req_s),

  /*! RF set tx power limit message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, SET_TX_PWR_LIMIT, RFA_TDSCDMA_MSG_ID_SET_TX_PWR_LIMIT, rfa_tdscdma_set_tx_pwr_limit_req_s),

  /*! RF get RF warmup time message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, GET_RF_WARMUP_TIME, RFA_TDSCDMA_MSG_ID_GET_RF_WARMUP_TIME, rfa_tdscdma_get_rf_warmup_time_req_s),

  /*! RF get RF txhain no message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, GET_RF_TXCHAIN_NO, RFA_TDSCDMA_MSG_ID_GET_RF_TXCHAIN_NO, rfa_tdscdma_get_rf_txchain_no_req_s),  

  /*! RF update buffers for BHO message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, UPDATE_BHO, RFA_TDSCDMA_MSG_ID_UPDATE_BHO, rfa_tdscdma_update_bho_req_s),

  /*! RF swap buffers*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, SWITCH_BUF, RFA_TDSCDMA_MSG_ID_SWITCH_BUF, rfa_tdscdma_switch_buf_req_s),

  /*! RF update SSBI and GRFC buffers for up to 8 TDSCDMA neighbors message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, UPDATE_TDS_NBR, RFA_TDSCDMA_MSG_ID_UPDATE_TDS_NBR, rfa_tdscdma_update_tds_nbr_req_s),

  /*! RF update SSBI and GRFC buffers for up to 32 GSM neighbors message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, UPDATE_GSM_NBR, RFA_TDSCDMA_MSG_ID_UPDATE_GSM_NBR, rfa_tdscdma_update_gsm_nbr_req_s),

  /*! RF get UE max tx power message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, GET_MAX_TX_PWR, RFA_TDSCDMA_MSG_ID_GET_MAX_TX_PWR, rfa_tdscdma_get_max_tx_pwr_req_s),

  /*! RF update therm backoff message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, THERM_BACKOFF, RFA_TDSCDMA_MSG_ID_THERM_BACKOFF, rfa_tdscdma_therm_backoff_req_s),

  /*! RF set SG tx power limit message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, SET_COEX_TX_PWR_LIMIT, RFA_TDSCDMA_MSG_ID_SET_COEX_TX_PWR_LIMIT, rfa_tdscdma_set_coex_tx_pwr_limit_req_s),

  //TODO - get_rxagc and get_agc_log most likely not needed, to delete
  /*! RF receive AGC value message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, GET_RXAGC_VAL, RFA_TDSCDMA_MSG_ID_GET_RXAGC_VAL, rfa_tdscdma_get_rxagc_val_req_s),

  /*! RF get agc log message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, GET_AGC_LOG, RFA_TDSCDMA_MSG_ID_GET_AGC_LOG, rfa_tdscdma_get_agc_log_req_s),

  /*! RF get timer trigger for tuner message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, UPDATE_TUNER, RFA_TDSCDMA_MSG_ID_UPDATE_TUNER, rfa_tdscdma_update_tuner_req_s),

  /*! RF get agc log message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, ANT_SW, RFA_TDSCDMA_MSG_ID_ANT_SW, rfa_tdscdma_get_agc_log_req_s),

  /*! RF get agc log message*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, IND, ANT_SW_ABORT, RFA_TDSCDMA_MSG_ID_ANT_SW_ABORT, rfa_tdscdma_ant_sw_abort_ind_s),

  /*! Dynamic tuner script for ifreq*/
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, DYNAMIC_SCRIPT, RFA_TDSCDMA_MSG_ID_DYNAMIC_SCRIPT, rfa_tdscdma_dynamic_script_req_s),

  /*! RxD Override message */
  MSGR_DEFINE_UMID(RFA, RF_TDSCDMA, REQ, RXD_OVERRIDE, RFA_TDSCDMA_MSG_ID_RXD_OVERRIDE, rfa_tdscdma_rxd_override_req_s),

};

/*===========================================================================

                   DEFINITIONS AND TYPES

===========================================================================*/

/*! Max number of Inter-Freq neighbors */
#define RFA_TDSCDMA_IFREQ_NBR_MAX_NUM TFW_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS

/*! Max number of IRAT GSM neighbors */
#define RFA_TDSCDMA_T2G_NBR_MAX_NUM 32

/*! Return status code type */
typedef enum
{
  RFA_TDSCDMA_SUCCESS=1,
  RFA_TDSCDMA_STATE_MACHINE_ERROR=2,
  RFA_TDSCDMA_EXECUTION_ERROR=3,
  RFA_TDSCDMA_INVALID_PARAM_ERROR=4,
  RFA_TDSCDMA_CMD_OUTCOME_MAX=5,
} rfa_tdscdma_cmd_outcome_t;

/*! BHO buffer swap type */
/* To swap rx buffers, swap tx buffers, or swap rx and tx buffers at the same time (0ms BHO) */
typedef enum
{
  RFA_TDSCDMA_BUF_RX,
  RFA_TDSCDMA_BUF_TX,
  RFA_TDSCDMA_BUF_RXTX,
  RFA_TDSCDMA_BUF_MAX,
} rfa_tdscdma_buf_swap_t;

/*! RxLM mode type */
typedef enum
{
  RFA_TDSCDMA_RXLM_MODE_ACQ,
  RFA_TDSCDMA_RXLM_MODE_TRK,
  RFA_TDSCDMA_RXLM_MODE_TRK_IDLE,
  RFA_TDSCDMA_RXLM_MODE_TRK_IRAT,
  RFA_TDSCDMA_RXLM_MODE_PWR_SCAN,
  RFA_TDSCDMA_RXLM_MODE_MAX,
} rfa_tdscdma_rxlm_mode_t;

/*! Rx path type */
/* Primary path only, diversity path only, or both */
typedef enum
{
  RFA_TDSCDMA_RX_PATH_PRIM,
  RFA_TDSCDMA_RX_PATH_DIV,
  RFA_TDSCDMA_RX_PATH_BOTH,
  RFA_TDSCDMA_RX_PATH_MAX,
} rfa_tdscdma_rx_path_t;


/*! Neighbor entry struct */
typedef struct
{
  /* neighbor channel */
  uint16  channel;
  /* delta buffer index for this neighbor */
  uint8   nbr_buf_idx;
} rfa_tdscdma_nbr_entry_t;
 

/*! RF TDSCDMA enter mode req type */
typedef struct
{
  msgr_hdr_struct_type    hdr;
  rfm_device_enum_type device;
  rfm_device_enum_type sec_device;
  /* rxlm working buffer idx (f1) */
  lm_handle_type          rxlm_buf_idx;
  /* rxlm working buffer mode (f1) */
  rfa_tdscdma_rxlm_mode_t rxlm_mode;
  boolean is_dsds;
} rfa_tdscdma_enter_mode_req_s;

/*! RF TDSCDMA enter mode cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
  boolean is_sawless;
} rfa_tdscdma_enter_mode_cnf_s;

/*! RF TDSCDMA exit mode req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfm_device_enum_type   device;
  rfm_device_enum_type   sec_device;
  boolean                wait_for_fw_release;
} rfa_tdscdma_exit_mode_req_s;

/*! RF TDSCDMA exit mode cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_exit_mode_cnf_s;

/*! RF TDSCDMA enable Rx req type */
typedef struct
{
  msgr_hdr_struct_type    hdr;
  rfm_device_enum_type device;
  rfm_device_enum_type sec_device;
  /* primary only, diversity only, or both */
  rfa_tdscdma_rx_path_t   rx_path;
  /* rx working channel (f1) */
  uint16                  channel;
  /* rxlm working buffer idx (f1) - primary */
  lm_handle_type          rxlm_buf_idx;
  /* rxlm working buffer mode (f1) - primary */
  rfa_tdscdma_rxlm_mode_t rxlm_mode;
  /* rxlm working buffer idx (f1) - diversity */
  lm_handle_type          rxlm_buf_idx_div;
  /* rxlm working buffer mode (f1) - diversity */
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div;
  boolean is_dsds;
  uint8 sawless_rx_lin_state;
  boolean use_sw_tune_in_acq;
} rfa_tdscdma_enable_rx_req_s;

/*! RF TDSCDMA enable Rx cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  /* carrier frequency in Hz */
  uint32                    carrier_freq;
  /* rx rf(agc,grfc,ssbi) buffer idx (f1) */
  uint8                     rx_buf_idx;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_enable_rx_cnf_s;

// Init tx is reserved for future use
/*! RF TDSCDMA init tx req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  ///* txlm working buffer idx (f1) */
  //lm_handle_type         txlm_buf_idx;
} rfa_tdscdma_init_tx_req_s;

/*! RF TDSCDMA init tx cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_init_tx_cnf_s;

/*! RF TDSCDMA enable Tx req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfm_device_enum_type device;
  
  /* tx working channel (f1), should be same as rx working channel */
  uint16                 channel;
  /* txlm working buffer idx (f1) */
  lm_handle_type         txlm_buf_idx;
} rfa_tdscdma_enable_tx_req_s;

/*! RF TDSCDMA enable Tx cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  /* tx rf(agc,grfc,ssbi) buffer idx (f1) */
  uint8                     tx_buf_idx;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_enable_tx_cnf_s;

/*! RF TDSCDMA disable Tx req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfm_device_enum_type device;
  
} rfa_tdscdma_disable_tx_req_s;

/*! RF TDSCDMA disable Tx cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_disable_tx_cnf_s;

/*! RF TDSCDMA sleep req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfm_device_enum_type device;
  rfm_device_enum_type sec_device;
} rfa_tdscdma_sleep_req_s;

/*! RF TDSCDMA sleep cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_sleep_cnf_s;

/*! RF TDSCDMA wakeup req type */

typedef enum
{
  RFA_TDSCDMA_WAKEUP_STEP0,
  RFA_TDSCDMA_WAKEUP_STEP1,
  RFA_TDSCDMA_WAKEUP_TOGETHER,
  RFA_TDSCDMA_WAKEUP_STEP0P1,
  RFA_TDSCDMA_WAKEUP_STEP0P2,
  RFA_TDSCDMA_WAKEUP_NA
} rfa_tdscdma_wakeup_indicator_t;

typedef struct
{
  msgr_hdr_struct_type    hdr;
  rfm_device_enum_type device;
  rfm_device_enum_type sec_device;
  /* primary only, diversity only, or both */
  rfa_tdscdma_rx_path_t   rx_path;
  /* rx channel to wakeup on (f1) */
  uint16                  channel;
  /* rxlm working buffer idx (f1) - primary */
  lm_handle_type          rxlm_buf_idx;
  /* rxlm working buffer mode (f1) - primary */
  rfa_tdscdma_rxlm_mode_t rxlm_mode;
  /* rxlm working buffer idx (f1) - diversity */
  lm_handle_type          rxlm_buf_idx_div;
  /* rxlm working buffer mode (f1) - diversity */
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div;
  /* step0, step1, or together */
  rfa_tdscdma_wakeup_indicator_t wakeup_step;  
} rfa_tdscdma_wakeup_req_s;

/*! RF TDSCDMA wakeup cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_wakeup_indicator_t wakeup_step;  
  boolean                   rebuild_ifreq_script;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_wakeup_cnf_s;

/*! RF TDSCDMA set Tx power limit req type */
typedef struct
{
  msgr_hdr_struct_type hdr;
  /* network tx pwr limit in dBm */
  int16                tx_pwr_limit;
} rfa_tdscdma_set_tx_pwr_limit_req_s;

/*! RF TDSCDMA set Tx power limit cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_set_tx_pwr_limit_cnf_s;

/*! RF TDSCDMA get rf warmup time req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_tdscdma_get_rf_warmup_time_req_s;

/*! RF TDSCDMA get rf warmup time cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  /* rf warmup time in micro seconds */
  uint16                    rf_warmup_time; 
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_get_rf_warmup_time_cnf_s;

/*! RF TDSCDMA get rf txchain no req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_tdscdma_get_rf_txchain_no_req_s;

/*! RF TDSCDMA get rf txchain no cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  /* rf chain no */
  uint16                    rf_txchain_no; 
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_get_rf_txchain_no_cnf_s;

/*! RF TDSCDMA update BHO req type */
typedef struct
{
  msgr_hdr_struct_type    hdr;
  rfm_device_enum_type device;
  rfm_device_enum_type sec_device;
  /* primary chan for the target cell */
  uint16                  tgt_prim_channel;
  /* target channel(f3) */
  uint16                  tgt_channel;
  /* primary only, diversity only, or both */
  rfa_tdscdma_rx_path_t   rx_path;
  /* rxlm target buffer idx (f3) - primary */
  lm_handle_type          rxlm0_tgt_buf_idx;
  /* rxlm target buffer mode (f3) - primary */
  rfa_tdscdma_rxlm_mode_t rxlm_mode;
  /* rxlm target buffer idx (f3) - diversity */
  lm_handle_type          rxlm_tgt_buf_idx_div;
  /* rxlm target buffer mode (f3) - diversity */
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div;
  /* txlm target buffer idx (f3) */
  lm_handle_type          txlm_tgt_buf_idx;
} rfa_tdscdma_update_bho_req_s;

/*! RF TDSCDMA update BHO cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  /* carrier frequency in Hz, corresponding to tgt_prim_channel */
  uint32                    prim_carrier_freq;
  /* carrier frequency in Hz, corresponding to tgt_channel */
  uint32                    carrier_freq;
  /* rx rf(agc,grfc,ssbi) buffer idx (f3) */
  uint8                     rx_buf_idx;
  /* tx rf(agc,grfc,ssbi) buffer idx (f3) */
  uint8                     tx_buf_idx;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_update_bho_cnf_s;

/*! RF TDSCDMA switch Buffer req type */
typedef struct
{
  msgr_hdr_struct_type    hdr;
  /* To swap rx buffers, swap tx buffers, or swap rx and tx buffers at the same time (0ms BHO) */
  rfa_tdscdma_buf_swap_t  buf_swap;
  /* The rxlm buffer idx that is becoming the working buffer - primary */
  lm_handle_type          rxlm0_wrk_buf_idx;
  /* The rxlm buffer idx that is becoming the working buffer - diversity */
  lm_handle_type          rxlm_wrk_buf_idx_div;
  /* The txlm buffer idx that is becoming the working buffer */
  lm_handle_type          txlm_wrk_buf_idx;
} rfa_tdscdma_switch_buf_req_s;

/*! RF TDSCDMA switch Buffer cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_switch_buf_cnf_s;

/*! RF TDSCDMA update TDS neighbor list req type */
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Number of TDS neighbors */
  uint8                    num_nbr;
  /* Neighbor list */ 
  rfa_tdscdma_nbr_entry_t  *nbr_list;
  /* carrier freq corresponding to nbr chan - to be filled by rf */
  uint32                   *carrier_freq_list;
  /* primary only, diversity only, or both */
  rfa_tdscdma_rx_path_t    rx_path;
  /* IFREQ rxlm buffer idx - primary */
  lm_handle_type           ifreq_rxlm_buf_idx;
  /* IFREQ rxlm mode - primary */
  rfa_tdscdma_rxlm_mode_t  ifreq_rxlm_mode;
  /* IFREQ rxlm buffer idx - diversity */
  lm_handle_type           ifreq_rxlm_buf_idx_div;
  /* IFREQ rxlm mode - diversity */
  rfa_tdscdma_rxlm_mode_t  ifreq_rxlm_mode_div;
  /* The delta buffer index for which diversity is needed */
  uint8                    div_buf_idx;
} rfa_tdscdma_update_tds_nbr_req_s;

/*! RF TDSCDMA update TDS neighbor list cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_update_tds_nbr_cnf_s;

/*! RF TDSCDMA update GSM neighbor list req type */
typedef struct
{
  msgr_hdr_struct_type    hdr;
  /* Number of GSM neighbours */
  uint8                   num_nbr;
  /* Neighbor list */ 
  rfa_tdscdma_nbr_entry_t *nbr_list;
  /* rxlm T2G buffer idx */
  lm_handle_type          rxlm_t2g_buf_idx;
} rfa_tdscdma_update_gsm_nbr_req_s;

/*! RF TDSCDMA update GSM neighbor list cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_update_gsm_nbr_cnf_s;

/*! RF TDSCDMA get ue max tx power req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_tdscdma_get_max_tx_pwr_req_s;

/*! RF TDSCDMA get ue max tx pwr cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  /* ue max tx pwr in dbm */
  int16                     max_tx_pwr; 
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_get_max_tx_pwr_cnf_s;

/*! RF TDSCDMA therm backoff req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  /* therm backoff in db */
  uint8                  backoff; 
} rfa_tdscdma_therm_backoff_req_s;

/*! RF TDSCDMA therm backoff cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_therm_backoff_cnf_s;

/*! RF TDSCDMA set SG-TDS coex Tx power limit req type */
/* note: this msg is sent from MCS */
typedef struct
{
  msgr_hdr_struct_type hdr;
  /* SAR Tx Power limit in dBm x10 */
  int16                sar_tx_pwr_limit;
  /* Coex Tx Power limit in dBm x10 */
  int16                coex_tx_pwr_limit;
} rfa_tdscdma_set_coex_tx_pwr_limit_req_s;


//TODO - get_rxagc and get_agc_log most likely not needed, to delete
/*! RF TDSCDMA receive AGC req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfm_device_enum_type device;
  rfm_device_enum_type sec_device;
} rfa_tdscdma_get_rxagc_val_req_s;

/*! RF TDSCDMA receive AGC cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  uint16                    rxagc_result; 
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_get_rxagc_val_cnf_s;

/*! RF TDSCDMA get AGC log req type */
typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_tdscdma_get_agc_log_req_s;

/*! RF TDSCDMA get AGC log cnf type */
typedef struct
{
  msgr_hdr_struct_type      hdr;
  uint16                    length;
  uint16*                   agc_log_result; 
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_get_agc_log_cnf_s;

typedef struct
{
  msgr_hdr_struct_type      hdr;
  boolean is_immediate;           //TRUE= immediate write in ACQ mode, FALSE = build script in TRK mode
} rfa_tdscdma_update_tuner_req_s;


typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_update_tuner_cnf_s;


typedef struct
{
  rfm_device_enum_type device;
  /* Antenna switch position */
  uint8 pos;
  void* cb_func;
  void* cb_data;
  boolean is_immediate;
} rfa_tdscdma_ant_sw_params_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_tdscdma_ant_sw_params_s ant_sw_params;
} rfa_tdscdma_ant_sw_req_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_tdscdma_ant_sw_params_s ant_sw_params;
  /* Result of the request */
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_ant_sw_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_tdscdma_ant_sw_abort_ind_s;

typedef struct
{
  msgr_hdr_struct_type    hdr;
  uint32   ifreq_meas_freq;         /* ifreq meas frequency in Hz */
  uint8    ifreq_buf_idx;           /* ifreq meas buffer idx (f2) */
} rfa_tdscdma_dynamic_script_req_s;

typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_dynamic_script_cnf_s;

typedef struct
{
  msgr_hdr_struct_type    hdr;
  boolean   rxd_override;         /* override RxD */
} rfa_tdscdma_rxd_override_req_s;

typedef struct
{
  msgr_hdr_struct_type      hdr;
  rfa_tdscdma_cmd_outcome_t req_result;
} rfa_tdscdma_rxd_override_cnf_s;


#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
typedef enum
{
  RF_TDSCDMA_DISPATCH_SUCESSFUL=1,
  RF_TDSCDMA_DISPATCH_UNSUCESSFUL=2,
  RF_TDSCDMA_DISPATCH_NULL_PTR=3,
  RF_TDSCDMA_DISPATCH_INVALID_PARAM =4,
  RF_TDSCDMA_DISPATCH_MAX
}rf_tdscdma_dispatch_outcome_t;
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


#endif /* RF_TDSCDMA_MSG_H */



