#ifndef CUSTRF_H
#define CUSTRF_H
/*===========================================================================

DESCRIPTION
  Configuration for RF

  Copyright (c) 2002  by Qualcomm Technologies Incorporated. All Rights Reserved.
  Copyright (c) 2008  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custrf.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/22/10   bmg     Added FEATURE_RF_IMPLEMENTS_HDR_REVB_PA_BACKOFF because
                   RF SW will handle computing the correct HDR max power
                   backoff based on channel configuration.
04/15/10   sar     Enabled RF_HAS_IS2000_1X_ADV feature for IS2000 1X ADV feature 
                   support.
02/04/10   zg      Enabled FEATURE_PA_ACCESS_PROBE_CONFIG 
07/18/08   kma     Removed FEATURE_SLEEP
06/10/08   kma     Resolved compilation issue for 8k targets
03/05/08   jfc     Removed temporary feature RF_HAS_GWZRF6285_SMPS.
02/20/08   jfc     Temporarily added RF_HAS_GWZRF6285_SMPS. Remove when mainlined.
11/26/07   adk     Removed FEATURE_MDSP_SUPPORTS_TX_AGC.
08/09/07   ad      Define FEATURE_HSDPA_PROGRAM_LNA_SWITCH to read HS LNA
                   switchpoint NVs during HS calls.
05/10/07   jfc     Define FEATURE_RF_DOES_NOT_HAVE_3U_HDET_ADC for WCDMA
03/08/07   jfc     Define FEATURE_MDSP_SUPPORTS_TX_AGC for 1x
02/27/07   jfc     Moved FEATURE_RFM_IOCTL from rfcom.h to custrf.h
01/26/07   jfc     Wrapped FEATURE_INTELLICEIVER with FEATURE_CDMA1X
12/14/06   jfc     Removed FEATURE_LIBRARY_ONLY. Should not be defined here.
11/21/06   jfc     Only define FEATURE_LIBRARY_ONLY if it isn't already
11/09/06   adk     Added RF_TODO_7600_ADK
10/18/06   ycl     Added RF_MSM7600_HWIO and FEATURE_LIBRARY_ONLY.
05/31/06   pp      Enable FEATURE_RFM_SLEEP_CALLBACK
03/06/06   dp      Defined FEATURE_RF_GET_BAND_CAPABILITY
01/16/06   pp      Enable FEATURE_RFM_INTERFACE
10/21/05   kt      Wrap hashed-include for CUSTUSURF under !defined WCDMA/GSM_PLT
09/07/05   src     Added hashed-include for CUSTUSURF header to account for
                   references to FEATURE_SLEEP defined in there.
06/14/05   adm     Added FEATURE_RF_VBATT_TX_PWR_COMP
02/14/05   adm     Added FEATURE_SLEEP
01/21/05   rv      Added FEATURE_CHANNEL_FILTER_STAGE2_FIX
01/17/05   src     Moved FEATURE_POLAR_RF out of this file into the proper
                   target files.
12/23/04   gfr     Added FEATURE_POLAR_RF
12/08/04   gfr     Removed FEATURE_SBI_MDSP_CONTROLLER
10/23/03   jac     added FEATURE_PMIC_VARIABLE_PA_VDD, removed undef FEATURE_PMIC_MANAGED_LDO
07/18/03   ry      Moved FEATURE_RF_DINSABER to msm6250.builds. Checked in
                   custrf.h#3 plus changes as custrf.h#5.
11/25/02   nxb     Added FEATURE_RADIO_ONE_RFNV
11/24/02   nxb     Removed FEATURE_NO_STATUS_SBI (obsolete)   
05/01/02   jct     Created
===========================================================================*/
/* DNN hack for might not needed MDSP defines */
#define MDSP_REG_eer_dc_offset_ctl 0
#define MDSP_REG_chfilt_general_ctl 0
#define MDSP_REG_polarmodcfg 0
#define MDSP_REG_eer_dc_offset_ctl 0
#define MDSP_REG_txphaseincrement 0
#define MDSP_REG_polarmodcfg 0
#define MDSP_REG_tx_phase_increment 0
#define MDSP_REG_chfiltrsbcoef 0
#define MDSP_REG_rsbcfg        0
#define MDSP_RF_CTL_prgm_chfilt_time_ctl   0
#define MDSP_RF_CTL_prgm_chfilt_gen_ctl    0
/* More removed defines from MDSP*/
#define MDSP_ssbiTxAgcEnabled   (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg0    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg1    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg2    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg3    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg4    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg5    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg6    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg7    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg8    (volatile uint16*)(0x3f2a)
#define MDSP_paCompBpg9    (volatile uint16*)(0x3f2a)
#define MDSP_rx0AgcValueCmInit0    (volatile uint16*)(0x3f2a)
#define MDSP_rx0AgcValueCmInit1    (volatile uint16*)(0x3f2a)
#define MDSP_rx0AgcValueCmInit2    (volatile uint16*)(0x3f2a)
#define MDSP_rx0AgcValueCmInit3    (volatile uint16*)(0x3f2a)
#define MDSP_rx0AgcValueCmInit4    (volatile uint16*)(0x3f2a)
#define MDSP_rx0CmLnaFall1    (volatile uint16*)(0x3f2a)
#define MDSP_rx0CmLnaFall2    (volatile uint16*)(0x3f2a)
#define MDSP_rx0CmLnaRise1    (volatile uint16*)(0x3f2a)
#define MDSP_rx1AgcValueCmInit0    (volatile uint16*)(0x3f2a)
#define MDSP_rx1AgcValueCmInit1    (volatile uint16*)(0x3f2a)
#define MDSP_rx1AgcValueCmInit2    (volatile uint16*)(0x3f2a)
#define MDSP_rx1AgcValueCmInit3    (volatile uint16*)(0x3f2a)
#define MDSP_rx1AgcValueCmInit4    (volatile uint16*)(0x3f2a)
#define MDSP_rx1CmLnaFall1    (volatile uint16*)(0x3f2a)
#define MDSP_rx1CmLnaFall2    (volatile uint16*)(0x3f2a)
#define MDSP_rx1CmLnaRise1    (volatile uint16*)(0x3f2a)
#define MDSP_rx1CmLnaRise2    (volatile uint16*)(0x3f2a)
#define MDSP_rx0CmLogReferenceMinusOne    (volatile uint16*)(0x3f2a)
#define MDSP_rx1CmLogReferenceMinusOne    (volatile uint16*)(0x3f2a)
#define MDSP_paRise1    (volatile uint16*)(0x3f2a)
#define MDSP_paRise2    (volatile uint16*)(0x3f2a)
#define MDSP_paRise3    (volatile uint16*)(0x3f2a)
#define MDSP_paFall1    (volatile uint16*)(0x3f2a)
#define MDSP_paFall2    (volatile uint16*)(0x3f2a)
#define MDSP_paFall3    (volatile uint16*)(0x3f2a)
#define MDSP_txPaRange0PdmTable    (volatile uint16*)(0x3f2a)
#define MDSP_txPaRange1PdmTable    (volatile uint16*)(0x3f2a)
#define MDSP_txPaRange2PdmTable    (volatile uint16*)(0x3f2a)
#define MDSP_txPaRange3PdmTable    (volatile uint16*)(0x3f2a)
#define MDSP_txPaRange0LinearTable0    (volatile uint16*)(0x3f2a)
#define MDSP_txPaRange1LinearTable0    (volatile uint16*)(0x3f2a)
#define MDSP_txPaRange2LinearTable0    (volatile uint16*)(0x3f2a)
#define MDSP_txPaRange3LinearTable0    (volatile uint16*)(0x3f2a)
#define BBRX_MOVED_TO_RFA //dyc moved to rfa/device/bbrx
#if defined FEATURE_WCDMA
   #if (!(defined FEATURE_WCDMA_PLT) && !(defined FEATURE_GSM_PLT))
      //#define FEATURE_SLEEP
      //#define FEATURE_CDMA1X
      //#define FEATURE_WCDMA
   #else
      #include "custusurf_wplt.h"
   #endif
