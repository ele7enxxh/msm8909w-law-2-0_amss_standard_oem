#ifndef FTM_GSM_H
#define FTM_GSM_H

/*===========================================================================

               G S M    S P E C I F I C   D E F I N I T I O N S
                         H E A D E R  F I L E

DESCRIPTION
   This file contains GSM definitions which are also 
   contained in a file on the PC.  This keeps them in a place where they 
   can be easily modified and updated as necessary to aid in it's maintenance.

 
   Copyright (c) 2001 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   Copyright (c) 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   Copyright (c) 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   Copyright (c) 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Initially, comments will be entered manually. Later, as the module is
  placed under version control, the version control tool will automatic-
  ally insert change comments into this section. [Delete this comment].

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gsm/ftm_gsm.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/15   par     Add new API for checking if in Cal mode
11/20/14   zbz     Fix KW
11/19/14   ck      Add a FTM feature to set pa bias NV items via diag
10/22/14   zbz     Increase max number of frame sweeps to 400 frames
07/08/14   ec      New type for Sawless linearity control
05/29/14   sb      IP2 cal bug fixes, updates
04/30/14   tsr     Remove RXD fields from gsm rx sweep results
04/23/14   ggs     Rx packet definition does not match between Tools SW and Embedded SW. 
04/16/14   sb      IP2 cal Interface and functionality changes
04/08/14   zbz     Temporarily define FTM_GSM_IQ_DIAG_MAX_SAMPLES to 500 
                   to pass FTM RF IQ Capture test
03/31/14   tsr     Add support for GSM RX Diversity
03/26/14   sb      Add support for IP2 cal for Dime PM
10/16/13   tws     Add PRX+DRX cal command and get rssi structures.
06/25/13   aka     dBm based pwr control cmd for GSM frame sweep
06/10/13   cpm     GSM frame sweep added macros for buffer size
04/17/13   aka     GSM frame sweep feature enhancements
04/22/13   ck      Add structures and macros to support GSM IQ capture
04/17/13   aka     GSM frame sweep feature changes
04/18/13   sb      Add FTM hook for overriding PA scaling,smps bias
03/28/13   aka     GSM frame sweep feature changes.
11/27/12   tws     Add device id to FTM. Remove structs that are not required
                   outside of FTM.
11/19/12   aka     Added ftm_gsm_env_gain_nv_data struct
06/13/12   nv      Added API's to set GSM FTM TX freq offset
11/29/11   sb      Added functionality for GSM IQ CAPTURE
07/07/11   vrb     Support for FTM KV Cal V2
03/16/11   sr      Removed un-used code.
03/11/11   sr      Removed un-used featurization
08/20/10   tsr     Remove INTLOCK/INTFREE code
01/20/10   tws     Use PACK(...) instead of PACKED for qdsp6 compatibility.
07/27/09   rsr     Fix for CR181048, code cleanup.
03/14/09   sr      Merged from LCU for linear PA support
08/08/07   jfc     Mainlined T_MSM6275 feature
08/08/07   jfc     Mainlined T_MSM6280 feature
06/13/07   jfc     Added macros for CS cal
03/28/06   rsr     Lint cleanup. Changed RX burst API to accept the number of
                   frames over which RSSI is averaged.
03/09/06   ka      Merged in 6280 changes.
03/04/05   rsr     Added support for TR6250 RSB calibration. 
12/09/04   rsr     Added support for autocalibration 
09/01/04   xw      Removed WIN32.
12/10/02   xw      Remove grfc_setup_info, mdsp_buffer and chfilt_cfg in 
                   ftm_gsm_data_union.
12/10/02   xw      Code clean up and renamed it to ftm_gsm.h
06/11/01   avi     Created.

=============================================================================*/
#include "comdef.h"
#include "rex.h"
#include "rfm_device_types.h"


typedef word enum_type;

/* Mask for 20 GRFC                             */
#define FTM_GRFC_MASK                          0x000FFFFFL                        

/* Number of entries in the PA LUT              */
#define FTM_PA_LUT_MAX_SIZE                    64                         

#define FTM_DIV_FACTOR                         10

#define FTM_MIN_RX_FRAME_CNT                   2

#define FTM_GSM_MIN_RX_FRAME_CNT               3

#define RX_TX_START_INIT_CNT               1      /* Wait 0.1 seconds to start Rx/Tx init   */

#define MOD_CFG_TRL_GB_POS                 8
#define MOD_CFG_LDN_GB_POS                 12
#define MOD_CFG_OSR_POS                    19
#define MOD_CFG_GB_POL_POS                 21
#define MOD_CFG_SPEC_INV_POS               22
#define MOD_CFG_GB_LEN_POS                 23
#define MOD_CFG_MDSP_IQ_SEL_POS            29
#define MOD_CFG_USE_ROTATOR_POS            30
#define MOD_CFG_CONST_MOD_EN_POS           31

#define FTM_AUTOCAL_NOGAINSTEPS 2

