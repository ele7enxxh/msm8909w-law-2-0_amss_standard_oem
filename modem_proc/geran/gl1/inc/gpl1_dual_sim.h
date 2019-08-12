#ifndef GPL1_DUAL_SIM_H
#define GPL1_DUAL_SIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     L1 DUAL SIM GENERAL PROCESSING HEADER FILE

GENERAL DESCRIPTION
This module handles the NULL mode from TASK context, when in DUAL SIM mode of
operation.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gpl1_dual_sim.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
26/07/16   bg        CR1040468 Fix for other sub X tech page miss during G2X TA
03/05/16   ap        CR849948 Race condition between handling MPH_SET_PRIORITY_IND and TRM grant callback 
21/09/15   nm        CR767743 Using actual duration in trm unlock cb for unlock by event
03/04/15   ab        CR809082 Do not schedule PWR_MON_EVENT for missed page ,
                     when Transfer Sub is Releasing TBF.
30/03/15   nm        CR765891 Allowing TA for TDS during PS RACH
25/02/15   am        CR799819 Ensure GBTA ACTIVE status is re-set if GBTA is no longer required
17/02/15   br        CR796211 QTA:Don't  start QTA from GL1, if gl1 deactivation is going on.
04/02/14   nk        CR767734 Allowing TA during 2-phase access
27/01/15   am        CR787063 Increase RESEL_EXITS_COUNTDOWN_THRESHOLD
27/01/15   nm        CR786009 Fix the merge error from CR668029 
16/01/15   js        CR782437 Providing API to WL1 for ROTA enhancement  
16/01/15   aga       CR780157: Remove transition from ACTIVE_GAP to ACTIVE for data call abort use cases
06/01/14   am        CR390109 Improved DSDS cell reselection
31/12/14   sp        CR776381 Ensure DEMOD_PAGE_INV_HI is also a GBTA reason
29/12/14   sp/aga    CR774163: Update GFW to active_gap when opening GTA gap
10/12/14   ws        CR768376 Q6 compile warning fixes
25/11/14   sjv       CR759816 Remove reason checks for G2T and G2W TA for TBF unlock_by  
23/10/14   sjv/aga   CR744425 using correct gas_id and TRM reasons in gl1_is_granted_geran_sub_doing_reselection
24/07/14   nm        CR698030 Use correct trm client id to query other sub trm reason when pch in transfer is enabled
16/10/14   aga       CR735726  Remove obsolete gta_set_wcdma_blacklisted functionality 
03/10/14   aga       CR732455 Changes required to tune away framework for TH 1.0 single sim build 
30/09/14   mko       CR728501 If TS adjustment is missed then send DS_ABORT & move to idle when TRM is granted back
22/09/14   ap        CR720862: FR 20912: SGLTE+G DSDA with WTR asymmetric GSM support
12/09/14   nm        CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
17/09/14   mko       CR725121:Add TRM_CHANNEL_MAINTENANCE &TRM_RESELECT reasons as G2T compatible reasons
20/05/14   ap        CR668029 Handling Serving Cell BCCH reads during Early Camp when X Rat is in Data Traffic
22/08/14   mk        CR712000 Shorten abort period for serving BCCH normal TBF suspension to avoid missing TS change
                     and handle abort when suspension timer is running
