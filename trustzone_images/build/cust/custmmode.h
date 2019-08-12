#ifndef CUSTMMODE_H
#define CUSTMMODE_H
/*========================================================================
DESCRIPTION

  Configuration CUST file for all of Multi Mode code

  Copyright (c) 2010  by Qualcomm Technologies Incorporated. All Rights Reserved.
========================================================================*/


/*========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custmmode.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------
10/06/10   rp    Disabling feature FEATURE_MODEM_STATISTICS 
10/05/10   gm    Add feature FEATURE_SD_ENHANCED_DEBUG_MSG
10/01/10   ak    Merged the Fusion code from Dev branch.
09/29/10   rm    Enabling FEATURE_MODEM_STATISTICS
09/21/10   np    Add feature FEATURE_CM_LTE_CC for LTE call control 
09/17/10   cl    Adding support for DOG_DYNAMIC for MMOC
09/17/10   cl    Adding support for DOG_DYNAMIC_API for CM
09/09/10   aj    Fix compilation issue on C2k only targets
09/03/10   aj    support to stay on any LTE system if UE is in conn state
08/03/10   fj    Add feature FEATURE_CM_GWL_CALL_OBJECT_TRANSFER.
07/20/10   aj    temporary limited srv Rat priority list update
07/16/10   sv    Define MDM_GWL_BSR and MMODE_MDM_MSM_EVENTS
06/15/10   sv    Define generic 9k fusion feature FEATURE_CM_FUSION_MDM
06/24/10   ay    Added EOOS support
06/15/10   aj    support for IRAT measurement support
04/26/10   cl    Remove FEATURE_CM_MVS_API
04/26/10   aj    Add HDR condition check while enabling MMSS
03/24/10   aj    MMSS 1.1 disabled
03/19/10   aj    MMSS support
02/24/10   aj    Add support for PS optimized silent redial
02/22/10   fj    Add FEATURE_MMSS_OPTION_1_1.
01/19/10   aj    Add FEATURE_VOIP_E911_SUPPORT
12/03/09   ak    Enabling FEATURE_HICPS_STACK_SYNC_UP on target by default.
11/09/09   ay    Added support for FEATURE_SD_GWL
10/14/09   sg    Added feaure FEATURE_CM_MODEM_HEAP, FEATURE_SD_MODEM_HEAP
                 and FEATURE_MMOC_MODEM_HEAP for SCMM target (2H09 package).
08/04/09   fj    Added LTE features.
04/04/09   ks    Removing feature flag FEATURE_CM_DELAY_SETUPRES because tear
                 down of sec pdp due to MT voice call is no more valid
02/11/08   sv    Enable HICPS feature to Sync GSM and HDR protocol ACQ's
                 when UOOS is not enabled
01/25/09   aj    Add FEATURE_CM_GAN
11/18/08   am    Added FEATURE_CM_MMGSDI
10/27/08   rk    Added feature FEATURE_PS_DORMANT_PWR_SAVE
10/17/08   sv    Added FEATURE_MMODE_LOW_MEM_TARGET to consolidate all the
                 Low Mem features in CM/SD/MMOC.
09/30/08   fj    Removed FEATURE_SD_DETECT_HDR_HW.
09/02/08   ak    Added feature FEATURE_UNIFORM_SCAN_OOS_HDR_ENH.
08/15/08   sv    Added internal feature FEATURE_CM_MVS_API
05/09/08   sv    Added feature FEATURE_CM_DETECT_HW under FEATURE_MSM_SUPPORT
04/24/08   rm    Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/10/08   cl    Added FEATURE_CM_NAS_SERVICE_REQ_TYPE and
                 FEATURE_CM_NAS_PH_STAT_CHGD features
03/19/08   rm    Removed FEATURE_CM_NAS_SERVICE_REQ_TYPE for CFG_GW
03/14/08   rm    Added FEATURE_CM_NAS_SERVICE_REQ_TYPE for CFG_GW
02/20/08   ic    Renamed FEATURE_MSMSUPPORT to FEATURE_MSM_SUPPORT.
                 Added FEATURE_MMOC_SELECTIVE_ACTIVATION under
                 FEATURE_MSM_SUPPORT
12/19/07   ic    Turn on FEATURE_MM_SUPERSET for targets that use RPCs
12/11/07   jqi   Remove FEATURE_IMS_VCC. It shall not be enabled in
                 CUSTMM.H file
12/11/07   rn    Added required #ifdef around FEATURE_IMS_VCC
11/20/07   ic    Added FEATURE_LIMITED_MANUAL_ACQ
11/20/07   ic    Added FEATURE_LIMITED_MANUAL_ACQ
11/15/07   ic    Put FEATURE_SD_DETECT_HDR_HW under FEATURE_MSMSUPPORT
11/13/07   ic    Put FEATURE_GW_EMERG_SRV_CATEG under WCDMA and GSM flags
11/01/07   ic    Defined FEATURE_ADD_GW_IF_IS683C_PRL for UMTS builds
10/31/07   ic    Defined FEATURE_ADD_GW_IF_IS683C_PRL for 1x+GSM builds
10/03/07   bh    Merged changes associated with new API in SD
08/16/07   ic    Initial revision.
                 Current custcm.h converted into custmm.h
========================================================================*/

