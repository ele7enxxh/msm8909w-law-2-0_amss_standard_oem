/*!
   @file
   rfgsm_core.h

   @brief

   @details

*/
/*=============================================================================
Copyright (c) 2008-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core.h#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/16   bp      Put the QPOET to standby mode after GSM Tx burst ends
12/23/15   bp      Stop further processing of therm_read_rsp if temp. compensation is stopped through GSM Tx Disable.
04/23/15   par     Fix incorrect AFC being applied in GBTA cases
02/26/15   tws     Fix compiler warning
01/27/15   piy     Logging Rx burst RxAGC info and disabling therm reads via debug NV
01/19/15   sw      Use tuner start delta to program XSW
01/13/15   sml     GL1 -> RF handshake mechanism to handles cases when 
                   GSM ASD response is not being received as scripts 
                   weren't executed
12/02/14   zbz     Make rfc gsm timing ptr per device
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/04/14   tsr     Fix Alternate path bug in IRAT due to GSM 1800_B band support not available in RFC 
10/06/14   sc      Deprecate unused functionality
10/01/14   tsr     GSM IDLE Mode RXD support
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
11/19/13   cpm     support for FTM_PROGRAM_QFE_SCENARIO and Antenna Tuner force detune
09/09/14   tsr     DR-DSDS C2/C3 RX calibration support
08/28/14   sc      Clean up Tx data initialisation to reduce wakeup timeline
08/19/14   zbz     Add RF GSM CCS execution time profiling
08/11/14   sc      Ensure chain update with alternate path and include IRAT
08/01/14   ec      Add prototype for function to get SAWless HL switch point table, SAWless status per band
07/31/14   tsr     GSM Alternate path support
07/30/14   cj      Added changes for Tx sharing feature
07/30/14   hoh     Add enhanced rx feature support 
07/28/14   tsr     DR-DSDS: Add support for device driver to handle worst case rx alpha for IDLE mode RX
07/28/14   ec      Add function prototypes to get scripts for Jammer Detector config
07/14/14   ec      Add function prototypes to manipulate HL/LL decision table
07/01/14   tsr     DR-DSDS: Lower RX CCS event priority when GSM sub is in Idle mode
06/18/14   tsr     RXD diversity call flow changes to enable DRX path along with PRX path
05/29/14   sb      IP2 cal bug fixes, updates
05/08/14   sc      Add RFLM GSM init once API interface
04/28/14   sc      Scale the Tx PA ramps based on subscription
04/24/14   sw      Introduce rffe-based api for ASD processing
04/23/04   tsr     Add API to query PRX device associated with DRX device
04/16/14   sb      IP2 cal Interface and functionality changes
03/31/14   tsr     Added support for handling combined Enter Mode for PRX and DRX
03/26/14   tsr     Add support to query rfm device for TX or RX capability
03/26/14   sb      Add support for IP2 cal for Dime PM
03/18/14   sc      Add subscription based execution intelligence
03/11/14   tsr     Added RXD support for RX burst 
02/28/14   tsr     Separate RX burst scripts and TX burst scripts retrieval from device driver
02/28/14   sc      Pass core data through mc to core layers
02/26/14   sc      Pass core data through cmd processing and mc layer
02/25/14   tsr     Changed return type to be void for rfgsm_core_calc_rx_timing
02/19/14   tsr     GSM RX Diversity 
02/17/14   sc      Clean up mDSP APIs
01/21/14   ggs     Initial changes for TXC DCOC and DPD calibration
01/15/14   sc      Remove unused APIs using FW shared memory
12/13/13   ry      Remove hardcoding of RFM_DEVICE_0
10/28/13   ggs     Add function to switch amam/ampm buffer index
10/23/13   tsr     Mainline D3295 changes for BOLT
10/14/13   sc      Mainline core DM and RFLM code
09/27/13   tsr     Added support retrieve to individual device scripts
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
09/17/13   sb      Support for SGLTE/SGTDS coexistence
09/12/13   sc      RFLM interface encompassed by core DM
08/27/13   sb      Add changes to update RX timing for GSM RXD
08/14/13   sc      Revert Rx burst type variable name
08/13/13   sb      GSM RXD changes
08/08/13   sc      Add PA ramp initialisation, storing NV tables in core DM
08/05/13   sc      Add PA ramp data management prototypes (featurised)
08/02/13   sml     Removing un-necessary code
08/02/13   sml     Fixing Complier warnings
07/29/13   sml/svi ASD support for GSM tech
07/26/13   tws     New API functions to retrieve rx/tx band
07/18/13   sc      Update RFLM data information (featurised)
07/12/13   av      GTA EXIT mode
06/20/13   ec      Add support for GSM Tune Away (GTA) enter mode
06/19/13   svi     Fix vbatt comp issues  
05/08/13   tws     Support DUAL_SIM for power & HAL voting
05/06/13   aka     Moved prototypes to this header file
04/30/13   aka     added rfgsm_core_handle_nv_init
04/22/13   sb      Add FTM hooks to override QPOET bias
03/27/13   sb      Added support for Enhanced APT
02/22/13   tws     Make the IQ capture buffer dynamic.
02/19/13   sc      Added APIs to allow GL1 to alloc and free Rx meas buffers
02/15/13   sb      Added FTM hook to override tuner tune code 
02/12/13   sr      changes to free the memory alloocated during rfm_init() 
02/06/13   tsr     Added GSM temp comp algorithm implementation 
01/22/12   sb      Changes to enable QPOET transactions to go out every gsm frame
01/11/13   aka     Added support for simultaneous Tx SAR
01/09/13   sr      Made changes to run vbatt/temp comp in rf app task context.
01/03/12   sc      Added therm read full call flow framework 
12/18/12   sc      Added therm read event generation framework
11/27/12   tws     Add prototype for use by FTM.
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/08/12   sg      Fixed compiler warnings
10/18/12   tsr     Removed CCS time profilling api created during DIME RF GSM Bringup
10/11/12   sc      Cleanup up unrequired Rx burst & SBI/Non-SBI API burst buffer params
10/04/12   sr      Moved IRAT APIs to rfgsm_core_meas file. 
10/04/12   sr      Added IRAT APIs. 
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK.  
09/24/12   sb      Made changes to update TXLM,RXLM modem chain
09/06/12   tsr     Fixed compiler warning
08/21/12   sc      Change GSM_RUMI bringup feature to task-specific feature
08/08/12   sr      added rfm_dev parameter to tx_power api
08/02/12   sc      Add num slots to setup Tx burst API
08/06/12   tsr     Fix the rfa Broken tips with CL2664361
08/02/12   tsr     Changes for GSM Rumi Bring-up
07/26/12   sr      Changes to APIS to take rfm_dev as parameter.
07/24/12   kb      Fixed compiler errors.
07/06/12   sc      Edit API parameters to support CCS buffer format
07/06/12   sc      Remove unrequired num transactions init functionality
06/19/12   sc      Added shared memory rf task support
05/11/12   sb      Added changes for debug
04/09/12   sb      Add changes to fix frequency error adjustment in FTM mode
12/07/11   sb      Support for GSM IQ capture in Online and FTM mode 
11/29/11   sb      Added functionality for GSM IQ CAPTURE
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame
09/15/11    sr     added antenna tuner support code.
08/06/11   sr      RFC Cleanup: added the function prototype.
07/11/11    sr     removed un-used code.
03/23/11    sr     temp comp code cleanup.
03/21/11    sr     Added LM support and timing info access interface.
11/02/10   lcl     Fixed compiler warning
21/09/10   ggs     Added new api prototype to return rf_warmup time
06/16/10   rsr     Removed KV_vs_temp function prototypes
03/18/10   lcl     Merged changed from QSC6295
12/07/09   jps     Added prototypes for temp comp functions
07/27/09   rsr     Cleanup compiler warnings.
07/20/09    sr     removed unused prototype.
07/06/09    sr     changes to make the operating band to rfcom_gsm_band type
03/25/09   sr      corrected the rfgsm_core_enter_mode() return type.
03/13/09   sr      code clean-up for SCMM build
10/14/08   sr      Initial version

============================================================================*/