/* TX_PHASE_INCREMENT required for a frequency offset is measured as 
** (Freq_offset_KHz * 1000 /13e6) x 2^(#bits to represent 360 deg)
** In order to make the calculation easier, 
** we store the number 2^(bits to represent 360 deg) /1000 , in this macro.  
*/
#define FTM_GSM_CLOCK_FREQ_MHZ 13

/***************************************************
                IQ CAPTURE
****************************************************/
/*! Maximum GSM IQ Samples (IQ-pairs) that can be captured  */
#define FTM_GSM_IQ_MAX_SAMPLES 8192 

/*! IQ format of the GSM IQ samples which are captured at DSR*/
#define FTM_GSM_IQ_SS_SAMPLE_WIDTH  FTM_IQ_16_BIT_FORMAT

#define FTM_GSM_IQ_SAMPLE_FREQ (2*270.833*1000)    // GSM Bit rate = 270.833 Kbps

/*! This is the maximum number of GSM IQ pairs that can be sent through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
/*! Through experimentation it is found to be 512 bytes: 128 samples * 4 bytes. Each sample:  I (2 byte) + Q(2 byte)  */
/* Temporarily define FTM_GSM_IQ_DIAG_MAX_SAMPLES to 500 to pass FTM RF IQ Capture */
/* #define FTM_GSM_IQ_DIAG_MAX_SAMPLES (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/4) */
#define FTM_GSM_IQ_DIAG_MAX_SAMPLES 500

/*! MACROS to set PA BIAS NV*/
#define MAX_GSM_MOD_TYPES 2
#define MAX_GSM_RGI_INDEX 32
#define MAX_GSM_PA_GAIN_STATE 6
#define MAX_GSM_PCL 16

typedef enum{
  FTM_IQ_CAPTURE_SINGLE_SHOT,
  FTM_IQ_CAPTURE_CONTINOUS,
  FTM_IQ_CAPTURE_STOP

}ftm_iqcapture_type;

/*! The structure used to hold information of acquired IQ samples in ACQUIRE SAMPLES command */
typedef struct{
  boolean data_available; // Flag to check if samples were sucessfully captured
  uint16 number_of_samples_req; // Number of samples requested
  int16 *gsm_iq_buf_ptr; // Buffer used to hold acquired samples. Consecutive elements are I and Q each 2 bytes	
}ftm_gsm_iq_capture_info_type;

/**************************************************
**   GRFC Communication buffer
**************************************************/
typedef enum
{
  FTM_GRFC_ACTIVE_HIGH,
  FTM_GRFC_ACTIVE_LOW
} ftm_enum_grfc_polarity;

typedef enum
{
  FTM_GRFC_INPUT,
  FTM_GRFC_OUTPUT
} ftm_enum_grfc_direction;

typedef enum
{
  FTM_GRFC_STATIC_OUTPUT,
  FTM_GRFC_TIMED_OUTPUT
} ftm_enum_grfc_output_mode;

typedef enum
{
  FTM_GRFC_0,
  FTM_GRFC_1,
  FTM_GRFC_2,
  FTM_GRFC_3,
  FTM_GRFC_4,
  FTM_GRFC_5,
  FTM_GRFC_6,
  FTM_GRFC_7,
  FTM_GRFC_8,
  FTM_GRFC_9,
  FTM_GRFC_10,
  FTM_GRFC_11,
  FTM_GRFC_12,
  FTM_GRFC_13,
  FTM_GRFC_14,
  FTM_GRFC_15,
  FTM_GRFC_16,
  FTM_GRFC_17,
  FTM_GRFC_18,
  FTM_GRFC_19,
  FTM_GRFC_MAX
} ftm_enum_grfc_id;

typedef enum
{
  FTM_GRFC_CHFILT_TIME,
  FTM_GRFC_PA_RAMP_TIME,
  FTM_GRFC_PA_PRECHARGE_TIME,
  FTM_GRFC_TX_SLEEP,
  FTM_GRFC_AUX_MAX
} ftm_enum_aux_grfc_id;

typedef PACK(struct)
{
  int16        start_offset;
  int16         stop_offset;
} ftm_grfc_timing_offset_type;

typedef PACK(struct)
{
  enum_type      ftm_grfc_id;
  PACK(union)
  {
    boolean      ftm_state;
    ftm_grfc_timing_offset_type ftm_grfc_timing;
  } ftm_grfc_cfg;
} ftm_grfc_type;

