/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          MGP API Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the MGPSS Core software interface.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/mgp/inc/navhw_api.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/10   pn      Updated DmLog structs with PACKED_POST.
09/16/09   jd      Added support for Phase II RF HW Status logging.
08/05/09   mwb     Added support for MobiSens high-power mode
07/30/09   gk      Added support for On-demand positioning 2.0.
06/05/09   jlp     Added support for injecting Sparse network Region(s)
01/21/09   kinw    Added support for XTRA-T upload.
11/06/08   jlp     Added support for getting/setting the gps restriction level.
07/17/09   jd      Added mgp_SendDiagRfCmd() for HW Config cmd over QXDM
07/17/09   jd      Added mgp_RfBPStartInit() and mgp_RfBPStopInit() for 
                   RF Boot up calibration.
10/27/08   jlp     Added interface to set Sv's that we should not search for
                   or track.
07/25/08   gk      Force receiver off changes
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
06/12/08    lt     Support for external coarse position injection.
04/25/08   gk      On-Demand feature changes
02/14/08    jw     Support for Optimistic Punc
12/18/07   lt      Added support for disabling time-alignment in periodic MSA.
07/31/07   br      Added support for 1x Allow GPS interface.
05/31/07   jw      Added Rcvr memconfig type (Hi or Lo Mem)
05/07/07   jw      Added Diag GPS Intelliciever support.
05/07/07   br      Added support for running PPM in MC task.
05/05/07   br      Added support for RF Info interface.
04/24/07    ld     Support for CELLDB (Interim Check-in)
04/23/07   jw      Flow the disable bit edge required before track flag to MC.
04/19/07    ld     Intermediate check-in for UMTS Time-tagging
04/17/07    ld     Modified MGP Tick Time to 1 second
02/20/07   br      Added support for TCXO Manager Rotator Push.
01/24/07    jw     Added new MGP Time Tick functionality
11/17/06   br      Added 1x SRCH interface.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "gps_variation.h"
#ifdef WIN32
#pragma once
#endif


#ifndef NAVHW_API_H
#define NAVHW_API_H

/*
 * Include files
*/
#include "customer.h"
#include "comdef.h"
#include "gps_types.h"
#include "gnss_consts.h"
#include "gnss_common.h"
#include "aries_os_globals.h"
#include  "cgps_api.h"
#include "gnss_api.h"

#include "cm.h"

typedef struct 
{
  /* Diag Data (Used to input commands or requests from internal user) */
  uint8 u_DiagData[36]; 
} navhw_DiagCmdStruct;

/* There is a separate document which describes how these commands 
   can be used with QXDM */
typedef enum
{
  MGP_NAV_DIAG_GET_HW_VERSION_CMD = 0,
  MGP_NAV_DIAG_GET_PREAMP_CMD, /* 1 */
  MGP_NAV_DIAG_GET_PREMEAN_CMD,/* 2 */
  MGP_NAV_DIAG_GET_REGISTER_BLOCK_CMD,/* 3 */
  MGP_NAV_DIAG_GET_SINGLE_REG_VAL_CMD,/* 4 */
  MGP_NAV_DIAG_PUT_MEAS_START_STOP_CMD,/* 5 */
  MGP_NAV_DIAG_GET_GNSS_ADC_CONFIG_CMD,/* 6 */
  MGP_NAV_DIAG_PUT_MEAS_CONFIG_CMD,/* 7 */
  MGP_NAV_DIAG_PUT_SINGLE_REG_VAL_CMD,/* 8 */
  MGP_NAV_DIAG_GET_AMPL_CMD,/* 9 */
  MGP_NAV_DIAG_PUT_GNSS_ADC_CONFIG_CMD, /* 10 */
  MGP_NAV_DIAG_GET_BP_AMP, /* 11 */
  MGP_NAV_DIAG_PUT_SPAN_CFG_CMD /* 12 */
} navhw_DiagCmdEnum;