#ifndef RFGSM_CORE_H
#define RFGSM_CORE_H

#include "ftm_gsm.h"
#include "rfmeas_types.h"
#include "rfgsm_core_handle.h"
#include "rf_apps_task.h"
#include "rf_task_common_services.h"
#include "rfgsm_msg.h"
#include "rfcommon_core.h"
#include "rfcom.h"


/*=============================================================================

                                 DEFINITIONS

=============================================================================*/
typedef struct
{
  rf_task_num_type rf_task;
  rfm_device_enum_type rfm_dev;
  rfm_device_enum_type rxd_rfm_dev;
  uint8 sub_id;
  sys_modem_as_id_e_type as_id;
  uint8 triple_buffer_id;
  boolean enable_rx_diversity;
  boolean device_is_tx;
  boolean prx_drx_combined_mode;
  boolean gl1_lower_rx_ccs_prio;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr_rxd;
}rfgsm_core_data_type;

#ifdef FEATURE_RF_ASDIV
/* Structure to hold the data to be used by the callback function used to create AsDiv script */
typedef struct
{
  rfm_device_enum_type  device;
  rfcom_gsm_band_type   band;       /* GSM RFM Band class type */
  boolean               immediate;  /* FALSE = script to be created. TRUE = immediate switch is to be done */
  int16                 start_delta;/* XSW start offset with respect to start of rx burst */
} rfgsm_asdiv_xsw_script_data;
#endif /* FEATURE_RF_ASDIV */

