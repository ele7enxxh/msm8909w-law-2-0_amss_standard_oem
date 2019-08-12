/*!
  @file
  intf_prs_fd_logging_types.h

  @brief
  PRS/CRS FD buffer logging types
*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03-26-15   nbonda  Created the file, /lte/api/lte_LL1_prs_fd_logging_types.h
===========================================================================*/

#ifndef LTE_LL1_PRS_FD_LOGGING_TYPES
#define LTE_LL1_PRS_FD_LOGGING_TYPES

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_l1_types.h"
#include "intf_prs.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_LL1_PRS_FD_LOGGING_MAX_RX_NUM 2

/*===========================================================================

                 None template related typedefs 

===========================================================================*/

/*!@brief HW scheduling per cell information */
typedef struct
{

  /*! Cell physical id */
  int16 cell_id;

  /*! current symbol offset; valid range [-1, +7]*/
  int16 sym_offset;

  /*! Number of PRS subframe */
  int16 n_prs;

  /*! Cell PRS bandwidth in RB */
  int16 prs_rb;

  /*! 1 or 2 pbch ants, 4 pbch ports; do we need consider
      other mapping for the multiple timing hypos */
  lte_LL1_prs_port_map_e  port_map; 

  /*! start_index */
  uint32 prune_start_idx;

  /*! win_size */
  uint32 prune_win_size;

  /*! Number of PRS symbol */
  int16 n_prs_sym;

  /*! PRS per-cell gain bias */
  int8 agc_bias;

  /*! fd sym index */
  uint8 fd_sym_idx;

  /*! COM based win adj value used to adjust prune window start index */
  int16 win_adj;

}lte_LL1_prs_fd_logging_cell_s; 

/*!@brief Per subframe scheduling record structure */
typedef struct
{
  /*! the frame and subframe number of this hw scheduling */
  uint16 frame_subframe;

}lte_LL1_prs_fd_logging_sf_record_s;


/*!@brief PRS HW module configuration structure */
typedef struct
{
  /*! the DL bandwidth in RB */
  uint32 dl_rb;

  /*! DL bw enum, to avoid unnecessary conversions */
  lte_bandwidth_enum dl_bandwidth;

  /*! The serving cell tx number */
  int8 num_tx_ant;

  /*! The serving cell CP type */
  lte_l1_cp_e cp_mode;

  /*! The serving cell id (for vshift) */
  int16 cell_id;
  
  /*! Flag indicating if new fd dma is needed */
  boolean need_fd_dma;
  
}lte_LL1_prs_fd_logging_config_s;

/*!@brief prs uk config state machine */
typedef struct
{
  /*! The starting subframe frame number of the current occasion */
  int16 occ_start_frame_subframe;

  /*! number of prs subframes,maximum number is 6 */
  uint8 num_prs_sf;

  /*! cell db */
  lte_LL1_prs_fd_logging_cell_s cells;

}lte_LL1_prs_fd_logging_meas_stm_t;

/*! @brief PRS FD DMA state machine
*/
typedef struct
{
  int32                                fd_dma_schd_cnt;       // Range: 0~5. Index of FD DMA (up to max n_prs, which is 6) 
  int32                                fd_dma_done_cnt;
  int32                                fd_dma_schd_sf_log[LTE_LL1_PRS_NUM_SF_MAX];
  uint16                               earliest_cell_idx;     // index of cell with earliest symbol
  uint32                               num_sym_to_xfer;       // Number of symbols to xfer in a subframe. 
  uint16                               curr_sf_prs_fd_idx[LTE_LL1_PRS_NUM_CELL_MAX];    // FD buffer index of the earlist PRS symbol in current SF                                
} lte_LL1_prs_fd_logging_fd_dma_stm_t;

/*! @brief PRS_SYM_PROC rotation params for a given cell, computed in 
           function lte_LL1_prs_util_compute_rot_params.
*/
typedef struct
{
  /*! current COM cumulative adjustment in Ts units  */
  uint32  com_adj_curr;
  /*! PRS_SYM_PROC rotator DC step parameter */
  uint32  rot_dc_step;
  /*! PRS_SYM_PROC rotator frequency parameter */
  uint32  rot_freq;
  /*! PRS_SYM_PROC rotator phase parameter */
  uint32  rot_phase;

} lte_LL1_prs_fd_logging_rot_params_sf_s;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  /*! Rotation params for PRS_SYM_PROC ukernel */
  lte_LL1_prs_fd_logging_rot_params_sf_s sf[LTE_LL1_PRS_FD_LOGGING_MAX_RX_NUM][LTE_LL1_PRS_NUM_SF_MAX];

} lte_LL1_prs_fd_logging_rot_params_cell_s;

