/*!
  @file
  rflm_api_gsm.h

  @brief
  RFLM API definitions for GSM
  
  @detail
  Provides the following API's for GFW to call:
  - Rx and Tx Frame configuration
  - RxLM and TxLM static configuration
  - RxFE and TxFE dynamic configuration
  - Slot Mode configuration
 
  - Programming CCS Events
    - Init
    - Tx Burst
    - Tx Band
    - Rx Burst
    - Therm Read
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_gsm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/15   bp      Add new message type RFLM_GSM_HMSC_MONITOR
01/30/14   sc      Add RF Sequence Number
10/01/14   sc      Reduce overhead on getting table ptr
09/19/14   ch      Introduce RFLM_GSM_PROCESSING_OVERHEAD to fix 144MHz GSM voice call drop
08/28/14   tsr     Introduce v2 api for GFW to retrieve idle mode RX Alpha
08/15/14   ec      Add jdet_read_trigger_offset param
08/07/14   tws     Fix lint error caused by incorrect comment line
07/30/14   cj      Added changes for TX sharing feature
07/30/14   hoh     Add enhanced rx feature support 
07/28/14   ec      Add definitions for Jammer Detector readings
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/09/14   ec      Add callback parameters to Jammer Detector read API type
07/02/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX
07/03/14   hk      TSTS support
06/05/14   ec      Add JDET FW interface
06/04/14   sb      Add sub id capability for rflm_gsm_txc_prog_amxm
06/03/14  hkm      Add sub id capability for txc_frame_config
05/20/14   sc      Start and release variable to device for TQ pair management
05/07/14   sc      Add RFLM GSM state machine
04/16/14   sb      IP2 cal Interface and functionality changes
04/07/14  tsr      Add RFLM RX gain params and spectral inversion coefficients for DRX
04/10/14   sc      Add support for FW cb registration
04/04/14   sb      Add interface changes for IP2 cal + profiling RFLM
03/26/14   sb      Add support for IP2 cal for Dime PM
03/26/14   sc      Add UMPLT initialisation functionality
03/13/14   sc      Add disTx/Rx support
03/13/14   ec      Fix potential overflow bug with Tx frame parameters
03/11/14   sc      Expose RFLM overhead to RFSW to ensure GL1 account for delay
03/11/14   ec      Add stubbed APIs for TxC programming, expand slot and pa_ramp types
02/24/14   sc      Add therm read abort handling
02/17/14   sc      Add profiling and improved logging
02/10/14   sw      Minor comment update
02/06/14   sc      Ensure set message macro does not modify the input value
01/28/14   sc      Add interface masks for Discontinuous Rx, Tx Cancellation
01/22/14   sc      Manage DM memory based on sub/mode/chain instead of device
01/21/14   ggs     Initial changes for TXC DCOC and DPD calibration 
12/03/13   ch      Added rflm_gsm_[get/set]_msg_type message type in upper word of intf_idx 
11/19/13   ggs     Changed Tx cal script vars in
11/08/13   cj      IRAT event processing APIs & structure definitions added
11/05/13   ggs     Move definition of RFLM_GSM_MAX_CAL_STEPS_PER_FRAME to
                   rflm_gsm_dm.h
10/24/13   sc      Get spectral inversion data API
10/24/13   ggs     Tx Calibration support
10/23/13   sc      Add max KV index define
10/10/13   sc      Add TxFE config DMA transfer functionality
10/04/13   sc      TxFE programming functionality
10/03/13   sc      Alloc/Free TQ mem management
10/02/13   hdz     Added GSM API version number
09/26/13   sc      Add FW->RFLM RF->RFLM data interface
09/25/13   sc      Add Rx frame config functionality
09/09/13   sc      Add Tx frame config functionality
09/09/13   sc      Clean up header includes
09/03/13   sc      Remove use of RFSW headers within RFLM
08/28/13   sc      FW data extraction functionality
08/21/13   sc      Clean up types and definitions
05/21/13   sc	   Initial version of RFLM FW API and structure definitions
==============================================================================*/

#ifndef RFLM_API_GSM_H
#define RFLM_API_GSM_H


/*=============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "comdef.h"
#include "rflm_api_cmn.h"



/*=============================================================================

                          DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief Version of the rflm dtr rx module.
*/
#define RFLM_API_GSM_MAJOR_VERSION            2
#define RFLM_API_GSM_MINOR_VERSION            1
#define RFLM_API_GSM_VERSION    \
              ((RFLM_API_GSM_MAJOR_VERSION << 16) | RFLM_API_GSM_MINOR_VERSION )


#define RFLM_GSM_MAX_DL_TS  (5)
#define RFLM_GSM_MAX_UL_TS  (4)

#define RFLM_GSM_MAX_TX_SLOT_PARAM_IDX  RFLM_GSM_MAX_UL_TS + 2 //Plus 1 for KV slot and 1 for Off ramp on TxC targets

#define RFLM_GSM_MAX_DEVICES (9)

#define RFLM_GSM_MAX_KV_DATA (3)

#define RFLM_GSM_MAX_TX_CAL_CMD_DMA_DATA_LENGTH (64)

#define RFLM_GSM_MAX_CAL_STEPS_PER_FRAME     26

/* This overhead is the processing time for an Rx */
/* Derived from observed worst-case Rx profile data at 144MHz of 1417XO/19.2/0.923 = 80qs */
#define RFLM_GSM_RX_MAX_PROC_OVERHEAD_QS 80

/* This overhead is the processing time for an Tx */
#ifdef RFLM_GSM_PROCESSING_OVERHEAD
#define RFLM_GSM_TX_MAX_PROC_OVERHEAD_QS 270
#else
#define RFLM_GSM_TX_MAX_PROC_OVERHEAD_QS 150
#endif

#define RFLM_GSM_MSG_TYPEID 0

#define RFLM_GSM_INTF_BITS  16

#define RFLM_GSM_INTF_MASK ((1<<RFLM_GSM_INTF_BITS)-1)

#define RFLM_GSM_MSG_TYPE_MASK ~RFLM_GSM_INTF_MASK

