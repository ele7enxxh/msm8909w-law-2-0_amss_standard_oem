#ifndef FTM_GSM_RFPROT_H
#define FTM_GSM_RFPROT_H

/*===========================================================================

    G S M  R F  C O N T R O L   F U N C T I O N S  P R O T O T Y P E S 
         

DESCRIPTION
   This file contains  definitions for RF controls which are also 
   contained in a file on the PC.  This keeps them in a place where they 
   can be easily modified and updated as necessary to aid in it's maintenance.

 
   Copyright (c) 2002, 2003 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   Copyright (c) 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Initially, comments will be entered manually. Later, as the module is
  placed under version control, the version control tool will automatic-
  ally insert change comments into this section. [Delete this comment].

  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/ftm_gsm_rfprot.h#1 $ 
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/27/14   tsr     GSM receive diversity 
05/06/13   aka     Changes to add payload length as a parameter in FRAME SWEEP APIs
04/23/13   ck      Add FTM IQ capture APIs to move from HWTC to FTM 
03/28/13   aka     GSM frame sweep feature changes.
12/05/12   aka     Adde rfm_dev in ftm_gsm_get_env_gain_nv
11/27/12   tws     Add device id & as_id to FTM.
11/19/12   aka     Added ftm_gsm_get_env_gain_nv to return NV value
08/30/12   ec      Changing name of prepare_tx_burst to reflect usage 
04/18/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API
04/26/12   sb      Add FTM recovery from RXFE stuck  
04/09/12   sb      Add changes to fix frequency error adjustment in FTM mode 
02/28/12   sb      Added changes to support disabling Idle frame in FTM mode 
11/29/11   sb      Added functionality for GSM IQ CAPTURE 
11/17/11   sb      Added FTM function to update frequency error 
09/07/11   sb/rsr  Changes for Delay match setting to use new FW interface.
07/07/11   vrb     Support for KV Cal V2
03/16/11   sr      Removed un-used code.
03/11/11   sr      Removed un-used featurization
31/01/11   sr      Removed un-used code
04/05/10   rsr     Added feature for multislot Rx.
02/08/10   sr      Lint fixes 
02/04/10   sr      Lint & kw fixes 
11/06/07   jfc     Added prototype for ftm_set_smps_pdm()
08/08/07   jfc     Mainlined T_MSM6275 feature
03/28/06   rsr     Lint cleanup. 
                   Changed RX burst API prototype (New parameter: number of frames 
                   over which RSSI is to be averaged)
                   Added prototype for ftm_gsm_do_rx_calibration
05/04/05   rsr     Removed warnings.Cleanup
03/04/05   rsr     Added support for OPLL calibration
12/09/04   rsr     Added support for autocalibration 
08/20/03   jwh     Added void param to ftm_gsm_tx_cal_sweep() decl 
7/21/03    rsr     Added support for tx cal API (Removed featurization around it)
7/14/03    rsr     Added support for continuous receive API
12/10/02   xw      Created.

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "ftm.h"
#include "ftm_gsm_ctl.h"
#include "ftm_gsm.h"
#include "gl1_defs.h"
#include "ftmdiag.h"
#include "ftm_rf_cmd.h"



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION FTM_GSM_SET_PDM

DESCRIPTION
   Sets the defined PDM to a defined value
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_set_pdm(rfm_device_enum_type rfm_dev, const ftm_id_val_type* pdm);

/*===========================================================================
FUNCTION FTM_GSM_GET_PDM

DESCRIPTION
   Gets the defined PDM to a defined value
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
int16 ftm_gsm_get_pdm(rfm_device_enum_type rfm_dev, ftm_gsm_pdm_id_type pdm_id);

/*===========================================================================
FUNCTION FTM_GSM_SET_TX

DESCRIPTION
   Turns entire TX chain on or off. This function is a wrapper for MDSP
   driver function mdsp_set_tx()
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_set_tx(rfm_device_enum_type rfm_dev, boolean on_off);

/*===========================================================================
FUNCTION FTM_GSM_SET_PA

DESCRIPTION
   Turns on/off PA.
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_set_pa(rfm_device_enum_type rfm_dev, boolean on_off);

/*===========================================================================
FUNCTION FTM_GSM_SET_LNA_RANGE

DESCRIPTION
   Sets the LNA gain range to High or Low. The LNA is implemented as a part
   of PMB 2411 (DP2000) and programmed via uWire command. The setting will 
   take effect on the next RX burst

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_set_lna_range(rfm_device_enum_type rfm_dev, ftm_lna_range_type range);

/*===========================================================================
FUNCTION FTM_GSM_SET_ARFCN

DESCRIPTION
   Sets the ARFCN

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_set_arfcn( rfm_device_enum_type rfm_dev, uint16 arfcn);


/*===========================================================================
FUNCTION FTM_GSM_DO_TX_BURST_BUFFER_DATA

DESCRIPTION
   Sets up the Tx burst from the data buffer

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_do_tx_burst_buffer_data(rfm_device_enum_type rfm_dev );

/*===========================================================================
FUNCTION FTM_GSM_DO_TX_CONT_RND

DESCRIPTION
   Transmits continuous psedo random data

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_do_tx_cont_rnd( rfm_device_enum_type rfm_dev );

/*===========================================================================
FUNCTION FTM_GSM_DO_TX_CONT_TONE

DESCRIPTION
   Transmits continuous tone data

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_do_tx_cont_tone( rfm_device_enum_type rfm_dev );

/*===========================================================================
FUNCTION FTM_GSM_DO_RX_CMD_RX_BURST_METRICS

DESCRIPTION
   Does receive and gets data from Rx debug buffer

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_do_rx_cmd_rx_burst_metrics( rfm_device_enum_type rfm_dev );


/*===========================================================================
FUNCTION FTM_GSM_DO_RX_BURST_PROCESSING

DESCRIPTION
   Does receive and gets data from Rx debug buffer

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_do_rx_burst_processing( rfm_device_enum_type rfm_dev );

/*===========================================================================
FUNCTION FTM_GSM_DO_RX_CMD_RX_CONT

DESCRIPTION
   Does receive keeps the rx circuitry on continuously
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_gsm_do_rx_cmd_rx_cont( rfm_device_enum_type rfm_dev );


/*===========================================================================
FUNCTION   FTM_GSM_DO_TX_STOP_CONT

DESCRIPTION
   Stops transmitting continuous data

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_do_tx_stop_cont( rfm_device_enum_type rfm_dev );


/*===========================================================================

FUNCTION FTM_GSM_FRAME_SWEEP_HANDLER

DESCRIPTION
   This is the main handler which is executed in every frame by ftm_tdma_isr.
   if gsm_frame_sweep_enabled flag is set for given rfm device, it processes one frame from frame sweep config.
   This function runs in tdma isr context and can only process per slot config for one frame at a time. 

     
DEPENDENCIES
      GSM Rx and Tx should be enabled before setting gsm_frame_sweep_enabled flag.

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

void ftm_gsm_frame_sweep_handler(rfm_device_enum_type rfm_dev);


/*===========================================================================
FUNCTION FTM_GSM_TX_BURST_COMMAND

DESCRIPTION
  Parameters:
  - pointer to ftm_tx_burst_type structure

  Issues the TX burst command with the specified source for TX modulation data

DEPENDENCIES
  PA LUT is supposed to be setup. SBI clock must be setup. VCTCXO is 
  assumed to have been tuned
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ftm_gsm_tx_burst_command( rfm_device_enum_type rfm_dev,
                                      const ftm_tx_burst_type* ftm_tx_burst);

/*===========================================================================
FUNCTION FTM_GSM_TX_CONT_COMMAND

DESCRIPTION
  Parameters:
  - pointer to ftm_tx_burst_type structure

  Issues the continuous TX burst command with the specified source for TX modulation data

DEPENDENCIES
  PA LUT is supposed to be setup. SBI clock must be setup. VCTCXO is 
  assumed to have been tuned
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_gsm_tx_cont_command( rfm_device_enum_type rfm_dev,
                              const ftm_tx_burst_type* ftm_tx_burst);

/*===========================================================================
FUNCTION FTM_GSM_RX_BURST_COMMAND

DESCRIPTION
  Parameters:
  - pointer to rx_burst_command_type structure

  Configures GRFC and SBI buffers and issues the RX burst command
  with the specified location for RX data

DEPENDENCIES
  SBI clock must be setup. VCTCXO is assumed to have been tuned
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_gsm_rx_burst_command( rfm_device_enum_type rfm_dev,
                               const ftm_rx_burst_type* ftm_rx_burst, 
                               uint8 num_frames_avg );




/*===========================================================================
FUNCTION FTM_GSM_SET_PA_START_DELTA

DESCRIPTION
   This function Modifies the pa start time
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   It modifies pa_start_delta used by ftm_gsm_prepare_tx_burst()

===========================================================================*/
void ftm_gsm_set_pa_start_delta( rfm_device_enum_type rfm_dev, 
                                 int16 delta );

