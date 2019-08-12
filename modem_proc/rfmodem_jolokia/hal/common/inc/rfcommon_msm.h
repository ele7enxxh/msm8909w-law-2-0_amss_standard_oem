#ifndef RFCOMMON_MSM_H
#define RFCOMMON_MSM_H

/*!
  @file rfcommon_msm.h

  @brief
  This file contains Common HAL for the target.
*/

/*============================================================================== 
   
  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
   
==============================================================================*/ 

/*==============================================================================
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/hal/common/inc/rfcommon_msm.h#1 $ 
   
when       who     what, where, why 
--------   ---     -------------------------------------------------------------
03/05/15   vv      Added support for ADC DC calibration
08/12/14   sml     Added DAC mission enter mode configuration API to take into 
                   account DAC reset after XO shutdown
08/14/14   aak     Move MGPI block config to Modem HAL module
06/19/14   aa      Updated the IQMC helper function to return the iqmc value
06/04/14   dej     Fix Compiler errors
04/25/14   vr      Link error fix
04/22/14   dej     Added a new API to change down_factor
04/02/14   dej     Implement common function to select DTR device parameters
02/11/14   ak      Implement Common Notch allocation for Bolt/Bolt+
12/20/13   cri     Enable ET Delay 
12/19/12   av      Delay match registers computation
10/25/13   cvd     DAC IREF caliberation support
10/16/13   ra      Support RFLM CCS 
10/16/13   cd      Add support for querying IREF LUT index
04/26/13   sar     Added macro to define missing API for modem lite builds.
04/17/13   aro     Cleaned up unused time profiler code
02/11/13   aro     Interface to return the supported XPT mode
01/11/13   aca     Dac cal cleanup
11/15/12   tks     Added support for rsb computation in 20pcfl format
10/29/12   sty     Renamed interface and type definitions for timer APIs
08/27/12   jfc     Rename rfcommon_msm_init_sbi to rfcommon_msm_init() 
08/22/12   aro     Renamed time profling functions
08/22/12   aro     Converted get_time_difference function to take start and
                   end time ticks for difference calculation
07/31/12   jfc     Include "rfcommon_time_profile.h" 
07/30/12   cvd     Implement the  rfcommon_msm_timtick_get and 
                   rfcommon_msm_timtick_difference interfaces.
07/19/12   cvd     Added the rfcommon_msm_timtick_get and 
                   rfcommon_msm_timtick_difference interfaces.
07/21/12   zhw     Unit conversion function deal with int32 instead of uint32 
07/16/12   zhw     Return uint32 rfcommon_msm_convert_microsecs_to_ustmr_units()
06/20/12   sty     Added rfcommon_msm_convert_microsecs_to_ustmr_units()
03/22/12   vb      Added function for programming DAC config HWIO registers.
03/22/12   vb      Added function for reading DAC config HWIO registers.
02/17/12   sty     Added FIX_POINT
02/06/12   sty     Renamed API to rfcommon_msm_get_spurs_to_suppress
01/26/12   sty     Minor changes to rfcommon_msm_retrieve_notch_entries
01/26/12   sty     Added rfcommon_msm_retrieve_notch_entries()
01/17/12   pl      Added sine function for NikeL
12/02/11   bmg     Updated LM parameters passed to
                   rfcommon_msm_get_bbrx_iq_mismatch_gain()
11/09/11   vrb     Added API to read and return the Modem ID
09/12/11   bmg     Added rfcommon_msm_get_bbrx_iq_mismatch_gain() for NikeL
                   I/Q channel imbalance in BBRx.
08/10/11   pl      Remove all references to legacy RFC structure and related API
07/08/10   tnt     RFC re-architecture to support GRFC
05/27/10   av      Fix compiler warnings 
02/22/10   aro     Moved the Function Prototype to header file
02/09/10   lcl     Added HAL layer API to initialize SBI clock for BBRX  
01/21/10   aro     [1] Added interface to init SBI
                   [2] Moved the Local Protoype to C file
01/20/10   aro     Initial version
==============================================================================*/ 