#define RFLM_GSM_SET_MSG(rflm_gsm_intf_idx,rflm_gsm_msg) (rflm_gsm_intf_idx | (rflm_gsm_msg << RFLM_GSM_INTF_BITS))


#define RFLM_MAX_IP2_CAL_STEPS 81

#define MAX_IP2_CAL_STEPS_IN_CCS_QUEUE 5

#define MAX_IP2_STEP_TASKS_IN_CCS_QUEUE 5  // CCS_RF_TASKQ_NUM_ITEMS

#define RFLM_MAX_JDET_READS_PER_RX  8
#define RFLM_GSM_NUM_TRANS_PER_JDET_READ 2

#define RFLM_JDET_INVALID_READING 0xFF

/* To convert XO to US without a divide             */
/* 1/19.2 << RFLM_GSM_XO_TO_US_SCALE_FACTOR =  3413 */
#define RFLM_GSM_XO_TO_US_SCALE_FACTOR       16
#define RFLM_GSM_XO_TO_US_SCALED           3413


typedef int32 im2_data_type;

typedef enum
{
  RFLM_GSM_DEFAULT_MOD,
  RFLM_GSM_UNKNOWN_MOD,
  RFLM_GSM_MOD_GMSK,
  RFLM_GSM_MOD_8PSK,
  RFLM_GSM_MAX_MODS
}rflm_gsm_mod_type;



typedef enum
{
  RFLM_GSM_ON_RAMP_OFF_RAMP,
  RFLM_GSM_OFF_RAMP,
  RFLM_GSM_ON_RAMP_NO_RAMP,
  RFLM_GSM_NO_RAMP,
  RFLM_GSM_KV_RAMP,
  RFLM_GSM_OFF_RAMP_ON_RAMP,
  RFLM_GSM_MAX_RAMP_TYPES
}rflm_gsm_ramp_type;



typedef enum
{
  RFLM_GSM_DEFAULT_CONFIG,
  RFLM_GSM_CONFIG_FRAME_SET,
  RFLM_GSM_CONFIG_UPDATE,
  RFLM_GSM_CONFIG_CANCEL,
  RFLM_GSM_MAX_CONFIG
}rflm_gsm_slot_config_type;



/*============================================================================*/
/*! @enum rflm_gsm_subscription_id                                            */
/*! @brief Structure that holds information to determine data locations       */
/*! @details In order to manage memory for multi-SIM configurations the       */
/*!          following pieces of information are required                     */
typedef enum
{
  RFLM_GSM_DEFAULT_SUBSCRIPTION,
  RFLM_GSM_SUBSCRIPTION_1=RFLM_GSM_DEFAULT_SUBSCRIPTION,
  RFLM_GSM_SUBSCRIPTION_2,
  RFLM_GSM_SUBSCRIPTION_3,
  RFLM_GSM_MAX_SUBSCRIPTIONS
}rflm_gsm_subscription_id;



typedef enum
{
  RFLM_GSM_DEFAULT_MSG = RFLM_GSM_MSG_TYPEID,
  RFLM_GSM_EARLY_DECODE,
  RFLM_GSM_REUSE_RX,
  RFLM_GSM_REUSE_TX,
  RFLM_GSM_OVERRIDE_QS_TO_USTMR,
  RFLM_GSM_RX_CANCELLATION,
  RFLM_GSM_TX_CANCELLATION,
  RFLM_GSM_THERM_READ_ABORT,
  RFLM_GSM_THERM_READ_TIMEOUT,
  RFLM_GSM_HMSC_MONITOR_DEFAULT,
  RFLM_GSM_HMSC_MONITOR_REUSE_TX,
  RFLM_GSM_MAX_MSG_TYPE
}rflm_gsm_msg_type;



/*===========================================================================*/
/*! @fp rflm_gsm_fw_cb_type                                                  */
/*! @brief fp definition of FW registered function to call every cb          */
/*! @details Call FW every GSM CCS cb with current num pending events        */
/*!          Required for FW to easily understand when RFLM GSM is complete  */
/*! @params void* - input arg pointer regsitered by FW to pass back          */
/*! @params uint32 - current number of pending events                        */
typedef void (*rflm_gsm_fw_cb_type)(void*,uint32);



/*=============================================================================

                  FW->RFLM GET INFO INPUT TYPE DEFINITIONS
  
==============================================================================*/
/*! @struct rflm_gsm_register_cb_input/output_type                            */
/*! @brief  This structure is for RF to register cb for FW                    */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  rflm_gsm_subscription_id sub_id;
  rflm_gsm_fw_cb_type callback;
  void* fw_cb_arg;
}rflm_gsm_register_cb_input_type;



/*============================================================================*/
/*! @struct rflm_gsm_get_enh_rx_params_input_type                                  */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint32 rflm_gsm_intf_idx;
}rflm_gsm_get_enh_rx_params_input_type;



/*============================================================================*/
/*! @struct rflm_gsm_get_enh_rx_params_output_type                            */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  boolean enh_rx_on_flag; // if this flag is false, FW will ignore the freq offset
  int32 enh_rx_offset_freq; // unit in hertz
}rflm_gsm_get_enh_rx_params_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_get_rx_alpha_input_type                                  */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint32 rflm_gsm_intf_idx;
}rflm_gsm_get_rx_alpha_input_type;

/*============================================================================*/
/*! @struct rflm_gsm_get_rx_alpha_output_type                                  */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint16 rx_alpha;
  uint16 idle_mode_rx_alpha;
}rflm_gsm_get_rx_alpha_output_type;




/*============================================================================*/
/*! @struct rflm_gsm_get_tx_alpha_input_type                                  */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint32 rflm_gsm_intf_idx;
}rflm_gsm_get_tx_alpha_input_type;



/*============================================================================*/
/*! @struct rflm_gsm_get_rf_gain_input_type                                   */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint32 rflm_gsm_intf_idx;
}rflm_gsm_get_rf_gain_input_type;

/*============================================================================*/
/*! @struct rflm_gsm_get_rf_gain_input_type                                   */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint16 rf_gain_prx;
  uint16 rf_gain_drx;
}rflm_gsm_get_rf_gain_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_ramp_deltas_input_type                                */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint32 rflm_gsm_intf_idx;
}rflm_gsm_tx_ramp_deltas_input_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_kv_data_input_type                                    */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint32 rflm_gsm_intf_idx;
}rflm_gsm_tx_kv_data_input_type;



