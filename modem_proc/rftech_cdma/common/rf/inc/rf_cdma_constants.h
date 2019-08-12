#ifndef RF_CDMA_CONSTANTS_H
#define RF_CDMA_CONSTANTS_H

/*!
   @file
   rf_cdma_constants.h

   @brief
   This file contains 1X and HDR constants specific to CDMA.
 
   @addtogroup RF_CDMA_CONSTANTS
   @{
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_constants.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/26/14   spa     Change RF_CDMA_CHIPX_RATE_MHZ to Hz (1228800 Hz)
03/26/14   spa     Added RF_CDMA_CHIPX_RATE_MHZ (1.2288 MHz) constant
03/03/14   JJ      Moved RF_1X_DEFAULT_AGC_ACQ_DURATION & 
                   RF_1X_RXAGC_UPDATE_DURATION_USEC to rf_cdma_msm.c
01/31/14   sty     Comments update
01/17/14   JJ      Deleted Settling Time for 1x & HDR, FW gets it from FED now
01/03/14   jj      moved RF_HDR_BTF_DEFAULT to rf_cdma_tx_agc_msm.c
12/17/13   spa     Make RF_1X_DEFAULT_AGC_ACQ_DURATION half ,for 128 chips 
12/11/13   sty     removed unused #defines
11/13/13   JJ      Moved rxagc max/min def from rf_1x_mdsp.c to this file
07/22/13   APU     Increased max RFFE writes to 500 for WTR3925.
08/01/13   zhw     Use #defined CDMA Tuner CL Time Out constant
07/30/13   fh      New PA AVGA/PA state/QPOET timing constants 
07/22/13   APU     Added  RF_1X_RXAGC_UPDATE_DURATION_USEC.
07/15/13   spa     Updated RF_CDMA_RX_STOP_SETTLE_TIME to 50
06/20/13   fh      Defined Notch fitler BW to 1.8MHz
05/29/13   spa     Added #def for GRFC delay (RF_CDMA_GRFC_SCRIPT_START_DELTA)
05/21/13   sty     Changed RF_CDMA_RX_STOP_SETTLE_TIME_IRAT_CLEANUP to 0
05/20/13   sty     reverted previous change for Dime1.0
05/17/13   sty     Changed RF_CDMA_RX_STOP_SETTLE_TIME_IRAT_CLEANUP to 0
04/29/13   sty     Reduced Rx settling time for IRAT scenarios
01/24/13   hdz     Added PA timing offset for all templates
01/15/13   zhw     Increase max RFFE script size for QFE2320
01/14/13   cd      Updated Rx wakeup settling time to match worst case time for
                   Rx wakeup script
12/13/12   APU     Changed the settling time during IRAT to match the online
                   operation.
12/12/12   hdz     Change Tx warmup time to 170 us and PA warm up time to 40 us 
12/11/12   hdz     Removed old Tx/PA warm-up and turn off delay time constants and 
                   added new constants
11/13/12   sty     Added script time for static events Rx and TX
09/05/12   spa     Added max ssbi/grfc/rffe script size constants 
07/31/12   spa     Added constant RF_CDMA_MAX_TEMP_COMP_SSBI_SCRIPTS 
06/14/12   zhw     Added new constants for Tx/PA gating time offsets.
05/18/12   zhw     Added new constant for Tx/PA Warmup time
05/09/12   zhw     Add new constant hdr btf values
04/27/12   hdz     Change 1x_PA related timing constants 
04/24/12   zhw     Initial version

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*!
  @name CDMA standard specific constants
 
*/

/*! @{ */

/*----------------------------------------------------------------------------*/
/*! CDMA Chip x Rate in Hz  */
#define RF_CDMA_CHIPX_RATE_HZ                                            1228800

/*============================================================================*/
/*!
  @name Tx AGC SSBI Action Update
 
  @brief
  These are the SSBI action time update constants that are used to set 
  parameters for CDMA PA State Machine Configuration during 1X/HDR Tx AGC 
  Update. Units: Cx1 Unit
*/
/*! @{ */
  
/*----------------------------------------------------------------------------*/
/*! Default AVGA_H2L_DELAY VALUE in HDR */
#define RF_HDR_AVGA_H2L_DELAY                                                  8
       
/*----------------------------------------------------------------------------*/
/*! Default AVGA_L2H_DELAY VALUE in HDR */
#define RF_HDR_AVGA_L2H_DELAY                                                  0
              
/*----------------------------------------------------------------------------*/
/*! Default PA_H2L_DELAY VALUE in HDR */
#define RF_HDR_PA_H2L_DELAY                                                  -10
      
/*----------------------------------------------------------------------------*/
/*! Default PA_L2H_DELAY VALUE in HDR */
#define RF_HDR_PA_L2H_DELAY                                                   16    

/*----------------------------------------------------------------------------*/
/*! Default AVGA_L2H_DELAY VALUE in 1X */
#define RF_1X_AVGA_L2H_DELAY                                                  15
  
/*----------------------------------------------------------------------------*/
/*! Default AVGA_H2L_DELAY VALUE in 1X */
#define RF_1X_AVGA_H2L_DELAY                                                 -13
    
