#ifndef CUSTGSMDRV_H
#define CUSTGSMDRV_H
/*===========================================================================

                           C U S T G S M D R V

DESCRIPTION
  Customer file for GSM Drivers.

  Copyright (c) 2002-2015 Qualcomm Technologies, Inc.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/cust/inc/custgsmdrv.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/05/16   akm     CR1014015 Enabled the feature flag FEATURE_GSM_SCE_REDUCED_NCELL_LIST
29/03/16   akm     CR739561 SCE opt top2/top6 mode, disable pwr msr of blacklisted cells
19/01/16   sk      CR963242 Corrected the featurization to work with SS targets
28/10/15   hd      CR930917 Added FEATURE_SINGLE_VFR_IN_FEERO - allocate primary VFR to both the SUBs
09/10/15   df      CR921075 FR23269 M2M devices: Handle RR->GL1 MPH_UPDATE_IDLE_DRX_IND
19/03/15   df      CR801738 Fix comments
01/12/14   jk      CR764612:Enabling SAWLESS feature flags
11/11/14   df      CR733947 Use EFS file settings to inject test delays in sleep code
24/09/14   jk      CR729382:Enable LIF feature by default
04/011/14  ab      CR750511: GSM Timeline reporting for WLAN COEX
01/11/14   df      CR748602 Rework and enable adaptive timeline
24/07/14   jk      CR697308:GL1 changes to support HLin LLin sawless feature
22/08/14   cgc     CR713634 obsoleted G2L code and featurisations FEATURE_GSM_G2L_TIMED_SRCH and FEATURE_CM_G2L removed
11/02/14   jk      CR614254:GL1 changes to support IP2 calibration
11/08/14   fan     CR703945 Adaptive warm up timeline
08/08/14   ka      CR706866 FR18739: DR-DSDS RxD Idle, part 2 - new TRM clients
07/08/14   cja     CR643648 Desense for channel adjacent to spur
06/08/14   df      CR536059 Allow Asynchronous sleep for all idle activities,
                   enabled by default
24/07/14   jk      CR699271:GL1 LIF changes for RxD
10/07/14   jk      CR688587:GL1 changes to support LIF feature
18/06/14   cos     CR682031 - GL1 control algorithm and changes for supporting AEQ PHASE 4
18/07/14   pg/pa   CR694601:GERAN L1 support for dedicated WTR hopping with FED architecture
02/07/14   mc      CR687340 : BOLT 2.0 : COEX : Mass Merge of COEX changes from DI4.0
01/07/14   dv      CR688068:Disable DR_DSDS for Jolokia only:Correction for CR687112
29/06/14   dv      CR687112 Enable DR_DSDS
30/05/14   pg      CR655759 Feature cleanup of FEATURE_BOLT_MODEM and FEATURE_DIME_MODEM
08/05/14   cos     CR661510 Enabling GSM Rx Diversity by default on Bolt for GL1
21/03/14   mc      CR.588853 : COEX : GL1 changes for Enhanced FCCH algorithm
19/03/14   cja     CR632037 Use RF Task for X2G
05/03/14   npt     CR626661 Re-enable parallel wake-up
03/03/14   npt     CR622060 Rework wake-up
15/02/14   ip/npt  CR547955 Define multi thread usage on wake up from sleep
30/01/14   npt     CR608373 Enable opt3 on Bolt
23/01/14   npt     CR606388 Temp disable opt3
09/11/13   mc      CR.572746 : COEX : DSDA G+G VBATT power back-offs for TxTx collisions are not correctly applied
19/11/13   ssh/ip  CR547956 DSDS/DSDA GL1 timeline Opt (parallelize the RF config. with GL1 warm-up)
15/11/13   nm      CR575532 Back-out changes for CR537690 & CR547955 - power optimization changes
08/11/13   ip      CR547955 - Define multi thread usage on wake up from sleep
22/11/13   cja     CR579775 On Dime use MCVS for settinig clocks for TCH AEq and speed-up
21/11/13   cja     CR455527 Remove featurisation from files in API directory
05/11/13   ssh     CR571474: Binding OPT3 with FEATURE_DISABLE_EARLY_DECODE
04/11/13   pa      CR570702:Featurisation of WCDMA DBDC W2G
01/11/13   pa      CR569692: X2G via RF Task.
14/10/13   pa      CR539505:Bolt: Added support for Dynamic Power Scheduling
14/10/13   pa      CR536820:Added GL1 Msgr IF Task.
09/10/13   ka      CR556038 Disable FEATURE_GSM_RFA_TASK_DEBUG
27/09/13   ip      CR549773 Enable power feature Option 3
03/09/13   npt     CR524770 - GL1 changes to support RFLM FED
21/08/13   npt     CR531633 - Remove FEATURE_DIME_MODEM dependencies
07/08/13   pa      CR521830: Bolt: Support for MCPM GSM Non EV Data setting for DL
18/06/13   npt     CR497521 - Added VSTMR changes
08/05/13   cgc     CR453758 add FEATURE_CM_G2L search functionality FR-3009
29/07/13   sk      CR519663 Partial QBTA bringup changes
09/05/13   kb/npt  CR385548 - Decode PCH after sleep wakeup error recovery.
19/03/12   ss      CR465249 Disable OPT3 for PCH decode
06/12/12   pg      CR421898: enable AEQ_SCH and AEQ_8PSK on Dime modem
21/11/12   npt     Disabled opt3 debug
19/11/12   ip/zf   CR341843: Opt16: Re-use SCH timing for re-acquiring Ncell.
19/11/12   ip      CR339131 - Enabled Field Power feature Option 17
14/11/12   npt     CR420954 - Added changes for power collapse
09/11/12   npt     CR380795 - Added core CPU vdd NPA client handle
09/11/12   npt     CR419261 - Enable opt3 for Dime
08/11/12   npt     CR419262 - Disabled opt3 and enabled sleep
23/10/12   npt     CR413635 - Port sleep code for DIME modem
03/09/12   zf/npt  CR339131: blacklist Ncell which fails FCB receive - opt17
16/08/12   zf      CR390228: Featurizing Opt 19 and Opt 20.
01/08/12   ky      Enabling the async intf feature
11/06/12   tjw     CR367302 - remove unused FEATURES (RFM_IDLE_FRAME_PROCESSING
                   GSM_MDSP_EXTRA_DM_NOPS, GL1_NEW_SLEEP_API, GSM_GPRS_MSC33_SUPPORTED
                   GSM_NIKEL_BU)
25/05/12   pg      Enable FEATURE_GSM_RFA_TASK
23/05/12   jj      CR364221 remove FEATURE_GSM_TX_PCL
18/05/12   ky      Async message router Interface Changes between SW & FW
04/05/12   jj      CR330315 Enable Feature FEATURE_GSM_TX_PCL
24/04/12   npt     CR354790 - Remove old sleep debug (under DEBUG_GL1_SLEEP) as it is not needed
                   after FEATURE_GL1_SLEEP_DEBUG_STRUCT is introduced
26/03/12   jj      CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
15/03/12   pg      Add feature FEATURE_GSM_SLPC_CONTROLLER for Dime modem
15/03/12   pg      Target feature MSM8974 now replaced with FEATURE_DIME_MODEM
14/03/12   pg      Add support for local HWIO macros
14/03/12   pg      Add support for DAL interrupt controller (tramp deprecated)
14/03/12   ip      CR318865 - Enabled scheduled wake up feature for 9x15
13/02/12   pm      Enabled FEATURE_GSM_GPRS_MSC34_SUPPORTED
01/02/12   pm      Disabled FEATURE_GSM_GPRS_MSC34_SUPPORTED temporarily
12/01/11   npt     CR311544 - Added sleep debug structure
                   FEATURE_GL1_SLEEP_DEBUG_STRUCT and FEATURE_GL1_SLEEP_DEBUG_STRUCT_F3
29/09/11   tjw     Duplicate definition of FEATURE_GSM_CB_DRX_SUPPORTED removed
06/07/11   rt      Disabling FEATURE_GSM_FCCH_FREQ_32_BIT
01/07/11   rt      Re-enabling FEATURE_GSM_8660_DYNAMIC_CLK_SWITCH and FEATURE_GSM_FCCH_FREQ_32_BIT
29/06/11   rt      Re-enabling FEATURE_GSM_GPRS_SLEEP_CANCEL_MIPS
28/06/11   rt      Temporarily undoing change to FEATURE_GSM_GPRS_SLEEP_CANCEL_MIPS
                   and disabling FEATURE_GSM_8660_DYNAMIC_CLK_SWITCH for patch release
23/06/11   npt     CR288201 Modified FEATURE_GSM_GPRS_SLEEP_CANCEL_MIPS
10/06/11   rt      Enable FEATURE_GSM_8660_DYNAMIC_CLK_SWITCH for 8660
20/04/11   rt      Enable FEATURE_GSM_L4_TRACE_BUFF
20/04/11   npt     CR276782 Canceling 340 MIPS when going to sleep
30/03/11   cja     Initial IRAT for Nikel
30/03/11   ws      Added GERAN_NIKEL_FW_INTERFACE define for GFW NIKEL SW IF
10/03/11   ws      CR276952 Added EBI bus request for EGPRS Data mover access
08/03/11   cja     Nikel RxLM/TxLM
03/03/11   pg      Enable GL1_9K_2P4_GSM_RXF0_SAMP_CLK_WORKAROUND and
                   GL1_9K_2P4_GSM_OFFLINE_CLK_WORKAROUND for MDM9K 2.4PL
                   previously defined in source files for all targets
23/02/11   pg      Enable FEATURE_GSM_GPRS_MSC34_SUPPORTED for Pegasus
31/01/11   rc      Enabled FEATURE_GSM_GPRS_MSC34_SUPPORTED
26/01/11   pg      Disable FEATURE_GSM_GPRS_MSC34_SUPPORTED temporarily for Pegasus
26/01/11   pg      Enable FEATURE_GSM_GP_CLK_DISABLED for Pegasus
26/01/11   pg      Enable FEATURE_WCDMA_G2W_SWC for Pegasus
21/01/11   rc      Disabled FEATURE_GSM_GPRS_MSC34_SUPPORTED temperorily
21/01/11   rc      Enabled Opt-3
14/01/11   ip      Enabled Option 3 & 4 power feature for 7x30
12/01/11   ws      Updates for NikeL MSM8960 target
13/12/10   pm      Enabled HMSC Poseidon targets
10/12/10   ws      Corrected missing #define from FEATURE_INTERRAT_SEND_TRK_LO_ADJ_GTOW
02/12/10   pm      Disable HMSC
24/11/10   cja     Integrate HMSC
21/10/10   cja     CR259899 Remove FEATURE_GSM_MAINTAIN_WCDMA_FRC from Poseidon
18/10/10   ps      Enabled FEATURE_GSM_MDSP_AEQ_8PSK
09/10/10   nf      9K removed FEATURE_GSM_MAINTAIN_WCDMA_FRC
17/09/10   ps      Enabled FEATURE_GSM_MDSP_AEQ_SCH and moved
                   FEATURE_GSM_WDOG_DYNAMIC to custgsm.h
16/09/10   pg      CR256046: Added FEATURE_GSM_WDOG_DYNAMIC for 9K
15/09/10   ip      CR249603, Added FEATURE_DEBUG_SLEEP and FEATURE_DEBUG_CLOCK
09/09/10   rc      Removed FEATURE_GSM_MDSP_DEBUG_ENHANCEMENTS
28/07/10   pm      Enabled FEATURE_GSM_CB_DRX_SUPPORTED for all 1H10 targets
27/07/10   dv      CR242019: Correct Fix for this CR
23/07/10   ip      CR245474 Added SLeep debug code
17/06/10   rc      Moved few features from custusurf.h
18/05/10   rc      Disabled FEATURE_GSM_DISABLE_CHIPXN_DIV2 for pos2
05/05/10   pg      Enabled FEATURE_GSM_MDSP_AEQ and FEATURE_GSM_AEQ_VOICE for 9K
17/03/10   ws      Enabled FEATURE_GSM_MDSP_IR and FEATURE_GSM_QDSP6_FW_HAS_IR_MEM
                   for 9K
21/01/10   rc      Enabled Aeq Ph1a (FEATURE_GSM_AEQ_VOICE) for Pos-2 modem.
15/01/10   rc      FEATURE_GSM_MAINTAIN_WCDMA_FRC disabled for Pos-2 modem
04/11/09   ip      Added FEATURE_GSM_GPRS_SUPPORTS_NPA for SCMM
06/11/09   ws      Disable EGPRS MDSP IR for 9K and 8220
27/10/09   rc      Enabled Aeq Ph1a for POS-2 Modem Core
05/08/09   ws      Removed FEATURE_GSM_MDSP_DTM it's now mainlined
28/09/09   ws      Added Featurisation for 7x30 target to mirror SCMM
23/09/09   ws      Added FEATURE_GSM_GPRS_GENESIS for MDM9000
23/09/09   ws      Added featurisation for MDM9000 to enable MUTEX and
                   QDSP6 GERAN features
09/07/09   ws      Featured off AEQ on SCMM for now
01/07/09   nf      Added DEBUG_GL1_SLEEP
12/06/09   rc      Added Aeq Ph1b suppport by enabling FEATURE_GSM_MDSP_AEQ
07/05/09   ps      Featurised FEATURE_GL1_NEW_SLEEP_API using FEATURE_NEW_SLEEP_API
23/01/09   ws      Added featurisation for SCMM bringup
04/11/08   ip      Added LCU clock regime changes
22/10/08   ws      Enabled FEATURE_GL1_NEW_SLEEP_API for LCU target
18/09/08   ws      Disable ASYNC peek/poke Interface for Raptor2
30/04/08   ws      Disable Rnn on this baseline since ESAIC is supported
30/04/08   ws      Don't enable RNN for LCU yet
18/03/08   ws      Enabled SMPS for KOEl and Hawk
20/02/08   ws      Added NEW_ASYNC_MDSP_INTF and re-enabled FEATURE_GSM_DISABLE_CHIPXN_DIV4
06/02/08   WS      added FEATURE_GSM_GPRS_SMPS
05/02/08   ws      Temp disable FEATURE_GSM_DISABLE_CHIPXN_DIV4 due to data abort
22/01/08   ws      Added T_QSC6270 specific featurisation
01/20/08   rc      Featurised FEATURE_GL1_NEW_SLEEP_API so that for Hawk & Koel this is not enabled
01/11/08   rc      Added FEATURE_GL1_NEW_SLEEP_API
01/11/08   rc      Removed FEATURE_GL1_NEW_SLEEP_API
08/10/07   cs      Add support for disablong CHIPXN_DIV4 in TCH for power saving
08/20/07   sjw     Updated for compatibility with MSM6800
03/10/06   agv     Added FEATURE_GSM_TRM, and FEATURE_GSM_TCXOMGR30 for 7600.
09/03/06   gfr     Mainlined mDSP debug features
01/16/06   gfr     Added FEATURE_GSM_MDSP_APPS_ENABLED_CM
12/23/05   gfr     Enable SAIC on 6280
12/13/05   gfr     Added FEATURE_GSM_SLEEP_ERROR_RECOVERY
11/29/05   gfr     Add FEATURE_GSM_MDSP_SAIC on 6275
11/17/05   gfr     Re-enable the rotator on 6280
11/16/05   gw      Enable FEATURE_GSM_SLEEP_CHECK_MDSP on MSM6280 too.
10/27/05   gw      Removed obsolete FEATURE_GSM_SLEEP_EXTRA_GTS_TIME.
                   Don't define FEATURE_GSM_SLEEP_CHECK_MDSP on 6280.
10/18/05   gfr     Add FEATURE_GSM_MDSP_FULL_CIPHERING on 6280
10/10/05   gfr     Disable ROTATOR support on 6280 temporarily
09/22/05   gfr     Do not define FEATURE_GSM_MDSP_EXTRA_DM_NOPS on 6280
09/22/05   gfr     Add FEATURE_GSM_MDSP_NEW_MDM_HW_API for 6280
09/08/05   ws      backed out previous change. re-instated FEATURE_GSM_MDSP_DTM
09/06/05   ws      Removed FEATURE_GSM_MDSP_DTM as new build Id's KMDQ and LLDQ
                   will define it
08/11/05   gfr     Added FEATURE_GSM_MDSP_DTM
05/18/05   gfr     Removed FEATURE_GSM_SAVE_IQ_SAMPLES (saves ~12k of ram)
                   Removed DEBUG_HW_SELECT_STAGE2_OUTPUT_CHANNEL_FILTER
                   Mainlined FEATURE_GSM_MDSP_RAVEN_SBI
                   Removed FEATURE_GSM_MDSP_DM_ACTIVE_INT
05/10/05   gfr     Added FEATURE_GSM_MDSP_DM_ACTIVE_INT
                   Removed FEATURE_GSM_EGPRS_REQUIRE_MCLK and FEATURE_GSM_MDSP_EER
04/22/05   gfr     Temp added FEATURE_USE_LATEST_GL1_HW_API
04/07/05   gfr     Added FEATURE_LOG_GSM_MDSP_APPS_ON_ERROR
03/16/05   gfr     Enable stage3 fw filter, Raven SBI format, mDSP apps logging
03/11/05   gfr     Renable rotator, disable ACI detection again,
                   add FEATURE_GSM_MDSP_EXTRA_DM_NOPS
03/08/05   gfr     Disable rotator
02/17/05   gfr     Added FEATURE_GSM_EGPRS_REQUIRE_MCLK
01/25/05   gfr     Added FEATURE_GSM_MDSP_IR
01/25/05   gfr     Added FEATURE_GSM_MDSP_ACI_DETECTION
01/24/05   gfr     Added FEATURE_GSM_MDSP_GTOW
01/12/04   gfr     Added FEATURE_GSM_MDSP_SINGLE_ISR and renabled sleep check
                   merged some defines from custusurf.h
12/23/04   gfr     Added FEATURE_GSM_SAVE_IQ_SAMPLES
12/02/07   gfr     Added FEATURE_GSM_MDSP_EGPRS
12/02/04   gfr     Added FEATURE_GSM_MDSP_EER
11/19/04   gfr     Moved DEBUG_HW_SELECT_STAGE2_OUTPUT_CHANNEL_FILTER here
                   (temporarily disabled) and re-enable mdsp memory dump
11/03/04   gfr     Temporarily remove memory dump on error feature
10/15/04   ws      Removed FEATURE_GPRS_FT_SLAM
08/30/04   gfr     No sleep check if EGPRS is defined.
07/29/04   bk      Added FEATURE_GPRS_FT_SLAM
19/05/04   gfr     Added FEATURE_GSM_SLEEP_EXTRA_GTS_TIME
13/05/04   npr     Initial revision.
===========================================================================*/