#endif 

#define FEATURE_MULTI_RX_CHAIN  //TODO_7600
#define RF_TODO_7600_ADK

/*David added for 7200 */
#define FEATURE_SSBI


/* ------------------------------------------------------------------------
** RF
** ------------------------------------------------------------------------ */

/* Adding rf_vbatt feature   */

#define FEATURE_RF_VBATT_TX_PWR_COMP
/* Perform ZIF logging
*/
#define FEATURE_ZIF_LOGGING

/* RF Warmup reduction from 42ms.
*/
#define FEATURE_FAST_WARMUP

/* Enable TX ACCESS POWER Fix.
*/
#define FEATURE_TX_ACCESS_POWER_FIX

/* Clock PA gain state machine at 800Hz (PCG) to enable Tx Gain Adjust to
** occur according to the tx data rate synchronized to tx frame boundaries.
*/
#define FEATURE_IS2000_CAGC

/* Enable NV item to calculate the PA warmup delay used for puncturing during
** transmit.
*/
#define FEATURE_SUBPCG_PA_WARMUP_DELAY

/* Enable bypass and powering down of PA during normal CDMA transmission.
** NOT YET FULLY SUPPORTED - DO NOT DEFINE.
*/
#undef FEATURE_PA_BYPASS_ENABLE

