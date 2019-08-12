/*==============================================================================

  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

==============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/gsm/rflm_gsm_dm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   ase     Optimize DM usage
12/07/15   bp      Add variable to store tx_stop priority for HMSC case
07/22/15   bp      Increase therm_read transactions to 30 for WTR2955
07/21/15   bp      Increase TX cal transactions to 60
03/16/15   sml     Increase the number of tasks for rx stop scripts to avoid crash in 
                   probe burst for dlca card
02/24/15   piy     Increase RFGSM_RX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B to 15 to support 
                   Tuner transactions during Probe Burst
01/30/15   sc      Add RF Sequence Number
01/13/15   sw      Increase RX stop tasks from 5 to 6 for probe burst/tuner with RxD
12/18/14   ec      Add holdoff flag to JDET structure to prevent Therm\JDET read clash
11/17/14   sc      Read and calculate delay match DM1-3 in RFLM GSM TxFE config
11/13/14   tsr     Increased Enter Mode transactions for IMRD
11/03/14   tsr     Increased Number of transactions for TX stop TaskQB and RX Stop for TaskQB to 10
                   Decreased Number of tasks for RX Start TaskQA to 3
                   Reduced RFGSM_THERM_MAX_TASKS_TASKQ_B to 1 and num transactions to 0
10/23/14   sc      Enable event type control from RFSW
10/16/14   sc      Enable non-issue sequence event tasks to be atomic
10/10/14   tws     Error fatal if DM memory size is > hard limit
10/10/14   tws     Increase init rffes to 220
10/07/14   sc      Program all delay match settings in RFLM DRIF
10/02/14   sc      Move dBm100 to dBm10 to RFSW to reduce overhead on RFLM
10/01/14   sc      Reduce overhead on getting table ptr
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status
09/19/14   ch      Introduce RFLM_GSM_PROCESSING_OVERHEAD to fix 144MHz GSM voice call drop
09/09/14   tsr     Temporarily decrease num of RX_START_MAX_NON_SSBI_TRANSACTIONS 
                   to 10 to accomodate increased WTR RX trans for rfm_dev2/rfm_dev3
08/18/14   sc      Create switch between basic and advanced profiling
08/11/14   cj      Increased DM tech buff size for tx sharing
07/23/14   cj      Added changes for Tx sharing feature
07/30/14   hoh     Add enhanced rx feature support 
07/28/14   ec      Add type definitions and data structures for Jammer detector reads
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/22/14   sc      Asserts to error fatals for more crash information
07/14/14   sc      Remove unused/unrequired functionality
07/09/14   tws     Add new RX burst type PCH
07/09/14   tws     Save DM memory by sharing between ip2 and rx.
07/08/14   jr      Increase ssbi transaction taskq to support WTR2605 IRAT
07/03/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX
07/03/14   ch      replaced tuner DM structure with pointer to common tuner
07/01/14   jr      Enable therm read on sub-2 
06/19/14   sb      changes to support cal on c2 for DSDA
06/10/14   hk      sub_id interface in FTM mode
06/06/14   sb      Reduce GSM heap memory usage
06/04/14   sc      Increase interface index buffer size
06/03/14   sb      DM memory allocation changes to support WTR2605
05/30/14   ldg     Add const define for slot mode control function
05/29/14   sb      IP2 cal bug fixes, updates
05/23/14   sc      Log intf data in RFLM trace buffer to tie up RFSW timeline
05/20/14   sc      Move mutex code to DM to allow common use of APIs
05/16/14   shb     Increased RFGSM_TX_CAL_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A to 51
05/13/14   sc      Add RFLM GSM initialisation functionality
05/13/14   ggs     Need to use triple buffering for Tx calibration
04/30/14   hoh     Increase size of RFFE task queue to handle addl APT transactions
04/24/14   ch      Increased Rx Tx frontend config max tasks
04/22/14   ch      Created dedicated Rx Tx frontend config TQs
04/17/14   sb      Increase number of atomic tasks for TX stop
04/16/14   sb      IP2 cal Interface and functionality changes
04/11/14   tsr     Add RFLM RX gain params and spectral inversion coefficients for DRX
04/11/14   sc      Add multiSIM support for UMPLT initialisation
04/10/14   sc      Add support for FW cb registration
04/07/14   sc      Add multi simultaneous CCS resource management support
04/04/14   sb      Add interface changes for IP2 cal + profiling RFLM
03/31/14   tsr     Increased RX Start script from 25 to 31 to accomodate RXD WTR writes
03/31/04   tsr     Increased GSM Tech DM Memory to 50K for RXD
03/26/14   sb      Add support for IP2 cal for Dime PM
03/26/14   sc      Add UMPLT initialisation functionality
03/18/14   tsr     Increased GSM tech DM memory to 47k( 43008 + 4096)
03/18/14   tsr     Increase RX stop buffer size for RFFEs to 12
03/13/14   lcl     Increase TX_ENABLE script buffer size to 80 
03/13/14   ec      Fix potential overflow bug with Tx frame parameters
03/11/14   sc      Correct the PA ramp up table for accurate PA ramp adjust
03/07/14  hm/shb   Increased GSM TX start size to 51
03/03/14   sc      Add device populated by RF SW to manage CCS TQs
02/17/14   sc      Add profiling and improved logging
02/05/14   sc      Add improved return from alloc/free DM mem APIs
02/06/14   sw      Optimise therm read non-SBI task transactions
01/30/14   sc      Remove unused/unrequried Tx/Rx vars
01/24/14   tsr     Added TXAGC trigger for trigger step during TX calibration
01/22/14   tsr     WTR TXAGC support for Multislot TX
01/22/14   sc      Manage DM memory based on sub/mode/chain instead of device
01/21/14   ggs     Initial changes for TXC DCOC and DPD calibration 
01/07/14   sc      Increase max number of non-standalone monitors to 7
01/06/14   tsr     Increased Number of TaskQB TX Cal Non SSBI transactions from 18 to 28 for SSKU card
12/16/13   tsr     Increased Number of set tx band transactions to 30 based on SS v95
12/13/13   hm      Increased Number of TX Start and TX Cal transactions to 40,
                   for GSM and EDGE multislot TX AGC
12/12/13   sc      Add predistortion enabled flag
12/11/13   sc      Add Tx start Tx transition merge
12/10/13   sc      Add envelope gain to Tx vars
12/09/13   sc      Remove unused Rx/Tx variables
12/03/13   tsr     Increased Number of Enter mode transactions to 200 based on SS v93
11/25/13   sc      Reduce data table size to less than max allocated
11/13/13   cj      Added script size for SSBI and NonSSBI trigger script
11/11/13   ggs     Added implementation to retrieve DMA data for Tx cal
11/07/13   tsr     TX Cal:Correct max number of transactions for each CCS taskq on Bolt/D3925 
11/05/13   ggs     Changes required for Tx Calibration support on Bolt
10/24/13   ggs     Tx Calibration support
10/21/13   sc      Set the event defines to specific RF card numbers
10/21/13   sc      Bug fixes and logging enhancement
10/15/13   sc      Add RFLM GSM DM table ptr set API for when mem is in RFSW
10/04/13   sc      TxFE programming functionality
09/26/13   sc      Add FW->RFLM RF->RFLM data interface
09/11/13   sc      Add memory size thresholds and validation
09/09/13   sc      Clean up header includes 
09/04/13   ra      Support interface to CCS_RF_INTF 
09/03/13   sc      Update headers and type resource 
08/22/13   sc	     Initial version of RFLM GSM DM
==============================================================================*/