/*============================================================================*/
/*! @struct rflm_gsm_spec_inv_data_input_type                                 */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint32 rflm_gsm_intf_idx;
}rflm_gsm_spec_inv_data_input_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_cal_dma_data_input_type                               */
/*! @brief  This structure is for FW to pass data to RFLM GSM                 */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint8 device_id;
}rflm_gsm_tx_cal_dma_data_input_type;



/*=============================================================================

                  FW->RFLM GET INFO OUTPUT TYPE DEFINITIONS
  
==============================================================================*/
/*! @struct rflm_gsm_tx_ramp_deltas_output_type                               */
/*! @brief  This structure is for RF to populate RF data to pass back to FW   */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will populate the variables with the relevant information */
typedef struct
{
  int16 pa_up_ramp_delta;
  int16 pa_down_ramp_delta;
}rflm_gsm_tx_ramp_deltas_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_kv_data_output_type                                   */
/*! @brief  This structure is for RF to populate RF data to pass back to FW   */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will populate the variables with the relevant information */
typedef struct
{
  uint16 num_tx_kv_cal_bits;
  uint64 tx_kv_cal_data[RFLM_GSM_MAX_KV_DATA];
}rflm_gsm_tx_kv_data_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_spec_inv_data_output_type                                */
/*! @brief  This structure is for RF to populate RF data to pass back to FW   */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will populate the variables with the relevant information */
typedef struct
{
  uint32 rsb_coefA_prx;
  uint32 rsb_coefB_prx;
  uint32 rsb_coefA_drx;
  uint32 rsb_coefB_drx;
}rflm_gsm_spec_inv_data_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_cal_dma_data_output_type                              */
/*! @brief  This structure is for RF to return RF data to FW                  */
/*! @details FW create a variable of this type and pass in address as argument*/
/*!          GSM RF will use this information to calc output return values    */
typedef struct
{
  uint64 dma_data[RFLM_GSM_MAX_TX_CAL_CMD_DMA_DATA_LENGTH];
}rflm_gsm_tx_cal_dma_data_output_type;



/*==============================================================================

                    HW PROGRAMMING TYPE DEFINITIONS

==============================================================================*/
/*! @struct rflm_gsm_rx_frame_input/output_type                               */
/*! @brief  This structure is for conveying the frame configuration to RFLM   */
/*! @param                                                                    */
typedef struct
{
  boolean active;
}rflm_gsm_rx_burst_params;

