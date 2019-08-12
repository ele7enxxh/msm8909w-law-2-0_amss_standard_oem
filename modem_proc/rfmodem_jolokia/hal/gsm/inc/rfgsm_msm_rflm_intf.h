#ifndef RFGSM_MSM_RFLM_HAL_H
#define RFGSM_MSM_RFLM_HAL_H
/*!
   @file
   rfgsm_msm_rflm_intf.h

   @brief
   Provides GSM MSM RFLM interface functions.

   @details
    The function interface between RFLM and the MSM is defined in this file.
*/

/*===========================================================================

Copyright (c) 2008, 2009, 2010, 2011, 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/hal/gsm/inc/rfgsm_msm_rflm_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/18/14   ec      Add definition for invalid JDET reading
11/28/14   tws     Add api to return jdet time and holdoff offsets 
11/17/14   sc      Read and calculate delay match DM1-3 in RFLM GSM TxFE config
10/23/14   sc      Program TxFE not TxET delay match registers - modem-specific
10/14/14   ec      Add definitions for improved control of multislot JDET readings
10/07/14   sc      Enable programming of all delay match settings in RFLM DRIF
09/09/14   ec      Add frame_id to HAL APIs
05/20/14   ec      Fix compilation warning due to redeclaration of macro
04/24/14   ec      Add changes for TxC tx (additional APIs), add in header info and missing rev
                   history, remove modem names from comments
03/17/14   ec      Initial version 

============================================================================*/

#include "rflm_gsm_dm.h"
#include "rflm_api_gsm.h"
#include "txlm_intf.h"


/* Jammer Detector Timer is from the start of PLL tune, so we account for 150 QS (138 us) */
#define RFLM_MSM_JDET_TIMING_PLL_MARGIN_QS 138

/* The following assume that QS and us are equivalent (1 us == 0.92 QS, but this is absorbed in the margins and avoids having to divide by float)*/
/* Allow 60 QS margin from when JDET read latches to when we can read it via CCS */
#define RFLM_MSM_JDET_TIMING_TRIGGER_TO_READ_MARGIN_QS 60

/* For Rx bursts, we set the timer to repeat at 500 us (max) and so we need to start the first measurement 350 qs into the burst */
#define RFLM_MSM_JDET_TIMING_TRIGGER_OFFSET_RX_QS   (RFLM_MSM_JDET_TIMING_PLL_MARGIN_QS + 350)
/* For monitor bursts (only 190 qs long) we need to read earlier. Repetition is not an issue as each monitor is single slot */
#define RFLM_MSM_JDET_TIMING_TRIGGER_OFFSET_MON_QS  (RFLM_MSM_JDET_TIMING_PLL_MARGIN_QS + 60)

/* Add margin to determine when readings can be retrieved via CCS (provided to GFW) */
#define RFLM_MSM_JDET_TIMING_READ_OFFSET_RX_QS      (RFLM_MSM_JDET_TIMING_TRIGGER_OFFSET_RX_QS - RFLM_MSM_JDET_TIMING_PLL_MARGIN_QS + RFLM_MSM_JDET_TIMING_TRIGGER_TO_READ_MARGIN_QS)
#define RFLM_MSM_JDET_TIMING_READ_OFFSET_MON_QS     (RFLM_MSM_JDET_TIMING_TRIGGER_OFFSET_MON_QS - RFLM_MSM_JDET_TIMING_PLL_MARGIN_QS + RFLM_MSM_JDET_TIMING_TRIGGER_TO_READ_MARGIN_QS)

/* Modem specific - set to 0xDEAD as this target is 15 bit 2s complement so MSB of 16 bits should be 0 for good reading */
#define RFLM_MSM_JDET_INVALID_VALUE 0xDEAD 

typedef enum
{
  RFGSM_MSM_MOD_GMSK,
  RFGSM_MSM_MOD_8PSK,
  RFGSM_MSM_MAX_MODS
}rfgsm_msm_mod_type;

/*! Modem specific settings which vary depending on modem */
typedef struct
{
    uint32  lmem_start_addr;       
    uint32  pp_gain;                
    uint32  pda_ctrl_wd;            
    uint32  rot_phase_inc;  
    int16   pa_up_t_delta;
    int16   pa_dn_t_delta;        
} rfgsm_msm_rflm_intf_prog_data_t;


/*! Generic RF parameters which should be common to DimePM and Bolt */
typedef struct
{
    rflm_gsm_txc_slot_timing_type      slot_timing;   //Timings for programming TxC slots
    rflm_gsm_ramp_type                 ramp_type;
} rfgsm_msm_rflm_intf_slot_data_t;

/*  A pointer to this type will be created and initialised at the start of the frame (in frame config), then passed into the Modem HAL at each programming point 
    The HAL will then extract the information it needs from this and then program/write memory accordingly. */