28/08/14   sjv       CR717166 pending activities are organized in an array for trm band release to avoid overwrite in case of diff. activities
31/07/14   xz        CR697883 Not allow GBTA if current idle frame is doing TDS IRAT in xfer mode
22/08/14   aga       CR713731 Updates to F3s and other recent changes 
11/08/14   sjw       CR706459: use L1_COMPLETE_DS_ABORT for race condition between abort and release
07/08/14   aga       CR689162: Abort tune away gap mechanism triggered if lower TS change is imminent
06/08/14   ka        CR699964 Implement Idle RXD support in GL1
04/08/14   nm        CR663403 Use TRM_BROADCAST_ACCESS trm reason in page reorg mode instead of DEMOD_PAGE_MAX_SENS
29/07/14   ap        CR695234: Changes for GL1's New Rach interface
25/07/14   aga       CR698326 - avoid scheduling monitors when TBF suspension is imminent  
16/06/14   nk        CR678832 Override TRM reason to QTA compatible reason one and avoid updating TRm reservation for TA
10/07/14   js        CR593074- ROTA/QBTA Feature implementation
16/07/14   aga       CR695332 PCH scheduling  is not initiated, GBTA is not stated 
11/07/14   sjw       Standardise use of gl1_is_geran_tuneaway_allowed()
14/07/14   cs        CR693967 Get DR-DSDS mode during startup
04/07/14   cs        CR688003 GRM functions moved to gpl1_grm_intf.c
02/07/14   aga       CR675214: G2T TA feature and GTA/G2W TA re-worked optimized solution
30/05/14   ws        CR657674 Data + MMS Phase 1
27/06/14   cs        CR685966 Add DR INFO async callback support
24/06/14   cs        CR684714 Add back in functionality removed for initial updated TRM API support
22/05/14   cah       Bolt 2.0: FEATURE_GERAN_DRDSDS
17/06/14   cs/ch     CR651795 TRM Updated changes for DR-DSDS
29/05/14   cs/pa     CR660831 Use new trm_status definitions specific to GL1
25/04/14   dg        CR654695 Back-out CR649702 to avoid regression
18/03/14   ap        CR629844 SGLTE BPLMN support when G1 in PS call in G+G - PS Hopping
18/03/14   ap        CR614949 FR 19574: Performing LTE/TDS measurements while GERAN is in connected mode using the second WTR
03/03/14   am        CR630494 Don't release FW in the same frame as FW setup
12/03/14   ssh       CR627985 Skip first burst decode when waking up from sleep inside a QTA gap
06/03/14   br        CR626780, RACE condition between TRANSFER G sub and IDLE G sub, issuing SCH in G2x gap.
28/02/14   ap        CR624060 Further changes to FEATURE_PBR_MARKOV applied
28/02/14   sjw       CR620569: adaptive unlock callback for acquisition
21/02/14   ap        CR620029 Ensure FEATURE_PBR_MARKOV is correctly applied
19/02/14   ap        CR600310 GSM L1 to send channel Id to GSM FW corresponding to the primary RF device
2/17/14    ab        CR612460 Fix race condition for power monitor update
09/01/14   zf        CR598637: DRX only updates TRM booking if DRX activity is matching with the TRM booking
24/01/14   mko       CR593859:Extend the TRM if SI read is in less than 11 frames after the paging in reselection state(HDR in BG Traffic)
13/02/14   ap        CR612587: Ensure call RF exit GSM mode when releasing TRM in DSDA mode
17/01/13   sk        CR600574 Ensure GBTA state is reset from Active if TRM released/reserved without the activity being run
24/12/13   ssh/sp    CR600563: Check for ISR overrun after Fw Init before issuing commands to GFW
03/02/14   gk        CR591181 Change WARM_UP_OVERHEAD to 150 instead of 100
19/12/13   js        CR581840 IDLE SUBs releasing TRM immediately after GRANT leading to unnecessary GTA Gaps
11/12/13   ap        CR586661 GPRS suspension during SI read after W2G reselection
17/12/13   sk        CR586741 Update TRM reservation to QTA compatible one if WCDMA is in QTA mode
16/12/13   dv        CR569122 High missed page rate due to W ID search
12/01/14   sjw       CR592154: use new TRM reason for ACQUISITION_MAX
07/01/14   sjw       CR596709: created new grm_release_or_exchange() function
24/12/13   ap        CR592011: FR 17482: Page Blocking Management in Multi-SIM Single Radio Devices amendment
14/12/13   sjw       CR589502: Install different unlock callback for change of acq priority
13/12/13   ap        CR588766: FR 15265: DSDA L/TDS/G+G with Opportunistic SGLTE/SGTDS amendment
04/12/13   ap        CR584084 Addressing WCDMA reconfirmation search whilst GERAN in Transfer
04/12/13   aga       CR549390 G2W_TA IRAT Enable W ID search and reconf in case the other G sub is in data call
03/12/13   js        CR574803 TRM extended for Ncell Acquisitions after PCH decode leading to missed page on next SUB
01/12/13   sk        CR583361 Backout CR549390
27/11/13   aga       CR549390 G2W_TA IRAT Enable W ID search and reconf in case the other G sub is in data call
19/11/13   ssh/ip    CR547956 DSDS/DSDA GL1 timeline Opt (parallelize the RF config. with GL1 warm-up)
14/11/13   zc        CR569943:Use correct resource for trm_freq_request
15/11/13   nm        CR575532 Back-out changes for CR537690 & CR547955 - power optimization changes
11/11/13   sk        CR573517 Ensure correct sys mode used by GL1 Arb while giving coex priority
31/10/13   sai       CR567424: Enhance G2W TA suspension mechanism.
24/10/13   sai       CR565751: Fix reference FN for G2W TA suspension reschedule mechanism.
21/10/13   sai       CR563915: Facilitate G2W TA after TBF suspend for BCCH decode.
05/12/13   sjw       CR583608: Use TRM lock acquired using inter-RAT exchange
12/02/13   sk        CR573517 Ensure correct sys mode used by GL1 Arb while giving coex priority
28/11/13   sjw       CR578873: Priority Inversion Management for Multi-SIM (PIMMS)
19/10/13   sk        CR563877 Ensure BCCH reads are given priority over PCH when doing reselection and consider early wakeup time into TRM reservation
31/10/13   sai       CR567424: Enhance G2W TA suspension mechanism.
24/10/13   sai       CR565751: Fix reference FN for G2W TA suspension reschedule mechanism.
13/11/13   ap        CR562878 FR 17482: Page Blocking Management in Multi-SIM Single Radio Devices
11/10/13   sk        CR556552 Ensure idle sub doesn’t invert the priority when other sub using global override for reselection in QTA scenario.
20/09/13   js        CR522426 Multi SIM CBCH priority decision to be taken only when we have more than one sub active
05/10/13   sk        CR554665 Backout the power opt temperorily(CR537690)
14/09/13   sp        CR524905 Update Minimum frames required for BPLMN activity
21/08/13   zf        CR524993: DSDA, when W in BG traffic, XO does not shutdown.
25/09/13   aga       CR547585 - G2W - avoid trm_gta_preemption_callback processing while suspended
24/09/13   zf        CR537690: Reduce G wakeup timeline for MS mode
26/09/13   cgc       CR548645 Generic QTA for 3.0 DSDS bringup changes
16/09/13   cs        CR503042 Updated XO RGS handling for Triton
14/09/13   ap/aga    CR545423 - G2W Tune Away Feature
02/09/13   nm        propagation of CR511322 Reset trm request state if RACH is aborted
26/08/13   gk        CR484897:W2G handover using exchange of TRM lock
10/09/13   cs        CR532298 Integrate all wakeup-mgr changes for Multi-SIM
20/08/13   sk        CR530970 Ensure GBTA works properly in DSDS config
15/08/13   sk        CR524039 GBTA bringup changes
07/08/13   gk        CR509196 :Abort the previous power scan, in case of reception of back-to-back power scan request
01/08/13   sk        CR521884 Priority inversion not working in different DRX combination
12/07/13   cs        CR509747 Apply frequency drift to TRM_DENIED SUB(s)
05/07/13   sk        CR509202 Priority inversion and alternate page skipping changes
02/07/13   sk        CR507513 TRM advanced call back integration to Triton-TSTS
01/07/13   ip        CR506628 - FC power code changes
21/06/13   cs        Major Triton TSTS Syncup
13/06/13   pjr       CR489588: Added check in IS_WCDMA_QTA_SUPPORTED() to return false in SS and DSDA
06/06/13   cs        Further TSTS Updates
28/05/13   ap        CR461268: DSDS XO management + async time drift problem
21/05/13   cs        TSTS Updates
17/05/13   zf        CR488883: Do not use QTA enabled TRM reason before W supports QTA.
27/03/13   am/ap     CR432888 Adjust BPLMN frames for acqusition of BCCH in DSDS mode
26/04/13   zf        CR477992: Fix G2W IRAT issue
29/04/13   ws        CR481359 enable GTA support
25/04/13   zf        CR479905: Reduce default FW load frames to 2
06/04/13   sv        CR462075: Corrected the logic when ignoring GRR commands in GTW PLMN cases.
07/03/13   pjr       CR460494 added macro OTHER_GSM_CLIENT_IS_INACTIVE
12/02/13   ap        CR450842 DSDS: PCH not being decode on SUB2 even though TRM is granted
07/02/13   ap        CR449859 Always enter GFW state MEAS_ACTIVE for gonly sub else
                              enter GFW ACTIVE state
07/02/13   ap        CR449348 DSDS W/G+G IRAT MO Voice: HO from W2G requires attention
18/05/12   mc        DSDS CR359187: LTA activity is granted TRM when GLite FW is still loaded - recovery code
14/02/13   pjr       CR452272 added support to set multisim mode at activation time
09/05/11   sjw       DSDS frame overhead reduction for SCMM.
02/03/11   njt       DSDS CR276931 - BPLMN suspension changes
25/01/11   nt        Added suspension state to L1_multi_sim_acquiring.
10/01/11   ap        DSDS CR270377 SYNC SIM is disabling L1 ISR while ASYNC SIM is still
                     active when consecutive start/stop GSM mode req
23/11/10   nt        DSDS CR250375 Changed the REDUCE_FRAMES_TASK_TO_ISR_LATENCY value
22/11/10   nt        DSDS CR261791 Perform post idle init is other SIM has the lock
13/10/10   nt        DSDS - CR258203 - Missing pages in C+G and W+G modes
24/09/10   nt        DSDS CR253591 Introduced MULTI_SIM_CCCH_FW_LOAD_GUARD for CCCH timeouts
13/08/10   nt        DSDS CR249783 fix to prevent premature release
06/08/10   nt        DSDS CR249192 - Lock out using TRM for FEE. Use FAIR estimate
28/07/10   nt        DSDS - Fixes for GplusG mode
20/07/10   nt        DSDS - Enable GpluG operating mode
09/07/10   nt        Move ASYNC to SYNC sim swap to ISR context
30/06/10   sjw       Introduction of GRM framework
12/05/10   nt        CR232162 - DS delayed BPLMN abort conf's until fully completed
12/04/10   nt        CR232391 - Add the changes to handle iRAT MPH_RECONNECT_REQ
12/04/10   nt        CR229761 - changes to reduce standby current of DS G+G
23/03/10   nt        Changes to prevent BPLMN mechanism from getting into the wrong state
===========================================================================*/

/*===========================================================================

           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "comdef.h"
#include "ms.h"
#include "l1_fm.h"

#include "gpl1_grm_intf.h"
#include "rfm_device_types.h"

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
#include "trm.h"
#include "l1i.h"
#include "geran_dual_sim.h"
#include "geran_multi_sim.h"
#include "gl1_msg_g.h"
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

/* Used for frame based scaling */
#define FRAME_SCALING_FACTOR    (4726) /* 4.615 * 1024     */
#define QSYM_SCLK_SCALING_NUM   (12)   /* One QSYM = 12 / 13 usecs */
#define QSYM_SCLK_SCALING_DENOM (13)
#define FRAME_SCALING_SHIFT     (10)   /* divide by 1024 */
#define TRM_AD_INFINITUM        (0x7FFFFFFF)
#define SCALING_FACTOR_2_MS     (2048) /* 2 * 1024     */

