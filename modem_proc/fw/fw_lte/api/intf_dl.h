/*!
  @file
  intf_dl.h

  @brief
  This file contains lte LL downlink command id's and parameters structures

  @detail
  description of functions in this file
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_dl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/09   anm     Added TDD, MBSFN cfg and updated PHICH duration type
08/25/09   anm     Removed unused ota_tx_time info from tb in pdsch stat ind
07/15/09   anm     Added PDSCH stat message to replace PDSCH result ind
===========================================================================*/
#ifndef INTF_DL_H
#define INTF_DL_H

#include "msgr.h"
#include "intf_common.h"
#include "intf_dl_msg.h"
#include "intf_dl_si.h"
#include "intf_meas_ttl_ftl.h"  
#include "intf_dl_pdsch_test.h"
#include "intf_dl_pdcch_test.h"
#include "intf_ul.h"
#include "intf_rxagc_cfg_param.h"
/* borrow this for ML1: lte_LL1_log_mbsfn_whitened_matrices_ind_struct;
   send the same info in log packet to ML1, to prepare SNR report for eMBMS */
#include "lte_LL1_log_embms.h"
#include "intf_sys.h"

/*===========================================================================

      Constants

===========================================================================*/

//#define LTE_LL1_MAX_PDSCH_TRANMISSIONS_PER_TTI 3 
#define LTE_LL1_MAX_PDSCH_TRANMISSIONS_PER_TTI 6
#define LTE_LL1_MAX_PDSCH_GRANT_PER_TTI         5
#define LTE_LL1_MAX_MBSFN_ALLOCATIONS 8
#define LTE_LL1_MAX_PMCH_DECODES_PER_FRAME     6 //TDD = 5, FDD = 6
#define LTE_PDSCH_MAX_NUM_TBLK_PER_PROCESS     2 // should be in sync with pdsch_harq.h and pdcch.h 

/*! @brief IQ sample sample capture min and max number of samples in msg request
*/
#define LTE_LL1_IQ_SAMP_CAPTURE_MIN_NUM_SAMPLES 100 //minimum #samples to capture
#define LTE_LL1_IQ_SAMP_CAPTURE_MAX_NUM_SAMPLES 4096 //maximum #samples to capture

/*! @brief The offsets on how the captured IQ samples are stored in the shared memory 
    RX0 samples starts from 0
    RX1 samples start from 4K, which is the max number of samples for capture
*/
#define LTE_LL1_IQ_SAMP_CAPTURE_RX0_MEMIDX_OFFSET 0 //the offset of RX0 iq sample in the shared mem
#define LTE_LL1_IQ_SAMP_CAPTURE_RX1_MEMIDX_OFFSET LTE_LL1_IQ_SAMP_CAPTURE_MAX_NUM_SAMPLES //offset of RX1 iq sample in the shared mem 

/*! @brief IQ sample capture enum for rx index bit selection 
*/
#define LTE_LL1_IQ_CAPTURE_RX0_BIT 0 ///< RX0 bit is bit 0
#define LTE_LL1_IQ_CAPTURE_RX1_BIT 1 ///< RX1 bit is bit1 

/*! @brief PBCH IC max # of IC cells  
*/
#define LTE_LL1_MAX_NUM_PBCH_IC_CELLS 3
/*===========================================================================

      Macros

===========================================================================*/




/*===========================================================================

      Typedefs

===========================================================================*/
typedef enum
{
   LTE_LL1_DL_CH_STATE_DISABLE = 0,
   LTE_LL1_DL_CH_STATE_ENABLE
}lte_LL1_dl_ch_state_change_req_enum;

typedef enum
{
   LTE_LL1_DL_CNF_SUCCESS = 0,
   LTE_LL1_DL_CNF_FAIL
}lte_LL1_dl_cnf_enum;

typedef enum
{
  LTE_LL1_DL_CH_PCFICH = 0,       
  LTE_LL1_DL_CH_PHICH,       
  LTE_LL1_DL_CH_PDCCH,       
  LTE_LL1_DL_CH_PDSCH,
  LTE_LL1_DL_CH_PBCH,
  LTE_LL1_DL_CH_CSI_RS,
  LTE_LL1_DL_CH_SERV_MEAS_PATN,
  LTE_LL1_DL_CH_CONFIG_TYPE_COUNT
} lte_LL1_dl_ch_config_type_enum;

#define lte_LL1_dl_phich_duration_enum lte_l1_phich_duration_e;

typedef enum
{
  LTE_LL1_DL_PDSCH_DYNAMIC_ASSIGN = 0,
  LTE_LL1_DL_PDSCH_PERSISTENT_ASSIGN
} lte_LL1_dl_pdsch_assign_enum;

typedef enum
{
  LTE_LL1_C_RNTI = 0,
  LTE_LL1_SPS_C_RNTI,
  LTE_LL1_P_RNTI,       
  LTE_LL1_RA_RNTI,       
  LTE_LL1_T_C_RNTI,       
  LTE_LL1_SI_RNTI,       
  LTE_LL1_TPC_PUSCH_RNTI,       
  LTE_LL1_TPC_PUCCH_RNTI, 
  LTE_LL1_MBMS_RNTI,
  LTE_LL1_RNTI_COUNT
} lte_LL1_rnti_type_enum;

typedef enum
{
  LTE_LL1_CCE_AGG_LVL_1 = 0,
  LTE_LL1_CCE_AGG_LVL_2,
  LTE_LL1_CCE_AGG_LVL_4,
  LTE_LL1_CCE_AGG_LVL_8
} lte_LL1_cce_agg_level_enum;

typedef enum
{
  LTE_LL1_CCE_CM_SRCH_SPACE = 0,
  LTE_LL1_CCE_UE_SRCH_SPACE
} lte_LL1_cce_search_space_enum;

typedef enum
{
  LTE_LL1_PDCCH_IND = 0,
  LTE_LL1_PHICH_ACK_NACK_IND
}lte_LL1_pdcch_phich_ind_mask_enum;

typedef enum
{
  LTE_LL1_DL_DISABLE = 0,
  LTE_LL1_DL_ENABLE
}lte_LL1_dl_enable_enum;

typedef enum
{
  LTE_LL1_DL_TEST_DDE_TDEC_MIN_ITERATION=0,
  LTE_LL1_DL_TEST_DDE_TDEC_MAX_ITERATION,
  LTE_LL1_DL_TEST_DDE_TDEC_NUM_CRC_PASS,
  LTE_LL1_DL_TEST_DDE_TDEC_MIN_LLR_THRESHOLD,
  LTE_LL1_DL_TEST_DDE_TDEC_MAP_WINDOW_SIZE,
  LTE_LL1_DL_TEST_DDE_TDEC_EARLY_EXIT_ENABLE,
  LTE_LL1_DL_TEST_DDE_TDEC_BETA_SELECT,
  LTE_LL1_DL_TEST_DDE_PHICH_THRESHOLD,
  LTE_LL1_DL_TEST_DDE_PBCH_TAP_THRESHOLD,
  LTE_LL1_DL_TEST_DDE_PBCH_NT_EST_NUM_RB,
  LTE_LL1_DL_TEST_DDE_PBCH_TRUNCAITON_LEN, 
  LTE_LL1_DL_TEST_DDE_IS_TEST_PDSCH,
  LTE_LL1_DL_TEST_DDE_IS_TEST_PHICH,
  LTE_LL1_DL_TEST_DDE_IS_TEST_PDCCH,
  LTE_LL1_DL_TEST_DDE_CONST_LOG_ENABLE,
  LTE_LL1_DL_TEST_DDE_CONST_LOG_SUBFRAME,
  LTE_LL1_DL_TEST_DDE_CONST_LOG_SYMBOL,
  LTE_LL1_DL_TEST_DDE_CONST_LOG_STEP_SIZE,
  LTE_LL1_DL_TEST_DDE_CONST_LOG_OFFSET,
  LTE_LL1_DL_TEST_DDE_CONST_LOG_NUM_SYMBOL_PAIR_PER_LAYER,
  LTE_LL1_DL_TEST_DDE_PARAM_COUNT  
}lte_LL1_DL_TEST_DDE_PARAM_enum;

typedef enum
{
  LTE_LL1_DL_TEST_DDE_SF_PHICH_RESULT = 0,
  LTE_LL1_DL_TEST_DDE_SF_PHICH_TARGET_ID,
  LTE_LL1_DL_TEST_DDE_SF_PDCCH_BIT_LEN,
  LTE_LL1_DL_TEST_DDE_SF_PDCCH_BITS_0,
  LTE_LL1_DL_TEST_DDE_SF_PDCCH_BITS_1,
  LTE_LL1_DL_TEST_DDE_SF_PCFICH_RESULT,
  LTE_LL1_DL_TEST_DDE_SF_SFN,
  LTE_LL1_DL_TEST_DDE_SF_SFI,
  LTE_LL1_DL_TEST_DDE_SF_PARAM_COUNT  
}lte_LL1_DL_TEST_DDE_SF_PARAM_enum;

typedef enum
{
  LTE_LL1_DL_TEST_CHEST_NUM_SF_TEST=0,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_0=1,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_1=2,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_2=3,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_3=4,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_4=5,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_5=6,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_6=7,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_7=8,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_8=9,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_9=10,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_10=11,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_11=12,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_12=13,
  LTE_LL1_DL_TEST_CHEST_TWO_TAP_FIR_WEIGHT_13=14,
  LTE_LL1_DL_TEST_CHEST_SEVEN_TAP_FIR_WEIGHT_0=15,
  LTE_LL1_DL_TEST_CHEST_SEVEN_TAP_FIR_WEIGHT_1=16,
  LTE_LL1_DL_TEST_CHEST_SEVEN_TAP_FIR_WEIGHT_2=17,
  LTE_LL1_DL_TEST_CHEST_SEVEN_TAP_FIR_WEIGHT_3=18,
  LTE_LL1_DL_TEST_CHEST_TAP_THRESHOLD=19,
  LTE_LL1_DL_TEST_CHEST_TRUNCAITON_LEN_SHORT_CP=20,
  LTE_LL1_DL_TEST_CHEST_TRUNCATION_LEN_LONG_CP=21, 
  LTE_LL1_DL_TEST_CHEST_HW_CIR_FIR_ENA=22, 
  LTE_LL1_DL_TEST_CHEST_PUSH_RAW_CIR=23, 
  LTE_LL1_DL_TEST_CHEST_SOFT_TAP_THRESHOLDING=24,
  LTE_LL1_DL_TEST_CHEST_NT_FOR_HIGH_DOPPLER=25,
  LTE_LL1_DL_TEST_CHEST_PARAM_COUNT
}lte_LL1_DL_TEST_CHEST_PARAM_enum;

/*! @brief Enable/Disable Radio Link Monitoring (RLM)*/
typedef enum
{
   LTE_LL1_RLM_DISABLE = 0,
   LTE_LL1_RLM_ENABLE
}lte_LL1_rlm_enable_enum;

