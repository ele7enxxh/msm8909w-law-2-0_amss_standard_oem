/*!
  @file rflte_ftm_mc.h

  @brief
  This file contains all declarations and definitions necessary to use
  the LTE FTM Main Control.

  @details

*/

/*===========================================================================

  Copyright (c) 2009 - 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rflte_ftm_mc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/08/14   yzw     Add FTM HORXD support
11/14/14   yb      Updated self test API to use coupler driver to update settings
11/13/14   ars     Added FTM API to enable/disable DRx chain 
11/04/14   kma     Added new API to override TX freq 
11/03/14   stw     Added API to return PA switchpoints
10/27/14   ars     Code changes to support X+G DR-DSDS FTM call  
09/11/14   bsh     Support FTM EXIT command
08/06/14   svi     Fixed not to query for Tx Capability on logical device on hard coded B1
07/31/14   ndb     Added support for extended EARFCN for LTE
04/11/14   bsh     Split Band Support
01/28/14   npi     Support for configuring/apply/reset LTE dedicated notches
01/10/14   npi     Support to disable an SCELL
01/08/14   cri     Add FBRx dedicated buffer for DPD sample capture on FBRx path
12/02/13   cri     Support for RFLM Tx override for LTE
10/11/13   bsh     LTE Sleep and wakeup using dispatch commands
09/10/13   nrk     Add FTM single Rx DL CA support
08/12/13   bsh     FTM: LTE Fixed Vector Tx
08/14/13   jf      Added: FB rx update.
08/12/13   pv      Using Tx and Rx Port State in FTM mode.
08/09/13   yzw     Added API to capture LTE IQ samples  
07/29/13   as      Added support for relative backoff on SAR limit based on 
                   TDD-LTE ULDL configuration 
07/19/13   php     Add TxLO Cal support to use info from ag 
07/19/13   ndb     Fxi the featurization issue
07/19/13   ndb     Clean-up the rflte_ftm_mc_program_qfe_scenario () API
09/02/13   nrk     Add FTM single Tx UL CA support
08/29/13   nrk     Add FTM UL CA support
08/21/13   nrk     Add FTM UL CA support
07/12/13   aca     Tuner obj accessor function update
06/24/13   php     TxLO Self Cal changes to configure FB Rx path 
06/27/13   ndb     Added rflte_mc_program_qfe_scenario()
06/10/13   aca     Tuner accessor function
06/04/13   cri     Update TxOverride by NV to use freq bins 
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/14/13   yzw     Fixed the issue when tune to LTE B29 
04/23/13   kab     Enable FTM mode for NBEE in IRAT 
04/17/13   cri     Turn on cal deconfig
04/03/13   bsh     Casting the band to rfcom_lte_band_type  
03/25/13   cri     XPT cal config support for LTE 
03/27/13   vss     Add TXLO cal support
03/13/13   aca     Tuner characterization
03/06/13   php     Add support for using NBEE instead of FW RxAGC estimate 
12/26/12   aca     CW errors fixed
12/14/12   pl      Unified all defines to rflte_mc.h
12/12/12   php     Add NBEE support for Calibration with flag to enable/disable 
11/14/12   cri     LTE radio setup opcode support
11/12/12   php     Add APIs to get and set LNA Range for NBEE support 
11/12/12   cri/jf  APT support 
10/22/12   pl      Update mdsp interface to become re-enterant
09/26/12   php     Add support for GRFC PAs 
09/19/12   sbm     Added rflte_ftm_mc_set_tx_on()
08/27/12   pl/sbm  Update to new device-based interfaces
07/06/12   pl      Update State machine to support CA
06/18/12   aca     LTE DAC Cal update- LTE specific Iref programming
03/24/12   yzw     Added FTM command FTM_LTE_ADJUST_TX_FREQ, 
03/10/12   ks      Hookup Cal v3 ET Enable/Disable actions to device
02/22/12   aca     Cleaned up LTE duplicate code for checking if a band is TDD
02/22/12   php     Move rflte_ftm_mc_in_tdd_band declaration to rflte_ftm_mc.h 
09/07/11   whc     New API to decouple PA State from PA Range 
08/26/11   whc     Cal V3 LTE optimize HDET reading  
07/22/11   pl      cleanup redundant FTM LTE interfaces
07/20/11   aro     Renamed Cal v3 Opcode/payload type names for consistency
07/08/11   tnt     Initial V3 cal support 
06/23/11   pl      Remove obsolete interface : set_rx_on
05/24/11   pl      Fix typo 
05/17/11   pl      Adding flags for RxAGC, TxAGC and clock starts 
04/27/11   aro     Compiler Error Fix
04/20/11   pl      Added LTE FTM state structure 
04/20/11   pl      Added LTE FTM State variable access functions
04/20/11   pl      Move FW interfaces to MDSP layer
04/20/11   pl      Move action performing functionalities in LTE FTM MC from
                   dispatcher layer
03/18/11   pl      Adding support for LTE clock configuration in FTM mode
03/14/11   pl      remove unused interfaces
02/04/11   vss     Change to the rflte_ftm_fw_rxagc_sys_config_app api to accomodate multiple bw
01/13/10   mkv     Clean up LTE FTM FW and RF upon exiting FTM mode  
05/14/10   tnt     adding rflte_ftm_mc_tdd_tx_config
05/03/10n  can     Code optimization.
04/01/10   pl      Initial Check-in/ split from rflte_mc.h
===========================================================================*/