#ifndef RFLM_GSM_DM_H
#define RFLM_GSM_DM_H


/*=============================================================================

                                  INCLUDES

==============================================================================*/
#include "comdef.h"
#include "rflm_api_gsm.h"
#include "rflm_ccs_rf_intf.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_time_profile.h"
#include "rflm_cmn_intf.h"
#include "qmutex.h"

/*=============================================================================

                          RFLM GSM DM DEFINITIONS

==============================================================================*/
#define RFLM_GSM_MAX_TX_UL_SLOTS             RFLM_GSM_MAX_UL_TS

#define RFLM_GSM_MAX_STANDALONE_RSSI_MEASUREMENTS (12)

#define RFLM_GSM_DOUBLE_BUFFER               2
#define RFLM_GSM_TRIPLE_BUFFER               3
#define RFLM_GSM_BUFFER_ID_NOT_REQUIRED      0xFF

#define RFLM_GSM_BURST_NUM_NOT_REQUIRED      0xFF

#define RFLM_GSM_MAX_RSSI_MEASUREMENTS       12

#define RFLM_GSM_MAX_NUM_RAMP_SAMPLES_52     24
#define RFLM_GSM_MAX_NUM_RAMP_SAMPLES_57P6   26
#define RFLM_GSM_MAX_NUM_RAMP_SAMPLES_76P8   36
#ifdef FEATURE_BOLT_MODEM
#define RFLM_GSM_MAX_NUM_RAMP_SAMPLES        RFLM_GSM_MAX_NUM_RAMP_SAMPLES_76P8
#define RFLM_GSM_MAX_NUM_AMAM_SAMPLES        64
#define RFLM_GSM_MAX_NUM_AMPM_SAMPLES        64
#else
#define RFLM_GSM_MAX_NUM_RAMP_SAMPLES        RFLM_GSM_MAX_NUM_RAMP_SAMPLES_52
#define RFLM_GSM_MAX_NUM_AMAM_SAMPLES        256
#define RFLM_GSM_MAX_NUM_AMPM_SAMPLES        256
#endif

/* Interface data entries */
/* This number must account for index retention during the acq (sync Rx)     */
/* lasting 33 frames while the other sub can be actively doing the worst case*/
/* 8 monitors per frame every frame                                          */
/* DSDA..                                                                    */
/* Sub-x acquisition max window of 33 frames                                 */
/* Sub-y 8mons per frame x (1Rx start idx+1Rx stop idx) x 33 frames          */
/* Note: rounded up as number must be an integer log2(n) number for efficient*/
/*       wrap-around masking reasons                                         */
#define RFLM_GSM_DM_MAX_INTF_DATA_ENTRIES   1024
#ifdef FEATURE_TX_SHARING
/* For preload script & trigger scripts interface index might wrap around and 
become invalid by the time the script needs to be executed for such cases handle meta is used
instead of interface index to retrieve device and sub info */

#define RFLM_GSM_DM_INTF_INDEX_TX_INIT_PREDEFINED RFLM_GSM_DM_MAX_INTF_DATA_ENTRIES+1 /*1025*/
#define RFLM_GSM_DM_INTF_INDEX_TX_ENABLE_PREDEFINED RFLM_GSM_DM_INTF_INDEX_TX_INIT_PREDEFINED+1 /*1026*/
#define RFLM_GSM_DM_INTF_INDEX_TX_DISABLE_PREDEFINED RFLM_GSM_DM_INTF_INDEX_TX_ENABLE_PREDEFINED+1 /*1027*/

#endif

#define RLFM_GSM_MAX_FRAME_BUFFERS           2

/* These macros replace GFW_GRFC "fake grfc" definitions that are used to set the register
"tag" value in rflm_gsm_msm_reg_script_unit */

#define RFLM_GSM_TX_CAL_EP_GAIN                        0x309
#define RFLM_GSM_TXC_DCOC                              0x30E
#define RFLM_GSM_TXC_PP_PHASE_OVR_VAL                  0x312
#define RFLM_GSM_TXC_EP_ENV_OVR_VAL                    0x313
#define RFLM_GSM_TXC_PREDISTORTION_ENABLE              0x314
#define RFLM_GSM_TXC_PREDISTORTION_PROGRAM             0x315
#define RFLM_GSM_TXC_EP_OVR_ENABLE                     0x316
#define RFLM_GSM_TXC_PP_OVR_ENABLE                     0x317
#define RFLM_GSM_PA_SCALE_FACTOR                       0x318

/* Slot mode control const*/
#define RFLM_GSM_SLOTMODECTRL_REG1_ACTIVESLOT_SHIFT    8
#define RFLM_GSM_SLOTMODECTRL_REG1_MODULATION_SHIFT    4



/*=============================================================================

                         RFLM GSM DM ERROR HANDLING

==============================================================================*/
/*! @brief This macro is an RFLM GSM version of assert                        */
/*! @details This is defined to allow for custom message with increased detail*/
#define RFLM_GSM_ERR_HANDLER(expr, msg, ...) \
                         if(!(expr)) \
                         { \
                           rflm_gsm_dm_error_handling(); \
                           RFLM_DIAG_ERR_FATAL(msg, ##__VA_ARGS__); \
                         }



/*============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*/

typedef enum
{
  RFLM_GSM_CCS_TASK_Q_A,
  RFLM_GSM_CCS_TASK_Q_B,
  RFLM_GSM_CCS_MAX_TASK_Q
}rflm_gsm_ccs_task_q;

typedef enum
{
  RFLM_GSM_CCS_DEFAULT_EVENT_TYPE,
  RFLM_GSM_CCS_ISSUE_SEQ,
  RFLM_GSM_CCS_NON_ISSUE_SEQ,
  RFLM_GSM_CCS_MAX_EVENT_TYPE
}rflm_gsm_ccs_event_type;

/*============================================================================*/

typedef enum
{
  RFLM_GSM_DM_ERR_NONE,
  RFLM_GSM_DM_DATA_TBL_TOO_LARGE,
  RFLM_GSM_DM_ALREADY_ALLOCATED,
  RFLM_GSM_DM_DEALLOCATING_NULL_TBL,
  RFLM_GSM_DM_DEALLOCATE_FAILED,
  RFLM_GSM_DM_MAX_ERR
}rflm_gsm_dm_err_type;

/*============================================================================*/

enum
{
  RFLM_GSM_SYNC_RX_EVENT = 0,
  RFLM_GSM_SCH_OR_ACQ_RX_EVENT,
  RFLM_GSM_PCH_RX_EVENT,
  RFLM_GSM_NUM_RX_EVENTS
};

/*============================================================================*/

typedef enum
{
  RFLM_GSM_BAND_GSM850,
  RFLM_GSM_BAND_GSM900,
  RFLM_GSM_BAND_GSM1800,
  RFLM_GSM_BAND_GSM1900,
  RFLM_GSM_NUM_GSM_BANDS
}rflm_gsm_band_type;

/*============================================================================*/

/* Generic MSM Reg Event typedef */
typedef struct
{
   uint16 reg; /* Also referred to as MSM reg tag ID */
   uint16 data_h;
   uint16 data_l;
} rflm_gsm_msm_reg_script_unit;



/*=============================================================================

                     CCS EVENT DEFINITIONS AND TYPES

==============================================================================*/

/* Generic CCS Event typedef */
#define RFGSM_CCS_EVENT_TYPE( event_type, task_num, payload_size ) \
   typedef struct \
   { \
    rflm_ccs_rf_event_description_buffer_header_type header; \
    ccs_rf_task_buf_item_t task[task_num]; \
    uint32 payload[payload_size]; \
   }event_type

#define RFGSM_MSM_REG_EVENT_TYPEDEF( MsmRegEventType, maxTransactions ) \
   typedef struct tag_##MsmRegEventType \
   { \
    int number_transactions; \
    rflm_gsm_msm_reg_script_unit transaction[maxTransactions]; \
   } MsmRegEventType

#define MAX_TRANS(a,b) ( ( (a) >= (b) ) ? (a) : (b)) 

/*============================================================================*/

/* Non-SBI event payload size in 32-bit words. Here we assume that all
   transactions are RFFE extended, i.e. transaction size is 2 32-bit words */
#define RFGSM_NON_SSBI_PAYLOAD_SIZE_32(num_trans)  (2*num_trans)

/* SBI event payload size in 32-bit words. Here we assume that all transactions
   are SSBI write (no SSBI access), i.e transaction size is 1 16-bit word. Each
   task transaction pointer must be 32-bit aligned, therefore we need to add numTasks */
#define RFGSM_SSBI_PAYLOAD_SIZE_32(num_trans, num_tasks)  (num_trans + num_tasks + 1)/2
          
/*============================================================================*/

#define RFGSM_MAX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE 20
#define RFGSM_MAX_TX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE 18
#define RFGSM_MAX_RX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE 10

/* Enter Mode, Wakeup */
#define RFGSM_INIT_MAX_TASKS_TASKQ_A 10
#define RFGSM_INIT_MAX_TASKS_TASKQ_B 1
#define RFGSM_INIT_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_INIT_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 265
#define RFGSM_INIT_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_INIT_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 0
#define RFGSM_INIT_TASK_A_PAYLOAD_SIZE \
        MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_INIT_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_INIT_MAX_TASKS_TASKQ_A)), \
        (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_INIT_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_INIT_MAX_TASKS_TASKQ_A)) )