/*-----------------------------------------------------------------------
** Define the data type for the PA LUT
-----------------------------------------------------------------------*/
typedef enum
{
  FTM_POWER_LEVEL_0_GSM_0900,
  FTM_POWER_LEVEL_1_GSM_0900 = FTM_POWER_LEVEL_0_GSM_0900,
  FTM_POWER_LEVEL_2_GSM_0900 = FTM_POWER_LEVEL_0_GSM_0900,
  FTM_POWER_LEVEL_3_GSM_0900,
  FTM_POWER_LEVEL_4_GSM_0900,
  FTM_POWER_LEVEL_5_GSM_0900,
  FTM_POWER_LEVEL_6_GSM_0900,
  FTM_POWER_LEVEL_7_GSM_0900,
  FTM_POWER_LEVEL_8_GSM_0900,
  FTM_POWER_LEVEL_9_GSM_0900,
  FTM_POWER_LEVEL_10_GSM_0900,
  FTM_POWER_LEVEL_11_GSM_0900,
  FTM_POWER_LEVEL_12_GSM_0900,
  FTM_POWER_LEVEL_13_GSM_0900,
  FTM_POWER_LEVEL_14_GSM_0900,
  FTM_POWER_LEVEL_15_GSM_0900,
  FTM_POWER_LEVEL_16_GSM_0900,
  FTM_POWER_LEVEL_17_GSM_0900,
  FTM_POWER_LEVEL_18_GSM_0900,
  FTM_POWER_LEVEL_19_GSM_0900,
  FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_21_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_22_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_23_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_24_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_25_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_26_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_27_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_28_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_29_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,
  FTM_POWER_LEVEL_30_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,  
  FTM_POWER_LEVEL_31_GSM_0900 = FTM_POWER_LEVEL_20_GSM_0900,

  FTM_POWER_LEVEL_0_GSM_1800= FTM_POWER_LEVEL_0_GSM_0900,
  FTM_POWER_LEVEL_1_GSM_1800= FTM_POWER_LEVEL_0_GSM_0900,
  FTM_POWER_LEVEL_2_GSM_1800= FTM_POWER_LEVEL_0_GSM_0900,
  FTM_POWER_LEVEL_3_GSM_1800,
  FTM_POWER_LEVEL_4_GSM_1800,
  FTM_POWER_LEVEL_5_GSM_1800,
  FTM_POWER_LEVEL_6_GSM_1800,
  FTM_POWER_LEVEL_7_GSM_1800,
  FTM_POWER_LEVEL_8_GSM_1800,
  FTM_POWER_LEVEL_9_GSM_1800,
  FTM_POWER_LEVEL_10_GSM_1800,
  FTM_POWER_LEVEL_11_GSM_1800,
  FTM_POWER_LEVEL_12_GSM_1800,
  FTM_POWER_LEVEL_13_GSM_1800,
  FTM_POWER_LEVEL_14_GSM_1800,
  FTM_POWER_LEVEL_15_GSM_1800,
  FTM_POWER_LEVEL_16_GSM_1800,
  FTM_POWER_LEVEL_17_GSM_1800,
  FTM_POWER_LEVEL_18_GSM_1800,
  FTM_POWER_LEVEL_19_GSM_1800,
  FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_21_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_22_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_23_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_24_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_25_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_26_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_27_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_28_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_29_GSM_1800 = FTM_POWER_LEVEL_20_GSM_1800,
  FTM_POWER_LEVEL_30_GSM_1800,
  FTM_POWER_LEVEL_31_GSM_1800,

  FTM_POWER_LEVEL_0_GSM_1900 = FTM_POWER_LEVEL_0_GSM_0900,
  FTM_POWER_LEVEL_1_GSM_1900,
  FTM_POWER_LEVEL_2_GSM_1900,
  FTM_POWER_LEVEL_3_GSM_1900,
  FTM_POWER_LEVEL_4_GSM_1900,
  FTM_POWER_LEVEL_5_GSM_1900,
  FTM_POWER_LEVEL_6_GSM_1900,
  FTM_POWER_LEVEL_7_GSM_1900,
  FTM_POWER_LEVEL_8_GSM_1900,
  FTM_POWER_LEVEL_9_GSM_1900,
  FTM_POWER_LEVEL_10_GSM_1900,
  FTM_POWER_LEVEL_11_GSM_1900,
  FTM_POWER_LEVEL_12_GSM_1900,
  FTM_POWER_LEVEL_13_GSM_1900,
  FTM_POWER_LEVEL_14_GSM_1900,
  FTM_POWER_LEVEL_15_GSM_1900,
  FTM_POWER_LEVEL_16_GSM_1900,
  FTM_POWER_LEVEL_17_GSM_1900,
  FTM_POWER_LEVEL_18_GSM_1900,
  FTM_POWER_LEVEL_19_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,
  FTM_POWER_LEVEL_20_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_21_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,    
  FTM_POWER_LEVEL_22_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,
  FTM_POWER_LEVEL_23_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_24_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_25_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_26_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_27_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_28_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_29_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_30_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_31_GSM_1900 = FTM_POWER_LEVEL_18_GSM_1900,  
  FTM_POWER_LEVEL_NUM_LEVELS = 19

} ftm_enum_tx_power_level_type;

