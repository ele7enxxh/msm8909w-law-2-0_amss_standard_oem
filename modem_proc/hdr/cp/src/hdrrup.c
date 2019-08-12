/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                R O U T E  U P D A T E   P R O T O C O L

GENERAL DESCRIPTION
  The default Route Update Protocol provides the procedures and messages
  used by the Access Terminal to keep track of the AT's approximate location
  and to maintain the radio link as the AT moves between the coverage areas of
  different sectors.

EXTERNALIZED FUNCTIONS (Global)
  hdrrup_msg_cb - Callback registered with HMP to deliver messages
  hdrrup_ind_cb - Callback registered for delivery of indications
  hdrrup_activate - Command function to activate RUP
  hdrrup_deactivate - Command function to deactivate RUP
  hdrrup_close - Command function to close a connection.

EXTERNALIZED FUNCTIONS (Regional)
  hdrrup_pwerup_init - RUP initialization that is done once at powerup.
  hdrrup_init - Function called by hdrmc task to initialize RUP.
  hdrrup_process_cmd - Function called by hdrmc task to process a
                       command for RUP
  hdrrup_process_ind - Function called by hdrmc task to process an
                       indication for RUP
  hdrrup_process_msg - Function called by hdrmc task to process a
                       message for RUP
  hdrrup_process_sig - Function called by hdrmc task to process a
                       signal for RUP

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrrup_powerup_init should be called at powerup.
  hdrrup_init should be called everytime HDR mode is entered.

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrrup.c_v   1.73   09 Apr 2003 09:24:42   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrrup.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
05/04/16   vko     Reduce F3 logging
12/14/15   vko     Fixed KW issues 
12/28/14   vko     Debug info for hdr
11/25/14   vko     Ignore fast connect TCA, if access handover is in progress
10/12/14   vko     Reset avoided channel list before retrieveing the list from SD
08/01/14   vko     Ignore fast connect TCA, when tune away already started
07/09/14   cnx     Support NV refresh without reset.
06/12/14   sat     Fixing Klocwork issues
04/07/14   vko     Fixed klocwork issues
03/14/14   vko/dsp FR19338:Reduce call setup time by skipping RF TX shutdown
03/04/14   vko     move hdrrup_process_conn_setup_timeout to global                   
3/10/14    vko     Fixed rev_chan_index out of bound
01/02/14   vko     Ignore TCA when IDLE is in CSETUP but substate not waiting for TCA
12/23/13   cnx     Notify SRCH about Sector ID change.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
12/04/13   sat     Removed unused parameters in F3s.
10/31/13   ukl     Fixed the TCA ignoring logic for avoided channel.
10/31/13   ukl     Keep NSET updated for system avoidance.
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
09/19/13   ukl     Reduce Code Duplication HDR-CP
08/27/13   cnx     Remove unsupported channel in hash before session negotiation.
06/13/13   vko     Fix SAP crash, due to connection close recieved just after RTC Ack during optmized HO
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
04/26/13   ukl     Removed checking num_rev_channels equal to 1 for returning
                   from check_mc_frequency_bdwidth.
03/29/13   vko     Fixed ConnectionReq not being sent after Access handoff
03/13/13   vko     Ignore TCA, when mac index is 0
03/05/13   vko     Ignore RUP request message, when buffers are exhausted 
10/08/12   wsh     Fixed race cond. in RUP
11/02/12   wsh     Disabled tuneaway after active HO for 3 sec.  
11/22/12   vko     In opt HO scenario limit the number of pilots listed in RUP which is sent along with Connection Request
01/13/13   ukl     Added NV control for 1x TuneAway during session negotiation.
01/13/13   rkc     Changed all MED F3's to HIGH.
01/10/13   ukl     Fix the argument mismatch in F3 printing 
10/02/12   wsh     Fixed data stall issue after active HO
09/10/12   cnx     Abort active handoff if TCA channel is in SD's avoidance list.
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
04/17/12   cnx     Support rfm_is_band_chan_supported_v2().
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
10/28/11   pba     Optimized handoff phase 2 support
09/24/11   cnx     Changed subtype checking for Optimized Handoff.
07/27/11   cnx     Merged Optimized Handoff changes.
07/26/11   cnx     Don't resend NULL msg when there is no transmit ATI. 
05/23/11   sju     Initialize AuxDRCCover during TCA processing.
04/15/11   cnx     Removed hdrrup_band_is_supported() if RF_HAS_RFM_BAND_CHAN_V2_API defined. 
10/22/10   cnx     Added check of the first instance of softer handoff field in TCA msg.
10/07/10   cnx     Fixed a buffer overflow issue when processing the TCA msg; 
                   Added sanity check before casting to an enum value.  
07/27/10   pba     Process IRAT pilot measurement done indication only measurement
                   in progress.
06/10/10   pba     IRAT Measurement feature.
11/12/09   sju     Added support for SVDO.
09/18/09   wsh     CMI: support changes from modem DATA
08/21/09   sju     Lint fix.
08/18/09   sju     Supported non-rectangular locked mode. 
06/16/09   sju     Set ReferencePilotChannelIncluded and ChannelIncluded to 1 for
                   MC-RUP sent on RTC.
05/21/09   sju     Control channel carrier should not be not reportable. 
04/07/09   sju     Fixed error in unpacking PILOG GID field in NL message
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI--
03/20/09   sju     Set default pilot gid to HDRSRCH_NOT_INCLUDED_PILOT_GID
03/11/09   sju     Fixed Lint warnings and checked number of pilots in saset.
03/06/09   sju     Fixed error in packing MC RUP message
02/19/09   wsh     Decrease tca_count if TCA is ignored in invalid state
02/18/09   wsh     Updated file to include private header hdrmc_v.h
01/15/09   etv     Fixed dsc initialization issue with Rev-A TCAM processing.
12/17/08   pba     Updated F3 messages.
12/10/08   smd     Replaced assert.h with amssassert.h
12/08/08   kss     Lint fix.
12/04/08   pba     Lint and Compiler warning fixes
09/30/08   wsh     Not checking if channel is within band while hashing
09/18/08   smd     Added fix for off-target build.
09/08/08   sju     Fixed typo, sub-aset comparison and merged back rectangular check
08/15/08   wsh     Fixed crash if TCA contains invalid channel
08/06/08   sju     Checked rectangular sub-active set only in locked mode 
08/06/08   pba     Klockwork fixes round 2.
7/30/08    sju     Added support for sorting rev channels in ascending order
06/17/08   sju     Allowed NumUniqueForwardTrafficMACIndices to be 0
04/07/08   cc      Allowed AuxDRCCoverIncluded to be 1.
05/13/08   sju     Fixed buffer overflow, array index of fwd_chan_index.
05/09/08   sju     Added support to remove avoided channels from NSET
04/18/08   mpa     Cleaned up mode from hdrsrch_nset_pilot_type struct
04/15/08   mpa     Added pilot pgid interface for SP message + cleaned up nset_udpate
01/28/08   sju     Used AT supported attribute value to check channel separation
01/11/08   sju     Fixed revB RUP msg sent on Traffic Channel
01/08/08   sju     Get Locked/Unlocked mode by calling FMAC API.
01/03/08   sju     Supported channel change requirement for conn setup failure 
12/18/07   sju     Removed temporary fix for reference pilot channel in RU
12/14/07   sju     Temporarily removed reference pilot channel in the RU msg. 
                   Added additional checks while validating TCA
12/12/07   sju     Renamed channel record and system type based on sys.h
11/16/07   sju     Set default values for rab_mac_index and rev_mac_index when 
                   default RUP is used
10/31/07   sju     Changed assigned_channel from cc saset to demod0 saset.
                   Added FEATURE_HDR_REVB_NON_RETANGULAR_ASET
10/17/07   sju     Added HDRRUP_SEND_RUP_MSG_CMD.
10/08/07   sju     Ignored not standard compliant MC TCA. Updated based on 
                   code review comments.
10/03/07   wsh     Updated pilot report timer to reflect new pilot search speed
09/14/07   yll     If Route Update Request contains the band that AT doesn't
                   support, don't pass the request to Search.
09/07/07   sju     Changed implementation of revB RU msg
08/23/07   sju     Fixed ReferenceChannel field in RU msg.   
08/17/07   sju     Fixed incorrect msg_seq in TCC 
08/16/07   wsh     Removed compiler and lint critical warnings
08/15/07   sju     Added support for revB RU and NL msg
08/14/07   yll     When preparing the RouteUpdate message, use system_type 
                   from Search's pilot report 
08/09/07   wsh     Fixed an error in previous fix for RuR report timer
08/08/07   mpa     Added hdrrup_get_traffic_mode()
07/31/07   wsh     Fixed RuR report timer such that search will not ignore an
                   RuR while processing the previous one.
07/19/07   sju     Added sending TCC+CC(reason0x2) if invalid RUP1 TCA was received
07/13/07   sju     Added support for revB Multicarrier RUP
06/15/07   rmg     Fixed error in parsing detaT2P field in TCA message
06/08/07   yll     Changed the processing of RouteUpdateRequest message. Save 
                   the pending RUP requests if Search pilot report for the
                   previous RUP request hasn't been received.
05/15/07   hal     Added hdrrup_increment_tca_count() to support multiple TCAs
05/04/07   sju     Featurized connected state HPT mode under feature 
                   FEATURE_HDR_ACCESS_HYBRID_CTRL                   
04/10/07   mt      Added LMAC interface for Route Update Request.
03/06/07   wsh     Removal of RVCT compiler warnings
02/21/07   hal     Moved DeltaT2P initialization routine to guarantee zero values 
                   if all fields are not included
02/20/07   hal     Updated DeltaT2P parsing based on review comments
02/14/07   hal     Added support to unpack the DeltaT2P field in the TCA message
02/01/07   sju     Add support for QOS HHO
01/11/07   sju     Added support to check if CC and TCA are in the same 
                   connection layer packet.
11/20/06   hal     Updated feature for SupportedCDMAChannels to silently close
                   session if band preferences are changed
09/21/06   yll     Fixed an infinite loop caused by processing registration 
                   outcomes
07/18/06   dna     Support for RouteUpdateTriggerCode, to send RUP msg and 
                   also retry following max access probes, if necessary.
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
06/02/06   hal     Fixed computation for RouteUpdateRadius distance based registration
05/31/06   hal     Changed parameter for hdrrup_band_is_supported()
05/23/06   etv     Process Config Rsp mesg instead of another cmd to hdrmc.
05/09/06   yll     Added support for Minimal UATI Mode.
04/26/06   hal     Renamed hdridle_con_is_for_signaling() to 
                   hdridle_con_is_for_session_config()
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
02/23/06   hal     Changed hdrovhd_get_sp_msg() to hdrovhd_get_sp_msg_location(),
                   hdrovhd_get_sp_msg_nset_info()
01/06/06   hal     Moved invalid neighbor pruning from hdrovhd_unpack_sp_msg() to
                   hdrrup_process_ovhd_info_updated_ind()
01/04/06   hal     Fixed rounding in hdrrup_process_ovhd_info_updated_ind()
12/16/05   mpa     Updated hdrlmac_idle_chan_change() interface.
12/13/05   pba     Changes to support multiple personality negotiation
                   Added support for handle TCA + ConnectionClose bundling
12/06/05   hal     Added support for AT-init cfg of SupportedCDMAChannels attrib
10/26/05   hal     Added handling of improper Route Update Request messages
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/12/05   hal     Added support for AT-init config
                   Added support for RouteUpdateRequest messages; no action taken 
                   if ChannelIncluded other than extracting all fields
                   Added hdrrup_band_is_supported for the SupportedCDMAChannels 
                   complex attribute
                   Added support for AN-initiated GAUP of simple attributes
09/09/05   dna     Added support for setting high priority traffic mode
09/06/05   kss     Use addendum version of TCA message.
07/11/05   dna     Report channel list and cached sectors with neighbor list.
07/11/05   pba     Restrict size of route update message based on maximum bytes 
                   instead of maximum pilots 
07/09/05   dna     Add support for Rel. A TCA message fields
           kss     Include support for non-Addendum TCA parsing.
06/22/05   pba     Modified hdrrup_form_rup_msg to return strongest num_pilots
                   requested if there is restriction on number of pilots to be
                   included in current probe.
06/14/05   dna     Remove assert on SD action when reporting successful access.
06/04/05   dna     Report to SD that our access attempt has ended successfully
                   so it can re-set it count of consecutive failed accesses.
05/24/05   pba     Added hdrrup_is_tc_assigned() to determine if RTC is assigned
03/24/05   pba     Lint cleanup (More)
03/09/05   pba     Lint cleanup
03/07/05   pba     Modified RUP timers to use callback functions instead of 
                   signals.
03/07/05   pba     Added synchronization between RUP and PAC to allow 
                   initialization of RLP before AT starts receiving data on FTC
02/25/05   pba     Added new event EVENT_HDR_RUP_DIST_BASED_REG
12/13/04   hal     Changed dsm_offset_new_buffer to use hdrutil_new_buffer.
11/29/04   grl     Added support for connected state stat logging.
11/08/04   pba     Check for invalid band class and num_pilot is not zero in TCA msg, 
                   remove neighbors with invalid band class from NL msg.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/19/04   mpa     Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
                   Ignore messages received when the session is not opened.
05/21/04   dna     Ignore TCA message if we are sleeping.
04/13/04   mpa     Reject messages decoded in connected state that would be
                   processed after going back to idle.
02/27/04   mpa     Give current color code info to searcher on ovhd updates.
01/27/04   vas     Added code to initialize/cleanup RLP when TCAM is received,
                   doing it before the connect is actually opened to avoid 
                   initial packet loss.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
07/02/03   mpa     Initialize TCAM sequence with seqNum of TCAM received in 
                   idle state (accept all messages in this state)
05/13/03   mpa     Reject TCAM with seqNum != 0 in idle state
04/03/03   mpa     Use hdrhmp_send_msg_on_existing_rtc for PilotRpt and TCC msgs
03/07/03   vas     Do not assert, but rather ignore message if message contains
                   invalid fields.
03/11/03   dna     Don't send TrafficChannelComplete message if connection is 
                   already closing before it is fully opened.
03/05/03   dna     Don't send RouteUpdate message while connection is closing.
01/17/03   mpa     Moved logging of successful connection attempt from hdridle
12/02/02   vas     Disable distance based registration if AT doing long sleep.
10/09/02   mpa     Added Set Management Override functionality
09/16/02   dna     Fixed infinite RUP registration problem
08/02/02   vas     Command srch to idle mode before giving it idle chan change
                   cmd while transtion from Conn setup to Idle.
07/26/02   dna     Report "tc_established" when TCComplete msg is sent.
07/23/02   om      Removed call to FTAP for traffic HOs (obsolete).
07/10/02   dna     Some initialization that should only be done at powerup
                   was happening when we re-enter HDR after power save.
06/10/02   aaj     Corrected RUP F3 message to print correct Ec/Io
06/07/02   dna     Make Connection Setup Timeout standard compliant
05/06/02   dna     Whether or not to tune away to 1x during HDR connection
03/15/02   dna     Build RouteUpdate Message in HDRSRCH context when connected.
02/20/02   vas     Moved certain variables from hdrrup_form_rup_msg() to the
                   static hdrrup control block as these were quite large &
                   could cause stack overflow.
02/19/02   aaj     Add MSG_HIGH in route update form message function
01/27/02   vas     Fix so that rmac is deactivated if RUPClose command is
                   received while in connection setup. Fix to ensure that last
                   RUP msg location is updated every time an access msg is
                   successfully transmitted, not only when NULL msg is queued
01/21/02   dna     Fixed unpacking of NeighborList message
12/04/01   vas     Fix to update neighborlist if unpacking NL msg successful.
11/20/01   dna     Added enum HDRHAI_SYSTYPE
11/14/01   om      Added notification of traffic HO for IS-890
10/05/01   vas     Fix in storing WindowOffset in NL msg.
10/04/01   vas     Added call to hdrplog_log_tca_info() to log TCA msg info.
10/01/01   vas     Fix to store Neighborlist PNs received in Neighborlist
                   message correctly
09/11/01   vas     Changed RUP Config Request Message Id macro name
09/07/01   vas     Added hdrrup_get_current_tca_seq_num()
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/18/01   sh      Added protocol logging support
06/06/01   dna     new param for hmp_send_msg(), TCA_MAC_INDEX_SIZE from 5 to 6.
06/01/01   vas     Re-naming for consistency. Code cleanup. Some re-organizing
05/16/01   vas     Fixed problem to not send RUP msg if rup_radius in rcvd SP
                   msg is zero. Reactivate ACMAC if connection setup fails.
03/19/01   vas     Removed using band to enum conversion function.
03/19/01   vas     Use hdrutil func to read/write from NV. Added separate funcs
                   to set default search, set mangement params.
03/10/01   sh      Removed HDRIND_RUP_TCA_RCVD. Added
                   HDRIND_RUP_CONN_SETUP_FAILED
02/16/01   vas     Added Trace for reading from NV or using default on init
                   Updated names of session indications. Added ASSERT to ensure
                   hdrlmac_get_pilots returns a value < max allowed pilots
02/13/01   vas     Commanding rmac/fmac to deactivate when processing
                   rup_deactivate command. Otherwise these were not
                   being called if RUP were in Connection Setup state.
02/13/01   dna     In report_loc_update, check for SP msg rxed was wrong.
                   Fixed handling of connection setup failure after TCA to
                   a different channel.
02/06/01   vas     Deleted dsm_free_packet if a msg tx callback is for
                   unrecognized msg
01/16/01   dna     Changes to compile with new hdrsrch_pilot_rpt_type
06/27/00   vas     Created module.

=============================================================================*/

/* <EJECT> */

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"
#include "hdrdebug.h"
#include "err.h"
#include "rex.h"
#include "memory.h"
#include "sys.h"
#include "fs_lib.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "distreg.h"

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif

#include "hdrdsm.h"
#include "hdrhai.h"
#include "hdrmci.h"
#include "hdrrup.h"
#include "hdrrupi.h"
#include "hdrrups.h"
#include "hdrrupc.h"
#include "hdramac.h"
#include "hdrrmac.h"
#ifdef FEATURE_HDR_REVB
#include "hdrrmac4.h"
#endif /* FEATURE_HDR_REVB */
#include "hdrfmac.h"
#include "hdrlmac.h"
#include "hdrsrch.h"
#include "hdrbit.h"
#include "hdrhmp.h"
#include "hdrslp.h"
#include "hdrutil.h"
#include "amssassert.h"
#include "nv.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrhmp.h"
#include "hdrtrace.h"
#include "hdridle.h"
#include "hdrsmp.h"
#include "hdramps.h"

#include "hdrscm.h"
#include "hdrscmdb.h"
#include "hdrscmfmac.h"

#ifdef FEATURE_IRAT_PILOT_MEAS
#include "cmxll.h"
#include "cmll.h"
#include "cm.h"
#endif /* FEATURE_IRAT_PILOT_MEAS */

/* DA peek hdridle state */
#include "hdridles.h" 
#include "hdrpac_api.h"

#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif
#include "hdrplog.h"
#include "hdrovhd.h"
#include "hdrcsna.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#include "hdrsrchset.h"
#endif /* FEATURE_LTE_TO_HDR_OH */


#include "hdralmps.h"
#include "hdrovhds.h"

/* <EJECT> */

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#define HDRRUP_RUP_MSG_ID  0x00
#define HDRRUP_TCA_MSG_ID  0x01
#define HDRRUP_TCC_MSG_ID  0x02
#define HDRRUP_RR_MSG_ID   0x03
#define HDRRUP_NL_MSG_ID   0x04

/*-Defined in HDRRUPC.H-
  Attrib Override msg ID      0x05
  Attrib Override rsp msg ID  0x06
 */

#define HDRRUP_RUP_REQ_MSG_ID   0x07

#define HDRRUP_RUP_MSG_PRIORITY  20
#define HDRRUP_TCA_MSG_PRIORITY  20
#define HDRRUP_TCC_MSG_PRIORITY  40
#define HDRRUP_RR_MSG_PRIORITY   40
#define HDRRUP_NULL_MSG_PRIORITY  255

/* This is actually T_RTCMPATSetup (See section 8.5.8) */
#define HDRRUP_CONN_SETUP_TIMER_MSECS 1500 

#define HDRRUP_PAC_TIMEOUT_TIMER_MSECS 15

/* Maximum number of undelivered RUM messages that are allowed to be sent
 * RTC that can be outstanding at one time.
 */
#define HDRRUP_RTC_RUP_MSG_BUFS_NUM 10
#define HDRRUP_RUP_MSG_DUMMY_TX_PWR 0xFF

#define HDRRUP_DISABLE_TUNEAWAY_TIME_MS 3000
/* <EJECT> */

/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

typedef enum
{
  HDRRUP_STATE_TIMER,        /* Route Update state timer */
  HDRRUP_CONN_SETUP_TIMER,   /* RUP Connection Setup timer*/
  HDRRUP_PILOT_RPT_TIMER,    /* Sanity timer in case Searcher doesn't send 
                                pilot report in response of RUP's Route 
                                Update Request */
  HDRRUP_DISABLE_TUNEAWAY_TIMER
                             /* Timer used to disable 1x tuneaway after 
                                active HO */
} hdrrup_timer_enum_type;

/*--------------------------------------------------------------------------
                      ROUTE UPDATE MESSAGE STRUCTURE

--------------------------------------------------------------------------*/

typedef struct
{
  hdrsrch_pilot_phase_type pn_phase;      /* Pilot PN phase */
  boolean chan_incl;                      /* Channel Included */
  sys_hdr_channel_record_s_type chan_rec;       /* Channel */
  hdrsrch_pilot_strength_type strength;   /* Pilot Strength */
  uint8 keep;                             /* Keep */
} hdrrup_rup_msg_pilot_type;

typedef struct
{
  boolean tx_pilot_pwr_incl;              /* ATTotalPilotTransmissionIncluded*/
  uint8 tx_pilot_pwr;                      /* ATTotalPilotTransmission*/
  boolean ref_pilot_chan_incl;            /* ReferencePilotChannel Included */
  sys_hdr_channel_record_s_type ref_chan_rec; /* Channel */
} hdrrup_rup_msg_mc_field_type;

typedef struct
{
  uint8  msg_id;                          /* Message ID */
  uint8  msg_seq_num;                     /* Message Sequence Number */
  sys_pilot_pn_type ref_pilot_pn;         /* Reference Pilot PN */
  hdrsrch_pilot_strength_type ref_pilot_strength; /* Ref. Pilot Strength */
  boolean ref_keep;                       /* Reference Keep */
  uint8 num_pilots;                       /* Number of Pilots */
  hdrrup_rup_msg_pilot_type pilot[HDRRUP_RUP_NUM_PILOTS_MAX];
                                          /* List of Pilots */
#ifdef FEATURE_LTE_TO_HDR_OH
  int16 ref_pilot_arrival;
  sys_hdr_channel_record_s_type ref_chan_rec;
#endif /* FEATURE_LTE_TO_HDR_OH */
  hdrrup_rup_msg_mc_field_type mc_info;
} hdrrup_rup_msg_type;

typedef struct
{
  /* RUP msg Sequence Number*/
  uint8 rup_seq_num;     

  /* Number of Pilots in RUP msg*/
  uint8 num_pilots;

  /* Pilot information*/
  hdrrup_rup_msg_pilot_type pilot[HDRRUP_RUP_NUM_PILOTS_MAX];

  /* Reference Pilot Channel Record*/
  sys_hdr_channel_record_s_type chan_rec;

  /* If this buffer is available to be used */
  boolean available;

  /* Pointer to the packed RUP msg*/
  dsm_item_type * msg_ptr;
} hdrrup_mc_info_buffer_type;

/* Address of the TCB for the task in which this protocol runs */
#define HDRRUP_TASK_PTR hdrmc_tcb_ptr

/* Maximum number of neighbors in the Neighborlist as specified by HAI  */
#define HDRRUP_MAX_NEIGHBORS 32

#if ( HDRRUP_MAX_NEIGHBORS < HDRSRCH_NSET_MAX_SIZE )
#error Incompatible neighborlist sizes. HDRSRCH_NSET_MAX_SIZE has to be greater
#endif

/* default search time if sector_count != 0 */
#define HDRRUP_PILOT_RPT_DEFAULT_TIMER_S  3UL

/* total time to search all 512 possible AN based on
   current 16 AN/1.5sec search speed, 512/16*1.5=48s */
#define HDRRUP_PILOT_RPT_TOTAL_SRCH_TIME_S  48

/* The following structure contains all the global variables used by RUP */
LOCAL struct
{

  /* Keeps track of msg seq number of last route update message */
  uint8  rup_msg_seq_num;

  /* Keeps track of longitude. latitude & route update radius of last Access
     Network to which route update msg is sent */
  struct
  {
    boolean reported;
    int32  latitude;
    int32  longitude;
    uint16 rup_radius;
  } location;

  /* Keeps track of last received Traffic Channel Assignment msg seq number */
  uint8  last_tca_msg_seq;

  /* Keeps track of the last received frame offset value in the tca msg */
  uint8  last_frame_offset;

  /* When first assigned a traffic channel, remember what channel the
     assignment is received on and what channel we are assigned to. The
     values stored in this structure are only valid in connection setup and
     connected states. */
  struct
  {
    sys_link_type idle_link;
    sys_channel_type assigned_channel;
  } initial_tca;

  /* Connection Setup Timer */
  rex_timer_type connection_setup_timer;

  /* RUP state timer*/
  rex_timer_type state_timer;

  /* These 2 variables were part of hdrrup_form_rup_msg. They have been moved
     as global because they are huge variables and may cause stack overflow if
     they were local to the function. */
  hdrrup_rup_msg_type rup_msg;    /* Route Update Message structure */
  hdrsrch_pilot_rpt_type pilot_rpt[HDRRUP_RUP_NUM_PILOTS_MAX];
                                  /* Pilot Reports */
  hdrsrch_traffic_tune_away_enum_type  tc_tune_away; 
  /* Each time we open a connection we determine if we wish to allow SRCH
     to tune away to 1x during the connection */

  #if !defined(FEATURE_HDR_ACCESS_HYBRID_CTRL) || defined(FEATURE_LTE_TO_HDR_OH)
  /* The traffic_mode is a changed through a socket IOCTL and its state 
     persists even when RUP is deactivated. */
  hdrsrch_tc_mode_enum_type traffic_mode;
  #endif

  /* Unpacked Traffic Channel Assignment Msg waiting for handshake to 
   * continue with the connection setup */
  hdrrup_tca_msg_type tca_msg;    

  uint8 cc_saset_index;   /* sub_aset which carries ControlChannel*/

  /* TCA message received while connection was closing, so it must be 
   * held off until we finish closing the connection.
   */
  dsm_item_type * pending_tca_msg_ptr; 

  /* The current number of TCA messages received. If we receive more than
   * one TCA message, we will only use the final TCA message received. This
   * count is incremented by HMP during the message routing process, and
   * decremented while RUP processes the routed TCA messages.
   */
  uint8 tca_count;

  /* Route Update Request message and related variables */
  struct
  {
    /* Route Update Request parameters to send to Search */
    hdrsrch_rup_req_params_type req;

    /* If we are waiting for Search's pilot report for a previous
     * RUP request message, save the current RUP request here */
    dsm_item_type * pending_req;

    /* Timer to wait for Search's pilot report for RUP request */
    rex_timer_type  pilot_rpt_timer;

  } rup_req;

  /* To store mc rup information which will be used to update MC RUP msg fields
   * if SLP carrier channel is different than reference ipilot channel*/
  hdrrup_mc_info_buffer_type mc_rup_info[HDRRUP_RTC_RUP_MSG_BUFS_NUM];

#ifdef FEATURE_IRAT_PILOT_MEAS
  /* Flag indicating whether irat meas is in progress or not */
  boolean irat_meas_in_progress;
#endif /* FEATURE_IRAT_PILOT_MEAS */

  /* Flag indicating whether 1X tuneaway during session negotiation is enabled */
  boolean enable_1x_ta_in_ses_cfg;
  
#ifdef FEATURE_LTE_TO_HDR_OH
    rex_timer_type  disable_tuneaway_timer;
#endif /* FEATURE_LTE_TO_HDR_OH */

  rex_crit_sect_type crit_sect;

  boolean cp_skip_disable_tx;
} hdrrup;

/*---------------------------------------------------------------------------
                   MESSAGE PACKING AND UNPACKING MACROS

  Following macros are used for unpacking received message and packing
  message to be sent
---------------------------------------------------------------------------*/

/* Mesage ID */
#define HDRRUP_MSG_ID( ptr, val_ptr )  \
           HDRBIT_UNPACK8( ptr, val_ptr, 0,  8 )

/*---------------------------------------------------------------------------
          TRAFFIC CHANNEL ASSIGNMENT MESSAGE FIELD EXTRACTION MACROS

---------------------------------------------------------------------------*/
#define HDRRUP_TCA_MSG_ID_SIZE           8
#define HDRRUP_TCA_MSG_SEQ_SIZE          8
#define HDRRUP_TCA_CHANNEL_INCL_SIZE     1

#define HDRRUP_TCA_BAND_CLASS_SIZE       5
#define HDRRUP_TCA_CHANNEL_NUMBER_SIZE   11
#define HDRRUP_TCA_SYSTEM_TYPE_SIZE   8
#define HDRRUP_TCA_FRAME_OFFSET_SIZE     4
#define HDRRUP_TCA_DRC_LENGTH_SIZE       2
#define HDRRUP_TCA_DRC_CHANNEL_GAIN_SIZE 6
#define HDRRUP_TCA_ACK_CHANNEL_GAIN_SIZE 6
#define HDRRUP_TCA_NUM_PILOTS_SIZE       4
#define HDRRUP_TCA_PILOT_PN_SIZE         9
#define HDRRUP_TCA_SOFTER_HO_SIZE        1
#define HDRRUP_TCA_MAC_INDEX_SIZE        6
#define HDRRUP_TCA_DRC_COVER_SIZE        3
#define HDRRUP_TCA_RAB_LENGTH_SIZE       2
#define HDRRUP_TCA_RAB_OFFSET_SIZE       3

#define HDRRUP_TCA_RA_CHANNEL_GAIN_INCLUDED_SIZE 1
#define HDRRUP_TCA_RA_CHANNEL_GAIN_SIZE          2
#define HDRRUP_TCA_MAC_INDEX_MSBS_INCLUDED_SIZE  1
#define HDRRUP_TCA_MAC_INDEX_MSB_SIZE            1
#define HDRRUP_TCA_DSC_CHANNEL_GAIN_SIZE         5
#define HDRRUP_TCA_DSC_SIZE                      3
#define HDRRUP_TCA_DELTA_T2P_INCLUDED_SIZE       1
#define HDRRUP_TCA_DELTA_T2P_SIZE                6

/*rev B*/
#define HDRRUP_TCA_NUM_SECTORS_SIZE               5
#define HDRRUP_TCA_NUM_SUB_ASETS_SIZE             4
#define HDRRUP_TCA_SCHEDULER_TAG_INCL_SIZE        1
#define HDRRUP_TCA_FEEDBACK_MUX_ENABLED_SIZE      1
#define HDRRUP_TCA_NUM_FWD_CH_THIS_SUB_ASET_SIZE  4
#define HDRRUP_TCA_FEEDBACK_ENABLED_SIZE          1
#define HDRRUP_TCA_FEEDBACK_MUX_INDEX_SIZE        9
#define HDRRUP_TCA_FEEDBACK_REV_CH_INDEX_SIZE     4
#define HDRRUP_TCA_SUB_ASET_CARRIES_CC_SIZE       1
#define HDRRUP_TCA_SUB_ASET_NOT_REPORTABLE_SIZE   1
#define HDRRUP_TCA_DSC_SUB_ASET_ENABLED_SIZE      1
#define HDRRUP_TCA_NEXT3_SAME_SIZE                1
#define HDRRUP_TCA_NUM_REV_CHANNELS_INCL_SIZE     1  
#define HDRRUP_TCA_NUM_REV_CHANNELS_SIZE          4
#define HDRRUP_TCA_REV_CHANNEL_CONFIG_SIZE        2
#define HDRRUP_TCA_REV_CHANNEL_DROPPING_RANK_SIZE 3
#define HDRRUP_TCA_PILOT_IN_THIS_SECTOR_INCL_SIZE 1
#define HDRRUP_TCA_FWD_CHANNEL_INDEX_SIZE         4
#define HDRRUP_TCA_PILOT_GROUP_ID_SIZE            3
#define HDRRUP_TCA_NUM_UNIQ_FWD_MAC_INDICES_SIZE  3
#define HDRRUP_TCA_SCHEDULER_TAG_SIZE             7
#define HDRRUP_TCA_AUX_DRC_COVER_INCLUDED_SIZE    1
#define HDRRUP_TCA_AUX_DRC_COVER_SIZE             3
#define HDRRUP_TCA_FWD_MAC_INDEX_INTERLACE_ENABLED_SIZE 1
#define HDRRUP_TCA_FWD_MAC_INDEX_SIZE            10
#define HDRRUP_TCA_ASSIGNED_INTERLACES_SIZE       4 
#define HDRRUP_TCA_REV_MAC_INDEX_SIZE             9
#define HDRRUP_TCA_RAB_MAC_INDEX_SIZE             7

/* Message Sequence number */
#define HDRRUP_TCA_MSG_SEQ( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 8, 8 )

/* Channel Included field */
#define HDRRUP_TCA_CHANNEL_INCL( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 16, 1 )

/* System Type */
#define HDRRUP_TCA_SYSTEM_TYPE( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, 17,  8 )

/* Band Class */
#define HDRRUP_TCA_BAND_CLASS( ptr, val_ptr ) \
          HDRBIT_UNPACK8( ptr, val_ptr, 25,  5 )

/* Channel Number */
#define HDRRUP_TCA_CHANNEL_NUMBER( ptr, val_ptr )  \
          HDRBIT_UNPACK16( ptr, val_ptr, 30,  11 )

/* RAB Length */
#define HDRRUP_TCA_RAB_LENGTH( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  2 )

/* DRC Length */
#define HDRRUP_TCA_DRC_LENGTH( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  2 )

/* Number of Pilots */
#define HDRRUP_TCA_NUM_PILOTS( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  4 )

/* Pilot PN */
#define HDRRUP_TCA_PILOT_PN( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, (uint16) offset,  9 )

/* Softer Handoff */
#define HDRRUP_TCA_SOFTER_HO( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* MAC Index */
#define HDRRUP_TCA_MAC_INDEX( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, (uint16) offset,  6 )

/* DRC Cover */
#define HDRRUP_TCA_DRC_COVER( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  3 )

/* Frame Offset */
#define HDRRUP_TCA_FRAME_OFFSET( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  4 )

/* DRC Channel Gain - Unpack & sign extend it */
#define HDRRUP_TCA_DRC_CHANNEL_GAIN( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8( ptr, (uint8 *) val_ptr, (uint16) offset,  6 ); \
          (*val_ptr = HDRRUP_EXTEND6(*val_ptr))

/* Ack Channel Gain - unpack & sign extend it */
#define HDRRUP_TCA_ACK_CHANNEL_GAIN( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8( ptr, (uint8 *) val_ptr, (uint16) offset,  6 ); \
          (*val_ptr = HDRRUP_EXTEND6(*val_ptr))

/* RAB Offset */
#define HDRRUP_TCA_RAB_OFFSET( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  3 )

/* rev B*/
/* Number of Sectors  */
#define HDRRUP_TCA_NUM_SECTORS( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,\
          HDRRUP_TCA_NUM_SECTORS_SIZE )

/* Number of sub active set  */
#define HDRRUP_TCA_NUM_SUB_ASET( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,\
          HDRRUP_TCA_NUM_SUB_ASET_SIZE )

/* scheduler tag included field  */
#define HDRRUP_TCA_SCHEDULER_TAG_INCL( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,\
          HDRRUP_TCA_SCHEDULER_TAG_INCL_SIZE)

/* feedback multiplexing enabled */
#define HDRRUP_TCA_FEEDBACK_MUX_ENABLED( ptr, val_ptr, offset ) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,\
          HDRRUP_TCA_FEEDBACK_MUX_ENABLED_SIZE)

/*---------------------------------------------------------------------------
            NEIGHBORLIST MESSAGE FIELD EXTRACTION MACROS

---------------------------------------------------------------------------*/
/* Size of various fields in the Neighborlist msg */
#define HDRRUP_NL_MSG_ID_SIZE                        8
#define HDRRUP_NL_NEIGHBOR_COUNT_SIZE                5
#define HDRRUP_NL_BAND_CLASS_SIZE                    5
#define HDRRUP_NL_CHANNEL_NUMBER_SIZE               11
#define HDRRUP_NL_SYSTEM_TYPE_SIZE                   8
#define HDRRUP_NL_NEIGHBOR_PN_SIZE                   9
#define HDRRUP_NL_NEIGHBOR_CHAN_INCL_SIZE            1
#define HDRRUP_NL_NEIGHBOR_SRCH_WIN_SIZE_INCL_SIZE   1
#define HDRRUP_NL_NEIGHBOR_SRCH_WINSIZE_SIZE         4
#define HDRRUP_NL_NEIGHBOR_SRCH_WIN_OFFSET_INCL_SIZE 1
#define HDRRUP_NL_NEIGHBOR_SRCH_WIN_OFFSET_SIZE      3
#define HDRRUP_NL_NEIGHBOR_FPDCH_SUPPORTED_INCL_SIZE 1
#define HDRRUP_NL_NEIGHBOR_FPDCH_SUPPORTED_SIZE      1
#define HDRRUP_NL_NEIGHBOR_PILOT_GROUPID_INCL_SIZE   1
#define HDRRUP_NL_NEIGHBOR_PILOT_GROUPID_SIZE        3

/* Number of Neighbors */
#define HDRRUP_NL_NEIGHBOR_COUNT( ptr, val_ptr )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) 8,  5 )

/* Neighbor PN */
#define HDRRUP_NL_NEIGHBOR_PN( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, (uint16) offset,  9 )

/* Neighbor Channel Included */
#define HDRRUP_NL_NEIGHBOR_CHAN_INCL( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* Channel from Channel Record */
#define HDRRUP_NL_BAND_CLASS( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  5 )