/*! @brief Connected DRX on/off */
typedef enum
{
  LTE_LL1_RLM_DRX_OFF = 0,
  LTE_LL1_RLM_DRX_ON
} lte_LL1_rlm_drx_on_off_enum;

typedef enum
{
  LTE_LL1_DL_SPS_CONF_TYPE_NULL,
  LTE_LL1_DL_SPS_CONF_TYPE_RELEASE, 
  LTE_LL1_DL_SPS_CONF_TYPE_ACTIVATION,
  LTE_LL1_DL_SPS_CONF_TYPE_CONFIGURED_ASSIGNMENT,
  LTE_LL1_DL_SPS_CONF_TYPE_RETRANSMISSION,
} lte_LL1_dl_sps_conf_type;
//--------------------------------------------------------------------------
//  Odyssey specific typedefs
// -------------------------------------------------------------------------

/*! @brief General Parameter list for Odyssey Channel estimation
rxfft_only               Program RxFFT and tone-split only
wola_is_not_prog         for FFTE WOLA task (don't program/program) 
center_mass_is_not_used  Don't use calculated center of mass 
destagger_is_not_used    Don't/Do destagger pilots
iir_coeff_is_constant    Use constant IIR coefficients
noise_td_avg_is_1_slot   Noise time domain average is 1/2 slot 
noise_fd_avg_is_2_rbs    Noise frequency domain average is 2/4 RBs 
noise_zero_off_diag_is_true     Zero off-diagonal entries in noise matrices
edge_extrap              PTM Edge extrapolation enum (normal/ none/
                                     no memory)
windowing                Winshift operation setting (normal/ over cp/
                                     false)
*/
typedef enum
{
  lte_LL1_DL_CONFIG_CE_GENERAL_rxfft_only,
  lte_LL1_DL_CONFIG_CE_GENERAL_wola_is_not_prog,         
  lte_LL1_DL_CONFIG_CE_GENERAL_center_mass_is_not_used,  
  lte_LL1_DL_CONFIG_CE_GENERAL_destagger_is_not_used,    
  lte_LL1_DL_CONFIG_CE_GENERAL_iir_coeff_is_constant,    
  lte_LL1_DL_CONFIG_CE_GENERAL_noise_td_avg_is_1_slot,   
  lte_LL1_DL_CONFIG_CE_GENERAL_noise_fd_avg_is_2_rbs,    
  lte_LL1_DL_CONFIG_CE_GENERAL_noise_zero_off_diag_is_true,     
  lte_LL1_DL_CONFIG_CE_GENERAL_edge_extrap,                                 
  lte_LL1_DL_CONFIG_CE_GENERAL_windowing,                                   
  LTE_LL1_DL_CONFIG_CE_GENERAL_PARAM_COUNT
}lte_LL1_dl_config_ce_general_param_enum;


/*! @brief IIR Parameter list per Tx pair per Rx for Odyssey CE 
coherent_input   Coeff for the input to the coherent filter     
coherent_state   Coeff for the state of the coherent filter     
se_input         Coeff for the input to the signal energy filter
se_state         Coeff for the state of the signal energy filter
sne_input        Coeff for the input to the signal-noise energy filter
sne_state        Coeff for the state of the signal-noise energy filter                                                
se_sum_input     Coeff for the input to the filter for sum over signal 
                           energy taps
se_sum_state     Coeff for the state of the filter for sum over  
                       signal energy taps
sne_sum_input    Coeff for the input to the filter for sum over  
                      signal-noise energy taps  
sne_sum_state    Coeff for the state of the filter for sum over  
                      signal-noise energy taps  
*/
typedef enum
{
  lte_LL1_DL_CONFIG_CE_IIR_coherent_input,
  lte_LL1_DL_CONFIG_CE_IIR_coherent_state,
  lte_LL1_DL_CONFIG_CE_IIR_se_input,
  lte_LL1_DL_CONFIG_CE_IIR_se_state,
  lte_LL1_DL_CONFIG_CE_IIR_sne_input,
  lte_LL1_DL_CONFIG_CE_IIR_sne_state,
  lte_LL1_DL_CONFIG_CE_IIR_se_sum_input,  
  lte_LL1_DL_CONFIG_CE_IIR_se_sum_state,              
  lte_LL1_DL_CONFIG_CE_IIR_sne_sum_input,
  lte_LL1_DL_CONFIG_CE_IIR_sne_sum_state,
  LTE_LL1_DL_CONFIG_CE_IIR_PARAM_COUNT
}lte_LL1_dl_config_ce_iir_param_enum;


typedef enum
{
  LTE_LL1_DL_TEST_RS_FTL_UPDATE_PERIOD=0,
  LTE_LL1_DL_TEST_RS_FTL_FEEDBACK_COEFF,
  LTE_LL1_DL_TEST_RS_FTL_ENABLE,
  LTE_LL1_DL_TEST_SAFENET_SSC_FTL_UPDATE_PERIOD,
  LTE_LL1_DL_TEST_SAFENET_SSC_FTL_FEEDBACK_COEFF,
  LTE_LL1_DL_TEST_SAFENET_SSC_FTL_ON_THRESH,
  LTE_LL1_DL_TEST_SAFENET_SSC_FTL_OFF_THRESH,
  LTE_LL1_DL_TEST_SAFENET_SSC_FTL_MONITOR_PERIOD,
  LTE_LL1_DL_TEST_SAFENET_SSC_FTL_MONITOR_DURATION,
  LTE_LL1_DL_TEST_SAFENET_SSC_FTL_ENABLE,
  LTE_LL1_DL_TEST_FTL_CORR_VCXO_UPDATE_ENABLE,
  LTE_LL1_DL_TEST_FTL_CORR_VCXO_UPDATE_RTC_PERIOD_LIMIT,
  LTE_LL1_DL_TEST_FTL_CORR_NCO_FREQ_CORR_HZ_LIMIT,
  LTE_LL1_DL_TEST_FTL_CORR_LOG_PERIOD,
  LTE_LL1_DL_TEST_RS_FTL_CORRECTION_THRESHOLD,
  LTE_LL1_DL_TEST_SAFENET_SSC_FTL_CORRECTION_THRESHOLD,
  LTE_LL1_DL_TEST_FTL_USE_ACC_TPM_MODE,
  LTE_LL1_DL_TEST_FTL_PARAM_COUNT
}lte_LL1_DL_TEST_FTL_PARAM_enum;

typedef enum
{
  LTE_LL1_DL_TEST_TTL_ALPHA=0,
  LTE_LL1_DL_TEST_TTL_THRESHOLD,
  LTE_LL1_DL_TEST_TTL_COH_COMB_LEN,
  LTE_LL1_DL_TEST_TTL_NONCOH_COMB_LEN,
  LTE_LL1_DL_TEST_TTL_UPDATE_PERIOD,
  LTE_LL1_DL_TEST_TTL_TIMING_UPDATE_DELAY,
  LTE_LL1_DL_TEST_TTL_X3_ADJ,
  LTE_LL1_DL_TEST_TTL_ENABLE,
  LTE_LL1_DL_TEST_TTL_RS_COG_ENABLE,
  LTE_LL1_DL_TEST_TTL_DWIN_LEN,
  LTE_LL1_DL_TEST_TTL_EWIN_LEN,
  LTE_LL1_DL_TEST_TTL_KSN,
  LTE_LL1_DL_TEST_TTL_KSP,
  LTE_LL1_DL_TEST_TTL_MAX_ADJ,
  LTE_LL1_DL_TEST_TTL_BACKOFF, 
  LTE_LL1_DL_TEST_TTL_CORRECTION_THRESHOLD,
  LTE_LL1_DL_TEST_TTL_PARAM_COUNT
}lte_LL1_DL_TEST_TTL_PARAM_enum;

/*! @brief Indicates whether CSF config or deconfig requests have been processed */
typedef enum
{
   CSF_CONFIG_ERROR = 0,
   CSF_CFG_DECFG_COMPLETE
}lte_LL1_csf_cfg_decfg_stat_enum;

/*! @brief Enum for Page_drx_cycle in radio frame.
*/
typedef enum
{
 LTE_LL1_PAGE_DRX_CYCLE_32_RF= 0,
 LTE_LL1_PAGE_DRX_CYCLE_64_RF,
 LTE_LL1_PAGE_DRX_CYCLE_128_RF,
 LTE_LL1_PAGE_DRX_CYCLE_256_RF
} lte_LL1_page_drx_cycle_enum;
/*! @brief 
 Enum Used to calculate Ns and N, eventually to derive PO(page occasion) and PF(page frame) of the cell.
 Ns=Max (1,nB/T)
 N=min(T,nB)
 PF SFN =0 mod(T/N)
 PO: Subframes in the following table
 Ns	TDD PO	     FDD PO
 1	  0	          9
 2	  0,5	        4,9
 4	  0,1,5,6	    0,4,5,9

*/
typedef enum
{
 LTE_LL1_PAGE_NB_4T= 0,
 LTE_LL1_PAGE_NB_2T,
 LTE_LL1_PAGE_NB_T,
 LTE_LL1_PAGE_NB_T_OVER_2,
 LTE_LL1_PAGE_NB_T_OVER_4,
 LTE_LL1_PAGE_NB_T_OVER_8,
 LTE_LL1_PAGE_NB_T_OVER_16,
 LTE_LL1_PAGE_NB_T_OVER_32,
 NB_MAX
} lte_LL1_page_nb_enum;

typedef enum
{
 IGNORE_ON_DURATION= 0,
 RX_ON_STOP_SUBFRAME,
 RX_ON_START_SUBFRAME
} lte_LL1_on_duration_enum;

/*! @brief 
 Define number of antenna ports as enum (1/2/4/8).
*/
typedef enum
{
 LTE_LL1_NUM_ANT_PORTS_UNKNOWN = 0,
 LTE_LL1_NUM_ANT_PORTS_1 = 1,
 LTE_LL1_NUM_ANT_PORTS_2 = 2,
 LTE_LL1_NUM_ANT_PORTS_4 = 4,
 LTE_LL1_NUM_ANT_PORTS_8 = 8,
} lte_LL1_num_ant_ports_enum;

/*! @brief 
 Enum Used to inform the FW about clk change.
 FW will not change the clock if it is default
 FW will only use the new CLK if it is ACTIVATION or DEACTIVATION. 
 FW will only delay the cnf if  it is DEACTIVATION.
 */
typedef enum
{
 LTE_LL1_RNTI_CHANGE_NO_CHANGE = 0,
 LTE_LL1_RNTI_CHANGE_ACTIVATION,
 LTE_LL1_RNTI_CHANGE_DEACTIVATION,
} lte_LL1_clk_change_ind_enum;

