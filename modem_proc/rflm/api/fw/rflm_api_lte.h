/*!
  @file
  rflm_api_lte.h

  @brief
  RF LM API definitions for LTE
  
  @detail
  
  
  @todo
 

*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_lte.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/15   stw     Chain mask per carrier added
01/14/15   pkg     Interface changes for Script based Tx Tune.
11/24/14    kg     Added new FW FED apis for IRAT scenarios
11/14/14    pv     Increased Rx path to support more secondary paths for HORxD
10/27/14   cri     TxAGC optimizations - Interface for split txagc
10/23/14   kab     Support negative LNA Offset
10/07/14   kab     0xB13B Log packet support for SGLTE+G
09/26/14   bsh     Support new API to check RF status
10/03/14   kab     Enable Rx Tune Scripts Execution by FW
09/30/14   gvn     3xCA gaps support
09/30/14   kab     RF-FW Interface changes for FW Triggered RF Tune
09/29/14   kab     Update DM with Rx LO spacing from Carrier Center Frequency
09/17/14   bsh     support rx enable/disable
09/11/14   vb      RSB/IQMC per LNA gain state support
08/18/14   kab     TxPL changes to support SGLTE Tx power backoff with Coex.
08/12/14   vb      NLIC clock mode support 
08/01/14   sk      Remove Common Txagc Seq creation in non-cal mode for LTE
07/30/14   gvn     Intra-band fixes: pass force_lna flag to fw when intra-status changes
07/02/14   jf      AsDiv support
06/18/14   sbm     Added support for WTR HDET
06/13/14   kab     Add 3xCA support
05/08/14   bsh     Added new "nlic_mode" to NLIC return params to FW
04/03/14   cri     Interface updates for TxDAC and DRIF
03/20/14    pv     FW intf change: Symbol level FBRx trigger 
03/14/14   bsh     NlIC changes: code clean up 
02/26/14   cri     Align modulation type with FW enum to resolve mismatch
02/24/14    pv     Interface update with correct datatype.
02/18/14    pv     Added Support for FBRx for RB based Gain Comp.
02/13/14   cri     Wmore clipper interface
02/13/14    pv     Added Support for FBRx trigger for MTPL.
02/13/14    ka     Added fbrx cal update API
02/05/14   kab     API to support Tx_ON/EN writes during DL CP
01/08/13   bsh     Fix compiler warning in RFLM
01/06/14   bsh     RxAGC support for BOLT
12/20/13    ka     Added fbrx params to tx agc api
12/13/13   svi     Added FW to pass Frame Number to FED per SF
12/09/13   cri     Added kernel term Nl2 to NLIC interface
11/06/13   svi     Added LNA Gainword transaction macro
10/30/13    ka     Updated for latest FBRx uK APIs
10/31/13   pl      IRAT script execution implementation
10/30/13   svi     Added Subframe type to PLL AFC payload
10/24/13   cri     Initial programming of digital settings. Iref and IQ gain.
10/02/13   hdz     Added lte api verison number
09/19/13   jf      Add CELL_SRS UL type
09/11/13   nrk     Add FED RxAGC support
08/20/13   jf      Add FBRX support
07/30/13   jf      Initial Version
==============================================================================*/

#ifndef RFLM_API_LTE_H
#define RFLM_API_LTE_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_cmn.h"
#include "rflm_api_fbrx_fw.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief Version of the rflm lte module.
*/
#define RFLM_API_LTE_MAJOR_VERSION            2
#ifdef FEATURE_LTE_HORXD
#error code not present
#else
#define RFLM_API_LTE_MINOR_VERSION            8
#endif
#define RFLM_API_LTE_VERSION    \
              ((RFLM_API_LTE_MAJOR_VERSION << 16) | RFLM_API_LTE_MINOR_VERSION )


/* Tx Antenna Indices for TM8 SRS Ant Swap */
#define RFLM_LTE_TX_ANT_0 0
#define RFLM_LTE_TX_ANT_1 1

#define RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL         4
#define RFLM_LTE_MAX_NUM_RX_GAIN_RANGES	   			     6
#define RFLM_LTE_DEFAULT_LNA_GAIN_STATE              2 /* Default LNA gain for LTE Tech*/
#define RFLM_LTE_MAX_NUM_CARRIER                     3
#define RFLM_LTE_MAX_NUM_DTR_SETTINGS                4  
#define RFLM_LTE_MAX_NUM_CELL                        1                  /*Was 2 earlier, changed it to 1 for memory reduction*/
#define RFLM_LTE_MAX_NUM_SF                         10
#define RFLM_LTE_MAX_SF_BUFFER                      1                   /*Was 2 earlier, changed it to 1 for memory reduction*/
#define RFLM_LTE_NUM_SF_FOR_INTERFACE               RFLM_LTE_MAX_NUM_SF *\
                                                    RFLM_LTE_MAX_SF_BUFFER
/*! Max number of LNA Gain word (GW) SSBI to program to tune to a Carrier. 
    This is to incorporate LNA + PLL split for CA on WTR1625 + WFR1620 */
#define RFLM_LTE_MAX_LNA_GW_TRANSACTION_PER_PATH    2

/* This is in MHz */
#define RFLM_LTE_SAMPLING_FREQ_FOR_10MHz_AND_BELOW 17200000
#define RFLM_LTE_SAMPLING_FREQ_FOR_ABOVE_10MHz     34400000

