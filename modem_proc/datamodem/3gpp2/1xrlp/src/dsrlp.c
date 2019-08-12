/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


  R A D I O   L I N K   P R O T O C O L   E X T E R N A L   F I L E

GENERAL DESCRIPTION
  This file contains functions that implement the external interfaces for
  the RLP modules.
  Function prototypes for the external functions in this file are given in
  dsrlpi.h

  The naming convention in this file is:
  All external functions start with dsrlp..
  There are no internal functions in this file.
  All internal variables (in file scope) start with dsrlp..

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Before calling any DSRLP functions, dsm_init() must have been called
  to initialize the memory pool.

  Here is how interface sequencing will work:
    At the beginning of the RLP session, call:
      dsrlp_init( sr_id, ver) with sr_id and version.
      dsrlp_setup_io( sr_id, tx_wm, rx_wm) with sr_id and the input
         and output watermarks.

   Every 20 ms to transmit a frame Mux laye calls,
     dsrlp_build_tx_frames( ..) - one call per sr_id;

   Every 20 ms after getting a frame from the physical layer, Mux layer calls,
     dsrlp_process_rx_frames(...) - one call per sr_id.

   At the end of the call:
     dsrlp_cleanup( sr_id) - will cleanup memory and data structures.

 Copyright (c) 1999 - 2016 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlp.c_v   1.63   12 Feb 2003 17:30:14   kdevara  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/04/14   nj         Klocwork error fixes.
12/18/13   sd         Added critical sections to avoid race condition issues
10/18/13   sd         Cleaning up Rx WM for HDR connection close.
06/26/13   sc         Replace all the memcpy and memmove with memscpy and 
                      memsmove as its safer.
06/18/13   sd         Removed multiple critical sections for dsrlpi_reset.
04/26/13   sd         Fixed assert failure for RLP state crash.
09/12/12   sd         Memory Reduction modification
04/03/12   msh        dsrlpi_reset called in data path crit sect     
03/14/12   msh        Initialize the queues only once to avoid memory leak
01/26/12   msh        Coan feature cleanup    
11/18/11   vpk        HDR SU API cleanup
04/15/11   op         1X API Cleanup
03/21/11   ack        Init Crit sect variable before usage
03/04/11   jee        Merged compiler warning fixes
02/09/11   ack        Added is_1xrlp_ready flag to chk for 1X RLP init state
02/02/11   ack        Added DS_RLP_INIT_CMD
02/04/11   op         Data SU API cleanup
01/18/11   op         Migrated to MSG 2.0 Macros
11/22/10   sn         Replaced REX premption macros with critical section for
                      mutual exclusion.
07/07/10   ms         Data statistics support for Field Test Display.
06/28/10   rp         Compiler warning fixes.
06/25/10   gc         Fixed CMI violation for clk.h
06/17/10   ms         Fix RLP Partial logging Crash issue on Q6.
04/26/10   gc         Lint fixes
04/16/10   ls         Fix the featurization issues
03/30/10   ss         Removing memset of the RLP control block in dsrlp_init()
                      which is causing memory leak of the queue mutex ptr.
12/17/09   ms         Compiler Warning fix.
09/18/09   ss         Fixed Klocwork errors.
04/13/09   ls         Initialize enable_inactivity_timer when dsrlp_init()
                      is called
02/27/09   spn        Fixed Lint Error
07/24/08   spn        Update the phys_layer_enabled parameter when it is
                      GAUPed without deactivating RLP
07/22/08   spn        Update the NAK Delay Timer change because of a GAUP
08/16/07   spn        Lint changes.
03/20/07   ac         Fix the compiling error when HDR is turned off
12/21/06   spn        Integration of the SPD module into dsrlp.
07/25/06   ksu        EMPA: 1x RLP code review comments
07/20/06   kvd        7500 re-arch changes - resides on modem proc.
06/06/06   ksu        EMPA: RLP Rx / Sig code review comments
05/30/06   ksu        EMPA: RLP init code review comments
05/11/06   squ        In dsrlp_build_tx_frames function, check if there is a
                      pending ack to send.
05/04/06   ksu        EMPA: RSP part 2
04/17/06   squ        In the 20 ms tx frame, added code to check ack_to_send
                      flag in case a pending ack need to send.
01/20/06   ksu        EMPA: link up hdrmrlpcif to hdrmrlpc
01/17/06   ksu/sy     Set the flush timer value for DPA.
01/03/06   ksu        EMPA: hook up forward link with new phys link architecture
12/23/05   ksu        EMPA RSP: two fwd rx queues
12/15/05   TMR        Changed RLP RX queue to watermark and added parameter to
                      dsrlp_reg_srvc() and RX callback function.
12/12/05   ksu/sy     Added runtime checks for dsrlpdel functions as it is
                      used by 1x only.
12/12/05   ksu/sy     Introduced a critical section between RX and TX task to
                      prevent RX task from resetting the RLP while the
                      the TX task build the packets and waiting for a
                      response from PCP.
12/04/05   ksu        EMPA segment sequencing: don't fragment for Format B
11/22/05   ksu        EMPA packet stream: segment sequencing (forward link only)
11/21/05   ksu        EMPA packet stream: out of order delivery to route protocol forward
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)
11/03/05   ksu        EMPA: packet stream, octet sequencing (reverse link only)
10/27/05   ksu        EMPA: octet stream, octet sequencing
10/10/05   ksu/sy     Eliminate hdrmrlpi_rx_process_nak()
09/30/05   ksu/sy     Eliminate hdrmrlp_get_new_or_flush_data() and
                      hdrmrlp_get_retx_data()
09/28/05   ksu/sy     HDR: use one-shot abort timer instead of 20ms ticks.
09/28/05   ksu/sy     Call hdr[m]rlpi_rx_process_sig_msg() directly from hdr[m]rlp_msg_cb().
09/01/05   ksu/sy     Only perform 1x specific initialization when ver=RLP3.
08/29/05   ksu/sy     Remove dsrlp_reg_hdlc() in favor of uint32 app_field
                      parameter to dsrlp_reg_srvc().
08/29/05   ksu        Move tx_item_ptr cleanup from dsrlpi_reset() to
                      dsrlpi_cleanup() to prevent loss of unsent data.
09/26/05   vas        Check for invalid rscb_index before TIMETEST logging.
08/08/05   sk/sy      Merged semi-dynamic QOS changes from sandbox.
06/10/05   sk         Added code to check if RLP is initialized before cleaning up RLP Stats
06/10/05   sk         Removed hdrmrlpi_startup() function call from this file
02/05/05   rsl        Moved cta timer start and stop on tx side to dsrlp.c from ds707_rmsm.c
11/29/04   gr         Merged PZID hysteresis changes into main line
11/22/04   vas        Added dsrlp_get_free_rscb_index(). Changed functions to
                      get negotiated MRLP parameters using new APIs
11/08/04   vas        Updated interface of MRLP get config
09/20/04   vas        Merged QOS related changes from September development branch.
09/08/04   vr         Added functionality to notify the pzid hysteresis
                      engine about the first ppp data exchange
07/12/04   gr         Added functionality to update the contiguous tx and rx
                      idle frame count. Also modified the fill rate to be a
                      variable that can be set/retrieved via IOCTL.
08/31/04   atp        Added print msg for DDW and REXMIT.
07/22/04   atp        Revamp of QOS blob as per 707-B.
06/08/04   atp        Support for RLP blob type 2.
05/07/04   atp        First cut at supporting EVDV (IS707-B).
05/07/04   kvd        variable scale for timetest RLP thput logging  based on
                      current data rate to get accurate  thput measurements.
02/19/04   gr         Changed F3 message to add more information for RLP
                      signaling
10/13/03   vas        Changed dsrlp_is_inited() to not return FALSE if ver
                      is HDR. It returns the value based of HDR is initialized
                      hdrrlp_init is called from dsrlp_init.
09/18/03   kvd        Changed timetest Logging msg to MEDIUM from HIGH.
8/25/03    junz       Change the RLP stats Get and Clear functions to fix data
                      aborts due to invalid SR_ID
08/08/03   kvd        Made changes to dsrlp_log_timetest_throughput to log rlp
                      stats for all rlps (currently 1X & HDR)
07/10/03   vas        Added code to check for fixed sr id values for HDR.
05/21/03   vas        Made changes to support Multimode HDRRLP
04/21/03   ik/js      Modifications for quque defeaturization.
04/10/03   sy         Added support for SVD algorithm to
                      dsrlp_any_sig_to_send().
03/18/03   js         Removed the unnecessary return from
                      dsrlp_process_rlp_blob().
03/07/03   js         Added RLP events.
02/12/03   kvd        Added F3 messages indicating the rlp cleanup in
                      dsrlp_cleanup after hdrrlp_cleanup since the F3 msg
                      inside hdrrlp_register_stream_callback is deleted now,
                      as it's inside INTLOCK.
01/31/03   sy         Changed funtion name dsrlp_next_turbo_pdu5_chain() to
                      dsrlp_get_next_dsm_item_chain().
01/24/02   js         Fixed a typographical error.
12/18/02   atp        Implemented change in standard 707 for QOS.
12/18/02   kvd        Modified dsrlp_is_inited  to check for HDr version
12/02/02   snn/sy     dsrlp_insert_pdu5null_hdr_turbo function will insert Null
                      header for the Turbo encoded PDUs.
10/15/02   js         Optimized f3 messages to print out RLP stream identifier.
10/16/02   js         Fixed small typographical error
10/15/02   ak         Added code to call MM DATA control/hold function.
10/03/02   vas        Added function dsrlp_is_hdr()
09/17/02   ak         Added function dsrlp_is_inited() which returns whether
                      DS RLP is initialized. (chked in by js)
08/08/02   js         Fixed minor compile-time warning.
08/07/02   vas        Move initialization of rx_sig_msg_q earlier in
                      dsrlp_init
07/17/02   js         Added dsrlp_next_turbo_pdu5_chain() to be used by MUX
                      to resolve a turbo encoded MUX PDU5 chain of DSM items.
07/15/02   ak         Added featurization under FEATURE_DATA_MM for new
                      architecture.
05/02/02   snn        Added a function dsrlp_eval_qos_blob and included snm.h.
03/18/02   vas        Call hdrrlp_init at the end of dsrlp_init to ensure all
                      RLP initialization is completed before callbacks are
                      registered with hdrrx/hdrpcp (called thru hdrrlp_init).
03/15/02   snn        Added function dsrlp_any_sig_to_send() for SVD.
02/28/02   vas        Changed dsrlp_init prototype to take rscb_index parameter
02/25/02   atp        Moved definition for QOS default from dsrlp.c to dsrlp.h.
02/20/02   snn        Added a message in check_sr_id function if the check fails.
                      This was necessary for sanity check in SVD builds.
02/14/02   hcg        Moved #include of timetest.h for TIMETEST-based RLP throughput
                      logging to prevent compilation problems.
02/04/02   ss         Made changes to use new DSM interface functions for
                      registering/handling memory events.
01/31/02   vr         Removed ASSERT is dsrlp_cleanup
01/31/02   atp        Added support for QOS.
01/28/02   sy         Changed the input to dsrlp_cleanup() from sr_id_index
                      to sr_id to make it consistent with with dsrlp_init().
12/13/01   snn        Fixed code to accept parameters greater than NAK_PER_ROUND
                      to be greater than {1,2,3}. It was always accepting parameters
                      greater than 1,2,3 but recent changes broke it. Fixed it back.
12/06/01   atp        Reworded error msg in dsrlp_build_tx_frames
12/03/01   snn        Added support for RLP options to return negotiated
                      parameters.
11/27/01   snn        Added support for modifying RLP3 nak policy.
10/31/01   vas        Changed DSRLP_FIXED_SR_ID to DSRLP_FIXED_SR_ID_INDEX in
                      function for logging RLP throughput. Change in timetest
                      throughput test to differentiate HDR & 1x call.
10/05/01   ak         SCRM function called in build_tx frames changed its
                      name.
09/21/01   ak         Put a check in build_frames to make sure that dsm_items
                      were freed in txc.
09/16/01   vas        Used rscb_index instead of sr_id to call hdrrlp_cleanup
09/14/01   rama       SR_ID fix. Differentiate between SR_ID and RLP Instance.
                      Set a mapping between SR_ID and RLP Instance.
09/04/01   hcg        Added RLP throughput logging TIMETEST support.
08/17/01   vas        Added call to hdrrlp_init() from dsrlp_init()
08/02/01   ak         If resetting, get out build_tx_frames pronto.  Also
                      initialize build_tx_frames return struct up front.
07/02/01   ak         Always remove from reseq queue at end of process_rx_
                      frames.
06/20/01   vas        Added calls to hdrrlp_startup from dsrlp_startup & to
                      hdrrlp_cleanup from dsrlp_cleanup.
06/13/01   sy         Added extra enum for P2 mode to tell which one of the
                      FCH/DCCH is inactive. This helps in calculating the
                      round trip time correctly.
05/27/01    snn       updated a comment.
05/24/01    hcg       Made changes to support HDR (1xEV).  Updates to support
                      new memory event callback registration mechanism.
05/21/01   sy         Making sure to call reset function if reset flag is set,
                      every 20msec.Added frame type DSRLPI_RESET to free all
                      frames before calling reset function.
05/11/01   ak         Further support for Control/Hold.  Some file cleanup.
                      Added checks to see if wmk ptr and rx queue ptrs were
                      NULL.  Remove limited # of items from reseq queue now.
04/10/01   lcc/ak     Added support for P2 mode.
04/01/01   ak         Only ask for SCRM now when lot of bytes to send.  Dont
                      ask to cancel when zero bytes left.
02/22/01   ak         Made an array which holds the RLP Logging ID for FCH
                      and SCH.
02/21/01   ak         Support for checking rx'ed new frames whose seq < V(S)
02/16/01   igt        New rlp BLOB processing: if during a call will force
                      a dsrlpi_reset() and also go through sync/ack procedures.
02/05/01   ak         Age NAK list only on MUX Null frames.  No longer age
                      NAKs on erasures.  Now remove ack'd frames from TXQ in
                      TX context.
01/15/01   igt        Added code to keep track of how much time spent to
                      sync by updating sync_delay and timing out.
12/07/00   ak         Do not age NAK timers on rexmits.  Only new data and
                      Idles.
12/06/00   ak         Changes to force FILL frames every 80 frames so that
                      other side gets our V(N) appropriately.
11/29/00   ak         Handle new DSRLP_RATE_ERASURE_SCH, due to LTU CRC
                      checks in MUX.
11/15/00   igt        remove calls to dsrlptxq_put_txq() and dsrlptxq_get_txq()
                      and changed the location of the call to dsrlptxq_init()
09/15/00   ak         In dsrlp_cleanup, set ver_ptr to NULL before releasing
                      txq and dsrlpdel arrays, to avoid end-of-call sync
                      problems with rxc.c
09/05/00   ttl        Enabled the data TX on SCH when the FCH is muted for data.
08/06/00   ak         Moved SCRM callback to after frames are xmitted, as
                      that is more efficient.  Also now process IDLEs after
                      all other new + delayed frames.
07/31/00   igt        If rtt !=0 but reset=1 in a BLOB, it will not sync/ack
                      Also added #ifndef MSM5000_IRAM_REV
07/24/00   ak         Added a call to SCRM when no data to send or lots of
                      data to send.
07/08/00   ak         Added function has_data_to_send() to indicate if new
                      data or rexmit data is awaiting transmission.
06/22/00   igt        Added dsrlp_stat_strings[], dsrlp_stats_name_ptr[]
                      and function dslrp_get_stats_at() to support AT$QCRL3D
                      and AT$QCRL3R. Also update "state" in dsrlp_get_stats().
06/08/00   ak         Added MSM5000_IRAM_FWD #ifndef's for IRAM support.
06/05/00   ak         Added support for RLP now using a callback rx function
                      instead of a watermark.  Also now indicate to dsmgr
                      if any data frames received in last 20msec.
05/30/00   igt        added dsrlp_get_max_ms_nak_rounds_fwd() and _rev()
05/15/00   na/ak      Support for delayed frame detection.
05/10/00   ak         Improved mux erasure reporting.  support for sending
                      fill frames.
05/10/00   igt        dsrlp_clear_stats() now updates 'reset_timestamp' field
05/09/00   igt        update RLP_BLOB_used and curr_tx_nak_rounds stats.
04/28/00   igt        Set the resets statistic back to zero after call to
                      dsrlp_init(). Also update last_rtt, rx_blank_fr_cnt,
                      rx_rexmit_fr_cnt, rx_idle_fr_cnt, rx_rlp_erasures,
                      rx_mux_erasures, largest_contig_erasure, rx_null_fr_cnt
                      aborts, tx_rexmit_fr_cnt and tx_idle_fr_cnt statistics
04/19/00   ak         Modified erasure handling to account for mux erasures,
                      and also getting erasures in the INIT stages.  Always
                      age NAK list now.
04/13/00   ak         Added some debug when bad frames rxed in init stage.
                      Also keep track of mux-level erasures.
03/22/00   igt        fixed a compiler warning
03/21/00   igt        added stats support for SO33 screen
02/18/00   ak         Provide callbacks for when memory gets low in the
                      system, and take appropriate actions in the rx process
                      function.
02/15/00   igt        Added new BLOB processing and creation functions
01/14/00   ak         Added new table used in setting the 'used' field in
                      dsm items.  Fixed rexmit_data to use this table.
01/12/00   na         Fixed bug where while transmitting Idles, num_frames
                      was not being incremented.
01/11/00   ak         In dsrlp_init, added setting of nak params to default
                      values.  Due to RLP_BLOB in rlp3, (and static values
                      for rlp 1 & 2), these values are no longer reset in
                      SYNC/ACK handshake or during dsrlpi_reset.
01/10/00   na         RLP state was not being set to ESTABLISHED. Fixed this.
01/04/00   na         Fixed an extraneous ASSERT; Fixed problem with recog.
                      Erasures
08/09/99 na/hcg/ak    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "data_err_suppr.h"
#include "dsrlp_api.h"
#include "dss_iface_ioctl.h"


#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))

#ifdef TIMETEST_DSRLP_STATS
#include "timetest.h"
#define LOW_THPUT_SCALE 100
#define MED_THPUT_SCALE 1000
#define TIMETEST_DSRLP_MED_THPUT        TIMETEST_DSRLP_IS856
#define TIMETEST_DSRLP_LOW_THPUT        TIMETEST_DSRLP_IS2K
#else
#define TIMETEST_DSRLP_IS856   0x4000
#define TIMETEST_DSRLP_IS2K    0x2000
#endif /* TIMETEST_DSRLP_STATS */

#include "time_svc.h"
#include "time_tod.h"
#include "timer.h" //TODO: eventually, this should be replaced
#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "dsrlpnak.h"
#include "dsrlptxq.h"
#include "dsrlprtx.h"
#include "dsrlprsq.h"
#include "dsrlpdel.h"
#include "dsdvrlp_rx.h"
#include "dsdvrlpi_rx_preq.h"
#include "amssassert.h"


#include "mccdma_v.h"


#include "memory.h"
#include "err.h"
#include "bit.h"
#include "dsm.h"
#include "log.h"
#include "data_msg.h"
#include "rxc.h"

#ifdef FEATURE_HDR
#include "hdrrlp.h"
#include "hdrrlpqn.h"
#include "hdrrlpnak.h"
#include "hdrrlprsq.h"
#include "hdrrlptxsh.h"
#include "hdrrlpmarq.h"
#include "hdrmrlpctl.h"

#ifdef FEATURE_HDR_QOS
#include "hdrmrlpspd.h"
#include "hdrmrlpcif.h"
#include "hdrmrlp.h"
#include "hdrmrlpsig.h"
#include "hdrcom_api.h"
#include "hdrcp_api.h"

#ifdef FEATURE_HDR_REVB
#include "hdrrlpdnak.h"
#include "hdrrlpmlnak.h"
#endif  /* FEATURE_HDR_REVB */
#endif  /* FEATURE_HDR_QOS */
#endif  /* FEATURE_HDR */

#ifdef FEATURE_DS_PSTATS
#include "dsi.h"
#endif /* FEATURE_DS_PSTATS */

  #include "ds707_chs.h"
  #include "ds707_scrm.h"
  #include "ds707_cta_rlp.h"

#include "dsrlp3.h"
#include "rlp_event_defs.h"
#include "dsutil.h"
#include "ds707_rrm.h"
#include "ds707_extif.h"
#include <stringl/stringl.h>

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*
  Contains common code for each version of RLP.  Version-specific calls will
  be handled through a function pointer table, which will call into the
  version-specific RLP files.
*/

/*---------------------------------------------------------------------------
  This boolean, when TRUE, causes the Rx task to go ahead and prematurely
  age the oldest nak in the nak list, hopefully causing a hole to disappear
  and many blocked items to be sent up to the higher layer.  Set TRUE in
  the dsm callback dsrlp_mem_event when the Few Items Left memory event
  occurs.  Does this for all active RLP sessions.
---------------------------------------------------------------------------*/
boolean     mem_warning_event = FALSE;

/*---------------------------------------------------------------------------
  When TRUE, causes a reset to all RLP's.  This is because the number of
  remaining dsm small items has reached a dangerous level, and so a reset
  is the only remedy.  Set to TRUE in dsm callback dsrlp_mem_event when the
  Do Not Exceed memory event occurs.
---------------------------------------------------------------------------*/
boolean     mem_critical_event    = FALSE;

/*---------------------------------------------------------------------------
  Global bitmap currently used by only TAP to disable/enable inactivity timer
  the bitmap indication the client ID, who tried to disable the timer
  ******XX
        ||________ Fowrward TAP
        |_________ Reverse TAP
  The bitmap equal to zero, indicates the inactivity timer is enabled
---------------------------------------------------------------------------*/
uint8 dsrlp_disable_inactivity_timer_bitmap = 0;


/*---------------------------------------------------------------------------
  Callbacks for logging TIMETEST throughput data.
---------------------------------------------------------------------------*/
timer_type dsrlp_timetest_cb;
/*---------------------------------------------------------------------------
  HDR RLP Flush Timer

  T(RLPFlush)
    Used to ages the RLP flush counter.  The flush timer is needed since HDR
    RLP does not have the concept of idle frames, thus cases could exist in
    which it is not known that an octet is missing.  For instance, if the
    last received octet was lost, and no other packets were sent by the
    transmitter, no NAK would be generated, thus the transmitter would never
    re-transmit the lost packet.

    The timer is reset with each outgoing RLP packet that is transmitted.
    If the timer expires, RLP shall send an RLP packet containing the octet
    with sequence number V(S)-1.  Queue up the packet for re-transmission.

    A timer value of 300 ms is specified in the IS-856 standard.
---------------------------------------------------------------------------*/
#define HDRRLP_FLUSH_TIMER_VAL_MSEC 300                      /* time in ms */


#ifdef FEATURE_DS_PSTATS
#ifdef FEATURE_DS_RLP3
/*---------------------------------------------------------------------------
  Array of stats names, stored as character strings. The strings could be
  defined within the array of name pointers, but the compiler would put them
  in CONST.
---------------------------------------------------------------------------*/
LOCAL const byte dsrlp_stats_strings[DSRLP_STATS_COUNT][18] =
{
  "Rx Data Cnt      ",
  "Tx Data Cnt      ",
  "Single Naks      ",
  "Double Naks      ",
  "Triple Naks      ",
  "Rx     Naks      ",
/*
  "Tx ReXmits       ",
  "Rx ReXmits       ",
  "Tx Idle          ",
  "Rx Idle          ",
  "Rx Blank         ",
  "Rx Null          ",
  "Tx Fundamental   ",
  "Rx Fundamental   ",
*/
  "Tx Total Bytes   ",
  "Rx Total Bytes   ",

  "ReXmits Not Found",
  "Fill Frames Rx'ed",

  "Rlp Erasures     ",
  "Mux Erasures     ",

  "Lrgst Cntg. Erase",

  "Last RTT         ",

  "Resets           ",
  "Aborts (Nak)     ",

/*
  "BLOB Usage       ",

  "Tx Nak Rounds    ",
*/
  "Rlp State        "

};

/*---------------------------------------------------------------------------
  Array of pointers to stats names.
---------------------------------------------------------------------------*/
const byte * const dsrlp_stats_name_ptr[DSRLP_STATS_COUNT] =
{
  dsrlp_stats_strings[0],
  dsrlp_stats_strings[1],
  dsrlp_stats_strings[2],
  dsrlp_stats_strings[3],
  dsrlp_stats_strings[4],
  dsrlp_stats_strings[5],
  dsrlp_stats_strings[6],
  dsrlp_stats_strings[7],
  dsrlp_stats_strings[8],
  dsrlp_stats_strings[9],
  dsrlp_stats_strings[10],
  dsrlp_stats_strings[11],
  dsrlp_stats_strings[12],
  dsrlp_stats_strings[13],
  dsrlp_stats_strings[14],
  dsrlp_stats_strings[15],
  dsrlp_stats_strings[16],
/*
  dsrlp_stats_strings[17],
  dsrlp_stats_strings[18],
  dsrlp_stats_strings[19],
  dsrlp_stats_strings[20],
  dsrlp_stats_strings[21],
  dsrlp_stats_strings[22],
  dsrlp_stats_strings[23],
  dsrlp_stats_strings[24],
  dsrlp_stats_strings[25],
  dsrlp_stats_strings[26]
*/
};

/*---------------------------------------------------------------------------
  Array of names of the last call ending state.
---------------------------------------------------------------------------*/
LOCAL const byte dsrlp_last_call_strings[3][20] =
{
  "ESTABLISHED        ",  /* Last call was established              */
  "RLP_NOT_ESTABLISHED",  /* RLP did not sync up in last call       */
  "TCP_NOT_ESTABLISHED"   /* TCP did not establish in the last call */
};

/*---------------------------------------------------------------------------
  Array of pointers to names of the last call ending state.
---------------------------------------------------------------------------*/
const byte * const dsrlp_last_call_synced[3] =
{
  dsrlp_last_call_strings[0],
  dsrlp_last_call_strings[1],
  dsrlp_last_call_strings[2]
};

/*---------------------------------------------------------------------------
  Pointer (into rlp_last_call_synced) to name of last call ending state.
---------------------------------------------------------------------------*/
byte * dsrlp_last_call_synced_ptr;

/*---------------------------------------------------------------------------
  Copy of the last round trip time.
---------------------------------------------------------------------------*/
word dsrlp_last_rtt;
#endif /* FEATURE_DS_RLP3 */
#endif /* FEATURE_DS_PSTATS */

#define HDRRLP_DPA_SEQ_LEN       22

#define HDRRLP_EMPA_SEQ_LEN_00 6
#define HDRRLP_EMPA_SEQ_LEN_01 14
#define HDRRLP_EMPA_SEQ_LEN_02 22

#define HDRRLP_MMPA_SEQ_LEN_00 6
#define HDRRLP_MMPA_SEQ_LEN_01 14
#define HDRRLP_MMPA_SEQ_LEN_02 22
#define HDRRLP_MMPA_SEQ_LEN_03 30
/*===========================================================================
               G L O B A L  V A R I A B L E S
===========================================================================*/
#ifdef FEATURE_DS_RLP3
#ifdef FEATURE_RLP_LOGGING
/*---------------------------------------------------------------------------
  Array that holds the log id's for rlp logging.
---------------------------------------------------------------------------*/
static const ds_log_rlp_phy_channel_enum_type dsrlp_log_id_table
                                                         [RLP_MAX_REV_PDU] =
{
  DS_LOG_RLP_FCH,     /* index 0 = FCH                                        */
  DS_LOG_RLP_DCCH,    /* index 0 = DCCH                                       */
  DS_LOG_RLP_SCH_0,   /* remainder are all on the SCH                         */
  DS_LOG_RLP_SCH_0,
  DS_LOG_RLP_SCH_0,
  DS_LOG_RLP_SCH_0,
  DS_LOG_RLP_SCH_0,
  DS_LOG_RLP_SCH_0,
  DS_LOG_RLP_SCH_0,
  DS_LOG_RLP_SCH_0
};
#endif

/*---------------------------------------------------------------------------
  The following table contains the length in bytes of all FCH/DCCH/SCH1x
  frames.
  The table is indexed by frame rate, type of service (primary/secondary) and
  rateset (where rateset1 is first entry, and rateset2 is second entry).
  The entries represent the number of bytes available for RLP excluding the
  type bits. The bits are rounded down to get number of bytes.
  This table is valid for MuxPDU1 and MuxPDU2.
  An entry of 0 means, this rate is not applicable.
---------------------------------------------------------------------------*/
static const byte dsrlp_len_table[DSRLP_RATE_ERASURE][DSRLP_GENERIC_SRVC]
                                 [DSRLP_MUX_PDU_3] =
{
  {{21, 33},     {20, 32}},                    /* Rate 1 -   full rate           */
  {{0,  0},      {19, 30}},                    /* Rate 7/8 - secondary only      */
  {{0,  0},      {16, 26}},                    /* Rate 3/4 - secondary only      */
  {{10, 15},     {11, 17}},                    /* Rate 1/2 - half rate           */
  {{0,  0},       {0, 15}},                    /* Rate 7/16- secondary only      */
  {{0,  0},       {0, 12}},                    /* Rate 3/8 - secondary only      */
  {{5,  6},       {0,  8}},                    /* Rate 1/4 - quarter rate        */
  {{0,  0},       {0,  6}},                    /* Rate 3/16- secondary only      */
  {{2,  2},       {0,  4}},                    /* Rate 1/8 - eigth rate          */
  {{0,  0},       {0,  2}}                    /* Rate 1/16- secondary only      */
};

/*---------------------------------------------------------------------------
  The following table contains the length in bytes of all FCH/DCCH frames.
  The table is indexed by frame rate, type of service (primary/secondary) and
  rateset (where rateset1 is first entry, and rateset2 is second entry).

  The entries represent the value that should go into the used field of the
  dsm item holding the frame info.

  An entry of 0 means, this rate is not applicable.
---------------------------------------------------------------------------*/
static const byte dsrlp_used_len_table[DSRLP_RATE_ERASURE][DSRLP_GENERIC_SRVC]
                                      [DSRLP_MUX_PDU_3] =
{
  {{22, 34},     {21, 33}},                    /* Rate 1 -   full rate           */
  {{0,  0},      {19, 31}},                    /* Rate 7/8 - secondary only      */
  {{0,  0},      {16, 26}},                    /* Rate 3/4 - secondary only      */
  {{10, 16},     {11, 18}},                    /* Rate 1/2 - half rate           */
  {{0,  0},       {0, 16}},                    /* Rate 7/16- secondary only      */
  {{0,  0},       {0, 13}},                    /* Rate 3/8 - secondary only      */
  {{5,  7},       {0,  9}},                    /* Rate 1/4 - quarter rate        */
  {{0,  0},       {0,  7}},                    /* Rate 3/16- secondary only      */
  {{2,  3},       {0,  4}},                    /* Rate 1/8 - eigth rate          */
  {{0,  0},       {0,  3}}                    /* Rate 1/16- secondary only      */
};
/*---------------------------------------------------------------------------

  RLP3 Naking policy is decided by 3 different variables. The variables are
    a) Default Values
    b) Desired Values
    c) Negotiated values

  Default Value: This value is used only while negotiating with the Base
  station during Service Negotiation (i.e. processing/sending RLP BLOB).
  Initialize it to 3 & {1,2,3} for forward Naks and Naks/round.

  Desired Value: This is the value which the application wants to use in its
  current RLP3 data call. Initialize this to Default Value.

  Negotiated Value: This is the value negotiated by BS and MS. It is minimum
  of what BS sends, and MS default values. During power on initialization
  these values are initialized to Maximum value.

  In addition to the above values,we have: Working value which the RLP
  implementation uses. The working value is decided by following logic:
   if (negotiation is done)
   {
       Negotiated Value = MIN (BS Value, MS Default Values);
       Working value = MIN(Negotiated Value, Desired Value).
   }
   else
   {
       Working value = Desired value.
   }

  To protect the RLP state, we change the variables only at end of the RLP
  Tx. So, we use the variable update_state_var for doing it.

  default_nak_rounds_fwd: Def. NAK rounds for BS
  default_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS]:
                         Def. NAKs per round for BS

  default_nak_rounds_rev: Default NAK rounds for MS
  default_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS]:

  negotiated_nak_rounds_fwd:  Def. NAK rounds for BS
  negotiated_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS]:
                      Def. NAKs per round for BS
  negotiated_nak_rounds_rev: Default NAK rounds for MS
  negotiated_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];
---------------------------------------------------------------------------*/
struct
{

  byte    default_nak_rounds_fwd;
  byte    default_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS];
  byte    default_nak_rounds_rev;
  byte    default_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];

  byte    negotiated_nak_rounds_fwd;
  byte    negotiated_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS];
  byte    negotiated_nak_rounds_rev;
  byte    negotiated_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];

  byte    rscb_index;
  boolean update_state_var;
} dsrlp3_opt;

uint8 dsrlp_last_fill_nak_gap = DSRLPI_LAST_FILL_NAK_GAP;

#ifdef FEATURE_IS2000_REL_A
/*---------------------------------------------------------------------------
   Global variable that holds all QOS values - default, desired, rx from BS
---------------------------------------------------------------------------*/
dsrlp_qos_values_type dsrlp_qos_values;
#endif /* FEATURE_IS2000_REL_A */

#endif /* FEATURE_DS_RLP3 */


typedef struct{
  uint64    last_rx_bytes; /* last updated total rx bytes */
  uint64    last_tx_bytes; /* last updated total tx bytes */
} dsrlp_throuput_data_type;

dsrlp_throuput_data_type rlp_data_1x;
#ifdef FEATURE_HDR
dsrlp_throuput_data_type rlp_data_hdr;
#endif /* FEATURE_HDR */

static boolean is_1xrlp_ready = FALSE;

/*---------------------------------------------------------------------------
  DSM memory level critical section variable.
---------------------------------------------------------------------------*/
LOCAL rex_crit_sect_type dsm_mem_level_crit_sect;

/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION      DSRLPI_ENTER_CONTROL_PATH_CRIT_SECT

DESCRIPTION   Wrapper function to enter the critical section only for HDR
              specific RLPs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  If a null pointer is passed then the function asserts.
===========================================================================*/
void dsrlpi_enter_control_path_crit_sect
(
  dsrlpi_sess_ctl_blk_type *rscb_ptr,
  dsrlp_ver_enum_type ver
)
{
  if( rscb_ptr == NULL)
  {
    /* This is an internal function so we can assert here if we get an invalid
       parameter passed to this function */
    ASSERT(0);
  }


  if ( ( ver == DSRLP_VER_HDR ) ||
       ( ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
       ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ) )
  {
    /* Enter Critical Section */
    rex_enter_crit_sect( &rscb_ptr->cfg.crit_sect  );
  }
} /* dsrlpi_enter_control_path_crit_sect() */


