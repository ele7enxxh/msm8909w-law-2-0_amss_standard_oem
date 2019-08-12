#ifndef CUSTCDMA_H
#define CUSTCDMA_H
/*===========================================================================

DESCRIPTION
  Configuration for CDMA and AMPS Operation

  Copyright (c) 2002 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cust/inc/custcdma.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   pk      1x2GTA Feature changes
10/25/11   ppr     FR-1146 code featurization with FEATURE_1X_ENH_POS_LOC 
                   feature name
07/28/11   vks     Changes to get 1xA working on nikel.
06/06/11   vks     Move custjcdma.h feature set to custcdma.h and remove
                   inclusion of custjcdma.h
11/03/10   adw     Added defines to support target cust file cleanup.
08/30/10   jtm     Defined FEATURE_ENC_CONTROL_TX_ON_EN
08/13/10   adw     Reverted previous change for FEATURE_DS.
07/20/10   adw     Added include for FEATURE_DS define.
04/08/10   mca     Defined FEATURE_ENC_CONTROL_TX_ON_EN
03/25/10   ag      Defined FEATURE_OTASP_UPDATE_MDN_NAI.
07/27/09   adw     Disabled CMI-lite.
04/16/09   adw     Removed T_ target defines.
03/24/09   adw     Enabled temporary CMI-lite feature.
12/15/08   jj      Defined FEATURE_IS683C_PRL and FEATURE_IS683C_OTASP.
10/29/08   adw     Defined FEATURE_AUTH, FEATURE_AUTH_DIGITS & FEATURE_DH_EXP.
10/03/08   mca     Defined FEATURE_MODEM_1X_DIAG
07/28/08   vlc     Defined FEATURE_MVS_MIGRATE.
11/02/05   fc      Added MOB_P_REV support for FEATURE_IS2000_REL_C_MI.
09/07/05   fc      Added MOB_P_REV support for FEATURE_IS2000_REL_D.
07/19/05   vlc     Defined FEATURE_RL_JUMP_ENHANCEMENT.
05/31/05   grl     Defined FEATURE_MC_SUPPORTS_SHDR.
07/02/04   st      Defined feature FEATURE_RXTX_TC_MSG_NOTIFY.
03/01/04   yll     Added support for Band Class 3 and 6.
09/08/03   aaj     Defined feature FEATURE_CDMA_RX_DIVERSITY
08/20/03   aaj     Included feature FEATURE_CDMA_RX_DIVERSITY
11/11/02   nxb     Added FEATURE_BUILD_CDMA_CELL_PCS.
05/01/02   jct     Created
===========================================================================*/

/* -----------------------------------------------------------------------
** Top Level CDMA Call Processing Mode Feature Selection
** ----------------------------------------------------------------------- */
#if (defined (FEATURE_BUILD_CDMA_CELL) && !defined (FEATURE_BUILD_JCDMA))

   /* Supports CDMA at 800 MHz
   */
//top level feature removed:    #define FEATURE_CDMA_800

#elif defined FEATURE_BUILD_CDMA_CELL_AMPS

   /* Supports CDMA at 800 MHz with AMPS
   */
//top level feature removed:    #define FEATURE_CDMA_800
   /* Enable Analog Call Processing.
   */

#elif (defined (FEATURE_BUILD_JCDMA) && !defined (FEATURE_BUILD_CDMA_PCS))

   /* Supports CDMA at 800 MHz with JCDMA
   */
//top level feature removed:    #define  FEATURE_CDMA_800

#elif (defined (FEATURE_BUILD_CDMA_PCS) && !defined (FEATURE_BUILD_JCDMA))

   /* Supports CDMA at 1900 MHz
   */
//top level feature removed:    #define FEATURE_CDMA_1900

#elif (defined (FEATURE_BUILD_CDMA_PCS) && defined (FEATURE_BUILD_JCDMA))

   /* Supports CDMA at 800 MHz with JCDMA
   */
//top level feature removed:    #define  FEATURE_CDMA_800

   /* Supports CDMA at 1900 MHz
   */
//top level feature removed:    #define FEATURE_CDMA_1900

