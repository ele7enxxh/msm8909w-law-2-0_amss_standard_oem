#ifndef RFLM_TXAGC_API_H
#define RFLM_TXAGC_API_H
/*
  @file
  rflm_txagc_api.h

  @brief
  This file contains RF FED tx agc definitions, enumerations and prototypes that
  are exposed to external modules, such as RF SW and FW.

  @details
  This file defines RF FED txagc interface,APIs, and commons structures

  @addtogroup RFLM_CDMA_FW_INTF
  @{
*/

/*=============================================================================

                R F  F E D TXAGC   H E A D E R   F I L E 
       
DESCRIPTION
  This file contains declarations and definitions to support RF frent end driver txagc module.

Copyright (c) 2013-2015  by Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/txagc/rflm_txagc_api.h#1 $

when       who    what, where, why
--------   ---    ------------------------------------------------------------- 
09/01/15   vs      Add phase info to TxAGC static LUT
04/03/15   zhh     update PA phase comp offset at the first Tx AGC update
02/17/15   cd      TxAGC updates for max power override support
02/10/15   cd      TxAGC updates for max power override support
09/11/14   hdz     Decoupling rfmeas_event_infor_type from Tx module
08/14/14   hdz     Added rflm_txagc_cmn_save_path_delay_params
07/23/14   hdz     Added rflm_txagc_et_path_delay_t
07/23/14   hdz     Added grfc_reset_offset
06/04/14    pv     Added support for FBRx RB Based Freq comp.
05/21/14   hdz     Increase fixup table size 
05/07/14   ka      Common tx agc / tx lin fix for fbrx gain state setting
04/23/14   bn     Renaming lte_set_pa_state_ptr fn to rflm_txagc_fed_set_pa_state_ptr
04/14/14   aw     Added txagc_meta structure array as a member of txagc parameter for performance optimization
04/04/14   hdz    Change due to wtr/pa/qpoet/iref regrouping
04/03/14   ljl    align data structure related to txagc lookup gain search
                  to 64-bits boundary
03/24/14   hdz    Moved pin/pout from dynamic to static buffer
03/24/14   cd      Interface update to build static and dynamic lin settings 
                   separately from issue seq preparation
03/23/14   hdz    Added rflm_txagc_reset_txagc_stm()
03/12/14   hdz    Reduced the size of param circular buffer to reduce timeline 
03/06/14   hdz    Added LTE ET/EPT gain search support
01/27/14   jf     New common PA state machine API for LTE
01/19/14   hdz    Added mem idx to indicate which TCM memory to be used
01/06/14   hdz    Added rflm_txagc_lut_diag_log_subpacket
12/11/13   hdz    Added log_context_id
12/04/13   hdz    Added chain_idx
11/17/13   hdz    Added xpt mode in txagc_param data structure
11/13/13   hdz    Added wtr_trigger_offset
11/05/13   hdz    h_id is added for pa state ptr API()
10/15/13   hdz    Repartition rflm_txagc_modem_xxx API prototype
10/10/13   hdz    Changed the wtr_trigger_time
10/07/13   hdz    Added pa state index array
10/03/13   hdz    Added xPT API prototypes
10/01/13   hdz    Added support for tds txagc
10/01/13   hdz    Moved rflm_txagc_prepare_seq() to this file
09/30/13   jf     Add numb_types for front_end_addr
09/20/13   hdz    Added support for TDS multi-events 
09/18/13   hdz    Change txagc LUT interface for new trigger task
09/16/13   hdz    Added support for new trigger task
09/13/13   hdz    Used internal DM buffer for txagc sequence
09/04/13   ra     Support interface to CCS_RF_INTF
08/27/13   hdz    Added addrs of RF front end devices
                  Reduced number of event from 5 to 1
08/19/13   hdz    Added Iref and wxe_byte support
08/15/13   hdz    Changed agva_gw from uint32 to uint64;
                  Removed q_curr; 
                  Added set_pa_lut to support upto 8 PAs
08/12/13   hdz    Changed the size of GRFC script
07/31/13   hdz    Added c2k_update_txagc support
07/25/13   hdz    removed comdef.h and change prefix ccs_ to rflm_ccs_
07/22/13   sty    use rflm_tech_id_t instead of rflm_txagc_tech_id_t 
04/04/13   hdz    Initial creation

===========================================================================*/
#include "rflm_ccs_rf_intf.h"
#include "rflm_api_cmn.h"
#include "rflm_ccs_irat_intf.h"


