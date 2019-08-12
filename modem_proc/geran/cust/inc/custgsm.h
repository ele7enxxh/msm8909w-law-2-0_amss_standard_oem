#ifndef CUSTGSM_H
#define CUSTGSM_H
/*===========================================================================

                           C U S T G S M

DESCRIPTION
  Customer file for GSM.

  Copyright (c) 2002-2015 by QUALCOMM Technologies, Inc. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/cust/inc/custgsm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/03/16   bg      CR983597 FR30575: SR-DSDS G+L with G to L tuneway - GL1 changes 
12/11/15   nm      CR933690 Enable FEATURE_GSM_GPRS_READ_CCCH_IN_XFER_DEBUG
09/10/15   df      CR921075 FR23269 M2M devices: Handle RR->GL1 MPH_UPDATE_IDLE_DRX_IND
30/03/15   nm      CR765891 Allowing TA for TDS during PS RACH
25/03/15   nm      CR814203 Disable Debug feature flag FEATURE_GSM_GPRS_READ_CCCH_IN_XFER_DEBUG
04/02/14   nk      CR767734 Allowing TA during 2-phase access
23/12/14   am      CR772910 Enable FEATURE_GSM_GPRS_TIMELINE_REPORTING
09/12/14   sb      CR752564 Re-enabling Feature AMR Partial Decode and DTX detection FR19667
15/05/13   nn      CR 487943 - Enable FEATURE_GSM_SEARCH_ALL_WCEL
11/11/14   sb      CR752564 Disable Feature AMR Partial Decode and DTX detection FR19667 temporarily
10/11/14   sb      CR753576 Enabling FR17571 GSM Coex with WLAN/BT
07/11/14   pjr     CR752564 Enable Feature AMR Partial Decode and DTX detection FR19667
17/10/14   nm      CR741161 Enable feature PCH in transfer on Jolokia and fix a merge error
14/10/14   sbr     CR738835 disable FEATURE_GSM_DED_SCELL_MONITOR
03/10/14   aga     CR732455 Changes required to tune away framework for TH 1.0 single sim build 
15/09/14   sv      CR724759 - enable FEATURE_GSM_DED_SCELL_MONITOR
10/07/14   js      CR593074- ROTA/QBTA Feature implementation
02/07/14   aga     CR675214: G2T TA feature and GTA/G2W TA re-worked optimized solution
25/06/14   cs      Remove FEATURE_ASYMMETRIC_GERAN_BANDS
10/06/14   jj      CR 668965 merge ASDIV from DI.3.0.c8 to BO 2.0
21/05/14   ssh     CR668643: gl1_hw_qta_gap_active() should return FALSE for multimode sub
11/11/13   ws      CR573757 Cust changes to support MAV10 DSDS
23/04/13    rc     CR650883 - Enable FEATURE_GERAN_TRM_PAGE_SKIPPING for DPM
05/11/13   cja     CR523987 Add option of FEATURE_GSM_DED_SCELL_MONITOR for extra SCell monitor in voice call
23/05/13   pg      CR460555: Enable 3GPP Rel10 RACH Power Reduction feature
14/09/13    ap/aga CR545423 - G2W Tune Away Feature
21/08/13   npt     CR531633 - Remove FEATURE_DIME_MODEM dependencies
15/08/13    sk     CR524039 GBTA bringup changes
29/07/13    sk     CR519663 Partial QBTA bringup changes
08/07/13    ab     CR 509759 Enable FEATURE_GSM_EGPRS_IR_WS_REDUCTION
16/05/13    ab     CR487396 - Support EGPRS IR Reduced Window in GFW
15/07/13    mko    CR511298: Undefining the Macro "FEATURE_GSM_HW_TASK_REQUEST"
29/04/13    ws     CR481359 enable GTA support
10/09/12   ss      CR363991 : Enable FEATURE_G2T_SRCH_LOGGING for G2T meas and acq response
20/07/12   rc      376792 - Enable FEATURE_SPEC_CR_2567_GTOW_SECURITY_KEY_HANDLING
11/06/12   tjw     CR367302 - remove unused FEATURES (NAS_NVMEM_HW_CLASSMARK, SCE,
                   COMMON_PWR_HANDLER, STOPPING_CELL_ID_SRCH, STOPPING_DED_CELL_ID_SRCH,
                   RR_LOCAL_SIMINFO, GSM_GPRS_ENABLE_RESELECTION_IN_TRANSFER,
                   GSM_GPRS_ENABLE_NC1_NC2_MODE_RESELECTION, GSM_GPRS_GRR_NO_NCELL_PSI1_READ,
                   GSM_GPRS_GRR_PSI_REFRESH, INTERRAT_CELL_RESELECTION_GTOW_GPRS,
                   INTERRAT_CELL_RESELECTION_GTOW_XFER, INTERRAT_NC_RESELECTION_GTOW_GPRS,
                   GPRS_GRR_GMM_PCCCH_PAGING_COORD, GHDI_MVS_MODE_TO_GL1
12/04/12    rc     Enable FEATURE_GSM_TDS_PSEUDO_FAST_RETURN & FEATURE_GSM_TDS_G2T_REDIRECTION
10/04/12    pm	   Enabled FEATURE_GSM_TDS_JDS
21/03/12    pg     Add FEATURE_GERAN_CRIT_SECTION_SUPPORT for DIME modem
16/03/12    rc	   Enabled FEATURE_GSM_QPA for QFE 1320
15/03/12    pg     Target feature MSM8974 now replaced with FEATURE_DIME_MODEM
12/03/12    pg     Enable initial MSM8974 feature set, derived from MSM8960
10/02/12    da     Enabled FEATURE_GERAN_SNDCP_REORDER for Nikel.
29/09/11    tjw    Duplicate definition of FEATURE_GSM_CB_DRX_SUPPORTED removed from
                   custgsmdrv.h instead
01/07/11    rt     Removing duplicate definition of FEATURE_GSM_CB_DRX_SUPPORTED
24/06/11    rt     Defining FEATURE_GERAN_CRIT_SECTION_SUPPORT for 8660 only
24/06/11    rt     Enabled FEATURE_GERAN_CRIT_SECTION_SUPPORT and FEATURE_GSM_CB_DRX_SUPPORTED
24/06/11    nf     CR289274 ported
01/03/11    rc     Enable FEATURE_GHDI_MVS_MODE_TO_GL1
23/02/11    pg     Enable IR Opt feature for Pegasus
26/01/11    pg     Disable IR Opt feature temporarily for Pegasus
20/01/11    rc     Enabled IR Opt feature. CB DRX Enabled only when DSDS is not enabled
07/01/11    pm     Enabled FEATURE_GSM_CB_DRX_SUPPORTED.
06/12/10   sjw     Disable mainline features during DSDS bringup on Poseidon
30/11/10    ab     Enabled IR MEMORY REDUCTION FEATURE.
                   FEATURE_GSM_MDSP_IR_3_BIT_PACKING
               FEATURE_GSM_EGPRS_IR_OPT_PER_TBF
                   FEATURE_GSM_EGPRS_IR_WS_REDUCTION
17/10/10    ps     Enabled FEATURE_GSM_WDOG_DYNAMIC for targets supporting
                   DYNAMIC WDOG; currently 9K targets
16/08/10    rc     Enabled FEATURE_GSM_TO_LTE for Genesis 3.x
23/07/10    rc     Disabled FEATURE_GSM_TO_LTE for Genesis
19/07/10    rc     Enabled FEATURE_GSM_TO_LTE for Genesis
18/06/10    rc     Moved FEATURE_NONBLIND_HANDOVER_DEBUG from cutsusurf.h
03/06/10    ws     Enabled FEATURE_GSM_DISABLE_SLEEP for 8660
03/06/10    ws     Enable FEATURE_GSM_8K_QDSP6_AUDIO for T_MSM8660 Blackbird
20/05/10    dv     Move FEATURE_GSM_RFA_IRAT so only applicable to 9k
20/05/10    ps     Enabled FEATURE_GSM_PCH_SINGLE_BURST_DEC
19/05/10    rc     Enabled FEATURE_GSM_FEMTO_SEARCH feature
07/05/10    ps     Removed FEATURE_GSM_TO_LTE; Not required for MDM9x00 1.x PL
20/04/10    rc     Enabling 2H09 feature set (L2 Random Padding) for Genesis Modem
30/03/10    ps     FEATURE_BPLMN_SEARCH_PRIORITY required for GOBI3000 product
16/02/10    ws     Enabled FEATURE_GSM_GPRS_AGC_UPDATE for 9K targets
02/02/10    rn     Added FEATURE_GSM_RR_CELL_CALLBACK for all targets, move away from custusurf.h target specific featurisation.
15/01/10    rc     Enabled FEATURE_GSM_GPRS_AGC_UPDATE for Pos-2 targets
30/11/09    rc     Disabling LIST_SEARCH_IN_PAGING_BLOCK for Pos-2 targets for G2W
27/10/09    rc     Featurized FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE under FEATURE_WCDMA
27/10/09    rc     Enabled Single Burst PCH decode & AMR-WB for Pos-2
07/10/09    rc     Enabled L2 Random padding, CSI, EDGE IR Opt for Pos-2
25/09/09    rc     Removed FEATURE_GERAN_STORED_LIST_CELL_SELECTION as the RR_ACQ_DB feature covers the functionality
28/09/09    ws     Added Featurisation for 7x30 target to mirror SCMM
23/09/09    ws     Added featurisation for MDM9000 to enable MUTEX and
                   QDSP6 GERAN features
09/07/09    ws     Disabled 1H09 feature for POSEIDON for now
09/06/09    nf     Added FEATURE_GSM_PCH_SINGLE_BURST_DEC
01/04/09    ws     Removed SCMM exclusion of R-SACCH
03/03/09    ws     Corrected featurisation for SCMM
26/02/09    ps     Added FEATURE_GSM_L2_RANDOM_PADDING. Shall be enabled later.
29/01/09    rc     Enabled FEATURE_GSM_GPRS_ENH_SNR_RESEL
23/01/09    ws     Commented features for SCMM bringup
19/12/08    pm     Commented Cell selection after Channel Release, G2W redirection after connection release
19/12/08    pm     Enabled DYNAMIC SAIC, Cell selection after Channel Release, G2W redirection after connection release and Removed EMR, PEMR
06/10/08    rc     Added FEATURE_BPLMN_SEARCH_PRIORITY for GOBI-2000 - Fix for CR:152804
02/10/08  ps     Enable AMR-WB for LCU
18/09/08    rc     Removed FEATURE_USB_WITH_ARM_HALT, Enabled FEATURE_GAN dependent features & FEATURE_LAPDM_REESTABLISHMENT
04/09/08    rc     Enabled FEATURE_GSM_RGS_MULTIPASS
01/09/08    rc     Enable R-SACCH for LCU
04/08/08    rc     Disable AMR-WB and R-SACCH feature in LCU for now.
08/07/08    ws     Changes for raptor2 Target, Disable AMR-WB and define FEATURE_GSM_8K_QDSP6_AUDIO
13/06/08    rc     Added 2H07 GERAN Feature AMR-WB
07/05/08    rc     Added 2H07 GERAN Features R-SACCH & PS HO
07/05/08    rc     Added FEATURE_GPRS_FAST_RACH
30/04/08    ws     Moved ESAIC feature to cutgsmdrv.h
30/04/08    ws     Enable ESAIC for LCU
06/02/08    ws     Added FEATURE_GSM_MDSP_ESAIC
05/02/08    ws     Added FEATURE_GSM_L1_IMPLEMENT_LPM
22/01/08    ws     Added T_QSC6270 specific featurisation
22/10/07    ws     Addede Enable ACQ database feature
14/09/07    rc     Added FEATURE_GPRS_COMP_ENUM_TYPES to re-use SNDCP VU that support IPHC with other layers that does not support IPHC
14/08/07    rc     Replaced FEATURE_GSM_SNDCP_IPHC_ROHC with FEATURE_GSM_SNDCP_IPHC
                   Added FEATURE_GSM_L1_OOS_SEARCH_TIME_ENH
09/07/07    rc     Removed FEATURE_CSN_REFACTORED
31/05/07    ws     Enabled FEATURE_GSM_L1_CONTROL_SET_BAND
16/04/07    npr    Enabled 2H06 features: EDTM, RFACCH, IPA, CSN
19/01/07    rc     Enabled FEATURE_GSM_GPRS_LLC_GEA_NV_SUPPORT
21/11/06    rc     Enabled FEATURE_GSM_L1_HFREQ_ERR_NCELL_SCH_DECODE
10/02/06    ws     Enabled DTM,EDA and PFC
09/06/05    ws     Removed FEATURE_GSM_DTM  since new build Id's use them
08/26/05    ws     Merged from Raven Branch features DISABLE_XFER_SNR_RESELECTION
                   and FEATURE_GPRS_GRR_GMM_PCCCH_PAGING_COORD
08/25/05    ws     Enabled FEATURE_GSM_DTM
06/23/05   src     Enabled FEATURE_GSM_GPRS_PEMR
05/24/05    gw     Enable WCDMA ID searches in transfer mode via
                   FEATURE_GTOW_RESELECTION_XFER_CELL_ID
04/24/05    bk     Temp disabled SNR based reselection in transfer via DISABLE_XFER_SNR_RESELECTION
11/19/04   gfr     Moved DEBUG_HW_SELECT_STAGE2_OUTPUT_CHANNEL_FILTER to
                   custgsmdrv.h
13/05/04   npr     Initial revision.
===========================================================================*/

