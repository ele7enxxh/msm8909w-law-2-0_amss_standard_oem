#ifndef CUSTHDR_H
#define CUSTHDR_H
/*===========================================================================

            " M S M 6 8 0 0 -  S U R F "   H E A D E R   F I L E

DESCRIPTION
  Configuration for IS-856 support on MSM6800 targets.

  Copyright (c) 2001 - 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cust/inc/custhdr.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/09/16   vko     Define FEATURE_HDR_TO_LTE_REDIRECTION only if FEATURE_LTE defined
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
02/12/16   vko     Remove FEATURE_QTA_THRU_TRM dependency in enabling QTA
05/04/15   mbs     Supported QTA using TRM API
09/01/15   rmv     Enabled RUMI related features for JO.1.2 only
03/09/15   ljl     To enable tDTX for TH. 
03/05/15   sat     Added Unified Micro Priority feature
02/05/14   vke     Enabled FEATURE_HDR_HANDLE_FC_IN_HDRTX for 
                   Thor 1.0.1 onwards
12/28/14   vko     Enabled FEATURE_HDR_DEBUG_INFO for JOLOKIA
11/26/14   sat     Enabled FEATURE_HDR_RUMI for Tabasco
11/05/14   vlc     Added FEATURE_POWER_INFO_LOG_PACKET for Thor.  
11/06/14   sat     Enabling FEATURE_HDR_TRAFFIC_DTX for Jolokia
10/29/14   tnp     disable QTA and FW CHAN ID for TA
10/28/14   sat     Changes to enable QTA and FW CHAN ID for JOLOKIA
10/14/14   wsh     Changes to support Jolokia RUMI
10/09/14   arm     Removed Band Avoidance feature. 
10/08/14   bb      Chanegs to choose LTA if QTA end time collides with onex
                   reservation time
10/07/14   rmv     Defined FEATURE_HDR_JOLOKIA_MODEM_BRINGUP for temp changes
09/24/14   rmv     Fixed compilation error for Tabasco. 
10/01/14   arm     Changes to support HDR band avoidance.
08/07/14   vko     Enable FEATURE_HDR_REVB_DTX_MODE, FEATURE_HDR_TRAFFIC_DTX for BO 2.1
07/24/14   vko     Enabled FEATURE_HDR_REVB_DTX_MODE for bolt
07/21/14   arm     Fixed compilation error for Thor.
07/16/14   arm     Added NV control to enable/disable QTA.
06/30/14   arm     Changes to support DR-DSDS.
06/24/14   dsp     Corrected FEATURE_HDR_DISABLE_ACCESS_TA_GSM
06/13/14   sat     Enabled FEATURE_DRC_RAMP_DOWN_OPTIMIZATION
06/12/14   cnx     Enable THERMAL feature when FEATURE_HDR_FL_FLOW_CONTROL is defined.
05/20/14   arm     Enabled Coex FW for Bolt 2.0.
05/08/14   dsp     Added FEAURE_HDR_DISABLE_ACESS_TA_GSM 
05/05/14   ljl     Added FEATURE_HDR_THOR_MODEM. 
04/03/14   kss/wsh Added T2P boost for RTT reduction
03/25/14   arm     Enabled DLNA for Bolt.
03/07/14   rmv     Enabled FEATURE_HDR_MODEM_API_RL_INFO 
02/21/14   vko     Enabled FEATURE_HDR_CC_DECODE_FAIL for DI 4.0 and DPM
01/27/14   sat     Mainlined FEATURE_HYBRID_ACCESS_DURING_OVERHEAD.
12/10/13   kss     Enable tuneaway optimizations. 
12/10/13   sat     Added FEATURE_MEMCPY_REMOVAL.
11/27/13   dsp     Added FEATURE_HDR_PAGE_BLOCKING_MGMT.
11/26/13   arm     Enabled wakeup manager feature for HDR.
10/31/13   ukl     Featurized REMOVE_AVOIDED_NSET_CHANNELS
10/25/13   rmg     Enabled T2P boost and enh permitted payload for TR 2.0.
10/09/13   rmg     Featurized removal of redundant RMAC definitions.
10/08/13   mbs     Cleared RUMI flags to enable call on SI
10/17/13   mbs     Removed FEATURE_HDR_DIME_RUMI_IQ to use Y1Y2 samples
09/30/13   vke     DRC ramp down optimization for rude wakeup cases
09/25/13   wsh     Changes to support BOLT bring up
10/02/13   mbs     Enabled FEATURE_HDR_NO_2SEC_INT for HDR TS module
09/04/13   wsh     Changes to support BOLT bring up
09/03/13   wsh     Replaced DIME/TRITON_MODEM with HDR_DIME/TRITON_MODEM 
08/19/13   arm     Merged DSDS feature for triton.
07/24/13   wsh     Enabled ASDiv feature
07/10/13   arm     Enabled tDTX feature.
06/26/13   wsh     Added Selective TxD feature
06/06/13   rmg     Removed FEATURE_HDR_DLNA from BOLT builds.
05/24/13   rmg     Added FEATURE_HDR_DLNA. 
05/22/13   ukl     Featurized Compress and decompress hdr efs file. 
04/22/13   arm     Added changes for DSDA coexistence manager. 
04/29/13   smd     Moved FEATURE_HDR_NEGOTIATE_REVB to hdr scons file.
03/31/13   cnx     Support LTE deprioritization.
02/17/13   arm     Enabled network based dtx feature. 
01/05/13   arm     Added changes for rmac tuneaway optimization. 
12/04/12   vke     Added and Enabled macro for idle diversity
12/03/12   vke     Removed the unused macros
12/20/12   vko     Fixed compiler warnings
12/17/12   smd     Defined FLEX_CONN and DTX.
12/04/12   mbs     Conditionally enabled FEATURE_HDR_LTE_TO_EHRPD_IRAT
11/30/12   wsh     Enable NLB on DIME
11/05/12   rkc     Enabled FEATURE_HDR_SYNC_CHECK and 
                   FEATURE_HDR_SYNC_CHECK_AND_SYSLOST. 
08/29/12   ljl     Removed FEATURE_HDR_DIME_RUMI.
08/15/12   smd     Enabled FEATURE_HDR_IDLE_TUNE_AWAY
08/09/12   ljl     Disbaled the feature FEATURE_HDR_DIME_RUMI_IQ.
08/12/12   ljl     Enabled FEATURE_HDR_DIME_RUMI_IQ for IQ streaming.
08/11/12   wsh     Defined QPCH feature, but disable by NV
08/09/12   ljl     Disbaled the feature FEATURE_HDR_DIME_RUMI_IQ.
07/25/12   arm     Enabled Idle and Acq state diversity for NI.4.1. 
06/13/12   ljl     Disabled Rev C on Dime for now. 
06/07/12   wsh     Fixed incorrect merge not disabled NLB
05/21/12   ljl     Added FEATURE_HDR_DIME_RUMI and FEATURE_HDR_DIME_RUMI_IQ.
04/26/12   vlc     Added FEATURE_HDR_TO_LTE and FEATURE_IRATMAN. 
03/19/12   ljl     Fixed Dime compiler errors.
03/02/12   smd     Added FEATURE_HDR_NON_SIGNALING_MSGR_INTF for FTM msgr interface. 
02/29/12   smd     Added FEATURE_HIT for FTM msgr interface.
02/23/12   arm     Removed FEATURE_HDR_SUPPORTS_FW_TIMING_STATE.
12/05/11   smd     Added FEATURE_HDR_QCHAT if FEATURE_QCHAT is defined.
11/21/11   grl     Enabled flow control and thermal mitigation.
10/11/11   arm     SU API clean up changes for MMODE.
10/10/11   arm     Enabled support for second chain test call.  
09/28/11   lyl     Added FEATURE_HDR_FL_DATA_ADAPTIVE_RXD.
09/28/11   kss     Enabled free running for HDR Rx task.
09/15/11   lyl     Added FEATURE_HDR_GRICE.
09/09/11   wsh     Added FEATURE_HDR_NEW_GSDI_API to support new GSDI API
08/01/11   cnx     Enabled FEATURE_LTE_TO_HDR_OH.
06/30/11   rkc     Added FEATURE_HDRSRCH_1_SEC_RGS for XO targets.
04/18/11   arm     Enabled support for second chain test call. 
04/19/11   lyl     Enabled SVDO diversity for svlte type 2.
04/18/11   arm     Enabled Thermal Mitigation feature for SVLTE type II only. 
03/22/11   arm     Added support for thermal pa backoff feature.  
02/10/11   kss     Enabled FEATURE_HYBRID_ACCESS_DURING_OVERHEAD.  
01/14/11   kss     Enabled free running for HDR Decoder task. 
10/21/10   rmg     Enabled negotiation of RevB.
09/15/10   ljl     Supported dynamic wdog.
07/23/10   ljl     Enabled access hybrid mode.
06/21/10   ljl     Added FEATURE_IRAT_USE_MEAS_RF_API.
06/01/10   ljl     Enabled fast agc mode.
05/19/10   ljl     Added FEATURE_HDR_FW_SRCH_WORKAROUND for T_MDM9X00B and 
                   defined FEATURE_HDR_DIV_SRCH_WORKAROUND for T_MDM9X00A.
05/19/10   ljl     Added FEATURE_HDR_SVDO_DIV_SUPPORT.
04/12/10   pba     Added FEATURE_EHRPD_VOIP_SR.
04/12/10   sju     Removed FEATURE_HDR_CP_9K_BRINGUP.
03/26/10   ljl     Added FEATURE_HDR_DISABLE_HYBRID_ACCESS for MDM9K.
03/19/10   ljl     Added FEATURE_HDR_FORCE_NORMAL_AGC_ACQ. 
03/18/10   ljl     Added FEATURE_HDR_DIV_SRCH_WORKAROUND.
03/11/10   ljl     Added FEATURE_HDR_SUPPORTS_FW_TIMING_STATE.
01/12/10   sju     Added FEATURE_HDR_CP_9K_BRINGUP.
09/17/09   pba     Defined FEATURE_KDDI_RESTRICTED_SESSION_CALL_FLOW for JCDMA.
07/09/09   etv     Defined FEATURE_EIDLE_SCI.
06/22/09   wsh     Defined additional features needed for JCDMA
06/19/09   wsh     Define FEATURE_HDR_SESSION_TIED_TO_PPP for JCDMA
06/07/09   kss     Define FEATURE_HDR_ACCOLC_ALTERNATE_MAPPING for JCDMA
05/12/09   etv     Defined FEATURE_HDR_DEC_TASK
04/16/09   rkc     Mainlined T_MSM7800 and T_MSM8650B.
03/17/09   grl     Added FEATURE_HDR_ETAP_DISABLE_CTA_TIMER and 
                   FEATURE_HDR_PER_BAND_DIVERSITY
02/05/08   etv     Enabled FEATURE_HDR_PHY3_NEGOTIATION.
02/25/08   rkc     Added FEATURE_HDR_CLKREGIM_RM
11/12/08   grl     Enabled FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
08/13/08   sju     Added FEATURE_HDR_RESET_KA_TIMER_ON_ACACK_ONLY
07/04/08   etv     undef HDEM feature when T_MSM8650B is defined.
06/11/08   etv     Enabled FEATURE_HDR_EMPA unconditionally (for Shrawan)
06/09/08   etv     Undef RevB 7500 features when T_MSM8650B is defined.
04/17/08   yll     Defined FEATURE_HDR_SRCH_DBG_MASK_ACCESS_HYBRID_DISABLED.
04/08/08   wsh     Merged in DCM related features
02/05/08   pba     Removed FEATURE_HDR_REVB_CUSTOM_CONFIG.
11/28/07   tpg     Added support for 7800.
10/31/07   sju     Added FEATURE_HDR_REVB_7500
09/11/07   cws     Added FEATURE_COMMON_PCP_RMAC0
08/07/07   cc      Added temporary feature FEATURE_HDR_REVB_RMAC_7500
                   to run rmac4 over rmac3drv on MSM7500.
07/05/07   cc      Added FEATURE_HDR_REVB and FEATURE_HDR_REVB_CUSTOM_CONFIG
06/14/07   grl     Turn on FEATURE_HDR_WUTA
04/17/07   ac      Turn off certain features when VoIP is off
03/27/07   pba     Added FEATURE_HDR_DDARF
01/08/07   yll     Added FEATURE_HDR_EMPA and FEATURE_HDR_CSNA
11/08/06   mpa     Removed QTV BCMCS features (enabled in custmp4.h instead)
10/19/06   ljl     Added FEATURE_HDR_STM
09/14/06   dnn     Added back FEATURE_HDRSRCH_POWER_PROFILE
09/13/06   dnn     Removed FEATURE_HDRSRCH_POWER_PROFILE
                   and FEATURE_RX_HDR_CLK_CONTROL
09/07/06   jyw     Added FEATURE_HDRSRCH_POWER_PROFILE
08/22/06   dnn     Added FEATURE_RX_HDR_CLK_CONTROL 
08/03/06   hal     Mainlined FEATURE_DSM_HDR_MSG_ITEMS
07/31/06   mpa     Defined FEATURE_HDR_HYBRID_ACCESS
07/24/06   hal     Added FEATURE_DSM_HDR_MSG_ITEMS
07/10/06   mt      Added FEATURE_HDR_IDLE_TIMELINE_V3
06/22/06   mpa     Mainlined FEATURE_DIAG_TRACE and FEATURE_HDR_DEBUG_TRACE.
05/17/06   dnn     Added FEATURE_HDR_MIN_UATI_MODE
05/11/06   rmg     Added message debug build mask for subsys-MSG_SSID_HDR_DEBUG
04/21/06   grl     Added FEATURE_HDR_IDLE_TIMELINE_V2
04/18/06   ds      Removed  FEATURE_MDSP_DEBUG_LOGGING
04/11/06   mpa     Added FEATURE_QTV_MCAST_IOCTL
                   Removed FEATURE_HDR_RL_RATE_INERTIA (KDDI only feature)
11/08/05   hrk     Defined FEATURE_HDR_RL_RATE_INERTIA
09/19/05   mpa     Make all bc related features dependent on FEATURE_BCMCS
09/06/05   ds/hrk  Defined HDR rotator based features
08/30/05   mpa     Defined FEATURE_HDR_DOS
08/23/05   grl     Mainlined FEATURE_HDR_TCXOMGR
08/10/05   etv     Defined FEATURE_HDR_REVA_L1
08/05/05   hrk     defined FEATURE_HDRSRCH_SPMF
08/02/05   grl     Defined FEATURE_HDR_TC_OFS_ENHANCEMENT
07/12/05   hrk     defined FEATURE_HDRIS890A, FEATURE_MDSP_DEBUG_LOGGING
05/31/05   grl     Defined FEATURE_HDR_TCXOMGR
05/10/05   hrk     Defined FEATURE_HDRSRCH_TIMED_WAIT
04/26/05   hrk     Defined FEATURE_HDR_EQ, Added FEATURE_HDRSRCH_TIMED_WAIT
04/21/05   hrk     Added FEATURE_HDR_EQ
04/21/05   hrk     Synced with //depot/asic/msm6500/build/cust/5.3/custhdr.h#4
02/07/05   kvd     Removed FEATURE_DS_CAM from this file.
01/04/05   sq      Mainlined the following features:
                   FEATURE_HDR_TC_OFS
                   FEATURE_HDR_TC_DIVERSITY_ONLY
                   FEATURE_HDR_RESELECT
                   FEATURE_HDR_DRC_FILTERING
12/03/04   kvd     Defined FEATURE_DS_CAM
09/08/04   mpa     Defined FEATURE_HDR_BCMCS_HYBRID.
08/31/04   mpa     Defined FEATURE_HDR_BCMCS_MFRQ.
08/10/04   st      Defined FEATURE_BCMCS, FEATURE_HDR_BCMCS, FEATURE_QTV_MCAST.
03/02/04   mpa     Defined FEATURE_HDR_DISABLE_SUSPEND_TIMER
02/04/04   sq      Added FEATURE_HDR_DRC_FILTERING
01/16/04   aaj     Defined FEATURE_HDR_HICPS20
12/15/03   aaj     Added FEATURE_HDR_HICPS20
11/05/03   mpa     Defined FEATURE_HDR_426MS_SLEEP and FEATURE_HDR_NO_HO_DURING_SCC
10/29/03   km      Defined FEATURE_DDTM_CNTL and removed FEATURE_MSM6500_CM_TEMP
10/22/03   mpa     Defined FEATURE_HDR_ALN
10/20/03   km      Defined FEATURE_HDR_RESELECT and FEATURE_HDR_RESELECT_CLOSE_CON
10/13/03   sq      Defined FEATURE_HDR_TC_OFS
10/02/03   sq      Defined FEATURE_DUAL_RX_CHAINS
                   Defined FEATURE_HDR_TC_DIVERSITY_ONLY
09/25/03   sq      Defined FEATURE_ERR_FATAL_ON_MDSP_HALT
09/19/03   ht      Changed FEATURE_BUILD_CDMA_CELL_AMPS to FEATURE_BUILD_CDMA_CELL.
09/18/03   mpa     Reenabled FEATURE_HDR_LONG_SLEEP
09/04/03   sq      Commented out FEATURE_MSM6500_DSP_SLOT_TIMER. This was a
                   work-around feature for MSM 1.0 Rev.
08/12/03   ht      Enabled FEATURE_HDR_HANDOFF
06/17/03   sq      Re-defined FEATURE_HDR_800 and FEATURE_HDR_1900 according
                   to Cougar convention.
01/10/03   ht      Disabled HANDOFF, AUTH, DDCM CNTL.  No Long Sleep.
11/01/02    SH     Added FEATURE_DDTM_CNTL
09/12/02    km     Added a compile flag for FEATURE_HDR_LONG_SLEEP
07/18/02    km     Defined FEATURE_HDR_ACCEPT_PROTOCOL_REV_244
06/11/02    km     Added FEATURE_HDR_LONG_SLEEP
05/02/02   mep     Added FEATURE_HDR_SESSION_QUICK_FAILURE
03/11/02   mep     Disabled FEATURE_HDR_PREF_SESSION_SEED
02/05/02   sh      Added FEATURE_HDR_PREF_SESSION_SEED
01/31/02   mep     Added FEATURE_HDR_INT_DATA_FOR_VOICE
01/07/02   st      Added FEATURE_HDR_HYBRID
                   Remove FEATURE_HDR_PRE_HYBR
12/08/01   sh      Added FEATURE_HDR_PRE_HYBR
12/06/01   sh      Added FEATURE_HDR_HANDOFF 
12/04/01   mep     Added FEATURE_HDR_AN_AUTH to support access network
                   authentication.
11/09/01   rao     Added DSM REUSE Optimization support.
09/20/01   rao     Added Partial RLP Frame logging support 
08/12/01   st      Added FEATURE_IS683C_PRL.
06/26/01   om      Created module.

===========================================================================*/

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
                          HDR debug masks