/*
 ******************************************************************************
 * Function mgp_GpsHWVersionGet
 *
 * Description:
 *
 *  This function sends a request to return the Callisto HW Version number.
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

boolean mgp_GpsHWVersionGet( uint8* p_GpsHWVersionNum );

/*
 ******************************************************************************
 * gnss_RfMeasCmdPut
 *
 * Description: 
 *  The baseband analog signal may have a large
 *  DC offset due to the mixer circuits within the RF component.
 *  It is mandatory to compensate most of the DC offset before the SDM
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
 ******************************************************************************
*/
extern void mgp_RfMeasConfigPut( const gnss_MeanMonConfigEnum e_Config);


/*
 ******************************************************************************
 * mgp_RfMeasCmdPut
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
 *     gnss_RfcMeasEnablePut() with gnss_RfMeasCmdEnum = MEAS_START; 
 *  2. Wait (RF drivers timer, in MC task context)
 *  3. Stop the mean estimation with a call to 
 *     gnss_RfMeasEnablePut() with gnss_RfMeasCmdEnum = MEAS_STOP; 
 *  4. Read the mean estimates with a call to gnss_RfAdcProcMeanMonGet().
 *
 *  In the fixed integration mode, the integration period is always 1ms, 
 *  and calls to this API are then unnecessary.
 *
 * Parameters:
 *  gnss_RfMeasCmdType that captures new linearity setting
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
extern void mgp_RfMeasCmdPut( const gnss_RfMeasCmdEnum e_MeasCmd);

/*
 ******************************************************************************
 * mgp_RfAdcProcMeanMonGet
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
 ******************************************************************************
*/
extern void mgp_RfAdcProcMeanMonGet
( 
  const gnss_AdcProcEnum   e_AdcProc,
  gnss_MeanMonType         *p_AdcProcMeanMon
);


/*
 ******************************************************************************
 * mgp_RfAdcProcConfigPut
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
 ******************************************************************************
*/
extern void mgp_RfAdcProcConfigPut
( 
  const gnss_AdcProcEnum     e_AdcProc,
  gnss_AdcProcConfigPutType  *p_AdcProcConfigPut
);


/*
 ******************************************************************************
 * mgp_RfAdcProcIqCntrlPut
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
 ******************************************************************************
*/
extern void mgp_RfAdcProcIqCntrlPut
( 
  const gnss_AdcProcEnum     e_AdcProc,
  gnss_AdcProcIqCntrlType    *p_IqCntrl
);


/*
 ******************************************************************************
 * mgp_RfAdcProcBlankConfigPut
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
 ******************************************************************************
*/
extern void mgp_RfAdcProcBlankConfigPut
( 
  const gnss_AdcProcEnum         e_AdcProc,
  gnss_AdcProcBlankConfigType    *p_BlankConfig
);


/*
 ******************************************************************************
 * mgp_RfAdcProcBlankPut
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
 ******************************************************************************
*/
extern void mgp_RfAdcProcBlankPut
( 
  const gnss_AdcProcEnum   e_AdcProc,
  boolean                  u_BlankSig
);





/*
 ******************************************************************************
 * mgp_RfBpConfigPut
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
 ******************************************************************************
*/
extern void mgp_RfBpConfigPut
( 
  const gnss_BpEnum       e_Bp,
  gnss_BpConfigPutType    *p_Config
);


/*
 ******************************************************************************
 * mgp_RfBpIfDownConvPut
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
 ******************************************************************************
*/
extern void mgp_RfBpIfDownConvPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpIfDownConvType    *p_IfDownConv
);


/*
 ******************************************************************************
 * mgp_RfBpEqCoefPut
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
 ******************************************************************************
*/
extern void mgp_RfBpEqCoefPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpEqCoefType        *p_EqCoef
);

/*
 ******************************************************************************
 * mgp_RfBpGroupDelayPut
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
 ******************************************************************************
*/
extern void mgp_RfBpGroupDelayPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_RfGrpDelayType      *p_GrpDelay
);


/*
 ******************************************************************************
 * mgp_RfBpNotchFltrConfigPut
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
 ******************************************************************************
*/
extern void mgp_RfBpNotchFltrConfigPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpNotchFltrType     *p_NotchFltr
);
/* -------------------------------------------------------------------------
 * navhw_RfBpPreNotchFltrConfigPut
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
void navhw_RfBpPreNotchFltrConfigPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpPreNotchFltrType     *p_PreNotchFltr
);

/*
 ******************************************************************************
 * mgp_RfBpMeanEstPut
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
 ******************************************************************************
*/
extern void mgp_RfBpMeanEstPut
( 
  const gnss_BpEnum        e_Bp,
  gnss_BpMeanEstType       *p_MeanEst
);