/***************************************************************************
                                 GSM
****************************************************************************/
/* Allows GL1 respond to MPH_UPDATE_IDLE_DRX_IND from RR which enables 
   DRX scaling (skip PCHs while maintining sync to save power) and to skip NCELL 
   activities (to save power in non-mobility scenarios */
/* #define FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_DUAL_SIM

/* Enable Voice Services DRIVER for GSM only Sub if not using single GSTMR HW */
#if !defined (FEATURE_GSM_SINGLE_HW)
#define FEATURE_GSM_VS_DRIVER
#endif

#if defined (FEATURE_WCDMA)
#define FEATURE_QBTA
#endif

#define FEATURE_CXM_QTA
/* Feature used to log the QTA events*/
#if defined (FEATURE_QTA) || defined (FEATURE_CXM_QTA)
#define QTA_EVENT_DATA_LOGGING
#endif

/*Feature to use actual duration passed by other tech in trm unlock to open G2X TA gap*/
#define FEATURE_USE_ACTUAL_TRM_DUR_UNLOCK

#endif /* FEATURE_DUAL_SIM */

/* MUSTANG FEATURE needed to port MVS code from MUSTANG */
#define FEATURE_GSM_GPRS_MUSTANG

#define FEATURE_GSM_SEARCH_ALL_WCELL