#define RFGSM_INIT_TASK_B_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_INIT_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B)

#ifndef FEATURE_TX_SHARING
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_init_task_q_a, RFGSM_INIT_MAX_TASKS_TASKQ_A, RFGSM_INIT_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_init_task_q_b, RFGSM_INIT_MAX_TASKS_TASKQ_B, RFGSM_INIT_TASK_B_PAYLOAD_SIZE);
#else
/* RX enter mode */
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_rx_init_task_q_a, RFGSM_INIT_MAX_TASKS_TASKQ_A, RFGSM_INIT_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_rx_init_task_q_b, RFGSM_INIT_MAX_TASKS_TASKQ_B, RFGSM_INIT_TASK_B_PAYLOAD_SIZE);

/* TX enter mode */
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_init_task_q_a, RFGSM_INIT_MAX_TASKS_TASKQ_A, RFGSM_INIT_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_init_task_q_b, RFGSM_INIT_MAX_TASKS_TASKQ_B, RFGSM_INIT_TASK_B_PAYLOAD_SIZE);

#endif
/* Tx Band */
#define RFGSM_TX_BAND_MAX_TASKS_TASKQ_A 10
#define RFGSM_TX_BAND_MAX_TASKS_TASKQ_B 5
#define RFGSM_TX_BAND_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_TX_BAND_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 95
#define RFGSM_TX_BAND_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_TX_BAND_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 0
#define RFGSM_TX_BAND_TASK_A_PAYLOAD_SIZE \
        MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_BAND_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_TX_BAND_MAX_TASKS_TASKQ_A)), \
                    (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_TX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_BAND_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_TX_BAND_MAX_TASKS_TASKQ_A)) )

#define RFGSM_TX_BAND_TASK_B_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_BAND_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B)
				  
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_band_task_q_a, RFGSM_TX_BAND_MAX_TASKS_TASKQ_A, RFGSM_TX_BAND_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_band_task_q_b, RFGSM_TX_BAND_MAX_TASKS_TASKQ_B, RFGSM_TX_BAND_TASK_B_PAYLOAD_SIZE);

#ifdef FEATURE_TX_SHARING
/* TX enable */
/* Taskq A will not be used for TX enable as there are no WTR writes for Tx enable */
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_enable_task_q_a, RFGSM_TX_BAND_MAX_TASKS_TASKQ_A, RFGSM_TX_BAND_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_enable_task_q_b, RFGSM_TX_BAND_MAX_TASKS_TASKQ_B, RFGSM_TX_BAND_TASK_B_PAYLOAD_SIZE);

/* TX disable */
/* Taskq A will not be used for TX disable as there are no WTR writes for Tx disable */
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_disable_task_q_a, RFGSM_TX_BAND_MAX_TASKS_TASKQ_A, RFGSM_TX_BAND_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_disable_task_q_b, RFGSM_TX_BAND_MAX_TASKS_TASKQ_B, RFGSM_TX_BAND_TASK_B_PAYLOAD_SIZE);

#endif

/* Therm */
#define RFGSM_THERM_MAX_TASKS_TASKQ_A 7
#define RFGSM_THERM_MAX_TASKS_TASKQ_B 1
#define RFGSM_THERM_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_THERM_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 30
#define RFGSM_THERM_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_THERM_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 0
#define RFGSM_THERM_TASK_A_PAYLOAD_SIZE \
        MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_THERM_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_THERM_MAX_TASKS_TASKQ_A)), \
                    (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(0) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_THERM_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_THERM_MAX_TASKS_TASKQ_A)) )



#define RFGSM_THERM_TASK_B_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_THERM_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B)
				  
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_therm_task_q_a, RFGSM_THERM_MAX_TASKS_TASKQ_A, RFGSM_THERM_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_therm_task_q_b, RFGSM_THERM_MAX_TASKS_TASKQ_B, RFGSM_THERM_TASK_B_PAYLOAD_SIZE);

/* Rx and Tx Frontend config for AOL tuner, ASD, Coupler*/
#define RFGSM_FRONTEND_CONFIG_MAX_TASKS_TASKQ_A 1
#define RFGSM_FRONTEND_CONFIG_MAX_TASKS_TASKQ_B 5
#define RFGSM_FRONTEND_CONFIG_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_FRONTEND_CONFIG_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 0
#define RFGSM_FRONTEND_CONFIG_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_FRONTEND_CONFIG_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 24
#define RFGSM_FRONTEND_CONFIG_TASK_A_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_FRONTEND_CONFIG_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + \
                  RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_FRONTEND_CONFIG_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_FRONTEND_CONFIG_MAX_TASKS_TASKQ_A)
#define RFGSM_FRONTEND_CONFIG_TASK_B_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_FRONTEND_CONFIG_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B)
				  
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_rx_frontend_config_task_q_a, RFGSM_FRONTEND_CONFIG_MAX_TASKS_TASKQ_A, RFGSM_FRONTEND_CONFIG_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_rx_frontend_config_task_q_b, RFGSM_FRONTEND_CONFIG_MAX_TASKS_TASKQ_B, RFGSM_FRONTEND_CONFIG_TASK_B_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_frontend_config_task_q_a, RFGSM_FRONTEND_CONFIG_MAX_TASKS_TASKQ_A, RFGSM_FRONTEND_CONFIG_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_frontend_config_task_q_b, RFGSM_FRONTEND_CONFIG_MAX_TASKS_TASKQ_B, RFGSM_FRONTEND_CONFIG_TASK_B_PAYLOAD_SIZE);

