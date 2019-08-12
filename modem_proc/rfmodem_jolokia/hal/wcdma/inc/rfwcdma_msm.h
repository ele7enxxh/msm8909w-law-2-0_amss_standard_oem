#ifndef RFWCDMA_MSM_H
#define RFWCDMA_MSM_H
/*!
  @file
  rfwcdma_msm.h

  @brief
  Provides WCDMA MSM interface functions.

  @details
  The function interface is defined in this file. This encompases functions
  for all the MSMs.

*/

/*=============================================================================

Copyright (c) 2009 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/hal/wcdma/inc/rfwcdma_msm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/20/15   ag      Dynamic Notch filter Control in WCDMA for better TP at good Ior and better sensitivity at weak signal
02/25/15   ag      Reverting Dynamic Notch filter Control in WCDMA to avoid mutex deadlock between WL1 and WFW thread
02/20/15   vs      Dynamic Notch filter Control in WCDMA for better TP at good Ior and better sensitivity at weak signal
11/25/14   sd      Changed parameter and updated rxlm buffer functions 
11/18/14   rmb     Add dynamic functions in MSM to compile for Jolokia.
11/10/14   aro     Interface to config offline clock for XPT IQ capture
10/28/14   vs      Added API to return the static DVGA offsets from the RXLM SS.
10/17/14   aa      Enable NBEE RX calibration
10/02/14   dw      Update TxFE bandwith specific settings
08/21/14   rmb     Add alternate support in the WCDMA MSM layer.
06/30/14   aro     Added support to handle FET+NBR+DSDA
06/06/14   ac      nbr support
04/25/14   vs      Added stub for rfwcdma_msm_update_iqmc_settings
03/25/14   kai     DimePM compiler error fix
03/14/14   aa      DimePM compiler error fix
02/07/14   aa      DimePM related changes
01/08/14   kai     Remove ET chain txlm buffer allocation
08/27/13   dj      Added rfwcdma_msm_update_rxlm_buffer_V2 which takes in 
                   modem_chain as a parameter
08/21/13    ka     Fix OFT link errors
08/18/13  zhh      Increase RxLM dynamic mask length to 32bit
07/22/13   sma     RSB dyn mask update
06/24/13   ac      txlm state for dc cfg 
04/04/13   aa      Removed rfwcdma_msm_get_txlm_chain
03/01/13   kai     Removed rfwcdma_msm_set_dpd_state 
02/22/13   id      Support for new MLine computation using RMS dBFs
02/07/13   ac      ici dyn mask update
02/06//13  ac      hs rxlm state update
01/24/13   kai     Add API to get the default IQ gain value from the TxLM buffer
11/15/12   ka      Initial support for DCHSUPA
11/09/12   kai     Added update ET TxLM buffer
11/05/12   tks     Added function prototype for rsb coefficients computation 
10/03/12   ac      Added support for modem chain configuration
09/10/12   aa      Added API to update modem chain configuration from RFC 
05/23/12   dw      ICI Cal updates for Nikel 
04/16/12   kai     Added support for Tx_AGC_ADJ_PDM_DELAY setting in 
                   apt_setting_txlm & removed old set_tx_adj_pdm_delay function 
03/07/12   cri     Added initial DPD support
02/16/11   ac      notch support for wcdma
12/22/11   ac      lna phase offset compensation
12/21/11   ac      iq sample capture fix for ici cal
12/05/11   swb     Add support for dynamic config of ICI coefficients
12/01/11   dw      Add support for WCDMA low power mode (DSR)
09/25/11   vb      Support for DAC cal using different Irefs
09/19/11   ac      spectral inversion change
09/06/11   dw      Enable APT
08/03/11   dw      Remove rfnv_wcdma.h
07/18/11   dw      return status for rfwcdma_msm_update_rxlm_buffer
06/13/11   dw      Add rfwcdma_msm_get_zero_db_digital_gain()
06/06/11   dw      Add rfwcdma_msm_spectrum_inversion_type
05/25/11   dw      Add support for single tone test
05/13/11   ad      Fix compiler warnings 
05/10/11   ad      Added IQ capture support with dynamic page change 
04/27/11   dw      mainline RF_INTERIM_NIKEL_CM_SUPPORT    
04/15/11   ad      Added preliminary support for IQ capture and ICI multi-carrier cal
04/14/11   dw      Update rfwcdma_msm_get_cm_delta_rxf()
04/12/11   dw      dynamic RxLM updates 
02/18/11   dw      Nikel CM support of delta RxF buffer 
03/02/11   dw      cleanup obsolete functions
02/17/11   dw      Add support to configure Tx modulator
01/28/11   dw      Initial TxLM support for WCDMA
01/05/10   dw      Initial support for RxLM
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
08/31/10   ka      Move PA valid bypass setting to pa enable function  
08/04/10   dw      Added support for channel specific ICI coeff programming
03/15/10   ad      Remove direct gpio writes (fixes jtag dependency issue)
03/09/10   kguo    Added prototype for rfwcdma_msm_clear_pa_tx_rot_angle
01/29/10    ad     Merge SC-DRX, DC-DRX changes from mdm9k dev branch
01/11/10   kguo    Added rfwcdma_msm_dc_set_rxf for DC rxf settings
08/18/09    ka     Added APIs to set pa range and check tx agc override.
03/27/09   ckl     Added rfwcdma_msm_get_tx_agc_adj function
03/09/09   clk     Code cleanup
10/27/08   av      Initial version.

==============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "ftm.h"
#include "ftm_hspa_lib.h"
#include "wfw_misc_intf.h"
#include "txlm_intf.h"
#include "rfdevice_wcdma_type_defs.h"

#define RFWCDMA_ARM_TX_GENERAL_CTL_FTM      0x004
#define RFWCDMA_ARM_TX_GENERAL_CTL_FTM_EUL  0x204
#define RFWCDMA_ARM_TX_GENERAL_CTL_ONLINE   0x284

/* Bitwidth of Tx rotator reolution */
#define RFWCDMA_MSM_TX_ROT_ROSOLUTION          24