#ifndef RFLTE_FTM_MC_H
#define RFLTE_FTM_MC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rf_cmd_dispatcher.h"
#include "rfcom.h"
#include "ftm.h"
#include "lm_types.h"
#include "rflte_ftm_calibration.h"
#include "rfdevice_lte_type_defs.h"
#include "rflte_mc.h"
#include "rfcommon_nv.h"
#include "rfdevice_coupler.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define RFLTE_FTM_MC_PRX RFLTE_MC_CARR_TYPE_PRX
#define RFLTE_FTM_MC_DRX RFLTE_MC_CARR_TYPE_DRX
#define RFLTE_FTM_MC_TX  RFLTE_MC_CARR_TYPE_TX

typedef enum
{
   RF_LTE_FTM_MC_SUCCESS,
   RF_LTE_FTM_MC_ERROR,
   RF_LTE_FTM_MC_STATUS_MAX,
} rflte_ftm_mc_status_type;

typedef enum
{
  RF_LTE_FTM_START,
  RF_LTE_FTM_DATA,
  RF_LTE_FTM_STOP,
  RF_LTE_FTM_NUM,
  RF_LTE_FTM_MAX = 0xFF
} rflte_ftm_state_type;

typedef struct
{
  /* RX Information*/
  rfm_device_enum_type prx_device;
  rfm_device_enum_type drx_device;
  rfm_device_enum_type drx_device2; /*HORXD DRx2*/
  rfm_device_enum_type drx_device3; /*HORXD DRx3*/   
  rfcom_lte_bw_type rx_bw;
  lm_handle_type rxlm_prx_buf_idx;
  lm_handle_type rxlm_drx_buf_idx;
  lm_handle_type rxlm_drx2_buf_idx; /*HORXD DRx2*/
  lm_handle_type rxlm_drx3_buf_idx;  /*HORXD DRx3*/ 
  lm_handle_type rxlm_fbrx_buf_idx;
  rfcom_lte_band_type prx_rx_band;
  rfcom_lte_band_type drx_rx_band;
  rfcom_lte_earfcn_type rx_chan;
  rfcom_lte_earfcn_type rx_freq;

  /* RX AGC state */
  uint8 prx_lna_range;
  uint8 drx_lna_range;
  uint8 drx2_lna_range;/*HORXD DRx2*/
  uint8 drx3_lna_range; /*HORXD DRx3*/  

  /* TX information */
  rfm_device_enum_type tx_device;
  rfcom_lte_band_type tx_band;
  lm_handle_type txlm_buf_idx;
  rfcom_lte_earfcn_type tx_chan;
  uint32 tx_freq;
  rfcom_lte_bw_type tx_bw;
  uint8 rb_block;
  uint8 rb_start;

  /* System config information */
  rfa_rf_lte_tdd_ul_dl_cfg_type tdd_ul_dl_cfg;  /* Cfg#0, Cfg#1....Cfg#6*/

  /* TX AGC state */
  uint16 min_tx_gain_index;
  uint16 max_tx_gain_index;
  rfa_rf_lte_ftm_ul_modulation_type ul_mode_type;
  byte   pa_range;
  uint8  pa_state;
  uint16 tx_gain_index;

  /* Operation states */
  boolean lte_fw_start_txagc;
  boolean lte_fw_start_rxagc;
  boolean single_tx_ul_ca_enable;
  boolean single_rx_dl_ca_enable;

  /* Notch Configuration */
  boolean dedicated_prx_notch_overide_enable;
  boolean dedicated_drx_notch_overide_enable;
  rfcommon_spur_table_type prx_spur_tbl;
  rfcommon_spur_table_type drx_spur_tbl;

  /* HORxD FTM flag */
  boolean is_horxd_enabled;  
}rflte_ftm_carrier_info_type;

