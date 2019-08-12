
#ifndef RFLTE_MDSP
#define RFLTE_MDSP

/*!
  @file rflte_mdsp.c 

  @brief
  This file contains the MDSP HAL for the LTE technology.



*/

/*===========================================================================

  Copyright (c) 2009 - 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rflte_mdsp.h#1 $

when       who     what, where, why 
-------------------------------------------------------------------------------- 
10/20/14   ndb     Remove the Deprecated  API's 
06/20/14    pv     Added support for TDD PortA/B swap for ASM.
04/17/14   svi     PA static NV and code redesign
02/05/14   cri     Freq comp optimization for DM population
08/14/13   jf      Added: FB rx support.
07/22/13   php     Add support for Intraband CA LNA thresholds  
07/19/13    pv     Added support for separate TDD Tx and Rx ASM scripts.
07/18/13   jf      Add API to get override TXAGC for FTM RF mode 
07/12/13   php     Add HDET active flag support to avoid RF-FW race condition  
08/29/13   svi     Changes to support UL CA
08/14/13   svi     Added support for Single Tx CA
07/08/13   php     Add NV support for Intraband RxAGC LNA threshold 
07/07/13   sbo     Added BW support for PA device
06/18/13   bar     Add FTM RF support to update MTPL during HDET 
06/06/13    pv     Added support for multple TDD ASM scripts.
06/20/13   jf      Adjust RB threshold based on BW for TXAGC and Env_scale scaling 
06/03/13   cri     Use default IQ gain from TxLM for APT PA states
05/21/13   sbm     K sensor changes 
04/18/13   vss     Added support for Dynamic SCELL switch
04/24/13   php     Add SCell Rx ON events support for CA
04/10/13   pv      Updated the API with unused Param for GNSS blanking. 
04/10/13   kab     Added support for Tx Antenna Swap.
03/22/13   jf      Add 2D lin_vs_temp_vs_freq_comp 
03/21/13   pv      Enable LTE TDD GNSS Blanking feature.
03/14/13   jf      Add XPT path delay support 
03/04/13   php     Remove deprecated API  
03/01/13   bsh     FTM IRAT: Copy rx_gain_ctrl from rxagc_ncell_ca_enb to rxagc: new function
02/20/13   php     Added support for Tx EN and Rx EN events
02/19/13   sbm     Bandedge max power backoff support in LTE(Nikel CR375880) 
01/29/13   sbm     HDET events clean up. 
01/15/13   jf      Populate DPD into SM for XPT
01/17/13   sbm     Suuport for therm reads.
12/15/12   pv      Handle multiple Rx LNA GW settings for MDSP.
12/07/12   cri     Dynamic allocation of linearizer tables 
12/04/12   php     Add support for getting HDET reads from FW IND message & remove HDET flag 
12/03/12   jf      Add ET enable
11/20/12   sbm     Add HDET Events support 
11/15/12   php     Add events timing and TDD ASM events 
11/12/12   cri/jf  APT support 
11/07/12   sbm     New events added
10/15/12   php     Move PA script creation to mc 
10/10/12   pl      remove SSBI bud hard coding
10/08/12   php     Add GRFC support for PA scripts  
10/02/12   vss     Add support for tdd asm_rx, asm_tx, rx_on, tx_on events   
09/14/12   php     Populate shared memory with PA RFFE script 
09/07/12   sbm     rflte_nv_tbl_ptr support for multiple carriersers
09/07/12   sbm     MDSP changes to support device based API
08/27/12   pl/sbm  update to device-based MDSP interface for CA support
08/03/12   vss     Change to add band to tx_cmd_dispatch 
07/30/12   pl      Fix warning
06/17/12   pl      Add missing interface for hdet info
03/02/12   vss     Add support for pdet dc cal 
03/08/12   php     Added support for TXP2 signals 
02/29/12   vss     Enable HDET/Therm V2 interface for MDM9X15 also 
02/22/12   vss     Add HDET/Therm V2 Interface
02/16/12   pl      Add a flag to inidicate RX tune is complete
11/15/11   cri     Support for TX PLL adjust
11/03/11   tnt     Added support for populating ANT_SEL GRFC to FW for TDD 
10/17/11   pl      Support SMPS temp comp
09/14/11   can     Merge MDM9K tips to NikeL.
09/13/11   whc     Remove deprecated mdsp ant_sel function   
08/18/11   pl      Support loading linearizer table from V3 linearizer structure
                   to MDSP interface
08/03/11   cri     Disable HDET when exp HDET vs AGC NV is invalid
07/28/11   pl      Added support to sent expected RxAGC to FW during calibration
06/29/11   cri     Move HDET struct from device to mdsp
06/28/11   can     LTE FTM TXAGC mode support.
05/11/11   whc     LTE SMPS logging
03/01/11   whc     MC Layer Cleanup
02/28/11   can     Support for ACQ and TRK mode for LTE Thermistor Reads.
01/31/11   can     Support for LTE GST Freq and Temp Comp.
01/27/10   whc     Temp Comp Support for GST
12/21/10   tnt     Add rflte_mdsp_get_inband_rxagc_val() to read inband RSSI 
12/13/10   tnt     Add rflte_mdsp_populate_ant_sel() to populate ant_sel into the lte_sm 
11/30/10   whc     Add Functions to get lin_vs_freq and lin_vs_temp 
10/22/10   can     Perform lim vs freq based on the new HDET MSGR message containing Physical RB allocation.
10/22/10   can     Support to calculate MTPL & freq comp for slot1, slot2 and SRS accurately.
10/12/10   can     Support to introduce linear interpolation for LTE temp. comp.
09/28/10   whc     Added functions for txagc override
08/11/10   can     Support for LTE TXAGC logging and code clean-up.
07/14/10   can     Changes to optimize HDET Low Power Mode.
06/21/10   qma     Added OLTA support
05/27/10   can     PDET Low power mode support.
05/25/10   can     Support for LIN vs TEMP.
05/17/10   can     LTE MPR, AMPR and PEMAX support.
05/11/10   can     Added an additional flag to avoid temp and PDET read clashes.
05/03/10   can     HDET support optimization.
04/16/10   can     Support for HDET and FW to take GRFC control.
03/10/10   can     Support to increase the LTE Shared Memory counter so the values are
                   updated on the FW side.
02/18/10   qma     Added RX cal support for delta offset
02/16/10   qma     Added FTM LTE RX AGC
01/29/10   qma     Added LTE RX AGC shared mem FW interface support from NV
12/18/09   can     Passing NV items to FW using SM, initial check-in. 
08/05/09   can     Initial version. 


===========================================================================*/