/*
 ******************************************************************************
 * mgp_RfBpSubChanThreshPut
 *
 * Description: 
 *  Provides write access to the BP bit fields BP_GAINALIGN, BP_THRESHI, 
 *  and BP_THRESHQ to allow software control of AGC. Overriding the AGC may 
 *  enable end-to-end noise figure measurements using a calibrated noise 
 *  source.  
 *
 *  NOTE: This API is not expected to be exercised normal operation mode, 
 *        but may be useful to debug problems.  
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpSubChanThreshType - Structure that stores sub-channel 
 *                             configuration
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void mgp_RfBpSubChanThreshPut
( 
  const gnss_BpEnum             e_Bp,
  gnss_BpSubChanThreshType      *p_SubChanThresh
);


/*
 ******************************************************************************
 * mgp_RfBpPreAmplGet
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
 ******************************************************************************
*/
extern void mgp_RfBpPreAmplGet
( 
  const gnss_BpEnum             e_Bp,
  gnss_BpPreAmplMonType         *p_PreAmplMon
);

/* -------------------------------------------------------------------------
 * navhw_RfAdcProcAmplMonGet
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
void navhw_RfAdcProcAmplMonGet
( 
  const gnss_AdcProcEnum   e_AdcProc,
  gnss_AdcProcAmplMonType  *p_AdcProcAmplMon
);

/*
 ******************************************************************************
 * Function navhw_RfAdcConfigPut
 *
 * Description:
 *
 *  This function is used to write to GNSS_ADC_CMD* registers.
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

void navhw_RfAdcConfigPut
( 
  gnss_RfAdcRegEnum e_AdcReg,
  uint8 u_Bitmask,
  uint8 u_Value 
);
/*
 ******************************************************************************
 * Function navhw_RfAdcConfigGet
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
 *  uint8 * p_RegVal - Pointer to value of ADC register
 *
 *
 ******************************************************************************
*/

boolean navhw_RfAdcConfigGet
( 
  gnss_RfAdcRegEnum e_AdcReg,
  uint8 * p_RegVal 
);

/*
 ******************************************************************************
 * Function navhw_ProcessDiagCommands
 *
 * Description:
 *
 *  This function is used to test GNSS RF APIs. 
 *
 * Parameters: 
 * 
 *  navhw_DiagCmdStruct - Struture that contains the data pertaining 
 *  to exercise RF APIs
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value:
 * 
 *
 *
 ******************************************************************************
*/
void navhw_ProcessDiagCommands( navhw_DiagCmdStruct * p_NavDiagCmd );


/*
 ******************************************************************************
 * navhw_RfAdcClockControl
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
 ******************************************************************************
*/
void navhw_RfAdcClockControl
( 
  boolean u_Enable
);


/* -------------------------------------------------------------------------
* navhw_RfBpAmplGet
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
extern boolean navhw_RfBpAmplGet
(
  const gnss_BpEnum  e_Bp,
  gnss_BpAmplType    *p_BpAmpInfo
);

/* -------------------------------------------------------------------------
* navhw_ApqPgaCalNvReadWrite
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
boolean navhw_ApqPgaCalNvReadWrite(int8 * p_Val, boolean b_ReadWrite);

/* -------------------------------------------------------------------------
* navhw_GtmAvailable(void)
*
* Description: 
*   Returns true or false depending on presense of GTM in the HW
*
* Parameters:
*
* Return value: 
*  True -  GTM is present
*  False - 
* 
*-----------------------------------------------------------------------*/
boolean navhw_GtmAvailable(void);

/*
 ******************************************************************************
 * navhw_bb_GetAdcSampleFreq
 *
 * Description: 
 *  This function gets the current ADC sampling rate in use
 *
 * Parameters:
 *  None.
 *
 * Return value: 
 *  ADC sampling rate in Hz
 ******************************************************************************
*/
uint32 navhw_bb_GetAdcSampleFreq(void);

#endif /* NAVHW_API_H */ 