/* Convert msecs into GSM 4.615msecs frame ticks */
#define MSECS_TO_FRAMES( msecs_duration ) \
          ( ( msecs_duration << FRAME_SCALING_SHIFT ) / FRAME_SCALING_FACTOR )

#define FRAMES_TO_MSECS( gsm_frames ) \
          ( ( gsm_frames * FRAME_SCALING_FACTOR ) >> FRAME_SCALING_SHIFT )

#define MODULO_FRAME_SCALING_FACTOR( msecs_duration ) \
          ( ( msecs_duration << FRAME_SCALING_SHIFT ) % FRAME_SCALING_FACTOR )

/*
 * For a particular target what is the maximum valid RF device id
 * that can be assigned to GERAN WAN/WCDMA WAN and LTE WAN
 * these can be different
 */
#define MAX_VALID_GERAN_RFM_DEVICE         RFM_DEVICE_5
#define MAX_VALID_WCDMA_RFM_DEVICE         RFM_DEVICE_5
#define MAX_VALID_LTE_RFM_DEVICE           RFM_DEVICE_5

#define GL1_DRDS_DBG_HDR "GL1_DRDS:"

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )

#define COMPLETE_PROCEDURE_DURATION 0xFFFFFFFF

#define PTM_IDLE_FRAME_FREQ_REGISTRATION_DURATION 3
#define PTM_SUSP_FREQ_REGISTRATION_DURATION 12
#define GPRS_IDLE_AWAKE_FREQ_REGISTRATION_DURATION 4
#define GPRS_IDLE_PM_REGISTARTION_DURATION 3
#define TRM_BAND_TUNE_DENIED 0x1
#define TRM_BAND_TUNE_OK  0x0
#define DR_TA_FALLBACK_DSC_THRESHOLD (50) /*% of max DSC*/

/*======================TRM DURATIONS========================*/
#define RESELECTION_DURATION_MS            200
#define ACQUISITION_DURATION_MS            100
#define PSCAN_MIN_DURATION_MS              (ACQUISITION_DURATION_MS)
#define ACQ_BCCH_LIST_MIN_DURATION_MS      (ACQUISITION_DURATION_MS)
#define RETAIN_LOCK_DEFAULT_FRAME_DURATION 150

/* To cover extended bcch minimum frames left are 40 e.g. extended bcch 
   at frame 5 (+ 6 bcch read) finishes at frame 11, then for the next 
   bcch FN i.e. 52  we get 40 frames. */
#define RESEL_ENTER_COUNTDOWN_THRESHOLD 40

/* 13 is the worst case frame required for idle sub to do the TRM request & notify, if it is too less then idle sub could miss the BCCH occasion
   13 = 1+5+2+3+2
   1 - for cell reselection state machine to back to other state
   5 - is the worst case overhead for transfer sub to align the radio block for GTA gap including 1 idle frame
   2 - transfer sub to release TRM
   3 - idle sub to get TRM and wait for rf/fw warmup time from ISR
   2 - to give some margin 
*/
#define RESEL_EXITS_COUNTDOWN_THRESHOLD  13

#define GRM_ACQUISITION_LOW (TRM_ACQUISITION)     /* this can be toggled HI by TRM internally */
#define GRM_ACQUISITION_MED (TRM_ACQUISITION_INV) /* this can be toggled HI by TRM internally */
#define GRM_ACQUISITION_MAX (TRM_ACQUISITION_MAX) /* this is NOT affected by HI/LO settings   */

#define GRM_ACQUISITION_REASON(rsn) (((rsn) == GRM_ACQUISITION_LOW) || ((rsn) == GRM_ACQUISITION_MED) || ((rsn) == GRM_ACQUISITION_MAX))

#define CHECK_INCLUDES_WCDMA_ACQUISITION (TRM_ACQUISITION)
#define CHECK_EXCLUDES_WCDMA_ACQUISITION (TRM_ACQUISITION_INV_HI) /* non-GSM clients never use this reason */

#define GRM_PAGING_REASON(pr) (((pr) == TRM_DEMOD_PAGE) || ((pr) == TRM_DEMOD_PAGE_MAX_SENS)|| ((pr) == TRM_DEMOD_PAGE_CONTINUATION)|| ((pr) == TRM_DEMOD_PAGE_INV))
#if defined FEATURE_QBTA
#define GRM_PAGING_REASON_ROTA(pr) (((pr) == TRM_DEMOD_PAGE) || ((pr) == TRM_DEMOD_PAGE_MAX_SENS)|| ((pr) == TRM_DEMOD_PAGE_CONTINUATION)|| ((pr) == TRM_DEMOD_PAGE_INV) || ((pr) == TRM_DEMOD_PAGE_INV_HI))
#endif
#define IS_RESEL_OR_ACQ_REASON(rsn) (\
((rsn) == TRM_RESELECT)          || \
((rsn) == TRM_ACQUISITION)       || \
((rsn) == TRM_ACQUISITION_INV)   || \
((rsn) == TRM_ACQUISITION_LOW)   ||  ((rsn) == TRM_ACQUISITION_MAX)  \
)


/*----------------Call in task definitions----------------*/
/* This defines the structures and function prototypes
for the TRM interaction within the task, called via ISR */

/*~~~~~~~~~~~multi_sim_rf_fw_load_t~~~~~~~~~~~*/
typedef struct{
  IMH_T                           header;
  trm_client_enum_t               client_id;
  gas_id_t                        gas_id;
} multi_sim_rf_fw_load_t;

/*~~~~~~~~~~~multi_sim_panic_reset_t~~~~~~~~~~~*/
typedef struct{
  IMH_T                           header;
  gas_id_t                        gas_id;
  l1_panic_reset_type_T           error_code;
  rr_l1_message_id_enum_T         reset_response;
} multi_sim_panic_reset_t;

/*~~~~~~~~~~~multi_sim_gonly_deactivate_t~~~~~~~~~~~*/
typedef struct{
  IMH_T                           header;
  gas_id_t                        gas_id;
} multi_sim_gonly_deactivate_t;

/*~~~~~~~~~~~~~~gl1_complete_ds_abort_t~~~~~~~~~~~~~*/
typedef struct
{
  IMH_T                   header;
  trm_client_enum_t       client;
  rr_l1_message_id_enum_T msg_id;
  gas_id_t                gas_id;
} gl1_complete_ds_abort_t;

typedef enum
{
 PS_ACCESS_NONE,  /* Not PS access */
 PS_ACCESS_MM_DEVICE,/* PS access on multimode device (DEV 0)*/
 PS_ACCESS_NON_MM_DEVICE   /* PS access on non multimode device (! DEV 0)*/
} ps_access_mode_t;

#ifdef FEATURE_G2T_2PHASE_ACCESS
/*===========================================================================

FUNCTION     gta_for_tds_handle_abort_callback

DESCRIPTION  L1 task handler for GTA abort callback which has been triggered from  
             TDSCDMA calling the GTA unlock callback 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gta_for_tds_handle_abort_callback(trm_client_enum_t       client,
                               trm_unlock_event_enum_t event,
                               uint32                  unlock_by_sclk,
                               gas_id_t                gas_id);

#endif /*FEATURE_G2T_2PHASE_ACCESS*/

#ifdef FEATURE_GTA_DURING_PS_RACH
/*===========================================================================

FUNCTION     gta_handle_abort_RACH_for_t_callback

DESCRIPTION  L1 task handler for GTA abort callback which has been triggered from  
             TDSCDMA calling the GTA unlock callback  during RACH state (for PS services)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gta_handle_abort_RACH_for_t_callback(trm_client_enum_t       winning_client,
                                                            trm_reason_enum_t       winning_client_reason,
                                                            trm_unlock_event_enum_t event,
                                                            uint32                  unlock_by_sclk,
                                                            gas_id_t                gas_id);

#endif /* FEATURE_GTA_DURING_PS_RACH */
extern void gl1_handle_abort_callback(
              trm_client_enum_t       client,
              trm_unlock_event_enum_t event,
              uint32                  unlock_by_sclk,
              gas_id_t                gas_id );

