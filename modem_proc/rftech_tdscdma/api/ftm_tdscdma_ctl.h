#ifndef FTM_TDSCDMA_CTL_H
#define FTM_TDSCDMA_CTL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            F T M   T D S C D M A   C T L   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM RF Commands 


Copyright (c) 2002, 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 
Copyright (c) 2008, 2009, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 
Copyright (c) 2011, 2012, 2104 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/ftm_tdscdma_ctl.h#1 $ 
  $DateTime: 2016/12/13 07:59:50 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/14   jmf     [selftest] Add support for TDSCDMA VSWR mag/phase measurement
11/03/14   jps     Updates for get PA switchpoints API
10/14/14   jps     Add support for GET_PA_SWITCHPOINTS command
10/13/14   al      Added ftm_tdscdma_set_devices to support dynamic device settings
09/26/14   jyu     Added support to calibrate different alt_path
09/21/14   jyu     Added RFcal support for DR-DSDS
09/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/05/14   jmf     [SelfTest] Port Dime Defs for VSWR result return
07/17/14   ms      Sawless RX support
06/03/14   jz      Add tuner tune code override command
04/28/14   jps     Support for TDS port switching
11/31/13   ych     Added an API for fast calibration for TxAGC override 
09/10/13   aka     Added FTM interface for Self test cmds 
09/06/13   ars     Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
07/24/13   gvn     Change name of stg parameter for RSB cal
07/22/13   kb      [XPT] Updated APT IQ capture API to return sampling rate.
07/08/13   ms      Support for program_qfe_scenario
06/19/13   kb      [XPT] Added support for Tx waveform switching.
06/06/12   jps     Clean up FTM RF data structures on exit mode
06/04/13   jps     Clean up FTM RF Mode data structures during exit mode
06/03/13   nsh	   Support Get Multi Synth State in Dime
05/25/13   qzh     Porting TDSCDMA IQ capture from NIKEL to DIME
05/13/13   kb      [XPT] Added xpt_iq_capture() API
05/06/13   kb      [XPT] Added xpt_cal_config() API
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
03/22/13   zg      Added support for RSB calibration
03/21/13   ms      Tuner TDET support
03/07/13   zg      Added support for IQ capture
01/31/13   ms      Added extern variables to support ftm_tdscdma_meas
07/18/12   jps     Add FTM error handling for RF mode commands 
7/18/12    jps     Compiler warning fix 
07/16/12   jps     Initial FTM burst TX/RX framework
03/26/12   hy     TD-SCDMA Cal time optimization
11/11/11   hy      Added TDSCDMA V2/V3 Cal, Tx DAC Cal prototype    
08-24-11   hy     Rx Cal support.
05-19-11   hy     Created 
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "ftmdiag.h"
#include "ftm.h"
#include "ftm_common.h"
#include "rf_tdscdma_mdsp_types.h"
#include "rfm_device_types.h"
#include "rfcom.h"
#include "rf_tdscdma_mc.h"
#include "rf_tdscdma.h"
#include "rfcommon_core_xpt_buffer.h"
#include "ftm_rf_cmd.h"
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Specifies TDSCDMA calibration state for RF
*/
typedef enum
{
  FTM_TDSCDMA_TX_CAL_LEGACY = 0,
  FTM_TDSCDMA_TX_CAL_SELF,
  FTM_TDSCDMA_TX_CAL_NUM
} ftm_tdscdma_tx_cal_type;

typedef enum
{
  FTM_TDSCDMA_CAL_STATE_INACTIVE = 0,
  FTM_TDSCDMA_CAL_STATE_RX,
  FTM_TDSCDMA_CAL_STATE_TX
} ftm_tdscdma_cal_state_type;

typedef enum
{
  FTM_TDSCDMA_BURST_TX_OFF = 0,
  FTM_TDSCDMA_BURST_TX_ON,
  FTM_TDSCDMA_BURST_TX_INVALID
} ftm_tdscdma_burst_tx_state_type;

typedef enum
{
  FTM_TDSCDMA_BURST_RX_OFF = 0,
  FTM_TDSCDMA_BURST_RX_ON,
  FTM_TDSCDMA_BURST_RX_INVALID
} ftm_tdscdma_burst_rx_state_type;

typedef enum
{
  FTM_TDSCDMA_IQ_CAPTURE_NO_RSP = 0,
  FTM_TDSCDMA_IQ_CAPTURE_FAIL,
  FTM_TDSCDMA_IQ_CAPTURE_SUCCEED
} ftm_tdscdma_iqcapture_status_type;