/*---------------------------------------------------------------------------
                           GSM MODEM
---------------------------------------------------------------------------*/

/* Enable GSM support for the TCXO shared resource manager */
/* POS2 modem chipsets use XO, not TCXO */
#define FEATURE_GSM_TCXO_SRM_SUPPORT

/* RFA_TASK and CCS IF support */
/* Enabling RX Diversity by default on Bolt */
#define FEATURE_GSM_RX_DIVERSITY
//#define FEATURE_GSM_RX_DIVERSITY_IDLE

/*---------------------------------------------------------------------------
                           GSM MDSP
---------------------------------------------------------------------------*/

/* Enable EGPRS support in the mDSP driver - required by Raven firmware */
#define FEATURE_GSM_MDSP_EGPRS

/* Firmware support GTOW */
#define FEATURE_GSM_MDSP_GTOW

/* Enable some extra EGPRS features */
#ifdef FEATURE_GSM_MDSP_EGPRS

   /* Enable ACI detection for DL voice and SACCH */
   #define FEATURE_GSM_MDSP_ACI_DETECTION_VOICE

   /* Enable incremental redundancy*/
   #define FEATURE_GSM_MDSP_IR

   #define FEATURE_GSM_QDSP6_FW_HAS_IR_MEM
#endif

/* SAIC */

