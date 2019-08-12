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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/cust/inc/custrf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
02/22/12   sar     Removed obsolete sbi features.
02/15/12   sar     Removed RF_HAS_HDR_HIT_SUPPORT_FOR_NS and T_MDM9X00B.
12/05/11   adk     Mainlined feature RF_INTERIM_NIKEL_MDM9X00_SBI_LISTS_FOR_FW
05/04/11   lp      Enabled LM features
05/04/11   dw      Mainline RF_INTERIM_NIKEL_CM_SUPPORT
05/02/11   sar     Enabled RF_HAS_1X_API to facilitate 1X api cleanup integration.
04/27/11   dw      Define RF_INTERIM_NIKEL_CM_SUPPORT
04/26/11   adk     Added feature RF_INTERIM_NIKEL_MDM9X00_SBI_LISTS_FOR_FW
12/29/10   aro     Mainlined RF_HAS_POWER_MODE_LIB
12/29/10   aro     Defined RF_HAS_POWER_MODE_LIB
12/14/10   aro     Added SVDO Feature
11/24/10   dw      Branched for Nikel
10/21/10   av      Enabled RF_FEATURE_LTOG 
10/06/10   tnt     Enable RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE on main/latest branch
09/01/10   tnt     Add RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE for 2.2 build
08/04/10   aro     Added RF_1XHDR_MERGE_INTERIM_FEATURE to assist 1xHDR Merge
                   transition
06/28/10   dnn     Removed FEATURE_RFNV
06/25/10   dnn     Defined FEATURE_RFNV
06/25/10   zg      Defined FEATURE_PA_ACCESS_PROBE_CONFIG
06/10/10   anr     Adding a new feature RF_HAS_HDR_HIT_SUPPORT_FOR_NS to use new HDR L1 cmds
03/10/10    ka     Defined dual carrier feature.
11/21/09   ans     Added bringup specific feature for RF.
01/20/09   ans     Added HDR MDSP changes.
09/16/09   tws     Add FEATURE_FTM_GSM_BER and FEATURE_FTM_EGPRS_BER.
09/03/09   pl      Add FEATURE_RF_TASK
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

#ifdef FEATURE_RFA_NIKEL_3X 

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
#define HDR_TOTAL_CARRIERS                               3
#define BBRX_MOVED_TO_RFA //dyc moved to rfa/device/bbrx
#if defined FEATURE_WCDMA
   #if (!(defined FEATURE_WCDMA_PLT) && !(defined FEATURE_GSM_PLT))
      //#define FEATURE_SLEEP
      //#define FEATURE_CDMA1X
      //#define FEATURE_WCDMA
   #else
      #error code not present
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
#endif /* FEATURE_WCDMA */

#define FEATURE_RF_TASK



/*----------------------------------------------------------------------*/
/*         FTM defines                                                  */
/*----------------------------------------------------------------------*/
#ifdef FEATURE_GSM
#define FEATURE_FTM_GSM_BER
#define FEATURE_FTM_EGPRS_BER
#endif
/*! @todo By feature complete this feature should be removed. 
    This is added for bringup */
#define RF_HAS_MDM9K_BRINGUP_MISSING_FUNCTIONALITY
#define RF_HAS_WCDMA_RX_DUAL_CARRIER
#define RF_DEVICE_MERGE_INTERIM_FEATURE
#define RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE
#if defined(FEATURE_LTE) && defined(FEATURE_GSM)
#define RF_FEATURE_LTOG
#endif

/* This is a temporary Feature. This will be defined for
Regular Release Build and will not be defined for Merge Build (branching
custrf.h). All Merge Code will be checked-in with #ifndef FEATURE so that 
Release builds will not be affected. */ 
#define RF_1XHDR_MERGE_INTERIM_FEATURE 

/* RF external interface is CMI compliant */
#define FEATURE_RFA_CMI_API

#define FEATURE_RF_SVDO_API
#define FEATURE_FACTORY_TESTMODE


/* Link Manager features  */
#define FEATURE_RF_COMMON_LM_RFM_INTERFACE
#define FEATURE_RF_WCDMA_LM_RFM_INTERFACE
#define FEATURE_RXLM
#define FEATURE_TXLM

#else /* FEATURE_RFA_NIKEL_3X */

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
#define HDR_TOTAL_CARRIERS                               3
#define BBRX_MOVED_TO_RFA //dyc moved to rfa/device/bbrx
#if defined FEATURE_WCDMA
   #if (!(defined FEATURE_WCDMA_PLT) && !(defined FEATURE_GSM_PLT))
      //#define FEATURE_SLEEP
      //#define FEATURE_CDMA1X
      //#define FEATURE_WCDMA
   #else
      #error code not present
#endif
#endif 

