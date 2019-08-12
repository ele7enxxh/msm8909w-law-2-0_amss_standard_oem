#ifndef FTM_WCDMA_CTL_H
#define FTM_WCDMA_CTL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            F T M   W C D M A   C T L   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM RF Commands 


Copyright (c) 2002, 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2012 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/ftm_wcdma_ctl.h#1 $ 
  $DateTime: 2016/12/13 07:59:54 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/15   ljz     Enable NBEE serial calibration for W
03/15/15   gh      Added API ftm_wcdma_execute_temp_comp_cb
02/03/15   xsh     Add ftm_wcdma_disable_NBEE API definition
01/29/15   rmb     Execute WCDMA temp comp every 10 seconds.  
12/18/14   ars/rmb Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
11/24/14   jmf     [selftest] Add support for WCDMA VSWR mag/phase measurement
11/14/14   vbh     Add api to get current FTM RF scenario
11/14/14   jmf     [selftest] WCDMA FBRx Noise floor calibration support
10/09/14   vbh     Add device as aparam for set_mode, remove unused ftm_wcdma_do_rsb_cal, ftm_wcdma_toggle_modes apis
09/23/14   kai     Add WCDMA DRDS Cal support
09/23/14   vbh     Use of rfm_device_enum_type instead of rfcom_device_enum_type
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/22/14   vbh     Updated the argument list for set_pa and set_tx apis. 
08/05/14   jmf     [SelfTest] Port Dime Defs for VSWR result return
12/05/13  ska     Graceful exit if FTM_WCDMA_SET_MODE is issued when the phone is not in FTM mode
06/27/14   ac      change to enable NBEE cal 
06/17/14   sma     Fixed RSB cal issue when there is no diversity
04/20/14   bn      WCDMA 8 PA state support and related cleanup
04/24/14   vbh     Added tune routine which would call rfm_wcdma apis for tune
03/25/14   rmb     Add a new get rxagc API during X2W measurements. 
02/13/14   ac      api to test the scripts for events
02/05/13   dw      Add on_off argument in tm_wcdma_set_offset_cw()
02/04/14   jmf     Adding xpt_mode as arg for ftm_wcdma_xpt_cal_config
12/18/13   zhh     add a check in set waveform command to check for RxTX state
11/25/13   kai     Remove legacy API ftm_wcdma_load_dpd_tables
                   Add support for RFLM-based Tx AGC override
09/06/13   ars     Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
09/10/13   aa      Enable/Disable NBEE RXCAL on the fly
08/27/13   dj      Added ftm_wcdma_feedback_setup to support tx_internal_cal
                   and self test