/*===========================================================================
FUNCTION FTM_GSM_SET_PA_STOP_DELTA

DESCRIPTION
   This function Modifies the pa stop time
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   It modifies pa_start_delta used by ftm_gsm_prepare_tx_burst()

===========================================================================*/
void ftm_gsm_set_pa_stop_delta( rfm_device_enum_type rfm_dev, 
                                int16 delta );

/*===========================================================================

FUNCTION  ftm_gsm_opll_bw_cal

DESCRIPTION
  This function determines whether the current band is low or high and 
  issues the command to set the rf opll bandwidth.  
   
DEPENDENCIES
   None.
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_opll_bw_cal( rfm_device_enum_type rfm_dev,
                          int8 mapval);


/*===========================================================================

FUNCTION  ftm_set_delay_match_cfg

DESCRIPTION
  This function set the delay for envelope and IQ path
   
DEPENDENCIES
   None
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
int16 ftm_gsm_set_delay_match_cfg( rfm_device_enum_type rfm_dev,
                               int16 delay_value );



/*===========================================================================

FUNCTION  ftm_set_frame_matrix

DESCRIPTION
  This function set burst frame matrix.  Note, it only set one slot at 
  a time.  Need to call 7 times to configure all 7 slots.
   
DEPENDENCIES
   None
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_set_frame_matrix( rfm_device_enum_type rfm_dev,
                           const ftm_frame_matrix_type *bm);


#ifdef FEATURE_PMIC
/*===========================================================================

FUNCTION FTM_CALPATH_ON

DESCRIPTION
   Turns the pmic vreg for calpath off

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_calpath_on( rfm_device_enum_type rfm_dev );

/*===========================================================================

FUNCTION FTM_CALPATH_OFF

DESCRIPTION
   Turns the pmic vreg for calpath off

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_calpath_off( rfm_device_enum_type rfm_dev );
#endif


/*===========================================================================

FUNCTION   ftm_gsm_set_no_frame_avg.

DESCRIPTION
   Sets the number of frames over which RSSI is averaged.
   
DEPENDENCIES
   None

RETURN VALUE
   Number of set bits in the input byte. 

SIDE EFFECTS
   None

===========================================================================*/
extern void ftm_gsm_set_no_frame_avg ( rfm_device_enum_type rfm_dev,
                                       uint8 val );