---------------------------------------------------------------------------*/

/* HDR Message Debug Build Mask */
/* Override default definde in \modem_proc\core\api\services\msgtgt.h */
#undef MSG_BUILD_MASK_MSG_SSID_HDR_DEBUG
#define MSG_BUILD_MASK_MSG_SSID_HDR_DEBUG   0x000007E0 /*Messages 5-10 enabled*/

/*---------------------------------------------------------------------------
                        IS-856 HDR Air Interface
---------------------------------------------------------------------------*/

/* HDR Air Interface support.
*/
//top level feature removed: #define FEATURE_HDR

/* HDR-1X handoff */
//top level feature removed: #define FEATURE_HDR_HANDOFF 

/* Partial RLP Frame logging */
//top level feature removed: #define FEATURE_HDRRLP_LOGGING

/* HDR DSM REUSE optimization */
//#define FEATURE_HDR_DSMREUSE_OPT    //vipin to avoid dsm crash

/* REX enhanced timer feature.
*/
//top level feature removed: #define FEATURE_REX_TIMER_EX

/* IS-863-C preferred roaming list support.
*/
//top level feature removed: #define FEATURE_IS683C_PRL

/* IS-890 test application.
*/
//top level feature removed: #define FEATURE_HDR_IS890
//top level feature removed: #define FEATURE_HDR_IS890A

