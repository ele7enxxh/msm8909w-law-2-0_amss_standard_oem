#ifndef MODEM_1X_DEFS_H
#define MODEM_1X_DEFS_H
/*===========================================================================
                      M O D E M   1 X   D E F S

DESCRIPTION
  This file contains 1x modem definitions.

  Copyright (c) 2009 - 2016 Qualcomm Technologies, Inc.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cust/inc/modem_1x_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/16   agh     Support QSH framework to collect periodic/event metrics
04/14/16   srk     Mainline optimized Tx Trigger.
02/05/16   pk      1x2GTA Feature changes
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB
10/21/15   eye     Define RF for Y1Y2 verification on RUMI.
09/15/15   agh     Add changes to enable voice adapter by default.
09/09/15   eye     Added new macro for VFR register usage.
08/21/15   ab      Undef ASDIV feature for RUMI builds
04/21/15   srk     Unification of IDLE DIV algorithm for all CRAT modes.
03/18/15   agh     Mainline feature flag FEATURE_LTE_RESEL_SPLMN_SUPPORT
03/16/15   ab      Enhanced Debug buffer for Idle State
03/05/15   ssh     General Cleanup.
02/27/15   eye     Added FEATURE_MODEM_1X_DRX, DRX feature for THOR.
02/27/15   ppr     Enable FEATURE_1X_CP_HEAP_MEM_CLEANUP
02/12/15   jh      Added FEATURE_MODEM_1X_SW_DEMBACK
01/09/15   srk     Feature cleanup.
12/02/14   agh     Enable customer specific voice SO feature on Thor
11/26/14   agh     Enable G->1X related features on BO 2.5
11/11/14   agh     Enable feature flag for G->1X related features for Jolokia
                   and Tabasco
10/30/14   ab      Enable unified COEX Priority
10/09/14   mmk     Enable FEATURE_MODEM_1X_TABASCO
09/17/14   ppr     Enable FEATURE_MODEM_1X_CP_NOTIFY_STATE feature
09/05/14   srk     Add Jolokia and Thor Related features.
08/13/14   pap     Adding updated IDLE DIV algorithm for DR-DSDS cases.
06/24/14   as      Enable new TRM APIs via MCS feature
05/28/14   as      Jolokia compile clean-up
05/09/14   as      Changes to define the PL dynamically
04/24/14   cjb     Bring-up feature removal and mainlining
04/16/14   cjb     Disabling the SRCH bring-up feature on BOLT
04/09/14   ssh     Added 1x Acq Priority Inversion support.
03/19/14   ppr     SRVCC Feature Changes
03/17/14   agh     FR 16951: Avoiding BSR from C2K->LTE when UE supports
                   C2K-LTE Cell Reselection
03/06/14   bb      Changes to define expensive neighbor search during
                   QPCH offline features
03/04/14   ab      Define local feature for DI.2.1.2
03/03/14   ssh     1xSRLTE feature code.
02/13/14   dkb     Add support for optimized Tx trigger.
01/16/14   pk      Enable feature FEATURE_MODEM_1X_BATCH_ACQ
01/08/14   ab      DDR power management Support / Feature Cleanup
02/11/14   bb      Changes to define Wakeup Manager feature for DSDS enabled
                   targets
02/03/14   dkb     Feature guard Triton UTX code under FEATURE_MODEM_1X_UTX
01/20/14   jh      Add FEATURE_1X_CHANGE_TRM_REASON_TO_MAINTENANCE flag
12/10/13   as      Conditionally enable FEATURE_MODEM_1X_SRCH_QDSS
11/27/13   jh      Add FEATURE_MODEM_1X_FEATURE_PBR_MARKOV flag
11/25/13   cjb     Switched logic for FEATURE_MODEM_1X_BOLT_OBSOLETE feature.
                   Mainlined FEATURE_MODEM_1X_TRITON.
11/06/13   pk      Enable feature ACQ_USES_FAILSAFE_RECOVERY
11/05/13   cjb     Enabling debug messages for SOD bring-up
10/31/13   gga     FR 17021: R8 CSFB Call setup optimization
10/03/13   vks     cleanup
10/03/13   vks     Delete FEATURE_SRCHZZ_TLC_SLEEPCTL_DEBUG
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
09/30/13   as      Implement SLPC single interrupt changes
09/25/13   cjb     Enable RF support on BOLT RUMI
08/28/13   pap     Mainlining features FEATURE_MODEM_1X_INHIBIT_VOC_ENABLE,
                   FEATURE_MODEM_1X_SUPPORTS_MCVS and
                   FEATURE_MODEM_1X_USE_ATMR_IN_RC11.
08/28/13   cjb     Mainline FEATURE_MODEM_1X_DIME
08/27/13   cjb     Mainline FEATURE_MODEM_1X_TRITON_REVISIT
08/27/13   cjb     Mainline FEATURE_MODEM_1X_DIME_REVISIT
08/23/13   vks     Add FEATURE_MODEM_1X_BOLT_OBSOLETE
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
08/20/13   dkb     Mainline FEATURE_INIT_2
08/19/13   cjb     Mainline FEATURE_1X_SRCH_ROT_ONLY_AFC and
                   FEATURE_1X_SRCH_AFC_SRL ( Removed VCTCXO support )
08/19/13   cjb     Mainline FEATURE_MODEM_1X_SRCH_TC_PC_STATE_ACCEL
08/19/13   cjb     Mainline FEATURE_MODEM_1X_QOF_PILOT_NOISE_EST
08/16/13   vks     Mainline FEATURE_TC_TRIAGE_ENH, FEATURE_ZZ2_2 and
                   FEATURE_SRCH_DRX_SUPPORTS_ONE_BAND
08/14/13   cjb     Added BOLT modem support
06/27/13   ppr     Enable FEATURE_1X_CP_IGNORE_GSRDM for DI.2.0+ and BO.1.0
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
05/31/13   as      Implemented QDSS
05/14/13   eye     Unifying Device only DTX for RC3/4 and RC8.
05/14/13   srk     Enable AsDIV for 1x.
05/16/13   dkb     Enable FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB for DI.2.1
04/12/13   cjb     Fixed debug messages.
04/02/13   pap     Feature cleanup
03/29/13   bph     Mainline RX_DIV_RT_CTRL, DYN_MEM, and RXD_IN_ACCESS
03/26/13   ppr     Enable FEATURE_PPP_CLEANUP_NON_SV_MODE feature
03/15/13   cjb     Triton revist clean-up. Removed NikeL features.
03/14/13   ppr     Disabling FEATURE_PPP_CLEANUP_NON_SV_MODE feature
03/11/13   ppr     Added FEATURE_PPP_CLEANUP_NON_SV_MODE
03/05/13   cjb     Undefine RUMI feature for TRITON.
03/02/13   cjb     Mainline SLPC implementation.
02/28/13   ppr     Added FEATURE_PPP_CLEANUP_SV_MODE (ppp cleanup SV mode)
02/26/13   cjb     Enable TSYNC support for DIME PLs.
02/25/13   trc     Enable FEATURE_MODEM_1X_QOF_PILOT_NOISE_EST
01/02/13   ab      Added Enhanced RDDS Support
01/23/13   trc     Decouple TSYNC and its QMI interface
01/10/13   cjb     Replace T_MDM9615_LE_NO_VOICE with new VOICE disable feature.
01/09/13   trc     Enable MCVS support for Dime (FEATURE_MODEM_1X_SUPPORTS_MCVS)
12/04/12   ssh     Corrected SMS over CSFB Feature Definition location.
11/19/12   ssh     Added SMS over CSFB support.
11/09/12   srk     Enable Device only DTX in DIME.
11/02/12   vks     Initial support for antenna selection diversity feature.
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
10/19/12   vks     Use 1x internal feature (FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB)
                   for HDOn-EVRC feature (FEATURE_VOC_4GV_WB_ON_NB)
11/09/12   bb      Changes to define FEATURE_TC_TRIAGE_ENH feature
10/18/12   srk     Added a 1x only feature to assign different defaults based
                   on targets.
09/15/12   ppr     Defined "FEATURE_1X_CP_MEM_OPT" for FR#2370 - 1x memory
09/15/12   ppr     Defined "FEATURE_1X_CP_MEM_OPT" for FR#2370 - 1x memory
                   optimization changes
08/02/12   srk     Enable Device only DTX feature when 1x advanced is enabled.
09/13/12   cjb     DIME feature cleanup and adding new TRITON feature.
08/28/12   cjb     Disable RUMI feature for DIME.
                   Added FEATURE_MODEM_1X_DEBUG_MSGS for DEBUG messages.
08/14/12   cjb     Added changes to use SLPC(new sleepctl replacement module).
05/31/12   trc     Enable adaptive FW clock mgmt
05/16/12   srk     Feature Cleanup
03/15/12   jtm     Enable voice support if T_MDM9615_LE_NO_VOICE is not defined.
03/06/12   srk     Removed unnecessary RCInit feature undefine.
02/16/12   srk     Introduced FEATURE_MODEM_1X_IRAT_LTO1X as an internal
                   feature for FEATURE_LTE_TO_1X.
01/31/12   srk     Feature Clean Up
01/24/12   sst     Implementation of RIN
01/23/12   srk     Feature Cleanup
01/20/12   adw     Enable ATIM.
12/20/11   srk     Added FEATURE_MODEM_1X_SUPPORTS_RCINIT for RCInit Framework
12/08/11   vks     Re-enable offline qpch.
11/29/11   sst     Temporarily undefine FEATURE_1X_TO_LTE
11/28/11   sst     Add support for TRM Band awareness and Opp RxD
11/10/11   adw     Enable PC accel and remove obsolete defines.
11/08/11   adw     Re-enable ATIM.
11/07/11   jtm     Integrate ZZ2_2 support.
11/03/11   adw     Temporarily disable ATIM.
10/31/11   jtm     Feature clean up. Mainlines Second Chain Test.
10/24/11   bb      Enable QLIC QOF support on NikeL
10/14/11   trc     Enable NS QLIC support on NikeL, leave QLIC QOF disabled
10/13/11   cjb     Enable FEATURE_1X_TO_LTE for NikeL.2.0 & later targets.
09/15/11   srk     Enable FEATURE_MSO_RDA_TEST for extra Markov test capability
08/18/11   vks     Clean up references to sample server which does not exist
                   anymore (since genesis).
08/18/11   adw     Mainline 1x message router support.
08/15/11   adw     Enable second chain test [Option-2] capability.
08/15/11   vks     Mainline FEATURE_MODEM_1X_SRCH_SUPPORTS_NEW_TASK_PRI_API
08/08/11   cjb     Mainline FEATURE_ONEX_USES_RF_SCRIPTS_FOR_IRAT.
07/11/11   vks     Mainline FEATURE_SRCH_RX_STM and FEATURE_SRCH_RX_SVDO.
07/06/11   jtm     Check for voice support and 1xA for EVRC_NW verification.
06/28/11   vks     Changes to get 1xA working on nikel.
06/16/11   adw     Temporarily enable sleep log buffer.
06/09/11   vks     Enable FEATURE_MODEM_1X_SUPPORTS_TURBO_CODING for nikel.
05/26/11   adw     Refactor voice support defines and enable voice for NikeL.
04/26/11   vks     Remove explicit RUMI feature define.
04/14/11   adw     Change to ensure QLIC is properly enabled on NikeL.
04/13/11   vks     Added support for new task priority api.
04/06/11   adw     Enable QLIC by default on NikeL.
03/23/11   vks     Add support for RX/TX Link Manager feature for Nikel.
03/23/11   adw     Enable onex_non_sig on NikeL.
03/22/11   cjb     Second_Chain_Test [Option-2] implementation.
03/15/11   ssh     Undefined FEATURE_MODEM_1X_VOICE_SUPPORT for GOBI targets.
03/13/11   adw     Correct ET feature name used to trigger 1x ET support.
03/13/11   adw     Disable MSO RDA test feature.
03/09/11   vks     Fix compiler error.
03/02/11   adw     Temporarily undefine passport mode feature on Pegasus.
03/01/11   jtm     Removed FEATURE_MODEM_1X_SUPPORTS_RLP_SU_API.
02/25/11   vks     Explicitly define RUMI feature for Nikel.
02/18/11   bb      Define FEATURE_SRCH_SUPPORTS_NS_TRIAGE feature, if
                   1X Advanced feature is ON
02/17/11   cjb     Added feature protecting DRx support for only one band.
02/17/11   ssh     Defining FEATURE_HIGH_PSIST for JCDMA requirements.
02/16/11   vks     Moved 1x internal features from custsrch.h to
                   modem_1x_defs.h.
02/16/11   ssh     Mainlining JCDMA specific requirements.
02/07/11   ssh     Removed the umbrella feature for FEATURE_1X_WDOG_DYNAMIC.
02/08/11   jtm     Define FEATURE_MODEM_1X_SUPPORTS_RLP_SU_API for Poseidon.
02/02/11   vks     Define RUMI feature.
02/01/11   jtm     Define FEATURE_MSO_RDA_TEST.
01/26/11   vks     Define FEATURE_MODEM_1X_SUPPORTS_RF.
                   Define FEATURE_SRCH_RX_STM if RF SVDO feature is defined.
01/25/11   adw     Cleaned up Pre-Candidate state accel featurization.
01/21/11   vks     Replace FEATURE_SRCH_RF_SVDO_API with FEATURE_1X_RF_SVDO_API
01/21/11   adw     Added support for MCPM.
01/21/11   adw     Added ET feature trigger for Pegasus.
01/13/11   ssh     Added the FEATURE_1X_WDOG_DYNAMIC on MODEM-LEVEL FEATURE
                   DEFINES section.
01/11/11   adw     Remove spurious FEATURE_1X_WDOG_DYNAMIC define.
01/06/11   vks     Enable 1x control of Tx Conditioning Clocks for Genesis.
01/05/11   adw     Disable 1x bring-up features for Pegasus.
12/28/10   ag      Added check to ensure feature for SO73 is defined when
                   feature for 1xAdv is defined.
12/17/10   ssh     Defined FEATURE_1X_WDOG_DYNAMIC.
12/14/10   vks     Clean up nikel featurization.
12/10/10   vks     Define FEATURE_MODEM_1X_NIKEL_REVISIT.
12/09/10   vks     Define explicity Nikel and Genesis modem features for now
                   for the main/latest (nikel) builds.
                   Undefine voice support under poseidon feature if genesis
                   modem feature defined.
12/01/10   vks     Remove target verification check.
                   Define FEATURE_MODEM_1X_NIKEL for nikel 1x modem (currently
                   defined if 9K target feature defined).
                   Define FEATURE_MODEM_1X_OWNS_TX_COND_CLKS for nikel.
11/18/10   jtm     Enable FEATURE_MODEM_1X_OWNS_TX_COND_CLKS for Pegasus modems.
11/15/10   jtm     Enable FEATURE_MODEM_1X_SUPPORTS_SU_API.
11/12/10   vks     Define feature for 1x SRCH task supporting AFLT srch4 search
                   responses. Define this feature for MDM9K 2.2PL.
11/08/10   adw     Enable 1x bring-up feature for Pegasus.
11/04/10   adw     Enabled support for RxD in Access.
11/01/10   adw     Added support for Pegasus modem.
10/22/10   adw     Enable dynamic NV config of Pre-Candidate state accel and
                   srch support of RxD in access.
10/15/10   adw     Enable Pre-Candidate acceleration based on active set size.
10/11/10   ag      Enabled FEATURE_HWID_SUPPORT.
09/20/10   ag      Enabled FEATURE_MODEM_1X_INHIBIT_VOC_ENABLE.
09/20/10   adw     Enable voice support for all Poseidon PLs.
09/15/10   vks     Add support for the new dynamic watchdog api.
09/14/10   vks     Use FEATURE_MODEM_1X_VOICE_SUPPORT to enable 1X voice
                   support.
                   Undefine this feature for MDM9K target which does not
                   support voice.
08/19/10   mca     Turn sleep on for 8660
08/18/10   vks     Undef the telescoping of SCI feature.
08/13/10   jtm     Remove undef of FEATURE_MODEM_1X_USE_RLP_LOG_TYPES for
                   off-target builds.
08/05/10   vks     Remove FEATURE_MODEM_1X_SRCH_COMMON_GROUP_REMAIN_ACTIVE
                   Remove FEATURE_MODEM_1X_BRINGUP.
                   Use FEATURE_MODEM_1X_DISABLE_QPCH_OFFTL instead of
                   FEATURE_MODEM_1X_BRINGUP to disable QPCH OFFTL.
                   Mainline FEATURE_MODEM_1X_USE_RLP_LOG_TYPES.
08/04/10   vks     Remove FEATURE_MODEM_1X_GENESIS_1_0 support. Replace
                   FEATURE_MODEM_1X_GENESIS_1_1 with FEATURE_MODEM_1X_GENESIS.
                   Define FEATURE_MODEM_1X_GENESIS_SRCH_HW_WORKAROUND for the
                   changes related to SRCH HW workaround.
                   Remove FEATURE_1XCP_Q6 (as CP moved to
                   FEATURE_MODEM_1X_SUPPORTS_Q6 )
07/30/10   jtm     Allow FEATURE_MODEM_1X_USE_RLP_LOG_TYPES to be defined for
                   off-target builds.
07/26/10   mca     Disable sleep for 8660
07/22/10   bb      Define "FEATURE_ONEX_USES_RF_SCRIPTS_FOR_IRAT"
                   if "LTE to 1X IRAT" feature is enabled
07/17/10   vks     Define the feature to recover from large reacq slews
07/15/10   jtm     Define FEATURE_MODEM_1X_MVS_API_GEN2 for 8660 targets.
07/13/10   jtm     Undef FEATURE_MODEM_1X_USE_RLP_LOG_TYPES in off-target builds.
07/13/10   jtm     Allow FEATURE_MODEM_1X_USE_RLP_LOG_TYPES to be defined in
                   off-target builds.
07/12/10   bb      Defined QLIC Management feature
06/29/10   jtm     Temporarily define FEATURE_MODEM_1X_USE_RLP_LOG_TYPES for
                   target builds. Undef'd for off-target for now.
06/16/10   jtm     Corrected use of 1xAdv feature.
06/16/10   pk      Added 1xAdvanced QLIC feature
06/15/10   jtm     Defined FEATURE_PLCM_ENHANCEMENTS, FEATURE_HHO_ENHANCEMENTS,
                   and FEATURE_MSM6500_DEC_DYNAMIC_CLK_OFF. Thsse used to be
                   defined at the target level. Now 1x is taking ownership.
06/01/10   vks     Define dynamic memory feature.
05/20/10   vks     Undef dynamic memory as it is breaking FTM.
05/19/10   vks     Turn on FEATURE_MODEM_1X_NEW_RF_API_MSGR (as RF is moving
                   to the new api)
05/19/10   cjb     Enable dynamic memory for SRCH.
05/18/10   vks     Mainline FEATURE_MDSP_SUPPORTS_TX_AGC.
05/17/10   vks     Remove include of RF file from previous check-in.
05/17/10   vks     Define temp feature for changes required in L1 for new
                   RF-MSGR api
03/30/10   vks     Define 1X feature for Q6 support to be used by all 1X files
                   instead of the target feature - FEATURE_QDSP6.
03/25/10   vks     Define features to distinguish between Genesis 1.0 and 1.1
03/25/10   vks     Define Gen2 MVS API feature.
02/19/10   adw     Enable the use of the RF API to get RF warmup time.
01/25/10   adw     Added AFC related feature defines.
01/25/10   adw     Added back in target define table and feature documentation.
01/05/10   jtm     Added definition for FEATURE_MDSP_SUPPORTS_TX_AGC to ensure
                   that 1x has control over this feature.
11/20/09   bb      Removed "FEATURE_MODEM_1X_SRCH_COMMON_GROUP_REMAIN_ACTIVE"
                   feature definition.
11/16/09   adw     Trigger NPA support based on FEATURE_NPA.
11/11/09   adw     Enable the modem bringup feature for Genesis modem.
11/11/09   vks     Enable 1X Bringup fetaure for Genesis
11/06/09   vks     Enable MSGR support for Genesis
11/09/09   adw     Re-disabled bringup feature for Poseidon modem.
10/28/09   sst     Update SVDO RF & TRMv3 features
10/19/09   adw     Added feature to force common stm group to remain on.
10/02/09   sst     SVDO RF & TRMv3 features
09/24/09   adw     Enabled slotted sleep on poseidon by disabling the modem
                   bringup feature. Disabled sleep for genesis bringup.
09/18/09   adw     Added support for searcher dynamic memory allocation.
09/14/09   adw     Enable support for Node Power Architecture (NPA).
07/28/09   mca     Added Genesis support
06/12/09   jtm     Added check for invalid attempt to enable 1xA.
04/20/09   jtm     Added definition of FEATURE_MODEM_1X_NEW_RF_API
04/03/09   adw     Removed target defines and enabled 1x bringup feature.
04/02/09   jtm     Added definition of FEATURE_MODEM_1X_MVS_API_GEN2 for
                   8650B. For now no other targets use this api.
03/24/09   adw     Initial revision.

===========================================================================*/