/* Support for RFCAL version.
*/
#define FEATURE_RFCAL_VERSION

/* Enable the RFM interface
*/
#define FEATURE_RFM_INTERFACE

/* Enable the RFM interface for sleep
*/
#define FEATURE_RFM_SLEEP_CALLBACK

/* ------------------------------------------------------------------------
** SBI
** ------------------------------------------------------------------------ */
#define FEATURE_SBI
#define FEATURE_MULTIMODE_SBI
#define FEATURE_NEW_SBI

/* ------------------------------------------------------------------------
** PMIC
** ------------------------------------------------------------------------ */

/* Define to enable Power Management features.  Supercedes all previous
** PM1000 feature flags.
*/
#define FEATURE_PMIC

#if defined(FEATURE_PMIC)

  /* Enables the PMIC ringer. It configures DMSS to use the PMIC ringer
  */
  /* JAI #define FEATURE_PMIC_RINGER */
#endif

/* CRF3100-PA and QRF3300-1 uses RFR3100
** Enables reading of Forward Mode Rx CAGC RF Cal items.
*/
#define FEATURE_RFR3100

/* Features to use the improved TX AGC supported by CRF3100-PA
** Features enable reading of Forward Mode Tx CAGC RF Cal items.
*/

#define FEATURE_PA_RANGE_TEMP_FREQ_COMP
#define FEATURE_4PAGE_TX_LINEARIZER

/* JAI
*/
#undef FEATURE_RADIO_ONE_RFNV

/*--------------------------------------------------------------------- 
      Feature to compile with RF directory from UZ3010 
---------------------------------------------------------------------*/
#define RF_HAS_ZIF

#define FEATURE_CHANNEL_FILTER_STAGE2_FIX

/*--------------------------------------------------
           FEATURE SLEEP
--------------------------------------------------*/

#ifdef FEATURE_SLEEP
    #define FEATURE_SLEEP_DISABLE_RF_GPIOS

    /* Enables powering down the synthesizer LDO in sleep */
    #define FEATURE_RF_DISABLE_LDO_IN_SLEEP
#endif
 
#define FEATURE_RF_GET_BAND_CAPABILITY

#define RF_MSM7600_HWIO

/* Used for remote API */
#define FEATURE_RFM_IOCTL

/* 1x specific features */
#ifdef FEATURE_CDMA1X
#define FEATURE_INTELLICEIVER
#define FEATURE_LIN_INTERPOLATION_FREQ_COMP
#define FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#define FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#define FEATURE_PA_ACCESS_PROBE_CONFIG
#endif /* FEATURE_CDMA1X */

/* UMTS specific features */
#ifdef FEATURE_WCDMA
/* Disable DM for HDET (only used on Platform 3U)
    This allows ADC drivers and RF drivers to distinguish between 
    the way 3U does MDSP based HDET reads (via DM) and the way we
    do MDSP based HDET on other targets (via SSBI read) */
#define FEATURE_RF_DOES_NOT_HAVE_3U_HDET_ADC
#define FEATURE_HSDPA_PROGRAM_LNA_SWITCH

#endif /* FEATURE_WCDMA */

/*  This feature enables the 1X Advanced FW Support in RF Driver, provided the 
    following dependencies are accounted for. With this feature undefined, RF driver
    will revert back to the legacy FW interface. The following are the dependencies 
    for RF support for 1X Advanced FW: 
   
    NV:
      VU_CORE_SERVICES_NV_COMMON.00.00.78
      VU_CORE_SERVICES_NV_JCDMA.00.00.78

    MDSP:
      TBD.

    CP:
      //depot/asic/sandbox/users/anishg/1xAdv/for_rf/mccso.c#1
*/
#define RF_HAS_IS2000_1X_ADV
#define FEATURE_RF_IMPLEMENTS_HDR_REVB_PA_BACKOFF

#endif /* CUSTRF_H */



