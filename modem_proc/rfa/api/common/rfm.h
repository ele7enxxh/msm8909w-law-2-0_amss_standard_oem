#ifndef RFM_H
#define RFM_H

/*
   @file
   rfm.h

   @brief
   RF Driver's common external interface file.

   @details

*/

/*===========================================================================
Copyright (c) 2002 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/05/16   hm      Added a new API 
                   rfm_get_sys_lte_band_mask_from_rflte_band_mask()
05/14/15   cv      Added support for 256 lte band mask check
10/20/14   vbh     Add a debug routine that prints WTR registers
09/24/14   pl      Added get_rx_lo_freq support
09/10/14   vv      Added RFM therm API support
02/18/14   sc      Remove unused code and dependencies
10/30/13   svi     Added support for TxPLL AFC
09/05/13   pl      Added Asdiv init default switch position API
08/14/13   ak      phone enter to "offline" with QRD RP4 RF card on TR1.1 CRM build
07/23/13   aro     Added inteface to do immediate write
07/22/13   aro     Added immediate flag for rfm_set_antenna()
07/18/13   aro     CB Pointer fix
07/17/13   aro     Moved the L1 callback definition to rfm.c
06/05/13   kb      [XPT] Support to update rfm_mode for TDS in online mode
05/31/13   pl      Added API for Antenna Switch Diversity Feature
03/11/13   sar     Updates for APQ, GNSS only target.
01/21/13   nrk     Changed input param type path to device for rfm_get_synth_lock_status
01/04/13   gvn     Move interface to query RF LTE CA bc config to rflte_mc
11/28/12   nrk     Added API to get SYNTH lock  
11/15/12   adk     Backed out Qtuner code
08/16/12   ndb     Change antenna tuner settings api to be customizable 
10/25/12   gvn     Interface for LTE CA bc config
07/26/12   vrb     Move TRM interface prototype to rfm_device_config_type.h
05/25/12   kb/npi  Updated the rfm_init() state using an enum
05/21/12   aca     AFC interface cleanup
04/13/12   kai     Revert the following update_txlm_buffer change since gsm layer1 dependence not ready
04/13/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx 
03/08/12   sar     Exported rfm_is_rfm_init_success() for RCINIT. 
02/09/12   aca     Tx PLL error correction
12/14/11   sbm     added rfm_get_rx_freq_from_rx_earfcn() function.
11/07/11   aak     Changes to implement Quiet mode  
10/13/11   whc     Adding RFM layer function to return the current PA state. 
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and FEATURE_RF_WCDMA_LM_RFM_INTERFACE.
08/12/11   jhe     Added support for TDS band mask
06/30/11   Saul    Compile error fix for CL 1817866.
06/14/11   aro     Updated TRM interface to return data validity
04/28/11   aro     Removed number of devices in TRM interface as this info
                   is redundant and TRM can deduce this based on other param
04/26/11   aro     Added number of devices in TRM interface
04/22/11   aro     Doxygen Documentation Update
04/22/11   aro     [1] Updated Device Configuration interface to have reduced
                   mask length for tech and antenna.
                   [2] Added enum to perform masking on multi-tech support
04/21/11   aro     Added Interface for TRM to get Device Configuration
04/20/11   dw      Temporarily add back lm buffer indexes
04/19/11   pl      Remove Warnings
04/19/11   bmg     Fixed extern "C" locations
04/19/11   aro     Added prototype for Get Rx Wakeup function
04/07/11   sar     Relocated to rfa/api to conform to CMI, Removed rfm_helper_incs.h 
                   and included files previously exported thru this file.
02/28/11   tnt     Merge to MDM9K PLF110 
02/25/11   ka/dw   Merge support for thermal mitigation  
02/17/11    av     Reverting GP_CLK changes 
02/02/11   av      Disable GP_CLK 
01/28/11    dw     RFM interface change for TxLM
01/05/10    dw     RFM interface change for RxLM
11/29/10   dbz     Added GSM fast scan notification API
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
09/23/10   sty     Reverted previous change
09/22/10   sty     Added rfm_init()
08/04/10    av     Update to LTE B13 GPS algorithm 
06/30/10    ap     Added support for Power Collapse & Early clock enable 
06/28/10   can     Removing Power collapse changes that cause a crash.
06/24/10   can     Support for LTE BC Config.
06/22/10    ap     Added support for Power Collapse 
05/21/10   lcl     Back out clock changes.  Wasn't broken GSM.
05/21/10   tws     Add API function to convert sys_band to rf_card_band.
04/01/10   adk     Merged branch scmm_mdm9k_merge_final
03/01/10   kma     Added power collapse status API
02/16/10   sty     Added prototype for rfm_get_rx_warmup_time()
10/22/09   dyc     Mode specific build support.
10/22/09   wen     Fix compiling warnings 
10/16/09   jhe     Added rfm_power_clock_on/off
08/31/09   bmg     Moved truly common functions to rfm_common.h
03/12/09   sar     Added changes needed for 1x
10/21/08   rmd     QSC7X30 code clean up REV1: Mainline/Removed features 
                   (FEATURE_..., RF_HAS_.. and T_MSM...)
08/18/08   adk     Moved function prototype rfm_get_tx_carrier_freq() from rf1x.h
07/14/08   adk     Ported DORB changes from the 7800 branch.
06/04/08   ra      move rfm_gps_pause() and rfm_gps_resume() to rfm file
06/11/08   ad      Added desciption for ADC return value in rfm_adc_read()
05/08/08   dw      Fixed two function prototypes.
05/07/08   ad      Export generic ADC read functionality through RFM API
05/07/08   jfc     Fixed prototypes for rfm_get_tx_pwr_limit()
04/24/08   dw      Added rfm_get_cgagc_settling_time_for_tune API to support both tune scenarios.
12/07/07   jfc     Add update_pavmode_burst_data() API for GL1
11/14/07   jfc     Export API for getting switchpoints
08/15/07   Vish    Renamed rfm_get_rx_agc_db256() to rfm_get_rx_agc(), and added
                   rfm_get_tx_[adjust/open_loop/pilot_power/total_power] APIs.
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
08/06/07   jfc     Featurized code for EDGE-only builds
08/06/07   bmg     Updated rfm_enable_diversity comments and return type
08/01/07   Vish    Added rfm_get_rx_agc_db256() and two associated macros -
                   RFM_GET_RX[0/1]_AGC_DB256().

07/12/07   jfc/lcl Added lna_range param for set_rx_gain()
06/25/07   Vish    Added rfm_enable_diversity().
                   Moved rfm_get_sub_class(), and rfm_get_rx_carrier_freq()
                   from "rf1x.h".
05/08/07   jfc     Fixed rfm_band_supports_rxd() to take rfi, not rfcom band type
02/28/07   ycl     Fix compiler warnings.
02/27/07   jfc     Cleaned up rfm_ioctl() headers
02/12/07   jfc     Added channel type parameter to set_tx_general_ctl()
02/05/07   ycl     Merge to 7200 tip.
01/30/07   jfc     Put rfm_get_rf_warmup_time() back
01/23/05   ra      Add support for rfm_trigger_gps_bbc_update()
01/17/07   jfc     Fixed featurization and cleaned up declarations.
01/17/07   jfc     Wrapped UMTS code with RF_HAS_FEATURE_WCDMA
01/11/06   ra      Added rfm_exit_gps
12/18/06   jfc     Moved rfm_is_sleeping to RFM layer
12/16/06   jfc     Added API for antenna for TRM layer
11/16/06   jfc     Removed warnings about deprecated declarations. Changed
                    rfm_get_band_mask() to take device type
10/31/06   ycl     Added rfm_get_mdsp_tx_power_limit() and 
                   rfm_get_mdsp_tx_agc() functions. 
10/17/06   ycl     Modifications for initial MSM7600 build.
04/12/06    bn     Pass down appropriate dev_ptr based on the device
04/03/06    dp     Add RxD query function to check for diversity capabilities
                   based on band.
03/16/06   jfc     Added rfm_enable_data_mover_clk_for_cm()
03/09/06   adm     Added API for returning RF warmup time to L1.
03/10/06    xw     Deleted rfm_band_enum_type.
03/06/06   rsr/rv  Added support for multislot power back off.
03/06/06   rsr     Mainlined T_MSM6280.
03/06/06    bn     Added multi chain RF API support.                   
02/17/06    xw     Included bsp.h and removed unused rfm_band_enum_type. 
01/17/06    xw     Added rfm_get_max_tx_power_nv_item() and rfm_set_tx_general_ctl().
01/13/06    xw     Added egprs_mode in rfm_set_amam_profile().
01/11/06    xw     Added new APIs to enforce other function area to go thru RFM.
11/15/05   jfc     Added new interfaces for GtoW, WtoG, and WtoG operation
11/04/05   adm     Merged changes from 6275 tip.
07/29/05   ycl     Correct rx agc interface funtion prototypes.
07/14/05   ycl     Added interface for initial prach tx min power.
04/27/05   jtn     Added extern to prevent compiler warning.
04/07/05   ycl     Add interface to get serving and away rx agc values.
03/29/05    xw     Added rfm_get_band_mask().
01/04/05    rv     Updated comments for  rfm_get_tx_pwr_limit().
12/17/04    rv     Added rfm_get_tx_pwr_limit
08/09/04    eh     Added rfm_band_enum_type.
05/11/04   dhh     Added AFC support.
03/10/04    eh     Modified trkloadj and rotator functions.
06/20/03    eh     Wrapped freq. maint. functions under FEATURE_FREQ_VS_TEMP_NV_UPDATE
05/07/03   thh     Include rfgsm.h instead of rfmgsm.h.  Rfmgsm.h is obsolete.
03/27/03   rv      Added MM API for frequency maintenance algorithm in WCDMA.
03/05/03   eh      Modified rfm_init to pass in a state variable. This state
                   indicates whether RF NV is valid or not. If it is invalid
                   then returns false, otherwise returns true.
12/13/02   sd      Removed temporary GSM synth tuning types (MON and PWR) for
                   non CDMA1X builds.
11/04/02   sd      Added support for intersystem measurements.
10/10/02   sar     Added support for cdma 1x mode.
09/10/02   sd      Added support for multimode API that includes GSM drivers.
07/01/02   sd      Initial revision.

============================================================================*/

