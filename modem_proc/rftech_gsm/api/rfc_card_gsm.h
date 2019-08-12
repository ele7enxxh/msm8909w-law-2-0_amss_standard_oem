#ifndef RFC_CARD_GSM_H
#define RFC_CARD_GSM_H
/*! @todo dchow:11/04/08: Update comments */
/*!
   @file
   rfc_card_gsm.h

   @brief
   This file contains GSM mode data definitions and prototypes.

   @details


*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfc_card_gsm.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
12/02/14   zbz     Make rfc gsm timing ptr per device
09/24/14   hoh     Add multislot APT control feature
09/09/14   ch      Mainline FEATURE_RF_HAS_QTUNER 
08/22/14   rr      Changes to support nv c3
07/31/14   tsr     GSM Alternate path support
07/31/14   sw      Support both rffe and grfc-based xsw for ASD
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/02/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX
06/02/14   sml     Adding necessary check condition in the case where tuners are not present 
                   and ASD switch is present to avoid un-necessary prints
04/29/14   sc      Manage Vregs based on subscription
04/29/14   tsr     Mainline GSM RFC config v2 apis 
04/24/14   sw      Introduce rffe-based api for ASD processing
03/28/14   sw      Disable tuners during X2G iRAT scenarios (port from DI3)
03/27/14   zbz     Add api to get device_id info
03/25/14   tsr     Add support to query rfm device for TX or RX capability
03/20/14   sw      Introduce api to get signal-specific timing info
03/17/14   kg      Port from Dime
02/11/14   as/sr   Re-design asdiv_tuner to support multiple RF configurations
03/07/14   tsr     Removed unwanted paramters to rfc_gsm_tx_band_config  api
03/06/14   sc      Remove dependency on rfmeas_sbi_type
02/25/14   tsr     Update RX PLL setting time from device driver to rfgsm core rx timing struct
02/24/14   tsr     Mainline Device driver script retrieval
10/01/13   tsr     Added support retrieve individual device scripts
10/01/13   sb      Disable SGLTE Coexistence filtered path cal data
09/17/13   sb      Support for SGLTE/SGTDS coexistence
08/27/13   sb      Add changes to update RX timing for GSM RXD
08/02/13   sml     Fixing Complier warnings
08/01/13   sb      Added interface chanegs to support GSM RX diversity
07/30/13   sml     Correcting the definition as per RF common changes
07/29/13   sml/svi ASD support for GSM tech
06/20/13   sb      Add hooks to populate Spur channel list from RFC
06/17/13   svi     "TX_GTR_THRES" NV values interpretation fix.
04/08/13   tc      Add QPOET/QTUNER timing override facility
03/21/13  yzw      Add device driver support to query RFFE device (PA) information {mfg_id, prd_id, rev_id} 
02/28/13   lcl     Changed function prototype 
02/15/13   sb      Added FTM hook to override tuner tune code 
02/05/13   sb      Created separate APIs to configure RF front end devices based on whether
                   they are called every TX/RX burst or only when the band is changed.
01/3/13    av      Changes to support chain2 for GSM
12/20/12   ggs     Add framework for appending rf_on grfc info from rfc  
12/19/12   sb      Optimize GSM APT
12/11/12   sb      Add hooks for GSM APT 
11/16/12   tsr     Tx Alpha and TX SBI start delta update for DIME based on wtr1605 
09/05/12   kai     Added tuner buffer pointers
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
09/24/12   sb      Made changes to update TXLM,RXLM modem chain
09/04/12   sr      added Tx_gain over-ride API.
08/10/12   sr      Made changes to add the rf_on signal to transition tx script.
08/09/12   ggs     Added APT bus type information
08/07/12   sr      added modulation type parameter for tx_gain onfig api
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/12/12   sn      Added qfe1320 buffer pointers
04/25/12   jps     Updated comments 
04/18/12   jps/lcl PA Range and PA R0/R1 logic decoupling 
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode. 
                   ( Added rfc_gsm_tx_timing_adjust(band) ) 
12/20/11   sn      Added support for QFE1320
10/28/11   vrb     Add Desense channel list for each band, for DC Spur Removal
10/11/11    sr     added rfc tx disable/enable calls to configure 
                   RFC signals for tx disable/enable state.
09/22/11   sb      Making FTM Temp Comp enable RFC dependent
08/06/11   sr      RFC Cleanup: removed un-used code.
08/05/11   sr/plim Old RFC cleanup.
07/08/11   tks     Added support for multiple GSM Tx npler. 
04/28/11   rsr     Support for GSM to use NPA APIs
04/01/11   bmg     Added extern "C" for C++ safety.
03/03/11    sr     merge from mdm9k branch
31/01/11   sr      removed un-used code.
09/01/10   av      GSM RFC Rearch
07/21/10   tnt     Moving IRAT code to common RFC
06/24/10   rsr     added support for scmm/mdm9k merge
01/20/09   tws     Move sigma_delta_max_db value to RFC card file.
12/04/09   sr      changed the name of the rx_timing struct elements and removed the
                   rx_debug_timing struct as we don't need it.
12/02/09   vrb     added power collapse flag to indicate RTR power collapse.
08/21/09   sar     Updated tlmm and gpio interface for CMI.
07/08/09   av      Cleanup and added port configuration (RTR) data  in RFC
03/13/09   sr      tlmm include file change
11/21/08   dyc     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfc_card.h"
#include "DDITlmm.h"
#include "mdsp_intf.h"
#include "rfcom.h"
#include "rfgsm_core_types.h"
#include "rf_buffer_intf.h"
#include "rfc_common.h"




#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
                           Data Definitions
===========================================================================*/

  /* rx nonsbi scripts */