08/27/13   aka     Added support for WCDMA self-test commands
08/02/13   kai     Added api ftm_configure_eul_waveform_by_subtest
07/24/13   gvn     Change name of stg parameter for RSB cal
07/15/12   ars     Added FTM interface for self-test APIs
06/28/13   vs      Add WCDMA support for FTM_PROGRAM_QFE_SCENARIO
05/21/13   rmb     Added prototype of APIs to get MULTI SYNTH STATE.
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/09/13   jl      Port IQ capture code
05/01/13   jmf     Implement xPT IQ Capture SWI Change
04/29/13   jj      Add function to convert rfcom wcdma band to ftm band
04/22/13   jmf     Implement xPT processing Enum Type
04/18/13   kai     Changed Tx agc override api
04/05/13   rmb     Added FTM APIs for WCDMA Sleep, Wakeup and Pretune to chan.
03/27/13   kai     Added xpt cal config/deconfig
03/14/13   tks     Added extra parameter to the rsb cal function prototype
03/13/13   kai     Added ftm_wcdma_xpt_cal_config 
02/26/13   tks     Modified rsb calibration function to return correct status 
02/12/13   jmf     Modify definition of IQ sample capture fn to take more params
02/07/13   ndb/vb  Added rfwcdma_mc_set_tuner_tune_code_override() implementation support 
01/30/13   ka      Updated FTM interfaces to logical device
01/28/13   kai     Removed ftm_wcdma_set_sapt_state
11/15/12   ka      Initial support for DCHSUPA
11/14/12   dw      Add support to set PA quiescent current
11/09/12   kai     Added XPT TXAGC Override
10/24/12   bn      Enable W sample capture for XPT
07/31/12   jfc     Rename "ftm_common_calibration_v3_xpt.h" to "ftm_calibration_v3_xpt.h"
07/16/12   Saul    WCDMA. Added APIs to set LTE PA state and RGI.
07/10/12   Saul    WCDMA. Added tune_with_lte_settings API.
06/14/12   anr     Removed EPT specific FTM functionality from this file. 
06/05/12   anr     Initial EPT Support
05/16/12   vb      Do DAC cal for a device path only if a tech is enabled for it 
04/23/12   ars     Modifed FTM frequency adjust logic to apply correction using XO manager API
03/10/12   ks      Hookup Cal v3 ET Enable/Disable actions to device
03/07/12   cri     Added initial DPD support 
03/06/12   kai     Added API for setting/getting Tx datapath IQ gain 
02/14/12   swb     Added API for FTM Tx Frequency Adjustment 
01/26/12   gh      Update deallocate_rxlm to take in Rx chain as arguement
12/21/11   dw      Legacy code cleanup
12/20/11   ars     Fix to return return RxAGC in dBm10 instead of dB for GET_RX_LEVEL_DBM 
11/03/11   swb     WCDMA Non-signaling Power Control Override 
10/27/11   gh      Add API to indicate to rf_task ftm_wcdma_enter/exit complete 
10/26/11   dw      Support for closed loop AGC in FTM
10/17/11   ars     Added FTM_SET_TX_POWER_DBM 
10/17/11   ars     Added FTM_SET_PA_STATE and FTM_SET_TX_POWER_DBM FTM Layer 
09/08/11   vb      Support for DAC Cal
08/24/11    gh     Move CalV3 implementations out to separate file
08/04/11   vb      Added FTM common Internal device calibration support 
07/27/11   aro     Updated config/retune radio interface to have return
07/20/11   aro     Added Rx Measurement OpCode
07/20/11   aro     Renamed Cal v3 Opcode/payload type names for consistency
07/13/11    gh     Add support for CalV3
07/11/11    vb     Prototyped ftm_wcdma_tune_to_multi_chan()
06/23/11    dw     Merge SC/DC DVGA offset funcitons
05/17/11   vb      RxLM support for XO Cal
05/12/11    dw     Add enable argument in DPCCH cfg cmd and TxLM cfg cmd
04/26/11    dw     CMI compliance changes
01/19/11   dw      Initial support for RxLM
10/29/10   ka      APT updates 
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
01/11/10   kguo    Merged mdm9k dev branch to main\latest
12/08/09    bn     Separate Tx and Rx PLL for current savings
11/11/09   ka      Support for dual carrier.
10/23/09   kguo    Merged changes for 9k bringup 
01/18/08   ad      Removed unused function ftm_rf_get_im2()
08/08/07   jfc     Mainlined T_MSM6280 feature
10/18/06   ycl     Added ftm_wcdma_set_tx_sweep_step_size() prototype.
10/17/06   ycl     Modifications for initial MSM7600 build.
04/19/06   ka      Updated Tx-Rx API for diversity/lpm changes.
04/12/06   bn      Added ftm_set_secondary_chain and ftm_second_chain_test_call prototype
04/07/06   lcl     Added DC calibration for interBand CM.
03-09-06   ka      Added support for tx-rx vs freq calibration.
03-06-06   rsr     Added prototype to fix compile error.
03-06-06   dp/bhas Add WCDMA diversity support.
02-07-06   ka      Added ftm_do_internal_device_calibration().
08-20-04   ka      Added command to configure tx sweep cal.
08-28-02   rjr     Created 
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmdiag.h"
#include "ftm.h"
#include "ftm_common.h"
#include "ftm_common_control.h"
#include "ftm_calibration_v3_xpt.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rfcommon_core_xpt.h"
#include "ftm_rf_cmd.h"

#include "rflm_api_fbrx.h"
#include "lm_types.h"

extern boolean rfcommon_fbrx_msm_update_static_settings( lm_handle_type fbrx_lm_handle, 
                                                        rfcommon_fbrx_tx_cfg_type cfg );

/* *************************************************************/
/* ***************FTM IQ capture structures and macros*****************/
/* *************************************************************/

// The WCDMA IQ capture structure which stores all the relevent data when acquiring samples.

/*! Maximum WCDMA IQ Samples that can be captured  */
#define FTM_WCDMA_IQ_MAX_SAMPLES 8192

/*! WCDMA Sample server IQ format */
#define FTM_WCDMA_IQ_SS_DATA_FORMAT FTM_IQ_8_BIT_FORMAT 

/*! CHIPX2 WCDMA frequency */
#define FTM_WCDMA_IQ_SAMPLE_FREQ_CHIPX2 (3.84e6*2)

/*! This is the maximum number of WCDMA IQ samples that can be sent through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
/*! Through experimentation it is found to be 512 bytes: 256 samples  I (1 byte) + Q(1 byte)  */
#define FTM_WCDMA_IQ_DIAG_MAX_SAMPLES (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)

/* *************************************************************/
/* ***************FTM FBRx IQ capture macros*****************/
/* *************************************************************/

// The WCDMA FBRx IQ capture structure which stores all the relevent data when acquiring samples.

/*! Maximum FBRx WCDMA IQ Samples that can be captured  */
#define FTM_WCDMA_FBRX_IQ_SAMPLES_MAX 9216

/*! FBRx WCDMA Sample server IQ format - Tx*/
#define FTM_WCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_TX FTM_IQ_8_BIT_FORMAT 

/*! FBRx WCDMA Sample server IQ format - Rx*/
#define FTM_WCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_RX FTM_IQ_32_BIT_FORMAT

/*! WCDMA FBRx IQ capture sampling frequency */
#define FTM_WCDMA_FBRX_IQ_SAMPLE_FREQ (30.72e6)