/*----------------------------------------------------------------------------*/
/*! Default PA_L2H_DELAY VALUE in 1X */
#define RF_1X_PA_L2H_DELAY                                                   -15

/*----------------------------------------------------------------------------*/
/*! Default PA_H2L_DELAY VALUE in 1X */
#define RF_1X_PA_H2L_DELAY                                                    17  

/*! @} */

/*============================================================================*/
/*!
  @name CDMA 1X AGC Acquisition Duration Constants.
 
  @brief
  The AGC acquisition duration constants used for setting up 1X AGC acquistion 
  mode data structure under different contexts. Units: Microsec (us). 
*/ 
/*! @{ */

/*============================================================================*/
/*!
  @name CDMA Settling Time constants.
 
  @brief
  CDMA Settling Time constants that are used for setting up Cx1 Delta under 
  various contexts for both 1X and HDR. Units: Cx1 Unit.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! Default Settling Time value used in CDMA Rx Stop Configuration 
  This value is based on the average number of writes we have in a Rx Stop 
  script including writes for WTR, GRFC writes and Front end device writes
 ---------------------------------------------------------------------------- 
 |Device   |       SSBI writes(2.2us)         |     RFFE writes (1.3us)
 |         |   Avg    |    Estimated margin   | Avg    |     Estimated margin
 ---------------------------------------------------------------------------- 
   WTR          1                  4            -                  -
  -------------------------20us bus switch-----------------------------------
   ASM          -                  -            2                  5
  -------------------------20us bus switch-----------------------------------
   Tuner        -                  -            6                  10
 ---------------------------------------------------------------------------- 
   Total : 70us ~ 50cx1
 ---------------------------------------------------------------------------- 
*/
#define RF_CDMA_RX_STOP_SETTLE_TIME                                           50

/*----------------------------------------------------------------------------*/
/*! Settling Time value used in CDMA Rx Stop Configuration under iRAT 
    Measurement 
*/
#define RF_CDMA_RX_STOP_SETTLE_TIME_IRAT_CLEANUP                               0

/*----------------------------------------------------------------------------*/
/*! Settling Time needed for HDR to execute the RX Start/Stop script          */
#define RF_HDR_STATIC_EVENT_RXTX_SCRIPT_TIME                                1300

/*----------------------------------------------------------------------------*/
/*! Settling Time needed for HDR to execute the IRAT script - since it has only 
    RX path to tune, this time is much lesser than - 
    RF_HDR_STATIC_EVENT_RXTX_SCRIPT_TIME. Settled on 700cx1 empirically.      */
#define RF_HDR_STATIC_IRAT_EVENT_SCRIPT_TIME                                 700

/*----------------------------------------------------------------------------*/
/*! Settling Time needed for 1X to execute the RX Start/stop script           */
#define RF_1X_STATIC_EVENT_RXTX_SCRIPT_TIME                                 1600

/*! @} */

/*============================================================================*/
/*!
  @name CDMA RTC Time constants
 
  @brief
  CDMA RTC Time constants that are used for setting up RTC Time. Units: Cx1 
  Unit. */

/*! @{ */

/*----------------------------------------------------------------------------*/
/*! Default RTC time for Rx Stop */
#define RF_CDMA_RX_STOP_RTC_CX1_TRIGGER                                        0

/*! @} */

/*============================================================================*/
/*!
  @name Tx/PA warm-up time and turn off delay time constants
 
  @brief
  Use for RF on off events to determine Tx/PA warm-up time before DAC output
  is enabled and Tx/PA turn off time after DAC output is disabled             */

/*! @{ */

/*----------------------------------------------------------------------------*/
/*! Tx warms up time in us*/
#define RF_CDMA_TX_WARM_UP_US                                               -170

/*! PA warms up time in us*/
#define RF_CDMA_PA_WARM_UP_US                                                -40

/*! Tx turns off delay time in us*/
#define RF_CDMA_TX_TURN_OFF_DELAY_US                                          25

/*! PA turns off delay time in us*/
#define RF_CDMA_PA_TURN_OFF_DELAY_US                                          50

/*! @} */
/*============================================================================*/
/*!
  @name MC layer script constants
 
  @brief
  Script constants that are used for allocating memory in MC code for creating 
  the buffer object */

/*! @{ */

/*----------------------------------------------------------------------------*/
/*! SSBI script size, defined as 300 as it is 269 in the most extreme case 
    (diversity + retune), memory isnt wasted as object is deleted after it has 
    been used */
