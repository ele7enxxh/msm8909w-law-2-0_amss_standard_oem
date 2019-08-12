#ifndef GNSS_API_H
#define GNSS_API_H
/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc.
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*===========================================================================

                     GNSS API Header File
GENERAL DESCRIPTION
  This module contains the interface functions between GNSS and RF Drivers,

Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/api/gnss_api.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
07/26/2009   jd      Naming conventions Changes dependent on RF PLF4 
                     release
07/17/2009   jd      Added gnss_RfBPStartInit() and gnss_RfBPStopInit() for 
                     RF Boot up calibration.
06/29/2009   jd      Phase Shift is already sent in HW units by RF.
12/15/2008   rw      Initial release
==========================================================================*/ 

/*--------------------------------------------------------------------------
 * Include Files
 * -----------------------------------------------------------------------*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "msgr.h"
#include "msgr_rex.h"
#include "msgr_umid.h"
#include "cxm.h"
#ifndef FEATURE_GNSS_SA
#include "sys.h"
#endif /* ! FEATURE_GNSS_SA */
#include "npa.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/
#define N_MAX_GLO_FREQ (14)
#define C_GNSS_RF_ELNA_GPIO_NUM_DEFAULT             255
#define C_PMIC_NPA_MODE_ID_RF_MODE_DEFAULT          255
#define C_NAVRF_NPA_RESOURCE_NAME_DEFAULT           "UNINITIALIZED"
#define C_NAVRF_MAX_NPA_RESOURCE_NAME_LEN_DEFAULT   40

 /* IMD definitions */
/* WLAN channel range 1-4 for 2.4GHZ band, 1-196 for 5GHz band. */
#define GNSS_IMD_MIN_WLAN_CHANNEL  1
#define GNSS_IMD_MAX_WLAN_CHANNEL_2_POINT_4_GHZ  14
#define GNSS_IMD_MAX_WLAN_CHANNEL_5_GHZ  196


/* WLAN bandwidth range */
#define GNSS_IMD_MIN_WLAN_BW  5
#define GNSS_IMD_MAX_WLAN_BW  40



#if ( defined(FEATURE_GNSS_SA) || defined(FEATURE_CGPS_PLAYBACK))
#define C_CXM_MAX_SUPPORTED_LINK_SETS   24
#endif
/*--------------------------------------------------------------------------
 * Type Declarations
 * -----------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * type gnss_RfLinearityType
 * This enumerates different RF linearity states.
 * -----------------------------------------------------------------------*/
typedef enum {
  GNSS_RF_MODE_5,       /* Dual-band GPS and Glonass */
  GNSS_RF_MODE_TRIBAND, /* Simultaneous GPS, Glonass, and Beidou */
  GNSS_RF_MODE_MAX
} gnss_RfModeEnum;

typedef enum
{
  RF_LINEARITY_LO,   
  RF_LINEARITY_HI
} gnss_RfLinearityEnum;

/*--------------------------------------------------------------------------
 * type gnss_RfMeasCmdEnum
 * This enumerates the commands to control the pre-mean and amplitude 
 * measurements use for DC DAC correction values.
 * -----------------------------------------------------------------------*/
typedef enum
{
  MEAS_STOP = 0,
  MEAS_START = 1
} gnss_RfMeasCmdEnum;

/*--------------------------------------------------------------------------
 * type gnss_MeanMonConfigEnum
 * This enum configures the pre-mean and amplitude measurement period
 * -----------------------------------------------------------------------*/
typedef enum
{
  MEAS_PERIOD_PROG = 0, /* programmable measurement period*/
  MEAS_PERIOD_1_MS,      /* fixed at 1 ms frame boundary */
  /* Enums below should be used for Nav HW Ver 4 */  
  MEAS_PERIOD_OFF = 0, /* OFF */
  MEAS_PERIOD_PROG1, /* Programmable mode (RX1) */
  MEAS_PERIOD_PROG2, /* Programmable mode (RX2) */
  MEAS_PERIOD_FRAME  /* fixed at 1 ms frame boundary */
} gnss_MeanMonConfigEnum;


/*--------------------------------------------------------------------------
 * type gnss_MeanMonType
 * This structure is used query the ADC processor blocks.  
 * RF driver calls this API to determine the I/Q mean of the 
 * digitized IF signal before any downconversion.
 * -----------------------------------------------------------------------*/
typedef struct
{
  boolean  u_SampleCorrupt;
  double   d_MeanMonI;
  double   d_MeanMonQ;
  double d_NumMeasLen;
    /* Measurement length */
} gnss_MeanMonType;

typedef struct {
  msgr_hdr_struct_type  hdr;                   /*!< MSGR header */
} gnss_PgiInitComplete; 

/* Create TECH MODULE MACRO FOR PGI - Second parameter should have 0x followed
   by two characters */
#define MSGR_GPS_PGI   MSGR_TECH_MODULE(MSGR_TECH_GPS, 0x01)

enum
{
  MSGR_DEFINE_UMID(GPS, PGI, CMD, INIT_COMPLETE, 0x00, gnss_PgiInitComplete),
  MSGR_DEFINE_UMID(GPS, PGI, RSP, INIT_COMPLETE, 0x01, gnss_PgiInitComplete)
}; 

/*--------------------------------------------------------------------------
 * type gnss_AdcProcEnum
 * This enumeration is used to select an ADC processor
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_ADC_PROC_NONE = 0,
    /* No ADC processor specified */
  GNSS_ADC_PROC_1_SYNC,
    /* Select synchronous ADC processing block */
  GNSS_ADC_PROC_2_SYNC,
    /* Select synchronous ADC processing block */
  GNSS_ADC_PROC_PRX_ASYNC,
    /* Select asynchronous ADC processing block for primary Rx. */
  GNSS_ADC_PROC_DRX_ASYNC,
    /* Select asynchronous ADC processing block for diversity Rx. */
  GNSS_ADC_PROC_MAX
    /* Upper bound to number of ADC processors */
} gnss_AdcProcEnum;

/*--------------------------------------------------------------------------
 * type gnss_AdcProcSampleRateEnum
 * This enumeration is used to select the ADC proc sample rate
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_ADC_PROC_SAMPLE_RATE_OFF = 0,
    /* Disable ADC Proc */
  GNSS_ADC_PROC_SAMPLE_RATE_FULL,
    /* Full main clock rate */
  GNSS_ADC_PROC_SAMPLE_RATE_HALF,
    /* Half main clock rate */
  GNSS_ADC_PROC_SAMPLE_RATE_QTR,
    /* Quarter main clock rate */
  GNSS_ADC_PROC_SAMPLE_RATE_MAX
    /* Upper bound of enum */
} gnss_AdcProcSampleRateEnum;


/*--------------------------------------------------------------------------
 * type gnss_AdcProcQuantizerTypeEnum
 * This enumeration is used to select the ADC proc quantizer type (Valid for 
 * Nav HW Ver 4) 
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_ADC_PROC_QUANT_TYPE_OFF = 0,
  GNSS_ADC_PROC_QUANT_TYPE_SDM,
  GNSS_ADC_PROC_QUANT_TYPE_LINEAR,
  GNSS_ADC_PROC_QUANT_TYPE_LIMITER
} gnss_AdcProcQuantizerTypeEnum;

/*--------------------------------------------------------------------------
 * type gnss_SyncAdcProcConfigPutType
 * This structure is used configure the synchronous ADC processor blocks 
 * which take the input from either the primary or diversity Rx input from 
 * the SDM on the digital die.  This synchronous ADC processor feeds the 
 * four baseband processors.  RF drivers will configure the synchronous 
 * ADC processors.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_AdcProcSampleRateEnum   e_AdcProcRate;
    /* Sample rate */
  uint32                       q_QuantMap;
    /* Quantization map */
  gnss_AdcProcQuantizerTypeEnum     e_AdcProcQuantType;
  /* Quantizer type is valid for Nav HW Ver 4 */
} gnss_SyncAdcProcConfigPutType;

/*--------------------------------------------------------------------------
 * type gnss_AdcProcConfigPutType
 * This union is used configure the corresponding ADC processor
 * -----------------------------------------------------------------------*/
typedef union
{
  gnss_SyncAdcProcConfigPutType   z_SyncAdcProcConfigPut;
    /* Synchronous ADC processors 1 and 2 */
} gnss_AdcProcConfigPutType;

/*--------------------------------------------------------------------------
 * type gnss_AdcProcIqCntrlType
 * This structure is used configure the swap I/Q and I/Q sign inversion 
 * command registers for either the synchronous or asynchronous ADC 
 * processors.
 * -----------------------------------------------------------------------*/
typedef struct
{
  boolean  u_SignInvI;
    /* Invert polarity of I component input */
  boolean  u_SignInvQ;
    /* Invert polarity of Q component input */
  boolean  u_SwapIQ;
    /* Swap I/Q component inputs */
} gnss_AdcProcIqCntrlType;

/*--------------------------------------------------------------------------
 * type gnss_AdcProcBlankPolEnum
 * This enumeration is used for the blanking polarity
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_ADC_PROC_BLANK_POL_POS = 0,
    /* Positive blank polarity (active high) */
  GNSS_ADC_PROC_BLANK_POL_NEG = 1,
    /* negative blank polarity (active low) */
  GNSS_ADC_PROC_BLANK_POL_MAX
    /* Upper bound of enum */
} gnss_AdcProcBlankPolEnum;

typedef enum
{
  GNSS_RF_DEFAULT_CFG,        // 0 /* NAVRF driver uses default config - Customer setting has no effect */
  GNSS_RF_HAS_ELNA,           // 1
  GNSS_RF_HAS_NO_ELNA,        // 2
  GNSS_RF_HAS_ILNA,           // 3
  GNSS_RF_SINGLE_SAW_HAS_ELNA // 4
} gnss_LnaConfigEnumType;

/* -------------------------------------------------------------------------
 * Function pointer for eLNA enable
 *
 * Description: 
 *  This function is used to enable/disable the eLNA GPIO.  This function
 *  will be called from the Nav RF driver.
 *
 * Parameters:
 *  uint8 - GPIO number that is used to control the eLNA
 *  boolean - TRUE for enable, FALSE for disable
 *
 * Return value: 
 *  boolean - TRUE for success, FALSE for failure
 * -----------------------------------------------------------------------*/
typedef boolean(*gnss_ExternalLnaFuncPtrType)(uint8, boolean);

typedef struct
{
  gnss_LnaConfigEnumType e_ExtElnaCtrl; /* External ELNA control to specify if non-eLNA/ieLNA/eLNA is present*/
  uint8                  u_GpioNumber;  /* GPIO number on which eLNA needs to be configured */
  gnss_ExternalLnaFuncPtrType p_ExternalLnaFunc; /* Function pointer for eLNA enable */
  char gnss_ExternalNpaResourceName[C_NAVRF_MAX_NPA_RESOURCE_NAME_LEN_DEFAULT]; /* NPA resource Name to be voted for */
  npa_resource_state gnss_ExternalNpaActiveState; /* Mode select in the PAM table can vary for different RF cards */
} gnss_ExternalGNSSConfigStruct;

/*--------------------------------------------------------------------------
 * Blanking signal is ON
 * -----------------------------------------------------------------------*/
#define GNSS_HW_BLANKING_SIG_ON  0x1

/*--------------------------------------------------------------------------
 * Blanking signal is OFF
 * -----------------------------------------------------------------------*/
#define GNSS_HW_BLANKING_SIG_OFF  0x0

/*--------------------------------------------------------------------------
 * type gnss_AdcProcBlankingEnum
 * This enumeration is used to select an Adc proc blanking input
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_WLAN_2G_BLANKING  = 0,
  GNSS_MODEM_EXTERNAL_BLANKING,
  GNSS_SECONDARY_MODEM_INTERNAL_BLANKING, 
  GNSS_ADC_BLANKING_MAX,
} gnss_AdcProcBlankingEnum;

/*--------------------------------------------------------------------------
 * type gnss_AdcProcBlankConfigType
 * This structure defines the blanking capabilities of the ADC processing 
 * block
 * -----------------------------------------------------------------------*/
typedef struct
{
  boolean                    u_BlankEn;
    /* Enable control for blanking signal, 
       must be set before blanking signal is turned on */
  gnss_AdcProcBlankPolEnum   e_BlankPol;
    /* Blank signal Polarity (Pos or Neg) */
  /* There are a total of 4 blanking inputs.
     Input 0 is taken care by u_BlankEn and e_BlankPol and Inputs 1 to 3
     are designed using arrays. */
  boolean                    u_BlankEn1To3[GNSS_ADC_BLANKING_MAX];
  gnss_AdcProcBlankPolEnum   e_BlankPol1To3[GNSS_ADC_BLANKING_MAX];
} gnss_AdcProcBlankConfigType;

/*--------------------------------------------------------------------------
 * type gnss_AsyncRsEnum
 * This enumeration is used to select an async resampler path
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_ASYNC_RS_NONE  = 0,
    /* No asynchronous resampler specified */
  GNSS_ASYNC_RS_LBW_AND_HBW,
    /* Select asynchronous resampler to configure for LBW and HBW BPs (Aux 1 and 4) */
  GNSS_ASYNC_RS_DRX, 
    /* Select asynchronous resampler to configure diversity BP (Aux 2) */
  GNSS_ASYNC_RS_GLO,
    /* Select asynchronous resampler to configure Glonass BP (Aux 3) */
  GNSS_ASYNC_RS_BEI,
    /* Select asynchronous resampler to configure BDS BP (Aux 2) */
  GNSS_ASYNC_RS_MAX
    /* Upper bound of enum */
} gnss_AsyncRsEnum;


/*--------------------------------------------------------------------------
 * Phase map size for Async Resampler block
 * -----------------------------------------------------------------------*/
#define GNSS_ASYNC_RS_PHASE_MAP_SIZE 16