/* Channel Record. Channel Number */
#define HDRRUP_NL_CHANNEL_NUMBER( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, (uint16) offset,  11 )

/* Channel Record System Type */
#define HDRRUP_NL_SYSTEM_TYPE( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  8 )

/* Neighbor Search Window Size Included */
#define HDRRUP_NL_NEIGHBOR_SRCH_WIN_SIZE_INCL( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* Neighbor Search Window Size */
#define HDRRUP_NL_NEIGHBOR_SRCH_WINSIZE( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset, 4 )

/* Neighbor Search Window Offset Included */
#define HDRRUP_NL_NEIGHBOR_SRCH_WIN_OFFSET_INCL( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* Neighbor Search Window Offset */
#define HDRRUP_NL_NEIGHBOR_SRCH_WIN_OFFSET( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  3 )

/* FPDCHSupportedIncluded */
#define HDRRUP_NL_NEIGHBOR_FPDCH_SUPPORTED_INCL( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* FPDCHSupported */
#define HDRRUP_NL_NEIGHBOR_FPDCH_SUPPORTED( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* PilotGroupIDIncluded */
#define HDRRUP_NL_NEIGHBOR_PILOT_GROUPID_INCL( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  1 )

/* PilotGroupID */
#define HDRRUP_NL_NEIGHBOR_PILOT_GROUPID( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  3 )

/*---------------------------------------------------------------------------
                     ROUTE UPDATE MESSAGE
  The following macros/definitions are used in building a Route update msg
---------------------------------------------------------------------------*/
#define HDRRUP_RUP_MSG_ID_SIZE             8
#define HDRRUP_RUP_MSG_SEQ_SIZE            8
#define HDRRUP_RUP_REF_PILOT_PN_SIZE       9
#define HDRRUP_RUP_REF_PILOT_STRENGTH_SIZE 6
#define HDRRUP_RUP_REF_KEEP_SIZE           1
#define HDRRUP_RUP_NUM_PILOTS_SIZE         4
#define HDRRUP_RUP_PILOT_PN_PHASE_SIZE     15
#define HDRRUP_RUP_CHANNEL_INCLUDED_SIZE   1
#define HDRRUP_RUP_SYSTEM_TYPE_SIZE        8
#define HDRRUP_RUP_BAND_SIZE               5
#define HDRRUP_RUP_CHANNEL_NUMBER_SIZE     11
#define HDRRUP_RUP_PILOT_STRENGTH_SIZE     6
#define HDRRUP_RUP_KEEP_SIZE 1
#define HDRRUP_RUP_PILOT_TX_PWR_INCL_SIZE  1
#define HDRRUP_RUP_PILOT_TX_PWR_SIZE       8
#define HDRRUP_RUP_REF_PILOT_CH_INCL_SIZE  1

#ifdef FEATURE_LTE_TO_HDR_OH
#define HDRRUP_RUP_REF_PILOT_ARRIVAL_INCL_SIZE  1
#define HDRRUP_RUP_REF_PILOT_ARRIVAL_SIZE  15
#endif /* FEATURE_LTE_TO_HDR_OH */

/* Size of routeupdate message (in bits) that has only the reference
   pilot is 36 bits, with 8 bits for HMP header */ 
#define HDRRUP_RUP_MSG_REF_PILOT_ONLY_SIZE (8 + 36)

/* Minimum size of revB fields of RouteUpdate message (in bits) that is required 
   if it is sent on RTC.(i.e. ATTotalPilotTransmissionIncluded, 
   ATTotalPilotTransmission and ReferencePilotChannelIncluded fields */
#define HDRRUP_RUP_MSG_RTC_MC_INFO_MIN_SIZE (1 + 8 + 1) 

/* Maximum size of revB fields of RouteUpdate message (in bits) that is required 
   if it is sent on RTC.(i.e. Minimum size + ReferencePilotChannel field */
#define HDRRUP_RUP_MSG_RTC_MC_INFO_MAX_SIZE (1 + 8 + 1 + 24) 

/* Maximum size of pilot information fields of RouteUpdate message (in bits) if 
 * channel is included (i.e. PilotPNPhase, ChannelIncluded, Channel, 
 * PilotStrength, Keep)*/
#define HDRRUP_RUP_MSG_PILOT_FIELDS_MAX_SIZE (15 + 1 + 24 + 6 + 1)

/* Maximum size of RUP msg starting from NumPilots field. It will be used when
 * appending data to the RUP msg sent on RTC */
#define HDRRUP_RUP_MSG_APPENDED_MAX_SIZE ( HDRRUP_RUP_NUM_PILOTS_SIZE + \
     (HDRRUP_RUP_NUM_PILOTS_MAX * HDRRUP_RUP_MSG_PILOT_FIELDS_MAX_SIZE) + \
     HDRRUP_RUP_MSG_RTC_MC_INFO_MAX_SIZE )   

#define HDRRUP_RUP_MSG_APPENDED_MAX_BYTE_SIZE \
                             ((HDRRUP_RUP_MSG_APPENDED_MAX_SIZE / 8) + 1)
/*---------------------------------------------------------------------------
                   TRAFFIC CHANNEL COMPLETE MESSAGE
  The following macros/definitions are used in building a TCC msg
---------------------------------------------------------------------------*/
#define HDRRUP_TCC_MSG_ID_SIZE 8
#define HDRRUP_TCC_MSG_SEQ_SIZE 8

/*---------------------------------------------------------------------------
                    MC Bandwidth  
  The following macros/definitions are used in checking mc bandwidth  
---------------------------------------------------------------------------*/
#define HDRRUP_CONVERT_NEGOTIATED_BANDWIDTH_TO_KHZ(bdwidth) \
          (bdwidth * 25 / 10)

/*---------------------------------------------------------------------------
                    Misc.
                      
---------------------------------------------------------------------------*/
#define HDRRUP_DEFAULT_RAB_MAC_INDEX   4

/* MaxNumberofFLSupported in revB Phase1, 
   it is needed when checking many phase1 limitations
*/
#define HDRRUP_MAX_NUM_FL_SUPPORTED_PHASE_1      0x03

/*=============================================================================

                       LOCAL FUNCTION DECLARATIONS

=============================================================================*/

LOCAL void hdrrup_timer_cb
(
  uint32 timer_id
);

LOCAL void hdrrup_process_activate_cmd
(
  hdrrup_cmd_type *rup_cmd_ptr
);

LOCAL void hdrrup_process_deactivate_cmd
(
  hdrrup_cmd_type *rup_cmd_ptr
);

LOCAL void hdrrup_process_close_cmd
(
  hdrrup_cmd_type *rup_cmd_ptr
);

LOCAL void hdrrup_process_continue_conn_setup_cmd( void );

static void hdrrup_process_continue_after_conn_close_cmd
( 
  hdrrup_cmd_type *rup_cmd_ptr
);

LOCAL void hdrrup_process_rr_msg( void );

LOCAL void hdrrup_process_tca_msg
(
  dsm_item_type *item_ptr
);

LOCAL void hdrrup_process_nl_msg
(
  dsm_item_type *item_ptr
);

LOCAL hdrerrno_enum_type hdrrup_unpack_nl_msg
(
  dsm_item_type *item_ptr,
  hdrsrch_nset_pilot_type *nset_ptr,
  uint8 *num_neighbors_ptr
);


LOCAL void hdrrup_process_send_pilot_report_ind
(
  void
);

LOCAL void hdrrup_process_ovhd_info_updated_ind( void );

LOCAL void hdrrup_process_link_acquired_ind( void );

LOCAL void hdrrup_rup_report_loc_update( void );

LOCAL dsm_item_type *hdrrup_form_rup_msg
( 
  hdrrup_rup_msg_build_type rup_msg_build_type,
  uint16 max_rup_msg_size 
);

LOCAL dsm_item_type *hdrrup_pack_rup_msg
(
  hdrrup_rup_msg_type *rup_msg_ptr
);

LOCAL dsm_item_type *hdrrup_form_tcc_msg( void );

LOCAL hdrerrno_enum_type hdrrup_unpack_tca_msg
(
  dsm_item_type *item_ptr
);

LOCAL hdrerrno_enum_type hdrrup_unpack_drup_tca_msg
(
  dsm_item_type *item_ptr
);

LOCAL hdrerrno_enum_type hdrrup_unpack_mcrup_tca_msg
(
  dsm_item_type *item_ptr
);

LOCAL void hdrrup_rup_msg_tx_outcome_cb
(
  void          *data_ptr,
  hdrerrno_enum_type  status
);

LOCAL void hdrrup_tcc_msg_tx_outcome_cb
(
  void          *data_ptr,
  hdrerrno_enum_type  status
);

LOCAL void hdrrup_process_registration_outcome
(
  hdrerrno_enum_type  status
);

LOCAL void hdrrup_process_set_traffic_mode_cmd
( 
  hdrrup_cmd_type *rup_cmd_ptr
);

LOCAL void hdrrup_process_conn_setup_to_idle_transition( void );

LOCAL void hdrrup_process_set_tc_handdown_mode_cmd
( 
  hdrrup_cmd_type *rup_cmd_ptr
);

void hdrrup_process_rup_req_msg
(
  dsm_item_type *item_ptr
);

LOCAL void hdrrup_send_rup_msg( void );

LOCAL void hdrrup_send_cmd_mode
( 
  hdrsrch_tc_mode_enum_type traffic_mode,
  hdrsrch_tc_handdown_mode_enum_type handdown_mode,
  hdrhai_protocol_name_enum_type sender,
  hdrrup_cmd_name_enum_type cmd_name
);

/*============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/
/*===========================================================================
FUNCTION HDRRUP_POWERUP_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  It is called
  once upon system startup.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_powerup_init
(
  void
)
{
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "RUP: Powerup Initialization");

  if ( hdrmc_is_first_init() )
  {
    /*---------------------------------------------------------------------------
    Initialize Connection Setup timer
    ---------------------------------------------------------------------------*/
    rex_def_timer_ex( &hdrrup.connection_setup_timer,
                      hdrrup_timer_cb,
                      HDRRUP_CONN_SETUP_TIMER );

    /*---------------------------------------------------------------------------
    Initialize RUP State timer
    ---------------------------------------------------------------------------*/
    rex_def_timer_ex( &hdrrup.state_timer,
                      hdrrup_timer_cb,
                      HDRRUP_STATE_TIMER );

    /*---------------------------------------------------------------------------
    Initialize Wait for Search's pilot report timer for RUP Request message
    ---------------------------------------------------------------------------*/
    rex_def_timer_ex( &hdrrup.rup_req.pilot_rpt_timer,
                       hdrrup_timer_cb,
                       HDRRUP_PILOT_RPT_TIMER );

#ifdef FEATURE_LTE_TO_HDR_OH
    rex_def_timer_ex( &hdrrup.disable_tuneaway_timer,
                      hdrrup_timer_cb,
                      HDRRUP_DISABLE_TUNEAWAY_TIMER );
#endif /* FEATURE_LTE_TO_HDR_OH */

    /*---------------------------------------------------------------------------
    Initialize critical section
    ---------------------------------------------------------------------------*/
    rex_init_crit_sect(&hdrrup.crit_sect);
  }

  /*---------------------------------------------------------------------------
  Initialize distance based registration related parameters.
  ---------------------------------------------------------------------------*/
  hdrrup.location.reported = FALSE;

  #if !defined(FEATURE_HDR_ACCESS_HYBRID_CTRL) || defined(FEATURE_LTE_TO_HDR_OH)
  /*---------------------------------------------------------------------------
  Initialize traffic mode flag to default.  It may be changed later through a
  socket IOCTL call.
  ---------------------------------------------------------------------------*/
  hdrrup.traffic_mode = HDRSRCH_DEFAULT_TC_MODE;
  #endif

  /*---------------------------------------------------------------------------
  Initialize the pending RUP request messages and Search pilot reports.
  ---------------------------------------------------------------------------*/
  hdrrup.rup_req.pending_req = NULL;

  /*---------------------------------------------------------------------------
  Initialize the state module & the configuration module
  ---------------------------------------------------------------------------*/
  hdrrups_powerup_init();
  hdrrupc_powerup_init();

  /*---------------------------------------------------------------------------
  Initialize 1x TuneAwaye During HDR Session Negotiation enabled flag
  ---------------------------------------------------------------------------*/
  if ( efs_get( HDRRUP_1X_TUNEAWAY_IN_SES_CFG_ENABLED,
                &hdrrup.enable_1x_ta_in_ses_cfg, 
                sizeof(hdrrup.enable_1x_ta_in_ses_cfg) ) == 
                (int)sizeof(hdrrup.enable_1x_ta_in_ses_cfg) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "1x TuneAwaye During HDR Session Negotiation enabled flag=%d",
      hdrrup.enable_1x_ta_in_ses_cfg );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "1x TuneAwaye During HDR Session Negotiation flag not available in NV,\
       enabled by default");
    hdrrup.enable_1x_ta_in_ses_cfg = TRUE;
  }

} /* hdrrup_powerup_init() */

/*===========================================================================
FUNCTION HDRRUP_INIT_MC_RUP_INFO_BUFFER

DESCRIPTION
  This function initializes the MC RUP info buffer  

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_init_mc_rup_info_buffer(void)
{
  uint8 index;

  for (index = 0; index < HDRRUP_RTC_RUP_MSG_BUFS_NUM ; index++)
  {
    hdrrup.mc_rup_info[index].available = TRUE;
    hdrrup.mc_rup_info[index].msg_ptr = NULL;
  }
} /* hdrrup_init_mc_rup_info_buffer*/

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_INIT

DESCRIPTION
  This function initializes the protocol prior to use.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_init
(
  void
)
{
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Initializing RUP protocol");

  /*---------------------------------------------------------------------------
   Initialize various parameters
  ---------------------------------------------------------------------------*/
  hdrrup.rup_msg_seq_num = 0xFF;
  hdrrup.last_frame_offset = 0;
  hdrrup.tca_count = 0;
#ifdef FEATURE_IRAT_PILOT_MEAS
  hdrrup.irat_meas_in_progress = FALSE;
#endif /* FEATURE_IRAT_PILOT_MEAS */

  hdrrup_init_mc_rup_info_buffer();

  /*---------------------------------------------------------------------------
  Set the SLP function pointer to point to the correct function for getting
  the Route Update message
  ---------------------------------------------------------------------------*/
  hdrslp_route_update_gen_func = hdrrup_get_rup_msg;
  hdrrup.cp_skip_disable_tx = FALSE;

} /* hdrrup_init() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_PROCESS_CONN_SETUP_TO_IDLE_TRANSITION

DESCRIPTION
  This function does the specific state transition for changing state from
  Connection Setup to Idle.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_process_conn_setup_to_idle_transition( void )
{
  /*---------------------------------------------------------------------------
  Deactivate RTC MAC & FTC MAC. Reactivate the ACMAC.
  ---------------------------------------------------------------------------*/
  hdrrmac_deactivate(HDRHAI_ROUTE_UPDATE_PROTOCOL);
  hdrfmac_deactivate(HDRHAI_ROUTE_UPDATE_PROTOCOL);
#ifdef FEATURE_LTE_TO_HDR_OH
  if (!hdrsap_lte_to_hdr_active_ho_is_in_progress())
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdramac_activate(HDRHAI_ROUTE_UPDATE_PROTOCOL);

    /*---------------------------------------------------------------------------
    Change searcher mode back to Idle.
    ---------------------------------------------------------------------------*/
    hdrlmac_idle_mode(HDRHAI_ROUTE_UPDATE_PROTOCOL);
    
    /*---------------------------------------------------------------------------
    Since the connection never got set up, the connection closed indication is
    not going to be given. We need to so explicitly close the RLP that may have
    been initialized when a TCAM was received.
    ---------------------------------------------------------------------------*/
    hdrpac_mdm_sn_cleanup(); 

    /*---------------------------------------------------------------------------
    If the protocol had tuned to the different channel included in the TCA msg on
    entering Connection Setup , then 
    1) in the case of only one channel was assigned in TCA:
       tune back to the channel being monitored while in Idle, prior to receiving 
       the TCA msg.
    2) in the case of multiple channels were assigned in TCA:
       based on request from HDR SRCH team, always call channel change command to 
       trigger HDR SRCH to switch from wide band to narrow band
    ---------------------------------------------------------------------------*/
    if (((hdrrup.tca_msg.num_sub_asets != 1) || 
        (hdrrup.tca_msg.sub_aset[0].num_fwd_channels != 1)) ||
        (!SYS_CHAN_CMP_EQUAL( hdrrup.initial_tca.idle_link.channel,
                            hdrrup.initial_tca.assigned_channel)))
    {
      hdrind_give_ind( HDRIND_RUP_FREQ_CHANGE_INITIATED, NULL );

      hdrlmac_idle_chan_change( &hdrrup.initial_tca.idle_link,
                                FALSE, /* Command not called in sleep state */
                                HDRHAI_ROUTE_UPDATE_PROTOCOL );
    }
  }

} /* hdrrup_process_conn_setup_to_idle_transition() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to RUP.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  /*-----------------------------------------------------------------------
  Queue the message & tag it for the route update protocol.
  -----------------------------------------------------------------------*/
  hdrmc_queue_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL, HDRHAI_IN_USE, msg_ptr );
} /* hdrrup_msg_cb() */


/*===========================================================================

FUNCTION HDRRUP_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to RUP in-config instance.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_config_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  /*-----------------------------------------------------------------------
  Queue the message & tag it for the route update protocol.
  -----------------------------------------------------------------------*/
  hdrmc_queue_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL, HDRHAI_IN_CONFIG, msg_ptr );
} /* hdrrup_config_msg_cb() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to RUP.

DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Indication to process, is a global enum for all possible
                 indications in HDR signaling
  ind_data_ptr - Optional data pointer for indication specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
)
{
  rex_tcb_type *hdrsrch_tcb_ptr; /* temp ptr */

/* ----------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
  If the ind callback called from within the same task, process it
  immediately, else queue it & tag it for the overhead protocol.
  -------------------------------------------------------------------------*/
  if (rex_self() == HDRRUP_TASK_PTR)
  {
    hdrrup_process_ind( ind_name, (hdrind_ind_data_union_type *)ind_data_ptr );
  }
  else if (ind_name == HDRIND_LMAC_SEND_PILOT_REPORT)
  {
    hdrsrch_tcb_ptr = rcinit_lookup_rextask("hdrsrch");
    ASSERT (rex_self() == hdrsrch_tcb_ptr);

    /* We want to build the pilot report without switching tasks in order
     * to ensure the same pilot measurements that triggered the report are
     * the values we report.
     */
    hdrrup_process_ind( ind_name, (hdrind_ind_data_union_type *)ind_data_ptr );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_ROUTE_UPDATE_PROTOCOL , ind_name, ind_data_ptr );
  }
} /* hdrrup_ind_cb() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_ACTIVATE

DESCRIPTION
  This function commands RUP to activate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrup_activate
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrrup_cmd_type hdrrup_cmd;          /* Local struct to build command into */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the rup_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrrup_cmd.name = HDRRUP_ACTIVATE_CMD;
  hdrrup_cmd.sender = sender;

  if (rex_self() == HDRRUP_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating RUP, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrrup_process_cmd( &hdrrup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol
    Currently no other task ever activates RUP, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("RUP activated from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrup_cmd ,
                     sizeof(hdrrup_cmd_type) );
  }

} /* hdrrup_activate() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_DEACTIVATE

DESCRIPTION
  This function commands RUP to deactivate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrup_deactivate
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrrup_cmd_type hdrrup_cmd;          /* Local struct to build command into */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the rup_cmd_type for the Deactivate command
  ---------------------------------------------------------------------------*/
  hdrrup_cmd.name = HDRRUP_DEACTIVATE_CMD;
  hdrrup_cmd.sender = sender;

  if (rex_self() == HDRRUP_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating RUP, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrrup_process_cmd( &hdrrup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol
    Currently no other task ever deactivates RUP, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("RUP deactivated from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrup_cmd ,
                     sizeof(hdrrup_cmd_type) );
  }

} /* hdrrup_deactivate() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_CLOSE

DESCRIPTION
  This function commands RUP to close connection.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrup_close
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrrup_cmd_type hdrrup_cmd;             /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the rup_cmd_type for the Close command
  ---------------------------------------------------------------------------*/
  hdrrup_cmd.name = HDRRUP_CLOSE_CMD;
  hdrrup_cmd.sender = sender;

  if (rex_self() == HDRRUP_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating RUP, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrrup_process_cmd( &hdrrup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol .Currently no
    other task ever gives close cmd to RUP, so we should never get here
    -------------------------------------------------------------------------*/
    ERR("RUP: Close command rcvd from outside MC!", 0, 0, 0);
    hdrmc_queue_cmd( HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrup_cmd ,
                     sizeof(hdrrup_cmd_type) );
  }

} /* hdrrup_close() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_CONTINUE_CONNECTION_SETUP

DESCRIPTION
  This function commands RUP to continue processing connection setup

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrup_continue_connection_setup
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrrup_cmd_type hdrrup_cmd;          /* Local struct to build command into */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the rup_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrrup_cmd.name = HDRRUP_CONTINUE_CONNECTION_SETUP_CMD;
  hdrrup_cmd.sender = sender;

  if (rex_self() == HDRRUP_TASK_PTR)
  {
    hdrrup_process_cmd( &hdrrup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol
     -------------------------------------------------------------------------*/
    hdrmc_queue_cmd( HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrup_cmd ,
                     sizeof(hdrrup_cmd_type) );
  }
} /* hdrrup_continue_connection_setup() */

/*===========================================================================

FUNCTION HDRRUP_CONTINUE_AFTER_CONNECTION_CLOSE

DESCRIPTION
  This function commands RUP to continue any actions that were left pending
  while the connection was closing.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrup_continue_after_connection_close
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrrup_cmd_type hdrrup_cmd;          /* Local struct to build command into */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the rup_cmd_type for the Activate command
  ---------------------------------------------------------------------------*/
  hdrrup_cmd.name = HDRRUP_CONTINUE_AFTER_CONN_CLOSE_CMD;
  hdrrup_cmd.sender = sender;

  if (rex_self() == HDRRUP_TASK_PTR)
  {
    hdrrup_process_cmd( &hdrrup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol
     -------------------------------------------------------------------------*/
    hdrmc_queue_cmd( HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrup_cmd ,
                     sizeof(hdrrup_cmd_type) );
  }
} /* hdrrup_continue_after_connection_close() */

/*===========================================================================

FUNCTION HDRRUP_PROCESS_SEND_RUP_MSG_CMD

DESCRIPTION
  This function is called to force the AT to send a RouteUpdate message.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrrup_process_send_rup_msg_cmd( void )
{
  hdrrup_send_rup_msg();

} /* hdrrup_process_send_rup_msg_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to RUP.

DEPENDENCIES
  None

PARAMETERS
  rup_cmd_ptr - Pointer to the command for RUP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrup_process_cmd
(
  hdrrup_cmd_type *rup_cmd_ptr
)
{

  HDRTRACE_CMD( HDRHAI_ROUTE_UPDATE_PROTOCOL, rup_cmd_ptr->name,
    rup_cmd_ptr->sender, hdrrups_get_state() );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  /*---------------------------------------------------------------------------
  Determine which command has been received.
  ---------------------------------------------------------------------------*/
  switch ( rup_cmd_ptr->name )
  {
    case HDRRUP_ACTIVATE_CMD:
      /*-----------------------------------------------------------------------
      Activate route update protocol command
      -----------------------------------------------------------------------*/
      hdrrup_process_activate_cmd( rup_cmd_ptr );
      break;

    case HDRRUP_DEACTIVATE_CMD:
      /*-----------------------------------------------------------------------
      Deactivate route update protocol command
      -----------------------------------------------------------------------*/
      hdrrup_process_deactivate_cmd( rup_cmd_ptr );
      break;

    case HDRRUP_CLOSE_CMD:
      /*-----------------------------------------------------------------------
      Close command
      -----------------------------------------------------------------------*/
      hdrrup_process_close_cmd( rup_cmd_ptr );
      break;

    case HDRRUP_CONTINUE_CONNECTION_SETUP_CMD:
      /*-----------------------------------------------------------------------
      Continue with connection set up 
      -----------------------------------------------------------------------*/
      hdrrup_process_continue_conn_setup_cmd(); 
      break;

    case HDRRUP_SET_TRAFFIC_MODE_CMD:
      /*-----------------------------------------------------------------------
      Set the new traffic mode and tell HDRSRCH about it if we are connected
      -----------------------------------------------------------------------*/
      hdrrup_process_set_traffic_mode_cmd(rup_cmd_ptr);
      break;

    case HDRRUP_CONTINUE_AFTER_CONN_CLOSE_CMD:
      /*-----------------------------------------------------------------------
      Continue with actions that were left pending while the connection was
      closing.
      -----------------------------------------------------------------------*/
      hdrrup_process_continue_after_conn_close_cmd( rup_cmd_ptr ); 
      break;

    case HDRRUP_SET_TC_HANDDOWN_MODE_CMD:
      /*-----------------------------------------------------------------------
      Set the new traffic handdown mode and tell HDRSRCH about it if we 
      are connected
      -----------------------------------------------------------------------*/
      hdrrup_process_set_tc_handdown_mode_cmd(rup_cmd_ptr);
      break;

    case HDRRUP_SEND_RUP_MSG_CMD:
      /*-----------------------------------------------------------------------
      Send RouteUpdate message
      -----------------------------------------------------------------------*/
      hdrrup_process_send_rup_msg_cmd();
      break;

#ifdef FEATURE_IRAT_PILOT_MEAS
    case HDRRUP_IRAT_MEAS_REQ_CMD:
      /*-----------------------------------------------------------------------
      Process IRAT measurement request.
      -----------------------------------------------------------------------*/
      if (!hdrrup.irat_meas_in_progress)
      {
        hdrrup.irat_meas_in_progress = TRUE;
        hdrlmac_irat_pilot_measurements_req( rup_cmd_ptr->pilot_meas_req );
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                     "IRAT measurement in progress - dropping new req");
      }
      break;
#endif /* FEATURE_IRAT_PILOT_MEAS */

    default:
      /*-----------------------------------------------------------------------
      Unrecognized command.
      -----------------------------------------------------------------------*/
      ERR("RUP: Received Unrecognized command:%d", rup_cmd_ptr->name, 0, 0 );
      break;
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

} /* hdrrup_process_cmd() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDRRUP_PROCESS_ACTIVATE_CMD

DESCRIPTION
  This function processes the activate command given to the RUP

DEPENDENCIES
  hdrrup_init should have been called before calling this function.

PARAMETERS
  rup_cmd_ptr - Pointer to the command for RUP to process

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrrup_process_activate_cmd
(
  hdrrup_cmd_type *rup_cmd_ptr
)
{

  hdrrups_state_enum_type rup_state = hdrrups_get_state();
                                      /* Current RUP state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rup_state != HDRRUPS_INACTIVE )
  {
    /*-------------------------------------------------------------------------
    If the Protocol is not in inactive state, do nothing.
    -------------------------------------------------------------------------*/
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                 rup_cmd_ptr->name, rup_cmd_ptr->sender,
                                 rup_state );
    return;
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_ROUTE_UPDATE_PROTOCOL ) &&
       !hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
  {
    hdrrups_change_state( HDRRUPS_TUNNEL );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdrrups_change_state( HDRRUPS_IDLE );
  }

} /* hdrrup_process_activate_cmd() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDRRUP_PROCESS_DEACTIVATE_CMD

DESCRIPTION
  This function processes the deactivate command given to the RUP

DEPENDENCIES
  None.

PARAMETERS
  rup_cmd_ptr - Pointer to the command for RUP to process

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrrup_process_deactivate_cmd
(
  hdrrup_cmd_type *rup_cmd_ptr
)
{

  hdrrups_state_enum_type rup_state = hdrrups_get_state();
                                      /* Current RUP state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( rup_state == HDRRUPS_INACTIVE )
  {
    /*-------------------------------------------------------------------------
    If the Protocol is in inactive state, do nothing.
    -------------------------------------------------------------------------*/
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                 rup_cmd_ptr->name, rup_cmd_ptr->sender,
                                 rup_state );
    return;
  }

#ifdef FEATURE_IRAT_PILOT_MEAS
  hdrrup.irat_meas_in_progress = FALSE;
#endif /* FEATURE_IRAT_PILOT_MEAS */

  /*---------------------------------------------------------------------------
  Deactivate RTC MAC & FTC MAC
  ---------------------------------------------------------------------------*/
  hdrrmac_deactivate(HDRHAI_ROUTE_UPDATE_PROTOCOL);
  hdrfmac_deactivate(HDRHAI_ROUTE_UPDATE_PROTOCOL);

  /*---------------------------------------------------------------------------
  Transition to inactive state
  ---------------------------------------------------------------------------*/
  hdrrups_change_state( HDRRUPS_INACTIVE );

  dsm_free_packet( &hdrrup.pending_tca_msg_ptr );

  /* Search will not send the Pilot Report in Inactive state. Cancel
   * the Pilot Report timer */
  (void) rex_clr_timer( &hdrrup.rup_req.pilot_rpt_timer );

  /* Clear the pending RUP message. dsm_free_packet will set the
   * pending_req to NULL upon return. */
  dsm_free_packet( &hdrrup.rup_req.pending_req );

} /* hdrrup_process_deactivate_cmd() */

/* <EJECT> */

/*=============================================================================

FUNCTION HDRRUP_PROCESS_CLOSE_CMD

DESCRIPTION
  This function processes the close command given to the RUP

DEPENDENCIES
  None.

PARAMETERS
  rup_cmd_ptr - Pointer to the command for RUP to process

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void hdrrup_process_close_cmd
(
  hdrrup_cmd_type *rup_cmd_ptr
)
{

  hdrrups_state_enum_type rup_state = hdrrups_get_state();
                                      /* Current RUP state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(( rup_state == HDRRUPS_INACTIVE ) ||
     ( rup_state == HDRRUPS_IDLE ))
  {
    /*-------------------------------------------------------------------------
    If the Protocol is idle or inactive state, do nothing.
    -------------------------------------------------------------------------*/
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                 rup_cmd_ptr->name, rup_cmd_ptr->sender,
                                 rup_state );
    return;
  }

  /*---------------------------------------------------------------------------
  Transition to Idle state. Special transition processing needed if transition
  is from Connection Setp to Idle
  ---------------------------------------------------------------------------*/
  if( rup_state == HDRRUPS_CONNECTION_SETUP )
  {
    hdrrup_process_conn_setup_to_idle_transition();
    hdrrups_change_state( HDRRUPS_IDLE );
  }
  else
  {
#ifdef  FEATURE_LTE_TO_HDR_OH
    if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
    {
      hdrrups_change_state( HDRRUPS_INACTIVE );
    }
    else
#endif  /* FEATURE_LTE_TO_HDR_OH */
    {
      hdrrups_change_state( HDRRUPS_IDLE );
    }
  }

} /* hdrrup_process_close_cmd() */

/* <EJECT> */

#ifdef  FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRRUP_EXTRACT_LTE_TO_HDR_ACTIVE_HO_CHAN

DESCRIPTION
  This function extracts a channel record from a active handoff information 
  pointed to by the void_info_ptr.  A pointer to this function is passed to SD 
  along with a pointer to the void_info_ptr containing the redirect message 
  payload from LTE, and SD passes the pointer back here to get the channels
  one at a time.

DEPENDENCIES
  None

PARAMETERS
  void_item_ptr - Pointer to the redirect information payload from LTE
  index - Which channel is being requested
  system_ptr - Where to place the extracted system information

RETURN VALUE
  TRUE if a channel is successfully extracted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrrup_extract_lte_to_hdr_active_ho_chan
(
  void *void_info_ptr,
  int chan_idx,
  sd_sys_s_type *system_ptr
)
{
  sys_channel_type *tca_channel_info;
    /* Channel Info */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tca_channel_info = (sys_channel_type*)void_info_ptr;

  if (chan_idx == 0)
  {
    system_ptr->band = tca_channel_info->band;
    system_ptr->chan = tca_channel_info->chan_num;
    system_ptr->mode = SYS_SYS_MODE_HDR;
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* hdrrup_extract_lte_to_hdr_active_ho_chan() */
#endif  /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

FUNCTION HDRRUP_PROCESS_CONTINUE_CONN_SETUP_CMD

DESCRIPTION
The HDRRUP_PROCESS_CONTINUE_CONN_SETUP_CMD is given to RUP when all other tasks
have finished any processing they needed to do prior to enabling the RTC.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrrup_process_continue_conn_setup_cmd( void )
{
#ifdef FEATURE_LTE_TO_HDR_OH
  sd_ss_act_e_type sd_action;  
    /* Action returned from SD */
#endif  /* FEATURE_LTE_TO_HDR_OH */
/*------------------------------------------------------------------------ */

  uint16 rup_subtype = hdrscp_get_current_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);

  if(hdrrups_get_state() == HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP )
  {
    hdrrups_change_state( HDRRUPS_CONNECTION_SETUP );

#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
    {
      sd_action = sd_ss_ind_hdr_user_redir_lte( hdrrup_extract_lte_to_hdr_active_ho_chan, 
                                    &hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel, 
                                    SD_SS_REDIR_HDR_LTE_RESEL_OPT_HO,
                                    NULL );

      if (sd_action != SD_SS_ACT_ACQ_HDR)
      {
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                        "Continue conn setup, SD action %d", sd_action );
        hdrind_give_ind( HDRIND_RUP_EXIT_HDR_MODE, NULL );
        return;
      }
    }
#endif /* FEATURE_LTE_TO_HDR_OH */

    hdrlmac_tc_assign( TRUE,
                       hdrrup.tca_msg.link_id,
                       hdrrup.tc_tune_away,
                       hdrrup.cc_saset_index,
                       &hdrrup.tca_msg,
                       HDRHAI_ROUTE_UPDATE_PROTOCOL,
                       rup_subtype );
  }
}
/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_PROCESS_SET_TRAFFIC_MODE_CMD

DESCRIPTION
  Process the command to change the traffic mode used by HDRSRCH and SHDR.

DEPENDENCIES
  None

PARAMETERS
  rup_cmd_ptr - Pointer to the command for RUP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrrup_process_set_traffic_mode_cmd
( 
  hdrrup_cmd_type *rup_cmd_ptr
)
{
#if !defined(FEATURE_HDR_ACCESS_HYBRID_CTRL) || defined(FEATURE_LTE_TO_HDR_OH)
  hdrrup.traffic_mode = rup_cmd_ptr->traffic_mode;
#endif

#ifdef FEATURE_LTE_TO_HDR_OH
  if (rex_get_timer(&hdrrup.disable_tuneaway_timer))
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, "IRAT: tuneaway disabled, not setting tc mode");
  }
  else
#endif
#ifndef FEATURE_HDR_ACCESS_HYBRID_CTRL
  if ( hdrrups_get_state() == HDRRUPS_CONNECTED )
#endif
  {
    hdrlmac_set_tc_mode( rup_cmd_ptr->traffic_mode );
  }

} /* hdrrup_process_set_traffic_mode_cmd() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_PROCESS_CONTINUE_AFTER_CONN_CLOSE_CMD

DESCRIPTION
  Process the command to change the traffic mode used by HDRSRCH and SHDR.

DEPENDENCIES
  None

PARAMETERS
  rup_cmd_ptr - Pointer to the command for RUP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void hdrrup_process_continue_after_conn_close_cmd
( 
  hdrrup_cmd_type *rup_cmd_ptr
)
{
  hdrrups_state_enum_type rup_state = hdrrups_get_state();
                                      /* Current RUP state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrrup.pending_tca_msg_ptr != NULL)
  {
    if (!hdrsmp_session_is_open(HDRHAI_ROUTE_UPDATE_PROTOCOL) )
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Abandon bundled TCA");
      dsm_free_packet( &hdrrup.pending_tca_msg_ptr );
    }
    else if (rup_state == HDRRUPS_IDLE)
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Continuing to process bundled TCA");
      hdrrup_process_tca_msg( hdrrup.pending_tca_msg_ptr );
      dsm_free_packet( &hdrrup.pending_tca_msg_ptr );
    }
    else
    {
      HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, 
                      "Can't continue processing bundled TCA", rup_state);

      HDRTRACE_CMD_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                   rup_cmd_ptr->name, rup_cmd_ptr->sender,
                                   rup_state );
    }
  }
} /* hdrrup_process_continue_after_conn_close_cmd() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_PROCESS_SET_TC_HANDDOWN_MODE_CMD

DESCRIPTION
  Process the command to change the traffic channel handdown mode used 
  by HDRSRCH and data applications.

DEPENDENCIES
  None

PARAMETERS
  rup_cmd_ptr - Pointer to the command for RUP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrrup_process_set_tc_handdown_mode_cmd
( 
  hdrrup_cmd_type *rup_cmd_ptr
)
{
  hdrlmac_set_tc_handdown_mode( (hdrsrch_tc_handdown_mode_enum_type)
                                rup_cmd_ptr->handdown_mode );

} /* hdrrup_process_set_tc_handdown_mode_cmd() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_PROCESS_LMAC_TCA_FAILED_IND

DESCRIPTION
  This function processes the LMAC TCA FAILED IND which is generated when 
  initial traffic channel setup fails due to channels specified in TCA conflict 
  with 1X channels.   
 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrup_process_lmac_tca_failed_ind( void )
{
  hdrind_ind_data_union_type ind_data;
    /* Indication data */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ind_data.rup_conn_fail_reason = HDRRUP_CONN_FAIL_TCA_FAILED;

  if( hdrrups_get_state() != HDRRUPS_CONNECTION_SETUP )
  {
    /*-------------------------------------------------------------------------
    If the Protocol is not in idle state, cause error.
    -------------------------------------------------------------------------*/
    ERR("RUP is not Conn Setup. Ignoring TCA FAILED ind", 0, 0, 0 );
    return;
  }

  /*---------------------------------------------------------------------------
  Transition to Idle State & do the associated transition processing
  ---------------------------------------------------------------------------*/
  hdrrup_process_conn_setup_to_idle_transition();