/*---------------------------------------------------------------------------
                               GSM L1
---------------------------------------------------------------------------*/

/* Increase the Cell ID search periodicity to 25sec, and send the
   measurement report to RR right after Cell ID search.
   Increase the Cell Reconf periodicity to 5sec
*/
#define FEATURE_INCREASED_PERIOD_FOR_CELL_ID_CELL_RECONF_SRCHS

/* Enable DYNAMIC SAIC */
#define FEATURE_GSM_DYNAMIC_SAIC

#define FEATURE_GERAN_TRM_PAGE_SKIPPING

/*---------------------------------------------------------------------------
                               GSM L2
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                               GSM RR
---------------------------------------------------------------------------*/

#define FEATURE_SPEC_CR_2567_GTOW_SECURITY_KEY_HANDLING

/***************************************************************************
                                 GPRS
****************************************************************************/

/*---------------------------------------------------------------------------
                           GPRS SNDCP
---------------------------------------------------------------------------*/

/* Enable GPRS SNDCP Compression Features*/
#define FEATURE_GSM_GPRS_SNDCP_PCOMP
#define FEATURE_GSM_GPRS_SNDCP_DCOMP
#define FEATURE_GSM_SNDCP_IPHC

/*---------------------------------------------------------------------------
                           GPRS LLC
---------------------------------------------------------------------------*/