typedef enum
{
  FTM_TDSCDMA_TX_WAVEFORM_SPECIAL_BURST = 0,
  FTM_TDSCDMA_TX_WAVEFORM_DPD,
  FTM_TDSCDMA_TX_WAVEFORM_INVALID
} ftm_tdscdma_tx_waveform_type;

typedef struct {
  uint32* startAddrPrx;        /* Start address of IQ capture buffer in shared memory. */
  uint32* startAddrDrx;
  boolean OverrideStg;           /* Using STG or an external signal */
  boolean PrintData;           /* Print data from F3 ? */
  uint32 IqCaptureOutputDelay; /* delay (us) used by the f3 debugging message. 40000*/
  ftm_tdscdma_iqcapture_status_type IqCaptureStatus;      
                               /* flag to indicate what FW's feedback looks like */                            
  uint16 MaxWaitCnt;           /* Max wait time for FW response with 1ms resolution */
  uint32 SampleSize;           /* how many IQ pairs are collected */
} ftm_tdscdma_iq_capture_data_type;

/* *************************************************************/
/* ***************FTM IQ capture structures *****************/
/* *************************************************************/
// This is the strcuture used to store the information regarding acquired IQ samples.
typedef struct
{

 rfm_device_enum_type device_to_aquire; // The device for which IQ samples have been acquired.

 rfm_device_enum_type device_with_IQ; // The device for which IQ samples have been got from FW.

 uint16 num_req_samples; // The number of samples requested to be acquired.

 uint16 *iq_buf; // The buffer which stores TDSCDMA IQ samples that are captured. Each element I(16-bit) Q(16-bit)

 boolean data_available; // Flag to indicate that data capture has been sucessful.
 
 boolean data_avail_from_FW; // Flag to indicate that if data capture from FW has been sucessful.

}ftm_tdscdma_iq_acquired_samples_data_type;

ftm_mode_id_type ftm_tdscdma_util_convert_band_rfcom_to_ftm(rfcom_tdscdma_band_type rfcom_band);
rfcom_tdscdma_band_type ftm_tdscdma_util_convert_band_ftm_to_rfcom(ftm_mode_id_type ftm_band);
boolean rfm_ftm_apply_xpt_tx_agc_override( rfm_mode_enum_type rfm_mode, rfm_device_enum_type device, rfcom_band_type_u rfm_band);

extern rf_tdscdma_mdsp_tfw_state_t ftm_tdscdma_current_fw_state;
extern ftm_tdscdma_cal_state_type ftm_tdscdma_currrent_cal_state;
extern rfcom_tdscdma_band_type ftm_tdscdma_current_mode;

extern rfm_device_enum_type ftm_tdscdma_current_pri_device;
extern rfm_device_enum_type ftm_tdscdma_current_sec_device;
extern rfm_device_enum_type ftm_tdscdma_current_tx_device;
extern uint8                ftm_tdscdma_current_pri_alt_path;
extern uint8                ftm_tdscdma_current_sec_alt_path;
extern boolean              ftm_tdscdma_override_alt_path;


extern ftm_lm_buf_type ftm_tdscdma_rxlm_buffer[2];
extern uint8 ftm_tdscdma_rx_lin_state;

/* *************************************************************/
/* ***************FTM FBRx IQ capture macros*****************/
/* *************************************************************/

// The TDSCDMA FBRx IQ capture structure which stores all the relevent data when acquiring samples.

/*! Maximum FBRx TDSCDMA IQ Samples that can be captured  */
#define FTM_TDSCDMA_FBRX_IQ_SAMPLES_MAX 9216

/*! FBRx TDSCDMA Sample server IQ format - Tx*/
#define FTM_TDSCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_TX FTM_IQ_8_BIT_FORMAT 

/*! FBRx TDSCDMA Sample server IQ format - Rx*/
#define FTM_TDSCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_RX FTM_IQ_32_BIT_FORMAT

/*! TDSCDMA FBRx IQ capture sampling frequency */
#define FTM_TDSCDMA_FBRX_IQ_SAMPLE_FREQ (10.24e6)

/*! This is the maximum number of TDSCDMA FBRx IQ samples that can be sent through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
#define FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)
#define FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX ((FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)/4)

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure templet for Measure Tx params payload.

  @details
*/
typedef struct
{
  int16 tx_power;        //Tx Power in dB10 units 
  uint16 evm_value;      //EVM in % * 100 units
  int16 aclr_value[6];   // Array of ACLR values 
  int16 vswr_ratio_dB10; //value of VSWR in dB10

  int16 vswr_v2_return_loss;
  /*!< value of VSWR Return Loss in  dB100 */

  int16 vswr_v2_mag;
  /*!< value of VSWR Magnitude in dB100*/

  int16 vswr_v2_phase;
  /*!< value of VSWR phase in degree100 */

  int16 vswr_v2_fwd_pwr;
  /*!< Value of forward power in dbm100 */

  int16 vswr_v2_rev_pwr;
  /*!< Value of reverse power in dbm100 */

} ftm_rf_tdscdma_payload_tx_params_type;

