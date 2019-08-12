#ifndef CUSTUIM_H
  #define CUSTUIM_H
  /*===========================================================================

              " C u s t -  U I M "   H E A D E R   F I L E

  DESCRIPTION
  Configuration for UIM Feature.

  Copyright (c) 2008-2016 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
  ===========================================================================*/
  /*===========================================================================

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/cust/inc/custuim.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  06/08/16   lm      Revert CR 901109
  06/07/16   dd      Enabled FEATURE_UIM_PERSISTENT_CACHE
  12/01/15   ll      Removing compile time disable polling feature flag   
  09/25/15   vr      Added support for handling multiple profiles enabled cards
  06/04/15   lm      Removed unused feature flags
  05/15/15   stv     Add SIMLOCK feature to protect QMI LOC changes
  12/01/14   av      Disable perso if FEATURE_CUST_1 is defined
  10/14/14   tl      Add FEATURE_QMI_UIM_SECURE_CHANNEL_TEST
  09/09/14   bd      Features clean up
  09/03/14   gs      Remove GSTK,ESTK Compiler Flags
  06/30/14   vv      Remove Perso code
  05/30/14   akv     UART_DM register logging
  05/27/14   js      FEATURE_UIM_USB_UICC clean up for legacy code
  05/12/14   vv      Enable FEATURE_SIMLOCK
  02/07/14   ks      Removal of mmgsdi NV dependency on ICC pup states
  01/27/14   df      Allow FEATURE_UIMDIAG to be removed from customer builds
  12/05/13   shr     Added support for preferred activation over
                     1x (currently disabled)
  10/11/13   ll      Wrap disable polling feature definition for cust builds compile error
  10/08/13   tkl     Renamed FEATURE_UIM_SGLTE
  10/07/13   ll      Disable polling for internal power measurement
  09/27/13   spo     Enable FEATURE_UIM_USES_TASK_HANDLE when FEATURE_MODEM_RCINIT is defined
  09/23/13   vr      qtf-crm MOB migration
  09/19/13   ak      Removed custuim_hw_dime.h
  09/16/13   yk      Removed the code for old targets and for old features
  08/27/13   yt      Deprecate VIRTUIM/VIRTSAT module
  08/19/13   hn      Remove FEATURE_ESTK_SETUP_MENU_INTERFACE
  08/06/13   ak      Cleanup unneeded target #ifdefs
  07/25/13   hn      Enable FEATURE_ESTK_BIP_SERVER_MODE, add control bitmask
  07/09/13   tl      Disable FEATURE_MMGSDI_PKCS15 and FEATURE_UIM_PKCS15
  07/02/13   yt      Remove FEATURE_QMI_UIM_DUAL_SLOT
  06/20/13   vr      Triton TSTS changes
  06/18/13   av      Fallback to SFS if SFP not available
  04/16/13   tl      Clean up of radio access features from MMGSDI code
  04/15/13   tl      Remove obsoleted featurization
  01/25/13   hn      Deprecation of FEATURE_UI_NO_TP and FEATURE_THIN_UI
  10/18/12   bd      Enabled FEATURE_GSTK_QMI_DMS_TIME always
  09/30/12   vs      Added TAL support for SGLTE
  09/04/12   tl      Added support for SGLTE dual slots
  08/28/12   js      Initial changes for UT framework
  08/21/12   av      Clean up custuim file by removing unused feature flags
  05/14/12   tkl     Add QMI_SIM_INTERNAL service
  05/08/12   yt      Move FEATURE_UIM_DEPRECATE_TMC to custuim.h
  03/30/12   av      Remove MMGSDI/NVRUIM featre flags that are moved to nvitem
  03/30/12   av      Remmoved feature flags that have been moved to NVITEMs
  03/15/12   rm      Changes for DIME
  02/29/12   bcho    Virtual SIM clean-up
  02/06/12   av      Undef FEATURE_RTRE_DEFAULT_IS_NV for TEST_FRAMEWORK
  01/20/12   js      Changes needed for 8930 UIM bring up
  12/21/11   bcho    Removed legacy GSDI interfaces
  12/12/11   vv      Disable FEATURE_QMI_CAT_GOBI_V
  12/05/11   hn      Backing out off-target change until we fix off-target compilation
  12/03/11   ssr     Enabled FEATURE_UIM_CAVE_AN_AUTH with FEATURE_HDR_AN_AUTH
  12/02/11   hn      Move all off-target featurization out of custuim.h
  11/28/11   js      Removing use of sleep APIs
  07/28/10   tkl     Enable FEATURE_DATA_QMI_CAT for QMI CAT service
  07/20/11   dd      Enable provide local time for QMI DMS time API
  07/19/11   ak      Add MDM9615 support
  06/09/11   yt      Added FEATURE_MMGSDI_SESSION_INCREASE
  04/28/11   nk      Enable FEATURE_RUIM
  04/27/11   tkl     Disabled FEATURE_ESTK for TDSCDMA MOB
  04/27/11   yk      Added 8960 support
  04/11/11   dd      Wait for icon_disp_rsp for network commands
  04/04/11   js      Define UIM GSM features when FEATURE_WCDMA is defined
  03/07/11   nb      Do not define FEATURE_ESTK_SETUP_MENU_INTERFACE with Dual STK
  02/11/11   xz      Only enable FEATURE_ESTK_SETUP_MENU_INTERFACE for DS/DS
  02/08/11   adp     Enabled FEATURE_ESTK_SETUP_MENU_INTERFACE
  02/04/11   nmb     Removed FEATURE_DATA_PS_INTERNAL_AUTH
  02/02/11   xz      Enabled BIP
  01/20/11   dd      Enable FEATURE_CAT_REL7
  01/19/11   tkl     Mainlined service anywhere feature
  12/16/10   shr     Reverted FEATURE_USIM_1000_PBM disablement
  11/26/10   shr     Removed FEATURE_USIM_1000_PBM since this is no longer
                     supported on session based MMGSDI
  11/22/10   bd      Enabled FEATURE_GSTK_FDN_CC_SUPPORT
  11/03/10   ms      Added changes for C+G support on QTF
  11/1/10    vs      Added SESSION_LIB and CSIM features
  10/30/10   yb      6x15 bring up changes
  10/28/10   kk      Removed CAT/ICAT Support for BMP Builds
  09/28/10   yt      Disabled RAT balancing support for Gobi builds
  09/23/10   ms      Enable FEATURE_MMGSDI_PERSONALIZATION and
                     FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  09/17/10   yk      Added support dynamic registration on WDOG.
  09/16/10   ms      Disable FEATURE_MMGSDI_PERSONALIZATION,
                     FEATURE_UIM_SUPPORT_DUAL_SLOTS
  09/08/10   ms      Added DSDS UT Framework changes
  09/13/10   yt      Enabled RAT indicator support independently of FEATURE_THIN_UI
  09/10/10   nmb     Enabled FEATURE_MMGSDI_ABSENT_MANDATORY_FILES_INIT
  08/22/10   dd      Fix LTE only compilation issues
  08/20/10   dd      No icon support if FEATURE_GOBI is defined
  07/29/10   nk      Enabled FEATURE_2ND_PRL
  07/27/10   nmb     Added FEATURE_MMGSDI_ABSENT_MANDATORY_FILES_INIT
  07/26/10   ssr     Enabled FEATURE_UIM_SUPPORT_3GPD_NV
  07/09/10   ssr     Enabled FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND
  07/14/10   mib     Added FEATURE_TMC_REMOVE_UIM_NV_INIT
  06/30/10   dd      Icon display confirmation
  06/16/10   tkl     Merge in FEATURE_BMP_M2MPROFILE changes
  06/11/10   mib     Added support for QMI CAT
  06/09/10   nmb     Enabled FEATURE_PERSO_RUIM
  05/28/10   yb      8660 Bring up changes
  05/26/10   yb      Removing inclusion of deprecated files
  05/24/10   bd      Disabled FEATURE_GSTK_FDN_CC_SUPPORT
  05/23/10   xz      Enable FEATURE_GSTK_NMR_UTRAN_SUPPORT
  05/10/10   tkl     Disable Perso feature in Gobi C2K build for RUIM_W_GSM_ACCESS
  04/26/10   ssr     Fixed GOBI 2000 feature support
  04/22/10   bd      Enabled FEATURE_GSTK_FDN_CC_SUPPORT
  04/21/10   mib     Fixed definition of FEATURE_CSIM
  04/16/10   mib     Added FEATURE_MMGSDI_SESSION_LIB_WITH_LEGACY. Added
                     FEATURE_CSIM and FEATURE_MMGSDI_SESSION_LIB for off-target
  03/22/10   mib     Moved FEATURE_CSIM and FEATURE_MMGSDI_SESSION_LIB to build level
  03/15/10   mib     Added FEATURE_DATA_QMI_UIM for QMI UIM service
  03/10/10   adp     Reverting the change for adding 6 MOB flavors for now (need more discussion)
  03/04/10   adp     Adding support for various MOB flavors
  03/05/10   nb      Enabled FEATURE_MMGSDI_MBMS unconditionally
  02/26/10   nk      Enabled FEATURE_MMGSDI_INIT_RTRE_CONFIG
  02/15/10   nb      Enabled DS/DS GSTK support
  02/09/10   jk      Enable FEATURE_MMGSDI_SESSION_READ_PRL
  02/04/10   nk      Reverted Memory Reduction Feature and disabling of Automation features
  02/02/10   sg      ESTK BIP enablement
  01/18/10   nk      Disabled MMGSDI Automation and enabled MMGSDI MEMORY Reduction
  01/13/10   nb/nk   Enabled CSIM features
  01/05/10   kp      Condition FEATURE_MODEM_HEAP removed for
                     FEATURE_UIM_ZI_MEMORY_REDUCTION
  11/11/09   sk      Enabling FEATURE_VIRTUAL_SIM
  10/20/09   ssr     Added OMH-related features
  10/08/09   dd      Enable raw messages in Symbian targets
  10/08/09   kp      Added FEATURE_UIM_ZI_MEMORY_REDUCTION
  09/25/09   yk      1) Move the UART selection to the HW cust file
                     2) Add support for 7x30
  09/13/09   ps      Add support for MDM9K
  07/21/09   mib     Conditionally defined FEATURE_UIM_TEST_FRAMEWORK if
                     TEST_FRAMEWORK is defined
  06/22/09   ssr     Moved AN authentication features from CUSTHW to CUST file
  06/15/09   sg      Enabled PERSO FEATURE for QTF
  06/01/09   dd      Enable FEATURE_CAT_REL6 for all targets
  06/05/09   yk      Enabled FEATURE_UIM_UICC required for FEATURE_UIM_USIM
  05/29/09   sg      Featurized FEATURE_UIM_WIM
  05/28/09   sg      Updated to work for test framework
  05/28/09   adp     Enable FEATURE_UIM_WIM.
  05/28/09   adp     Define UIM MFLO features conditionally based on feature
                     FEATURE_MFLO_OPENCA
  05/20/09   ssr     Fixed FEATURE_RUIM for multimode support.
  05/13/09   kk      Eliminated FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC feature
  04/25/09   ssr     Added FEATURE_UIM_RUIM_SUPPORT_SCI
  04/13/09   ssr     Add feature for SC2x target
  04/03/09   yk      SCMM specific cust file inclusion
  03/19/09   rm      Platform specific cust file inclusions
  02/23/09   nd      Fixed linker error when FEATURE_VIRTUAL_SIM is defined
  02/10/09   yb      Added FEATURE_GSTK_TEST_AUTOMATION
  01/14/09   vs      Added FEATURE_UIM_USES_NEW_SLEEP_API
  01/12/09   sun     Removed dependency on FEATURE_MMGSDI_PERSONALIZATION and
                     MODEM_MBMS
  01/07/09   sk      Added FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT and
                     related support for ESTK and gstk streaming apdu.
  12/11/08   yk      Removed FEATURE_UIM_PMIC_ON_UIM1 which is a target
                     specific feature
  11/25/08   nk      Updated with RUIM support
  11/17/08   as/nk   Initial Revision.
  ===========================================================================*/
  /*---------------------------------------------------------------------------
                              Task Related Defines
  ---------------------------------------------------------------------------*/

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */
  #include "custsfs.h"

  /*---------------------------------------------------------------------------
                              ALWAYS DEFINED
  ---------------------------------------------------------------------------*/
  #ifdef FEATURE_DUAL_SIM
    /* Support for Dual Slots in UIMDRV */
    #define FEATURE_UIM_SUPPORT_DUAL_SLOTS
  #endif /* FEATURE_DUAL_SIM */

  #ifdef FEATURE_TRIPLE_SIM
    /* Support for Triple Slots in UIM */
    #define FEATURE_UIM_SUPPORT_TRIPLE_SLOTS
  #endif /* FEATURE_TRIPLE_SIM */

  #ifdef FEATURE_CDMA
    #define FEATURE_MMGSDI_3GPP2
    #ifdef FEATURE_BUILD_JCDMA
       #define FEATURE_UIM_JCDMA_RUIM_SUPPORT
    #endif /* FEATURE_BUILD_JCDMA */
  #endif

  #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) || defined (FEATURE_LTE)
    #define FEATURE_MMGSDI_3GPP
  #endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */

  /* -- T=1 PROTOCOL SUPPORT -- */
  /* Enable support for T=1 protocol */
  #define FEATURE_UIM_T_1_SUPPORT

  /* Enable QMI SIM INTERNAL service*/
  #ifdef FEATURE_8960_SGLTE_FUSION
    #define FEATURE_UIM_SIM_INTERNAL_INTERFACE
    #define FEATURE_UIM_DUAL_MODEM_SGLTE
    #define FEATURE_GSTK_TAL
  #endif /* FEATURE_8960_SGLTE_FUSION */

  /*---------------------------------------------------------------------------
                                  MMGSDI
  ---------------------------------------------------------------------------*/
  /* MMGSDI Task support */
  #ifdef FEATURE_MMGSDI

    /* FEATURE_UIMDIAG is required by I&T, MST etc., but some customers
       require that it be switched off in Z builds delivered to them.
       If not required, this will be undef in the target level builds file */
    #define FEATURE_UIMDIAG

    /* Enable personalization only if FEATURE_CUST_1 is not defined */
    #ifndef FEATURE_CUST_1
      /* PERSONALIZATION Features*/
      #define FEATURE_SIMLOCK
    #endif /* !FEATURE_CUST_1 */

    #if defined(FEATURE_SIMLOCK) && defined(FEATURE_SIMLOCK_RSU) && defined(FEATURE_CGPS) && defined(FEATURE_CGPS_LOC_MIDDLEWARE)
      /* Feature for SIMLOCK - QMI LOC interactions */
      #define FEATURE_SIMLOCK_QMI_LOC
    #endif /* FEATURE_SIMLOCK && FEATURE_SIMLOCK_RSU && FEATURE_CGPS && FEATURE_CGPS_LOC_MIDDLEWARE */

    #ifdef FEATURE_ECALL_APP
      /* Feature for MULTI SIM profile */
      #define FEATURE_UIM_MULTI_PROFILE 
    #endif /* FEATURE_ECALL_APP */

  #endif /* FEATURE_MMGSDI */

  #ifdef FEATURE_QMI_UIM_SECURE_CHANNEL_TEST
    #error code not present