/* Enable ESAIC on all targets */
#define FEATURE_GSM_MDSP_ESAIC

/* Added features for debugging */
#if defined ( FEATURE_DEBUG_CLOCK )
#define DEBUG_GL1_CLK
#endif

/* Include FEATURE_GL1_SLEEP_DEBUG_STRUCT logging in all builds */
/* Include FEATURE_GL1_SLEEP_DEBUG_STRUCT_F3 only if needed     */
#define FEATURE_GL1_SLEEP_DEBUG_STRUCT
//#define FEATURE_GL1_SLEEP_DEBUG_STRUCT_F3
//#define FEATURE_GSM_DO_NOT_LEAVE_GP_CLK_ON

/* Added VSTMR changes for BOLT modem*/
#define FEATURE_GERAN_VSTMR
#define FEATURE_GSM_MCPM_DL_DATA_RATE
#define FEATURE_GSM_RFLM_FED
#ifndef FEATURE_JOLOKIA_MODEM
#if defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM
#define FEATURE_GERAN_DRDSDS
#define DEBUG_DRDSDS
#endif
#endif
#define FEATURE_SINGLE_VFR_IN_FEERO
#define FEATURE_GSM_DBDC_W2G
//Additional debug for decoding xCCH after missing frames due to sleep wakeup error recovery
//#define DEBUG_GSM_SLEEP_ERROR_RECOVERY

