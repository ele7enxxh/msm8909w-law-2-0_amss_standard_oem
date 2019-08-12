
#ifndef RFLM_DTR_TX_STRUCT_AG_H
#define RFLM_DTR_TX_STRUCT_AG_H


#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated at:    Tue Dec 22 19:08:19 2015
Generated using: lm_autogen.exe v5.1.36
Generated from:  v6.5.3 of Jolokia_TXLM_register_settings.xlsx
*/

/*=============================================================================

           T X    A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the modem register settings 
  configured by FW, provided by the rflm_dtr_tx.

Copyright (c) 2009, 2010, 2011, 2012, 2013, 2014, 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

$DateTime: 2016/12/13 07:59:25 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/lm/inc/rflm_dtr_tx_struct_ag.h#1 $

=============================================================================*/

/*=============================================================================
                           REVISION HISTORY
Version    Author   Date   
         Comments   IR Number   Other POC (Comma Delimited)   Release Validation Notes   Dependencies   AG File Location   Changes for   
6.5.3   nichunw   12/21/2015   
         1. Updated SYSBDY_duration_NANOSECS  for LTE10MHz.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            LTE   
6.5.2   nichunw   12/18/2015   
         1. Fixed REF_I_GAIN/REF_Q_GAIN to 1 for HDR.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            C2K   
6.5.1   nichunw   11/30/2015   
         1. Update SYSBDY_duration_NANOSECS for 1x from 192 cx1 to 384 cx1.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            C2K   
6.5.0   nichunw   11/4/2015   
         1. Separate TXR_IQ_DAC_An from TXR block to a new TXR_IQ_DAC block (runtime dynamic). C2K needs to dynamically updating this register on hand-off between low-band and mid/high-band in WTR2955.       nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            C2K   
6.4.0   nichunw   10/17/2015   
         1. Added tech system boundary duration in TxLM var,SYSBDY_duration_NANOSECS, for autopin feature.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            Autopin   
6.3.2   nichunw   8/28/2015   
         1. Enabled TWO_SAMPLES_CYCLE for DO, to accommodate the lower offline clock rate (72MHz).      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DO   
6.3.1   nichunw   5/21/2015   
         1. Added modem spec column to handle the SWI difference between Cheel and Lykan. 2. Remove GSM chain 1 state.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.3.0   nichunw   5/18/2015   
         FW interface change for LTE power optimization. (No spreadsheet change).      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            LTE   
6.2.8   sunilk   2/11/2015   
         updates to cal/mission-mode DAC_CFG0/1/2 values from Filip; new DAC_IREF LUT for GF in the IREF_Dynamic tab      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DAC   
6.2.7   sunilk   2/11/2015   
         1. Change REF_DWN_FACTOR to 1 for TDS_sAPT states in DAC rate 230.4MHz to lower down the DPD capture rate by half.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            TDS   
6.2.6   nichunw   11/18/2014   
         1. Added foundry dependent DAC configuration for TSMC and GF.       nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DAC   
6.2.5   nichunw   10/29/2014   
         1. Enable PEQ2/SORU blocks for LTE and WCDMA (with coefficients for WTR4905).      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            LTE, UMTS   
6.2.4   nichunw   10/21/2014   
         1. Added UMTS_SC_DAC_230p4MHz_chain0_CW state.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            UMTS   
6.2.3   nichunw   10/21/2014   
         1. Updated COMBODAC_CFG1_0 to enable reading back the latched IREF gain.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DAC   
6.2.2   nichunw   10/2/2014   
         1. Added COMBODAC_COMP_CTR::IQ_EN_IMMED=0 for all states. 2. Switch off PEQ/SORU before getting updated coefficients for WTR2955. 3. Updated DP_PEQ_SW_Dynamic tab to add UMTS with WTR4905. 4. Updated COMBODAC_CFGx in DAC_cal_LM_settings tab.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.2.1   nichunw   8/15/2014   
         1. Change modem version for TXFE_DELAYMATCH block from DimePM to Jolokia. 2. Added the fix of DAC avg_error and overflow bits (ported from DimePM)  3. Added COMBODAC_CLK_SRC_CFG::DAC_FCAL_CLK_SEL and COMBODAC_COMP_CTRL::DAC_COMP_MODE, so that after MCPM sleep the register values are reprogrammed.       nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.2.0   nichunw   7/29/2014   
         1. . Updated PEQ1/PEQ2/SORU coefficients for WTR4905, both on main tab and DP_PEQ_SW_dynamic tab. 2. Added two new txlm variable TXC_input_freq_hz and DAC_freq_khz. 3. Changed state specific mask write to normal mask write for COMBODAC_CLK_SRC_CFG. 4. Added COMBODAC_CAL_CTRL_0::FSM_CAL_MODE=2 for all states.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.1.3   nichunw   6/5/2014   
         1. Change d index to 0 index for register COMBODAC_CAL_STATUS.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DAC   
6.1.2   nichunw   6/2/2014   
         1. Added GSM_DAC_76p8MHz_Anritsu_chain0 state.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            GSM   
6.1.1   nichunw   5/28/2014   
         1. Changed An indices under TXC0 and TXC_DP_IQ_GAIN blocks to A0. 2. Changed d indices for COMBODAC_CFG to 0.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.1.0   nichunw   5/22/2014   
         1. Updated DAC config settings for mission mode and factory cal mode. 2. Change to P3 SWI names (DAC registers use d_index) 3. Added TXFE_DELAYMATCH block for GSM to dynamically change the DM values. 4. Added Foundry column for foundry-dependent DAC_cfg. 5. Change TWO_SAMPLES_CYCLE to 1 for 1x and TDS 230.4MHz. 6. Disable EP_PEQ1/PEQ2_ENABLE for LTE, UMTS, 1x, DO. EP is not used by those techs. 7. Add Bolt indices to IREF LUT. SW should use Bolt indices to match indices used on WTR.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.0.4   nichunw   5/15/2014   
         1. Change PP_GAIN for GSM 76.8MHz to 44373.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            GSM   
6.0.3   nichunw   5/6/2014   
         1. Change two_sample_cycle to 1 for TDS and 1x.  2. Added mask write to COMBODAC_CLK_SRC_CFG.  3. Set TX_MEM_EN=1 for LTE.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.0.2   nichunw   4/21/2014   
         1. Revert the DAC-related register names to match HW P2 release to unblock FW.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.0.1   nichunw   4/16/2014   
         1. Removed COMBODAC_DIGVAL_EN_d from the main tab. This is to optimize the single-Tx DSDA timeline, so txlm config can be done outside the transition time gap.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DSDA   
6.0.0   nichunw   4/3/2014   
         "Iinitial version.                     
Derived from DimePM txlm 5.3.7 with the following changes:                              
1. DAC rate=230.4MHz for all non-GSM states and 76.8MHz for GSM states.                              
2. Only GSM on chain 1.                              
3. New TXC0 block for chain 0 only registers.                              
4. Updated TX_COMMON block to Triton-based TX registers.                              
5. Updated field names in TXC/TXR."      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com   
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rflm_dtr_tx_fw_intf_ag.h" 
#include "rflm_dtr_tx_typedef_ag.h" 



/* This struct captures flags that control whether certain groups of registers are programmed for a given state. */
typedef struct
{
  boolean mtc_common_flag;/* This flag enables programming of the TXC registers to program both the MTC_TRIG blocks*/
  boolean mdm_clk1_flag;/* This flag enables programming of the MTC registers to program the CLK_DAC for Tx chain 1*/
  boolean mdm_clk0_flag;/* This flag enables programming of the MTC registers to program the CLK_DAC for Tx chain 1*/
  boolean tx_common_flag;/* This flag enables programming of the TXC registers to program both the Tx chains*/
  boolean iref_update_flag;/* This flag enables programming of the DAC IREF registers to program both the IREF gain value for both chains*/
  boolean txc_flag;/* This flag enables programming of the TXC registers to program both chains*/
  boolean txc0_flag;/* This flag enables programming of the TXC registers to program the Tx chain 0*/
  boolean txr_flag;/* This flag enables programming of the TXR registers to program the Tx chain n*/
  boolean txr_iq_dac_flag;/* This flag enables programming of the TXR_IQ_DAC registers to program the Tx chain n*/
}rflm_dtr_tx_block_valid_flags_struct;



 /* Group MTC_TRIG: */ 