// This is the strcuture used to store the information regarding acquired IQ samples.
typedef struct
{
   rfm_device_enum_type device; 
   /*!< The device for which IQ samples have been acquired */

   uint16 req_samples;          
   /*!< The number of Tx and Rx samples requested to be acquired */

   int32 *iq_buf_tx;            
   /*!< The buffer which stores TDSCDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   int32 *iq_buf_rx;         
   /*!< The buffer which stores TDSCDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   int32 *iq_buf_tx_rsp_data;
   /*!< The buffer which stores TDSCDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   int32 *iq_buf_rx_rsp_data;         
   /*!< The buffer which stores TDSCDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   uint16 tx_iq_data_siz_bytes;
   /*!< The buffer which stores TDSCDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   uint16 rx_iq_data_siz_bytes;
   /*!< The buffer which stores TDSCDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   boolean data_available;      
   /*!< Flag to indicate that data capture has been successful */

   uint32 sampling_frequency;
   /*!< Sampling frequency that was used to acquire IQ samples */

   ftm_rf_tdscdma_payload_tx_params_type meas_results;
}ftm_rf_tdscdma_fbrx_iq_acquired_samples_data_type;

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_PDM

DESCRIPTION
    This function takes a pdm value and writes it to the specified PDM.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS

===========================================================================*/
void ftm_tdscdma_set_pdm(ftm_pdm_id_type pdm_id, int2 val);


/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_TXAGC
DESCRIPTION
    This function sets LUT index, paRange and IQ gain for a given TxAGC

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_tdscdma_set_txagc(int16 txAgc, boolean paRange_override, uint8 paRange );


/*===========================================================================

FUNCTION FTM_TDSCDMA_TXAGC_SWEEP

DESCRIPTION
    This function does a power sweep based on the configured txagc values.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_tdscdma_txagc_sweep(int16 start, int16 stop, int16 step, uint16 delay,
                                           boolean parange_override, uint8 parange);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_CW

DESCRIPTION
   This function will force the interleaver to generate a Continuous Wave.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_tdscdma_set_cw(boolean on_off);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_OFFSET_CW_TONE

DESCRIPTION
   This function will produce an offset Continuous Wave in TDSCDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_tdscdma_set_offset_cw(int32 freq_offset);


/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_MODE

DESCRIPTION
    This function will switch modes to TDSCDMA.  

DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   When a mode is changed WHILE transmitting, this function will
   automatically shut down the transmit for the old mode and enter the
   transmit state for the new mode.

===========================================================================*/
void ftm_tdscdma_set_mode(ftm_mode_id_type mode);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_LNA_OFFSET

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_set_lna_offset(ftm_receive_chain_handle_type rx_chain, byte index, int2 setting);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_LNA_RANGE

DESCRIPTION
  This function sets the LNA range 0 (highest gain) to 3 (lowest gain). 

  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_set_lna_range(ftm_receive_chain_handle_type rx_chain, byte ftm_lna_range ,byte ftm_rx_lin_state);

/*===========================================================================
FUNCTION  FTM_TDSCDMA_SET_PA_RANGE        

DESCRIPTION
  This function sets PA range 0 (low gain) or 1 (high gain).

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_pa_range(byte paRange);

/*===========================================================================

FUNCTION FTM_TDSCDMA_GET_LNA_OFFSET

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_tdscdma_get_lna_offset(ftm_receive_chain_handle_type rx_chain, byte index, word expected_agc_val ,byte rx_lin_state);

/*===========================================================================

FUNCTION FTM_TDSCDMA_GET_RX_AGC

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_tdscdma_get_rx_agc(ftm_receive_chain_handle_type rx_chain);


/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_RX

DESCRIPTION
   This function will set the RX state for TDSCDMA.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Tx will be turned off.

===========================================================================*/
void ftm_tdscdma_set_rx(boolean state);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_TX

DESCRIPTION
   This function will set the TX state for TDSCDMA.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Rx will be turned off.

===========================================================================*/
void ftm_tdscdma_set_tx(boolean state);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_TX_CONTINOUS

DESCRIPTION
   This function will set the TX state to continuous for TDSCDMA.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Rx would be turned off.//hyang todo: revise.

===========================================================================*/
void ftm_tdscdma_set_tx_continuous(boolean state);