typedef struct
{
  uint8                    sub_id;
  msgr_umid_type           therm_read_umid;
  msgr_umid_type           therm_resp_umid;
}rfgsm_sub_to_therm_umid_map_type;

extern rfgsm_sub_to_therm_umid_map_type rfgsm_sub_to_therm_umid_map[3];


/*=============================================================================

                             FUNCTION PROTOTYPES

=============================================================================*/
boolean rfgsm_core_enter_mode(rfgsm_core_data_type *core_data, uint32 rxlm_buf_idx, const void *shared_mem_ptr);

boolean rfgsm_core_build_enter_mode_ccs_events( rfgsm_core_data_type* core_data,
                                                const void *enter_mode_ptr);

boolean rfgsm_core_build_wakeup_mode_ccs_events( rfgsm_core_data_type* core_data,
                                                 const void *wakeup_mode_ptr);

rf_task_num_type rfgsm_core_get_task_num( void );

rf_task_num_type rfgsm_core_get_task_id_from_sub_id(uint8 sub_id );

boolean rfgsm_core_init(rfm_device_enum_type rfm_dev);
boolean rfgsm_core_deinit(void);

boolean rfgsm_core_init_once(void);

boolean rfgsm_core_prepare_therm_read(rfgsm_core_data_type *core_data, const void *shared_mem_ptr);

boolean rfgsm_core_build_therm_read_event(rfgsm_core_data_type *core_data);

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the Tx scripts to program the synthesizer to the
  requested channel.

  rfgsm_set_band must be called before this function or the 
  FEATURE_SET_BAND_IN_GL1 must be turned off so it may be called in this 
  function before proceseding

  @retval
  TRUE/FALSE - sucessful/unsuccessful tune.
*/
boolean rfgsm_core_tune_tx(rfgsm_core_data_type *core_data, rfa_rf_gsm_tx_burst_type *tx_burst_params);

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the Rx scripts to program the synthesizer to the
  requested channel.

  @retval
  TRUE/FALSE - sucessful/unsuccessful tune.
*/
boolean rfgsm_core_tune_rx(rfgsm_core_data_type *core_data, uint16 arfcn);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param 

   @retval 
   
*/
boolean rfgsm_core_sleep( rfgsm_core_data_type *core_data );

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param 

   @retval 
   
*/
boolean rfgsm_core_wakeup( rfgsm_core_data_type *core_data, const void *shared_mem_ptr );

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function computes and validates the Rx power at the antenna.  It
  accomplishes this by using the following formula:

              10log(rssi) - Grx

  where Grx represents the entire Rx chain gain from the antenna input to
  baseband.

  NOTE: Grx = G(digital) + G(rf)

  The design of this function assumes that rf_set_rx_gain() was
  previously invoked;  the gain value that is required for the Rx power
  computation is established there.

  @param
  rssi_value    : input,  (I^2 + Q^2) value
  dBm_x16_value : output, absolute power result
  gain_range    : output, gain range used in deriving the dBm_x16_value
                  (only provided for informational purposes - logging)
  stored_gain  : gain to be computed 