#define RFLM_LTE_EXECUTE_TUNE_SCRIPTS_TX0_HANDLE       0
#define RFLM_LTE_EXECUTE_TUNE_SCRIPTS_TX1_HANDLE       RFLM_LTE_EXECUTE_TUNE_SCRIPTS_TX0_HANDLE + 1
#define RFLM_LTE_EXECUTE_TUNE_SCRIPTS_PRX_HANDLE       RFLM_LTE_EXECUTE_TUNE_SCRIPTS_TX1_HANDLE + 1
#define RFLM_LTE_EXECUTE_TUNE_SCRIPTS_DRX_HANDLE       RFLM_LTE_EXECUTE_TUNE_SCRIPTS_PRX_HANDLE + 1
#define RFLM_LTE_EXECUTE_TUNE_SCRIPTS_DRX2_HANDLE      RFLM_LTE_EXECUTE_TUNE_SCRIPTS_DRX_HANDLE + 1
#define RFLM_LTE_EXECUTE_TUNE_SCRIPTS_DRX3_HANDLE      RFLM_LTE_EXECUTE_TUNE_SCRIPTS_DRX2_HANDLE + 1
#define RFLM_LTE_EXECUTE_TUNE_SCRIPTS_MAX_HANDLE       RFLM_LTE_EXECUTE_TUNE_SCRIPTS_DRX3_HANDLE + 1

typedef enum
{
  RFLM_LTE_RX_PATH_PRIMARY = 0,
  RFLM_LTE_RX_PATH_SECONDARY_0,
#ifdef FEATURE_LTE_HORXD
  #error code not present
#endif
  /* Scalable for future use (3 way RX div) by adding RFLM_LTE_RX_PATH_SECONDARY_1 */
  RFLM_LTE_MAX_NUM_RX_PATHS 
}rflm_lte_rx_path_type;

typedef enum
{
   RFLM_LTE_UL_CMD_PRACH = 0,
   RFLM_LTE_UL_CMD_PUSCH,             
   RFLM_LTE_UL_CMD_PUCCH,                               
   RFLM_LTE_UL_CMD_SRS,
   RFLM_LTE_UL_CMD_CELL_SRS,                  
   RFLM_LTE_UL_CMD_SRS_UPPTS,             
   RFLM_LTE_UL_CMD_PUCCH_PUSCH,           
   RFLM_LTE_UL_CMD_DTX,                   
   RFLM_LTE_UL_CMD_DAC_DTX,                 
   RFLM_LTE_UL_CHAN_TYPE_COUNT,           
   RFLM_LTE_UL_CMD_MAX = 0xFF           
} rflm_lte_ul_chan_type_e;

typedef enum
{
  RFLM_LTE_MC_CELL0,
  RFLM_LTE_MC_CELL1,
  RFLM_LTE_MC_CELL2,
  RFLM_LTE_MC_CELL3,
  RFLM_LTE_MC_CELL4,
  RFLM_LTE_MC_CELL5,
  RFLM_LTE_MC_CELL6,
  RFLM_LTE_MC_CELL7,
  RFLM_LTE_MC_INVALID_CELL
}rflm_lte_cell_id_e;

typedef enum
{
    RFLM_LTE_BW_1P4        = 0,
    RFLM_LTE_BW_3          = 1,
    RFLM_LTE_BW_5          = 2,
    RFLM_LTE_BW_10         = 3,
    RFLM_LTE_BW_15         = 4,
    RFLM_LTE_BW_20         = 5,
    RFLM_LTE_BW_40         = 6,
    RFLM_LTE_NUM_OF_BW  
}rflm_lte_bw_type_s;


typedef enum
{
    RFLM_LTE_PRACH      = 0,
    RFLM_LTE_SLOT1      = 0,
    RFLM_LTE_SLOT2      = 1,
    RFLM_LTE_SRS1       = 2,
    RFLM_LTE_SRS2       = 3
}rflm_lte_slot_type_s;

typedef enum
{
    RFLM_LTE_MOD_BPSK      = 0,
    RFLM_LTE_MOD_QPSK     = 1,
    RFLM_LTE_MOD_16QAM      = 2,
    RFLM_LTE_MOD_64QAM     = 3,
    RFLM_LTE_NUM_OF_MOD 
}rflm_lte_mod_type_s;	


typedef enum
{
  RFLM_LTE_EVENT_TX_ON_UL_SF,                   /* FDD */
  RFLM_LTE_EVENT_TX_ON_PRACH_OR_SRS_ONLY,       /* FDD */
  RFLM_LTE_EVENT_TX_CONT_UL_SF,                 /* FDD and TDD */
  RFLM_LTE_EVENT_TX_CONT_PRE_UE_SRS,            /* FDD and TDD */
  RFLM_LTE_EVENT_TX_CONT_POST_UE_SRS,           /* FDD and TDD */
  RFLM_LTE_EVENT_TX_OFF_NULL_SF,                /* FDD */
  RFLM_LTE_EVENT_TX_OFF_CELL_SRS,               /* FDD */
  RFLM_LTE_EVENT_TDD_TX_ON_UL_SF,               /* TDD */
  RFLM_LTE_EVENT_TDD_TX_ON_PRACH_OR_SRS_ONLY,   /* TDD */
  RFLM_LTE_EVENT_TDD_TX_OFF_NULL_SF,            /* TDD */
  RFLM_LTE_EVENT_TDD_TX_OFF_CELL_SRS,           /* TDD */
  RFLM_LTE_EVENT_ID_MAX
} rflm_lte_event_id_type;