/* Tx DAC sampling rate in Hz */
#define RFWCDMA_MSM_TX_DAC_RATE         275200000 

/* TxFE input sampling rate in Hz */
#define RFWCDMA_MSM_TXFE_IN_RATE      RFWCDMA_MSM_TX_DAC_RATE/16

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to store the TX delay values. The data is updated from NV at bootup.
  The values are the amount of time in chip resolution

*/

typedef struct
{
  /* @brief: Time delta between the PA_ON signal rise and the start of Tx frame
  boundary*/
  uint16    pa_on_rise_delay;
  /* @brief: Time delta between the PA_ON signal falls after the end of Tx frame
  boundary*/
  uint16    pa_on_fall_delay;
  /* @brief: Time delta between the TX_ON signal rise and the start of Tx frame
  boundary*/
  uint16    tx_on_rise_delay;
  /* @brief: Time delta between the TX_ON signal falls after the end of Tx frame
  boundary*/
  uint16    tx_on_fall_delay;
}rfwcdma_msm_delay_type;

typedef struct
{
  uint32 ici_i_coef0_1;
  uint32 ici_i_coef2_3;
  uint32 ici_i_coef4_5;
  uint32 ici_i_coef6_7;
  uint32 ici_i_coef8_9;
  uint32 ici_i_coef10_11;
  uint32 ici_i_coef12_13;
  uint32 ici_i_coef14_15;
  uint32 ici_q_coef0_1;
  uint32 ici_q_coef2_3;
  uint32 ici_q_coef4_5;
  uint32 ici_q_coef6_7;
  uint32 ici_q_coef8_9;
  uint32 ici_q_coef10_11;
  uint32 ici_q_coef12_13;
  uint32 ici_q_coef14_15;
  uint32 ici_conig;
} rfwcdma_msm_ici_coeff_type;

typedef enum
{
  RFWCDMA_MSM_ICI_LOW_CHAN,
  RFWCDMA_MSM_ICI_HIGH_CHAN,
  RFWCDMA_MSM_ICI_NUM_CHAN_TYPE,
} rfwcdma_msm_ici_chan_type;

/* Enum for Notch Configuration*/
typedef enum
{
  /*  During regular tune and G2W, L2W tune away*/
  RFWCDMA_MSM_STATIC_NOTCH_TUNE,

  /* During Neighbour W2W Measurement */
  RFWCDMA_MSM_STATIC_NOTCH_W2W_MEAS,

  /* During dynamic callback in Tune */
  RFWCDMA_MSM_DYNAMIC_NOTCH_TUNE,

  /* During dynamic callback in Sleep*/
  RFWCDMA_MSM_DYNAMIC_NOTCH_SLEEP,

  RFWCDMA_MSM_MAX_NOTCH,

  RFWCDMA_MSM_INVALID_NOTCH = RFWCDMA_MSM_MAX_NOTCH
} rfwcdma_msm_notch_config_type;