/*===========================================================================

                  INCLUDE FILES

===========================================================================*/
#include "msg.h"
#include "rffw_lte_intf.h"
#include "rflte_core_txpl.h"
#include "rflte_core_temp_comp.h"
#include "rfdevice_lte_type_defs.h"
#include "rfcommon_core_txlin.h"
#include "rfc_common.h"
#include "rflte_nv.h"
#include "rflte_msg.h"
#include "rflte_core_freq_comp.h"

#define RFLTE_MDSP_HDET_RD_RESULT_NUM_TRANS 4

/* Copy the same value to be accessed from MC/Core*/
#define RFLTE_MDSP_MAX_LNA_TO_TUNE RFFW_MAX_LNA_GW_SSBI_PER_PATH

typedef struct
{
   uint8 sbi_setting[RFFW_LTE_NUM_TX_GAIN_SBI_REGS];
   uint8 srs_sbi_setting[RFFW_LTE_NUM_TX_GAIN_SBI_REGS];
}rflte_mdsp_txagc_sbi_type;

typedef struct
{
    uint16 hdet_rd[RFLTE_MDSP_HDET_RD_RESULT_NUM_TRANS];
    uint8 num_avg;
    uint8 status;
}rflte_hdet_rd_result_data_type;

typedef struct
{
   boolean et_enable_status;
   lm_handle_type et_txlm_buf_handle;
}et_status_s;