#ifdef FEATURE_LTE_TO_HDR_OH
  if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
  {
    hdrrups_change_state( HDRRUPS_INACTIVE );

    /*------------------------------------------------------------------------
    Give indication for exit HDR upon Connection Setup failure
    -------------------------------------------------------------------------*/
    hdrind_give_ind (HDRIND_RUP_EXIT_HDR_MODE, NULL );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdrrups_change_state( HDRRUPS_IDLE );

    /*------------------------------------------------------------------------
    Give indication for Connection Setup failure
    -------------------------------------------------------------------------*/
    hdrind_give_ind( HDRIND_RUP_CONN_SETUP_FAILED, (void*) (&ind_data));
  }

} /* hdrrup_process_lmac_tca_failed_ind() */

/* <EJECT> */

#ifdef FEATURE_IRAT_PILOT_MEAS
/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_PROCESS_IRAT_PLT_MEAS_DONE_IND

DESCRIPTION
  None
    
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrup_process_irat_plt_meas_done_ind
( 
  hdrind_ind_name_enum_type  ind_name
)
{
  sys_pilot_pn_type ref_pn;
  sys_channel_type ref_pn_channel;
  int16 ref_pn_strength;
  int pilot_count = 0;                                 
  hdrsrch_pilot_meas_rpt_type pilot_meas_list[HDRSRCH_PILOT_MEAS_SECTOR_LIST_MAX_SIZE];
  hdrerrno_enum_type status = E_SUCCESS;
  cm_mc_rpt_type *cmd_ptr;
  uint8 i;
  /* Current RUP state */
  hdrrups_state_enum_type rup_state = hdrrups_get_state();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrrup.irat_meas_in_progress == FALSE)
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                HDRIND_LMAC_NETWORK_LOST,
                                rup_state );

    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
      "Recvd sys-loss ind when IRAT measurement is not in progress");
    return;
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
      "Recvd srch response for IRAT measurement request");
  }

  /* Reset flag to indicate that pilot measurement is done */
  hdrrup.irat_meas_in_progress = FALSE;

  /* Queue the message on CM's report queue */
  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->hdr_meas_resp.hdr.cmd = CM_HDR_MEAS_RESP_F;
    cmd_ptr->hdr_meas_resp.hdr.status = CM_DONE_S;

    if (ind_name == HDRIND_LMAC_IRAT_PLT_MEAS_DONE_IND)
    {
      /* Get the pilot measurement report from searcher */
      pilot_count = hdrsrch_get_pilot_meas_report( &ref_pn, 
                                              &ref_pn_channel,
                                              &ref_pn_strength,
                                              16,
                                              pilot_meas_list );

      if (pilot_count >= 0)
      {
        /* Translate from SRCH to CM pilot measurement request data 
           structure */
        cmd_ptr->hdr_meas_resp.meas_resp.meas_status = CM_SS_MEAS_STATUS_OK;

        cmd_ptr->hdr_meas_resp.meas_resp.ref_pilot_pn = ref_pn;
        cmd_ptr->hdr_meas_resp.meas_resp.ref_pilot_strength = 
            ref_pn_strength;
        cmd_ptr->hdr_meas_resp.meas_resp.ref_pilot_chan = 
            ref_pn_channel;
        cmd_ptr->hdr_meas_resp.meas_resp.num_pilots = pilot_count;

        for (i = 0; i < pilot_count; i++ )
        {
          cmd_ptr->hdr_meas_resp.meas_resp.pilot_info[i].pilot_pn = 
              pilot_meas_list[i].pilot_pn;
          cmd_ptr->hdr_meas_resp.meas_resp.pilot_info[i].pilot_pn_phase =  
              pilot_meas_list[i].pn_phase;
          cmd_ptr->hdr_meas_resp.meas_resp.pilot_info[i].is_chan_incl = 
              pilot_meas_list[i].channel_included;
          cmd_ptr->hdr_meas_resp.meas_resp.pilot_info[i].channel =  
              pilot_meas_list[i].channel;
          cmd_ptr->hdr_meas_resp.meas_resp.pilot_info[i].pilot_strength = 
              pilot_meas_list[i].strength;
        }
      }
      else
      {
        cmd_ptr->hdr_meas_resp.meas_resp.meas_status = 
            CM_SS_MEAS_STATUS_BAR_PARM;
      }
    }
    else if (ind_name == HDRIND_LMAC_NETWORK_LOST)
    {
      cmd_ptr->hdr_meas_resp.meas_resp.meas_status = 
          CM_SS_MEAS_STATUS_NO_SRV;
    }

    HDR_MSG_PROT_1( MSG_LEGACY_LOW, 
                    "RUP: Send CM IRAT meas rpt %d",cmd_ptr->hdr.cmd );
    cm_mc_rpt(cmd_ptr);

  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "RUP: No free buffers on cm_mc_rpt_free_q" );
  }

} /* hdrrup_process_irat_plt_meas_done_ind */
/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_IRAT_PILOT_MEAS_REQ

DESCRIPTION
  This function commands RUP to get pilot measurements from searcher.
  
DEPENDENCIES
  None

PARAMETERS
  sender - Protocol sending this message.
  pilot_meas - Pilots for which measurement is requested.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrup_irat_pilot_meas_req
( 
  hdrhai_protocol_name_enum_type  sender,
  cm_ss_hdr_info_s_type           pilot_meas
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrrup_cmd_type  hdrrup_cmd;
  uint8 i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in all members of the hdrrup_cmd_type for this command */
  hdrrup_cmd.name = HDRRUP_IRAT_MEAS_REQ_CMD;
  hdrrup_cmd.sender = sender;

  /* Translate from CM to SRCH pilot measurement request data 
     structure */
  hdrrup_cmd.pilot_meas_req.channel_included = TRUE;
  hdrrup_cmd.pilot_meas_req.channel = pilot_meas.channel;
  hdrrup_cmd.pilot_meas_req.window_size_included = pilot_meas.srch_win_size_inc;
  hdrrup_cmd.pilot_meas_req.window_offset_included = FALSE;
  hdrrup_cmd.pilot_meas_req.sector_count = pilot_meas.neighbor_count;

  for (i = 0; i < pilot_meas.neighbor_count; i++)
  {
    hdrrup_cmd.pilot_meas_req.sector_info[i].pilot_pn = 
        pilot_meas.neighbor_pilot_pn[i];
    hdrrup_cmd.pilot_meas_req.sector_info[i].window_offset = 0;
    hdrrup_cmd.pilot_meas_req.sector_info[i].window_size = 
        pilot_meas.srch_win_size;
  }

  /* Queue or immediately process the command based on the current 
     task context */
  if ( rex_self() == HDRRUP_TASK_PTR )
  {
    hdrrup_process_cmd( &hdrrup_cmd );
  }
  else
  {
    hdrmc_queue_cmd( HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrup_cmd,
                     sizeof( hdrrup_cmd_type ) );
  }

} /* hdrrup_irat_pilot_meas_req */

/*===========================================================================
FUNCTION hdrrup_is_irat_meas_in_progress

DESCRIPTION
  This function returns whether IRAT measurement is in progress or not.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if IRAT measurement is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrrup_is_irat_meas_in_progress( void )
{

  return hdrrup.irat_meas_in_progress;

} /* hdrrup_is_irat_meas_in_progress */

#endif /* FEATURE_IRAT_PILOT_MEAS */
/*===========================================================================

FUNCTION HDRRUP_PROCESS_IND

DESCRIPTION
  This function processes indications given to RUP.

DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication for RUP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrrup_process_ind
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type *ind_data_ptr
)
{

  hdrrups_state_enum_type rup_state = hdrrups_get_state();
                                      /* Current RUP state */

  sd_ss_act_e_type  ret_val = SD_SS_ACT_CONTINUE;
                              /* action returned by SD */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_IND( HDRHAI_ROUTE_UPDATE_PROTOCOL, ind_name, rup_state );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  /*---------------------------------------------------------------------------
  Determine which indication has been received.
  ---------------------------------------------------------------------------*/
  switch ( ind_name )
  {
    case HDRIND_LMAC_SEND_PILOT_REPORT:
      /*-----------------------------------------------------------------------
      Indication from lmac that the Pilot sets have changed.
      -----------------------------------------------------------------------*/
      hdrrup_process_send_pilot_report_ind();
      break;

    case HDRIND_OVHD_INFO_UPDATED:
      /*-----------------------------------------------------------------------
      Indication from OVHD that the info has been updated
      -----------------------------------------------------------------------*/
      hdrrup_process_ovhd_info_updated_ind();
      break;

    case HDRIND_RTCMAC_LINK_ACQUIRED:
      /*-----------------------------------------------------------------------
      Link Acquired indication from RTC MAC
      -----------------------------------------------------------------------*/
      hdrrup_process_link_acquired_ind();
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      /*-----------------------------------------------------------------------
      Configuration Related indications
      -----------------------------------------------------------------------*/
      hdrrupc_process_ind( ind_name, ind_data_ptr );
      break;

  case HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL:
      /*-----------------------------------------------------------------------
      Every Access Channel transmission is accompanied by a Route Update
      message transmission. Update the location at which the last RUP msg was
      sent
      -----------------------------------------------------------------------*/
      hdrrup_rup_report_loc_update();

      ret_val = sd_ss_ind_hdr_opr_acc_term(SD_SS_ACC_TERM_NORMAL, NULL);

      if ( ret_val != SD_SS_ACT_CONTINUE )
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "TX Successful, SD says %d",ret_val);
      }
      break;

    case HDRIND_RUP_PILOT_RPT_RECEIVED:
      /*-----------------------------------------------------------------------
      Pilot Report for RUP Request has been received.
      -----------------------------------------------------------------------*/

      /* Clear the Pilot Report Timer. */
      (void) rex_clr_timer( &hdrrup.rup_req.pilot_rpt_timer );

      /* Process any pending RUP requests. */
      if ( hdrrup.rup_req.pending_req != NULL )
      {
        /*------------------------------------------------------------------
          1. If this pending request msg was received in Idle state but now
          AT moves to a different state, ignore the message.
          2. If this pending request msg was received in connected state but
          now AT moves to a different state, ignore the message. This is
          done in hdrrup_process_msg function, thus no checking here.
        ------------------------------------------------------------------*/
        if ( !HDRDSM_GET_TRAFFIC_BIT( hdrrup.rup_req.pending_req ) &&
             ( hdrrups_get_state() != HDRRUPS_IDLE )
           )
        {
          HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                          "Pending RUR received in Idle but curstate=%d)",
                          hdrrups_get_state());
          dsm_free_packet( &hdrrup.rup_req.pending_req );
        }
        else
        {
          hdrrup_process_msg( hdrrup.rup_req.pending_req, HDRHAI_IN_USE );
          hdrrup.rup_req.pending_req = NULL;
            /* Reset the pending_req to NULL. */
        }
      }
      break;

#if defined (FEATURE_SVDO) || defined (FEATURE_LTE_TO_HDR_OH)
    case HDRIND_LMAC_TCA_FAILED:
      /*-----------------------------------------------------------------------
      TCA FAILED indication from LMAC 
      -----------------------------------------------------------------------*/
      hdrrup_process_lmac_tca_failed_ind();
      break;
#endif /* FEATURE_SVDO || FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_IRAT_PILOT_MEAS
    case HDRIND_LMAC_IRAT_PLT_MEAS_DONE_IND:
    case HDRIND_LMAC_NETWORK_LOST:
      hdrrup_process_irat_plt_meas_done_ind( ind_name );
      break;

#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_SAP_CONNECTION_INITIATED:
      hdrind_give_ind( HDRIND_RUP_CONNECTION_INITIATED, NULL );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      /*-----------------------------------------------------------------------
      Default: Indication not recognized.
      -----------------------------------------------------------------------*/
      ERR("RUP: Unrecognized Indication: %d", ind_name, 0, 0 );
      break;
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

} /* hdrrup_process_ind() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_PROCESS_SEND_PILOT_REPORT_IND

DESCRIPTION
  This function processes the Send Pilot Report indication.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrup_process_send_pilot_report_ind
(
  void
)
{
  dsm_item_type *rup_msg_dsm_ptr = NULL;  /* Packed RUP msg pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine if in valid state */
  if( hdrrups_get_state() != HDRRUPS_CONNECTED )
  {
    /*-------------------------------------------------------------------------
    If the Protocol is not in connected state, do nothing.
    -------------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_HIGH, 
        "RUP not in CONNECTED. Ignore LMAC_IND_PILOT_STRENGTH_MEAS");
    return;
  }

  if (hdrcon_connection_is_closing(HDRHAI_ROUTE_UPDATE_PROTOCOL))
  {
    /*-------------------------------------------------------------------------
    The Protocol is in connected state, but the connection is closing.
    If we sent a message now it would never make it out on the RTC, but 
    HMP would re-open the connection just to send it, which would be bad.
    -------------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                  "Con is closing, ignore LMAC_IND_PILOT_STRENGTH_MEAS");
    return;
  }

  if( hdrsmp_session_is_open(HDRHAI_ROUTE_UPDATE_PROTOCOL) )
  {
    /* Get the pilot report & form route update msg */
    if( (rup_msg_dsm_ptr = hdrrup_form_rup_msg( HDRRUP_BUILD_NEW_RUP_MESSAGE, 
                                   HDRRUP_RUP_SIZE_NO_RESTRICTION )) == NULL )
    {
      ERR("Could not form RUP msg.", 0, 0, 0 );
      return;
    }

    /* Keep track of the number of Route Update Protocol messages sent
       for logging purposes. */
    hdrplog_inc_conn_state_stats_counter( HDRPLOG_RUP_MESSAGE_COUNT, 1 );

    /*-----------------------------------------------------------------------
       Send the route update msg on the reverse traffic channel.  
       It is still possible that the connection will be closing before HMP 
       gets a chance to give the message to SLP for transmit.  In this case,
       the message will be discarded by HMP who will not attempt to open a
       new connection just to send out this message 
    -----------------------------------------------------------------------*/
    hdrhmp_send_msg_on_existing_rtc( 
      HDRHAI_ROUTE_UPDATE_PROTOCOL,
      TRUE,
      HDRRUP_RUP_MSG_PRIORITY,
      HDRHAI_IN_USE, /* Instance of protocol sending message */
      rup_msg_dsm_ptr,
      NULL,
      NULL );

    /*-----------------------------------------------------------------------
    Update the current location, that is the location at which the Route
    Update message is reported
    -----------------------------------------------------------------------*/
    hdrrup_rup_report_loc_update();
  }

} /* hdrrup_process_pilot_sets_changed_ind() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_RUP_REPORT_LOC_UPDATE

DESCRIPTION
  This function updates the location of the last reported Route Update
  Message to the current location,

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  If accessing the Sector Parameters fails, then hdrrup.location.reported
  is set to FALSE
===========================================================================*/
LOCAL void hdrrup_rup_report_loc_update( void )
{
  /* Location given by the most recent SP message */
  hdrovhd_sp_msg_location_type sp_msg_location;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
  Update the current location, that is the location at which the Route Update
  message is reported
  -------------------------------------------------------------------------*/
  if( hdrovhd_get_sp_msg_location( &sp_msg_location, 
        HDRHAI_ROUTE_UPDATE_PROTOCOL ) == E_SUCCESS )
  {
    hdrrup.location.latitude = sp_msg_location.latitude ;
    hdrrup.location.longitude = sp_msg_location.longitude ;
    hdrrup.location.rup_radius = sp_msg_location.rup_radius ;
    hdrrup.location.reported = TRUE;

    /* Report to OVHD for RouteUpdateTriggerCode list maintenance */
    hdrovhd_registration_success();

  }
  else
  {
    /*-----------------------------------------------------------------------
    Getting the SP msg failed. Thus the stored last location at which RUP msg
    was updated is incorrect and cannot be used in distance based Route
    Update reporting. We set RUP reported to FALSE to ensure the next
    time RUP is sent irrespective of the distance calculation.
    -----------------------------------------------------------------------*/
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "RUP: Reading SP parameters Failed!");
    hdrrup.location.reported = FALSE;
  }
} /* hdrrup_rup_report_loc_update() */

/* <EJECT> */
  
/*===========================================================================
FUNCTION HDRRUP_SORT_PILOTREPORT

DESCRIPTION
  This function sorts pilot report returned by LMAC in descending order of 
  pilot strength.
  
DEPENDENCIES
  None.

PARAMETERS
  num_pilots - Number of pilots in pilot report array

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrup_sort_pilotreport( int num_pilots )
{
   hdrsrch_pilot_rpt_type tmp_pilot_rpt;
     /* Temporary storage for pilot report used for swapping */
   boolean  pilots_swapped;
     /* Flag indicating whether we swapped any on this pass */
   uint8  p;
     /* Index into the pilot report array */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   ASSERT(num_pilots <= HDRRUP_RUP_NUM_PILOTS_MAX);

   /* Bubble sort algorithm for sorting the pilot report */
   do
   {
     pilots_swapped = FALSE;
     /* Bubble sort boolean variable which indicates the sort status of
      * the entire array */

     /* First element is reference pilot, leave it at first position and
      * sort the rest */
     for ( p = 1; p < (num_pilots-1); p++ )
     {
       /* Smaller value indicates better pilot strength, bubble stronger
        * pilots to beginning of the array */
       if ( hdrrup.pilot_rpt[p].strength > hdrrup.pilot_rpt[p+1].strength )
       {
         /* Move the stronger pilot to front of the array */
         tmp_pilot_rpt         = hdrrup.pilot_rpt[p];
         hdrrup.pilot_rpt[p]   = hdrrup.pilot_rpt[p+1];
         hdrrup.pilot_rpt[p+1] = tmp_pilot_rpt;
         
         pilots_swapped = TRUE;     
       }
     }
   } while ( pilots_swapped );    

} /* hdrrup_sort_pilotreport() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_GET_RUP_MSG_MIN_SIZE

DESCRIPTION
  This function returns the minimum size needed for RUP message 

  Following is the structure of Route update message
  Field                    Length ( bits )
  -----                    ---------------
  MessageID                      8
  MessageSequence                8
  ReferencePilotPN               9
  ReferencePilotStrength         6
  ReferenceKeep                  1
  NumPilots                      4

  Total size of these fields     36 bits
                                  
  NumPilots occurences of the following fields

  PilotPNPhase                   15
  ChannelIncluded                1
  Channel                        0 or 24
  PilotStrength                  6
  Keep                           1

  Total size of each pilot record could be 23 or 47

  Additional fields in revB
  ATTotalPilotTransmissionIncluded   1
  ATTotalPilotTransmission           0 or 8
  ReferencePilotChannelIncluded      1
  ReferencePilotChannel              0 or 24

  Total size of revB fields could be : IDLE State : 2(1+1) 
                                       Connected State: 10(1+8+1+0) or 
                                                        34(1+8+1+24) 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Minimum size needed for RUP message.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdrrup_get_rup_msg_min_size(void)
{
  uint16 routeupdate_size;

  if ( hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
         == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
       || 
       hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
         == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */
     )
  {
    if (hdrrups_get_state() == HDRRUPS_CONNECTED)
    {
      /* We are not sure if ReferencePilotChannel(24 bits) is needed or not 
       * until slp calls rup to update revb fields of RU msg.  
       * It's possible that we need to pack ReferencePilotChannel info later.
       * To Simplify the design, instead of using 10 bits for the case of 
       * ReferencePilotChannelIncluded ==0 and 34 bits for  the case of 
       * ReferencePilotChannelIncluded == 1, alwasys assume ReferencePilotChannel 
       * field is needed (i.e. 34 bits are needed) 
       */ 
      routeupdate_size = HDRRUP_RUP_MSG_REF_PILOT_ONLY_SIZE + 34;
    }
    else
    {
      routeupdate_size = HDRRUP_RUP_MSG_REF_PILOT_ONLY_SIZE + 2;
    }
  }
  else
  {
    routeupdate_size = HDRRUP_RUP_MSG_REF_PILOT_ONLY_SIZE;
  }

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"min size: %d", routeupdate_size);

  return routeupdate_size;
} /* hdrrup_get_rup_msg_min_size() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_FORM_RUP_MSG

DESCRIPTION
  This function gets the pilot report from lmac, forms a route update message
  and packs it into a dsm item.

  Note that this function gets called from the HDRTX task when Idle (as part
  of forming an access channel message) and gets called from the mc task
  when connected. (As a result of receiving Pilot set changed indication).
  However, since the function cannot be called from both places 
  simultaneously, even though it uses global vairables, no inlocking is
  needed.
  
  The caller is expected to first call this function with NO_RESTRICTION, 
  and then call a second time with a size restriction if the first RUP 
  message is too big. If a size restriction is specified, the pilots in
  the RUP message are sorted in descending order of strength.  Otherwise
  they are not sorted.
  
  Following is the structure of Route update message
  Field                    Length ( bits )
  -----                    ---------------
  MessageID                      8
  MessageSequence                8
  ReferencePilotPN               9
  ReferencePilotStrength         6
  ReferenceKeep                  1
  NumPilots                      4

  Total size of these fields     36 bits
                                  
  NumPilots occurences of the following fields

  PilotPNPhase                   15
  ChannelIncluded                1
  Channel                        0 or 24
  PilotStrength                  6
  Keep                           1

  Total size of each pilot record could be 23 or 47
                                                           
  Additional fields in revB
  ATTotalPilotTransmissionIncluded   1
  ATTotalPilotTransmission           0 or 8
  ReferencePilotChannelIncluded      1
  ReferencePilotChannel              0 or 24
                                                           
  Total size of revB fields could be : IDLE State : 2(1+1) 
                                       Connected State: 10(1+8+1+0) or 
                                                        34(1+8+1+24) 

DEPENDENCIES
  None.

PARAMETERS
  rup_msg_build_type - specifies whether we are building a new route update 
                   message or rebuilding the same with reduced size.
  max_rup_msg_size - maximum allowed size for route update message in units
                     of bytes

RETURN VALUE
  Pointer to the dsm item containing the packed route update message.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL dsm_item_type *hdrrup_form_rup_msg 
( 
  hdrrup_rup_msg_build_type rup_msg_build_type, 
  uint16 max_rup_msg_size 
)
{
  dsm_item_type *rup_msg_dsm_ptr; 
    /* Pointer to Packed RUP msg in DSM item */

  int num_pilots;                            
    /* Number of Pilots in Report */

  uint8 cnt;                      /* Index */

  boolean has_off_freq_pilots = FALSE;    
                            /* Assume we don't have of pilots for logging. */

  boolean has_all_keep_zero_pilots = TRUE;  
                       /* Assume we have only keep = 0 pilots for logging. */

  uint16 routeupdate_size;            
    /* Route update message size in bits */

  uint32 max_rup_msg_bits = max_rup_msg_size*8;

  boolean is_same_chan; 
                /* Flag to identify if pilot is off-frequency pilot or not */

  hdrrups_state_enum_type rup_state;

  uint16 rup_subtype;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_LTE_TO_HDR_OH
  if (hdrsap_tunnel_mode_is_enabled(HDRHAI_ROUTE_UPDATE_PROTOCOL))
  {
    if( (num_pilots = hdrsap_get_pilots( 1+HDR_MAX_CSET_SIZE,
                       hdrrup.pilot_rpt, HDRHAI_ROUTE_UPDATE_PROTOCOL )) == 0 )
    {
      ERR("Could not get Pilot Report from LTE ML1", 0, 0, 0 );
      return NULL;
    }
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    if( (num_pilots = hdrlmac_get_pilots( HDRRUP_RUP_NUM_PILOTS_MAX, 
                       hdrrup.pilot_rpt, HDRHAI_ROUTE_UPDATE_PROTOCOL )) == 0 )
    {
      ERR("Could not get Pilot Report from SRCH", 0, 0, 0 );      
      return NULL;
    }
  }

  rex_enter_crit_sect(&hdrrup.crit_sect);

  rup_state = hdrrups_get_state();
  rup_subtype = hdrscp_get_current_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);

  /* If the calling function requests smaller size route update message, sort
   * the pilot report from LMAC based on strength so that the strongest pilots
   * are included */
  if ( max_rup_msg_size < HDRRUP_RUP_SIZE_NO_RESTRICTION )
  {
    (void) hdrrup_sort_pilotreport( num_pilots );
    HDR_MSG_PROT (MSG_LEGACY_HIGH, 
         " Requesting hdrrup_form_rup_msg for smaller route-update message" );
  }

  if ( rup_msg_build_type == HDRRUP_BUILD_NEW_RUP_MESSAGE )
  {
    hdrrup.rup_msg_seq_num++;
  }
  else /* HDRRUP_REBUILD_SAME_RUP_MESSAGE */
  {
    /* We are rebuilding the same route update message with reduced
     * size, so need not increment the sequence number */
  }

  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "HDR RUP: num_pilots returned by lmac=%d", 
                  num_pilots );

  ASSERT( num_pilots <= HDRRUP_RUP_NUM_PILOTS_MAX );

  hdrrup.rup_msg.msg_id = HDRRUP_RUP_MSG_ID ;
  hdrrup.rup_msg.msg_seq_num = hdrrup.rup_msg_seq_num;

  /*---------------------------------------------------------------------------
  The first pilot is the reference pilot
  ---------------------------------------------------------------------------*/
  hdrrup.rup_msg.ref_pilot_pn = hdrrup.pilot_rpt[0].link.pilot_pn;
  hdrrup.rup_msg.ref_pilot_strength = hdrrup.pilot_rpt[0].strength;
  hdrrup.rup_msg.ref_keep = hdrrup.pilot_rpt[0].keep;
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrrup.rup_msg.ref_pilot_arrival = (hdrrup.pilot_rpt[0].pn_phase  - 
                                      (int16)(hdrrup.pilot_rpt[0].link.pilot_pn * 64));
  hdrrup.rup_msg.ref_chan_rec.channel.chan_num =
                     hdrrup.pilot_rpt[0].link.channel.chan_num;
  hdrrup.rup_msg.ref_chan_rec.channel.band =
                 hdrrup.pilot_rpt[0].link.channel.band;
  hdrrup.rup_msg.ref_chan_rec.mode = (sys_hdr_chan_rec_mode_e_type)
                 hdrrup.pilot_rpt[0].system_type;
#endif /* FEATURE_LTE_TO_HDR_OH */

  if ( hdrrup.rup_msg.ref_keep != 0 )
  {

    /* The reference pilot has a keep that isn't zero. Keep track of 
       that so we can log it later. */
    has_all_keep_zero_pilots = FALSE;
  }

  routeupdate_size = hdrrup_get_rup_msg_min_size(); 

  /* Number of pilots returned by LMAC for route pilot report */
  hdrrup.rup_msg.num_pilots = (uint8)(num_pilots - 1);

  /*---------------------------------------------------------------------------
  We copy only till num_pilots-1,because the first pilot is the reference pilot
  ---------------------------------------------------------------------------*/
  for(cnt=0; cnt < (num_pilots-1); cnt++)
  {
    is_same_chan = SYS_CHAN_CMP_EQUAL( hdrrup.pilot_rpt[cnt+1].link.channel,
                                       hdrrup.pilot_rpt[0].link.channel );

    if ( ( rup_subtype == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
           || rup_subtype == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */ 
         ) && 
         ( rup_state == HDRRUPS_CONNECTED )
       )
    {
      /* Override is_same_chan variable, because we will pack 
       * pilot information later. For now, need to store pilot information
       */
      is_same_chan = FALSE;
    }

    /* Size of single pilot report is determined based on whether channel info
     * is included or not ( with channel it has 24 additional bits ) */
    routeupdate_size += is_same_chan ? 23 : 47;

    /*--------------------------------------------------------------------
    Check if there is enough space to add another pilot to the message
    ---------------------------------------------------------------------*/
    if ( routeupdate_size > max_rup_msg_bits )
    /* No more pilots can be added */
    {
      /* We need to cut this RUP message short.  Update the num_pilots field
       * to reflect how many we actually fit in */
      hdrrup.rup_msg.num_pilots = cnt;
      
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
        "HDR RUP: Last %d weakest pilots didn't fit for max size %d", 
                  ((num_pilots -1) - cnt), max_rup_msg_size );
      break;
    }
    else 
    {
      hdrrup.rup_msg.pilot[cnt].pn_phase = hdrrup.pilot_rpt[cnt+1].pn_phase;
      hdrrup.rup_msg.pilot[cnt].strength = hdrrup.pilot_rpt[cnt+1].strength;
      hdrrup.rup_msg.pilot[cnt].keep     = (uint8) hdrrup.pilot_rpt[cnt+1].keep;
  
      if ( hdrrup.rup_msg.pilot[cnt].keep != 0 )
      {
  
        /* It turns out we do have pilots without keep = 0.  Keep track
           of this so we can log that fact later. */
        has_all_keep_zero_pilots = FALSE;
      }
  
      /*----------------------------------------------------------------------
      The first element in the array (ref pilot) specifies the current channel.
      If the pilot is on the current channel, the channel is not included
      -----------------------------------------------------------------------*/
      if( is_same_chan )
      {
        hdrrup.rup_msg.pilot[cnt].chan_incl = FALSE;
      }
      else
      {
        hdrrup.rup_msg.pilot[cnt].chan_incl = TRUE;
        hdrrup.rup_msg.pilot[cnt].chan_rec.channel.chan_num =
                           hdrrup.pilot_rpt[cnt+1].link.channel.chan_num;
        hdrrup.rup_msg.pilot[cnt].chan_rec.channel.band =
                       hdrrup.pilot_rpt[cnt+1].link.channel.band;
        hdrrup.rup_msg.pilot[cnt].chan_rec.mode = (sys_hdr_chan_rec_mode_e_type)
                       hdrrup.pilot_rpt[cnt+1].system_type;
  
        /* It turns out we do have pilots without keep = 0.  Keep track
         * of this so we can log that fact later. 
         */
        has_off_freq_pilots = TRUE;
      }
    }
  }

  if ( has_off_freq_pilots )
  {
    /* Keep track of whether we had off frequency pilots for logging purposes. */
    hdrplog_inc_conn_state_stats_counter(
      HDRPLOG_RUP_WITH_OFS_NEIGHBORS_COUNT, 1 );
    }

  if ( has_all_keep_zero_pilots )
  {
    /* Keep track of whether we had all pilots with keep=0 for logging 
       purposes. */
    hdrplog_inc_conn_state_stats_counter(
      HDRPLOG_RUP_WITH_ZERO_KEEP_COUNT, 1 );
  }

  /* Keep track of the number of pilots in the RUP message.  The +1 is so that
     we include the reference pilot. */
  hdrplog_inc_conn_state_stats_counter(
    HDRPLOG_RUP_PILOT_COUNT, (uint16) ( hdrrup.rup_msg.num_pilots + 1 ) );

  HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, "HDR RUP: seq#=%d num=%d ref_pn=%d",
           hdrrup.rup_msg.msg_seq_num,
           hdrrup.rup_msg.num_pilots,
           (hdrrup.pilot_rpt[0].pn_phase + 32) / 64);
  
  rup_msg_dsm_ptr = hdrrup_pack_rup_msg( &hdrrup.rup_msg );
  
  rex_leave_crit_sect(&hdrrup.crit_sect);

  return rup_msg_dsm_ptr;
} /* hdrrup_form_rup_msg() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_ADD_TRAFFIC_MC_RUP_INFO

DESCRIPTION
  This function stores mc rup fields into local buffer. Data stored in buffer will
  be used to pack MC RUP msg later. 

DEPENDENCIES
  None.

PARAMETERS
  rup_msg_ptr - Unpacked route update message.
  msg_ptr - pointer to dsm item  holding RUP msg

RETURN VALUE
   buffer(hdrrup.mc_rup_info) in which unpacked route update information stored

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL hdrerrno_enum_type hdrrup_add_traffic_mc_rup_info
(
  hdrrup_rup_msg_type *rup_msg_ptr,
  void *msg_ptr
)
{
  uint8 buf_index;
  uint8 pilot_index;
  hdrerrno_enum_type result = E_FAILURE;

  for (buf_index = 0; buf_index < HDRRUP_RTC_RUP_MSG_BUFS_NUM; buf_index++)
  {
    /* Find a available buffer and store unpacked route update msg */
    if (hdrrup.mc_rup_info[buf_index].available)
    {
      hdrrup.mc_rup_info[buf_index].rup_seq_num = rup_msg_ptr->msg_seq_num;
      hdrrup.mc_rup_info[buf_index].num_pilots = rup_msg_ptr->num_pilots;

      for(pilot_index = 0; pilot_index < rup_msg_ptr->num_pilots; pilot_index++)
      {
        hdrrup.mc_rup_info[buf_index].pilot[pilot_index].pn_phase = rup_msg_ptr->pilot[pilot_index].pn_phase;
        hdrrup.mc_rup_info[buf_index].pilot[pilot_index].strength = rup_msg_ptr->pilot[pilot_index].strength;
        hdrrup.mc_rup_info[buf_index].pilot[pilot_index].keep = rup_msg_ptr->pilot[pilot_index].keep;
        hdrrup.mc_rup_info[buf_index].pilot[pilot_index].chan_rec.mode = 
                                              rup_msg_ptr->pilot[pilot_index].chan_rec.mode;
        hdrrup.mc_rup_info[buf_index].pilot[pilot_index].chan_rec.channel.band = 
                                              rup_msg_ptr->pilot[pilot_index].chan_rec.channel.band;
        hdrrup.mc_rup_info[buf_index].pilot[pilot_index].chan_rec.channel.chan_num = 
                                              rup_msg_ptr->pilot[pilot_index].chan_rec.channel.chan_num;
      }
      
      hdrrup.mc_rup_info[buf_index].chan_rec.mode = hdrrup.pilot_rpt[0].link.mode;
      hdrrup.mc_rup_info[buf_index].chan_rec.channel.band = hdrrup.pilot_rpt[0].link.channel.band;
      hdrrup.mc_rup_info[buf_index].chan_rec.channel.chan_num = hdrrup.pilot_rpt[0].link.channel.chan_num;

      /* For debug purpose only, to make sure we modify correct dsm item later*/
      hdrrup.mc_rup_info[buf_index].msg_ptr = msg_ptr;  
      /* Mark buffer unavailable */
      hdrrup.mc_rup_info[buf_index].available = FALSE;

      result = E_SUCCESS;
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Added rup seq# %d to mc rup info array ",
                      rup_msg_ptr->msg_seq_num);
      break;
    }
  }

  return result;
} /* hdrrup_add_traffic_mc_rup_info */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_PACK_RUP_MSG

DESCRIPTION
  This function takes a unpacked route update message, allocates a new dsm
  item and packs the route update message into this.

DEPENDENCIES
  None.

PARAMETERS
  rup_msg_ptr - Unpacked route update message.

RETURN VALUE
  Pointer to a dsm item conatining the packed route update message.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL dsm_item_type *hdrrup_pack_rup_msg