/*! @brief PRS SYM_PROC Rotator Parameters
*/
typedef struct
{
  lte_LL1_prs_fd_logging_rot_params_cell_s cell[LTE_LL1_PRS_NUM_CELL_MAX];

} lte_LL1_prs_fd_logging_rot_params_s;

typedef union 
{ 
  struct 
  {
     uint32 ncoh_scale : 8; /* Word 0 */
     uint32 reserved1 : 8; /* Word 0 */
     uint32 sym_start_offset : 16; /* Word 0 */
  } ; 

  /* Union of all the above */
  uint32 word_arr[1];
  uint32 word_val;
} lte_LL1_prs_fd_logging_sub_frame_proc_word_0_t;

typedef union 
{ 
  struct 
  {
     uint32 cp_type : 1; /* Word 0 */
     uint32 num_crs_port_sel : 1; /* Word 0 */
     uint32 coh_ncoh_acc_mode : 1; /* Word 0 */
     uint32 acc_init : 1; /* Word 0 */
     uint32 ca_sel : 1; /* Word 0 */
     uint32 op_en : 1; /* Word 0 */
     //uint32 fmt_sel : 1; /* Word 0 */
     uint32 reserved2 : 9; /* Word 0 */
     uint32 prune_win_size : 16; /* Word 0 */
  } ; 

  /* Union of all the above */
  uint32 word_arr[1];
  uint32 word_val;

} lte_LL1_prs_fd_logging_sub_frame_proc_word_1_t;

typedef union 
{ 
  struct 
  {
     uint32 rx0_scale : 16; /* Word 0 */
     uint32 reserved2 : 16; /* Word 0 */
  } ; 

  /* Union of all the above */
  uint32 word_arr[1];
  uint32 word_val;

} lte_LL1_prs_fd_logging_occasion_proc_word_0_t;


typedef union 
{ 
  struct 
  {
     uint32 rx1_scale : 8; /* Word 0 */
     uint32 reserved2 : 24; /* Word 0 */
  } ; 

  /* Union of all the above */
  uint32 word_arr[1];
  uint32 word_val;

} lte_LL1_prs_fd_logging_occasion_proc_word_1_t;

typedef union 
{ 
  struct 
  {
     uint32 bias : 8; /* Word 0 */
     uint32 reserved3 : 24; /* Word 0 */

  } ; 

  /* Union of all the above */
  uint32 word_arr[1];
  uint32 word_val;

} lte_LL1_prs_fd_logging_occasion_proc_word_2_t;


typedef union 
{ 
  struct 
  {
     uint32 coh_ncoh_comb_mode : 1; /* Word 0 */
     uint32 ca_sel : 1; /* Word 0 */
     uint32 op_en : 1; /* Word 0 */
     uint32 reserved4 : 29; /* Word 0 */
  } ; 

  /* Union of all the above */
  uint32 word_arr[1];
  uint32 word_val;

} lte_LL1_prs_fd_logging_occasion_proc_word_3_t;

typedef struct{
  uint32 sys_bw : 4;
  uint32 max_prs_bw : 4;
  uint32 num_rx_ant : 4;
  uint32 sym_en_bitmask : 14;
  uint32 num_symbol_xfered : 4;
  uint32 reserved : 2;
  uint32 dc_step : 16;
  uint32 reserved2 : 16;
}lte_LL1_prs_fd_logging_core_common_cfg_t;

typedef struct{
    
  uint32 prune_win_size : 16;
  uint32 prune_win_start_index : 16;
  uint32 prs_bw : 16;
  uint32 sym_start_offset : 16;
  uint32 bias : 8;
  uint32 cp_type : 1;
  uint32 num_crs_port_sel : 1;
  uint32 coh_ncoh_acc_mode : 1;
  uint32 ca_sel : 2;
  uint32 proc_mode : 1;

#define LTE_LL1_PRS_CELL_IC_DISABLE  0
#define LTE_LL1_PRS_CELL_IC_ENABLE  1
  // Any cells enabled AFTER a cell with ic_enabled will potentially be using
  // the alterned FD tone buffer. Hence cells after an offending IC cell must be 
  // either offended cells or cells that use none of the modified FD tones, i.e.
  // it's v-shift / symbol offset combination will land it on un-modified tones.
  uint32 ic_enable : 1;

  uint32 cell_id_mod6;
}lte_LL1_prs_fd_logging_core_cell_cfg_t;

