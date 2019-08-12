#ifndef FTMICAP_H
#define FTMICAP_H

/*===========================================================================

             F T M   C a p a b i l i t i e s   H e a d e r   F i l e

DESCRIPTION
  This header file that defines FTM capabilities.
  This header file is internal to the FTM unit.

Copyright (c) 2003 - 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftmicap.h#1 $
  $DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/30/14   qzh     Enable FTM NS new intf and remove the dependency
11/20/13   qzh     Featurized FTM NS new interface to disable it on BO 0.1 
07/19/13   qzh     Update Non-signaling R4&HSUPA code with new TL1-RFSW interface and function partition
07/09/13   aca     Logging support for all techs
12/06/12   ms      HSUPA support (Nikel merge)  
05/23/12   hy      Added support for TDSCDMA HSDPA Non-signaling
02/16/12   hy      Added support for TDSCDMA Non-signaling
02/15/12   sar     Removed FEATURE_FTM_MC.
02/14/12   sar     Reinstated TDSCDMA features.
01/28/12   sar     Feature reduction changes.
07/29/11   hy      do not enable FTM_HAS_TDSCDMA_BER by default.
07/11/11   hy      Including TD-SCDMA support.
02/18/11   sty     Moved few #defines to FTM 1x HAL
10/23/10   pv      Add BC10 support 
06/29/10   kma     Added SV module feature
04/01/10   adk     Merged branch scmm_mdm9k_merge_final
01/29/10   dyc     Config XO Cal common to 1x and UMTS
01/11/10   sr      Deleted rficap.h
11/11/09   ka      Do not define FTM_HAS_GSM for WPLT builds.
10/15/09   hp      Add feature FTM_HAS_GNSS
09/24/09   sar     Removed feature flags related to BT, WLAN, QFUSE and FM.
10/06/09   jps     Adding feature FTM_HAS_IRAT
09/20/09   lp      Cleaned up and removed featurize #if defined. Mainlined code
                   with T_QSC6695 defined
09/10/09   tws     Add FEATURE_FTM_HAS_LTE dependent on FEATURE_LTE.
09/09/09   aro     Removed DateTime and Author + Extra Space Removal
07/07/09   dyc     FTM_HAS_FM support.
06/15/09   ns      XO calibration changes for CDMA SCMM bringup
06/09/09   aro     Added Enum for 0 Khz Offset Tx rotator Register Value
05/11/09   aro     Fixed HDR featurization.
04/24/09   jps     removed cust_modem_T_MSM.h file
04/13/09   sr      included cust_modem_T_MSM.h file
11/19/08   ans     Fixed HDR featurization.
09/11/08   aro     Fixed the featurization for AUX_SBI read/write Support
07/14/08   adk     Merged QSD8650 code.
06/30/08   Vish    Added support for Tx 19.2KHz cordic rotation for MSM7800.
06/19/08   et      included rficap.h and wrapped FTM_HAS_SUPPORT_FOR_BC10
                   inside RF_HAS_BC10.
05/28/08   adk     Ported DORB changes from the 7800 branch.
05/21/08   adm     Fix for compiler errors on MFLO builds.
05/14/08   sk      Added support for WinMob
04/09/08   aak     Corrected featurization for MSM7500.
03/06/08   aak/ad  Added support for FTM HHO on 7600, 7500B, 6800B targets
03/06/08   ad      Corrected target definition for qfuse
03/06/08   ad      Added qfuse support for 7k targets
12/21/07   rmd     Change necessary to allow MSM7500-90 to compile.
11/20/07   adk     Merged changes from FTM tips. Removed duplicate definitions.
08/08/07   jfc     Condensing UMTS features
08/06/07   jfc     Featurized code for EDGE-only builds
06/25/07   ad      Added support for single ended BER
06/22/07   avi     Added FTM_HAS_MDSP_ANTENNA1_CLK_CTL feature to control ant1
                   clock
05/18/07   sar     Enabled FTM_HAS_HDR feature and FTM_HAS_LOGGING for MSM7500.
11/15/06   jfc     Removed FTM_HAS_AUDIO until missing symbols are figured out
10/31/06   ycl     Enable FEATURE_FTM_MC.
10/17/06   ycl     Modifications for initial MSM7600 build.
07/13/06   xw      Added 7200 support.
06/26/06   et      GSM quadband support
06/26/06   dbc     Added support for FTM_HAS_MFLO on UMTS Targets
06/13/06   ra      Added FTM_HAS_WLAN on UMTS Targets
05/23/06   vm      Added feature FTM_HAS_HDR_ENHANCED_ACCESS_CHANNEL to support
                   enhanced access channel in HDR non signaling mode
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
05/12/06   ra      Add support for FTM_HAS_WLAN on 1x Targets
04/10/06   vm      Added new feature FTM_USES_NEW_IM2_SEARCH_METHOD to support
                   new IM2 Calibration Algorithm.
03/15/06   zg      Defined ZIF_LIBRARY_SUPPORTS_IM2_API for pre MSM6800 targets.
01/10/06   lp      Added FTM_HAS_AGC_CARRIER_FREQ_ERR to adjust
                   TRK_LO_ADJ_VCTCXO PDM.
11/18/05   lp      For MSM7500, enabled FTM_HAS_TX_DACC_CONTROL and
                   FEATURE_FTM_CONFIG_TX_SWEEP_CAL defines and removed
                   ZIF_LIBRARY_SUPPORTS_IM2_API.
11/14/05   dyc     Updates to allow this file to be picked up for MSM6500
11/14/05   zg      Added ZIF_LIBRARY_SUPPORTS_IM2_API for MSM6500
10/24/05   ra      Added FTM_HAS_TX_DACC_CONTROL for relevant target
09/05/05   ra      Create UMTS and 1x sections.
                   Enable FTM_HAS_HDR_REV_A based on FEATURE_HDR_REVA_L1
09/05/05   ra      Define FEATURE_FTM_CONFIG_TX_SWEEP_CAL. This will enable FTM_HAS_CFG_TX_SWP_CAL.
                   The user may choose remove FEATURE_FTM_CONFIG_TX_SWEEP_CAL which will
                   enable FTM_LEGACY_TX_SWEEP_CAL
06/13/05   ra      Added FTM_HAS_CFG_TX_SWP_CAL
05/16/05   et      added FTM_HAS_I2C under FEATURE_FTM_I2C under MSM6500
05/02/05   dbc     Added FTM_HAS_MFLO, predicated by FEATURE_MFLO.
03/11/05   ka      Added FTM_HAS_GSM_EXTENDED for Raven.
02/09/05   rsr     Added FTM_HAS_MULTI_SLOT for raven
12/07/04   ra      Removed obsolete FTM_HAS_AGC_LOGGING definition from 1x projects.
                   This feature has been mainlined.
11/23/04   ra      Removed I2C support from 1x projects
10/13/04   lp      Removed FEATURE_CAMERA_V7 to fix MSM6550 FTM camera
                   compile errors.
10/12/04   jac     Removed unneeded FTM_HAS_ASYNC_PKT definitions.
10/12/04   lp      Turned off FTM_HAS_CAMERA feature for MSM6550 when
                   FEATURE_CAMERA_V7 is defined.
10/06/04   lp      Added FTM_HAS_CAMERA
08/22/04   ra      Merge 1X and UMTS
08/20/04   xw      Removed FEATURE_FTM_BT and added FTM_HAS_I2C.
10/10/03   sw      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#define FEATURE_RF_TL1_FTM_NEW_INTF
#endif

#include "rfa_variation.h"
#include "target.h"     /* Target definitions   */
#include "customer.h"   /* Customer definitions */