/*--------------------------------------------------------------------------
 * type gnss_AsyncRsConfigType
 * This structure is used configure the asynchronous resampler phase map, 
 * floor map, and low pass filter enable .
 * The output of the asynchronous resampler feeds the auxiliary inputs 
 * to the low bandwidth (BP#1), high bandwidth (BP#4), Glonass (BP#3) 
 * and diversity (BP #2).  RF drivers will configure the 
 * asynchronous ADC processors.
 * -----------------------------------------------------------------------*/
typedef struct
{
  uint8    u_LowCoef[GNSS_ASYNC_RS_PHASE_MAP_SIZE];
    /* Phase map look-up table (Low Coefficients) */
  uint8    u_HighCoef[GNSS_ASYNC_RS_PHASE_MAP_SIZE];
    /* Phase map look-up table (High Coefficients) */
  uint8    u_FloorCount;
      /* Hardware compares its latched end count value with this value 
         to decide which phase mapping it should use. */
  boolean  u_AafEnable;
    /* Low pass filter enable */
} gnss_AsyncRsConfigType;

/*--------------------------------------------------------------------------
 * type gnss_AysncRsIfTypeEnum
 * This enum describes the receiver IF type
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_ASYNC_RS_ZIF = 0, 
    /* bypass digital IF downconverter */
  GNSS_ASYNC_RS_LIF  = 1
    /* use digital IF downconverter */
} gnss_AysncRsIfTypeEnum;


/*--------------------------------------------------------------------------
 * Maximum resampling frequency (122.22 MHz)
 * -----------------------------------------------------------------------*/
#define GNSS_ASYNC_RS_SAMPLING_FREQ_MAX  122220000.0


/*--------------------------------------------------------------------------
 * type gnss_AysncRsIfDownConvType
 * This structure defines the values for the digital IF down converter 
 * which frequency translates a complex carrier signal at IF to near 
 * baseband (LIF or ZIF).
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_AysncRsIfTypeEnum    e_IfType;
    /* Downconverter type:
       Zero-Intermediate Freq. or Low-Intermediate Freq. */
  double                     d_IfDownConvFreq;
    /* Digital IF downconverter frequency. Rotator frequency in 
       Hz represented. */
  double                     d_AsyncRsFreq;
    /* Async Resampler sampling frequency in Hz. 
       Typically in the range of 20-120 MHz */
} gnss_AysncRsIfDownConvType;


/*--------------------------------------------------------------------------
 * type gnss_BpEnum
 * This enum is used to select a baseband processor.
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_NONE = 0, 
    /* No baseband processor specified */
  GNSS_BP1_LBW,
    /* Low bandwidth BP #1 */
  GNSS_BP2_DRX, 
    /* Rx diversity path BP #2 */
  GNSS_BP3_GLO,
    /* Glonass BP #3 */
  GNSS_BP4_HBW, 
    /* High bandwidth BP #4 */
  GNSS_BP2_BEI, 
    /* BDS BP #2 */
  GNSS_BP_MAX
    /* Upper bound */
} gnss_BpEnum;


/*--------------------------------------------------------------------------
 * type gnss_AdcProcSampleRateEnum
 * This enumeration is used to select the BP decimation filter sample rate
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_DEC_FLTR_RATE_OFF = 0,
    /* Disable ADC Proc */
  GNSS_BP_DEC_FLTR_RATE_FULL,
    /* Full main clock rate */
  GNSS_BP_DEC_FLTR_RATE_HALF,
    /* Half main clock rate */
  GNSS_BP_DEC_FLTR_RATE_QTR,
    /* Quarter main clock rate */
  GNSS_BP_DEC_FLTR_RATE_MAX
    /* Upper bound of enum */
} gnss_BpDecFltrRateEnum;

/*--------------------------------------------------------------------------
 * type gnss_AdcProcSampleRateEnum
 * This enumeration is used to select the BP decimation filter sample rate
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_4_DEC_FLTR_RATE_OFF = 0,
    /* Disable ADC Proc */
  GNSS_BP_4_DEC_FLTR_RATE_R80,
    /* 80 spc */
  GNSS_BP_4_DEC_FLTR_RATE_R72,
    /* 72 spc, only for BOC(6,1) */
  GNSS_BP_4_DEC_FLTR_RATE_MAX
    /* Upper bound of enum */
} gnss_Bp4DecFltrRateEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpGainStepEnum
 * This enumeration is used to select the BP gain steps.
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_GAIN_STEP_1_32 = 0,
    /* G0, gain = 1/32 */
  GNSS_BP_GAIN_STEP_1_64 = 1,
    /* G1, gain = 1/128 */
  GNSS_BP_GAIN_STEP_1_128 = 2,
    /* G2, gain = 1/128 */
  GNSS_BP_GAIN_STEP_1_256 = 3,
    /* G3, gain = 1/256 */
  GNSS_LEGACY_BP_GAIN_STEP_MAX,
    /* Upper bound of legacy enum */
  GNSS_BP_GAIN_STEP_G0_1, /* Gain step G0  (Gain = 1) */
  GNSS_BP_GAIN_STEP_G3_3_4, /* Gain step G3  (Gain = 3/4) */
  GNSS_BP_GAIN_STEP_G6_1_2, /* Gain step G6  (Gain = 1/2) */
  GNSS_BP_GAIN_STEP_G9_3_8, /* Gain step G9  (Gain = 3/8) */
  GNSS_BP_GAIN_STEP_G12_1_4, /* Gain step G12  (Gain = 1/4) */
  GNSS_BP_GAIN_STEP_G15_3_16, /* Gain step G15  (Gain = 3/16) */
  GNSS_BP_GAIN_STEP_G18_1_8, /* Gain step G18  (Gain = 1/8) */
  GNSS_BP_GAIN_STEP_G21_3_32, /* Gain step G21  (Gain = 3/32) */
  GNSS_BP_GAIN_STEP_G24_1_16, /* Gain step G24  (Gain = 1/16) */
  GNSS_BP_GAIN_STEP_G27_3_64, /* Gain step G27  (Gain = 3/64) */
  GNSS_BP_GAIN_STEP_MAX
} gnss_BpGainStepEnum;

/*--------------------------------------------------------------------------
 * type gnss_Bp1LBwConfigPutType
 * This union is used configure the low bandwidth BP #1.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_AdcProcEnum         e_AdcSrcSel;
    /* Input source select, either ADC proc 1 or 2 */
  boolean                  u_AafEnable;
    /* Bypass anti-alias filter */
  boolean                  u_AuxSelect;
    /* Select AUX input */
  gnss_BpDecFltrRateEnum   e_DecFltrRate;
    /* Decimation filter rate */
  gnss_BpGainStepEnum      e_GainStep;
    /* Specifies the fixed 6 dB gain step */
} gnss_Bp1LBwConfigPutType;

/*--------------------------------------------------------------------------
 * type gnss_Bp2DRxConfigPutType
 * This union is used configure the Rx diversity path BP #2.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_AdcProcEnum         e_AdcSrcSel;
    /* Input source select, either ADC proc 1 or 2 */
  boolean                  u_AafEnable;
    /* Bypass anti-alias filter */
  boolean                  u_AuxSelect;
    /* Select AUX input */
  gnss_BpDecFltrRateEnum   e_DecFltrRate;
    /* Decimation filter rate */
  gnss_BpGainStepEnum      e_GainStep;
    /* Specifies the fixed 6 dB gain step */
} gnss_Bp2DRxConfigPutType;

/*--------------------------------------------------------------------------
 * type gnss_Bp2BdsConfigPutType
 * This union is used configure the BDS BP #2.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_AdcProcEnum         e_AdcSrcSel;
    /* Input source select, either ADC proc 1 or 2 */
  boolean                  u_AafEnable;
    /* Bypass anti-alias filter */
  boolean                  u_AuxSelect;
    /* Select AUX input */
  gnss_BpDecFltrRateEnum   e_DecFltrRate;
    /* Decimation filter rate */
  gnss_BpGainStepEnum      e_GainStep;
    /* Specifies the fixed 6 dB gain step */
} gnss_Bp2BdsConfigPutType;

/*--------------------------------------------------------------------------
 * type gnss_Bp3GloConfigPutType
 * This union is used configure the Glonass BP #3.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_AdcProcEnum         e_AdcSrcSel;
    /* Input source select, either ADC proc 1 or 2 */
  boolean                  u_AafEnable;
    /* Bypass anti-alias filter */
  boolean                  u_AuxSelect;
    /* Select AUX input */
  gnss_BpDecFltrRateEnum   e_DecFltrRate;
    /* Decimation filter rate */
  gnss_BpGainStepEnum      e_GainStep;
    /* Specifies the fixed 6 dB gain step */
} gnss_Bp3GloConfigPutType;

/*--------------------------------------------------------------------------
 * type gnss_Bp4HBwConfigPutType
 * This union is used configure the high bandwidth BP #4.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_AdcProcEnum         e_AdcSrcSel;
    /* Input source select, either ADC proc 1 or 2 */
  boolean                  u_AafEnable;
    /* Bypass anti-alias filter */
  boolean                  u_AuxSelect;
    /* Select AUX input */
  gnss_Bp4DecFltrRateEnum  e_DecFltrRate;
    /* Decimation filter rate */
  gnss_BpGainStepEnum      e_GainStep;
    /* Specifies the fixed 6 dB gain step */
} gnss_Bp4HBwConfigPutType;

/*--------------------------------------------------------------------------
 * type gnss_BpConfigPutType
 * This union is used configure the corresponding baseband processor.
 * -----------------------------------------------------------------------*/
typedef union
{
  gnss_Bp1LBwConfigPutType  z_Bp1LBwConfigPut;
    /* Low bandwidth BP #1 */
  gnss_Bp2DRxConfigPutType  z_Bp2DRxConfigPut;
    /* Rx diversity path BP #2 */
  gnss_Bp3GloConfigPutType  z_Bp3GloConfigPut;
    /* Glonass BP #3 */
  gnss_Bp4HBwConfigPutType  z_Bp4HBwConfigPut;
    /* High bandwidth BP #4 */
  gnss_Bp2BdsConfigPutType  z_Bp2BdsConfigPut;
    /* BDS BP #2 */
} gnss_BpConfigPutType;

/*--------------------------------------------------------------------------
 * type gnss_BpIfTypeEnum
 * This enum describes the receiver IF type
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_ZIF = 0, 
    /* bypass digital IF downconverter */
  GNSS_BP_LIF  = 1
    /* use digital IF downconverter */
} gnss_BpIfTypeEnum;

/*--------------------------------------------------------------------------
 * Quarter BP  clk rate is 20 * 511
 * -----------------------------------------------------------------------*/
#define GNSS_BP_3_GLO_SAMPLE_RATE  102200000.0

/*--------------------------------------------------------------------------
 * type gnss_BpIfDownConvType
 * This structure defines the values for the digital IF down converter 
 * which frequency translates a complex carrier signal at IF to near 
 * baseband (LIF or ZIF).
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_BpIfTypeEnum    e_IfType;
    /* Downconverter type:
       Zero-Intermediate Freq. or Low-Intermediate Freq. */
  double               d_IfDownConvFreq; 
    /* Digital IF downconverter frequency. Rotator frequency in 
       Hz represented. */
} gnss_BpIfDownConvType;

/*--------------------------------------------------------------------------
 * type gnss_BpEqCoefType
 * This structure defines the values for the digital IF down converter 
 * which frequency translates a complex carrier signal at IF to near 
 * baseband (LIF or ZIF).
 * -----------------------------------------------------------------------*/
typedef struct
{
  uint8  u_EqCoef1Q;
    /* Complex FIR equalizer coefficient.1 Q */
  uint8  u_EqCoef2Q;
    /* Complex FIR equalizer coefficient.2 Q */
  uint8  u_EqCoef3Q;
    /* Complex FIR equalizer coefficient.3 Q */
  uint8  u_EqCoef1I;
    /* Complex FIR equalizer coefficient.1 I */
  uint8  u_EqCoef2I;
    /* Complex FIR equalizer coefficient.2 I */
  uint8  u_EqCoef3I;
    /* Complex FIR equalizer coefficient.3 I */
} gnss_BpEqCoefType;


/*--------------------------------------------------------------------------
 * BP1  Notch Frequency resolution 4.092 MHz
 * -----------------------------------------------------------------------*/
#define GNSS_BP_NOTCH_FREQ_RES  4092000.0

/* There is only one prenotch; so #define used.
   If there are more pre notches, this can be converted to enum.
   Choosing 50 as notch index is uint8 (also leaving some gap
   for regular notches - not that we envision to use 49 regular notches) */
#define GNSS_BP_PRENOTCH_INDEX   50
/*--------------------------------------------------------------------------
 * type gnss_BpNotchIndexEnum
 * This enum describes the notch filter index. 
 * (1-6 notch filters for each BP)
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_NOTCH_INVALID = 0,
  GNSS_BP_NOTCH_INDEX_1, /* Notch index for BPs */
  GNSS_BP_NOTCH_INDEX_2,
  GNSS_BP_NOTCH_INDEX_3,
  GNSS_BP_NOTCH_INDEX_4,
  GNSS_BP_NOTCH_INDEX_5,
  GNSS_BP_NOTCH_INDEX_6,
  GNSS_BP1_BP2_BP4_NOTCH_INDEX_MAX, /* BP1, BP2 and BP4 have 6 notches only */
  GNSS_BP_NOTCH_INDEX_7 = 7,
  GNSS_BP_NOTCH_INDEX_8,
  GNSS_BP_NOTCH_INDEX_9,
  GNSS_BP_NOTCH_INDEX_10,
  GNSS_BP_NOTCH_INDEX_11,
  GNSS_BP_NOTCH_INDEX_12,
  GNSS_BP_NOTCH_INDEX_MAX /* This max is used for BP3 only */
} gnss_BpNotchIndexEnum;