/* Enabled Aeq Ph 1b feature - SAIC on MCS1 to MCS4 */
#define FEATURE_GSM_AEQ_VOICE
/* Enabled Aeq on SCH for 9K */
#define FEATURE_GSM_MDSP_AEQ_SCH
#define FEATURE_GSM_MDSP_AEQ_8PSK
#define FEATURE_GSM_DYNAMIC_AEQ_8PSK
/* Enabling AEQ PH 4 */
#define FEATURE_AEQ_PHASE4

#ifdef   FEATURE_GSM_GPRS_MSC34
#define  FEATURE_GSM_GPRS_MSC33
#endif /* FEATURE_GSM_GPRS_MSC34  */

/* Definitions for Nikel (and inherited by Dime) */

#define FEATURE_GSM_NIKEL_HW


#ifdef FEATURE_LTE
#define FEATURE_GSM_TO_LTE
/*
** FR3009 add G2L measurements in connected mode (CM), now on by default
** FEATURE_GSM_G2L_TIMED_SRCH and FEATURE_CM_G2L removed from codebase
*/
#endif /* FEATURE_LTE */

#define FEATURE_GSM_TO_WCDMA_FATPATH

/* Remove functionality no longer supported by MDSP */
#undef FEATURE_GSM_MDSP_RAND_GEN
#undef MDSP_dmComplete