extern void gl1_handle_grant_callback(
              trm_client_enum_t          client,
              trm_duration_t             duration,
              gas_id_t                   gas_id );

#ifdef FEATURE_G2X_TUNEAWAY
void g2x_ta_handle_abort_callback(trm_client_enum_t       client,
                               trm_unlock_event_enum_t event,
                               uint32                  unlock_by_sclk,
                               uint32                  winning_client_duration,
                               gas_id_t                gas_id);
#endif /* FEATURE_G2X_TUNEAWAY */
#ifdef FEATURE_SGLTE
extern void gta_handle_abort_for_l_or_t_callback(
              trm_client_enum_t       client,
              trm_unlock_event_enum_t event,
              uint32                  unlock_by_sclk,
              gas_id_t                gas_id);
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_G2X_TUNEAWAY
/* These variables are used to store the last winning client, reason for the last winning client etc.
 * during the GTA gap. These are initialized to invalid values at the end of the GTA gap, inside
 * dsds_init_pch_suspension function. These are updated at the start of the gta_preemption_callback
 * function. The purpose is, we don't want the unlock callback to be called again, if the last winning client
 * and the current winning client are calling this callback for the same reason. The reason to avoid this repeated
 * callback is to avoid delaying the tbf abort time repeatedly.
 */
typedef struct {
  trm_client_enum_t           last_client;
  trm_unlock_event_enum_t     last_event;
  uint32                      last_unlock_by_sclk;
  trm_reason_enum_t           last_unlock_by_reason;
} gpl1_gprs_tbf_abort_cb_store_data_t;

extern gpl1_gprs_tbf_abort_cb_store_data_t gpl1_gprs_tbf_abort_cb_store_data[];

#ifdef FEATURE_G2T_2PHASE_ACCESS

typedef struct {
  uint32    pch_suspension_FN;
  boolean   tick_susp_state_machine;
  boolean   disable_rx;
} gta_tds_info_t;

extern gta_tds_info_t gta_tds_info[];

#endif /*FEATURE_G2T_2PHASE_ACCESS*/

#endif

#define GSM_ACCESS_DURATION_MS 300
#define GSM_ACCESS_DURATION_SCLKS (GSM_ACCESS_DURATION_MS << 5)

boolean  gl1_set_gfw_to_active(gas_id_t gas_id);

boolean gl1_detect_page_collision_with_other_sub( gas_id_t gas_id );

#if defined FEATURE_QBTA
boolean gl1_check_qbta_allowed(gas_id_t gas_id);
#endif
trm_client_enum_t gl1_get_client_id_of_other_sub_in_transfer(gas_id_t gas_id);

/*===========================================================================

FUNCTION  l1_isr_find_bcch

DESCRIPTION
  Do top level ISR processing for mode mentioned in function name.

DEPENDENCIES
  l1_tskisr_blk

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_isr_multi_sim_find_bcch( gas_id_t gas_id );

/*===========================================================================

FUNCTION  l1_isr_multi_sim_null

DESCRIPTION
  Do top level ISR processing for dual sim null mode.

DEPENDENCIES
  l1_tskisr_blk

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_isr_multi_sim_null(gas_id_t gas_id);

/*===========================================================================

FUNCTION  l1_isr_idle_mode

DESCRIPTION
  Do top level ISR processing for mode mentioned in function name.

DEPENDENCIES
  l1_tskisr_blk

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_isr_multi_sim_idle( gas_id_t gas_id );
/*===========================================================================

FUNCTION  l1_isr_multi_sim_decoupled

DESCRIPTION
  Do top level ISR processing for mode mentioned in function name.

DEPENDENCIES
  l1_tskisr_blk

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_isr_multi_sim_decoupled( gas_id_t gas_id );
/*===========================================================================

FUNCTION  l1_isr_handle_multi_sim_find_sch_command

DESCRIPTION
   Handle the main command mentioned in the function name in various modes.

DEPENDENCIES
  l1_tskisr_blk

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_isr_handle_multi_sim_find_sch_command( gas_id_t gas_id );

/*===========================================================================

FUNCTION  l1_isr_handle_multi_sim_find_bcch_command

DESCRIPTION
 used for dual sim acqusition activities and reselection
DEPENDENCIES
  l1_tskisr_blk

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_isr_handle_multi_sim_find_bcch_command(gas_id_t gas_id);

/*===========================================================================

FUNCTION  gl1_handle_grm_requests

DESCRIPTION
  Handles the issuing of TRM requests for sudden changes of state
  to access mode, dedicated mode, transfer mode. The issuing of the fw
  and rf loading is also contained within this state machine.

DEPENDENCIES
  l1_tskisr_blk,l1_isr_data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean gl1_handle_grm_requests( trm_reason_enum_t reason,     boolean  reset_state,
                                        ARFCN_T           arfcn_info, gas_id_t gas_id );

/*===========================================================================

FUNCTION  l1_init_multi_sim_data

DESCRIPTION
  Initialises the DUAL SIM data

DEPENDENCIES
  l1_tskisr_blk, l1_tskisr_blk_1

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_init_multi_sim_data( gas_id_t gas_id );

/*===========================================================================

FUNCTION l1_swap_multi_sim_sync_state_or_deactivate

DESCRIPTION
  This function handles the deactivation of one of the dual sims
  If the SIM that is deactivating is the SYNC SIM, and there is
  an ASYNC SIM this must become the SYNC SIM.

DEPENDENCIES
  This function needs to be run at the end of the isr. Beacuse if the sync sim
  enters inactive mode this is the trigger to pull down the isr. If there are two sims
  and one becomes inactive we do not want to tear down the isr.

  +++++ This must occur in task context due to the call to trm_release +++++

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void l1_swap_multi_sim_sync_state_or_deactivate( gas_id_t gas_id );

/*===========================================================================

FUNCTION   call_in_task_multi_sim_panic_reset

DESCRIPTION
   Sends a message to the layer1 task telling it to action the panic reset
   mechanism

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void call_in_task_multi_sim_panic_reset( gas_id_t gas_id );

/*===========================================================================

FUNCTION   call_in_task_gplusg_deactivate

DESCRIPTION
   Sends a message to the layer1 task telling it to call the function
   that will deactivate the mdsp and rf

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void call_in_task_gplusg_deactivate( gas_id_t gas_id );

/*===========================================================================

FUNCTION   l1_set_global_override_trm_reason

DESCRIPTION
   Sets a TRM reson to be used in IDLE mode, that will oveeride the normal
   expected reasons. This will mean that instead of usaing the TRM_DEMOD_PAGE
   the reason can be set elsewhere to increase the priority to TRM_RESELECT or
   TRM_DEMOD_PAGE_MAX_SENS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_set_global_override_trm_reason( trm_reason_enum_t reason, gas_id_t gas_id );

/*===========================================================================

FUNCTION   l1_get_global_override_trm_reason

DESCRIPTION
   Gets a TRM reson to be used in IDLE mode, that will overide the normal
   expected reasons. If no override reason is set then TRM_DEMOD_PAGE is
   the default.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
trm_reason_enum_t l1_get_global_override_trm_reason( gas_id_t gas_id );

/*===========================================================================

FUNCTION   l1_clear_global_override_trm_reason

DESCRIPTION
   Clears out the override reason for the TRM reservation request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_clear_global_override_trm_reason( gas_id_t gas_id );

/*===========================================================================

FUNCTION   gl1_multi_sim_fw_rf_load

DESCRIPTION
   Sends a message to the layer1 task telling it to call the function
   to action the load of the firmware and RF modules

   IN - data - The data to be passed to the function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gl1_multi_sim_fw_rf_load( trm_client_enum_t client_id, gas_id_t gas_id );

/*===========================================================================

FUNCTION  grm_abort_callback

DESCRIPTION
  This function handles the trm abort callback for both GSM clients.
  And changes the trm_status within the clients global data, this is picked
  up by the ISR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void grm_abort_callback( trm_unlock_callback_data *abort_cb_data_p );

extern void gpl1_gprs_complete_ds_abort(trm_client_enum_t       client_id,
                                        rr_l1_message_id_enum_T pending_msg,
                                        gas_id_t                gas_id);

extern boolean gl1_check_all_subs_l1_state( byte l1_state );
extern boolean gl1_check_any_subs_l1_state( byte l1_state );

/*===========================================================================

FUNCTION  grm_enh_grant_callback

DESCRIPTION
  This function handles the enhanced GRANT callback for both GSM clients. It changes
  the trm_status within the clients global data, this is picked up by the ISR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void grm_enh_grant_callback( trm_client_enum_t          granted_client,
                                    trm_grant_return_enum_type grant_status,
                                    trm_duration_t             duration );

#ifdef FEATURE_G2X_TUNEAWAY
/*===========================================================================

FUNCTION  grm_gta_preemption_callback

DESCRIPTION
  This function handles the TRM abort callback for the GSM client
  that is doing the Packet transfer
  And changes the trm_status within the clients global data, this is picked
  up by the ISR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void grm_gta_preemption_callback(
              trm_unlock_callback_data *unlock_data_p );


/*===========================================================================

FUNCTION  grm_tbf_enh_grant_callback

DESCRIPTION
  This function handles the enhanced GRANT callback for the GSM client
  that is doing the Packet transfer . It changes
  the trm_status within the clients global data, this is picked up by the ISR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void grm_tbf_enh_grant_callback
(
  trm_client_enum_t               client,
  trm_grant_return_enum_type      grant_status,
  trm_duration_t                  duration
);





boolean gpl1_gprs_get_gta_config(gas_id_t gas_id);
boolean gpl1_gprs_is_this_the_active_tbf_id(gas_id_t current_gas_id);
boolean gl1_page_collision_with_x_sub( trm_client_enum_t client,l1_fm_priority_T priority, gas_id_t gas_id  );



#endif /* FEATURE_G2X_TUNEAWAY */