#include "1x_variation.h"
#include "customer.h"

/*===========================================================================
                                 MODEM DEFINES
===========================================================================*/
/* Enable the modem based on target configuration */
#if defined( FEATURE_TABASCO_MODEM )
  #define FEATURE_MODEM_1X_TABASCO
#elif defined( FEATURE_THOR_MODEM )
  #define FEATURE_MODEM_1X_THOR
#elif defined( FEATURE_JOLOKIA_MODEM )
  #define FEATURE_MODEM_1X_JOLOKIA
#elif defined( FEATURE_BOLT_MODEM )
  #define FEATURE_MODEM_1X_BOLT
#else
  #error Unknown modem specification
#endif

/*===========================================================================
                        MODEM-SPECIFIC FEATURE DEFINES
===========================================================================*/

/* ------------------------------------------------------------------------
**                      TABASCO
** ------------------------------------------------------------------------ */
#ifdef FEATURE_MODEM_1X_TABASCO

  /* REVISIT feature for TABASCO */
  #define FEATURE_MODEM_1X_TABASCO_REVISIT

  /* Leverage from JOLOKIA */
  #define FEATURE_MODEM_1X_JOLOKIA

#endif /* FEATURE_MODEM_1X_TABASCO */

/* ------------------------------------------------------------------------
**                      JOLOKIA
** ------------------------------------------------------------------------ */
#ifdef FEATURE_MODEM_1X_JOLOKIA

  /* REVISIT feature for JOLOKIA */
  #define FEATURE_MODEM_1X_JOLOKIA_REVISIT

  /* Leverage from BOLT */
  #define FEATURE_MODEM_1X_BOLT

  /* UTX and Single DEMBACK are only used in JOLOKIA */
  #define FEATURE_MODEM_1X_UTX
  #define FEATURE_MODEM_1X_SINGLE_DEMBACK

  /* VFR register usage specific to JO and TA */
  #define FEATURE_MODEM_1X_VFR_SEL_REG

