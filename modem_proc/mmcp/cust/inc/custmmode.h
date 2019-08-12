#ifndef CUSTMMODE_H
#define CUSTMMODE_H
/*========================================================================
DESCRIPTION

  Configuration CUST file for all of Multi Mode code

  Copyright (c) 2011  by Qualcomm Technologies Incorporated. All Rights Reserved.
========================================================================*/


/*========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/cust/inc/custmmode.h#1 $

when       who   what, where, why
------------------------------------------------------------------------------ 
07/21/14   ss    Adding FEATURE_CM_UPDATE_EF_CFIS 
01/24/14   rm    Adding debug buffers 
12/04/13   jvo   Mainline FEATURE_MMODE_RPM
09/04/13   mh/jv Mainline the modem statistics code
07/19/13   jvo   Remove all MDM/MSM Fusion features
04/25/13   qf    Enable FEATURE_MMODE_SC_SGLTE with condition FEATURE_SGLTE
02/04/13   cl    Enable FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
01/23/13   qf    Enable FEATURE_DISABLE_CM_INIT_NV_WRITE by default to remove
                  nv write during cm initialization
12/04/12   qf    FR2873 - TDS-CDMA support when device mode is single sim
11/02/12   fj    Enable FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE.
10/25/12   gm    Enable FEATURE_MMODE_SC_SVLTE under LTE by default
07/23/12   xs    Added support of location based BSR
07/13/12   gm    Enable FEATURE_CM_THERMAL_EMERGENCY_SUPPORT under LTE feature
07/02/12   qf    Enable FEATURE_AVOID_DUP_BAND_SCAN and
                 FEATURE_LIMITED_SCAN_OPT
05/24/12   skp   Added FEATURE_DATA_REFRESH_VOTING definition here for mmode
                 as data team has mainlined the feature.
03/28/12   jm    Enable FEATURE_MMODE_RPM
08/04/11   xs    Replace FEATURE_MMODE_NIKEL with FEATURE_MMODE_SC_SVLTE
07/24/11   aj    SC SVLTE 1.0 merge to mainline
05/04/11   rm    Enable FEATURE_CM_CELL_BROADCAST
03/28/11   cl    Enabled FEATURE_MMODE_MDM_MMSS and FEATURE_MMODE_MSM_MMSS
03/25/11   gm    LTE (Lmtd) to DO resel support
03/15/11   gm    Separate features for CFM interface & therm emerg
03/10/11   aj    Enable MMSS on MSM fusion
02/28/11   gm    Added FEATURE_CM_THERMAL_EMERGENCY_SUPPORT for all LTE
02/17/11   cl    Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/11/11   ak    MDM to notify MSM of DO activity for acq throttling
02/09/11   skp   Emergency call category valid when eithre of gsm or wcdma
                 feature is defined. changing the featurizatoin accordingly.
02/07/11   fj    Add FEATURE_SD_LTE for GWL common scripts.
02/07/11   rm    Defined FEATURE_MMODE_CMI_4
01/28/11   fj    Remove FEATURE_SD_GWL.
01/25/11   rm    Enable GW related mmode features
12/24/10   sv    Integrate DSDS Changes
12/17/10   am    Removing TF_UNIT_TEST
11/24/10   xs    Fusion featurization clean up
11/10/10   am    Removing dependency on custmmode_test.h
11/07/10   aj    Add support for FEATURE_MMODE_LTE_RESEL
11/15/10   rm    Added FEATURE_MODEM_HEAP_PHASE_2 for memory optimization
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
04/24/08   rm    Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/10/08   cl    Added FEATURE_CM_NAS_SERVICE_REQ_TYPE and
                 FEATURE_CM_NAS_PH_STAT_CHGD features
03/19/08   rm    Removed FEATURE_CM_NAS_SERVICE_REQ_TYPE for CFG_GW
03/14/08   rm    Added FEATURE_CM_NAS_SERVICE_REQ_TYPE for CFG_GW
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

   /*
  ** TF_UNIT_TEST should be stripped off for all the targets.
  */
  #ifdef FEATURE_WDOG_DYNAMIC
  #define FEATURE_CM_WDOG_DYNAMIC
  #define FEATURE_MMOC_WDOG_DYNAMIC
  #endif