/*! This is the maximum number of WCDMA FBRx IQ samples that can be sent through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
#define FTM_WCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)
#define FTM_WCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX ((FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)/4)

// This is the strcuture used to store the information regarding acquired IQ samples.
typedef struct
{

 rfm_device_enum_type device; // The device for which IQ samples have been acquired.

 uint8 carrier; // The carrier for which IQ samples have been acquired.

 uint16 req_samples; // The number of samples requested to be acquired.

 uint16 *iq_buf; // The buffer which stores WCDMA IQ samples that are captured. Each element I(8-bit) Q(8-bit)

 boolean data_available; // Flag to indicate that data capture has been sucessful.

}ftm_wcdma_iq_acquired_samples_data_type;

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
  
} ftm_rf_wcdma_payload_tx_params_type;

// This is the strcuture used to store the information regarding acquired IQ samples.
typedef struct
{
   rfm_device_enum_type device; 
   /*!< The device for which IQ samples have been acquired */

   uint16 req_samples;          
   /*!< The number of Tx and Rx samples requested to be acquired */

   int32 *iq_buf_tx;            
   /*!< The buffer which stores WCDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   int32 *iq_buf_rx;         
   /*!< The buffer which stores WCDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   int32 *iq_buf_tx_rsp_data;
   /*!< The buffer which stores WCDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   int32 *iq_buf_rx_rsp_data;         
   /*!< The buffer which stores WCDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   uint16 tx_iq_data_siz_bytes;
   /*!< The buffer which stores WCDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   uint16 rx_iq_data_siz_bytes;
   /*!< The buffer which stores WCDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   boolean data_available;      
   /*!< Flag to indicate that data capture has been successful */

   uint32 sampling_frequency;
   /*!< Sampling frequency that was used to acquire IQ samples */

   ftm_rf_wcdma_payload_tx_params_type meas_results;

   ftm_rf_fbrx_noise_binsum_type noise_meas_results;
   /*!< Structure to contain self-test fbrx noise cal results */

}ftm_rf_fbrx_iq_acquired_samples_data_type;

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

FUNCTION FTM_WCMDA_SET_DVGA_GAIN_OFFSET                        

DESCRIPTION
  This function sets the CDMA Rx VAGC offset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_dvga_gain_offset(rfm_device_enum_type device, int2 dvgaGainOffset);
/*===========================================================================

FUNCTION    FTM_WCDMA_GET_DVGA_GAIN_OFFSET

DESCRIPTION
  This function returns the WCDMA Rx DVGA gain offset.

DEPENDENCIES
  None

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_dvga_gain_offset(rfm_device_enum_type device, word expected_agc_val);
/*===========================================================================

FUNCTION FTM_WCDMA_SET_PDM

DESCRIPTION
    This function takes a pdm value and writes it to the specified PDM.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS

===========================================================================*/
void ftm_wcdma_set_pdm(ftm_pdm_id_type pdm_id, int2 val);

/*===========================================================================

FUNCTION FTM_WCDMA_TPC_CONTROL

DESCRIPTION
    This function will force all up or all down bits for the slots in the tpc_pattern
    parameter.

    Override flag is (tpcOrideFlag)
    Boolean, 0=no override TPC, 1= override TPC
    It is a 32-bit register (tpcOrideDecHist)
    Format is: 
         Bit address
    Bits 0:1 = slot 0
    Bits 2:3 = slot 1
    etc
    Bits 28:29 = slot 14
         Format of data value is
    0=none, 1=up, 2 or 3 = down
         Write
    Down 0x3FFFFFFF
    Up 0x15555555 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_wcdma_tpc_control(boolean enable, uint32 tpc_pattern);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_CW

DESCRIPTION
   This function will force the interleaver to generate a Continuous Wave.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ftm_rsp_pkt_type ftm_wcdma_set_cw(boolean on_off);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_OFFSET_CW_TONE

DESCRIPTION
   This function will produce an offset Continuous Wave in WCDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_wcdma_set_offset_cw(int32 freq_offset, boolean on_off);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_MODE

DESCRIPTION
    This function will switch modes to WCDMA.  

DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   When a mode is changed WHILE transmitting, this function will
   automatically shut down the transmit for the old mode and enter the
   transmit state for the new mode.

===========================================================================*/
ftm_rsp_pkt_type ftm_wcdma_set_mode(rfm_device_enum_type device, ftm_mode_id_type mode);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_LNA_OFFSET

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_lna_offset(rfm_device_enum_type device, byte index, int2 setting);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_LNA_RANGE

DESCRIPTION
  This function sets the LNA range 0 (highest gain) to 3 (lowest gain). 

  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_lna_range(rfm_device_enum_type device, byte ftm_lna_range );

/*===========================================================================
FUNCTION  FTM_WCDMA_SET_PA_RANGE        

DESCRIPTION
  This function sets PA range 0 (low gain) or 1 (high gain).

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_wcdma_set_pa_range(byte paRange);

/*===========================================================================

FUNCTION FTM_WCDMA_GET_LNA_OFFSET

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_lna_offset(rfm_device_enum_type device, byte index, word expected_agc_val);

/*===========================================================================

FUNCTION FTM_WCDMA_GET_LNA_OFFSET_MEAS_NBPWR

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int2 ftm_wcdma_get_lna_offset_meas_nbpwr(rfm_device_enum_type device, byte index, word expected_agc_val);

/*===========================================================================
FUNCTION FTM_WCDMA_SET_TX

DESCRIPTION
   This function will set the TX state for WCDMA.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_wcdma_set_tx(rfm_device_enum_type device, boolean state);

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
void ftm_uplink_sweep_cal (boolean enable );

/*===========================================================================

FUNCTION       FTM_WCDMA_SET_HDET_TRACKING

DESCRIPTION
  Enables and disables hdet tracking.  If state is non-zero, the hdet
  tracking loop is turned on, and controlled to the value specified
  in set_point.  If state is zero, hdet tracking is disabled.
===========================================================================*/
void ftm_wcdma_set_hdet_tracking( boolean state, unsigned short set_point );