/* This enum maps the HW groups power level definitions into array indices 
  for the gsm_dacCount_power and dcs_dacCount_poewr arrays defined in 
  ftm_gsm_paprofile.c 
  NOTE: This is not to be confused with the POWER LEVEL's described above 
  and used by the FTM software
*/
typedef enum {
  FTM_GSM_DAC_LEVEL19 = 0,
  FTM_GSM_DAC_LEVEL18,  
  FTM_GSM_DAC_LEVEL17,  
  FTM_GSM_DAC_LEVEL16,  
  FTM_GSM_DAC_LEVEL15,  
  FTM_GSM_DAC_LEVEL14,  
  FTM_GSM_DAC_LEVEL13,  
  FTM_GSM_DAC_LEVEL12,  
  FTM_GSM_DAC_LEVEL11,  
  FTM_GSM_DAC_LEVEL10,  
  FTM_GSM_DAC_LEVEL9 ,  
  FTM_GSM_DAC_LEVEL8 ,  
  FTM_GSM_DAC_LEVEL7 ,  
  FTM_GSM_DAC_LEVEL6 ,  
  FTM_GSM_DAC_LEVEL5 ,  
  FTM_DCS_DAC_LEVEL15 = 0,
  FTM_DCS_DAC_LEVEL14,
  FTM_DCS_DAC_LEVEL13,
  FTM_DCS_DAC_LEVEL12,
  FTM_DCS_DAC_LEVEL11,
  FTM_DCS_DAC_LEVEL10,
  FTM_DCS_DAC_LEVEL9, 
  FTM_DCS_DAC_LEVEL8, 
  FTM_DCS_DAC_LEVEL7, 
  FTM_DCS_DAC_LEVEL6, 
  FTM_DCS_DAC_LEVEL5, 
  FTM_DCS_DAC_LEVEL4, 
  FTM_DCS_DAC_LEVEL3, 
  FTM_DCS_DAC_LEVEL2, 
  FTM_DCS_DAC_LEVEL1, 
  FTM_DCS_DAC_LEVEL0
} dac_power_level;

typedef PACK(struct)
{
  uint8                               ftm_ramp_step_size;
  uint8                                  ftm_pa_lut_size;
} ftm_pa_lut_cfg_type;

typedef PACK(struct)
{
  uint16                    ftm_ramp_up[FTM_PA_LUT_MAX_SIZE];
  uint16                  ftm_ramp_down[FTM_PA_LUT_MAX_SIZE];
  ftm_pa_lut_cfg_type                             ftm_pa_cfg;
} ftm_pa_lut_type;

/*------------------------------------------------------------------------------
** Define the data type for the modulator config
*/

typedef PACK(struct)
{
  boolean         cont_mod_en;     /* set cont modulation;                 bit [31]    in mod_cfg */
  boolean         use_rotator;     /* use rotation;                        bit [30]    in mod_cfg */
  boolean         mdsp_iq_sel;     /* mdsp I/Q selector;                   bit [29]    in mod_cfg */
  boolean            spec_inv;     /* use spectral inversion;              bit [22]    in mod_cfg */
  boolean          gd_bit_pol;     /* guard bit polarity;                  bit [21]    in mod_cfg */
  uint8         gd_bit_bursts;     /* number of guard bits between bursts; bits[28:23] in mod_cfg */
  uint8               os_rate;     /* oversampling rate;                   bits[20:19] in mod_cfg */
  uint8           gd_bit_lead;     /* number of leading guard bits;        bits[18:12] in mod_cfg */
  uint8          gd_bit_trail;     /* number of trailing guard bits;       bits[11:8]  in mod_cfg */
  uint8          burst_length;     /* number of bits in a burst;           bits[7:0]   in mod_cfg */
} ftm_mod_cfg_type;


/*------------------------------------------------------------------------------
** Define the data type for the Burst Transmit Command
** and Continuous Burst Transmit Command
*/

typedef enum
{
  FTM_TX_DATA_SOURCE_PSDRND,
  FTM_TX_DATA_SOURCE_TONE,
  FTM_TX_DATA_SOURCE_BUFFER,
  FTM_TX_DATA_SOURCE_TWOTONE
} ftm_enum_tx_data_source_type;

typedef PACK(struct)
{
  uint8                     slot_num;
  enum_type              data_source;
  uint8                    tsc_index;
  uint32                  num_bursts; /* this number is examined if infinite_duration == FALSE */
  boolean          infinite_duration; /* this flags sets the infinite duration of */
} ftm_tx_burst_type;

typedef PACK(struct)
{
  uint32                 rssi;
  uint32                 rssi_rxd;
} ftm_gsm_rssi_type;

/*------------------------------------------------------------------------------
** Types to set CHFILT parameters
*/

typedef enum
{
  FTM_DMA_ACKNOWLEDGE,
  FTM_AUTO_ACKNOWLEDGE
} ftm_enum_chfilt_auto_ack_type;

typedef enum
{
  FTM_DMA_DISABLE,
  FTM_DMA_ENABLE
} enum_chfilt_dma_ena_type;

typedef enum
{
  FTM_DMA_SEL_A,
  FTM_DMA_SEL_B,
  FTM_DMA_SEL_C,
  FTM_DMA_SEL_D
} ftm_enum_chfilt_dma_sel_type;