/*===========================================================================

FUNCTION FTM_UPLINK_SWEEP_CAL

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_uplink_sweep_cal (boolean enable );

/*===========================================================================

FUNCTION       FTM_TDSCDMA_SET_HDET_TRACKING

DESCRIPTION
  Enables and disables hdet tracking.  If state is non-zero, the hdet
  tracking loop is turned on, and controlled to the value specified
  in set_point.  If state is zero, hdet tracking is disabled.
===========================================================================*/
void ftm_tdscdma_set_hdet_tracking( boolean state, unsigned short set_point );

/*===========================================================================

FUNCTION       FTM_HDET_TRACKING_CB

DESCRIPTION
  1.25ms callback function for closed-loop tracking of HDET.
  Adjusts TX_AGC_ADJ PDM to control HDET to the desired set point.
===========================================================================*/
void ftm_tdscdma_hdet_tracking_cb( int4 interval );

/*===========================================================================

FUNCTION ftm_tdscdma_configure_tx_sweep_cal

DESCRIPTION
  This function allows the user to set the length of each step in the tx cal 
  sweep and the number of HDET readings averaged per step.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_configure_tx_sweep_cal(uint16 interval, byte num_avg);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_PA

DESCRIPTION
   This function will set the pa on/off.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_set_pa(boolean state);

/*===========================================================================
FUNCTION  FTM_TDSCDMA_MODULATOR_TIMING      

DESCRIPTION   Configurate modulator timing in mDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_modulator_timing(void);

/*===========================================================================
FUNCTION   FTM_UPLINK_DPCCH_CONFIG

DESCRIPTION  Configurate DPCCH in mDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_uplink_dpcch_config(boolean enable);

/*===========================================================================
FUNCTION   FTM_CONFIG_MODULATOR 

DESCRIPTION     Configurate uplink modulator

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_config_modulator(void);
#ifdef FEATURE_TXLM
/*===========================================================================
FUNCTION    FTM_SEND_TXLM_CFG_CMD

DESCRIPTION     Send TxLM cfg cmd

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_send_txlm_cfg_cmd(uint32 buf_idx, boolean enable); 
#endif
/*===========================================================================
FUNCTION   FTM_VALIDATE_BETA_ED_SF2

DESCRIPTION     Validate Beta Ed SF2

DEPENDENCIES

RETURN VALUE   
  boolean
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean ftm_tdscdma_validate_beta_ed_sf2(int8 ded, uint8 bed2);


/*===========================================================================
FUNCTION   FTM_CONFIG_EUL_WAVEFORM

DESCRIPTION     Configure modulator for enhanced uplink 

DEPENDENCIES

RETURN VALUE
  MPR value for the configured waveform

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 ftm_tdscdma_configure_eul_waveform(uint8 bc, uint8 bd, int8 dhs, int8 dec, int8 ded, uint8 bed2, uint8 mu,
                                           uint8 num_dpdch, boolean hsdpcch_en, uint8 modulation_scheme );

/*===========================================================================
FUNCTION   FTM_SET_BETA_GAIN_REGISTER 

DESCRIPTION     Configure beta gain registers in MDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_set_beta_gain_register(uint16 bc_hw, uint16 bd_hw, uint16 bhs_hw);

/*===========================================================================
FUNCTION   FTM_SET_EUL_CTL_REGISTER 

DESCRIPTION     Configure EUL Control registes in ARM and MDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_set_eul_ctl_register(uint16 bec_hw, uint8 num_dpdch);

/*===========================================================================
FUNCTION   FTM_SET_EUL_DATA_REGISTER 

DESCRIPTION     Configure EUL Data registes in ARM and MDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_set_eul_data_register(uint16 bed_hw, uint16 bed2_hw, uint8 mu, uint8 num_dpdch, boolean hsdpcch_en, uint8 mod);

/*===========================================================================
FUNCTION   FTM_ENABLE_EUL_WAVEFORM 

DESCRIPTION     Enables the Configured EUL Waveform

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_enable_eul_waveform(void);

/*===========================================================================
FUNCTION   FTM_DISABLE_EUL_WAVEFORM 

DESCRIPTION     Disable the Configured EUL Waveform

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_tdscdma_disable_eul_waveform(void);


/*===========================================================================

FUNCTION FTM_TDSCDMA_DO_TX_RX_FREQ_CAL_SWEEP

DESCRIPTION
  This function configures and starts the TX/RX frequency calibration sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  Error code.

SIDE EFFECTS
  None

===========================================================================*/
uint16 ftm_tdscdma_do_tx_rx_freq_cal_sweep(uint16 rx_chain,
                                   uint8  num_freqs, 
                                   uint8  num_steps_per_freq, 
                                   uint16 step_length, 
                                   uint16 *freq_list_ptr, 
                                   uint16 *pdm_list_ptr, 
                                   int16  *pri_exp_agc_val_ptr,
                                   uint8  *pa_range_list_ptr, 
                                   uint8  *read_hdet_ptr, 
                                   uint8  *rx_action_ptr,
                                   uint8  *rx_lpm_list_ptr,
                                   uint8  rx_lin_state);