/* Based on 01/23/14 LL-ML sync up, PBCH-IC is NOT time-critical & NORMAL_IC_PBCH
   is sufficient for now. The other two modes remain in the enum, but its support
   will be on future demand (e.g., when data IC is to be done on non-participating
   nbr of the pico serving cell, in which case nbr PBCH decode is needed frequently
   to find the # of Tx for data IC). FW is designed in a flexible way such that 
   the advanced PBCH_IC modes can be easily accommodated w/ minimum code change.

   Modes of pbch decodes based on systems use cases: ASAP_IC_PBCH is used when
   ML1 thinks signal quality is good and we don't need to wait for all 4 rv's
   to be ready. In this case, ic pbch will start asap collecting all rv's before
   rv3 or before sample collection is interrupted by forced macro sleep. 
   ONE_RV_IC_PBCH is used for ic pbch over short on duration in CDRX and can be
   seen as a special case of ASAP_IC_PBCH. However, we do want ML1 to tell us 
   explicitly to avoid uncertainties. 
*/
typedef enum
{
  LTE_LL1_LEGACY_PBCH = 0,
  LTE_LL1_NORMAL_IC_PBCH,  ///< ic pbch exercising full 4-rv state machine
  LTE_LL1_ONE_RV_IC_PBCH,  ///< ic pbch using only single rv's data 
  LTE_LL1_ASAP_IC_PBCH,    ///< ic pbch that does not wait for rv0 to start
} lte_LL1_pbch_mode_e; 

// --------------------------------------------------------------------------
//
// Request message parameter structures
//
// --------------------------------------------------------------------------

/*! @brief TDD configuration
 */
typedef struct
{
 /*! Sub frame assignment
     Deafult assumed as 0 until initial SIB1 is decoded where tdd cfg
     is present. SIB1 is always at 5th subframe and in TDD
     5th subframe is always DL
 */
  lte_l1_tdd_subframe_assign_e          ul_dl_config;

  /*! Special subframe patterns 
     Deafult assumed as 0 until initial SIB1 is decoded where tdd cfg
     is present. SIB1 is always at 5th subframe and in TDD
     5th subframe is always DL and special subframe length is fixed = 1ms
  */
  lte_l1_tdd_special_subframe_pattern_e special_subframe_config;
} lte_LL1_dl_tdd_cfg_s;

/*! @brief The mbsfn configuration
 */
typedef struct
{
 /*! flag to indicate if mbsfn cfg is enable or disable.
     Default is disabled if config is not present 
     enable  => TRUE
     disable => FALSE
 */
  boolean mbsfn_enable;

  /*! Number of MBSFN frame allocations */
  uint8 num_mbsfn_allocations;   

  /*! MBSFN frame allocation info */
  lte_l1_mbsfn_allocation_s mbsfn_alloacation[LTE_LL1_MAX_MBSFN_ALLOCATIONS];
} lte_LL1_dl_mbsfn_cfg_s;

/*! @brief DL Channel configuration input structure
*/
typedef struct {
  uint8        pcfich_lower_limit;   ///< range 1 to 3, PHICH duration
  lte_LL1_dl_mbsfn_cfg_s mbsfn_cfg;  ///< mbsfn configuration
  lte_LL1_dl_tdd_cfg_s   tdd_cfg;    ///< tdd configuration
} lte_LL1_dl_pcfich_params_struct;

typedef struct {
  int8  num_phich_groups;     ///< number of PHICH groups, <=ceil(num_prb/4)
  lte_l1_phich_duration_e         phich_duration; ///< normal or extended
} lte_LL1_dl_phich_params_struct;
  

typedef struct {
  uint16   serv_cell_index;      ///< Ranges 0..7. Mapped to each DL CC as defined in 36.331. Used as carrier indicator field 
                                 ///< for PDCCH search. See 9.1.1 of 36.213.
  uint16   rnti_enable_mask;     ///< bitmask to enable certain RNTI's for PDCCH blind decoder
                 ///< Use lte_LL1_rnti_type_enum for bit location of RNTIs
  uint16   c_rnti;               ///< connecgted state, common and ue specific search space
  uint16   sps_c_rnti;           ///< connecgted state, semi-persistent assignment, common and ue specific search space
  uint16   p_rnti;               ///< paging in RRC_CONNECTED  or IDLE   
  uint16   ra_rnti;              ///< Random access
  uint16   t_c_rnti;             ///< temp c_rnti for message 4 when contention is not resolved
  uint16   si_rnti;              ///< system info, connected and idle
  uint16   tpc_pusch_rnti;       ///< group power control for PUSCH (persistent assignment), connected state
  uint16   tpc_pucch_rnti;       ///< group power control for PUCCH, connected state
  uint16   mbms_rnti;            ///< MBMS RNTI on PDCCH DCI 1C MBSFN subframe (non-MBSFN region), idle/connected state

  boolean  clear_harq_buf;       ///< HARQ buffer flag: TRUE means clear harq buffers
  uint16   sps_periodicity; 	 ///< structure has valid content if the SPS RNTI is enabled in the bit mask
  uint16   num_cnfg_sps_process; ///< Number of configured SPS processes
  boolean  is_drx_on_duration;   ///< indicating first Tx after DRX wakeup

  lte_LL1_clk_change_ind_enum clk_change_ind;
} lte_LL1_dl_pdcch_params_struct;

typedef struct {
  lte_LL1_on_duration_enum state; ///< 0, don't care
                                  ///<1, on_duration to end on 
                                  ///<frame/subframe associated with;
                                  ///<2, on_duration to start on 
                                  ///<frame/subframe associated with;
  int16 frame;
  int8  subframe;
  int32 inactive_timer;
}lte_LL1_dl_on_duration_struct;

typedef struct { 
  lte_l1_dl_tx_mode_e  transmission_mode; ///< transmission_mode for PDSCH (UE specifical search space)
//  lte_cdd_delay_enum_t     cdd_delay;   ///< 0: no delay CDD, 1: small delay CDD, 2: large delay CDD
  lte_LL1_dl_pdsch_assign_enum  persistent_assignment_flag; ///< 0: dynamic assignment from PDCCH
                                    ///< 1: persistent assignment
  lte_l1_pa_e                    p_a; ///< PDSCH and RS EPRE ratio index
  lte_l1_pb_e                    p_b; ///< PDSCH type A and Type B power ratio index(index from broadcast SIB)
} lte_LL1_dl_pdsch_params_struct;

/*! @brief 
  [36.331] CSI-RS-ConfigZP-r11 
  Zero power CSI RS configuration: muting to allow other eNB CSI-RS transmission
*/
typedef struct {
  uint8    id_zp;           ///< the ZP CSI RS identity
  uint16   resource_config; ///< 031(36.211 table 6.10.5.2-1 & table 6.10.5.2-2). 
                            ///< ZP CSIRS is defined on 4 port CSIRS config. Each bit in the bitmap corresponds
                            ///< to one of the 4 CSIRS port configuration. For example, bit(0)=1 means configuration 0 is muted. In rel-10, per Amir, up to 1 ZP CSI RS configuration muting. In rel-11, up to 4 ZP CSI RS.
  uint8  subframe_config;   ///< 0..154  (36.211 table 6.10.5.3-1)
} lte_LL1_dl_zp_csi_rs_cfg_struct;

/*! @brief 
  Non zero power CSI RS configuration: [36.331] CSI-RS-ConfigNZP-r11 
*/
typedef struct {
  uint16 qcl_scrambling_id; ///< 0 to 503
  uint8  crs_ports_count;   ///< ENUM! {n1, n2, n4, sparse}
  lte_LL1_dl_mbsfn_cfg_s mbsfn_subframe_cfg; 
} lte_LL1_dl_nzp_csi_rs_qcl_crs_info_struct;
typedef struct {
  // CSI-RS-ConfigNZP-r11
  uint8    id_nzp;          ///< the ZP CSI RS identity
  uint8    p_c_index;       //index ranging [0,23], representing the ratio of PDSCH EPRE to CSIRS EPRE in the range of [-8,15]
  lte_LL1_num_ant_ports_enum ant_port_count; ///< Enum of 1,2,4,8  
  uint16   resource_config; ///< 0 - 31(36.211 table 6.10.5.2-1 & table 6.10.5.2-2). For non-zero power CSI RS, this is the value of the configuration index. 
                            ///<For non-zero power CSI RS, this is the 16 bit bitmask, ZP CSIRS is defined on 4 port CSIRS config. Each bit in the bitmap corresponds to one of the 4 CSIRS port configuration. For example, bit(0)=1 means configuration 0 is muted. In rel-10, per Amir, up to 1 ZP CSI RS configuration muting. In rel-11, up to 4 ZP CSI RS.
  uint8  subframe_config;   ///< 0..154  (36.211 table 6.10.5.3-1)
  uint16 scrambling_id;     ///< 0 to 503
  lte_LL1_dl_nzp_csi_rs_qcl_crs_info_struct qcl_crs_info;
} lte_LL1_dl_nzp_csi_rs_cfg_struct;

typedef struct {

  // Note the following 4 flags are FW generated. ML1 does not populate the fields in the message
  // FW defined them in the DL CH CFG message for convenience of sharing the message payload to the 
  // DL DB contens. To avoid code churn, keep it as is but make the note here. (ideally, should 
  // them from message payload and add them to DL DB instead)
  boolean csi_rs_collision_chk;   ///< NZP CSI-RS collision check defined in 36.211, true => collide with sync, SIB1, page
  uint8   csi_rs_existence;       ///< NZP CSI-RS existence in a SF
  boolean zp_csi_rs_collision_chk;///< ZP CSI-RS collision check defined in 36.211, true => collide with sync, SIB1, page
  uint8   ztp_csi_rs_existence;   ///< ZTP CSI-RS existence in a SF
  // End FW generated flag section
  
  //Multiple CSI RS configuration can be used in a given cell (36.211, 6.10.5.2)
  // zeroPowerCSI-RS configured by upper layer: zero or more
  // non-zero CSI RS config: zero or 1 in rel-10, up to 3 in rel-11
  uint8   num_nzp_csi_rs;        ///< number of nonZeroTxPowerCsiRs: [36.211, 6.10.5.2], 0 or 1 configuration for non-zero-power CSI RS. If 0, ignore nzp_csi_rs
  lte_LL1_dl_nzp_csi_rs_cfg_struct nzp_csi_rs[3]; ///< config index and subframe config for non-zero power CSI RS

  uint8   num_zp_csi_rs;        ///< number of ZeroTxPowerCsiRs: [36.211, 6.10.5.2], 0 or more configuration for zero-power CSI RS. If 0, ignore zp_csi_rs
  lte_LL1_dl_zp_csi_rs_cfg_struct zp_csi_rs[4]; ///< config index and subframe config for non-zero power CSI RS

  // specify common paging configuration (Note: do not use IMSI based paging configuration here) so that UE knows CSI RS not transmitted in this subframe.
  lte_LL1_page_drx_cycle_enum   t_paging_cycle;       ///< no CSI RS in paging subframe   
  lte_LL1_page_nb_enum nb;               ///< 

} lte_LL1_dl_csi_rs_params_struct;