/*===========================================================================

FUNCTION  gpl1_multi_sim_calculate_grm_reservation_time

DESCRIPTION
  This function determines the required TRM request period in sclcks,
  factoring in the time duringt he frame at which this is called.

DEPENDENCIES
  None

RETURN VALUE
  TRM period request in sclcks.

SIDE EFFECTS
===========================================================================*/
uint32 gpl1_multi_sim_calculate_grm_reservation_time( uint32   requested_FN,
                                                      gas_id_t gas_id );

/*===========================================================================

FUNCTION  gpl1_multi_sim_convert_frames_to_sclcks

DESCRIPTION
  This function determines the requested GSM frames period in sclcks.

DEPENDENCIES
  None

RETURN VALUE
  Requested GSM frames period request in sclcks.

SIDE EFFECTS
===========================================================================*/
uint32 gpl1_multi_sim_convert_frames_to_sclcks( uint32 Gframes );

/*===========================================================================

FUNCTION  GPL1_GPRS_HANDLE_MULTI_SIM_FIND_BCCH

DESCRIPTION
  This function handles the L1 Task in dual sim find bcch mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_handle_multi_sim_find_bcch(IMH_T *msg_header);

/*===========================================================================

FUNCTION  gpl1_gprs_handle_multi_sim_idle

DESCRIPTION
  This function handles the L1 Task in idle mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_handle_multi_sim_idle(IMH_T *msg_header, gas_id_t gas_id);

/*===========================================================================

FUNCTION  gpl1_gprs_handle_multi_sim_null

DESCRIPTION
  This function handles the L1 Task in dual sim null mode, and the messages
  received within the task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_handle_multi_sim_null( IMH_T * msg_header );
/*===========================================================================

FUNCTION  gpl1_gprs_handle_multi_sim_decoupled

DESCRIPTION
  This function handles the L1 Task in dual sim decoupled mode, and the messages
  received within the task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_gprs_handle_multi_sim_decoupled(IMH_T *msg_header);


/*===========================================================================

FUNCTION  l1_ds_isr_inactive_mode

DESCRIPTION
  Do top level ISR processing for mode mentioned in function name.
  This mimicks l1_isr_inactive_mode but it does not tear down the gstmr handler
  as there is still a sim active.

DEPENDENCIES
  l1_tskisr_blk

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_ds_isr_inactive_mode( gas_id_t gas_id );

/*===========================================================================

FUNCTION  L1_send_PH_RANDOM_ACCESS_CNF_with_result

DESCRIPTION
  This function sends the PH_RANDOM_ACCESS_CNF to L2.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void L1_send_PH_RANDOM_ACCESS_CNF_with_result(const gas_id_t gas_id,
                                              const geran_result_t rach_result);

/*===========================================================================

FUNCTION gl1_ms_switch_multi_sim_data

DESCRIPTION
             Only used for Dual SIM/Dual data space.  Switches the working
             data pointer to the data space indicated by gas_id.

PARAMS       gas_id - geran access stratum id.

DEPENDENCIES

RETURN VALUE None

===========================================================================*/
void gl1_ms_switch_multi_sim_data(gas_id_t gas_id);

/*===========================================================================

FUNCTION  gpl1_multi_sim_bplmn_decode_bcch_inactive

DESCRIPTION
  Rteurns the status of the parrallel bcch decode state machine

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean gpl1_multi_sim_bplmn_decode_bcch_inactive( gas_id_t gas_id  );

/*===========================================================================

FUNCTION  gpl1_multi_sim_panic_reset

DESCRIPTION
          Performs the Dual sim specific panic reset

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_multi_sim_panic_reset( l1_panic_reset_type_T   error_code,
                                 rr_l1_message_id_enum_T reset_response,
                                 gas_id_t                gas_id );


typedef enum{
  L1_MULTI_SIM_BGRND_HPLMN_NULL,
  L1_MULTI_SIM_BGRND_HPLMN_FW_LOADING,
  L1_MULTI_SIM_BGRND_HPLMN_RUNNING,
  L1_MULTI_SIM_BGRND_HPLMN_SUSPENDING
}background_hplm_state_t;

/*===========================================================================

FUNCTION  l1_multi_sim_bplmn_abort

DESCRIPTION
  This function is used as the abort callback by the frame manager to ensure
  that the background activity cleanly completes. The action is
  simply to change the trm_status to GL1_TRM_ABORTING to force the state
  machine through its cleanup proceedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l1_multi_sim_bplmn_abort ( void* dummy, gas_id_t gas_id );

/*===========================================================================

FUNCTION  gpl1_get_multi_sim_bplmn_state

DESCRIPTION
  returns the variable background_hplm_state
===========================================================================*/
background_hplm_state_t gpl1_get_multi_sim_bplmn_state( gas_id_t gas_id );

int8 gl1_check_num_active_G_subs( void );
extern boolean l1_get_starting_time_elapsed( gas_id_t gas_id );
extern void l1_set_starting_time_elapsed( boolean,gas_id_t gas_id );
extern void l1_set_access_isr_send_mac_sb_failure( boolean value,gas_id_t gas_id );
/* EXTERNS used in gpl1_dual_sim.c */
extern void gl1_load_current_cell_data( gas_id_t gas_id );
extern void gl1_begin_INTERRAT_Reconnect(gas_id_t gas_id);