/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_SECONDARY_CHAIN

DESCRIPTION
  Enable and disable secondary chain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_set_secondary_chain( boolean on_off );


/*===========================================================================

FUNCTION FTM_SECOND_CHAIN_TEST_CALL

DESCRIPTION
  Enable will set the RxD controller NV to 6 to use ANT1 for primary reception
  Disable will set the RxD controller NV to 0 to use ANT0 for primary reception

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_second_chain_test_call( uint8 on_off );

/*===========================================================================

FUNCTION FTM_SET_TX_SWEEP_STEP_SIZE

DESCRIPTION
  This function allows the user to set the PDM step size for the TX cal sweep.
  The PDM will be decreased by the given number of counts for each step in
  the sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_set_tx_sweep_step_size(uint8 step);


/*===========================================================================

FUNCTION FTM_DO_DC_CAL

DESCRIPTION
  This function will calibrate the value DC value for use during compressed
  mode.
  
DEPENDENCIES
  Set band and channel must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  Writes calibration results to NV.

===========================================================================*/
void ftm_tdscdma_do_dc_cal(void);

/*===========================================================================

FUNCTION FTM_TDSCDMA_DO_TX_DAC_CAL

DESCRIPTION
    This function will complete the TX DAC cal on TDSCDMA.    

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
boolean ftm_tdscdma_do_tx_dac_cal(void);

/*===========================================================================

FUNCTION FTM_TDSCDMA_TOGGLE_MODES

DESCRIPTION
  This function will switch from TDSCDMA to GSM mode and back, or vice-versa.
  This triggers the updating of rf driver internal variables.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_toggle_modes( void );

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_TX_SWEEP_STEP_SIZE

DESCRIPTION
  This function allows the user to set the PDM step size for the TX cal sweep.
  The PDM will be decreased by the given number of counts for each step in
  the sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ftm_tdscdma_set_tx_sweep_step_size(uint8 step);

/*===========================================================================

FUNCTION FTM_TDSCDMA_TUNE_TO_CHAN

DESCRIPTION
  This function tunes the radio to a given uplink channel number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ftm_tdscdma_tune_to_chan(ftm_receive_chain_handle_type rx_chain, uint16 chan);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_SMPS_POUT_TABLE_TYPE

DESCRIPTION
  This function tunes the radio to a given uplink channel number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ftm_tdscdma_set_smps_pout_table_type(tx_pout_data_type set_tbl_type);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_SMPS_PA_BIAS_OVERRIDE

DESCRIPTION
    This function overrides the SMPS PA Bias tables and whenever PDM
    is set (in FTM mode only), the overriden value will take effect. 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_tdscdma_set_smps_pa_bias_override(boolean val);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_SMPS_PA_BIAS_VAL

DESCRIPTION
    This function sets the SMPS PA Bias Value in FTM mode only.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_tdscdma_set_smps_pa_bias_val(uint16 val);

/*===========================================================================

FUNCTION FTM_TDSCDMA_ALLOCATE_RXLM

DESCRIPTION
    This function will request rxlm buffers FTM TDSCDMA for the given chain

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
void ftm_tdscdma_allocate_rxlm(ftm_receive_chain_handle_type chain);

/*===========================================================================

FUNCTION FTM_TDSCDMA_DEALLOCATE_RXLM

DESCRIPTION
    This function will deallocate rxlm buffers FTM TDSCDMA has requested

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
void ftm_tdscdma_deallocate_rxlm(void);

/*===========================================================================
FUNCTION FTM_TDSCDMA_ALLOCATE_TXLM

DESCRIPTION
    This function will request txlm buffers FTM TDSCDMA 

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
void ftm_tdscdma_allocate_txlm(void);

/*===========================================================================

FUNCTION FTM_TDSCDMA_DEALLOCATE_TXLM

DESCRIPTION
    This function will deallocate txlm buffers FTM TDSCDMA has requested

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
void ftm_tdscdma_deallocate_txlm(void);


/*===========================================================================
  @brief
  Enter TDSCDMA Mode

  @details
  Configure the RF to TDSCDMA mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
===========================================================================*/
boolean ftm_tdscdma_mode_enter(rfm_device_enum_type device);