typedef enum
{
  FTM_INVERT_Y1_ENABLE,
  FTM_INVERT_Y1_DISABLE
} ftm_enum_chfilt_invert_y1_type;

typedef enum
{
  FTM_ROTATOR_DISABLE,
  FTM_ROTATOR_ENABLE
} ftm_enum_chfilt_use_rotator_type;

/*------------------------------------------------------------------------------
** Define the data type for the channel filter config
*/

typedef PACK(struct)
{
  boolean         auto_ack;  /* 0 - use real DMA acknowledge signal (normal)    */
                             /* 1 - use auto-acknowledge signal (test & debug)  */
  boolean          dma_ena;  /* 0 - disable DMA transfer of CHFILT data to MDSP */
  uint8            dma_sel;  /* 0 - input to stage 1 is DMA'd to MDSP           */
  /* 1 - input to stage 2 is DMA'd to MDSP           */
  /* 2 - input to stage 3 is DMA'd to MDSP           */
  /* 3 - output of stage 3 is DMA'd to MDSP          */
  boolean       invert_y1i;  /* 0 - map 0 to +1, 1 to -1                        */
                             /* 1 - map 0 to -1, 1 to +1                        */
  boolean       invert_y1q;  /* see invert_y1i                                  */
  boolean      use_rotator;  /* 0 - do not use rotator between stages 2 & 3     */
                             /* 1 - use rotator between stages 2 & 3            */
} ftm_chfilt_cfg_type;

/*------------------------------------------------------------------------------
** Define the data type for the Burst Receive Command
*/
typedef enum
{
  FTM_RX_FUNCTION_GET_RSSI,
  FTM_RX_FUNCTION_GET_DC_OFFSETS,
  FTM_RX_FUNCTION_STOP_CONTINOUS_RX,
  FTM_RX_FUNCTION_RF_RX_HW_ON,
  FTM_RX_FUNCTION_RF_RX_HW_OFF,
  FTM_RX_FUNCTION_SINGLE_BURST
} ftm_enum_rx_function_type;

typedef PACK(struct)
{
  uint16       ftm_i_dc_offset;
  uint16       ftm_q_dc_offset;
  uint16       ftm_i_dc_offset_drx;
  uint16       ftm_q_dc_offset_drx;
} ftm_i_q_dc_offset_type;

typedef PACK(struct)
{
  uint8        slot_num;
  enum_type    ftm_rx_funct;
} ftm_rx_burst_type;

typedef PACK(struct)
{
  uint16 dac_start;
  uint16 dac_stop;
  uint16 dac_step;
}ftm_gsm_tx_cal_sweep_type;

/*Enums*/
typedef enum
{
  FTM_PDM_GSM_VCTCXO_ADJ,
  FTM_PDM_GSM_I_DC_OFFSET,
  FTM_PDM_GSM_Q_DC_OFFSET,
  FTM_PDM_GSM_MAX_NUM
} ftm_gsm_pdm_id_type;

/* Return the current phone state */
typedef PACK(struct)
{
  /* Miscellaneous Status */
  enum_type               ftm_rf_mode;    /* Current Mode               */
  enum_type               ftm_phone_id;   /* Phone Type                 */
  /* RF status            */
  uint16                   ftm_channel;   /* Current Channel            */
  enum_type           ftm_tx_power_lvl;   /* Current TX PCL             */
  boolean                 ftm_tx_state;   /* TX state                   */
  boolean                 ftm_pa_state;   /* PA state                   */
  enum_type              ftm_lna_range;   /* LNA gain range             */
  enum_type          ftm_lna_range_drx;   /* DRX LNA gain range         */
  boolean               ftm_lna_on_off;   /* LNA power supply state     */
  boolean              ftm_mixer_state;   /* Mixer state                */
  int16                   ftm_vga_gain;   /* VGA gain (DP2000) support  */
  uint32                      ftm_rssi;   /* RSSI returned by MDSP      */
  uint32                  ftm_rssi_drx;   /* RSSI returned by MDSP      */
  int16   ftm_twotone_freqoffsetkHz;   /* Freq offset for 2 tone tx  */
  /* GRFC llaneous Status */
  uint32             ftm_grfc_io_state;   /* GRFC input/output states   */
  uint32                ftm_grfc_omode;   /* GRFC output mode           */
  uint32             ftm_grfc_polarity;   /* GRFC polarity(acHI/acLO)   */
  uint32                 ftm_grfc_data;   /* GRFC data (register values)*/

  /* PDM status */
  uint16  ftm_pdm_val[FTM_PDM_GSM_MAX_NUM];    /* PDM Values                               */
  ftm_mod_cfg_type             ftm_mod_cfg;    /* Modulator config information             */
  ftm_chfilt_cfg_type       ftm_chfilt_cfg;    /* Channel filter config information        */
  ftm_i_q_dc_offset_type ftm_i_q_dc_offset;    /* I and Q DC offsets calculated by MDSP    */
  uint16         ftm_gsm_padac_override_val;   /* PA dac override value. */
} ftm_gsm_state_type;