/*! @brief  For CA

*/

typedef struct {
  uint8 pdsch_start;	    ///< 1..4 PDSCH start symbol
  uint8 sched_cell_id;   ///< 0..1 Indicate which serving cell the dl allocation and UL grants for the Scell.
}lte_LL1_dl_cross_carrier_sched_params_struct;


/*! @brief DL config request input structure
*/
typedef struct {
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
  uint8 channel_config_mask; ///< each bit corresponding to one DL_CH_CONFIG_TYPE, 
                             ///< refer to lte_LL1_dl_ch_config_type_enum.
                             ///< "1" means corresponding config is valid.
  lte_LL1_dl_pcfich_params_struct pcfich_param;
  lte_LL1_dl_phich_params_struct  phich_param;
  lte_LL1_dl_pdcch_params_struct  pdcch_param;

  // Cross carrier scheduling Configuration parameters.
  // Only valid for SSC.
  // The cross_carrier_sched_param channel_config_mask is used as cif_presence.
  lte_LL1_dl_cross_carrier_sched_params_struct cross_carrier_sched_param; 

  lte_LL1_dl_pdsch_params_struct  pdsch_param;
  lte_LL1_dl_csi_rs_params_struct csi_rs_param; ///< Csi_rs definition for a serving cell

  // Serving cell measurement subframe pattern.
  // Used for serving cell RSRP/RSSI/RSRQ measurement and RLM measurement.
  // Only valid for PCC.
  // From 36.331 MeasSubframePatternPCell-r10.
  lte_LL1_meas_subframe_pattern_struct serv_meas_pattern_param;
} lte_LL1_dl_ch_config_req_carrier_payload_struct;

typedef struct {
  lte_LL1_req_header_struct  req_hdr;
  uint8 num_carriers;                           ///< Number of carriers to be configured.
  lte_LL1_dl_ch_config_req_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_dl_ch_config_req_struct;

typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_ch_config_req_struct msg_payload;
} lte_LL1_dl_ch_config_req_msg_struct;


/*! @brief DL channel state change request input structure
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;
  lte_LL1_dl_ch_state_change_req_enum state_change_flag; ///< 0: LTE_LL1_DL_CH_STATE_DISABLE
               ///< 1: LTE_LL1_DL_CH_STATE_ENABLE
} lte_LL1_dl_ch_state_change_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_ch_state_change_req_struct msg_payload;
} lte_LL1_dl_ch_state_change_req_msg_struct;

/*! @brief DL pdcch enable request input structure
*/
typedef struct {
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
  lte_LL1_dl_pdcch_params_struct rntis;
  lte_LL1_dl_on_duration_struct on_duration;
} lte_LL1_dl_pdcch_rnti_change_req_carrier_payload_struct;
typedef struct {
  lte_LL1_req_header_struct req_hdr;
  uint8 num_carriers;                           ///< Number of carriers to be configured.
  lte_LL1_dl_pdcch_rnti_change_req_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_dl_pdcch_rnti_change_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_pdcch_rnti_change_req_struct msg_payload;
} lte_LL1_dl_pdcch_rnti_change_req_msg_struct;

/*! @ Deprecated
 *   brief DL PBCH state change request input structure
 */
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  lte_LL1_dl_ch_state_change_req_enum state_change_flag;
} lte_LL1_dl_pbch_state_change_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_pbch_state_change_req_struct msg_payload;
} lte_LL1_dl_pbch_state_change_req_msg_struct;

/*! @brief DL start/stop request input structure
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  lte_LL1_dl_enable_enum dl_enable;
  int16 freq_correction;    //total current correction/compensation 
  uint32 lna_state_valid;   //last lna status valid, odysey specific (not used for espresso)
  uint32 lna_state[6];      //last lna state before DL disable - 6 for place holder, actual is TBD, odysey specific (not used for espresso)
} lte_LL1_dl_enable_disable_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_enable_disable_req_struct msg_payload;
} lte_LL1_dl_enable_disable_req_msg_struct;


/*! @brief DL TEST_DDE request input structure
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  uint32 num_params;
  lte_LL1_config_element_struct test_dde_params[LTE_LL1_DL_TEST_DDE_PARAM_COUNT];

} lte_LL1_dl_test_dde_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_test_dde_req_struct msg_payload;
} lte_LL1_dl_test_dde_req_msg_struct;


/*! @brief DL TEST_DDE_SF request input structure
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  uint32 num_params;
  lte_LL1_config_element_struct test_dde_sf_params[LTE_LL1_DL_TEST_DDE_SF_PARAM_COUNT];

} lte_LL1_dl_test_dde_sf_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_test_dde_sf_req_struct msg_payload;
} lte_LL1_dl_test_dde_sf_req_msg_struct;


/*! @brief DL TEST_CHEST request input structure
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  uint32 num_params;
  lte_LL1_config_element_struct test_chest_params[LTE_LL1_DL_TEST_CHEST_PARAM_COUNT];

} lte_LL1_dl_test_chest_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_test_chest_req_struct msg_payload;
} lte_LL1_dl_test_chest_req_msg_struct;

/*@!brief RLM enable/disable request */
typedef struct {    
  lte_LL1_carrier_type_enum          carrier_type; ///< Enum for dl carriers in  CA
  lte_LL1_rlm_enable_enum            en_dis;
  lte_LL1_rlm_drx_on_off_enum        drx_on;    ///< DRX on/off flag
  uint16                             cycl_len;  ///< DRX cycle length (in unit of ms)
} lte_LL1_dl_rlm_req_carrier_payload_struct;

