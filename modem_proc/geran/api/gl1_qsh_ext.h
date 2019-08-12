#ifndef GL1_QSH_EXT_H
#define GL1_QSH_EXT_H

/*========================================================================
                 GSM GPRS QSH DEFINITIONS FILE
DESCRIPTION
   This file contains definitions used by both the GSM/GPRS QSH interface

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gl1_qsh_ext.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
19/02/15    ab      Initial Version
20/02/15    ab      CR773948 GL1 Support for provding Dumps and Metrics for QSH


===========================================================================*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "qsh.h"
#include "sys_type.h"
#include "gprsdef.h"
#ifdef GFW_QSH_NOT_REQ_ON_JO
#include "gfw_qsh_ext.h"
#endif

#define GL1_QSH_MAJOR_VER  1
#define GL1_QSH_MINOR_VER  1

/* -------------------  Definitions for data dump types  ------------------ */


#define GL1_QSH_DUMP_DATA_LEN     732  // gl1_qsh_dump_tag_data_s

#define GL1_QSH_STATE_LOG_LEN     16 

#define GL1_QSH_MSG_LOG_LEN       16

#define GL1_QSH_MDSP_LOG_LEN      20


// re-work of existing types
typedef struct
{
   uint32 FN;
   uint8  state;
   uint8  sub_state;
   uint8  main_command;
   uint8  command;
   
} gl1_qsh_state_log_t;         // size is 8  

typedef struct
{
   uint32   FN;
   uint16   msg_set;
   uint16   msg_ID;
   
} gl1_qsh_msg_traffic_log_t;   //size is 8  

typedef struct {           
 /* General Handover Details */
 byte                  handover_type;
 byte                  BSIC;
 byte                  handover_synchronisation; 
 byte                  band;
 
 uint16                BCCH_ARFCN;
 /* Used for TA and QBIT_LAG calculation */
 byte                  timing_advance_valid;
 byte                  timing_advance;
 
 byte                  real_time_difference_valid;
 byte                  real_time_difference;
 byte                  NCI;
 /* Used for vocoder blanking */
 byte                  old_channel_mode;
 
 byte                  new_channel_mode;
 byte                  ho_detailsPad1;
 byte                  ho_detailsPad2;
 byte                  ho_detailsPad3;
} gl1_qsh_ho_details_T;  // size is 16.

typedef  struct  {
  word    arfcn;
  int16  rx_power;          /* last averaged power level */
  
  uint32   fn_lag;            /* only valid if bsic_known == TRUE */
  
  byte bsic_known;
  byte    bsic;              /* only valid if bsic_known == TRUE */   
  int16   qbit_lag;          /* only valid if bsic_known == TRUE */ 
} gl1_qsh_metric_ncell_info_T;  // 12 bytes

typedef struct
{
  uint32   SwFN;
  uint32   HwFN;
  uint16   seq_num;
  uint16   Qs;
  uint8    num_cmds;
  uint8    cmdId;
  uint8    is_async;
  uint8    mdsp_cmd_buf_pad1;
} gl1_qsh_mdsp_cmd_buf_t;   //size is 16  

typedef struct
{
  uint32                    wakeup_delay;               
  uint32                    mcpm;
  uint32                    rf_awake;
  uint32                    isr;

  uint32                    rf_enter_mode;
  uint32                    rf_script_dload_us;
  int32                     rf_cnf_to_vstmr_us;
  uint32                    total_wakeup_time;  
}gl1_qsh_sleep_wakeup_t;   //32


typedef struct
{
  uint32                                go_to_sleep_frame_number;
  uint32                                wakeup_frame_number;
  uint32                                go_to_sleep;
  uint32                                rf_rx_off;
  uint32                                mcpm;
} gl1_qsh_sleep_start_t;  // 20

typedef struct
{
  byte                                  scheduled;
  byte                                  msim_sys_mode;   
  byte                                  low_monitor_mode;
  byte                                  diag_mask_disabled;

  byte                                  diag_f3_trace_disabled;
  byte                                  parallel_warmup;
  uint16                                returned_rf_warmup_usec;
  uint16                                adaptive_warmup_us;
  uint16                                used_warmup_usec;
//+12

  uint32                                gsm_clk_sleep_duration;
  uint32                                gsm_clk_sleep_duration_frames;
  uint32                                egts_offset_gsm_clks;
  uint32                                late_ols_offset_usec;
// +16
  int32                                 awake_duration_us;
  int32                                 warmup_end_to_gstmr_us;
  int32                                 rf_wakeup_to_vstmr_us;
  int32                                 commit_gsmclks;   
// +16
} gl1_qsh_sleep_info_t;  //+44