/*===========================================================================

FUNCTION      DSRLPI_LEAVE_CONTROL_PATH_CRIT_SECT

DESCRIPTION   Wrapper function to leave the critical section only for HDR
              specific RLPs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  If a null pointer is passed then the function asserts.
===========================================================================*/
void dsrlpi_leave_control_path_crit_sect
(
  dsrlpi_sess_ctl_blk_type *rscb_ptr ,
  dsrlp_ver_enum_type ver
)
{
  if( rscb_ptr == NULL)
  {
    /* This is an internal function so we can assert here if we get an invalid
       parameter passed to this function */
    ASSERT(0);
  }


  if ( ( ver == DSRLP_VER_HDR ) ||
       ( ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
       ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ) )
  {
    /* Leave Critical Section */
    rex_leave_crit_sect( &rscb_ptr->cfg.crit_sect  );
  }
} /* dsrlpi_leave_control_path_crit_sect() */

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
#ifdef FEATURE_DS_RLP3
//extern dsrlpi_ver_const_type dsrlp3_constants; //linting, already being defined.
extern void dsrlp3_opt_init(void);
extern void dsrlp_opt_update_state(void);
#ifdef FEATURE_IS2000_REL_A
extern void dsrlp_qos_init(void);
#endif /* FEATURE_IS2000_REL_A */
#endif /* FEATURE_DS_RLP3 */


void dsrlp_log_timetest_throughput ( int4 ms);

/*===========================================================================
FUNCTION       DSRLP_GET_LAST_FILL_NAK_GAP

DESCRIPTION    Accessor method for dsrlp_last_fill_nak_gap

PARAMETERS
  None

RETURN VALUE
  dsrlp_last_fill_nak_gap

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 dsrlp_get_last_fill_nak_gap(void)
{
    return dsrlp_last_fill_nak_gap;
}

/*===========================================================================
FUNCTION       DSRLP_SET_LAST_FILL_NAK_GAP

DESCRIPTION    Mutator method for dsrlp_last_fill_nak_gap

PARAMETERS
  new value for dsrlp_last_fill_nak_gap

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_set_last_fill_nak_gap
(
  uint8 last_fill_nak_gap
)
{
    dsrlp_last_fill_nak_gap = last_fill_nak_gap;
}


/*===========================================================================

FUNCTION       DSRLP_MEM_EVENT

DESCRIPTION    Called when the DSM pool reaches a registered memory level for
               small items.

PARAMETERS
  dsm_mem_pool_id_enum type   pool_id  -  Memory pool ID
  dsm_mempool_level_enum_type event    -  Memory Level
  dsm_mem_op_enum_type        mem_op   -  Type of memory operation

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e715 unreferenced variable*/
void dsrlp_mem_event
(
  dsm_mempool_id_enum_type pool_id,
  dsm_mem_level_enum_type level,
  dsm_mem_op_enum_type mem_op
)
{
  /*-------------------------------------------------------------------------
    Enter DSM memory level critical section. mem_critical_event and
    mem_warning_event global variables are checked and modified in
    dsrlp_process_rx_frames() which runs in MUX task context every 20 ms and
    so these variables need to be protected by mutual exclusion.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&dsm_mem_level_crit_sect);

  if (level == DSM_MEM_LEVEL_LINK_LAYER_DNE)
  {
    mem_critical_event = TRUE;
  }
  else if (level == DSM_MEM_LEVEL_LINK_LAYER_FEW)
  {
    mem_warning_event = TRUE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Sanity check.  We should not have received notification for anything
      but the Do Not Exceed and Few Items Left memory events.
    -----------------------------------------------------------------------*/
    DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                  "Callback called for unregistered memory level %d", level);
  }

  /*-------------------------------------------------------------------------
    Leave DSM memory level critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&dsm_mem_level_crit_sect);
}
/*lint -restore*/

/*===========================================================================

FUNCTION       DSRLP_STARTUP

DESCRIPTION    This function is to be called only once per power-up
               of the phone.  This function initializes any structs
               which may be global across all RLP versions or instances.

               DSRLP_INIT is called per RLP instance, and so can be
               called many times, but this function must be called
               only once, and before any other RLP functions are called.


DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlp_startup
(
  void
)
{
  int i;
  int p;
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr;
#ifdef FEATURE_HDR
  dsrlpi_per_combo_type *handles_per_combo;
  q_type *hdrrlpmarq_info;
  int index;
#endif /*FEATURE_HDR*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Reset the entire RLP control block array to 0.
    Initialize the NAK list free queue.
    Initialize the Transmit queues (saved frame arrays).
    Initialize the Retransmit free queue.
  -------------------------------------------------------------------------*/
  memset( dsrlpi_rscb_array, 0, sizeof( dsrlpi_rscb_array));
  dsrlpnak_init();
#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
    Initialize the DelayedNak list free queue.
    Initialize the MultiLinkNak list free queue.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REVB
  hdrrlpmlnak_init();
#endif /* FEATURE_HDR_REVB */

#endif /* FEATURE_HDR */

  dsrlprtx_init();

  /*-------------------------------------------------------------------------
     Set the RLP last fill NAK gap to the default value.
     It can be changed via the IOCTL interface
  -------------------------------------------------------------------------*/
  dsrlp_last_fill_nak_gap = DSRLPI_LAST_FILL_NAK_GAP;

#ifdef FEATURE_DS_RLP3
  dsrlpdel_init();
#ifdef FEATURE_IS2000_REL_A
  dsrlp_qos_init();
#endif /* FEATURE_IS2000_REL_A */
#endif /* FEATURE_DS_RLP3 */

  /*-------------------------------------------------------------------------
    Initialize the critical section for each RLP.
  -------------------------------------------------------------------------*/
  for(i=0;i<DSRLP_MAX_RSCB_ELEMENTS;i++)
  {
    dsrlpi_rscb_array[i].is_hdrmrlp_active = FALSE;

#ifdef FEATURE_HDR_QOS
    dsrlpi_rscb_array[i].cfg.sched_head_node_ptr = NULL;
    dsrlpi_rscb_array[i].cfg.num_sched_nodes = 0;
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      dsrlpi_rscb_array[i].rlp_list[p].state.sched_rr_node_ptr = NULL;
    }
#endif /* FEATURE_HDR_QOS */

    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      rex_init_crit_sect( &dsrlpi_rscb_array[i].rlp_list[p].state.data_path_crit_sect );
      rrcb_ptr = &dsrlpi_rscb_array[i].rlp_list[p];
      /*-------------------------------------------------------------------------
        We initialize all queues in the startup function that is called on power-
        up. These queues are reset (i.e. elements are deleted) but not destroyed every
        time dsrlp_cleanup() function is called. 
      -------------------------------------------------------------------------*/
      /*lint -save -e534*/

       /*----------------------------------------------------------
       * nak_q is used in hdrrlpnak.c and dsrlpnak.c
       * delayed_nak_q is used in hdrrlpdnak.c
       * Note: Both these queues are used only if FEATURE_HDR_REVB is defined
       * but we initialize the queues irrespective of featurization    
       -----------------------------------------------------------*/
      (void)q_init(&rrcb_ptr->state.nak_q);
      (void)q_init(&rrcb_ptr->state.delayed_nak_q);

      /*multilink_nak_q is used in hdrrlpi.c and hdrrlpmlnak.c*/
      (void)q_init(&rrcb_ptr->state.multilink_nak_q);

      /*--------------------------------------------------------------
      * quick_repeat_nak_q is used in hdrrlpmlnak.c and hdrrlpi.c
      * Note: it is used only if FEATURE_HDR_REVB_QUICKREPEATNAK is defined
      * but we initialize it in all cases
      ----------------------------------------------------------------*/
      (void)q_init(&rrcb_ptr->state.quick_repeat_nak_q);

      /* tx_item_q is used in hdrrlp.c and hdrrlpi.c*/
      (void)q_init(&rrcb_ptr->state.tx_item_q);

      /* buffered_rx_q is used in hdrmrlp.c, hdrrlpi.c and hdrrlpsort.c*/
      (void)q_init(&rrcb_ptr->state.buffered_rx_q);
      /*lint -restore */

      /* reset_q is used in hdrrlprsq.c & dsrlprsq.c*/
      (void)q_init(&(rrcb_ptr->state.reseq_q) );

      /*tx_q isused in dsrlptxq.c & hdrrlptxq.c*/
      (void)q_init(&(rrcb_ptr->state.tx_q) );

#ifdef FEATURE_HDR
      /*hdr_rtxq is used in dsrrlprtx.c*/   
      (void)q_init(&rrcb_ptr->state.hdr_rtxq );

      /*used in hdrrlptxsh.c*/
      handles_per_combo = rrcb_ptr->state.handles_per_combo;
      for (index = 0; index < HDRPCP_MAX_COMBOS; index++)
      {
         (void)q_init(&handles_per_combo[index].carrier_handles_q);
         handles_per_combo[index].combo_id = HDRPCP_NO_COMBO_ID_ACCEPTED;
      }
      /*used in hdrrlpmarq.c*/
      hdrrlpmarq_info = rrcb_ptr->state.hdrrlpmarq_info;
      for (index = 0; index < DSRLP_MARQ_NUM_SUBFRAMES; index++)
      {
         (void) q_init(&hdrrlpmarq_info[index]);
      }
#endif /*FEATURE_HDR*/

      /*
       * Cleanup of these fwd_input_q, fwd_pre_q queues is done by MUX
       * so no explicit cleanup is required
       */
      (void) q_init(&rrcb_ptr->state.fwd_input_q);

      /*This function initializes RLP's preQ.*/
      dsdvrlpi_rx_pre_q_init( rrcb_ptr );

    }

    rex_init_crit_sect( &dsrlpi_rscb_array[i].cfg.crit_sect );
  }

#ifdef FEATURE_HDR
  hdrrlp_startup();

#ifdef FEATURE_HDR_QOS
  hdrmrlpi_startup();
#endif /* FEATURE_HDR_QOS */
#endif /* FEATURE_HDR */

  dsrlp3_opt_init();

  /*-------------------------------------------------------------------------
    Register the clock callback for TIMETEST throughput profiling.
  -------------------------------------------------------------------------*/
  timer_def2(&dsrlp_timetest_cb, NULL);

  /*-------------------------------------------------------------------------
    Initialize DSM memory level critical section.
  -------------------------------------------------------------------------*/
  DS_INIT_CRIT_SECTION( &dsm_mem_level_crit_sect );
} /* dsrlp_startup() */

/*===========================================================================

FUNCTION DSRLP_INIT

DESCRIPTION
  This function initializes the RLP session corresponding to the passed
  sr_id for the given version of RLP. All data structures are setup and
  RLP is reset to start new operation. This function should be called at the
  beginning of each call for each service.

  Input Parameters:
    rscb_index: Index in the rscb array where this record will be stored
    sr_id: The Service ID for the particular service. Primary is usually 1
           and secondary is usually 2.
    ver  : The RLP version - 1,2,3...
    octet_sequenced: This RLP will run in byte sequenced mode. This means
           that all new frames and retransmissions will have a single
           byte sequence space.
           Mode of operation cannot be changed in the middle of a RLP
           session.
    enhanced: This RLP will carry EMPA or MMPA traffic

  Note that this function will not work for HDR MRLP AN stream until app
  subtype is supported as input parameter.

DEPENDENCIES
  None

RETURN VALUE
  Returns a handle to the RLP that is initialized. This is the array index
  that has got initialized for the RLP. There are the foll special cases:
  - In case the RLP was already initialized when this function is called,
    then the function returns a handle to the initialized RLP instance.
  - If there are no more RLPs available to be initialized, then the RLP
    returns DSRLPI_INVALID_RSCB_INDEX.
  - In case of HDR MultiFlow RLP (Forward and Reverse), if the RLP is not
    active at the time the function is called, the RLP returns
    DSRLPI_INVALID_RSCB_INDEX.

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_init
(
  byte rscb_index,                           /* index in the rscb array where
                                                this record will be stored */
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver,                   /* RLP version                */
  boolean octet_sequenced,                   /* Byte sequenced operation?  */
  dsrlp_enhanced_ver_enum_type enhanced      /* EMPA or MMPA               */
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  byte                                   i;  /* Loop counter               */
  event_rlp_payload_type rlp_event_payload;  /* payload for RLP evnt report*/
#ifdef FEATURE_HDR_QOS
  hdrcp_mrlpc_flow_timers_type flow_timers;
  boolean nak_enabled;
  uint8 phys_link_nak_enabled_rev;
  uint8 route_protocol_sdu;
  uint8 out_of_order_delivery_to_route_protocol_fwd;
  uint8 data_unit;
  hdrcp_mrlpc_flow_sequence_length_type flow_seq_length;
  hdrcp_mrlpc_flow_identification_type flow_identification;
  hdrcp_mrlpc_flow_prot_parm_rev_type flow_prot_parm_rev;
  hdrcp_mrlpc_flow_prot_parm_fwd_type flow_prot_parm_fwd;
  hdrcp_mrlpc_route_prot_parm_rev_type route_prot_parm_rev;
  hdrcp_mrlpc_route_prot_parm_fwd_type route_prot_parm_fwd;
  boolean value_changed;
  boolean transmit_abort_timer_rev_val_changed = FALSE;
  uint32 transmit_abort_timer_rev_val = 0x00;
#endif /* FEATURE_HDR_QOS */
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr;
  uint8 p;

#ifdef FEATURE_HDR_QOS
  memset(&flow_timers, 0, sizeof(hdrcp_mrlpc_flow_timers_type));
  memset(&flow_seq_length, 0, sizeof(hdrcp_mrlpc_flow_sequence_length_type));
  memset(&flow_identification, 0, sizeof(hdrcp_mrlpc_flow_identification_type));
  memset(&flow_prot_parm_rev, 0, sizeof(hdrcp_mrlpc_flow_prot_parm_rev_type));
  memset(&flow_prot_parm_fwd, 0, sizeof(hdrcp_mrlpc_flow_prot_parm_fwd_type));
  memset(&route_prot_parm_rev, 0, sizeof(hdrcp_mrlpc_route_prot_parm_rev_type));
  memset(&route_prot_parm_fwd, 0, sizeof(hdrcp_mrlpc_route_prot_parm_fwd_type));
#endif /* FEATURE_HDR_QOS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_RLP_MSG3(MSG_LEGACY_HIGH, "dsrlp_init called for RLP: %d ver:%d rscb:%d", sr_id, ver, rscb_index);
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id. Initialize the state info to zero.
    The cfg. info is not reset, because the watermarks might have been set
    before a call to dsrlp_init.
    Statistics are not reset, because we might be collecting statistics
    across multiple calls.
  -------------------------------------------------------------------------*/
#ifndef FEATURE_HDR_QOS
  ASSERT((sr_id < DSRLPI_MAX_CDMA_SR_ID_VALUE) ||
         ((sr_id == DSRLP_HDR_SN_RLP_SR_ID_VALUE) && (ver == DSRLP_VER_HDR)) ||
         ((sr_id == DSRLPI_HDR_AN_AUTH_SR_ID_VALUE) && (ver == DSRLP_VER_HDR)))
#endif /* FEATURE_HDR_QOS */

  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  /* Enter Critical Section */
  dsrlpi_enter_control_path_crit_sect( rscb_ptr, ver );


  if( rscb_ptr->cfg.ver_ptr != NULL)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "dsrlp_init called multiple times RLP: %d", sr_id);

    /* Leave Critical Section */
    dsrlpi_leave_control_path_crit_sect( rscb_ptr, ver );
    return;
  }

  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

    rrcb_ptr->route = p;
    rrcb_ptr->cfg_ptr = &rscb_ptr->cfg;

  #ifdef FEATURE_HDR_QOS
    /*---------------------------------------------------------------------
     Check the HDR MRLP state. If it is active, this means that the RLP is
     reserved for use by the HDR MultiFlow RLP. We need to check that we
     are trying to initialize the RLP with the same version it is reserved
     for. If there is a conflict, then return.
    ---------------------------------------------------------------------*/
    if( rscb_ptr->is_hdrmrlp_active == TRUE )
    {
      /* If the MRLP state is active, version has to be MRLP Type */
        ASSERT( (rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) ||
                (rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE));

      if(( ver != rscb_ptr->cfg.ver ) ||
           ( sr_id != rscb_ptr->cfg.sr_id))
      {
        DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Cant init active "
                      "rscb:%d ver:%d sr_id:%d. conflict with i/p",
                      rscb_index, ver, sr_id);

        /* Leave Critical Section */
        dsrlpi_leave_control_path_crit_sect( rscb_ptr, ver );
        return;
      }

      /*-------------------------------------------------------------------
        If the RLP is is in Closed state, then do not initialize the RLP
      -------------------------------------------------------------------*/
      if( rrcb_ptr->state.state == DSRLPI_CLOSE_STATE )
      {
        DATA_RLP_MSG3(MSG_LEGACY_HIGH, "RSCB %d RLP %d ver %d in Closed State. Not Init-ing",
                 rscb_index, sr_id, ver );
        /* Leave Critical Section */
        dsrlpi_leave_control_path_crit_sect( rscb_ptr, ver );
        return;
      }

    }
    else /* rlp is not active */
    {
      /*-------------------------------------------------------------------
        If the RLP is not active, then ensure that the RLP we are trying
        to initialize is not a multiflow RLP. If it is, then this is an error
        For the HDR MRLP to be initialized, it needs to have been activated
        before
      -------------------------------------------------------------------*/
      if(( ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
         ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ))
      {
        DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Cant init MRLP "
                      "ver:%d sr_id:%d. rscb:%d not activated",
                      ver, sr_id, rscb_index );
        /* Leave Critical Section */
        dsrlpi_leave_control_path_crit_sect( rscb_ptr, ver );
        return;
      }
    }
  #endif /* FEATURE_HDR_QOS */
  }

  /*-------------------------------------------------------------------------
     Set the RLP last fill NAK gap to the default value.
     It can be changed via the IOCTL interface
  -------------------------------------------------------------------------*/
  dsrlp_last_fill_nak_gap = DSRLPI_LAST_FILL_NAK_GAP;

  /*---------------------------------------------------------------------
     Report RLP establishing event
  ---------------------------------------------------------------------*/
  rlp_event_payload.event_rlp_type = event_rlp_establishing;
  rlp_event_payload.info = rscb_ptr->cfg.sr_id;
  event_report_payload (EVENT_RLP,
                        sizeof(event_rlp_payload_type),
                        &rlp_event_payload);

  rscb_ptr->cfg.sr_id = sr_id;
  rscb_ptr->cfg.ver = ver;

  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

#ifdef FEATURE_DS_RLP3
    if(ver == DSRLP_VER_1X_3)
    {
      rrcb_ptr->state.del_fr_id = dsrlpdel_get_array();
      ASSERT(rrcb_ptr->state.del_fr_id != DSRLPDEL_NO_ARRAY);
    }
#endif
  }

  /*-----------------------------------------------------------------------
    Only register the memory callbacks for non-HDR calls.
  -----------------------------------------------------------------------*/
  if (ver == DSRLP_VER_1X_3)
  {
    dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_LINK_LAYER_DNE,
                         DSM_MEM_OP_NEW,
                         dsrlp_mem_event);

    dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_LINK_LAYER_FEW,
                         DSM_MEM_OP_NEW,
                         dsrlp_mem_event);
  } /* ver != DSRLP_VER_HDR */

  /*-------------------------------------------------------------------------
    Initialize the Retransmit List and Resequencing Queue.
  -------------------------------------------------------------------------*/
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

    rrcb_ptr->state.combo_index = 0;
#ifdef FEATURE_HDR
    for ( i=0; i< HDRCP_PCP_MAX_COMBOS;i++ )
    {
      hdrrlp_reset_new_data_local_state(&rrcb_ptr->state, i );
      rrcb_ptr->state.local_state[i].txed_flush_pkt = FALSE;
    }

    hdrrlptxsh_init(rrcb_ptr);
    hdrrlpmarq_init(rrcb_ptr);
#endif /* FETURE_HDR */
  }
#ifdef FEATURE_HDR
  if ((rscb_ptr->cfg.ver == DSRLP_VER_HDR) ||
      (rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) ||
      (rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE))
  {
    /* --------------------------------------------------------------------
       Needs to reset this value, since it can be changed by other
       application (HDRFETAP)
       -------------------------------------------------------------------*/
    rscb_ptr->cfg.enable_inactivity_timer = TRUE;
  }
  else
#endif /* FEATURE_HDR */
  {
    dsrlprsq_init( rscb_ptr);
  }
  dsrlptxq_init( rscb_ptr);

  /*-------------------------------------------------------------------------
    Initialize the data structures for the given RLP session. This involves
    setting up the version specific constants.
  -------------------------------------------------------------------------*/
  dsrlpi_init_constants( rscb_ptr, ver);
  rscb_ptr->cfg.octet_sequenced = octet_sequenced;
  rscb_ptr->cfg.enhanced = enhanced;

  if( ver == DSRLP_VER_1X_3 )
  {
    rscb_ptr->cfg.seq_modulus = DSRLPSEQ_32BIT_SEQ_MODULUS;
    /*** Need to change to use the values from the cfg structure instead of static*/
  }
#ifdef FEATURE_HDR
  else if( ver == DSRLP_VER_HDR )
  {
    rscb_ptr->cfg.seq_length = HDRRLP_DPA_SEQ_LEN;
    rscb_ptr->cfg.seq_modulus = HDRRLPSEQ_32BIT_SEQ_MODULUS;
    rscb_ptr->cfg.rlp_id_len_in_pkt = 0;
    rscb_ptr->cfg.nak_enabled = TRUE;
    rscb_ptr->cfg.nak_abort_timer_val = HDRRLP_NAK_ABORT_TIMER_VALUE;
    rscb_ptr->cfg.flush_timer_val = HDRRLP_FLUSH_TIMER_VAL_MSEC;
  }
#endif /* FEATURE_HDR */

#ifdef FEATURE_HDR_QOS
  else if(( ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
          ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ))
  {
    /*-----------------------------------------------------------------------
    Set this field to FALSE so that if any of the attributes change after
    initialization it will update propoerly. Note that we set it to FALSE before
    getting the attribute values cause attributes could change in the middle of
    accessing the individual attribs.
    -----------------------------------------------------------------------*/
    rscb_ptr->cfg.has_active_rlp_cfg_changed = FALSE;

    rscb_ptr->cfg.seq_length_short = 0;
    rscb_ptr->cfg.qn_seq_length = 0;

    if (rscb_ptr->cfg.enhanced == DSRLP_ENHANCED_VER_MMPA)
    {
      (void)hdrmrlpcif_get_sequence_length_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                                   sr_id,
                                                   ver,
                                                   &flow_seq_length,
                                                   &value_changed );
      if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        rscb_ptr->cfg.seq_length = flow_seq_length.sar_seq_len;
        rscb_ptr->cfg.seq_length_short = flow_seq_length.sar_seq_len_short;
        rscb_ptr->cfg.qn_seq_length = flow_seq_length.sar_seq_len - flow_seq_length.sar_seq_len_short;
      }
      else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
      {
        switch (flow_seq_length.sar_seq_len)
        {
          case 0x00:
            rscb_ptr->cfg.seq_length = HDRRLP_MMPA_SEQ_LEN_00;
            break;
          case 0x01:
            rscb_ptr->cfg.seq_length = HDRRLP_MMPA_SEQ_LEN_01;
            break;
          case 0x02:
            rscb_ptr->cfg.seq_length = HDRRLP_MMPA_SEQ_LEN_02;
            break;
          case 0x03:
            rscb_ptr->cfg.seq_length = HDRRLP_MMPA_SEQ_LEN_03;
            break;
          default:
            DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                          "Invalid SequenceLengthRev attribute value %d",
                          flow_seq_length.sar_seq_len);
            break;
        }
      }
      /* The LinkFlowId is trivial in EMPA */
      rscb_ptr->cfg.rlp_id_in_pkt  = sr_id;
      rscb_ptr->cfg.rlp_id_len_in_pkt = HDRRLP_MMPA_LINK_FLOW_ID_LEN;
    }
    else if (rscb_ptr->cfg.enhanced == DSRLP_ENHANCED_VER_EMPA)
    {
      (void)hdrmrlpcif_get_sequence_length_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                                   sr_id,
                                                   ver,
                                                   &flow_seq_length,
                                                   &value_changed );

      /* Convert configuration setting to actual sequence length in bits  */
      switch (flow_seq_length.sar_seq_len)
      {
        case 0x00:
          rscb_ptr->cfg.seq_length = HDRRLP_EMPA_SEQ_LEN_00;
          break;
        case 0x01:
          rscb_ptr->cfg.seq_length = HDRRLP_EMPA_SEQ_LEN_01;
          break;
        case 0x02:
          rscb_ptr->cfg.seq_length = HDRRLP_EMPA_SEQ_LEN_02;
          break;
        default:
          DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                        "Invalid SequenceLengthRev attribute value %d",
                        flow_seq_length.sar_seq_len);
          break;
      }

      /* The LinkFlowId is trivial in EMPA */
      rscb_ptr->cfg.rlp_id_in_pkt  = sr_id;
      rscb_ptr->cfg.rlp_id_len_in_pkt = HDRRLP_EMPA_LINK_FLOW_ID_LEN;
    }
    else if (rscb_ptr->cfg.enhanced == DSRLP_VER_MFPA)
    {
      (void)hdrmrlpcif_get_identification_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                                  sr_id,
                                                  ver,
                                                  &flow_identification,
                                                  &value_changed );

      rscb_ptr->cfg.seq_length = flow_identification.seq_len;
      rscb_ptr->cfg.rlp_id_in_pkt  = flow_identification.rlp_id;
      rscb_ptr->cfg.rlp_id_len_in_pkt = flow_identification.rlp_id_len;
    }

    (void)hdrmrlpcif_get_timers_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                        sr_id,
                                        ver,
                                        &flow_timers,
                                        &value_changed );

    (void)hdrmrlpcif_get_nak_enabled_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                             sr_id,
                                             ver,
                                             &nak_enabled,
                                             &value_changed );

    if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
    {
      (void)hdrmrlpcif_get_phys_layer_nak_en_rev_attrib
                                  ( HDRMRLPCIF_CLIENT_MRLP_SN,
                                    sr_id,
                                    ver,
                                    &phys_link_nak_enabled_rev,
                                    &value_changed );

      rscb_ptr->cfg.phys_link_nak_enabled_rev = phys_link_nak_enabled_rev;
      /*
         If Reverse RLP, then query for the attribute, and update SPD module
         with the value of the attribute. If the attribute value is non-default
         SPD module will start polling for this RLP as well, if the attribute
         value is default then SPD module will remove this RLP from its polling
         list
      */
      (void) hdrmrlpcif_get_transmit_abort_timer_rev_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                      sr_id,
                                      ver,
                                      &transmit_abort_timer_rev_val,
                                      &transmit_abort_timer_rev_val_changed);
      /*
        Set the Transmit abort timer value only for the non default flows. This takes care of setting the
        flow capability mask only for the PS flows bound to the non default RLP flows. It is to be noted that
        this change was done to prevent any SPD to be provided on the default RLP flow.
      */
      if (FALSE == hdrmrlpctl_is_rlp_default(sr_id, ver))
      {
        rscb_ptr->cfg.transmit_abort_timer_rev_val = (dword) transmit_abort_timer_rev_val * 5;
      }
      else
      {
        rscb_ptr->cfg.transmit_abort_timer_rev_val = 0;
      }
      rscb_ptr->cfg.dropped_stale_packet = FALSE;
      rscb_ptr->cfg.dropped_marq_packet  = FALSE;
      /* Notify no matter what the value is, if it 0 then SPD will take care of it,
         and in cases where it is non_zero SPD will also take care of it.
      */
      hdrmrlpspd_transmit_abort_timer_changed_notif ( rscb_index,  transmit_abort_timer_rev_val );
    } /* if DSRLP_VER_HDR_MULTIFLOW_REVERSE */

    /* The modulus is calculated as modulus = 2^sequence space */
    rscb_ptr->cfg.seq_modulus = (1 << (rscb_ptr->cfg.seq_length ));
    rscb_ptr->cfg.seq_modulus_short = (1 << (rscb_ptr->cfg.seq_length_short ));
    rscb_ptr->cfg.qn_seq_modulus = (1 << (rscb_ptr->cfg.qn_seq_length ));

    rscb_ptr->cfg.nak_enabled = nak_enabled;
    rscb_ptr->cfg.nak_abort_timer_val = flow_timers.abort_timer_val;
    rscb_ptr->cfg.flush_timer_val = flow_timers.flush_timer_val;
    rscb_ptr->cfg.nak_delay_timer_val = flow_timers.nak_delay_timer_val;


    /* EMPA/MMPA have a few extra attributes */
    if (rscb_ptr->cfg.enhanced == DSRLP_ENHANCED_VER_MMPA ||
        rscb_ptr->cfg.enhanced == DSRLP_ENHANCED_VER_EMPA)
    {
      (void)hdrmrlpcif_get_data_unit_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                             sr_id,
                                             ver,
                                             &data_unit,
                                             &value_changed );
      rscb_ptr->cfg.segment_sequencing = (data_unit == 0x01);

      (void)hdrmrlpcif_get_route_protocol_sdu_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                                      sr_id,
                                                      ver,
                                                      &route_protocol_sdu,
                                                      &value_changed );
      rscb_ptr->cfg.packet_stream = (route_protocol_sdu == 0x01);

      if (rscb_ptr->cfg.packet_stream &&
          ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        (void)hdrmrlpcif_get_out_of_order_delivery_to_route_protocol_fwd_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                                                                 sr_id,
                                                                                 ver,
                                                                                 &out_of_order_delivery_to_route_protocol_fwd,
                                                                                 &value_changed );
        rscb_ptr->cfg.out_of_order_allowed = (out_of_order_delivery_to_route_protocol_fwd == 0x01);
      }
      else
      {
        rscb_ptr->cfg.out_of_order_allowed = FALSE;
      }

#ifdef FEATURE_HDR_QOS
      if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        (void)hdrmrlpcif_get_flow_protocol_parameters_fwd_attrib (
                                 HDRMRLPCIF_CLIENT_MRLP_SN,
                                 sr_id,
                                 &flow_prot_parm_fwd,
                                 &value_changed );
        rscb_ptr->cfg.flow_protocol = flow_prot_parm_fwd.id;
        (void)hdrmrlpcif_get_route_protocol_parameters_fwd_attrib(
                                 HDRMRLPCIF_CLIENT_MRLP_SN,
                                 sr_id,
                                 &route_prot_parm_fwd,
                                 &value_changed );
        rscb_ptr->cfg.route_protocol = route_prot_parm_fwd.id;
      }
      else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
      {
        (void)hdrmrlpcif_get_flow_protocol_parameters_rev_attrib(HDRMRLPCIF_CLIENT_MRLP_SN,
                                                               sr_id,
                                                               &flow_prot_parm_rev,
                                                               &value_changed );
        rscb_ptr->cfg.flow_protocol = flow_prot_parm_rev.id;
        (void)hdrmrlpcif_get_route_protocol_parameters_rev_attrib(HDRMRLPCIF_CLIENT_MRLP_SN,
                                                               sr_id,
                                                               &route_prot_parm_rev,
                                                               &value_changed );
        rscb_ptr->cfg.route_protocol = route_prot_parm_rev.id;
      }
#endif /* FEATURE_HDR_QOS   */
    }
  }
#endif
  else
  {
    /*----------------------------------------------------------------------
      Trying to initialize invalid RLP. Assert!
    -----------------------------------------------------------------------*/
    ASSERT( 0 );
  }

  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

  /*------------------------------------------------------------------------
     Forward NAK parameter defaults.  These are done here since either
     they are a default (RLP 1 & 2) or negotiated via BLOB (RLP 3).  In
     either case, they are not changed by dsrlpi_reset.
  -------------------------------------------------------------------------*/
    rrcb_ptr->state.nak_rounds_fwd = rscb_ptr->cfg.ver_ptr->desired_nak_rounds_fwd;

    for (i = 0; i < rrcb_ptr->state.nak_rounds_fwd; i++)
    {
      if(rscb_ptr->cfg.ver != DSRLP_VER_1X_3)
      {
        rrcb_ptr->state.naks_per_round_fwd[i] =
          rscb_ptr->cfg.ver_ptr->desired_naks_per_round_fwd[i];
      }
      else
      {
        rrcb_ptr->state.naks_per_round_fwd[i] =
          MIN(rscb_ptr->cfg.ver_ptr->desired_naks_per_round_fwd[i],
                     dsrlp3_opt.default_naks_per_round_fwd[i]);
      }
    }
  }

  if (ver == DSRLP_VER_1X_3)
  {
    /*-------------------------------------------------------------------------
      By now the default parameters that the applications wants to use exist in
      dsrlp3_opt.default parameters. So the negotiated can be either max values
      i.e. {3,3,3} or lesser. We program the negotiated values with default
      values.
      If negotiation occurs then negotiated may be different than default.
      If negotiation did not happen, still negotiated is same as default.
    ---------------------------------------------------------------------------*/
    dsrlp3_opt.negotiated_nak_rounds_fwd = dsrlp3_opt.default_nak_rounds_fwd;
    for (i=0; i< (dsrlp3_opt.negotiated_nak_rounds_fwd); i++)
    {
      dsrlp3_opt.negotiated_naks_per_round_fwd[i] =
        dsrlp3_opt.default_naks_per_round_fwd[i] ;
    }
  }

  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

  /*------------------------------------------------------------------------
    Reverse NAK parameter defaults
  -------------------------------------------------------------------------*/
    rrcb_ptr->state.nak_rounds_rev = rscb_ptr->cfg.ver_ptr->desired_nak_rounds_rev;

    for (i = 0; i < rrcb_ptr->state.nak_rounds_rev; i++)
    {
      if (rscb_ptr->cfg.ver != DSRLP_VER_1X_3)
      {
        rrcb_ptr->state.naks_per_round_rev[i] =
          rscb_ptr->cfg.ver_ptr->desired_naks_per_round_rev[i];
      }
      else
      {
        rrcb_ptr->state.naks_per_round_rev[i] =
          MIN(rscb_ptr->cfg.ver_ptr->desired_naks_per_round_rev[i],
                    dsrlp3_opt.default_naks_per_round_rev[i]);
      }
    }
  }

  if (ver == DSRLP_VER_1X_3)
  {
    /*-------------------------------------------------------------------------
      By now the default parameters that the applications wants to use exist in
      dsrlp3_opt.default parameters. So the negotiated can be either default or
      lesser. So, we program the negotiated values with default values.
      If negotiation occurs then negotiated may be different than default.
      If negotiation did not happen, still negotiated is same as default.
    -------------------------------------------------------------------------*/
    dsrlp3_opt.negotiated_nak_rounds_rev = dsrlp3_opt.default_nak_rounds_rev;
    for (i=0; i< (dsrlp3_opt.negotiated_nak_rounds_rev); i++)
    {
       dsrlp3_opt.negotiated_naks_per_round_rev[i] =
           dsrlp3_opt.default_naks_per_round_rev[i];
    }
  }

  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

  /*-------------------------------------------------------------------------
    Reset RLP. This will cleanup all the memory and initialiaze all the state
    variables.
  -------------------------------------------------------------------------*/
    dsrlpi_reset(rrcb_ptr);

  /*-------------------------------------------------------------------------
    Set this back to 0 in here so it will not accumulate from call to call.
    This cannot be set to 0 in dsrlpi_reset() as that function is called
    even between init state changes say from S/A to Sync.
  -------------------------------------------------------------------------*/
    rrcb_ptr->state.sync_delay  = 0;


  /*-------------------------------------------------------------------------
    Set the stats.resets counter back to zero. This ensures that this call
    to dsrlpi_reset() will not count as a reset in the  middle of a call,
    which the statistic keeps track of.
  -------------------------------------------------------------------------*/
    rrcb_ptr->stats.resets = 0;
  }

  /*-------------------------------------------------------------------------
    Clear the statistics structure. Safe to do this last.
  -------------------------------------------------------------------------*/
  dsrlp_clear_stats(rscb_index);

  /*-------------------------------------------------------------------------
    Register the clock callback for TIMETEST throughput profiling.  Gets
    called periodically every 500 ms.
  -------------------------------------------------------------------------*/
  timer_reg(&dsrlp_timetest_cb,
          (timer_t2_cb_type)dsrlp_log_timetest_throughput,
          (timer_cb_data_type) NULL,
          500,
          500);