/* Tx Cal */
#define RFGSM_TX_CAL_MAX_TASKS_TASKQ_A 10
#define RFGSM_TX_CAL_MAX_TASKS_TASKQ_B 10
/* Legacy support:WTR1625*/
#define RFGSM_TX_CAL_MAX_SSBI_TRANSACTIONS_TASKQ_A     50
/*WTR3925 rffes + PAPM*/
#define RFGSM_TX_CAL_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 60
#define RFGSM_TX_CAL_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_TX_CAL_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 28
#define RFGSM_TX_CAL_TASK_A_PAYLOAD_SIZE \
 MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_CAL_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_TX_CAL_MAX_TASKS_TASKQ_A)), \
                    (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_TX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_CAL_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_TX_CAL_MAX_TASKS_TASKQ_A)) )


#define RFGSM_TX_CAL_TASK_B_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_CAL_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B)
				  
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_cal_task_q_a, RFGSM_TX_CAL_MAX_TASKS_TASKQ_A, RFGSM_TX_CAL_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_cal_task_q_b, RFGSM_TX_CAL_MAX_TASKS_TASKQ_B, RFGSM_TX_CAL_TASK_B_PAYLOAD_SIZE);

#define RFGSM_MAX_TX_CAL_MSM_REG_TRANSACTIONS  15 /* Max MSM reg transactions in one calibration step */
RFGSM_MSM_REG_EVENT_TYPEDEF( rflm_gsm_tx_cal_msm_reg_event, RFGSM_MAX_TX_CAL_MSM_REG_TRANSACTIONS );

/* Rx Start */
#define RFGSM_RX_START_MAX_TASKS_TASKQ_A 3   /*There are only 2 tasks for RX start on wtr3925*/
#define RFGSM_RX_START_MAX_TASKS_TASKQ_B 7
#define RFGSM_RX_START_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_RX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 38
#define RFGSM_RX_START_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_RX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 18
#define RFGSM_RX_START_TASK_A_PAYLOAD_SIZE \
       MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_RX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_RX_START_MAX_TASKS_TASKQ_A)), \
                    (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_RX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_RX_START_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_RX_START_MAX_TASKS_TASKQ_A)) )


#define RFGSM_RX_START_TASK_B_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_RX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B)
				  
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_rx_start_task_q_a, RFGSM_RX_START_MAX_TASKS_TASKQ_A, RFGSM_RX_START_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_rx_start_task_q_b, RFGSM_RX_START_MAX_TASKS_TASKQ_B, RFGSM_RX_START_TASK_B_PAYLOAD_SIZE);

/* Rx Stop*/
#define RFGSM_RX_STOP_MAX_TASKS_TASKQ_A 3
#define RFGSM_RX_STOP_MAX_TASKS_TASKQ_B 9
#define RFGSM_RX_STOP_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_RX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 10
#define RFGSM_RX_STOP_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_RX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 21
#define RFGSM_RX_STOP_TASK_A_PAYLOAD_SIZE \
        MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_RX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_RX_STOP_MAX_TASKS_TASKQ_A)), \
                    (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_RX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_RX_STOP_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_RX_STOP_MAX_TASKS_TASKQ_A)) )



#define RFGSM_RX_STOP_TASK_B_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_RX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B)
				  
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_rx_stop_task_q_a, RFGSM_RX_STOP_MAX_TASKS_TASKQ_A, RFGSM_RX_STOP_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_rx_stop_task_q_b, RFGSM_RX_STOP_MAX_TASKS_TASKQ_B, RFGSM_RX_STOP_TASK_B_PAYLOAD_SIZE);

/* Jammer Detector Read Script */
#define RFGSM_JDET_READ_MAX_TASKS_TASKQ_A 1
#define RFGSM_JDET_READ_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_JDET_READ_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 2
#define RFGSM_JDET_READ_TASK_A_PAYLOAD_SIZE  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_JDET_READ_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A)

RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_jdet_read_task_q_a, RFGSM_JDET_READ_MAX_TASKS_TASKQ_A, RFGSM_JDET_READ_TASK_A_PAYLOAD_SIZE);

/* Tx Start */
#define RFGSM_TX_START_MAX_TASKS_TASKQ_A 5
#define RFGSM_TX_START_MAX_TASKS_TASKQ_B 14
#define RFGSM_TX_START_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_TX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 60
#define RFGSM_TX_START_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_TX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 33
#define RFGSM_TX_START_TASK_A_PAYLOAD_SIZE \
        MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_TX_START_MAX_TASKS_TASKQ_A)), \
                    (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_TX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_START_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_TX_START_MAX_TASKS_TASKQ_A)) )


#define RFGSM_TX_START_TASK_B_PAYLOAD_SIZE \
        MAX_TRANS( RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B), RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_TX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE ))


RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_start_task_q_a, RFGSM_TX_START_MAX_TASKS_TASKQ_A, RFGSM_TX_START_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_start_task_q_b, RFGSM_TX_START_MAX_TASKS_TASKQ_B, RFGSM_TX_START_TASK_B_PAYLOAD_SIZE);

/* Tx Trans */
#define RFGSM_TX_TRANS_MAX_TASKS_TASKQ_A 5
#define RFGSM_TX_TRANS_MAX_TASKS_TASKQ_B 6
#define RFGSM_TX_TRANS_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_TX_TRANS_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 8
#define RFGSM_TX_TRANS_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_TX_TRANS_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 9
#define RFGSM_TX_TRANS_TASK_A_PAYLOAD_SIZE \
        MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_TRANS_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_TX_TRANS_MAX_TASKS_TASKQ_A)), \
                    (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_TRANS_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_TRANS_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_TX_TRANS_MAX_TASKS_TASKQ_A)) )


#define RFGSM_TX_TRANS_TASK_B_PAYLOAD_SIZE \
        MAX_TRANS( RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_TRANS_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B) , RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_TRANS_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B))


RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_trans_task_q_a, RFGSM_TX_TRANS_MAX_TASKS_TASKQ_A, RFGSM_TX_TRANS_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_trans_task_q_b, RFGSM_TX_TRANS_MAX_TASKS_TASKQ_B, RFGSM_TX_TRANS_TASK_B_PAYLOAD_SIZE);

/*IP2 cal */

#define RFGSM_IP2_CAL_STEP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 5
#define RFGSM_IP2_CAL_CLEANUP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 10
#define RFGSM_IP2_CAL_SETUP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 100

#define RFGSM_IP2_CAL_SETUP_MAX_TASKS_TASKQ_A 10
#define RFGSM_IP2_CAL_STEP_MAX_TASKS_TASKQ_A 3
#define RFGSM_IP2_CAL_CLEANUP_MAX_TASKS_TASKQ_A 5

#define RFGSM_IP2_CAL_SETUP_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_IP2_CAL_SETUP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) 

#define RFGSM_IP2_CAL_STEP_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_IP2_CAL_STEP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) 

#define RFGSM_IP2_CAL_CLEANUP_PAYLOAD_SIZE \
                  RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_IP2_CAL_CLEANUP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) 

RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_ip2_cal_setup_task_q_a, RFGSM_IP2_CAL_SETUP_MAX_TASKS_TASKQ_A, RFGSM_IP2_CAL_SETUP_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_ip2_cal_step_task_q_a, RFGSM_IP2_CAL_STEP_MAX_TASKS_TASKQ_A, RFGSM_IP2_CAL_STEP_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_ip2_cal_cleanup_task_q_a, RFGSM_IP2_CAL_CLEANUP_MAX_TASKS_TASKQ_A, RFGSM_IP2_CAL_CLEANUP_PAYLOAD_SIZE);

/* Tx Stop */
#define RFGSM_TX_STOP_MAX_TASKS_TASKQ_A 5
#define RFGSM_TX_STOP_MAX_TASKS_TASKQ_B 10
#define RFGSM_TX_STOP_MAX_SSBI_TRANSACTIONS_TASKQ_A     0
#define RFGSM_TX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A 10
#define RFGSM_TX_STOP_MAX_SSBI_TRANSACTIONS_TASKQ_B     0
#define RFGSM_TX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 10
#define RFGSM_TX_STOP_TASK_A_PAYLOAD_SIZE \
        MAX_TRANS( (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_A) + RFGSM_SSBI_PAYLOAD_SIZE_32(0, RFGSM_TX_STOP_MAX_TASKS_TASKQ_A)), \
                    (RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_TX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE) + RFGSM_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_STOP_MAX_SSBI_TRANSACTIONS_TASKQ_A, RFGSM_TX_STOP_MAX_TASKS_TASKQ_A)) )



#define RFGSM_TX_STOP_TASK_B_PAYLOAD_SIZE \
        MAX_TRANS( RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_TX_STOP_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B), RFGSM_NON_SSBI_PAYLOAD_SIZE_32(RFGSM_MAX_TX_NON_SSBI_ON_SSBI_SUPPORTED_DEVICE)  )

RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_stop_task_q_a, RFGSM_TX_STOP_MAX_TASKS_TASKQ_A, RFGSM_TX_STOP_TASK_A_PAYLOAD_SIZE);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_stop_task_q_b, RFGSM_TX_STOP_MAX_TASKS_TASKQ_B, RFGSM_TX_STOP_TASK_B_PAYLOAD_SIZE);

/* Tx Combined Start */
#define RFGSM_TX_COMBINED_START_MAX_TASKS_TASKQ_A (RFGSM_TX_START_MAX_TASKS_TASKQ_A + RFGSM_TX_TRANS_MAX_TASKS_TASKQ_A)
#define RFGSM_TX_COMBINED_START_MAX_TASKS_TASKQ_B (RFGSM_TX_START_MAX_TASKS_TASKQ_B + RFGSM_TX_TRANS_MAX_TASKS_TASKQ_B)
#define RFGSM_TX_COMBINED_START_PAYLOAD_NOT_REQUIRED 1
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_combined_start_task_q_a, RFGSM_TX_COMBINED_START_MAX_TASKS_TASKQ_A, RFGSM_TX_COMBINED_START_PAYLOAD_NOT_REQUIRED);
RFGSM_CCS_EVENT_TYPE(rflm_gsm_ccs_tx_combined_start_task_q_b, RFGSM_TX_COMBINED_START_MAX_TASKS_TASKQ_B, RFGSM_TX_COMBINED_START_PAYLOAD_NOT_REQUIRED);

/* IRAT Trigger script */
#define RFGSM_MEAS_TRIGGER_MAX_SSBI_TRANSACTIONS_TASKQ_A 300
#define RFGSM_MEAS_TRIGGER_MAX_RFFE_TRANSACTIONS_TASKQ_A 25
#define RFGSM_MEAS_TRIGGER_MAX_GRFC_TRANSACTIONS_TASKQ_A 0

/*=============================================================================

                     RFLM GSM DM DEFINITIONS AND TYPES
             How RF GSM decide to allocate the 40k RFLM DM memory
==============================================================================*/
#ifndef FEATURE_TX_SHARING
/* Enter mode and Wakeup */
typedef struct
{
  rflm_gsm_ccs_init_task_q_a task_q_a;
  rflm_gsm_ccs_init_task_q_b task_q_b;
}rflm_gsm_init_data;

#else
/* RX Enter mode and Wakeup */
typedef struct
{
  rflm_gsm_ccs_rx_init_task_q_a task_q_a;
  rflm_gsm_ccs_rx_init_task_q_b task_q_b;
}rflm_gsm_rx_init_data;

/* TX Enter mode and Wakeup */
typedef struct
{
  rflm_gsm_ccs_tx_init_task_q_a task_q_a;
  rflm_gsm_ccs_tx_init_task_q_b task_q_b;
}rflm_gsm_tx_init_data;

/* TX Enable */
typedef struct
{
  rflm_gsm_ccs_tx_enable_task_q_a task_q_a;
  rflm_gsm_ccs_tx_enable_task_q_b task_q_b;
}rflm_gsm_tx_enable_data;

/* TX Enable */
typedef struct
{
  rflm_gsm_ccs_tx_disable_task_q_a task_q_a;
  rflm_gsm_ccs_tx_disable_task_q_b task_q_b;
}rflm_gsm_tx_disable_data;

#endif

/* Tx band */
typedef struct
{
  rflm_gsm_ccs_tx_band_task_q_a task_q_a;
  rflm_gsm_ccs_tx_band_task_q_b task_q_b;
  int16 pa_up_ramp_delta;
  int16 pa_down_ramp_delta;
}rflm_gsm_tx_band_data;

/* Therm read */
typedef struct
{
  rflm_gsm_ccs_therm_task_q_a task_q_a;
  rflm_gsm_ccs_therm_task_q_b task_q_b;
}rflm_gsm_therm_read_data;

/* Rx Frontend config */
typedef struct
{
  rflm_gsm_ccs_rx_frontend_config_task_q_a task_q_a;
  rflm_gsm_ccs_rx_frontend_config_task_q_b task_q_b;
}rflm_gsm_rx_frontend_config_data;

/* Tx Frontend config */
typedef struct
{
  rflm_gsm_ccs_tx_frontend_config_task_q_a task_q_a;
  rflm_gsm_ccs_tx_frontend_config_task_q_b task_q_b;
}rflm_gsm_tx_frontend_config_data;

/* Tx Cal Step */
typedef struct
{
  rflm_gsm_ccs_tx_cal_task_q_a task_q_a[RFLM_GSM_MAX_CAL_STEPS_PER_FRAME];
  rflm_gsm_ccs_tx_cal_task_q_b task_q_b[RFLM_GSM_MAX_CAL_STEPS_PER_FRAME];
  rflm_gsm_tx_cal_msm_reg_event msmRegTxCal[RFLM_GSM_MAX_CAL_STEPS_PER_FRAME];
}rflm_gsm_tx_cal_data;

/* Rx start */
typedef struct
{
  rflm_gsm_ccs_rx_start_task_q_a task_q_a;
  rflm_gsm_ccs_rx_start_task_q_b task_q_b;
}rflm_gsm_rx_start_data;

/* Rx stop */
typedef struct
{
  rflm_gsm_ccs_rx_stop_task_q_a task_q_a;
  rflm_gsm_ccs_rx_stop_task_q_b task_q_b;
}rflm_gsm_rx_stop_data;