#include "rfcom.h"
#include "rfm_types.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "db.h"
#endif
#include "rfumts.h"
#include "rfllgsm.h"
#include "rfm_common.h"
#include "rfm_device_config_type.h"
#include "rfm_init_event.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RFM_MAX_NUM_FREQ_LIST 3

/*----------------------------------------------------------------------------*/
/*! Type defintion for L1 Call back function */
typedef void(*rfm_l1_cb_type)(void *cb_data);

typedef struct
{
  uint8 num_valid_freq;
  uint32 frequency_list[RFM_MAX_NUM_FREQ_LIST];
} rfm_frequency_list_type;

/*----------------------------------------------------------------------------*/
/*
  Enum to indicate whether RFM initialization was done and if so, whether
  this was successful
*/
typedef enum
{
  RFM_INIT_FAILED,                /* rfm_init() ran but failed    */
  RFM_INIT_SUCCEEDED,             /* rfm_init() ran and succeeded */
  RFM_INIT_NOT_RUN,               /* rfm_init() did not run       */
} rfm_init_state_enum_type;

/*----------------------------------------------------------------------------*/
/*! This enum lists the types of Simultaneous RF Operation. */
typedef enum
{

  RFM_SIMULTANEOUS_RX,
  /*!< Simultaneous Receiver */ 

  RFM_SIMULTANEOUS_TX
  /*!< Simultaneous Transkmitter */ 

} rfm_simultaneous_op_enum_type;