/* IS-878 Security Layer.
*/
//top level feature removed: #define FEATURE_HDR_AN_AUTH  

/* Hybrid operation
*/
//top level feature removed: #define FEATURE_HDR_HYBRID

/* Generate a preferred Session seed
   so the hash value of preferred control channel
   cycle will not collide with the 1X page */
#undef FEATURE_HDR_PREF_SESSION_SEED

/* Provide a way for a test build without long sleep */
#ifndef HDR_TEST_NO_LONG_SLEEP
#define FEATURE_HDR_LONG_SLEEP 
#endif

/*  continue to support HDR protocol rev 244 base stations */
#define FEATURE_HDR_ACCEPT_PROTOCOL_REV_244

/* Data Dedicated Transmission mode 
   to minimize the hyrbid HDR data call interruption 
*/ 
//top level feature removed: #define FEATURE_DDTM_CNTL

/* Use DSP based slot timer interrupt in place of DMOD interrupt */
#ifdef FEATURE_MSM6500_HDR_DSP_SLOT_TIMER
#define FEATURE_MSM6500_DSP_SLOT_TIMER
#endif

//top level feature removed: #define FEATURE_ERR_FATAL_ON_MDSP_HALT

#define FEATURE_DUAL_RX_CHAINS

/* Alternative to Location Notification */
//top level feature removed: #define FEATURE_HDR_ALN