/* This block consist of the TXC registers to program both the MTC_TRIG blocks*/
typedef struct
{
  uint32 tx_cn_ctrl_sel;
  uint32 tx_cn_trig_cmd;
  uint32 tx_cn_trig_val;
}rflm_dtr_tx_mtc_trig_group_struct;



 /* Group DACCOMP_MUX: */ 
/* This block consist of the TXC registers to program both the MTC_TRIG blocks*/
typedef struct
{
  uint32 daccomp_mux_chainn_strt_val;
  uint32 daccomp_mux_chainn_strt_trig_cmd;
  uint32 daccomp_mux_chainn_stop_val;
  uint32 daccomp_mux_chainn_stop_trig_cmd;
}rflm_dtr_tx_daccomp_mux_group_struct;



 /* Group DAC_CLK: */ 
/* This block consist of the TXC registers to program both the MTC_TRIG blocks*/
typedef struct
{
  uint32 combodac_clk_src_cfg;
}rflm_dtr_tx_dac_clk_group_struct;



 /* Group MTC1: */ 
/* This block consist of the MTC registers to program the CLK_DAC for Tx chain 1*/
typedef struct
{
  uint32 combodac1_clk_cfg;
}rflm_dtr_tx_mtc1_group_struct;



 /* Group MTC: */ 