/*--------------------------------------------------------------------------
 * type gnss_BpNotchPoleEnum
 * This enum describes the notch filter bandwidth defined in the 
 * Callisto HW HDD
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_NOTCH_POLE_0 = 0, 
  GNSS_BP_NOTCH_POLE_1,
  GNSS_BP_NOTCH_POLE_2,
  GNSS_BP_NOTCH_POLE_3,
  GNSS_BP_NOTCH_POLE_4,
  GNSS_BP_NOTCH_POLE_5,
  GNSS_BP_NOTCH_POLE_6,
  GNSS_BP_NOTCH_POLE_7,
  GNSS_BP_NOTCH_POLE_MAX
} gnss_BpNotchPoleEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpPreNotchPoleEnum
 * This enum describes the pre notch filter bandwidth defined in the 
 * Gen8A HW HDD
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_PRENOTCH_POLE_0 = 0, 
  GNSS_BP_PRENOTCH_POLE_1,
  GNSS_BP_PRENOTCH_POLE_2,
  GNSS_BP_PRENOTCH_POLE_3,
  GNSS_BP_PRENOTCH_POLE_4,
  GNSS_BP_PRENOTCH_POLE_5,
  GNSS_BP_PRENOTCH_POLE_6,
  GNSS_BP_PRENOTCH_POLE_7,
  GNSS_BP_PRENOTCH_POLE_MAX
} gnss_BpPreNotchPoleEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpNotchModeEnum
 * This enum describes the notch fitler mode
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_NOTCH_MODE_OFF = 0, 
    /* Notch filter OFF */
  GNSS_BP_NOTCH_MODE_ON  = 1
    /* Notch filter ON */
} gnss_BpNotchModeEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpPreNotchModeEnum
 * This enum describes the pre notch fitler mode
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_PRENOTCH_MODE_OFF = 0, 
    /* Notch filter OFF */
  GNSS_BP_PRENOTCH_MODE_ON  = 1
    /* Notch filter ON */
} gnss_BpPreNotchModeEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpNotchFltrType
 * This structure is used to configure notch filters (up to 6 different 
 * notch filters per BP)
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_BpNotchIndexEnum     e_Index;
    /* Selects the notch filter to configure. Current Range 1-6 */
  gnss_BpNotchPoleEnum      e_Pole;
    /* Notch filter pole magnitude. */
  gnss_BpNotchModeEnum      e_Mode;
    /* Notch filter control. When off, the notch filter is bypassed 
       but sample delay still applies */
  double                    d_NotchFreq;
    /* Notch filter center frequency, represented as signed frequency 
       in Hz */
} gnss_BpNotchFltrType;

/*--------------------------------------------------------------------------
 * type gnss_BpPreNotchFltrType
 * This structure is used to configure pre notch filters (one pre notch 
 * filter per BP) 
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_BpPreNotchPoleEnum      e_Pole;
    /* Pre Notch filter pole magnitude. */
  gnss_BpPreNotchModeEnum      e_Mode;
    /* Pre Notch filter control. When off, the pre notch filter is bypassed 
       but sample delay still applies */
  double                       d_PreNotchFreq;
    /* Pre Notch filter center frequency, represented as signed frequency 
       in Hz */
} gnss_BpPreNotchFltrType;


/*--------------------------------------------------------------------------
 * type gnss_BpGainAlignModeEnum
 * This enum selects which DC gain alignment method to use to set 
 * the BP#_DCALIGN register.
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_GAIN_ALIGN_SW = 0,
    /* Software to controls threshold settings */
  GNSS_BP_GAIN_ALIGN_HW = 1,
    /* Hardware will apply correction automatically 
       (either with integrate-hold for 1 ms or IIR LPF estimate) */
} gnss_BpGainAlignModeEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpSpecAnalyzerProbePointsEnum
 * This enum is used to select BP# probe point for spectrum analyzer 
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_SPEC_AN_PROBE_POINT_P1 = 0, /* pre notch filter input */
  GNSS_BP_SPEC_AN_PROBE_POINT_P2,     /* pre notch filter output */
  GNSS_BP_SPEC_AN_PROBE_POINT_P3,     /* first notch filter input or test bus input */
  GNSS_BP_SPEC_AN_PROBE_POINT_P4      /* last notch filter output */
} gnss_BpSpecAnalyzerProbePointsEnum;

/*--------------------------------------------------------------------------
 * type gnss_SpecAnalyzerSignalSourceEnum
 * This enum is used to select BP for spec analyzer 
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_SPEC_AN_SIG_SRC_BP1 = 0, /* BP1 */
  GNSS_SPEC_AN_SIG_SRC_BP2,     /* BP2 */
  GNSS_SPEC_AN_SIG_SRC_BP3,     /* BP3 */
  GNSS_SPEC_AN_SIG_SRC_BP4      /* BP4 */
} gnss_SpecAnalyzerSignalSourceEnum;

/*--------------------------------------------------------------------------
 * type gnss_SpecAnalyzerModeEnum
 * This enum is used to select spec analyzer mode
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_SPEC_AN_MODE_OFF = 0, /* Disable SA to save power */
  GNSS_SPEC_AN_MODE_BYPASS,  /* Bypass SA and go directly from BP to SL */
  GNSS_SPEC_AN_MODE_DEC32, /* Decimate by 32 */
  GNSS_SPEC_AN_MODE_DEC64, /* Decimate by 64 */
  GNSS_SPEC_AN_MODE_DEC128, /* Decimate by 128 */
  GNSS_SPEC_AN_MODE_DEC256, /* Decimate by 256 */
  GNSS_SPEC_AN_MODE_OFF2, /* OFF2 - Similar to OFF mode */
  GNSS_SPEC_AN_MODE_OFF3 /* OFF3  - Similar to OFF mode */
} gnss_SpecAnalyzerModeEnum;

/*--------------------------------------------------------------------------
 * type gnss_AdSampleLogSignalSourceEnum
 * This enum is used to select sample log signal source 
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_AD_SAMPLELOG_SRC_RX1IQ1 = 0, /*  1 bit complex samples from RX1 */
  GNSS_AD_SAMPLELOG_SRC_RX1IQ2,     /* 2 bit complex samples from RX1 */
  GNSS_AD_SAMPLELOG_SRC_RX1IQ3,     /* 3 bit complex samples from RX1 */
  GNSS_AD_SAMPLELOG_SRC_RX2IQ1,     /* 1 bit complex samples from RX2 */
  GNSS_AD_SAMPLELOG_SRC_RX2IQ2,     /* 2 bit complex samples from RX2 */
  GNSS_AD_SAMPLELOG_SRC_RX2IQ3,     /* 3 bit complex samples from RX2 */
  GNSS_AD_SAMPLELOG_SRC_RX12IQ1,     /* 1 bit complex samples from RX1 and RX2 */
  GNSS_AD_SAMPLELOG_SRC_RX12IQ2,     /* 2 bit complex samples from RX1 and RX2 */
  GNSS_AD_SAMPLELOG_SRC_RX1IQ6,     /* 6 bit complex samples from RX1 */
  GNSS_AD_SAMPLELOG_SRC_RX2IQ6,     /* 6 bit complex samples from RX2 */
  /* Enums 10 to 14 are not used */
  GNSS_AD_SAMPLELOG_SRC_SAIQ8 = 15     /* 8 bit complex samples from Spec Analyzer */
} gnss_AdSampleLogSignalSourceEnum;

/*--------------------------------------------------------------------------
 * type gnss_AdSampleLogModeEnum
 * This enum is used to select sample log mode 
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_AD_SAMPLELOG_MODE_OFF = 0, /*  OFF */
  GNSS_AD_SAMPLELOG_MODE_VECTOR, /*  VECTOR */
  GNSS_AD_SAMPLELOG_MODE_CIRCULAR, /*  CIRCULAR */
  GNSS_AD_SAMPLELOG_MODE_FRAME /* FRAME  */
} gnss_AdSampleLogModeEnum;

/*--------------------------------------------------------------------------
 * type gnss_AdSampleLogDepthEnum
 * This enum is used to select sample log depth (number of words to store in 
 * sample memory). 
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_AD_SAMPLELOG_DEPTH_N512 = 0, 
  GNSS_AD_SAMPLELOG_DEPTH_N1K, 
  GNSS_AD_SAMPLELOG_DEPTH_N2K, 
  GNSS_AD_SAMPLELOG_DEPTH_N4K, 
  GNSS_AD_SAMPLELOG_DEPTH_N8K, 
  GNSS_AD_SAMPLELOG_DEPTH_N16K, 
  GNSS_AD_SAMPLELOG_DEPTH_N32K, 
  GNSS_AD_SAMPLELOG_DEPTH_NMAX /*  all of sample memory */
} gnss_AdSampleLogDepthEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpDcAlignManualType
 * This structure defines the settings for the DC offset correction 
 * by manual alignment. 
 * -----------------------------------------------------------------------*/
typedef struct
{
  int16    x_DcOffsetQ;
    /* DC offset setting for Q Component. */
  int16    x_DcOffsetI;
    /* DC offset setting for Q Component. */
} gnss_BpDcAlignManualType;

/*--------------------------------------------------------------------------
 * Notch Pole magnitude maximum
 * -----------------------------------------------------------------------*/
#define GNSS_BP_DC_NOTCH_POLE_MAX  5

#define GNSS_NAV_HW_VER4_BP_DC_NOTCH_POLE_MAX  7
/*--------------------------------------------------------------------------
 * type gnss_BpMeasEstModeEnum
 * This enum defines the mean estimate control type for automatic 
 * alignment mode.
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_BP_AUTO_MEAN_EST_MODE_1_MS_INT = 0, /* Integrate-hold for 1 ms */
  GNSS_BP_AUTO_MEAN_EST_MODE_IIR, /* IIR LPF estimator */
  /* OFF modes are used only in Gen8A*/
  GNSS_BP_AUTO_MEAN_EST_MODE_OFF, /* Power savings */
  GNSS_BP_AUTO_MEAN_EST_MODE_OFF2 /* Power savings */
} gnss_BpAutoMeasEstModeEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpDcAlignAutoType
 * This structure defines the settings for the DC offset correction by 
 * automatic alignment.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_BpAutoMeasEstModeEnum    e_MeanMonMode;
     /* Controls the type of mean estimation. 
           FALSE: integrate-hold estimator to 1ms
           TRUE: IIR LPF estimator */
  uint8                     u_DcNotchPole;
    /* Controls the notch bandwidth by defining DC notch 
       filter pole magnitude (range 0-5). */
} gnss_BpDcAlignAutoType;

/*--------------------------------------------------------------------------
 * type gnss_BpDcAlignType
 * This structure defines the settings for the DC offset correction by 
 * either automatic or manual alignment.
 * -----------------------------------------------------------------------*/
typedef union
{
  gnss_BpDcAlignManualType  z_DcAlignManual;
    /* Settings for manual DC alignment by hardware only 
       valid when b_DcAlignMode is set to FALSE */
  gnss_BpDcAlignAutoType    z_DcAlignAuto;
    /* Settings for automatic DC alignment by hardware 
       only valid when b_DcAlignMode is set to TRUE */
} gnss_BpDcAlignType;

/*--------------------------------------------------------------------------
 * type gnss_BpMeanEstType
 * This structure defines the settings for the DC offset correction by 
 * either automatic or manual alignment.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_BpGainAlignModeEnum   e_DcAlignMode;
    /* Either use hardware or software based DC offset alignment mode. */
  gnss_BpDcAlignType         z_DcAlignConfig;
    /* Either the automatic hardware based DC correction 
       or software configurable offsets */
} gnss_BpMeanEstType;

/*--------------------------------------------------------------------------
 * Number of sub-channels for BP1 LBW
 * -----------------------------------------------------------------------*/
#define GNSS_BP1_LBW_SUB_CHAN_NUM  3

/*--------------------------------------------------------------------------
 * Number of sub-channels for BP2 DRx
 * -----------------------------------------------------------------------*/
#define GNSS_BP2_DRX_SUB_CHAN_NUM  3

/*--------------------------------------------------------------------------
 * Number of sub-channels for BP3 GLO
 * -----------------------------------------------------------------------*/
#define GNSS_BP3_GLO_SUB_CHAN_NUM  12

/*--------------------------------------------------------------------------
 * Number of sub-channels for BP4 HBW
 * -----------------------------------------------------------------------*/
#define GNSS_BP4_HBW_SUB_CHAN_NUM  1


/*--------------------------------------------------------------------------
 * type gnss_BpSubChanThreshType
 * This structure defines the settings sub-channel threshold programming.
 * -----------------------------------------------------------------------*/
typedef struct
{
  boolean      u_AutoGainAlign;
    /* Automatic-alignment mode for final quantizer threshold.  When ON, 
       gain control is doen in HW.  When OFF, SW must control the threshold
       settings (Mission mode operation SW controls settings)
    */
  uint8        u_SubChanIndex;
    /* The sub-channel index.  For, Glonass BP (#3) there are 12 subchannels.  
       While the NB and WB BPs have 3 sub-channels and diversity BP uses only 
       one sub-channel.  GNSS software will verify this parameter is specified 
       correctly.
    */
  uint8         u_ThreshI;
    /* Only in manual mode operation. */
  uint8         u_ThreshQ;
    /* Only in manual mode operation. */
} gnss_BpSubChanThreshType;

/*--------------------------------------------------------------------------
 * type gnss_BpPreAmplMonType
 * This structure defines the preliminary amplitude estimator readings to 
 * provide feedback to RF gain settings.
 * -----------------------------------------------------------------------*/
typedef struct
{
  boolean      u_SampleCorrupt;
    /* If blanking was enabled then samples may not be valid. */
  uint32       q_GnssRtc;
    /* Current GPS RTC of this measurement. */
  double       d_PreAmplMonI;
    /* Preliminary amplitude estimator for I component. */
  double       d_PreAmplMonQ;
    /* Preliminary amplitude estimator for Q component */
  double d_NumMeasLen;
    /* Measurement length */
} gnss_BpPreAmplMonType;