typedef struct
{
  boolean frame_id;
  rflm_gsm_slot_config_type config;
  rflm_gsm_rx_burst_params  rx_burst[RFLM_GSM_MAX_DL_TS];
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_rx_frame_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_rx_frame_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_frame_input/output_type                               */
/*! @brief  This structure is for conveying the frame configuration to RFLM   */
/*! @param                                                                    */
/*! @example                                                                  */
/*!  rflm_gsm_tx_frame_input_type config_var_in;                              */
/*!  rflm_gsm_tx_frame_input_type config_var_out;                             */
/*!  config_var_in.tx_slot[0].active = TRUE;                                  */
/*!  config_var_in.tx_slot[0].modulation = RFLM_GSM_MOD_GMSK;                 */
/*!  config_var_in.tx_slot[1].active = TRUE;                                  */
/*!  config_var_in.tx_slot[1].modulation = RFLM_GSM_MOD_GMSK;                 */
/*!  config_var_in.tx_slot[2].active = FALSE;                                 */
/*!  config_var_in.tx_slot[3].active = FALSE;                                 */
/*!  rflm_gsm_tx_frame_config( handle, &config_var_in, &config_var_out );     */
typedef struct
{
  boolean active;
  rflm_gsm_mod_type modulation;
  rflm_gsm_ramp_type ramp_type;
  uint32 transition_length;
}rflm_gsm_tx_slot_params;



typedef struct
{
  boolean frame_id;
  rflm_gsm_slot_config_type config;
  rflm_gsm_tx_slot_params   tx_slot[RFLM_GSM_MAX_TX_SLOT_PARAM_IDX]; //Includes KV slot and off ramp slot
  uint32* lmem_start_addr;
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_tx_frame_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_tx_frame_output_type;


/*============================================================================*/
/*! @struct rflm_gsm_txc_config_input/output_type                             */
/*! @brief  This structure is for conveying TxC specific settings to RFLM     */
/*! @param  pp_gain - Phase Path gain setting     							  */
/*! @param  pda_ctrl_wd - PDA Control Word				  					  */
/*! @param  rot_phase_increment - Tx Rotator Phase Increment     			  */

typedef struct
{
  uint32       pp_gain;
  uint32       pda_ctrl_wd;
  int32        rot_phase_inc; 
}rflm_gsm_txc_config_input_type;

typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_txc_config_output_type;

/*============================================================================*/
/*! @struct rflm_gsm_txc_prog_amxm_input_type                            	  */
/*! @brief  This structure defines the parameters necessary to program the    */
/*! 		amam/ampm tables for TxC targets. This will be called in a        */
/*! 		dedicated GFW thread before the first Tx slot is programmed       */
/*! @param  rflm_gsm_intf_idx - RFLM interface index from Tx Start            */
/*! @param  frame_id - Used to select the appropriate amam/ampm tables		  */

typedef struct
{

  boolean      frame_id;
  uint32       rflm_gsm_intf_idx;
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_txc_prog_amxm_input_type;

typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_txc_prog_amxm_output_type;

/*============================================================================*/
/*! @struct rflm_gsm_txc_frame_config_input/output_type                       */
/*! @brief  This structure is for TxC timing information to RFLM              */
/*! 		at the beginning of the frame.									  */
/*! @param	frame_id - This will be used to index into the correct timing info*/
/*!			structure when the settings are copied into RFLM local storage	  */
/*!	@param  slot_timing - this array contains the timing settings for this	  */
/*! 		frame															  */

typedef struct
{
  uint32 t1_time;
  uint32 trans_time;
  uint32 ramp_gap_length;
}rflm_gsm_txc_slot_timing_type;

typedef struct
{
  boolean      frame_id;
  rflm_gsm_txc_slot_timing_type slot_timing[RFLM_GSM_MAX_TX_SLOT_PARAM_IDX]; //KV slot and off ramp slot
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_txc_frame_config_input_type;

typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_txc_frame_config_output_type;

/*============================================================================*/
/*! @struct rflm_gsm_rxfe_input/output_type                                   */
/*! @brief Structure to RxFE config parameters                                */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param trigger time - time for script to take effect                      */
/*! @param execution time - time take for script to execute                   */
/* @TODO STRUCTURE MAY NOT BE NEEDED AS FW PROGRAM DIRECTLY TO DTR FOR RXFE   */
typedef struct
{
  rflm_ustmr_t trigger_time;
}rflm_gsm_rxfe_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_rxfe_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_txfe_input/output_type                                   */
/*! @brief Structure to TxFE config parameters                                */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param trigger time - time for script to take effect                      */
/*! @param execution time - time take for script to execute                   */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param modulation type - GMSK or 8-PSK for the slot                       */
/*! @param slot id - whic slot is this command for                            */
/*@TODO TRIGGER TIME MAY NOT BE NEEDED AS TRIGGER IS HANDLED BY FW->DTR       */
typedef struct
{
  rflm_ustmr_t trigger_time;
  uint32       mod_type;
  boolean      frame_id;
  uint8        slot_id;
  boolean      immediate_trig_flag;
  uint32       rflm_gsm_intf_idx;
}rflm_gsm_txfe_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
  rflm_dtr_tx_xfer_list_s *txfer_list_ptr;
}rflm_gsm_txfe_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_cal_step_txfe_input/output_type                       */
/*! @brief Structure to Tx Cal config parameters                              */
/*! @details This structure is for passing/returning information to/from FED  */

/*@TODO TRIGGER TIME MAY NOT BE NEEDED AS TRIGGER IS HANDLED BY FW->DTR       */
typedef struct
{
  rflm_ustmr_t trigger_time;
  boolean      immediate_trig_flag;
  uint8        device_id;
  uint8        buffer_index;
  uint8        step_id;
  uint32       step_duration;
  rflm_gsm_ramp_type ramp_mode;
  uint32* lmem_start_addr;
}rflm_gsm_tx_cal_step_txfe_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
  rflm_dtr_tx_xfer_list_s *txfer_list_ptr;
}rflm_gsm_tx_cal_step_txfe_output_type;



/*==============================================================================

                   CCS PROGRAMMING TYPE DEFINITIONS

==============================================================================*/
/*! @struct rflm_gsm_start_input/output_type                                  */
/*! @brief Structure for starting the CCS resources                           */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param rflm_gsm_intf_idx - index to array of infromation populated by RF  */
/*! @param sub_id - subscription the command is intended for                  */
/*! @param execution time - time take for script to execute                   */
typedef struct
{
  uint32 device_id;
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_start_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_start_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_release_input/output_type                                */
/*! @brief Structure for releasing the CCS resources                          */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param rflm_gsm_intf_idx - index to array of infromation populated by RF  */
/*! @param sub_id - subscription the command is intended for                  */
/*! @param execution time - time take for script to execute                   */
typedef struct
{
  uint32 device_id;
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_release_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_release_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_init_ccs_input/output_type                               */
/*! @brief Structure for init CCS script processing input/output params       */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param trigger time - time for script to take effect                      */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param execution time - time take for script to execute                   */
/*! @param common command - CCS event type to execute                         */
typedef struct
{
  rflm_ustmr_t trigger_time;
  boolean      immediate_trig_flag;
  uint32       rflm_gsm_intf_idx;
}rflm_gsm_init_ccs_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_init_ccs_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_rx_ccs_input_type                                        */
/*! @brief Structure for Rx CCS script processing input params                */
/*! @details This structure is for passing information to FED from GFW        */
/*! @param trigger time - time for script to take effect                      */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param rflm_gsm_intf_idx - RFLM interface index from Tx Start             */
/*! @param rf_sequence_number - GL1 managed number for overrun detection      */
typedef struct
{
  rflm_ustmr_t trigger_time;
  boolean      immediate_trig_flag;
  uint32       rflm_gsm_intf_idx;
  uint32       rf_sequence_number;
}rflm_gsm_rx_ccs_input_type;



/*============================================================================*/
/*! @struct rflm_gsm_rx_ccs_output_type                                       */
/*! @brief Structure for Rx CCS script processing output params               */
/*! @details This structure is for returning information from FED to GFW      */
/*! @param execution time - time take for script to execute                   */
/*! @param jdet_read_offset_qs - GFW JDET read time relative to Rx slot start */
typedef struct
{
  rflm_ustmr_t execution_time;

  /*! @brief GFW use this to set up their JDET read trigger */
  uint16       jdet_read_offset_qs; 
}rflm_gsm_rx_ccs_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_ccs_input_type                                        */
/*! @brief Structure for Tx CCS script processing input params                */
/*! @details This structure is for passing information to FED from GFW        */
/*! @param trigger time - time for script to take effect                      */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param frame_id - which RFLM GSM Tx frame buffer to use managed by GFW    */
/*! @param rflm_gsm_intf_idx - RFLM interface index from Tx Start             */
/*! @param rf_sequence_number - GL1 managed number for overrun detection      */
typedef struct
{
  rflm_ustmr_t trigger_time;
  boolean      immediate_trig_flag;
  boolean      frame_id;
  uint32       rflm_gsm_intf_idx;
  uint32       rf_sequence_number;
}rflm_gsm_tx_ccs_input_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_ccs_output_type                                       */
/*! @brief Structure for Tx CCS script processing output params               */
/*! @details This structure is for returning information from FED to GFW      */
/*! @param execution time - time take for script to execute                   */
typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_tx_ccs_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_band_ccs_input/output_type                            */
/*! @brief Structure for Tx band CCS script processing input/output params    */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param trigger time - time for script to take effect                      */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param execution time - time take for script to execute                   */
typedef struct
{
  rflm_ustmr_t trigger_time;
  boolean      immediate_trig_flag;
  uint32       rflm_gsm_intf_idx;
}rflm_gsm_tx_band_ccs_input_type;



typedef struct
{
  int16 pa_up_ramp_delta;
  int16 pa_down_ramp_delta;
  rflm_ustmr_t execution_time;
}rflm_gsm_tx_band_ccs_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_band_ccs_input/output_type                            */
/*! @brief Structure for Tx band CCS script processing input/output params    */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param trigger time - time for script to take effect                      */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param execution time - time take for script to execute                   */
typedef struct
{
  rflm_ustmr_t trigger_time;
  boolean      immediate_trig_flag;
  uint32       rflm_gsm_intf_idx;
}rflm_gsm_temp_comp_ccs_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_temp_comp_ccs_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_tx_cal_ccs_step_input/output_type                        */
/*! @brief Structure for Tx cal CCS script processing input/output params     */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param trigger time - time for script to take effect                      */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param device_id - index used by RF to retrieve the Tx cal events */
/*! @param buffer index - index used by RF to retrieve the Tx cal events (double-buffered) */
/*! @param step id - Cal step number in frame (MAX 250)                  */
typedef struct
{
  rflm_ustmr_t trigger_time;
  boolean      immediate_trig_flag;
  uint8        device_id;
  uint8        buffer_index;
  uint8        step_id;
}rflm_gsm_tx_cal_ccs_step_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_tx_cal_ccs_step_output_type;



/*==============================================================================*/
/*! @struct rflm_gsm_irat_ccs_input/output_type                               */
/*! @brief Structure for irat CCS script processing input/output params       */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param trigger time - time for script to take effect                      */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param script id - id of script to run   */
/*! @param action - For future use, if combining preload and trigger is needed*/
typedef struct
{ 
  rflm_ustmr_t trigger_time; 
  boolean 	 immediate_flag; /* If immediate_flag is set to true then trigger_time will be ignore by RFLM */
  uint8      script_id; 
  uint32     action; 
}rflm_gsm_irat_ccs_input_type; 



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_irat_ccs_output_type;



/*===========================================================================*/
/*! @struct rflm_gsm_init_umplt_input/output_type                            */
/*! @brief Structure for initialising UMPLT data table                       */
/*! @details This structure is for passing/returning information to/from FED */
typedef struct
{ 
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_init_umplt_input_type; 



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_init_umplt_output_type;



/*============================================================================*/
/*! @struct rflm_gsm_ip2_cal_ccs_input/output_type                                 */
/*! @brief Structure for Rx CCS script processing input/output params         */
/*! @details This structure is for passing/returning information to/from FED  */
/*! @param trigger time - time for script to take effect                      */
/*! @param immediate flag - send script ASAP and ignore the trigger time      */
/*! @param execution time - time take for script to execute                   */

typedef struct
{
  rflm_ustmr_t trigger_time_arr[MAX_IP2_STEP_TASKS_IN_CCS_QUEUE];
  boolean      immediate_trig_flag;
  uint32       ip2_cal_step_idx_arr[MAX_IP2_STEP_TASKS_IN_CCS_QUEUE];
  uint8         num_indices;
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_ip2_cal_step_ccs_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
  uint8        last_idx_enq_pass;
}rflm_gsm_ip2_cal_step_ccs_output_type;


typedef struct
{
  rflm_ustmr_t trigger_time;
  boolean      immediate_trig_flag;
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_ip2_cal_setup_cleanup_ccs_input_type;



typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_ip2_cal_setup_cleanup_ccs_output_type;


typedef struct
{
  rflm_ustmr_t        trigger_time;
  boolean             immediate_trig_flag; //If this is true, the event should be scheduled immediately 
                                           //and the function will block until the read is done.
  uint32              rflm_gsm_intf_idx;     //Use the same interface index as for Rx Start, 
                                             //so we can extract any information we need (should just be sub_id/device)
  uint8               reading_idx;     //To indicate which register to read back, this should correspond to the Rx slot
  rflm_gsm_fw_cb_type callback;         //GFW registers a callback to retrieve the result
  void*               fw_jdet_cb_arg;   //GFW needs args to recover their context in the callback
}rflm_gsm_prog_jdet_read_ccs_input_type;


typedef struct
{
  rflm_ustmr_t execution_time;
}rflm_gsm_prog_jdet_read_ccs_output_type;

typedef struct
{
  uint32       rflm_gsm_intf_idx;   //Use the same interface index as for Rx Start, 
                                    //so we can extract any information we need (should just be sub_id/device)
  uint8        reading_idx;     //To indicate which register to read back, this should correspond to the Rx slot
}rflm_gsm_get_jdet_reading_input_type;

typedef struct
{
  rflm_ustmr_t execution_time;
  uint16       jdet_reading_raw;    //Represents a voltage from JDET, will be checked against thresholds in RFSW
}rflm_gsm_get_jdet_reading_output_type;

#ifdef FEATURE_TX_SHARING
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    RF GSM CCS Event ID enum definition
 
    @details
    This enum defines the RF event ID associated with a (set of) device script
    execution that are added for tx sharing feature.
*/

typedef enum
{
  /*! Tx Init Event Trigger.*/           
  RF_GSM_EVENT_TX_INIT,  
  /*! Tx Band Preload.*/           
  RF_GSM_EVENT_TX_BAND_PRELOAD,  
  /*! Tx Burst Preload Event .*/
  RF_GSM_EVENT_TX_BURST_PRELOAD,
  /*! Tx Enable*/
  RF_GSM_EVENT_TX_ENABLE,
  /*! Tx Disable*/
  RF_GSM_EVENT_TX_DISABLE,
  /* Event Max */
  RF_GSM_EVENT_MAX
} rflm_gsm_event_id_type;

/*! @brief  Input parameters for rflm_gsm_rf_send_script API */
typedef struct
{
  rflm_ustmr_t    effect_time;        ///< Time for script to take effect
  boolean         immediate_flag;     ///< Flag for immediate effect_time
  int8            event_id;          ///< id of script to run  
  uint32          rflm_gsm_intf_idx;  
} rflm_gsm_rf_send_script_in_t;

/*! @brief  Output parameters for rflm_gsm_rf_send_script API */
typedef struct
{
  rflm_ustmr_t   est_done_time;       ///< estimated completion time  
} rflm_gsm_rf_send_script_out_t;

#endif

/*============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*/



/*============================================================================== 

                   RFLM CONFIG RELATED API PROTOTYPES

==============================================================================*/
/*! @fn rflm_gsm_register_fw_cb                                               */
/*! @brief A function that enables GFW to register callbacks                  */
/*! @details When CCS has completed processing an event RFLM GSM callback is  */
/*!          executed. This callback will execute a FW callback, if registered*/
/*!          with the pending event info to enable FW to determine whether    */
/*!          clocks can be turned off                                         */
extern rflm_err_t rflm_gsm_register_fw_cb( rflm_gsm_register_cb_input_type *vars_in );

/*============================================================================*/
/*! @fn rflm_gsm_deregister_fw_cb                                             */
/*! @brief A function that enables GFW to deregister callbacks                */
/*! @details FW can call this to deregister a callback previously regsitered  */
extern rflm_err_t rflm_gsm_deregister_fw_cb( rflm_gsm_subscription_id sub_id );



/*============================================================================*/
/*! @fn rflm_gsm_get_msg_type                                                 */
/*! @brief A function that returns message type as defined by RFLM_GSM_MSG_TYPE */
/*! @details Based on the upper half of rflm_gsm_intf_idx, this function      */
/*!          returns the RFLM_GSM message type.                               */
extern rflm_err_t rflm_gsm_get_msg_type( uint32 rflm_gsm_intf_idx, 
                                         rflm_gsm_msg_type *rflm_gsm_msg);

/*! @fn rflm_gsm_get_data_layer_variables                                     */
/*! @brief These functions are used to retrieve data-layer variables          */
/*! @details These functions retrieve RF variables required for FW processing */
extern rflm_err_t rflm_gsm_get_enh_rx_params( rflm_handle_rx_t rx_handle,
                                              rflm_gsm_get_enh_rx_params_input_type *rflm_gsm_vars_in,
                                              rflm_gsm_get_enh_rx_params_output_type *rflm_gsm_vars_out );

extern rflm_err_t rflm_gsm_get_rx_alpha( rflm_handle_rx_t rx_handle,
                                         rflm_gsm_get_rx_alpha_input_type *rflm_gsm_vars_in,
                                         uint16 *rflm_gsm_vars_out );

extern rflm_err_t rflm_gsm_get_rx_alpha_v2( rflm_handle_rx_t rx_handle,
                                            rflm_gsm_get_rx_alpha_input_type *rflm_gsm_vars_in,
                                            rflm_gsm_get_rx_alpha_output_type *rflm_gsm_vars_out );

extern rflm_err_t rflm_gsm_get_tx_alpha( rflm_handle_tx_t tx_handle,
                                         rflm_gsm_get_tx_alpha_input_type *rflm_gsm_vars_in,
                                         uint16 *rflm_gsm_vars_out );

extern rflm_err_t rflm_gsm_get_rf_gain( rflm_handle_rx_t rx_handle,
                                        rflm_gsm_get_rf_gain_input_type *rflm_gsm_vars_in,
                                        rflm_gsm_get_rf_gain_output_type *rflm_gsm_vars_out );


extern rflm_err_t rflm_gsm_get_tx_ramp_deltas( rflm_handle_tx_t tx_handle,
                                               rflm_gsm_tx_ramp_deltas_input_type *rflm_gsm_vars_in,
                                               rflm_gsm_tx_ramp_deltas_output_type *rflm_gsm_vars_out );

extern rflm_err_t rflm_gsm_get_kv_data( rflm_handle_tx_t tx_handle,
                                        rflm_gsm_tx_kv_data_input_type *rflm_gsm_vars_in,
                                        rflm_gsm_tx_kv_data_output_type *rflm_gsm_vars_out );

extern rflm_err_t rflm_gsm_get_spec_inv_data( rflm_handle_rx_t rx_handle,
                                              rflm_gsm_spec_inv_data_input_type *rflm_gsm_vars_in,
                                              rflm_gsm_spec_inv_data_output_type *rflm_gsm_vars_out );

extern rflm_err_t rflm_gsm_get_tx_cal_dma_data( rflm_handle_tx_t tx_handle,
                                                rflm_gsm_tx_cal_dma_data_input_type *rflm_gsm_vars_in,
                                                rflm_gsm_tx_cal_dma_data_output_type *rflm_gsm_vars_out );

/*============================================================================*/
/*! @fn rflm_gsm_rx_frame_config                                              */
/*! @brief A function to program and update FED with GSM Rx frame information */
/*! @details This function can be used to set up a frames worth of data for   */
/*!          later use when programming burst data.                           */
extern rflm_err_t rflm_gsm_rx_frame_config( rflm_handle_rx_t handle,
                                            rflm_gsm_rx_frame_input_type *rxlm_config_in,
                                            rflm_gsm_rx_frame_output_type *rxlm_config_out );



/*============================================================================*/
/*! @fn rflm_gsm_tx_frame_config                                              */
/*! @brief A function to program and update FED with GSM Tx frame information */
/*! @details This function can be used to set up a frames worth of data for   */
/*!          later use when programming burst data.                           */
extern rflm_err_t rflm_gsm_tx_frame_config( rflm_handle_tx_t handle,
                                            rflm_gsm_tx_frame_input_type *tx_frame_config_in,
                                            rflm_gsm_tx_frame_output_type *tx_frame_config_out );

/*============================================================================*/
/*! @fn rflm_gsm_txc_frame_config                                             */
/*! @brief A function to program TxC specific settings for Dime               */
/*! @details This function stores the timing settings for each Tx slot for use*/
/*! 		     during slot programming								                          */
extern rflm_err_t rflm_gsm_txc_frame_config( rflm_handle_tx_t handle,
                                             rflm_gsm_txc_frame_config_input_type *txc_frame_config_in,
                                             rflm_gsm_txc_frame_config_output_type *txc_frame_config_out );

/*============================================================================*/
/*! @fn rflm_gsm_txc_config                                                   */
/*! @brief A function to program TxC specific settings for Dime               */
/*! @details This function programs TxC registers for the frame               */

extern rflm_err_t rflm_gsm_txc_config( rflm_handle_tx_t handle,
                                       rflm_gsm_txc_config_input_type *txc_config_in,
                                       rflm_gsm_txc_config_output_type *txc_config_out );

/*============================================================================*/
/*! @fn rflm_gsm_txc_prog_amxm                                                */
/*! @brief A function to program TxC specific settings for Dime               */
/*! @details This function programs the AMXM tables based on the	            */
/*!          rflm_gsm_intf_idx for the Tx Start                               */

extern rflm_err_t rflm_gsm_txc_prog_amxm( rflm_handle_tx_t handle,
                                          rflm_gsm_txc_prog_amxm_input_type *txc_prog_amxm_in,
                                          rflm_gsm_txc_prog_amxm_output_type *txc_prog_amxm_out );
/*============================================================================== 
 
                         HW RELATED API PROTOTYPES
 
==============================================================================*/
/*! @fn rflm_gsm_rxfe_config                                                  */
/*! @brief A function to program the dynamic Rx settings                      */
/*! @details This function programs DRIF settings such as spectral inversion  */
/*@TODO THIS API WILL PROBABLY NOT BE NEEDED AS FW TALK DIRECTLY TO RXFE DTR  */
extern rflm_err_t rflm_gsm_rxfe_config( rflm_handle_rx_t handle,
                                        rflm_gsm_rxfe_input_type *rxfe_config_in,
                                        rflm_gsm_rxfe_output_type *rxfe_config_out );



/*============================================================================*/
/*! @fn rflm_gsm_txfe_config                                                  */
/*! @brief A function to program the dynamic Tx settings                      */
/*! @details This function programs DRIF settings such as ramp tables to LMEM */
/*!          FW need to trigger the DMA to DTR as a seperate call to DTR.     */
extern rflm_err_t rflm_gsm_txfe_config( rflm_handle_tx_t handle,
                                       rflm_gsm_txfe_input_type *txfe_config_in,
                                       rflm_gsm_txfe_output_type *txfe_config_out );




/*============================================================================*/
/*! @fn rflm_gsm_tx_cal_step_txfe_config                                      */
/*! @brief A function to program TxFE LMEM Tx calibration data                */
/*! @details This function should be called per calibration step              */
extern rflm_err_t rflm_gsm_tx_cal_step_txfe_config( rflm_handle_tx_t handle,
                                                    rflm_gsm_tx_cal_step_txfe_input_type *tx_cal_config_in,
                                                    rflm_gsm_tx_cal_step_txfe_output_type *tx_cal_config_out );



/*============================================================================== 
 
        RF GSM CCS SCRIPT PROGRAMMING RELATED API PROTOTYPES
 
==============================================================================*/
/*! @fn rflm_gsm_start                                                        */
/*! @brief An initialisation command to init RFLM on FW command               */
/*! @details Only FW can know when RFLM resources are required and can be     */
/*!          released, and therefore FW needs to trigger when this happens.   */
extern rflm_err_t rflm_gsm_start( rflm_gsm_start_input_type *vars_in,
                                  rflm_gsm_start_output_type *vars_out );



/*============================================================================*/
/*! @fn rflm_gsm_release                                                      */
/*! @brief A release command to init RFLM on FW command                       */
/*! @details Only FW can know when RFLM resources are required and can be     */
/*!          released, and therefore FW needs to trigger when this happens.   */
extern rflm_err_t rflm_gsm_release( rflm_gsm_release_input_type *vars_in,
                                    rflm_gsm_release_output_type *vars_out );



/*============================================================================*/
/*! @fn rflm_gsm_prog_init_ccs_script_cmd                                     */
/*! @brief A command for FW to call FED for CCS enter-mode processing         */
/*! @details Enter mode, Wakeup                                               */
extern rflm_err_t rflm_gsm_prog_init_ccs_script( rflm_handle_rx_t rflm_handle,
                                                 rflm_gsm_init_ccs_input_type *script_vars_in,
                                                 rflm_gsm_init_ccs_output_type *script_vars_out );



/*============================================================================*/
/*! @fn rflm_gsm_prog_rx_ccs_script                                           */
/*! @brief A command for FW to call FED for CCS Rx event processing           */
/*! @details Rx burst event processing called for start and stop with slot id */
extern rflm_err_t rflm_gsm_prog_rx_ccs_script( rflm_handle_rx_t rflm_handle,
                                               rflm_gsm_rx_ccs_input_type *script_vars_in,
                                               rflm_gsm_rx_ccs_output_type *script_vars_out );



/*============================================================================*/
/*! @fn rflm_gsm_prog_tx_ccs_script                                           */
/*! @brief A command for FW to call FED for CCS Tx event processing           */
/*! @details Tx burst event processing called for start, trans and stop with  */
/*!          slot id                                                          */
extern rflm_err_t rflm_gsm_prog_tx_ccs_script( rflm_handle_tx_t rflm_handle,
                                               rflm_gsm_tx_ccs_input_type *script_vars_in,
                                               rflm_gsm_tx_ccs_output_type *script_vars_out );



/*============================================================================*/
/*! @fn rflm_gsm_prog_tx_band_ccs_script                                      */
/*! @brief A command for FW to call FED for CCS Tx band event processing      */
/*! @details Tx set band event processing called to send the Tx band event,   */
/*!          where only one can exist at any one time and therefore no id is  */
/*!          required                                                         */
extern rflm_err_t rflm_gsm_prog_tx_band_ccs_script( rflm_handle_tx_t rflm_handle,
                                                    rflm_gsm_tx_band_ccs_input_type *script_vars_in,
                                                    rflm_gsm_tx_band_ccs_output_type *script_vars_out );



/*============================================================================*/
/*! @fn rflm_gsm_prog_temp_comp_ccs_script                                    */
/*! @brief A command for FW to call FED for CCS Therm read event processing   */
/*! @details Therm read event processing called to send the therm read event, */
/*!          where only one can exist at any one time and therefore no id is  */
/*!          required                                                         */
extern rflm_err_t rflm_gsm_prog_temp_comp_ccs_script( rflm_handle_tx_t rflm_handle,
                                                      rflm_gsm_temp_comp_ccs_input_type *script_vars_in,
                                                      rflm_gsm_temp_comp_ccs_output_type *script_vars_out );



/*============================================================================*/
/*! @fn rflm_gsm_prog_tx_cal_ccs_step                                         */
/*! @brief A command for FW to call FED for CCS Tx Cal step event processing  */
/*! @details Tx cal event processing called to send the Tx cal event          */
extern rflm_err_t rflm_gsm_prog_tx_cal_ccs_step( rflm_handle_tx_t rflm_handle,
                                                 rflm_gsm_tx_cal_ccs_step_input_type *script_vars_in,
                                                 rflm_gsm_tx_cal_ccs_step_output_type *script_vars_out );



/*============================================================================*/
/*! @fn rflm_gsm_prog_irat_preload_ccs_script                                 */
/*! @brief A command for FW to call FED for CCS irat preload event processing */
/*! @details irat preload event proc called to send the irat preload event    */
extern rflm_err_t rflm_gsm_prog_irat_preload_ccs_script( rflm_handle_rx_t rflm_handle,
                                                         rflm_gsm_irat_ccs_input_type *script_vars_in,
                                                         rflm_gsm_irat_ccs_output_type *script_vars_out);



/*============================================================================*/
/*! @fn rflm_gsm_prog_irat_trigger_ccs_script                                 */
/*! @brief A command for FW to call FED for CCS irat trigger event processing */
/*! @details irat trigger event proc called to send the irat trigger event    */
extern rflm_err_t rflm_gsm_prog_irat_trigger_ccs_script( rflm_handle_rx_t rflm_handle,
                                                         rflm_gsm_irat_ccs_input_type *script_vars_in,
                                                         rflm_gsm_irat_ccs_output_type *script_vars_out );


/*============================================================================*/
extern rflm_err_t rflm_gsm_prog_ip2_cal_cleanup_script( uint32 ip2_cal_rflm_rx_handle,
                                                        uint32 ip2_cal_rflm_tx_handle,
                                                        rflm_gsm_ip2_cal_setup_cleanup_ccs_input_type *script_vars_in,
                                                        rflm_gsm_ip2_cal_setup_cleanup_ccs_output_type *script_vars_out );

/*============================================================================*/
boolean rflm_gsm_init_ip2_cal_data_tbl(rflm_gsm_subscription_id sub_id);
/*============================================================================*/
boolean rflm_gsm_reset_ip2_cal_data_tbl(rflm_gsm_subscription_id sub_id);

/*============================================================================*/

extern rflm_err_t rflm_gsm_prog_ip2_cal_step_script( uint32 ip2_cal_rflm_rx_handle,
                                                     uint32 ip2_cal_rflm_tx_handle,
                                                     rflm_gsm_ip2_cal_step_ccs_input_type *script_vars_in,
                                                     rflm_gsm_ip2_cal_step_ccs_output_type *script_vars_out );

/*============================================================================*/

extern rflm_err_t rflm_gsm_prog_ip2_cal_setup_script( uint32 ip2_cal_rflm_rx_handle,
                                                      uint32 ip2_cal_rflm_tx_handle,
                                                      rflm_gsm_ip2_cal_setup_cleanup_ccs_input_type *script_vars_in,
                                                      rflm_gsm_ip2_cal_setup_cleanup_ccs_output_type *script_vars_out );

/*============================================================================*/
extern rflm_err_t rflm_gsm_update_ip2_cal_step_result( uint8 cal_step_index,
                                                       im2_data_type result,
                                                       rflm_gsm_subscription_id sub_id,
                                                       rflm_ustmr_t trig_time);

/*============================================================================*/
extern boolean rflm_gsm_get_ip2_cal_params( rflm_gsm_subscription_id sub_id, 
                                            uint32* settling_time, 
                                            uint16* ip2_cal_alpha );


/*============================================================================*/

extern rflm_err_t rflm_gsm_prog_jdet_read_script( rflm_handle_tx_t rflm_handle,
                                                  rflm_gsm_prog_jdet_read_ccs_input_type *prog_jdet_in,
                                                  rflm_gsm_prog_jdet_read_ccs_output_type *prog_jdet_out );
                                   
/*============================================================================*/               
                                             
extern rflm_err_t rflm_gsm_get_jdet_reading( rflm_handle_tx_t rflm_handle,
                                             rflm_gsm_get_jdet_reading_input_type *get_jdet_read_in,
                                             rflm_gsm_get_jdet_reading_output_type *get_jdet_read_out );



/*============================================================================== 
 
                           RF GSM INITIALISATION APIs

==============================================================================*/
void rflm_gsm_init_state_machine( void );



/*============================================================================== 
 
                               RF GSM UMPLT APIs

==============================================================================*/
/*! @fn rflm_gsm_init_umplt                                                   */
/*! @brief A command for FW to call FED for initialising data for UMPLT       */
/*! @details UMPLT does not have the RF SW code which initialises all the data*/
/*!          and therefore needs a hook to provide reasonable data so all     */
/*!          other APIs function as usual                                     */
extern rflm_err_t rflm_gsm_init_umplt( rflm_handle_rx_t rflm_handle,
                                       rflm_gsm_init_umplt_input_type *vars_in,
                                       rflm_gsm_init_umplt_output_type *vars_out );



/*============================================================================

                               TX SHARING APIs

============================================================================*/
/*! @fn rflm_gsm_rf_send_script                                 */
/*! @brief A command for FW to call FED for CCS tx sharing event processing */
/*! @details FW can send different types of command for Tx sharing feature through this API*/
/*!              Based on in->script_type event processing will be done by FED                */
#ifdef FEATURE_TX_SHARING
extern rflm_err_t rflm_gsm_rf_send_script( rflm_handle_tx_t rflm_handle, 
                                           rflm_gsm_rf_send_script_in_t  *in,
                                           rflm_gsm_rf_send_script_out_t *out );

#endif
/*============================================================================*/

#ifdef __cplusplus
} // extern "C"
#endif
/*============================================================================*/
#endif /* RFLM_API_GSM_H */