(
  hdrrup_rup_msg_type *rup_msg_ptr
)
{
  dsm_item_type *item_ptr = NULL;  /* Pointer to DSM item */
  int i;                           /* Index counter */
  uint16 offset = 0;   /* Offset of a field (in bits) from start of msg */

  hdrerrno_enum_type err_number;        /* Result of adding mc rup to buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  item_ptr = hdrutil_new_msg_buffer();

  /* Pack the route update message fields */
  HDRBIT_PACK8( item_ptr ,
                rup_msg_ptr->msg_id ,
                offset ,
                HDRRUP_RUP_MSG_ID_SIZE );
  offset += HDRRUP_RUP_MSG_ID_SIZE;

  HDRBIT_PACK8( item_ptr ,
                rup_msg_ptr->msg_seq_num ,
                offset ,
                HDRRUP_RUP_MSG_SEQ_SIZE );
  offset += HDRRUP_RUP_MSG_SEQ_SIZE;

  HDRBIT_PACK16( item_ptr ,
                 rup_msg_ptr->ref_pilot_pn ,
                 offset,
                 HDRRUP_RUP_REF_PILOT_PN_SIZE );
  offset += HDRRUP_RUP_REF_PILOT_PN_SIZE;

  HDRBIT_PACK8(  item_ptr ,
                 (uint8) rup_msg_ptr->ref_pilot_strength ,
                 offset ,
                 HDRRUP_RUP_REF_PILOT_STRENGTH_SIZE );
  offset += HDRRUP_RUP_REF_PILOT_STRENGTH_SIZE;

  HDRBIT_PACK8( item_ptr ,
                 rup_msg_ptr->ref_keep ,
                 offset,
                 HDRRUP_RUP_REF_KEEP_SIZE );
  offset += HDRRUP_RUP_REF_KEEP_SIZE;

  if ( ( hdrscp_get_current_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) 
           == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
         ||
         hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
           == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */ 
       ) &&
       ( hdrrups_get_state() == HDRRUPS_CONNECTED )
     )
  {
    /* We will add remaining fields when msg is queued to slp because we need to get
     * slp carrier infomation from RMAC */

    /* We will update the MC RUP fields later, so store following info into buffer
     * - rup msg sequence 
     * - pilot information  
     * - rup msg ptr  
     */
    HDR_MSG_PROT_1( MSG_LEGACY_LOW, "addr 0x%x", item_ptr); 
    err_number = hdrrup_add_traffic_mc_rup_info( rup_msg_ptr,
                                            item_ptr);

    /* buffer size is same as HDRHMP_RTC_MSG_BUFS_NUM, we should have enough 
     * space to store mc rup info*/
    if(err_number != E_SUCCESS)
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "RUP msg could not be formed. No available buffer !!");
      dsm_free_packet( &item_ptr );
      return NULL;
    }
  }
  else
  {
    HDRBIT_PACK8( item_ptr ,
                  rup_msg_ptr->num_pilots ,
                  offset ,
                  HDRRUP_RUP_NUM_PILOTS_SIZE );
    offset += HDRRUP_RUP_NUM_PILOTS_SIZE;

    for(i=0; i<rup_msg_ptr->num_pilots ; i++)
    {
      HDRBIT_PACK16( item_ptr ,
                     (uint16) rup_msg_ptr->pilot[i].pn_phase ,
                     offset ,
                     HDRRUP_RUP_PILOT_PN_PHASE_SIZE );
      offset += HDRRUP_RUP_PILOT_PN_PHASE_SIZE;

      if( rup_msg_ptr->pilot[i].chan_incl )
      {
        /* Pack Channel */
        HDRBIT_PACK8( item_ptr ,
                      (uint8) 1 ,
                      offset ,
                      HDRRUP_RUP_CHANNEL_INCLUDED_SIZE );
        offset += HDRRUP_RUP_CHANNEL_INCLUDED_SIZE;

        HDRBIT_PACK8( item_ptr ,
                      (uint8) rup_msg_ptr->pilot[i].chan_rec.mode ,
                      offset ,
                      HDRRUP_RUP_SYSTEM_TYPE_SIZE );
        offset += HDRRUP_RUP_SYSTEM_TYPE_SIZE;

        HDRBIT_PACK8( item_ptr ,
                      (uint8) rup_msg_ptr->pilot[i].chan_rec.channel.band ,
                      offset ,
                      HDRRUP_RUP_BAND_SIZE );
        offset += HDRRUP_RUP_BAND_SIZE;

        HDRBIT_PACK16( item_ptr ,
                       rup_msg_ptr->pilot[i].chan_rec.channel.chan_num ,
                       offset ,
                       HDRRUP_RUP_CHANNEL_NUMBER_SIZE );
        offset += HDRRUP_RUP_CHANNEL_NUMBER_SIZE;
      }
      else
      {
        HDRBIT_PACK8( item_ptr ,
                      (uint8) 0 ,
                      offset ,
                      HDRRUP_RUP_CHANNEL_INCLUDED_SIZE );
        offset += HDRRUP_RUP_CHANNEL_INCLUDED_SIZE;
      }

      HDRBIT_PACK8( item_ptr ,
                     (uint8) rup_msg_ptr->pilot[i].strength ,
                     offset ,
                     HDRRUP_RUP_PILOT_STRENGTH_SIZE );
      offset += HDRRUP_RUP_PILOT_STRENGTH_SIZE;

      HDRBIT_PACK8( item_ptr ,
                     rup_msg_ptr->pilot[i].keep ,
                     offset ,
                     HDRRUP_RUP_KEEP_SIZE );
      offset += HDRRUP_RUP_KEEP_SIZE;
    }

#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrsap_tunnel_mode_is_enabled(HDRHAI_ROUTE_UPDATE_PROTOCOL))
    {
      /* Pack ATTotalPilotTransmissionIncluded */
      HDRBIT_PACK8( item_ptr,
                    (uint8) 0,
                    offset ,
                    HDRRUP_RUP_PILOT_TX_PWR_INCL_SIZE );
      offset += HDRRUP_RUP_PILOT_TX_PWR_INCL_SIZE;

      /* Pack ReferencePilotChannelIncluded */
      HDRBIT_PACK8( item_ptr,
                    (uint8) 1,
                    offset ,
                    HDRRUP_RUP_REF_PILOT_CH_INCL_SIZE );
      offset += HDRRUP_RUP_REF_PILOT_CH_INCL_SIZE;

      HDRBIT_PACK8( item_ptr ,
                    (uint8) rup_msg_ptr->ref_chan_rec.mode ,
                    offset ,
                    HDRRUP_RUP_SYSTEM_TYPE_SIZE );
      offset += HDRRUP_RUP_SYSTEM_TYPE_SIZE;

      HDRBIT_PACK8( item_ptr ,
                    (uint8) rup_msg_ptr->ref_chan_rec.channel.band ,
                    offset ,
                    HDRRUP_RUP_BAND_SIZE );
      offset += HDRRUP_RUP_BAND_SIZE;

      HDRBIT_PACK16( item_ptr ,
                     rup_msg_ptr->ref_chan_rec.channel.chan_num ,
                     offset ,
                     HDRRUP_RUP_CHANNEL_NUMBER_SIZE );
      offset += HDRRUP_RUP_CHANNEL_NUMBER_SIZE;

      /* Pack ReferencePilotArrival */
      HDRBIT_PACK8( item_ptr,
                    (uint8) 1,
                    offset,
                    HDRRUP_RUP_REF_PILOT_ARRIVAL_INCL_SIZE );
      offset += HDRRUP_RUP_REF_PILOT_ARRIVAL_INCL_SIZE;

      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,"ReferencePilotArrival from LTE ML1= %d", 
                      rup_msg_ptr->ref_pilot_arrival);

      HDRBIT_PACK16( item_ptr,
                    rup_msg_ptr->ref_pilot_arrival,
                    offset,
                    HDRRUP_RUP_REF_PILOT_ARRIVAL_SIZE );
      offset += HDRRUP_RUP_REF_PILOT_ARRIVAL_SIZE;
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    if ( ( hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
             == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
           ||
           hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
             == HDRSCP_IRAT_MC_RUP
           ||
           hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
             == HDRSCP_IRAT_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */
         ) &&
        ( hdrrups_get_state() != HDRRUPS_CONNECTED )
       )
    {
      /* If RUP msg is sent on RTC, fields after ReferenceKeep will be pack 
       * after msg is queued to slp because we need to get slp carrier info
       * from RMAC then*/

      /* Pack ATTotalPilotTransmissionIncluded:
      * Standard: The access terminal shall set this field to '0' if 
      *           this message is sent on the access channel.
      */
      HDRBIT_PACK8( item_ptr,
                    (uint8) 0,
                    offset ,
                    HDRRUP_RUP_PILOT_TX_PWR_INCL_SIZE );
      offset += HDRRUP_RUP_PILOT_TX_PWR_INCL_SIZE;

      /* Pack ReferencePilotChannelIncluded:
       * In access channel, the ReferencePilotChannnel should be the FDD-paired 
       * forward CDMA channel associated with the reverse CDMA channel on which 
       * this RUP message is being sent. This field shall be set to '0'
       */
      HDRBIT_PACK8( item_ptr,
                    (uint8) 0,
                    offset ,
                    HDRRUP_RUP_REF_PILOT_CH_INCL_SIZE );
      offset += HDRRUP_RUP_REF_PILOT_CH_INCL_SIZE;
    }
  }

  return item_ptr;
} /* hdrrup_pack_rup_msg() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_GET_RUP_MSG

DESCRIPTION
  This function calls hdrrup_form_rup_msg to form the route update message
  and then adds the HMP header to the DSM pointer returned by the function.
  This function is externalized and called by SLP to send RUP msg on the
  Access channel.


    Note that the location of last reported RUP msg coordinates are update on
    receiving a ACMAC_TRANSMISSION_SUCCESS indication since every ACMAC
    transmission is accompanied by a RUP msg & successful transmission 
    ensures that e do not update the location if the transmission fails.
    
DEPENDENCIES
  None.

PARAMETERS
  rup_msg_build_type - specifies whether we are building a new route update 
                   message or rebuilding the same with reduced size.
  max_rup_msg_size - maximum allowed size for route update message in units
                     of bytes

RETURN VALUE
  Pointer to the dsm item containing the packed route update message.

SIDE EFFECTS
  None.
===========================================================================*/
dsm_item_type *hdrrup_get_rup_msg
( 
  hdrrup_rup_msg_build_type rup_msg_build_type, 
  uint16 max_rup_msg_size 
)
{
  dsm_item_type *item_ptr;       /* dsm item pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Form the route update message */
  item_ptr = hdrrup_form_rup_msg(rup_msg_build_type, max_rup_msg_size);
  if(item_ptr != NULL)  
    hdrhmp_add_header( HDRHAI_ROUTE_UPDATE_PROTOCOL, item_ptr );
  return item_ptr;
} /* hdrrup_get_rup_msg() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_PROCESS_OVHD_INFO_UPDATED_IND

DESCRIPTION
  This function processes the overhead information updated indication.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrup_process_ovhd_info_updated_ind( void )
{
  /* Location given by the most recent SP message */
  hdrovhd_sp_msg_location_type sp_msg_location;

  /* Nset info retrieved from the most recent SP message */
  hdrovhd_sp_msg_nset_info_type nset_info;

  /* Pilot GroupID retrieved from the most recent SP message */
  uint8 serving_pilot_gid;

  /* Traps any error codes */
  int err_number = 0;

  /* Current RUP state */
  hdrrups_state_enum_type rup_state = hdrrups_get_state();

  /* Link info of the current QC message */
  sys_link_type link_info;

  /* Current subnet's color code */
  uint8 ovhd_color_code = 0;

  /* Least significant 24 bits of current sector id */
  uint32 sector_id_24 = 0;

#ifdef FEATURE_HDR_TO_LTE
  hdraddr_type sector_id;
  uint8 subnet_mask = 0;
#endif /* FEATURE_HDR_TO_LTE */

  /* Number of sectors in the overhead cache */
  uint8 num_sectors;

  /* Sectors in cache */
  sys_link_type sector_list[HDROVHD_MAX_CACHE_RECORDS];

  /* Radius to use for distance based registration */
  int16 rup_radius = 0;

  /* RouteUpdateRadiusMultiply attribute */
  uint8 rm = hdrrupc_get_rup_rad_mult_attrib();

  /* RouteUpdateRadiusAdd attribute */
  int16 ra = hdrrupc_get_rup_rad_add_attrib();

  /* number of avoided channels returned by SD*/
  uint8 num_avoided_channels = 0;

  /* Current avoided channels returned by SD*/
  sys_avoided_channel_record_type avoided_chan_rec[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];       

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
  uint8 avoided_chan_idx; /* debug purpose only to print avoided channels*/
#endif /*FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize  local varariables */
  memset(avoided_chan_rec, 0, 
         (sizeof(sys_avoided_channel_record_type) * HDRSRCH_AVOIDED_CHAN_MAX_SIZE));

  if( rup_state == HDRRUPS_INACTIVE )
  {
    /*-------------------------------------------------------------------------
    If the Protocol is not in inactive state, do nothing.
    -------------------------------------------------------------------------*/
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                HDRIND_OVHD_INFO_UPDATED,
                                rup_state );
    return;
  }

  /*-------------------------------------------------------------------------
  Update the searcher with the ColorCode of the current link we are on
  -------------------------------------------------------------------------*/
  /* Retrieve the current link info for the QC message */
  if (hdrovhd_get_qc_link( &link_info, HDRHAI_ROUTE_UPDATE_PROTOCOL ) != E_SUCCESS)
  {
    ERR_FATAL("Can't retrieve Link Info",0,0,0);
  }

  /* Retrieve the current color code */
  if (hdrovhd_get_color_code(&ovhd_color_code, HDRHAI_ROUTE_UPDATE_PROTOCOL) != E_SUCCESS)
  {
    ERR_FATAL("Can't retrieve Color Code",0,0,0);
  }

  /* Inform the searcher */
  hdrlmac_color_code_update(link_info, ovhd_color_code);

#ifdef FEATURE_HDR_TO_LTE
  /* Update SectorID to searcher */
  if ( E_SUCCESS == 
         hdrovhd_get_sector_id(sector_id, &subnet_mask, HDRHAI_ROUTE_UPDATE_PROTOCOL) )
  { 
    hdrlmac_update_sector(sector_id);
  }
#endif /* FEATURE_HDR_TO_LTE */

  /*-------------------------------------------------------------------------
  Since the Neighborlist msg has priority over neighborlist from the Sector
  Parameters msg, process this indication only if no Neighborlist has been
  received. Note that the Neighborlist message is only considered in CONNECTED
  state.   
  -------------------------------------------------------------------------*/
  err_number |= hdrovhd_get_sp_msg_pilot_gid_and_nset_info(&serving_pilot_gid, 
                                                      &nset_info, 
    HDRHAI_ROUTE_UPDATE_PROTOCOL );

  err_number |= hdrovhd_get_sp_msg_location( &sp_msg_location, 
    HDRHAI_ROUTE_UPDATE_PROTOCOL );

  if( err_number != 0 )
  {
    ERR("RUP: Reading SP parameters Failed!", 0, 0, 0 );
    return;
  }

  num_sectors = hdrovhd_get_cached_sectors( sector_list,
                                            HDRHAI_ROUTE_UPDATE_PROTOCOL );

  /*-------------------------------------------------------------------------
  Prune invalid neighbors from nset (including unpaired channels).
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
    num_avoided_channels = hdrovhd_update_sys_avoid_list(
        HDRSRCH_AVOIDED_CHAN_MAX_SIZE,
        avoided_chan_rec);
#endif /*FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS*/

  if(num_avoided_channels > HDRSRCH_AVOIDED_CHAN_MAX_SIZE)
  {
    HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "num_avoided_channels is %d greater than "
                    "max allowed. limiting to max %d.", num_avoided_channels, 
                                                HDRSRCH_AVOIDED_CHAN_MAX_SIZE);
    num_avoided_channels = HDRSRCH_AVOIDED_CHAN_MAX_SIZE;
  }

  (void) hdrutil_remove_invalid_neighbors( nset_info.nset, &nset_info.num_neighbors,
                                    avoided_chan_rec, num_avoided_channels );

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
  for (avoided_chan_idx = 0; avoided_chan_idx < num_avoided_channels; avoided_chan_idx++ )
  {
    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, "current avoided channels: mode: %d, band: %d, chan: %d",
                    avoided_chan_rec[avoided_chan_idx].mode,
                    avoided_chan_rec[avoided_chan_idx].channel.band,
                    avoided_chan_rec[avoided_chan_idx].channel.chan_num
                    );
  }
#endif /*FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS*/

  /*-------------------------------------------------------------------------
  Update the searcher neighbor list.
  -------------------------------------------------------------------------*/
  hdrlmac_nset_update( nset_info.num_neighbors, nset_info.nset, 
                       nset_info.channel_count, nset_info.channel_list, 
                       num_sectors, sector_list,
                       num_avoided_channels, avoided_chan_rec,
                       HDRSRCH_NSET_UPDATE_BROADCAST, 
                       HDRHAI_ROUTE_UPDATE_PROTOCOL, 
                       serving_pilot_gid);

  /*-------------------------------------------------------------------------
  If the session is open an RUP is in idle state, check if the RUP message
  needs to be sent. If we have long (40 sec) sleep enabled, then do not send 
  a RUP nessage if the AT is currently doing long sleep.

  When MUM is enabled and maintain EVDO session is FALSE, AT shouldn't
  send the route update message for distance based registration. However
  we couldn't assume that when maintain EVDO session is FALSE, the session
  is closed. Because AT could be in a handoff within the same subnet,
  where the session is still open but maintain EVDO session is FALSE.
  -------------------------------------------------------------------------*/
  if( ( hdrsmp_session_is_open( HDRHAI_ROUTE_UPDATE_PROTOCOL ) ) &&
      ( hdrsmp_is_to_maintain_evdo_session() )                   &&
      ( rup_state == HDRRUPS_IDLE )
#ifdef FEATURE_HDR_LONG_SLEEP
      && ( hdridle_ready_for_long_sleep() == FALSE )
#endif /* FEATURE_HDR_LONG_SLEEP */
       
      )
  {

    /*-----------------------------------------------------------------------
    Check if we need to send route update message. If the RUP radius in the
    received SP msg is not zero, then the AN allows distance based
    registration & we should check if it is needed. If the distance travelled
    since the last route update message was sent is more than the previously
    received route update radius, then we need to send a new message.

    Update: If the computed distance is greater than the max(0, rm * ro + ra)
    then we will need to send a new message. In this case "ro" is represented
    by rup_radius.
    -----------------------------------------------------------------------*/
    /* Adjust the rup_radius by the configured attributes rm and ra:
       rup_radius = rm * rup_radius + ra */
    
    if( rm != HDRRUPC_DEFAULT_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_VAL )
    {
      rup_radius = ( ( hdrrup.location.rup_radius * rm) / 10 ) + ra;
    }
    else
    {
      /* Default value of rm will cause equation to be in the form:
          (hdrrup.location.rup_radius * 10 * 10 / 100) + ra
          
         This is reducible to: hdrrup.location.rup_radius + ra */
      rup_radius = hdrrup.location.rup_radius + ra;
    }
    

    /* rup_radius = max( 0, rup_radius ) */
    if( rup_radius < 0 )
    {
      rup_radius = 0;
    }
    
    if( ( sp_msg_location.rup_radius != 0 ) && ( rm != 0 ) &&
        ( ( hdrrup.location.reported == FALSE ) ||
          /* if the computed value of r is greater than max( 0, rm x ro + ra ) */
          ( reg_dist( hdrrup.location.latitude,
                      sp_msg_location.latitude,
                      hdrrup.location.longitude,
                      sp_msg_location.longitude,
                      rup_radius ) == TRUE ) ) )
    {
      /*---------------------------------------------------------------------
      Form a NULL message to be transmitted on access channel. Since SLP
      always first sends a route update message while transmitting on access
      channel, the NULL message causes the route update message to be sent.
      Since the message is being sent on Access Channel which is unreliable,
      a callback function is passed to HMP to determine the outcome of the
      transmission & resend if necessary.  Because this message is zero bytes
      it must be lowest priority or else PCP may to stop filling the probe 
      with other messages still queued for transmit that would fit in it.
      ---------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Sending distance based registration");
      if ( hdrovhd_get_sector_id_24(&sector_id_24, 
                                    HDRHAI_ROUTE_UPDATE_PROTOCOL) != E_SUCCESS)
      {
        /*------------------------------------------------------------------ 
        This case should never happen because distance based Route Update 
        occurs only when ovhd info has been updated. So there is valid sector 
        id with which AT is associated. hdrovhd_get_sector_id_24 should
        return E_SUCCESS above.
        -------------------------------------------------------------------*/

        ERR("RUP: Could not get current sectorId24 value", 0, 0, 0); 
      }
      
      HDRTRACE_RUP_DIST_BASED_REG(sector_id_24);

      hdrhmp_send_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                       HDRHAI_AC,
                       FALSE,
                       HDRRUP_NULL_MSG_PRIORITY,
                       HDRHAI_IN_USE, /* Instance of protocol sending msg */
                       NULL,
                       hdrrup_rup_msg_tx_outcome_cb,
                       NULL );
    }
  }
} /* hdrrup_process_ovhd_info_updated_ind() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_PROCESS_LINK_ACQUIRED_IND

DESCRIPTION
  This function processes the link acquired indication.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrup_process_link_acquired_ind( void )
{
  dsm_item_type *tcc_msg_dsm_ptr; /* Pointer to packed TCC in DSM item */
  hdrrups_state_enum_type rup_state = hdrrups_get_state();
                                     /* Current RUP state */

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrhai_tunnel_mode_type curr_tunnel_mode;
#endif /* FEATURE_LTE_TO_HDR_OH */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Process this indication only if RUP is in connection setup state.
  ---------------------------------------------------------------------------*/
  if( rup_state != HDRRUPS_CONNECTION_SETUP )
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                HDRIND_RTCMAC_LINK_ACQUIRED,
                                rup_state );
    return;
  }

  /* change state to connected */
  hdrrups_change_state( HDRRUPS_CONNECTED );

#ifdef FEATURE_LTE_TO_HDR_OH  
  curr_tunnel_mode = hdrsap_get_current_tunnel_mode_value (
                         HDRHAI_ROUTE_UPDATE_PROTOCOL); 
  if ( (curr_tunnel_mode == HDRHAI_TUNNEL_MODE_DISABLED) ||
       (curr_tunnel_mode == HDRHAI_TUNNEL_MODE_ACTIVE_HO) )
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
    {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
           "IRAT: LTE->HDR traffic ho success!!!, auto activation ind sent to MMOC");
        mmoc_rpt_prot_auto_actd_ind( PROT_ACT_GWL_REDIR, PROT_STATE_ONLINE_HDR );
#ifdef FEATURE_HDR_DEBUG_INFO
		hdrmc_log_sent_rpt(0, HDR_MMOC_RPT_PROT_AUTO_ACTD_IND);
#endif /* FEATURE_HDR_DEBUG_INFO */
        hdrsap_set_tunnel_mode ( HDRHAI_TUNNEL_MODE_DISABLED );

        /* Set to high prio traffic. This call must be made after
           SRCH is in traffic. Effect of this call does not persist
           after SRCH transition out of TC state. */
        hdrrup_set_tuneaway_allowed( FALSE );
    }
#endif /* FEATURE_LTE_TO_HDR_OH */

    if (hdrcon_connection_is_closing(HDRHAI_ROUTE_UPDATE_PROTOCOL))
    {
      /*-------------------------------------------------------------------------
      The Protocol is in connection setup state, but the connection is closing.
      If we sent a message now it would never make it out on the RTC, but 
      HMP would re-open the connection just to send it, which would be bad.
      -------------------------------------------------------------------------*/
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Not sending TCComplete, Con is closing");
    }
    else
    {
      /* Form the Traffic channel complete msg using the last rcvd TCA msg */
      tcc_msg_dsm_ptr = hdrrup_form_tcc_msg();
                         
      /*-----------------------------------------------------------------------
         Send the traffic channel complete msg on the reverse traffic channel.
         It is still possible that the connection will be closing before HMP 
         gets a chance to give the message to SLP for transmit.  In this case,
         the message will be discarded by HMP who will not attempt to open a
         new connection just to send this message
      -----------------------------------------------------------------------*/
      hdrhmp_send_msg_on_existing_rtc( 
        HDRHAI_ROUTE_UPDATE_PROTOCOL,
        TRUE,
        HDRRUP_TCC_MSG_PRIORITY,
        HDRHAI_IN_USE, /* Instance of protocol sending message */
        tcc_msg_dsm_ptr,
        hdrrup_tcc_msg_tx_outcome_cb,
        NULL );
    }
  }

} /* hdrrup_process_link_acquired_ind() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_FORM_TCC_MSG

DESCRIPTION
  This function forms a traffic channel complete message and packs it into a
  dsm item.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Pointer to the dsm item containing the packed traffic channel complete
  message.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL dsm_item_type * hdrrup_form_tcc_msg( void )
{
  dsm_item_type *item_ptr;  /* Pointer to DSM item */
  uint16 offset = 0;        /* Offset (in bits) of field from start of msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  item_ptr = hdrutil_new_msg_buffer();
  HDRBIT_PACK8( item_ptr, HDRRUP_TCC_MSG_ID, offset, HDRRUP_TCC_MSG_ID_SIZE );
  offset += HDRRUP_TCC_MSG_ID_SIZE;
  HDRBIT_PACK8(item_ptr, hdrrup.last_tca_msg_seq, offset, HDRRUP_TCC_MSG_SEQ_SIZE);

  return item_ptr;
} /* hdrrup_form_tcc_msg() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_TCC_MSG_TX_OUTCOME_CB

DESCRIPTION
  This is the callback function that SLP calls with the status for the
  transmission of a TrafficChannelComplete message.

DEPENDENCIES
  None.

PARAMETERS
  data_ptr    - pointer to user data.
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrup_tcc_msg_tx_outcome_cb
(
  void          *data_ptr,
  hdrerrno_enum_type  status
)
{
  hdrrups_state_enum_type cur_state = hdrrups_get_state();
                                   /* Current RUP state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT ( data_ptr == NULL );
  
  hdrplog_end_conn_attempt(HDRPLOG_CONN_ATT_SUCCESS);
  hdrlmac_tc_established(HDRHAI_ROUTE_UPDATE_PROTOCOL);

  /* Keep track of the number of Traffic Channel Complete messags for logging
     purposes. */  
  hdrplog_inc_conn_state_stats_counter( HDRPLOG_TCC_MESSAGE_COUNT, 1 );

  if (status == E_SUCCESS)
  {
    HDRTRACE_TXMSG_ACKED(HDRHAI_ROUTE_UPDATE_PROTOCOL,
      HDRRUP_TCC_MSG_ID, cur_state); /*lint !e506 */
  }
  else
  {
    HDRTRACE_TXMSG_FAILED(HDRHAI_ROUTE_UPDATE_PROTOCOL,
      HDRRUP_TCC_MSG_ID, cur_state); /*lint !e506 */
  }
} /* hdrrup_tcc_msg_tx_outcome_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_PROCESS_MSG

DESCRIPTION
  This function processes messages for RUP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrup_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */
  boolean free_the_dsm_item = TRUE; /* If we should free the DSM item */  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG));

  /*---------------------------------------------------------------------------
  Extract the first byte of the received message. This is the message ID.
  ---------------------------------------------------------------------------*/
  if( HDRRUP_MSG_ID( item_ptr, &msg_id ) != E_SUCCESS )
  {
    ERR( "Could not extract msg_id", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /*---------------------------------------------------------------------------
  Ignore packets that were decoded while in connected state, but would be
  processed in idle.  Note that if a ConnectionClose/TCA bundle is received,
  the TRAFFIC bit is set to FALSE to simulate the message being received 
  while idle.
  Ignore messages received when the session is not opened 
  ---------------------------------------------------------------------------*/
  if ( (HDRDSM_GET_TRAFFIC_BIT(item_ptr) &&
        !hdrrup_is_tc_assigned(HDRHAI_ROUTE_UPDATE_PROTOCOL)) ||
       !hdrsmp_session_is_open(HDRHAI_ROUTE_UPDATE_PROTOCOL) )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH,
                    "RUP: Ignore message (id: %d state: %d)",
                    msg_id,hdrrups_get_state());
    HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL, msg_id,
                                  hdrrups_get_state() );

    if (msg_id == HDRRUP_TCA_MSG_ID) 
    {
      /* hdrrup.tca_count is accessed by HDRMC task and HDRRX task.
         HDRRX task only increment the count. It is safe  not to 
         INTLOCK here, in the sense tca_count-- will not become
         negative if HDRRX preempts after check tca_count == 0 */

      if( hdrrup.tca_count == 0 )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "TCA count out of sync:0!");
      }
      else
      {
        hdrrup.tca_count--;
      }
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                      "TCA Ignored, %d TCA msg(s) still on queue", 
                      hdrrup.tca_count );
    }

    dsm_free_packet( &item_ptr );
    return;
  }

  HDRTRACE_MSG( HDRHAI_ROUTE_UPDATE_PROTOCOL, msg_id, hdrrups_get_state() );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  if (protocol_instance == HDRHAI_IN_USE)
  {
    switch( msg_id )
    {
      case HDRRUP_RR_MSG_ID:
        /*-----------------------------------------------------------------------
        ResetReport message
        -----------------------------------------------------------------------*/
        HDR_MSG_PROT (MSG_LEGACY_HIGH, "Reset report message received");
        hdrrup_process_rr_msg();
        break;

      case HDRRUP_TCA_MSG_ID:
        /*-----------------------------------------------------------------------
        TrafficChannelAssignment message
        -----------------------------------------------------------------------*/
        if( hdrrup.tca_count == 0 )
        {
          HDR_MSG_PROT( MSG_LEGACY_ERROR, "TCA out of sync:0!" );
        }
        else
        {
          /* If we decrement the TCA count and if the count is still greater 
             than 0 we have multiple TCAs. We will need to skip this TCA and 
             move onto the next before we actually process the message */
          hdrrup.tca_count--;
          /* if HDRRX task preempts here and increment counter, this TCA will
             not be processed, but next TCA will be. Still safe without 
             INTLOCK() */
          if( hdrrup.tca_count == 0 )
          {
            /* Process this TCA */
            HDR_MSG_PROT( MSG_LEGACY_HIGH, "Processing TCA..." );
          }
          else
          {
            HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
              "Ignoring TCA; %d TCA msg(s) still on queue", hdrrup.tca_count );

            break; /* Skip to the end of the function and avoid processing TCA */
          }
        }
          
        if (hdrcon_connection_is_closing( HDRHAI_ROUTE_UPDATE_PROTOCOL ))
        {
          if ( hdrcon_hho_is_in_progress() )
          {
            /* We need to wait until we have closed the current connection
             * before we open a new one, so hold onto this message for a little
             * while rather than processing it now.
             */
            free_the_dsm_item = FALSE;
            hdrrup.pending_tca_msg_ptr = item_ptr;
            HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                          "Delaying TCA until connection is closed");
          }
          else
          {
            /* Connection is closing and it's not for HHO case (TCA is not bundled 
             * with ConnectionClose), ignore TCA.
             */
            HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                          "RUP: Connection is closing, Ignore TCA message");
            HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL, msg_id,
                                          hdrrups_get_state() );
          }
        }
        else
        {
          hdrrup_process_tca_msg( item_ptr );
        }
        break;

      case HDRRUP_NL_MSG_ID:
        /*-----------------------------------------------------------------------
        Neighborlist message
        -----------------------------------------------------------------------*/
        HDR_MSG_PROT (MSG_LEGACY_HIGH, "Neighbor list message received");
        hdrrup_process_nl_msg( item_ptr );
        break;

      case HDRRUPC_AO_MSG_ID:
        /*-----------------------------------------------------------------------
        Attribute Override message
        -----------------------------------------------------------------------*/
        if ( hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
               == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
             || 
             hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
               == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */
           )
        {
          /* AttributeOverride message is removed in MC RUP */
          HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "MC RUP: Unknown message id (%d) received", msg_id);
        }
        else
        {
          hdrrupc_process_msg( item_ptr, msg_id );
        }
        break;

      case HDRRUPC_ATTRIB_UPDATE_REQ_MSG_ID:
        /*-----------------------------------------------------------------------
        AN-initiated GAUP message
        -----------------------------------------------------------------------*/
        hdrrupc_process_msg( item_ptr, msg_id );
        break;

      case HDRRUP_RUP_REQ_MSG_ID:
        /*-----------------------------------------------------------------------
        RouteUpdateRequest message
        -----------------------------------------------------------------------*/
        if ( item_ptr != hdrrup.rup_req.pending_req )
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                        "RouteUpdateRequest message receieved" );

          /* If we are waiting for Search's pilot report for
           * a previous RUP Request message, save this current
           * message in the buffer */
          if ( rex_get_timer(&hdrrup.rup_req.pilot_rpt_timer) != 0)
          {
            /* If there are already existing pending RUP request
             * message, free it */
            if ( hdrrup.rup_req.pending_req != NULL )
            {
              HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                           "Free the previous pending RUP request");
              dsm_free_packet( &hdrrup.rup_req.pending_req );
            }

            /* Save the current RUP request message, 
             * and don't free it */
            hdrrup.rup_req.pending_req = item_ptr;
            free_the_dsm_item = FALSE;

            HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                         "Wait for pilot rpt, save pending RUP Req msg");
          }
          else
          {
            hdrrup_process_rup_req_msg( item_ptr );
          }
        }
        else
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                        "Process pending RouteUpdateRequest message" );

          if(( rex_get_timer(&hdrrup.rup_req.pilot_rpt_timer) != 0 ))
          {
            ERR_FATAL( "RUP Pilot rpt timer already running", 0, 0, 0 );
          }

          hdrrup_process_rup_req_msg( item_ptr );
        }
        break;
        
      default:
        /*-----------------------------------------------------------------------
        Default: message not recognized.
        -----------------------------------------------------------------------*/
        ERR("RUP: Unknown in use msg %d", msg_id, 0, 0 );
        break;
    }
  }
  else /* (protocol_instance == HDRHAI_IN_CONFIG) */
  {
    switch( msg_id )
    {
      case HDRRUPC_CON_REQ_MSG_ID:
        /*-----------------------------------------------------------------------
        Configuration Request message
        -----------------------------------------------------------------------*/
        hdrrupc_process_msg( item_ptr, msg_id );
        break;

      case HDRRUPC_CON_RSP_MSG_ID:
        /*-----------------------------------------------------------------------
        Configuration Response message
        -----------------------------------------------------------------------*/
        hdrscm_process_msg(item_ptr, HDRHAI_IN_CONFIG);
        /* RUP configuration response message is handled by SCM and is freed by 
           hdrscm_process_msg */
        free_the_dsm_item = FALSE;
        break;

      default:
        /*-----------------------------------------------------------------------
        Default: message not recognized.
        -----------------------------------------------------------------------*/
        ERR("RUP: Unknown in config msg %d", msg_id, 0, 0 );
        break;
    }
  }
  /*---------------------------------------------------------------------------
  Finished processing message. Free the dsm item containing the message.
  ---------------------------------------------------------------------------*/
  if (free_the_dsm_item)
  {
    dsm_free_packet( &item_ptr );
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

} /* hdrrup_process_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_CREATE_SUBACTIVE_SET_LIST

DESCRIPTION
  This function is used internally to generate sub-active set list based on 
  unpacked revB TCA. Generated sub-active set list will be used to check if 
  received TCA is valid.

DEPENDENCIES
  None

PARAMETERS
  subactive_set_list_ptr - Location to store created subactive set list 
  saset_size - maximum number of sub-active set allowed in the TCA 
  tca_msg_ptr - pointer to unpacked MC TCA structure in static memory

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrrup_create_subactive_set_list
(
  hdrsrch_subactive_set_type *subactive_set_list_ptr,
  uint8 saset_size,
  const hdrrup_tca_msg_type *tca_msg_ptr
)
{
  int subaset_id;
  int cnt;        /* Loop counter  */
  int num_pilots; /* Number of pilots in subactive set*/
  hdrsrch_aset_pilot_type aset_list[HDRSRCH_ASET_MAX_SIZE];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize aset_list */
  memset( aset_list, 0, sizeof(hdrsrch_aset_pilot_type) * HDRSRCH_ASET_MAX_SIZE );

  /* Number of sub-active sets in the TCA shouldn't greater than maximum number of sub-active set*/
  ASSERT(tca_msg_ptr->num_sub_asets <= saset_size);

  for (subaset_id = 0; subaset_id < tca_msg_ptr->num_sub_asets; subaset_id++)
  {
    /* Initialize num_pilots for each sub-active set*/
    num_pilots = 0;

    /* Forward link CDMA channels associated with this sub-active set..
     * Should be a list of channels, but AT only supports one channel (K=1) per sub-active 
     * set for revB (for phase1 only?), use the first element in fwd_channel array 
     */
    subactive_set_list_ptr[subaset_id].channel.band = 
      tca_msg_ptr->sub_aset[subaset_id].fwd_channel[0].channel.band;

    subactive_set_list_ptr[subaset_id].channel.chan_num = 
      tca_msg_ptr->sub_aset[subaset_id].fwd_channel[0].channel.chan_num;

    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, "sub-active set: %d, band: %d, channel : %d",
                     subaset_id,
                     subactive_set_list_ptr[subaset_id].channel.band,
                     subactive_set_list_ptr[subaset_id].channel.chan_num);

    /* Whether this subactive set should not be reported. */
    subactive_set_list_ptr[subaset_id].not_reportable = 
      tca_msg_ptr->sub_aset[subaset_id].not_reportable;

    /* Number of pilots in this subactive set */
    for (cnt = 0; cnt < tca_msg_ptr->num_sectors; cnt++)
    {
      if (tca_msg_ptr->sub_aset[subaset_id].sector_params[cnt].pilot_included)
      {
        /* pilot_pn*/
        aset_list[num_pilots].pilot_pn = tca_msg_ptr->sector_info[cnt].pilot_pn;

        /* softer_handoff*/
        aset_list[num_pilots].softer_handoff = tca_msg_ptr->sector_info[cnt].softer_handoff;

        /* mac_index */
        aset_list[num_pilots].mac_index = 
          tca_msg_ptr->sub_aset[subaset_id].sector_params[cnt].ftc_params[0].ftc_mac_index;

        /* drc_cover*/
        aset_list[num_pilots].drc_cover = tca_msg_ptr->sector_info[cnt].drc_cover;

        /* rl_mac_index :
         * Use the first element of rtc_params because we only supporte one rev ch (for phase1?)
         */
        aset_list[num_pilots].rl_mac_index = 
          tca_msg_ptr->sub_aset[subaset_id].sector_params[cnt].rtc_params[0].rev_mac_index;

        /* rab_mac_index*/
        aset_list[num_pilots].rab_mac_index = 
          tca_msg_ptr->sub_aset[subaset_id].sector_params[cnt].rtc_params[0].rab_mac_index;

        /* delta_t2p*/
        aset_list[num_pilots].delta_t2p = 
          tca_msg_ptr->sub_aset[subaset_id].sector_params[cnt].rtc_params[0].delta_t2p;

        /* ra_channel_gain*/
        aset_list[num_pilots].ra_channel_gain = tca_msg_ptr->sector_info[cnt].ra_channel_gain;

        /* dsc*/
        aset_list[num_pilots].dsc = tca_msg_ptr->sector_info[cnt].dsc;

        /* pilot_group_id*/
        aset_list[num_pilots].pilot_group_id = 
          tca_msg_ptr->sub_aset[subaset_id].sector_params[cnt].pilot_group_id;

        /* scheduler_tag*/
        aset_list[num_pilots].scheduler_tag = 
          tca_msg_ptr->sub_aset[subaset_id].sector_params[cnt].scheduler_tag;

        num_pilots++;
      }
    }

    subactive_set_list_ptr[subaset_id].num_aset_pilots = num_pilots;

    /* Copy aset_list */
    for (cnt = 0; cnt < num_pilots; cnt++ )
    {
      subactive_set_list_ptr[subaset_id].aset_list[cnt] = aset_list[cnt];
    }
  }
} /* hdrrup_create_subactive_set_list*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_SET_MC_TCA_REV_CHAN_ORDER

DESCRIPTION
   This function is used to set the rev channel in descending order 

DEPENDENCIES
  MC TCA should be parsed already .

PARAMETERS
  None

RETURN VALUE
  E_SUCCESS  - Rev channels in ascending order have been set successfully 
  E_VERSION_MISMATCH  - TCA is NOT compliant with revB FDD

SIDE EFFECTS
  None

===========================================================================*/
LOCAL hdrerrno_enum_type hdrrup_set_mc_tca_rev_chan_order(void)
{
  uint32 tca_chan_freq_list[ HDRRUP_TCA_REV_CH_MAX_SIZE]; /* To store ch(khz)*/
  uint8 rev_chan_index = 0 ;  
    /* index used to store channel to tca_chan_freq_list[]*/

  uint32 temp_ch; /* used to sort tca_chan_freq_list*/
  uint8 saset_id; /* loop counter */
  uint8 chan_id;  /* loop counter */
  uint8 chan_id2; /* loop counter*/   
  boolean order_set = FALSE;
  hdrerrno_enum_type result = E_SUCCESS; /* Return result */
  sys_channel_type rev_channel;

/* ------------------------------------------------------------------------*/

  for (saset_id = 0; saset_id < hdrrup.tca_msg.num_sub_asets; saset_id++)
  {
    if (result != E_SUCCESS )
    {
      break;
    }

    if (hdrrup.tca_msg.sub_aset[saset_id].num_rev_channels > HDRRUP_REV_CH_PER_SUBASET_MAX_SIZE)
    {
        HDR_MSG_PROT_3 (MSG_LEGACY_ERROR, "num of rev chan %d in subaset %d is greater than max number %d ",
                        hdrrup.tca_msg.sub_aset[saset_id].num_rev_channels, saset_id, HDRRUP_REV_CH_PER_SUBASET_MAX_SIZE);
        result = E_VERSION_MISMATCH;
        break;
    }	  
	  
    for (chan_id = 0; (chan_id < hdrrup.tca_msg.sub_aset[saset_id].num_rev_channels)&&(chan_id < HDRRUP_REV_CH_PER_SUBASET_MAX_SIZE); chan_id++)    
    {
      if (rev_chan_index >= HDRRUP_TCA_REV_CH_MAX_SIZE)
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "num of rev chan is greater than max number %d ",
                        HDRRUP_TCA_FWD_CH_MAX_SIZE);
        result = E_VERSION_MISMATCH;
        break;
      }

      rev_channel = hdrrup.tca_msg.sub_aset[saset_id].rev_channel[chan_id].channel;

      /* Convert rev channels into frequency (KHz)*/
      tca_chan_freq_list[rev_chan_index] = 
        hdrlmac_get_tx_carrier_freq(rev_channel.band, rev_channel.chan_num); 

        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "DEBUG: freq:  %d Khz",
                        tca_chan_freq_list[rev_chan_index]);
        rev_chan_index++;
    }
  }

  if (result != E_SUCCESS )
  {
    /* Number of rev channels is incorrect*/
    return result;
  }

  /* Sorting frequencies in ascending order*/
  for (chan_id = 1; chan_id < rev_chan_index; chan_id++)
  {
    temp_ch = tca_chan_freq_list[chan_id];
    chan_id2 = chan_id;
    while ((chan_id2 > 0) && (tca_chan_freq_list[chan_id2 - 1] > temp_ch))
    {
      tca_chan_freq_list[chan_id2] = tca_chan_freq_list[chan_id2 - 1];
      chan_id2--;
    }
    tca_chan_freq_list[chan_id2] = temp_ch;
  }

  /* Set the ascending order for each rev channel of every sub active set:
   * The tca_chan_freq_list[] is sorted (element 0 has lowest frequency,  element 
   *"rev_chan_index -1" has highest freqyency),
   * now go through all channels one by one and match the rev channel frequency 
   * of each sub active to set the ascending order(a_order)
   * */
  for (chan_id = 0; chan_id < rev_chan_index; chan_id++)
  {
    order_set = FALSE;

    for (saset_id = 0; saset_id < hdrrup.tca_msg.num_sub_asets; saset_id++)
    {
      if (order_set)
      {
        break;
      }

      for (chan_id2 = 0; chan_id2 < hdrrup.tca_msg.sub_aset[saset_id].num_rev_channels; chan_id2++)
      {
        if (order_set)
        {
          break;
        }

        rev_channel = hdrrup.tca_msg.sub_aset[saset_id].rev_channel[chan_id2].channel;
        if(hdrlmac_get_tx_carrier_freq( rev_channel.band, rev_channel.chan_num) ==
           tca_chan_freq_list[chan_id])
        {
          /* set the ascending order( from 1 to "rev_chan_index" )*/
          hdrrup.tca_msg.sub_aset[saset_id].rev_channel[chan_id2].a_order = chan_id + 1;
          order_set = TRUE;
        }
      }
    }
  }

  return result;

} /* hdrrup_set_mc_tca_rev_chan_order() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_CHECK_MC_FREQUENCY_BDWIDTH

DESCRIPTION
   This function is used to check if channel frequency span is within valid 
   range.

DEPENDENCIES
  tca_msg_ptr should point to valid memory.

PARAMETERS
  None

RETURN VALUE
  E_SUCCESS  - frequency span is within valid range
  E_PROT_UNKN_MSG - TCA is not compliant with standard
  E_VERSION_MISMATCH  - TCA is NOT compliant with revB FDD

SIDE EFFECTS
  None

===========================================================================*/
LOCAL hdrerrno_enum_type hdrrup_check_mc_frequency_bdwidth (void)
{
  hdrerrno_enum_type check_result = E_SUCCESS; /* Return result */
  uint16 neg_fl_bd_no_jammer;   /* MaxForwardLinkBandwidthNoJammer the AT can support*/
  uint32 max_fl_bd_no_jammer_in_khz; 
  uint16 neg_fl_bd_jammer;      /* MaxForwardLinkBandwidthWithJammer the AT can support*/
  uint32 max_fl_bd_jammer_in_khz; 
  uint32 tca_chan_freq_list[ HDRRUP_TCA_FWD_CH_MAX_SIZE]; /* To store ch(khz)*/
  uint8 fwd_chan_index = 0 ;  /* used to store channel to tca_chan_freq_list[]*/

  uint32 temp_ch; /* used to sort tca_chan_freq_list*/
  uint8 saset_id; /* loop counter */
  uint8 chan_id;  /* loop counter */
  uint8 chan_id2; /* loop counter*/   
/* ------------------------------------------------------------------------*/

  if (hdrrup.tca_msg.num_sub_asets > HDRSRCH_MAX_NUM_SUBASETS )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid num_sub_asets %d ", 
                   hdrrup.tca_msg.num_sub_asets);
    return E_VERSION_MISMATCH;
  }

  /* We check the maximum separation between the highest and lowest fwd link 
   * CDMA channels against the value of MaxForwardLinkBandwidth(No/With)Jammer 
   * attribute supported by the AT instead of negotiated value because it is 
   * "AT's capability" attribute, even the AN rejects, the AT should still use 
   * supported value internally 
   */
  /* In phase1, fl_bd_no_jammer, fl_bd_jammer and rl_bd should have same value
   * we check fl_bd_no_jammer and fl_bd_jammer in phase1 (TODO: check rl_bd if 
   * it is different than fl_bd in later phase)
   */
  neg_fl_bd_no_jammer = HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_NO_JAMMER;
  neg_fl_bd_jammer = HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_W_JAMMER;

  if ((hdrrup.tca_msg.num_sub_asets == 1) && 
      (hdrrup.tca_msg.sub_aset[0].num_fwd_channels == 1))
  {
    /* Only have one channel, do not need to check*/
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "Only one saset w/ one Fwd Channel");
    return E_SUCCESS;
  }

  /* convert to unit of KHz*/
  max_fl_bd_no_jammer_in_khz = HDRRUP_CONVERT_NEGOTIATED_BANDWIDTH_TO_KHZ(neg_fl_bd_no_jammer);
  max_fl_bd_jammer_in_khz = HDRRUP_CONVERT_NEGOTIATED_BANDWIDTH_TO_KHZ(neg_fl_bd_jammer);

  do
  {
    if (hdrrup.tca_msg.chan_incl)
    {
      /* Checking frequency span now
       * At this point we know all sub-active sets have same band class 
       */
      for (saset_id = 0; saset_id < hdrrup.tca_msg.num_sub_asets; saset_id++)
      {
        if (check_result != E_SUCCESS )
        {
          break;
        }

        if (hdrrup.tca_msg.sub_aset[saset_id].num_fwd_channels > HDRRUP_FWD_CH_PER_SUBASET_MAX_SIZE)
        {
			HDR_MSG_PROT_3 (MSG_LEGACY_ERROR, "num of fwd chan %d in subaset %d is greater than max number %d ",
							hdrrup.tca_msg.sub_aset[saset_id].num_fwd_channels, saset_id, HDRRUP_FWD_CH_PER_SUBASET_MAX_SIZE);
			check_result = E_VERSION_MISMATCH;
			break;
		}	  
		  

        for (chan_id = 0; (chan_id < hdrrup.tca_msg.sub_aset[saset_id].num_fwd_channels) && (chan_id < HDRRUP_FWD_CH_PER_SUBASET_MAX_SIZE); chan_id++)                
        {
          if (fwd_chan_index >= HDRRUP_TCA_FWD_CH_MAX_SIZE)
          {
            HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "num of fwd chan: %d is greater than max number %d ",
                            fwd_chan_index + 1, HDRRUP_TCA_FWD_CH_MAX_SIZE);
            check_result = E_VERSION_MISMATCH;
            break;
          }

          /* Store all channels into array (in KHZ)*/
          tca_chan_freq_list[fwd_chan_index] = 
            hdrlmac_get_rx_carrier_freq( hdrrup.tca_msg.sub_aset[saset_id].fwd_channel[chan_id].channel.band,
                                         hdrrup.tca_msg.sub_aset[saset_id].fwd_channel[chan_id].channel.chan_num);

            HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "DEBUG: freq:  %d Khz", tca_chan_freq_list[fwd_chan_index]);
            fwd_chan_index++;
        }
      }

      if (check_result != E_SUCCESS )
      {
        /* Number of fwd channels is incorrect*/
        break;
      }

      /* At this point, We should have at least two channels in the tca_chan_freq_list */ 
      ASSERT((fwd_chan_index > 1) &&
            (fwd_chan_index <= HDRRUP_TCA_FWD_CH_MAX_SIZE ));

      /* Sorting frequencies in ascending order*/
      for (chan_id = 1; chan_id < fwd_chan_index; chan_id++)
      {
        temp_ch = tca_chan_freq_list[chan_id];
        chan_id2 = chan_id;
        while ((chan_id2 > 0) && (tca_chan_freq_list[chan_id2 - 1] > temp_ch))
        {
          tca_chan_freq_list[chan_id2] = tca_chan_freq_list[chan_id2 - 1];
          chan_id2--;
        }
        tca_chan_freq_list[chan_id2] = temp_ch;
      }

      /* Channel separation between the highest and the lowest channels should be 
       * less than or equal to  max_fl_bd_no_jammer_in_khz and max_fl_bd_jammer_in_khz
       */
      if (((tca_chan_freq_list[fwd_chan_index - 1]  - tca_chan_freq_list[0]) > 
          max_fl_bd_no_jammer_in_khz) ||
          ((tca_chan_freq_list[fwd_chan_index - 1] - tca_chan_freq_list[0]) >
           max_fl_bd_jammer_in_khz) )
      {
        HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "bandwidth span %d (Khz) is greater than %d (Khz)",
                        tca_chan_freq_list[fwd_chan_index - 1] - tca_chan_freq_list[0],
                        max_fl_bd_no_jammer_in_khz);
        check_result =  E_PROT_UNKN_MSG;
        break; 
      }
    }
    else
    {
      /* If AssignedChannelIncluded is set to '0', then the channel that carries 
       * TCA is the channel that the AN is assigning to the AT.
       * i.e. same channel for all subactive sets 
       * E_SUCCESS case !
       */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "AssignedChannelIncluded == 0");
    }

  } while ( 0 );

  return check_result;
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_validate_mcrup_tca