/*===========================================================================

FFUNCTION   ftm_gsm_do_rx_calibration_prx.

DESCRIPTION
   Accepts a list of channels and a bitmask of which gain ranges to calibrate 
   for each channel. Then returns the RSSI results for each of the channel/gain
   range combinations.
   
DEPENDENCIES
   None

RETURN VALUE
   List of RSSI readings for each of the input combinations.

SIDE EFFECTS
   None

===========================================================================*/
extern void  ftm_gsm_do_rx_calibration_prx( rfm_device_enum_type rfm_dev,
                                        ftm_gsm_rxcal_parms_type * );


/*===========================================================================

FUNCTION   ftm_gsm_do_rx_calibration_prx.

DESCRIPTION
   Accepts a list of channels and a bitmask of which gain ranges to calibrate 
   for each channel. Then returns the RSSI results for each of the channel/gain
   range combinations.
   
DEPENDENCIES
   None

RETURN VALUE
   List of RSSI readings for each of the input combinations.

SIDE EFFECTS
   None

===========================================================================*/
extern void  ftm_gsm_do_rx_calibration_prx_drx( rfm_device_enum_type rfm_dev,
                                                ftm_gsm_rxcal_parms_prx_drx_type * );

/*===========================================================================

FUNCTION FTM_GSM_DO_TX_KV_CAL

DESCRIPTION
   This function will perform Kv calibration.

DEPENDENCIES
   Make sure the band and channel before call this API.

RETURN VALUE
   Kv calibration code.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_do_tx_kv_cal( rfm_device_enum_type rfm_dev,
                           ftm_gsm_generic_2x16bit_u * );

/*===========================================================================

FUNCTION FTM_GSM_DO_TX_KV_CAL_V2

DESCRIPTION
   This function will perform Kv calibration.

DEPENDENCIES
   Make sure the band and channel before call this API.

RETURN VALUE
   Kv calibration codes 

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_do_tx_kv_cal_v2( rfm_device_enum_type rfm_dev,
                              ftm_gsm_kv_cal_v2_type * );


/*===========================================================================

FUNCTION ftm_set_pa_range

DESCRIPTION
   This function will force the r0/r1 settings as required.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_gsm_set_pa_range( rfm_device_enum_type rfm_dev,
                              const ftm_gsm_pa_range_type * );

/*===========================================================================

FUNCTION ftm_gsm_set_linear_rgi

DESCRIPTION
   This function will force the rgi settings as required. It will also force edge env gain to either the 
   gsm or edge value as requested. The previous pa range setting , for the specified slot, will be maintained . 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_gsm_set_linear_rgi( rfm_device_enum_type rfm_dev,
                                    const ftm_gsm_linear_rgi_type * );


/*===========================================================================

FUNCTION ftm_gsm_set_tx_slot_config

DESCRIPTION
   This function will force the pa state, rgi settings and env_gain for a given rfm device and tx slot.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

extern void ftm_gsm_set_tx_slot_config( rfm_device_enum_type rfm_dev,
                             const ftm_gsm_tx_slot_config_type * tx_slot_config);

/*===========================================================================

FUNCTION ftm_gsm_set_multislot_rx

DESCRIPTION
   This function used to turn on multislot_rx
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
extern void ftm_gsm_set_multislot_rx( rfm_device_enum_type rfm_dev,
                                      uint8* );
/*===========================================================================

FUNCTION FTM_GSM_GET_MEAS_PWR

DESCRIPTION
   Convert ftm_rssi to dbm16 value

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

extern int16 ftm_gsm_get_rssi_dbx16( rfm_device_enum_type rfm_dev );

/*===========================================================================

FUNCTION FTM_GSM_GET_ENV_GAIN_NV

DESCRIPTION
   Returns a data structure which hold value of GSM ENV gain NV for current Tx band

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

extern void ftm_gsm_get_env_gain_nv( rfm_device_enum_type rfm_dev, ftm_gsm_env_gain_nv_data_type *data_ptr );

/*===========================================================================

FUNCTION FTM_GSM_FRAME_SWEEP_CMD_HANDLER

DESCRIPTION
   processes GSM handler cmds

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

extern boolean ftm_gsm_frame_sweep_cmd( ftm_gsm_frame_sweep_cmd_id cmd_id, uint8* cmd_buffer, uint16 length);


/*===========================================================================

FUNCTION FTM_GSM_SETUP_RX_BURST_FOR_EXPECTED_PWR

DESCRIPTION
   Sets up burst based on the expected pwr/pwr output by callbox.

DEPENDENCIES
   GSM mode and channel needs to be set

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/

extern void ftm_gsm_core_setup_rx_burst_calver( rfm_device_enum_type rfm_dev,
                                                int16* );

/*===========================================================================

FUNCTION ftm_gsm_set_freq_err

DESCRIPTION
   This function used to set the frequency error in FTM mode
 
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

extern int32 ftm_gsm_set_freq_err( rfm_device_enum_type rfm_dev,
                                   int32 user_freq_adjust_ppb, 
                                   uint32 center_frequency_hz, 
                                   uint8 enable_xo_freq_adjust);


/*===========================================================================

FUNCTION FTM_GSM_PREDIST_OVERRIDE

DESCRIPTION
    This function sets a FTM flag which tells if predistortion can be nebaled in FTM mode. 
    predist_override flag is by d efault false. if it is set TRUE, predist will not be enabled in FTM mode.
    
DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   None

===========================================================================*/
extern void ftm_gsm_set_predist_override( rfm_device_enum_type rfm_dev,
                       boolean enable_disable );