#endif /* FEATURE_MODEM_1X_JOLOKIA */

/* ------------------------------------------------------------------------
**                      THOR
** ------------------------------------------------------------------------ */
#ifdef FEATURE_MODEM_1X_THOR

  /* REVISIT feature for THOR */
  #define FEATURE_MODEM_1X_THOR_REVISIT

  /* Leverage from BOLT */
  #define FEATURE_MODEM_1X_BOLT

  /* 1x/1xA DRX feature */
  #define FEATURE_MODEM_1X_DRX

  /* Feature to support RF MEASUREMENT INTERFACE_V2 */
  #define FEATURE_ONEX_SUPPORTS_RF_MEAS_INTF_V2

  /* ------------------------------------------------------------------------
   ** Software Demback - Needs supporting FW
   ** ------------------------------------------------------------------------ */
  #define FEATURE_MODEM_1X_SW_DEMBACK

  /* Debug messages for bringup */
  #define FEATURE_MODEM_1X_DEBUG_MSGS

  /* ------------------------------------------------------------------------
   ** 1X QTA
   ** ------------------------------------------------------------------------ */
  #ifdef FEATURE_MODEM_COEXISTENCE_FW
    #define FEATURE_MODEM_1X_QTA
  #endif /* FEATURE_MODEM_COEXISTENCE_FW */