/*===========================================================================
  @brief
  Exit TDSCDMA Mode for FTM

  @details
  This function is used to clean-up procedure for TDSCDMA.

  @param device RF Device for which FTM will exit TDSCDMA

  @return
  TRUE on success and FALSE on failure
===========================================================================*/
boolean ftm_tdscdma_mode_exit(rfm_device_enum_type device);

/*============================================================================

FUNCTION FTM_TDSCDMA_GET_RX_LEVEL_DBM
 
DESCRIPTION
  Function to get Rx Power Level. Returns the Rx level, for a specified receiver, in 
  both Rx AGC and dBm10 units.

DEPENDENCIES
  None
 
RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS 
=============================================================================*/ 
boolean ftm_tdscdma_get_rx_level_dbm(rfcom_device_enum_type device, int16 *rx_level_agc, int16 *rx_level_dbm10);

/*============================================================================

FUNCTION FTM_TDSCDMA_GET_MULTIPLE_RX_LEVEL_DBM
 
DESCRIPTION
  This API returns the Rx level, for all the requested receiver chains associated
  with one carrier, in both Rx AGC and dBm units. This command is only valid after
  the mode (Technology/Band) is set using FTM_SET_MODE and FTM_SET_CALIBRATION_STATE
  has been used to deactivate calibration state or calibration state has not been 
  activated while the device was powered up. Also, its required to enable all 
  receiver chains and program them to correct LNA state  before requesting RxAGC 
  for a Rx chain (e.g. secondary chain should be enabled using FTM_SET_SECODNARY_CHAIN 
  before using this command).

DEPENDENCIES
  None
 
RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS 
=============================================================================*/ 
boolean ftm_tdscdma_get_multiple_rx_level_dbm( rfcom_device_enum_type device, uint16 select_rx_chain, ftm_rsp_pkt_type *rsp_pkt );

/*============================================================================

FUNCTION FTM_TDSCDMA_DO_FREQ_ERROR_COMP

DESCRIPTION 
  This API is used for applying a Tx frequency error compensation value.
  Frequency error is in Hz. The amount of frequency correction applied is either
  the user_freq_adjust_ppb or the combination of user_freq_adjust_ppb + the 
  frequency error estimate from XO manager. This selection is based on the input 
  value of enable_xo_freq_adjust.
 
DEPENDENCIES
  None

RETURN VALUE
  int32 - Total Frequency adjust in Hz
 
SIDE EFFECTS

=============================================================================*/
int32 ftm_tdscdma_set_freq_adjust(int32 user_freq_adjust_ppb, uint32 center_frequency_hz, uint8 enable_xo_freq_adjust);

boolean ftm_tdscdma_set_burst_tx_params(uint8 num_slots, uint8 mod_type, int16 ts1_pwrlvl,int16 ts2_pwrlvl,int16 ts3_pwrlvl,int16 ts4_pwrlvl,int16 ts5_pwrlvl);

boolean ftm_tdscdma_burst_tx(boolean on_off);

boolean ftm_tdscdma_burst_rx(int16 exp_rx_lvl, boolean on_off);

void ftm_tdscdma_get_lna_range(int16 exp_rx_lvl);

void ftm_tdscdma_cmd_get_iq_data_from_fw(rfm_device_enum_type device);

void ftm_tdscdma_store_iq_data(uint32 start_address, uint32 start_address_div, uint32 num_samples);

boolean ftm_tdscdma_rf_mode_tx_cmd_handler(void);

boolean ftm_tdscdma_rf_mode_rx_cmd_handler(void);

/*===========================================================================
FUNCTION ftm_tdscdma_rf_mode_blocking_cmd_handler

DESCRIPTION
   This function used to handle all the blocking cmd under FTM-RF

DEPENDENCIES
   None.

RETURN VALUE
   Boolean, FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS if cmd return successful, 
   FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE if the cmd returns failure, or even
   not return until the timeout threshold
   

SIDE EFFECTS
   None.

===========================================================================*/
boolean ftm_tdscdma_rf_mode_blocking_cmd_handler(rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg);

void ftm_tdscdma_rf_mode_temp_comp_handler(void);

void ftm_tdscdma_do_iq_capture (uint16 channel);

void ftm_tdscdma_config_tx_selfcal(boolean enable);

boolean ftm_tdscdma_do_rsb_cal(ftm_rsb_txlo_rsp_type *cal_params, boolean override_stg); 

boolean ftm_tdscdma_xpt_cal_config(rfm_device_enum_type device, rfm_device_enum_type device_fb,uint8 xpt_mode, boolean config);