/*--------------------------------------------------------------------------
 * type gnss_AdcProcAmplMonType
 * This structure defines the amplitude estimator (mean magnitude)readings to 
 * provide feedback to RF gain settings.
 * -----------------------------------------------------------------------*/
typedef struct
{
  boolean      u_SampleCorrupt;
    /* If blanking was enabled then samples may not be valid. */
  uint32       q_GnssRtc;
    /* Current GPS RTC of this measurement. */
  double       d_AmplMonI;
    /* amplitude estimator for I component. */
  double       d_AmplMonQ;
    /* amplitude estimator for Q component */
  double d_NumMeasLen;
    /* Measurement length */
} gnss_AdcProcAmplMonType;


/*--------------------------------------------------------------------------
 * type gnss_BpReSamplerType
 * This structure defines the values for the resampler block in the BPs.
 * Each resampler consists of a timing control and linear interpolator block.
 * -----------------------------------------------------------------------*/
typedef struct
{
  boolean      b_RsEnable;
  /* Enable resampler, or bypass linear interpolator and disable 
     resmapler phase accumulator. */
  uint32       q_RsRate;
    /* Resampler rate. An unsigned 32-bit fraction that is added to 
       the sample phase accumulator on every valid RS output sample.  */
  uint32       q_PhaseShift;
    /* In HW unit see BPx_RS_PHASE. HW_VAL=(phase_in_degree/360)*2^32.
       Programs the BP resampler for the phase shift. */
} gnss_BpReSamplerType;


/*--------------------------------------------------------------------------
 * type gnss_RfPathEnum
 * This enum defines the different RF paths
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_RF_PATH_NONE = 0,
    /* NONE */
  GNSS_RF_PATH_PRX,
    /* primary RX */
  GNSS_RF_PATH_DRX, 
    /* diversity RX */
  GNSS_RF_PATH_MAX 
    /* Max enum */
} gnss_RfPathEnum;

/*--------------------------------------------------------------------------
 * type gnss_RfIqDcDacCodeType
 * This struct defines DC DAC I/Q values
 * -----------------------------------------------------------------------*/
typedef struct
{
  uint8 u_DacI;
  uint8 u_DacQ;
} gnss_RfIqDcDacCodeType;

/*--------------------------------------------------------------------------
 * type gnss_RfStatusType
 * Maximum size of RF status string
 * -----------------------------------------------------------------------*/

#define GNSS_RF_STATUS_MAX_STR_LEN  12


/*--------------------------------------------------------------------------
 * type gnss_RfStatusType
 * This struct defines the RF status report on every RF configuration.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_RfPathEnum         e_Path;

  struct
  {
     char                         u_StringData[GNSS_RF_STATUS_MAX_STR_LEN];  
     uint8                        u_OpMode;
     uint8                        u_LinearityState;
     uint8                        u_VcoTuneCode;
     int32                        w_PGAGain;
  } z_Data;

} gnss_RfStatusType;

/*--------------------------------------------------------------------------
 * type gnss_RfDynamicStatusType
 * This struct defines the RF status report every 1 second from RF.
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_RfPathEnum         e_Path;

  struct
  {
    gnss_RfIqDcDacCodeType       z_DcDac[2];
    int32                        w_ADCPreMeanI;
    int32                        w_ADCPreMeanQ;
    uint32                       q_LBWPreAmpI;
    uint32                       q_LBWPreAmpQ;
  } z_Data;
} gnss_RfDynamicStatusType;

/*--------------------------------------------------------------------------
 * type gnss_RfGrpDelayDestEnum
 * This enum defines how to compensate for group delay
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_RF_GRP_DELAY_DEST_NONE = 0,
    /* NONE */
  GNSS_RF_GRP_DELAY_DEST_HW,
    /* Compensate in HW (BPs) */
  GNSS_RF_GRP_DELAY_DEST_SW, 
    /* Compensate in software (CC) */
  GNSS_RF_GRP_DELAY_DEST_MAX 
    /* Max enum */
} gnss_RfGrpDelayDestEnum;


/*--------------------------------------------------------------------------
 * type gnss_RfGrpDelayType
 * This struct defines the RF group delay
 * -----------------------------------------------------------------------*/
typedef struct
{
  gnss_RfGrpDelayDestEnum  e_GrpDelayDest;
  float                    f_GroupDelayNs;
} gnss_RfGrpDelayType;

/*--------------------------------------------------------------------------
 * type gnss_RfAdcRegEnum
 * This enum is used to specify GNSS ADC Register (1 to 16)
 * -----------------------------------------------------------------------*/
typedef enum
{
  GNSS_ADC_REG1 = 1,
  GNSS_ADC_REG2,
  GNSS_ADC_REG3, 
  GNSS_ADC_REG4, 
  GNSS_ADC_REG5,
  GNSS_ADC_REG6, 
  GNSS_ADC_REG7, 
  GNSS_ADC_REG8,
  GNSS_ADC_REG9, 
  GNSS_ADC_REG10, 
  GNSS_ADC_REG11,
  GNSS_ADC_REG12, 
  GNSS_ADC_REG13, 
  GNSS_ADC_REG14,
  GNSS_ADC_REG15, 
  GNSS_ADC_REG16,
  GNSS_ADC_IDDQ 
} gnss_RfAdcRegEnum;


/* This enum is for the available WWAN modes */
typedef enum
{
  GNSS_WWAN_STATE_NONE = 0,
  GNSS_WWAN_STATE_1X,
  GNSS_WWAN_STATE_WCDMA,
  GNSS_WWAN_STATE_GSM,
  GNSS_WWAN_STATE_GSM1 = GNSS_WWAN_STATE_GSM,
  GNSS_WWAN_STATE_EVDO,
  GNSS_WWAN_STATE_LTE, 
  GNSS_WWAN_STATE_TDSCDMA,
  GNSS_WWAN_STATE_GSM2,
  GNSS_WWAN_STATE_GSM3
} gnss_WwanStateEnum;


/* Enum of the band */
typedef enum {
  GNSS_RF_BC0_BAND = 0,                  /*    US Cellular (800MHz )*/
  GNSS_RF_BC1_BAND = 1,                  /*    US PCS (1900MHz)     */
  GNSS_RF_BC2_BAND = 2,                  /*    JTACS                */
  GNSS_RF_BC3_BAND = 3,                  /*    JCDMA                */
  GNSS_RF_BC4_BAND = 4,                  /*    Korean PCS           */
  GNSS_RF_BC5_BAND = 5,                  /*    NMT (450MHz)         */
  GNSS_RF_BC6_BAND = 6,                  /*    IMT (2.1GHz)         */
  GNSS_RF_BC7_BAND = 7,
  GNSS_RF_BC8_BAND = 8,
  GNSS_RF_BC9_BAND = 9,
  GNSS_RF_BC10_BAND = 10,
  GNSS_RF_BC11_BAND = 11,
  GNSS_RF_BC12_BAND = 12,
  GNSS_RF_BC13_BAND = 13,
  GNSS_RF_BC14_BAND = 14,
  GNSS_RF_BC15_BAND = 15,
  GNSS_RF_BC16_BAND = 16,
  GNSS_RF_BC17_BAND = 17,
  GNSS_RF_BC18_BAND = 18,
  GNSS_RF_BC19_BAND = 19,

  GNSS_RF_GPS_BAND,                      /*    1785MHz              */
  GNSS_RF_FM_BAND,                       /*    ANALOG BAND          */

  GNSS_RF_BCI_BAND,                      /* WCDMA IMT 2100     */
  GNSS_RF_BCII_BAND,                     /* WCDMA PCS 1900     */
  GNSS_RF_BCIII_BAND,                    /* WCDMA 1700         */
  GNSS_RF_BCIV_BAND,                     /* WCDMA 1700-2100    */
  GNSS_RF_BCV_BAND,                      /* WCDMA 850          */
  GNSS_RF_BCVI_BAND,                     /* WCDMA 800          */
  GNSS_RF_BCVIII_BAND,                   /* WCDMA 900          */
  GNSS_RF_BCIX_BAND,                     /* WCDMA 1700         */
  GNSS_RF_BCXI_BAND,                     /* WCDMA 1500         */
  GNSS_RF_BCXIX_BAND,                    /* WCDMA 830-875      */

  GNSS_RF_EGSM_BAND,                     /* GSM 900             */
  GNSS_RF_DCS_BAND,                      /* GSM 1800            */
  GNSS_RF_GSM_850_BAND,                  /* GSM 850             */
  GNSS_RF_GSM_1900_BAND,                 /* GSM 1900            */

  GNSS_RF_LTE_B1_BAND,
  GNSS_RF_LTE_B4_BAND,
  GNSS_RF_LTE_B7_BAND,
  GNSS_RF_LTE_B13_BAND,
  GNSS_RF_LTE_B17_BAND,
  GNSS_RF_LTE_B40_BAND,

  GNSS_RF_GSM_450_BAND,
  GNSS_RF_GSM_480_BAND,
  GNSS_RF_GSM_750_BAND,

  GNSS_RF_MAX_BAND
} gnss_RfCardBandEnum;

 
typedef enum
{
  GNSS_RFCOM_BAND_MIN = 0,
  GNSS_RFCOM_BAND_IMT = GNSS_RFCOM_BAND_MIN,
  GNSS_RFCOM_BAND_1900,
  GNSS_RFCOM_BAND_BC3,
  GNSS_RFCOM_BAND_BC4,
  GNSS_RFCOM_BAND_800,
  GNSS_RFCOM_BAND_BC8,
  GNSS_RFCOM_BAND_BC9,
  GNSS_RFCOM_BAND_BC11,
  GNSS_RFCOM_BAND_BC19,
  GNSS_RFCOM_NUM_WCDMA_BANDS,
  GNSS_RFCOM_BAND_BC6,
  GNSS_RFCOM_BAND_BC7,
  GNSS_RFCOM_BAND_INVALID
} gnss_RfcomWcdmaBandEnum;


/* LTE specific band enum */
typedef enum 
{
  GNSS_RFCOM_BAND_LTE_B1,
  GNSS_RFCOM_BAND_LTE_B2,
  GNSS_RFCOM_BAND_LTE_B3,
  GNSS_RFCOM_BAND_LTE_B4,
  GNSS_RFCOM_BAND_LTE_B5,
  GNSS_RFCOM_BAND_LTE_B6,
  GNSS_RFCOM_BAND_LTE_B7,
  GNSS_RFCOM_BAND_LTE_B8,
  GNSS_RFCOM_BAND_LTE_B9,
  GNSS_RFCOM_BAND_LTE_B10,
  GNSS_RFCOM_BAND_LTE_B11,
  GNSS_RFCOM_BAND_LTE_B12,
  GNSS_RFCOM_BAND_LTE_B13,
  GNSS_RFCOM_BAND_LTE_B14,
  GNSS_RFCOM_BAND_LTE_B15,
  GNSS_RFCOM_BAND_LTE_B16,
  GNSS_RFCOM_BAND_LTE_B17,
  GNSS_RFCOM_BAND_LTE_B18,
  GNSS_RFCOM_BAND_LTE_B19,
  GNSS_RFCOM_BAND_LTE_B20,
  GNSS_RFCOM_BAND_LTE_B21,
  GNSS_RFCOM_BAND_LTE_B22,
  GNSS_RFCOM_BAND_LTE_B23,
  GNSS_RFCOM_BAND_LTE_B24,
  GNSS_RFCOM_BAND_LTE_B25,
  GNSS_RFCOM_BAND_LTE_B26,
  GNSS_RFCOM_BAND_LTE_B27,
  GNSS_RFCOM_BAND_LTE_B28,
  GNSS_RFCOM_BAND_LTE_B29,
  GNSS_RFCOM_BAND_LTE_B30,
  GNSS_RFCOM_BAND_LTE_B31,
  GNSS_RFCOM_BAND_LTE_B32,
  GNSS_RFCOM_BAND_LTE_B33,
  GNSS_RFCOM_BAND_LTE_B34,
  GNSS_RFCOM_BAND_LTE_B35,
  GNSS_RFCOM_BAND_LTE_B36,
  GNSS_RFCOM_BAND_LTE_B37,
  GNSS_RFCOM_BAND_LTE_B38,
  GNSS_RFCOM_BAND_LTE_B39,
  GNSS_RFCOM_BAND_LTE_B40,
  GNSS_RFCOM_BAND_LTE_B41,
  GNSS_RFCOM_BAND_LTE_B42,
  GNSS_RFCOM_BAND_LTE_B43,
  GNSS_RFCOM_NUM_LTE_BANDS,
  GNSS_RFCOM_BAND_LTE_INVALID
} gnss_RfcomLTEBandEnum;

/* TDSCDMA specific band enum */
typedef enum
{
   GNSS_RFCOM_BAND_TDSCDMA_B34,
   GNSS_RFCOM_BAND_TDSCDMA_B39,
   GNSS_RFCOM_BAND_TDSCDMA_B40,
   GNSS_RFCOM_NUM_TDSCDMA_BANDS,
  /* Any PARTIAL BAND information needs to be entered after this point */
   GNSS_RFCOM_BAND_TDSCDMA_B40_P,
   GNSS_RFCOM_BAND_TDSCDMA_INVALID
} gnss_RfcomTdscdmaBandEnum;

/* Union of all the Band Types */
typedef union 
{
  gnss_RfcomTdscdmaBandEnum e_TDSCDMABand; /* for TDSCDMA Band */
  gnss_RfcomWcdmaBandEnum e_WCDMABand; /* For WCDMA Band */
  gnss_RfcomLTEBandEnum e_LTEBand;     /* For LTE Band */
  gnss_RfCardBandEnum e_RfBand;        /* For other than WCDMA, LTE, or TDSCDMA Band */
} gnss_RfBandType;