#endif /* FEATURE_MODEM_1X_THOR */

/* ------------------------------------------------------------------------
**                      BOLT
** ------------------------------------------------------------------------ */
#ifdef FEATURE_MODEM_1X_BOLT

  /* REVISIT feature for BOLT */
  #define FEATURE_MODEM_1X_BOLT_REVISIT

  /* Registers/api obsolete on bolt. Code for reference only. */
  #undef FEATURE_MODEM_1X_BOLT_OBSOLETE

  /* Enable TSYNC here, when HWIO is ready. Enable QMI when API is present   */
  #define FEATURE_MODEM_1X_TSYNC
  #undef FEATURE_MODEM_1X_TSYNC_QMI

  /* Feature for PPP cleanup changes when moving from 1xtoL in SV mode       */
  #define FEATURE_PPP_CLEANUP_SV_MODE

  /* Feature for PPP cleanup changes when moving from 1xtoL in NON_SV mode   */
  #define FEATURE_PPP_CLEANUP_NON_SV_MODE

  /* Enable extra Markov test capability for additional
     statistic gathering and analysis. */
  #define FEATURE_MODEM_1X_MSO_RDA_TEST

  #if defined (FEATURE_MODEM_1X_MPSS_BO_1_1) || defined (FEATURE_MODEM_1X_MPSS_BO_2_1) || \
      defined (FEATURE_MODEM_1X_MPSS_TH_1_1) || defined (FEATURE_MODEM_1X_MPSS_TH_2_1)
    /* MPSS.BO.1.1/BO.2.1 or TH1.1/2.1 Modem specific features */
    #define FEATURE_MODEM_1X_ALT_DEFAULTS
  #endif /* FEATURE_MPSS_BO_1_1 || FEATURE_MODEM_1X_MPSS_BO_2_1 */

  #ifdef FEATURE_TRM_V6
    /* Enable V6 TRM APIs*/
    #define FEATURE_MODEM_1X_DUAL_RECEIVE
  #endif /* FEATURE_TRM_V6 */

  /* Temporary until custrela.h can be edited */
  #undef FEATURE_IS2000_REL_A_CC
  #undef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
  #undef FEATURE_IS2000_REL_A_CC_SMMC
  #undef FEATURE_IS2000_REL_A_CC_QPM
  #undef FEATURE_IS2000_REL_A_CC_BCSMS

  #ifdef FEATURE_1X_BUILD_QDSS
    /* Feature for QDSS timeline profiling */
    #define FEATURE_MODEM_1X_SRCH_QDSS
  #endif /* FEATURE_1X_BUILD_QDSS */