#ifdef FEATURE_HDR
  if (ver == DSRLP_VER_HDR)
  {
    /*-----------------------------------------------------------------------
    Call the HDR RLP initialization function. This needs to happen at the
    end since it registers callbacks with PCP that result in received data
    being passed to RLP. If RLP is not initialized completely, it may not be
    able to handle this data.
    -----------------------------------------------------------------------*/
    hdrrlp_init( sr_id );
  }
#ifdef FEATURE_HDR_QOS
  else if(( ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
          ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ))
   {
     hdrmrlp_init( sr_id, ver );
   }
#endif /* FEATURE_HDR_QOS */

#endif /* FEATURE_HDR */

  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

  /*-------------------------------------------------------------------------
    Default values of DDW and REXMIT_TIMER = 0
  -------------------------------------------------------------------------*/
    rrcb_ptr->state.fwd_ddw = 0;
    rrcb_ptr->state.fwd_rexmit_time = 0;
  }

  /* Leave Critical Section */
  dsrlpi_leave_control_path_crit_sect( rscb_ptr, ver );

} /* dsrlp_init() */

/*===========================================================================

FUNCTION DSRLP_CLEANUP

DESCRIPTION
  This function cleansup the memory and resets the RLP state for a given
  rscb_index (corresponding to an sr_id). This should be called at the end
  of a RLP session.

  Input Parameters:
    rscb_index: The index in the rscb array for the particular service.

  Note that for HDR MultiFlow RLP, there are some fields that are persistent
  across connections, that is, even when the RLP is torn down. These fields
  are not cleared in this function. These include fields:
  sr_id, cfg.ver, and state.is_hdrmrlp_active

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_cleanup
(
  byte sr_id,       /* Service ID */
  dsrlp_ver_enum_type rlp_type
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type        *rrcb_ptr;  /* rlp-specific control block */
  byte                           rscb_index; /* Index in rscb array        */
  dsrlp_stats_info_type   *stats_ptr;        /* local stats ptr            */
  dsm_item_type            *item_ptr;
  int p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------------
    Get the index of in the rscb array corresponding to sr_id
  --------------------------------------------------------------------*/
  rscb_index = dsrlp_get_rlp_rscb_index(sr_id,rlp_type);

  if(rscb_index >= DSRLPI_INVALID_RSCB_INDEX)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "dsrlp_cleanup called with invalid sr_id: %d",sr_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    rscb_index (corresponding to an sr_id).
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  /*-------------------------------------------------------------------------
    Enter critical section. This function is called from 707 packet manager
    and HDR Data module. Packet manager runs on DS task and HDR Data call
    flow does not run on interrupt context and so critical section suffice.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);

  if( rscb_ptr->cfg.ver_ptr != NULL)
  {
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      rrcb_ptr = &rscb_ptr->rlp_list[p];

      /*-----------------------------------------------------------------------
        Reset RLP. This will cleanup all the memory and initialiaze all the
        state variables.  INTLOCK This and the ver_ptr = NULL, so no data
        can get sent up to the RLP world by TXC as the call dies.
      -----------------------------------------------------------------------*/
      rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
      dsrlpi_reset( rrcb_ptr);

      if(rrcb_ptr->stats.resets > 0)
      {
        /*---------------------------------------------------------------------
          We're resetting as a result of cleanup, so reset would have
          incremented this value and we are restoring to its original.
        ---------------------------------------------------------------------*/
        rrcb_ptr->stats.resets--;
      }

      /*-------------------------------------------------------------------------
        Free any untransmitted data.  This can't be done within dsrlpi_reset(),
        or some unsent data might be lost.
      -------------------------------------------------------------------------*/
      dsm_free_packet(&rrcb_ptr->state.tx_item_ptr);
      rrcb_ptr->state.tx_item_ptr = NULL;
      while ((item_ptr = (dsm_item_type *)q_get(&rrcb_ptr->state.tx_item_q)) != NULL)
      {
        dsm_free_packet(&item_ptr);
      }
      rrcb_ptr->state.seek_offset = 0;

      if ((rrcb_ptr->cfg_ptr->rx_wm_ptr[p] != NULL) &&
          ((rscb_ptr->cfg.ver == DSRLP_VER_HDR) ||
           (rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) ||
           (rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)))
      {      
        /*---------------------------------------------------------------------
          Check if PPP is active before clearing UM Rx watermark (Fwd Link) to
          avoid dropping of important packets.
        ---------------------------------------------------------------------*/
        if (FALSE == ds707_extif_is_ppp_up())
        {
          /*-------------------------------------------------------------------
            Empty UM Rx watermark (Fwd Link)
          -------------------------------------------------------------------*/
          DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Empty UM Rx wmk:0x%x having "
                        "current_count = %d",
                        rrcb_ptr->cfg_ptr->rx_wm_ptr[p],
                        rrcb_ptr->cfg_ptr->rx_wm_ptr[p]->current_cnt);
     
          if (rrcb_ptr->cfg_ptr->rx_wm_ptr[p]->current_cnt > 0)
          {
            /*-----------------------------------------------------------------
              Empty the dsm queue
            -----------------------------------------------------------------*/
            dsm_empty_queue(rrcb_ptr->cfg_ptr->rx_wm_ptr[p]);

            /*-----------------------------------------------------------------
              Reset the total received count
            -----------------------------------------------------------------*/
            rrcb_ptr->cfg_ptr->rx_wm_ptr[p]->total_rcvd_cnt = 0;
          }
        }
      }
      
      rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
    }

    /*-----------------------------------------------------------------------
      Reset the ver_ptr back to NULL, as this is used to tell if a particular
      rscb is being used/not used.

      Do this before releasing the txq and the delayed frame arrays, as
      the absence of those causes potential ERR_FATALs, if rxc still pushes
      data up to RLP as the call ends.
    -----------------------------------------------------------------------*/
    if ( rscb_ptr->cfg.ver == DSRLP_VER_1X_3)
    {
      dsm_unreg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                             DSM_MEM_LEVEL_LINK_LAYER_DNE,
                             DSM_MEM_OP_NEW);

      dsm_unreg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL,
                             DSM_MEM_LEVEL_LINK_LAYER_FEW,
                             DSM_MEM_OP_NEW);

      DATA_RLP_MSG3(MSG_LEGACY_HIGH, "RLP NAK Q Stats = %d %d %d",
                q_cnt( &dsrlpi_rscb_array[0].rlp_list[DSRSP_ROUTE_A].state.nak_q ),
                q_cnt( &dsrlpi_rscb_array[1].rlp_list[DSRSP_ROUTE_A].state.nak_q ),
                dsrlpnak_get_freenak_count());
      /*--------------------------------------------------------------------
        Now release the delayed frame arrays.
      -----------------------------------------------------------------------*/
#ifdef FEATURE_DS_RLP3
      for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
      {
        rrcb_ptr = &rscb_ptr->rlp_list[p];
        dsrlpdel_put_array(rrcb_ptr->state.del_fr_id);
      }
#endif
    } /* ver == DSRLP_VER_1X_3 */

#ifdef FEATURE_HDR
    else if( rscb_ptr->cfg.ver == DSRLP_VER_HDR)
    {
      /*--------------------------------------------------------------------
       Note that hdrrlp_cleanup needs to be called after dsrlpi_reset
       because dsrlpi_reset sets the RLP state to established, whereas,
       hdrrlp_cleanup will set it to NULL state.
     --------------------------------------------------------------------*/
        hdrrlp_cleanup( sr_id );
    }
#ifdef FEATURE_HDR_QOS
    else if(( rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
            ( rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ))
    {
        /*
          Remove this RLP from SPD's polling mechanism.
        */
        if ( rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE )
        {
          hdrmrlpspd_cleanup_for_rscb ( rscb_index );
        }

        hdrmrlp_cleanup( sr_id, rscb_ptr->cfg.ver );
    }
#endif /* FEATURE_HDR_QOS */

#endif /*FEATURE_HDR*/

    rscb_ptr->cfg.ver_ptr = NULL;

    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "dsrlp_cleanup complete called for RLP: %d ver:%d rscb:%d", sr_id, rlp_type, rscb_index );
  }

  (void)timer_clr(&dsrlp_timetest_cb, T_NONE);

  /*--------------------------------------------------------------------
    Now clear the tx and rx contiguous idle frame count
  -----------------------------------------------------------------------*/
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

    stats_ptr     = &rrcb_ptr->stats;
    stats_ptr->tx_contig_idle_fr_cnt = 0;
    stats_ptr->rx_contig_idle_fr_cnt = 0;
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);
} /* dsrlp_cleanup() */


/*===========================================================================

FUNCTION      DSRLP_PROCESS_INIT_CMD

DESCRIPTION   This function processes RLP INIT command received and does
              required initialization.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_process_init_cmd
(
  ds_cmd_type  *ds_cmd_ptr
)
{
  uint8 i = 0;
  dsrlp_blob_cmd_type   *dsrlp_blob_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, 
             "ds_cmd_ptr or cmd_payload_ptr is NULL. Can't process RLP BLOB.");
    return;
  }

  dsrlp_blob_ptr = (dsrlp_blob_cmd_type *)ds_cmd_ptr->cmd_payload_ptr;

#ifdef FEATURE_DATA_IS707
  ds707_rrm_call_init();
  ds707_rrm_reg_funcs();
#endif /* FEATURE_DATA_IS707 */

  /*
   The featurization in the call to the function below is due to the fact that the
   RLP is different between RevB and non RevB targets on 1H08. Since the function
   signatures have changed between these, and the higher layer calls the appropriate
   function based on FEATURE_HDR_REVB
  */
#if defined (FEATURE_HDR_REVB)
  dsrlp_init((byte)DSRLP_FIXED_SR_ID_INDEX,
              dsrlp_blob_ptr->sr_id,
              DSRLP_VER_1X_3,
              FALSE,
              DSRLP_VER_MFPA
            );
#else
  dsrlp_init((byte)DSRLP_FIXED_SR_ID_INDEX,
              dsrlp_blob_ptr->sr_id,
              DSRLP_VER_1X_3,
              FALSE,
              FALSE
            );
#endif

  /*-----------------------------------------------------------------------
    Now that we have initialized the rlp, go ahead and process the BLOB
  -----------------------------------------------------------------------*/
  if (dsrlp_blob_ptr->rlp_info_incl)
  {
    DATA_RLP_MSG2( MSG_LEGACY_HIGH, "Service ID %d, BLOB Len %d",
                   dsrlp_blob_ptr->sr_id,
                   dsrlp_blob_ptr->rlp_blob_len  );

    for(i=0; i<dsrlp_blob_ptr->rlp_blob_len; i++)
    {
      DATA_RLP_MSG1( MSG_LEGACY_HIGH, "BLOB data is %x",
                     dsrlp_blob_ptr->rlp_blob[i] );
    }

    /*-------------------------------------------------------------------
      this assumes that the session control block pointed  by the
      rscb_ptr that we gonna get by using this sr_id has been
      initialized and pointing to valid info. dsrlp_init() does that.
    -------------------------------------------------------------------*/
    if (dsrlp_process_rlp_blob(
                                dsrlp_blob_ptr->sr_id,
                                (byte *)dsrlp_blob_ptr->rlp_blob,
                                dsrlp_blob_ptr->rlp_blob_len
                              ) == FALSE)
    {
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "RLP BLOB DID NOT PARSE CORRECTLY!!!");
    }
  }

  rxc_init_sch_mux();

  /*------------------------------------------------------------------------
  1X RLP is initialized. RX/TX tasks can start data transfer now
  ------------------------------------------------------------------------*/
  dsrlp_set_1xrlp_ready_flag(TRUE);
} /* dsrlp_process_init_cmd */

/*===========================================================================

FUNCTION      DSRLP_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to RLP.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_process_cmd
(
  ds_cmd_type  *ds_cmd_ptr
)
{
  if( ds_cmd_ptr == NULL )
  {
    
    return;
  }

  switch (ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_RLP_INIT:
      dsrlp_process_init_cmd(ds_cmd_ptr);
      break;

    default:
      DATA_RLP_MSG1( MSG_LEGACY_ERROR, "Unknown RLP cmd %d", ds_cmd_ptr->hdr.cmd_id );
      break;
  }
} /* dsrlp_process_cmd */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION      DSRLP_ACTIVATE_HDRMRLPCTL_BLK

DESCRIPTION   This Activates the HDR Multi Flow RLP. It reserves an RSCB index
              for the particular RLP (sr_id). The control block is needed in
              order to maintain state of the RLP even when the RLP is not
              initialized. For example, the RLP may be Active Open or Active
              Close irrespective of the state of the connection.

              Note that this function is specific for HDR MultiFlow RLP

DEPENDENCIES  None

RETURN VALUE  The RLP instance, if successfully activated.
              an invalid index otherwise

SIDE EFFECTS  None
===========================================================================*/
byte dsrlp_activate_hdrmrlp_ctl_blk
(
  byte rscb_index,                           /* index in the rscb array where
                                                this record will be stored */
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver                    /* RLP version                */
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  byte                            ret_val= DSRLPI_INVALID_RSCB_INDEX;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);

  ASSERT(( ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
         ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ));

  /*-------------------------------------------------------------------------
    Enter critical section. HDR Data call flow does not run on interrupt
    context and so critical section suffice here.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);

  /*---------------------------------------------------------------------
  If the control block for this RLP is already being used, then cannot
  activate it again. Check to see if the RLP is active. If it is ensure
  that it is an HDR MultiFlow Type RLP.
  ---------------------------------------------------------------------*/
  if( rscb_ptr->is_hdrmrlp_active == TRUE )
  {
    ASSERT(( rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
           ( rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ));

    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "RLP Ctl Blk %d already active. sr_id: %d ver: %d",
             rscb_index, rscb_ptr->cfg.sr_id, rscb_ptr->cfg.ver);
  }

  /*---------------------------------------------------------------------
  Ensure that the RLP control block is not in use. If the ver_ptr is
  non NULL, that means it is currently being used.
  ---------------------------------------------------------------------*/
  else if( rscb_ptr->cfg.ver_ptr != NULL )
  {
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Cannot activate. RLP ctl blk %d in use sr_id: %d ver %d",
             rscb_index, rscb_ptr->cfg.sr_id, rscb_ptr->cfg.ver);
  }
  else
  {
    /*---------------------------------------------------------------------
    The RLP control block is inactive and not being used. Assign the RLP
    Flow and version for this RLP and set the flag to activate it.
    ---------------------------------------------------------------------*/
    rscb_ptr->is_hdrmrlp_active = TRUE;
    rscb_ptr->cfg.sr_id = sr_id;
    rscb_ptr->cfg.ver = ver;
    ret_val = rscb_index;
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);

  return ret_val;
} /* dsrlp_activate_hdrmrlp_ctl_blk() */
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION      DSRLP_DEACTIVATE_HDRMRLPCTL_BLK

DESCRIPTION   This Deactivates the HDR Multi Flow RLP.

It reserves an RSCB index
              for the particular RLP (sr_id). The control block is needed in
              order to maintain state of the RLP even when the RLP is not
              initialized. For example, the RLP may be Active Open or Active
              Close irrespective of the state of the connection.

              Note that this function is specific for HDR MultiFlow RLP

DEPENDENCIES  None

RETURN VALUE  The RLP instance, if successfully activated.
              an invalid index otherwise

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_deactivate_hdrmrlp_ctl_blk
(
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver                    /* RLP version                */
)
{
  byte                     rscb_index;
  dsrlpi_sess_ctl_blk_type *rscb_ptr;     /* session control block pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(( ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
         ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ));

  rscb_index = dsrlp_get_rlp_rscb_index(sr_id, ver);

  if( rscb_index >= DSRLPI_INVALID_RSCB_INDEX )
  {
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Cant Deactivate. rscb for sr_id%d ver %d not found",
              sr_id, ver);
    return;
  }

  /*---------------------------------------------------------------------
  Ensure that the function returned a correct rscb index
  ---------------------------------------------------------------------*/
  ASSERT(( dsrlpi_rscb_array[rscb_index].is_hdrmrlp_active == TRUE ) &&
         ( dsrlpi_rscb_array[rscb_index].cfg.ver == ver ));

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    rscb_index (corresponding to an sr_id).
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[rscb_index] );

  /*-------------------------------------------------------------------------
    Enter critical section. HDR Data call flow does not run on interrupt
    context and so critical section suffice here.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);

  dsrlpi_rscb_array[rscb_index].is_hdrmrlp_active = FALSE;
  dsrlpi_rscb_array[rscb_index].cfg.ver_ptr = NULL;

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);

} /* dsrlp_deactivate_hdrmrlp_ctl_blk() */
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION      DSRLP_IS_RLP_ACTIVE

DESCRIPTION   Returns whether the RLP is activated or not.
              Note that this function is specific for HDR MultiFlow RLP

DEPENDENCIES  None

RETURN VALUE  TRUE if RLP is active
              FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_rlp_active
(
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver                    /* RLP version                */
)
{
  byte rscb_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(( ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD ) ||
         ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE ));

  rscb_index = dsrlp_get_rlp_rscb_index(sr_id,ver);

  /*-------------------------------------------------------------------------
  If RLP index is not found, then the RLP is not activated.
  -------------------------------------------------------------------------*/
  if( rscb_index == DSRLPI_INVALID_RSCB_INDEX )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* dsrlp_is_rlp_active() */
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION       DSRLP_UPDATE_ACTIVE_MRLP_CHANGED_PARAMS

DESCRIPTION    Determines the parameters that have changed for an active RLP.
               If any have chnaged, it sets a flag to the RLP indicating that
               parameters have changed and need to be updated.

               Note that currently this function only works for the the
               Multiflow Packet App bound to the SN. Will need to check
               for the app subtype if it is to be supported for the AN stream.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlp_update_active_mrlp_changed_params
(
  byte sr_id,
  dsrlp_ver_enum_type ver
)
{
  byte rscb_index;
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;/* session control block pointer     */
  boolean timers_attrib_changed = FALSE;
  hdrcp_mrlpc_flow_timers_type flow_timers;
  boolean nak_enabled_attrib_changed = FALSE;
  boolean nak_enabled;
  uint8 phys_link_nak_enabled_rev_attrib_changed = FALSE;
  uint8 phys_link_nak_enabled_rev;
  uint8 data_unit_attrib_changed = FALSE;
  uint8 data_unit = 0x00;
  uint8 route_protocol_sdu_attrib_changed = FALSE;
  uint8 route_protocol_sdu = 0x00;
  uint8 out_of_order_delivery_to_route_protocol_fwd_attrib_changed = FALSE;
  uint8 out_of_order_delivery_to_route_protocol_fwd = 0x00;
  boolean transmit_abort_timer_rev_val_changed = FALSE;
  uint32 transmit_abort_timer_rev_val = 0x00;
  uint8 flow_prot_parm_fwd_changed = FALSE;
  uint8 flow_prot_parm_rev_changed = FALSE;
  uint8 route_prot_parm_fwd_changed = FALSE;
  uint8 route_prot_parm_rev_changed = FALSE;
  hdrcp_mrlpc_flow_prot_parm_rev_type flow_prot_parm_rev;
  hdrcp_mrlpc_flow_prot_parm_fwd_type flow_prot_parm_fwd;
  hdrcp_mrlpc_route_prot_parm_rev_type route_prot_parm_rev;
  hdrcp_mrlpc_route_prot_parm_fwd_type route_prot_parm_fwd;

  memset(&flow_timers, 0, sizeof(hdrcp_mrlpc_flow_timers_type));
  memset(&flow_prot_parm_rev, 0, sizeof(hdrcp_mrlpc_flow_prot_parm_rev_type));
  memset(&flow_prot_parm_fwd, 0, sizeof(hdrcp_mrlpc_flow_prot_parm_fwd_type));
  memset(&route_prot_parm_rev, 0, sizeof(hdrcp_mrlpc_route_prot_parm_rev_type));
  memset(&route_prot_parm_fwd, 0, sizeof(hdrcp_mrlpc_route_prot_parm_fwd_type));
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_index = dsrlp_get_rlp_rscb_index(sr_id,ver);
  if (rscb_index == DSRLPI_INVALID_RSCB_INDEX)
  {
    /* Cannot process an entry that doesn't exist! */
    DATA_RLP_MSG2(MSG_LEGACY_ERROR, "Did not find RLP %d ver %d",sr_id,ver);
    return;
  }

  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);
  ASSERT((rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) ||
         (rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE));

  (void)hdrmrlpcif_get_timers_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                sr_id,
                                      ver,
                                &flow_timers,
                                &timers_attrib_changed );

  (void)hdrmrlpcif_get_nak_enabled_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                sr_id,
                                           ver,
                                &nak_enabled,
                                &nak_enabled_attrib_changed );

  (void)hdrmrlpcif_get_phys_layer_nak_en_rev_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                sr_id,
                                                     ver,
                                &phys_link_nak_enabled_rev,
                                &phys_link_nak_enabled_rev_attrib_changed );

  if ( ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE )
  {
    (void) hdrmrlpcif_get_transmit_abort_timer_rev_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                  sr_id,
                                  ver,
                                  &transmit_abort_timer_rev_val,
                                  &transmit_abort_timer_rev_val_changed);

    if ( transmit_abort_timer_rev_val_changed == TRUE )
    {
        /*
        The change in the transmit abort timer value for a default RLP flow is to be ignored. This is because we do
        not plan to support SPD on the default flow.
        */
        if (TRUE == hdrmrlpctl_is_rlp_default(sr_id, ver))
        {
          transmit_abort_timer_rev_val = 0;
        }
      hdrmrlpspd_transmit_abort_timer_changed_notif ( rscb_index,  transmit_abort_timer_rev_val*5 );
    }
  } /* if DSRLP_VER_HDR_MULTIFLOW_REVERSE */

  /* EMPA/MMPA specific atributes */
  if (rscb_ptr->cfg.enhanced == DSRLP_ENHANCED_VER_MMPA ||
      rscb_ptr->cfg.enhanced == DSRLP_ENHANCED_VER_EMPA)
  {
    (void)hdrmrlpcif_get_data_unit_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                           sr_id,
                                           ver,
                                           &data_unit,
                                           &data_unit_attrib_changed );

    (void)hdrmrlpcif_get_route_protocol_sdu_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                                    sr_id,
                                                    ver,
                                                    &route_protocol_sdu,
                                                    &route_protocol_sdu_attrib_changed );

    if (route_protocol_sdu == 0x01 &&
        ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
    {
      (void)hdrmrlpcif_get_out_of_order_delivery_to_route_protocol_fwd_attrib( HDRMRLPCIF_CLIENT_MRLP_SN,
                                                                               sr_id,
                                                                               ver,
                                                                               &out_of_order_delivery_to_route_protocol_fwd,
                                                                               &out_of_order_delivery_to_route_protocol_fwd_attrib_changed );
    }

    if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        (void)hdrmrlpcif_get_flow_protocol_parameters_fwd_attrib(HDRMRLPCIF_CLIENT_MRLP_SN,
                                 sr_id,
                                 &flow_prot_parm_fwd,
                                 &flow_prot_parm_fwd_changed);
        (void)hdrmrlpcif_get_route_protocol_parameters_fwd_attrib(HDRMRLPCIF_CLIENT_MRLP_SN,
                                                               sr_id,
                                                               &route_prot_parm_fwd,
                                                               &route_prot_parm_fwd_changed );
    }
    else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
    {
      (void)hdrmrlpcif_get_flow_protocol_parameters_rev_attrib(HDRMRLPCIF_CLIENT_MRLP_SN,
                                                               sr_id,
                                                               &flow_prot_parm_rev,
                                                               &flow_prot_parm_rev_changed );
      (void)hdrmrlpcif_get_route_protocol_parameters_rev_attrib(HDRMRLPCIF_CLIENT_MRLP_SN,
                                                               sr_id,
                                                               &route_prot_parm_rev,
                                                               &route_prot_parm_rev_changed );
    }

  }

  if( timers_attrib_changed ||
      nak_enabled_attrib_changed ||
      phys_link_nak_enabled_rev_attrib_changed ||
      data_unit_attrib_changed ||
      route_protocol_sdu_attrib_changed ||
      out_of_order_delivery_to_route_protocol_fwd_attrib_changed ||
      transmit_abort_timer_rev_val_changed ||
      flow_prot_parm_fwd_changed ||
      flow_prot_parm_rev_changed ||
      route_prot_parm_fwd_changed ||
      route_prot_parm_rev_changed)

  {
    /* Enter critical section */
    dsrlpi_enter_control_path_crit_sect( rscb_ptr, rscb_ptr->cfg.ver );

    /* Copy all elements to the temporary structure in the RLP control block */
    rscb_ptr->cfg.has_active_rlp_cfg_changed = TRUE;
    rscb_ptr->cfg.changed_flush_timer_val = flow_timers.flush_timer_val;
    rscb_ptr->cfg.changed_nak_abort_timer_val = flow_timers.abort_timer_val;
    rscb_ptr->cfg.changed_nak_enabled = (boolean) nak_enabled;
    rscb_ptr->cfg.changed_nak_delay_timer_val = flow_timers.nak_delay_timer_val;
    rscb_ptr->cfg.changed_segment_sequencing = (data_unit == 0x01);
    rscb_ptr->cfg.changed_packet_stream = (route_protocol_sdu == 0x01);
    rscb_ptr->cfg.changed_out_of_order_allowed = (out_of_order_delivery_to_route_protocol_fwd == 0x01);
    rscb_ptr->cfg.changed_phys_link_nak_enabled_rev_val = phys_link_nak_enabled_rev;

    if (rscb_ptr->cfg.enhanced == DSRLP_ENHANCED_VER_EMPA ||
        rscb_ptr->cfg.enhanced == DSRLP_ENHANCED_VER_MMPA)
    {
      if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        /*lint -save -e644 Dont know how to initialize the following structures without lint errors */
            rscb_ptr->cfg.changed_flow_protocol  = flow_prot_parm_fwd.id;
            rscb_ptr->cfg.changed_route_protocol = route_prot_parm_fwd.id;
        /*lint -restore*/
        }
        else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
        {
        /*lint -save -e644 Dont know how to initialize the following structures without lint errors */
            rscb_ptr->cfg.changed_flow_protocol  = flow_prot_parm_rev.id;
            rscb_ptr->cfg.changed_route_protocol = route_prot_parm_rev.id;
        /*lint -restore*/
        }
    }

    /* This parameter does not need to be copied in another variable, as this is not actively
       used by RLP during the building packets */
    if ( rscb_ptr->cfg.ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE )
    {
      rscb_ptr->cfg.transmit_abort_timer_rev_val = transmit_abort_timer_rev_val*5;
    }
    /* Leave critical section */
    dsrlpi_leave_control_path_crit_sect( rscb_ptr, rscb_ptr->cfg.ver );
  }

} /* dsrlp_update_active_mrlp_changed_params() */

#endif /* FEATURE_HDR_QOS */
/*===========================================================================

FUNCTION       DSRLP_GET_RLP_RCSB_INDEX

DESCRIPTION    Get the RLP instance (rscb array index) used by given RLP.
               The RLP is identified by RLP Type and ID (Flow).

               Note that if the RLP version is an HDR MultiFlow RLP Type,
               then the function returns the rscb index as long as the RLP
               is active.
               In case of RLP3 and HDR RLP, the function will return the rscb
               index only if the RLP is initialized (ver_ptr is not NULL)

               The function is optimized so that the search for the rscb
               index is done only for the range reserved for the particular
               version of the RLP, rather than searching through all the
               rscb indices.

DEPENDENCIES   None

RETURN VALUE   The RLP instance, if found, an invalid index otherwise

SIDE EFFECTS   None
===========================================================================*/
byte dsrlp_get_rlp_rscb_index
(
  byte sr_id,
  dsrlp_ver_enum_type ver
)
{
#ifdef FEATURE_HDR
  byte i; /* Index counter */
#endif /* FEATURE_HDR */
  byte rlp_index = DSRLPI_INVALID_RSCB_INDEX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( ver )
  {
    case DSRLP_VER_1X_3:
      if(( dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX].cfg.ver_ptr != NULL ) &&
         ( dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX].cfg.sr_id == sr_id ))
      {
        rlp_index = DSRLP_FIXED_SR_ID_INDEX;
      }
      break;

#ifdef FEATURE_HDR

    case DSRLP_VER_HDR:
      for( i = DSRLP_HDR_BASE_INDEX;
           i < (DSRLP_HDR_BASE_INDEX+DSRLP_MAX_NUM_HDR_RLP);
           i++ )
      {
        if((dsrlpi_rscb_array[i].cfg.ver_ptr != NULL) &&
           (( dsrlpi_rscb_array[i].cfg.sr_id == sr_id) &&
            ( dsrlpi_rscb_array[i].cfg.ver == ver )))
        {
          rlp_index = i;
          break;
        }
      }
      break;

    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
      for( i = DSRLP_HDR_MRLP_FWD_BASE_INDEX;
           i < (DSRLP_HDR_MRLP_FWD_BASE_INDEX+DSRLP_MAX_NUM_HDR_MRLP_FWD);
           i++ )
      {
        if((dsrlpi_rscb_array[i].is_hdrmrlp_active == TRUE) &&
           (( dsrlpi_rscb_array[i].cfg.sr_id == sr_id) &&
            ( dsrlpi_rscb_array[i].cfg.ver == ver )))
        {
          rlp_index = i;
          break;
        }
      }
      break;

    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      for( i = DSRLP_HDR_MRLP_REV_BASE_INDEX;
           i < (DSRLP_HDR_MRLP_REV_BASE_INDEX+DSRLP_MAX_NUM_HDR_MRLP_REV);
           i++ )
      {
        if((dsrlpi_rscb_array[i].is_hdrmrlp_active == TRUE) &&
           (( dsrlpi_rscb_array[i].cfg.sr_id == sr_id) &&
            ( dsrlpi_rscb_array[i].cfg.ver == ver )))
        {
          rlp_index = i;
          break;
        }
      }
      break;
#endif /* FEATURE_HDR */

    default:
      ERR_FATAL("Cant find RLP ver %d", (uint32)ver,0,0);
      break;
  }

  return rlp_index;
} /* dsrlp_get_rlp_rscb_index() */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION       DSRLP_GET_FREE_RCSB_INDEX

DESCRIPTION    This function returns an index to a free RLP control block of
               the version specified. By free, it means that this index has
               not been reserved for any RLP.

               For the HDRMRLP, activating an RLP reserves an index,
               deactivating frees the index.

DEPENDENCIES   None

RETURN VALUE   If a free index of the version specified is found, it returns
               the index, otherwise an invalid index is returned.

SIDE EFFECTS   None
===========================================================================*/
byte dsrlp_get_free_rscb_index
(
  dsrlp_ver_enum_type ver
)
{
  byte i; /* Index counter */
  byte rlp_index = DSRLPI_INVALID_RSCB_INDEX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_RLP_MSG1(MSG_LEGACY_HIGH,"RLP ver support %d", ver);
  switch( ver )
  {
    case DSRLP_VER_1X_3:
    case DSRLP_VER_HDR:
      
      break;

    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
      for( i = DSRLP_HDR_MRLP_FWD_BASE_INDEX;
           i < (DSRLP_HDR_MRLP_FWD_BASE_INDEX+DSRLP_MAX_NUM_HDR_MRLP_FWD);
           i++ )
      {
        if(dsrlpi_rscb_array[i].is_hdrmrlp_active == FALSE)
        {
          rlp_index = i;
          break;
        }
      }
      break;

    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      for( i = DSRLP_HDR_MRLP_REV_BASE_INDEX;
           i < (DSRLP_HDR_MRLP_REV_BASE_INDEX+DSRLP_MAX_NUM_HDR_MRLP_REV);
           i++ )
      {
        if(dsrlpi_rscb_array[i].is_hdrmrlp_active == FALSE)
        {
          rlp_index = i;
          break;
        }
      }
      break;

    default:
      
      break;
  }

  return rlp_index;
} /* dsrlp_get_free_rscb_index() */
#endif /* FEATURE_HDR_QOS */

#ifndef MSM5000_IRAM_FWD
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION DSRLP_PROCESS_RX_FRAMES

DESCRIPTION
  This function processes the RLP frame(s) received over the air. This is the
  interface between RLP receive and the multiplex sublayer. The multiplex
  sublayer can send all the frames it received in one frame time to RLP or
  split it up into multiple calls of this function.

  Input Parameters:

  The parameter to this function is a pointer to a structure which contains
  an array of frames that the mux sublayer wants RLP to process. Each element
  of this array includes all the information that RLP needs to process the
  frame independently. The information passed with each frame includes a
  pointer to the dsm item containing the frame, the muxPDU type, service type
  (primary or secondary).

  The fr_item_ptr can be a pointer to dsm packet chain (with multiple
  dsm items).

  This per frame data structure is passed to a number of functions within
  the RLP code.