#endif /* FEATURE_QMI_UIM_SECURE_CHANNEL_TEST */

  /*--------------------------------------------------------------------------
                         TOOLKIT / GSTK Support
  ---------------------------------------------------------------------------*/
    #define GSTK_MAX_NUM_OF_INSTANCES 4

  /*---------------------------------------------------------------------------
                            UIM Debug Features
  ---------------------------------------------------------------------------*/
  #ifndef TEST_FRAMEWORK
    /* Feature used to enable logging of UARTDM register operations
       into a global circular buffer */
    #define FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING
  #endif /* TEST_FRAMEWORK */
 /*=========================================================================== */

  /*************************************************
   PLATFORM SPECIFIC CUST FILE INCLUSIONS ONLY
   *************************************************/

  /* Platform specific custuim file inclusions */
  #if defined (TEST_FRAMEWORK)
    #error code not present
#endif

  #ifdef FEATURE_MODEM_RCINIT
    #define FEATURE_UIM_USES_TASK_HANDLE /* Feature to use TASK_HANDLE */
  #endif

  #ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
    #define FEATURE_UIM_SUPPORT_DUAL_SLOTS
    #undef  FEATURE_UIM_SUPPORT_TRIPLE_SLOTS
  #endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

  #if defined (FEATURE_UIM_RUIM) && defined (FEATURE_UIM_AUTH_CDMA_DF_FIRST)
    #define FEATURE_UIM_AUTH_CDMA_DF_FIRST_DEFAULT TRUE
  #else
    #define FEATURE_UIM_AUTH_CDMA_DF_FIRST_DEFAULT FALSE
  #endif

#endif /* CUSTUIM_H */