#ifdef FTMICAP_HEADER
  #include FTMICAP_HEADER
#else

/*===========================================================================

               Definitions common to 1x and UMTS

===========================================================================*/


/*===========================================================================

                         UMTS Projects

===========================================================================*/




#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  #define FTM_HAS_UMTS
#endif

#ifdef FTM_HAS_UMTS

  #define FTM_HAS_COMMON_COMMANDS
  #define FTM_HAS_IM2_CAL

  #ifndef FEATURE_WPLT
  #define FTM_HAS_GSM_EXTENDED
  #endif
  #define FTM_HAS_SE_BER

#endif  /* FTM_HAS_UMTS */

#if defined (FEATURE_WCDMA) && defined (FEATURE_GSM) && !defined(FEATURE_WPLT)
  #define FTM_HAS_IRAT
#endif

/*===========================================================================

                         TDSCDMA Projects

===========================================================================*/
#ifdef FEATURE_TDSCDMA
  #define FTM_HAS_TDSCDMA
  
  #ifdef FEATURE_TDSCDMA_FTM_NS  
    #define FTM_HAS_TDSCDMA_BER
    #ifdef FEATURE_RF_TL1_FTM_NEW_INTF
//    #define FTM_HAS_TDS_HSDPA
    #else
    #define FTM_HAS_TDS_HSDPA
    #endif
    #define FTM_HAS_TDS_HSUPA

    #ifndef FTM_HAS_SE_BER
    #define FTM_HAS_SE_BER
    #endif

  #endif

  //reserved feature: FTM_HAS_TDS_HSUPA
#if defined(FEATURE_RF_TDSCDMA_RX_DIVERSITY)
   #define FTM_HAS_TDSCDMA_RX_DIVERSITY
#endif /*FEATURE_RF_TDSCDMA_RX_DIVERSITY*/
#endif /*FEATURE_TDSCDMA*/


/*===========================================================================

                         LTE Projects

===========================================================================*/



/*===========================================================================

                         COMMON

===========================================================================*/




#define FTM_HAS_COMMON_COMMANDS

#define FTM_HAS_LOGGING



/*===========================================================================

                      Simulaneous Voice and Data Projects

===========================================================================*/
#ifdef RF_HAS_REMOTE_RF_DRIVER
#define FTM_HAS_SV_API
#endif

#endif /* else FTMICAP_HEADER */
#endif /* FTMICAP_H */