/* Hybrid in CDMA Power-Save algorithm Rev 2.0 */
//top level feature removed: #define FEATURE_HDR_HICPS20

/* Disable the suspend timer by default */
#define FEATURE_HDR_DISABLE_SUSPEND_TIMER

/* Features for HDR broadcast/multicast service */

/* HDR Broadcast related features */
//top level feature removed: #define FEATURE_BCMCS

#ifdef FEATURE_BCMCS
//top level feature removed: #define FEATURE_HDR_BCMCS
#define FEATURE_HDR_BCMCS_MFRQ
#define FEATURE_HDR_BCMCS_HYBRID
#endif /* FEATURE_HDR_BCMCS */

/* Feature for Equalizer support */
#define FEATURE_HDR_EQ

/* Feature to support REX timed wait */
#define FEATURE_HDRSRCH_TIMED_WAIT

/* Feature for L1 Rev-A Support */
//top level feature removed: #define FEATURE_HDR_REVA_L1

/* Feature for Rev-B Support */
//top level feature removed: #define FEATURE_HDR_REVB
//top level feature removed: #define FEATURE_HDR_REVB_ENH_RATES

/* Feature for HDR Quality Of Service (used for both HDR and data side) */
//top level feature removed: #define FEATURE_HDR_QOS

#ifdef FEATURE_HDR_QOS
  /* Feature for Data over Signalling */