/* Rx */
typedef struct
{
  rflm_gsm_rx_frontend_config_data rx_frontend_config;
  rflm_gsm_rx_start_data rx_start;  
  rflm_gsm_rx_stop_data  rx_stop;
  uint32  rf_sequence_number;
  uint16  rf_gain_prx;
  uint16  rf_gain_drx;
  uint32  rsb_coefA_prx;
  uint32  rsb_coefB_prx;
  uint32  rsb_coefA_drx;
  uint32  rsb_coefB_drx;
  boolean rflm_enh_rx_on_flag;
  int32 rflm_enh_rx_offset_freq;
}rflm_gsm_rx_data;


/* Jammer Detector data */
#define RFLM_JDET_READ_BUFF_SIZE 4
#define RFLM_JDET_NUM_MON_EVENTS 3

#define RFLM_JDET_READ_BUFF_INCREMENT_CHECK(i) (i >= RFLM_JDET_READ_BUFF_SIZE ? 0 : i)
#define RFLM_JDET_READ_MON_INDEX(i) (i % RFLM_JDET_NUM_MON_EVENTS)

typedef struct
{
  void* event_info_ptr;
  rflm_gsm_subscription_id sub_id;
  uint32* payload_ptr;
  uint8   reading_idx;
  uint32  rflm_intf_idx;
  rflm_gsm_fw_cb_type fw_callback;  //Callback to send the reading to GFW
  void* fw_jdet_cb_arg;             //Pointer containing GFW context info
}jdet_read_callback_args_type;

/* Jammer Detector Read Script */
typedef struct
{
  rflm_gsm_ccs_jdet_read_task_q_a task_q_a;
  jdet_read_callback_args_type    cb_params;
}rflm_gsm_jdet_read_script_data;


/*============================================================================*/
/*! @struct rflm_gsm_jdet_read_data_type                                      */
/*! @brief Stores data for Jammer Detector read scripts                       */
/*! @details Seperate buffers are used for normal Rx activity and for Monitor */
/*!          bursts, due to the way the registers are accessed. Monitors are  */
/*!          triple buffered as they occur multiple times in a frame. This    */
/*!          struct also holds a boolean flag to holdoff JDET reads whilst a  */
/*!          Therm read is in progress.                                       */
typedef struct
{
  rflm_gsm_jdet_read_script_data  rx_jdet_read_script[RFLM_MAX_JDET_READS_PER_RX];
  rflm_gsm_jdet_read_script_data  mon_jdet_read_script[RFLM_JDET_NUM_MON_EVENTS];
  boolean                         jdet_therm_read_holdoff;
}rflm_gsm_jdet_read_data_type;

/* Tx start */
typedef struct
{
  rflm_gsm_ccs_tx_start_task_q_a task_q_a;
  rflm_gsm_ccs_tx_start_task_q_b task_q_b;
}rflm_gsm_tx_start_data;

/* Tx trans */
typedef struct
{
  rflm_gsm_ccs_tx_trans_task_q_a task_q_a;
  rflm_gsm_ccs_tx_trans_task_q_b task_q_b;
}rflm_gsm_tx_trans_data;

/* Tx stop */
typedef struct
{
  rflm_gsm_ccs_tx_stop_task_q_a task_q_a;
  rflm_gsm_ccs_tx_stop_task_q_b task_q_b;
}rflm_gsm_tx_stop_data;

/* Tx combined start */
typedef struct
{
  rflm_gsm_ccs_tx_combined_start_task_q_a task_q_a;
  rflm_gsm_ccs_tx_combined_start_task_q_b task_q_b;
}rflm_gsm_tx_combined_start_data;

/* Tx */
typedef struct
{
  rflm_gsm_tx_frontend_config_data tx_frontend_config;
  rflm_gsm_tx_start_data tx_start;  
  rflm_gsm_tx_trans_data tx_trans[RFLM_GSM_MAX_TX_UL_SLOTS];
  rflm_gsm_tx_stop_data  tx_stop;
  rflm_gsm_tx_combined_start_data tx_combined_start;
  uint32 rf_sequence_number;
  uint16 *amam_table;
  uint16 *ampm_table;
  boolean program_amam_ampm_tables;
  boolean predist_enabled[RFLM_GSM_MAX_TX_UL_SLOTS];
  uint16 *pa_ramp_up_profile[RFLM_GSM_MAX_TX_UL_SLOTS];
  uint16 *pa_ramp_down_profile[RFLM_GSM_MAX_TX_UL_SLOTS];
  int32  tx_pwr_level_dbm10[RFLM_GSM_MAX_TX_UL_SLOTS];
  int16  pa_up_ramp_delta;
  int16  pa_down_ramp_delta;
  int64  path_delay_val;
  uint16 env_gain[RFLM_GSM_MAX_TX_UL_SLOTS];
}rflm_gsm_tx_data;
/* IP2 cal*/

typedef struct
{
 rflm_gsm_ccs_ip2_cal_setup_task_q_a task_q_a;
 uint16 ip2_cal_alpha;
//uint16 *amam_table;
//uint16 *ampm_table;
//boolean program_amam_ampm_tables;
//boolean predist_enabled;
//uint16 *pa_ramp_up_profile;
//uint16 *pa_ramp_down_profile;
//int16  pa_up_ramp_delta;
//int16  pa_down_ramp_delta;
//int16  tx_path_delay_value;
//uint16 env_gain;
}
rflm_gsm_ip2_cal_setup_data;

typedef struct
{
 rflm_gsm_ccs_ip2_cal_step_task_q_a task_q_a;
}rflm_gsm_ip2_cal_step_data;

/* Data structure containing information about the overlapping grid
 * of two successive IP2 cal iterations 
 */
typedef struct
{
  /* boolean indicating averaging is on/off*/
  boolean do_average;
  /* Pointer to an array containing cal step 
   * indices of overlapping area of current 
   * iteration
   */
  uint8 *curr_iter_overlap_arr;
  /* Pointer to an array containing cal step 
   * indices of overlapping area of previous 
   * iteration
   */
  uint8 *prev_iter_overlap_arr;
  /* Number of overlappinf points between 
   * current and previous IP2 cal iterations
   */
  uint8 num_overlap_points;
  /* Index pointing to the next cal step index
   * in the traversal sequence for the current 
   * iteration
   */
  uint8 overlap_index;
}rflm_gsm_ip2_cal_grid_info_type;

typedef struct
{
  uint16 dc_settling_time_qs;
  uint16 step_duration_qs;
  int32 min_im2_value;
  uint8 min_im2_step_idx;
  uint8 max_num_cal_steps; // Max number of cal steps supported
  boolean final_step_done;
  rflm_gsm_ip2_cal_grid_info_type ip2_cal_grid_info;
  rflm_gsm_ip2_cal_step_data ip2_cal_step[RFLM_MAX_IP2_CAL_STEPS];
}
rflm_gsm_ip2_cal_meas_data;


typedef struct
{
 rflm_gsm_ccs_ip2_cal_cleanup_task_q_a task_q_a;
}
rflm_gsm_ip2_cal_cleanup_data;