/*--------------------------------------------------------------------------*/
/*! Structure type used for dynamic notch command */
typedef struct
{
  /* Number of notches to be programmed to Common Notch RxFE Group 01*/
  uint32 num_notch_cmn_grp01_regs;
  /* Number of notches to be programmed to Common Notch RxFE Group 23*/
  uint32 num_notch_cmn_grp23_regs;
}rfwcdma_msm_dyn_notch_cmd_param_type;

/* narrow band power estimator configuratoin */
typedef struct
{
  uint32 config;
  uint32 start_action_sample;
  uint32 start_mask;
  uint32 stop_action_sample;
  uint32 command;
} rfwcdma_msm_nbpwr_setting_type;

/* WCDMA NOTCH GROUP CONFIG */

#define RFWCDMA_NUM_NOTCH_GROUP 4
#define RFWCDMA_NUM_WB_NOTCH_GROUP 2
#define RFWCDMA_NUM_CMN_NOTCH_GROUP 2

typedef struct
{
/* WB Notch config to say how many filters are enabled.*/
  uint8 cfg;

}rfwcdma_msm_wb_notch_filter_cfg;

typedef struct
{

/* Common Notch config to say how many filters are enabled.*/
  uint8 cfg;
  
  /* Every Common notch filter should be associated with a WB chain. The below variable confirms it*/
  boolean is_associated;

  /* The RXLM buffer id which is associated with this common notch*/
  uint8 associated_rxlm_buff;  	
}rfwcdma_msm_cmn_notch_filter_cfg;



typedef struct
{

/*Common Notch  Config*/
  rfwcdma_msm_wb_notch_filter_cfg wb_notch[RFWCDMA_NUM_WB_NOTCH_GROUP];

/*Common Notch  Config*/
  rfwcdma_msm_cmn_notch_filter_cfg cmn_notch[RFWCDMA_NUM_CMN_NOTCH_GROUP];
 
}rfwcdma_msm_notch_filter_cfg;
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Enum definition of RxLM/DTR dynamic group for WCDMA
 
    @details
    This is internal enum used by RF WCDMA to identify the DTR dynamic group

*/
typedef enum
{
  /*! IQ mistmach group */
  RFWCDMA_MSM_DTR_IQMC_GROUP,

  /*! ICI filter group */
  RFWCDMA_MSM_DTR_ICI_GROUP,

  /*! Dedicated Notch group */
  RFWCDMA_MSM_DTR_DEDICATED_NOTCH_GROUP,

  /*! Common Notch 01 group */
  RFWCDMA_MSM_DTR_COMMON_NOTCH_01_GROUP,

  /*!Common Notch 23 group */
  RFWCDMA_MSM_DTR_COMMON_NOTCH_23_GROUP,

  /* DTR group max */
  RFWCDMA_MSM_DTR_GROUP_MAX
}rfwcdma_msm_dtr_dyn_group_type;

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_init_lna_ctrl(void);

/*--------------------------------------------------------------------------*/
uint8 rfwcdma_msm_is_pa_on(void);