typedef struct
{
  /* Per carrier information */
  rflte_ftm_carrier_info_type carrier[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /* Overall System information -- carrier indenpendent */
  boolean initialized;
  boolean lte_fw_start_system_time;
  boolean lte_start_clk;
  boolean use_fb_path;
  boolean use_fixed_encoder_input_ul; 
  uint32  current_carrier_idx;
  boolean disable_power_update;
} rflte_ftm_mc_sm_type;

typedef struct
{
	rfm_device_enum_type prx_device;
	rfm_device_enum_type drx_device;
	rfm_device_enum_type drx_device2; /*HORXD DRx2*/
	rfm_device_enum_type drx_device3; /*HORXD DRx3*/
}rx_device_struct;

extern boolean nbee_enable_flag;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*----------------------------------------------------------------------------*/
void rf_lte_ftm_dispatch_init(void);

/* -------------------------------------------------------------------------- 
    STATE MACHINE ACCESSOR FUNCTIONS 
 * -------------------------------------------------------------------------- */
/* SETs */
rflte_ftm_mc_sm_type * rflte_ftm_mc_get_sm (void);
void rflte_ftm_mc_sm_reset (void);
void rflte_ftm_mc_sm_set_carrier_idx(uint32 carrier_idx);
void rflte_ftm_mc_sm_set_prx_device(rfm_device_enum_type device);
void rflte_ftm_mc_sm_set_drx_device(rfm_device_enum_type device);
void rflte_ftm_mc_sm_set_tx_device(rfm_device_enum_type device);
boolean rflte_ftm_mc_set_rx_tx_device(uint32 carrier_idx, rfm_device_enum_type tx_device, rfm_device_enum_type prx_device, rfm_device_enum_type drx_device);
void rflte_ftm_mc_sm_set_band(ftm_mode_id_type mode);
void rflte_ftm_mc_sm_set_band_by_num(rfcom_lte_band_type band);
void rflte_ftm_mc_sm_set_rx_chan(rfcom_lte_earfcn_type channel);
void rflte_ftm_mc_sm_set_rx_bw(rfcom_lte_bw_type bw);
void rflte_ftm_mc_sm_set_tx_chan(rfcom_lte_earfcn_type channel);
void rflte_ftm_mc_sm_set_tx_bw(rfcom_lte_bw_type bw);
void rflte_ftm_mc_sm_set_fw_time_start(boolean flag);
void rflte_ftm_mc_sm_set_fw_rxagc_start(boolean flag);
void rflte_ftm_mc_sm_set_fw_txagc_start(boolean flag);
void rflte_ftm_mc_sm_set_clk_start(boolean flag);
void rflte_ftm_mc_sm_set_min_max_tx_gain_index(uint16 min_index, uint16 max_index);
void rflte_ftm_mc_sm_set_tx_gain_index(uint16 tx_gain_index);
void rflte_ftm_mc_sm_set_ul_mod_type(rfa_rf_lte_ftm_ul_modulation_type ul_mod_type );
void rflte_ftm_mc_sm_set_pa_state(uint8 pa_state);
void rflte_ftm_mc_sm_set_pa_range(uint8 pa_range);
void rflte_ftm_mc_set_pa_icq(uint8 pa_current);
void rflte_ftm_mc_set_papm_bias(uint16 pa_bias);
void rflte_ftm_mc_sm_set_lna_range(uint8 lna_range, uint32 prx_drx_flag);
void rflte_ftm_mc_sm_reset_carrier(uint32 carrier_idx);
void rflte_ftm_mc_set_ftm_mode(boolean ftm_mode);
void rflte_ftm_mc_sm_set_use_fb_path(boolean txlo_prog);
void rflte_ftm_mc_set_dedicated_prx_notch_override_enable(boolean enable);
void rflte_ftm_mc_set_dedicated_drx_notch_override_enable(boolean enable);

/* GETs */
uint32 rflte_ftm_mc_sm_get_carrier_index(void);
rfm_device_enum_type rflte_ftm_mc_sm_get_prx_device(void);
rfm_device_enum_type rflte_ftm_mc_sm_get_drx_device(void);
rfm_device_enum_type rflte_ftm_mc_sm_get_tx_device(void);
rfcom_lte_band_type rflte_ftm_mc_sm_get_band(void);
rfcom_lte_band_type rflte_ftm_mc_sm_get_drx_band(void);
rfcom_lte_earfcn_type rflte_ftm_mc_sm_get_rx_chan(void);
rfcom_lte_earfcn_type rflte_ftm_mc_sm_get_tx_chan(void);
rfcom_lte_bw_type rflte_ftm_mc_sm_get_rx_bw(void);
rfcom_lte_bw_type rflte_ftm_mc_sm_get_tx_bw(void);
rfcom_lte_bw_type rflte_ftm_mc_sm_get_tx_bw(void);
uint8 rflte_ftm_mc_sm_get_pa_state(void);
uint8 rflte_ftm_mc_sm_get_pa_range(void);
uint16 rflte_ftm_mc_sm_get_tx_gain_index(void);
lm_handle_type rflte_ftm_mc_sm_get_rxlm_buffer(uint32 prx_drx_flag);
lm_handle_type rflte_ftm_mc_sm_get_txlm_buffer(uint32 prx_drx_flag);
rfa_rf_lte_ftm_ul_modulation_type rflte_ftm_mc_sm_get_ul_mod_type(void);
uint16 rflte_ftm_mc_sm_get_min_tx_gain_index(void);
uint16 rflte_ftm_mc_sm_get_max_tx_gain_index(void);
uint8 rflte_ftm_mc_sm_get_lna_range(uint32 prx_drx_flag);
boolean rflte_ftm_mc_get_ftm_mode(void);
boolean rflte_ftm_mc_sm_get_use_fb_path(void);
uint8 rflte_ftm_mc_sm_get_num_rbs(void);
uint8 rflte_ftm_mc_sm_get_start_rb(void);
boolean rflte_ftm_mc_sm_use_fixed_encoder_input_ul( void );
boolean rflte_ftm_mc_is_dedicated_rx_notch_override_enabled(rfcom_device_enum_type device);
boolean rflte_mc_ftm_config_reconfig_rx_chain (uint32 carrier_idx, uint16 chain_mask, boolean enable);

void rflte_ftm_mc_sm_set_fixed_encoder_input_ul( boolean set_flag );

/* In Split band concept Tx and Rx can have different bands */
rfcom_lte_band_type rflte_ftm_mc_sm_get_tx_band(void);

/*Port state is shared between PRx and DRx */
uint8 rflte_ftm_mc_sm_get_rx_asm_port_state(rflte_mc_carrier_type carrier_type);

uint8 rflte_ftm_mc_sm_get_tx_asm_port_state(void);

/* QUERYs*/
boolean rflte_ftm_mc_sm_is_mdsp_time_started( void );
boolean rflte_ftm_mc_sm_is_mdsp_rxagc_started( void );
boolean rflte_ftm_mc_sm_is_mdsp_txagc_started( void );
boolean rflte_ftm_mc_sm_is_clk_started( void );
/* Tuner Obj accessor Fn */
void* rflte_ftm_mc_get_tuner_object(rfm_device_enum_type device, rfdevice_type_enum_type device_type);


/*----------------------------------------------------------------------------
 * FTM MC OPERATIONS
 *----------------------------------------------------------------------------*/
rflte_ftm_mc_status_type rflte_ftm_mc_enter_mode(rfm_device_enum_type device);
uint16 rflte_ftm_mc_sm_get_current_chain_mask(void);
rflte_ftm_mc_status_type rflte_ftm_mc_get_devices_from_chain_mask(uint32 carrier_idx, uint16 chain_mask, rx_device_struct *rx_device);
rflte_ftm_mc_status_type rflte_ftm_mc_tune_to_chan(uint32 chan, uint16 chain_mask);
void rflte_ftm_mc_set_tx_wf(ftm_cal_instr_payload_config_tx_wf_type const *tx_wf );
void rflte_ftm_mc_set_pa_range(byte pa_range);
void rflte_ftm_mc_set_pa_state(uint8 pa_state);
boolean rflte_ftm_mc_set_tx_gain_index(int16 tx_gain_index);
void rflte_ftm_mc_sm_set_rb_alloction(uint8 rb_block, uint8 rb_start);
void rflte_ftm_mc_enable_hdet(rfdevice_lte_hdet_power_mode_type ftm_hdet_pm, uint8 bw);
uint16 rflte_ftm_mc_read_hdet(rfdevice_lte_hdet_power_mode_type ftm_hdet_pm);
void rflte_ftm_mc_disable_hdet(void);
void rflte_ftm_mc_set_tx(boolean tx_on);
boolean rflte_ftm_mc_program_lte_iref(rfdevice_lte_program_iref_type iref_programmer);
void rflte_ftm_mc_set_rx_iq_mismatch_coefts(rfm_device_enum_type device, uint32 iq_mismatch_coeft);
int16 rflte_ftm_mc_get_rx_gain_offset(rfm_device_enum_type device, uint8 rx_gain_index, int16 expected_agc_val);
void rflte_ftm_mc_set_lte_clock( rflte_ftm_state_type mcpm_req, rfcom_lte_bw_type lte_bw );
void rflte_ftm_mc_set_et_path(uint8 pa_state, boolean enable_et_path);
boolean rflte_ftm_mc_chan_modulation(rfa_rf_lte_ftm_ul_modulation_type requested_modulation);
uint16 rflte_ftm_mc_sm_get_ul_dl_freq_ratio(rfcom_lte_band_type band, rfcom_lte_earfcn_type rx_chan, rfcom_lte_earfcn_type tx_chan);
rflte_ftm_mc_status_type rflte_ftm_mc_set_freq_adjust(int32 rx_freq_adj);
void rflte_ftm_mc_set_tx_on(rfm_device_enum_type device,rfcom_lte_band_type band,boolean tx_on);
void rflte_ftm_mc_set_pa_on_off_grfc(rfm_device_enum_type device, rfcom_lte_band_type band, boolean pa_on);
void rflte_ftm_mc_set_pa_range_grfc(rfm_device_enum_type device, rfcom_lte_band_type band, uint16 pa_range);
int16 rflte_ftm_mc_get_rx_gain_offset_nbee(rfm_device_enum_type device, uint8 rx_gain_index, int16 expected_agc_val);
uint32 rflte_ftm_mc_get_ant_path_from_device(rfm_device_enum_type device);
uint32 rflte_ftm_mc_get_carrier_index_from_device(rfm_device_enum_type device);
boolean rflte_ftm_mc_is_device_support_tx ( rfm_device_enum_type device, boolean check_all_bands_for_tx_capability, rfcom_lte_band_type tx_band_req );
boolean rflte_ftm_iq_capture_get_config(ftm_iq_capture_get_config_type *data);
void rflte_ftm_mc_send_fbrx_update_msrg_rsp(void);
boolean ftm_rflte_get_pa_switchpoints(uint8 carrier_idx ,rfcom_lte_band_type rfcom_band, int16* pa_rise_swpts, int16* pa_fall_swpts, int16 *active_pa_states);
rflte_ftm_mc_status_type rflte_ftm_mc_override_tx_freq(uint32 chan);

boolean rflte_ftm_tx_hw_ctl_config_coupler_device_imm
( 
  rfcom_device_enum_type device, 
  rfdevice_coupling_direction_type coupler_dir
);

void rflte_ftm_mc_program_qfe_scenario(void);

boolean 
rflte_ftm_mc_perform_txlo_cal_search
(
   rfm_device_enum_type rfm_device,
   rfm_device_enum_type rfm_device_fb,
   uint8 search_resolution
);
uint32
rflte_ftm_mc_get_samp_rate
(
   uint8 rx_lm_buf_index
);
boolean
rflte_ftm_mc_is_single_rx_dl_ca_enabled
(
  uint32 carrier_idx
);

rfm_device_enum_type
rflte_ftm_mc_get_single_rx_dl_ca_prx_device
(
  uint32 carrier_idx
);

rfm_device_enum_type
rflte_ftm_mc_get_single_rx_dl_ca_drx_device
(
  uint32 carrier_idx
);

/* For D3925 Bring up Build, stubbing out
features dependent on other layers */
#ifdef RFLTE_BOLT_SINGLE_RX_CA_SUPPORTED
rflte_mc_status_type
rflte_ftm_mc_set_single_rx_dl_ca_bw
(
  rfa_rf_dl_ca_single_rx_config_req_s *dl_ca_single_rx_param
);
#endif

/* For D3925 Bring up Build, stubbing out
features dependent on other layers */
#ifdef RFLTE_BOLT_SINGLE_RX_CA_SUPPORTED
rflte_mc_status_type
rflte_mc_status_type 
rflte_ftm_mc_set_dl_ca_single_rx_param
(
  rfa_rf_dl_ca_single_rx_config_req_s *dl_ca_single_rx_param
);
#endif

boolean
rflte_ftm_mc_is_single_tx_ul_ca_enabled
(
  uint32 carrier_idx
);
rfm_device_enum_type
rflte_ftm_mc_get_single_tx_ul_ca_device
(
  uint32 carrier_idx
);

/* For D3925 Bring up Build, stubbing out
features dependent on other layers */
#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
rflte_mc_status_type
rflte_ftm_mc_set_single_tx_ul_ca_bw
(
  rfa_rf_ul_ca_single_tx_config_req_s *ul_ca_single_tx_param
);
#endif

/* For D3925 Bring up Build, stubbing out
features dependent on other layers */
#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
rflte_mc_status_type 
rflte_ftm_mc_set_ul_ca_single_tx_param
(
  rfa_rf_ul_ca_single_tx_config_req_s *ul_ca_single_tx_param
);
#endif

uint8
rflte_ftm_mc_sm_set_single_tx_ul_ca_enable
(
  boolean single_tx_ul_ca_enable
);
boolean
rflte_ftm_mc_sm_get_single_tx_ul_ca_enable
(
  uint32 carrier_idx
);
uint8
rflte_ftm_mc_sm_set_single_rx_dl_ca_enable
(
  boolean single_rx_dl_ca_enable
);
boolean
rflte_ftm_mc_sm_get_single_rx_dl_ca_enable
(
  uint32 carrier_idx
);

/*----------------------------------------------------------------------------*/
rflte_ftm_mc_status_type rflte_ftm_mc_sleep(void);
/*----------------------------------------------------------------------------*/
rflte_ftm_mc_status_type rflte_ftm_mc_wakeup(void);
rflte_ftm_mc_status_type rflte_ftm_mc_exit(void);
/*----------------------------------------------------------------------------*/
rflte_ftm_mc_status_type
rflte_ftm_disable_scell
(
  uint32 carrier_idx
);
/*----------------------------------------------------------------------------*/
rfcommon_spur_table_type*
rflte_ftm_mc_get_dedicated_rx_notch_params
(
  rfcom_device_enum_type device
);
/*----------------------------------------------------------------------------*/
rflte_ftm_mc_status_type
rflte_ftm_set_msm_notch_filter
(
  ftm_set_msm_notch_filter_data_s* notch_params
);
/*----------------------------------------------------------------------------*/
rflte_ftm_mc_status_type
rflte_ftm_apply_msm_notch_filter_settings
(
  rfcom_device_enum_type device
);
/*----------------------------------------------------------------------------*/
rflte_ftm_mc_status_type
rflte_ftm_reset_msm_notch_filter
(
  rfcom_device_enum_type device
);
#endif /* RFLTE_FTM_MC_H */


