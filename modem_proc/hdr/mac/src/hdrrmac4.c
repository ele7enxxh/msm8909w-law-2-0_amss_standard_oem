/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H D R   S U B T Y P E  4
                           
             R E V E R S E   T R A F F I C   C H A N N E L   M A C

                            P R O T O C O L

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Reverse Traffic Channel
  Media Access Control (MAC) Protocol for an HDR access terminal.

EXTERNALIZED FUNCTIONS
  hdrrmac4_init
    Initialize the Reverse Traffic Channel MAC protocol.

  hdrrmac4_activate
    Activate the Reverse Traffic Channel MAC protocol.
    Power up RF Tx hardware.

  hdrrmac4_deactivate
    Deactivate the Reverse Traffic Channel MAC protocol.
    Turn PA off (if necessary), turn RF Tx off (if necessary).

  hdrrmac4_is_active
    Determine if protocol is active.

  hdrrmac4_start_tx
    Start TX after it has been stopped or after an activate command.
    If RF warmup is complete, turn PA on, start data.

  hdrrmac4_stop_tx
    Stop (suspend) transmission.
    Turn PA off, send HDRIND_RTCMAC_TX_STOPPED, stop asking for data.

  hdrrmac4_powerdown_tx
    Turns off RF TX hardware while RTC MAC is active.

  hdrrmac4_powerup_tx
    Turns on RF Tx hardware that was turned off with hdrrmac4_powerdown_tx().

  hdrrmac4_suspend_connection
    Suspends a connection -- no more data is requested for tranmission over
    the reverse link.

  hdrrmac4_resume_connection
    Resumes a connection that has been suspended. Turn PA, request data.

  hdrrmac4_shutdown
    Give HDRIND_RTCMAC_SHUTDOWN_COMPLETE indication at the completion of
    the current frame, then cease data transmission (no more data is
    requested from PCP).

  hdrrmac4_process_cmd
    Process Forward Reverse Channel MAC commands from the task command queue.

  hdrrmac4_process_sig
    Process Forward Reverse Channel MAC signals received at task level.

  hdrrmac4_process_msg
    Process received Reverse Traffic Channel MAC messages.

  hdrrmac4_msg_cb
    Callback function that queues received Reverse Traffic Channel MAC
    messages onto the task message queue.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating
  the protocol.

  Copyright (c) 2005 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/src/hdrrmac4.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/02/15   wsh     Fallback to TA boost for tuneaway when RTT boost is enabled 
11/19/14   wsh     Disallow DTX if TxPwr is too high 
11/06/14   wsh     RTT:Fixed incorrectly reporting boosted T2P for RRI > 6 
10/14/14   wsh     RTT:Revert to prefer LoLat for RRI12, only boost HiCap
10/03/14   wsh     RTT not turned on on single carrier RevB traffic
08/28/14   wsh     RTT:Updated RTT threshold, used seperate hysteresis flags
08/12/14   tnp     Fix compiler warnings
08/12/14   wsh     Enable RTT boost feature by default
08/12/14   wsh     Disable RTT in TAP call; force HiCap if boost, new threshold
07/29/14   tnp     FR22375: Fix Medium compiler warnings in HDR
06/11/14   rmv     Handle FTCMAC supervision in RMAC to stop processing  packets
                   from Tx queues 
04/09/14   rmv     FR16966 - Exclude Permitted Payload constraint from 
                   Estimated RL rate algorithm 
02/03/14   rmv     Skip RTC Ack processing if it is for a disabled carrier
11/06/13   arm     Enabled tDTX only for single revb or funnel mode.
11/06/13   arm     Enabled tDTX once connection setup is complete. 
06/04/14   arm     Changes to support DO to G QTA
05/14/14   sat     Fixed Klocwork errors
04/22/14   rmv     FR16966 - For RL rate estimate mark flows as non-empty
                   only when there are ZERO non-empty flows. 
04/03/14   kss/wsh Added T2P boost for RTT reduction
04/17/14   kss/wsh Tuneaway/tuneback T2P boost tweaking  
04/03/14   wsh     Added more F3 for T2P boost feature  
03/20/14   rmv     FR16966 - Increased precision for RL rate calculations 
03/21/14   rmv     FR16966 - Keep RL Estimate algorithm running when there is
                   no data on any of the flows. 
03/14/14   rmv     Added sanity check for the RL rate estimate 
02/27/14   arm     Fixed missing sDTX fl_active member in DTX log packet.                   
12/10/13   kss     Added Rev B T2P Boost support.
11/29/13   vke     Fixed compilation error in hdrrmac4_flexconn_dtx_monitor
11/27/13   arm     Consolidated Flexconn, sDTX, tDTX and nDTX into one log
                   packet and removed FW specific values.
12/16/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
11/26/13   rmg     Supported enabling carriers in FW without a frame delay.
11/25/13   rmg     Added changes to reduce idle subframes before tuneaway.
10/11/13   rmv     Corrected the check for number of channels 
10/02/13   rmg     Added support for RL rate estimate and RLP queue info for
                   modem API.
09/16/13   vke     Fix to prevent the bucket level to become zero 
                   after tune back
09/04/13   rmg     Fixed issue with RTCAck arrival on disabled carriers.
09/04/13   rmv     Fixed KW issue - Added HDR_ASSERT for number of channels
08/28/13   rmg     Fixed a calculation error in T2P decay calculation.
08/08/13   arm     Fixed bolt compiler errors.
08/01/13   smd     Used new secapi to get random number.
07/17/13   arm     Enabled device only dtx in DSDA depending on band 
                   combinations. 
07/10/13   arm     Supported traffic DTX.
06/28/13   rmg     Added change to skip power differential carrier drop 
                   processing unless the dropped carrier is in open state.
06/07/13   rmg     Fixed issue where multiple RCCDs are sent when carriers in
                   setup state are dropped due to PA headroom.
06/03/13   rmg     Added MCDO connected setup enhancement support.
05/15/13   rmv     Added updates for Cost of Modem Power API support 
03/06/13   kss     Do not process drc supervision expired if carrier is 
                   scheduled to be disabled.
03/26/13   smd     Checked whether to stop fc/dtx monitoring before calling 
                   hdrrmac4drv_enter_drc_supervision. 
02/25/13   dsp     Added a check to prevent double initialization 
                   of a critical section in powerup_init().
02/20/13   smd     Handled the case of HHO during DRC supervision for DTX/FC.
02/11/13   smd     Added new fields in DTX and FlexConn log packets.
01/25/13   smd     Removed hdrrmac4_all_carriers_exit_drc_supervision(). 
01/18/13   smd     Removed the check for calling rex_init_crit_sect()
01/15/13   arm     Added changes for rmac tuneaway optimization. 
01/08/13   dsp     Added a check to prevent multiple  calls to rex_init_crit_sect(). 
12/09/12   smd     Used AuxDRCCover to detect CSM6850 for DTX.
08/31/12   vke     Logging fix for Max-T2P value
12/04/12   kss     Check for carrier active before acting on drc supervision 
                   indication. 
12/19/12   vko     Fix Klocwork issues
11/27/12   smd     Added log packets for FlexConn and DTX.
11/08/12   smd     Get random data packet in FTM mode if tx pattern is 0. 
11/01/12   smd     Disabled FlexConn and DTX in TAP call.
                   Added FRAB info in F3 message.
10/10/12   smd     Added fix of enabling FlexConn mode one dsc length after
                   FlexConn being disabled.
                   Added checking FRAB for FlexConn
10/10/12   smd     Fixed fc/dtx control for RevB DRC supervision timeout.
10/03/12   rmg     Corrected tx power initialization during optimized handoff.
09/05/12   smd     Fixes for FlexConn and DTX.
09/12/12   rmg     Corrected T2PInflowRange lookup in away compensate.
08/23/12   smd     Added updates for Device only DTX and FlexCONN.
07/27/12   rmg     Added enhanced PA backoff support.
06/20/12   smd     Supported Device only DTX and FlexCONN.
03/19/12   ljl     Fixed Dime compiler errors.
11/11/11   rmg/vlc Added inclusion of rfm_hdr.h. 
11/07/11   vlc     Fixed Klocwork warnings. 
10/27/11   kss     Added support for LTE to DO optimized handoff.
08/04/11   ssu     Reference ftm_mode using ftm_get_mode().       
08/04/11   ssu     Replace hdr_ftmi_xx with hdrhitmsg_xx using hdrhitmsg APIs. 
07/27/11   cnx     Merged Optimized Handoff changes.
04/26/11   kss     Added checks for KW.
03/01/11   rmg     Added range check for MAC flow id in grant msg processing. 
11/30/10   rmg     Reverted ZI memory reduction changes.
11/16/10   rmg     Skipped open channel count tally when number of open 
                   channels is refreshed from chan_enter_inactive_state().
11/16/10   rmg     Shifted structure of RMAC4 variables out of ZI memory as a
                   workaround for memory corruption issue.
11/01/10   rmg     Updated number of open channels before transitioning them 
                   to inactive state in enter_inactive_state().
10/15/10   rmg     Refresh open channel list when a carrier transitions to 
                   inactive state. 
08/10/10   rmg     Fixed sending multiple duplicate RCCDs in same RL packet.
08/05/10   rmg     Added temporary workaround for Agilent bug that causes
                   multiple phy chans to be mapped to one log chan in TAP mode.
08/02/10   rmg     Fixed skipping of per-flow info in T2P resource log pkt 
                   when all assigned carriers are under DRC supervision.
07/08/10   rmg     Fixed opening a new connection for sending RCCD.
06/25/10   rmg     Initialized physical to logical channel mapping at power up.
06/21/10   cnx     Fixed compiler warnings.
06/09/10   cnx     Fixed Klocwork errors. 
06/03/10   cnx     Fixed Klocwork errors.
05/18/10   rmg     Shifted DRC supervision timer management to FMAC: added 
                   APIs hdrrmac4_enter/exit_drc_supervision().
04/07/10   rmg     Set RMAC3 configuration attributes to default values 
                   during powerup init in FTM mode.
01/26/10   wsh     Merged ZI memory reduction to main branch
12/01/09   rmg     Added support to call a cb once TCA is processed by MC RMAC.
11/18/09   rmg     Changed attribute id of MaxRLPilotTxPwrDiff to 0xFFF4.
11/17/09   rmg     Refresh open chan list when a carrier moves to open state.
09/22/09   rmg     Added support for RL flow control.
09/15/09   kss     Use new PACK() macro for Q6 compatibility.
07/30/09   rmg     Intlocks cleanup.
07/29/09   rmg     Added support to use negotiated MaxRLPilotTxPwrDiff value.
07/29/09   rmg     Cancel RCCD msg repeats after sending carrier request.
07/23/09   rmg     Shifted DRC supervision disable control to RMAC driver.
06/15/09   rmg     Added support for carrier drop due to transmit pilot power
                   differential.
06/15/09   etv     Fixed variable name typo from previous check-in.
06/10/09   rmg     Added check for valid phy chan index in NC attrib update.
06/10/09   etv     Fixed bug with calculating verno index for NC attribs.
05/19/09   etv     Added support for negotiation & GAUP of MaxRLPilotTxPwrDiff
03/24/09   rmg     Shifted subframe number information passed to pcp from 
                   prev_subframe_status() to get_packet().
03/24/09   rmg     Fixed removal of a carrier in SETUP state when it is 
                   removed from a subsequent TCA before RTCAck timer expiry.
03/11/09   kss     Add Rev B support for hdrrmac_get_current_tx_packet_info().
03/02/09   rmg     RMAC Optimization: Changes to make calculation of bucket 
                   factor and T2PUp/Down values more efficient.
12/17/08   kss     Fix priority of a couple F3 messages.
12/13/08   rmg     Fix to disable a carrier once its rev acq timer expires.
12/11/08   kss     Removed FEATURE_HDR_REVB_RMAC_7500.
12/08/08   kss     Couple lint fixes.
12/04/08   rmg     Removed intlocks.
11/24/08   rmg     Lint fixes.
12/03/08   smd     Changed errno to errnum.
11/10/08   smd     Removed off-target build errors. 
10/26/08   rmg     Added support for RevB FTM.
10/06/08   rmg     Fixed data bucket calculation.
08/11/08   etv     Added support for accepting GAUP of RRIChannelPowerParams.
08/22/08   rmg     Fix to prevent scheduling of data on carriers under DRC
                   supervision or with fwd link fingers out of lock.
07/08/08   rmg     Support for checking RLSI/DRC supervision in SETUP state.
06/26/08   rmg     Change to drop carrier after DRC restart timer expiration.
06/26/08   rmg     Support for SLP carrier forced switch upon DRC supervision.
06/12/08   rmg     Shifted carrier bucket initialization from SETUP to 
                   OPEN state.
06/09/08   rmg     Added event to indicate change in RMAC carrier state.
05/27/08   rmg     Change to clear acutal flow allocation for non-transmitting
                   open channels.
05/20/08   rmg     Fixed logging of imminent tuneaway and imminent RLSI in 
                   hdr interrupted reason.
05/19/08   kss     Fixes parsing of multiple RTCAcks.
05/15/08   rmg     Fixed txt2p update for open channels not queuing new data.
04/29/08   rmg     Fixed logging of fields in T2P resource info.
04/22/08   rmg     Fixed multiple channel dropped message transmissions.
04/15/08   rmg     Switched order of sending channel drop and deactivating
                   carrier.
04/10/08   rmg     Fixed carrier drop mechanism.
04/01/08   cc      Log HDRRMAC4DRV_HDR_MODE_TUNE_AWAY_IMMINENT and
                   HDRRMAC4DRV_HDR_MODE_RLSI_IMMINENT HDR inactive
                   reasons.
02/19/08   cc      Fixed the ReverseCDMAChannelDropped message packing bug.
02/15/08   cc      Only process RTCAck in the SETUP state.
02/13/08   rmg     Fixed flow allotment logging.
02/13/08   rmg     Fixed T2POutflow calculation.
02/09/08   cc      TCA carrier add/drop bug fixes.
01/30/08   cc      Clean up the rmac4 packet status reporting.
01/11/08   cc      Pass subframe count information to PCP for PCP logging 
                   purpose.
01/07/08   cc      Set the recommended size to 0 for the flows in the eligible
                   flows set if the max. pkt size is determined to be 0.
01/04/08   cc      Fixed logging of 0 frame build time.
12/26/07   cc      Fixed at$qchdrr support.
12/14/07   cc      Check IsSectorMultiCarrierCapable before sending
                   CarrierRequest Message.
12/13/07   cc      Fixed the bug that a single PilotStrengthPS (instead of
                   per RL) is used to update delta T2P.
12/13/07   cc      Included 2nd interation information in log packet.
12/11/07   cc      Disabled FEATURE_HDR_RMAC4_DEBUG.
12/10/07   cc      Added temporary work-around of incorrect MARQ reporting.
11/30/07   cc      Moved hdrrmac4_common pointer initialization from _init
                   to powerup_init function.
11/30/07   rmg     Temporary fixes for 7800 build.
11/20/07   cc      Removed hho_marq support.
11/16/07   cc      Increased number of outstanding RL Drop messages
                   supported to 6; when run out RL Drop Message Ack timer,
                   used the most aged one; clean up all outstanding RL Drop
                   Message Ack timers when protocol is deactivated; only
                   decrement CarrerRequest variable "C" if the channel
                   de-assigned by TCA is currently in Open state. 
11/09/07   cc      Added HDRPCP_RRI_CANCELED for packet continuation tracking.
11/02/07   cc      Fixed queue length update bug in
                   hdrrmac4_update_active_flows_set_tmp().
11/02/07   cc      Added 3x unit test support on 7500.
10/31/07   cc      MARQ outstanding packets on deleted channels and after HHO.
10/26/07   cc      Changes based Carrier Request code review comments.
10/26/07   cc      Moved hdrrmac4_allot_flow_data() inside the channel iteration
                   loop.
10/25/07   cc      Fixed unit test issue that T2POutflow not updated correctly.
10/19/07   cc      Initial Carrier Request implementation.
10/17/07   cc      Added num_mac_flows_in_pkt & num_app_pkts_in_pkt support.
10/16/07   cc      Renamed normal_hdr_inactive to hdr_interrupted.
10/15/07   rmg     Changes to support get_rl_add_drop_info() interface change.
10/15/07   cc      Renamed num_rlp_pkts_in_pkt to num_app_pkts_in_pkt in 
                   LOG_HDR_RMAC_PKT_TIME_INFO_C log packet.
10/15/07   cc      Added init_attrib argument in powerup_init function.
10/15/07   rmg     Corrected merge errors in get_sector_info() calls.
10/15/07   rmg     Changes to support get_sector_info() interface change.
10/11/07   cc      1. Removed retrieval of InUse attribute values from
                   hdrrmac4_powerup_init(); 2. Set all attributes to 
                   default values on Session Change Indication to support 
                   unit test output; 3. Force max_packet_size to 12288 bits
                   for unit test.
10/09/07   cc      Only include channels with packet built when calling
                   hdrrmac4drv_send_packet().
10/06/07   cc      Added single carrier unit test support; fixed bucket
                   factor computation.
10/03/07   rmg     Added NEG_INFINITY_T2P support.
09/28/07   cc      Added "normal_hdr_inactive" and "enc_status" logging support.
09/28/07   cc      Fixed some macros to use upper case consistently.
09/24/07   cc      Added support for LOG_HDR_REVB_RMAC4_T2P_RESOURCE_C &
                   LOG_HDR_REVB_RMAC4_PKT_INFO_C, and 
                   LOG_HDR_RMAC_PKT_TIME_INFO_C log packets
09/24/07   rmg     Added hdr_inactive_reason support.
09/20/07   rmg     Fixed circular inclusions.
09/19/07   cc      Replaced HDRIND_RMAC3_... indications by HDRIND_RMAC_....
08/15/07   cc      DOrB updates.  RMAC4 Phase 2.0 development.
08/08/07   cc      DOrB updates; running rmac4 over rmac3drv.
07/30/07   cc      Renamed HDRSCMRMAC4_MAX_NUM_REV_CHANNELS to
                   HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS.
07/24/07   cc      Temporarily include hdrrmac3.h to resolve the compilation
                   error.
07/17/07   cc      Code review comments.
07/12/07   cc      Added hdrrmac_update_rl_carriers() & 
                   hdrrmac_get_slp_carrier_and_tx_pilot_pwr_info support for
                   MSM7500 Rev. B integration.
07/02/07   cc      Initial creation.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "customer.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_HDR_REVB
/* General include files */
#include <memory.h>
#include "comdef.h"
#include "dsm.h"
#include "err.h"
#include "hdrdebug.h"
#include "qw.h"
#include "rex.h"
#include "ran.h"
#include "rf.h"
#include "timetick.h"

/* HDR protocol-related include files */
#include "hdrhai.h"
#include "hdramac.h"
#include "hdrhmp.h"
#include "hdrind.h"
#include "hdrpcp.h"
#include "hdrscp.h"
#include "hdrovhd.h"
#include "hdramp.h"
#include "hdrutil.h"
#include "hdrmci.h"
#include "hdrrup.h"
#include "hdrrmac4.h"
#include "hdrrmac4i.h"
#include "hdrrmacmath.h"
#include "hdrrmac4drv.h"
#include "hdrfmac.h"
#include "hdrdsm.h"
#include "hdrts.h"

/* Other HDR-specific include files */
#include "hdrbit.h"
#include "hdrmod.h"
#include "hdrenc.h"
#include "hdrlogi.h"
#include "hdrlog.h"
#include "hdrtxi.h"
#include "hdrrx.h"
#include "hdrtrace.h"
#include "hdrmdspmcr.h"
#include "hdrmdsp.h"
#include "hdrsrch.h"
#include "hdrrmaclog.h"
#include "hdrslpi.h"

#ifdef FEATURE_HDR_IS890A
#include "hdrrtap.h"
#endif

#include "hdrrmaci.h"
#include "hdrscmrmac4.h"
#include "hdrscmdb.h"
#include "hdrrmaccmni.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "hdrhitmsg.h"
#include "ftm.h"
#endif

#include "rfm_hdr.h"
#ifdef FEATURE_HDR_MODEM_API
#include "hdrsrchrf.h"
#endif /* FEATURE_HDR_MODEM_API */


/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                            PROTOCOL TASK CONTEXT

  The RTC MAC protocol is executed within the context of the
  following task and the following task signals are assigned to the Control
  Channel MAC protocol.  The following assignments are specific to the task
  within which the protocol executes.
---------------------------------------------------------------------------*/
#include "hdrtx.h"
#include "task.h"

#ifdef FEATURE_FACTORY_TESTMODE
byte          hdrrmac4_phy_chan_idx_revb[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
              { 0, 1, 2 };
#define MAX_REVA_PAYLOAD_SIZE_BYTES  1532
#endif

//#undef FEATURE_HDR_RMAC4_DEBUG
//#define FEATURE_HDR_RMAC4_DEBUG
/* Define this to do unit test */
#undef FEATURE_HDR_RMAC4_UNIT_TEST
/* Define this and RMAC4_UNIT_TEST to do 3 carriers unit test on 7500 */
#undef FEATURE_HDR_RMAC4_3X_UNIT_TEST

/* 
 * Define this to nothing 
 *    #define RMAC4_LOCAL
 * to externalize static functions, or when seeing debugger symbol misaligned.
 */
//#define RMAC4_LOCAL static
#define RMAC4_LOCAL

#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
#error code not present
#endif



/*---------------------------------------------------------------------------
                             PROTOCOL TIMERS

  The Setup timer is used to abort the setup process if an acknowledgement
  is not received from the network within the required amount of time. 

  Note:  The Setup Timer is not implemented since there is a duplicate timer
         in the Route Update protocol that will *always* expire before this
         timer could expire.  Since the expiration of this upper layer timer
         causes this protocol's timer to reset, this timer is effictively
         not used.  The upper layer timer used is the T_RUPConnectionSetup
         timer in the Route Update State protocol's connection setup state.
---------------------------------------------------------------------------*/

#define HDRRMAC4_SETUP_TIMER_MSECS            1500  /* 1.5s T_RTCMPATSetup*/

/* ReverseAcquisition Timer */
RMAC4_LOCAL rex_timer_type 
  hdrrmac4_rl_acq_timer[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

#define HDRRMAC4_RL_DROP_ACK_TIMER_MSECS      250   /* msec */

#define HDRRMAC4_MAX_RL_DROP_MSG_ATTEMPTS     3     /* N_SLPAttempt */

/* Enumeration for drop reason code*/
typedef enum
{
  HDRRMAC4_RL_DROP_REASON_HEADROOM_LIMITATION = 0,  /* Headroom Limitation */
  HDRRMAC4_RL_DROP_REASON_FTC_DRC_SUPERVISION = 1,  /* FTC DRC Supervision */
  HDRRMAC4_RL_DROP_REASON_RL_ACQ_TMR_EXPIRED  = 2,  /* RL ACQ Timer Expiration */
  HDRRMAC4_RL_DROP_REASON_TXP_DIFF            = 3   /* RL transmit pilot power
                                                       differential */
} hdrrmac4_rl_drop_reason_code_e_type;

typedef struct
{
  boolean                             in_use;
    /* TRUE if this entry is currently is in use, await pending ack */

  uint8                               num_attempts;
    /* Number of attempts the same message has been transmitted */

  struct
  {
    uint8                                 trans_id;
      /* Transaction ID */

    uint8                                 num_rls;
      /* Number of Reverse Channels, 1 less than the actual */

    struct
    {
      sys_channel_type                    chan;
      hdrrmac4_rl_drop_reason_code_e_type reason_code;
    } chan_record[HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];

  } rl_dropped_msg;

  rex_timer_type                      rl_drop_ack_timer;
    /* ReverseCDMAChannelDroppedAck Timer */

  boolean                             msg_send_status_arrived;
    /* TRUE if accept/reject status on the last queued RL drop message has
       arrived */

} hdrrmac4_rl_drop_msg_type;

/* Number of outstanding RL Drop messages supported. */
#define HDRRMAC4_MAX_NUM_DROP_MSGS  6

static struct
{
  uint8 trans_id;
    /* Transaction ID of the last ReverseCDMAChannelDropped Message */

  hdrrmac4_rl_drop_msg_type rl_drop_msg[HDRRMAC4_MAX_NUM_DROP_MSGS];
    /* control and message structure for every ReverseCDMAChannelDropped Message */
} hdrrmac4_rl_drop_var;

typedef struct
{
  uint8                               phy_chan_idx;
    /* ID of physical channel to be dropped */

  hdrrmac4_rl_drop_reason_code_e_type reason_code;
    /* reason code */

} hdrrmac4_drop_info_type;

/* Enumeration of timers processed by RMAC4. */
typedef enum
{
  HDRRMAC4_TIMER_RL_ACQ      = 0, /* Reverse acquisition timer */
  HDRRMAC4_TIMER_RL_DROP_ACK = 1, /* RCCD Ack timer */
  HDRRMAC4_TIMER_MAX         = 2  /* Number of RMAC4 timers */
} hdrrmac4_timer_enum_type;

/* RTCAck message. */
#define HDRRMAC4_RTCACK_MSG_ID                0x00

/* Request message (not supported). */
#define HDRRMAC4_REQUEST_MSG_ID               0x02

/* Grant message. */
#define HDRRMAC4_GRANT_MSG_ID                 0x03

/* ReverseCDMAChannelDropped message. */
#define HDRRMAC4_RL_DROP_MSG_ID               0x05
#define HDRRMAC4_RL_DROP_MSG_PRI              30

/* ReverseCDMAChannelDroppedAck message. */
#define HDRRMAC4_RL_DROP_ACK_MSG_ID           0x06

#ifdef FEATURE_LTE_TO_HDR_OH
/* HRPDOpenLoopParameters message. */
#define HDRRMAC4_HRPD_OPEN_LOOP_PARAMS_MSG_ID    0x07
#endif /* FEATURE_LTE_TO_HDR_OH */

/* CarrierRequest message. */
#define HDRRMAC4_CARRIER_REQ_MSG_ID           0x04
#define HDRRMAC4_CARRIER_REQ_MSG_PRI          50

/* BroadcastReverseRateLimit message. */
#define HDRRMAC0_BROADCAST_REVERSE_RATE_LIMIT_MSG_ID    0x01

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
/* Index to force hdrrmac3_tx_t2p_no_tx_filter_tc
   to 256 frames. */
#define HDRRMAC4_T2P_NO_TX_FILTER_TC_IDX      5
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

/*---------------------------------------------------------------------------
                             PROTOCOL VARIABLES
---------------------------------------------------------------------------*/
/* ========================================================================== */
/* =====================RTCMAC Subtype 4 Data Structures===================== */
/* ========================================================================== */

/* Macro to check if given MAC flow is active */
#define HDRRMAC4_IS_FLOW_ACTIVE( f )\
          ( hdrrmac4_config_ptr->flow[(f)].bucket_level_max > 0 )

/* Macro to check if given MAC flow's QRAB Select switch is set to QRAB_PS */
#define HDRRMAC4_QRAB_SEL_IS_QRAB_PS( f )\
          (hdrrmac4_config_ptr->flow[(f)].qrab_select == \
           HDRRMACCMN_QRAB_VAL_QRAB_PS)

/* Macro to check if given MAC flow's QRAB Select switch is set to QRAB_PS */
#define HDRRMAC4_QRAB_SEL_IS_QRAB( f )\
          (hdrrmac4_config_ptr->flow[(f)].qrab_select == \
           HDRRMACCMN_QRAB_VAL_QRAB)

/* Macro to check if given MAC flow has been newly activated */
#define HDRRMAC4_IS_MAC_FLOW_NEW( f )\
          (hdrrmac4_var_ptr->data_flow[(f)].bucket.last_bucket_level_max == 0)\
          && (hdrrmac4_config_ptr->flow[(f)].bucket_level_max != 0)

/* Macro to determine for the QRAB for given flow is LOADED or UNLOADED */
#define HDRRMAC4_FLOW_QRAB_LOADED( c, f )\
          (hdrrmac4_var_ptr->phy_chan[(c)].flow[(f)].qrab_flow == HDRRMAC4DRV_QRAB_LOADED)

/* Macro to determine if a physical channel is in OPEN state */
#define HDRRMAC4_IS_RL_OPEN( c )\
          (hdrrmac4_var_ptr->phy_chan[(c)].chan_state == HDRRMAC4_CHAN_OPEN_STATE)

/* Macro to determine if a physical channel is to be deleted */
#define HDRRMAC4_IS_RL_TO_BE_DELETED( c )\
          ((hdrrmac4_var_ptr->phy_chan[(c)].chan_state ==     \
            HDRRMAC4_CHAN_DELETE_STATE) ||                    \
           (hdrrmac4_var_ptr->phy_chan[(c)].chan_state ==     \
            HDRRMAC4_CHAN_ADD_DELETE_STATE))

/* Macro to determine if a MARQ is to be reported for ongoing transmissions on
   a physical channel. The flag marq_outstanding_pkts is currently set for a
   channel when -
   - the channel is put in DELETE state
   - the channel is dropped due to transmit pilot power differential */
#define HDRRMAC4_IS_REPORT_MARQ( c )\
          (hdrrmac4_var_ptr->phy_chan[(c)].marq_outstanding_pkts == TRUE) 

/* Macro to determine if a physical channel is not in INACTIVE state */
#define HDRRMAC4_IS_RL_NOT_INACTIVE( c )                      \
          (hdrrmac4_var_ptr->phy_chan[(c)].chan_state !=      \
           HDRRMAC4_CHAN_INACTIVE_STATE)

/* The following struct is used to keep all RMAC4 logging info          */
typedef struct
{
  uint8                             sub_frame_count;
    /* sub-frame counter */
  
  uint16                            cand_ps_set[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* final candidate packet size set */

  int16                             rand_num[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* random number - used in t2p_dither_term calculation */

  PACK(struct)
  {
    uint8   final_ps_index     : 4;
      /* final packet size decided by RTC MAC */
    uint8   tm                 : 1;
      /* transmission mode ( HC/LL ) */
    uint8   reserved           : 3;
  } ps_info[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  uint16 tx_t2p_max[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* maximum transmitt T2P */

  uint16 round_down_quantum[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* minimum unit of allocation used in round down = max pkt size/loop cnt */

  uint8  num_pkts_in_mac_pkt[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* 
     * Number of Application packets contained in this MAC packet built for 
     * this carrier.
     */

  uint8  num_mac_flows_with_data[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /*
     * Number of MAC flows whose data is contained in the MAC packet built for
     * this carrier.
     */

  boolean prev_pkt_accepted[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* if prev pkt. was accepted? */

  boolean two_iterations;
    /* TRUE if 2nd interation is executed */

} hdrrmac4_logging_struct_type;

/* HDR RMAC4 logging structure  */
RMAC4_LOCAL hdrrmac4_logging_struct_type  hdrrmac4_logging;

/* Pointer to local Configuration data in OTA format as defined by SCM */
RMAC4_LOCAL hdrscmrmac4_config_struct_type *hdrrmac4_scm_cfg_ptr;

/* Pointer to Local configuration data initialized */
RMAC4_LOCAL hdrrmac4_config_struct_type *hdrrmac4_config_ptr;

/* Local RTCMAC4 object */
RMAC4_LOCAL hdrrmac4_vars_struct_type   *hdrrmac4_var_ptr;

/* Make pointed to const */
RMAC4_LOCAL hdrrmac_common_type * hdrrmac4_common;

/* Set of active MAC flows */
RMAC4_LOCAL hdrpcp_mac_flow_allocation_type 
  hdrrmac4_active_flows_set[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

/* Temporary working copy, updated after each channel allocation */
RMAC4_LOCAL hdrpcp_mac_flow_allocation_type 
  hdrrmac4_active_flows_set_tmp[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

RMAC4_LOCAL uint8 hdrrmac4_num_active_flows;

/* Set of active MAC flows with non-empty queues */
RMAC4_LOCAL hdrpcp_mac_flow_allocation_type 
  hdrrmac4_non_empty_flows_set[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]
                              [HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

RMAC4_LOCAL uint8 hdrrmac4_num_non_empty_flows[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
 
/* Set of active MAC flows eligible for transmission */
RMAC4_LOCAL hdrpcp_mac_flow_allocation_type 
  hdrrmac4_elig_flows_set[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]
                         [HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

RMAC4_LOCAL uint8 hdrrmac4_num_elig_flows[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

/* History of RMAC4 decisions. 3 interlaces * 4 transmissions = 12 plus 1 
 * because we load a packet 1 subframe in advance. TODO optimize size?
 * #define HDRRMAC4_HIST_SIZE ( ( HDRRMAC4_MAX_NUM_SUBPACKETS\
 *                              * HDRRMAC4_NUM_INTERLACES ) + 1 )
 */

#define HDRRMAC4_HIST_SIZE 75 /* to match the range of tx_pkt_id returned by
                               * lower layer TODO cut the size; 75 >> needed
                               */
RMAC4_LOCAL hdrrmaccmn_decision_hist_struct_type 
        hdrrmac4_hist[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS][ HDRRMAC4_HIST_SIZE ];

/*
 * Number of iterations to run the algorithm to improve Eb/No efficiency
 */
#define HDRRMAC4_MAX_PS_ITERATIONS  2

/* 
 * Contains a list of phy_chan_id of the channels that are in the OPEN state.
 * This is the list that the RMAC4 per channel algorithm will be operate on.
 */
RMAC4_LOCAL uint8 hdrrmac4_open_rl_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

/* 
 * Contains a list of phy_chan_id sorted according to each channel's RTCMAC
 * priority.  The first channel has the highest priority and the packet 
 * allocation is to be done first.  The last channel has the lowest priority.
 */
RMAC4_LOCAL uint8 hdrrmac4_ranked_rl_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

/* 
 * Contains a list of phy_chan_id sorted according to each channel's CAPUT
 * priority. The first channel has the highest priority and it will be occupy
 * the first entry in the channel list presented to PCP, so the packet with
 * the lowest RLP sequence number will be sent on that channel.  The last 
 * channel has the lowest priority.
 */
RMAC4_LOCAL uint8 hdrrmac4_caput_rl_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

/* combo structure used with hdrpcp_get_packet() interface */
RMAC4_LOCAL hdrpcp_build_data_per_combo_type hdrrmac4_combo;

static struct
{
  hdrrmaccmn_tx_mode_enum_type  tm;               /* transmission mode     */
  uint8                         tt;               /* termination target    */
  uint16                        t2p_conv_factor;  /* T2P conversion factor */
  int32                         bucket_level[ HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS ];
                                                  /* current bucket level  */
  uint32                        t2p_inflow[ HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS ];
                                                  /* current t2p inflow    */
  uint32                        max_tx_t2p;       /* Max T2P supported for 
                                                     next subpacket in dB  */
  rex_crit_sect_type            mac_info_crit_sect;
                      /* Critical Section for mac_info_raw_data management */
} mac_info_raw_data;

/* This variable is FALSE if we have either tuned away from HDR or RLSI is in
 * effect
 */
RMAC4_LOCAL boolean hdrrmac4_hdr_mode_active = TRUE;

/* Constants used with the hdrpcp_report_prev_subframe_status() &
   hdrpcp_get_packet() */
#define HDRRMAC4_DEFAULT_COMBO_ID     0
#define HDRRMAC4_DEFAULT_NUM_COMBOS   1

/* structure to interface with hdrrmac4drv_get_prev_tx_status() */
RMAC4_LOCAL hdrrmac4drv_tx_status_type rmac4_pkt_status;

/* Fixed packet size used for testing only. When it is set to 0xF normal
 * packet size selection applies. 
 */
RMAC4_LOCAL hdrrmaccmn_payload_size_enum_type hdrrmac4_test_ps_index_fixed; 

/* This variable is set to TRUE by the following functions when the 
   current SLP carrier needs to be force switched:
   ~hdrrmac4_tca_update() - when the current SLP carrier is removed by TCA
   ~hdrrmac4drv_check_drc_supervision() - when the current SLP carrier is
    under DRC supervision */
boolean hdrrmac4_force_slp_switch = FALSE;

/* T2P-FRAB grid struct definition */
typedef struct
{
  hdrrmacmath_t2p_axis_struct_type  t2p_axis;
  hdrrmacmath_frab_axis_struct_type frab_axis;
  int32                             t2p_frab_grid[HDRRMACMATH_MAX_T2P_PTS]
                                                 [HDRRMACMATH_MAX_FRAB_PTS];
} hdrrmac4_t2p_frab_grid_type;


static struct
{
  /* BucketFactor grid */
  hdrrmac4_t2p_frab_grid_type bucket_factor[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

  /* T2PUp grid */
  hdrrmac4_t2p_frab_grid_type t2p_up[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS]
    [HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* T2PDown grid */
  hdrrmac4_t2p_frab_grid_type t2p_down[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS]
    [HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

} hdrrmac4_t2p_frab_grid_data;

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)||\
    defined ( FEATURE_HDR_TRAFFIC_DTX)
/* Info for flexconn log packet */
struct
{

  LOG_HDR_MC_DTX_INFO_C_type    log;           /* Log data                */
  uint8                        sf_count;           /* Current subframe count  */
  boolean                      enabled;         /* Logging enabled flag    */
} hdrrmac4_dtx_log_info;
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX || FEATURE_HDR_FLEX_CONN 
          || FEATURE_HDR_TRAFFIC_DTX*/
/* TRUE if one-time rex init has been done */
RMAC4_LOCAL boolean hdrrmac4_rex_init_is_called = FALSE; 

/*---------------------------------------------------------------------------
                   SCM RELATED DEFINITIONS AND DECLARATIONS
---------------------------------------------------------------------------*/

RMAC4_LOCAL uint8 hdrrmac4_non_ncx_gaup_verno
                [HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS ];

RMAC4_LOCAL uint8 hdrrmac4_nn_gaup_verno
                [(HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS * 
                  HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS)];

RMAC4_LOCAL uint8 hdrrmac4_cc_gaup_verno
                [(HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS *
                  HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS)];

RMAC4_LOCAL uint8 hdrrmac4_xx_gaup_verno
                [(HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS *
                  HDRSCMRMAC4_MAX_NUM_CONFIGS)];

RMAC4_LOCAL uint8 hdrrmac4_nc_gaup_verno
                [(HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS *
                  HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS *
                  HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS)];

/* Pointer to each one of the non-per-flow GAUPable attribute in the SCM config 
   struct */
RMAC4_LOCAL void* hdrrmac4_non_ncx_gaup_attribs
              [HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS];

/* Pointer to each one of the per-flow GAUPable attribute in the SCM config 
   struct */
RMAC4_LOCAL void* hdrrmac4_nn_gaup_attribs
  [HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS][HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

/* Pointer to each one of the per-Reverse-Logical-Channel attribute in the
   SCM config struct */
RMAC4_LOCAL void* hdrrmac4_cc_gaup_attribs
  [HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS][HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];

/* Pointer to each one of the per-Configuration attribute in the SCM config
   struct */
RMAC4_LOCAL void* hdrrmac4_xx_gaup_attribs
  [HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS][HDRSCMRMAC4_MAX_NUM_CONFIGS];

/* Pointer to each one of the per-Flow-per-Reverse-Logical-Channel attribute
   in the SCM config struct */
RMAC4_LOCAL void* hdrrmac4_nc_gaup_attribs
  [HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS]
  [HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS]
  [HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];

/* Attribute IDs of Non-NCX GAUPable Attributes */
RMAC4_LOCAL uint16 const hdrrmac4_gaup_attrib_ids
                      [HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS] = 
{
  /* Simple Attributes */
  0xFFF4, /* 0.  MaxRLPilotTxPwrDiff*/
  0xFFFB, /* 1.  PayloadThresh */
  0xFFFC, /* 2.  MergeThreshold */

  /* Complex Attributes */
  0x0000, /* 3.  AuxiliaryPilotChannelParameters */
  0x0003, /* 4.  PermittedPayload */
  0x0005, /* 5.  PowerParameters128 */
  0x0006, /* 6.  PowerParameters256 */
  0x0007, /* 7.  PowerParameters512 */
  0x0008, /* 8.  PowerParameters768 */
  0x0009, /* 9.  PowerParameters1024 */
  0x000A, /* 10.  PowerParameters1536 */
  0x000B, /* 11. PowerParameters2048 */
  0x000C, /* 12. PowerParameters3072 */
  0x000D, /* 13. PowerParameters4096 */
  0x000E, /* 14. PowerParameters6144 */
  0x000F, /* 15. PowerParameters8192 */
  0x0010, /* 16. PowerParameters12288 */
  0x0011, /* 17. RequestParameters */
  0x0012  /* 18. RRIChannelPowerParameters */
};

/* 
 * The value of this #define has to match with the index of the 
 * AssociatedFlows00 attribute enty in the hdrrmac4_gaup_nn_attrib_ids below.
 */
#define HDRRMAC4_ASSOCIATED_FLOWS_ATTRIB_OFFSET 6

/* Attribute IDs of Per-Flow GAUPable attributes */
RMAC4_LOCAL uint16 const hdrrmac4_gaup_nn_attrib_ids
                      [HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS] = 
{
  /* Per-Flow Simple Attributes */
  0xF900, /* 0, T2PFilterTC00 */
  0xFA00, /* 1, BurstDurationFactor00 */
  0xFB00, /* 2, QRABSelect00 */
  0xFC00, /* 3, TransmissionMode00 */ 
  0xFD00, /* 4, MergeThreshold00 */
  0xFE00, /* 5, BucketLevelMax00 */

  /* Per-Flow Complex Attributes */
  0x0100, /* 6, AssociatedFlows00, need to match with the #define above */
  0x0300, /* 7, BucketFactor00 */
  0x1900  /* 8, DataTokenBucket00 */
};

/* Attribute IDs of Per-Reversr-Logical-Channel GAUPable attributes */
RMAC4_LOCAL uint16 const hdrrmac4_gaup_cc_attrib_ids
                      [HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS] = 
{
  /* Per-Channel Simple Attributes */
  0xF600, /* 0, PilotStrengthQRABThresholdDRCUnlock00 */
  0xF700, /* 1, PilotStrengthQRABThresholdDRCLock00 */
  0xF800, /* 2, ARQMode00 */

  /* Per-Channel Complex Attributes */
  0x1800  /* 3, AssociatedATConfigurations00 */
};

/* Attribute IDs of Per-Configuration GAUPable attributes */
RMAC4_LOCAL uint16 const hdrrmac4_gaup_xx_attrib_ids
                      [HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS] = 
{
  /* Per-Configuration Complex Attributes */
  0x1500, /* 0, T2PInflowRangeConfig00 */
  0x1600, /* 1, T2PTransitionFunctionConfig00 */
  0x1B00, /* 2, PilotStrengthConfig00 */
  0x1C00  /* 3, TxT2PMaxConfig00 */
};

/* Attribute IDs of Per-Flow-Per-Reversr-Logical-Channel GAUPable attributes */
RMAC4_LOCAL uint16 const hdrrmac4_gaup_nc_attrib_ids
                      [HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS] = 
{
  /* Per-Flow-Per-Channel Complex Attributes */
  0x1700  /* 0, AssociatedFlowConfigurations00 */
};

/* Indices of non-per-flow GAUPable attributes in the uint8 arrays 
   hdrscmrmac4_verno_type.non_nn_simple & 
   hdrscmrmac4_verno_type.non_nn_complex respectively.  This table 
   is initialized on power up by using the SCM interface. */
RMAC4_LOCAL uint16 hdrrmac4_gaup_non_ncx_attrib_verno_indices
                [HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS];

/* Indices of per-flow GAUPable attributes in the uint8 arrays 
   This table is initialized on power up by using the SCM interface. */
RMAC4_LOCAL uint16 hdrrmac4_gaup_nn_attrib_verno_indices
                [HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS];

/* Indices of per-Reverse-Logical-Channel GAUPable attributes in the uint8 
   arrays.
   This table is initialized on power up by using the SCM interface. */
RMAC4_LOCAL uint16 hdrrmac4_gaup_cc_attrib_verno_indices
                [HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS];

/* Indices of per-Configuration GAUPable attributes in the uint8 arrays.
   This table is initialized on power up by using the SCM interface. */
RMAC4_LOCAL uint16 hdrrmac4_gaup_xx_attrib_verno_indices
                [HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS];

/* Indices of per-NC GAUPable attributes in the uint8 arrays.
   This table is initialized on power up by using the SCM interface. */
RMAC4_LOCAL uint16 hdrrmac4_gaup_nc_attrib_verno_indices
                      [HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS];

RMAC4_LOCAL hdrrmaccmn_payload_size_enum_type const 
  hdrrmac4_max_packet_size_lut[5] =
{
  HDRRMACCMN_1024_BITS,
  HDRRMACCMN_2048_BITS,
  HDRRMACCMN_4096_BITS,
  HDRRMACCMN_8192_BITS,
  HDRRMACCMN_12288_BITS
};

/*---------------------------------------------------------------------------
               CARRIER REQUEST RELATED DEFINITIONS AND DECLARATIONS
---------------------------------------------------------------------------*/
/* 
 * If a CarrierDrop is sent after a CarrierRequest within the duration of
 * HDRRMAC4_CARRIER_REQ_DEFAULT_AGGRESSIVE_DROP_TMO_MS, AT is no 
 * "aggressive drop" mode.  In unit of ms.
 */
#define HDRRMAC4_CARRIER_REQ_DEFAULT_AGGRESSIVE_DROP_TMO_MS 2000

/*
 * If AT is in "aggressive drop" mode, the CarrierRequest cannot be sent
 * during HDRRMAC4_CARRIER_REQ_DEFAULT_BACKOFF_TMO_MS period after the previous
 * CarrierDrop.  In unit of ms.
 */
#define HDRRMAC4_CARRIER_REQ_DEFAULT_BACKOFF_TMO_MS         5000

/* Macro to determine if number of carriers supported by PA headroom increased? */
#define HDRRMAC4_CARRIER_REQ_NUM_CHANS_PA_HEADROOM_INCREASED()  \
  ((hdrrmac4_carrier_req_var.dropped &&                         \
   (hdrrmac4_carrier_req_var.num_chans_pa_headroom >            \
    hdrrmac4_carrier_req_var.num_chans_req)) ? 1 : 0 )

/* Macro to update the number of channels PA headroom can support */
#define HDRRMAC4_CARRIER_REQ_UPDATE_NUM_CHANS_PA_HEADROOM(num_chans_curr_pa_headroom)     \
  (hdrrmac4_carrier_req_var.num_chans_pa_headroom = (num_chans_curr_pa_headroom))

/* Variables controlling the Carrier Request Functionality */
typedef struct
{
  uint8   num_chans_allocated;
    /* 
     * "C"- Current number of carriers allocated to the AT when in a 
     *      connection.
     */

  uint8   num_chans_pa_headroom;
    /* 
     * "P"- Number of carriers for which the AT currently has PA headroom 
     *      available.
     */

  uint8   num_chans_req;
    /*
     * "R"- Number of carriers that the AT indicated in the CarrierRequest
     *      Message sent to the AN.
     */

  boolean dropped;
    /*
     * "D"- A flag to indicate if the AT dropped one or more carrier due to
     *      PA headroom conditions.
     */

  boolean aggressive_drop;
    /*
     * Flag to indicate if the mobile has currently detected an aggressive
     * drop.
     */

  boolean sent_carrier_req;
    /*
     * If a CarrierRequest message was sent previously?
     */

  boolean sent_carrier_drop;
    /*
     * If a CarrierDropped message was sent previously?
     */

  timetick_type last_carrier_req_time;
    /*
     * The time when the last CarrierRequest message was sent.  In unit of ms.
     * Only interpret this value when sent_carrier_req is TRUE.
     */

  timetick_type last_carrier_drop_time;
    /*
     * The time when the last CarrierDrop message was sent.  In unit of ms.
     * Only interpret this value when sent_carrier_drop is TRUE.
     */

  timetick_type aggressive_drop_detect_timer;
    /*
     * "T1"- Minimum time between sending of CarrierRequest requesting for 
     *       additional carriers and the subsequent additional carrier 
     *       allocation resulting in a ReverseCDMAChannelDropped. If one 
     *       or more carriers are dropped prior to this timer expiration,
     *       the AT considers this to be an aggressive drop and initiates
     *       a backoff for the next CarrierRequest message sent by the 
     *       mobile. Default value of T1 = 2 secs.
     * In unit of ms.
     */

  timetick_type carrier_req_backoff_timer;
    /* 
     * "T2"- The backoff timer used by the AT when an aggressive 
     *       ReverseCDMAChannelDropped message is detected for the next 
     *       CarrierRequest message sent by the mobile. This is the minimum 
     *       timer after the mobile has sent a ReverseCDMAChannelDropped 
     *       message that is considered an aggressive drop before the mobile 
     *       is allowed to send a CarrierRequest message. Default value of 
     *       T2 = 5 secs.
     * In unit of ms.
     */
} hdrrmac4_carrier_req_var_type;

RMAC4_LOCAL hdrrmac4_carrier_req_var_type hdrrmac4_carrier_req_var;

/* Event types as input to Carrier Request Function */
typedef enum
{
  HDRRMAC4_CARRIER_REQ_EVENT_INIT                 = 0,
    /* when connection first opens */

  HDRRMAC4_CARRIER_REQ_EVENT_RTCACK               = 1,
    /* when a RTCAck is received */

  HDRRMAC4_CARRIER_REQ_EVENT_PA_HEADROOM_DROP     = 2,
    /* when carriers to be dropped due to PA headroom limitation */

  HDRRMAC4_CARRIER_REQ_EVENT_TCA_DROP             = 3,
    /* when a TCA is received to delete carriers */

  HDRRMAC4_CARRIER_REQ_EVENT_DRC_SUPERVISION_DROP = 4,
    /* when carriers to be dropped due to DRC Supervision Timeout */

  HDRRMAC4_CARRIER_REQ_EVENT_PA_HEADROOM_ADD      = 5,
    /* when PA headroom is available for additional carriers */

  HDRRMAC4_CARRIER_REQ_EVENT_TXP_DIFF             = 6
    /* when carriers are to be dropped due to transmit pilot power
       differential */

} hdrrmac4_carrier_req_event_type;



/*---------------------------------------------------------------------------
                             INTERNAL FUNCTION PROTOTYPES
---------------------------------------------------------------------------*/
RMAC4_LOCAL void hdrrmac4_chan_enter_open_state ( uint8 phy_chan_idx );

RMAC4_LOCAL void hdrrmac4_tca_update( void );

RMAC4_LOCAL void hdrrmac4_update_bucket_factor_grid ( uint8 flow_id );

RMAC4_LOCAL void hdrrmac4_update_gu_gd_grids
(
  uint8 flow_id,
  uint8 phy_chan_idx
);

RMAC4_LOCAL void hdrrmac4_stop_rl_drop_ack_timer ( uint8 entry );

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
RMAC4_LOCAL void hdrrmac4_flexconn_dtx_monitor( void );
RMAC4_LOCAL void hdrrmac4_submit_flexconn_dtx_log( void );
RMAC4_LOCAL void hdrrmac4_init_flexconn_dtx_log_info( void );
#endif /* defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
uint8 hdrrmac4_get_min_term_target ( void );
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
void hdrrmac4_update_rl_data_rate_est( void );
void hdrrmac4_update_curr_rl_data_rate( void );
void hdrrmac4_latch_rl_info( void );
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*--------------------------------------------------------------------------
               RTCACK MESSAGE STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  uint8   msg_id;
  uint8   num_reverse_channels;
  struct
  {
    sys_channel_type  reverse_channel;
    uint8             reverse_logical_channel;
  } channel_records[HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];
} hdrrmac4_rtcack_msg_type;

RMAC4_LOCAL hdrrmac4_rtcack_msg_type  hdrrmac4_rtcack_msg;

/*--------------------------------------------------------------------------
               RTCACK MESSAGE BIT UNPACKING MACROS
--------------------------------------------------------------------------*/
#define HDRRMAC4_RTCACK_MSG_ID_SIZE                   8

#define HDRRMAC4_RTCACK_NUM_REVERSE_CHANNELS_SIZE     4
#define HDRRMAC4_RTCACK_REVERSE_CHANNEL_SIZE          24
  /* Has the following 3 fields */
#define HDRRMAC4_RTCACK_SYSTEM_TYPE_SIZE              8
#define HDRRMAC4_RTCACK_BAND_CLASS_SIZE               5
#define HDRRMAC4_RTCACK_CHANNEL_NUMBER_SIZE           11
#define HDRRMAC4_RTCACK_REVERSE_LOGICAL_CHANNEL_SIZE  4

/* SystemType Definitions */
#define HDRRMAC4_RTCACK_SYSTEM_TYPE_IS856             0
#define HDRRMAC4_RTCACK_SYSTEM_TYPE_IS2000            1

/* Number of ReverseChannel record in this message */
#define HDRRMAC4_RTCACK_NUM_REVERSE_CHANNELS( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8(ptr, val_ptr, offset,                        \
                         HDRRMAC4_RTCACK_NUM_REVERSE_CHANNELS_SIZE)

/* ReverseChannel SystemType */
#define HDRRMAC4_RTCACK_REVERSE_CHANNEL_SYSTEM_TYPE( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8(ptr, val_ptr, offset, HDRRMAC4_RTCACK_SYSTEM_TYPE_SIZE)

/* ReverseChannel BandClass */
#define HDRRMAC4_RTCACK_REVERSE_CHANNEL_BAND_CLASS( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8(ptr, val_ptr, offset, HDRRMAC4_RTCACK_BAND_CLASS_SIZE)

/* ReverseChannel ChannelNumber */
#define HDRRMAC4_RTCACK_REVERSE_CHANNEL_CHANNEL_NUMBER( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK16(ptr, val_ptr, offset, HDRRMAC4_RTCACK_CHANNEL_NUMBER_SIZE)

/* ReverseLogicalChannel */
#define HDRRMAC4_RTCACK_REVERSE_CHANNEL_REVERSE_LOGICAL_CHANNEL( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8(ptr, val_ptr, offset,                                          \
                         HDRRMAC4_RTCACK_REVERSE_LOGICAL_CHANNEL_SIZE)

/*--------------------------------------------------------------------------
               GRANT MESSAGE STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  uint8    msg_id;                    /* Message ID */
  uint8    num_reverse_channels;      /* Number of Reverse Channels */
  struct
  {
    uint8    reverse_logical_channel;   /* Reverse Logical Channel */
    uint8    num_mac_flows;             /* Number of MAC flows */
    struct
    {
      uint8  mac_flow_id;               /* MAC Flow ID in [0..15] */
      uint8  t2p_inflow;                /* Avg. T2P. 255 means -inifinity; 
                                         * otherwise range is [0..63.5dB] in 1/4dB
                                         * units
                                         */
      uint8  bucket_level;              /* BucketLevel. 255 means -infinity; 
                                         * otherwise range is [0..63.5dB] in 1/4dB
                                         * units
                                         */
      uint8  tt2p_hold;                 /* Duration (in frames) for which the
                                         * T2PInflow from Grant message is to be
                                         * used
                                         */
    } mac_flows[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS];

  } rl_channel_record[HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS];

} hdrrmac4_grant_msg_type;

RMAC4_LOCAL hdrrmac4_grant_msg_type hdrrmac4_grant_msg;

/*--------------------------------------------------------------------------
               GRANT MESSAGE BIT UNPACKING MACROS
--------------------------------------------------------------------------*/
#define HDRRMAC4_GRANT_MSG_ID_SIZE                  8

#define HDRRMAC4_GRANT_NUM_REVERSE_CHANNELS_SIZE    4
#define HDRRMAC4_GRANT_REVERSE_LOGICAL_CHANNEL_SIZE 4
#define HDRRMAC4_GRANT_NUM_MAC_FLOWS_SIZE           4
#define HDRRMAC4_GRANT_MAC_FLOW_ID_SIZE             4
#define HDRRMAC4_GRANT_T2P_INFLOW_SIZE              8
#define HDRRMAC4_GRANT_BUCKET_LEVEL_SIZE            8
#define HDRRMAC4_GRANT_TT2P_HOLD_SIZE               6

/* Number of ReverseChannel record in this message */
#define HDRRMAC4_GRANT_NUM_REVERSE_CHANNELS( ptr, val_ptr, offset )   \
          HDRBIT_UNPACK8(ptr, val_ptr, offset,                        \
                         HDRRMAC4_GRANT_NUM_REVERSE_CHANNELS_SIZE)

/* ReverseLogicalChannel */
#define HDRRMAC4_GRANT_REVERSE_CHANNEL_REVERSE_LOGICAL_CHANNEL( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8(ptr, val_ptr, offset,                                          \
                         HDRRMAC4_GRANT_REVERSE_LOGICAL_CHANNEL_SIZE)

/* Number of MAC flow records in this message */
#define HDRRMAC4_GRANT_NUM_MAC_FLOWS( ptr, val_ptr, offset )  \
         HDRBIT_UNPACK8(ptr, val_ptr, offset, HDRRMAC4_GRANT_NUM_MAC_FLOWS_SIZE)

/* MAC flow ID */
#define HDRRMAC4_GRANT_MAC_FLOW_ID( ptr, val_ptr, offset )  \
         HDRBIT_UNPACK8(ptr, val_ptr, offset, HDRRMAC4_GRANT_MAC_FLOW_ID_SIZE)

/* T2PInflow */
#define HDRRMAC4_GRANT_T2P_INFLOW( ptr, val_ptr, offset )  \
         HDRBIT_UNPACK8(ptr, val_ptr, offset,  HDRRMAC4_GRANT_T2P_INFLOW_SIZE)

/* BucketLevel */
#define HDRRMAC4_GRANT_BUCKET_LEVEL( ptr, val_ptr, offset ) \
         HDRBIT_UNPACK8(ptr, val_ptr, offset,  HDRRMAC4_GRANT_BUCKET_LEVEL_SIZE)

/* TT2PHold */
#define HDRRMAC4_GRANT_TT2P_HOLD( ptr, val_ptr, offset )  \
         HDRBIT_UNPACK8(ptr, val_ptr, offset,  HDRRMAC4_GRANT_TT2P_HOLD_SIZE)

/*--------------------------------------------------------------------------
               ReverseCDMAChannelDroppedAck MESSAGE STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  uint8   msg_id;                     /* Message ID */
  uint8   transaction_id;             /* Transaction ID */
} hdrrmac4_rl_drop_ack_msg_type;

/*--------------------------------------------------------------------------
            ReverseCDMAChannelDroppedAck MESSAGE BIT UNPACKING MACROS
--------------------------------------------------------------------------*/
#define HDRRMAC4_RL_DROP_ACK_MSG_ID_SIZE            8
#define HDRRMAC4_RL_DROP_ACK_TRANS_ID_SIZE          8

/* TransactionID in this message */
#define HDRRMAC4_RL_DROP_ACK_TRANS_ID( ptr, val_ptr, offset)    \
          HDRBIT_UNPACK8(ptr, val_ptr, offset,                  \
                         HDRRMAC4_RL_DROP_ACK_TRANS_ID_SIZE)

/*--------------------------------------------------------------------------
            ReverseCDMAChannelDropped MESSAGE BIT PACKING MACROS
--------------------------------------------------------------------------*/
#define HDRRMAC4_RL_DROP_MSG_ID_SIZE                8
#define HDRRMAC4_RL_DROP_TRANS_ID_SIZE              8
#define HDRRMAC4_RL_DROP_NUM_RLS_SIZE               4
#define HDRRMAC4_RL_DROP_SYSTEM_TYPE_SIZE           8
#define HDRRMAC4_RL_DROP_BAND_CLASS_SIZE            5
#define HDRRMAC4_RL_DROP_CHANNEL_NUMBER_SIZE        11
#define HDRRMAC4_RL_DROP_REASON_CODE_SIZE           4

/*--------------------------------------------------------------------------
               CarrierRequest MESSAGE BIT PACKING MACROS 
--------------------------------------------------------------------------*/
#define HDRRMAC4_CARRIER_REQ_MSG_ID_SIZE            8
#define HDRRMAC4_CARRIER_REQ_NUM_RL_CHANS_SIZE      4
#define HDRRMAC4_CARRIER_REQ_RESERVED_SIZE          4

/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/
 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CARRIER_REQ_INIT                                 INTERNAL

DESCRIPTION
  This function processes the INIT event for Carrier Request functionality.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_carrier_req_init (void)
{
  /* init the control variable */
  memset (&hdrrmac4_carrier_req_var,
          0,
          sizeof(hdrrmac4_carrier_req_var_type));

  hdrrmac4_carrier_req_var.num_chans_pa_headroom =
    hdrrmac4_carrier_req_var.num_chans_req =
      HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;

  hdrrmac4_carrier_req_var.aggressive_drop_detect_timer =
    HDRRMAC4_CARRIER_REQ_DEFAULT_AGGRESSIVE_DROP_TMO_MS;
  hdrrmac4_carrier_req_var.carrier_req_backoff_timer =
    HDRRMAC4_CARRIER_REQ_DEFAULT_BACKOFF_TMO_MS;

  return;

} /* hdrrmac4_carrier_req_init */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CARRIER_REQ_RTCACK                                INTERNAL

DESCRIPTION
  This function processes the RTCACK event for Carrier Request functionality.

DEPENDENCIES
  None.

PARAMETERS
  num_chans - Number of additional channels to be added as a result of
              receiving RTCAck Message.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_carrier_req_rtcack
(
  uint8 num_chans
)
{

  /* Increment the number of channels allocated */
  hdrrmac4_carrier_req_var.num_chans_allocated += num_chans;

  /* Make sure we are doing the right thing */
  if (hdrrmac4_carrier_req_var.num_chans_allocated >
      HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
  {
    ERR_FATAL("CR: num_chans_allocated overflow, %d",
              hdrrmac4_carrier_req_var.num_chans_allocated,
              0,
              0);
  }

  return;

} /* hdrrmac4_carrier_req_rtcack */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CARRIER_REQ_PA_HEADROOM_DROP                      INTERNAL

DESCRIPTION
  This function processes the PA_HEADROOM_DROP event for Carrier Request 
  functionality.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_mask - Mask with bits corresponding to carriers dropped due to
                  PA headroom limitation set.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_carrier_req_pa_headroom_drop
(
  uint8 phy_chan_mask
)
{
  uint8         phy_chan_idx;
    /* Loop counter */

  timetick_type curr_time_ms;
    /* current time in unit of ms */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( phy_chan_idx = 0; 
        phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
        phy_chan_idx++ )
  {
    if ( ((phy_chan_mask & (1 <<phy_chan_idx)) != 0) &&
         HDRRMAC4_IS_RL_OPEN( phy_chan_idx ) )
    {
      /* Update the number of channels allocated */
      hdrrmac4_carrier_req_var.num_chans_allocated -= 1;

      /* Update number of open channels */
      hdrrmac4_var_ptr->num_open_state_chans -= 1;

      /* Make sure we are doing the right thing */
      if (hdrrmac4_carrier_req_var.num_chans_allocated >
          HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
      {
        ERR_FATAL("CR: num_chans_allocated underflow, %d",
                  hdrrmac4_carrier_req_var.num_chans_allocated,
                  0,
                  0);
      }
    }
  }

  hdrrmac4_carrier_req_var.dropped = TRUE;

  /* Set the number of channels reported to AN that AT can support */
  hdrrmac4_carrier_req_var.num_chans_req =
    hdrrmac4_carrier_req_var.num_chans_pa_headroom;

  /* When was the last time we sent the Carrier Request message */
  if (hdrrmac4_carrier_req_var.sent_carrier_req)
  {
    /* 
     * If we drop the carrier shortly after we requested additional carrier,
     * go into aggressive drop mode.
     */
    curr_time_ms = timetick_get_ms();
    if ((curr_time_ms - hdrrmac4_carrier_req_var.last_carrier_req_time) <
        hdrrmac4_carrier_req_var.aggressive_drop_detect_timer)
    {
      hdrrmac4_carrier_req_var.aggressive_drop = TRUE;
    }
  }

  /* We sent a CarrierDrop Message.  Record the current time. */
  hdrrmac4_carrier_req_var.last_carrier_drop_time = timetick_get_ms();
  hdrrmac4_carrier_req_var.sent_carrier_drop = TRUE;

  /* Clear the CarrierRequest sent flag */
  hdrrmac4_carrier_req_var.sent_carrier_req = FALSE;

  return;

} /* hdrrmac4_carrier_req_pa_headroom_drop */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CARRIER_REQ_TCA_DROP                             INTERNAL

DESCRIPTION
  This function processes the TCA_DROP event for Carrier Request functionality.

DEPENDENCIES
  None.

PARAMETERS
  num_chans - Number of channels to be dropped as a result of receiving a
              new TCA.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_carrier_req_tca_drop
(
  uint8 num_chans
)
{
  /* Decrement the number of channels allocated */
  hdrrmac4_carrier_req_var.num_chans_allocated -= num_chans;

  /* Make sure we are doing the right thing */
  if (hdrrmac4_carrier_req_var.num_chans_allocated >
      HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
  {
    ERR_FATAL("CR: num_chans_allocated underflow, %d",
              hdrrmac4_carrier_req_var.num_chans_allocated,
              0,
              0);
  }

  return;

} /* hdrrmac4_carrier_req_tca_drop */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CARRIER_REQ_DRC_SUPERVISION_DROP                  INTERNAL

DESCRIPTION
  This function processes the DRC_SUPERVISION_DROP event for Carrier Request 
  functionality.

DEPENDENCIES
  None.

PARAMETERS
  num_chans - Number of channels to be dropped due to DRC Supervision timeout.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_carrier_req_drc_supervision_drop
(
  uint8 phy_chan_idx
)
{

  /* Update number of allocated and open state channels if the dropped
     carrier is in OPEN state */
  if (HDRRMAC4_IS_RL_OPEN( phy_chan_idx ))
  {
    /* Decrement the number of channels allocated */
    hdrrmac4_carrier_req_var.num_chans_allocated--;

    /* Update number of open channels */
    hdrrmac4_var_ptr->num_open_state_chans--;

    /* Make sure we are doing the right thing */
    if (hdrrmac4_carrier_req_var.num_chans_allocated >
        HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
    {
      ERR_FATAL("CR: num_chans_allocated underflow, %d",
                hdrrmac4_carrier_req_var.num_chans_allocated,
                0,
                0);
    }
  }

  return;

} /* hdrrmac4_carrier_req_drc_supervision_drop */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CARRIER_REQ_POWER_DIFF_DROP                       INTERNAL

DESCRIPTION
  This function processes the transmit pilot power differential drop 
  event for Carrier Request functionality.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Index of carrier dropped due to power differential.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_carrier_req_power_diff_drop
(
  uint8 phy_chan_idx
)
{

  /* Update number of allocated and open state channels if the dropped
     carrier is in OPEN state */
  if (HDRRMAC4_IS_RL_OPEN( phy_chan_idx ))
  {
    /* Decrement the number of channels allocated */
    hdrrmac4_carrier_req_var.num_chans_allocated--;

    /* Update number of open channels */
    hdrrmac4_var_ptr->num_open_state_chans--;

    /* Make sure we are doing the right thing */
    if (hdrrmac4_carrier_req_var.num_chans_allocated >
        HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
    {
      ERR_FATAL("CR: num_chans_allocated underflow, %d",
                hdrrmac4_carrier_req_var.num_chans_allocated,
                0,
                0);
    }
  }

  return;

} /* hdrrmac4_carrier_req_power_diff_drop */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_SEND_CARRIER_REQ_MSG                              INTERNAL

DESCRIPTION
  This function is called to transmit a CarrierRequest message.

DEPENDENCIES
  None.

PARAMETERS
  num_chans - Total number of Reverse CDMA channels AT is requesting be
              allocated to it.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_send_carrier_req_msg
(
  uint8 num_chans
)
{

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to message to send */

  uint8 msg_offset = 0;
    /* Message field offset from start of DSM item */

  uint8 entry;
    /* rl_drop entry number */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                 "Send CarrierReq Msg, NumReverseChannels %d",
                 (num_chans-1));  /* one less */

  /* Get a new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Pack CarrierRequest message's Message ID */
  HDRBIT_PACK8(send_msg_ptr,
               HDRRMAC4_CARRIER_REQ_MSG_ID,
               msg_offset,
               HDRRMAC4_CARRIER_REQ_MSG_ID_SIZE);
  msg_offset += HDRRMAC4_CARRIER_REQ_MSG_ID_SIZE;

  /* Pack CarrierRequest message's NumReverseChannels */
  HDRBIT_PACK8(send_msg_ptr,
               (num_chans-1), /* one less */
               msg_offset,
               HDRRMAC4_CARRIER_REQ_NUM_RL_CHANS_SIZE);
  msg_offset += HDRRMAC4_CARRIER_REQ_NUM_RL_CHANS_SIZE;

  /* Pack CarrierRequest message's reserved field */
  HDRBIT_PACK8(send_msg_ptr,
               0,
               msg_offset,
               HDRRMAC4_CARRIER_REQ_RESERVED_SIZE);
  msg_offset += HDRRMAC4_CARRIER_REQ_RESERVED_SIZE;

  /* Send the msg. */
  hdrhmp_send_msg_on_existing_rtc(
    HDRHAI_RTC_MAC_PROTOCOL,
    TRUE,          /* Reliable */
    HDRRMAC4_CARRIER_REQ_MSG_PRI,
    HDRHAI_IN_USE,  /* protocol instance */
    send_msg_ptr,
    NULL,
    NULL);

  /* Stop resending of carrier drop msgs once a carrier request is queued. */
  for (entry = 0; entry < HDRRMAC4_MAX_NUM_DROP_MSGS; entry++)
  {
    if (hdrrmac4_rl_drop_var.rl_drop_msg[entry].in_use)
    {
      /* Cancel the ACK timer */
      hdrrmac4_stop_rl_drop_ack_timer(entry);
    }
  }

  return;

} /* hdrrmac4_send_carrier_req_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CARRIER_REQ_PA_HEADROOM_ADD                        INTERNAL

DESCRIPTION
  This function processes the PA_HEADROOM_ADD event for Carrier Request 
  functionality.

DEPENDENCIES
  None.

PARAMETERS
  num_chans - Total number of channels that can be supported based on currently
              available PA headroom.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_carrier_req_pa_headroom_add
(
  uint8 num_chans
)
{
  timetick_type curr_time_ms;
    /* current time in unit of ms */

  boolean       sector_support_mc;
    /* does current serving sector support multi-carrier? */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get the current time */
  curr_time_ms = timetick_get_ms();

  if ((!hdrrmac4_carrier_req_var.aggressive_drop) ||
       (!hdrrmac4_carrier_req_var.sent_carrier_drop) ||
       ((curr_time_ms - hdrrmac4_carrier_req_var.last_carrier_drop_time) >
         hdrrmac4_carrier_req_var.carrier_req_backoff_timer))
  {
    /* Retrieve IsSectorMultiCarrierCapable from OVHD */
    if (hdrovhd_get_sector_is_mc_capable
          (&sector_support_mc,
           HDRHAI_RTC_MAC_PROTOCOL) != E_SUCCESS)
    {
      HDR_MSG_PROT
        (MSG_LEGACY_HIGH,
         "IsSectorMultiCarrierCapable unavailble, force FALSE");

      sector_support_mc = FALSE;
    }

    if (sector_support_mc)
    {
      /* 
       * We can send the CarrierRequest Message if IsSectorMultiCarrierCapable
       * is true AND if any one of the following is true:
       *    1. Not in aggressive drop mode;
       *    2. Last CarrierRequest did not result in a CarrierDrop
       *    3. Even through last CarrierRequest resulted in a CarrierDrop,
       *       the backoff time has since elapsed.
       */

      /* Construct and send CarrierRequest Message */
      hdrrmac4_send_carrier_req_msg(hdrrmac4_carrier_req_var.num_chans_pa_headroom);

      /* Send RouteUpdate CMD */
      hdrrup_send_route_update(HDRHAI_RTC_MAC_PROTOCOL);

      /* record the time the CarrierRequest Message is sent */
      hdrrmac4_carrier_req_var.last_carrier_req_time = timetick_get_ms();
      hdrrmac4_carrier_req_var.sent_carrier_req = TRUE;

      /* clear the CarrierDrop sent flag */
      hdrrmac4_carrier_req_var.sent_carrier_drop = FALSE;

      /* Update number of channels requested */
      hdrrmac4_carrier_req_var.num_chans_req = num_chans;

      /* Clear the aggressive drop flag */
      hdrrmac4_carrier_req_var.aggressive_drop = FALSE;
    }
  }

  return;

} /* hdrrmac4_carrier_req_pa_headroom_add */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CARRIER_REQ_EVENT                                 INTERNAL

DESCRIPTION
  This function accepts the event and implements the Carrier Request 
  functionality.

DEPENDENCIES
  None.

PARAMETERS
  event         - One of hdrrmac4_carrier_req_event_type.
  num_chans     - Number of channels associated with the input event
  phy_chan_idx  - Physical channel index of channel associated with 
                  the input event.
  phy_chan_mask - Physical channel mask associated with the input event.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_carrier_req_event
(
  hdrrmac4_carrier_req_event_type event,
  uint8                           num_chans,
  uint8                           phy_chan_idx,
  uint8                           phy_chan_mask
)
{
  switch (event)
  {
    case HDRRMAC4_CARRIER_REQ_EVENT_INIT:
      hdrrmac4_carrier_req_init();
      break;
  
    case HDRRMAC4_CARRIER_REQ_EVENT_RTCACK:
      hdrrmac4_carrier_req_rtcack(num_chans);
      break;
  
    case HDRRMAC4_CARRIER_REQ_EVENT_PA_HEADROOM_DROP:
      hdrrmac4_carrier_req_pa_headroom_drop(phy_chan_mask);
      break;
  
    case HDRRMAC4_CARRIER_REQ_EVENT_TCA_DROP:
      hdrrmac4_carrier_req_tca_drop(num_chans);
      break;
  
    case HDRRMAC4_CARRIER_REQ_EVENT_DRC_SUPERVISION_DROP:
      hdrrmac4_carrier_req_drc_supervision_drop(phy_chan_idx);
      break;
  
    case HDRRMAC4_CARRIER_REQ_EVENT_PA_HEADROOM_ADD:
      hdrrmac4_carrier_req_pa_headroom_add(num_chans);
      break;

    case HDRRMAC4_CARRIER_REQ_EVENT_TXP_DIFF:
      hdrrmac4_carrier_req_power_diff_drop(phy_chan_idx);
      break;
  
    default:
      ERR_FATAL("CR: Unknown event %d",
                event,
                0,
                0);
  }

  /* Any event might trigger a need to update */
  if (hdrrmac4_carrier_req_var.dropped)
  {
    if (hdrrmac4_carrier_req_var.num_chans_allocated >
        hdrrmac4_carrier_req_var.num_chans_req)
    {
      hdrrmac4_carrier_req_var.num_chans_req =
        hdrrmac4_carrier_req_var.num_chans_allocated;

      hdrrmac4_carrier_req_var.aggressive_drop = FALSE;
    }

    if (hdrrmac4_carrier_req_var.num_chans_req ==
        HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
    {
      hdrrmac4_carrier_req_var.dropped = FALSE;
    }
  }

  /* Dump the debug information */
  HDR_MSG_DEBUG_6( HDR_MSG_DBG_RTCMAC_DECISIONS,
                   "CR: Ev %d, C %d, P %d, R %d, dropped %d, aggressive %d",
                   event,
                   hdrrmac4_carrier_req_var.num_chans_allocated,
                   hdrrmac4_carrier_req_var.num_chans_pa_headroom,
                   hdrrmac4_carrier_req_var.num_chans_req,
                   hdrrmac4_carrier_req_var.dropped,
                   hdrrmac4_carrier_req_var.aggressive_drop );

  HDR_MSG_DEBUG_4( HDR_MSG_DBG_RTCMAC_DECISIONS,
                   "CR: sent CarrierReq %d, sent CarrierDrop %d, LastReq %d, LastDrop %d",
                   hdrrmac4_carrier_req_var.sent_carrier_req,
                   hdrrmac4_carrier_req_var.sent_carrier_drop,
                   hdrrmac4_carrier_req_var.last_carrier_req_time,
                   hdrrmac4_carrier_req_var.last_carrier_drop_time );

  return;

} /* hdrrmac4_carrier_req_event */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_START_RL_ACQ_TIMER                                 INTERNAL

DESCRIPTION
  This function starts the connection setup timer for the specified channel.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel whose timer is to be started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_start_rl_acq_timer
(
  uint8 phy_chan_idx
)
{

  (void) rex_set_timer( &hdrrmac4_rl_acq_timer[phy_chan_idx],
    HDRRMAC4_SETUP_TIMER_MSECS );

  return;

} /* hdrrmac4_start_rl_acq_timer */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_STOP_RL_ACQ_TIMER                                 INTERNAL

DESCRIPTION
  This function stops the connection setup timer for the specified channel.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel whose timer is to be started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_stop_rl_acq_timer
(
  uint8 phy_chan_idx
)
{

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                 "RL ACQ Timer %d stopped",
                 phy_chan_idx);

  (void) rex_clr_timer ( &hdrrmac4_rl_acq_timer[phy_chan_idx] );

  return;

} /* hdrrmac4_stop_rl_acq_timer */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_START_RL_DROP_ACK_TIMER                            INTERNAL

DESCRIPTION
  This function starts the rl drop ack timer.

DEPENDENCIES
  None.

PARAMETERS
  entry - hdrrmac4_rl_drop_var.rl_drop_msg[] entry number.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_start_rl_drop_ack_timer
(
  uint8 entry
)
{

  (void) rex_set_timer( &hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_drop_ack_timer,
    HDRRMAC4_RL_DROP_ACK_TIMER_MSECS );

  return;

} /* hdrrmac4_start_rl_drop_ack_timer */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_STOP_RL_DROP_ACK_TIMER                            INTERNAL

DESCRIPTION
  This function stops the rl drop ack timer.

DEPENDENCIES
  None.

PARAMETERS
  entry - hdrrmac4_rl_drop_var.rl_drop_msg[] entry number.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_stop_rl_drop_ack_timer
(
  uint8 entry
)
{

  (void) rex_clr_timer ( 
    &hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_drop_ack_timer );

  hdrrmac4_rl_drop_var.rl_drop_msg[entry].in_use = FALSE;
  hdrrmac4_rl_drop_var.rl_drop_msg[entry].msg_send_status_arrived = FALSE;

  return;

} /* hdrrmac4_stop_rl_drop_ack_timer */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_RL_ACQ_TIMER_CB                                 INTERNAL

DESCRIPTION
  This function is the timer callback function when connection setup timer
  expires.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_rl_acq_timer_cb
(
  uint32 timer_id
)
{
  /* The callback is called in timer context when RL acquisition timer 
     expires. The timer expiry processing is queued to be processed by HDRTX
     task. A timer id is assigned here for hdrrmac4_process_timer() to 
     differentiate between processing for different RMAC4 timers. */

  /* Use the first 16 bits for identifying RMAC4 timer id. */
  timer_id &= 0xFFFF;
  timer_id |= (HDRRMAC4_TIMER_RL_ACQ << 16);

  /* queue the timer to RMAC protocol */
  hdrtx_queue_timer(HDRHAI_RTC_MAC_PROTOCOL, timer_id);

  return;

} /* hdrrmac4_rl_acq_timer_cb */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_GENERATE_RL_DROP_MSG                              INTERNAL

DESCRIPTION
  This function is called to construct a new ReverseCDMAChannelDropped
  message.

DEPENDENCIES
  None.

PARAMETERS
  num_chans     - Number of channels to be dropeed.
  drop_info_ptr - pointer to num_chans instance of hdrrmac4_drop_info_type
                  structure.

RETURN VALUE
  entry - the entry number in hdrrmac4_rl_drop_var.rl_drop_msg[] used to 
          genrate this drop msg.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL uint8 hdrrmac4_generate_rl_drop_msg
(
  uint8                     num_chans,
  hdrrmac4_drop_info_type   *drop_info_ptr
)
{
  uint8 loop;
    /* loop index */

  uint8 phy_chan_idx;
    /* channel loop index */

  boolean found = FALSE;
    /* found an available entry */

  uint8 max_attempts=0;
    /* 
     * The max. number of attempts of sending the RL drop msg.  When running
     * out of available timer block for re-tx monitoring, the one with the
     * max. number of attempts will be replaced.
     */

  uint8 max_attempts_entry=0;
    /* The entry index of the timer block with max. attempts. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* increment the global transaction ID */
  hdrrmac4_rl_drop_var.trans_id++;

  /* search for an available drop message entry */
  for (loop = 0; 
       (!found &&
        (loop < HDRRMAC4_MAX_NUM_DROP_MSGS)); 
       loop++)
  {
    if (!hdrrmac4_rl_drop_var.rl_drop_msg[loop].in_use)
    {
      hdrrmac4_rl_drop_var.rl_drop_msg[loop].in_use = TRUE;
      hdrrmac4_rl_drop_var.rl_drop_msg[loop].msg_send_status_arrived = FALSE;

      /* reset the re-tx count */
      hdrrmac4_rl_drop_var.rl_drop_msg[loop].num_attempts = 0;

      /* break out from the loop */
      found = TRUE;
    }
    else
    {
      /*
       * Keep track of the message entry with the max. number of attempts.
       * If we can't find a free entry, use the one with the max. attempts.
       */
      if (hdrrmac4_rl_drop_var.rl_drop_msg[loop].num_attempts >
          max_attempts)
      {
        /* update the max. attempts and entry */
        max_attempts = hdrrmac4_rl_drop_var.rl_drop_msg[loop].num_attempts;
        max_attempts_entry = loop;
      }
    }
  }

  if (!found)
  {
    /* 
     * This may happen if there are many back-to-back drops, but we still
     * hold on to at least 1 carrier to keep the overall connection.
     */
    HDR_MSG_PROT( MSG_LEGACY_FATAL, "No free rl drop msg slot");

    /* Running out of the timer block.  Use the oldest one. */
    loop = max_attempts_entry;
  }
  else
  {
    /* adjust for the post-increment from the loop */
    loop--;
  }

  /* copy the trans_id */
  hdrrmac4_rl_drop_var.rl_drop_msg[loop].rl_dropped_msg.trans_id =
    hdrrmac4_rl_drop_var.trans_id;

  /* NumReverseChannels */
  hdrrmac4_rl_drop_var.rl_drop_msg[loop].rl_dropped_msg.num_rls = num_chans;

  /* copy drop channel information */
  for (phy_chan_idx = 0; phy_chan_idx < num_chans; phy_chan_idx++)
  {
  /* Channel Record */
#ifdef FEATURE_MEMCPY_REMOVAL
    hdrrmac4_rl_drop_var.rl_drop_msg[loop].
       rl_dropped_msg.chan_record[phy_chan_idx].chan = 
    hdrrmac4_var_ptr->phy_chan
       [(drop_info_ptr[phy_chan_idx].phy_chan_idx)].chan_record;
#else
    memcpy((void *)&hdrrmac4_rl_drop_var.rl_drop_msg[loop].
                      rl_dropped_msg.chan_record[phy_chan_idx].chan,
           (void *)&hdrrmac4_var_ptr->phy_chan
                    [(drop_info_ptr[phy_chan_idx].phy_chan_idx)].chan_record,
           sizeof(sys_channel_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* reason code */
    hdrrmac4_rl_drop_var.rl_drop_msg[loop].rl_dropped_msg.
      chan_record[phy_chan_idx].reason_code =
        drop_info_ptr[phy_chan_idx].reason_code;
  }

  return(loop);

} /* hdrrmac4_generate_rl_drop_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_RL_DROP_MSG_CB                                    INTERNAL

DESCRIPTION
  This function is called when the status on queued ReverseCDMAChannelDropped
  message to HMP is available.

DEPENDENCIES
  None.

PARAMETERS
  data_ptr - pointer to the queued RL drop message.
  status   - send status of the the queued message

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_rl_drop_msg_cb
( 
  void                *data_ptr,
  hdrerrno_enum_type  status
)
{
  hdrrmac4_rl_drop_msg_type * rl_drop_msg_ptr;

  /* Error catching */
  ASSERT ( data_ptr != NULL );

  rl_drop_msg_ptr = (hdrrmac4_rl_drop_msg_type *)data_ptr;
  
  rl_drop_msg_ptr->msg_send_status_arrived = TRUE;

  if ( status != E_SUCCESS )
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                   "RCCD send failed: Trans_id=%d, chan=%d",
                   rl_drop_msg_ptr->rl_dropped_msg.trans_id,
                   rl_drop_msg_ptr->rl_dropped_msg.chan_record[0].chan.chan_num);
  }

} /* hdrrmac4_rl_drop_msg_cb */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_SEND_RL_DROP_MSG                                INTERNAL

DESCRIPTION
  This function is called to transmit a ReverseCDMAChannelDropped
  message.

DEPENDENCIES
  None.

PARAMETERS
  entry - the entry number in hdrrmac4_rl_drop_var.rl_drop_msg[] whose msg
          is to be sent.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_send_rl_drop_msg
(
  uint8 entry
)
{

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to message to send */

  uint8 msg_offset = 0;
    /* Message field offset from start of DSM item */

  uint8 chan;
    /* channel loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Array boundary sanity check */
  ASSERT ( entry < HDRRMAC4_MAX_NUM_DROP_MSGS );

  /* If this is a repeat RCCD, queue it only if the previous one has been 
     transmitted */
  if ( (hdrrmac4_rl_drop_var.rl_drop_msg[entry].num_attempts == 0)
    || (hdrrmac4_rl_drop_var.rl_drop_msg[entry].msg_send_status_arrived) )
  {
    HDR_MSG_PROT_6
      (MSG_LEGACY_HIGH,
       "Send RL Dropped Msg, Trans_id = %d, attempt %d, num_rls %d, band %d, chan %d, reason %d",
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.trans_id,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].num_attempts,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.num_rls,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.chan_record[0].chan.band,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.chan_record[0].chan.chan_num,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.chan_record[0].reason_code);
  
    /* Get a new DSM buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();
  
    /* Pack ReverseCDMAChannelDropped message's Message ID */
    HDRBIT_PACK8(send_msg_ptr,
                 HDRRMAC4_RL_DROP_MSG_ID,
                 msg_offset,
                 HDRRMAC4_RL_DROP_MSG_ID_SIZE);
    msg_offset += HDRRMAC4_RL_DROP_MSG_ID_SIZE;
  
    /* Pack ReverseCDMAChannelDropped message's Transaction ID */
    HDRBIT_PACK8(send_msg_ptr,
                 hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.trans_id,
                 msg_offset,
                 HDRRMAC4_RL_DROP_TRANS_ID_SIZE);
    msg_offset += HDRRMAC4_RL_DROP_TRANS_ID_SIZE;
  
    /* Pack ReverseCDMAChannelDropped message's NumReverseChannels */
    HDRBIT_PACK8(send_msg_ptr,
                 (hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.num_rls-1), /* 1 less */
                 msg_offset,
                 HDRRMAC4_RL_DROP_NUM_RLS_SIZE);
    msg_offset += HDRRMAC4_RL_DROP_NUM_RLS_SIZE;
  
    for (chan=0;
         chan<hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.num_rls;
         chan++)
    {
      /* Pack SystemType of ReverseChannel */
      HDRBIT_PACK8(send_msg_ptr,
                   0, /* System Compliant to IS-856 */
                   msg_offset,
                   HDRRMAC4_RL_DROP_SYSTEM_TYPE_SIZE);
      msg_offset += HDRRMAC4_RL_DROP_SYSTEM_TYPE_SIZE;
  
      /* Pack BandClass of ReverseChannel */
      HDRBIT_PACK8(send_msg_ptr,
                   (uint8) hdrrmac4_rl_drop_var.rl_drop_msg[entry].
                   rl_dropped_msg.chan_record[chan].chan.band,
                   msg_offset,
                   HDRRMAC4_RL_DROP_BAND_CLASS_SIZE);
      msg_offset += HDRRMAC4_RL_DROP_BAND_CLASS_SIZE;
  
      /* Pack ChannelNumber of ReverseChannel */
      HDRBIT_PACK16(send_msg_ptr,
                    hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.
                       chan_record[chan].chan.chan_num,
                    msg_offset,
                    HDRRMAC4_RL_DROP_CHANNEL_NUMBER_SIZE);
      msg_offset += HDRRMAC4_RL_DROP_CHANNEL_NUMBER_SIZE;
  
      /* Pack ReasonCode */
      HDRBIT_PACK8(send_msg_ptr,
                   (uint8) hdrrmac4_rl_drop_var.rl_drop_msg[entry].
                   rl_dropped_msg.chan_record[chan].reason_code,
                   msg_offset,
                   HDRRMAC4_RL_DROP_REASON_CODE_SIZE);
      msg_offset += HDRRMAC4_RL_DROP_REASON_CODE_SIZE;
  
    } /* for each ReverseChannel */
  
    /* Reset RCCD msg send status flag */
    hdrrmac4_rl_drop_var.rl_drop_msg[entry].msg_send_status_arrived = FALSE;
  
    /* Send the msg. */
    hdrhmp_send_msg_on_existing_rtc(
      HDRHAI_RTC_MAC_PROTOCOL,
      FALSE,          /* Not reliable */
      HDRRMAC4_RL_DROP_MSG_PRI,
      HDRHAI_IN_USE,  /* protocol instance */
      send_msg_ptr,
      (hdrslp_outcome_cb_type) hdrrmac4_rl_drop_msg_cb,
      (void *)(&hdrrmac4_rl_drop_var.rl_drop_msg[entry]));
  }
  else
  {
    HDR_MSG_PROT_6
      (MSG_LEGACY_HIGH,
       "Skipped RL Dropped Msg, Trans_id = %d, attempt %d, num_rls %d, band %d, chan %d, reason %d",
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.trans_id,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].num_attempts,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.num_rls,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.chan_record[0].chan.band,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.chan_record[0].chan.chan_num,
       hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.chan_record[0].reason_code);
  }

  /* Start the ACK timer */
  hdrrmac4_start_rl_drop_ack_timer(entry);

  return;

} /* hdrrmac4_send_rl_drop_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_RL_DROP_ACK_TIMER_CB                              INTERNAL

DESCRIPTION
  This function is the timer callback function when RL dropped ack timer
  expires.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_rl_drop_ack_timer_cb
(
  uint32 timer_id
)
{
  /* The callback is called in timer context when RL drop timer expires. The
     timer expiry processing is queued to be processed by HDRTX task. A timer 
     id is assigned here for hdrrmac4_process_timer() to differentiate between
     processing for different RMAC4 timers. */

  /* Use the first 16 bits for identifying RMAC4 timer id. */
  timer_id &= 0xFFFF;
  timer_id |= (HDRRMAC4_TIMER_RL_DROP_ACK << 16);

  /* queue the timer to RMAC protocol */
  hdrtx_queue_timer(HDRHAI_RTC_MAC_PROTOCOL, timer_id);

  return;

} /* hdrrmac4_rl_drop_ack_timer_cb */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_INIT_TIMERS                                       INTERNAL

DESCRIPTION
  This function initializes the connection setup and RL dropped ack timers.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_init_timers (void)
{

  uint8 channel_id;
    /* Reverse Channel ID */

  uint8 slot;
    /* rl_drop_msg slot index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Define the REX ReverseAcquisition Timer for each channel */
  for (channel_id = 0; channel_id < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; channel_id++)
  {
    rex_def_timer_ex( &hdrrmac4_rl_acq_timer[channel_id],
                      hdrrmac4_rl_acq_timer_cb,
                      channel_id ); /* cb_param */
  }

  /* Define up to HDRRMAC4_MAX_NUM_DROP_MSGS timers */
  for (slot = 0; slot < HDRRMAC4_MAX_NUM_DROP_MSGS; slot++)
  {
    hdrrmac4_rl_drop_var.rl_drop_msg[slot].in_use = FALSE;

    hdrrmac4_rl_drop_var.rl_drop_msg[slot].msg_send_status_arrived = FALSE;

    rex_def_timer_ex( &hdrrmac4_rl_drop_var.rl_drop_msg[slot].rl_drop_ack_timer,
                    hdrrmac4_rl_drop_ack_timer_cb,
                      slot ); /* cb_param */
  }

  return;

} /* hdrrmac4_init_timers */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_FROM_RF_TO_PHY                                 INTERNAL

DESCRIPTION
  This function finds the associated physical channel index for the specified
  RF channel in sys_channel_type format.

DEPENDENCIES
  None.

PARAMETERS
  rf_chan_ptr - Pointer to the RF channel structure in sys_channel_type

RETURN VALUE
  phy_chan_id - ID of the physical channel assigned to this RF channel, or
                HDRRMAC4_INVALID_CHAN_ID if no match found.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL uint8 hdrrmac4_from_rf_to_phy
(
  sys_channel_type  *rf_chan_ptr
)
{

  uint8                   phy_chan;
    /* loop index for physical channels */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (phy_chan=0; phy_chan<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;phy_chan++)
  {
    if ((rf_chan_ptr->band == 
         hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record.band) &&
        (rf_chan_ptr->chan_num == 
         hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record.chan_num))
    {
      break;
    }
  }

  if (phy_chan == HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
  {
    /* Can't find a match when reaching the end of the loop */
    phy_chan = HDRRMAC4_INVALID_CHAN_ID;

    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                   "band %d, chan %d not found",
                   rf_chan_ptr->band,
                   rf_chan_ptr->chan_num);
  }

  return phy_chan;

} /* hdrrmac4_from_rf_to_phy */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_FROM_LOG_TO_PHY                                 INTERNAL

DESCRIPTION
  This function finds the associated physical channel index for the specified
  reverse logical channel.

DEPENDENCIES
  None.

PARAMETERS
  log_chan_id - ID of the reverse logical channel.

RETURN VALUE
  phy_chan_id - ID of the physical channel assigned to this reverse logical channel,
                or HDRRMAC4_INVALID_CHAN_ID if no match found.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL uint8 hdrrmac4_from_log_to_phy
(
  uint8 log_chan
)
{
  return(hdrrmac4_var_ptr->log_chan[log_chan].phy_chan_id);
} /* hdrrmac4_from_log_to_phy */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_FROM_PHY_TO_LOG                                 INTERNAL

DESCRIPTION
  This function finds the associated reverse logical channel index for the 
  specified physical channel.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_id - ID of the physical channel.

RETURN VALUE
  log_chan_id - ID of the reverse logical channel assigned to this physical 
                channel, or HDRRMAC4_INVALID_CHAN_ID if no match found.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL uint8 hdrrmac4_from_phy_to_log
(
  uint8 phy_chan
)
{
  return(hdrrmac4_var_ptr->phy_chan[phy_chan].log_chan_id);
} /* hdrrmac4_from_phy_to_log */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_INIT_OPEN_CHAN_LIST                               INTERNAL

DESCRIPTION
  This function construct a list of open channels to be processed by the RMAC4
  per-channel algorithm for the current subframe.

DEPENDENCIES
  None.

PARAMETERS
  tally_open_channel_count
        - TRUE  => tally open channel count with the number stored in 
            num_open_state_chans
        - FALSE => Do not tally open channel count.  This is used in 
            situations where it is known that the open channel count tally 
            may fail. 
            e.g. from hdrrmac4_chan_enter_inactive_state().  This is done
            to cover for the case when two carriers are dropped due to PA 
            headroom in a single subframe.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_init_open_chan_list
(
  boolean tally_open_chan_count
)
{
  uint8 chan;
    /* channel loop index */

  uint8 open_chan_count = 0;
    /* total number of OPEN channels */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
  {
    if (HDRRMAC4_IS_RL_OPEN(chan))
    {
      /* Record the OPEN channel ID */
      hdrrmac4_open_rl_list[open_chan_count] = chan;

      /* increment the count */
      open_chan_count++;
    }
  }

  if (tally_open_chan_count)
  {
    /* Do a sanity check */
    if (open_chan_count != hdrrmac4_var_ptr->num_open_state_chans)
    {
      ERR_FATAL("Invalid OPEN channel count %d vs. %d",
                open_chan_count,
                hdrrmac4_var_ptr->num_inactive_state_chans,
                0);
    }
  }

  return;

} /* hdrrmac4_init_open_chan_list */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CHAN_ENTER_INACTIVE_STATE                         INTERNAL

DESCRIPTION
  This function transitions the specified reverse physical channel into
  INACTIVE state.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_chan_enter_inactive_state
(
  uint8 phy_chan_idx
)
{
  uint8 log_chan;
    /* Logical channel id */

  uint8 chan;
    /* channel loop index */

  HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                  "Channel %d entering inactive state: %d (num chans %d)",
                  phy_chan_idx, hdrrmac4_var_ptr->num_inactive_state_chans );

  /* Trigger event to report carrier state change */
  HDRTRACE_RMAC_CARRIER_STATE_CHANGED( 
    phy_chan_idx, 
    (int32) hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_record.band,
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_record.chan_num,
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state,
    HDRRMAC4_CHAN_INACTIVE_STATE );

  /* Change the channel state variable */
  hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state = 
    HDRRMAC4_CHAN_INACTIVE_STATE;

  /* increment the INACTIVE state channel counts */
  hdrrmac4_var_ptr->num_inactive_state_chans++;

  /* Obtain logical channel id corresponding to the channel to be removed */
  log_chan = hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
    log_chan_id;

  /* Clean up the logical channel information */
  if (log_chan != HDRRMAC4_INVALID_CHAN_ID)
  {
    hdrrmac4_var_ptr->log_chan[log_chan].phy_chan_id =
        HDRRMAC4_INVALID_CHAN_ID;

    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].log_chan_id =
      HDRRMAC4_INVALID_CHAN_ID;
  }

  /* Agilent incorrectly assigns same logical channel to multiple 
     physical channels.  When one of the assigned channels is dropped and
     the corresponding logical channel data cleared, AT crashes.  
     The code below is to work around the Agilent bug. */

  for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
  {
    if (HDRRMAC4_IS_RL_OPEN(chan))
    {
      /* Obtain logical channel id corresponding to this channel */
      log_chan = hdrrmac4_var_ptr->phy_chan[chan].log_chan_id;

      ASSERT( log_chan != HDRRMAC4_INVALID_CHAN_ID );

      if ( hdrrmac4_var_ptr->log_chan[log_chan].phy_chan_id ==
        HDRRMAC4_INVALID_CHAN_ID )
      {
        HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
          "Invalid logical (%d) --> physical (%d) channel mapping!",
          log_chan,
          chan );

        /* Assign valid physical channel to this logical channel */
        hdrrmac4_var_ptr->log_chan[log_chan].phy_chan_id = 
          chan;

      } /* if incorrect logical<->physical channel mapping is detected */

    } /* if the channel is in OPEN state */

  } /* for all channels */


  /* Clean up the RF channel information */
  hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_record.band =
    SYS_BAND_CLASS_NONE;

  /* cancel the RL timer in case it's active */
  hdrrmac4_stop_rl_acq_timer(phy_chan_idx);

  /* Refresh open channel list */
  hdrrmac4_init_open_chan_list(FALSE);

  return;

} /* hdrrmac4_chan_enter_inactive_state */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_INIT_BUCKET_FOR_NEW_RL                              INTERNAL

DESCRIPTION
  This function initializes bucket parameters for newly addeded reverse 
  channels.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_id - ID of the newly added physical channel.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
RMAC4_LOCAL void hdrrmac4_init_bucket_for_new_rl( uint8 phy_chan_id )
{
  uint8 flow;
  uint8 log_chan_id;
    /* Logical Channel ID */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* C.6 Init Bucket for each MAC flows in each newly added reverse channel */
  for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++)
  {
    if ((hdrrmac4_var_ptr->data_flow[flow].bucket.last_bucket_level_max != 0) &&
        (hdrrmac4_config_ptr->flow[flow].bucket_level_max != 0))
    {
       log_chan_id = hdrrmac4_from_phy_to_log(phy_chan_id);

       ASSERT(log_chan_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS);

       /* The first two initializations will make the effective initial value 
        * of bucket_level equal to 0 for newly activated MAC flows when the 
        * bucket levels are updated (see * hdrrmac4_update_bucket_level()) for
        * all flows. This avoids the extra step of reimbursing the buckets 
        * after the T2P resource has been deducted assuming that the packet 
        * will be sent, whereas a packet may not be transmitted due to a NAK.
        */
       hdrrmac4_var_ptr->phy_chan[phy_chan_id].flow[flow].
         bucket.bucket_level.prev = 
          -( (int32) hdrrmac4_config_ptr->nc[flow][log_chan_id].
                       t2p_inflow_range_ptr->min );
       hdrrmac4_var_ptr->phy_chan[phy_chan_id].flow[flow].
         bucket.t2p_inflow.prev   = 
          hdrrmac4_config_ptr->nc[flow][log_chan_id].
           t2p_inflow_range_ptr->min;

       hdrrmac4_var_ptr->phy_chan[phy_chan_id].flow[flow].
         bucket.tt2p_hold.prev    = 0;

       hdrrmac4_var_ptr->phy_chan[phy_chan_id].flow[flow].
         bucket.bucket_level_sat  = 
          hdrrmac4_config_ptr->flow[flow].bucket_level_max;

       hdrrmac4_var_ptr->data_flow[flow].bucket.last_bucket_level_max = 
           hdrrmac4_config_ptr->flow[flow].bucket_level_max;

    } /* if a flow is activated */
  } /* for each flow */

  return;

} /* hdrrmac4_init_bucket_for_new_rl */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CHAN_ENTER_ADD_STATE                          INTERNAL

DESCRIPTION
  This function transitions the reverse physical channel specified in the
  update_rl_carriers information into ADD state.  If the specified channel is
  already in the DELETE state, the specified channel will be placed in the
  ADD_DELETE state.

DEPENDENCIES
  None.

PARAMETERS
  assigned_chan_idx   - Assigned channel index in the 
                        hdrrmac_rl_carrier_info_type[]
  update_rl_ptr       - Pointer to the 
                        hdrrmac_cmd_update_rl_carriers_struct_type

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_chan_enter_add_state
(
  uint8                                       assigned_chan_idx,
  hdrrmac_cmd_update_rl_carriers_struct_type  *update_rl_ptr
)
{
  hdrrmac4_phy_chan_vars_struct_type  *phy_chan_ptr;
    /* convenience pointer */

  hdrrmac_rl_carrier_info_type        *rl_carrier_ptr;
    /* convenience pointer */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rl_carrier_ptr = &update_rl_ptr->rl_carrier[assigned_chan_idx];

  phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[rl_carrier_ptr->rl_chan_index];

  /* Check if the channel is in DELETE state */
  if (phy_chan_ptr->chan_state == HDRRMAC4_CHAN_DELETE_STATE)
  {
    /* Trigger event to report carrier state change */
    HDRTRACE_RMAC_CARRIER_STATE_CHANGED( 
      rl_carrier_ptr->rl_chan_index,
      (int32) phy_chan_ptr->chan_record.band,
      phy_chan_ptr->chan_record.chan_num,
      phy_chan_ptr->chan_state,
      HDRRMAC4_CHAN_ADD_DELETE_STATE);

    /* Change the channel state to ADD_DELETE state */
    phy_chan_ptr->chan_state = HDRRMAC4_CHAN_ADD_DELETE_STATE;
  }
  else
  {
    /* Trigger event to report carrier state change */
    HDRTRACE_RMAC_CARRIER_STATE_CHANGED( 
      rl_carrier_ptr->rl_chan_index,
      (int32) phy_chan_ptr->chan_record.band,
      phy_chan_ptr->chan_record.chan_num,
      phy_chan_ptr->chan_state,
      HDRRMAC4_CHAN_ADD_STATE);

    /* Change the channel state to ADD state */
    phy_chan_ptr->chan_state = HDRRMAC4_CHAN_ADD_STATE;
  }

#ifdef FEATURE_MEMCPY_REMOVAL
  /* record the channel information */
  phy_chan_ptr->chan_record = rl_carrier_ptr->channel_record;

  /* record subaset information */
  phy_chan_ptr->subaset_info = 
             update_rl_ptr->subaset_info[rl_carrier_ptr->subaset_index];
#else
  /* record the channel information */
  memcpy (&phy_chan_ptr->chan_record,
          &rl_carrier_ptr->channel_record,
          sizeof(phy_chan_ptr->chan_record));

  /* record subaset information */
  memcpy (&phy_chan_ptr->subaset_info,
          &update_rl_ptr->subaset_info[rl_carrier_ptr->subaset_index],
          sizeof(phy_chan_ptr->subaset_info));
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* record dropping rank, make the prev != curr to force a report  */
  phy_chan_ptr->dropping_rank.curr = rl_carrier_ptr->drop_rank;
  phy_chan_ptr->dropping_rank.prev = rl_carrier_ptr->drop_rank - 1;

  return;

} /* hdrrmac4_chan_enter_add_state */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CHAN_ENTER_SETUP_STATE                          INTERNAL

DESCRIPTION
  This function transitions the specified reverse physical channel into 
  SETUP state.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_chan_enter_setup_state
(
  uint8 phy_chan_idx
)
{
  hdrrmac4_phy_chan_vars_struct_type  *phy_chan_ptr;
    /* convenience pointer */

#ifdef FEATURE_FACTORY_TESTMODE
  /* Channel mask to pass to RMACDRV to start channels */
  uint16  chan_mask = 0;
#endif 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[phy_chan_idx];

  /* Trigger event to report carrier state change */
  HDRTRACE_RMAC_CARRIER_STATE_CHANGED( 
    phy_chan_idx,
    (int32) phy_chan_ptr->chan_record.band,
    phy_chan_ptr->chan_record.chan_num,
    phy_chan_ptr->chan_state,
    HDRRMAC4_CHAN_SETUP_STATE);

  /* decrement the total number of INACTIVE channels */
  if (phy_chan_ptr->chan_state == HDRRMAC4_CHAN_ADD_STATE)
  {
    hdrrmac4_var_ptr->num_inactive_state_chans--;
  }

  /* Change the channel state to SETUP state */
  phy_chan_ptr->chan_state = HDRRMAC4_CHAN_SETUP_STATE;

#ifdef FEATURE_FACTORY_TESTMODE
  if (( ftm_get_mode() == FTM_MODE ))
  {
    /* Do not start Reverse Acquisition timer in FTM mode */

    /* Transition the channel state to SETUP in RMACDRV before 
       transitioning the channel to OPEN state */
    chan_mask |= (0x1 << phy_chan_idx);
    (void)hdrrmac4drv_start_channels(chan_mask, FALSE);

    /* Assign logical channel to the physical channel.
       It is assumed here that it is okay to assign a logical channel 
       with an id same as the physical channel to a physical channel. */
    hdrrmac4_var_ptr->log_chan[phy_chan_idx].phy_chan_id = phy_chan_idx;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].log_chan_id = phy_chan_idx;

    /* Transition the channel directly to OPEN state without 
       waiting for an RTCAck */
    hdrrmac4_chan_enter_open_state(phy_chan_idx);

    /* Increment the count of number of allocated channels */
    hdrrmac4_carrier_req_var.num_chans_allocated++;

    ASSERT(hdrrmac4_carrier_req_var.num_chans_allocated <= 
      HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );
  }
  else
  {
#endif /* FEATURE_FACTORY_TESTMODE */

    /* start the ReverseAcquisition timer for this channel */
    hdrrmac4_start_rl_acq_timer(phy_chan_idx);

#ifdef FEATURE_FACTORY_TESTMODE
  }
#endif /* FEATURE_FACTORY_TESTMODE */

  return;

} /* hdrrmac4_chan_enter_setup_state */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CHAN_ENTER_OPEN_STATE                         INTERNAL

DESCRIPTION
  This function transitions the specified reverse physical channel into
  OPEN state.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_chan_enter_open_state
(
  uint8 phy_chan_idx
)
{

  hdrrmac4_phy_chan_vars_struct_type *chan_ptr;
    /* convenience pointer */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Trigger event to report carrier state change */
  HDRTRACE_RMAC_CARRIER_STATE_CHANGED( 
    phy_chan_idx,
    (int32) hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_record.band,
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_record.chan_num,
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state,
    HDRRMAC4_CHAN_OPEN_STATE);

  chan_ptr = &hdrrmac4_var_ptr->phy_chan[phy_chan_idx];

  /* Sanity check, this channel should currently be in the SETUP state */
  if (chan_ptr->chan_state == HDRRMAC4_CHAN_SETUP_STATE)
  {

    chan_ptr->chan_state = HDRRMAC4_CHAN_OPEN_STATE;

    /* increment the total number of channels that are in the OPEN state */
    hdrrmac4_var_ptr->num_open_state_chans++;

#ifdef FEATURE_FACTORY_TESTMODE
    if ( ftm_get_mode() != FTM_MODE )
    {
#endif /* FEATURE_FACTORY_TESTMODE */

      /* Cancel this channel's RL ACQ timer */
      hdrrmac4_stop_rl_acq_timer(phy_chan_idx);

#ifdef FEATURE_FACTORY_TESTMODE
    }
#endif /* FEATURE_FACTORY_TESTMODE */

    /* Perform any low level open state config */
    hdrrmac4drv_start_open_state
      (phy_chan_idx,
       &hdrrmac4_config_ptr->channel[chan_ptr->log_chan_id]);

    /* 
     * If this is the only channel in the OPEN state, generate a LinkAcquired
     * inidcation.
     */
    if (hdrrmac4_var_ptr->num_open_state_chans == 1)
    {
      /* The overall MAC state now is OPEN */
      hdrrmac_enter_state( HDRRMAC_OPEN_STATE );

      hdrind_give_ind( HDRIND_RTCMAC_LINK_ACQUIRED, NULL );

      /* Report AGC on F3 here */
      hdrtx_report_agc( 0 );

      /* Set DRC mac rate to max rate */
      hdrfmac_set_traffic_max_drc();
    }

    /* Init Bucket for all Activated MAC flows */
    hdrrmac4_init_bucket_for_new_rl(phy_chan_idx);

    /* Refresh open channel list */
    hdrrmac4_init_open_chan_list(TRUE);

    /* Set MARQ outstanding packets flag to FALSE. */
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].marq_outstanding_pkts = FALSE;

  }
  else
  {
    /* Don't do any processing */
    HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                    "rl chan %d enter OPEN state from invalid state %d",
                    phy_chan_idx,
                    chan_ptr->chan_state );
  }

  return;

} /* hdrrmac4_chan_enter_open_state */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CHAN_ENTER_DELETE_STATE                           INTERNAL

DESCRIPTION
  This function transitions the specified reverse physical channel into
  DELETE state.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_chan_enter_delete_state
(
  uint8 phy_chan_idx
)
{
  /* Trigger event to report carrier state change */
  HDRTRACE_RMAC_CARRIER_STATE_CHANGED( 
    phy_chan_idx,
    (int32) hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_record.band,
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_record.chan_num,
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state,
    HDRRMAC4_CHAN_DELETE_STATE);

  /* If the channel is currently in OPEN state, need to decrement the count */
  if (HDRRMAC4_IS_RL_OPEN(phy_chan_idx))
  {
    hdrrmac4_var_ptr->num_open_state_chans--;
  }

  hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state =
    HDRRMAC4_CHAN_DELETE_STATE;

  /* MARQ outstanding packets on the channel during the next subframe
     processing */
  hdrrmac4_var_ptr->phy_chan[phy_chan_idx].marq_outstanding_pkts = TRUE;

  /* Notify driver */
  hdrrmac4drv_set_chan_removed_by_tca(phy_chan_idx);

  return;

} /* hdrrmac4_chan_enter_delete_state */



/*===========================================================================
FUNCTION HDRRMAC4_INIT_FLOWS                                         INTERNAL

DESCRIPTION
  This function initializes RMAC4 data structures

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_init_flows( void )
{

  uint8 flow;
    /* flow loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++ )
  {
    /* zero out the entire structure */
    memset (&hdrrmac4_var_ptr->data_flow[flow],
            0,
            sizeof(hdrrmac4_flow_vars_struct_type));
  }

  return;

} /* hdrrmac4_init_flows */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_INIT_CHANS                                         INTERNAL

DESCRIPTION
  This function initializes RMAC4 channel data structures

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_init_chans( void )
{

  uint8 chan;
    /* channel loop index */

  uint8 flow;
    /* flow loop index */

  hdrrmac4_phy_chan_vars_struct_type *phy_chan_ptr;
    /* convenience pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  for( chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++ )
  {
    phy_chan_ptr = &(hdrrmac4_var_ptr->phy_chan[chan]);

    /* zero out the entire structure */
    memset(phy_chan_ptr,
           0,
           sizeof(hdrrmac4_phy_chan_vars_struct_type));

    /* Set physical to logical channel associations to invalid value */
    phy_chan_ptr->log_chan_id = HDRRMAC4_INVALID_CHAN_ID;

    phy_chan_ptr->curr_tx_pkt_id = HDRRMAC4DRV_IDLE_PACKET;

    phy_chan_ptr->pkt_size_index = HDRRMACCMN_0_BITS;

    /* No limit from RTAP on targte PS */
    phy_chan_ptr->rtap_tgt_ps_index = HDRRMACCMN_IS856_A_RRI;

    for (flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++)
    {
      phy_chan_ptr->flow[flow].qrab_flow = HDRRMAC4DRV_QRAB_UNLOADED;
    }

    /* 
     * BucketLevelMax for Flows 0 & 1 has non-zero values (enabled by
     * default).  Initialize these here so the flows are enabled as soon
     * as the RL is open.
     */
    phy_chan_ptr->flow[0].bucket.bucket_level_sat =
      hdrrmac4_config_ptr->flow[0].bucket_level_max;
    phy_chan_ptr->flow[1].bucket.bucket_level_sat =
      hdrrmac4_config_ptr->flow[1].bucket_level_max;

    /* Reset flag to MARQ outstanding packets.
     * The line is kept only for clarity's sake.  It is not needed here 
     * since the entire structure is zeroed out.
     */
    phy_chan_ptr->marq_outstanding_pkts = FALSE;

  }

  /* Init. number of channels in INACTIVE state */
  hdrrmac4_var_ptr->num_inactive_state_chans =
    HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;

  hdrrmac4_var_ptr->num_open_state_chans = 0;

  for ( chan = 0; chan < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS; chan++)
  {
    /* invalidate the logical to physical channel associations */
    hdrrmac4_var_ptr->log_chan[chan].phy_chan_id = HDRRMAC4_INVALID_CHAN_ID;
  }

  /* init. the channel lists */
  memset((void *)&hdrrmac4_open_rl_list[0],
         HDRRMAC4_INVALID_CHAN_ID,
         sizeof(hdrrmac4_open_rl_list));
  memset((void *)&hdrrmac4_ranked_rl_list[0],
         HDRRMAC4_INVALID_CHAN_ID,
         sizeof(hdrrmac4_ranked_rl_list));

  return;

} /* hdrrmac4_init_chans */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_INIT_VERNO_OFFSETS                                    LOCAL

DESCRIPTION
  This function is used to initialize the arrays storing the attribute offsets
  information.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_init_verno_offsets(void)
{

  uint8 attrib_index;
    /* Loop Attribute Index */

  uint16  verno_offset;
    /* Attrib verno offset */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* non-NCX GAUPABLE attributes */
  for ( attrib_index = 0; 
        attrib_index < HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS;
        ++attrib_index )
  {
    if (hdrscmrmac4_get_attrib_verno_index(
          hdrrmac4_gaup_attrib_ids[attrib_index],
          &verno_offset) == E_SUCCESS)
    {
      hdrrmac4_gaup_non_ncx_attrib_verno_indices[attrib_index] = 
        verno_offset;
    }
    else
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Get non-ncx attrib verno index error a_id=0x%x",
                     hdrrmac4_gaup_attrib_ids[attrib_index]);
    }
  }

  /* per-flow GAUPABLE attributes */
  for ( attrib_index = 0; 
        attrib_index < HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS;
        ++attrib_index )
  {
    if (hdrscmrmac4_get_attrib_verno_index(
          hdrrmac4_gaup_nn_attrib_ids[attrib_index],
          &verno_offset) == E_SUCCESS)
    {
      hdrrmac4_gaup_nn_attrib_verno_indices[attrib_index] = 
        verno_offset;
    }
    else
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Get nn attrib verno index error a_id=0x%x",
                     hdrrmac4_gaup_nn_attrib_ids[attrib_index]);
    }
  }

  /* per-reverse-logical-channel GAUPABLE attributes */
  for ( attrib_index = 0; 
        attrib_index < HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS;
        ++attrib_index )
  {
    if (hdrscmrmac4_get_attrib_verno_index(
          hdrrmac4_gaup_cc_attrib_ids[attrib_index],
          &verno_offset) == E_SUCCESS)
    {
      hdrrmac4_gaup_cc_attrib_verno_indices[attrib_index] = 
        verno_offset;
    }
    else
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Get cc attrib verno index error a_id=0x%x",
                     hdrrmac4_gaup_cc_attrib_ids[attrib_index]);
    }
  }

  /* per-configuration GAUPABLE attributes */
  for ( attrib_index = 0; 
        attrib_index < HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS;
        ++attrib_index )
  {
    if (hdrscmrmac4_get_attrib_verno_index(
          hdrrmac4_gaup_xx_attrib_ids[attrib_index],
          &verno_offset) == E_SUCCESS)
    {
      hdrrmac4_gaup_xx_attrib_verno_indices[attrib_index] = 
        verno_offset;
    }
    else
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Get xx attrib verno index error a_id=0x%x",
                     hdrrmac4_gaup_xx_attrib_ids[attrib_index]);
    }
  }

  /* per-nc GAUPABLE attributes */
  for ( attrib_index = 0; 
        attrib_index < HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS;
        ++attrib_index )
  {
    if (hdrscmrmac4_get_attrib_verno_index(
          hdrrmac4_gaup_nc_attrib_ids[attrib_index],
          &verno_offset) == E_SUCCESS)
    {
      hdrrmac4_gaup_nc_attrib_verno_indices[attrib_index] = 
        verno_offset;
    }
    else
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Get nc attrib verno index error a_id=0x%x",
                     hdrrmac4_gaup_nc_attrib_ids[attrib_index]);
    }
  }

  return;

} /* hdrrmac4_init_verno_offsets */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_DATA_TOKEN_BUCKET_TO_OCTETS                           LOCAL

DESCRIPTION
  This function calculates floor(2^(x/12+4)-15) operation.

DEPENDENCIES
  None.

PARAMETERS
  data_token - Value specified in the DataTokenBucket attribute.

RETURN VALUE
  Number of octets

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL uint32 hdrrmac4_data_token_bucket_to_octets
(
  uint8 data_token
)
{
  uint32  round_num;
  uint32  fraction_num;
  static const uint32 two_to_the_power_of_12th[]={
    67108864, 71099364,   75327152,   79806338,   84551870,   89579586, 
    94906265, 100549685,  106528681,  112863206,  119574401,  126684665 };

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  round_num     = data_token / 12;
  fraction_num  = data_token % 12;

  return ((two_to_the_power_of_12th[fraction_num] >> (22 - round_num)) - 15);

} /* hdrrmac4_data_token_bucket_to_octets */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_TRANSLATE_DATA_TOKEN_BUCKET                           LOCAL

DESCRIPTION
  This function is used to translate the DataTokenBucket attribute from OTA
  format to SW format.

  This function calculates floor(2^(x/12+4)-15) operation.

DEPENDENCIES
  None.

PARAMETERS
  rmac_data_token_bucket_ptr  - pointer to the DataTokenBucket in RMAC SW
                                internal format
  scm_data_token_bucket_ptr   - pointer to the DataTokenBucket in OTA format

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_translate_data_token_bucket
(
  hdrrmac4_data_token_bucket_struct_type    *rmac_data_token_bucket_ptr,
  hdrscmrmac4_data_token_bucket_struct_type *scm_data_token_bucket_ptr
)
{
  /* DataTokenInflow */
  if (scm_data_token_bucket_ptr->data_token_inflow == 
      HDRSCMRMAC4_DATA_TOKEN_INFLOW_INFINITE)
  {
    rmac_data_token_bucket_ptr->data_token_inflow =
      HDRRMAC4_DATA_TOKEN_INFLOW_INFINITE;
  }
  else
  {
    rmac_data_token_bucket_ptr->data_token_inflow =
      hdrrmac4_data_token_bucket_to_octets (
        scm_data_token_bucket_ptr->data_token_inflow);
  }

  /* DataBucketLevelMax */
  if (scm_data_token_bucket_ptr->data_bucket_level_max ==
      HDRSCMRMAC4_DATA_BUCKET_LEVEL_MAX_INFINITE)
  {
    rmac_data_token_bucket_ptr->data_bucket_level_max =
      HDRRMAC4_DATA_BUCKET_LEVEL_MAX_INFINITE;
  }
  else
  {
    rmac_data_token_bucket_ptr->data_bucket_level_max =
      hdrrmac4_data_token_bucket_to_octets (
        scm_data_token_bucket_ptr->data_bucket_level_max);
  }

  return;

} /* hdrrmac4_translate_data_token_bucket */

 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_TRANSLATE_RRI_CHAN_POWER_PARAMS

DESCRIPTION
  This function is used to translate the RRIChannelPowerParameters from OTA 
  format to SW format.

  1. RRIChannelGainPreTransition
     OTAFormat: 4 bit 2's complement.
     SW Format: int8

  2. RRIChannelGainPostTransition
     OTAFormat: 4 bit 2's complement.
     SW Format: int8

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_translate_rri_chan_power_params ( void )
{
  uint8 t2p_trans = 0;
  /* T2P Transition(less one) in subframes used as array index for 
     RRIChannelPowerParams */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* RRIChannelPowerParameters */
  for ( t2p_trans=0; t2p_trans<HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS; ++t2p_trans )
  {
    /* RRIChannelGainPreTransition<t2p_trans> */
    hdrrmac4_config_ptr->rri_chan_power_params[t2p_trans].
      rri_chan_gain_pre_transition = 
      HDRRMACCMN_SIGN_EXTEND_4_TO_8(
        hdrrmac4_scm_cfg_ptr->rri_chan_power_params[t2p_trans].
          rri_chan_gain_pre_transition);

    /* RRIChannelGainPostTransition<t2p_trans> */
    hdrrmac4_config_ptr->rri_chan_power_params[t2p_trans].
      rri_chan_gain_post_transition = 
      HDRRMACCMN_SIGN_EXTEND_4_TO_8(
        hdrrmac4_scm_cfg_ptr->rri_chan_power_params[t2p_trans].
          rri_chan_gain_post_transition);

  } /* for */

} /* hdrrmac4_translate_rri_chan_power_params */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_TRANSLATE_CONFIG_PARAMS                            LOCAL

DESCRIPTION
  This function is used to translate the configuration parameters from OTA
  format to SW format.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_translate_config_params(void)
{

  uint8 flow_id = 0;
    /* Flow index */

  uint8 chan_id = 0;
    /* Reverse Logical Channel Index */

  uint8 config_id = 0;
    /* Config index */

  uint8 t2p_boost_gain = 0;
    /* Boost gain to use if needed. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------
                          NON-PER-FLOW SIMPLE ATTRIBS
  -------------------------------------------------------------------------*/

  /* MaxPacketSize */
#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
  #error code not present
#else
  hdrrmac4_config_ptr->max_packet_size =
    hdrrmac4_max_packet_size_lut[hdrrmac4_scm_cfg_ptr->max_packet_size];
#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */

  /* SupportGAUPMaxNumSubPackets */
  hdrrmac4_config_ptr->is_max_sub_pkts_gaupable = 
    hdrrmaccmn_is_max_sub_pkts_gaupable
      [hdrrmac4_scm_cfg_ptr->gaup_max_num_sub_pkts];

  /* FRABFilterTimeConstant in OTA index */
  hdrrmac4_config_ptr->filt_time_const.frab =
    hdrrmac4_scm_cfg_ptr->frab_filter_tc;

  /* Pilot Strength Filter Time Constant in OTA index */
  hdrrmac4_config_ptr->filt_time_const.pilot_strength =
    hdrrmac4_scm_cfg_ptr->ps_filter_tc;

  /* QRAB Filter time constant in OTA index */
  hdrrmac4_config_ptr->filt_time_const.qrab = 
    hdrrmac4_scm_cfg_ptr->qrab_filter_tc;

  /* T2PNoTxFilter Time Constant in subframes */
#ifndef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
  hdrrmac4_config_ptr->filt_time_const.t2p_no_tx = 
    hdrrmaccmn_tx_t2p_no_tx_filter_tc
      [hdrrmac4_scm_cfg_ptr->t2p_no_tx_filter_tc];
#else
  /* Overwrite no tx filter tc for tuneaway
     optimization */
  hdrrmac4_config_ptr->filt_time_const.t2p_no_tx = 
    hdrrmaccmn_tx_t2p_no_tx_filter_tc
    [HDRRMAC4_T2P_NO_TX_FILTER_TC_IDX];
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */


  /* FRABLow from LUT */
  hdrrmac4_config_ptr->flow_elig_thresh.frab_low =
    hdrrmaccmn_frab_low[hdrrmac4_scm_cfg_ptr->frab_low];

  /* Merge Threshold in Octets */
  hdrrmac4_config_ptr->flow_elig_thresh.merge_thresh =
    hdrrmaccmn_merge_thresh[hdrrmac4_scm_cfg_ptr->merge_thresh];

  /* Payload Threshold in SW enum index */
  hdrrmac4_config_ptr->payload_thresh = 
    hdrrmaccmn_payload_thresh[hdrrmac4_scm_cfg_ptr->payload_thresh];

  /* Max RL pilot power differential threshold in Q.10 dB units. */
  hdrrmac4_config_ptr->max_rl_pilot_pwr_diff = 
    (int32) (hdrrmac4_scm_cfg_ptr->max_rl_pilot_tx_pwr_diff << 10);
  
  /*------------------------------------------------------------------------
                          NON-PER-FLOW COMPLEX ATTRIBS
  -------------------------------------------------------------------------*/

  /* AuxiliaryPilotChannelParameters */
  hdrrmaccmn_translate_aux_pilot_chan_params(
    &hdrrmac4_config_ptr->aux_pilot_info,
    &hdrrmac4_scm_cfg_ptr->aux_pilot_info);

  /* PermittedPayload */
  hdrrmaccmn_translate_permitted_payload(
    (hdrrmaccmn_permitted_payload_size_type*)hdrrmac4_config_ptr->permitted_payload,
    (hdrscmrmaccmn_permitted_payload_type*)hdrrmac4_scm_cfg_ptr->permitted_payload);

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  t2p_boost_gain = 
    hdrrmaccmn_validate_boost_gain( hdrrmac4_scm_cfg_ptr->power_params,
                                    hdrrmac4_var_ptr->t2p_boost.gain);

  /* Enable/disable based on gain */
  if ( t2p_boost_gain == 0 )
  {
    hdrrmac4_var_ptr->t2p_boost_enabled = FALSE;
  }
  else
  {
    hdrrmac4_var_ptr->t2p_boost_enabled = TRUE;
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
  
  hdrrmaccmn_calc_t2p_info( hdrrmac4_scm_cfg_ptr->power_params,
                            &hdrrmac4_config_ptr->aux_pilot_info,
                            hdrrmac4_config_ptr->t2p_info,
                            t2p_boost_gain,
                            hdrrmac4_var_ptr->t2p_boost.deboost_gain,
                            hdrrmac4_var_ptr->t2p_boost.tx_t2p_info );

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
  /* Calculate minimum term target across both transmission modes
     and all packet sizes*/
  hdrrmac4_var_ptr->min_term_target = hdrrmac4_get_min_term_target();
#endif  /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */


  /* RequestParameters */
  hdrrmaccmn_translate_req_params( &hdrrmac4_config_ptr->req_params,
                                   &hdrrmac4_scm_cfg_ptr->req_params );

  /* RRIChannelPowerParameters */
  hdrrmac4_translate_rri_chan_power_params();

  /* CommonPowerParameters */
  /* AllocationStagger's OTA format is scaled by Q.4 and SW needs it in Q.4. */
  hdrrmac4_config_ptr->common_pow_params.alloc_stagger =
    hdrrmac4_scm_cfg_ptr->common_pow_params.alloc_stagger;

  /* TxT2PMin in dB scaled by Q.2 */
  hdrrmac4_config_ptr->common_pow_params.tx_t2p_min_db =
    hdrrmac4_scm_cfg_ptr->common_pow_params.tx_t2p_min_db;

  /* RPCStep in dB scaled by Q.2 from LUT */
  hdrrmac4_config_ptr->common_pow_params.rpc_step_db =
    hdrrmaccmn_com_pow_rpc_step
      [hdrrmac4_scm_cfg_ptr->common_pow_params.rpc_step_db];

  /*------------------------------------------------------------------------
                          PER-FLOW ATTRIBUTES
  -------------------------------------------------------------------------*/

  for ( flow_id = 0; flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++ flow_id)
  {

    /*------------------------------------------------------------------------
                            PER-FLOW SIMPLE ATTRIBS
    -------------------------------------------------------------------------*/

    hdrrmaccmn_translate_bucket_level_max_nn(
      &hdrrmac4_config_ptr->flow[flow_id].bucket_level_max,
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].bucket_level_max);

    /* For flow 0, a threshold of 4 indicates infinity. where as for all 
       other flows 6 indicates infinity. Infinity is denoted by '65535' in 
       LUT and is present at index 6. Handle Flow 0's infinity as a special 
       case */
    if ((flow_id == 0) && 
        (hdrrmac4_scm_cfg_ptr->flows[flow_id].merge_thresh ==
          HDRSCMRMACCMN_INFINITE_MERGE_THRESHOLD_00))
    {
      /* Infinite Merge threshold in Octets from Lookup table */
      hdrrmac4_config_ptr->flow[flow_id].merge_thresh = 
        hdrrmaccmn_pf_merge_thresh[6];
    }
    else
    {
      /* Merge threshold in Octets from Lookup table */
      hdrrmac4_config_ptr->flow[flow_id].merge_thresh = 
        hdrrmaccmn_pf_merge_thresh
          [hdrrmac4_scm_cfg_ptr->flows[flow_id].merge_thresh];
    }

    /* TxMode enum assignment. {0,1} => {HiCap, LoLaT} */
    hdrrmac4_config_ptr->flow[flow_id].tx_mode = 
      (hdrrmaccmn_tx_mode_enum_type)
      hdrrmac4_scm_cfg_ptr->flows[flow_id].tx_mode;

    /* QRABSelect enum. {0,1} => {QRAB, QRABps} */
    hdrrmac4_config_ptr->flow[flow_id].qrab_select = 
      (hdrrmaccmn_qrab_select_enum_type)
      hdrrmac4_scm_cfg_ptr->flows[flow_id].qrab_select;

    /* BurstDurationFactor in Subframes from LUT */
    hdrrmac4_config_ptr->flow[flow_id].burst_duration_factor = 
      hdrrmaccmn_pf_burst_duration_factor
        [hdrrmac4_scm_cfg_ptr->flows[flow_id].burst_duration_factor];

    /* T2PFilterTC in subframes from LUT */
    hdrrmac4_config_ptr->flow[flow_id].t2p_filt_tc = hdrrmaccmn_pf_t2p_filt_tc
      [hdrrmac4_scm_cfg_ptr->flows[flow_id].t2p_filt_tc];

    /*------------------------------------------------------------------------
                            PER-FLOW COMPLEX ATTRIBS
    -------------------------------------------------------------------------*/

    /* BucketFactorNN */
    hdrrmaccmn_translate_bucket_factor_nn(
      &hdrrmac4_config_ptr->flow[flow_id].bucket_factor,
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].bucket_factor);

    /* Update BucketFactor grid */
    hdrrmac4_update_bucket_factor_grid( flow_id );

    /* AssociatedFlowsNN */
    hdrrmaccmn_translate_associated_flows_nn(
      flow_id,
      &hdrrmac4_config_ptr->flow[flow_id].flow_mapping,
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].flow_mapping);

    /* DataTokenBucketNN */
    hdrrmac4_translate_data_token_bucket(
      &hdrrmac4_config_ptr->flow[flow_id].data_token_bucket,
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].data_token_bucket);

  } /* For each flow */

  /*------------------------------------------------------------------------
                          PER-CONFIGURATION ATTRIBUTES
  -------------------------------------------------------------------------*/

  for ( config_id = 0; config_id < HDRSCMRMAC4_MAX_NUM_CONFIGS; ++ config_id)
  {

    /*------------------------------------------------------------------------
                            PER-CONFIGURATION COMPLEX ATTRIBS
    -------------------------------------------------------------------------*/

    /* PilotStrengthConfigXX */
    hdrrmaccmn_translate_pilot_strength__config_xx(
      &hdrrmac4_config_ptr->config[config_id].pilotstrength,
      &hdrrmac4_scm_cfg_ptr->configurations[config_id].pilot_strength);

    /* TxT2PMaxConfigXX */
    hdrrmaccmn_translate_txt2pmax__config_xx(
      &hdrrmac4_config_ptr->config[config_id].txt2p_max,
      &hdrrmac4_scm_cfg_ptr->configurations[config_id].txt2p_max);

    /* T2PTransitionFunctionConfigXX */
    hdrrmaccmn_translate_gu_gd_nn__config_xx(
      &hdrrmac4_config_ptr->config[config_id].gu_gd,
      &hdrrmac4_scm_cfg_ptr->configurations[config_id].gu_gd);

    /* T2PInflowRangeConfigXX */
    hdrrmaccmn_translate_t2p_inflow_range_nn__config_xx(
      &hdrrmac4_config_ptr->config[config_id].t2p_inflow_range,
      &hdrrmac4_scm_cfg_ptr->configurations[config_id].t2p_inflow_range);

  } /* For each config */

  /*------------------------------------------------------------------------
                          PER-REVERSE LOGICAL CHANNEL ATTRIBUTES
  -------------------------------------------------------------------------*/

  for ( chan_id = 0; chan_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS; ++ chan_id)
  {

    /*------------------------------------------------------------------------
                            PER-CHANNEL SIMPLE ATTRIBS
    -------------------------------------------------------------------------*/

    /* ARQModeCC */
    hdrrmac4_config_ptr->channel[chan_id].arq_mode = 
      (hdrrmaccmn_arq_mode_enum_type)
      hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].arq_mode;

    /* PilotStrengthQRABThresholdDRC<Lock/Unlock> */
    hdrrmac4_config_ptr->channel[chan_id].ps_qrab_thresh.drc_lock =
      hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].
        ps_qrab_thresh_drc_lock;
    hdrrmac4_config_ptr->channel[chan_id].ps_qrab_thresh.drc_unlock =
      hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].
        ps_qrab_thresh_drc_unlock;

    /*------------------------------------------------------------------------
                            PER-CHANNEL COMPLEX ATTRIBS
    -------------------------------------------------------------------------*/

    /* AssociatedATConfigurationsCC */
    /* TxT2PMax */
    hdrrmac4_config_ptr->channel[chan_id].txt2p_max_ptr =
      &hdrrmac4_config_ptr->config
        [(hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].
          associated_at_config.tx_t2p_max_select)].
            txt2p_max;
    /* PilotStrength */
    hdrrmac4_config_ptr->channel[chan_id].pilotstrength_ptr =
      &hdrrmac4_config_ptr->config
        [(hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].
          associated_at_config.ps_select)].
            pilotstrength;

    /* MaxNumSubPackets */
    hdrrmaccmn_translate_max_num_sub_pkts__cc();

  } /* For each channel */

  /*------------------------------------------------------------------------
                          PER-NC ATTRIBUTE
  -------------------------------------------------------------------------*/

  for ( flow_id = 0; flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++ flow_id)
  {
    for ( chan_id = 0; chan_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS; ++ chan_id)
    {

      /*------------------------------------------------------------------------
                              PER-NC COMPLEX ATTRIBS
      -------------------------------------------------------------------------*/

      /* AssociatedFlowConfigurationsNC */
      /* T2PTransitionFunction */
      hdrrmac4_config_ptr->nc[flow_id][chan_id].gu_gd_ptr =
        &hdrrmac4_config_ptr->config
          [(hdrrmac4_scm_cfg_ptr->
              associated_flow_config[flow_id][chan_id].
                t2p_transition_func_select)].gu_gd;
      /* T2PInflowRange */
      hdrrmac4_config_ptr->nc[flow_id][chan_id].t2p_inflow_range_ptr =
        &hdrrmac4_config_ptr->config
          [(hdrrmac4_scm_cfg_ptr->
              associated_flow_config[flow_id][chan_id]).
                t2p_inflow_range_select].t2p_inflow_range;

    } /* For each channel */

  } /* For each flow */

  return;

} /* hdrrmac4_translate_config_params */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_RECORD_VERSION_NUMS                                LOCAL

DESCRIPTION
  The version number of an attribute is useful only if the attribute is 
  GAUPable. This function is used to record the version numbers of 
  GAUPable attributes.

DEPENDENCIES
  None.

PARAMETERS
  hdrrmac4_verno - Pointer to RMAC4's SCM version number struct.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_record_version_nums
(
  hdrscmrmac4_verno_type hdrrmac4_verno
)
{

  uint8 g_attr_ind;
    /* GAUP attribute index in the version number array */

  uint8 flow_id;
    /* Flow ID */

  uint8 channel_id;
    /* Channel ID */

  uint8 config_id;
    /* Configuration ID */

  uint8 nn_g_attr_ind;
    /* NN GAUP attribute index */

  uint8 cc_g_attr_ind;
    /* CC GAUP attribute index */

  uint8 xx_g_attr_ind;
    /* XX GAUP attribute index */

  uint8 nc_g_attr_ind;
    /* NC GAUP attribute index */

  uint32 curr_nc_g_base_offset = 0;
    /* Current NC GAUP attrib base offset in verno array */

  uint32 curr_nn_g_attr_ind;
    /* Current Per-Flow GAUP attribute index */

  uint32 curr_cc_g_attr_ind;
    /* Current Per-Channel GAUP attribute index */

  uint32 curr_xx_g_attr_ind;
    /* Current Per-Configuration attribute index */

  uint32 curr_nc_g_attr_ind;
    /* Current Per-flow-per-channel attribute index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /*------------------------------------------------------------------------
        Store the version numbers of GAUPable Non Per-ncx attributes
  -------------------------------------------------------------------------*/

  /* For each non per-flow attribute */
  for (g_attr_ind = 0;
       g_attr_ind < HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS; 
       ++g_attr_ind)
  {
    /* Copy the attribute's version number from Master verno array */
    hdrrmac4_non_ncx_gaup_verno[g_attr_ind] = 
      hdrrmac4_verno[hdrrmac4_gaup_non_ncx_attrib_verno_indices[g_attr_ind]];
  }

  /*------------------------------------------------------------------------
          Store the version numbers of GAUPable per-flow attributes
  -------------------------------------------------------------------------*/

  /* For each per flow attribute */
  for (nn_g_attr_ind = 0; 
       nn_g_attr_ind < HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS; 
       ++nn_g_attr_ind)
  {
    /* For each MAC flow */
    for (flow_id=0; 
         flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; 
         ++flow_id)
    {
      /* Compute the index of NN GAUP attribute in the array for NN GAUP 
         attribs */
      curr_nn_g_attr_ind = nn_g_attr_ind * HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS 
                            + flow_id;

      /* Copy the attribute's version number from Master Verno array */
      hdrrmac4_nn_gaup_verno[curr_nn_g_attr_ind] = 
        hdrrmac4_verno[hdrrmac4_gaup_nn_attrib_verno_indices[nn_g_attr_ind] + 
                       flow_id];
    }
  }

  /*------------------------------------------------------------------------
          Store the version numbers of GAUPable per-Channel attributes
  -------------------------------------------------------------------------*/

  /* For each per channel attribute */
  for (cc_g_attr_ind = 0; 
       cc_g_attr_ind < HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS; 
       ++cc_g_attr_ind)
  {
    /* For each Reverse Logical Channel */
    for (channel_id=0; 
         channel_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS; 
         ++channel_id)
    {
      /* Compute the index of CC GAUP attribute in the array for CC GAUP 
         attribs */
      curr_cc_g_attr_ind = cc_g_attr_ind * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS 
                            + channel_id;

      /* Copy the attribute's version number from Master Verno array */
      hdrrmac4_cc_gaup_verno[curr_cc_g_attr_ind] = 
        hdrrmac4_verno[hdrrmac4_gaup_cc_attrib_verno_indices[cc_g_attr_ind] + 
                       flow_id];
    }
  }

  /*------------------------------------------------------------------------
          Store the version numbers of GAUPable per-Configuration attributes
  -------------------------------------------------------------------------*/

  /* For each per configuration attribute */
  for (xx_g_attr_ind = 0; 
       xx_g_attr_ind < HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS;
       ++xx_g_attr_ind)
  {
    /* For each configuration */
    for (config_id=0; 
         config_id < HDRSCMRMAC4_MAX_NUM_CONFIGS;
         ++config_id)
    {
      /* Compute the index of XX GAUP attribute in the array for XX GAUP 
         attribs */
      curr_xx_g_attr_ind = xx_g_attr_ind * HDRSCMRMAC4_MAX_NUM_CONFIGS + 
                            config_id;

      /* Copy the attribute's version number from Master Verno array */
      hdrrmac4_xx_gaup_verno[curr_xx_g_attr_ind] = 
        hdrrmac4_verno[hdrrmac4_gaup_xx_attrib_verno_indices[xx_g_attr_ind] + 
                       config_id];
    }
  }

  /*------------------------------------------------------------------------
          Store the version numbers of GAUPable per-flow-per-channel 
          attributes
  -------------------------------------------------------------------------*/

  /* For each per nc attribute */
  for (nc_g_attr_ind = 0; 
       nc_g_attr_ind < HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS; 
       ++nc_g_attr_ind)
  {
    curr_nc_g_base_offset = nc_g_attr_ind * HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS *
                            HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS;

    /* For each MAC flow */
    for (flow_id=0; 
         flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++flow_id)
    {
      curr_nn_g_attr_ind = ( curr_nc_g_base_offset + 
                             (flow_id * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS) );

      /* For each channel */
      for (channel_id = 0; 
           channel_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS; 
           ++channel_id)
      {
        /* Compute the index of NC GAUP attribute in the array for NC GAUP 
           attribs */
        curr_nc_g_attr_ind = curr_nn_g_attr_ind + channel_id;

        /* Copy the attribute's version number from Master Verno array */
        hdrrmac4_nc_gaup_verno[curr_nc_g_attr_ind] = 
          hdrrmac4_verno[(hdrrmac4_gaup_nc_attrib_verno_indices[nc_g_attr_ind] + 
                         (flow_id * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS) +
                         channel_id)];
      }
    }
  }

  return;

} /* hdrrmac4_record_version_nums */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_NON_NCX_ATTRIB_UPDATE                        LOCAL

DESCRIPTION
  This function is used to process a GAUP update to a Non-Per-NCX attribute.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - ID of the attribute that was just GAUPed

RETURN VALUE
  E_SUCCESS - If the attribute ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrerrno_enum_type hdrrmac4_process_non_ncx_attrib_update
(
  uint16 attrib_id
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Switch on the attribute ID and take appropriate action */
  switch(attrib_id)
  {
    /*------------------------------------------------------------------
                 GAUPABLE NON-PER-NCX SIMPLE ATTRIBUTES
    -------------------------------------------------------------------*/

    /* MaxRLPilotTxPwrDiff */
    case HDRSCMRMAC4_MAX_RL_PILOT_TX_PWR_DIFF_ATTRIB:
      hdrrmac4_config_ptr->max_rl_pilot_pwr_diff = 
        (int32) (hdrrmac4_scm_cfg_ptr->max_rl_pilot_tx_pwr_diff << 10);
      break;

    /* PayloadThreshold */
    case HDRSCMRMAC4_PAYLOAD_THRESH_ATTRIB:
      hdrrmac4_config_ptr->payload_thresh = 
        hdrrmaccmn_payload_thresh[hdrrmac4_scm_cfg_ptr->payload_thresh];
      break;

    /* MergeThreshold */
    case HDRSCMRMAC4_MERGE_THRESH_ATTRIB:
      hdrrmac4_config_ptr->flow_elig_thresh.merge_thresh = 
        hdrrmaccmn_merge_thresh[hdrrmac4_scm_cfg_ptr->merge_thresh];
      break;

    /*------------------------------------------------------------------
                  GAUPABLE NON-PER-FLOW COMPLEX ATTRIBUTES
    -------------------------------------------------------------------*/

    /* AuxiliaryPilotChannelParameters */
    case HDRSCMRMAC4_AUX_PILOT_CHAN_PARAMS_ATTRIB:
      hdrrmaccmn_translate_aux_pilot_chan_params(
        &hdrrmac4_config_ptr->aux_pilot_info,
        &hdrrmac4_scm_cfg_ptr->aux_pilot_info);
      break;

    /* PermittedPayload */
    case HDRSCMRMAC4_PERMITTED_PAYLOAD_ATTRIB:
      hdrrmaccmn_translate_permitted_payload(
        (hdrrmaccmn_permitted_payload_size_type*)hdrrmac4_config_ptr->permitted_payload,
        (hdrscmrmaccmn_permitted_payload_type*)hdrrmac4_scm_cfg_ptr->permitted_payload);
      break;

    /* PowerParameters */
    case HDRSCMRMAC4_POWER_PARAMS_128_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_256_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_512_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_768_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_1024_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_1536_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_2048_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_3072_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_4096_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_6144_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_8192_ATTRIB:
    case HDRSCMRMAC4_POWER_PARAMS_12288_ATTRIB:

      /* Only action required is to recalculate T2P information. Done later! */
      break;

    /* RequestParameters */
    case HDRSCMRMAC4_REQUEST_PARAMS_ATTRIB:
      hdrrmaccmn_translate_req_params(
        &hdrrmac4_config_ptr->req_params,
        &hdrrmac4_scm_cfg_ptr->req_params);
      break;

    case HDRSCMRMAC4_RRI_CHAN_POWER_PARAMS_ATTRIB:
      hdrrmac4_translate_rri_chan_power_params();
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Error proc GAUP; Inval non-pncx attr Id %d", 
                      attrib_id );
      err_no = E_DATA_INVALID;

  } /* switch */

  return err_no;

} /* hdrrmac4_process_non_ncx_attrib_update */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_NN_ATTRIB_UPDATE                        LOCAL

DESCRIPTION
  This function is used to process a GAUP update to a Per-Flow attribute.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - ID of the GAUPed attribute.
  flow_id   - MAC flow ID of the flow corresponding to this attribute.

RETURN VALUE
  E_SUCCESS - If the attribute ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrerrno_enum_type hdrrmac4_process_nn_attrib_update
(
  uint16 attrib_id, 
  uint8 flow_id
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  switch (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id))
  {
    /*------------------------------------------------------------------
                  GAUPABLE PER-FLOW SIMPLE ATTRIBUTES
    -------------------------------------------------------------------*/

    /* T2PFilterTCNN */
    case HDRSCMRMAC4_T2P_FILTER_TC_NN_BASE:
      /* T2PFilterTC in subframes from LUT */
      hdrrmac4_config_ptr->flow[flow_id].t2p_filt_tc = 
        hdrrmaccmn_pf_t2p_filt_tc
          [hdrrmac4_scm_cfg_ptr->flows[flow_id].t2p_filt_tc];
      break;

    /* BurstDurationFactorNN */
    case HDRSCMRMAC4_BURST_DURATION_FACT0R_NN_BASE:
      /* BurstDurationFactor in Subframes from LUT */
      hdrrmac4_config_ptr->flow[flow_id].burst_duration_factor = 
        hdrrmaccmn_pf_burst_duration_factor
          [hdrrmac4_scm_cfg_ptr->flows[flow_id].burst_duration_factor];
      break;

    /* QRABSelectNN */
    case HDRSCMRMAC4_QRAB_SELECT_NN_BASE:
      /* QRABSelect enum. {0,1} => {QRAB, QRABps} */
      hdrrmac4_config_ptr->flow[flow_id].qrab_select = 
        (hdrrmaccmn_qrab_select_enum_type)
        hdrrmac4_scm_cfg_ptr->flows[flow_id].qrab_select;
      break;

    /* TransmissionModeNN */
    case HDRSCMRMAC4_TRANSMISSION_MODE_NN_BASE:
      /* TxMode enum assignment. {0,1} => {HiCap, LoLaT} */
      hdrrmac4_config_ptr->flow[flow_id].tx_mode = 
        (hdrrmaccmn_tx_mode_enum_type)
        hdrrmac4_scm_cfg_ptr->flows[flow_id].tx_mode;
      break;

    /* MergeThresholdNN */
    case HDRSCMRMAC4_MERGE_THRESHOLD_NN_BASE:
      /* For flow 0, a threshold of 4 indicates infinity. where as for all 
         other flows 6 indicates infinity. Infinity is denoted by '65535' in 
         LUT and is present at index 6. Handle Flow 0's infinity as a special 
         case */
      if ((flow_id == 0) && 
          (hdrrmac4_scm_cfg_ptr->flows[flow_id].merge_thresh ==
            HDRSCMRMACCMN_INFINITE_MERGE_THRESHOLD_00)) 
      {
        /* Merge threshold in Octets from Lookup table */
        hdrrmac4_config_ptr->flow[flow_id].merge_thresh = 
          hdrrmaccmn_pf_merge_thresh[6];
      }
      else
      {
        /* Merge threshold in Octets from Lookup table */
        hdrrmac4_config_ptr->flow[flow_id].merge_thresh = 
          hdrrmaccmn_pf_merge_thresh
            [hdrrmac4_scm_cfg_ptr->flows[flow_id].merge_thresh];
      }
      break;

    /* BucketLevelMaxNN */
    case HDRSCMRMAC4_BUCKET_LEVEL_MAX_NN_BASE:

      hdrrmaccmn_translate_bucket_level_max_nn(
        &hdrrmac4_config_ptr->flow[flow_id].bucket_level_max,
        &hdrrmac4_scm_cfg_ptr->flows[flow_id].bucket_level_max);
      break;

    /*------------------------------------------------------------------
                    GAUPABLE PER-FLOW COMPLEX ATTRIBUTES
    -------------------------------------------------------------------*/

    /* BucketFactorNN */
    case HDRSCMRMAC4_BUCKET_FACTOR_NN_BASE:
      hdrrmaccmn_translate_bucket_factor_nn(
        &hdrrmac4_config_ptr->flow[flow_id].bucket_factor,
        &hdrrmac4_scm_cfg_ptr->flows[flow_id].bucket_factor);

      /* Update BucketFactor grid */
      hdrrmac4_update_bucket_factor_grid( flow_id );
      break;

    /* DataTokenBucketNN */
    case HDRSCMRMAC4_DATA_TOKEN_BUCKET_NN_BASE:
      hdrrmac4_translate_data_token_bucket(
        &hdrrmac4_config_ptr->flow[flow_id].data_token_bucket,
        &hdrrmac4_scm_cfg_ptr->flows[flow_id].data_token_bucket);
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Error proc GAUP; Inval pn attr Id %d", 
                      attrib_id );
      err_no = E_DATA_INVALID;

  } /* switch */

  return err_no;

} /* hdrrmac4_process_nn_attrib_update */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_CC_ATTRIB_UPDATE                        LOCAL

DESCRIPTION
  This function is used to process a GAUP update to a Per-Channel attribute.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - ID of the GAUPed attribute.
  chan_id   - ID of the Reverse Logical Channel corresponding to this 
              attribute.

RETURN VALUE
  E_SUCCESS - If the attribute ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrerrno_enum_type hdrrmac4_process_cc_attrib_update
(
  uint16 attrib_id, 
  uint8 chan_id
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  switch (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id))
  {
    /*------------------------------------------------------------------
                  GAUPABLE PER-CHANNEL SIMPLE ATTRIBUTES
    -------------------------------------------------------------------*/

    /* ARQModeCC */
    case HDRSCMRMAC4_ARQ_MODE_CC_BASE:
      hdrrmac4_config_ptr->channel[chan_id].arq_mode =
        (hdrrmaccmn_arq_mode_enum_type)
        hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].arq_mode;
      break;
  
    /* PilotStrengthQRABThresholdDRCLockCC */
    case HDRSCMRMAC4_PS_QRAB_THRESH_DRC_LOCK_CC_BASE:
      hdrrmac4_config_ptr->channel[chan_id].ps_qrab_thresh.drc_lock =
        hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].
          ps_qrab_thresh_drc_lock;
      break;
  
    /* PilotStrengthQRABThresholdDRCUnlockCC */
    case HDRSCMRMAC4_PS_QRAB_THRESH_DRC_UNLOCK_CC_BASE:
      hdrrmac4_config_ptr->channel[chan_id].ps_qrab_thresh.drc_unlock =
        hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].
          ps_qrab_thresh_drc_unlock;
      break;
  
    /*------------------------------------------------------------------
                  GAUPABLE PER-CHANNEL COMPLEX ATTRIBUTES
    -------------------------------------------------------------------*/

    /* AssociatedATConfigurationsCC */
    case HDRSCMRMAC4_ASSOCIATED_AT_CONFIG_CC_BASE:
      /* TxT2PMax */
      hdrrmac4_config_ptr->channel[chan_id].txt2p_max_ptr =
        &hdrrmac4_config_ptr->config
          [(hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].
            associated_at_config.tx_t2p_max_select)].txt2p_max;
      /* PilotStrength */
      hdrrmac4_config_ptr->channel[chan_id].pilotstrength_ptr =
        &hdrrmac4_config_ptr->config
          [(hdrrmac4_scm_cfg_ptr->reverse_channels[chan_id].
            associated_at_config.ps_select)].pilotstrength;
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Error proc GAUP; Inval pc attr Id %d", 
                      attrib_id );
      err_no = E_DATA_INVALID;

  } /* switch */

  return err_no;

} /* hdrrmac4_process_cc_attrib_update */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_XX_ATTRIB_UPDATE                        LOCAL

DESCRIPTION
  This function is used to process a GAUP update to a Per-Config attribute.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - ID of the GAUPed attribute.
  config_id - ID of the Configuration corresponding to this attribute.

RETURN VALUE
  E_SUCCESS - If the attribute ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrerrno_enum_type hdrrmac4_process_xx_attrib_update
(
  uint16 attrib_id, 
  uint8 config_id
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  switch (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id))
  {
    /*------------------------------------------------------------------
                  GAUPABLE PER-CONFIG COMPLEX ATTRIBUTES
    -------------------------------------------------------------------*/

    /* T2PInflowRangeConfigXX */
    case HDRSCMRMAC4_T2P_INFLOW_RANGE_CONFIG_XX_BASE:
      hdrrmaccmn_translate_t2p_inflow_range_nn__config_xx(
        &hdrrmac4_config_ptr->config[config_id].t2p_inflow_range,
        &hdrrmac4_scm_cfg_ptr->configurations[config_id].t2p_inflow_range);
      break;

    /* T2PTransitionFunctionConfigXX */
    case HDRSCMRMAC4_T2P_TRANSITION_FUNC_CONFIG_XX_BASE:
      hdrrmaccmn_translate_gu_gd_nn__config_xx(
        &hdrrmac4_config_ptr->config[config_id].gu_gd,
        &hdrrmac4_scm_cfg_ptr->configurations[config_id].gu_gd);
      break;

    /* PilotStrengthConfigXX */
    case HDRSCMRMAC4_PS_CONFIG_XX_BASE:
      hdrrmaccmn_translate_pilot_strength__config_xx(
        &hdrrmac4_config_ptr->config[config_id].pilotstrength,
        &hdrrmac4_scm_cfg_ptr->configurations[config_id].pilot_strength);
      break;

    /* TxT2PMaxConfigXX */
    case HDRSCMRMAC4_TX_T2P_MAX_CONFIG_XX_BASE:
      hdrrmaccmn_translate_txt2pmax__config_xx(
        &hdrrmac4_config_ptr->config[config_id].txt2p_max,
        &hdrrmac4_scm_cfg_ptr->configurations[config_id].txt2p_max);
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Error proc GAUP; Inval px attr Id %d", 
                      attrib_id );
      err_no = E_DATA_INVALID;

  } /* switch */

  return err_no;

} /* hdrrmac4_process_xx_attrib_update */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_NC_ATTRIB_UPDATE                        LOCAL

DESCRIPTION
  This function is used to process a GAUP update to a Per-NC attribute.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - ID of the GAUPed attribute.
  flow_id   - ID of the MAC flow corresponding to this attribute.
  chan_id   - ID of the Reverse Logical Channel corresponding to this 
              attribute.

RETURN VALUE
  E_SUCCESS - If the attribute ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrerrno_enum_type hdrrmac4_process_nc_attrib_update
(
  uint16 attrib_id,
  uint8 flow_id,
  uint8 chan_id
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

  uint8 phy_chan_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  switch (HDRSCMRMAC4_GET_NCX_ATTRIB_BASE(attrib_id))
  {
    /*------------------------------------------------------------------
                  GAUPABLE PER-NC COMPLEX ATTRIBUTES
    -------------------------------------------------------------------*/

    /* AssociatedFlowConfigurationsNC */
    case HDRSCMRMAC4_ASSOCIATED_FLOW_CONFIG_NC_BASE:
      /* T2PTransitionFunction */
      hdrrmac4_config_ptr->nc[flow_id][chan_id].gu_gd_ptr =
        &hdrrmac4_config_ptr->config
          [(hdrrmac4_scm_cfg_ptr->associated_flow_config[flow_id][chan_id].
            t2p_transition_func_select)].gu_gd;

      
      phy_chan_idx = hdrrmac4_from_log_to_phy(chan_id);
      if (phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
      {
        /* Populate T2PUp/Down grids. */
        hdrrmac4_update_gu_gd_grids(flow_id, phy_chan_idx);

        /* T2PInflowRange */
        hdrrmac4_config_ptr->nc[flow_id][chan_id].t2p_inflow_range_ptr =
          &hdrrmac4_config_ptr->config
            [(hdrrmac4_scm_cfg_ptr->associated_flow_config[flow_id][chan_id].
              t2p_inflow_range_select)].t2p_inflow_range;
      }
      else 
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
            "Invalid physical chan - process nc attrib update - %d",
            phy_chan_idx );
      }

      break;


    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Error proc GAUP; Inval non-pnc attr Id %d", 
                      attrib_id );
      err_no = E_DATA_INVALID;

  } /* switch */

  return err_no;

} /* hdrrmac4_process_nc_attrib_update */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_ATTRIB_UPDATE                                LOCAL

DESCRIPTION
  This function is used to process the GAUP update for RMAC4.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_process_attrib_update(void)
{
  static boolean is_changed[HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS] = 
                  {FALSE};
    /* Array of booleans specifying the attributes that changed because of 
       recent GAUP update */

  static boolean is_nn_changed
          [HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS * 
           HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS] = {FALSE};
    /* Array of booleans to specify the attribute that has changed because of 
       recent GAUP update */

  static boolean is_cc_changed
          [HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS * 
           HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS] = {FALSE};
    /* Array of booleans to specify the attribute that has changed because of 
       recent GAUP update */

  static boolean is_xx_changed
          [HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS * 
           HDRSCMRMAC4_MAX_NUM_CONFIGS] = {FALSE};
    /* Array of booleans to specify the attribute that has changed because of 
       recent GAUP update */

  static boolean is_nc_changed
    [HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS * 
     HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS * 
     HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS] = {FALSE};
    /* Array of booleans to specify the attribute that has changed because of 
       recent GAUP update */

  uint8 g_attr_ind = 0;
    /* GAUP Attribute Index */

  uint16 attrib_id = 0;
    /* GAUP attribute ID */

  uint8 flow_id = 0;
    /* MAC flow ID */

  uint8 chan_id = 0;
    /* Reverse Logical Channel ID */

  uint8 config_id = 0;
    /* Configuration ID */

  uint32 curr_nn_g_attr_ind = 0;
    /* Current NN gaupable Attribute's index */

  uint32 curr_nc_g_base_offset = 0;
    /* Current NC GAUP attrib base offset in verno array */

  uint32 curr_cc_g_attr_ind = 0;
    /* Current CC gaupable Attribute's index */

  uint32 curr_xx_g_attr_ind = 0;
    /* Current XX gaupable Attribute's index */

  uint32 curr_nc_g_attr_ind = 0;
    /* Current NC gaupable Attribute's index */

  uint32 bl_max_prev = 0;
    /* Previous copy of bucket Level max */

  boolean rmacdrv_update_needed = FALSE;
    /* Do we have to call hdrrmacdrv_update_attributes? */

  boolean calc_t2p_info_needed = FALSE;
    /* Do we need to calculate T2P info again? */

  boolean flow_mapping_changed = FALSE;
    /* Has the MAC-flow RLP-flow mapping changed? */

  uint8 t2p_boost_gain = 0;
    /* Boost gain to use if needed. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* Query SCM DB to find the changed Non-NN attributes */
  hdrscmdb_find_changed_attribs( HDRHAI_RTC_MAC_PROTOCOL,
                                 HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS,
                                 hdrrmac4_non_ncx_gaup_verno,
                                 hdrrmac4_gaup_non_ncx_attrib_verno_indices,
                                 is_changed );

  /*------------------------------------------------------------------------
    For Each attribute, if it was changed, read the attribute from DB and 
    take appropriate action.
  -------------------------------------------------------------------------*/
  for (g_attr_ind = 0; 
       g_attr_ind < HDRSCMRMAC4_NUM_GAUPABLE_NON_NCX_ATTRIBS; 
       ++g_attr_ind )
  {
    if (is_changed[g_attr_ind])
    {
      attrib_id = hdrrmac4_gaup_attrib_ids[g_attr_ind];

      HDR_MSG_PROT_1(MSG_LEGACY_MED, "RMAC4 attrib %x GAUPed", attrib_id);

      /* Get the Updated attribute value from the SCM Database */
      hdrrmac4_non_ncx_gaup_verno[g_attr_ind] = 
        hdrscmdb_get_inuse_attrib( HDRHAI_RTC_MAC_PROTOCOL,
                                   attrib_id, 
                                   hdrrmac4_non_ncx_gaup_attribs[g_attr_ind],
                                   HDRSCP_MC_RMAC );

      /* Process the update of the Non-Per-Flow attribute */
      (void) hdrrmac4_process_non_ncx_attrib_update(attrib_id);

      /* If we don't need to call hdrrmacdrv_update_attributes already... */
      if (!rmacdrv_update_needed)
      {
        /* Do we need to call hdrrmacdrv_update_attributes at the end? 
           If the attribute GAUPed was either one of the following attributes,
           we go ahead and call the function at the end. */ 
        rmacdrv_update_needed = 
          ( (attrib_id == (uint16) HDRSCMRMAC4_AUX_PILOT_CHAN_PARAMS_ATTRIB) ||
            ((attrib_id & HDRSCMRMAC4_NCX_BASE_MASK) == 
             (uint16) HDRSCMRMAC4_ARQ_MODE_CC_BASE)                          ||
            ((attrib_id & HDRSCMRMAC4_NCX_BASE_MASK) ==
             (uint16) HDRSCMRMAC4_PS_QRAB_THRESH_DRC_LOCK_CC_BASE)           ||
            ((attrib_id & HDRSCMRMAC4_NCX_BASE_MASK) ==
             (uint16) HDRSCMRMAC4_PS_QRAB_THRESH_DRC_UNLOCK_CC_BASE)         ||
            (attrib_id == (uint16) HDRSCMRMAC4_RRI_CHAN_POWER_PARAMS_ATTRIB) );
      }

      /* If we don't need to call hdrrmac4_calc_t2p_info already... */
      if (!calc_t2p_info_needed)
      {
        calc_t2p_info_needed =
          ((attrib_id == (uint16) HDRSCMRMAC4_AUX_PILOT_CHAN_PARAMS_ATTRIB) ||
           ((attrib_id >= (uint16) HDRSCMRMAC4_FIRST_POWER_PARAMS_ATTRIB) &&
            (attrib_id <= (uint16) HDRSCMRMAC4_LAST_POWER_PARAMS_ATTRIB)));
      }

    } /* If is_changed */
  }

  /* Query SCM DB to find the changed CC attributes 
     Some of the CC attribute changes will require drv_update_attributes */
  hdrscmdb_find_changed_cc_attribs( HDRHAI_RTC_MAC_PROTOCOL,
                                    HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS,
                                    HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS,
                                    hdrrmac4_cc_gaup_verno,
                                    hdrrmac4_gaup_cc_attrib_verno_indices,
                                    is_cc_changed );

  /*------------------------------------------------------------------------
    For Each attribute, if it was changed, read the attribute from DB and 
    take appropriate action.
  -------------------------------------------------------------------------*/

  /* For each per-channel attribute */
  for (g_attr_ind=0;
       g_attr_ind<HDRSCMRMAC4_NUM_GAUPABLE_CC_ATTRIBS; 
       ++g_attr_ind)
  {
    /* For each channel instance of the per-channel attribute */
    for (chan_id=0;
         chan_id<HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS;
         ++chan_id)
    {
      /* Compute the current attribute's index... */
      curr_cc_g_attr_ind = g_attr_ind * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS + 
                            chan_id;

      /* If the current attrib was just GAUPed */
      if (is_cc_changed[curr_cc_g_attr_ind])
      {
        /* Get the attribute ID from the base and channel ID */
        attrib_id = hdrrmac4_gaup_cc_attrib_ids[g_attr_ind] | chan_id;

        HDR_MSG_PROT_1(MSG_LEGACY_MED, "RMAC4 attrib %x GAUPed", attrib_id);

        /* Get the update attribute value from SCM database */
        hdrrmac4_cc_gaup_verno[curr_cc_g_attr_ind] =
          hdrscmdb_get_inuse_attrib(
            HDRHAI_RTC_MAC_PROTOCOL,
            attrib_id,
            hdrrmac4_cc_gaup_attribs[g_attr_ind][chan_id],
            HDRSCP_MC_RMAC);

        /* Process the update of the Per-Channel Attribute */
        (void) hdrrmac4_process_cc_attrib_update(attrib_id, chan_id);

        /* If we don't need to call hdrrmacdrv_update_attributes already... */
        if (!rmacdrv_update_needed)
        {
          attrib_id = hdrrmac4_gaup_cc_attrib_ids[g_attr_ind];

          /* Do we need to call hdrrmacdrv_update_attributes at the end? 
             If the attribute GAUPed was either one of the following attributes,
             we go ahead and call the function at the end. */ 
          rmacdrv_update_needed = 
            ( (attrib_id == (uint16) HDRSCMRMAC4_ARQ_MODE_CC_BASE) ||
              (attrib_id == (uint16) 
              HDRSCMRMAC4_PS_QRAB_THRESH_DRC_LOCK_CC_BASE) ||
              (attrib_id == (uint16) 
              HDRSCMRMAC4_PS_QRAB_THRESH_DRC_UNLOCK_CC_BASE) );
        }

      } /* If is_changed */

    } /* For each reverse logical channel */

  } /* For each per-channel attribute */

  if (rmacdrv_update_needed)
  {
    hdrrmac4drv_update_attributes();
  }

  if (calc_t2p_info_needed)
  {

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
    t2p_boost_gain = 
      hdrrmaccmn_validate_boost_gain( hdrrmac4_scm_cfg_ptr->power_params,
                                      hdrrmac4_var_ptr->t2p_boost.gain);

    /* Enable/disable based on gain */
    if ( t2p_boost_gain == 0 )
    {
      hdrrmac4_var_ptr->t2p_boost_enabled = FALSE;
    }
    else
    {
      hdrrmac4_var_ptr->t2p_boost_enabled = TRUE;
    }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

    hdrrmaccmn_calc_t2p_info( hdrrmac4_scm_cfg_ptr->power_params,
                              &hdrrmac4_config_ptr->aux_pilot_info,
                              hdrrmac4_config_ptr->t2p_info,
                              t2p_boost_gain,
                              hdrrmac4_var_ptr->t2p_boost.deboost_gain,
                              hdrrmac4_var_ptr->t2p_boost.tx_t2p_info );

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
    /* Calculate minimum term target across both transmission modes
       and all packet sizes*/
    hdrrmac4_var_ptr->min_term_target = hdrrmac4_get_min_term_target();
#endif  /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

  }

  /* Query SCM DB to find the changed NN attributes */
  hdrscmdb_find_changed_nn_attribs( HDRHAI_RTC_MAC_PROTOCOL,
                                    HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS,
                                    HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS,
                                    hdrrmac4_nn_gaup_verno,
                                    hdrrmac4_gaup_nn_attrib_verno_indices,
                                    is_nn_changed );

  /*------------------------------------------------------------------------
    For Each attribute, if it was changed, read the attribute from DB and 
    take appropriate action.
  -------------------------------------------------------------------------*/

  /* For each per-flow attribute */
  for (g_attr_ind=0; 
       g_attr_ind<HDRSCMRMAC4_NUM_GAUPABLE_NN_ATTRIBS; 
       ++g_attr_ind)
  {
    /* For each MAC flow instance of the per-flow attribute */
    for (flow_id=0; flow_id<HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++flow_id)
    {
      /* Compute the current attribute's index... */
      curr_nn_g_attr_ind = g_attr_ind * HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS +
                            flow_id;

      /* If the current attrib was just GAUPed */
      if (is_nn_changed[curr_nn_g_attr_ind])
      {
        /* If the GAUPed attribute was BucketLevelMax... */
        if (hdrrmac4_gaup_nn_attrib_ids[g_attr_ind] == 
          (uint16) HDRSCMRMAC4_BUCKET_LEVEL_MAX_NN_BASE)
        {
          /* Remember the attribute so we know if we need to give indn to APP */
          bl_max_prev = hdrrmac4_config_ptr->flow[flow_id].bucket_level_max;
        }

        /* Get the attribute ID from the base and flow ID */
        attrib_id = hdrrmac4_gaup_nn_attrib_ids[g_attr_ind] | flow_id;

        HDR_MSG_PROT_1(MSG_LEGACY_MED, "RMAC4 attrib %x GAUPed", attrib_id);

        /* All GAUPs other than Associated Flows..*/
        if (hdrrmac4_gaup_nn_attrib_ids[g_attr_ind] != 
          (uint16) HDRSCMRMAC4_ASSOCIATED_FLOWS_NN_BASE)
        {
          /* Get the update attribute value from SCM database */
          hdrrmac4_nn_gaup_verno[curr_nn_g_attr_ind] =
            hdrscmdb_get_inuse_attrib( 
              HDRHAI_RTC_MAC_PROTOCOL,
              attrib_id,
              hdrrmac4_nn_gaup_attribs[g_attr_ind][flow_id],
              HDRSCP_MC_RMAC);

          /* Process the update of the Per-Flow attribute */
          (void) hdrrmac4_process_nn_attrib_update(attrib_id, flow_id);
        }
        else
        {
          HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                         "Associated Flows[%d] Gauped", 
                         flow_id);

          /* Just remember that flow binding has changed */
          flow_mapping_changed = TRUE;
        }

        /* If the GAUPed attribute was BucketLevelMax... */
        if (hdrrmac4_gaup_nn_attrib_ids[g_attr_ind] == 
          (uint16) HDRSCMRMAC4_BUCKET_LEVEL_MAX_NN_BASE)
        {
          /* Give indication to App only if the BucketLevelMax was changed from
             zero to non-zero or vice versa */
          if ((bl_max_prev == 0) && 
              (hdrrmac4_config_ptr->flow[flow_id].bucket_level_max != 0))
          {
            /* RMAC4 Flow Activated */
            hdrind_give_ind(HDRIND_RMAC_FLOW_ACTIVATED, (void*)&flow_id );

            HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                           "RMAC4 flow %d Activated", 
                           flow_id);
          }
          else if ((bl_max_prev != 0) && 
                   (hdrrmac4_config_ptr->flow[flow_id].bucket_level_max == 0))
               
          {
            /* RMAC4 Flow Deactivated */
            hdrind_give_ind(HDRIND_RMAC_FLOW_DEACTIVATED, (void*)&flow_id );

            HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                           "RMAC4 flow %d Deactivated", 
                           flow_id);
          }

        } /* If bucketlevel max was gauped */

      } /* If is_changed */

    } /* For each flow */

  } /* For each per-flow attribute */

  if (flow_mapping_changed)
  {
    /* Give indication to App about the change in flow mapping */
    hdrind_give_ind(HDRIND_RMAC_FLOW_MAPPING_CHANGED, NULL);
  }

  /* Query SCM DB to find the changed NN attributes */
  hdrscmdb_find_changed_xx_attribs( HDRHAI_RTC_MAC_PROTOCOL,
                                    HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS,
                                    HDRSCMRMAC4_MAX_NUM_CONFIGS,
                                    hdrrmac4_xx_gaup_verno,
                                    hdrrmac4_gaup_xx_attrib_verno_indices,
                                    is_xx_changed );

  /*------------------------------------------------------------------------
    For Each attribute, if it was changed, read the attribute from DB and 
    take appropriate action.
  -------------------------------------------------------------------------*/

  /* For each per-config attribute */
  for (g_attr_ind=0; 
       g_attr_ind<HDRSCMRMAC4_NUM_GAUPABLE_XX_ATTRIBS; 
       ++g_attr_ind)
  {
    /* For each Config instance of the per-config attribute */
    for (config_id=0; config_id<HDRSCMRMAC4_MAX_NUM_CONFIGS; ++config_id)
    {
      /* Compute the current attribute's index... */
      curr_xx_g_attr_ind = g_attr_ind * HDRSCMRMAC4_MAX_NUM_CONFIGS +
                            config_id;

      /* If the current attrib was just GAUPed */
      if (is_xx_changed[curr_xx_g_attr_ind])
      {
        /* Get the attribute ID from the base and config ID */
        attrib_id = hdrrmac4_gaup_xx_attrib_ids[g_attr_ind] | config_id;

        HDR_MSG_PROT_1(MSG_LEGACY_MED, "RMAC4 attrib %x GAUPed", attrib_id);

        /* Get the update attribute value from SCM database */
        hdrrmac4_xx_gaup_verno[curr_xx_g_attr_ind] =
          hdrscmdb_get_inuse_attrib(
            HDRHAI_RTC_MAC_PROTOCOL,
            attrib_id,
            hdrrmac4_xx_gaup_attribs[g_attr_ind][config_id],
            HDRSCP_MC_RMAC);

        /* Process the update of the Per-Config Attribute */
        (void) hdrrmac4_process_xx_attrib_update(attrib_id, config_id);

      } /* If is_changed */

    } /* For each configuration */

  } /* For each per-config attribute */

  /* Query SCM DB to find the changed NN attributes */
  hdrscmdb_find_changed_nc_attribs( HDRHAI_RTC_MAC_PROTOCOL,
                                    HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS,
                                    HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS,
                                    HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS,
                                    hdrrmac4_nc_gaup_verno,
                                    hdrrmac4_gaup_nc_attrib_verno_indices,
                                    is_nc_changed );

  /*------------------------------------------------------------------------
    For Each attribute, if it was changed, read the attribute from DB and 
    take appropriate action.
  -------------------------------------------------------------------------*/

  /* For each per-NC attribute */
  for (g_attr_ind=0;
       g_attr_ind<HDRSCMRMAC4_NUM_GAUPABLE_NC_ATTRIBS; 
       ++g_attr_ind)
  {
    curr_nc_g_base_offset = g_attr_ind * HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS *
                            HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS;

    /* For each flow instance of the per-NC attribute */
    for (flow_id=0; flow_id<HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++flow_id)
    {
      curr_nn_g_attr_ind = ( curr_nc_g_base_offset + 
                             (flow_id * HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS) );

      /* For each channel instance of the per-NC attribute */
      for (chan_id=0; chan_id<HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS; ++chan_id)
      {
        /* Compute the current attribute's index... */
        curr_nc_g_attr_ind = curr_nn_g_attr_ind + chan_id;

        /* If the current attrib was just GAUPed */
        if (is_nc_changed[curr_nc_g_attr_ind])
        {
          /* Get the attribute ID from the base and flow and channel ID's */
          attrib_id = hdrrmac4_gaup_nc_attrib_ids[g_attr_ind]     |
                        (flow_id << HDRSCMRMAC4_NCX_NN_ID_SHIFT)  |
                        chan_id;

          HDR_MSG_PROT_1(MSG_LEGACY_MED, "RMAC4 attrib %x GAUPed", attrib_id);

          /* Get the update attribute value from SCM database */
          hdrrmac4_nc_gaup_verno[curr_nc_g_attr_ind] =
            hdrscmdb_get_inuse_attrib(
              HDRHAI_RTC_MAC_PROTOCOL,
              attrib_id,
              hdrrmac4_nc_gaup_attribs[g_attr_ind][flow_id][chan_id],
              HDRSCP_MC_RMAC);

          /* Process the update of the Per-NC Attribute */
          (void) hdrrmac4_process_nc_attrib_update(attrib_id, flow_id, chan_id);

        } /* If is_changed */

      } /* For each channel */

    } /* For each flow */

  } /* For each per-NC attribute */

  return;

} /* hdrrmac4_process_attrib_update */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_RESTART_TIMER_EXPIRED_IND                    LOCAL

DESCRIPTION
  This function processes the FTCMAC.ReverseTrafficChannelRestartExpired
  indication.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel whose Restart Timer expired.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_process_restart_timer_expired_ind
(
  uint8 phy_chan_idx
)
{
  hdrrmac4_drop_info_type drop_info;
    /* Carrier drop information record */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "DRC Supervision Restart Expired: %d",
                  phy_chan_idx );

  /* Only handle if carrier is in setup or open -- that is, it isn't inactive
     or in the processing of going inactive */
  if ((hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state ==
       HDRRMAC4_CHAN_SETUP_STATE) ||
      (hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state ==
      HDRRMAC4_CHAN_OPEN_STATE))
  {

    /* fill in the dropped information */
    drop_info.phy_chan_idx = phy_chan_idx;
    drop_info.reason_code = HDRRMAC4_RL_DROP_REASON_FTC_DRC_SUPERVISION;

    hdrrmac4_send_rl_drop_msg(
      hdrrmac4_generate_rl_drop_msg(1,  /* Always 1 channel per indication */
                                    &drop_info));

    /* post event to Carrier Request function */
    hdrrmac4_carrier_req_event(HDRRMAC4_CARRIER_REQ_EVENT_DRC_SUPERVISION_DROP,
                               0,
                               phy_chan_idx,
                               0); 
      /* num_chans and phy_chan_mask not used */

    /* Put channel in INACTIVE state */
    hdrrmac4_chan_enter_inactive_state(phy_chan_idx);

    /* If all channels are in INACTIVE state, notify FTMAC about 
       DRC supervision failure */
    if ( hdrrmac4_var_ptr->num_inactive_state_chans ==
         HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS )
    {
      hdrfmac_drc_supervision_failure();
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "DRC Supervision Failure" );
    }

    /* Tell rmac4drv to stop this channel */
    (void) hdrrmac4drv_stop_channels(0x1 << phy_chan_idx);
#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
    /* if all carriers have DRC supervision timeout, the connection will be closed
       and flexconn/dtx will be reset. This is taken care of by
       hdrfmac_drc_supervision_failure().
       The following logic is for case of one carrier being dropped because of
       DRC supervision timeout. There are other good carriers, so that we need
       to start monitoring FlexConn and DTX again.
    */
    if ( hdrrmac4_var_ptr->num_inactive_state_chans <
         HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS  )
    {
      if ( hdrrmac4drv_no_carrier_in_drc_supervision() == TRUE )
    {
       hdrrmac_start_flexconn_dtx_monitor( TRUE );
          /* Start monitoring FL and RL if all active carrier are out of DRC
             supervision
          */
      }
    }
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */
  }
  else
  {
    /* For flexconn/dtx, we don't need to start flexconn dtx monitor if DRC
       supervision timer of inactive carrier has expired.Here is the reason:
       When dropping a carrier who has entered DRC supervision,
       fmac_exit_supervision() will be called. In most cases, the timer will
       be cancelled and we won't see DRC supervision timer expiry of inactive 
       carrier. In rare case of race condition,fmac_exit_supervision()
       being called and timer expiry happen at the same time, RMAC will
        receive exit drc supervision for the inactive carrier too. Start 
       flexconn dtx monitor is processed there. 
    */
    HDR_MSG_PROT_1(
      MSG_LEGACY_ERROR,
      "DRC restart timer ind received for inactive carrier %d",
      phy_chan_idx );
  }

  return;

} /* hdrrmac4_process_restart_timer_expired_ind */


/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_TRANSMIT_POWER_DIFF                          LOCAL

DESCRIPTION
  This function processes carrier drop due to transmit pilot power
  differential.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel to be dropped due of power differential.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_process_transmit_power_diff
(
  uint8 phy_chan_idx
)
{
  hdrrmac4_drop_info_type drop_info;
    /* Carrier drop information record */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (HDRRMAC4_IS_RL_OPEN( phy_chan_idx ))
  {
    /* fill in the dropped information */
    drop_info.phy_chan_idx = phy_chan_idx;
    drop_info.reason_code = HDRRMAC4_RL_DROP_REASON_TXP_DIFF;

    hdrrmac4_send_rl_drop_msg(
      hdrrmac4_generate_rl_drop_msg(1, &drop_info));

    /* post event to Carrier Request function */
    hdrrmac4_carrier_req_event(HDRRMAC4_CARRIER_REQ_EVENT_TXP_DIFF,
                               0,
                               phy_chan_idx,
                               0); 
    /* num_chans and phy_chan_mask not used */

    /* Put channel in INACTIVE state */
    hdrrmac4_chan_enter_inactive_state(phy_chan_idx);

    /* Tell rmac4drv to stop this channel */
    (void) hdrrmac4drv_stop_channels(0x1 << phy_chan_idx);

    /* MARQ outstanding packets on this channel in the next subframe
       processing */
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].marq_outstanding_pkts = TRUE;
  }
  else
  {
    HDR_MSG_PROT_2(
      MSG_LEGACY_HIGH,
      "Transmit power diff reported in non-open state. Ch=%d, State=%d",
      phy_chan_idx,
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state );
  }

  return;

} /* hdrrmac4_process_transmit_power_diff */


 /* EJECT */
/*=========================================================================

FUNCTION     : HDRRMAC4_IS_APP_FLOW_BOUND_ALREADY                 LOCAL

DESCRIPTION  : 
  When AN GAUPs the AssociatedFlows attribute to redefine the MAC-RLP flow 
  mapping for a particular MAC flow, it doesn't explicitly unbinds those
  RLP flows if they were already bound to a different MAC flow. Having an 
  RLP flow bound to two different MAC flows at the same time is invalid. So
  AT needs to unbind the RLP flow from a MAC flow before binding it to a 
  new MAC flow as per AN's update. 

  This API is used to find out if a particular app_flow is bound to any 
  MAC flow other than the one given by 'bound_mf_id'. If a duplicate binding
  exists, the MAC flow ID and the AppFlow array index that has the duplicate 
  binding are stored into 'dup_mf_id' and 'dup_af_ind' respectively.

Note: This function is called from DS task.

DEPENDENCIES : None

PARAMETERS   :
  app_flow    - AppFlow being looked-up for duplicate mapping.
  bound_mf_id - MAC Flow bound to the AppFlow given by 'app_flow'.
  dup_mf_id   - If there is a duplicate binding, the MAC Flow ID which has
                the duplicate mapping is stored here.
  dup_af_ind  - This is used to store the index into the AppFlow array where
                duplicate binding is found.

RETURN VALUE : 
  TRUE if a duplicate binding is found.
  FALSE otherwise.

SIDE EFFECTS : None

=========================================================================*/
RMAC4_LOCAL boolean hdrrmac4_is_app_flow_bound_already
(
  hdrrmaccmn_app_flow_struct_type *app_flow,
  uint8 bound_mf_id,
  uint8 *dup_mf_id,
  uint8 *dup_af_ind
)
{
  uint8 mf_id = 0;
    /* MAC flow ID */

  uint8 af_ind;
    /* App Flow index */

  hdrrmaccmn_flow_mapping_struct_type *flow_mapping = NULL;
    /* Convenience pointer */

  hdrrmaccmn_app_flow_struct_type *paf = NULL;
    /* Convenience pointer */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* For each MAC flow ID except the one newly bound to this AppFlow  */
  for (mf_id=0; mf_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++mf_id)
  {
    if (mf_id != bound_mf_id)
    {
      flow_mapping = &hdrrmac4_config_ptr->flow[mf_id].flow_mapping;

      /* For each AppFlow bound to MAC flow 'mf_id' */
      for (af_ind=0; af_ind < flow_mapping->flow_count; ++af_ind)
      {
        paf = &flow_mapping->app_flows[af_ind];

        if ((paf->stream_id == app_flow->stream_id) &&
            (paf->sub_stream_id == app_flow->sub_stream_id))
        {
          *dup_mf_id = mf_id;
          *dup_af_ind = af_ind;
          return TRUE;
        }

      } /* For each AppFlow bound to MAC flow mf_id */

    } /* If */

  } /* For each MAC flow ID */

  return FALSE;

} /* hdrrmac4_is_rlp_bound_to_mac_flow */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_ENTER_SETUP_STATE                                  INTERNAL

DESCRIPTION
  Enter the Setup state.  Sets up RTC MAC parameters for the reverse link.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_enter_setup_state (void)
{

  #ifdef FEATURE_FACTORY_TESTMODE
  
  /* Do not get the ATI record from session layer in FTM Mode*/
  if ( ftm_get_mode() != FTM_MODE )
  #endif
  {
    /* Get the current ATI record from the Session Layer. */
    (void) hdramp_get_transmit_ati( &hdrrmac4_var_ptr->ati_record, 
                                    HDRHAI_RTC_MAC_PROTOCOL );
  }

  /* Protocol specifies a Setup timer.  A duplicate timer exists in RUP,
     so this one is not used. */

  /* Reset boolean */
  hdrrmac4_var_ptr->data_allowed = FALSE;

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
  /* Clear history since we are on new call */
  hdrrmac4drv_clear_carrier_history();
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

  HDR_MSG_PROT( MSG_LEGACY_MED, "RTCMAC has entered setup state" );

  return;

} /* hdrrmac4_enter_setup_state */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_ISR                                               INTERNAL

DESCRIPTION
  Frame interrupt ISR.  Runs in interrupt context; sets signal for task
  to run subframe processing.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_isr( void )
{
  /* Set signal */
  (void) rex_set_sigs( HDRRMACCMN_TASK_PTR, HDRRMACCMN_ISR_SIG );

  return;

} /* hdrrmac4_isr */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_SHUTDOWN_CB                                       INTERNAL

DESCRIPTION
  This function is called by the RTCMAC driver layer after a 
  hdrrmac4_shutdown() call to indicate that all packets have completed 
  tx (protocol is ready to shutdown).
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_shutdown_cb( void )
{
  /* Give indication immediately */
  hdrind_give_ind(HDRIND_RTCMAC_SHUTDOWN_COMPLETE, NULL);

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "RTC Shutdown: Data channel ended" );

  return;

} /* hdrrmac4_shutdown_cb */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_START_TX                                          INTERNAL

DESCRIPTION
  Start or resume transmission from suspension.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - pointer to received command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_start_tx( void )
{
  if (hdrrmac4_var_ptr->tx_status != TX_STARTED)
  {
    hdrrmac4_start_rmac();
  }

  return;

} /* hdrrmac4_start_tx */


#ifdef FEATURE_FACTORY_TESTMODE
 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_ISR_SIG                                  INTERNAL

DESCRIPTION
  (Test function)  
  This function handles the signal from the Open State interrupt service
  routine.  The maximum allowable packet size is computed and a new transmit
  packet is requested from the Security Layer.  If there is a packet to
  transmit, the packet is loaded into the encoder.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_process_isr_sig( void )
{
  uint16 load_tx_pkt_id; /* The pkt_id's returned by lower layer for the 
                            packet currently being sent and the one just 
                            loaded, respectively */

  static hdrrmac4drv_pkt_enc_info_struct_type 
    pkt_enc[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* packet encoding structure used with hdrrmac4drv_send_packet() interface */

  uint8 phy_chan_idx, chan, num_channels;

  hdrrmac4_phy_chan_vars_struct_type  *phy_chan_ptr;
    /* convenience pointer */

  byte hdr_revb_packet[MAX_REVA_PAYLOAD_SIZE_BYTES];
    /* Packet data */

  uint32 trans_data; 
  uint16 shift;
  uint16 i;
  uint16 dsm_packet_size;
    /* Intermediate variables used in data generation */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT( MSG_LEGACY_HIGH, 
    "In the subframe ISR routine for FTM for RevB");

  /* Check for rc indicating error.  Abort if this occurs. */
  if (!hdrrmac4drv_get_prev_tx_status( 
      &hdrrmac4_var_ptr->enc_window_missed, 
      &rmac4_pkt_status ))
  {
    return;
  }

  /* update the TCA information */
  hdrrmac4_tca_update();

  num_channels = hdrhitmsg_get_rmac_params_ptr()->revb_num_carriers;

  HDR_ASSERT (num_channels <= HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS);

  for ( chan = 0; chan < num_channels; chan++ )
  {
    phy_chan_idx = hdrrmac4_phy_chan_idx_revb[chan];
    phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[phy_chan_idx];

    pkt_enc[chan].phy_chan_idx = phy_chan_idx;

    /* Generate data to be sent on carriers */

    if ( hdrhitmsg_get_rmac_params_ptr()->tx_payload_revb_c[phy_chan_idx] > 
      MAX_REVA_PAYLOAD_SIZE_BYTES )
    {
       HDR_MSG_PROT( MSG_LEGACY_HIGH,
         "Payload size is greater than maximum allowed");

       return;
    }

    if ( hdrhitmsg_get_rmac_params_ptr()->rtc_pattern == 0 )
    {
      /* tx pattern == 0 means we want random data packet */

      for ( i = 0; i<hdrhitmsg_get_rmac_params_ptr()->tx_payload_revb_c[phy_chan_idx]; i++ )
      {
        hdr_revb_packet[i] = (byte) ( ran_dist( hdrutil_get_random(), 0, 256) );
          /* Get random data */
      } 
    }
    else
    {
    /* Generate data with FTM specified pattern */
    for ( i=0; i<hdrhitmsg_get_rmac_params_ptr()->tx_payload_revb_c[phy_chan_idx] ; i++ )
    {
      shift = (i%4)*8;

      trans_data = ( hdrhitmsg_get_rmac_params_ptr()->rtc_pattern >> shift ) & 0x0000ff;

      hdr_revb_packet[i] = (byte)trans_data;
    }
    }
    /* Set the FTM dictated RRI, transmission mode and payload size in 
       bytes for RevB packet */
    phy_chan_ptr->pkt_size_index = (hdrrmaccmn_payload_size_enum_type)
      hdrhitmsg_get_rmac_params_ptr()->tx_rri_revb_c[phy_chan_idx];
    pkt_enc[chan].pkt_size_index = phy_chan_ptr->pkt_size_index;

    phy_chan_ptr->pkt_tx_mode = (hdrrmaccmn_tx_mode_enum_type) 
      hdrhitmsg_get_rmac_params_ptr()->hicap_lolatency;
    pkt_enc[chan].transmission_mode = phy_chan_ptr->pkt_tx_mode;

    pkt_enc[chan].data_length = hdrhitmsg_get_rmac_params_ptr()->tx_payload_revb_c[phy_chan_idx];

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
    pkt_enc[chan].not_data_lmtd = FALSE;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

    /* Form packet */
    pkt_enc[chan].pkt_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL );

    if (pkt_enc[chan].pkt_ptr == NULL)
    {
       HDR_MSG_PROT( MSG_LEGACY_HIGH,
         "Cannot allocate the memory for the packet");

       return;
    }

    dsm_packet_size = dsm_pushdown(&(pkt_enc[chan].pkt_ptr), 
      &hdr_revb_packet[0],
      hdrhitmsg_get_rmac_params_ptr()->tx_payload_revb_c[phy_chan_idx] - 2,
      DSM_DS_SMALL_ITEM_POOL );

    if(dsm_packet_size < hdrhitmsg_get_rmac_params_ptr()->tx_payload_revb_c[phy_chan_idx]-2)
    {    
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
        "Cannot allocate necessary bytes. Only allocate: %d",
        dsm_packet_size);
    }

    /* Hard code Conn. Layer format; PCP is never called in FTM mode */
    phy_chan_ptr->pkt_info.format = HDRRMAC4DRV_FORMAT_B_PACKET;
    pkt_enc[chan].conn_format = phy_chan_ptr->pkt_info.format;

  } /* For all channels that are deemed enabled by FTM */

  /* Send the packet; lower layer takes care of updating PktTxT2P and
   * RRIChannelGain
   */
  load_tx_pkt_id = hdrrmac4drv_send_packet(
    &hdrrmac4_var_ptr->enc_window_missed,
    num_channels,
    &pkt_enc[0]
#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
    ,FALSE
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
    );

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
    "RevB packet sent.  LoadTxPacket id: %d", 
    load_tx_pkt_id);

  return;

} /* hdrrmac4_process_isr_sig */
#endif /* FEATURE_FACTORY_TESTMODE */

 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_DETERMINE_HDR_MODE                                 INTERNAL

DESCRIPTION
  This function checks if HDR mode is active or not

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  HDR mode active/inactive flag 

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL boolean hdrrmac4_determine_hdr_mode( void )
{
  /* Overall HDR mode active status */
  boolean                               hdr_mode_active = FALSE;

  /* HiCap subpackets available */
  hdrrmac4drv_pa_availability_enum_type hicap_subpackets_available;

  /* LoLat subpackets available */
  hdrrmac4drv_pa_availability_enum_type lolat_subpackets_available;

  /* HDR inactive reason */
  hdrrmac4drv_hdr_mode_enum_type        hdr_inactive_reason;

  /* Number of subpackets available before RLSI */
  int                                   sp_to_rlsi;
    
  /* Number of subpackets available before 1x Suspend */
  int                                   sp_to_suspend;

  /* Loop counter */
  uint8                                 phy_chan_idx;

  /* Carrier available flag - set to TRUE if a carrier is not under DRC
     supervision AND its corresponding fwd link has fingers in lock */
  boolean                               carrier_available;

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
  /* Total TX power */
  int16                                 tx_pwr = 0;
  /* Max TX power reported by FW */
  int16                                 max_pwr;
  /* K and Nominal power reported by RF */
  int16                                 k, nominal_pwr;
  /* RF device */
  rfm_device_enum_type                  rf_dev;
  /* New boost state */  
  hdrrmaccmn_t2p_boost_state_enum_type  new_state;
  /* Whether we are in funnel mode */
  boolean                               funnel_mode;
  /* temp looping variable */
  uint8                                 i;
  /* If any carrier is FRAB limited */
  boolean                               frab_limited = FALSE;
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get and record the number of subpackets we have available. HiCap must
     be restricted based on any outage...*/
  hicap_subpackets_available = hdrrmac4drv_get_pa_availability(
    &hdr_inactive_reason, 
    &sp_to_rlsi,
    &sp_to_suspend );

  /* ...while LoLat is only restricted by suspend. */
  lolat_subpackets_available = 
    (hdrrmac4drv_pa_availability_enum_type) sp_to_suspend;

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
  /* If number of subpacket available < min_term_target, then graceful
  ta has started, wont be able to send anymore packets. */
  if ( ( hdrrmac4_var_ptr->min_term_target + 1 ) > 
       ( uint8 ) lolat_subpackets_available )
  {
    hdrrmac4_var_ptr->graceful_ta_started = TRUE;
  
    HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
      "Graceful TA started, Num subpackets %d < min term target %d. t2p_no_tx = %d",
      lolat_subpackets_available,
      hdrrmac4_var_ptr->min_term_target,
      hdrrmac4_config_ptr->filt_time_const.t2p_no_tx
    );
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

  for ( phy_chan_idx = 0; 
        phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
        phy_chan_idx++ )
  {
    if ( HDRRMAC4_IS_RL_NOT_INACTIVE(phy_chan_idx) )
    {
      /* Populate per-carrier subpackets to RLSI, subpackets to suspend
         and available subpackets fields */
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].sp_to_rlsi = 
        sp_to_rlsi;
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].sp_to_suspend = 
        sp_to_suspend;
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hc_subpackets_available = 
        hicap_subpackets_available;
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].ll_subpackets_available = 
        lolat_subpackets_available;

      /* Copy over global hdr_inactive_reason to per-carrier 
         hdr_inactive_reason */
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hdr_inactive_reason = 
        hdr_inactive_reason;

      /* Obtain carrier availability and update per-carrier
         hdr_inactive_reason */
      carrier_available = hdrrmac4drv_get_carrier_availability( 
#ifdef FEATURE_HDR_RMAC4_3X_UNIT_TEST
        #error code not present
#else /* FEATURE_HDR_RMAC4_3X_UNIT_TEST */
        phy_chan_idx, 
#endif /* FEATURE_HDR_RMAC4_3X_UNIT_TEST */
        &hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hdr_inactive_reason );

      /* Find per-carrier and global hdr_mode_active */
      if ( ( HDRRMAC4_IS_RL_OPEN(phy_chan_idx) ) &&
           ( hicap_subpackets_available != HDRRMAC4DRV_PA_NOT_AVAILABLE ) &&
           ( hdrrmac4_var_ptr->data_allowed == TRUE ) &&
           ( carrier_available ) )
      {
        hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hdr_mode_active = TRUE;

        /* If HDR mode is active on any channel, overall HDR mode 
           is available */
        hdr_mode_active = TRUE;
      }
      else
      {
        hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hdr_mode_active = FALSE;
      }
    }
    else
    {
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hdr_mode_active = FALSE;
    }

  } /* for each physical channel */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
  /* if RTT reduction NV is enabled, check RTT condition first */
  rf_dev = hdrsrchrf_get_rf_dev( HDRSRCHRF_PRI_CHAIN );
  if ( hdrrmac4_var_ptr->t2p_boost_enabled && 
       hdrrmac4_var_ptr->rtt_reduct_opt_enabled && 
       ( hdrrmac4_var_ptr->t2p_boost.state != 
         HDRRMACCMN_T2P_BOOST_PRE_TA_ACTIVE ) &&
       ( hdrrmac4_var_ptr->t2p_boost.state != 
         HDRRMACCMN_T2P_BOOST_POST_TB_ACTIVE ) &&
       ( rf_dev != RFM_INVALID_DEVICE ) &&
       hdrmod_pa_is_on() )
  {
    new_state = hdrrmac4_var_ptr->t2p_boost.state;

    funnel_mode = hdrrmac4drv_get_funnel_mode();
      /* Check with RMAC4Drv if we are in funnel mode,
         we need to use txTotalOverallMaxLimitNeq if so */
    tx_pwr = rfm_hdr_get_filtered_tx_power_v2( 
               rf_dev,
               RFM_TX_FILTER_POWER_WHEN_TX_ON );

    rfm_hdr_get_turnaround_info(rf_dev, &k, &nominal_pwr );

    max_pwr = HDRMDSP_GET_MAX_TX_LIMIT( funnel_mode );

    max_pwr = hdrrmaccmn_fw_pwr_2_db10( max_pwr, k );

      /* Check and update if AT is FRAB limited */
    for (i=0; i<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; i++)
    {
      if ( hdrrmac4_var_ptr->phy_chan[i].hdr_mode_active )
      {
        if ( hdrrmac4_var_ptr->t2p_boost.frab_limited[i] &&
             ( hdrrmac4_var_ptr->phy_chan[i].sector_info.frab < 
               HDRRMACCMN_T2P_BOOST_RRI_RAB_THRESH_START ) )
        {
          hdrrmac4_var_ptr->t2p_boost.frab_limited[i] = FALSE;
        }
        else if ( ( !hdrrmac4_var_ptr->t2p_boost.frab_limited[i] ) &&
                  ( hdrrmac4_var_ptr->phy_chan[i].sector_info.frab > 
                    HDRRMACCMN_T2P_BOOST_RRI_RAB_THRESH_STOP ) )
        {
          hdrrmac4_var_ptr->t2p_boost.frab_limited[i] = TRUE;
        }

        frab_limited |= hdrrmac4_var_ptr->t2p_boost.frab_limited[i];
      }
    }
      /* Check and update if AT is TxPwr limited */
    if ( hdrrmac4_var_ptr->t2p_boost.txpwr_limited && 
         ( tx_pwr <= ( max_pwr - hdrrmac4_var_ptr->t2p_boost.y1 ) ) )
    {
      hdrrmac4_var_ptr->t2p_boost.txpwr_limited = FALSE;
    }
    else if ( ( !hdrrmac4_var_ptr->t2p_boost.txpwr_limited ) &&
              ( tx_pwr >= ( max_pwr - HDRRMACCMN_T2P_BOOST_TX_PWR_THRESH_STOP ) ) )
    {
      hdrrmac4_var_ptr->t2p_boost.txpwr_limited = TRUE;
    }

    if ( ( hdrrmac4_var_ptr->t2p_boost.state != 
           HDRRMACCMN_T2P_BOOST_RTT_REDUCT_ACTIVE ) &&
         ( !hdrrmac4_common->is_tap_call ) &&
         ( !frab_limited ) &&
         ( !hdrrmac4_var_ptr->t2p_boost.txpwr_limited ) &&
         ( !hdrrmac4_common->is_tap_call ) )
    {
      HDR_MSG_PROT_6( MSG_LEGACY_HIGH, 
                      "T2P boost RTT activated, TAP: %d FRAB: %d/%d/%d, Tx Pwr:%d Max Pwr: %d",
                      hdrrmac4_common->is_tap_call,
                      hdrrmac4_var_ptr->phy_chan[0].sector_info.frab,
                      hdrrmac4_var_ptr->phy_chan[1].sector_info.frab,
                      hdrrmac4_var_ptr->phy_chan[2].sector_info.frab,
                      tx_pwr, max_pwr );

      new_state = HDRRMACCMN_T2P_BOOST_RTT_REDUCT_ACTIVE;
    }
    else if ( ( hdrrmac4_var_ptr->t2p_boost.state == 
              HDRRMACCMN_T2P_BOOST_RTT_REDUCT_ACTIVE ) &&
              ( frab_limited ||
                hdrrmac4_var_ptr->t2p_boost.txpwr_limited ||
                hdrrmac4_common->is_tap_call ) )
    {
      new_state = HDRRMACCMN_T2P_BOOST_INACTIVE;
    }

    if ( hdrrmac4_var_ptr->t2p_boost.state != new_state )
    {    
      HDR_MSG_PROT_9( MSG_LEGACY_HIGH, 
                      "T2P boost RTT State:%d=>%d TAP:%d FRAB:%d/%d/%d Funnel:%d Tx Pwr:%d Max Pwr: %d",
                      hdrrmac4_var_ptr->t2p_boost.state,
                      new_state,
                      hdrrmac4_common->is_tap_call,
                      hdrrmac4_var_ptr->phy_chan[0].sector_info.frab,
                      hdrrmac4_var_ptr->phy_chan[1].sector_info.frab,
                      hdrrmac4_var_ptr->phy_chan[2].sector_info.frab,
                      funnel_mode,
                      tx_pwr, max_pwr );

      hdrrmac4_var_ptr->t2p_boost.state = new_state;
    }
  }
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */

  /* If less than four subpackets remain before tuneaway, activate
     pre-tuneaway T2P boost. */
  if ( hdrrmac4_var_ptr->t2p_boost_enabled &&
       hdrrmac4_var_ptr->pre_ta_opt_enabled &&
       ( lolat_subpackets_available < 
         HDRRMAC4DRV_PA_4_SUBPACKETS_AVAILABLE ) && 
       ( hdrrmac4_var_ptr->t2p_boost.state != HDRRMACCMN_T2P_BOOST_PRE_TA_ACTIVE ) )
  {
    hdrrmac4_var_ptr->t2p_boost.state = HDRRMACCMN_T2P_BOOST_PRE_TA_ACTIVE;

    HDR_MSG_PROT( MSG_LEGACY_HIGH, "T2P boost TA activated" );
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

  if (!hdr_mode_active)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                    "HDR mode inactive: RLSI or Data disallowed %d ",
                    hdrrmac4_var_ptr->data_allowed);
  }

  return hdr_mode_active;

} /* hdrrmac4_determine_hdr_mode */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UNPACK_RTCACK_MSG                                 INTERNAL

DESCRIPTION
  This function unpacks the received RTCACK message into the location provided.

DEPENDENCIES
  grant_msg_ptr should point to valid memory.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  grant_msg_ptr - Location into which to extract the received message

RETURN VALUE
  E_SUCCESS  - If extraction & unpacking of the message succeeds & field values 
               are valid.
  E_NO_DATA  - If there was an error extracting the message.

SIDE EFFECTS
  None

===========================================================================*/
RMAC4_LOCAL hdrerrno_enum_type hdrrmac4_unpack_rtcack_msg
(
  dsm_item_type *item_ptr,
  hdrrmac4_rtcack_msg_type *rtcack_msg_ptr
)
{
  int  offset;                       /* Offset(bits) of field from msg start */
  uint8 channel;                     /* channel loop counter */
  uint8 band;                        /* band class */
  uint8 system_type;                 /* System Type */
  int  err;                          /* Error value */
  hdrerrno_enum_type unpack_result;  /* Return value of unpack Grant msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset = HDRRMAC4_RTCACK_MSG_ID_SIZE;
  err = 0; /* E_SUCCESS = 0 */
  unpack_result = E_SUCCESS;

  /* Unpack the received message */
  err |= (int) HDRRMAC4_RTCACK_NUM_REVERSE_CHANNELS( 
                item_ptr , 
                &rtcack_msg_ptr->num_reverse_channels,
                offset );
  offset += HDRRMAC4_RTCACK_NUM_REVERSE_CHANNELS_SIZE;
  
  /* NumReverseChannels in the RTCACK msg is 1 less than the number of 
     Reverse Channels */
  rtcack_msg_ptr->num_reverse_channels += 1;

  if ( rtcack_msg_ptr->num_reverse_channels > 
       HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS )
  {
    ERR( "Invalid num_reverse_channels %d rvcd in RTCACK", 
          rtcack_msg_ptr->num_reverse_channels, 0, 0);
    err |= (int32) E_NO_DATA;
  }

  /* Ensure that num of Reverse Channels never exceeds 
     HDRRMAC4_MAX_NUM_REV_LOG_CHANS */
  rtcack_msg_ptr->num_reverse_channels = 
    MIN( rtcack_msg_ptr->num_reverse_channels, 
         HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS );

  for( channel = 0; 
       channel < rtcack_msg_ptr->num_reverse_channels; 
       channel++ )
  {
    /* SystemType */
    err |= (int) HDRRMAC4_RTCACK_REVERSE_CHANNEL_SYSTEM_TYPE(
                  item_ptr, &system_type, offset);
    offset += HDRRMAC4_RTCACK_SYSTEM_TYPE_SIZE;

    if ((system_type == HDRRMAC4_RTCACK_SYSTEM_TYPE_IS856) ||
        (system_type == HDRRMAC4_RTCACK_SYSTEM_TYPE_IS2000))
    {
      /* BandClass */
      err |= (int) HDRRMAC4_RTCACK_REVERSE_CHANNEL_BAND_CLASS( 
                      item_ptr, &band, offset);
      rtcack_msg_ptr->channel_records[channel].reverse_channel.band = 
        (sys_band_class_e_type) band;
      offset += HDRRMAC4_RTCACK_BAND_CLASS_SIZE;

      /* ChannelNumber */
      err |= (int) HDRRMAC4_RTCACK_REVERSE_CHANNEL_CHANNEL_NUMBER(
                      item_ptr,
                      &rtcack_msg_ptr->channel_records[channel].
                        reverse_channel.chan_num, 
                      offset);
      offset += HDRRMAC4_RTCACK_CHANNEL_NUMBER_SIZE;

      /* ReverseLogicalChannel */
      err |= (int) HDRRMAC4_RTCACK_REVERSE_CHANNEL_REVERSE_LOGICAL_CHANNEL(
                      item_ptr,
                      &rtcack_msg_ptr->channel_records[channel].
                        reverse_logical_channel, 
                      offset);
      offset += HDRRMAC4_RTCACK_REVERSE_LOGICAL_CHANNEL_SIZE;
    }
    else
    {
      /* Skip this channel record if the SystemType is any other value */
      offset += (HDRRMAC4_RTCACK_BAND_CLASS_SIZE +
                 HDRRMAC4_RTCACK_CHANNEL_NUMBER_SIZE +
                 HDRRMAC4_RTCACK_REVERSE_LOGICAL_CHANNEL_SIZE);

      HDR_MSG_PROT( MSG_LEGACY_ERROR, "RTCACK Msg skipping ST=2 chan record" );
    }
  }

  if (err != 0)
  {
    unpack_result = E_NO_DATA;

    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Error unpacking RTCACK Msg" );
  }

  return unpack_result;

} /* hdrrmac4_unpack_rtcack_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_RTCACK_MSG                                  INTERNAL

DESCRIPTION
  This function processes the RTCAck message.

DEPENDENCIES
  None.

PARAMETERS
  rcv_msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_process_rtcack_msg
(
  dsm_item_type * rcv_msg_ptr
)
{
  hdrerrno_enum_type      errnum;
    /* Result of unpacking RTCACK message */

  uint8                   log_chan;
    /* loop index for Reverse Logical Channels */

  uint8                   phy_chan;
    /* physical channel associated with this logical channel */

  uint8                   num_open_chans=0;
    /* Number of channels enter OPEN state after RTCAck is validated */

  uint8                   flow_id;
    /* Flow ID */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  errnum = hdrrmac4_unpack_rtcack_msg( rcv_msg_ptr, &hdrrmac4_rtcack_msg );

  if (errnum == E_SUCCESS)
  {
    /* 
     * For each logical channel record in the RTCAck, find a physical channel
     * contains matching band and channel number.
     */
    for (log_chan = 0; 
         log_chan < hdrrmac4_rtcack_msg.num_reverse_channels; 
         log_chan++)
    {
      phy_chan = hdrrmac4_from_rf_to_phy(
                  &hdrrmac4_rtcack_msg.channel_records[log_chan].reverse_channel);

      if (phy_chan != HDRRMAC4_INVALID_CHAN_ID)
      {
        /* 
         * Make sure that the we only process the RTCAck message when the 
         * channel is in the SETUP state.
         */
        if ((hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state ==
             HDRRMAC4_CHAN_SETUP_STATE) &&
             hdrrmac4drv_is_carrier_in_setup_state(phy_chan))
        {
          /* record the physical channel index */
          hdrrmac4_var_ptr->log_chan
            [(hdrrmac4_rtcack_msg.channel_records[log_chan].
              reverse_logical_channel)].phy_chan_id = 
            phy_chan;

          /* record the logical channel index */
          hdrrmac4_var_ptr->phy_chan[phy_chan].log_chan_id =
            hdrrmac4_rtcack_msg.channel_records[log_chan].
              reverse_logical_channel;

          /* Transition the channel into OPEN state */
          hdrrmac4_chan_enter_open_state(phy_chan);

          /* increment the validated open channel count */
          num_open_chans++;

          HDR_MSG_PROT_3(
            MSG_LEGACY_HIGH,
            "RTCACK, band %d, chan_num %d, logical %d",
            hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record.band,
            hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record.chan_num,
            hdrrmac4_var_ptr->phy_chan[phy_chan].log_chan_id);

          for (flow_id = 0; flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow_id++)
          {
            /* Populate T2PUp/Down grids.  Then grids require physical<->logical
               channel mapping, obtained in RTCAck, to be present. */
            hdrrmac4_update_gu_gd_grids(flow_id, phy_chan);
          }
        }
        else
        {
          /* Don't process the message */
          HDR_MSG_PROT_5(
            MSG_LEGACY_HIGH,
            "RTCACK Ignored in st %d, 4drv_carrier_in_setup? %d, band %d, "
            "chan_num %d, logical %d",
            hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state,
            hdrrmac4drv_is_carrier_in_setup_state(phy_chan),
            hdrrmac4_rtcack_msg.channel_records[log_chan].reverse_channel.band,
            hdrrmac4_rtcack_msg.channel_records[log_chan].reverse_channel.chan_num,
            hdrrmac4_rtcack_msg.channel_records[log_chan].reverse_logical_channel);
        }
      } /* if found a physical channel */

    } /* for each logical channel */

    /* Post RTCACK event to Carrier Request function */
    hdrrmac4_carrier_req_event(HDRRMAC4_CARRIER_REQ_EVENT_RTCACK,
                               num_open_chans,
                               0,
                               0); 
      /* phy_chan_idx and phy_chan_mask not used */
  }

  return;

} /* hdrrmac4_process_rtcack_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UNPACK_GRANT_MSG                                 INTERNAL

DESCRIPTION
  This function unpacks the received Grant message into the location provided.

DEPENDENCIES
  grant_msg_ptr should point to valid memory.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  grant_msg_ptr - Location into which to extract the received message

RETURN VALUE
  E_SUCCESS  - If extraction & unpacking of the message succeeds & field values 
               are valid.
  E_NO_DATA  - If there was an error extracting the message.

SIDE EFFECTS
  None

===========================================================================*/
RMAC4_LOCAL hdrerrno_enum_type hdrrmac4_unpack_grant_msg
(
  dsm_item_type *item_ptr,
  hdrrmac4_grant_msg_type *grant_msg_ptr
)
{
  int  offset;                      /* Offset(bits) of field from msg start */
  uint8 flow;                       /* flow loop counter */
  uint8 chan;                       /* channel loop counter */
  int  err;                         /* Error value */
  hdrerrno_enum_type unpack_result; /* Return value of unpack Grant msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset = HDRRMAC4_GRANT_MSG_ID_SIZE;
  err = 0; /* E_SUCCESS = 0 */
  unpack_result = E_SUCCESS;

  /* Unpack the received message */
  err |= (int) HDRRMAC4_GRANT_NUM_REVERSE_CHANNELS( 
                item_ptr,
                &grant_msg_ptr->num_reverse_channels,
                offset);
  offset += HDRRMAC4_GRANT_NUM_REVERSE_CHANNELS_SIZE;

  /* One less than the actual number */
  grant_msg_ptr->num_reverse_channels += 1;

  if( grant_msg_ptr->num_reverse_channels > HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS )
  {
    ERR( "Invalid num_reverse_channels %d rvcd in GRANT", 
          grant_msg_ptr->num_reverse_channels, 0, 0);
    err |= (int32) E_NO_DATA;
  }

  /* Ensure that num of reverse logical channels never exceeds 
     HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS */
  grant_msg_ptr->num_reverse_channels = 
    MIN( grant_msg_ptr->num_reverse_channels, 
         HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS );

  for (chan=0; chan<grant_msg_ptr->num_reverse_channels; chan++)
  {

    err |= (int) HDRRMAC4_GRANT_REVERSE_CHANNEL_REVERSE_LOGICAL_CHANNEL(
                  item_ptr,
                  &grant_msg_ptr->rl_channel_record[chan].reverse_logical_channel,
                  offset);
    offset += HDRRMAC4_GRANT_REVERSE_LOGICAL_CHANNEL_SIZE;

    err |= (int) HDRRMAC4_GRANT_NUM_MAC_FLOWS( 
                  item_ptr , 
                  &grant_msg_ptr->rl_channel_record[chan].num_mac_flows,
                  offset );
    offset += HDRRMAC4_GRANT_NUM_MAC_FLOWS_SIZE;

    /* NumMACFlows in the Grant msg is 1 less than the number of MAC flows */
    grant_msg_ptr->rl_channel_record[chan].num_mac_flows += 1;

    if( grant_msg_ptr->rl_channel_record[chan].num_mac_flows > 
        HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS )
    {
      ERR( "Invalid num_mac_flows %d rvcd in GRANT", 
            grant_msg_ptr->rl_channel_record[chan].num_mac_flows, 0, 0);
      err |= (int32) E_NO_DATA;
    }

    /* Ensure that num of MAC flows never exceeds HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS */
    grant_msg_ptr->rl_channel_record[chan].num_mac_flows = 
      MIN( grant_msg_ptr->rl_channel_record[chan].num_mac_flows, 
           HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS );

    for( flow = 0; 
         flow < grant_msg_ptr->rl_channel_record[chan].num_mac_flows; 
         flow++ )
    {

      err |= (int) HDRRMAC4_GRANT_MAC_FLOW_ID( 
                      item_ptr, 
                      &grant_msg_ptr->rl_channel_record[chan].
                      mac_flows[flow].mac_flow_id, 
                      offset );
      offset += HDRRMAC4_GRANT_MAC_FLOW_ID_SIZE;

      if( grant_msg_ptr->rl_channel_record[chan].mac_flows[flow].mac_flow_id >=
        HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS )
      {
        ERR( "Invalid mac_flow_id %d for flow %d for chan %d rvcd in GRANT", 
          grant_msg_ptr->rl_channel_record[chan].mac_flows[flow].mac_flow_id,
          flow, 
          chan );
        err |= (int32) E_NO_DATA;
      }

      err |= (int) HDRRMAC4_GRANT_T2P_INFLOW( 
                      item_ptr,
                      &grant_msg_ptr->rl_channel_record[chan].
                      mac_flows[flow].t2p_inflow, 
                      offset );
      offset += HDRRMAC4_GRANT_T2P_INFLOW_SIZE;

      err |= (int) HDRRMAC4_GRANT_BUCKET_LEVEL( 
                      item_ptr,
                      &grant_msg_ptr->rl_channel_record[chan].
                      mac_flows[flow].bucket_level, 
                      offset );
      offset += HDRRMAC4_GRANT_BUCKET_LEVEL_SIZE;

      err |= (int) HDRRMAC4_GRANT_TT2P_HOLD( 
                      item_ptr,
                      &grant_msg_ptr->rl_channel_record[chan].
                      mac_flows[flow].tt2p_hold, 
                      offset );
      offset += HDRRMAC4_GRANT_TT2P_HOLD_SIZE;

    } /* for each MAC Flow */

  } /* for each reverse logical channel */

  if (err != 0)
  {
    unpack_result = E_NO_DATA;

    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Error unpacking Grant Msg" );
  }

  return unpack_result;

} /* hdrrmac4_unpack_grant_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_GRANT_MSG                                  INTERNAL

DESCRIPTION
  This function processes the Grant message.

DEPENDENCIES
  None.

PARAMETERS
  rcv_msg_ptr - pointer to received message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_process_grant_msg
(
  dsm_item_type * rcv_msg_ptr
)
{

  uint8                   flow;
    /* loop index for MAC flows */

  uint8                   log_chan;
    /* loop index for reverse logical channel */

  uint8                   phy_chan;
    /* physical channel index */

  hdrerrno_enum_type      errnum;
    /* Result of unpacking Grant message */

  uint32 bucket_level_lin;
    /* Temporary variable for MIN check of bucket level. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrrmac4_common->state == HDRRMAC_OPEN_STATE )
  {
    /* Unpack the received message. Copy the T2PInflow, BucketLevel and 
     * TT2PHold from the Grant message into RTCMAC4 data structures. The 
     * algorithm checks to see if we should use the BucketLevel and T2PInflow 
     * values from a Grant message instead of the calculated values.
     */
    errnum = hdrrmac4_unpack_grant_msg( rcv_msg_ptr, &hdrrmac4_grant_msg );

    if( errnum == E_SUCCESS )
    {
      for (log_chan = 0; 
           log_chan < hdrrmac4_grant_msg.num_reverse_channels;
           log_chan++ )
      {
        /* Find out the associated physical channel */
        phy_chan = hdrrmac4_from_log_to_phy(
                      hdrrmac4_grant_msg.rl_channel_record[log_chan].
                        reverse_logical_channel);

        /* Sanity checking */
        if (phy_chan >= HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
        {
          HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                         "Invalid phy %d to log %d chan",
                         phy_chan,
                         hdrrmac4_grant_msg.rl_channel_record[log_chan].
                                              reverse_logical_channel);
        }
        else if (!HDRRMAC4_IS_RL_OPEN(phy_chan))
        {
          HDR_MSG_PROT_3(MSG_LEGACY_ERROR,
                         "Invalid state %d phy %d log %d chan",
                         hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state,
                         phy_chan,
                         hdrrmac4_grant_msg.rl_channel_record[log_chan].
                                              reverse_logical_channel);

        }
        else
        {
          for( flow = 0; 
               flow < hdrrmac4_grant_msg.rl_channel_record[log_chan].
                      num_mac_flows; 
               flow++ )
          {
            uint8 fid, bucket_level, t2p_inflow; 
              /* temp variables for corresponding values from Grant message */

            fid = hdrrmac4_grant_msg.rl_channel_record[log_chan].
                                      mac_flows[flow].mac_flow_id;

            /* Grant message specifies TT2pHold in units of frames; convert to
             * subframes
             */
            hdrrmac4_var_ptr->phy_chan[phy_chan].flow[fid].bucket.tt2p_hold.prev =
              hdrrmac4_grant_msg.rl_channel_record[log_chan].
                                  mac_flows[flow].tt2p_hold * 4;
            hdrrmac4_var_ptr->phy_chan[phy_chan].flow[fid].bucket.tt2p_hold.curr =
              hdrrmac4_grant_msg.rl_channel_record[log_chan].
                                  mac_flows[flow].tt2p_hold * 4;

            /* Copy fields from Grant msg into temp variables for convenience */
            bucket_level = hdrrmac4_grant_msg.rl_channel_record[log_chan].
                                                mac_flows[flow].bucket_level;
            t2p_inflow = hdrrmac4_grant_msg.rl_channel_record[log_chan].
                                                mac_flows[flow].t2p_inflow;

            if( bucket_level == 255 )
            {
              /* 255 indicates a value of -infinity dB, which is 0 linear */
              hdrrmac4_var_ptr->phy_chan[phy_chan].
                flow[fid].bucket.bucket_level.curr = 0;
              hdrrmac4_var_ptr->phy_chan[phy_chan].
                flow[fid].bucket.bucket_level.prev = 0;
            }
            else
            {
              /* Use LUT to convert from dB (0..63.5 in 1/4 dB units) to 
               * linear Q22.10 format
               */
              bucket_level_lin = hdrrmacmath_db_2_lin_q22x10( bucket_level );
              hdrrmac4_var_ptr->phy_chan[phy_chan].
                flow[fid].bucket.bucket_level.curr = 
                (int32) MIN( (int32) 0x7FFFFFFF, (int64) bucket_level_lin );
              hdrrmac4_var_ptr->phy_chan[phy_chan].
                flow[fid].bucket.bucket_level.prev = 
                hdrrmac4_var_ptr->phy_chan[phy_chan].
                  flow[fid].bucket.bucket_level.curr;
            }

            if( t2p_inflow == 255 )
            {
              /* 255 indicates a value of -infinity dB, which is 0 linear */
              hdrrmac4_var_ptr->phy_chan[phy_chan].flow[fid].bucket.t2p_inflow.curr
                =0;
              hdrrmac4_var_ptr->phy_chan[phy_chan].flow[fid].bucket.t2p_inflow.prev
                =0;
            }
            else
            {
              /* Use LUT to convert from dB (0..63.5 in 1/4 dB units) to 
               * linear Q22.10 format
               */
              hdrrmac4_var_ptr->phy_chan[phy_chan].flow[fid].bucket.t2p_inflow.curr
                = hdrrmacmath_db_2_lin_q22x10( t2p_inflow );
              hdrrmac4_var_ptr->phy_chan[phy_chan].flow[fid].bucket.t2p_inflow.prev
                = hdrrmacmath_db_2_lin_q22x10( t2p_inflow );
            }

            HDR_MSG_PROT_2( MSG_LEGACY_HIGH,
               "Grant Message Received Log Chan %d Phy Chan %d",
               hdrrmac4_grant_msg.rl_channel_record[log_chan].
                                      reverse_logical_channel,
               phy_chan);

            HDR_MSG_PROT_4( MSG_LEGACY_HIGH, 
               "Grant Message Received Flow: %d BL: %d T2P: %u TT2P: %d",
               fid,
               hdrrmac4_var_ptr->phy_chan[phy_chan].
                            flow[fid].bucket.bucket_level.curr,
               hdrrmac4_var_ptr->phy_chan[phy_chan].
                            flow[fid].bucket.t2p_inflow.curr,
               hdrrmac4_var_ptr->phy_chan[phy_chan].
                            flow[fid].bucket.tt2p_hold.curr );
          }
        }
      }
    }
    else
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Error: %d unpacking Grant Msg", errnum );
    }
  }
  else
  {
    HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_RTC_MAC_PROTOCOL,
      HDRRMAC4_GRANT_MSG_ID, (uint8) hdrrmac4_common->state );
  }

  return;

} /* hdrrmac4_proc_grant_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UNPACK_RL_DROP_ACK_MSG                             INTERNAL

DESCRIPTION
  This function unpacks the received ReverseCDMAChannelDroppedAck message 
  into the location provided.

DEPENDENCIES
  rl_drop_ack_msg_ptr should point to valid memory.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  rl_drop_ack_msg_ptr - Location into which to extract the received message

RETURN VALUE
  E_SUCCESS  - If extraction & unpacking of the message succeeds & field values 
               are valid.
  E_NO_DATA  - If there was an error extracting the message.

SIDE EFFECTS
  None

===========================================================================*/
RMAC4_LOCAL hdrerrno_enum_type hdrrmac4_unpack_rl_drop_ack_msg
(
  dsm_item_type *item_ptr,
  hdrrmac4_rl_drop_ack_msg_type *rl_drop_ack_msg_ptr
)
{
  int  offset;                      /* Offset(bits) of field from msg start */
  int  err;                         /* Error value */
  hdrerrno_enum_type unpack_result; /* Return value of unpack Grant msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset = HDRRMAC4_RL_DROP_ACK_MSG_ID_SIZE;
  err = 0; /* E_SUCCESS = 0 */
  unpack_result = E_SUCCESS;

  /* Unpack the received message */
  err |= (int) HDRRMAC4_RL_DROP_ACK_TRANS_ID(
                item_ptr,
                &rl_drop_ack_msg_ptr->transaction_id,
                offset);

  if (err != 0)
  {
    unpack_result = E_NO_DATA;

    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Error unpacking Grant Msg" );
  }

  return unpack_result;

} /* hdrrmac4_unpack_rl_drop_ack_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_RL_DROP_ACK_MSG                           INTERNAL

DESCRIPTION
  This function processes the ReverseCDMAChannelDroppedAck message.

DEPENDENCIES
  None.

PARAMETERS
  rcv_msg_ptr - pointer to received message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_process_rl_drop_ack_msg
(
  dsm_item_type * rcv_msg_ptr
)
{
  hdrerrno_enum_type      errnum;
    /* Result of unpacking ReverseCDMAChannelDroppedAck message */

  hdrrmac4_rl_drop_ack_msg_type rl_drop_ack_msg;
    /* local storage of unpacked message */

  uint8 entry;
    /* rl_drop entry number */

  boolean found = FALSE;
    /* found an entry matching the trans_id of the drop ack msg?*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  errnum = hdrrmac4_unpack_rl_drop_ack_msg( rcv_msg_ptr, &rl_drop_ack_msg);

  if ( errnum == E_SUCCESS )
  {
    for (entry = 0; 
         (!found &&
          (entry < HDRRMAC4_MAX_NUM_DROP_MSGS)); 
         entry++)
    {
      if ((rl_drop_ack_msg.transaction_id == 
           hdrrmac4_rl_drop_var.rl_drop_msg[entry].rl_dropped_msg.trans_id) &&
          hdrrmac4_rl_drop_var.rl_drop_msg[entry].in_use)
      {
        found = TRUE;
      }
    }

    if(found)
    {
      /* Compensate for extra increment provided by for loop */
      entry--;

      /* Cancel the ACK timer */
      hdrrmac4_stop_rl_drop_ack_timer(entry);
    }

  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "Error: %d unpacking ReverseCDMAChannelDroppedAck Msg", 
                    errnum );
  }

  return; 

} /* hdrrmac4_process_rl_drop_ack_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_AWAY_COMPENSATE                                   INTERNAL

DESCRIPTION
  Perform catch up calculations to the correct values of T2PInflow and
  BucketLevelSat to make up for not updating them during HHO/Tuneaway.

DEPENDENCIES
  None.

PARAMETERS
  ms - ms that connection was suspended
  hho_flag - FALSE -> resume from tune-away
             TRUE  -> resume from hard-handoff

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_away_compensate
(
  dword   ms,
    /* time, in mS, that connection was suspended. */

  boolean hho_flag
    /* FALSE - resume from tune-away
     * TRUE  - resume from hard-handoff
     */
)
{
  int awaytime_sf = 0; 
    /* time in subframes for which hdr was suspended */

  uint32 t2p_decay_coef;
    /* t2p_decay_coef is an intermediate variable used to calculate
     * t2p_decay_factor. It is = (1 - 1/T2PNoTxFilterTC)
     */

  uint32 t2p_decay_factor_global, t2p_decay_factor;
    /* t2p_decaf_factor represents the amount by which to scale T2PInflow
     * to make up for the tuneaway
     */

  uint8 flow;
    /* loop index */

  uint8 chan_index;
    /* channel loop index */

  uint8 active_flow_id;
  
  hdrrmac4_phy_chan_vars_struct_type  *phy_chan_ptr;
    /* convenience pointer */

  hdrrmaccmn_t2p_inflow_range_struct_type *t2p_inflow_range_ptr;
    /* convenience pointer */

  uint32 tc = hdrrmac4_config_ptr->filt_time_const.t2p_no_tx;
    /* shortcut for T2PNoTx filter time constant */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  uint32 decay_coef;
    /* decay_coef is an intermediate variable used to calculate
     * decay_factor. It is = (1 - 1/ModemAPIFilterTC)
     */

  uint32 decay_factor;
    /* decaf_factor represents the amount by which to scale TxOnFactor
     * to make up for the tuneaway
     */
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert away time in ms to subframes; 1 subframe = 20/3 ms */
  awaytime_sf = (int) (( (ms * 3) + 10) / 20);

  /* Compute decay coefficient and decay factor (common for all flows) */
  t2p_decay_coef = (( (uint64) (HDRRMACCMN_T2P_NO_TX_FILT_SCALING_FACTOR - 
    (HDRRMACCMN_T2P_NO_TX_FILT_SCALING_FACTOR / tc )) << 32) + 
    (HDRRMACCMN_T2P_NO_TX_FILT_SCALING_FACTOR >> 1)) / 
    HDRRMACCMN_T2P_NO_TX_FILT_SCALING_FACTOR;

  t2p_decay_factor_global = 
    hdrrmacmath_exp_2( awaytime_sf * ( hdrrmacmath_log_2(t2p_decay_coef) 
                                        - ( 32 * (1 << 20) ) ) ); 

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
  if (hho_flag)
  {
    /* Clear history since we are on new channel */
    hdrrmac4drv_clear_carrier_history();
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  decay_coef = (( (uint64) (HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR - 
    (HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / HDRRMACCMN_MODEMAPI_FILT_TC )) 
    << 32) + (HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR >> 1)) / 
    HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR;

  decay_factor = 
    hdrrmacmath_exp_2( awaytime_sf * ( hdrrmacmath_log_2(decay_coef) 
                                        - ( 32 * (1 << 20) ) ) ); 
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

  for ( chan_index = 0; 
        chan_index < hdrrmac4_var_ptr->num_open_state_chans; 
        chan_index++)
  {
    uint8 chan = hdrrmac4_open_rl_list[chan_index];

    phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
    /* Decay TxOnFactor */
    phy_chan_ptr->tx_on_factor = (uint32) 
      ( ( ((uint64) (decay_factor)) * phy_chan_ptr->tx_on_factor 
      + 0x8000 ) >> 16 );
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

    for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
    {
      active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

      t2p_decay_factor = t2p_decay_factor_global;

      if( hho_flag ) /* returning from HHO */
      {
        /* Clear tt2p_hold */
        phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.prev = 0;
        phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr = 0;
      }

      /* Check if T2PHold from a Grant is still in effect */
      if( phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr 
          >= awaytime_sf )
      {
        /* No need to update T2pInflow and BucketLevelSat for this flow */
        phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr -= 
          awaytime_sf;
      }
      else 
      {
        if( ( phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr > 0 )
            && ( phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr 
            < awaytime_sf ) )
        {
          /* Reduce the tuneaway duration by TT2PHold, because T2P does
           * not decay as long as a Granted value is in effect, i.e., during
           * TT2PHold. Use this adjusted tuneaway duration to recalculate 
           * decay factor
           */ 
          int temp_awaytime_sf = awaytime_sf - 
            phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr;

          t2p_decay_factor = 
            hdrrmacmath_exp_2( temp_awaytime_sf * 
                                ( hdrrmacmath_log_2(t2p_decay_coef) 
                                  - ( 32 * (1 << 20) ) ) ); 

          phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.prev = 0;
          phy_chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr = 0;
        }

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
        /* In case of HHO reset bucket level, else for tuneaway
           freeze the bucket */
        if( hho_flag ) /* returning from HHO */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */
        {
          phy_chan_ptr->flow[active_flow_id].bucket.bucket_level.curr = 0;
        }

        phy_chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr = 0;

        phy_chan_ptr->flow[active_flow_id].bucket.delta_t2p_inflow = 0;

        phy_chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr = (uint32) 
          ( ( ((uint64) (t2p_decay_factor)) * phy_chan_ptr->
          flow[active_flow_id].bucket.t2p_inflow.curr + 0x8000 ) >> 16 );

        /* Ensure T2PInflow is within T2PInflow min-max */
        t2p_inflow_range_ptr =
          hdrrmac4_config_ptr->nc[active_flow_id][phy_chan_ptr->log_chan_id].
            t2p_inflow_range_ptr;
        if( phy_chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr
            > t2p_inflow_range_ptr->max )
        {
          phy_chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr = 
            t2p_inflow_range_ptr->max;
        }
        else if(  phy_chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr
                  < t2p_inflow_range_ptr->min )
        {
          phy_chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr = 
            t2p_inflow_range_ptr->min;
        }
      }
    } /* for each active flow */
  } /* for each channel */

  return;

} /* hdrrmac4_away_compensate */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_LOG_T2P_RESOURCE_INFO                              INTERNAL

DESCRIPTION
  This function logs the T2P resource info log packet

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_log_t2p_resource_info(void)
{
  LOG_HDR_REVB_RMAC4_T2P_RESOURCE_C_type  *log_ptr;
  uint8                                   flow, chan;
  hdrlog_rmac4_chan_wide_info_type        *log_chan_ptr;
  hdrlog_rmac4_flow_info_type             *log_flow_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is a variable size record. Allocate just enough memory for the
   * log buffer to accommodate diag data for the number of channels and
   * flows.
   */

  log_ptr = (LOG_HDR_REVB_RMAC4_T2P_RESOURCE_C_type *)
            log_alloc_ex( (log_code_type) LOG_HDR_REVB_RMAC4_T2P_RESOURCE_C ,
                          (sizeof(LOG_HDR_REVB_RMAC4_T2P_RESOURCE_C_type) -
                           sizeof(hdrlog_rmac4_chan_wide_info_type) + 
                           (hdrrmac4_var_ptr->num_open_state_chans *
                            (sizeof(hdrlog_rmac4_chan_wide_info_type) +
                             ((hdrrmac4_num_active_flows - 1) * 
                              sizeof(hdrlog_rmac4_flow_info_type))))) );

  if ( log_ptr != NULL )
  {
    log_ptr->version = HDRLOG_REVB_RMAC4_T2P_RESOURCE_VER;

    /* Log the current sub-frame count  */
    log_ptr->sub_frame_count = hdrrmac4_logging.sub_frame_count;

    log_ptr->num_carriers = hdrrmac4_var_ptr->num_open_state_chans;

    log_ptr->num_flows = hdrrmac4_num_active_flows;

    /* init. the log packet channel record pointer */
    log_chan_ptr = &log_ptr->carrier_info[0];

    /* Fill in Chan-wide info */
    for (chan = 0; chan < log_ptr->num_carriers; chan++)
    {
      uint16 curr_tx_pkt_id;
      uint8 active_chan = hdrrmac4_open_rl_list[chan];

      log_chan_ptr->carrier_id.channel_id = active_chan;

      /* RF Channel Information */
      log_chan_ptr->carrier_id.channel_no = 
        hdrrmac4_var_ptr->phy_chan[active_chan].chan_record.chan_num;
      log_chan_ptr->carrier_id.band_class = (uint16)((uint32) (int32)
        hdrrmac4_var_ptr->phy_chan[active_chan].chan_record.band);

      /* Packet ID */
      curr_tx_pkt_id = log_chan_ptr->pkt_id =
          hdrrmac4_var_ptr->phy_chan[active_chan].curr_tx_pkt_id;

      /* HDR Interrupted */
      log_chan_ptr->hdr_interrupted = (uint8)
        hdrrmac4_var_ptr->phy_chan[active_chan].hdr_inactive_reason;

      /* Packet Size Index & SumPayload */
      if (curr_tx_pkt_id != HDRRMAC4DRV_IDLE_PACKET)
      {
        log_chan_ptr->pkt_size_index = (uint8)
          hdrrmac4_hist[active_chan][curr_tx_pkt_id].pkt_size_index;

        log_chan_ptr->sum_payload =
          hdrrmac4_hist[active_chan][curr_tx_pkt_id].sum_payload;
      }
      else
      {
        log_chan_ptr->pkt_size_index = 0;
        log_chan_ptr->sum_payload = 0;
      }

      /* TXT2P */
      log_chan_ptr->tx_t2p = hdrrmac4_var_ptr->phy_chan[active_chan].tx_t2p;

      /* PilotStrength(pilot_strength) */
      log_chan_ptr->pilot_strength_ps =
        hdrrmac4_var_ptr->phy_chan[active_chan].sector_info.pilot_strength;

      /* FRAB */
      log_chan_ptr->frab =
        hdrrmac4_var_ptr->phy_chan[active_chan].sector_info.frab;

      /* init. the flow pointer */
      log_flow_ptr = &log_chan_ptr->flow_info[0];

      /* Fill in flow-specfic info */
      for (flow = 0; flow < log_ptr->num_flows; flow++)
      {
        hdrrmac4_per_nc_vars_struct_type *flow_ptr;
          /* convenience pointer to flow data structure */

        uint8                           fid;
          /* temp variable to store MAC flow ID used to index into history buffer */

        fid = hdrrmac4_active_flows_set[flow].mac_flow_id;

        flow_ptr = &(hdrrmac4_var_ptr->phy_chan[active_chan].flow[fid]);

        /* Actual size in bytes */
        if (curr_tx_pkt_id != HDRRMAC4DRV_IDLE_PACKET)
        {
          /* Fixing KW errors */ 
          HDR_ASSERT(curr_tx_pkt_id < HDRRMAC4_HIST_SIZE);

          log_flow_ptr->flow_allotment =
            hdrrmac4_hist[active_chan][curr_tx_pkt_id].flows[fid].size;
        }
        else
        {
          log_flow_ptr->flow_allotment = 0;
        }

        /* T2POutflow */
        log_flow_ptr->t2p_outflow = flow_ptr->bucket.t2p_outflow.curr;

        /* BucketLevel */
        log_flow_ptr->bucket_level = flow_ptr->bucket.bucket_level.curr;

        /* QRAB */
        log_flow_ptr->qrab = (int8) flow_ptr->qrab_flow;

        /* mac_flow_id */
        log_flow_ptr->mac_flow_id = fid;

        /* DeltaT2PInflow */
        log_flow_ptr->delta_t2p_inflow = flow_ptr->bucket.delta_t2p_inflow;

        /* T2PInflow */
        log_flow_ptr->t2p_inflow = flow_ptr->bucket.t2p_inflow.curr;

        /* BucketLevelSat */
        log_flow_ptr->bucket_level_sat = flow_ptr->bucket.bucket_level_sat;

        /* BucketFactor */
        log_flow_ptr->bucket_factor = 
          flow_ptr->bucket.bucket_factor;

        /* TT2PHold */
        log_flow_ptr->tt2p_hold = flow_ptr->bucket.tt2p_hold.curr;

        /* increment the flow pointer */
        log_flow_ptr++;
      }

      /* Set the address of the next channel record */
      log_chan_ptr = (hdrlog_rmac4_chan_wide_info_type *)log_flow_ptr;
    }

    /* submit the log to DIAG */
    log_commit( (log_type*) log_ptr );

  } /* if */

  return;

} /* hdrrmac4_log_t2p_resource_info */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_LOG_PKT_INFO                                      INTERNAL

DESCRIPTION
  This function logs packet determination info

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_log_pkt_info(void)
{
  LOG_HDR_REVB_RMAC4_PKT_INFO_C_type    *log_ptr;
  uint8                                 flow, chan;
  uint8                                 mac_flow_id;
    /* temporary storage  */
  uint8                                 i;  
    /* loop index         */
  hdrlog_rmac4_pkt_data_flow_info_type  *log_data_flow_ptr;
  hdrlog_rmac4_pkt_chan_wide_info_type  *log_chan_ptr;
  hdrlog_rmac4_pkt_flow_info_type       *log_flow_ptr;
  hdrrmaccmn_tx_mode_enum_type          tm;
    /* temporary storage  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is a variable size record. Allocate just enough memory for the
   * log buffer to accommodate diag data for the number of channels and
   * flows.
   */

  log_ptr = (LOG_HDR_REVB_RMAC4_PKT_INFO_C_type *)
            log_alloc_ex( (log_code_type) LOG_HDR_REVB_RMAC4_PKT_INFO_C,
                          (sizeof(LOG_HDR_REVB_RMAC4_PKT_INFO_C_type) -
                           sizeof(hdrlog_rmac4_pkt_data_flow_info_type) +
                           (hdrrmac4_num_active_flows * 
                            sizeof(hdrlog_rmac4_pkt_data_flow_info_type)) -
                           sizeof(hdrlog_rmac4_pkt_chan_wide_info_type) +
                           (hdrrmac4_var_ptr->num_open_state_chans *
                            (sizeof(hdrlog_rmac4_pkt_chan_wide_info_type) -
                             sizeof(hdrlog_rmac4_pkt_flow_info_type) +
                             (hdrrmac4_num_active_flows *
                              sizeof(hdrlog_rmac4_pkt_flow_info_type))))) );

  if( log_ptr != NULL )
  {
    log_ptr->version = HDRLOG_REVB_RMAC4_PKT_INFO_VER;

    /* Log the current sub-frame count  */
    log_ptr->sub_frame_count = hdrrmac4_logging.sub_frame_count;

    log_ptr->num_carriers = hdrrmac4_var_ptr->num_open_state_chans;

    if (hdrrmac4_hdr_mode_active)
    {
      log_ptr->num_flows = hdrrmac4_num_active_flows;
    }
    else
    {
      log_ptr->num_flows = 0;
    }

    /* combo ID */
    log_ptr->combo_id = HDRRMAC4_DEFAULT_COMBO_ID;

    /* SLP carrier */
    log_ptr->at_ps_info.slp_carrier_id = hdrrmac4_var_ptr->slp_carrier_idx;

    /* 2nd iteration information */
    log_ptr->at_ps_info.two_iterations_executed = 
      hdrrmac4_logging.two_iterations;

    /* init. the log packet data flow record pointer */
    log_data_flow_ptr = &log_ptr->flow_info[0];

    /* Fill in data flow-specific info */
    for( flow = 0; flow < log_ptr->num_flows; flow++ )
    {
      mac_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

      log_data_flow_ptr->mac_flow_id = mac_flow_id;

      /* SumTransmittedData */
      log_data_flow_ptr->sum_transmitted_data =
        hdrrmac4_var_ptr->data_flow[mac_flow_id].data_token.sum_transmitted_data;

      /* DataBucketLevel */
      log_data_flow_ptr->data_bucket_level =
        hdrrmac4_var_ptr->data_flow[mac_flow_id].data_token.data_bucket_level.curr;

      /* increment to the next record */
      log_data_flow_ptr++;
    }

    /* init. the log packet channel record pointer */
    log_chan_ptr = (hdrlog_rmac4_pkt_chan_wide_info_type *)log_data_flow_ptr;

    /* Fill in channel specific info */
    for (chan = 0; chan < log_ptr->num_carriers; chan++)
    {
      uint8 active_chan = hdrrmac4_open_rl_list[chan];

      log_chan_ptr->carrier_id.channel_id = active_chan;

      /* RF Channel Information */
      log_chan_ptr->carrier_id.channel_no = 
        hdrrmac4_var_ptr->phy_chan[active_chan].chan_record.chan_num;
      log_chan_ptr->carrier_id.band_class = (uint16) ((uint32) (int32)
        hdrrmac4_var_ptr->phy_chan[active_chan].chan_record.band);

      /* Channel Priorities */
      log_chan_ptr->priority.rtcmac = 
        hdrrmac4_var_ptr->phy_chan[active_chan].rtcmac_pri;
      log_chan_ptr->priority.caput =
        hdrrmac4_var_ptr->phy_chan[active_chan].caput_pri;

      /* cand_ps_set */
      log_chan_ptr->cand_ps_set = hdrrmac4_logging.cand_ps_set[active_chan];

      /* rand_num */
      log_chan_ptr->rand_num = hdrrmac4_logging.rand_num[active_chan];

      /* TxT2PMax */
      log_chan_ptr->tx_t2p_max = hdrrmac4_logging.tx_t2p_max[active_chan];

      /* Round Down Quantum */
      log_chan_ptr->round_down_quantum = 
        hdrrmac4_logging.round_down_quantum[active_chan];

      log_chan_ptr->ps_info.final_ps_index =
        hdrrmac4_logging.ps_info[active_chan].final_ps_index;
      tm = (hdrrmaccmn_tx_mode_enum_type) hdrrmac4_logging.ps_info[active_chan].tm;
      log_chan_ptr->ps_info.tm = (uint8) tm;

      /* init. the flow record pointer within the channel record */
      log_flow_ptr = &log_chan_ptr->flow_info[0];

      /* Fill in flow-specific info */
      for( flow = 0; flow < log_ptr->num_flows; flow++ )
      {
        mac_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

        log_flow_ptr->flow_set_info.mac_flow_id = mac_flow_id;

        log_flow_ptr->initial_queue_size =
          hdrrmac4_var_ptr->phy_chan[active_chan].flow[mac_flow_id].
                              queue_info.initial_size;

        log_flow_ptr->pot_t2p_outflow =
          hdrrmac4_var_ptr->phy_chan[active_chan].flow[mac_flow_id].
                              bucket.pot_t2p_outflow[tm];

        log_flow_ptr->q_outflow =
          hdrrmac4_var_ptr->phy_chan[active_chan].flow[mac_flow_id].
                              queue_info.q_outflow[tm];

        log_flow_ptr->suggested_flow_allocation = 
          hdrrmac4_var_ptr->phy_chan[active_chan].flow[mac_flow_id].
                              queue_info.suggested_size;

        log_flow_ptr->actual_flow_allocation =
          hdrrmac4_var_ptr->phy_chan[active_chan].flow[mac_flow_id].
                              queue_info.actual_size;

        /* The following loop checks whether the current flow belongs to
           the non-empty flow set                                       */
        log_flow_ptr->flow_set_info.non_empty_flow_set = 0;

        for(i = 0; i < hdrrmac4_num_non_empty_flows[active_chan]; i++)
        {
          if ( mac_flow_id == 
               hdrrmac4_non_empty_flows_set[active_chan][i].mac_flow_id )
          {
            log_flow_ptr->flow_set_info.non_empty_flow_set = 1;
            break;
          } /* if */
        } /* for */

        /* The following loop checks whether the current flow belongs to
           the eligible flow set                                       */
        log_flow_ptr->flow_set_info.eligible_flow_set = 0;

        for(i = 0; i < hdrrmac4_num_elig_flows[active_chan]; i++)
        {
          if ( mac_flow_id == 
               hdrrmac4_elig_flows_set[active_chan][i].mac_flow_id )
          {
            log_flow_ptr->flow_set_info.eligible_flow_set = 1;
            break;
          } /* if */
        } /* for */

        /* increment to the next record */
        log_flow_ptr++;
      } /* for flow */

      log_chan_ptr = (hdrlog_rmac4_pkt_chan_wide_info_type *) log_flow_ptr;

    } /* for channel */

    /* submit the log to DIAG */
    log_commit( (log_type*) log_ptr );

  } /* if log_ptr != NULL */

  return;

} /* hdrrmac4_log_pkt_info  */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_LOG_PKT_TIME_INFO                                  INTERNAL

DESCRIPTION
  This function logs packet build/load time info.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_log_pkt_time_info(void)
{
  LOG_HDR_RMAC_PKT_TIME_INFO_C_type     *log_ptr;
  uint8                                 chan;
  hdrlog_rmac_pkt_time_chan_wide_info_type  *log_chan_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is a variable size record. Allocate just enough memory for the
   * log buffer to accommodate diag data for the number of channels
   */

  log_ptr = (LOG_HDR_RMAC_PKT_TIME_INFO_C_type *)
            log_alloc_ex( (log_code_type) LOG_HDR_RMAC_PKT_TIME_INFO_C,
                          (sizeof(LOG_HDR_RMAC_PKT_TIME_INFO_C_type) -
                           sizeof(hdrlog_rmac_pkt_time_chan_wide_info_type) +
                           (hdrrmac4_var_ptr->num_open_state_chans *
                            sizeof(hdrlog_rmac_pkt_time_chan_wide_info_type))) );

  if ( log_ptr != NULL )
  {
    log_ptr->version = HDRLOG_RMAC_PKT_TIME_INFO_VER;

    /* Log the current sub-frame count  */
    log_ptr->sub_frame_count = hdrrmac4_logging.sub_frame_count;

    log_ptr->num_carriers = hdrrmac4_var_ptr->num_open_state_chans;

    /* enc_status */
    if (hdrrmac4_var_ptr->enc_window_missed)
    {
      log_ptr->enc_status = HDRLOG_ENC_STATUS_SW_TOO_LATE;
    }
    else
    {
      log_ptr->enc_status = HDRLOG_ENC_STATUS_NORMAL;
    }

    /* frame_build_time */
    log_ptr->frame_build_time =
      hdrrmaccmn_ts_calc_elapsed_time(HDRRMACCMN_TS_FRAME_BUILD_START,
                                      HDRRMACCMN_TS_FRAME_BUILD_END);
    /* frame_load_time */
    log_ptr->frame_load_time =
      hdrrmaccmn_ts_calc_elapsed_time(HDRRMACCMN_TS_FRAME_LOAD_START,
                                      HDRRMACCMN_TS_FRAME_LOAD_END);

    /* rpt_pkt_status_time */
    log_ptr->rpt_pkt_status_time =
      hdrrmaccmn_ts_calc_elapsed_time(HDRRMACCMN_TS_RPT_PKT_STATUS_START,
                                      HDRRMACCMN_TS_RPT_PKT_STATUS_END);

    /* get_pkt_info_time */
    log_ptr->get_pkt_info_time =
      hdrrmaccmn_ts_calc_elapsed_time(HDRRMACCMN_TS_GET_PKT_INFO_START,
                                      HDRRMACCMN_TS_GET_PKT_INFO_END);

    /* get_pkt_time */
    log_ptr->get_pkt_time =
      hdrrmaccmn_ts_calc_elapsed_time(HDRRMACCMN_TS_GET_PKT_START,
                                      HDRRMACCMN_TS_GET_PKT_END);

    /* init. the log packet channel record pointer */
    log_chan_ptr = &log_ptr->carrier_info[0];

    /* Fill in channel specific info */
    for (chan = 0; chan < log_ptr->num_carriers; chan++)
    {
      uint8 active_chan = hdrrmac4_open_rl_list[chan];

      log_chan_ptr->carrier_id.channel_id = active_chan;

      /* RF Channel Information */
      log_chan_ptr->carrier_id.channel_no = 
        hdrrmac4_var_ptr->phy_chan[active_chan].chan_record.chan_num;
      log_chan_ptr->carrier_id.band_class = (uint16) ((uint32) (int32)
        hdrrmac4_var_ptr->phy_chan[active_chan].chan_record.band);

      /* pkt_info: num_mac_flows_in_pkt */
      log_chan_ptr->pkt_info.num_mac_flows_in_pkt =
        hdrrmac4_logging.num_mac_flows_with_data[active_chan];

      /* pkt_info: is_format_b  */
      if (hdrrmac4_var_ptr->phy_chan[active_chan].pkt_info.format ==
          HDRRMAC4DRV_FORMAT_B_PACKET)
      {
        log_chan_ptr->pkt_info.is_format_b = TRUE;
      }
      else
      {
        log_chan_ptr->pkt_info.is_format_b = FALSE;
      }

      /* pkt_info: prev_pkt_status */
      if (hdrrmac4_var_ptr->phy_chan[active_chan].curr_tx_pkt_id ==
          HDRRMAC4DRV_IDLE_PACKET)
      {
        log_chan_ptr->pkt_info.prev_pkt_status = HDRLOG_PREV_PKT_IDLE;
      }
      else
      {
        if (hdrrmac4_logging.prev_pkt_accepted[active_chan])
        {
          log_chan_ptr->pkt_info.prev_pkt_status = HDRLOG_PREV_PKT_ACCEPTED;
        }
        else
        {
          log_chan_ptr->pkt_info.prev_pkt_status = HDRLOG_PREV_PKT_CANCELED;
        }
      }

      /* pkt_info: num_app_pkts_in_pkt */
      log_chan_ptr->pkt_info.num_app_pkts_in_pkt =
        hdrrmac4_logging.num_pkts_in_mac_pkt[active_chan];

      /* pkt_info: pkt_size_index */
      log_chan_ptr->pkt_info.pkt_size_index = (uint16)
        hdrrmac4_var_ptr->phy_chan[active_chan].pkt_size_index;

      /* pkt_info: data_bytes_len */
      log_chan_ptr->pkt_info.data_bytes_len =
        hdrrmac4_var_ptr->phy_chan[active_chan].actual_pkt_size;

      log_chan_ptr++;
    }

    /* submit the log to DIAG */
    log_commit( (log_type*) log_ptr );

  }

  return;

} /* hdrrmac4_log_pkt_time_info */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_LOG_DIAG_DATA                                      INTERNAL

DESCRIPTION
  Top level logging function

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_log_rmac4_diagnostics( void )
{

      /* fill in the T2P resource information fields */
  hdrrmac4_log_t2p_resource_info( );

  /* fill in the packet information fields */
  hdrrmac4_log_pkt_info( );

  /* fill in the packet build/load time information fields */
  hdrrmac4_log_pkt_time_info();

  return;
}


 /* EJECT */
/* ========================================================================== */
/* ========================RTCMAC Subtype 4 Algorithm======================== */
/* ========================================================================== */

 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_FLOW_QRAB                                   INTERNAL

DESCRIPTION
  This function reads sector load related information (PilotStrength, QRAB,
  QRAB_ps and FRAB) from the lower layer and updates QRAB_i, the QRAB for each
  active MAC flow.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Sector information is updated

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_update_flow_qrab( void )
{
  uint8 flow;
  uint8 chan_index;
  uint8 active_flow_id;
  hdrrmac4drv_channel_sector_record_type 
    chan_sector_info[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (chan_index = 0;
       chan_index < hdrrmac4_var_ptr->num_open_state_chans;
       chan_index++)
  {
    uint8 chan = hdrrmac4_open_rl_list[chan_index];

    chan_sector_info[chan_index].phy_chan_id = chan;

    chan_sector_info[chan_index].sector_info = 
      &(hdrrmac4_var_ptr->phy_chan[chan].sector_info);
  }

  /* Fixing KW errors */  
  HDR_ASSERT(hdrrmac4_var_ptr->num_open_state_chans <= 
                                  HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS);

  /* Update local copy of per-carrier sector information. Lower layer 
     is responsible for all sector related calculations */
  hdrrmac4drv_get_sector_info( hdrrmac4_var_ptr->num_open_state_chans,
    &chan_sector_info[0] );

#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
  #error code not present
#else
  for (chan_index = 0;
       chan_index < hdrrmac4_var_ptr->num_open_state_chans;
       chan_index++)
  {
    uint8 chan = hdrrmac4_open_rl_list[chan_index];

    /* C.6.a Update QRAB_i by choosing qrab or qrab_ps based on qrab_select 
     * switch for each MAC flow 
     */
    for( flow = 0; flow < hdrrmac4_num_active_flows; flow++)
    {
      active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

      if( ( HDRRMAC4_QRAB_SEL_IS_QRAB_PS( active_flow_id ) == TRUE
              && (hdrrmac4_var_ptr->phy_chan[chan].sector_info.qrab_ps == 
              (int16) HDRRMAC4DRV_QRAB_LOADED) )
            ||
          ( HDRRMAC4_QRAB_SEL_IS_QRAB( active_flow_id ) == TRUE
              && (hdrrmac4_var_ptr->phy_chan[chan].sector_info.qrab == 
              HDRRMAC4DRV_QRAB_LOADED)) )
      {
        hdrrmac4_var_ptr->phy_chan[chan].flow[active_flow_id].qrab_flow = 
          HDRRMAC4DRV_QRAB_LOADED;
      }
      else
      {
        hdrrmac4_var_ptr->phy_chan[chan].flow[active_flow_id].qrab_flow = 
          HDRRMAC4DRV_QRAB_UNLOADED;
      }
    }
  }
#endif

  return;

} /* hdrrmac4_update_flow_qrab */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_INIT_BUCKET                                         INTERNAL

DESCRIPTION
  This function initializes bucket parameters for newly activated MAC flows

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
RMAC4_LOCAL void hdrrmac4_init_bucket( void )
{
  uint8 flow;
  uint8 chan_index;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* C.5 Init Bucket for newly activated MAC flows */
  for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++)
  {
    if( HDRRMAC4_IS_MAC_FLOW_NEW( flow ) )
    {
      /* DataBucketLevel = DataBucketLevelMax */
      hdrrmac4_var_ptr->data_flow[flow].data_token.data_bucket_level.prev =
        hdrrmac4_config_ptr->flow[flow].data_token_bucket.
          data_bucket_level_max;

      hdrrmac4_var_ptr->data_flow[flow].bucket.last_bucket_level_max = 
          hdrrmac4_config_ptr->flow[flow].bucket_level_max;

      for (chan_index = 0;
           chan_index < hdrrmac4_var_ptr->num_open_state_chans;
           chan_index++)
      {
        uint8 chan = hdrrmac4_open_rl_list[chan_index];

        uint8 log_chan_id;
          /* Logical Channel ID */

        log_chan_id = hdrrmac4_from_phy_to_log(chan);

        ASSERT(log_chan_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS);

        /* The first two initializations will make the effective initial value 
         * of bucket_level equal to 0 for newly activated MAC flows when the 
         * bucket levels are updated (see * hdrrmac4_update_bucket_level()) for
         * all flows. This avoids the extra step of reimbursing the buckets 
         * after the T2P resource has been deducted assuming that the packet 
         * will be sent, whereas a packet may not be transmitted due to a NAK.
         */
        hdrrmac4_var_ptr->phy_chan[chan].flow[flow].bucket.bucket_level.prev = 
          -( (int32) hdrrmac4_config_ptr->nc[flow][log_chan_id].
                        t2p_inflow_range_ptr->min );
        hdrrmac4_var_ptr->phy_chan[chan].flow[flow].bucket.t2p_inflow.prev   = 
          hdrrmac4_config_ptr->nc[flow][log_chan_id].
            t2p_inflow_range_ptr->min;

        hdrrmac4_var_ptr->phy_chan[chan].flow[flow].bucket.tt2p_hold.prev    =
          0;

        hdrrmac4_var_ptr->phy_chan[chan].flow[flow].bucket.bucket_level_sat  = 
          hdrrmac4_config_ptr->flow[flow].bucket_level_max;

      } /* for each physical channel */
    } /* if a flow is newly activated */
  } /* for each flow */

  return;

} /* hdrrmac4_init_bucket */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_UPDATE_BUCKET_LEVEL                                INTERNAL

DESCRIPTION
  This function updates the bucket level for each MAC flow by adding the net
  difference between the T2PInflow from previous subframe and the T2POutflow
  from current subframe to the bucket.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
RMAC4_LOCAL void hdrrmac4_update_bucket_level( void )
{
  uint8 flow; 
  uint8 active_flow_id;
  uint8 chan_index;
  uint16 pkt_id;
  hdrrmac4_phy_chan_vars_struct_type *chan_ptr;
    /* convenience pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (chan_index = 0;
       chan_index < hdrrmac4_var_ptr->num_open_state_chans; 
       chan_index++)
  {
    uint8 chan = hdrrmac4_open_rl_list[chan_index];

    chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];

    pkt_id    = chan_ptr->curr_tx_pkt_id;

    /* 1. Calculate  t2p_outflows for each active MAC flow */
    if( pkt_id != HDRRMAC4DRV_IDLE_PACKET ) /* Packet will be transmitted */
    {
      if( hdrrmac4_hist[chan][pkt_id].sum_payload == 0 )
      {
        for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
        {
          active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
          chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr = 0;
        }

        HDR_MSG_PROT_2( MSG_LEGACY_LOW, 
                        "SumPayload 0! chan: %d, pid: %d", 
                        chan,
                        pkt_id );
      }
      else
      {
        if( hdrrmac4_hist[chan][pkt_id].pkt_size_index >= 
            ((hdrrmaccmn_payload_size_enum_type) 
            hdrrmac4_config_ptr->aux_pilot_info.min_payload) )
        {
          for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
          {
            /* Adjust for Aux Pilot gain, rounding the result */
            active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
            chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr = 
                (uint32) ( ( ( (uint64) ( (int64) hdrrmaccmn_q_10_scaling_factor 
                               + hdrrmac4_config_ptr->aux_pilot_info.gain ) 
                             * hdrrmac4_hist[chan][pkt_id].flows[active_flow_id].size 
                               * chan_ptr->tx_t2p )
                               / hdrrmac4_hist[chan][pkt_id].sum_payload  
                               + ((uint16) 1 << 9) ) >> 10 );
          }
        }
        else
        {
          for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
          {
            /* No Aux Pilot */
            active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
            chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr = 
                (uint32) ( ( ( (uint64) chan_ptr->tx_t2p << 1) 
                         * hdrrmac4_hist[chan][pkt_id].flows[active_flow_id].size )
                           / hdrrmac4_hist[chan][pkt_id].sum_payload );

            /* Round */
            chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr = 
              ( chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr + 1 ) >> 1;
          }
        }
      }
    } /* !IDLE_PACKET */
    else
    {
      for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
      {
        active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
        chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr = 0;
      }
    }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* 2. Update bucket_level for each active MAC flow */
    if( chan_ptr->hdr_mode_active == FALSE )
    {
      for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
      {
        active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
        /* freeze bucket level before tuneaway */
        if ( hdrrmac4_var_ptr->graceful_ta_started != TRUE )     
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */
        {
          chan_ptr->flow[active_flow_id].bucket.bucket_level.curr = 0;
        }
      } /* for */
    }
    else
    {
      for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
      {
        active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

        /* bucket level ~= previous bucket level 
         *                 + inflow from previous subframe
         *                 - outflow from current subframe
         */
        chan_ptr->flow[active_flow_id].bucket.bucket_level.curr = 
          (int32) MIN( (int32) 0x7FFFFFFF,\
          ((int64) chan_ptr->flow[active_flow_id].bucket.bucket_level.prev)
          + chan_ptr->flow[active_flow_id].bucket.t2p_inflow.prev
          - chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr );

        /* Take min( bucket level, bucket_level_sat ) */ 
        if( chan_ptr->flow[active_flow_id].bucket.bucket_level.curr > 0 )
        {
          if( (uint32) chan_ptr->flow[active_flow_id].bucket.bucket_level.curr 
              > chan_ptr->flow[active_flow_id].bucket.bucket_level_sat )
          {
            chan_ptr->flow[active_flow_id].bucket.bucket_level.curr = 
              chan_ptr->flow[active_flow_id].bucket.bucket_level_sat;
          }
        }
      } /* for each active flow */
    }
  }

  return;

} /* hdrrmac4_update_bucket_level */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_UPDATE_DATA_TOKEN_BUCKET_LEVEL                      INTERNAL

DESCRIPTION
  This function updates the data token bucket level for each MAC flow by 
  adding the net difference between the DataTokenInflow from previous 
  subframe and the SumTransmittedData from current subframe to the bucket.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
RMAC4_LOCAL void hdrrmac4_update_data_token_bucket_level( void )
{
  uint8 flow;
  uint8 active_flow_id;
  uint8 chan_index;
  uint16 pkt_id;
  hdrrmac4_phy_chan_vars_struct_type  *chan_ptr;
  hdrrmac4_flow_config_struct_type    *flow_config_ptr;
  hdrrmac4_flow_vars_struct_type      *flow_ptr;
  /* convenience pointers */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Zero out the SumTransmittedData for this subframe */
  for (flow = 0; flow < hdrrmac4_num_active_flows; flow++)
  {
    active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
    hdrrmac4_var_ptr->data_flow[active_flow_id].data_token.
      sum_transmitted_data = 0;
  }

  /* Calculate the SumTransmittedData for each active MAC flow */
  for (chan_index = 0; 
       chan_index < hdrrmac4_var_ptr->num_open_state_chans; 
       chan_index++)
  {
    uint8 chan = hdrrmac4_open_rl_list[chan_index];

    chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];
    pkt_id    = chan_ptr->curr_tx_pkt_id;

    if (pkt_id != HDRRMAC4DRV_IDLE_PACKET)
    {
      for (flow = 0; flow < hdrrmac4_num_active_flows; flow++)
      {
        active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
        hdrrmac4_var_ptr->data_flow[active_flow_id].data_token.
          sum_transmitted_data +=
            hdrrmac4_hist[chan][pkt_id].flows[active_flow_id].size;
      }
    }
  } /* for each channel */

  /* Calculate
   * DataBucketLevel[i][n+1] =
   *  min(
   *    (DataTokenInflow[i] + DataBucketLevel[i][n] - SumTransmittedData[i][n]),
   *    DataBucketLevelMax[i])
   */
  for (flow = 0; flow < hdrrmac4_num_active_flows; flow++)
  {
    active_flow_id  = hdrrmac4_active_flows_set[flow].mac_flow_id;
    flow_config_ptr = &hdrrmac4_config_ptr->flow[active_flow_id];
    flow_ptr        = &hdrrmac4_var_ptr->data_flow[active_flow_id];

    if (flow_config_ptr->data_token_bucket.data_token_inflow ==
        HDRRMAC4_DATA_TOKEN_INFLOW_INFINITE)
    {
      flow_ptr->data_token.data_bucket_level.curr = 
        HDRRMAC4_DATA_BUCKET_LEVEL_MAX_INFINITE;
    }
    else
    {
      flow_ptr->data_token.data_bucket_level.curr =
          flow_config_ptr->data_token_bucket.data_token_inflow +
          flow_ptr->data_token.data_bucket_level.prev -
          flow_ptr->data_token.sum_transmitted_data;

      flow_ptr->data_token.data_bucket_level.curr = (int32)
        MIN(((int64)(flow_ptr->data_token.data_bucket_level.curr)),
        ((int64)(flow_config_ptr->data_token_bucket.data_bucket_level_max)));
    }
  }

  return;

} /* hdrrmac4_update_data_token_bucket_level */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_BUCKET_FACTOR_SCALING_FUNC                          INTERNAL

DESCRIPTION
  This function interacts with PCP and builds a new packet

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL int32 hdrrmac4_bucket_factor_scaling_func( void *bucket_factor )
{
  uint8 temp_bf;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_bf = *( (uint8 *) bucket_factor ); /* BucketFactor grids are 
                                           * unsigned 8-bit numbers
                                           */

  return( (int32) ( ( ( temp_bf + 8 ) * hdrrmaccmn_q_20_scaling_factor ) / 8 ) );
} /* hdrrmac4_bucket_factor_scaling_func */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_CALC_BUCKET_FACTOR                                  INTERNAL

DESCRIPTION
  This function calculates the BucketFactor() by interpolating on the T2P, FRAB
  grid

DEPENDENCIES
  None.

PARAMETERS
  chan_ptr  - Points to the physical channel structure whose flow saturation
              levels are to be updated.
  flow      - Flow number.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL int32 hdrrmac4_calc_bucket_factor
(
  hdrrmac4_phy_chan_vars_struct_type *chan_ptr,
  uint8 flow
)
{
  int32 bucket_factor;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* For the given flow prepare BucketFactor interpolation axes and grid by 
   * scaling the values to the proper precision before passing to 
   * bi-linear interpolation math routine
   */
  bucket_factor = hdrrmacmath_interpolate_2d(
    chan_ptr->flow[flow].bucket.t2p_inflow_db,
    chan_ptr->sector_info.frab,
    &hdrrmac4_t2p_frab_grid_data.bucket_factor[flow].t2p_axis,
    &hdrrmac4_t2p_frab_grid_data.bucket_factor[flow].frab_axis,
    hdrrmac4_t2p_frab_grid_data.bucket_factor[flow].t2p_frab_grid );

  return( bucket_factor );

} /* hdrrmac4_calc_bucket_factor */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_UPDATE_BUCKET_LEVEL_SAT                            INTERNAL

DESCRIPTION
  This function updates the saturation level of each active flow's bucket in
  the specified channel.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_update_bucket_level_sat(void)
{
  uint8 flow;
  uint8 active_flow_id;
  uint32 bl_sat;
  uint8 chan_index;
  hdrrmac4_phy_chan_vars_struct_type *chan_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (chan_index = 0; 
       chan_index < hdrrmac4_var_ptr->num_open_state_chans; 
       chan_index++)
  {
    uint8 chan = hdrrmac4_open_rl_list[chan_index];

    chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];

    for( flow=0; flow < hdrrmac4_num_active_flows; flow++ )
    {
      active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

      chan_ptr->flow[active_flow_id].bucket.t2p_inflow_db = 
        hdrrmacmath_lin_2_db( chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr );

      chan_ptr->flow[active_flow_id].bucket.bucket_factor =
        hdrrmac4_calc_bucket_factor( chan_ptr, active_flow_id );

      /* Round BucketLevelSat */
      bl_sat = (uint32) MIN( (uint32) 0xFFFFFFFF, ( (uint64) ( ( (int64)
        hdrrmac4_config_ptr->flow[active_flow_id].burst_duration_factor
        * chan_ptr->flow[active_flow_id].bucket.bucket_factor )
        * chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr
                              + ((uint32) 1 << 19) ) ) >> 20 );
      /* burst_duration_factor = 32 (max)        - Q0  format
         bucket_factor         = 33 * 2^20 (max) - Q20 format
         t2p_inflow            = 2^32 - 1 (max)               */

      chan_ptr->flow[active_flow_id].bucket.bucket_level_sat = 
        hdrrmacmath_min_u32( bl_sat, 
                             hdrrmac4_config_ptr->flow[active_flow_id].bucket_level_max );
    }
  }

  return;

} /* hdrrmac4_update_bucket_level_sat */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_T2P_UP_DOWN_SCALING_FUNC                            INTERNAL

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL int32 hdrrmac4_t2p_up_down_scaling_func( void *t2p_delta )
{
  int8 temp_t2p_delta;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  temp_t2p_delta = *( (int8 *) t2p_delta ); /* T2PUp & T2PDown grids are signed
                                             * 8-bit numbers
                                             */

   /* Convert from 8-bit 2's complement values in 0.25 dB units to 
    * signed 32-bit values in dB units
    */ 
  return( (int32) ( ( temp_t2p_delta * hdrrmaccmn_q_20_scaling_factor ) / 4 ) );
} /* hdrrmac4_t2p_up_down_scaling_func */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_CALC_T2P_DOWN                                       INTERNAL

DESCRIPTION
  This function calculates the T2PDn() by interpolating on the T2P, FRAB
  grid

DEPENDENCIES
  None.

PARAMETERS
  log_chan        - Reverse Logical Channel number.
  flow            - Flow number.
  t2p_axis_input  - T2P axis input value
  frab_axis_input - FRAB axis input value

RETURN VALUE
  Calculated T2PDn value.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL int32 hdrrmac4_calc_t2p_down
( 
  uint8 log_chan,
  uint8 flow,
  int32 t2p_axis_input,
  int32 frab_axis_input
)
{
  int32 t2p_down, phy_chan_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  phy_chan_idx = hdrrmac4_from_log_to_phy(log_chan);

  ASSERT(phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS);

  /* For the given flow prepare T2PDown interpolation axes and grid by 
   * scaling the values to the proper precision before passing to 
   * bi-linear interpolation math routine
   */
  t2p_down = 
    hdrrmacmath_interpolate_2d(
    t2p_axis_input,
    frab_axis_input,
    &hdrrmac4_t2p_frab_grid_data.t2p_down[flow][phy_chan_idx].t2p_axis,
    &hdrrmac4_t2p_frab_grid_data.t2p_down[flow][phy_chan_idx].frab_axis,
    hdrrmac4_t2p_frab_grid_data.t2p_down[flow][phy_chan_idx].t2p_frab_grid );

  return( t2p_down );

} /* hdrrmac4_calc_t2p_down  */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_CALC_T2P_UP                                         INTERNAL

DESCRIPTION
  This function calculates the T2PUp() by interpolating on the T2P, FRAB
  grid

DEPENDENCIES
  None.

PARAMETERS
  log_chan        - Reverse Logical Channel number.
  flow            - Flow number.
  t2p_axis_input  - T2P axis input value
  frab_axis_input - FRAB axis input value

RETURN VALUE
  Calculated T2PUp value.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL int32 hdrrmac4_calc_t2p_up
( 
  uint8 log_chan,
  uint8 flow,
  int32 t2p_axis_input,
  int32 frab_axis_input
)
{
  int32 t2p_up, phy_chan_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  phy_chan_idx = hdrrmac4_from_log_to_phy(log_chan);

  ASSERT(phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS);

  /* For the given flow prepare T2PUp interpolation axes and grid by 
   * scaling the values to the proper precision before passing to 
   * bi-linear interpolation math routine
   */
  t2p_up = 
    hdrrmacmath_interpolate_2d(
    t2p_axis_input,
    frab_axis_input,
    &hdrrmac4_t2p_frab_grid_data.t2p_up[flow][phy_chan_idx].t2p_axis,
    &hdrrmac4_t2p_frab_grid_data.t2p_up[flow][phy_chan_idx].frab_axis,
    hdrrmac4_t2p_frab_grid_data.t2p_up[flow][phy_chan_idx].t2p_frab_grid );

  return( t2p_up );

} /* hdrrmac4_calc_t2p_up  */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_UPDATE_BUCKET_INFLOW                                INTERNAL

DESCRIPTION
  This function calculates the incremental inflow for the buckets, and hence 
  the updated bucket T2PInflows. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
RMAC4_LOCAL void hdrrmac4_update_bucket_inflow( void )
{
  uint8 flow;
  uint8 active_flow_id;
  uint8 chan_index;
  uint8 log_chan;
  hdrrmac4_phy_chan_vars_struct_type *chan_ptr;
    /* convenience pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (chan_index = 0; 
       chan_index < hdrrmac4_var_ptr->num_open_state_chans;
       chan_index++)
  {
    uint8 chan = hdrrmac4_open_rl_list[chan_index];

    chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];
    log_chan = hdrrmac4_from_phy_to_log(chan);
    ASSERT(log_chan < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS);

    /* convert T2PInflow of previous subframe into dB for interpolation */
    for( flow=0; flow < hdrrmac4_num_active_flows; flow++ )
    {
      active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
      chan_ptr->flow[active_flow_id].bucket.t2p_inflow_db = 
        hdrrmacmath_lin_2_db( chan_ptr->flow[active_flow_id].bucket.t2p_inflow.prev );
    }

    /* Compute the T2P increment or decrement (delta_t2p_inflow) for each 
     * MAC flow */
    if( chan_ptr->hdr_mode_active == FALSE )
    {
      for( flow=0; flow < hdrrmac4_num_active_flows; flow++ )
      {
        active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
        chan_ptr->flow[active_flow_id].bucket.delta_t2p_inflow = 0;
      }
    }
    else
    {
      for( flow=0; flow < hdrrmac4_num_active_flows; flow++ )
      {
        int32 delta_t2p_inflow_db = 0;
        active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

        if( chan_ptr->flow[active_flow_id].bucket.tt2p_hold.prev > 0 )
        {
          chan_ptr->flow[active_flow_id].bucket.delta_t2p_inflow = 0;
        }
        else
        {
          int32 t2p_axis_input;

          /* Offset T2PInflow by PilotStrength(PS), which is calculated and 
           * passed up by the lower layer
           */
          t2p_axis_input = chan_ptr->flow[active_flow_id].bucket.t2p_inflow_db 
                            + chan_ptr->sector_info.pilot_strength;

          if( HDRRMAC4_FLOW_QRAB_LOADED( chan, active_flow_id ) == TRUE )       /* Loaded */
          {
            delta_t2p_inflow_db = hdrrmac4_calc_t2p_down( 
                                    log_chan,
                                    active_flow_id, 
                                    t2p_axis_input,
                                    chan_ptr->sector_info.frab );

            /* Covert result back to linear */
            chan_ptr->flow[active_flow_id].bucket.delta_t2p_inflow = 
              -( (int32) hdrrmacmath_db_2_lin( delta_t2p_inflow_db ) );
          }
          else /* Unloaded */
          {
            delta_t2p_inflow_db = hdrrmac4_calc_t2p_up( 
                                    log_chan,
                                    active_flow_id, 
                                    t2p_axis_input,
                                    chan_ptr->sector_info.frab );

            /* Covert result back to linear */
            chan_ptr->flow[active_flow_id].bucket.delta_t2p_inflow =
              hdrrmacmath_db_2_lin( delta_t2p_inflow_db );

            /* JIC bucket level for this flow >= saturation point set the
             * T2PInflow increment to 0
             */
            if( chan_ptr->flow[active_flow_id].bucket.bucket_level.curr > 0 )
            {
              if( chan_ptr->flow[active_flow_id].bucket.bucket_level.curr >=
                (int32) chan_ptr->flow[active_flow_id].bucket.
                bucket_level_sat )
              {
                chan_ptr->flow[active_flow_id].bucket.delta_t2p_inflow = 0;
              }
            }
          }
        }
      } /* for */
    }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* Compute T2PInflow for each MAC flow */
    for( flow=0; flow < hdrrmac4_num_active_flows; flow++ )
    {
      active_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;
      if( chan_ptr->flow[active_flow_id].bucket.tt2p_hold.prev > 0 )
      {
        chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr = 
          chan_ptr->flow[active_flow_id].bucket.t2p_inflow.prev;

        chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr = 
          chan_ptr->flow[active_flow_id].bucket.tt2p_hold.prev - 1;
      }
      else
      {
        uint64 inflow, outflow;

        /* If tt2p_hold.prev is set to 0 [say by init_bucket()],
         * tt2p_hold.curr should be made 0 before save_bucket_state()
         * to prevent the updated previous value from getting overwritten
         * by older tt2p_hold.curr value 
         */
        chan_ptr->flow[active_flow_id].bucket.tt2p_hold.curr = 0;

        /* If tuned away from HDR or graceful tuneaway started */
        if( ( hdrrmac4_var_ptr->data_allowed == FALSE )    
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
          || ( hdrrmac4_var_ptr->graceful_ta_started == TRUE )     
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */
        )
        {
          inflow = ( HDRRMACCMN_T2P_NO_TX_FILT_SCALING_FACTOR
                     - ( HDRRMACCMN_T2P_NO_TX_FILT_SCALING_FACTOR 
                         / hdrrmac4_config_ptr->filt_time_const.t2p_no_tx ) )  
                     * (uint64) chan_ptr->flow[active_flow_id].
                                  bucket.t2p_inflow.prev;

          /* Round T2PInflow */
          chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr = 
            (uint32) ( (inflow + (HDRRMACCMN_T2P_NO_TX_FILT_SCALING_FACTOR >> 1
            )) / HDRRMACCMN_T2P_NO_TX_FILT_SCALING_FACTOR );
        }
        else
        {
          inflow  = ( HDRRMACCMN_T2P_FILT_SCALING_FACTOR
                      - ( HDRRMACCMN_T2P_FILT_SCALING_FACTOR 
                          / hdrrmac4_config_ptr->flow[active_flow_id].t2p_filt_tc ) )  
                      * (uint64) chan_ptr->flow[active_flow_id].bucket.t2p_inflow.prev;

          outflow = (HDRRMACCMN_T2P_FILT_SCALING_FACTOR 
                     * (uint64) chan_ptr->flow[active_flow_id].bucket.t2p_outflow.curr)
                     / hdrrmac4_config_ptr->flow[active_flow_id].t2p_filt_tc;

          /* Round T2PInflow */
          chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr = 
            (uint32) MIN( (uint32) 0xFFFFFFFF, 
            (inflow + outflow + (HDRRMACCMN_T2P_FILT_SCALING_FACTOR >> 1
            )) / HDRRMACCMN_T2P_FILT_SCALING_FACTOR 
            + chan_ptr->flow[active_flow_id].bucket.delta_t2p_inflow );

        }

        /* Ensure T2PInflow is within T2PInflow min-max */
        if( chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr
            > hdrrmac4_config_ptr->nc[active_flow_id][log_chan].
                t2p_inflow_range_ptr->max )
        {
          chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr = 
            hdrrmac4_config_ptr->nc[active_flow_id][log_chan].
              t2p_inflow_range_ptr->max;
        }
        else if(  chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr
                  < hdrrmac4_config_ptr->nc[active_flow_id][log_chan].
                      t2p_inflow_range_ptr->min ) 
        {
          chan_ptr->flow[active_flow_id].bucket.t2p_inflow.curr = 
            hdrrmac4_config_ptr->nc[active_flow_id][log_chan].
              t2p_inflow_range_ptr->min;
        }
      }
    } /* for */
  } /* for each physical channel */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update bucket saturation level for each MAC flow */
  hdrrmac4_update_bucket_level_sat();
    
  return;

} /* hdrrmac4_update_bucket_inflow */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_CALC_POT_DATA_BUCKET_OUTFLOW                        INTERNAL

DESCRIPTION
  This function calculates the PotentialDataBucketOutflow for each active MAC
  flow.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
RMAC4_LOCAL void hdrrmac4_calc_pot_data_bucket_outflow( void )
{
  uint8 flow;
  uint8 active_flow_id;
  hdrrmac4_flow_config_struct_type    *flow_config_ptr;
  hdrrmac4_flow_vars_struct_type      *flow_ptr;
  /* convenience pointers */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (flow = 0; flow < hdrrmac4_num_active_flows; flow++)
  {
    active_flow_id  = hdrrmac4_active_flows_set[flow].mac_flow_id;
    flow_config_ptr = &hdrrmac4_config_ptr->flow[active_flow_id];
    flow_ptr        = &hdrrmac4_var_ptr->data_flow[active_flow_id];

    if ((flow_config_ptr->data_token_bucket.data_token_inflow ==
         HDRRMAC4_DATA_TOKEN_INFLOW_INFINITE) ||
        (flow_ptr->data_token.data_bucket_level.curr ==
         HDRRMAC4_DATA_BUCKET_LEVEL_MAX_INFINITE))
    {
      flow_ptr->data_token.pot_data_bucket_outflow =
        HDRRMAC4_DATA_BUCKET_LEVEL_MAX_INFINITE;
    }
    else
    {
      flow_ptr->data_token.pot_data_bucket_outflow =
        flow_config_ptr->data_token_bucket.data_token_inflow +
        flow_ptr->data_token.data_bucket_level.curr;
    }

    if (flow_ptr->data_token.pot_data_bucket_outflow < 0)
    {
      flow_ptr->data_token.pot_data_bucket_outflow = 0;
    }
  }

  return;

} /* hdrrmac4_calc_pot_data_bucket_outflow */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_BUILD_NON_EMPTY_FLOWS_SET                            INTERNAL

DESCRIPTION
  This function constructs the set, F, of active MAC flows with non-empty queues

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index that the non-empty flow set is built
                 for.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
RMAC4_LOCAL void hdrrmac4_build_non_empty_flows_set
(
  uint8 phy_chan_idx
)
{
  uint8 flow;
  uint8 non_empty_flow_cnt = 0;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for( flow = 0; flow < hdrrmac4_num_active_flows; flow++)
  {
    if( hdrrmac4_active_flows_set_tmp[flow].size > 0 )
    {
      if ((hdrrmac4_active_flows_set_tmp[flow].mac_flow_id != 0) ||
          ((hdrrmac4_active_flows_set_tmp[flow].mac_flow_id == 0) &&
           (phy_chan_idx == hdrrmac4_var_ptr->slp_carrier_idx)))
          /* SLP flow only goes on SLP Carrier */
      {
        /* add this flow to set of non-empty flows */
        hdrrmac4_non_empty_flows_set[phy_chan_idx][non_empty_flow_cnt].mac_flow_id =
          hdrrmac4_active_flows_set_tmp[flow].mac_flow_id;
  
        hdrrmac4_non_empty_flows_set[phy_chan_idx][non_empty_flow_cnt].size =
          hdrrmac4_active_flows_set_tmp[flow].size;
        
        non_empty_flow_cnt++;
      }
    }
  }
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  if (non_empty_flow_cnt == 0)
  {
     /* If no non-Empty flows mark all flows as non-empty for the purpose
	  of RL rate estimate calculation */
     for (flow = 0; flow < hdrrmac4_num_active_flows; flow++) 
     {
         if ((hdrrmac4_active_flows_set_tmp[flow].mac_flow_id != 0) ||
             ((hdrrmac4_active_flows_set_tmp[flow].mac_flow_id == 0) &&
              (phy_chan_idx == hdrrmac4_var_ptr->slp_carrier_idx)))
             /* SLP flow only goes on SLP Carrier */
         {
           /* add this flow to set of non-empty flows */
           hdrrmac4_non_empty_flows_set[phy_chan_idx][non_empty_flow_cnt].mac_flow_id =
             hdrrmac4_active_flows_set_tmp[flow].mac_flow_id;
     
           hdrrmac4_non_empty_flows_set[phy_chan_idx][non_empty_flow_cnt].size =
             hdrrmac4_active_flows_set_tmp[flow].size;
           
           non_empty_flow_cnt++;
         }
     }
  }
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

  hdrrmac4_num_non_empty_flows[phy_chan_idx] = non_empty_flow_cnt;

  return;

} /* hdrrmac4_build_non_empty_flows_set */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_CALC_Q_OUTFLOWS                                     INTERNAL

DESCRIPTION
  This function calculates the queue outflows for each flow e F and the
  aggregate queue outflow by summing up the queue outflows of F.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
RMAC4_LOCAL void hdrrmac4_calc_q_outflows
(
  uint8 phy_chan_idx
)
{
  uint8                             flow;
  hdrrmaccmn_tx_mode_enum_type      tm;
  uint32                            sum_q_outflow = 0;
  hdrrmac4_per_nc_vars_struct_type  *flow_ptr;
  hdrrmac4_flow_config_struct_type  *flow_cfg_ptr;
  int16                             rand_num = hdrrmaccmn_get_rand();

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* save the random number for logging */
  hdrrmac4_logging.rand_num[phy_chan_idx] = rand_num;

  /* Clear QOutflow & Potential T2P outflow for all flows. This is to
   * avoid having non-zero values for empty flows.
   */
  for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
  {
    flow_ptr = 
      &( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
          flow[hdrrmac4_active_flows_set[flow].mac_flow_id] );

    flow_ptr->queue_info.q_outflow[HDRRMACCMN_TX_MODE_HI_CAP] = 0; 
    flow_ptr->queue_info.q_outflow[HDRRMACCMN_TX_MODE_LO_LAT] = 0; 
    flow_ptr->bucket.pot_t2p_outflow[HDRRMACCMN_TX_MODE_HI_CAP] = 0; 
    flow_ptr->bucket.pot_t2p_outflow[HDRRMACCMN_TX_MODE_LO_LAT] = 0; 
  }

  for( flow = 0; flow < hdrrmac4_num_non_empty_flows[phy_chan_idx]; flow++ )
  {
    uint8 flow_id;

    flow_id = hdrrmac4_non_empty_flows_set[phy_chan_idx][flow].mac_flow_id;

    flow_ptr = 
      &( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].flow[flow_id] );

    for( tm = HDRRMACCMN_TX_MODE_HI_CAP; tm < HDRRMACCMN_NUM_TX_MODES; tm++ )
    {
      int32 t2p_dither_term;
      uint8 stagger = hdrrmac4_config_ptr->common_pow_params.alloc_stagger;
      uint32 pot_outflow_octets;

      if (hdrrmac4_var_ptr->data_flow[flow_id].data_token.
            pot_data_bucket_outflow == 0)
      {
        /* If PotentialDataBucketOutflow is 0, 
         * set PotentialT2POutflow[tm] to 0
         */
        flow_ptr->bucket.pot_t2p_outflow[tm] = 0;
      }
      else
      {
        /* The t2p_dither_term is an intermediate result used to determine
         * the potential t2p outflow. Note that the formula is different for
         * LoLat and HiCap modes
         */
        if( tm == HDRRMACCMN_TX_MODE_HI_CAP )
        {
          /* Round t2p_dither_term */
          t2p_dither_term = 
            (int32) ((int64) (( ((uint32) (0x1000 + stagger * (int32) rand_num))
                              * ((int64) flow_ptr->bucket.bucket_level.curr 
                              + ((int64) flow_ptr->bucket.t2p_inflow.curr << 2) ) 
                              + ((uint32) 1 << 13) ) >> 14 ));
        }
        else
        {
          /* Round t2p_dither_term */
          t2p_dither_term = 
            (int32) ((int64) (( ((uint32) (0x1000 + stagger * (int32) rand_num))
                              * ((int64) flow_ptr->bucket.bucket_level.curr 
                              + ((int64) flow_ptr->bucket.t2p_inflow.curr << 1) ) 
                              + ((uint32) 1 << 12) ) >> 13 ));
        }

        /* Calculate potential queue outflows */
        if( t2p_dither_term < 0 )
        {
          flow_ptr->bucket.pot_t2p_outflow[tm] = 0;
        }
        else
        {
          uint32 max_outflow;

          /* Round max_outflow */
          max_outflow = (uint32) MIN( (uint32) 0xFFFFFFFF,
            ( (uint64) ( ( (int64) flow_ptr->bucket.bucket_factor )
            * flow_ptr->bucket.t2p_inflow.curr
            + ((uint32) 1 << 19) ) ) >> 20 );

          flow_ptr->bucket.pot_t2p_outflow[tm] = 
            hdrrmacmath_min_u32( (uint32) t2p_dither_term, max_outflow );

        }
      }

      /* Calculate actual queue outflows */
      pot_outflow_octets = 
        (uint32) ( ( (uint64) flow_ptr->bucket.pot_t2p_outflow[tm]
                 * (uint32) hdrrmac4_config_ptr->t2p_info[tm].t2p_conv_factor ) 
                 >> 16 );

      /* QOutflow = min( Q_i, PotentialT2POutflow * T2PConvFactor ) */
      flow_ptr->queue_info.q_outflow[tm] = 
        hdrrmacmath_min_u32( flow_ptr->queue_info.initial_size, 
                              pot_outflow_octets );
    } /* for tm */

    flow_cfg_ptr = &(hdrrmac4_config_ptr->flow[flow_id]);
  
    if( flow_cfg_ptr->tx_mode == HDRRMACCMN_TX_MODE_HI_CAP )
    {
      /* Calculate aggregate queue outflow = sum of all HiCap q_outflows */
      sum_q_outflow += flow_ptr->queue_info.q_outflow[HDRRMACCMN_TX_MODE_HI_CAP];
    }

  } /* for flow */

  /* store sum_q_outflow */
  hdrrmac4_var_ptr->phy_chan[phy_chan_idx].sum_q_outflow = sum_q_outflow;

  return;

} /* hdrrmac4_calc_q_outflows */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_DETERMINE_PACKET_TX_MODE                          INTERNAL

DESCRIPTION
  This function decides if the packet transmission mode is HiCap mode or LoLat
  mode.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_determine_packet_tx_mode
(
  uint8 phy_chan_idx
) 
{
  uint8                         flow;
  boolean                       done = FALSE;
  hdrrmaccmn_tx_mode_enum_type  tm   = HDRRMACCMN_TX_MODE_HI_CAP;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for( flow = 0; 
       !done && (flow < hdrrmac4_num_non_empty_flows[phy_chan_idx]); 
       flow++ )
  {
    hdrrmac4_flow_config_struct_type *flow_ptr;

    flow_ptr = 
      &(hdrrmac4_config_ptr->
          flow[(hdrrmac4_non_empty_flows_set[phy_chan_idx][flow].mac_flow_id)]);

    if( flow_ptr->tx_mode == HDRRMACCMN_TX_MODE_LO_LAT )
    {
      tm = HDRRMACCMN_TX_MODE_LO_LAT;
      done = TRUE; 
    }
  }

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  if ( hdrrmac4_var_ptr->t2p_boost.state == HDRRMACCMN_T2P_BOOST_PRE_TA_ACTIVE )
  {
    /* Only LoLat packets are allowed to be transmitted at this point.
       Force packet tx mode to LoLat. */
    tm = HDRRMACCMN_TX_MODE_LO_LAT;
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

  hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode = tm;

  return;

} /* hdrrmac4_determine_packet_tx_mode */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_BUILD_ELIGIBLE_FLOWS_SET                            INTERNAL

DESCRIPTION
  This function constructs the set, F*, subset of F (active MAC flows with 
  non-empty queues) that are eligible for transmission in the next packet

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
RMAC4_LOCAL void hdrrmac4_build_eligible_flows_set
(
  uint8 phy_chan_idx
)
{
  uint8   flow;
  boolean tm_hc, frab_below_thresh, sum_q_outflow_above_thresh;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* Transmission Mode is HiCap */
  tm_hc = ( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode == 
            HDRRMACCMN_TX_MODE_HI_CAP );

  /* FRAB is below AT-wide FRAB threshold for sector loading based flow
   * eligibility 
   */
  frab_below_thresh = 
    ( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].sector_info.frab 
      < hdrrmac4_config_ptr->flow_elig_thresh.frab_low );

  /* Sum of all HiCap queue outflows is above AT-wide merge threshold */
  sum_q_outflow_above_thresh = 
    ( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].sum_q_outflow 
      >= hdrrmac4_config_ptr->flow_elig_thresh.merge_thresh );

  if( tm_hc || frab_below_thresh || sum_q_outflow_above_thresh )
  {
    /* F = F* */
    for( flow = 0; flow < hdrrmac4_num_non_empty_flows[phy_chan_idx]; flow++)
    {
      /* add this flow to set of eligible flows */
      hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id =
        hdrrmac4_non_empty_flows_set[phy_chan_idx][flow].mac_flow_id;

      hdrrmac4_elig_flows_set[phy_chan_idx][flow].size =
        hdrrmac4_non_empty_flows_set[phy_chan_idx][flow].size;
    }

    hdrrmac4_num_elig_flows[phy_chan_idx] = 
      hdrrmac4_num_non_empty_flows[phy_chan_idx];
  }
  else
  {
    boolean tm_ll, q_outflow_above_flow_merge_thresh;
    uint8 elig_flow_cnt = 0;
      
    /* F is a proper subset of F* */
    for( flow = 0; flow < hdrrmac4_num_non_empty_flows[phy_chan_idx]; flow++ )
    {
      hdrrmac4_per_nc_vars_struct_type *flow_var_ptr;
      hdrrmac4_flow_config_struct_type *flow_cfg_ptr;
  
      flow_var_ptr = 
        &(hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
            flow[hdrrmac4_non_empty_flows_set[phy_chan_idx][flow].mac_flow_id]);
  
      flow_cfg_ptr = 
        &(hdrrmac4_config_ptr->
            flow[hdrrmac4_non_empty_flows_set[phy_chan_idx][flow].mac_flow_id]);
  
      /* Transmission Mode is LoLat */
      tm_ll = ( (flow_cfg_ptr->tx_mode) == HDRRMACCMN_TX_MODE_LO_LAT );

      /* q_outflow for this flow is above its merge threshold */
      q_outflow_above_flow_merge_thresh = 
        ( flow_var_ptr->queue_info.q_outflow[HDRRMACCMN_TX_MODE_HI_CAP]
          >= flow_cfg_ptr->merge_thresh );

      if( tm_ll || q_outflow_above_flow_merge_thresh )
      {
        /* add this flow to set of eligible flows */
        hdrrmac4_elig_flows_set[phy_chan_idx][elig_flow_cnt].mac_flow_id =
          hdrrmac4_non_empty_flows_set[phy_chan_idx][flow].mac_flow_id;

        hdrrmac4_elig_flows_set[phy_chan_idx][elig_flow_cnt].size =
          hdrrmac4_non_empty_flows_set[phy_chan_idx][flow].size;

        elig_flow_cnt++;
      }
    } /* for */

    hdrrmac4_num_elig_flows[phy_chan_idx] = elig_flow_cnt;
  }

  return;

} /* hdrrmac4_build_eligible_flows_set */


 /* EJECT */
/*=============================================================================
FUNCTION HDRSCMFMAC4_SUM_POT_T2P_OUTFLOWS                                 INTERNAL

DESCRIPTION
  This function calculates (for each transmission mode) the aggregate 
  PotentialT2POutflow by summing all the potential T2P outflows of eligible 
  MAC flows

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_sum_pot_t2p_outflows
(
  uint8 phy_chan_idx
)
{
  uint8  flow, tm;
  uint32 sum_pot_t2p_outflow[HDRRMACCMN_NUM_TX_MODES] = {0, 0};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
  {
    hdrrmac4_per_nc_vars_struct_type *flow_ptr;

    flow_ptr = 
      &( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
          flow[hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id] );

    for( tm = (uint8) HDRRMACCMN_TX_MODE_HI_CAP; 
         tm < (uint8) HDRRMACCMN_NUM_TX_MODES; tm++ )
    {
      sum_pot_t2p_outflow[tm] += flow_ptr->bucket.pot_t2p_outflow[tm];
    } /* for tm */
  } /* for flow */

  for( tm = (uint8) HDRRMACCMN_TX_MODE_HI_CAP; 
       tm < (uint8) HDRRMACCMN_NUM_TX_MODES; tm++ )
  {
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].sum_pot_t2p_outflow[tm] = 
      sum_pot_t2p_outflow[tm]; 
  }

  return;

} /* hdrrmac4_sum_pot_t2p_outflows */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_DIFF_PS_SETS                                         INTERNAL

DESCRIPTION
  This function calculates the set difference between two sets of packet
  sizes.

DEPENDENCIES
  None.

PARAMETERS
  The 2 sets to diff, say, A & B.

RETURN VALUE
  A - B

  NOTE: A representation of the resulting subset is returned

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrrmaccmn_ps_set_type hdrrmac4_diff_ps_sets
(
  hdrrmaccmn_ps_set_type ps_set_a,
  hdrrmaccmn_ps_set_type ps_set_b
)
{
  return( (ps_set_a) & (~ps_set_b) );
} /* hdrrmac4_diff_ps_sets */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_CALC_CANDIDATE_PS_SUBSET                             INTERNAL

DESCRIPTION
  This function calculates the set of valid Physical Layer packet sizes subject
  to PermittedPayload, PA Headroom and other constraints.

DEPENDENCIES
  None.

PARAMETERS
  The packet transmission mode, initial PS set, max permitted pkt size and 
  max T2P (in dB)

RETURN VALUE
  A subset of the set of all possible packet sizes, Psi = {128, 256,...,12288}, 
  satisfying the constraints mentioned above. The actual packet size selected 
  will be an element of ths subset.

  NOTE: A representation of the resulting subset is returned

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrrmaccmn_ps_set_type hdrrmac4_calc_candidate_ps_subset
(
  uint8                             phy_chan_idx,
  hdrrmaccmn_tx_mode_enum_type      tm,
  hdrrmaccmn_ps_set_type            ps_set,
  hdrrmaccmn_payload_size_enum_type pp_max_ps,
  uint32                            max_tx_t2p
)
{
  uint8 ps = 0;
  uint32 max_db;        /* Max T2P in dB */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  boolean last_boosted = FALSE;   
    /* If current RRI exceeded threshold where boosted pkt can be sent */
  boolean last_rri = FALSE;           
    /* If current RRI exceeded threshold where nonboosted pkt can be sent */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  hdrrmaccmn_payload_size_enum_type max_ps = HDRRMACCMN_0_BITS; 
                                            /* upper bound on cand_ps_set */
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

#ifdef FEATURE_HDR_RMAC4_DEBUG
  #error code not present
#endif /* FEATURE_HDR_RMAC4_DEBUG */

  ASSERT (pp_max_ps < HDRRMACCMN_MAX_PAYLOADS);

  /* 
   * If Zero max. T2P is allocated for this channel by RMAC4DRV, record
   * the reason for logging purpose.
   */
  if (max_tx_t2p == HDRRMAC4DRV_NEG_INFINITY_T2P)  
  {
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hdr_inactive_reason =
      (hdrrmac4drv_hdr_mode_enum_type)
      (((uint8) hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hdr_inactive_reason) |
      ((uint8) HDRRMAC4DRV_HDR_MODE_0_T2P));

    /* not allowed to allocate any PS */
    ps_set = hdrrmaccmn_null_ps_set;

    /* Log tx_t2p_max as 0, carrier is in data-disallowed state, no data to be assigned */
    hdrrmac4_logging.tx_t2p_max[phy_chan_idx] = 0;
  }
  else
  {
    /* 1. Check TxT2PNominal and PA headroom constraints */
    for( ps = 1; ps <= (uint8) pp_max_ps; ps++ )
    {
      /* get TxT2PNominal for this packet size and tm */
      max_db = hdrrmac4_config_ptr->t2p_info[tm].tx_t2p_info[ps].max_db;

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
      if ( ( hdrrmac4_var_ptr->t2p_boost_enabled )
        && ( ( ( hdrrmac4_var_ptr->t2p_boost.state == 
        HDRRMACCMN_T2P_BOOST_PRE_TA_ACTIVE )
        && ( ( hdrrmac4_config_ptr->t2p_info[tm].tx_t2p_info[ps].term_target + 1 ) >
        hdrrmac4_var_ptr->phy_chan[phy_chan_idx].ll_subpackets_available )
        && ( ( ps >= HDRRMACCMN_T2P_BOOST_PRE_TA_RRI_THRESH ) 
           || ( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].sector_info.frab < 
                HDRRMACCMN_T2P_BOOST_RRI_RAB_THRESH ) )
        && ( ps >= HDRRMACCMN_T2P_BOOST_PRE_TA_RRI_THRESH ) )
        || ( ( hdrrmac4_var_ptr->t2p_boost.state == 
        HDRRMACCMN_T2P_BOOST_POST_TB_ACTIVE )
        && ( ps >= HDRRMACCMN_T2P_BOOST_POST_TB_RRI_THRESH ) ) 
#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
        || ( ( hdrrmac4_var_ptr->t2p_boost.state == 
               HDRRMACCMN_T2P_BOOST_RTT_REDUCT_ACTIVE ) &&
             ( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode == HDRRMACCMN_TX_MODE_HI_CAP ) &&
             ( hdrrmac4_config_ptr->t2p_info[tm].tx_t2p_info[ps].term_target + 1 > 2 ) )
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
         ) )
      {
        /* Use boosted T2P if T2P boost is enabled and if the conditions
           in any of the following three groups are satisfied
           Group-1:
             - Pre-tuneaway T2P boost is active AND
             - Packet termination target is greater than the number of
               subpackets available for transmission before tuneaway AND
             - Packet size is greater than or equal to RRI threshold for
               pre-tuneaway T2P boost
           Group-2:
             - Post-tuneback T2P boost is active AND
             - Packet size is greater than or equal to RRI threshold for
               post-tuneback T2P boost
           Group-3:
             - RTT reduction is active
             - Transmission mode is HI-CAP
             - Termination Target > 2
           */

        if ( hdrrmac4_var_ptr->t2p_boost.tx_t2p_info[ps].max_db <= max_tx_t2p )
        {
          max_db = hdrrmac4_var_ptr->t2p_boost.tx_t2p_info[ps].max_db;

          /* Set bit position corresponding to this packet size in boosted
             RRI mask */
          hdrrmac4_var_ptr->boosted_rri_mask[phy_chan_idx] |= (1 << ps);
        }
        else if ( !last_boosted )
        {
          last_boosted = TRUE;

          HDR_MSG_PROT_6( MSG_LEGACY_MED, 
                          "T2P boost: type:%d ch_idx:%d TM:%d RRI:%d stop boost max_t2p %d>%d",
                          hdrrmac4_var_ptr->t2p_boost.state,
                          phy_chan_idx, tm, ps,
                          hdrrmac4_var_ptr->t2p_boost.tx_t2p_info[ps].max_db,
                          max_tx_t2p );

         if ( hdrrmac4_var_ptr->t2p_boost.state == 
              HDRRMACCMN_T2P_BOOST_PRE_TA_ACTIVE )
         {
           /* For TA, prefer boosted lower PS */
           max_db = hdrrmac4_var_ptr->t2p_boost.tx_t2p_info[ps].max_db;
         }
        }
      }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

      if( ( hdrrmac4_config_ptr->t2p_info[tm].tx_t2p_info[ps].nominal > 
            hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
                sum_pot_t2p_outflow[tm] ) ||
          ( max_db > max_tx_t2p  ) )
      {
        /* Prune candidate set */
        ps_set = hdrrmac4_diff_ps_sets( ps_set,
                                        (hdrrmaccmn_ps_set_type) (1 << (ps - 1) ));

        if ( !last_rri )
        {
          last_rri = TRUE;

          HDR_MSG_PROT_9( MSG_LEGACY_MED, 
                          "T2P boost: ch_idx:%d TM:%d RRI:%d LL:%d FRAB:%d nominal:%d/%d max:%d/%d",
                          phy_chan_idx, tm, ps,
                          hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
                              ll_subpackets_available,
                          hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
                              sector_info.frab,
                          hdrrmac4_config_ptr->t2p_info[tm].tx_t2p_info[ps].
                              nominal,
                          hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
                              sum_pot_t2p_outflow[tm],
                          max_db,
                          max_tx_t2p );
        }
      }
    }

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
    /* For RL estimate purpose, pick the Max Packet Size from Candidate set 
       before applying Permitted Payload constraint */
    max_ps = (hdrrmaccmn_payload_size_enum_type)
               ( hdrrmacmath_msb_pos( ps_set ) + 1 );

     /* Update MAC limited packet size */
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].mac_lmtd_rri = MAX( 
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].mac_lmtd_rri, max_ps );
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

    /* 2. Prune candidate set */
    ps_set &= ( 1 << (uint8) pp_max_ps ) - 1;

    /* Save this for logging  */
    hdrrmac4_logging.tx_t2p_max[phy_chan_idx] = max_tx_t2p;
  }

  return ps_set;

} /* hdrrmac4_calc_candidate_ps_subset */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_PS_SET_MEMBER                                        INTERNAL

DESCRIPTION
  This function determines whether or not the given element is a member of the
  set 

DEPENDENCIES
  None.

PARAMETERS
  The element (packet size, ps) and the set of packet sizes, ps_set

RETURN VALUE
  TRUE if element is in the set; FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL boolean hdrrmac4_ps_set_member
(
  hdrrmaccmn_payload_size_enum_type ps,
  hdrrmaccmn_ps_set_type            ps_set
)
{
  boolean is_member = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  if( ( ps_set & ( 1 << ((uint8)ps - 1) ) ) != hdrrmaccmn_null_ps_set )
  {
    is_member = TRUE;
  }

  return is_member;

} /* hdrrmac4_ps_set_member */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_DETERMINE_MIN_T2P_PS                                INTERNAL

DESCRIPTION
  This function finds a packet size >= min_ps in cand_ps_set that has mininum
  TxT2P

DEPENDENCIES
  None.

PARAMETERS
  The mininum packet size (min_ps)
  The set of packet sizes under consideration (cand_ps_set)
  The transmission mode (tm)

RETURN VALUE
  The best packet size index in the given input set

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrrmaccmn_payload_size_enum_type hdrrmac4_determine_min_t2p_ps
(
  hdrrmaccmn_payload_size_enum_type min_ps,
  hdrrmaccmn_ps_set_type            cand_ps_set,
  hdrrmaccmn_tx_mode_enum_type      tm
)
{
  hdrrmaccmn_payload_size_enum_type ps; /* packet size array loop index */
  hdrrmaccmn_payload_size_enum_type best_ps = HDRRMACCMN_0_BITS;
  uint32                            smallest_nom_tx_t2p = 0xFFFFFFFF;
                                      /* absurdly large enough to make
                                       * comparison easier
                                       */ 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  if( min_ps != HDRRMACCMN_0_BITS )
  {
    best_ps = min_ps; /* initialize best packet size to the minimum */
  
    /* min_ps is a lower bound on cand_ps_set. Now find a packet size
     * >= min_ps in cand_ps_set with the smallest nominal TxT2P */

    for( ps = min_ps; ps < HDRRMACCMN_MAX_PAYLOADS; ps++ )
    {
      if( hdrrmac4_ps_set_member( ps, cand_ps_set ) == TRUE )
      {
        if( hdrrmac4_config_ptr->t2p_info[tm].tx_t2p_info[ps].nominal
            < smallest_nom_tx_t2p )
        {
          smallest_nom_tx_t2p = 
            hdrrmac4_config_ptr->t2p_info[tm].tx_t2p_info[ps].nominal;
  
          best_ps = ps; /* found a better packet size */
        }
      }
    } /* for */
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Min PS: 0");
  }

  return best_ps;

} /* hdrrmac4_determine_min_t2p_ps */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_DETERMINE_PACKET_SIZE                                INTERNAL

DESCRIPTION
  This function selects a suitable Physical Layer packet size

DEPENDENCIES
  None.

PARAMETERS
  The set of packet sizes under consideration. This is going to be either

    1. {128,...,12288} ==> The universe of available packet sizes, or,
    2. {PayloadThresh,...,12288} ==> A subset of all available packet sizes

  If in step #1 above a packet size of 12288 is returned and the transmission
  mode is HI_CAP; then we make a second pass at trying to find a packet size
  that is in [PayloadThresh..12288] in LO_LAT mode with the idea of achieving
  higher data rates.

RETURN VALUE
  The best packet size index in the given input set

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL hdrrmaccmn_payload_size_enum_type hdrrmac4_determine_packet_size
( 
  uint8                               phy_chan_idx,
  hdrrmaccmn_ps_set_type              init_ps_set,
  hdrrmaccmn_payload_size_enum_type   pp_max_ps,
  uint32                              max_tx_t2p
)
{
  hdrrmaccmn_payload_size_enum_type best_ps = HDRRMACCMN_0_BITS; 
  hdrrmaccmn_ps_set_type            cand_ps_set;
  hdrrmaccmn_payload_size_enum_type max_ps = HDRRMACCMN_0_BITS; 
                                             /* upper bound on cand_ps_set */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* First calculate the aggregate potential outflow of eligible flows */
  hdrrmac4_sum_pot_t2p_outflows(phy_chan_idx);

  cand_ps_set = hdrrmac4_calc_candidate_ps_subset(
    phy_chan_idx,
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode, 
    init_ps_set, 
    pp_max_ps, 
    max_tx_t2p );

  if( ( cand_ps_set == hdrrmaccmn_null_ps_set ) 
      &&
      ( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode == 
        HDRRMACCMN_TX_MODE_LO_LAT ) )
  {
    /* Try to find a packet size in HiCap mode */
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode = 
      HDRRMACCMN_TX_MODE_HI_CAP; 
    cand_ps_set = hdrrmac4_calc_candidate_ps_subset(
                    phy_chan_idx,
                    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode, 
                    init_ps_set, 
                    pp_max_ps, 
                    max_tx_t2p );
  }

  if( ( cand_ps_set == hdrrmaccmn_null_ps_set ) 
      &&
      ( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode == 
        HDRRMACCMN_TX_MODE_HI_CAP ) )
  {
    best_ps = HDRRMACCMN_0_BITS;
  }
  else
  {
    uint8                             flow;
    hdrrmaccmn_payload_size_enum_type ps; /* packet size array loop index */

    hdrrmaccmn_payload_size_enum_type min_ps = HDRRMACCMN_0_BITS; 
                                              /* lower bound on cand_ps_set */

    boolean                           done = FALSE;   
    uint32                            actual_sum_q_outflow = 0;
    hdrrmaccmn_tx_mode_enum_type      tm = hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode;

#ifdef FEATURE_HDR_RMAC4_DEBUG
    #error code not present
#endif

    /* calculate aggregate queue outflow of eligible MAC flows */
    for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
    {
      hdrrmac4_per_nc_vars_struct_type *flow_ptr;

      flow_ptr = 
        &( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
            flow[hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id] );

      actual_sum_q_outflow += 
        flow_ptr->queue_info.q_outflow[hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
                                        pkt_tx_mode];
    }

    /* Remember this as it is used again later */
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].actual_sum_q_outflow = 
      actual_sum_q_outflow;

    /* If the number of bits to be sent is bigger than the biggest PS
     * we are allowed then just pick that as the best PS
     */
    max_ps = (hdrrmaccmn_payload_size_enum_type) 
              (hdrrmacmath_msb_pos( cand_ps_set ) + 1);

    if( actual_sum_q_outflow > hdrrmaccmn_pkt_size_array[max_ps] )
    {
      best_ps = max_ps;
    }
    else
    {
      /* find the smallest packet size that can carry this many bits */
      for( ps = HDRRMACCMN_128_BITS; !done && (ps <= max_ps); ps++ )
      {
        if( hdrrmaccmn_pkt_size_array[ps] >= actual_sum_q_outflow )
        {
          done = TRUE;
          min_ps = ps;
        }
      }

      best_ps = hdrrmac4_determine_min_t2p_ps(min_ps,cand_ps_set,tm);
    }
    
  } /* cand_ps_set != NULL */

  hdrrmac4_logging.cand_ps_set[phy_chan_idx] = cand_ps_set;
    /* record cand_ps_set in the logging structure  */

  return best_ps;

} /* hdrrmac4_determine_packet_size */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_RESET_ACTIVE_FLOWS                                   INTERNAL

DESCRIPTION
  This function resets the queue sizes of active flows to 0 and sets actual
  packet size of all open channels to 0.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_reset_active_flows( void )
{
  uint8 flow;

  uint8 chan;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
  {
    hdrrmac4_active_flows_set[flow].size = 0;
  }

  for (chan = 0; chan < hdrrmac4_var_ptr->num_open_state_chans; chan++)
  {
    hdrrmac4_var_ptr->phy_chan[chan].actual_pkt_size = 0;
  }

  return;

} /* hdrrmac4_reset_active_flows */


 /* EJECT */
/*=============================================================================
FUNCTION hdrrmac4_order_flows                                          INTERNAL

DESCRIPTION
  This function order the flows in the hdrrmac4_elig_flows_set[][] according to
  the flow priority, with the flow with the highest priority being the first
  entry in the list.  The signaling flow (0), if present in 
  hdrrmac4_elig_flows_set[][], always is the first in the list.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_order_flows
(
  uint8 phy_chan_idx
)
{
  hdrpcp_mac_flow_allocation_type flow_tmp;

  uint8 flow_i, flow_j;
    /* flow loop indexes */

  uint8 start_entry = 0;
    /* 0 if siglaing flow is not present; 1 otherwise */

  int32 max_pri, pri_j;
    /* max. flow priority & flow j priority */

  uint8 hpf;
    /* higher priority flow index */

  uint8 log_chan_id = hdrrmac4_from_phy_to_log(phy_chan_idx);
    /* Logical channel id */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  /* Sanity check */
  ASSERT(log_chan_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS);

  /*
   * Assumption is that if signaling flow is present, it will always be
   * hdrrmac4_elig_flows_set[][0].  Don't move it according to the computed
   * priority.
   */
  if (hdrrmac4_elig_flows_set[phy_chan_idx][0].mac_flow_id == 0)
  {
    /* skip the first entry in the sorting loop*/
    start_entry = 1;
  }

  /* 
   * Selection Sort- swap poisition with the current one if a higher priority
   * one is found
   */
  for (flow_i = start_entry; 
       flow_i < hdrrmac4_num_elig_flows[phy_chan_idx]; 
       flow_i++)
  {
    /* assuming flow_i is so far the highest priority flow */
    hpf = flow_i;
    max_pri = 
      hdrrmac4_config_ptr->nc[(hdrrmac4_elig_flows_set[phy_chan_idx][flow_i].mac_flow_id)]
                             [log_chan_id].gu_gd_ptr->pri;

    for (flow_j = (flow_i + 1); 
         flow_j < hdrrmac4_num_elig_flows[phy_chan_idx];
         flow_j++)
    {
      pri_j =
        hdrrmac4_config_ptr->nc[(hdrrmac4_elig_flows_set[phy_chan_idx][flow_j].mac_flow_id)]
                               [log_chan_id].gu_gd_ptr->pri;

      if (pri_j > max_pri)
      {
        hpf = flow_j;
        max_pri = pri_j;
      }
    } /* flow_j */

    /* swap between flow_i & hpf if they are different */
    if (flow_i != hpf)
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      /* copy flow_i to tmp */
      flow_tmp = hdrrmac4_elig_flows_set[phy_chan_idx][flow_i];

      /* copy hpf to flow_i */
      hdrrmac4_elig_flows_set[phy_chan_idx][flow_i] = hdrrmac4_elig_flows_set[phy_chan_idx][hpf];

      /* copy tmp to hpf */
      hdrrmac4_elig_flows_set[phy_chan_idx][hpf] = flow_tmp;
#else
      /* copy flow_i to tmp */
      memcpy ((void *)&flow_tmp,
              (void *)&hdrrmac4_elig_flows_set[phy_chan_idx][flow_i],
              sizeof(hdrpcp_mac_flow_allocation_type));

      /* copy hpf to flow_i */
      memcpy ((void *)&hdrrmac4_elig_flows_set[phy_chan_idx][flow_i],
              (void *)&hdrrmac4_elig_flows_set[phy_chan_idx][hpf],
              sizeof(hdrpcp_mac_flow_allocation_type));

      /* copy tmp to hpf */
      memcpy ((void *)&hdrrmac4_elig_flows_set[phy_chan_idx][hpf],
              (void *)&flow_tmp,
              sizeof(hdrpcp_mac_flow_allocation_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }

  } /* flow_i */

  return;

} /* hdrrmac4_order_flows */


/*=============================================================================
FUNCTION HDRRMAC4_ROUND_DOWN                                           INTERNAL

DESCRIPTION
  This function loops until Sum(d'_i) (for all eligible MAC flows, i) 
  <= selected packet size

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#ifndef FEATURE_HDR_RMAC4_DEBUG_ROUND_DOWN
RMAC4_LOCAL void hdrrmac4_round_down
(
  uint8 phy_chan_idx,
  uint8 loop_cnt
)
{
  uint8 flow = 0;

  static uint8 rr_index = 0;
    /* round-robin index */

  boolean first_tie = TRUE;

  hdrrmac4_per_nc_vars_struct_type *flow_ptr;
    /* convenient pointer to flow */

  hdrrmac4_phy_chan_vars_struct_type  *phy_chan_ptr;
    /* convenient pointer to channel */

  boolean done = FALSE;
    /* stopping condition for while loop */

  int32 quantum = 0, vacancy;
    /* unit of allotment and remaining space left in packet: both in bytes */

  int32 pri[HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS] = {0};
    /* Array to keep track of flow priorities */

  uint32 sum_allotment = 0;
    /* variable to keep track of current total allotment among all flows */

  hdrrmaccmn_tx_mode_enum_type tm;
  hdrrmaccmn_payload_size_enum_type ps;

  uint8 log_chan_id = hdrrmac4_from_phy_to_log(phy_chan_idx);
    /* logical channel number */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  ASSERT(log_chan_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS);

  phy_chan_ptr  = &hdrrmac4_var_ptr->phy_chan[phy_chan_idx];
  vacancy       = phy_chan_ptr->max_pkt_size;
  tm            = phy_chan_ptr->pkt_tx_mode;
  ps            = phy_chan_ptr->pkt_size_index;

  ASSERT(ps < HDRRMACCMN_MAX_PAYLOADS);

  /* Arrange the hdrrmac4_elig_flows_set[][] according to flow priorities */
  hdrrmac4_order_flows(phy_chan_idx);

  /* Signaling flow (0) gets the highest priority. It gets the smaller of
   * its q_outflow and max packet size.
   */
  if( hdrrmac4_elig_flows_set[phy_chan_idx][0].mac_flow_id == 0 )
  {
    hdrrmac4_elig_flows_set[phy_chan_idx][0].size = 
      hdrrmacmath_min_u32( phy_chan_ptr->max_pkt_size, 
                           phy_chan_ptr->flow[0].queue_info.q_outflow[tm] );

    /* Signaling flow has been allotted its maximum, so remove it from
     * further consideration by setting its priority to the lowest value
     */
    pri[0] = hdrrmaccmn_neg_infinity;

    sum_allotment += hdrrmac4_elig_flows_set[phy_chan_idx][0].size;

    vacancy = phy_chan_ptr->max_pkt_size - 
                hdrrmac4_elig_flows_set[phy_chan_idx][0].size;

    flow++;
  }

  /* If signaling fills up all of the packet then skip the rest of this
   * routine
   */
  if( vacancy <= 0 )
  {
    done = TRUE;
  }
  else
  {
    /* Allocate remaining space in packet by dividing the available bytes
     * into loop_cnt parts, and give 1 part to the highest priority flow 
     * (excluding signaling) each time through the loop until the packet 
     * is filled.
     */
    if( (vacancy % loop_cnt) != 0 )
    {
      /* Round up */
      quantum = (vacancy + loop_cnt) / loop_cnt;
    }
    else
    {
      quantum = vacancy / loop_cnt;
    }

    /* Save this for logging later */
    hdrrmac4_logging.round_down_quantum[phy_chan_idx] = quantum;

    /* Initialize flow priorities except signaling flow (0) */
    for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
    {
      if( hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id != 0 )
      {
        pri[flow] = 
          hdrrmac4_config_ptr->nc[(hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id)]
                                 [log_chan_id].gu_gd_ptr->pri;

#ifdef FEATURE_HDR_RMAC4_DEBUG
        #error code not present
#endif
      }
    }
    
  }

  while( !done )
  {
    uint8 hpf = HDRRMACCMN_MAC_FLOW_ID_INVALID;
      /* variable to keep track of highest priority flow */

    int32 max_pri = hdrrmaccmn_neg_infinity;
      /* -2^31 initial value will always make first comparison succeed */

    /* If the space left is less than the quantum then set the quantum to 
     * this minimum increment 
     */
    if( quantum > (int32)(phy_chan_ptr->max_pkt_size - sum_allotment) ) 
    {
      quantum = phy_chan_ptr->max_pkt_size - sum_allotment;
    }
    /* Find the highest priority flow (hpf). Priority of flow, i, is: 
     * T2PUp_i/T2PDown_i evaluated @ (d'_i * TxT2PNominal[tm][ps]/PS, FRAB).
     */
    for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
    {
      /* flow offset by rr_index = last non-hpf flow with max_pri */
      uint8 flow_rr = (flow + rr_index) % hdrrmac4_num_elig_flows[phy_chan_idx];

      if( pri[flow_rr] > max_pri )
      {
        hpf = flow_rr;
        max_pri = pri[flow_rr];
      }
    }

    if( hpf != HDRRMACCMN_MAC_FLOW_ID_INVALID )
    {
      uint32 increment;
        /* number of bytes added to a flow's allotment */

      if( first_tie == TRUE )
      {
        /* remember the non-hpf flow with max_pri */
        for( flow = 0; 
             ( first_tie && (flow < (hdrrmac4_num_elig_flows[phy_chan_idx] - 1)) ); 
             flow++ )
        {
          uint8 tied_flow = (flow + hpf + 1) % hdrrmac4_num_elig_flows[phy_chan_idx];

            /* Only detect the first tie */
          if( pri[tied_flow] == max_pri ) 
          {
            rr_index = tied_flow;
            first_tie = FALSE;
          }
        }
      }

      /* Fixing KW errors */ 
      HDR_ASSERT(hpf < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS);

      flow_ptr = 
        &(phy_chan_ptr->flow[hdrrmac4_elig_flows_set[phy_chan_idx][hpf].mac_flow_id] );

      /* Increment flow's allotment by min(quantum, (q_outflow_i - d'_i))
       * Noting that d'_i is 0 initially for all eligible flows
       */
      increment = hdrrmacmath_min_u32( quantum, 
                              (flow_ptr->queue_info.q_outflow[tm]
                               - hdrrmac4_elig_flows_set[phy_chan_idx][hpf].size) );

      hdrrmac4_elig_flows_set[phy_chan_idx][hpf].size += increment;

      /* Update aggregate allotment */
      sum_allotment += increment;

      vacancy -= increment;

      if( vacancy <= 0 )
      {
        done = TRUE;
      }

      /* Skip the expensive step of updating priorities if we are done */
      if( !done )
      {
          /* Update flow priority */
        if( hdrrmac4_elig_flows_set[phy_chan_idx][hpf].size 
            == flow_ptr->queue_info.q_outflow[tm] )
        {
          /* If this flow is completely filled set its priority to -2^31 */
          pri[hpf] = hdrrmaccmn_neg_infinity;
        }
        else
        {
          uint32 t2p_axis_pt;
          int32 t2p_axis_pt_in_db;
  
          t2p_axis_pt= ( hdrrmac4_elig_flows_set[phy_chan_idx][hpf].size 
                       * hdrrmac4_config_ptr->t2p_info[tm].tx_t2p_info[ps].nominal
                       + (phy_chan_ptr->max_pkt_size/2) )
                       / phy_chan_ptr->max_pkt_size;

          t2p_axis_pt_in_db = hdrrmacmath_lin_2_db(t2p_axis_pt);

          /* These functions operate in dB domain, so just subtract to get
           * priority
           */
          pri[hpf] = 
            hdrrmac4_calc_t2p_up( 
              log_chan_id,
              hdrrmac4_elig_flows_set[phy_chan_idx][hpf].mac_flow_id, 
              t2p_axis_pt_in_db,
              phy_chan_ptr->sector_info.frab ) 
            - hdrrmac4_calc_t2p_down( 
                log_chan_id,
                hdrrmac4_elig_flows_set[phy_chan_idx][hpf].mac_flow_id, 
                t2p_axis_pt_in_db,
                phy_chan_ptr->sector_info.frab );
        }
      }

#ifdef FEATURE_HDR_RMAC4_DEBUG
      #error code not present
#endif
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "Highest priority flow not found." );
      done = TRUE;
    }
  } /* while */

  return;

} /* hdrrmac4_round_down */

#else
#error code not present
#endif /* !FEATURE_HDR_RMAC4_DEBUG_ROUND_DOWN */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_ALLOT_FLOW_DATA                                      INTERNAL

DESCRIPTION
  This function determines suggested allotments (d'_i) to each eligible MAC flow

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_allot_flow_data
(
  uint8 phy_chan_idx
)
{
  uint8                               flow;
  hdrrmac4_phy_chan_vars_struct_type *phy_chan_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[phy_chan_idx];

  /* If test mode is in effect ignore bucket calculations */
  if( (hdrrmac4_test_ps_index_fixed != HDRRMACCMN_IS856_A_RRI) &&
      (hdrrmac4_test_ps_index_fixed <= HDRRMACCMN_12288_BITS ) )
  {
    uint32 actual_sum_q = 0;

    /* Ignore bucket-based queue outflow values */
    for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
    {
      hdrrmac4_per_nc_vars_struct_type *flow_ptr;

      flow_ptr = 
        &( phy_chan_ptr->flow[hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id] );

      /* 
       * Set the suggested flow allocation to the smaller of the initial 
       * queue length and the fixed packet size set by the AT command.
       */
      if (hdrrmaccmn_pkt_size_array[hdrrmac4_test_ps_index_fixed] >
          flow_ptr->queue_info.initial_size)
      {
        hdrrmac4_elig_flows_set[phy_chan_idx][flow].size = 
          flow_ptr->queue_info.initial_size;
      }
      else
      {
        hdrrmac4_elig_flows_set[phy_chan_idx][flow].size = 
          hdrrmaccmn_pkt_size_array[hdrrmac4_test_ps_index_fixed];
      }

      actual_sum_q += hdrrmac4_elig_flows_set[phy_chan_idx][flow].size;
    }

    phy_chan_ptr->actual_sum_q_outflow = actual_sum_q;
  }
  else
  {
    /* If sum of all flow sizes is > Physical Layer packet size then round 
     * down flow allocations until they fit in the packet. This is achieved 
     * by setting each flow's allotment to 0 and then "growing" them in order
     * of highest priority (see definition in roun down function) until the 
     * packet is filled.
     */
    if( phy_chan_ptr->actual_sum_q_outflow > phy_chan_ptr->max_pkt_size )
    {
      for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
      {
        hdrrmac4_elig_flows_set[phy_chan_idx][flow].size = 0;
      }
  
      hdrrmac4_round_down( phy_chan_idx, HDRRMACCMN_ROUND_DOWN_LOOP_COUNT );
    }
    else
    {
      for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
      {
        hdrrmac4_per_nc_vars_struct_type *flow_ptr;
  
        flow_ptr = 
          &( phy_chan_ptr->flow[hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id] );
  
        hdrrmac4_elig_flows_set[phy_chan_idx][flow].size =
          flow_ptr->queue_info.q_outflow[phy_chan_ptr->pkt_tx_mode];
      }
    }
  }

  for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
  {
    hdrrmac4_per_nc_vars_struct_type *flow_ptr;

    flow_ptr = 
      &( phy_chan_ptr->flow[hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id] );

    flow_ptr->queue_info.suggested_size = 
      hdrrmac4_elig_flows_set[phy_chan_idx][flow].size;
  }

  /* TODO Current PCP implementation does not round up. Ordering of F*
   * becomes important once PCP implements round up.
   */

  return;

} /* hdrrmac4_allot_flow_data */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_ALLOT_ZERO_FLOW_DATA                               INTERNAL

DESCRIPTION
  This function sets the suggested allotments (d'_i) to each eligible MAC 
  flow to 0.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_allot_zero_flow_data
(
  uint8 phy_chan_idx
)
{
  uint8                               flow;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
  {
    hdrrmac4_per_nc_vars_struct_type *flow_ptr;

    /* Set the suggested flow size to 0 */
    hdrrmac4_elig_flows_set[phy_chan_idx][flow].size = 0;

    flow_ptr = 
      &( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
          flow[hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id] );

    /* 
     * Also set the global per channel per flow suggested size correctly
     * for logging.
     */
    flow_ptr->queue_info.suggested_size = 
      hdrrmac4_elig_flows_set[phy_chan_idx][flow].size;
  }

  return;

} /* hdrrmac4_allot_flow_data */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_INIT_ACTIVE_FLOWS_SET_TMP                           INTERNAL

DESCRIPTION
  This function initializes the active MAC flows set information in a temporary
  copy that can be used for per channel allocation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_init_active_flows_set_tmp( void )
{
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy ((void *)&hdrrmac4_active_flows_set_tmp[0],
          sizeof(hdrrmac4_active_flows_set_tmp), 
          (void *)&hdrrmac4_active_flows_set[0],
          sizeof(hdrrmac4_active_flows_set));
#else
  memcpy ((void *)&hdrrmac4_active_flows_set_tmp[0],
          (void *)&hdrrmac4_active_flows_set[0],
          sizeof(hdrrmac4_active_flows_set_tmp));
#endif /* FEATURE_MEMCPY_REMOVAL */

  return;

} /* hdrrmac4_init_active_flows_set_tmp */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_UPDATE_ACTIVE_FLOWS_SET_TMP                          INTERNAL

DESCRIPTION
  This function updates the active MAC flows set information in the temporary
  copy after each channel allocation.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index of the channel that just completed
                 allocation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_update_active_flows_set_tmp
(
  uint8 phy_chan_idx
)
{
  uint8 flow_i, flow_j;
    /* flow index for active_flows_set_tmp & elig_flows_set */

  boolean found;
    /* found a matched flow */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for (flow_i = 0; flow_i < hdrrmac4_num_active_flows; flow_i++)
  {
    found = FALSE;

    for (flow_j = 0; 
         (!found && (flow_j < hdrrmac4_num_elig_flows[phy_chan_idx]));
         flow_j++)
    {
      /* 
       * Can only update the queue length if the mac_flow_id from the 
       * acitve_flows_set_tmp matches with the mac_flow_id from the
       * elig_flows_set.
       */
      if (hdrrmac4_active_flows_set_tmp[flow_i].mac_flow_id ==
          hdrrmac4_elig_flows_set[phy_chan_idx][flow_j].mac_flow_id)
      {
        if (hdrrmac4_active_flows_set_tmp[flow_i].size >
            hdrrmac4_elig_flows_set[phy_chan_idx][flow_j].size)
        {
          /* subtract the allocated amount from the current remaining size */
          hdrrmac4_active_flows_set_tmp[flow_i].size -= 
            hdrrmac4_elig_flows_set[phy_chan_idx][flow_j].size;
        }
        else
        {
          hdrrmac4_active_flows_set_tmp[flow_i].size = 0;
        }

        /* Found a matched one.  Update the next queue length of the next flow. */
        found = TRUE;
      }
    }

    HDR_MSG_DEBUG_3( HDR_MSG_DBG_RTCMAC_DECISIONS,
                     "Updated RL %d, Q[%d]= %d",
                     phy_chan_idx,
                     hdrrmac4_active_flows_set_tmp[flow_i].mac_flow_id,
                     hdrrmac4_active_flows_set_tmp[flow_i].size );
  }

  return;

} /* hdrrmac4_update_active_flows_set_tmp */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_DOWNGRADE_PACKET_SIZE                               INTERNAL

DESCRIPTION
  This function performs packet-size downgrade

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx    - Physical channel index
  tx_mode_changed - flag indicating if tx_mode has been changed from HI_CAP
  to LO_LAT

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
RMAC4_LOCAL void hdrrmac4_downgrade_packet_size
(
  uint8   phy_chan_idx,
  boolean tx_mode_changed
)
{
  hdrrmaccmn_payload_size_enum_type min_ps_index = 
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_size_index;
                            /* min_ps_index tracks the minimum packet 
                             * size chosen by RTCMAC to carry the pcp 
                             * packet [not considering min t2p requirements]
                             */
  boolean ps_downgrade = FALSE; /* flag to indicate if packet-size has been
                                 * downgraded
                                 */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  while ((min_ps_index > 0) && 
         (hdrrmac4_var_ptr->phy_chan[phy_chan_idx].actual_pkt_size <= 
         hdrrmaccmn_pkt_size_array[(uint8) min_ps_index - 1]))
  {
    min_ps_index--;
    ps_downgrade = TRUE;
  }

  if (ps_downgrade)
  {
    hdrrmaccmn_ps_set_type downgrade_cand_ps_set = hdrrmaccmn_null_ps_set;
    hdrrmaccmn_payload_size_enum_type ps_loop;

    /* Change tx_mode back to HI_CAP if pkt_size_index is below payload
     * threshold corresponding to LO_LAT mode
     */
    if ( (min_ps_index < hdrrmac4_config_ptr->payload_thresh) &&
         tx_mode_changed )
    {
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode = 
        HDRRMACCMN_TX_MODE_HI_CAP;
    }

    /* form a subset of allowed pkt sizes = 
    *     {min_ps_index,...,hdrrmac4_var_ptr->pkt_size_index}
    *            |                               |
    *   minimum size after downgrade             |
    *                              minimum size before downgrade
    */
    for (ps_loop = min_ps_index; 
         ps_loop <= hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_size_index;
         ps_loop++)
    {
      downgrade_cand_ps_set |= 1 << ( (uint8) ps_loop - 1 );
    }

    min_ps_index = hdrrmac4_determine_min_t2p_ps(
                      min_ps_index,
                      downgrade_cand_ps_set, 
                      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_tx_mode);

    HDR_MSG_PROT_2( MSG_LEGACY_HIGH,
                    "PSindex_before_downgrade = %d, PSindex_after_downgrade = %d",
                    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_size_index,
                    min_ps_index);              

    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_size_index = min_ps_index;
  }

  return;

} /* hdrrmac4_downgrade_packet_size */



/*=============================================================================
FUNCTION hdrrmac4_save_mac_info                                        INTERNAL

DESCRIPTION
  Saves MAC payload size equivalents (in bytes) for PA headroom, bucket
  level and T2PInflow in mac_info_raw_data.  The information is sent to the 
  VT application when hdrrmac4_get_mac_info() is called. 

DEPENDENCIES
  None.

PARAMETERS
  max_tx_t2p - Max T2P in dB

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_save_mac_info( uint32 max_tx_t2p )
{
#if 0 /* TODO */
  int8 flow_cnt;

  /* Entering critical section... */
  rex_enter_crit_sect(&mac_info_raw_data.mac_info_crit_sect);

  mac_info_raw_data.tm = hdrrmac3_var->pkt_tx_mode;
                                                     /* transmission mode */

  mac_info_raw_data.tt = hdrrmac3_config->t2p_info[mac_info_raw_data.tm].\
                         tx_t2p_info[HDRRMAC3_2048_BITS].term_target + 1;
                                                    /* termination target */

  mac_info_raw_data.t2p_conv_factor = hdrrmac3_config->\
                          t2p_info[mac_info_raw_data.tm].t2p_conv_factor;
                                                     /* conversion factor */

  for( flow_cnt = 0; flow_cnt < HDRRMAC3_MAX_NUM_MAC_FLOWS; flow_cnt++ )
  {
    mac_info_raw_data.bucket_level[flow_cnt] = 
                   hdrrmac3_var->flows[flow_cnt].bucket.bucket_level.curr;
                                                          /* bucket level */

    mac_info_raw_data.t2p_inflow[flow_cnt] = 
                   hdrrmac3_var->flows[flow_cnt].bucket.t2p_inflow.curr;
                                                            /* T2P inflow */
  }

  mac_info_raw_data.max_tx_t2p = max_tx_t2p;
                                                       /* max T2P (in dB) */

  /* Leaving critical section... */
  rex_leave_crit_sect(&mac_info_raw_data.mac_info_crit_sect);
#endif /* 0 */

  return;
} /* hdrrmac4_save_mac_info */


 /* EJECT */
/*===========================================================================
FUNCTION hdrrmac4_next_iteration_needed                              INTERNAL

DESCRIPTION
  This function determines if 2nd iteration of packet size determination is
  needed to optimize Eb/No efficiency.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if next iteration is needed
  FALSE - if next iteration is not needed

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL boolean hdrrmac4_next_iteration_needed
(
  uint32  *max_tx_t2p
)
{
  boolean   next_iter_needed = TRUE;
  uint8     chan_index;
  uint8     flow;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* only run 1 iteration for single carrier assignment */
  if (hdrrmac4_var_ptr->num_open_state_chans == 1)
  {
    next_iter_needed = FALSE;

    HDR_MSG_DEBUG( HDR_MSG_DBG_RTCMAC_DECISIONS,
                   "no 2nd iter- Single carrier assignment!");
  }

  /* only run 1 iteration if MaxPacketSize attribute disallows 12288-bit pkt */
  if (next_iter_needed)
  {
    if (hdrrmac4_config_ptr->max_packet_size != HDRRMACCMN_12288_BITS)
    {
      next_iter_needed = FALSE;

      HDR_MSG_DEBUG_1( HDR_MSG_DBG_RTCMAC_DECISIONS,
                       "no 2nd iter- MaxPacketSize %d",
                       hdrrmac4_config_ptr->max_packet_size);
    }
  }

  /* Look for a flow with non empty remaining queue length */
  if (next_iter_needed)
  {
    boolean found_non_empty_flow = FALSE;

    for (flow = 0; 
         (!found_non_empty_flow && (flow < hdrrmac4_num_active_flows));
         flow++)
    {
      if (hdrrmac4_active_flows_set_tmp[flow].size != 0)
      {
        found_non_empty_flow = TRUE;

        HDR_MSG_DEBUG_2( HDR_MSG_DBG_RTCMAC_DECISIONS,
                         "2nd iter- Remaining queue length %d in flow %d",
                         hdrrmac4_active_flows_set_tmp[flow].size,
                         flow );
      }
    }

    if (!found_non_empty_flow)
    {
      next_iter_needed = FALSE;

      HDR_MSG_DEBUG( HDR_MSG_DBG_RTCMAC_DECISIONS, "no 2nd iter- All q's empty!");
    }
  }

  if (next_iter_needed)
  {
    boolean found_qualified_rl = FALSE;

    for (chan_index = 0;
         (!found_qualified_rl && 
          (chan_index < hdrrmac4_var_ptr->num_open_state_chans));
         chan_index++)
    {
      uint8 open_chan = hdrrmac4_open_rl_list[chan_index];

      if ((hdrrmac4_var_ptr->phy_chan[open_chan].pkt_tx_mode ==
           HDRRMACCMN_TX_MODE_HI_CAP) &&
          (hdrrmac4_var_ptr->phy_chan[open_chan].pkt_size_index ==
           HDRRMACCMN_8192_BITS) &&
          (max_tx_t2p[open_chan] >= 
           hdrrmac4_config_ptr->t2p_info[HDRRMACCMN_TX_MODE_HI_CAP].
            tx_t2p_info[HDRRMACCMN_12288_BITS].max_db))

      {
        found_qualified_rl = TRUE;

        HDR_MSG_DEBUG_4( HDR_MSG_DBG_RTCMAC_DECISIONS,
                         "2nd iter- Qualified rl %d, tm %d, ps %d, max_tx_t2p %d",
                         open_chan,
                         hdrrmac4_var_ptr->phy_chan[open_chan].pkt_tx_mode,
                         hdrrmac4_var_ptr->phy_chan[open_chan].pkt_size_index,
                         max_tx_t2p[open_chan]);
      }
    }

    if (!found_qualified_rl)
    {
      next_iter_needed = FALSE;
    }
  }

  return (next_iter_needed);

} /* hdrrmac4_next_iteration_needed */


 /* EJECT */
/*===========================================================================
FUNCTION hdrrmac4_run_carrier_management                             INTERNAL

DESCRIPTION
  This function interacts with rmac4drv to perform the carrier management 
  function.

DEPENDENCIES
  None.

PARAMETERS
  max_tx_t2p_ptr - pointer to an array of HDRRMAC4_MAX_NUM_REV_PHY_CHANS
                   elements of max_tx_t2p values to be filled when function
                   returns.

RETURN VALUE
  None.

SIDE EFFECTS
  Update the contents of hdrrmac4_ranked_rl_list[].
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_run_carrier_management
(
  uint32  *max_tx_t2p_ptr
)
{
  boolean channel_dropped[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* array of flags indicating if the channel to be dropped */

  uint8   chan;
    /* channel loop index */

  uint8   num_dropped_rls = 0;
    /* number of dropped RL to report */

  uint8   dropped_chan_mask = 0;
    /* Mask with bits corresponding to indices of dropped channels set */

  hdrrmac4_drop_info_type drop_info[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* Carrier drop information record */

  uint8   num_chans_pa_headroom;
    /* Total number of channels PA headroom can support */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Inform rmac4drv if flow 0 has data */
  if ((hdrrmac4_active_flows_set[0].size != 0) ||
      hdrrmac4_var_ptr->disallow_slp_switch)
  {
    if (hdrrmac4_force_slp_switch)
    {
      /* Clear the flag */
      hdrrmac4_force_slp_switch = FALSE;

      hdrrmac4drv_flow0_has_data(FALSE);
    }
    else
    {
      hdrrmac4drv_flow0_has_data(TRUE);
    }

    /* clear the flag for next subframe */
    hdrrmac4_var_ptr->disallow_slp_switch = FALSE;
  }
  else
  {
    /* Reset force SLP switch flag */
    hdrrmac4_force_slp_switch = FALSE;

    hdrrmac4drv_flow0_has_data(FALSE);
  }

  /* Find out from the DRV. the max. TxT2P allowed for all the open channels */
  hdrrmac4drv_get_max_t2p(max_tx_t2p_ptr);

  /* 
   * Retrieved the ordered channel list from rmac4drv.  The channels are
   * ordered according to the average TX pilot power.  The lower the average
   * TX pilot power, the higher the priority.  The return value indicates
   * number of OPEN channel from rmac4drv point of view.
   */
  (void) hdrrmac4drv_get_prioritized_carriers(&hdrrmac4_ranked_rl_list[0]);

  /* Retrieve the SLP carrier information */
  hdrrmac4_var_ptr->slp_carrier_idx = hdrrmac4drv_get_slp_carrier();

  /* Retrieve the carrier drop information */
  num_chans_pa_headroom = 
    hdrrmac4drv_get_rl_add_drop_info(&channel_dropped[0]);

  /* Find out the channels to be dropped */
  for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
  {
    if ( ( ( hdrrmac4_var_ptr->phy_chan[chan].chan_state ==
             HDRRMAC4_CHAN_SETUP_STATE ) ||
           ( hdrrmac4_var_ptr->phy_chan[chan].chan_state ==
             HDRRMAC4_CHAN_OPEN_STATE ) ) &&  
         ( channel_dropped[chan] ) )
    {
      /* fill in the dropped information */
      drop_info[num_dropped_rls].phy_chan_idx = chan;
      drop_info[num_dropped_rls].reason_code = 
        HDRRMAC4_RL_DROP_REASON_HEADROOM_LIMITATION;

      /* increment the drop carrier count */
      num_dropped_rls++;

      /* Added to dropped channel mask */
      dropped_chan_mask |= (1 << chan);
    }
  }

  /* Update PA headroom */
  HDRRMAC4_CARRIER_REQ_UPDATE_NUM_CHANS_PA_HEADROOM(num_chans_pa_headroom);

  /* Generate and send CarrierDrop Msg */
  if ( (num_dropped_rls != 0) && 
       (num_dropped_rls < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS ) )
  {
    hdrrmac4_send_rl_drop_msg(
      hdrrmac4_generate_rl_drop_msg(num_dropped_rls,
                                    &drop_info[0]));

    /* Post event to Carrier Request function */
    hdrrmac4_carrier_req_event(HDRRMAC4_CARRIER_REQ_EVENT_PA_HEADROOM_DROP,
                               0,
                               0,
                               dropped_chan_mask); 
      /* num_chans and phy_chan_idx not used */
  }

  /* Put channels to be dropped in INACTIVE state */
  for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
  {
    if ( ( ( hdrrmac4_var_ptr->phy_chan[chan].chan_state ==
             HDRRMAC4_CHAN_SETUP_STATE ) ||
           ( hdrrmac4_var_ptr->phy_chan[chan].chan_state ==
             HDRRMAC4_CHAN_OPEN_STATE ) ) && 
         ( channel_dropped[chan] ) )
    {
      /* Put channel in INACTIVE state */
      hdrrmac4_chan_enter_inactive_state(chan);
    }
  }

  if (HDRRMAC4_CARRIER_REQ_NUM_CHANS_PA_HEADROOM_INCREASED())
  {
    /* 
     * Flag the case that we are dropping carrier and increase PA headroom at 
     * the same time.
     */
    if (num_dropped_rls != 0)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR,
        "Inconsistent add/drop: %d/%d",
                num_chans_pa_headroom,
        num_dropped_rls );
    }

    /* post event to Carrier Request function */
    hdrrmac4_carrier_req_event(HDRRMAC4_CARRIER_REQ_EVENT_PA_HEADROOM_ADD,
                               num_chans_pa_headroom,
                               0,
                               0); 
      /* phy_chan_idx and phy_chan_mask not used */
  }

  return;

} /* hdrrrmac4_run_carrier_management */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_BUILD_NEW_PACKET                                     INTERNAL

DESCRIPTION
  This function interacts with PCP and builds a new packet

DEPENDENCIES
  None.

PARAMETERS
  combo_ptr - pointer to a single combo data structure used to interface with
              PCP.

RETURN VALUE
  pkt_built - TRUE if at least one of the assigned channels has a valid data
                   ptr from PCP.
              FALSE no packet is built for all the assigned channels

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL boolean hdrrmac4_build_new_packet
(
  hdrpcp_build_data_per_combo_type  *combo_ptr
)
{
  hdrrmaccmn_payload_size_enum_type best_ps_index = HDRRMACCMN_0_BITS;
  boolean tx_mode_changed[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {FALSE}; 
                                   /* flag indicating tx_mode change from
                                    * HI_CAP to LO_LAT (used in downgrade
                                    * section)
                                    */
  hdrrmaccmn_payload_size_enum_type 
    pp_max_ps[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
  uint32 max_tx_t2p[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  uint8   chan; /* channel loop index */
  uint8   iteration;
  hdrrmac4_phy_chan_vars_struct_type  *phy_chan_ptr;
  uint8                               flow;
  uint8                               active_chan;
  boolean                             iterate = TRUE;
    /* need to run 2nd iteration? */
  uint8                               num_chans_in_caput_list;
  boolean                             pkt_built = FALSE;
    /* if any of the assigned channels has a valid data ptr from PCP */

  uint32 channel_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
    {HDRRMAC4_INVALID_CHAN_ID, 
     HDRRMAC4_INVALID_CHAN_ID, 
     HDRRMAC4_INVALID_CHAN_ID};

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  uint64                              mac_lmtd_tput; 
                                                 /* MAC limited throughput */
  uint64                              not_data_lmtd_factor;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  boolean packet_is_t2p_boosted = FALSE;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  uint16                            boosted_rri_mask;
    /* Temp variable to store HiCap boost mask */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

  uint8                             term_target = 0;
    /* term target to check against subpkt left before tuneaway */

  uint8                             subpkt_left;
    /* subpkt left before tuneaway */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* 
   * Find out from DRV the max. permitted packet size for all open channels 
   * based on the packet size transition rule.
   */
  hdrrmac4drv_get_max_permitted_pkt_size
    ((hdrrmaccmn_payload_size_enum_type *)&pp_max_ps[0]);

  /* Do carrier management */
  hdrrmac4_run_carrier_management(&max_tx_t2p[0]);

#if 0 /* TODO */
  hdrrmac4_save_mac_info();
#endif /* 0 */

  for (iteration = 0; iterate; iteration++)
  {
    hdrrmaccmn_ps_set_type  ps_set=hdrrmaccmn_univ_ps_set;

    /* Limit the packet size by MaxPacketSize attribute */
    ps_set = ps_set & (~((uint16)( 1 << hdrrmac4_config_ptr->max_packet_size)));

    /* 
     * Initialize the active MAC flow set information in the temporary working 
     * copy.
     */
    hdrrmac4_init_active_flows_set_tmp();

    for (chan = 0; chan < hdrrmac4_var_ptr->num_open_state_chans; chan++)
    {
      active_chan = hdrrmac4_ranked_rl_list[chan];
      phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[active_chan];

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
      /* Clear MAC limited RRI */
      hdrrmac4_var_ptr->phy_chan[active_chan].mac_lmtd_rri = 0;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

      /* Nominal TxT2P's for each packet size and transmission mode, and T2P
       * conversion factors for each transmission mode are derived from the
       * PowerParameters attribute. This functions expects that this data has
       * already been extracted and available to this module
       */

      /* Construct the set, F,  of active MAC flows with non-empty queues */
      hdrrmac4_build_non_empty_flows_set(active_chan);

      /* copy the remaining queue length into this channel structure */
      for (flow = 0; flow < hdrrmac4_num_non_empty_flows[active_chan]; flow++)
      {
        phy_chan_ptr->flow[hdrrmac4_non_empty_flows_set[active_chan][flow].mac_flow_id].
          queue_info.initial_size =
          hdrrmac4_non_empty_flows_set[active_chan][flow].size;
      }

      /* For this set, F, determine QOutflow and SumQOutflow = 
       * sum( HiCap flows that are members of F ) */
      hdrrmac4_calc_q_outflows(active_chan);

#ifdef FEATURE_HDR_RMAC4_DEBUG
      #error code not present
#endif

      /* Decide if this new packet will be transmitted in HiCap mode or 
       * LoLat mode */
      hdrrmac4_determine_packet_tx_mode(active_chan);

      /* Construct the set, F* (subset of F), of MAC flows that are eligible
       * for transmission in this new packet based on the transmission mode */
      hdrrmac4_build_eligible_flows_set(active_chan);

      if ((hdrrmac4_var_ptr->num_open_state_chans > 1) && 
            /* if there are more than 1 carrier assigned */
          (iteration != (HDRRMAC4_MAX_PS_ITERATIONS-1)) &&
            /* if this is not the last iteration */
          (phy_chan_ptr->pkt_tx_mode == HDRRMACCMN_TX_MODE_HI_CAP))
            /* if this channel is HiCap */
      {
        /* The first iteration limits the packet size to 8K */
        ps_set &= ~( 1 << ((uint8)HDRRMACCMN_12288_BITS - 1));
      }

      /* a. Pick a suitable Physical Layer packet size subject to constraints:
       *    PermittedPayload, PA headroom and max TXT2P 
       *
       * b. Look up packet size index to get bits
       *
       * c. convert into octets as all our calculations of flow sizes are in
       *    octets
       */
      best_ps_index = hdrrmac4_determine_packet_size(
                        active_chan,
                        ps_set,
                        pp_max_ps[active_chan],
                        max_tx_t2p[active_chan] ); 

      HDR_MSG_DEBUG_6
        (HDR_MSG_DBG_PS_SELECTION,
         "Ch %d, iter %d, ps_set %x, pp_max_ps %x, max_tx_t2p %x, best_ps_i %d",
         active_chan,
         iteration,
         ps_set,
         pp_max_ps[active_chan],
         max_tx_t2p[active_chan],
         best_ps_index);

      /* See if HiCap packet can be transmitted in LoLat mode to get a higher
       * data rate, BUT only allow this transition in the 2nd iteration.  We
       * don't need to explicitly check against the 2nd iteration, since the
       * best PS will never be 12288 bits from the first iteration.
       */
      if ( ( phy_chan_ptr->pkt_tx_mode == HDRRMACCMN_TX_MODE_HI_CAP ) &&
           ( best_ps_index == HDRRMACCMN_12288_BITS ) )
      {
        hdrrmaccmn_payload_size_enum_type ps;
        hdrrmaccmn_ps_set_type            lim_univ_ps_set = 
                                            hdrrmaccmn_null_ps_set;
    
        /* Form a subset of all allowed pkt sizes = {PayloadThresh,...,12288} */
        for( ps = hdrrmac4_config_ptr->payload_thresh; 
             ps <= HDRRMACCMN_12288_BITS; 
             ps++ )
        {
          lim_univ_ps_set |= 1 << ( (uint8)ps - 1 );
        }

        /* Try to achieve higher data rates by transmitting in LoLat mode with
         * a possibly smaller packet size
         */
        phy_chan_ptr->pkt_tx_mode = HDRRMACCMN_TX_MODE_LO_LAT; /* Change TX_MODE */
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
        boosted_rri_mask = hdrrmac4_var_ptr->boosted_rri_mask[active_chan];
          /* Below function will override hdrrmac4_var_ptr->boosted_rri_mask, so we
             save it here in case forcing LoLat gives worse RRI */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

        best_ps_index = hdrrmac4_determine_packet_size( 
                          active_chan,
                          lim_univ_ps_set,
                          pp_max_ps[active_chan],
                          max_tx_t2p[active_chan]);
    
        if(( best_ps_index >= hdrrmac4_config_ptr->payload_thresh ) && 
           ( phy_chan_ptr->pkt_tx_mode == HDRRMACCMN_TX_MODE_LO_LAT))
        {
          phy_chan_ptr->pkt_size_index = best_ps_index; /* Change best PS */
          tx_mode_changed[active_chan] = TRUE;
                                   /* indicates that tx_mode was changed
                                    * from HI_CAP to L0_LAT
                                    * (used in downgrade section)
                                    */
        }
        else
        {
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
          hdrrmac4_var_ptr->boosted_rri_mask[active_chan] = boosted_rri_mask;
            /* restore the HiCap boost mask */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
          phy_chan_ptr->pkt_size_index = HDRRMACCMN_12288_BITS;
          phy_chan_ptr->pkt_tx_mode = HDRRMACCMN_TX_MODE_HI_CAP;
        }
      }
      else
      {
        phy_chan_ptr->pkt_size_index = best_ps_index;
      }

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
      /* Decay MAC limited throughput */
      mac_lmtd_tput = ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR - 
        ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
        HDRRMACCMN_MODEMAPI_FILT_TC ) )
        * ( (uint64) hdrrmac4_var_ptr->phy_chan[active_chan].mac_lmtd_tput );

      /* Add current subframe input to filter */
      mac_lmtd_tput += ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
        HDRRMACCMN_MODEMAPI_FILT_TC ) * hdrrmaccmn_get_tput_from_rri( 
        hdrrmac4_var_ptr->phy_chan[active_chan].mac_lmtd_rri );

      /* Remove scaling factor multiplier and store the value */
      hdrrmac4_var_ptr->phy_chan[active_chan].mac_lmtd_tput = ( uint32) ( ( 
        mac_lmtd_tput + ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR >> 1 ) ) /
        HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR );

      /* Decay not-data-limited factor */
      not_data_lmtd_factor = ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR - 
        ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
        HDRRMACCMN_MODEMAPI_FILT_TC ) )
        * ( (uint64) hdrrmac4_var_ptr->phy_chan[active_chan].
        not_data_lmtd_factor );

      if ( hdrrmac4_var_ptr->phy_chan[active_chan].mac_lmtd_rri > 
        best_ps_index )
      {
        hdrrmac4_var_ptr->phy_chan[active_chan].not_data_lmtd = FALSE;
      }
      else
      {
        hdrrmac4_var_ptr->phy_chan[active_chan].not_data_lmtd = TRUE;

        /* Add unit input to filter if the current subframe is not data
           limited */
        not_data_lmtd_factor += ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
          HDRRMACCMN_MODEMAPI_FILT_TC )*(1 << 16);
      }

      /* Remove scaling factor multiplier and store the value */
      hdrrmac4_var_ptr->phy_chan[active_chan].not_data_lmtd_factor = ( uint32)
        ( ( not_data_lmtd_factor + 
        ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR >> 1 ) ) /
        HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR );

      /* Set plus flag and set its reset counter if MAC limited RRI is zero */
      if ( hdrrmac4_var_ptr->phy_chan[active_chan].mac_lmtd_rri == 0 )
      {
        hdrrmac4_var_ptr->plus_flag = TRUE;
        hdrrmac4_var_ptr->plus_flag_reset_counter = 
          HDRRMACCMN_PLUS_FLAG_RESET_CNT;
      }

#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

      /* If we are in a test mode where a fixed packet size is required then use
       * that as the best packet size, but obey the permitted payload constraint
       */
      if( (hdrrmac4_test_ps_index_fixed != HDRRMACCMN_IS856_A_RRI) &&
          (hdrrmac4_test_ps_index_fixed <= HDRRMACCMN_12288_BITS ) )
      {
        phy_chan_ptr->pkt_size_index = 
          MIN( pp_max_ps[active_chan],
               hdrrmac4_test_ps_index_fixed );
      }

      /* If the Reverse MC Test Application is running and the Configured
       * Packet Payload Size mode is enabled, then RETAP will specify a target
       * payload size (rtap_tgt_ps_index). RTCMAC is required to select the
       * MIN of this and its decision (hdrrmac4_var_ptr->pkt_size_index) as the final
       * payload size. See section 5.7.3.1.3 of IS-890A.
       */
      phy_chan_ptr->pkt_size_index = 
        MIN( phy_chan_ptr->rtap_tgt_ps_index,
             phy_chan_ptr->pkt_size_index );

      /* Default to check against LL subpkt.
         LoLat/Boosted: ll_subpkt
         HiCap/TxMode changed: hc_subpkt (set later) */
      subpkt_left = (uint8) phy_chan_ptr->ll_subpackets_available;
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
      packet_is_t2p_boosted = 
        ( ( hdrrmac4_var_ptr->boosted_rri_mask[active_chan] & 
        ( 1 << (uint8) phy_chan_ptr->pkt_size_index ) ) != 0 ) ? TRUE : FALSE;

      if ( packet_is_t2p_boosted )
      {
        /* if boosted, check against boosted term target */
        term_target = hdrrmac4_var_ptr->t2p_boost.tx_t2p_info[
          phy_chan_ptr->pkt_size_index].term_target+1;
      }
      else
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
      {
        /* non-boosted use non-boosted term target */
        term_target = hdrrmac4_config_ptr->t2p_info[phy_chan_ptr->pkt_tx_mode].
            tx_t2p_info[phy_chan_ptr->pkt_size_index].term_target+1;

        if ( ( phy_chan_ptr->pkt_tx_mode == HDRRMACCMN_TX_MODE_HI_CAP ) ||
             tx_mode_changed )
        {
          /* Nonboosted HiCap or TM has changed, check against hc_subpkt */
          subpkt_left = (uint8) phy_chan_ptr->hc_subpackets_available;
        }
      }

      /* Check term target against subpkt left and stop if too big */
      if ( term_target > subpkt_left )
      {
        HDR_MSG_PROT_6( MSG_LEGACY_HIGH, 
                        "CH %d PS to 0 due to tuneaway outage boost:%d tm:%d ps:%d tt:%d>left:%d",
                        chan,
                        packet_is_t2p_boosted, 
                        phy_chan_ptr->pkt_tx_mode, 
                        phy_chan_ptr->pkt_size_index,
                        term_target, 
                        subpkt_left );

        phy_chan_ptr->pkt_size_index = HDRRMACCMN_0_BITS;

        if ( ( phy_chan_ptr->pkt_tx_mode == HDRRMACCMN_TX_MODE_HI_CAP ) ||
             tx_mode_changed )
        {
          /* 
           * Check if outage is due to 
           * - Imminent tuneway 
           * - Imminent RLSI
           * and log appropriate outage reason.
           */
          if (phy_chan_ptr->hc_subpackets_available != 
              HDRRMAC4DRV_PA_NOT_AVAILABLE)
          {
            if (phy_chan_ptr->sp_to_suspend < (uint8) 
                HDRRMAC4DRV_PA_4_SUBPACKETS_AVAILABLE)
            {
              phy_chan_ptr->hdr_inactive_reason = 
                (hdrrmac4drv_hdr_mode_enum_type)
                ( ((uint8) phy_chan_ptr->hdr_inactive_reason) |
                ((uint8) HDRRMAC4DRV_HDR_MODE_TUNE_AWAY_IMMINENT) );
            }

            if (phy_chan_ptr->sp_to_rlsi < (uint8) 
                HDRRMAC4DRV_PA_4_SUBPACKETS_AVAILABLE)
            {
              phy_chan_ptr->hdr_inactive_reason =
                (hdrrmac4drv_hdr_mode_enum_type)
                (((uint8) phy_chan_ptr->hdr_inactive_reason) |
                ((uint8) HDRRMAC4DRV_HDR_MODE_RLSI_IMMINENT));
            }
          }
        }
        else
        {
          phy_chan_ptr->hdr_inactive_reason = 
            (hdrrmac4drv_hdr_mode_enum_type)
            (((uint8) phy_chan_ptr->hdr_inactive_reason) |
            ((uint8) HDRRMAC4DRV_HDR_MODE_TUNE_AWAY_IMMINENT));
        }
      }
      /* Per-carrier HDR_Mode_Active flag is set to FALSE for the following 
         reasons for an OPEN channel when the AT is not under tuneaway or RLSI
         - DRC supervision
         - Fwd link finger out-of-lock */
      else if (!(phy_chan_ptr->hdr_mode_active))
      {
        HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
          "Chan %d PS to 0 due to DRC supervision or finger out of lock (%d)",
          active_chan,
          phy_chan_ptr->hdr_inactive_reason );

        phy_chan_ptr->pkt_size_index = HDRRMACCMN_0_BITS;
      }

      phy_chan_ptr->max_pkt_size = 
        hdrrmaccmn_pkt_size_array[phy_chan_ptr->pkt_size_index];

      /* record the RTCMAC priority for logging purpose */
      phy_chan_ptr->rtcmac_pri = chan;
  
      if (phy_chan_ptr->max_pkt_size != 0)
      {
        hdrrmac4_allot_flow_data(active_chan);
      }
      else
      {
        hdrrmac4_allot_zero_flow_data(active_chan);
      }

      /* 
       * Update the active flows set queue information based on the allocation
       * that's done on this channel.
       */
      hdrrmac4_update_active_flows_set_tmp(active_chan);

    } /* for each channel */

    if (iteration < (HDRRMAC4_MAX_PS_ITERATIONS-1))
    {
      iterate = hdrrmac4_next_iteration_needed(&max_tx_t2p[0]);

      /* Update logging information */
      hdrrmac4_logging.two_iterations = iterate;
    }
    else
    {
      iterate = FALSE;
    }

  } /* for each iteration */

  /* Update the DRV with SumPotentialT2POutflow information */
  {
    uint32 sum_pot_t2p_outflows[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

    for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
    {
      sum_pot_t2p_outflows[chan] = 
        hdrrmac4_var_ptr->phy_chan[chan].sum_pot_t2p_outflow
          [(hdrrmac4_var_ptr->phy_chan[chan].pkt_tx_mode)];
    }

    hdrrmac4drv_update_sum_pot_t2p_outflows(&sum_pot_t2p_outflows[0]);
  }

  /* Ask drv for CAPUT priority */
  num_chans_in_caput_list = 
    hdrrmac4drv_get_caput_prioritized_carriers(&hdrrmac4_caput_rl_list[0]);
  if (num_chans_in_caput_list != hdrrmac4_var_ptr->num_open_state_chans)
  {
    /* fail the sanity check */
    ERR_FATAL("Invalid channel count, 4drv %d vs. rmac4 %d",
              num_chans_in_caput_list,
              hdrrmac4_var_ptr->num_open_state_chans,
              0);
  }

  /* Call PCP to build packets */
  combo_ptr->combo_id = HDRRMAC4_DEFAULT_COMBO_ID;
  combo_ptr->num_carriers = 0;
  for (chan = 0; chan < hdrrmac4_var_ptr->num_open_state_chans; chan++)
  {
    active_chan = hdrrmac4_caput_rl_list[chan];
    phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[active_chan];

    /* record the CAPUT priority for logging purpose */
    phy_chan_ptr->caput_pri = chan;

    /* clear the logging information */
    hdrrmac4_logging.num_pkts_in_mac_pkt[active_chan] = 0;
    hdrrmac4_logging.num_mac_flows_with_data[active_chan] = 0;

    if (phy_chan_ptr->max_pkt_size != 0)
    {
      combo_ptr->carrier_data[combo_ptr->num_carriers].carrier_id = 
        active_chan;
      combo_ptr->carrier_data[combo_ptr->num_carriers].
        recommended_info.num_mac_flows =
          hdrrmac4_num_elig_flows[active_chan];
      combo_ptr->carrier_data[combo_ptr->num_carriers].
        recommended_info.mac_flows =
          &hdrrmac4_elig_flows_set[active_chan][0];
      combo_ptr->carrier_data[combo_ptr->num_carriers].
        recommended_info.max_pkt_size =
          phy_chan_ptr->max_pkt_size;

      /* Record the current channel in the channel list */
      channel_list[combo_ptr->num_carriers] = active_chan;

      combo_ptr->num_carriers++;
    }
    else
    {
      phy_chan_ptr->actual_pkt_size = 0;

      /* 
       * No data is to be allocated for this channel.  Log the 0 packet size 
       * to prevent the stale packet size from the previous subframe being
       * logged.
       */
      hdrrmac4_logging.ps_info[active_chan].final_ps_index = 0;
      hdrrmac4_logging.ps_info[active_chan].tm = (uint8) 
        HDRRMACCMN_TX_MODE_HI_CAP;

      /* Clear actual allocation */
      for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++)
      {
        hdrrmac4_per_nc_vars_struct_type *flow_ptr;

        flow_ptr = &( phy_chan_ptr->flow[flow] );

        flow_ptr->queue_info.actual_size = 0;

      }
    }
  }

  HDR_MSG_DEBUG_4( HDR_MSG_DBG_PKT_STATUS,
                   "Get pkt from PCP num_chans %d, chan_list %d %d %d",
                   combo_ptr->num_carriers,
                   channel_list[0],
                   channel_list[1],
                   channel_list[2] );

  if (combo_ptr->num_carriers != 0)
  {
#ifndef FEATURE_HDR_RMAC4_UNIT_TEST
    hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_GET_PKT_START);

    /* Call PCP to get the packets */
    (void) hdrpcp_get_packet( combo_ptr, HDRRMAC4_DEFAULT_NUM_COMBOS, 
      rmac4_pkt_status.subframe_n );

    hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_GET_PKT_END);

#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */

    for (chan = 0; chan < combo_ptr->num_carriers; chan++)
    {
      /* store the information returned from PCP */
      active_chan = combo_ptr->carrier_data[chan].carrier_id;
      phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[active_chan];

#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
      #error code not present
#else /* FEATURE_HDR_RMAC4_UNIT_TEST */
      phy_chan_ptr->actual_pkt_size =
        combo_ptr->carrier_data[chan].built_data.actual_pkt_size;
      if (combo_ptr->carrier_data[chan].built_data.pkt_format == HDRPCP_PACKET_FORMAT_B)
      {
        phy_chan_ptr->pkt_info.format = HDRRMAC4DRV_FORMAT_B_PACKET;
      }
      else
      {
        phy_chan_ptr->pkt_info.format = HDRRMAC4DRV_FORMAT_A_PACKET;
      }
      phy_chan_ptr->pkt_info.handle   = 
        combo_ptr->carrier_data[chan].built_data.pkt_handle;
      phy_chan_ptr->pkt_info.pkt_ptr  = 
        combo_ptr->carrier_data[chan].built_data.tx_pkt_ptr;

      if (phy_chan_ptr->pkt_info.pkt_ptr != NULL)
      {
        pkt_built = TRUE;
      }

      /* Checking if packet-size downgrade is required and
       * setting appropriate pkt_size_index
       */
      if(phy_chan_ptr->actual_pkt_size == 0)  /* no downgrade required */
      {
        phy_chan_ptr->pkt_size_index = HDRRMACCMN_0_BITS;
      }
      else
      {
        hdrrmac4_downgrade_packet_size(active_chan,
                                       tx_mode_changed[active_chan]);
      }

      hdrrmac4_logging.ps_info[active_chan].final_ps_index = (uint8)
        phy_chan_ptr->pkt_size_index;
      hdrrmac4_logging.ps_info[active_chan].tm = (uint8)
        phy_chan_ptr->pkt_tx_mode;
      hdrrmac4_logging.num_pkts_in_mac_pkt[active_chan] =
        combo_ptr->carrier_data[chan].built_data.num_pkts;
      hdrrmac4_logging.num_mac_flows_with_data[active_chan] =
        combo_ptr->carrier_data[chan].built_data.num_mac_flows_with_data;
        /* Save this for logging  */
#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */

      HDR_MSG_DEBUG_5( HDR_MSG_DBG_RTCMAC_DECISIONS, 
                       "Chan %d Best PS: %d bytes TM: %d Actual PS: %d Fmt: %c", 
                       active_chan,
                       hdrrmaccmn_pkt_size_array[phy_chan_ptr->pkt_size_index] + 4, 
                       phy_chan_ptr->pkt_tx_mode,
                       phy_chan_ptr->actual_pkt_size,
                       ('A' + (uint8) phy_chan_ptr->pkt_info.format) );

      /* copy d_i,r (acutal flow sizes) into flow data structure */
      for (flow = 0; flow < hdrrmac4_num_non_empty_flows[active_chan]; flow++)
      {
        hdrrmac4_per_nc_vars_struct_type *flow_ptr;

        flow_ptr = 
          &( phy_chan_ptr->flow[hdrrmac4_non_empty_flows_set[active_chan][flow].mac_flow_id] );

        flow_ptr->queue_info.actual_size = 
          hdrrmac4_elig_flows_set[active_chan][flow].size;

        HDR_MSG_DEBUG_7( HDR_MSG_DBG_RTCMAC_DECISIONS,
                         "f%d: Q_%d: %d d'_%d: %d d_%d: %d",
                         hdrrmac4_elig_flows_set[active_chan][flow].mac_flow_id, 
                         hdrrmac4_elig_flows_set[active_chan][flow].mac_flow_id,
                         flow_ptr->queue_info.initial_size,
                         hdrrmac4_elig_flows_set[active_chan][flow].mac_flow_id,
                         flow_ptr->queue_info.suggested_size,
                         hdrrmac4_elig_flows_set[active_chan][flow].mac_flow_id,
                         flow_ptr->queue_info.actual_size);
      }
    }
  }
  else
  {
    /* clear the logging buffer */
    for (chan = 0; chan < hdrrmac4_var_ptr->num_open_state_chans; chan++)
    {
      active_chan = hdrrmac4_open_rl_list[chan];

      hdrrmac4_logging.ps_info[active_chan].final_ps_index = 0;
      hdrrmac4_logging.ps_info[active_chan].tm = (uint8) 
        HDRRMACCMN_TX_MODE_HI_CAP;
    }

    /* Set all d'_i = 0 */
    hdrrmac4_reset_active_flows();

    HDR_MSG_DEBUG( HDR_MSG_DBG_RTCMAC_DECISIONS,
                   "No carrier assigned in combo" );
  }

  return pkt_built;

} /* hdrrmac4_build_new_packet */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_INIT_ACTIVE_FLOWS                                   INTERNAL

DESCRIPTION
  This function inits the queue sizes and MAC id's of active flows

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL uint8 hdrrmac4_init_active_flows( void )
{
  uint8 flow, active_flow_cnt = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++)
  {
    if( ( HDRRMAC4_IS_FLOW_ACTIVE( flow ) ) == TRUE )
    {
      hdrrmac4_active_flows_set[active_flow_cnt].mac_flow_id = flow;
      active_flow_cnt++;
    }
    else
    {
       hdrrmac4_active_flows_set[flow].mac_flow_id = 
         HDRRMACCMN_MAC_FLOW_ID_INVALID;
    }

    hdrrmac4_active_flows_set[flow].size = 0;
  }

  return active_flow_cnt;
} /* hdrrmac4_init_active_flows */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_INIT_QUEUE_INFO                                     INTERNAL

DESCRIPTION
  This function resets the queue sizes before querying PCP every subframe 

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_init_queue_info( uint8 phy_chan_idx )
{
  uint8 flow;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++ )
  {
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].flow[flow].queue_info.
      initial_size = 0;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].flow[flow].queue_info.
      suggested_size = 0;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].flow[flow].queue_info.
      actual_size = 0;
  }

  return;

} /* hdrrmac4_init_queue_info */


 /* EJECT */
/*===========================================================================
FUNCTION hdrrmac4_save_bucket_state                                  INTERNAL

DESCRIPTION
  This function saves a copy of current bucket variables in preparation for the
  next subframe.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_save_bucket_state( void )
{
  uint8 flow;
  uint8 chan_index;
  hdrrmac4_phy_chan_vars_struct_type *phy_chan_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for (chan_index = 0; 
       chan_index < hdrrmac4_var_ptr->num_open_state_chans;
       chan_index++)
  {
    uint8 chan = hdrrmac4_open_rl_list[chan_index];

    phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];

    for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++ )
    {
      phy_chan_ptr->flow[flow].bucket.bucket_level.prev =
        phy_chan_ptr->flow[flow].bucket.bucket_level.curr;

      phy_chan_ptr->flow[flow].bucket.t2p_inflow.prev =
        phy_chan_ptr->flow[flow].bucket.t2p_inflow.curr;

      phy_chan_ptr->flow[flow].bucket.t2p_outflow.prev =
        phy_chan_ptr->flow[flow].bucket.t2p_outflow.curr;

      phy_chan_ptr->flow[flow].bucket.tt2p_hold.prev =
        phy_chan_ptr->flow[flow].bucket.tt2p_hold.curr;

    } /* for each flow */
  } /* for each channel */

  for (flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++)
  {
    /* last_bucket_level_max is used to know when a flow becomes
     * reactivated. When this happens the flow is reinitialized in
     * hdrrmac4_init_bucket()
     */
    hdrrmac4_var_ptr->data_flow[flow].bucket.last_bucket_level_max =
      hdrrmac4_config_ptr->flow[flow].bucket_level_max;

    hdrrmac4_var_ptr->data_flow[flow].data_token.data_bucket_level.prev =
      hdrrmac4_var_ptr->data_flow[flow].data_token.data_bucket_level.curr;
  } /* for each flow */

  return;

} /* hdrrmac4_save_bucket_state */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_TCA_UPDATE                                        INTERNAL

DESCRIPTION
  This function updates the channel states and dropping rank according to the
  most recent TCA.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_tca_update(void)
{
  uint8 chan;
    /* channel loop index */

  hdrrmac4_phy_chan_vars_struct_type  *phy_chan_ptr;
    /* convenience pointer */

  uint16  chan_mask;
    /* channel bit mask to send to rmac4drv */

  boolean update_rmac4drv = FALSE;
    /* need to update rmac4drv with dropping rank information */

  uint8 dropping_rank[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* to update rmac4drv */

  uint8 num_chans_dropped = 0;
    /* Number of channel to be dropped */

  boolean new_connection_setup = FALSE;
    /* True if new connection setup procedure needs to be followed for
       turning on RL carriers */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* find out all the channels to be deleted */
  chan_mask = 0;
  for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
  {
    phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];

    if (HDRRMAC4_IS_RL_TO_BE_DELETED( chan ))
    {
      chan_mask |= (0x1 << chan);

      num_chans_dropped++;

      /* Find out if this TCA is going to drop the current SLP */
      if (chan == hdrrmac4_var_ptr->slp_carrier_idx)
      {
        /* Has to force a SLP switch regardless if flow 0 has data */
        hdrrmac4_force_slp_switch = TRUE;
      }
    }

    /* put the channel in the inactive state */
    if (phy_chan_ptr->chan_state == HDRRMAC4_CHAN_DELETE_STATE)
    {
      hdrrmac4_chan_enter_inactive_state(chan);
    }
  }

  if (chan_mask != 0)
  {
    /* Tell rmac4drv to stop those channels */
    (void) hdrrmac4drv_stop_channels(chan_mask);

    /* Post event to Carrier Request function */
    hdrrmac4_carrier_req_event(HDRRMAC4_CARRIER_REQ_EVENT_TCA_DROP,
                               num_chans_dropped,
                               0,
                               0); 
      /* phy_chan_idx and phy_chan_mask not used */
  }

  if (hdrrmac4_carrier_req_var.num_chans_allocated == 0)
  {
    /* Starting a connection with no allocated carriers
       => Set new connection setup flag */
    new_connection_setup = TRUE;
  }

  /* find out all the channels to be added */
  chan_mask = 0;
  for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
  {
    phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];

    if ((phy_chan_ptr->chan_state == HDRRMAC4_CHAN_ADD_STATE) ||
        (phy_chan_ptr->chan_state == HDRRMAC4_CHAN_ADD_DELETE_STATE))
    {
      chan_mask |= (0x1 << chan);

      /* update rmac4drv with channel information */
      hdrrmac4drv_update_rl_info(
        chan,
        &phy_chan_ptr->chan_record,
        &phy_chan_ptr->subaset_info, 
        phy_chan_ptr->dropping_rank.curr );

      /* put the channel in SETUP state */
      hdrrmac4_chan_enter_setup_state(chan);
    }
  }

#ifdef FEATURE_FACTORY_TESTMODE
  /* Channels are started in rmac4drv as a part of enter_setup_state()
     processing in FTM mode => Skip re-starting the channels. */
  if (!( ftm_get_mode() == FTM_MODE ))
#endif
  {
    if (chan_mask != 0)
    {
      /* tell rmac4drv to start those channels */
      (void) hdrrmac4drv_start_channels (chan_mask, new_connection_setup);
    }
  }

  /* scan through the dropping rank and update rmac4drv if necessary */
  for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
  {
    phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];

    if (HDRRMAC4_IS_RL_OPEN(chan))
    {
      if (phy_chan_ptr->dropping_rank.prev != phy_chan_ptr->dropping_rank.curr)
      {
        phy_chan_ptr->dropping_rank.prev = phy_chan_ptr->dropping_rank.curr;

        update_rmac4drv = TRUE;
      }
      dropping_rank[chan] = phy_chan_ptr->dropping_rank.prev;
    }
    else
    {
      dropping_rank[chan] = HDRRMAC4DRV_INVALID_DROP_RANK;
    }
  }

  if (update_rmac4drv)
  {
    hdrrmac4drv_update_drop_ranks(&dropping_rank[0]);
  }

  if (hdrrmac4_var_ptr->tca_processed_cb_ptr != NULL)
  {
    /* Call TCA update done callback */
    hdrrmac4_var_ptr->tca_processed_cb_ptr();

    /* Reset callback pointer */
    hdrrmac4_var_ptr->tca_processed_cb_ptr = NULL;
  }

  return;

} /* hdrrmac4_tca_update */


#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
 #error code not present
#else


 /* EJECT */
/*===========================================================================
FUNCTION hdrrmac4_process_subframe                                   INTERNAL

DESCRIPTION
  This is the main routine of the RTCMAC algorithm and it is triggered by the 
  subframe Tx interrupt every 4 slots. Runs in Tx task context.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_process_subframe( void )
{
  uint8                             flow;
  uint8                             phy_chan_idx, combo_ch_idx;
    /* Physical channel loop index */
  uint16                            load_tx_pkt_id;
                                    /* The pkt_id's returned by lower layer
                                     * for the packet currently being sent and
                                     * the one just loaded, respectively
                                     */
  static hdrpcp_prev_subframe_combo_info_type prev_sf_combo_info;
    /* structure to inform PCP about previous TX status */

  static hdrrmac4drv_pkt_enc_info_struct_type 
    pkt_enc[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* packet encoding structure used with hdrrmac4drv_send_packet() interface */

  hdrrmac4_phy_chan_vars_struct_type  *phy_chan_ptr;
    /* convenience pointer */

  uint16  marq_nak_id;
    /* MARQ Packet ID */

  uint32  channel_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
    {HDRRMAC4_INVALID_CHAN_ID, 
     HDRRMAC4_INVALID_CHAN_ID, 
     HDRRMAC4_INVALID_CHAN_ID};

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  boolean stop_t2p_boost = FALSE;
    /* Set to TRUE if T2P boost reset conditions are satisfied */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  uint64 tx_on_factor;
    /* Intermediate variable for calculating TxOnFactor */
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Record current time for profiling */
  hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_FRAME_BUILD_START);

  /* Get previous packet status.  If return value is zero, an error
     has occurred causing subframe processing to run twice in one subframe.
     RTC MAC should abort processing this entire subframe immediately. */
  if (!hdrrmac4drv_get_prev_tx_status( 
        &hdrrmac4_var_ptr->enc_window_missed, 
        &rmac4_pkt_status ))
  {
    return;
  }

  /* Check if we need to perform a carrier drop due to pilot power 
     differential */
  if (hdrrmac4drv_is_power_diff_carrier_drop(&phy_chan_idx))
  {
    /* Drop phy_chan_idx */
    hdrrmac4_process_transmit_power_diff(phy_chan_idx);
  }

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  for (phy_chan_idx = 0;
       phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
       phy_chan_idx++)
  {
    /* Reset boosted RRI mask */
    hdrrmac4_var_ptr->boosted_rri_mask[phy_chan_idx] = 0;
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  if ( hdrrmac4_var_ptr->plus_flag_reset_counter > 0 )
  {
    /* Decrement plus flag reset counter and reset the flag if the
       counter becomes zero */
    hdrrmac4_var_ptr->plus_flag_reset_counter--;

    if ( hdrrmac4_var_ptr->plus_flag_reset_counter == 0 )
    {
      hdrrmac4_var_ptr->plus_flag = FALSE;
    }
  }
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

  /*
   * First scan the information returned from RMAC4DRV for MARQ information
   */
  for (phy_chan_idx = 0;
       phy_chan_idx < rmac4_pkt_status.marq_status.num_chans;
       phy_chan_idx++)
  {
    uint8   active_chan;

    active_chan = rmac4_pkt_status.marq_status.c[phy_chan_idx].phy_chan_idx;

    /* Inform PCP if there was a packet failure indication (P-ARQ Nak) */
    marq_nak_id = rmac4_pkt_status.marq_status.c[phy_chan_idx].marq_nak_id;
    if (marq_nak_id != HDRRMAC4DRV_NO_MARQ_NAK)
    {
      /* TODO: Temporary change to work-around incorrect MARQ reporting */
      if ((hdrrmac4_hist[active_chan][marq_nak_id].pkt_size_index != 
           HDRRMACCMN_0_BITS) &&
          (hdrrmac4_hist[active_chan][marq_nak_id].pkt_handle != 0))
      {
        hdrpcp_packet_failed(hdrrmac4_hist[active_chan][marq_nak_id].pkt_handle);

        HDR_MSG_DEBUG_3( HDR_MSG_DBG_PKT_STATUS, 
                         "Packet Failed ch: %d, marq_nak_id: %d, pkt_handle: %d",
                         active_chan,
                         marq_nak_id,
                         hdrrmac4_hist[active_chan][marq_nak_id].pkt_handle );
      }
      else
      {
        HDR_MSG_PROT_4( MSG_LEGACY_FATAL,
          "Inconsistent MARQ report ch: %d, ps: %d, pkt_handle %d, marq_nak_id %d",
                  active_chan,
                  hdrrmac4_hist[active_chan][marq_nak_id].pkt_size_index,
          hdrrmac4_hist[active_chan][marq_nak_id].pkt_handle,
          marq_nak_id );
      }
    }
  }

  prev_sf_combo_info.combo_id = HDRRMAC4_DEFAULT_COMBO_ID;

  /* Clear index of channels to be reported in the combo status. */
  combo_ch_idx = 0;

  /* Reset current tx pkt id and txt2p on all channels.  This makes sure they
     are cleared on channels not transmitted in previous subframe */
  for (phy_chan_idx = 0;
       phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
       phy_chan_idx++)
  {
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].curr_tx_pkt_id = 
      HDRRMAC4DRV_IDLE_PACKET;

    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].tx_t2p = 0;
  }

  /*
   * Second pass to scan the information returned from RMAC4DRV for packet
   * accept/cancel information.
   */
  for (phy_chan_idx = 0;
       phy_chan_idx < rmac4_pkt_status.prev_pkt_status.num_chans;
       phy_chan_idx++)
  {
    uint8   active_chan;
    uint16  curr_tx_pkt_id;

    active_chan = rmac4_pkt_status.prev_pkt_status.c[phy_chan_idx].phy_chan_idx;

    /* Store the pkt_id of the packet currently being transmitted */
    hdrrmac4_var_ptr->phy_chan[active_chan].curr_tx_pkt_id = 
      rmac4_pkt_status.prev_pkt_status.c[phy_chan_idx].prev_pkt_id;

    hdrrmac4_var_ptr->phy_chan[active_chan].tx_t2p = 
      rmac4_pkt_status.prev_pkt_status.c[phy_chan_idx].tx_t2p;

    /* 
     * Fill in the previous combo information status.  Combo information 
     * status is primarily used for Multi-carrier with CAPUT multiple packet
     * pre-building assumptions.  In current RMAC4, only one combo is 
     * supported.
     */
    prev_sf_combo_info.carrier_info[combo_ch_idx].carrier_id = 
      active_chan;

    curr_tx_pkt_id = hdrrmac4_var_ptr->phy_chan[active_chan].curr_tx_pkt_id;

    if (curr_tx_pkt_id != HDRRMAC4DRV_IDLE_PACKET)
    {
      if (rmac4_pkt_status.prev_pkt_status.c[phy_chan_idx].prev_pkt_accepted)
      {
        prev_sf_combo_info.carrier_info[combo_ch_idx].pkt_accepted = 
          E_SUCCESS;

        prev_sf_combo_info.carrier_info[combo_ch_idx].rri = (uint8)
          hdrrmac4_hist[active_chan][curr_tx_pkt_id].pkt_size_index;

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
        /* Set flag to stop post-tuneback T2P boost if RRI 10 or higher has 
           been accepted for transmission */
        if ( ( hdrrmac4_var_ptr->t2p_boost.state == 
          HDRRMACCMN_T2P_BOOST_POST_TB_ACTIVE )
          && ( hdrrmac4_hist[active_chan][curr_tx_pkt_id].pkt_size_index >= 
          HDRRMACCMN_6144_BITS ) )
        {
          stop_t2p_boost = TRUE;
        }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

      }
      else
      {
        prev_sf_combo_info.carrier_info[combo_ch_idx].pkt_accepted = 
          E_CANCELED;

        prev_sf_combo_info.carrier_info[combo_ch_idx].rri =
          HDRPCP_RRI_CANCELED;
      }

      /* Treat the packet loaded in the previous subframe as canceled is all
       * the ongoing transmissions on the channel are to be MARQed.
       */
      if (HDRRMAC4_IS_REPORT_MARQ( active_chan ))
      {
        prev_sf_combo_info.carrier_info[combo_ch_idx].pkt_accepted = 
          E_CANCELED;

        prev_sf_combo_info.carrier_info[combo_ch_idx].rri =
          HDRPCP_RRI_CANCELED;
      }

      /* Lookup history buffer using tx_pkt_id as index to get handle */
      prev_sf_combo_info.carrier_info[combo_ch_idx].pkt_handle =
        hdrrmac4_hist[active_chan][curr_tx_pkt_id].pkt_handle;

    }
    else
    {
      prev_sf_combo_info.carrier_info[combo_ch_idx].pkt_accepted = 
        E_CANCELED;
      prev_sf_combo_info.carrier_info[combo_ch_idx].pkt_handle = 
        0;
      prev_sf_combo_info.carrier_info[combo_ch_idx].rri = 
        0;

      HDR_MSG_DEBUG_1( HDR_MSG_DBG_PKT_STATUS, 
                       "Idle packet ch: %d",
                       phy_chan_idx);
    }

    /* save for logging */
    if (prev_sf_combo_info.carrier_info[combo_ch_idx].
          pkt_accepted == E_SUCCESS)
    {
      hdrrmac4_logging.prev_pkt_accepted[active_chan] = TRUE;
    }
    else
    {
      hdrrmac4_logging.prev_pkt_accepted[active_chan] = FALSE;
    }

    /* Record the current channel in the channel list to be printed in F3 */
    channel_list[combo_ch_idx] = active_chan;

    /* Increment index */
    combo_ch_idx++;

  }

  /* Write channel count in the combo status */
  prev_sf_combo_info.num_carriers = 
    rmac4_pkt_status.prev_pkt_status.num_chans_prev;

  HDR_MSG_DEBUG_5( HDR_MSG_DBG_PKT_STATUS,
                   "SF: %d, Report to PCP num_chans %d, chan_list %d %d %d",
                   rmac4_pkt_status.subframe_n,
                   prev_sf_combo_info.num_carriers,
                   channel_list[0],
                   channel_list[1],
                   channel_list[2] );

  hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_RPT_PKT_STATUS_START);

  /* report previous packet status to PCP */
  if (prev_sf_combo_info.num_carriers != 0)
  {
    hdrpcp_report_prev_subframe_status(&prev_sf_combo_info);
  }

  /* Check if outstanding packets on any channel are to be MARQed.
   * This holds true when the AT -
   * - just completed a HHO and there are channels to be deleted OR
   * - dropped a channel due to transmit pilot power differential
   */
  for (phy_chan_idx = 0;
       phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
       phy_chan_idx++)
  {
    uint8 pkt_id[HDRSCMRMACCMN_MAX_NUM_INTERLACES];
      /* Packet ID per interlace */

    if (HDRRMAC4_IS_REPORT_MARQ( phy_chan_idx ))
    {
      uint8 interlace;
        /* Interlace index */

      /* Retrieve the outstanding packet ID's from RMAC4DRV */
      hdrrmac4drv_get_outstanding_packet_ids(phy_chan_idx, &pkt_id[0]);

      for (interlace = 0;
           interlace < HDRSCMRMACCMN_MAX_NUM_INTERLACES;
           interlace++)
      {
        marq_nak_id = pkt_id[interlace];

        if (marq_nak_id != HDRRMAC4DRV_IDLE_PACKET)
        {
          /* TODO: Temporary change to work-around incorrect MARQ reporting */
          if ((hdrrmac4_hist[phy_chan_idx][marq_nak_id].pkt_size_index != 
               HDRRMACCMN_0_BITS) &&
              (hdrrmac4_hist[phy_chan_idx][marq_nak_id].pkt_handle != 0))
          {
            /* Inform PCP if there was a packet failure indication (P-ARQ Nak) */
            hdrpcp_packet_failed(hdrrmac4_hist[phy_chan_idx][marq_nak_id].
                                 pkt_handle);

            HDR_MSG_DEBUG_4( HDR_MSG_DBG_PKT_STATUS, 
                             "RL Drop Packet Failed ch: %d, i: %d, marq_nak_id: %d, pkt_handle: %d",
                             phy_chan_idx,
                             interlace,
                             marq_nak_id,
                             hdrrmac4_hist[phy_chan_idx][marq_nak_id].pkt_handle );
          }
          else
          {
            HDR_MSG_PROT_5( MSG_LEGACY_FATAL,
              "Inconsistent MARQ report ch: %d, ps: %d, pkt_handle %d, i: %d, marq_nak_id %d",
                      phy_chan_idx,
                      hdrrmac4_hist[phy_chan_idx][marq_nak_id].pkt_size_index,
              hdrrmac4_hist[phy_chan_idx][marq_nak_id].pkt_handle,
                      interlace,
              marq_nak_id );
          }
        } /* not idle packet */
      } /* for each interlace */

      /* Reset the flag to MARQ outstanding packets */
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].marq_outstanding_pkts = FALSE;

    } /* if a channel has report-MARQ flag set */
  } /* for each RL */

  hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_RPT_PKT_STATUS_END);

  /* update the TCA information */
  hdrrmac4_tca_update();

  /* Determine HDR mode */
  hdrrmac4_hdr_mode_active = hdrrmac4_determine_hdr_mode();

  /* Get subframe number from driver layer for use in log packets */
  hdrrmac4_logging.sub_frame_count = rmac4_pkt_status.subframe_n;

  for (phy_chan_idx = 0; 
       phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
       phy_chan_idx++)
  {
    /* No round down necessary; set this field to 0 in log packet */ 
    hdrrmac4_logging.round_down_quantum[phy_chan_idx] = 0;

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
    /* Decay TxOnFactor */
    tx_on_factor = ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR - 
      ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
      HDRRMACCMN_MODEMAPI_FILT_TC ) ) * 
      ( (uint64) hdrrmac4_var_ptr->phy_chan[phy_chan_idx].tx_on_factor );
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

    if (hdrrmac4_var_ptr->phy_chan[phy_chan_idx].hdr_mode_active)
    {
      /* Init. queue info for each OPEN channel */
      hdrrmac4_init_queue_info(phy_chan_idx);

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
      /* AT is going to load a tx packet on this carrier in this subframe
         => Add unit input (in Q8 units) to TxOnFilter value */
      tx_on_factor += ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
        HDRRMACCMN_MODEMAPI_FILT_TC ) * ( 1 << 16 );
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

    }

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
    /* Remove scaling factor multiplier in TxOnFactor and store the value */
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].tx_on_factor = ( uint32) ( ( 
      tx_on_factor + ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR >> 1) ) / 
      HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR );
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

  } /* for each physical channel */

  /* initialze the list of open channels to be processed in this subframe */
  hdrrmac4_init_open_chan_list(TRUE);

  if(hdrrmac4_hdr_mode_active)
  {
    /* Populate set of active mac_flows with MAC flow IDs */
    hdrrmac4_num_active_flows = hdrrmac4_init_active_flows();

    hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_GET_PKT_INFO_START);

    hdrpcp_get_packet_info( hdrrmac4_active_flows_set,
                            hdrrmac4_num_active_flows); 

    hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_GET_PKT_INFO_END);

    /* Copy Q_i (initial flow sizes) into flow data structure */
    for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
    {
      uint8 mac_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

      hdrrmac4_flow_vars_struct_type *flow_ptr = 
        &( hdrrmac4_var_ptr->data_flow[mac_flow_id] );

      flow_ptr->queue_info.initial_size = hdrrmac4_active_flows_set[flow].size;

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
      if ( mac_flow_id != 0 )
      {
        hdrrmac4_var_ptr->rlp_q_size += hdrrmac4_active_flows_set[flow].size;
          /* Calculater RLP queue size by summing up queue sizes of
             non-signalig flows */
      }
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */
    }

#ifdef FEATURE_HDR_RMAC4_DEBUG
    #error code not present
#endif

  } /* hdr_mode_active is TRUE */
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  else
  {
    /* Populate set of active mac_flows with MAC flow IDs */
    hdrrmac4_num_active_flows = hdrrmac4_init_active_flows();

    /* Obtain per-flow queue sizes from PCP */
    hdrpcp_get_packet_info( hdrrmac4_active_flows_set,
                            hdrrmac4_num_active_flows); 

    for( flow = 0; flow < hdrrmac4_num_active_flows; flow++ )
    {
      uint8 mac_flow_id = hdrrmac4_active_flows_set[flow].mac_flow_id;

      if ( mac_flow_id != 0 )
      {
        hdrrmac4_var_ptr->rlp_q_size += hdrrmac4_active_flows_set[flow].size;
          /* Calculater RLP queue size by summing up queue sizes of
             non-signalig flows */
      }
    }

  } /* hdr_mode_active is FALSE */

#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

  /* Update QRAB for each MAC flow based on QRAB, FRAB QRAB_ps and
   * PilotStrength read from lower layer 
   */
  hdrrmac4_update_flow_qrab();
  
  /* Initialize bucket parameters for newly activated MAC flows */
  hdrrmac4_init_bucket();

  /* Update bucket level as follows:
   * current bucket level ~= previous bucket level 
   *                         + inflow from previous subframe
   *                         - outflow from current subframe
   * 
   */
  hdrrmac4_update_bucket_level();

  /* Update DataBucketLevel as follows:
   * current DataBucketLevel = DataTokenInflow 
   *                            + previous DataBucketLevel
   *                            - SumTransmittedData from previous subframe
   * AND
   * current DataBucketLevel cannot exceed DataBucketLevelMax */
  hdrrmac4_update_data_token_bucket_level();

  /* Calculate Incremental T2PInflow, and from that update T2PInflow for each
   * MAC flow
   */
  hdrrmac4_update_bucket_inflow();

  /* Calculate PotentialDataBucketOutflow */
  hdrrmac4_calc_pot_data_bucket_outflow();

#ifdef FEATURE_HDR_RMAC4_DEBUG
  #error code not present
#endif

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  /* T2P boost state change should happen between obtaining of previous
     packet status and formation of new packet.  This ensures the same boost
     state between packet formation and its consumed T2P reporting in
     previous packet status */
  if ( hdrrmac4_var_ptr->t2p_boost.reset_counter > 0 )
  {
    hdrrmac4_var_ptr->t2p_boost.reset_counter--;

    /* Set flag to stop T2P boost if reset counter goes to zero */
    if ( hdrrmac4_var_ptr->t2p_boost.reset_counter == 0 )
    {
      stop_t2p_boost = TRUE;
    }
  }

  if ( stop_t2p_boost )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "T2P boost %d => stopped", 
                    hdrrmac4_var_ptr->t2p_boost.state );

    /* Stop T2P boost from being applied to new packets and reset the limited flag */
    hdrrmac4_var_ptr->t2p_boost.state = HDRRMACCMN_T2P_BOOST_INACTIVE;
    hdrrmac4_var_ptr->t2p_boost.frab_limited[0] = FALSE;
    hdrrmac4_var_ptr->t2p_boost.frab_limited[1] = FALSE;
    hdrrmac4_var_ptr->t2p_boost.frab_limited[2] = FALSE;
    hdrrmac4_var_ptr->t2p_boost.txpwr_limited = FALSE;

    /* Reset counter */
    hdrrmac4_var_ptr->t2p_boost.reset_counter = 0;

  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */


  if( hdrrmac4_hdr_mode_active == TRUE )
  {
    uint8 chan;
    boolean enc_window_missed;

    /* Calculate nominal TxT2Ps, compute set of eligible MAC flows, determine 
     * PacketSize and transmission mode and size of each MAC flow in this 
     * packet. Interact with PCP to build the actual packet
     */
    if (hdrrmac4_build_new_packet(&hdrrmac4_combo))
    {
      uint16 sum_payload;
      uint8 num_carriers=0;

      for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
      {
        channel_list[chan] = HDRRMAC4_INVALID_CHAN_ID;
      }
      
      /* added the following assert statement, to fix the klocwork issue*/
      ASSERT(hdrrmac4_combo.num_carriers <= HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS);
    
      for (chan = 0; chan < hdrrmac4_combo.num_carriers; chan++)
      {
        phy_chan_idx = hdrrmac4_combo.carrier_data[chan].carrier_id;
        phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[phy_chan_idx];

        if (phy_chan_ptr->actual_pkt_size != 0)
        {
          /* Only include the channel that has packet built */

          pkt_enc[num_carriers].phy_chan_idx = phy_chan_idx;
          pkt_enc[num_carriers].data_length = phy_chan_ptr->actual_pkt_size;
          pkt_enc[num_carriers].pkt_ptr = phy_chan_ptr->pkt_info.pkt_ptr;
          pkt_enc[num_carriers].conn_format = phy_chan_ptr->pkt_info.format;
          pkt_enc[num_carriers].pkt_size_index = (uint8) 
            phy_chan_ptr->pkt_size_index;
          pkt_enc[num_carriers].transmission_mode = 
            (hdrrmaccmn_tx_mode_enum_type) phy_chan_ptr->pkt_tx_mode;
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
          pkt_enc[chan].not_data_lmtd = phy_chan_ptr->not_data_lmtd;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
          pkt_enc[num_carriers].packet_is_t2p_boosted = 
            ( ( hdrrmac4_var_ptr->boosted_rri_mask[phy_chan_idx] & 
            ( 1 << (uint8) phy_chan_ptr->pkt_size_index ) ) != 0 ) ? TRUE : FALSE;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

          /* Record the channel in the channel list */
          channel_list[num_carriers] = phy_chan_idx;

          /* increment the channel count */
          num_carriers++;
        }
        else
        {
          HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Ch %d empty packet",
                    phy_chan_idx);
        }
      }

      hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_FRAME_LOAD_START);

      HDR_MSG_DEBUG_4( HDR_MSG_DBG_PKT_STATUS,
                       "Send pkt to DRV num_chans %d, chan_list %d %d %d",
                       num_carriers,
                       channel_list[0],
                       channel_list[1],
                       channel_list[2] );

      /* Send the packet; lower layer takes care of updating PktTxT2P and
       * RRIChannelGain
       */
      load_tx_pkt_id = hdrrmac4drv_send_packet(
                        &enc_window_missed,
                        num_carriers,
                        &pkt_enc[0]
#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
                        ,hdrrmac4_var_ptr->rtt_reduct_opt_enabled
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
                        );

      hdrrmaccmn_ts_get_time(HDRRMACCMN_TS_FRAME_LOAD_END);

      for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
      {
        /* Clear history buffer entry (for all flows and all channels) 
           before updating it */
        hdrrmac4_hist[chan][load_tx_pkt_id].num_flows = 0;
        hdrrmac4_hist[chan][load_tx_pkt_id].sum_payload = 0;
        hdrrmac4_hist[chan][load_tx_pkt_id].pkt_handle = 0;

        for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++ )
        {
          hdrrmac4_hist[chan][load_tx_pkt_id].flows[flow].size = 0;

          hdrrmac4_hist[chan][load_tx_pkt_id].flows[flow].mac_flow_id = 
            HDRRMACCMN_MAC_FLOW_ID_INVALID;
        }

      }

      for (chan = 0; chan < hdrrmac4_combo.num_carriers; chan++)
      {
        phy_chan_idx = hdrrmac4_combo.carrier_data[chan].carrier_id;
        phy_chan_ptr = &hdrrmac4_var_ptr->phy_chan[phy_chan_idx];

        sum_payload = 0;

        /* Store the decisions in hdrrmac4_hist buffer. Rely on the packet id
         * returned by lower layer to index into this history buffer
         */
        hdrrmac4_hist[phy_chan_idx][load_tx_pkt_id].pkt_size_index = 
          phy_chan_ptr->pkt_size_index;

        for( flow = 0; flow < hdrrmac4_num_elig_flows[phy_chan_idx]; flow++ )
        {
          uint8 flow_index = 
            hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id;

          hdrrmac4_hist[phy_chan_idx][load_tx_pkt_id].flows[flow_index].size = 
            hdrrmac4_elig_flows_set[phy_chan_idx][flow].size;

          hdrrmac4_hist[phy_chan_idx][load_tx_pkt_id].flows[flow_index].mac_flow_id = 
            hdrrmac4_elig_flows_set[phy_chan_idx][flow].mac_flow_id;

          sum_payload += hdrrmac4_elig_flows_set[phy_chan_idx][flow].size;
        }

        hdrrmac4_hist[phy_chan_idx][load_tx_pkt_id].num_flows = 
          hdrrmac4_num_elig_flows[phy_chan_idx];

        hdrrmac4_hist[phy_chan_idx][load_tx_pkt_id].sum_payload = sum_payload;

        hdrrmac4_hist[phy_chan_idx][load_tx_pkt_id].pkt_handle = 
          phy_chan_ptr->pkt_info.handle;
      } /* for num carriers in combo */
    }
    else
    {
      /*
       * No packet being built, still call the send_packet function so the
       * rmac4drv can maintain the previous channel count correctly.
       */
      load_tx_pkt_id = hdrrmac4drv_send_packet(
                        &enc_window_missed,
                        0, /* num_carriers */
                        &pkt_enc[0]
#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
                        ,hdrrmac4_var_ptr->rtt_reduct_opt_enabled
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
                        );

      for (chan = 0; chan < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; chan++)
      {
        /* Clear history buffer entry (for all flows and all channels) 
           before updating it */
        hdrrmac4_hist[chan][load_tx_pkt_id].num_flows = 0;
        hdrrmac4_hist[chan][load_tx_pkt_id].sum_payload = 0;
        hdrrmac4_hist[chan][load_tx_pkt_id].pkt_handle = 0;

        for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++ )
        {
          hdrrmac4_hist[chan][load_tx_pkt_id].flows[flow].size = 0;

          hdrrmac4_hist[chan][load_tx_pkt_id].flows[flow].mac_flow_id = 
            HDRRMACCMN_MAC_FLOW_ID_INVALID;
        }
      } /* for all channels */
    }

    /*
     * Record the enc status for logging purpose.
     * Don't overwrite the late case possibly returned from
     * hdrrmac4drv_get_prev_tx_status().
     */
    if (!hdrrmac4_var_ptr->enc_window_missed)
    {
      hdrrmac4_var_ptr->enc_window_missed = enc_window_missed;
    }

  } /* hdr_mode_active */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  /* Increment throughput update counter */
  hdrrmac4_var_ptr->rl_info_update_counter++;

  if ( hdrrmac4_var_ptr->rl_info_update_counter >= 
    HDRRMACCMN_TPUT_UPDATE_PERIOD )
  {
    /* Update throughput numbers */
    hdrrmac4_update_rl_data_rate_est();
    hdrrmac4_update_curr_rl_data_rate();

    /* Store RL info in double buffer for reporting */
    hdrrmac4_latch_rl_info();

    /* Reset counter */
    hdrrmac4_var_ptr->rl_info_update_counter = 0;
  }
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

  /* Save a copy of bucket state for the next subframe */
  hdrrmac4_save_bucket_state();

  /* Log RMAC4 diag packets. */
  hdrrmac4_log_rmac4_diagnostics();

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
  /* if graceful TA processing is completed then clear graceful_ta_started flag */
  if( hdrrmac4_var_ptr->graceful_ta_done == TRUE )
  {
    hdrrmac4_var_ptr->graceful_ta_started = FALSE;

    hdrrmac4_var_ptr->graceful_ta_done = FALSE;

    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
      "RMAC_TA_OPTIMIZ: graceful_ta_started cleared after a subframe after tuneback"
              );
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

  return;

} /* hdrrmac4_process_subframe */
#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

 /* EJECT */
/*=========================================================================

FUNCTION     : HDRRMAC4_GET_CHANGED_FLOW_MAPPING

DESCRIPTION  : 
  This function finds a set of App flows that are either bound or unbound from
  a particular MAC flow and stores it in the arrays bound_flows and 
  unbound_flows respectively.

Note: This function is called from DS task.

DEPENDENCIES : None

PARAMETERS   :
  unbound_flows - Array of Flow mapping structs with a set of App flows for 
                  each MAC flow that are unbound from this MAC flow due to 
                  recent GAUPing
  bound_flows   - Array of Flow mapping structs with a set of App flows for 
                  each MAC flow that are bound due to recent GAUPing

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrrmac4_get_changed_flow_mapping
(
  hdrrmac_flow_mapping_struct_type *unbound_flows,
  hdrrmac_flow_mapping_struct_type *bound_flows
)
{
  uint16 attrib_id;
    /* Attribute ID of the AssociatedFlows attribute */

  hdrrmaccmn_flow_mapping_struct_type in_use_flow_mapping;
    /* Temporary variable to store the in-use instance */

  uint8 in_use_verno;
    /* Version number of the In-Use instance */

  uint16 verno_index;
    /* Index of Associated Flows attribute's version number */

  uint8 m_flow_id = 0;
    /* MAC flow ID */

  uint8 baf_ind;
    /* Bound App Flow index */

  uint8 dup_m_flow_id;
    /* MAC flow with duplicate App-Flow binding */

  boolean is_af_bound = FALSE;
    /* Is AppFlow bound to a MAC flow already? */

  hdrrmaccmn_flow_mapping_struct_type *dup_fm = NULL;
    /* Convinience pointer to RMAC4s flow Mapping struct */

  hdrscmrmaccmn_associated_flows_struct_type *dup_scm_fm = NULL;
    /* Convenience pointer to SCMRMAC4s flow Mapping struct */

  uint8 app_flow_ind;
    /* Index into the AppFlow array where duplicate mapping is found */

  uint8 n_unbound_flows;
    /* Number of Unbound flows */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* Sanity Check */
  ASSERT( (unbound_flows != NULL) && (bound_flows != NULL) );

  /* Initialize the params */
  memset(unbound_flows,  0, (sizeof(hdrrmaccmn_flow_mapping_struct_type) * 
          HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS));
  memset(bound_flows, 0, (sizeof(hdrrmaccmn_flow_mapping_struct_type) *
          HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS));

  /* For each MAC flow find the binding change... */
  for (m_flow_id = 0; m_flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++m_flow_id)
  {
    /* Backup the current in-use flow mapping */
#ifdef FEATURE_MEMCPY_REMOVAL
    in_use_flow_mapping = hdrrmac4_config_ptr->flow[m_flow_id].flow_mapping;
#else
    memcpy(&in_use_flow_mapping, 
           &(hdrrmac4_config_ptr->flow[m_flow_id].flow_mapping),
           sizeof(hdrrmaccmn_flow_mapping_struct_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

    /* Compute the version number index */
    verno_index = (HDRRMAC4_ASSOCIATED_FLOWS_ATTRIB_OFFSET * 
                   HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS + m_flow_id);

    in_use_verno = hdrrmac4_nn_gaup_verno[verno_index];

    /* Get the attribute ID from the flow ID and AssociatedFlow's Base */
    attrib_id = ((uint16)HDRSCMRMAC4_ASSOCIATED_FLOWS_NN_BASE | m_flow_id);

    /* Read the attribute from the Database */
    hdrrmac4_nn_gaup_verno[verno_index] = 
      hdrscmdb_get_inuse_attrib( 
        HDRHAI_RTC_MAC_PROTOCOL,
        attrib_id,
        hdrrmac4_nn_gaup_attribs[HDRRMAC4_ASSOCIATED_FLOWS_ATTRIB_OFFSET]
                                [m_flow_id],
        HDRSCP_MC_RMAC);

    /* Translate the attribute to RMAC4's format */
    hdrrmaccmn_translate_associated_flows_nn(
      m_flow_id,
      &hdrrmac4_config_ptr->flow[m_flow_id].flow_mapping,
      &hdrrmac4_scm_cfg_ptr->flows[m_flow_id].flow_mapping);

    /* Store the MAC flow ID */
    unbound_flows[m_flow_id].mac_flow_id = m_flow_id;

    /* Store the MAC flow ID */
    bound_flows[m_flow_id].mac_flow_id = m_flow_id;

    if (in_use_verno < hdrrmac4_nn_gaup_verno[verno_index] )
    {

      /* Find all App flows that were unbound from this MAC flow */
      hdrrmaccmn_find_flow_binding_change(
        &in_use_flow_mapping,
        &(hdrrmac4_config_ptr->flow[m_flow_id].flow_mapping),
        &unbound_flows[m_flow_id]);

      /* Find all new App flows that were bound to this MAC flow */
      hdrrmaccmn_find_flow_binding_change(
        &(hdrrmac4_config_ptr->flow[m_flow_id].flow_mapping),
        &in_use_flow_mapping,
        &bound_flows[m_flow_id]);

    }

  } /* For each MAC Flow */

  /*--------------------------------------------------------------------------
    By binding an RLP flow to a MAC flow 'x', AN implicitly unbinds that RLP 
    flow from MAC flow 'y' if it is already bound to the MAC flow 'y'. After 
    getting a list of bound flows, we need to make sure that these flows are 
    not bound to any other MAC flow. 
    -------------------------------------------------------------------------*/
  for (m_flow_id=0; m_flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; ++m_flow_id)
  {
    /* For each APP flow newly bound to the MAC flow 'm_flow_id'... */
    for (baf_ind=0; baf_ind < bound_flows[m_flow_id].flow_count; ++baf_ind)
    {
      HDR_MSG_PROT_3(MSG_LEGACY_HIGH,
                     "RLP flow [%d,%d], bound to MAC flow %d",
                     bound_flows[m_flow_id].app_flows[baf_ind].stream_id,
                     bound_flows[m_flow_id].app_flows[baf_ind].sub_stream_id,
                     m_flow_id);

      is_af_bound = FALSE;

      /* Find out if the newly bound App flow is bound to any other MAC flow. */
      is_af_bound = hdrrmac4_is_app_flow_bound_already(
                      &bound_flows[m_flow_id].app_flows[baf_ind],
                      m_flow_id,
                      &dup_m_flow_id,
                      &app_flow_ind);

      /* If the AppFlow is already bound to a different MAC flow, unbind
         it from that MAC flow */
      if (is_af_bound)
      {

        HDR_MSG_PROT_3(MSG_LEGACY_HIGH,
                       "Dup binding: Unbind RLP flow [%d,%d], from MAC flow %d",
                       bound_flows[m_flow_id].app_flows[baf_ind].stream_id,
                       bound_flows[m_flow_id].app_flows[baf_ind].sub_stream_id,
                       dup_m_flow_id);

        /* Initialize the convenience pointers */
        dup_fm = &hdrrmac4_config_ptr->flow[dup_m_flow_id].flow_mapping;
        dup_scm_fm = &hdrrmac4_scm_cfg_ptr->flows[dup_m_flow_id].flow_mapping;

        /*-----------------------------------------------------------------
          Add this AppFlow to Unbound list of that MAC flow to notify RLP
        -----------------------------------------------------------------*/
        n_unbound_flows = unbound_flows[dup_m_flow_id].flow_count;

        unbound_flows[dup_m_flow_id].app_flows[n_unbound_flows].stream_id = 
          dup_fm->app_flows[app_flow_ind].stream_id;
        unbound_flows[dup_m_flow_id].app_flows[n_unbound_flows].sub_stream_id = 
          dup_fm->app_flows[app_flow_ind].sub_stream_id;
        ++unbound_flows[dup_m_flow_id].flow_count;

        /*-----------------------------------------------------------------
            Unbind the AppFlow from the MAC flow in RMAC datastructures
        -----------------------------------------------------------------*/
        if (dup_fm->flow_count > 1)
        {
          ASSERT(dup_fm->flow_count <= HDRRMAC_MAX_APP_FLOWS);
          dup_fm->app_flows[app_flow_ind].stream_id = 
            dup_fm->app_flows[dup_fm->flow_count-1].stream_id;
          dup_fm->app_flows[app_flow_ind].sub_stream_id = 
            dup_fm->app_flows[dup_fm->flow_count-1].sub_stream_id;
          --dup_fm->flow_count;

          dup_scm_fm->app_flows[app_flow_ind].stream_id = 
            dup_scm_fm->app_flows[dup_scm_fm->flow_count-1].stream_id;
          dup_scm_fm->app_flows[app_flow_ind].sub_stream_id = 
            dup_scm_fm->app_flows[dup_scm_fm->flow_count-1].sub_stream_id;
          --dup_scm_fm->flow_count;
        }
        else
        {
          dup_fm->flow_count = dup_scm_fm->flow_count = 0;
          dup_fm->app_flows[0].stream_id = 
            dup_scm_fm->app_flows[0].stream_id = 0; 
          dup_fm->app_flows[0].sub_stream_id = 
            dup_scm_fm->app_flows[0].sub_stream_id = 0;
        }

        /*-----------------------------------------------------------------
                            Commit the update to DB 
        -----------------------------------------------------------------*/
        /* Any update to DB should happen only in HDRMC task context. Using
           HDRPACC cmd to update this attribute to DB */
        hdrpacc_update_flow_mapping_in_scmdb(
          HDRHAI_RTC_MAC_PROTOCOL, 
          (hdrscmrmac3_associated_flows_struct_type *)
            &hdrrmac4_scm_cfg_ptr->flows[dup_m_flow_id].flow_mapping,
          ((uint16) HDRSCMRMAC4_ASSOCIATED_FLOWS_NN_BASE | dup_m_flow_id));
          
      } /* Is AppFlow Bound already? */
    }
  }

  return;

} /* hdrrmac4_get_changed_flow_mapping */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_CONFIG_EARLY_TERMINATION                       EXTERNAL

DESCRIPTION
  Configures early termination on the RL link. This is a test mode function.  

DEPENDENCIES
  None.

PARAMETERS
  config_mode: 0 - Default behavior, RL early termination on.
               1 - Forced early termination after 1 subpacket.
               2 - Forced early termination after 2 subpackets.
               3 - Forced early termination after 3 subpackets.
               4 - Forced termination after 4 subpackets.

RETURN VALUE
  TRUE (config mode accepted),
  FALSE (config mode not valid).

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_config_early_termination
(
  int config_mode
)
{
  return (hdrrmac4drv_config_early_termination( config_mode ));
} /* hdrrmac4_config_early_termination */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_RESET_RTC_RATE_COUNT                           EXTERNAL

DESCRIPTION
  Wrapper function to reset the current rate count.  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_reset_rtc_rate_count( void )
{
  hdrrmac4drv_reset_rtc_rate_count();

  return;

} /* hdrrmac4_reset_rtc_rate_count */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_LOG_RTC_RATE_COUNT                             EXTERNAL

DESCRIPTION
  Wrapper function for logging the current rate count.  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_log_rtc_rate_count( void )
{
  hdrrmac4drv_log_rtc_rate_count();

  return;

} /* hdrrmac4_log_rtc_rate_count */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_SET_FRAME_OFFSET                                   EXTERNAL

DESCRIPTION
  This function sets the RMAC frame offset.

DEPENDENCIES
  The parameter must not be NULL.

PARAMETERS
  frame_offset - the Frame offset

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_set_frame_offset
(
  uint8 frame_offset
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrrmac4_var_ptr->frame_offset = frame_offset;
    /* Copy the data */

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "RMAC frame offset: %d",
                  hdrrmac4_var_ptr->frame_offset );

  /* Pass it along */
  hdrrmac4drv_set_frame_offset(frame_offset);

  return;

} /* hdrrmac4_set_frame_offset */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_SET_FIXED_RRI                                      EXTERNAL

DESCRIPTION
  Sets the RRI to a fixed value for testing purposes. Since in IS856-A we deal
  with packet sizes rather than rates, rri actually refers to the payload size 
  index.

DEPENDENCIES
  None.

PARAMETERS
  rri      - The RRI to fix to. Use 0xf, or HDRRMAC4_IS856_RRI for
             normal operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_set_fixed_rri
(
  uint8 rri
)
{
  boolean is_rri_valid = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* Check if the given RRI is within valid range */ 
  if (( rri < (uint8) HDRRMACCMN_MAX_PAYLOADS ) || 
      (rri == (uint8) HDRRMACCMN_IS856_A_RRI ))
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Warning! Fixing RRI to %d", rri );

    hdrrmac4_test_ps_index_fixed = (hdrrmaccmn_payload_size_enum_type) rri;

    is_rri_valid = TRUE;
  }

  return is_rri_valid;
  
} /* hdrrmac4_set_fixed_rri */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_INIT                                               EXTERNAL

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon the protocol enters the SETUP state.  It initializes the subtype
  specific variables that control the OPEN state operations.

DEPENDENCIES
  Should be run AFTER hdrrmac4_powerup_init().

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_init (void)
{
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  uint8  phy_chan_idx;              /* Channel loop index */
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Reset rate statistics. */
  hdrrmac4_reset_rtc_rate_count();

  /* Init status states and flags */
  hdrrmac4_var_ptr->tx_status = TX_SUSPENDED;
  hdrrmac4_var_ptr->data_allowed = FALSE;
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
  hdrrmac4_var_ptr->graceful_ta_started = FALSE;
  hdrrmac4_var_ptr->graceful_ta_done = FALSE;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  /* Initialize T2P boost variables */
  hdrrmac4_var_ptr->t2p_boost.state = HDRRMACCMN_T2P_BOOST_INACTIVE;
  hdrrmac4_var_ptr->t2p_boost.frab_limited[0] = FALSE;
  hdrrmac4_var_ptr->t2p_boost.frab_limited[1] = FALSE;
  hdrrmac4_var_ptr->t2p_boost.frab_limited[2] = FALSE;
  hdrrmac4_var_ptr->t2p_boost.txpwr_limited = FALSE;

  hdrrmac4_var_ptr->t2p_boost.reset_counter = 0;
  hdrrmac4drv_set_t2p_boost_info( hdrrmac4_var_ptr->t2p_boost.tx_t2p_info );
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  /* Initialize modem API implementation variables */
  for ( phy_chan_idx = 0;
    phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
    phy_chan_idx++ )
  {
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].not_data_lmtd = FALSE;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].not_data_lmtd_factor = 0;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].not_data_lmtd_tput = 0;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].mac_lmtd_tput = 0;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].tx_on_factor = 1 << 16;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].rl_tput_est = 0;
    hdrrmac4_var_ptr->phy_chan[phy_chan_idx].rl_tput_curr = 0;
  }

  hdrrmac4_var_ptr->rlp_q_size = 0;
  hdrrmac4_var_ptr->rl_tput_est = 0;
  hdrrmac4_var_ptr->rl_tput_curr = 0;
  hdrrmac4_var_ptr->plus_flag = FALSE;
  hdrrmac4_var_ptr->plus_flag_reset_counter = 0;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

  /* Initialize the logging structure */
  hdrrmac4_logging.sub_frame_count = 0;

  hdrrmac4_init_flows();

  hdrrmac4_init_chans();

  /* Init. Carrier Requst function */
  hdrrmac4_carrier_req_event(HDRRMAC4_CARRIER_REQ_EVENT_INIT,
                             0,
                             0,
                             0);  
    /* num_chans, phy_chan_idx and phy_chan_mask not used */

  /* Init TCA processed callback */
  hdrrmac4_var_ptr->tca_processed_cb_ptr = NULL;

  /* Init hdrrmac4drv */
  hdrrmac4drv_init(hdrrmac4_common);

  return;

} /* hdrrmac4_init */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_POWERUP_INIT                                       EXTERNAL

DESCRIPTION
  This function initializes RMAC session configuration.

DEPENDENCIES
  Note that this is run in HDRMC context.
  Should be run BEFORE hdrrmac4_init().

PARAMETERS
  init_attrib - TRUE- RMAC4 is the current InUse subtype.  Attributes need
                to be initialized; FALSE otherwies.
  common_parameters - Pointer to the higher level RMAC common parameter
                block.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_powerup_init
(
  boolean init_attrib,
  hdrrmac_common_type * common_parameters
)
{
  static hdrscmrmac4_verno_type hdrrmac4_verno;
    /* uint8 Array to hold the version numbers of RTCMAC config attribs */

  uint8 pl = 0;
    /* Payload index */

  uint8 flow_id = 0;
    /* MAC Flow ID */

  uint8 channel_id = 0;
    /* Reverse Logical Channel ID */

  uint8 config_id = 0;
    /* Configuration ID */

#if defined(FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST) || \
  defined(FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD)
  uint64 mac_nv;           
    /* Temp variable MAC debug mask from NV */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST || 
          FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  uint8 t2p_boost_config = 0;
    /* Boost value to use for tuneaway */
  uint8 y1_config = 0;
    /* Y1 (to calculate start threshold) for RTT boost (0 if RTT undefined) */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrrmac4_common = common_parameters;

  /* Initialize to normal operation */
  hdrrmac4_test_ps_index_fixed = HDRRMACCMN_IS856_A_RRI;

  /* 
   * Retrieve the pointers to memory location to store the configuration
   * computation variables.
   */
  hdrrmaccmn_get_configs_and_vars_mem (HDRSCP_MC_RMAC,
                                       (void **)&hdrrmac4_scm_cfg_ptr,
                                       (void **)&hdrrmac4_config_ptr,
                                       (void **)&hdrrmac4_var_ptr);

  /*-------------------------------------------------------------------------
           Initialize critical sections
  -------------------------------------------------------------------------*/
  if ( !hdrrmac4_rex_init_is_called )
  {
    rex_init_crit_sect( &mac_info_raw_data.mac_info_crit_sect );
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
    rex_init_crit_sect( &hdrrmac4_var_ptr->rl_info_crit_sect );
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

    hdrrmac4_rex_init_is_called = TRUE;
  }

#if defined(FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST) || \
    defined(FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD)
  if ( hdrlmac_get_user_mac_debug_mask( &mac_nv ) != TRUE )
  {
    /* NV is unreadable or inactive, use defaults. */
    HDR_MSG_PROT( MSG_LEGACY_MED, "MAC Debug NV: Using default settings" );

    mac_nv = 0;
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST || 
          FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  /* Set T2P boost config from NV */
  t2p_boost_config = (uint8) ( ( mac_nv & 
    HDRLMAC_USER_MAC_DEBUG_T2P_BOOST_M ) >> 
    HDRLMAC_USER_MAC_DEBUG_T2P_BOOST_S );

  /* Look up and store T2P boost gain in Q.2 dB */
  hdrrmac4_var_ptr->t2p_boost.gain = 
    hdrrmaccmn_nv_to_t2p_boost_map[t2p_boost_config];

  /* Set pre-tuneaway optimization enabled flag */
  hdrrmac4_var_ptr->pre_ta_opt_enabled = ( ( mac_nv & 
    HDRLMAC_USER_MAC_DEBUG_PRE_TA_OPT_DIS ) == 0 ) ? TRUE : FALSE;

  /* Set post-tuneback optimization enabled flag */
  hdrrmac4_var_ptr->post_tb_opt_enabled = ( ( mac_nv & 
    HDRLMAC_USER_MAC_DEBUG_POST_TB_OPT_DIS ) == 0 ) ? TRUE : FALSE;

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
  hdrrmac4_var_ptr->rtt_reduct_opt_enabled = ( ( ( mac_nv & 
    HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_OPT_DIS) == 0 ) ) ? TRUE : FALSE;

  if ( hdrrmac4_var_ptr->rtt_reduct_opt_enabled )
  {
    t2p_boost_config = (uint8) ( ( mac_nv & 
      HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_DEBOOST_M ) >> 
      HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_DEBOOST_S );

    /* Look up and store T2P deboost gain in Q.2 dB */
    hdrrmac4_var_ptr->t2p_boost.deboost_gain = 
      hdrrmaccmn_nv_to_t2p_deboost_map[t2p_boost_config];

    /* Read Y1 config from NV */
    y1_config = (uint8) ( ( mac_nv & 
      HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_Y1_M ) >> 
      HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_Y1_S );
    hdrrmac4_var_ptr->t2p_boost.y1 = hdrrmaccmn_nv_to_y1_map[y1_config];

    /* Reuse for F3 */
    t2p_boost_config = hdrrmac4_var_ptr->t2p_boost.deboost_gain;
    y1_config = hdrrmac4_var_ptr->t2p_boost.y1;
  }
  else
  {
    hdrrmac4_var_ptr->t2p_boost.deboost_gain = 
        HDRRMACCMN_RTT_DEBOOST_DISABLED;
  }
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */

  HDR_MSG_PROT_5( MSG_LEGACY_HIGH,
                  "T2P boost enabled for TA:%d TB:%d gain:%d/-%d y1:%d", 
                  hdrrmac4_var_ptr->pre_ta_opt_enabled, 
                  hdrrmac4_var_ptr->post_tb_opt_enabled, 
                  hdrrmac4_var_ptr->t2p_boost.gain,
                  t2p_boost_config,
                  y1_config );

#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
  /* Check if permitted payload optimization is enabled */
  if ( mac_nv & HDRLMAC_USER_MAC_DEBUG_PERM_PAYLD_OPT_DIS )
  {
    hdrrmac4drv_enable_permitted_payload_optimization( FALSE );
  }
  else
  {
    hdrrmac4drv_enable_permitted_payload_optimization( TRUE );
  }

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "T2P boost enabled for perm payload:%d", 
                  mac_nv & HDRLMAC_USER_MAC_DEBUG_PERM_PAYLD_OPT_DIS );
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

  /*------------------------------------------------------------------------
          Initialize pointers for Non Per-NCX config attributes
  -------------------------------------------------------------------------*/

  /* Simple Attributes */
  hdrrmac4_non_ncx_gaup_attribs[0] = 
    &hdrrmac4_scm_cfg_ptr->max_rl_pilot_tx_pwr_diff;
  hdrrmac4_non_ncx_gaup_attribs[1] = &hdrrmac4_scm_cfg_ptr->payload_thresh;
  hdrrmac4_non_ncx_gaup_attribs[2] = &hdrrmac4_scm_cfg_ptr->merge_thresh;

  /* Complex Attributes */
  hdrrmac4_non_ncx_gaup_attribs[3] = &hdrrmac4_scm_cfg_ptr->aux_pilot_info;
  hdrrmac4_non_ncx_gaup_attribs[4] = hdrrmac4_scm_cfg_ptr->permitted_payload;

  for (pl=0; pl < (uint8) HDRSCMRMACCMN_MAX_NUM_PAYLOADS-1; ++pl)
  {
    hdrrmac4_non_ncx_gaup_attribs[5 + pl] = 
      &hdrrmac4_scm_cfg_ptr->power_params[pl+1][0];
  }

  hdrrmac4_non_ncx_gaup_attribs[17] = &hdrrmac4_scm_cfg_ptr->req_params;
  hdrrmac4_non_ncx_gaup_attribs[18] = hdrrmac4_scm_cfg_ptr->rri_chan_power_params;

  /*------------------------------------------------------------------------
          Initialize pointers for Per-flow config attributes
  -------------------------------------------------------------------------*/

  for (flow_id = 0; 
       flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; 
       ++flow_id)
  {
    /* Simple NN attributes */
    hdrrmac4_nn_gaup_attribs[0][flow_id] = 
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].t2p_filt_tc;
    hdrrmac4_nn_gaup_attribs[1][flow_id] = 
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].burst_duration_factor;
    hdrrmac4_nn_gaup_attribs[2][flow_id] = 
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].qrab_select;
    hdrrmac4_nn_gaup_attribs[3][flow_id] = 
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].tx_mode;
    hdrrmac4_nn_gaup_attribs[4][flow_id] = 
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].merge_thresh;
    hdrrmac4_nn_gaup_attribs[5][flow_id] = 
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].bucket_level_max;

    /* Complex NN Attributes */
    hdrrmac4_nn_gaup_attribs[6][flow_id] =
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].flow_mapping;
    hdrrmac4_nn_gaup_attribs[7][flow_id] = 
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].bucket_factor;
    hdrrmac4_nn_gaup_attribs[8][flow_id] = 
      &hdrrmac4_scm_cfg_ptr->flows[flow_id].data_token_bucket;

  } /* For each flow */

  /*------------------------------------------------------------------------
          Initialize pointers for Per-channel config attributes
  -------------------------------------------------------------------------*/

  for (channel_id = 0; 
       channel_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS; 
       ++channel_id)
  {
    /* Simple CC Attribute */
    hdrrmac4_cc_gaup_attribs[0][channel_id] =
      &hdrrmac4_scm_cfg_ptr->reverse_channels[channel_id].
        ps_qrab_thresh_drc_unlock;
    hdrrmac4_cc_gaup_attribs[1][channel_id] =
      &hdrrmac4_scm_cfg_ptr->reverse_channels[channel_id].
        ps_qrab_thresh_drc_lock;
    hdrrmac4_cc_gaup_attribs[2][channel_id] =
      &hdrrmac4_scm_cfg_ptr->reverse_channels[channel_id].arq_mode;

    /* Complex CC Attribute */
    hdrrmac4_cc_gaup_attribs[3][channel_id] =
      &hdrrmac4_scm_cfg_ptr->reverse_channels[channel_id].
        associated_at_config;
  }

  /*------------------------------------------------------------------------
          Initialize pointers for Per-Configuration config attributes
  -------------------------------------------------------------------------*/

  for (config_id = 0; 
       config_id < HDRSCMRMAC4_MAX_NUM_CONFIGS; 
       ++config_id)
  {
    /* Complex XX Attribute */
    hdrrmac4_xx_gaup_attribs[0][config_id] =
      &hdrrmac4_scm_cfg_ptr->configurations[config_id].t2p_inflow_range;
    hdrrmac4_xx_gaup_attribs[1][config_id] =
      &hdrrmac4_scm_cfg_ptr->configurations[config_id].gu_gd;
    hdrrmac4_xx_gaup_attribs[2][config_id] =
      &hdrrmac4_scm_cfg_ptr->configurations[config_id].pilot_strength;
    hdrrmac4_xx_gaup_attribs[3][config_id] =
      &hdrrmac4_scm_cfg_ptr->configurations[config_id].txt2p_max;
  }

  /*------------------------------------------------------------------------
          Initialize pointers for Per-flow-per-channel config attributes
  -------------------------------------------------------------------------*/

  for (flow_id = 0; 
       flow_id < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; 
       ++flow_id)
  {
    for (channel_id = 0;
         channel_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS; 
         ++channel_id)
    {
      /* Complex NC Attribute */
      hdrrmac4_nc_gaup_attribs[0][flow_id][channel_id] =
        &hdrrmac4_scm_cfg_ptr->associated_flow_config[flow_id][channel_id];
    }
  }

  /* Initialize the verno offset arrays */
  hdrrmac4_init_verno_offsets();

  if (init_attrib)
  {

#ifdef FEATURE_FACTORY_TESTMODE

    if ( ftm_get_mode() == FTM_MODE )
    {
      /* Use default attributes in FTM mode. */
      hdrscmrmac4_set_config_to_default(hdrrmac4_scm_cfg_ptr);
    }
    else

#endif /* FEATURE_FACTORY_TESTMODE */
    {
      /* Get the current attrib value */
      hdrscmdb_get_rmac_inuse_config_and_verno( 
        hdrrmac4_scm_cfg_ptr,
        sizeof(hdrscmrmac4_config_struct_type),
        hdrrmac4_verno,
        sizeof(hdrscmrmac4_verno_type),
        HDRSCP_MC_RMAC);
  
      /* Record the version numbers of GAUPable attributes. */
      hdrrmac4_record_version_nums(hdrrmac4_verno);
    }
  
    /* Translate the config parameters to a format that RMAC4 could use */
    hdrrmac4_translate_config_params();
  }

  hdrrmac4drv_config_params(hdrrmac4_config_ptr);

  /* Init. connection setup & RL dropped ack timer */
  hdrrmac4_init_timers();

  return;

} /* hdrrmac4_powerup_init */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_ENTER_OPEN_STATE

DESCRIPTION
  Enter the Open state.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_enter_open_state (void)
{
  uint8 flow;
    /* loop index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the hdrrmac4 structure. */
  hdrrmac4_var_ptr->data_allowed = TRUE;

  /* Initialize LastBucketLevelMax for all flows */
  for( flow = 0; flow < HDRSCMRMACCMN_MAX_NUM_MAC_FLOWS; flow++)
  {
    hdrrmac4_var_ptr->data_flow[flow].bucket.last_bucket_level_max = 0;
  }

#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
  #error code not present
#endif /* FEATURE_HDR_RMAC3_UNIT_TEST*/

  HDR_MSG_PROT( MSG_LEGACY_MED, "RTCMAC has entered open state" );

  return;

} /* hdrrmac4_enter_open_state */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PA_IS_AVAILABLE                                    EXTERNAL

DESCRIPTION
  This function queries the current availability of the PA.

DEPENDENCIES
  None.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  = PA is available
  FALSE = PA is not currently available, due to 1) RTC MAC is not active or
          2) RTC MAC is in DRC supervision.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_pa_is_available ( hdrhai_protocol_name_enum_type caller )
{
  /* PA is available if we are not in DRC supervision */
  return (hdrrmac4drv_tx_is_available(hdrrmac4_var_ptr->slp_carrier_idx));
} /* hdrrmac4_pa_is_available */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_START_RMAC

DESCRIPTION
  This function is invoked by either hdrrmac4_start_tx() or the RF tx complete
  callback function to initialize and start the RMAC isr.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_start_rmac( void )
{

  /* Make sure RMAC is currently off. */
  if (hdrrmac4_var_ptr->tx_status != TX_STARTED)
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "RTC MAC starting" );

    hdrrmac4drv_update_attributes();

    /* Check state */
    if (hdrrmac4_common->state == HDRRMAC_SETUP_STATE)
    {
      /* Start Tx in setup state */
      hdrrmac4_enter_setup_state();
    }
    else
    {
      hdrrmac4_var_ptr->data_allowed = TRUE;
    }

    /* Configure modulator */
    hdrrmac4drv_config_modulator(hdrrmac4_var_ptr->ati_record.ati_value
#ifdef FEATURE_HDR_BOLT_MODEM
      , hdrrmac4_var_ptr->carrier_wav_enable_mask
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    /* Start frame interrupt */
    hdrrmac4drv_start(hdrrmac4_isr);

    hdrrmac4_var_ptr->tx_status = TX_STARTED;

    #ifdef FEATURE_FACTORY_TESTMODE
    if(( ftm_get_mode() == FTM_MODE ) &&
       (hdrrmac4_common->state != HDRRMAC_OPEN_STATE))
    {
      /* Go straight to Open State in FTM Mode without 
         waiting for RTCACK signal*/
      hdrrmac_enter_state( HDRRMAC_OPEN_STATE );
    }
    #endif 

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
    hdrrmac4_init_flexconn_dtx_log_info();
      /* Initialize the structure to control flexconn or dtx log packet. start_rmac()
         is called at connection setup, tune-away and HHO. This is the common place
         where we could initialize the structure in all three scenarios.
           connection setup, tune away: rmac_tx_enabled_cb()->rmac4_start_rmac
           HHO: rmac_tx_enabled_cb->rmac4_resume_connection->rmac4_start_tx->rmac4_start_rmac
      */
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
    hdrrmac4_var_ptr->t2p_boost.suspended = FALSE;
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
  }

  return;

} /* hdrrmac4_start_rmac */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_ENTER_INACTIVE_STATE

DESCRIPTION
  Enter the inactive state.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_enter_inactive_state( void )
{

  uint8 phy_chan_idx;
    /* channel loop index */

  uint16 chan_mask = 0;
    /* channel mask to stop rmac4drv */

  uint8 timer_index;
    /* timer loop index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  for (phy_chan_idx = 0;
       phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
       phy_chan_idx++)
  {
    if(hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state != 
       HDRRMAC4_CHAN_INACTIVE_STATE)
    {
      if (hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state != 
          HDRRMAC4_CHAN_ADD_STATE)
      {
        /* 
         * DRV has not started the channel if it's in either the INACTIVE or
         * ADD state, so no need to stop the channel.
         */
        chan_mask |= (0x01 << phy_chan_idx);

        if (hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state 
          == HDRRMAC4_CHAN_OPEN_STATE)
        {
          /* Decrement the number of open channels */
          hdrrmac4_var_ptr->num_open_state_chans--;
        }

      }

      hdrrmac4_chan_enter_inactive_state(phy_chan_idx);
    }
  } /* for each physical channel */

  if (chan_mask != 0)
  {
    /* Disabel individual channels */
    (void) hdrrmac4drv_stop_channels(chan_mask);
  }

  /* Check if we need to shutdown */
  if (hdrrmac4_var_ptr->tx_status != TX_SUSPENDED)
  {
    hdrrmac4_var_ptr->tx_status = TX_SUSPENDED;
  }

  /* Disable reverse link drivers.  */
  hdrrmac4drv_stop(
#ifdef FEATURE_HDR_QTA 
   FALSE
#endif /* FEATURE_HDR_QTA */
   );

  /* Disable Rev A mode in drivers, since we are exiting */
  hdrrmac4drv_exit();

  /* Clean up all outstanding RL Drop ACK timers */
  for (timer_index = 0;
       timer_index < HDRRMAC4_MAX_NUM_DROP_MSGS;
       timer_index++)
  {
    hdrrmac4_stop_rl_drop_ack_timer(timer_index);
  }

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
  hdrrmac4_submit_flexconn_dtx_log();
   /* Submit log packet at the end of connection. When tearing down connection,
      the call flow is:
      rmac_enter_inactive_state-> rmac4_enter_inactive_state(), mod_stop_tx() (DAC stop)
                                  rmac4_enter_inactive_state()->rmac4drv_stop()-> disable frame isr, pa off, etc.
   */
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
  hdrrmac4_var_ptr->graceful_ta_started = FALSE;
  hdrrmac4_var_ptr->graceful_ta_done = FALSE;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

  return;

} /* hdrrmac4_enter_inactive_state */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_SHUTDOWN    

DESCRIPTION
  This function starts the shutdown process.  This will allow all packets 
  still in transit to complete before RTC MAC is disabled.

  Once any packets still being transmitted complete, the callback function 
  will be called to give the shutdown complete indication.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_shutdown( void )
{
  
  hdrrmac4_var_ptr->tx_status = TX_SUSPENDED;

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "RTC Shutdown: Data channel ending" );

  /* Don't ask for more data */
  hdrrmac4_var_ptr->data_allowed = FALSE;

  /* Inform driver layer that we've been asked to shutdown. */
  hdrrmac4drv_shutdown(hdrrmac4_shutdown_cb);

  return;

} /* hdrrmac4_shutdown */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_STOP_TX                                           INTERNAL

DESCRIPTION
  Stop, or suspend, the transmitter.  If no frame TX is in progress, the
  TX will turn off immediately.  Otherwise, TX shutdown is syncronized with
  the end of the current frame.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - pointer to received command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_stop_tx( void )
{
  /* Check state for setup */
  if (hdrrmac4_common->state == HDRRMAC_SETUP_STATE)
  {
    ERR("hdrrmac4_stop_tx() may not be called in the setup state",0,0,0);
  }

  /* Don't ask for more data */
  hdrrmac4_var_ptr->data_allowed = FALSE;

  /* Check if we are already suspended */
  if (hdrrmac4_var_ptr->tx_status == TX_STARTED)
  {
    hdrrmac4_var_ptr->tx_status = TX_SUSPENDED;

    /* Disable driver layer */
    hdrrmac4drv_stop(
#ifdef FEATURE_HDR_QTA 
   FALSE
#endif /* FEATURE_HDR_QTA */
   );
  
    hdrind_give_ind(HDRIND_RTCMAC_TX_STOPPED, NULL);

    /* We were able to disable immediately */
    HDR_MSG_PROT(MSG_LEGACY_HIGH,"Tx disabled");
  
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "TX STOP" );

  }
  else
  {
    /* Tx wasn't on; give TX_STOPPED indication to keep in sync. */
    hdrind_give_ind(HDRIND_RTCMAC_TX_STOPPED, NULL);
  }

  /* If were pending start, just go back to suspended */
  if (hdrrmac4_var_ptr->tx_status == TX_START_PENDING)
  {
    hdrrmac4_var_ptr->tx_status = TX_SUSPENDED;
  }

  return;

} /* hdrrmac4_stop_tx */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_SIG                                         EXTERNAL

DESCRIPTION
  This function handles the task signals assigned to this protocol.

DEPENDENCIES
  None.

PARAMETERS
  signal - task signal to be processed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_process_sig
(
  rex_sigs_type signal
)
{
  if ( signal == HDRRMACCMN_ISR_SIG )
  {
    if( hdrrmac4_common->state != HDRRMAC_INACTIVE_STATE )
    {
      #ifdef FEATURE_FACTORY_TESTMODE
      if( ftm_get_mode() != FTM_MODE )
      {
      #endif
        #ifdef FEATURE_HDR_RMAC4_UNIT_TEST
        #error code not present
#else
        hdrrmac4_process_subframe();
        #if defined( FEATURE_HDR_FLEX_CONN )\
        ||defined( FEATURE_HDR_DEVICE_ONLY_DTX )\
        ||defined ( FEATURE_HDR_TRAFFIC_DTX )
        /* check whether to turn on flexconn or dtx mode */
        hdrrmaccmn_update_dtx_tx_limit( hdrrmac4drv_get_funnel_mode() );
        hdrrmac4_flexconn_dtx_monitor();
        #endif /* FEATURE_HDR_FLEX_CONN|| FEATURE_HDR_DEVICE_ONLY_DTX ||
        || FEATURE_HDR_TRAFFIC_DTX */
        #endif
      #ifdef FEATURE_FACTORY_TESTMODE
      }
      /*In FTM Mode every subframe interrupt ISR is executed in 
        hdrrmac4_process_isr_sig() function*/
      else 
      {
        hdrrmac4_process_isr_sig();
      }
      #endif
    }
  }
  else
  {
    ERR("Invalid signal! = 0x%h",signal,0,0);
  }

  return;

} /* hdrrmac4_process_sig */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_MSG                                        EXTERNAL

DESCRIPTION
  This function processes incoming messages for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  rcv_msg_ptr - pointer to receive message buffer.
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_process_msg
(
  dsm_item_type * rcv_msg_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  /* Receive message header buffer. */
  hdrrmaccmn_msg_id_type rcv_msg_id;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG));

  /*-------------------------------------------------------------------------
   Extract the message ID to determine which message has been received.
  -------------------------------------------------------------------------*/
  if (HDRRMACCMN_GET_MSG_ID( rcv_msg_ptr, &rcv_msg_id ) == E_SUCCESS)
  {
    
    HDRTRACE_MSG( HDRHAI_RTC_MAC_PROTOCOL, rcv_msg_id, 
                  (int32) hdrrmac4_common->state );
    
    if (protocol_instance == HDRHAI_IN_USE)
    {
      /*---------------------------------------------------------------------
       If the message ID is recognized, process the message.
      ---------------------------------------------------------------------*/
      switch (rcv_msg_id)
      {
        case HDRRMAC4_RTCACK_MSG_ID:
          hdrrmac4_process_rtcack_msg( rcv_msg_ptr );
          break;

        case HDRRMAC4_GRANT_MSG_ID:
          hdrrmac4_process_grant_msg( rcv_msg_ptr );
          break;

        case HDRRMAC4_RL_DROP_ACK_MSG_ID:
          hdrrmac4_process_rl_drop_ack_msg( rcv_msg_ptr);
          break;

#ifdef FEATURE_LTE_TO_HDR_OH
        case HDRRMAC4_HRPD_OPEN_LOOP_PARAMS_MSG_ID:
          hdrrmac_process_olp_msg( rcv_msg_ptr );
          break;
#endif /* FEATURE_LTE_TO_HDR_OH */

        case HDRRMAC0_BROADCAST_REVERSE_RATE_LIMIT_MSG_ID:
          HDR_MSG_PROT( MSG_LEGACY_MED, 
                        "Rx'd Rev 0 broadcast rate limit message; Ignoring it");
          break;

        case HDRRMAC4_REQUEST_MSG_ID:
          HDR_MSG_PROT( MSG_LEGACY_HIGH,
                        "Rx'd Request Msg. Ignoring (msg not supported).");
          break;

        default:
          ERR("Invalid in use message ID! = %d",rcv_msg_id,0,0);
          break;
      }
    }
  }
  else
  {
    ERR("Unable to unpack msg_id!",0,0,0);
  }

  /*-------------------------------------------------------------------------
   Free the message buffer and exit.
  -------------------------------------------------------------------------*/
  /* The same message will be send to RMAC3 processing so don't free the buffer yet */
  dsm_free_packet( &rcv_msg_ptr );

  return;

} /* hdrrmac4_proc_msg */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_PROCESS_TIMER                                      EXTERNAL

DESCRIPTION
  This function processes the timer expiry events for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id: Timer Id

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_process_timer(uint32 timer_id)
{

  boolean     tmo = FALSE;
    /* if we declare time out on the RCCD ACK message */

  hdrrmac4_timer_enum_type  rmac4_timer_id;
    /* for identifying RMAC4 timer that expired */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract RMAC4 timer id. */
  rmac4_timer_id = (hdrrmac4_timer_enum_type) (timer_id >> 16);
  timer_id &= 0xFFFF;

  /* Switch on the RMAC4 timer ID and take appropriate action. */
  switch(rmac4_timer_id)
  {
    /* ReverseAcquisition timer expiry */
    case HDRRMAC4_TIMER_RL_ACQ:

      HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                     "RL ACQ Timer %d expired",
                     timer_id);

      /* Sanity check the timer_id value */
      if (timer_id < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
      {
        /* Sanity check the channel state */
        if (hdrrmac4_var_ptr->phy_chan[timer_id].chan_state == 
            HDRRMAC4_CHAN_SETUP_STATE)
        {
          if (hdrrmac4_var_ptr->num_open_state_chans != 0)
          {
            hdrrmac4_drop_info_type drop_info;
              /* drop information */

            uint8 drop_slot;

            /* 
             * If at least one channel is in the OPEN state, generate a 
             * ReverseCDMAChannelDropped Message for this channel
             */
            drop_info.phy_chan_idx = timer_id;  /* timer id is the channel index */
            drop_info.reason_code = HDRRMAC4_RL_DROP_REASON_RL_ACQ_TMR_EXPIRED;

            drop_slot = hdrrmac4_generate_rl_drop_msg(1, &drop_info);

              /* send the message */
            hdrrmac4_send_rl_drop_msg(drop_slot);
          }
          else if (hdrrmac4_var_ptr->num_inactive_state_chans ==
                   HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
          {
            /*
             * If all channels are in INACTIVE state, generate a SupervisionFailed
             * indication.
             */

            /* 
             * Not needed since a link acquired indication is generated on 
             * acquiring the 1st RL.
             */
          }

          /* ReverseAcquisition timer expires for this channel */
          hdrrmac4_chan_enter_inactive_state(timer_id);

          /* Stop the channel in rmac4drv */
          (void) hdrrmac4drv_stop_channels(0x01 << timer_id);

        }
        else
        {
          HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                         "Invalid state %d for chan %d",
                         hdrrmac4_var_ptr->phy_chan[timer_id].chan_state,
                         timer_id);
        }
      }
      else
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                       "Invalid timer id %d",
                       timer_id);
      }

      break;

    /* RCCD Ack timer expiry */
    case HDRRMAC4_TIMER_RL_DROP_ACK:

      /* Array boundary sanity check */
      ASSERT ( timer_id < HDRRMAC4_MAX_NUM_DROP_MSGS );

      /* Increment the attempt count */
      hdrrmac4_rl_drop_var.rl_drop_msg[timer_id].num_attempts++;

      if (hdrrmac4_rl_drop_var.rl_drop_msg[timer_id].num_attempts ==
          HDRRMAC4_MAX_RL_DROP_MSG_ATTEMPTS)
      {
        tmo = TRUE;
      }

      if (tmo)
      {
        /* Never got an ACK back from AN, give it up */
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                        "RL DROP ACK TMO Trans_id = %d",
                        hdrrmac4_rl_drop_var.rl_drop_msg[timer_id].rl_dropped_msg.
                          trans_id );

        /* free the slot */
        hdrrmac4_rl_drop_var.rl_drop_msg[timer_id].in_use = FALSE;
        hdrrmac4_rl_drop_var.rl_drop_msg[timer_id].msg_send_status_arrived = 
          FALSE;
      }
      else
      {
        /* Retransmit the same message */
        hdrrmac4_send_rl_drop_msg(timer_id);
      }

      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Invalid RMAC4 timer id %d", 
                      rmac4_timer_id );

  } /* switch */

  return;

} /* hdrrmac4_process_timer */


/* EJECT */
/*===========================================================================

FUNCTION HDRRMAC4_PROCESS_IND

DESCRIPTION
  This function processes indications given to AMAC.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for AMAC to process
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrmac4_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  static hdrscmrmac4_verno_type hdrrmac4_verno;
    /* uint8 Array to hold the version numbers of RTCMAC config attribs */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDRTRACE_IND(HDRHAI_RTC_MAC_PROTOCOL, ind_name, 
               (int32) hdrrmac4_common->state );

  switch (ind_name)
  {
    /* Session Change Indication */
    case HDRIND_SCM_SESSION_CHANGED:

#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
      #error code not present
#else
      /* Get the current attrib value */
      hdrscmdb_get_rmac_inuse_config_and_verno( 
        hdrrmac4_scm_cfg_ptr,
        sizeof(hdrscmrmac4_config_struct_type),
        hdrrmac4_verno,
        sizeof(hdrscmrmac4_verno_type),
        HDRSCP_MC_RMAC);

      /* Record the version numbers of GAUPable attributes. */
      hdrrmac4_record_version_nums(hdrrmac4_verno);
#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */

      /* Translate the config parameters to a format that RMAC4 could use */
      hdrrmac4_translate_config_params();
      break;

    /* RMAC4 GAUP Update indication */
    case HDRIND_SCM_RMAC_GAUP: 
#ifndef FEATURE_HDR_RMAC4_UNIT_TEST
      hdrrmac4_process_attrib_update();
#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */
      break;

    /* FTCMAC.ReverseTrafficChannelRestartExpired */
    case HDRIND_RTC_RESTART_TIMER_EXPIRED:
      hdrrmac4_process_restart_timer_expired_ind (ind_data_ptr->carrier_id);
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_SAP_LINK_ACQUIRED:
      hdrind_give_ind( HDRIND_RTCMAC_LINK_ACQUIRED, NULL );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    case HDRIND_FTCMAC_SUPERVISION_FAILED:
      if(hdrrmac4_common->state != HDRRMAC_INACTIVE_STATE)
      {
        hdrrmac4drv_disable_frame_isr();
      }
      break;

    default:
      ERR( "Unknown indication: %d", ind_name, 0, 0 );
      break;
  }

} /* hdrrmac4_process_ind */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_NOTIFY_SUSPEND                                EXTERNAL

DESCRIPTION
  Notifies RTC MAC of upcoming suspend event so that it may ramp down
  RL transmissions if necessary.  For example, for tuneaway this function
  would be called by HDR Searcher (hdrsrchtc) to schedule the tuneaway.

DEPENDENCIES
  None.

PARAMETERS
  cancel_suspend - TRUE when scheduled suspend is to be cancelled
  suspend_slot   - If cancel_suspend is FALSE, slot when link is scheduled
                   to be suspended.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_notify_suspend
( 
  boolean cancel_suspend,
    /* TRUE when scheduled suspend is to be cancelled */

  uint32  suspend_slot
    /* Slot when link is scheduled to be suspended. */
)
{

  /* Check state for setup */
  if (hdrrmac4_common->state != HDRRMAC_SETUP_STATE)
  {
    hdrrmac4drv_notify_suspend( cancel_suspend, suspend_slot );
  
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                  "RTC MAC suspend: cancel = %d, scheduled suspend slot = %d",
                  cancel_suspend,
                  suspend_slot);

  }
  else
  {
    ERR("Suspend in start state not supported!",0,0,0);
  }

  return;

} /* hdrrmac4_notify_suspend */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_SUSPEND_CONNECTION                                 EXTERNAL

DESCRIPTION
  Suspends an HDR traffic connection.  The RTC MAC will not ask for more
  data from PCP until hdrrmac4_resume_connection() is called.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_suspend_connection( void )
{
  /* Check state for setup */
  if (hdrrmac4_common->state != HDRRMAC_SETUP_STATE)
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "RTC MAC suspend" );

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
    hdrrmac4_var_ptr->t2p_boost.suspended = TRUE;
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
  }
  else
  {
    ERR("Suspend in start state not supported!",0,0,0);
  }

  return;

} /* hdrrmac4_suspend_connection */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_RESUME_CONNECTION                                 EXTERNAL

DESCRIPTION
  Resume an HDR traffic connection which was suspended by
  hdrrmac4_suspend_connection().

DEPENDENCIES
  None.

PARAMETERS
  ms - mS that connection was suspended
  hho_flag - FALSE -> resume from tune-away
             TRUE  -> resume from hard-handoff

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_resume_connection
(
  int ms,
    /* time, in mS, that connection was suspended. */

  boolean hho_flag
    /* FALSE - resume from tune-away
     * TRUE  - resume from hard-handoff
     */
)
{

  /* Catch up to the correct values of T2PInflow and BucketLevelSat to
   * make up for not updating them during tuneaway
   */
  hdrrmac4_away_compensate( ms, hho_flag );
                                                                
  /* Update bucket saturation level for each MAC flow */
  hdrrmac4_update_bucket_level_sat();

  /* Save a copy of bucket state for the next subframe */
  hdrrmac4_save_bucket_state();

  hdrrmac4_var_ptr->data_allowed = TRUE;
    /* Data is now allowed */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
  hdrrmac4_var_ptr->graceful_ta_done = TRUE;
    /* operations based on graceful TA would have completed, 
     * set to clear the graceful_ta_started flag 
     */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  if ( hdrrmac4_var_ptr->post_tb_opt_enabled &&
       hdrrmac4_var_ptr->t2p_boost_enabled )
  {
    /* Start T2P boost for HiCap packets with RRIs 7 and higher */
    hdrrmac4_var_ptr->t2p_boost.state = HDRRMACCMN_T2P_BOOST_POST_TB_ACTIVE;

    /* Set T2P boost reset counter */
    hdrrmac4_var_ptr->t2p_boost.reset_counter = HDRRMACCMN_T2P_BOOST_SFS + 1;

    HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, "T2P boost TB started, gain:%d", 
                       hdrrmac4_var_ptr->t2p_boost.gain );
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

  hdrrmac4_start_tx();
    /* Trigger to start up RL link */

  return;

} /* hdrrmac4_resume_connection */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_POWERDOWN_TX                                       EXTERNAL

DESCRIPTION
  Powers down the RF Tx hardware. Note: should be command.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_powerdown_tx
( 
#ifdef FEATURE_HDR_QTA 
   boolean is_qta /* Is power up tx after QTA? */
#else
  void
#endif /* FEATURE_HDR_QTA */
)
{

  /* Disable driver layer (PA, frame interrupt) */
  hdrrmac4drv_stop( 
#ifdef FEATURE_HDR_QTA 
   is_qta
#endif /* FEATURE_HDR_QTA */
  );

  /* Make sure we don't request data */
  hdrrmac4_var_ptr->data_allowed = FALSE;

  /* Update tx state */
  hdrrmac4_var_ptr->tx_status = TX_SUSPENDED;

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
  hdrrmac4_submit_flexconn_dtx_log();
   /* Submit log packet before HHO and tune-away. The call flows are:
      HHO:
        LMAC->stop_tx()->rmac4_stop_tx()->rmac3drv_stop()->
                 mod_disable_frame_isr(), rmac3drv_pa_off()(send timed pa_off msg)
        Then SRCH->powerdown_tx()->rmac4drv_stop() ( this is duplicated), mod_stop_tx() (DAC stop here)
      Tune-Away:
        SRCH->powerdown_tx()->rmac4drv_stop() , mod_stop_tx() (DAC stop here)
   */
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "T2P boost %d => stopped due to TX shutdown", 
                  hdrrmac4_var_ptr->t2p_boost.state );

  /* Reset T2P boost and limiting flag */
  hdrrmac4_var_ptr->t2p_boost.state = HDRRMACCMN_T2P_BOOST_INACTIVE;

  if ( !hdrrmac4_var_ptr->t2p_boost.suspended )
  {
    /* If shutdown not due to tuneaway, reset FRAB/TxPwr limited flag */
    hdrrmac4_var_ptr->t2p_boost.frab_limited[0] = FALSE;
    hdrrmac4_var_ptr->t2p_boost.frab_limited[1] = FALSE;
    hdrrmac4_var_ptr->t2p_boost.frab_limited[2] = FALSE;
    hdrrmac4_var_ptr->t2p_boost.txpwr_limited = FALSE;
  }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

  return;

} /* hdrrmac4_powerdown_tx */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_RL_CARRIERS                                 EXTERNAL

DESCRIPTION
  Update the RMAC4 with RL information from TCA.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - Pointer to the command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_update_rl_carriers
(
  hdrrmac_cmd_union_type  *cmd_ptr
)
{
  uint8       phy_chan, assigned_chan;
    /* channel loop index */

  boolean     assigned;
    /* is the channel assigned? */

  uint16  chan_mask = 0;
    /* channel bit mask to send to rmac4drv */

  hdrrmac_cmd_update_rl_carriers_struct_type  *update_rl_ptr;
    /* convenience pointer */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* init. the pointer */
  update_rl_ptr = &cmd_ptr->update_rl_carriers;

  /* Store pointer to callback to be called once TCA is processed */
  hdrrmac4_var_ptr->tca_processed_cb_ptr = update_rl_ptr->
    tca_processed_cb_ptr;

  /* update the frame offset information */
  hdrrmac4_var_ptr->frame_offset = update_rl_ptr->frame_offset;
  hdrrmac4drv_set_frame_offset(hdrrmac4_var_ptr->frame_offset);

  /* update the DSC gain base */
  hdrrmac4drv_set_dsc_gain_base(update_rl_ptr->dsc_gain_base);

  /* Find out the channels to be deactivated */
  for (phy_chan=0; phy_chan<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; phy_chan++)
  {
    if (hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state != 
        HDRRMAC4_CHAN_INACTIVE_STATE)
    {
      assigned = FALSE;

      for (assigned_chan=0; 
           ((assigned_chan < update_rl_ptr->num_rls) && !assigned);
           assigned_chan++)
      {
        if (SYS_CHAN_CMP_EQUAL(
              (hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record),
              (update_rl_ptr->rl_carrier[assigned_chan].channel_record)))
        {
          /* Found the same channel (band class and channel number equal) */
          assigned = TRUE;

          /* Do a sanity check to make sure that it has the same rl_chan_index
             as from previous assignment */
          if (phy_chan != 
              update_rl_ptr->rl_carrier[assigned_chan].rl_chan_index)
          {
            /* Complain */
            HDR_MSG_PROT_4(
              MSG_LEGACY_ERROR,
              "band %d chan %d assignment changed! old %d, new %d",
              hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record.band,
              hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record.chan_num,
              phy_chan,
              update_rl_ptr->rl_carrier[assigned_chan].rl_chan_index);

            /* Deactivate the current channel to clean up */
            assigned = FALSE;
          }
          else
          {
            /* 
             * Update the subaset information in case it's changed due to
             * new TCA.
             */
#ifdef FEATURE_MEMCPY_REMOVAL
            hdrrmac4_var_ptr->phy_chan[phy_chan].subaset_info = 
                      update_rl_ptr->subaset_info[
                     (update_rl_ptr->rl_carrier[assigned_chan].subaset_index)];
#else
            memcpy (&hdrrmac4_var_ptr->phy_chan[phy_chan].subaset_info,
                    &update_rl_ptr->subaset_info[
                      (update_rl_ptr->rl_carrier[assigned_chan].
                       subaset_index)],
                    sizeof(hdrrmac_subaset_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

            /* update rmac4drv with subaset information */
            hdrrmac4drv_update_rl_info(
              phy_chan,
              &hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record,
              &hdrrmac4_var_ptr->phy_chan[phy_chan].subaset_info,
              hdrrmac4_var_ptr->phy_chan[phy_chan].dropping_rank.curr );
          }

        } /* if same BandClass and ChannelNumber */

      } /* for each assigned channel in the TCA */

      if (!assigned)
      {
        if (hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state ==
            HDRRMAC4_CHAN_OPEN_STATE)
        {
          /*
           * If the channel to be deleted has already gone to OPEN state,
           * we have to read out all the previous TX status before we can
           * put the channel in the INACTIVE state.  Ready the channel to 
           * be deactivated.
           */
           hdrrmac4_chan_enter_delete_state(phy_chan);
        }
        else if (hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state ==
          HDRRMAC4_CHAN_SETUP_STATE)
        {
          /* 
           * If the channel to be deleted is in setup state, stop sending
           * carrier pilot and transition it to INACTIVE state.
           */

          /* Construct bit mask for channels to be deactivated in RMACDRV */
          chan_mask |= (0x1 << phy_chan);

          /* Deactivate channel */
          hdrrmac4_chan_enter_inactive_state(phy_chan);
        }
        else
        {
          /*
           * If the channel is neither in OPEN, nor in SETUP state, 
           * just transition it to INACTIVE state.
           */
          hdrrmac4_chan_enter_inactive_state(phy_chan);
        }
      }

    } /* if chanell is not in INACTIVE state */

  } /* for each physical channels */

  if (chan_mask != 0)
  {
    /* Tell rmac4drv to stop the marked channels */
    (void) hdrrmac4drv_stop_channels(chan_mask);
  }

  /* Find out channels to be activated */
  for (assigned_chan=0; 
       assigned_chan<update_rl_ptr->num_rls;
       assigned_chan++)
  {
    assigned = FALSE;

    for (phy_chan=0; 
         ((phy_chan<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS) && !assigned);
         phy_chan++)
    {
      if ((hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state != 
           HDRRMAC4_CHAN_INACTIVE_STATE) &&
          (hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state !=
           HDRRMAC4_CHAN_DELETE_STATE))
      {
        if (SYS_CHAN_CMP_EQUAL(
              (hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record),
              (update_rl_ptr->rl_carrier[assigned_chan].channel_record)))
        {
          assigned = TRUE;

          /* Do a sanity check to make sure that it has the same rl_chan_index
             as from previous assignment */
          if (phy_chan != 
              update_rl_ptr->rl_carrier[assigned_chan].rl_chan_index)
          {
            /* Complain */
            HDR_MSG_PROT_4(
              MSG_LEGACY_ERROR,
              "band %d chan %d assignment changed! old %d, new %d",
              hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record.band,
              hdrrmac4_var_ptr->phy_chan[phy_chan].chan_record.chan_num,
              phy_chan,
              update_rl_ptr->rl_carrier[assigned_chan].rl_chan_index);

            /* Deactivate the current channel to clean up */
            assigned = FALSE;

            hdrrmac4_chan_enter_delete_state(phy_chan);
          }
          else
          {
            /* 
             * Channel is currently assigned, update the new dropping rank.
             * Any change will be updated in the next subframe.
             */
            hdrrmac4_var_ptr->phy_chan[phy_chan].dropping_rank.curr =
              update_rl_ptr->rl_carrier[assigned_chan].drop_rank;
          }
        }
      }
    } /* for each physical channel */

    if (!assigned)
    {
      /* Activate the assigned channel */
      hdrrmac4_chan_enter_add_state(assigned_chan, update_rl_ptr);
    }
  } /* for each assigned channel in TCA */

#ifdef FEATURE_HDR_BOLT_MODEM
  /* Update mask for enabling carriers in WAV */
  hdrrmac4_var_ptr->carrier_wav_enable_mask = 0;
  for (phy_chan=0; phy_chan<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; phy_chan++)
  {
    if ( ( hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state != 
         HDRRMAC4_CHAN_DELETE_STATE ) &&
       ( hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state != 
         HDRRMAC4_CHAN_INACTIVE_STATE ) )
    {
      hdrrmac4_var_ptr->carrier_wav_enable_mask |= ( 1 << phy_chan );
    }
  }
#endif /* FEATURE_HDR_BOLT_MODEM */

  return;

} /* hdrrmac4_update_rl_carriers */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_ENTER_DRC_SUPERVISION                             EXTERNAL

DESCRIPTION
  Performs sanity checks on enter drc supervision command and then 
  passes it to RMAC driver which in turn disables RL transmission and 
  reverse power control on the specified carrier.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - Pointer to the command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_enter_drc_supervision
(
  hdrrmac_cmd_union_type  *cmd_ptr
)
{
  uint8       phy_chan;
    /* channel loop index */

  hdrrmac_cmd_drc_supervision_struct_type  *drc_sup_ptr;
    /* convenience pointer */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* init. the pointer */
  drc_sup_ptr = &cmd_ptr->drc_supervision;

  /* Find out the channels under DRC supervision */
  for (phy_chan=0; phy_chan<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; phy_chan++)
  {
    if ((drc_sup_ptr->rl_carrier_mask & (1<<phy_chan)) != 0)
    {
      if (hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state != 
          HDRRMAC4_CHAN_INACTIVE_STATE)
      {
#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
        /* Stop DTX/FC monitoring when the first carrier enters DRC
           supervision
        */
        if ( hdrrmac4drv_no_carrier_in_drc_supervision() == TRUE )
        {
          /* hdrrmac4drc_no_carrier_in_drc_supervision() should return TRUE if 
             no carrier is in DRC supervision yet. Therefore, now it is the first
             carrier enters DRC supervision. 
          */
          hdrrmac_stop_flexconn_dtx_monitor( TRUE );
            /* If there is any carrier in DRC supervision, we want to stop
               DTX/FC monitoring. The action to stop DTX/FC monitoring should be
               done only once, at the time when the first carrier entering DRC
               supervision.
               DTX should be disabled before PA off. PA off is done in enter
               DRC supervision which is the next step.
               Input param is true, because we want to suspend RL inactive timer
               when entering DRC supervision.
            */
        }
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */

        hdrrmac4drv_enter_drc_supervision(phy_chan);

      }
      else
      {
        HDR_MSG_PROT_1(
          MSG_LEGACY_ERROR,
          "Enter DRC supervision cmd received for inactive carrier %d",
          phy_chan );
      }

    } /* if the channel is under DRC supervision */

  } /* for all RL physical channels */

  return;

} /* hdrrmac4_enter_drc_supervision */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_EXIT_DRC_SUPERVISION                              EXTERNAL

DESCRIPTION
  Performs sanity checks on exit drc supervision command and then 
  passes it to RMAC driver which in turn re-enables RL transmission and 
  reverse power control on the specified carrier.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - Pointer to the command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_exit_drc_supervision
(
  hdrrmac_cmd_union_type  *cmd_ptr
)
{
  uint8       phy_chan;
    /* channel loop index */

  hdrrmac_cmd_drc_supervision_struct_type  *drc_sup_ptr;
    /* convenience pointer */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* init. the pointer */
  drc_sup_ptr = &cmd_ptr->drc_supervision;

  /* Find out the channels under DRC supervision */
  for (phy_chan=0; phy_chan<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; phy_chan++)
  {
    if ((drc_sup_ptr->rl_carrier_mask & (1<<phy_chan)) != 0)
    {
      if (hdrrmac4_var_ptr->phy_chan[phy_chan].chan_state != 
          HDRRMAC4_CHAN_INACTIVE_STATE)
      {
        hdrrmac4drv_exit_drc_supervision(phy_chan);

        HDR_MSG_PROT_1(
          MSG_LEGACY_HIGH,
          "Exit DRC supervision cmd received for carrier %d",
          phy_chan );
      }
      else
      {
        HDR_MSG_PROT_1(
          MSG_LEGACY_HIGH,
          "Exit DRC supervision cmd received for inactive carrier %d",
          phy_chan );
      }

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
  
      /* Start DTX/FC monitoring if all carriers are out of DRC supervision. It
         should be called for either active carrier or inactive carrier.
         For active carrier:
         PA will be turned on inside rmac4_exit_drc_supervision().
         Since dtx enable won't be sent until FL and RL timer expires,
         at that time PA ON response should be received already. SW does not
         have code to check PA on response explicityly.
         Resume RL inactive timer after exiting DRC supervision and pass TRUE 
         as input param.
         For inactive carrier: fmac_exit_supervision() will be enforced when
         a carrier is dropped and that carrier has entered DRC supervision. We
         will end up here.
         The logic for inactive carrier covers the following scenario:
             1. AT has c0, c1, c2
             2. c0 enters DRC supervision
             3. HHO to c1, c2. Basically c0 gets dropped
             4. c0 becames inactive carrier. FTCMAC would send a exit
                DRC supervsion for c0. 
             5. We need to start FC/DTX monitoring here since c1 and c2 are
             not in DRC supervision.
      */
      if (  hdrrmac4drv_no_carrier_in_drc_supervision() == TRUE )
      {
        hdrrmac_start_flexconn_dtx_monitor( TRUE );
      }
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */
    } /* if the channel exits DRC supervision */

  } /* for all RL physical channels */

  return;

} /* hdrrmac4_exit_drc_supervision */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_IS_FLOW_ACTIVE                                     EXTERNAL

DESCRIPTION
  Function to find out if a given flow is active. A flow is considered active
  if its BucketLevelMax attribute is > 0

DEPENDENCIES
  None.

PARAMETERS
  MAC flow ID in the range [0x0 - 0xF]

RETURN VALUE
  TRUE if flow is active; false otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_is_flow_active( uint8 flow )
{
  boolean rc = FALSE;

  if( flow < HDRRMAC_MAX_NUM_MAC_FLOWS && HDRRMAC4_IS_FLOW_ACTIVE( flow ) )
  {
    rc = TRUE;
  }

  return rc;
} /* hdrrmac4_is_flow_active */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_GET_FLOW_MAPPING                                   EXTERNAL

DESCRIPTION
  Returns the AssociatedFlowsNN attribute to caller. 

DEPENDENCIES
  None.

PARAMETERS
  mapping - Pointer to structure giving the set of upper layer flows mapped 
            to a given MAC flow. 
  flow_id - MAC Flow Id 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_get_flow_mapping
( 
  hdrrmac_flow_mapping_struct_type *mapping, 
  uint8 flow_id 
)
{

  /* Ensure that the given pointer is not NULL */
  ASSERT(mapping != NULL);

  /* Copy the AssociatedFlows<flow_id> instance */
#ifdef FEATURE_MEMCPY_REMOVAL
  *mapping = hdrrmac4_config_ptr->flow[flow_id].flow_mapping;
#else
  memcpy( mapping, 
          &(hdrrmac4_config_ptr->flow[flow_id].flow_mapping), 
          sizeof(hdrrmac_flow_mapping_struct_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

} /* hdrrmac4_get_flow_mapping */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_THROTTLING

DESCRIPTION
  This function updates the Reverse Link throttling state.
  (Note that normal operation has RL throttling on as per standard.)

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_update_throttling( void )
{
  hdrrmac4drv_update_throttling();
} /* hdrrmac4_update_throttling */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_GET_MAC_INFO                                       EXTERNAL

DESCRIPTION
  Returns MAC payload size equivalents (in bytes) for PA headroom, bucket
  level and T2PInflow to caller. 

DEPENDENCIES
  None.

PARAMETERS
  flow     - MAC Flow Id
  mac_info - Pointer to structure with a set of MAC payload size equivalents 
             for PA headroom, bucket level and T2PInflow

RETURN VALUE
  TRUE  - If the fuction succeeds in getting the requested values
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
#if 0
boolean hdrrmac4_get_mac_info
( 
  uint8 flow, 
  hdrrmac4_mac_info_type *mac_info 
)
{
  boolean rc = FALSE;

  uint32 bucket_level_payload_size32;  /* 32-bit temporary storage for       */
                            /* bucket_level_payload_size to prevent overflow */

  uint32 t2p_inflow_payload_size32;    /* 32-bit temporary storage for       */
                            /* t2p_inflow_payload_size to prevent overflow   */

  hdrrmac4_payload_size_enum_type ps                 = HDRRMAC4_0_BITS;
  hdrrmac4_payload_size_enum_type max_pa_headroom_ps = HDRRMAC4_0_BITS;

  if( flow < HDRRMAC4_MAX_NUM_MAC_FLOWS )
  {
    /* Entering critical section... */
    rex_enter_crit_sect(&mac_info_raw_data.mac_info_crit_sect);

    /* PA headroom limited maximum MAC payload size */

    /* Check PA headroom constraints */
    for( ps = HDRRMAC4_128_BITS; ps <= HDRRMAC4_12288_BITS; ps++ )
    {
      if( hdrrmac4_config_ptr->t2p_info[mac_info_raw_data.tm].tx_t2p_info[ps].\
            max_db <= mac_info_raw_data.max_tx_t2p )
      /* both max_db and max_tx_t2p are in 1/4 dB */
      {
        max_pa_headroom_ps = ps;
      }
    }

    if( max_pa_headroom_ps == HDRRMAC4_0_BITS)
    {
      mac_info->pa_headroom_payload_size = 0;
    }
    else
    {
      mac_info->pa_headroom_payload_size = hdrrmac4_pkt_size_array[\
               max_pa_headroom_ps];
    }

    /* Bucket Level equivalent MAC nominal payload size */

    if(mac_info_raw_data.bucket_level[flow] < 0) /* payload size cannot be negative */
    {
      mac_info->bucket_level_payload_size = 0;
    }
    else
    {
      /* bucket_level and t2p_conv_factor are in Q10 and Q6 formats      */
      /* respectively.  Hence >> 16 to obtain bucket_level_payload_size  */
      /* in Q0 format                                                    */

      bucket_level_payload_size32 = (uint32) ( ( ( ( (int64)
                                   mac_info_raw_data.bucket_level[flow] ) *
                                   mac_info_raw_data.t2p_conv_factor ) /
                                   mac_info_raw_data.tt ) >> 16 );

      if (bucket_level_payload_size32 <= HDRRMACCMN_MAX_PAYLOAD_SIZE) 
                          /* payload size cannot exceed max-payload size */
      {
        mac_info->bucket_level_payload_size = ( (uint16) \
                                              bucket_level_payload_size32 );
      }
      else
      {
        mac_info->bucket_level_payload_size = HDRRMACCMN_MAX_PAYLOAD_SIZE;
      }

    }

    /* Current T2PInflow equivalent MAC nominal payload size */

    /* t2p_inflow and t2p_conv_factor are in Q10 and Q6 formats        */
    /* respectively.  Hence >> 16 to obtain t2p_inflow_payload_size    */
    /* in Q0 format                                                    */

    t2p_inflow_payload_size32 = (uint32) ( ( ( (uint64) \
                                mac_info_raw_data.t2p_inflow[flow] ) *\
                                mac_info_raw_data.t2p_conv_factor ) >> 16 );

    if(t2p_inflow_payload_size32 <= HDRRMACCMN_MAX_PAYLOAD_SIZE)
                        /* payload size cannot exceed max-payload size */
    {
      mac_info->t2p_inflow_payload_size = ( (uint16) \
                                             t2p_inflow_payload_size32 );
    }
    else
    {
      mac_info->t2p_inflow_payload_size = HDRRMACCMN_MAX_PAYLOAD_SIZE;
    }

    /* Leaving critical section... */
    rex_leave_crit_sect(&mac_info_raw_data.mac_info_crit_sect);

    rc = TRUE;

    HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "mac_info->pa_headroom_payload_size = %d",
                   mac_info->pa_headroom_payload_size );

    HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "mac_info->bucket_level_payload_size = %d",
                   mac_info->bucket_level_payload_size );

    HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                  "mac_info->t2p_inflow_payload_size = %d",
                   mac_info->t2p_inflow_payload_size );

  }

  return rc;
} /* hdrrmac4_get_mac_info */
#endif /* 0 */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_GET_SLP_CARRIER_AND_TX_PILOT_PWR_INFO              EXTERNAL

DESCRIPTION
  Retrieve the current SLP carrier and the TX pilot power from all the 
  assigned reverse CDMA Channels.

DEPENDENCIES
  None.

PARAMETERS
  slp_carrier_ptr - Pointer to the SLP carrier structure to be filled.
  avg_tx_pilot_pwr_ptr - Pointer to the average TX pilot power variable to
                         be filled.  Variable is in unit of 0.5dBm in 2's
                         complement format.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_get_slp_carrier_and_tx_pilot_pwr_info
(
  sys_channel_type  *slp_carrier_ptr,
  int8              *total_tx_pilot_pwr_ptr
)
{

  /* set the flag to indicate that the SLP carrier switch is not allowed */
  hdrrmac4_var_ptr->disallow_slp_switch = TRUE;

  slp_carrier_ptr->band = 
    hdrrmac4_var_ptr->phy_chan[hdrrmac4_var_ptr->slp_carrier_idx].chan_record.band;
  slp_carrier_ptr->chan_num = 
    hdrrmac4_var_ptr->phy_chan[hdrrmac4_var_ptr->slp_carrier_idx].chan_record.chan_num;

  *total_tx_pilot_pwr_ptr = hdrrmac4drv_get_total_tx_pilot_pwr();

  return;

} /* hdrrmac4_get_slp_carrier_and_tx_pilot_pwr_info */


 /* EJECT */
#ifdef FEATURE_HDR_IS890A
/*===========================================================================
FUNCTION HDRRMAC4_SET_TARGET_PS_INDEX

DESCRIPTION
  This function is invoked by RETAP to set the maximum RRI value on the 
  specified channel.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index
  max_rri       - maximum RRI value.

RETURN VALUE
  rri_valid - TRUE if the specified RRI is valid; FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_set_target_ps_index
(
  uint8 phy_chan_idx,
  uint8 max_rri
)
{
  hdrrmaccmn_payload_size_enum_type tgt_ps_index;
  boolean                           rri_valid=FALSE;

  tgt_ps_index = (hdrrmaccmn_payload_size_enum_type) max_rri;

  if ((tgt_ps_index < HDRRMACCMN_MAX_PAYLOADS) || 
      (tgt_ps_index == HDRRMACCMN_IS856_A_RRI))
  {
    if (phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
    {
      rri_valid = TRUE;

      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].rtap_tgt_ps_index = 
        tgt_ps_index;
    }
  }

  return (rri_valid);

} /* hdrrmac4_set_target_ps_index */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_GET_SELECTED_PS_INDEX

DESCRIPTION
  This function is invoked by RETAP to get the payload size index selected 
  by RTCMAC4.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index

RETURN VALUE
  payload size index in the range [1..12]

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4_get_selected_ps_index
(
  uint8 phy_chan_idx
)
{
  if (phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS)
  {
    return ((uint8) hdrrmac4_var_ptr->phy_chan[phy_chan_idx].pkt_size_index);
  }
  else
  {
    return 0;
  }

} /* hdrrmac4_get_selected_ps_index */
#endif /* FEATURE_HDR_IS890A */


 /* EJECT */
/*=============================================================================
FUNCTION HDRRMAC4_UPDATE_GRID                                         INTERNAL

DESCRIPTION
  This function, given an interpolation structure (T2P axis, FRAB axis and
  T2P-FRAB grid), prepares and populates the grid parameters as expected by 
  the math function by scaling the values to appropriate precision. 
  This function is used populate T2PUp(), T2PDown() and BucketFactor() grids. 

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the grid to be populated (t2p_frab_grid), the interpolation grid 
  structure (grid_info) and a function that does the scaling of the grid 
  values, since T2PUp() and T2PDown() grids have different units than 
  BucketFactor().

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_update_grid
( 
  hdrrmac4_t2p_frab_grid_type           *t2p_frab_grid,
  hdrrmaccmn_t2p_frab_grid_struct_type  *grid_info,
  hdrrmaccmn_scaling_func_type           scaling_func
)
{
  uint8  t2p, frab; /* loop indices */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /* 1. Prepare T2P axis */
  t2p_frab_grid->t2p_axis.num_points = grid_info->t2p_axis.num_points;

  for( t2p = 0; t2p < t2p_frab_grid->t2p_axis.num_points; t2p++ )
  {
    /* Convert from 8-bit values in 0.25 dB units to signed 32-bit values
     * in dB units
     */
    t2p_frab_grid->t2p_axis.values[t2p] = 
      (int32) ( ( grid_info->t2p_axis.values[t2p]
                  * hdrrmaccmn_q_20_scaling_factor ) / 4 );

  }

  /* 2. Prepare FRAB axis */
  t2p_frab_grid->frab_axis.num_points = grid_info->frab_axis.num_points;

  for( frab = 0; frab < t2p_frab_grid->frab_axis.num_points; frab++ )
  {

    /* The FRABAxis attribute ranges from -1..7/8 in steps of 1/8th. Scale
     * it to Q.15 format
     */
    t2p_frab_grid->frab_axis.values[frab] =  
      (int32) ( ( grid_info->frab_axis.values[frab] * 
                  hdrrmaccmn_q_15_scaling_factor ) / 8 );

  }

  /* 3. Prepare T2P-FRAB grid */
  for( t2p = 0; t2p < t2p_frab_grid->t2p_axis.num_points; t2p++ )
  {
    for( frab = 0; frab < t2p_frab_grid->frab_axis.num_points; frab++ )
    {
      t2p_frab_grid->t2p_frab_grid[t2p][frab] = 
        scaling_func( (void *) &(grid_info->grid[t2p][frab]) );
    }
  }

} /* hdrrmac4_update_grid */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_BUCKET_FACTOR_GRID                         INTERNAL

DESCRIPTION
  This function populates the bucket factor grid.

DEPENDENCIES
  None.

PARAMETERS
  flow_id  - MAC flow ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_update_bucket_factor_grid
(
  uint8 flow_id
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  hdrrmac4_update_grid(
    &hdrrmac4_t2p_frab_grid_data.bucket_factor[flow_id],
    &hdrrmac4_config_ptr->flow[flow_id].bucket_factor,
    hdrrmac4_bucket_factor_scaling_func );

  return;
} /* hdrrmac4_update_bucket_factor_grid */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_GU_GD_GRIDS                                INTERNAL

DESCRIPTION
  This function populates the bucket factor grid.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
RMAC4_LOCAL void hdrrmac4_update_gu_gd_grids
(
  uint8 flow_id,
  uint8 phy_chan_idx
)
{
  uint8 log_chan_id;
    /* Logical channel id */
   
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  log_chan_id = hdrrmac4_from_phy_to_log( phy_chan_idx );

  ASSERT(log_chan_id < HDRSCMRMAC4_MAX_NUM_REV_LOG_CHANS);

  hdrrmac4_update_grid(
    &hdrrmac4_t2p_frab_grid_data.t2p_up[flow_id][phy_chan_idx],
    &hdrrmac4_config_ptr->nc[flow_id][log_chan_id].gu_gd_ptr->up,
    hdrrmac4_t2p_up_down_scaling_func );

  hdrrmac4_update_grid(
    &hdrrmac4_t2p_frab_grid_data.t2p_down[flow_id][phy_chan_idx],
    &hdrrmac4_config_ptr->nc[flow_id][log_chan_id].gu_gd_ptr->down,
    hdrrmac4_t2p_up_down_scaling_func );

  return;
} /* hdrrmac4_update_gu_gd_grids */


/*===========================================================================
FUNCTION HDRRMAC4_GET_CURRENT_TX_PACKET_INFO

DESCRIPTION
  Returns the currently transmitting RRI or packet size. 
  
DEPENDENCIES
  None.

PARAMETERS
  Point to packet info structure to be filled out.

RETURN VALUE
  TRUE if valid data was filled in. 
  FALSE if there was no data (due to PA not on, or RTCMAC not 
    active, for instance).

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_get_current_tx_packet_info
(
  hdrrmac_current_tx_packet_info_type * pkt_info
)
{
  return hdrrmac4drv_get_current_tx_packet_info(pkt_info);
}


/*===========================================================================
FUNCTION HDRRMAC4_START_FLOW_CONTROL

DESCRIPTION
  This function stops RMAC from making packets on non-SLP carriers.  
  hdrrmac4_stop_flow_control must be called to re-enable data on non-SLP 
  carriers.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_start_flow_control( void )
{

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "RL flow control: start" );

  hdrrmac4drv_start_flow_control();

} /* hdrrmac4_start_flow_control */


/*===========================================================================
FUNCTION HDRRMAC4_STOP_FLOW_CONTROL

DESCRIPTION
  This function brings RMAC out of flow control mode ( Calling 
  hdrrmac4_start_flow_control() puts RMAC in flow control mode ) and 
  re-enables data on non-SLP carriers.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_stop_flow_control( void )
{

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "RL flow control: stop" );

  hdrrmac4drv_stop_flow_control();
  
} /* hdrrmac4_stop_flow_control */

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)\
||defined(FEATURE_HDR_TRAFFIC_DTX)
/*===========================================================================
FUNCTION hdrrmac4_submit_dtx_log                              INTERNAL

DESCRIPTION
  This function submit the log packet of dtx info

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_submit_dtx_log( void )
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( hdrrmac4_dtx_log_info.sf_count > HDRLOG_MAX_SUBFRAMES_DTX_INFO_PKT )
  {
    hdrrmac4_dtx_log_info.sf_count = HDRLOG_MAX_SUBFRAMES_DTX_INFO_PKT;
  }
  /* Make sure there are entries to submit. */
  if ((hdrrmac4_dtx_log_info.sf_count != 0) && 
      (hdrrmac4_dtx_log_info.enabled == TRUE))
  {
    hdrrmac4_dtx_log_info.log.hdr.len     =
      sizeof(LOG_HDR_MC_DTX_INFO_C_type) -
      ((HDRLOG_MAX_SUBFRAMES_DTX_INFO_PKT  - 
        hdrrmac4_dtx_log_info.sf_count ) *
       ( sizeof( hdrlog_mc_dtx_info_sf_wide_type ) ) );

    /* Get time */
    (void) hdrts_get((unsigned long *) hdrrmac4_dtx_log_info.log.hdr.ts);

    hdrrmac4_dtx_log_info.log.hdr.code    = LOG_HDR_MC_DTX_INFO_C;
    hdrrmac4_dtx_log_info.log.subframe_count = 
                             hdrrmac4_dtx_log_info.sf_count;

    hdrrmac4_dtx_log_info.log.version = HDRLOG_MC_DTX_INFO_VERSION;

    (void) log_submit((log_type *) &hdrrmac4_dtx_log_info.log);

    hdrrmac4_dtx_log_info.sf_count = 0;
  }
}/* hdrrmac4_submit_dtx_log */

/*===========================================================================
FUNCTION hdrrmac4_log_dtx_info                              INTERNAL

DESCRIPTION
  This function logs the dtx info log packet

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_log_dtx_info( void )
{
  
  boolean                             status;                       
    /* Logging status */

  hdrlog_mc_dtx_info_sf_wide_type     *dtx_log_ptr;
    /* Pointer to log structure */

  int8                                ch;
    /* looping index */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check whether log packet is enabled. */
  status = log_status(LOG_HDR_MC_DTX_INFO_C);

  /* Reset count whenever log status changes. */
  if ( status != hdrrmac4_dtx_log_info.enabled )
  {
    hdrrmac4_dtx_log_info.enabled = status;
    hdrrmac4_dtx_log_info.sf_count = 0;
  }


  /* Log packet is enabled, so store the log data */
  if (hdrrmac4_dtx_log_info.enabled)
  {
    dtx_log_ptr = 
      & (hdrrmac4_dtx_log_info.log.dtx_info_per_sf[ hdrrmac4_dtx_log_info.sf_count ]);

    /* Get current subframe number and hstr */
    dtx_log_ptr->subframe_num = hdrrmac4_logging.sub_frame_count - 1;
    dtx_log_ptr->curr_hstr = HDRHSTR_GET_COUNT();
    /* Get data from SW */
    dtx_log_ptr->in_tap_call = hdrrmac4_common->is_tap_call;
    dtx_log_ptr->rl_active     = hdrrmac4drv_rl_is_active();
    dtx_log_ptr->mac_index_cnt = hdrrmac4_common->mac_indexes_cnt;
    dtx_log_ptr->first_dsc_duty_cycle = hdrrmac4_common->first_dsc_duty_cycle;
    dtx_log_ptr->dsc_length_in_slots = hdrfmac_get_dsc_length_in_slot();

#ifdef FEATURE_HDR_FLEX_CONN
    dtx_log_ptr->fc_enabled_sw = hdrrmac4_common->flexconn_mode; 
    dtx_log_ptr->fc_monitor_started = hdrrmac4_common->flexconn_monitor_started;
    dtx_log_ptr->is_repointing_msg_rcvd = hdrrmac4_common->is_repointing_msg_rcvd;
    dtx_log_ptr->fl_active_flexconn = hdrrmac4_common->is_fl_active_flexconn;
#else
    dtx_log_ptr->fc_enabled_sw = FALSE; 
    dtx_log_ptr->fc_monitor_started = FALSE;
#endif /* FEATURE_HDR_FLEX_CONN */

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
    dtx_log_ptr->sdtx_enabled = hdrrmac4_common->device_only_dtx_mode; 
    dtx_log_ptr->dtx_monitor_started = hdrrmac4_common->dtx_monitor_started;
    dtx_log_ptr->fl_active_sdtx = hdrrmac4_common->is_fl_active_dtx;
#else
    dtx_log_ptr->sdtx_enabled = FALSE;
    dtx_log_ptr->dtx_monitor_started = FALSE;
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_REVB_DTX_MODE
    dtx_log_ptr->ndtx_enabled = hdrrmac4_common->network_dtx_mode;
#else
    dtx_log_ptr->ndtx_enabled = FALSE;
#endif /* FEATURE_HDR_REVB_DTX_MODE */

#ifdef FEATURE_HDR_TRAFFIC_DTX
    dtx_log_ptr->tdtx_enabled = hdrrmac4_common->tdtx_mode;
#else
    dtx_log_ptr->tdtx_enabled = FALSE; 
#endif /* FEATURE_HDR_TRAFFIC_DTX */
    dtx_log_ptr->cxm_allows_dtx = hdrrmac4_common->cxm_allows_dtx;
    dtx_log_ptr->drc_length_in_slot = hdrrmac4_common->dtx_drc_length;
    dtx_log_ptr->dtx_frame_offset_sw= hdrrmac4_common->dtx_frame_offset;
    dtx_log_ptr->drc_boost = hdrrmac4_common->dtx_drc_boost;

    for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      dtx_log_ptr->dtx_info_per_chan[ch].carrier_id = ch;
      dtx_log_ptr->dtx_info_per_chan[ch].frab = 
            hdrrmac4_var_ptr->phy_chan[ch].sector_info.frab;

    }
    /* Update subframe count and submit log if needed. */
    hdrrmac4_dtx_log_info.sf_count++;
    if ( hdrrmac4_dtx_log_info.sf_count == HDRLOG_MAX_SUBFRAMES_DTX_INFO_PKT )
    {
        /* Submit log */
      hdrrmac4_submit_dtx_log();
    }
  }
} /* hdrrmac4_log_dtx_info */

/*===========================================================================
FUNCTION hdrrmac4_submit_flexconn_dtx_log                              INTERNAL

DESCRIPTION
  This function submit the log packet of flexconn info and dtx info

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_submit_flexconn_dtx_log( void )
{
  hdrrmac4_submit_dtx_log();

}/* hdrrmac4_submit_flexconn_dtx_log */

/*===========================================================================
FUNCTION hdrrmac4_init_flexconn_dtx_log_info                         INTERNAL

DESCRIPTION
  This function initialize the structure to control flexconn or dtx log packet

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_init_flexconn_dtx_log_info( void )
{
  hdrrmac4_dtx_log_info.sf_count = 0;
  hdrrmac4_dtx_log_info.enabled = FALSE;
} /* hdrrmac4_init_flexconn_dtx_log_info */

/*==========================================================================

FUNCTION  HDRRMAC4_FLEXCONN_CHECK_FRAB_THRESH_1

DESCRIPTION
  Check whether FRAB of all carriers are less than the busy threshold 1
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  whether FRAB of all carriers are less than the busy threshold

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrrmac4_flexconn_check_frab_thresh_1( void )
{
  uint8 chan_index;
  uint8 chan;
  hdrrmac4_phy_chan_vars_struct_type *chan_ptr;
  boolean frab_check = TRUE;
  int16 frab_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (chan_index = 0; 
        chan_index < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
        chan_index++ )
  {
    frab_list[chan_index]= 0;
  }
  for (chan_index = 0; 
       chan_index < hdrrmac4_var_ptr->num_open_state_chans; 
       chan_index++)
  {
    chan = hdrrmac4_open_rl_list[chan_index];
    chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];
    frab_list[chan_index] = chan_ptr->sector_info.frab;
    if ( chan_ptr->sector_info.frab >= HDRRMAC_FLEXCONN_FRAB_BUSY_THRESH_1 )
    {
      frab_check = FALSE;
      break;
    }
  }
  HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
      "FLEXCONN check_frab_thresh_1: chan_num %d FRAB %d %d %d",
      hdrrmac4_var_ptr->num_open_state_chans,
      frab_list[0],frab_list[1],frab_list[2] );

   return frab_check;
} /* hdrrmac4_flexconn_check_frab_thresh_1 */

/*==========================================================================

FUNCTION  HDRRMAC4_FLEXCONN_CHECK_FRAB_THRESH_2

DESCRIPTION
  Check whether FRAB of all carriers are greater than the busy threshold 2
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  whether FRAB of all carriers are greater than the busy threshold 2

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrrmac4_flexconn_check_frab_thresh_2( void )
{
  uint8 chan_index;
  uint8 chan;
  hdrrmac4_phy_chan_vars_struct_type *chan_ptr;
  boolean frab_check = TRUE;
  int16 frab_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (chan_index = 0; 
        chan_index < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
        chan_index++ )
  {
    frab_list[chan_index]= 0;
  }
  for (chan_index = 0; 
       chan_index < hdrrmac4_var_ptr->num_open_state_chans; 
       chan_index++)
  {
    chan = hdrrmac4_open_rl_list[chan_index];
    chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];
    frab_list[chan_index] = chan_ptr->sector_info.frab;
    if ( chan_ptr->sector_info.frab <= HDRRMAC_FLEXCONN_FRAB_BUSY_THRESH_2 )
    {
      frab_check = FALSE;
      break;
    }
  }

  HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
      "FLEXCONN check_frab_thresh_2: chan_num %d FRAB %d %d %d",
      hdrrmac4_var_ptr->num_open_state_chans,
      frab_list[0],frab_list[1],frab_list[2] );
   return frab_check;
} /* hdrrmac4_flexconn_check_frab */

/*==========================================================================

FUNCTION  HDRRMAC4_FLEXCONN_DTX_MONITOR

DESCRIPTION
  Check whether to turn on FlexConn or DTX mode 
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac4_flexconn_dtx_monitor( void )
{
  /* whether RL is active */
  boolean rl_active;

#ifdef FEATURE_HDR_FLEX_CONN
  uint16  dsc_length_in_sf = 0;
#endif /* FEATURE_HDR_FLEX_CONN */

#ifdef FEATURE_HDR_TRAFFIC_DTX
  uint8 num_carriers = 0;
#endif /* FEATURE_HDR_TRAFFIC_DTX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ( hdrrmac4_common->is_tap_call == TRUE )&&
       (
         #ifdef FEATURE_HDR_FLEX_CONN
         ( hdrrmac4_common->flexconn_monitor_started == TRUE )
         #endif /* FEATURE_HDR_FLEX_CONN */
         #if defined(FEATURE_HDR_FLEX_CONN)&&defined(FEATURE_HDR_DEVICE_ONLY_DTX)
         ||
         #endif /* FEATURE_HDR_FLEX_CONN && FEATURE_HDR_DEVICE_ONLY_DTX */
         #ifdef FEATURE_HDR_DEVICE_ONLY_DTX
         ( hdrrmac4_common->dtx_monitor_started == TRUE )
         #endif /* FEATURE_HDR_DEVICE_ONLY_DTX */
       )
     )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "FLEXCONN/DTX: stop flexconn_dtx_monitor for TAP call");
    hdrrmac_stop_flexconn_dtx_monitor( FALSE );
  }

#ifdef FEATURE_HDR_FLEX_CONN
  if ( ( hdrrmac4_common->flexconn_monitor_started == TRUE ) &&
       ( hdrrmac4_common->is_flexconn_nv_enabled  ) )
  {
    rl_active = hdrrmac4drv_rl_is_active();
    if ( rl_active)
    {
      hdrrmac4_common->rl_inactive_sf_cnt = 0;
        /* reset reverse link inactive counter */
    }
    else 
    {
      /* Count reverse link inactive time when flexconn mode is not enabled */

      if ( hdrrmac4_common->rl_inactive_sf_cnt <= HDRRMAC_FLEXCONN_RL_INACTIVE_TIMEOUT_SF_CNT )
      hdrrmac4_common->rl_inactive_sf_cnt++;
        /* Saturate rl_inactive_sf_cnt to HDRRMAC_FLEXCONN_RL_INACTIVE_TIMEOUT_SF_CNT+1. 
           The in the check below we will
           still consider RL is inactive. Otherwise we may fail the RL inactive check
           below and disable FlexConn mode. This is not correct.
        */
    }

    dsc_length_in_sf = hdrfmac_get_dsc_length_in_slot() >> 2;
      /* get dsc length in subframe */

    if ( hdrrmac4_common->first_dsc_duty_cycle == TRUE )
    {
      hdrrmac4_common->rl_sf_cnt++;

      if ( hdrrmac4_common->rl_sf_cnt >= dsc_length_in_sf )
      {
        hdrrmac4_common->first_dsc_duty_cycle = FALSE;
      }
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH,
          "FlexCONN:  rl_sf_cnt %d no fc enabled in first_dsc_duty_cycle, rl_inactive_sf_cnt=%d",
          hdrrmac4_common->rl_sf_cnt,hdrrmac4_common->rl_inactive_sf_cnt
                     );
    }
    else if ( hdrrmac4_common->flexconn_disabled_before_tx_stop == TRUE )
    {
      /* FlexConn mode is disabled 80 slots before ta/ofs, or 2 dsc length before RLSI.
         We don't need to check if we need to enable/disable flexconn mode.
      */
      hdrrmac4_common->rl_sf_cnt++;
        /* Number of subfram in FC disabled period increases */

      HDR_MSG_PROT_2( MSG_LEGACY_HIGH,
         "FLEXCONN: monitoring,flexconn disabled before ta/ofs/rlsi,rl_inactive_sf_cnt=%d,rl_sf_cnt=%d",
         hdrrmac4_common->rl_inactive_sf_cnt, hdrrmac4_common->rl_sf_cnt );
    }
    else
    {
      if ( hdrrmac4_common->flexconn_mode == FALSE )
      {
        hdrrmac4_common->rl_sf_cnt++;
          /* Number of subfram in FC disabled period increases */
      }

      HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
                   "FLEXCONN: monitoring. max_inx_cnt=%d fl_active_fc=%d rl_inactive_sf_cnt=%d rl_sf_cnt=%d ",
                   hdrrmac4_common->mac_indexes_cnt,
                   hdrrmac4_common->is_fl_active_flexconn, 
                   hdrrmac4_common->rl_inactive_sf_cnt,
                   hdrrmac4_common->rl_sf_cnt
                   );

      if ( ( hdrrmac4_common->is_fl_active_flexconn == FALSE ) &&
           ( hdrrmac4_common->rl_inactive_sf_cnt >= HDRRMAC_FLEXCONN_RL_INACTIVE_TIMEOUT_SF_CNT ) &&
           ( ( hdrrmac4_common->mac_indexes_cnt <= HDRRMAC_FLEXCONN_MAX_ACTIVE_MAC_INDEXES ) ||
             ( hdrrmac4_flexconn_check_frab_thresh_1() == TRUE ) ) &&
           ( hdrrmac4_common->rl_sf_cnt >= dsc_length_in_sf )  )
      { 
        /* Reverse link is not active now. Foward link is not active either,
           turn on FLEXCONN mode
        */
        //hdrrmac4_common->rl_inactive_sf_cnt = 0;
        hdrrmac4drv_set_flexconn_mode( TRUE );
      }
      else if ( ( rl_active == TRUE ) || 
                ( ( hdrrmac4_common->mac_indexes_cnt > HDRRMAC_FLEXCONN_MAX_ACTIVE_MAC_INDEXES )&&
                  ( hdrrmac4_flexconn_check_frab_thresh_2() == TRUE ) )
              )
      {
        /* FC mode is upated when FL active notification is received. Therefor we need to
           disable FC in cases that other conditions are not met
         */
        hdrrmac4drv_set_flexconn_mode( FALSE );
      }
    } 
  } /* ( hdrrmac4_common->flexconn_monitor_started == TRUE ) &&
       ( hdrrmac4_common->is_flexconn_nv_enabled  )
     */
#endif /* FEATURE_HDR_FLEX_CONN */


#if defined(FEATURE_HDR_DEVICE_ONLY_DTX)||defined(FEATURE_HDR_TRAFFIC_DTX)
  /* Check with CXM if it is ok to allow DTX based
     on DSDA band combinations. */
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
  if ( hdrmultirat_cxm_allows_dtx() )
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */
  {
    hdrrmac4_common->cxm_allows_dtx = TRUE;
  #ifdef FEATURE_HDR_DEVICE_ONLY_DTX
    
    if ( ( hdrrmac4_common->dtx_monitor_started == TRUE ) && 
         ( hdrrmac4_common->is_dtx_nv_enabled ) )
    { 

      rl_active = hdrrmac4drv_rl_is_active();

      HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
                     "DTX monitoring. fl_active_dtx=%d rl_active=%d,"
                     "frame_offset=%d,drc_length=%d",
                     hdrrmac4_common->is_fl_active_dtx, rl_active,
                     hdrrmac4_common->dtx_frame_offset,
                     hdrrmac4_common->dtx_drc_length );

      if ( ( hdrrmac4_common->is_fl_active_dtx == FALSE ) &&
           ( rl_active == FALSE ) &&
           ( hdrrmac4_common->dtx_drc_length >= 
             HDRRMAC_DTX_DRC_LENGTH_MIN_IN_SLOTS ) )
      { 
        /*  Reverse link is not active now. Foward link is not active either.
            dtx_frame_offset is valid which means the combination of
            frame_offset and sector type(csm6800 or csm6850) allows dtx
            being enabled.
        */
        hdrrmac4_common->device_only_dtx_mode = TRUE;
      }
      else if ( rl_active == TRUE )
      {
        /* DTX mode is updated when fl active nofication is received, or when
           frame_offset/drc_length is changed after soft HO. Therefore we only
           need to check if reverse link is back to active and then disable dtx
        */
        hdrrmac4_common->device_only_dtx_mode = FALSE;
      }

      hdrrmac4_common->device_only_dtx_mode &= ( !hdrrmaccmn_is_dtx_tx_limited( ) );
    } /* ( hdrrmac4_common->dtx_monitor_started == TRUE ) &&
         ( hdrrmac4_common->is_dtx_nv_enabled )
      */
  #endif /* FEATURE_HDR_DEVICE_ONLY_DTX */

  #ifdef FEATURE_HDR_TRAFFIC_DTX

    /* Check if following conditions hold true turn on tDTX:
       1. If not in tap call
       2. If tDTX is supported via NV
       3. If we are done with connection setup
       4. If DRC length is > 4.
       5. Limited to RAB set to less than 20% of time -0.6 in [-1,1] scale 
       6. If only one carrier is active */

    num_carriers = HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS - 
      hdrrmac4_var_ptr->num_inactive_state_chans;

    if ( !hdrrmac4_common->is_tap_call && hdrrmac4_common->tdtx_nv_is_enabled 
         && ( hdrrmac4_common->state == HDRRMAC_OPEN_STATE )
         && ( hdrrmac4_common->dtx_drc_length >= 
           HDRRMAC_DTX_DRC_LENGTH_MIN_IN_SLOTS ) && 
         ( hdrrmac4_check_tdtx_frab_thresh() ) &&
         ( num_carriers == 1 ) )
    {
      hdrrmac4_common->tdtx_mode = TRUE;
    }
    else
    {
      hdrrmac4_common->tdtx_mode = FALSE;
    }

    HDR_MSG_PROT_6( MSG_LEGACY_HIGH,  "tDTX: tdtx_mode: %d nv_enabled: %d"
                    " drc_length: %d tap: %d, rmac_state %d, num carriers %d ",
                    hdrrmac4_common->tdtx_mode,
                    hdrrmac4_common->tdtx_nv_is_enabled,
                    hdrrmac4_common->dtx_drc_length,
                    hdrrmac4_common->is_tap_call,
                    hdrrmac4_common->state,
                    num_carriers );
   
  #endif /* FEATURE_HDR_TRAFFIC_DTX */
   
  }
#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "HDR DSDA DTX: cxm does not allow dtx!" );
    hdrrmac4_common->cxm_allows_dtx = FALSE;
#ifdef FEATURE_HDR_TRAFFIC_DTX
    hdrrmac4_common->tdtx_mode = FALSE;
#endif /* FEATURE_HDR_TRAFFIC_DTX */
#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
    hdrrmac4_common->device_only_dtx_mode = FALSE;
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */
  }
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */
#endif /* defined(FEATURE_HDR_DEVICE_ONLY_DTX)||defined(FEATURE_HDR_TRAFFIC_DTX) */


  if ( hdrrmac4_common->is_dtx_nv_enabled || 
       hdrrmac4_common->is_flexconn_nv_enabled
#ifdef FEATURE_HDR_TRAFFIC_DTX
       || hdrrmac4_common->tdtx_nv_is_enabled
#endif 
     )
  {
    hdrrmac4_log_dtx_info();
  }

  hdrrmac4drv_update_dtx_mode();

  hdrrmac4_common->is_repointing_msg_rcvd = FALSE;
    /* Reset repointing bool after we hv logged it for that subframe */


} /* hdrrmac4_flexconn_dtx_monitor */

#endif /* defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)||defined(FEATURE_HDR_TRAFFIC_DTX)*/

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
/*===========================================================================
FUNCTION HDRRMAC4_GET_FRAME_OFFSET                                   EXTERNAL

DESCRIPTION
  This function gets the RMAC frame offset.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  uint8 frame_offset.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4_get_frame_offset ( void )
{
  return ( hdrrmac4_var_ptr->frame_offset );
}/* hdrrmac4_get_frame_offset */

/*==========================================================================

FUNCTION  HDRRMAC4_UPDATE_DTX_MODE

DESCRIPTION
  This function call driver to send a message to FW to enable or disable
  DTX mode
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac4_update_dtx_mode( void )
{
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  hdrrmac4drv_update_dtx_mode();  

} /* hdrrmac4_update_dtx_mode */
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */


/*===========================================================================
FUNCTION HDRRMAC4_CONFIG_FUNNEL_MODE

DESCRIPTION
  This function configures funnel mode in FW.

DEPENDENCIES
  None.

PARAMETERS
  enable - TRUE  => Enable Funnel mode
           FALSE => Disable funnel mode
  data_active_carrier_idx 
         - Index of data active carrier to be conveyed to FW

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4_config_funnel_mode
(
  boolean  enable,
  uint8    data_active_carrier_idx
)
{
  hdrrmac4drv_config_funnel_mode(enable, data_active_carrier_idx);
}

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION
/*===========================================================================
FUNCTION HDRRMAC4_GET_MIN_TERM_TARGET                                EXTERNAL

DESCRIPTION
  This function gets the minimum termination target across both 
  transmission modes and all packet sizes

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  uint8 min_term_target.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4_get_min_term_target ( void )
{
   /* packet size loop counter */
  uint8 ps = 0;

  /* Intialize min term target to max possible */
  uint8  min_term_target = HDRRMACCMN_MAX_NUM_SUBPACKETS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  for( ps = (uint8)HDRRMACCMN_128_BITS; 
         ps <= (uint8)HDRRMACCMN_12288_BITS; ps++)
  {
    min_term_target = MIN ( min_term_target,
      MIN( hdrrmac4_config_ptr->t2p_info[HDRRMACCMN_TX_MODE_LO_LAT]
           .tx_t2p_info[ps].term_target,
           hdrrmac4_config_ptr->t2p_info[HDRRMACCMN_TX_MODE_HI_CAP]
           .tx_t2p_info[ps].term_target ) );
     
  }

  return ( min_term_target );
} /* hdrrmac4_get_min_term_target */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION */

#ifdef FEATURE_HDR_MODEM_API
/*==========================================================================

FUNCTION  HDRRMAC4_REPORT_COST_OF_MODEM_POWER_EST

DESCRIPTION
  This function calculates and reports estimated cost of modem power
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Estimated cost of modem power LOW, MEDIUM, HIGH

SIDE EFFECTS
  None

==========================================================================*/
hdrrmac_cost_of_power_est_enum_type 
hdrrmac4_report_cost_of_modem_power_est( void )
{
   int16 tx_open_loop, filt_txp, k, nominal_pwr;
   int32 drc_gain, ack_gain, rri_gain, t2p;
   int32 tx_pilot_power, tx_pilot_power_lin, pilot_ovhd_t2p_lin;
   int32 tx_total_power, tx_total_power_lin;
   int64 tmp_tx_total_power_lin;
   int32 l_bound_q10, u_bound_q10;

   /* Initialize to invalid value */
   hdrrmac_cost_of_power_est_enum_type cost_of_power_est = 
      HDRRMAC_EST_COST_OF_POWER_IS_INVALID;

   /* RRI Gain for Rev B AT is 
      RTCMAC3:RRIChannelPowerParameters:RRIChannelGainPreTransition2
      rri_gain is in dB, convert to 0.25 dB (Q2) */
   rri_gain = 
      (hdrrmac4_config_ptr->rri_chan_power_params[2].
       rri_chan_gain_pre_transition) << 2;

   /* T2P for Rev A is
      RTCMAC3:PowerParameters1024:T2PHiCapPreTransition1024
      t2p is in 0.25 dB, i.e Q.2 */
   t2p = hdrrmac4_config_ptr->t2p_info[HDRRMACCMN_TX_MODE_HI_CAP].
      tx_t2p_info[HDRRMACCMN_1024_BITS].data_t2p_db[0];

   /* Use the RMAC state to see if AT is in IDLE or CONNECTED
      Note - This API gives power estimation only when AT is in these states
      It is expected that prior to executing the command for 
      Cost of Modem Power estimate, caller checks AT is in Idle or Connected 
      state. Here we are only identifying which one of these states AT is in.
      If RMAC state is HDRRMAC_INACTIVE_STATE, then treat AT is in idle
      If RMAC state is HDRRMAC_OPEN_STATE or HDRRMAC_SETUP_STATE,
      then treat AT is in connected. */

   /* If AT is in IDLE state */
   if(hdrrmac4_common->state == HDRRMAC_INACTIVE_STATE)
   {
      /* Get TxPilotPower
         Since AT is Idle state, here is how we calculate that
         Use the Measured RxAGCs filtered over the entire wakeup period with 
         a time constant of RxAGC_TC (e.g., 4 half slots), 
         denoted as avgRxAGC (in dBm);
         Then Map avgRxAGC to TxPilotPwr (in dBm) using open-loop equation
         We get this directly from the API rfm_hdr_get_tx_open_loop */

      rfm_device_enum_type rf_dev;

      /* DRC Channel Gain for idle AT in 0.25 dB (Q.2) */
      drc_gain = HDRRMAC_IDLE_DRC_GAIN_FOR_POWER_COST_EST;

      /* ACK Channel Gain for idle AT in 0.25 dB (Q.2) */
      ack_gain = HDRRMAC_IDLE_ACK_GAIN_FOR_POWER_COST_EST;

      rf_dev = hdrsrchrf_get_rf_dev(HDRSRCHRF_PRI_CHAIN);

      /* What we get below is in dB256 (Q.8) format on success */
      tx_open_loop = rfm_hdr_get_tx_open_loop ( rf_dev, 
                                 RFM_HDR_CARRIER_ID__0, 
                                 RFM_HDR_AGC_FORMAT__DBM256 );

      #ifdef FEATURE_HDR_MODEM_API_UT
      HDR_MSG_PROT_1(
                 MSG_LEGACY_ERROR,"FHMA: tx_open_loop(Q.8)=%d",tx_open_loop);
      #endif /* FEATURE_HDR_MODEM_API_UT */

      //* tx_pilot_power in (Q.10) units */
      tx_pilot_power = tx_open_loop << 2;

      /* Input is Q.10 in dBm. Output is Q.22 in mw */
      tx_pilot_power_lin = hdrrmacmath_dbm_2_mw( tx_pilot_power );

      /* PilotPlusOvhd_linear = 1 + DRCGain_linear + ACKGain_linear
                                  + RRIGain_linear + T2P_linear */   
       
      /* Scale up all inputs for dbm_2_mw to Q.10, Output is Q.22 linear */
      pilot_ovhd_t2p_lin = ( 1<<22 ) + 
                              hdrrmacmath_dbm_2_mw( ( drc_gain)<<8 ) +
                              hdrrmacmath_dbm_2_mw( ( ack_gain)<<8 ) +
                              hdrrmacmath_dbm_2_mw( ( rri_gain)<<8 ) +
                              hdrrmacmath_dbm_2_mw( ( t2p)<<8 );

      /* TxTotalPwr_linear = TxPilotPwr_linear * PilotOvhdT2P_linear */

      /* tx_pilot_power_lin and pilot_ovhd_t2p_lin are in Q.22
      multiplied value will be of Q.44 */
      tmp_tx_total_power_lin = 
         (int64)tx_pilot_power_lin * (int64)pilot_ovhd_t2p_lin ;

      /* Scale down tmp_tx_total_power_lin from Q.44 to Q.22 */
      tx_total_power_lin = (int32)(tmp_tx_total_power_lin >> 22);

      #ifdef FEATURE_HDR_MODEM_API_UT
      HDR_MSG_PROT_8(
                 MSG_LEGACY_ERROR,
                 "FHMA: drc_gain(Q.2)=%d, ack_gain(Q.2)=%d, rri_gain(Q.2)=%d, "
                 "t2p(Q.2)=%d, tx_pilot_power(Q.10)=%d, "
                 "tx_pilot_power_lin(Q.22)=%d, pilot_ovhd_t2p_lin(Q.22)=%d, "
                 "tx_total_power_lin(Q.22)=%d",
                 drc_gain, ack_gain, rri_gain, t2p, tx_pilot_power, 
                 tx_pilot_power_lin, pilot_ovhd_t2p_lin, tx_total_power_lin);
      #endif /* FEATURE_HDR_MODEM_API_UT */
   }
   else /* AT is in CONNECTED state */
   {
      int8 carrier_idx;

      tx_total_power_lin = 0;

      /* Call the below API to get the Constant 'k' (turnaround) used in
         converting values from DSP units to dBm */
      rfm_hdr_get_turnaround_info(hdrsrchrf_get_rf_dev(HDRSRCHRF_PRI_CHAIN), 
                                 &k, 
                                 &nominal_pwr );

      #ifdef FEATURE_HDR_MODEM_API_UT
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"FHMA: Turn around Const k=%d", k);
      #endif /* FEATURE_HDR_MODEM_API_UT */

      for (carrier_idx = 0; 
            carrier_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; carrier_idx++)
      {
         if (!HDRRMAC4_IS_RL_OPEN( carrier_idx ))
      {     
            /* If the RL is not in OPEN state for this carrier, skip this
               carrier in calculating the estimate */
            continue;
         }

         /* DRC Gain for Connected AT is 
            RUP:TrafficChannelAssignment:DRCChannelGainBase[sas] (Rev B)
            drc_gain is in 0.25 dB, i.e Q.2 */
         drc_gain = hdrfmac_get_drc_channel_gain(carrier_idx);

         /* If we fail to retrieve, use the same default gain value used in Idle
            estimation case */
         if (drc_gain == HDRFMAC_CHAN_GAIN_INVALID)
         {
            drc_gain = HDRRMAC_IDLE_DRC_GAIN_FOR_POWER_COST_EST;
         }

         /* ACK Gain for Connected AT is
            RUP:TrafficChannelAssignment:ACKChannelGain[sas] (Rev B)
            ack_gain is in 0.25 dB, i.e Q.2 */
         ack_gain = hdrfmac_get_ack_channel_gain(carrier_idx);

         /* If we fail to retrieve, use the same default gain value used in Idle
            estimation case */
         if (ack_gain == HDRFMAC_CHAN_GAIN_INVALID)
         {
            ack_gain = HDRRMAC_IDLE_ACK_GAIN_FOR_POWER_COST_EST;
         }

        /* In Connected state we can directly use FiltTxPilotPower from
            MDSP shared memory (Convert the 32bit FW value to 16bit) */

         #ifdef FEATURE_HDR_MODEM_API_UT
         HDR_MSG_PROT_2(
                    MSG_LEGACY_ERROR,"FHMA: Carrier=%d, pilotPowerFilt(DSP)=%d",
                    carrier_idx, 
                    hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
            rmacInfo[carrier_idx].txAgc.pilotPowerFilt);
         #endif /* FEATURE_HDR_MODEM_API_UT */

         filt_txp = 
            (hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
            rmacInfo[carrier_idx].txAgc.pilotPowerFilt) >> 16;

         /* Convert FW DSP values to dBm (Q.10) */
         if (filt_txp >= 0)
         {
           tx_pilot_power = (k << 10) + 65331 -
             ( ( ( filt_txp * 819 ) + (((int16)1) << 8) ) >> 9 );
         }
         else
         {
           tx_pilot_power = (k << 10) + 65331 -
             ( ( ( filt_txp * 819 ) - (((int16)1) << 8) ) >> 9 );
         }

         /* Input is Q.10 in dBm. Output is Q.22 in mw */
         tx_pilot_power_lin = hdrrmacmath_dbm_2_mw( tx_pilot_power );

         /* PilotPlusOvhd_linear = 1 + DRCGain_linear + ACKGain_linear
                                     + RRIGain_linear + T2P_linear */   
          
         /* Scale up all inputs for dbm_2_mw to Q.10, Output is Q.22 linear */
         pilot_ovhd_t2p_lin = ( 1<<22 ) + 
                                 hdrrmacmath_dbm_2_mw( ( drc_gain)<<8 ) +
                                 hdrrmacmath_dbm_2_mw( ( ack_gain)<<8 ) +
                                 hdrrmacmath_dbm_2_mw( ( rri_gain)<<8 ) +
                                 hdrrmacmath_dbm_2_mw( ( t2p)<<8 );

         /* TxTotalPwr_linear = TxPilotPwr_linear * PilotOvhdT2P_linear */

         /* tx_pilot_power_lin and pilot_ovhd_t2p_lin are in Q.22
            multiplied value will be of Q.44 */
         tmp_tx_total_power_lin = 
            (int64)tx_pilot_power_lin * (int64)pilot_ovhd_t2p_lin ;

         /* Scale down tmp_tx_total_power_lin from Q.44 to Q.22 and add to
            previous carrier aggregated total */
         tx_total_power_lin += (int32)(tmp_tx_total_power_lin >> 22);

         #ifdef FEATURE_HDR_MODEM_API_UT
         HDR_MSG_PROT_8(
                    MSG_LEGACY_ERROR,
                    "FHMA: drc_gain(Q.2)=%d, ack_gain(Q.2)=%d, rri_gain(Q.2)=%d, "
                    "t2p(Q.2)=%d, tx_pilot_power(Q.10)=%d, "
                    "tx_pilot_power_lin(Q.22)=%d, pilot_ovhd_t2p_lin(Q.22)=%d, "
                    "tx_total_power_lin(Q.22)=%d",
                    drc_gain, ack_gain, rri_gain, t2p, tx_pilot_power, 
                    tx_pilot_power_lin, pilot_ovhd_t2p_lin, tx_total_power_lin);
         #endif /* FEATURE_HDR_MODEM_API_UT */
      }
   } 
   
   /* tx_total_power_lin is in Q.22, Output value from mw_2_dbm is in Q.10 */
   tx_total_power = hdrrmacmath_mw_2_dbm(tx_total_power_lin);

  /* Convert lower and upper bound values to Q.10 for cost estimate comparison*/
   l_bound_q10 = HDRRMAC_COST_OF_POWER_L_BOUND << 10;
   u_bound_q10 = HDRRMAC_COST_OF_POWER_U_BOUND << 10;

   /* Below comparison is between two Q.10 entities */
   if (tx_total_power < l_bound_q10)
   {
     cost_of_power_est = HDRRMAC_EST_COST_OF_POWER_IS_LOW;
   }
   else if ( (tx_total_power >= l_bound_q10) &&
             (tx_total_power <= u_bound_q10) )
   {
     cost_of_power_est = HDRRMAC_EST_COST_OF_POWER_IS_MEDIUM;
   }
   else
   {
     cost_of_power_est = HDRRMAC_EST_COST_OF_POWER_IS_HIGH;
   }

   #ifdef FEATURE_HDR_MODEM_API_UT
   /* TODO: Need to update global structure holding all the
     Modem API results
     For now, to do Unit Testing just print all the values */
   HDR_MSG_PROT_2(
              MSG_LEGACY_ERROR,
              "FHMA: tx_total_power(Q.10)=%d, cost_of_power_est=%d",
              tx_total_power, cost_of_power_est);
   #endif /* FEATURE_HDR_MODEM_API_UT */

   return cost_of_power_est;

}
#endif /* FEATURE_HDR_MODEM_API */


#ifdef FEATURE_HDR_MODEM_API_RL_INFO
/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_RL_DATA_RATE_EST

DESCRIPTION
  Updates estimated reverse link throughput. The throughput value is in kbps.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrmac4_update_rl_data_rate_est( void )
{
  uint8  phy_chan_idx;              /* Channel loop index */
  uint64 rl_tput_est_ch;            /* Reverse link throughput estimate for a 
                                       channel */
  uint64 rl_tput_est = 0;           /* Total reverse link throughput 
                                       estimate */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( phy_chan_idx = 0;
    phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
    phy_chan_idx++ )
  {
    if( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state == 
      HDRRMAC4_CHAN_OPEN_STATE )
    {
      /* Obtain throughput in Q24.8 kbps units based on not-data-limited
         packets. */
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].not_data_lmtd_tput = 
        hdrrmac4drv_get_not_data_lmtd_tput( phy_chan_idx );

      /* Calculate -
         RLTputEst = ( NotDataLmtdFactor * NotDataLmtdTput +
           ( 1 - NotDataLmtdFactor ) * MacLmtdTput ) * TxOnFactor
         in Q24.8 units.
      */ 
      rl_tput_est_ch = ( ( ( (uint64) (hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
        not_data_lmtd_factor >> 8) * hdrrmac4_var_ptr->phy_chan[phy_chan_idx].
        not_data_lmtd_tput + (uint64) ( (1 << 8) - (hdrrmac4_var_ptr->phy_chan[
        phy_chan_idx].not_data_lmtd_factor >> 8) ) * hdrrmac4_var_ptr->phy_chan[
        phy_chan_idx].mac_lmtd_tput ) >> 8 ) * (hdrrmac4_var_ptr->phy_chan[
        phy_chan_idx].tx_on_factor >> 8) ) >> 8;
        
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].rl_tput_est = 
        (uint32) rl_tput_est_ch;

      /* Add per-carrier throughput estimate to total throughput estimate */
      rl_tput_est += rl_tput_est_ch;
    }
  }

  /* Save overall RL throughput estimate */
  hdrrmac4_var_ptr->rl_tput_est = (uint32) rl_tput_est;

} /* hdrrmac4_update_rl_data_rate_est */


/*===========================================================================
FUNCTION HDRRMAC4_UPDATE_CURR_RL_DATA_RATE                          EXTERNAL

DESCRIPTION
  Updates current reverse link throughput. The throughput value is in kbps.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrmac4_update_curr_rl_data_rate( void )
{
  uint8   phy_chan_idx;             /* Channel loop index */
  uint64  rl_tput_ch;               /* Reverse link throughput for a 
                                       channel */
  uint64  rl_tput = 0;              /* Total reverse link throughput */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( phy_chan_idx = 0;
    phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
    phy_chan_idx++ )
  {
    if( hdrrmac4_var_ptr->phy_chan[phy_chan_idx].chan_state == 
      HDRRMAC4_CHAN_OPEN_STATE )
    {
      /* Obtain current throughput in Q24.8 kbps units based on all
         the transmitted packets since last RL metrics reset. */
      rl_tput_ch = hdrrmac4drv_get_current_tput( phy_chan_idx );

      /* Calculate -
         RLTput = CurrentTput * TxOnFactor
         in Q24.8 units.
      */ 
      rl_tput_ch = ( rl_tput_ch * (hdrrmac4_var_ptr->phy_chan[
        phy_chan_idx].tx_on_factor >> 8) ) >> 8;
        
      hdrrmac4_var_ptr->phy_chan[phy_chan_idx].rl_tput_curr = 
        (uint32) rl_tput_ch;

      /* Add per-carrier throughput to total throughput */
      rl_tput += rl_tput_ch;

    }
  }

  /* Save overall RL throughput */
  hdrrmac4_var_ptr->rl_tput_curr = (uint32) rl_tput;

} /* hdrrmac4_update_curr_rl_data_rate */


/*===========================================================================
FUNCTION HDRRMAC4_LATCH_RL_INFO                                     EXTERNAL

DESCRIPTION
  Latches RL info in double buffer for reporting.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrmac4_latch_rl_info( void )
{
  /* Write buffer index */
  uint8 write_buf_idx;

  /* Shorthand pointer to RL info write buffer */
  hdrrmaccmn_rl_info_struct_type *rl_info_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Entering critical section */
  rex_enter_crit_sect( &hdrrmac4_var_ptr->rl_info_crit_sect );

  write_buf_idx = ( hdrrmac4_var_ptr->rl_info_read_buffer_idx + 1 ) % 
    HDRRMACCMN_RL_INFO_BUF_SIZE;

  rl_info_ptr = &hdrrmac4_var_ptr->rl_info[ write_buf_idx ];

  /* Save RL info */
  rl_info_ptr->rl_tput_est = (uint16) ( hdrrmac4_var_ptr->rl_tput_est >> 8 );
  rl_info_ptr->rl_tput_curr = (uint16) ( hdrrmac4_var_ptr->rl_tput_curr >> 8 );
  rl_info_ptr->plus_flag = hdrrmac4_var_ptr->plus_flag;
  rl_info_ptr->rlp_q_size = hdrrmac4_var_ptr->rlp_q_size;
  
  /* Sanity check RL rate estimate. It should not be less than the current rate.
     Correct it to be equal if less than curr rate */
  rl_info_ptr->rl_tput_est = MAX(rl_info_ptr->rl_tput_est,
                                 rl_info_ptr->rl_tput_curr);
    
  /* Update read buffer index to point to now readied write buffer */
  hdrrmac4_var_ptr->rl_info_read_buffer_idx = write_buf_idx;

  /* Leaving critical section */
  rex_leave_crit_sect( &hdrrmac4_var_ptr->rl_info_crit_sect );

} /* hdrrmac4_latch_rl_info */


/*===========================================================================
FUNCTION HDRRMAC4_GET_RL_RATE_INFO

DESCRIPTION
  This function populates the following in the passed struct:
  1. Estimated reverse link throughput in kbps
  2. Flag indicating if throughput greater than estimated rate may be possible
  3. Current reverse link throughput in kbps
  4. RLP queue length in octets
  
DEPENDENCIES
  None
 
PARAMETERS
  Pointer to location where reverse link rate info is to be populated

RETURN VALUE 
  None

SIDE EFFECTS
  None
 
NOTE 
  This API should not be called directly by external entities.
  hdrrmac_get_rl_rate_info() should be called instead.
===========================================================================*/
void hdrrmac4_get_rl_rate_info
(
  hdrrmac_rl_rate_info_struct_type * rl_info_ptr
)
{
  /* Entering critical section */
  rex_enter_crit_sect( &hdrrmac4_var_ptr->rl_info_crit_sect );

  /* Populate RL info from read buffer */

  rl_info_ptr->est_rl_tput = hdrrmac4_var_ptr->rl_info[ 
    hdrrmac4_var_ptr->rl_info_read_buffer_idx ].rl_tput_est;

  rl_info_ptr->plus_flag = hdrrmac4_var_ptr->rl_info[ 
    hdrrmac4_var_ptr->rl_info_read_buffer_idx ].plus_flag;

  rl_info_ptr->curr_rl_tput = hdrrmac4_var_ptr->rl_info[ 
    hdrrmac4_var_ptr->rl_info_read_buffer_idx ].rl_tput_curr;

  rl_info_ptr->q_len = hdrrmac4_var_ptr->rl_info[ 
    hdrrmac4_var_ptr->rl_info_read_buffer_idx ].rlp_q_size;

  /* Leaving critical section */
  rex_leave_crit_sect( &hdrrmac4_var_ptr->rl_info_crit_sect );

  HDR_MSG_PROT_4( MSG_LEGACY_MED,
    "EstRLTput = %u, Plus = %u, CurrTput = %u, QLen = %u",
    rl_info_ptr->est_rl_tput,
    rl_info_ptr->plus_flag,
    rl_info_ptr->curr_rl_tput,
    rl_info_ptr->q_len );

} /* hdrrmac4_get_rl_rate_info */

#endif /* FEATURE_HDR_MODEM_API_RL_INFO */


#ifdef FEATURE_HDR_TRAFFIC_DTX
/*===========================================================================
FUNCTION HDRRMAC4_CHECK_TDTX_FRAB_THRESH                                EXTERNAL

DESCRIPTION
  Check whether FRAB of all carriers are less than than the threshold.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  TRUE: if FRAB of all carriers are less than than the threshold.
  otherwise FALSE.
  

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4_check_tdtx_frab_thresh ( void )
{

  uint8 chan_index;
  uint8 chan;
  hdrrmac4_phy_chan_vars_struct_type *chan_ptr;
  boolean frab_check = TRUE;
  int16 frab_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (chan_index = 0; 
        chan_index < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
        chan_index++ )
  {
    frab_list[chan_index]= 0;
  }
  for (chan_index = 0; 
       chan_index < hdrrmac4_var_ptr->num_open_state_chans; 
       chan_index++)
  {
    chan = hdrrmac4_open_rl_list[chan_index];
    chan_ptr = &hdrrmac4_var_ptr->phy_chan[chan];
    frab_list[chan_index] = chan_ptr->sector_info.frab;
    if ( chan_ptr->sector_info.frab >= HDRRMAC_TDTX_FRAB_THRESHOLD )
    {
      frab_check = FALSE;
      break;
    }
  }
  HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
      "tDTX check_frab_threshold: chan_num %d FRAB %d %d %d",
      hdrrmac4_var_ptr->num_open_state_chans,
      frab_list[0],frab_list[1],frab_list[2] );

   return frab_check;
  
} /* hdrrmac4_check_tdtx_frab_thresh */
#endif /* FEATURE_HDR_TRAFFIC_DTX */

#ifdef FEATURE_HDR_FLEX_CONN
/*==========================================================================

FUNCTION  HDRRMAC4_SET_FLEXCONN_MODE

DESCRIPTION
  This function calls appropriate driver function to send a message to FW to
  enable or disable FLEXCONN mode
    
DEPENDENCIES
  None

INPUTS
  dtx_mode:  dtx mode, TRUE means enabling DTX
  frame_offset: frame offset

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac4_set_flexconn_mode( boolean flexconn_mode )
{
  hdrrmac4drv_set_flexconn_mode( flexconn_mode );
} /* hdrrmac3_set_flexconn_mode */
#endif /* FEATURE_HDR_FLEX_CONN */

#endif /* FEATURE_HDR_REVB */