/*--------------------------------------------------------------------------*/
uint8 rfwcdma_msm_is_tx_on(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_set_pa_range(uint8 pa_range);
/*--------------------------------------------------------------------------*/
void rfwcdma_msm_bypass_ici_filter
(
  ftm_receive_chain_handle_type chain,
  rfcom_multi_carrier_hspa_id_type carrier,
  rfcom_multi_carrier_id_type carrier_id
);
/*--------------------------------------------------------------------------*/
void rfwcdma_msm_set_pa_bypass(boolean state);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_set_smps_pa_bias_val(uint16 smps_bias_val);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_set_smps_pa_bias_override(boolean flag);
/*--------------------------------------------------------------------------*/
uint16 rfwcdma_msm_get_smps_pa_bias_val(void);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_rxlm_buffer
(
  rfcom_device_enum_type device,
  uint16 channel[],
  uint32 carrier_mask,
  uint32 rxlm_buf_idx,
  uint32 alt_path,
  boolean nbr_enabled,
  rfdevice_wcdma_spur_param_type* dyn_notch_filter_settings,
  rfwcdma_msm_notch_config_type notch_config
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_txlm_buffer
( 
  uint32 txlm_buf_idx,rfcom_device_enum_type device,
  rfcom_wcdma_band_type curr_band_tx ,
  uint32 multi_carrier_idx,
  rfcom_tx_multi_carrier_cfg_type dc_hsupa_cfg
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_static_rxlm_buffer
(
  rfcom_device_enum_type device,
  uint16 channel[],
  uint32 carrier_mask,
  uint32 rxlm_buf_idx,
  boolean nbr_enabled
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_modem_chain_rxlm
(
  uint32 rxlm_buf_idx,
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint32 alt_path
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_modem_chain_rxlm_xpt
(
  uint32 rxlm_buf_idx,
  rfcom_device_enum_type device_fb,
  rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfwcdma_msm_config_tx_modulator(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_get_cm_delta_rxf
(
  rfcom_wcdma_band_type band,
  uint16 chan[],
  rfcom_multi_carrier_hspa_id_type carrier_type,
  WfwCmRfRxfDeltaRegisterStruct *rxf_buf 
);

/*--------------------------------------------------------------------------*/
uint32 *rfwcdma_msm_iq_capture (ftm_config_iq_log_type *msg_ptr);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_free_iq_buffer(void);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_phase_offset_rxlm
(
  rfcom_device_enum_type device,
  uint16 channel[],
  rfcom_multi_carrier_hspa_id_type num_chan,
  uint32 rxlm_buf_idx
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_notch_filter_setting_rxlm
(
  rfcom_device_enum_type device,
  uint16 channel[],
  rfcom_multi_carrier_hspa_id_type num_chan,
  uint32 rxlm_buf_idx
);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_set_cw(boolean on_off,int32 freq_offset, uint32 txlm_handle);

/*--------------------------------------------------------------------------*/
uint32 rfwcdma_msm_get_zero_db_digital_gain(void);

/*--------------------------------------------------------------------------*/
int32 rfwcdma_msm_get_zero_db_digital_gain_10dBm(void);

/*--------------------------------------------------------------------------*/
int32 rfwcdma_msm_convert_raw_nbee_to_10dBm(uint32 raw_nbee);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_set_low_power_mode
(
  boolean en,            
  rfcom_device_enum_type device,                                      
  uint16 channel,                                              
  uint32 alt_path,                                          
  uint32 rxlm_buf_idx                                          
);                                                           
/*--------------------------------------------------------------------------*/
uint32 rfwcdma_msm_get_dtr_group_mask
(
  rfcom_device_enum_type device,
  rfwcdma_msm_dtr_dyn_group_type dtr_group
);
/*--------------------------------------------------------------------------*/
uint32 rfwcdma_msm_rxlm_dyn_mask_update
(
   rfcom_device_enum_type device
);
/*--------------------------------------------------------------------------*/
uint8 rfwcdma_msm_rxlm_rsb_dyn_mask_update
(
   rfcom_device_enum_type device
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_dyn_ici_rxlm
(
  rfcom_device_enum_type device,
  rfcom_tech_type tech,
  uint8 rxlm_buf_idx[]
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_dvga_rxlm
(
  rfcom_device_enum_type device,
  rfcom_tech_type curr_tech,
  uint8 rxlm_buf_idx[]
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_reset_iqmc_rxlm
(
  rfcom_device_enum_type device,
  uint16 channel[],
  rfcom_multi_carrier_hspa_id_type num_chan,
  uint32 rxlm_buf_idx
); 
/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_override_iqmc_rxlm
(
  rfcom_device_enum_type device,
  uint8 rxlm_buf_idx,
  int32 rsb_a,
  int32 rsb_b
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_ici_params_rxlm
(
  rfcom_device_enum_type device,                                                             
  uint16 channel[],                                                                   
  rfcom_multi_carrier_hspa_id_type num_chan,                                          
  uint32 rxlm_buf_idx
);                     

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_bypass_rotator_rxlm
(
  rfcom_device_enum_type device,
  uint16 channel[],
  uint32 rxlm_buf_idx
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_set_et_state_rxlm(uint32 rxlm_buf_idx);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_get_ici_nv_status
(
  rex_tcb_type *task_ptr,
  rex_sigs_type task_wait_sig,
  void (*task_wait_handler)( rex_sigs_type )
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_apt_setting_txlm(uint32 txlm_buf_idx);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_get_rsb_coeff
(
  rfm_device_enum_type curr_device,
  rfcom_wcdma_band_type curr_band,
  boolean spec_inv_status, 
  int32 *rsb_iqmc_a,
  int32 *rsb_iqmc_b
); 
/*--------------------------------------------------------------------------*/

boolean rfwcdma_msm_update_iqmc_settings
(
  rfcom_device_enum_type device,
  uint8 rxlm_buf_idx,
  rfcom_multi_carrier_hspa_id_type num_carrier, 
  rfcom_wcdma_band_type band,
  int32 rsb_a,
  int32 rsb_b,
  uint32 alt_path
);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_config_tx_modulator_dc(boolean enable);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_set_tx_gains(uint16 gain0, uint16 gain1);

/*--------------------------------------------------------------------------*/
uint32 rfwcdma_msm_get_default_txc_gain(lm_handle_type txlm_handle);

/*--------------------------------------------------------------------------*/
int32 rfwcdma_msm_get_rms_dbfs_x100_val(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_set_TX_HSDPCCH_OVSF_NUM(uint32 val);

/*--------------------------------------------------------------------------*/
void rfwcdma_msm_set_TX_EUL_ERAM_MEMORY_BANK(uint32 val);

/*--------------------------------------------------------------------------*/
uint32 rfwcdma_msm_get_samp_rate(uint8 rx_lm_buf_index);

/*--------------------------------------------------------------------------*/
int16 rfwcdma_msm_get_iqgain_val(void);

/*---------------------------------------------------------------------------*/
int32 rfwcdma_msm_read_nbpwr(rfcom_device_enum_type device ,  uint32 rxlm_buf_idx );

/*---------------------------------------------------------------------------*/
boolean rfwcdma_msm_setup_nbpwr(rfcom_device_enum_type device ,  uint32 rxlm_buf_idx, rfwcdma_msm_nbpwr_setting_type *nbpwr_setting);
 
/*---------------------------------------------------------------------------*/
void rfwcdma_msm_configure_nbpwr(rfcom_device_enum_type device,uint32 rxlm_buf_idx,rfcom_multi_carrier_hspa_id_type num_chan);

/*---------------------------------------------------------------------------*/
int32 rfwcdma_msm_get_dvga_offset(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_msm_disable_et_dac(uint32 txlm_buf_idx);
/*----------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_nbr_rxlm_buffer
(
  rfcom_device_enum_type device,                  
  uint16 channel[],                                                                       
  rfcom_multi_carrier_hspa_id_type num_chan,                                              
  uint32 rxlm_buf_idx,
  uint32 alt_path
);       
/*----------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_tx_bw(rfcom_multi_carrier_hspa_id_type carrier_type,
                                 uint32 txlm_buf_idx
                                 );

void rfwcdma_msm_get_static_dvga_offset(uint32 rxlm_buf_idx, int32* static_dvga_offset);

/*------------------------------------------------------------------------------------------------*/
boolean
rfwcdma_msm_xpt_configure_clock
(
  rfm_device_enum_type device
);

/*------------------------------------------------------------------------------------------------*/
boolean
rfwcdma_msm_xpt_deconfigure_clock
(
  rfm_device_enum_type device
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_update_dynamic_notch
(
  rfcom_device_enum_type device,
  uint16 channel[],
  rfcom_multi_carrier_hspa_id_type num_chan,
  uint32 rxlm_buf_idx,
  rfdevice_wcdma_spur_param_type* dyn_notch_filter_settings,
  rfwcdma_msm_notch_config_type notch_config,
  rfwcdma_msm_dyn_notch_cmd_param_type* out_notch_cmd_params
); 
/*--------------------------------------------------------------------------*/
boolean rfwcdma_msm_cmn_notch_filter_deallocate(rfcom_device_enum_type device, uint32 rxlm_buf_idx);
/*--------------------------------------------------------------------------*/
void rfwcdma_msm_enable_notch_filter(int32 rxlm_buf_idx, boolean enable);


#endif /*RFWCDMA_MSM_H*/