DESCRIPTION
  This function is used to validate revB TCA based on Limitations described in 
  RevB FDD. 

DEPENDENCIES
  MC TCA msg needs to be unpacked and stored in static memory - hdrrup.tca_msg 

PARAMETERS
  None

RETURN VALUE
  E_SUCCESS           - TCA is compliant with revB FDD and revB standard
  E_PROT_UNKN_MSG     - TCA is NOT compliant with revB standard 
  E_VERSION_MISMATCH  - TCA is NOT compliant with revB FDD
  E_NOT_SUPPORTED     - band class of assigned channel is invalid
  E_NOT_ALLOWED      -  band class of assigned channel is in avoided list
  E_DATA_INVALID      - num_sectors = 0 in the TCA msg

SIDE EFFECTS
  None
===========================================================================*/
LOCAL hdrerrno_enum_type hdrrup_validate_mcrup_tca (void)
{
  hdrerrno_enum_type check_result = E_SUCCESS; 
    /* Return err_number from validating TCA */

  hdrsrch_subactive_set_type subactive_set_list[HDRSRCH_MAX_NUM_SUBASETS];
    /* storage for srch sub-active set structure from TCA*/

  uint8 num_cc_subasets = 0;     
    /* Total number of subactives that carry cc*/

  uint8 total_num_fwd_chans = 0; 
    /* Total number of fwd channels */

  uint8 total_num_rev_chans = 0; 
    /* Total number of rev channels */

  boolean locked_mode = TRUE; 
    /* Based on ServingSectorLockAcrossSubActiveSets attribute*/ 

  int cnt;  /* loop counter*/
  int cnt2; /* loop counter*/
  int cnt3; /* loop counter*/
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
  uint8 num_avoided_channels = 0;
  /* Current avoided channels returned by SD*/
  sys_avoided_channel_record_type avoided_chan_rec[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];
  int ch;  
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
/* ----------------------------------------------------------------------------*/
  if (hdrrup.tca_msg.num_sub_asets > HDRSRCH_MAX_NUM_SUBASETS )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid num_sub_asets %d ", 
                   hdrrup.tca_msg.num_sub_asets);
    return E_VERSION_MISMATCH;
  }

  if (hdrrup.tca_msg.num_sectors > HDRRUP_SECTOR_MAX_SIZE )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid total number of sectors %d ", 
                   hdrrup.tca_msg.num_sectors);
    return E_VERSION_MISMATCH;
  }

  /* Set the rev channels in ascending order */
  check_result = hdrrup_set_mc_tca_rev_chan_order();

  /* Determine if Locked mode is used*/
  if (hdrfmac_serving_sector_locked_across_sub_asets(&locked_mode) != E_SUCCESS)
  {
    ERR_FATAL("Can't retrieve locked mode from fmac",0,0,0);
  }
  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"DEBUG: Locked Mode %d ", locked_mode);

  /* Some Checking will be easier by using srch sub-active set structure*/
  hdrrup_create_subactive_set_list(subactive_set_list, 
                                   HDRSRCH_MAX_NUM_SUBASETS,
                                   &hdrrup.tca_msg);

  do
  {
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Reset avoided channel list");
        num_avoided_channels = 0;
        memset( avoided_chan_rec, 0, sizeof(avoided_chan_rec) );		
        num_avoided_channels = hdrovhd_update_sys_avoid_list(
        HDRSRCH_AVOIDED_CHAN_MAX_SIZE,
        avoided_chan_rec);
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
	/*------------------------------------------------------------------------
      Check if the channel assigned in TCA msg is of valid band class 
      ( same checking as revA TCA)
    ------------------------------------------------------------------------*/
    for (cnt = 0; cnt < hdrrup.tca_msg.num_sub_asets; cnt++)
    {
      if (check_result != E_SUCCESS)
      {
        /* band class in previous subaset is invalid, skip remaining subasets*/
        break;
      }

      for (cnt2 = 0; cnt2 < hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels; cnt2++)
      {
        if (check_result != E_SUCCESS)
        {
          /* band in previous channel is invalid, skip remaining channel*/
          break;
        }

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
        /*------------------------------------------------------------------------
          Check if the channel assigned in TCA msg is avoided by SD
        ------------------------------------------------------------------------*/
	for ( ch = 0; ch < num_avoided_channels; ch++ )
        {
           if ( SYS_CHAN_CMP_EQUAL( hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel, 
               avoided_chan_rec[ch].channel) )
           {
             check_result = E_NOT_ALLOWED;
	     HDR_MSG_PROT_2( MSG_LEGACY_ERROR, "hdrrup_validate_mcrup_tca(), Avoided band%d freq%d in TCA ", 
		         hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.band,
			 hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.chan_num);
             break ;
           }
        }

        if (check_result != E_SUCCESS)
        {
          /* current channel is avoided, skip remaining channel*/
          break;
        }
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
        if ( /* if bandclass not supported OR */
             (!hdrutil_is_band_supported( 
                 hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.band)) ||
             /* if channel not in band OR */
             (!hdrrup_is_band_chan_supported(
                 &hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel)) )
        {
          /*------------------------------------------------------------------------
          Check if the channel assigned in TCA msg is of valid chan & band class
          ------------------------------------------------------------------------*/
          HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"Unsupported band/chan %d/%d rvcd in TCA",
                            hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.band,
                            hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.chan_num);
          check_result = E_NOT_SUPPORTED;
        }
        total_num_fwd_chans++;
      }

      if (subactive_set_list[cnt].num_aset_pilots == 0)
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"No Pilot in subaset %d ", cnt);
        check_result = E_DATA_INVALID;
        break;
      }
    }

    if (check_result != E_SUCCESS)
    {
      break; 
    }

    /*------------------------------------------------------------------------
      Check NumSectors is not zero (same checking as revA TCA)
     ------------------------------------------------------------------------*/
    if( hdrrup.tca_msg.num_sectors == 0 )
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid NumSectors %d rvcd in TCA",
                        hdrrup.tca_msg.num_sectors);
      check_result = E_DATA_INVALID;
      break;
    }

    /* Compare between subactive sets here */
    for (cnt = 0; cnt < (hdrrup.tca_msg.num_sub_asets - 1); cnt++)
    {
      if (check_result != E_SUCCESS)
      {
        /* previous comparison failed, skip remaing comparison */
        break;
      }

      /* CP FDD:
       * The AN shall not specify different band classes across different sub-Active Sets
       * ( TODO: only one channel per sub-active set, if this assumption is changed, 
       *   need to compare band class for all channels)
       */
      if (subactive_set_list[cnt].channel.band != subactive_set_list[cnt+1].channel.band)
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR,"Different Band classes across Sub-Active Sets");
        check_result = E_VERSION_MISMATCH;
        break;
      }

      if (locked_mode && 
          (subactive_set_list[cnt].num_aset_pilots != subactive_set_list[cnt+1].num_aset_pilots))
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH,"Locked Mode; TCA with different number of Pilots in subasets!");
      }
    }

    for (cnt = 0; cnt < hdrrup.tca_msg.num_sub_asets; cnt++)
    {
      for (cnt2 = cnt+1; cnt2 < hdrrup.tca_msg.num_sub_asets; cnt2++)
      {
        if (check_result != E_SUCCESS)
        {
          /* previous check failed */
          break;
        }

        /* CP FDD:
         * The AN shall not specify same channel number across different sub-Active Sets
         */
        if (subactive_set_list[cnt].channel.chan_num == subactive_set_list[cnt2].channel.chan_num)
        {
          HDR_MSG_PROT(MSG_LEGACY_ERROR,"Same Channel Numbers across sub-Active Sets");
          check_result = E_VERSION_MISMATCH;
          break;
        }

        /* Need to check FeedbackReverseChannelIndex based on FDD paired requirement*/
        if ((hdrrup.tca_msg.feedback_mux_enabled == 0) &&
            (hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_rev_ch_id ==
             hdrrup.tca_msg.sub_aset[cnt2].ftc_params.feedback_rev_ch_id))
        {
          HDR_MSG_PROT(MSG_LEGACY_ERROR,"FeedbackReverseChannelIndex should be different");
          check_result = E_VERSION_MISMATCH;
          break;
        }
      }
    }

    if (check_result != E_SUCCESS)
    {
      /* previous check failed */
      break;
    }

    /* Channel span check.Standard: MaxForwardLinkBandwidthNoJammer*/
    check_result = hdrrup_check_mc_frequency_bdwidth(); 
    if (check_result != E_SUCCESS)
    {
      /* Channel span checking failed*/
      break;
    }

    if (check_result != E_SUCCESS)
    {
      /* previous check failed */
      break;
    }

    /* revB FDD: 
     * The AN shall not set FeedbackMultiplexingEnabled to be 1*/
    if (hdrrup.tca_msg.feedback_mux_enabled &&
        (hdrrupc_get_fwd_fb_mux_supp_attrib() == HDRRUPC_DEFAULT_FWD_FEEDBACK_MUX_SUPPORTED))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,"FeedbackMultiplexingEnabled should not be set to 1 ");
      check_result = E_VERSION_MISMATCH;
      break;
    }

    /* SubActive Set Parameters*/
    for( cnt = 0; cnt < hdrrup.tca_msg.num_sub_asets; cnt++ )  
    {
      if (check_result != E_SUCCESS)
      {
        /* If any check failed in previous sub-active set, skip remaining sub-active sets */
        break;
      }

      if (hdrrup.tca_msg.sub_aset[cnt].carries_cc == 1)
      {
        num_cc_subasets++;
        hdrrup.cc_saset_index = cnt;

        /* revB FDD 
         * The AN shall set the sub-active set which carries the control channel as
         * ThisSubActiveSetNotReportable = 0
         */
        if (hdrrup.tca_msg.sub_aset[cnt].not_reportable == 1)
        {
          HDR_MSG_PROT(MSG_LEGACY_ERROR,"CC carrier shall not be NotReportable");
          check_result = E_VERSION_MISMATCH;
          break;
        }
      }

      /* revB FDD 
       * The AN shall set NumFwdChannelsThisSubActiveSet to 1. 
       */
      if (hdrrup.tca_msg.chan_incl && 
          (hdrrupc_get_max_num_fl_supported_attrib() <= HDRRUP_MAX_NUM_FL_SUPPORTED_PHASE_1 ) &&
          (hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels != 1)) 
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"sub aset %d,NumFwdChannelsThisSubActiveSet:%d ,shall be set to 1",
                       cnt, hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels);
        check_result = E_VERSION_MISMATCH;
        break;
      }

      /* revB FDD 
       * The AN shall set FeedbackEnabled to 1
       */
      if ((hdrrupc_get_max_num_fl_supported_attrib() <= HDRRUP_MAX_NUM_FL_SUPPORTED_PHASE_1) &&
          (hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_enabled != 1)) 
      {
        HDR_MSG_PROT (MSG_LEGACY_ERROR, "FeedbackEnabled shall set to 1 in TCA");
        check_result = E_VERSION_MISMATCH;
        break;
      }

      /* revB FDD 
       * The AN shall set DSCForThisSubactiveSetEnabled to 1
       */
      if ((hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_enabled == 1) &&
          (hdrrupc_get_max_num_fl_supported_attrib() <= HDRRUP_MAX_NUM_FL_SUPPORTED_PHASE_1 ) &&
           (hdrrup.tca_msg.sub_aset[cnt].ftc_params.dsc_enabled != 1)) 
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "sub aset %d,DSCForThisSubactiveSetEnabled shall set to 1",
                        cnt );
        check_result = E_VERSION_MISMATCH;
        break;
      }

      /* The AN shall set NumReverseChannels to 1 */
      if (hdrrup.tca_msg.sub_aset[cnt].num_rev_channels != 1)
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"sub aset %d,NumReverseChannels:%d ,shall be set to 1",
                       cnt, hdrrup.tca_msg.sub_aset[cnt].num_rev_channels);
        check_result = E_VERSION_MISMATCH;
        break;
      }

      /* revB FDD:  
       * The AN shall set ReverseChannelConfiguration to '01' */
      for( cnt2 = 0; cnt2 < hdrrup.tca_msg.sub_aset[cnt].num_rev_channels; cnt2++ )
      {
        if (check_result != E_SUCCESS)
        {
          /* Previous rev channel check failed, skip remaining rev channels */
          break;
        }

        if ((hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].ch_config != 
             HDRRUP_REV_CH_CONFIG_PAIRED_CH) &&
            (hdrrupc_get_fwd_fb_mux_supp_attrib() == HDRRUPC_DEFAULT_FWD_FEEDBACK_MUX_SUPPORTED)) 
        {
          HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"sub aset %d,RevChannelConfiguration:%d ,shall be set to 1",
                         cnt, hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].ch_config);
          check_result = E_VERSION_MISMATCH;
          break;
        } 

        total_num_rev_chans++;
      }

      if (check_result != E_SUCCESS)
      {
        /* RevChannelConfiguration check failed*/
        break;
      }

      /* Checking Sub-Active Set Sector records*/
      for( cnt2 = 0; cnt2 < hdrrup.tca_msg.num_sectors; cnt2++ )
      {
        if (check_result != E_SUCCESS)
        {
          /* if any checking failed in previous sector, skip remaining sectors */
          break;
        }

        if ((hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].pilot_included) &&
            (hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_enabled))
        {
          /* revB FDD: 
           * The AN shall not set NumUniqueForwardTrafficMACIndices to be greater than 1. 
           * This requirement shall be applied based on SupportedDRXPatterns
           */
          if (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac > 1)
          {
            HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"sub aset %d,NumUniqueForwardTrafficMACIndices:%d (!=1) ",
                           cnt, hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac);
            check_result = E_VERSION_MISMATCH;
            break;
          }
        }

        /* CHECK_7_1
         * Standard 7.9.6.1.1.3:
         * DRC Covers associated with members of the sub-active set shall be unique within the 
         * sub-active set
         */
        if (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].pilot_included)
        {
          for (cnt3 = cnt2 + 1; cnt3 < hdrrup.tca_msg.num_sectors; cnt3++)
          {
            if (check_result != E_SUCCESS)
            {
              /* if first round check failed, skip remaining sectors*/
              break;
            }

            if (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt3].pilot_included &&
                (hdrrup.tca_msg.sector_info[cnt2].drc_cover == hdrrup.tca_msg.sector_info[cnt3].drc_cover))
            {
              HDR_MSG_PROT(MSG_LEGACY_ERROR,"DRC Covers shall be unique within the sub-active set");
              check_result = E_PROT_UNKN_MSG;
              break; /* break cnt3 loop*/
            }
          }

          if (check_result != E_SUCCESS)
          {
            /* previous check failed*/
            break;
          }

          for (cnt3 = 0; cnt3 < hdrrup.tca_msg.sub_aset[cnt].num_rev_channels; cnt3++)
          {
            if (check_result != E_SUCCESS)
            {
              /* previous check failed*/
              break;
            }
            if (((hdrrupc_get_max_num_fl_supported_attrib()) <= HDRRUP_MAX_NUM_FL_SUPPORTED_PHASE_1) &&
                (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].rab_mac_index != 
                HDRRUP_DEFAULT_RAB_MAC_INDEX))
            {
              HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"RABMACIndex %d in sub_aset %d is not equal to 4",
                              hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].rab_mac_index,
                              cnt);
              check_result = E_VERSION_MISMATCH;
              break; /* break cnt3 loop*/
            }

            /* For Phase1: (revB CP FDD) 
             * The ForwardTrafficMACIndex and ReverseLinkMACIndex need to be set to
             * the same value for all sectors of assigned carriers
             * Because we already check num_rev_channels = NumUniqueForwardTrafficMACIndices = 1, we can 
             * do the check here.
             */
            if (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].rev_mac_index !=
                hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_params[cnt3].ftc_mac_index)
            {
              HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"rev_mac_index %d in different than fwd_mac_index %d",
                              hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].rev_mac_index,
                              hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_params[cnt3].ftc_mac_index);
              check_result = E_VERSION_MISMATCH;
              break; /* break cnt3 loop*/
            }
          }
        }
      }  /* Checking Sub-Active Set Sector records*/
    } /* end of SubActive Set parameters check*/

    if (check_result != E_SUCCESS)
    {
      /* previous check failed*/
      break;
    }

    /* Standard: The AN shall set the SubActiveSetCarriesControlChannel field 
     * of the TCA for exactly one sub-Active Set in the AT's Active Set 
     */
    if (num_cc_subasets != 1 )
    {
      /* should only have only subactive set carries ControlChannel*/
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"%d subactive set(s) carry CC", num_cc_subasets);
      check_result = E_PROT_UNKN_MSG ;
      break; 
    }

    if (total_num_fwd_chans > hdrrupc_get_max_num_fl_supported_attrib() )
    {
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"number of fwd channels in tca %d is greater than negotiated value %d", 
                     total_num_fwd_chans, hdrrupc_get_max_num_fl_supported_attrib());
      check_result = E_PROT_UNKN_MSG ;
      break; 
    }

    if (total_num_rev_chans > hdrrupc_get_max_num_rl_supported_attrib() )
    {
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"number of rev channels in tca %d is greater than negotiated value %d", 
                     total_num_rev_chans, hdrrupc_get_max_num_rl_supported_attrib());
      check_result = E_PROT_UNKN_MSG ;
      break; 
    }
  } while (0);

  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "validate tca result: %d", check_result);
  return check_result;

} /* hdrrup_validate_mcrup_tca*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_PROCESS_TCA_MSG

DESCRIPTION
  This function processes the Traffic Channel Assignment message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrrup_process_tca_msg( dsm_item_type *item_ptr )
{
  dsm_item_type *tcc_msg_dsm_ptr; 
    /* Pointer to packed TCC msg in dsm item */

  sys_link_type tca_link;         
    /* Link of received TCA msg */

  hdrrups_state_enum_type rup_state = hdrrups_get_state();
                                   /* Current RUP state */

  hdrerrno_enum_type err_number;        
    /* Result of unpacking TCA msg */

  sd_ss_act_e_type sd_action;      
                            /* Continue, acquire HDR, enter power save etc. */

  sd_ss_bad_sys_e_type bad_sys_reason = SD_SS_BAD_SYS_MAX; 
                                   /* Reason for bad system */

  boolean send_tcc_cc = FALSE;  
    /* Sending TCC and CC upon receiving invalid TCA*/

  uint16 rup_subtype = hdrscp_get_current_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);
    /* Current RUP subtype*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Ignore message if RUP in inactive state
  ---------------------------------------------------------------------------*/
  if((rup_state == HDRRUPS_INACTIVE) ||
     (rup_state == HDRRUPS_CONNECTION_SETUP) ||
     (rup_state == HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP))
  {
    HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                  HDRRUP_TCA_MSG_ID, rup_state );
    return;
  }
  
  /*---------------------------------------------------------------------------
  Peek at the state of the IDLE protocol and ignore the TCAM if it is in SLEEP
  state.  This will prevent the protocols from trying to enable transmit while 
  the RF is also asleep (this is an invalid state transition). 
  ---------------------------------------------------------------------------*/
  if (hdridles_get_state() == HDRIDLES_SLEEP_STATE)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Ignoring TCA while asleep");
    return;
  }
  
  /*---------------------------------------------------------------------------
  Ignore TCAM, if IDLE is in connected setup state, but the connection request
  is not yet sent out of AT. Here, there could still be  a small window of race 
  where the susbstate moved to "ConReq given to ACMAC state" but the actual msg
  has not yet reached AN. 
  ---------------------------------------------------------------------------*/
  
  if( ((hdridles_get_state() == HDRIDLES_CONN_SETUP_STATE) 
     && (hdridles_get_csetup_substate()!=HDRIDLES_CSETUP_CREQ_ACMAC)) )	 
  {
	 HDR_MSG_PROT(MSG_LEGACY_HIGH, "Ignoring TCA, while IDLE is CONN_SETUP, substate not waiting for TCA");
	return;
  }
  
  if((hdridles_get_csetup_substate() == HDRIDLES_CSETUP_INVALID)&&
    (hdralmps_get_state() != HDRALMPS_CONNECTED ) && hdrlmac_is_idle_ta_started())
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
    "Ignoring TCA (fast connect), while not in traffic and connection request is not sent and tune away started.");
    return;
  }  
  
  if((hdridles_get_csetup_substate() == HDRIDLES_CSETUP_INVALID)&&
     (hdralmps_get_state() != HDRALMPS_CONNECTED ) && (hdrovhds_get_state() == HDROVHDS_ACC_HO_IN_PROG))
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
    "Ignoring TCA (fast connect), when access handover is in progress.");
    return;
  }
	 
  memset( &tca_link, 0, sizeof(sys_link_type) );

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( !hdrsap_lte_to_hdr_active_ho_is_in_progress() )
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    /* Get the current link */
    hdrlmac_get_link_info( (hdrsrch_link_id_type)
                            HDRDSM_GET_LINK_ID( item_ptr ), &tca_link );
  }

  /*---------------------------------------------------------------------------
  Unpack the received message and check if it's a valid message. 
  ---------------------------------------------------------------------------*/
  err_number = hdrrup_unpack_tca_msg( item_ptr);

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
  if (err_number == E_NOT_ALLOWED)
  {
    /*---------------------------------------------------------------------
    Avoided Channel in TCA msg 
    ---------------------------------------------------------------------*/
    bad_sys_reason = SD_SS_BAD_SYS_TCA_INV_BAND;  
   /*Resuing the reason since no SD new action is needed */
  }
  else 
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */  
  if (err_number == E_NOT_SUPPORTED)
  {
    /*---------------------------------------------------------------------
    Invalid band class in TCA msg 
    ---------------------------------------------------------------------*/
    bad_sys_reason = SD_SS_BAD_SYS_TCA_INV_BAND;
  }
  else if (err_number == E_DATA_INVALID)
  {
    /*---------------------------------------------------------------------
    Invalid num_pilot in TCA msg
    ---------------------------------------------------------------------*/
    bad_sys_reason = SD_SS_BAD_SYS_TCA_INV_NUM_PILOTS;
  }
  else if (err_number == E_VERSION_MISMATCH)
  {
    /* TCA is not compliant with FDD*/
    if (rup_state == HDRRUPS_IDLE)
    {
      /* Treat E_VERSION_MISMATCH in IDLE state as E_PORT_UNKW_MSG => Ignore message */
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Receiving invalid TCA in IDLE state, ignore it"); 
      err_number = E_PROT_UNKN_MSG;
    }
    else if (rup_state == HDRRUPS_CONNECTED)
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Receiving invalid TCA in Connected state, send TCC and CC"); 
    }
  }
  
  if( bad_sys_reason != SD_SS_BAD_SYS_MAX )
  {
    #ifdef FEATURE_HDR_QOS
    /* Inform ReservationModule about TCA failed, it is needed for QOS HHO*/
    hdrind_give_ind( HDRIND_RUP_ASSIGNMENT_FAILED, NULL );
    #endif
#ifdef FEATURE_LTE_TO_HDR_OH
    if (!hdrsap_lte_to_hdr_active_ho_is_in_progress())
#endif /* FEATURE_LTE_TO_HDR_OH */
    {
      sd_action = sd_ss_ind_hdr_opr_bad_sys( bad_sys_reason, NULL );
          
      HDR_MSG_PROT_1 ( MSG_LEGACY_HIGH,  "SD Action %d", sd_action);
          
      ASSERT (sd_action != SD_SS_ACT_CONTINUE);
          
      if ( sd_action == SD_SS_ACT_ACQ_HDR )
      {
        hdrind_give_ind( HDRIND_RUP_CHANGE_HDR_SYSTEMS, NULL );
      }
      else
      {
        hdrind_give_ind( HDRIND_RUP_EXIT_HDR_MODE, NULL );
      }
    }
  }
  
  if ((err_number == E_PROT_UNKN_MSG) ||
      (err_number == E_NOT_SUPPORTED) ||
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
      (err_number == E_NOT_ALLOWED)   ||	  
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
      (err_number == E_DATA_INVALID)) 
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error while unpacking TCA msg. Ignoring it"); 

#ifdef FEATURE_LTE_TO_HDR_OH
    if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
    {
      hdrind_give_ind (HDRIND_RUP_EXIT_HDR_MODE, NULL );
    }