//top level feature removed:   #define FEATURE_HDR_DOS
#endif

/* Feature for Enhanced Multi-flow Packet Application. Turn on only for 7800 */
//top level feature removed: #define FEATURE_HDR_EMPA

#ifdef FEATURE_VOIP
  #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
/* Feature to enable Circuit Services Notification Application */
  #define FEATURE_HDR_CSNA
  #endif
#endif
/* HDR rotator based fetaures */

/* Feature to support HDR FLL rotators */
#define FEATURE_HDRSRCH_ROTATORS 

#ifdef FEATURE_HDRSRCH_ROTATORS
  /* Feature for Equalizer support */
  #define FEATURE_HDR_EQ
#endif /* FEATURE_HDRSRCH_ROTATORS */

#define FEATURE_HDRSRCH_POWER_PROFILE
   /* Enable the hdr power strobe */

#define FEATURE_HDR_FORCE_PREAMBLE_DET
#define FEATURE_HDR_AGC_OPTIMIZATION_P1

/* Minimal UATI Mode feature */
//top level feature removed: #define FEATURE_HDR_MIN_UATI_MODE
#ifdef FEATURE_HDR_MIN_UATI_MODE
  /* When maintain EVDO session is FALSE, whether to explicitly
   * close session (ECS) by sending SessionClose message to AN */
  #undef FEATURE_HDR_MIN_UATI_MODE_ECS
#endif  

/* Feature to enable Hybrid access (suspend) */
#define FEATURE_HDR_HYBRID_ACCESS

/* Feature to enable Delayed Deactivation After Reacquisition Failure */
#define FEATURE_HDR_DDARF

/* Feature to enable control of access hybrid through the tc mode setting
   used for enabling HPT mode. */