typedef struct
{
  uint32                             frame_number;
  uint32                             start_qsc;
  int32                              start_ustmr;

  byte                               ignore_next_gstmr;
  byte                               async_sleep;
  byte                               async_exec;
  byte                               async_poss;

  byte                               async_NV_poss;
  byte                               async_option3_NV_poss;
  byte                               precheck;
  byte                               int_pend;

  byte                               irat_active;
  byte                               tx_on;
  byte                               saic_status;
  byte                               asyncPad1; 
} gl1_qsh_sleep_async_t;  // 28

typedef struct 
{
  byte Paging_Multiframe;
  byte Paging_Block_Index;
  byte Paging_Data_Valid;
  byte All_Paging_Blocks;
   
  byte Paging_Mode;
  byte CCCH_Group;
  byte Paging_Group;
  byte Pwr_Av_Freq;
}gl1_qsh_paging_data_t;  //  8

/*
** Notes:
** for sleep the common type gsm_sleep_common_type (4 x uint64) has been replaced with uint32 for duration only in us 
*/
typedef struct
{
  gl1_qsh_sleep_info_t               sleep_info;         //44
  gl1_qsh_sleep_async_t              async_msg;          //28
  byte                               forced_wakeup;      // 1
  byte                               sleef_buf_pad1;
} gl1_qsh_sleep_buf_t;  // 74


typedef struct
{   
  byte                        current_alloc_type;
  byte                        mac_ul_ctrl_available;
  byte                        release_pending;
  byte                        confirm_pending;
  
  byte                        release_in_progress;
  byte                        egprs_or_gprs_tbf;
  byte                        rlc_mode;  
  byte                        mac_mode;

  uint32                      abort_fn;
  uint32                      wait_for_pca_fn;
 
  /*  Simultaneous PUA/PDA starting time variables */
  uint32                      dl_st;
  uint32                      ul_st;
}gl1_qsh_transfer_t;   //    24


/* -----------------------  Definitions for QSH Dump  --------------------- */
typedef enum
{
  GL1_QSH_DUMP_TAG_AS_ID_1   = 0,
  GL1_QSH_DUMP_TAG_AS_ID_2   = 1,
  GL1_QSH_DUMP_TAG_MAX
} gl1_qsh_dump_tag_e;

// GL1_QSH_DUMP_TAG_DATA structure for QSH. sizeof(hdr) + 808 bytes.
typedef struct
{
  uint32                     gstmr_FN; 

  byte                       main_command;
  byte                       command;
  byte                       l1_state;
  byte                       sub_state;

  byte                       sub_sub_state;
  byte                       pscan_state;
  byte                       acq_bcch_state;
  byte                       trm_client_id;        /* was trm_client_enum_t */

  byte                       l1_prev_state;
  byte                       multi_sim_pscan_state;
  byte                       multi_sim_acq_bcch_list_state;
  byte                       multi_sim_acquiring_state;

  byte                       multi_sim_find_sch_state;
  byte                       multi_sim_reselection_state;
  byte                       multi_sim_decoupled_state;
  byte                       firmware_loaded;

  byte                       first_multi_sim_init;
  byte                       firmware_load_status;  /* firmware_load_status_t */
  byte                       firmware_unload_in_progress;
  byte                       going_active;         /* Transitioning from Inactive to Active */

  byte                       qta_in_progress;
  byte                       reservation_for_bcch;
  byte                       rf_in_deep_sleep_whilst_forced_wakeup;
  byte                       g2x_resel_in_progress;

//+28

// irat
  byte                       gsm_only;           
  byte                       sce_mode;           
  byte                       sce_running; 
  byte                       G2L_INIT_sent;

  byte                       receive_in_progress;    
  byte                       monscan_in_progress;    
  byte                       wcdma_srch_in_progress;
  byte                       wcdma_id_srch_in_progress; 

  byte                       wcdma_srch_abort; 
  byte                       wcdma_srch_state;
  byte                       wcdma_srch_id_state; 
  byte                       wcdma_srch_reconfirm_state; 

  byte                       wcdma_srch_ded_state; 
  byte                       wcdma_srch_id_in_tbf_state; 
  byte                       num_freqs;           
  byte                       num_W_freqs;           

  byte                       num_L_freqs; 
  byte                       active_irat;           /* bit mask for L , T , W  */
//+18

// x2g , simple bytes could use bit field

  byte                       cm_initialised;
  byte                       mdsp_awake_flag;
  byte                       gl1_qta_fw_rf_initialised;
  byte                       gl1_qta_wcdma_has_initialised;
                                                              // aligned to 16 x 4  =  64
//+4

// most recent L1 states changes in time order
  gl1_qsh_state_log_t         gl1_state_log_buf[GL1_QSH_STATE_LOG_LEN];   // 16 x 8   = 128

// most recent filtered L1 msgs in time order
  gl1_qsh_msg_traffic_log_t   gl1_log_msg[GL1_QSH_MSG_LOG_LEN];           // 16 x 8   = 128

// gl1_hw_sleep_struct subset
  gl1_qsh_sleep_buf_t         gl1_hw_sleep_buf;                          //            74 
  
  gl1_qsh_transfer_t          gl1_transfer_buf;                          //            24
  
  gl1_qsh_paging_data_t       gl1_paging_data_buf;                      //             8
  
  // mdsp cmd buf subset
  gl1_qsh_mdsp_cmd_buf_t      mdsp_cmd_buf[GL1_QSH_MDSP_LOG_LEN];        // 20 x 16  = 320
  
} gl1_dump_tag_data_s;    //  732