/* Mask for Rx Config Script Execution */
#define RFLM_LTE_ACTION_RX_CONFIG_PRELOAD_MASK RFLM_LTE_IRAT_ACTION_PRELOAD_MASK
#define RFLM_LTE_ACTION_RX_CONFIG_TRIGGER_MASK RFLM_LTE_IRAT_ACTION_TRIGGER_MASK
#define RFLM_LTE_ACTION_RX_CONFIG_NON_TRX_MASK RFLM_LTE_IRAT_ACTION_NON_TRX_MASK

#define RFLM_LTE_MAX_NUM_HANDLES 6


typedef enum
{
  RFLM_LTE_TX_HANDLE,
  RFLM_LTE_RX_HANDLE,
  RFLM_LTE_MAX_HANDLE_TYPE
}rflm_lte_handle_type_e;

typedef enum
{
   RFLM_LTE_TX_ON = 0,
   RFLM_LTE_TX_OFF,             
   RFLM_LTE_TX_EN,
   RFLM_LTE_TX_DISABLE,
   RFLM_LTE_TXAGC_MAX_TX_ON_EN_TYPE
} rflm_lte_tx_on_en_type;

typedef enum
{
  RFLM_LTE_BEFORE_SYMBOL,
  RFLM_LTE_AFTER_SYMBOL
}rflm_lte_symbol_boundary_selection_type_s;

typedef enum
{
  RFLM_LTE_MC_CELL0_ONLY,
  RFLM_LTE_MC_CELL1_ONLY,
  RFLM_LTE_MC_CELL0_AND_CELL1
}rflm_lte_active_cell;

typedef enum
{
   RFLM_LTE_TX0 = 0,  
   RFLM_LTE_TX1,
   RFLM_LTE_PRX,
   RFLM_LTE_DRX,
   RFLM_LTE_DRX2,
   RFLM_LTE_DRX3,
   RFLM_LTE_PATH_MAX         
} rflm_lte_path_id_e;


typedef struct
{
  boolean wmore_clipper_en;
  uint32 clipper_thr_0;
  uint32 clipper_thr_1;
}rflm_lte_wmore_clipper_t;

/*==============================================================================

                     Input/Output structure definitions
  
==============================================================================*/

/*==============================================================================
                TXAGC related input/output structure definitions
==============================================================================*/