DEPENDENCIES
  dsrlp_init() should have been previously invoked. rlp_establish_srvc_type()
  should have been previously invoked to set up the traffic type, rate set
  and RLP type in use.

  The statistics updating in this function assume that it will be called
  once per 20ms period. Alternatively we can use a new interface that will
  tell whether a call corresponds to a Fundamental frame.

  The init state machine requires that MUX-level erasures are only sent to
  the RLP layer when the erasure occurs on the FCH/DCCH.  Erasures on the
  SCH's should not be sent up by the MUX.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_process_rx_frames
(
  dsrlp_fwd_frame_block_type *fwd_frame_block/* Ptr to rcvd RLP frames     */
)
{
  dsrlpi_sess_ctl_blk_type*rscb_ptr;         /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr;         /* rlp-specific control block */
  dsrlp_fwd_frame_type    *fwd_frame_ptr;    /* info for one rlp frame     */
  struct dsrlpi_fcn_ptr_table *fcn_ptr_table;/* Ptr table to RLP functions */
  dsrlpi_fr_det_enum_type  frame_type;       /* New/Rexmit/NAK/Idle/Init ? */
  byte                     frame_no;         /* Frame counter              */
  dsrlpi_curr_fr_info_type curr_fr_info;     /* Post processed frame data  */
  dsrlpi_curr_fr_info_type idle_fr_info;     /* Post processed idle frame  */
  boolean                  idle_fr_rxed;     /* Idle fr rxed this 20 msec? */
  dsrlpi_state_info_type *state_ptr;         /* local state ptr            */
  dsrlp_stats_info_type   *stats_ptr;        /* local stats ptr            */
  int                      i;                /* for...loop invariant       */
  boolean                  rx_data_frs;      /* received any data frs?     */
  static uint16            current_erasure=0;/* help count contig erasures */
  boolean                  age_nak_list = FALSE;/*should nak list be aged? */
  word                     curr_rlp_delay;   /* rlp_delay on fcn entry     */
  event_rlp_payload_type   rlp_event_payload;/* payload for RLP events     */
  event_rlp_nak_abort_payload_type rlp_nak_abort_event_payload;
                                       /* payload for RLP NAK ABORT events */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    The call may have ended, but the message has not yet gotten to rxc.
    So go ahead and check to see if the ver_ptr is null, and if so, then
    dump the rxc frames.

    This may also happen if the user has not setup both the rx_func_ptr
    and rx_queue.

    NOTE: Although the variable is named fwd_frame_block->sr_id, it is
    actually the RLP instance for this service (Currently only one RLP
    instance is supported)
  -------------------------------------------------------------------------*/
  rscb_ptr      = &( dsrlpi_rscb_array[fwd_frame_block->sr_id]);

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
    Increase the 20 ms counter that counts # fundamental frames. This assumes
    that this function is called once per 20ms period and not multiple times.
  -------------------------------------------------------------------------*/
  rrcb_ptr->stats.rx_20ms_fr_cnt++;

  if ((rscb_ptr->cfg.ver_ptr == NULL) ||
      (rscb_ptr->cfg.rx_func_ptr == NULL) ||
      (rscb_ptr->cfg.rx_wm_ptr[DSRSP_ROUTE_A] == NULL)
     )
  {
    for (i = 0; i < fwd_frame_block->num_rlp_frames; i++)
    {
      dsm_free_packet(&(fwd_frame_block->fwd_arr[i].fr_item_ptr));
    }
    mem_critical_event = FALSE;
    mem_warning_event = FALSE;
    return;
  }

  /*-------------------------------------------------------------------------
    If PPP data was received after dormancy and notification was requested,
    notify the required module (PZID hysterisis).
  -------------------------------------------------------------------------*/
  if((rscb_ptr->cfg.notify_ppp_data_transfer_func_ptr != NULL) &&
     (rrcb_ptr->state.ppp_data_transfer_after_dormancy == TRUE))
  {
    rscb_ptr->cfg.notify_ppp_data_transfer_func_ptr(DSRLP_FIXED_SR_ID_INDEX);
  }

  /*-------------------------------------------------------------------------
    Indicate that no IDLE frames were rxed.  If an Idle frame is rxed this
    20 msec, then it is processed last.  This helps to account for delayed
    frames on the SCH.
  -------------------------------------------------------------------------*/
  idle_fr_rxed = FALSE;

  /*-------------------------------------------------------------------------
    In order to support dormant mode, need to know if any data/ctl frames
    were received this 20msec period.  The following boolean will be set
    to TRUE if any data frames are received.
  -------------------------------------------------------------------------*/
  rx_data_frs = FALSE;

  /*-------------------------------------------------------------------------
    Enter DSM memory level critical section. mem_critical_event and
    mem_warning_event global variables are modified upon DSM memory level
    callback dsrlp_mem_event(). Since dsrlp_process_rx_frames() runs on MUX
    context it can prempt dsrlp_mem_event() and so entering critical section
    here while checking and updating memory related event variables.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&dsm_mem_level_crit_sect);

  /*-------------------------------------------------------------------------
    Check the memory event variables. If they are set, then some action
    may be required, over all RLP sessions.  Normal processing can then
    proceed.  In the case of a reset, the received frames will be dumped
    as bad frames, unless they happen to be sync frames.
  -------------------------------------------------------------------------*/
  if (mem_critical_event == TRUE)
  {
    /*-----------------------------------------------------------------------
      Small items are in short supply.  Reset all RLP instances; otherwise
      data will start to be dropped on the floor, eventually resulting in
      rlp resets anyways.
    -----------------------------------------------------------------------*/
    for(i = 0; i < DSRLP_MAX_RSCB_ELEMENTS; i++)
    {
      if (dsrlpi_rscb_array[i].cfg.ver_ptr != 0)
      {
        DATA_RLP_MSG1(MSG_LEGACY_HIGH, "mem critical - resetting RLP %d",i);

        dsrlpi_reset(&(dsrlpi_rscb_array[i].rlp_list[DSRSP_ROUTE_A]));

       /*--------------------------------------------------------------------
          Report RLP reset event
       --------------------------------------------------------------------*/
       rlp_event_payload.event_rlp_type = event_rlp_reset;
       rlp_event_payload.info = rscb_ptr->cfg.sr_id;
       event_report_payload (EVENT_RLP,
                             sizeof(event_rlp_payload_type),
                             &rlp_event_payload);

      }
    }
    mem_critical_event = FALSE;
  }
  else if (mem_warning_event == TRUE)
  {
    mem_warning_event = FALSE;
  }

  /*-------------------------------------------------------------------------
    Leave DSM memory level critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&dsm_mem_level_crit_sect);

  /*-------------------------------------------------------------------------
    Using the sr_id, get the session control block and fcn_ptr_table.
  -------------------------------------------------------------------------*/
  state_ptr     = &rrcb_ptr->state;
  stats_ptr     = &rrcb_ptr->stats;
  fcn_ptr_table = rscb_ptr->cfg.ver_ptr->fcn_ptr_table;
  ASSERT( fcn_ptr_table != NULL);

  /*-------------------------------------------------------------------------
    Record the current rlp_delay (the RTT).  At the end of this function,
    make sure that the rlp_delay has only incremented by one (max).  This
    covers the case where we receive SYNC frames on both FCH & DCCH in a
    VP2 call.
  -------------------------------------------------------------------------*/
  curr_rlp_delay = state_ptr->rlp_delay;

  /*-------------------------------------------------------------------------
    Check if the maximum time to achieve synchronization has been exceeded.
    Discard the received frame(s), end call  and return, if so.
  -------------------------------------------------------------------------*/
  if(((state_ptr->state == DSRLPI_SYNC_STATE)    ||
      (state_ptr->state == DSRLPI_SYNC_ACK_STATE)||
      (state_ptr->state == DSRLPI_ACK_STATE))    &&
      (++(state_ptr->sync_delay) == DSRLPI_MAX_RTT_SETUP))
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "SYNC state Timeout");
    /*-----------------------------------------------------------------------
      Free the buffers for all the rx'ed RLP frames, reset RLP and return.
    -----------------------------------------------------------------------*/
    for(i = 0; i < fwd_frame_block->num_rlp_frames; i++)
    {
      dsm_free_packet(&(fwd_frame_block->fwd_arr[i].fr_item_ptr));
    }

    /*-----------------------------------------------------------------------
      Reset RLP. This will cleanup all the memory and initialiaze all state
      variables. This also updates the "resets" stat.
    -----------------------------------------------------------------------*/
    dsrlpi_reset( rrcb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->cfg.sr_id;
    event_report_payload (EVENT_RLP,
                          sizeof(event_rlp_payload_type),
                          &rlp_event_payload);



    /*-----------------------------------------------------------------------
      Call this function in dsmgr.c to end the current call
    -----------------------------------------------------------------------*/
    ds707_cta_rlp_sync_timeout();

    /*-----------------------------------------------------------------------
      Set the counter back to 0 as we now have reached the limit and the
      call will be dropped
    -----------------------------------------------------------------------*/
    state_ptr->sync_delay = 0;

    /*-----------------------------------------------------------------------
      Now just return, nothing else to do here
    -----------------------------------------------------------------------*/
    return;
  }

  /*-------------------------------------------------------------------------
    Within a frame block we can receive multiple RLP frames.  Loop
    until all frames have been processed
  -------------------------------------------------------------------------*/
  for (frame_no=0;
       frame_no < fwd_frame_block->num_rlp_frames;
       frame_no++)
  {
    /*-----------------------------------------------------------------------
      Get a frame.  Fundamental channel frames are at index 0,
      while supplemental channel frames are at index > 0.
      Initialize curr_fr_info for this frame.
    -----------------------------------------------------------------------*/
    fwd_frame_ptr = &( fwd_frame_block->fwd_arr[frame_no]);
    memset( &curr_fr_info, 0, sizeof( curr_fr_info));

    /*-----------------------------------------------------------------------
      If the frame is valid (determined by conditional above or by
      MUX layer), then determine its frame type using the RLP
      specific frame determination function.
    -----------------------------------------------------------------------*/
    if( (fwd_frame_ptr->fr_rate != DSRLP_RATE_ERASURE) &&
        (fwd_frame_ptr->fr_rate != DSRLP_RATE_BLANK)   &&
        (fwd_frame_ptr->fr_rate != DSRLP_RATE_ERASURE_SCH) &&
        (fwd_frame_ptr->fr_rate != DSRLP_RATE_DCCH_DTX)    &&
        (fwd_frame_ptr->fr_rate != DSRLP_RATE_INACTIVE))
    {
      frame_type= fcn_ptr_table->rx_frame_det( rrcb_ptr, fwd_frame_ptr,
                                               &curr_fr_info);

      /*---------------------------------------------------------------------
        If RLP is not established, see if the received frame was Idle or New.
        In this case RLP state moves from ACK or RT_MEAS to ESTABLISHED.
        A frame other than IDLE or New will be discarded, and state reset
        to sending SYNCs.
      ---------------------------------------------------------------------*/
      rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
      if ( rrcb_ptr->state.state != DSRLPI_ESTABLISHED_STATE)
      {
        /*-------------------------------------------------------------------
          Do not go dormant if RLP has not synced up
        -------------------------------------------------------------------*/
        rx_data_frs = TRUE;

        if ((state_ptr->state == DSRLPI_ACK_STATE) ||
            (state_ptr->state == DSRLPI_RT_MEAS_STATE))
        {
          /*-----------------------------------------------------------------
            In this state, either will transition to the est'd state, or
            need to increment the rlp_delay count (in the case of erasures.
            Otherwise, down below, the frame will be passed to rx_init
            and processed accordingly.
          -----------------------------------------------------------------*/
          if (frame_type == DSRLPI_RLP_ERASURE)
          {
            state_ptr->rlp_delay++;
          }
          else if (frame_type != DSRLPI_INIT)
          {
            state_ptr->state= DSRLPI_ESTABLISHED_STATE;

            /*---------------------------------------------------------------
              Set this back to zero now that we are out of all 3 init states
            ---------------------------------------------------------------*/
            state_ptr->sync_delay = 0;

            DATA_RLP_MSG2(MSG_LEGACY_HIGH, "RLP (sr_id=%d) SYNCED UP with RTT=%d",
                      rscb_ptr->cfg.sr_id, state_ptr->rlp_delay);
            stats_ptr->last_rtt = state_ptr->rlp_delay;
          }
        }
        else if( frame_type != DSRLPI_INIT)
        {
          /*-----------------------------------------------------------------
            in the sync or sync-ack state, and so are expecting some kind of
            init frame.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Got frame_type=%d, when state=%d. Discarding frame. RLP:%d",
                    frame_type, state_ptr->state, rscb_ptr->cfg.sr_id);
          /*-----------------------------------------------------------------
            If the frame type is an RLP erasure or a NULL, then let the
            handling below take care of it, and do not reset RLP.  Otherwise,
            just free the packet and return.  In the latter case, since the
            frame was not what was expected, go ahead and reset RLP, so that
            RLP returns to the SYNC state (with all variables set correctly).
          -----------------------------------------------------------------*/
          if ((frame_type != DSRLPI_RLP_ERASURE) &&
              (frame_type != DSRLPI_NULL))
          {
            
            DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Used = 0x%x Bytes = 0x%x 0x%x",
                      dsrlpi_ctl_used,dsrlpi_ctl_byte1,dsrlpi_ctl_byte2);
            if (state_ptr->state != DSRLPI_SYNC_STATE)
            {
              rrcb_ptr->state.reset_rlp = TRUE;           /* Set the reset flag */
            }
            dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
            frame_type = DSRLPI_RESET;
          }
        }
      } /* if RLP not ESTABLISHED */
      rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
    }   /* if not ERASURE */
    else if (fwd_frame_ptr->fr_rate == DSRLP_RATE_ERASURE_SCH)
    {
      /*---------------------------------------------------------------------
        erasure on the SCH... discard frame and do no further processing.
        Do not call this an MUX_ERASURE, as that implies a MUX_ERASURE on
        the FCH.

        Free dsm item for safety.
      ---------------------------------------------------------------------*/
      dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
      frame_type = DSRLPI_DONE;
    }
    else if (fwd_frame_ptr->fr_rate == DSRLP_RATE_INACTIVE)
    {
      /*---------------------------------------------------------------------
       blank item on the inactive channel, discard frame and do no further
       processing. Free dsm item for safety.
      ---------------------------------------------------------------------*/
      dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
      frame_type = DSRLPI_DONE;
    }
    else
    {
      /*---------------------------------------------------------------------
        Received either an erasure or blank from MUX.  Specify frame type
        as such, so correctly processed below.  Increment the rlp_delay
        if in that state.
      ---------------------------------------------------------------------*/
      if ((fwd_frame_ptr->fr_rate == DSRLP_RATE_BLANK) ||
          (fwd_frame_ptr->fr_rate == DSRLP_RATE_DCCH_DTX))
      {
        stats_ptr->rx_blank_fr_cnt++;
      }
      if ((state_ptr->state == DSRLPI_ACK_STATE) ||
          (state_ptr->state == DSRLPI_RT_MEAS_STATE))
      {
        state_ptr->rlp_delay++;
      }
      if (fwd_frame_ptr->fr_rate == DSRLP_RATE_DCCH_DTX)
      {
        frame_type = DSRLPI_DCCH_DTX;
      }
      else
      {
        frame_type = DSRLPI_MUX_ERASURE;
      }
    }

    /*-----------------------------------------------------------------------
      Start processing the frame.  Since this can be an iterative
      process per frame, we keep looping til we resolve being done
      with this frame.  For example, a new data frame could have a
      CRC error, and then it turns into a ERASURE...
    -----------------------------------------------------------------------*/
    while (frame_type != DSRLPI_DONE)
    {
      if (frame_type != DSRLPI_MUX_ERASURE)
      {
        current_erasure = 0;
      }/* if */

      switch (frame_type)
      {
        case DSRLPI_NEW:
          rx_data_frs = TRUE;
          frame_type= fcn_ptr_table->rx_new_data( rrcb_ptr,
                                                  &(fwd_frame_ptr->fr_item_ptr),
                                                  &curr_fr_info);
          /*---------------------------------------------------------------
            Increase the count of new frames received. Ignore rare case it
            is not immediately DSRLPI_DONE.
          ---------------------------------------------------------------*/
          stats_ptr->rx_new_data_fr_cnt++;
          rrcb_ptr->state.last_fill_nak_cnt++;
          age_nak_list = TRUE;
          break;

        case DSRLPI_REXMIT:
          rx_data_frs = TRUE;
          frame_type= fcn_ptr_table->rx_rexmit( rrcb_ptr, fwd_frame_ptr,
                                                &curr_fr_info);
          /*---------------------------------------------------------------
            Increase the count of re-transmitted  frames received.
          ---------------------------------------------------------------*/
          stats_ptr->rx_rexmit_fr_cnt++;
          break;

        case DSRLPI_INIT:
          rx_data_frs = TRUE;
          frame_type= fcn_ptr_table->rx_init( rrcb_ptr, &curr_fr_info);
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          break;

        case DSRLPI_IDLE:
          /*-----------------------------------------------------------------
            Increase the count of idle frames received and delete the dsm
            item.  Save the frame info, as idle frames are always processed
            last.  This is to account for potential delayed frames.
          -----------------------------------------------------------------*/
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          stats_ptr->rx_idle_fr_cnt++;

          idle_fr_rxed = TRUE;
          idle_fr_info = curr_fr_info;
          age_nak_list = TRUE;
          frame_type = DSRLPI_DONE;
          break;

        case DSRLPI_RLP_ERASURE:
          frame_type = fcn_ptr_table->rx_rlp_erasure( rrcb_ptr, fwd_frame_ptr);
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          rrcb_ptr->stats.rx_rlp_erasures++;
          break;

        case DSRLPI_MUX_ERASURE:
          frame_type = fcn_ptr_table->rx_mux_erasure( rrcb_ptr, fwd_frame_ptr);
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          rrcb_ptr->stats.rx_mux_erasures++;

          if (rrcb_ptr->stats.largest_contig_erasure < ++current_erasure)
          {
            rrcb_ptr->stats.largest_contig_erasure = current_erasure;
          }
          break;

        case DSRLPI_NULL:
          frame_type = DSRLPI_DONE;
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          rrcb_ptr->stats.rx_null_fr_cnt++;
          age_nak_list = TRUE;
          break;

        case DSRLPI_NAK:
          rx_data_frs = TRUE;
          frame_type = DSRLPI_DONE;
          break;

        case DSRLPI_FILL:
          frame_type = DSRLPI_DONE;
          break;

        case DSRLPI_RESET:
          while((++frame_no) < fwd_frame_block->num_rlp_frames)
          {
            dsm_free_packet(&(fwd_frame_block->fwd_arr[frame_no].fr_item_ptr));
          }
          frame_type = DSRLPI_DONE;
          break;

        case DSRLPI_DCCH_DTX:
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          age_nak_list = TRUE;
          frame_type = DSRLPI_DONE;
          break;

        default:
          ASSERT( 0);
      }

    } /* while (frame_type != DSRLPI_DONE) */

  } /* for */

  /*-------------------------------------------------------------------------
    Clear out the delayed frames (if any) from the previous 20 msec
  -------------------------------------------------------------------------*/
  dsrlpdel_remove(rrcb_ptr);

  /*-------------------------------------------------------------------------
    If an idle was received, process it.
  -------------------------------------------------------------------------*/
  if (idle_fr_rxed == TRUE)
  {
    (void)fcn_ptr_table->rx_idle(rrcb_ptr, &idle_fr_info);
  }

  if (((rrcb_ptr->stats.rx_20ms_fr_cnt % 100) == 0) &&
      (rrcb_ptr->stats.rx_20ms_fr_cnt > 0)          &&
      (rrcb_ptr->state.mux_erasure_cnt > 0))
  {
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, "rx'd %d erases & blanks fr mux in 100 fr times RLP:%d",
             rrcb_ptr->state.mux_erasure_cnt,rscb_ptr->cfg.sr_id);
    rrcb_ptr->state.mux_erasure_cnt = 0;
  }

  /*-------------------------------------------------------------------------
    Age the nak list if new data/idle/mux erasures were received. V(N) could
    have changed when the NAK list is aged, because a NAK might have been
    aborted. In this case, the resequencing queue needs to be cleared.
  -------------------------------------------------------------------------*/
  if (age_nak_list == TRUE)
  {
    if ( dsrlpnak_age_list( rrcb_ptr) == TRUE)
    {
      DATA_RLP_MSG3(MSG_LEGACY_HIGH, " NAK ABORT new V(N)=%x, V(R)=%x RLP:%d",
               rrcb_ptr->state.needed_seq_num,
               rrcb_ptr->state.expected_seq_num,
               rscb_ptr->cfg.sr_id);
      rrcb_ptr->stats.aborts++;
      /*---------------------------------------------------------------------
        Report the RLP NAK abort event
      ---------------------------------------------------------------------*/
      rlp_event_payload.event_rlp_type = event_rlp_nak_abort;
      rlp_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
      event_report_payload (EVENT_RLP,
                            sizeof(event_rlp_payload_type),
                            &rlp_event_payload);

      rlp_nak_abort_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
      rlp_nak_abort_event_payload.first_seq = rrcb_ptr->state.needed_seq_num;
      rlp_nak_abort_event_payload.last_seq = rrcb_ptr->state.expected_seq_num;
      event_report_payload (EVENT_RLP_NAK_ABORT,
                          sizeof(event_rlp_nak_abort_payload_type),
                          &rlp_nak_abort_event_payload);
    }
  }

  if (rrcb_ptr->state.old_frames >= 5)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,"Reset due to too many old frames from BS");
    rrcb_ptr->state.reset_rlp = TRUE;          /*Set the reset flag*/
  }

  /*-------------------------------------------------------------------------
    Make sure that rlp_delay hasn't been over-incremented.  If so, set
    rlp_delay such that it is only incremented by 1.
  -------------------------------------------------------------------------*/
  if (state_ptr->rlp_delay > (curr_rlp_delay + 1))
  {
    state_ptr->rlp_delay = curr_rlp_delay + 1;
  }

  /*-------------------------------------------------------------------------
   Check if reset flag is set by tx, if it does then reset rlp else continue
   processing the rx.
  -------------------------------------------------------------------------*/
  if (rrcb_ptr->state.reset_rlp == TRUE)
  {
    dsrlpi_reset(rrcb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->cfg.sr_id;
    event_report_payload (EVENT_RLP,
                          sizeof(event_rlp_payload_type),
                          &rlp_event_payload);
    return;
  }

  /*-------------------------------------------------------------------------
    See if anything left to remove from resequencing queue.
  -------------------------------------------------------------------------*/
  dsrlprsq_remove(rrcb_ptr, DSRLPI_MAX_CHAINS_TO_REMOVE);

  /*-------------------------------------------------------------------------
    Tell DSMGR if any data frames were received, so that dormant mode
    processing can occur.
  -------------------------------------------------------------------------*/
  ds_rlp_data_rxed(rx_data_frs);

  /*-------------------------------------------------------------------------
    Update the stats with the count of contiguous IDLE frames received.
    Reset the counter if a valid data/ctl frame is received
  -------------------------------------------------------------------------*/
  if(rx_data_frs == TRUE)
  {
    stats_ptr->rx_contig_idle_fr_cnt = 0;
  }
  else
  {
    stats_ptr->rx_contig_idle_fr_cnt++;
  }

#if defined(FEATURE_IS2000_CHS)
  ds707_chs_rxed_data(rx_data_frs);
#endif

}  /* dsrlp_process_rx_frames() */
#endif /* FEATURE_DS_RLP3 */
#endif /* MSM5000_IRAM_FWD */


#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION DSRLP_BUILD_RLP_BLOB

DESCRIPTION

  This function builds an RLP BLOB (Block of Bytes)

  Input Parameters:

  *blob_ptr:   is a pointer pointing to free space for new blob be built.


DEPENDENCIES
  None

RETURN VALUE
  *blob_ptr: is a pointer pointing to the blob that was build.

   blob_len: is returned by the function and contains the length of the new
             blob.

SIDE EFFECTS
  None
===========================================================================*/
#define MAX_BLOB_LENGTH       16

byte dsrlp_build_rlp_blob
(
  byte  *tx_blob_ptr
)
{

  byte   blob_len;               /* holds the calculated BLOB length       */
  byte   new_byte_value;         /* holds the value of the next byte       */
  byte   max_nak_rounds_fwd;     /* max # of forward nak rounds            */
  byte   max_nak_rounds_rev;     /* max # of reverse nak rounds            */
  byte   desired_nak_rounds_fwd; /* desired # of forward nak rounds        */
  byte   desired_nak_rounds_rev; /* desired # of reverse nak rounds        */

  word   field_length  = 0;      /* holds the field length for next field  */
  word   field_position= 0;      /* holds a cumulative number of bytes used*/
  word   i;                      /* loop variable                          */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(tx_blob_ptr != NULL);
  memset(tx_blob_ptr, 0, MAX_BLOB_LENGTH);


  /*-------------------------------------------------------------------------
    Prepare RLP_BLOB_TYPE field with value 001.
  -------------------------------------------------------------------------*/
  field_length       = 3;
  new_byte_value     = 0x01;
  b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);

  /*-------------------------------------------------------------------------
    Prepare RLP_VERSION field with value 011.
  -------------------------------------------------------------------------*/
  field_position    += field_length;
  field_length       = 3;
  new_byte_value     = 0x03;
  b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);

  /*-------------------------------------------------------------------------
    Prepare RESET_VAR field with value 1 to always force an RLP reset
  -------------------------------------------------------------------------*/
  field_position    += field_length;
  field_length       = 1;
  new_byte_value     = 0x01;
  b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);


  /*-------------------------------------------------------------------------
    Prepare MAX_MS_NAK_ROUNDS_FWD field by consulting the constants data
    structure. We do not use the rscb_ptr here as it may not be initialized
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  max_nak_rounds_fwd     = dsrlp3_constants.max_nak_rounds_fwd;
  b_packb(max_nak_rounds_fwd, tx_blob_ptr, field_position, field_length);

  /*-------------------------------------------------------------------------
    Prepare MAX_MS_NAK_ROUNDS_REV field by consulting the constants data
    structure. We do not use the rscb_ptr here as it may not be initialized
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  max_nak_rounds_rev     = dsrlp3_constants.max_nak_rounds_rev;
  b_packb(max_nak_rounds_rev, tx_blob_ptr, field_position, field_length);


  /*-------------------------------------------------------------------------
    Prepare NAK_ROUNDS_FWD field. The MS will just enter the desirable values
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  desired_nak_rounds_fwd = dsrlp3_opt.default_nak_rounds_fwd;
  b_packb(desired_nak_rounds_fwd, tx_blob_ptr, field_position, field_length);


  /*-------------------------------------------------------------------------
    Prepare NAK_ROUNDS_REV field. The MS will just enter the desirable values
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  desired_nak_rounds_rev = dsrlp3_opt.default_nak_rounds_rev;
  b_packb(desired_nak_rounds_rev, tx_blob_ptr, field_position, field_length);


  /*-------------------------------------------------------------------------
    Prepare NAK_PER_ROUND_FWD field. The MS will enter the desirable values
  -------------------------------------------------------------------------*/
  for (i=0; i < desired_nak_rounds_fwd; i++ )
  {
    field_position    += field_length;
    field_length       = 3;
    new_byte_value     = dsrlp3_opt.default_naks_per_round_fwd[i];
    b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);
  }/* for */

  /*-------------------------------------------------------------------------
    Prepare NAK_PER_ROUND_REV field. The MS will enter the desirable values
  -------------------------------------------------------------------------*/
  for (i=0; i < desired_nak_rounds_rev; i++ )
  {
    field_position    += field_length;
    field_length       = 3;
    new_byte_value     = dsrlp3_opt.default_naks_per_round_rev[i];
    b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);
  }/* for */


  /*-------------------------------------------------------------------------
    Calculate the BLOB length in bytes and assign
  -------------------------------------------------------------------------*/
  field_position += field_length;

  if ( field_position % 8 == 0)
  {
      blob_len = (byte) ( field_position / ((word)8));

  }  else {

      blob_len = (byte)(field_position/((word)8)) + 1;
  }

  return(blob_len);

}/* dsrlp_build_rlp_blob() */
#endif /* FEATURE_DS_RLP3 */


#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION DSRLP_PROCESS_RLP_BLOB