#elif defined FEATURE_BUILD_CDMA_TRIMODE

   /* Supports CDMA at 800  MHz
   **          CDMA at 1900 Mhz
   **          AMPS
   */
//top level feature removed:    #define FEATURE_CDMA_800
//top level feature removed:    #define FEATURE_CDMA_1900
   /* Enable Analog Call Processing.
   */

#elif defined FEATURE_BUILD_CDMA_CELL_PCS

   /* Supports CDMA at 800  MHz
   **          CDMA at 1900 Mhz
   */
//top level feature removed:    #define FEATURE_CDMA_800
//top level feature removed:    #define FEATURE_CDMA_1900

#else
   #error Must specify a FEATURE_BUILD_CDMA_xxx option
#endif


/* -----------------------------------------------------------------------
** General CDMA Call Processing
** ----------------------------------------------------------------------- */

/* Feature to enable the code for New externalized CP API called by 
** other modules to get the UE CDMA Base Station Position information  
** including currently Active and Neighbor's Base station position 
** information
** 
** This New CP API was required to get the extra information about cell position parameters 
** for Active and Neighbor base stations and this information is helpful for applications
** in estimating the Current UE CDMA Base station position more accurately. 
*/
#define FEATURE_1X_ENH_POS_LOC

/* Feature to enable some test code to demonstrate how Packet Zone Id change
** handler is used.  This feature should be turned off by the customers.
*/
#undef FEATURE_PZID_CHANGE_EXAMPLE

/* Rate determination adjustments for SCH DTX detection
*/
//top level feature removed: #define FEATURE_RDA_CORRECTION

/* Define Power Class III SCM presentation
*/
//top level feature removed: #define FEATURE_POWER_CLASS_III

/* Preferred Roaming
*/
//top level feature removed: #define  FEATURE_PREFERRED_ROAMING

/* These features need to be turned on to enable all the functionality needed for
** FEATURE_OTASP.
*/

/* Enables code to process a roaming list with the format specified in
** IS 683A. Otherwise, the code will process a roaming list with the format
** specified in IS 683.
*/
//top level feature removed: #define  FEATURE_IS683A_PRL
/* Allows a mode that restricts the phone to home systems only.
*/

/*  Defining these features to enable code process a roaming
**  list with the format specified in IS 683C.Operators
**  have a flexibility of turning off IS683C PRL capability
**  by undefining FEATURE_IS683C_OTASP */

//top level feature removed: #define FEATURE_IS683C_PRL
#define FEATURE_IS683C_OTASP

//top level feature removed: #define  FEATURE_HOME_ONLY

/* This feature is required to ensure the proper functionality of the preferred roaming
** code, for both PCS and SSPR 800 builds.
** Implements special processing in the event of MAX ACCESS PROBES on the
** Access channel to prevent the phone from returning immediately to the
** system, because it is probably not usable.
*/
//top level feature removed: #define  FEATURE_MAX_ACCESS_FALLBACK

/* GSRM Service Indicator
*/
//top level feature removed: #define  FEATURE_GSRM_SVC_IND

/* EVRC SO Management
*/
//top level feature removed: #define  FEATURE_EVRC_SO_MGMT

/* EVRC Rate Determination Improvement
*/
//top level feature removed: #define  FEATURE_EVRC_RDA

/*  Class 1 IMSI support
*/
//top level feature removed: #define FEATURE_CLASS_1_IMSI

/*  Emergency 911
*/
//top level feature removed: #define FEATURE_E911

/*  Use faster algorithm to scan through the PRL when a system is acquired
*/
//top level feature removed: #define FEATURE_FASTPRL

/*  Wildcard System ID Management
*/
#undef FEATURE_WCARD_SID_MGMT

/*  1X Diagnostics
*/
#define FEATURE_MODEM_1X_DIAG

/* -----------------------------------------------------------------------
** IS-95B
** ----------------------------------------------------------------------- */
//top level feature removed: #define FEATURE_IS95B