*/
rf_measurement_T rfgsm_core_compute_rx_power(uint32 rssi_value,
                                             dBx16_T *dBm_x16_value,
                                             rf_gain_range_T *gain_range,
                                             const rf_gain_T *stored_gain,
                                             rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
/*!
  @details
   This function calculates the amam tables for the specificed power
   leves and returns a pointer to the table.

  @param pwr_profile[5] : power level for 5 possible tx bursts.

  @retval
  pointer to the profile to be used.

  @pre
     NV must have been initialized.
*/
void rfgsm_core_set_tx_power ( rfgsm_core_data_type *core_data, 
                               rfgsm_power_profile_type pwr_profile[5],
                               uint8 num_tx_slots,
                               uint8 num_assigned_uplink_slots);

/*----------------------------------------------------------------------------*/
/*!
  @details
   This function applies SAR limit passed from MCS

  @param sar_limit_gmsk[5] : SAR limits for GSM for all 5 different multislot configs
         sar_limit_8psk[5] : SAR limits for EDGE for all 5 different multislot configs

*/
void rfgsm_core_set_simult_sar_limit(rfgsm_core_handle_type *rfgsm_core_handle_ptr, int16 sar_limit_gmsk[5], int16 sar_limit_8psk[5]);

/*----------------------------------------------------------------------------*/
/*!
  @details
   This function gets current Tx pwr limit for a given rfm device, modulation and no of uplink slot.
   This limit is the value as requested by MCS for current state of all tx chains.

  @param rfm_dev : RFM device for which tx pwr limit is required
               mod_type : 0 for GMSK, 1 for 8PSK
               uplink slots : No of Uplink slots in a frame
               simult_tx_pwr_limit : Variable to return tx_pwr_limit               
*/

int16 rfgsm_core_get_simult_tx_pwr_limit(rfm_device_enum_type rfm_dev, rfgsm_modulation_type mod_type, uint8 uplink_slots);


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param  

   @retval 
   
*/
void rfgsm_core_set_band(rfgsm_core_data_type *core_data, rfcom_gsm_band_type band );


/*----------------------------------------------------------------------------*/
void rfgsm_core_turn_on_vregs(rf_path_enum_type path, uint8 sub_id, rfm_path_state rf_path_state);

/*----------------------------------------------------------------------------*/
/*!
  @details
  This functions sets up the Rx scripts to configure the Rx gain and the LNA
  based on the requested Rx level.  The gain range switch points are
  adjusted by the calibration results to apply the correct gain setting.

  This function relies on the arfcn information to be valid for the given
  burst so that the appropriate CAL offsets can be applied; therefore,
  rf_tune_to_channel() must be called first.

  rx_lvl_dBm - 1/16 dB format: sign bit, 11 decimal bits, 4 fractional bits

  @retVal
  TRUE - requested gain was set.
  FALSE - unable to set the requested gain.
*/
boolean rfgsm_core_set_rx_gain(rfgsm_core_data_type *core_data,
                               dBx16_T rx_lvl_dbm, 
                               rf_gain_T *gain, 
                               uint8 lna_range,
                               uint16 arfcn,
                               rfgsm_enh_rx_params_type enh_rx_params);

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the GRFC and SBI/uwire buffer required for the
  Rx burst.

  @param burst : burst type
  @param buffer : buffer to use for the burst
*/
void rfgsm_core_setup_rx_burst(rfgsm_core_data_type* core_data,
                               rf_burst_type burst,
                               rfgsm_rx_burst_type rx_burst_type);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls all the rfgsm_core functionality to create the RF CCS for all the
  bursts requested from GL1.

  This function can be called from RF Task context from the rfgsm_cmd_processing dispatcher
  or iRAT scenarios.

  @param device                   : RF Multimode device
  @param num_rx_bursts            : Number of Rx/Monitor bursts required
  @param rx_chan_params           : Rx burst parameters for each burst i.e arfcn
  @param gfw_rf_burst_event_ptrs  : Pointer to an array of pointers to shared mem
*/
boolean rfgsm_core_build_rx_burst_ccs_events( rfgsm_core_data_type* core_data,
											  void* rx_burst_event_ptr);



/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the GRFC and SBI/uwire buffer required for the
  Tx burst.

  @retval
  tx_alpha - Tx alpha value for the burst set up
*/
uint16 rfgsm_core_setup_tx_burst(rfgsm_core_data_type *core_data, uint8 num_tx_slots, void *tx_burst_event_ptr );

/*----------------------------------------------------------------------------*/
boolean rfgsm_core_populate_burst_rflm_data_info( rfgsm_core_data_type *core_data,
                                                  rf_burst_type burst,
                                                  uint8 num_tx_slots,
                                                  void *burst_event_ptr );

/*----------------------------------------------------------------------------*/
uint32 rfgsm_core_convert_arfcn_to_rx_freq(rfcom_gsm_band_type , uint16 );
/*----------------------------------------------------------------------------*/

void rfgsm_core_enable_tx(boolean enable, uint32 txlm_buf_idx, rfgsm_core_data_type *core_data);

/*----------------------------------------------------------------------------*/

void rfgsm_core_update_tx_timing(rfgsm_core_data_type *core_data);

/*----------------------------------------------------------------------------*/
void rfgsm_core_nv_init(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/

boolean rfgsm_core_exit_mode(rfgsm_core_data_type *core_data, rfcom_mode_enum_type to_mode);

/*----------------------------------------------------------------------------*/

void rfgsm_core_get_timing_info(rfgsm_core_data_type *core_data, rfgsm_timing_info_type *timing_info, boolean hmsc_mode);

/*----------------------------------------------------------------------------*/

void rfgsm_core_set_slot_override_flag( rfgsm_core_handle_type *rfgsm_core_handle_ptr, boolean override_val );

/*----------------------------------------------------------------------------*/

void rfgsm_core_set_bias_override_flag( rfm_device_enum_type rfm_dev, uint8 override_flag );

/*----------------------------------------------------------------------------*/

void rfgsm_core_do_idle_frame_processing(rfgsm_core_data_type *core_data);

/*----------------------------------------------------------------------------*/
void rfgsm_core_log_iq_data(rfm_device_enum_type rfm_dev, GfwIqSamplesBuffer* data);

/*----------------------------------------------------------------------------*/
int16* rfgsm_core_get_iq_buffer_ptr(rfm_device_enum_type rfm_dev);

void rfgsm_core_set_gl1_freq_error_flag( rfm_device_enum_type rfm_dev, boolean pass_flag);
boolean rfgsm_core_get_gl1_freq_error_flag( rfgsm_core_handle_type *rfgsm_core_handle_ptr );
void rfgsm_core_debug(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
uint16 rfgsm_map_arfcn_linearly(rfcom_gsm_band_type band, uint16 arfcn);

/*----------------------------------------------------------------------------*/
uint16 rfgsm_core_get_rf_sigma_delta_max_db(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
boolean rfgsm_core_handle_init(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
boolean rfgsm_core_handle_nv_init(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
boolean rfgsm_core_wakeup_data_init(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
rfgsm_core_handle_type* rfgsm_core_handle_get(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
rfgsm_core_handle_type* rfgsm_core_handle_create_and_init(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
boolean rfgsm_core_handle_destroy(void);

int8 rfgsm_core_set_tx_pow_dBm (rfgsm_core_handle_type *rfgsm_core_handle_ptr, int16  powdBm, int8 index);

uint8 rfgsm_core_get_pa_range(rfm_device_enum_type rfm_dev, uint8 slotcnt);

/*----------------------------------------------------------------------------*/
void rfgsm_core_msg_log_handler(rf_apps_cmd_type *debug_msg_log_cmd);

/*----------------------------------------------------------------------------*/
void rfgsm_core_append_apt_script(rf_buffer_intf* dest_buf_ptr, rf_buffer_intf* src_buf_ptr);
/*----------------------------------------------------------------------------*/
uint32 rfgsm_core_set_tuner_tune_code_override( rfm_device_enum_type rfm_dev,uint8 override_flag,void *data,uint8* tuner_nv_ptr,uint8 *tuner_id_ptr);

/*----------------------------------------------------------------------------*/
void rfgsm_core_allocate_rx_script_buffers( rfgsm_core_data_type *core_data );

/*----------------------------------------------------------------------------*/
void rfgsm_core_free_rx_script_buffers( rfgsm_core_data_type *core_data );
/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_clear_rx_burst_device_scripts(rfm_device_enum_type rfm_dev);

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_clear_tx_burst_device_scripts( rfgsm_core_handle_type *rfgsm_core_handle_ptr);

/*----------------------------------------------------------------------------*/

boolean rfgsm_core_populate_rx_burst_device_buffers( rfgsm_core_data_type* core_data,
													 boolean enable_rx_diversity,
                                                     rf_burst_type burst);

/*----------------------------------------------------------------------------*/

boolean rfgsm_core_populate_tx_burst_device_buffers( rfgsm_core_data_type *core_data,
													 rf_burst_type burst);
/*---------------------------------------------------------------------------------------*/

uint16 rfgsm_core_get_max_pcl_index( rfm_device_enum_type rfm_dev, int16 *tx_pow_array, uint8 num_slots, uint8* slot_index);

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_ten_log_of_power_val(uint32 power, dBx16_T *dBx16_result_ptr);

/*---------------------------------------------------------------------------------------*/
void rfgsm_core_set_radio_gain(rfm_device_enum_type rfm_dev, dBx16_T rx_lvl_dbm,  rf_gain_T *gain, rfgsm_enh_rx_params_type enh_rx);

/*---------------------------------------------------------------------------------------*/
dBx16_T rfgsm_core_get_lna_gain(rfm_device_enum_type rfm_dev, uint16 arfcn, uint16 gain_range);

/*---------------------------------------------------------------------------------------*/
uint16 rfgsm_core_get_pcl_index( rfm_device_enum_type rfm_dev, int16 tx_pwr);

/*---------------------------------------------------------------------------------------*/
rfcom_gsm_band_type rfgsm_core_get_tx_band( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/
rfcom_gsm_band_type rfgsm_core_get_rx_band( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_is_device_prx (rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/

rfm_device_enum_type rfgsm_core_get_prx_dev( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/

rfm_device_enum_type rfgsm_core_get_drx_dev( rfm_device_enum_type prx_device );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_set_device_capability(rfm_device_enum_type rfm_dev,
                                         rf_path_enum_type rf_path );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_get_device_tx_capability(rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_get_device_rx_capability(rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/

void rfgsm_core_log(rfm_device_enum_type rfm_dev);

/*---------------------------------------------------------------------------------------*/

void rfgsm_core_calc_rx_timing(rfm_device_enum_type rfm_dev);

/*---------------------------------------------------------------------------------------*/
void rfgsm_core_update_rx_timing(rfgsm_core_data_type *core_data);

/*---------------------------------------------------------------------------------------*/

void rfgsm_core_enter_mode_rx_init(rfgsm_core_data_type *core_data);

/*---------------------------------------------------------------------------------------*/

void rfgsm_core_enter_mode_tx_init(rfgsm_core_data_type *core_data);

/*---------------------------------------------------------------------------------------*/

uint16 rfgsm_core_get_arfcn_bin_index_char_offset_tbl(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, uint16 arfcn, rfc_rxtx_enum_type rx_tx);

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_init_master_pa_ramp_tables( rfgsm_core_data_type *core_data );

boolean rfgsm_core_scale_tx_pa_ramp_data( rfm_device_enum_type rfm_dev,
                                          uint8 sub_id,
                                          uint8 tx_slot,
                                          uint16 pa_scale );

boolean rfgsm_core_tx_pa_ramp_data_buffer_switch( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_tx_amam_ampm_data_buffer_switch( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_build_ip2_cal_setup_script(rfgsm_core_data_type *core_data_ptr, 
                                              rfcom_gsm_band_type rfcom_band, 
                                              uint16 arfcn, 
                                              uint16 tx_rgi, 
                                              uint8 lna_range );


/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_build_ip2_cal_cleanup_script(rfm_device_enum_type rfm_dev, 
                                                rfcom_gsm_band_type rfcom_band);

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_build_ip2_cal_step_script(rfm_device_enum_type rfm_dev, 
                                             rfcom_gsm_band_type rfcom_band, 
                                             rfgsm_ip2_cal_alg_params_struct* ip2_cal_info,
                                             rfgsm_ip2_cal_grid_info_type *grid_info_ptr);



/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_msm_init_ip2_cal(rfgsm_core_data_type *core_data_ptr, 
                                    uint32 ip2_rxlm_idx, 
                                    uint32 ip2_txlm_idx);

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_build_ip2_cal_script(rfgsm_core_data_type *core_data_ptr);


/*---------------------------------------------------------------------------------------*/


boolean rfgsm_core_get_sawless_status(rfm_device_enum_type rfm_dev);

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_get_band_sawless_status(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfcom_band);

/*---------------------------------------------------------------------------------------*/

void rfgsm_core_init_sawless_status(rfm_device_enum_type rfm_dev);

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_load_default_ip2_cal_params(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfcom_band);

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_use_ip2_cal_results(rfm_device_enum_type rfm_dev, 
                                       rfcom_gsm_band_type rfcom_band, 
                                       uint8 sub_id,  
                                       rfgsm_ip2_cal_params_type *ip2_cal_params,
                                       uint8 flag_mask,
                                       boolean cal_end );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_get_jdet_config_scripts(rfgsm_core_data_type *core_data_ptr);

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_build_jdet_read_scripts(rfgsm_core_data_type *core_data_ptr);

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_allocate_high_lin_flag_table( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_deallocate_high_lin_flag_table( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_is_arfcn_in_high_lin( rfm_device_enum_type rfm_dev, 
                                         uint16 arfcn, 
                                         rfcom_gsm_band_type band,
                                         boolean *is_high_lin );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_set_arfcn_to_high_lin ( rfm_device_enum_type rfm_dev, 
                                           uint16 arfcn, 
                                           rfcom_gsm_band_type band );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_set_arfcn_to_low_lin ( rfm_device_enum_type rfm_dev, 
                                          uint16 arfcn, 
                                          rfcom_gsm_band_type band );

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_antenna_tuner_program_qfe_scenario ( void );
/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_process_burst_metrics( rfm_device_enum_type rfm_dev,
                                          rfgsm_band_type      band,
                                          uint16               arfcn,
                                          dBx16_T              rx_lvl_dbm,
                                          uint16               snr,
                                          uint16               jdet_value);

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_get_sawless_switchpoints(rfm_device_enum_type rfm_dev, 
                                            uint16 arfcn, 
                                            rfcom_gsm_band_type band, 
                                            rfgsm_nv_rx_switch_type *rx_swt_ptr);

/*---------------------------------------------------------------------------------------*/
#ifdef FEATURE_TX_SHARING
boolean rfgsm_core_build_tx_enable_ccs_events( rfgsm_core_data_type* core_data,
											  rfm_device_enum_type rfm_dev);
boolean rfgsm_core_build_tx_disable_ccs_events( rfgsm_core_data_type* core_data,
											  rfm_device_enum_type rfm_dev);
#endif
/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_update_alternate_rx_path_port_mapping( rfgsm_core_data_type *core_data, uint8 *alt_path );

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_update_irat_alternate_rx_path_port_mapping( rfm_device_enum_type rfm_dev, 
                                                               uint8* alt_path,
                                                               uint8 rfc_rx_supported_band_mask);

/*---------------------------------------------------------------------------------------*/

boolean rfgsm_core_get_alternate_rx_path_per_band( rfm_device_enum_type rfm_dev,
                                                   rfcom_gsm_band_type rfgsm_band,
                                                   uint8* alt_path);

/*---------------------------------------------------------------------------------------*/

uint32 rfgsm_core_get_ccs_exe_us( rfm_device_enum_type rfm_dev, rf_buffer_intf *script_ptr);

/*---------------------------------------------------------------------------------------*/

void rfgsm_core_populate_device_status_read_buffers( rfgsm_core_data_type *core_data, 
                                                     rf_burst_type burst_type);

/*---------------------------------------------------------------------------------------*/

void rfgsm_core_validate_rx_timing_info(void);

/*---------------------------------------------------------------------------------------*/

#ifdef FEATURE_RF_ASDIV

/*----------------------------------------------------------------------------*/
void rfgsm_core_asd_rsp_check_init( rfm_device_enum_type rfm_dev );

/*----------------------------------------------------------------------------*/
void rfgsm_core_asd_rsp_check_start( rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
void rfgsm_core_asd_rsp_check_stop( rfm_device_enum_type rfm_dev );

/*----------------------------------------------------------------------------*/
void rfgsm_core_asd_rsp_check_trigger(uint32 t_unused, timer_cb_data_type data);

#endif

boolean rfgsm_core_handle_rx_debug_logging_enable_init(rfm_device_enum_type rfm_dev);

int32 rfgsm_core_get_freq_error(rfgsm_core_handle_type *rfgsm_core_handle_ptr);

void rfgsm_core_update_freq_error(rfgsm_core_data_type *core_data, int32 freq_err);

void rfgsm_core_update_rx_freq_error(rfgsm_core_data_type *core_data, int32 freq_err);

#endif /* RFGSM_CORE_H */