#define FEATURE_HDR_ACCESS_HYBRID_CTRL

/* Feature to enable the Warm Up Time Adjustment Algorithm */
#define FEATURE_HDR_WUTA

/* Feature to enable common PCP interface between all RMAC subtypes, should be 
mainlined after test */
#define FEATURE_HDR_COMMON_PCP_RMAC0

/* We want to move JCDMA specific features into this package specific file
   rather than the target specific file. */
#ifdef FEATURE_BUILD_JCDMA

#define FEATURE_HDR_ALLOW_SPIBI_HANDOFFS
  /* Turn on Same PN Inter Band Idle Handoff support. */

#define FEATURE_HDR_ACCOLC_ALTERNATE_MAPPING
  /* JCDMA builds use the alternate mapping for access persistence. */

#define FEATURE_HDR_SESSION_TIED_TO_PPP

#define FEATURE_HDR_NO_ACTIVE_HANDDOWN

#define FEATURE_REMOVE_OFF_BAND_NEIGHBORS

#define FEATURE_HDR_DISABLE_SUSPEND_TIMER 

#define FEATURE_HDR_KA_REQUEST_DISABLED

#define FEATURE_HDR_RL_RATE_INERTIA

#define FEATURE_KDDI_RESTRICTED_SESSION_CALL_FLOW
                     
#endif /* FEATURE_BUILD_JCDMA */

#define FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
  /* Do not send SLP messages until the AT receives a UATI assignment. */

#define FEATURE_HDR_ETAP_DISABLE_CTA_TIMER
  /* Disable the RLP CTA timer when ETAP is enabled */

#ifdef FEATURE_DEDICATED_C2K_MODE
/* CDMA HDR Uniform Mode requires the feature
 * of HDR Session Tied to PPP, MUM, and DDTM
 * to work together. */
#ifndef FEATURE_HDR_SESSION_TIED_TO_PPP
#define FEATURE_HDR_SESSION_TIED_TO_PPP
#endif 

#define FEATURE_DEDICATED_C2K_MODE_DATA

#ifndef FEATURE_HDR_MIN_UATI_MODE
//top level feature removed: #define FEATURE_HDR_MIN_UATI_MODE
#endif

#ifndef FEATURE_DDTM_CNTL
//top level feature removed: #define FEATURE_DDTM_CNTL
#endif

#define FEATURE_HDR_SRCH_DBG_MASK_ACCESS_HYBRID_DISABLED
  /* Disable access hybrid by initialize HDR SRCH debug mask */

#endif /* FEATURE_DEDICATED_C2K_MODE */

#define FEATURE_HDR_RESET_KA_TIMER_ON_ACACK_ONLY
  /* Do not reset the KA timer on every mobile directed packet. Only reset
     the timer after the ACACK is received. */

#define FEATURE_HDR_PHY3_NEGOTIATION
  /* Negotiate MaxTransmitPower when PHY-3 is negotiated */

#define FEATURE_HDR_DEC_TASK
  /* Define HDR Decoder Task */

//top level feature removed: #define FEATURE_EIDLE_SCI
  /* Support for AT-initiated GAUP of Slotted Mode attribute for
   * Enhanced Idle protocol */

#ifdef T_MDM9X00A
  #define FEATURE_HDR_DIV_SRCH_WORKAROUND
    /* MDM9K 1.1 workaround for search with div enabled */

  #define FEATURE_HDR_FORCE_NORMAL_AGC_ACQ
    /* To remove when fast/resume agc work */

#endif /* T_MDM9X00A */

#ifdef T_MDM9X00B
  #define FEATURE_HDR_FW_SRCH_WORKAROUND
    /* Searcher fw workaround for Genesis 1.1 */
#endif /* T_MDM9X00B */

#ifdef FEATURE_IP_CALL
#define FEATURE_EHRPD_VOIP_SR
#endif /* FEATURE_IP_CALL */

#if defined FEATURE_MDM_MSM_FUSION || defined FEATURE_8X60_DUAL_RADIO_FUSION
  #define FEATURE_HDR_SVDO_DIV_SUPPORT
    /* Enable SVDO diversity support for Fusion */
#endif /* FEATURE_MDM_MSM_FUSION || FEATURE_8X60_DUAL_RADIO_FUSION */

#ifdef FEATURE_WDOG_DYNAMIC 
#define FEATURE_HDR_WDOG_DYNAMIC
  /* Enable dynamic wdog support */
#endif /* FEATURE_WDOG_DYNAMIC */

#define FEATURE_HDR_DIME_MODEM
  /* mainlining DIME features */

#if defined FEATURE_JOLOKIA_MODEM || defined FEATURE_TABASCO_MODEM
#define FEATURE_HDR_JOLOKIA_MODEM
#define FEATURE_HDR_JOLOKIA_MODEM_BRINGUP
  /* Temp code needed to support Jolokia Brngup */
#define FEATURE_HDR_BOLT_MODEM
#define FEATURE_HDR_UNIFIED_TX

#endif /* FEATURE_JOLOKIA_MODEM */