typedef struct {    
  lte_LL1_req_header_struct          req_hdr;
  uint8                              num_carriers; ///< Number of carriers to be configured.
  lte_LL1_dl_rlm_req_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_dl_rlm_req_struct;

typedef struct 
{
  msgr_hdr_struct_type               msg_hdr;
  lte_LL1_dl_rlm_req_struct          msg_payload;
}lte_LL1_dl_rlm_req_msg_struct;

//--------------------------------------------------------------------------
//  Odyssey specific request message structures
// -------------------------------------------------------------------------
/*! @brief DL CONFIG_CE general request input structure (Odyssey)
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;
  uint32 num_params;
  lte_LL1_config_element_struct config_ce_general_params[LTE_LL1_DL_CONFIG_CE_GENERAL_PARAM_COUNT];
} lte_LL1_dl_config_ce_general_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_config_ce_general_req_struct msg_payload;
} lte_LL1_dl_config_ce_general_req_msg_struct;

/*! @brief DL CONFIG_CE IIR request input structure (per Rx and Tx pair) (Odyssey)
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;
  uint16                    tx_ant_pair;
  uint16                    rx_ant;
  lte_LL1_config_element_struct config_ce_iir_params[LTE_LL1_DL_CONFIG_CE_IIR_PARAM_COUNT];
} lte_LL1_dl_config_ce_iir_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_config_ce_iir_req_struct msg_payload;
} lte_LL1_dl_config_ce_iir_req_msg_struct;



/*! @brief DL TEST_FTL request input structure
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  uint32 num_params;
  lte_LL1_config_element_struct test_ftl_params[LTE_LL1_DL_TEST_FTL_PARAM_COUNT];

} lte_LL1_dl_test_ftl_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_test_ftl_req_struct msg_payload;
} lte_LL1_dl_test_ftl_req_msg_struct;

/*! @brief DL TEST_TTL request input structure
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  uint32 num_params;
  lte_LL1_config_element_struct test_ttl_params[LTE_LL1_DL_TEST_TTL_PARAM_COUNT];

} lte_LL1_dl_test_ttl_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_dl_test_ttl_req_struct msg_payload;
} lte_LL1_dl_test_ttl_req_msg_struct;

/*! @brief ANT CORR  start/stop request input structure
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;

  boolean ant_corr_enable;    ///< 0: disable, 1: enable
  uint16 report_period;       ///< reporting period in ms units 
  uint16 iir_alpha;           ///< Q-format: 16U14
} lte_LL1_ant_corr_enable_disable_req_struct;
typedef struct {
  msgr_hdr_struct_type msg_hdr;
  lte_LL1_ant_corr_enable_disable_req_struct msg_payload;
} lte_LL1_ant_corr_enable_disable_req_msg_struct;


// --------------------------------------------------------------------------
//
// confirmation message parameter structures
//
// --------------------------------------------------------------------------

/*! @brief parameter structure config confirmation
*/
typedef struct { 
  lte_LL1_dl_cnf_enum err_code;
  lte_LL1_carrier_type_enum carrier_type;  ///< Enum for dl carriers in  CA
} lte_LL1_dl_ch_config_cnf_carrier_payload_struct;
typedef struct { 
  lte_LL1_cnf_header_struct cnf_hdr;
  uint8 num_carriers;                           ///< Number of carriers to be configured.
  lte_LL1_dl_ch_config_cnf_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_dl_ch_config_cnf_struct;
typedef struct {
  msgr_hdr_struct_type                msg_hdr;      ///< message router header
  lte_LL1_dl_ch_config_cnf_struct     msg_payload; 
} lte_LL1_dl_ch_config_cnf_msg_struct;


/*! @brief parameter structure channel state change confirmation
*/
typedef struct { 
  lte_LL1_cnf_header_struct cnf_hdr;
  lte_LL1_dl_cnf_enum err_code;
} lte_LL1_dl_ch_state_change_cnf_struct;
typedef struct {
  msgr_hdr_struct_type                      msg_hdr;      ///< message router header
  lte_LL1_dl_ch_state_change_cnf_struct     msg_payload; 
} lte_LL1_dl_ch_state_change_cnf_msg_struct;


/*! @brief parameter structure PDCCH rnti change confirmation
*/
typedef struct { 
  lte_LL1_dl_cnf_enum err_code;
  lte_LL1_carrier_type_enum carrier_type;  ///< Enum for dl carriers in  CA
} lte_LL1_dl_pdcch_rnti_change_cnf_carrier_payload_struct;
typedef struct { 
  lte_LL1_cnf_header_struct cnf_hdr;
  uint8 num_carriers;                           ///< Number of carriers to be configured.
  lte_LL1_dl_pdcch_rnti_change_cnf_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_dl_pdcch_rnti_change_cnf_struct;
typedef struct {
  msgr_hdr_struct_type                      msg_hdr;      ///< message router header
  lte_LL1_dl_pdcch_rnti_change_cnf_struct   msg_payload; 
} lte_LL1_dl_pdcch_rnti_change_cnf_msg_struct;


/*! @ Deprecated
 * brief parameter structure PBCH state change confirmation
 */
typedef struct { 
  lte_LL1_cnf_header_struct cnf_hdr;
  lte_LL1_dl_cnf_enum err_code;
} lte_LL1_dl_pbch_state_change_cnf_struct;
typedef struct {
  msgr_hdr_struct_type                      msg_hdr;      ///< message router header
  lte_LL1_dl_pbch_state_change_cnf_struct   msg_payload; 
} lte_LL1_dl_pbch_state_change_cnf_msg_struct;

/*! @brief parameter structure DL ENABLE/DISABLE confirmation
*/
typedef struct { 
  lte_LL1_cnf_header_struct cnf_hdr;
  lte_LL1_dl_cnf_enum err_code;
  int16 freq_correction;    //total current correction/compensation 
  uint16 ref_sfn;           //SFN of the last radio frame before DL disable
  uint64 ref_time;          //64bit absolulte RTC time of the last radio frame start before DL disable
  uint32 lna_state[6];      //last lna state before DL disable - 6 for place holder, actual is TBD - odyseey specific (not used for espresso)
} lte_LL1_dl_enable_disable_cnf_struct;
typedef struct {
  msgr_hdr_struct_type                      msg_hdr;      ///< message router header
  lte_LL1_dl_enable_disable_cnf_struct      msg_payload; 
} lte_LL1_dl_enable_disable_cnf_msg_struct;

/*! @brief structure for CSF config confirmation */
typedef struct
{
   lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
   lte_LL1_csf_cfg_decfg_stat_enum config_status; ///<.
} lte_LL1_csf_config_cnf_carrier_payload_struct;

typedef struct
{
   lte_LL1_cnf_header_struct cnf_hdr;             ///<  confirmation header
   uint8 num_carriers;                           ///< Number of carriers to be configured.
   lte_LL1_csf_config_cnf_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_csf_config_cnf_struct;

/*! @brief Confirmation for CSF configuration */
typedef struct
{
  msgr_hdr_struct_type                  msg_hdr;                                          
  lte_LL1_csf_config_cnf_struct         msg_payload;
}lte_LL1_dl_csf_config_cnf_msg_struct;


/*! @brief Confirmation for CSF deconfiguration */
typedef struct
{
  msgr_hdr_struct_type                  msg_hdr;                                          
  lte_LL1_csf_config_cnf_struct         msg_payload;
}lte_LL1_dl_csf_deconfig_cnf_msg_struct;


/*! @brief Structure for RLM enable/disable request confirmation */
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;
  uint8 num_carriers;                           ///< Number of carriers to be configured.
  lte_LL1_carrier_type_enum carrier_type[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_rlm_cnf_struct;

/*! @brief Confirmation for RLM enable/diable request */
typedef struct
{
  msgr_hdr_struct_type          msg_hdr;                                          
  lte_LL1_rlm_cnf_struct        msg_payload;
}lte_LL1_dl_rlm_cnf_msg_struct;

// --------------------------------------------------------------------------
//
// Indication message parameter structures
//
// --------------------------------------------------------------------------

#define       LTE_LL1_DL_MAX_PDCCH_RESULTS                6

typedef struct { 
  boolean crc_status;                      ///< 1 = CRC_PASSED, 0 = CRC_FAILED;
  uint8 payload_size;                      ///< payload size
  lte_LL1_rnti_type_enum        rnti_type; ///< rnti type
  lte_LL1_cce_agg_level_enum    agg_level; ///< aggregation level: [0..3] = [1,2,4,8]
  lte_LL1_cce_search_space_enum search_space; ///< common = 0 or ue specific = 1
  uint32 energy_metric;                    ///< bits 0..20 Energy metric from decoder
  uint64                                    pdcch_payload;

  ///< flag indicating the type of sps grant received
  lte_LL1_dl_sps_conf_type                 sps_conf_type;    
  uint8                                    tx_ant_sel; 
} lte_LL1_dl_pdcch_ind_struct;

typedef struct { 
  uint8                                     pdcch_ind_tpc_bits;
  uint8                                     pdcch_ind_num_trblks;
  lte_l1_dci_format_e                       pdcch_ind_dci_format_type;
  uint16                                    pdcch_ind_n_cce;
  boolean                                   pdcch_ind_asrs_en;
  boolean                                   pdsch_skip;
  uint8                                     harq_id;

  //aggregate TB size (without CRC) for PDSCH in bytes. (Max TB size for CAT4: 150752 bits: per TS 36.306 Table 4.1-1)
  uint16                                    pdsch_tb_aggr_size_bytes; 
    
  uint8                                     pdcch_ind_dai;///< Downlink Assignment Index ( TDD only )

  /*! new tx/re-tx indication per TB: TRUE - new tx; FALSE - else  */
  boolean                                   pdcch_ind_new_tx[LTE_PDSCH_MAX_NUM_TBLK_PER_PROCESS];

} lte_LL1_dl_pdcch_ind_info_struct;


typedef struct {    
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA 

  uint8                     pdcch_phich_ind_mask; ///< bit(0: LTE_LL1_PDCCH_IND) set: PDCCH ind
                                                  ///< bit(1: LTE_LL1_PHICH_ACK_NACK_IND 0) set: PHICH0 value present
                                                  ///< bit(2: LTE_LL1_PHICH_ACK_NACK_IND 1) set: PHICH1 value present
                                                  ///<   i.e. IPhich == 1

  int16                                     sfn_phich;             ///< not used
  int8                                      subframe_index_phich;  ///< not used
  lte_l1_phich_result_e                     phich_val;
  uint8                                     num_pdcch_results; // up to 6

  // The "pdcch_ind_info" buffer has one to one mapping to "pdcch_ind_header_payload" buffer
  // If a grant is a (DL)PDSCH grant, then the corresponding "pdcch_ind_info" element 
  // in the array will contain a "non-zero" "pdcch_ind_num_trblks", 2 bits TPC value for PUCCH
  // power control and DCI format for DL Assignment.
  // If the grant is not DL grant, then the corresponding "pdcch_ind_num_trblks" in 
  // "pdcch_ind_info" buffer will set to "zero" and the tpc and DCI format will be invalid.

  lte_LL1_dl_pdcch_ind_info_struct          pdcch_ind_info[LTE_LL1_DL_MAX_PDCCH_RESULTS];
  lte_LL1_dl_pdcch_ind_struct               pdcch_ind_header_payload[LTE_LL1_DL_MAX_PDCCH_RESULTS];

  uint32                                    send_timestamp;       ///< Sleep counter timestamp when message was sent
  boolean  interf_active;       ///Result is interfered by other tech
  boolean  is_ul_dropped;

} lte_LL1_dl_pdcch_phich_ind_struct;

typedef struct {
lte_LL1_ind_header_struct				  header;	
uint8  num_carriers;
lte_LL1_dl_pdcch_phich_ind_struct	carrier[LTE_LL1_DL_NUM_CARRIERS];
}lte_LL1_dl_pdcch_phich_ind_payload_struct;

typedef struct {
msgr_hdr_struct_type					  msg_hdr;		///< message router header
lte_LL1_dl_pdcch_phich_ind_payload_struct		  msg_payload;
} lte_LL1_dl_pdcch_phich_ind_msg_struct;


typedef enum
{
  LTE_LL1_PDSCH_RESULT_CRC_FAILED = 0,
  LTE_LL1_PDSCH_RESULT_CRC_PASSED,
} lte_LL1_pdsch_result_e; 

/*!@brief lte_LL1_pdsch_tb_info_struct 
*/

typedef struct {
  lte_LL1_pdsch_result_e result;
  boolean pdsch_result_discard_retx;
  uint8 rv;
  uint8 ndi;
  uint8 tb_index;
  uint8 mcs;
  boolean did_recomb;
  uint32 tblk_size;
}lte_LL1_pdsch_tb_info_struct;

/*!@brief PDSCH result struct 
*/
typedef struct {
  uint8 num_rbs;    ///<	number or rbs allocated
  uint8 num_layers; ///<   number of layers
  uint8 pmch_id;    ///<	only applicable to pmch decodes
  uint8 area_id;    ///<	only applicable to pmch decodes
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
  lte_LL1_rnti_type_enum rnti_id;   ///<	RNTI type 
  uint8 harq_id;    ///<	HARQ id
  uint8 num_trbs;   ///< number of transport blocks: Up to 2 TBs for PDSCH.
  lte_LL1_pdsch_tb_info_struct tb[2]; ///< PDSCH info per TB

}lte_LL1_dl_pdsch_result_struct;

/*!@brief CA changes
*/
typedef struct {
  lte_LL1_ind_header_struct	header; 	
  uint32 send_timestamp;
  uint32 num_pdsch_result;   ///<Num of PDSCH result this msg includes
  lte_LL1_dl_pdsch_result_struct pdsch_result[LTE_LL1_MAX_PDSCH_GRANT_PER_TTI]; ///<	 	

}lte_LL1_dl_pdsch_stat_ind_struct;

typedef struct {
  msgr_hdr_struct_type                      msg_hdr;      ///< message router header
  lte_LL1_dl_pdsch_stat_ind_struct        msg_payload; 
} lte_LL1_dl_pdsch_stat_ind_msg_struct;


/*! Deprecated
 * PBCH result indication
 */
typedef struct {    
  lte_LL1_ind_header_struct                 header;

  lte_phy_cell_id_t  cell_id;
  uint8  rv;
  uint8  num_tx_ant;
  uint64 pbch_payload;
} lte_LL1_dl_pbch_result_ind_struct;

typedef struct {
  msgr_hdr_struct_type                      msg_hdr;      ///< message router header
  lte_LL1_dl_pbch_result_ind_struct         msg_payload; 
} lte_LL1_dl_pbch_result_ind_msg_struct;


/*@!brief DL BLER indication */
typedef struct {    
  lte_LL1_ind_header_struct          header;
  lte_LL1_carrier_type_enum          carrier_type; ///< Enum for dl carriers in  CA
  uint16                             filtered_bler_oos; //< Filtered BLER for out of sync
  uint16                             filtered_bler_is; //< Filtered BLER for in sync
} lte_LL1_dl_rlm_bler_ind_struct;

typedef struct 
{
  msgr_hdr_struct_type                  msg_hdr;
  lte_LL1_dl_rlm_bler_ind_struct        msg_payload;
}lte_LL1_dl_rlm_bler_ind_msg_struct;

/*@!brief ANT CORR result indication */
typedef struct {    
  lte_LL1_ind_header_struct                 header;

  uint32 phase;                      ///< relative phase between Rx0 and Rx1, Q-format: 32U31
                                     ///< range [0,2), representing [0, 2PI)
  int32 rssi[2];                     ///< RSSI per Rx antenna in dBm. Q-format: 32S24 
} lte_LL1_ant_corr_result_ind_struct;

typedef struct {
  msgr_hdr_struct_type                      msg_hdr;      ///< message router header
  lte_LL1_ant_corr_result_ind_struct        msg_payload; 
} lte_LL1_ant_corr_result_ind_msg_struct;


/*! 
    @brief Payload for CSF on the DL
*/
typedef struct
{
   lte_LL1_csf_sched_chan_type chan_type;               ///<.
   lte_LL1_pusch_csf_report_payload pusch_csf_report;   ///<.
}lte_LL1_ul_csf_report_payload_struct;


/*! @brief  */
typedef struct
{
   boolean sched_pusch_csf;             ///<.
}lte_LL1_pusch_csf_dyn_params_struct;

/*! @brief  */
typedef struct
{
//   lte_LL1_pucch_reporting_type_enum pucch_reporting_type;  ///<.
   uint32 bwp_index;   ///< Each Bandwidth part is a group of subbands. The BWP Index reflect 
                       ///< the group from which the CSF report will be generated in SubBand-CQI reporting.
   // For PUCCH mode, CSF CFG will use major mode for the configuration message: 1-0, 1-1, 2-0, 2-1. 
   lte_LL1_pucch_reporting_type_enum pucch_reporting_type; ///< main type that defines the PUCCH reporting period. 
                                                      ///< PUCCH mode 1-1 set to PUCCH_CSF_REPORT_TYPE_5 
                                                      ///< or PUCCH_CSF_REPORT_TYPE_3. 
                                                      ///< For PUCCH mode 2-1, set to PUCCH_CSF_REPORT_TYPE_6.

   // For TM 1 - 8 and TM9 with less than 8 CSI RS ports, the PUCCH only has one type choices in ML1:
   //              so ML1 should set the pucch_reporting_sub_type[0]/[1] to be set 
   //              the same as pucch_reporting_type - i.e., there is no report expansion
   // 
   // For TM9 with 8 CSI RS ports, due to double code book expansion in the report, ML1 will indicate sub-mode 
   //             in CSF schedule message option so that FW can decide which report to generate  and transmit. 
   //             This only apply to mode 1_1 and mode 2_1, when PMI report is involved.
   // For PUCCH mode 1-1, option is submode 1 and submode 2, and there is no ambiguity. ML1 will set the 
   //                     pucch_reporting_sub_type[1] = pucch_reporting_sub_type[2], options for 
   //                     (pucch_reporting_type, pucch_reporting_sub_type[0], pucch_reporting_sub_type[1])
   //                     value sets are:
   //                     submode1: (T5, T5, T5), (T5, T2b, T2b)
   //                     submode2: (T3, T3, T3), (T3, T2c, T2c)
   // For PUCCH mode 2-1, ML1 will include 2 choices of submode type, so that FW can decide to use the correct 
   //                     report type based on the PTI value in the current type 6 reporting period. the 
   //                     pucch_reporting_type = T6 always in this mode, and submode should be set as one
   //                     of the following options (pucch_reporting_type, pucch_reporting_sub_type[0], pucch_reporting_sub_type[1]):
   //                     (T6, T6, T6), (T6, T2a, T2b), (T6, T2b, T1a), (T6, T2a, T1a)
    lte_LL1_pucch_reporting_type_enum pucch_reporting_sub_type[2];

}lte_LL1_pucch_csf_dyn_params_struct;
                        
/*! @brief  */
typedef union
{
   lte_LL1_pucch_csf_dyn_params_struct pucch_dyn_params;    ///<.
   lte_LL1_pusch_csf_dyn_params_struct pusch_dyn_params;    ///<.
}lte_LL1_pucch_pusch_csf_dyn_params_union;
 
/*! @brief  */
typedef struct
{
   lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
   lte_LL1_csf_sched_chan_type chan_type; ///< Define whetehr this is periodic (0) or aperiodic(1) report 
   uint8 rs_sfn;                          ///< DL frame number where the OTA subframe is used for CSF generation 
   uint8 rs_subframe_num;                 ///< DL subframe where the OTA subframe is used for CSF generation 
   uint8 sched_interval_k;                ///< Subframe offset interval between DL subframe on which CSF report is requested
                                          ///<  and UL subframefrom on which CSF payload is sent
    boolean csf_oor_report_en;            ///< flag for an OOR csf reporting.
    uint8 forced_cqi;                     ///< FW will report this forced_cqi, if not equal to 0xFF
    uint8 forced_ri;                      ///< FW will report this forced_ri, if not equal to 0xFF
   
   lte_LL1_pucch_pusch_csf_dyn_params_union dyn_params_un;  ///<.
   // indicate which set this request is for:
   uint8 csi_meas_set_idx;               ///< 0: set 1, 1: set 2
}lte_LL1_csf_dyn_params_struct;

typedef struct 
{
  // A PUCCH report will be on at a (sfn,sfi) sbuframe when the following equation satisfies:
  // (sfn * 10 + sfi - pucch_csf_report_offset) % pucch_csf_report_period = 0
  uint8 pucch_csf_report_period;        ///< PUCCH report type period
  uint8 pucch_csf_report_offset;        ///< PUCCH report type offset
}lte_LL1_pucch_csf_report_loc_t;
/*! 
    @brief Used to configure semi-static parameters for periodic CQI/PMI/RI 
    reporting on PUCCH.
*/
typedef struct
{
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
  lte_l1_dl_tx_mode_e tx_mode;            ///< DL transmission mode
                                          
  // 36.213 Table 7.2-1b
  uint64 pmi_cb_subset_restriction_mask;  ///<  Bit mask for restricting the 
                                          ///<  set of PMI's used by the eNB
  // For TM9 8 CSI RS ports, mask can be up to 109 bits, add one more word
  // without having to change existing code. This is the most significant long word
  // of the two bit mask long words (64 bit long word)
  uint64 pmi_cb_subset_restriction_mask2_mslw;  ///<  bit 64 to bit 127 of PMI mask, LSB aligned

  // 36.331, 6.3.2, pmi-RI-Report: presense of this field means PMI/RI reporting
  // is configured; otherwise the PMI/RI reporting is not configured. Only on 
  // TM8, TM9 or TM10 should this flag set to TRUE. 
  // LTE system algorithm: when is_pmi_ri_reporting is TRUE in TM9,  FW will
  //                       use CSI-RS based CSF reporting; TM10 still uses CSI-RS
  //     if is_pmi_ri_report=FALSE, CSF will be based on CRS assuming SIMO or
  //                       tx diversity transmission depending on the number of CRS ports.
  //     for TM1 to 7, is_pmi_ri_report is NA, and ognored.                 
  boolean is_pmi_ri_report;

  lte_LL1_pucch_csf_report_mode_enum pucch_csf_reporting_mode; ///< PUCCH report mode: 1-0, 1-1, 2-0 or 2-1
  lte_LL1_pucch_csf_report_submode_enum pucch_csf_reporting_mode_1_1_submode; ///< PUCCH report mode 1-1 submode: 1 or 2

  // 36.213, section 7.2.2, for 8 CSI-RS based reporting on TM9, PUCCH reporting
  // has expansion on mode 1-1, and mode 2-1 for double code book.
  // for mode 1-1 submode 1, Type 5 and type 2B report are included within a reporting period. 
  //                         When we report type 5, the WB PMI_1 reported should be used in
  //                         subsequent type 2B location for PMI_2 reporting. 
  //                         This causes a limitation on FW schedule of CSF task,
  //                         as we will need to limit  type 2B location to 
  //                         only use the W1 from the preceding type 5 report.
  // Same limitation on PUCCH mode 2-1, type 2a location's WB PMI_1 will be used on
  //                         the subsequent type 2B report location
  // To handle CSF frontend schedule, we need to know the location when T5/T2B or
  //    T2A/T2B, or T2B/T1A are expected so we can make sure the periodic report is following the same W1.
  // For other mode, these two fields are do not care.
  lte_LL1_pucch_csf_report_loc_t pucch_wb_pmi1_loc; 
                                ///< for PUCCH mode 1-1 submode 1, this is type 5 periodicity location
                                ///< for PUCCH mode 2-1 3 subframe, this is the location of the 2nd subframe 
                                ///            report that contains WB_PMI1 (either type 2A (when PTI=0) or type 2B (when PTI=1))
  lte_LL1_pucch_csf_report_loc_t pucch_sub_report_conditioning_on_wb_pmi1_loc; 
                                ///< for PUCCH mode 1-1 submode 1, this is type 2B periodicity location
                                ///< for PUCCH mode 2-1 3 subframe, this is the location of the 3rd subframe 
                                ///            report (either type 2B (when PTI=0) or type 1A (when PTI=1))

  lte_l1_cqi_reporting_mode_aperiodic_e pusch_csf_reporting_mode; ///< aperiodic reporting mode

	// measurement set 1 (clean subframe) and set 2 (non-clean subframe). 
  // Note: for TDD, ML1 will send the correct configuration based on UL-DL
  //       configuration in the TDD system.
  // Note that CSI measurement set 1 and set 2 should be same period.
  // 40ms for FDD, 20/70/60ms for TDD
	// Note: only PCC has restricted measurement set definition. 
	//       for CSF, the set configuration is independent of serving cell or neighbor cell restricted set config.
  lte_LL1_meas_subframe_pattern_struct meas_set_mask[2];

  int8   non_rs_t2p_delta_offset;         ///< CSF specific TPR in dB, valid value: [-2, 0, 2, 4, 6, 8, 10, 12] 

}lte_LL1_dl_csf_config_req_carrier_payload_struct;

/*! @brief  */
typedef struct
{
   lte_LL1_req_header_struct   req_hdr;          ///< request header
   uint8 num_carriers;                           ///< Number of carriers to be configured.
   lte_LL1_dual_demod_csf_type_e  dual_demod_csf_type; 
   lte_LL1_dl_csf_config_req_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_dl_csf_config_req_struct;

LTE_LL1_INTF_MSG( dl_csf_config_req );

#define MAX_CSF_TYPE 4
/*! @brief  */
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;          ///< request header
  uint8  num_type[LTE_LL1_CARRIER_COUNT];
  uint32 sleep_duration;
  lte_LL1_csf_dyn_params_struct carrier[MAX_CSF_TYPE]; ///<.
} lte_LL1_dl_bundle_csf_sched_req_struct;

LTE_LL1_INTF_MSG( dl_bundle_csf_sched_req );

/*! @brief CSF schedule request message  */
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;          ///< request header
  uint8  num_carriers; 
  lte_LL1_csf_dyn_params_struct carrier[LTE_LL1_DL_NUM_CARRIERS]; ///<

} lte_LL1_dl_csf_sched_req_struct;

LTE_LL1_INTF_MSG( dl_csf_sched_req );


/*! @brief  */
typedef struct
{
   uint8 deconfig_status;                        ///<.
   lte_LL1_carrier_type_enum carrier_type;  ///< Enum for dl carriers in  CA
} lte_LL1_dl_csf_deconfig_req_carrier_payload_struct;

typedef struct
{
   lte_LL1_req_header_struct   req_hdr;          ///< request header
   uint8  num_carriers; 
   lte_LL1_dl_csf_deconfig_req_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///<.
} lte_LL1_dl_csf_deconfig_req_struct;

LTE_LL1_INTF_MSG( dl_csf_deconfig_req );

/*! @brief message structure for PBCH decode request message 
    Based on 01/24/14 LL-ML sync up, only mode is added to the request, no other
    param, e.g., the start/stop_rv or start/stop_sfn that specify timing, is 
    added as was originally proposed (by ML/LL): PBCH is non-time-critical & thus
    has no action timing in legacy mode. ML intends to keep it the same way for 
    ic mode, assuming that the use cases (every 3 hr MIB update or macro-pico HO)
    and timeline requirements are similar w.r.t the legacy.
    Added support for using dynamic frequency hypothesis on neighbour PBCH request.
*/
#define NUM_DYN_FREQ_OFFSET_PBCH (3)
typedef struct
{
  boolean use_dyn_freq_offset;          ///< If this flag is TRUE. We'll use dynamic freq offset. This is NOT used in init_acq PBCH.
  int32   dyn_freq_offset[NUM_DYN_FREQ_OFFSET_PBCH]; ///< in units of Hz
} lte_LL1_dl_pbch_dyn_freq_struct;

/*! tx ant cfg signaled by HO cmd, TX_CFG_0/1/2 mapped to 1/2/4 Tx. */
typedef enum
{
  TX_CFG_0 = 0,              ///< eNB has 1 Tx antenna
  TX_CFG_1 = 1,              ///< eNB has 2 Tx antennas
  TX_CFG_2 = 2,              ///< eNB has 4 Tx antennas
  MAX_NUM_TX_CFG             ///< Number of Tx ant not known
} lfw_enb_tx_ant_cfg_e;

typedef struct 
{
  lte_LL1_req_header_struct   req_hdr;  ///< Common request header
  lte_LL1_nb_id_enum nb_id;  ///< Enum for NB id mapping
  uint64 ref_time;           ///< 10 ms boundary
  lte_earfcn_t earfcn;       ///< earfch used as tag
  lte_phy_cell_id_t cell_ID; ///< cell id for this instance
  boolean serving_cell;      ///< Serving or non-serving
  uint16  carrier_freq;      ///< in units of 100KHz
  int16   freq_offset;       ///< in units of Hz
  boolean initial_pbch;      ///< Flag to indicate initial pbch
  uint8   max_frames;        ///< give up after this many frames
  lte_l1_cp_e cp_type;       ///< Normal or extended
  lte_LL1_pbch_mode_e mode;  ///< legacy or 1 of 3 ic pbch modes
  lte_LL1_dl_pbch_dyn_freq_struct dyn_freq_info; ///< Structure containing info of dyanmic frequency offset.
  lfw_enb_tx_ant_cfg_e  tx_ant_cfg;  ///< expected tx ant cfg
} lte_LL1_dl_pbch_decode_req_struct;

/*! @brief message structure for PBCH decode confirmation
*/
LTE_LL1_INTF_MSG( dl_pbch_decode_req );

/*! @brief info pertaining to one ic cell   
*/
typedef struct
{
  lte_phy_cell_id_t cell_ID;    ///< echo of requested cell_ID
  boolean crc_pass;             ///< 0 if fail, 1 if pass
  uint8  ic_cell_idx;           ///< crs ic cell idx; range 0~3; FW internal,
                                ///< let's confirm w/ ML1 to see if needed.
                                ///< good for dbg, let's keep it
  uint8  rv;                    ///< last two bits of sfn. serv-sync? or decoded rv?
  uint8  num_antennas;          ///< number of antennas for cell: 1, 2, or 4; invalid if crc_pass = 0
  uint32 payload;               ///< PBCH payload; invalid if crc_pass = 0
} lte_LL1_dl_pbch_ic_cell_t;

/*! @brief collection of all IC cells info
*/
typedef struct
{
  uint8 num_cells;          ///< num of existing IC cells
  lte_LL1_dl_pbch_ic_cell_t cells[LTE_LL1_MAX_NUM_PBCH_IC_CELLS]; 
} lte_LL1_dl_pbch_ic_info_t;

typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;  ///< confirmation message header
  lte_LL1_nb_id_enum nb_id;     ///< Enum for NB id mapping
  lte_LL1_dl_cnf_enum err_code; ///< success or fail
  uint64 ref_time;              ///< 10 ms boundary; invalid if crc_pass = 0
  lte_earfcn_t earfcn;          ///< echo of requested earfcn
  lte_phy_cell_id_t cell_ID;    ///< echo of requested cell_ID
  int16  freq_offset;           ///< updated frequency offset estimate (in Hz); 
                                ///< invalid if crc_pass = 0 or freq_offset_invalid
  boolean crc_pass;             ///< 0 if fail, 1 if pass
  uint32 payload;               ///< PBCH payload; invalid if crc_pass = 0
  uint8  rv;                    ///< last two bits of sfn
  uint8  num_decodes;           ///< number of 10 ms frames [1 to max_frames]    
  uint8  num_antennas;          ///< number of antennas for cell: 1, 2, or 4; invalid if crc_pass = 0
  boolean  interf_active;       ///<Result is interfered by other tech
  uint8  ic_cell_idx;           ///< FW cell idx; range 0~3 (0 for target cell)
                                ///< point to 1 cell in the CRS-IC ic cell list
  lte_LL1_pbch_mode_e mode;     ///< echo back mode sent in pbch decode req
  lte_LL1_dl_pbch_ic_info_t ic_cell_info;  ///< info concerning all IC cells
} lte_LL1_dl_pbch_decode_cnf_struct;

LTE_LL1_INTF_MSG( dl_pbch_decode_cnf );

/*! @brief message structure for PBCH stop decode request message 
 */
typedef struct 
{
  lte_LL1_req_header_struct req_hdr;     ///<Common request header
  lte_LL1_nb_id_enum nb_id;  ///< Enum for NB id mapping
  lte_earfcn_t earfcn;       // used to tag request
  lte_phy_cell_id_t cell_ID; // cell id
} lte_LL1_dl_pbch_stop_decode_req_struct;

/*! @brief message structure for PBCH stop decode confirmation
*/
LTE_LL1_INTF_MSG( dl_pbch_stop_decode_req );

typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr; ///<confirmation message header
  lte_LL1_nb_id_enum nb_id;          ///< Enum for NB id mapping
  lte_LL1_dl_cnf_enum err_code;     // success or fail
  uint32 earfcn;                    // echo of requested earfcn
  lte_phy_cell_id_t cell_ID;        // echo of requested cell_ID
} lte_LL1_dl_pbch_stop_decode_cnf_struct;

LTE_LL1_INTF_MSG( dl_pbch_stop_decode_cnf );

/*! @brief Parameters for search configuration request. ML sends
    this message if any of the parameters changed
*/
typedef struct
{
  lte_LL1_req_header_struct   req_hdr;    ///<Common request header
  uint32 num_params;
  lte_LL1_rxagc_cfg_element_struct_t rxagc_cfg_params[LTE_LL1_RXAGC_CFG_PARAM_COUNT];
} lte_LL1_rxagc_cfg_param_req_struct;

LTE_LL1_INTF_MSG( rxagc_cfg_param_req );

/*! @brief Structure for RX AGC config param confirmation structure.
*/
typedef struct
{
  lte_LL1_cnf_header_struct cnf_hdr;      ///<confirmation message header
} lte_LL1_rxagc_cfg_param_cnf_struct;

/*! @brief message structure for rxagc cfg param response message.
    It only contains msg_hdr.
*/
LTE_LL1_INTF_MSG( rxagc_cfg_param_cnf );

// --------------------------------------------------------------------------
//
// PMCH decode request bundled for a frame
// 
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
//
// PMCH decode info
// 
// --------------------------------------------------------------------------
/*! @brief
    Enumeration defines non-mbsfn region length in MBSFN subframe
*/
typedef enum
{
  LTE_L1_NON_MBSFN_SYMBOL_ONE_FW = 1,
  LTE_L1_NON_MBSFN_SYMBOL_TWO_FW = 2
} lte_l1_non_mbsfn_region_length_fw_e;

/*! @brief  PMCH decode info      
*/
typedef struct {
  //[Area ID + PMCH ID] needs to be add along with OTA time stamp in PMCH decode results to upper layers

  /*! PMCH decode OTA time [SFN, Subfn]: Frame should be same as frame in action time */
  lte_LL1_sys_time_struct               pmch_ota_time; ///< OTA action time of timed request 
  
  /*! MBSFN area id: [0 - 255] */
  uint8                                 area_id;
    
  /*! PMCH ID:  Only used by higher layers TB-PMCH identification mapping [0 - 15] */
  uint8                                 pmch_id;   

  /*! Non-MBSFN region length */
  lte_l1_non_mbsfn_region_length_fw_e   non_mbsfn_symbols;
 
  /*! MCS applicable for PMCH decode */
  uint8                                 mcs;     
  
} lte_LL1_dl_pmch_decode_info_struct;

/*! @brief  PMCH decode request bundled for PMCH decode in multiple subframes within 
            a frame.
 
            - Action time will have first PMCH decode [frame, subframe] 
            - This request is valid only for frame# as mentioned in action time
            - Request sent 'X' ms prior to first PMCH subframe for decode (e.g. 2ms)
              
            Assumption: PMCH decode in a partuclar subframe corresponds to single
            MBSFN area i.e. no PMCH allocations collision among MBSFN areas
            monitored by UE
 
            Action time interpretation:
            Req hdr should have action time for first valid PMCH [frame, subframe] to decode in bundle
            or some earlier subframe to make request available to FW asap
            e.g. frame X, subframes 1,2,3,7,8 PMCH info present in bundle, LTE_LL1_REQ_IGNORE_MISSED_ACT_TIME
            - Arrives on time: Schedule PMCH decodes for all PMCH in bundle
            - Arrives late: Schedule left possible PMCH decodes e.g. Arrive time [X, 2] then we can perform
              remanining [X,3], [X, 7], [X, 8] PMCH decodes
            - Arrives late: Schedule left possible PMCH decodes e.g. Arrive time [X, 8] or [X+1, 0] then we
              are not left with any PMCH decode. Drop request. 
*/
typedef struct {
  lte_LL1_req_header_struct             req_hdr;      ///< Common request parameters
  
  /*! Number of decodes requested in this frame */
  uint8                                 num_decodes;
  
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  /*! PMCH decode info for individual subframes */
  lte_LL1_dl_pmch_decode_info_struct    pmch_decode_info[LTE_LL1_MAX_PMCH_DECODES_PER_FRAME ];
} lte_LL1_dl_pmch_decode_req_struct;

LTE_LL1_INTF_MSG( dl_pmch_decode_req );

/* No DL_PMCH_DECODE_CNF needed by ML1 */
        
// --------------------------------------------------------------------------
//
// Stop PMCH monitor
//
// --------------------------------------------------------------------------
/*! @brief  Stop PMCH decodes            
*/
typedef struct {
  lte_LL1_req_header_struct   req_hdr;      ///< Common request parameters
  
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA
    
} lte_LL1_dl_pmch_decode_stop_req_struct;

LTE_LL1_INTF_MSG( dl_pmch_decode_stop_req );


/*! @brief  Stop PMCH decode confirmation message 
            - Once PMCH stop decode confirmation message sent to ML1, there
              should not be any pending PMCH decode at FW 
*/
typedef struct { 
  lte_LL1_cnf_header_struct   cnf_hdr;      ///< Common confirmation parameters
  
} lte_LL1_dl_pmch_decode_stop_cnf_struct;

LTE_LL1_INTF_MSG( dl_pmch_decode_stop_cnf );

// --------------------------------------------------------------------------
// borrow log packet: lte_LL1_log_mbsfn_whitened_matrices_ind_struct
// send it to ML1
// --------------------------------------------------------------------------

typedef lte_LL1_log_mbsfn_whitened_matrices_ind_struct lte_LL1_dl_embms_whitened_matrices_ind_struct;

/*! @brief  eMBMS whitened matrices */
LTE_LL1_INTF_MSG( dl_embms_whitened_matrices_ind );

// --------------------------------------------------------------------------
//
// IQ sample capture requests/confirmations
// 
// --------------------------------------------------------------------------
/*! @brief IQ sample capture enum for mempool buffer selection 
*/
typedef enum{
    IQ_CAPTURE_SRCH_PRIMARY_PATH=0, ///< SRCH TD mempool buffer
    IQ_CAPTURE_SRCH_SECONDARY_PATH, ///< SECONDARY SEARCH mempool buffer
    IQ_CAPTURE_DEMOD_PATH ///< DEMOD TD mempool buffer
}lte_LL1_iq_sample_capture_mempool_path_enum_t;

typedef enum{
IQ_CAPTURE_QSIZE_16=0,
IQ_CAPTURE_QSIZE_20
}lte_LL1_iq_sample_capture_qsize_enum_t;

/*! @brief IQ sample sample capture request 
*/
typedef struct
{
  lte_LL1_req_header_struct	req_hdr;	  ///< Common request header
	lte_LL1_carrier_type_enum carrier;		  // 0->PCC, 1->SCC_0
	uint8						rx_ant_bmask; ///< Rx Ant bit mask for capturing samples 
											  ///<	(bit0 for rx0, bit 1 for rx1. 1: enable, 0: disable)
											  ///<	valid setting 0x1 (RX Ant0), 0x2 (RX ANT1), 0x3 (both RX ANT0 & 1), 0x0 is invalid!
	lte_LL1_iq_sample_capture_mempool_path_enum_t mempool_path;///< Mempool location for source data
														///< SRCH or DEMOD
	lte_LL1_iq_sample_capture_qsize_enum_t q_size;
	uint8 prx_cap_idx;	  //capture buffer index of the PRX chain
	uint8 drx_cap_idx;	  //capture buffer index of the DRX chain
	uint16						num_samples;  ///< Number of samples to capture (16384),
} lte_LL1_dl_iq_sample_capture_req_struct;

LTE_LL1_INTF_MSG( dl_iq_sample_capture_req );


/*! @brief IQ sample capture confirmation
*/
typedef struct
{
  lte_LL1_cnf_header_struct  cnf_hdr;  ///< Confirmation message header
	 lte_LL1_carrier_type_enum carrier; 	   // 0->PCC, 1->SCC_0	
	 uint8						rx_ant_bmask; ///< Rx Ant bit mask for captured samples 
											   ///< (bit0 for rx0, bit 1 for rx1. 1: enabled, 0: disabled)
	 uint8 prx_cap_idx; 	 //capture buffer index of the PRX chain
	 uint8 drx_cap_idx; 	 //capture buffer index of the DRX chain											
	 uint16 					num_samples;  ///< Number of samples captured
         uint16 last_sample_ix;  //< last sample pointer in sample buffer (16K)
                                 //< only for prx, that of drx not needed per
                                 //< XO cal design requirements
} lte_LL1_dl_iq_sample_capture_cnf_struct;

LTE_LL1_INTF_MSG( dl_iq_sample_capture_cnf );

// --------------------------------------------------------------------------
//
// CRSIC_LQICE request:
//   - CRS_IC: can be enable or disable. if enable, Kalman (new VPE image) 
//     must be enabled already
//   - LQICE: can be enable or disable. If enable, CRS_IC must be enabled too.
// 
// --------------------------------------------------------------------------

/*! @brief CRS-IC noise estimate addback mode
           CRS-IC will cancel (subtract) IC cell's CRS; 
           but sometimes we need to add it back for noise estimate
  
  This is determined in ML1 algorithm, typical scenario:
  1. if serv cell is macro, because nbr pico cells do "not" have ABS blanking, assume noisy, always addback
  2. if serv cell is pico, joint demod (LQICE/dataIC) is on, then assume clean: no addback; because joint demod will model it
  3. if serv cell is pico, nbr macro cells participate in ABS, then let FW algorithm decide:
     no addback for ABS SF (because it is clean); addback for nonABS (because it is noisy)

  This is just the basic idea, ML1 algorithm will be different
*/
typedef enum {
  LTE_LL1_DL_NOISE_EST_ADDBACK_ON,
  LTE_LL1_DL_NOISE_EST_ADDBACK_OFF,
  LTE_LL1_DL_NOISE_EST_ADDBACK_FW_ALGORITHM
} lte_LL1_dl_noise_est_addback_mode_e;

/*! @brief IC cell info (needed if CRS-IC enable)
*/
typedef struct {

  uint16   cell_id;        /* cell_id for this IC cell */

  uint64   frame_bndry;    /* 64 bit frame boundary RTC counter, ML1 has it from srch */

  int32    freq_offset;    /* do we want this? ML1's value is probably very old */

  int32    rsrp;           /* ML1 has it from meas */

  /* these are HetNet assistance info from RRC; eNB may or may not have it */
  lte_LL1_mbsfn_cfg_s mbsfnCfg;        /* if not available, cfg_type = LTE_LL1_MBSFN_CFG_UNKNOWN; not disable */
  lte_LL1_num_ant_ports_enum  tx_port; /* if not available, LTE_LL1_NUM_ANT_PORTS_INVALID */
  
  boolean lqice_icell;    /* flag to indicate this ic cell is part of LQICE */
  
  lte_l1_pb_e p_b;        /* PDSCH type A and type B power ratio index (used by LQICE) */

  // Bit mask used by LQICE to indicate which P_A values to test:
  // P_A valid range is {-6, -4.77, -3, -1.77, 0, 1, 2, 3} in unit of dB (36.331, 6.3.2),
  // and bit-0 corresponds to -6dB, and bit-7 corresponds to 3dB.
  uint8   p_a_config_mask;  
  
  // to indicate whether to addback the cancelled CRS in noise estimate?
  // always addback, never addback, or let FW vary it from SF to SF
  lte_LL1_dl_noise_est_addback_mode_e noise_est_addback;

  // PHICH duration from IC cell MIB decode.
  // In case PHICH duration is unknown, ML1 should set this
  // field to LTE_L1_PHICH_MAX_NUM_DURATION_MODES.
  lte_l1_phich_duration_e phich_duration;

} lte_LL1_dl_ic_cell_info_struct;

/*! @brief LQICE operation mode enum. Different modes are mapped to 
           different clock/voltage settings.
*/
typedef enum {
  LTE_LL1_DL_LQICE_SVS_MODE,
  LTE_LL1_DL_LQICE_NORMAL_MODE,
  LTE_LL1_DL_LQICE_TURBO_MODE,
  LTE_LL1_DL_LQICE_MODE_COUNT
} lte_LL1_dl_lqice_mode_e;

/*! @brief CRSIC_LQICE per carrier request message payload
*/
typedef struct {
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  /* if enable, need the 3 IC cells' info: cell_info[]
     if disable, no need for cell_info[], this info is not used */
  boolean crsic_enable;

  /* max LTE_MAX_NUM_CRS_IC_CELLS IC cells */
  uint8 num_ic_cell;

  /* ic order for the serving cell.
    valid range: [0, num_ic_cell].
  */
  uint8 serv_cell_ic_order;

  /* If enabled, need at least 1 (up to 2) IC cells' with lqice_icell flag set 
     in cell_info[] array;
     If disabled, fw will ignore lqcie related fields in cell_info[] array
  */
  boolean lqice_enable;

  lte_LL1_dl_lqice_mode_e   lqice_op_mode;  ///< LQICE operation mode

  /* Bitmask for transmission modes monitored by LQICE:
      - bit0: TM2
      - bit1: TM3
      - bit2: TM4
      - bit3: other TM's
  */
  uint8 lqice_tm_config_mask;
  
  /* max 3 IC cell info */
  lte_LL1_dl_ic_cell_info_struct    cell_info[ LTE_MAX_NUM_CRS_IC_CELLS ];

} lte_LL1_dl_crsic_lqice_req_carrier_payload_struct;

/*! @brief CRSIC_LQICE request message
*/
typedef struct {
  lte_LL1_req_header_struct req_hdr;      ///< Common request parameters

  uint8 num_carriers;                           ///< Number of carriers to be configured.
  lte_LL1_dl_crsic_lqice_req_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_dl_crsic_lqice_req_struct;

LTE_LL1_INTF_MSG( dl_crsic_lqice_req );

/*! @brief CRSIC_LQICE request message CNF's per carrier payload
*/
typedef struct { 
  lte_LL1_carrier_type_enum carrier_type; ///< Enum for dl carriers in  CA

  boolean crsic_enable;                   ///< same as in req_msg

  /* max LTE_MAX_NUM_CRS_IC_CELLS IC cells */
  uint8 num_ic_cell;
 
  struct {
    uint16   cell_id;        /* cell_id for this IC cell */
    int32    freq_offset;    /* do we want this? FW compute the most current */
  } cell_info[ LTE_MAX_NUM_CRS_IC_CELLS ];

} lte_LL1_dl_crsic_lqice_cnf_carrier_payload_struct;

/*! @brief CRSIC_LQICE request message CNF
*/
typedef struct { 
  lte_LL1_cnf_header_struct cnf_hdr;      ///< Common confirmation parameters

  uint8 num_carriers;                           ///< Number of carriers to be configured.
  lte_LL1_dl_crsic_lqice_cnf_carrier_payload_struct carrier[LTE_LL1_DL_NUM_CARRIERS ]; ///< parameters per CA.
} lte_LL1_dl_crsic_lqice_cnf_struct;

LTE_LL1_INTF_MSG( dl_crsic_lqice_cnf );

/*! @brief  */
typedef struct
{
   lte_LL1_req_header_struct   req_hdr;          ///< request header
} lte_LL1_dl_pdcch_monitor_cancel_req_struct;

LTE_LL1_INTF_MSG( dl_pdcch_monitor_cancel_req );

/*! @brief  Deactivate SPS            
*/
typedef struct {
  lte_LL1_req_header_struct   req_hdr;      ///< Common request parameters
    
} lte_LL1_dl_sps_deactivate_req_struct;

LTE_LL1_INTF_MSG( dl_sps_deactivate_req );

#endif /* INTF_DL_H */