#define GERAN_L1_ENHANCED_RX

#define GERAN_L1_IP2CAL
#define GERAN_L1_HLLL_LNASTATE

/*---------------------------------------------------------------------------
                           GSM GTMRS
---------------------------------------------------------------------------*/
 /*
  * Shorten warm-up duration by running RF on and first frame in parallel
  */
#define FEATURE_GSM_MULTI_THREAD_WAKEUP
 /*
 * This supports the asynch interrupt service from the mDSP for shortened
 * frames.  */
/* Enable for targets supporting asynchronous idle sleep */
#define FEATURE_ASYNCHRONOUS_SLEEP
 
#if !defined ( FEATURE_DISABLE_EARLY_DECODE ) && \
    !defined ( FEATURE_ASYNCHRONOUS_SLEEP )
  #define FEATURE_GSM_ASYNCH_INT_OPTION3
#endif

#define FEATURE_GSM_ADAPTIVE_TMLN

/* Inject EFS controlled sleep delays for internal testing */
#define FEATURE_GSM_TEST_SLEEP_DELAYS

/* Enable the following feature for debug info on asynch interrupt */
#define FEATURE_GSM_ASYNCH_INT_DEBUG

/* Enable for debug SLPC timing error debug prints */
/* #define FEATURE_GSM_SLPC_TIMING_ERROR_DEBUG */