/*! @brief rflm_lte_update_tx_sf_input_s is the input from FW to schedule TXAGC 
  per SF per cell
  -	ul_subframe_type: Uplink command type
  -	cell_index: PCELL/SCELL
  -	slot_active: True if the slot is active. Each element in array corresponds
                 to one slot, i.e. Slot0, Slot1, SRS 1 and SRS 2
  -	sf_num: Subframe number (0 - 9)
  -	num_rbs: Number of RBs for each slot. 
  -	start_rbs: Start RB index for each slot. 
  -	action_time: Action time for each slot. The unit is USTMR 
  -	rflm_lte_pwr_db10: Desired Tx Power in dB * 100 per slot.  
  -	bw_type: Bandwidth type for the SF
  -	mod_type: Modulation type (QPSK, 16QAM, BPSK, 64QAM)
  - tx_ant_index: ant swap index for TX
  - drif_group_a: Defines the DRIF group (A or B)
  - afc_sf_active: Flags if AFC correction is needed for this SF
  - afc_action_time: The time to enable AFC for this SF
  - tx_pwr_backoff: Flag determines if Tx Power Backoff (Co-EX) has to be applied
*/
typedef struct
{
  rflm_lte_ul_chan_type_e        ul_subframe_type;            
  rflm_lte_cell_id_e             cell_index;
  boolean                        slot_active[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  uint8                          sf_num;
  uint16                         num_rbs[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];          
  uint16                         start_rbs[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];                                                                                       
  uint32                         action_time[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];      
  int32                          rflm_lte_pwr_db10[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];    
  rflm_lte_bw_type_s             bw_type;
  rflm_lte_mod_type_s            mod_type[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  uint8                          tx_ant_index[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  uint8                          drif_group_idx;
  boolean                        afc_sf_active;
  uint32                         afc_action_time;
  uint32                         frame_num;
  boolean                        fbrx_available;
  int32                          action_time_tx_on_en[RFLM_LTE_TXAGC_MAX_TX_ON_EN_TYPE];
  boolean                        tx_dac_active;
  boolean                        tx_pwr_coex_backoff; 
}rflm_lte_update_tx_sf_input_s;

typedef struct
{
  /* Timing Offset based on Timing advance and DL CP symbol location.*/
  int32                                     ul_timing_offset_wrt_dl[RFLM_LTE_TXAGC_MAX_TX_ON_EN_TYPE];
  /* Obtain timing of DL CP symbol either before or after "Time" calculated using Timing advance
   * and fixed timing offset. */
  rflm_lte_symbol_boundary_selection_type_s start_stop_symbol_boundary[RFLM_LTE_TXAGC_MAX_TX_ON_EN_TYPE]; 
}rflm_lte_ul_dl_timing_delta_s;


/*! @brief rflm_lte_update_tx_sf_return_s is the output from FED to FW 
  per SF per cell
  -	txagc_success: Flag successful run of TxAGC
  - wmore_backoff: Wmore digital backoff setting
  - drif_settings: DRIF configuration (Iref, DAC settings, and LUT DMA info)
  - run_fbrx: Represent the symbol on which fbrx needs to be triggered. Max total symbols 
    which can be high are 14 including SRS symbol.

*/
typedef struct
{  
  boolean                         txagc_success[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  int16                           wmore_backoff[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  rflm_drif_out_cfg_t             drif_settings[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  rflm_fbrx_dynamic_param_t       fbrx_dynamic_params[RFLM_FBRX_LTE_MAX_RUNS_PER_SF];
  rflm_fbrx_pwr_dependent_param_t fbrx_pwr_dep_params[RFLM_FBRX_LTE_MAX_RUNS_PER_SF];
  uint16                          run_fbrx; /* 16 bits captures each symbol within a SF */
  rflm_lte_wmore_clipper_t        wmore_clipper[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  uint8                           drif_index[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
#if !defined FEATURE_THOR_MODEM && !defined FEATURE_JOLOKIA_MODEM
  int32                           mtpl;
#endif
}rflm_lte_update_tx_sf_return_s;

/*! @brief rflm_lte_execute_tx_tune_input_s is a call from FW to execute the 
  tx retune script for UL CA.
 
  - cell_index: PCELL/SCELL
  - action_time: when to execute the script
*/
typedef struct
{          
  rflm_lte_cell_id_e cell_index;
  uint32             action_time;
}rflm_lte_execute_tx_tune_input_s;

typedef struct
{        
  uint32             reserved;  
}rflm_lte_execute_tx_tune_return_s;

/*==============================================================================
                FBRX related input/output structure definitions
==============================================================================*/

typedef struct
{
  uint8                          sf_num;
  rflm_lte_active_cell           active_cell;
}rflm_lte_fbrx_input_s;

typedef struct
{
 boolean                 fbrx_on[RFLM_LTE_MAX_NUM_CELL][RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
 int32                   capture_length[RFLM_LTE_MAX_NUM_CELL][RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
 int32                   scale_val[RFLM_LTE_MAX_NUM_CELL][RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
}rflm_lte_fbrx_return_s;

/*==============================================================================
                RXAGC related input/output structure definitions
==============================================================================*/

/*! @brief rflm_lte_update_rx_sf_input_s is in the input structure for FW 
  to pass RFLM with RX settings per SF 
  -	chain_enable_mask:  Each bit corresponds to a path.
                        Bit0 = PCELL path0,
                        Bit1 = PCELL path1,
                        Bit2 = SCELL path0,
                        Bit3 = SCELL path1
  - rx_info: It is an 2D array of structure. First dimension corresponds
  to number of carriers (PCELL and SCELL), second dimension corresponds
  to number of rx paths (rx0, rx1) in each carrier. Each structure has
  the following elements
                       total_rssi_cBm (int32)
                       inband_rssi_cBm (int32)
                       rx_gain_state
  -	action time: 
  -	subframe number:
*/

typedef struct
{
  int32             total_rssi_cBm;
  int32             inband_rssi_cBm;
  int8              gain_state;
  rflm_handle_rx_t  handle_id;
}rflm_lte_rx_info_s;

typedef struct
{
  uint8                chain_enable_mask; 
  rflm_lte_rx_info_s   rx_info[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_MAX_NUM_RX_PATHS];
  uint8                sf_num;
  boolean              irat_mode;
  uint8                carr_chain_enable_mask[RFLM_LTE_MAX_NUM_CARRIER];
}rflm_lte_update_rx_sf_input_s;

typedef struct
{
  int16      gain_offset;        
  int8       gain_state; 
  boolean    success;
  boolean    force_lna_update; /* tell FW to trigger LNA settings in case intra-band status changes */
}rflm_lte_rx_return_info_s;

typedef struct
{
  rflm_lte_rx_return_info_s rx_return_info[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_MAX_NUM_RX_PATHS];
}rflm_lte_update_rx_sf_return_s;

/* ------------------------------------------------------------------------- */

typedef enum 
{
  RFLM_LTE_RXAGC_SHARED_MEM_STANDALONE=0,
  RFLM_LTE_RXAGC_SHARED_MEM_NCELL=1,
  RFLM_LTE_RXAGC_SHARED_MEM_MAX_COUNT=2
} rflm_lte_rxagc_shared_mem_source_e;

typedef struct
{
uint8 carrier_idx;
uint8 path;
uint8 gain_state;
uint8 sf_num; 
rflm_lte_rxagc_shared_mem_source_e smem_src;
boolean irat_mode;
}rflm_lte_get_lna_settings_input_s;

typedef struct
{
  int16     rx_gain_offset; // unit is dB/10
} rflm_lte_get_lna_settings_return_s;

typedef struct
{
uint8 carrier_idx; 
uint8 path;
uint8 gain_state;
uint8 sf_num;
int32 action_time;
uint32 vsrc_count;
boolean irat_mode;
}rflm_lte_set_lna_state_input_s;

typedef struct
{
uint8 carrier_idx; 
uint8 path;
uint8 gain_state;
uint8 sf_num;
int32 action_time;
}rflm_lte_set_lna_state_input_forced_s;

typedef struct
{
  uint32 ustmr_cnt_update_rx_sf;
  uint32 ustmr_cnt_set_lna;
  int32  time_delta;
}rflm_lte_ustmr_pair_s;

/* ------------------------------------------------------------------------- */

typedef struct
{
uint8                     carrier_idx;
boolean                   enable_rx_path;
uint8                     sf_num;
int32                     action_time;
rflm_handle_rx_t          handle_id[RFLM_LTE_MAX_NUM_RX_PATHS]; 
}rflm_lte_enable_rx_path_input_s;


/* ------------------------------------------------------------------------- */

typedef struct
{
  uint8                     carrier_idx;
  uint8                     rx_path;
  boolean                   enable_rx_path;
  int32                     action_time;
  rflm_handle_rx_t          handle_id;   
} rflm_lte_enable_disable_rx_path_input_s;


/*==============================================================================
                NLIC related input/output structure definitions
==============================================================================*/

typedef struct
{
  uint8 subframe_num;
  boolean active_slot[8];
  /*rflm_lte_sub_frame_enum sub_frame_type[8];*/
}rflm_lte_nlic_input_s;

typedef struct
{
      boolean nlic_en[3];
      uint8 rx_en;
      boolean delay_trk_en;
      uint8 kernel_term_extra_rxy_dly_acq;
      uint8 kernel_term_l0;
      uint8 kernel_term_nl0;
      uint8 kernel_term_nl1;
      uint8 kernel_term_nl2;
      boolean linear_conj_en;
      boolean rsb_en; 
      int32 freq_offset;
      uint8 rx_victim_carrier_id;
      uint16 rx_victim_bw;
      uint8 tx_aggr_carrier_id;
      uint16 tx_aggr_bw;
      int32 tx_intf_power_db;
      uint8 nlic_mode;
      int16 time_delay;
      uint8 clock_mode;
}rflm_lte_nlic_return_s;


typedef struct
{
  uint8 carrier_idx;        /*Cell index*/
  uint8 path_mask;          /*path mask: 0x01 = PRX, 0x02 = DRX, 0x03 = PRX + DRX*/
}rflm_lte_get_rx_config_status_input_s;


/*==============================================================================
                Other LTE input/output structure definitions
==============================================================================*/

typedef struct
{
  boolean    immediate_write;
  int8       script_id;                     
  uint32     time;
}rflm_lte_execute_script_s;

typedef struct
{
  uint8      carrier_idx;
  uint8      path;
  int16      error_in_hz;
  uint8      sf_num;
  int32      action_time;
  rflm_lte_ul_chan_type_e      ul_subframe_type; 
}rflm_lte_pll_settings_input_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
              API input/output function definitions -- TXAGC 
==============================================================================*/

/*!
  @brief API to do LTE txagc update

  @detail 
  This API is called by FW to do txagc update, including pa 
  state machine, txagc lookup, build txagc ccs issue_seq script and commit to 
  CCS, and program iq_gain,env_scale and dpd table via DRIF driver
 
  This API will be called only once for a given SF and all information
  regarding slot0, slot1 and SRS will be provided per CELL.
 
  If UL CA is active, and SCELL is present, then Modem FW will make
  2 calls of the following function, in which the CELL ID will be provided
  as an argument. 

  @param handle
  tx handle that contains information to de-referene tx LUT and tx script.

  @param in
  pointer to a structure that holds input parameters from FW

  @param out
  pointer to a stucture that holds output parameter to FW
 
  Need to confirm

  @return
  enum of API status  
*/

rflm_err_t rflm_lte_update_tx_sf
( 
   rflm_handle_tx_t                   handle_id,
   rflm_lte_update_tx_sf_input_s      *tx_sf_input,
   rflm_lte_update_tx_sf_return_s     *tx_sf_return 
);

/* Feedback API for Common FBRx for each trigger */
void rflm_lte_update_fbrx_gain_err (uint32 carrier_idx, uint32 start_rb, uint32 rb_block,
                                    int16 gain_err, int16 txagc_val);

void rflm_lte_hdet_start_read (uint32 carrier_idx, uint32 start_rb, uint32 rb_block,
                                    int16 gain_err, int16 txagc_val);
/*==============================================================================
                API input/output function definitions -- RXAGC
==============================================================================*/

/*!
  @brief API to do LTE rxagc update

  @detail 
  This API is called by FW to do rxagc update, including all the RX
  LNA state machine related settings for each path of each CELL. 
 
  The function is responsible for interaction with the data layer
  and the write to the CCS layer to send out the HW settings at the
  action time. 

  @param handle
  tx handle that contains information to de-referene tx LUT and tx script.

  @param in
  pointer to a structure that holds input parameters from FW, including

  @param out
  pointer to a stucture that holds output parameter to FW, including gain_state,
  gain in dB, and a boolean to validate the return result.

  @return
  enum of API status  
*/

rflm_err_t rflm_lte_update_rx_sf
( 
   rflm_lte_update_rx_sf_input_s      *rx_sf_input,
   rflm_lte_update_rx_sf_return_s     *rx_sf_return
);


/*!
  @brief API to do LTE rxagc update

  @detail 
  This API is called by FW to do rxagc update, including all the RX
  LNA state machine related settings for each path of each CELL. 
 
  The function is responsible for interaction with the data layer
  and the write to the CCS layer to send out the HW settings at the
  action time. 

  This API checks for IRAT mode param from FW to decide the cell index.

  @param handle
  tx handle that contains information to de-referene tx LUT and tx script.

  @param in
  pointer to a structure that holds input parameters from FW, including

  @param out
  pointer to a stucture that holds output parameter to FW, including gain_state,
  gain in dB, and a boolean to validate the return result.

  @return
  enum of API status  
*/

rflm_err_t rflm_lte_get_lna_state
( 
   rflm_lte_update_rx_sf_input_s      *rx_sf_input,
   rflm_lte_update_rx_sf_return_s     *rx_sf_return
);


/* ------------------------------------------------------------------------- */
/*!
  @brief API to get LTE LNA settings

  @detail 
  This API is called by Modem FW to get the RX gain offset
  value to set the DVGA loop related settings.


  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_index, path, gain_state, and subframe_num.
 
  @param out
  pointer to a stucture that holds output parameter to FW, including
  nominal gain, delta_rx_gain_offset, and default_rx_gain_offset.


  @return
  enum of API status  
*/

rflm_err_t rflm_lte_get_lna_settings
( 
   rflm_handle_rx_t                   handle_id,
   rflm_lte_get_lna_settings_input_s  *lna_settings_info,
   rflm_lte_get_lna_settings_return_s *total_gain_offset
);

/* ------------------------------------------------------------------------- */
/*!
  @brief API to get LTE LNA settings

  @detail 
  This API is called by Modem FW to get the RX gain offset
  value to set the DVGA loop related settings.

  This API checks for IRAT mode param from FW to decide the cell index.

  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_index, path, gain_state, and subframe_num.
 
  @param out
  pointer to a stucture that holds output parameter to FW, including
  nominal gain, delta_rx_gain_offset, and default_rx_gain_offset.


  @return
  enum of API status  
*/

rflm_err_t rflm_lte_get_lna_offset
( 
   rflm_handle_rx_t                   handle_id,
   rflm_lte_get_lna_settings_input_s  *lna_settings_info,
   rflm_lte_get_lna_settings_return_s *total_gain_offset
);

/* ------------------------------------------------------------------------- */
/*!
  @brief API to set LTE LNA settings

  @detail 
  This API is called by Modem FW to override the RX gain state without
  running the RXAGC loop. 


  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_index, path, gain_state, subframe_num, and action_time.
 
  @param out
  None.


  @return
  enum of API status  
*/
rflm_err_t rflm_lte_set_lna_state
( 
   rflm_handle_rx_t                   handle_id, 
   rflm_lte_set_lna_state_input_s     *lna_state_settings_info
);

/* ------------------------------------------------------------------------- */
/*!
  @brief API to set LTE LNA settings

  @detail 
  This API is called by Modem FW to override the RX gain state without
  running the RXAGC loop. 

  This API checks for IRAT mode param from FW to decide the cell index.

  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_index, path, gain_state, subframe_num, and action_time.
 
  @param out
  None.


  @return
  enum of API status  
*/
rflm_err_t rflm_lte_update_lna_state
( 
   rflm_handle_rx_t                   handle_id, 
   rflm_lte_set_lna_state_input_s     *lna_state_settings_info
);

/* ------------------------------------------------------------------------- */
rflm_err_t rflm_lte_set_iqmc_val  ( rflm_handle_rx_t handle_id, 
                                    uint8 lna_gain_state,
                                    boolean update_iqmc_value_imm,
                                    uint32 vsrc_counter_val  );

/* ------------------------------------------------------------------------- */
/*!
  @brief API to control RX path, i.e. on and off

  @detail 
  This API is used to turn on or off the RX path based on the value of the
  Boolean enable. 


  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  enable, carrier_index, subframe_num, and action_time.
 
  @param out
  None.


  @return
  enum of API status  
*/
rflm_err_t rflm_lte_enable_rx_path
( 
   rflm_lte_enable_rx_path_input_s    *enable_rx_path_info 
);

rflm_err_t rflm_lte_rx_on_off(rflm_lte_enable_rx_path_input_s *enable_rx_path_info);

rflm_err_t rflm_lte_rx_path_enable_disable(rflm_lte_enable_disable_rx_path_input_s *enable_rx_path_info);

/*==============================================================================
              API input/output function definitions -- FBRX
==============================================================================*/

rflm_err_t rflm_lte_fbrx_update
(
  rflm_handle_tx_t handle,
  boolean fbrx_done,
  int32 pwr_error
);

rflm_err_t rflm_lte_fbrx_arbitrator
(
   rflm_handle_tx_t handle,
   boolean *enable
);

rflm_err_t rflm_lte_run_fbrx
(
  rflm_handle_tx_t handle_id,
  rflm_lte_fbrx_input_s *fbrx_input,
  rflm_lte_fbrx_return_s *fbrx_return
);

/*==============================================================================
              API input/output function definitions -- NLIC
==============================================================================*/

rflm_err_t rflm_lte_get_nlic_settings_sf
(
  rflm_lte_nlic_input_s *nlic_info,
  rflm_lte_nlic_return_s *nlic_settings
);

/*==============================================================================
            API input/output function definitions -- script execute
==============================================================================*/
#define RFLM_LTE_IRAT_ACTION_PRELOAD_MASK 0x00000001
#define RFLM_LTE_IRAT_ACTION_TRIGGER_MASK 0x00000002
#define RFLM_LTE_IRAT_ACTION_NON_TRX_MASK 0x00000004
#define RFLM_LTE_IRAT_ACTION_ALL_MASK     (RFLM_LTE_IRAT_ACTION_PRELOAD_MASK | \
                                           RFLM_LTE_IRAT_ACTION_TRIGGER_MASK | \
                                           RFLM_LTE_IRAT_ACTION_NON_TRX_MASK)

/*==============================================================================
  For rflm_lte_execute_script
==============================================================================*/
/*! @brief  Input parameters for rflm_lte_execute_script API */
typedef struct
{
  rflm_ustmr_t    effect_time;         ///< Time for script to take effect
  rflm_ustmr_t    preload_offset_time; ///< Time advance from effect_time
  uint32          action;              ///< Action to perform
  boolean         immediate_flag;      ///< Flag for immediate effect_time
  int8            event_id;            ///< id of script to run
  rflm_handle_rx_t handle;              ///< handle_id
} rflm_handle_info_t;


typedef struct
{
  uint32 handle_bitmask;               ///< Bit mask definition 0   1   2   3    4    5
                                       ///<                     Tx0 Tx1 PRx DRX DRx2 DRx3
                                       ///< eg:  if bit mask = 0x0000 000A, handle[1] & handle[3] are valid
  rflm_handle_info_t handle_list[RFLM_LTE_MAX_NUM_HANDLES];    ///< handle Info for RX and TX (PRX0/1, DRX0/1, TX0/1,.)
} rflm_lte_execute_script_carrier_in_t;

typedef struct
{
  rflm_ustmr_t    effect_time;         ///< Time for script to take effect
  rflm_ustmr_t    preload_offset_time; ///< Time advance from effect_time
  uint32          action;              ///< Action to perform
  boolean         immediate_flag;      ///< Flag for immediate effect_time
  int8            event_id;            ///< id of script to run
} rflm_lte_execute_script_in_t;

/*! @brief  Output parameters for rflm_lte_execute_script API */
typedef struct
{
  rflm_ustmr_t   est_done_time;       ///< estimated completion time  
} rflm_lte_execute_script_out_t;

/*=================================================================================
  For rflm_lte_execute_tune_scripts_in_t - To-Be updated with new RF-FW Interface
==================================================================================*/
/*! @brief  Input parameters for rflm_lte_execute_tune_scripts API 
*/
typedef struct
{
  rflm_lte_execute_script_in_t script_info[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_EXECUTE_TUNE_SCRIPTS_MAX_HANDLE];
} rflm_lte_execute_tune_scripts_in_t;

/*! @brief 
    Opaque handle for uniquely identifying rx channel subscription in 
    LM API calls.  Handles are passed to FW via L1 prior to channel activation.
    The handle remains valid until channel deactivation.
*/
typedef struct
{
  uint32 idx[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_EXECUTE_TUNE_SCRIPTS_MAX_HANDLE];
}rflm_lte_handle_id_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sends an RF control script.

  @details
  This API tells LM to execute a generic RF control script, for example a
  tuning script or IRAT measurement script.

  @param handle : Handle containing script data and control information
  @param *in    : pointer to input data structure. Contains Event ID 
  @param *out   : pointer to data structure where results will be returned.

  @retval Returns an error status



===========================================================================*/
extern rflm_err_t rflm_lte_execute_scripts_per_carrier( rflm_lte_execute_script_carrier_in_t  *input,
                                           rflm_lte_execute_script_out_t *output);

extern rflm_err_t rflm_lte_execute_script( rflm_handle_rx_t handle,
                                           rflm_lte_execute_script_in_t  *in,
                                           rflm_lte_execute_script_out_t *out );

/*===========================================================================

  FUNCTION:  rflm_lte_execute_tune_scripts

===========================================================================*/
/*!
    @brief
    The function enqueues Rx Tune to chan scripts in to CCS TQ
    - To-Be updated with new RF-FW Interface (Once Available).

    @detail
    Scripts are identified by handle and script id. 

    @input
    rflm_dm_handle_id_t: handle ID combo - per Cell per Path
    rflm_lte_execute_tune_scripts_in_t  *input: It includes below members
      rflm_ustmr_t    effect_time;         ///< Time for script to take effect
      rflm_ustmr_t    preload_offset_time; ///< Time advance from effect_time
      uint32          action;              ///< Action to perform
      boolean         immediate_flag;      ///< Flag for immediate effect_time
      int8            event_id;    
    
    fw_func_cb         : FW CallBack function   
    rf_cell_config    : Info on Paths tuned per Cell (Debug only) .

    @return
    rflm_err_t: Error status
*/
extern rflm_err_t rflm_lte_execute_tune_scripts(rflm_lte_handle_id_t                *handle_combo,
                                                rflm_lte_execute_tune_scripts_in_t  *input,
                                                void                                *fw_func_cb,
                                                uint16                               rf_cell_config[RFLM_LTE_MAX_NUM_CARRIER]);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns Rx LO Offset from Carrier Center Frequency in KHz

  @details  

  @param cell_id      : PCELL/SCELL Indices
  @param path_id      : Index to identify Tx/Prx/Drx/DRx2/DRx3
                        RFLM_LTE_TX0/PRX/DRX/DRX2/DRX3
  @param *freq_delta  : Rx LO Offset from carrier center frequeny in KHz
                        per Cell per Path.
                        Center_freq of Carrier (UL or DL) - LO Frequency

  @retval Returns an error status

===========================================================================*/
extern rflm_err_t rflm_lte_get_carrier_freq_vs_lo_freq_offset( rflm_lte_cell_id_e    cell_id,    
                                                               rflm_lte_path_id_e    path_id,
                                                               int32*                freq_delta_in_KHz);

/*==============================================================================
               API input/output function definitions -- PLL
==============================================================================*/

/*!
  @brief API to do AFC for tx

  @detail 
  This API is used to adjust freq error for TX


  @param handle
  tx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_idx, path, frequency_error_in_hz, sf_num, and action_time.
 
  @param out
  Sends a MSRG to RFSW to confirm once PLL adjustment is done.

  @return
  enum of API status  
*/
rflm_err_t rflm_lte_handle_pll_tx
( 
   rflm_handle_tx_t                  handle_id,
   rflm_lte_pll_settings_input_s     *pll_setting_in
);

/*!
  @brief API for FW to execute on demand tune for Tx

  @detail  

  @param handle
  tx handle that contains information to de-referene tx LUT and tx script.

  @param in
  pointer to a structure that holds input parameters from FW

  @param out
  pointer to a stucture that holds output parameter to FW
 
  Need to confirm

  @return
  enum of API status  
*/

void rflm_lte_txagc_execute_tx_tune
(
   rflm_handle_tx_t handle_id,
   rflm_lte_execute_tx_tune_input_s *tx_tune_input,
   rflm_lte_execute_tx_tune_return_s *tx_tune_return
);

/* ------------------------------------------------------------------------- */
/*!
  @brief API to do AFC for rx

  @detail 
  This API is used to adjust freq error for RX


  @param handle
  tx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_idx, path, frequency_error_in_hz, sf_num, and action_time.
 
  @param out
  Sends a MSRG to RFSW to confirm once PLL adjustment is done.

  @return
  enum of API status  
*/
rflm_err_t rflm_lte_handle_pll_rx
( 
   rflm_handle_rx_t                  handle_id,
   rflm_lte_pll_settings_input_s     *pll_setting_in 
);


/* ------------------------------------------------------------------------- */
/*!
  @brief API to get RX config status

  @detail 
  This API is called by Modem FW to get the RX config status.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_index, path mask 
 
  @param out
  TRUE or FALSE
  TRUE = rx config is still active
  FALSE = rx config is done

  @return
  enum of API status  
*/

rflm_err_t rflm_lte_get_rx_config_status
( 
   rflm_lte_get_rx_config_status_input_s  *lna_settings_info,
   boolean *rx_config_active_status
);


/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/*!
  @brief API to get FTM RF enable status flag
*/
boolean rflm_lte_get_ftm_rf_flag(void);

/* ------------------------------------------------------------------------- */
/*!
  @brief API to get Cal mode status flag
*/
boolean rflm_lte_get_cal_mode_flag(void);



/* ------------------------------------------------------------------------- */
/*! @brief  Input parameters for rflm_lte_fbrx_cal_update API */
typedef struct
{
  uint8 reserved; /* placeholder */
} rflm_lte_fbrx_cal_update_in_t;

/*! @brief  Output parameters for rflm_lte_fbrx_cal_update API 
 - run_fbrx: Represent the symbol on which fbrx needs to be triggered. Max total symbols 
    which can be high are 14 including SRS symbol.
    */

typedef struct
{
  uint16                          run_fbrx; /* 16 bits captures each symbol within a SF */
  rflm_fbrx_dynamic_param_t       fbrx_dynamic_params[RFLM_FBRX_LTE_MAX_RUNS_PER_SF];
  rflm_fbrx_pwr_dependent_param_t fbrx_pwr_dep_params[RFLM_FBRX_LTE_MAX_RUNS_PER_SF];
} rflm_lte_fbrx_cal_update_out_t;

/*----------------------------------------------------------------------------*/
extern rflm_err_t rflm_lte_fbrx_cal_update( rflm_handle_tx_t handle,
                                            rflm_lte_fbrx_cal_update_in_t  *in,
                                            rflm_lte_fbrx_cal_update_out_t *out );

/*----------------------------------------------------------------------------*/
rflm_err_t rflm_lte_handle_asdiv(rflm_lte_cell_id_e cell_idx, uint32 action_time);

typedef struct
{
  rflm_lte_update_tx_sf_return_s     *tx_sf_return_ptr;
  uint8                              slot_id_bitmask;
  uint8                              ping_pong_idx;
  rflm_lte_ul_chan_type_e            ch_type;
  uint8                              sf_num;
  uint32                             frame_num;
}rflm_lte_tx_update_s;

rflm_err_t rflm_lte_txagc_register_fed_return_fn(void (*fn_ptr)(rflm_lte_tx_update_s*));

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_LTE_H */