/*===========================================================================

FUNCTION       FTM_HDET_TRACKING_CB

DESCRIPTION
  1.25ms callback function for closed-loop tracking of HDET.
  Adjusts TX_AGC_ADJ PDM to control HDET to the desired set point.
===========================================================================*/
void ftm_hdet_tracking_cb( int4 interval );

/*===========================================================================

FUNCTION ftm_wcdma_configure_tx_sweep_cal

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
void ftm_wcdma_configure_tx_sweep_cal(uint16 interval, byte num_avg);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_PA

DESCRIPTION
   This function will set the pa on/off.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_wcdma_set_pa(rfm_device_enum_type device, boolean state);

/*===========================================================================
FUNCTION  FTM_WCDMA_MODULATOR_TIMING      

DESCRIPTION   Configurate modulator timing in mDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_wcdma_modulator_timing(void);

/*===========================================================================
FUNCTION   FTM_UPLINK_DPCCH_CONFIG

DESCRIPTION  Configurate DPCCH in mDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_uplink_dpcch_config(boolean enable);

/*===========================================================================
FUNCTION   FTM_CONFIG_MODULATOR 

DESCRIPTION     Configurate uplink modulator

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_config_modulator(void);
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
extern void ftm_send_txlm_cfg_cmd(uint32 buf_idx, boolean enable); 
#endif

/*===========================================================================
FUNCTION    FTM_SEND_TX_DAC_CAL_CMD

DESCRIPTION     Send Tx DAC Cal cmd to MDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean ftm_send_tx_dac_cal_cmd(boolean ant0, boolean ant1);

/*===========================================================================
FUNCTION   FTM_VALIDATE_BETA_ED_SF2

DESCRIPTION     Validate Beta Ed SF2

DEPENDENCIES

RETURN VALUE   
  boolean
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean ftm_validate_beta_ed_sf2(int8 ded, uint8 bed2);


/*===========================================================================
FUNCTION   FTM_CONFIG_EUL_WAVEFORM

DESCRIPTION     Configure modulator for enhanced uplink 

DEPENDENCIES

RETURN VALUE
  MPR value for the configured waveform

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 ftm_configure_eul_waveform(uint8 bc, uint8 bd, int8 dhs, int8 dec, int8 ded, uint8 bed2, uint8 mu,
                                           uint8 num_dpdch, boolean hsdpcch_en, uint8 modulation_scheme );

/*===========================================================================
FUNCTION   FTM_CONFIG_MODULATOR_BY_SUBTEST

DESCRIPTION  Configure modulator for enhanced uplink according subtest type

DEPENDENCIES

RETURN VALUE   
  MPR value for the configured waveform.  
  MPR value of 0xFF is returned for invalid configuration

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 ftm_configure_eul_waveform_by_subtest(uint16 subtest);

/*===========================================================================
FUNCTION   FTM_SET_BETA_GAIN_REGISTER 

DESCRIPTION     Configure beta gain registers in MDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_set_beta_gain_register(uint16 bc_hw, uint16 bd_hw, uint16 bhs_hw);

/*===========================================================================
FUNCTION   FTM_SET_EUL_CTL_REGISTER 

DESCRIPTION     Configure EUL Control registes in ARM and MDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_set_eul_ctl_register(uint16 bec_hw, uint8 num_dpdch);


/*===========================================================================
FUNCTION   FTM_FIND_MPR_FROM_LUT

DESCRIPTION  This function takes the values that need to be searched from LUT having EUL waveforms and returns the 
                      MPR backoff value 

DEPENDENCIES Table waveform_table should be arranged in montonically increased value of mu

RETURN VALUE   
  uint8,
  the mpr back off value is entry is present
  0xFF is entry is not present
  
SIDE EFFECTS
  None
===========================================================================*/

extern uint8 ftm_wcdma_eul_get_mpr(uint8 bc, uint8 bd, int8 dhs, int8 dec, int8 ded, uint8 bed2, uint8 mu,
                                                uint8 num_dpdch, boolean hsdpcch_en);



