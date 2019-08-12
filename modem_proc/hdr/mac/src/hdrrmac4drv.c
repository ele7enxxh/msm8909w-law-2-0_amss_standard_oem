/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H D R   S U B T Y P E  4
                           
             R E V E R S E   T R A F F I C   C H A N N E L   M A C

                            P R O T O C O L

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Multicarrier Reverse 
  Traffic Channel Media Access Control (MAC) Protocol for an HDR access
  terminal.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating
  the protocol.

  Copyright (c) 2007 - 2015 by Qualcomm Technologies, Incorporated.  
  All Rights Reserved.
    
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/src/hdrrmac4drv.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/03/15   mbs     Forced 2HS interval between delivery of 2 FW cmds
01/31/15   vke     Added API to check silence frame
01/23/15   wsh     Fixed PermPayload updating RRI on wrong channel 
01/07/15   ljl     Fixed the logging for RRI 8 and 9.
11/19/14   wsh     Disallow DTX if TxPwr is too high 
11/06/14   rmv     Handle FTCMAC supervision in RMAC to stop processing  packets
                   from Tx queues 
11/06/14   wsh     RTT:Fixed incorrectly reporting boosted T2P for RRI > 6 
09/29/14   arm     Skip disabling carrier for QTA.
09/22/14   arm     HDR QTA code cleanup
08/12/14   wsh     Disable RTT in TAP call; force HiCap if boost, new threshold
07/29/14   tnp     FR22375: Fix Medium compiler warnings in HDR
10/10/13   arm     Fixed bug in stopping sDTX  when enabling nDTX.
02/03/14   rmv     Skip RTC Ack processing if it is for a disabled carrier 
07/24/14   vko     Enabled FEATURE_HDR_REVB_DTX_MODE for bolt
07/03/14   kss     Removed FEATURE_HDR_RF_SUPPORTS_API_FOR_MAX_PWR_LIMIT 
06/26/14   sat     Fixed Klocwork errors.
06/04/14   arm     Changes to support DO to G QTA
05/27/14   wsh     Fixed compile error for Jolokia build
05/19/14   wsh     Cancel prev. RevLinkCarrier override in next TCA
05/14/14   sat     Fixed Klocwork errors
05/05/14   sat     Changes in initializing channel ID of a dropped carrier 
04/17/14   kss/wsh Tuneaway/tuneback T2P boost tweaking  
04/03/14   wsh     Added more F3 for T2P boost feature 
01/14/14   kss     Fixed RRI history alignment issue in permitted payload opt.
01/07/14   smd     undefine t2p unit test related code.
12/16/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
12/10/13   kss     Added Rev B T2P Boost support. 
11/26/13   rmg     Supported enabling carriers in FW without a frame delay.
11/25/13   rmg     Added changes to reduce idle subframes before tuneaway.
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
10/22/13   arm     Send term target to FW for tDTX. 
10/08/13   mbs     Added support for BOLT interface changes
10/02/13  rmg     Added support for RL rate estimate for modem API.
09/04/13   rmg     Fixed issue with RTCAck arrival on disabled carriers.
09/03/13   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
08/29/13   arm     Provided tDTX Tx Power threshold to FW.
08/23/13   rmg     Disable DRC supervision in FTM single FL multi RL mode.
08/01/13   smd     Used new secapi to get random number.
07/30/13   rmg     Fixed carrier drops during HHO.
07/10/13   arm     Supported traffic DTX. 
06/03/13   rmg     Added MCDO connected setup enhancement support.
05/31/13   vko     Fixed klocwork issues
05/24/13   smd     Reverted KW fixes. 
05/16/13   vko     Fixed klocwork issues
05/15/13   kss     Use hdrts API to check slot time.
05/06/13   mbs     Fixed compiler errors for BOLT build
03/26/13   smd     Fixed RevB MC call.
03/15/13   dsp     Programmed User LC Mask in config_modulator() in FTM mode.
02/20/12   smd     Handled the case of HHO during DRC supervision for DTX/FC.
02/17/13   arm     Supported network dtx and device only dtx co-existence.
01/25/13   smd     Removed hdrrmac4drv_all_carriers_exit_drc_supervision().
                   Submitted RL gain log also when PA is off. 
01/17/13   rmg     Removed carrier argument from timed PA enable/disable.
                   Used scheduled PA control instead of immediate for RLSI.
01/14/13   rmg     Enable carriers in modulator when starting RMAC.
01/14/13   rmg     Rectified criterion for turning off PA when a carrier 
                   enters DRC supervision.
01/10/13   kss     Handle unexpected encode status from FW.
12/12/12   kss     Removed redundant write to modulator to reset to rev 0. 
12/04/12   kss     Disable DRC supervision timer on channel exit.
10/10/12   smd     Fixed fc/dtx control for RevB DRC supervision timeout.
10/09/12   rmg     Fixed check for overwriting subframe_id in prev_tx_status().
09/28/12   smd     Reset pa_on_status if PA is not turned on after RLSI.
09/05/12   smd     Fixes for FlexConn and DTX.
08/23/12   smd     Added updates for Device only DTX and FlexCONN.
07/27/12   ssm     Added fields for logging AT-funnel mode and maximum 
                   available transmit power in funnel mode. 
07/27/12   rmg     Added enhanced PA backoff support.
06/20/12   smd     Supported Device only DTX and FlexCONN.
04/30/12   dsp     Fixed compiler warnings on Dime. 
04/02/12   rmg     Made changes to clear closed loop adjusts for carriers 
                   other than C0 when entering RMAC after access.
03/20/12   arm     Resolve TCB errors for Dime. 
03/19/12   ljl     Fixed Dime compiler errors.
02/14/12   wsh     Added support for SVLTE+SHDR
01/26/12   ljl     Replaced tramp with DAL.
01/04/12   rmg     Used timed enable when turning on PA after RLSI.
12/08/11   mbs     Supported preparation of modulator to start tx for FTM
11/21/11   rmg     Populated trailing entries in txCarriersOrderedByRank list
                   with 0xFFFF.
11/07/11   vlc     Fixed Klocwork warnings. 
11/01/11   rmg     Prevented subframe ISR from being processed after it is 
                   uninstalled.
09/12/11   arm     PA headroom improvements. 
08/04/11   ssu     Reference ftm_mode using ftm_get_mode().       
08/04/11   ssu     Replace hdr_ftmi_xx with hdrhitmsg_xx using hdrhitmsg APIs. 
07/27/11   rmg     Added PA enable fix for RevB.
07/07/11   kss     Corrected order of modulator mode transition for NikeL.
06/21/11   rmg     Added support for updated hdrmod_tx_start/stop().
06/21/11   arm     PA headroom improvements. 
04/28/11   kss     Fixed compiler warnings.
04/27/11   kss     Supported FW-controlled Tx enable.
04/26/11   kss     Added checks for KW.
03/23/11   kss     Adjusted RF includes for NikeL RF.
02/08/11   kss/rmg Disable carriers at tuneaway.
01/21/11   rmg     Supported RF API changes for NikeL.
01/21/11   rmg     Fixed enabling of ovhd chans on RL carriers after tuneback.
12/16/10   rmg     Fixed a compiler warning. 
12/13/10   rmg     Removed tx pilot power initialization for added carriers.
12/13/10   rmg     Supported change in FW to make pilotPowerFilt 32-bits.
12/09/10   rmg     Added changes to support increased Ec/Io resolution.
12/01/10   rmg     Made hdrrmac4drv_metrics a packed struct.
                   Corrected enabling of RPC in start_channels().
11/16/10   rmg     Set FW to Rel0 mode when exiting RevB.
11/16/10   rmg     Handled the case when sf valid status is set to FALSE after
                   data for a partial set of carriers is loaded to encoder.
10/25/10   rmg     Added change to obtain demod idx from SRCH in RL carrier 
                   update processing. 
10/21/10   rmg     Fixed incorrect interpretation of FW encode status.
10/11/10   rmg     Change to shift carriers transitions to data-inactive / 
                   inactive states from prev_tx_status() to carrier mgmt proc.
08/05/10   rmg     Added updates to FW interaction messages.
08/04/10   kss     Fixed rpc_step conversion in FTM.
07/21/10   kss     Changed to obtain rpc_step in FTM mode. 
06/21/10   cnx     Fixed compiler warnings.
06/03/10   cnx     Fixed Klocwork errors.
05/18/10   rmg     Shifted DRC supervision timer management to FMAC: added 
                   APIs hdrrmac4drv_enter/exit_drc_supervision().
04/09/10   kss     Corrected RPC parameter to firmware. 
03/04/10   rmg     Switched pilotstrength_ps calculation from using reference
                   subaset to subaset corresponding to RL carrier.
03/03/10   rmg     Cleared interlace history for the carrier on its removal.
12/14/09   rmg     Added a code explanation comment in carrier management code.
11/23/09   rmg     Switched MDSP interaction to message router.  Modules yet 
                   to change: logging, DRC supervision.
11/19/09   rmg     Bypassed power differential check in TAP mode.
11/18/09   rmg     Corrected out of bound array access.
11/17/09   rmg     Rounded up MaxTxAvail value.
11/04/09   rmg     Changed constant used in equation to convert MDSP units to
                   dBm from 63.5 to 63.8.
11/04/09   rmg     Fixed DRC supervision reset on receiving a new TCA.
10/14/09   rmg     PA backoff migration to RF SW.
10/14/09   rmg     Fixed incorrect triggering of power differential carrier 
                   drop when a carrier is put in to-be-inactive state.
10/02/09   rmg     Used leftmost point of TxT2PmaxPS curve for calculating 
                   ACI mitigation TxT2PmaxPS cap. 
09/22/09   rmg     RL flow control: data disabling on non-SLP carriers.
09/22/09   rmg     ACI mitigation changes: restricting TxT2PMax_PS.
07/29/09   rmg     Added support to use negotiated pilot power diff value.
07/29/09   rmg     F3 message clean up.
07/23/09   rmg     Fixed incorrect clearing of DRC supervision timers on 
                   existing carriers during retunes due to disjoint TCAs.
06/25/09   rmg     Added support for carrier drop due to transmit pilot power
                   differential.
06/24/09   kss     Handle zero/negative value in notify_suspend function.
06/22/09   etv     Fixed Lint warnings.
06/17/09   rmg     Removed clearing of closed loop adj when removing carrier.
06/15/09   kss     Handle case where a disable is forced during shutdown.
06/05/09   rmg     Fixed div by zero error when all carriers are under DRC 
                   supervision.
06/04/09   rmg     Added accounting for power backoff before sending carrier 
                   request.
06/04/09   rmg     Added RL carrier update to the conditions triggering an 
                   update of number of reportable carriers.
05/05/09   rmg     Added deltaT2P accounting in PA headroom.
04/16/09   rmg     Changes to follow permitted payload constraints after
                   missed subframes.
04/15/09   rmg     Corrected some code comments.
03/27/09   rmg     Restore single carrier backoff value when exiting RevB.
03/11/09   kss     Add Rev B support for hdrrmac_get_current_tx_packet_info().
03/10/09   rmg     Change to include carrier's reportability into carrier 
                   management algorithm.
03/03/09   rmg     Removed finger-in-lock check for PA availability during 
                   tune-away processing.
12/11/08   kss     Removed FEATURE_HDR_REVB_RMAC_7500.
12/04/08   rmg     Removed intlocks.
12/04/08   rmg     Changes to initialize pilot power of newly added carrier.
11/24/08   rmg     Lint fixes.
10/27/08   rmg     Fixed slot FRAB logging.
10/26/08   rmg     Added support for RevB FTM.
10/20/08   rmg     Removed per-carrier RF backoff accounting.
10/10/08   rmg     Clear drop ranks after the carrier to-be-dropped is logged.
10/07/08   rmg     Disable carrier mgmt when throttle disable flag is set.
10/06/08   rmg     Removed bias in CAPUT prioritized carrier list formation.
09/26/08   rmg     Updated power margin parameters
09/26/08   rmg     Corrected term tgt value used in CAPUT pri calculations.
08/19/08   kss     Initialize pa_status.
08/11/08   etv     Code Review changes for DTXMode support.
                   Added support for accepting GAUP of RRIChannelPowerParams.
08/07/08   rmg     Change to rewrite PAC parameters to MDSP every subframe.
08/01/08   rmg     Exclude carrier under DRC supervision from SLP selection.
07/30/08   rmg     Read TxP from PilotPowerPeak register for hdrm calculations.
07/30/08   rmg     Change to not allocate power to channel under DRC
                   supervision.
07/16/08   kss     Fixes for RLSI race condition (turning tx on during RLSI).
07/10/08   rmg     Ensure DRC Sup failed on at least one channel.
07/08/08   rmg     Clear closed loop adjust on new carriers.
07/08/08   rmg     Support for checking RLSI/DRC supervision in SETUP state.
07/01/08   rmg     Corrected formula for pilot power filterTC calculation.
07/01/08   kss     Do not reset early_term_config before each call.
06/26/08   rmg     Change to reset filtered txp on carrier activation.
06/26/08   rmg     Support for SLP carrier forced switch upon DRC supervision.
06/18/08   etv     Enabled/disabled DTXMode on configuration change.
06/17/08   rmg     Added new fields in PA headroom log packet.
06/17/08   rmg     Change to use new DSP interface to write carrier drop ranks.
06/17/08   rmg     Pulled out temporary change to set FiltTxP_TC to 400.
06/16/08   etv     Added timestamp to RL phy layer log packets.
05/28/08   rmg     Corrected CAPUT priority computation.
05/23/08   rmg     Change to force SLP carrier switch when the carrier is 
                   removed by TCA.
05/23/08   kss     Add BetaP and global Txlimiting to RL Gains log.
05/15/08   rmg     Fixed txt2p update for open channels not queuing new data.
05/13/08   rmg     DRC supervision cleanup.
05/13/08   rmg     Fixed interlace history update.
05/13/08   rmg     Temporary change to set FiltTxP_TC to 400.
05/12/08   rmg     Made changes to program correct demod carrier in MDSP.
05/08/08   rmg     Removed allocated maxT2P jumps in assigned carriers in eq 
                   pwr allocation when a carriers becomes data_inactive.
05/08/08   rmg     Changed carrier management thresholds.
05/08/08   rmg     Added filtered power margin to carrier mgmt calculations.
05/08/08   rmg     Changed transmit pilot filter TC to 600 slots.
05/08/08   rmg     Initialized filt RAB limited power to a large value.
05/02/08   rmg     Disabled RL gains logging for channels in DRC supervision.
04/28/08   rmg     Change to clear number of requested carriers every sf.
04/28/08   rmg     Fixed incorrect increments in RL metrics.
04/22/08   rmg     Made changes in power margin parameters.
04/11/08   rmg     Fixed carrier drop mechanism.
04/08/08   rmg     Change to update open channel count after carrier mgmt.
04/07/08   rmg     Replaced hardcoded Pmax value with DSP returned value.
04/04/08   rmg     Fixed PARQ reporting.
03/10/08   rmg     Enabled equal distribution PA hdrm and carrier management.
03/10/08   rmg     Loop optimization.
03/10/08   rmg     Added support for logging dropping rank in hdrm log pkt.
03/10/08   rmg     Added TxT2Pmax_PS based reallocation in inv prop PA hdrm.
02/27/08   rmg     Made rounding changes in PA hdrm and CM.
02/27/08   rmg     Added prev sf SLP carrier and SLP data status fields in 
                   in UT inputs.
02/27/08   rmg     Updated SLP carrier selection procedure.
02/27/08   rmg     Updated RTCMAC and overall priorities to give minimum 
                   priorities to TO_BE_INACTIVE and INACTIVE carriers.
02/27/08   rmg     Made change to treat TO_BE_DATA_INACTIVE carrier as 
                   DATA_INACTIVE for carrier management power calculations.
02/12/08   rmg     Added support for MC_PA_HDRM_AND_CARRIER_MGMT log packet.
02/12/08   rmg     Renamed hdrenc_load_rev_b_frame() to hdrenc_load_frame().
02/06/08   kss     Copy access adjust to all carriers on call init.
02/04/08   etv     Added support for logging RL physical layer packet dump.
02/01/08   rmg     Added carrier management unit test framework.
01/29/08   rmg     get_prev_tx_status() cleanup.
01/29/08   rmg     Fixed TxT2Pmax computation.
01/29/08   rmg     Instantaneous pilot power used in FiltRAB calculations.
01/29/08   rmg     Corrected RMAC priority computation.
01/29/08   rmg     Added overall backoff.
01/29/08   rmg     Enabled equal headroom distribution.
01/29/08   rmg     Added support for reference subaset logging.
01/29/08   rmg     Added an F3 msg to print I/Q masks.
01/29/08   rmg     get_prev_tx_status() cleanup.
01/07/08   kss     Log Tx AGC values, set default RRI.
01/04/08   cc      Removed timing profile functionality.  It's done in
                   rmac4.
12/17/07   cc      Set the unassigned channel ID to 0x0F in RL Metrics log.
12/13/07   cc      Corrected PilotStrengthPS computation.
12/13/07   cc      Fixed invalid channel record in RL Metrics after stats
                   reset; added tweakable hard-coded max_tx_t2p.
12/12/07   kss     Enable RPC on all active carriers.
12/11/07   cc      Disabled HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS &
                   HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES.
12/07/07   rmg     Temporary change to bypass CM decisions.
12/05/07   rmg     Fix to return Tx statuses in correct order to RMAC
11/29/07   rmg     Included support for carrier management states.
11/29/07   rmg     Temporary change to bypass PA headroom algorithm.
11/26/07   rmg     Factored get_max_t2p() into
                   - get_max_t2p_rev_a() - RevA PA headroom
                   - get_max_t2p_rev_b() - RevB PA headroom with/without UT.
11/26/07   rmg     Corrected divide by zero error in PA headroom computation.
11/20/07   cc      Set packet ID for all interlaces to be Idle in start_channel.
11/16/07   rmg     Corrected RTCMAC priority computation.
11/12/07   rmg     Added unit test support for PA headroom algorithm.
11/07/07   rmg     Added support for MC_RL_GAINS log packet.
11/02/07   cc      Added 3x unit test support on 7500.
11/01/07   rmg     Added support for MC_RL_PKT_STATUS log packet.
10/30/07   rmg     Updated RTCMAC priority computation.
10/29/07   rmg     Made changes to support common RMAC driver files between
                   7500 and 7800 builds.
10/27/07   rmg     Shifted enabling of carrier from start_channels() to 
                   start_open_state().
10/27/07   rmg     Added hdrrmac4drv_get_outstanding_packet_ids().
10/26/07   rmg     Added support for per-carrier RAB and DRC control.
10/26/07   rmg     Made changes to support PA headroom algorithm updates.
10/25/07   cc      Fixed unit test issue that T2POutflow not updated correctly.
10/20/07   kss     Use ENABLE/DISABLE RPC macros.
10/18/07   rmg     Added support for MC_RL_METRICS log packet.
10/16/07   rmg     Removed log pkt test code.
10/15/07   rmg     Added carrier request support.
10/09/07   rmg     Added support for LOG_HDR_MC_RL_ARQ_STATUS_C and 
                   LOG_HDR_MC_RAB_UPDATE_INFO_C log packets.
10/06/07   cc      Added unit test support.
10/03/07   rmg     Added NEG_INFINITY_T2P support.
09/26/07   rmg     Made corrections based on code review comments.
09/25/07   rmg     Added encoder_window_missed support.
09/24/07   rmg     Added hdr_inactive_reason support.
09/20/07   rmg     Fixed circular inclusions.
09/19/07   rmg     Removed RVCT compiler warnings.
09/14/07   rmg     Added to source control with MSM7500 support changes.
06/12/07   rmg     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "customer.h"

/* General include files */
#include "comdef.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#include "hdrutil.h"
#ifdef FEATURE_HDR_REVA_L1
#include "dsm.h"
#include "ran.h"
#include "task.h"
#include "hdrenc.h"
#include "hdrrmac4drv.h"
#include "hdrrmacmath.h"
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
#include "hdrrmaccmni.h"
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */
#include "hdrdebug.h"
#include "hdrmod.h"
#include "hdrmdspmcr.h"
#include "hdrsrchmac.h"
#include "hdrsrchdef.h"
#include "hdrsrchtypes.h"
#include "hdrtrace.h"
#include "hdrtx.h"
#include "hdrts.h"
#ifndef FEATURE_RF_SVDO_API
#include "rf1x.h"
#endif
#include "hdrfmac.h"
#include "hdrlog.h"
#include "hdrlogi.h"
#include "rfm_hdr.h" 
#include "hdrphy.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "hdrhitmsg.h"
#include "ftm.h"
#endif

#include "hdrmsg.h"
#include "hdrsrchrf.h"
#include "hdrrmaci.h"

//#define HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
//#define HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#define HDRRMAC4DRV_EQUAL_HEADROOM_DISTRIBUTION

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Define this to do unit test */ 
#undef FEATURE_HDR_RMAC4_UNIT_TEST
/* Define this and RMAC4_UNIT_TEST to do 3 carriers unit test on 7500 */ 
#undef FEATURE_HDR_RMAC4_3X_UNIT_TEST

/* Define this to enable unit test capability for PA headroom and 
   carrier management */
#undef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST

#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
#error code not present
#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Open State ISR offset, in slots. */ 
#define HDRRMAC4DRV_INTERRUPT_OFFSET_SLOTS   (0)

/* Open State ISR offset, in chips. */ 
#define HDRRMAC4DRV_INTERRUPT_OFFSET_CHIPS   (0 * 2048)

/* Last halfslot within a subframe that we will even attempt to prepare or 
   load a subpacket */ 
#define HDRRMAC4DRV_HALFSLOT_DEADLINE         6

/* Number of frames needed to setup for RLSI */ 
#define HDRRMAC4DRV_SILENCE_SETUP_FRAMES      3

/* Index for last subpacket of a packet */
#define HDRRMAC4DRV_LAST_SUBPACKET         (HDRRMACCMN_MAX_NUM_SUBPACKETS - 1)

/* Number of slots in a frame */
#define HDRRMAC4DRV_SLOTS_PER_FRAME          16

/* Number of slots in a subframe */
#define HDRRMAC4DRV_SLOTS_PER_SUBFRAME        4

/* Number of chips per slot */
#define HDRRMAC4DRV_CHIPS_PER_SLOT   2048

/* Number of halfslots in a 4-slot subframe */
#define HDRRMAC4DRV_HALFSLOTS_PER_SUBFRAME    8

/* Number of halfslots per slot :) */
#define HDRRMAC4DRV_HALFSLOTS_PER_SLOT        2

/* Slot index of subframes (within a frame) */
#define HDRRMAC4DRV_SUBFRAME_0_SLOT           0
#define HDRRMAC4DRV_SUBFRAME_1_SLOT           4
#define HDRRMAC4DRV_SUBFRAME_2_SLOT           8
#define HDRRMAC4DRV_SUBFRAME_3_SLOT          12

/* Maximum number of subframe N values */
#define HDRRMAC4DRV_SUBFRAME_N_MAX           75

/* Indicates an undefined or NULL packet */
#define HDRRMAC4DRV_INVALID_PACKET         0xFE

/* Indicates unassigned channel ID */
#define HDRRMAC4DRV_INVALID_CHAN_ID        0x0F

/* MAC trailer bit positions */
#define HDRRMAC4DRV_FORMAT_BIT_S                 7
#define HDRRMAC4DRV_TX_MODE_BIT_S                6


/* -- Definitions for creating the I/Q mask -- */

/* Long code mask upper ten bits. */
#define I_MASK_UPPER_BITS 0x000003ff
#define Q_MASK_UPPER_BITS 0x00000000

/* ATI permutation array. */
#define HDRRMAC4DRV_PERM_LEN 32
LOCAL const byte hdrrmac4drv_ati_permutation_array[] =
{
   0, 31, 22, 13,  4, 26, 17,  8, 30, 21, 12,  3, 25, 16,  7, 29,
  20, 11,  2, 24, 15,  6, 28, 19, 10,  1, 23, 14,  5, 27, 18,  9
};

/* List of bits for XOR for Q mask bit 0 */
#define HDRRMAC4DRV_XOR_LEN 20
LOCAL const byte hdrrmac4drv_q_xor_array[] =
{
   0, 1, 2, 4, 5, 6, 9, 15, 16, 17, 18, 20, 21, 24, 25, 26,
  30, 32, 34, 41
};

/* Tx Power Margin Values for Headroom */
#define HDRRMAC4DRV_TX_PWR_MARGIN_MAX_V        0x0500  /* Max: 2 dB          */
#define HDRRMAC4DRV_TX_PWR_MARGIN_MIN_V        0x0000  /* Min: 0 dB          */
#define HDRRMAC4DRV_TX_PWR_MARGIN_UP_STEP_V    0x0014  /* Upstep: 0.03125 dB   */
#define HDRRMAC4DRV_TX_PWR_MARGIN_DOWN_STEP_V  0x0014  /* Downstep: 0.03125 dB*/
#define HDRRMAC4DRV_PWR_MARGIN_DYNAMIC_RANGE   0x0066  /* Dynamic Range 
                                                            = 102.4 dB       */

#define HDRRMAC4DRV_FILT_TXP_DELTA               1024  /* 1 dB 
                                                          in Q.10 units      */

#define HDRRMAC4DRV_MAX_DROP_RANK              0x7     /* Maximum drop rank  */


/* Filtered power margin is set to the following value when it needs
   to be initialized */
#define HDRRMAC4DRV_FILT_PWR_MARGIN_INVALID    0xFFFFFFFF

/* Filtered RAB limited power is set to max value for a newly added
   carrier */
#define HDRRMAC4DRV_FILT_RAB_LTD_PWR_MAX       0xFFFFFFFF

/* Time constants */
/* RAB: 1/TC_RAB = 1/600 slots = 1/150 subframes ~ 437 >> 16 */
#define HDRRMAC4DRV_TC_RAB_MUL                 437
#define HDRRMAC4DRV_TC_RAB_SHIFT                16

/* Power Margin: 1/TC_PWRMARGIN = 1/600 slots = 1/150 subframes ~ 437 >> 16 */
#define HDRRMAC4DRV_TC_PWRMARGIN_MUL           437
#define HDRRMAC4DRV_TC_PWRMARGIN_SHIFT          16

/* PilotPower: TC_Pilot = 600 slots = ROUND(1/600 * 65536) in DSP units
                        = 109 = 0x6D in DSP units */
#define HDRRMAC4DRV_TC_PILOT                   0x6D

/* Macro to sign extend a 6-bit 2's comp field to 32 bits */ 
#define HDRRMAC4DRV_SIGN_EXTEND_6_TO_32( val )         \
      ((( (val) & 0x20 ) == 0 )? (val) :( (val)| (int32) 0xffffffc0 ))

/* Macro to add two unsigned numbers and saturate the result to max_val.
   The macro assumes that val2 is lower than max_val. */
#define HDRRMAC4DRV_ADD_UINT_SAT( val1, val2, max_val )  \
  (((val1) < ((max_val) - (val2)))? ((val1)+(val2)) :(max_val))

/* MDSP interaction: shifts and bit masks */
#define HDRRMAC4DRV_FW_PKT_STATUS_SHFT  11
#define HDRRMAC4DRV_FW_PKT_STATUS_BMSK  0x001F
#define HDRRMAC4DRV_FW_FRAME_LOADED     0xD000

/* Max allowable per-carrier funnel bias value in Q.10 dBm */
#define HDRRMAC4DRV_MAX_FUNNEL_BIAS     10240  /* 10 dBm */

/* In case of a new connection setup, duration in subframes from enabling
   of the best carrier after which MCDO feasibility is to be evaluated. */
#define HDRRMAC4DRV_MCDO_EVAL_SUBFRAMES 8

/* In case of delayed PA enable, duration in subframes after which PA 
   enable command should be sent to FW. */
#define HDRRMAC4DRV_PA_ENABLE_DELAY_SUBFRAMES 1

/*---------------------------------------------------------------------------
                             CONSTANTS
---------------------------------------------------------------------------*/

/* Look up table for frab filter. Values determined by:
**   val = (tc - 1) * 65536 / tc 
*/
const uint16 hdrrmac4drv_frab_tc[] =
{
  0xFE00,   /* 128 slots */
  0xFF00,   /* 256 slots */
  0xFF55,   /* 328 slots */
  0xFF80    /* 512 slots */
};


/* Look up table for qrab filter */ 
const uint16 hdrrmac4drv_qrab_tc[] =
{
  0xC000,   /* 4 slots */
  0xE000    /* 8 slots */
};


/* Look up table for pilot filter time constant, converted to subframes */
const uint8 hdrrmac4drv_pilot_filter_lengths[] =
{
  (32/4), 
  (64/4), 
  (128/4)
};


/*---------------------------------------------------------------------------
                             Carrier Mangement
---------------------------------------------------------------------------*/
/* Margin applied to long term average of transmit pilot powers of the 
   assigned carriers to estimate the transmit pilot power of an unassigned 
   carrier. */ 
#define HDRRMAC4DRV_UNASSIGNED_CARRIER_TXP_MARGIN  2048
  /* = 3 dB (2 lin) in Q.10 linear units */

/* Carrier configurations.  The channel numbers here are not physical 
   channels ids.  In case of data funneling, channel 0 translate to the 
   channel with maximum RTCMAC priority and in case of channel dropping,
   channel 0 would be the channel with maximum overall priority */ 
typedef enum
{
  /* Carrier configuration # 0
     Pilot plus overhead on prioritized channels 0,1 and 2 enabled,
     Data on prioritized channels 0, 1 and 2 enabled */
  CARRIER_CONFIG_P012_D012      = 0,

  /* Carrier configuration # 1
     Pilot plus overhead on prioritized channels 0,1 and 2 enabled,
     Data on prioritized channels 0 and 1 enabled */
  CARRIER_CONFIG_P012_D01       = 1,

  /* Carrier configuration # 2
     Pilot plus overhead on prioritized channels 0,1 and 2 enabled,
     Data on prioritized channels 0 enabled */
  CARRIER_CONFIG_P012_D0        = 2,

  /* Carrier configuration # 3
     Pilot plus overhead on prioritized channels 0 and 1 enabled,
     Data on prioritized channels 0 and 1 enabled */
  CARRIER_CONFIG_P01_D01        = 3,

  /* Carrier configuration # 4
     Pilot plus overhead on prioritized channels 0 and 1 enabled,
     Data on prioritized channels 0 enabled */
  CARRIER_CONFIG_P01_D0         = 4,

  /* Carrier configuration # 5
     Pilot plus overhead on prioritized channels 0 enabled,
     Data on prioritized channels 0 enabled */
  CARRIER_CONFIG_P0_D0          = 5,

  CARRIER_CONFIG_MAX            = 6,

  CARRIER_CONFIG_INVALID        = 0xFF

} hdrrmac4drv_carrier_config_id_enum_type;

/* Carrier management thresholds are defined in terms of T2P values 
   for the packet sizes a carrier is expected to be able to carry in 
   different carrier states.  The following defines the packet sizes for
   these thresholds */
#define HDRRMAC4DRV_REMAIN_ACTIVE_PS   HDRRMACCMN_8192_BITS
#define HDRRMAC4DRV_ACTIVATE_PS        HDRRMACCMN_768_BITS
#define HDRRMAC4DRV_DEACTIVATE_PS      HDRRMACCMN_512_BITS
#define HDRRMAC4DRV_MIN_RATE_PS        HDRRMACCMN_768_BITS
#define HDRRMAC4DRV_ADD_PS             HDRRMACCMN_256_BITS

/* Carrier management thresholds */
typedef struct
{
  /* TxT2P required for maintaining an active carrier that is not
     a candidate for deactivation */
  uint32 t2p_remain_active;

  /* TxT2P required by a carrier to get promoted from not carrying
     data to carrying data */
  uint32 t2p_activate;

  /* Minimum TxT2P required by a carrier to sustain active state */
  uint32 t2p_deactivate;

  /* TxT2P required to maintain minimum data rate */
  uint32 t2p_min_rate;

  /* TxT2P required to add an additional carrier */
  uint32 t2p_add;

} hdrrmac4drv_carrier_mgt_thresh_type;

hdrrmac4drv_carrier_mgt_thresh_type hdrrmac4drv_carrier_mgt_thresh;

/* Next configuration type */
typedef struct
{
  /* Carrier configuration id */
  hdrrmac4drv_carrier_config_id_enum_type  config_id;

  /* Flag set if the transition to the next carrier configuration is due to 
     data funneling */
  boolean                                  data_funneling;

} hdrrmac4drv_next_config_type;


/* Carrier states
   Major States: 
     INACTIVE, SETUP, OPEN
   Minor States/Substates: 
     DATA_ACTIVE, DATA_INACTIVE, TO_BE_DATA_INACTIVE, TO_BE_INACTIVE */
typedef enum
{
  /* INACTIVE 
     - transmission disabled */
  HDRRMAC4DRV_CH_ST_INACTIVE                 = 0x00,

  /* SETUP 
     - the carrier transitions from inactive state to open state via 
     setup state */
  HDRRMAC4DRV_CH_ST_SETUP                    = 0x10,

  /* OPEN 
     - transmission on the carrier enabled */
  HDRRMAC4DRV_CH_ST_OPEN                     = 0x20,

  /* The following are substates a carrier can be in while in OPEN state */
  /* OPEN->DATA_ACTIVE 
     - transmission of data plus overhead channels enabled */
  HDRRMAC4DRV_CH_ST_DATA_ACTIVE              = 0x24,

  /* OPEN->TO_BE_DATA_INACTIVE 
     - carrier scheduled for transition to DATA_INACTIVE state. In this 
     state, unless the carrier is an SLP carrier, the carrier will be 
     shifted to DATA_INACTIVE state after the ongoing transmission on the
     carrier is complete.  No new data is allowed to be trasmitted on the
     carrier. If the carrier is an SLP carrier, the carrier will transition
     to DATA_INACTIVE state only after the data in the SLP queue has been 
     drained and the ongoing transmission on the carrier is complete. */
  HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE      = 0x23,

  /* OPEN->DATA_INACTIVE 
     - transmission of only overhead channels enabled */
  HDRRMAC4DRV_CH_ST_DATA_INACTIVE            = 0x22,

  /* OPEN->TO_BE_DATA_INACTIVE 
     - carrier scheduled for transition to INACTIVE state. In this 
     state, unless the carrier is an SLP carrier, the carrier will be 
     shifted to INACTIVE state after the ongoing transmission on the
     carrier is complete.  No new data is allowed to be trasmitted on the
     carrier. If the carrier is an SLP carrier, the carrier will transition
     to INACTIVE state only after the data in the SLP queue has been 
     drained and the ongoing transmission on the carrier is complete. */
  HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE           = 0x21

} hdrrmac4drv_carrier_state_enum_type;

/* To mask out minor channel states*/
#define HDRRMAC4DRV_CH_ST_MAJOR_MASK           0xF0


/* Carrier configuration information */
typedef struct
{
  /* Number of carriers in the configuration */
  uint8 num_carriers;

  /* Next up configuration */
  hdrrmac4drv_next_config_type next_up_config;

  /* Next down configuration */
  hdrrmac4drv_next_config_type next_down_config;

  /* States of carriers in the configuration */
  hdrrmac4drv_carrier_state_enum_type 
    carrier_state[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

} hdrrmac4drv_carrier_config_type;


/* Carrier configurations */
hdrrmac4drv_carrier_config_type 
  hdrrmac4drv_carrier_config[CARRIER_CONFIG_MAX] =
{
  /* Carrier configuration # 0 [CARRIER_CONFIG_P012_D012] */
  { 3,
    {CARRIER_CONFIG_INVALID  , TRUE} ,
    {CARRIER_CONFIG_P012_D01 , TRUE }, 
    {HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_DATA_ACTIVE   } },

  /* Carrier configuration # 1 [CARRIER_CONFIG_P012_D01] */
  { 3,
    {CARRIER_CONFIG_P012_D012, TRUE },
    {CARRIER_CONFIG_P012_D0, TRUE }  , 
    {HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_DATA_INACTIVE } },

  /* Carrier configuration # 2 [CARRIER_CONFIG_P012_D0] */
  { 3,
    {CARRIER_CONFIG_P012_D01, TRUE } ,
    {CARRIER_CONFIG_P01_D01, FALSE}  , 
    {HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_DATA_INACTIVE , 
     HDRRMAC4DRV_CH_ST_DATA_INACTIVE } },

  /* Carrier configuration # 3 [CARRIER_CONFIG_P01_D01] */
  { 2,
    {CARRIER_CONFIG_P012_D012, FALSE},
    {CARRIER_CONFIG_P01_D0, TRUE }   , 
    {HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_INACTIVE      } },

  /* Carrier configuration # 4 [CARRIER_CONFIG_P01_D0] */
  { 2,
    {CARRIER_CONFIG_P01_D01, TRUE }  ,
    {CARRIER_CONFIG_P0_D0, FALSE }   , 
    {HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_DATA_INACTIVE , 
     HDRRMAC4DRV_CH_ST_INACTIVE      } },

  /* Carrier configuration # 5 [CARRIER_CONFIG_P0_D0] */
  { 1,
    {CARRIER_CONFIG_P01_D01, FALSE}  ,
    {CARRIER_CONFIG_INVALID, FALSE}  , 
    {HDRRMAC4DRV_CH_ST_DATA_ACTIVE   , 
     HDRRMAC4DRV_CH_ST_INACTIVE      , 
     HDRRMAC4DRV_CH_ST_INACTIVE      } }
};


/* Carrier priorities that can be used during carrier config transition */
typedef enum
{
  /* RTCMAC priority */
  RTCMAC,

  /* Overall priority */
  OVERALL

} hdrrmac4drv_carrier_priority_enum_type;


/* Carrier state transitions */
typedef enum
{
  /* Request carrier addition */
  REQUEST_ADD,

  /* Activate carrier */
  ACTIVATE_DATA,

  /* Schedule carrier for deactivation */
  DEACTIVATE_DATA,

  /* Schedule carrier for dropping */
  DROP,

  /* Do nothing */
  DO_NOTHING

} hdrrmac4drv_carrier_state_transition_enum_type;


/* Carrier configuration transition information */
typedef struct
{
  /* Flag to indicate that the carrier configuration transition is valid */
  boolean valid_transition;

  /* Carrier priority to be used for the transition */
  hdrrmac4drv_carrier_priority_enum_type priority;

  /* Individual carrier state transitions */
  hdrrmac4drv_carrier_state_transition_enum_type 
    carrier_transition[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

} hdrrmac4drv_carrier_config_transition_type;


/* Carrier configuration transitions */
hdrrmac4drv_carrier_config_transition_type 
  hdrrmac4drv_carrier_config_transition_table
  [CARRIER_CONFIG_MAX][CARRIER_CONFIG_MAX] = 
{
  /* Transitions from configuration # 0 */
  { { TRUE , RTCMAC , {ACTIVATE_DATA , ACTIVATE_DATA   , ACTIVATE_DATA   } },
    { TRUE , RTCMAC , {ACTIVATE_DATA , ACTIVATE_DATA   , DEACTIVATE_DATA } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } } },

  /* Transitions from configuration # 1 */
  { { TRUE , RTCMAC , {ACTIVATE_DATA , ACTIVATE_DATA   , ACTIVATE_DATA   } },
    { TRUE , RTCMAC , {ACTIVATE_DATA , ACTIVATE_DATA   , DEACTIVATE_DATA } },
    { TRUE , RTCMAC , {ACTIVATE_DATA , DEACTIVATE_DATA , DEACTIVATE_DATA } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } } },

  /* Transitions from configuration # 2 */
  { { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { TRUE , RTCMAC , {ACTIVATE_DATA , ACTIVATE_DATA   , DEACTIVATE_DATA } },
    { TRUE , RTCMAC , {ACTIVATE_DATA , DEACTIVATE_DATA , DEACTIVATE_DATA } },
    { TRUE , OVERALL, {ACTIVATE_DATA , ACTIVATE_DATA   , DROP            } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } } },

  /* Transitions from configuration # 3 */
  { { TRUE , OVERALL, {ACTIVATE_DATA , ACTIVATE_DATA   , REQUEST_ADD     } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { TRUE , OVERALL, {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { TRUE , RTCMAC , {ACTIVATE_DATA , DEACTIVATE_DATA , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } } },

  /* Transitions from configuration # 4 */
  { { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { TRUE , RTCMAC , {ACTIVATE_DATA , ACTIVATE_DATA   , DO_NOTHING      } },
    { TRUE , RTCMAC , {ACTIVATE_DATA , DEACTIVATE_DATA , DO_NOTHING      } },
    { TRUE , OVERALL, {ACTIVATE_DATA , DROP            , DROP            } } },

  /* Transitions from configuration # 5 */
  { { TRUE , OVERALL, {ACTIVATE_DATA , REQUEST_ADD     , REQUEST_ADD     } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { TRUE , OVERALL, {ACTIVATE_DATA , REQUEST_ADD     , DO_NOTHING      } },
    { FALSE, RTCMAC , {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } },
    { TRUE , OVERALL, {DO_NOTHING    , DO_NOTHING      , DO_NOTHING      } } }
};

/* One of the following six possible carrier arrangements are randomly
   selected as the initial carrier arrangement in sort_caput_carriers()
   to remove bias in carrier sorting. */
uint8 hdrrmac4drv_carrier_arrangement[6][HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] =
{
  {0, 1, 2},
  {0, 2, 1},
  {1, 2, 0},
  {1, 0, 2},
  {2, 1, 0},
  {2, 0, 1}
};

/* All empty entries in ranked carrier list sent to FW in SUBFRAME_CFG_CMD 
   msg should be set to this value. */
#define HDRRMAC4DRV_RANKED_CARRIER_LIST_EMPTY_ENTRY  0xFFFF

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


/* Tracked PA states */ 
typedef enum
{
  PA_ON_PENDING,
  PA_ON,
  PA_OFF_PENDING,
  PA_OFF
} hdrrmac4drv_pa_status_enum_type;

/* Type definition for rri to pilot ratios */ 
typedef struct
{
  int16 subpacket_gain[HDRRMACCMN_MAX_NUM_SUBPACKETS];
} hdrrmac4drv_pkt_gains_type;


/* Store information for each packet we need to track */ 
typedef struct
{
  uint8 packet_id;
    /* Id of packet. */

  uint8 subpacket;
    /* Current subpacket (0-3) */
  
  uint8 rri;
    /* RRI for packet. */

  uint8 tm;  
    /* Transmission mode for packet */

  uint8 tt;
    /* Termination target.  This is used to calculate CAPUT priority. */

  uint16 data_length;
    /* Stored for pkt logging */

  uint8 tx_rri;
    /* This is the actual Tx'd rri for the subframe. It should mirror rri,
       except in the case of RLSI or DRC supervision, in which case it will
       indicate rate 0. This value is used by the permitted payload check. */
  
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  boolean not_data_lmtd;
    /* TRUE if the packet was not data limited.  FALSE otherwise. */
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */
     
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  boolean boosted_t2p;
    /* TRUE if the packet is T2P boosted. FALSE otherwise. */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
 
} hdrrmac4drv_packet_info_type;

/* Overhead channel gains */
typedef struct
{
  boolean dsc_enabled; /* DSC enabled flag */
  int32   drc;          /* DRC channel gain in  0.5 dB */
  int32   ack;          /* ACK channel gain in  0.5 dB */
  int32   dsc;          /* DSC channel gain in -0.5 dB */
} hdrrmac4drv_channel_gains_struct_type;


/* Per channel information */
typedef struct
{
  /* Carrier band class and channel information */
  sys_channel_type channel_record;

  /* Carrier state */
  hdrrmac4drv_carrier_state_enum_type carrier_state;

  /* Fwd Channel ID / Subactive set index given in TCA */
  uint8 fwd_link_index;

  /* Subaset index maintained by SRCH.  This is determined in accordance with 
     persistent carrier assignment rule. */
  uint8 demod_index;

  /* Drop rank */
  uint8 drop_rank;

  /* Channel configuration attributes */
  hdrrmac4_channel_config_struct_type * channel_config;

  /* Channel Gains */
  hdrrmac4drv_channel_gains_struct_type channel_gains;

  /* Pilot + DRC + RRI + max(ACK, DSC) gains in Q.10, linear */
  uint32 pilot_plus_overhead_gains;

  /* Filtered RAB limited traffic power based on sum of potential T2P 
     outflow during last subframe */
  uint32 filt_rab_limited_p_data;

  /* Filtered power margin (Q22.10 linear). */
  uint32 filt_pwr_margin;

  /* Instantaneous transmit pilot power */
  uint32 txp_inst;

  /* Keep track of last accepted packets and their rate, mode, etc. */
  hdrrmac4drv_packet_info_type interlace_history
    [HDRSCMRMACCMN_MAX_NUM_INTERLACES];

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
  uint8 rri_history[HDRSCMRMACCMN_MAX_NUM_INTERLACES];
    /* Keep track of last accepted packet sizes */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

  /* Holds MARQ packet info for logging */
  hdrrmac4drv_packet_info_type marq_info;

  /* Flag to indicate whether MARQ info is valid */
  boolean marq_info_valid;

  /* Temp. storage for a loaded packet's RRI (waiting to be accepted) */
  uint8 rri_pending;

  /* Temp. storage for a loaded packet's TM (waiting to be accepted) */
  uint8 tm_pending;

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  boolean boosted_t2p_pending;
    /* Temp. storage for a loaded packets's T2P boosted flag value
       (waiting to be accepted) */
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  boolean not_data_lmtd_pending;
    /* Temp. storage for a loaded packet's NotDataLimited flag value
       (waiting to be accepted) */

  uint32 not_data_lmtd_tput;
    /* Served throughput in kbps calculated for packets formed when AT was
       not data limited. The value is stored in Q24.8 units. */

  uint32 curr_tput;
    /* Served throughput in kbps calculated for all transmitted packets.
       The value is stored in Q24.8 units. */
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

  /* Filtered pilot strength for each active set pilot. */
  uint16 filtered_ps[HDRSRCH_SUBASET_MAX_SIZE];

  /* Pilit PN for each active set pilot -- used only for tracking. */
  uint16 pn_offset[HDRSRCH_SUBASET_MAX_SIZE];

  /* DRC lock thresholds */
  uint16 drc_lock_threshold;
  uint16 drc_unlock_threshold;

  /* Pilot strength of best sector */
  int    best_sector_db;

  /* TxT2P adjust value of best sector in 1/4 dB */
  int8   best_sector_delta_t2p;

  /* TxT2PMax(PS) */
  uint32 txt2p_max_ps;

  /* TxT2PMax(PS) calculated at the leftmost point of PilotStrenght axis.
     It is updated every time per-carrier attributes are updated.  This value 
     minus per-carrier filt_txp_diff is the upper bound for TxT2PMax_PS. */
  int32 txt2p_max_ps_leftmost;

  /* DRC superivision flag */
  boolean  drc_supervision_expired;

  /* Filtered transmit pilot Q.10 dBm. */
  int32  filt_txp_dbm;

  /* Filtered transmit pilot power differential with respect to the minimum
     pilot power carrier in Q.2 dBm. */
  int32  filt_txp_diff;

  /* Flag set when a carrier is to be dropped due to transmit pilot
     power differential. */
  boolean  drop_carrier_txp_diff;

  /* Flag to indicate that the carrier is reportable */
  boolean  reportable;

  /* Left justified 2-bit mac trailer, for encoder */
  uint8    mac_trailer;

  /* Data length of packet */
  uint16   data_length;

  /* Pointer to packet data */
  dsm_item_type * pkt_ptr;

  /* Flag to indicate that a carrier in transient state has empty interlaces 
     and is okay to deactivate / shift to inactive state */
  boolean  okay_to_deactivate;

  /* Bias to be applied to carrier filtered transmit pilot power before 
     determining its RTCMAC priority.  This is a part of enhanced PA backoff 
     feature. */
  int32    funnel_bias;

  /* TRUE if a carrier is to be enabled per TCA but has been kept off till
     MCDO evaluation */
  boolean  off_till_mcdo_eval;

  /* TRUE if the carrier is removed by the last TCA and is awaiting TCA
     update processing to be put to inactive state. Carriers with this
     flag set should not be enabled when RMAC is started */
  boolean  removed_by_tca;

} hdrrmac4drv_per_carrier_info_struct_type;

/* Prioritized carriers */
typedef struct
{
  /* Carriers sorted by RTCMAC priorities based on transmit pilot powers */
  uint8 rtcmac[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Carriers sorted by overall priorities i.e. drop_rank + RTCMAC priority */
  uint8 overall[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Carriers sorted by overall priorities i.e. subpacket_id - term_target */
  uint8 caput[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

} hdrrmac4drv_prioritized_carriers_type;

/* SLP Carrier */
typedef struct
{
  /* Current SLP carrier */
  uint8 current;

  /* Current SLP carrier has data to be transmitted in this subframe
     SLP carrier cannot be switched when there is data in its queue */
  boolean has_data;

} hdrrmac4drv_slp_carrier_type;


/* Previous subframes processed */
typedef struct
{
  /* Last subframe when subpackets_to_RLSI was calculated */
  uint8 rlsi;

  /* Last subframe when subpackets_to_suspend was calculated */
  uint8 suspend;

} hdrrmac4drv_previous_subframe_type;


struct
{
  /* Function to call each subframe */
  void  (*isr_ptr)(void);

  /* Packet interlace, updated in hdrrmac4drv_get_prev_tx_status().
     Thus, after hdrrmac4drv_get_prev_tx_status(), this holds the interlace 
     for the next packet to be loaded.  After hdrrmac4drv_send_packet(), it 
     is considered to be holding the currently transmitting interlace */
  uint8 interlace;

  /* Subframe number, updated in hdrrmac4drv_get_prev_tx_status().
     After hdrrmac4drv_get_prev_tx_status(), this holds the subframe 
     number for the next packet to be loaded. */
  uint8 subframe_id;  //rename to subframe_n?

  /* Local copies of attributes */
  int16 aux_pilot_gain;

  /* Minimum payload size that uses the auxiliary pilot */
  hdrrmaccmn_payload_size_enum_type aux_pilot_min_payload;

  /* Default RRI gain */
  int16 rri_default;

  /* Table used to load RRI to encoder */
  hdrrmac4drv_pkt_gains_type rri_to_pilot[HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS];

  /* Number of enabled channels */ 
  uint8 num_chans; 

  /* List of enabled (OPEN) channels */
  uint8 open_chans[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Number of enabled channels carrying data */
  uint8 num_chans_data;

  /* List of enabled channels carrying data
     This is a list of OPEN channels that are not in DATA_INACTIVE state */
  uint8 open_chans_data[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Number of active/enabled channels in previous subframe
     This number is updated in send_packet() and is used to look up first
     num_chans_prev CAPUT prioritized carriers in when returning previous
     subframe tx info */ 
  uint8 num_chans_prev; 

  /* Current carrier configuration */
  hdrrmac4drv_carrier_config_id_enum_type carrier_config_id;

  /* Target carrier configuration */
  hdrrmac4drv_carrier_config_id_enum_type target_carrier_config_id;

  /* Number of extra carriers requested in this subframe */
  uint8 num_req_carriers;

  /* Number of reportable carriers */
  uint8 num_reportable_carriers;

  /* Per carrier info */
  hdrrmac4drv_per_carrier_info_struct_type 
    per_carrier_info[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Prioritized carriers */
  hdrrmac4drv_prioritized_carriers_type prioritized_carriers;

  /* SLP carrier */
  hdrrmac4drv_slp_carrier_type slp_carrier;

  /* Values used in the pilot filtering calculation */
  uint16 ps_filter_mult;
  uint16 ps_filter_shift;
  
  /* PA status indicator */
  hdrrmac4drv_pa_status_enum_type pa_status;

  /* Transmit frame offset.  */
  uint8    frame_offset;

  /* Interrupt offset, based on frame offset */
  uint16   interrupt_offset;

  /* RLSI start time */
  qword    silence_begin_time;

  /* RLSI frame counter */
  int      silence_count;

  /* RLSI is upon us (indicator) */
  boolean  silence_now_flag;
           
  /* Indicates whether max power protection is enabled */
  boolean  throttle_disable;

  /* Indicates whether rpc has been disabled */
  boolean  rpc_disabled;

  /* Values used for calulating the current subframe number. */
  uint16   subframe_halfslot_offset; 
  uint16   subframe_offset;

  /* Early termination (test mode) config. */
  uint16   early_term_config;

  /* Shutdown Callback */
  hdrrmac4drv_shutdown_cb_type shutdown_cb;

  /* Track whether a graceful shutdown is pending */
  /* (Graceful shutdown == make sure all packets in progress are completed) */
  boolean  shutdown_pending;
  
  /* When a timing error occurs that needs an idle subframe for recovery */
  boolean  timing_error;

  /* Slot when link is scheduled to be suspended. */
  uint32   suspend_slot;

  /* Set when subframes to suspend is ot be computed at the beginning of a
     subframe */
  boolean  compute_subframes_to_suspend;

  /* Countdown until suspend (i.e, tuneaway). */
  int16 subframes_to_suspend;                   

  /* TRUE if AT is not in silence interval */
  boolean at_not_in_rlsi;

  /* Previous subframe */
  hdrrmac4drv_previous_subframe_type prev_subframe;

  /* Constant used in converting values from DSP units to dBm 
     K = -73 (cellular) and -76 (PCS) */
  int8 k;

  /* Total transmit pilot power */
  int8 total_tx_pilot_pwr;

  /* Backoff subtracted total available power for one, two and three carrier
     configurations in Q.22 mW.  When RevB PA backoff is disabled from RMAC, 
     the array reflects estimates of available power for entries other than
     the current configuration . i.e. if AT is in a two carrier configuration,
     pmax[1] would show currently available max power and pmax[0] and pmax[2]
     would contain estimates of available power for one and three carrier 
     configurations respectively. */
  uint32 pmax[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Available transmit power to be used when AT is in funnel state. 
     When in funnel state, the value should be used for headroom distribution 
     and carrier drop calculations */
  uint32 pmax_neq;

  /* Half slot number within a subframe.  Used for time checking */
  uint8 halfslot_start;

  /* Indicates whether above subframe packet values have been updated. */
  boolean current_tx_updated;

  /* TRUE if the first TCA after access has been processed.  This is used
     for closed loop pilot power initialization.  Closed loop is -
     - initialized to carrier-0 closed loop value for all carriers
       for the first TCA after access
     - cleared for all added carriers for all subsequent TCAs */
  boolean processed_first_tca_after_access;

  /* TRUE if CPU based RL flow control mode is turned ON.  When TRUE, data is 
     disabled on non-SLP carriers */
  boolean flow_control_on;

  /* TRUE if enhanced PA backoff is enabled */
  boolean enh_pa_backoff_enabled;

  /* TRUE if AT is in funnel mode */
  boolean in_funnel_mode;

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
  /* TRUE if data is expected in the next subframe.  The flag is reset at 
     the beginning of subframe processing and is set if an RL packet is 
     loaded to the encoder at the end of subframe processing.
     The flag status should be read only in HDRTX task context. */
  boolean rl_packet_scheduled;

#endif /* defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/

  /* In case of a new connection setup, number of subframes after which
     MCDO feasibility is to be evaluated. */
  uint8   subframes_till_mcdo_eval;

  /* In case of a new connection setup, the best and the only enabled
     RL carrier till MCDO evaluation takes place. */
  uint8   ch_best;

  /* Number of subframes after which PA enable command should be sent to FW.*/
  uint8   subframes_till_pa_enable;

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
  /* Pointer to boosted TxT2P parameters */
  hdrrmaccmn_tx_t2p_info_type * boosted_t2p_info_ptr;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD 
  uint8 rri_history_idx;
    /* Track last written RRI in history buffer. */

  /* TRUE if permitted payload optimization is enabled */
  boolean perm_pyld_opt_enabled;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

} hdrrmac4drv;

/* List of channels on which data was sent during last subframe.  The channels
   are arranged in CAPUT priority order.  The list is updated in send_packet()
   function and is used for logging pkt accept/reject statuses. */
uint8 hdrrmac4drv_prev_sf_sent_channel_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

/* Carrier status info */
typedef struct
{
  /* HARQ info valid flag */
  boolean harq_info_valid;

  /* Holds info on current tx'ing packet */
  hdrrmac4drv_packet_info_type * packet_info;

  /* PARQ NAKs */
  boolean parq_nak_info;

  /* MARQ info valid flag */
  boolean marq_info_valid;

  /* Holds info on packet that *could* be MARQ-NAK'd */
  hdrrmac4drv_packet_info_type * marq_info;

  /* Working vars for dsp status */
  uint16 arq_status;
  uint16 encode_status;

} hdrrmac4drv_carrier_status_info_type;


/* PA headroom per-carrier DSP info */
typedef struct
{
  /* Instantaneous transmit pilot power */
  int16 inst_txp;

  /* Transmit pilot power
     = Peak transmit pilot power if 
         hdrrmac4drv_margin_loop_use_peak == 1
     = Instantaneous transmit pilot power if 
         hdrrmac4drv_margin_loop_use_peak == 0 */
  int16 txp;

  /* Filtered transmit pilot power */
  int16 filt_txp;

  /* Power margin */
  int16 pwr_margin;

} hdrrmac4drv_hdrm_carrier_info_type;

/* PA headroom DSP info */
typedef struct
{
  /* Maximum available transmit power when not in funnel mode */
  int16 p_max;

  /* Maximum available transmit power when in funnel mode */
  int16 p_max_neq;

  /* Per-carrier DSP info */
  hdrrmac4drv_hdrm_carrier_info_type c[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

} hdrrmac4drv_hdrm_dsp_info_type;


/*-------------------------------------------------------------------------*/

/* Pointer to RMAC common parameters */
/* Make pointer to const; don't forget to init */
static hdrrmac_common_type * hdrrmac4drv_common;

/* Pointer to RMAC4 configuration parameters */
static hdrrmac4_config_struct_type * hdrrmac4drv_config;


/* Power margin structure */ 
struct
{
  /* sw power margin */
  int16 pwr_margin; 

  /* tx power margin from MDSP for previous sf */
  int16 tx_dsp_pwr_margin_prev;

  /* min power margin */
  int16 min;        

  /* max power margin */
  int16 max;

  /* power margin up step */
  int16 up_step;

  /* power margin down step */
  int16 down_step;

} hdrrmac4drv_pwr_margin;


/*---------------------------------------------------------------------------
                             Protocol Profiling
---------------------------------------------------------------------------*/
/* Turn the messages displaying profiling information on/off */
boolean hdrrmac4drv_enable_profiling = FALSE;


/*---------------------------------------------------------------------------
                       Logging / Metric Tracking Structures
---------------------------------------------------------------------------*/

/* Info for RL gains log packet */
struct
{
  /* Current subframe count  */
  uint8 sf_count;           

  /* Previous subframe PA enabled status */
  boolean prev_pa_enabled;    

  /* Backoff used */
  /* RMG: No value is currently given to backoff.  
     RF interface to be obtained to write backoff value. */
  uint8 backoff;

  /* Max available Tx Power */
  uint16 max_avail_tx_pwr;

  /* Flags to enable channel logging.  The flag for a channel is set if
       it has valid data during atleast one of the last 
       'sf_count' subframes when the packet is logged */
  boolean enable_channel_logging[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Log data */
  hdrlog_rl_gains_pwr_info_type
    pwr_info[HDRLOG_MAX_SUBFRAMES_PER_MC_RL_GAINS_PKT];        
  hdrlog_rl_gains_per_chan_subframe_wide_info_type
    log[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]
    [HDRLOG_MAX_SUBFRAMES_PER_MC_RL_GAINS_PKT];

} hdrrmac4drv_gain_log_info;


/* Info for RL ARQ status log packet */
struct
{
  /* Current subframe count  */
  uint8 sf_count;           

  /* Flags to enable channel logging.  The flag for a channel is set if
       it has valid data during atleast one of the last 
       'sf_count' subframes when the packet is logged */
  boolean enable_channel_logging[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Log data */
  hdrlog_rl_arq_status_subframe_wide_info_type 
    log[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]
    [HDRLOG_MAX_SUBFRAMES_PER_REVB_RL_ARQ_PKT];        

} hdrrmac4drv_arq_status_log_info;


/* Info for RL Pkt status log packet */
struct
{
  /* Per channel packet count */
  uint8 packet_count[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Log data */
  hdrlog_rl_pkt_status_packet_wide_info_type 
    log[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]
    [HDRLOG_MAX_ENTRIES_PER_MC_RL_PKT_STATUS_PKT];

} hdrrmac4drv_pkt_status_log_info;


/* TODO: Added for compilation till RF_HDR_REVB_RATE_OFFSET is defined */
#ifdef FEATURE_RF_SVDO_API
#define RFM_HDR_REVB_RATE_OFFSET             RFM_HDR_REVA_RATE_OFFSET  
#else
#define RF_HDR_REVB_RATE_OFFSET              RF_HDR_REVA_RATE_OFFSET  
#endif /* FEATURE_RF_SVDO_API */

/* RL Rate logging metrics */
typedef PACK(struct)
{
  /* Flag to trigger submission of RLMetrics log packet in next subframe */
  boolean submit_rl_metrics;

  /* Log sequence number */
  uint32 metrics_sequence_num;

  /* Reverse link packets dropped */
  uint32 mc_rl_pkts_dropped;

  /* Log data */
  hdrlog_rl_metrics_chan_wide_info_type 
    log[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

} hdrrmac4drv_metrics_type;

/* RL Metrics log info */
hdrrmac4drv_metrics_type hdrrmac4drv_metrics;


/* RABUpdateInfo log */ 
struct
{
  /* Current subframe count  */
  uint8 sf_count;           

  /* Flags to enable channel logging.  The flag for a channel is set if
     it has valid data during atleast one of the last 
     'sf_count' subframes when the packet is logged */
  boolean enable_channel_logging[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Log data */
  hdrlog_rab_update_subframe_wide_info_type 
    log[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]
    [HDRLOG_MAX_SUBFRAMES_PER_REVB_RAB_UPDATE_PKT];        

} hdrrmac4drv_rab_update_log_info;


/* PA Headroom and Carrier Management log */
struct
{
  /* Current subframe count  */
  uint8 sf_count;           

  /* Flags to enable channel logging.  The flag for a channel is set if
       it has valid data during atleast one of the last 
       'sf_count' subframes when the packet is logged */
  boolean enable_channel_logging[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Log data: per-subframe non-per-carrier data */
  hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_info_type
    log_sf[HDRLOG_MAX_SUBFRAMES_PER_MC_PA_HDRM_CM_PKT];

  /* Log data: per-subframe per-carrier data */
  hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_chan_wide_info_type 
    log_carrier[HDRLOG_MAX_SUBFRAMES_PER_MC_PA_HDRM_CM_PKT]
    [HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

} hdrrmac4drv_hdrm_cm_log_info;


#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

#ifdef HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#error code not present
#endif

#ifdef HDRRMAC4DRV_EQUAL_HEADROOM_DISTRIBUTION
boolean hdrrmac4drv_enable_equal_headroom_dist = TRUE;
#endif


/* Carrier management power margin loop configuration:
   Use instantaneous pilot power */
static uint8 hdrrmac4drv_margin_loop_use_peak = 0;

/* Leftmost point in pilotstrength axis in TxT2PMax_PilotStrength grid. */
#define HDRRMAC4DRV_TXT2PMAXPS_MAX_PS_POINT  0

/*---------------------------------------------------------------------------
                             Function Declarations 
---------------------------------------------------------------------------*/
void hdrrmac4drv_init_carrier_management( void );

void hdrrmac4drv_submit_pa_hdrm_cm_info_log( void );

void hdrrmac4drv_update_open_chan_list( void );

void hdrrmac4drv_update_open_chan_data_list( void );

void hdrrmac4drv_update_num_reportable_carriers( void );

void hdrrmac4drv_sync_start_pa( void );

void hdrrmac4drv_update_funnel_state( void );

void hdrrmac4drv_update_funnel_bias( void );

void hdrrmac4drv_eval_mcdo_connection_setup( void );

/*---------------------------------------------------------------------------
                PA Headroom and Carrier Management Unit Test
---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST

/* Unit test enable flag */
boolean hdrrmac4drv_headroom_ut_enable = FALSE;

/* Headroom unit test run length in subframes */
#define HDRRMAC4DRV_HEADROOM_UT_RUN_LENGTH      50

/* Unit test subframe */
static uint8 hdrrmac4drv_ut_sf = 0;

/* Filtered transmit pilot power delta in Q.10 dB units.
   This is used in RTCMAC priority computation */
static uint32 hdrrmac4drv_ut_filt_txp_delta = 1024;

/* Minimum T2P on best carrier in Q.2 dB units */
static uint32 hdrrmac4drv_ut_txt2pmin_db = 26;

/* T2P_MaxRate*(1+AuxPilotGain) in Q.2 dB units */
static uint32 hdrrmac4drv_ut_t2p_maxrate_db = 86;

/* UT per channel info */
typedef struct
{
  /* Peak transmit pilot power in 
     ((K + 63.8) - X dBm)*65536/DynamicRange units */
  int32 txp;

  /* Power margin in 
     (X dB)*65536/DynamicRange units */
  int32 pwr_margin;

  /* Filtered transmit pilot power in
     ((K + 63.8) - X dBm)*65536/DynamicRange units */
  int32 filt_txp;

  /* Filtered RAB limited traffic power in Q.22 linear units */
  uint32 filt_rab_limited_p_data;

  /* TxT2PMax_ps in Q.2 dB */
  uint32 txt2p_max_ps;

  /* Dropping rank */
  uint8  dropping_rank;

  /* Carrier state */
  hdrrmac4drv_carrier_state_enum_type carrier_state;

  /* Filtered power margin in Q22.10 units */
  uint32 filt_pwr_margin;

} hdrrmac4drv_headroom_ut_per_chan_info_type;

/* UT per subframe info */
typedef struct
{
  /* Total available tx power in 
     ((K + 63.8) - X dBm)*65536/DynamicRange units */
  int32 p_max;

  /* Carrier configuration */
  hdrrmac4drv_carrier_config_id_enum_type carrier_config;

  /* Previous subframe SLP carrier */
  uint8 prev_slp_carrier;

  /* Flag to indicate that SLP queue has data in current subframe */
  boolean slp_has_data;

  /* Per carrier info */
  hdrrmac4drv_headroom_ut_per_chan_info_type 
    c[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

} hdrrmac4drv_headroom_ut_per_sf_info_type;


/* Headroom unit test input data */
static hdrrmac4drv_headroom_ut_per_sf_info_type 
  hdrrmac4drv_headroom_ut_input[HDRRMAC4DRV_HEADROOM_UT_RUN_LENGTH] =
{
  /* Subframe # 0 */
   {-18880,
    CARRIER_CONFIG_P012_D012,
    1,
    TRUE,
    { {25920, -1920, 25920, 1326, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 513},
      {6720, -1920, 6720, 333165, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 513},
      {25920, -1920, 25920, 1326, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 513} } },

  /* Subframe # 1 */
   {-18880,
    CARRIER_CONFIG_P012_D01,
    1,
    FALSE,
    { {18240, -768, 19997, 8636, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 645},
      {4480, -768, 5380, 469713, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 645},
      {23360, -1152, 24355, 2049, 108, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 595} } },

  /* Subframe # 2 */
   {-18880,
    CARRIER_CONFIG_P012_D01,
    2,
    TRUE,
    { {10560, 384, 12395, 100177, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 910},
      {2240, 384, 3388, 786464, 108, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 910},
      {20800, -384, 22044, 3919, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 743} } },

  /* Subframe # 3 */
   {-18880,
    CARRIER_CONFIG_P012_D01,
    2,
    FALSE,
    { {2880, 1536, 4717, 1202559, 108, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1345},
      {0, 1536, 1207, 1396992, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1345},
      {18240, 384, 19536, 8008, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 960} } },

  /* Subframe # 4 */
   {-18880,
    CARRIER_CONFIG_P012_D01,
    1,
    TRUE,
    { {-4800, 2688, -2963, 14457025, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2019},
      {-2240, 2688, -1020, 2525039, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2019},
      {15680, 1152, 16987, 16641, 108, 0, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1255} } },

  /* Subframe # 5 */
   {-18880,
    CARRIER_CONFIG_P012_D01,
    2,
    FALSE,
    { {-12480, 3840, -10643, 173811217, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 3048},
      {-4480, 3840, -3257, 4586281, 108, 0, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 3048},
      {13120, 1920, 14429, 34722, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1649} } },

  /* Subframe # 6 */
   {-18880,
    CARRIER_CONFIG_P012_D01,
    2,
    TRUE,
    { {-4800, 2688, -9036, 94878758, 48, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 2871},
      {-6720, 2688, -5496, 8341393, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2871},
      {10560, 1152, 11870, 72522, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1599} } },

  /* Subframe # 7 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    0,
    FALSE,
    { {2880, 1536, -7148, 47820998, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2325},
      {-8960, 1536, -7736, 15176707, 108, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 2325},
      {8000, 384, 9310, 151508, 108, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1388} } },

  /* Subframe # 8 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    1,
    TRUE,
    { {10560, 384, -5226, 23928764, 48, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1750},
      {-11200, 384, -9976, 27615999, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1750},
      {5440, -384, 6750, 316539, 108, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1140} } },

  /* Subframe # 9 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    2,
    FALSE,
    { {18240, -768, -3300, 11965256, 48, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1264},
      {-13440, -768, -12216, 50252325, 108, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1264},
      {2880, -1152, 4190, 661341, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 908} } },

  /* Subframe # 10 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    0,
    TRUE,
    { {25920, -1920, -1373, 5982670, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 888},
      {-15680, -1920, -14456, 91443931, 48, 0, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 888},
      {320, -1920, 1630, 1381736, 108, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 711} } },

  /* Subframe # 11 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    1,
    FALSE,
    { {18240, -768, 551, 2992209, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 833},
      {-13440, -768, -13994, 82166291, 48, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 833},
      {-2240, -1152, -930, 3865036, 108, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 694} } },

  /* Subframe # 12 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    2,
    TRUE,
    { {10560, 384, 2403, 1514370, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1004},
      {-11200, 384, -12934, 61110791, 48, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1004},
      {-4800, -384, -3490, 11518352, 80, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 793} } },

  /* Subframe # 13 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    0,
    FALSE,
    { {2880, 1536, 2631, 1138804, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1392},
      {-8960, 1536, -11604, 41561406, 48, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1392},
      {-3520, 384, -3505, 13030775, 80, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 984} } },

  /* Subframe # 14 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    1,
    TRUE,
    { {-4800, 2688, -3059, 8542552, 108, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 2043},
      {-6720, 2688, -10120, 26828955, 48, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2043},
      {-1120, 1152, -2561, 10202020, 80, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1267} } },

  /* Subframe # 15 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    2,
    FALSE,
    { {-12480, 3840, -10646, 170853980, 48, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 3060},
      {-4480, 3840, -8537, 16738240, 48, 0, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 3060},
      {1280, 1920, -1257, 6970099, 80, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1655} } },

  /* Subframe # 16 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    0,
    TRUE,
    { {-4800, 2688, -9039, 99282736, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2877},
      {-2240, 2688, -6884, 10195663, 48, 0, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 2877},
      {3680, 1152, 235, 4432660, 80, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1602} } },

  /* Subframe # 17 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    1,
    FALSE,
    { {2880, 1536, -7150, 50793839, 48, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 2328},
      {0, 1536, -5182, 6101591, 48, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2328},
      {6080, 384, 1841, 2696760, 80, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1389} } },

  /* Subframe # 18 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    2,
    TRUE,
    { {10560, 384, -5229, 25492778, 48, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1752},
      {2240, 384, -3442, 3602403, 48, 0, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1752},
      {8480, -384, 3524, 1591953, 80, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1140} } },

  /* Subframe # 19 */
   {-18880,
    CARRIER_CONFIG_P01_D01,
    1,
    FALSE,
    { {18240, -768, -3303, 12754362, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1264},
      {4480, -768, -1671, 2104332, 48, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1264},
      {10880, -1152, 5260, 919466, 80, 1, HDRRMAC4DRV_CH_ST_INACTIVE, 909} } },

  /* Subframe # 20 */
   {-18880,
    CARRIER_CONFIG_P01_D01,
    1,
    TRUE,
    { {25920, -1920, -1376, 6377844, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 889},
      {6720, -1920, 123, 1218749, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 889},
      {13280, -1920, 7036, 522341, 80, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 711} } },

  /* Subframe # 21 */
   {-18880,
    CARRIER_CONFIG_P01_D01,
    2,
    FALSE,
    { {18240, -768, 548, 3196895, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 833},
      {4480, -768, 1523, 912505, 108, 0, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 833},
      {15680, -1152, 8841, 283130, 80, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 694} } },

  /* Subframe # 22 */
   {-18880,
    CARRIER_CONFIG_P01_D01,
    2,
    TRUE,
    { {10560, 384, 2400, 1694306, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1004},
      {2240, 384, 1858, 1007860, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1004},
      {18080, -384, 10669, 149268, 80, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 793} } },

  /* Subframe # 23 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    0,
    FALSE,
    { {2880, 1536, 2630, 1999624, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1392},
      {0, 1536, 777, 1507690, 108, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1392},
      {20480, 384, 12516, 77335, 80, 2, HDRRMAC4DRV_CH_ST_INACTIVE, 984} } },

  /* Subframe # 24 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    1,
    TRUE,
    { {-4800, 2688, -3059, 14855558, 108, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 2043},
      {-2240, 2688, -1122, 2580388, 108, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2043},
      {22880, 1152, 14376, 39615, 80, 2, HDRRMAC4DRV_CH_ST_INACTIVE, 1267} } },

  /* Subframe # 25 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    0,
    FALSE,
    { {-12480, 3840, -10646, 174010483, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 3060},
      {-4480, 3840, -3280, 4613956, 108, 2, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 3060},
      {25920, 1920, 16259, 20072, 108, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1655} } },

  /* Subframe # 26 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    0,
    TRUE,
    { {-4800, 2688, -9039, 94978391, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2877},
      {-6720, 2688, -5501, 8355230, 108, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 2877},
      {23360, 1152, 17978, 10587, 108, 2, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1602} } },

  /* Subframe # 27 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    1,
    FALSE,
    { {2880, 1536, -7150, 47870815, 48, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 2328},
      {-8960, 1536, -7737, 15183625, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2328},
      {20800, 384, 19045, 6446, 108, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1389} } },

  /* Subframe # 28 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    2,
    TRUE,
    { {10560, 384, -5229, 23953673, 48, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1752},
      {-11200, 384, -9976, 27619459, 108, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1752},
      {18240, -384, 18614, 5631, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1140} } },

  /* Subframe # 29 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    1,
    FALSE,
    { {18240, -768, -3303, 11977711, 48, 1, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1264},
      {-13440, -768, -12216, 50254054, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1264},
      {15680, -1152, 16776, 7846, 108, 2, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 909} } },

  /* Subframe # 30 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    2,
    TRUE,
    { {25920, -1920, -1376, 5988897, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 889},
      {-15680, -1920, -14456, 91444796, 48, 2, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 889},
      {13120, -1920, 14386, 14434, 108, 3, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 711} } },

  /* Subframe # 31 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    0,
    FALSE,
    { {18240, -768, 548, 2995323, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 833},
      {-13440, -768, -13994, 82166723, 48, 2, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 833},
      {10560, -1152, 11861, 38959, 108, 3, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 694} } },

  /* Subframe # 32 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    0,
    TRUE,
    { {10560, 384, 2400, 1515927, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1004},
      {-11200, 384, -12934, 61111008, 48, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1004},
      {8000, -384, 9308, 115338, 108, 3, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 793} } },

  /* Subframe # 33 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    2,
    FALSE,
    { {2880, 1536, 2630, 1139582, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1392},
      {-8960, 1536, -11604, 41561514, 48, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1392},
      {5440, 384, 6749, 347156, 108, 3, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 984} } },

  /* Subframe # 34 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    1,
    TRUE,
    { {-4800, 2688, -3059, 8542941, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 2043},
      {-6720, 2688, -10120, 26829009, 48, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2043},
      {2880, 1152, 4190, 1047817, 108, 3, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1267} } },

  /* Subframe # 35 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    2,
    FALSE,
    { {-12480, 3840, -10646, 170854175, 48, 1, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 3060},
      {-4480, 3840, -8537, 16738267, 48, 2, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 3060},
      {320, 1920, 1630, 3164066, 108, 3, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1655} } },

  /* Subframe # 36 */
   {-18880,
    CARRIER_CONFIG_P01_D0,
    1,
    TRUE,
    { {-4800, 2688, -9039, 99282833, 48, 1, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 2877},
      {-2240, 2688, -6884, 10195676, 48, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2877},
      {-2240, 1152, -930, 9555183, 108, 3, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 1602} } },

  /* Subframe # 37 */
   {-18880,
    CARRIER_CONFIG_P0_D0,
    0,
    FALSE,
    { {2880, 1536, -7150, 50793887, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2328},
      {0, 1536, -5182, 6101598, 48, 2, HDRRMAC4DRV_CH_ST_INACTIVE, 2328},
      {-4800, 384, -3490, 28856118, 80, 3, HDRRMAC4DRV_CH_ST_INACTIVE, 1389} } },

  /* Subframe # 38 */
   {-18880,
    CARRIER_CONFIG_P0_D0,
    0,
    TRUE,
    { {10560, 384, -5229, 25492802, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1752},
      {2240, 384, -3442, 3602406, 48, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1752},
      {-3520, -384, -3505, 32693489, 80, 3, HDRRMAC4DRV_CH_ST_INACTIVE, 1140} } },

  /* Subframe # 39 */
   {-18880,
    CARRIER_CONFIG_P0_D0,
    0,
    FALSE,
    { {18240, -768, -3303, 12754374, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1264},
      {4480, -768, -1671, 2104334, 48, 2, HDRRMAC4DRV_CH_ST_INACTIVE, 1264},
      {-1120, -1152, -2561, 25607148, 80, 3, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 909} } },

  /* Subframe # 40 */
   {-18880,
    CARRIER_CONFIG_P0_D0,
    1,
    TRUE,
    { {25920, -1920, -1376, 6377850, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 889},
      {6720, -1920, 123, 1218750, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 889},
      {1280, -1920, -1257, 17498513, 80, 0, HDRRMAC4DRV_CH_ST_INACTIVE, 711} } },

  /* Subframe # 41 */
   {-18880,
    CARRIER_CONFIG_P0_D0,
    0,
    FALSE,
    { {18240, -768, 548, 3196898, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 833},
      {4480, -768, 1523, 912506, 108, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 833},
      {3680, -1152, 235, 10396015, 80, 0, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 694} } },

  /* Subframe # 42 */
   {-18880,
    CARRIER_CONFIG_P0_D0,
    1,
    TRUE,
    { {10560, 384, 2400, 1694307, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1004},
      {2240, 384, 1858, 1007860, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1004},
      {6080, -384, 1841, 5775611, 80, 0, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 793} } },

  /* Subframe # 43 */
   {-18880,
    CARRIER_CONFIG_P0_D0,
    2,
    FALSE,
    { {2880, 1536, 2630, 1999624, 108, 1, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1392},
      {0, 1536, 777, 1507690, 108, 2, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 1392},
      {8480, 384, 3524, 3090401, 80, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 984} } },

  /* Subframe # 44 */
   {-18880,
    CARRIER_CONFIG_P012_D012,
    2,
    TRUE,
    { {-4800, 2688, -3059, 14855558, 108, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2043},
      {-2240, 2688, -1122, 2580388, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2043},
      {10880, 1152, 5260, 1616261, 80, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1267} } },

  /* Subframe # 45 */
   {-18880,
    CARRIER_CONFIG_P012_D01,
    0,
    FALSE,
    { {-12480, 3840, -10646, 174010484, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 3060},
      {-4480, 3840, -3280, 4613956, 108, 2, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 3060},
      {13280, 1920, 7036, 833055, 80, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1655} } },

  /* Subframe # 46 */
   {-18880,
    CARRIER_CONFIG_P012_D01,
    2,
    TRUE,
    { {-4800, 2688, -9039, 94978391, 48, 1, HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE, 2877},
      {-6720, 2688, -5501, 8355230, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2877},
      {15680, 1152, 8841, 425270, 80, 0, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1602} } },

  /* Subframe # 47 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    0,
    FALSE,
    { {2880, 1536, -7150, 47870815, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 2328},
      {-8960, 1536, -7737, 15183625, 108, 2, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 2328},
      {18080, 384, 10669, 215701, 80, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1389} } },

  /* Subframe # 48 */
   {-18880,
    CARRIER_CONFIG_P012_D0,
    1,
    TRUE,
    { {10560, 384, -5229, 23953673, 48, 1, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1752},
      {-11200, 384, -9976, 27619459, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1752},
      {20480, -384, 12516, 108926, 80, 0, HDRRMAC4DRV_CH_ST_DATA_INACTIVE, 1140} } },

  /* Subframe # 49 */
   {-18880,
    CARRIER_CONFIG_P01_D01,
    0,
    FALSE,
    { {18240, -768, -3303, 11977711, 48, 1, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1264},
      {-13440, -768, -12216, 50254054, 108, 2, HDRRMAC4DRV_CH_ST_DATA_ACTIVE, 1264},
      {22880, -1152, 14376, 54840, 80, 0, HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE, 909} } }
};

/* Per channel pilot plus overhead gains in Q.10 linear units */
static uint32 
  hdrrmac4drv_headroom_ut_pilot_plus_ovg[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
{
  4816,
  4816,
  4816
};

/* UT Carrier management thresholds */
static hdrrmac4drv_carrier_mgt_thresh_type hdrrmac4drv_headroom_ut_cm_thresh = 
{
  /* TxT2P_Remain_Active */
  74,

  /* TxT2P_Activate */
  28,

  /* TxT2P_Deactivate */
  15,

  /* TxT2P_Min_Rate */
  35,

  /* TxT2P_Add */
  28
};

#endif /* FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST */

/* The following flag is used to pick the first open RL as the SLP carrier.  
 * It is used in: 
 * - hdrrmac4drv_start_open_state() for SLP carrier selection.
 * - hdrrmac4drv_stop_channels() to reset the flag when there are no open
 *   channels
 */
boolean hdrrmac4drv_slp_selected = FALSE;


/*---------------------------------------------------------------------------
                    RMAC4DRV Debug and Control Switches
---------------------------------------------------------------------------*/
/* The following bypasses PA headroom as well as carrier management
 * Used in: 
 * - hdrrmac4drv_get_max_t2p() to bypass PA headroom calculations. 
 * - hdrrmac4drv_start_open_state() for SLP carrier selection.
 */
boolean hdrrmac4drv_bypass_pa_headroom_alg = FALSE;

/* If PA headroom algorithm is bypassed max_t2p for all the channels is
 * set hdrrmac4drv_max_tx_t2p.
 * Used in:
 * - hdrrmac4drv_get_max_t2p()
 */
uint32  hdrrmac4drv_max_tx_t2p[3] = {0x78, 0x78, 0x78};

/* The following swtich is effective when PA headroom algorithm is NOT bypassed 
 * (i.e. hdrrmac4drv_bypass_pa_headroom_alg = FALSE)
 *  FALSE => Run carrier management but ignore its decisions
 *  TRUE  => Run carrier management and change carrier states and carrier config
 *           based on CM decisions.
 * Used in:
 * - hdrrmac4drv_set_new_config() to bypass CM decisions.
 */
boolean hdrrmac4drv_bypass_cm_decisions = FALSE;

/* Enable feature to ramp up packet size in case of missed subframes */
boolean hdrrmac4drv_enable_missed_subframes_ramp_up = TRUE;

/* Packet size ramp up will be undertaken if the number of subframes missed
   is greater than or equal to this threshold */
uint8 hdrrmac4drv_missed_subframes_threshold = 3;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*===========================================================================
FUNCTION HDRRMAC4DRV_CONFIG_PARAMS                                  EXTERNAL

DESCRIPTION
  To be called during powerup. Makes the configuration data in 
  hdrrmac4_config accessible via hdrrmac4drv_config.
  
DEPENDENCIES
  None.

PARAMETERS
  config - Pointer to RMAC4 configuration data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_config_params
( 
  hdrrmac4_config_struct_type * config
)
{
  hdrrmac4drv_config = config;

  /* Set common config pointer to NULL at powerup */
  hdrrmac4drv_common = NULL;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_config_params() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_PER_CHANNEL_ATTRIBUTES                  INTERNAL

DESCRIPTION
  Updates per-channel attributes (gaupable)
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE  - if per-channel attributes are availabe
  FALSE - if per-channel attributes are not available

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_update_per_channel_attributes( void )
{
  /* loop counter for physical channels */
  uint8 ch; 

  /* Flag to indicate if per channel attributes are available */
  boolean attributes_available = FALSE;

  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware hARQCoeffC2i config message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_HARQCOEFFC2I_CFG_CMD );

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    /* If physical_channel->logical_channel_attributes mapping is available
       configure H-ARQ and QRAB thresholds */
    if (hdrrmac4drv.per_carrier_info[ch].channel_config != NULL)
    {

#ifdef FEATURE_FACTORY_TESTMODE
      if( ftm_get_mode() == FTM_MODE )
      { 
        /* Set the FTM dictated H-ARQ Mode in FTM Mode*/
        hdrrmac4drv.per_carrier_info[ch].channel_config->arq_mode = 
          (hdrrmaccmn_arq_mode_enum_type) hdrhitmsg_get_rmac_params_ptr()->arq_mode;
      }
#endif

      /* -- ARQModeCC (GAUPable)-- */
      /* Configure H-ARQ MODE */
      if (hdrrmac4drv.per_carrier_info[ch].channel_config->arq_mode == 
          HDRRMACCMN_ARQ_MODE_ON_OFF)
      {
        /* ON-OFF Keying */
        fw_cmd.rmac_harqcoeffc2i_cfg.carrierMask |= (1 << ch);
        fw_cmd.rmac_harqcoeffc2i_cfg.hArqCoeffC2i[ch] = HDRMDSP_HARQ_MODE_1_VAL;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#error code not present
#endif

      }
      else
      {
        /* Bipolar (default) */
        fw_cmd.rmac_harqcoeffc2i_cfg.carrierMask |= (1 << ch);
        fw_cmd.rmac_harqcoeffc2i_cfg.hArqCoeffC2i[ch] = HDRMDSP_HARQ_MODE_0_VAL;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#error code not present
#endif

      }

      /* -- PilotStrengthQRABThresholdDRCUnlockCC (GAUPable)-- */

      /* Convert DRC unlock QRAB threshold to minus half dB units */
      hdrrmac4drv.per_carrier_info[ch].drc_lock_threshold = 
        hdrsrchmac_ecio_to_eng((hdrrmac4drv.per_carrier_info[ch].
        channel_config->ps_qrab_thresh.drc_lock >> 1) );

      /* -- PilotStrengthQRABThresholdDRCUnlockCC (GAUPable)-- */

      /* Convert DRC lock QRAB threshold to minus half dB units */
      hdrrmac4drv.per_carrier_info[ch].drc_unlock_threshold = 
        hdrsrchmac_ecio_to_eng((hdrrmac4drv.per_carrier_info[ch].
        channel_config->ps_qrab_thresh.drc_unlock >> 1) );

      /* Update TxT2Pmax_PS_Leftmost */
      hdrrmac4drv.per_carrier_info[ch].txt2p_max_ps_leftmost = 
        hdrrmacmath_interpolate_1d(
        HDRRMAC4DRV_TXT2PMAXPS_MAX_PS_POINT, 
        hdrrmac4drv.per_carrier_info[ch].channel_config->
        txt2p_max_ptr->ps_axis,
        hdrrmac4drv.per_carrier_info[ch].channel_config->
        txt2p_max_ptr->txt2p_max_ps_axis,
        hdrrmac4drv.per_carrier_info[ch].channel_config->
        txt2p_max_ptr->num_ps_axis_values );

      /* Set per channel attributes available flag */
      attributes_available = TRUE;
    }
  }

  /* Send hARQCoeffC2i config message to firmware if it is non-empty */
  if (fw_cmd.rmac_harqcoeffc2i_cfg.carrierMask != 0)
  {
    estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return attributes_available;

} /* hdrrmac4drv_update_per_channel_attributes() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_ATTRIBUTES                              EXTERNAL

DESCRIPTION
  To be called at activation. Updates attribute (gaupable and non-gaupable)
  configuration information maintained by RMAC4DRV.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_attributes( void )
{
  /* Loop counters */
  uint8 transition_point, i, ch;

  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware configure attributes message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_ATTRIB_CFG_CMD );

  /* -- RRIChannelPowerParameters -- */

  /* Set up RRI table for each transition point */
  for (transition_point = 0; 
       transition_point < HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS; 
       transition_point++) 
  {

#ifdef FEATURE_FACTORY_TESTMODE
    if( ftm_get_mode() == FTM_MODE )
    {
      /* In FTM Mode set the FTM dictated RRI to Pilot channel gains */
      if( transition_point == hdrhitmsg_get_rmac_params_ptr()->gain_transition_point )
      {
        hdrrmac4drv_config->rri_chan_power_params[transition_point].
          rri_chan_gain_pre_transition =
        hdrhitmsg_get_rmac_params_ptr()->rri_ch_gain_pre_transition;

        hdrrmac4drv_config->rri_chan_power_params[transition_point].
          rri_chan_gain_post_transition =
        hdrhitmsg_get_rmac_params_ptr()->rri_ch_gain_post_transition;
      }
    }
#endif

    for (i=0; i<transition_point+1; i++)
    { 
      /* Convert units from 1 dB to .25 dB for DSP */
      hdrrmac4drv.rri_to_pilot[transition_point].subpacket_gain[i] = 
        ( hdrrmac4drv_config->rri_chan_power_params[transition_point].
          rri_chan_gain_pre_transition ) << 2;
    }
    for (; i< HDRSCMRMACCMN_NUM_T2P_TRANS_POINTS; i++)
    { 
      /* Convert units from 1 dB to .25 dB for DSP */
      hdrrmac4drv.rri_to_pilot[transition_point].subpacket_gain[i] = 
        ( hdrrmac4drv_config->rri_chan_power_params[transition_point].
          rri_chan_gain_post_transition ) << 2;
    }
  }

#ifdef FEATURE_FACTORY_TESTMODE
  /* Set default RRI gain */
  if( ftm_get_mode() == FTM_MODE )
  {
    hdrrmac4drv.rri_default = hdrhitmsg_get_rmac_params_ptr()->rri_ch_gain_pre_transition << 2;
  }
  else
  {
#endif /* FEATURE_FACTORY_TESTMODE */

    /* Set default RRI gain - used when no packets are tx'd */
    hdrrmac4drv.rri_default = 
      ( hdrrmac4drv_config->rri_chan_power_params[3].
        rri_chan_gain_pre_transition ) << 2;

#ifdef FEATURE_FACTORY_TESTMODE
  }
#endif /* FEATURE_FACTORY_TESTMODE */

  /* -- FRABFilterTC -- */
  fw_cmd.rmac_attrib_cfg.oneMinusFrabCoeff = hdrrmac4drv_frab_tc[
    hdrrmac4drv_config->filt_time_const.frab];

  /* -- QRABFilterTC -- */
  fw_cmd.rmac_attrib_cfg.oneMinusQrabCoeff = hdrrmac4drv_qrab_tc[
    hdrrmac4drv_config->filt_time_const.qrab];

  /* -- PilotStrengthFilterTC -- */

  /* Set up pilot strength filter */
  /* Multiplier is tc - 1 */
  hdrrmac4drv.ps_filter_mult = 
    hdrrmac4drv_pilot_filter_lengths[
    hdrrmac4drv_config->filt_time_const.pilot_strength] - 1;

  /* The filter_shift configures the divider (using shift instead of div). */
  hdrrmac4drv.ps_filter_shift = 
    (uint16)(hdrrmac4drv_config->filt_time_const.pilot_strength) + 3;

  /* Clear PN list ist at activation */
  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    for(i=0; i<HDRSRCH_SUBASET_MAX_SIZE; i++)
    {
      hdrrmac4drv.per_carrier_info[ch].pn_offset[i] = HDR_NO_PN_OFFSET;
    }
  }


#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() == FTM_MODE )
  { 
    /* Set the FTM dictated Auxiliary Pilot gain in FTM Mode*/
    hdrrmac4drv_config->aux_pilot_info.gain_db = -( hdrhitmsg_get_rmac_params_ptr()->auxpil_chan_gain * 4 );

    /* Set the FTM dictated Minimum Payload at which Aux Pilot channel is ON */
    hdrrmac4drv_config->aux_pilot_info.min_payload = 
      hdrhitmsg_get_rmac_params_ptr()-> auxpil_payload_thresh + 1;
  }
#endif

  /* -- AuxiliaryPilotChannelParameters (GAUPable) -- */
  hdrrmac4drv.aux_pilot_gain = hdrrmac4drv_config->aux_pilot_info.gain_db;
  hdrrmac4drv.aux_pilot_min_payload = (hdrrmaccmn_payload_size_enum_type)
    hdrrmac4drv_config->aux_pilot_info.min_payload;

  /* Update per-channel attributes 
     The attributes will be updated only if 
     Physical_Channel->Logical_Channel_Attribues mapping is available. 
     The mapping is obtained in start_open_state().  Do not call the function
     none of the channels are in OPEN state. */
  if (hdrrmac4drv.num_chans != 0)
  {
    if (!hdrrmac4drv_update_per_channel_attributes())
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
        "Per-carrier attribute update failed!" );
    }
  }

  /* Load RRI and Aux Pilot values into MDSP */ 
  fw_cmd.rmac_attrib_cfg.auxPilotMinPayload = hdrrmac4drv.aux_pilot_min_payload;

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    fw_cmd.rmac_attrib_cfg.carrierMask |= (1 << ch);

    fw_cmd.rmac_attrib_cfg.attribCfg[ch].auxPilotToData = 
      hdrrmac4drv.aux_pilot_gain;

    /* Load default RRI gain into MDSP for channel ch*/
    fw_cmd.rmac_attrib_cfg.attribCfg[ch].rriToPilot = 
      hdrrmac4drv.rri_default;
  } 

  /* -- CommonPowerParameters -- */

  /* Set TxT2Pmin in MDSP.  This is used by the DSP to determine whether it
     should enable tx throttling. */
  fw_cmd.rmac_attrib_cfg.txTrafficToPilotMin = 
    hdrrmac4drv_config->common_pow_params.tx_t2p_min_db;

  /* Send RMAC attribute updates to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "Update attributes." );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_update_attributes() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_CONFIG_EARLY_TERMINATION                        EXTERNAL

DESCRIPTION
  Configures early termination of reverse link packets.
  This is a test mode function.  

DEPENDENCIES
  None.

PARAMETERS
  config_mode: 0 - Default behavior, RL early termination on.
               1 - Forced early termination after 1 subpacket.
               2 - Forced early termination after 2 subpackets.
               3 - Forced early termination after 3 subpackets.
               4 - Forced early termination after 4 subpackets.

RETURN VALUE
  TRUE (config mode accepted),
  FALSE (config mode not valid).

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_config_early_termination
(
  int config_mode
)
{

  boolean rc = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check that config_mode is valid */
  if ((config_mode >= 0) && (config_mode <= HDRRMACCMN_MAX_NUM_SUBPACKETS))
  {

    rc = TRUE;

    hdrrmac4drv.early_term_config = (uint16) config_mode;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return rc;

} /* hdrrmac4drv_config_early_termination() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_RESET_RTC_RATE_COUNT                           EXTERNAL

DESCRIPTION
  This function resets the count of reverse traffic frames of each rate.
  The RL rate loggic metrics structure [hdrrmac4drv_metrics] is cleared.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_reset_rtc_rate_count( void )
{

  uint8 ch;
    /* channel loop index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear stats to zero */
  memset((void*)&hdrrmac4drv_metrics, 0, sizeof(hdrrmac4drv_metrics_type));

  /* Re-populate the channel information */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    /* log all channels except the ones in INACTIVE state */
    if (hdrrmac4drv.per_carrier_info[ch].carrier_state !=
        HDRRMAC4DRV_CH_ST_INACTIVE)
    {
      /* Save channel information in RL Metrics log packet.  This 
         information is not refreshed till the channel is 
         dropped/replaced. */
      hdrrmac4drv_metrics.log[ch].carrier_id.channel_id = ch;
      hdrrmac4drv_metrics.log[ch].carrier_id.channel_no = 
        (uint16) hdrrmac4drv.per_carrier_info[ch].channel_record.chan_num;
      hdrrmac4drv_metrics.log[ch].carrier_id.band_class = (uint16) ((uint32) 
        (int32) hdrrmac4drv.per_carrier_info[ch].channel_record.band);
    }
    else
    {
      /* Mark all other channels as unassigned */
      hdrrmac4drv_metrics.log[ch].carrier_id.channel_id = 
        HDRRMAC4DRV_INVALID_CHAN_ID;
    }
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_reset_rtc_rate_count() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_LOG_RTC_RATE_COUNT                             EXTERNAL

DESCRIPTION
  This function triggers submission of RL Metrics log packet in the next
  subframe.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrmac4drv_log_rtc_rate_count( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrrmac4drv_metrics.submit_rl_metrics = TRUE;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_log_rtc_rate_count() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SET_FRAME_OFFSET                               EXTERNAL

DESCRIPTION
  This function sets the RMAC frame offset.

DEPENDENCIES
  None.

PARAMETERS
  frame_offset - the Frame offset

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_set_frame_offset
(
  uint8 frame_offset
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrrmac4drv.frame_offset = frame_offset;
    /* Copy the data */

  frame_offset = frame_offset % HDRRMAC4DRV_SLOTS_PER_SUBFRAME;

  hdrrmac4drv.interrupt_offset = 
    (((frame_offset + HDRRMAC4DRV_SLOTS_PER_SUBFRAME) * 
      HDRRMAC4DRV_CHIPS_PER_SLOT) - 
     HDRRMAC4DRV_INTERRUPT_OFFSET_CHIPS) % 
    (HDRRMAC4DRV_SLOTS_PER_SUBFRAME * HDRRMAC4DRV_CHIPS_PER_SLOT);

  HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, "RMAC frame offset: %d",
                  hdrrmac4drv.frame_offset );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_set_frame_offset() */

#ifdef FEATURE_HDR_REVB_DTX_MODE
/*===========================================================================
FUNCTION HDRRMAC4DRV_MONITOR_DTX_MODE                          EXTERNAL

DESCRIPTION
  This function is a registered callback to monitor changes to network
  DTXMode.
  
DEPENDENCIES
  None.

PARAMETERS
  enabled - Indicated whether DTXMode is enabled/disabled.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_monitor_dtx_mode ( boolean enabled )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( enabled  && 
       ( ( hdrrmac4drv_common->dtx_drc_length < 
           HDRRMAC_DTX_DRC_LENGTH_MIN_IN_SLOTS ) ) )
  {
    HDR_MSG_DRIVERS(
       MSG_LEGACY_HIGH, "nDTX: drc length not valid or TxPwr limited, not enabling" );
    return;    
  }

  hdrrmac4drv_common->network_dtx_mode = enabled;
  
  HDR_MSG_DRIVERS_1(
      MSG_LEGACY_HIGH, "nDTX: dtx_mode changed %d",
      hdrrmac4drv_common->network_dtx_mode );

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX

  /* If enabling nDTX, disable devcie only DTX*/
  if ( hdrrmac4drv_common->network_dtx_mode )
  {
    hdrrmac_stop_device_only_dtx_monitor();    
  }
  /* If disabling nDTX enable devcie only DTX*/
  else if ( !hdrrmac4drv_common->network_dtx_mode )
  {
    /* Start device only dtx monitor */
      hdrrmac_start_device_only_dtx_monitor();
  }
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */

  /* Program DTXMode */
  hdrrmac4drv_update_dtx_mode( );
} /* hdrrmac4drv_monitor_dtx_mode */
#endif /* FEATURE_HDR_REVB_DTX_MODE */

#if defined FEATURE_HDR_REVB_DTX_MODE || defined FEATURE_HDR_TRAFFIC_DTX
/*===========================================================================
FUNCTION HDRRMAC4DRV_MONITOR_PA_MODE                          EXTERNAL

DESCRIPTION
  This function is a registered callback to monitor changes to PAMode.
  
DEPENDENCIES
  None.

PARAMETERS
  enabled - Indicated whether PAMode is ON/OFF.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_monitor_pa_mode ( boolean pa_on )
{
  HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, "n/t DTX: PA mode change rsp from FW: "
                                      "%d", pa_on );

  /* PA is on program DTXMode */
  if ( pa_on )
  {
    HDR_MSG_DRIVERS_2( MSG_LEGACY_HIGH, "n/t DTX: Programming DTXMode pa_on "
                                        "%d and rmac state: %d",
                       pa_on, hdrrmac4drv_common->state );

    hdrrmac4drv_update_dtx_mode( );

  }
  else
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "n/t DTX: PA OFF" );
  }

} /* hdrrmac4drv_monitor_pa_mode */
#endif /* FEATURE_HDR_REVB_DTX_MODE || FEATURE_HDR_TRAFFIC_DTX */

#if defined(FEATURE_HDR_DEVICE_ONLY_DTX)|| defined FEATURE_HDR_REVB_DTX_MODE\
 || defined FEATURE_HDR_TRAFFIC_DTX
/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_DTX_MODE                          EXTERNAL

DESCRIPTION
  This function programs DTXMode to FW.
 
  DTX Config:
  Bit14 enables PA control
 
  sDTX only:  0xC000
  nDTX only:  0xC001
  tDTX only:  0xC002
  tdtx+ ndtx: 0xC003
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_dtx_mode ( void )
{
  hdrfw_msg_u fw_cmd;      /* Firmware command */

  errno_enum_type estatus; /* Error status */

  uint16 dtx_config = 0x0;  /* Variable to evaluate dtx config */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if ( hdrrmac4drv.pa_status != PA_OFF )
  {
    /* If PA is being turned off, program dtx off.*/
    if ( hdrrmac4drv.pa_status == PA_OFF_PENDING )
    {
      dtx_config = 0x0000;

    }
    else
    {
#ifdef FEATURE_HDR_TRAFFIC_DTX
      /* Is tDTX enabled? */
      if( hdrrmac4drv_common->tdtx_mode )
      {
        dtx_config |= 0xC002;
      }
#endif /* FEATURE_HDR_TRAFFIC_DTX */

#ifdef FEATURE_HDR_REVB_DTX_MODE
      /* Is nDTX enabled? */
      if( hdrrmac4drv_common->network_dtx_mode )
      {
        dtx_config |= 0xC001;
      }
#endif /* FEATURE_HDR_REVB_DTX_MODE */

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
      /* Is sDTX enabled?
         sDTX overwrites tDTX
         s and nDTX cannot coexist */
      if( hdrrmac4drv_common->device_only_dtx_mode )
      {
        dtx_config = 0xC000;
      }
#endif /* FEATURE_HDR_DEVICE_ONLY_DTX */
    }

    /* Any change in DTX config ? */
    if ( hdrrmac4drv_common->dtx_config != dtx_config )
    {
      hdrrmac4drv_common->dtx_config = dtx_config;

      /* Initialize DTXMode configuration message */
      memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
      
      msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
                      HDR_FW_RMAC_DTXMODE_CFG_CMD );

      fw_cmd.rmac_dtxmode_cfg.dtxConfig = hdrrmac4drv_common->dtx_config;
        
      /* If sDTX use different frame offset */    
      if ( hdrrmac4drv_common->dtx_config == 0xC000 )
      {
        fw_cmd.rmac_dtxmode_cfg.dtxFrameOffset = 
          hdrrmac4drv_common->dtx_frame_offset;

        fw_cmd.rmac_dtxmode_cfg.drcGainBoostCfg = 
          (uint8)hdrrmac4drv_common->dtx_drc_boost;
      }
      else
      {
        fw_cmd.rmac_dtxmode_cfg.dtxFrameOffset = hdrrmac4drv.frame_offset;
      }
      
#ifdef FEATURE_HDR_TRAFFIC_DTX
        /* dtxPowerThreshold in fw units:
           txFWTtotal = ( K + 63.8 - TxTotal) * 640
        */
      fw_cmd.rmac_dtxmode_cfg.dtxTxPwrThreshold = 
        ( ( hdrrmac4drv_common->turnaround_db256 + 16333 
        - ( HDRRMAC_TDTX_TX_PWR_THRESH_DB * 256 ) ) / 256 ) * 640; 

      HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH,
        "tDTX: TxPwrThresh %d ", fw_cmd.rmac_dtxmode_cfg.dtxTxPwrThreshold );
#endif /* FEATURE_HDR_TRAFFIC_DTX */
      
  #ifdef FEATURE_HDR_FLEX_CONN
      /* Keep flexconn_mode same as before. We won't want to touch it here */
      fw_cmd.rmac_dtxmode_cfg.flexConnEnable = 
        hdrrmac4drv_common->flexconn_mode;
  #endif /* FEATURE_HDR_FLEX_CONN */
        
      HDR_MSG_DRIVERS_3( MSG_LEGACY_HIGH,
        "DTX: send DTXCFG_CMD dtxConfig 0x%x frame offset %d HSTR %d",
        fw_cmd.rmac_dtxmode_cfg.dtxConfig,
        fw_cmd.rmac_dtxmode_cfg.dtxFrameOffset,
        HDRHSTR_GET_COUNT());

      /* Send DTXMode config command to firmware */
      estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
      ASSERT ( estatus == E_SUCCESS );
    }
    else
    {
       
#ifdef FEATURE_HDR_FLEX_CONN

       HDR_MSG_DRIVERS_1( MSG_LEGACY_MED,
        "sDTX: No change in  dtx config  sDTX %d",
        hdrrmac4drv_common->device_only_dtx_mode );
#endif /* FEATURE_HDR_FLEX_CONN */

#ifdef FEATURE_HDR_TRAFFIC_DTX

       HDR_MSG_DRIVERS_1( MSG_LEGACY_MED,
        "tDTX: No change in  dtx config tDTX %d",
        hdrrmac4drv_common->tdtx_mode );
#endif /* FEATURE_HDR_TRAFFIC_DTX */
    }
  } /*  hdrrmac4drv.pa_status != PA_OFF  */
  else
  {
     HDR_MSG_DRIVERS_1( MSG_LEGACY_MED,
      "DTX: not programming DTX mode because PA is OFF %d",
      hdrrmac4drv.pa_status );
  }

} /* hdrrmac4drv_update_dtx_mode */
#endif /*  FEATURE_HDR_DEVICE_ONLY_DTX || FEATURE_HDR_REVB_DTX_MODE 
|| FEATURE_HDR_TRAFFIC_DTX */

/*===========================================================================
FUNCTION HDRRMAC4DRV_INIT                                           EXTERNAL

DESCRIPTION
  Performs any start up initialization needed.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_init
(
  hdrrmac_common_type * common_parameters
)
{
  uint8 ch;                /* Loop counter */

  hdrfw_msg_u fw_cmd;      /* Firmware command */

  errno_enum_type estatus; /* Error status */

  int32 pmax_q10dbm; /* Total available max power in Q10 dBm units */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* This function is called at end of access.  Set flag to indicate
     that no RevB TCA has been processed by RMAC since access. */
  hdrrmac4drv.processed_first_tca_after_access = FALSE;

  /* Initialize firmware configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD );

  /* Initialize common pointer */
  hdrrmac4drv_common = common_parameters;

  /* Reset 
     - channel enabled statuses
     - channel DRC supervision statuses
     - channel tx pilot differential drop flag
     - channel reportable status
     - channel drop ranks
     - ARQ status log info
     - RAB update log info      
     - Packet status log info
     - RL gains log info */
  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv.per_carrier_info[ch].carrier_state = 
      HDRRMAC4DRV_CH_ST_INACTIVE;

    hdrrmac4drv.per_carrier_info[ch].fwd_link_index = 
      HDRRMAC4DRV_INVALID_CHAN_ID;

    hdrrmac4drv.per_carrier_info[ch].demod_index = 
      HDRRMAC4DRV_INVALID_CHAN_ID;

    hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired = 
      FALSE;

    hdrrmac4drv.per_carrier_info[ch].drop_carrier_txp_diff = 
      FALSE;

    hdrrmac4drv.per_carrier_info[ch].reportable = FALSE;

    hdrrmac4drv.per_carrier_info[ch].drop_rank = 0;

    hdrrmac4drv.per_carrier_info[ch].okay_to_deactivate = FALSE;

    hdrrmac4drv_arq_status_log_info.enable_channel_logging[ch] = 
      FALSE;
    hdrrmac4drv_rab_update_log_info.enable_channel_logging[ch] = 
      FALSE;
    hdrrmac4drv_gain_log_info.enable_channel_logging[ch]       = 
      FALSE;

    hdrrmac4drv_pkt_status_log_info.packet_count[ch] = 0;

    hdrrmac4drv.open_chans[ch] = ch;
    hdrrmac4drv.open_chans_data[ch] = ch;

    /* Initialize closed loop of carriers to the value set by the access 
       carrier (carrier 0) */
    fw_cmd.rmac_misc_cfg.fieldMask.gainAdjOverride_Valid = 1;
    fw_cmd.rmac_misc_cfg.carrierMask |= (1 << ch);
    fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.carrier = 0x0;
    fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjVal = 0x1;

    /* Use 23 dBm as a temporary initial value.  The calculated pmax value will
       be overwritten when headroom calculation runs */
    pmax_q10dbm = 23552;

    /* Initialize backoff subtracted avilabale transmit power */
    hdrrmac4drv.pmax[ch] = hdrrmacmath_dbm_2_mw( pmax_q10dbm );

  }

  /* Send RMAC closed loop override command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  hdrrmac4drv.num_chans = 0;
  hdrrmac4drv.num_chans_data = 0;
  hdrrmac4drv.num_chans_prev = 0;
  hdrrmac4drv.num_req_carriers = 0;
  hdrrmac4drv.num_reportable_carriers = 0;

  hdrrmac4drv_arq_status_log_info.sf_count = 0;
  hdrrmac4drv_rab_update_log_info.sf_count = 0;
  hdrrmac4drv_gain_log_info.sf_count       = 0;
  hdrrmac4drv_hdrm_cm_log_info.sf_count    = 0;

  hdrrmac4drv_gain_log_info.prev_pa_enabled = FALSE;
  hdrrmac4drv_gain_log_info.backoff = 0;
  hdrrmac4drv_gain_log_info.max_avail_tx_pwr = 0;

  /* Initialize carrier management thresholds */
  hdrrmac4drv_init_carrier_management();

  /* Initialize flow control flag */
  hdrrmac4drv.flow_control_on = FALSE;

  /* Initialize enhanced PA backoff flag */
  hdrrmac4drv.enh_pa_backoff_enabled = FALSE;

#ifdef FEATURE_HDR_REVB_DTX_MODE
  /* Register DTXMode Monitor */
  hdrphy_register_dtx_mode_monitor( hdrrmac4drv_monitor_dtx_mode );
#endif /* FEATURE_HDR_REVB_DTX_MODE */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_init() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_START_OPEN_STATE                               EXTERNAL

DESCRIPTION
  This function performs any low level init/config for open state 
  transition for the specified physical channel.  This function currently
  enables RAB.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index
  chan_cfg_ptr - Pointer to the configuration structure of the logical 
                 channel associated with the above physical channel.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_start_open_state
(
  uint8                                phy_chan_idx,
  hdrrmac4_channel_config_struct_type  *chan_cfg_ptr
)
{
  /* Loop counters */
  uint8 ch, idx;

  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware config message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD );

  /* If the channel is in SETUP state, enable channel */
  if (hdrrmac4drv.per_carrier_info[phy_chan_idx].carrier_state == 
    HDRRMAC4DRV_CH_ST_SETUP)
  {
    /* Submit PA headroom and carrier management log before enabling
       a new channel */
    hdrrmac4drv_submit_pa_hdrm_cm_info_log();

    /* If we are not in RLSI, enable RAB. */
    if ((!hdrrmac4drv.silence_now_flag) || 
        (hdrrmac4drv.silence_now_flag && (hdrrmac4drv.silence_count < 0)))
    {
      /* Unfreeze RAB */
      fw_cmd.rmac_misc_cfg.fieldMask.rabDecisionCtl_Valid = 1;
      fw_cmd.rmac_misc_cfg.carrierMask |= ( 1 << phy_chan_idx );
      fw_cmd.rmac_misc_cfg.C[phy_chan_idx].rabDecisionCtl.M = 0;
      fw_cmd.rmac_misc_cfg.C[phy_chan_idx].rabDecisionCtl.R = 0;
      fw_cmd.rmac_misc_cfg.C[phy_chan_idx].rabDecisionCtl.D = 0;
    }

    /* Store channel configuration */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].channel_config = chan_cfg_ptr;

    /* Update per-channel attributes */
    if (!hdrrmac4drv_update_per_channel_attributes())
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
        "Per-carrier attribute update failed!" );
    }

    /* Enable channel */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].carrier_state = 
      HDRRMAC4DRV_CH_ST_DATA_ACTIVE;

    if (!hdrrmac4drv_slp_selected)
    {
      /* Pick the first Open RL as SLP carrier. */
      hdrrmac4drv_slp_selected = TRUE;
      hdrrmac4drv.slp_carrier.current = phy_chan_idx;
    }

    if (hdrrmac4drv_bypass_pa_headroom_alg)
    {
      /* Since the headroom algorithm is bypassed, order RTCMAC prioritized
         channels as per the order of their RTCAcks received. */
      hdrrmac4drv.prioritized_carriers.rtcmac[hdrrmac4drv.num_chans] =
        phy_chan_idx;
    }

    /* Increment number of active channels */
    hdrrmac4drv.num_chans++;

    /* Update OPEN channel list */
    hdrrmac4drv_update_open_chan_list();

    /* Set appropriate carrier states.
       Note that the carrier states are reset to allow data on 
       all assigned carriers every time a carrier is added. */
    for (idx=0; idx < hdrrmac4drv.num_chans; idx++)
    {
      ch = hdrrmac4drv.open_chans[idx];

      /* Change the states of all OPEN channels to DATA_ACTIVE
         and reset their okay-to-deactivate flags */
      hdrrmac4drv.per_carrier_info[ch].carrier_state = 
        HDRRMAC4DRV_CH_ST_DATA_ACTIVE;

      hdrrmac4drv.per_carrier_info[ch].okay_to_deactivate = FALSE;

    }

    /* Update list of channels carrying data */
    hdrrmac4drv_update_open_chan_data_list();

    /* Update number of reportable carriers */
    hdrrmac4drv_update_num_reportable_carriers();

    /* Set appropriate carrier configuration. */
    if (hdrrmac4drv.num_chans == 1)
    {
      /* Carrier configuration with one active channel */
      hdrrmac4drv.carrier_config_id = CARRIER_CONFIG_P0_D0;

    }
    else if (hdrrmac4drv.num_chans == 2)
    {
      /* Carrier configuration with two active channels */
      hdrrmac4drv.carrier_config_id = CARRIER_CONFIG_P01_D01;

    }
    else if (hdrrmac4drv.num_chans == 3)
    {
      /* Carrier configuration with three active channels */
      hdrrmac4drv.carrier_config_id = CARRIER_CONFIG_P012_D012;

    }
    else
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
        "Start Open State: Invalid number of open channels!" );
    }

    /* Funnel state can change due to change in number of assigned 
       or data carrying carriers => update funnel state. */
    hdrrmac4drv_update_funnel_state();

    /* Reset target carrier config to carrier config value */
    hdrrmac4drv.target_carrier_config_id = hdrrmac4drv.carrier_config_id;

  }
  else
  {
    HDR_MSG_DRIVERS_1( MSG_LEGACY_ERROR, 
      "Start Open State: Channel %d not in SETUP state!",
      phy_chan_idx );
  }

  /* Send RAB config message to firmware if it is non-empty */
  if (fw_cmd.rmac_misc_cfg.carrierMask != 0)
  {
    estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );
  }

#ifdef FEATURE_HDR_TRAFFIC_DTX
  /* Check if tDTX is enabled */
  if( hdrrmac4drv_common->tdtx_mode )
  {
    /* Register to be notified when pa mode changes. */
    hdrmod_register_pa_mode_monitor ( hdrrmac4drv_monitor_pa_mode );
  }
#endif /* FEATURE_HDR_TRAFFIC_DTX */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_start_open_state() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_TX_IS_AVAILABLE
  RMG: Previously this was hdrrmac4drv_drc_supervision_timer_status()

DESCRIPTION
  This function checks if packets can be sent on the specified carrier.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_id - Physical channel ID

RETURN VALUE
  TRUE  - if packets can be sent on the specified reverse channel.
  FALSE - otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_tx_is_available( uint8 phy_chan_id )
{
  boolean foward_link_locked;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  foward_link_locked = (hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
    rmacInfo[phy_chan_id].fingLockedFlag != 0)? TRUE: FALSE;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  /* When not in tuneaway, PA is declared available if 
     - AT is not in DRC supervision AND
     - Fingers are in lock 
     When in tuneaway, PA is declared available if
     - AT is not in DRC supervision */

  if ( ( !(hdrrmac4drv.per_carrier_info[phy_chan_id].drc_supervision_expired) )
        &&
       ( ( hdrrmac4drv.subframes_to_suspend == 0 ) ||
         foward_link_locked ) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* hdrrmac4drv_tx_is_available() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SET_FRAME_OFFSET                               INTERNAL

DESCRIPTION
  Configures the encoder before use.
  
DEPENDENCIES
  None.
  
PARAMETERS
  frame_offset - the frame offset of the reverse link.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_setup_frame_offset
(
  uint16 frame_offset
)
{
  
/* --------------------------------------------------------------------- */

  /* This value holds time shifting value need to compensate for 
     - Encoder loading window: 1 SF + 1 Slot (see diagram)
     - Frame offset in upto 3 slots ( because modulator has upto 3 slot 
       frame_offset resolution in RevA/B )
     It is used to calculate next subframe time:
     NextSubframeTime = Modulator Time + Encoder Loading Window - Frame Offset
                      = Modulator Time + Subframe_Halfslot_Offset */
  hdrrmac4drv.subframe_halfslot_offset =  
    HDRRMAC4DRV_HALFSLOTS_PER_SUBFRAME + 
    HDRRMAC4DRV_HALFSLOTS_PER_SLOT - 
    ((frame_offset % HDRRMAC4DRV_SLOTS_PER_SUBFRAME) * 
     HDRRMAC4DRV_HALFSLOTS_PER_SLOT);

  /* A subframe offset is caused by a frame offset > 3. */
  hdrrmac4drv.subframe_offset = frame_offset / 
    HDRRMAC4DRV_SLOTS_PER_SUBFRAME;

  /* Gives the subframe we are interested in */
  hdrrmac4drv.subframe_id = HDRRMAC4DRV_INVALID_PACKET;

  /* Reset previous subframe number */
  hdrrmac4drv.prev_subframe.rlsi    = HDRRMAC4DRV_INVALID_PACKET;
  hdrrmac4drv.prev_subframe.suspend = HDRRMAC4DRV_INVALID_PACKET;

  /* Set the modulator frame offset. The modulator simply wants to know
     which slot within a subframe to begin. */
  hdrmod_set_tx_frame_start_offset( frame_offset % 4);
                    
  HDR_MSG_DRIVERS_2(MSG_LEGACY_HIGH,
                    "Frame offset: %d - Subframe offset: %d ",
                    frame_offset, 
                    hdrrmac4drv.subframe_offset);

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_setup_frame_offset() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_THROTTLING                              INTERNAL

DESCRIPTION
  This function checks whether Reverse Link throttling should be disabled
  and enables/disables based on this. (Note that normal operation has RL
  throttling on as per standard.)

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_throttling( void )
{
  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware throttle configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_THROTTLE_CFG_CMD );

  if (hdrrmac4drv_common->throttle_disable_requests)
  {
    /* Disable RL throttling. */
    fw_cmd.rmac_throttle_cfg.txThrottleDisable = 1;

    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "HDR RL throttle disabled." );

    hdrrmac4drv.throttle_disable = TRUE;
  }
  else
  {
    /* Re-enable RL throttling */
    fw_cmd.rmac_throttle_cfg.txThrottleDisable = 0;

    if (hdrrmac4drv.throttle_disable != FALSE)
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "HDR RL throttle enabled." );
      hdrrmac4drv.throttle_disable = FALSE;
    }
  }

  /* Send throttle config command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_update_throttling() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_RESET_DRC_SUPERVISION                          INTERNAL

DESCRIPTION
  Resets DRC supervision expiration status for all channels.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_reset_drc_supervision( void )
{
  uint8 ch;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired = FALSE;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_reset_drc_supervision() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_CONFIG_MODULATOR                               EXTERNAL

DESCRIPTION
  Configures the HDR modulator for the specified modulator channels.
  
DEPENDENCIES
  None.

PARAMETERS
  ati_value     - Current ATI value. 
  phy_chan_mask - Physical channel mask

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_config_modulator
(
  uint32 ati_value
#ifdef FEATURE_HDR_BOLT_MODEM
  , uint8  phy_chan_mask
#endif /* FEATURE_HDR_BOLT_MODEM */
)
{

  /* I-channel PN mask. */
  qword i_mask;

  /* Q-channel PN mask. */
  qword q_mask;

  /* Permuted ATI value. */
  uint32 permuted_ati = 0;

  /* Loop counter. */
  int i;

  /* Bit 0 of Q mask */
  uint32 xor;

  /* Working registers for PN mask computation */
  uint32 i_mask_32, i_shift;

  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Place modulator Rev A/B mode */
  hdrmod_set_traffic_mode(HDRMOD_TRAFFIC_MODE_REVB);

  /* Initialize firmware modulator configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_MODULATOR_CFG_CMD );

#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() != FTM_MODE )
#endif
  {
    /* Permute the ATI value to determine the User Long Code Mask. */
    for ( i=0; i<HDRRMAC4DRV_PERM_LEN; i++ )
    {
      if ( ati_value & 1<<hdrrmac4drv_ati_permutation_array[i] )
      {
        permuted_ati |= 1<<(31-i);
      }
    }
    qw_set( i_mask, I_MASK_UPPER_BITS, permuted_ati );

    /* -- Construct Q mask -- */

    /* First, figure out what bit 0 should be. */
    xor = 0;
    for (i=0;i<HDRRMAC4DRV_XOR_LEN;i++)
    {

      i_shift = hdrrmac4drv_q_xor_array[i];
      if (i_shift > 31 )
      {
        i_mask_32 = qw_hi(i_mask);
        i_shift-=32;
      }
      else
      {
        i_mask_32 = qw_lo(i_mask);
      }

      xor ^= ((i_mask_32 >> i_shift) & 0x00000001UL);
    }

    /* Set Q to I, shifted by one bit to the left. */
    qw_set(q_mask, qw_hi(i_mask), qw_lo(i_mask));

    qw_shift(q_mask, 1);

    /* Add in last bit. */
    qw_set(q_mask, qw_hi(q_mask) & 0x3ff, (qw_lo(q_mask) | xor));

    /* Program the User Long Code Mask into the modulator. */
    hdrmod_set_pn_long_code_mask( i_mask, q_mask 
#ifdef FEATURE_HDR_BOLT_MODEM
      ,phy_chan_mask  /* Enable PN generator for specified carriers */
#endif /* FEATURE_HDR_BOLT_MODEM */
      );

    HDR_MSG_DRIVERS_4( MSG_LEGACY_MED, 
      "I_MASK_hi = %x, I_MASK_lo = %x, Q_MASK_hi = %x, Q_MASK_lo = %x",
      qw_hi(i_mask),
      qw_lo(i_mask),
      qw_hi(q_mask),
      qw_lo(q_mask) );

    /* Set RPC step size: Macro takes units of 1/2 dB.
       In RPC_step field, 0 = .5 dB steps
                          1 = 1.0 dB steps.
       (Thus 1 is added to convert)  */
    fw_cmd.rmac_modulator_cfg.txGainAdjStep = 
      (hdrrmac4drv_config->common_pow_params.rpc_step_db * 128);
  }
#ifdef FEATURE_FACTORY_TESTMODE
  else
  {
    hdrhitmsg_set_pn_long_code_mask();
    fw_cmd.rmac_modulator_cfg.txGainAdjStep = ( hdrhitmsg_get_rmac_params_ptr()->rpc_step + 1 )* 256;
  }
#endif
  
  /* Place MDSP in Rev A/B mode */  
  /* In Bolt, FW enables carriers in wav before frame boundary when DAC start
     is expected to take an effect. Enabling of RL carriers requires mode
     information. Hence, modulator mode config must be sent to FW before
     DAC start. */
  fw_cmd.rmac_modulator_cfg.hdrModulatorModeConfig = HDRMOD_MOD_CFG_REVB;

  /* Program transmit pilot filter time constant */
  fw_cmd.rmac_modulator_cfg.txPilotPowerFiltCoeff = HDRRMAC4DRV_TC_PILOT;

  /* Set early termination config mode */
  if (hdrrmac4drv.early_term_config == 0)
  {
    /* Disable forced termination test mode */
    fw_cmd.rmac_modulator_cfg.rlForceEarlyTerminate.force = 0;
  }
  else
  {
    /* Enable forced termination test mode */
    fw_cmd.rmac_modulator_cfg.rlForceEarlyTerminate.force = 1;
    fw_cmd.rmac_modulator_cfg.rlForceEarlyTerminate.n = 
      hdrrmac4drv.early_term_config;
  } 

  /* Send modulator config command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  /* Prepare modulator to start tx */
  hdrmod_tx_start();

#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() == FTM_MODE )
  { 
    /* Set the FTM dicated rpc_enable in FTM Mode*/
    if( hdrhitmsg_get_rmac_params_ptr()->rpc_enable )
    {
      hdrrmac4drv.rpc_disabled = FALSE;

      HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "RPC is enabled for FTM for RevB" );
    }
    else
    {
      hdrrmac4drv.rpc_disabled = TRUE;

      HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "RPC is disabled for FTM for RevB" );
    }
  }
  else
#endif
  {
    /* Check RPC disable... */
    if (hdrlog_is_dipsw_set(HDRLOG_DIPSW_RPC_OFF))
    {
      /* Store RPC state -- RPC will be enabled when PA is turned on. */
      hdrrmac4drv.rpc_disabled = TRUE;

      HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "Inner loop (rpc) is disabled." );
    }
    else
    {
      /* Store RPC state */
      hdrrmac4drv.rpc_disabled = FALSE;
    }
  }

  /* Initialize encoder for HDR */
  hdrenc_init();

  /* Set frame offset */
  hdrrmac4drv_setup_frame_offset( hdrrmac4drv.frame_offset );

  /* For now, interrupt offset occurs exactly one subframe early. */
  hdrmod_set_tx_subframe_interrupt_offset( 
    hdrrmac4drv.interrupt_offset );

  /* Update RL throttling state */
  hdrrmac4drv_update_throttling();

  /* Update funnel bias */
  hdrrmac4drv_update_funnel_bias();

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_config_modulator() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_CALC_PILOT_PLUS_OVERHEAD_GAIN                  INTERNAL

DESCRIPTION
  Calculates and updates pilot_plus_overhead_gains for the channel indexed
  by phy_chan_id
  
DEPENDENCIES
  None.

PARAMETERS
  phy_chan_id    - Physical channel index

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_calc_pilot_plus_overhead_gain
(
  /* Physical channel id */
  uint8 phy_chan_id
)
{
  /* Loop counters */
  hdrrmaccmn_tx_mode_enum_type              tm;
  hdrrmaccmn_payload_size_enum_type         ps;

  /* shorthand pointer */
  hdrrmaccmn_rri_power_params_struct_type  * rri_pwr_params;

  /* min_trans_point set to maximum transition value */
  uint8                                      trans_point, min_trans_point = 3;

  /* Max RRI gain */
  int32                                      max_rri_gain;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Look up max RRI channel gain */
  for( tm = HDRRMACCMN_TX_MODE_HI_CAP; tm < HDRRMACCMN_NUM_TX_MODES; tm++ )
  { 
    for( ps = HDRRMACCMN_128_BITS; ps <= HDRRMACCMN_12288_BITS; ps++ )
    {
      trans_point = 
        hdrrmac4drv_config->t2p_info[tm].tx_t2p_info[ps].trans_point;

      if (min_trans_point > trans_point)
      {
        /* we have a new min_trans_point value */
        min_trans_point = trans_point;
      }
    }
  }


  rri_pwr_params = 
    &hdrrmac4drv_config->rri_chan_power_params[min_trans_point];

  /* Store RRI channel gain in 1/4 dB */
  max_rri_gain = (int32) 
    (MAX( rri_pwr_params->rri_chan_gain_pre_transition,
          rri_pwr_params->rri_chan_gain_post_transition ))<<2;

  /* Calculate and store 
   * pilot plus overhead = 1 + lin(RRI) + lin(DRC) + lin(max(ACK,DSC))
   * in linear Q.10 
   */
  hdrrmac4drv.per_carrier_info[phy_chan_id].pilot_plus_overhead_gains = 
    ( 1<<10 ) + 
    hdrrmacmath_db_2_lin( ( max_rri_gain)<<18 ) +
    hdrrmacmath_db_2_lin( 
      ( hdrrmac4drv.per_carrier_info[phy_chan_id].channel_gains.drc)<<18 ) +
    hdrrmacmath_db_2_lin( ( MAX( 
      hdrrmac4drv.per_carrier_info[phy_chan_id].channel_gains.ack,
      hdrrmac4drv.per_carrier_info[phy_chan_id].channel_gains.dsc ))<<18 );

  HDR_MSG_DRIVERS_2( MSG_LEGACY_MED,
    "Ch: %d, Pilot plus overhead[linear Q.10] = %d",
    phy_chan_id,
    hdrrmac4drv.per_carrier_info[phy_chan_id].pilot_plus_overhead_gains );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_calc_pilot_plus_overhead_gain() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_RL_INFO                                 EXTERNAL

DESCRIPTION
  Updates reverse link information.  Currently it updates DRC and ACK gains
  for the specified channel
  
DEPENDENCIES
  None.

PARAMETERS
  phy_chan_id    - Physical channel index
  channel_record - Reverse link channel record [will be used to program
                   rotator in future]
  subaset_info   - Sub-Active set information structure
  dropping_rank  - Carrier dropping rank

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_rl_info
(
  uint8                           phy_chan_id,
  sys_channel_type                *channel_record,
  hdrrmac_subaset_info_type       *subaset_info,
  uint8                           dropping_rank
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Store channel band class and channel number information */
  hdrrmac4drv.per_carrier_info[phy_chan_id].channel_record.band = 
    channel_record->band;
  hdrrmac4drv.per_carrier_info[phy_chan_id].channel_record.chan_num = 
    channel_record->chan_num;

  /* Store carrier dropping rank */
  hdrrmac4drv.per_carrier_info[phy_chan_id].drop_rank = dropping_rank;

  /* Set K value
     Per-carrier K is not supported.  K is used to convert total available
     power from DSP units to dBm.  This power is distributed between 
     the available carriers. */
  hdrrmac4drv.k = hdrrmac4drv_common->turnaround_db256 / 256;

  HDR_MSG_DRIVERS_1( MSG_LEGACY_MED, 
    "Turnaround constant: %d",
    hdrrmac4drv.k );


  /* Store the subactive set index for RL->FL mapping */
  hdrrmac4drv.per_carrier_info[phy_chan_id].fwd_link_index
    = subaset_info->subaset_index;

  /* Obtain demod index */
  hdrrmac4drv.per_carrier_info[phy_chan_id].demod_index
    = hdrsrchmac_get_demod_idx( subaset_info->subaset_index );

  /* Store channel reportable status */
  hdrrmac4drv.per_carrier_info[phy_chan_id].reportable 
    = subaset_info->reportable;

  /* Update number of reportable carriers */
  hdrrmac4drv_update_num_reportable_carriers();

  /* Store channel gain information */
  hdrrmac4drv.per_carrier_info[phy_chan_id].channel_gains.dsc_enabled = 
    subaset_info->dsc_enabled;

  if (!subaset_info->dsc_enabled)
  {
    hdrrmac4drv.per_carrier_info[phy_chan_id].channel_gains.dsc = 0;
  }

  /* Sign extend 6 bits to 32 and convert units from half dB to quarter dB */
  hdrrmac4drv.per_carrier_info[phy_chan_id].channel_gains.drc = 
    2 * ( HDRRMAC4DRV_SIGN_EXTEND_6_TO_32( subaset_info->gains.drc ) );

  hdrrmac4drv.per_carrier_info[phy_chan_id].channel_gains.ack = 
    2 * ( HDRRMAC4DRV_SIGN_EXTEND_6_TO_32( subaset_info->gains.ack ) );

  /* Calculate pilot plus overhead gain for the channel */
  hdrrmac4drv_calc_pilot_plus_overhead_gain( phy_chan_id );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_update_rl_info() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SET_DSC_GAIN_BASE                              EXTERNAL

DESCRIPTION
  Updates DSC gains for all the carriers.
  
DEPENDENCIES
  None.

PARAMETERS
  dsc_gain_base  - DSC channel gain in unit of -0.5 dB

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_set_dsc_gain_base
(
  uint8 dsc_gain_base
)
{
  uint8 ch;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set DSC channel gains for all carriers */
  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    /* Convert units from negative half dB to quarter dB */
    hdrrmac4drv.per_carrier_info[ch].channel_gains.dsc = 
      (-2) * dsc_gain_base;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_set_dsc_gain_base() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_COMPUTE_SILENCE_TIME                           INTERNAL

DESCRIPTION
  Computes the next Reverse Silence Interval time

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  hdrrmac4.silence_begin_time is updated with the frame time for the
  next silence interval.
===========================================================================*/
void hdrrmac4drv_compute_silence_time( void )
{
  
  qword  silence_cycles;
  qword  silence_time;
  qword  current_frametime;
  uint32 frame;
  uint32 slot;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current time in units of frames. Slot is the remainder, 
     so it indicates the slot we are on within the frame. */
  hdrts_get_cycle_and_slot(current_frametime, 
                           &slot, 
                           HDRRMAC4DRV_SLOTS_PER_FRAME);
  
  /* Set silence_cycles to current silence interval cycle number
     and set frames to the number of frames into the cycle we are 
     currently at. */
  frame = qw_div( silence_cycles, current_frametime, 
                  hdrrmac4drv_common->silence_period);

  /* Convert cycle into frames from UTC=0. In other words, this gives us the
     frame number if the first frame in the current RLSI period. (This is a 
     frame in the past.) */
  qw_mul( silence_time, silence_cycles, 
          hdrrmac4drv_common->silence_period);

  /* To find the start time of the next RLSI, add an interval (to complete
     the current period) and subtract off the necessary setup frames. 
     ((RLSI_time - setup time) is actually  stored as the begin time for the 
     next interval) */
  qw_inc( silence_time,
          hdrrmac4drv_common->silence_period - 
          HDRRMAC4DRV_SILENCE_SETUP_FRAMES );


  /* Now, we need to find out if we are currently in (1) the silence interval
     itself, or (2) in the time needed for setup.  If this is not an async
     function call, we can assume that we are not in/exiting RLSI, and do 
     not need to compute this. */
  if (hdrrmac4drv.silence_now_flag == FALSE)
  {

    /* Advance frame if frame event (ISR) would have already happened */
    if (slot >= (uint32)(HDRRMAC4DRV_SUBFRAME_3_SLOT + (hdrrmac4drv.frame_offset % 4)))
    {
      frame++;

      /* Corner case: If we missed the last frame interrupt in the RLSI, we 
         need to take care of scheduling the PA enable here. */
      if (frame == hdrrmac4drv_common->silence_duration)
      {

        HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, 
                           "Silence Interval in progress: Restarting (f=%d)",
                           hdrrmac4drv.frame_offset );

        /* RLSI is about to end, but we missed frame interrupt. 
           Need to schedule the PA to turn on at next frame boundary. */
        hdrrmac4drv_sync_start_pa();
      }
    }

    /* Compensate for setup frames */
    frame = (frame + HDRRMAC4DRV_SILENCE_SETUP_FRAMES) % 
      hdrrmac4drv_common->silence_period;
      

    /* Check whether we are currently in RLSI */
    if (frame < (uint32)(hdrrmac4drv_common->silence_duration + 
                HDRRMAC4DRV_SILENCE_SETUP_FRAMES))
    {
      hdrrmac4drv.silence_now_flag = TRUE;
      hdrrmac4drv.silence_count = 
        (int)( frame - HDRRMAC4DRV_SILENCE_SETUP_FRAMES );

      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                      "Silence Interval: In progress (%d)",
                      hdrrmac4drv.silence_count);

      /* If we are already in the middle of the interval, back up silence_time
         to the start time of processing the silence interval. */
      if (hdrrmac4drv.silence_count >= 0 )
      {
        /* Account for setup frames for the interval progress. */
        qw_dec( silence_time, hdrrmac4drv_common->silence_period );

      }
    }
  }

  /* Record the time of the interval */
  qw_equ( hdrrmac4drv.silence_begin_time, silence_time);

  HDR_MSG_PROT_3( MSG_LEGACY_HIGH, "Silence Interval @ 0x%x %x +%d",
                  qw_hi(silence_time), qw_lo(silence_time),    
                  HDRRMAC4DRV_SILENCE_SETUP_FRAMES );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

} /* hdrrmac4drv_compute_silence_time() */


/*===========================================================================

FUNCTION HDRRMAC4DRC_IS _SILENCE_FRAME

DESCRIPTION
  This function checks whether QTA falls in the scheduled RLSI

DEPENDENCIES
  None.

PARAMETERS
  frame_start   - frame which needs to be checked for RLSI

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrmac4drv_is_silence_frame
(
  qword frame_start   /* frame which needs to be checked for RLSI */
)
{
  qword frame_end, rlsi_frame_start, rlsi_frame_end;

  qw_equ(rlsi_frame_start, hdrrmac3drv.silence_begin_time);

  qw_inc(rlsi_frame_start, HDRRMAC4DRV_SILENCE_SETUP_FRAMES);
    /* Beginging of silence */

  qw_equ(rlsi_frame_end, rlsi_frame_start);

  qw_inc(rlsi_frame_end, hdrrmac4drv_common->silence_duration);
    /* End of silence */

  qw_equ(frame_end, frame_start);

  qw_inc(frame_end, HDRRMAC_QTA_TOTAL_TA_TIME_FRAMES);
    /* End time of QTA */

  
  /* Check whether QTA intersects RLSI */
  if ((HDRRMAC_QW_FRAME_CMP(frame_start, rlsi_frame_start) /* If QTA start is within RLSI */
      && HDRRMAC_QW_FRAME_CMP(rlsi_frame_end, frame_start)) 
    || (HDRRMAC_QW_FRAME_CMP(frame_end, rlsi_frame_start) /* If QTA End is within RLSI */
      && HDRRMAC_QW_FRAME_CMP(rlsi_frame_end, frame_end)) 
    || (HDRRMAC_QW_FRAME_CMP(rlsi_frame_start, frame_start) /* If RLSI is within QTA */
      && HDRRMAC_QW_FRAME_CMP(frame_end, rlsi_frame_end)))
  {
    /* If QTA start or end falls within the RLSI time  
          or RLSI is within QTA then return true */
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================
FUNCTION HDRRMAC4DRV_PA_ON                                          INTERNAL

DESCRIPTION
  Turns the reverse link on.

DEPENDENCIES
  None.

PARAMETERS
  immediate   - TRUE  => turn PA on immediately
                FALSE => turn PA on at slot indicated by the 'slot' parameter
  slot        - Slot offset at which to enable PA if not immediately

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_pa_on
(
  boolean immediate,
    /* Whether to enable as soon as possible */
  uint8 slot
    /* slot on which to enable */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Skip if PA enable has been scheduled. */
  if ( hdrrmac4drv.subframes_till_pa_enable != 0 )
  {
    HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, 
      "PA enable skipped, sfs to enable = %d",
      hdrrmac4drv.subframes_till_pa_enable );

    return;
  }

  /* Enable the transmitter. */
  hdrmod_setup_tx_timed_enable(immediate, slot);  
  hdrrmac4drv.pa_status = PA_ON;

  HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "Reverse Link Tx is now on");

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_pa_on() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_START_CHANNELS                                 EXTERNAL

DESCRIPTION
  Enables the channels specified by the channel mask.
  
DEPENDENCIES
  Uses demod carrier information (fwd_link_index) obtained form 
  update_rl_info() for enabling channels.

PARAMETERS
  phy_chan_mask - Physical channel ID mask
                  Set bit 'n' to enable channel 'n'
  new_connection_setup - TRUE if new connection setup procedure needs to be
                  followed for turning on RL carriers

RETURN VALUE
  Number of active channels.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4drv_start_channels
(
  uint16 phy_chan_mask,
    /* Physical channel mask */

  boolean new_connection_setup
    /* TRUE if AT is starting a new connection */
)
{
  uint8 ch, interlace;
  int16 pwr_adjust_dsp;
  int16 ch_rxagc;

  /* Best carrier attribues */
  uint8 ch_best = HDRRMAC4DRV_INVALID_CHAN_ID;
  int16 ch_best_drop_rank = -1;
  int16 ch_best_rxagc = -32767;

  /* Firmware commands */
  hdrfw_msg_u fw_cmd_misc, fw_cmd_enable_carrier, fw_cmd_pilotpwr_reset;

  /* Error status */
  errno_enum_type estatus;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware messages */
  memset ( &fw_cmd_enable_carrier, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD ); 

  memset ( &fw_cmd_pilotpwr_reset, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_pilotpwr_reset.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_RESET_PILOTPWR_FILT_CMD ); 

  memset ( &fw_cmd_misc, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_misc.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD ); 

  /* If this is a new connection setup, pick the best carrier to enable */
  if (new_connection_setup)
  {
    for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      if ((phy_chan_mask & (1<<ch)) != 0)
      {
        /* Obtain carrier RxAGC */
        ch_rxagc = rfm_hdr_get_rx_agc(hdrsrchrf_get_rf_dev(
          HDRSRCHRF_PRI_CHAIN),hdrrmac4drv.per_carrier_info[ch].demod_index,
          RFM_HDR_AGC_FORMAT__DBM256);

        /* Pick the best carrier by drop ranks. If there is a tie in
           drop ranks, pick the carrier with highest RxAGC value */
        if ((hdrrmac4drv.per_carrier_info[ch].drop_rank > ch_best_drop_rank)
          || ((hdrrmac4drv.per_carrier_info[ch].drop_rank == 
          ch_best_drop_rank) && (ch_rxagc > ch_best_rxagc)))
        {
          ch_best = ch;
          ch_best_drop_rank = hdrrmac4drv.per_carrier_info[ch].drop_rank;
          ch_best_rxagc = ch_rxagc;
        }
      } /* If the carrier is included in input mask */
    } /* For all carriers */

    /* Store the best carrier index */
    hdrrmac4drv.ch_best = ch_best;

    HDR_MSG_DRIVERS_2( MSG_LEGACY_HIGH, 
      "Start_Channels: i/p mask = 0x%x, best carrier = %d",
      phy_chan_mask,
      ch_best );

    for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      if ((phy_chan_mask & (1<<ch)) != 0)
      {
        if (ch != ch_best)
        {
          /* Mark the carrier for delayed enable decision */
          hdrrmac4drv.per_carrier_info[ch].off_till_mcdo_eval = TRUE;

          hdrrmac4drv.subframes_till_mcdo_eval = 
            HDRRMAC4DRV_MCDO_EVAL_SUBFRAMES;
        }
      } /* If the carrier is included in input mask */
      else
      {
        /* Reset off-till-MCDO-eval flag for any carrier that is not
           to be enabled */
        hdrrmac4drv.per_carrier_info[ch].off_till_mcdo_eval = FALSE;
      }
    } /* For all carriers */

    /* Modify input mask to just enable best carrier */
    phy_chan_mask = (1<<ch_best);

  } /* If this is a new connection */

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if ((phy_chan_mask & (1<<ch)) != 0)
    {
      /* If the channel is in INACTIVE state, enable channel */
      if (hdrrmac4drv.per_carrier_info[ch].carrier_state == 
          HDRRMAC4DRV_CH_ST_INACTIVE)
      {
        /* Reset off-till-MCDO-eval flag for any carrier that is being
           enabled */
        hdrrmac4drv.per_carrier_info[ch].off_till_mcdo_eval = FALSE;

        /* Reset flag indicating pending removal by TCA */
        hdrrmac4drv.per_carrier_info[ch].removed_by_tca = FALSE;

        /* Enable channel in modulator and MDSP */
        hdrmod_config_carrier(ch, TRUE, 1<<
          hdrrmac4drv.per_carrier_info[ch].demod_index);
        fw_cmd_enable_carrier.rmac_enable_carrier.
          carrierMask |= (1 << ch);
        fw_cmd_enable_carrier.rmac_enable_carrier.
          modulatorConfig[ch].carrierMask = (1 << 
          hdrrmac4drv.per_carrier_info[ch].demod_index);
        fw_cmd_enable_carrier.rmac_enable_carrier.
          modulatorConfig[ch].enable = 1;

        /* Compute "zero" value for txAdj */
        pwr_adjust_dsp = ( hdrrmac4drv_common->nominal_pwr_db256 -
          hdrrmac4drv_common->turnaround_db256 ) << 1;

        /* Enable RPC since we are enabling channel */
        if (hdrrmac4drv.rpc_disabled == FALSE)
        {
          if ( (hdrrmac4drv.processed_first_tca_after_access) ||
               ( ch != 0 ) )
          {
            /* Clear gain adjust and enable RPC */
            fw_cmd_misc.rmac_misc_cfg.fieldMask.
              gainAdjOverride_Valid = 1;

            fw_cmd_misc.rmac_misc_cfg.carrierMask |= (1 << ch);

            fw_cmd_misc.rmac_misc_cfg.C[ch].gainAdjWrite = pwr_adjust_dsp;

            fw_cmd_misc.rmac_misc_cfg.C[ch].gainAdjOverride.
              useGainAdjWrite = 1;
          
            fw_cmd_misc.rmac_misc_cfg.C[ch].gainAdjOverride.
              triggerMode = 1;
          }
          else
          {
            /* If this is the first TCA, maintain TxAdj value from end of 
               access for carrier 0 and enable RPC i.e. pass gainAdjOverride
               with all entries set to zero. */
            fw_cmd_misc.rmac_misc_cfg.fieldMask.
              gainAdjOverride_Valid = 1;
          }
        }
        else
        {

#ifdef FEATURE_FACTORY_TESTMODE
          if( ftm_get_mode() == FTM_MODE )
          { 
            /* RPC disabled in FTM mode */
            /* Override closed loop gains of three carriers 
               with specified gains */
            fw_cmd_misc.rmac_misc_cfg.fieldMask.
              gainAdjOverride_Valid = 1;
            fw_cmd_misc.rmac_misc_cfg.C[ch].gainAdjWrite = 
              hdrhitmsg_get_rmac_params_ptr()->revb_closed_loop_c[ch];
          }
          else
          {
#endif /* FEATURE_FACTORY_TESTMODE */

            if (hdrrmac4drv.processed_first_tca_after_access)
            {
              /* Clear gain adjust */
              fw_cmd_misc.rmac_misc_cfg.fieldMask.
                gainAdjOverride_Valid = 1;

              fw_cmd_misc.rmac_misc_cfg.carrierMask |= (1 << ch);
  
              fw_cmd_misc.rmac_misc_cfg.C[ch].gainAdjWrite = pwr_adjust_dsp;

              fw_cmd_misc.rmac_misc_cfg.C[ch].gainAdjOverride.
                useGainAdjWrite = 1;
            }

#ifdef FEATURE_FACTORY_TESTMODE
          }
#endif /* FEATURE_FACTORY_TESTMODE */

        }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#error code not present
#endif

        /* Save channel information in RL Metrics log packet.  This 
           information is not refreshed till the channel is 
           dropped/replaced. */
        hdrrmac4drv_metrics.log[ch].carrier_id.channel_id = ch;
        hdrrmac4drv_metrics.log[ch].carrier_id.channel_no = 
          (uint16) hdrrmac4drv.per_carrier_info[ch].channel_record.chan_num;
        hdrrmac4drv_metrics.log[ch].carrier_id.band_class = (uint16) ((uint32)
          (int32) hdrrmac4drv.per_carrier_info[ch].channel_record.band);

        if(!hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired)
        {
          /* Enable DRC. */
          fw_cmd_misc.rmac_misc_cfg.fieldMask.enableDRC_Valid = 1;
          fw_cmd_misc.rmac_misc_cfg.carrierMask |= ( 1 << ch);
          fw_cmd_misc.rmac_misc_cfg.C[ch].enableDrc = 1;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#error code not present
#endif

        }

        /* Enable and unfreeze DRC supervision timers in DSP */
        fw_cmd_misc.rmac_misc_cfg.fieldMask.enableDrcSupervision_Valid = 1;
        fw_cmd_misc.rmac_misc_cfg.C[ch].enableDrcSupervision = 1;
        fw_cmd_misc.rmac_misc_cfg.fieldMask.unfreezeDrcSupervision_Valid = 1;
        fw_cmd_misc.rmac_misc_cfg.C[ch].unfreezeDrcSupervision = 1;

        /* Clear RAB */
        fw_cmd_misc.rmac_misc_cfg.fieldMask.rabDecisionCtl_Valid = 1;
        fw_cmd_misc.rmac_misc_cfg.carrierMask |= ( 1 << ch);
        fw_cmd_misc.rmac_misc_cfg.C[ch].rabDecisionCtl.D = 1;
        fw_cmd_misc.rmac_misc_cfg.C[ch].rabDecisionCtl.R = 1;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#error code not present
#endif

        /* Reset -
           - MARQ status
           - Interlace history
           - Filtered RAB limited traffic power
           for the channels that are enabled. */

        /* Clear MARQ status */
        hdrrmac4drv.per_carrier_info[ch].marq_info_valid = FALSE;

        /* Clear interlace history, packets-in-transit count.  */
        memset((void*)hdrrmac4drv.per_carrier_info[ch].interlace_history, 0, 
          sizeof(hdrrmac4drv_packet_info_type) * 
          HDRSCMRMACCMN_MAX_NUM_INTERLACES);

        /* Set packet ID in all interlaces to be Idle */
        for (interlace = 0; 
             interlace < HDRSCMRMACCMN_MAX_NUM_INTERLACES;
             interlace++)
        {
          hdrrmac4drv.per_carrier_info[ch].interlace_history[interlace].
            packet_id = HDRRMAC4DRV_IDLE_PACKET;
        }

        /* Reset filtered RAB limited traffic power. */
        hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data = 
          HDRRMAC4DRV_FILT_RAB_LTD_PWR_MAX;

        /* Reset filtered power margin */
        hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin = 
          HDRRMAC4DRV_FILT_PWR_MARGIN_INVALID;

        /* Transition channel to SETUP state.  A channel should be in 
           SETUP state for start_open_state() to transition it to 
           OPEN state */
        hdrrmac4drv.per_carrier_info[ch].carrier_state = 
          HDRRMAC4DRV_CH_ST_SETUP;

        /* Reset filtered transmit pilot power for the newly added channel. */
        fw_cmd_pilotpwr_reset.rmac_reset_pilotpwrfilt.carrierMask |= (1 << ch);

      }
      else
      {
        HDR_MSG_DRIVERS_1( MSG_LEGACY_ERROR, 
          "Start Channels: Channel %d not in INACTIVE state!",
          ch );
      }
    }
  }

  /* Set TCA processed flag */
  hdrrmac4drv.processed_first_tca_after_access = TRUE;

  /* Send the commands to firmware if they are populated */
  if (fw_cmd_enable_carrier.rmac_enable_carrier.carrierMask != 0)
  {
    /* Pause from issuing next RMAC_ENABLE_CARRIER cmd
       to FW immediately */
    hdrtx_pause_fw_cmd_delivery();
    
    estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    estatus = msgr_send ( &fw_cmd_pilotpwr_reset.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    estatus = msgr_send ( &fw_cmd_misc.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    /* Unless we are already counting down for PA enable, schedule sending
       of PA enable command to FW */
    if( hdrrmac4drv.subframes_till_pa_enable == 0 )
    {
      hdrrmac4drv.subframes_till_pa_enable = 
        HDRRMAC4DRV_PA_ENABLE_DELAY_SUBFRAMES;
    }
  }


#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return hdrrmac4drv.num_chans;

} /* hdrrmac4drv_start_channels() */



/*===========================================================================
FUNCTION HDRRMAC4DRV_INIT_CARRIER_MANAGEMENT                        INTERNAL

DESCRIPTION
  This function initializes carrier management thresholds

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_init_carrier_management( void )
{
  /* Loop counter */
  uint8 ch;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* TxT2P_Remain_Active = 
     T2PHiCapPreTransition_RemainActivePS in Q.10 linear */
  hdrrmac4drv_carrier_mgt_thresh.t2p_remain_active = 
    hdrrmacmath_db_2_lin((
    hdrrmac4drv_config->t2p_info[HDRRMACCMN_TX_MODE_HI_CAP].
    tx_t2p_info[HDRRMAC4DRV_REMAIN_ACTIVE_PS].data_t2p_db[0]) << 18);

  /* TxT2P_Activate = 
     T2PHiCapPreTransition_ActivatePS in Q.10 linear */
  hdrrmac4drv_carrier_mgt_thresh.t2p_activate = 
    hdrrmacmath_db_2_lin((
    hdrrmac4drv_config->t2p_info[HDRRMACCMN_TX_MODE_HI_CAP].
    tx_t2p_info[HDRRMAC4DRV_ACTIVATE_PS].data_t2p_db[0]) << 18);

  /* TxT2P_Deactivate = 
     T2PHiCapPreTransition_DeactivatePS in Q.10 linear */
  hdrrmac4drv_carrier_mgt_thresh.t2p_deactivate = 
    hdrrmacmath_db_2_lin((
    hdrrmac4drv_config->t2p_info[HDRRMACCMN_TX_MODE_HI_CAP].
    tx_t2p_info[HDRRMAC4DRV_DEACTIVATE_PS].data_t2p_db[0]) << 18);

  /* TxT2P_Min_Rate = 
     T2PHiCapPreTransition_MinRatePS in Q.10 linear */
  hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate = 
    hdrrmacmath_db_2_lin((
    hdrrmac4drv_config->t2p_info[HDRRMACCMN_TX_MODE_HI_CAP].
    tx_t2p_info[HDRRMAC4DRV_MIN_RATE_PS].data_t2p_db[0]) << 18);

  /* TxT2P_Add = 
     T2PHiCapPreTransition_AddPS in Q.10 linear */
  hdrrmac4drv_carrier_mgt_thresh.t2p_add = 
    hdrrmacmath_db_2_lin((
    hdrrmac4drv_config->t2p_info[HDRRMACCMN_TX_MODE_HI_CAP].
    tx_t2p_info[HDRRMAC4DRV_ADD_PS].data_t2p_db[0]) << 18);

  /* Initialize prioritized carrier lists */
  for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv.prioritized_carriers.rtcmac[ch] = ch;
    hdrrmac4drv.prioritized_carriers.overall[ch] = ch;
    hdrrmac4drv.prioritized_carriers.caput[ch] = ch;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_init_carrier_management() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_ISR                                                 ISR

DESCRIPTION
  Wrapper function to call hdrrmac4drv.isr_ptr()
  Selects appropriate log buffer from which hdrrmac4drv_log_rl_gains() reads
  DigiGains
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_isr
( 
  uint32 unused_param
    /* Parameter */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Mark current tx info as not updated */
  hdrrmac4drv.current_tx_updated = FALSE;

  if (hdrrmac4drv.isr_ptr != NULL)
  {
    /* Call the function pointed by isr_ptr */
    hdrrmac4drv.isr_ptr();
  
#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
  #error code not present
#endif
  }

  return;

} /* hdrrmac4drv_isr() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SET_MARGIN_PARAMETERS                          INTERNAL

DESCRIPTION
  Set power margin parameters.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac4drv_set_margin_parameters( void )
{
  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware power margin configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_PWRMARGIN_CFG_CMD );

  /* Set up power margins for Rev B. HDRMDSP macros are not used to keep 
     interface internal. */
  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginMax = 
    HDRRMAC4DRV_TX_PWR_MARGIN_MAX_V;
  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginMin = 
    HDRRMAC4DRV_TX_PWR_MARGIN_MIN_V;
  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginUpStep = 
    HDRRMAC4DRV_TX_PWR_MARGIN_UP_STEP_V;
  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginDownStep = 
    HDRRMAC4DRV_TX_PWR_MARGIN_DOWN_STEP_V;

  if (hdrrmac4drv_margin_loop_use_peak)
  {
    fw_cmd.rmac_pwrmargin_cfg.txPwrMarginUseCL = 0; 
  }
  else
  {
    fw_cmd.rmac_pwrmargin_cfg.txPwrMarginUseCL = 1; 
  }

  /* Send power margin config command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

} /* hdrrmac4drv_set_margin_parameters() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_START                                          EXTERNAL

DESCRIPTION
  Installs the indicated function to be called each subframe. 
  
DEPENDENCIES
  None.

PARAMETERS
  isr_ptr - Function to be called when subframe interrupt occurs

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_start
(
  void (*isr_ptr)(void)
    /* Function to be called when subframe interrupt occurs. */
)
{
  /* Loop counters */
  uint8 ch;

  /* Major channel state: INACTIVE, SETUP, OPEN */
  hdrrmac4drv_carrier_state_enum_type major_channel_state;

  /* Firmware commands */
  hdrfw_msg_u fw_cmd, fw_cmd_enable_carrier;

  /* Error status */
  errno_enum_type estatus;

  /* Whether to send carrier enable message */
  boolean send_enable = FALSE;

  /* Mask for enabling carrier */
  uint8 enable_carrier_mask = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD );

  /* Initialize carrier enable message */
  memset ( &fw_cmd_enable_carrier, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD ); 

  /* Save isr */
  hdrrmac4drv.isr_ptr = isr_ptr;

  /* Reset silence interval flag and error setting */
  hdrrmac4drv.silence_now_flag = FALSE;
  hdrrmac4drv.timing_error = FALSE;

  /* Initialize PA status */
  hdrrmac4drv.pa_status = PA_OFF;

  /* Reset suspend count */
  hdrrmac4drv.subframes_to_suspend = HDRRMAC_CANCEL_SUSPEND;

  /* Compute next silence interval time, if enabled */
  if (hdrrmac4drv_common->silence_enabled)
  {
    hdrrmac4drv_compute_silence_time();
  }

#ifndef FEATURE_RUMI_BRINGUP
  /* Used when FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST is 
     defined.  If not defined, the rf function does nothing. */
#ifdef FEATURE_RF_SVDO_API
  rfm_hdr_notify_data_rate( hdrsrchrf_get_rf_dev(HDRSRCHRF_PRI_CHAIN), 
                            RFM_HDR_REVB_RATE_OFFSET );

#else
  rfm_hdr_notify_data_rate(RF_HDR_REVB_RATE_OFFSET);   

#endif /* FEATURE_RF_SVDO_API */

#endif /*  FEATURE_RUMI_BRINGUP */

  /* If the silence interval is in progress, don't turn PA on yet. 
     If PA is scheduled for enable (PENDING), do not turn it on now.  It 
     will be enabled at next frame boundary.  */
  if ((hdrrmac4drv.pa_status != PA_ON_PENDING) &&
      ((!hdrrmac4drv.silence_now_flag)
     || (hdrrmac4drv.silence_now_flag && (hdrrmac4drv.silence_count < 0))))
  {
    /* Set field mask */
    fw_cmd.rmac_misc_cfg.fieldMask.rabDecisionCtl_Valid = 1;
    fw_cmd.rmac_misc_cfg.fieldMask.unfreezeDrcSupervision_Valid = 1;
    fw_cmd.rmac_misc_cfg.fieldMask.enableDRC_Valid = 1;
    fw_cmd.rmac_misc_cfg.fieldMask.gainAdjOverride_Valid = 1;

    /* Make sure DRC, RPC and RAB are enabled correctly. */
    for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      major_channel_state = (hdrrmac4drv_carrier_state_enum_type)
        (((uint16)hdrrmac4drv.per_carrier_info[ch].carrier_state) &
        ((uint16)HDRRMAC4DRV_CH_ST_MAJOR_MASK));

      if (major_channel_state == HDRRMAC4DRV_CH_ST_OPEN)
      {
        fw_cmd.rmac_misc_cfg.carrierMask |= (1 << ch);

        /* Enable RAB for OPEN channels */
        fw_cmd.rmac_misc_cfg.C[ch].rabDecisionCtl.D = 0;
        fw_cmd.rmac_misc_cfg.C[ch].rabDecisionCtl.R = 0;
        fw_cmd.rmac_misc_cfg.C[ch].rabDecisionCtl.M = 0;

        /* Unfreeze DRC supervision timers in DSP */
        fw_cmd.rmac_misc_cfg.C[ch].unfreezeDrcSupervision = 1;

      } /* if the channel is in OPEN state */

      else if (major_channel_state == HDRRMAC4DRV_CH_ST_SETUP)
      {
        fw_cmd.rmac_misc_cfg.carrierMask |= (1 << ch);

        /* Keep RAB disabled for SETUP state channels */
        fw_cmd.rmac_misc_cfg.C[ch].rabDecisionCtl.D = 1;

        /* Unfreeze DRC Supervision timers in DSP */
        fw_cmd.rmac_misc_cfg.C[ch].unfreezeDrcSupervision = 1;

      } /* if the channel is in SETUP state */

      if ( (major_channel_state == HDRRMAC4DRV_CH_ST_OPEN) || 
           (major_channel_state == HDRRMAC4DRV_CH_ST_SETUP) )
      {
        /* Enable DRC and RPC for channels not in DRC supervision */
        if (hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired == FALSE)
        {
          /* Enable DRC. */
          fw_cmd.rmac_misc_cfg.C[ch].enableDrc = 1;

          if (hdrrmac4drv.rpc_disabled == FALSE)
          {
            /* Enable the transmit closed loop power control (RPC). */
            fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjVal = 0;
            fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjWrite = 0;
          }
          else
          {
            /* Disable RPC. */
            fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjVal = 1;
            fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.carrier = ch;
          }

          /* Enable the carrier if it is not removed by TCA */
          if (hdrrmac4drv.per_carrier_info[ch].removed_by_tca == FALSE)
          {
            fw_cmd_enable_carrier.rmac_enable_carrier.
              modulatorConfig[ch].carrierMask = (1 << 
              hdrrmac4drv.per_carrier_info[ch].demod_index);
            fw_cmd_enable_carrier.rmac_enable_carrier.
              modulatorConfig[ch].enable = 1;
            enable_carrier_mask |= (1 << ch);

            /* Enable carrier in HW. */
            hdrmod_config_carrier(ch, TRUE, 1<<
              hdrrmac4drv.per_carrier_info[ch].demod_index);

            send_enable = TRUE;
          }

        } /* if the channel is not under DRC supervision */
        else
        {
          /* Disable DRC. */
          fw_cmd.rmac_misc_cfg.C[ch].enableDrc = 0;

          /* Disable RPC. */
          fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjVal = 1;
          fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.carrier = ch;
        }
      }

    } /* for all channels */

    /* Send RMAC config command to firmware */
    estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    if (send_enable) 
    {
      fw_cmd_enable_carrier.rmac_enable_carrier.carrierMask=enable_carrier_mask;

      /* Pause from issuing next RMAC_ENABLE_CARRIER cmd
         to FW immediately */
      hdrtx_pause_fw_cmd_delivery();
      
      /* Send RMAC enable command to firmware */
      estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof(hdrfw_msg_u) );
      ASSERT ( estatus == E_SUCCESS );

      /* Unless we are already counting down for PA enable, schedule sending
         of PA enable command to FW */
      if( hdrrmac4drv.subframes_till_pa_enable == 0 )
      {
        hdrrmac4drv.subframes_till_pa_enable = 
          HDRRMAC4DRV_PA_ENABLE_DELAY_SUBFRAMES;
      }
    }
    else
    {
      HDR_MSG_DRIVERS(MSG_LEGACY_HIGH, 
        "hdrrmac4drv_start(): PA and carrier enable skipped");
    }

  }
  else
  {
    HDR_MSG_DRIVERS(MSG_LEGACY_HIGH, 
      "hdrrmac4drv_start(): PA not turned on due to RLSI.");
  }

  /* Set margin parameters */
  hdrrmac4drv_set_margin_parameters();

  /* Reset subframe id. */
  hdrrmac4drv.subframe_id = HDRRMAC4DRV_INVALID_PACKET;

  /* Reset previous subframe number */
  hdrrmac4drv.prev_subframe.rlsi    = HDRRMAC4DRV_INVALID_PACKET;
  hdrrmac4drv.prev_subframe.suspend = HDRRMAC4DRV_INVALID_PACKET;

  /* If TX/PA enable is pending, isr is in use already.  Correct isr
     will be installed when tx is enabled. */
  if (hdrrmac4drv.pa_status != PA_ON_PENDING)
  {
    /* Install the appropriate isr. */
    hdrmod_enable_frame_isr( hdrrmac4drv_isr );
  }

  HDRTRACE_RMAC_TX_STARTED();

  /* Report AGC here */
  hdrtx_report_agc( 0 );
  /* RMG: Do we need to pass the phy_chan_mask here so that the AGC of only 
          the FL channels associated with the started RL channels can be 
          reported */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_start() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SUBMIT_RL_GAIN_LOG                            INTERNAL

DESCRIPTION
  This function logs the reverse gains log packet once enough data
  frames have been collected.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_submit_rl_gain_log( void )
{
  LOG_HDR_MC_RL_GAINS_C_type *log_ptr;
    /* Log pointer to the RL gains packet */

  hdrlog_rl_gains_pwr_info_type  *log_pwr_ptr;
    /* Pointer to per-subframe power information in log packet */

  hdrlog_rl_gains_chan_wide_info_type *log_chan_ptr;
    /* Pointer to channel wide information in log packet */

  hdrlog_rl_gains_per_chan_subframe_wide_info_type *log_sf_ptr;
    /* Pointer to subframe wide information in log packet */

  uint8 num_chans_to_log = 0;
    /* Number of channels with valid arq status log information */

  uint8 sf, ch, open_chan_id;
    /* Loop counters */

  uint8 open_chan_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0};
    /* List of open channels to be logged */

  uint32 log_record_length;
    /* Log packet size in bytes */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* Determine number of channels to log */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv_gain_log_info.enable_channel_logging[ch])
    {
      /* Store the channel index in open channel list */
      open_chan_list[num_chans_to_log] = ch;

      /* Increment number of channels to log */
      num_chans_to_log++;
    }
  }


  /* Make sure there are entries to submit. */
  if ((hdrrmac4drv_gain_log_info.sf_count != 0) &&
      (num_chans_to_log != 0))
  {
    /* This is a variable size record. Allocate just enough memory for the
     * log buffer to accommodate diag data for the number of subframes and
     * channels.
     */
    log_record_length = HDRLOG_VAR_SIZEOF_2( LOG_HDR_MC_RL_GAINS_C_type,
      hdrlog_rl_gains_chan_wide_info_type,
      num_chans_to_log,
      hdrlog_rl_gains_per_chan_subframe_wide_info_type,
      hdrrmac4drv_gain_log_info.sf_count ) + 
      (hdrrmac4drv_gain_log_info.sf_count - 1) *
      sizeof(hdrlog_rl_gains_pwr_info_type);

    log_ptr = (LOG_HDR_MC_RL_GAINS_C_type *)
      log_alloc_ex((log_code_type) LOG_HDR_MC_RL_GAINS_C ,
      log_record_length );

    if (log_ptr != NULL)
    {
      /* Log packet version */
      log_ptr->version = HDRLOG_MC_RL_GAINS_VER;

      /* Current subframe count */
      log_ptr->subframe_count = hdrrmac4drv_gain_log_info.sf_count;

      /* Number of logged channels */
      log_ptr->num_carriers = num_chans_to_log;

      /* Intialize log packet per-subframe power record pointer */
      log_pwr_ptr = log_ptr->power_info;

      /* Fill per-subframe power information */
      for (sf = 0; sf < log_ptr->subframe_count; sf++)
      {
#ifdef FEATURE_MEMCPY_REMOVAL
       *log_pwr_ptr = hdrrmac4drv_gain_log_info.pwr_info[sf];
#else
        memcpy((void*)(log_pwr_ptr),
          (void*)&(hdrrmac4drv_gain_log_info.pwr_info[sf]),
          sizeof(hdrlog_rl_gains_pwr_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

        /* Increment subframe record pointer */
        log_pwr_ptr++;
      }

      /* Initialize log packet channel record pointer */
      log_chan_ptr = (hdrlog_rl_gains_chan_wide_info_type *)log_pwr_ptr;

      /* Fill channel info for open channels */
      for (ch = 0; ch < num_chans_to_log; ch++)
      {
        open_chan_id = open_chan_list[ch];

        /* Channel information */
        log_chan_ptr->carrier_id.channel_id = open_chan_id;
        log_chan_ptr->carrier_id.channel_no = 
          (uint16) hdrrmac4drv.per_carrier_info[open_chan_id].
          channel_record.chan_num;
        log_chan_ptr->carrier_id.band_class = (uint16) ((uint32) (int32)
          hdrrmac4drv.per_carrier_info[open_chan_id].channel_record.band);

        /* Initialize log packet subframe record pointer */
        log_sf_ptr = &log_chan_ptr->subframe_info[0];

        /* Fill per-subframe info */
        for (sf = 0; sf < log_ptr->subframe_count; sf++)
        {
#ifdef FEATURE_MEMCPY_REMOVAL
          *log_sf_ptr = hdrrmac4drv_gain_log_info.log[open_chan_id][sf];
#else
          memcpy((void*)(log_sf_ptr),
            (void*)&(hdrrmac4drv_gain_log_info.log[open_chan_id][sf]),
            sizeof(hdrlog_rl_gains_per_chan_subframe_wide_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

          /* Increment subframe record pointer */
          log_sf_ptr++;
        }

        /* Set the addres of the next channel record */
        log_chan_ptr = (hdrlog_rl_gains_chan_wide_info_type *)log_sf_ptr;

      }

      /* submit the log to diag */
      log_commit( (log_type *) log_ptr );

    } /* if log_ptr != NULL */
    else
    {
      HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH,
        "Cannot allocate RLGains log buffer (%d subfrms)",
        hdrrmac4drv_gain_log_info.sf_count );

    } /* if log_ptr == NULL */

  } /* if there are valid entries to submit */

  /* Reset subframe count and channel logging enabled flags */
  hdrrmac4drv_gain_log_info.sf_count = 0;

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv_gain_log_info.enable_channel_logging[ch] = FALSE;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_submit_rl_gain_log() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SUBMIT_ARQ_STATUS_LOG                         INTERNAL

DESCRIPTION
  This function logs the RMAC4 ARQ status log packet.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_submit_arq_status_log( void )
{
  LOG_HDR_MC_RL_ARQ_STATUS_C_type *log_ptr;
    /* Log pointer to the ARQ status info packet */

  hdrlog_rl_arq_status_chan_wide_info_type *log_chan_ptr;
    /* Pointer to channel wide information in log packet */

  hdrlog_rl_arq_status_subframe_wide_info_type *log_sf_ptr;
    /* Pointer to subframe wide information in log packet */

  uint8 num_chans_to_log = 0;
    /* Number of channels with valid arq status log information */

  uint8 sf, ch, open_chan_id;
    /* Loop counters */

  uint8 open_chan_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0, 1, 2};
    /* List of open channels to be logged - initialized to keep lint happy */

  uint32 log_record_length;
    /* Log packet size in bytes */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine number of channels to log */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv_arq_status_log_info.enable_channel_logging[ch])
    {
      /* Store the channel index in open channel list */
      open_chan_list[num_chans_to_log] = ch;

      /* Increment number of channels to log */
      num_chans_to_log++;
    }
  }

  /* Make sure there are entries to submit. */
  if ((hdrrmac4drv_arq_status_log_info.sf_count != 0) &&
      (num_chans_to_log != 0))
  {
    /* This is a variable size record. Allocate just enough memory for the
     * log buffer to accommodate diag data for the number of subframes and
     * channels.
     */
    log_record_length = HDRLOG_VAR_SIZEOF_2( LOG_HDR_MC_RL_ARQ_STATUS_C_type,
      hdrlog_rl_arq_status_chan_wide_info_type,
      num_chans_to_log,
      hdrlog_rl_arq_status_subframe_wide_info_type,
      hdrrmac4drv_arq_status_log_info.sf_count );

    log_ptr = (LOG_HDR_MC_RL_ARQ_STATUS_C_type *)
      log_alloc_ex((log_code_type) LOG_HDR_MC_RL_ARQ_STATUS_C ,
      log_record_length );

    if (log_ptr != NULL)
    {
      /* Log packet version */
      log_ptr->version = HDRLOG_MC_RL_ARQ_STATUS_VER;

      /* Current subframe count */
      log_ptr->subframe_count = hdrrmac4drv_arq_status_log_info.sf_count;

      /* Number of logged channels */
      log_ptr->num_carriers = num_chans_to_log;

      /* Initialize log packet channel record pointer */
      log_chan_ptr = &log_ptr->rl_arq_status[0];

      /* Fill channel info for open channels */
      for (ch = 0; ch < num_chans_to_log; ch++)
      {
        open_chan_id = open_chan_list[ch];

        /* Channel information */
        log_chan_ptr->carrier_id.channel_id = open_chan_id;
        log_chan_ptr->carrier_id.channel_no = 
          (uint16) hdrrmac4drv.per_carrier_info[open_chan_id].
          channel_record.chan_num;
        log_chan_ptr->carrier_id.band_class = (uint16) ((uint32) (int32)
          hdrrmac4drv.per_carrier_info[open_chan_id].channel_record.band);

        /* Initialize log packet subframe record pointer */
        log_sf_ptr = &log_chan_ptr->subframe_info[0];

        /* Fill per-subframe info */
        for (sf = 0; sf < log_ptr->subframe_count; sf++)
        {
#ifdef FEATURE_MEMCPY_REMOVAL
          *log_sf_ptr = hdrrmac4drv_arq_status_log_info.log[open_chan_id][sf];
#else
          memcpy((void*)(log_sf_ptr),
            (void*)&(hdrrmac4drv_arq_status_log_info.log[open_chan_id][sf]),
            sizeof(hdrlog_rl_arq_status_subframe_wide_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

          /* Increment subframe record pointer */
          log_sf_ptr++;
        }

        /* Set the addres of the next channel record */
        log_chan_ptr = (hdrlog_rl_arq_status_chan_wide_info_type *)log_sf_ptr;

      }

      /* submit the log to diag */
      log_commit( (log_type *) log_ptr );

    } /* if log_ptr != NULL */
    else
    {
      HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH,
        "Cannot allocate ARQStatus log buffer (%d subfrms)",
        hdrrmac4drv_arq_status_log_info.sf_count );

    } /* if log_ptr == NULL */

  } /* if there are valid entries to submit */

  /* Reset subframe count and channel logging enabled flags */
  hdrrmac4drv_arq_status_log_info.sf_count = 0;

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv_arq_status_log_info.enable_channel_logging[ch] = FALSE;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_submit_arq_status_log() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SUBMIT_RAB_UPDATE_INFO_LOG                     INTERNAL

DESCRIPTION
  This function logs RMAC4 RAB update info log packet.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_submit_rab_update_info_log( void )
{
  LOG_HDR_MC_RAB_UPDATE_INFO_C_type  *log_ptr;
    /* Log pointer to the RAB update info packet */

  hdrlog_rab_update_chan_wide_info_type *log_chan_ptr;
    /* Pointer to channel wide information in log packet */

  hdrlog_rab_update_subframe_wide_info_type *log_sf_ptr;
    /* Pointer to subframe wide information in log packet */

  uint8 num_chans_to_log = 0;
    /* Number of channels with valid rab update information */

  uint8 sf, ch, open_chan_id;
    /* Loop counters */

  uint8 open_chan_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0, 1, 2};
    /* List of open channels to be logged - 
       initialized to remove lint warning. */

  uint32 log_record_length;
    /* Log packet size in bytes */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine number of channels to log */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv_rab_update_log_info.enable_channel_logging[ch])
    {
      /* Store the channel index in open channel list */
      open_chan_list[num_chans_to_log] = ch;

      /* Increment number of channels to log */
      num_chans_to_log++;
    }
  }

  /* Make sure there are entries to submit. */
  if ((hdrrmac4drv_rab_update_log_info.sf_count != 0) &&
      (num_chans_to_log != 0))
  {
    /* This is a variable size record. Allocate just enough memory for the
     * log buffer to accommodate diag data for the number of subframes and
     * channels.
     */
    log_record_length = HDRLOG_VAR_SIZEOF_2( LOG_HDR_MC_RAB_UPDATE_INFO_C_type,
      hdrlog_rab_update_chan_wide_info_type,
      num_chans_to_log,
      hdrlog_rab_update_subframe_wide_info_type,
      hdrrmac4drv_rab_update_log_info.sf_count );

    log_ptr = (LOG_HDR_MC_RAB_UPDATE_INFO_C_type *)
      log_alloc_ex((log_code_type) LOG_HDR_MC_RAB_UPDATE_INFO_C , 
      log_record_length );

    if (log_ptr != NULL)
    {
      /* Log packet version */
      log_ptr->version = HDRLOG_MC_RAB_UPDATE_INFO_VER;

      /* Current subframe count */
      log_ptr->subframe_count = hdrrmac4drv_rab_update_log_info.sf_count;

      /* Number of logged channels */
      log_ptr->num_carriers = num_chans_to_log;

      /* Initialize log packet channel record pointer */
      log_chan_ptr = &log_ptr->rab_update_info[0];

      /* Fill channel info for open channels */
      for (ch = 0; ch < num_chans_to_log; ch++)
      {
        open_chan_id = open_chan_list[ch];

        /* Channel information */
        log_chan_ptr->carrier_id.channel_id = open_chan_id;
        log_chan_ptr->carrier_id.channel_no = 
          (uint16) hdrrmac4drv.per_carrier_info[open_chan_id].
          channel_record.chan_num;
        log_chan_ptr->carrier_id.band_class = (uint16) ((uint32) 
          (int32) hdrrmac4drv.per_carrier_info[open_chan_id].
          channel_record.band);

        /* Initialize log packet subframe record pointer */
        log_sf_ptr = &log_chan_ptr->subframe_info[0];

        /* Fill per-subframe info */
        for (sf = 0; sf < log_ptr->subframe_count; sf++)
        {
#ifdef FEATURE_MEMCPY_REMOVAL
          *log_sf_ptr = hdrrmac4drv_rab_update_log_info.log[open_chan_id][sf];
#else
          memcpy((void*)(log_sf_ptr),
            (void*)&(hdrrmac4drv_rab_update_log_info.log[open_chan_id][sf]),
            sizeof(hdrlog_rab_update_subframe_wide_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

          /* Increment subframe record pointer */
          log_sf_ptr++;
        }

        /* Set the addres of the next channel record */
        log_chan_ptr = (hdrlog_rab_update_chan_wide_info_type *)log_sf_ptr;

      }

      /* submit the log to diag */
      log_commit( (log_type *) log_ptr );

    } /* if log_ptr != NULL */
    else
    {
      HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH,
        "Cannot allocate RABUpdateInfo log buffer (%d subfrms)",
        hdrrmac4drv_rab_update_log_info.sf_count );

    } /* if log_ptr == NULL */

  } /* if there are valid entries to submit */

  /* Reset subframe count and channel logging enabled flags */
  hdrrmac4drv_rab_update_log_info.sf_count = 0;

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv_rab_update_log_info.enable_channel_logging[ch] = FALSE;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

}  /* hdrrmac4drv_submit_rab_update_info_log() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SUBMIT_PKT_STATUS_LOG                          INTERNAL

DESCRIPTION
  This function logs the RMAC4 pkt status log packet once enough data
  frames have been collected.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_submit_pkt_status_log( void )
{
  LOG_HDR_MC_RL_PKT_STATUS_C_type *log_ptr;
    /* Log pointer to the RL status info packet */

  hdrlog_rl_pkt_status_chan_wide_info_type *log_chan_ptr;
    /* Pointer to carrier wide information in log packet */

  hdrlog_rl_pkt_status_packet_wide_info_type *log_pkt_ptr;
    /* Pointer to packet wide information in log packet */

  uint8 total_packet_count = 0;
    /* Total number of packets to be logged */

  uint8 num_chans_to_log = 0;
    /* Number of channels with valid rl pkt status log information */

  uint8 pkt, ch, chan_id;
    /* Loop counters */

  uint8 chan_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0, 1, 2};
    /* List of channels to be logged -
       initialized to keep lint happy. */

  uint32 log_record_length;
    /* Log packet size in bytes */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine number of packets to log */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv_pkt_status_log_info.packet_count[ch] > 0)
    {
      /* Store the channel index in channel list */
      chan_list[num_chans_to_log] = ch;

      /* Increment number of channels to log */
      num_chans_to_log++;

      /* Increment total packets to log */
      total_packet_count += hdrrmac4drv_pkt_status_log_info.packet_count[ch];
    }
  }

  /* Make sure there are entries to submit. */
  if (total_packet_count != 0)
  {
    /* This is a variable size record. Allocate just enough memory for the
     * log buffer to accommodate diag data for the number of subframes and
     * channels.
     */
    log_record_length = 
      sizeof(LOG_HDR_MC_RL_PKT_STATUS_C_type) + 
      (num_chans_to_log - 1) * 
      sizeof(hdrlog_rl_pkt_status_chan_wide_info_type) + 
      (total_packet_count - num_chans_to_log) * 
      sizeof(hdrlog_rl_pkt_status_packet_wide_info_type);

    log_ptr = (LOG_HDR_MC_RL_PKT_STATUS_C_type *)
      log_alloc_ex((log_code_type) LOG_HDR_MC_RL_PKT_STATUS_C ,
      log_record_length );

    if (log_ptr != NULL)
    {
      /* Log packet version */
      log_ptr->version = HDRLOG_MC_RL_PKT_STATUS_VER;

      /* Number of logged channels */
      log_ptr->num_carriers = num_chans_to_log;

      /* Initialize log packet channel record pointer */
      log_chan_ptr = &log_ptr->rl_pkt_status[0];

      /* Fill channel info for the channels to be logged */
      for (ch = 0; ch < num_chans_to_log; ch++)
      {
        chan_id = chan_list[ch];

        /* Channel information */
        log_chan_ptr->carrier_id.channel_id = chan_id;
        log_chan_ptr->carrier_id.channel_no = 
          (uint16) hdrrmac4drv.per_carrier_info[chan_id].
          channel_record.chan_num;
        log_chan_ptr->carrier_id.band_class = (uint16) ((uint32) (int32)
          hdrrmac4drv.per_carrier_info[chan_id].channel_record.band);

        /* Packet count */
        log_chan_ptr->packet_count = 
          hdrrmac4drv_pkt_status_log_info.packet_count[chan_id];

        /* Initialize log packet pkt record pointer */
        log_pkt_ptr = &log_chan_ptr->pkt_info[0];

        /* Fill per-packet info */
        for (pkt = 0; pkt < log_chan_ptr->packet_count; pkt++)
        {
#ifdef FEATURE_MEMCPY_REMOVAL
          *log_pkt_ptr = hdrrmac4drv_pkt_status_log_info.log[chan_id][pkt];
#else
          memcpy((void*)(log_pkt_ptr),
            (void*)&(hdrrmac4drv_pkt_status_log_info.log[chan_id][pkt]),
            sizeof(hdrlog_rl_pkt_status_packet_wide_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

          /* Increment packet record pointer */
          log_pkt_ptr++;
        }

        /* Set the address of the next channel record */
        log_chan_ptr = 
          (hdrlog_rl_pkt_status_chan_wide_info_type *)log_pkt_ptr;

      }

      /* submit the log to diag */
      log_commit( (log_type *) log_ptr );

    } /* if log_ptr != NULL */
    else
    {
      HDR_MSG_DRIVERS_2( MSG_LEGACY_HIGH,
        "Cannot allocate PktStatus log buffer (%d chans, %d pkts)",
        num_chans_to_log,
        total_packet_count );

    } /* if log_ptr == NULL */

  } /* if there are valid entries to submit */

  /* Reset packet counts */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv_pkt_status_log_info.packet_count[ch] = 0;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_submit_pkt_status_log() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SUBMIT_RL_METRICS_LOG                          INTERNAL

DESCRIPTION
  This function submits RL Metrics log for all channels.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_submit_rl_metrics_log( void )
{
  LOG_HDR_MC_RL_METRICS_C_type *log_ptr;
    /* Log pointer to the RAB update info packet */

  uint8 ch;
    /* Loop counter */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    log_ptr = (LOG_HDR_MC_RL_METRICS_C_type *)
      log_alloc_ex((log_code_type) LOG_HDR_MC_RL_METRICS_C ,
      sizeof(LOG_HDR_MC_RL_METRICS_C_type) );

    if (log_ptr != NULL)
    {
      /* Log packet version */
      log_ptr->version = HDRLOG_MC_RL_METRICS_VER;

      /* Log sequence number */
      log_ptr->log_seq_num = hdrrmac4drv_metrics.metrics_sequence_num;

      /* Number of open channels */
      log_ptr->num_carriers = hdrrmac4drv.num_chans;

#ifdef FEATURE_MEMCPY_REMOVAL
      log_ptr->mc_rl_pkt_metrics = hdrrmac4drv_metrics.log[ch];
#else
      memcpy((void*)&(log_ptr->mc_rl_pkt_metrics),
        (void*)&hdrrmac4drv_metrics.log[ch],
        sizeof(hdrlog_rl_metrics_chan_wide_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      /* submit the log to diag */
      log_commit( (log_type *) log_ptr );

    } /* if log_ptr != NULL */
    else
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_HIGH,
        "Cannot allocate RLMetrics log buffer" );

    } /* if log_ptr == NULL */

  }

  /* Increment sequence number.  The sequence number is incremented per RL 
     Metric submission and not per log packet i.e. RL Metric log packets for
     all the channels logged in this submission will have the same metric 
     sequence number */
  hdrrmac4drv_metrics.metrics_sequence_num++;

  /* Reset submit_rl_metrics flag */
  hdrrmac4drv_metrics.submit_rl_metrics = FALSE;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_submit_rl_metrics_log() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SUBMIT_PA_HDRM_CM_INFO_LOG                    INTERNAL

DESCRIPTION
  This function logs the RMAC4 PA Headroom and Carrier Management info log
  packet.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_submit_pa_hdrm_cm_info_log( void )
{
  LOG_HDR_MC_PA_HDRM_AND_CARRIER_MGMT_C_type *log_ptr;
    /* Log pointer to the PA headroom and carrier mgmt info packet */

  hdrlog_rl_hdrm_and_carrier_mgmt_sf_wide_info_type *log_sf_ptr;
    /* Pointer to subframe wide information in log packet */

  hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_chan_wide_info_type *log_chan_ptr;
    /* Pointer to per-subframe, per-channel information in log packet */

  uint8 num_chans_to_log = 0;
    /* Number of channels with valid hdrm log information */

  uint8 sf, ch;
    /* Loop counters */

  uint8 logged_chan_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0,1,2};
    /* List of channels to be logged - 
       initialized to remove lint warning. */

  uint8 logged_chan_id;
    /* Channel id of channel being logged */

  uint32 log_record_length;
    /* Log packet size in bytes */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine number of channels to log */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv_hdrm_cm_log_info.enable_channel_logging[ch])
    {
      /* Store the channel index in open channel list */
      logged_chan_list[num_chans_to_log] = ch;

      /* Increment number of channels to log */
      num_chans_to_log++;
    }
  }

  /* Make sure there are entries to submit. */
  if ((hdrrmac4drv_hdrm_cm_log_info.sf_count != 0) &&
      (num_chans_to_log != 0))
  {
    /* This is a variable size record. Allocate just enough memory for the
     * log buffer to accommodate diag data for the number of subframes and
     * channels.
     */
    log_record_length = HDRLOG_VAR_SIZEOF_2( 
      LOG_HDR_MC_PA_HDRM_AND_CARRIER_MGMT_C_type,
      hdrlog_rl_hdrm_and_carrier_mgmt_sf_wide_info_type,
      hdrrmac4drv_hdrm_cm_log_info.sf_count,
      hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_chan_wide_info_type,
      num_chans_to_log );

    log_ptr = (LOG_HDR_MC_PA_HDRM_AND_CARRIER_MGMT_C_type *)
      log_alloc_ex((log_code_type) LOG_HDR_MC_PA_HDRM_AND_CARRIER_MGMT_C,
      log_record_length );

    if (log_ptr != NULL)
    {
      /* Log packet version */
      log_ptr->version = HDRLOG_MC_PA_HDRM_AND_CARRIER_MGMT_VER;

      /* Current subframe count */
      log_ptr->subframe_count = hdrrmac4drv_hdrm_cm_log_info.sf_count;

      /* Number of logged channels */
      log_ptr->num_carriers = num_chans_to_log;

      /* Populate channel data entries for this subframe */
      for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
      {
        /* Channel information */
        if ( hdrrmac4drv.per_carrier_info[ch].carrier_state != 
          HDRRMAC4DRV_CH_ST_INACTIVE )
        {
          log_ptr->rl_hdrm_cm_chan_info[ch].carrier_id.channel_id = ch;
        }
        else
        {
          /* Parsing tools should look at channel id and not display
             the channel number and band class information if the channel
             id is invalid (HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS) */
          log_ptr->rl_hdrm_cm_chan_info[ch].carrier_id.channel_id = 
            HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;
        }

        log_ptr->rl_hdrm_cm_chan_info[ch].carrier_id.channel_no = 
          (uint16) hdrrmac4drv.per_carrier_info[ch].channel_record.chan_num;
        log_ptr->rl_hdrm_cm_chan_info[ch].carrier_id.band_class = (uint16) 
          ((uint32) (int32) hdrrmac4drv.per_carrier_info[ch].
          channel_record.band);

        /* Dropping rank */
        log_ptr->rl_hdrm_cm_chan_info[ch].ch_info.dropping_rank = 
          hdrrmac4drv.per_carrier_info[ch].drop_rank;

        /* Clear reserved bits */
        log_ptr->rl_hdrm_cm_chan_info[ch].ch_info.reserved = 0;

      }

      /* Initialize log packet subframe record pointer */
      log_sf_ptr = &log_ptr->rl_hdrm_cm_sf_info[0];

      /* Fill per-subframe info */
      for (sf = 0; sf < log_ptr->subframe_count; sf++)
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        log_sf_ptr->sf_info = hdrrmac4drv_hdrm_cm_log_info.log_sf[sf];
#else
        memcpy((void*)(&log_sf_ptr->sf_info),
          (void*)&(hdrrmac4drv_hdrm_cm_log_info.log_sf[sf]),
          sizeof(hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

        /* Initialize log packet per-sf->per-channel record pointer */
        log_chan_ptr = &log_sf_ptr->carrier_info[0];

        /* Fill per-subframe per-carrier info */
        for (ch = 0; ch < num_chans_to_log; ch++)
        {
          logged_chan_id = logged_chan_list[ch];

#ifdef FEATURE_MEMCPY_REMOVAL
          *log_chan_ptr = hdrrmac4drv_hdrm_cm_log_info.log_carrier[sf]
                                                         [logged_chan_id];
#else
          memcpy((void*)(log_chan_ptr),
            (void*)&(hdrrmac4drv_hdrm_cm_log_info.log_carrier[sf][logged_chan_id]),
            sizeof(
            hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_chan_wide_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

          /* Increment channel record pointer */
          log_chan_ptr++;
        }

        /* Set the address of the next channel record */
        log_sf_ptr = (hdrlog_rl_hdrm_and_carrier_mgmt_sf_wide_info_type *)
          log_chan_ptr;

      }

      /* submit the log to diag */
      log_commit( (log_type *) log_ptr );

    } /* if log_ptr != NULL */
    else
    {
      HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH,
        "Cannot allocate PAHdrmCMgmt log buffer (%d subfrms)",
        hdrrmac4drv_hdrm_cm_log_info.sf_count );

    } /* if log_ptr == NULL */

  } /* if there are valid entries to submit */

  /* Reset subframe count and channel logging enabled flags */
  hdrrmac4drv_hdrm_cm_log_info.sf_count = 0;

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv_hdrm_cm_log_info.enable_channel_logging[ch] = FALSE;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;
  
} /* hdrrmac4drv_submit_pa_hdrm_cm_info_log() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_STOP_CHANNEL                                   INTERNAL

DESCRIPTION
  Disables the specified channel.
  
DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index of channel to be disabled

RETURN VALUE
  Number of active channels

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_stop_channel
(
  uint8 phy_chan_idx
)
{

  /* Major channel state: INACTIVE, SETUP, OPEN */
  hdrrmac4drv_carrier_state_enum_type major_channel_state;

  /* Firmware commands */
  hdrfw_msg_u fw_cmd_misc, fw_cmd_enable_carrier;

  /* Error status */
  errno_enum_type estatus;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware messages */
  memset ( &fw_cmd_misc, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_misc.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD ); 

  memset ( &fw_cmd_enable_carrier, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD ); 

  major_channel_state = (hdrrmac4drv_carrier_state_enum_type)
    (((uint16) hdrrmac4drv.per_carrier_info[phy_chan_idx].carrier_state) &
    ((uint16) HDRRMAC4DRV_CH_ST_MAJOR_MASK));

  /* If the channel is not already disabled, disable channel */
  if ((major_channel_state == HDRRMAC4DRV_CH_ST_OPEN) || 
      (major_channel_state == HDRRMAC4DRV_CH_ST_SETUP))
  {
    /* Disable RPC */
    fw_cmd_misc.rmac_misc_cfg.fieldMask.gainAdjOverride_Valid = 1;
    fw_cmd_misc.rmac_misc_cfg.carrierMask |= (1 << phy_chan_idx);

    fw_cmd_misc.rmac_misc_cfg.C[phy_chan_idx].gainAdjOverride.
      carrier = phy_chan_idx;

    fw_cmd_misc.rmac_misc_cfg.C[phy_chan_idx].gainAdjOverride.
      useGainAdjVal = 1;

    /* Disable DRC supervision */
    fw_cmd_misc.rmac_misc_cfg.fieldMask.enableDrcSupervision_Valid = 1;
    fw_cmd_misc.rmac_misc_cfg.C[phy_chan_idx].enableDrcSupervision = 0;

    /* Send command to firmware */
    estatus = msgr_send ( &fw_cmd_misc.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    /* Disable channel */ 
    hdrmod_config_carrier(phy_chan_idx, FALSE, 1<<phy_chan_idx);

    fw_cmd_enable_carrier.rmac_enable_carrier.carrierMask |= 
      (1 << phy_chan_idx);
    fw_cmd_enable_carrier.rmac_enable_carrier.modulatorConfig
      [phy_chan_idx].enable = 0;

      /* Pause from issuing next RMAC_ENABLE_CARRIER cmd
         to FW immediately */
    hdrtx_pause_fw_cmd_delivery();

    /* Send carrier disable command to firmware */
    estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    /* Set channel to INACTIVE state and reset okay-to-deactivate flag */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].carrier_state = 
      HDRRMAC4DRV_CH_ST_INACTIVE;
    hdrrmac4drv.per_carrier_info[phy_chan_idx].okay_to_deactivate = 
      FALSE;

    /* Clear forward link mapping */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].fwd_link_index = 
      HDRRMAC4DRV_INVALID_CHAN_ID;
    hdrrmac4drv.per_carrier_info[phy_chan_idx].demod_index = 
      HDRRMAC4DRV_INVALID_CHAN_ID;

    if ( hdrrmac4drv.per_carrier_info[phy_chan_idx].drc_supervision_expired 
         == TRUE )
    {
      /* We only need to inform FTCMAC that DRC supervision timer should be 
         reset if FTC MAC has started the timer for that carrier. Otherwise 
         RMAC will end up receiving exit_drc_supervision for carrier who has
         never received enter_drc_supervision() before. One side effect is 
         that start/stop flexconn dtx monitor will be messed up.
      */

      /* Reset DRC supervision status */
      hdrrmac4drv.per_carrier_info[phy_chan_idx].drc_supervision_expired = 
        FALSE;

      /* Inform FTC MAC that DRC supervision timer should be reset */
      hdrfmac_exit_drc_supervision(1 << phy_chan_idx);

    }
    /* Reset channel transmit pilot power differential flag */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].drop_carrier_txp_diff = 
      FALSE;

    /* Reset channel reportable status */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].reportable = FALSE;

    /* Submit collected logs */
    hdrrmac4drv_submit_rl_gain_log();
    hdrrmac4drv_submit_arq_status_log();
    hdrrmac4drv_submit_rab_update_info_log();
    hdrrmac4drv_submit_pkt_status_log();
    hdrrmac4drv_submit_pa_hdrm_cm_info_log();

    /* Reset channel drop rank */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].drop_rank = 0;

    /* Reset flag indicating pending removal by TCA */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].removed_by_tca = FALSE;

    /* Clear channel information from RL Metrics */
    hdrrmac4drv_metrics.log[phy_chan_idx].carrier_id.channel_id = 
                                                   HDRRMAC4DRV_INVALID_CHAN_ID;
    hdrrmac4drv_metrics.log[phy_chan_idx].carrier_id.channel_no = 0;
    hdrrmac4drv_metrics.log[phy_chan_idx].carrier_id.band_class = 0;

    /* Clear interlace history */
    memset((void*)hdrrmac4drv.per_carrier_info[phy_chan_idx].interlace_history,
      0, 
      sizeof(hdrrmac4drv_packet_info_type) * HDRSCMRMACCMN_MAX_NUM_INTERLACES);

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
    /* Clear current and not-data-limited throughput values */
    hdrrmac4drv.per_carrier_info[phy_chan_idx].curr_tput = 0;
    hdrrmac4drv.per_carrier_info[phy_chan_idx].not_data_lmtd_tput = 0;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

    /* Decrement number of active channels */
    if (major_channel_state == HDRRMAC4DRV_CH_ST_OPEN)
    {
      hdrrmac4drv.num_chans--;
      ASSERT( hdrrmac4drv.num_chans <= HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );
    }

    HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, 
      "Stop Channel: Channel %d disabled",
      phy_chan_idx );
  }
  else
  {
    HDR_MSG_DRIVERS_1( MSG_LEGACY_MED, 
      "Stop Channel: Channel %d already disabled!",
      phy_chan_idx );
  }

} /* hdrrmac4drv_stop_channel() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_STOP_CHANNELS                                  EXTERNAL

DESCRIPTION
  Disables the channels specified by the channel mask.
  
DEPENDENCIES
  None.

PARAMETERS
  phy_chan_mask - Physical channel ID mask
                  Set bit 'n' to disable channel 'n'

RETURN VALUE
  Number of active channels

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4drv_stop_channels
(
  uint16 phy_chan_mask
)
{
  /* Loop counters */
  uint8 ch, idx;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if ((phy_chan_mask & (1<<ch)) != 0)
    {
      /* Disable channel */
      hdrrmac4drv_stop_channel(ch);

    } /* if the channel needs to be disabled */

  } /* for all channels */

  /* Update OPEN channel list */
  hdrrmac4drv_update_open_chan_list();

  /* Set appropriate carrier states.
     Note that the carrier states are reset to allow data on 
     all assigned carriers every time a carrier is removed by AN. */
  for (idx=0; idx < hdrrmac4drv.num_chans; idx++)
  {
    ch = hdrrmac4drv.open_chans[idx];

    /* Change the states of all OPEN channels to DATA_ACTIVE */
    hdrrmac4drv.per_carrier_info[ch].carrier_state = 
      HDRRMAC4DRV_CH_ST_DATA_ACTIVE;
  }


  /* Update list of channels carrying data */
  hdrrmac4drv_update_open_chan_data_list();

  /* Update number of reportable carriers */
  hdrrmac4drv_update_num_reportable_carriers();

  /* Set appropriate carrier configuration. */
  if (hdrrmac4drv.num_chans == 1)
  {
    /* Carrier configuration with one active channel */
    hdrrmac4drv.carrier_config_id = CARRIER_CONFIG_P0_D0;

  }
  else if (hdrrmac4drv.num_chans == 2)
  {
    /* Carrier configuration with two active channels */
    hdrrmac4drv.carrier_config_id = CARRIER_CONFIG_P01_D01;

  }
  else if (hdrrmac4drv.num_chans == 3)
  {
    /* Carrier configuration with three active channels */
    hdrrmac4drv.carrier_config_id = CARRIER_CONFIG_P012_D012;

  }
  else if (hdrrmac4drv.num_chans == 0)
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
      "Stop Channels: No open channels" );

    /* Reset SLP_Selected flag to facilitate picking the first 
       Open RL as the SLP carrier. */
    hdrrmac4drv_slp_selected = FALSE;

  }
  else
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
      "Stop Channels: Invalid number of open channels!" );
  }

  /* Funnel state can change due to change in number of assigned 
     or data carrying carriers => update funnel state. */
  hdrrmac4drv_update_funnel_state();

  /* Reset target carrier config to carrier config value */
  hdrrmac4drv.target_carrier_config_id = hdrrmac4drv.carrier_config_id;


#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return hdrrmac4drv.num_chans;

} /* hdrrmac4drv_stop_channels() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_PA_OFF                                         INTERNAL

DESCRIPTION
  Turns the reverse link off.

DEPENDENCIES
  None.

PARAMETERS
  immediate   - TRUE  => turn PA off immediately
                FALSE => turn PA off at slot indicated by the 'slot' parameter
  slot        - Slot offset at which to disable PA if not immediately

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_pa_off
(
  boolean immediate,
    /* Whether to disable immediately */
  uint8 slot
    /* slot on which to disable */
)
{
#if defined FEATURE_HDR_REVB_DTX_MODE || defined FEATURE_HDR_TRAFFIC_DTX
  boolean dtx_mode = FALSE;
#endif /*
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(hdrrmac4drv.pa_status != PA_OFF)
  {
    hdrrmac4drv.pa_status = PA_OFF_PENDING;

#if defined FEATURE_HDR_REVB_DTX_MODE || defined FEATURE_HDR_TRAFFIC_DTX
    /* If dtx_mode is enabled and we are about to turn PA off then
       turn off dtx mode and register to enable it on PA_ON */
#ifdef FEATURE_HDR_TRAFFIC_DTX
      dtx_mode = hdrrmac4drv_common->tdtx_mode;
#endif /* FEATURE_HDR_TRAFFIC_DTX */
#ifdef FEATURE_HDR_REVB_DTX_MODE
      dtx_mode = dtx_mode  || hdrrmac4drv_common->network_dtx_mode;
#endif /* FEATURE_HDR_REVB_DTX_MODE */

if (dtx_mode == TRUE)
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "n/t DTX: Turning off dtx_mode "
                                        "before PA off" );
      hdrrmac4drv_update_dtx_mode( );      

    }
#endif /* FEATURE_HDR_REVB_DTX_MODE || FEATURE_HDR_TRAFFIC_DTX */

    hdrrmac4drv.pa_status = PA_OFF;

    /* Disable PA */
    hdrmod_setup_tx_timed_disable(immediate, slot);  

    HDRTRACE_RMAC_TX_STOPPED();

    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "Reverse Link Tx is now off" );
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_pa_off() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_STOP                                           EXTERNAL

DESCRIPTION
  This function turns off the PA off, disables the frame interrupt, and 
  disables DRC.  The internally maintained channel states are not affected.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_stop
( 
#ifdef FEATURE_HDR_QTA 
   boolean is_qta /* Is power up tx after QTA? */
#else
  void
#endif /* FEATURE_HDR_QTA */
)
{
  /* loop counter */
  uint8 ch; 

  /* Firmware command */
  hdrfw_msg_u fw_cmd_reset_interlaces, fw_cmd_misc;

  /* Firmware command */
  hdrfw_msg_u fw_cmd_enable_carrier;

  /* Error status */
  errno_enum_type estatus;

  /* Carrier mask for PA disable */
  uint8 disable_carrier_mask = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware messages */
  memset ( &fw_cmd_misc, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_misc.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD );

  memset ( &fw_cmd_reset_interlaces, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_reset_interlaces.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_RESET_RL_INTERLACE_CMD );

  memset ( &fw_cmd_enable_carrier, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD ); 

  /* If shutdown is pending, call callback. */
  if (hdrrmac4drv.shutdown_pending == TRUE)
  {

    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Tx stop during shutdown...calling shutdown_cb()" );

    hdrrmac4drv.shutdown_pending = FALSE;
    hdrrmac4drv.shutdown_cb();
  }

  /* Uninstall interrupt */
  hdrrmac4drv_disable_frame_isr();

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    fw_cmd_misc.rmac_misc_cfg.carrierMask |= (1 << ch);

    /* Disable RPC */
    fw_cmd_misc.rmac_misc_cfg.fieldMask.
      gainAdjOverride_Valid = 1;
    fw_cmd_misc.rmac_misc_cfg.C[ch].
      gainAdjOverride.carrier = ch;
    fw_cmd_misc.rmac_misc_cfg.C[ch].
      gainAdjOverride.useGainAdjVal = 1;

    /* Disable DRC */
    fw_cmd_misc.rmac_misc_cfg.fieldMask.
      enableDRC_Valid = 1;
    fw_cmd_misc.rmac_misc_cfg.C[ch].
      enableDrc = 0;

    /* Freeze RAB */
    fw_cmd_misc.rmac_misc_cfg.fieldMask.
      rabDecisionCtl_Valid = 1;
    fw_cmd_misc.rmac_misc_cfg.C[ch].
      rabDecisionCtl.D = 1;

    /* Freeze DRC supervision */
    fw_cmd_misc.rmac_misc_cfg.fieldMask.unfreezeDrcSupervision_Valid = 1;
    fw_cmd_misc.rmac_misc_cfg.C[ch].unfreezeDrcSupervision = 0;

    /* Cancel any packets in progress by forcing an interlace
       reset in the DSP. */
    fw_cmd_reset_interlaces.rmac_reset_rl_interlace.carrierMask |= (1 << ch);

    /* Free the last packet loaded.  May be NULL if no 
       packet was scheduled. */
    dsm_free_packet(&hdrrmac4drv.per_carrier_info[ch].pkt_ptr);

    fw_cmd_enable_carrier.rmac_enable_carrier.modulatorConfig
      [ch].enable = 0;
    disable_carrier_mask |= (1 << ch);

    /* Disable carrier in modulator */
    hdrmod_config_carrier(ch, FALSE, 1<<ch);
  }

  /* Send the constructed messages to firmware */
  estatus = msgr_send ( &fw_cmd_misc.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  estatus = msgr_send ( &fw_cmd_reset_interlaces.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );
  
#ifdef FEATURE_HDR_QTA 
  if (!is_qta)
#endif /* FEATURE_HDR_QTA */
  {
    fw_cmd_enable_carrier.rmac_enable_carrier.carrierMask = disable_carrier_mask;

    /* Send carrier disable command to firmware */
    estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    /* Make sure PA, DRC and RAB are disabled. */
    /* Disable PA*/
    hdrrmac4drv_pa_off(TRUE, 0);
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#error code not present
#endif

  /* Submit remaining collected logs */
  hdrrmac4drv_submit_rl_gain_log();
  hdrrmac4drv_submit_arq_status_log();
  hdrrmac4drv_submit_rab_update_info_log();
  hdrrmac4drv_submit_pkt_status_log();
  hdrrmac4drv_submit_pa_hdrm_cm_info_log();

  /* Reset RL gain logging info */
  hdrrmac4drv_gain_log_info.prev_pa_enabled = FALSE;
  hdrrmac4drv_gain_log_info.backoff = 0;
  hdrrmac4drv_gain_log_info.max_avail_tx_pwr = 0;


  /* Reset carrier management */
  hdrrmac4drv_init_carrier_management();

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_stop() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_EXIT                                           EXTERNAL

DESCRIPTION
  Performs exit processing when RTCMAC is deactivated.    
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_exit( void )
{

  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware modulator configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_MODULATOR_CFG_CMD );

  /* Place MDSP in Rel0 mode. Modulator is set to Rev 0 on hdrmod_exit() */
#ifdef FEATURE_HDR_BOLT_MODEM
  fw_cmd.rmac_modulator_cfg.hdrModulatorModeConfig = HDRMOD_TRAFFIC_MODE_REV0;
#else
  fw_cmd.rmac_modulator_cfg.hdrModulatorModeConfig = 
    HDRMOD_TRAFFIC_MODE_REV0 << HDRMODE_MODULATOR_SHIFT;
#endif /* FEATURE_HDR_BOLT_MODEM */

  /* Send modulator config command to firmware.  Fields other than 
     hdrModulatorModeConfig are not relevant to Rel0 and so are not 
     populated. */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  /* DSP PA headroom alg. is set back to Rev 0 config. */

  /* Restore rel0 power margin configuration */

  /* Initialize firmware power margin configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_PWRMARGIN_CFG_CMD );

  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginMax = 0x0000;      /* Max:        0 dB */
  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginMin = 0xf100;      /* Min:      - 6 dB */
  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginUpStep = 0x0050;   /* Up step:   .5/4  */
  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginDownStep = 0x0028; /* Down step: .25/4 */
  fw_cmd.rmac_pwrmargin_cfg.txPwrMarginUseCL = 0x0000; 
    /* Use peak CL adjustment to calculate power margin*/

  /* Send power margin config command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_REVB_DTX_MODE
  /* Disable DTXMode */
  hdrrmac4drv_common->network_dtx_mode = FALSE;

  hdrrmac4drv_update_dtx_mode( ); 
#endif /* FEATURE_HDR_REVB_DTX_MODE */

  /* Reset DRC supervision */
  hdrrmac4drv_reset_drc_supervision();

#if defined FEATURE_HDR_REVB_DTX_MODE || defined FEATURE_HDR_TRAFFIC_DTX
  HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
                   "n/t DTX: exit rmac4, deregister PA monitor.");

  /* We dont need to monitor PA anymore. */
  hdrmod_deregister_pa_mode_monitor();
#endif /* FEATURE_HDR_REVB_DTX_MODE */

  return;

} /* hdrrmac4drv_exit() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_PACKETS_ARE_ACTIVE                             INTERNAL

DESCRIPTION
  Returns TRUE if there are any packets active (scheduled with no ACK yet
  received) on the requested carrier on any interlace.
    
DEPENDENCIES
  None.
  
PARAMETERS
  phy_chan_id - Carrier to be checked for active packets.

RETURN VALUE
  TRUE/FALSE based on whether packets are active.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_packets_are_active
(
  uint8 phy_chan_id
)
{
  uint8 i;  /* Loop variable. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  /* Check interlace buffer for active packets */
  for (i=0; i<HDRSCMRMACCMN_MAX_NUM_INTERLACES; i++)
  {
    if ( hdrrmac4drv.per_carrier_info[phy_chan_id].interlace_history[i].
         rri != 0 )
    {
      return TRUE;
    }
  }

  return FALSE;

} /* hdrrmac4drv_packets_are_active() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_MARQ_STATUS_IS_PENDING                         INTERNAL

DESCRIPTION
  Returns TRUE if there are any packets with pending MARQ status.
    
DEPENDENCIES
  None.
  
PARAMETERS
  None.  

RETURN VALUE
  TRUE/FALSE based on whether MARQ is pending.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_marq_status_is_pending( void )
{
  uint8 ch;        /* Loop variables. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  /* Check MARQ info valid flag for all the channels */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv.per_carrier_info[ch].marq_info_valid)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* hdrrmac4drv_marq_status_is_pending() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SHUTDOWN                                       EXTERNAL

DESCRIPTION
  This function starts shutdown procedure.  When all packets in transmit 
  have been completed, the callback will be called.  No new packets will
  be sent after this call (unless RTC MAC is re-activated).

DEPENDENCIES
  None.

PARAMETERS
  Callback function to call when all interlaces have completed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_shutdown
(
  hdrrmac4drv_shutdown_cb_type shutdown_cb
)
{
  /* Whether any packets are active on any interlace. */
  boolean packets_active = FALSE;

  /* Loop counter */
  uint8 ch; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure cb pointer is valid */ 
  if (shutdown_cb != NULL)
  {
    /* Check if packets are active on any interlace. */    
    for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      if (hdrrmac4drv_packets_are_active(ch))
      {
        packets_active = TRUE;

        break;
      }
    }

    /* If nothing is in progress, call callback now. */ 
    if (!packets_active)
    {
      shutdown_cb(); 
    }
    else
    {
      /* Otherwise set shutdown pending so we can call it later. */
      hdrrmac4drv.shutdown_cb = shutdown_cb;
      hdrrmac4drv.shutdown_pending = TRUE;
    }
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_shutdown() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_CHECK_PILOT_POWER_DIFFERENTIAL                 INTERNAL

DESCRIPTION
  - Calculates and stores per-carrier filtered pilot power difference wrt
    min pilot power carrier.
  - Performs RL pilot power differential check.

DEPENDENCIES
  hdrrmac4drv_update_rtcmac_priorities() must be called before calling
  this function.  This function uses RTCMAC prioritized carrier list
  as a list of carriers sorted by filtered transmit pilot powers.

  Note: Although RTCMAC prioritized carrier list has a built in hysteresis
  ( 1 dB HDRRMAC4DRV_FILT_TXP_DELTA ), as long as the hysteresis delta is 
  lower than max_rl_pilot_pwr_diff ( min val: 8 dB ), the list of carriers
  sorted by filtered transmit pilot powers is not required to be repopulated
  in this function.
  
PARAMETERS
  filt_txp_dbm - pointer to HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS instances of
                 filtered transmit pilot power in Q.10 dBm.

RETURN VALUE
  TRUE  - Exercised power differential corrective action 
            i.e. flagged the carrier with high tx pilot power for 
            carrier drop.
  FALSE - No carrier was scheduled for carrier drop.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_check_pilot_power_differential( void )
{
  /* Loop counter */
  uint8  ch_idx;

  /* Temporary variable to hold channel number */
  uint8 ch;

  /* Minimum transmit pilot power in Q.10 dBm */
  int32 min_txp_dbm;

  /* Per-carrier filtered tx pilot pwr diff wrt min pwr carrier in Q.10 dBm */
  int32 filt_txp_diff_q10[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0,0,0};

  /* Index of max pilot pwr carrier */
  uint8 c_max_txp;

  /* TRUE if AT is under DRC supervision */
  boolean under_drc_supervision = FALSE;

  /* TRUE if a candidate for carrier drop due to pwr diff has been found */
  boolean drop_candidate_found = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Perform power differential check if -
     - power differential check enabled by AN and 
     - we are not operating in TAP mode. */
  if ( (hdrrmac4drv_config->max_rl_pilot_pwr_diff != 0) && 
    !(hdrrmac4drv.throttle_disable) )
  {
    /* Initialize min filtered tx pilot power variable */
    min_txp_dbm = 
      hdrrmac4drv.per_carrier_info[hdrrmac4drv.open_chans[0]].filt_txp_dbm;

    for ( ch_idx=0; ch_idx<hdrrmac4drv.num_chans; ch_idx++ )
    {
      ch = hdrrmac4drv.open_chans[ch_idx];

      /* Find min filt tx pilot pwr */
      if (min_txp_dbm > hdrrmac4drv.per_carrier_info[ch].filt_txp_dbm)
      {
        min_txp_dbm = hdrrmac4drv.per_carrier_info[ch].filt_txp_dbm;
      }

      /* Check for DRC supervision */
      if (hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired)
      {
        /* Skip pilot power differential carrier drop if any carrier is to be 
         * dropped due to DRC supervision.  This is to avoid getting into a
         * situation where all the carriers are dropped in the next subframe
         * without sending a connection close (e.g. Two carrier scenario 
         * with carriers C0 and C1 where C0 dropped due to power differential
         * and C1 dropped due to DRC supervision in the same subframe.)
         */
        under_drc_supervision = TRUE;
      }

    } /* For all OPEN channels */

    /* Store pilot power difference with respect to min pilot pwr carrier 
       for all OPEN state channels */
    for ( ch_idx=0; ch_idx<hdrrmac4drv.num_chans; ch_idx++ )
    {
      ch = hdrrmac4drv.open_chans[ch_idx];

      /* Calculate filtered pilot power difference. */
      filt_txp_diff_q10[ch] = hdrrmac4drv.per_carrier_info[ch].filt_txp_dbm - 
        min_txp_dbm;

      /* Store in Q.2 dBm */
      hdrrmac4drv.per_carrier_info[ch].filt_txp_diff = 
        filt_txp_diff_q10[ch] >> 8;
    }

    /* Flag a carrier for carrier drop due to pilot power differential if 
     * the following conditions are satisfied:
     * 1. AT is not under DRC supervision 
     * 2. A carrier drop candidate has not already been found - only one 
     *    carrier can be dropped per subframe due to power differential
     * 3. Carriers with drop rank 0x7 are to be excluded from power
     *    differential based carrier drop
     * 4. The power difference wrt min pilot power carrier is greater than 
     *    max RL pilot power diff threshold.
     */
    if (!under_drc_supervision)
    {
      for ( ch_idx=(hdrrmac4drv.num_chans-1); 
            (!drop_candidate_found && (ch_idx>0)); ch_idx-- )
      {
        /* Look up index of the carrier with the least RTCMAC priority. */
        c_max_txp = hdrrmac4drv.prioritized_carriers.rtcmac[ch_idx];

        if ( ( hdrrmac4drv.per_carrier_info[c_max_txp].drop_rank != 
               HDRRMAC4DRV_MAX_DROP_RANK ) &&
             ( filt_txp_diff_q10[c_max_txp] > 
               hdrrmac4drv_config->max_rl_pilot_pwr_diff ) )
        {
          hdrrmac4drv.per_carrier_info[c_max_txp].drop_carrier_txp_diff = TRUE;

          /* Set flag to indicate that a drop candidate has been found. */
          drop_candidate_found = TRUE;

          /* Force SLP carrier switch if the current SLP carrier is to be 
             dropped due to pilot power differential. */
          if (c_max_txp == hdrrmac4drv.slp_carrier.current)
          {
            hdrrmac4_force_slp_switch = TRUE;
          }
        }
      }

    }  /* For all OPEN channels */

  } /* If pwr diff check is enabled */
  else
  {
    /* Reset pilot power difference with respect to min pilot pwr carrier 
       for all channels */
    for ( ch_idx=0; ch_idx<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch_idx++ )
    {
      hdrrmac4drv.per_carrier_info[ch_idx].filt_txp_diff = 0;
    }

  } /* If pwr diff check is disabled */

  return drop_candidate_found;

} /* hdrrmac4drv_check_pilot_power_differential */


/*===========================================================================
FUNCTION HDRRMAC4DRV_IS_POWER_DIFF_CARRIER_DROP                     EXTERNAL

DESCRIPTION
  Returns if a carrier drop due to pilot power differential is needed.  If
  so, physical carrier index of the carrier to be dropped is also returned.

DEPENDENCIES
  None.
  
PARAMETERS
  phy_chan_idx - pointer to location where physical channel index of 
    the carrier to be dropped is returned.

RETURN VALUE
  TRUE  - A carrier drop due to pilot power differential is needed.
  FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_is_power_diff_carrier_drop
(
  uint8 * phy_chan_idx
)
{
  /* Loop counters. */
  uint8 ch, idx;

  /* Flag to indicate if a carrier drop is needed. */
  boolean drop_carrier = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *phy_chan_idx = HDRRMAC4DRV_INVALID_CHAN_ID;

  /* For all carriers in OPEN state, check if any of the carriers are marked
     to be dropped due to transmit pilot power differential. */
  for (idx=0; idx < hdrrmac4drv.num_chans; idx++)
  {
    ch = hdrrmac4drv.open_chans[idx];

    if (hdrrmac4drv.per_carrier_info[ch].drop_carrier_txp_diff)
    {
      /* Set the carrier drop flag. */
      drop_carrier = TRUE;

      /* Populate the index of carrier to be dropped. */
      *phy_chan_idx = ch;

      /* Reset power differential carrier drop flag */
      hdrrmac4drv.per_carrier_info[ch].drop_carrier_txp_diff = FALSE;

      break;
    }
  }

  return drop_carrier;

} /* hdrrmac4drv_is_power_diff_carrier_drop() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SYNC_SHUTDOWN_PA                               INTERNAL

DESCRIPTION
  Little helper function to schedule the PA to turn off at the frame boundary.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_sync_shutdown_pa( void )
{
  /* Loop counter */
  uint8 ch;

  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

  /* Carrier mask */
  uint8 carrier_mask = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize firmware configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD );

  /* Disable PA at slot 0. */
  hdrrmac4drv_pa_off(FALSE, 0);

  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    carrier_mask |= (1 << ch);

    /* Disable RPC */
    fw_cmd.rmac_misc_cfg.fieldMask.
      gainAdjOverride_Valid = 1;
    fw_cmd.rmac_misc_cfg.C[ch].
      gainAdjOverride.carrier = ch;
    fw_cmd.rmac_misc_cfg.C[ch].
      gainAdjOverride.useGainAdjVal = 1;

    /* Disable/freeze RAB for RLSI*/
    fw_cmd.rmac_misc_cfg.fieldMask.
      rabDecisionCtl_Valid = 1;
    fw_cmd.rmac_misc_cfg.C[ch].
      rabDecisionCtl.D = 1;
    fw_cmd.rmac_misc_cfg.C[ch].
      rabDecisionCtl.M = 1;
  }

  fw_cmd.rmac_misc_cfg.carrierMask = carrier_mask;

  /* Send command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  return;

} /* hdrrmac4drv_sync_shutdown_pa() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SYNC_START_PA                                  INTERNAL

DESCRIPTION
  Little helper function to schedule the PA to turn on at the frame boundary.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_sync_start_pa( void )
{
  uint8   ch;               /* loop counter for channels */
  boolean start_pa = FALSE; /* Flag to start PA */
  hdrrmac4drv_carrier_state_enum_type major_channel_state;
                            /* Major channel state: INACTIVE, SETUP, OPEN */
  hdrfw_msg_u fw_cmd, fw_cmd_enable_carrier;       
                            /* Firmware commands */
  errno_enum_type estatus;  /* Error status */
  uint8 carrier_mask = 0;   /* Mask for carrier misc config */
  uint8 enable_carrier_mask = 0;
                            /* Mask for carrier enable */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
  hdrrmac_start_flexconn_dtx_monitor( TRUE );
    /* start flexconn/dtx monitoring after RLSI.*/
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */

  hdrrmac4drv.pa_status = PA_ON_PENDING;

  /* Initialize firmware configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD );

  /* Initialize carrier enable message */
  memset ( &fw_cmd_enable_carrier, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD ); 

  /* Set field mask */
  fw_cmd.rmac_misc_cfg.fieldMask.rabDecisionCtl_Valid = 1;
  fw_cmd.rmac_misc_cfg.fieldMask.unfreezeDrcSupervision_Valid = 1;

  /* Make sure DRC, RPC and RAB are enabled correctly. */
  for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    major_channel_state = (hdrrmac4drv_carrier_state_enum_type)
      (((uint16) hdrrmac4drv.per_carrier_info[ch].carrier_state) &
      ((uint16) HDRRMAC4DRV_CH_ST_MAJOR_MASK));

    if (major_channel_state == HDRRMAC4DRV_CH_ST_OPEN)
    {
      carrier_mask |= (1 << ch);

      /* Enable RAB for OPEN channels */
      fw_cmd.rmac_misc_cfg.C[ch].
        rabDecisionCtl.D = 0;
      fw_cmd.rmac_misc_cfg.C[ch].
        rabDecisionCtl.R = 0;
      fw_cmd.rmac_misc_cfg.C[ch].
        rabDecisionCtl.M = 0;

      /* Unfreeze DRC supervision timers in DSP */
      fw_cmd.rmac_misc_cfg.C[ch].unfreezeDrcSupervision = 1;

      /* Enable RPC for channels not in DRC supervision */
      if (hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired == FALSE)
      {
        /* There is at least one OPEN channel that is not under 
           DRC supervision */
        start_pa = TRUE;

        /* Enable the transmit closed loop power control (RPC). */
        if (hdrrmac4drv.rpc_disabled == FALSE)
        {
          /* Enable RPC */
          fw_cmd.rmac_misc_cfg.fieldMask.
            gainAdjOverride_Valid = 1;
          fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjVal = 0;
          fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjWrite = 0;
        }

        /* Enable carrier in FW and modulator. */
        fw_cmd_enable_carrier.rmac_enable_carrier.
          modulatorConfig[ch].carrierMask = (1 << 
          hdrrmac4drv.per_carrier_info[ch].demod_index);
        fw_cmd_enable_carrier.rmac_enable_carrier.
          modulatorConfig[ch].enable = 1;
        enable_carrier_mask |= (1 << ch);

        hdrmod_config_carrier(ch, TRUE, 1<<
          hdrrmac4drv.per_carrier_info[ch].demod_index);

      } /* if the channel is not under DRC supervision */

    } /* if the channel is in OPEN state */

    else if (major_channel_state == HDRRMAC4DRV_CH_ST_SETUP)
    {
      carrier_mask |= (1 << ch);

      start_pa = TRUE;

      /* Keep RAB disabled for SETUP state channels */
      fw_cmd.rmac_misc_cfg.C[ch].rabDecisionCtl.D = 1;

      /* Unfreeze DRC Supervision timers in DSP */
      fw_cmd.rmac_misc_cfg.C[ch].unfreezeDrcSupervision = 1;

      /* Enable the transmit closed loop power control (RPC). */
      if (hdrrmac4drv.rpc_disabled == FALSE)
      {
        /* Enable RPC */
        fw_cmd.rmac_misc_cfg.fieldMask.
          gainAdjOverride_Valid = 1;
        fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjVal = 0;
        fw_cmd.rmac_misc_cfg.C[ch].gainAdjOverride.useGainAdjWrite = 0;
      }

      /* Enable carrier in FW and modulator. */
      fw_cmd_enable_carrier.rmac_enable_carrier.
        modulatorConfig[ch].carrierMask = (1 << 
        hdrrmac4drv.per_carrier_info[ch].demod_index);
      fw_cmd_enable_carrier.rmac_enable_carrier.
        modulatorConfig[ch].enable = 1;
      enable_carrier_mask |= (1 << ch);

      hdrmod_config_carrier(ch, TRUE, 1<<
        hdrrmac4drv.per_carrier_info[ch].demod_index);

    } /* if the channel is in SETUP state */

  } /* for all channels */

  fw_cmd.rmac_misc_cfg.carrierMask = carrier_mask;

  /* Send command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  /* Turn PA on if at least one -
     - channel in SETUP state
     - channel in OPEN state that is not under DRC supervision. */
  if (start_pa)
  {
    fw_cmd_enable_carrier.rmac_enable_carrier.carrierMask=enable_carrier_mask;

    /* Pause from issuing next RMAC_ENABLE_CARRIER cmd
       to FW immediately */
    hdrtx_pause_fw_cmd_delivery();
    
    /* Send carrier enable command to firmware */
    estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    /* Enable the transmitter at slot 0. */
    hdrrmac4drv_pa_on(FALSE, 0);

  }
  else
  {
    hdrrmac4drv.pa_status = PA_OFF;
    HDR_MSG_DRIVERS(MSG_LEGACY_HIGH, 
                    "PA not turned on due to DRC supervision.");
  }

  return;

} /* hdrrmac4drv_sync_start_pa() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_CHECK_SILENCE_INTERVAL                         INTERNAL

DESCRIPTION
  Silence interval (RLSI) processing.  Since PA transitions for RLSI are 
  synchronous to the 26.66 ms frame boundary, function may schedule a PA 
  state change to happen on the next frame boundary.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  The number of subpackets that can be used on the current interlace.

SIDE EFFECTS
  None.
===========================================================================*/
int hdrrmac4drv_check_silence_interval( void )
{
  qword  time_now;               /* Current time */
  int    frames_to_restart;      /* Countdown to end of silence interval */
  uint32 slot;                   /* Current slot (within frame) */
  qword  temp_time_to_rlsi;      /* Temp var for computing time to RLSI. */
  static int pkts_to_rlsi = 4;   /* Subpackets to RLSI */
  int    slots_to_rlsi;          /* Time to RLSI, in slots */
  uint8  ch, idx;                /* Loop counter */
  boolean start_data = FALSE;    /* Flag to start data */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Calculate pkts_to_rlsi only if this is a new subframe */
  if (hdrrmac4drv.prev_subframe.rlsi != hdrrmac4drv.subframe_id)
  {
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* SILENCE INTERVAL                                                      */
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (hdrrmac4drv_common->silence_enabled)
    {

      /* Get frame time and the number of slots into the frame we are at. */
      hdrts_get_cycle_and_slot(time_now, 
                               &slot, 
                               HDRRMAC4DRV_SLOTS_PER_FRAME);

      /* If we are not in the silence interval, check the time to see if it is
         about to start */
      if (hdrrmac4drv.silence_now_flag == FALSE)
      {

        /* Check time. The time indicates when the AT should stop building new
           packets. */
        if ((qw_cmp(time_now, hdrrmac4drv.silence_begin_time) == 0)
            && (slot > HDRRMAC4DRV_SUBFRAME_1_SLOT))
        {
          hdrrmac4drv.silence_now_flag = TRUE;

          HDR_MSG_DRIVERS_4( MSG_LEGACY_HIGH, 
                          "RLSI imminent... (now=%x %x) +%d, %d slots",
                          qw_hi(time_now),
                          qw_lo(time_now),
                          HDRRMAC4DRV_SILENCE_SETUP_FRAMES,
                          slot );

          hdrrmac4drv.silence_count = -HDRRMAC4DRV_SILENCE_SETUP_FRAMES;
        }
        else
        {
          pkts_to_rlsi = 4;
        }

#ifdef FEATURE_HDR_FLEX_CONN
        /* Compute the number of slots until the RLSI */
        qw_sub(temp_time_to_rlsi, hdrrmac4drv.silence_begin_time, time_now);
        slots_to_rlsi = (qw_lo(temp_time_to_rlsi) +
                             HDRRMAC4DRV_SILENCE_SETUP_FRAMES) * 16;

        if ( ( hdrrmac4drv_common->is_flexconn_nv_enabled ) &&
             ( slots_to_rlsi <= ( hdrfmac_get_dsc_length_in_slot()*2 ) ) )
        {
          /* To avoid that base station could not receive dsc in two consecutive 
             dsc cycle, we disable flexconn 2 dsc length before RLSI. In worst case
             we will gurantee that base station receives 1 dsc length of dsc
          */
          if ( hdrrmac4drv_common->flexconn_disabled_before_tx_stop == FALSE )
          {     
            HDR_MSG_DRIVERS_3( MSG_LEGACY_HIGH, 
                          "FLEXCONN: RLSI in 2 dsc length (now=%x %x), %d slots to RLSI",
                          qw_hi(time_now),
                          qw_lo(time_now),
                          slots_to_rlsi );
            hdrrmac4drv_set_flexconn_mode( FALSE );
            hdrrmac4drv_common->flexconn_disabled_before_tx_stop = TRUE;
          }
        }
#endif /* FEATURE_HDR_FLEX_CONN */
      }


      /* We are in, or setting up for, the silence interval */
      if (hdrrmac4drv.silence_now_flag)
      {

        /* Compute the number of slots until the RLSI */
        qw_sub(temp_time_to_rlsi, hdrrmac4drv.silence_begin_time, time_now);

        slots_to_rlsi = (qw_lo(temp_time_to_rlsi) +
          HDRRMAC4DRV_SILENCE_SETUP_FRAMES) * HDRRMAC4DRV_SLOTS_PER_FRAME;

        slots_to_rlsi -= ((slot / HDRRMAC4DRV_SLOTS_PER_SUBFRAME) *
          HDRRMAC4DRV_SLOTS_PER_SUBFRAME) + 
          (hdrrmac4drv.frame_offset % HDRRMAC4DRV_SLOTS_PER_SUBFRAME);

        /* Subtract off slots needed for setup and tx time (2 subframes) */
        slots_to_rlsi -= HDRRMAC4DRV_SLOTS_PER_SUBFRAME * 2;

        /* Find how many subpackets we can send on this interlace. */
        if (slots_to_rlsi >= 0)
        {
          pkts_to_rlsi = 1 + (slots_to_rlsi / 
            (HDRRMAC4DRV_SLOTS_PER_SUBFRAME * 
             HDRSCMRMACCMN_MAX_NUM_INTERLACES));
        }
        else
        {
          pkts_to_rlsi = 0;
        }

        /* Frames to restart PA again */
        frames_to_restart = hdrrmac4drv_common->silence_duration - 
          hdrrmac4drv.silence_count;


        /* Since these are frame counters (not subframe counters), only 
           do counter updates in the last subframe of each frame. */
        if (slot >= HDRRMAC4DRV_SUBFRAME_3_SLOT)
        {
          /* In last subframe before RLSI, trigger PA shutdown at slot 0 */
          if (hdrrmac4drv.silence_count == - 1)
          {
#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
            hdrrmac_stop_flexconn_dtx_monitor( TRUE );
              /* Disable flexconn/dtx before RLSI.*/
#endif /* FEATURE_HDR_FLEX_CONN || FEATURE_HDR_DEVICE_ONLY_DTX */
            /* Set to shutdown on frame boundary */
            hdrrmac4drv_sync_shutdown_pa();

            HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
                             "Silence Interval: Shutting down" );

          }


          /* We are in the frame prior to when the Reverse Link must start
             up again.  Set up the PA to turn on at the frame boundary... */
          if (frames_to_restart == 1)
          {
            hdrrmac4drv_compute_silence_time();
            hdrrmac4drv.silence_now_flag = FALSE;

            HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, 
                               "Silence Interval: Restarting (f=%d)",
                               hdrrmac4drv.frame_offset );

            /* RLSI is about to end. Schedule the PA to turn on at next 
               frame boundary. */

            hdrrmac4drv_sync_start_pa();

            /* Ask for data if atleast one OPEN channel is not in DRC supervision */
            for (idx=0; idx < hdrrmac4drv.num_chans; idx++)
            {
              ch = hdrrmac4drv.open_chans[idx];

              if (hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired == FALSE)
              {
                start_data = TRUE;
                break;
              }
            }

            /* Don't ask for data if in DRC supervision. */
            if (start_data)
            {

              /* Check to see if we can get data yet.  If not, it will start in 
                 the next subframe. Frame offset 0 can never send data in the 
                 subframe after RLSI because of PA warmup, and to keep the 
                 algorithm fair between frame offsets. */
              if ((hdrrmac4drv.frame_offset % HDRRMAC4DRV_SLOTS_PER_SUBFRAME) 
                  != 0 )
              {
                /* PA is available */
                pkts_to_rlsi = 4;
              }
            }

          } /* last subframe before Restart */

          hdrrmac4drv.silence_count++;

        } /* last subframe in a frame */

      } /* we are in, or setting up for, the silence interval */
      else
      {
        /* Failsafe - if somehow the start subframe is skipped, we need to 
           recompute the RLSI.  Silence_begin_time should always be in the 
           future. Comparison is true if time_now > silence_begin-time. */
        if ((qw_cmp(time_now, hdrrmac4drv.silence_begin_time) == 1))
        {
          HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
                          "RLSI start frame skipped. Re-computing...");
  
          hdrrmac4drv_compute_silence_time();
        } 
      }

    } /* if silence enabled */

    hdrrmac4drv.prev_subframe.rlsi = hdrrmac4drv.subframe_id;

  } /* if new subframe */

  if ((hdrrmac4drv.silence_now_flag == FALSE) && (pkts_to_rlsi != 4))
  {
    HDR_MSG_DRIVERS_1( MSG_LEGACY_FATAL,
                       "Not in RLSI but pkts_to_rlsi != 4 (%d)",
                       pkts_to_rlsi );

  }


#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return pkts_to_rlsi;

} /* hdrrmac4drv_check_silence_interval() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_SUBPACKETS_TO_SUSPEND                      INTERNAL

DESCRIPTION
  Returns the number of subpackets available on the current interlace until
  the next scheduled suspend (ie, for tuneaway).
  
DEPENDENCIES
  None.
  
PARAMETERS
  None.
  
RETURN VALUE
  Returns the number of subpackets available on the current interlace until
  the next scheduled suspend (ie, for tuneaway).

SIDE EFFECTS
  None.
===========================================================================*/
int hdrrmac4drv_get_subpackets_to_suspend( void )
{

  static uint16 subpkts_to_suspend = 4; 
    /* Indicates how many subpackets are available on 
       interlace before suspend. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Calculate subpackets_to_suspend only if this is a new subframe */
  if (hdrrmac4drv.prev_subframe.suspend != hdrrmac4drv.subframe_id)
  {
    /* Find how many subpackets we can send on this interlace. */
    if (hdrrmac4drv.subframes_to_suspend > 0)
    {
      /* If there are non-zero subframes available, there is at least one
         subpacket that can be transmitted. The formula below implements the
         following mapping:
         Subframes to suspend  -----> Subpackets to suspend
         0  ------------------------> 0
         1,2,3 ---------------------> 1
         4,5,6 ---------------------> 2
         ...
         */
      subpkts_to_suspend = ((hdrrmac4drv.subframes_to_suspend + 
        HDRSCMRMACCMN_MAX_NUM_INTERLACES - 1) / 
        HDRSCMRMACCMN_MAX_NUM_INTERLACES);

      hdrrmac4drv.subframes_to_suspend--;
    }
    else if (hdrrmac4drv.subframes_to_suspend == 0)
    {
      subpkts_to_suspend = 0;
    }
    else
    {
      subpkts_to_suspend = 4;
    }

    hdrrmac4drv.prev_subframe.suspend = hdrrmac4drv.subframe_id;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return subpkts_to_suspend;

} /* hdrrmac4drv_get_subpackets_to_suspend() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_PA_AVAILABILITY                            EXTERNAL

DESCRIPTION
  Returns 
  - expected PA availability in units of subframes.
  - HDR inactive reason if HDR is to be switched to inactive state.
        
DEPENDENCIES
  None.

PARAMETERS
  hdr_inactive_reason - This is an output parameter.  It is populated with 
    one or more of the following:
    0         => HDR Normal Mode
    Bit 2 = 1 => HDR Inactive on the given channel due to RLSI
  subpackets_until_rlsi - This is an output parameter.  It returns 
    subpackets until rlsi, on one interlace
  subpkts_until_suspend - This is an output parameter.  It returns 
    subpackets until suspend, on one interlace

RETURN VALUE
  Number of subframes PA is available for.

SIDE EFFECTS
  May turn PA on, the case the PA should be available but is not yet on.
  
===========================================================================*/
hdrrmac4drv_pa_availability_enum_type hdrrmac4drv_get_pa_availability
(
  hdrrmac4drv_hdr_mode_enum_type *hdr_inactive_reason,
  int                            *subpackets_until_rlsi,
  int                            *subpackets_until_suspend
)
{
  hdrrmac4drv_pa_availability_enum_type pa_available = 
    HDRRMAC4DRV_PA_NOT_AVAILABLE;
    /* PA available status */

  boolean at_not_in_rlsi;
    /*  TRUE if we are not in silence interval */

  uint16  subpkts_until_rlsi;
    /* Subpackets until rlsi, on one interlace. */

  uint16  subpkts_until_suspend;
    /* Subpackets until suspend, on one interlace. */
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Set hdr inactive reason to normal mode */
  *hdr_inactive_reason = HDRRMAC4DRV_HDR_MODE_NORMAL;

  /* Make sure link is not turned off for RLSI */
  subpkts_until_rlsi = hdrrmac4drv_check_silence_interval();
  at_not_in_rlsi = (subpkts_until_rlsi == 0)?FALSE:TRUE;

  /* Save AT RLSI status.  This is used in exit_drc_supervision(). */
  hdrrmac4drv.at_not_in_rlsi = at_not_in_rlsi;

  /* Set appropriate bit in inactive reason if in RLSI */
  if (!at_not_in_rlsi)
  {
    *hdr_inactive_reason = (hdrrmac4drv_hdr_mode_enum_type) 
      (((uint16) *hdr_inactive_reason) | 
      ((uint16) HDRRMAC4DRV_HDR_MODE_RLSI));
  }

  /* Check for upcoming outages */
  subpkts_until_suspend = hdrrmac4drv_get_subpackets_to_suspend();

  HDR_MSG_DRIVERS_2( MSG_LEGACY_HIGH, 
                     "subpkts_until_rlsi: %d, subpkts_until_suspend: %d ",
                     subpkts_until_rlsi,
                     subpkts_until_suspend );

  /* Fill in caller fields */
  *subpackets_until_suspend = subpkts_until_suspend;
  *subpackets_until_rlsi = subpkts_until_rlsi;

  pa_available = (hdrrmac4drv_pa_availability_enum_type)
    MIN(subpkts_until_rlsi, subpkts_until_suspend);
  
  /* Display MSG when we have a limit on the number of subpackets that 
     can be used. */
  if (pa_available < HDRRMAC4DRV_PA_AVAILABLE)
  {
    HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, 
                       "RTC: Subpackets allowed: %d", pa_available);
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return pa_available;

} /* hdrrmac4drv_get_pa_availability() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_CARRIER_AVAILABILITY                        EXTERNAL

DESCRIPTION
  Populates HDR inactive reason for the specified channel.
        
DEPENDENCIES
  hdrrmac4drv_get_pa_availability() must be called before this function.

PARAMETERS
  phy_chan_id - Physical Channel id
  hdr_inactive_reason - This is an input/output parameter.  It is updated
    with one or more of the following:
    Bit 0 = 1 => HDR Inactive on the given channel due to finger out of lock
    Bit 1 = 1 => HDR Inactive on the given channel due to DRC Supervision

RETURN VALUE
  FALSE - Carrier is unavailable due to fwd link finger out-of-lock or
          DRC supervision.
  TRUE  - Otherwise

SIDE EFFECTS
  May turn PA on, the case the PA should be available but is not yet on.
  
===========================================================================*/
boolean hdrrmac4drv_get_carrier_availability
(
  uint8                           phy_chan_id,
  hdrrmac4drv_hdr_mode_enum_type *hdr_inactive_reason
)
{
  boolean forward_link_locked;
    /* Indicates whether we are receiving the FL normally. */

  boolean carrier_available = TRUE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Verify that the forward link is ok by checking that there is at 
    least one finger in lock. */
  forward_link_locked = (hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
    rmacInfo[phy_chan_id].fingLockedFlag != 0)? TRUE: FALSE;

  /* Set appropriate bit in inactive reason if fwd link is out of lock */
  if (!forward_link_locked)
  {
    *hdr_inactive_reason = (hdrrmac4drv_hdr_mode_enum_type) 
      (((uint16) *hdr_inactive_reason) | 
      ((uint16) HDRRMAC4DRV_HDR_MODE_NO_FINGER_LOCK));


    /* Set carrier available flag to FALSE */
    carrier_available = FALSE;
  }

  /* Set appropriate bit in inactive reason if the channel is in 
     DRC supervision */
  if (hdrrmac4drv.per_carrier_info[phy_chan_id].drc_supervision_expired)
  {
    *hdr_inactive_reason = (hdrrmac4drv_hdr_mode_enum_type) 
      (((uint16) *hdr_inactive_reason) | 
      ((uint16) HDRRMAC4DRV_HDR_MODE_DRC_SUPERVISION));

    /* Set carrier available flag to FALSE */
    carrier_available = FALSE;
  }

  /* Check if data on the carrier is to be disabled due to CPU flow control.
   * When under flow control, data on non-SLP carriers is disabled.  
   * Note: Current SLP carrier reflects SLP carrier selected in the previous
   *       subframe when checked in this function.  In the scenario when SLP
   *       carrier changes when flow control is ON, data will be disallowed
   *       on SLP carrier for one subframe.
   */
  if ( ( hdrrmac4drv.flow_control_on ) && 
       ( phy_chan_id != hdrrmac4drv.slp_carrier.current ) )
  {
    *hdr_inactive_reason = (hdrrmac4drv_hdr_mode_enum_type) 
      (((uint16) *hdr_inactive_reason) | 
      ((uint16) HDRRMAC4DRV_HDR_MODE_CPU_FLOW_CTRL));

    /* Set carrier available flag to FALSE */
    carrier_available = FALSE;
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return carrier_available;

} /* hdrrmac4drv_get_carrier_availability() */



/*===========================================================================
FUNCTION HDRRMAC4DRV_LOG_RL_GAINS                                   INTERNAL

RMG: Log packet under discussion
     Logging interface to be received from FW guys

DESCRIPTION
  This function fills one subframe entry in the reverse gains log packet.

DEPENDENCIES
  Should be called after frame interrupt before 
  hdrrmac4drv_get_prev_tx_status().

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_log_rl_gains( void )
{

  /* Previous subframe logging enabled status */
  static boolean prev_sf_log_enable_status = FALSE;

  /* Current subframe logging enabled status */
  boolean log_enable_status;

  /* Carrier enabled status for logging */
  boolean carrier_enabled;

  /* Convenience pointers */
  hdrlog_rl_gains_pwr_info_type                    *pwr;
  hdrlog_rl_gains_per_chan_subframe_wide_info_type *data;

  /* Last tx'd subframe */
  uint8  subframe_n;

  /* Logged interlace */
  uint8  interlace;                                 

  /* Tx agc status */
  uint16  tx_agc_status;

  /* Tx limit status */
  uint16  tx_limiting_status;

  /* Tx log buffer index */
  uint8   buf_index = 0;

  /* Loop counter */
  uint8  ch;

  /* HSTR corresponding to the first half slot of tx logging period */
  uint16  tx_log_hstr;

  /* Indicates if carrier log info is valid */
  uint16 carrier_buf_valid;
 
  int16  turnaround_db256 = -19456; /* -76 in x256 units */
  int16  nominal_pwr_db256 = 0;

#ifdef FEATURE_FACTORY_TESTMODE
 
  int16 p_max_dsp; /* Available tx power in DSP units */
  int32 p_max_dbm; /* Available tx power in Q.10 dBm units */

#endif /* FEATURE_FACTORY_TESTMODE */
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check whether log packet is enabled */
  log_enable_status = log_status(LOG_HDR_MC_RL_GAINS_C);

  if (prev_sf_log_enable_status != log_enable_status)
  {
    /* Submit log packet whenever log status changes from enabled to
       disabled. */
    if (prev_sf_log_enable_status == TRUE)
    {
      hdrrmac4drv_submit_rl_gain_log();
    }

    /* Update logging status for comparison during in next subframe */
    prev_sf_log_enable_status = log_enable_status;

  }

  /* HDR_MC_RL_GAINS packet enabled, so perform logging. */
  if (log_enable_status)
  {

    /* Obtain turnaround and nominal power values */
#ifdef FEATURE_FACTORY_TESTMODE
    if( ftm_get_mode() == FTM_MODE )
    {
      /* Obtain turnaround info from RF */
#ifdef FEATURE_RF_SVDO_API
      rfm_hdr_get_turnaround_info( RFM_DEVICE_0, 
                                   &turnaround_db256, 
                                   &nominal_pwr_db256 );
#else
      rfm_hdr_get_turnaround_info( &turnaround_db256, 
                                   &nominal_pwr_db256 );
#endif /* FEATURE_RF_SVDO_API */

      hdrrmac4drv.k = (int8) turnaround_db256;

      /* Scale RF returned values to x256 units */
      turnaround_db256 *= 256;
      nominal_pwr_db256 *= 256;
    }
    else
#endif /* FEATURE_FACTORY_TESTMODE */
    {
      /* Obtain turnaround and nominal power values from RMAC 
         common parameters info. */
      if (hdrrmac4drv_common != NULL)
      {
        turnaround_db256 = hdrrmac4drv_common->turnaround_db256;
        nominal_pwr_db256 = hdrrmac4drv_common->nominal_pwr_db256;
      }
    }

    /* Tx buffer index */
    buf_index = HDRLOG_GET_TX_BUF_INDEX();

    /* Tx log HSTR */
    tx_log_hstr = HDRLOG_GET_TX_HSTR( buf_index );

    /* Carrier buffer valid status */
    carrier_buf_valid = HDRLOG_GET_TX_CARRIER_BUFF_VALID( buf_index );


    /* Last tx'd subframe and associated interlace. */
    subframe_n = 
      (hdrrmac4drv.subframe_id + HDRRMAC4DRV_SUBFRAME_N_MAX - 1) % 
      HDRRMAC4DRV_SUBFRAME_N_MAX;

    interlace = subframe_n % HDRSCMRMACCMN_MAX_NUM_INTERLACES;


    /* Populate non-per-channel data entries for this subframe */
    /* Set convenience pointer */
    pwr = &hdrrmac4drv_gain_log_info.pwr_info
      [hdrrmac4drv_gain_log_info.sf_count];

    /* Log last tx'd subframe number */
    pwr->subframe_n = subframe_n;

    /* Log previous subframe PA status */
    pwr->rl_common_info.pa_enabled = 
      hdrrmac4drv_gain_log_info.prev_pa_enabled;

    /* Grab tx agc status and update log fields */
    tx_limiting_status = 
      HDRLOG_GET_TX_LIMITING_LOG( buf_index );

    /* Tx throttle flag.
       1 => TX AGC is reducing the power because total 
            power required exceeds available transmit power. */
    pwr->rl_common_info.tx_throttle =  
      (tx_limiting_status & HDRLOG_TX_LIMITING_LOG_THROTTLE_M) >> 
      HDRLOG_TX_LIMITING_LOG_THROTTLE_S;

    /* Tx MAX power flag.
       1 => TX AGC has ratcheted the RPC up command. */
    pwr->rl_common_info.tx_max_power = 
      (tx_limiting_status & HDRLOG_TX_LIMITING_LOG_MAX_POWER_M) >> 
      HDRLOG_TX_LIMITING_LOG_MAX_POWER_S;

    /* Tx MIN power flag.
       1 => TX AGC has ratcheted the RPC down command. */
    pwr->rl_common_info.tx_min_power =
      (tx_limiting_status & HDRLOG_TX_LIMITING_LOG_MIN_POWER_M) >> 
      HDRLOG_TX_LIMITING_LOG_MIN_POWER_S;

    /* Backoff in Q.2 dB */
    pwr->backoff = hdrrmac4drv_gain_log_info.backoff;

    pwr->total_tx_power = 
      (int2)((-(((HDRLOG_GET_TOTAL_POWER_LOG(buf_index) * 256)
             / HDRDSP_AGC_COUNTS_PER_DB)
             + (HDRDSP_AGC_RX_OFFSET_256 - 
                turnaround_db256))));
      /* Tx power is read in second half-slot */

#ifdef FEATURE_FACTORY_TESTMODE
    if( ftm_get_mode() == FTM_MODE )
    {
      /* In FTM mode, headroom and carrier management sections of code, which 
         populate max available tx power, are not executed. Hence available 
         tx power needs to be computed within this function when in FTM 
         mode. */

      p_max_dsp = HDRMDSP_GET_MAX_TX_LIMIT( hdrrmac4drv.in_funnel_mode );

      if ( p_max_dsp >= 0 )
      {
        p_max_dbm = (hdrrmac4drv.k << 10) + 65331 - 
          ( ( ( p_max_dsp * 
          819 ) + (((int16)1) << 8) ) >> 9 );
      }
      else
      {
        p_max_dbm = (hdrrmac4drv.k << 10) + 65331 - 
          ( ( ( p_max_dsp * 
          819 ) - (((int16)1) << 8) ) >> 9 );
      }

      hdrrmac4drv_gain_log_info.max_avail_tx_pwr = (p_max_dbm+255) >> 8;

    }
#endif /* FEATURE_FACTORY_TESTMODE */

    pwr->max_avail_tx_power = hdrrmac4drv_gain_log_info.max_avail_tx_pwr;

    /* Populate per-channel data entries for this subframe */
    for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      /* Set convenience pointer */
      data = &hdrrmac4drv_gain_log_info.log[ch]
        [hdrrmac4drv_gain_log_info.sf_count];

      /* Record last tx'd subframe. */
      data->subframe_n = subframe_n;

      /* Get prev sf carrier enable status.  Include setup state. */
      carrier_enabled = 
        ( ( ( ( ((uint16) hdrrmac4drv.per_carrier_info[ch].carrier_state) & 
        ((uint16) HDRRMAC4DRV_CH_ST_MAJOR_MASK) ) == 
        ((uint16)HDRRMAC4DRV_CH_ST_OPEN) ) ||
        ( ((uint16)hdrrmac4drv.per_carrier_info[ch].carrier_state) == 
        ((uint16) HDRRMAC4DRV_CH_ST_SETUP) ) ) ? TRUE : FALSE );

      /* Mark the carrier as disabled if it is in DRC supervision */
      carrier_enabled = carrier_enabled 
        && (!hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired );

      /* Store prev sf carrier enable status, considering PA status too */
      data->rl_info.carrier_enabled =  carrier_enabled 
                         && hdrrmac4drv_gain_log_info.prev_pa_enabled;

      if (carrier_enabled)
      {
        if (carrier_buf_valid & (1<<ch))
        {

          /* Tag the channel for logging */
          hdrrmac4drv_gain_log_info.enable_channel_logging[ch] = 
            TRUE;

          /* Store RRI (aka PacketSizeIndex), transmission mode, 
             and subpacketN */
          data->rl_info.rri = 
            hdrrmac4drv.per_carrier_info[ch].interlace_history[interlace].rri;

          data->rl_info.transmission_mode = 
            hdrrmac4drv.per_carrier_info[ch].interlace_history[interlace].tm;

          data->rl_info.sp_n = 
            hdrrmac4drv.per_carrier_info[ch].interlace_history[interlace].
            subpacket;

          /* Grab tx agc status and update log fields */
          tx_agc_status = 
            HDRLOG_GET_TX_AGC_STATUS_LOG_MC( buf_index, ch, 1 );

          /* RPC */
          data->rl_info.rpc =          
            (tx_agc_status & HDRLOG_TX_AGC_STATUS_LOG_RPC_M) >> 
            HDRLOG_TX_AGC_STATUS_LOG_RPC_S;

          /* - - Record gains from last frame. - - */

          /* Linear pilot power in halfslot 0 of subframe */
          data->pilot_hs0 = 
            HDRLOG_GET_PILOT_DIGITAL_GAIN_LOG_MC( buf_index, ch, 0 );

          /* Linear ACK channel power in halfslot 0 of subframe */
          data->ack_hs0 = 
            HDRLOG_GET_ACK_DIGITAL_GAIN_LOG_MC( buf_index, ch, 0 );

          /* Linear pilot power in halfslot 1 of subframe */
          data->pilot_hs1 = 
            HDRLOG_GET_PILOT_DIGITAL_GAIN_LOG_MC( buf_index, ch, 1 );

          /* Linear RRI channel power in halfslot 1 of subframe */
          data->rri_hs1 = 
            HDRLOG_GET_RRI_DIGITAL_GAIN_LOG_MC( buf_index, ch, 1 );

          /* Linear DSC channel power in halfslot 1 of subframe */
          data->dsc_hs1 = 
            HDRLOG_GET_DSC_DIGITAL_GAIN_LOG_MC( buf_index, ch, 1 );

          /* Linear DRC channel power in halfslot 1 of subframe */
          data->drc_hs1 = 
            HDRLOG_GET_DRC_DIGITAL_GAIN_LOG_MC( buf_index, ch, 1 );

            /* Linear data0 channel power in halfslot 1 of subframe */
          data->data_hs1 = 
            HDRLOG_GET_DATA0_DIGITAL_GAIN_LOG_MC( buf_index, ch, 1 );

          /* Linear auxiliary pilot gain in halfslot 1 of subframe */
          data->aux_hs1 = 
            HDRLOG_GET_AUX_PILOT_DIGITAL_GAIN_LOG_MC( buf_index, ch, 1 );

          /* BetaP gain in halfslot 1 of subframe */
          data->betap_hs1 = 
            HDRLOG_GET_BETA_P_DIGITAL_GAIN_LOG_MC( buf_index, ch, 1 );

          /* Fill in AGC values */
          data->tx_agc.tx_open_loop = 
            (int2)(-(((HDRLOG_GET_OPEN_LOOP_POWER_LOG_MC(buf_index, ch) * 256)
                     / HDRDSP_AGC_COUNTS_PER_DB)
                     + (HDRDSP_AGC_RX_OFFSET_256 - 
                        nominal_pwr_db256)));

          data->tx_agc.tx_total = 
            (int2)((-(((HDRLOG_GET_TOTAL_POWER_LOG_MC(buf_index, ch) * 256)
                   / HDRDSP_AGC_COUNTS_PER_DB)
                   + (HDRDSP_AGC_RX_OFFSET_256 - 
                      turnaround_db256))));

          data->tx_agc.tx_pilot = 
            (int2)((-(((HDRLOG_GET_PILOT_POWER_LOG_MC(buf_index, ch) * 256)
                   / HDRDSP_AGC_COUNTS_PER_DB)
                   + (HDRDSP_AGC_RX_OFFSET_256 - 
                      turnaround_db256))));

          data->tx_agc.tx_adjust = 
            (int2)((HDRLOG_GET_CLOSED_LOOP_POWER_LOG_MC(buf_index, ch) / 2 )
             - (nominal_pwr_db256 -
                turnaround_db256));

        }
        else
        {
          HDR_MSG_DRIVERS_1( MSG_LEGACY_ERROR,
            "MDSP carrier logging information invalid! %d",
            carrier_buf_valid );

        }

      } /* if the channel is enabled */

    } /* for all channels */


    if (tx_log_hstr != HDRLOG_GET_TX_HSTR( buf_index ))
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_ERROR,
        "RLGains information overwritten by FW while it was being read!" );
    }


    /* Update subframe count and submit log if needed. */
    hdrrmac4drv_gain_log_info.sf_count++;

    if ( hdrrmac4drv_gain_log_info.sf_count == 
         HDRLOG_MAX_SUBFRAMES_PER_MC_RL_GAINS_PKT )
    {
      /* Submit log */
      hdrrmac4drv_submit_rl_gain_log();
    }

  }  /* if logging is enabled */

  /* Sample PA state for next log. */
  hdrrmac4drv_gain_log_info.prev_pa_enabled = hdrmod_pa_is_on();

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_log_rl_gains() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_LOG_ARQ_STATUS                                INTERNAL

DESCRIPTION
  This function fills one subframe entry in the reverse arq status log packet.

DEPENDENCIES
  Should be called after frame interrupt before 
  hdrrmac4drv_get_prev_tx_status().

PARAMETERS
  carrier info - Pointer to struct containing 
                 - per-channel ARQ status info
                 - per-channel Packet Encode status info

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_log_arq_status
( 
  hdrrmac4drv_carrier_status_info_type *carrier_info
)
{
  /* Previous subframe logging enabled status */
  static boolean prev_sf_log_enable_status = FALSE;

  /* Current subframe logging enabled status */
  boolean log_enable_status;

  /* Logged interlace */
  uint16  interlace;                                 

  /* Convenience pointer */
  hdrlog_rl_arq_status_subframe_wide_info_type * data; 

  /* ARQ */
  uint8 arq;

  /* Loop counters */
  uint8  ch, idx;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check whether log packet is enabled. */
  log_enable_status = log_status(LOG_HDR_MC_RL_ARQ_STATUS_C);

  if (prev_sf_log_enable_status != log_enable_status)
  {
    /* Submit log packet whenever log status changes from enabled to
       disabled. */
    if (prev_sf_log_enable_status == TRUE)
    {
      hdrrmac4drv_submit_arq_status_log();
    }

    /* Update logging status for comparison during in next subframe */
    prev_sf_log_enable_status = log_enable_status;

  }

  /* HDR_MC_ARQ_STATUS packet enabled, so perform logging. */
  if (log_enable_status)
  {
    /* Error catching */
    ASSERT( hdrrmac4drv_arq_status_log_info.sf_count < 
      HDRLOG_MAX_SUBFRAMES_PER_REVB_RL_ARQ_PKT );

    /* for all carriers
        if the carrier is enabled 
          - log per-subframe info 
          - set enable_channel_logging flag for the channel
        else
          - fill in invalid values for the carrier for this subframe */

    for (idx = 0; idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
    {
      ch = hdrrmac4drv.open_chans[idx];

      /* Populate channel data entries for this subframe */

      data = &hdrrmac4drv_arq_status_log_info.log[ch]
        [hdrrmac4drv_arq_status_log_info.sf_count];

      /* Record subframe_n and interlace */
      data->subframe_n = (hdrrmac4drv.subframe_id + 
                          HDRRMAC4DRV_SUBFRAME_N_MAX - 1) % 
                          HDRRMAC4DRV_SUBFRAME_N_MAX;
      interlace = data->subframe_n % HDRSCMRMACCMN_MAX_NUM_INTERLACES;

      /* Interlace */
      data->packet_info.interlace = interlace;

      if ( idx < hdrrmac4drv.num_chans )
      {
        /* OPEN channel => Tag the channel for logging */
        hdrrmac4drv_arq_status_log_info.enable_channel_logging[ch] = 
          TRUE;

        /* Packet ID */
        data->packet_id = hdrrmac4drv.per_carrier_info[ch].
          interlace_history[interlace].packet_id;

        /* Packet Info */

        /* RRI */
        data->packet_info.rri = hdrrmac4drv.per_carrier_info[ch].
          interlace_history[interlace].rri;

        /* Aux pilot enabled */
        if ( hdrrmac4drv.per_carrier_info[ch].
             interlace_history[interlace].rri >= 
             (uint8) hdrrmac4drv.aux_pilot_min_payload )
        {
          data->packet_info.aux_pilot_enabled = TRUE;
        }
        else
        {
          data->packet_info.aux_pilot_enabled = FALSE;
        }

        /* Transmission mode */
        data->packet_info.transmission_mode = hdrrmac4drv.per_carrier_info
          [ch].interlace_history[interlace].tm; 

        /* Subpacket number */
        data->packet_info.sp_n = hdrrmac4drv.per_carrier_info[ch].
          interlace_history[interlace].subpacket; 


        /* ARQ Status */
        /* Packet Status */
        data->arq_status.packet_status = carrier_info[ch].encode_status;

        /* P-ARQ Ack/NaK */ /* RMG: How did this work in RevA? */
        arq = (carrier_info[ch].arq_status & HDRMDSP_RL_ARQ_STATUS_M) >>
           HDRMDSP_RL_ARQ_STATUS_S;
        data->arq_status.parq = arq & 0x1;
        data->arq_status.harq = (arq & 0x6)>> 0x1;

      }
      else
      {
        /* Channel not enabled in this subframe */

        /* Packet ID */
        data->packet_id = HDRRMAC4DRV_INVALID_PACKET;

        /* Packet Info */

        /* RRI */
        data->packet_info.rri = (uint16) HDRRMACCMN_MAX_PAYLOADS;

        /* Aux pilot enabled */
        data->packet_info.aux_pilot_enabled = FALSE;

        /* Transmission mode */
        data->packet_info.transmission_mode = (uint16) 
          HDRRMACCMN_TX_MODE_HI_CAP; 

        /* Subpacket number */
        data->packet_info.sp_n = 0; 

        /* ARQ Status */
        /* Packet Status */
        data->arq_status.packet_status = (uint8) HDRMDSP_RL_STATUS_IDLE;

        /* P-ARQ Ack/NaK */
        data->arq_status.parq = 0;
        data->arq_status.harq = 0;
      }

    } /* for all channels */

    /* Update subframe count and submit log if needed. */
    hdrrmac4drv_arq_status_log_info.sf_count++;
    if (hdrrmac4drv_arq_status_log_info.sf_count == 
        HDRLOG_MAX_SUBFRAMES_PER_REVB_RL_ARQ_PKT)
    {
      /* Submit log */
      hdrrmac4drv_submit_arq_status_log();
    }

  } /* Log packet enabled */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_log_arq_status() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_NEXT_SUBFRAME_ID                           INTERNAL

DESCRIPTION
  Returns the subframe number for the next subframe.  
  
  
  n = subframe number

    (Slot within subframe...)
     0   1   2   3    0   1   2   3    0   1   2   3
  ----------------------------------------------------
  |            xxxx|            xxxx|                |
  |      n-2   xxxx|      n-1   xxxx|        n       |
  |            xxxx|            xxxx|                |
  ----------------------------------------------------
               |   |<-- 4 slots --> |
               |<-- 4 slots --> |
               A                B 
               
  At point A: ARQ for frame N-4 is available.
              Encode starts for N-1.               
  At point B: ARQ for frame N-3 is available.
              Encode starts for N.
              
  The window for AMSS to compute and load data for subframe
  N is between A and B.  Note that this window of time is
  offset from the actual frame by 1 slot. This is for two 
  reasons: 1) We may begin computing subframe N as soon
  as we know whether the last subframe was sent or not (point A);
  and 2) the deadline to finish is when the encoder must begin
  encoding (point B).

  This function, when called between points A and B, will return
  n as the next subframe id.
    
DEPENDENCIES
  hdrrmac4drv_set_frame_offset() must have already been called.
  
PARAMETERS
  None.  

RETURN VALUE
  The next subframe id.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4drv_get_next_subframe_id( void )
{            

  uint32 mod_time = 0, mod_hs_time;
    /* Current modulator time, and modulator time in halfslots */

  uint16 next_pkt_hs_time;
    /* Next subframe time in halfslots */

  uint8 subframe;
    /* Number associated with next subframe (subframe id) */
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Compute subframe id for a packet loaded in current subframe (to be sent 
     next subframe) */
  mod_time = hdrts_get_tx_chip_time();
  
  /* Shift by 10 (div 1024) to convert to halfslots */
  mod_hs_time = mod_time >> 10;
  next_pkt_hs_time = (mod_hs_time & 0x0FFF) + 
    hdrrmac4drv.subframe_halfslot_offset;

  /* Where we are in the window for creating and loading above subframe.
     Value should range from 0 - 7.  
     Used only for time checking (and profiling info) */
  hdrrmac4drv.halfslot_start = next_pkt_hs_time % 
    HDRRMAC4DRV_HALFSLOTS_PER_SUBFRAME;

  /* Gives the subframe we are interested in */
  subframe = (((next_pkt_hs_time / HDRRMAC4DRV_HALFSLOTS_PER_SUBFRAME) + 
    (HDRRMAC4DRV_SUBFRAME_N_MAX - hdrrmac4drv.subframe_offset)) %
    HDRRMAC4DRV_SUBFRAME_N_MAX);

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return subframe;

} /* hdrrmac4drv_get_next_subframe_id() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_LOG_PKT_STATUS                                INTERNAL

DESCRIPTION
  This function fills packet entries in the RL pkt status log packet.

DEPENDENCIES
  None.
  
PARAMETERS
  carrier info - Pointer to struct containing 
                 - per-channel packet status information valid flag
                 - per-channel packet status info
                 - per-channel MARQ status information valid flag
                 - per-channel MARQ packet status info
                 - per-channel PARQ NAK received flag
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_log_pkt_status
(
  hdrrmac4drv_carrier_status_info_type *carrier_info
)
{
  /* Previous subframe logging enabled status */
  static boolean prev_sf_log_enable_status = FALSE;

  /* Current subframe logging enabled status */
  boolean log_enable_status;

  /* Whether any packets are active on any interlace. */
  boolean packets_active = FALSE;

  /* Set when log packet enable status changes from enabled
     to disabled.  This is one of the triggers in log packet submission */
  boolean log_enable_status_changed = FALSE;

  /* Transition point associated with packet size */
  uint8 transition_point;   

  /* Shorthand pointers */
  hdrlog_rl_pkt_status_packet_wide_info_type * data; 
  hdrrmac4drv_packet_info_type               * 
    packet_status[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS][2];
  hdrrmaccmn_tx_t2p_info_type                * rate_info;

  /* Total number of packets accumulated */
  uint8 total_packet_count = 0;

  /* Packets to log in this subframe */
  uint8 packets_to_log[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0,0,0};

  /* Loop counters */
  uint8  ch, sp_id, pkt;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Error catching */
  ASSERT(carrier_info != NULL);

  /* Clear packet status - Lint fix. */
  memset( (void*)&(packet_status), 0,  sizeof(packet_status) );

  /* Check whether log packet is enabled. */
  log_enable_status = log_status(LOG_HDR_MC_RL_PKT_STATUS_C);

  if (prev_sf_log_enable_status != log_enable_status)
  {
    /* Set flag if the log status changed from enabled to
       disabled. */
    if (prev_sf_log_enable_status == TRUE)
    {
      log_enable_status_changed = TRUE;
    }

    /* Update logging status for comparison during in next subframe */
    prev_sf_log_enable_status = log_enable_status;

  }

  /* Check if packets are active on any interlace. */    
  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv_packets_are_active(ch))
    {
      packets_active = TRUE;

      break;
    }
  }

  /* Submit pkt status log if 
     (a) Log enable status changed from enabled to disabled OR
     (b) there are no packets in progress (all interlaces are empty). */
  if ((log_enable_status_changed) ||
      ((!hdrrmac4drv_marq_status_is_pending()) && (!packets_active)))
  {
    hdrrmac4drv_submit_pkt_status_log();
  }

  /* HDR_MC_RL_PKT_STATUS packet enabled, so perform logging. */
  if (log_enable_status)
  {
    for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      total_packet_count += 
        hdrrmac4drv_pkt_status_log_info.packet_count[ch];

      if (carrier_info[ch].harq_info_valid)
      {
        /* Store pointer to packet info */
        packet_status[ch][packets_to_log[ch]] = carrier_info[ch].packet_info;

        /* Increment packets to log count for the carrier */
        packets_to_log[ch]++;
      }

      if (carrier_info[ch].marq_info_valid)
      {
        /* Store pointer to marq info */
        packet_status[ch][packets_to_log[ch]] = carrier_info[ch].marq_info;

        /* Increment packets to log count for the carrier */
        packets_to_log[ch]++;
      }
    }

    /* Error catching */
    ASSERT( total_packet_count < 
      HDRLOG_MAX_ENTRIES_PER_MC_RL_PKT_STATUS_PKT );

    /* for all carriers
        - log per-packet info 
        - increment packet_count for the carrier */
    for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      for (pkt = 0; pkt < packets_to_log[ch]; pkt++)
      {
        /* Set up convenience pointer */
        data = &hdrrmac4drv_pkt_status_log_info.log[ch]
          [hdrrmac4drv_pkt_status_log_info.packet_count[ch]];

        /* Packet ID */
        data->packet_id = packet_status[ch][pkt]->packet_id;

        /* Number of non-padding data bytes */
        data->data_bytes_sent = packet_status[ch][pkt]->data_length;

        /* Packet Info */
        /* Interlace */
        data->packet_info.interlace = packet_status[ch][pkt]->packet_id % 
          HDRSCMRMACCMN_MAX_NUM_INTERLACES;

        /* RRI */
        data->packet_info.rri = packet_status[ch][pkt]->rri; 

        /* Aux Pilot Enabled */
        if ( packet_status[ch][pkt]->rri >= 
           (uint8) hdrrmac4drv.aux_pilot_min_payload )
        {
          data->packet_info.aux_pilot_enabled = TRUE;
        }
        else
        {
          data->packet_info.aux_pilot_enabled = FALSE;
        }

        /* Transmission mode */
        data->packet_info.transmission_mode = packet_status[ch][pkt]->tm;

        /* Subpacket number */
        data->packet_info.subpackets = packet_status[ch][pkt]->subpacket;

        /* PARQ NAK received flag */
        data->packet_info.parq_nak_received = carrier_info[ch].parq_nak_info;

        /* Figure out packet gains */
        rate_info = &hdrrmac4drv_config->t2p_info[packet_status[ch][pkt]->tm].
          tx_t2p_info[packet_status[ch][pkt]->rri];

        transition_point = rate_info->trans_point;

        /* Copy for each subframe */
        for (sp_id=0; sp_id < HDRRMACCMN_MAX_NUM_SUBPACKETS; sp_id++)
        {
          data->subframe_t2p[sp_id] = (uint8)
            (((int16) rate_info->data_t2p_db[sp_id]) + 
             hdrrmac4drv.per_carrier_info[ch].best_sector_delta_t2p );

          data->rri_gain[sp_id] = (int8)
            (hdrrmac4drv.rri_to_pilot[transition_point].subpacket_gain[sp_id]);
        }

        /* Update packet count and submit log if needed. */
        hdrrmac4drv_pkt_status_log_info.packet_count[ch]++;
        total_packet_count++;

        if (total_packet_count == HDRLOG_MAX_ENTRIES_PER_MC_RL_PKT_STATUS_PKT)
        {
          hdrrmac4drv_submit_pkt_status_log();

          total_packet_count = 0;
        }

      } /* for packets_to_log packets */

    } /* for all channels */

  } /* Log packet enabled */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_log_pkt_status() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_PREV_TX_STATUS                             EXTERNAL

DESCRIPTION
  Retrieves status of last packet to be loaded into the encoder.  The
  channel packet status information is arranged by channel CAPUT priority 
  order; the first being the highest priority channel.  Only the first
  num_chans entries in the list are valid, where num_chans is the number
  of active channels.

DEPENDENCIES
  None.

PARAMETERS
  enc_window_missed_ptr
                - Pointer to encoder window missed flag.  It is set if 
                  one of the following is true:
                  Interrupt for the current subframe arrived late
                  Previous subframe was not accepted because SW was late
                  Previous subframe was not accepted because FW was late
  packet_status - Holds information on packets loaded during previous 
                  subframe and are currently being transmitted.
  
RETURN VALUE
  Return Code: 0 == error has occuring and caller should abort processing
                    the subframe.
               1 == success.

SIDE EFFECTS
  Initializes hdrrmac4drv.subframe_id, and interlace on first time through.
  
===========================================================================*/
boolean hdrrmac4drv_get_prev_tx_status
(
  boolean                    * enc_window_missed_ptr,
  hdrrmac4drv_tx_status_type * tx_status_ptr
)
{

  /* Carrier status info */
  hdrrmac4drv_carrier_status_info_type 
    carrier_status_info[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Interlace for which the latest ARQ bit is for */
  uint8 arq_interlace;
    
  /* Previous interlace, also the interlace the MARQ is for */
  uint8 prev_interlace;

  /* Loop counter for interlace loop */
  uint8 interlace;

  /* Subframe number of the packet to be loaded this subframe for 
     transmission next subframe. (thus, the actual value corresponds to 
     next subframe */
  uint8 subframe_to_load, interlace_to_load;

  /* Number of subframes (interrupts) missed -- shouldn't happen */
  uint8 missed_subframes = 0;

  /* Whether any packets are active on any interlace. */
  boolean packets_active = FALSE;

  /* Channels for which entries are filled in encode status processing */
  boolean ch_entries_filled[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
    {FALSE, FALSE, FALSE};

  /* List of channels for which encode status needs to be reported to RMAC */
  uint8 encode_status_ch_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Number of channels for which previous packet encode status is reported */
  uint8 num_chans_encode_status;

  /* Shorthand pointer */
  hdrrmac4drv_packet_info_type * packet_info;

  /* Loop counters */
  uint8  ch, idx, phy_chan_id, num_marq_chans = 0;

#ifdef FEATURE_FACTORY_TESTMODE
  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;
#endif /* FEATURE_FACTORY_TESTMODE */

  /* Short hand pointer to shared memory location with RL packet status
     information*/
  volatile hdrfw_rl_packet_rd_t * pkt_status_ptr;

  /* Flag to indicate that subframes greater than missed subframe threshold
     are misssed */
  boolean subframes_missed = FALSE;

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
  /* Temporary storage for RRI history */
  uint8 rri_back_1, rri_back_2;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
  /* Intermediate variable for calculating not data limited throughput */
  uint64 not_data_lmtd_tput;

  /* Intermediate variable for calculating current throughput */
  uint64 curr_tput;

  /* Number of bits acknowledged */
  uint32 bits_ackd;

  /* Subframe throughput calculated from number of bits acknowledged in this
     subframe */
  uint64 sf_tput;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Turn PA on if scheduled */
  if ( hdrrmac4drv.subframes_till_pa_enable > 0 )
  {
    hdrrmac4drv.subframes_till_pa_enable--;

    HDR_MSG_DRIVERS_1( MSG_LEGACY_MED,
     "Subframes to PA enable: %u",
     hdrrmac4drv.subframes_till_pa_enable );

    if (hdrrmac4drv.subframes_till_pa_enable == 0)
    {
      hdrrmac4drv_pa_on(TRUE, 0);
    }
  }

  /* If subframes to suspend are to be computed, do so. */
  if ( hdrrmac4drv.compute_subframes_to_suspend )
  {
    /* Current time in frames and slots */
    qword  current_time_frames;
    uint32 current_slot_in_frame;
    uint32 current_time_slots;

    /* Current slot within subframe */
    uint8  curr_slot_within_sf;

    /* Number of slots available till suspend */
    uint32 slots_till_suspend;

    /* Find out current slot */

    /* Get current time in frames and slots within current frame */
    hdrts_get_cycle_and_slot( current_time_frames, &current_slot_in_frame, 
                              HDRTS_SLOTS_PER_FRAME);

    /* Convert current time in frames to slots */
    qw_shift( current_time_frames, 4 );

    /* Compute current time in slots */
    current_time_slots = qw_lo( current_time_frames ) + current_slot_in_frame;

    /* Current slot within subframe
       = ( current slot + 4 - subframe offset ) % 4 */
    curr_slot_within_sf = ( current_time_slots + HDRRMAC4DRV_SLOTS_PER_SUBFRAME - 
      (hdrrmac4drv.frame_offset % HDRRMAC4DRV_SLOTS_PER_SUBFRAME) ) %
      HDRRMAC4DRV_SLOTS_PER_SUBFRAME;

    /* Calculate slots available till suspend */
    slots_till_suspend = (hdrrmac4drv.suspend_slot - current_time_slots) + 1;

    /* Subframes to suspend
       = full subframes from slots to suspend +
         a subframe from remaing slots based on offset adjusted slot within SF
       = slots_till_suspend / 4 +
         ( slots_till_suspend % 4 + curr_slot_within_sf ) / 4 */
    hdrrmac4drv.subframes_to_suspend = ( slots_till_suspend >> 2 ) + 
      ( ( ( slots_till_suspend % 4 ) + curr_slot_within_sf ) >> 2 );

    /* Subtract a subframe to arrive at the number of subframes available to 
       load new packets before suspend */
    hdrrmac4drv.subframes_to_suspend--;

    /* Reset flag */
    hdrrmac4drv.compute_subframes_to_suspend = FALSE;
  }

  /* Make LINT happy */
  ASSERT(tx_status_ptr != NULL);

  /* Clear the contents of carrier status info */
  memset((void*)&carrier_status_info[0], 0, 
    HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS * 
    sizeof(hdrrmac4drv_carrier_status_info_type));

  /* Reset encoder window missed flag */
  *enc_window_missed_ptr = FALSE;

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
  /* Reset RL packet scheduled flag */
  hdrrmac4drv.rl_packet_scheduled = FALSE;
#endif /* defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/

  /* Rewrite margin parameters */
  hdrrmac4drv_set_margin_parameters();

  /* Take care of logging rl gains */
  hdrrmac4drv_log_rl_gains();

#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() == FTM_MODE )
  {
    /* Special single fwd link mode for this so that we do not carry over 
       the gains when three forward links are actually present */
    if ( hdrhitmsg_get_rmac_params_ptr()->revb_single_fwd_link_mode )
    {
      /* FTM in single forward link mode */

      /* Initialize firmware command for open loop override */
      memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
      msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
        HDR_FW_RMAC_OPENLOOP_OVERRIDE_CMD );

      /* Copy over open loop gains of first carrier to the other two 
         carriers */
      fw_cmd.rmac_openloop_override.openLoopOverride[1].fieldValid = 1;
      fw_cmd.rmac_openloop_override.openLoopOverride[1].carrier = 
        HDRMDSP_DEMOD_CARRIER_0;
      fw_cmd.rmac_openloop_override.openLoopOverride[1].enableOverride = 1;

      fw_cmd.rmac_openloop_override.openLoopOverride[2].fieldValid = 1;
      fw_cmd.rmac_openloop_override.openLoopOverride[2].carrier = 
        HDRMDSP_DEMOD_CARRIER_0;
      fw_cmd.rmac_openloop_override.openLoopOverride[2].enableOverride = 1;

      /* Send open loop override command to firmware */
      estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
      ASSERT ( estatus == E_SUCCESS );

      if (hdrrmac4drv.rpc_disabled == FALSE)
      {
        /* RPC enabled in FTM single forward link mode.
           Here RPC on the first carrier controls closed loop power of 
           other carriers. */

        /* Initialize firmware command for closed loop override */
        memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
        msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, 
          HDR_FW_RMAC_MISC_CFG_CMD );
        fw_cmd.rmac_misc_cfg.fieldMask.gainAdjOverride_Valid = 1;
        fw_cmd.rmac_misc_cfg.fieldMask.enableDrcSupervision_Valid = 1;

        /* Set carrier 1 and carrier 2 bits in carrier Mask */
        fw_cmd.rmac_misc_cfg.carrierMask = (1 << 1) | (1 << 2);

        /* Copy over closed loop gains of the first carrier to other 
           carriers */
        fw_cmd.rmac_misc_cfg.C[1].gainAdjOverride.carrier = 
          HDRMDSP_DEMOD_CARRIER_0;
        fw_cmd.rmac_misc_cfg.C[1].gainAdjOverride.
          useGainAdjVal = 1;

        fw_cmd.rmac_misc_cfg.C[2].gainAdjOverride.carrier = 
          HDRMDSP_DEMOD_CARRIER_0;
        fw_cmd.rmac_misc_cfg.C[2].gainAdjOverride.
          useGainAdjVal = 1;

        /* Disable DRC supervision on carriers without FL */
        fw_cmd.rmac_misc_cfg.C[1].enableDrcSupervision = 0;
        fw_cmd.rmac_misc_cfg.C[2].enableDrcSupervision = 0;

        /* Send closed loop override command to firmware */
        estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
        ASSERT ( estatus == E_SUCCESS );
      }
    }
  }
#endif /* FEATURE_FACTORY_TESTMODE */

  /* Get packet status and subframe id. The subframe id is for the next 
     packet to be loaded into the encoder */
  subframe_to_load = hdrrmac4drv_get_next_subframe_id();

  /* Interested interlace, this is the interlace to be loaded. */
  interlace_to_load = subframe_to_load % HDRSCMRMACCMN_MAX_NUM_INTERLACES;

  /* Check if we are too late to even bother with trying to build a frame. */
  if (hdrrmac4drv.halfslot_start > HDRRMAC4DRV_HALFSLOT_DEADLINE)
  {
    HDR_MSG_DRIVERS_1( MSG_LEGACY_ERROR,
                       "RTC: Signal arrived too late (hs %d)",
                       hdrrmac4drv.halfslot_start);

    /* Yep, it's too late.  Signal timing error to keep this frame idle. */
    hdrrmac4drv.timing_error = TRUE;

    /* Set encoder window missed flag */
    *enc_window_missed_ptr = TRUE;
  }
  else
  {
    /* Reset error flag to FALSE for now if we're ok. */
    hdrrmac4drv.timing_error = FALSE;
  }

  /* Check if subframe id needs to be overwritten due to any of the following:
   *  - Subframe miss
   *  - RMAC has just started and subframe id is invalid
   */
  if ((subframe_to_load != 
       ((hdrrmac4drv.subframe_id + 1) % HDRRMAC4DRV_SUBFRAME_N_MAX)) ||
      (hdrrmac4drv.subframe_id == HDRRMAC4DRV_INVALID_PACKET))
  {
    /* Complain if this isn't the first time through... */
    if (hdrrmac4drv.subframe_id != HDRRMAC4DRV_INVALID_PACKET)
    {
      /* Missed interrupt...complain! */
      missed_subframes = ((subframe_to_load + HDRRMAC4DRV_SUBFRAME_N_MAX ) 
                          - hdrrmac4drv.subframe_id)  
                          % HDRRMAC4DRV_SUBFRAME_N_MAX;

      /* This is the second time we've run in the last subframe!
         Return with error */
      if (missed_subframes == 0)
      {
        HDR_MSG_DRIVERS_1( MSG_LEGACY_ERROR,
          "RTC: Subframe %d already processed, abort.",
          subframe_to_load);

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

        return FALSE;
      }
      else
      {
        HDR_MSG_DRIVERS_4( MSG_LEGACY_ERROR,
          "RTC: Missed subframe(s)! Expected %d, got %d (missed %d, hs %d)",
          hdrrmac4drv.subframe_id, subframe_to_load, missed_subframes,
          hdrrmac4drv.halfslot_start);

        if (hdrrmac4drv_enable_missed_subframes_ramp_up)
        {
          if (missed_subframes >= hdrrmac4drv_missed_subframes_threshold)
          {
            subframes_missed = TRUE;
          }
        }
      }
    }

    /* Compute values, either for first time or after missed interrupt */
    hdrrmac4drv.subframe_id = (subframe_to_load - 1 + 
                            HDRRMAC4DRV_SUBFRAME_N_MAX) %
                            HDRRMAC4DRV_SUBFRAME_N_MAX;

    hdrrmac4drv.interlace = hdrrmac4drv.subframe_id
      % HDRSCMRMACCMN_MAX_NUM_INTERLACES;

    if (subframes_missed)
    {
      interlace = hdrrmac4drv.interlace;

      for (idx=0; idx<MIN(missed_subframes, HDRSCMRMACCMN_MAX_NUM_INTERLACES);
        idx++)
      {
        for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
        {
          /* Point to the interlace history data for the interlace. */
          packet_info = &hdrrmac4drv.per_carrier_info[ch].
            interlace_history[interlace];

          /* Mark the missed subframe interlace as empty for use in 
             permitted payload. */
          packet_info->packet_id   = (uint8) HDRRMAC4DRV_IDLE_PACKET;
          packet_info->tx_rri      = (uint8) HDRRMACCMN_0_BITS;

        }

        /* Set interlace to previous interlace */
        interlace = (interlace + 2 ) % HDRSCMRMACCMN_MAX_NUM_INTERLACES;
      }
    }

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
    /* For each channel... */
    for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      /* Either first time through or missed interlace. Align 
         RRI history appropriately. */
      if (hdrrmac4drv.rri_history_idx < HDRSCMRMACCMN_MAX_NUM_INTERLACES) 
      {

        /* Save the most recent RRI sent */
        rri_back_1 = hdrrmac4drv.per_carrier_info[ch].rri_history[hdrrmac4drv.rri_history_idx];
 
        /* Save the second most recent RRI sent */
        rri_back_2 = hdrrmac4drv.per_carrier_info[ch].rri_history[(hdrrmac4drv.rri_history_idx+2) 
                     % HDRSCMRMACCMN_MAX_NUM_INTERLACES];

        /* Load "now-1" with the latest from RRI history. */
        hdrrmac4drv.per_carrier_info[ch].rri_history[(hdrrmac4drv.interlace+2) 
          % HDRSCMRMACCMN_MAX_NUM_INTERLACES] = rri_back_1;

        /* Load "now-2" with the 2nd latest from RRI history. */
        hdrrmac4drv.per_carrier_info[ch].rri_history[(hdrrmac4drv.interlace+1) 
          % HDRSCMRMACCMN_MAX_NUM_INTERLACES] = rri_back_2;
      }

      /* Either first time through or missed interlace.  Clear RRI
         history for that subframe to be conservative. */
      hdrrmac4drv.per_carrier_info[ch].rri_history[hdrrmac4drv.interlace] = 0;
    }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

  }

  /* Store current subframe */
  tx_status_ptr->subframe_n = hdrrmac4drv.subframe_id;

  /* This is the interlace the ARQ is for, or the currently transmitting
     interlace.  
     Thus the ARQ was transmit and received in the previous subframe. */
  arq_interlace = hdrrmac4drv.interlace;

  /* Compute previous interlace */
  prev_interlace = (arq_interlace + 2 ) % HDRSCMRMACCMN_MAX_NUM_INTERLACES;

  for (idx=0; idx<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
  {
    ch = hdrrmac4drv.open_chans[idx];

    /* For all OPEN channels, populate
       - carrier_status_info
       - MARQ status
       based on ARQ status and MARQ info valid flag */
    if ( idx < hdrrmac4drv.num_chans )
    {
      pkt_status_ptr = &(hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
        rmacInfo[ch].rlPacket);

      /* Get ARQ status */
      carrier_status_info[ch].arq_status = pkt_status_ptr->arqStatus;

      /* Get encoder (packet) status */
#ifdef FEATURE_HDR_RMAC4_UNIT_TEST
      #error code not present
#else
      carrier_status_info[ch].encode_status = ( ( pkt_status_ptr->status >> 
        HDRRMAC4DRV_FW_PKT_STATUS_SHFT ) & HDRRMAC4DRV_FW_PKT_STATUS_BMSK );
#endif


#ifdef HDRRMAC4DRV_DEBUG_TRACE_MDSP_WRITES
#error code not present
#endif

      /* Point to info for current packet */
      carrier_status_info[ch].packet_info = 
        &hdrrmac4drv.per_carrier_info[ch].interlace_history[arq_interlace];

      /* MARQ info */
      carrier_status_info[ch].marq_info = 
        &hdrrmac4drv.per_carrier_info[ch].marq_info;

      /* Save MARQ info valid status */
      carrier_status_info[ch].marq_info_valid = 
        hdrrmac4drv.per_carrier_info[ch].marq_info_valid;

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
      /* Decay current and not-data-limited throughput values and convert
         them from Q.8 to Q.11 units. */
      curr_tput = ( ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR - 
        ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
        HDRRMACCMN_MODEMAPI_FILT_TC ) ) * 
        ( (uint64) hdrrmac4drv.per_carrier_info[ch].curr_tput ) )<< 3;

      not_data_lmtd_tput = ( ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR - 
        ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
        HDRRMACCMN_MODEMAPI_FILT_TC ) ) * 
        ( (uint64) hdrrmac4drv.per_carrier_info[ch].not_data_lmtd_tput ) )<< 3;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

      /* Evaluate H-ARQ Status */
      if(carrier_status_info[ch].arq_status & 
         HDRMDSP_RL_HARQ_ACK_RECEIVED_V)
      {
        /* Set H-ARQ info valid flag */
        carrier_status_info[ch].harq_info_valid = TRUE;

        /* Increment metrics ack count based on RRI, subpacket, and TM */
        hdrrmac4drv_metrics.log[ch].interlace[arq_interlace].
          tm[carrier_status_info[ch].packet_info->tm].
          rri[carrier_status_info[ch].packet_info->rri-1].
          acks[carrier_status_info[ch].packet_info->subpacket]++;

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
        /* Update the current throughput value with the number of bits
           acknowledged in this subframe */
        bits_ackd = hdrrmaccmn_get_bits_from_rri( 
          carrier_status_info[ch].packet_info->rri );

        sf_tput = ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
          HDRRMACCMN_MODEMAPI_FILT_TC ) * bits_ackd;

        curr_tput += sf_tput;

        if ( carrier_status_info[ch].packet_info->not_data_lmtd )
        {
          /* Update the not-data-limited throughput value with the number
             of bits acknowledged in this subframe */
          not_data_lmtd_tput += sf_tput;
        }
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

        /* Sanity check...may be removed after initial integration */
        if (carrier_status_info[ch].packet_info->subpacket == 
            HDRRMAC4DRV_LAST_SUBPACKET)
        {
          HDR_MSG_DRIVERS(MSG_LEGACY_ERROR, 
            "ERROR! HARQ in wrong subpacket.");
        }

      } /* Received HARQ-ACK*/

      if(carrier_status_info[ch].arq_status & 
              HDRMDSP_RL_PARQ_NAK_RECEIVED_V)
      {
        /* Handle MARQ */

        /* If MARQ info is valid, and we received a NAK... */
        if (hdrrmac4drv.per_carrier_info[ch].marq_info_valid)
        {
          /* Set PARQ NAK status */
          carrier_status_info[ch].parq_nak_info = TRUE;

          hdrrmac4drv_metrics.log[ch].interlace[prev_interlace].
            tm[carrier_status_info[ch].marq_info->tm].
            rri[carrier_status_info[ch].marq_info->rri-1].parq_naks++;

          /* Store MARQ status for returning to RMAC */
          tx_status_ptr->marq_status.c[num_marq_chans].marq_nak_id = 
            (hdrrmac4drv.subframe_id - 13 + HDRRMAC4DRV_SUBFRAME_N_MAX) % 
            HDRRMAC4DRV_SUBFRAME_N_MAX;

          tx_status_ptr->marq_status.c[num_marq_chans].phy_chan_idx = 
            ch;

          HDR_MSG_DEBUG_2( HDR_MSG_DBG_PKT_STATUS, 
            "MARQ received on channel %d : %d",
            ch,
            tx_status_ptr->marq_status.c[num_marq_chans].marq_nak_id );

          /* Increment number of channels on which MARQ is received */
          num_marq_chans++;

        }
        else /* If MARQ info is not valid, and we received a NAK... */
        {
          HDR_MSG_DEBUG_2( HDR_MSG_DBG_PKT_STATUS,
             "Unexpected MARQ for packet %d rx'd in subframe %d",
             (hdrrmac4drv.subframe_id - 13 + HDRRMAC4DRV_SUBFRAME_N_MAX) % 
             HDRRMAC4DRV_SUBFRAME_N_MAX,
             (hdrrmac4drv.subframe_id - 1 + HDRRMAC4DRV_SUBFRAME_N_MAX) % 
             HDRRMAC4DRV_SUBFRAME_N_MAX );

          /* Record unexpected MARQ NAKs */
          hdrrmac4drv_metrics.log[ch].interlace[prev_interlace].
            unexpected_parq_naks++;

        }

      } /* Received MARQ */

      else
      {
        /* If MARQ info is valid, and did not received a NAK... */
        if (hdrrmac4drv.per_carrier_info[ch].marq_info_valid)
        {
          /* Reset PARQ NAK status */
          carrier_status_info[ch].parq_nak_info = FALSE;

          /* Implied MARQ ACK. */
          hdrrmac4drv_metrics.log[ch].interlace[prev_interlace].
            tm[carrier_status_info[ch].marq_info->tm].
            rri[carrier_status_info[ch].marq_info->rri-1].acks[3]++;

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
          /* Update the current throughput value with the number of bits
             acknowledged in this subframe */
          bits_ackd = hdrrmaccmn_get_bits_from_rri( 
            carrier_status_info[ch].marq_info->rri );

          sf_tput = ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR / 
            HDRRMACCMN_MODEMAPI_FILT_TC ) * bits_ackd;

          curr_tput += sf_tput;

          if ( carrier_status_info[ch].marq_info->not_data_lmtd )
          {
            /* Update the not-data-limited throughput value with the number
               of bits acknowledged in this subframe */
            not_data_lmtd_tput += sf_tput;
          }
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

        }

      } /* No HARQ-ACK, no MARQ */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
      /* Remove scaling factor multiplier in the throughput values,
         convert them from Q.11 to Q.8 units and store. */
      hdrrmac4drv.per_carrier_info[ch].curr_tput = ( uint32) ( ( ( 
        curr_tput + ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR >> 1) )
        / HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR ) >> 3 );

      hdrrmac4drv.per_carrier_info[ch].not_data_lmtd_tput = ( uint32) ( ( ( 
        not_data_lmtd_tput + ( HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR >> 1) )
        / HDRRMACCMN_MODEMAPI_FILT_SCALING_FACTOR ) >> 3 );
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

      /* Prepare for possible MARQ (in next subframe) */
      if (carrier_status_info[ch].packet_info->subpacket == 
          HDRRMAC4DRV_LAST_SUBPACKET)
      {
        /* Point to current packet */
        packet_info = &hdrrmac4drv.per_carrier_info[ch].
          interlace_history[arq_interlace];

        hdrrmac4drv.per_carrier_info[ch].marq_info.subpacket      = 
          packet_info->subpacket;
        hdrrmac4drv.per_carrier_info[ch].marq_info.packet_id      = 
          packet_info->packet_id;
        hdrrmac4drv.per_carrier_info[ch].marq_info.rri            = 
          packet_info->rri;
        hdrrmac4drv.per_carrier_info[ch].marq_info.tm             = 
          packet_info->tm;
        hdrrmac4drv.per_carrier_info[ch].marq_info.data_length    = 
          packet_info->data_length;
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
        hdrrmac4drv.per_carrier_info[ch].marq_info.boosted_t2p    = 
          packet_info->boosted_t2p;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
        hdrrmac4drv.per_carrier_info[ch].marq_info.not_data_lmtd  = 
          packet_info->not_data_lmtd;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */
        hdrrmac4drv.per_carrier_info[ch].marq_info_valid = 
          TRUE;

        /* Clear packet info contents */
        packet_info->subpacket   = 0;
        packet_info->packet_id   = (uint8) HDRRMAC4DRV_IDLE_PACKET;
        packet_info->rri         = (uint8) HDRRMACCMN_0_BITS;
        packet_info->tx_rri      = (uint8) HDRRMACCMN_0_BITS;
        packet_info->tm          = (uint8) HDRRMACCMN_TX_MODE_HI_CAP;
        packet_info->data_length = 0;

      }
      else
      {
        /* Reset MARQ_Info_Valid flag */
        hdrrmac4drv.per_carrier_info[ch].marq_info_valid = 
          FALSE;
      }

    } /* if a channel is in OPEN state */
    else
    {
      /* Reset MARQ_Info_Valid flag for channels not in OPEN state */
      hdrrmac4drv.per_carrier_info[ch].marq_info_valid = FALSE;

    } /* if a channel is not in OPEN state */

  } /* for all channels */

  HDR_MSG_DEBUG_7( HDR_MSG_DBG_PKT_STATUS,
    "From interlace %d -- ARQ: %x,%x,%x Pkt: %x,%x,%x",
    prev_interlace,
    carrier_status_info[0].arq_status, 
    carrier_status_info[1].arq_status,
    carrier_status_info[2].arq_status,
    carrier_status_info[0].encode_status,
    carrier_status_info[1].encode_status,
    carrier_status_info[2].encode_status);

  /* Store number of MARQ'd channels */
  tx_status_ptr->marq_status.num_chans = num_marq_chans;

  /* Log ARQ_status */
  hdrrmac4drv_log_arq_status( carrier_status_info );

  /* Packet transmission completed, log packet status. */
  hdrrmac4drv_log_pkt_status( carrier_status_info );

  /* Populate encode_status_ch_list */
  /* First pass - store channels for which send_packet was called 
     in the previous subframe */
  for (ch=0; ch<hdrrmac4drv.num_chans_prev; ch++)
  {
    phy_chan_id = hdrrmac4drv_prev_sf_sent_channel_list[ch];

    encode_status_ch_list[ch] = phy_chan_id;

    /* Mark the channel entry as accounted for */
    ch_entries_filled[phy_chan_id] = TRUE;
  }

  num_chans_encode_status = hdrrmac4drv.num_chans_prev;

  /* Second pass - store OPEN channels for which send_packet was 
     not called in the previous subframe */
  for (idx=0; idx<hdrrmac4drv.num_chans; idx++)
  {
    phy_chan_id = hdrrmac4drv.open_chans[idx];

    if (!ch_entries_filled[phy_chan_id])
    {
      /* Error catching */
      ASSERT( num_chans_encode_status < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );

      encode_status_ch_list[num_chans_encode_status] = phy_chan_id;

      num_chans_encode_status++;
    }
  }


  /* Update statuses for channels on which encode status is to be 
     reported */
  for (ch=0; ch<num_chans_encode_status; ch++)
  {
    /* Populate channel previous packet statuses in the sent channel order */
    phy_chan_id = encode_status_ch_list[ch];

    tx_status_ptr->prev_pkt_status.c[ch].phy_chan_idx = phy_chan_id;

    /* Point to current packet info in interlace history */
    packet_info = &hdrrmac4drv.per_carrier_info[phy_chan_id].
      interlace_history[arq_interlace];

    /* Evaluate subframe status */
    switch(carrier_status_info[phy_chan_id].encode_status)
    {
      case HDRMDSP_RL_STATUS_PACKET_ACCEPTED:

        /* Sanity check for packet accept and rri pending. 
           If we accepted a packet but pending rri is PS 0 (implying idle),
           something is wrong.*/
        if ( hdrrmac4drv.per_carrier_info[phy_chan_id].rri_pending == 
             (uint8) HDRRMACCMN_0_BITS )
        {
          HDR_MSG_DRIVERS(MSG_LEGACY_ERROR, "RTC: DSP Out of sync!!");

          /* Tell the caller nothing was sent. */
          tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_accepted = 
            FALSE;

          packet_info->packet_id = (uint8) HDRRMAC4DRV_IDLE_PACKET;
          packet_info->subpacket = 0;
          packet_info->rri       = (uint8) HDRRMACCMN_0_BITS;
        }
        else
        {

          /* Update with new packet info */
          packet_info->rri         = 
            hdrrmac4drv.per_carrier_info[phy_chan_id].rri_pending;
          packet_info->tm          =          
            hdrrmac4drv.per_carrier_info[phy_chan_id].tm_pending;
          packet_info->packet_id   =   
            hdrrmac4drv.subframe_id;
          packet_info->data_length = 
            hdrrmac4drv.per_carrier_info[phy_chan_id].data_length;
          packet_info->subpacket   =   0;
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
          packet_info->boosted_t2p = 
            hdrrmac4drv.per_carrier_info[phy_chan_id].boosted_t2p_pending;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
#ifdef FEATURE_HDR_MODEM_API_RL_INFO
          packet_info->not_data_lmtd = 
            hdrrmac4drv.per_carrier_info[phy_chan_id].not_data_lmtd_pending;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

          /* Store termination target.  This is used to calculate CAPUT 
             priority */
          packet_info->tt          = 
            hdrrmac4drv_config->t2p_info[packet_info->tm].
            tx_t2p_info[packet_info->rri].term_target;

          /* Increment metrics for accepted based on RRI, subpacket, and TM */
          hdrrmac4drv_metrics.log[phy_chan_id].interlace[arq_interlace].
            tm[packet_info->tm].rri[(packet_info->rri)-1].total_sent++;

          /* Tell the caller it was accepted */
          tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_accepted = 
            TRUE;

          /* Log ST-3 RTC packet once we know it was accepted. */
          if ( log_status ( LOG_HDR_RL_PHY_LAYER_PKT_C ) )
          {
            hdrlog_mc_carrier_id_type carrier_id;

            carrier_id.channel_id = phy_chan_id;
            carrier_id.channel_no = (uint16)
              hdrrmac4drv.per_carrier_info[phy_chan_id].channel_record.chan_num;
            carrier_id.band_class = (uint16) ((uint32) (int32) 
              hdrrmac4drv.per_carrier_info[phy_chan_id].channel_record.band);

            hdrenc_log_rtc_phy_layer_pkt( 
              hdrrmac4drv.per_carrier_info[phy_chan_id].pkt_ptr, 
              hdrrmac4drv.per_carrier_info[phy_chan_id].mac_trailer, 
              hdrrmac4drv.per_carrier_info[phy_chan_id].data_length, 
              hdrrmac4drv.per_carrier_info[phy_chan_id].rri_pending,
              HDRLOG_PHY_LAYER_REVISION_B,
              &carrier_id,
              hdrrmac4drv.subframe_id );
          }

        }

        break;

      case HDRMDSP_RL_STATUS_CONTINUING_PREVIOUS:

        /* Tell the caller it was rejected */
        tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_accepted = 
          FALSE;

        /* Update interlace history */
        packet_info->subpacket++;

        break;


      case HDRMDSP_RL_STATUS_LATE_SW_ERROR:
      case HDRMDSP_RL_STATUS_LATE_FW_ERROR:

        /* Set encoder window missed flag */
        *enc_window_missed_ptr = TRUE;

      /* Treat an unknown code like an error */
      default:

        HDR_MSG_DRIVERS_1( MSG_LEGACY_ERROR, 
          "RL Pkt Dropped: mDSP error %x for previous packet",
          carrier_status_info[phy_chan_id].encode_status);

        /* Record error so we don't build a new frame this time around */
        hdrrmac4drv.timing_error = TRUE;

        hdrrmac4drv_metrics.mc_rl_pkts_dropped++;

        /* Drop through intentional */

      case HDRMDSP_RL_STATUS_IDLE:

        /* Tell the caller nothing was sent. */
        tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_accepted = 
          FALSE;

        packet_info->packet_id = (uint8) HDRRMAC4DRV_IDLE_PACKET;
        packet_info->subpacket = 0;
        packet_info->rri       = (uint8) HDRRMACCMN_0_BITS;

        break;

    }

    /* Free the last packet loaded.  May be NULL if no packet was scheduled. */
    dsm_free_packet(&hdrrmac4drv.per_carrier_info[phy_chan_id].pkt_ptr);

    /* If PA is off (during RLSI) set reported packet id to IDLE,
       and mark this subframe as empty for use in permitted payload. */
    if (hdrrmac4drv.pa_status == PA_OFF)
    {
      tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_id = 
        HDRRMAC4DRV_IDLE_PACKET;

      packet_info->tx_rri = (uint8) HDRRMACCMN_0_BITS;
    }
    else
    {
      tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_id = 
        packet_info->packet_id;

      packet_info->tx_rri = packet_info->rri;
      
#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
      hdrrmac4drv.per_carrier_info[phy_chan_id].rri_history[arq_interlace] = 
        packet_info->rri;
      hdrrmac4drv.rri_history_idx = arq_interlace;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

    }

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
    if ( packet_info->boosted_t2p )
    {
      /* Store TxT2P of packet being Tx'd */
      tx_status_ptr->prev_pkt_status.c[ch].tx_t2p = 
        hdrrmac4drv.boosted_t2p_info_ptr[
        packet_info->rri].tx_t2p[packet_info->subpacket];
    }
    else
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
    {
      /* Store TxT2P of packet being Tx'd */
      tx_status_ptr->prev_pkt_status.c[ch].tx_t2p = 
        hdrrmac4drv_config->t2p_info[packet_info->tm].
        tx_t2p_info[packet_info->rri].
        tx_t2p[packet_info->subpacket];
    }

    /* Reset pending RRI */
    hdrrmac4drv.per_carrier_info[phy_chan_id].rri_pending = 
      (uint8) HDRRMACCMN_0_BITS;

#if 1
    /* -- TST MSG -- */
    if (packet_info->packet_id != HDRRMAC4DRV_IDLE_PACKET)
    {
      if (tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_accepted)
      {
        HDR_MSG_DEBUG_3( HDR_MSG_DBG_PKT_STATUS,
          "Packet accepted; Ch %d, Tx Id %d, T2P %d",
          tx_status_ptr->prev_pkt_status.c[ch].phy_chan_idx,
          tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_id,
          tx_status_ptr->prev_pkt_status.c[ch].tx_t2p );  
      }
      else
      {
        HDR_MSG_DEBUG_3( HDR_MSG_DBG_PKT_STATUS,
          "Packet cancelled; Ch %d, Tx Id %d, T2P %d",
          tx_status_ptr->prev_pkt_status.c[ch].phy_chan_idx,
          tx_status_ptr->prev_pkt_status.c[ch].prev_pkt_id,
          tx_status_ptr->prev_pkt_status.c[ch].tx_t2p );  
      }
    }
#endif
    
  }

  /* Store the total number of channels on which data was sent during
     previous subframe */
  tx_status_ptr->prev_pkt_status.num_chans_prev = hdrrmac4drv.num_chans_prev;

  /* Store the total number of channels for which previous packet 
     encode status is reported */
  tx_status_ptr->prev_pkt_status.num_chans = num_chans_encode_status;

  /* Update for next subframe */
  hdrrmac4drv.interlace =   interlace_to_load;
  hdrrmac4drv.subframe_id = subframe_to_load;

  /* Mark tx info as updated. Used by tx pkt info request API */
  hdrrmac4drv.current_tx_updated = TRUE;

  /* Submit RL Metrics log packet if the submit flag is set */
  if (hdrrmac4drv_metrics.submit_rl_metrics)
  {
    hdrrmac4drv_submit_rl_metrics_log();
  }

  /* If we are counting down subframes till MCDO evalulation, decrement
     the counter and call evaluation function if the counter becomes zero */
  if (hdrrmac4drv.subframes_till_mcdo_eval > 0)
  {
    hdrrmac4drv.subframes_till_mcdo_eval--;

    if (hdrrmac4drv.subframes_till_mcdo_eval == 0)
    {
      hdrrmac4drv_eval_mcdo_connection_setup();
    }
  }

  /* If there are no packets in progress on a carrier
     - Drop the carrier if carrier drop is pending
     - Deactivate the carrier if carrier deactivation is pending */
  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv_packets_are_active(ch))
    {
      packets_active = TRUE;
    }
    else
    {
      /* if carrier is scheduled for drop or data-deactivation */
      if ( ( hdrrmac4drv.per_carrier_info[ch].carrier_state 
        == HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE ) ||
        ( hdrrmac4drv.per_carrier_info[ch].carrier_state 
        == HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE ) )
      {
        hdrrmac4drv.per_carrier_info[ch].okay_to_deactivate = TRUE;

        HDR_MSG_DRIVERS_1( MSG_LEGACY_MED, 
          "Carrier %d okay to deactivate",
          ch );
      }

    } /* if there are no active packets on the carrier */

  } /* for all carriers */

  /* If shutdown is pending, check if we've completed all
     packets that were in progress. If so, call callback. */
  if ((hdrrmac4drv.shutdown_pending == TRUE) && (!packets_active))
  {
    hdrrmac4drv.shutdown_pending = FALSE;
    hdrrmac4drv.shutdown_cb();
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  /* Reset num_chans_prev and list of channels to queue data
     to encoder in the previous subframe */
  hdrrmac4drv.num_chans_prev = 0;
  memset ( (void*)&hdrrmac4drv_prev_sf_sent_channel_list, 0, 
    sizeof(hdrrmac4drv_prev_sf_sent_channel_list) );

  /* Return success */
  return TRUE;

} /* hdrrmac4drv_get_prev_tx_status() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SUBFRAME_ID_IS_VALID                           INTERNAL

DESCRIPTION
  Checks a subframe id to see if it is valid given the current time.
    
DEPENDENCIES
  None.
  
PARAMETERS
  None.  

RETURN VALUE
  TRUE/FALSE based on whether passed subframe id is valid.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_subframe_id_is_valid
(
  uint16 subframe_to_check
    /* subframe id to validate */
)
{            
  uint32 mod_time = 0;
    /* Current modulator time */

  uint32 mod_hs_time;
    /* Current modulator time in halfslots */

  uint16 next_pkt_hs_time;
    /* Next subframe time in halfslots */

  uint16 subframe;
    /* Number associated with next subframe (subframe id) */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Latch modulator time and read. */
  mod_time = hdrts_get_tx_chip_time();
  
  /* Shift by 10 (div 1024) to convert to halfslots */
  mod_hs_time = mod_time >> 10;
  next_pkt_hs_time = (mod_hs_time & 0x0FFF) + 
    hdrrmac4drv.subframe_halfslot_offset;

  /* Gives the subframe we are interested in */
  subframe = (((next_pkt_hs_time / HDRRMAC4DRV_HALFSLOTS_PER_SUBFRAME) + 
    (HDRRMAC4DRV_SUBFRAME_N_MAX - hdrrmac4drv.subframe_offset)) %
    HDRRMAC4DRV_SUBFRAME_N_MAX);

  /* Mod by halfslots per subframe to get which halfslot we are at within
     the subframe */
  next_pkt_hs_time = next_pkt_hs_time % HDRRMAC4DRV_HALFSLOTS_PER_SUBFRAME;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  /* Check subframe number against expected subframe number. 
     If they don't match, or if we are too late in the subframe,
     drop the packet we are working on. We have already or will miss
     the encoder deadline. */
  if ((subframe != subframe_to_check) || 
      (next_pkt_hs_time> HDRRMAC4DRV_HALFSLOT_DEADLINE))
  {
    HDR_MSG_DRIVERS_4( MSG_LEGACY_ERROR,
      "RL Pkt Dropped: Missed load deadline. (sf0 %d, sf1 %d, hs0 %d, hs1 %d)",
      subframe_to_check,
      subframe,
      hdrrmac4drv.halfslot_start,
      next_pkt_hs_time);

    hdrrmac4drv_metrics.mc_rl_pkts_dropped++;

    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* hdrrmac4drv_subframe_id_is_valid() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SEND_PACKET                                    EXTERNAL

DESCRIPTION
  Sends a Reverse Traffic packet(s) to the encoder.

DEPENDENCIES
  MUST be called after hdrrmac4drv_get_prev_tx_status() within a given
  subframe.  The packet information for the enabled channels in pkt_enc_ptr
  must be arranged according to the channel CAPUT priorities.
  RMG: Currently send packet does not care about the arrangement order.
    
PARAMETERS
  enc_window_missed_ptr
                    - Pointer to encoder window missed flag.  It is set if 
                      one of the following is true:
                      Packet wasn't loaded because SW was late
                      Packet wasn't loaded because DSP was not ready
  num_chans         - Number of records contained in pkt_enc_ptr
  pkt_enc_ptr       - Pointer to num_chans instances of packet information

RETURN VALUE
  Returns the packet id of the packet loaded.
  
SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdrrmac4drv_send_packet
(
  boolean                              *enc_window_missed_ptr,
  uint8                                 num_chans,
  hdrrmac4drv_pkt_enc_info_struct_type *pkt_enc_ptr
#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
  ,boolean                             rtt_boost_enabled 
    /* if boost is applied due to RTT. */
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
)
{

  uint8 mac_trailer;
    /* Left justified 2-bit mac trailer, for encoder */

  uint8 transition_point;
    /* Transition point is used to determine the RRI gain profile to use */

  hdrrmaccmn_tx_t2p_info_type * rate_info;
    /* Shorthand pointer */

  hdrrmac4drv_pkt_gains_type * rri_gains_ptr;

  uint16 packet_id = HDRRMAC4DRV_IDLE_PACKET;
    /* Return value; the id of the packet loaded */

  uint8  i;
    /* Loop counter */

  boolean sf_valid = TRUE;
    /* temp to store whether the subframe is valid */

  uint8  ch;
    /* Loop counter */

  uint8 phy_chan_id;
    /* Physical channel id */

  hdrfw_msg_u fw_cmd;
    /* Firmware command */

  errno_enum_type estatus;
    /* Error status */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Error catching */
  ASSERT( pkt_enc_ptr != NULL );

  /* Initialize firmware send packet message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_SEND_PACKET_CMD );

  /* Reset encoder window missed flag */
  *enc_window_missed_ptr = FALSE;

  for (ch=0; ch<num_chans; ch++)
  {
#ifndef FEATURE_HDR_RMAC4_UNIT_TEST
    /* Error catching */
    ASSERT( pkt_enc_ptr[ch].pkt_ptr != NULL );
#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */

    phy_chan_id = pkt_enc_ptr[ch].phy_chan_idx;

    /* Include carrier in firmware subframe config msg carrier mask */
    fw_cmd.rmac_send_packet.carrierMask |= ( 1 << phy_chan_id );

    /* Store channel id for packet accept/reject status logging */
    hdrrmac4drv_prev_sf_sent_channel_list[ch] = phy_chan_id;

    /* Sanity check on pointer -- it should have been cleared by now. */ 
    if (hdrrmac4drv.per_carrier_info[phy_chan_id].pkt_ptr != NULL)
    {
      /* ERROR! Someone didn't clear previous packet! */
      HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, "RL Pkt: Last packet not freed?!?");
      dsm_free_packet( &hdrrmac4drv.per_carrier_info[phy_chan_id].pkt_ptr );
    }

    /* Copy packet pointer so it can be freed later. */
    hdrrmac4drv.per_carrier_info[phy_chan_id].pkt_ptr = pkt_enc_ptr[ch].pkt_ptr;


#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
    if ( pkt_enc_ptr[ch].packet_is_t2p_boosted )
    {
      rate_info = 
        &( hdrrmac4drv.boosted_t2p_info_ptr[pkt_enc_ptr[ch].pkt_size_index] );
      /* Force transmission mode to HiCap */
      pkt_enc_ptr[ch].transmission_mode = HDRRMACCMN_TX_MODE_HI_CAP;
    }
    else
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
    {
      /* Get information on packet size we are to send */
      rate_info = &hdrrmac4drv_config->t2p_info[pkt_enc_ptr[ch].
        transmission_mode].tx_t2p_info[pkt_enc_ptr[ch].pkt_size_index];
    }

#ifdef FEATURE_FACTORY_TESTMODE
    if( ftm_get_mode() == FTM_MODE )
    { 
      /* Set the FTM dictated Gain transition point in FTM Mode*/
      rate_info->trans_point = hdrhitmsg_get_rmac_params_ptr()->gain_transition_point;

      for (i=0;i<HDRRMACCMN_MAX_NUM_SUBPACKETS;i++)
      {
        if ( i <= hdrhitmsg_get_rmac_params_ptr()->gain_transition_point )
        {
          /* Set the FTM dictated t2p pre transition gains*/ 
          rate_info->data_t2p_db[i] = 
            hdrhitmsg_get_rmac_params_ptr()->t2p_pre_transition_revb_c[phy_chan_id];
        }
        else
        { 
          /* Set the FTM dictated t2p post transition gains*/ 
          rate_info->data_t2p_db[i] = 
            hdrhitmsg_get_rmac_params_ptr()->t2p_post_transition_revb_c[phy_chan_id];
        }
      }
    }
#endif /* FEATURE_FACTORY_TESTMODE */


    transition_point = rate_info->trans_point;

    /* Set mac_trailer byte */
    mac_trailer = 
      (((uint8)pkt_enc_ptr[ch].conn_format) << 
       HDRRMAC4DRV_FORMAT_BIT_S) | 
      (((uint8)pkt_enc_ptr[ch].transmission_mode) << 
       HDRRMAC4DRV_TX_MODE_BIT_S);

    rri_gains_ptr = &hdrrmac4drv.rri_to_pilot[transition_point];

    /* Tell FW we are about to load data */
    /* DSP only uses this to check for a timing deadline.  Since
       we already test for timing deadline in INTLOCK before
       HDRMDSP_REQ_PACKET_TX_COMPLETE() below, this is not needed. */
    /* HDRMDSP_REQ_PACKET_TX_START(); */

    /* Write RRI and interlace to DSP. The interlace seeds the scrambler. */
    fw_cmd.rmac_send_packet.rlPacketCfg[phy_chan_id].
      rri = (hdrrmac4drv.interlace << 4) | pkt_enc_ptr[ch].pkt_size_index;

    for (i=0; i<HDRRMACCMN_MAX_NUM_SUBPACKETS; i++)
    {
      /* Write traffic to pilot ratios to FW */
      fw_cmd.rmac_send_packet.rlPacketCfg[phy_chan_id].dataToPilot[i] = 
        ((int16) rate_info->data_t2p_db[i]) + 
        hdrrmac4drv.per_carrier_info[phy_chan_id].best_sector_delta_t2p;

      /* Write RRI to pilot ratios to FW */
      fw_cmd.rmac_send_packet.rlPacketCfg[phy_chan_id].rriToPilot[i] = 
        (uint16) rri_gains_ptr->subpacket_gain[i];

      /* If any backoff from MAX power is required for this payload type,
         configure here for DSP.  Currently, zero backoff is programmed for
         all subpackets. */
      fw_cmd.rmac_send_packet.rlPacketCfg[phy_chan_id].maxTxBackOff[i] = 0;
    }

#ifdef FEATURE_HDR_TRAFFIC_DTX
    /* Write term target to FW for tDTX */
    fw_cmd.rmac_send_packet.rlPacketCfg[phy_chan_id].terminationTarget
      = rate_info->term_target;
#endif /* FEATURE_HDR_TRAFFIC_DTX */

#ifndef FEATURE_HDR_RMAC4_UNIT_TEST
    /* Send the frame to encoder driver. */
    hdrenc_load_frame(
      phy_chan_id,
      pkt_enc_ptr[ch].pkt_ptr,
      pkt_enc_ptr[ch].data_length,
      mac_trailer,
      pkt_enc_ptr[ch].pkt_size_index);
#endif /* FEATURE_HDR_RMAC4_UNIT_TEST */

    /* Record pending RRI and TM, so we may log them if accepted. */
    hdrrmac4drv.per_carrier_info[phy_chan_id].rri_pending = 
      pkt_enc_ptr[ch].pkt_size_index;
    hdrrmac4drv.per_carrier_info[phy_chan_id].tm_pending  = 
      (uint8) (pkt_enc_ptr[ch].transmission_mode);

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
    /* Report boost for rates 7 and above */
    if ( ( pkt_enc_ptr[ch].pkt_size_index < HDRRMACCMN_T2P_BOOST_REPORT_RRI_THRESH )
#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
         || rtt_boost_enabled
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */
       )
    {
      hdrrmac4drv.per_carrier_info[phy_chan_id].boosted_t2p_pending = 
        FALSE;
    }
    else
    {
      hdrrmac4drv.per_carrier_info[phy_chan_id].boosted_t2p_pending = 
        pkt_enc_ptr[ch].packet_is_t2p_boosted;        
    }
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */

#ifdef FEATURE_HDR_MODEM_API_RL_INFO
    hdrrmac4drv.per_carrier_info[phy_chan_id].not_data_lmtd_pending = 
      pkt_enc_ptr[ch].not_data_lmtd;
#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

    hdrrmac4drv.per_carrier_info[phy_chan_id].mac_trailer = mac_trailer;
    hdrrmac4drv.per_carrier_info[phy_chan_id].data_length = 
      pkt_enc_ptr[ch].data_length;

    /* Ensure we haven't missed our deadline before latching the frame */
    /* Enter critical section */
    REX_DISABLE_PREMPTION();

    sf_valid = hdrrmac4drv_subframe_id_is_valid(hdrrmac4drv.subframe_id);

    if( sf_valid ) 
    {
      /* Tell FW that the frame has been loaded. */
      /* TODO: this is no longer needed in Q6 since send_packet cmd already
         conveys that a frame has been loaded.  Request removal. */
      fw_cmd.rmac_send_packet.rlPacketCfg[phy_chan_id].request = 
        HDRRMAC4DRV_FW_FRAME_LOADED;
    }

    /* Exit critical section */
    REX_ENABLE_PREMPTION();

    /* Indicate we were too late in F3 */
    if (!sf_valid)
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_ERROR,
        "RL Pkt Dropped: Load time too late.");

      /* Set encoder window missed flag */
      *enc_window_missed_ptr = TRUE;

      /* Increment number of RL packets dropped */
      hdrrmac4drv_metrics.mc_rl_pkts_dropped++;
    }

    /* Print F3 if we sent something */
    if (sf_valid)
    {

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
      /* Set RL packet scheduled flag */
      hdrrmac4drv.rl_packet_scheduled = TRUE;
#endif /* defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
      HDR_MSG_DRIVERS_7(MSG_LEGACY_HIGH,
        "HDR Encoder: Ch %d, Loading %d bytes. RRI: %d Int: %d N: %d (%x) Boost: %d",
        phy_chan_id,
        pkt_enc_ptr[ch].data_length,
        pkt_enc_ptr[ch].pkt_size_index,
        hdrrmac4drv.interlace,
        hdrrmac4drv.subframe_id,
        pkt_enc_ptr[ch].transmission_mode, 
        pkt_enc_ptr[ch].packet_is_t2p_boosted );
#else
      HDR_MSG_DRIVERS_6(MSG_LEGACY_HIGH,
        "HDR Encoder: Ch %d, Loading %d bytes. RRI: %d Int: %d N: %d (%x)",
        phy_chan_id,
        pkt_enc_ptr[ch].data_length,
        pkt_enc_ptr[ch].pkt_size_index,
        hdrrmac4drv.interlace,
        hdrrmac4drv.subframe_id,
        pkt_enc_ptr[ch].transmission_mode);
#endif
    }

  } /* for all channels */

  if (sf_valid)
  {
    /* Send RL packet to firmware */
    estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );
  }

  /* Store the total number of channels transmitting data */
  hdrrmac4drv.num_chans_prev = num_chans;

  packet_id = hdrrmac4drv.subframe_id;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  /* Return transmitted subframe number (packet id). */
  return packet_id;

} /* hdrrmac4drv_send_packet() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_MAX_PERMITTED_PKT_SIZE                     EXTERNAL

DESCRIPTION
  Returns the maximum permitted packet size for the next subpacket on all
  the enabled channels.

DEPENDENCIES
  Must be called after get_prev_tx_status.

PARAMETERS
  max_pkt_size_ptr - Pointer to MAX_NUM_PHY_CHANS instances of max_pkt_size.
                     The entries corresponding to the un-assigned channels
                     will be ignored.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_get_max_permitted_pkt_size
(
  hdrrmaccmn_payload_size_enum_type * max_pkt_size_ptr
)
{
  uint8  pkt_interlace;
    /* Interlace */

  uint8 ch, idx;
    /* Channel loop counter */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
  hdrrmaccmn_payload_size_enum_type max_prev_rri, max_perm_rri;
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Grab stored interlace, which has already been calculated. 
     This interlace is the interlace we are interested in sending a packet
     in (n), as opposed to the currently transmitting interlace (n-1). */
  pkt_interlace = hdrrmac4drv.interlace; 

  /* Recover from encoder error by blanking one subframe.  We blank by
     returning a max_rri of 0, as if we cannot tx. */
  if (hdrrmac4drv.timing_error)
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
                     "RTC: Limiting to null packet due to missed deadline.");

    for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      max_pkt_size_ptr[ch] = HDRRMACCMN_0_BITS;
    }
  }
  else /* No encoder error */
  {
    for (idx=0; idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
    {
      ch = hdrrmac4drv.open_chans[idx];

      if ( idx < hdrrmac4drv.num_chans )
      {
        ASSERT( hdrrmac4drv.per_carrier_info[ch].interlace_history
                  [(pkt_interlace+2)%HDRSCMRMACCMN_MAX_NUM_INTERLACES].tx_rri < 
                  HDRRMACCMN_MAX_PAYLOADS );

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
        if ( hdrrmac4drv.perm_pyld_opt_enabled )
        {
          max_perm_rri = hdrrmac4drv_config->permitted_payload[
            hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace+2) %
            HDRSCMRMACCMN_MAX_NUM_INTERLACES]][HDRRMACCMN_SUBFRAME_N_MINUS_1];

          max_perm_rri = MIN(max_perm_rri, 
            hdrrmac4drv_config->permitted_payload[
            hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace+1) %
            HDRSCMRMACCMN_MAX_NUM_INTERLACES]][HDRRMACCMN_SUBFRAME_N_MINUS_2]);

          max_perm_rri = MIN(max_perm_rri, 
            hdrrmac4drv_config->permitted_payload[
            hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace)
            ]][HDRRMACCMN_SUBFRAME_N_MINUS_3]);

          /* Find max of the last 3 rates used from history */
          max_prev_rri = 
            hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace+2)
            % HDRSCMRMACCMN_MAX_NUM_INTERLACES];

          max_prev_rri = MAX(max_prev_rri, 
            hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace+1)
            % HDRSCMRMACCMN_MAX_NUM_INTERLACES]);

          max_prev_rri = MAX(max_prev_rri, 
            hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace)]);

          /* Pick the highest of the two rates */
          max_pkt_size_ptr[ch] = MAX(max_prev_rri, max_perm_rri);

          HDR_MSG_PROT_6( MSG_LEGACY_HIGH,   
                          "ch %d perm %d; prev rri %d [%d %d %d]", 
                          ch,
                          max_perm_rri,
                          max_prev_rri,
                          hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace+2) % 
                          HDRSCMRMACCMN_MAX_NUM_INTERLACES],
                          hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace+1) % 
                          HDRSCMRMACCMN_MAX_NUM_INTERLACES],
                          hdrrmac4drv.per_carrier_info[ch].rri_history[(pkt_interlace) % 
                          HDRSCMRMACCMN_MAX_NUM_INTERLACES] );
        }
        else
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */
        {

          /* Compute the max_rri from the Permitted Payload attribute data and 
             rate history per interlace */
          max_pkt_size_ptr[ch] = (hdrrmaccmn_payload_size_enum_type) 
            hdrrmac4drv_config->permitted_payload
            [hdrrmac4drv.per_carrier_info[ch].interlace_history
              [(pkt_interlace+2)%HDRSCMRMACCMN_MAX_NUM_INTERLACES].tx_rri]
            [HDRRMACCMN_SUBFRAME_N_MINUS_1];

          max_pkt_size_ptr[ch] = MIN(max_pkt_size_ptr[ch], 
            (hdrrmaccmn_payload_size_enum_type) 
            hdrrmac4drv_config->permitted_payload
            [hdrrmac4drv.per_carrier_info[ch].interlace_history
              [(pkt_interlace+1)%HDRSCMRMACCMN_MAX_NUM_INTERLACES].tx_rri]
            [HDRRMACCMN_SUBFRAME_N_MINUS_2]);

          max_pkt_size_ptr[ch] = MIN(max_pkt_size_ptr[ch], 
            (hdrrmaccmn_payload_size_enum_type) 
            hdrrmac4drv_config->permitted_payload
            [hdrrmac4drv.per_carrier_info[ch].interlace_history
              [(pkt_interlace)].tx_rri]
            [HDRRMACCMN_SUBFRAME_N_MINUS_3]);
        }
        /* Sanity check */
        if (max_pkt_size_ptr[ch] >= HDRRMACCMN_MAX_PAYLOADS)
        {
          HDR_MSG_DRIVERS_1(MSG_LEGACY_ERROR, 
                         "Max packet size (%d) exceeds the max payload", 
                         max_pkt_size_ptr[ch]);
          max_pkt_size_ptr[ch] = HDRRMACCMN_0_BITS;
        }

        /* -TSTMSG- */
        HDR_MSG_DEBUG_6(HDR_MSG_DBG_PS_SELECTION,
          "Channel %d; Max rri %d; Current interlace %d [%d %d %d]", 
          ch,
          max_pkt_size_ptr[ch],
          pkt_interlace,
          hdrrmac4drv.per_carrier_info[ch].interlace_history
            [(pkt_interlace+2) % HDRSCMRMACCMN_MAX_NUM_INTERLACES].tx_rri,
          hdrrmac4drv.per_carrier_info[ch].interlace_history
            [(pkt_interlace+1) % HDRSCMRMACCMN_MAX_NUM_INTERLACES].tx_rri,
          hdrrmac4drv.per_carrier_info[ch].interlace_history
            [(pkt_interlace) % HDRSCMRMACCMN_MAX_NUM_INTERLACES].tx_rri);

      }
      else /* Channel not enabled */
      {
        max_pkt_size_ptr[ch] = HDRRMACCMN_0_BITS;
      }
    } /* for all channels */

  } /* No encoder error */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_get_max_permitted_pkt_size() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_NOTIFY_SUSPEND                                EXTERNAL

DESCRIPTION
  Notifies RTC MAC of upcoming suspend event so that it may ramp down
  RL transmissions if necessary.

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
void hdrrmac4drv_notify_suspend
( 
  boolean cancel_suspend,
    /* TRUE when scheduled suspend is to be cancelled */

  uint32  suspend_slot
    /* Slot when link is scheduled to be suspended. */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cancel_suspend )
  {
    hdrrmac4drv.subframes_to_suspend = HDRRMAC_CANCEL_SUSPEND;
      /* Reset timeline */

    hdrrmac4drv.compute_subframes_to_suspend = FALSE;
      /* Reset flag to compute subframes to suspend */

#ifdef FEATURE_HDR_FLEX_CONN
    if ( hdrrmac4drv_common->is_flexconn_nv_enabled )
    {
      hdrrmac4drv_common->flexconn_disabled_before_tx_stop = FALSE;
        /* Suspend is cancelled. We set the flag to false here. In next subframe
           processing, RMAC will start monitoring conditions for flexconn. If all
           conditions are met, flexconn will be enabled again.
        */
    }
#endif /* FEATURE_HDR_FLEX_CONN */

    HDR_MSG_PROT( MSG_LEGACY_HIGH, "RTC MAC: Suspend cancelled" );
  }
  else
  {
    /* Latch suspend slot and set flag for computing subframes to suspend
       in the next subframe */
    hdrrmac4drv.suspend_slot = suspend_slot;
    hdrrmac4drv.compute_subframes_to_suspend = TRUE;

#ifdef FEATURE_HDR_FLEX_CONN
    if ( hdrrmac4drv_common->is_flexconn_nv_enabled ) 
    {
      hdrrmac4drv_set_flexconn_mode( FALSE );
      hdrrmac4drv_common->flexconn_disabled_before_tx_stop = TRUE;
        /* Disable flexconn at 80 slots before tune away or OFS. SRCH would
           notify RMAC that there is an incoming tune away or OFS with
           this function about 80 slots before tune away. To avoid that
           base station could not receive dsc in two consecutive dsc cycle,
           we disable flexconn here. In worst case we should transmit 40 slots
           of DSC in the last dsc cycle before tune away. This is good enough
           for base state (according to system)
        */
    }
#endif /* FEATURE_HDR_FLEX_CONN */
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_notify_suspend() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_LOG_RAB_UPDATE_INFO                            INTERNAL

DESCRIPTION
  This function fills one subframe entry in the RAB update info log packet
  for the given channel.

DEPENDENCIES
  None.

PARAMETERS
  Number of open channels
  Sector Info
  Search Info

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_log_rab_update_info
( 
  uint8 num_chans,
    /* Number of open channels */

  hdrrmac4drv_channel_sector_record_type * channel_sector_record,
    /* Sector info */

  hdrsrchmac_active_sectors_struct_type  *srch_info
    /* Search info */

)
{
  /* Previous subframe logging enabled status */
  static boolean prev_sf_log_enable_status = FALSE;

  /* Current subframe logging enabled status */
  boolean log_enable_status = FALSE;

  /* Convenience pointers */
  hdrlog_rab_update_subframe_wide_info_type * data;
  hdrrmac4drv_sector_info_type *sector_info;

  /* Loop counters */
  uint8 ch, idx, phy_chan_id, i;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check whether log packet is enabled */
  log_enable_status = log_status(LOG_HDR_MC_RAB_UPDATE_INFO_C);

  if (prev_sf_log_enable_status != log_enable_status)
  {
    /* Submit log packet whenever log status changes from enabled to
       disabled. */
    if (prev_sf_log_enable_status == TRUE)
    {
      hdrrmac4drv_submit_rab_update_info_log();
    }

    /* Update logging status for comparison during in next subframe */
    prev_sf_log_enable_status = log_enable_status;

  }

  /* HDR_MC_RAB_INFO packet enabled, so perform logging. */
  if (log_enable_status)
  {
    /* Error catching */
    ASSERT(( hdrrmac4drv_rab_update_log_info.sf_count < 
      HDRLOG_MAX_SUBFRAMES_PER_REVB_RAB_UPDATE_PKT ) &&
          (channel_sector_record != NULL) &&
          (srch_info != NULL));

    /* Populate channel data entries for this subframe */
    for (ch=0; ch<num_chans; ch++)
    {
      phy_chan_id = channel_sector_record[ch].phy_chan_id;

      /* Tag the channel for logging */
      hdrrmac4drv_rab_update_log_info.enable_channel_logging[phy_chan_id] = 
        TRUE;

      sector_info = channel_sector_record[ch].sector_info;

      data = &hdrrmac4drv_rab_update_log_info.log[phy_chan_id]
        [hdrrmac4drv_rab_update_log_info.sf_count];

      /* Record subframe_n */
      data->subframe_n                 = hdrrmac4drv.subframe_id;

      /* FRAB */
      data->frab                       = sector_info->frab;

      /* sector_info->qrab is 1 or -1 */
      data->bit_info.qrab              = 
        ( sector_info->qrab == HDRRMAC4DRV_QRAB_LOADED ) ? 1 : 0;

      /* sector_info->qrab_ps is 1 or -1 */
      data->bit_info.qrab_ps           = 
        ( sector_info->qrab_ps == 1 ) ? 1 : 0;

      /* Best ASP */
      data->serving_asp                = 
        srch_info[phy_chan_id].best_sector_idx + 1;

      /* Reference subaset flag */
      data->bit_info.reference_subaset = 
        srch_info[phy_chan_id].reference_subaset;

      /* Reset DRCLock */
      data->bit_info.drc_lock_per_asp  = 0;

      /* Reset SlotQRAB */
      data->bit_info.slot_qrab_per_asp = 0;

      for ( i = 0; i < HDRLOG_SUBASET_MAX_SIZE; i++ )
      {
        /* SlotFRAB */
        data->slot_frab_per_asp[ i ]  = 
          srch_info[phy_chan_id].active_sectors[ i ].slot_frab;

        /* Filtered pilot strength */
        data->pilot_strength[ i ]     = hdrrmac4drv.per_carrier_info[
          phy_chan_id].filtered_ps[ i ];

        /* DRCLock */
        data->bit_info.drc_lock_per_asp   |= 
          ( srch_info[phy_chan_id].active_sectors[ i ].drc_lock & 0x1 ) << i;

        /* SlotQRAB */
        data->bit_info.slot_qrab_per_asp  |= 
          ( ( srch_info[phy_chan_id].active_sectors[ i ].slot_qrab == 1 ) ? 1 : 0 ) << i;

      } /* for all subactive sets */

      /* Reserved bits */
      data->bit_info.reserved_bit = 0;

    } /* for all open channnels */

    /* Fill in invalid entries for disabled channels */
    /* RMG: It is assumed here that currently channels statuses maintained
       by RMAC4DRV and RMAC4 are exactly same */
    for ( idx = hdrrmac4drv.num_chans; 
      idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++ )
    {
      ch = hdrrmac4drv.open_chans[idx];

      data = &hdrrmac4drv_rab_update_log_info.log[ch]
        [hdrrmac4drv_rab_update_log_info.sf_count];

      /* Record subframe_n */
      data->subframe_n                 =  hdrrmac4drv.subframe_id;

      /* FRAB */
      data->frab                       = 0;

      /* sector_info->qrab is 1 or -1 */
      data->bit_info.qrab              = 0;

      /* sector_info->qrab_ps is 1 or -1 */
      data->bit_info.qrab_ps           = 0;

      /* Best ASP */
      data->serving_asp                = HDRLOG_SUBASET_MAX_SIZE + 1;

      /* Reset DRCLock */
      data->bit_info.drc_lock_per_asp  = 0;

      /* Reset SlotQRAB */
      data->bit_info.slot_qrab_per_asp = 0;

      for ( i = 0; i < HDRLOG_SUBASET_MAX_SIZE; i++ )
      {
        /* SlotFRAB */
        data->slot_frab_per_asp[ i ]  = 0;

        /* Filtered pilot strength */
        data->pilot_strength[ i ]     = 0;

      } /* for all subactive sets */

      /* Reserved bits */
      data->bit_info.reserved_bit     = 0;

    } /* for all disabled channnels */

    /* Update subframe count and submit log if needed. */
    hdrrmac4drv_rab_update_log_info.sf_count ++;
    if ( hdrrmac4drv_rab_update_log_info.sf_count == 
         HDRLOG_RAB_UPDATE_INFO_SUBFRAMES )
    {
      hdrrmac4drv_submit_rab_update_info_log();
    }

  } /* if logging is enabled */


#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

}  /* hdrrmac4drv_log_rab_update_info() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_SECTOR_INFO                                EXTERNAL

DESCRIPTION
  Returns sector information of the subactive set mapped to the requested
  RL channel.  The mapping is obtained in hdrrmac4drv_update_rl_info().

DEPENDENCIES
  None

PARAMETERS
  num_chans - Number of open channels; records contained in 
    channel_sector_record_ptr  
  channel_sector_record - Points to num_chans instances of channel sector
    record.
    channel_sector_record[].phy_chan_id is populated by the calling
    function
    channel_sector_record[].sector_info points to the location where sector
    info correspoinding to phy_chan_id is returned.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_get_sector_info
(
  /* Number of records contained in channel_sector_record_ptr */
  uint8 num_chans,

  /* Points to num_chans instances of channel_sector_record information */
  hdrrmac4drv_channel_sector_record_type * channel_sector_record
)
{
  hdrsrchmac_active_sectors_struct_type srch_info
    [HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];
    /* Information on sectors in the active set */

  hdrrmac4drv_sector_info_type *sector_info;

  uint16 new_val;

  uint8 phy_chan_id, ch, i;
    /* Loop counters */

  int16 max_frab_s;
    /* max(FRABs) across all sectors 's' including both in-lock
       and out-of-lock sectors */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(channel_sector_record != NULL);

  for (ch=0; ch<num_chans; ch++)
  {
    phy_chan_id = channel_sector_record[ch].phy_chan_id;

    sector_info = channel_sector_record[ch].sector_info;

    /* Init QRAB and QRABps to not busy (unloaded) */
    sector_info->qrab_ps = -1;
    sector_info->qrab = HDRRMAC4DRV_QRAB_UNLOADED;

    /* Init max(FRABs) to the the minimum value in the range */
    max_frab_s = -32768;

    /* Get pilot strenth, QRABn,s, and DRCLock per sector from HDR SRCH */
    /* hdrsrchmac_get_mac_subaset_info() is abscent in 7500 builds since 
       RevB search changes are not included in 7500 builds. */
    hdrsrchmac_get_mac_aset_info( 
      hdrrmac4drv.per_carrier_info[phy_chan_id].fwd_link_index, 
      &srch_info[phy_chan_id] ); 

    /* Loop through each sector */
    for (i=0; i<HDRSRCH_ASET_MAX_SIZE; i++)
    {
      /* Only look at valid ASPs */
      if (srch_info[phy_chan_id].active_sectors[i].pn_offset != 
          HDR_NO_PN_OFFSET)
      {

        /* Filter pilot strength from each active set pilot */
        new_val = srch_info[phy_chan_id].active_sectors[i].pilot_strength;

        /* If the PN for this ASP has not changed, run filter with new value */
        if (srch_info[phy_chan_id].active_sectors[i].pn_offset == 
            hdrrmac4drv.per_carrier_info[phy_chan_id].pn_offset[i])
        {
          /* Filter equation is:
              new_value       prev_value * (tc-1)
             -----------  +  ---------------------
                tc                    tc

             In the code, ps_filer_mult = tc-1; 
                          2^ps_filter_shift = tc (ps_filter_shift = log2 tc)
          */
          hdrrmac4drv.per_carrier_info[phy_chan_id].filtered_ps[i] = (uint16)
            (new_val + (hdrrmac4drv.per_carrier_info[phy_chan_id].
            filtered_ps[i] * hdrrmac4drv.ps_filter_mult))
            >> hdrrmac4drv.ps_filter_shift;
        }
        else
        {
          /* New ASP; initialize to the only valid value we have. */
          hdrrmac4drv.per_carrier_info[phy_chan_id].filtered_ps[i] = new_val;
        }

        /* Set QRAB if *any* sector is busy */
        if (srch_info[phy_chan_id].active_sectors[i].slot_qrab == 1)
        {
          sector_info->qrab = HDRRMAC4DRV_QRAB_LOADED;
        }

        /* Determine QRABps */
        if ((srch_info[phy_chan_id].active_sectors[i].slot_qrab == 1) &&
            ((i == srch_info[phy_chan_id].best_sector_idx) || 
             ((srch_info[phy_chan_id].active_sectors[i].drc_lock == 0) && 
              (hdrrmac4drv.per_carrier_info[phy_chan_id].filtered_ps[i] > 
               hdrrmac4drv.per_carrier_info[phy_chan_id].drc_unlock_threshold))
              ||
             ((srch_info[phy_chan_id].active_sectors[i].drc_lock == 1) && 
              (hdrrmac4drv.per_carrier_info[phy_chan_id].filtered_ps[i] > 
               hdrrmac4drv.per_carrier_info[phy_chan_id].drc_lock_threshold))))
        {
          sector_info->qrab_ps = 1;
        }

        /* If current ASP's FRAB is greater than current Max, Set max to 
           current sector's FRAB */
        max_frab_s = MAX(max_frab_s, 
          srch_info[phy_chan_id].active_sectors[i].slot_frab);

      } /* If this ASP is valid  */
      else
      {
        /* Pilot not in aset; clear pilot strength */
        hdrrmac4drv.per_carrier_info[phy_chan_id].filtered_ps[i] = 0;
      }

      /* Remember PN for each ASP in order to detect changes. */
      hdrrmac4drv.per_carrier_info[phy_chan_id].pn_offset[i] = 
        srch_info[phy_chan_id].active_sectors[i].pn_offset;

    } /* For each ASP */

    /* Remember max(FRABs) for later use */
    sector_info->frab = max_frab_s;

    /* Pilot strength is converted to signed Q.2 */
    hdrrmac4drv.per_carrier_info[phy_chan_id].best_sector_db = 
      hdrsrchmac_eng_to_ecio(hdrrmac4drv.per_carrier_info[phy_chan_id].
      filtered_ps[srch_info[phy_chan_id].best_sector_idx]);

    /* Compute PilotStrength(pilotStrength). All inputs are in Q.20. */
    sector_info->pilot_strength = hdrrmacmath_interpolate_1d(
      hdrrmac4drv.per_carrier_info[phy_chan_id].best_sector_db << 18, 
      hdrrmac4drv.per_carrier_info[phy_chan_id].channel_config->
      pilotstrength_ptr->ps_axis,
      hdrrmac4drv.per_carrier_info[phy_chan_id].channel_config->
      pilotstrength_ptr->pilotstrength_ps_axis,
      hdrrmac4drv.per_carrier_info[phy_chan_id].channel_config->
      pilotstrength_ptr->num_ps_axis_values);

    /* Best sector DeltaT2P in dB/4 */
    hdrrmac4drv.per_carrier_info[phy_chan_id].best_sector_delta_t2p = 
      srch_info[phy_chan_id].active_sectors[srch_info[phy_chan_id].
      best_sector_idx].delta_t2p;

    HDR_MSG_DRIVERS_1(MSG_LEGACY_MED, "DeltaT2P: %d", 
      hdrrmac4drv.per_carrier_info[phy_chan_id].best_sector_delta_t2p);

    HDR_MSG_DEBUG_5( HDR_MSG_DBG_RAB,
      "--RAB-- FRAB: %d QRAB: %d QRABps: %d PS: %d dB/4 (%d)",
      sector_info->frab,
      sector_info->qrab,
      sector_info->qrab_ps,
      hdrrmac4drv.per_carrier_info[phy_chan_id].best_sector_db,
      hdrrmac4drv.per_carrier_info[phy_chan_id].filtered_ps[
      srch_info[phy_chan_id].best_sector_idx]);

  }

  /* Log RAB update info for one sub frame and 
     log commit if more that 8 sub frames have been logged*/
  hdrrmac4drv_log_rab_update_info( num_chans, 
    channel_sector_record,
    srch_info );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_get_sector_info()*/


/*===========================================================================
FUNCTION HDRRMAC4DRV_SORT_CAPUT_CARRIERS                            INTERNAL

DESCRIPTION
  Sorts carriers based on their passed CAPUT priorities.  In case of a tie,
  one of the tied carriers would be randomly given a higher priority.
  
DEPENDENCIES
  None.

PARAMETERS
  metric      - Pointer to MAX_NUM_PHY_CHANS instances of carrier priorities
                arranged in linear order.
  sorted_list - Pointer to a uint8 array of size MAX_NUM_PHY_CHANS.  The 
                entries will be filled by this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_sort_caput_carriers
(
  uint8  *metric,
  uint8  *sorted_list
)
{
  uint8 ch, ch_i, ch_j, ch_temp, rand_carrier_list_idx;
  boolean sorted = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Randomly pick one of the six possible carrier arrangements as the 
     initial carrier arrangement in sorted_list[] */
  rand_carrier_list_idx = ran_dist( hdrutil_get_random(), 0, 6 );

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    sorted_list[ch] = hdrrmac4drv_carrier_arrangement
      [rand_carrier_list_idx][ch];
  }

  /* Sort carriers according to the input metric in ascending order */
  for (ch_i=1; (!sorted && (ch_i<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS));
    ch_i++)
  {
    sorted = TRUE;

    for (ch_j=0; ch_j<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS - ch_i; ch_j++)
    {
      /* Compare metric[] values and switch sorted carrier positions if 
         necessary. */
      if ( metric[sorted_list[ch_j]] < metric[sorted_list[ch_j + 1]] )
      {
        ch_temp = sorted_list[ch_j];
        sorted_list[ch_j] = sorted_list[ch_j + 1];
        sorted_list[ch_j + 1] = ch_temp;
        sorted = FALSE;
      }

    } /* bubble sort inner loop */

  } /* bubble sort outer loop */

} /* hdrrmac4drv_sort_caput_carriers */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SORT_CARRIERS                                  INTERNAL

DESCRIPTION
  Sorts carriers based on their passed priority metrics.
  
DEPENDENCIES
  None.

PARAMETERS
  metric      - Pointer to MAX_NUM_PHY_CHANS instances of carrier priorities
                arranged in linear order.
  sorted_list - Pointer to a uint8 array of size MAX_NUM_PHY_CHANS.  The 
                entries will be filled by this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

LIMITATIONS
  Designed to just handle three carrier sorting.  This is not a generic
  sort function.
===========================================================================*/
void hdrrmac4drv_sort_carriers
(
  uint8  *metric,
  uint8  *sorted_list
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sort carriers according to the metric */
  if (metric[0] > metric[1])
  {
    if (metric[0] > metric[2])
    {
      if (metric[1] > metric[2])
      {
        sorted_list[0] = 0;
        sorted_list[1] = 1;
        sorted_list[2] = 2;
      }
      else
      {
        sorted_list[0] = 0;
        sorted_list[1] = 2;
        sorted_list[2] = 1;
      }
    }
    else
    {
      sorted_list[0] = 2;
      sorted_list[1] = 0;
      sorted_list[2] = 1;
    }
  }
  else
  {
    if (metric[1] > metric[2])
    {
      if (metric[0] > metric[2])
      {
        sorted_list[0] = 1;
        sorted_list[1] = 0;
        sorted_list[2] = 2;
      }
      else
      {
        sorted_list[0] = 1;
        sorted_list[1] = 2;
        sorted_list[2] = 0;
      }
    }
    else
    {
      sorted_list[0] = 2;
      sorted_list[1] = 1;
      sorted_list[2] = 0;
    }
  }

} /* hdrrmac4drv_sort_carriers() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_RTCMAC_PRIORITIES                       INTERNAL

DESCRIPTION
  Updates the list of RTCMAC prioritized carriers based on their
  given filtered pilot powers.
  
DEPENDENCIES
  None.

PARAMETERS
  filt_txp - pointer to HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS instances of
             filtered transmit pilot power in Q.10 dBm.
  prioritized_carriers
           - pointer to table containing RTCMAC prioritized carriers.  The
             table will be updated by this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

LIMITATIONS
  Designed to just handle three carrier RTCMAC priority update.
===========================================================================*/
void hdrrmac4drv_update_rtcmac_priorities
(
  int32  *filt_txp,
  uint8  *prioritized_carriers
)
{
  /* Delta applied to filtered transmit pilot power when computing
     RTCMAC priority. Units: Q.10 dB */
  int32  filt_txp_delta;

  /* Previous subframe RTCMAC prioritized carrier list. */
  uint8  prev_sf_carrier_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Priority order change with respect to previous subframe carrier list. */
  uint8  carrier_order_change[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]={0,1,2};

  /* Priority switch number */
  uint8  k=0;

  /* Loop counter */
  uint8  ch;

  /* Adjusted filtered transmit pilot power values */
  int32  filt_txp_adj[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    /* Copy previous subframe prioritized carrier list */
    prev_sf_carrier_list[ch] = prioritized_carriers[ch];

    /* Populate adjusted filtered transmit power */
    if ( hdrrmac4drv.enh_pa_backoff_enabled )
    {
      /* Apply funnel bias if enhanced PA backoff is enabled */
      filt_txp_adj[ch] = filt_txp[ch] + 
        hdrrmac4drv.per_carrier_info[ch].funnel_bias;
    }
    else
    {
      filt_txp_adj[ch] = filt_txp[ch];
    }
  }

  /* Obtain filt_txp_delta value Q.10 dB units */
  filt_txp_delta = HDRRMAC4DRV_FILT_TXP_DELTA;

#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
  if (hdrrmac4drv_headroom_ut_enable)
  {
    /* Overwrite filt_txp_delta with UT FiltTxPDelta */
    filt_txp_delta = hdrrmac4drv_ut_filt_txp_delta;
  }
#endif

  if ( filt_txp_adj[prev_sf_carrier_list[0]] > 
    filt_txp_adj[prev_sf_carrier_list[1]] + filt_txp_delta )
  {
    carrier_order_change[0]++;
    carrier_order_change[1]--;
  }
  else
  {
    k++;
  }

  if ( filt_txp_adj[prev_sf_carrier_list[1]] > 
    filt_txp_adj[prev_sf_carrier_list[2]] + filt_txp_delta )
  {
    carrier_order_change[1]++;
    carrier_order_change[2]--;
  }
  else
  {
    k++;
  }

  if ( filt_txp_adj[prev_sf_carrier_list[0]] > 
    filt_txp_adj[prev_sf_carrier_list[2]] + filt_txp_delta )
  {
    carrier_order_change[0]++;
    carrier_order_change[2]--;

    if (k==2)
    {
      carrier_order_change[1]++;
      carrier_order_change[2]--;
    }
  }

  /* Update prioritized carrier list */
  for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    prioritized_carriers[carrier_order_change[ch]] = 
      prev_sf_carrier_list[ch];
  }

} /* hdrrmac4drv_update_rtcmac_priorities() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SET_SLP_CARRIER                                INTERNAL

DESCRIPTION
  Sets the current SLP carrier to first DATA_ACTIVE carrier the overall
  prioritized carrier list.
  
DEPENDENCIES
  None.

PARAMETERS
  pri_carrier_list - List of overall prioritized carriers.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_set_slp_carrier
(
  uint8  *pri_carrier_list
)
{
  /* Loop counter */
  uint8 chan;

  /* SLP candidate */
  uint8 new_slp;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* If 
     - SLP has_data flag is set
       AND
       ~ The current SLP carrier is not in DATA_ACTIVE state
         OR
       ~ The current SLP carrier is under DRC supervision
     reset SLP has_data flag to force SLP carrier switch.
     This is expected to happen when one of the following happens while there
     is data in SLP queue:
     1. The current SLP carrier is removed by TCA
     2. The current SLP carrier is removed due to DRC supervision failure
     3. The current SLP carrier is removed due to transmit pilot power differential
     4. The current SLP carrier enters DRC supervision */
  if ( ( hdrrmac4drv.slp_carrier.has_data ) && 
       ( (hdrrmac4drv.per_carrier_info[hdrrmac4drv.slp_carrier.current].
          carrier_state != HDRRMAC4DRV_CH_ST_DATA_ACTIVE) ||
         (hdrrmac4drv.per_carrier_info[hdrrmac4drv.slp_carrier.current].
          drc_supervision_expired == TRUE) ) )
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "Forcing SLP carrier switch..." );

    hdrrmac4drv.slp_carrier.has_data = FALSE;
  }

  /* Set the current SLP carrier if the carrier does not have data to be 
     transmitted in this subframe */
  if (!hdrrmac4drv.slp_carrier.has_data)
  {
    /* Pick a new SLP carrier */
    for (chan=0; chan < hdrrmac4drv.num_chans; chan++)
    {
      new_slp = pri_carrier_list[chan];

      if ( (hdrrmac4drv.per_carrier_info[new_slp].
        carrier_state == HDRRMAC4DRV_CH_ST_DATA_ACTIVE) && 
        !(hdrrmac4drv.per_carrier_info[new_slp].drc_supervision_expired) &&
        !(hdrrmac4drv.per_carrier_info[new_slp].drop_carrier_txp_diff) )
      {
        hdrrmac4drv.slp_carrier.current = new_slp;

        /* break from the for loop */
        break;
      }
    }

    if (chan == hdrrmac4drv.num_chans)
    {
      HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
        "SLP Selection Error: No data allowed RL carrier to select!" );
    }

  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

} /* hdrrmac4drv_set_slp_carrier() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_CARRIER_PRIORITIES                      INTERNAL

DESCRIPTION
  The function 
  - Updates carrier priorities
    -----------------------------------------------------------------
    | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
    -----------------------------------------------------------------
    |       |       |       | Not   | Not   |Report-|       |       |
    |  Overall priority     | used  | used  |able   |RTCMAC priority|
    |       |       |       | (0)   | (0)   |status |       |       |
    -----------------------------------------------------------------
  - Writes updated overall carrier priorities as DSP carrier ranks
  - Triggers SLP carrier update
  
DEPENDENCIES
  None.

PARAMETERS
  filt_txp_dbm - pointer to HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS instances of
                 filtered transmit pilot power in Q.10 dBm.
  carriers_ordered_by_rank - pointer to location where list of carriers 
                 ordered by overall priority is to be populated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_carrier_priorities
(
  int32  * filt_txp_dbm,
  uint16 * carriers_ordered_by_rank
)
{
  /* RTCMAC priorities.  The array elements are uint32 array for the 
     sort function */
  uint8  rtcmac_carrier_priorities[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Overall carrier priorities  The array elements are uint32 array for
     the sort function */
  uint8 overall_carrier_priorities[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Priority based on carrier reportable status and number of reportable
     carriers */
  uint8 reportable_carrier_priority;

  /* Loop counters */
  uint8  i, ch;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Find RTCMAC prioritized carriers */
  hdrrmac4drv_update_rtcmac_priorities( filt_txp_dbm,
    hdrrmac4drv.prioritized_carriers.rtcmac );

  /* Calculate 2-bit (for 3 carriers) RTCMAC carrier priorities and 
     8-bit overall carrier priorities.  The 8-bit overall carrier 
     priority can be directly written as DSP_drop_rank */
  for (i=0; i < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; i++)
  {
    /* Reset reportable_carrier_priority */
    reportable_carrier_priority = 0;

    ch = hdrrmac4drv.prioritized_carriers.rtcmac[i];

    if (( ((uint16)hdrrmac4drv.per_carrier_info[ch].carrier_state) & 
      ((uint16) HDRRMAC4DRV_CH_ST_MAJOR_MASK) ) == 
      ((uint16) HDRRMAC4DRV_CH_ST_OPEN))
    {
      /* For assigned carriers, min(RTCMAC_pri) = 1
         For unassigned carriers, RTCMAC_pri = 0 */
      rtcmac_carrier_priorities[ch] = HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS - i;

      if (hdrrmac4drv.per_carrier_info[ch].carrier_state == 
        HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE)
      {
        /* Give lowest dropping rank (0) to TO_BE_INACTIVE carriers and
           compute overall priority */
        overall_carrier_priorities[ch] = rtcmac_carrier_priorities[ch];
      }
      else
      {
        if (hdrrmac4drv.num_reportable_carriers == 1)
        {
          if (hdrrmac4drv.per_carrier_info[ch].reportable)
          {
            /* Last reportable carrier => give higher priority */
            reportable_carrier_priority = 1;
          }
        }
        else if (hdrrmac4drv.num_reportable_carriers != 0)
        {
          /* More than one reportable carriers
             => give lower priority to reportable carriers
             i.e. give higher priority to non-reportable carriers */
          if (!hdrrmac4drv.per_carrier_info[ch].reportable)
          {
            reportable_carrier_priority = 1;
          }
        }

        /* Compute overall priority - refer diagram in function header */
        overall_carrier_priorities[ch] = 
          ( hdrrmac4drv.per_carrier_info[ch].drop_rank << 5 ) | 
          ( reportable_carrier_priority << 2 ) |
          rtcmac_carrier_priorities[ch];
      }
    }
    else
    {
      /* Assign lowest priorities to INACTIVE carriers */
      rtcmac_carrier_priorities[ch] = 0;
      overall_carrier_priorities[ch] = 0;
    }

  }

  /* Find overall prioritized carriers */
  hdrrmac4drv_sort_carriers( overall_carrier_priorities, 
    hdrrmac4drv.prioritized_carriers.overall );

  ASSERT( hdrrmac4drv.num_chans <= HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );

  /* Populate list of carriers ordered by ranks */
  for (i=0; i<hdrrmac4drv.num_chans; i++)
  {
    /* Look up the carrier index starting with the least priority carrier */
    ch = hdrrmac4drv.prioritized_carriers.overall[hdrrmac4drv.num_chans-1-i];

    /* Populate carriers in ascending order of their priorities */
    carriers_ordered_by_rank[i] = ch;
  }


  /* Specify the highest overall priority carrier as the eligible SLP 
     carrier.  The SLP carrier is set to this carrier if there is no 
     ongoing transmission on the carrier */
  hdrrmac4drv_set_slp_carrier( hdrrmac4drv.prioritized_carriers.overall );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

}  /* hdrrmac4drv_update_carrier_priorities() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_FIND_UP_POW                                    INTERNAL

DESCRIPTION
  The function returns power required to support a configuration one 
  step higher than the current configuration.
  
DEPENDENCIES
  None.

PARAMETERS
  curr_config - Current configuration.
  pilot_pow   - Points to MAX_NUM_PHY_CHANS instances of power margin 
                corrected transmit pilot powers.

RETURN VALUE
  Mininum transmit power required to support next up configuration.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hdrrmac4drv_find_up_pow
(
  /* Current configuration */
  hdrrmac4drv_carrier_config_id_enum_type curr_config,

  /* Points to MAX_NUM_PHY_CHANS instances of power margin corrected 
     transmit pilot powers */
  uint32 * pilot_pow
)
{
  /* Mininum transmit power required to support next up configuration */
  uint32 tx_pwr_min = 0; 

  /* Power calculation intermediate result */
  uint32 pwr = 0; 

  /* Maximum pilot plus overhead gains among assigned channels */
  uint32 max_pilot_plus_overhead_gains = 0;

  /* T2PRemainActive */
  uint32 t2p_remain_active;

  /* Loop counters */
  uint8 ch, phy_chan_id;

  /* Carrier state */
  hdrrmac4drv_carrier_state_enum_type carrier_state;

  /* Flag to indicate that the carrier holds the highest priority among
     inactive/dropped carriers and hence is eligible for activation */
  boolean carrier_eligible_for_activation = TRUE;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrrmac4drv_carrier_config[curr_config].next_up_config.data_funneling)
  {
    /* Reverse funneling - the next up state enables data transmission on one
       of the carriers on which we are currently transmitting only overhead 
       channels.  The eligible carrier is determined by RTCMAC priorities. */

    for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      phy_chan_id = hdrrmac4drv.prioritized_carriers.rtcmac[ch];

      carrier_state = hdrrmac4drv.per_carrier_info[phy_chan_id].carrier_state;

      if ( carrier_state == HDRRMAC4DRV_CH_ST_DATA_ACTIVE )
      {
        /* Calculate T2PRemainActive */
        t2p_remain_active = MAX( 
          hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate,
          ( ( ( ( (((uint64) 1) << 32) / pilot_pow[phy_chan_id] ) * 
          hdrrmac4drv.per_carrier_info[phy_chan_id].filt_rab_limited_p_data )
          + (((uint32)1) << 21) )
          >> 22 ) );

        t2p_remain_active = MIN( 
          hdrrmac4drv_carrier_mgt_thresh.t2p_remain_active, 
          t2p_remain_active );

        /* Power required to maintain an active carrier that is not 
             a candidate for deactivation */
        pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, 
          (((((uint64) pilot_pow[phy_chan_id]) * 
          ( hdrrmac4drv.per_carrier_info[phy_chan_id].
          pilot_plus_overhead_gains + t2p_remain_active )) + 
          (((uint32)1) << 9))
          >> 10 ) );

        tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

        HDR_MSG_DEBUG_5( HDR_MSG_DBG_PS_SELECTION, 
          "RevFunneling: Data_Active carrier [%d] %u,%u,%u,pwr: %u",
          phy_chan_id,
          pilot_pow[phy_chan_id],
          hdrrmac4drv.per_carrier_info[phy_chan_id].pilot_plus_overhead_gains,
          t2p_remain_active,
          pwr );

      }
      else if ( ( carrier_state == HDRRMAC4DRV_CH_ST_DATA_INACTIVE ) || 
        ( carrier_state == HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE ) )
      {
        if (carrier_eligible_for_activation)
        {
          /* This is the inactive carrier with highest RTCMAC priority.  
             Hence data transmission will be enabled on this carrier 
             in case of reverse funneling. */

          /* Minimum power required to enable data on this carrier. */
          pwr = (uint32) MIN( (uint64) 0xFFFFFFFF,
            (((((uint64) pilot_pow[phy_chan_id]) * 
            ( hdrrmac4drv.per_carrier_info[phy_chan_id].
            pilot_plus_overhead_gains + 
            hdrrmac4drv_carrier_mgt_thresh.t2p_activate )) + 
            (((uint32)1) << 9))
            >> 10 ) );

          tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

          HDR_MSG_DEBUG_5( HDR_MSG_DBG_PS_SELECTION, 
            "RevFunneling: Carrier Elig for activation [%d] %u,%u,%u,pwr: %u",
            phy_chan_id,
            pilot_pow[phy_chan_id],
            hdrrmac4drv.per_carrier_info[phy_chan_id].pilot_plus_overhead_gains,
            hdrrmac4drv_carrier_mgt_thresh.t2p_activate,
            pwr );

          carrier_eligible_for_activation = FALSE;
        }
        else
        {
          /* Power consumed by an inactive carrier */
          pwr = (uint32) MIN( (uint64) 0xFFFFFFFF,
            (((((uint64) pilot_pow[phy_chan_id]) * 
            ( hdrrmac4drv.per_carrier_info[phy_chan_id].
            pilot_plus_overhead_gains )) + 
            (((uint32)1) << 9))
            >> 10 ) );

          tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

          HDR_MSG_DEBUG_4( HDR_MSG_DBG_PS_SELECTION, 
            "RevFunneling: Data_Inactive carrier [%d] %u,%u,pwr: %u",
            phy_chan_id,
            pilot_pow[phy_chan_id],
            hdrrmac4drv.per_carrier_info[phy_chan_id].pilot_plus_overhead_gains,
            pwr );

        }
      }
    }

    HDR_MSG_DEBUG_1( HDR_MSG_DBG_PS_SELECTION, 
      "Reverse Funneling power reqd: %d",
      tx_pwr_min );

  }
  else
  {
    /* Carrier addition - the next up state enables data transmission on one
       of the dropped carriers.  The eligible carrier is determined by 
       overall priorities. */

    if ((hdrrmac4drv.carrier_config_id == CARRIER_CONFIG_P0_D0) && 
        (curr_config == CARRIER_CONFIG_P01_D01))
    {
      /* Special case: 2nd carrier addition from 
         carrier config # 5 [CARRIER_CONFIG_P0_D0] */

      for (ch = 0; ch < hdrrmac4drv_carrier_config[CARRIER_CONFIG_P0_D0].
           num_carriers; ch++)
      {
        phy_chan_id = hdrrmac4drv.prioritized_carriers.overall[ch];

        carrier_state = hdrrmac4drv.per_carrier_info[phy_chan_id].carrier_state;

        if ( carrier_state == HDRRMAC4DRV_CH_ST_DATA_ACTIVE )
        {
          /* Power required to maintain minimum data rate on an active
             carrier */
          pwr = (uint32) MIN( (uint64) 0xFFFFFFFF,
            (((((uint64) pilot_pow[phy_chan_id]) * 
            ( hdrrmac4drv.per_carrier_info[phy_chan_id].
            pilot_plus_overhead_gains + 
            hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate )) + 
            (((uint32)1) << 9))
            >> 10 ) );

          tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

          HDR_MSG_DEBUG_5( HDR_MSG_DBG_PS_SELECTION, 
            "Add: Data_Active carrier [%d] %u,%u,%u,pwr: %u",
            phy_chan_id,
            pilot_pow[phy_chan_id],
            hdrrmac4drv.per_carrier_info[phy_chan_id].pilot_plus_overhead_gains,
            hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate,
            pwr );

          /* Maximum of pilot plus overhead gains of active carriers.
             This is used as pilot plus overhead gain of the unassigned
             carrier for transmit power calculation. */
          max_pilot_plus_overhead_gains = MAX( max_pilot_plus_overhead_gains,
            hdrrmac4drv.per_carrier_info[phy_chan_id].
            pilot_plus_overhead_gains );
        }
        /* We shouldn't have any DATA_INACTIVE carriers in this state.  
           If we did, we would have opted for reverse funneling and not 
           carrier addition. */
      }

      /* Look up physical carrier id of to-be-added carrier.  
         i.e. If there are n carriers in the current configuration, 
         look up entry # n+1 in overall prioritized carrier list. 
         One entry beyond the number of allocated carriers is being accessed
         here.  hdrrmac4drv_carrier_config[P0_D0].num_carriers is hard-coded 
         to '1'.  This will not cause out of array bounds access */
      phy_chan_id = 
        hdrrmac4drv.prioritized_carriers.overall[
        hdrrmac4drv_carrier_config[CARRIER_CONFIG_P0_D0].num_carriers];

      /* Minimum power required for adding two additional carriers */
      pwr = (uint32) MIN( (uint64) 0xFFFFFFFF,
        ((((((uint64) pilot_pow[phy_chan_id]) * 
        ( max_pilot_plus_overhead_gains + 
        hdrrmac4drv_carrier_mgt_thresh.t2p_add )) + 
        (((uint32)1) << 9))
        >> 10 ) * 2) );

      tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

      HDR_MSG_DEBUG_1( HDR_MSG_DBG_PS_SELECTION, 
        "Add: Pwr to add 2 carriers: %u",
        pwr );

    }
    else
    {
      for (ch = 0; ch < hdrrmac4drv_carrier_config[curr_config].num_carriers;
           ch++)
      {
        phy_chan_id = hdrrmac4drv.prioritized_carriers.overall[ch];

        carrier_state = hdrrmac4drv.per_carrier_info[phy_chan_id].carrier_state;

        if ( carrier_state == HDRRMAC4DRV_CH_ST_DATA_ACTIVE )
        {
          /* Power required to maintain minimum data rate on an active
             carrier */
          pwr = (uint32) MIN( (uint64) 0xFFFFFFFF,
            (((((uint64) pilot_pow[phy_chan_id]) * 
            ( hdrrmac4drv.per_carrier_info[phy_chan_id].
            pilot_plus_overhead_gains + 
            hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate )) + 
            (((uint32)1) << 9))
            >> 10 ) );

          tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

          HDR_MSG_DEBUG_5( HDR_MSG_DBG_PS_SELECTION, 
            "Add: Data_Active carrier [%d] %u,%u,%u,pwr: %u",
            phy_chan_id,
            pilot_pow[phy_chan_id],
            hdrrmac4drv.per_carrier_info[phy_chan_id].pilot_plus_overhead_gains,
            hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate,
            pwr );

          /* Maximum of pilot plus overhead gains of active carriers.
             This is used as pilot plus overhead gain of the unassigned
             carrier for transmit power calculation. */
          max_pilot_plus_overhead_gains = MAX(max_pilot_plus_overhead_gains, 
            hdrrmac4drv.per_carrier_info[phy_chan_id].
            pilot_plus_overhead_gains);
        }
        /* We shouldn't have any DATA_INACTIVE carriers in this state.  
           If we did, we would have opted for reverse funneling and not 
           carrier addition. */
      }

      /* If we are adding carriers, 
         we must have less that max allowed carriers */
      ASSERT(hdrrmac4drv_carrier_config[curr_config].num_carriers < 
             HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS);

      /* Look up physical carrier id of to-be-added carrier.  
         i.e. If there are n carriers in the current configuration, 
         look up entry # n+1 in overall prioritized carrier list. */
      phy_chan_id = 
        hdrrmac4drv.prioritized_carriers.overall[
        hdrrmac4drv_carrier_config[curr_config].num_carriers];

      /* Minimum power required for adding an additional carrier */
      pwr = (uint32) MIN( (uint64) 0xFFFFFFFF,
        (((((uint64) pilot_pow[phy_chan_id]) * 
        ( max_pilot_plus_overhead_gains + 
        hdrrmac4drv_carrier_mgt_thresh.t2p_add )) + 
        (((uint32)1) << 9))
        >> 10 ) );

      tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

      HDR_MSG_DEBUG_1( HDR_MSG_DBG_PS_SELECTION, 
        "Add: Pwr to add 1 carrier: %u",
        pwr );

    }

    HDR_MSG_DEBUG_1( HDR_MSG_DBG_PS_SELECTION, 
      "Carrier addition power reqd: %d",
      tx_pwr_min );

  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return tx_pwr_min;

} /* hdrrmac4drv_find_up_pow() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_FIND_CURR_POW                                  INTERNAL

DESCRIPTION
  The function returns the power required to sustain current configuration.
  
DEPENDENCIES
  None.

PARAMETERS
  curr_config - Current configuration.
  pilot_pow   - Points to MAX_NUM_PHY_CHANS instances of power margin 
                corrected transmit pilot powers.

RETURN VALUE
  Mininum transmit power required to support current configuration.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hdrrmac4drv_find_curr_pow
(
  /* Current configuration */
  hdrrmac4drv_carrier_config_id_enum_type curr_config,

  /* Points to MAX_NUM_PHY_CHANS instances of power margin corrected 
     transmit pilot powers */
  uint32 * pilot_pow
)
{
  /* Mininum transmit power required to support current configuration */
  uint32 tx_pwr_min = 0; 

  /* Power calculation intermediate result */
  uint32 pwr = 0; 

  /* T2PRemainActive */
  uint32 t2p_remain_active;

  /* Loop counters */
  uint8 ch;
  int8  chan;

  /* Carrier state */
  hdrrmac4drv_carrier_state_enum_type carrier_state;

  /* The carrier with least priority */
  uint8 least_pri_carrier = HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;

  /* Adjustment to be applied to power calculation for carrier drop 
     if enhanced PA backoff is enabled */
  uint32 pwr_adjust = 0;

  /* Power adjust intermediate value */
  int64  pwr_adjust_int = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrrmac4drv_carrier_config[curr_config].next_down_config.data_funneling)
  {
    /* Data funneling - the next down state disables data transmission on 
       the carriers with minimum RTCMAC priority. */

    /* Find the candidate for data-deactivation */
    for (chan=HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS-1; chan>=0; chan--)
    {
      if ( hdrrmac4drv.per_carrier_info[
        hdrrmac4drv.prioritized_carriers.rtcmac[chan]].carrier_state == 
        HDRRMAC4DRV_CH_ST_DATA_ACTIVE )
      {
        /* This is the least priority DATA_ACTIVE carrier.  Hence data will 
           be disabled on this carrier in case of data funneling. */
        least_pri_carrier = hdrrmac4drv.prioritized_carriers.rtcmac[chan];

        break;
      }
    }

    /* Error catching */
    ASSERT(least_pri_carrier != HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS);

    for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      /* Obtain carrier state */
      carrier_state = hdrrmac4drv.per_carrier_info[ch].carrier_state;

      if ( carrier_state == HDRRMAC4DRV_CH_ST_DATA_ACTIVE )
      {
        if (ch == least_pri_carrier)
        {
          /* This is the last DATA_ACTIVE carrier in the current carrier 
             configuration.  Hence data will be disabled on this carrier
             in case of data funneling. */

          /* Minimum power required to maintain this carrier in 
             active state */
          pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, 
            (((((uint64) pilot_pow[ch]) * 
            ( hdrrmac4drv.per_carrier_info[ch].
            pilot_plus_overhead_gains + 
            hdrrmac4drv_carrier_mgt_thresh.t2p_deactivate )) + 
            (((uint32)1) << 9))
            >> 10 ) );

          tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

          HDR_MSG_DEBUG_5( HDR_MSG_DBG_PS_SELECTION, 
            "Funneling: Data_Inactive_Candidate carrier [%d] %u,%u,%u,pwr: %u",
            ch,
            pilot_pow[ch],
            hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains,
            hdrrmac4drv_carrier_mgt_thresh.t2p_deactivate,
            pwr );

        }
        else
        {
          /* Calculate T2PRemainActive */
          t2p_remain_active = MAX( 
            hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate,
            ( ( ( ( (((uint64) 1) << 32) / pilot_pow[ch] ) * 
            hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data ) + 
            (((uint32)1) << 21) ) >> 22 ) );

          t2p_remain_active = MIN( 
            hdrrmac4drv_carrier_mgt_thresh.t2p_remain_active, 
            t2p_remain_active );

          /* Power required to maintain an active carrier that is not 
             a candidate for deactivation */
          pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, 
            (((((uint64) pilot_pow[ch]) * 
            ( hdrrmac4drv.per_carrier_info[ch].
            pilot_plus_overhead_gains + t2p_remain_active )) + 
            (((uint32)1) << 9))
            >> 10 ) );

          tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

          HDR_MSG_DEBUG_5( HDR_MSG_DBG_PS_SELECTION, 
            "Funneling: Data_Active carrier [%d] %u,%u,%u,pwr: %u",
            ch,
            pilot_pow[ch],
            hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains,
            t2p_remain_active,
            pwr );

        }
      }
      else if ( ( carrier_state == HDRRMAC4DRV_CH_ST_DATA_INACTIVE ) || 
        ( carrier_state == HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE ) )
      {
        /* Power consumed by an data inactive carrier */
        pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, 
          (((((uint64) pilot_pow[ch]) * 
          ( hdrrmac4drv.per_carrier_info[ch].
          pilot_plus_overhead_gains )) + 
          (((uint32)1) << 9) )
          >> 10 ) );

        tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

        HDR_MSG_DEBUG_4( HDR_MSG_DBG_PS_SELECTION, 
          "Funneling: Data_Inactive carrier [%d] %u,%u,pwr: %u",
          ch,
          pilot_pow[ch],
          hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains,
          pwr );

      }
    }

    HDR_MSG_DEBUG_1( HDR_MSG_DBG_PS_SELECTION, 
      "Next down is funneling.  Pwr reqd to maintain current state: %d",
      tx_pwr_min );

  }
  else
  {
    /* Carrier drop - the next down state drops the carrier with minimum
       overall priority. */

    for (ch = 0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      /* Obtain carrier state */
      carrier_state = hdrrmac4drv.per_carrier_info[ch].carrier_state;

      if ( carrier_state == HDRRMAC4DRV_CH_ST_DATA_ACTIVE )
      {
        /* Power required to maintain minimum data rate on an active
           carrier */
        pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, 
          (((((uint64) pilot_pow[ch]) * 
          ( hdrrmac4drv.per_carrier_info[ch].
          pilot_plus_overhead_gains + 
          hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate )) + 
          (((uint32)1) << 9))
          >> 10 ) );

        /* If enhanced PA backoff is enabled, make adjustments to required 
           power. 

           Rationale for power adjustment: 
           The power computed by this function is compared against pmax by
           the caller to determine if we need to go one level down in carrier
           configuration.  The condition checked is - 
             if( required power  >  pmax ), go one carrier config down
           In funnel mode, the available power is pmax_neq (> pmax) so the 
           condition check should be changed to - 
             if( required power > pmax_neq )
           i.e. if ( required power + pmax - pmax_neq ) > pmax
           i.e. if ( reqired power - power adjust ) > pmax
           where power adjust = pmax_neq - pmax
           */
        if ( hdrrmac4drv.enh_pa_backoff_enabled )
        {
          /* Compute power adjust value */
          if ( hdrrmac4drv.pmax_neq >= hdrrmac4drv.pmax[
            hdrrmac4drv.num_chans-1] )
          {
            /* Integral promotion causes result of the following operation
               to be an int64 value. Storing it as such in a temporary 
               variable. */
            pwr_adjust_int = hdrrmac4drv.pmax_neq - hdrrmac4drv.pmax[
              hdrrmac4drv.num_chans-1];
          }
          else
          {
            HDR_MSG_DRIVERS_2( MSG_LEGACY_ERROR, 
              "Enhanced PA backoff: MaxTxPwr_Neq [%u] < MaxTxPwr_Eq [%u]",
              hdrrmac4drv.pmax_neq,
              hdrrmac4drv.pmax[hdrrmac4drv.num_chans-1] );
          }

          /* TODO: Remove this message after feature verification. */
          HDR_MSG_DRIVERS_4( MSG_LEGACY_MED, 
            "Enhanced PA backoff: carrier drop pwr adjust [%u - %u]: %d. Req pwr before adjust: %u",
            hdrrmac4drv.pmax_neq,
            hdrrmac4drv.pmax[hdrrmac4drv.num_chans-1],
            pwr_adjust_int,
            pwr );

          /* Convert intermediate int64 power adjust value to uint32.  
             The value is expected to be non-negative. */
          pwr_adjust = (uint32) MIN( ((uint64) 0xFFFFFFFF), 
            ((uint64) pwr_adjust_int) );

          /* Reduce required power by the power adjust value */
          if ( pwr > pwr_adjust )
          {
            pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, 
              (uint64) ( ((int64) pwr) - pwr_adjust ) );
          }
          else
          {
            /* Power to be adjusted is lower than the power adjust value
               => Set required power value to zero */
            pwr = 0;
          }

          /* TODO: remove this message after feature verification. */
          HDR_MSG_DRIVERS_2( MSG_LEGACY_MED, 
            "Enhanced PA backoff: pwr adjust: %d. Req pwr after adjust: %u",
            pwr_adjust,
            pwr );
        }

        tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

        HDR_MSG_DEBUG_5( HDR_MSG_DBG_PS_SELECTION, 
          "Drop: Data_Active carrier [%d] %u,%u,%u,pwr: %u",
          ch,
          pilot_pow[ch],
          hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains,
          hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate,
          pwr );

      }
      else if ( ( carrier_state == HDRRMAC4DRV_CH_ST_DATA_INACTIVE ) || 
        ( carrier_state == HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE ) )
      {
        /* Power consumed by an data inactive carrier */
        pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, 
          (((((uint64) pilot_pow[ch]) * 
          ( hdrrmac4drv.per_carrier_info[ch].
          pilot_plus_overhead_gains )) + 
          (((uint32)1) << 9))
          >> 10 ) );

        tx_pwr_min = HDRRMAC4DRV_ADD_UINT_SAT( tx_pwr_min, pwr, 0xFFFFFFFF );

        HDR_MSG_DEBUG_4( HDR_MSG_DBG_PS_SELECTION, 
          "Drop: Data_Inactive carrier [%d] %u,%u,pwr: %u",
          ch,
          pilot_pow[ch],
          hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains,
          pwr );

      }
    }

    HDR_MSG_DEBUG_1( HDR_MSG_DBG_PS_SELECTION, 
      "Next down is carrier drop.  Pwr reqd to maintain current state: %d",
      tx_pwr_min );

  }


#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return tx_pwr_min;

} /* hdrrmac4drv_find_curr_pow() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_FIND_NEXT_CONFIG                               INTERNAL

DESCRIPTION
  Given the current carrier configuration, the function returns the
  next configuration based on carrier pilot powers and total available
  transmit pilot power.
  
DEPENDENCIES
  None.

PARAMETERS
  curr_config - Current configuration.
  pilot_pow   - Points to MAX_NUM_PHY_CHANS instances of power margin 
                corrected transmit pilot powers.

RETURN VALUE
  Next carrier configuration.

SIDE EFFECTS
  None.
===========================================================================*/
hdrrmac4drv_carrier_config_id_enum_type hdrrmac4drv_find_next_config
(
  /* Current configuration */
  hdrrmac4drv_carrier_config_id_enum_type  curr_config,

  /* Points to MAX_NUM_PHY_CHANS instances of power margin corrected 
     transmit pilot powers */
  uint32 * pilot_pow
)
{
  hdrrmac4drv_carrier_config_id_enum_type  up_config;
  hdrrmac4drv_carrier_config_id_enum_type  down_config;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  HDR_ASSERT (curr_config < CARRIER_CONFIG_MAX);

  up_config = 
    hdrrmac4drv_carrier_config[curr_config].next_up_config.config_id;

  down_config = 
    hdrrmac4drv_carrier_config[curr_config].next_down_config.config_id;
  
  HDR_ASSERT(( up_config < CARRIER_CONFIG_MAX ) || ( up_config == CARRIER_CONFIG_INVALID ));
  
  /* Check if we can support next up configuration */
  if ( (up_config != CARRIER_CONFIG_INVALID) && 
    (hdrrmac4drv_find_up_pow( curr_config, pilot_pow ) < 
    hdrrmac4drv.pmax[hdrrmac4drv_carrier_config[up_config].num_carriers-1]) )
  {
    /* We can support next up carrier configuration */

    /* Treat carrier_configuration # 5 as special case to allow the 
       possibility of sending carrier add request for two carriers in
       one subframe */
    if ( curr_config == CARRIER_CONFIG_P0_D0 )
    {
      if ( hdrrmac4drv_find_up_pow( up_config, pilot_pow ) < 
           hdrrmac4drv.pmax[2] )
      {
        /* We can support two more carriers */
        return hdrrmac4drv_carrier_config[up_config].next_up_config.config_id;
      }
    }

    /* Return next up carrier configuration */
    return up_config;

  }
  /* We cannot support next up carrier configuration.  
     Check if we can support the current carrier configuration */
  else if (hdrrmac4drv_find_curr_pow( curr_config, pilot_pow ) <= 
    hdrrmac4drv.pmax[hdrrmac4drv_carrier_config[curr_config].num_carriers-1] )
  {
    return curr_config;

  }
  /* We cannot support the current configuration. */
  else
  {
    /* Return next down carrier configuration if possible */
    if ( down_config != CARRIER_CONFIG_INVALID )
    {
      return down_config;
    }
    else
    {
      /* We cannot go further down - return current configuration */
      return curr_config;
    }
  }

} /* hdrrmac4drv_find_next_config() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_OPEN_CHAN_LIST                          INTERNAL

DESCRIPTION
  The function updates the list of OPEN channels.  At the end of this 
  function hdrrmac4drv.open_chans[] should have an updated list of OPEN
  channels such that - 
  - open_chans[0 -> (hdrrmac4drv.num_chans - 1)] lists the channels that are
     currently in OPEN state
  - open_chans[hdrrmac4drv.num_chans -> MAX_CHANS] lists the channels that
     are not in OPEN state
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_open_chan_list( void )
{
  /* Loop counters */
  uint8 ch, idx1, idx2;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  idx1 = 0;
  idx2 = HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS - 1;

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if ( ( ((uint16)hdrrmac4drv.per_carrier_info[ch].carrier_state) & 
      ((uint16)HDRRMAC4DRV_CH_ST_MAJOR_MASK) ) == 
      ((uint16)HDRRMAC4DRV_CH_ST_OPEN) )
    {
      hdrrmac4drv.open_chans[idx1] = ch;
      idx1++;
    }
    else
    {
      hdrrmac4drv.open_chans[idx2] = ch;
      idx2--;      
    }
  }

  if ( idx1 != hdrrmac4drv.num_chans )
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
      "num_chans != Number of OPEN channels!" ); 
  }

  HDR_MSG_DEBUG_4( HDR_MSG_DBG_PS_SELECTION, 
    "NumOpenChans: %d, OpenChanList: %d, %d, %d",
    hdrrmac4drv.num_chans,
    hdrrmac4drv.open_chans[0],
    hdrrmac4drv.open_chans[1],
    hdrrmac4drv.open_chans[2] );

} /* hdrrmac4drv_update_open_chan_list() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_OPEN_CHAN_DATA_LIST                     INTERNAL

DESCRIPTION
  The function updates the list of channels that are in OPEN state and are
  carrying data i.e. channels in one of the following states:
  DATA_ACTIVE
  TO_BE_DATA_INACTIVE
  TO_BE_INACTIVE
  At the end of this function -
  - open_chans_data[0 -> (hdrrmac4drv.num_chans_data - 1)] lists the 
     OPEN channels that are currently carrying data
  - open_chans_data[hdrrmac4drv.num_chans_data -> MAX_CHANS] lists
     rest of the channels
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_open_chan_data_list( void )
{
  /* Loop counters */
  uint8 ch, idx1, idx2;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  idx1 = 0;
  idx2 = HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS - 1;

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if ( ( ( ((uint16)hdrrmac4drv.per_carrier_info[ch].carrier_state) & 
      ((uint16)HDRRMAC4DRV_CH_ST_MAJOR_MASK) ) == 
      ((uint16)HDRRMAC4DRV_CH_ST_OPEN) ) && 
      ( hdrrmac4drv.per_carrier_info[ch].carrier_state != 
      HDRRMAC4DRV_CH_ST_DATA_INACTIVE ) )
    {
      hdrrmac4drv.open_chans_data[idx1] = ch;
      idx1++;
    }
    else
    {
      hdrrmac4drv.open_chans_data[idx2] = ch;
      idx2--;      
    }
  }

  hdrrmac4drv.num_chans_data = idx1;

  HDR_MSG_DEBUG_4( HDR_MSG_DBG_PS_SELECTION, 
    "NumDataChans: %d, OpenChanList: %d, %d, %d",
    hdrrmac4drv.num_chans_data,
    hdrrmac4drv.open_chans_data[0],
    hdrrmac4drv.open_chans_data[1],
    hdrrmac4drv.open_chans_data[2] );

  /* Error catching */
  ASSERT( hdrrmac4drv.num_chans_data <= HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );

} /* hdrrmac4drv_update_open_chan_data_list() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_NUM_REPORTABLE_CARRIERS                 INTERNAL

DESCRIPTION
  The function updates number of reportable carriers.
  
DEPENDENCIES
  This function assumes that open channel list is correctly populated.
  Hence hdrrmac4drv_update_open_chan_list() should be called prior to calling
  this function.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_num_reportable_carriers( void )
{
  /* Loop counters */
  uint8 idx, ch;

  /* Number of reportable carriers */
  uint8 num_reportable_carriers = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (idx=0; idx<hdrrmac4drv.num_chans; idx++ )
  {
    ch = hdrrmac4drv.open_chans[idx];

    if ( ((uint16)hdrrmac4drv.per_carrier_info[ch].carrier_state) != 
      ((uint16)HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE) )
    {
      if (hdrrmac4drv.per_carrier_info[ch].reportable)
      {
        /* Increment count of reportable carriers for every reportable
           carrier that is in OPEN state and is not about to be dropped */
        num_reportable_carriers++;
      }
    }
  }

  hdrrmac4drv.num_reportable_carriers = num_reportable_carriers;

  return;

} /* hdrrmac4drv_update_num_reportable_carriers */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_FUNNEL_STATE                            INTERNAL

DESCRIPTION 
  Updates AT funnel state.
  AT enters funnel state when there are more than one carriers assigned and 
  only one of the assigned carriers is data-active.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_funnel_state( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Set funnel state flag if all of the following conditions are satisfied:
     1. Enhanced PA backoff is enabled 
     2. There are more than one assigned carriers 
     3. There is just one data carrying carrier 
   */  
  if ( hdrrmac4drv.enh_pa_backoff_enabled && 
       ( hdrrmac4drv.num_chans > 1 ) &&
       ( hdrrmac4drv.num_chans_data == 1 ) )
  {
    hdrrmac4drv.in_funnel_mode = TRUE;
  }
  else
  {
    hdrrmac4drv.in_funnel_mode = FALSE;
  }
}


/*===========================================================================
FUNCTION HDRRMAC4DRV_SET_NEW_CONFIG                                 INTERNAL

DESCRIPTION
  The function 
  - sets new carrier configuration if the transition is allowed by the 
    carrier state transition table.
  - Checks if data can be disabled on carriers in to-be-deactivated and
    to-be-dropped transition states.
  
DEPENDENCIES
  None.

PARAMETERS
  next_config - Next carrier configuration.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_set_new_config
(
  /* Next carrier configuration */
  hdrrmac4drv_carrier_config_id_enum_type  next_config
)
{
  /* Points to either RTCMAC priority table or overall priority table 
     depending on the carrier configuration transition */
  uint8 * carrier_priority_ptr;

  /* Current carrier configuration */
  hdrrmac4drv_carrier_config_id_enum_type  curr_config;

  /* Carrier configuration change possible flag */
  boolean config_change_possible = TRUE;

  /* Next carrier states */
  hdrrmac4drv_carrier_state_enum_type 
    carrier_next_state[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Loop counters */
  uint8 ch, phy_chan_id;

  /* Find new SLP carrier flag */
  boolean find_new_slp = FALSE;

  /* New eligible SLP carrier */
  uint8 new_slp = hdrrmac4drv.slp_carrier.current;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrrmac4drv_bypass_cm_decisions)
  {
    /* Bypass carrier management decisions */
    return;
  }

  /* Save target carrier configuration */
  hdrrmac4drv.target_carrier_config_id = next_config;

  curr_config = hdrrmac4drv.carrier_config_id;

  ASSERT ((curr_config < CARRIER_CONFIG_MAX) &&
          (next_config < CARRIER_CONFIG_MAX));

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    /* Save carrier states */
    carrier_next_state[ch] = hdrrmac4drv.per_carrier_info[ch].carrier_state;
  }

  /* Check if the carrier configuration change is allowed and
     if the transition is valid */
  if ( hdrrmac4drv_carrier_config_transition_table[
       curr_config][next_config].valid_transition )
  {
    /* Select appropriate carrier priority table */
    if (hdrrmac4drv_carrier_config_transition_table[
        curr_config][next_config].priority == OVERALL)
    {
      carrier_priority_ptr = hdrrmac4drv.prioritized_carriers.overall;
    }
    else
    {
      carrier_priority_ptr = hdrrmac4drv.prioritized_carriers.rtcmac;
    }

    for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++ )
    {
      phy_chan_id = carrier_priority_ptr[ch];

      HDR_MSG_DEBUG_2( HDR_MSG_DBG_PS_SELECTION, 
        "Proposed CMA: Carrier[%d] %d",
        phy_chan_id,
        hdrrmac4drv_carrier_config_transition_table[curr_config]
        [next_config].carrier_transition[ch] );

      switch (hdrrmac4drv_carrier_config_transition_table[
              curr_config][next_config].
              carrier_transition[ch])
      {
        case REQUEST_ADD: 
          hdrrmac4drv.num_req_carriers++;
          break;
  
        case ACTIVATE_DATA:
          switch(hdrrmac4drv.per_carrier_info[phy_chan_id].carrier_state)
          {
            case HDRRMAC4DRV_CH_ST_DATA_ACTIVE:
              break;

            case HDRRMAC4DRV_CH_ST_DATA_INACTIVE:
              /* Activate data-funneled carrier */
              carrier_next_state[phy_chan_id] = 
                HDRRMAC4DRV_CH_ST_DATA_ACTIVE;

              break;

            case HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE:
              /* Fall through intentional */
  
            case HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE:

            case HDRRMAC4DRV_CH_ST_INACTIVE:

            default:
              config_change_possible = FALSE;
              break;
          }
          break;
  
        case DEACTIVATE_DATA:
          switch(hdrrmac4drv.per_carrier_info[phy_chan_id].carrier_state)
          {
            case HDRRMAC4DRV_CH_ST_DATA_ACTIVE:

              if ( hdrrmac4drv.slp_carrier.current != phy_chan_id )
              {
                /* Schedule the carrier for deactivation */
                carrier_next_state[phy_chan_id] = 
                  HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE;

              }
              else /* The candidate for data_deactivation is the SLP carrier */
              {
                /* If the SLP queue is empty
                   - Deactivate data on the current SLP carrier
                   - Choose a new SLP carrier */
                if (hdrrmac4drv.slp_carrier.has_data == FALSE)
                {
                  /* Set find new SLP flag */
                  find_new_slp = TRUE;

                  /* Deactivate data on the carrier */
                  carrier_next_state[phy_chan_id] = 
                    HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE;

                }
                else /* SLP has data */
                {
                  /* Ignore the carrier management action */
                  config_change_possible = FALSE;
                }
              }
              
              break;

            case HDRRMAC4DRV_CH_ST_DATA_INACTIVE:
              /* Do nothing - carrier already DATA_INACTIVE state */
              break;

            case HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE:
              /* Do nothing - carrier already scheduled for data-deactivation */
              break;

            case HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE:
              /* Fall through intentional */

            case HDRRMAC4DRV_CH_ST_INACTIVE:

            default:
              config_change_possible = FALSE;
              break;
          }
          break;

        case DROP:
          switch(hdrrmac4drv.per_carrier_info[phy_chan_id].carrier_state)
          {
            case HDRRMAC4DRV_CH_ST_DATA_ACTIVE:

              if ( hdrrmac4drv.slp_carrier.current != phy_chan_id )
              {
                /* Schedule the carrier for drop */
                carrier_next_state[phy_chan_id] = 
                  HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE;

              }
              else /* The candidate for drop is the SLP carrier */
              {
                /* If the SLP queue is empty
                   - Schedule the current SLP carrier for drop
                   - Choose a new SLP carrier */
                if (hdrrmac4drv.slp_carrier.has_data == FALSE)
                {
                  /* Set find new SLP flag */
                  find_new_slp = TRUE;

                  /* Schedule the carrier for drop */
                  carrier_next_state[phy_chan_id] = 
                    HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE;

                }
                else /* SLP has data */
                {
                  /* Ignore the carrier management action */
                  config_change_possible = FALSE;
                }
              }

              break;

            case HDRRMAC4DRV_CH_ST_DATA_INACTIVE:
              /* Since there is no ongoing data on the carrier, the carrier 
              can be directly transitioned to INACTIVE state */
              carrier_next_state[phy_chan_id] = 
                HDRRMAC4DRV_CH_ST_INACTIVE;
              break;

            case HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE:
              /* Carrier can transition from TO_BE_DATA_INACTIVE to
              TO_BE_INACTIVE state */
              carrier_next_state[phy_chan_id] = 
                HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE;
              break;

            case HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE:
              /* Do nothing - carrier already scheduled for drop */
              break;

            case HDRRMAC4DRV_CH_ST_INACTIVE:
              /* Do nothing - carrier already in INACTIVE state */
              break;

            default:
              config_change_possible = FALSE;
              break;
          }
  
          break;
  
        case DO_NOTHING:
          break;
  
        default:
          HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, "Illegal carrier transition!" );

      }
    } /* for all channels */


    /* If current SLP carrier is scheduled for deactivation/drop, 
       find a new SLP carrier */
    if (config_change_possible && find_new_slp)
    {
      /* Find a new SLP carrier */
      for (ch=0; ch < hdrrmac4drv.num_chans; ch++)
      {
        new_slp = hdrrmac4drv.prioritized_carriers.overall[ch];

        if ( carrier_next_state[new_slp] == HDRRMAC4DRV_CH_ST_DATA_ACTIVE )
        {
          /* New SLP carrier found - break from the for loop */
          break;
        }
      }

      if (ch == hdrrmac4drv.num_chans)
      {
        /* Could not find new SLP carrier */
        /*HDR_MSG_DRIVERS_8( MSG_LEGACY_ERROR, 
          "Invalid CMA! SLP=%d, PriLst=[%d,%d,%d], St[%d,%d,%d], NumChans=%d",
          hdrrmac4drv.slp_carrier.current,
          hdrrmac4drv.prioritized_carriers.overall[0],
          hdrrmac4drv.prioritized_carriers.overall[1],
          hdrrmac4drv.prioritized_carriers.overall[2],
          carrier_next_state[hdrrmac4drv.prioritized_carriers.overall[0]],
          carrier_next_state[hdrrmac4drv.prioritized_carriers.overall[1]],
          carrier_next_state[hdrrmac4drv.prioritized_carriers.overall[2]],
          hdrrmac4drv.num_chans );*/

        /* Ignore carrier management action */
        config_change_possible = FALSE;
      }
      else
      {
        /* Set new SLP carrier */
        hdrrmac4drv.slp_carrier.current = new_slp;
      }

    } /* find new SLP carrier */


    /* Apply per-carrier state changes if the carrier configuration 
       change is possible */
    if (config_change_possible)
    {
      for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++ )
      {
        if ( ( carrier_next_state[ch] == HDRRMAC4DRV_CH_ST_INACTIVE ) && 
          ( hdrrmac4drv.per_carrier_info[ch].carrier_state != 
          HDRRMAC4DRV_CH_ST_INACTIVE ) )
        {
          /* Drop carrier */
          hdrrmac4drv_stop_channel(ch);
        }
        else
        {
          /* Copy over the new carrier states */
          hdrrmac4drv.per_carrier_info[ch].carrier_state = 
            carrier_next_state[ch];

        }

      } /* for all channels */

      /* In all cases other than carrier addition, carrier management 
         sets the current configuration state to the next state.
         For carrier addition, num_req_carriers info is returned to
         RMAC in hdrrmac4drv_get_rl_add_drop_info(). */
      if (hdrrmac4drv.num_req_carriers == 0)
      {
        hdrrmac4drv.carrier_config_id = next_config;
      }

      /* Update OPEN channel list */
      hdrrmac4drv_update_open_chan_list();

      /* Update list of channels that are carrying data */
      hdrrmac4drv_update_open_chan_data_list();

      /* Update number of reportable carriers */
      hdrrmac4drv_update_num_reportable_carriers();

      /* Funnel state can change due to change in number of assigned 
         or data carrying carriers => update funnel state. */
      hdrrmac4drv_update_funnel_state();

    }

  } /* if carrier config transition is valid */

  HDR_MSG_DEBUG_3( HDR_MSG_DBG_PS_SELECTION, 
    "Final carrier states: %d, %d, %d",
    hdrrmac4drv.per_carrier_info[0].carrier_state,
    hdrrmac4drv.per_carrier_info[1].carrier_state,
    hdrrmac4drv.per_carrier_info[2].carrier_state );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_set_new_config() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_CARRIER_CONFIG                          INTERNAL

DESCRIPTION
  The function performs carrier management.  
  
DEPENDENCIES
  None.

PARAMETERS
  filt_txp   - Filtered transmit pilot powers * Filtered pwr margins of
               assigned channels in Q.22 mW.
  p_max      - Maximum total available transmit power.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_carrier_config
(
  /* Points of MAX_NUM_PHY_CHANS instances of filtered tx pilot powers */
  uint32 * filt_txp
)
{
  /* Pilot power */
  uint32 pilot_pow[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Total filtered transmit pilot power */
  uint32 total_filt_txp = 0;

  /* Average filtered transmit pilot power */
  uint32 avg_filt_txp;

  /* Current carrier configuration */
  hdrrmac4drv_carrier_config_id_enum_type  curr_config;

  /* Next carrier configuration id */
  hdrrmac4drv_carrier_config_id_enum_type  next_config;

  /* Loop counter */
  uint8  ch, idx;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (idx=0; idx<hdrrmac4drv.num_chans; idx++ )
  {
    ch = hdrrmac4drv.open_chans[idx];

    total_filt_txp = HDRRMAC4DRV_ADD_UINT_SAT( total_filt_txp, 
      filt_txp[ch], 0xFFFFFFFF );
  }

  avg_filt_txp = total_filt_txp / hdrrmac4drv.num_chans;

  for (idx=0; idx<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++ )
  {
    ch = hdrrmac4drv.open_chans[idx];

    if ( idx < hdrrmac4drv.num_chans )
    {
      /* Channel enabled */
      pilot_pow[ch] = filt_txp[ch];
    }
    else
    {
      /* Channel disabled */
      pilot_pow[ch] = (uint32)( ((((uint64) avg_filt_txp) * 
        HDRRMAC4DRV_UNASSIGNED_CARRIER_TXP_MARGIN) + 
        (((uint32)1) << 9)) >> 10 );
    }

  } /* for all channels */

  curr_config = hdrrmac4drv.carrier_config_id;

  next_config = hdrrmac4drv_find_next_config( 
    curr_config, pilot_pow );

  hdrrmac4drv_set_new_config( next_config );

  HDR_MSG_DEBUG_9( HDR_MSG_DBG_PS_SELECTION, 
    "Curr_Config:%d, Prop_Next_Config: %d, Next_Config: %d, FiltP:%u,%u,%u,CalcFiltP:%u,%u,%u",
    curr_config,
    next_config,
    hdrrmac4drv.carrier_config_id,
    filt_txp[0],
    filt_txp[1],
    filt_txp[2],
    pilot_pow[0],
    pilot_pow[1],
    pilot_pow[2] );

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_update_carrier_config() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_LOG_PA_HDRM_CM_INFO                            INTERNAL

DESCRIPTION
  This function fills one subframe entry in the PA headroom and carrier
  management log packet.

DEPENDENCIES
  None.

PARAMETERS
  DSP Info - Information read from DSP for headroom computation and CM 
             decisions.
  PA Headroom run decision flag
            - FALSE => headroom algorithm bypassed due to sufficient power
  CM run decision flag
            - FALSE => carrier management bypassed due to sufficient power

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_log_pa_hdrm_cm_info
( 
  hdrrmac4drv_hdrm_dsp_info_type dsp,
  boolean hdrm_run_decision,
  boolean cm_run_decision,
  uint32 * max_t2p_ptr
)
{
  /* Previous subframe logging enabled status */
  static boolean prev_sf_log_enable_status = FALSE;

  /* Current subframe logging enabled status */
  boolean log_enable_status = FALSE;

  /* Convenience pointers */
  hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_info_type * data_sf;
  hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_chan_wide_info_type * data_c;

  /* Loop counters */
  uint8 ch, idx;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check whether log packet is enabled */
  log_enable_status = log_status(LOG_HDR_MC_PA_HDRM_AND_CARRIER_MGMT_C);

  if (prev_sf_log_enable_status != log_enable_status)
  {
    /* Submit log packet whenever log status changes from enabled to
       disabled. */
    if (prev_sf_log_enable_status == TRUE)
    {
      hdrrmac4drv_submit_pa_hdrm_cm_info_log();
    }

    /* Update logging status for comparison during in next subframe */
    prev_sf_log_enable_status = log_enable_status;

  }

  /* PA_HDRM_AND_CARRIER_MGMT packet enabled, so perform logging. */
  if (log_enable_status)
  {
    /* Error catching */
    ASSERT( hdrrmac4drv_hdrm_cm_log_info.sf_count < 
      HDRLOG_MAX_SUBFRAMES_PER_MC_PA_HDRM_CM_PKT );

    data_sf = &hdrrmac4drv_hdrm_cm_log_info.
      log_sf[hdrrmac4drv_hdrm_cm_log_info.sf_count];

    /* Current subframe */
    data_sf->subframe_n = hdrrmac4drv.subframe_id;

    /* Total available transmit power */
    data_sf->p_max = dsp.p_max;

    /* AT Funnel mode state */
    data_sf->in_funnel_mode = hdrrmac4drv.in_funnel_mode;

    /* Maximum available transmit power when AT in funnel mode */
    data_sf->p_max_neq = dsp.p_max_neq;

    /* Current carrier configuration */
    data_sf->carrier_config = (uint8) hdrrmac4drv.carrier_config_id;

    /* Target carrier configuration */
    data_sf->target_carrier_config = (uint8) 
      hdrrmac4drv.target_carrier_config_id;

    /* RTCMAC prioritized carriers */
    data_sf->priority_and_run_info.rtcmac_pri_carrier0 = 
      hdrrmac4drv.prioritized_carriers.rtcmac[0];
    data_sf->priority_and_run_info.rtcmac_pri_carrier1 = 
      hdrrmac4drv.prioritized_carriers.rtcmac[1];
    data_sf->priority_and_run_info.rtcmac_pri_carrier2 = 
      hdrrmac4drv.prioritized_carriers.rtcmac[2];

    /* Overall prioritized carriers */
    data_sf->priority_and_run_info.overall_pri_carrier0 = 
      hdrrmac4drv.prioritized_carriers.overall[0];
    data_sf->priority_and_run_info.overall_pri_carrier1 = 
      hdrrmac4drv.prioritized_carriers.overall[1];
    data_sf->priority_and_run_info.overall_pri_carrier2 = 
      hdrrmac4drv.prioritized_carriers.overall[2];

    /* PA Headroom run decision flag */
    data_sf->priority_and_run_info.pa_hdrm_run_decision = 
      hdrm_run_decision;

    /* Carrier management run decision flag */
    data_sf->priority_and_run_info.cm_run_decision = 
      cm_run_decision;

    /* Log SLP_has_data status */
    data_sf->priority_and_run_info.slp_has_data = 
      hdrrmac4drv.slp_carrier.has_data;

    /* Clear reserved bit */
    data_sf->priority_and_run_info.reserved = 0;

    /* Populate channel data entries for this subframe */
    for (idx=0; idx<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
    {
      ch = hdrrmac4drv.open_chans[idx];

      data_c = &hdrrmac4drv_hdrm_cm_log_info.
        log_carrier[hdrrmac4drv_hdrm_cm_log_info.sf_count][ch];

      if ( idx < hdrrmac4drv.num_chans )
      {
        /* OPEN channel => Tag the channel for logging */
        hdrrmac4drv_hdrm_cm_log_info.enable_channel_logging[ch] = 
          TRUE;

        /* Channel id */
        data_c->bit_info.channel_id = ch;

        /* Carrier state */
        data_c->bit_info.carrier_state = (uint8)
          hdrrmac4drv.per_carrier_info[ch].carrier_state;

        /* Carrier not-disabled-by-DRC-supervision flag */
        data_c->bit_info.data_allowed = 
          !(hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired);

        /* SLP carrier flag */
        if (ch == hdrrmac4drv.slp_carrier.current)
        {
          data_c->bit_info.is_slp_carrier = TRUE;
        }
        else
        {
          data_c->bit_info.is_slp_carrier = FALSE;
        }

        /* Clear reserved bits */
        data_c->bit_info.reserved = 0;

        /* Instantaneous transmit pilot power from DSP */
        data_c->inst_txp = 
          dsp.c[ch].inst_txp;

        /* Transmit pilot power from DSP */
        data_c->peak_txp = 
          dsp.c[ch].txp;

        /* Filtered transmit pilot power from DSP */
        data_c->filt_txp = 
          dsp.c[ch].filt_txp;

        /* Power margin from DSP */
        data_c->pwr_margin = 
          dsp.c[ch].pwr_margin;

        /* Filtered power margin */
        data_c->filt_pwr_margin = 
          hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin;

        /* Filtered RAB limited power */
        data_c->filt_rab_limited_p_data = 
          hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data;

        /* TxT2PMax(PS) */
        data_c->txt2p_max_ps = 
          hdrrmac4drv.per_carrier_info[ch].txt2p_max_ps;

        /* TxT2PMax */
        data_c->txt2p_max = 
          max_t2p_ptr[ch];

      }
      else
      {
        /* Channel not enabled in this subframe */

        /* Channel id */
        data_c->bit_info.channel_id = ch;

        /* Carrier state */
        data_c->bit_info.carrier_state = (uint8) HDRRMAC4DRV_CH_ST_INACTIVE;

        /* Carrier not-disabled-by-DRC-supervision flag */
        data_c->bit_info.data_allowed = 
          !(hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired);

        /* SLP carrier flag */
        data_c->bit_info.is_slp_carrier = FALSE;

        /* Clear reserved bits */
        data_c->bit_info.reserved = 0;

        /* Instantaneous transmit pilot power from DSP */
        data_c->inst_txp = 0;

        /* Transmit pilot power from DSP */
        data_c->peak_txp = 0;

        /* Filtered transmit pilot power from DSP */
        data_c->filt_txp = 0;

        /* Power margin from DSP */
        data_c->pwr_margin = 0;

        /* Filtered RAB limited power */
        data_c->filt_rab_limited_p_data = 0;

        /* TxT2PMax(PS) */
        data_c->txt2p_max_ps = 0;

        /* TxT2PMax */
        data_c->txt2p_max = 0;

      }

    } /* for all carriers */

    /* Update subframe count and submit log if needed. */
    hdrrmac4drv_hdrm_cm_log_info.sf_count++;

    if ( hdrrmac4drv_hdrm_cm_log_info.sf_count == 
         HDRLOG_MAX_SUBFRAMES_PER_MC_PA_HDRM_CM_PKT )
    {
      hdrrmac4drv_submit_pa_hdrm_cm_info_log();
    }

  } /* if logging is enabled */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_log_pa_hdrm_cm_info() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_PROCESS_PENDING_CARRIER_STATE_TRANSITIONS      INTERNAL

DESCRIPTION 
  1. Drops carriers in to-be-inactive state that are marked okay to
     deactivate.
  2. Transitions carriers in to-be-data-inactive state to data-inactive
     state if they are marked okay to deactivate.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_process_pending_carrier_state_transitions( void )
{

  /* Loop counter */
  uint8  ch;

  /* Flag to indicate that channel lists need to be updated */
  boolean update_chan_lists = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If there are no packets in progress on a carrier
     - Drop the carrier if carrier drop is pending
     - Deactivate the carrier if carrier deactivation is pending */
  for ( ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++ )
  {
    if ( hdrrmac4drv.per_carrier_info[ch].okay_to_deactivate )
    {
      if ( hdrrmac4drv.per_carrier_info[ch].carrier_state == 
        HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE )
      {
        /* We do not have to treat SLP carrier as a special case here.
           If the SLP carrier had data to be transmitted in the 
           previous subframe, there would be active packets on the
           carrier */

        /* Drop carrier */
        hdrrmac4drv_stop_channel(ch);

        /* Set update channel lists flag */
        update_chan_lists = TRUE;
      }
      else if ( hdrrmac4drv.per_carrier_info[ch].carrier_state ==
        HDRRMAC4DRV_CH_ST_TO_BE_DATA_INACTIVE )
      {
        /* Transition carrier to inactive state */
        hdrrmac4drv.per_carrier_info[ch].carrier_state = 
          HDRRMAC4DRV_CH_ST_DATA_INACTIVE;

        /* Set update channel lists flag */
        update_chan_lists = TRUE;
      }

      /* Reset okay-to-deactivate flag */
      hdrrmac4drv.per_carrier_info[ch].okay_to_deactivate = FALSE;

    } /* if the carrier is marked for transition to inactive or data-inactive
         state and has no active packets */

  } /* for all carriers */

  /* Update channel lists if carriers have changed states */
  if (update_chan_lists)
  {
    /* Update OPEN channel list */
    hdrrmac4drv_update_open_chan_list();

    /* Update list of channels that are carrying data */
    hdrrmac4drv_update_open_chan_data_list();

    /* Funnel state can change due to change in number of assigned 
       or data carrying carriers => update funnel state. */
    hdrrmac4drv_update_funnel_state();

    /* This function only transitions statuses of carriers that are in 
       to_be_inactive state.  These carriers should already be counted out
       of reportable carrier list.  So updating number of reportable carriers
       is not necessary here. */
  }

} /* hdrrmac4drv_process_pending_carrier_state_transitions */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_MAX_T2P_REV_B                              INTERNAL

DESCRIPTION
  The function returns the maximum T2P value that can be supported for the
  next subpacket for all assigned channels using RevB PA headroom algorithm.
  
DEPENDENCIES
  None.

PARAMETERS
  max_t2p_ptr - Pointer to HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS instances of 
                max_t2p (in unit of 1/4 dB).
  ut          - TRUE => Use RevB PA headroom algorithm in unit test mode.
                FALSE => Use the algorithm in normal mode.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_get_max_t2p_rev_b
(
  uint32 * max_t2p_ptr,
  boolean ut
)
{
  /* Values read from DSP for headroom computation */
  hdrrmac4drv_hdrm_dsp_info_type dsp;

  /* Pwr Margin in Q.20 dB */
  int32  tx_pwr_margin_db[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]; 

  /* Pwr Margin in Q22.10 linear units */
  uint32 tx_pwr_margin[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0, 0, 0};

  /* Maximum available TxT2P */
  uint32 txt2p_max_ps[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0, 0, 0}; 

  /* TxT2PMax_PS before ACI mitigation adjustment */
  uint32  txt2p_max_ps_pre_aci_adj; 

  /* TxT2PMax_PS leftmost value adjusted by filt_txp_diff.  TxT2Pmax_PS
     is capped at this value. */
  uint32  txt2p_max_ps_aci_cap; 

  /* Filtered transmit pilot power in Q.10 dBm units */
  int32  filt_txp_dbm[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]; 

  /* Filtered transmit pilot power in Q.22 mW */
  uint32 filt_txp[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]; 

  /* Inverse of filtered transmit pilot power */
  uint32 inv_filt_txp[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0, 0, 0}; 

  /* Transmit pilot power in Q.10 dBm units */
  int32  txp_dbm[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Transmit pilot power */
  uint32 txp[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0,0,0};

  /* (Filtered transmit pilot power) * (Filtered power margin) in Q.22 lin */
  uint32 filt_txp_mgn[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]; 

  /* Instantaneous transmit pilot power in Q.10 dBm units */
  int16  txp_inst_dbm;

  /* Loop counters */
  uint8  ch, idx; 

  /* Number of OPEN channels that are not in DATA_INACTIVE state.
     These channels are included in distributing available data power */
  uint8 num_chans_pwr_dist = 0;

  /* Sum of filtered transmit pilot powers of enabled channels */
  uint32 sum_pilot_pwr = 0; 

  /* Maximum total available transmit power in dBm */
  int32  p_max_dbm = 0;       /* Non-funnel mode */
  int32  p_max_neq_dbm = 0;   /* Funnel mode */

  /* Maximum total available transmit power in mW */
  uint32 p_max = 0;           /* Non-funnel mode */
  uint32 p_max_neq = 0;       /* Funnel mode */

  /* Maximum available power for traffic */
  uint32 p_data_max; 

  /* Maximum available power for traffic per carrier in Q.22 mW */
  uint32 p_data_max_per_carrier[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
    {0, 0, 0}; 

  /* Maximum available power for traffic per carrier in scaled
     dB units */
  int16  p_data_max_per_carrier_db[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]; 

  /* Maximum available T2P in dB */
  int32  max_t2p_db; 

  /* Maximum T2P power that can be consumed by a carrier */
  uint32 max_req_carrier_pwr[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0};

  /* Maximum required per carrier tx power to support max data */
  uint32 max_req_carrier_tx_pwr = 0;

  /* Maximum required tx power to support max data on all enabled 
     carriers */
  uint32 max_req_tx_pwr = 0;

  /* Extra power in RAB based reallocation */
  uint32 extra_power_rab = 0; 

  /* Extra power in TxT2PMax based reallocation */
  uint32 extra_power_txt2pmax = 0; 

  /* Per carrier extra power */
  uint32 extra_power_c = 0; 

  /* TxT2Pmin in Q.2 dB units */
  int32  tx_t2p_min_db; 

  /* TxT2Pmin in Q22.10 linear units */
  uint32 tx_t2p_min; 

  /* Sum of inverse of filt_txp for all enabled channels */
  uint32 sum_inverse_filt_txp = 0; 

  /* Sum of inverse of filt_txp for all enabled channels except the SLP
     carrier */
  uint32 sum_inverse_filt_txp_minus_slp = 0; 

  /* Sum of inverse of filt_txp for all channels that need extra power in
     TxT2PMax based reallocation */
  uint32 sum_inverse_filt_txp_extra_power = 0; 

  /* Extra power candidate type */
  typedef enum
  {
    /* The channel has extra power that can be given to other channels */
    GIVER,

    /* The channel needs extra power */
    TAKER,

    /* The channel can neither give extra power nor can take it */
    NEITHER

  } extra_power_candidate_type;

  /* By default the channels are neither givers nor takers */
  extra_power_candidate_type extra_power_candidate[
    HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
    {NEITHER, NEITHER, NEITHER}; 

  /* Flag to indicate that there are extra power candidates for 
     RAB based reallocation */
  boolean extra_power_candidates_rab = FALSE;

  /* Flag to indicate that there are extra power candidates for 
     TxT2PMax based reallocation */
  boolean extra_power_candidates_txt2pmax = FALSE;

  /* temporary storage for storing values compared in MAX/MIN macros */
  uint32 num1, num2; 

  /* Power required to support MinTxT2P on SLP carrer */
  uint32 min_pow_slp_carrier;

  /* Power consumed by overhead channels in Q.22 mW */
  uint32 overhead_chan_pow[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0};

  /* Headroom run decision 
     FALSE => sufficient power => PA headroom algorithm not run */
  boolean hdrm_run_decision = TRUE;

  /* Carrier management run decision
     FALSE => sufficient power and config 0 => CM algorithm not run */
  boolean cm_run_decision = TRUE;
#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
  /* Pre-UT num of OPEN channels */
  uint8 pre_ut_num_chans = HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS;

  /* Pre-UT carrier configuration.
     Used in UT mode to store carrier configuration prior to running the
     algorithm.  The configuration is restored at the end of the function. */
  hdrrmac4drv_carrier_config_id_enum_type pre_ut_carrier_config = 
    CARRIER_CONFIG_P012_D012;

  /* Pre-UT Channel states.
     Used in UT mode to store channel states prior to running the
     algorithm.  All the channels are set to DATA_ACTIVE during UT.  
     The channel states are restored at the end of the function. */
  hdrrmac4drv_carrier_state_enum_type 
    pre_ut_channel_state[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = { 
    HDRRMAC4DRV_CH_ST_INACTIVE, HDRRMAC4DRV_CH_ST_INACTIVE, 
    HDRRMAC4DRV_CH_ST_INACTIVE };

  /* Pre-UT pilot plus overhead gains */
  uint32 pre_ut_pilot_plus_ovhd_gains[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
    {0, 0, 0};

  /* Pre-UT CM thresholds */
  hdrrmac4drv_carrier_mgt_thresh_type pre_ut_cm_thresholds = 
    {0, 0, 0, 0, 0};

  /* Pre-UT RTCMAC prioritized carriers */
  uint8 pre_ut_rtcmac_prioritized_carriers[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] 
    = {0, 1, 2};

  /* Pre-UT overall prioritized carriers */
  uint8 pre_ut_overall_prioritized_carriers[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] 
    = {0, 1, 2};

  /* Pre-UT drop ranks */
  uint8 pre_ut_drop_ranks[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0, 0, 0};
#endif /* FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST */

#ifdef HDRRMAC4DRV_EQUAL_HEADROOM_DISTRIBUTION
  /* Flag set if the carrier needs to be excluded from equal 
     power distribution */
  boolean eq_dist_excluded_chan[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = 
    { FALSE, FALSE, FALSE };

  /* Total available power for equal headroom distribution */
  uint32 eq_dist_available_pwr;

  /* Per-carrier power for equal headroom distribution */
  uint32 eq_dist_per_carrier_pwr;

#endif

  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

  /* Short hand pointer to shared memory location with TxAGC
     information */
  volatile hdrfw_txagc_rd_t * txagc_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrrmac4drv_bypass_pa_headroom_alg)
  {
    /* Bypass PA Headroom algorithm */

    max_t2p_ptr[0] = hdrrmac4drv_max_tx_t2p[0];
    max_t2p_ptr[1] = hdrrmac4drv_max_tx_t2p[1];
    max_t2p_ptr[2] = hdrrmac4drv_max_tx_t2p[2];

    return;
  }

  /* Initialize firmware subframe configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_SUBFRAME_CFG_CMD );


  /*--------------------- RevB PA headroom algorithm --------------------*/

  hdrrmac4drv_process_pending_carrier_state_transitions();

#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
  if (ut)
  {
    /* UT Setup */

    /* Save number of OPEN channels */
    pre_ut_num_chans = hdrrmac4drv.num_chans;

    /* Save carrier configuration */
    pre_ut_carrier_config = hdrrmac4drv.carrier_config_id;

    /* Overwrite carrier configuration */
    hdrrmac4drv.carrier_config_id = 
      hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].carrier_config;

    /* Save CM thresholds */
    pre_ut_cm_thresholds.t2p_remain_active = 
      hdrrmac4drv_carrier_mgt_thresh.t2p_remain_active;

    pre_ut_cm_thresholds.t2p_activate = 
      hdrrmac4drv_carrier_mgt_thresh.t2p_activate;

    pre_ut_cm_thresholds.t2p_deactivate = 
      hdrrmac4drv_carrier_mgt_thresh.t2p_deactivate;

    pre_ut_cm_thresholds.t2p_min_rate = 
      hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate;

    pre_ut_cm_thresholds.t2p_add = 
      hdrrmac4drv_carrier_mgt_thresh.t2p_add;


    /* Overwrite CM thresholds */
    hdrrmac4drv_carrier_mgt_thresh.t2p_remain_active = 
      hdrrmacmath_db_2_lin((
      hdrrmac4drv_headroom_ut_cm_thresh.t2p_remain_active << 18));

    hdrrmac4drv_carrier_mgt_thresh.t2p_activate = 
      hdrrmacmath_db_2_lin((
      hdrrmac4drv_headroom_ut_cm_thresh.t2p_activate << 18));

    hdrrmac4drv_carrier_mgt_thresh.t2p_deactivate = 
      hdrrmacmath_db_2_lin((
      hdrrmac4drv_headroom_ut_cm_thresh.t2p_deactivate << 18));

    hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate = 
      hdrrmacmath_db_2_lin((
      hdrrmac4drv_headroom_ut_cm_thresh.t2p_min_rate << 18));

    hdrrmac4drv_carrier_mgt_thresh.t2p_add = 
      hdrrmacmath_db_2_lin((
      hdrrmac4drv_headroom_ut_cm_thresh.t2p_add << 18));

    hdrrmac4drv.slp_carrier.current = hdrrmac4drv_headroom_ut_input[
      hdrrmac4drv_ut_sf].prev_slp_carrier;

    hdrrmac4drv.slp_carrier.has_data = hdrrmac4drv_headroom_ut_input[
      hdrrmac4drv_ut_sf].slp_has_data;

    hdrrmac4drv.num_chans = 0;

    for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      /* Save channel state. */
      pre_ut_channel_state[ch] = 
        hdrrmac4drv.per_carrier_info[ch].carrier_state;

      /* Overwrite channel state */
      hdrrmac4drv.per_carrier_info[ch].carrier_state = 
        hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].carrier_state;

      if ((((uint8)hdrrmac4drv.per_carrier_info[ch].carrier_state) & 
        ((uint8)HDRRMAC4DRV_CH_ST_MAJOR_MASK)) == 
        ((uint8)HDRRMAC4DRV_CH_ST_OPEN))
      {
        hdrrmac4drv.num_chans++;
      }

      /* Save RTCMAC prioritized carriers */
      pre_ut_rtcmac_prioritized_carriers[ch] = 
        hdrrmac4drv.prioritized_carriers.rtcmac[ch];

      /* Save overall prioritized carriers */
      pre_ut_overall_prioritized_carriers[ch] = 
        hdrrmac4drv.prioritized_carriers.overall[ch];

      /* Save pilot plus overhead gains */
      pre_ut_pilot_plus_ovhd_gains[ch] = 
        hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains;

      /* Save drop ranks */
      pre_ut_drop_ranks[ch] = 
        hdrrmac4drv.per_carrier_info[ch].drop_rank;

      /* Overwrite drop ranks */
      hdrrmac4drv.per_carrier_info[ch].drop_rank = 
        hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].dropping_rank;

      /* Overwrite pilot plus overhead gains with UT value - used in CM */
      hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains = 
        hdrrmac4drv_headroom_ut_pilot_plus_ovg[ch];

      /* Overwrite filtered RAB limited traffic power with UT value */
      hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data = 
        hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].
        filt_rab_limited_p_data;

      /* Overwrite K */
      hdrrmac4drv.k = -73;
    }

    /* Update channel lists */
    hdrrmac4drv_update_open_chan_list();
    hdrrmac4drv_update_open_chan_data_list();
  }
#endif /* FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST */

#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
  if (ut)
  {
    dsp.p_max = hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].p_max;
    dsp.p_max_neq = dsp.p_max;

    HDR_MSG_DEBUG_1( HDR_MSG_DBG_PS_SELECTION, 
      "UT: subframe#%d",
      hdrrmac4drv_ut_sf );

  }
  else
#endif /* FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST */
  {
    dsp.p_max = hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
      txTotalOverallMaxLimit;

    dsp.p_max_neq = hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
      txTotalOverallMaxLimitNeq;
  }

  /* Get maximum total availabe transmit power */
  /* Max. Avail. Pow. from DSP
        P = ((K + 63.8) - X dBm)*65536/DynamicRange
     => X = K + 63.8 - P*DynamicRange/65536  in dBm
          = K*1024 + 63.8*1024 - P*(102.4/65536)*1024 in Q21.10 dBm
          = K<<10 + 65331 - (P*819 >> 9)  in Q21.10 dBm  

     (102.4/65536)*1024 ~ 819 >> 9
     The above converts the floating point division to a fixed point 
     multiplication followed by a shift operation. */
  if (dsp.p_max >= 0)
  {
    p_max_dbm = (hdrrmac4drv.k << 10) + 65331 - 
      ( ( ( dsp.p_max * 
      819 ) + (((int16)1) << 8) ) >> 9 );
  }
  else
  {
    p_max_dbm = (hdrrmac4drv.k << 10) + 65331 - 
      ( ( ( dsp.p_max * 
      819 ) - (((int16)1) << 8) ) >> 9 );
  }

  /* Convert maximum available transmit power to Q10.22 mW */
  p_max = hdrrmacmath_dbm_2_mw( p_max_dbm );

  /* Store maximum available transmit power only for the current
     configuration. */
  hdrrmac4drv.pmax[hdrrmac4drv.num_chans-1] = p_max;

  HDR_MSG_DEBUG_3( HDR_MSG_DBG_PS_SELECTION, 
    "P_Max: %d DSP, %d dBm, %d mW",
    dsp.p_max,
    p_max_dbm,
    p_max );

  /* If enhanced PA backoff is enabled, get maximum total transmit 
     power for funnel mode. */
  if ( hdrrmac4drv.enh_pa_backoff_enabled )
  {
    /* Convert available power in funnel mode in Q.10 dBm units */
    if ( dsp.p_max_neq >= 0 )
    {
      p_max_neq_dbm = (hdrrmac4drv.k << 10) + 65331 - 
        ( ( ( dsp.p_max_neq * 
        819 ) + (((int16)1) << 8) ) >> 9 );
    }
    else
    {
      p_max_neq_dbm = (hdrrmac4drv.k << 10) + 65331 - 
        ( ( ( dsp.p_max_neq * 
        819 ) - (((int16)1) << 8) ) >> 9 );
    }

    /* Convert available power in funnel mode to Q10.22 mW */
    p_max_neq = hdrrmacmath_dbm_2_mw( p_max_neq_dbm );

    /* Store maximum available transmit power when in funnel mode */
    hdrrmac4drv.pmax_neq = p_max_neq;

    /* TODO: remove this message after feature verification. */
    HDR_MSG_DEBUG_3( HDR_MSG_DBG_PS_SELECTION,
      "P_Max_neq: %d DSP, %d dBm, %d mW",
      dsp.p_max_neq,
      p_max_neq_dbm,
      p_max_neq );

  }
  else
  {
    /* Set max available power neq value to be same as eq value if 
       enhanced PA backoff is disabled */
    hdrrmac4drv.pmax_neq = p_max_neq = p_max;    
  }

  /* Collect per-carrier information from MDSP */
  for (idx=0; idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
  {
    ch = hdrrmac4drv.open_chans[idx];

    if ( idx < hdrrmac4drv.num_chans )
    {
#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
      if (ut)
      {
        /* Transmit pilot power */
        dsp.c[ch].txp = 
          hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].txp;

        /* Instantaneous pilot power
           For UT, inst_pilot_power = peak_pilot_power */
        dsp.c[ch].inst_txp = 
          hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].txp;

        /* Filtered transmit pilot power */
        dsp.c[ch].filt_txp = 
          hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].filt_txp;

        /* Power margin */
        dsp.c[ch].pwr_margin =  
          hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].pwr_margin;

      }
      else
#endif /* FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST */
      {
        txagc_ptr = &(hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
          rmacInfo[ch].txAgc);

        /* Transmit pilot power. DSP register PilotPowerPeak holds
           - peak value when margin loop is set to use peak.
           - instantaneous value when margin loop is set to use 
             instantaneous value */
        dsp.c[ch].txp = txagc_ptr->pilotPowerPeak;

        /* Instantaneous pilot power */
        dsp.c[ch].inst_txp = txagc_ptr->pilotPower;

        /* Filtered transmit pilot power.  Convert FW returned 32-bit value
           to a 16-bit one. */
        dsp.c[ch].filt_txp = (txagc_ptr->pilotPowerFilt >> 16);

        /* Power margin */
        dsp.c[ch].pwr_margin = txagc_ptr->pwrMargin;

      }

      /* Get transmit pilot power in Q21.10 dBm */
      if (dsp.c[ch].txp >= 0)
      {
        txp_dbm[ch] = (hdrrmac4drv.k << 10) + 65331 - 
          ( ( ( dsp.c[ch].txp * 
          819 ) + (((int16)1) << 8) ) >> 9 );
      }
      else
      {
        txp_dbm[ch] = (hdrrmac4drv.k << 10) + 65331 - 
          ( ( ( dsp.c[ch].txp * 
          819 ) - (((int16)1) << 8) ) >> 9 );
      }

      /* Get inst transmit pilot power in Q21.10 dBm */
      if (dsp.c[ch].inst_txp >= 0)
      {
        txp_inst_dbm = (hdrrmac4drv.k << 10) + 65331 - 
          ( ( ( dsp.c[ch].inst_txp * 
          819 ) + (((int16)1) << 8) ) >> 9 );
      }
      else
      {
        txp_inst_dbm = (hdrrmac4drv.k << 10) + 65331 - 
          ( ( ( dsp.c[ch].inst_txp * 
          819 ) - (((int16)1) << 8) ) >> 9 );
      }

      /* Get filtered transmit pilot power in Q21.10 dBm */
      if (dsp.c[ch].filt_txp >= 0)
      {
        filt_txp_dbm[ch] = (hdrrmac4drv.k << 10) + 65331 -
          ( ( ( dsp.c[ch].filt_txp * 
          819 ) + (((int16)1) << 8) ) >> 9 );
      }
      else
      {
        filt_txp_dbm[ch] = (hdrrmac4drv.k << 10) + 65331 -
          ( ( ( dsp.c[ch].filt_txp * 
          819 ) - (((int16)1) << 8) ) >> 9 );
      }

      /* Get current power margin in Q21.10 dB
         PowMargin = DSP_PowMargin * DynamicRange/65536  in dB
                   = (DSP_PowMargin * 819) >> 9 in Q21.10 dB */
      if (dsp.c[ch].pwr_margin >= 0)
      {
        tx_pwr_margin_db[ch] = 
          ( ( ( dsp.c[ch].pwr_margin * 
          819 ) + (((int16)1) << 8) ) >> 9 );
      }
      else
      {
        tx_pwr_margin_db[ch] = 
          ( ( ( dsp.c[ch].pwr_margin * 
          819 ) - (((int16)1) << 8) ) >> 9 );
      }

      /* Get transmit pilot power in Q10.22 mW */
      txp[ch] = hdrrmacmath_dbm_2_mw( txp_dbm[ch] );

      /* Store inst transmit pilot power for the enabled 
         channel. This is used in 
         hdrrmac4drv_update_sum_pot_t2p_outflows() */
      hdrrmac4drv.per_carrier_info[ch].txp_inst = 
        hdrrmacmath_dbm_2_mw(txp_inst_dbm);

      /* Store filtered transmit pilot power.  This is used in 
         hdrrmac4drv_check_pilot_power_differential(). */
      hdrrmac4drv.per_carrier_info[ch].filt_txp_dbm = filt_txp_dbm[ch];

      /* Get filtered transmit pilot power in Q10.22 mW */
      filt_txp[ch] = hdrrmacmath_dbm_2_mw( filt_txp_dbm[ch] );

      /* Determine sum of pilot power */
      sum_pilot_pwr += filt_txp[ch];

      /* Determine inverse of filtered transmit power in Q22.10 */
      if (filt_txp[ch] != 0)
      {
        inv_filt_txp[ch] = (uint32)( ( ((uint64) 1) << 32 )/filt_txp[ch] );
      }
      else
      {
        HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
          "Zero FiltTxP!  Inv_FiltTxP set to max value" );

        inv_filt_txp[ch] = 0xFFFFFFFF;
      }

      /* Get current power margin in Q22.10 linear */
      tx_pwr_margin[ch] = hdrrmacmath_db_2_lin( 
        tx_pwr_margin_db[ch] << 10 );

#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
      if (ut)
      {
        hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin = 
          hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].filt_pwr_margin;
      }
#endif

      /* Calculate filtered power margin */
      if ( hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin == 
        HDRRMAC4DRV_FILT_PWR_MARGIN_INVALID )
      {
        /* Carrier enabled in last subframe.
           Set filtered power margin = instantaneous power margin */
        hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin = tx_pwr_margin[ch];
      }
      else
      {
        /* FilTxPMgn_n 
           = (1 - 1/TC_PWRMARGIN) * FiltTxPMgn_n-1
             + (1/TC_PWRMARGIN) * InstTxPMgn_n
           = FiltTxPMgn_n-1 
             - (FiltTxPMgn_n-1 * TC_PWRMARGIN_mul) >> TC_PWRMARGIN_shift
             + (InstTxPMgn_n * TC_PWRMARGIN_mul) >> TC_PWRMARGIN_shift */
        hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin -=
          (uint32) ( ( ( (uint32) hdrrmac4drv.per_carrier_info[ch].
          filt_pwr_margin ) * HDRRMAC4DRV_TC_PWRMARGIN_MUL + 
          ( 1 << (HDRRMAC4DRV_TC_PWRMARGIN_SHIFT - 1)) ) >> 
          HDRRMAC4DRV_TC_PWRMARGIN_SHIFT );

        hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin +=
          (uint32) ( ( ( (uint32) tx_pwr_margin[ch] ) * 
          HDRRMAC4DRV_TC_PWRMARGIN_MUL + 
          ( 1 << (HDRRMAC4DRV_TC_PWRMARGIN_SHIFT - 1)) ) >> 
          HDRRMAC4DRV_TC_PWRMARGIN_SHIFT );
      }

      /* Calculate (filtered txp)*(filtered margin) in Q.22 mW.  This value
         is used in carrier management for power consumption estimation of 
         a carrier configuration */
      filt_txp_mgn[ch] = (uint32) ( MIN( ( (uint64) 0xFFFFFFFF ), 
        ( ( ( ( (uint64) filt_txp[ch] ) * 
        hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin ) + 
        (((uint64)1) << 9) ) >> 10 ) ) );

      HDR_MSG_DEBUG_9( HDR_MSG_DBG_PS_SELECTION, 
        "ch:%d TxP=%dmW,FiltTxP=%ddBm,%dmW,InvFiltTxp=%d,Mgn=%ddB,%d,FiltMgn=%d,FiltTxPMgn=%d",
        ch,
        txp[ch],
        filt_txp_dbm[ch],
        filt_txp[ch],
        inv_filt_txp[ch],
        tx_pwr_margin_db[ch],
        tx_pwr_margin[ch],
        hdrrmac4drv.per_carrier_info[ch].filt_pwr_margin,
        filt_txp_mgn[ch] );

      /* Assign a high filt_txp_dbm value to TO_BE_INACTIVE channels so that 
         they are assigned lowest RTCMAC priorities */
      if ( hdrrmac4drv.per_carrier_info[ch].carrier_state == 
        HDRRMAC4DRV_CH_ST_TO_BE_INACTIVE )
      {
#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
        if (ut)
        {
          filt_txp_dbm[ch] = 0x7FFFFFFF - 
            (2*hdrrmac4drv_ut_filt_txp_delta) - 1;
        }
        else
#endif
        {
          filt_txp_dbm[ch] = 0x7FFFFFFF - 
            (2*HDRRMAC4DRV_FILT_TXP_DELTA) - 1;
        }
      }

    } /* carrier enabled */
    else
    {
      /* Assign a high filt_txp value to unassigned channels so that 
         they are assigned lowest RTCMAC priorities */
      filt_txp[ch] = 0xFFFFFFFF;
#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
      if (ut)
      {
        filt_txp_dbm[ch] = 0x7FFFFFFF - hdrrmac4drv_ut_filt_txp_delta;
      }
      else
#endif
      {
        filt_txp_dbm[ch] = 0x7FFFFFFF - HDRRMAC4DRV_FILT_TXP_DELTA;
      }

    } /* carrier disabled */

    /* Set all entries in txCarriersOrderedByRank to -1.
       If there are 'n' open state carriers, the first 'n' entries in 
       the list will be populated in rank-order by 
       hdrrmac4drv_update_carrier_priorities(). */
    fw_cmd.rmac_subframe_cfg.txCarriersOrderedByRank[idx] = 
      HDRRMAC4DRV_RANKED_CARRIER_LIST_EMPTY_ENTRY;

  } /* for all carriers */

  /* Calculate carrier priorities and populate list of tx carriers 
     ordered by overall priorities */
  hdrrmac4drv_update_carrier_priorities( filt_txp_dbm, 
    &fw_cmd.rmac_subframe_cfg.txCarriersOrderedByRank[0] );

  /* Convert sum_pilot_pwr in Q10.22 linear units to Q.1 dB.
     This value is used in hdrrmac4drv_get_total_tx_pilot_pwr() */
  hdrrmac4drv.total_tx_pilot_pwr = 
    (int8) ( ( hdrrmacmath_lin_2_db((sum_pilot_pwr + 
    (((uint32)1) << 11)) >> 12)
    + (1 << 18) ) >>19 );

  /* Check tx pilot power differential.  This must be done after updating 
     carrier priorities and before calculating TxT2PMax_PS in this function.
     It uses filtered pilot power stored at hdrrmac4drv.per_carrier_info[] 
     rather than local filt_txp_dbm.  Local filt_txp_dbm has values 
     corresponding to to-be-inactive carriers set to max value to 
     facilitate sinking to-be-inactive carriers lower in RTCMAC
     priority. */
  (void) hdrrmac4drv_check_pilot_power_differential();

  /* Compute maximum power required to support the highest data rate on all 
     assigned carriers */
  for (idx=0; idx < hdrrmac4drv.num_chans; idx++)
  {
    ch = hdrrmac4drv.open_chans[idx];
#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
    if (ut)
    {
      /* Obtain TxT2Pmax(pilotStrength) in Q.2 dB. */
      txt2p_max_ps[ch] = 
        hdrrmac4drv_headroom_ut_input[hdrrmac4drv_ut_sf].c[ch].txt2p_max_ps;

      /* Maximum required power in Q.22 mW
         = Sum_AllActiveCarriers{ Margin*PilotPower*(1+OvG
             + MIN[TxT2P_maxrate_lin*(1+AuxPilotGain_lin), TxT2Pmax_lin]) }
         = Sum_AllActiveCarriers{ Margin*PilotPower*(PilotPlusOvhdGain
             + db2lin( MIN[max_db,TxT2Pmax] ) ) */
      max_req_carrier_pwr[ch] = (uint32) MIN( (uint64) 0xFFFFFFFF, ( ( ( ( 
        ( (uint64)tx_pwr_margin[ch] )*txp[ch] + (((uint32)1) << 9) ) 
        >> 10 ) * ( hdrrmacmath_db_2_lin( ( MIN( 
        hdrrmac4drv_ut_t2p_maxrate_db, txt2p_max_ps[ch] ) ) << 18 ) ) 
        + (((uint32)1) << 9) ) >> 10 ) );

      max_req_carrier_tx_pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, ( ( ( ( 
        ( (uint64)tx_pwr_margin[ch] )*txp[ch] + (((uint32)1) << 9) ) 
        >> 10 ) * ( hdrrmac4drv_headroom_ut_pilot_plus_ovg[ch] ) 
        + (((uint32)1) << 9) ) >> 10 ) + max_req_carrier_pwr[ch] );

      max_req_tx_pwr = HDRRMAC4DRV_ADD_UINT_SAT( max_req_tx_pwr, 
        max_req_carrier_tx_pwr, 0xFFFFFFFF );

    } /* ut mode */
    else
#endif /* FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST */
    {
      /* In non-UT mode, only consider the carriers not under DRC supervision
         for max_req_tx_pwr calculation */

      if ( !(hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired) )
      {
        /* Compute TxT2Pmax(pilotStrength). All inputs are in Q.2 (dB/4). */
        txt2p_max_ps_pre_aci_adj = (uint32) hdrrmacmath_interpolate_1d(
          hdrrmac4drv.per_carrier_info[ch].best_sector_db, 
          hdrrmac4drv.per_carrier_info[ch].channel_config->
          txt2p_max_ptr->ps_axis,
          hdrrmac4drv.per_carrier_info[ch].channel_config->
          txt2p_max_ptr->txt2p_max_ps_axis,
          hdrrmac4drv.per_carrier_info[ch].channel_config->
          txt2p_max_ptr->num_ps_axis_values );

        /* Compute ACI mitigation cap on TxT2Pmax_PS. */
        txt2p_max_ps_aci_cap = (uint32) ( MAX( ( 
          hdrrmac4drv.per_carrier_info[ch].txt2p_max_ps_leftmost - 
          hdrrmac4drv.per_carrier_info[ch].filt_txp_diff ) , 0 ) );

        /* Store the minimum of the above two values as TxT2Pmax_PS. */
        txt2p_max_ps[ch] = MIN( txt2p_max_ps_pre_aci_adj , txt2p_max_ps_aci_cap );

        /* Maximum required power in Q.22 mW
           = Sum_AllActiveCarriers{ Margin*PilotPower*(1+OvG
               + MIN[TxT2P_maxrate_lin*(1+AuxPilotGain_lin), TxT2Pmax_lin]) }
           = Sum_AllActiveCarriers{ Margin*PilotPower*(PilotPlusOvhdGain
               + db2lin( MIN[max_db,TxT2Pmax] ) ) */
        max_req_carrier_pwr[ch] = (uint32) MIN( (uint64) 0xFFFFFFFF, ( ( ( (
          ( (uint64)tx_pwr_margin[ch] )*txp[ch] + (((uint32)1) << 9) ) 
          >> 10 ) * ( hdrrmacmath_db_2_lin( ( MIN(
          ( hdrrmac4drv_config->t2p_info[HDRRMACCMN_TX_MODE_LO_LAT].
          tx_t2p_info[HDRRMACCMN_12288_BITS].max_db ), 
          txt2p_max_ps[ch] ) ) << 18 ) ) 
          + (((uint32)1) << 9) ) >> 10 ) );

        max_req_carrier_tx_pwr = (uint32) MIN( (uint64) 0xFFFFFFFF, ( ( ( (
          ( (uint64)tx_pwr_margin[ch] )*txp[ch] + (((uint32)1) << 9) ) 
          >> 10 ) * 
          ( hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains ) 
          + (((uint32)1) << 9) ) >> 10 ) + max_req_carrier_pwr[ch] );

        max_req_tx_pwr = HDRRMAC4DRV_ADD_UINT_SAT( max_req_tx_pwr, 
          max_req_carrier_tx_pwr, 0xFFFFFFFF );

      }


    } /* non-ut mode */

    /* Store TxT2Pmax(PilotStrength) for logging */
    hdrrmac4drv.per_carrier_info[ch].txt2p_max_ps = txt2p_max_ps[ch];

    HDR_MSG_DEBUG_2( HDR_MSG_DBG_PS_SELECTION, 
      "Chs 0->%d, MaxReqTxPwr: %d Q.22 mW",
      ch,
      max_req_tx_pwr );

  } /* for all OPEN channels */


  /* Check for throttle disable flag.  If the flag is set, 
     - skip both headroom computation and carrier management
     - set per-carrier max_t2p to txt2p_max_ps */
  if (hdrrmac4drv.throttle_disable == TRUE)
  {
    /* Since carrier management is not supposed to run when 
       throttle_disable is set, all OPEN state carriers are 
       assumed to be in DATA_ACTIVE state. */
    for (idx=0; idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
    {
      ch = hdrrmac4drv.open_chans[idx];

      if (idx < hdrrmac4drv.num_chans)
      {
        /* Channel in OPEN state. */

        /* Don't limit with PA headroom if throttling is disabled. 
           Set per-carrier max_t2p to txt2p_max_ps. */
        max_t2p_ptr[ch] = txt2p_max_ps[ch];

        /* Set max allocated per-carrier transmit power in MDSP 
           to max value. */
        fw_cmd.rmac_subframe_cfg.txTotalMaxAllocated[ch] = 0x7FFF;
      }
      else
      { 
        /* Channel not in OPEN state. */

        /* Disable traffic on the carrier. */
        max_t2p_ptr[ch] = HDRRMAC4DRV_NEG_INFINITY_T2P;

        /* Set max allocated per-carrier transmit power in MDSP to 
           min value */
        fw_cmd.rmac_subframe_cfg.txTotalMaxAllocated[ch] = -32768;
      }
    }

    /* Send RMAC subframe config command to firmware */
    estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    return;
  }


  HDR_MSG_DEBUG_2( HDR_MSG_DBG_PS_SELECTION, 
    "MaxReqTxPwr: %u, PMax: %d Q.22 mW",
    max_req_tx_pwr,
    p_max );

  if (max_req_tx_pwr < p_max)
  {
    /* Sufficient power => Bypass PA headroom algorithm */
    hdrm_run_decision = FALSE;

    if (hdrrmac4drv.carrier_config_id == CARRIER_CONFIG_P012_D012)
    {
      /* Sufficient power + all carriers active configuration
         => Bypass carrier management */
      cm_run_decision = FALSE;
    }
    else
    {
      /* Run carrier management */
      hdrrmac4drv_update_carrier_config( filt_txp_mgn );
    }

    for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      if (hdrrmac4drv.per_carrier_info[ch].carrier_state == 
        HDRRMAC4DRV_CH_ST_DATA_ACTIVE)
      {
        max_t2p_ptr[ch] = txt2p_max_ps[ch];

        /* Set p_data_max_per_carrier to max value */
        p_data_max_per_carrier_db[ch] = 0x7FFF;
      }
      else
      {
        max_t2p_ptr[ch] = HDRRMAC4DRV_NEG_INFINITY_T2P;

        /* Set p_data_max_per_carrier to min value */
        p_data_max_per_carrier_db[ch] = -32768;
      }

      /* Set max allocated transmit power on the carrier */
      fw_cmd.rmac_subframe_cfg.txTotalMaxAllocated[ch] = 
        p_data_max_per_carrier_db[ch];

    }

    HDR_MSG_DEBUG_3( HDR_MSG_DBG_PS_SELECTION, 
      "Sufficient Power.  MaxT2P_c = TxT2Pmax_c = %d,%d,%d Q.2dB.",
      txt2p_max_ps[0],
      txt2p_max_ps[1],
      txt2p_max_ps[2] );

  }
  else
  {
    /* Run carrier management */
    hdrrmac4drv_update_carrier_config( filt_txp_mgn );

    /* PA headroom algorithm */

    /* Determine total available power for traffic */
    if ( hdrrmac4drv.in_funnel_mode )
    {
      p_data_max = p_max_neq;
    }
    else
    {
    p_data_max = p_max;
    }

    for (idx=0; idx < hdrrmac4drv.num_chans; idx++)
    {
      ch = hdrrmac4drv.open_chans[idx];

      if (p_data_max > 0)
      {
        /* Overhead Gains in Q.22 mW */
        overhead_chan_pow[ch] = ( ( ( ((uint64)tx_pwr_margin[ch])*txp[ch] 
          + (((uint32)1) << 9) ) >> 10 ) * 
          hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains 
          + (((uint32)1) << 9) ) >> 10;

        if (p_data_max > overhead_chan_pow[ch])
        {
          /* (Total power - Overhead Gains) in Q.22 mW */
          p_data_max = p_data_max - overhead_chan_pow[ch];
        }
        else
        {
          p_data_max = 0;
        }
      }

      /* Determine sum of inverse of filtered pilot powers in Q22.10.
         Exclude channels disabled due to DRC supervision. */
      if ( (hdrrmac4drv.per_carrier_info[ch].carrier_state != 
        HDRRMAC4DRV_CH_ST_DATA_INACTIVE ) &&
        !(hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired) )
      {
        sum_inverse_filt_txp += inv_filt_txp[ch];
      }

    } /* for all OPEN channels */

#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
    if (ut)
    {
      /* tx_t2p_min_db is in Q.2 dB units. */
      tx_t2p_min_db = hdrrmac4drv_ut_txt2pmin_db;
    }
    else
#endif
    {
      /* tx_t2p_min_db is in Q.2 dB units. */
      tx_t2p_min_db = hdrrmac4drv_config->common_pow_params.tx_t2p_min_db;
    }

    /* tx_t2p_min is in Q.10 linear units. */
    tx_t2p_min = hdrrmacmath_db_2_lin( tx_t2p_min_db << 18 );

    /* Minimum tx power reserved for SLP carrier in Q.22 mW */
    min_pow_slp_carrier = (uint32)( ( ( ( ((uint64) tx_t2p_min) * 
      txp[hdrrmac4drv.slp_carrier.current] + (((uint32)1) << 9) ) >> 10 ) *
      tx_pwr_margin[hdrrmac4drv.slp_carrier.current] + (((uint32)1) << 9) )
      >> 10 );

    HDR_MSG_DEBUG_6( HDR_MSG_DBG_PS_SELECTION, 
      "p_data_max:%dQ.22mW,Ovd0:%d,Ovd1:%d,Ovd2:%d,TxT2Pmin:%dQ.10Lin,MinPowSLP:%dQ.22mW",
      p_data_max,
      overhead_chan_pow[0],
      overhead_chan_pow[1],
      overhead_chan_pow[2],
      tx_t2p_min,
      min_pow_slp_carrier );

#ifdef HDRRMAC4DRV_EQUAL_HEADROOM_DISTRIBUTION
    if (hdrrmac4drv_enable_equal_headroom_dist)
    {
      num_chans_pwr_dist = hdrrmac4drv.num_chans;

      if ( hdrrmac4drv.in_funnel_mode )
      {
        /* Available power in funnel mode */
        eq_dist_available_pwr = p_max_neq;
      }
      else
      {
        /* Available power when not in funnel mode */
      eq_dist_available_pwr = p_max;
      }

      if (num_chans_pwr_dist != 0)
      {
        /* Exclude carriers under DRC supervision from headroom 
           distribution. */
        for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
        {
          if ( hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired )
          {
            num_chans_pwr_dist--;
            eq_dist_excluded_chan[ch] = TRUE;

          }
        }

        if (num_chans_pwr_dist != 0)
        {
          /* Calculate per-carrier fair share */
          eq_dist_per_carrier_pwr = eq_dist_available_pwr / num_chans_pwr_dist;
        }
        else
        {
          /* All the carriers are disabled due to DRC supervision.  
             Here the value of eq_dist_per_carrier_pwr is immaterial */
          eq_dist_per_carrier_pwr = eq_dist_available_pwr;
        }

        /* Find 
           - available power
           - number of candidate channels for headroom distribution
           - channels to be excluded from headroom distribution. */
        for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
        {
          if ( hdrrmac4drv.per_carrier_info[ch].carrier_state == 
            HDRRMAC4DRV_CH_ST_DATA_INACTIVE )
          {
            /* Subtract overhead channel power from total available power 
               for carriers in DATA_INACTIVE state */
            eq_dist_available_pwr = (uint32) (MAX( ((int64)(0)), 
              ((int64)(eq_dist_available_pwr)) - 
              MIN(overhead_chan_pow[ch], eq_dist_per_carrier_pwr) ));

            /* Exclude DATA_INACTIVE carriers from headroom distribution */
            num_chans_pwr_dist--;
            eq_dist_excluded_chan[ch] = TRUE;

          }
        } /* for all channels */

        /* Re-calculate per-carrier fair share */
        if (num_chans_pwr_dist != 0)
        {
          eq_dist_per_carrier_pwr = eq_dist_available_pwr / num_chans_pwr_dist;

        }
        else
        {
          HDR_MSG_DEBUG( HDR_MSG_DBG_PS_SELECTION, 
            "No DATA_ACTIVE channels!" );

          eq_dist_per_carrier_pwr = eq_dist_available_pwr;
        }
      }
      else
      {
        HDR_MSG_DEBUG( HDR_MSG_DBG_PS_SELECTION, "No OPEN channels!" );
        eq_dist_per_carrier_pwr = eq_dist_available_pwr;
      }

      for (idx=0; idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
      {
        ch = hdrrmac4drv.open_chans_data[idx];

        if ( ( idx < hdrrmac4drv.num_chans_data ) &&
          ( !eq_dist_excluded_chan[ch] ) )
        {
          /* Distribute available headroom evenly among all OPEN carriers
             that are 
             - not in DATA_INACTIVE state 
             - not under DRC supervision */
          p_data_max_per_carrier[ch] = (uint32) ( MAX( ((int64)(0)), 
            (((int64)(eq_dist_per_carrier_pwr)) - overhead_chan_pow[ch]) ) );
        }
        else
        {
          /* Set PDataMax to zero for all the carriers that are not in OPEN
             state */
          p_data_max_per_carrier[ch] = 0;
        }

      } /* for all channels */

      HDR_MSG_DEBUG_6( HDR_MSG_DBG_PS_SELECTION, 
        "EqPerCPwr:%d,Pdata0:%d,Pdata1:%d,Pdata2:%d,Slp:%d,MinSlpPow:%d",
        eq_dist_per_carrier_pwr,
        p_data_max_per_carrier[0],
        p_data_max_per_carrier[1],
        p_data_max_per_carrier[2],
        hdrrmac4drv.slp_carrier.current,
        min_pow_slp_carrier );

      /* Make sure SLP carrier gets at least min_t2p */
      p_data_max_per_carrier[hdrrmac4drv.slp_carrier.current] = 
        MAX( p_data_max_per_carrier[hdrrmac4drv.slp_carrier.current],
        min_pow_slp_carrier );
    }
    else
    {
#endif 

      /* Determine SLP carrier share from total available power in Q.22 mW
         based on inverse proportion distribution. */
      if (sum_inverse_filt_txp != 0)
      {
        num2 = (uint32) ((((uint64) p_data_max ) * 
          inv_filt_txp[hdrrmac4drv.slp_carrier.current] ) / 
          sum_inverse_filt_txp );
      }
      else
      {
        HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
          "Zero SumInverseFiltTxP!!" );

        num2 = 0;

      }

      /* SLP carrier power in Q.22 */
      p_data_max_per_carrier[hdrrmac4drv.slp_carrier.current] = 
        MAX( min_pow_slp_carrier, num2);

      /* Total available traffic power for the rest of the carriers */
      p_data_max = (uint32) MAX(0, ((int64) p_data_max) - 
        p_data_max_per_carrier[hdrrmac4drv.slp_carrier.current]);

      HDR_MSG_DEBUG_3( HDR_MSG_DBG_PS_SELECTION, 
        "New_p_data_max: %d,slp_carrier: %d,p_data_slp_carrier: %d",
        p_data_max,
        hdrrmac4drv.slp_carrier.current,
        p_data_max_per_carrier[hdrrmac4drv.slp_carrier.current] );

      /* Calculate sum_inverse_filt_txp_minus_slp */
      if ( !(hdrrmac4drv.per_carrier_info
        [hdrrmac4drv.slp_carrier.current].drc_supervision_expired) )
      {
        /* Subtract the contribution due to SLP carrier from 
           sum_inverse_filt_txp */
        sum_inverse_filt_txp_minus_slp = sum_inverse_filt_txp - 
          inv_filt_txp[hdrrmac4drv.slp_carrier.current];
      }
      else
      {
        /* Set sum_inverse_filt_txp_minus_slp to sum_inverse_filt_txp */
        sum_inverse_filt_txp_minus_slp = sum_inverse_filt_txp;
      }

      /* Distribute available power among non-SLP data-carrying carriers 
         that are not under DRC supervision */
      for (idx=0; idx < hdrrmac4drv.num_chans_data; idx++)
      {
        ch = hdrrmac4drv.open_chans_data[idx];

        if ( (ch != hdrrmac4drv.slp_carrier.current) && 
          !(hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired) )
        {
          if ( p_data_max > 0 )
          {
            /* Carrier share from total available power in Q.22 mW */
            if (sum_inverse_filt_txp_minus_slp != 0)
            {
              p_data_max_per_carrier[ch] = (uint32) ((((uint64) p_data_max ) *
                inv_filt_txp[ch] ) / sum_inverse_filt_txp_minus_slp );
            }
            else
            {
              HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
                "Zero SumInverseFiltTxP for NonSLP carrier!!" );

              p_data_max_per_carrier[ch] = 0;
            }
          }
          else
          {
            /* There is no available power for data on the carrier */
            p_data_max_per_carrier[ch] = 0;
          }

        } /* non-SLP carrier not under DRC supervision */

      } /* for all data carrying carriers */

      /* Calculate extra power available */
      for (idx=0; idx < hdrrmac4drv.num_chans_data; idx++)
      {
        ch = hdrrmac4drv.open_chans_data[idx];

        if (!(hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired))
        {
          /* Calculate extra power */
          /* filt_rab_limited_p_data is updated in 
             update_sum_pot_t2p_outflows() */
          if ( p_data_max_per_carrier[ch] > 
               hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data )
          {
            /* A carrier is deemed to have extra power in RAB based 
               reallocation if 
               - Its allocated data power > RAB limited data power AND
               - Allocated power for SLP carrier does not go below min
                 SLP carrier power */
            if ( ( ch != hdrrmac4drv.slp_carrier.current ) ||
              ( hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data >
              min_pow_slp_carrier ) )
            {
              /* Carrier has extra power */
              extra_power_candidate[ch] = GIVER;

              extra_power_rab += (p_data_max_per_carrier[ch] - 
                hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data);

            }
            else
            {
              /* SLP has more than its filtered RAB limited power but we 
                 cannot take away the extra power since the RAB limited power
                 is less than minSLPpwr.  In this situtation:
                 - Do not trigger RAB limited power reallocation.
                 - Count the carrier as a pwr taker if power reallocation 
                   is executed. */
              extra_power_candidate[ch] = TAKER;
            }
          }
          else
          {
            /* Carrier needs extra power */
            extra_power_candidate[ch] = TAKER;

            /* Set extra power candidates flag */
            extra_power_candidates_rab = TRUE;

          }

          HDR_MSG_DEBUG_5( HDR_MSG_DBG_PS_SELECTION, 
            "Ch:%d, p_data_c: %d,filt_rab_p:%d Q.22 mW,ExtraRABPwrCand:%d,ExtraPwr:%d",
            ch,
            p_data_max_per_carrier[ch],
            hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data,
            extra_power_candidate[ch],
            extra_power_rab );

        } /* if carrier is not disabled due to DRC supervision */

      } /* for all data carrying carriers */

      /* Redistribute extra power based on RAB based reallocation */
      for (idx=0; idx < hdrrmac4drv.num_chans_data; idx++)
      {
        ch = hdrrmac4drv.open_chans_data[idx];

        if (!(hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired))
        {
          /* Redistribute extra power if 
             - there is atleast one carrier with extra power AND
             - there is atleast one carrier in need of extra power */
          if ((extra_power_candidates_rab) && (extra_power_rab > 0))
          {
            /* Calculate extra power to be given to this carrier.
               A carrier's share of extra power is inversely proportional
               to its filtered transmit pilot power */
            if (sum_inverse_filt_txp != 0)
            {
              extra_power_c = (uint32) ((((uint64) extra_power_rab ) * 
                inv_filt_txp[ch] ) / sum_inverse_filt_txp );
            }
            else
            {
              extra_power_c = 0;

              HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
                "Zero SumInverseFiltTxP!!" );
            }

            if (extra_power_candidate[ch] == TAKER)
            {
              /* The carrier is eligible for extra power 
                 (is not RAB limited) */
              p_data_max_per_carrier[ch] = HDRRMAC4DRV_ADD_UINT_SAT( 
                p_data_max_per_carrier[ch], extra_power_c, 0xFFFFFFFF );


            }
            else if (extra_power_candidate[ch] == GIVER)
            {
              /* Carrier has extra power */
              p_data_max_per_carrier[ch] = HDRRMAC4DRV_ADD_UINT_SAT( 
                hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data,
                extra_power_c, 0xFFFFFFFF );
            }

          } /* Redistribute extra power obtained from RAB limited carriers */

          /* Reset extra power candidate status */
          extra_power_candidate[ch] = NEITHER;

          /* Find candidates for TxT2PMax based power reallocation */
          if (p_data_max_per_carrier[ch] > max_req_carrier_pwr[ch])
          {
            /* A carrier is deemed to have extra power in TxT2PMax based 
               reallocation if 
               - Its allocated data power > Max power required to support 
                                            TxT2PMax
                 AND
               - Allocated power for SLP carrier does not go below min
                 SLP carrier power */
            if ( ( ch != hdrrmac4drv.slp_carrier.current ) ||
              ( max_req_carrier_pwr[ch] > min_pow_slp_carrier ) )
            {
              /* Carrier has extra power */
              extra_power_candidate[ch] = GIVER;

              /* Extract extra power */
              extra_power_txt2pmax += 
                ( p_data_max_per_carrier[ch] - max_req_carrier_pwr[ch] );
            }
            else
            {
              /* SLP has more than its TxT2PMax based power but we 
                 cannot take away the extra power since the TxT2PMax power
                 is less than minSLPpwr.  In this situtation, SLP is neither
                 giver, nor taker */
              extra_power_candidate[ch] = NEITHER;
            }
          }
          else
          {
            /* Carrier needs extra power */
            extra_power_candidate[ch] = TAKER;

            /* Set extra power candidates flag */
            extra_power_candidates_txt2pmax = TRUE;

            sum_inverse_filt_txp_extra_power += inv_filt_txp[ch];

          }

          HDR_MSG_DEBUG_7( HDR_MSG_DBG_PS_SELECTION, 
            "Ch:%d, p_data_c: %d,TxT2PMax:%d Q.2dB %u Q.10Lin,TxT2PMax_p:%u Q.22 mW,ExtrTxT2PMaxPwrCand:%d,ExtrPwr:%d",
            ch,
            p_data_max_per_carrier[ch],
            txt2p_max_ps[ch],
            hdrrmacmath_db_2_lin(txt2p_max_ps[ch] << 18),
            max_req_carrier_pwr[ch],
            extra_power_candidate[ch],
            extra_power_txt2pmax );

        } /* if carrier is not disabled due to DRC supervision */

      } /* for all data carrying carriers */

      /* Redistribute extra power based on TxT2PMax based reallocation if 
         - there is atleast one carrier with extra power AND
         - there is atleast one carrier in need of extra power */
      if ((extra_power_candidates_txt2pmax) && (extra_power_txt2pmax > 0))
      {
        for (idx=0; idx < hdrrmac4drv.num_chans_data; idx++)
        {
          ch = hdrrmac4drv.open_chans_data[idx];

          if (extra_power_candidate[ch] == TAKER)
          {
            /* Calculate extra power to be given to this carrier.
               A carrier's share of extra power is inversely proportional
               to its filtered transmit pilot power */
            if (sum_inverse_filt_txp_extra_power != 0)
            {
              extra_power_c = (uint32) MIN( ((((uint64) extra_power_txt2pmax ) 
                * inv_filt_txp[ch] ) / sum_inverse_filt_txp_extra_power ), 
                ( max_req_carrier_pwr[ch] - p_data_max_per_carrier[ch] ) );

            }
            else
            {
              extra_power_c = 0;

              HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
                "Zero SumInverseFiltTxP!!" );
            }

            /* Add extra power to carrier data power */
            p_data_max_per_carrier[ch] = HDRRMAC4DRV_ADD_UINT_SAT( 
              p_data_max_per_carrier[ch], extra_power_c, 0xFFFFFFFF );

            /* Subtract power from extra power pool */
            extra_power_txt2pmax -= extra_power_c;

            /* Adjust sum of inverse of filtered transmit pilot powers */
            sum_inverse_filt_txp_extra_power -= inv_filt_txp[ch];

          } /* Extra power candidate */
          else if (extra_power_candidate[ch] == GIVER)
          {
            /* Carrier has extra power 
               => Take away the extra power */
            p_data_max_per_carrier[ch] = max_req_carrier_pwr[ch];

          }

        } /* for all data carrying carriers */

      } /* if TxT2PMax based reallocation is needed */

#ifdef HDRRMAC4DRV_EQUAL_HEADROOM_DISTRIBUTION
    }
#endif

    /* Calculate maximum available TxT2P in every assigned carrier */
    /* Zero out power on data-disallowed carriers */
    for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      if ( ( hdrrmac4drv.per_carrier_info[ch].carrier_state == 
        HDRRMAC4DRV_CH_ST_DATA_ACTIVE ) && 
        !( hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired ) )
      {
        /* Maximum available TxT2P for the carrier in Q.10 linear */
        /* Power margin is in Q.10
           p_data_max_per_carrier and txp are in Q.22 */
        if ((tx_pwr_margin[ch] != 0) && (txp[ch] != 0))
        {
          num1 = (uint32)((((((uint64) p_data_max_per_carrier[ch])<<10) /
            txp[ch])<<10)/tx_pwr_margin[ch]);
        }
        else
        {
          /* Just assign some value that will not trigger an invalid
             input to lin2db function */
          num1 = min_pow_slp_carrier;

          /* Report error */
          HDR_MSG_DRIVERS_3( MSG_LEGACY_ERROR, 
            "Ch:%d, Zero TxP(%d) or TxPwrMargin(%d)!!",
            ch,
            txp[ch],
            tx_pwr_margin[ch] );
        }


        /* Result obtained in Q.2 dB
           Q22.10 linear -> Q11.20 dB (after lin2db) -> Q.2 (after shift) */
        max_t2p_db = (hdrrmacmath_lin_2_db( num1 ) + (((int32)1) << 17)) >> 18;
        max_t2p_ptr[ch] = (uint32)(((max_t2p_db - 
          hdrrmac4drv.per_carrier_info[ch].best_sector_delta_t2p) > 0) ?
          max_t2p_db: 0);


        max_t2p_ptr[ch] = MIN(max_t2p_ptr[ch], txt2p_max_ps[ch]);

        /* Calculate p_data_max_per_carrier in scaled dB units 
           P_data_max_per_carrier_db
           = P_data_max_per_carrier+Ovhd_pow in (65536/DynamicRange) dB units
           = (K + 63.8 - (P_data_max_per_c+Ovhd_pow)) * (65536/DynamicRange)
           = ((K<<10 + 65331 - (P_data_max_per_c+Ovhd_pow)_Q.10)*5)>>3 */
        p_data_max_per_carrier_db[ch] = 
          ( ( (hdrrmac4drv.k << 10) + 65331 - 
          ( hdrrmacmath_mw_2_dbm( p_data_max_per_carrier[ch] + 
          overhead_chan_pow[ch] ) ) ) * 5 + (1 << 2) ) >> 3;

        max_t2p_ptr[ch] = ((max_t2p_ptr[ch] == 0)? 
          HDRRMAC4DRV_NEG_INFINITY_T2P: max_t2p_ptr[ch]);

        HDR_MSG_DEBUG_9( HDR_MSG_DBG_PS_SELECTION, 
          "ch: %d,PDataMax:%d,Hdrm:%d,%d,%d,TxT2PMax:%d,Min:%d,best_sector_db:%d,PMaxAlloc:%d",
          ch,
          p_data_max_per_carrier[ch],
          num1,
          max_t2p_db,
          max_t2p_ptr[ch],
          txt2p_max_ps[ch],
          tx_t2p_min_db,
          hdrrmac4drv.per_carrier_info[ch].best_sector_db,
          p_data_max_per_carrier_db[ch] );

      } /* Channel allowed to send new data */
      else
      {
        /* Channel disallowed to send data - set T2P to neg infinity */
        max_t2p_ptr[ch] = HDRRMAC4DRV_NEG_INFINITY_T2P;

        /* Set p_data_max_per_carrier to min value */
        p_data_max_per_carrier_db[ch] = -32768;

      }

      /* Set max allocated transmit power on the carrier */
      fw_cmd.rmac_subframe_cfg.txTotalMaxAllocated[ch] =
        p_data_max_per_carrier_db[ch];

    } /* for all carriers */

  }  /* PA headroom algorithm */

  /* Populate funnel mode fields in FW msg */
  if ( hdrrmac4drv.enh_pa_backoff_enabled )
  {
    fw_cmd.rmac_subframe_cfg.txFunnelModeCfg.funnelStateValid = 1;

    if ( hdrrmac4drv.in_funnel_mode )
    {
      fw_cmd.rmac_subframe_cfg.txFunnelModeCfg.funnelState = 1;
      fw_cmd.rmac_subframe_cfg.txFunnelModeCfg.dataActiveCarrierIndex = 
        hdrrmac4drv.open_chans_data[0];

      /* TODO: remove this message feature verification */
      HDR_MSG_DRIVERS_1( MSG_LEGACY_MED, 
        "In funnel mode. DataActiveCarrierIndex = %d",
        hdrrmac4drv.open_chans_data[0] );
    }
  }

  /* Send RMAC subframe config command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  /* Save total available tx power used in headroom calculation in 
     Q.2 dBm for logging */
  if ( ( hdrrmac4drv.enh_pa_backoff_enabled ) && 
    ( hdrrmac4drv.in_funnel_mode ) )
  {
    hdrrmac4drv_gain_log_info.max_avail_tx_pwr = (p_max_neq_dbm+255) >> 8;
  }
  else
  {
    hdrrmac4drv_gain_log_info.max_avail_tx_pwr = (p_max_dbm+255) >> 8;
  }

  /* TODO: remove this message after feature verification */
  HDR_MSG_DRIVERS_2( MSG_LEGACY_MED, 
    "Max Tx Pwr (Q.10 dBm): eq = %d, Neq = %d",
    p_max_dbm,
    p_max_neq_dbm );

  /*---------------------------------------------------------------------*/

  /* Print final max_t2p's */
  HDR_MSG_DEBUG_3( HDR_MSG_DBG_PS_SELECTION, 
    "Final max_t2p: %d, %d, %d",
    max_t2p_ptr[0],
    max_t2p_ptr[1],
    max_t2p_ptr[2] );

  /* Submit values for logging */
  hdrrmac4drv_log_pa_hdrm_cm_info( dsp, hdrm_run_decision, 
    cm_run_decision, max_t2p_ptr );

#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST
  if (ut)
  {
    /* - Restore states so that UT changes in invisible outside this
         module
       - Increment UT subframe number */

    hdrrmac4drv.num_chans = pre_ut_num_chans;

    hdrrmac4drv.carrier_config_id = pre_ut_carrier_config;

    /* Restore CM thresholds */
    hdrrmac4drv_carrier_mgt_thresh.t2p_remain_active = 
      pre_ut_cm_thresholds.t2p_remain_active;

    hdrrmac4drv_carrier_mgt_thresh.t2p_activate = 
      pre_ut_cm_thresholds.t2p_activate;

    hdrrmac4drv_carrier_mgt_thresh.t2p_deactivate = 
      pre_ut_cm_thresholds.t2p_deactivate;

    hdrrmac4drv_carrier_mgt_thresh.t2p_min_rate = 
      pre_ut_cm_thresholds.t2p_min_rate;

    hdrrmac4drv_carrier_mgt_thresh.t2p_add = 
      pre_ut_cm_thresholds.t2p_add;

    for (ch=0; ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
    {
      /* Restore channel state */
      hdrrmac4drv.per_carrier_info[ch].carrier_state = 
        pre_ut_channel_state[ch];

      /* Restore RTCMAC prioritized carriers */
      hdrrmac4drv.prioritized_carriers.rtcmac[ch] = 
        pre_ut_rtcmac_prioritized_carriers[ch];

      /* Restore overall prioritized carriers */
      hdrrmac4drv.prioritized_carriers.overall[ch] = 
        pre_ut_overall_prioritized_carriers[ch];

      /* Restore drop ranks */
      hdrrmac4drv.per_carrier_info[ch].drop_rank = 
        pre_ut_drop_ranks[ch];

      /* Restore pilot plus overhead gains */
      hdrrmac4drv.per_carrier_info[ch].pilot_plus_overhead_gains = 
        pre_ut_pilot_plus_ovhd_gains[ch];

      /* Disallow data on disabled channels by setting T2P to neg infinity */
      if ( (!((((uint8)hdrrmac4drv.per_carrier_info[ch].carrier_state) & 
        ((uint8)HDRRMAC4DRV_CH_ST_MAJOR_MASK)) == 
        ((uint8)HDRRMAC4DRV_CH_ST_OPEN))) || 
        (hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired) )
      {
        max_t2p_ptr[ch] = HDRRMAC4DRV_NEG_INFINITY_T2P;
      }
    }

    /* Increment UT subframe number */
    hdrrmac4drv_ut_sf = ( hdrrmac4drv_ut_sf + 1 ) % 
      HDRRMAC4DRV_HEADROOM_UT_RUN_LENGTH;
  }
#endif /* FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_get_max_t2p_rev_b() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_MAX_T2P                                    EXTERNAL

DESCRIPTION
  The function returns the maximum T2P value that can be supported for the
  next subpacket for all assigned channels.
  
DEPENDENCIES
  None.

PARAMETERS
  max_t2p_ptr - Pointer to HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS instances of 
                max_t2p (in unit of 1/4 dB).

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_get_max_t2p( uint32 * max_t2p_ptr )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST

  if (hdrrmac4drv_headroom_ut_enable)
  {
    /* Use RevB PA headroom algorithm in unit test mode */
    hdrrmac4drv_get_max_t2p_rev_b( max_t2p_ptr, TRUE );

  }
  else
  {
    /* Use RevB PA headroom algorithm on 7800 if unit test
       mode is not enabled */
    hdrrmac4drv_get_max_t2p_rev_b( max_t2p_ptr, FALSE );
  }

#else

  /* Use RevB PA headroom algorithm on 7800 if unit test
     feature is not defined */
  hdrrmac4drv_get_max_t2p_rev_b( max_t2p_ptr, FALSE );

#endif /* FEATURE_HDR_RMAC4_HEADROOM_UNIT_TEST */

  return;

} /* hdrrmac4drv_get_max_t2p() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_CAPUT_PRIORITIZED_CARRIERS                 EXTERNAL

DESCRIPTION
  Returns list of carriers sorted by their CAPUT priorities; the first
  in the list being the carrier with highest CAPUT priority.
  
DEPENDENCIES
  None.

PARAMETERS
  prioritized_carriers - Pointer to uint8.  It is set to the base address
                         of the table containing CAPUT prioritized 
                         carriers.

RETURN VALUE
  num_chans - Num of enabled carriers.  The first num_chans entries in the 
              prioritized carriers table are valid.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4drv_get_caput_prioritized_carriers
(
  uint8 *prioritized_carriers
)
{
  uint8 ch, idx; /* Loop counters */
  hdrrmac4drv_packet_info_type *priority_info; /* Shorthand pointer   */
  uint8 caput_priorities[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS]; /* Caput Priorities */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (idx=0; idx<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
  {
    ch = hdrrmac4drv.open_chans[idx];

    if (idx < hdrrmac4drv.num_chans)
    {
      /* Channel enabled */

      priority_info = &hdrrmac4drv.per_carrier_info[ch].
        interlace_history[hdrrmac4drv.interlace];

      /* Assign max CAPUT priority to the carrier if - 
       * - there are no packets in progress on the existing interlace OR
       * - the last subpacket was transmitted on the interlace
       */
      if ( (priority_info->rri == 0) || 
        (priority_info->subpacket == HDRRMACCMN_MAX_NUM_SUBPACKETS - 1) )
      {
        caput_priorities[ch] = HDRRMACCMN_MAX_NUM_SUBPACKETS + 
          HDRRMACCMN_MAX_NUM_SUBPACKETS;
      }
      else
      {
        /* determine CAPUT priority for the carrier:
           CAPUT priority = Current subpacket id - termination target 
           HDRRMACCMN_MAX_NUM_SUBPACKETS is added to all OPEN channels.
           This helps sink down non-OPEN channels in the CAPUT prioritized
           carrier list. */
        caput_priorities[ch] = priority_info->subpacket - priority_info->tt + 
          HDRRMACCMN_MAX_NUM_SUBPACKETS;
      }
    }
    else
    {
      /* Set CAPUT priority of disabled channel to zero */
      caput_priorities[ch] = 0x0;
    }

  } /* for all channels */

  hdrrmac4drv_sort_caput_carriers( caput_priorities, 
    hdrrmac4drv.prioritized_carriers.caput );

  for (ch=0; ch<hdrrmac4drv.num_chans; ch++)
  {
    prioritized_carriers[ch] = hdrrmac4drv.prioritized_carriers.caput[ch];
  }
  
#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return hdrrmac4drv.num_chans;

} /* hdrrmac4drv_get_caput_prioritized_carriers() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_TOTAL_TX_PILOT_PWR                         EXTERNAL

DESCRIPTION
  The function returns sum of tx pilot power over all enabled channels.  The
  returned power is in 0.5 dBm units in 2's complement format.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Sum of tx pilot power over all enabled channels.

SIDE EFFECTS
  None.
===========================================================================*/
int8 hdrrmac4drv_get_total_tx_pilot_pwr( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return hdrrmac4drv.total_tx_pilot_pwr;

} /* hdrrmac4drv_get_total_tx_pilot_pwr() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_DROP_RANKS                              EXTERNAL

DESCRIPTION
  Updates reverse link drop rank information.  This information is combined 
  with the RTCMAC priority to obtain overall carrier drop ranks which are 
  written to the MDSP every subframe.
  
DEPENDENCIES
  None.

PARAMETERS
  drop_ranks_ptr - Pointer to MAX_NUM_PHY_CHANS instances of drop ranks 
                   listed by physical channel order.  The drop ranks 
                   corresponding to unassigned channels are set to an 
                   invalid value 0xFF

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_drop_ranks
(
  uint8 *drop_ranks_ptr
)
{
  /* Loop counter */
  uint8 ch;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv.per_carrier_info[ch].drop_rank = drop_ranks_ptr[ch];
  }


#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_update_drop_ranks() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_RL_ADD_DROP_INFO                           EXTERNAL

DESCRIPTION
  Returns channel add and drop information.  If the entry corresponding to
  a channel in channel_dropped[] table is TRUE, the channel needs to be 
  dropped.

DEPENDENCIES
  None.

PARAMETERS
  channel_dropped - Pointer to MAX_NUM_PHY_CHANS instances of channel 
                    dropped statuses arranged in physical channle order.  
                    If the entry corresponding to a channel is TRUE, 
                    the channel needs to be dropped.

RETURN VALUE
  total_supported_carriers - Total number of estimated reverse link
    carriers (including the currently assigned ones) that can be supported
    by the current PA headroom.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4drv_get_rl_add_drop_info( boolean *channel_dropped )
{
  /* Loop counter */
  uint8 ch, total_num_carriers;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    channel_dropped[ch] = 
      (hdrrmac4drv.per_carrier_info[ch].carrier_state == 
      HDRRMAC4DRV_CH_ST_INACTIVE) && 
      (hdrrmac4drv.per_carrier_info[ch].off_till_mcdo_eval ==
      FALSE);
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  total_num_carriers = hdrrmac4drv.num_chans + hdrrmac4drv.num_req_carriers;

  /* Reset number of requested carriers in this subframe */
  hdrrmac4drv.num_req_carriers = 0;

  return (total_num_carriers);

} /* hdrrmac4drv_get_rl_add_drop_info() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_SLP_CARRIER                                EXTERNAL

DESCRIPTION
  Returns the physical channel index of the carrier designated to carry 
  SLP data.
  
DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  Physical channel index of SLP carrier.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4drv_get_slp_carrier( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return (hdrrmac4drv.slp_carrier.current);

} /* hdrrmac4drv_get_slp_carrier() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_PRIORITIZED_CARRIERS                       EXTERNAL

DESCRIPTION
  Returns list of carriers sorted by their RTCMAC priorities; the first
  in the list being the highest priority carrier.  The RTCMAC priority
  of a carriers is inversely proportional to its average tx pilot powers.  
  Because priorities are only assigned to the enabled channels, only the 
  first num_chans entries in the table are valid.
  
DEPENDENCIES
  None.

PARAMETERS
  prioritized_carriers - Pointer to uint8.  It is set to the base address
                         of the table containing RTCMAC prioritized 
                         carriers.

RETURN VALUE
  num_chans - Num of enabled carriers.  The first num_chans entries in the 
              prioritized carriers table are valid.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrmac4drv_get_prioritized_carriers
( 
  uint8* prioritized_carriers
)
{
  /* Loop counter */
  uint8 ch;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (ch=0; ch<hdrrmac4drv.num_chans; ch++)
  {
    prioritized_carriers[ch] = hdrrmac4drv.prioritized_carriers.rtcmac[ch];
  }

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return hdrrmac4drv.num_chans;

} /* hdrrmac4drv_get_prioritized_carriers() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_FLOW0_HAS_DATA                                 EXTERNAL

DESCRIPTION
  Informs driver layer that there is SLP data to be transmitted in this 
  subframe.
  
DEPENDENCIES
  None.

PARAMETERS
  has_data - if TRUE, indicated that the SLP flow has data to transmit.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_flow0_has_data( boolean has_data )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrrmac4drv.slp_carrier.has_data = has_data;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_flow0_has_data() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_SUM_POT_T2P_OUTFLOWS                    EXTERNAL

DESCRIPTION
  Updates SumPotentialT2POutflow information for all assigned channels.
  
DEPENDENCIES
  None.

PARAMETERS
  sum_pot_t2p_outflows_ptr - Pointer to MAX_NUM_PHY_CHANS instances of 
                             SumPotentialT2POutflows

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_sum_pot_t2p_outflows
(
  uint32 *sum_pot_t2p_outflows_ptr
)
{
  /* Loop counter */
  uint8 ch, idx;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (idx=0; idx<hdrrmac4drv.num_chans; idx++)
  {
    ch = hdrrmac4drv.open_chans[idx];

    /* FiltRABLimitedP_Data_n 
       = (1 - 1/TC_RAB) * FiltRABLimitedP_Data_n-1
         + (1/TC_RAB) * SumPotentialT2POutflow_n * T2P_n 
       = FiltRABLimitedP_Data_n-1 
         - (FiltRABLimitedP_Data_n-1 * TC_RAB_mul) >> TC_RAB_shift
         + (SumPotentialT2POutflow_n * TC_RAB_mul) >> TC_RAB_shift */
    hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data -= 
      (uint32)( ( ( (uint64) hdrrmac4drv.per_carrier_info[ch].
      filt_rab_limited_p_data ) * HDRRMAC4DRV_TC_RAB_MUL + 
      (1 << (HDRRMAC4DRV_TC_RAB_SHIFT - 1))) >> HDRRMAC4DRV_TC_RAB_SHIFT );

    hdrrmac4drv.per_carrier_info[ch].filt_rab_limited_p_data += 
      (uint32)( ( ( ( (uint64) sum_pot_t2p_outflows_ptr[ch] ) * 
      hdrrmac4drv.per_carrier_info[ch].txp_inst ) * 
      HDRRMAC4DRV_TC_RAB_MUL + (1 << (HDRRMAC4DRV_TC_RAB_SHIFT - 1)))
      >> HDRRMAC4DRV_TC_RAB_SHIFT );

  } /* for all enabled channels*/

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_update_sum_pot_t2p_outflows() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_OUTSTANDING_PACKET_IDS                     EXTERNAL

DESCRIPTION
  Returns outstanding packet ids on all the interlaces for a given channel.
  All the outstanding packets on the channel are treated as MARQ'ed for 
  logging purposes.  The channel should be disabled after calling this
  function (hdrrmac4drv_stop_channels()) before 
  hdrrmac4drv_get_prev_tx_status() is called.
  
DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index of the channel to be looked up.
  pkt_id_ptr   - Output parameter.  Points to an array of size 
    HDRSCMRMACCMN_MAX_NUM_INTERLACES instances.  Outstanding packet ids
    on all the interlaces on the given channel are returned here.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_get_outstanding_packet_ids
(
  /* Physical channel index */
  uint8 phy_chan_idx,

  /* Pointer to return location where outstanding packet ids are returned. */
  uint8 *pkt_id_ptr
)
{
  /* Loop counter */
  uint8 i;

  /* Convenience pointer */
  hdrrmac4drv_packet_info_type *hist;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (i=0; i < HDRSCMRMACCMN_MAX_NUM_INTERLACES; i++)
  {
    hist = &hdrrmac4drv.per_carrier_info[phy_chan_idx].
      interlace_history[i];

    /* Store packet id */
    pkt_id_ptr[i] = hist->packet_id;

    if (pkt_id_ptr[i] != HDRRMAC4DRV_IDLE_PACKET)
    {
      /* Valid packet id: increment MARQ count */
      hdrrmac4drv_metrics.log[phy_chan_idx].interlace[i].
        tm[hist->tm].rri[hist->rri-1].parq_naks++;
    }
  }

  /* Reset marq_info_valid flag */
  hdrrmac4drv.per_carrier_info[phy_chan_idx].marq_info_valid = FALSE;

#ifdef HDRRMAC4DRV_DEBUG_TRACE_FUNCTION_CALLS
#error code not present
#endif

  return;

} /* hdrrmac4drv_get_outstanding_packet_ids() */

/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_CURRENT_TX_PACKET_INFO

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
boolean hdrrmac4drv_get_current_tx_packet_info
(
  hdrrmac_current_tx_packet_info_type * pkt_info
)
{

  boolean tx_valid;
    /* Whether we can get good info.  If we can't get good info,
       (RL is not on, data not yet updated), don't fill in the pkt
       info structure. */

  uint8 interlace;
    /* Temp for computing the in-transit interlace */

  hdrrmac4drv_packet_info_type * current_packet_info;
    /* Convenience pointer */

  uint8 ch;
    /* Channel loop */

  uint8 chan_entry_index = 0;
    /* Index into carrier array */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  tx_valid = hdrmod_pa_is_on() && hdrrmac4drv.current_tx_updated;    


  HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                   "Tx_packet_info: %d (PA), %d update",
                   hdrmod_pa_is_on(),hdrrmac4drv.current_tx_updated);


  /* Only give pkt info if RL is on. */
  if (tx_valid)
  {

    /* The interlace in the hdrrmac4drv structure is for the *next* subframe,
       so find the interlace for the current subframe by effectively 
       subtracting one. */
    interlace = (hdrrmac4drv.interlace + HDRSCMRMACCMN_MAX_NUM_INTERLACES - 1) % 
      HDRSCMRMACCMN_MAX_NUM_INTERLACES;


    for (chan_entry_index=0; chan_entry_index < hdrrmac4drv.num_chans; 
          chan_entry_index++)
    {
      ch = hdrrmac4drv.open_chans[chan_entry_index];

      current_packet_info = 
        &hdrrmac4drv.per_carrier_info[ch].interlace_history[interlace];

      /* Record rri, and lookup the t2p (data gain) in 1/4 dB units. */
      pkt_info->carrier_info[chan_entry_index].ps_index = 
        current_packet_info->tx_rri;

       pkt_info->carrier_info[chan_entry_index].channel_record.band = 
        hdrrmac4drv.per_carrier_info[ch].channel_record.band;

      pkt_info->carrier_info[chan_entry_index].channel_record.chan_num = 
        hdrrmac4drv.per_carrier_info[ch].channel_record.chan_num;


      /* If tx_rri is 0 due to RLSI or DRC supervision, tm and t2p values 
         still reflect what would have been transmitted.  Force them to null
         packet values if this is the case. */
      if (pkt_info->carrier_info[chan_entry_index].ps_index != 0)
      { 
        pkt_info->carrier_info[chan_entry_index].packet_is_lolat = 
          (boolean)current_packet_info->tm;

        pkt_info->carrier_info[chan_entry_index].t2p = 
          hdrrmac4drv_config->
          t2p_info[current_packet_info->tm].
          tx_t2p_info[current_packet_info->tx_rri].
          data_t2p_db[current_packet_info->subpacket];
      }
      else
      { 
        pkt_info->carrier_info[chan_entry_index].packet_is_lolat = FALSE;
     
        pkt_info->carrier_info[chan_entry_index].t2p = 0;
      }


    } /* for each carrier */

    pkt_info->num_carriers = hdrrmac4drv.num_chans;

  }

  return tx_valid;

}

 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4DRV_ENTER_DRC_SUPERVISION                          EXTERNAL

DESCRIPTION
  Performs drc supervision entry processing for the passed RL carrier - 
  disables RL transmission and reverse power control.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index of carrier entering DRC supervision.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_enter_drc_supervision
(
  /* Physical channel index */
  uint8 phy_chan_idx
)
{
  uint8 ch;
    /* Loop counter */

  hdrfw_msg_u fw_cmd_enable_carrier, fw_cmd_misc;
    /* Firmware commands */

  errno_enum_type estatus;
    /* Error status */

  uint8  num_chans_drc_timer_expired = 0;
    /* Number of channels for which the DRC supervision timer has expired*/

  uint8  num_enabled_chans = 0;
    /* Number of channels that are not in inactive state i.e. are in setup
       or open states. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Error catching */
  if (hdrrmac4drv.per_carrier_info[phy_chan_idx].drc_supervision_expired)
  {
    HDR_MSG_DRIVERS_1( MSG_LEGACY_ERROR,
      "Enter DRC supervision: Carrier %d already in DRC supervision!",
      phy_chan_idx );

    return;
  }

  /* Initialize firmware messages */
  memset ( &fw_cmd_enable_carrier, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD ); 

  memset ( &fw_cmd_misc, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_misc.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD ); 

  /* Set DRC supervision expired flag */
  hdrrmac4drv.per_carrier_info[phy_chan_idx].drc_supervision_expired = TRUE;

  fw_cmd_misc.rmac_misc_cfg.
    carrierMask |= (1 << phy_chan_idx);

  /* Disable RPC since we are disabling the channel */
  fw_cmd_misc.rmac_misc_cfg.
    fieldMask.gainAdjOverride_Valid = 1;
  fw_cmd_misc.rmac_misc_cfg.
    C[phy_chan_idx].gainAdjOverride.carrier = phy_chan_idx;
  fw_cmd_misc.rmac_misc_cfg.
    C[phy_chan_idx].gainAdjOverride.useGainAdjVal = 1;

  /* Disable DRC */
  fw_cmd_misc.rmac_misc_cfg.
    fieldMask.enableDRC_Valid = 1;
  fw_cmd_misc.rmac_misc_cfg.
    C[phy_chan_idx].enableDrc = 0;

  /* Disable channel transmission */
  hdrmod_config_carrier(phy_chan_idx, FALSE, 1<<phy_chan_idx);
  fw_cmd_enable_carrier.rmac_enable_carrier.
    carrierMask |= (1 << phy_chan_idx);
  fw_cmd_enable_carrier.rmac_enable_carrier.
    modulatorConfig[phy_chan_idx].enable = 0;

    /* Pause from issuing next RMAC_ENABLE_CARRIER cmd
       to FW immediately */
  hdrtx_pause_fw_cmd_delivery();
    
  /* Send the constructed messages to firmware */
  estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  estatus = msgr_send ( &fw_cmd_misc.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  /* Force SLP carrier switch if the current SLP carrier is under 
     DRC supervision*/
  if (phy_chan_idx == hdrrmac4drv.slp_carrier.current)
  {
    hdrrmac4_force_slp_switch = TRUE;
  }

  HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, 
                     "DRC Supervision: Tx disabled on ch %d", 
                     phy_chan_idx );

  /* Check if all assigned carriers are under DRC supervision and
     if so, turn of the PA. */
  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired)
    {
      num_chans_drc_timer_expired++;
    }

    if (hdrrmac4drv.per_carrier_info[ch].carrier_state != 
      HDRRMAC4DRV_CH_ST_INACTIVE)
    {
      num_enabled_chans++;
    }
  }

  if (num_chans_drc_timer_expired == num_enabled_chans)
  {
    /* DRC supervision timer expired for all the active channels */

    /* Turn off PA */
    hdrrmac4drv_pa_off(TRUE, 0);

    /* Indicate that we've stopped Tx */
    hdrind_give_ind(HDRIND_RTCMAC_DRC_TX_STOPPED, NULL);

    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "DRC Supervision: Tx disabled" );
  }

  return;

} /* hdrrmac4drv_enter_drc_supervision() */


 /* EJECT */
/*===========================================================================
FUNCTION HDRRMAC4DRV_EXIT_DRC_SUPERVISION                           EXTERNAL

DESCRIPTION
  Performs drc supervision exit processing for the passed RL carrier - 
  re-enables RL transmission and reverse power control.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Physical channel index of carrier exiting DRC supervision.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_exit_drc_supervision
(
  /* Physical channel index */
  uint8 phy_chan_idx
)
{
  hdrfw_msg_u fw_cmd_enable_carrier, fw_cmd_misc;
    /* Firmware commands */

  errno_enum_type estatus;
    /* Error status */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Error catching */
  if (!hdrrmac4drv.per_carrier_info[phy_chan_idx].drc_supervision_expired)
  {
    HDR_MSG_DRIVERS_1( MSG_LEGACY_ERROR,
      "Exit DRC supervision: Carrier %d not in DRC supervision!",
      phy_chan_idx );

    return;
  }

  /* Initialize firmware messages */
  memset ( &fw_cmd_enable_carrier, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_enable_carrier.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_ENABLE_CARRIER_CMD ); 

  memset ( &fw_cmd_misc, 0, sizeof(hdrfw_msg_u) );
  msgr_init_hdr ( &fw_cmd_misc.hdr, MSGR_HDR_RMAC, 
    HDR_FW_RMAC_MISC_CFG_CMD ); 

  /* The mobile received enough valid DRC's, so it can resume. */
  hdrrmac4drv.per_carrier_info[phy_chan_idx].drc_supervision_expired
    = FALSE;

  /* Enable channel transmission */
  hdrmod_config_carrier(phy_chan_idx, TRUE, 1<<
    hdrrmac4drv.per_carrier_info[phy_chan_idx].demod_index);

  /* Enable channel in MDSP */
  fw_cmd_enable_carrier.rmac_enable_carrier.
    carrierMask |= (1 << phy_chan_idx);
  fw_cmd_enable_carrier.rmac_enable_carrier.
    modulatorConfig[phy_chan_idx].carrierMask = (1 << 
    hdrrmac4drv.per_carrier_info[phy_chan_idx].demod_index);
  fw_cmd_enable_carrier.rmac_enable_carrier.
    modulatorConfig[phy_chan_idx].enable = 1;

  /* Enable DRC. */
  fw_cmd_misc.rmac_misc_cfg.
    carrierMask |= (1 << phy_chan_idx);
  fw_cmd_misc.rmac_misc_cfg.
    fieldMask.enableDRC_Valid = 1;
  fw_cmd_misc.rmac_misc_cfg.
    C[phy_chan_idx].enableDrc = 1;

  /* Enable RPC since we are enabling channel */
  if (hdrrmac4drv.rpc_disabled == FALSE)
  {
    fw_cmd_misc.rmac_misc_cfg.
      fieldMask.gainAdjOverride_Valid = 1;
    fw_cmd_misc.rmac_misc_cfg.
      C[phy_chan_idx].gainAdjOverride.useGainAdjVal = 0;
    fw_cmd_misc.rmac_misc_cfg.
      C[phy_chan_idx].gainAdjOverride.useGainAdjWrite = 0;
  }

  /* Pause from issuing next RMAC_ENABLE_CARRIER cmd
     to FW immediately */
  hdrtx_pause_fw_cmd_delivery();
  
  /* Send the constructed messages to firmware */
  estatus = msgr_send ( &fw_cmd_enable_carrier.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  estatus = msgr_send ( &fw_cmd_misc.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );

  if ( ( (hdrrmac4drv.pa_status == PA_OFF) && (hdrrmac4drv.at_not_in_rlsi) ) ||
       (hdrrmac4drv.pa_status == PA_ON_PENDING) )
  {
    /* At this point, reasons for PA not being ON could be - 
        - DRC supervision, which we just exited   OR
        - RLSI
     
       Turn on PA if AT - 
        - is not in RLSI   OR
        - is in process of RLSI exit that would have scheduled PA ON had AT 
          not been in DRC supervision */

      /* Set the modulator frame interrupt offset. */
      hdrmod_set_tx_subframe_interrupt_offset( 
        hdrrmac4drv.interrupt_offset );

      /* Enable the transmitter. */
      hdrrmac4drv_pa_on(TRUE, 0);

      /* Give Tx restarted indication */
      hdrind_give_ind(HDRIND_RTCMAC_DRC_TX_RESTARTED, NULL);

  } /* if PA is not ON */

  HDR_MSG_DRIVERS_1( MSG_LEGACY_HIGH, 
                     "DRC Supervision: Tx reenabled on ch %d", 
                     phy_chan_idx );

  return;

} /* hdrrmac4drv_exit_drc_supervision() */


/*===========================================================================
FUNCTION HDRRMAC4DRV_START_FLOW_CONTROL                             EXTERNAL

DESCRIPTION
  This function stops RMAC from making packets on non-SLP carriers by 
  reporting FALSE for non-SLP carriers when 
  hdrrmac4drv_get_carrier_availability() is called.  
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
void hdrrmac4drv_start_flow_control( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrrmac4drv.flow_control_on = TRUE;

} /* hdrrmac4drv_start_flow_control */


/*===========================================================================
FUNCTION HDRRMAC4DRV_STOP_FLOW_CONTROL                              EXTERNAL

DESCRIPTION
  This function brings RMAC out of flow control mode and 
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
void hdrrmac4drv_stop_flow_control( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrrmac4drv.flow_control_on = FALSE;

} /* hdrrmac4drv_stop_flow_control */

#if defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
/*===========================================================================
FUNCTION HDRRMAC4DRV_RL_IS_ACTIVE                                    EXTERN

DESCRIPTION
  Returns TRUE if there is RL transmission.
    
DEPENDENCIES
  None.
  
PARAMETERS
  None.

RETURN VALUE
  TRUE/FALSE based on whether thre is RL transmission

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac4drv_rl_is_active ( void )
{

  /* Whether any packets are active on any interlace. */
  boolean packets_active = FALSE;
  /* looping index */
  uint8   ch;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check if packets are active on any interlace. */    
  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv_packets_are_active(ch))
    {
      packets_active = TRUE;

      break;
    }
  }

  HDR_MSG_DRIVERS_2( MSG_LEGACY_LOW,
                  "FLEXCONN/DTX: rl_packet_scheduled=%d,packets_active=%d",
                   hdrrmac4drv.rl_packet_scheduled, packets_active );

  return ( packets_active || hdrrmac4drv.rl_packet_scheduled );
} /* hdrrmac4drv_rl_is_active */

/*===========================================================================
FUNCTION HDRRMAC4DRV_NO_CARRIER_IN_DRC_SUPERVISION

DESCRIPTION
  Returns TRUE if all active carriers are not in DRC supervision.
    
DEPENDENCIES
  None.
  
PARAMETERS
  None.

RETURN VALUE
  TRUE/FALSE based on whether all active carriers are not in DRC supervion.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrrmac4drv_no_carrier_in_drc_supervision ( void )
{

  uint8 ch;
    /* Loop counter */

  boolean no_carrier_in_drc_supervision = TRUE;
    /* Whether there is no carrier in DRC supervision */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if ( hdrrmac4drv.per_carrier_info[ch].drc_supervision_expired && 
         ( hdrrmac4drv.per_carrier_info[ch].carrier_state != 
               HDRRMAC4DRV_CH_ST_INACTIVE ) )
    {
      no_carrier_in_drc_supervision = FALSE;
        /* one active carrier is in DRC supervision, set the flag of no
           carrier in DRC supervision to be FALSE
        */
      break;
    }
  }

  return no_carrier_in_drc_supervision;
}
#endif /* defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/

/*===========================================================================
FUNCTION HDRRMAC4DRV_UPDATE_FUNNEL_BIAS                             EXTERNAL

DESCRIPTION
  This function updates the following information supplied by RF -
  - Whether enahnced PA backoff is enabled or not
  - Per-carrier funnel bias

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_update_funnel_bias( void )
{
  /* Funneling bias array */
  rfm_hdr_per_carrier_funneling_bias_type 
    funnel_bias[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Loop index */
  uint8 idx;
  uint8 ch; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Clear funnel bias entries. */  
  memset ( (void *)funnel_bias, 0, 
    sizeof(rfm_hdr_per_carrier_funneling_bias_type) *  
    HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    hdrrmac4drv.per_carrier_info[ch].funnel_bias = 0;
  }


  /* Obtain enhanced PA backoff enable status and funnel bias
     entries from RF.  The bias values are in Q.10 dBm units */
  hdrrmac4drv.enh_pa_backoff_enabled = rfm_hdr_get_funneling_metrics( 
    hdrsrchrf_get_rf_dev(HDRSRCHRF_PRI_CHAIN),
    funnel_bias );

  /* Store per-carrier bias if PA backoff is enabled. */
  if ( hdrrmac4drv.enh_pa_backoff_enabled )
  {
    for (idx=0; idx<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
    {
      ch = funnel_bias[idx].carrier_id;
  
      if ( ch != RFM_HDR_INVALID_CARRIER_ID )
      {
        /* Sanity check */
        ASSERT( ch < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );
  
        /* Store funnel bias after bounds check */
        hdrrmac4drv.per_carrier_info[ch].funnel_bias = 
          MIN( HDRRMAC4DRV_MAX_FUNNEL_BIAS, funnel_bias[idx].funnel_bias );
  
      } /* if carrier_id is valid */
  
    } /* for all funnel_bias entries */

  } /* if PA backoff is enabled */
  
  /* TODO: Remove this message after feature verification. */
  HDR_MSG_DRIVERS_6( MSG_LEGACY_HIGH, 
    "PA backoff: funnel bias raw values (Q.10 dBm): ch%d[%d], ch%d[%d], ch%d[%d]",
    funnel_bias[0].carrier_id,
    funnel_bias[0].funnel_bias,
    funnel_bias[1].carrier_id,
    funnel_bias[1].funnel_bias,
    funnel_bias[2].carrier_id,
    funnel_bias[2].funnel_bias );

  HDR_MSG_DRIVERS_4( MSG_LEGACY_HIGH, 
    "PA backoff status: %d, funnel bias (Q.10 dBm): %d, %d, %d",
    hdrrmac4drv.enh_pa_backoff_enabled,
    hdrrmac4drv.per_carrier_info[0].funnel_bias,
    hdrrmac4drv.per_carrier_info[1].funnel_bias,
    hdrrmac4drv.per_carrier_info[2].funnel_bias );

} /* hdrrmac4drv_update_funnel_bias */


/*===========================================================================
FUNCTION HDRRMAC4DRV_CONFIG_FUNNEL_MODE                             EXTERNAL

DESCRIPTION
  When in FTM mode, this function configures funnel mode in FW.

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
void hdrrmac4drv_config_funnel_mode
(
  boolean  enable,
  uint8    data_active_carrier_idx
)
{
  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;

  /* Loop index */
  uint8       idx;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
#ifdef FEATURE_FACTORY_TESTMODE
  if( ftm_get_mode() == FTM_MODE )
  {
    /* Sanity check */  
    ASSERT( data_active_carrier_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );
   
    /* Initialize firmware subframe configuration message */
    memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );
    msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_SUBFRAME_CFG_CMD );
   
    for (idx=0; idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
    {
      /* Set all entries in ranked list to EMPTY_ENTRY 
         and max allocated per-carrier powers to max values. */
      fw_cmd.rmac_subframe_cfg.txCarriersOrderedByRank[idx] = 
        HDRRMAC4DRV_RANKED_CARRIER_LIST_EMPTY_ENTRY;
  
      fw_cmd.rmac_subframe_cfg.txTotalMaxAllocated[idx] = 0x7FFF;
    }
  
    /* Set the top entry in the ranked carrier list to the 
       specified carrier. */
    fw_cmd.rmac_subframe_cfg.txCarriersOrderedByRank[0] = 
      data_active_carrier_idx;
  
    /* Populate funnel mode and data active carrier entries */
    fw_cmd.rmac_subframe_cfg.txFunnelModeCfg.funnelStateValid = 1;
    fw_cmd.rmac_subframe_cfg.txFunnelModeCfg.funnelState = enable;
    fw_cmd.rmac_subframe_cfg.txFunnelModeCfg.dataActiveCarrierIndex = 
      data_active_carrier_idx;
  
    /* Send command to firmware */
    estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
    ASSERT ( estatus == E_SUCCESS );

    HDR_MSG_DRIVERS_2( MSG_LEGACY_HIGH, 
      "Config funel mode: enable = %d, data active carrier = %d",
      enable,
      data_active_carrier_idx );

  }
  else
#endif /* FEATURE_FACTORY_TESTMODE */
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_ERROR, 
      "Config funnel mode error: phone not in FTM mode!" );
  }

} /* hdrrmac4drv_config_funnel_mode */


/*===========================================================================
FUNCTION HDRRMAC4DRV_EVAL_MCDO_CONNECTION_SETUP

DESCRIPTION
  In case of a new MCDO connection setup where RL carriers assigned in TCA
  other than the best carrier have not been turned on yet, this function
  decides among the following alternatives:
  1. to turn on these carriers if they can be supported by available power
  2. to keep them turned off otherwise
  In case of alternative#2, an RF API is called to switch to single carrier
  PA backoff.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_eval_mcdo_connection_setup( void )
{
  /* Channel index */
  uint8       ch;

  /* Transmit pilot power */
  int16       txp_dsp[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0};

  /* Transmit pilot power in Q.10 dBm units */
  int32       txp_dbm[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0};

  /* Transmit pilot power in Q.22 mW */
  uint32      txp_mw[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0};

  /* Per-carrier transmit power */
  uint32      req_carrier_pwr[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS] = {0};

  /* Total required transmit power if all assigned carriers are
     enabled */
  uint32      req_pwr = 0;

  /* Previously determined best carrier index */
  uint8       ch_best;

  /* Max available transmit power in DSP units */
  int16       p_max_dsp;

  /* Max available transmit power in Q.10 dBm units */
  int32       p_max_dbm;

  /* Max available transmit power in Q.22 mW */
  uint32      p_max_mw;

  /* Channel mask constructed from carriers to be enabled */
  uint16      phy_chan_mask = 0;

  /* Band-chan array to be passed to RF for switching to SCDO backoff */
  sys_channel_type ch_list[HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS];

  /* Temporary variables for calculating pilot power estimate for carriers
     that are not yet enabled */
  int16       openloop, gainadj, closedloop;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  ASSERT (hdrrmac4drv.ch_best < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS);

  ch_best = hdrrmac4drv.ch_best;

  /* Get best carrier pilot power in FW units */
  txp_dsp[ch_best] = hdrmdsp_shared_mem_interface->read.rf_rd.txC[
    hdrrmac4drv.ch_best].pilotPower;

  /* Get best carrier transmit pilot power in Q21.10 dBm */
  txp_dbm[ch_best] = (hdrrmac4drv.k << 10) + 65331 - ( ( ( txp_dsp[ch_best] * 
    819 ) + (((int16)1) << 8) ) >> 9 );

  /* Get best carrier transmit pilot power in Q10.22 mW */
  txp_mw[ch_best] = hdrrmacmath_dbm_2_mw( txp_dbm[ch_best] );

  /* Required power in Q.22 mW for already enabled carrier
     = PilotPower*(1+OvG+T2Pthreshold) */
  req_carrier_pwr[ch_best] = (uint32) MIN( (uint64) 0xFFFFFFFF, ( ( ( 
    (uint64) txp_mw[ch_best] ) * ( hdrrmac4drv.per_carrier_info[ch_best].
    pilot_plus_overhead_gains + hdrrmacmath_db_2_lin( (
    hdrrmac4drv_config->t2p_info[HDRRMACCMN_TX_MODE_LO_LAT].
    tx_t2p_info[HDRRMACCMN_256_BITS].max_db ) << 18 ) ) 
    + (((uint32)1) << 9) ) >> 10 ) );

  /* Add per-carrier required power to total required power */
  req_pwr = HDRRMAC4DRV_ADD_UINT_SAT( req_pwr, 
    req_carrier_pwr[ch_best], 0xFFFFFFFF );

  for (ch=0; ch<HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; ch++)
  {
    if (hdrrmac4drv.per_carrier_info[ch].off_till_mcdo_eval)
    {
      ASSERT( ch != ch_best );

      /* Add carrier index to enable mask. This mask is used to enable
         carriers if required power to enable carriers is less than
         available power. */
      phy_chan_mask |= (1 << ch);

      openloop = hdrmdsp_shared_mem_interface->read.rf_rd.txC[ch].OpenLoop;
      gainadj = hdrmdsp_shared_mem_interface->read.rf_rd.txC[ch_best].GainAdjVal;
      closedloop =  -( gainadj * HDRDSP_AGC_COUNTS_PER_DB ) >> 9;

      /* Get pilot power estimate in FW units from open loop of this carrier
         and closed loop of already enabled carrier. */
      txp_dsp[ch] = openloop + closedloop;

      HDR_MSG_DEBUG_7( HDR_MSG_DBG_PS_SELECTION, 
        "Ch:%d, OpenLoop: %d, GainAdj[%d]: %d, ClosedLoop[%d]:%d, TxP: %d",
        ch,
        openloop,
        ch_best,
        gainadj,
        ch_best,
        closedloop,
        txp_dsp[ch] );

      /* Get transmit pilot power in Q21.10 dBm */
      txp_dbm[ch] = (hdrrmac4drv.k << 10) + 65331 - ( ( ( txp_dsp[ch] * 
        819 ) + (((int16)1) << 8) ) >> 9 );

      /* Get transmit pilot power in Q10.22 mW */
      txp_mw[ch] = hdrrmacmath_dbm_2_mw( txp_dbm[ch] );

      /* Estimate of required power in Q.22 mW for this carrier if it were
         to be enabled
         = PilotPowerEstimate*(1+OvG) */
      req_carrier_pwr[ch] = (uint32) MIN( (uint64) 0xFFFFFFFF, ( ( ( 
        (uint64) txp_mw[ch] ) * ( hdrrmac4drv.per_carrier_info[ch].
        pilot_plus_overhead_gains ) 
        + (((uint32)1) << 9) ) >> 10 ) );

      /* Add per-carrier required power to total required power */
      req_pwr = HDRRMAC4DRV_ADD_UINT_SAT( req_pwr, 
        req_carrier_pwr[ch], 0xFFFFFFFF );

      /* Reset off-till-MCDO-eval flag */
      hdrrmac4drv.per_carrier_info[ch].off_till_mcdo_eval = FALSE;
    }

    HDR_MSG_DEBUG_6( HDR_MSG_DBG_PS_SELECTION, 
      "Ch:%d, TxPdsp=%d, dbm=%d, mw=%u, pwr=%u, totPwr=%u",
      ch,
      txp_dsp[ch], 
      txp_dbm[ch],
      txp_mw[ch],
      req_carrier_pwr[ch],
      req_pwr );
  }

  /* Get maximum total availabe transmit power */
  p_max_dsp = hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.
    txTotalOverallMaxLimit;

  /* Max. Avail. Pow. from DSP
        P = ((K + 63.8) - X dBm)*65536/DynamicRange
     => X = K + 63.8 - P*DynamicRange/65536  in dBm
          = K*1024 + 63.8*1024 - P*(102.4/65536)*1024 in Q21.10 dBm
          = K<<10 + 65331 - (P*819 >> 9)  in Q21.10 dBm  

     (102.4/65536)*1024 ~ 819 >> 9
     The above converts the floating point division to a fixed point 
     multiplication followed by a shift operation. */
  if (p_max_dsp >= 0)
  {
    p_max_dbm = (hdrrmac4drv.k << 10) + 65331 - 
      ( ( ( p_max_dsp * 
      819 ) + (((int16)1) << 8) ) >> 9 );
  }
  else
  {
    p_max_dbm = (hdrrmac4drv.k << 10) + 65331 - 
      ( ( ( p_max_dsp * 
      819 ) - (((int16)1) << 8) ) >> 9 );
  }

  /* Convert maximum available transmit power to Q10.22 mW */
  p_max_mw = hdrrmacmath_dbm_2_mw( p_max_dbm );

  /* Check if we can sustain MCDO call. */
  if (req_pwr < p_max_mw)
  {
    /* MCDO call can be sustained
       => enable those TCA carriers that have not been enabled yet. */
    hdrrmac4drv_start_channels(phy_chan_mask, FALSE);

    HDR_MSG_DRIVERS_2( MSG_LEGACY_HIGH, 
      "MCDO eval: enabled rest of the carriers in TCA. PwrReq=%u, PwrAvail=%u",
      req_pwr,
      p_max_mw );
  }
  else
  {
    /* Switch to single carrier PA backoff */

    /* Populate best carrier as the only carrier in channel list to be
       passed to RF */
    ch_list[0] = hdrrmac4drv.per_carrier_info[ch_best].channel_record;
    
    /* Override Rev Link to 1 carrier */    
    hdrrmac_override_rev_link_carrier_set( TRUE,
                                            ch_list,
                                            1 );

    HDR_MSG_DRIVERS_3( MSG_LEGACY_HIGH, 
      "MCDO eval: dropping all but ch %d carriers. PwrReq=%u, PwrAvail=%u",
      ch_best,
      req_pwr,
      p_max_mw );
  }

} /* hdrrmac4drv_eval_mcdo_connection_setup */

#ifdef FEATURE_HDR_FLEX_CONN
/*==========================================================================

FUNCTION  HDRRMAC4DRV_SET_FLEXCONN_MODE

DESCRIPTION
  This function sends a message to FW to enable or disable FLEXCONN mode
    
DEPENDENCIES
  None

INPUTS
  flexconn_mode:  flexconn mode, TRUE means enabling flexconn
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrrmac4drv_set_flexconn_mode( boolean flexconn_mode )
{
  /* Firmware command */
  hdrfw_msg_u fw_cmd;

  /* Error status */
  errno_enum_type estatus;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  flexconn_mode &= ( !hdrrmaccmn_is_dtx_tx_limited( ) );

  if ( hdrrmac4drv_common->flexconn_mode == flexconn_mode)
  {
    return;
  }

  if ( hdrrmac4drv_common->flexconn_mode == TRUE )
  {
    hdrrmac4drv_common->rl_sf_cnt = 0;
      /* Reset rl subfram count at the transition of FC enabled->FC disabled.
         This count will be used to record the number of sub frame during FC disabled period
      */
  }
  hdrrmac4drv_common->flexconn_mode = flexconn_mode;
    /* update flexconn_mode stored in hdrrmac structure */

  /* Initialize DTXMode configuration message */
  memset ( &fw_cmd, 0, sizeof(hdrfw_msg_u) );

  msgr_init_hdr ( &fw_cmd.hdr, MSGR_HDR_RMAC, HDR_FW_RMAC_DTXMODE_CFG_CMD );

  if ( flexconn_mode == TRUE )
  {
   fw_cmd.rmac_dtxmode_cfg.flexConnEnable = 0x1;
  }
  else
  {
    fw_cmd.rmac_dtxmode_cfg.flexConnEnable = 0x0;
  }

  /* Keep dtx_mode same as before */
  fw_cmd.rmac_dtxmode_cfg.dtxConfig = hdrrmac4drv_common->dtx_config;

  if ( hdrrmac4drv_common->device_only_dtx_mode == TRUE )
  {
    fw_cmd.rmac_dtxmode_cfg.drcGainBoostCfg = 
      ( uint8 )hdrrmac4drv_common->dtx_drc_boost;
  }
  else
  {
    fw_cmd.rmac_dtxmode_cfg.drcGainBoostCfg = 0;
  } 

#ifdef FEATURE_HDR_TRAFFIC_DTX
  /* dtxPowerThreshold in fw units:
     txFWTtotal = ( K + 63.8 - TxTotal) * 640
  */
  fw_cmd.rmac_dtxmode_cfg.dtxTxPwrThreshold = 
    ( ( hdrrmac4drv_common->turnaround_db256 + 16333 
        - ( HDRRMAC_TDTX_TX_PWR_THRESH_DB * 256 ) ) / 256 ) * 640; 
#endif /* FEATURE_HDR_TRAFFIC_DTX */

  HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                  "FLEXCONN: send FLEXCONN_CFG_CMD flexconn_Config %d hstr %d",
                  fw_cmd.rmac_dtxmode_cfg.flexConnEnable,
                  HDRHSTR_GET_COUNT()
                  ); 

  /* Send DTXMode config command to firmware */
  estatus = msgr_send ( &fw_cmd.hdr, sizeof(hdrfw_msg_u) );
  ASSERT ( estatus == E_SUCCESS );
 
} /* hdrrmac4drv_set_flexconn_mode */
#endif /* FEATURE_HDR_FLEX_CONN */


/*===========================================================================
FUNCTION HDRRMAC4DRV_SET_CHAN_REMOVED_BY_TCA

DESCRIPTION
  This function conveys the physical channel id of the carrier that is
  removed by TCA.  Such carriers should not be enabled when RMAC is
  started after retune for TCA processing.
  
  This function is called when a carrier enters DELETE state. A carrier in 
  this state can go through one of the following paths:
  1. DELETE -> ADD_DELETE -> SETUP state. This happens when the carrier 
     replaced with another carrier in TCA
  2. DELETE -> INACTIVE state. This happens when the carrier is not replaced
     with any other carrier and is removed by RMAC.
  The flag set by this function, removed_by_tca, is reset when a carrier 
  enters SETUP or INACTIVE states.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Channel index of carrier removed by TCA

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_set_chan_removed_by_tca
(
  /* Physical channel index */
  uint8 phy_chan_idx
)
{
  /* Sanity check*/
  ASSERT ( phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );

  hdrrmac4drv.per_carrier_info[phy_chan_idx].removed_by_tca = TRUE;

} /* hdrrmac4drv_set_chan_removed_by_tca */


#ifdef FEATURE_HDR_MODEM_API_RL_INFO
/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_NOT_DATA_LMTD_TPUT                         EXTERNAL

DESCRIPTION
  Returns served throughput calculated based on filtered value of
  not-data-limited packet bits acknowledged per unit time.  

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Index of channel for which data limited throughput is to
                 be reported
 
RETURN VALUE
  Throughput value in Q24.8 kbps units.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hdrrmac4drv_get_not_data_lmtd_tput
(
  uint8    phy_chan_idx
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check */
  ASSERT( phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );

  return hdrrmac4drv.per_carrier_info[phy_chan_idx].not_data_lmtd_tput;

} /* hdrrmac4drv_get_not_data_lmtd_tput */


/*===========================================================================
FUNCTION HDRRMACDRV_GET_CURRENT_TPUT                      

DESCRIPTION
  Returns served throughput calculated based on filtered value of
  bits acknowledged per unit time.

DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx - Index of channel for which data limited throughput is to
                 be reported

RETURN VALUE
  Throughput value in Q24.8 kbps units.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hdrrmac4drv_get_current_tput
(
  uint8    phy_chan_idx
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check */
  ASSERT( phy_chan_idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS );

  return hdrrmac4drv.per_carrier_info[phy_chan_idx].curr_tput;

} /* hdrrmac4drv_get_current_tput */

#endif /* FEATURE_HDR_MODEM_API_RL_INFO */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
/*===========================================================================
FUNCTION HDRRMAC4DRV_SET_T2P_BOOST_INFO                              EXTERNAL

DESCRIPTION
  Initializes pointer to retrieve T2P boost info.

DEPENDENCIES
  None.

PARAMETERS
  Pointer to T2P boost info.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_set_t2p_boost_info
(
  hdrrmaccmn_tx_t2p_info_type* tx_t2p_info_ptr
)
{
  /* Sanity check */
  ASSERT( tx_t2p_info_ptr != NULL );

  /* Set pointer to boosted TxT2P info */
  hdrrmac4drv.boosted_t2p_info_ptr = tx_t2p_info_ptr;

  HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, "Boost info updated" );

} /* hdrrmac3drv_set_t2p_boost_info */

#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */


#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD
/*=========================================================================
FUNCTION     : HDRRMAC4DRV_CLEAR_CARRIER_HISTORY                  EXTERNAL

DESCRIPTION  : Clear carrier history, typically for new call or HHO. 

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/
void hdrrmac4drv_clear_carrier_history( void )
{

  uint8 idx;

  /* Clear RRI interlace history */
  for (idx=0; idx < HDRRMAC4DRV_MAX_NUM_REV_PHY_CHANS; idx++)
  {
    memset((void*)hdrrmac4drv.per_carrier_info[idx].rri_history, 0, 
      sizeof(uint8) * HDRSCMRMACCMN_MAX_NUM_INTERLACES);
  }

  hdrrmac4drv.rri_history_idx = HDRSCMRMACCMN_MAX_NUM_INTERLACES;
    /* Reset index value */

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "Carrier history cleared." );

} /* hdrrmac4drv_clear_carrier_history */


/*===========================================================================
FUNCTION HDRRMAC4DRV_ENABLE_PERMITTED_PAYLOAD_OPTIMIZATION           EXTERNAL

DESCRIPTION
  Enables/disables permitted payload optimization.

DEPENDENCIES
  None.

PARAMETERS
  TRUE  - Enable permitted payload optimization
  FALSE - Disable permitted payload optimization

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_enable_permitted_payload_optimization
(
  boolean enable
)
{

  hdrrmac4drv.perm_pyld_opt_enabled = enable;

  if( enable )
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
      "Permitted payload optimization enabled" );
  }
  else
  {
    HDR_MSG_DRIVERS( MSG_LEGACY_HIGH, 
      "Permitted payload optimization disabled" );
  }

} /* hdrrmac4drv_enable_permitted_payload_optimization */

/*===========================================================================
FUNCTION HDRRMAC4DRV_GET_FUNNEL_MODE

DESCRIPTION
  Checks if if RMAC4 is in funnel mode

DEPENDENCIES
  None.

PARAMETERS
  None
 
RETURN VALUE
  Whether RMAC4 is in funnel mode

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_get_funnel_mode( void )
{
  return hdrrmac4drv.in_funnel_mode;
}
#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_PERM_PYLD */

/*===========================================================================
FUNCTION HDRRMAC4DRV_IS_CARRIER_IN_SETUP_STATE                       EXTERNAL

DESCRIPTION
  Returns whether the carrier state for the given physical channel index
  is HDRRMAC4DRV_CH_ST_SETUP or not.
  
DEPENDENCIES
  None.

PARAMETERS
  phy_chan_idx  - Physical channel index.
 
RETURN VALUE
  TRUE - Carrier is in HDRRMAC4DRV_CH_ST_SETUP state
  FALSE - Carrier is not in HDRRMAC4DRV_CH_ST_SETUP state

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac4drv_is_carrier_in_setup_state
( 
   uint8 phy_chan_idx
)
{

  return (hdrrmac4drv.per_carrier_info[phy_chan_idx].carrier_state
          == HDRRMAC4DRV_CH_ST_SETUP);

} /* hdrrmac4drv_is_carrier_in_setup_state() */

/*===========================================================================

FUNCTION HDRRMAC4DRV_DISABLE_FRAME_ISR                               EXTERNAL

DESCRIPTION
  This function uninstall/Disable the RMAC frame ISR.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void hdrrmac4drv_disable_frame_isr( void )
{
      /* Uninstall interrupt. */
   hdrmod_disable_frame_isr();
   (void) rex_clr_sigs(  HDRTX_TASK_PTR, HDRTX_RMAC_ISR_SIG );
   hdrrmac4drv.isr_ptr = NULL;
}
#endif /* FEATURE_HDR_REVA_L1 */