//top level feature removed: #define FEATURE_MULTI_RX_CHAIN  //TODO_7600
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
//top level feature removed: #define FEATURE_FAST_WARMUP

/* Enable TX ACCESS POWER Fix.
*/
//top level feature removed: #define FEATURE_TX_ACCESS_POWER_FIX

/* Clock PA gain state machine at 800Hz (PCG) to enable Tx Gain Adjust to
** occur according to the tx data rate synchronized to tx frame boundaries.
*/
//top level feature removed: #define FEATURE_IS2000_CAGC

/* Enable NV item to calculate the PA warmup delay used for puncturing during
** transmit.
*/
//top level feature removed: #define FEATURE_SUBPCG_PA_WARMUP_DELAY

/* Enable bypass and powering down of PA during normal CDMA transmission.
** NOT YET FULLY SUPPORTED - DO NOT DEFINE.
*/
#undef FEATURE_PA_BYPASS_ENABLE

/* Support for RFCAL version.
*/
//top level feature removed: #define FEATURE_RFCAL_VERSION

/* Enable the RFM interface
*/
//top level feature removed: #define FEATURE_RFM_INTERFACE

/* Enable the RFM interface for sleep
*/
#define FEATURE_RFM_SLEEP_CALLBACK

/* ------------------------------------------------------------------------
** PMIC
** ------------------------------------------------------------------------ */

/* Define to enable Power Management features.  Supercedes all previous
** PM1000 feature flags.
*/
//top level feature removed: #define FEATURE_PMIC

#if defined(FEATURE_PMIC)

  /* Enables the PMIC ringer. It configures DMSS to use the PMIC ringer
  */
  /* JAI #define FEATURE_PMIC_RINGER */
#endif

/* CRF3100-PA and QRF3300-1 uses RFR3100
** Enables reading of Forward Mode Rx CAGC RF Cal items.
*/
//top level feature removed: #define FEATURE_RFR3100

/* Features to use the improved TX AGC supported by CRF3100-PA
** Features enable reading of Forward Mode Tx CAGC RF Cal items.
*/

//top level feature removed: #define FEATURE_PA_RANGE_TEMP_FREQ_COMP
//top level feature removed: #define FEATURE_4PAGE_TX_LINEARIZER

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
//top level feature removed: #define FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
//top level feature removed: #define FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
//top level feature removed: #define FEATURE_PA_ACCESS_PROBE_CONFIG
#endif /* FEATURE_CDMA1X */

/* UMTS specific features */
#ifdef FEATURE_WCDMA
/* Disable DM for HDET (only used on Platform 3U)
    This allows ADC drivers and RF drivers to distinguish between 
    the way 3U does MDSP based HDET reads (via DM) and the way we
    do MDSP based HDET on other targets (via SSBI read) */
#define FEATURE_RF_DOES_NOT_HAVE_3U_HDET_ADC
//top level feature removed: #define FEATURE_HSDPA_PROGRAM_LNA_SWITCH
#endif /* FEATURE_WCDMA */

//top level feature removed: #define FEATURE_RF_TASK



/*----------------------------------------------------------------------*/
/*         FTM defines                                                  */
/*----------------------------------------------------------------------*/
#ifdef FEATURE_GSM
//top level feature removed: #define FEATURE_FTM_GSM_BER
//top level feature removed: #define FEATURE_FTM_EGPRS_BER
#endif
/*! @todo By feature complete this feature should be removed. 
    This is added for bringup */
#define RF_HAS_MDM9K_BRINGUP_MISSING_FUNCTIONALITY
#define RF_HAS_WCDMA_RX_DUAL_CARRIER
#define RF_DEVICE_MERGE_INTERIM_FEATURE
//top level feature removed: #define RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE
#if defined(FEATURE_LTE) && defined(FEATURE_GSM)
//top level feature removed: #define RF_FEATURE_LTOG
#endif

/* This is a temporary Feature. This will be defined for
Regular Release Build and will not be defined for Merge Build (branching
custrf.h). All Merge Code will be checked-in with #ifndef FEATURE so that 
Release builds will not be affected. */ 
#define RF_1XHDR_MERGE_INTERIM_FEATURE 

/* RF external interface is CMI compliant */
//top level feature removed: #define FEATURE_RFA_CMI_API

//top level feature removed: #define FEATURE_RF_SVDO_API
//top level feature removed: #define FEATURE_FACTORY_TESTMODE


/* Link Manager features  */
//top level feature removed: #define FEATURE_RF_COMMON_LM_RFM_INTERFACE
//top level feature removed: #define FEATURE_RF_WCDMA_LM_RFM_INTERFACE
//top level feature removed: #define FEATURE_RXLM
//top level feature removed: #define FEATURE_TXLM

#endif /* FEATURE_RFA_NIKEL_3X */
#endif /* CUSTRF_H */