typedef struct
{
  uint32                  q_FreqKHz; /* Tech operating frequency in KHz */
  uint32                  q_BwKHz; /* Tech operating bandwidth in kHz */

#if ( defined(FEATURE_GNSS_SA) || defined(FEATURE_CGPS_PLAYBACK))
  uint64  e_RfBand;    /* RF Band */ 
  uint8   e_RfDevice;  
  uint8   e_Direction; /* Direction (Up-link/Down-Link/Both) */
  uint32  q_LinkType;  /* Type (Diversity/PowerMonitor) */
#else
  sys_band_class_e_type  e_RfBand;    /* RF Band */ 
  cxm_rfdev_type         e_RfDevice; /* RF device the tech is using */  
  cxm_tech_link_direction e_Direction; /* Direction (Up-link/Down-Link/Both) */
  cxm_tech_link_type      q_LinkType;  /* Type (Diversity/PowerMonitor) */
#endif /* FEATURE_GNSS_SA */
  uint32                 q_Channel;  /* Channel info */ 
  int16                   x_TxPower;  /* Tx Power - needed for LTE bands */
} gnss_WwanStateLinkInfo;

typedef struct
{
  cxm_tech_type           e_WwanTech;  /* WWAN tech */  
  uint64                  t_WwanStateSlowClkTime; /* Slow clk timestamp when WWAN state info was received */
  uint64                  t_PowerSlowClkTime; /* Slow clk timestamp when WWAN state info was received */
  uint8                   u_NumLinkInfoSets; 
#if ( defined(FEATURE_GNSS_SA) || defined(FEATURE_CGPS_PLAYBACK))
  gnss_WwanStateLinkInfo  z_LinkInfo[C_CXM_MAX_SUPPORTED_LINK_SETS];
#else
  gnss_WwanStateLinkInfo  z_LinkInfo[CXM_MAX_SUPPORTED_LINK_SETS]; 
#endif

} gnss_WwanStateInfo;


/* This enum is for the WLAN blank control */
typedef enum
{
  GNSS_WLAN_BLANK_DISABLE = 0,
  GNSS_WLAN_BLANK_ENABLE_UNCONDITIONALLY,
  GNSS_WLAN_BLANK_ENABLE_CONDITIONALLY,
  GNSS_WLAN_BLANK_OVERRIDE_WITH_IMD,
  GNSS_WLAN_BLANK_MAX = GNSS_WLAN_BLANK_OVERRIDE_WITH_IMD
} gnss_WlanBlankEnum;


/* Sucess/error codes for WLAN IMD API */
typedef enum
{
  GNSS_WLAN_IMD_API_UNDEFINED = 0,
  GNSS_WLAN_IMD_API_SUCCESS = 1,
  GNSS_WLAN_IMD_API_PARAM_ERROR = 2,
  GNSS_WLAN_IMD_API_MISSION_CMD_IN_SIM_MODE = 3,
  GNSS_WLAN_IMD_API_ERROR_MISC = 4
} gnss_WlanImdRetCodeEnum;

/* This enum is for telling GNSS which WLAN type is being used */
typedef enum
{
  WLAN_BAND_UNKNOWN = 0,
  WLAN_BAND_2_POINT_4_GHZ, /* WLAN band 2.4GHz */
  WLAN_BAND_5_GHZ,  /* WLAN band 5GHz */
  WLAN_BAND_3_POINT_6_GHZ, /* 3.6Ghz band */
  WLAN_BAND_4_POINT_9_GHZ, /* 4.9GHz band - Public safety band */
  WLAN_BAND_MAX
} gnss_WlanBandEnumType;

/* This enum is for telling GNSS which TX antenna (Main or Aux) is being used */
typedef enum
{
  WWAN_TX_ANTENNA_MAIN = 0, /* Select this for the primary WWAN tx antenna */
  WWAN_TX_ANTENNA_AUX,  /* Select this for the secondary WWAN tx antenna */
  WWAN_TX_ANTENNA_MAX
} gnss_TxAntennaEnumType;

/* This enum represents the different IMD strategies that may be employed. */
typedef enum
{
  GNSS_IMD_NO_JAMMING = 0, /* No special Xcorr/ACI Checks required. */
  GNSS_IMD_XCORR_ACI_ADD = 1, /* GNSS Xcorr/ACI Addition */
  GNSS_IMD_SRCH_MOD = 2, /* Specific GNSS Search Strategy/ Job Mananger Modification */
  GNSS_IMD_SRCH_XCORR_ACI = 3, /* Both GNSS Srch Strategy/XCorr Modification */
  GNSS_IMD_IDLE = 4, /* IMD is forcing GNSS to be IDLE state */
  GNSS_IMD_XCORR_ACI_GPS_CNO_QUAL = 5, /* GNSS Xcorr/ACI, GPS C/No Qual */
  GNSS_IMD_STRATEGY_MAX = GNSS_IMD_XCORR_ACI_GPS_CNO_QUAL /* Max strategy value */
} gnss_IMDJammingType;


/* This enum specifies the GNSS Type */
typedef enum
{
  GNSS_TYPE_GPS = 0,
  GNSS_TYPE_GLO,
  GNSS_TYPE_LAST = GNSS_TYPE_GLO
} gnss_GnssType;


/* This enum is for the Bluetooth IMD Jamming status */
typedef enum
{
  GNSS_BLUETOOTH_IMD_JAMMING_DISABLE = 0,  /* Disable Bluetooth IMD Jamming*/
  GNSS_BLUETOOTH_IMD_JAMMING_ENABLE,      /* Enable Bluetooth IMD Jamming */
  GNSS_BLUETOOTH_IMD_JAMMING_MAX = GNSS_BLUETOOTH_IMD_JAMMING_ENABLE
} gnss_BluetoothImdEnum;

/*--------------------------------------------------------------------------
 * type gnss_BpAmplType
 * This structure holds the BP Amp information which is a linear (voltage) 
 * measure of the signal level present at the GPS digital front end.
 * -----------------------------------------------------------------------*/
typedef struct
{
  /* BpAmp I Component */
  uint32  q_BpAmplI;
  /* BpAmp Q Component */
  uint32  q_BpAmplQ; 
} gnss_BpAmplType;

/*--------------------------------------------------------------------------
 * type gnss_SimulateImdType
 * This structure is used to send parameters for IMD simulation mode.
 * -----------------------------------------------------------------------*/
typedef struct
{
  boolean             b_EnableIMD; /* Notifies if IMD needs to be enabled or not
                                      FALSE - Disable IMD; TRUE  - Enable IMD*/
  gnss_GnssType       e_GnssVictim; /* The GNSS Victim to be jammed.
                                      0 - GPS
                                      1 - GLONASS */
  gnss_IMDJammingType e_ImdStrategy; /* The IMD mitigation strategy to be used. */
  int16               x_ImdJammingPower; /* IMD Jamming power in dBm */
} gnss_SimulateImdType;


/*--------------------------------------------------------------------------
 * type gnss_RfConfig parameters
 * This structure contains GNSS RF configuration parameters from QMI 
 * 1) Bit mask of available data 
 * 2) GNSS peak antenna gain in tenth of dB-Hz ( -20 .. + 50 ) 
 * 3) GPS system processing loss in  tenth of dB-Hz ( typical : 3.0 dB_Hz ) 
 * 4) GLONASS system processing loss for frequencies -7,0, +6 in tenth of dB-Hz
 * 5) BDS system processing loss in  tenth of dB-Hz 
 * 6) GAL system processing loss in  tenth of dB-Hz 
 * -----------------------------------------------------------------------*/
#define GNSS_RF_CONFIG_PEAK_ANTENNA_GAIN  0x01
#define GNSS_RF_CONFIG_GPS_SPL            0x02
#define GNSS_RF_CONFIG_GLO_SPL            0x04
#define GNSS_RF_CONFIG_BDS_SPL            0x08
#define GNSS_RF_CONFIG_GAL_SPL            0x10

typedef struct
{
  uint8   u_GnssRfConfigBitMask; /* Available data */
  int16   x_GnssPeakAntennaGain;  /* GNSS Antenna peak gain in tenth of dB-Hz */
  uint16  w_GpsSpl;               /* GPS System processing loss in tenth of dB-Hz */
  uint16  w_GloSpl[3];            /* GLO SPL for frequencies -7,0, +6 in tenth of dB-Hz */
  uint16  w_BdsSpl;               /* BDS System processing loss in tenth of dB-Hz */  
  uint16  w_GalSpl;               /* GAL System processing loss in tenth of dB-Hz */
}gnss_RfConfigSplAntennaGainParamsType;

typedef enum {
  GNSS_INJECT_SENSOR_ASST_TIME_ENUM_TYPE_MIN_ENUM_VAL = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eGNSS_INJECT_SENSOR_ASST_UTC_TIME = 0, /**<  Time type is UTC, starting Jan1, 1970.   */
  eGNSS_INJECT_SENSOR_ASST_GPS_TIME = 1, /**<  Time type is GPS Time  */
  GNSS_INJECT_SENSOR_ASST_TIME_ENUM_TYPE_MAX_ENUM_VAL = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}gnss_InjectSensorAsstTimeEnum;

typedef enum
{
  GNSS_POSITION_RELIABILITY_NOT_SET = 0, /* Not set, reliability info is not present */
  GNSS_POSITION_RELIABILITY_VERY_LOW,    /* Not checked, use at user's own risk */
  GNSS_POSITION_RELIABILITY_LOW,         /* Limited cross-checked */
  GNSS_POSITION_RELIABILITY_MEDIUM,      /* Medium level cross-checked */
  GNSS_POSITION_RELIABILITY_HIGH,        /* Robust cross-checked */
  GNSS_POSITION_RELIABILITY_MAX = 0x10000000    /* Forced to 32 bit */
} gnss_ReliabilityEnum;

/* Data structure to pass on Course Over Ground information */
typedef struct
{
  gnss_InjectSensorAsstTimeEnum e_TimeType;
  /* Identifies the type of time being used. */

  uint64 t_TimestampMsec;
  /**<   Timestamp of the injected course over ground data. The reference
       time is as specified in the type.
       Units: milliseconds */

  float f_CourseOverGround;
  /**<   Course over ground specified as the clockwise horizontal azimuth angle. 
       Units: Degrees with respect to True North. 
              ( True North is 0 degrees and East is 90 degrees )
       Range: [0.0 - 360.00 ]
       Type: float*/

  float f_CourseOverGroundUnc;
  /**<   Course over ground uncertainty ( error estimate at 95 percent confidence )
       Units: Degrees
       Range: [0.01 - 180.00 ] 
       Type: float*/

  gnss_ReliabilityEnum e_Reliability;

}gnss_CourseOverGroundDataType;


/* Data structure to pass on External sensor assistance information */
typedef struct
{
  gnss_InjectSensorAsstTimeEnum e_TimeType;
  /* Identifies the type of time being used. */

  uint64 t_TimestampMsec;
  /**<   Timestamp of the injected Speed data. The reference
       time is as specified in the type.
       Units: milliseconds */

  float f_Speed;
  /**<   3D Speed injected. 
       Units: m/s 
       Range: [>= 0.0]
       Type: float*/

  float f_SpeedUnc;
  /**<   95 percentile Uncertainty of the 3d speed injected 
       Units: m/s
       Range: [ > 0.0 ] 
       Type: float*/


}gnss_ExternalSensorAssistDataType;


#define GNSS_RF_CONFIG_SUCESS   0x01
#define GNSS_RF_CONFIG_FAIL_IPC_MSG_DELIVARY  0x02
#define GNSS_RF_CONFIG_FAIL_RF_CONFIG_OUT_OF_RANGE  0x04

/* All the below values are in tenth of a dB-Hz */
#define GNSS_RF_CONFIG_PEAK_ANTENNA_GAIN_MIN   -20
#define GNSS_RF_CONFIG_PEAK_ANTENNA_GAIN_MAX   50

#define GNSS_RF_CONFIG_GPS_GLO_SPL_MAX          100

typedef struct
{
  uint16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;     /* GPS msec at reference tick [msecs] */
  float  f_ClkTimeBias; /* Clock bias [msecs]. Constrait to >-0.5 and <0.5 */
  float  f_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */

}gnss_SClkGpsTimeStructType;


typedef struct
{
  uint8                 u_FourYear;     /* GLO 4 year number from 1996 at the reference tick*/
  uint16                w_Days;         /* GLO day number in 4 years at the reference tick*/
  uint32                q_GloMsec;      /* GLO msec in the day at reference tick [msecs] */
  float                 f_ClkTimeBias;  /* Clock bias [msecs] */
  float                 f_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */
}gnss_SClkGloTimeStructType;

typedef struct
{
  uint8               u_DeltaLeapSec;    /* GPS time leap second delta to UTC time */
  uint8               u_DeltaLeapSecUnc; /* Uncertainty for GPS leap second */
  float                 f_GGTBMs;    /* GPS to Glonass Time Bias */
  float                 f_GGTBUncMs; /* Uncertainty for GPS to Glonass Time Bias */
  gnss_SClkGpsTimeStructType  z_GpsTime; /* GPS Time at w_RefFCount */
  gnss_SClkGloTimeStructType  z_GloTime; /*GLO Time at the reference counter */
}gnss_SClkTimeStructType;

typedef struct
{
  boolean             u_Valid;
  uint64              t_SystemRtcMs;    /* System RTC value */
  gnss_SClkTimeStructType  z_GnssTime;       /* GNSS clock structure */
}gnss_SlowClockStructType;

/*--------------------------------------------------------------------------
 * Function Declarations
 * -----------------------------------------------------------------------*/