#endif /* FEATURE_MODEM_1X_BOLT */

/* ------------------------------------------------------------------------
** AFC Related Control Features
** ------------------------------------------------------------------------ */
#ifndef FEATURE_XO
  #error FEATURE_XO not defined. 1x supports XO targets only
#endif /* FEATURE_XO */

/* ------------------------------------------------------------------------
** 1x Voice support
** ------------------------------------------------------------------------ */
#ifndef FEATURE_MODEM_NO_VOICE_SUPPORT
/* Enable Voice support */
#define FEATURE_MODEM_1X_VOICE_SUPPORT
#endif /* FEATURE_MODEM_NO_VOICE_SUPPORT  */

#if defined(FEATURE_VOC_4GV_WB_ON_NB) && defined (FEATURE_MODEM_1X_ALT_DEFAULTS)
/* Audio HDOn-EVRC feature */
  #define FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
#endif

/* ------------------------------------------------------------------------
** 1xAdvanced - SO73 feature
** ------------------------------------------------------------------------ */
#if defined(FEATURE_IS2000_1X_ADV) && defined(FEATURE_MODEM_1X_VOICE_SUPPORT)
/* SO73 Support must be enabled for 1xAdvanced */
#ifndef FEATURE_VOC_4GV_NW
#error FEATURE_VOC_4GV_NW to support SO73 must be enabled for 1xAdvanced!
#endif /* FEATURE_VOC_4GV_NW */
#endif /* FEATURE_IS2000_1X_ADV && FEATURE_MODEM_1X_VOICE_SUPPORT */