DESCRIPTION
  This function processes the received RLP BLOB (Block of Bytes)

  Input Parameters:

  sr_id: used to find the appropriate session control block pointer

  *blob_ptr: points to the BLOB to be processed

  blob_len: specifies the length of the BLOB to be processed

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e715*/
boolean dsrlp_process_rlp_blob
(
  byte   sr_id,
  byte  *blob_ptr,
  byte   blob_len
)
{

  byte rscb_index;
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;/* session control block pointer     */
  dsrlpi_rlp_ctl_blk_type   *rrcb_ptr;/* session control block pointer     */

  byte    blob_type;                  /* RLP_BLOB_TYPE                     */
  byte    desired_nak_rounds_fwd;     /* desired forward nack rounds       */
  byte    desired_nak_rounds_rev;     /* desired reverse nack rounds       */
  byte    naks_per_round_fwd;         /* nacks per round on forward        */
  byte    naks_per_round_rev;         /* nacks per round on reverse        */
  byte    new_byte;                   /* holds the new received byte       */
  byte    rtt_byte=0;                 /* hold the rtt to be able to restore*/


  word    field_length       = 0;     /* holds the length of the field     */
  word    field_position     = 0;     /* cumulative count of bytes         */
  word    i                  = 0;     /* loop variable                     */
  byte    max_to_set_fwd     = 0;     /* helps use only relevant fields    */
  byte    max_to_set_rev     = 0;     /* helps use only relevant fields    */

  boolean no_sync_ack        = FALSE; /* do we need to do sync/ack ?       */
  boolean BLOB_during_call   = FALSE; /* did we rx the BLOB during a call? */

  event_rlp_payload_type     rlp_event_payload;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_index = dsrlp_get_rlp_rscb_index(sr_id,DSRLP_VER_1X_3);
  if (rscb_index == DSRLPI_INVALID_RSCB_INDEX)
  {
     /* Cannot process an entry that doesn't exist! */
     return FALSE;
  }

  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);
  

  /* 1x has only one route */
  ASSERT((rscb_ptr->cfg.ver == DSRLP_VER_1X_3) && 
         (rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE) &&
         (rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD));
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
    Determine if we are receiving this BLOB during a call. If this is the
    case we want to process all parameters and then force a dsrlpi_reset()
    and not force the state to _ESTABLISHED so that sync/ack happens.
  -------------------------------------------------------------------------*/
  if (rrcb_ptr->state.state == DSRLPI_ESTABLISHED_STATE)
  {
      BLOB_during_call = TRUE;
  }

  /*-------------------------------------------------------------------------
    Process RLP_BLOB_TYPE field.
  -------------------------------------------------------------------------*/
  field_length = 3;
  new_byte     = b_unpackb(blob_ptr, field_position, field_length);
  blob_type    = new_byte;
  if ( ( blob_type != 0x01 ) && ( blob_type != 0x02 ) )
  {
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, "BLOB::RLP_BLOB_TYPE invalid %d RLP:%d",
             blob_type, sr_id);
    return(FALSE);
  }
  if ( ( blob_type == 0x02 )  && ( mc_get_p_rev_in_use() < 9 ) )
  {
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "BLOB::RLP_BLOB_TYPE invalid %d p_rev %d RLP:%d",
             blob_type, mc_get_p_rev_in_use(), sr_id );
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    Process RLP_VERSION field with value 011.
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length    = 3;
  new_byte        = b_unpackb(blob_ptr, field_position, field_length);
  if (new_byte != 0x03)
  {
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, "BLOB::RLP_VERSION  invalid %d RLP:%d",
              new_byte, sr_id);
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    We know that this BLOB will at least specify a NAK scheme
  -------------------------------------------------------------------------*/
  rrcb_ptr->stats.RLP_BLOB_used = DSRLP_BLOB_NAK_SCHEME;

  /*-------------------------------------------------------------------------
    Process RTT field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length    = 4;
  new_byte        = b_unpackb(blob_ptr, field_position, field_length);
  if (new_byte == 0)
  {
    /*-----------------------------------------------------------------------
      SYNC exchange procedures required to establish a value for RLP_DELAY
    -----------------------------------------------------------------------*/
    dsrlpi_reset(rrcb_ptr);

    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->cfg.sr_id;
    event_report_payload (EVENT_RLP,
                          sizeof(rlp_event_payload),
                          &rlp_event_payload);
  }
  else
  {
    /*-----------------------------------------------------------------------
      NO SYNC exchange procedures required, use this value for RLP_DELAY
    -----------------------------------------------------------------------*/
    no_sync_ack = TRUE;
    rrcb_ptr->state.rlp_delay = rtt_byte = new_byte;
    rrcb_ptr->state.state     = DSRLPI_ESTABLISHED_STATE;
    DATA_RLP_MSG0(MSG_LEGACY_MED, "process blob, rlp move to est state");
    /*---------------------------------------------------------------------
       Report RLP established event
    ---------------------------------------------------------------------*/
   rlp_event_payload.event_rlp_type = event_rlp_established;
   rlp_event_payload.info = rscb_ptr->cfg.sr_id;
   event_report_payload (EVENT_RLP,
                         sizeof(event_rlp_payload_type),
                         &rlp_event_payload);


    /*-----------------------------------------------------------------------
      Set this back to zero now that we are out of all 3 init states
    -----------------------------------------------------------------------*/
    rrcb_ptr->state.sync_delay= 0;

    DATA_RLP_MSG2(MSG_LEGACY_MED, "BLOB::RTT value is %d RLP:%d", new_byte, sr_id);

    /*-----------------------------------------------------------------------
      This BLOB specifies RTT as well as a  NAK scheme
    -----------------------------------------------------------------------*/
    rrcb_ptr->stats.RLP_BLOB_used = DSRLP_BLOB_RTT_PLUS_NAK;
  }


  /*-------------------------------------------------------------------------
    Process RESET_VAR field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length    = 1;
  new_byte        = b_unpackb(blob_ptr, field_position, field_length);
  if (
       ( blob_type > 0x01 ) &&
       ( dsrlp_is_rlp_encrypt_on() == TRUE ) &&
       ( new_byte != 0x1 )
     )
  {
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "BLOB::BLOB_TYPE %d,init %d but encrypt on, RLP:%d",
             blob_type, new_byte, sr_id);
    return(FALSE);
  }

  if (new_byte == 0x1)
  {
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, "BLOB::RESET_VAR field is %d. Forcing RESET RLP:%d",
             new_byte, sr_id);
    dsrlpi_reset(rrcb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->cfg.sr_id;
    event_report_payload (EVENT_RLP,
                          sizeof(event_rlp_payload_type),
                          &rlp_event_payload);

    /*-----------------------------------------------------------------------
      Avoid sync exchange procedures if we got an RTT > 0
    -----------------------------------------------------------------------*/
    if (no_sync_ack)
    {
      rrcb_ptr->state.rlp_delay = rtt_byte;
      rrcb_ptr->state.state     = DSRLPI_ESTABLISHED_STATE;
      DATA_RLP_MSG0(MSG_LEGACY_MED, "process blob, rlp move to est state");
      /*---------------------------------------------------------------------
         Report RLP established event
      ---------------------------------------------------------------------*/
      rlp_event_payload.event_rlp_type = event_rlp_established;
      rlp_event_payload.info = rscb_ptr->cfg.sr_id;
      event_report_payload (EVENT_RLP,
                            sizeof(event_rlp_payload_type),
                            &rlp_event_payload);



      /*---------------------------------------------------------------------
        Set this back to zero now that we are out of all 3 init states
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.sync_delay= 0;

    }

  }

  /*-------------------------------------------------------------------------
    Process EXT_SEQ field
  -------------------------------------------------------------------------*/
  if (
      ( blob_type > 0x01 )                     &&
      ( dsrlp_is_rlp_encrypt_on()   == TRUE  ) &&
      ( dsrlp_is_l2_l3_encrypt_on() == FALSE )
     )
  {
    field_position      += field_length;
    field_length         = 32;
    new_byte             = b_unpackb(blob_ptr, field_position, field_length);
    rrcb_ptr->state.ext_seq = new_byte;
  }


  /*-------------------------------------------------------------------------
    Process NAK_ROUNDS_FWD field
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  desired_nak_rounds_fwd = b_unpackb(blob_ptr, field_position, field_length);

  if (desired_nak_rounds_fwd <= rscb_ptr->cfg.ver_ptr->max_nak_rounds_fwd )
  {
    /*-----------------------------------------------------------------------
      In this case set the current to the value we just received
    -----------------------------------------------------------------------*/
    rrcb_ptr->state.nak_rounds_fwd = desired_nak_rounds_fwd;

    DATA_RLP_MSG2(MSG_LEGACY_MED, "NAK_ROUNDS_FWD is SET to %d. RLP: %d",
            desired_nak_rounds_fwd, sr_id);
    max_to_set_fwd       = desired_nak_rounds_fwd;

  }
  else
  {
    /*-----------------------------------------------------------------------
      Best we can do is set the desired to be equal to the max
    -----------------------------------------------------------------------*/
    max_to_set_fwd = rrcb_ptr->state.nak_rounds_fwd =
                          rscb_ptr->cfg.ver_ptr->max_nak_rounds_fwd;

    DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Requested NAK_ROUNDS_FWD %d is INVALID. RLP:%d",
             desired_nak_rounds_fwd, sr_id);
    DATA_RLP_MSG2(MSG_LEGACY_MED, "NAK_ROUNDS_FWD is SET to %d. RLP:%d",
             max_to_set_fwd, sr_id);

  }
   /*------------------------------------------------------------------------
    Update the negotiated options field with lowest of BS and MS requested
    values.
    Note that the value that RLP will use, is minimum of (negotiated and
    active/current value). So write that value to the "working" variable.
    Now we have MIN(Negotiate,Active) as working variable.
  ------------------------------------------------------------------------*/
  dsrlp3_opt.negotiated_nak_rounds_fwd = rrcb_ptr->state.nak_rounds_fwd;
  max_to_set_fwd = MIN(dsrlp3_constants.desired_nak_rounds_fwd,
                       dsrlp3_opt.negotiated_nak_rounds_fwd);
  rrcb_ptr->state.nak_rounds_fwd = max_to_set_fwd;
  ASSERT( max_to_set_fwd <= DSRLP_MAX_NAK_ROUNDS);

  /*-------------------------------------------------------------------------
    Process NAK_ROUNDS_REV field
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  desired_nak_rounds_rev = b_unpackb(blob_ptr, field_position, field_length);

  if (desired_nak_rounds_rev <= rscb_ptr->cfg.ver_ptr->max_nak_rounds_rev )
  {
    /*-----------------------------------------------------------------------
      In this case set the desired to the value we just received
    -----------------------------------------------------------------------*/
    rrcb_ptr->state.nak_rounds_rev = desired_nak_rounds_rev;
    DATA_RLP_MSG2(MSG_LEGACY_MED, "NAK_ROUNDS_REV is SET to %d. RLP:%d",
            desired_nak_rounds_rev, sr_id);
    max_to_set_rev       = desired_nak_rounds_rev;

  }
  else
  {
    /*-----------------------------------------------------------------------
      Best we can do is set the desired to be equal to the max
    -----------------------------------------------------------------------*/
    max_to_set_rev = rrcb_ptr->state.nak_rounds_rev =
                          rscb_ptr->cfg.ver_ptr->max_nak_rounds_rev;
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Requested NAK_ROUNDS_REV %d is INVALID. RLP: %d",
              desired_nak_rounds_rev, sr_id);
    DATA_RLP_MSG1(MSG_LEGACY_MED, "NAK_ROUNDS_REV is SET to %d.\n", max_to_set_rev);
  }

  /*-------------------------------------------------------------------------
    Update the negotiated options in reverse direction with lowest of BS
    and MS. rrcb_ptr->state.nak_rounds_rev has the lowest value between BS
    and MS. So copy it to negotiated value.
    Note that the value that RLP will use as working variable , is minimum
    of (negotiated and current/active value). So write that value to the
    "working" variable. Now we have MIN(Negotiate,Active) as working
    variable.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.negotiated_nak_rounds_rev = rrcb_ptr->state.nak_rounds_rev;
  max_to_set_rev= MIN(dsrlp3_constants.desired_nak_rounds_rev,
                      dsrlp3_opt.negotiated_nak_rounds_rev);
  rrcb_ptr->state.nak_rounds_rev = max_to_set_rev;

  /*-------------------------------------------------------------------------
    Update the statistic for current tx naks rounds
  -------------------------------------------------------------------------*/
  ASSERT( max_to_set_rev <= DSRLP_MAX_NAK_ROUNDS);
  rrcb_ptr->stats.curr_tx_nak_rounds = max_to_set_rev;
  /*-------------------------------------------------------------------------
    Process NAK_PER_ROUND_FWD fields
  -------------------------------------------------------------------------*/
  for (i=0; i < desired_nak_rounds_fwd; i++)
  {

    field_position    += field_length;
    field_length       = 3;
    naks_per_round_fwd = b_unpackb(blob_ptr, field_position, field_length);

    if(i < max_to_set_fwd)
    {

      /*---------------------------------------------------------------------
        Obtain the Negotiated Value: Negotiated value = BS requested value
        We choose that Negotiated value is same as BS value. We use this
        value as our upper ceiling.
      ---------------------------------------------------------------------*/
      dsrlp3_opt.negotiated_naks_per_round_fwd[i] = naks_per_round_fwd;

      /*---------------------------------------------------------------------
        Obtain the value to be used as Working Value:
         Working Vale = Min(Negotiated Value,Active Value)
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.naks_per_round_fwd[i] =
          MIN(dsrlp3_opt.negotiated_naks_per_round_fwd[i],
              dsrlp3_constants.desired_naks_per_round_fwd[i]);

      DATA_RLP_MSG2(MSG_LEGACY_MED, "NAKS_PER_ROUND_FWD is SET to %d. RLP: %d",
              rrcb_ptr->state.naks_per_round_fwd[i], sr_id);

    }/* if */

  }/* for */

  /*-------------------------------------------------------------------------
    Process NAK_PER_ROUND_REV fields
  -------------------------------------------------------------------------*/
  for (i=0; i < desired_nak_rounds_rev; i++)
  {

    field_position    += field_length;
    field_length       = 3;
    naks_per_round_rev = b_unpackb(blob_ptr, field_position, field_length);

    if(i < max_to_set_rev)
    {
      /*---------------------------------------------------------------------
        Obtain the Negotiated Value: Negotiated value = BS requested Value
        We choose that Negotiated value is same as BS value. We use this
        value as our upper ceiling.
      ---------------------------------------------------------------------*/
      dsrlp3_opt.negotiated_naks_per_round_rev[i] = naks_per_round_rev;


      /*---------------------------------------------------------------------
        Obtain the value to be used as Working Value:
          Working Vale = MIN(Negotiated Value,Active Value);
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.naks_per_round_rev[i] =
          MIN(dsrlp3_opt.negotiated_naks_per_round_rev[i],
              dsrlp3_constants.desired_naks_per_round_rev[i]);

      DATA_RLP_MSG2(MSG_LEGACY_MED, "NAKS_PER_ROUND_REV is SET to %d. RLP:%d",
               rrcb_ptr->state.naks_per_round_rev[i], sr_id);

    }/* if */

  }/* for */

  /*-------------------------------------------------------------------------
    Process DDW
  -------------------------------------------------------------------------*/
  if ( blob_type > 0x01 )
  {
    field_position      += field_length;
    field_length         = 8;
    new_byte             = b_unpackb(blob_ptr, field_position, field_length);
    rrcb_ptr->state.fwd_ddw = new_byte*20;
  }

  /*-------------------------------------------------------------------------
    Process REXMIT_TIMER
  -------------------------------------------------------------------------*/
  if ( blob_type > 0x01 )
  {
    field_position      += field_length;
    field_length         = 8;
    new_byte             = b_unpackb(blob_ptr, field_position, field_length);
    rrcb_ptr->state.fwd_rexmit_time = new_byte*20;
  }

  DATA_RLP_MSG2(MSG_LEGACY_HIGH, "rxed ddw=%d, rxmit=%d",
              rrcb_ptr->state.fwd_ddw,
              rrcb_ptr->state.fwd_rexmit_time
              );

  /*-------------------------------------------------------------------------
    We are receiving this BLOB during a call. Force a dsrlpi_reset()
    and not force the state to _ESTABLISHED so that sync/ack happens.
  -------------------------------------------------------------------------*/
  if (BLOB_during_call)
  {
    dsrlpi_reset(rrcb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->cfg.sr_id;
    event_report_payload (EVENT_RLP,
                          sizeof(event_rlp_payload_type),
                          &rlp_event_payload);

  }

  /*-------------------------------------------------------------------------
    if the code is here then everything is fine with the BLOB
  -------------------------------------------------------------------------*/
  return(TRUE);

}/* dsrlp_process_rlp_blob() */
/*lint -restore*/
#endif /* FEATURE_DS_RLP3 */


/*===========================================================================

FUNCTION DSRLP_GET_STATS_EX

DESCRIPTION
  This function gets an sr_id and then returns a pointer to the appropriate
  structure containing the related statistics.

  Input Parameters:

  sr_id:         used to find the appropriate session control block pointer

  route:         The route for which stats are needed

  stats_ptr_ptr: is a pointer to a pointer to the stats structure

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  stats_ptr_ptr:  is the pointer to the pointer to the stats structure
  return TRUE if the *stats_ptr_ptr points to a valid memory location,
  otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_get_stats_ex(
  byte                      rscb_index,
  byte                      route,
  dsrlp_stats_info_type   **stats_ptr_ptr
)
{
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
  dsrlpi_rlp_ctl_blk_type   *rrcb_ptr;
  boolean status = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(rscb_index >= DSRLP_MAX_RSCB_ELEMENTS)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Using Invalid SR_ID %d to Retrieve RLP Stats", rscb_index);

    /*----------------------------------------------------------------------
    Hard Code it to 1 to avoid potential memory corruption if the client does
    not check the return value
    -----------------------------------------------------------------------*/
    rscb_index = 1;
    status = FALSE;
  }
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id_index.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);
  rrcb_ptr = &rscb_ptr->rlp_list[route];

  /*lint -save -e641 Conversion of enum to int */
  /*-------------------------------------------------------------------------
    Update the rlp "state" field in the stats stucture now.
  -------------------------------------------------------------------------*/
  rrcb_ptr->stats.state = rrcb_ptr->state.state;
  /*lint -restore*/

  /*-------------------------------------------------------------------------
    Pass a pointer to the stats structure to the ouside world
  -------------------------------------------------------------------------*/
  *stats_ptr_ptr = &rrcb_ptr->stats;

  return status;
}/* dsrlp_get_stats_ex() */

/*===========================================================================
FUNCTION DSRLP_GET_STATS

DESCRIPTION
  This function gets an sr_id and then returns a pointer to the appropriate
  structure containing the related statistics.

  Input Parameters:

  sr_id:         used to find the appropriate session control block pointer

  stats_ptr_ptr: is a pointer to a pointer to the stats structure

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  stats_ptr_ptr:  is the pointer to the pointer to the stats structure
  return TRUE if the *stats_ptr_ptr points to a valid memory location,
  otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_get_stats(
  byte                      rscb_index,
  dsrlp_stats_info_type   **stats_ptr_ptr
)
{
  return dsrlp_get_stats_ex(rscb_index, DSRSP_ROUTE_A, stats_ptr_ptr);
}/* dsrlp_get_stats() */


/*===========================================================================

FUNCTION DSRLP_CLEAR_STATS

DESCRIPTION
  This function takes an index in the rscb array (corresponding to an sr_id)
  control block pointer and from there it will initialize the related
  structure with the statistics.

  Input Parameters:

  rscb_index:         used to find the appropriate session control block pointer


DEPENDENCIES
  A valid rscb_index value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_clear_stats(byte rscb_index)
{
    dsrlpi_sess_ctl_blk_type  *rscb_ptr;
  dsrlpi_rlp_ctl_blk_type   *rrcb_ptr;
  int p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(rscb_index >= DSRLP_MAX_RSCB_ELEMENTS)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Using Invalid SR_ID %d to Clear RLP Stats", rscb_index);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  /*-------------------------------------------------------------------------
    Ensure RLP is initialized before trying to clear statistics
  -------------------------------------------------------------------------*/

  if(rscb_ptr->cfg.ver_ptr == NULL)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP for index:%d not initialized. Not clearing stats", rscb_index);
    return;
  }

  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    rrcb_ptr = &rscb_ptr->rlp_list[p];

    memset( &rrcb_ptr->stats, 0, sizeof(dsrlp_stats_info_type) );

  /*-------------------------------------------------------------------------
    Set these fields  to their defaults at this time.
  -------------------------------------------------------------------------*/
    rrcb_ptr->stats.curr_tx_nak_rounds = rscb_ptr->cfg.ver_ptr->max_nak_rounds_rev;
    rrcb_ptr->stats.RLP_BLOB_used      = DSRLP_BLOB_NOT_USED;

  /*-------------------------------------------------------------------------
    Note the time
  -------------------------------------------------------------------------*/
    (void)time_get(rrcb_ptr->stats.reset_timestamp );
  }
}/* dsrlp_clear_stats() */



#ifdef FEATURE_DS_PSTATS
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION DSRLP_GET_STATS_AT

DESCRIPTION
  This function copies the RLP stats into the given array.
  This particular function is for use with the AT commands.

DEPENDENCIES
  The given array must be of size DSRLP_STATS_COUNT or greater.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_get_stats_at
(
  uint32 *stats_ptr,  /* pointer to array to load with stats             */
  byte    rscb_index  /* index to help find right session ctl block      */
)
{
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  /*-------------------------------------------------------------------------
    Store the latest values to the passed struct pointer.
  -------------------------------------------------------------------------*/
  stats_ptr[DSRLP_STATS_RX_DATA_CNT] = (uint32)rrcb_ptr->stats.rx_new_data_fr_cnt;
  stats_ptr[DSRLP_STATS_TX_DATA_CNT] = (uint32)rrcb_ptr->stats.tx_new_data_fr_cnt;
  stats_ptr[DSRLP_STATS_NAKS       ] = (uint32)rrcb_ptr->stats.tx_naks[0];
  stats_ptr[DSRLP_STATS_DOUBLE_NAKS] = (uint32)rrcb_ptr->stats.tx_naks[1];
  stats_ptr[DSRLP_STATS_TRIPLE_NAKS] = (uint32)rrcb_ptr->stats.tx_naks[2];
  stats_ptr[DSRLP_STATS_RX_NAKS    ] = (uint32)rrcb_ptr->stats.naks_received;
/*
  stats_ptr[DSRLP_STATS_TX_REXMITS ] = (uint32)rrcb_ptr->stats.tx_rexmit_fr_cnt;
  stats_ptr[DSRLP_STATS_RX_REXMITS ] = (uint32)rrcb_ptr->stats.rx_rexmit_fr_cnt;
  stats_ptr[DSRLP_STATS_TX_IDLE_CNT] = (uint32)rrcb_ptr->stats.tx_idle_fr_cnt;
  stats_ptr[DSRLP_STATS_RX_IDLE_CNT] = (uint32)rrcb_ptr->stats.rx_idle_fr_cnt;
  stats_ptr[DSRLP_STATS_RX_BLANK_CNT]= (uint32)rrcb_ptr->stats.rx_blank_fr_cnt;
  stats_ptr[DSRLP_STATS_RX_NULL_CNT ]= (uint32)rrcb_ptr->stats.rx_null_fr_cnt;
  stats_ptr[DSRLP_STATS_TX_FUNDAMENTAL] = (uint32)rrcb_ptr->stats.tx_20ms_fr_cnt;
  stats_ptr[DSRLP_STATS_RX_FUNDAMENTAL] = (uint32)rrcb_ptr->stats.rx_20ms_fr_cnt;
*/
  stats_ptr[DSRLP_STATS_TX_TOTAL_BYTES] = (uint32)rrcb_ptr->stats.tx_total_bytes;
  stats_ptr[DSRLP_STATS_RX_TOTAL_BYTES] = (uint32)rrcb_ptr->stats.rx_total_bytes;


  stats_ptr[DSRLP_STATS_RE_XMITS_NOT_FOUND] =
                                     (uint32)rrcb_ptr->stats.re_xmits_not_found;
  stats_ptr[DSRLP_STATS_FILL_FRAMES_RXED  ] = (uint32)rrcb_ptr->stats.rx_fill_fr_cnt;

  stats_ptr[DSRLP_STATS_RLP_ERASURES] = (uint32)rrcb_ptr->stats.rx_rlp_erasures;
  stats_ptr[DSRLP_STATS_MUX_ERASURES] = (uint32)rrcb_ptr->stats.rx_mux_erasures;

  stats_ptr[DSRLP_STATS_LARGEST_CONTIG_ERASE] =
                                 (uint32)rrcb_ptr->stats.largest_contig_erasure;

  stats_ptr[DSRLP_STATS_LAST_RTT] = (uint32)rrcb_ptr->stats.last_rtt;

  stats_ptr[DSRLP_STATS_RESETS] = (uint32)rrcb_ptr->stats.resets;
  stats_ptr[DSRLP_STATS_ABORTS] = (uint32)rrcb_ptr->stats.aborts;
/*
  stats_ptr[DSRLP_STATS_BLOB_USED] = (uint32)rrcb_ptr->stats.RLP_BLOB_used;
  stats_ptr[DSRLP_STATS_CURR_TX_NAK_RNDS] = (uint32)rrcb_ptr->stats.curr_tx_nak_rounds;
*/
  stats_ptr[DSRLP_STATS_RLP_STATE] = (uint32)rrcb_ptr->stats.state;

  dsrlp_last_call_synced_ptr = (byte *)dsrlp_last_call_synced[ds_last_call_var];

} /* dsrlp_get_stats_at */

#endif /* FEATURE_DS_RLP3   */
#endif /* FEATURE_DS_PSTATS */

/*===========================================================================
FUNCTION      DSRLP_REG_SRVC

DESCRIPTION   Registers the watermarks and queues/callbacks for an RLP
              session corresponding to a given route.

DEPENDENCIES  A valid index (corresponding to an sr_id) must be passed
              into this function. All input ptrs must be non-NULL.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void dsrlp_reg_srvc
(
  byte                rscb_index,            /* index for a session id     */
  dsrlp_ver_enum_type ver,                   /* RLP version                */
  dsm_watermark_type  *tx_watermark_ptr,     /* ptr to Tx wm for txmit frs */
  void               (*post_rx_func_ptr)(uint32), /* rx callback for rx frames  */
  dsm_watermark_type  *post_rx_wm_ptr,      /* watermark for rx'ed frames    */
  byte                 route,              /* route for this RLP registration */
  uint32               post_rx_data       /* arg to post_rx_func_ptr()  */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);


  if(( ver == DSRLP_VER_1X_3 ) || ( ver == DSRLP_VER_HDR ))
  {
  ASSERT( (rscb_ptr != NULL) && 
          (tx_watermark_ptr != NULL) && 
          (post_rx_func_ptr != NULL) && 
          (post_rx_wm_ptr   != NULL));

  }

  /*-------------------------------------------------------------------------
    For the given RLP session, set the input (payload for txmit) and output
    (payload after receive) watermarks.
    Note, changing watermarks does not cause a RLP reset.
  -------------------------------------------------------------------------*/
  rscb_ptr->cfg.tx_wm_ptr   = tx_watermark_ptr;
  rscb_ptr->cfg.rx_func_ptr = post_rx_func_ptr;
  rscb_ptr->cfg.rx_wm_ptr[route]   = post_rx_wm_ptr;
  rscb_ptr->cfg.rx_data = post_rx_data;
} /* dsrlp_reg_srvc */

/*===========================================================================
FUNCTION      DSRLP_REG_PZID_HYST_NOTIFY_FUNC

DESCRIPTION   This registers the function to notify PZID hysterisis engine
              if a new RLP packet is sent/received after going dormant.

              If a NULL function is passed, the "notify PZID" function is
              deregistered.

DEPENDENCIES  A valid index (corresponding to an sr_id) must be passed
              into this function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_reg_ppp_data_transfer_notify_func
(
  byte                rscb_index,            /* index for a session id     */
  void (*ppp_data_transfer_notify_func) (byte rlp_instance)
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;        /* rlp-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity check - Since this should only be called for 1X calls, this must
    always be called with the fixed SR_ID index. If this assumption changes
    in the future, remove this ASSERT.
  -------------------------------------------------------------------------*/
  ASSERT(rscb_index == DSRLP_FIXED_SR_ID_INDEX);

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);
  ASSERT((rscb_index == DSRLP_FIXED_SR_ID_INDEX)&& 
         (rscb_ptr         != NULL) && 
         (rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD));

  /* 1x has only one route */
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  rscb_ptr->cfg.notify_ppp_data_transfer_func_ptr =
                                               ppp_data_transfer_notify_func;
  /*-----------------------------------------------------------------------
   Since this function will only be registered when the call goes dormant,
   set the PPP data transfer flag to FALSE to indicate that no data
   transfer has happened since dormancy. It will be set to TRUE when data
   is received or transmitted by RLP3.

   If deregistering this function, set the flag to FALSE for cleanup.
   This flag is required because the PPP data could be transmitted/
   received in two different tasks (RX and TX)
  -----------------------------------------------------------------------*/
  rrcb_ptr->state.ppp_data_transfer_after_dormancy = FALSE;
}




/*===========================================================================
FUNCTION      DSRLP_BYTES_DATA_TO_SEND

DESCRIPTION   Gives some indication of the # new bytes of data to send by
              RLP 3.

DEPENDENCIES  The passed in SR_ID is good.

RETURN VALUE  TRUE - new data or rexmits to be sent.  FALSE - no data to be
              rexmitted.

SIDE EFFECTS  None
===========================================================================*/
word dsrlp_bytes_data_to_send
(
  byte  rscb_index                       /* passed in SR_ID  index         */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;        /* rlp-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rscb_index < DSRLP_MAX_RSCB_ELEMENTS)
  {
    rscb_ptr = &dsrlpi_rscb_array[rscb_index];
    ASSERT(rscb_ptr != NULL);
  }
  else
  {
    /*-----------------------------------------------------------------------
       invalid SR_ID...return FALSE
    -----------------------------------------------------------------------*/
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "BAD SR ID");
    return(0);
  }

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  if ( ((rscb_ptr->cfg.tx_wm_ptr != NULL) &&
        (rscb_ptr->cfg.tx_wm_ptr->current_cnt > 0)
       )                                                ||
       (DSRLPRTX_HAS_REXMITS(rrcb_ptr))
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}

/*===========================================================================
FUNCTION      DSRLP_HAS_DATA_TO_SEND

DESCRIPTION   Does RLP3 have stuff to send? (either naks, rexmits, new data,
              etc).

DEPENDENCIES  The passed in SR_ID is good.

RETURN VALUE  TRUE - RLP needs to send some data.

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_has_data_to_send
(
   byte  rscb_index                /* passed in index for the SR_ID        */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;        /* rlp-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rscb_index >= DSRLP_MAX_RSCB_ELEMENTS)
  {
    /*-----------------------------------------------------------------------
       invalid SR_ID...return FALSE
    -----------------------------------------------------------------------*/
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "BAD SR ID");
    return(FALSE);
  }
  else
  {
    rscb_ptr = &( dsrlpi_rscb_array[ rscb_index ]);
    ASSERT(rscb_ptr != NULL);
  }

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
    Conditions to check for - there is new data to send
                            - there are rexmits to send
                            - it is time to send a NAK
                            - We are calculating RTT during SYNC-ACK
  -------------------------------------------------------------------------*/
  if ( ((rscb_ptr->cfg.tx_wm_ptr != NULL) &&
        (rscb_ptr->cfg.tx_wm_ptr->current_cnt > 0)
       )                                               ||
       (DSRLPRTX_HAS_REXMITS(rrcb_ptr))                ||
       (rrcb_ptr->state.naks_to_send == TRUE)          ||
       (rrcb_ptr->state.state == DSRLPI_RT_MEAS_STATE) ||
       (rrcb_ptr->state.state == DSRLPI_ACK_STATE)     ||
         (rrcb_ptr->state.ack_to_send == TRUE )
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* dsrlp_has_data_to_send() */

/*===========================================================================

FUNCTION DSRLP_LOG_TIMETEST_THROUGHPUT_DATA

DESCRIPTION
  This function  logs RLP throughput statistics; called from
  dsrlp_log_timetest_throughput_hdr for SN RLP, every 500 ms.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_log_timetest_throughput_data
(
  uint16 rlp_type
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;  /* rlp-specific control block  */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr=NULL;  /* rlp-specific control block  */
#ifdef FEATURE_HDR
  uint8     rlp_id;
  uint8     rscb_index;
#endif /* FEATURE_HDR */
  uint32    rx_delta, tx_delta;             /* bytes rx/tx in interval     */
  dsrlp_throuput_data_type rlp_data_local;
#ifdef TIMETEST_DSRLP_STATS
  uint16    tx_div,rx_div;                  /* rx/tx scale factors         */
  uint16     rx_throuput, tx_throuput;
  uint16    token_type;
#endif /* TIMETEST_DSRLP_STATS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Get the total Rx and Tx bytes and calculate the delta.  This delta will
    be written out to the TIMETEST port.

    Note, for RX, for HDR this value will be counted in 1000's of bytes
    (K's of bytes) whereas for 1X it will be in 100's of bytes.
    This is to accomodate the higher forward link rates of HDR, while using
    only a fixed 1 byte value to write out to TIMETEST.

    For both HDR and 1X, the Tx value will always be in 100's of bytes.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Calculate Rx divisor based on what current type of call.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_HDR
  if(rlp_type == TIMETEST_DSRLP_IS856)
  {
    rlp_id = DSRLP_HDR_SN_RLP_SR_ID_VALUE;
    rscb_index = dsrlp_get_rlp_rscb_index( rlp_id, DSRLP_VER_HDR );
    /*-------------------------------------------------------------------------
      If the RLP is not found, then there is no logging done.
    -------------------------------------------------------------------------*/
    if( rscb_index == DSRLPI_INVALID_RSCB_INDEX )
    {
      return;
    }
    rscb_ptr = &dsrlpi_rscb_array[rscb_index];
    memscpy(&rlp_data_local,sizeof(dsrlp_throuput_data_type),
    	    &rlp_data_hdr,sizeof(dsrlp_throuput_data_type));
  } else
#endif /* FEATURE_HDR */

  if(rlp_type == TIMETEST_DSRLP_IS2K)
  {
    rscb_ptr = &dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX];
    memscpy(&rlp_data_local,sizeof(dsrlp_throuput_data_type),
    	    &rlp_data_1x,sizeof(dsrlp_throuput_data_type));
  }
  else
  {
     DATA_RLP_MSG0(MSG_LEGACY_ERROR, "invalid rlp_type in timetest rlp logging");
     return;
  }

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
    Calculate interval Rx bytes and log to TIMETEST port.
  -------------------------------------------------------------------------*/
  rx_delta = (uint32)(rrcb_ptr->stats.rx_total_bytes - rlp_data_local.last_rx_bytes);
  rlp_data_local.last_rx_bytes = rrcb_ptr->stats.rx_total_bytes;

  /*-------------------------------------------------------------------------
    Calculate interval Tx bytes and log to TIMETEST port.
  -------------------------------------------------------------------------*/
#ifdef TIMETEST_DSRLP_STATS
  tx_div = 100;
  rx_div = 100;
#endif /* TIMETEST_DSRLP_STATS */
  tx_delta = (uint32)(rrcb_ptr->stats.tx_total_bytes - rlp_data_local.last_tx_bytes);
  rlp_data_local.last_tx_bytes = rrcb_ptr->stats.tx_total_bytes;

#ifdef TIMETEST_DSRLP_STATS
  rx_throuput = (uint16)(rx_delta / rx_div);
  tx_throuput = (uint16)(tx_delta / tx_div);
#endif /* TIMETEST_DSRLP_STATS */

  /*-------------------------------------------------------------------------
    Update dsrlp stats struct with the new rx & tx thrput info.
    store throuput in bits per sec hence multiplication of delta bytes
    by 2 & 8 since we update we get statistics every 1/2 sec.
  -------------------------------------------------------------------------*/
  rrcb_ptr->stats.rx_throuput = rx_delta *2 *8 ;
  rrcb_ptr->stats.tx_throuput = tx_delta *2 *8 ;

#ifdef TIMETEST_DSRLP_STATS

  /*-------------------------------------------------------------------------
    Check if it fits in 1 byte, else increase the scale at the expense of
    accuracy
  -------------------------------------------------------------------------*/

  if (rx_throuput < 255 )
  {
    rx_div = LOW_THPUT_SCALE;
    rx_throuput = rx_delta / rx_div;
    token_type = TIMETEST_DSRLP_LOW_THPUT;
  }
  else
  {
    rx_div = MED_THPUT_SCALE;
    rx_throuput = rx_delta / rx_div;
    token_type = TIMETEST_DSRLP_MED_THPUT;
  }

  /*-------------------------------------------------------------------------
    if no data transferred in this interval donot write to timetest port
  -------------------------------------------------------------------------*/

  if( ! (( rx_throuput == 0) && ( tx_throuput == 0)))
  {

    TIMETEST_DSRLP_BYTES( token_type,
                          (uint8)(rx_throuput),
                          (uint8)(tx_throuput) );
    DATA_RLP_MSG3(MSG_LEGACY_MED, "Log RLP TIMETEST rx: %d tx: %d token: %x",
             (uint8)(rx_delta/rx_div),(uint8)(tx_delta/tx_div),token_type);
  }
#endif /* TIMETEST_DSRLP_STATS */

#ifdef FEATURE_HDR
  if(rlp_type == TIMETEST_DSRLP_IS856)
  {
    memscpy( &rlp_data_hdr, sizeof(dsrlp_throuput_data_type),
    	     &rlp_data_local, sizeof(dsrlp_throuput_data_type) );
  } else
#endif /* FEATURE_HDR */
  if(rlp_type == TIMETEST_DSRLP_IS2K)
  {
    memscpy( &rlp_data_1x, sizeof(dsrlp_throuput_data_type),
    	     &rlp_data_local, sizeof(dsrlp_throuput_data_type) );
  }
} /* dsrlp_log_timetest_throughput_data() */

/*===========================================================================

FUNCTION DSRLP_LOG_TIMETEST_THROUGHPUT

DESCRIPTION
  This function is a clock callback function to control logging of the
  total Rx and Tx RLP bytes to the TIMETEST port.  It is controlled by the
  dsrlp_timetest_cb timer_type and is registered in dsrlp_init() to go off
  every 500 ms.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void dsrlp_log_timetest_throughput
(
  int4 ms
)
{
  /* Lint Suppress Error */
  DATA_MDM_SYMBOL_NOT_USED(ms);

#ifdef FEATURE_HDR
  dsrlp_log_timetest_throughput_data(TIMETEST_DSRLP_IS856);
#endif
  dsrlp_log_timetest_throughput_data(TIMETEST_DSRLP_IS2K);
} /* dsrlp_log_timetest_throughput() */


/*===========================================================================
FUNCTION      DSRLP_QUEUE_SDB_RX_DATA

DESCRIPTION   Queues SDB RX data into RLP RX queue and calls registered
              RX callback

DEPENDENCIES  The passed in RSCB_INDEX is good.

RETURN VALUE  TRUE if operation is sucessful, FALSE if not.  If function
              returns FALSE, input data packet will need to be freed by
              caller

SIDE EFFECTS  Adds data to RX watermark
===========================================================================*/
boolean
dsrlp_queue_sdb_rx_data
(
  byte          rscb_index,
  dsm_item_type *data_packet
)
{
  dsm_watermark_type *rx_wm_ptr;   /* Pointer to receive WM list*/

  /*-------------------------------------------------------------------------
    Check for correctness in SDB Rx path to DS:
      a) Confirm the Receive queue is registered
      b) Confirm that the recieve queue does not have any more data.

    Obtain the rx queue into which outgoing data has to be copied
    Confirm the Receive queue is registered correctly. If its not, then
    thrash the message.
  -------------------------------------------------------------------------*/
  rx_wm_ptr = dsrlpi_rscb_array[rscb_index].cfg.rx_wm_ptr[DSRSP_ROUTE_A];

  if(rx_wm_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "No Rx WM for Rx SDB");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Confirm the Receive queue is empty. If its not empty, nothing much we
    can do now. Queue it any way after printing the message. Note that SDB
    of 1x always uses route DSRSP_ROUTE_A.
  -------------------------------------------------------------------------*/
  if(!dsm_is_wm_empty (rx_wm_ptr))
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "SDB Write to non-empty WM");
  }


  if(dsrlpi_rscb_array[rscb_index].cfg.rx_func_ptr != NULL)
  {
    /*-------------------------------------------------------------------------
      Copy the data to the WM.
    -------------------------------------------------------------------------*/
    dsm_enqueue (rx_wm_ptr, &data_packet);

    /*-------------------------------------------------------------------------
      Now call the RX function. This will set the flag informing incoming data
    -------------------------------------------------------------------------*/
    dsrlpi_rscb_array[rscb_index].cfg.rx_func_ptr
          (dsrlpi_rscb_array[rscb_index].cfg.rx_data);
  }
  else
  {
    /*-----------------------------------------------------------------------
      We should not come here. But just in case... let's inform developer.
    -----------------------------------------------------------------------*/
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "No function for SDB Rx");
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================
FUNCTION      DSRLP3_OPT_INIT