/* -------------------------------------------------------------------------
 * gnss_RfLinearityPut
 *
 * Description: 
 *  This function allows the RF driver to report the RF linearity setting
 *  to the GNSS receiver. The GNSS receiver uses this information in 
 *  diagnostic logs. 
 *
 *  The RF driver should dispatch this function whenever the RF linearity
 *  is programmed. (ie - At RF initialization and when the linearity is 
 *  switched).
 *
 * Parameters:
 *  Pointer to gnss_RfLinearityType that captures new linearity setting
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfLinearityPut( const gnss_RfLinearityEnum );

/* -------------------------------------------------------------------------
 * gnss_RfSynthModifyPut
 *
 * Description: 
 *  This function allows the RF driver to report the RF synthesizer
 *  modification events to the GNSS engine. 
 *
 *  The problems encountered with the RTR6285 and Ramsis RF solutions 
 *  will likely require that the RF Driver reprogram the GPS VCO to 
 *  address. A side effect of programming the GPS synthesizer is that a 
 *  180 degree carrier phase ambiguity may be imposed.
 * 
 *  The GNSS SW can takes steps to accommodate the carrier phase ambiguity, 
 *  but needs to know the GNSS RTC at the moment of modification.
 *
 * Parameters:
 *  None
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfSynthModifyPut( void );

/* -------------------------------------------------------------------------
 * gnss_RfMeasConfigPut
 *
 * Description: 
 *  The baseband analog signal may have a large
 *  DC offset due to the mixer circuits within the RF component.
 *  It is mandatory to compensate most of the DC offset befreo the SDM
 *  interface. 
 *
 *  This function provides an interface for RF drivers to enable and 
 *  configure the measurement period for the preliminary mean and 
 *  amplitude estimators. This triggers to the start and stop of 
 *  mean/amplitude estimation period. RF drivers will then read the 
 *  mean estimator to correct large DC offsets.  This setting applies 
 *  to all four sync and async ADC processors.  In the fixed integration 
 *  mode, the integration period is always 1ms. 
 *
 * Parameters:
 *  gnss_MeanMonConfigType that captures new linearity setting
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfMeasConfigPut
( 
  const gnss_MeanMonConfigEnum  e_Config
);

/* -------------------------------------------------------------------------
 * gnss_RfMeasCmdPut
 *
 * Description: 
 *  The baseband analog signal may have a large
 *  DC offset due to the mixer circuits within the RF component.
 *  It is mandatory to compensate most of the DC offset befreo the SDM
 *  interface. 
 *
 *  This function provides an interface for RF drivers to enable and 
 *  configure the measurement period for the preliminary mean and 
 *  amplitude estimators.  This triggers the start and stop of 
 *  mean/amplitude estimation period. RF drivers will then read the 
 *  mean estimator to correct large DC offsets.  This setting applies 
 *  to all four sync and async ADC processors.  
 *
 *  The RF driver calling sequence for mean estimation is: 
 *  1. Start the mean estimation with a call to 
 *     gnss_RfcMeasEnablePut() with gnss_RfMeasCmdType = MEAS_START; 
 *  2. Wait (RF drivers timer, in MC task context)
 *  3. Stop the mean estimation with a call to 
 *     gnss_RfMeasEnablePut() with gnss_RfMeasCmdType = MEAS_STOP; 
 *  4. Read the mean estimates with a call to gnss_RfAdcProcMeanMonGet().
 *
 *  In the fixed integration mode, the integration period is always 1ms, 
 *  and calls to this API are then unnecessary.
 *
 * Parameters:
 *  gnss_RfMeasCmdType - Start or Stop measurement.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfMeasCmdPut
( 
  const gnss_RfMeasCmdEnum e_MeasCmd
);

/* -------------------------------------------------------------------------
 * gnss_RfAdcProcMeanMonGet
 *
 * Description: 
 *  The baseband analog signal may have a large
 *  DC offset due to the mixer circuits within the RF component.
 *  It is mandatory to compensate most of the DC offset befreo the SDM
 *  interface. 
 *
 *  This function provides an interface for RF drivers to query the 
 *  I/Q means of the digitized  IF signal, before any downconversion. 
 *  MC is responsible for mean values from the bit fields AD_PREMEANMONI/Q.
 *  To do this calculation, Callisto must check the bit fields AD_MEAS_PERIOD 
 *  and AD_MEAS_BLANK.
 *
 *  In the asynchronous mean estimation mode (AD_MEAS_PERIOD = 0), the bit 
 *  field AD_MEAS_LENGTH contains the number of samples collected to form the 
 *  mean estimate.  Assuming AD_MEAS_BLANK = 0, the accumulated sums in 
 *  AD_PREMEANMONI/Q must be divided by AD_MEAS_LENGTH to give mean estimates.
 *
 *  If AD_MEAS_BLANK = 1, then the mean estimate is invalid.  Since there is 
 *  no way of knowing how many samples were blanked, this means that variable 
 *  integration for mean/amplitude estimation cannot be used in GSM mode.
 *
 *
 *  The RF driver calling sequence for mean estimation is: 
 *  1. Start the mean estimation with a call to 
 *     gnss_RfcMeasEnablePut() with gnss_RfMeasCmdType = MEAS_START; 
 *  2. Wait (RF drivers timer, in MC task context)
 *  3. Stop the mean estimation with a call to 
 *     gnss_RfMeasEnablePut() with gnss_RfMeasCmdType = MEAS_STOP; 
 *  4. Read the mean estimates with a call to gnss_RfAdcProcMeanMonGet().
 *
 *  In the fixed integration mode, the integration period is always 1ms, 
 *  and calls to this API are then unnecessary.
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc 1 or 2, sync ADC proc 1 or 2).
 *
 *  gnss_AdcProcMeanMonType - Gets the status registers for either the 
 *                            async or sync ADC processing block that 
 *                            monitors the I/Q mean.Function must be 
 *                            called as part of GPS RF turn on processing
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfAdcProcMeanMonGet
( 
  const gnss_AdcProcEnum   e_AdcProc,
  gnss_MeanMonType         *p_AdcProcMeanMon
);


/* -------------------------------------------------------------------------
 * gnss_RfAdcProcConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to configure bit 
 *  fields in each BP.  RF drivers will call this function once on 
 *  narrowband mode entry or once on diversity mode entry. The RF driver 
 *  is controlling the mapping of ADC processor to BP.  So the driver may 
 *  have a redirection layer that is RF card dependent. Function must be 
 *  called as part of GPS RF turn on processing.
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc primary or diversity
 *                      or sync ADC proc 1 or 2).
 *
 *  gnss_AdcProcConfigPutType - Sets the static controls for either the 
 *                              async or sync ADC processing block. 
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfAdcProcConfigPut
( 
  const gnss_AdcProcEnum     e_AdcProc,
  gnss_AdcProcConfigPutType  *p_AdcProcConfigPut
);

/* -------------------------------------------------------------------------
 * gnss_RfAdcProcIqCntrlPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to configure bit 
 *  fields in each BP.  RF drivers will call this function once on 
 *  narrowband mode entry, wideband entry, or once on diversity mode entry.
 *  These bit fields could change dynamically.  RF drivers might need 
 *  different settings in high and low linearity. 
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc primary or diversity
 *                      or sync ADC proc 1 or 2).
 *
 *  gnss_AdcProcIqCntrlType - Sets the static controls for either the 
 *                            async or sync ADC processing block for 
 *                            swapping I/Q or sign inverting I/Q data. 
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfAdcProcIqCntrlPut
( 
  const gnss_AdcProcEnum     e_AdcProc,
  gnss_AdcProcIqCntrlType    *p_IqCntrl
);

/* -------------------------------------------------------------------------
 * gnss_RfAdcProcBlankConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to configure the 
 *  blanking configuration, such as the enable/disable.  RF API must call 
 *  this API for primary and diversity separately. There are two blanking 
 *  controls which map to either the primary or diversity path selection.
 *  MC will map the e_AdcProc to the correct blanking control path 
 *  (either primary or diversity path ADC processor).
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc primary or diversity
 *                      or sync ADC proc 1 or 2).
 *
 *  gnss_AdcProcBlankConfigType - Sets the static control for blanking.
 *                                (Does not blank the signal)
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfAdcProcBlankConfigPut
( 
  const gnss_AdcProcEnum         e_AdcProc,
  gnss_AdcProcBlankConfigType    *p_BlankConfig
);


/* -------------------------------------------------------------------------
 * gnss_RfAdcProcBlankPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to start software 
 *  blanking, which takes effect immediately. The intelliceiver calls this 
 *  API during transitions.  RF API must call this API for primary and 
 *  diversity separately. There are two blanking controls which map to 
 *  either the primary or diversity path selection.  MC will map the 
 *  e_AdcProc to the correct blanking control path 
 *  (either primary or diversity path ADC processor).
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc primary or diversity
 *                      or sync ADC proc 1 or 2).
 *
 *  boolean u_BlankSig - Blank signal command. This command is 
 *                       ORed with the blank signal input to the core.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfAdcProcBlankPut
( 
  const gnss_AdcProcEnum   e_AdcProc,
  boolean                  u_BlankSig
);



/* -------------------------------------------------------------------------
 * gnss_RfBpConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to configure bit 
 *  fields in each BP.  RF drivers will call this function once on 
 *  narrowband mode entry, once on diversity mode entry, and three times 
 *  on wideband mode entry (NB, WB, Glonass BPs).
 *  
 *  Callisto software should be in control of certain bit fields in each BP.
 *  For NB and diversity BP, the bit field BP[1-2]_SCH[1-3]_EN controls this 
 *  option. The wideband BP does not have subchannels but uses the bit field 
 *  BP4_MODE to select from several matched filter options. The Glonass BP 
 *  has 12 subchannels, each with controls for the fine rotator frequency 
 *  (BP3_SCH[1-12]_FREQ) and matched filter selection. (BP3_SCH[1-12]_MODE).
 *
 *  MC must ensure that all BP clocks have been enabled before writing to 
 *  the appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpConfigPutType - Sets the static controls for the BP specified 
 *                         by the first parameter 
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpConfigPut
( 
  const gnss_BpEnum       e_Bp,
  gnss_BpConfigPutType    *p_Config
);


/* -------------------------------------------------------------------------
 * gnss_RfBpIfDownConvPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to set the coarse 
 *  rotator frequency in the BPs. The RF driver shall convert the desired 
 *  frequency shift in Hz to the frequency command format.
 *
 *  Note: The baseband processors provides the same capability, but is 
 *  bypassed when using the auxiliary inputs.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpIfDownConvType - Sets the IF down converter frequency 
 *                          and type (LIF or ZIF)
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpIfDownConvPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpIfDownConvType    *p_IfDownConv
);


/* -------------------------------------------------------------------------
 * gnss_RfBpCarrierPhasePut
 *
 * Description: 
 *  This function allows RF drivers to program the baseband processor 
 *  coarse rotator for a constant phase shift.  For example, the input 
 *  argument might be +10 degrees and MC will then shift the phase by 
 *  -10 degrees in the baseband processor.  
 *
 *  NOTE: This support must be added in next revision of Callisto Hardware
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  float f_PhaseShift - Carrier phase shift in degrees
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpCarrierPhasePut
( 
  const gnss_BpEnum        e_Bp,
  float                    f_PhaseShift
);

/* -------------------------------------------------------------------------
 * gnss_RfBpEqCoefPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to write the complex 
 *  equalizer coefficients in the BPs.  These coefficients can change 
 *  dynamically, for example in high and low linearity.
 *
 *  MC must ensure that all BP clocks have been enabled before writing 
 *  to the appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpEqCoefType - Structure containing the settings for the FIR 
 *                      complex equalizer with programmable coefficients. 
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpEqCoefPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpEqCoefType        *p_EqCoef
);

/* -------------------------------------------------------------------------
 * gnss_RfBpGroupDelayPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to set the group 
 *  delay jump in nanoseconds. The group delay input argument is a jump 
 *  relative to low-linearity mode, not an absolute jump. If the input 
 *  argument is +10ns, then BP_INTDELAY and BP_FRACDELAY bit fields should 
 *  be programmed to ?10ns.
 *
 *  For example:
 *  1. Enter low linearity mode: gnss_RfGroupDelayPut(0ns).
 *  2. Enter high linearity mode: gnss_RfGroupDelayPut(50ns) if RF delay 
 *     is 50ns greater in high linearity than in low linearity. The input 
 *     argument is the jump relative to low linearity mode, not the absolute 
 *     jump.
 *
 *  Group delay differences being corrected through this API:
 *  1. Any jumps due to change of sampling rate when Intelliceiver state 
 *     changes.
 *  2. Difference in RF/analog delay between wideband and narrowband modes.
 *  3. Difference in RF/analog delay between primary NB and diversity paths.  
 *
 *  MC must ensure that all BP clocks have been enabled before writing to the 
 *  appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_RfGrpDelay - Group Delay in Nanoseconds, either HW or SW compensation
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpGroupDelayPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_RfGrpDelayType      *p_GrpDelay
);

/* -------------------------------------------------------------------------
 * gnss_RfBpNotchFltrConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to the notch filters 
 *  (up to six filters) in each BP. MC will convert the frequency in Hz into 
 *  the correct command register format.
 *
 *  MC must ensure that all BP clocks have been enabled before writing to 
 *  the appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpNotchFltrType - Sets the notch filter bandwidth, center 
 *                         frequency and mode.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpNotchFltrConfigPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpNotchFltrType     *p_NotchFltr
);

/* -------------------------------------------------------------------------
 * gnss_RfBpPreNotchFltrConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to the pre notch
 *  filter in each BP. MC will convert the frequency in Hz into 
 *  the correct command register format.
 *
 *  MC must ensure that all BP clocks have been enabled before writing to 
 *  the appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpPreNotchFltrType - Sets the pre notch filter bandwidth, center 
 *                         frequency and mode.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpPreNotchFltrConfigPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpPreNotchFltrType     *p_PreNotchFltr
);

/* -------------------------------------------------------------------------
 * gnss_RfBpMeanEstPut
 *
 * Description: 
 *  Provides write access to configure the mean estimator block such as BP 
 *  bit fields BP_DCALIGN, BP_OFFSETI, and BP_OFFSETQ to allow software 
 *  control of the DC filter.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpMeanEstType - Structure that stores the mean estimate 
 *                       configuration
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpMeanEstPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpMeanEstType       *p_MeanEst
);


/* -------------------------------------------------------------------------
 * gnss_RfBpPreAmplGet
 *
 * Description: 
 *  This function provides an interface for RF drivers to get the value 
 *  of preliminary amplitude monitor BP registers.  The amplitude estimates 
 *  will be used for coarse gain trimming in the RFIC. The integration period 
 *  is controlled by bit field AD_MEAS_PERIOD and AD_MEAS_ENABLE in the ADC 
 *  processing block. MC should scale the accumulator value.  
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpPreAmplMonType - Query the current register values for 
 *                             preliminary amplitude estimates.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfBpPreAmplGet
( 
  const gnss_BpEnum             e_Bp,
  gnss_BpPreAmplMonType         *p_PreAmplMon
);


/* -------------------------------------------------------------------------
 * gnss_RfAdcProcAmplMonGet
 *
 * Description: 
 *  This function provides an interface for RF drivers to query the 
 *  I/Q means of the digitized  IF signal, before any downconversion. 
 *
 *  The RF driver calling sequence for mean estimation is: 
 *  1. Start the mean estimation with a call to 
 *     gnss_RfcMeasEnablePut() with gnss_RfMeasCmdType = MEAS_START; 
 *  2. Wait (RF drivers timer, in MC task context)
 *  3. Stop the mean estimation with a call to 
 *     gnss_RfMeasEnablePut() with gnss_RfMeasCmdType = MEAS_STOP; 
 *  4. Read the mean estimates with a call to gnss_RfAdcProcAmplMonGet().
 *
 *  In the fixed integration mode, the integration period is always 1ms, 
 *  and calls to this API are then unnecessary.
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block (sync ADC proc 1 or 2)
 *
 *  gnss_AdcProcAmplMonType - Gets the mean magnitude estimates of the
 *                            signal
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfAdcProcAmplMonGet
( 
  const gnss_AdcProcEnum   e_AdcProc,
  gnss_AdcProcAmplMonType  *p_AdcProcAmplMon
);



/* -------------------------------------------------------------------------
 *  gnss_RfStatusReportPut
 *
 * Function description:
 *
 *  Stores the data reported by the RF driver to be logged later.
 *
 * Parameters: 
 *  gnss_RfStatusType - data reported
  *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfStatusReportPut
(
  gnss_RfStatusType          *p_RfReport
);

/* -------------------------------------------------------------------------
 *  gnss_RfDynamicStatusReportPut
 *
 * Function description:
 *
 *  Stores the data reported by the RF driver to be logged later.
 *
 * Parameters: 
 *  gnss_RfDynamicStatusType - data reported
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfDynamicStatusReportPut
(
  gnss_RfDynamicStatusType          *p_RfDynamicReport
);

/* -------------------------------------------------------------------------
 *  gnss_RfSamplClkRatePut
 *
 * Function description:
 *
 *  This function configures the sample clock source MUX when using the 
 *  synchronous ADC processor.
 *
 * Parameters: 
 *
 *   gnss_AdcProcSampleRateEnum d_SamplRate - Desired sample clock 0 rate
 *   (chipx80/chipx40/chipx20/off)
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfSamplClkRatePut
(
  gnss_AdcProcSampleRateEnum d_SamplRate
);

void gnss_RfBPStartInitMode( gnss_RfModeEnum gnss_rf_mode_selected );

/*============================================================================
FUNCTION - gnss_RfBPStopInit

DESCRIPTION
    Function for rf sw to call which turns of clocks needed for all BPs. 

    Note: This is called at boot up in TMC ask context when 
          other tasks are not yet started.

    This function stops all the clocks started in gnss_RfBPStartInit 
    effectively disabling all BPs

RETURN VALUE

DEPENDENCIES

LIMITATIONS

=============================================================================*/
extern void gnss_RfBPStopInit(void);

