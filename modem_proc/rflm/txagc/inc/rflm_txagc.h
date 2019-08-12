#ifndef RFLM_TXAGC_H
#define RFLM_TXAGC_H
/*
  @file
  rflm_txagc.h

  @brief
  This file contains RF FED tx agc definitions, enumerations and prototypes that
  are used in rflm scope.

  @details
  This file defines RF FED txagc interface,APIs, and commons structures

  @addtogroup RFLM_TXAGC_MODULE
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

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/17/15   cd      TxAGC updates for max power override support
02/10/15   cd      TxAGC updates for max power override support
07/24/14   hdz     Added rflm_txagc_grfc_pa_reset_config()
07/17/14   hdz     Added tech_id in rflm_txagc_log_packet_t
06/04/14   hdz     Added tech_id in allocating memory API arg list
04/03/14   ljl     enhance buffer ptr API change
03/12/14   hdz     WCDMA TCM data placement
03/04/14   cri     LTE specific CCS commit for Tx Override
01/20/14   hdz     Added txagc tcm data structure
01/06/14   hdz     Added rflm_txagc_lut_log_packet_t
12/11/13   hdz     Added rflm_txagc_diag_log_subpacket
11/18/13   cri     RFLM based Tx override hookup for LTE
10/17/13   hdz     Added xpt_mode in script setting
10/14/13   hdz     Removed rflm_txagc_c2k_build_script
07/25/13   hdz     change to rflm_tech_id_t
04/04/13   hdz     Initial creation

===========================================================================*/
#include "rflm_txagc_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! number PA states that is supported in FED due to WCDMA luts are in TCM*/
#define RFLM_WCDMA_NUM_PA_STATE                                                5

/*! size of txagc issue seq in words */
#define RFLM_WCDMA_TXAGC_ISSUE_SEQ_SIZE                                      190

/*! num of bytes in qpoet non-trigger data */
#define RFLM_WCDMA_TXAGC_QPOET_NON_TRIGGER_SIZE                                4

/* number of bytes in wtr non-trigger data */
#define RFLM_WCDMA_TXAGC_WTR_NON_TRIGGER_SIZE                                  4

/*------------------------------------------------------------------------------
RF FED txagc data types /function prototypes used in Tech FED
------------------------------------------------------------------------------*/
/*!  @brief txagc PA STM output data type  */
typedef struct
{
  /*! @brief  hyster_timer_count after the current PA STM run */
  int16 hyst_time_cnt;

  /*! @brief  the new pa state after the current PA STM run */
  rflm_txagc_pa_state_data_t pa_state;

  /*! @brief  the new pa action after the current PA STM run */
  rflm_txagc_pa_action_t pa_action;
  
}rflm_txagc_pa_output_t;

/*!   @brief txagc PA STM input data type  */
typedef struct
{
  /*! @brief  hyster_timer_count from last PA STM run, input to current PA STM run */
  int16 hyst_time_cnt;

  /*! @brief target txagc pwr in dBm of current PA STM run; pased in from FW */
  int16 current_txagc_dbm10;

  /*! @brief txagc pwr in dBm of last PA STM run, used to determine the new PA action */
  int16 previous_txagc_dbm10;

  /*! @brief  pa state */
  rflm_txagc_pa_state_data_t pa_state; 
  
}rflm_txagc_pa_input_t;

/*!
  @brief txagc  lookup input data type
 */
typedef struct
{
  /*! @brief target txagc pwr in dBm10*/
  int16 txagc_dbm10;

  /*! @brief current active tech ID*/
  rflm_tech_id_t tech_id;      

  /*! @brief new PA state from current PA STM run */
  rflm_txagc_pa_state_data_t pa_state;

  /*! @brief pout/pin index for temp/freq compensation, passed in from FW. This is used 
     only for LTE. All other teches just set to all zero, e.g., {0,0}*/
  rflm_txagc_pout_pin_t pout_pin_idx;  

}rflm_txagc_agc_input_t;

/*!  @brief txagc  lookup output data type  */
typedef struct
{
  /*! @brief row index of smps table */
  uint8 row_idx_smps;   

  /*! @brief row index of const table*/
  uint8 row_idx_const;                   

  /*! @brief current iq gain*/
  uint32 iq_gain;                        

  /*! @brief current env scale*/
  uint32 env_scale;    
  
}rflm_txagc_agc_output_t;