extern uint32 rfc_gsm_rx_script_start_index;

/*  Tx non sbi scripts */ 
extern uint32 rfc_gsm_tx_script_start_index; 

typedef struct 
{
  int rx_alpha_qs;
  int idle_mode_rx_alpha_qs;
  int ant_start_delta_qs;
  int ant_stop_delta_qs;
  int dc_cal_time_qs;
  int modem_delay_time_qs;
  int pll_settling_time_qs;
  int rx_mdsp_overhead;
  int mux_dly_freez_time_qs;
  uint32 rx_burst_processing_time_from_pll_qs;
  uint16 rx_burst_sbis_from_pll;
  uint16 rx_burst_sbis_before_pll;
} rfc_gsm_rx_timing_info_type;

typedef struct
{
    uint16 num_pll_sbis;
    uint16 kv_length;
    int tx_sbi_start_delta_qs;
    int tx_alpha_qs;
} rfc_gsm_tx_timing_info_type;

/* ----------------------------------------------------------------------- */
/*!
   @brief
   GSM PA Range and R0,R1 mapping table

   @details
   Three dimensional array to specify PA R0, R1 mapping based on band and
   PA range. This can be over-written in any specific RFC if the R0, R1
   logic differs from the default (legacy) R0, R1 mapping.

*/
typedef struct
{
   uint8 pa_gain_map[RFCOM_NUM_GSM_BANDS][RFGSM_MAX_TX_GAIN_RANGE][2];
} rfc_gsm_pa_gain_range_map_type; 

/* ----------------------------------------------------------------------- */

typedef struct
{
   rfcom_gsm_band_type supported_band_map[RFM_MAX_DEVICES][RFC_CONFIG_RXTX_MAX];
} rfc_gsm_supported_band_map_type; 

/* ----------------------------------------------------------------------- */
/*!
   @brief
   GSM Mode core configuration data types.

   @details

*/
typedef struct
{
   rfc_gsm_rx_timing_info_type  * rx_timing;
   rfc_gsm_rx_timing_info_type *rx_mon_timing;
   rfc_gsm_tx_timing_info_type *tx_timing;
   mdsp_xo_desense_arfcn_struct *desense_chan_list;
   boolean                        pwr_off_in_sleep;
   uint16 rf_warmup_time;
   uint16                         rf_sigma_delta_max_db;
   uint8                          rfc_gsm_tx_ref_clock_mode;
   boolean                        ftm_temp_comp_enable_flag;
   boolean                    qpa_status_flag;  
   rfc_gsm_pa_gain_range_map_type *rfc_gsm_pa_gain_range_map;
   rfc_apt_config_enum_type       rfc_gsm_apt_config;
   rfc_gsm_supported_band_map_type *rfc_gsm_supported_band_map;
} rfc_gsm_core_config_type;


/*@todo: anupamav: This needs to be in 6x95 specific */
typedef struct
{
  int path;
} rfc_gsm_band_params_type;

typedef struct
{
  rfcom_gsm_band_type band;
  rfc_coex_filter_status_type status;
}rfc_gsm_coex_band_info_type ;

/*===========================================================================
                           Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/* function prototypes */

void rfc_gsm_init_once( rfm_device_enum_type rfm_dev );

void rfc_gsm_mode_config( rfm_device_enum_type rfm_dev, rf_buffer_intf* enter_mode_buffer,uint8 alt_path); 

void rfc_gsm_disable_rffe( rfm_device_enum_type rfm_dev, rf_buffer_intf *buffer, rfcom_gsm_band_type band );

void *rfc_gsm_mode_config_get (rfm_device_enum_type rfm_dev, rfc_mode_config_data_enum_type config_data);

int32 rfc_gsm_command_dispatch( rfm_device_enum_type rfm_dev, int32 cmd, void *data );

boolean rfc_gsm_get_cfg_info(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfgsm_band, rfc_rxtx_enum_type rx_tx, rfc_cfg_info_type *cfg_struct, uint32 alt_path );

void *rfc_gsm_rx_band_config_get(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                                 rfc_band_config_data_enum_type config_data);

void rfc_gsm_tx_band_config( rfm_device_enum_type rfm_dev, 
							 rfcom_gsm_band_type band,
                             rf_buffer_intf *tx_band_script);