typedef struct
{
    uint32                              lmem_start_addr;        //Will be unused on DimePM
    txlm_chain_type                     chain;
    uint8                               slot_idx;       //<- Increments as each slot is programmed
    rfgsm_msm_rflm_intf_slot_data_t     slot_data[RFLM_GSM_MAX_TX_SLOT_PARAM_IDX];    //See #2 - Include KV slot
    rfgsm_msm_rflm_intf_prog_data_t     txc_prog_data;
} rfgsm_msm_rflm_intf_frame_data_t;


typedef struct
{
  boolean       predist_program;
  uint32        pp_enable;
  uint32        pp_override_val;
  boolean       pp_override_flag;
  uint32        ep_enable;
  uint32        ep_override_val;
  boolean       ep_override_flag;
} rfgsm_msm_rflm_intf_cal_overrides_t;

/* FUNCTION DEFINITIONS*/
boolean rfgsm_msm_rflm_setup_frame_intf ( txlm_chain_type chain, uint32 lm_handle, uint8 frame_id);

boolean rfgsm_msm_rflm_setup_frame_intf_cal_temp( txlm_chain_type chain, uint32* lmem_start_addr);

boolean rfgsm_msm_rflm_setup_txc_timing( txlm_chain_type chain, uint8 frame_id, rflm_gsm_txc_slot_timing_type* timing_info, int16 pa_ramp_up_delta, int16 pa_ramp_down_delta);

boolean rfgsm_msm_rflm_txc_afc_cfg( txlm_chain_type chain, rfgsm_msm_rflm_intf_prog_data_t* afc_info);

boolean rfgsm_msm_rflm_txfe_setup_ramp( txlm_chain_type chain, 
                                        uint8 frame_id,
                                        rflm_gsm_ramp_type ramp_type,
                                        uint32 transition_length,
                                        uint32 rflm_lut_index );

boolean rfgsm_msm_rflm_txfe_setup_cal_trans( txlm_chain_type chain,
                                             uint16 msm_reg_tag_id,
                                             uint32 data,
                                             uint32* env_phase_path_override_enable,
                                             boolean* program_amam_ampm_tables );

boolean rfgsm_msm_rflm_txfe_cal_ramp_cfg( txlm_chain_type chain, 
                                          rflm_gsm_ramp_type ramp_type,
                                          uint32 rflm_lut_index,
                                          uint32 env_phase_path_override_enable,
                                          uint16* pa_ramp_up,
                                          uint16* pa_ramp_down,
                                          rflm_dtr_tx_xfer_list_s* txfer_list );


boolean rfgsm_msm_rflm_txfe_prog_pp_gain( txlm_chain_type chain, uint8 frame_id, rfgsm_msm_mod_type modulation );

boolean rfgsm_msm_rflm_txfe_prog_pa_luts( txlm_chain_type chain, uint8 frame_id, uint16* pa_ramp_up, uint16* pa_ramp_down, rflm_dtr_tx_xfer_list_s* txfer_list  );

boolean rfgsm_msm_rflm_txfe_cal_prog_pa_luts( txlm_chain_type chain, uint16* pa_ramp_up, uint16* pa_ramp_down, rflm_dtr_tx_xfer_list_s* txfer_list, rflm_gsm_ramp_type ramp_mode );

boolean rfgsm_msm_rflm_txfe_prog_amxm_luts( txlm_chain_type chain, uint16* amam_profile, uint16* ampm_profile, rflm_dtr_tx_xfer_list_s* txfer_list  );

boolean rfgsm_msm_rflm_txfe_set_predistortion( txlm_chain_type chain, boolean predist_enable );

boolean rfgsm_msm_rflm_txfe_set_path_delay( rflm_handle_tx_t tx_handle,
                                            txlm_chain_type chain,
                                            int64 path_delay_val );

boolean rfgsm_msm_rflm_txfe_set_env_gain( txlm_chain_type chain, uint32 env_gain, rfgsm_msm_mod_type modulation);

boolean rfgsm_msm_rflm_txfe_get_dyn_txfer_params( txlm_chain_type chain, rflm_dtr_tx_xfer_list_s* txfer_list  );

boolean rfgsm_msm_rflm_txc_prog_amam_ampm_luts( txlm_chain_type chain, uint16* amam_table, uint16* ampm_table );

uint8 rfgsm_msm_rflm_txfe_set_slot_indx( txlm_chain_type chain, uint8 frame_id, uint8 slot_indx );

boolean rfgsm_msm_rflm_validate_txfer_list(rflm_dtr_tx_xfer_list_s* txfer_list );

boolean rfgsm_msm_rflm_get_jdet_offset( rflm_gsm_script_type rx_script_type, 
                                                           uint16* offset_in_qs );

boolean rfgsm_msm_rflm_get_jdet_holdoff_time( uint16* normal_holdoff_time_in_us, uint16* mon_holdoff_time_in_us );

#endif // RFGSM_MSM_RFLM_HAL_H