typedef struct {
  lte_LL1_prs_fd_logging_core_common_cfg_t common_cfg;
  lte_LL1_prs_fd_logging_core_cell_cfg_t cell_cfg[LTE_LL1_PRS_NUM_CELL_MAX];
} lte_LL1_prs_fd_logging_core_common_api_t;

typedef union 
{ 
  struct 
  {
     uint32 coh_scale;/* Word 0 */
     lte_LL1_prs_fd_logging_sub_frame_proc_word_0_t word_0; /* Word 1 -- Word 1 */ 
     uint32 prune_win_start_index : 16; /* Word 4 */
     uint32 prs_bw : 16; /* Word 4 */
     lte_LL1_prs_fd_logging_sub_frame_proc_word_1_t word_1; /* Word 5 -- Word 5 */ 
     uint32 sys_bw : 16; /* Word 6 */
     uint32 num_rx_ant : 16; /* Word 6 */
     uint32 bias : 8;      /* Word 7 */
     uint32 reserved : 24; /* Word 7 */
  };
  /* Union of all the above */
  uint32 word_arr[6];
} lte_LL1_prs_fd_logging_sub_frame_proc_t;

typedef union 
{ 
  struct 
  {
     uint32 freq;/* Word 0 */
     uint32 phase;/* Word 1 */
     uint32 desr_init_st2;/* Word 2 */
     uint32 v_shift : 16; /* Word 3 */
     uint32 unused : 16; /* Word 3 */
     uint32 smooth_scale;/* Word 4 */
  } ; 

  /* Union of all the above */
  uint32 word_arr[5];
} lte_LL1_prs_fd_logging_sub_frame_proc_sym_cfg_t;

typedef struct {
  lte_LL1_prs_fd_logging_sub_frame_proc_t prs_sub_frame_proc[LTE_LL1_PRS_NUM_CELL_MAX]; /* Word 606 -- Word 629 */ 
  lte_LL1_prs_fd_logging_sub_frame_proc_sym_cfg_t prs_sub_frame_proc_sym_cfg[LTE_LL1_PRS_NUM_CELL_MAX][8]; /* Word 630 -- Word 789 */  
} lte_LL1_prs_fd_logging_core_api_t;

typedef union 
{ 
  struct 
  {
     lte_LL1_prs_fd_logging_occasion_proc_word_0_t word_0; /* Word 0 -- Word 0 */ 
     lte_LL1_prs_fd_logging_occasion_proc_word_1_t word_1; /* Word 1 -- Word 1 */ 
     //lte_LL1_prs_fd_logging_occasion_proc_word_2_t word_2; /* Word 2 -- Word 2 */ 
     uint32 prs_bw : 16; /* Word 3 */
     uint32 coh_ncoh_acc_state_vec_len : 16; /* Word 3 */
     lte_LL1_prs_fd_logging_occasion_proc_word_3_t word_3; /* Word 4 -- Word 4 */ 
  } ; 

  /* Union of all the above */
  uint32 word_arr[5];
} lte_LL1_prs_fd_logging_occasion_proc_t;

typedef struct {
  lte_LL1_prs_fd_logging_occasion_proc_t prs_occasion_proc[LTE_LL1_PRS_NUM_CELL_MAX]; /* Word 790 -- Word 809 */ 
}lte_LL1_prs_fd_logging_core_occasion_api_t;

/*!@brief structure for L1/L0 cfg for one PRS occasion*/
typedef struct
{
  lte_LL1_prs_fd_logging_core_common_api_t     core_common;
  lte_LL1_prs_fd_logging_core_api_t            sym_sf_cfg[LTE_LL1_PRS_NUM_SF_MAX];
  lte_LL1_prs_fd_logging_core_occasion_api_t   oc_cfg;
}lte_LL1_prs_fd_logging_meas_core_api_cfg_t;