#include "comdef.h"
#include "rfcom.h"
#include "rfcommon_nv.h"
#include "rfc_bsp.h"
#include "rfc_card.h"
#include "rxlm_intf.h"
#include "rfcommon_msm_cal.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_core_xpt.h"
#include "rflm_dtr_tx_typedef_ag.h"
#include "rflm_dtr_rx_typedef_ag.h"
#include "rflm_api_cmn.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief
  Macro to convert A to 1Q18 format 
*/
#define FIX_POINT(A, FRAC_BITS) ((int32) ((A<0)?((A)*(1<<FRAC_BITS)-0.5):((A)*(1<<FRAC_BITS)+0.5)))

/*!
  @brief
  Frequency of clock. 
*/
#define CLOCK_FREQ 19.2e6

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum value in UNIV_STMR_MSTMR. (2^22)
*/
#define MSTMR_TIMETICKS_RANGE 4194304

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Time, in microseconds, it takes timer/register to wraparound.
*/
#define WRAPAROUND_TIME (MSTMR_TIMETICKS_RANGE / (CLOCK_FREQ * 1e-6))


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register mask to write 11:15 bits to adc_config2_reg, only applies to Jolokia
*/
#define BBRX_ADC_CAL_CONFIG2_REG_MASK 0xFFFF07FF


/*----------------------------------------------------------------------------*/
typedef union
{
  uint32 bw_kHz;                              /* For CDMA */
  rfcom_multi_carrier_hspa_id_type num_carr;  /* For WCDMA */
  rfcom_lte_bw_type bw;                        /* For LTE */
} rfcommon_msm_num_carr_bw;

typedef enum
{
  BBRX_MODE_0,
  BBRX_MODE_1,
  BBRX_MODE_2,
  BBRX_MODE_3,
  BBRX_MODE_MAX,
  BBRX_MODE_NUM = BBRX_MODE_MAX
} rfcommon_msm_bbrx_mode_type;

typedef struct
{
    boolean is_set;
    uint32 qfuse_code;
    uint32 overflow_bit;
    uint32 avg_error;
} caliberate_dac_iref_struct;

/*----------------------------------------------------------------------------*/
uint32
rfcommon_msm_get_sbi_sel_data
(
  uint32 sbi_bus
);

/*----------------------------------------------------------------------------*/
uint32 rfcommon_msm_get_rxf_agc_sbi_sel_data
(
  uint32 sbi_bus
);

/*----------------------------------------------------------------------------*/
void
rfcommon_msm_init
(
  void
);

/*----------------------------------------------------------------------------*/
void
rfcommon_msm_init_sbi_clk
(
   void
);

/*----------------------------------------------------------------------------*/

void 
rfcommon_msm_config_mgpi_block
(
   void
);

/*----------------------------------------------------------------------------*/
void rfcommon_msm_set_rf_control_sig
(
 rf_control_signal_type rf_signal, 
 rf_sig_out_val_type    value
);

/*----------------------------------------------------------------------------*/
int32
rfcommon_msm_get_bbrx_iq_mismatch_gain
(
  lm_handle_type rxlm_handle
);

/*----------------------------------------------------------------------------*/
#ifdef FEATURE_GNSS_ONLY_NO_WWAN
#define rfcommon_msm_read_modem_id() 0
#else
uint32 rfcommon_msm_read_modem_id
(
  void
);
#endif