/*===========================================================================
FUNCTION   FTM_SET_EUL_DATA_REGISTER 

DESCRIPTION     Configure EUL Data registes in ARM and MDSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_set_eul_data_register(uint16 bed_hw, uint16 bed2_hw, uint8 mu, uint8 num_dpdch, boolean hsdpcch_en, uint8 mod);

/*===========================================================================
FUNCTION   FTM_ENABLE_EUL_WAVEFORM 

DESCRIPTION     Enables the Configured EUL Waveform

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_enable_eul_waveform(void);

/*===========================================================================
FUNCTION   FTM_DISABLE_EUL_WAVEFORM 

DESCRIPTION     Disable the Configured EUL Waveform

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ftm_disable_eul_waveform(void);

/*===========================================================================
FUNCTION    FTM_WAIT_TIL_DSP_READY

DESCRIPTION   Wait until DSP is ready

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_wait_til_dsp_ready(void);

#ifdef FTM_HAS_COMMON_COMMANDS

/*===========================================================================

FUNCTION FTM_WCDMA_DO_TX_RX_FREQ_CAL_SWEEP

DESCRIPTION
  This function configures and starts the TX/RX frequency calibration sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  Error code.

SIDE EFFECTS
  None

===========================================================================*/
uint16 ftm_wcdma_do_tx_rx_freq_cal_sweep(uint16 rx_chain,
                                   uint8  num_freqs, 
                                   uint8  num_steps_per_freq, 
                                   uint16 step_length, 
                                   uint16 *freq_list_ptr, 
                                   uint16 *pdm_list_ptr, 
                                   int16  *pri_exp_agc_val_ptr,
                                   uint8  *pa_range_list_ptr, 
                                   uint8  *read_hdet_ptr, 
                                   uint8  *rx_action_ptr,
                                   uint8  *rx_lpm_list_ptr);
#endif /* FTM_HAS_COMMON_COMMANDS */

#ifdef FEATURE_WCDMA_RX_DIVERSITY
/*===========================================================================

FUNCTION FTM_WCDMA_SET_SECONDARY_CHAIN

DESCRIPTION
  Enable and disable secondary chain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_set_secondary_chain(rfcom_device_enum_type device, 
                                   boolean on_off );


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
void ftm_second_chain_test_call( uint8 on_off );
#endif /* FTM_HAS_WCDMA_RX_DIVERSITY */

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
void ftm_set_tx_sweep_step_size(uint8 step);


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
void ftm_do_dc_cal(void);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_TX_SWEEP_STEP_SIZE

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
extern void ftm_wcdma_set_tx_sweep_step_size(uint8 step);

/*===========================================================================

FUNCTION FTM_WCDMA_TUNE_TO_CHAN

DESCRIPTION
  This function tunes the radio to a given uplink channel number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ftm_wcdma_tune_to_chan(rfm_device_enum_type device, uint16 chan);


/*===========================================================================

FUNCTION FTM_WCDMA_TUNE_TO_MULTI_CHAN

DESCRIPTION
  This function tunes the radio to a given uplink channel number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_tune_to_multi_chan(rfm_device_enum_type device, uint16 chan[]);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_SMPS_PA_BIAS_OVERRIDE

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
void ftm_wcdma_set_smps_pa_bias_override(boolean val);

/*===========================================================================

FUNCTION FTM_WCDMA_SET_SMPS_PA_BIAS_VAL

DESCRIPTION
    This function sets the SMPS PA Bias Value in FTM mode only.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_wcdma_set_smps_pa_bias_val(uint16 val);
/*===========================================================================

FUNCTION FTM_WCDMA_SET_PA_CURRENT

DESCRIPTION
    This function set PA quiescent current

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_wcdma_set_pa_current(uint8 val);
/*===========================================================================

FUNCTION FTM_WCDMA_ALLOCATE_RXLM

DESCRIPTION
    This function will request rxlm buffers FTM WCDMA for the given chain

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
void ftm_wcdma_allocate_rxlm(ftm_receive_chain_handle_type chain);

/*===========================================================================

FUNCTION FTM_WCDMA_DEALLOCATE_RXLM

DESCRIPTION
    This function will deallocate rxlm buffers FTM WCDMA has requested
    for a given chain

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
void ftm_wcdma_deallocate_rxlm(ftm_receive_chain_handle_type chain);

/*===========================================================================
FUNCTION FTM_WCDMA_ALLOCATE_TXLM

DESCRIPTION
    This function will request txlm buffers FTM WCDMA 

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
void ftm_wcdma_allocate_txlm(void);

/*===========================================================================

FUNCTION FTM_WCDMA_DEALLOCATE_TXLM

DESCRIPTION
    This function will deallocate txlm buffers FTM WCDMA has requested

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
void ftm_wcdma_deallocate_txlm(void);

/*===========================================================================

FUNCTION FTM_WCDMA_CONFIGURE_XO_CAL_STATE

DESCRIPTION
  This function performs wcdma DC calibration.  The calibrated value will be
  used during WCDMA inter-freq/inter-band.

DEPENDENCIES
  Made sure
    - WCDMA mode is set
    - Channel is tuned
    - LNA State is set

RETURN VALUE
  None

SIDE EFFECTS
  Writes calibration results to NV.

===========================================================================*/
void ftm_wcdma_enable_xo_cal_state( ftm_receive_chain_handle_type chain, 
                                    uint32 xo_cal_buff_idx );