#ifdef FEATURE_IS95B
  /*---------------------------------------*/
  /* Enable the individual IS-95B features */
  /*---------------------------------------*/

  /* IS95B compliant version of True IMSI
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_TRUE_IMSI

  /* IS95B compliant version of System Reselection
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_SYS_RESELEC

  /* IS95B compliant version of New Soft Handoff Algorithm
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_SOFT_HO

  /* IS95B compliant version of Calling NAme Presentation (CNAP)
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_EXT_DISP

  /* IS95B compliant version of Access Handoff
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_ACCESS_HO

  /* IS95B compliant version of Pilot Reporting
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_PILOT_RPT

  /* IS95B compliant version of Neighbor Search Enhancements
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_NGHBR_SRCH_ENH

  /* IS95B compliant version of the Release Order on Access channel
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_RELEASE_ON_AC

  /* IS95B compliant version of the Periodic Pilot Strength Measurement Msg
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_PPSMM

  /* IS95B compliant version of Network Directed System Selection (NDSS)
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_NDSS

  /* IS95B compliant version of the Incoming Call Forwarding Enhancement
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_INC_CALL_FWD_ENH

  /* IS95B compliant version of the Alert Answer Bypass Feature
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_ALERT_ANS_BYPASS

  /*-----------------------------------------------------------*/
  /* Enable the individual IS-95B Protocol Revision 5 features */
  /* These are optional for Revsion 4                          */
  /*-----------------------------------------------------------*/

  /* IS95B compliant version of Mobile Assisted Hard Handoff feature.
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_MAHHO

  /*-----------------------------------------------------------*/
  /* Enable the optional IS-95B Protocol Revision features     */
  /*-----------------------------------------------------------*/

  /* IS95B compliant version of Access Entry Handoff
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_ACCESS_ENTRY_HO

  /* IS95B compliant version of Access Probe Handoff
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_IS95B_ACCESS_PROBE_HO

  /* TMSI support is required for IS-95B Band Class 0
  ** This is included only if FEATURE_IS95B is included
  */
//top level feature removed:   #define FEATURE_TMSI

  /*-----------------------------------------------------------*/
  /* End of IS-95B Feature Definitions                         */
  /*-----------------------------------------------------------*/

#endif /* FEATURE_IS95B */

/* -----------------------------------------------------------------------
** IS-2000
** ----------------------------------------------------------------------- */
//top level feature removed: #define FEATURE_IS2000

#ifdef FEATURE_IS2000

  /* Fast Forward Power Control
  */
//top level feature removed:   #define FEATURE_FFPC

  /* Quick Paging Channel
  */
//top level feature removed:   #define FEATURE_IS2000_QPCH

  /* Quick Paging Channel - Configuration Change Indicators
  */
//top level feature removed:   #define FEATURE_IS2000_QPCH_CCI

  /* Reverse Link Gain Control
  */
//top level feature removed:   #define FEATURE_RLGC

  /* LTU CRC checking for conv. coded SCH.  Can heavily load CPU
  */
//top level feature removed:   #define FEATURE_IS2000_LTU_CRCS

  /* Data related features
  */
  #ifdef FEATURE_DS
   /*
    * Featurizes data code for IS-2000.
    * Data Services support for IS-2000 data, such as RLP3.
   */
//top level feature removed:     #define FEATURE_DS_IS2000
    /* This is the new RLP for IS-2000
    */
//top level feature removed:     #define FEATURE_DS_RLP3

    #ifdef FEATURE_DS_IS2000

      //Supplemental Channel
//top level feature removed:       #define FEATURE_IS2000_SCH

      #ifdef FEATURE_IS2000_SCH

        // Forward supplemental channel
//top level feature removed:         #define FEATURE_IS2000_F_SCH

        // Reverse supplemental channel
//top level feature removed:         #define FEATURE_IS2000_R_SCH

      #endif

      // Supplemental Channel MUX statistics
//top level feature removed:       #define FEATURE_IS2000_SCH_STATS
    #endif

    /* Enables logging of RLP3 frames.
     * Supports logging of RLP Rx and Tx frames for IS-2000.
    */
//top level feature removed:     #define FEATURE_RLP_LOGGING
  #endif

  /* P2 mode support (DCCH)
  */