DESCRIPTION   Initializes all the RLP option fields

DEPENDENCIES  Should be initialized before making the first data call. So
              power on initialization is the best place.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void dsrlp3_opt_init
(
  void
)
{
  uint8 i;                                                  /* loop variable */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the default and negotiated nak rounds forward to the MAX
    nak rounds permitted by MS RLP implementation.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.default_nak_rounds_fwd    = DSRLP_MAX_NAK_ROUNDS;
  dsrlp3_opt.negotiated_nak_rounds_fwd = DSRLP_MAX_NAK_ROUNDS;

  /*-------------------------------------------------------------------------
    We are using the "for" loop, to copy contents of the dsrlp3_option
    variables because currently the default values are 1,2 & 3. To play safe,
    we could have copied the default values explicitly here but
    its assumed that the values we are always interested initially in default
    are 1,2,3 and so we optimized it.

    For negotiated value, initialize to maximum possible value. After
    negotiation is done then this value will be modified. This is needed
    because if negotiation is not done then:
     working value = MIN(negotiated,desired). So desired should be less than
    DSRLP_MAX_NAKS_PER_ROUND for any NAK round.
  -------------------------------------------------------------------------*/
  for (i=0; i<DSRLP_MAX_NAK_ROUNDS;i++)
  {
    dsrlp3_opt.default_naks_per_round_fwd[i]    = i+1;
    dsrlp3_opt.negotiated_naks_per_round_fwd[i] = DSRLP_MAX_NAKS_PER_ROUND;
  }

  /*-------------------------------------------------------------------------
    Initialize the default and negotiated nak rounds reverse to the MAX nak
    rounds permitted by MS RLP implementation.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.default_nak_rounds_rev    = DSRLP_MAX_NAK_ROUNDS;
  dsrlp3_opt.negotiated_nak_rounds_rev = DSRLP_MAX_NAK_ROUNDS;

  /*-------------------------------------------------------------------------
    We are using the "for" loop, to copy contents of the dsrlp3_option
    variables because currently the default values are 1,2 & 3. To play safe,
    we could have copied the values explicitly (without a "for" loop) here
    but its assumed that the values we are always interested initially are
    1,2,3 and so we optimized it.

    For negotiated value, initialize to maximum possible value. After
    negotiation is done then this value will be modified. This is needed
    because if negotiation is not done then:
     working value = MIN(negotiated,desired). So desired should be less than
    DSRLP_MAX_NAKS_PER_ROUND for any NAK round.
  -------------------------------------------------------------------------*/
  for (i=0; i<DSRLP_MAX_NAK_ROUNDS;i++)
  {
    dsrlp3_opt.default_naks_per_round_rev[i]    = i+1;
    dsrlp3_opt.negotiated_naks_per_round_rev[i] = DSRLP_MAX_NAKS_PER_ROUND;
  }

  dsrlp3_opt.update_state_var = FALSE;
} /* dsrlp3_opt_init */

/*===========================================================================

FUNCTION     DSRLP_GET_ALL_DEF_NAKS
DESCRIPTION  Returns the number of default rounds and the naks per round to be
             used when negotiating an RLP3 service option
DEPENDENCIES There must be sufficient space in the naks_per_round arrays
             to hold the returned data
RETURN VALUE Boolean:
             True: the operation was success.
             False: The operation was not done. None of the values in the
                    passed pointers may be good.
             Values are returned by argument pointers
SIDE EFFECTS None
===========================================================================*/
boolean dsrlp_get_all_def_naks
(
   byte *nak_rounds_fwdp,                /* Num of rounds for forward NAKs */
   byte *naks_per_round_fwdp,            /* Num of NAKs per round forward  */
   byte *nak_rounds_revp,                /* Num of rounds for reverse NAKs */
   byte *naks_per_round_revp             /* Num of NAKs per round reverse  */
)
{
 int i;                                                  /* loop Counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------
   Confirm that all the input paramters are in the range. If they are not
   In range then return FALSE.
 -------------------------------------------------------------------------*/
 if ( (nak_rounds_fwdp == NULL)     ||
      (naks_per_round_fwdp == NULL) ||
      (nak_rounds_revp == NULL)     ||
      (naks_per_round_revp == NULL)
    )
 {
    return FALSE;
 }

 /*-------------------------------------------------------------------------
   Copy the values to the passed arguments for fwd parameters.
 -------------------------------------------------------------------------*/
 *nak_rounds_fwdp = dsrlp3_opt.default_nak_rounds_fwd;
 for (i=0; i<dsrlp3_opt.default_nak_rounds_fwd; i++)
 {
    naks_per_round_fwdp[i] = dsrlp3_opt.default_naks_per_round_fwd[i];
 }

 /*-------------------------------------------------------------------------
   Copy the values to the passed arguments for Rev parameters.
 -------------------------------------------------------------------------*/
 *nak_rounds_revp = dsrlp3_opt.default_nak_rounds_rev;
 for (i=0; i<dsrlp3_opt.default_nak_rounds_rev; i++)
 {
    naks_per_round_revp[i] = dsrlp3_opt.default_naks_per_round_rev[i];
 }
 return TRUE;
} /* dsrlp_get_all_def_naks */

/*===========================================================================
  FUNCTION      DSRLP_SET_ALL_DEF_NAKS

  DESCRIPTION   Set the number of rounds and the naks per round to be
                used when negotiating an RLP3 service option

  DEPENDENCIES  Can't lengthen the number of rounds beyond max_rounds.

  RETURN VALUE  TRUE:  If we were able to set the values.
                FALSE: None of the values are changed, since some parameter
                       is having a wrong value.
                Values are returned through arguments.

  SIDE EFFECTS  Changes don't take effect until next SO negotiation
===========================================================================*/
boolean dsrlp_set_all_def_naks
(
  byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
  byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
  byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
  byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Suppress Lint Error */
  DATA_MDM_ARG_NOT_CONST(nak_rounds_fwdp);
  DATA_MDM_ARG_NOT_CONST(naks_per_round_fwdp);
  DATA_MDM_ARG_NOT_CONST(nak_rounds_revp);
  DATA_MDM_ARG_NOT_CONST(naks_per_round_revp);

  /*-------------------------------------------------------------------------
    Confirm that all the input paramters are in the range. If they are not
    in range then return FALSE.
    Confirm that none of the pointers are NULL.
    Confirm that the contents of the pointers are within valid range. The
    valid values are given below:

     nak_rounds_fwd:     Should not be NUL
     nak_rounds_fwd:     Should not be greater than MAX_NAK_ROUNDS
     naks_per_round_fwd: Should not be NULL
     nak_rounds_rev:     Should not be NULL
     nak_rounds_rev:     Should not be Greater than MAX_NAK_ROUNDS
     naks_per_round_rev: Should not be NULL
  -------------------------------------------------------------------------*/
  if(
     ( nak_rounds_fwdp == NULL)                 ||
     ( *nak_rounds_fwdp > DSRLP_MAX_NAK_ROUNDS) ||
     ( naks_per_round_fwdp == NULL)             ||
     ( nak_rounds_revp == NULL)                 ||
     ( *nak_rounds_revp > DSRLP_MAX_NAK_ROUNDS) ||
     ( naks_per_round_revp == NULL)
    )
    {
       return FALSE;
    }

  /*-------------------------------------------------------------------------
    Confirm that the values of NAKS PER ROUND are within the range.
  -------------------------------------------------------------------------*/
  for (i=0; i<(*nak_rounds_fwdp); i++)
  {
     if (naks_per_round_fwdp[i] > DSRLP_MAX_NAKS_PER_ROUND)
        return FALSE;
  }

  for (i=0; i<(*nak_rounds_revp); i++)
  {
     if (naks_per_round_revp[i] > DSRLP_MAX_NAKS_PER_ROUND)
        return FALSE;
  }


  /*-------------------------------------------------------------------------
    Update the structure dsrlp3_opt and the values given here will be
    requested by Mobile in the next Service Negotiation. But if the peer
    RLP requests for values lower than these, then the negotiated values
    are different than the modified values.

    Update the Nak Rounds and Naks per round values that should be
    used in the forward channel.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.default_nak_rounds_fwd = *nak_rounds_fwdp;
  for (i=0; i<(*nak_rounds_fwdp); i++)
  {
    dsrlp3_opt.default_naks_per_round_fwd[i] = naks_per_round_fwdp[i];
  }

  /*-------------------------------------------------------------------------
    Update the Nak Rounds and Naks per round values that should be
    used in the Reverse channel.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.default_nak_rounds_rev = (*nak_rounds_revp);
  for (i=0; i< (*nak_rounds_revp); i++)
  {
     dsrlp3_opt.default_naks_per_round_rev[i] = naks_per_round_revp[i];
  }
  return TRUE;

} /* dsrlp_set_all_def_naks */

/*===========================================================================
  FUNCTION      DSRLP_GET_CUR_NAKS

  DESCRIPTION   Return the number of rounds and the naks per round currently
                in use (or last used) in an RLP3 service option call

  DEPENDENCIES  There must be sufficient space in the naks_per_round arrays
                to hold the returned data
                For current implementation rscb_index can have only
                DSRLP_FIXED_SR_ID_INDEX

  RETURN VALUE  Values are returned by argument pointers
                TRUE:  If we were able to get the values.
                FALSE: Some argument is wrong. So could not return values.


  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_get_all_cur_naks
(
  byte rscb_index,                       /* RLP instance                   */
  byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
  byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
  byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
  byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;       /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* rlp-specific control block */
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Confirm that all the input paramters are in the range. If they are not
    In range then return FALSE.
  -------------------------------------------------------------------------*/
  if (
      (rscb_index >= DSRLP_MAX_RSCB_ELEMENTS)  ||
      (nak_rounds_fwdp == NULL)                ||
      (naks_per_round_fwdp == NULL)            ||
      (nak_rounds_revp == NULL)                ||
      (naks_per_round_revp == NULL)
     )
  {
     return FALSE;
  }

  /*-------------------------------------------------------------------------
    Obtain the pointer of the RLP parameters array
    Note: For current implementation rscb_index can have only
    DSRLP_FIXED_SR_ID_INDEX
  -------------------------------------------------------------------------*/
  rscb_ptr = &dsrlpi_rscb_array[rscb_index];
  ASSERT(rscb_ptr != NULL);

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
    Obtain the NAK rounds and Naks per round in forward direction.

    It is assumed that the caller of this function will read only elements
    upto naks_per_round_fwd[nak_rounds_fwd-1] in the array so, we fill
    only those values in the array.
  -------------------------------------------------------------------------*/
  *nak_rounds_fwdp = rrcb_ptr->state.nak_rounds_fwd;
  for (i=0; i<rrcb_ptr->state.nak_rounds_fwd; i++)
  {
     naks_per_round_fwdp[i] = rrcb_ptr->state.naks_per_round_fwd[i];
  }

  /*-------------------------------------------------------------------------
    Obtain the NAK rounds and Naks per round in Reverse direction.

    It is assumed that the caller of this function will read only elements
    upto naks_per_round_fwd[nak_rounds_fwd-1] in the array ans so, we fill
    only those values in the array.
  -------------------------------------------------------------------------*/
  *nak_rounds_revp = rrcb_ptr->state.nak_rounds_rev;
  for (i=0; i<rrcb_ptr->state.nak_rounds_rev; i++)
  {
    naks_per_round_revp[i] = rrcb_ptr->state.naks_per_round_rev[i];
  }

  /*-------------------------------------------------------------------------
    Since we give the information that caller requested, return TRUE
  -------------------------------------------------------------------------*/
  return TRUE;

} /* dsrlp_get_all_naks */

/*===========================================================================
  FUNCTION      DSRLP_SET_ALL_CUR_NAKS

  DESCRIPTION   Set the number of rounds and the naks per round to be
                used in the current call, overiding the defaults or
                negotiated values. These values are used over the next call
                also if it is less than negotiated value.

  DEPENDENCIES  Each NAK round must be less than or equal to what was
                specified as the default when the call was originated.
                The passed in RSCB_INDEX is good. Also dsrlp_init must have
                been invoked first for the returned SR_ID to be good.

  RETURN VALUE  TRUE:  If we were able to set the values.
                FALSE: None of the values are changed, since some parameter
                       is having a wrong value.
                Values are returned through arguments.

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_set_all_cur_naks
(
  byte rscb_index,                       /* RLP instance                   */
  byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
  byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
  byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
  byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Lint Suppress Error */
  DATA_MDM_ARG_NOT_CONST(nak_rounds_fwdp);
  DATA_MDM_ARG_NOT_CONST(naks_per_round_fwdp);
  DATA_MDM_ARG_NOT_CONST(nak_rounds_revp);
  DATA_MDM_ARG_NOT_CONST(naks_per_round_revp);

  /*-------------------------------------------------------------------------
    Confirm that all the input paramters are in the range. If they are not
    in range then return FALSE.
    Check for nak_rounds_fwd to be > dsrlp3_constants max_nak_rounds instead
    of the DSRLP_MAX_NAK_ROUNDS because, in a call,
    We should not set a value higher than negotiated value. Same for reverse
    rounds also.

    rscb_index:         Should be less than Max_RSCB_Elements
    nak_rounds_fwd:     Should not be NULL
    *nak_rounds_fwd:    Should be less than Maximum allowed and negotiated
                            values. Though we can assume that negotiated value
                            will be less than MAX, we put extra check to test
                            for MAX also.
    naks_per_round_fwd: Should not be NULL
    nak_rounds_rev:     Should not be NULL
    *nak_rounds_rev:    Should be less than Max value and Negotiated value
    naks_per_round_rev: Should not be NULL
  -------------------------------------------------------------------------*/
  if(
     (rscb_index >= DSRLP_MAX_RSCB_ELEMENTS)                    ||
     (nak_rounds_fwdp == NULL)                                   ||
     (*nak_rounds_fwdp > dsrlp3_constants.max_nak_rounds_fwd)    ||
     (*nak_rounds_fwdp > dsrlp3_opt.negotiated_nak_rounds_fwd)   ||
     (*nak_rounds_fwdp > IOCTL_707_MAX_NAK_ROUNDS)               || 
     (naks_per_round_fwdp == NULL)                               ||
     (nak_rounds_revp == NULL)                                   ||
     (*nak_rounds_revp > dsrlp3_constants.max_nak_rounds_rev)    ||
     (*nak_rounds_revp > dsrlp3_opt.negotiated_nak_rounds_rev)   ||
     (*nak_rounds_revp > IOCTL_707_MAX_NAK_ROUNDS)               ||
     (naks_per_round_revp == NULL)
    )
    {
      return FALSE;
    }
  /*-------------------------------------------------------------------------
    Confirm that the values are within the range.
  -------------------------------------------------------------------------*/
  for (i=0; i<(*nak_rounds_fwdp); i++)
  {
     if( (naks_per_round_fwdp[i] > DSRLP_MAX_NAKS_PER_ROUND) ||
         (naks_per_round_fwdp[i] > dsrlp3_opt.negotiated_naks_per_round_fwd[i])
       )
     {
         return FALSE;
     }
  }

  for (i=0; i<(*nak_rounds_revp); i++)
  {
     if ( (naks_per_round_revp[i] > DSRLP_MAX_NAKS_PER_ROUND) ||
          (naks_per_round_revp[i] > dsrlp3_opt.negotiated_naks_per_round_rev[i] )
        )
     {
         return FALSE;
     }
  }

  /*-------------------------------------------------------------------------
    Obtain the rscb_index
  -------------------------------------------------------------------------*/
  dsrlp3_opt.rscb_index = rscb_index;

  /*-------------------------------------------------------------------------
    Update the Forward NAK rounds for current call and also for future calls
  -------------------------------------------------------------------------*/
  dsrlp3_constants.desired_nak_rounds_fwd = *nak_rounds_fwdp;

  /*-------------------------------------------------------------------------
    Update the Forward NAKs per round for current call and also for future
    calls.
  -------------------------------------------------------------------------*/
  for (i=0; i< (*nak_rounds_fwdp); i++)
  {
    dsrlp3_constants.desired_naks_per_round_fwd[i] = naks_per_round_fwdp[i];
  }

  /*-------------------------------------------------------------------------
    Update the Reverse NAK rounds for current call and also for future
    calls.
  -------------------------------------------------------------------------*/
  dsrlp3_constants.desired_nak_rounds_rev = (*nak_rounds_revp);

  /*-------------------------------------------------------------------------
    Update the Reverse NAKs per round for current call and also for future
    calls.
  -------------------------------------------------------------------------*/
  for (i=0; i<(*nak_rounds_revp); i++)
  {
    dsrlp3_constants.desired_naks_per_round_rev[i] = naks_per_round_revp[i];
  }

  dsrlp3_opt.update_state_var = TRUE;
  return TRUE;

} /* dsrlp_set_all_cur_naks */

/*===========================================================================
  FUNCTION      DSRLP_GET_ALL_NEG_NAKS

  DESCRIPTION   This function will return the negotiated parameters. Note
                that this function has to be called only for getting
                the parameters. (we donot have a corresponding set_all_
                neg_naks function).

  DEPENDENCIES  The values returned here are valid only within a call.

  RETURN VALUE  Values are returned by argument pointers
                TRUE:  If we were able to get the values.
                FALSE: Some argument is wrong. So could not return values.

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_get_all_neg_naks
(
  byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
  byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
  byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
  byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Confirm that all the input paramters are in the range. If they are not
    In range then return FALSE.
  -------------------------------------------------------------------------*/
  if (
      (nak_rounds_fwdp     == NULL)     ||
      (naks_per_round_fwdp == NULL)     ||
      (nak_rounds_revp     == NULL)     ||
      (naks_per_round_revp == NULL)
     )
  {
     return FALSE;
  }


  /*-------------------------------------------------------------------------
    Copy the parameters for forward options
  -------------------------------------------------------------------------*/
  *nak_rounds_fwdp = dsrlp3_opt.negotiated_nak_rounds_fwd;
  for (i=0; i< (*nak_rounds_fwdp); i++)
  {
     naks_per_round_fwdp[i] = dsrlp3_opt.negotiated_naks_per_round_fwd[i];
  }

  /*-------------------------------------------------------------------------
    Copy the parameters for reverse options
  -------------------------------------------------------------------------*/
  *nak_rounds_revp = dsrlp3_opt.negotiated_nak_rounds_rev;
  for (i=0; i< (*nak_rounds_revp); i++)
  {
     naks_per_round_revp[i] = dsrlp3_opt.negotiated_naks_per_round_rev[i];
  }

  /*-------------------------------------------------------------------------
    Since we give the information that caller requested, return TRUE
  -------------------------------------------------------------------------*/
  return TRUE;

} /* dsrlp_get_all_neg_naks */


/*===========================================================================
  FUNCTION      DSRLP_OPT_UPDATE_STATE

  DESCRIPTION  Update the state variables for the NAK scheme.

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_opt_update_state(void)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;       /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* rlp-specific control block */
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Obtain the rscb_ptr and confirm that its not NULL
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[dsrlp3_opt.rscb_index]);
  ASSERT(rscb_ptr != NULL);

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
    Update the Forward NAK rounds for current call and also for future calls
  -------------------------------------------------------------------------*/
  rrcb_ptr->state.nak_rounds_fwd = dsrlp3_constants.desired_nak_rounds_fwd;

  /*-------------------------------------------------------------------------
    Update the Forward NAKs per round for current call and also for future
    calls.
  -------------------------------------------------------------------------*/
  for (i=0; i< dsrlp3_constants.desired_nak_rounds_fwd; i++)
  {
    rrcb_ptr->state.naks_per_round_fwd[i] =
        dsrlp3_constants.desired_naks_per_round_fwd[i];
    DATA_RLP_MSG1(MSG_LEGACY_MED, "NAKS_PER_ROUND_FWD is SET to %d.\n",
             rrcb_ptr->state.naks_per_round_fwd[i]);

  }

  /*-------------------------------------------------------------------------
    Update the Reverse NAK rounds for current call and also for future
    calls.
  -------------------------------------------------------------------------*/
  rrcb_ptr->state.nak_rounds_rev = dsrlp3_constants.desired_nak_rounds_rev;

  /*-------------------------------------------------------------------------
    Update the Reverse NAKs per round for current call and also for future
    calls.
  -------------------------------------------------------------------------*/
  for (i=0; i<dsrlp3_constants.desired_nak_rounds_rev; i++)
  {
    rrcb_ptr->state.naks_per_round_rev[i] =
        dsrlp3_constants.desired_naks_per_round_rev[i];
    DATA_RLP_MSG1(MSG_LEGACY_MED, "NAKS_PER_ROUND_REV is SET to %d.\n",
             rrcb_ptr->state.naks_per_round_rev[i]);
  }

  dsrlp3_opt.update_state_var = FALSE;

} /* dsrlpi_opt_update_state */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
FUNCTION      DSRLP_QOS_INIT

DESCRIPTION   Initializes the RLP QOS fields

DEPENDENCIES  Should be initialized before making the first data call. So
              power on initialization is the best place.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void dsrlp_qos_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Default values. In case of non-assured mode, default value of priority
   adjustment is subscription priority value as defined in IS-707 A2.12, ie
   no adjustment to priority.
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.defaults.qos_incl= TRUE;
  dsrlp_qos_values.defaults.max_blob_type_supp_incl = FALSE;
  dsrlp_qos_values.defaults.blob_type_incl =          TRUE;
  dsrlp_qos_values.defaults.max_blob_type_supported =
                                        DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED;
  dsrlp_qos_values.defaults.assur_mode = FALSE;
  dsrlp_qos_values.defaults.params.non_assur_params.pri_adj_incl = TRUE;
  dsrlp_qos_values.defaults.params.non_assur_params.pri_adj
   = DSRLP_QOS_NON_ASSUR_DEFAULT_PRI_ADJ;

  /*-------------------------------------------------------------------------
   Desired values not to be used unless requested by application,
   so qos_incl set to FALSE,  but for safety, other fields
   are initialized to default values anyway
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.desired.qos_incl = FALSE;
  dsrlp_qos_values.desired.max_blob_type_supp_incl = FALSE;
  dsrlp_qos_values.desired.blob_type_incl =          TRUE;
  dsrlp_qos_values.desired.max_blob_type_supported =
                                        DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED;
  dsrlp_qos_values.desired.blob_type = DSRLP_QOS_DEFAULT_BLOB_TYPE;
  dsrlp_qos_values.desired.assur_mode
  = dsrlp_qos_values.defaults.assur_mode;
  dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl = TRUE;
  dsrlp_qos_values.desired.params.non_assur_params.pri_adj
  = dsrlp_qos_values.defaults.params.non_assur_params.pri_adj;

  /*-------------------------------------------------------------------------
   Rx from bs values not to be used unless actually received from
   base-station, so qos_incl set to FALSE, but for safety, other fields
   are initialized to default values anyway
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.rx_from_bs.qos_incl = FALSE;
  dsrlp_qos_values.rx_from_bs.max_blob_type_supp_incl = FALSE;
  dsrlp_qos_values.rx_from_bs.blob_type_incl =          TRUE;
  dsrlp_qos_values.rx_from_bs.max_blob_type_supported =
                                        DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED;
  dsrlp_qos_values.rx_from_bs.blob_type = DSRLP_QOS_DEFAULT_BLOB_TYPE;
  dsrlp_qos_values.rx_from_bs.assur_mode
  = dsrlp_qos_values.defaults.assur_mode;
  dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj_incl = TRUE;
  dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj
  = dsrlp_qos_values.defaults.params.non_assur_params.pri_adj;

  /*-------------------------------------------------------------------------
   Curr QOS values ptr is initialized to point to default
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.curr_p  =
                      &(dsrlp_qos_values.defaults);

} /* dsrlp_qos_init() */

/*===========================================================================
  FUNCTION      DSRLP_BUILD_QOS_BLOB

  DESCRIPTION   This function creates a qos blob to be sent by mobile to BS

  DEPENDENCIES  None

  RETURN VALUE  Length of blob (0 if no blob needs to be sent, ie default)
                Blob is returned by argument pointer

  SIDE EFFECTS  None
===========================================================================*/
byte dsrlp_build_qos_blob
(
   byte *qos_blob_p                                      /* Resulting blob */
)
{
  byte   blob_len = 0;           /*      holds the calculated BLOB length  */
  byte   new_byte_value=0;       /*      holds the value of the next byte  */
  word   field_length  = 0;      /* holds the field length for next field  */
  word   field_position= 0;      /* holds a cumulative number of bytes used*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(qos_blob_p != NULL);

  /*-------------------------------------------------------------------------
    Only if desired values are set, blob has to be sent.
  -------------------------------------------------------------------------*/
  if ((dsrlp_qos_values.desired.qos_incl) == TRUE)
  {
    if (dsrlp_are_qos_values_same
           (
             &(dsrlp_qos_values.defaults),
             &(dsrlp_qos_values.desired)
           )
        )
     {
       /*--------------------------------------------------------------------
         If desired values set but there's no diff betw default,
         blob doesn't have to be sent.
       --------------------------------------------------------------------*/
       blob_len = 0;
     }
     else
     {
       /*--------------------------------------------------------------------
         Blob needs to be built from desired values.
       --------------------------------------------------------------------*/
       memset(qos_blob_p, 0, DSRLP_MOBILE_QOS_BLOB_LEN_MAX);

       field_position    += field_length;

       /*--------------------------------------------------------------------
         Prepare QOS_BLOB_TYPE_INCL field (2 bits).
       --------------------------------------------------------------------*/
       field_length       = 2;
       if (   ( dsrlp_qos_values.desired.max_blob_type_supp_incl == FALSE )
           && (dsrlp_qos_values.desired.blob_type_incl == FALSE ) )
       {
         new_byte_value = 0x0;
       }
       else if (( dsrlp_qos_values.desired.max_blob_type_supp_incl == FALSE )
            &&  (dsrlp_qos_values.desired.blob_type_incl == TRUE ) )
       {
         new_byte_value = 0x1;
       }
       else if (( dsrlp_qos_values.desired.max_blob_type_supp_incl == TRUE )
            &&  (dsrlp_qos_values.desired.blob_type_incl == FALSE ) )
       {
         new_byte_value = 0x2;
       }
       else if (( dsrlp_qos_values.desired.max_blob_type_supp_incl == TRUE )
            &&  (dsrlp_qos_values.desired.blob_type_incl == TRUE ) )
       {
         new_byte_value = 0x3;
       }
       b_packb
           ( new_byte_value,
             qos_blob_p,
             field_position,
             field_length
           );
      field_position    += field_length;


       /*--------------------------------------------------------------------
         Prepare MAX_QOS_BLOB_TYPE_SUPPORTED field (4 bits).
       --------------------------------------------------------------------*/
       if ( dsrlp_qos_values.desired.max_blob_type_supp_incl == TRUE )
       {
         field_length       = 4;
         new_byte_value = dsrlp_qos_values.desired.max_blob_type_supported;
         b_packb
             ( new_byte_value,
               qos_blob_p,
               field_position,
               field_length
             );
       }
       field_position    += field_length;

       /*--------------------------------------------------------------------
         Prepare QOS_BLOB_TYPE field (4 bits).
       --------------------------------------------------------------------*/
       if ( dsrlp_qos_values.desired.blob_type_incl == TRUE )
       {
         field_length       = 4;
         new_byte_value = dsrlp_qos_values.desired.blob_type;
         b_packb
             ( new_byte_value,
               qos_blob_p,
               field_position,
               field_length
             );
       }
       field_position    += field_length;


       if ( dsrlp_qos_values.desired.assur_mode == TRUE)
       {
         /*------------------------------------------------------------------
           Assured mode not yet implemented.
          -----------------------------------------------------------------*/
         blob_len = 0;
       }
       else
       {
         /*------------------------------------------------------------------
           Non-assured mode
          -----------------------------------------------------------------*/

          /*-----------------------------------------------------------------
            Prepare MODE field (1 bit) with value 0.
          -----------------------------------------------------------------*/
          field_length       = 1;
          new_byte_value     = 0x0;
          b_packb
              ( new_byte_value,
                qos_blob_p,
                field_position,
                field_length
              );
          field_position    += field_length;

          /*-----------------------------------------------------------------
            Prepare NON_ASSURED_PRI_ADJ_INCL field ( 1 bit ).
          -----------------------------------------------------------------*/
          field_length       = 1;
          if ( dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl)
          {
            new_byte_value     = 0x1;
          }
          else
          {
            new_byte_value     = 0x0;
          }
          b_packb
              ( new_byte_value,
                qos_blob_p,
                field_position,
                field_length
              );
          field_position    += field_length;

          /*-----------------------------------------------------------------
            Prepare NON_ASSURED_PRI_ADJ field (4 bits)
          -----------------------------------------------------------------*/
          if ( dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl)
          {
            field_length       = 4;
            new_byte_value     =
                   dsrlp_qos_values.desired.params.non_assur_params.pri_adj;
            b_packb
                ( new_byte_value,
                  qos_blob_p,
                  field_position,
                  field_length
                );
            field_position += field_length;
          }
       }

      /*---------------------------------------------------------------------
        Calculate the BLOB length in bytes
       --------------------------------------------------------------------*/
       if ( field_position % 8 == 0)
       {
         blob_len = (byte)(field_position / 8);
       }
       else
       {
         blob_len = (byte)(field_position/8) + 1;
       }

     } /* blob needs to be built from desired values */

  } /* desired values have been set */

  DATA_RLP_MSG3(MSG_LEGACY_MED, "DesirQOS:incl=%d,mode=%d,priadj=%x",
           dsrlp_qos_values.desired.qos_incl,
           dsrlp_qos_values.desired.assur_mode,
           dsrlp_qos_values.desired.params.non_assur_params.pri_adj
          );

  return blob_len;

} /* dsrlp_build_qos_blob() */


/*===========================================================================
  FUNCTION      DSRLP_PROCESS_QOS_BLOB

  DESCRIPTION   This function processes the qos blob rx from BS

  DEPENDENCIES  None

  RETURN VALUE
              TRUE:  If we were able to get the values.
              FALSE: If blob didn't have qos params.

  SIDE EFFECTS  The rx_from_bs Qos value struct is filled up.
===========================================================================*/
boolean dsrlp_process_qos_blob
(
   const byte *rx_qos_blob_p,                               /* Ptr to blob */
   byte blob_len
)
{
  byte    temp_qos_blob[DSRLP_BS_QOS_BLOB_LEN_MAX];
  word    field_length       = 0;     /* holds the length of the field     */
  word    field_position     = 0;     /* cumulative count of bytes         */
  byte    new_byte;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    For safety, ensure blob length is within acceptable range
  -------------------------------------------------------------------------*/
  if (  ( (int) blob_len == 0 )
     || ( blob_len > (byte) DSRLP_BS_QOS_BLOB_LEN_MAX ) )
  {
    return FALSE;
  }

  ASSERT(rx_qos_blob_p != NULL);

  /*-------------------------------------------------------------------------
    Copy blob into a temp variable that we can work with as blob is a ptr to
    a const param
  -------------------------------------------------------------------------*/
  memscpy(
           (void *) temp_qos_blob,
           DSRLP_BS_QOS_BLOB_LEN_MAX,
           rx_qos_blob_p,
           (size_t) blob_len
         );

  dsrlp_qos_values.rx_from_bs.max_blob_type_supp_incl = FALSE;
  dsrlp_qos_values.rx_from_bs.blob_type_incl = TRUE;

  /*-------------------------------------------------------------------------
    Process MAX_QOS_BLOB_TYPE_SUPPORTED field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length = 4;
  new_byte     = b_unpackb(temp_qos_blob, field_position, field_length);
  dsrlp_qos_values.rx_from_bs.max_blob_type_supported = new_byte;

  /*-------------------------------------------------------------------------
    Process QOS_BLOB_TYPE field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length = 4;
  new_byte     = b_unpackb(temp_qos_blob, field_position, field_length);
  dsrlp_qos_values.rx_from_bs.blob_type = new_byte;

  /*-------------------------------------------------------------------------
    Process ASSURED_MODE field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length = 1;
  new_byte     = b_unpackb(temp_qos_blob, field_position, field_length);
  if ( new_byte == 1 )
  {
     /*----------------------------------------------------------------------
       Assured mode not yet implemented
     ----------------------------------------------------------------------*/
    dsrlp_qos_values.rx_from_bs.assur_mode = TRUE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Non-assured mode
    -----------------------------------------------------------------------*/
    dsrlp_qos_values.rx_from_bs.assur_mode = FALSE;
    /*-----------------------------------------------------------------------
      If BS has sent a QOS blob and it's non-assured mode, we assume
      BS accepted the requested value of priority. If there wasn't a
      requested value, then we revert to default value.
    -----------------------------------------------------------------------*/
    if ( dsrlp_qos_values.desired.qos_incl == TRUE )
    {
      dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj_incl
        = dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl;
      dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj
        = dsrlp_qos_values.desired.params.non_assur_params.pri_adj;
    }
    else
    {
      dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj_incl
        = dsrlp_qos_values.defaults.params.non_assur_params.pri_adj_incl;
      dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj
        = dsrlp_qos_values.defaults.params.non_assur_params.pri_adj;
    }
  }

  return TRUE;
} /* dsrlp_process_qos_blob() */

/*===========================================================================
  FUNCTION      DSRLP_CALC_CURR_QOS_VALUES

  DESCRIPTION   This function calculates and stores current qos values
                based on desired values requested and the values received
                from BS.

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_calc_curr_qos_values(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Logic is that if BS hasn't sent QOS blob, default values are assumed.
   But if BS has sent QOS blob, those are considered the current values.
   Refer IS-707 A-2.12
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
   curr values ptr initialized to point to default values
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.curr_p  =
                      &(dsrlp_qos_values.defaults);


  /*-------------------------------------------------------------------------
   If qos blob has been received from BS, curr values ptr should point
   to rx_from_bs values.
  -------------------------------------------------------------------------*/
  if ( dsrlp_qos_values.rx_from_bs.qos_incl == TRUE )
  {
    dsrlp_qos_values.curr_p  =
                      &(dsrlp_qos_values.rx_from_bs);
  }

  DATA_RLP_MSG3(MSG_LEGACY_MED,
           "CurrQOS:incl=%d,mode=%d,priadj=%x",
           (dsrlp_qos_values.curr_p)->qos_incl,
           (dsrlp_qos_values.curr_p)->assur_mode,
           (dsrlp_qos_values.curr_p)->params.non_assur_params.pri_adj
          );

} /* dsrlp_calc_curr_qos_values() */