/* This block consist of the MTC registers to program the CLK_DAC for Tx chain 1*/
typedef struct
{
  uint32 combodac0_clk_cfg;
}rflm_dtr_tx_mtc_group_struct;



 /* Group TX_EN: */ 
/* This block consist of the TXC registers to program both the Tx chains*/
typedef struct
{
  uint32 tx_enable;
}rflm_dtr_tx_tx_en_group_struct;



 /* Group TX_TOP: */ 
/* This block consist of the TXC registers to program both the Tx chains*/
typedef struct
{
  uint32 tx_enable_gsm_c;
  uint32 tx_mode_common;
  uint32 tx_mode_common_mask;
  uint32 tx_tds_ctrl;
  uint32 tx_tds_ctrl_mask;
  uint32 tx_mode_ofdma;
  uint32 tx_mode_ofdma_mask;
  uint32 tx_mem_access_cfg;
  uint32 tx_tech_mux_ctrl;
  uint32 tx_tech_mux_ctrl_mask;
  uint32 tx_stmr_trig_frac_cnt_c;
  uint32 tx_mode_c;
}rflm_dtr_tx_tx_top_group_struct;



 /* Group TX_DAC_CFG: */ 
/* This block consist of the TXC registers to program both the Tx chains*/
typedef struct
{
  uint32 combodac_cfg0_0;
  uint32 combodac_cfg1_0;
  uint32 combodac_cfg2_0;
}rflm_dtr_tx_tx_dac_cfg_group_struct;



 /* Group TX_DAC_MODE: */ 
/* This block consist of the TXC registers to program both the Tx chains*/
typedef struct
{
  uint32 combodac_cal_ctrl_0;
}rflm_dtr_tx_tx_dac_mode_group_struct;



 /* Group DAC_COMP_MODE: */ 
/* This block consist of the TXC registers to program both the Tx chains*/
typedef struct
{
  uint32 combodac_comp_ctrl;
}rflm_dtr_tx_dac_comp_mode_group_struct;



 /* Group AXIW: */ 