/* ------------------------------------------------------------------------
** RF related features
** ------------------------------------------------------------------------ */
#define FEATURE_MODEM_1X_SUPPORTS_RF

/* ------------------------------------------------------------------------
** PBR related features
** ------------------------------------------------------------------------ */
/* Page Blocking */
#ifdef FEATURE_PBR_MARKOV
  #define FEATURE_MODEM_1X_FEATURE_PBR_MARKOV

  #if !defined(FEATURE_MODEM_1X_MPSS_BO_1_1) && !defined(FEATURE_MODEM_1X_MPSS_BO_1_0)
     /* After decoding all General Page Messages, and when going from Slew to
        Paging channel, change TRM priority to TRM_CHANNEL_MAINTENANCE */
    #define FEATURE_1X_CHANGE_TRM_REASON_TO_MAINTENANCE
  #endif /* !FEATURE_MODEM_1X_MPSS_BO_1_1 && !FEATURE_MODEM_1X_MPSS_BO_1_0 */
#endif /* FEATURE_PBR_MARKOV */

/* ------------------------------------------------------------------------
** Software Demback - Needs supporting FW
** ------------------------------------------------------------------------ */
#undef FEATURE_MODEM_1X_SW_DEMBACK

/* Define FEATURE_HIGH_PSIST for KDDI specific requirement. */
#define FEATURE_HIGH_PSIST