/* -----------------------------------------------------------------------
** Multi Mode sanity
** ---------------------------------------------------------------------*/

  /*
  ** Make sure custmmode.h is used
  ** MMODE code checks for this flag and throws an error if not defined
  */
  #define FEATURE_CUSTMMODE_H

  /* Make sure custcm.h is no longer used
  ** Got to check for
  ** - build local copy (defines CUSTCM_H only)
  ** - obsolete custcm.h file from CM VU (defines both CUSTCM_H and
  **   FEATURE_CM_CUSTCM_H)
  */
  #if (defined CUSTCM_H) || (defined FEATURE_CM_CUSTCM_H)
  #error custcm.h is obsolete, use custmmode.h instead !!!
  #endif

  #ifdef FEATURE_WDOG_DYNAMIC 
  #define FEATURE_CM_WDOG_DYNAMIC
  #define FEATURE_MMOC_WDOG_DYNAMIC
  #endif

/* -----------------------------------------------------------------------
** Multi Mode general features
** ---------------------------------------------------------------------*/

  /* Enable LTE related features
  */
  #ifdef FEATURE_LTE
  #define FEATURE_CM_LTE
  #define FEATURE_SD_LTE
  #define FEATURE_MMOC_LTE
  #define FEATURE_CM_LTE_CC
  /* In mainline, enable LTE to GW call object transfer;
  ** In 1.x PL, disable this feature.
  */
  #ifndef FEATURE_MDM9K_TGT_1_0
  #define FEATURE_CM_GWL_CALL_OBJECT_TRANSFER
  #else
  #undef FEATURE_CM_GWL_CALL_OBJECT_TRANSFER
  #endif
  #endif

  /* MMSS Feature Set
  */
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined FEATURE_HDR) && (defined (FEATURE_WCDMA) || defined (FEATURE_GSM)) && defined (FEATURE_LTE)
  #define FEATURE_MMSS_3_1
  #endif

  #ifndef  SD_DEBUG

  /* Temporary SD feature to limit the rat priority list to the
  ** 3GPP RATs that need to be scanned in the
  ** limited service request
  */
  #define FEATURE_SD_LTD_RAT_PRI_LIST_UPDATE

  /* temporary fix to prevent more preferred acquisition
  ** in LTE connected mode
  */
  #ifdef FEATURE_MMSS_3_1
  #define FEATURE_SD_NO_MMSS_IN_LTE_CONN_MODE
  #endif

  #endif // !SD_DEBUG

  #if (defined FEATURE_WCDMA) && (defined FEATURE_GSM) && (defined FEATURE_LTE)
  #define FEATURE_SD_GWL
  #endif

  /* Enable FEATURE_MM_SUPERSET for targets that use RPCs and thus
  ** need remote API files.
  ** This flag being on results in uniform remote API files for MMODE
  ** as even 1X-only targets will then pull in NAS files that CM uses in
  ** its API
  */
  #ifdef FEATURE_MULTIPROCESSOR
  #define FEATURE_MM_SUPERSET
  #endif

  /* Enable this feature to reduce memory in all the modules
  ** CM/SD and MMOC
  */
  #ifdef FEATURE_MMODE_LOW_MEM_TARGET
  #define FEATURE_CM_LOW_MEM_TARGET
  #define FEATURE_SD_LOW_MEM_TARGET
  #define FEATURE_MMOC_LOW_MEM_TARGET
  #else
  /* Enhanced SD debug feature sexpand strings in run time & may affect
  ** the performance 
  */
  #define FEATURE_SD_ENHANCED_DEBUG_MSG
 
  #endif

 
  /* Enable HICPS feature to Sync GSM and HDR protocol ACQ's
  */
  #if (defined(FEATURE_HDR) && ((defined FEATURE_WCDMA) || (defined FEATURE_GSM)))
    #define FEATURE_HICPS_STACK_SYNC_UP
  #endif

  /* Enable multimode specific EHRPD feature when overall EHRPD feature is
  ** enabled.
  */
  #if defined(FEATURE_EHRPD) && defined(FEATURE_IP_CALL)
    #define  FEATURE_VOIP_E911_SUPPORT
  #endif
  /* Allow optimized silent redial for EHRPD calls
  */
  #ifdef FEATURE_EHRPD
    #define FEATURE_CM_HDR_OPTI_SILENT_REDIAL
  #endif