/* This block consist of the TXC registers to program both the Tx chains*/
typedef struct
{
  uint32 tx_axi_wr_intf_event;
  uint32 tx_axi_wr_intf_event_mask;
}rflm_dtr_tx_axiw_group_struct;



 /* Group AXIR: */ 
/* This block consist of the TXC registers to program both the Tx chains*/
typedef struct
{
  uint32 tx_axi_rd0_intf_ctl;
  uint32 tx_axi_rd0_intf_ctl_mask;
  uint32 tx_axi_rd0_intf_event;
  uint32 tx_axi_rd0_intf_event_mask;
  uint32 tx_axi_rd1_intf_ctl;
  uint32 tx_axi_rd1_intf_ctl_mask;
  uint32 tx_axi_rd1_intf_event;
  uint32 tx_axi_rd1_intf_event_mask;
}rflm_dtr_tx_axir_group_struct;



 /* Group IREF: */ 
/* This block consist of the DAC IREF registers to program both the IREF gain value for both chains*/
typedef struct
{
  uint32 tx_dacn_iref_gain;
  uint32 tx_dacn_update_strt_val;
  uint32 tx_dacn_update_strt_trig_cmd;
  uint32 tx_dacn_update_stop_val;
  uint32 tx_dacn_update_stop_trig_cmd;
}rflm_dtr_tx_iref_group_struct;



 /* Group TIMESTAMP_TXC: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_timestamp_t1_an;
  uint32 txc_timestamp_t2_an;
}rflm_dtr_tx_timestamp_txc_group_struct;



 /* Group TXC_DP_CFG: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_dp_cfg_an;
}rflm_dtr_tx_txc_dp_cfg_group_struct;



 /* Group TXC: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_dp_env_scale_val_an;
  uint32 txc_dp_env_scale_val_set2_an;
  uint32 txc_dp_phase_ovr_val_an;
  uint32 txc_pp_phase_ovr_val_an;
  uint32 txc_dp_env_ovr_val_an;
  uint32 txc_ep_env_ovr_val_an;
  uint32 txc_ep_env_ovr_val_set2_an;
  uint32 txc_ep_gain_an;
  uint32 txc_ep_gain_set2_an;
  uint32 txc_pp_gain_an;
  uint32 txc_pp_spill_an;
  uint32 txc_timestamp_reuse_an;
  uint32 txc_sample_count_ctl_an;
  uint32 txc_amam_pending_an;
  uint32 txc_ampm_pending_an;
  uint32 txc_amam_pending_set2_an;
  uint32 txc_ampm_pending_set2_an;
}rflm_dtr_tx_txc_group_struct;



 /* Group RAMP: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_dp_ramp_arm_an;
  uint32 txc_dp_ramp_start_time_an;
  uint32 txc_dp_ramp_slot_a_cfg_an;
  uint32 txc_dp_ramp_slot_b_cfg_an;
  uint32 txc_dp_ramp_gap_an;
}rflm_dtr_tx_ramp_group_struct;



 /* Group ROT: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_dp_rot_phase_inc_an;
  uint32 txc_dp_rot_phase_inc_set2_an;
  uint32 txc_dp_rot_phase_init_an;
  uint32 txc_dp_rot_phase_init_set2_an;
  uint32 txc_dp_rot_phase_shift_an;
  uint32 txc_dp_rot_phase_shift_set2_an;
}rflm_dtr_tx_rot_group_struct;



 /* Group DC: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_ep_dcoc_an;
  uint32 txc_ep_dcoc_set2_an;
  uint32 txc_dp_dcoc_a0;
  uint32 txc_dp_dcoc_set2_a0;
}rflm_dtr_tx_dc_group_struct;



 /* Group DM: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_dm1_cfg_an;
}rflm_dtr_tx_dm_group_struct;



 /* Group REF: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_ref_arm_an;
  uint32 txc_ref_dwn_factor_an;
  uint32 txc_ref_num_samples_an;
  uint32 txc_ref_start_time_an;
  uint32 txc_ref_mode_an;
  uint32 txc_ref_i_gain_val_an;
  uint32 txc_ref_q_gain_val_an;
  uint32 txc_ref_powercalc_or_log_an;
}rflm_dtr_tx_ref_group_struct;



 /* Group DBG: */ 