#endif /* FEATURE_LTE_TO_HDR_OH */
    return;
  }
  else if (err_number == E_VERSION_MISMATCH)
  {
    send_tcc_cc = TRUE;
  }
  else
  {
    /* E_SUCCESS*/
    ASSERT (err_number == E_SUCCESS);
  }

  /*---------------------------------------------------------------------------
  Log the Information received from the Traffic Channel Assignment Message
  ---------------------------------------------------------------------------*/
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "TC Assignment msg received (seq#=%d)", 
                  hdrrup.tca_msg.msg_seq);

  /* AST has decided not to add new fields for MC RUP TCA, instead, they 
   * will use other log packets to get MC TCA information. 
   * hdrplog_log_tca_info() is mainly used for rev0/A TCA (first sub-active set) 
   */
  hdrplog_log_tca_info( hdrrup.tca_msg.num_sectors,
                        hdrrup.tca_msg.sub_aset[0].ftc_params.drc_length,
                        hdrrup.tca_msg.sub_aset[0].ftc_params.drc_channel_gain,
                        hdrrup.tca_msg.sub_aset[0].ftc_params.ack_channel_gain,
                        hdrrup.tca_msg.frame_offset
                       );

  /*---------------------------------------------------------------------------
  Determine RUP state.
  ---------------------------------------------------------------------------*/
  if (( hdrrups_get_state() == HDRRUPS_IDLE )
#ifdef FEATURE_LTE_TO_HDR_OH
      || 
      ( hdrrups_get_state() == HDRRUPS_TUNNEL )
#endif /* FEATURE_LTE_TO_HDR_OH */
     )
  {
    /*-------------------------------------------------------------------------
    Initialize the last received message sequence number
    -------------------------------------------------------------------------*/
    hdrrup.last_tca_msg_seq = hdrrup.tca_msg.msg_seq;

    /*-------------------------------------------------------------------------
    Store idle channel and assigned channel in case reverse link acquisition
    fails and we need to tune back to the idle channel.
    -------------------------------------------------------------------------*/
    hdrrup.initial_tca.idle_link = tca_link;

    /* Remember the assigned channel, it will be used during conn_setup to 
     * idle transition
       */
    if ((hdrrup.tca_msg.num_sub_asets == 1) && 
        (hdrrup.tca_msg.sub_aset[0].num_fwd_channels == 1))
    {
      /* Only one channel in TCA, it is assigned_channel */
      hdrrup.initial_tca.assigned_channel = 
        hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel;

      if(SYS_CHAN_CMP_EQUAL(tca_link.channel,hdrrup.initial_tca.assigned_channel)
         &&  (hdrlmac_feature_skip_disable_tx_enabled() == TRUE))
      {
        /* Single carrier and TCAM has the same band/channel as access attempt */
        hdrrup.cp_skip_disable_tx = TRUE;
      }
      else
      {
        /* Single carrier and TCAM do not have the same band/channel as access attempt
         * or if the feature is not defined
         */
        hdrrup.cp_skip_disable_tx = FALSE;
      }
    }
    else
    {
      /* multiple channels in the TCA*/

      /* For MC RUP, in case of reverse link acquisition fails, HDR SRCH will stay
       * on sub-active set which carries cc and need protocol to call 
       * channel channel command so srch can 1)switch RF from possible wideband to 
       * narrow band 2) switch the current ASET to demode0 if not used demode0.
       *
       * It means if multiple carriers was assigned in TCA,instead of comparing 
       * assigned channel with channel which TCA was received first then call 
       * channel change command if compared result is different, protocol now needs 
       * to always call channel change command.
       */
      hdrrup.cp_skip_disable_tx = FALSE;
    }

    /*-------------------------------------------------------------------------
    Put RUP in Synchronize Connection Setup state
    -------------------------------------------------------------------------*/

    hdrrups_change_state( HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP );

    /*-------------------------------------------------------------------------
    Update lmac TCA msg (which updates searcher active set) & give indication
    -------------------------------------------------------------------------*/
    hdrrup.last_frame_offset = hdrrup.tca_msg.frame_offset;

    /*-------------------------------------------------------------------------
    If the nv enabling 1x tuneaway during session config is set to False and  
    if hdridle_con_is_for_session_config() is true set  
    hdrrup.tc_tune_away = HDRSRCH_TUNE_AWAY_NONE else allow tuneaway 
    -------------------------------------------------------------------------*/
    if ((hdrrup.enable_1x_ta_in_ses_cfg==FALSE) && hdridle_con_is_for_session_config() )
    {
      hdrrup.tc_tune_away = HDRSRCH_TUNE_AWAY_NONE;
    }
    else
    {
      hdrrup.tc_tune_away = HDRSRCH_TUNE_AWAY_IS2000;
    }

    hdrrup.tca_msg.link_id = HDRDSM_GET_LINK_ID( item_ptr );

    hdrplog_tca_rcvd(hdrrup.tca_msg.msg_seq ,item_ptr);
  }
  else if( rup_state == HDRRUPS_CONNECTED )
  {
    /*-------------------------------------------------------------------------
    Validate received message sequence number
    -------------------------------------------------------------------------*/
    if( !hdrutil_msg_seq_num_is_greater ( hdrrup.tca_msg.msg_seq ,
                                          hdrrup.last_tca_msg_seq ,
                                          HDRRUP_TCA_MSG_SEQ_SIZE ) )
    {
      HDRTRACE_RXMSG_IGNORED_SEQ( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                                  HDRRUP_TCA_MSG_ID, rup_state );
#ifdef FEATURE_LTE_TO_HDR_OH
      if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
      {
        hdrind_give_ind (HDRIND_RUP_EXIT_HDR_MODE, NULL );
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
      return;
    }

    /*-------------------------------------------------------------------------
    Update the last received message sequence number
    -------------------------------------------------------------------------*/
    hdrrup.last_tca_msg_seq = hdrrup.tca_msg.msg_seq;

    /*-------------------------------------------------------------------------
    If the Frame offset in the TCA msg is different from the frame offset of
    the last TCA msg received in Idle state, discard the msg & give indication.
    -------------------------------------------------------------------------*/
    if( hdrrup.tca_msg.frame_offset != hdrrup.last_frame_offset )
    {
      hdrind_give_ind( HDRIND_RUP_ASSIGNMENT_REJECTED, NULL );
    }
    else
    {
      /* If AT is going to close connection, don't need to update searcher */
      if (!send_tcc_cc)
      {
      /*-----------------------------------------------------------------------
      Update lmac TCA msg (which updates searcher active set)
      -----------------------------------------------------------------------*/
      hdrlmac_tc_assign( FALSE,
                         (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr ),
                         hdrrup.tc_tune_away,
                           hdrrup.cc_saset_index,
                           &hdrrup.tca_msg,
                           HDRHAI_ROUTE_UPDATE_PROTOCOL,
                           rup_subtype );
      }

      /*-----------------------------------------------------------------------
      Form traffic channel complete message
      -----------------------------------------------------------------------*/
      tcc_msg_dsm_ptr = hdrrup_form_tcc_msg();

      /*-----------------------------------------------------------------------
      Send the traffic channel complete msg on the reverse traffic channel
      If the connection is lost before we send it, drop the message.
      -----------------------------------------------------------------------*/
      hdrhmp_send_msg_on_existing_rtc( 
        HDRHAI_ROUTE_UPDATE_PROTOCOL,
        TRUE,
        HDRRUP_TCC_MSG_PRIORITY,
        HDRHAI_IN_USE, /* Instance of protocol sending message */
        tcc_msg_dsm_ptr,
        NULL,
        NULL );
      
      if (send_tcc_cc)
      {
        /* Received Invalid TCA (errorno == E_VERSION_MISMATCH)
        * Notify AN by sending ConnectionClose with Reason "Connection Error" */
        hdralmp_close_connection(HDRHAI_ABORT_ERROR, HDRHAI_ROUTE_UPDATE_PROTOCOL);
      }
    }
  }
} /* hdrrup_process_tca_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_UNPACK_TCA_MSG

DESCRIPTION
  This function unpacks the received Traffic Channel Assignment message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  E_SUCCESS          - Unpacking of the message succeeds & field values are valid.
  E_PROT_UNKN_MSG    - Error extracting the message or message is not compliant 
                       with standard
  E_VERSION_MISMATCH - Value is not FDD compliant.   
  E_NOT_SUPPORTED    - band class of assigned channel is invalid
  E_NOT_ALLOWED      - band class of assigned channel is in avoided list
  E_DATA_INVALID     - num_sectors = 0 in the TCA msg

SIDE EFFECTS
  None
===========================================================================*/
LOCAL hdrerrno_enum_type hdrrup_unpack_tca_msg
(
  dsm_item_type *item_ptr
)
{
  hdrerrno_enum_type err_number;        /* Result of unpacking TCA msg */

  if ( hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
         == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
       ||
       hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
         == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */
     )
  {
    err_number = hdrrup_unpack_mcrup_tca_msg(item_ptr);

    if (err_number == E_SUCCESS)
    {
      err_number = hdrrup_validate_mcrup_tca();
    }
  }
  else
  {
    err_number = hdrrup_unpack_drup_tca_msg(item_ptr);
  }

  return err_number;
} /* hdrrup_unpack_tca_msg*/

/*===========================================================================

FUNCTION HDRRUP_UNPACK_DRUP_TCA_MSG

DESCRIPTION
  This function unpacks the received rev0/A Traffic Channel Assignment message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  E_SUCCESS          - Unpacking of the message succeeds & field values are valid.
  E_PROT_UNKN_MSG    - Error extracting the message or message is not compliant 
                       with standard
  E_NOT_SUPPORTED    - band class of assigned channel is invalid
  E_NOT_ALLOWED      - band class of assigned channel is in avoided list
  E_DATA_INVALID     - num_sectors = 0 in the TCA msg

SIDE EFFECTS
  None

===========================================================================*/
LOCAL hdrerrno_enum_type hdrrup_unpack_drup_tca_msg
(
  dsm_item_type *item_ptr
)
{
  int offset;                       /* Offset(bits) of field from msg start */
  int cnt;                          /* Index counter */
  int err;                          /* Error value */
  sys_link_type tca_link;           /* Link of received TCA msg */
  uint8  band;                      /* Band received on TCA msg */
  uint8  system_type;               /* System Type received on TCA msg */
  hdrerrno_enum_type unpack_result = E_SUCCESS; 
                                    /* Return value of unpack TCA msg */
  errno_enum_type err_number;            /* return err_number from unpacking */
  boolean ra_channel_gain_included; /* If Rel. A fields are included */
  uint8 ra_channel_gain_ota = 0; /* Value straignt from TCA message */
  static const int8 ra_channel_gain_map[] = {-6, -9, -12, -15}; /* mapping */
  boolean mac_index_msbs_included = FALSE;  /* If the 7th bit is incl. */
  uint8 mac_index_msb_ota = 0; /* Value straignt from TCA message */
  uint8 dsc_channel_gain_ota = 0; /* Value straignt from TCA message */

  boolean delta_t2p_included = FALSE; /* If the DeltaT2P fields are included */
  int8 delta_t2p;                     /* Value from the TCA message */
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
  uint8 num_avoided_channels = 0;
  /* Current avoided channels returned by SD*/
  sys_avoided_channel_record_type avoided_chan_rec[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];
  int ch;  
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  offset = HDRRUP_TCA_MSG_ID_SIZE;
  err = 0; /* E_SUCCESS = 0 */

  /* Treat rev0/A tca as a special case of revB TCA - NumSubActiveSets = 1*/
  hdrrup.tca_msg.num_sub_asets = HDRRUP_MAX_NUM_SUBASETS_NOT_REVB_TCA;

#ifdef FEATURE_LTE_TO_HDR_OH
  if (!hdrsap_lte_to_hdr_active_ho_is_in_progress())
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    /* Get the current link */
    hdrlmac_get_link_info( (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr ),
                           &tca_link );
  }

  /* Unpack the received message */
  err |= (int) HDRRUP_TCA_MSG_SEQ( item_ptr , &hdrrup.tca_msg.msg_seq );
  offset += HDRRUP_TCA_MSG_SEQ_SIZE;

  err |= (int) HDRRUP_TCA_CHANNEL_INCL(item_ptr,&hdrrup.tca_msg.chan_incl);
  offset += HDRRUP_TCA_CHANNEL_INCL_SIZE;

  if( hdrrup.tca_msg.chan_incl == 1 )
  {
     /* Extract the channel from the rcvd message */
     err |= (int) HDRRUP_TCA_SYSTEM_TYPE( item_ptr ,
            &system_type );
     offset += HDRRUP_TCA_SYSTEM_TYPE_SIZE;

     err |= (int) HDRRUP_TCA_BAND_CLASS( item_ptr , &band );
     if( (sys_band_class_e_type) band >= SYS_BAND_CLASS_MAX )
     {
       ERR("Invalid band %d rvcd in TCAM", band, 0, 0);
       err |= E_NO_DATA;
     }

    hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.band = (sys_band_class_e_type) band; 
     offset += HDRRUP_TCA_BAND_CLASS_SIZE;

     err |= (int) HDRRUP_TCA_CHANNEL_NUMBER( item_ptr ,
            &(hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.chan_num) );
     offset += HDRRUP_TCA_CHANNEL_NUMBER_SIZE;

    hdrrup.tca_msg.sub_aset[0].fwd_channel[0].mode = (sys_hdr_chan_rec_mode_e_type) system_type;

    if( hdrrup.tca_msg.sub_aset[0].fwd_channel[0].mode == SYS_HDR_CHAN_REC_MODE_MAX )
     {
       err |= E_NO_DATA;
       ERR("Invalid system_type %d rcvd in TCAM", system_type, 0, 0);
     }
  }
  else
  {
    /* TODO, system type(mode) should get from link_info also*/
    hdrrup.tca_msg.sub_aset[0].fwd_channel[0].mode = SYS_HDR_CHAN_REC_MODE_IS856;
    hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.band = tca_link.channel.band; 
    hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.chan_num = tca_link.channel.chan_num; 
  }

  err |= (int) HDRRUP_TCA_FRAME_OFFSET( item_ptr,
                    &hdrrup.tca_msg.frame_offset, offset);
  offset += HDRRUP_TCA_FRAME_OFFSET_SIZE;

  err |= (int) HDRRUP_TCA_DRC_LENGTH( item_ptr ,
                    &hdrrup.tca_msg.sub_aset[0].ftc_params.drc_length , offset );
  offset += HDRRUP_TCA_DRC_LENGTH_SIZE;

  err |= (int) HDRRUP_TCA_DRC_CHANNEL_GAIN( item_ptr,
                     &hdrrup.tca_msg.sub_aset[0].ftc_params.drc_channel_gain, offset);
  offset += HDRRUP_TCA_DRC_CHANNEL_GAIN_SIZE;

  err |= (int) HDRRUP_TCA_ACK_CHANNEL_GAIN( item_ptr,
                     &hdrrup.tca_msg.sub_aset[0].ftc_params.ack_channel_gain, offset);
  offset += HDRRUP_TCA_ACK_CHANNEL_GAIN_SIZE;

  err |= (int) HDRRUP_TCA_NUM_PILOTS( item_ptr ,
                    &hdrrup.tca_msg.num_sectors , offset );
  offset += HDRRUP_TCA_NUM_PILOTS_SIZE;

  if( hdrrup.tca_msg.num_sectors > HDRSRCH_ASET_MAX_SIZE )
  {
    ERR("Invalid num_pilots %d rvcd in TCAM", hdrrup.tca_msg.num_sectors, 0, 0);
    err |= E_NO_DATA;
  }

  /* Ensure that num of pilots never exceeds HDRSRCH_ASET_MAX_SIZE */
  hdrrup.tca_msg.num_sectors  = MIN(hdrrup.tca_msg.num_sectors,HDRSRCH_ASET_MAX_SIZE);

  for( cnt=0; cnt < hdrrup.tca_msg.num_sectors ; cnt++ )
  {
    err |= (int) HDRRUP_TCA_PILOT_PN(item_ptr,
                    &hdrrup.tca_msg.sector_info[cnt].pilot_pn, offset);
    offset += HDRRUP_TCA_PILOT_PN_SIZE;
    err |= (int) HDRRUP_TCA_SOFTER_HO( item_ptr,
                    &hdrrup.tca_msg.sector_info[cnt].softer_handoff, offset);
    offset += HDRRUP_TCA_SOFTER_HO_SIZE;
    /* AN should set the first instance of this field to '0' */
    if ( cnt == 0 &&
         hdrrup.tca_msg.sector_info[cnt].softer_handoff == 1 )
    {
      ERR("Invalid: first softer handoff is 1", 0, 0, 0);
      err |= E_PROT_UNKN_MSG;  
    }

    err |= (int) HDRRUP_TCA_MAC_INDEX( item_ptr ,
                    &hdrrup.tca_msg.sub_aset[0].sector_params[cnt].ftc_params[0].ftc_mac_index, offset );
    offset += HDRRUP_TCA_MAC_INDEX_SIZE;

    err |= (int) HDRRUP_TCA_DRC_COVER( item_ptr,
                    &hdrrup.tca_msg.sector_info[cnt].drc_cover, offset );
    offset += HDRRUP_TCA_DRC_COVER_SIZE;
    err |= (int) HDRRUP_TCA_RAB_LENGTH( item_ptr ,
                    &hdrrup.tca_msg.sector_info[cnt].rab_length , offset );
    offset += HDRRUP_TCA_RAB_LENGTH_SIZE;
    err |= (int) HDRRUP_TCA_RAB_OFFSET( item_ptr ,
                    &hdrrup.tca_msg.sector_info[cnt].rab_offset , offset );
    offset += HDRRUP_TCA_RAB_OFFSET_SIZE;

    /* Guarantee that all DeltaT2P fields are initialized to 0
       if the DeltaT2P fields are included */
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].rtc_params[0].delta_t2p = 0;    
  }

  /* The fact that the offset is already being maintained inside this routine
   * makes the macro wrappers unnecessary, so for new fields just use the 
   * bit unpack macros directly.
   */

  err_number = HDRBIT_UNPACK8( item_ptr, &ra_channel_gain_included,
                          offset,  HDRRUP_TCA_RA_CHANNEL_GAIN_INCLUDED_SIZE );
  offset += HDRRUP_TCA_RA_CHANNEL_GAIN_INCLUDED_SIZE;

  if (err_number == E_NO_DATA)
  {
    ra_channel_gain_included = FALSE;

    /* set ra_channel_gain to 0, so srch knows that rev A addendum is not included*/
    for( cnt = 0; cnt < hdrrup.tca_msg.num_sectors; cnt++ )
    {
      hdrrup.tca_msg.sector_info[cnt].ra_channel_gain = 0; 
    }
  }

  if ((ra_channel_gain_included) && (err == 0))
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "TCA. Includes Rel. A Fields" );

    for( cnt = 0; cnt < hdrrup.tca_msg.num_sectors; cnt++ )
    {
      err |= (int) HDRBIT_UNPACK8( item_ptr, &ra_channel_gain_ota,
                                   offset,  HDRRUP_TCA_RA_CHANNEL_GAIN_SIZE );
      offset += HDRRUP_TCA_RA_CHANNEL_GAIN_SIZE;

      hdrrup.tca_msg.sector_info[cnt].ra_channel_gain = 
        ra_channel_gain_map[ra_channel_gain_ota];
    }

    err |= (int) HDRBIT_UNPACK8( item_ptr, &mac_index_msbs_included, offset,  
                                 HDRRUP_TCA_MAC_INDEX_MSBS_INCLUDED_SIZE );
    offset += HDRRUP_TCA_MAC_INDEX_MSBS_INCLUDED_SIZE;

    if (mac_index_msbs_included)
    {
      for( cnt = 0; cnt < hdrrup.tca_msg.num_sectors; cnt++ )
      {
        err |= (int) HDRBIT_UNPACK8( item_ptr, &mac_index_msb_ota,
                                     offset,  HDRRUP_TCA_MAC_INDEX_MSB_SIZE );
        offset += HDRRUP_TCA_MAC_INDEX_MSB_SIZE;

        if (mac_index_msb_ota == 1)
        {
          /* MACIndexMSB is the 7th bit of the MAC Index */
          hdrrup.tca_msg.sub_aset[0].sector_params[cnt].ftc_params[0].ftc_mac_index |= 0x40;
        }
      }

      err |= (int) HDRBIT_UNPACK8( item_ptr, &dsc_channel_gain_ota,
                                   offset,  HDRRUP_TCA_DSC_CHANNEL_GAIN_SIZE );
      offset += HDRRUP_TCA_DSC_CHANNEL_GAIN_SIZE;

      /* We may want to convert the uints in the future... */
      hdrrup.tca_msg.dsc_channel_gain = dsc_channel_gain_ota;

      for( cnt = 0; cnt < hdrrup.tca_msg.num_sectors; cnt++ )
      {
        if (hdrrup.tca_msg.sector_info[cnt].softer_handoff)
        {
          /* The DSC is only specified for the first pilot from a given cell. 
             Copy the dsc for rest of the pilots as well  */

          /* 3GPP2 C.S0024-B Section 7.8.6.2.2 
             "The access network shall set the first instance of softer_handoff
             to '0'". So, cnt should never be '0', the first time we enter this 
             if condition. Sanity check to make LINT happy */
          if (cnt != 0)
          {
            hdrrup.tca_msg.sector_info[cnt].dsc = 
              hdrrup.tca_msg.sector_info[cnt-1].dsc;
          }

          continue;
        }

        err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                     &hdrrup.tca_msg.sector_info[cnt].dsc, offset,
                                     HDRRUP_TCA_DSC_SIZE );
        offset += HDRRUP_TCA_DSC_SIZE;
      }

      /* Only if the DSC fields are included and properly parsed will the 
         DeltaT2P fields be included */
      if( err == 0 )
      {
        /* Check to see if the Delta T2P fields are included */
        err_number = HDRBIT_UNPACK8( item_ptr, &delta_t2p_included, offset, 
                                     HDRRUP_TCA_DELTA_T2P_INCLUDED_SIZE );
        offset += HDRRUP_TCA_DELTA_T2P_INCLUDED_SIZE;
    
        /* DeltaT2P cannot be included if there is no data to be unpacked */
        if( err_number == E_NO_DATA )
        {
          delta_t2p_included = FALSE;
        }
    
        /* Unpack NumPilots occurrences if the DeltaT2P fields are included */
        if( delta_t2p_included )
        {
          for( cnt = 0; cnt < hdrrup.tca_msg.num_sectors; cnt++ )
          {
            err |= (int) HDRBIT_UNPACK8( item_ptr, (uint8 *) &delta_t2p, offset, 
                                         HDRRUP_TCA_DELTA_T2P_SIZE );
            offset += HDRRUP_TCA_DELTA_T2P_SIZE;
    
            /* DeltaT2P is a 6 bit value that requires sign extension */
            delta_t2p = HDRRUP_EXTEND6( delta_t2p );
    
            /* Store the n-th pilot's DeltaT2P value */
            hdrrup.tca_msg.sub_aset[0].sector_params[cnt].rtc_params[0].delta_t2p = delta_t2p;
          }
        }
      }
    } /* if (mac_index_msbs_included) */

    if (err != 0)
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "TCA. Error unpacking Rel. A Fields" );
    }
  } /* if (ra_channel_gain_included) */

  /* Initiate revB Fields of revA TCA. it may be useful when lmac create subaset list for 
   * both rev0/A and revB TCA
   */
  hdrrup.tca_msg.scheduler_tag_incl = 0;
  hdrrup.tca_msg.feedback_mux_enabled = 0;
  hdrrup.tca_msg.sub_aset[0].num_fwd_channels = 1;
  hdrrup.tca_msg.sub_aset[0].ftc_params.feedback_enabled = 1;
  hdrrup.tca_msg.sub_aset[0].ftc_params.feedback_rev_ch_id = 1;
  hdrrup.tca_msg.sub_aset[0].carries_cc = 1;
  hdrrup.tca_msg.sub_aset[0].not_reportable = 0;
  hdrrup.tca_msg.sub_aset[0].ftc_params.dsc_enabled = 1;
  hdrrup.tca_msg.sub_aset[0].ftc_params.next_3fields_same = 0;
  hdrrup.tca_msg.sub_aset[0].num_rev_channels_incl = 1;
  hdrrup.tca_msg.sub_aset[0].num_rev_channels = 1;
  hdrrup.tca_msg.sub_aset[0].rev_channel[0].channel =                         
                                          hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel;
  hdrrup.tca_msg.sub_aset[0].rev_channel[0].a_order = 1;                         
  hdrrup.tca_msg.sub_aset[0].rev_channel[0].ch_config = 1;                         
  hdrrup.cc_saset_index = 0;
  for( cnt=0; cnt < hdrrup.tca_msg.num_sectors ; cnt++ )
  {
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].pilot_included = 1;
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].fwd_chan_index = 1;
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].pilot_group_id = 
                                                     HDRSRCH_NOT_INCLUDED_PILOT_GID;
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].scheduler_tag = HDRSRCH_REV0A_SCHDTAG;
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].num_unique_ftc_mac = 1;
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].ftc_mac_index_interlace_enabled = 0;
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].rtc_params[0].rab_mac_index = 
                                                       HDRRUP_DEFAULT_RAB_MAC_INDEX;
    hdrrup.tca_msg.sub_aset[0].sector_params[cnt].rtc_params[0].rev_mac_index = 
             hdrrup.tca_msg.sub_aset[0].sector_params[cnt].ftc_params[0].ftc_mac_index;
  }

  /* Check for MAC Index Zero */
  for( cnt=0; cnt < hdrrup.tca_msg.num_sectors ; cnt++ )
  {
    if(hdrrup.tca_msg.sub_aset[0].sector_params[cnt].ftc_params[0].ftc_mac_index == 0)
    {
        err |=  E_PROT_UNKN_MSG;
		HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "TCA. Invalid MAC Index for sector[%d]", cnt);
    }
  }

  if( err != 0 )
  {
    /* ignore invalid TCA */
    unpack_result = E_PROT_UNKN_MSG;
  }
  else if ( /* if bandclass not supported OR */
            (!hdrutil_is_band_supported( 
                hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.band)) ||
            /* if channel not in band OR */
            (!hdrrup_is_band_chan_supported(
                &hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel)) )

  {
    /*------------------------------------------------------------------------
    Check if the channel assigned in TCA msg is of valid band class
    ------------------------------------------------------------------------*/
    ERR("Unsupported band/chan %d/%d rvcd in TCAM", 
        hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.band,
        (uint32)hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.chan_num,
        0);
    unpack_result = E_NOT_SUPPORTED;
  }
  else if( hdrrup.tca_msg.num_sectors == 0 )
  {
    /*------------------------------------------------------------------------
    Check num_pilots is not zero
    ------------------------------------------------------------------------*/
    ERR("Invalid num_pilots %d rvcd in TCAM", hdrrup.tca_msg.num_sectors, 0, 0);
    unpack_result = E_DATA_INVALID;
  }
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
  /*------------------------------------------------------------------------
     Check if the channel assigned in TCA msg is avoided by SD
   ------------------------------------------------------------------------*/
  else  
  {
     for ( ch = 0; ch < num_avoided_channels; ch++ )
     {
        if ( SYS_CHAN_CMP_EQUAL( hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel, 
                                 avoided_chan_rec[ch].channel) )
        {
          unpack_result = E_NOT_ALLOWED;
          HDR_MSG_PROT_2( MSG_LEGACY_ERROR, "hdrrup_unpack_drup_tca_msg(), Avoided band%d freq%d in TCA ", 
              hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.band,
     	      hdrrup.tca_msg.sub_aset[0].fwd_channel[0].channel.chan_num);
          break ;
        }
     }
   }
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

  return unpack_result;

} /* hdrrup_unpack_drup_tca_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_UNPACK_MCRUP_TCA_MSG

DESCRIPTION
  This function unpacks the received revB(MultiCarrier) Traffic Channel 
  Assignment message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  E_SUCCESS          - Unpacking of the message succeeds & field values are valid.
  E_PROT_UNKN_MSG    - Error extracting the message or message is not compliant 
                       with standard
  E_VERSION_MISMATCH - Value is not FDD compliant.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL hdrerrno_enum_type hdrrup_unpack_mcrup_tca_msg
(
  dsm_item_type *item_ptr
)
{
  int offset;                       /* Offset(bits) of field from msg start */
  int cnt;                          /* Index counter */
  int cnt2;                         /* Index counter */
  int cnt3;                         /* Index counter */
  int err;                          /* Unpack result */

  sys_link_type tca_link;           /* Link of received TCA msg */
  uint8 ra_channel_gain_ota = 0; /* Value straignt from TCA message */
  static const int8 ra_channel_gain_map[] = {-6, -9, -12, -15}; /* mapping */
  hdrerrno_enum_type unpack_result = E_SUCCESS; /* Return value */

  boolean locked_mode = TRUE; 
    /* Based on ServingSectorLockAcrossSubActiveSets attribute*/ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  offset = HDRRUP_TCA_MSG_ID_SIZE;
  err = 0; /* E_SUCCESS = 0 */