/*
 ******************************************************************************
 * Function gnss_GpsHWVersionGet
 *
 * Description:
 *
 *  This function attempts to get Callisto HW version number. If the clks are 
 *  turned OFF because of which the version number could not be obtained, a FALSE
 *  boolean value is returned. Else, TRUE is returned and the version number is
 *  populated in the pointer variable sent as parameter.
 *
 * Parameters: 
 *
 *  uint8* Pointer variable to store the HW version number.
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  boolean - TRUE if the clocks were ON, FALSE otherwise
 *
 ******************************************************************************
*/

extern boolean gnss_GpsHWVersionGet( uint8* p_GpsHWVersionNum );

/* -------------------------------------------------------------------------
 * gnss_RfLTEB13TxStatus
 *
 * Description: 
 *
 *  This function provides an interface for RF drivers to notify GNSS SW
 *  on when LTE B13 has started to transmit or stopped.
 *
 * Parameters:
 *  u_TxOn True - LTE B13 started transmitting
 *         False - LTE B13 has stopped transmitting
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
extern void gnss_RfLTEB13TxStatus ( boolean u_TxOn );


/* -------------------------------------------------------------------------
* gnss_RfWwanStatus
*
* Description: 
*  This function provides an interface for RF drivers to notify the
*  GPS engine the status of the current WWAN technology.  This should be
*  called prior to the RF tuning to the band/channel.
*
* Parameters:
*  e_CurWwan - Current active WWAN Technology
*  e_Band - Use to set band when e_CurWwan is not GNSS_WWAN_STATE_WCDMA. 
*           In this case, e_WCDMABand will be invalid.
*  e_WCDMABand - Use to set band when e_CurWwan is GNSS_WWAN_STATE_WCDMA.
*                In this case, e_Band will be invalid.
*  q_Channel - Channel in Mhz
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_RfWwanStatus
( 
  gnss_WwanStateEnum       e_CurWwan,
  gnss_RfCardBandEnum      e_Band,
  gnss_RfcomWcdmaBandEnum  e_WCDMABand,
  uint32                   q_Channel
);
 

/* -------------------------------------------------------------------------
* gnss_ExternalWLanStatus
*
* Description: 
*  This function provides an interface for GNSS to be notified of the current
*  WLan (WiFi) status.   This should be called prior to the WLan tuning to the
*  new band/channel.
*
* Parameters:
*  e_BlankStatus - See gnss_WlanBlankEnum
*  q_Channel - WLan Channel - BitMask of the channels Valid bits 0 to 13
*                             {0x00000000 to 0x00003FFF}
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_ExternalWLanStatus
( 
  gnss_WlanBlankEnum       e_BlankStatus,
  uint32                   q_Channel
);


/* -------------------------------------------------------------------------
* gnss_WLanAttachStatusPut
*
* Description: 
*  This function provides an interface for GNSS to be notified of the current
*  WLan (WiFi) attachment status. This should be called prior to the WLAN tuning
*  to the new band/channel. This functions assumes that WLAN is on 2.4GHz band.
*
* Parameters:
*  b_isWlanAttached - WLAN Attach status
*                       TRUE  - WLAN attached
*                       FALSE - WLAN detached
*  u_WlanChannel    - The WLAN channel number to which the device is attached
*                       Valid Range: 1 - 14
*                       Default: 0xFF, when unknown or WLAN is detached
*
* Return value: 
*  TRUE if parameters were accepted for processing, FALSE otherwise
* -----------------------------------------------------------------------*/
extern boolean gnss_WLanAttachStatusPut
( 
  boolean  b_isWlanAttached,
  uint8    u_WlanChannel
);


/* -------------------------------------------------------------------------
* gnss_WlanAttachStatusPutExt
*
* Description: 
*  This function provides an interface for GNSS to be notified of the current
*  WLan (WiFi) attachment status. This should be called prior to the WLAN tuning
*  to the new band/channel.
*
* Parameters:
*  b_isWlanAttached - WLAN Attach status
*                       TRUE  - WLAN attached
*                       FALSE - WLAN detached
*  e_WlanBand       - The WLAN band to which the device is attached.
*                          Valid Values: 0 - Unknown
*                                        1 - 2.4GHz
*                                        2 - 5GHz
*                                        3 - 3.6GHz
*                                        4 - 4.9GHz(Public Safety band)
*  u_WlanChannelNum - The WLAN channel number to which the device is attached.
*                       Valid Range: 1 - 14 for 2.4 Ghz band
*                                    1 - 196 for 5 GHz band
*                       Default: 0xFF, when unknown or WLAN is detached
*  u_WlanChannelBw  - The bandwidth of the WLAN Channel in Mhz.
*                     Valid Range:  5-40 MHz
*
* Return value: 
*  0x1 - Success; Parameters were accepted for processing.
*  0x2 - Parameters are invalid error
*  0x3 - Regular attach command in Simulation mode error
* -----------------------------------------------------------------------*/
extern gnss_WlanImdRetCodeEnum gnss_WlanAttachStatusPutExt
( 
  boolean                  b_isWlanAttached,
  gnss_WlanBandEnumType    e_WlanBand,
  uint8                    u_WlanChannelNum,
  uint8                    u_WlanChannelBw
);


/* -------------------------------------------------------------------------
* gnss_SimulateWLanAttachAndTx
*
* Description: 
*  This function provides an interface for GNSS to be notified of the current
*  WLan (WiFi) attachment status. This should be called prior to the WLAN tuning
*  to the new band/channel.This functions assumes that WLAN is on 2.4GHz band.
*
* Parameters:
*  b_isWlanAttached - WLAN Attach status
*                       TRUE  - WLAN attached
*                       FALSE - WLAN detached
*  u_WlanChannel    - The WLAN channel number to which the device is attached
*                       Valid Range: 0 - 13
*                       Default: 0xFF, when unknown or WLAN is detached
*  b_TxPinAssert    - State of WLAN Tx Pin to be simulated.
*                       0 - Tx pin Deasserted 
*                       1 - Tx pin Asserted
*
* Return value: 
*  TRUE if parameters are valid, FALSE if parameters are invalid
* -----------------------------------------------------------------------*/
extern boolean gnss_SimulateWLanAttachAndTx
( 
  boolean  b_isWlanAttached,
  uint8    u_WlanChannel,
  boolean  b_TxPinAssert
);


/* -------------------------------------------------------------------------
* gnss_SimulateWLanAttachAndTx
*
* Description: 
*  This function provides an interface for GNSS to be notified of the current
*  WLan (WiFi) attachment status and the status of WLan Tx simulation.
*  This should be called prior to the WLAN tuning to the new band/channel.
*
* Parameters:
*  b_isWlanAttached - WLAN Attach status
*                       TRUE  - WLAN attached
*                       FALSE - WLAN detached
*  e_WlanBand       - The WLAN band to which the device is attached.
*                          Valid Values: 0 - Unknown
*                                        1 - 2.4GHz
*                                        2 - 5GHz
*                                        3 - 3.6GHz
*                                        4 - 4.9GHz(Public Safety band)
*  u_WlanChannelNum - The WLAN channel number to which the device is attached.
*                       Valid Range: 1 - 14 for 2.4 Ghz band
*                                    1 - 196 for 5 GHz band
*                       Default: 0xFF, when unknown or WLAN is detached
*  u_WlanChannelBw  - The bandwidth of the WLAN Channel in Mhz.
*                     Valid Range:  5-40 MHz
*  b_TxPinAssert    - State of WLAN Tx Pin to be simulated.
*                       0 - Tx pin Deasserted 
*                       1 - Tx pin Asserted
*
* Return value: 
*  0x1 - Success; Parameters were accepted for processing.
*  0x2 - Parameters are invalid error
*  0x3 - Regular attach command in Simulation mode error
* -----------------------------------------------------------------------*/
extern gnss_WlanImdRetCodeEnum gnss_SimulateWLanAttachAndTxExt
( 
  boolean                  b_isWlanAttached,
  gnss_WlanBandEnumType    e_WlanBand,
  uint8                    u_WlanChannelNum,
  uint8                    u_WlanChannelBw,
  boolean                  b_TxPinAssert
);


/* -------------------------------------------------------------------------
* gnss_SimulateImdJamming
*
* Description: 
*  This function provides an interface for GNSS to simulated IMD Jamming
*  This function simulates 12 second of GPS Jamming followed by 
*  12 seconds of GLONASS jamming. 
*
* Parameters:
    None
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_SimulateImdJamming
( 
  void
);



/* -------------------------------------------------------------------------
* gnss_DisableSimulatedImdJamming
*
* Description: 
*  This function disables simulated IMD Jamming.
*
* Parameters:
    None
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_DisableSimulatedImdJamming
( 
  void
);


/* -------------------------------------------------------------------------
*
*  gnss_SimulateImd
*
* Description: 
*   This functions provides a GNSS interface to simulate an IMD Jamming scenario.
*
* Parameters:
*  p_SimulateImdParams - Pointer to a structure that has configuration 
*                         parameters needed for IMD simulation.
*
* Return value: 
*  TRUE if parameters are valid, FALSE if parameters are invalid
* -----------------------------------------------------------------------*/
extern boolean gnss_SimulateImd
( 
  gnss_SimulateImdType *p_SimulateImdParams
);