/* This block consist of the TXC registers to program both chains*/
typedef struct
{
  uint32 txc_testbus_select_an;
}rflm_dtr_tx_dbg_group_struct;



 /* Group DP_CFG: */ 
/* This block consist of the TXC registers to program the Tx chain 0*/
typedef struct
{
  uint32 txc_tech_sel_a0;
  uint32 txc_dp_cfg_2_a0;
}rflm_dtr_tx_dp_cfg_group_struct;



 /* Group IQMC: */ 
/* This block consist of the TXC registers to program the Tx chain 0*/
typedef struct
{
  uint32 txc_dp_iqmc_a0;
  uint32 txc_dp_iqmc_set2_a0;
}rflm_dtr_tx_iqmc_group_struct;



 /* Group PEQ: */ 
/* This block consist of the TXC registers to program the Tx chain 0*/
typedef struct
{
  uint32 txc_dp_peq1_a0;
  uint32 txc_dp_peq1_set2_a0;
  uint32 txc_dp_peq1_q_a0;
  uint32 txc_dp_peq1_q_set2_a0;
  uint32 txc_dp_peq2_a0;
  uint32 txc_dp_peq2_set2_a0;
}rflm_dtr_tx_peq_group_struct;



 /* Group EP_PEQ: */ 
/* This block consist of the TXC registers to program the Tx chain 0*/
typedef struct
{
  uint32 txc_ep_peq1_a0;
  uint32 txc_ep_peq1_set2_a0;
  uint32 txc_ep_peq2_a0;
}rflm_dtr_tx_ep_peq_group_struct;



 /* Group TXFE_DELAYMATCH: */ 
/* This block consist of the delay matching registers to program the Tx chain n*/
typedef struct
{
  uint32 txc_ep_dm2_fine_tau_an;
  uint32 txc_ep_dm3_frac_tau_an;
}rflm_dtr_tx_txfe_delaymatch_group_struct;



 /* Group DPIQGAIN_TXC: */ 
/* This block consist of the DP_IQ_GAIN registers to program the Tx chain 0*/
typedef struct
{
  uint32 txc_dp_iq_gain_a0;
  uint32 txc_dp_iq_gain_a0_mask;
  uint32 txc_dp_iq_gain_set2_a0;
  uint32 txc_dp_iq_gain_set2_a0_mask;
}rflm_dtr_tx_dpiqgain_txc_group_struct;



 /* Group TXR: */ 
/* This block consist of the TXR registers to program the Tx chain n*/
typedef struct
{
  uint32 txr_str_ctrl_an;
  uint32 txr_pda_ctrl_an;
  uint32 txr_pda_wd_update_ctrl_an;
  uint32 txr_pda_ctrl_wd_offset_an;
  uint32 txr_pda_offset_cmd_an;
  uint32 txr_fifo_cmd_an;
  uint32 txr_fifo_ctrl_an;
}rflm_dtr_tx_txr_group_struct;



 /* Group PPDSM: */ 
/* This block consist of the TXR registers to program the Tx chain n*/
typedef struct
{
  uint32 txr_ppdsm_ctl_an;
  uint32 txr_ppdsm_override_val_an;
  uint32 txr_ppdsm_override_rep_an;
}rflm_dtr_tx_ppdsm_group_struct;



 /* Group TXR_STREAM: */ 