boolean ftm_tdscdma_xpt_override_txagc( rfm_device_enum_type device,
                                        uint8  xpt_mode,
                                        uint8  amam_bank_idx,
                                        uint16 amam_start_offset,
                                        uint8  amam_tbl_size,
                                        uint8  ampm_bank_idx,
                                        uint16 ampm_start_offset,
                                        uint8  ampm_tbl_size,
                                        uint8  epdt_bank_idx,
                                        uint16 epdt_start_offset,
                                        uint8  epdt_tbl_size,
                                        uint8  iq_gain_action_type,
                                        uint32 iq_gain,
                                        uint8  env_scale_action_type,
                                        uint32 env_scale,
                                        uint16 rgi,
                                        uint16 smps_bias,
                                        uint8  pa_state,
                                        uint16 pa_curr,
                                        uint8  delay_action_type,
                                        int32  delay);

boolean ftm_tdscdma_xpt_process_override_txagc( rfm_device_enum_type device,
                                        uint8  xpt_mode,
                                        uint8  amam_bank_idx,
                                        uint16 amam_start_offset,
                                        uint8  amam_tbl_size,
                                        uint8  ampm_bank_idx,
                                        uint16 ampm_start_offset,
                                        uint8  ampm_tbl_size,
                                        uint8  epdt_bank_idx,
                                        uint16 epdt_start_offset,
                                        uint8  epdt_tbl_size,
                                        uint8  iq_gain_action_type,
                                        uint32 iq_gain,
                                        uint8  env_scale_action_type,
                                        uint32 env_scale,
                                        uint16 rgi,
                                        uint16 smps_bias,
                                        uint8  pa_state,
                                        uint16 pa_curr,
                                        uint8  delay_action_type,
                                        int32  delay);

boolean ftm_tdscdma_xpt_override_rf_settings(ftm_xpt_txagc_override_type* params);

boolean 
ftm_tdscdma_xpt_iq_capture
(
   uint8 device,
   uint8 device_fb,
   uint8 xpt_capture_type,
   uint16 num_samples,
   boolean first_trig,
   boolean last_trig,
   rfcommon_xpt_processing_type do_processing,
   rf_common_xpt_buffer_id_type buffer_id,
   uint32 * samp_rate
);

/*============================================================================

FUNCTION FTM_TDSCDMA_IQ_CAPTURE_GET_CONFIG

DESCRIPTION
    This API is used to get the IQ capture configuration for TDSCDMA.

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_tdscdma_iq_capture_get_config(ftm_iq_capture_get_config_type *data);

/*============================================================================

FUNCTION FTM_TDSCDMA_IQ_CAPTURE_ACQUIRE_SAMPLES

DESCRIPTION
  This API is used to Acquire samples for TDSCDMA and store the information related to them in a structure. The information include the
  device, carrier and number of samples acquired.

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE,  - RF chain has not been configured

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_tdscdma_iq_capture_acquire_samples(rfcom_device_enum_type device,ftm_iq_capture_acquire_samples_type *data);

/*============================================================================

FUNCTION FTM_TDSCDMA_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get samples for TDSCDMA from buffer that has been used to capture samples from ACQUIRE SAMPLES. 

DEPENDENCIES
 The CAPTURE command should have been issued.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_tdscdma_iq_capture_get_samples(rfcom_device_enum_type device,ftm_iq_capture_get_samples_type *data,ftm_rsp_pkt_type *rsp_pkt_ptr);


/*============================================================================

FUNCTION FTM_TDSCDMA_FREE_MEMORY

DESCRIPTION
  This API is used to free up the memory allocated in TDSCDMA FTM mode. This API will be generally called while exiting the mode

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_tdscdma_free_memory(void);
/*===========================================================================
FUNCTION ftm_tdscdma_get_multi_synth_handler

DESCRIPTION
   This function is used to get the multi synth status for Tx and Rx Pll
 
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_get_multi_synth_handler(void);



/*===========================================================================
FUNCTION ftm_tdscdma_reset_multi_synth_state

DESCRIPTION
   This function used to reset the multi synth status for Tx and Rx Pll
 
DEPENDENCIES
   None.

RETURN VALUE
   Return TRUE if both Tx and Rx pll been set to UNDERTERMINED

SIDE EFFECTS
   None.

===========================================================================*/
boolean ftm_tdscdma_reset_multi_synth_state(void);