/*!  @brief txagc  build script input data type  */
typedef struct
{
  /*! @brief hyst timer count  */
  int16 hyst_time_cnt;

  /*! @brief row index of smps table */
  uint8 row_idx_smps;

  /*! @brief row index of const table */
  uint8 row_idx_const;

  /*! @brief current iq gain */
  uint32 iq_gain;

  /*! @brief  current env scale */
  uint32 env_scale;

  /*! @brief new PA state */
  rflm_txagc_pa_state_data_t pa_state;

  /*! @brief new PA action */
  rflm_txagc_pa_action_t pa_action;

  /*! @brief new xpt_mode */
  rflm_txagc_xpt_mode_t xpt_mode;
  
  /*! @brief capped tx power in dBm10
   this is used with rflm_txagc_agc_input_t.max_env_scale together to reflect the 
   actual tx power after capping by max_env_scale;

    actual_tx_power = txagc_dbm10 - (final_env_scale - env_scale);
    
   e.g.,  max_env_scale = 10 -> 1dB;
   env_scale after et lookup is 8 --> 0.8 dB;
  then final_env_scale = max(8,10) = 10 --> backoff 1 dB, and
  actual_tx_power = txagc_dbm10 - (final_env_scale - env_scale)
  */
  int16 actual_tx_power;     

  /*! @brief Max capped TxLin power in dBm10 
   this is max TxLin power after capping by max_env_scale
  */
  int16 max_txlin_power;  
}rflm_txagc_script_input_t;

/*!  @brief txagc  log packet structure */
typedef struct
{
  uint32 tx_pwrdbm10     : 16; /*!< tx pwr */
  uint32 chain_idx       : 4;  /*!< chain idx */
  uint32 env_scale       : 12; /*!< env_scale */

  uint32 iq              : 12;
  uint32 xpt_mode        : 4; /*!< tx pwr */
  uint32 pa_action       : 4; /*!< tx pwr */
  uint32 pa_state        : 4;
  uint32 row_idx_smps    : 8; /*!< tx pwr */

  uint32 do_fbrx         : 4;  /*!< do fbrx */
  uint32 row_idx_const   : 8; /*!< tx pwr */
  uint32 tech_id         : 8; /*!< tech Id */
  uint32 reserved        : 12; /*!< reserved */
  
}rflm_txagc_log_packet_t;

/*!  @brief txagc  lut log packet structure */
typedef struct
{
  uint8 tech_id;

  uint8 pa_state;

  int16 txpwr_dbm10_fullbias[RFLM_TXAGC_TX_LIN_APT_SIZE];

  /*! @brief  txagc linearizer data structure for all PA states*/
  int16 txpwr_dbm10_smpsbias[RFLM_TXAGC_TX_LIN_APT_SIZE];

  /*! @brief IQ gain */
  uint32 iq_gain[RFLM_TXAGC_TX_LIN_APT_SIZE]; 

  /* @brief  tbl to const tbl. E.g., index_to_const_bias_tbl[x] is the row index in 
    txpwr_dbm10_fullbias[] where txpwr_dbm10_smpsbias[x] and 
    txpwr_dbm10_fullbias[index_to_const_bias_tbl[x]] has the same RGI;  */    
  uint8 index_to_const_bias_tbl[RFLM_TXAGC_TX_LIN_APT_SIZE]; 

}rflm_txagc_lut_logpacket_one_pa_t;

/*!  @brief txagc  lut log packet structure for 4 PA states */
typedef struct
{
  /*! @ brief lut log packet for 4 PA states. This is determined by size of log 
  buffer which is 3968 bytes.*/
  rflm_txagc_lut_logpacket_one_pa_t tx_lut[4];

}rflm_txagc_lut_log_packet_t;

/*! @brief structure to hold txagc tcm data*/
typedef struct
{
  /*! used to hold the PA rise sp of 8 PA states (each is int16) and in TCM
    pa_rise_sp_uint64[0] = pa0_rise_sp|pa1_rise_sp|pa2_rise_sp|pa3_rise_sp
    pa_rise_sp_uint64[1] = pa4_rise_sp|pa5_rise_sp|pa6_rise_sp|pa7_rise_sp

    used in intrinsic API to do 4 half words comparison
*/
  union
  {
    uint64 pa_rise_sp_uint64[2];
    int16 pa_rise_sp_int16[8]; 
  }pa_rise_sp;
  
}rflm_txagc_tcm_t;