/* Sends the block quality unknown to RR */
void L1_send_UNKNOWN_CCCH_MPH_BLOCK_QUALITY_IND( gas_id_t gas_id );

/* global define for the time to allow the FW/RF to load up on a dual sim build */
#ifdef T_MSM7x27
#define MULTI_SIM_FW_LOAD_FRAMES_DEFAULT 5
#else
#define MULTI_SIM_FW_LOAD_FRAMES_DEFAULT 0
#endif /*T_MSM7x27*/

#define L1_MIN_HDR_INTER_GAP 11 /* Minimum seperation between consecutive QTA gaps */

#ifdef FEATURE_QTA
/*
** 3.0 DSDS for generic QTA now 1 frame earlier to allow for either QTA or LTA, was 0
*/
#define MULTI_SIM_FW_LOAD_FRAMES_QTA 1
#endif


#define ZERO_MULTI_SIM_FW_LOAD_FRAMES_DEFAULT 0
#define SHORT_MULTI_SIM_FW_LOAD_FRAMES_DEFAULT 0
#define MULTI_SIM_CCCH_FW_LOAD_GUARD 10
#define L1_MIN_WCDMA_INTER_QTA_GAP 11 /* Minimum seperation between consecutive QTA gaps */

typedef struct{
  boolean           fw_loaded_and_ready;
  trm_client_enum_t active_client;
  boolean           three_subs_page_colliding;
} GOnly_mode_settings_t;

extern GOnly_mode_settings_t GOnly_mode_settings;


#ifdef FEATURE_GPRS_GBTA

typedef enum {
  GBTA_EV_NONE,
  GBTA_EV_IDLE_PWR_MON  /* event set during PCH Decode */
}grm_gbta_idle_event_t;

typedef enum {
  GBTA_STATUS_NONE,
  GBTA_STATUS_PWR_MON_ACTIVE,      /* Idle Power Monitor in process of being scheduled */
  GBTA_STATUS_PWR_MON_SCHEDULED    /* Idle Power Monitor Scheduled */
}grm_gbta_idle_event_status_t;

typedef enum {
  GRM_GBTA_NONE,
  GRM_GBTA_ALLOWED,
  GRM_GBTA_ACTIVE,
  GRM_GBTA_BLOCKED
}grm_gbta_state_t;

typedef struct {
  boolean                          leaving_idle_mode; /* allows not to schedule pwr monitor */
  boolean                          missed_page; /*indicates whether last page was missed*/
  grm_gbta_idle_event_t            idle_event;
  grm_gbta_idle_event_status_t     idle_event_status;
}grm_gbta_info_t;

extern grm_gbta_info_t grm_gbta_info[NUM_GERAN_DATA_SPACES];
extern grm_gbta_state_t            gbta_state;

#endif /*FEATURE_GPRS_GBTA */

extern uint16 MULTI_SIM_FW_LOAD_FRAMES;
#define REDUCE_FRAMES_TASK_TO_ISR_LATENCY 1
#define RF_FW_SETUP_FRAME_DELAY           2
#define RF_FW_WARMUP_FRAME_DELAY          1
#define SUSPENSION_FRAME_DELAY            2
#define ACQ_SUSPENSION_FRAME_DELAY        3

#define CHECK_ANY_GSM_CLIENT_HAS_LOCK() \
( !gl1_check_any_subs_trm_status( GL1_TRM_GRANTED ) && \
  !gl1_check_any_subs_trm_status( GL1_TRM_RETAINED_FOR_ACCESS ) )

#define CHECK_ANY_GSM_CLIENT_IS_ABORTING_LOCK() \
( !gl1_check_any_subs_trm_status( GL1_TRM_ABORTING ) )

#define GONLY_OPERATION_IS_ACTIVE() \
( gl1_check_gonly_mode_active() )

#define CHECK_ANY_OTHER_GSM_CLIENT_IS_INACTIVE( gas_id ) \
( gl1_check_any_other_subs_l1_state( gas_id, L1_INACTIVE_MODE ) )

#define CHECK_ANY_OTHER_GSM_CLIENT_IS_GOING_ACTIVE( gas_id ) \
( gl1_check_any_other_subs_active_state( gas_id ) )

#define GBTA_ALLOWED_CAN_BE_SET(gas_id) \
( !gl1_check_all_other_subs_l1_state( gas_id, L1_INACTIVE_MODE ) )



#define OTHER_GSM_CLIENT_IN_TRANSFER(gas_id) \
( gl1_check_any_other_subs_l1_state(gas_id, L1_TRANSFER_MODE))


#define OTHER_GSM_CLIENT_IN_PACKET_ACCESS(gas_id) \
( \
  (l1_transfer_data[(gas_id)].current_alloc_type == L1_SINGLE_BLOCK_ALLOC) \
)

#ifdef FEATURE_GPRS_GBTA

/* Definition for GBTA */

#define IS_SUB_LEAVING_IDLE(gas_id) \
(\
  grm_gbta_info[gas_id].leaving_idle_mode \
)

/*
** GBTA STATE MACROS
*/
#define SET_GBTA_STATUS(gbta_status)  \
(\
  (gbta_state = gbta_status) \
)

#define GET_GBTA_STATE gbta_state \


#define IS_GBTA_ALLOWED() \
( \
  (gbta_state == GRM_GBTA_ALLOWED) \
)

#define IS_GBTA_BLOCKED() \
( \
  (gbta_state == GRM_GBTA_BLOCKED) \
)

#define IS_GBTA_ACTIVE() \
( \
  (gbta_state == GRM_GBTA_ACTIVE) \
)

#define IS_GBTA_INACTIVE() \
( \
  (gbta_state == GRM_GBTA_NONE) \
)

#define IS_GBTA_ALLOWED_OR_ACTIVE() \
( \
  (gbta_state == GRM_GBTA_ALLOWED) || \
  (gbta_state == GRM_GBTA_ACTIVE)     \
)

#define IS_IDLE_EV_PWR_MON_ACTIVE(gas_id) \
  (grm_gbta_info[gas_id].idle_event_status == GBTA_STATUS_PWR_MON_ACTIVE)


#define IS_IDLE_EV_PWR_MON_SCHEDULED(gas_id) \
  (grm_gbta_info[gas_id].idle_event_status == GBTA_STATUS_PWR_MON_SCHEDULED)

#define IS_IDLE_EV_PWR_MON_SET(gas_id) \
  (grm_gbta_info[gas_id].idle_event == GBTA_EV_IDLE_PWR_MON)

#define IS_GBTA_REASON(rsn) (\
((rsn) == TRM_DEMOD_PAGE)             || ((rsn) == TRM_DEMOD_BROADCAST) || \
((rsn) == TRM_DEMOD_PAGE_MAX_SENS)    || ((rsn) == TRM_DEMOD_PAGE_CONTINUATION) || \
((rsn) == TRM_SMALL_SCI_PAGE)         || ((rsn) == TRM_DEMOD_PAGE_INV)  || \
((rsn) == TRM_DEMOD_PAGE_INV_HI)  \
)

#if defined FEATURE_QBTA
#define IS_QBTA_REASON(rsn) (\
((rsn) == TRM_DEMOD_PAGE)      || ((rsn) == TRM_DEMOD_PAGE_HI)      || \
((rsn) == TRM_DEMOD_PAGE_INV)  || ((rsn) == TRM_DEMOD_PAGE_INV_HI)  || \
((rsn) == TRM_DEMOD_BROADCAST) || ((rsn) == TRM_DEMOD_BROADCAST_HI) || ((rsn) == TRM_DEMOD_PAGE_CONTINUATION)\
)
#endif

#define OTHER_SUB_RELEASE_PENDING() \
 ( gl1_check_other_sub_release_pending( ))