/*===========================================================================
FUNCTION ftm_tdscdma_init_multi_synth_state

DESCRIPTION
   This function is used to initialize the multi synth status for Tx and Rx Pll to DISABLED or 
   turn it back to INVALID
 
DEPENDENCIES
   None.

RETURN VALUE
   Return TRUE if both Tx and Rx pll been set to UNDERTERMINED

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_set_multi_synth_state(boolean on_off);

/*===========================================================================
FUNCTION ftm_tdscdma_rf_mode_get_multi_synth_state

DESCRIPTION
   This function is used to send speficif cmd to get both Tx and Rx PLL status from FW
 
DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_rf_mode_get_multi_synth_state(uint8 * rx_pll_status, uint8 * tx_pll_status);



/*===========================================================================
FUNCTION ftm_tdscdma_init_multi_synth_state

DESCRIPTION
   This function is used to initialize the multi synth status for Tx and Rx Pll to DISABLED or 
   turn it back to INVALID
 
DEPENDENCIES
   None.

RETURN VALUE
   Return TRUE if both Tx and Rx pll been set to UNDERTERMINED

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_set_multi_synth_state(boolean on_off);

void ftm_tdscdma_rfmode_cleanup(void);

/*===========================================================================
FUNCTION FTM_TDSCDMA_SWITCH_TX_WAVEFORM

DESCRIPTION
  This function switches the waveform between Special burst and DPD waveform.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_switch_tx_waveform(byte tx_waveform);

#ifdef FEATURE_RF_HAS_QTUNER
/*============================================================================

FUNCTION 
ftm_tdscdma_program_qfe_scenario

DESCRIPTION
This API will be called to program the tuner immediately and no script or FW 
 action is required. Inside this API check all the active paths then give a call to
 tuner manager associated with appropriate tuners

DEPENDENCIES
  None

RETURN VALUE
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)

SIDE EFFECTS
  None
=============================================================================*/

boolean ftm_tdscdma_program_qfe_scenario(void);

#endif /* FEATURE_RF_HAS_QTUNER */

/*============================================================================

FUNCTION FTM_RF_MEAS_TX_PARAMS

DESCRIPTION
  This API is used to capture and process Tx IQ samples using FBRx and to calculate
  various Tx parameters (i.e. Tx Power, EVM and ACLR)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_tdscdma_fbrx_meas_tx_params
(
   rfcom_device_enum_type device,
   uint16 num_averages,
   uint32 capture_offset,
   uint32 tx_measurement_config,
   ftm_rsp_pkt_type *rsp_pkt
);

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_TRIGGER

DESCRIPTION
  This command acquires one set of IQ samples and returns key parameters used for
  collecting Rx and TX IQ samples. The samples collected in on acquisition will be
  contiguous. Sampels from different acquisitions are not time coherent.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_tdscdma_fbrx_iq_capture_trigger
(
   rfcom_device_enum_type device,
   uint8 sample_size,
   uint8 fbrx_chain,
   ftm_rsp_pkt_type *rsp_pkt_ptr
);

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get Tx and/or Rx IQ samples for TDSCDMA from buffer that has been used to capture samples from ACQUIRE SAMPLES. 

DEPENDENCIES
 FTM_FBRX_IQ_CAPTURE_TRIGGER command should have been issued before executing get sample command.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_tdscdma_fbrx_iq_capture_get_samples
(
   rfcom_device_enum_type device,
   uint8 sample_source,
   uint32 sample_offset,
   uint32 sample_size,
   ftm_rsp_pkt_type *rsp_pkt
);

uint16 ftm_tdscdma_trigger_asm_update(ftm_tdscdma_port_state_type port);

void ftm_tdscdma_set_rx_lin_state(uint8 rx_lin_state);

/*============================================================================
FUNCTION ftm_tdscdma_set_tuner_tune_code_override

DESCRIPTION 
  This API is used override the current Antenna tuner tune code settings 
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
=============================================================================*/
ftm_tuner_override_status_type ftm_tdscdma_set_tuner_tune_code_override
( 
  rfcom_device_enum_type device,
  uint8 override_flag,
  void *data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
);

/*============================================================================
FUNCTION ftm_tdscdma_set_alt_path

DESCRIPTION 
  This API is used override the alt_path
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
=============================================================================*/
void ftm_tdscdma_set_alt_path
( 
  ftm_tdscdma_alt_path_oride_params_type oride_param
);

/*============================================================================
FUNCTION ftm_tdscdma_set_devices

DESCRIPTION 
  This API sets pri, sec and tx devices
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
=============================================================================*/
void ftm_tdscdma_set_devices(void *ftm_req_data);

/*============================================================================
FUNCTION ftm_tdscdma_get_pa_switchpoints

DESCRIPTION 
  This API is used extract the PA switchpoints for a given band
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
=============================================================================*/
boolean ftm_tdscdma_get_pa_switchpoints(rfm_device_enum_type tx_device, rfcom_tdscdma_band_type band, int16* pa_rise_swpts, int16* pa_fall_swpts, int16* active_pa_states);

/*! @} */

#endif  // FEATURE_FACTORY_TESTMODE
#endif  // FTM_TDSCDMA_CTL_H