//top level feature removed:   #define FEATURE_IS2000_P2

  #ifdef FEATURE_IS2000_P2
    /* DCCH DTX using DCCH Frame Quality Indicator */
    #define FEATURE_DCCH_FRAME_QUALITY_SUPERVISION
    /* The enhanced DCCH DTX logic is based on the Power Control Bit
     * energy(PC_ACC), CRC, SER, Frame Energy for frame classification.
     * It declares a GOOD frame when CRC passes; otherwise the ratio
     * between the PC_ACC value and the FRAME_ENERGY is used to detect
     * if the data bits are present and based on the comparison between
     * the ratio and SER value, the frame is declared as DTX or ERASURE.
    */
    #define FEATURE_ENHANCE_DCCH_DTX_DETECTION
  #endif
  /* Enables the reduction of the mobile's maximum total reverse
   * power as a function of a state vector.
  */
//top level feature removed:   #define FEATURE_TX_POWER_BACK_OFF

  /* Control Hold State support
  */
//top level feature removed:   #define FEATURE_IS2000_CHS

  #ifdef FEATURE_BUILD_JCDMA
     #undef FEATURE_BUILD_DS_MIP
  #endif /* FEATURE_BUILD_JCDMA */

  #ifdef FEATURE_BUILD_DS_MIP
     /* Mobile IP
     */
//top level feature removed:      #define FEATURE_DS_MOBILE_IP

     /* Mobile IP Performance Metrics
     */
//top level feature removed:      #define FEATURE_DS_MOBILE_IP_PERF
  #endif

  /* RS2 Packet Data support - IS-2000 only
  */
  #define FEATURE_DS_IS2000_RS2

  /* Test Data Service Option Support
  */
//top level feature removed:   #define FEATURE_IS2000_TDSO

  /* Bias triage towards pilots with supplemental channels
  */
//top level feature removed:   #define FEATURE_SCH_TRIAGE

  /* Max Power Protection on the SCH
  */
//top level feature removed:   #define FEATURE_MAX_PWR_PROTECTION

  /* Handle cases of SO/P_REV mismatch caused by P_REV 5 in Sync
  ** Channel for 1x systems.
  */
  #define FEATURE_VERIFY_DS_SO_ON_ORIG
  /*
  * Causes update of the P_REV information in use during the paging
  * channel state.
  */
  #define FEATURE_UPDATE_P_REV_IN_PCH

  /*turn on for backward compatability*/
  #define FEATURE_IS2000_SLOTTED_TIMER
  /* Enable the IS-2000 Release 0 Reverse FCH 1/8 rate gating feature.
  */
  #define FEATURE_IS2000_REV_FCH_GATING

  /* Enable RL jump to hyperspace enhancement to save call set up time.
  */
  #define FEATURE_RL_JUMP_ENHANCEMENT

  /* Give control of TX_ON to the encoder
  */
  #define FEATURE_ENC_CONTROL_TX_ON_EN

  /* Enable 1x Advanced Features */
//top level feature removed:   #define FEATURE_IS2000_1X_ADV

#endif /* FEATURE_IS2000 */

/* Enables management of all service options by SNM
*/
//top level feature removed: #define FEATURE_COMPLETE_SNM

/* This feature changes the rxc/txc task's way of processing traffic channel
** bit streams. The changes are centered around the viterbi decoder ISR and
** convolutional encoder isr.
*/
//top level feature removed: #define FEATURE_MINIMIZE_ISR_LOADING

/* Perform AGC/power control logging
*/
//top level feature removed: #define FEATURE_AGC_LOGGING

#define FEATURE_IS2000_REL_C_FCS
#define FEATURE_IS2000_REL_C
#define FEATURE_IS2000_REL_B
//top level feature removed: #define FEATURE_IS2000_REL_A

/* ----------------------------------------------------------------------
** P_REV definition
** ----------------------------------------------------------------------- */
#ifdef FEATURE_IS2000_REL_D
#error code not present
#elif defined ( FEATURE_IS2000_REL_C_MI )
#error code not present
#elif defined ( FEATURE_IS2000_REL_C )

   /* Pull in Release A specific definitions
   */
//target cust file include removed:    #include "custrela.h"

//top level feature removed:    #define MOB_P_REV 9