/*===========================================================================*/
extern void ftm_gsm_start_stop_iq_capture( rfm_device_enum_type rfm_dev,
                                           ftm_iqcapture_type iq_cap_type );

extern void ftm_gsm_override_gl1_freq_err( rfm_device_enum_type rfm_dev );

/*============================================================================

FUNCTION FTM_GSM_IQ_CAPTURE_GET_CONFIG

DESCRIPTION
  This API is used to get the IQ capture configuration for GSM.

DEPENDENCIES
  None

RETURN VALUE
  Always TRUE

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_gsm_iq_capture_get_config(ftm_iq_capture_get_config_type *data);

/*============================================================================

FUNCTION FTM_GSM_IQ_CAPTURE_ACQUIRE_SAMPLES

DESCRIPTION
  This API is used to Acquire samples for GSM and store the information related to them in a structure. The information include the
  device, carrier and number of samples acquired.

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE  - RF chain has not been configured

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_gsm_iq_capture_acquire_samples(rfm_device_enum_type rfm_dev, ftm_iq_capture_acquire_samples_type *data);

/*============================================================================

FUNCTION FTM_GSM_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get samples for GSM from buffer that has been used to capture samples from ACQUIRE SAMPLES. 

DEPENDENCIES
 The CAPTURE command should have been issued.

SIDE EFFECTS
 None
=============================================================================*/
void ftm_gsm_iq_capture_get_samples(rfm_device_enum_type rfm_dev, ftm_iq_capture_get_samples_type *data,ftm_rsp_pkt_type *rsp_pkt);

/*============================================================================

FUNCTION FTM_GSM_FREE_MEMORY

DESCRIPTION
  This API is used to free up the memory allocated in GSM FTM mode. This API will be generally called while exiting the mode

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_gsm_iq_capture_info_free_memory(void);

/*===========================================================================
FUNCTION ftm_gsm_set_pa_bias_nv

DESCRIPTION
  Used to set smps pdm nv items from 25084 to 25091, and pa icq bias nv
  items from 26941 to 26950. Also, update pdm and icq values stored in
  rfgsm_core_handler. 
   
DEPENDENCIES
   None.

RETURN VALUE
   ftm_gsm_error_code_type

SIDE EFFECTS
   None.

===========================================================================*/
ftm_gsm_error_code_type ftm_gsm_set_pa_bias_nv(rfm_device_enum_type rfm_dev, 
	                                                  ftm_mode_id_type band,
	                                                  uint16* smps_online, 
	                                                  uint8 num_pa_state, 
	                                                  ftm_gsm_pa_bias_params_type* pa_bias_paras);

#endif /* FTM_GSMRFPROT_H */