/* This block consist of the TXR registers to program the Tx chain n*/
typedef struct
{
  uint32 txr_stream_if_ctrl_an;
  uint32 txr_fmt_conv_ctl_an;
}rflm_dtr_tx_txr_stream_group_struct;



 /* Group TXR_IQ_DAC: */ 
/* This block consist of the TXR_IQ_DAC registers to program the Tx chain n*/
typedef struct
{
  uint32 txr_iq_dac_an;
}rflm_dtr_tx_txr_iq_dac_group_struct;



// SW FW Interface Buffer

typedef struct ALIGN(32)
{
  rflm_dtr_tx_xo_vars_group_struct xo_vars_params;
  rflm_dtr_tx_sw_xo_vars_group_struct sw_xo_vars_params;
  rflm_dtr_tx_dig_delay_vars_group_struct dig_delay_vars_params;
  rflm_dtr_tx_analog_delay_vars_group_struct analog_delay_vars_params;
  rflm_dtr_tx_txagc_dm_vars_group_struct txagc_dm_vars_params;
  rflm_dtr_tx_txlm_vars_group_struct txlm_vars_params;
  rflm_dtr_tx_mtc_trig_group_struct mtc_trig_params;
  rflm_dtr_tx_daccomp_mux_group_struct daccomp_mux_params;
  rflm_dtr_tx_dac_clk_group_struct dac_clk_params;
  rflm_dtr_tx_mtc1_group_struct mtc1_params;
  rflm_dtr_tx_mtc_group_struct mtc0_params;
  rflm_dtr_tx_tx_en_group_struct tx_en_params;
  rflm_dtr_tx_tx_top_group_struct tx_top_params;
  rflm_dtr_tx_tx_dac_cfg_group_struct tx_dac_cfg_params;
  rflm_dtr_tx_tx_dac_mode_group_struct tx_dac_mode_params;
  rflm_dtr_tx_dac_comp_mode_group_struct dac_comp_mode_params;
  rflm_dtr_tx_axiw_group_struct axiw_params;
  rflm_dtr_tx_axir_group_struct axir_params;
  rflm_dtr_tx_iref_group_struct iref_params;
  rflm_dtr_tx_timestamp_txc_group_struct timestamp_txc_params;
  rflm_dtr_tx_txc_dp_cfg_group_struct txc_dp_cfg_params;
  rflm_dtr_tx_txc_group_struct txc_params;
  rflm_dtr_tx_ramp_group_struct ramp_params;
  rflm_dtr_tx_rot_group_struct rot_params;
  rflm_dtr_tx_dc_group_struct dc_params;
  rflm_dtr_tx_dm_group_struct dm_params;
  rflm_dtr_tx_ref_group_struct ref_params;
  rflm_dtr_tx_dbg_group_struct dbg_params;
  rflm_dtr_tx_dp_cfg_group_struct dp_cfg_params;
  rflm_dtr_tx_iqmc_group_struct iqmc_params;
  rflm_dtr_tx_peq_group_struct peq_params;
  rflm_dtr_tx_ep_peq_group_struct ep_peq_params;
  rflm_dtr_tx_txfe_delaymatch_group_struct txfe_delaymatch_params;
  rflm_dtr_tx_dpiqgain_txc_group_struct dpiqgain_txc_params;
  rflm_dtr_tx_timestamp_txc_group_struct timestamp_params;
  rflm_dtr_tx_txr_group_struct txr_params;
  rflm_dtr_tx_ppdsm_group_struct ppdsm_params;
  rflm_dtr_tx_txr_stream_group_struct txr_stream_params;
  rflm_dtr_tx_txr_iq_dac_group_struct txr_iq_dac_params;
  rflm_dtr_tx_indices_struct rflm_dtr_tx_reg_indices;
  rflm_dtr_tx_block_valid_flags_struct rflm_dtr_tx_block_valid;
  rflm_dtr_tx_header_struct tx_header;
}rflm_dtr_tx_settings_type_ag;




#ifdef __cplusplus
}
#endif



#endif