/* Aglilent 8960 does not size it's SN-PDUs correctly according to N201-U/I
   defaults. This reduces the stictness of length checking as performed by
   LLC on received frames. */
#undef DEBUG_GSM_GPRS_LLC_AGILENT8960_WORKAROUND

/*---------------------------------------------------------------------------
                           GPRS RLC
---------------------------------------------------------------------------*/

#undef DEBUG_GSM_GPRS_GRLC_DISABLE_TWO_PHASE_REQ

/*---------------------------------------------------------------------------
                           GPRS MAC
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           GPRS L1
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           GPRS RR
---------------------------------------------------------------------------*/

/* Agilent 8960 includes the SGSNR bit in PSI13, but not SI13, leading to
   multiple notifications to MM of changing SGSNR. This workaround sets the
   SGSNR to that received in the previous SI13 whenever PSI13 is received */
#define DEBUG_GSM_GPRS_GRR_SGSNR_AGILENT8960_WORKAROUND

/* Local inter-rat feature defines for ongoing development. */
#define FEATURE_INTERRAT_NC_RESELECTION_GTOW_GPRS


#if  defined (FEATURE_GOBI)
   #define FEATURE_BPLMN_SEARCH_PRIORITY
#endif

#define FEATURE_GSM_PCH_SINGLE_BURST_DEC