/*----------------------------------------------------------------------------*/
typedef enum
{
  LTE_HDET_START_SETTINGS,
  LTE_HDET_LO_PWR_HI_BW_SETTINGS,
  LTE_HDET_LO_PWR_MID_BW_SETTINGS,
  LTE_HDET_LO_PWR_LO_BW_SETTINGS,
  LTE_HDET_HI_PWR_HI_BW_SETTINGS,
  LTE_HDET_HI_PWR_MID_BW_SETTINGS,
  LTE_HDET_HI_PWR_LO_BW_SETTINGS,
  LTE_HDET_CONVERSION_SETTINGS,
  LTE_HDET_PAGE_REGISTER_ADDR,
  LTE_THERM_INIT_SETTINGS,
  LTE_THERM_CLEANUP_SETTINGS,

} rflte_mdsp_hdet_settings_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
    rflte_mdsp_hdet_settings_type hdet_mode;
    uint16 hdet_read;
} rflte_mdsp_ftm_txagc_hdet_settings_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rflte_mdsp_hdet_settings_type hdet_setting_type;
  rffw_lte_hdet_filter_sbi_s *hdet_setting_val;
  uint8 *num_hdet_sbi;
} rflte_mdsp_get_tx_hdet_settings_type;


void  rflte_mdsp_init_static_config(rflte_nv_tbl_type   *rflte_nv_tbl_ptr, uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_pa_time (uint16 rflte_pa_up_time, 
                             uint16 pa_down_time);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_rxagc_band_config (rfm_device_enum_type device, 
                                   rfcom_lte_band_type rf_band,
                                   rflte_nv_tbl_type *lte_nv_tbl_ptr,
                                   uint32 carrier_idx,
                                   boolean intra_band_ca_active);
/* ----------------------------------------------------------------------- */
void rflte_meas_mdsp_rx_gain_sbi(rfm_device_enum_type device, 
                                 rfdevice_lte_get_rx_gain_state_tbl_settings_type *lte_rx_gain_table, uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_rx_gain_sbi(rfm_device_enum_type device, 
                            rfdevice_lte_get_rx_gain_state_tbl_settings_type *lte_rx_gain_table, uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_txagc_sbi_time (uint16 txagc_sbi_time);
/* ----------------------------------------------------------------------- */
rffw_lte_current_pa_state_s rflte_mdsp_get_pa_state (void);
/* ----------------------------------------------------------------------- */
int32 rflte_mdsp_get_oride_txagc(void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_oride_txagc_output_power(boolean pwr_oride_enable, 
                                          int32 dbm_pwr_oride_val);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_oride_txagc_backoff(boolean backoff_oride_enable, 
                                             int32 backoff_oride_val );
/* ----------------------------------------------------------------------- */
void rflte_mdsp_init (void);

/* ----------------------------------------------------------------------- */
void rflte_mdsp_tx_pll_clear (void);

/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_tx_pll_values(rfdevice_lte_tx_pll_data_type tx_pll_values);

/* ----------------------------------------------------------------------- */
uint16 rflte_mdsp_get_rxagc_val(rf_path_enum_type path);

/* ----------------------------------------------------------------------- */
int16 rflte_mdsp_get_inband_rxagc_val(rfm_device_enum_type device,
                                      uint32 carrier_idx);

/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_forced_rx_gain_state(rfm_device_enum_type device, 
                                         uint8 rx_gain_force_state,
                                         int16 expected_agc_val,
                                         uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
uint8 rflte_mdsp_get_rx_gain_state(rf_path_enum_type path);
/* ----------------------------------------------------------------------- */
void rflte_meas_mdsp_set_delta_rx_gain_offset(rfm_device_enum_type device, 
                                              uint8 index, 
                                              int16 delta_rx_gain_offset,
                                              uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_delta_rx_gain_offset(rfm_device_enum_type device, 
                                         uint8 index, 
                                         int16 delta_rx_gain_offset,
                                         uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
int16 rflte_mdsp_get_delta_rx_gain_offset(rf_path_enum_type path, 
                                          uint8 index);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_rx_gain_rise_fall_thrshlds(rfm_device_enum_type device, 
                                               uint8 index, 
                                               int16 rise_thrshlds,
                                               uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_increase_rx_counter (void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_increase_tx_counter (void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_increase_hdet_counter (void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_pass_grfc_ctrl_info (void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_temp_comp(uint8 temp_index,
                                 uint8 temp_rem,
                                 rflte_nv_tbl_type *rflte_nv_tbl_ptr);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_pass_rx_ant_sel(rfm_device_enum_type device,rfcom_lte_band_type band);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_pass_tx_ant_sel(rf_path_enum_type path,rfcom_lte_band_type band);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_rx_gain_force(rf_path_enum_type path);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_clear_rx_gain_force(rf_path_enum_type path);
/* ----------------------------------------------------------------------- */
boolean rflte_mdsp_get_hdet_active_flag_val ( void );
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_hdet_active_flag ( boolean hdet_flag_value );
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_hdet_master_flag ( boolean hdet_master_flag_value );
/* ----------------------------------------------------------------------- */
boolean rflte_mdsp_get_hdet_master_flag ( void );
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_hdet_loop_timer_period(uint32 loop_timer_period);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_hdet_loop_output (int32 loop_output);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_hdet_enable_swpt (int16 enable_swpt);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_pdet_mode_swpt (uint16 pdet_mode_swpt);
/* ----------------------------------------------------------------------- */
uint16 rflte_mdsp_get_baseband_backoff(void);
/* ----------------------------------------------------------------------- */
uint16 rflte_mdsp_get_tx_gain_index(void);
/* ----------------------------------------------------------------------- */
int32 rflte_mdsp_get_lte_pwr_db10(void);
/* ----------------------------------------------------------------------- */
uint8 rflte_mdsp_get_num_rbs(void);
/* ----------------------------------------------------------------------- */
uint8 rflte_mdsp_get_start_rb_index(void);
/* ----------------------------------------------------------------------- */
uint8 rflte_mdsp_get_current_pa_state(void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_hdet_mtpl(uint8 subframe_num, int32 hdet_mtpl);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_hdet_mtpl_srs(uint8 subframe_num, int32 hdet_mtpl_srs);
/* ----------------------------------------------------------------------- */
int32 rflte_mdsp_get_current_lin_vs_freq(void);
/* ----------------------------------------------------------------------- */
int32 rflte_mdsp_get_current_lin_vs_temp(void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_pa_smps_pdm(uint8 subframe_num, 
                                   uint8 slot0_slot1, 
                                   boolean srs_data);

/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_therm_read_mode (rflte_core_temp_comp_mode_type therm_mode);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_txagc_band_config_with_smps(uint32 carrier_idx,
                                            rf_path_enum_type path,
                                            rfcom_lte_band_type band, 
                                            rfcom_lte_bw_type bw,
                                            uint32 tx_freq, 
                                            rflte_nv_tbl_type *rflte_nv_tbl_ptr);
/* ----------------------------------------------------------------------- */
uint16 rflte_mdsp_get_pa_smps_pdm(void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_ftm_txagc_set_sm(boolean enable);
/* ----------------------------------------------------------------------- */
uint16 rflte_mdsp_get_pdet_mode_swpt(void);
/* ----------------------------------------------------------------------- */
uint16 rflte_mdsp_get_hdet_enable_swpt(void);
/* ----------------------------------------------------------------------- */
uint32 rflte_mdsp_get_therm_read_period(void);
/* ----------------------------------------------------------------------- */
uint32 rflte_mdsp_set_pdet_read_period(void);
/* ----------------------------------------------------------------------- */
uint8  rflte_mdsp_get_pdet_filter_lo_bw_rb_swpt(void);
/* ----------------------------------------------------------------------- */
uint8  rflte_mdsp_get_pdet_filter_mid_bw_rb_swpt(void);
/* ----------------------------------------------------------------------- */
uint8  rflte_mdsp_get_pdet_filter_hi_bw_rb_swpt(void);
/* ----------------------------------------------------------------------- */
void  rflte_mdsp_set_rx_config_done(boolean flag);
/* ----------------------------------------------------------------------- */

boolean rflte_mdsp_get_hdet_therm_v2_flag(void);
/* ----------------------------------------------------------------------- */
uint16 rflte_mdsp_get_hdet_readings(rflte_hdet_rd_result_data_type* hdet_rd_event_result);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_txp2_ctrl_info(rfm_device_enum_type device, rfnv_lte_tx_gtr_thresh_type tx_gtr_thresh);
/* ----------------------------------------------------------------------- */
int32 rflte_mdsp_pass_gnss_grfc(rfm_device_enum_type device,
                                rfcom_lte_band_type band,
                                rfdevice_lte_script_data_type* script_data_ptr,
                                rfc_logic_type tx_on_off);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_pass_hdet_ctrl_info (uint32 carrier_idx, rfm_device_enum_type device, rfcom_lte_band_type band);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_therm_read_period (int32 therm_read_period_ms);
/* ----------------------------------------------------------------------- */
uint32 rflte_mdsp_get_hdet_loop_period(void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_hdet_therm_v3_settings_sm
(  
   rfm_device_enum_type device,
   rfcom_lte_band_type band,
   boolean update_read_scripts
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_therm_scripts_sm
( 
 rfm_device_enum_type device,
 rfcom_lte_band_type band, 
 boolean update_read_scripts
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_hdet_scripts_sm
( 
 rfm_device_enum_type device,
 rfcom_lte_band_type band, 
 boolean update_read_scripts
);
/*------------------------------------------------------------------------- */
void rflte_mdsp_update_tdet_temp_comp_sm
( 
 rfm_device_enum_type device,
 rfcom_lte_band_type band
);

/*--------------------------------------------------------------------------*/
void rflte_core_tdet_temp_comp_enable(boolean enable);
/* ----------------------------------------------------------------------- */
uint32 rflte_mdsp_get_path_from_device(rfm_device_enum_type device, uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_reset_event_buffer_index( void );
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_rx_on_wr_on_event
(    
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr,
    uint32 carrier_idx
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_rx_on_wr_off_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr,
    uint32 carrier_idx
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_tx_on_wr_on_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr,
    boolean gnss_blanking_enabled
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_tx_on_wr_off_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr,
    boolean gnss_blanking_enabled
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_tx_en_tx_on_wr_on_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_tx_en_tx_on_wr_off_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_rx_en_rx_on_wr_off_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_tdd_asm_rx_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    uint32 carrier_idx,
    rfdevice_lte_script_data_type* script_data_ptr
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_tdd_asm_tx_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    uint32 carrier_idx,
    rfdevice_lte_script_data_type* script_data_ptr
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_pa_script(rfm_device_enum_type device, 
                                 rfcom_lte_band_type band, 
                                 rfcom_lte_bw_type bw,
                                 rflte_nv_tbl_type *rflte_nv_tbl_ptr,
                                 rfdevice_lte_script_data_type* script_data_ptr);
/*----------------------------------------------------------------------------*/

void rflte_mdsp_update_papm_on_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfcom_lte_bw_type bw,
    rfdevice_lte_script_data_type* script_data_ptr
);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_update_papm_off_event
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr
);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_populate_xpt_mode(uint32 carrier_idx, tx_linearizer_aux_data_type aux_lin_data,
                                  uint8 pa_state_index);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_populate_papm_bias(rfm_device_enum_type device,
                                   rffw_lte_xpt_mode_type xpt_mode,
                                   rfcom_lte_band_type band,
                                   uint8 pa_state,
                                   uint32 max_rgi,
                                   tx_linearizer_table_type* tx_tbl_ptr);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_populate_pa_icq(rfm_device_enum_type device,
                                rfcom_lte_band_type band,
                                rfcom_lte_bw_type bw,
                                uint8 pa_state_index,
                                uint32 max_rgi,
                                tx_linearizer_table_type* tx_tbl_ptr);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_update_tdd_asm_rx_script(rfm_device_enum_type device,
                                         rfcom_lte_band_type band,
                                         uint32 carrier_idx,
                                         rfdevice_lte_script_data_type *script_data_ptr,
                                         uint8 script_index);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_update_tdd_asm_tx_script(rfm_device_enum_type rfm_device,
                                         rfcom_lte_band_type band,
                                         uint32 carrier_idx,
                                         rfdevice_lte_script_data_type* script_data_ptr,
                                         uint8 script_index);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_update_tx_on_off_script(rfm_device_enum_type rfm_device,
                                        rfcom_lte_band_type band,
                                        rfdevice_lte_script_data_type* script_data_ptr);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_update_tx_en_off_script(rfm_device_enum_type rfm_device,
                                        rfcom_lte_band_type band,
                                        rfdevice_lte_script_data_type* script_data_ptr);
/*----------------------------------------------------------------------------*/
void rflte_mdsp_update_rf_events_timing(rfm_device_enum_type rfm_device,
                                        rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
errno_enum_type rflte_mdsp_send_msgr_msg
( 
   msgr_umid_type umid, 
   size_t size, 
   void *msg_ptr 
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_et_enable_status
(
   boolean enable, 
   lm_handle_type et_txlm_buf_handle
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_get_et_enable_status(et_status_s* et_txlm_buf);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_populate_dpd(uint8 pa_state_index,
                             rfcommon_xpt_dpd_bank_info_type* amam_bank_info,
                             rfcommon_xpt_dpd_bank_info_type* ampm_bank_info,
                             rfcommon_xpt_dpd_bank_info_type* epdt_bank_info,
                             uint16 num_rgi_with_dpd_info);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_tx_en_wr_on_event(rfm_device_enum_type rfm_device,
                                         rfcom_lte_band_type band,
                                         rfdevice_lte_script_data_type* script_data_ptr);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_tx_en_wr_off_event(rfm_device_enum_type rfm_device,
                                          rfcom_lte_band_type band,
                                          rfdevice_lte_script_data_type *script_data_ptr);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_rx_en_wr_on_event(rfm_device_enum_type rfm_device,
                                         rfcom_lte_band_type band,
                                         rfdevice_lte_script_data_type *script_data_ptr);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_rx_en_wr_off_event(rfm_device_enum_type rfm_device,
                                          rfcom_lte_band_type band,
                                          rfdevice_lte_script_data_type *script_data_ptr);

/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_xpt_path_delay_slot1(uint8 subframe_num, int32 xpt_path_delay_slot1);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_xpt_path_delay_slot2(uint8 subframe_num, int32 xpt_path_delay_slot2);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_update_xpt_path_delay_srs(uint8 subframe_num, int32 xpt_path_delay_srs);
/* ----------------------------------------------------------------------- */
boolean rflte_mdsp_copy_rx_gain(rfm_device_enum_type device, uint32 carrier_idx);
/* ----------------------------------------------------------------------- */
void
rflte_mdsp_update_p_cmax
(
   uint8 subframe_num,
   uint32 carrier_idx,
   rflte_nv_tbl_type   *rflte_nv_tbl_ptr,
   /* Freq comp */
   rflte_core_freq_comp_type *freq_comp,
   rflte_core_freq_comp_type *freq_comp_srs,
   boolean srs_enabled,
   boolean lin_vs_freq_vs_temp_enabled,
   boolean dpd_enabled,
   /* PA SWPTS */
   uint8 mpr_value,
   rflte_core_txpl_ns_type ns_x,
   uint8 backoff_value,
   /* TPL */
   int32 tpl
);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_populate_first_sweep_txlin(rf_path_enum_type path, 
                                           rfcom_lte_band_type band, 
                                           tx_linearizer_table_type * tx_tbl_ptr, 
                                           uint8 pa_state_index);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_populate_env_scale_and_iq_gain_unity_value(rflte_nv_tbl_type *rflte_nv_tbl_ptr,
                                                           uint32 default_iq_gain);
/* ----------------------------------------------------------------------- */
#if 0 /*Needed only for NON-FED based targets*/

void rflte_mdsp_update_ant_swap_script
(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type band,
    rfdevice_lte_script_data_type* script_data_ptr,
    boolean ant_swap_en
);
#endif
/* ----------------------------------------------------------------------- */
boolean rflte_mdsp_asm_port_update(rfa_rf_lte_rx_tx_chain_type rx_tx_chain,
                                   uint8 tx_port_state,
                                   uint8 rx_port_state,
                                   boolean trigger_immediate);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_port_switch_transition_detected(void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_adjust_iq_scaling_based_on_bw(rfcom_lte_bw_type bw);
/* ----------------------------------------------------------------------- */
boolean rflte_mdsp_ftm_txagc_get_sm_flag(void);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_hdet_update_active_flag (boolean hdet_flag_value);
/* ----------------------------------------------------------------------- */
void rflte_mdsp_set_intra_ca_active_flag(boolean flag);

boolean rflte_mdsp_get_hdet_update_active_flag_val (void);
/* ----------------------------------------------------------------------- */
boolean rflte_mdsp_get_fed_txagc_flag(void);
/* ----------------------------------------------------------------------- */

#endif /*RFLTE_MDSP*/