typedef union {
  struct {
    uint32 op_en                          :  1;  /* W[0] [ 0: 0] */
    uint32 reserved0                      : 31;  /* W[0] [31: 1] */
    uint32 carrier_idx                    :  2;  /* W[1] [ 1: 0] */
    uint32 reserved1_1                    : 14;  /* W[1] [15: 2] */
    uint32 first_ant_idx                  :  3;  /* W[1] [18:16] */
    uint32 reserved1_2                    :  5;  /* W[1] [23:19] */
    uint32 second_ant_idx                 :  3;  /* W[1] [26:24] */
    uint32 reserved1_3                    :  5;  /* W[1] [31:27] */
    uint32 mnoc_first_ant_dest_addr       : 30;  /* W[2] [29: 0] */
    uint32 reserved2                      :  2;  /* W[2] [31:30] */
    uint32 mnoc_second_ant_dest_addr      : 30;  /* W[3] [29: 0] */
    uint32 reserved3                      :  2;  /* W[3] [31:30] */
    uint32 mnoc_sym_size                  : 30;  /* W[4] [29: 0] */
    uint32 reserved4                      :  2;  /* W[4] [31:30] */
    uint32 start_word_offset_in_sym       : 10;  /* W[5] [ 9: 0] */
    uint32 reserved5_0                    :  6;  /* W[5] [15:10] */
    uint32 xfer_len_per_sym               : 10;  /* W[5] [25:16] */
    uint32 reserved5_1                    :  6;  /* W[5] [31:26] */
    uint32 sys_sym_size                   :  3;  /* W[6] [ 2: 0] */
    uint32 reserved6                      : 29;  /* W[6] [31: 3] */
    uint32 start_fd_sym_buf_idx           :  7;  /* W[7] [ 6: 0] */
    uint32 reserved7_0                    :  1;  /* W[7] [ 7: 7] */
    uint32 start_prs_sym_idx              :  4;  /* W[7] [11: 8] */
    uint32 reserved7_1                    :  4;  /* W[7] [15:12] */
    uint32 num_xfer_sym                   :  4;  /* W[7] [19:16] */
    uint32 reserved7_2                    : 12;  /* W[7] [31:20] */
  };

  uint32 word_arr[8];

} lte_LL1_prs_fd_logging_cchp_cfg_t;

/*!@brief structure for Per SF configuration/flags for PRS hw schd */
typedef struct
{
  /*! FD DMA STM */
  lte_LL1_prs_fd_logging_cchp_cfg_t  fd_dma_prs_cfg_tpl;
  boolean                    dma_done;
  boolean                    tpl_done;
  boolean                    sem_raised;
  uint32                     dest_ddr_addr_rx0;
  uint32                     dest_ddr_addr_rx1;
}
lte_LL1_prs_fd_logging_per_sf_cfg_s;

/*!@brief hw scheduling database */
typedef struct
{
  /*! Common Info */
  uint8                          num_cells; // Number of prs cells; maximum number is 10 for Thor 
  uint32                         uk_prs_rb; // ukernal prs bandwidth 
  lte_LL1_prs_fd_logging_config_s   cfg;       // PRS static config 

  /*! Per SF Structures */
  lte_LL1_prs_fd_logging_per_sf_cfg_s per_sf_cfg[LTE_LL1_PRS_NUM_SF_MAX];

  /*! FD DMA STM */
  lte_LL1_prs_fd_logging_fd_dma_stm_t dma_stm;

  /*! Measurement STM */
  lte_LL1_prs_fd_logging_meas_stm_t meas_stm[LTE_LL1_PRS_NUM_CELL_MAX];

  /*! PRS dmem cfg structure for one PRS occasion    */
  lte_LL1_prs_fd_logging_meas_core_api_cfg_t core_cfg;

  /*! maximum number of prs subframes among all the cells,maximum number is 6 */
  uint8 max_nprs;

  /*! earliest symbol index for all cells */
  uint32 earliest_fd_sym_idx;
  uint32 earliest_cell_idx;
  
  /*! PRS COM adjustment and Rotation Params */
  lte_LL1_prs_fd_logging_rot_params_s rot_params;

} lte_LL1_prs_fd_logging_db_s;


#endif /* LTE_LL1_PRS_FD_LOGGING_TYPES */