/*===========================================================================

FUNCTION    FTM_WCDMA_GET_DVGA_GAIN_OFFSET_SETUP

DESCRIPTION
  This functions sets up Rx for measuring DVGA gain offset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_get_dvga_gain_offset_setup(rfm_device_enum_type device);

/*===========================================================================

FUNCTION    FTM_WCDMA_GET_DVGA_GAIN_OFFSET_MEAS

DESCRIPTION
  This function returns the WCDMA Rx DVGA gain offset.
  1. Injecting Rx power at expected_agc_val (convert it to dBm).
  2. ftm_wcdma_get_dvga_gain_offset_meas() will automatically calculate the DVGA gain offset.

DEPENDENCIES
  ftm_wcdma_get_dvga_gain_offset_setup must have been called

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_dvga_gain_offset_meas(rfm_device_enum_type device, word expected_agc_val);

/*===========================================================================

FUNCTION FTM_WCDMA_GET_LNA_OFFSET_SETUP

DESCRIPTION
  Set up Rx for measuring LNA offset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_wcdma_get_lna_offset_setup(rfm_device_enum_type device, byte index);

/*===========================================================================

FUNCTION FTM_WCDMA_GET_LNA_OFFSET_MEAS

DESCRIPTION
  Get Rx for LNA offset

DEPENDENCIES
  ftm_wcdma_get_lna_offset_setup must have been called

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_wcdma_get_lna_offset_meas(rfm_device_enum_type device, byte index, word expected_agc_val);



/*===========================================================================

FUNCTION FTM_WCDMA_DO_TX_DAC_CAL

DESCRIPTION
    This function will complete the TX DAC cal on WCDMA

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   

===========================================================================*/
ftm_common_dac_cal_rsp_type 
ftm_wcdma_do_tx_dac_cal 
( 
  rfm_device_enum_type device 
);

/*============================================================================

FUNCTION FTM_WCDMA_SET_TX_POWER_DBM

DESCRIPTION
  This command programs the PA range, LUT index, and Digital Gain to reach the programmed
  Tx power level with power limiting enabled
 
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS

=============================================================================*/
boolean ftm_wcdma_set_tx_power_dbm(uint8 enable, int16 power_level);

/*============================================================================

FUNCTION FTM_WCDMA_GET_RX_LEVEL_DBM
 
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
boolean ftm_wcdma_get_rx_level_dbm(rfm_device_enum_type device, int16 *rx_level_agc, int16 *rx_level_dbm10);

/*============================================================================

FUNCTION FTM_WCDMA_GET_RX_LEVEL_DBM_X2W_MEAS

DESCRIPTION
  Function to get Rx Power Level in X2W measurement. Returns the Rx level, for a specified receiver, in
  both Rx AGC and dBm10 units.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS
=============================================================================*/
boolean ftm_wcdma_get_rx_level_dbm_xtow_meas(rfm_device_enum_type device, uint16 neigh_channel, int16 *rx_level_agc, int16 *rx_level_dbm10, boolean bypass_rxagcon);

/*============================================================================

FUNCTION FTM_WCDMA_SET_PA_STATE

DESCRIPTION
  This command programs the PA state. PA State is the low level hardware control indicator, which is
  different from the PA range software abstraction
 
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE - Success
  Returns FALSE - Error Condition

SIDE EFFECTS

=============================================================================*/
boolean ftm_wcdma_set_pa_state(uint8 pa_state);

/*============================================================================

FUNCTION FTM_WCDMA_NOTIFY_RF_TASK

DESCRIPTION 
  This API is used for notifying rf_task that ftm_wcdma_enter/exit had completed
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS

=============================================================================*/
void ftm_wcdma_notify_rf_task(void);

/*============================================================================

FUNCTION FTM_WCDMA_DO_FREQ_ERROR_COMP

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
int32 ftm_wcdma_set_freq_adjust(int32 user_freq_adjust_ppb, uint32 center_frequency_hz, uint8 enable_xo_freq_adjust);

/*============================================================================

FUNCTION FTM_WCDMA_SET_IQGAIN_VALUE

DESCRIPTION 
  This API is used for setting IQ gain value 
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS

=============================================================================*/
void ftm_wcdma_set_iqgain_value(int16 iqGain);

/*============================================================================

FUNCTION FTM_WCDMA_GET_IQGAIN_VALUE

DESCRIPTION 
  This API is used for getting IQ gain value 
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS

=============================================================================*/
int16 ftm_wcdma_get_iqgain_value(void);

/*----------------------------------------------------------------------------*/
boolean ftm_wcdma_xpt_cal_config
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  uint8 xpt_mode,
  boolean config
);

/*----------------------------------------------------------------------------*/
boolean ftm_wcdma_xpt_iq_capture
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

/*----------------------------------------------------------------------------*/
boolean ftm_wcdma_xpt_override_rf_settings
(
  ftm_xpt_txagc_override_type* params
);

/*----------------------------------------------------------------------------*/
boolean ftm_wcdma_xpt_override_txagc
( 
  rfm_device_enum_type device,
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
  int32  delay
);

boolean
ftm_wcdma_xpt_set_txagc_override_params
( 
  rfm_device_enum_type device,
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
  int32  env_delay_val
);

/*============================================================================

FUNCTION FTM_WCDMA_SET_ET_PATH

DESCRIPTION 
  This API is used to setup the ET Path. Currently code looks similar across
  technologies but is subject to change later
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS

=============================================================================*/
void ftm_wcdma_set_et_path(uint8 pa_state, boolean enable_et_path);