/* Feature for new TRM development for Oppurtunistic Rx Diversity, dealing
   with adding band class knowledge to TRM */
#define SRCH_OPP_RXD

#ifdef FEATURE_CUST_1
  /* This feature is to notify MMODE about Access/Traffic state Entry/Exit */
  #define FEATURE_MODEM_1X_CP_NOTIFY_STATE
#endif /* FEATURE_CUST_1 */

/* ------------------------------------------------------------------------
** GSRDM Ignore feature support
** ------------------------------------------------------------------------ */
/* This feature is to ignore GSRDM msg.
   If SD says all channels in GSRDM list are permanent MAP system then
   MC will ignore the GSRDM msg till
   1. GSRDM IGNORE timer expiry OR
   2. Overhead information change
*/
#define FEATURE_1X_CP_IGNORE_GSRDM

/* ------------------------------------------------------------------------
** 1xAdvanced - QLIC Features
** ------------------------------------------------------------------------ */
#ifdef FEATURE_IS2000_1X_ADV
  /* Enable QLIC QOF Support for 1xAdvanced */
  #define FEATURE_SRCH_SUPPORTS_QLIC_QOF

  /* Enable adaptive FW clock speed settings */
  #define FEATURE_1X_ADAPTIVE_FW_CLK
#endif

/* ------------------------------------------------------------------------
** Device only DTX
** ------------------------------------------------------------------------ */
/* Enable Device Only DTX */
#define FEATURE_MODEM_1X_DEVICE_ONLY_DTX

/* ------------------------------------------------------------------------
** Memory Reduction Features
** ------------------------------------------------------------------------ */
/* Feature to enable FR#2370 - 1x memory optimization changes */
#define FEATURE_1X_CP_MEM_OPT

#ifdef FEATURE_MODEM_1X_JOLOKIA
/* Feature to enable timer based mech to clean up bs_info[] Array heap mem */
#define FEATURE_1X_CP_HEAP_MEM_CLEANUP
#endif /* FEATURE_MODEM_1X_JOLOKIA */

/* ------------------------------------------------------------------------
** Enhanced Receive Diversity Dynamic Switching
** ------------------------------------------------------------------------ */
#define FEATURE_ENHANCED_RDDS

/* ------------------------------------------------------------------------
** Antenna Selection Diversity
** ------------------------------------------------------------------------ */
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  /* Enable Antenna Switch Diversity */
  #define FEATURE_MODEM_1X_SRCH_ASD
#endif /* FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY */

/* ------------------------------------------------------------------------
** IRAT Features [ LTE -> 1x and 1x -> LTE ]
** ------------------------------------------------------------------------ */
#ifdef FEATURE_LTE
  #define FEATURE_1X_TO_LTE
#endif /* FEATURE_LTE */

#ifdef FEATURE_LTE_TO_1X
  #define FEATURE_MODEM_1X_IRAT_LTO1X

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    #define FEATURE_CSFB_CALL_SETUP_OPT
    /* Feature to reduce the call setup time for redirection based CSFB call */
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* ------------------------------------------------------------------------
** 1xSRVCC Feature Support
** ------------------------------------------------------------------------ */
#if defined(FEATURE_MODEM_1X_IRAT_LTO1X) &&  \
    defined(FEATURE_1X_SRVCC)
  /* Feature to support SRVCC(Single Radio Voice Call Continuity) scenarios */
  #define FEATURE_MODEM_1X_SRVCC
#endif /* FEATURE_1X_SRVCC */

  /* SMS over CSFB */
  #ifdef FEATURE_TUNNELED_SMS_OVER_CSFB
    #define FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    #define FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76
  #endif /* FEATURE_TUNNELED_SMS_OVER_CSFB */

  /* Define Simultaneous 1X LTE feature */
  #ifdef FEATURE_1XSRLTE
    #define FEATURE_SIMUL_1X_LTE
  #endif /* FEATURE_1XSRLTE */

#endif /* FEATURE_LTE_TO_1X */

/* ------------------------------------------------------------------------
** Coexistence Manager
** ------------------------------------------------------------------------ */
#if defined FEATURE_MODEM_COEXISTENCE_SW || defined FEATURE_MODEM_DDR_MGMT
  #define FEATURE_MODEM_1X_COEXISTENCE
#endif /* FEATURE_MODEM_COEXISTENCE_SW || FEATURE_MODEM_DDR_MGMT */

#ifdef FEATURE_MCS_WWCOEX_UNIFIED_PRIORITY
  #define FEATURE_1X_UNIFIED_COEX_PRIORITY
#endif /* FEATURE_MCS_WWCOEX_UNIFIED_PRIORITY */

#ifdef FEATURE_1X_TO_LTE
  /* --------------------------------------------------------------------
  ** Feature to enable avoiding BSR from C2K->LTE when UE supports
  ** C2K->LTE Cell Reselection
  ** -------------------------------------------------------------------- */
  #define FEATURE_1X_SPLMN_SUPPORT