#ifdef T_RUMI_EMULATION
/* Features for bring up */
#define FEATURE_HDR_RUMI 
#define FEATURE_HDR_RUMI_TX_ONLY
  /* TX only can now be turned on for RX testing, so
     its good to always define it for RUMI. */
#endif /* T_RUMI_EMULATION */


#ifdef FEATURE_TRITON_MODEM
#define FEATURE_HDR_TRITON_MODEM
#define FEATURE_HDR_UNIFIED_TX
#endif /* FEATURE_TRITON_MODEM */

#ifdef FEATURE_BOLT_MODEM
#define FEATURE_HDR_BOLT_MODEM
#define FEATURE_HDR_NO_2SEC_INT

/* small search window 0x800 for BOLT */
#ifndef FEATURE_HDR_SMALL_SRCH_WINDOW
#define FEATURE_HDR_SMALL_SRCH_WINDOW
#endif /* FEATURE_HDR_SMALL_SRCH_WINDOW */

/* Define DR and QTA only for bolt, not for thor */
#if ( !(defined FEATURE_THOR_MODEM) && !(defined FEATURE_JOLOKIA_MODEM) && !(defined FEATURE_TABASCO_MODEM) )
#define FEATURE_HDR_DR_DSDS
  /* Enable DR-DSDS feature */
#endif /* FEATURE_THOR_MODEM */

#endif /* FEATURE_BOLT_MODEM */

/* Enabling QTA for Jolokia */
#ifdef FEATURE_QTA
#define FEATURE_HDR_QTA
  /* Define QTA for DSDS */
#define FEATURE_HDR_FW_CHAN_ID
/* Send Channel Id to FW, needed for QTA & DR */

#ifdef FEATURE_QTA_THRU_TRM
#define FEATURE_HDR_QTA_THRU_TRM
  /* Define QTA thru TRM API for DSDS */
#endif /* FEATURE_QTA_THRU_TRM*/

#endif /* FEATURE_QTA */

#if defined T_MDM9X00 || defined FEATURE_HDR_DIME_MODEM
#define FEATURE_HDR_MC_MSGR
#endif /* T_MDM9X00 */

/* Enable HDR flow control if the CPU monitors are enabled */
#if defined (FEATURE_CPU_BASED_FLOW_CONTROL) && defined (FEATURE_FC_CPU_MONITOR)
  //#define FEATURE_HDR_RL_FLOW_CONTROL
    /* Enable the HDR RL Flow Control Algorithm. Currently used by Data. */
    /* Data no longer uses FEATURE_HDR_RL_FLOW_CONTROL, it has its own
       feature FEATURE_DATA_RL_FLOW_CONTROL */

//top level feature removed:   #define FEATURE_HDR_FL_FLOW_CONTROL
    /* Enable the HDR FL Flow Control Algorithm */

#ifdef FEATURE_HDR_FL_FLOW_CONTROL
  #define FEATURE_HDR_THERMAL_BACKOFF
    /* Enable Thermal PA backoff feature. This feature is dependent on the 
       above two flow control features  */
#endif
#endif 

#define FEATURE_HDR_FL_DATA_ADAPTIVE_RXD

#define FEATURE_HDR_GRICE

#ifdef T_MDM9615
  /* currently only target on NI 1.1 is MDM9615.
     Need to update this as new PL are added */
#define FEATURE_HDR_NEW_GSDI_API
#endif /* T_MDM9615 */

#define FEATURE_HDR_SECOND_CHAIN_TEST_CALL
  /* Enable second chain test call feature */

#ifdef FEATURE_XO
#define FEATURE_HDRSRCH_1_SEC_RGS
  /* When HDR is awake, send RGS once per second. */
#endif /* FEATURE_XO */

/* Turn on sanity check of SYNC time and system time.
   Declare system lost if SYNC time delta is too large. */
#define FEATURE_HDR_SYNC_CHECK
#ifdef FEATURE_HDR_SYNC_CHECK
#define FEATURE_HDR_SYNC_CHECK_AND_SYSLOST
#endif /* FEATURE_HDR_SYNC_CHECK */

//top level feature removed: #define FEATURE_LTE_TO_HDR_OH

#ifdef FEATURE_LTE_TO_HDR_OH
#define FEATURE_IRAT_DSM_SUPPORT
#endif /* FEATURE_LTE_TO_HDR_OH */

#define FEATURE_HDR_FL_DATA_ADAPTIVE_RXD

//top level feature removed: #define FEATURE_HDR_SU_API_CLEANUP
/* Enable SU API changes */

#ifdef FEATURE_QCHAT
#define FEATURE_HDR_QCHAT
#endif

/* GSDI will be in all future targets starting NI 3.0 */
#define FEATURE_HDR_NEW_GSDI_API

#define FEATURE_HIT

//top level feature removed: #define FEATURE_HDR_NON_SIGNALING_MSGR_INTF

#define FEATURE_HDR_REVC

//top level feature removed: #define FEATURE_HDR_TO_LTE
#define FEATURE_IRATMAN
  /* DO to LTE reselection features */