typedef PACK(struct)
{
  int16 Aval[FTM_AUTOCAL_NOGAINSTEPS];
  int16 Bval[FTM_AUTOCAL_NOGAINSTEPS];
}ftm_rsb_corr_type;

typedef PACK(struct)
{
  int16 pwr;
  uint16 dac;
}ftm_pwr_dac_type;

typedef PACK(struct)
{
  int maxpwr;
  int minpwr;
}ftm_amam_dynamic_range_type;

typedef PACK(struct)
{
  uint8 data_length;
  uint16* data;
}ftm_gsm_kv_factory_data_type;

typedef PACK(struct)
{
  ftm_pwr_dac_type ExtMeas;
  ftm_amam_dynamic_range_type AmDynRange;
  uint8 writenv;
  ftm_rsb_corr_type rsbcorr;
} ftm_autocal_pkt_type;

typedef PACK(struct)
{
  uint8   slot_num;
  boolean on_off;
  int16   power_dB;
  uint8   mcs;
}ftm_frame_matrix_type;

typedef PACK(struct)
{
  uint8 reg1val;
  uint8 reg2val;
}ftm_chargepumpsbi_type;


typedef PACK(struct)
{
  int16 Aval;
  int16 Bval;
}ftm_rsb_meas_type;

typedef enum
{
  FTM_RSB_DO_DCCAL_G0,
  FTM_RSB_DO_DCCAL_G1,
  FTM_RSB_DO_DCCAL_G2,
  FTM_RSB_DO_DCCAL_G3,
  FTM_RSB_DO_MEASURE_RSB,
  FTM_INVALID_CMD
}ftm_rsb_cmdparam_type;

typedef PACK(struct)
{
  ftm_rsb_cmdparam_type cmdparam;
  int8 doneflag;
  ftm_rsb_meas_type rsbvals;
}ftm_rsb_params_type;

typedef PACK(struct)
{
  uint16 slot_num;
  uint16 pa_range;
}ftm_gsm_pa_range_type;

typedef PACK(struct)
{
  uint16 slot_num;
  uint16 rgi;
  uint8 mod_type_is_gsm;
}ftm_gsm_linear_rgi_type;


typedef PACK(struct) 
{
  uint16 slot_num;
  uint16 pa_range;
  uint16 rgi;
  uint16 env_gain;
  uint16 smps_bias;
  uint8 mod_type_is_gsm;
  uint16 pa_scaling;
}ftm_gsm_tx_slot_config_type;

typedef PACK(struct) 
{
  uint8 gain_range_bitmask; 
  uint8 num_avgs;
  boolean toggle_rx_on;
  uint32 rssi_result[8];
}ftm_gsm_rxcal_parms_type;

typedef PACK(struct) 
{
  uint8 gain_range_bitmask; 
  uint8 num_avgs;
  boolean toggle_rx_on;
  uint32 rssi_result[8];
  uint32 rssi_result_rxd[8];
}ftm_gsm_rxcal_parms_prx_drx_type;

typedef PACK(struct)
{
  uint16  generic_a_16bit_u;
  uint16  generic_b_16bit_u;
}ftm_gsm_generic_2x16bit_u;


#define KV_CAL_V2_MAX_BANDS 4
#define KV_CAL_V2_MAX_CHANNELS 8
typedef PACK(struct)
{
   uint16 num_bands;        //Number of Bands
   uint16 num_tx_cycles;    //Number of Tx Cycles
   uint16 num_meas;         //Number of Measurements per cycle
   uint16 rfmode_list[KV_CAL_V2_MAX_BANDS];   //GSM RF Mode list
   uint16 kv_chan_code[KV_CAL_V2_MAX_CHANNELS];  //KV chan in input & code in resp, 2 * 4
} ftm_gsm_kv_cal_v2_type;

typedef PACK(struct)
{
  uint16 gsm_linear_tx_gain_val;
  uint16 edge_linear_tx_gain_val; 
  uint8 charpredist_env_gain[16];
}ftm_gsm_env_gain_nv_data_type;

#define MAX_NUM_OF_SLOTS 8
#define MAX_NUM_OF_DEVICES 2
#define MAX_NUM_OF_FRAMES 400
#define GSM_FRAME_SWEEP_CONFIG_SIZE 15000
#define MAX_NUM_OF_RESULTS_PER_SLOT 2
#define GSM_FRAME_SWEEP_RESULT_BUFFER_SIZE 3400
#define GSM_FRAME_SWEEP_CONFIG_BUFFER_SIZE 24000

typedef struct
{
  uint8  frame_id;
  uint8  slot_num;
  uint16 chan;
  uint8  lna_state;
  uint8  lna_state_drx;
  uint16 gain_valx16;
  uint16 gain_valx16_drx;
} ftm_gsm_frame_sweep_rx_tag_type;