typedef struct
{
  rflm_gsm_ip2_cal_setup_data ip2_cal_setup;
  rflm_gsm_ip2_cal_meas_data ip2_cal_meas;
  rflm_gsm_ip2_cal_cleanup_data ip2_cal_cleanup;
#ifdef FEATURE_ADV_PROF
  rflm_time_profile_meas_s ip2_cal_prof_data;
#else
  uint32 ip2_cal_prof_data;
#endif
  uint8 ip2_cal_tag; // Stores the unique number to associate variables with IP2 cal events in an idel frame
  boolean ip2_cal_supported;
  boolean first_iter;
}
rflm_gsm_ip2_cal_data;



/*===========================================================================*/
/* Number of non standalone (i.e. issued together with Rx and Tx) RSSI       */
/* measurements per TDMA frame. Note that this definition is specific to the */
/* rflm_gsm_burst_data structure definition below.                           */
#define RFLM_GSM_MAX_NON_STANDALONE_RSSI_MEASUREMENTS 7

/*===========================================================================*/

typedef union
{
   rflm_gsm_rx_data monitor[RFLM_GSM_MAX_RSSI_MEASUREMENTS];
   struct
   {
    rflm_gsm_rx_data dummy_monitor[RFLM_GSM_MAX_NON_STANDALONE_RSSI_MEASUREMENTS]; //DO NOT USE
    rflm_gsm_rx_data rx[RFLM_GSM_NUM_RX_EVENTS];
    rflm_gsm_tx_data tx;
   };
}rflm_gsm_burst_data;

/*============================================================================*/
/*TxFE Dynamic LMEM Data                                                      */
typedef struct
{
  uint16 pa_ramp_up_table[RFLM_GSM_MAX_NUM_RAMP_SAMPLES];
  uint16 pa_ramp_down_table[RFLM_GSM_MAX_NUM_RAMP_SAMPLES];
}rflm_gsm_pa_ramp_type;

typedef struct
{
  rflm_gsm_pa_ramp_type pa_slot_ramps[RFLM_GSM_MAX_TX_UL_SLOTS];
}rflm_gsm_pa_frame_ramps_type;

typedef struct
{
  uint16 amam_table[RFLM_GSM_MAX_NUM_AMAM_SAMPLES];
  uint16 ampm_table[RFLM_GSM_MAX_NUM_AMPM_SAMPLES];
}rflm_gsm_amam_ampm_table_type;

typedef struct
{
  /* sub id for FTM mode*/
  rflm_gsm_subscription_id ftm_sub_id;
  /* Slot mode control data*/ 
  boolean use_slot_mode_ctrl_flag; 
  uint32  slot_mode_ctrl_1_reg_addr;
  uint32  slot_mode_ctrl_2_reg_addr;
  
 /* RF Device this subscription is operating on */
  uint8 rfm_dev;

  /* TQ pair subscription is operating on */
  uint8 tq_pair;

  /* Table Vars */
  uint16 rx_alpha;
  uint16 idle_mode_rx_alpha;
  uint16 tx_alpha;
  uint16 tx_alpha_rflm_overhead;  /*This is the total RFLM processing overhead*/  
  uint16 num_tx_kv_cal_bits;
  uint64 tx_kv_cal_data[3];

  /* Frame data */
  rflm_gsm_tx_slot_params frame_data_tx[RLFM_GSM_MAX_FRAME_BUFFERS][RFLM_GSM_MAX_TX_SLOT_PARAM_IDX]; //Includes KV

  /* FW registered cb */
  rflm_gsm_fw_cb_type fw_callback;
  void* fw_cb_arg;

  /* TX_STOP priority in the HMSC case */
  uint8 tx_stop_hmsc_prio;
}rflm_gsm_vars_type;

typedef struct
{
  rflm_gsm_amam_ampm_table_type amam_ampm_tables[RFLM_GSM_DOUBLE_BUFFER];
  rflm_gsm_pa_frame_ramps_type  pa_frame_ramps[RFLM_GSM_DOUBLE_BUFFER];
  rflm_gsm_pa_ramp_type         pa_master_ramps[RFLM_GSM_NUM_GSM_BANDS];
}rflm_gsm_txfe_dynamic_type;

typedef struct
{
#ifdef FEATURE_ADV_PROF
  rflm_time_profile_meas_s init_prof_data;
  rflm_time_profile_meas_s tx_band_prof_data;
  rflm_time_profile_meas_s rx_prof_data;
  rflm_time_profile_meas_s tx_prof_data;
  rflm_time_profile_meas_s therm_read_prof_data;
#else
  uint32 init_prof_data;
  uint32 tx_band_prof_data;
  uint32 rx_prof_data;
  uint32 tx_prof_data;
  uint32 therm_read_prof_data;
#endif
}rflm_gsm_profile_data;

/*============================================================================*/

typedef struct
{
#ifndef FEATURE_TX_SHARING
  rflm_gsm_init_data             rfgsm_init_event;
#else
  rflm_gsm_rx_init_data          rfgsm_rx_init_event; /* Added for TX sharing */
  rflm_gsm_tx_init_data          rfgsm_tx_init_event; /* Added for TX sharing */
  rflm_gsm_tx_enable_data        rfgsm_tx_enable_event; /* Added for TX sharing */
  rflm_gsm_tx_disable_data        rfgsm_tx_disable_event; /* Added for TX sharing */
#endif
  rflm_gsm_tx_band_data          rfgsm_tx_band_event;
  rflm_gsm_therm_read_data       rfgsm_therm_read_event;
  union
  {
    rflm_gsm_burst_data            rfgsm_burst_event[RFLM_GSM_TRIPLE_BUFFER];
    rflm_gsm_ip2_cal_data          rfgsm_ip2_cal_event;
  };
  rflm_gsm_txfe_dynamic_type     rfgsm_txfe_data;
  rflm_gsm_vars_type             rflm_gsm_vars;
  rflm_gsm_profile_data          rflm_gsm_profile_data;
  rflm_cmn_tuner_event_data_type *rflm_tuner_intf; 
  /*! Status of tuner AOL. Written only from RFLM */
  rflm_ccs_rf_event_status_type  *tuner_aol_rx_status_ptr;
  rflm_ccs_rf_event_status_type  *tuner_aol_tx_status_ptr;
  /*! Status of tuner CL. Written only from RFLM */
  rflm_cmn_tuner_event_status_type *tuner_cl_status_ptr;
  rflm_gsm_jdet_read_data_type    rfgsm_jdet_read_data;
}rflm_gsm_data_type;



/*============================================================================*/
/* Seperating the Tx Calibration data from the other data because this can
   be destroyed after calibration to save memory */
typedef struct
{
  /* sub id for FTM mode*/
  rflm_gsm_subscription_id ftm_sub_id;

  rflm_gsm_tx_cal_data rfgsm_tx_cal_event[RFLM_GSM_TRIPLE_BUFFER];
  /* RFLM_GSM_MAX_TX_CAL_CMD_DMA_DATA_LENGTH definition lives in rflm_api_gsm.h */
  uint64 dma_data[RFLM_GSM_MAX_TX_CAL_CMD_DMA_DATA_LENGTH];
}rflm_gsm_tx_cal_data_type;