#define OTHER_SUB_RECONFIG_PROGRESS(frames_required) \
(gl1_check_other_sub_reconfig_progress (frames_required))

#define OTHER_SUB_START_SC_TICK_TRANS(gas_id) \
(gpl1_start_sc_tick_trans(gas_id))

#define OTHER_SUB_STOP_SC_TICK_TRANS(gas_id) \
( gpl1_stop_sc_tick_trans(gas_id))

#define OTHER_SUB_SC_OP_IN_PROGRESS() \
(gl1_check_other_sub_sc_op_progress())

#define OTHER_SUB_WCDMA_ID_SEARCHING_IN_TBF(gas_id) \
(gl1_check_other_sub_id_search_progress( ))

#ifdef FEATURE_GSM_TDS
#define OTHER_SUB_TDS_IRAT_IN_IDLE_FRAME(gas_id) \
(gl1_check_other_sub_tds_irat_progress( ))
#endif

#define OTHER_SUB_NCELL_MEAS_IN_PROGRESS(gas_id) \
(gl1_check_other_sub_ncell_meas_progress())
/* GBTA Assocaited Functions */

#define OTHER_SUB_IN_TBF_SUSP() \
(gl1_check_other_sub_in_tbf_susp())

#define OTHER_SUB_SUSP_PLANNED(gas_id) \
(gl1_other_sub_check_tbf_susp_planned())

#define OTHER_SUB_MISSED_LAST_PAGE(gas_id) \
(gl1_other_sub_check_missed_page(gas_id))

#define SET_MISSED_PAGE_STATUS(missed_page_status,gas_id) \
(\
  (grm_gbta_info[gas_id].missed_page = missed_page_status) \
)

#define OTHER_G_SUB(gas_id) \
(gl1_get_gas_id_of_other_g_sub( gas_id ))

#define OTHER_G_SUB_IN_TRANSFER(gas_id) \
(gl1_get_gas_id_of_other_g_sub_in_transfer( gas_id ))

/* GBTA Assocaited Functions */

void grm_notify_event(grm_gbta_idle_event_t event,boolean leaving_idle,gas_id_t gas_id );
boolean gl1_gbta_schedule_idle_pwr_mon_if_allowed(gas_id_t gas_id);
void    gl1_gbta_scheduling_idle_event(grm_gbta_idle_event_t event,gas_id_t gas_id);

gas_id_t gl1_gbta_get_transfer_sub(gas_id_t gas_id);
boolean gl1_is_gbta_reason(trm_reason_enum_t reason,gas_id_t gas_id);
void    gpl1_gbta_schedule_idle_pwr_mon(gas_id_t gas_id);
boolean gl1_gbta_check_gbta_allowed(trm_reason_enum_t req_reason,uint32 frames_required,gas_id_t gas_id);
boolean gpl1_check_no_transfer_sub_conflict(uint32 frames_req,gas_id_t gas_id);

boolean gl1_gbta_schedule_idle_per_mon_if_allowed(gas_id_t gas_id);
boolean gpl1_gbta_check_tbf_susp_planned(gas_id_t gas_id );
void    gpl1_gbta_reset_active_state(gas_id_t gas_id);


#endif  /*FEATURE_GPRS_GBTA */

#ifdef L1_HOST_TEST
#error code not present
#endif/*L1_HOST_TEST*/

#define IS_QTA_REASON(rsn) (\
((rsn) == TRM_SMALL_SCI_PAGE)  || \
((rsn) == TRM_DEMOD_PAGE)      || ((rsn) == TRM_DEMOD_PAGE_INV ) || \
((rsn) == TRM_DEMOD_BROADCAST) || ((rsn) == TRM_DEMOD_PAGE_CONTINUATION ) \
)

#define IS_GTA_REASON(rsn) (\
((rsn) == TRM_DEMOD_PAGE)            ||  ((rsn) == TRM_RESELECT)      || \
((rsn) == TRM_ACQUISITION_INV)       ||  ((rsn) == TRM_ACQUISITION)      || \
((rsn) == TRM_DEMOD_PAGE_MAX_SENS)   ||  ((rsn) == TRM_CHANNEL_MAINTENANCE)   || \
((rsn) == TRM_DEMOD_BROADCAST)       ||  ((rsn) == TRM_DEMOD_PAGE_CONTINUATION) || \
((rsn) == TRM_ACQUISITION_MAX)       ||  ((rsn) == TRM_SMALL_SCI_PAGE)     || \
((rsn) == TRM_DEMOD_PAGE_INV)        ||  ((rsn) == TRM_BROADCAST_ACCESS)\
)
#ifdef FEATURE_G2X_TUNEAWAY

#define MULTI_SIM_FW_LOAD_FRAMES_GPAGE_GTA 0

boolean gpl1_is_client_g(trm_client_enum_t client_id); 
#define IS_CLIENT_G(client) (gpl1_is_client_g(client))

boolean gpl1_g2x_ta_is_ts_change_missed(gas_id_t gas_id);
boolean gpl1_g2x_ta_is_seq_registered_not_G(gas_id_t gas_id);

#ifdef  FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
void gl1_trans_grm_reserve_at_self_ccch( uint32 required_frame_nbr,byte page_mode,gas_id_t gas_id );
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

#endif /* FEATURE_G2X_TUNEAWAY */


#define IS_WCDMA_IN_BG_TRAFFIC() ((trm_get_extension_flag(TRM_UMTS, TRM_BG_TRAFFIC, TRM_PRIORITY_INV_ENABLED)) && \
                                   gl1_msg_get_multi_sim_mode() )

#define WCDMA_QTA_SUPPORTED_MASK (TRM_PRIORITY_INV_ENABLED + TRM_QUICK_TA_SUPPORTED)

#define IS_WCDMA_VOTED_PAGE_SKIP() (trm_get_extension_flag(TRM_UMTS, TRM_BG_TRAFFIC, TRM_ALTERNATE_PAGE_ENABLED))

#define WCDMA_LTA_PAGE_SKIP_MASK (TRM_PRIORITY_INV_ENABLED + TRM_ALTERNATE_PAGE_ENABLED)


#define IS_WCDMA_QTA_SUPPORTED() ( ((trm_get_extension_mask(TRM_UMTS, TRM_BG_TRAFFIC) & WCDMA_QTA_SUPPORTED_MASK) == WCDMA_QTA_SUPPORTED_MASK) && \
                                   gl1_msg_get_multi_sim_mode() )

#if defined ( FEATURE_TRIPLE_SIM )
#define IS_MULTI_SIM_MODE( mode ) ( ( (mode) == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY ) \
                                    || ( (mode) == SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY ) \
                                    || ( (mode) == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE ) \
                                  )
#else
#define IS_MULTI_SIM_MODE( mode ) ( ( (mode) == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY ) \
                                    || ( (mode) == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE ) \
                                  )
#endif /* FEATURE_TRIPLE_SIM */

/* GRM definitions start here */
boolean grm_send_trm_band_registration_start(const trm_band_t band, gl1_sc_activities_t sc_activity, uint32 frames_required,gas_id_t gas_id,  grm_info_t *info_ptr);

boolean grm_check_idle_sc_activity_ok(l1_fm_priority_T pri, gas_id_t gas_id, uint32 frames_required, uint32 reserved_fn);

void grm_add_freq_type_to_trm_freq_input_type(
       const trm_frequency_type_t gl1_freq_info,
       const trm_client_enum_t    client_id );

void grm_add_sys_band_type_to_trm_freq_input_type(
       const trm_band_t band,
       const trm_client_enum_t    client_id,
       gl1_sc_activities_t sc_activity);

void grm_clear_stale_client_state_information(const trm_client_enum_t    client_id , gl1_sc_activities_t sc_activity);