/* Code added on Nikel 4.x */
#define FEATURE_GSM_POWER_OPTION16
#define FEATURE_GSM_NCELL_LOWER_ACQ_FREQ_OPTION17

#define FEATURE_GSM_POWER_MONITOR_SCHEDULE_OPTION19
#define FEATURE_GSM_CELL_RESELECTION_OPTION20

#define FEATURE_GSM_SCE_REDUCED_NCELL_LIST

/* SCE debug: optimisation: reduce number of ncells from 6 to 2 */
#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST
#define FEATURE_GSM_SCE_REDUCED_NCELL_LIST_DEBUG
#endif

/* SCE optimistaion: disable power monitoring on blacklisted ncells */
#define FEATURE_GSM_SCE_DISABLE_PWR_MSR_ON_BLACKLIST_NCELLS

/* Dime/Triton - add feature for removing adjacnet channel SPUR */
#define FEATURE_GSM_ADJACENT_SPUR_CHANNEL

/*---------------------------------------------------------------------------
                           GSM G2W
---------------------------------------------------------------------------*/

#ifdef FEATURE_DUAL_SIM
#define FEATURE_GSM_TDS_DSDA
#ifdef FEATURE_BOLT_MODEM
#define FEATURE_GSM_WTR_HOP
#endif /*FEATURE_BOLT_MODEM*/
#endif /*FEATURE_DUAL_SIM*/