/*=============================================================================

                              INTERFACE DATA

==============================================================================*/
/*! @enum rflm_gsm_script_type                                                */
/*! @brief CCS script for processing                                          */
/*! @details The script ID is used to enable RFLM GSM to locate the correct   */
/*!          prepopulated script in memory RFLM DM                            */
typedef enum
{
  RFLM_GSM_DEFAULT_SCRIPT,
  RFLM_GSM_ENTER_MODE_SCRIPT,
  RFLM_GSM_WAKEUP_SCRIPT,
  RFLM_GSM_TX_BAND_SCRIPT,
  RFLM_GSM_RX_START_SCRIPT,
  RFLM_GSM_RX_STOP_SCRIPT,
  RFLM_GSM_MON_START_SCRIPT,
  RFLM_GSM_MON_STOP_SCRIPT,
  RFLM_GSM_RX_ACQ_START_SCRIPT,
  RFLM_GSM_RX_ACQ_STOP_SCRIPT,
  RFLM_GSM_RX_PCH_START_SCRIPT,
  RFLM_GSM_RX_PCH_STOP_SCRIPT,
  RFLM_GSM_TX_START_SCRIPT,
  RFLM_GSM_TX_TRANS_SCRIPT,
  RFLM_GSM_TX_STOP_SCRIPT,
  RFLM_GSM_TX_CAL_SCRIPT,
  RFLM_GSM_THERM_READ_SCRIPT,
  RFLM_GSM_IP2_CAL_SETUP_SCRIPT,
  RFLM_GSM_IP2_CAL_STEP_SCRIPT,
  RFLM_GSM_IP2_CAL_CLEANUP_SCRIPT,
  RFLM_GSM_RX_FRONTEND_CONFIG_SCRIPT,
  RFLM_GSM_TX_FRONTEND_CONFIG_SCRIPT,
  RFLM_GSM_RX_JDET_READ_SCRIPT,
  RFLM_GSM_MON_JDET_READ_SCRIPT,
#ifdef FEATURE_TX_SHARING
  RFLM_GSM_RX_ENTER_MODE_SCRIPT, 
  RFLM_GSM_TX_ENTER_MODE_SCRIPT,
  RFLM_GSM_RX_WAKEUP_SCRIPT,
  RFLM_GSM_TX_WAKEUP_SCRIPT,
  RFLM_GSM_TX_ENABLE_SCRIPT,
  RFLM_GSM_TX_DISABLE_SCRIPT,
#endif
  RFLM_GSM_MAX_SCRIPT_TYPES
}rflm_gsm_script_type;



/*============================================================================*/
/*! @struct rflm_gsm_intf_data                                                */
/*! @brief Data that is populated by RF for use in RFLM GSM to determine what */
/*!        command was sent from FW and where to locate the required data     */
/*! @details FW allocate a variable of this type which is passed to RF from L1*/
/*!          RF populate the structure with the information related to the cmd*/
/*!          being processed. FW pass the same structure variable to RFLM GSM */
/*!          which is used to locate the data stored in RFLM DM required for  */
/*!          programming the HW.                                              */
typedef struct
{
  uint8                    device;
  rflm_gsm_subscription_id sub_id;
  uint32                   intf_data_idx; //for debug
  rflm_gsm_script_type     script;
  uint8                    buffer_id;
  uint8                    burst_num;
}rflm_gsm_intf_data;



/*============================================================================*/
/*! @struct rflm_gsm_intf                                                     */
/*! @brief Structure that holds FW->RFLM interface data                       */
/*! @details A variable of this struct is allocated and pointer assigned to dm*/
/*!          GSM where it is managed                                          */
typedef struct
{
  uint32 intf_data_idx;
  rflm_gsm_intf_data intf_data[RFLM_GSM_DM_MAX_INTF_DATA_ENTRIES];
}rflm_gsm_intf;



/*============================================================================*/
/*! @struct rflm_gsm_ccs_event_info_type                                      */
/*! @brief Structure that holds pointers to event type per script per tq      */
/*! @details A structure created to allow for static definition in RFSW and   */
/*!          initialisation in RFLM DM for reference when processing event.   */
typedef struct
{
  const rflm_gsm_ccs_event_type * event_settings[RFLM_GSM_MAX_SCRIPT_TYPES][RFLM_GSM_CCS_MAX_TASK_Q];
}rflm_gsm_ccs_event_info_type;



/*=============================================================================

                            FUNCTION PROTOTYPES

==============================================================================*/

void rflm_gsm_create_mutex( qmutex_t *mutex );

/*---------------------------------------------------------------------------*/

void rflm_gsm_delete_mutex( qmutex_t mutex );

/*---------------------------------------------------------------------------*/

void rflm_gsm_lock_mutex( qmutex_t mutex );

/*---------------------------------------------------------------------------*/

void rflm_gsm_unlock_mutex( qmutex_t mutex );

/*---------------------------------------------------------------------------*/

void rflm_gsm_log_intf_data( rflm_gsm_intf_data *rflm_intf_data_ptr );

/*---------------------------------------------------------------------------*/

boolean rflm_gsm_dm_validate_intf_data( rflm_gsm_intf_data *rflm_gsm_data );

/*---------------------------------------------------------------------------*/

rflm_gsm_data_type* rflm_gsm_dm_get_table_ptr( rflm_gsm_subscription_id sub_id );

/*---------------------------------------------------------------------------*/

boolean rflm_gsm_dm_init_intf_buf( rflm_gsm_intf* intf );

/*---------------------------------------------------------------------------*/

rflm_gsm_intf_data* rflm_gsm_dm_get_intf_data( uint32 rflm_gsm_intf_idx );

/*---------------------------------------------------------------------------*/

boolean rflm_gsm_dm_clear_intf_data( void );

/*---------------------------------------------------------------------------*/

rflm_gsm_ccs_event_type rflm_gsm_dm_get_event_type( rflm_gsm_script_type script, rflm_gsm_ccs_task_q tq );

/*---------------------------------------------------------------------------*/

rflm_gsm_dm_err_type rflm_gsm_dm_allocate_tech_buf( rflm_gsm_subscription_id sub_id );

/*---------------------------------------------------------------------------*/

rflm_gsm_dm_err_type rflm_gsm_dm_deallocate_tech_buf( rflm_gsm_subscription_id sub_id );

/*---------------------------------------------------------------------------*/

rflm_gsm_tx_cal_data_type* rflm_gsm_dm_get_tx_cal_table_ptr( uint8 device_id );

/*---------------------------------------------------------------------------*/

boolean rflm_gsm_dm_set_tx_cal_table_ptr( uint8 device_id, rflm_gsm_tx_cal_data_type* table_ptr );

/*---------------------------------------------------------------------------*/

boolean rflm_gsm_dm_init( rflm_gsm_ccs_event_info_type * event_info_map );

/*---------------------------------------------------------------------------*/

boolean rflm_gsm_dm_deinit( void );

/*---------------------------------------------------------------------------*/

boolean rflm_gsm_dm_init_umplt_data( rflm_gsm_subscription_id sub_id );

/*---------------------------------------------------------------------------*/

void rflm_gsm_dm_error_handling( void );

/*---------------------------------------------------------------------------*/

void rflm_gsm_time_profile_start
(
#ifdef FEATURE_ADV_PROF
  rflm_time_profile_meas_s *meas
#else
  uint32 *meas
#endif
);

/*---------------------------------------------------------------------------*/

uint32 rflm_gsm_time_profile_end
(
#ifdef FEATURE_ADV_PROF
  rflm_time_profile_meas_s *meas
#else
  uint32 *meas
#endif
);

#ifdef __cplusplus
} // extern "C"
#endif
/*============================================================================*/
#endif /* RFLM_GSM_DM_H */