/*! therm device handle for external use*/
typedef uint8 rfm_thermistor_handle_type;


/*! temperature result type */
typedef struct
{
  /*variable to store the read temperature in dgree celcius */
  int16 therm_read_deg_C;

}rfm_thermistor_result_type;

/*! This structure contains therm handles and any other associated data that may be used in the future by the thermal algorithm 
    to improve their performance */
typedef struct
{
  rfm_thermistor_handle_type handle;

  /*To identify the thermistor withtin the chip when you have multiple thermistors per chip.*/
  uint8 thermistor_within_chip;
  /*Device info of the therm device*/
  uint16 mfg_id; /*! RFFE Manafacturer ID  */
  uint16 prd_id; /*! RFFE Product ID */
  uint16 rev_id; /*! Revision ID */

}rfm_thermistor_type;


/*!--------rfm therm APIs for external use----------*/



/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm API to get the active number of thermistors
 
  @details
  rfm API to get the active number of thermistors

  @param   size : out parameter. Returns the number of active thermistors

*/
boolean rfm_get_thermistor_count(uint8* size);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm API to enumerate and return the active thermistors for external use
 
  @details
  rfm API to enumerate and return the active thermistors for external use

  @param   size : in+out parameter. It specifies the number of entries provide in the array by the caller, 
                  and is updated to the number of thermistors that have been filled in after the call
  @param   therm_device_list : out parameter.Array of structures containing handles and any other associated data 
                      that may be used in the future by the thermal algorithm to improve their performance