/* -----------------------------------------------------------------------
** Call Manager
** ---------------------------------------------------------------------*/

  /* Call Manager is used.
  ** Feature flag has to remain as REX and TMC code uses it.
  */
  #define FEATURE_CM

  /* Legacy feature - OEMPhone.c etc uses it to distinguish what CM API to
  ** call (gotta clean up)
  */
  #define FEATURE_NEWCM

  /* Enables CM clients to interface with CM3.0.
  ** (this should be always turned on going forward as we will be using
  ** only CM3.0 and shim layer will not be supported)
  ** Required for OEM
  */
  #define FEATURE_CM_MULTIMODE

  /* Turn on/off misc CM_ASSERT() assertions and extra debug info
  */
  #ifdef  CM_UNIT_TEST
  #define CM_DEBUG
  #else
  #undef  CM_DEBUG
  #endif

  /* Enable CM Powerup-hold-origination feature to hold origination
  ** right after powerup giving service a chance to appear.
  */
  #define FEATURE_CM_PWRUP_HOLD_ORIG

  /* Enable CM Preferred-service-hold-origination feature to hold
  ** origination after powerup or mode preference change giving preferred
  ** service a chance to appears once sufficient service is available.
  */
  #define FEATURE_CM_PREF_SRV_HOLD_ORIG

  /* Enable CM Silent redial feature to silently redial failed calls or
  ** to ride through limited periods of no service.
  */
  #define FEATURE_CM_SILENT_REDIAL

  /* Features that need to be defined on build to enable HDR meas */
  #ifdef FEATURE_IRAT_PILOT_MEAS
  #define FEATURE_CM_SS_MEAS
  #endif

  /*
  ** Multi Mode features that are WCDMA or GSM specific
  */
  #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)

    /*  CM abort MO X call request.
    */
    #define FEATURE_CM_ABORT_X_CALL

    /* Gives the status of plmn list request (success/abort).
    ** PLMN list request is given to protocols to search for PLMNs.
    ** This feature enables protocols to return success or failure
    ** for the requested list.
    */
    #define FEATURE_GW_GET_NET_ABORT

    /* Call Manager informs clients if ciphering gets enabled for calls
    ** on either CS or PS domains
    ** (mainlined in CM, gotta check other subsystems if it can
    ** be mainlined / removed)
    */
    #define FEATURE_CIPHER_IND

    /* Release 5 changes including emergency call list, traffic flow
    ** template for PDP modify and protocol config
    */
    #define FEATURE_GW_ELIST_TFT_PCO

    /* Feature needed by MultiMode CM
    */
    #define FEATURE_CM_USE_UMTS_REG_INTERFACE

    // Pending - no support for this merged into mainline yet
    // #define FEATURE_GW_MANUAL_PLMN_IN_PS_CALL

    /* Feature supports sending minimum qos to pdp modify instead of nsapi.
    */
    #define FEATURE_GW_MINQOS_PDP_MODIFY

    /* Feature to send PROT_DEACT_LPM as reason for
    ** operating mode change to LPM
    */
    #define FEATURE_CM_LPM_DEACT_REAS

    #define FEATURE_GW_DORMANT_DURING_ALT_SCAN

    /* For CR42797: Allow UE goes to power save mode when PS call is dormant.
    ** This feature is required for GSM/UMTS capable configuration including
    ** multimode configuration.
    */
    #define FEATURE_PS_DORMANT_PWR_SAVE

    /* FEATURE_CM_GAN enables CM support for UMA GAN
    ** FEATURE_GAN is turned on if the build supports GAN
    */
    #ifdef FEATURE_GAN
      #define FEATURE_CM_GAN
    #endif /* FEATURE_GAN */


  #endif /* (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */

  /*
  ** Multi Mode features that are WCDMA and GSM specific
  */
  #if (defined FEATURE_WCDMA) && (defined FEATURE_GSM)

    /* Feature supports sending emergency service category along
    ** with emergency call request.
    */
    #define FEATURE_GW_EMERG_SRV_CATEG
    #define FEATURE_EMERGENCY_CATEGORY

  #endif /* (defined FEATURE_WCDMA) && (defined FEATURE_GSM) */

  /* Feature to support HW detection.
  */
  #ifdef FEATURE_MSM_SUPPORT

    #define FEATURE_CM_DETECT_HW

  #endif /* FEATURE_MSM_SUPPORT */

  #if defined(FEATURE_MMGSDI)

  /*
  ** Feature switch on changes done to replace gsdi to MMGSDI
  */
  #define FEATURE_CM_MMGSDI

  #endif /* FEATURE_MMGSDI */

  #if defined(FEATURE_MSM_FUSION)

  #define FEATURE_CM_MSM_CDMA_SRV_RPT

  #endif


  #if defined(FEATURE_MDM_FUSION)

  #define FEATURE_MMODE_MDM_MSM_EVENTS
  #define FEATURE_MMODE_MDM_GWL_BSR
  #define FEATURE_CM_FUSION_MDM

  #endif

  /* Modem Statistics */
  /* Note Disabling the feature still dependency with HDR CP
   * and WL1 team get resolved*/
  // #define FEATURE_MODEM_STATISTICS