/* -----------------------------------------------------------------------
** Multi Mode general features
** ---------------------------------------------------------------------*/

  /* Enable the feature so that CM does not perform nv/efs write during cm initialization
   ** The feature should be mainlined once stable
   */
  #define FEATURE_DISABLE_CM_INIT_NV_WRITE

  /* Enable the feature so that SD does not perform efs write during initialization
   ** The feature should be mainlined once stable
   */
  #define FEATURE_DISABLE_SD_INIT_NV_WRITE

  /* Enable GSM/WCDMA functionality in MMODE */
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA))
  #define CM_GW_SUPPORTED
  #endif

  #ifdef FEATURE_GSM
  #define SD_GSM_SUPPORTED
  #endif

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA))
  #define SD_WCDMA_SUPPORTED
  #endif

  #ifdef FEATURE_TDSCDMA
  #ifndef FEATURE_TDSCDMA_SIGNAL_STATUS
  #define FEATURE_TDSCDMA_SIGNAL_STATUS
  #endif
  #endif

  /* Enable LTE related features
  */
  #ifdef FEATURE_LTE
  #define FEATURE_CM_LTE
  #define FEATURE_SD_LTE
  #define FEATURE_MMOC_LTE
  #define FEATURE_CM_LTE_CC
  #define FEATURE_CM_THERMAL_EMERGENCY_SUPPORT
  #define FEATURE_CM_CFM_AVAILABLE
  /* In mainline, enable LTE to GW call object transfer;
  ** In 1.x PL, disable this feature.
  */

  #define FEATURE_CM_GWL_CALL_OBJECT_TRANSFER

  /* Enable domain selection if CSFB feature is defined */
  #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
  #define FEATURE_DOMAIN_SELECTION
  #endif

  /* Define GSM and WCDMA related mmode features, when LTE is defined */
  #define CM_GW_SUPPORTED
  #define SD_GSM_SUPPORTED
  #define SD_WCDMA_SUPPORTED
  #if defined FEATURE_CDMA_800 && defined FEATURE_HDR_HYBRID
  #define FEATURE_MMODE_SC_SVLTE
  #define FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  #endif

  #endif /* FEATURE_LTE */

  /* Fusion */

  #define FEATURE_PRL_EXTENDED
  #define FEATURE_SD20

  #if (defined(FEATURE_CDMA) || defined(FEATURE_HDR))
  #define FEATURE_MMODE_CDMA
  #endif

  #if (defined(FEATURE_CDMA_1900) || defined(FEATURE_HDR))
  #define FEATURE_MMODE_CDMA_1900
  #endif


  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_HDR))
  #define FEATURE_MMODE_CDMA_800
  #endif

  #if (defined(FEATURE_SSPR_800) || defined(FEATURE_HDR))
  #define FEATURE_MMODE_SSPR_800
  #endif

  #if (defined(FEATURE_8X60_SINGLE_RADIO_FUSION) ||\
       defined(FEATURE_8X60_DUAL_RADIO_FUSION))
  #error code not present
#endif

  /* MMSS Feature Set
  */
  #if (defined (FEATURE_MMODE_CDMA)  \
       && defined (CM_GW_SUPPORTED)  \
       && defined (FEATURE_LTE))

  #define FEATURE_MMSS_3_1
  #endif

  /* If target level feature flag for DO-> LTE reselection is enabled,
  ** enable mmode specific feature flag
  */
  #ifdef FEATURE_HDR_TO_LTE
  #define FEATURE_MMODE_LTE_RESEL
  #endif

  /* The following features are temporary and defined on Target only
  */
  #ifndef  SD_DEBUG

  /* temporary fix to prevent more preferred acquisition
  ** in LTE connected mode
  */
  #ifdef FEATURE_MMSS_3_1
  #define FEATURE_SD_NO_MMSS_IN_LTE_CONN_MODE
  #endif

  #endif // !SD_DEBUG


  /* Enable FEATURE_MM_SUPERSET for targets that use RPCs and thus
  ** need remote API files.
  ** This flag being on results in uniform remote API files for MMODE
  ** as even 1X-only targets will then pull in NAS files that CM uses in
  ** its API
  */
  #ifdef FEATURE_MULTIPROCESSOR
  #define FEATURE_MM_SUPERSET
  #endif


  /* Enhanced SD debug feature sexpand strings in run time & may affect
  ** the performance
  */
  #define FEATURE_SD_ENHANCED_DEBUG_MSG


  /* Enable HICPS feature to Sync GSM and HDR protocol ACQ's
  */
  #if (defined(FEATURE_HDR) && ((defined FEATURE_WCDMA) || (defined FEATURE_GSM) || (defined CM_GW_SUPPORTED)))
    #define FEATURE_HICPS_STACK_SYNC_UP
  #endif


  /* Allow optimized silent redial for EHRPD calls
  */
  #ifdef FEATURE_EHRPD
    #define FEATURE_CM_HDR_OPTI_SILENT_REDIAL
  #endif