*/
boolean rfm_enumerate_thermistors(uint8* size, rfm_thermistor_type* therm_device_list);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm API to read and return the temperature for the requested thermistor 
 
  @details
  rfm API to read and return the temperature for the requested thermistor 

  @param  handle : in parameter to specify the thermistor to read temperature
  @param  result : out parameter which will be updated with the therm read value if the read was successfull

*/
boolean rfm_query_thermistor(rfm_thermistor_handle_type handle, rfm_thermistor_result_type* result);

/*!-------------------------------------------------*/


/*----------------------------------------------------------------------------*/
uint32 rfm_tune_to_chan ( rfcom_device_enum_type device, uint16 channel,
                          rfm_synth_tune_enum_type synths,
                          const void *user_data_ptr,
                          rfm_cb_handler_type cb_handler);

/*----------------------------------------------------------------------------*/
uint32 rfm_enable_tx ( rfcom_device_enum_type device,
                       const void *user_data_ptr,rfm_cb_handler_type cb_handler
                       ,uint32 txlm_buf_idx
                       );

/*----------------------------------------------------------------------------*/
void rfm_disable_tx (rfcom_device_enum_type device
                     ,uint32 txlm_buf_idx
                    );

/*----------------------------------------------------------------------------*/
void rfm_set_quiet_mode (rfm_mode_enum_type mode, boolean enable);