void grm_send_band_release_to_trm(
       gas_id_t gas_id,
       gl1_sc_activities_t activity,
       boolean clear_irat_bands);

void grm_update_client_state_info(grm_sc_activity_state_info_t * activity_info, gl1_sc_activities_t activity);

boolean grm_check_w_t_reconf_in_pg_block_ok( gas_id_t gas_id, uint32 frames_required, uint32 freq_index);

boolean grm_check_ptm_irat_sc_activity_ok( gas_id_t gas_id, uint16 freq_offset,  uint32 frames_required);

boolean grm_check_ptm_ncell_sc_activity_ok(sys_band_T sc_band, gl1_sc_activities_t sc_activity, gas_id_t gas_id, uint32 frames_required);

gl1_sc_activities_t map_fm_priority_to_sc_activity(l1_fm_priority_T pri);

/* GRM definitions start here */

/* extern void grm_complete_x2g_drdsds_ho_operation(sys_band_T band); */

extern void grm_set_slot_cycle( const trm_client_enum_t client_id,
                                const uint8             bs_pa_mfrms);

extern void grm_register_pbr_info( trm_client_enum_t client_id,
                                   uint32            drx_cycle );

extern void gl1_update_grm_reservation_for_ta( gas_id_t gas_id );

extern void gpl1_multi_sim_setup_afc_tt( gas_id_t gas_id );

extern void grm_request_at_warmup( gas_id_t gas_id );

extern void grm_enh_grant_panic_reset_callback(
             trm_client_enum_t          client,
             trm_grant_return_enum_type grant_status,
             trm_duration_t             duration
);

extern void grm_setup_access_retry(gas_id_t gas_id, boolean pending_gl1_event, ps_access_mode_t access_for_ps);

uint32 l1_bcch_current_bcch_fn( gas_id_t gas_id );

extern boolean gl1_ms_trm_blocks_activity(trm_client_enum_t active_client,
                                          trm_reason_enum_t extra_wcdma_reason,
                                          gas_id_t           gas_id);

extern boolean gl1_is_granted_geran_sub_doing_reselection(trm_client_enum_t active_client);
extern trm_resource_enum_t gl1_ms_get_trm_resource(gas_id_t gas_id, trm_reason_enum_t reason);

extern boolean gl1_is_any_sub_in_transfer( void );

extern boolean gl1_check_any_subs_l1_state( byte l1_state );
extern boolean gl1_check_any_other_subs_l1_state( gas_id_t gas_id, byte l1_state );
boolean gl1_check_any_other_sub_using_trm_global_overide( gas_id_t gas_id );
extern boolean gl1_get_trm_global_overide_status( gas_id_t gas_id );

trm_client_enum_t  gl1_get_client_id_of_other_idle_sub(gas_id_t gas_id);
trm_reason_enum_t gl1_get_reserve_reason(gas_id_t gas_id);

extern boolean gl1_check_any_other_subs_active_state( gas_id_t gas_id );

#ifdef FEATURE_GPRS_GBTA
extern boolean gl1_check_any_other_subs_idle_evt_power_mon( gas_id_t gas_id,gas_id_t *idle_gas_id_ptr );
extern void gl1_set_idle_event_status( grm_gbta_idle_event_status_t ev_stat, gas_id_t gas_id );
extern boolean gl1_check_other_subs_idle_event_status(grm_gbta_idle_event_status_t stat, gas_id_t gas_id ,gas_id_t *idle_gas_id_ptr);
extern boolean gl1_check_other_sub_release_pending( void );
extern boolean gl1_check_other_sub_reconfig_progress( uint32 frames_required );
extern boolean gl1_check_other_sub_sc_op_progress( void );
extern boolean gl1_check_other_sub_id_search_progress( void );
#ifdef FEATURE_GSM_TDS
extern boolean gl1_check_other_sub_tds_irat_progress( void );
#endif
extern boolean gl1_check_other_sub_ncell_meas_progress( void );
extern boolean gl1_check_other_sub_in_tbf_susp( void );
extern boolean gl1_other_sub_check_tbf_susp_planned(void);
extern boolean gl1_other_sub_check_missed_page( gas_id_t gas_id );
extern boolean gl1_check_all_other_subs_l1_state( gas_id_t gas_id, byte l1_state  );
extern boolean gl1_check_gonly_mode_active( void );
extern boolean gl1_is_own_gbta_active( gas_id_t gas_id );
#endif

extern boolean gl1_check_any_other_subs_fw_load( gas_id_t gas_id );

extern boolean gl1_check_any_subs_trm_status( gl1_trm_state_t trm_status );

extern gas_id_t gl1_get_gas_id_for_ptm_client( void );

gas_id_t gl1_get_peer_gas_id_for_ptm_client( void );

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
trm_client_enum_t gl1_get_current_trm_client_id( trm_client_enum_t client );
#endif 

extern boolean gl1_check_any_other_client_has_lock( gas_id_t gas_id );
extern boolean l1_sc_wcdma_get_gsm_only_mode( gas_id_t gas_id );

extern gas_id_t gl1_get_gas_id_of_other_g_sub( gas_id_t gas_id  );
extern gas_id_t gl1_get_gas_id_of_other_g_sub_in_transfer( gas_id_t gas_id  );

extern uint8 grm_num_clashing_subs( void );
extern uint8 gl1_get_num_inactive_subs( void );
extern gas_id_t gl1_get_gas_id_of_other_idle_sub(gas_id_t gas_id);
extern boolean gl1_check_any_other_sub_in_bg_traffic( gas_id_t gas_id );
background_hplm_state_t gl1_ms_background_hplmn_state( gas_id_t gas_id );


void gl1_multi_sim_reset_trm_request_state( gas_id_t gas_id );
extern boolean gl1_check_possible_isr_overrun(gas_id_t gas_id);
extern boolean gl1_check_qta_active_in_warmup(gas_id_t gas_id);

void gpl1_grm_reserve_rx_sec(gas_id_t gas_id, trm_time_t *when, trm_duration_t *duration, trm_reason_enum_t *reason);
void gpl1_grm_release_rx_sec(gas_id_t gas_id);
void gpl1_grm_request_rx_sec(gas_id_t gas_id, trm_duration_t *duration, trm_grant_return_enum_type *trm_event, boolean init_start);

boolean gl1_check_if_getting_deactivated(gas_id_t gas_id);

/*==============================================================================
FUNCTION   gl1_get_global_trm_reason

DESCRIPTION
this function is used to get the global trm reason

DEPENDENCIES
  None

RETURN VALUE
  global trm reason

SIDE EFFECTS
  None
======================================*/

trm_reason_enum_t gl1_get_global_trm_reason( gas_id_t gas_id );

#if defined(FEATURE_SGLTE_DUAL_SIM) || defined(FEATURE_SVLTE_DUAL_SIM)
void l1_handle_device_mode_cause_plmn(const gas_id_t gas_id);
#endif

#ifdef FEATURE_DUAL_DATA
/*===========================================================================

FUNCTION grm_set_data_sub_priority

DESCRIPTION
  Sets specific Subscription to assigned priority as sets other subscriptions
  to GERAN_MMS_PRIORITY_NOMINAL

PARAMS
  None

DEPENDENCIES

RETURN VALUE

===========================================================================*/
void grm_set_data_sub_priority(geran_priority_t priority,gas_id_t gas_id);

/*===========================================================================

FUNCTION grm_get_data_sub_priority

DESCRIPTION
  Returns the priority of subscription

PARAMS
  None

DEPENDENCIES

RETURN VALUE

===========================================================================*/
geran_priority_t grm_get_data_sub_priority(gas_id_t gas_id);
#endif /* FEATURE_DUAL_DATA */

boolean gl1_check_svlte_mode_active(void);

#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

#endif /* GPL1_DUAL_SIM_H */