/*============================================================================

FUNCTION FTM_WCDMA_IQ_CAPTURE_GET_CONFIG

DESCRIPTION
    This API is used to get the IQ capture configuration for WCDMA.

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_wcdma_iq_capture_get_config(ftm_iq_capture_get_config_type *data);

/*============================================================================

FUNCTION FTM_WCDMA_IQ_CAPTURE_ACQUIRE_SAMPLES

DESCRIPTION
  This API is used to Acquire samples for WCDMA and store the information related to them in a structure. The information include the
  device, carrier and number of samples acquired.

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE,  - RF chain has not been configured

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_wcdma_iq_capture_acquire_samples(rfm_device_enum_type device,ftm_iq_capture_acquire_samples_type *data);

/*============================================================================

FUNCTION FTM_WCDMA_IQ_CAPTURE_ANALYZE_SAMPLES

DESCRIPTION
  This API has two operations
  1) Acquire IQ samples 
  2) Analyze samples and call function to compute sensitivity

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE,  - RF chain has not been configured

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_wcdma_iq_capture_analyze_samples(rfm_device_enum_type device,ftm_iq_capture_analyze_samples_type *data, ftm_rsp_pkt_type *rsp_pkt);


/*============================================================================

FUNCTION FTM_WCDMA_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get samples for WCDMA from buffer that has been used to capture samples from ACQUIRE SAMPLES. 

DEPENDENCIES
 The CAPTURE command should have been issued.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_wcdma_iq_capture_get_samples(rfcom_device_enum_type device,ftm_iq_capture_get_samples_type *data,ftm_rsp_pkt_type *rsp_pkt_ptr);

/*===========================================================================
FUNCTION FTM_WCDMA_IQ_CAPTURE_ANALYZE_SAMPLES_RSP_DIAG_PKT
/*!
  @brief This function builds the DIAG response packet for
         FTM_RESET_MULTI_SYNTH_STATE

  @param rsp_pkt_ptr pointer to the DIAG response packet
  @param status the status of the response

  @return None
*/
/*=========================================================================*/

void ftm_wcdma_iq_capture_analyze_samples_rsp_diag_pkt(ftm_rsp_pkt_type *rsp_pkt_ptr, ftm_iq_capture_error_code_type status,diagpkt_subsys_cmd_code_type cmd_id,uint32 analysis_id,uint16 size,int8 *analysis_result);

/*============================================================================

FUNCTION FTM_WCDMA_FREE_MEMORY

DESCRIPTION
  This API is used to free up the memory allocated in WCDMA FTM mode. This API will be generally called while exiting the mode

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_wcdma_free_memory(void);


/*============================================================================
FUNCTION ftm_wcdma_set_tuner_tune_code_override

DESCRIPTION 
   This API is used override the current Antenna tuner tune code settings 
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS

=============================================================================*/
ftm_tuner_override_status_type ftm_wcdma_set_tuner_tune_code_override( rfm_device_enum_type device,uint8 override_flag,void *data,uint8* tuner_nv_ptr,uint8 *tuner_id_ptr);

/*----------------------------------------------------------------------------*/
boolean
ftm_wcdma_tune_with_lte_settings
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  uint32 tx_chan
);

/*----------------------------------------------------------------------------*/
boolean
ftm_wcdma_set_lte_pa_state
(
  rfm_device_enum_type device,
  uint8 pa_state
);

/*----------------------------------------------------------------------------*/
boolean
ftm_wcdma_set_lte_rgi
(
  rfm_device_enum_type device,
  uint16 rgi
);

/*----------------------------------------------------------------------------*/
void ftm_wcdma_set_tx_multi_carrier(rfm_device_enum_type device,
                                    uint16 state, 
                                    uint32 multi_carrier_idx);

/*----------------------------------------------------------------------------*/
void ftm_wcdma_set_tx_multi_carrier_gains(uint16 gain0, uint16 gain1);

/*----------------------------------------------------------------------------*/
rfcom_wcdma_band_type ftm_wcdma_convert_phone_mode_to_rfcom_band_type
(
  ftm_mode_id_type phone_mode
);

/*----------------------------------------------------------------------------*/
rfcom_wcdma_band_type ftm_wcdma_convert_ftm_band_to_rfcom_band_type
(
  ftm_rf_mode_type ftm_band
);

/*----------------------------------------------------------------------------*/
ftm_rf_mode_type ftm_wcdma_convert_rfcom_band_type_to_ftm_band
(
   rfcom_wcdma_band_type rfcom_band
);

void ftm_wcdma_sleep(rfm_device_enum_type device);

boolean ftm_wcdma_enable_scripts(rfm_device_enum_type device, int8 event_id);

void ftm_wcdma_wakeup(rfm_device_enum_type device);

void ftm_wcdma_pretune_to_chan(rfm_device_enum_type device, uint16 chan[]);