/*===========================================================================
  FUNCTION      DSRLP_GET_CURR_QOS_VALUES

  DESCRIPTION   This function returns current QOS values

  DEPENDENCIES  None

  RETURN VALUE  Current values of Qos  are returned as an argument pointer

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_get_curr_qos_values
(
   dsrlp_qos_s_type *dsrlp_qos_s_p             /* Curr Qos values returned */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrlp_qos_s_p->qos_incl =
    (dsrlp_qos_values.curr_p)->qos_incl;
  dsrlp_qos_s_p->max_blob_type_supp_incl =
    (dsrlp_qos_values.curr_p)->max_blob_type_supp_incl;
  dsrlp_qos_s_p->blob_type_incl =
    (dsrlp_qos_values.curr_p)->blob_type_incl;
  dsrlp_qos_s_p->max_blob_type_supported =
    (dsrlp_qos_values.curr_p)->max_blob_type_supported;
  dsrlp_qos_s_p->blob_type =
    (dsrlp_qos_values.curr_p)->blob_type;
  dsrlp_qos_s_p->assur_mode =
    (dsrlp_qos_values.curr_p)->assur_mode;

  if ( dsrlp_qos_s_p->assur_mode == TRUE )
  {
    /*-----------------------------------------------------------------------
      Assured mode not yet implemented
    -----------------------------------------------------------------------*/
  }
  else
  {
    /*-----------------------------------------------------------------------
      Non-assured mode
    -----------------------------------------------------------------------*/
    dsrlp_qos_s_p->params.non_assur_params.pri_adj_incl =
    (dsrlp_qos_values.curr_p)->params.non_assur_params.pri_adj_incl;
    dsrlp_qos_s_p->params.non_assur_params.pri_adj =
    (dsrlp_qos_values.curr_p)->params.non_assur_params.pri_adj;
  }

} /* dsrlp_get_curr_qos_values() */

/*===========================================================================
  FUNCTION      DSRLP_GET_DESIRED_QOS_VALUES

  DESCRIPTION   This function returns desired QOS values

  DEPENDENCIES  None

  RETURN VALUE  Desired values of Qos  are returned as an argument pointer

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_get_desired_qos_values
(
   dsrlp_qos_s_type *dsrlp_qos_s_p          /* Desired Qos values returned */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrlp_qos_s_p->qos_incl =
    dsrlp_qos_values.desired.qos_incl;
  dsrlp_qos_s_p->max_blob_type_supp_incl =
    dsrlp_qos_values.desired.max_blob_type_supp_incl;
  dsrlp_qos_s_p->blob_type_incl =
    dsrlp_qos_values.desired.blob_type_incl;
  dsrlp_qos_s_p->max_blob_type_supported =
    dsrlp_qos_values.desired.max_blob_type_supported;
  dsrlp_qos_s_p->blob_type =
    dsrlp_qos_values.desired.blob_type;
  dsrlp_qos_s_p->assur_mode =
    dsrlp_qos_values.desired.assur_mode;

  if ( dsrlp_qos_s_p->assur_mode == TRUE )
  {
    /*-----------------------------------------------------------------------
        Assured mode not yet implemented
    -----------------------------------------------------------------------*/
  }
  else
  {
    /*-----------------------------------------------------------------------
        Non-assured mode
    -----------------------------------------------------------------------*/
    dsrlp_qos_s_p->params.non_assur_params.pri_adj_incl =
     dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl;
    dsrlp_qos_s_p->params.non_assur_params.pri_adj =
     dsrlp_qos_values.desired.params.non_assur_params.pri_adj;
  }

} /* dsrlp_get_desired_qos_values() */

/*===========================================================================
  FUNCTION      DSRLP_SET_DESIRED_QOS_VALUES

  DESCRIPTION   This function sets desired Qos values

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_set_desired_qos_values
(
   dsrlp_qos_s_type *dsrlp_qos_s_p                   /* Desired Qos values */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Lint Suppress Error */
  DATA_MDM_ARG_NOT_CONST(dsrlp_qos_s_p);

  dsrlp_qos_values.desired.qos_incl
    = dsrlp_qos_s_p->qos_incl;
  dsrlp_qos_values.desired.max_blob_type_supp_incl =
    dsrlp_qos_s_p->max_blob_type_supp_incl;
  dsrlp_qos_values.desired.blob_type_incl =
    dsrlp_qos_s_p->blob_type_incl;
  dsrlp_qos_values.desired.max_blob_type_supported =
    dsrlp_qos_s_p->max_blob_type_supported;
  dsrlp_qos_values.desired.blob_type =
    dsrlp_qos_s_p->blob_type;
  dsrlp_qos_values.desired.assur_mode
    = dsrlp_qos_s_p->assur_mode;

  if ( dsrlp_qos_s_p->assur_mode == TRUE )
  {
    /*-----------------------------------------------------------------------
        Assured mode not yet implemented
    -----------------------------------------------------------------------*/
  }
  else
  {
    /*-----------------------------------------------------------------------
        Non-assured mode
    -----------------------------------------------------------------------*/
    dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl
      = dsrlp_qos_s_p->params.non_assur_params.pri_adj_incl;
    dsrlp_qos_values.desired.params.non_assur_params.pri_adj
      = dsrlp_qos_s_p->params.non_assur_params.pri_adj;
  }
} /* dsrlp_set_desired_qos_values() */

/*===========================================================================
  FUNCTION      DSRLP_GET_DESIRED_QOS_NON_ASSUR_PRI

  DESCRIPTION   This function gets desired value of QOS non-assured mode
                priority adjustment

  DEPENDENCIES  None

  RETURN VALUE  TRUE- If value of non-assured mode priority got successfully
                FALSE- If desired values not set, or if mode is assured
                Desired priority adjustment value returned as argument ptr.

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_get_desired_qos_non_assur_pri
(
   byte *pri_adj_value_p           /* Desired value of priority adjustment */
)
{
  dsrlp_qos_s_type dsrlp_qos_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrlp_get_desired_qos_values ( &dsrlp_qos_s );

  /*------------------------------------------------------------------------
    If desired values weren't set or if they were set but they were assured
    mode, return error.
  ------------------------------------------------------------------------*/
  if (
      ( dsrlp_qos_s.qos_incl == FALSE ) ||
      ( dsrlp_qos_s.assur_mode == TRUE )
     )
  {
     return FALSE;
  }
  if ( dsrlp_qos_s.params.non_assur_params.pri_adj_incl == FALSE )
  {
    return FALSE;
  }

  *pri_adj_value_p = dsrlp_qos_s.params.non_assur_params.pri_adj;

  return TRUE;

} /* dsrlp_get_desired_qos_non_assur_pri() */

/*===========================================================================
  FUNCTION      DSRLP_SET_DESIRED_QOS_NON_ASSUR_PRI

  DESCRIPTION   This function sets desired value of QOS non-assured mode
                priority adjustment

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_set_desired_qos_non_assur_pri
(
   byte pri_adj_value              /* Desired value of priority adjustment */
)
{
  dsrlp_qos_s_type dsrlp_qos_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrlp_qos_s.qos_incl = TRUE;
  dsrlp_qos_s.assur_mode = FALSE;
  dsrlp_qos_s.max_blob_type_supp_incl = FALSE;
  dsrlp_qos_s.blob_type_incl = TRUE;
  dsrlp_qos_s.max_blob_type_supported = DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED;
  dsrlp_qos_s.blob_type = DSRLP_QOS_DEFAULT_BLOB_TYPE;
  dsrlp_qos_s.params.non_assur_params.pri_adj_incl = TRUE;
  dsrlp_qos_s.params.non_assur_params.pri_adj = pri_adj_value;

  dsrlp_set_desired_qos_values ( &dsrlp_qos_s );
} /* dsrlp_set_desired_qos_non_assur_pri() */


/*===========================================================================
  FUNCTION      DSRLP_ARE_QOS_VALUES_SAME

  DESCRIPTION   This function takes as argument two qos structures and
                compares whether they are the same. (Eg: useful for
                checking if desired values equals default values)

  DEPENDENCIES  If either argument has field qos_incl false, ie it doesn't
                care about Qos, function returns TRUE

  RETURN VALUE
              TRUE:  If values are identical in both qos structures
              FALSE: If values are not identical

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_are_qos_values_same
(
   dsrlp_qos_s_type *dsrlp_qos_s_1_p,       /* Ptr to struct of qos params */
   dsrlp_qos_s_type *dsrlp_qos_s_2_p        /* Ptr to struct of qos params */
)
{
  boolean result = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Lint Suppress Error */
  DATA_MDM_ARG_NOT_CONST(dsrlp_qos_s_1_p);
  DATA_MDM_ARG_NOT_CONST(dsrlp_qos_s_2_p);

  /*-------------------------------------------------------------------------
    Only meaningful if both of them have qos included
  -------------------------------------------------------------------------*/
  if (
      (dsrlp_qos_s_1_p->qos_incl) &&
      (dsrlp_qos_s_2_p->qos_incl)
     )
  {
    if (
        (dsrlp_qos_s_1_p->assur_mode) !=
        (dsrlp_qos_s_2_p->assur_mode)
       )
    {
      /*---------------------------------------------------------------------
        One is assured, and the other is non-assured
      ---------------------------------------------------------------------*/
      result = FALSE;
    }
    else
    {
      if ( (dsrlp_qos_s_1_p->assur_mode) == FALSE )
      {
        /*-------------------------------------------------------------------
           Non-assured mode
        -------------------------------------------------------------------*/

        /*-------------------------------------------------------------------
          Check value of priority adj field
         ------------------------------------------------------------------*/
        if (
             ( dsrlp_qos_s_1_p->params.non_assur_params.pri_adj_incl == TRUE)
           &&( dsrlp_qos_s_2_p->params.non_assur_params.pri_adj_incl == TRUE)
           )
        {
          if ((dsrlp_qos_s_1_p->params.non_assur_params.pri_adj) ==
              (dsrlp_qos_s_2_p->params.non_assur_params.pri_adj))
            result = TRUE;
          else
            result = FALSE;
        }
        else
        {
          result = FALSE;
        }
      }
      else
      {
         /*------------------------------------------------------------------
           Assured mode not yet implemented
         ------------------------------------------------------------------*/
        result = FALSE;
      }
    }
  }
  return result;
} /* dsrlp_are_qos_values_same() */

/*===========================================================================
  FUNCTION      DSRLP_EVAL_QOS_BLOB

  DESCRIPTION   This function evaluates the qos blob rx from BS. For now,
                we donot do anything. So accept whatever the BS gives.

  DEPENDENCIES  None

  RETURN VALUE
                SNM_ACCEPT_CFG: DS can accept anything that it receives from BS
                because, currently DS does not process it.

  SIDE EFFECTS
===========================================================================*/
/*lint -save -e715 unreferenced variable*/
snm_eval_status_type dsrlp_eval_qos_blob( byte   bs_blob_len,
                                          byte * bs_blob,
                                          boolean can_suggest_alt,
                                          byte * ret_alt_blob_len,
                                          byte * ret_alt_blob
                                         )
{
  /* Lint Suppress Error */
  DATA_MDM_ARG_NOT_CONST(bs_blob);
  DATA_MDM_ARG_NOT_CONST(ret_alt_blob_len);
  DATA_MDM_ARG_NOT_CONST(ret_alt_blob);

  return (SNM_ACCEPT_CFG);
}
/*lint -restore*/

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================
  FUNCTION      DSRLP_IS_INITED

  DESCRIPTION   If RLP is already INIT'd, then returns TRUE.  Else returns
                FALSE.

                Takes an RLP instance as input.

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_inited
(
  byte rscb_index                        /* RLP instance                   */
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  boolean ret_val = FALSE;

  if ( rscb_index < DSRLP_MAX_RSCB_ELEMENTS)
  {
    rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

    if( rscb_ptr->cfg.ver_ptr == NULL)
    {
      ret_val = FALSE;
    }

    else
    {
      ret_val = TRUE;
    }

  }
  else
  {
    ASSERT(0);
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION      DSRLP_IS_RLP_ENCRYPT_ON

  DESCRIPTION   Tells whether RLP encryption should be on

  DEPENDENCIES  None

  RETURN VALUE  True/False

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_rlp_encrypt_on
(
  void
)
{
  return FALSE;
} /* dsrlp_is_rlp_encrypt_on() */


/*===========================================================================
  FUNCTION      DSRLP_IS_L2_L3_ENCRYPT_ON

  DESCRIPTION   Tells whether L2 or L3 encryption is on

  DEPENDENCIES  None

  RETURN VALUE  True/False

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_l2_l3_encrypt_on
(
  void
)
{
  return FALSE;
} /* dsrlp_is_l2_l3_encrypt_on() */


/*===========================================================================

FUNCTION DSRLP_ENABLE_QOS

DESCRIPTION
  This function is sets the rx_from_bs.qos_incl to TRUE.

DEPENDENCIES
  None.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/

void dsrlp_enable_qos(void)
{
  dsrlp_qos_values.rx_from_bs.qos_incl = TRUE;
}

/*===========================================================================

FUNCTION DSRLP_DISABLE_QOS

DESCRIPTION
  This function is sets the rx_from_bs.qos_incl to TRUE.

DEPENDENCIES
  None.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/

void dsrlp_disable_qos(void)
{
  dsrlp_qos_values.rx_from_bs.qos_incl = FALSE;
}


/*===========================================================================

FUNCTION dsrlp_register_snm_initializers

DESCRIPTION
  This function registers the RLP callback functions with SNM.

DEPENDENCIES
  None.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
snm_status_type dsrlp_register_snm_initializers(void)
{
  snm_status_type     status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  status = snm_reg_rlp_blob_generator(CAI_SO_PPP_PKT_DATA_3G,
                                      dsrlp_build_rlp_blob);
  if (status == SNM_FAILURE)
  {
    ERR_FATAL("Could not register initializer",0,0,0);
  }

#ifdef FEATURE_IS2000_REL_A
  status =   snm_reg_qos_functions( CAI_SO_PPP_PKT_DATA_3G,
                                    dsrlp_build_qos_blob,
                                    dsrlp_eval_qos_blob);

  if (status == SNM_FAILURE)
  {
    ERR_FATAL("Could not register initializer",0,0,0);
  }

#endif

  return status;
}

#ifdef FEATURE_HDR_QOS

/*===========================================================================

FUNCTION dsrlp_get_resv_list

DESCRIPTION
   Given SRID & ver num, returns teh reservation list, if any associated with
   that RLP.

DEPENDENCIES
  None.

RETURN VALUE
   Current reservation list

SIDE EFFECTS
  None.

===========================================================================*/
int dsrlp_get_resv_list
(
 dsrlp_rlp_identity_type   rlp,
 dsrlp_resv_list_type*     resv_list
)
{
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
  uint8                     rscb_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rscb_index = dsrlp_get_rlp_rscb_index(rlp.flow, rlp.ver);

  if( rscb_index == DSRLPI_INVALID_RSCB_INDEX )
  {
    DATA_RLP_MSG2(MSG_LEGACY_ERROR,
                  "RLP %d ver %d not found. Cant delete Rev list for RLP",
                  rlp.flow,
                  rlp.ver);
    return -1;
  }

  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);
  memscpy(resv_list, sizeof(dsrlp_resv_list_type),
  	  &(rscb_ptr->cfg.rsv), sizeof(dsrlp_resv_list_type));

  return 0;
} /* dsrlp_get_resv_list */

/*===========================================================================

FUNCTION dsrlp_update_resv_list

DESCRIPTION
   Given teh rlp flow & ver, updates the rsv_list corresponding to this
   rlp with the new rsv list.

DEPENDENCIES
  None.

RETURN VALUE
   resets resv count to 0.

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_update_resv_list
(
  dsrlp_rlp_identity_type  rlp,
  dsrlp_resv_list_type     new_rsv_list
)
{
  int j;
  uint8                     rscb_index;
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rscb_index = dsrlp_get_rlp_rscb_index(rlp.flow, rlp.ver);

  if( rscb_index == DSRLPI_INVALID_RSCB_INDEX )
  {
    DATA_RLP_MSG2(MSG_LEGACY_ERROR,
                  "RLP %d ver %d not found. Cant Reset Rev list for RLP",
                  rlp.flow,
                  rlp.ver);
    return;
  }

  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);

  /*-------------------------------------------------------------------------
    Removed premption macro as this function is called only by DS task and
    rsv.label and rsv.count parameters are modified only in this function.
  -------------------------------------------------------------------------*/

  /* Check the new list count is less than the maxium number allowed */
  for(j = 0; j < new_rsv_list.count && j < DS707_MAX_N_RESV_PER_DIR; j++)
  {
    rscb_ptr->cfg.rsv.label[j] =new_rsv_list.label[j];
  }
  rscb_ptr->cfg.rsv.count = new_rsv_list.count;

} /* dsrlp_update_resv_list() */

/*===========================================================================

FUNCTION DSRLP_GET_DROPPED_STALE_PACKETS

DESCRIPTION
   Returns if packets has been dropped for this RLP

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  Packets have been dropped
  FALSE: Packets have not been dropped

SIDE EFFECTS
  Resets variable tracking dropping of packets

===========================================================================*/
void dsrlp_get_dropped_stale_packets
(
 dsrlp_rlp_identity_type   rlp,
 boolean                   *dropped
)
{
  uint8                     rscb_index;
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rscb_index = dsrlp_get_rlp_rscb_index(rlp.flow, rlp.ver);

  if( rscb_index == DSRLPI_INVALID_RSCB_INDEX )
  {
    DATA_RLP_MSG2(MSG_LEGACY_ERROR, "RLP %d ver %d not found. "
                  "Cant get the dropped stale packet flag",
                  rlp.flow,
                  rlp.ver);
    return;
  }

  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);

  /* Need a criticial section, as this variable
     is also accessed in the HDRTX task and
     DS task when a GAUP happens */
  dsrlpi_enter_control_path_crit_sect( rscb_ptr, rscb_ptr->cfg.ver );
  *dropped = rscb_ptr->cfg.dropped_stale_packet;
  rscb_ptr->cfg.dropped_stale_packet = FALSE;
  dsrlpi_leave_control_path_crit_sect( rscb_ptr, rscb_ptr->cfg.ver );


} /* dsrlp_get_dropped_stale_packets() */

/*===========================================================================

FUNCTION  DSRLP_GET_MARQ_INFO

DESCRIPTION This function clears the MARQ information for a given
            RLP after it is read. This allows the application to
            get the latest info about MARQ since it's last request.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Resets the boolean in the sess_ctl_blk of the rlp for which the query is
  made.

===========================================================================*/
void dsrlp_get_marq_info
(
 dsrlp_rlp_identity_type   rlp,
 boolean                   *marq_dropped
)
{
  uint8                     rscb_index;
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rscb_index = dsrlp_get_rlp_rscb_index(rlp.flow, rlp.ver);

  if( rscb_index == DSRLPI_INVALID_RSCB_INDEX )
  {
    DATA_RLP_MSG2(MSG_LEGACY_ERROR,
                  "RLP %d ver %d not found.Cannot get MARQ info",
                  rlp.flow,
                  rlp.ver);
    return;
  }

  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);

  /* Need a criticial section, as this variable
     is accessed multiple places */
  dsrlpi_enter_control_path_crit_sect( rscb_ptr, rscb_ptr->cfg.ver );
  *marq_dropped = rscb_ptr->cfg.dropped_marq_packet;
  rscb_ptr->cfg.dropped_marq_packet = FALSE;
  dsrlpi_leave_control_path_crit_sect( rscb_ptr, rscb_ptr->cfg.ver );

} /* dsrlp_get_marq_info() */

#endif /* FEATURE_HDR_QOS */

#endif /* RLP 3 || HDR defined */

/*===========================================================================
                 SU Level APIs
===========================================================================*/

/*===========================================================================
  FUNCTION      DSRLP_ANY_SIG_TO_SEND

  DESCRIPTION   This function will return TRUE if any  important RLP
                frames ( called as Signalling in name of fn) are to be
                transmitted.
                Important frames are:
                    a) SYNC/SYNC-ACK/AC
                    b) NAK's:
                    c) Retransmissions to be done

               If need be add IDLE and Fill frames also.

  DEPENDENCIES
              This function should be called only if Data is secondary
              service. If Data is primary service this function should
              not be called.(because, entire channel is for data to
              send)

  RETURN VALUE
              TRUE:  If RLP has control frames to send
              FALSE: If RLP has NO control frames to send

  SIDE EFFECTS  None
===========================================================================*/

boolean dsrlp_any_sig_to_send(dsrlp_tx_sig_enum_type *priority)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;        /* rlp-specific control block */
  boolean                  ret_val  = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined(FEATURE_IS2000_REL_A)&& (defined(FEATURE_DS_RLP3)||defined(FEATURE_HDR)))
  rscb_ptr      = &(dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX]);

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
      Find out if MS RLP has established.
           If no, then we are in process of establishing. So, DS should get a
           chance on FCH to send signaling data continously to calculate RTT.

      If MS has already established RLP,
           then see if we have to send any Naks. We should be able to transmit
           the NAK's immediately (1/2 rate is sufficient).

  -------------------------------------------------------------------------*/
  if (rrcb_ptr->state.state != DSRLPI_ESTABLISHED_STATE)
  {
     *priority = DSRLP_TX_CONTINUOUS;
     ret_val = TRUE;
     DATA_RLP_MSG2(MSG_LEGACY_MED, " RLP Data : SYNC :%d , RTT:%d", rrcb_ptr->state.state, rrcb_ptr->state.rlp_delay);
  }
  else if (rrcb_ptr->state.ack_to_send == TRUE)
  {
     *priority = DSRLP_TX_NO_DELAY;
     ret_val = TRUE;
     DATA_RLP_MSG0(MSG_LEGACY_MED, " RLP Data : ack_to_send = TRUE");
  }
  else if(rrcb_ptr->state.naks_to_send == TRUE)
  {
     *priority = DSRLP_TX_NO_DELAY;
     ret_val = TRUE;
  }
  else if(rrcb_ptr->state.idle_timer == 0)
  {
   /*--------------------------------------------------------------------------
     If the idle timer expires we want to send the idle frame right away.
    ---------------------------------------------------------------------------*/
     *priority = DSRLP_TX_NO_DELAY;
     ret_val = TRUE;
  }
  else
  {
     *priority =  DSRLP_TX_NONE;
  }
  return (ret_val);

#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR,"dsrlp_any_sig_to_send() not supported");
  return FALSE;
#endif

} /* dsrlp_any_sig_to_send() */

/*===========================================================================
  FUNCTION      DSRLP_GET_NEXT_DSM_ITEM_CHAIN

  DESCRIPTION   This function takes the pointer to the current beginning of
                a chain of DSM items, which contains RLP payload to be
                transmitted by the MUX layer, as input, and returns pointer
                to the next item in chain. If there is no more item in
                chain, NULL is returned.


  DEPENDENCIES  None

  RETURN VALUE  Pointer to the next item in chain; if there is no more
                item in chain, return NULL.

  SIDE EFFECTS  None
===========================================================================*/
dsm_item_type *dsrlp_get_next_dsm_item_chain( dsm_item_type *cur_ptr_to_payload )
{
#if (defined(FEATURE_IS2000_REL_A)&& (defined(FEATURE_DS_RLP3)||defined(FEATURE_HDR)))
    /* Lint Suppress Error */
   DATA_MDM_ARG_NOT_CONST(cur_ptr_to_payload);
   /*------------------------------------------------------------------------
      Assert that the pointer to the current head of the chain should not
      be NULL.
   ------------------------------------------------------------------------*/
   if (NULL == cur_ptr_to_payload)
   {
      ASSERT (0);
      return (NULL);
   }
   /*-----------------------------------------------------------------------
      If there is more DSM item in chain, return the pointer; otherwise
      return NULL to indicate the end of the chain
   -----------------------------------------------------------------------*/
   return (cur_ptr_to_payload->pkt_ptr);
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                "dsrlp_get_next_dsm_item_chain() not supported");
  return NULL;
#endif
} /* dsrlp_get_next_dsm_item_chain */

/*===========================================================================

FUNCTION      DSRLP_ENABLE_INACTIVITY_TIMER

DESCRIPTION   Enable the inactivity timer in each RLP by setting the flag
              "enable_inactivity_timer" to TRUE.

DEPENDENCIES  None

RETURN VALUE  TRUE:  The inactivity timer is enabled
              FALSE: The inactivity timer is disabled

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_enable_inactivity_timer( dsrlp_cta_client_id_enum_type client, boolean enable)
{
  uint8                     rscb_index;      /* Index to RLP control block */
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;       /* RLP-specific control block */
  boolean                   reval;           /* Return value */
  uint8                     mask = 0x01;
  boolean                   start_cta_timer = FALSE;
  boolean                   stop_cta_timer = FALSE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
  mask = (uint8)(mask << (uint8)client);
  if (enable)
  {
    if ( dsrlp_disable_inactivity_timer_bitmap == mask)
    {
      start_cta_timer = TRUE;
    }
    dsrlp_disable_inactivity_timer_bitmap &= ~mask;
  }
  else
  {
    if (dsrlp_disable_inactivity_timer_bitmap == 0)
    {
      stop_cta_timer = TRUE;
    }
    dsrlp_disable_inactivity_timer_bitmap |= mask;
  }

  if (dsrlp_disable_inactivity_timer_bitmap != 0)
  {
    reval = FALSE;
  }
  else
  {
    reval = TRUE;
  }
#ifdef FEATURE_HDR
  for (rscb_index = DSRLP_HDR_BASE_INDEX;
       rscb_index < (DSRLP_MAX_NUM_HDR_MRLP_REV + DSRLP_MAX_NUM_HDR_MRLP_FWD + DSRLP_MAX_NUM_HDR_RLP + DSRLP_HDR_BASE_INDEX);
       rscb_index++ )
  {
    rscb_ptr   = &(dsrlpi_rscb_array[rscb_index]);

    /*-------------------------------------------------------------------------
      Enter critical section. HDR Data call flow does not run on interrupt
      context and so critical section suffice here.
    -------------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);

    rscb_ptr->cfg.enable_inactivity_timer = reval;

    /*-------------------------------------------------------------------------
      Leave critical section.
    -------------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);
  }

  if (start_cta_timer)
  {
    DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Enhanced_cta client(TAP etc) enable inactivity timer act as phy link up");
    hdrrlp_phy_link_up_inactivity_timer_hdlr();
  }
  if (stop_cta_timer)
  {
    DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Enhanced_cta client(TAP etc) disable inactivity timer act as phy link down");
    hdrrlp_phy_link_down_inactivity_timer_hdlr();
  }
#endif /* FEATURE_HDR */
  return reval;
#else
  return FALSE;
#endif
}