#define RF_CDMA_MAX_SSBI_SCRIPT_SIZE                                         350
/*----------------------------------------------------------------------------*/
/*! RFFE script size*/
#define RF_CDMA_MAX_RFFE_SCRIPT_SIZE                                         500
/*----------------------------------------------------------------------------*/
/*! GRFC script size, maximum GRFC writes supported is 32*/
#define RF_CDMA_MAX_GRFC_SCRIPT_SIZE                                          32
/*----------------------------------------------------------------------------*/
/*! GRFC script start delta, This is the delay added between GRFC scripts to 
    avoid having back to back writes to the same GRFC being in the same task,if 
    there are writes to the same GRFC in the same task, the GRFC goes into an 
    undefined state,the value is in microseconds, and is populated into the 
    GRFC script
*/
#define RF_CDMA_GRFC_SCRIPT_START_DELTA                                        5
/*! @} */
/*============================================================================*/
/*!
  @name Baseband filter constants

  @brief Notch filter constrants defined suggested by system team.

  @details
  Normally, if a spur is outside the CDMA signal BW (1.23-1.25MHz), the MSM 
  modem digital filter is expected to attenuate the spur enough to not cause a 
  desense. Somehow in Triton, we are seeing a problem at channel 461 and 462 
  For 1x with a channel BW of 1.23Mhz, CDMA occupied BW is only 1.49MHz We are 
  increasing NOTCH filters bandwidth to 1.8MHz per system advice
*/
 #define RF_CDMA_NOTCH_FILTER_BW_HZ                                      1800000

/*============================================================================*/
/*!
  @name PA AVGA/PA state/QPOET/Q-Current timing constants
 
  @brief
  The following 14 constants define the timing offset relative to PCG boundary
  in USTMR for all templates for AVGA/PA state change/QPOET/QCURRENT update in
  [1] PA changes from high PA to low PA;
  [2] PA changes from low  PA to high PA;
  [3] No PA state changes, RGI changes from low RGI to high RGI;
  [4] No PA state changes, RGI changes from high RGI to low RGI;
 
  Currently, GRFC/RFFE PAs all use the same timing offset even RFFE PA PA state
  changes has a 5-transaction delay since issuing the cmd while GRFC PA doesn't
  have the delay.
*/

/*! @{ */
/*----------------------------------------------------------------------------*/
/*! PA_L2H AVGA (SBI) update time offset in USTMR                             */
#define RF_CDMA_PA_L2H_AVGA                                                 -200
/*----------------------------------------------------------------------------*/
/*! PA L2H PA state change (RFFE/GRFC) time offset in USTMR                   */
#define RF_CDMA_PA_L2H_PASTATE                                              -20
/*----------------------------------------------------------------------------*/
/*! PA L2H QPOET update time offset in USTMR                                  */
#define RF_CDMA_PA_L2H_QPOET                                                -120
/*----------------------------------------------------------------------------*/
/*! PA L2H Q-CURRENT update time offset in USTMR                              */
#define RF_CDMA_PA_L2H_QCURRENT                                              150
/*----------------------------------------------------------------------------*/
/*! PA H2L AVGA (SBI) update time offset in USTMR                             */
#define RF_CDMA_PA_H2L_AVGA                                                 -180
/*----------------------------------------------------------------------------*/
/*! PA H2L PA state change (RFFE/GRFC) time offset in USTMR */
#define RF_CDMA_PA_H2L_PASTATE                                              -50
/*----------------------------------------------------------------------------*/
/*! PA H2L QPOET update time offset in USTMR */
#define RF_CDMA_PA_H2L_QPOET                                                -150
/*----------------------------------------------------------------------------*/
/*! PA H2L Q-CURRENT update time offset in USTMR */
#define RF_CDMA_PA_H2L_QCURRENT                                               0
/*----------------------------------------------------------------------------*/
/*! RGI L2H AVGA (SBI) update time offset in USTMR */
#define RF_CDMA_RGI_L2H_AVGA                                                -100
/*----------------------------------------------------------------------------*/
/*! RGI L2H QPOET update time offset in USTMR */
#define RF_CDMA_RGI_L2H_QPOET                                               -100
/*----------------------------------------------------------------------------*/
/*! RGI L2H QCURRENT update time offset in USTMR  */
#define RF_CDMA_RGI_L2H_QCURRENT                                            -80
/*----------------------------------------------------------------------------*/
/*! RGI H2L AVGA (SBI) update time offset in USTMR */
#define RF_CDMA_RGI_H2L_AVGA                                                -160
/*----------------------------------------------------------------------------*/
/*! RGI H2L QPOET update time offset in USTMR */
#define RF_CDMA_RGI_H2L_QPOET                                               -160
/*----------------------------------------------------------------------------*/
/*! RGI H2L QCURRENT update time offset in USTMR */
#define RF_CDMA_RGI_H2L_QCURRENT                                              0
/*! @} */


/*============================================================================*/
/*!
  @name RxAGC Maximum and Minimum values
*/

/*! @{ */
/*----------------------------------------------------------------------------*/
/*! RxAGC Minimum Value */
#define RF_1X_MDSP_RXAGC_MIN_VAL -32768

/*----------------------------------------------------------------------------*/
/*! RxAGC Maximum Value */
#define RF_1X_MDSP_RXAGC_MAX_VAL 32767

/*============================================================================*/
/*!
  @name CDMA Tuner Closeloop Time out constant
 
*/

/*! @{ */

/*----------------------------------------------------------------------------*/
/*! CDMA Tuner CL Timeout constant in MS */
#define RF_CDMA_TUNER_CL_TIME_OUT_MS                                          10

/*! @} */

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* RF_CDMA_CONSTANTS_H */