/* -------------------------------------------------------------------------
* gnss_BluetoothImdJammingStatus
*
* Description: 
*  This function provides an interface for GNSS to be notified of the current
*  Bluetooth IMD Jamming status.   This should be called prior to Bluetooth
*  being turned ON/OFF.
*
* Parameters:
*  e_ImdJammingStatus -  see gnss_BluetoothImdEnum
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_BluetoothImdJammingStatus
( 
  gnss_BluetoothImdEnum       e_BtImdJammingStatus
);


/*
 ******************************************************************************
 * Function gnss_RfAdcConfigPut
 *
 * Description:
 *
 *  This function is used to write to ADC_REG* in GNSS_ADC_CMD* registers.
 *
 * Parameters: 
 * 
 *  gnss_RfAdcRegEnum - Enum for ADC register inside ADC Command register
 *  uint8 u_Bitmask - Bitmask 
 *  uint8 u_Value - Register Value to be written
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

extern void gnss_RfAdcConfigPut
( 
  gnss_RfAdcRegEnum e_AdcReg,
  uint8 u_Bitmask,
  uint8 u_Value 
);

/*
 ******************************************************************************
 * Function gnss_RfAdcConfigGet
 *
 * Description:
 *
 *  This function is used to read to ADC_REG* in GNSS_ADC_CMD* registers.
 *
 * Parameters: 
 * 
 *  gnss_RfAdcRegEnum - Enum for ADC register inside ADC Command register
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value:
 * 
 *  boolean - TRUE if RTC is running
 *            FALSE otherwise
 * 
 *  uint8 * p_Value - Pointer to value of ADC register
 *
 *
 ******************************************************************************
*/

extern boolean gnss_RfAdcConfigGet
( 
  gnss_RfAdcRegEnum e_AdcReg,
  uint8 * p_Value 
);

/* -------------------------------------------------------------------------
* gnss_MgpCcReset
*
* Description: 
*  This function provides an interface for pausing and resuming GPS. The 
*  lowest layer in the GPS stack (Chan Controller) and the layer above it 
*  (meas controller) are paused and resumed. Upon returning from this function
*  (after several ms) CC and MC are guaranteed to be in pause state. A few ms
*  later, the resume will automatically trigger. CC flushes the data mover
*  during the pause.
*
* Parameters:
*  None
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_MgpCcReset( void );

/* -------------------------------------------------------------------------
 * gnss_SetLowErrEst
 *
 * Description: 
 *  This function sets the aggressively low error estimate in the low 
 *  measurement count scenario via the LocMW interface.
 *
 * Parameters: 
 *  q_LowErrEstCfg - Used to set the low error estimate 
 *
 * Dependencies:
 *  None
 *
 * Return value: 
 *  boolean - TRUE if the setting is successfully queued. FALSE otherwise.
 * -----------------------------------------------------------------------*/
extern boolean gnss_SetLowErrEst ( uint32 q_LowErrEstCfg );

/* Grp Delay in milliseconds for all Glonass channels
   0th index is for Freq -7
   13th index is for Freq 6
*/
typedef struct
{
  float f_GroupDelayMs[N_MAX_GLO_FREQ];
} gnss_GloGrpDelayType;

/* -------------------------------------------------------------------------
* gnss_RfBpGloGroupDelayPut
*
* Description: 
*  This function provides an interface for GNSS to be provided
*  the group delay compensation info for all GLO channels
*
* Parameters:
*  p_GrpDelay - Grp Delay for all Glonass channels
*                   0th index is for Freq -7
*                   13th index is for Freq 6
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_RfBpGloGroupDelayPut
(
  gnss_GloGrpDelayType *p_GrpDelay
);

/* -------------------------------------------------------------------------
* Cell database control structure definitions
* -----------------------------------------------------------------------*/
/* Definition of CELLDB Control Structure */
#define GNSS_CELLDB_CONTROL_ENABLE_MASK_POS    0x00000001
#define GNSS_CELLDB_CONTROL_ENABLE_MASK_TIME   0x00000002

typedef struct
{
  boolean b_EnableMaskIsValid;    /* TRUE if q_EnableMask contains valid data */
  uint32  q_EnableMask;           /* See mask definitions above */
} gnss_CellDbControlStructType;


/* -------------------------------------------------------------------------
* gnss_RfWwanStatus
*
* Description: 
*  This function provides an interface for RF drivers to notify the
*  GPS engine the status of the current WWAN technology.  This should be
*  called prior to the RF tuning to the band/channel.
*  This function will make gnss_RfWwanStatus() obsolete.
*
* Parameters:
*  e_CurWwan - Current active WWAN Technology
*  e_Band - Union of Band information.  
*           When e_CurWwan is GNSS_WWAN_STATE_WCDMA, use e_WCDMABand. 
*           When e_CurWwan is GNSS_WWAN_STATE_LTE, use e_LTEBand. 
*           All else, use e_RfBand.
*  q_Channel - Channel number
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_RfWwanInfo
( 
  gnss_WwanStateEnum       e_CurWwan,
  gnss_RfBandType          e_Band,
  uint32                   q_Channel
);



/* -------------------------------------------------------------------------
* gnss_RfWwanTxInfo
*
* Description: 
*  This function provides an interface for RF drivers to notify the
*  GPS engine if the current WWAN is in TX or not.
*
*  u_TxOn shall not be deasserted when the Tx is turned off briefly while 
*  inn the connected state. Examples include:
*    IS-95 puncturing
*    WCDMA compressed mode
*    GSM Rx/idle slots
*    HSPA Tx gating (DTx) during continuous packet connections
*    Hard hand-offs
*
* Parameters:
*  u_TxOn - TRUE: WWAN is in TX
*           FALSE: WWAN is NOT in TX
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_RfWwanTxInfo
( 
  boolean u_TxOn
);


/* -------------------------------------------------------------------------
* gnss_RfAdcClockControl
*
* Description: 
*  This function provides an interface for RF drivers to enable/disable
*  the GNSS ADC Clock
*
* Parameters:
*  u_Enable : TRUE - turn on ADC Clock
*             FALSE - turn off AD Clock
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
extern void gnss_RfAdcClockControl
( 
  boolean u_Enable
);


/*
******************************************************************************
* gnss_UpdateWwanTxAntennaStatus
*
* Function description:
*
*  gnss_UpdateWwanTxAntennaStatus is used to update the status of which
*  TX Antenna is being used for WWAN transmissions. This antenna selection
*  control is needed to estimate the impact of the WLAN and WWAN Tx on GPS.
*  Different antennas provide different levels of isolation from the GPS band.
*  Based on the antenna used, GNSS receiver selects appropriate thresholds
*  for cross-correlation, or IMD technique to mitigate the impact.
*
*  This function should be called at start of a Receiver bootup, and whenever the 
*  antenna selection is changed. 
*  The function updates a GNSS global variable to set the Tx Antenna status
*  and returns immediately without performing any further processing.
*  Note: If the device has only a single antenna for WWAN Tx, this API should 
*        be called with "WWAN_TX_ANTENNA_MAIN" as the antenna type.
*
* Parameters:
*
*  e_TxAntUsed - Indicates which Tx Antenna is selected.
*
* Return value:
*
*  void
*
******************************************************************************
*/
void gnss_UpdateWwanTxAntennaStatus( gnss_TxAntennaEnumType e_TxAntUsed );


/* -------------------------------------------------------------------------
* gnss_RfBpAmplGet
*
* Description: 
*  This function provides an interface for RF drivers to get the current bp
*  amp information from the GPS baseband processor.
*
* Parameters:
*  gnss_BpEnum - Select one of four BPs
*                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
*  p_BpAmpInfo - Bp Amp information 
*
* Return value: 
*  True - valid BP amp reading
*  False - invalide Bp amp reading (i.e Gps engine is off)
* 
* -----------------------------------------------------------------------*/
extern boolean gnss_RfBpAmplGet
(
  const gnss_BpEnum  e_Bp,
  gnss_BpAmplType    *p_BpAmpInfo
);

/* -------------------------------------------------------------------------
* gnss_ConfigRfSplAntGain
*
* Description: 
* This function provides an inteface to configure the following RF params
* 1) GNSS peak antenna gain in tenth of dB-Hz
* 2) GPS SPL ( system processing loss ) in tenth of dB-Hz
* 3) GLO SPL ( system processing loss ) for GLO frequencies -7,0,+6

* Parameters:
*  p_GnssRfConfig - Pointer to RF configuration
*
* Return value: 
*  True - RF configuration successfully sent to Measuerment Engine.
*  False - Unable to send RF configuration to Measurement Engine.
* 
* -----------------------------------------------------------------------*/
uint8 gnss_ConfigRfSplAntGain
(
  const gnss_RfConfigSplAntennaGainParamsType *p_RfConfig
);

/* -------------------------------------------------------------------------
* gnss_InjectCourseOverGroundData
*
* Description: 
* This function provides an interface to inject the course over ground 
* information to the Position Engine

* Parameters:
*  pz_CogData - Pointer to Course Over Ground information 
*
* Return value: 
*  True - COG data is valid and successfully sent to Position Engine.
*  False - Unable to send COG data to Position Engine, or it is invalid
* 
* -----------------------------------------------------------------------*/
boolean gnss_InjectCourseOverGroundData
(
 const gnss_CourseOverGroundDataType *pz_CogData
);

/* -------------------------------------------------------------------------
* gnss_InjectExternalSensorAssistanceData
*
* Description: 
* This function provides an interface to inject the external sensor  
* assitance information to the Position Engine
* Parameters:
*  pz_ExtSensorAsstData - Pointer to external sensor asstistance information 
*
* Return value: 
*  True - Ext Sensor data is valid and successfully sent to Position Engine
*  False - Unable to send Ext Sensor data to Position Engine, or it is invalid
* 
* -----------------------------------------------------------------------*/
boolean gnss_InjectExternalSensorAssistanceData
(
 const gnss_ExternalSensorAssistDataType *pz_ExtSensorAsstData
);

/* -------------------------------------------------------------------------
 * gnss_RfLTEB13TxPower
 *
 * Description: 
 *
 *  This function provides an interface for LTE L1 to notify GNSS SW
 *  of LTE TX power. LTE L1 layer provides this update at 100 msec rate.
 *
 * Parameters:
 *  t_LteClockTick   : LTE clock when Avg tx power is updated
 *  x_LteAvgTxPower: LTE Max tx power relative to absolute LTE max tx power in tenth of a dB-Hz
 *    Invalid value : 0x8000 ( No transmit in the last 100 msecs )
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
void gnss_RfLTEB13TxPower ( uint64 t_LteClockTick, int16 x_LteAvgTxPower );


/* -------------------------------------------------------------------------
 * gnss_GetGnssSlowClockTime
 *
 * Description: 
 *  This function provides the current GNSS slow clock time.
 *
 * Parameters:
 *  p_GnssSlowClockTime -  Pointer to GNSS slow clock data structure
 *
 * Return value: 
 *  TRUE - Valid GPS time available
 *  FALSE - No GPS time available
 * -----------------------------------------------------------------------*/
boolean gnss_GetGnssSlowClockTime(gnss_SlowClockStructType *p_GnssSlowClockTime);

/*
******************************************************************************
* gnss_GetGnssRcvrState
*
* Function description:
*
*  This API returns the current GNSS receiver state
*
* Parameters:
*  None.
* 
* Return value:
*  boolean: GNSS Receiver State
*  TRUE:   GNSS Receiver Session is active
*  FALSE:  GNSS Receiver session is inactive
*
******************************************************************************
*/
boolean gnss_GetGnssRcvrState(void);

/*
  ******************************************************************************
  *
  * gnss_SetExternalCtrlForGNSS
  *
  * Function description:
  *  This function provides an API by which customer can modify
  *  ELNA and NPA configuration.
  *
  * Parameters: 
  *  e_GnssCfg -- ELNA and NPA config that customer opts to use
  * 
  * Return value: None
  *  
  *
  ******************************************************************************
*/
void gnss_SetExternalCtrlForGNSS(gnss_ExternalGNSSConfigStruct e_GnssCfg);


/*
  ******************************************************************************
  *
  * gnss_GetExternalCtrlForGNSS
  *
  * Function description:
  *  This function is used to get ELNA, NPA config information. This will provide
  *  an interface by which driver knows if ELNA and NPA config information has been
  *  modified externally.
  *
  * Parameters:
  *   p_GnssCfg
  *  
  * 
  * Return value:
  *   None
  *  
  *
  ******************************************************************************
*/
void gnss_GetExternalCtrlForGNSS(gnss_ExternalGNSSConfigStruct *p_GnssCfg);

/* -------------------------------------------------------------------------
* gnss_ApqPgaCalNvReadWrite
*
* Description: 
*  This function is used to read/write to APQ PGA CAL
*  NV item. This NV item is used when an RF device does not
*  know when WWAN is transmitting.
*
* Parameters:
*  p_Val – Pointer to the value to be written/read
* b_ReadWrite – TRUE – Read; FALSE – Write

* Return value: 
*  True - Read/Write Successed
*  False - Unable to Read/Write NV
* -----------------------------------------------------------------------*/
boolean gnss_ApqPgaCalNvReadWrite(int8 * p_Val, boolean b_ReadWrite);

/*
******************************************************************************
* gnss_DisableBdsGnssRcvr
*
* Function description:
*
*  This API enables/disables BeiDou signal tracking / positioning at run time.
*  The setting does not persist across modem restarts, i.e. the value is not
*  stored in non-volatile memory.
*  Notes:
*  1) Changes are effective on next session start 
*  2) Request is ignored if the receiver is in RF dev mode. 
*  3) Will not write setting to NV.
*  4) BeiDou enablement is only honored if LTE B13 is not active and
*     memory is available for BeiDou to function properly.
*
* Parameters:
*  u_DisableBdsGnssRcvr - BeiDou will be disabled when this is set to TRUE
*                       - If this parameter is set to FALSE, BeiDou will
*                         be enabled only if BeiDou memory is available
*                         and the receiver has not detected LTE B13 concurrency,
*                         and receiver is not in RF dev mode.
* 
* Return value:
*  None.
*
******************************************************************************
*/
void gnss_DisableBdsGnssRcvr( boolean u_DisableBdsGnssRcvr  );


#endif /* GNSS_API_H */