void rfc_gsm_rx_burst_config( rfm_device_enum_type rfm_dev,
                              rfcom_gsm_band_type band,
                              uint16 arfcn ,uint8 alt_path);

void rfc_gsm_tx_burst_config( rfm_device_enum_type rfm_dev,
                              rfcom_gsm_band_type band,
                              uint16 arfcn );

void rfc_gsm_tx_gain_config( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                             rfgsm_modulation_type mod_type, uint8 slot_num, int32 pwr_in_dbm, 
                             uint8 pa_range, rf_buffer_intf *tx_trans_buffer, boolean bias_flag, 
                             uint16 bias, uint8 apt_mode, boolean override_start_time);
 
void rfc_gsm_append_rf_off_on_entries( rfm_device_enum_type rfm_dev, rf_buffer_intf *tx_trans_buffer );

boolean rfc_gsm_rf_on_grfc_ctl( rfm_device_enum_type rfm_dev );
void rfc_gsm_tx_gain_override( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                               rfgsm_modulation_type mod_type, uint8 pa_range, 
                               rf_buffer_intf *tx_trans_buffer, boolean bias_flag, 
                               uint16 bias, uint8 apt_mode, boolean override_start_time );

void *rfc_gsm_tx_band_config_get(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                                 rfc_band_config_data_enum_type config_data);

void rfc_gsm_sleep(rfm_device_enum_type rfm_dev);
void rfc_gsm_wakeup(rfm_device_enum_type rfm_dev,uint8 alt_path);

void rfc_gsm_tx_disable(rfm_device_enum_type rfm_dev);
void rfc_gsm_tx_enable(rfm_device_enum_type rfm_dev);

void rfc_gsm_tx_timing_adjust(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band);

void rfc_gsm_manage_vregs ( rf_path_enum_type path, rfcom_gsm_band_type band, rfm_path_state state, uint8 sub_id );

boolean rfc_gsm_get_papm_multislot_ctl_support(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band);

boolean rfc_gsm_data_get_device_info(rfc_cfg_params_type *cfg, rfc_device_info_type **device_info_pptr);

void *rfc_gsm_get_phy_device(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                 rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type);

void *rfc_gsm_get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                 rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type, uint32);

void** rfc_gsm_get_rf_device_list(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                              rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type);


boolean rfc_gsm_get_rffe_device_info(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, rfdevice_type_enum_type rffe_device_type, 
                                                                                                                               uint16 *mfg_id, uint16 *prd_id,uint16 *rev_id);
boolean rfc_gsm_populate_spur_channel_list(void);
#ifdef FEATURE_RF_ASDIV
  boolean rfc_gsm_append_txd_grfc(uint32 command, rf_buffer_intf *start_up_script, void* cb_data);

  boolean rfc_gsm_append_asdiv_rffe(uint32 command, rf_buffer_intf *start_up_script, void* cb_data);

  boolean rfc_gsm_append_asdiv(uint32 command, rf_buffer_intf *start_up_script, void* cb_data);

  boolean rfc_gsm_get_timing_info(  rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfgsm_band, rfdevice_type_enum_type dev_type, rfc_signal_type sig_info, int16 *start_delta );
#endif

uint32 rfc_gsm_get_rf_path(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx, rfcom_gsm_band_type band, uint32 instance);


void *rfc_gsm_get_phy_device_ptr(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                 rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type, uint32 instance);


rfcom_gsm_band_type rfc_gsm_get_supported_band(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx);


boolean rfc_gsm_update_rx_timing_params(uint32 rx_burst_time, 
										uint16 num_sbi_before_pll, 
										uint16 num_sbi_from_pll,
                                        uint16 rx_pll_settling_time_qs,
                                        rfm_device_enum_type rfm_dev);


boolean rfc_gsm_set_device_capability(rfm_device_enum_type rfm_dev,
                                      rf_path_enum_type rf_path );


boolean rfc_get_split_band_type(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,rfcom_gsm_band_type rfgsm_band, uint16 arfcn, rfc_gsm_coex_band_info_type* final_split_band_info);


boolean rfc_band_split_cfg_data_get(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx, rfcom_gsm_band_type rfgsm_band, rfc_band_split_info_type **band_split_cfg_ptr);

rfcom_gsm_band_type rfc_get_unfiltered_band_type(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,rfcom_gsm_band_type rfgsm_band);

rfdevice_id_enum_type rfc_gsm_get_device_id(rfm_device_enum_type rfm_dev,
                                            rfc_rxtx_enum_type rx_tx,
                                            rfcom_gsm_band_type band, 
                                            rfdevice_type_enum_type dev_type, 
                                            uint32 instance);

boolean rfc_gsm_ip2_cal_config( rfm_device_enum_type rfm_dev,
                              rfcom_gsm_band_type band,
                              uint16 arfcn,
                              void* setup_script_ptr,
                              void* cleanup_script_ptr);

#ifdef __cplusplus
}
#endif

#endif /* RFC_CARD_GSM_H */