#ifdef FEATURE_LTE_TO_HDR_OH
  if (!hdrsap_lte_to_hdr_active_ho_is_in_progress())
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    /* Get the current link */
    hdrlmac_get_link_info( (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr ),
                           &tca_link );
  }

  /* Determine if Locked mode is used*/
  if (hdrfmac_serving_sector_locked_across_sub_asets(&locked_mode) != E_SUCCESS)
  {
    ERR_FATAL("Can't retrieve locked mode from fmac",0,0,0);
  }

  do
  {
    /* Unpack the received message */
    /* Message Sequence */
    err |= (int) HDRBIT_UNPACK8( item_ptr , &hdrrup.tca_msg.msg_seq,
                                 offset, HDRRUP_TCA_MSG_SEQ_SIZE);
    offset += HDRRUP_TCA_MSG_SEQ_SIZE;

    /* DSC Channel Gain Base*/
    err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.dsc_channel_gain,
                                 offset,  HDRRUP_TCA_DSC_CHANNEL_GAIN_SIZE );
    offset += HDRRUP_TCA_DSC_CHANNEL_GAIN_SIZE;

    /* frame offset*/
    err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.frame_offset,
                                 offset,  HDRRUP_TCA_FRAME_OFFSET_SIZE );
    offset += HDRRUP_TCA_FRAME_OFFSET_SIZE;

    /* Number Sectors*/
    err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.num_sectors,
                                 offset,  HDRRUP_TCA_NUM_SECTORS_SIZE );
    offset += HDRRUP_TCA_NUM_SECTORS_SIZE;

    /* Number Sub Active Sets*/
    err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.num_sub_asets,
                                 offset,  HDRRUP_TCA_NUM_SUB_ASETS_SIZE );
    offset += HDRRUP_TCA_NUM_SUB_ASETS_SIZE;

    if (err != 0)
    {
      /* Unpack error */
      unpack_result = E_PROT_UNKN_MSG;
      break;
    }
    else if((hdrrup.tca_msg.num_sub_asets > hdrrupc_get_max_num_sub_asets_attrib()) ||
            (hdrrup.tca_msg.num_sub_asets == 0))
    {
      HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Invalid num_sub_asets %d rvcd in TCAM",
                      hdrrup.tca_msg.num_sub_asets);
      unpack_result = E_PROT_UNKN_MSG;
      break;
    }

    /* Sanity check */
    if (hdrrup.tca_msg.num_sub_asets > HDRSRCH_MAX_NUM_SUBASETS )
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid num_sub_asets %d ", 
                     hdrrup.tca_msg.num_sub_asets);
      return E_VERSION_MISMATCH;
    }

    if (hdrrup.tca_msg.num_sectors > HDRRUP_SECTOR_MAX_SIZE )
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid total number of sectors %d ", 
                     hdrrup.tca_msg.num_sectors);
      return E_VERSION_MISMATCH;
    }

    /* Assigned Channel Included */
    err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.chan_incl,
                                 offset,  HDRRUP_TCA_CHANNEL_INCL_SIZE );
    offset += HDRRUP_TCA_CHANNEL_INCL_SIZE;

    /* Scheduler Tag Included */
    err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.scheduler_tag_incl,
                                 offset,  HDRRUP_TCA_SCHEDULER_TAG_INCL_SIZE );
    offset += HDRRUP_TCA_SCHEDULER_TAG_INCL_SIZE;

    /* Feedback Multiplexing Enabled */
    err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.feedback_mux_enabled,
                                 offset,  HDRRUP_TCA_FEEDBACK_MUX_ENABLED_SIZE );
    offset += HDRRUP_TCA_FEEDBACK_MUX_ENABLED_SIZE;

    if (err != 0)
    {
      /* something is wrong before unpacking SectorInformation*/
      unpack_result = E_PROT_UNKN_MSG;
      break;
    }

    /* Sector Information*/
    for( cnt=0; cnt < hdrrup.tca_msg.num_sectors ; cnt++ )
    {
      if (err != 0)
      {
        /* something is wrong while unpacking previous sector, skip remaining sectors*/
        unpack_result = E_PROT_UNKN_MSG;
        break;
      }
      /* RA Channel Gain */
      err |= (int) HDRBIT_UNPACK8( item_ptr, &ra_channel_gain_ota,
                                   offset,  HDRRUP_TCA_RA_CHANNEL_GAIN_SIZE );
      offset += HDRRUP_TCA_RA_CHANNEL_GAIN_SIZE;

      hdrrup.tca_msg.sector_info[cnt].ra_channel_gain = 
        ra_channel_gain_map[ra_channel_gain_ota];
    
      /* Pilot PN */
      err |= (int) HDRBIT_UNPACK16( item_ptr, &hdrrup.tca_msg.sector_info[cnt].pilot_pn,
                                   offset,  HDRRUP_TCA_PILOT_PN_SIZE );
      offset += HDRRUP_TCA_PILOT_PN_SIZE;

      /* DRC Cover */
      err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sector_info[cnt].drc_cover,
                                   offset,  HDRRUP_TCA_DRC_COVER_SIZE );
      offset += HDRRUP_TCA_DRC_COVER_SIZE;

      /* Softer Handoff */
      err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sector_info[cnt].softer_handoff,
                                   offset,  HDRRUP_TCA_SOFTER_HO_SIZE );
      offset += HDRRUP_TCA_SOFTER_HO_SIZE;
    }

    if (err != 0)
    {
      /* something is wrong before unpacking DSC*/
      unpack_result = E_PROT_UNKN_MSG;
      break;
    }

    /* DSC */
    for( cnt = 0; cnt < hdrrup.tca_msg.num_sectors; cnt++ )
    {
      if (hdrrup.tca_msg.sector_info[cnt].softer_handoff)
      {
        if (cnt != 0)
        {
          /* update dsc info for all sectors in the same cell*/
          hdrrup.tca_msg.sector_info[cnt].dsc =
            hdrrup.tca_msg.sector_info[cnt-1].dsc;
        }
        else 
        {
          /* AN should set the first instance of this field to '0' */
          HDR_MSG_PROT( MSG_LEGACY_ERROR,
                        "Invalid: first softer handoff is 1" );
          err |= E_PROT_UNKN_MSG;
        }
        /* The DSC is only specified for the first pilot from a given cell */
        continue;
      }
      err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sector_info[cnt].dsc,
                                   offset, HDRRUP_TCA_DSC_SIZE );
      offset += HDRRUP_TCA_DSC_SIZE;
    }

    if (err != 0)
    {
      /* something is wrong before unpacking SubActiveSets*/
      unpack_result = E_PROT_UNKN_MSG;
      break;
    }

    /* SubActive Set Parameters*/
    for( cnt = 0; cnt < hdrrup.tca_msg.num_sub_asets; cnt++ )  
    {
      if ((err != 0) || (unpack_result != E_SUCCESS))
      {
        /* something is wrong while unpacking previous SubActiveSets parametes, 
         * skip remaining subactive sets */
        unpack_result = E_PROT_UNKN_MSG;
        break;
      }

      /* NumFwdChannelsThisSubActiveSet and AssignedChannel*/
      if (hdrrup.tca_msg.chan_incl)
      {
        /* NumFwdChannelsThisSubActiveSet*/
        err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels,
                                     offset,  HDRRUP_TCA_NUM_FWD_CH_THIS_SUB_ASET_SIZE );
        offset += HDRRUP_TCA_NUM_FWD_CH_THIS_SUB_ASET_SIZE;

        if( err != 0 )
        {
          /* Unpack error */
          unpack_result = E_PROT_UNKN_MSG;
          break;
        }
        else if (hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels == 0)
        {
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"sub aset %d,Valid range of NumFwdChannelsThisSubActiveSet is 1 to 7",
                         cnt);
          unpack_result = E_PROT_UNKN_MSG ;
          break;
        }
        else if ( hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels > HDRRUP_FWD_CH_PER_SUBASET_MAX_SIZE )
        {
          HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                          "NumFwdChannelsThisSubActiveSet (%d) is invalid", 
                          hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels );
          unpack_result = E_PROT_UNKN_MSG;
          break;
        }

        /* AssignedChannel */
        for( cnt2 = 0; cnt2 < hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels; cnt2++ )
        {
          if ((err != 0) || (unpack_result != E_SUCCESS))
          {
            /* something is wrong while unpacking previous Channel record , 
             * skip remaining channel records */
            unpack_result = E_PROT_UNKN_MSG;
            break;
          }
          /* SystemType */
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                        (uint8 *) &hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].mode,
                         offset,  HDRRUP_TCA_SYSTEM_TYPE_SIZE );
          offset += HDRRUP_TCA_SYSTEM_TYPE_SIZE;

          /* Band */
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                        (uint8 *) &hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.band,
                         offset,  HDRRUP_TCA_BAND_CLASS_SIZE );
          offset += HDRRUP_TCA_BAND_CLASS_SIZE;

          /* Channel */
          err |= (int) HDRBIT_UNPACK16( item_ptr, 
                                       &hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.chan_num,
                                       offset,  HDRRUP_TCA_CHANNEL_NUMBER_SIZE );
          offset += HDRRUP_TCA_CHANNEL_NUMBER_SIZE;

          if (err != 0)
          {
            /* Unpack error */
            unpack_result = E_PROT_UNKN_MSG;
            break;
          }

          /* Check if system type is valid
           * revB phase1, only support system type 0
           */
          if(hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].mode != SYS_HDR_CHAN_REC_MODE_IS856)  
          {
            HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Invalid system_type %d rvcd in TCAM",
                            hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].mode);
            unpack_result = E_VERSION_MISMATCH;
            break;
          }

          if( hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.band >= SYS_BAND_CLASS_MAX )
          {
            HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Invalid fwd band %d rvcd in TCAM",
                            hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.band);
            err |= E_NO_DATA;
          }
        }
      }
      else 
      {
        /* the AN shall omit this field, and the value of this field is assumed to be one*/
        hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels = 1;

        /* Set the channel to the current one */
        /* TO DO: new from tca_link 
        hdrrup.tca_msg.sub_aset[cnt].fwd_channel[0].mode = tca_link.mode; */
        hdrrup.tca_msg.sub_aset[cnt].fwd_channel[0].mode = SYS_HDR_CHAN_REC_MODE_IS856;

        hdrrup.tca_msg.sub_aset[cnt].fwd_channel[0].channel.band = tca_link.channel.band;
        hdrrup.tca_msg.sub_aset[cnt].fwd_channel[0].channel.chan_num = tca_link.channel.chan_num;
      }

      if ((err != 0) || (unpack_result != E_SUCCESS))
      {
        /* something is wrong while unpacking Assigned Channel */ 
        unpack_result = E_PROT_UNKN_MSG;
        break;
      }

      /* FeedbackEnabled */
      err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                   &hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_enabled,
                                   offset,  HDRRUP_TCA_FEEDBACK_ENABLED_SIZE );
      offset += HDRRUP_TCA_FEEDBACK_ENABLED_SIZE;

      if (err != 0)
      {
        /* something is wrong while unpacking FeedbackEnabled */ 
        unpack_result = E_PROT_UNKN_MSG;
        break;
      }

      /* FeedbackMultiplexingIndex*/
      if (hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_enabled &&
          hdrrup.tca_msg.feedback_mux_enabled)
      {
        err |= (int) HDRBIT_UNPACK16( item_ptr, 
                                     &hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_mux_id,
                                     offset,  HDRRUP_TCA_FEEDBACK_MUX_INDEX_SIZE );
        offset += HDRRUP_TCA_FEEDBACK_MUX_INDEX_SIZE;
      }

      /* FeedbackReverseChannelIndex*/
      if (hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_enabled)
      {
        err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                     &hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_rev_ch_id,
                                     offset,  HDRRUP_TCA_FEEDBACK_REV_CH_INDEX_SIZE );
        offset += HDRRUP_TCA_FEEDBACK_REV_CH_INDEX_SIZE;

      }

      /* SubActiveSetCarriesControlChannel */
      err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sub_aset[cnt].carries_cc,
                                   offset,  HDRRUP_TCA_SUB_ASET_CARRIES_CC_SIZE );
      offset += HDRRUP_TCA_SUB_ASET_CARRIES_CC_SIZE;

      /* ThisSubActiveSetNotReportable */
      err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sub_aset[cnt].not_reportable,
                                   offset,  HDRRUP_TCA_SUB_ASET_NOT_REPORTABLE_SIZE );
      offset += HDRRUP_TCA_SUB_ASET_NOT_REPORTABLE_SIZE;

      if (hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_enabled)
      {
        /* DSCForThisSubActiveSetEnabled */
        err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sub_aset[cnt].ftc_params.dsc_enabled,
                                     offset,  HDRRUP_TCA_DSC_SUB_ASET_ENABLED_SIZE );
        offset += HDRRUP_TCA_DSC_SUB_ASET_ENABLED_SIZE;

        /* Next3FieldsSameAsBefore */
        err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sub_aset[cnt].ftc_params.next_3fields_same,
                                     offset,  HDRRUP_TCA_NEXT3_SAME_SIZE );
        offset += HDRRUP_TCA_NEXT3_SAME_SIZE;

        if (err != 0)
        {
          /* Unpack Error */
          unpack_result = E_PROT_UNKN_MSG;
          break;
        }

        if (hdrrup.tca_msg.sub_aset[cnt].ftc_params.next_3fields_same == 1)
        {
          if (cnt == 0)
          {
            /* The AN shall set the value of the first instance of this field to 0*/
            HDR_MSG_PROT(MSG_LEGACY_ERROR, "Nex3FieldsSameAsBefore shall not set to 1 in first sub-aset");
            unpack_result = E_PROT_UNKN_MSG;
            break;
          }
          else
          {
            /* copy values from previous sub-active set*/
            hdrrup.tca_msg.sub_aset[cnt].ftc_params.drc_length = 
              hdrrup.tca_msg.sub_aset[cnt-1].ftc_params.drc_length;

            hdrrup.tca_msg.sub_aset[cnt].ftc_params.drc_channel_gain = 
              hdrrup.tca_msg.sub_aset[cnt-1].ftc_params.drc_channel_gain;

            hdrrup.tca_msg.sub_aset[cnt].ftc_params.ack_channel_gain = 
              hdrrup.tca_msg.sub_aset[cnt-1].ftc_params.ack_channel_gain;
          }
        }
        else
        {
          /* DRCLength*/
          err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sub_aset[cnt].ftc_params.drc_length,
                                       offset,  HDRRUP_TCA_DRC_LENGTH_SIZE );
          offset += HDRRUP_TCA_DRC_LENGTH_SIZE;

          /* DRCChannelGainBase*/
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                         (uint8 *) &hdrrup.tca_msg.sub_aset[cnt].ftc_params.drc_channel_gain,
                          offset,  HDRRUP_TCA_DRC_CHANNEL_GAIN_SIZE );
          offset += HDRRUP_TCA_DRC_CHANNEL_GAIN_SIZE;

          /* ACKChannelGain*/
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                         (uint8 *) &hdrrup.tca_msg.sub_aset[cnt].ftc_params.ack_channel_gain,
                          offset,  HDRRUP_TCA_ACK_CHANNEL_GAIN_SIZE );
          offset += HDRRUP_TCA_ACK_CHANNEL_GAIN_SIZE;
        }
      }

      if (err != 0)
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, "Something is wrong before unpacking ReverseChannel");
        unpack_result = E_PROT_UNKN_MSG;
        break;
      }

      /* NumReverseChannelsIncluded*/
      err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sub_aset[cnt].num_rev_channels_incl,
                                   offset,  HDRRUP_TCA_NUM_REV_CHANNELS_INCL_SIZE );
      offset += HDRRUP_TCA_NUM_REV_CHANNELS_INCL_SIZE;

      if(hdrrup.tca_msg.sub_aset[cnt].num_rev_channels_incl)
      {
        /* NumReverseChannels*/
        err |= (int) HDRBIT_UNPACK8( item_ptr, &hdrrup.tca_msg.sub_aset[cnt].num_rev_channels,
                                     offset,  HDRRUP_TCA_NUM_REV_CHANNELS_SIZE );
        offset += HDRRUP_TCA_NUM_REV_CHANNELS_SIZE;
      }
      else
      {
        hdrrup.tca_msg.sub_aset[cnt].num_rev_channels = 0;
      }

      if (err != 0)
      {
        /* something is wrong while unpacking ReverseChannel */ 
        unpack_result = E_PROT_UNKN_MSG;
        break;
      }
      else if ( hdrrup.tca_msg.sub_aset[cnt].num_rev_channels > HDRRUP_REV_CH_PER_SUBASET_MAX_SIZE )
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                        "NumReverseChannels (%d) is invalid", 
                        hdrrup.tca_msg.sub_aset[cnt].num_rev_channels );
        unpack_result = E_PROT_UNKN_MSG;
        break;
      }

      for( cnt2 = 0; cnt2 < hdrrup.tca_msg.sub_aset[cnt].num_rev_channels; cnt2++ )
      {
        if ((err != 0) || (unpack_result != E_SUCCESS))
        {
          /* something is wrong while unpacking previous Channel record , 
           * skip remaining channel records */
          unpack_result = E_PROT_UNKN_MSG;
          break;
        }

        /* ReverseChannelConfiguration */
        err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                     &hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].ch_config,
                                     offset,  HDRRUP_TCA_REV_CHANNEL_CONFIG_SIZE );
        offset += HDRRUP_TCA_REV_CHANNEL_CONFIG_SIZE;

        if (hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].ch_config == 
            HDRRUP_REV_CH_CONFIG_PAIRED_CH)
        {
          /* Standard: If NumFwdChannelsThisSubActiveSet associated with this sub-Active
           * set is included and set to more than one, the acess network shall not set
           * this field to '01'*/
          if (hdrrup.tca_msg.sub_aset[cnt].num_fwd_channels > 1)
          {
            HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"sub aset %d, RevChannelConfiguration shouldn't set to 1",
                           cnt);
            unpack_result = E_PROT_UNKN_MSG ;
            break;
          }

          /* Paired reverse link channel */
          hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].channel.band = 
            hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.band;

          hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].channel.chan_num = 
            hdrrup.tca_msg.sub_aset[cnt].fwd_channel[cnt2].channel.chan_num;
        }
        else if (hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].ch_config == 
            HDRRUP_REV_CH_CONFIG_CH_SPECIFIED)
        {
          /* Band */
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                         (uint8 *) &hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].channel.band,
                          offset,  HDRRUP_TCA_BAND_CLASS_SIZE );
          offset += HDRRUP_TCA_BAND_CLASS_SIZE;

          if( hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].channel.band >= SYS_BAND_CLASS_MAX )
          {
            HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Invalid rev band %d rvcd in TCAM",
                            hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].channel.band);
            err |= E_NO_DATA;
          }

          /* Channel */
          err |= (int) HDRBIT_UNPACK16( item_ptr, 
                                       &hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].channel.chan_num,
                                       offset,  HDRRUP_TCA_CHANNEL_NUMBER_SIZE );
          offset += HDRRUP_TCA_CHANNEL_NUMBER_SIZE;
        }

        if (hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].ch_config != 
            HDRRUP_REV_CH_CONFIG_NO_CH)
        {
          /* ReverseChannelDroppingRank */
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                       &hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt2].drop_rank,
                                       offset,  HDRRUP_TCA_REV_CHANNEL_DROPPING_RANK_SIZE );
          offset += HDRRUP_TCA_REV_CHANNEL_DROPPING_RANK_SIZE;
        }

        if (err != 0)
        {
          /* Unpack error */
          unpack_result = E_PROT_UNKN_MSG;
          break;
        }
      }

      if (err != 0)
      {
        /* something is wrong while unpacking ReverseChannel info */ 
        unpack_result = E_PROT_UNKN_MSG;
        break;
      }

      /* NumSectors of the folllowing record*/
      for( cnt2 = 0; cnt2 < hdrrup.tca_msg.num_sectors; cnt2++ )
      {
        if (err != 0)
        {
          /* something is wrong while unpacking previous sector info
           * skip remaining sectors */ 
          unpack_result = E_PROT_UNKN_MSG;
          break;
        }

        /* PilotInThisSectorIncluded*/
        err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                     &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].pilot_included,
                                     offset,  HDRRUP_TCA_PILOT_IN_THIS_SECTOR_INCL_SIZE );
        offset += HDRRUP_TCA_PILOT_IN_THIS_SECTOR_INCL_SIZE;

        if (err != 0)
        {
          /* something is wrong while unpacking PilotInThisSectorIncluded */
          unpack_result = E_PROT_UNKN_MSG;
          break;
        }
        else if (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].pilot_included)
        {
          /*ForwardChannelIndexThisPilot*/
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                       &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].fwd_chan_index,
                                       offset,  HDRRUP_TCA_FWD_CHANNEL_INDEX_SIZE );
          offset += HDRRUP_TCA_FWD_CHANNEL_INDEX_SIZE;

          /*PilotGroupID*/
          err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                       &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].pilot_group_id,
                                       offset,  HDRRUP_TCA_PILOT_GROUP_ID_SIZE );
          offset += HDRRUP_TCA_PILOT_GROUP_ID_SIZE;

          if (err != 0)
          {
            HDR_MSG_PROT(MSG_LEGACY_ERROR, "Something is wrong before unpacking NumUniqFwdTrafficMACIndices");
            unpack_result = E_PROT_UNKN_MSG;
            break;
          }

          if (hdrrup.tca_msg.sub_aset[cnt].ftc_params.feedback_enabled)
          {
            /*NumUniqueForwardTrafficMACIndices*/
            err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                         &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac,
                                         offset,  HDRRUP_TCA_NUM_UNIQ_FWD_MAC_INDICES_SIZE );
            offset += HDRRUP_TCA_NUM_UNIQ_FWD_MAC_INDICES_SIZE;

            if (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac > 0)
            {
              if (hdrrup.tca_msg.scheduler_tag_incl)
              {
                /* SchedulerTag*/
                err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                             &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].scheduler_tag,
                                             offset,  HDRRUP_TCA_SCHEDULER_TAG_SIZE );
                offset += HDRRUP_TCA_SCHEDULER_TAG_SIZE;
              }
              else
              {
                /* Set it to 0xFE for searcher*/
                hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].scheduler_tag = HDRSRCH_NOT_INCLUDED_SCHDTAG;
              }

              /*AuxDRCCoverIncluded*/
              err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                           &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].aux_drc_cover_incl,
                                           offset,  HDRRUP_TCA_AUX_DRC_COVER_INCLUDED_SIZE );
              offset += HDRRUP_TCA_AUX_DRC_COVER_INCLUDED_SIZE;

              if (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].aux_drc_cover_incl)
              {
                /*AuxDRCCover*/
                err |= (int) HDRBIT_UNPACK8( item_ptr, 
                                             &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].aux_drc_cover,
                                             offset,  HDRRUP_TCA_AUX_DRC_COVER_SIZE );
                offset += HDRRUP_TCA_AUX_DRC_COVER_SIZE;
              }
              else
              {
                /* Set it to 0 to turn off higher rev b rates */
                hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].aux_drc_cover = 0;

                HDR_MSG_PROT(MSG_LEGACY_HIGH, "Aux DRC Cover is not included.");
              }
            }
            else
            {
              if (hdrrup.tca_msg.scheduler_tag_incl)
              {
                /* In later phase, num_unique_ftc_mac could be 0
                 * Set it to NOT_USED for searcher*/
                hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].scheduler_tag = HDRSRCH_NOT_USED_SCHDTAG;
              }
              else
              {
                /* Set it to 0xFE for searcher*/
                hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].scheduler_tag = HDRSRCH_NOT_INCLUDED_SCHDTAG;
              }

              /* Per SRCH request, if NumUniqueForwardTrafficMACIndices = 0, set ftc_mac_index to 0 */
              hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_params[0].ftc_mac_index = 0;
            }

            if (err != 0)
            {
              /* something is wrong before unpacking ftc mac info  */
              unpack_result = E_PROT_UNKN_MSG;
              break;
            }

            /* ForwardTrafficMACIndexPerInterlaceEnabled*/
            if (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac == 1)
            {
              err |= (int) HDRBIT_UNPACK8( item_ptr, 
                            &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_mac_index_interlace_enabled,
                            offset,  HDRRUP_TCA_FWD_MAC_INDEX_INTERLACE_ENABLED_SIZE );
              offset += HDRRUP_TCA_FWD_MAC_INDEX_INTERLACE_ENABLED_SIZE;
              if (err != 0)
              {
                /* something is wrong before unpacking ftc mac info  */
                unpack_result = E_PROT_UNKN_MSG;
                break;
              }
            }
            else
            {
              hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_mac_index_interlace_enabled = 0;
            }

            /* Sanity check */
            if ( hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac >
                 HDRRUP_UNIQUE_FTCMAC_INDICES_MAX_SIZE )
            {
              HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                        "NumUniqueForwardTrafficMACIndices (%d) is invalid", 
                        hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac );
              unpack_result = E_PROT_UNKN_MSG;
              break;
            }

            for ( cnt3 = 0; cnt3 < hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac; cnt3++)
            {
              if (err != 0)
              {
                /* something is wrong while unpacking previous sector info  
                 * skip remaining sectors */ 
                unpack_result = E_PROT_UNKN_MSG;
                break;
              }
              /* ForwardTrafficMACIndex*/
              err |= (int) HDRBIT_UNPACK16( item_ptr, 
                            &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_params[cnt3].ftc_mac_index,
                            offset,  HDRRUP_TCA_FWD_MAC_INDEX_SIZE );
			  /* Check for MAC Index Zero */
			  if(hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_params[cnt3].ftc_mac_index == 0)
			  {	
				err |= E_PROT_UNKN_MSG;
				HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
								"TCA. Invalid MAC Index for sub_aset[%d] sector[%d] ftc_par[%d]", 
								cnt,cnt2,cnt3);
              }
              offset += HDRRUP_TCA_FWD_MAC_INDEX_SIZE;

              /* AssignedInterlaces - shall not be true in phase1 */
              if ((hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_mac_index_interlace_enabled == 1) ||
                  (hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].num_unique_ftc_mac > 1))
              {
                err |= (int) HDRBIT_UNPACK8( item_ptr, 
                             &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].ftc_params[cnt3].assigned_interlaces,
                             offset,  HDRRUP_TCA_ASSIGNED_INTERLACES_SIZE );
                offset += HDRRUP_TCA_ASSIGNED_INTERLACES_SIZE;
              }
            }
          } /* end of feedback_enabled*/
          else
          {
            /* TODO later phase: FeedbackEnabled == 0*/
            /* Set default value for each field if needed (for example ForwardTrafficMACIndex)*/
          }

          if (err != 0)
          {
            HDR_MSG_PROT(MSG_LEGACY_ERROR, "Something is wrong before unpacking rev Channel info");
            unpack_result = E_PROT_UNKN_MSG;
            break;
          }

          for (cnt3 = 0; cnt3 < hdrrup.tca_msg.sub_aset[cnt].num_rev_channels; cnt3++)
          {
            if (hdrrup.tca_msg.sub_aset[cnt].rev_channel[cnt3].ch_config != HDRRUP_REV_CH_CONFIG_NO_CH)
            {
              /* ReverseLinkMACIndex */
              err |= (int) HDRBIT_UNPACK16( item_ptr, 
                           &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].rev_mac_index,
                           offset,  HDRRUP_TCA_REV_MAC_INDEX_SIZE );
              offset += HDRRUP_TCA_REV_MAC_INDEX_SIZE;

              /* RABMACIndex */
              err |= (int) HDRBIT_UNPACK8( item_ptr, 
                           &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].rab_mac_index,
                           offset,  HDRRUP_TCA_RAB_MAC_INDEX_SIZE );
              offset += HDRRUP_TCA_RAB_MAC_INDEX_SIZE;

              /* DeltaT2P */
              err |= (int) HDRBIT_UNPACK8( item_ptr, 
                           (uint8 *) &hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].delta_t2p,
                           offset,  HDRRUP_TCA_DELTA_T2P_SIZE );
              offset += HDRRUP_TCA_DELTA_T2P_SIZE;

              /* DeltaT2P is a 6 bit value that requires sign extension */
              hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].delta_t2p= 
                HDRRUP_EXTEND6(hdrrup.tca_msg.sub_aset[cnt].sector_params[cnt2].rtc_params[cnt3].delta_t2p);
            }
            else
            {
              /* TODO. later phase, set default value 
               * srch/rmac/fmac shouldn't use this field if reverseChannelConfiguration is set to '00'
               * TODO: will srch/rmac/fmac know reverseChannelConfiguration? 
               *       should protocol put some predefined value to indicate it's invalid?
               */
            }
          } /* end of cnt3 loop */
        } /* end of pilot_included*/
      }/* End of NumSectors (cnt2)loop*/
    } /* End of SubActive Set Parameters*/
  } while ( 0 );

  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "unpack tca result: %d", unpack_result);
  return unpack_result;

} /* hdrrup_unpack_mcrup_tca_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_PROCESS_RR_MSG

DESCRIPTION
  This function processes the reset report message.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrrup_process_rr_msg( void )
{
  /*-------------------------------------------------------------------------
  Inform lmac that the ResetReport message has been received
  -------------------------------------------------------------------------*/
  hdrlmac_pilot_report_reset( HDRHAI_ROUTE_UPDATE_PROTOCOL );

  /* Log that we received a reset report message */
  hdrplog_inc_conn_state_stats_counter( HDRPLOG_RESET_REPORT_MESSAGE_COUNT, 
                                        1 );
} /* hdrrup_process_rr_msg() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_PROCESS_NL_MSG

DESCRIPTION
  This function processes the Neighborlist message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrrup_process_nl_msg
(
  dsm_item_type *item_ptr
)
{
  hdrsrch_nset_pilot_type nset[HDRRUP_MAX_NEIGHBORS]; /* Neighborlist */
  uint8  num_neighbors;                 /* Number of Neighbors in List */
  /* number of avoided channels returned by SD*/
  uint8 num_avoided_channels = 0;

  /* Current avoided channels returned by SD*/
  sys_avoided_channel_record_type avoided_chan_rec[HDRSRCH_AVOIDED_CHAN_MAX_SIZE];       
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize  local varariables */
  memset(avoided_chan_rec, 0, 
         (sizeof(sys_avoided_channel_record_type) * HDRSRCH_AVOIDED_CHAN_MAX_SIZE));

  if( hdrrup_unpack_nl_msg(item_ptr, nset, &num_neighbors) == (int) E_SUCCESS )
  {
    /*------------------------------------------------------------------------
    Remove channel record(s) with invalid band class
    ------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
    num_avoided_channels = hdrovhd_update_sys_avoid_list(
        HDRSRCH_AVOIDED_CHAN_MAX_SIZE,
        avoided_chan_rec);
#else
    sd_misc_sys_avoid_list(SYS_SYS_MODE_HDR, avoided_chan_rec, 
                           &num_avoided_channels, HDRSRCH_AVOIDED_CHAN_MAX_SIZE);
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

	if(num_avoided_channels > HDRSRCH_AVOIDED_CHAN_MAX_SIZE)
	{
	    HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "num_avoided_channels is %d greater than max allowed. limiting to max %d.",num_avoided_channels, HDRSRCH_AVOIDED_CHAN_MAX_SIZE);  
		num_avoided_channels = HDRSRCH_AVOIDED_CHAN_MAX_SIZE;
	}
	
    (void) hdrutil_remove_invalid_neighbors( nset, &num_neighbors,
                                      avoided_chan_rec, num_avoided_channels );

    /*-----------------------------------------------------------------------
    Update the searcher neighbor list.
    -----------------------------------------------------------------------*/
    hdrlmac_nset_update( num_neighbors, 
                         nset, 
                         0, 
                         NULL, /* no channel list */
                         0, 
                         NULL, /* don't bother to update cached sectors, either */
                         num_avoided_channels, 
                         avoided_chan_rec, /* avoided channel list */
                         HDRSRCH_NSET_UPDATE_DIRECTED, 
                         HDRHAI_ROUTE_UPDATE_PROTOCOL,
                         HDRSRCH_NOT_INCLUDED_PILOT_GID);       
       /* The PilotGroupID of the serving pilot is only valid if nset update 
          is triggered by the SP message. */

  }
  else
  {
    HDR_MSG_PROT (MSG_LEGACY_ERROR, "Ignore invalid NL msg");
  }

  /* Keep track of the the number of neighbor list messages for logging
     purposes. */   
  hdrplog_inc_conn_state_stats_counter( HDRPLOG_NEIGHBOR_LIST_MESSAGE_COUNT,
                                       1 );

} /* hdrrup_process_nl_msg() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_UNPACK_NL_MSG

DESCRIPTION
  This function unpacks the Neighborlist message.

DEPENDENCIES
  nset_ptr - Must point to a valid array of hdrsrch_nset_pilot_type

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  nset_ptr - Neighorlist
  num_neighbors_ptr - Pointer tonumber of neighbors in the list

RETURN VALUE
  E_SUCCESS - If unpacking is successful
  E_NOT_ALLOWED - If incorrect state
  E_NO_DATA - If unpacking fails

SIDE EFFECTS
  None

===========================================================================*/
LOCAL hdrerrno_enum_type hdrrup_unpack_nl_msg
(
  dsm_item_type *item_ptr,
  hdrsrch_nset_pilot_type *nset_ptr,
  uint8 *num_neighbors_ptr
)
{
  uint8  neighbor_chan_incl;            /* NeighborChannelIncluded */
  uint8  band;                          /* Band from Channel Record */
  uint8  system_type;                   /* SystemType from Channel Record */
  uint8  neighbor_srch_win_size_incl;   /* SearchWindowSizeIncluded */
  uint8  neighbor_srch_win_offset_incl; /* SearchWindowOffsetIncluded */
  uint8  neighbor_fpdch_supported_incl; /* FPDCHSupportedIncluded */
  uint8  neighbor_pilot_groupid_incl;   /* PilotGroupIDIncluded */
  uint16 offset;                        /* Field Offset from start of msg */
  int    err;                           /* Error value. 0 = E_SUCCESS */
  int    cnt;                           /* Index Counter */
  sys_link_type curr_link;          /* Current link on which msg received */
  hdrrups_state_enum_type rup_state = hdrrups_get_state();
                                    /* Current RUP state */
  hdrerrno_enum_type unpack_result;     /* Return value of unpack NL msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  err = (int) E_SUCCESS;

  if( rup_state != HDRRUPS_CONNECTED )
  {
    ERR("Neighborlist message rcvd. RUP not in connected state!", 0, 0, 0 );
    return E_NOT_ALLOWED;
  }

  /*-------------------------------------------------------------------------
  Get current link
  -------------------------------------------------------------------------*/
  hdrlmac_get_link_info(
              (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID(item_ptr),
              &curr_link );

  /*-------------------------------------------------------------------------
  Unpack the message
  -------------------------------------------------------------------------*/
  err |= (int) HDRRUP_NL_NEIGHBOR_COUNT( item_ptr, num_neighbors_ptr );
  offset = HDRRUP_NL_MSG_ID_SIZE + HDRRUP_NL_NEIGHBOR_COUNT_SIZE;

  /* Initialize PilotGroupID   
   * It will be updated later if PilotGroupID is specified in the message
   */
  for( cnt=0; cnt < *num_neighbors_ptr ; cnt++ )
  {
    nset_ptr[cnt].pilot_gid = HDRSRCH_NOT_INCLUDED_PILOT_GID;
  }

  /* Extract the neighbor list */
  for( cnt=0; cnt < *num_neighbors_ptr ; cnt++ )
  {
    err |= (int) HDRRUP_NL_NEIGHBOR_PN( item_ptr ,
                         &nset_ptr[cnt].link.pilot_pn,
                         offset );

    offset = offset + HDRRUP_NL_NEIGHBOR_PN_SIZE;
  }

  for( cnt=0; cnt < *num_neighbors_ptr ; cnt++ )
  {
    err |= (int) HDRRUP_NL_NEIGHBOR_CHAN_INCL(item_ptr,
                                           &neighbor_chan_incl,
                                           offset);
    offset = offset + HDRRUP_NL_NEIGHBOR_CHAN_INCL_SIZE;

    if( neighbor_chan_incl == 1 )
    {
      /* Extract the channel from the rcvd message */
      err |= (int) HDRRUP_NL_SYSTEM_TYPE( item_ptr , &system_type , offset );
      offset += HDRRUP_NL_SYSTEM_TYPE_SIZE;

      err |= (int) HDRRUP_NL_BAND_CLASS( item_ptr , &band, offset );
      if( (sys_band_class_e_type) band >= SYS_BAND_CLASS_MAX )
      {
        err |= E_NO_DATA;
        ERR("Invalid band:%d rvcd in NL msg", band, 0, 0);
      }
      /* Since there is a direct mapping between the number & enum value no
      conversion function is needed */
      nset_ptr[cnt].link.channel.band  = (sys_band_class_e_type) band ;

      offset += HDRRUP_NL_BAND_CLASS_SIZE;

      err |= (int) HDRRUP_NL_CHANNEL_NUMBER( item_ptr ,
             &nset_ptr[cnt].link.channel.chan_num , offset );
      offset += HDRRUP_NL_CHANNEL_NUMBER_SIZE;

      nset_ptr[cnt].link.mode = (sys_hdr_chan_rec_mode_e_type) system_type;
      
      if( nset_ptr[cnt].link.mode == SYS_HDR_CHAN_REC_MODE_MAX )
      {
        err |= E_NO_DATA;
        ERR("Invalid system_type %d rcvd in NL", system_type, 0, 0);
      }

    }
    else
    {
      /* Update the channel to the current channel */
      nset_ptr[cnt].link.channel.chan_num = curr_link.channel.chan_num;
      nset_ptr[cnt].link.channel.band = curr_link.channel.band;
      /* TODO: IS856 or IS856_FWD_CH*/
      nset_ptr[cnt].link.mode = SYS_HDR_CHAN_REC_MODE_IS856;
    }
  }

  err |= (int) HDRRUP_NL_NEIGHBOR_SRCH_WIN_SIZE_INCL(item_ptr,
                                  &neighbor_srch_win_size_incl,
                                  offset);
  offset += HDRRUP_NL_NEIGHBOR_SRCH_WIN_SIZE_INCL_SIZE;

  for( cnt=0; cnt < *num_neighbors_ptr ; cnt++ )
  {
    if( neighbor_srch_win_size_incl == 1 )
    {
      /* Window size is an enum but is 4 bits in the message,
       * so it can be safely unpacked as a byte
       */
      err |= (int) HDRRUP_NL_NEIGHBOR_SRCH_WINSIZE(item_ptr,
                                 (uint8 *) &nset_ptr[cnt].window_size,
                                  offset);
      offset += HDRRUP_NL_NEIGHBOR_SRCH_WINSIZE_SIZE;
    }
    else
    {
      nset_ptr[cnt].window_size = HDRSRCH_WINSIZE_DEFAULT;
    }
  }

  err |= (int) HDRRUP_NL_NEIGHBOR_SRCH_WIN_OFFSET_INCL(item_ptr,
                                    &neighbor_srch_win_offset_incl,
                                    offset);
  offset += HDRRUP_NL_NEIGHBOR_SRCH_WIN_OFFSET_INCL_SIZE;

  for( cnt=0; cnt < *num_neighbors_ptr ; cnt++ )
  {
    if( neighbor_srch_win_offset_incl == 1 )
    {
      /* Window offset is an enum but is 4 bits in the message,
       * so it can be safely unpacked as a byte
       */
      err |= (int) HDRRUP_NL_NEIGHBOR_SRCH_WIN_OFFSET(item_ptr,
                                   (uint8 *) &nset_ptr[cnt].window_offset,
                                   offset);
      offset += HDRRUP_NL_NEIGHBOR_SRCH_WIN_OFFSET_SIZE;
    }
    else
    {
      nset_ptr[cnt].window_offset = HDRSRCH_WINOFFSET_DEFAULT;
    }
  }

  /* Unpack FPDCHSupportedIncluded and PilotGroupIDIncluded only for MC RUP
   * Even FPDCHSupportedIncluded is also in revA NL, but the AT doesn't support it (
   * Don't need to unpack it
   */
  if ( ( hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL )
           == HDRSCP_MC_RUP 
#ifdef FEATURE_LTE_TO_HDR_OH
         || 
         hdrscp_get_current_subtype( HDRHAI_ROUTE_UPDATE_PROTOCOL ) 
           == HDRSCP_IRAT_MC_RUP
#endif /* FEATURE_LTE_TO_HDR_OH */ 
       ) &&
       err == (int)E_SUCCESS )
  {
    do
    {
      err |= (int) HDRRUP_NL_NEIGHBOR_FPDCH_SUPPORTED_INCL(item_ptr,
                                          &neighbor_fpdch_supported_incl,
                                          offset);
      offset += HDRRUP_NL_NEIGHBOR_FPDCH_SUPPORTED_INCL_SIZE;

      if (err != 0)
      {
        /* This shall be a required field. 
         * It's wrong in the standard(C.S0024-B v2.0) to specify 0 or 1 bit as its length
         */
        HDR_MSG_PROT (MSG_LEGACY_ERROR, "MC RUP: FPDCHSupportedIncluded needs to be included");
        break;
      }

      if (neighbor_fpdch_supported_incl)
      {
        /* Even the AT doesn't support FPDCHSupported field, we can still unpack and ignore 
         * it, then continue unpacking PilotGroupID field. After discussing with system folks,
         * it is decided to just ignore NL message if AN set FPDCHSupportedIncluded to 1
         */
        HDR_MSG_PROT (MSG_LEGACY_ERROR, "MC RUP: FPDCHSupportedIncluded shoud be set to 0");
        err |= E_NO_DATA;
        break;
      }

      for (cnt =0; cnt < *num_neighbors_ptr; cnt++)
      {
        if (err != 0)
        {
          HDR_MSG_PROT (MSG_LEGACY_ERROR, "MC RUP: Error unpacking PilotGroupID");
          break;
        }

        /* Unpack PilotGroupIDIncluded */
        err |= (int) HDRRUP_NL_NEIGHBOR_PILOT_GROUPID_INCL(item_ptr,
                                            &neighbor_pilot_groupid_incl,
                                            offset);
        offset += HDRRUP_NL_NEIGHBOR_PILOT_GROUPID_INCL_SIZE;
      
        if (err != 0)
        {
          HDR_MSG_PROT (MSG_LEGACY_ERROR, "MC RUP: PilotGroupIDIncluded needs to be included");
          break;
        }

        if (nset_ptr[cnt].link.mode == SYS_HDR_CHAN_REC_MODE_IS856 ||
            nset_ptr[cnt].link.mode == SYS_HDR_CHAN_REC_MODE_IS856_FWD_CH)
        {
          if (neighbor_pilot_groupid_incl)
          {
            err |= (int) HDRRUP_NL_NEIGHBOR_PILOT_GROUPID(item_ptr,
                                                (uint8 *) &nset_ptr[cnt].pilot_gid,
                                                offset);
            offset += HDRRUP_NL_NEIGHBOR_PILOT_GROUPID_SIZE;
          }
          else
          {
            /* The implicit specification of the PilotGroupID value is equivalent to
             * explicit specification iwth a value of zero*/
            nset_ptr[cnt].pilot_gid = HDRSRCH_PILOT_GID0;
          }
        }
      } 

    } while ( 0 );
  }

  if (err != 0) 
  {
    unpack_result = E_NO_DATA;
  }
  else
  {
    unpack_result = E_SUCCESS;
  }
  
  return unpack_result;
} /* hdrrup_unpack_nl_msg() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_PROCESS_CONN_SETUP_TIMEOUT

DESCRIPTION
  This function processes the connection setup timeout signal.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_process_conn_setup_timeout( void )
{
  hdrind_ind_data_union_type ind_data;
    /* Indication data */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ind_data.rup_conn_fail_reason = HDRRUP_CONN_FAIL_TIMEOUT;

  if( hdrrups_get_state() != HDRRUPS_CONNECTION_SETUP )
  {
    /*-------------------------------------------------------------------------
    If the Protocol is not in idle state, cause error.
    -------------------------------------------------------------------------*/
    ERR("RUP is not Conn Setup. Ignoring Setup Timeout", 0, 0, 0 );
    return;
  }

  /*---------------------------------------------------------------------------
  Transition to Idle State & do the associated transition processing
  ---------------------------------------------------------------------------*/
  hdrrup_process_conn_setup_to_idle_transition();

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
  {
    hdrrups_change_state( HDRRUPS_INACTIVE );

    /*------------------------------------------------------------------------
    Give indication for exit HDR upon Connection Setup failure
    -------------------------------------------------------------------------*/
    hdrind_give_ind (HDRIND_RUP_EXIT_HDR_MODE, NULL );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdrrups_change_state( HDRRUPS_IDLE );

    /*------------------------------------------------------------------------
    Give indication for Connection Setup failure
    -------------------------------------------------------------------------*/
    hdrind_give_ind( HDRIND_RUP_CONN_SETUP_FAILED, (void*) (&ind_data));
  }

} /* hdrrup_process_conn_setup_timeout() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_MSG_TX_OUTCOME_CB

DESCRIPTION
  This is the callback function that SLP calls with the status for the
  transmission of a RouteUpdate message.

DEPENDENCIES
  None.

PARAMETERS
  data_ptr    - pointer to user data.
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrup_rup_msg_tx_outcome_cb
(
  void          *data_ptr,
  hdrerrno_enum_type  status
)
{

  ASSERT ( data_ptr == NULL );
  hdrrup_process_registration_outcome( status );

} /* hdrrup_rup_msg_tx_outcome_cb() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRRUP_PROCESS_REGISTRATION_OUTCOME

DESCRIPTION
  This is the callback function that SLP calls with the status for the
  transmission of a NULL message on the Access Channel.

DEPENDENCIES
  None

PARAMETERS
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void hdrrup_process_registration_outcome
(
  hdrerrno_enum_type  status
)
{
  if(status == E_SUCCESS)
  {
    /* If msg transmission is successful, no other processing is needed */
    return;
  }

  if (( hdrrups_get_state() != HDRRUPS_IDLE ) ||
      ( hdramps_get_state() == HDRAMP_INACTIVE ) ||
      ( hdridle_is_connecting() ))
  {
    /* RUP is not in Idle state, no ATI available or IDLE is in Connection 
       setup state, thus no processing for callback */
    return;
  }

  /* RUP is still in Idle state, try to re-send the message */
  hdrhmp_send_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                   HDRHAI_AC,
                   FALSE,
                   HDRRUP_NULL_MSG_PRIORITY,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   NULL,
                   hdrrup_rup_msg_tx_outcome_cb,
                   NULL );
} /* hdrrup_process_registration_outcome() */

/*===========================================================================
FUNCTION HDRRUP_GET_CURRENT_TCA_SEQ_NUM

DESCRIPTION
  This function returns the current sequence number for Traffic Channel
  Assignment message.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Current traffic channel assignment message sequence number.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrup_get_current_tca_seq_num( void )
{
  return hdrrup.last_tca_msg_seq;
} /* hdrrup_get_current_tca_seq_num() */

/*===========================================================================
FUNCTION HDRRUP_START_CONN_SETUP_TIMER

DESCRIPTION
  This function starts the connection setup timer

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_start_conn_setup_timer( void )
{
  (void) rex_set_timer( &hdrrup.connection_setup_timer,
                        HDRRUP_CONN_SETUP_TIMER_MSECS );
} /* hdrrup_start_conn_setup_timer() */

/*===========================================================================
FUNCTION HDRRUP_STOP_CONN_SETUP_TIMER

DESCRIPTION
  This function stops the connection setup timer

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_stop_conn_setup_timer( void )
{
  (void) rex_clr_timer( &hdrrup.connection_setup_timer );
} /* hdrrup_stop_conn_setup_timer() */

/*===========================================================================
FUNCTION HDRRUP_START_STATE_TIMER

DESCRIPTION
  This function starts the state timer

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_start_state_timer( void )
{
  (void) rex_set_timer( &hdrrup.state_timer,
                        HDRRUP_PAC_TIMEOUT_TIMER_MSECS );
} /* hdrrup_start_state_timer() */

/*===========================================================================
FUNCTION HDRRUP_STOP_STATE_TIMER

DESCRIPTION
  This function stops the state timer

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_stop_state_timer( void )
{
  (void) rex_clr_timer( &hdrrup.state_timer );
} /* hdrrup_stop_state_timer() */

/*===========================================================================
FUNCTION HDRRUP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Route Update
  protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_process_timer
(
  uint32 timer_id
)
{
  hdrrups_state_enum_type cur_state = hdrrups_get_state(); 
                                      /*current RUP state*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
  Determine which Timer has expired
  -------------------------------------------------------------------------*/

  switch ( timer_id )
  {
    case HDRRUP_STATE_TIMER:
      switch (cur_state)
      {
        case HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP:
         /*------------------------------------------------------------------
          RUP-PAC synchronization timer expired
          -----------------------------------------------------------------*/
          hdrrup_continue_connection_setup(HDRHAI_ROUTE_UPDATE_PROTOCOL);
          break;

        default:
          break;
      }
      break;
    
    case HDRRUP_CONN_SETUP_TIMER:
      /*------------------------------------------------------------------
       Connection Setup timer expired:
       -----------------------------------------------------------------*/
      HDRTRACE_HDRMC_RUP_CONN_SETUP_TIMER_SIG( cur_state );
      hdrrup_process_conn_setup_timeout();
      break;

    case HDRRUP_PILOT_RPT_TIMER:
      /*------------------------------------------------------------------
       Pilot report sanity timer is expired
       -----------------------------------------------------------------*/
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "RUP pilot rpt timeout");

      /* Process any pending RUP requests. */
      if ( hdrrup.rup_req.pending_req != NULL )
      {
        /*------------------------------------------------------------------
          1. If this pending request msg was received in Idle state but now
          AT moves to a different state, ignore the message.
          2. If this pending request msg was received in connected state but
          now AT moves to a different state, ignore the message. This is
          done in hdrrup_process_msg function, thus no checking here.
        ------------------------------------------------------------------*/
        if ( !HDRDSM_GET_TRAFFIC_BIT( hdrrup.rup_req.pending_req ) &&
             ( hdrrups_get_state() != HDRRUPS_IDLE )
           )
        {
          HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                          "Pending RUR received in Idle but curstate=%d)",
                          hdrrups_get_state());
          dsm_free_packet( &hdrrup.rup_req.pending_req );
        }
        else
        {
          hdrrup_process_msg( hdrrup.rup_req.pending_req, HDRHAI_IN_USE );
          hdrrup.rup_req.pending_req = NULL;
            /* Reset the pending_req to NULL. */
        }
      }

      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRRUP_DISABLE_TUNEAWAY_TIMER:
      hdrrup_set_tuneaway_allowed( TRUE );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default :
      ERR("Invalid RUP protocol signal!",0,0,0);
      break;
  }
}  /* hdrrup_process_timer() */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_TIMER_CB

DESCRIPTION
  This function is registered to be called whenever any timer associated with 
  RUP expires.

DEPENDENCIES
  None

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