typedef enum
{
  TX_PCL_MEAS_OP,
  TX_RGI_MEAS_OP,
  RX_RSSI_MEAS_OP,
  RX_DBM_MEAS_OP,
  TX_DBM_MEAS_OP,
} ftm_gsm_frame_sweep_op_id_type;

typedef PACK(struct)
{
  uint8 mcs;
  uint8 pcl;
}ftm_gsm_tx_pcl_meas_data_type;

typedef PACK(struct)
{
  uint8 mcs;
  int16 power_dBm100;
}ftm_gsm_tx_dbm_meas_data_type;

typedef PACK(struct)
{
  uint8 mod_type;
  uint8 pa_state;
  uint8 rgi;
  uint8 env_gain;
}ftm_gsm_tx_rgi_meas_data_type;

typedef PACK(struct)
{
  uint16 chan;
  uint8 lna_state;
}ftm_gsm_rx_rssi_meas_data_type;

typedef PACK(struct)
{
  uint16 chan;
  int16 expected_rx_pwr;
}ftm_gsm_rx_dbm_meas_data_type;

typedef PACK(struct)
{
  uint8 device_id;
  uint8 slot_num;
  uint8 op_id;
  PACK(union)
  {
    ftm_gsm_tx_pcl_meas_data_type tx_pcl_meas_data;
    ftm_gsm_tx_rgi_meas_data_type tx_rgi_meas_data;	
    ftm_gsm_rx_rssi_meas_data_type rx_rssi_meas_data;
    ftm_gsm_rx_dbm_meas_data_type rx_dbm_meas_data;
    ftm_gsm_tx_dbm_meas_data_type tx_dbm_meas_data;
  }op_data;
}ftm_gsm_slot_payload_type;

typedef PACK(struct)
{
  uint8 band;
  uint16 chan;
  uint8 num_slot_payload;
}ftm_gsm_frame_header_type;

typedef PACK(struct)
{
  uint16 num_frame;
  uint16 num_device;
  uint8  device_id[MAX_NUM_OF_DEVICES];
  uint16 reserved[MAX_NUM_OF_DEVICES];
}ftm_gsm_config_header_type;

typedef enum
{
  FTM_GSM_FRAME_SWEEP_CFG,
  FTM_GSM_FRAME_SWEEP_EXECUTE,
  FTM_GSM_FRAME_SWEEP_GET_RESULTS,
  FTM_GSM_FRAME_SWEEP_CLEANUP,
  FTM_GSM_FRAME_SWEEP_MAX = 0xFFFF
}ftm_gsm_frame_sweep_cmd_id;

typedef struct
{
  uint8 *buffer;
  uint32 used_bytes_count;
}ftm_gsm_frame_sweep_config_type;

typedef PACK(struct)
{
  uint16 chan;
  uint8 lna_state;
  uint32 rssi;
  int16 rx_pwr_dbm16;
}ftm_gsm_rx_rssi_meas_result_type;

typedef PACK(struct)
{
  uint8 op_id;
  PACK(union)
  {
    ftm_gsm_rx_rssi_meas_result_type rssi_meas_result;
  }op_result;
}ftm_gsm_result_type;

typedef PACK(struct)
{
  uint16 frame_id;
  uint8 device_id;
  uint8 slot_num;
  uint8 num_of_results;
  ftm_gsm_result_type result[MAX_NUM_OF_RESULTS_PER_SLOT];
}ftm_gsm_result_payload_type;

typedef PACK(struct)
{
  uint8  empty_result_buffer;
  uint16 buffer_size;
  uint8 buffer_data[GSM_FRAME_SWEEP_RESULT_BUFFER_SIZE];
}ftm_gsm_frame_sweep_result_buffer_type;

typedef PACK(struct)
{
  uint16 payload_count;
  ftm_gsm_result_payload_type result_payload[MAX_NUM_OF_FRAMES*MAX_NUM_OF_DEVICES];
}ftm_gsm_frame_sweep_response_type;

typedef struct
{
  ftm_gsm_frame_sweep_response_type* buffer;
  uint32 buff_write_count;
  uint32 buff_read_count;
}ftm_gsm_frame_sweep_result_type;

typedef PACK(struct)
{
  uint8 gain_state;
  uint16 cal_pdm;
  uint32 icq_val[MAX_GSM_MOD_TYPES][MAX_GSM_RGI_INDEX];
}ftm_gsm_pa_bias_params_type;

typedef struct
{
  uint16 cal_smps_pdm_tbl[MAX_GSM_PA_GAIN_STATE];
  uint16 gsm_smps_pdm_tbl[MAX_GSM_PCL];
  uint16 edge_smps_pdm_tbl[MAX_GSM_PCL];
  
}ftm_gsm_smps_pdm_nv_data_type;

typedef PACK(struct)
{
  uint32 icq_bias[MAX_GSM_PA_GAIN_STATE][MAX_GSM_MOD_TYPES][MAX_GSM_RGI_INDEX];
}ftm_gsm_icq_bias_nv_data_type;