#ifdef FEATURE_MODEM_COEXISTENCE_SW
  #define FEATURE_GSM_COEX_SW_CXM

/* WLAN-GSM COEX feature */
/* Note : Both Features needs to be enabled at the same time */
//#define FEATURE_WLAN_COEX_SW_CXM
//#define FEATURE_GSM_GPRS_TIMELINE_REPORTING

    /* WLAN-GSM COEX feature */
#define FEATURE_WLAN_COEX_SW_CXM
    
#ifdef FEATURE_WLAN_COEX_SW_CXM 
#define FEATURE_GSM_GPRS_TIMELINE_REPORTING
#endif /* FEATURE_WLAN_COEX_SW_CXM */
    

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  #define FEATURE_GSM_COEX
  #define FEATURE_GSM_COEX_ENHANCED_FCCH
  #define FEATURE_GSM_COEX_REDUCED_MEAS
  #define FEATURE_GSM_COEX_BLANK_CS_UL
  #define FEATURE_GSM_COEX_BLANK_CS_DL
  #define FEATURE_GSM_LOG_COEX
#endif
#ifdef FEATURE_DUAL_SIM
  #define FEATURE_GSM_COEX_RF
#endif
#endif

#ifdef FEATURE_MODEM_COEXISTENCE_FW
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  #define FEATURE_GSM_COEX_FW_CXM
#endif
#endif


/*-------------------------------------------------------------------------*/

#endif /* CUSTGSMDRV_H */
