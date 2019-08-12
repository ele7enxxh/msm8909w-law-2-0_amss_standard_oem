#ifndef RFGSM_MSM_H
#define RFGSM_MSM_H
/*!
   @file
   rfgsm_msm.h

   @brief
   Provides GSM MSM interface functions.

   @details
    The function interface is defined in this file. This encompases functions
    for all the MSMs. 

*/

/*===========================================================================

Copyright (c) 2008, 2009, 2010, 2011, 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/hal/gsm/inc/rfgsm_msm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/26/15   tws     Remove deprecated function for vbatt read.
10/07/14   sc      Enable programming of all delay match settings in RFLM DRIF
09/12/14   ec      Add support for 2s complement Jammer Detector readings
08/29/14   tws     Add alternate_path support
07/14/14   ec      Add stubbed API to fix compiler errors
14/02/14   hkm     Fix the HAL errors for DPM
05/03/13   sb      Fixes for vbatt read taking too long 
02/22/13   sb      Fix memory leak due to repeated calling of DALsys malloc 
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/14/12   ggs     Edge Envelope gain needs to be doubled for Dime.  
10/18/12   jfc     Remove unused rfgsm_msm_set_grfc_stall_bypass_register() 
09/24/12   sb      Made changes to update TXLM,RXLM modem chain 
02/15/12   dej     Added function to enable/disable grfc_stall_bypass_register
01/10/12   sr      changes to enable vbatt comp.
11/21/11   vrb     Add RxLM Buf Index to rfgsm_mdsp call
10/21/11   dej     Added rfgsm_msm_compute_bb_headroom() prototype.
10/12/11   sb      Enabled RSB correction  
09/08/11   rsr     Port Non-IQ cal feature from older targets
08/06/11   sr      RFC Cleanup: moved MSM related macros to HAL layer from core code.
03/22/11    sr     Added the changes which is missed before for NikeL..
03/21/11    sr     Added LM support.
03/10/11   sr      Moved Tx power related functions to new rfgsm_core_tx_power.c file
03/08/11   sr     ported the msm interface functions from genesis modem. 
31/01/11   sr     moved the vbatt_read() to hal layer
07/27/10   lcl    Compiler warning fix.
01/19/10   tws    Clear up compiler warning.
09/07/09   ggs    Added rfgsm_msm_configure_tx_on_grfc and 
                  rfgsm_msm_configure_gnss_blanking
07//0209   sr     added rfgsm_msm_init() prototype.
07/11/09   sr     removed un-used code
04/02/09   sr     added new function to configure pa range grfcs
10/27/08   av     Initial version.

============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfgsm_core_types.h"
#include "rfcommon_msm.h"
#include "rfm_device_types.h"

#define RFGSM_MSM_PA_SCALE_UNITY_GAIN       32768
#define RFGSM_MSM_EDGE_ENVELOPE_UNITY_GAIN   2*512
#define RFGSM_MSM_PA_RAMP_UNITY_GAIN        8192
/* Bit width from nv table needs to be reduced from 16 to 14 bits. Equivalent to scaling by 1/4. Sign bit must be preserved */
#define RFGSM_MSM_ENVDC_REDUCED_BIT_SHIFT    2
#define RFGSM_MSM_ENV_OFFSETBIN_BUG_WORKAROUND 0x1FFF

/* Assumes a 16 bit JDET reading passed from GL1 in 15 bit 2's complement */
#define RFGSM_MSM_JDET_MSB_DISCARD_BITS 1 //Number of bits to discard from the MSB of the jdet reading

typedef enum {
  RFGSM_MSM_EDGE_PHASE_SHIFT_0,
  RFGSM_MSM_EDGE_PHASE_SHIFT_1,
  RFGSM_MSM_EDGE_PHASE_SHIFT_2,
  RFGSM_MSM_EDGE_PHASE_SHIFT_3
} rfgsm_msm_edge_phase_shift_sel_type;

void rfgsm_msm_init( uint32 rxlm_buf_idx , rfm_device_enum_type rfm_dev, boolean ip2_cal_mode, uint32 alternate_path );

void rfgsm_msm_configure_pa_range_grfc(void);

void rfgsm_msm_enable_pa(void);

void rfgsm_msm_set_edge_phase_shift_sel( rfgsm_msm_edge_phase_shift_sel_type phase_shift );

void rfgsm_msm_configure_tx_on_grfc( void );

void rfgsm_msm_configure_gnss_blanking( void );

void rfgsm_msm_set_tx_band( rfcom_gsm_band_type band );

uint32 rfgsm_msm_micro_gsm_time_read( void );

void rfgsm_msm_disarm_pa_state_machine( uint8 tx_chain );

void rfgsm_msm_pa_ramp_data_write( uint8 tx_chain, uint8 pa_ramp_index, uint32 pa_ramp0, uint32 pa_ramp1 );

void rfgsm_msm_program_pa_transition_ctl( uint8 tx_chain );

void rfgsm_msm_setup_pa_ramp( uint8 tx_chain, uint32 gsm_time );

void rfgsm_msm_write_txc_dc_cal_value( uint8 tx_chain, uint32 tx_dc_cal_val );

void rfgsm_msm_enable_predistortion( uint8 tx_chain, uint32 gsm_time, boolean enable );

void rfgsm_msm_enable_dc_cal_mode( uint8 tx_chain, uint32 gsm_time, boolean dc_cal_mode );

void rfgsm_msm_modulator_enable_continuous_mode( boolean continuous );

void rfgsm_msm_write_to_amam_lut_bank_0_and_1( uint8 tx_chain, uint16 index_i, uint32 index_j );

void rfgsm_msm_enable_txc_cgc_ctl( uint8 tx_chain, boolean enable );

uint32 rfgsm_msm_get_env_backedoff_value(rfgsm_modulation_type mod_type, int16 pwr_delta, uint16 env_val_to_backoff);

void rfgsm_msm_enable_tx(uint32 txlm_buf_idx, rfm_device_enum_type rfm_dev, boolean ip2_cal_mode, uint32 alternate_path);

void rfgsm_msm_disable_tx(void);

int32 rfgsm_msm_get_coefb_bbrx_iqmc_compensated(int32 CoefB, uint32 rxlm_buf_idx);

void rfgsm_msm_compute_bb_headroom(rfm_device_enum_type device, rfcom_gsm_band_type rfgsm_band, int32 *bb_headroom);

boolean rfgsm_msm_update_modem_chain_rxlm(uint32 rxlm_buf_idx, rfcom_device_enum_type device, uint32 alternate_path);

rfc_device_info_type* rfgsm_msm_get_device_info(rfcom_device_enum_type device, rfc_rxtx_enum_type rx_tx, uint32 alternate_path);

uint32 rfgsm_msm_get_adc_read(rfm_device_enum_type, boolean);

boolean rfgsm_msm_adc_client_init(rfm_device_enum_type rfm_dev);

boolean rfgsm_msm_adc_handle_destroy(void);

boolean rfgsm_msm_configure_path_delay( uint32 txlm_buf_index, 
                                        int64 delay, 
                                        uint32 *dm1_coarse_tau, 
                                        uint32 *dm2_fine_tau, 
                                        uint32 *dm3_frac_tau );

boolean rfgsm_msm_sawless_lin_update ( rfm_device_enum_type rfm_dev,
                                       rfcom_gsm_band_type  band,
                                       uint16               arfcn,
                                       dBx16_T              rx_lvl_dbm,
                                       uint16               snr,
                                       int16                signed_jdet_value);

#endif