#ifdef __cplusplus
extern "C" {
#endif

#define RFLM_TXAGC_MAX_TASK_NUM                                             14
#define RFLM_TXAGC_MAX_TASK_DATA_SIZE                                       60
#define RFLM_TXAGC_NUM_ROWS_TX_SCRIPT                                        2
#define RFLM_TXAGC_NUM_COLLUMS_TX_SCRIPT                                     4
#define RFLM_TXAGC_TX_LIN_APT_SIZE                                          64
#define RFLM_TXAGC_TX_LIN_EPT_SIZE                                           8
#define RFLM_TXAGC_TX_LIN_ET_SIZE                                            4
#define RFLM_TXAGC_XPT_NUM_FREQ_COMP                                         8
#define RFLM_TXAGC_XPT_NUM_SLOT                                              3
#define RFLM_TXAGC_XPT_NUM_SF                                               10
#define RFLM_TXAGC_PP_BUFF_SIZE                                              2
#define RFLM_TXAGC_NUM_PA_SP                           RFLM_TXAGC_PA_STATE_MAX
#define RFLM_TXAGC_NUM_PARAM_BUFFER                                          3
#define RFLM_TXAGC_MAX_TRANS_NUM                                            50
#define RFLM_TXAGC_MAX_FIXUP_TBL_SIZE_BYTE                                  50
#define RFLM_TXAGC_NON_TRIGGER_DATA_SIZE                                     8
#define RFLM_TXAGC_WTR_NON_TRIGGER_DATA_SIZE                                 6
#define RFLM_TXAGC_QPOET_NON_TRIGGER_DATA_SIZE                               4
#define RFLM_TXAGC_NUM_TXAGC_EVENT_SEQ                                       3
#define RFLM_TXAGC_FBRX_NUM_GAIN_STATES                                      4
#define RFLM_TXAGC_SCRIPT_DATA_BUF_SIZE_WORDS     (RFLM_TXAGC_MAX_TRANS_NUM*4) 
/*---------------------------------------------------------------------------2---
RF FED txagc handle related data structure/ function APIs
------------------------------------------------------------------------------*/                                             
/*!  @brief enum of txagc memory handle       */
typedef enum
{
  RFLM_TXAGC_MEM_0,
  RFLM_TXAGC_MEM_1,
  RFLM_TXAGC_MEM_2,
  RFLM_TXAGC_MAX_MEM
}rflm_txagc_handle_t;

/*------------------------------------------------------------------------------
-                              RF FED fw->txagc interface
------------------------------------------------------------------------------*/
/*!  @brief structure of txagc pout pint parameter type. */
typedef struct
{
  /*! @brief slot iD, Set to "0" if tech doesn't have slot  */
  uint8 slot;

  /*!@brief subframe ID. Set to "0"  if tech doesn't have subframe */
  uint8 subframe;   
  
}rflm_txagc_pout_pin_t;

/*!  @brief structure of txagc target power type. One of input parameter for txagc 
  common API. 
*/
typedef struct
{
  /*! @brief  txagc in dbm10 unit  */  
  int16 txagc_dbm10;

  /*! @brief  tech ID  */
  rflm_tech_id_t tech_id;

  /*! @brief pingpong buffer ID, set to "0" by default  */
  uint8 pp_buff_id; 

  /*! @brief pin_pout idx, set to all 0 by default */
  rflm_txagc_pout_pin_t pout_pin_idx;  
  
}rflm_txagc_target_pwr_t;

/*------------------------------------------------------------------------------
-                              RF FED sw->txagc interface
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
RF FED PA state related
------------------------------------------------------------------------------*/
/*!  @brief enum of txagc PA state  */
typedef enum
{
  RFLM_TXAGC_PA_STATE0 = 0,          // lowest PA state
  RFLM_TXAGC_PA_STATE1,
  RFLM_TXAGC_PA_STATE2,
  RFLM_TXAGC_PA_STATE3,  
  RFLM_TXAGC_PA_STATE4,  
  RFLM_TXAGC_PA_STATE5,  
  RFLM_TXAGC_PA_STATE6,  
  RFLM_TXAGC_PA_STATE7,              // highest PA state
  RFLM_TXAGC_PA_STATE_MAX,
}rflm_txagc_pa_state_data_t;

/*!  @brief enum of txagc xpt mode  */
typedef enum
{
  /*! @brief APT mode */
  RFLM_TXAGC_APT,

  /*! @brief EPT mode */
  RFLM_TXAGC_EPT, 

  /*! @brief ET mode */
  RFLM_TXAGC_ET, 

  /*! @brief max xPT mode */
  RFLM_TXAGC_MAX_XPT_MODE  
  
}rflm_txagc_xpt_mode_t;

/*!  @brief enum of txlin max_pwr cap/boost mode  */
typedef enum
{
  /*! @brief Do nothing to default Tx AGC algorithm */
  RFLM_TXAGC_TXLIN_MAX_PWR_OVR_DEFAULT,

  /*! @brief EPT mode */
  RFLM_TXAGC_TXLIN_MAX_PWR_OVR_BOOST, 

  /*! @brief ET mode */
  RFLM_TXAGC_TXLIN_MAX_PWR_OVR_QPA_BACKOFF, 

}rflm_txagc_txlin_max_pwr_ovr_mode_t;

/*!  @brief structure of txagc single PA state switch point. */
typedef struct
{
  /*! @brief active if TRUE; non-active if FALSE */
  boolean pa_state_is_active;

  /*! @brief  the switch point to fall from current PA state to next lower PA state */
  int16 pa_fall_sp; 
  
  /*! @brief switch point to rise from the previous lower PA state to current PA state.*/
  int16 pa_rise_sp;

  /*! @brief xPT mode*/
  rflm_txagc_xpt_mode_t xpt_mode;

  /*! @brief PA state */
  rflm_txagc_pa_state_data_t pa_state; 
  
}rflm_txagc_single_pa_sp_t;

/*! @brief structure of PA switch point of all PA states */
typedef struct
{
  /*! @brief switch point for all PA states */
  rflm_txagc_single_pa_sp_t sp[RFLM_TXAGC_PA_STATE_MAX];

  /*! @brief PA state active pointer for all PA states */
  rflm_txagc_single_pa_sp_t* active_pa_state_ptr[RFLM_TXAGC_PA_STATE_MAX];

  /*! @brief hold pa state index*/  
  rflm_txagc_pa_state_data_t temp_pa_state[RFLM_TXAGC_PA_STATE_MAX];

  /*! @brief hyst timer */
  int16 hysteresis_fall;  
  
} rflm_txagc_pa_sp_t;

/*------------------------------------------------------------------------------
RF FED rflm_txagc_event_t related
------------------------------------------------------------------------------*/
/*!
  @brief
  Common TxAGC event Data structure 
*/
typedef struct
{
  /*! @brief Header and tasks for the event */
  rflm_ccs_rf_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[RFLM_TXAGC_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) rflm_txagc_event_infor_type;

/*!
  @brief enum of tasks needs txagc dynamically fill in data

  @detail The tasks in the list need txagc dynamically to fill in data. After 
  txagc lookup, build script API will loop through the chosen sequence and fill 
  in data for the tasks that are listed in this ENUM. For the tasks that are 
  not listed in this enum will not be touched.
*/
typedef enum
{
  /*! @brief non dynamic programming tasks */
  RFLM_TXAGC_NONE_ACTION_TASK,

  /*! @brief  set wtr task */
  RFLM_TXAGC_SET_WTR_TASK,

  /*! @brief  set q-current task */
  RFLM_TXAGC_SET_QCURR_TASK,

  /*! @brief  set QPOET task */
  RFLM_TXAGC_SET_QPOET_TASK,

  /*! @brief  set pa task */
  RFLM_TXAGC_SET_PA_TASK,
  
  /*! @brief  set FBRx task */
  RFLM_TXAGC_SET_FBRx_TASK,

  /*! @brief  set WXE task */
  RFLM_TXAGC_SET_WXE_TASK,

  /*! @brief  PA trigger task */
  RFLM_TXAGC_PA_TRIG_TASK,

  /*! @brief  QPOET early trigger task */
  RFLM_TXAGC_QPOET_EARLY_TRIG_TASK,
  
  /*! @brief  QPOET late trigger task */
  RFLM_TXAGC_QPOET_LATE_TRIG_TASK,  
  
  /*! @brief  WTR early trigger task */
  RFLM_TXAGC_WTR_EARLY_TRIG_TASK,

  /*! @brief  WTR late trigger task */
  RFLM_TXAGC_WTR_LATE_TRIG_TASK,

  /*! @brief  max task */
  RFLM_TXAGC_MAX_TASK
  
}rflm_txagc_task_t;

/*!
  @brief enum of type of transaction that txagc can automatically handle
*/
typedef enum
{
  /*! @brief normal RFFE/SSBI type */
  RFLM_TXAGC_RFFE_SSBI,

  /*! @brief  RFFE ext type */
  RFLM_TXAGC_RFFE_EXT,

  /*! @brief  grfc type */
  RFLM_TXAGC_GRFC,
  
  /*! @brief  max type */
  RFLM_TXAGC_MAX_TYPE
  
}rflm_txagc_task_type_t;

/*!
  @brief rflm txagc cmn ccs data buffer type
*/
typedef union
{
  /*! @brief EXT RFFE write type */
  rflm_ccs_rf_transaction_rffe_ext_rw_t rffe_ext_data;

  /*! @brief normal RFFE/SSBI type */
  rflm_ccs_rf_transaction_ssbi_rffe_rw_pair_t rffe_ssbi_data;
    
}rflm_txagc_ccs_data_buff_t;

/*!
  @brief rflm txagc transaction descriptor  type
*/
typedef enum
{
  /*! @brief DUMMY write */
  RFLM_TXAGC_TRANS_DUMMY,

  /*! @brief DUMMY write */
  RFLM_TXAGC_TRANS_WRITE,

}rflm_txagc_trans_descriptor_t;

/*!  @brief enum of txagc action type*/
typedef enum
{
  /* @brief PA change from Low PA(APT) to High PA(APT)*/
  RFLM_TXAGC_PA_L_2_H_APT_2_APT,  

  /*!@brief PA change from Low PA(APT) to High PA(EPT)  */
  RFLM_TXAGC_PA_L_2_H_APT_2_EPT,   

  /*! @brief PA change from Low PA(APT) to High PA(ET) */
  RFLM_TXAGC_PA_L_2_H_APT_2_ET,    

  /*! @brief PA change from Low PA(EPT) to High PA(ET)  */
  RFLM_TXAGC_PA_L_2_H_EPT_2_ET,    

  /*! @brief PA change from High PA(APT) to Low PA(APT) */
  RFLM_TXAGC_PA_H_2_L_APT_2_APT,   

  /*! @brief PA change from High PA(EPT) to Low PA(APT) */
  RFLM_TXAGC_PA_H_2_L_EPT_2_APT,   

  /*! @brief PA change from High PA(ET) to Low PA(APT) */
  RFLM_TXAGC_PA_H_2_L_ET_2_APT,    

  /*! @brief PA change from High PA(ET) to Low PA(EPT) */
  RFLM_TXAGC_PA_H_2_L_ET_2_EPT,    

  /*! @brief No pA change, avga change for low pwr to high pwr */
  RFLM_TXAGC_AVGA_L_2_H,   

  /*! @brief No pA change, avga change for High pwr to Low pwr */
  RFLM_TXAGC_AVGA_H_2_L,   

  /*! @brief Max PA action */
  RFLM_TXAGC_PA_MAX_ACT
  
}rflm_txagc_pa_action_t;

/*! @brief enum for type of fixup table   */
typedef enum
{
  /*! @brief RGI from Lo to Hi */
  RFLM_TXAGC_FIXUP_RGI_Lo_2_Hi,

  /*! @brief RGI from Hi to Lo */
  RFLM_TXAGC_FIXUP_RGI_Hi_2_Lo,

  /*! @brief PA from Lo to Hi */
  RFLM_TXAGC_FIXUP_PA_Lo_2_Hi,

  /*! @brief PA from Hi to Lo */  
  RFLM_TXAGC_FIXUP_PA_Hi_2_Lo,

  /*! @brief Max type */  
  RFLM_TXAGC_FIXUP_MAX,

  /*! @brief Generic seq type */  
  RFLM_TXAGC_FIXUP_GENERIC
  
}rflm_txagc_fixup_tbl_t;

/*! @brief This structure define the QPOET early trigger/later trigger task and 
WTR early trigger /later trigger task relative timing information for four 
scenraios
*/
typedef struct
{
  rflm_txagc_trans_descriptor_t qpoet_early_trigger_task;
  rflm_txagc_trans_descriptor_t qpoet_late_trigger_task;
  rflm_txagc_trans_descriptor_t wtr_early_trigger_task;
  rflm_txagc_trans_descriptor_t wtr_late_trigger_task;
}rflm_txagc_trigger_task_t;

/*!  @brief  rflm txagc fixup data type  */
typedef struct
{
  /*! word offset from ccs sequence header  */
  uint32 word_offset;

  /*! byte offset from in the word  a unit32 word   byte3|byte2|byte1|byte0 
    have byte offset  3 |2 |1 |0     */
  uint32 byte_offset;
  
}rflm_txagc_fixup_data_t;

/*!  @brief structure of txagc event data type.  */
typedef struct
{
  /*! @brief  ccs task infor*/
  rflm_txagc_event_infor_type ccs_task_info;

  rflm_ccs_rf_task_t  ccs_task_buff[RFLM_TXAGC_MAX_TASK_NUM];

  /*! @brief  ccs task list */
  rflm_txagc_task_t ccs_task_list[RFLM_TXAGC_MAX_TASK_NUM];

  /*! @brief  the task_idx in the ccs_task_buff that listed in ccs_task_list
    @detail:
     For every task in ccs_task_buff has a task type listed in 
     ccs_task_list array. Now, for a given task in ccs_task_list, say 
     RFLM_TXAGC_WTR_EARLY_TRIG_TASK, task_idx[RFLM_TXAGC_WTR_EARLY_TRIG_TASK] 
     will give the idx of task in ccs_task_buff; e.g.,
     
    ccs_task_buff[task_idx[RFLM_TXAGC_WTR_EARLY_TRIG_TASK] ] is 
    RFLM_TXAGC_WTR_EARLY_TRIG_TASK;

    Note that, for the task that is not in the ccs_task_list, 
    task_idx[task_name] = RFLM_TXAGC_MAX_TASK_NUM ;

  */
  uint8 task_idx[RFLM_TXAGC_MAX_TASK];

  /*! @brief  ccs task type list */
  rflm_txagc_task_type_t ccs_task_type_list[RFLM_TXAGC_MAX_TASK_NUM];

  /*! @brief  ccs transaction type list for all four scenarios */
  rflm_txagc_trans_descriptor_t ccs_task_trans_list[RFLM_TXAGC_FIXUP_MAX]
                                                   [RFLM_TXAGC_MAX_TRANS_NUM];
}rflm_txagc_event_t;
/*------------------------------------------------------------------------------
RF FED rflm_txagc_pa_state_lut_t  related
------------------------------------------------------------------------------*/
/*!  @brief structure of set pa state LUT */
typedef union
{
  /*! @brief  ssbi_rrf_write structure*/
  uint8 data[8]; 

  /*! @brief  grfc PA write: grfc_mask[0] is for level_low;grfc_mask[1] is for 
    levle_high*/
  uint32 grfc_mask[2];  
  
}rflm_txagc_set_pa_state_lut_t;
/*------------------------------------------------------------------------------
RF FED rflm_txagc_tx_lin_full_bias_t  related
------------------------------------------------------------------------------*/
/*!
  @brief Structure for sending bank & base for AMAM, AMPM, ET detrough to FW . 
  One instance of this is needed per linearizer row per table type. (table
  type = AMAM or AMPM or ET detrough). ET detrough info is unused for EPT PA
  state
*/
typedef struct ALIGN(4)
{
  /*! @brief Base addr (offset within bank) */
  uint16 base;
  
  /*! @brief Bank ID . This doesn't have to be 2 bytes. 1 byte is enough. But
   * for word alignment, we are using 16 bits. */
  uint16 bank_id;
  
} rflm_txagc_xpt_bank_info_lin_row_t;

/*!
  @brief Structure for sending bank and base for AMAM,AMPM and ET detrough
  info to FW . This structure holds info for one row of a Linearizer table
  i.e PA state.  Size = 4 * 3 (bytes)= 12 bytes . Hence Aligned to word
  boundary .
*/
typedef struct ALIGN(4)
{
  /*! @brief AMAM table information */
  rflm_txagc_xpt_bank_info_lin_row_t amam;

  /*! @brief AMPM table information */
  rflm_txagc_xpt_bank_info_lin_row_t ampm;

  /*! @brief EPDT (Envelope path detrough table) information */
  rflm_txagc_xpt_bank_info_lin_row_t epdt;
  
} rflm_txagc_xpt_bank_info_row_t;

/*!
  @brief Structure of dtr iqmc data (alpha and beta) used for 
  IQ mismatch compensation
*/
typedef struct 
{
  uint16 alpha;
  uint16 beta;
} rflm_txagc_dtr_iqmc_data_t;

/*!
  @brief Structure of txagc static lut scripts, including WTR/PA/QPOET and Iref.
   The structure is aligned with 32 bytes for one cache line and therefore it is
   padded.
*/
typedef struct ALIGN(32)
{
  /*! @brief  pointer to the set PA lut script in Script manager */
  rflm_txagc_set_pa_state_lut_t set_pa_lut;
  
  /*! @ qpoet write data, assuming qpoet[0] is the first go-out byte */
  struct rflm_txagc_qpoet_data_s{
    uint8 qpoet_non_trigger_data[RFLM_TXAGC_QPOET_NON_TRIGGER_DATA_SIZE];
    uint8 qpoet_trigger_data;
  } qpoet_write_data;

  /*! @ AVGA gainword, assuming agva_gw[0-3] is the first-go-out byte for 
      wtr1625, For wtr3925, avga_gw[0] is wxe; avga_gw[1] is for fbrx; and
      avga_gw[2-5] are for wtr, the num of trans is determined by the sequence;                  */
  struct rflm_txagc_wtr_data_s{
    uint8 wtr_non_trigger_data[RFLM_TXAGC_WTR_NON_TRIGGER_DATA_SIZE];
    uint8 wtr_trigger_data;
  } wtr_write_data;

  /*! @brief RGI value from calibration */
  uint16 rgi_val;

  /*! @brief wtr3925 iref */
  uint8 iref; 
  
  /*! @brief iqmc */
  rflm_txagc_dtr_iqmc_data_t iqmc_data;
    
  /*! @brief fbrx gain state*/
  uint8 fbrx_gain_state;  

  /*! @brief phase compensation */
  int16 phase;

  /*! @brief padding for 32 bytes alignment *///reducing the padding to 4 bytes from 6
  uint8 reserved[4];

}rflm_txagc_static_lut_script_t;

/*!  @brief structure of smps tx lin, align to 32 bytes boundary
*/
typedef struct ALIGN(32)
{
  /* txagc static lut script */
  rflm_txagc_static_lut_script_t txagc_static_lut_script[RFLM_TXAGC_TX_LIN_APT_SIZE];

  /*! @brief xpt info */
  rflm_txagc_xpt_bank_info_row_t xpt_info[RFLM_TXAGC_TX_LIN_EPT_SIZE];
 
  /*! @brief constant biased power table */
  int16 txpwr_dbm10_fullbias[RFLM_TXAGC_TX_LIN_APT_SIZE];   
 
  /*! @ hold pout freq comp index table */
  uint16  freq_comp_tbl[RFLM_TXAGC_TX_LIN_APT_SIZE];

  /*! @brief xpt linearizer table size*/
  uint8 xpt_lin_size;
  
  /*! @brief Additional Env Scale offset for capped linearizer entries */
  uint32 max_env_scale[RFLM_TXAGC_TX_LIN_EPT_SIZE];

  /*! @brief Max Lin power cap/boost algorithm value */
  uint8 max_lin_cap_boost_algo;
      
}rflm_txagc_lin_static_t;

/*!  @brief structure of tx agc lin   */
typedef struct ALIGN(8)
{
  /*! @brief smps biased power table */
  int16 txpwr_dbm10_smpsbias[RFLM_TXAGC_TX_LIN_APT_SIZE];

  /*! @brief IQ gain */
  uint32 iq_gain[RFLM_TXAGC_TX_LIN_APT_SIZE]; 

  /* @brief  tbl to const tbl. E.g., index_to_const_bias_tbl[x] is the row index in 
    txpwr_dbm10_fullbias[] where txpwr_dbm10_smpsbias[x] and 
    txpwr_dbm10_fullbias[index_to_const_bias_tbl[x]] has the same RGI;  */    
  uint8 index_to_const_bias_tbl[RFLM_TXAGC_TX_LIN_APT_SIZE]; 
  
  /* index of RGI2PWR table that has highest power value */
  uint8 highest_pwr_rgi_index[RFLM_TXAGC_XPT_NUM_SLOT]
                             [RFLM_TXAGC_XPT_NUM_SF];
  
}rflm_txagc_lin_dynamic_t;
/*------------------------------------------------------------------------------
RF FED rflm_txagc_pa_state_lut_t  related
------------------------------------------------------------------------------*/
/*!  @brief enum of txagc parameters type  */
typedef struct ALIGN(4)
{
  /*! @brief use to hold current hyst_timer count-down counter */
  int16 hyst_timer_cnt;   

  /*! @brief current pa state */
  rflm_txagc_pa_state_data_t pa_state; 

  /*! @brief current pwr*/
  int16 current_pwr;      

  /*! @brief current PAaction*/
  rflm_txagc_pa_action_t pa_action;   

  /*! @brief 1" do fbrx; "0" disable fbrx */
  boolean do_fbrx;      

  /*! @brief current iq_gain */
  uint32 iq_gain;       

  /*! @brief  current env_scale; */
  uint32 env_scale;    

  /*! @brief  txagc look up index for smps table */
  uint8 row_idx_smps;  

  /*! @brief txagc look up index for const table  */
  uint8 row_idx_const; 

  /*! @brief txagc look up index for const table  */
  uint8 xpt_mode;  

}rflm_txagc_meta_t;

typedef struct
{

    /*! @brief txagc meta  */
  rflm_txagc_meta_t txagc_meta[RFLM_TXAGC_NUM_PARAM_BUFFER];

  /*! @brief the current index of param buffers  */
  uint8 current_param_idx;

  /*! @brief current Tx chain_idx */
  uint32 chain_idx;

  /*! @brief current lmem groupA addr */
  uint32 lmem_group_a_addr;

  /*! @brief current lmem groupB addr */
  uint32 lmem_group_b_addr;
  
}rflm_txagc_params_t;

/*! @brief  For c2k/wcdma/tdscdma:
  ET/EPT use pin_adj[0][0][pa] and pout[0][0][pa][] as memory for pin_adj /pout_adj 
  APT uses pin_adj[0][0][pa] and pout_adj[0][0][pa][0] only;

  Note here Pin is a scalar for each PA for APT/EPT/ET;
  Pout is size of 4 for EPT, size of 8 for EPT and 0 for APT.

  For lte:
  ET/EPT use the whole 3-d arrays of  pin_adj[][][pa] and pout[][][pa][]
  APT use the first entry pin_adj[][][pa] and pout[][][pa][0]   
*/
typedef struct ALIGN(8)
{
  /*! @brief pout adjust table */
  int16 pout_adj[RFLM_TXAGC_XPT_NUM_SLOT]
                [RFLM_TXAGC_XPT_NUM_SF]
                [RFLM_TXAGC_XPT_NUM_FREQ_COMP];    

  /*! @brief pout adjust table */
  int16 pin_adj[RFLM_TXAGC_XPT_NUM_SLOT]
               [RFLM_TXAGC_XPT_NUM_SF];   


}rflm_txagc_freq_temp_comp_t;

/*! @ brief FBRx RB dependent Temp freq ajust */
typedef struct ALIGN(8)
{
  int32   gain[RFLM_TXAGC_XPT_NUM_SF];
  int16   tx_power[RFLM_TXAGC_XPT_NUM_SF];
  uint8   ubias[RFLM_TXAGC_XPT_NUM_SF];
  uint16  rx_scale[RFLM_TXAGC_XPT_NUM_SF];
  uint16  predet_index[RFLM_TXAGC_XPT_NUM_SF];
  int16   tx_power_offset_vs_temp_vs_freq[RFLM_TXAGC_XPT_NUM_SF];

}rflm_fbrx_comp_t;

/*!  @brief structure of txagc script type */
typedef struct
{
  /*! @brief fixup table for the 4 scenarios*/
  rflm_txagc_fixup_data_t fixup_tbl[RFLM_TXAGC_FIXUP_MAX][RFLM_TXAGC_MAX_FIXUP_TBL_SIZE_BYTE];

  /*! @brief pointer array for txagc event*/
  rflm_txagc_event_t* txagc_event_ptr[RFLM_TXAGC_NUM_TXAGC_EVENT_SEQ];

  /*! @brief pointer to a dm buffer for pre-compiled txagc_event*/
  uint32* precompiled_seq_ptr[RFLM_TXAGC_NUM_TXAGC_EVENT_SEQ];

  /*! @brief pointer array to dm buffers for dma_sequence for four scenarios 
   The first four are used for non-tds tech's four scenarios;
   For TDS, buffer0/buffer4/buffer5 will be used for the three events*/
  uint32* dma_seq_ptr[(int)RFLM_TXAGC_FIXUP_MAX+(int)RFLM_TXAGC_NUM_TXAGC_EVENT_SEQ-1];

  /*! @brief buffer for txagc lookup optimization */  
  uint32 *enh_feature_buf;

  /*! @brief fbrx disable script  */
  uint8 fbrx_disable_script;
    
}rflm_txagc_script_t;

/*!  @brief structure addrs of all rf front end devices .
          Assume xx_addr[0] is the first-go-out byte.
          Each element is per transaction addr;
          For SSBI/RFFE, the last 8 bits are addr;
          For RFFE-EXT, the whole 16 bits are addr;
          W also assume the last byte is the addr for dummy trigger; e.g.,  pa_addr[7] will be 
          dummp addr for the PA trigger;
          The rest will be valid addr for the device; e.g.,  pa_addr[0-6] will 
          hold valid PA address */
typedef struct
{
  /*! @brief pa address, pa_addr data structure*/
  struct pa_addr_s{
    uint16 pa_non_trigger_addr[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    uint16 pa_trigger_addr;
    uint16 pa_dummy_addr;
  }pa_addr[RFLM_TXAGC_PA_STATE_MAX];

  struct pa_addr_num_bytes_s{
    int8 pa_non_trigger_num_bytes[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    int8 pa_trigger_num_bytes;
    int8 pa_dummy_num_bytes;
  }pa_num_bytes; 

  /*! @brief qpoet address */
  struct qpoet_addr_s{
    uint16 qpoet_non_trigger_addr[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    uint16 qpoet_trigger_addr;
    uint16 qpoet_dummy_addr;
  }qpoet_addr;  

  struct qpoet_addr_num_bytes_s{
    int8 qpoet_non_trigger_num_bytes[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    int8 qpoet_trigger_num_bytes;
    int8 qpoet_dummy_num_bytes;
  }qpoet_num_bytes; 

  /*! @brief wtr address */
  struct wtr_addr_s{
    uint16 wtr_non_trigger_addr[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    uint16 wtr_trigger_addr;
    uint16 wtr_dummy_addr;
  }wtr_addr;

  struct wtr_addr_num_bytes_s{
    int8 wtr_non_trigger_num_bytes[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    int8 wtr_trigger_num_bytes;
    int8 wtr_dummy_num_bytes;
  }wtr_num_bytes;

  /*! @brief fbrx address */
  struct fbrx_addr_s{
    uint16 fbrx_non_trigger_addr[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    uint16 fbrx_trigger_addr;
    uint16 fbrx_dummy_addr;
  }fbrx_addr;

  struct fbrx_addr_num_bytes_s{
    int8 fbrx_non_trigger_num_bytes[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    int8 fbrx_trigger_num_bytes;
    int8 fbrx_dummy_num_bytes;
  }fbrx_num_bytes;

  /*! @brief wxe address */
  struct wxe_addr_s{
    uint16 wxe_non_trigger_addr[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    uint16 wxe_trigger_addr;
    uint16 wxe_dummy_addr;
  }wxe_addr;

  struct wxe_addr_num_bytes_s{
    int8 wxe_non_trigger_num_bytes[RFLM_TXAGC_NON_TRIGGER_DATA_SIZE];
    int8 wxe_trigger_num_bytes;
    int8 wxe_dummy_num_bytes;
  }wxe_num_bytes;

  /* Flag to indicate if ICQ addr is per PA state or fixed for all PA states
     TRUE: one ICQ addr for all PA states 
     FALSE: one ICQ addr per PA state
  */
  boolean fixed_pa_icq_addr;

}rflm_txagc_front_end_addr_t;

/*!  @brief  path delay params data type*/
typedef struct
{
  uint32 dm1_rate_q31;
  uint32 dm1_rate_inv_q24;
  uint32 dm2_rate_q31;
  uint32 dm2_rate_inv_q26;
  uint32 dm3_rate_q25;
  uint32 dm3_rate_inv_q31;
}rflm_txagc_path_delay_params_t;

/*!  @brief  wtr and qpoet trigger time */
typedef struct
{
  /*! @brief wtr early trigger time*/
  uint32 wtr_early_trigger_offset;

  /*! @brief wtr late trigger time*/  
  uint32 wtr_late_trigger_offset;

  /*! @brief qpoet early trigger time*/
  uint32 qpoet_early_trigger_offset;

  /*! @brief qpoet late trigger time*/  
  uint32 qpoet_late_trigger_offset;

  /*! @brief wtr trigger time for all fixup scenarios*/  
  uint32 wtr_trigger_offset[RFLM_TXAGC_FIXUP_MAX];
  
}rflm_txagc_trigger_time_t;

/*!  @brief structure of txagc static data */
typedef struct
{
  /*! @brief  Current active tech ID */
  rflm_tech_id_t tech_id;

  /*! @brief  txagc analog parameters that are only updated once. */
  rflm_txagc_lin_static_t tx_lin_static[RFLM_TXAGC_PA_STATE_MAX];

  /*! @brief addr of all RF analog devices   */
  rflm_txagc_front_end_addr_t tx_front_end_addr;

  /*! @brief qpoet/wtr trigger time offset*/
  rflm_txagc_trigger_time_t tx_trigger_time;

  /*! @brief  pointer to txagc script and pa lut script */
  rflm_txagc_script_t tx_script;

  /*! @brief freq_comp temp comp adj tables*/
  rflm_txagc_freq_temp_comp_t freq_temp_adj[RFLM_TXAGC_PA_STATE_MAX];

  /*! @brief fbrx gain state wtr non trigger data*/
  //uint8 fbrx_wtr_non_trigger_data[RFLM_FBRX_NUM_GAIN_STATES];
  uint8 fbrx_wtr_non_trigger_data[RFLM_TXAGC_FBRX_NUM_GAIN_STATES];

    /*! @brief RB based freq_comp temp comp adj tables for FBRx */
  rflm_fbrx_comp_t fbrx_freq_temp_adj[RFLM_TXAGC_FBRX_NUM_GAIN_STATES];

  /*! @brief path delay params */
  rflm_txagc_path_delay_params_t path_delay_params;

}rflm_txagc_static_t;

/*!  @brief structure of txagc dynamic data */
typedef struct
{
  /*! @brief  txagc linearizer data structure for all PA states*/
  rflm_txagc_lin_dynamic_t tx_lin_dynamic[RFLM_TXAGC_PA_STATE_MAX];

  /*! @brief  PA switch point structure */
  rflm_txagc_pa_sp_t pa_sp;

}rflm_txagc_dynamic_t;

/*! @brief  txagc control data structure */
typedef struct
{
  /*! @brief  flag to indicate if the memory is locked or unlocked.  1--> locked; 0--> 
   unlocked  */
  boolean rflm_txagc_mem_lock;

  /*! @brief a flag to indicate a SW update is ready */
  boolean sw_update_ready;

  /*! @brief current txagc active buffer */
  boolean txagc_active_buff;

  /*! @brief et_exit flag. This is use to indicate if current PA transaction 
  does et exit or not*/
  boolean et_exit;

  /*! @brief rflm logpack context id for txagc */
  int log_context_id;

  /*! @brief rflm logpack context id for txagc lut*/
  int lut_log_context_id;

  /*! @brief txagc mem idx*/
  uint32 txagc_mem_idx;

  /*! @brief grfc pa reset task pdm memory offset*/
  uint32 grfc_reset_offset;

  /*! @brief the first txagc update flag*/
  boolean first_txagc_update;

}rflm_txagc_ctl_t;

/*------------------------------------------------------------------------------
RF FED txagc event template type, SW to txagc main interface
------------------------------------------------------------------------------*/
/*!  @brief txagc event template type */
typedef struct
{
  /*! @brief  static data that only need to be updated once */
  rflm_txagc_static_t txagc_static_data;

  /*! @brief  dynamic data that will be updated during run-time rountine, such as temp  
  // comp etc. Therefore two memories are allocated for ping-pong mechanism.*/
  rflm_txagc_dynamic_t txagc_dynamic_data[2];

  /*! @brief  internal structure to hold some key paramsters */
  rflm_txagc_params_t params;

  /*! @brief  txagc internal control signals*/
  rflm_txagc_ctl_t txagc_ctl_data;

}rflm_txagc_event_template_t;

/*! @brief  txagc set data type*/
typedef enum
{
  /*! @brief set static data*/
  TXAGC_SET_STATIC_DATA,

  /*! @brief set dynamic data*/
  TXAGC_SET_DYNAMIC_DATA,

  /*! @brief set  params*/
  TXAGC_SET_PARAMS_DATA
  
}rflm_txagc_set_data_action_e;

/*! @brief txagc get data type*/
typedef enum
{
  /*! @brief get static data*/
  TXAGC_GET_STATIC_DATA,

  /*! @brief get dynamic data*/
  TXAGC_GET_DYNAMIC_DATA,

  /*! @brief get params data*/
  TXAGC_GET_PARAMS_DATA
  
}rflm_txagc_get_data_action_e;

/*! @brief txagc et path delay type*/
typedef struct
{
  /*! @brief coarse_tau */
  uint32 coarse_tau;

  /*! @brief dm2 fine tau */
  uint32 dm2_fine_tau;

  /*! @brief dm3_frac_tau*/
  uint32 dm3_frac_tau;
  
}rflm_txagc_et_path_delay_t;

/*------------------------------------------------------------------------------
RF FED common TXAGC external APIs prototyes
------------------------------------------------------------------------------*/
void
rflm_txagc_update_pa_state_ptr
(
  rflm_txagc_dynamic_t *txagc_dynamic_data,
  boolean *pa_active
);


/*---------------------------------------------------------------------------*/
/*! @detail
  The API is used by RFSW to prepare all txagc needed tables/sequences/params 
  based on dm_handle_id (or txlm_handle_id)
  which are:

  [1]. to convert event type of sequence into issue_seq type of seq;
  [2]. prepare tasktype list, e.g., SSBI/RFFE-EXT/GRFC
  [3]. prepare transaction type list, e.g., RFLM_TXAGC_TRANS_WRITE or DUMMY 
  WRITE;
  [4]. prepare fixup tables and dma sequences for all scenarios

  The API should be called before starting txagc, and typically in configure 
  txagc and is called once per Tx wakeup.

  However, this API shoulde be called after RFSW has populated the txagc event 
  sequence to comon txagc memory since the event sequence is needed for 
  preparing in the API.

  ex:

  RFSW populates tx LUT;
  RFSW populates event;
  then RF SW calls this API
  
*/
void
rflm_txagc_prepare_agc_seq
(
  uint32 h_id
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_prepare_dynamic_lin_settings
(
  uint32 h_id
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_prepare_static_lin_settings
(
  uint32 h_id
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_prepare_smps_lut_srch_complete
(
  rflm_tech_id_t tech_id,  
  rflm_txagc_pa_state_data_t pa_state,
  rflm_txagc_pout_pin_t pout_pin_idx, 
  rflm_txagc_static_t  *txagc_static,
  rflm_txagc_dynamic_t *txagc_dynamic
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_prepare_smps_lut_srch_update
(
  int16  smpsbias_pwr_dbm10,
  uint8  rgi_index,
  rflm_tech_id_t tech_id,  
  rflm_txagc_pa_state_data_t pa_state,
  rflm_txagc_pout_pin_t pout_pin_idx, 
  rflm_txagc_static_t  *txagc_static,
  rflm_txagc_dynamic_t *txagc_dynamic
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_set_pa_state_ptr
(
  rflm_txagc_dynamic_t *txagc_dynamic_data,
  boolean *pa_active,
  uint32 handle_id
);


/*---------------------------------------------------------------------------*/
void
rflm_txagc_fed_set_pa_state_ptr
(
  rflm_txagc_dynamic_t *txagc_dynamic_data,
  boolean *pa_active,
  uint32 handle_id
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_lut_diag_log_subpacket
(
  uint32 handle_id
);

/*---------------------------------------------------------------------------*/
void
rflm_txagc_reset_txagc_stm
(
  uint32 handle_id,
  boolean sleep_et_dac
);

/*---------------------------------------------------------------------------*/
uint32*
rflm_txagc_get_static_buffer
(
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
rflm_err_t 
rflm_wcdma_txagc_set_first_txagc_update
(
  rflm_handle_tx_t handle_id,
  boolean first_update
);

#ifdef __cplusplus
}
#endif

/*!   @} */

#endif