typedef PACK(struct)
{
  uint16 req_rep_pkt_len;
  uint8  error_code;
  uint8  band;
  uint16 online_smps[MAX_GSM_MOD_TYPES][MAX_GSM_PCL];
  uint8  num_pa_state;
  ftm_gsm_pa_bias_params_type ftm_gsm_pa_bias_params[MAX_GSM_PA_GAIN_STATE];
}ftm_gsm_set_pa_bias_nv_type;


typedef PACK(struct)
{
  boolean use_default_params;
  uint8 rfcom_band;
  int16 start_i_code;
  int16 start_q_code;
  //uint8 num_cal_points;  // Total number of IP2 cal nodes
  uint8 unit_step_size; // distance between adjacent cal points
  uint8 width;  // Number of cal points around the start I Q code
  uint16 arfcn;
  uint8 tx_rgi;
  uint8 lna_range;
  uint8 num_frames;
  uint16 step_duration_qs;
  uint16 dc_settling_time_qs;
}
ftm_gsm_ip2_cal_params_type;

typedef PACK(struct)
{
  boolean   use_high_lin;
  uint8     band;
  uint16    arfcn;
}
ftm_gsm_sawless_lin_type;

typedef PACK(struct)
{
    /*Results*/
  uint8 status;
  int32 min_im2_result;
  int16 final_i_code;
  int16 final_q_code;

}
ftm_gsm_ip2_cal_resp_type;


typedef PACK(struct)
{
  enum_type                    ftm_gsm_test_id;
  PACK(union)
  {
    boolean                                      generic_bool;
    uint8                                      generic_8bit_u;
    uint16                                    generic_16bit_u;
    uint32                                    generic_32bit_u;
    enum_type                                  ftm_tx_pwr_lvl;
    int16                                        ftm_vga_gain;
    ftm_gsm_rssi_type                                ftm_rssi;
    ftm_grfc_type                                    ftm_grfc;    /* generic state data for a GRFC      */
    ftm_pa_lut_cfg_type                        ftm_pa_lut_cfg;    /* PA LUT configuration & power level */
    ftm_pa_lut_type                                ftm_pa_lut;    /* PA LUT values and cfg information  */
    ftm_mod_cfg_type                              ftm_mod_cfg;    /* modultaor configuration            */
    ftm_tx_burst_type                            ftm_tx_burst;    /* TX burst configuration             */
    ftm_rx_burst_type                            ftm_rx_burst;    /* RX burst command                   */
    ftm_gsm_state_type                          ftm_gsm_state;    /* Phone state                        */
    ftm_i_q_dc_offset_type                  ftm_i_q_dc_offset;    /* I/Q DC offsets computed by MDSP    */
    ftm_frame_matrix_type                    ftm_frame_matrix;    /* frame matrix                       */
    ftm_gsm_rxcal_parms_type                  ftm_rxcal_parms;
    ftm_gsm_rxcal_parms_prx_drx_type  ftm_rxcal_prx_drx_parms;
    ftm_gsm_generic_2x16bit_u               generic_2x16bit_u;
    ftm_gsm_pa_range_type                 ftm_pa_range_params;
    ftm_gsm_linear_rgi_type                ftm_lin_rgi_params;
    ftm_gsm_tx_slot_config_type        ftm_gsm_tx_slot_config;
    ftm_gsm_kv_cal_v2_type               ftm_kv_val_v2_params;
    ftm_gsm_ip2_cal_params_type        ftm_gsm_ip2_cal_params;
    ftm_gsm_sawless_lin_type       ftm_gsm_sawless_lin_params;
    ftm_gsm_set_pa_bias_nv_type     ftm_set_pa_bias_nv_params;
  } ftm_gsm_data_union;
} gsm_option_type;

/*-----------------------------------------------------------------------------
                               Mutexes
-----------------------------------------------------------------------------*/
extern rex_crit_sect_type ftm_gsm_lock;

/*! mutual exclusion and to provide atomic behavior */
#define RFA_FTM_GSM_ISR_LOCK(rfa_mutex_ptr) REX_ISR_LOCK(rfa_mutex_ptr); \
                                               REX_DISABLE_PREMPTION()

#define RFA_FTM_GSM_ISR_UNLOCK(rfa_mutex_ptr) REX_ENABLE_PREMPTION(); \
                                              REX_ISR_UNLOCK(rfa_mutex_ptr)


/*-----------------------------------------------------------------------------*/







/*===========================================================================

FUNCTION  FTM_GET_TX_FREQUENCY_OFFSET

DESCRIPTION
   This function gets the GSM TX frequency offset set in FTM mode.
   
DEPENDENCIES
   None.

RETURN VALUE
  GSM TX frequency offset set in FTM mode.

SIDE EFFECTS
   None.

===========================================================================*/

/*! todo make this device dependent. This is required by wtr1605 driver */
int32 ftm_gsm_get_tx_frequency_offset( void );

boolean ftm_gsm_is_in_calibration_mode( void );

#endif /* FTM_GSM_H */