/*============================================================================

FUNCTION FTM_WCDMA_GET_LATEST_MULTISYNTH_STATUS

DESCRIPTION
  This API is used for get latest Mutlsynth status by quering RFM layer
  which inturn will query device layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

void ftm_wcdma_get_latest_multisynth_status
(
  rfcom_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock
);
/*============================================================================

FUNCTION FTM_WCDMA_UPDATE_AGGREATED_MULTISYNTH_STATUS

DESCRIPTION
  This API is used for get aggregated Mutlsynth status by quering the latest
  SYNTH lock status and latching on to UNLOCKED status if
  unlock it detected in any of the queries.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

void ftm_wcdma_update_aggregated_multisynth_status(rfm_device_enum_type device);

/*============================================================================

FUNCTION FTM_WCDMA_RESET_MULTISYNTH_STATUS

DESCRIPTION
  This API is used to get reset multisynth lock status.
  This is the only way get the multisynth status out of UNLOCK status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/


boolean ftm_wcdma_reset_multisynth_status(rfm_device_enum_type device);
/*============================================================================

FUNCTION FTM_WCDMA_GET_MULTISYNTH_STATUS

DESCRIPTION
  This API is used to get aggregate multisynth lock value.
  It also does a latest query before returning the value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

void ftm_wcdma_get_multisynth_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock
);

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

void ftm_wcdma_fbrx_meas_tx_params
(
   rfm_device_enum_type device,
   uint16 num_averages,
   uint32 capture_offset,
   uint32 tx_measurement_config,
   ftm_rsp_pkt_type *rsp_pkt
);

/*============================================================================

FUNCTION FTM_RF_MEAS_FBRX_NOISE

DESCRIPTION
  This API is used to setup very low Tx power for given WCDMA band and bandwidth,
  perform FBRx IQ capture, calculate noise binsum powers for ACLR correction
  and process/write corresponding NV items

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_wcdma_fbrx_meas_noise_params
(
   ftm_mode_id_type curr_tech_band,
   rfcom_device_enum_type device,
   uint16 num_averages,
   uint16 tx_chan,
   ftm_rsp_pkt_type *rsp_pkt
);


/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_TRIGGER

DESCRIPTION
  This command acquires one set of IQ samples and returns key parameters used for
  collecting Rx and TX IQ samples. The samples collected in one acquisition will be
  contiguous. Samples from different acquisitions are not time coherent.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_wcdma_fbrx_iq_capture_trigger
(
   rfm_device_enum_type device,
   uint8 sample_size,
   uint8 fbrx_chain,
   ftm_rsp_pkt_type *rsp_pkt
);

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get Tx and/or Rx IQ samples for WCDMA from buffer that has
  been used to capture samples in ftm_wcdma_fbrx_iq_capture_trigger().

DEPENDENCIES
 FTM_FBRX_IQ_CAPTURE_TRIGGER command should have been issued before executing
 get samples command.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_wcdma_fbrx_iq_capture_get_samples
(
   rfm_device_enum_type device,
   uint8 sample_source,
   uint32 sample_offset,
   uint32 sample_size,
   ftm_rsp_pkt_type *rsp_pkt
);

/*============================================================================

FUNCTION ftm_wcdma_get_multiple_rx_level_dbm

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
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_wcdma_get_multiple_rx_level_dbm
(
   rfm_device_enum_type device,
   uint16 select_rx_chain,
   ftm_rsp_pkt_type *rsp_pkt
);

/*============================================================================

FUNCTION 
ftm_wcdma_feedback_setup

DESCRIPTION
This API will be called to configure fbrx for tx internal calibration and 
self test 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void ftm_wcdma_feedback_setup( rfm_device_enum_type primary_device,
                              rfm_device_enum_type feedback_device,
                              uint16 feedback_setup_action,
                              uint8 fb_gain_state,
                              boolean wtr_enable_mode,
                              boolean xpt_rxlm_enable,
                              boolean fb_rxlm_enable );


ftm_rsp_pkt_type ftm_wcdma_set_rxcal_optimization(void  *ftm_req_data);

void ftm_wcdma_enable_NBEE(void);

void ftm_wcdma_disable_NBEE(void);

boolean ftm_wcdma_rxcal_opt_enabled(void);

#ifdef FEATURE_RF_HAS_QTUNER
/*============================================================================

FUNCTION 
ftm_wcdma_program_qfe_scenario

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

void ftm_wcdma_program_qfe_scenario(void);

#endif /* FEATURE_RF_HAS_QTUNER */

/*============================================================================

FUNCTION 
ftm_wcdma_tune_to_chan_scripts

DESCRIPTION
This API will be called to call rfm_wcdma tune routines prep and exec tune. 
This routine should be the only access point for rfm_wcdma apis for tuning. 			

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/


void ftm_wcdma_tune_to_chan_scripts(uint8 device_mask,
                                    uint16 chan[RFCOM_MAX_CARRIERS], 
                                    uint32 carrier_mask);

boolean ftm_wcdma_get_pa_switchpoints
(
  uint8 carrier_idx,
  rfcom_wcdma_band_type rf_band,
  int16* pa_rise_swpts,
  int16* pa_fall_swpts,
  int16 *active_pa_states
);

uint32 ftm_wcdma_get_current_test_scenario();

void ftm_wcdma_execute_temp_comp(void);

void ftm_wcdma_execute_temp_comp_cb(rfm_cb_event_enum_type temp_comp_status, void* user_data);

#ifdef __cplusplus
}
#endif

#endif  // FEATURE_FACTORY_TESTMODE
#endif  // FTM_WCDMA_CTL_H