/* --------------------------------------------------------------------------
** System Determination
** ------------------------------------------------------------------------*/

  /* Feature to enable loading of the PRL
  */
  #define FEATURE_PREFERRED_ROAMING

  /* Add GSM/UMTS acquistion rec when IS683C formate PRL is used in MS
  */
  #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
  #define FEATURE_ADD_GW_IF_IS683C_PRL
  #endif

  #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
  #define FEATURE_LIMITED_MANUAL_ACQ
  #endif

/* --------------------------------------------------------------------------
** Multi Mode Controller
** ------------------------------------------------------------------------*/

  /*
  ** Multimode Controller run time hardware detection to support GSM-UMTS
  ** only targets with multimode image (1x+EVDO+GSM+UMTS)
  */
  #ifdef FEATURE_MSM_SUPPORT
  #define FEATURE_MMOC_SELECTIVE_ACTIVATION
  #endif

  #ifndef FEATURE_BSR2
  #define FEATURE_BSR2
  #endif

  #ifndef FEATURE_SD_GW_SINGLE_BAND_GROUP
  #define FEATURE_SD_GW_SINGLE_BAND_GROUP
  #endif

  #ifdef FEATURE_EOOS
  #ifndef FEATURE_RAT_PRIORITY_LIST
      #error FEATURE_RAT_PRIORITY_LIST has to be defined for EOOS feature
  #endif
  #endif

  #if defined (FEATURE_EOOS) && (defined(FEATURE_WCDMA) || defined (FEATURE_GSM))
  #define FEATURE_EOOS_UMTS
  #endif

#endif /* CUSTMMODE_H */