/*----------------------------------------------------------------------------*/
boolean rfm_is_sleeping( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
uint32 rfm_sleep( rfcom_device_enum_type device, const void *user_data_ptr,
                  rfm_cb_handler_type cb_handler 
                  ,uint32 rxlm_buf_idx
                  );

/*----------------------------------------------------------------------------*/
uint32 rfm_wakeup ( rfcom_device_enum_type device, rfm_mode_enum_type rf_mode,        
                    const void *user_data_ptr, rfm_cb_handler_type cb_handler
                    ,uint32 rxlm_buf_idx
                  );     

/*----------------------------------------------------------------------------*/
uint32 rfm_enable_rx ( rfcom_device_enum_type device, const void *user_data_ptr,            
                       rfm_cb_handler_type cb_handler);

/*----------------------------------------------------------------------------*/
void rfm_disable_rx ( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
void rfm_multimode_handover (rf_multimode_handover_cmd_type chain_0_cmd,
                             rf_multimode_handover_cmd_type chain_1_cmd);

/*----------------------------------------------------------------------------*/
uint64 rfm_get_band_mask ( rfcom_device_enum_type device );

#ifdef LTE_BAND_NUM 
/*----------------------------------------------------------------------------*/
sys_lte_band_mask_e_type rfm_get_lte_band_mask ( void ); 
sys_lte_band_mask_e_type 
 rfm_get_sys_lte_band_mask_from_rflte_band_mask ( uint64 rflte_band_mask );

#else 
/*----------------------------------------------------------------------------*/
uint64 rfm_get_lte_band_mask ( void );
#endif 

/*----------------------------------------------------------------------------*/
uint64 rfm_get_tds_band_mask ( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
rfcom_ioctl_return_type rfm_ioctl ( rfcom_device_enum_type device,
                                    rfcom_ioctl_param_type request,
                                    void *pbuf,
                                    int32 length );

/*----------------------------------------------------------------------------*/
rfcom_adc_err_enum_type rfm_adc_read(rfcom_adc_logical_channel_type adc_addr, 
                                     uint16 *adc_val);

/*----------------------------------------------------------------------------*/
/* 
uint32 
rfm_get_rx_warmup_time 
( 
  rfm_device_enum_type device, 
  rfm_mode_enum_type   rf_mode
);
*/

/*----------------------------------------------------------------------------*/
rf_card_band_type rfm_convert_sys_band_to_rf_card_band( sys_band_class_e_type );

/*----------------------------------------------------------------------------*/
void rfm_gnss_tx_band(rfcom_lte_band_type rf_tx_band); 
                                                       
/*----------------------------------------------------------------------------*/                                                       
void rfm_gnss_tx_indicator(boolean enable);

/*----------------------------------------------------------------------------*/
boolean 
rfm_reload_nv 
( 
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
void rfm_reduce_mptl_db_for_therm( uint8 backoff );

/*----------------------------------------------------------------------------*/
uint32 
rfm_get_rx_warmup_time 
( 
  rfm_device_enum_type device, 
  rfm_mode_enum_type   rf_mode
);

/*----------------------------------------------------------------------------*/
uint8 rfm_get_current_pa_state(void);

/*----------------------------------------------------------------------------*/
uint32 rfm_get_rx_freq_from_rx_earfcn(uint16 rx_chan, rfcom_band_type_u band, rfm_mode_enum_type mode);

/*----------------------------------------------------------------------------*/
uint32 rfm_get_tx_freq_from_tx_earfcn(uint16 tx_chan, rfcom_band_type_u band, rfm_mode_enum_type mode);

/*----------------------------------------------------------------------------*/
void rfm_get_txpll_script( rfm_mode_enum_type mode,
                           int32 rx_freq_error_in_hz, 
                           void* txpll_script_buf_ptr);

/*----------------------------------------------------------------------------*/
void rfm_get_rxpll_script( rfm_mode_enum_type mode,
                           void* rxpll_script_buf_ptr);

/*----------------------------------------------------------------------------*/
void rfm_power_clock_off(void);
/*----------------------------------------------------------------------------*/
void rfm_power_clock_on(void);
/*----------------------------------------------------------------------------*/

extern boolean rfm_is_rfm_init_success
(
  void
);

/*----------------------------------------------------------------------------*/
boolean rfm_get_calibration_state(void);

/*----------------------------------------------------------------------------*/

boolean rfm_deinit (void);

/*----------------------------------------------------------------------------*/
void
rfm_get_synth_lock_status
(
  rfm_device_enum_type device,
  rfcom_mode_enum_type rf_mode,
  rfcom_synth_type synth_type,
  void *is_synth_locked
);

/*----------------------------------------------------------------------------*/
boolean
rfm_set_antenna
(
  rfm_device_enum_type device, 
  uint32 ant_sw_pos,
  rfm_l1_cb_type cb_func,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rfm_set_antenna_imm
(
  rfm_device_enum_type device, 
  uint32 ant_sw_pos,
  rfm_l1_cb_type cb_func,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rfm_set_antenna_abort
(
  rfm_device_enum_type device, 
  uint32 ant_sw_pos,
  rfm_l1_cb_type cb_func,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rfm_init_antenna
(
  uint32 ant_sw_pos
);

/*----------------------------------------------------------------------------*/
void rfm_mode_update(const rfcom_device_enum_type device, const rfcom_mode_enum_type rf_mode);

/*----------------------------------------------------------------------------*/


boolean rfm_is_tech_supported(rfcom_device_enum_type device, rfm_mode_enum_type tech);


boolean rfm_is_coex_backoff_req (rfm_device_enum_type rfm_dev, rfm_mode_enum_type tech);

uint32 rfm_get_rx_lo_frequency(rfm_mode_enum_type tech, rfm_frequency_list_type freq_list);

void rfm_read_wtr_regs(uint8 register_set);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* RFM_H */