#elif defined ( FEATURE_IS2000_REL_B )

   /* Pull in Release A specific definitions
   */
//target cust file include removed:    #include "custrela.h"

//top level feature removed:    #define MOB_P_REV 8

#elif defined ( FEATURE_IS2000_REL_A )

   /* Pull in Release A specific definitions
   */
//target cust file include removed:    #include "custrela.h"

   /* MOB_P_REV definition needed for run-time P_REV checking
   ** It is defined as 7 for IS-2000 Rel A compliant mobiles
   */
//top level feature removed:    #define MOB_P_REV 7

#elif defined (FEATURE_IS2000)

   /* MOB_P_REV definition needed for run-time P_REV checking
   ** It is defined as 6 for IS-2000 Rel 0 compliant mobiles
   */
//top level feature removed:    #define MOB_P_REV 6

#elif defined (FEATURE_IS95B)

   /* MOB_P_REV definition needed for run-time P_REV checking
   ** It is defined as 4 for IS-95B compliant mobiles
   */
//top level feature removed:    #define MOB_P_REV 4

#else
   /* MOB_P_REV definition needed for run-time P_REV checking
   ** It is defined as 3 to assist the P_REV1+ solution for MDR
   */
//top level feature removed:    #define MOB_P_REV 3

#endif

/* -----------------------------------------------------------------------
** AMPS
** ----------------------------------------------------------------------- */
#ifdef FEATURE_ACP
   /*  FM sleep improvements
   */
   #define FEATURE_FM_SLOTTED

   /* FM slotted debug information
   */
   #undef FM_SLOTTED_DEBUG

   /* Gain Adjustment for ACP WB data.
    * Enables FREQ_SENSE_GAIN value to be modified by a percentage
    * defined in this file when transmitting WBD. Value is defaulted
    * to 10% reduction.
   */
   #define FEATURE_FSG_CONTROL

#endif

/* -----------------------------------------------------------------------
** CDMA at 800MHz Specific
** ----------------------------------------------------------------------- */
#ifdef FEATURE_CDMA_800
   /* This feature allows the use of a roaming list as defined in IS-683A.
   ** SSPR_800 can operate without a roaming list, in which case it behaves similar
   ** to the old algorithm, but not exactly the same.
   */
//top level feature removed:    #define FEATURE_SSPR_800

   /* These features need to be turned on to enable all the functionality needed for
   ** FEATURE_OTASP.
   ** Allows the phone to be forced into a specific mode
   ** (for example, "DIGITAL_ONLY").
   */
//top level feature removed:    #define  FEATURE_PRL_FORCE_MODE

   /* Needs to be turned on to enable all the functionality needed for FEATURE_SSPR_800.
   */
//top level feature removed:    #define  FEATURE_SSPR_ENHANCEMENTS

#endif

#if  !defined (FEATURE_BUILD_CDMA_PCS)
#if defined FEATURE_BUILD_CDMA_CELL     || \
    defined FEATURE_BUILD_JCDMA || \
    defined FEATURE_BUILD_CDMA_CELL_AMPS
      /* Feature to prevent roam side available systems from being acquired.
       * For 800 MHz band carriers in markets where A and B systems
       * are present. If the carrier uses A band, this feature prevents
       * the phone from acquiring B band systems, unless they are
       * explicitly listed in the PRL or CDMA SID/NID list.
      */
      #define FEATURE_AVAIL_HOME_SIDE_PREF
#endif
#endif /*  !defined (FEATURE_BUILD_CDMA_PCS) */
#if defined FEATURE_BUILD_CDMA_TRIMODE
   /* Enable MC silent redial in AMPS
   */
   #define FEATURE_MC_SILENT_REDIAL_AMPS
#endif

/* -----------------------------------------------------------------------
** OTASP/OTAPA
** ----------------------------------------------------------------------- */

/* Phone will do OTASP Protocol
** This feature includes full compliance with IS-683A except for OTAPA.
*/
//top level feature removed: #define  FEATURE_OTASP

/* Unselect non-standard OTASP implementation.
*/
#undef FEATURE_NSOTASP

/* OTASP_OTAPA
*/
#if defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP)
   /* Support for Over the Air Authentication
    * OTAPA is the network-initiated portion of IS-683A OTASP.
   */