/*===========================================================================

FUNCTION DSRLP_BUILD_TX_FRAMES

DESCRIPTION
  This function builds RLP frame(s) to be transmitted over the air.
  This is the interface between RLP transmit and the multiplex sublayer.
  The multiplex sublayer can either call this function once, to build all
  the frames or call this several times to build a few frames at a time.

  Input Parameters:

  The parameter to this function is a pointer to a structure which contains
  an array of frames that the mux sublayer wants RLP to process. Each element
  of this array includes all the information that RLP needs to process the
  frame independently.   This per frame data structure is passed to a number
  of functions within the RLP frame building code.The information passed
  with each frame includes fields which are set by the mux layer, and fields
  which are set by RLP, when RLP builds the frame.

  Fields set by Mux layer for entire se of frames asked.

    sr_id         : service id for this frame. For SCH frames, sr_id is in
                    the MuxPDU header. For FCH/DCCH frames, this is set by
                    the Mux layer after looking up the appropriate sr_id for
                    primary or secondary. MUX layer must ask for frames of
                    one sr_id in one invocation of this function.

  The fields set by the mux layer per frame:

    srvc_type     : For FCH/DCCH frames, the mux layer sets the srvc_type
                    to DSRLP_PRIMARY_SRVC or DSRLP_SECONDARY. For SCH/SCCH,
                    Mux layer sets the srvc_type to DSRLP_GENERIC_SRVC.

    mux_pdu_type  : Set to DSRLP_MUX_PDU_1 for rateset1 FCH/DCCH; set to
                    DSRLP_MUX_PDU_2 for rateset2 FCH/DCCH; set to
                    DSRLP_MUX_PDU_3 for SCH operation.

    fr_cat_mask   : For FCH/DCCH, Mux layer sets this mask to
                    ( DSRLP_CAT_DATA_MASK | DSRLP_CAT_CTL_MASK |
                      DSRLP_CAT_IDLE_MASK), because data, ctl/idle frames
                    can be sent on FCH/DCCH.
                    For SCH/SCCH, Mux layer sets this mask to
                    DSRLP_CAT_DATA_MASK, because only data frames can be
                    sent on SCH/SCCH.

    max_frame_size: This specifies the max. number of bytes avail. for RLP to
                    put in the frame. For MuxPDU type1 and 2, when multiple
                    frame rates are available, the Mux layer should set
                    this field to the size of the highest frame rate.
                    The Mux layer sets this field to the number of bytes
                    available for RLP to put itsheader and payload excluding
                    the type bits.
                    Basically, take the number of bits of RLP payload as
                    specified in the standard, subtract 8 from it, divide it
                    by 8 and round up to the next byte.
                    e.g for secondary rate1 frames on FCH rateset1,
                    max_frame_size = (168-8)/8 = 20 bytes.

                    For MuxPDU3, this will be set to MuxPDU size-1. e.g. for
                    SCH double size LTUs in Service Option 33, this will be
                    43 bytes.

    fr_rate_mask  : The mux layer ors in all the different frame rates
                    allowed for this frame in this mask. For FCH, the mask
                    can take on a whole range of values depending on primary
                    or secondary service. For DCCH/SCH/SCCH, the mask is
                    usually DSRLP_RATE_1_MASK.


  The fields set by the RLP:

    idle_frame    : RLP sets this to TRUE, if this frame can be DTXed by
                    the Mux layer (for DCCH/secondary operation). When this
                    is FALSE, the Mux layer has to send out this frame.

    fr_rate       : RLP sets this to the frame rate this frame should be
                    used by the Mux layer to send out this frame.

    fr_item_ptr   : Pointer to dsm packet chain (may have multiple dsm items)
                    RLP should ensure that the used fields are properly set
                    in the item, to the number of bits used by RLP rounded
                    up to the next byte.
                    The mux layer shall transmit the number of bytes in the
                    item chain. If there are any left over bits, the Mux
                    layer shall pad the bits with zeros.
                    RLP shall set the unused part of the item chain to zeros

DEPENDENCIES
  dsrlp_init() should have been called.

  Mux layer must ask for frames for only one sr_id in one invocation of this
  function.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_build_tx_frames
(
  dsrlp_rev_frame_block_type *rev_fr_blk     /* Ptr to frames to trasmit   */
)
{
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
  byte    fr_num;                            /* Ctr to loop through frames */
  dsrlp_rev_frame_type  *rev_fr_ptr;         /* Ptr to a specific tx frame */
  boolean frame_built = FALSE;               /* frame has been built       */
  struct dsrlpi_fcn_ptr_table *fcn_ptr_table;/* Ptr table to RLP functions */
  dsrlpi_sess_ctl_blk_type*rscb_ptr;         /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr;         /* rlp-specific control block */
  dsrlp_stats_info_type    *stats_ptr;       /* local stats pointer        */
  boolean non_idle_blank = TRUE;             /* frames wasnt Idle or Blank */
  boolean missed_txc = FALSE;
  boolean tx_data_frs = FALSE;               /* Sent relevant data?        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Set the vars in case we return immediately.
  -------------------------------------------------------------------------*/
  rev_fr_blk->num_frames = 0;
  for(fr_num = 0; fr_num< rev_fr_blk->max_frames; fr_num++)
  {
    if ((rev_fr_blk->rev_arr[fr_num].free_dsm_pkt == TRUE) &&
        (rev_fr_blk->rev_arr[fr_num].fr_item_ptr  != NULL))
    {
      missed_txc = TRUE;
      dsm_free_packet(&(rev_fr_blk->rev_arr[fr_num].fr_item_ptr));
    }
    else
    {
      rev_fr_blk->rev_arr[fr_num].fr_item_ptr  = NULL;
    }
    rev_fr_blk->rev_arr[fr_num].free_dsm_pkt = TRUE;
    rev_fr_blk->rev_arr[fr_num].fr_rate      = DSRLP_RATE_BLANK;
  }
  if (missed_txc == TRUE)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "RLP didn't build frames in time?");
  }

  /*------------------------------------------------------------------------
  1X RLP might be in the process of initializing. Check the state before
  proceeding.
  ------------------------------------------------------------------------*/
  if (!dsrlp_get_1xrlp_ready_flag())
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "is_1xrlp_ready is FALSE. 1X RLP not Initalized yet" );
    return;
  }

  /*-------------------------------------------------------------------------
    The call may have ended, but the message has not yet gotten to txc.
    So go ahead and check to see if the ver_ptr is null, and if so, then
    dump the rxc frames.

    NOTE: Although the variable is named rev_fr_blk->sr_id, it is actually
    the RLP instance for this service (currently only one RLP instance is
    supported). When actaully building the individual frames, the correct
    sr_id value stored in rscb_ptr is used for the mux header.
  -------------------------------------------------------------------------*/
  rscb_ptr      = &( dsrlpi_rscb_array[ rev_fr_blk->sr_id]);

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  if (rscb_ptr->cfg.ver_ptr == NULL)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Ver_ptr is NULL for sr_id %x",rscb_ptr->cfg.sr_id);
    return;
  }

  if (rrcb_ptr->state.reset_active == TRUE)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP RESET during TX, get out RLP:%d",rscb_ptr->cfg.sr_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Using the sr_id, get the session control block and fcn_ptr_table.
  -------------------------------------------------------------------------*/
  stats_ptr     = &rrcb_ptr->stats;
  fcn_ptr_table = rscb_ptr->cfg.ver_ptr->fcn_ptr_table;
  ASSERT( fcn_ptr_table != NULL);

  /*-------------------------------------------------------------------------
    Increase the counter of 20ms periods. This assumes that this function
    will only be called once per 20ms period, or we can use a new interface
    that lets us tell if this is not the case.
  -------------------------------------------------------------------------*/
  stats_ptr->tx_20ms_fr_cnt++;

  /*-------------------------------------------------------------------------
    Make sure that the tx watermark has been set up.  If not, then just
    punt out of this function.  This means that RLP will take a little
    longer in sync'ing up, etc... but since RLP has no data to transmit,
    it has no effect.  Mux will react by sending 1/8th rate NULLs.
  -------------------------------------------------------------------------*/
  if (rscb_ptr->cfg.tx_wm_ptr == NULL)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Watermark is NULL for sr_id %x",rscb_ptr->cfg.sr_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Before building any frames, first go ahead and cleanup the saved frames
    queue, of any frames that are acked by the BS.
  -------------------------------------------------------------------------*/
  dsrlptxq_remove_acked(rrcb_ptr,rrcb_ptr->state.peer_vn);

  /*-------------------------------------------------------------------------
    Go through all the frames in the frame array and try to build them.
  -------------------------------------------------------------------------*/
  for( fr_num =0; fr_num< rev_fr_blk->max_frames; fr_num++)
  {
    rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
    
    /*-----------------------------------------------------------------------
      Get a pointer to the specific frame that is going to be build.
    -----------------------------------------------------------------------*/
    rev_fr_ptr               = &( rev_fr_blk->rev_arr[fr_num]);
    rev_fr_ptr->idle_frame   = FALSE;
    rev_fr_ptr->free_dsm_pkt = TRUE;
    rev_fr_ptr->fr_item_ptr  = NULL;
    frame_built              = FALSE;

    /*-----------------------------------------------------------------------
      If the state is not established, then RLP needs to send SYNC/ACK frames
      While RLP is sending SYNC/ACK frames, it cannot send any other frames,
      so just exit out of loop.
    -----------------------------------------------------------------------*/

    if ( ( ((rrcb_ptr->state).state   != DSRLPI_ESTABLISHED_STATE) &&
           ((rrcb_ptr->state).state   != DSRLPI_RT_MEAS_STATE)
         ) || ((rrcb_ptr->state).ack_to_send == TRUE)
       )
    {
      if( (rev_fr_ptr->fr_cat_mask) &  DSRLP_CAT_CTL_MASK)
      {
        ASSERT( fcn_ptr_table->tx_init != NULL);
        frame_built= fcn_ptr_table->tx_init( rrcb_ptr, rev_fr_ptr);
        if ( frame_built == TRUE)
        {
          tx_data_frs = TRUE;
          fr_num++;
        }
        rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
        break;
      }
    }
    
    /*-----------------------------------------------------------------------
      If this frame has been blanked out.
    -----------------------------------------------------------------------*/
    if( ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_BLANK_MASK))

    {
      ASSERT(frame_built == FALSE );
      //frame_built= TRUE;
      rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
      continue;
    }

    /*-----------------------------------------------------------------------
      For P2 mode, it is possible that FCH is BLANK'd, but DCCH is not.
      Do this check just to be sure code does not run amok.  If true,
      then the check for BLANK_MASK will catch that case.
    -----------------------------------------------------------------------*/
    if ( ((rrcb_ptr->state).state   != DSRLPI_ESTABLISHED_STATE) &&
         ((rrcb_ptr->state).state   != DSRLPI_RT_MEAS_STATE))
    {
      DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Did not send SYNC/ACK or FCH/DCCH "
                    "State:%x, Mask %x, Frame Num %x",
                    (rrcb_ptr->state).state, rev_fr_ptr->fr_cat_mask, fr_num);

      rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
      break;
    }

    /*-----------------------------------------------------------------------
      If there were NAKs waiting to be sent, send a NAK.
    -----------------------------------------------------------------------*/

    if( ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_CTL_MASK) &&
        ((rrcb_ptr->state).naks_to_send == TRUE))
    {
      ASSERT( fcn_ptr_table->tx_nak != NULL && (frame_built == FALSE ));
      frame_built= fcn_ptr_table->tx_nak( rrcb_ptr, rev_fr_ptr);

      if (frame_built == TRUE)
      {
        tx_data_frs = TRUE;
        rrcb_ptr->state.last_fill_nak_cnt = 0;
      }
    }

    /*-----------------------------------------------------------------------
      Send a fill frame if the other side has not been updated with our
      V(N) in a while.  Do this after the NAK, as that can also sends our
      V(N) to the other side.
    -----------------------------------------------------------------------*/
    if ((frame_built == FALSE) &&
        ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_IDLE_MASK) &&
        (rrcb_ptr->state.last_fill_nak_cnt >= dsrlp_last_fill_nak_gap))
    {
      ASSERT( fcn_ptr_table->tx_fill != NULL);
      frame_built= fcn_ptr_table->tx_fill( rrcb_ptr, rev_fr_ptr);

      if (frame_built == TRUE)
      {
        rrcb_ptr->state.last_fill_nak_cnt = 0;
        DATA_RLP_MSG1(MSG_LEGACY_MED, "Forced a FILL frame. RLP:%d",rscb_ptr->cfg.sr_id);
      }
      else
      {
        DATA_RLP_MSG1(MSG_LEGACY_ERROR,"Could not construct FILL frame RLP:%d",
                      rscb_ptr->cfg.sr_id);
      }
    }

    /*-----------------------------------------------------------------------
      If there are retransmit frames queued, then build a retransmitted data
      frame.
    -----------------------------------------------------------------------*/
    if( ( frame_built == FALSE) &&
        ( DSRLPRTX_HAS_REXMITS(rrcb_ptr)) &&
        ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_DATA_MASK))
    {
      ASSERT( fcn_ptr_table->tx_rexmit != NULL);
      frame_built= fcn_ptr_table->tx_rexmit( rrcb_ptr, rev_fr_ptr);
      stats_ptr->tx_rexmit_fr_cnt++;
      tx_data_frs = TRUE;
    }

    /*-----------------------------------------------------------------------
      If there are data available in the transmit watermark, then build a
      new frame.
    -----------------------------------------------------------------------*/
    if( (frame_built == FALSE) &&
        ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_DATA_MASK) &&
        ( ((rrcb_ptr->state).tx_item_ptr != NULL) ||
          (((rscb_ptr->cfg).tx_wm_ptr    != NULL) &&
           ((rscb_ptr->cfg).tx_wm_ptr->current_cnt != 0)) ))
    {
      ASSERT( fcn_ptr_table->tx_new_data != NULL);
      frame_built= fcn_ptr_table->tx_new_data( rrcb_ptr, rev_fr_ptr);

      /*---------------------------------------------------------------------
        Set the flag if this is the first new packet transmitted out of
        dormancy.
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.ppp_data_transfer_after_dormancy = TRUE;

      /*---------------------------------------------------------------------
        Increase the counter of new frames transmitted. Ignore rare case
        frame_built = FALSE.
      ---------------------------------------------------------------------*/
      stats_ptr->tx_new_data_fr_cnt++;
      tx_data_frs = TRUE;
    }

    /*-----------------------------------------------------------------------
      If no frame was built, see if an idle frame can be sent. Otherwise send
      a blank indication to the Mux layer.
    -----------------------------------------------------------------------*/
    if( frame_built == FALSE)
    {
      /*---------------------------------------------------------------------
        If RLP could build an idle, the tx_idle() function will return TRUE.
        In this case, set num_frames to include the idle. If the tx_idle()
        function returned FALSE, then num_frames is not incremented, and
        the Mux layer sends a BLANK frame
      ---------------------------------------------------------------------*/
      non_idle_blank = FALSE;

      //Initialize frame rate to blank in case non-idle frame building fails
      //due to reasons such as no-DSM memory
      rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
      if ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_IDLE_MASK)
      {
        /*-------------------------------------------------------------------
          Note, if an idle frame is built, frame_built is still false,
          because it will be used to change idle_timer values.
        -------------------------------------------------------------------*/
        ASSERT( fcn_ptr_table->tx_idle != NULL);
        frame_built = fcn_ptr_table->tx_idle( rrcb_ptr, rev_fr_ptr);

        if ( frame_built == TRUE)
        {
          fr_num++;
        }
        else
        {
          //Even no idle frame, so tell MUX the frame is blank
          rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
          DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Could not make FCH IDLE frame");
        }
      } /* if idle mask */
      rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
      break;
    }
    rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
  } /* for - looping through all frames */


  /*-------------------------------------------------------------------------
    RLP Options may change. Check if the state variables have to be updated.
    if yes, then update the variables.
  -------------------------------------------------------------------------*/
  if (dsrlp3_opt.update_state_var == TRUE)
  {
    dsrlp_opt_update_state();
  }

  rev_fr_blk->num_frames = fr_num;
  if ( non_idle_blank == TRUE)
  {
    /*---------------------------------------------------------------------
      If a non-idle/non-blank frame was sent, reset the idle timer to its
      initial values. The idle timer is used for sending idle frames on
      channels that can be DTXed to ensure that the receiver knows about
      the last data frames sent in a burst.
    ---------------------------------------------------------------------*/
    rrcb_ptr->state.idle_timer_active = TRUE;
    rrcb_ptr->state.idle_timer        = DSRLPI_IDLE_TIMER_DEF;
    rrcb_ptr->state.idle_fr_xmit_cntr = 0;
  }

  /*-------------------------------------------------------------------------
    Update the stats with the count of contiguous IDLE frames transmitted.
    Reset the counter if a valid data/ctl frame is transmitted
  -------------------------------------------------------------------------*/
  if(tx_data_frs == TRUE)
  {
    stats_ptr->tx_contig_idle_fr_cnt = 0;
  }
  else
  {
    stats_ptr->tx_contig_idle_fr_cnt++;
  }
  /*-------------------------------------------------------------------------
    Tell DSMGR if any data frames were transmitted, so that dormant mode
    processing can occur.
  -------------------------------------------------------------------------*/
  ds_rlp_data_txed(tx_data_frs);

#ifdef FEATURE_IS2000_R_SCH
    ds707_scrm_rlp_cback(rscb_ptr->cfg.tx_wm_ptr->current_cnt);
#endif

#ifdef FEATURE_IS2000_CHS
  if ( (rscb_ptr->cfg.tx_wm_ptr->current_cnt > 0)      ||
       (DSRLPRTX_HAS_REXMITS(rrcb_ptr))                ||
       (rrcb_ptr->state.naks_to_send == TRUE)          ||
       (rrcb_ptr->state.state == DSRLPI_RT_MEAS_STATE) ||
       (rrcb_ptr->state.state == DSRLPI_ACK_STATE)     ||
         (rrcb_ptr->state.ack_to_send == TRUE )
     )
  {
    ds707_chs_leave_chs();
  }

  ds707_chs_txed_data(tx_data_frs);

#endif

#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_build_tx_frames() not supported");
#endif
  return;
} /* dsrlp_build_tx_frames() */

/*===========================================================================

FUNCTION DSRLP_UPDATE_TA_GAP

DESCRIPTION
  This function RLP NAK(s) age so that the NAK can be sent at the right time
  and not slowed because of the Tuneaway
  This is the interface between RLP transmit and the multiplex sublayer.
  The multiplex sublayer can call this function once, to update all
  the RLP nak at a time.

  Input Parameters:

  Fields set by Mux layer for entire se of frames asked.

    sr_id         : service id for this frame. For SCH frames, sr_id is in
                    the MuxPDU header. For FCH/DCCH frames, this is set by
                    the Mux layer after looking up the appropriate sr_id for
                    primary or secondary. MUX layer must ask for frames of
                    one sr_id in one invocation of this function.

  
    fr_missed    :  Number of frames that have been missed as tuneaway was 
                    in progress.

DEPENDENCIES
  dsrlp_init() should have been called.

  Mux layer must ask for frames for only one sr_id in one invocation of this
  funciton.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_update_ta_gap
( 
  byte sr_id,            /* MUX: Service Identifier   */
  int16 frame_missed     /* MUX: Frames missed in TA gap  */
)
{
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
  struct dsrlpi_fcn_ptr_table  *fcn_ptr_table;/* Ptr table to RLP functions */
  dsrlpi_sess_ctl_blk_type     *rscb_ptr;     /* rlp-specific control block */
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr;     /* rlp-specific control block */
  dsrlp_stats_info_type        *stats_ptr;    /* local stats pointer        */

  q_type                       *nak_q_ptr;     /*  ptr to rlp's nak list   */
  dsrlpi_nak_list_entry_type   *nak_entry_ptr; /*  nak list entry          */
  dsrlpi_nak_list_entry_type   *temp_ptr;      /*  nak list entry          */
  dsrlpi_state_info_type       *state_ptr;     /*  curr state              */ 
  uint32                        modulus;       /* sequence modulus         */
  uint32                        old_vn;        /* the old V(N)             */
  uint32                       first,last;     /* dummy place holders      */
  int16                        fr_missed  = 0; /* Frames missed for each nak*/
  boolean                      nak_aborted = FALSE; /* Flag for nak abort */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_RLP_MSG1(MSG_LEGACY_MED, "IN dsrlp_update_ta_gap Frame Cnt = %d",
                frame_missed);
    
  /*-------------------------------------------------------------------------
    The call may have ended, but the message has not yet gotten to txc.
    So go ahead and check to see if the ver_ptr is null, and if so, then
    dump the rxc frames.

    NOTE: Although the variable is named rev_fr_blk->sr_id, it is actually
    the RLP instance for this service (currently only one RLP instance is
    supported). When actaully building the individual frames, the correct
    sr_id value stored in rscb_ptr is used for the mux header.
  -------------------------------------------------------------------------*/
  rscb_ptr      = &( dsrlpi_rscb_array[sr_id]);

  /* 1x has only one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);

  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];
  ASSERT(rrcb_ptr != NULL);

  rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));

  if (rscb_ptr->cfg.ver_ptr == NULL)
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Ver_ptr is NULL for sr_id %x",rscb_ptr->cfg.sr_id);
    rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
    return;
  }

  /*-------------------------------------------------------------------------
    Using the sr_id, get the session control block and fcn_ptr_table.
  -------------------------------------------------------------------------*/
  stats_ptr     = &rrcb_ptr->stats;
  fcn_ptr_table = rscb_ptr->cfg.ver_ptr->fcn_ptr_table;
  ASSERT( fcn_ptr_table != NULL);

  /*-------------------------------------------------------------------------
    Increase the counter of 20ms periods. This assumes that this function
    will only be called once per 20ms period, or we can use a new interface
    that lets us tell if this is not the case.
  -------------------------------------------------------------------------*/
  stats_ptr->tx_20ms_fr_cnt++;

  /*-------------------------------------------------------------------------
    Age the NAK list based on the missed frame count received from MUX
  -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
    Set up some local vars.  These are used more for cleanliness of code,
    so that some statements can fit on one line
  ------------------------------------------------------------------------*/
  state_ptr = &(rrcb_ptr->state);
  nak_q_ptr = &(state_ptr->nak_q);
  modulus   = rrcb_ptr->cfg_ptr->seq_modulus;
  
  temp_ptr = (dsrlpi_nak_list_entry_type *)q_check(nak_q_ptr);

  while (temp_ptr != NULL)
  {
    /*---------------------------------------------------------------------
      Within the loop, it is possible for the current nak entry to be
      deleted. Therefore, make a copy of the ptr to the current entry, and
      then immediately get the next entry.  This way, the current entry can
      be deleted safely.  (Otherwise, once the current entry is deleted, it
      becomes impossible to find the next entry).
    ---------------------------------------------------------------------*/ 
    nak_entry_ptr = temp_ptr;
    temp_ptr = DSRLPNAK_GET_NEXT_ENTRY(nak_q_ptr,temp_ptr);

    /*---------------------------------------------------------------------
      Since this routine goes through every entry in the list, this is a
      good place to confirm that all RLP entries are sytactically correct:
      
      4  ->  7   GOOD
      4  ->  4a  GOOD
      4a ->  4b  GOOD
      4a ->  5   GOOD
      
      4a -> 6    INCORRECT!
      4  -> 4    INCORRECT
      4  -> 3    INCORRECT (assuming no modulus rollover)
    ---------------------------------------------------------------------*/ 
    first = nak_entry_ptr->first_seq;
    last  = nak_entry_ptr->last_seq;

    ASSERT((last != first) &&
           (DSRLPSEQ_GE_SEQ_CHECK(last,first,modulus)));

    if (DSRLPSEQ_HAS_S_SEQ(first))
    {
      if (DSRLPSEQ_HAS_S_SEQ(last))
      {
        ASSERT(DSRLPSEQ_HAS_SAME_SEQ(first,last));
      }
      else
      {
        ASSERT(DSRLPSEQ_GET_NEXT_SEQ(first,modulus) == last);
      }
    }
    else if (DSRLPSEQ_HAS_S_SEQ(last))
    {
      ASSERT(DSRLPSEQ_HAS_SAME_SEQ(first,last));
    }

    /* Reset frame missed and nak_aborted for this nak entry */
    fr_missed = frame_missed;
    nak_aborted = FALSE;
    /*---------------------------------------------------------------------
      When tuneaway is in progress, the default 20ms timer callbacks for TX 
      and RX path will not be invoked. So the NAK packets do not age when 
      tuneaway is in progress. On completion of tuneaway the timers start 
      and the aging process continues. To compensate the aging due to TA
      update the ages of each NAKs in accordance with the value of frame
      missed received from the MUX layer
      fr_missed - here denotes the number of 20ms cycle missed due to TA
      nak_age, naks_left and nak_rnd needs to be updated for the fr_missed
      Below if and while conditions will update the above three parameters
    ---------------------------------------------------------------------*/ 

    /*---------------------------------------------------------------------
      If the nak_age is less than the fr_missed, then decrement the age by 
      fr_missed and turn on the flag to send nak and reset the fr_missed
      Neednt turn on the send naks flag as it needs to age more
    ---------------------------------------------------------------------*/ 
    if(nak_entry_ptr->age > fr_missed)
    {
        nak_entry_ptr->age -= fr_missed;
        fr_missed = 0;
     }    
     /*---------------------------------------------------------------------
      If the fr_missed and nak ptr is valid then,
      - Decrement the fr_missed by age 
      - Update the send naks flag is condition satified
      - Decrement the naks_left flag 
      - compute the rounds left 
         * if its exceeded max then delete entry
         * if its less than the max rounds then update the age and naks_left
     ---------------------------------------------------------------------*/
     while ((fr_missed > 0) && (nak_aborted != TRUE))
     {
        if (nak_entry_ptr->age > 0) 
        {
          if(fr_missed > nak_entry_ptr->age)
          {
             fr_missed -= nak_entry_ptr->age;
             nak_entry_ptr->age = 0;
          }
          else
          {
             nak_entry_ptr->age -= fr_missed;
             fr_missed = 0;
          }
        }
        /*--------------------------------------------------------------------
          If the nak timer has expired, then need to set a flag so that the
          Tx routine sends the Naks.  However, it is possible that in this
          round, zero naks are to be sent; in that case, do not set the flag.
          Instead, update the nak timer to the next round information.  If
          this is the last round, then remove this entry.
        --------------------------------------------------------------------*/ 
        ASSERT(nak_entry_ptr->curr_rnd <= DSRLP_MAX_NAK_ROUNDS);  

        if (nak_entry_ptr->age == 0)
        {
          /*---------------------------------------------------------------------
            Enter critical section here as Tx task fiddles with naks_left value.
          ---------------------------------------------------------------------*/
          DS_ENTER_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);
        
         /*---------------------------------------------------------------------
           If naks_left is valid then turn on the flag to send NAKs. If the
           fr_missed has value then decrement the nak_left and the fr_missed.
         ---------------------------------------------------------------------*/
        if(nak_entry_ptr->naks_left > 0)
        {
          if(fr_missed > 0)
          {
            nak_entry_ptr->naks_left--;
            fr_missed--;
          }
        }
        else /* if(nak_entry_ptr->naks_left = 0)*/
        {
          /*---------------------------------------------------------------
           This will update the curr_rnd and accordingly reset the age
           naks_left value
          ---------------------------------------------------------------*/
          nak_aborted = rlpnaki_next_nak_rnd(rrcb_ptr,nak_entry_ptr);
        }
        /*---------------------------------------------------------------------
          Leave critical section.
        ---------------------------------------------------------------------*/
        DS_LEAVE_CRIT_SECTION(&rscb_ptr->cfg.crit_sect);
      } /* (nak_entry_ptr->age == 0) */
    } /* ((fr_missed > 0) && (nak_aborted != TRUE)) */    
 
    /*--------------------------------------------------------------------
      Set the naks_to_send flag true if the nak entry still exist 
    --------------------------------------------------------------------*/
    if((fr_missed == 0 ) && (nak_aborted != TRUE) && (nak_entry_ptr->age == 0))
    {
      state_ptr->naks_to_send = TRUE;          
    }
  }        
  /*--------------------------------------------------------------------
   It is possible that the first item in the list was deleted. Update
   the V(N).  If the NAK list is empty, the V(N) = V(R).
  --------------------------------------------------------------------*/
  temp_ptr = (dsrlpi_nak_list_entry_type *)q_check(nak_q_ptr);
  old_vn   = state_ptr->needed_seq_num;

  if (temp_ptr != NULL)
  {
    state_ptr->needed_seq_num = temp_ptr->first_seq;
  }
  else
  {
    state_ptr->needed_seq_num = state_ptr->expected_seq_num;
  }
  rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
  
  DATA_RLP_MSG2(MSG_LEGACY_HIGH, " old_vn %d  state_ptr->needed_seq_num %d",
                old_vn, state_ptr->needed_seq_num);
  
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_update_ta_gap() not supported");
#endif
  return;
} /* dsrlp_update_ta_gap */

/*===========================================================================
FUNCTION DSRLP_GET_MAX_MS_NAK_ROUNDS_FWD

DESCRIPTION
  This function the MAX number of nak rounds the mobile will support
  on the forward channel

  Input Parameters: None

DEPENDENCIES
  None

RETURN VALUE
  The max number of nak rounds

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_get_max_ms_nak_rounds_fwd()
{
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
    return(dsrlp3_constants.max_nak_rounds_fwd);
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                "dsrlp_get_max_ms_nak_rounds_fwd() not supported");
  return 0;
#endif
}

/*===========================================================================

FUNCTION DSRLP_GET_MAX_MS_NAK_ROUNDS_REV

DESCRIPTION
  This function the MAX number of nak rounds the mobile will support
  on the reverse channel

  Input Parameters: None

DEPENDENCIES
  None

RETURN VALUE
  The max number of nak rounds

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_get_max_ms_nak_rounds_rev()
{
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
    return(dsrlp3_constants.max_nak_rounds_rev);
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                "dsrlp_get_max_ms_nak_rounds_rev() not supported");
  return 0;
#endif
}

/*===========================================================================
FUNCTION      DSRLP_CHECK_SR_ID

DESCRIPTION   Check if the SR_ID for the service running on a particular RLP
              instance (rscb array index) is the same as in the MUX header in
              the frame

DEPENDENCIES  The passed in RSCB_INDEX is good.

RETURN VALUE  TRUE if the SR_ID matches, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_check_sr_id(byte rscb_index, dsm_item_type *frame_ptr)
{
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
    /* Suppress Lint Error */
    DATA_MDM_ARG_NOT_CONST(frame_ptr);

    if (((frame_ptr->data_ptr[ 0]) & 0xe0) == dsrlpi_rscb_array[rscb_index].cfg.sr_id << 5)
    {
        return TRUE;
    }
    else
    {
        /*------------------------------------------------------------------
          Note that for 1x rate this message may show up and you can ignore
          it.For 2x,4x,8x,16x this should not happen.
        ------------------------------------------------------------------*/
        DATA_RLP_MSG2(MSG_LEGACY_HIGH, " SR_ID mismatch BS:%d & MS:%d ",
                               (((frame_ptr->data_ptr[ 0]) & 0xe0) >> 5),
                               dsrlpi_rscb_array[rscb_index].cfg.sr_id);

        return FALSE;
    }
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_check_sr_id() not supported");
  return FALSE;
#endif
}

/*===========================================================================
FUNCTION      DSRLP_GET_SR_ID

DESCRIPTION   Return the SR_ID for the service running on a particular RLP
              instance (rscb array index)

DEPENDENCIES  The passed in RSCB_INDEX is good. Also dsrlp_init must have
              been invoked first for the returned SR_ID to be good.

RETURN VALUE  The SR_ID corresponding to the RLP instance

SIDE EFFECTS  None
===========================================================================*/
byte dsrlp_get_sr_id(byte rscb_index)
{
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
   return dsrlpi_rscb_array[rscb_index].cfg.sr_id;
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "dsrlp_get_sr_id() not supported");
  return 0;
#endif
}

/*===========================================================================
  FUNCTION      DSRLP_INSERT_PDU5NULL_HDR_TURBO

  DESCRIPTION   This function will insert Null header for the Turbo
                encoded PDUs.

  DEPENDENCIES

  RETURN VALUE

  SIDE EFFECTS
===========================================================================*/

void dsrlp_insert_pdu5null_hdr_turbo(dsm_item_type *out_pkt_ptr)
{
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
    byte temp_data_arr[3];       /* Array that will hold the first 3 bytes
                                    of outgoing Mux PDU5. Size is 3 bytes
                                    because max length of PDU5 can be
                                    obtained from 3 bytes                  */
    uint16 data_len;             /* Indicates # of bytes copied from PDU   */
    uint16 pdu5_len  = 0;        /* Length of the PDU */
    uint16 pdu5_temp = 0;        /* Temp variable used to calculate Length */
    uint16 offset    = 0;        /* variable used to hold offset where the
                                    PDU payload starts. For 8 bit length
                                    offset has a value of 2; for 16bit it
                                    has offset of 3                        */
    byte pkt_len_indicator;      /* Length indicator is in the first
                                    location
                                    If length indicator == 00
                                     then we have fixed length.
                                    else if (length indicator == 01)
                                     then we have 8 bits length
                                    else if (length indicator == 10)
                                     then we have 16 bits length           */
    dsm_item_type *temp_pkt_ptr; /* used to hold input pkt ptr for
                                    processing                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
     Make a copy; So that we operate on "copied ptr" and avoid any
       changes to passed pointer.
    -----------------------------------------------------------------------*/
    temp_pkt_ptr = out_pkt_ptr;

    /*-----------------------------------------------------------------------
      Extract the data from the passed pointer. Since the length information
      is only in first 3 bytes; extract only first 3 bytes.
      Confirm that we got the 3 bytes. If we did not get them, some thing is
      amiss; return after printing error.
    -----------------------------------------------------------------------*/
    data_len = dsm_extract(temp_pkt_ptr,0,temp_data_arr,3);
    if (data_len !=3)
    {
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Not enough data to insert Null Hdr");
        return;
    }

    /*-----------------------------------------------------------------------
      Get the Length Indicator for this PDU. The length indicator is the bits
      5,6 from MS Byte i.e. xxxx LLxx. The LL bits are length indicator
    -----------------------------------------------------------------------*/
    pkt_len_indicator = (temp_data_arr[0] & RLP3_MUXPDU5_LEN_IND_MASK);

    /*-----------------------------------------------------------------------
      See if the length indicator is xxxx01xx i.e. 1 byte.
      If yes, then extract the length from the array into pdu5_len.
    -----------------------------------------------------------------------*/
    if (pkt_len_indicator == 0x04)
    {
        DATA_RLP_MSG0(MSG_LEGACY_LOW, "Frame with Len Indicator 01 ");
        /*-------------------------------------------------------------------
          Extract the length (x: Don't care; L: Length)
          temp_data_array[0]: xxxx xxLL
          temp_data_array[1]: LLLL LLxx
          Combine both the length segments into one variable, pdu5_len
        -------------------------------------------------------------------*/
        pdu5_temp =(uint16) (temp_data_arr[0] & 0x03);
        pdu5_len  =(uint16) (pdu5_temp << 6);
        pdu5_temp =(uint16) temp_data_arr[1];
        pdu5_len  |= (uint16)(pdu5_temp >> 2);

        /*-------------------------------------------------------------------
          Since the length is total number of bytes after the length
          field, we should have an offset so that we poke NULL PDU HDR
          (0xE0) at the right place
          Since we have two bytes before start of MuxPDU5 payload, the
          offset is 2.
        -------------------------------------------------------------------*/
        offset    = 2;
    }
    else if (pkt_len_indicator == 0x08)
    {

        DATA_RLP_MSG0(MSG_LEGACY_LOW, "Frame with Len Indicator 10 ");

        /*-----------------------------------------------------------------
          Now obtain the length of the PDU5 Extract the length
           (x: Don't care; L: Length)
          temp_data_array[0]: xxxx xxLL
          temp_data_array[1]: LLLL LLLL
          temp_data_array[2]: LLLL LLxx
         Combine both the length segments into one variable, pdu5_len
        -----------------------------------------------------------------*/
        pdu5_temp = (temp_data_arr[0] & 0x03);
        pdu5_len  = (uint16) (pdu5_temp << 14);
        pdu5_temp = temp_data_arr[1];
        pdu5_len  |= (uint16) (pdu5_temp << 6);
        pdu5_temp = (temp_data_arr[2] & 0xFF);
        pdu5_len  |= (uint16)(pdu5_temp >> 2);

        /*-------------------------------------------------------------------
          Since the length is total number of bytes after the length
          field, we should have an offset so that we poke NULL PDU HDR
          (0xE0) at the right place
          Since we have two bytes before start of MuxPDU5 payload, the
          offset is 3.
        -------------------------------------------------------------------*/
        offset    = 3;
    }
    else if (pkt_len_indicator == 0x0C)
    {
       /*--------------------------------------------------------------------
          Should not come here. Since we came here anyway, let's print out
          error and return.
          Expect that some other entity should catch this problem and fix
          the code.
       --------------------------------------------------------------------*/
       DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid Len Indicator %x",pkt_len_indicator);
       return;
    }
    else
    {
        /*-------------------------------------------------------------------
          Nothing to do return
        -------------------------------------------------------------------*/
       return;
    }

    /*-----------------------------------------------------------------------
      The exact place where null pdu header should be added is given by
      Offset + PDU5_len ;
    -----------------------------------------------------------------------*/
    offset += pdu5_len;

    /*-----------------------------------------------------------------------
      By now we obtained the Length of the PDU. So we should poke the Null
      PDU Header byte (0xE0) at offset

      While (offset > size),
         obtain next ptr and reduce the offset.

      Note,  look at used field but should look at the size field.
    -----------------------------------------------------------------------*/
    temp_pkt_ptr = out_pkt_ptr;
    while ( (temp_pkt_ptr != NULL) && (offset >= temp_pkt_ptr->size))
    {
        temp_pkt_ptr = temp_pkt_ptr->pkt_ptr;
        offset -= out_pkt_ptr->size;
    }

    /*-----------------------------------------------------------------------
      Insert the Null PDU header
      For safety, confirm that the pkt_ptr is not NULL
    -----------------------------------------------------------------------*/
    if (temp_pkt_ptr != NULL)
    {
        temp_pkt_ptr->data_ptr[offset] = 0xE0;
    }
    else
    {
        DATA_RLP_MSG2(MSG_LEGACY_ERROR, "offset:%x,Out_ptr:%x;Should not come here",offset,out_pkt_ptr);
    }
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                "dsrlp_insert_pdu5null_hdr_turbo() not supported");
#endif
  return;
} /* dsrlp_insert_pdu5null_hdr_turbo */

/*===========================================================================
FUNCTION       DSRLP_GET_RLP_LOGGING_LOG_ID

DESCRIPTION    Accessor method for dsrlp_log_id_table

PARAMETERS
  log_id_index

RETURN VALUE
  log id from dsrlp_log_id_table
  DS_LOG_RLP_INVALID(-1) - incorrect parameter

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_log_rlp_phy_channel_enum_type dsrlp_get_rlp_logging_log_id
(
  byte log_id_index
)
{
  if(log_id_index >= RLP_MAX_REV_PDU)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid index into dsrlp_log_id_table");
    return DS_LOG_RLP_INVALID;
  }
  return dsrlp_log_id_table[log_id_index];
}

/*===========================================================================
FUNCTION       DSRLP_GET_FRAME_LEN

DESCRIPTION    Accessor method for dsrlp_len_table

PARAMETERS
  fr_rate
  srvc_type
  mux_pdu_type

RETURN VALUE
  frame length from dsrlp_len_table
  0 - incorrect parameters or rate is not applicable.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_get_frame_len
(
  dsrlp_rate_enum_type fr_rate,
  dsrlp_srvc_enum_type srvc_type,
  dsrlp_mux_pdu_enum_type mux_pdu_type
)
{
  if(fr_rate >= DSRLP_RATE_ERASURE ||
     srvc_type >= DSRLP_GENERIC_SRVC ||
     mux_pdu_type >= DSRLP_MUX_PDU_3)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid index into dsrlp_len_table");
    return 0;
  }
  return dsrlp_len_table[fr_rate][srvc_type][mux_pdu_type];
}

/*===========================================================================
FUNCTION       DSRLP_GET_USED_FRAME_LEN

DESCRIPTION    Accessor method for dsrlp_used_len_table

PARAMETERS
  fr_rate
  srvc_type
  mux_pdu_type

RETURN VALUE
  frame length from dsrlp_used_len_table
  0 - incorrect parameters or rate is not applicable.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_get_used_frame_len
(
  dsrlp_rate_enum_type fr_rate,
  dsrlp_srvc_enum_type srvc_type,
  dsrlp_mux_pdu_enum_type mux_pdu_type
)
{
  if(fr_rate >= DSRLP_RATE_ERASURE ||
     srvc_type >= DSRLP_GENERIC_SRVC ||
     mux_pdu_type >= DSRLP_MUX_PDU_3)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid index into dsrlp_used_len_table");
    return 0;
  }
  return dsrlp_used_len_table[fr_rate][srvc_type][mux_pdu_type];
}

/*===========================================================================
FUNCTION      dsrlp_get_rscb_cfg_ver

DESCRIPTION   This function returns the Config version type for a given RSCB
              Index.

DEPENDENCIES  NONE.

RETURN VALUE  Config Version Enum type for a particular RLP Flow

SIDE EFFECTS  NONE
===========================================================================*/
dsrlp_ver_enum_type dsrlp_get_rscb_cfg_ver
(
  byte index
)
{
  dsrlp_ver_enum_type cfg_ver;
  dsrlpi_sess_ctl_blk_type *rscb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the RSCB Index passed is Invalid then return INVALID */
  if( index == DSRLPI_INVALID_RSCB_INDEX )
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,"Invalid RSCB Index");
    return DSRLP_VER_INVALID;
  }

  rscb_ptr = &(dsrlpi_rscb_array[index]);
  cfg_ver = rscb_ptr->cfg.ver;

  return cfg_ver;
} /* dsrlp_get_rscb_cfg_ver */

/*===========================================================================
FUNCTION       DSRLP_GET_1XRLP_READY_FLAG

DESCRIPTION    Accessor method for is_1xrlp_ready flag

PARAMETERS
  None

RETURN VALUE
  Returns the value of is_1xrlp_ready flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_get_1xrlp_ready_flag(void)
{
  return is_1xrlp_ready;

} /* dsrlp_get_is_1xrlp_ready  */

/*===========================================================================
FUNCTION       DSRLP_SET_1XRLP_READY_FLAG

DESCRIPTION    Accessor method to set the is_1xrlp_ready flag

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_set_1xrlp_ready_flag(boolean flag)
{
  DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Setting is_1xrlp_ready to: %d ", flag);
  is_1xrlp_ready = flag;
  return;
} /* dsrlp_set_is_1xrlp_ready */