void hdrrup_timer_cb
(
  uint32 timer_id
)
{
  
  hdrmc_queue_timer(HDRHAI_ROUTE_UPDATE_PROTOCOL, timer_id);

} /* hdrrup_timer_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_IS_TC_ASSIGNED

DESCRIPTION
  This function checks if the AT has been assigned a Reverse Traffic channel
  
DEPENDENCIES
  None

PARAMETERS
  caller - Protocol calling this function 

RETURN VALUE
  TRUE - If RTC has been assigned, FALSE otherwise

SIDE EFFECTS
===========================================================================*/

boolean hdrrup_is_tc_assigned
(
  hdrhai_protocol_name_enum_type caller 
)
{
  hdrrups_state_enum_type current_state = hdrrups_get_state();
  boolean tc_is_assigned = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

  if( current_state == HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP  ||
      current_state == HDRRUPS_CONNECTION_SETUP ||
      current_state == HDRRUPS_CONNECTED )
  {
    tc_is_assigned = TRUE;
  }

  return tc_is_assigned;
} /* hdrrup_is_tc_assigned() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_GET_TRAFFIC_MODE

DESCRIPTION
  This function gets the HDR traffic mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  traffic_mode - The HDRSRCH traffic mode.
  
SIDE EFFECTS
  None.
===========================================================================*/
hdrsrch_tc_mode_enum_type hdrrup_get_traffic_mode (void)
{

  hdrsrch_tc_mode_enum_type traffic_mode;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_HDR_ACCESS_HYBRID_CTRL
  traffic_mode = hdrrup.traffic_mode;
#else
  traffic_mode = hdrlmac_get_tc_mode();
#endif /* !FEATURE_HDR_ACCESS_HYBRID_CTRL */

#ifdef FEATURE_LTE_TO_HDR_OH
  if (rex_get_timer(&hdrrup.disable_tuneaway_timer))
  {
    traffic_mode = HDRSRCH_S1XHDR_HP_TC_MODE;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "HDRRUP: Traffic mode (%d)", traffic_mode);

  return traffic_mode;

} /* hdrrup_get_traffic_mode() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_SEND_CMD_MODE

DESCRIPTION
  This function sets the HDR traffic mode or tc handdown cmd.

DEPENDENCIES
  None.

PARAMETERS
  traffic_mode - The HDRSRCH traffic mode to allow applications to tweak,
                 for example, specifics of SHDR operation while connected.
				 0 for HDRRUP_SET_TC_HANDDOWN_MODE_CMD
  handdown_modehad - hdrsrch_tc_handdown_mode_enum_type for HDRRUP_SET_TC_HANDDOWN_MODE_CMD
                     0 for HDRRUP_SET_TRAFFIC_MODE_CMD
  sender - The protocol giving this command (only for software trace)
  cmd_name - HDRRUP_SET_TRAFFIC_MODE_CMD
             HDRRUP_SET_TC_HANDDOWN_MODE_CMD
             

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_send_cmd_mode
( 
  hdrsrch_tc_mode_enum_type traffic_mode,
  hdrsrch_tc_handdown_mode_enum_type handdown_mode,
  hdrhai_protocol_name_enum_type sender,
  hdrrup_cmd_name_enum_type cmd_name
)
{
  hdrrup_cmd_type hdrrup_cmd;             /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the rup_cmd_type for the command
  ---------------------------------------------------------------------------*/
  hdrrup_cmd.name = cmd_name;
  hdrrup_cmd.sender = sender;
  if (cmd_name == HDRRUP_SET_TRAFFIC_MODE_CMD)
     hdrrup_cmd.traffic_mode = traffic_mode;
  else if (cmd_name == HDRRUP_SET_TC_HANDDOWN_MODE_CMD)
           hdrrup_cmd.handdown_mode = handdown_mode;

  if (rex_self() == HDRRUP_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating RUP, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrrup_process_cmd( &hdrrup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol.
    -------------------------------------------------------------------------*/
    hdrmc_queue_cmd( HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrup_cmd,
                     sizeof(hdrrup_cmd_type) );
  }

} /* hdrrup_send_cmd_mode() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_SET_TRAFFIC_MODE

DESCRIPTION
  This function sets the HDR traffic mode.

DEPENDENCIES
  None.

PARAMETERS
  traffic_mode - The HDRSRCH traffic mode to allow applications to tweak,
                 for example, specifics of SHDR operation while connected.
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_set_traffic_mode
( 
  hdrsrch_tc_mode_enum_type traffic_mode,
  hdrhai_protocol_name_enum_type sender
)
{
  hdrrup_send_cmd_mode(traffic_mode, 0, sender, HDRRUP_SET_TRAFFIC_MODE_CMD);
} /* hdrrup_set_traffic_mode() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRRUP_SET_TC_HANDDOWN_MODE

DESCRIPTION
DESCRIPTION  : This function sets the handdown mode during HDR connected
               state. Handdown can be enabled or disabled during HDR 
               traffic connection.

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrrup_set_tc_handdown_mode
( 
  hdrsrch_tc_handdown_mode_enum_type handdown_mode,
    /* the handdown mode selected */
  hdrhai_protocol_name_enum_type sender
)
{
  hdrrup_send_cmd_mode(0, handdown_mode, sender, HDRRUP_SET_TC_HANDDOWN_MODE_CMD);
} /* hdrrup_set_tc_handdown_mode() */



/* <EJECT> */

/*===========================================================================

FUNCTION HDRRUP_SEND_RUP_MSG

DESCRIPTION
  This function sends a Route Update message.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrrup_send_rup_msg( void )
{
  hdrrups_state_enum_type current_state = hdrrups_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( current_state == HDRRUPS_IDLE )
  {
    hdrhmp_send_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                     HDRHAI_AC,
                     FALSE,
                     HDRRUP_NULL_MSG_PRIORITY,
                     HDRHAI_IN_USE, /* Instance of protocol sending msg */
                     NULL,
                     hdrrup_rup_msg_tx_outcome_cb,
                     NULL );
  }
  else if( current_state == HDRRUPS_CONNECTED )
  {
    hdrrup_process_send_pilot_report_ind();
  }

} /* hdrrup_send_rup_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_PROCESS_RUP_REQ_MSG

DESCRIPTION
  This function responds to a RouteUpdateRequest message. If the current 
  state is IDLE send the RouteUpdate over the Access Channel; otherwise,
  send it over the Reverse Traffic Channel.
  
DEPENDENCIES
  None.

PARAMETERS
  item_ptr - The DSM item containing the message to be parsed. Although 
    RouteUpdateRequest does not support the ChannelIncluded field the 
    parsing of the message is still being done.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_process_rup_req_msg
(
  dsm_item_type *item_ptr
)
{
  uint8  sector_count = 0;
  uint16 field_offset = 0;
  hdrerrno_enum_type err_code = E_SUCCESS;

  /* Retrieve the current SupportRouteUpdateEnhancements attrib */
  const uint8 support_rup_enhancements = hdrrupc_get_supp_rup_enh_attrib();

  int     i;     /* Loop counter */
  uint8   temp;  /* temporary storage for message fields */
  hdrsrch_link_id_type  link_id;
  sys_link_type         link;
          /* For comparing the included channel with serving channel */
  hdrcsna_1x_search_params_type   srch_1x_params;
          /* 1x parameters from 3G1XParameters message */

  /* When processing the RouteUpdateRequest message, three possible
   * results. */
  enum hdrrup_proc_rur_status_enum_type
  {
    HDRRUP_STOP_PROCESSING    = 0, /* Message is invalid, stop processing */
    HDRRUP_SEND_CUR_RUP_MSG   = 1, /* Send the RUP message on current chan */
    HDRRUP_SEND_SRCH_RUP_REQ  = 2  /* Send RUP Req to Search for OFS */

  } next_action = HDRRUP_STOP_PROCESSING;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Use do while for handling error cases */
  do
  {
    /* Discard the RUP request message if unsupported */
    if( support_rup_enhancements == 0x00 )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "SRUE not neg for RUP req msgs" );
      next_action = HDRRUP_STOP_PROCESSING;
      break;
    }
  
    /* Clear the variable */
    memset( &hdrrup.rup_req.req, 0, sizeof(hdrrup.rup_req.req) );
  
    /* Advance the field value's offset by 8 (8 bit MessageID field) */ 
    field_offset += 8;
  
    err_code |= HDRBIT_UNPACK8( item_ptr, 
                                &(temp), 
                                field_offset, 1 );
                                
    hdrrup.rup_req.req.channel_included = ( boolean ) temp;
    
    field_offset += 1;
  
    if( err_code )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                    "Error unpacking fields in RUP req msg" );
      next_action = HDRRUP_STOP_PROCESSING;
      break;
    }
  
    if( !hdrrup.rup_req.req.channel_included )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "RUR: channels not included" );
      next_action = HDRRUP_SEND_CUR_RUP_MSG;
      break;
    }
    else
    {
      /* If the attrib has not been negotiated for ChannelIncluded fields */
      if( support_rup_enhancements == 0x01 )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "RUR: Ignoring channels because SRUE == 0x01" );
        next_action = HDRRUP_SEND_CUR_RUP_MSG;
        break;
      }

      err_code |= HDRBIT_UNPACK8( item_ptr, 
                                  &(temp), 
                                  field_offset, 8 );
      if ( err_code ||
           temp > HDRSRCH_SYSTYPE_1X )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "Error unpacking fields in RUP req msg" );
        next_action = HDRRUP_STOP_PROCESSING;
        break;
      }
      hdrrup.rup_req.req.system_type = ( hdrsrch_system_enum_type ) temp;
      
      field_offset += 8;

      HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "RUR: SRUE=%d, sys_type=%d",
                     support_rup_enhancements, 
                     hdrrup.rup_req.req.system_type);

      if ( hdrrups_get_state() == HDRRUPS_IDLE )
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, "RUP is in idle state");
        next_action = HDRRUP_SEND_CUR_RUP_MSG;
        break;
      }

      /* AN shall set the SystemType field of this record to 0x00 if
       * SupportRouteUpdateEnhancements is equal to 0x02, or set the
       * SystemType field of this record to 0x00 or 0x01 if 
       * SupportRouteUpdateEnhancements is equal to 0x03.
       * Note that SystemType 0x02 is not supported for Rev A.
       */
      if (((support_rup_enhancements == 0x02) &&
           (hdrrup.rup_req.req.system_type == 0x00))
          ||
          ((support_rup_enhancements == 0x03) &&
           (hdrrup.rup_req.req.system_type == 0x00 ||
            hdrrup.rup_req.req.system_type == 0x01   )))
      {
        /* Valid configuration for OFS */
      }
      else
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "RUR: sys_type & SRUE don't match");
        next_action = HDRRUP_SEND_CUR_RUP_MSG;
        break;
      }
  
      err_code |= HDRBIT_UNPACK8( item_ptr, 
                                (uint8 *) &(hdrrup.rup_req.req.channel.band), 
                                  field_offset, 5 );
      field_offset += 5;
    
      err_code |= HDRBIT_UNPACK16( item_ptr, 
                           (uint16 *) &(hdrrup.rup_req.req.channel.chan_num),
                                   field_offset, 11 );
      field_offset += 11;
  
      if( err_code )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "Error unpacking fields in RUP req msg" );
        next_action = HDRRUP_STOP_PROCESSING;
        break;
      }

      /*-------------------------------------------------------------------
       Check if AT supports the band included in the RouteUpdate Request. 
       If not supported, don't pass the Route Update Request to Search. 
       Instead, just send the RouteUpdate message with current channel 
       information.
      -------------------------------------------------------------------*/
      if ( /* if bandclass not supported OR */
           (!hdrutil_is_band_supported(hdrrup.rup_req.req.channel.band)) ||
           /* if channel not in band OR */
           (!hdrrup_is_band_chan_supported(&hdrrup.rup_req.req.channel)) )

      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "RUR band %d not supported",
                        (uint32) hdrrup.rup_req.req.channel.band );
        next_action = HDRRUP_SEND_CUR_RUP_MSG;
        break;
      }

      /*-------------------------------------------------------------------
      Get the link_id for received message. Compare the included channel
      with the serving channel. If they are the same, just send the report
      for current channel.
      -------------------------------------------------------------------*/
      link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );
  
      /* Call LMAC function to get the HDR channel associated with this 
       * link_id.*/
      hdrlmac_get_link_info( link_id, &link );
  
      if (SYS_CHAN_CMP_EQUAL( link.channel, hdrrup.rup_req.req.channel ))
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "RUR: channel included same as serving channel" );
        next_action = HDRRUP_SEND_CUR_RUP_MSG;
              /* send reports for current channel */
        break;
      }
  
      err_code |= HDRBIT_UNPACK8( item_ptr, 
                                  &(hdrrup.rup_req.req.sector_count), 
                                  field_offset, 4 );
      field_offset += 4;
  
      sector_count = hdrrup.rup_req.req.sector_count;
        /* copy to local variable */
        
      for( i = 0; i < sector_count; i++ )
      {
        err_code |= HDRBIT_UNPACK16( item_ptr, 
                               &(hdrrup.rup_req.req.sector_info[i].pilot_pn),
                                     field_offset, 9 );
        field_offset += 9;
      }
  
      if (  sector_count != 0 )
      {
        err_code |= HDRBIT_UNPACK8( item_ptr, 
                                    &(temp),
                                    field_offset, 1 );
                                    
        hdrrup.rup_req.req.window_size_included = ( boolean ) temp;
        
        field_offset += 1;
      }
      else
      {
        hdrrup.rup_req.req.window_size_included = FALSE;
      }
  
      if( hdrrup.rup_req.req.window_size_included )
      {
        for( i = 0; i < sector_count; i++ )
        {
          err_code |= HDRBIT_UNPACK8( item_ptr, 
                  (uint8 *) &(hdrrup.rup_req.req.sector_info[i].window_size),
                                      field_offset, 4 );
          field_offset += 4;
        }
      }
  
      if (  sector_count != 0 )
      {
        err_code |= HDRBIT_UNPACK8( item_ptr, 
                                    &(temp), 
                                      field_offset, 1 );
                                    
        hdrrup.rup_req.req.window_offset_included = ( boolean ) temp;
        
        field_offset += 1;
      }
      else
      {
        hdrrup.rup_req.req.window_offset_included = FALSE;
      }
  
      if( hdrrup.rup_req.req.window_offset_included )
      {
        for( i = 0; i < sector_count; i++ )
        {
          err_code |= HDRBIT_UNPACK8( item_ptr, 
                (uint8 *) &(hdrrup.rup_req.req.sector_info[i].window_offset),
                                      field_offset, 3 );
          field_offset += 3;
        }
      }

      if( err_code )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "Error unpacking fields in RUP req msg" );
        next_action = HDRRUP_STOP_PROCESSING;
        break;
      }

      /* If 1x search parameters are available from 3G1XParameters message 
         pass to searcher to be modified later */
      hdrcsna_get_1x_srch_params( &srch_1x_params );
  
      if ( srch_1x_params.pilot_inc_included ||
           srch_1x_params.t_add_included )
      {
        hdrrup.rup_req.req.chan_mgmt_1x_included = TRUE;
      }
      else
      {
        /* If 3G1XParameters is not valid, set 1x Search parameters
         * to default */
        hdrrup.rup_req.req.chan_mgmt_1x_included = FALSE;
      }
  
      if ( srch_1x_params.t_add_included )
      {
        hdrrup.rup_req.req.chan_mgmt_1x.pilot_add = srch_1x_params.t_add;     
      }
      else
      {
        hdrrup.rup_req.req.chan_mgmt_1x.pilot_add = 28;
          /* default pilot add for 1x is -14 dB */
      }
        
      if ( srch_1x_params.pilot_inc_included )
      {
        hdrrup.rup_req.req.chan_mgmt_1x.pilot_inc = srch_1x_params.pilot_inc;
      }
      else
      {
        hdrrup.rup_req.req.chan_mgmt_1x.pilot_inc = 1;
        /* default pilot increment is 1 */
      }

      /* If we reach here, we need to send the RUP Request to Search */
      next_action = HDRRUP_SEND_SRCH_RUP_REQ;

    } /* if hdrrup.rup_req.req.channel_included  */

  } while ( 0 );

  if ( next_action == HDRRUP_STOP_PROCESSING )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Stop processing RUP Req message" );
  }
  else if (next_action == HDRRUP_SEND_CUR_RUP_MSG)
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Send RUP message on current channel" );
    hdrrup_send_rup_msg();
  }
  else /* next_action == HDRRUP_SEND_SRCH_RUP_REQ */
  {
    /* We are about to send Search the RUP request and wait for 
     * its pilot report. Start the timer to wait for pilot report
     * for RUP Request */
    if (hdrrup.rup_req.req.sector_count == 0)
    {      
      /* AN didn't specify the number of neighboring pilots to search, 
       * so AT needs to search the complete search space, i.e., 512 pilots.
       * Search hardware is capable of search 16 pilots every 1 second,
       * depending on the PILOT_INC, it will roughly take Searcher
       * [INT(512/(PILOT_INC * 16)*1.5) + 1] seconds to finish the search
       * and report back to HDRMC. */

      if (hdrrup.rup_req.req.system_type == 0x01) /* if 1x system */
      {
        temp = hdrrup.rup_req.req.chan_mgmt_1x.pilot_inc;
      }
      else  /* DO system*/
      {
        /* system_type is only defined for 0x00 or 0x01 for Do RevA.
           Use the PILOT_INC from hdrrupc.current.srch.pilot_inc */
        temp = hdrrupc_get_pilot_inc();
      }
      temp = (uint8) ((HDRRUP_PILOT_RPT_TOTAL_SRCH_TIME_S / temp)
                       + 1);
    }
    else
    {
      /* AN can include no more than 16 pilots in sector_count field
       * of RouteUpdateRequest message. This will take Searcher one
       * second to finish the searching. with an extra 1 second as a 
       * cushion for sending the pilot report to HDRMC */
      temp = (uint8) HDRRUP_PILOT_RPT_DEFAULT_TIMER_S;
    }
    (void) rex_set_timer( &hdrrup.rup_req.pilot_rpt_timer, 1000UL * 
                   temp );


    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "Send RUR to SRCH, start pilot rpt timer=%d s",
                    temp );

    /* call lmac interface to pass the request */
    hdrlmac_rup_request( &(hdrrup.rup_req.req) );
  }

} /* hdrrup_process_rup_req_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_CONFIGURE

DESCRIPTION
  This function is called by SCP to start AT-initiated configuration.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The calling protocol.

RETURN VALUE
  TRUE if the configuration is required; FALSE if configuration is not
  needed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrup_configure
(
  hdrhai_protocol_name_enum_type sender
)
{

  if( !hdrrupc_config_is_needed() )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "RUP Config is not required" );
    return FALSE;
  }

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "RUP Config is required" );

  return hdrrupc_start_config();

} /* hdrrup_configure() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_SEND_ROUTE_UPDATE

DESCRIPTION
  This function is called to force the AT to send a RouteUpdate message.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_send_route_update
( 
  hdrhai_protocol_name_enum_type sender 
)
{
  hdrrup_cmd_type hdrrup_cmd;             /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the rup_cmd_type for the Close command
  ---------------------------------------------------------------------------*/
  hdrrup_cmd.name = HDRRUP_SEND_RUP_MSG_CMD;
  hdrrup_cmd.sender = sender;

  if (rex_self() == HDRRUP_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating RUP, then this is an
    internal command.
    -------------------------------------------------------------------------*/
    hdrrup_process_cmd( &hdrrup_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the route update protocol.
    -------------------------------------------------------------------------*/
    hdrmc_queue_cmd( HDRHAI_ROUTE_UPDATE_PROTOCOL, &hdrrup_cmd,
                     sizeof(hdrrup_cmd_type) );
  }
} /* hdrrup_send_route_update() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_BAND_PREF_IS_CHANGED

DESCRIPTION
  Determines if the band preference was changed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrup_band_pref_is_changed( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrrupc_band_pref_is_changed();
} /* hdrrup_band_pref_is_changed() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_INCREMENT_TCA_COUNT

DESCRIPTION
  This function increments the number of TCA messages received. The AT will
  process only the final TCA message sent by the AN, and will discard any
  intermediate TCA messages.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_increment_tca_count( void )
{
  hdrrup.tca_count++;
} /* hdrrup_increment_tca_count() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_FIND_TRAFFIC_MC_RUP_INFO

DESCRIPTION
  This function is used to find the mc rup info based on rup msg sequence number
  and it will fill other mc rup info to the pointer passed in

DEPENDENCIES
  None.

PARAMETERS
  mc_rup_info_ptr - Pointer to the  mc_rup_msg_info  

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
hdrerrno_enum_type hdrrup_find_traffic_mc_rup_info
(
  dsm_item_type *msg_ptr,
  hdrrup_mc_info_buffer_type *mc_rup_info_ptr
)
{
  uint8 index;
  hdrerrno_enum_type result = E_FAILURE;

  for (index = 0; index < HDRRUP_RTC_RUP_MSG_BUFS_NUM; index++)
  {
    if ((mc_rup_info_ptr->rup_seq_num == hdrrup.mc_rup_info[index].rup_seq_num) &&
        (hdrrup.mc_rup_info[index].msg_ptr == msg_ptr))

    {
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "passed addr 0x%x orig addr 0x%x", 
                      msg_ptr, hdrrup.mc_rup_info[index].msg_ptr); 
      ASSERT(hdrrup.mc_rup_info[index].available == FALSE);

#ifdef FEATURE_MEMCPY_REMOVAL
      *mc_rup_info_ptr = hdrrup.mc_rup_info[index];
#else
      memcpy(mc_rup_info_ptr, &hdrrup.mc_rup_info[index], sizeof(hdrrup_mc_info_buffer_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      /* Make this buffer available to be used*/
      hdrrup.mc_rup_info[index].available = TRUE;
      hdrrup.mc_rup_info[index].msg_ptr = NULL;
      result = E_SUCCESS;
    }
  }

  return result;
} /* hdrrup_find_traffic_mc_rup_info*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_PACK_BYTE

DESCRIPTION
  This function is called to pack given value into a byte array. It is only
  used internally to pack MC RouteUpdate msg sent on RTC

DEPENDENCIES
  None.

PARAMETERS
  byte_array_ptr  - Pointer to an  array in which  the value will be stored 
  byte_array_index_ptr - array index 
  value - value to be stored in byte array  
  len   - length of value (in bits)
  remaining_bits_ptr  - number of bits can be used to store value (in bits)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_pack_byte
(
  uint8 *byte_array_ptr,
  uint8 *byte_index_ptr,
  uint16 value,
  uint8 len,
  uint8 *remaining_bits_ptr
)
{
  uint8 remaining_field_bits = len;
  uint8 remaining_bits = *remaining_bits_ptr;
  uint8 byte_index = *byte_index_ptr;
  uint16 mask = 0xFFFF; /* max len passed in is 15, so use uint16*/
  /* max len passed in is 15 */
  ASSERT(len < 16);

  do
  {
    if (remaining_field_bits > remaining_bits)
    {
      /* bits need to be packed is greater than the un-used bits of 
       * byte_array[byte_index], need to use next byte(byte_index + 1) to pack
       * unpacked value */

      /* Packing un-used bits(remaining_bits) with "value" into byte_array_ptr[byte_index]*/
      byte_array_ptr[byte_index] |= value >> (remaining_field_bits - remaining_bits);
      remaining_field_bits -= remaining_bits;

      /* new byte needs to be used to packet remaining_field_bits */
      remaining_bits = 8;

      /* removed packed bits, the unpacked bits will be packed into next byte */
      value &= ~((uint16) (((mask >> (16 - len)) >> remaining_field_bits) << remaining_field_bits));
    }
    else
    {
      byte_array_ptr[byte_index] |= value << (remaining_bits - remaining_field_bits);
      remaining_bits -= remaining_field_bits;
      remaining_field_bits = 0;
    }

    HDR_MSG_PROT_4( MSG_LEGACY_LOW, 
                    "byte: index %d, value %d, Remaining bits: fields %d, byte %d", 
                    byte_index, byte_array_ptr[byte_index],
                    remaining_field_bits, remaining_bits);

    /* If remaining bits  is 8, it means we need to use next byte*/
    (remaining_bits == 8) ? byte_index++ : byte_index;

  } while (remaining_field_bits > 0 );

  *remaining_bits_ptr = remaining_bits;
  *byte_index_ptr = byte_index;
}
/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_CREATE_MC_RUP_FIELDS

DESCRIPTION
  This function is called when transmitting MC RUP on RTC. It creates MC RUP 
  fields starting from NumPilots and store them into a byte array.
  Created byte array will be appended to the MC RUP 

  Instead of packing entire MC RUP in hdrrup_form_rup(), this procedure is needed
  because we need to get the latest SLP carrier and tx pwr information from RMAC 
  immediately before MC RUP msg is sending on RTC (after MC RUP is queued to SLP). 

DEPENDENCIES
  MC RUP has been constructed before NumPilots fields in hdrrup_form_rup_msg() 
  and queued to SLP.

PARAMETERS
  byte_array_ptr  - Pointer to an  array in which values to be appended to
                    MC RUP msg will be stored 
  mc_rup_info_ptr - Pointer to the buffer holding MC RUP msg    

RETURN VALUE
  number of byte to be appended to MC RUP msg

SIDE EFFECTS
===========================================================================*/
uint8 hdrrup_create_mc_rup_fields
(
  uint8 *byte_array,
  hdrrup_mc_info_buffer_type *mc_rup_info_ptr
)
{
  sys_channel_type slp_carrier; 
   /* Channel record of the carrier designated to carry the SLP packets */
  int8 tx_pilot_pwr;   
   /* Average TX pilot power from all assigned rev channels*/
  hdrrups_state_enum_type rup_state = hdrrups_get_state();

  hdrrup_rup_msg_mc_field_type mc_field;

  uint8 pilot_index;

  /* variables used to pack byte array*/
  uint8 remaining_bits = 8; /* remaining bits can be packed to particular byte*/
  uint8 byte_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* This function should be called only when RUP is in CONNECTED state*/
  ASSERT(rup_state == HDRRUPS_CONNECTED);

  /* Call RMAC4 for slp carrier and TxPower*/
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "MC RUP: Get SLP carrier and TxPilotPower Info" );
#ifdef FEATURE_HDR_REVB_SUPPORTED
  hdrrmac4_get_slp_carrier_and_tx_pilot_pwr_info(&slp_carrier, &tx_pilot_pwr);
#else
  slp_carrier.band = 0;
  slp_carrier.chan_num = 0;
  tx_pilot_pwr = 0;
#endif /* FEATURE_HDR_REVB_SUPPORTED */
  HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, "MC RUP: SLP Carrier band %d, Ch %d, pwr %d", 
                  slp_carrier.band, slp_carrier.chan_num, tx_pilot_pwr );


  /* ReferencePilotChannelIncluded and ChannelInclude shall be set to 1
   * for MC RUP sent on RTC
   */
  mc_field.ref_pilot_chan_incl = 1;  
  mc_field.tx_pilot_pwr_incl = 1;
  mc_field.tx_pilot_pwr = (uint8) tx_pilot_pwr;
  mc_field.ref_chan_rec.mode = mc_rup_info_ptr->chan_rec.mode; 
  mc_field.ref_chan_rec.channel.band = mc_rup_info_ptr->chan_rec.channel.band; 
  mc_field.ref_chan_rec.channel.chan_num = mc_rup_info_ptr->chan_rec.channel.chan_num; 

  /* Packing byte arrary now*/
  /* Packing NumPilots into byte array*/
  byte_array[byte_index] |= mc_rup_info_ptr->num_pilots << 
                               (remaining_bits - HDRRUP_RUP_NUM_PILOTS_SIZE);
  remaining_bits -= HDRRUP_RUP_NUM_PILOTS_SIZE;

  /* Packing PilotPNPhase, ChannelIncluded, Channel, PilotStrength, Keep */
  for(pilot_index = 0; pilot_index < mc_rup_info_ptr->num_pilots; pilot_index++)
  {
    /*Packing PilotPNPhase */
    hdrrup_pack_byte(byte_array, &byte_index, 
                     mc_rup_info_ptr->pilot[pilot_index].pn_phase,
                     HDRRUP_RUP_PILOT_PN_PHASE_SIZE, &remaining_bits);

    /*Packing ChannelIncluded */
    mc_rup_info_ptr->pilot[pilot_index].chan_incl = 1;

    hdrrup_pack_byte(byte_array, &byte_index, 
                     mc_rup_info_ptr->pilot[pilot_index].chan_incl,
                     HDRRUP_RUP_CHANNEL_INCLUDED_SIZE, &remaining_bits);

    /*Packing Channel */
    hdrrup_pack_byte( byte_array, &byte_index, 
                      (uint16) mc_rup_info_ptr->pilot[pilot_index].chan_rec.mode,
                      HDRRUP_RUP_SYSTEM_TYPE_SIZE, &remaining_bits);
    hdrrup_pack_byte( byte_array, &byte_index, 
                      (uint8) mc_rup_info_ptr->pilot[pilot_index].chan_rec.channel.band,
                      HDRRUP_RUP_BAND_SIZE, &remaining_bits);
    hdrrup_pack_byte( byte_array, &byte_index, 
                      mc_rup_info_ptr->pilot[pilot_index].chan_rec.channel.chan_num,
                      HDRRUP_RUP_CHANNEL_NUMBER_SIZE, &remaining_bits);

    /* Packing PilotStrength*/
    hdrrup_pack_byte( byte_array, &byte_index, 
                      (uint8) mc_rup_info_ptr->pilot[pilot_index].strength,
                      HDRRUP_RUP_PILOT_STRENGTH_SIZE, &remaining_bits);

    /* Packing Keep*/
    hdrrup_pack_byte( byte_array, &byte_index, 
                      mc_rup_info_ptr->pilot[pilot_index].keep,
                      HDRRUP_RUP_KEEP_SIZE, &remaining_bits);

  }

  /* Packing ATTotalPilotTransmissionIncluded*/
  hdrrup_pack_byte( byte_array, &byte_index, 
                    mc_field.tx_pilot_pwr_incl,
                    HDRRUP_RUP_PILOT_TX_PWR_INCL_SIZE, &remaining_bits);

  /* Packing ATTotalPilotTransmission*/
  hdrrup_pack_byte( byte_array, &byte_index, 
                    mc_field.tx_pilot_pwr,
                    HDRRUP_RUP_PILOT_TX_PWR_SIZE, &remaining_bits);

  /* Packing ReferencePilotChannelIncluded*/
  hdrrup_pack_byte( byte_array, &byte_index, 
                    mc_field.ref_pilot_chan_incl,
                    HDRRUP_RUP_REF_PILOT_CH_INCL_SIZE, &remaining_bits);

  /* Packing ReferencePilotChannel*/
  hdrrup_pack_byte( byte_array, &byte_index, 
                    (uint16) mc_field.ref_chan_rec.mode,
                    HDRRUP_RUP_SYSTEM_TYPE_SIZE, &remaining_bits);
  hdrrup_pack_byte( byte_array, &byte_index, 
                    (uint8) mc_field.ref_chan_rec.channel.band,
                    HDRRUP_RUP_BAND_SIZE, &remaining_bits);
  hdrrup_pack_byte( byte_array, &byte_index, 
                    mc_field.ref_chan_rec.channel.chan_num,
                    HDRRUP_RUP_CHANNEL_NUMBER_SIZE, &remaining_bits);

  /* Return total number of bytes (0 ~ byte_index)*/
  return byte_index + 1;

} /* hdrrup_create_mc_rup_fields */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRRUP_UPDATE_MC_RUP_MSG

DESCRIPTION
  This function is called by slp to modify ATTotalPilotTransmission and 
  ReferencePilotChannel fields of MC RUP msg before giving it RMAC.
  We update MC RUP fields when RU msg is in SLP queue to make sure we have  
  latest SLP carrier information. 
  SLP carrier information may change every subframe, if we query RMAC for 
  SLP carrier information in the hdrrup_form_rup_msg(), information may be 
  out of date when message is queued to SLP.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the RouteUpdate message

RETURN VALUE
  None.

SIDE EFFECTS
  DSM item which holds RouteUpdate message may be modified.
===========================================================================*/
void hdrrup_update_mc_rup_msg
(
  dsm_item_type *msg_ptr
)
{
  hdrrup_mc_info_buffer_type mc_rup_info;
    /* Used to store the retrieved hdrrup_mc_infor_buffer_type*/

  uint8 proto_id_size = HDRHMP_HEADER_SIZE * 8;      
    /* size(unit of bits) of protocol id which added to rup msg by hmp*/

  uint8 mc_rup_byte_array[HDRRUP_RUP_MSG_APPENDED_MAX_BYTE_SIZE];
    /* Data to be appended to msg_ptr(MC RUP sent on RTC) starting from NumPilots*/

  uint16 offset;   
  uint8 appended_byte = 0;   
  hdrerrno_enum_type err_number;      
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize mc_rup_byte_array*/
  memset(mc_rup_byte_array, 0, sizeof(mc_rup_byte_array));

  /* Protocol id was added by hmp*/
  offset = HDRRUP_RUP_MSG_ID_SIZE + proto_id_size;  

  /* Get sequence number*/
  err_number = HDRBIT_UNPACK8( msg_ptr, &mc_rup_info.rup_seq_num, offset, HDRRUP_RUP_MSG_SEQ_SIZE);
  ASSERT(err_number == E_SUCCESS);

  /* Based on sequence number, searching hdrrup.mc_rup_info buffer to 
   * retrieve offset and channel info 
   */
  err_number = hdrrup_find_traffic_mc_rup_info(msg_ptr, &mc_rup_info);
  ASSERT(err_number == E_SUCCESS);
  HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "seq %d, ref channel num %d ", 
                  mc_rup_info.rup_seq_num, mc_rup_info.chan_rec.channel.chan_num);

  /* Based on stored mc rup information, generate data to be appended to MC RUP msg*/
  appended_byte = hdrrup_create_mc_rup_fields(mc_rup_byte_array, &mc_rup_info );

  /* Appened fields to MC RUP starting from NumPilots field */
  (void) hdrutil_pushdown_tail(&msg_ptr, mc_rup_byte_array, appended_byte);

} /* hdrrup_update_mc_rup_msg*/

/*===========================================================================

FUNCTION HDRRUP_IS_BAND_CHAN_SUPPORTED

DESCRIPTION
  This function acts as an entry point for the LMAC function 
  hdrlmac_is_band_chan_supported(), which checks if a channel/band
  combo is supported by the RF chipset of the AT.
  
DEPENDENCIES
  None.

PARAMETERS
  channel_ptr - The band class and channel being compared against the 
  supported bands/channel of RF chipset.

RETURN VALUE
  TRUE if the chan/band combo is supported by the RF chipset in use. 
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrup_is_band_chan_supported
(
  sys_channel_type *channel_ptr
)
{
  boolean band_chan_is_supported;
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_LTE_TO_HDR_OH
  if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 "Active handoff no RF band/channel validation");
    return TRUE;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  band_chan_is_supported = hdrlmac_is_band_chan_supported(channel_ptr->band,
                                        (word)channel_ptr->chan_num);
  if(band_chan_is_supported == FALSE)
  {
     HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
         "hdrrup_is_band_chan_supported is FALSE for band/chan %d/%d rvcd in TCA",
          channel_ptr->band, channel_ptr->chan_num);
  }

  return band_chan_is_supported;
} /* hdrrup_band_is_supported() */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRRUP_ACT_HO_COMPLETED

DESCRIPTION
  This function sends ACT_HO_COMPLETED_IND so DS can send data thru
  HDR
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_act_ho_completed( void )
{
    /* Notify DS active HO has completed and HDR is connected */
  hdrind_give_ind( HDRIND_RUP_ACT_HO_COMPLETED, NULL );
}

/*===========================================================================

FUNCTION HDRRUP_SET_TUNEAWAY_ALLOWED

DESCRIPTION
  This function enable/disables tuneaway by setting HPT mode. When
  tuneaway is disallowed, HPT mode is forced, and settings from
  hdrrup_set_traffic_mode() are overriden. When tuneaway is allowed,
  setting from hdrrup_set_traffic_mode() is restored.
   
DEPENDENCIES
  None.

PARAMETERS 
  allowed - if tuneaway is allowed 
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_set_tuneaway_allowed
(
  boolean allowed
)
{
  if (allowed)
  {
    rex_clr_timer(&hdrrup.disable_tuneaway_timer);

    if ( hdrlmac_get_tc_mode() != hdrrup.traffic_mode )
    {
      hdrlmac_set_tc_mode( hdrrup.traffic_mode );
    }    
  }
  else
  {
    if (rex_get_timer(&hdrrup.disable_tuneaway_timer))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "tuneaway already disabled!");
    }

    rex_set_timer(&hdrrup.disable_tuneaway_timer, 
                  HDRRUP_DISABLE_TUNEAWAY_TIME_MS);

    if ( hdrlmac_get_tc_mode() != HDRSRCH_S1XHDR_HP_TC_MODE )
    {
      hdrlmac_set_tc_mode( HDRSRCH_S1XHDR_HP_TC_MODE );
    }
  }  

  HDR_MSG_PROT_2( MSG_LEGACY_MED, "IRAT: tuneaway allowed: %d/%d",
                  allowed, hdrrup.traffic_mode );
}
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

FUNCTION HDRRUP_RESET_CP_SKIP_DISABLE_TX

DESCRIPTION
  This function resets the skip disable RF TX flag 
   
DEPENDENCIES
  None.

PARAMETERS 
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_reset_cp_skip_disable_tx(void)
{
  hdrrup.cp_skip_disable_tx = FALSE;  
} /* hdrrup_reset_cp_skip_disable_tx */

/*===========================================================================

FUNCTION HDRRUP_CP_SKIP_DISABLE_TX_ENABLED

DESCRIPTION
  This function sets the skip disable TX flag, to not to disable RF TX 
   
DEPENDENCIES
  None.

PARAMETERS 

RETURN VALUE
  TRUE - if skip disabling RF TX is allowed.
  FALSE - if skip disabling RF TX is not allowed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrup_cp_skip_disable_tx_enabled(void)
{
  return hdrrup.cp_skip_disable_tx;  
} /* hdrrup_cp_skip_disable_tx_enabled */