//top level feature removed:    #define FEATURE_OTASP_OTAPA

   /* Hold the ending command while an OTASP commit is in progress.
    * CM keeps user's call-end and call-origination commands from
    * being sent to CP while an OTASP commit is in process.
   */
//top level feature removed:    #define FEATURE_OTASP_HOLD_CMD_IN_COMMIT

   #ifdef FEATURE_MDN_BASED_NAI
     /* Support for updating MDN based NAIs from OTASP module instead of NV */
     #define FEATURE_OTASP_UPDATE_MDN_NAI
   #endif

#endif

/* CDMA Rx diversity feature */
//top level feature removed: #define FEATURE_CDMA_RX_DIVERSITY

/* New support for TC message notification from CP to SRCH. */
#define FEATURE_RXTX_TC_MSG_NOTIFY

/* Supports revised searcher API that among other things supports SHDR */
#define FEATURE_MC_SUPPORTS_SHDR

/* The following 4 features are Authentication features */
//top level feature removed: #define FEATURE_AUTH
#define FEATURE_AUTH_DIGITS

/* Enable the Diffie-Helman task.
 * Used by OTASP to perform AKey exchange. Enables the task and code
 * to send commands to and receive reports from the task.
*/
//top level feature removed: #define FEATURE_DH

/* This feature is needed for OTASP authentication for NV only build or
** RUIM build with run time RUIM enabled.  If it's RUIM only build without
** run time RUIM enabled, the feature should not need to be defined. */
//top level feature removed: #define FEATURE_DH_EXP

/* MVS migration in 1x is under this feature */
#define FEATURE_MVS_MIGRATE

/* Temporary CMI-lite */
#undef FEATURE_1X_DISABLE_CMI

/* Enable data burst messaging */
//top level feature removed: #define FEATURE_DBM

/* Enable MEID support */
//top level feature removed: #define FEATURE_MEID_SUPPORT