/*----------------------------------------------------------------------------*/
int32 rfcommon_msm_calc_sine
(
  int32 x
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_msm_get_spurs_to_suppress
(
  uint32 freq_hz,
  uint32 bw_hz,
  const rfcommon_spur_table_type* master_spur_table,
  uint8 master_spur_table_size,
  rfcommon_spur_table_type* spurs_to_supress_table,
  uint8 max_num_supressible_spurs
);

/*----------------------------------------------------------------------------*/
int32
rfcommon_msm_convert_microsecs_to_ustmr_units
(
  int32 time_us
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_msm_compute_rsb_coefficients
(
  uint32* access_ptr, 
  uint16 sample_size,
  int16* rsb_coeff_a,
  int16* rsb_coeff_b
); 

/*----------------------------------------------------------------------------*/
xpt_cfg_type
rfcommon_msm_get_supported_xpt_mode
(
  void
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Change TXC_DOWN_FACTOR setting dynamically
 
  @details
  This function is used to update Tx down_factor settings dynamically.
  it reads current txlm_dwn_factor setting and updates in  prev_txlm_dwn_factor variable.
 
  @return
  Status of operation
*/
boolean rfcommon_msm_override_txc_down_factor
(
  lm_handle_type txlm_handle,
  uint32  txlm_dwn_factor,
  uint32 * prev_txlm_dwn_factor
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Change TXC_REF_MODE setting dynamically
 
  @details
  This function is used to update Tx Ref_mode settings dynamically.
  it reads current ref_mode setting and updates in  prev_txlm_refmode variable.
 
  @return
  Status of operation
*/
boolean rfcommon_msm_override_txc_refmode
(
  lm_handle_type txlm_handle,
  uint32  txlm_refmode,
  uint32 * prev_txlm_refmode
);

/*----------------------------------------------------------------------------*/
uint8
rfcommon_msm_get_iref_lut_index
(
  uint16 iref_uamp_x10
);


boolean
rfcommon_msm_ccs_transaction_set_isr
(
   void
);

boolean
rfcommon_msm_ccs_transaction_clear_isr
(
   void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_msm_caliberate_dac_iref
(
  lm_handle_type handle
);

/*----------------------------------------------------------------------------*/
lm_status_type rfcommon_msm_allocate_common_notch
(
  lm_handle_type rxlm_handle,
  rxlm_chain_type chain,
  uint32* cmn_notch_dyn_block_id
);

lm_status_type rfcommon_msm_free_common_notch
(
  lm_handle_type rxlm_handle,
  rxlm_chain_type chain,
  uint32 cmn_notch_dyn_block_id
);

rflm_dtr_rx_enum_trx rfcommon_msm_get_rflm_dtr_rx_device_id
(
   rfdevice_id_enum_type device_id
);

#define BBRX_RSB_FUSE_SIZE 3
#define BBRX_RSB_FUSE_COMBINATIONS (1<<BBRX_RSB_FUSE_SIZE)

/*----------------------------------------------------------------------------*/

typedef struct
{
  struct bits_location_struct
  {
    uint32 reg_addr;
    uint32 reg_mask;
    uint8 reg_offset;
  } bits_location[BBRX_RSB_FUSE_SIZE];

  uint8 fuse_value;
} bbrx_rsb_fuse_reg_data;

typedef uint32 bbrx_fuse_to_gain_delta_value_type[BBRX_MODE_MAX][BBRX_RSB_FUSE_COMBINATIONS];

uint32* rfcommon_msm_get_bbrx_fuse_to_gain_value_table(void);

bbrx_fuse_to_gain_delta_value_type *rfcommon_msm_get_bbrx_fuse_to_gain_delta_value_table(void);

bbrx_fuse_to_gain_delta_value_type *rfcommon_msm_get_bbrx_fuse_to_gain_delta_value_table_fbrx(void);

/*----------------------------------------------------------------------------*/
boolean rfcommon_msm_read_and_parse_qfuse_iqmc_register
(
    rxlm_chain_type rxlm_chain, 
    uint8 *fuse_phase,
    uint8 *fuse_gain,
    rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/

uint32 rfcommon_msm_get_fuse_gain_delta
(
    rxlm_chain_type rxlm_chain, 
    rfcommon_msm_bbrx_mode_type mode
);

uint32 rfcommon_msm_get_fuse_gain(rxlm_chain_type rxlm_chain);

/*----------------------------------------------------------------------------*/
boolean rfcommon_msm_read_and_parse_qfuse_iqmc_register_wcdma
(rxlm_chain_type rxlm_chain, 
 uint8 *fuse_phase,
 uint8 *fuse_gain,
 rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
boolean rfcommon_msm_apply_phase_correction
(
   rxlm_chain_type rxlm_chain, 
   rfcom_multi_carrier_hspa_id_type num_chan,
   int32 *final_gain_delta,
   uint8 fuse_phase,
   rfm_mode_enum_type mode,
   rfcom_lte_bw_type bw
);

/*----------------------------------------------------------------------------*/
lm_status_type rfcommon_msm_update_rxlm_by_qfuse
(
   lm_handle_type rxlm_handle,
   rxlm_chain_type rxlm_chain,
   rfcommon_msm_bbrx_mode_type mode
);

/*----------------------------------------------------------------------------*/

rfcommon_msm_bbrx_mode_type rfcommon_msm_get_bbrx_mode
(
    rxlm_chain_type rxlm_chain,
    lm_tech_type tech,
    rfcommon_msm_num_carr_bw num_carr_bw
);

/*----------------------------------------------------------------------------*/

void rfcommon_msm_convert_bbrx_phase_reg_data_to_fuse_value(void);

/*----------------------------------------------------------------------------*/

void rfcommon_msm_convert_bbrx_gain_reg_data_to_fuse_value(void);

/*----------------------------------------------------------------------------*/

lm_status_type rfcommon_msm_update_iqmc
(
  lm_handle_type rxlm_handle,
  rxlm_chain_type rxlm_chain,
  lm_tech_type tech,
  rfcommon_msm_num_carr_bw num_carr_bw,
  int32 rsb_a,
  int32 rsb_b
);

/*---------------------------------------------------------------------------*/
boolean rfcommon_rx_msm_update_rsb_coeff
( 
  rflm_handle_rx_t handle, 
  int32 rsb_coeff_a,
  int32 rsb_coeff_b,
  rflm_tech_id_t tech,
  rfcommon_msm_num_carr_bw num_carr_bw,
  boolean debug_level
 );

/*---------------------------------------------------------------------------*/
rfcommon_msm_bbrx_mode_type rfcommon_msm_get_bbrx_mode( rxlm_chain_type rxlm_chain, 
                                                        lm_tech_type tech, 
                                                        rfcommon_msm_num_carr_bw num_carr_bw);

/*---------------------------------------------------------------------------*/
lm_tech_type rfcommon_msm_convert_rflm_tech_to_lm_tech( rflm_tech_id_t rflm_tech );

/*---------------------------------------------------------------------------*/
uint32 rfcommon_msm_get_iqmc_coeffs
(
  lm_handle_type rxlm_handle,
  rxlm_chain_type rxlm_chain,
  lm_tech_type tech,
  rfcommon_msm_num_carr_bw num_carr_bw,
  int32 rsb_a,
  int32 rsb_b
);

/*---------------------------------------------------------------------------*/
lm_status_type rfcommon_msm_write_iqmc_to_hw( lm_handle_type rxlm_handle, uint32 iqmc_coeff );

/*----------------------------------------------------------------------------*/

boolean rfcommon_msm_adc_cal(void);

boolean rfcommon_msm_get_adc_cal_reg_setting(int32 adc_dc_cal_val,
                                             uint32* adc_config2_reg
                                             );

boolean rfcommon_msm_read_adc_vcm_cal_efs_data(void);

boolean rfcommon_msm_apply_adc_dc_cal(lm_handle_type rflm_rx_handle);

/*!

@details
This functions calls the DAC power up Sequence once we know that DAC has 
reset after XO shutdown.

*/

void rfcommon_msm_dac_mission_mode_reenter(void);

#ifdef __cplusplus
}
#endif

#endif /*RFCOMMON_MSM_H*/