/*! 
  @brief 
  structure to hold txagc dma seq and pa/wtr/qpoet data in tcm

*/
typedef struct
{

  /*! flag to indicate the tcm buffer is in use or not*/
  boolean in_use;

  /*! dma seq buffers  */
  uint32 dma_seq_buf[RFLM_TXAGC_FIXUP_MAX][RFLM_WCDMA_TXAGC_ISSUE_SEQ_SIZE];

}rflm_txagc_fixup_tcm_t;


/*------------------------------------------------------------------------------
RF FED common TXAGC external APIs prototyes
------------------------------------------------------------------------------*/
boolean
rflm_txagc_init
(
  void
);

/*----------------------------------------------------------------------------*/
rflm_txagc_event_template_t*
rflm_txagc_allocate_memory
(
  uint8 tech_id
);

/*----------------------------------------------------------------------------*/
boolean
rflm_txagc_release_memory
(
  rflm_txagc_event_template_t *txagc_template
);

/*----------------------------------------------------------------------------*/
void
rflm_txagc_pa_state_machine
(
  rflm_txagc_pa_input_t *pa_input,
  rflm_txagc_script_input_t *pa_output,
  rflm_txagc_event_template_t *txagc_template
);

/*----------------------------------------------------------------------------*/
void 
rflm_txagc_sync_data_buffer
(
  rflm_txagc_event_template_t *txagc_template
);

/*----------------------------------------------------------------------------*/
void
rflm_txagc_lookup
(
  rflm_txagc_agc_input_t* txagc_input,
  rflm_txagc_script_input_t* txagc_output,
  rflm_txagc_event_template_t *txagc_template
);

/*----------------------------------------------------------------------------*/
void rflm_txagc_bias_rgi_selection(
   rflm_txagc_script_input_t* in, 
   rflm_txagc_event_template_t* txagc_template
);

/*----------------------------------------------------------------------------*/
boolean 
rflm_txagc_get_data
(
  rflm_txagc_event_template_t *txagc_template,
  void *user_output_data,
  rflm_txagc_get_data_action_e data_t  
);

/*----------------------------------------------------------------------------*/
boolean
rflm_txagc_set_data
(
  rflm_txagc_event_template_t *txagc_template,
  const void *user_input_data,
  rflm_txagc_set_data_action_e data_t
);

/*----------------------------------------------------------------------------*/
uint8 rflm_txagc_binary_search
(
  int16 pwr_dbm10[],
  int16 search,
  uint8 n
);

/*----------------------------------------------------------------------------*/
void
rflm_txagc_lookup_apt
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static
);

/*----------------------------------------------------------------------------*/
void
rflm_txagc_lookup_ept
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static
);

/*----------------------------------------------------------------------------*/
void
rflm_txagc_lookup_et
(
  rflm_txagc_agc_input_t *txagc_input,
  rflm_txagc_script_input_t *txagc_output,
  rflm_txagc_dynamic_t *txagc_dynamic,
  rflm_txagc_static_t *txagc_static
);

/*----------------------------------------------------------------------------*/
void
rflm_txagc_diag_log_subpacket
(
  rflm_txagc_event_template_t *txagc_template
);

/*----------------------------------------------------------------------------*/
uint32*
rflm_txagc_get_active_enh_buff_ptr
(
  rflm_txagc_event_template_t *txagc_template,
  rflm_txagc_pa_state_data_t pa_state
);

/*----------------------------------------------------------------------------*/
uint32*
rflm_txagc_get_nonactive_enh_buff_ptr
(
  rflm_txagc_event_template_t *txagc_template,
  rflm_txagc_pa_state_data_t pa_state
);

/*----------------------------------------------------------------------------*/
void
rflm_txagc_grfc_pa_reset_config
(
  void* input_ptr,
  rflm_txagc_event_template_t *txagc_template,
  uint32 pdm_addr
);

#ifdef __cplusplus
}
#endif

/*!   @} */

#endif