/* Following moved from custusurf.h */
#define FEATURE_NONBLIND_HANDOVER_DEBUG
#define FEATURE_G2W_SRCH_LOGGING_ENABLED

/* Enable G to L only for targets that support LTE */
#if defined(FEATURE_LTE)
#define FEATURE_GSM_TO_LTE
#endif

/* Enable TD-SCDMA IRAT for GERAN */
#if defined(FEATURE_TDSCDMA)
#define FEATURE_GSM_TDS
#define FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
#define FEATURE_GSM_TDS_G2T_REDIRECTION
#define FEATURE_G2T_SRCH_LOGGING
#if defined(FEATURE_DEDICATED_PRIORITY) && defined(FEATURE_LTE)
#define FEATURE_TDSCDMA_DEDICATED_PRI_INFO
#endif /* FEATURE_DEDICATED_PRIORITY && FEATURE_LTE */
#endif

#if defined(FEATURE_LTE) && defined(FEATURE_GSM_TDS)
#define FEATURE_FAST_G2L_PSEUDO_SI2Q
#endif /* FEATURE_LTE && FEATURE_GSM_TDS */

/* Voice related features */
#define FEATURE_QDSP6_ADSP_MDSP_RADIO_FUSION

#define FEATURE_GSM_GPRS_TIME_PWR_AVG

#define FEATURE_GSM_EGPRS_IR_WS_REDUCTION



#if defined (FEATURE_SGLTE)
#define FEATURE_SGLTE_G2X_CONN_MODE_RESEL
#endif

#ifdef FEATURE_DUAL_SIM
#define FEATURE_GPRS_GBTA
#define FEATURE_G2X_TUNEAWAY
#define FEATURE_G2L_TUNEAWAY
#define FEATURE_GTA_DURING_PS_RACH
#define FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
#define FEATURE_GSM_GPRS_READ_CCCH_IN_XFER_DEBUG
#define FEATURE_G2T_2PHASE_ACCESS
#endif /*FEATURE_DUAL_SIM*/

#define FEATURE_VAMOS_II

// FR 17504 Better handling of BSR scans when there is GERAN Data
#define FEATURE_GERAN_BSR_PS_STATUS

// FR 16887 Display of Partial PLMN Search Results during Manual Search (GRR)
#define FEATURE_MPLMN_PARTIAL_UPDATING

/*FR 19557*/
#define FEATURE_GSM_BAND_AVOIDANCE

/*FR 19667*/
#define FEATURE_GSM_AMR_PD_DTX_DETECT

/*-------------------------------------------------------------------------*/
#if defined(FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY)
#define FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
#define FEATURE_GSM_QSC_TX_DIV
#endif

/* WLAN-GSM COEX feature */
#define FEATURE_WLAN_COEX_SW_CXM

#ifdef FEATURE_WLAN_COEX_SW_CXM 
#define FEATURE_GSM_GPRS_TIMELINE_REPORTING
#endif /* FEATURE_WLAN_COEX_SW_CXM */

/* Feature flag to store GRLC F3s in internal buffer */
#define FEATURE_GRLC_STORE_F3_MSG

#define FEATURE_FAKE_BTS_CELL_INFO
//#define FEATURE_FAKE_BTS_CELL_BARRING

#include "geran_msgs.h"
#endif /* CUSTGSM_H */