/* Enable FEATURE_HYBR_HDR_REDIAL_WAIT_ACQ_FAIL:
** This feature is no longer defined in HDR component.
** Only MMODE  uses this feature, so define here
*/
#define FEATURE_HYBR_HDR_REDIAL_WAIT_ACQ_FAIL
/* -----------------------------------------------------------------------
** Call Manager
** ---------------------------------------------------------------------*/

  /* Call Manager is used.
  ** Feature flag has to remain as REX and TMC code uses it.
  */
  #define FEATURE_CM

  /* Turn on/off misc CM_ASSERT() assertions and extra debug info
  */
  #ifdef  CM_UNIT_TEST
  #error code not present
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


  /* Features that need to be defined on build to enable HDR meas */
  #ifdef FEATURE_IRAT_PILOT_MEAS
  #define FEATURE_CM_SS_MEAS
  #endif

  /*
  ** Multi Mode features that are WCDMA or GSM specific
  */
  #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM) || (defined CM_GW_SUPPORTED)


    /* Gives the status of plmn list request (success/abort).
    ** PLMN list request is given to protocols to search for PLMNs.
    ** This feature enables protocols to return success or failure
    ** for the requested list.
    */


    // Pending - no support for this merged into mainline yet
    // #define FEATURE_GW_MANUAL_PLMN_IN_PS_CALL

    /* Feature supports sending minimum qos to pdp modify instead of nsapi.
    */
    #define FEATURE_GW_MINQOS_PDP_MODIFY

  #endif /* (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */

  #define FEATURE_CM_DEBUG_BUFFER
  #ifdef FEATURE_DUAL_SIM
  #define FEATURE_MMODE_DUAL_SIM
  #define CM_API_DUAL_STANDBY_MAIN
    #define FEATURE_CM_DUAL_SIM_TUNEAWAY

  #ifdef FEATURE_TRIPLE_SIM
  #define FEATURE_MMODE_TRIPLE_SIM
  #endif

  #endif

  #define  FEATURE_SUPS_RETRY

  #if defined( FEATURE_DUAL_ACTIVE) && defined (FEATURE_DUAL_SIM)
  #define FEATURE_MMODE_DUAL_ACTIVE
  #define FEATURE_MMODE_DUAL_SIM
  #define FEATURE_CM_CFM_AVAILABLE
  #define FEATURE_CM_THERMAL_EMERGENCY_SUPPORT
  #endif

  /* From DI 3.0 + */
  #if defined FEATURE_SGLTE_DUAL_SIM || defined FEATURE_SVLTE_DUAL_SIM || (defined FEATURE_1XSRLTE && defined FEATURE_DUAL_SIM)
  #define FEATURE_MMODE_SXLTE_G
  #endif  

  /* Cell Broadcast from RR/RRC */
  #define FEATURE_CM_CELL_BROADCAST

  #ifdef FEATURE_TDSCDMA
  #define FEATURE_MMODE_RPM_TDSCDMA
  #endif

  #if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
  #define FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
  #endif

  /* This feature lets the UE to deactivate PDP context with the NW before Detach
  ** Enabling this feature alone doesnot provide the functionality.
  ** This functionality is controlled by NV item - NV_PDP_DEACTIVATE_BEFORE_DETACH_SUPPORTED_I
  */
  #ifndef CM_UNIT_TEST
  #define FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH
  #endif


  #define FEATURE_MRU_UPDATE
/* --------------------------------------------------------------------------
** System Determination
** ------------------------------------------------------------------------*/




/* --------------------------------------------------------------------------
** Multi Mode Controller
** ------------------------------------------------------------------------*/

  #ifndef FEATURE_SD_GW_SINGLE_BAND_GROUP
  #define FEATURE_SD_GW_SINGLE_BAND_GROUP
  #endif

  #ifdef FEATURE_EOOS
  #error code not present
#endif

  #if defined (FEATURE_EOOS) && (defined(FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_SD_LTE))
  #error code not present
#endif

  #if !defined(FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH) && defined(FEATURE_MMGSDI_SESSION_LIB)
  #define FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
  #endif

  #ifndef FEATURE_MMOC_LOW_MEM_TARGET
  #define MMOC_STRING_MAPS_ACTIVE
  #endif

  /* CMI-4 */
  #define FEATURE_MMODE_CMI_4

  /* --------------------------------------------------------------------------
  ** QMI
  ** ------------------------------------------------------------------------*/
  #define FEATURE_QMI_NAS_HDR
  #define FEATURE_MMODE_IGNORE_RSSI_FOR_SERVICE_STATUS
  #define FEATURE_QMI_MMODE_TASK
  #define FEATURE_MMODE_REPORT_POWERSAVE

  /* FEATURE avoid duplicate band scan
  */
  #ifdef FEATURE_MMODE_SC_SVLTE
  #define FEATURE_AVOID_DUP_BAND_SCAN
  #define FEATURE_LIMITED_SCAN_OPT
  #endif

  /* Location based BSR */
  #define FEATURE_IRAT_FORCE_BSR_D2L_SETTING

  /* Define FEATURE_MMODE_SC_SGLTE if target level FEATURE_SGLTE is enabled */
  #ifdef FEATURE_SGLTE
  #define FEATURE_MMODE_MULTISTACK
  #define FEATURE_MMODE_SC_SGLTE
  #endif

  /* Feature to enable MMOC and CMREGPRX Debug buffer */
  #ifndef MMODE_ADDITIONAL_DEBUG_INFO
  #define MMODE_ADDITIONAL_DEBUG_INFO
  #endif
  
  /* Feature to enable updating EF-CFIS from CM for sups update */
  #ifdef FEATURE_CUST_1
  #define FEATURE_CM_UPDATE_EF_CFIS
  #endif

  #ifdef FEATURE_ALS
  #define FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
  #endif
  
  /* Though COREAPP CSG is not as such relevant to CDMA, for memory savings it is being turned on
  ** only in CDMA on flavours
  */
  #ifdef FEATURE_CDMA
  #define FEATURE_COREAPP_CSG
  #endif
#endif /* CUSTMMODE_H */