#endif /* FEATURE_1X_TO_LTE */

/* ------------------------------------------------------------------------
** 1X throttling Mechanism
** ------------------------------------------------------------------------ */
#define FEATURE_1X_THROT

/* ------------------------------------------------------------------------
** 1X IDLE DEBUG BUFFER
** ------------------------------------------------------------------------ */
 #define SRCH_IDLE_DEBUG_BUFFER

/* ------------------------------------------------------------------------
** Batch Acquisition algorithm
** ------------------------------------------------------------------------ */
#define FEATURE_MODEM_1X_BATCH_ACQ

/* ------------------------------------------------------------------------
** Un-equal Sensitivity Acquisition algorithm
** ------------------------------------------------------------------------ */
#define FEATURE_MODEM_1X_UESIN

/* ------------------------------------------------------------------------
** Failsafe Acquisition
** ------------------------------------------------------------------------ */
#ifdef FEATURE_TCXOMGR_CLIENT_RGS_API
  #define ACQ_USES_FAILSAFE_RECOVERY
#endif /* TCXOMGR_CLIENT_RGS_API */

#ifdef FEATURE_DUAL_SIM
  #ifndef FEATURE_WAKEUP_OPTIMIZED
    /* Enable Wakeup Manager feature for DSDS targets */
    #define FEATURE_WAKEUP_OPTIMIZED
  #endif /* !FEATURE_WAKEUP_OPTIMIZED */
#endif /* FEATURE_DUAL_SIM */

/* ------------------------------------------------------------------------
** Expensive Neighbir pilot search during QPCH offline
** Refresh overheads for all expensive handoff scenarios
**------------------------------------------------------------------------*/
#define FEATURE_QPCH_OFFTL_SUPPORTS_EXPENSIVE_NBHR_SRCH

#if !defined(FEATURE_MODEM_1X_MPSS_BO_1_1) && !defined(FEATURE_MODEM_1X_MPSS_BO_1_0)
  /* Feature to support the below three optimizations:
   * 1. Caching of overheads across state transitions
   * 2. Validating expired overheads
   * 3. Direct Traffic to Idle transition without BS support
   */
  #define FEATURE_1X_OVHD_OPTIMIZATION

  /* Feature Flag to support 1X Split Overheads */
  #define  FEATURE_1X_SPLIT_OVERHEAD

  /* Feature flag to support powerup NV error reporting to CM */
  #define  FEATURE_MODEM_1X_PWRUP_NV_ERR

#endif /* !FEATURE_MODEM_1X_MPSS_BO_1_1 && !FEATURE_MODEM_1X_MPSS_BO_1_0 */

/* The functionality within feature FEATURE_ONEX_SUPPORT_REFRESH_OVHDS is a
 * subset of functionality of FEATURE_1X_OVHD_OPTIMIZATION. Hence both should
 * not be enabled together */
#ifndef FEATURE_1X_OVHD_OPTIMIZATION
  /* Legacy feature to validate expired overheads */
  #define FEATURE_ONEX_SUPPORT_REFRESH_OVHDS
#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

#if !defined(FEATURE_MODEM_1X_JOLOKIA) && !defined(FEATURE_MODEM_1X_BOLT)
  /* Feature to support customer specific new Voice SO */
  #define FEATURE_1X_CUST_VOICE_SO
#endif /* FEATURE_MODEM_1X_THOR */

/* DRVCC for an already established voice call from IWLAN to native 1X
 * and 1XCSFB */
#define FEATURE_MODEM_1X_DRVCC

/* ------------------------------------------------------------------------
** RUMI related features
** ------------------------------------------------------------------------ */
#ifdef T_RUMI_EMULATION
  #define FEATURE_MODEM_1X_RUMI
#endif

#ifdef FEATURE_MODEM_1X_RUMI

  /* Define RF in order to support Y1Y2 verification on RUMI */
  #define FEATURE_MODEM_1X_SUPPORTS_RF

  /* Define the 1x Bringup features */
  /* 1x-SRCH: Sleep/Fing debug support and disable DIV by default */
  #define FEATURE_MODEM_1X_BRINGUP_SRCH

  /* Debug messages for bringup */
  #define FEATURE_MODEM_1X_DEBUG_MSGS

  /* Enable ACQ debug capability */
  #define FEATURE_MODEM_1X_SRCH_ACQ_DEBUG

  /* Undefine ASDIV */
  #undef FEATURE_MODEM_1X_SRCH_ASD

  /* TODO: Disable sleep and all timelines, anything else? */

#endif /* FEATURE_MODEM_1X_RUMI */

/* Feature Flag to indicate that new Audio Architecture based on Voice Agent
 * and Voice Adapter is being used */
#define FEATURE_VOICE_AGENT_ARCHITECTURE 

/* Disable 1x->X QTA */
#undef FEATURE_ONEX_DATA_QUICK_TUNEAWAY

/* Feature flag to guard the changes made to support QSH framework by 1X_CP */
#define FEATURE_1XCP_QSH_SUPPORT

#endif /* MODEM_1X_DEFS_H */