/* REVISIT:
 
   The custjcdma.h is being pulled from an orphan location in perforce
   (//depot/asic/msm7200/main/build/cust/custjcdma.h#1) and it is only 1x that
   is including it (though technically there are other sub-systems that need it
   as well). The target team wants 1x to own the file but custjcdma.h is not
   specific to 1x and there are subsystems that use it as as well. 
   So as a first step, the entire custjcdma.h feature set has been moved into
   this file and bounded by feature FEATURE_BUILD_JCDMA. With this, the target
   team can remove custjcdma.h.
   As a next step, the features not related to 1x need to be moved to the
   appropriate sub-systems.
 
*/
#ifdef FEATURE_BUILD_JCDMA

  /*---------------------------------------------------------------------------
      Japanese CDMA related FEATURES
  ---------------------------------------------------------------------------*/
  
  /* ARIB STD-T53 protocol
  */
  #define FEATURE_T53
  
  /* This feature is for only application for T53 Ver 1.2 */
  #define FEATURE_T53_1P2ONLY
  
  /* JCDMA specification
  */
  #define FEATURE_JCDMA
  
  // Data stuf for JCDMA
  #define FEATURE_JCDMA_1X
  
  #ifdef FEATURE_BUILD_JRF
  
    /* ARIB-T53 protocol under JRF */
    #undef FEATURE_T53_WITH_US_FREQ
    #define FEATURE_T53_RF
  
  #else
  
  /* ARIB-T53 protocol under U.S. RF band
  ** Note: SURF board only supports U.S. RF band, so keep define this function
  ** since not support JCDMA RF band currently.
  */
  #define FEATURE_T53_WITH_US_FREQ
  
  #undef FEATURE_T53_RF
  
  #endif
  
  /* Compliance with the ARIB T-53 frequency assignment. 
   * Note: This feature is currently undefined because the current SURF 
   * boards only support the U.S. RF band.
  */
  #define FEATURE_JCDMA_GPSONE
  
  /*---------------------------------------------------------------------------
                            Non-Volatile Memory
  ---------------------------------------------------------------------------*/
  
  /* This feature configures to change initial value when NV item is not
  ** activated for JCDMA.
  */
  #ifdef FEATURE_JCDMA
    #define FEATURE_JCDMA_NV
  #endif
  
  /*---------------------------------------------------------------------------
                              Data Services
  ---------------------------------------------------------------------------*/
  
  #ifdef FEATURE_DS
  
  /* Enables reconnection on packet zone ID change
  */
  #undef FEATURE_DS_PZID_RECON
  
  /* Do not originate with IS-707 standard service options
     (If FEATURE_DS_CHOOSE_SO is defined, this specifies the default set.)
  */
  #undef FEATURE_DS_ORIG_IS707_SO
  
  /* Originate with proprietary service options (JCDMA SO set)
  */
  #define FEATURE_DS_ORIG_JCDMA_SO
  
  /* Support choosing the set of service options used for originations.
  */
  #undef FEATURE_DS_CHOOSE_SO
  
  /* Support Quick Net Connect originations
  */
  #undef FEATURE_DS_QNC
  
  /* Enable QNC and SO33 operation on 3G mobiles with separate NAI/password 
  ** pairs.
  */
  #undef FEATURE_DS_QNC_FIXED_NAI
  
  /* Enables three data features to support the Japan CDMA spec without having to
  ** enable each individually.
  */
  //#define FEATURE_JCDMA_DS
  
  /* END key puts mobile in NULL state (rather forcing dormancy.)  
  */
  #ifdef FEATURE_JCDMA_1X
    #define FEATURE_DS_END_KEY_DROPS_CALL
  #else
    /* this can be used to enable this feature individually
    */
    #undef FEATURE_DS_END_KEY_DROPS_CALL
  #endif /* FEATURE_JCDMA_1X */
  
  /* When an end-to-end resync is required, drop the call.
  */
  #ifdef FEATURE_JCDMA_1X
    #define FEATURE_DS_NET_MODEL_DROPS_CALL
  #else
    /* this can be used to enable this feature individually
    */
    #undef FEATURE_DS_NET_MODEL_DROPS_CALL
  #endif /* FEATURE_JCDMA_1X */
  
  #ifdef FEATURE_DS_NET_MODEL
    /* In a network model call use a different password for resyncs.
    */
    #if defined(FEATURE_JCDMA_1X) && defined(FEATURE_DS_CHAP)
      #define FEATURE_DS_CHAP_RESYNC
    #else
      /* this can be used to enable this feature individually
      */
      #undef FEATURE_DS_CHAP_RESYNC
    #endif /* FEATURE_JCDMA_1X */
  #endif /* FEATURE_DS_NET_MODEL  */
  
  /* Enable no origination for LCP TERM-Req during dormant state
  */
  #ifdef FEATURE_JCDMA_1X
    #define FEATURE_DS_PPP_TERM_REQ_DROP
  #else
    #undef FEATURE_DS_PPP_TERM_REQ_DROP
  #endif /* FEATURE_JCDMA_1X */
  
  /* Enable dormancy for laptop calls is enabled when AT+CTA=0
  */
  #ifdef FEATURE_JCDMA_1X
//top level feature removed:     #define FEATURE_DS_ENABLE_RM_DMNCY_IF_CTA_0
  #else
    #undef FEATURE_DS_ENABLE_RM_DMNCY_IF_CTA_0
  #endif /* FEATURE_JCDMA_1X */
  
  #endif /* FEATURE_DS */
  
  /*---------------------------------------------------------------------------
                              Call Manager
  ---------------------------------------------------------------------------*/
  
  /* Enbale FEATURE_JCDMA_CM for DMSS_J Call Manager Features */
  #define FEATURE_JCDMA_CM
  
  /* Enable CM monior features to provide seacher, RF and other information
  ** for JCDMA Monitor specification.
  */
  #undef  FEATURE_JCDMA_MONITOR
  #define FEATURE_MONITOR
  /* Per JCDMA requirements, cap the maximum R-SCH.
  */
  #define FEATURE_JCDMA_MAX_RATE

#endif /* FEATURE_BUILD_JCDMA */

/* Enable 1x->X TA */
#define FEATURE_ONEX_DATA_TUNEAWAY
#endif /* CUSTCDMA_H */