// GL1_QSH_DUMP_DATA_LEN  needs to be updated.
typedef struct
{
  qsh_dump_tag_hdr_s         hdr;
  gl1_dump_tag_data_s        gl1_dump_data;
}gl1_qsh_dump_tag_data_s;


typedef gl1_qsh_dump_tag_data_s gl1_qsh_dump_tag_data_as_id_1_s;
typedef gl1_qsh_dump_tag_data_s gl1_qsh_dump_tag_data_as_id_2_s;

// Pointer declared as type of QSH data structure for parser
gl1_qsh_dump_tag_data_s  *gl1_qsh_dump_tag_0_ptr;
gl1_qsh_dump_tag_data_s  *gl1_qsh_dump_tag_1_ptr;



/* ----------------  Definitions for data metrics types  ------------------ */

/* --------------------  Definitions for QSH Metrics  --------------------- */

typedef enum
{
  GL1_QSH_METRIC_RSSI            = 0,
  GL1_QSH_METRIC_RACH_TX_PWR     = 1,
  GL1_QSH_METRIC_TCH_RX_PARAMS   = 2,
  GL1_QSH_METRICS_MAX
} gl1_qsh_metric_e;


#define GL1_QSH_METRIC_RSSI_LEN              4
#define GL1_QSH_METRIC_RACH_ATTEMPT_LEN      4
#define GL1_QSH_METRIC_RACH_FAILURE_LEN      4
#define GL1_QSH_METRIC_RSSI_LEN_LEN          4


// GL1_QSH_METRIC_RSSI structure
typedef struct
{
  qsh_metric_hdr_s       hdr;

  int32                  avg_rssi;  // ag rssi over 100ms  
} gl1_qsh_metric_rssi_s;

// GL1_QSH_METRIC_RACH_TX_PWR structure
typedef struct
{
  qsh_metric_hdr_s       hdr;
  int16                  rach_tx_pwr;
} gl1_qsh_metric_rach_tx_pwr_s;

/* GL1_QSH_METRIC_TCH_RX_PARAMS Structure */
typedef struct
{
  qsh_metric_hdr_s      hdr;

  /*TCH RxLev Full and Sub on Primary Antenna in dBx16*/
  int32                 tch_pri_rxlev_full;
  int32                 tch_pri_rxlev_sub;
  /*TCH RxLev Full and Sub on Secondary Antenna in dBx16*/
  int32                 tch_sec_rxlev_full;
  int32                 tch_sec_rxlev_sub;
  /*TCH RxQual Full and Sub (Combined , not Antenna Specific)*/
  uint8                 tch_rxqual_full;
  uint8                 tch_rxqual_sub;
} gl1_qsh_metric_tch_rx_params_s;

#ifdef GFW_QSH_NOT_REQ_ON_JO
// GFW_QSH_DUMP_DATA_LEN  needs to be updated.
typedef struct
{
  qsh_dump_tag_hdr_s         hdr;
  gfw_qsh_dump_s             gfw_dump_data;
}gfw_qsh_dump_tag_data_s;
#endif

#endif /* GL1_QSH_EXT_H */
/* EOF */