#define FEATURE_HDR_IDLE_DIV
  /* Enable diversity feature, this macro targeted for Nikel 4.x and 5.x PLs */

#if defined FEATURE_HDR_IDLE_DIV || defined FEATURE_HDR_DIME_MODEM
#define FEATURE_HDR_ACCESS_DIV
#define FEATURE_HDR_DYNAMIC_IDLE_DIV
#define FEATURE_HDR_DYNAMIC_ACQ_DIV
#endif

#define FEATURE_HDR_IDLE_TUNE_AWAY
#define FEATURE_HDR_QUICK_IDLE
#ifdef FEATURE_LTE_TO_HDR
#define FEATURE_HDR_LTE_TO_EHRPD_IRAT
#endif /* FEATURE_LTE_TO_HDR */

#define FEATURE_HDR_DEVICE_ONLY_DTX
#define FEATURE_HDR_FLEX_CONN

#define FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION

#define FEATURE_LTE_DEPRIORITIZATION

#ifdef FEATURE_MODEM_COEXISTENCE_SW
#define FEATURE_HDR_MODEM_COEXISTENCE_SW

/* small search window 0x800 for BOLT */
#ifndef FEATURE_HDR_SMALL_SRCH_WINDOW
#define FEATURE_HDR_SMALL_SRCH_WINDOW
#endif /* FEATURE_HDR_SMALL_SRCH_WINDOW */

#ifdef FEATURE_MODEM_COEXISTENCE_FW
#define FEATURE_HDR_MODEM_COEXISTENCE_FW
#endif /* FEATURE_MODEM_COEXISTENCE_FW */

#endif /* FEATURE_MODEM_COEXISTENCE_SW */

#define FEATURE_HDR_EFS_COMPRESS

#define FEATURE_HDR_TRAFFIC_DTX

#if (!defined(FEATURE_HDR_TRITON_MODEM))
#ifndef FEATURE_HDR_RUMI
#define FEATURE_HDR_DLNA
  /* DLNA cannot be enabled in RUMI due to FW bug */
#endif /* FEATURE_HDR_RUMI */
#endif /* !FEATURE_HDR_TRITON_MODEM  */

/* Feature - API to Estimate Cost of Modem Power */
#define FEATURE_HDR_MODEM_API
#define FEATURE_HDR_MODEM_API_UT

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
#define FEATURE_HDR_SELECTIVE_TXD
#endif /* FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY */

#ifdef FEATURE_DUAL_SIM
#define FEATURE_HDR_DUAL_SIM
  /* Enabled DSDS for HDR */
#define FEATURE_HDR_WAKEUP_MANAGER
   /* Enable wakeup manager for HDR */

#endif /* FEATURE_DUAL_SIM */

#define FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS

#define FEATURE_MEMCPY_REMOVAL

#define FEATURE_HDR_MODEM_API_RL_INFO

#define FEATURE_HDR_USE_RMAC_CMN_DEFS
#define FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  /* RTT reduction feature DEPENDS on T2P boost feature */
#define FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST 
#define FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
  /* Enable tuneaway optimizations */

#ifdef FEATURE_PBR_MARKOV
#define FEATURE_HDR_PAGE_BLOCKING_MGMT
#endif /* FEATURE_PBR_MARKOV */

#if (!defined(FEATURE_MPSS_DI_3_0)) && (!defined(FEATURE_DIME_2_1))
#define FEATURE_HDR_CC_DECODE_FAIL

#ifdef FEATURE_HDR_CC_DECODE_FAIL
#define FEATURE_HDR_DDARF /* DDARF must also be defined, if CC_DECODE_FAIL is defined */
#endif /* FEATURE_HDR_CC_DECODE_FAIL */
#endif /* (!defined(FEATURE_MPSS_DI_3_0)) && (!defined(FEATURE_DIME_2_1)) */

#define FEATURE_HDR_DRC_RAMP_DOWN_OPTIMIZATION
  /* Enable DRC RAMP DOWN optimizations */

#define FEATURE_HDR_DISABLE_ACCESS_TA_GSM

#ifdef FEATURE_THOR_MODEM
#define FEATURE_HDR_THOR_MODEM
#define FEATURE_POWER_INFO_LOG_PACKET

#ifdef FEATURE_HDR_FL_FLOW_CONTROL
#define FEATURE_HDR_HANDLE_FC_IN_HDRTX
#endif /* FEATURE_HDR_FL_FLOW_CONTROL */

#endif /* FEATURE_THOR_MODEM */

#define FEATURE_HDR_DEBUG_INFO /* HDR debug info */

#ifdef FEATURE_MCS_WWCOEX_UNIFIED_PRIORITY
#define FEATURE_HDR_UNIFIED_MICRO_PRIORITY
#endif /* FEATURE_MCS_WWCOEX_UNIFIED_PRIORITY */

#ifdef FEATURE_LTE
#define FEATURE_HDR_TO_LTE_REDIRECTION
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
#define FEATURE_HDR_TO_LTE
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
#endif /* FEATURE_LTE */

#endif /* CUSTHDR_H */

