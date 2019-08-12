/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I G H     D A T A     R A T E

       R A D I O   L I N K   P R O T O C O L   E X T E R N A L   F I L E

GENERAL DESCRIPTION
  This file contains the implementation of the IS-856 Radio Link
  Protocol (HDR RLP).

  This file contains functions that implement the external interfaces for 
  HDR RLP.  Function prototypes for the external functions in this file are 
  given in hdrrlp.h.
  
  The naming convention in this file is as follows:

  All external functions start with hdrrlp.
  There are no internal functions in this file.
  All internal variables (in file scope) start with hdrrlp..

  Note that HDR RLP internal design and interfaces uses the internal RLP 
  interfaces to the generic RLP framework, as the software re-uses many
  of the data structures, etc.
  
EXTERNALIZED FUNCTIONS

  hdrrlp_queue_msg()
    Called from the PCP layer, it directs RLP to build RLP signaling messages
    used for control, and queue it onto SLP transmit queue.  

  hdrrlp_get_pkt()
    Called from the PCP layer, it builds and returns an RLP packet, 
    containing either new or re-transmit data. 

  hdrrlp_put_pkt()
    Called from the PCP layer, it processes received RLP packets.

  hdrrlp_msg_cb(*msg_ptr)
    Callback function invoked by SLP whenever a signaling message is received
    for RLP.  This function queues up a received RLP signaling messages to 
    the RLP signaling message RX queue.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Before calling any DSRLP functions, dsm_init() must have been called
  to initialize the memory pool.
  
  Here is how interface sequencing will work:

    At the beginning of the RLP session, call:
      dsrlp_init( sr_id, ver) with sr_id and version.  For HDR, we default to
        sr_id of PRIMARY.
      dsrlp_setup_io( sr_id, tx_wm, rx_wm) with sr_id and the input
         and output watermarks.
         
   Every "build frame" transmit interrupt, the PCP layer calls
     hdrrlp_queue_msg( ..) which will queue up any signaling messages
       to SLP's queue

     hdrrlp_get_pkt_cb() is called on a per packet basis by PCP to 
       build up an RLP data packet.  This is called until the maximum
       size packet has been reached and/or there are no more bytes 
       available for transmit.

   Every "receive frame" interrupt, PCP calls     
     hdrrlp_put_pkt_cb(...) - which processes any signaling messages
       queued up from SLP, as well as processing the incoming data packet.

   At the end of the call:
     dsrlp_cleanup( sr_id) - will cleanup memory and data structures.  

 Copyright (c) 2000-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrlp.c#1 $

when       who        what, where, why
--------   ---        ------------------------------------------------------- 
06/26/13   sc         Replace all the memcpy and memmove with memscpy and 
                      memsmove as its safer.
10/30/12   sd         Memory Reduction modification
03/21/12   msh        TCB changes due to Core changes  
12/07/11   msh        Replaced FEATURE_HDR_RL_FLOW_CONTROL by FEATURE_DATA_RL_FLOW_CONTROL  
11/18/11   vpk        HDR SU API cleanup
09/26/11   msh        Fixed compiler warning 
08/16/11   vpk        Move hdr log packet definitions to hdrdata
07/07/11   vpk        Handle duplicate DSM pool full condition
05/12/11   op         Migrated to MSG 2.0 macros
04/08/11   ls         Remove ds707_ccm_init.h header file
02/26/11   ls         Global Variable cleanup
02/11/11   ack        Global Variable Cleanup
01/31/11   sa         Made changes for Software decoupling.
12/27/10   ms         Replaced REX premption macros with critical section for 
                      mutual exclusion.
11/05/10   ls         Avoid crash if not obtaining pkt_info_ptr from free queue
08/06/10   ls         Add featurization to avoid compiler errors
07/27/10   rp         Fixed the issue of crash due to RLP header extract fail 
06/21/10   spn        Replaced INTLOCK with REX_(ENABLE/DISABLE)_PREEMPTION
05/17/10   ls         HDR interface change for CPU flow control
05/15/10   ls         Set/Clear enable_inactivity_timer flag when inactivity timer
                      is up/down
02/10/10   spn        RX Task Optimization
03/02/10   spn        Changed hdrcp_pcp_register function call.
02/02/10   ms         Fixed Klocwork Errors.
11/10/09   as         Cleared Windows Compiler warnings for MOB builds
11/02/09   as         Fixed ARM Compiler warning - new line at end of file
10/05/09   ca         Fixed Klocwork Error.
04/09/09   ls         In hdrrlp_cta_clear_enhanced_idle_bit(), add parameter
                      enable_inactivity_timer to check before going to dormant
03/04/09   sa         AU level CMI modifications.
02/27/09   spn        Fixed Lint Error
10/07/08   cs         Compiler warning fix.
04/08/08   hs         Fixed the problem with improper logging for packet 0x12A5
03/05/08   hs         Fixed Featurization for RLP Logging. Removed IRAM features
01/31/08   pba        Removed HDRRX internal header file.
10/11/07    ac        Propagate the default CTA timer value to RLP to take care of multi-mode
                      targets
08/08/07   ac         The hdrrlp_data_activity_update is used to update the 
                      inactivity timer. This fuction is called, when RLP
					  is busy.
01/24/07   sy         Callback support for MARQ.
01/09/07   ac         Added the flush give up timer
11/28/06   spn        Updated V(R) in case where we recive a packet, where 
                      V(R) <= X < V(N), but we dont find a NAK entry.  
10/11/06   sy         Fixed a case where if the NAK removal causes V(R)=V(N)
                      then V(N) needs to be updated for any extra bytes
                      received in that received packet along with V(N).
10/05/06   spn        Changed the function definition of 
                      hdrrlp_populate_tx_data_q_level(), as it was calculating 
                      from wrong data structures, which caused NULL ptr 
                      dereferencing.
09/01/06   spn        Updated the MRLP Log Packet, to reflect the sequence 
                      length, and the RLP ID Length.
08/08/06   ksu        EMPA: RLP Tx code review comments
07/31/06   ksu/sy     Fixed the code to dump the just the duplicate bytes 
                      instead of the trashing the whole packet.
06/06/06   ksu        EMPA: RLP Rx / Sig code review comments
05/30/06   ksu        EMPA: RLP init code review comments
04/26/06   sy         Fixed  issue where the input to the macro DSRLPSEQ_PUT_HDR_SEQ()
                      is expecting a right-shifted modulus.
04/13/06   ksu        EMPA: capacity testing
02/07/06   ksu        EMPA: forward link RoHC testing
02/06/06   ksu        EMPA: reverse link RoHC testing
01/11/06   sy         Renamed hdrrlp_process_pkt_payload() to hdrrlp_process_rlp_pkt()
                      and moved the seq_num processing into this function, and
                      made it common for MPA and DPA.
12/14/05   sy         Fixed the code to take the appropriate flush timer
                      value instead of a constant.
12/12/05   vrk/squ    Reset the CTA timer if we are not transmitting Data
12/12/05   ksu/sy     Introduced a critical section between RX and TX task to 
                      prevent RX task from resetting the RLP while the
                      the TX task build the packets and waiting for a 
                      response from PCP.
12/04/05   ksu        EMPA segment sequencing: don't fragment for Format B
12/01/05   ksu        EMPA packet stream: segment sequencing (reverse link also)
11/22/05   ksu        EMPA packet stream: segment sequencing (forward link only)
11/21/05   ksu        EMPA packet stream: out of order delivery to route protocol forward
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)
11/03/05   ksu        EMPA: packet stream, octet sequencing (reverse link only)
11/28/05   ksu        CR 81596: Rev 0 data call fails all the time with dsrlpsq
                      crash during bi-directional data transfer
10/10/05   ksu/sy     Eliminate hdrmrlpi_rx_process_nak()
10/10/05   ksu/sy     Combine payload processing of hdrrlp_put_pkt_cb() and hdrmrlp_put_pkt_cb()
10/10/05   ksu/sy     Eliminate hdrmrlpi_reset_proc()
09/30/05   ksu/sy     Eliminate hdrmrlp_get_new_or_flush_data() and hdrmrlp_get_retx_data()
09/29/05   ksu/sy     MFPA: short sequence numbers work for flow with naks disabled
09/28/05   ksu/sy     HDR: use one-shot abort timer instead of 20ms ticks
09/28/05   ksu/sy     Call hdr[m]rlpi_rx_process_sig_msg() directly from hdr[m]rlp_msg_cb().
09/27/05   ksu/sy     Only age nak lists with correct RLP version.
09/01/05   ksu/sy     Call hdrrlpi_reset_proc() instead of dsrlpi_reset() upon
                      NAK list exhaustion
09/01/05   ksu/sy     RLP reset should clear flush timer.
09/22/05   vas        Fixed to call hdrmrlpsched_get_new_data_cnt() only for
                      MRLP Rev RLP, not for default RLP.
09/14/05   sy         Updated code to use hdrmrlpsched_get_new_data_cnt()
                      to get the watermark count for a reverse RLP.
05/17/05   vas        Changes to support Rev A PCP interface
02/22/05   gr         Added the functionality to handle CTA(inactivity) timer
01/31/05   sy         Moved HDRRLP_NAK_ABORT_TIMER_VALUE to hdrrlp.h
10/19/04   mpa        Replaced hdrscp_session_is_open with hdrcp_smp_session_is_open.
09/20/04   mpa        Tear down connection through pac/almp on RLP reset.
09/09/04   vas        Preliminary QOS support.
11/12/03   igt        Added capability to report received bytes every second
10/21/03   vas        Removed spurious message
10/20/03   vas        Fixed macro to get stream header for logging partial 
                      RLP tx packets
10/13/03   vas        Removed hdrrlp_resume. Combined some message macros.
09/23/03   vas        Pass correct stream ids when deregistering PCP cbacks
09/18/03   vas        Registering/deregistering of RLP callbacks with PCP 
                      happen as part of the HDR RLP initialization/cleanup
09/10/03   mpa        Converted F3 messages to use new MSG2.0 HDRDATA SSID
08/28/03   vas        Code cleanup.
07/23/03   mpa        Merged back in MSM6500 baseline
07/10/03   vas        Code cleanup. AN & SN RLP have fixed sr id values
06/26/03   kvd        included header file ts.h
06/18/03   vas        Added code to end data call if the reset timeout occurs
02/12/03   kvd        Added debug msg after register_stream_callback in
                      hdrrlp_init since the msg is removed from 
                      register_stream_callback
10/22/02   js         Modified F3 messages to distinguish between AN and SN 
                      RLP streams. 
09/19/02   vr         Send updated RLP stats to diag when RLP is torn down
08/29/02   vr         Fixed race condition between RX and TX task during
                      RLP reset
08/07/02   vr         Fixed not naking complete hole issue for HDR
07/25/02   vas        Update V(R) to new value for the case that a packet is 
                      received which contains data such that it fills a hole,
                      and also contains new data.
07/18/02   vas        Moved dsrlpi_reset to be executed in the Rx task instead
                      of Tx task.
                      When Failsafe RLP reset timer expires, give command to 
                      CM to end the call instead of directly notifying DS that
                      call ended.
06/12/02   vas        Print error & discard received RLP packet if it contains
                      only sequence number with no payload.
05/14/02   hcg        Relocated code to IRAM on a functional basis, so that the
                      hdrrlp.o file could be removed from the scatter load file.
05/09/02   hcg        Modified RLP reset condition check to use updated macro,
                      to account for cases in which RLP is to be reset at the
                      next HDRTX frame building.
04/01/02   vr         Added check for valid rscb_index(sr_id) in hdrrlp_msg_cb
03/18/02   vas        Changes to ensure no RLP packets /signaling messages are
                      processed before RLP initialization has been completed.
03/01/02   hcg        Added support for multiple simultaneous RLPs.
02/15/02   hcg        Added definition of reset_timer to prevent potential 
                      reset state deadlock problem.  Added definition and
                      callback function.
02/14/02   hcg        Removed manual flush timer processing.  Flush timer is 
                      now part of the RLP control block and is updated as a 
                      Rex timer.
02/11/02   ss         Modified parameters of hdrl1_fmac_enable_flow_control().
01/15/02   hcg        Update NAK abort timer value.
12/09/01   vr         Set the PS PPP queue limit based on relay/net model call
                      Also, ensure that it is only used in HDR system
12/09/01   vr         Flow control HDR FL is PS PPP queue too large.
11/30/01   hcg        Fixed RLP seq MSG macros to display seq #'s correctly.
11/29/01   rao        Adding Stream Id info to Partial RLP Log packets, also
                      added HDRRLP_GET_STREAM_ID macro.
11/15/01   rao        Fixed Partial RLP ( Flush packet ) logging problem.
11/15/01   rao        Fixed Partial RLP ( Re-Transmit ) logging problem.
11/09/01   rao        Added DSM REUSE Optimization support.
11/05/01   vas        Changed hdrrlp_clear_stats() to clear stats for fixed
                      SR_ID_INDEX. Removed check to see if RLP initialized or
                      is for HDR. This allows clearing even while dormant.
10/29/01   mpa        Modified hdrrlp_clear_stats() so it would also log the
                      packet after reset.
10/17/01   vas        Code cleanup. (Code review changes)
10/05/01   vas        Merge with 1.17.1.2. Fixed to deregister service for 
                      data optimized path when RLP is cleaned up. In 
                      hdrrlp_msg_cb(), used correct rlp_id index into rscb
                      array.  Also, fixed logic to prevent uninitialized RLP
                      control block from being used.
09/25/01   hcg        Updated max sixe of TXQ to match Tx frame building 
                      interval.
09/20/01   rao        Added Partial RLP frame logging support
09/19/01   hcg        Added support for test application.
09/19/01   vas        Added hdrrlp_clear_stats()
09/16/01   vas        Changes to support SR ID changes in dsrlp which 
                      differentiate between SR_ID and RLP index
09/14/01   hcg        Prevent RLP from transmitting or receiving data if the
                      flow for the stream is disabled.
09/13/01   hcg        Fixed problem to prevent corrupt RLP Tx packet that 
                      could cause derefernce of NULL ptr causing exception.
                      Cleaned up some messages.
09/13/01   vas        Merge with rev 1.4.1.0 Updated to support data path 
                      optimizations.  Added re-registration function for 
                      receive queue and Rx callback fcn.  Also added calls to
                      dsrlprsq_remove_chain(). Added hdrrlp_set_ds_curr_sr_id
                      to change the current sr_id in dsmgr
09/11/01   rao        Updated flush timer value time increment.
08/28/01   vas        Added hdrrlp_reg_srvc()
08/17/01   vas        Added support for having multiple instances of RLP, for
                      any of the streams. rlp_age_nak_list() & queue_msg() do 
                      processing for all initialized RLPs. Added mechanism 
                      for registering callbacks with PCP for receiving /
                      sending data on the stream. Added hdrrlp_init().Removed
                      check for FCP pkts as those are directly routed thru PAC.
                      Removed deprecated functions flow_enable(), ind_cb(),
                      flow_disable(), data_read_cb(). 
08/15/01   hcg        Added code to protect case where RLP data is received
                      but RLP is not yet established.  Also, route FCP
                      messages directly to FCP, rather than through RLP.
04/02/01   hcg        Removed call to dsrlprsq_insert_sqntl().  Removed 
                      FEATURE_DS_HDRRLP.
03/20/01   hcg        Removed RLP memory management since the HDR forward 
                      link is now flow controlled.
03/12/01   hcg        Added memory management scheme when running low on 
                      DSM items.  Added duplicate packet statistics count.  
                      Removed RLP reset statistics count since it is now
                      performed in dsrlpi_reset().
02/17/00   hcg        Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "data_msg.h"
#include "hdrrlp_api.h"
#include "dsutil.h"
#include <stringl/stringl.h>

#if defined(FEATURE_HDR)

#include "comdef.h"
#include "rex.h"
#include "bit.h"
#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "hdrrlpnak.h"
#include "hdrrlprsq.h"
#include "hdrrlpseq.h"
#include "ds707_cta_rlp.h"
#include "hdrcom_api.h"
#include "hdrpac.h"
#include "amssassert.h"
#include "memory.h"
#include "err.h"
#include "time_svc.h"
#include "time_tod.h"
#include "cm.h"
#include "hdrds.h"

#include "hdrpac_mdm.h"
#include "hdrrlptxq.h"
#include "hdrrlpsort.h"
#include "hdrrlpqn.h"
#include "hdrrlplog.h"
#include "qw.h"
#include "hdrrlp_util.h"
#include "ds707_pkt_mgri.h"

#ifdef FEATURE_USB
#include "usbdc.h"
#endif

#if defined(FEATURE_HDR_DSMREUSE_OPT)
#include "hdrds.h"
#endif

#include "ds707_pkt_mgr.h"
#include "hdrrlptxph.h"
#include "hdrrlptxsh.h"
#include "hdrrlpmarq.h"
#include "hdrmrlp.h"
#include "hdrmrlpi.h"
#include "hdrrlprtx.h"
#ifdef FEATURE_HDR_QOS
#include "hdrmrlpsched.h"
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_DATA_RL_FLOW_CONTROL
#include "ds707_ccmi.h"
#endif

#include "hdrrlp.h"
#include "hdrl1_api.h"
#include "hdrcp_api.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/* Enhanced dorm bitmask every bit represent on RLP 
   0 - 31 Forward RLP
   32- 63 Reverse RLP */
static qword hdrrlp_enhanced_dormant_bitmap;

rex_crit_sect_type hdrrlp_enhanced_dormant_bitmap_crit_sect;
rex_crit_sect_type dsrlp_cfg_activity_crit_sect;

static dsat_num_item_type hdrrlp_default_cta_val;

#define HDRRLP_STREAM1_HMP_HDR  HDRHAI_N_APP1_TYPE 
#define HDRRLP_STREAM2_HMP_HDR  HDRHAI_N_APP2_TYPE 
#define HDRRLP_STREAM3_HMP_HDR  HDRHAI_N_APP3_TYPE 

/*---------------------------------------------------------------------------
  Signaling message priority.
---------------------------------------------------------------------------*/
#define  HDRRLP_SIG_MSG_PRIORITY            50

/*---------------------------------------------------------------------------
Since we are using a global pool of TXQ items, the TXQ size is equal to
the number of items in the pool.
---------------------------------------------------------------------------*/
#define  HDRRLP_TXQ_SIZE                    HDRRLPTXQ_TOTAL_NUM_ITEMS

/*---------------------------------------------------------------------------
  Number of rounds of NAKs.  For HDR, there is only 1 round of NAKs and 1 
  NAK per round.
---------------------------------------------------------------------------*/
#define  HDRRLP_MAX_NAK_ROUNDS              1

/*---------------------------------------------------------------------------
  For creating RLP signaling messages, offset to leave for header 
  information.  The value is in bytes.
---------------------------------------------------------------------------*/
#define HDRRLP_SIGHDR_OFFSET 12

/*---------------------------------------------------------------------------
  Useful Macros & defines for HDRRLP Partial logging
---------------------------------------------------------------------------*/
#if defined(FEATURE_HDRRLP_LOGGING)
static log_hdrrlp_frames_rx_type hdrrlp_fr_rx_log ALIGN(4);  /*  RLP rx frames logging  */
static log_hdrrlp_frames_tx_type hdrrlp_fr_tx_log ALIGN(4);  /*  RLP tx frames logging  */

#define RXC_HDRRLP_FRAMES_OFFSET \
  (hdrrlp_fr_rx_log.hdr.len - FPOS(log_hdrrlp_frames_rx_type, hdrrlp_log_buf))

#define TXC_HDRRLP_FRAMES_OFFSET \
  (hdrrlp_fr_tx_log.hdr.len - FPOS(log_hdrrlp_frames_tx_type, hdrrlp_log_buf))

#endif /*  (FEATURE_HDRRLP_LOGGING) */

#ifdef FEATURE_DATA_RL_FLOW_CONTROL
static rex_timer_type hdrrlp_rlfc_delay_timer;
#endif

#define HDRRLP_SEQ_MASK       0x003FFFFF

/*---------------------------------------------------------------------------
  Control structure to enable / disable extreme verbose RLP debug f3 messages
---------------------------------------------------------------------------*/
static hdrrlp_debug_ctl hdrrlp_debug = {0,0};

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        MACRO DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

MACRO         HDRRLP_SIG_MSGS_TO_SEND

DESCRIPTION   This macro determines if there are any signaling messages to
              send, based on the internal state variables structure.

PARAMETERS    RLP state variable structure.

DEPENDENCIES  None.

RETURN VALUE  Boolean value indicating if there are signaling messages to 
              send.

SIDE EFFECTS  None.
===========================================================================*/
#define HDRRLP_SIG_MSGS_TO_SEND(sp)                                         \
   ( (sp->rst_to_send||sp->rst_ack_to_send||sp->naks_to_send)?TRUE:FALSE)


#if defined(FEATURE_HDRRLP_LOGGING)

/*===========================================================================
FUNCTION      GET_HDRRLP_FR_RX_LOG_PTR

DESCRIPTION   Accessor function for hdrrlp_fr_rx_log
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_fr_rx_log

SIDE EFFECTS  None
===========================================================================*/
log_hdrrlp_frames_rx_type* get_hdrrlp_fr_rx_log_ptr 
(
  void
)
{
  return &hdrrlp_fr_rx_log;
} /* get_hdrrlp_fr_rx_log_ptr */

/*===========================================================================
FUNCTION      GET_HDRRLP_FR_TX_LOG_PTR

DESCRIPTION   Accessor function for hdrrlp_fr_tx_log
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_fr_tx_log

SIDE EFFECTS  None
===========================================================================*/
log_hdrrlp_frames_tx_type* get_hdrrlp_fr_tx_log_ptr 
(
  void
)
{
  return &hdrrlp_fr_tx_log;
} /* get_hdrrlp_fr_tx_log_ptr */

/*===========================================================================
FUNCTION      HDRRLP_GET_STREAM_ID

DESCRIPTION   This function gets the streamID given RLP ID and VERSION
              
DEPENDENCIES  None

RETURN VALUE  Stream header (2 MSB of a byte)

SIDE EFFECTS  None
===========================================================================*/
uint8 hdrrlp_get_stream_id(uint8 rlp_id, dsrlp_ver_enum_type ver)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* RLP-specific control block */
  uint8                     rscb_index;      /* index in the rscb array where
                                                this record will be stored */
  uint8   stream_header;       /* Stream for the RLP         */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Select the index in dsrlpi_rscb_array where the record for this sr_id
  will be stored. 
  -------------------------------------------------------------------------*/
  if (ver == DSRLP_VER_HDR)
  {
    rscb_index = dsrlp_get_rlp_rscb_index( rlp_id, DSRLP_VER_HDR );
  }
  else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
  {
    rscb_index = dsrlp_get_rlp_rscb_index( rlp_id, DSRLP_VER_HDR_MULTIFLOW_REVERSE );
  }
  else
  {
    return 0;
  }

  /* Return 0 is the rscb_index is invalid */
  if(rscb_index == DSRLPI_INVALID_RSCB_INDEX)
  {
    return 0;
  }

  rscb_ptr   = &(dsrlpi_rscb_array[rscb_index]);

  stream_header = (int)(rscb_ptr->cfg).stream_id;

  stream_header = stream_header<< HDRRLP_STREAM_ID_SHIFT_BITS;
  stream_header &= HDRRLP_STREAM_ID_MASK;
  return stream_header;
}


/*===========================================================================
FUNCTION      HDRRLP_TX_RLP_FRAMES_LOGGING

DESCRIPTION   Logs the tx frame based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  rev_pkt:    data packet built for TX
  
DEPENDENCIES  None.
  None.
RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrrlp_tx_rlp_frames_logging
(
  hdrrlp_tx_packet_info_type* tx_packet_info_ptr
)
{
  uint8   stream_header; /* Stream Id info                */
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr = tx_packet_info_ptr->rrcb_ptr;
  dsm_item_type               *rev_pkt = tx_packet_info_ptr->rev_pkt_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((rrcb_ptr == NULL) || (rrcb_ptr->cfg_ptr == NULL) || (rev_pkt == NULL))
  {
    return;
  }

  switch (rrcb_ptr->cfg_ptr->ver)
  {
    case DSRLP_VER_HDR:
      if (log_status(hdrrlp_fr_tx_log.hdr.code))
      {
        /*-------------------------------------------------------------------------
        Get the Stream Id info from RLP session Control Block sr_id
        this updates the uppper 2 bits with the Steam Id info. 
        -------------------------------------------------------------------------*/
        stream_header = hdrrlp_get_stream_id(rrcb_ptr->cfg_ptr->sr_id, DSRLP_VER_HDR);
        
        if(stream_header == 0)
        {
            DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                            "RLP Tx[%d,%d] V[%d,%d]hdrrlp_get_stream_id() returned error ", 
                            rrcb_ptr->cfg_ptr->sr_id,
                            "AB"[(rrcb_ptr->route)],
                            rrcb_ptr->cfg_ptr->ver,
                            rrcb_ptr->cfg_ptr->enhanced);
           return;
        }

        /*------------------------------------------------------------------
        Clear the first 2 bits of first byte
        ------------------------------------------------------------------*/
        *((uint8*) rev_pkt->data_ptr) &= 0x3F;

        /*------------------------------------------------------------------
        Clear the last 6 bits of stream_header (this is for sanity)
        ------------------------------------------------------------------*/
        stream_header &= 0xC0;

        /*------------------------------------------------------------------
        Add Stream Id info to RLP header.
        ------------------------------------------------------------------*/
        *((uint8*) rev_pkt->data_ptr) |= stream_header;
        hdrrlp_log_tx_rlp_frames(rev_pkt); 
      }
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      if ( rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA && 
		    (log_status(LOG_HDR_ENHANCED_MRLP_TX_PKTS_C)) )
      {
         /*------------------------------------------------------------------
          Get the Stream Id info from RLP session Control Block sr_id
          this updates the uppper 2 bits with the Steam Id info. 
          -------------------------------------------------------------------*/
        stream_header = hdrrlp_get_stream_id(rrcb_ptr->cfg_ptr->sr_id, 
                                             DSRLP_VER_HDR_MULTIFLOW_REVERSE);          
        if(stream_header == 0)
        {
          DATA_RLP_MSG4( MSG_LEGACY_ERROR,
                          "RLP[%d,%d] V[%d,%d] hdrrlp_get_stream_id returned error",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced );
          return;
        }

        /*------------------------------------------------------------------
        Clear the first 2 bits of first byte
        ------------------------------------------------------------------*/
        *((uint8*) rev_pkt->data_ptr) &= 0x3F;

        /*------------------------------------------------------------------
        Clear the last 6 bits of stream_header (this is for sanity)
        ------------------------------------------------------------------*/
        stream_header &= 0xC0;

        /*------------------------------------------------------------------
        Add Stream Id info to RLP header.
        ------------------------------------------------------------------*/
        *((uint8*) rev_pkt->data_ptr) |= stream_header;

        hdrrlplog_enhanced_log_tx_rlp_frames(rev_pkt, rrcb_ptr, rrcb_ptr->route);  
      }
      else if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA && 
             (log_status(LOG_HDR_MULTILINK_MRLP_TX_PKTS_C)) )
      {
      /*------------------------------------------------------------------
          Get the Stream Id info from RLP session Control Block sr_id
          this updates the uppper 2 bits with the Steam Id info. 
          -------------------------------------------------------------------*/
        stream_header = hdrrlp_get_stream_id(rrcb_ptr->cfg_ptr->sr_id, 
                                             DSRLP_VER_HDR_MULTIFLOW_REVERSE);          
        if(stream_header == 0)
        {
          DATA_RLP_MSG4( MSG_LEGACY_ERROR,
                          "RLP[%d,%d] V[%d,%d] hdrrlp_get_stream_id returned error",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced );
          return;
        }

        /*------------------------------------------------------------------
        Clear the first 2 bits of first byte
        ------------------------------------------------------------------*/
        *((uint8*) rev_pkt->data_ptr) &= 0x3F;

        /*------------------------------------------------------------------
        Clear the last 6 bits of stream_header (this is for sanity)
        ------------------------------------------------------------------*/
        stream_header &= 0xC0;

        /*------------------------------------------------------------------
        Add Stream Id info to RLP header.
        ------------------------------------------------------------------*/
        *((uint8*) rev_pkt->data_ptr) |= stream_header;

        hdrrlplog_multilink_log_tx_rlp_frames(tx_packet_info_ptr);  
      }
      else if(log_status(LOG_HDR_MRLP_TX_PKTS_C))
      {
		/*-------------------------------------------------------------------------
        Get the Stream Id info from RLP session Control Block sr_id
        this updates the uppper 2 bits with the Steam Id info. 
        -------------------------------------------------------------------------*/
        stream_header = hdrrlp_get_stream_id(rrcb_ptr->cfg_ptr->sr_id, 
                                             DSRLP_VER_HDR_MULTIFLOW_REVERSE);          
        if(stream_header == 0)
        {
          DATA_RLP_MSG4( MSG_LEGACY_ERROR,
                          "RLP[%d,%d] V[%d,%d] hdrrlp_get_stream_id returned error",
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced );
          return;
        }

        /*------------------------------------------------------------------
        Clear the first 2 bits of first byte
        ------------------------------------------------------------------*/
        *((uint8*) rev_pkt->data_ptr) &= 0x3F;

        /*------------------------------------------------------------------
        Clear the last 6 bits of stream_header (this is for sanity)
        ------------------------------------------------------------------*/
        stream_header &= 0xC0;

        /*------------------------------------------------------------------
        Add Stream Id info to RLP header.
        ------------------------------------------------------------------*/
        *((uint8*) rev_pkt->data_ptr) |= stream_header;

        hdrrlplog_log_tx_rlp_frames(rev_pkt, rrcb_ptr);  
      }
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)rrcb_ptr->cfg_ptr->ver, 
                              rrcb_ptr->cfg_ptr->sr_id, 0);
      break;
  }
}

#endif /* defined(FEATURE_HDRRLP_LOGGING) */


/*===========================================================================
               G L O B A L  V A R I A B L E S
===========================================================================*/

/*---------------------------------------------------------------------------
  All the constants for HDR RLP. This includes function pointers, and other
  numeric constants that are version specific.  Most values are specified
  as NULL, since they do not apply to HDR.
---------------------------------------------------------------------------*/
static dsrlpi_ver_const_type hdrrlp_constants =
{
  NULL,                                      /* Ptr to function table      */
  0,                                         /* Max conseq. erasures       */

  HDRRLP_TXQ_SIZE,                           /* Size of TXQ                */

  HDRRLP_MAX_NAK_ROUNDS,                     /* Max fwd rounds of NAKs     */
  HDRRLP_MAX_NAK_ROUNDS,                     /* Max rev rounds of NAKs     */
  
  HDRRLP_MAX_NAK_ROUNDS,                     /* Desired fwd rounds of NAKs */
  {0, 0, 0},                                 /* Desired NAKs per round fwd */
  HDRRLP_MAX_NAK_ROUNDS,                     /* Desired rev rounds of NAKs */
  {1, 0 ,0},                                 /* Desired NAKs per round rev.*/

  TRUE                                       /* segmentation allowed       */
};


#if defined(FEATURE_HDRRLP_LOGGING)
static uint8  hdrrlp_log_tx_frame_count = 0;
#endif

/*---------------------------------------------------------------------------
  Pointer to queue of free dsrlpi_pkt_info structs.
---------------------------------------------------------------------------*/
static q_type   hdrrlp_pkt_info_free_q;  

/*---------------------------------------------------------------------------
  This pool of nak entries will be maintained in a free queue.  These
  entries are shared across all HDR RLP instances on the mobile.
---------------------------------------------------------------------------*/
static dsrlpi_pkt_info_type   hdrrlp_pktinfo_pool[HDRRLP_MAX_PKT_INFO_ENTRIES];

/*---------------------------------------------------------------------------
  Variables to keep track of what RLPs received data in the last slot
---------------------------------------------------------------------------*/
static uint16 hdrrlp_num_rrcbs_used_in_slot = 0;
static dsrlpi_rlp_ctl_blk_type* hdrrlp_rrcbs_used_in_slot[DSRSP_NUM_ROUTES];

/*===========================================================================
FUNCTION HDRRLP_GET_HEADER_SIZE

DESCRIPTION
  Computes RLP header size based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The computed header size

SIDE EFFECTS
  None.
===========================================================================*/
int hdrrlp_get_header_size
(
  const dsrlpi_cfg_info_type* cfg_ptr
)
{
  int hdr_size = 0;

  switch (cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      hdr_size = HDRRLP_HEADER_SIZE;
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
    
      if (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) 
      {
        hdr_size = HDRMMRLP_HEADER_SIZE(cfg_ptr);
      }
      else if (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA) 
      {
        hdr_size = HDRERLP_HEADER_SIZE(cfg_ptr);
      }
      else 
      {
        hdr_size = (int)HDRMRLP_HEADER_SIZE(cfg_ptr);
      }
      break;
      case DSRLP_VER_HDR_MULTIFLOW_REVERSE:      
      if ( (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)  ||
          (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA) )
      {
        hdr_size = HDRERLP_HEADER_SIZE(cfg_ptr);
      }
      else 
      {
        hdr_size = HDRMRLP_HEADER_SIZE(cfg_ptr);
      }
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)cfg_ptr->ver, cfg_ptr->sr_id, 0);
      break;
  }

  return hdr_size;
}


/*===========================================================================
FUNCTION      HDRRLP_RESET_TIMEOUT

DESCRIPTION   Called by RLP when the RLP reset failsafe timer expires.  
              Should bring down the traffic channel.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void hdrrlp_reset_timeout(void)
{
  DATA_RLP_MSG0 (MSG_LEGACY_HIGH, "RLP reset timed out, Ending Call");
  hdrpac_close_connection();
}

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

#if defined(FEATURE_HDRRLP_LOGGING)

/*===========================================================================

FUNCTION HDRRLP_RX_SEND_FRAMES_LOG

DESCRIPTION
  Terminate and send the accumulated HDR RLP partial rx frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
void hdrrlp_rx_send_frames_log(void )
{             
  
  if (hdrrlp_fr_rx_log.hdr.len > FPOS(log_hdrrlp_frames_rx_type,
                                                hdrrlp_log_buf))
  {
    /* submit the log buffer to diag */
    (void) log_submit((log_type *) &hdrrlp_fr_rx_log);   
    /* Re-initialize counters */
    hdrrlp_fr_rx_log.hdr.len = FPOS(log_hdrrlp_frames_rx_type,
                                              hdrrlp_log_buf);
    hdrrlp_fr_rx_log.count   = 0;
  }


} /* hdrrlp_rx_send_frames_log */


/*===========================================================================

FUNCTION HDRRLP_TX_SEND_FRAMES_LOG

DESCRIPTION
  Terminate and send the accumulated RLP partial  tx frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RLP partial tx frames log may be sent.

===========================================================================*/
void hdrrlp_tx_send_frames_log(void)
{

  if (hdrrlp_fr_tx_log.hdr.len  > FPOS(log_hdrrlp_frames_tx_type,
                                                 hdrrlp_log_buf))
  {
    /* submit the log buffer to diag */
    (void) log_submit((log_type *) &hdrrlp_fr_tx_log);
    /* Re-initialize counters */
    hdrrlp_fr_tx_log.hdr.len = FPOS(log_hdrrlp_frames_tx_type,
                                              hdrrlp_log_buf);
    hdrrlp_fr_tx_log.count   = 0;
   
  }

} /* txc_send_hdrrlp_frames_log */

#endif  /* FEATURE_HDRRLP_LOGGING */

#ifdef FEATURE_DATA_RL_FLOW_CONTROL
/*===========================================================================
FUNCTION      GET_HDRRLP_RLFC_DELAY_TIMER_PTR

DESCRIPTION   Accessor function for hdrrlp_rlfc_delay_timer
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_rlfc_delay_timer

SIDE EFFECTS  None
===========================================================================*/
rex_timer_type* get_hdrrlp_rlfc_delay_timer_ptr 
(
  void
)
{
  return &hdrrlp_rlfc_delay_timer;
} /* get_hdrrlp_rlfc_delay_timer_ptr */
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */

/*===========================================================================
FUNCTION      GET_HDRRLP_CONSTANTS_PTR

DESCRIPTION   Accessor function for hdrrlp_constants
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_constants

SIDE EFFECTS  None
===========================================================================*/
dsrlpi_ver_const_type* get_hdrrlp_constants_ptr 
(
  void
)
{
  return &hdrrlp_constants;
} /* get_hdrrlp_constants_ptr */

/*===========================================================================
FUNCTION      GET_HDRRLP_DEBUG_PTR

DESCRIPTION   Accessor function for hdrrlp_debug
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_debug

SIDE EFFECTS  None
===========================================================================*/
hdrrlp_debug_ctl* get_hdrrlp_debug_ptr 
(
  void
)
{
  return &hdrrlp_debug;
} /* get_hdrrlp_debug_ptr */

/*===========================================================================
FUNCTION      GET_HDRRLP_PKT_INFO_FREE_Q_PTR

DESCRIPTION   Accessor function for hdrrlp_pkt_info_free_q
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_pkt_info_free_q

SIDE EFFECTS  None
===========================================================================*/
q_type* get_hdrrlp_pkt_info_free_q_ptr 
(
  void
)
{
  return &hdrrlp_pkt_info_free_q;
} /* get_hdrrlp_pkt_info_free_q_ptr */

/*===========================================================================
FUNCTION      GET_HDRRLP_DEFAULT_CTA_VAL_PTR

DESCRIPTION   Accessor function for hdrrlp_default_cta_val
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_default_cta_val

SIDE EFFECTS  None
===========================================================================*/
dsat_num_item_type* get_hdrrlp_default_cta_val_ptr 
(
  void
)
{
  return &hdrrlp_default_cta_val;
} /* get_hdrrlp_default_cta_val_ptr */

/*===========================================================================
FUNCTION      GET_HDRRLP_ENHANCED_DORMANT_BITMAP_PTR

DESCRIPTION   Accessor function for hdrrlp_enhanced_dormant_bitmap
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrrlp_enhanced_dormant_bitmap

SIDE EFFECTS  None
===========================================================================*/
qword* get_hdrrlp_enhanced_dormant_bitmap_ptr 
(
  void
)
{
  return &hdrrlp_enhanced_dormant_bitmap;
} /* get_hdrrlp_enhanced_dormant_bitmap_ptr */

/*===========================================================================
FUNCTION HDRRLP_RESET_LOCAL_STATE

DESCRIPTION
  Resets the local state variables for a given combo_index
  
PARAMETERS
  state_ptr:  State pointer for a particular rlp
  combo_index: Index to the combo that needs to be cleared up.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrlp_reset_new_data_local_state
(
 dsrlpi_state_info_type  *state_ptr,
 uint8 combo_index
)
{  
  ASSERT( state_ptr );
  //TODO: Comment for tx_flush_status....
  state_ptr->local_state[combo_index].new_handles_built_count = 0;
  state_ptr->local_state[combo_index].next_seq_num = 0;
  state_ptr->local_state[combo_index].seek_offset= 0;
  state_ptr->local_state[combo_index].new_handle_status_rcvd_count = 0;
} /* hdrrlp_reset_local_state() */

/*===========================================================================

FUNCTION HDRRLP_GET_PENDING_TX_DATA_QUEUE_SIZE

DESCRIPTION
  This function is called to determine the number of bytes pending transmission 
  by this RLP. 
  
  The size returned is the total number of bytes in the retransmission queue,
  plus the new data pending transmission. The new data pending transmission
  includes data in the tx watermark as well as data that has been  dequeued
  from the watermark, but not yet transmitted.
  
  This function is also used as a callback by hdrpac (transmit)

DEPENDENCIES
  This function shall be called once, per subframe and the values returned
  are valid across all combos.

RETURN VALUE
  Number of bytes pending transmission for this RLP.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hdrrlp_get_pending_tx_data_queue_size
(
  uint32 *num_pkts,            /* Number of separate RLP packets           */
  void *pkt_rrcb_ptr           /* RLP-specific ctl block                   */
  
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;   /* RLP-specific ctl block          */
  dsrlpi_state_info_type   *state_ptr;  /* local state ptr                 */

  uint32 num_tx_bytes = 0; /* Number of bytes pending transmission         */
  uint32 num_bytes = 0;                         /* Number of bytes         */
  uint32 num_new_data_pkts = 0; /* Number of New or Flush data packets */
  uint32 num_retx_data_pkts = 0;  /* Number of retransmit data packets */
  dsm_item_type *item_ptr;
  uint32 num_retx_bytes = 0;
  hdrmrlp_app_ctl_blk_type *hdrmrlp_sn_acb_ptr = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Make sure the control block pointer is not NULL
  -------------------------------------------------------------------------*/
  ASSERT( pkt_rrcb_ptr != NULL );

  hdrmrlp_sn_acb_ptr = get_hdrmrlp_sn_acb_ptr();

  /*-------------------------------------------------------------------------
    Get the session control block and the fcn_ptr_table.
  -------------------------------------------------------------------------*/
  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type *) pkt_rrcb_ptr;
  state_ptr     = &rrcb_ptr->state;

  /*-------------------------------------------------------------------------
    If flow is disabled for this RLP instance, it cannot transmit any data.
    Return  immediately and indicate zero-length packet, instructing the 
    PCP layer to no longer query RLP for data.
    -----------------------------------------------------------------------*/
  if (hdrcp_fcp_stream_is_open(rrcb_ptr->cfg_ptr->stream_id) == FALSE) 
  {
    *num_pkts = 0;
    return (0);
  }

  /*-------------------------------------------------------------------------
    If RLP is not established, then we cannot send any packets.  Return
    immediately and indicate zero-length packet, instructing the PCP layer
    to no longer query RLP for data.
    -----------------------------------------------------------------------*/
  if ((state_ptr->state != DSRLPI_ESTABLISHED_STATE) ||
      (state_ptr->reset_active == TRUE))
  {
    DATA_RLP_MSG4 (MSG_LEGACY_MED, 
                    "RLP[%d,%d] V[%d,%d] RLP not established for xmit data.", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced);
    *num_pkts = 0;
    return (0);
  }

  rex_enter_crit_sect(&state_ptr->data_path_crit_sect);
  /*-------------------------------------------------------------------------
  Add the number of bytes that have been de-qued from the tx-watermark,
  but not yet transmitted.
  -----------------------------------------------------------------------*/
  if ((item_ptr = (dsm_item_type *)q_check(&rrcb_ptr->state.tx_item_q)) != NULL)
  {
    num_tx_bytes += dsm_length_packet(item_ptr);
    num_new_data_pkts++;

    while ((item_ptr = (dsm_item_type *)q_next(&rrcb_ptr->state.tx_item_q, &item_ptr->link)) != NULL)
    {
      num_tx_bytes += dsm_length_packet(item_ptr);

      /* EMPA/MMPA packet stream: count Route Protocol packets */
      if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA ||
          rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
          rrcb_ptr->cfg_ptr->packet_stream)
      {
        num_new_data_pkts++;
      }
    }

    /* Subtract any leading packet fragments that have already been transmitted */
    ASSERT(num_tx_bytes > state_ptr->seek_offset);
    num_tx_bytes -= state_ptr->seek_offset;
  }

  /*-------------------------------------------------------------------------
  Add the total number of bytes in the transmit watermark.
  -----------------------------------------------------------------------*/
  num_bytes = hdrrlpi_get_new_data_count(rrcb_ptr);
  if (num_bytes > 0) 
  {
    num_tx_bytes += num_bytes;

    /* EMPA/MMPA packet stream: count Route Protocol packets */
    if ((num_new_data_pkts == 0) ||
        ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA ||
         rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
         rrcb_ptr->cfg_ptr->packet_stream))
    {
      num_new_data_pkts++;
    }
  }

  /*-------------------------------------------------------------------------
  If flush timer expired & there is no new data, then need to send the flush
  packet (last data unit)
  -----------------------------------------------------------------------*/
  if( num_tx_bytes == 0  && state_ptr->flush_timer_expired == TRUE)
  {
    if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA ||
        rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
    {
		  if (state_ptr->flush_giveup_timer_expired == FALSE)
		  {
			  if (rrcb_ptr->cfg_ptr->segment_sequencing)
			  {
				  /* EMPA/MMPA segment sequencing: flush the entire last packet */ 
          /* (one data unit may be multiple octets) */
				  num_tx_bytes += hdrrlptxq_access_last_frame(rrcb_ptr, NULL);
			  }
			  else
			  {
				  num_tx_bytes++;
			  }
			  num_new_data_pkts++;
		  }	
	  }
	  else
	  {
		  num_tx_bytes++;
		  num_new_data_pkts++;
	  }
  }
  
  /*-------------------------------------------------------------------------
  If the number of bytes at this point is non-zero, it means that we need to
  transmit either some new data or some flush data. Add the appropriate RLP
  header size to the number of bytes being transmitted.
  -----------------------------------------------------------------------*/
  if( num_tx_bytes != 0 )
  {
    num_tx_bytes += num_new_data_pkts * hdrrlp_get_header_size(rrcb_ptr->cfg_ptr);
  }

  /*----------------------------------------------------------------------
    Now determine number of bytes queued up for re-transmission.
    NOTE: Sending Combo Index "0", since this is called once every 
          subframe, and at that point "0" will give a correct estimate
          of retransmit data.
  -----------------------------------------------------------------------*/  
  if ( hdrrlprtx_has_rexmits(rrcb_ptr, 0) )
  {
    num_retx_bytes = hdrrlprtx_get_data_size(rrcb_ptr,&num_retx_data_pkts);
    num_tx_bytes += num_retx_bytes;
    if(hdrrlp_util_is_task_hdrtx())
    {
      hdrcp_pcp_notify_app_has_hp_data ( 
      (rrcb_ptr->cfg_ptr)->sr_id | (hdrmrlp_sn_acb_ptr->active_route<< 7) ,
                                    (int)(rrcb_ptr->cfg_ptr)->stream_id, 
                                    rrcb_ptr );
    }
    else
    {
      DATA_RLP_MSG0 (MSG_LEGACY_HIGH, "not hdrtx task, not notifying");
    }

  }

  *num_pkts = num_new_data_pkts + num_retx_data_pkts;
  rex_leave_crit_sect(&state_ptr->data_path_crit_sect);

  if ( ( num_tx_bytes > 0 ) ||
       ( num_retx_bytes > 0 ) )
  {     
    DATA_RLP_MSG7 (MSG_LEGACY_HIGH, 
                  "RLP Tx[%d,%d] V[%d,%d] Pending bytes = %d, RetxBytes = %d pkts = %d", 
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced,
                  num_tx_bytes,
                  num_retx_bytes,
                  *num_pkts);
  }                  

  return num_tx_bytes;

} /* hdrrlp_get_pending_tx_data_queue_size() */



/*===========================================================================
FUNCTION HDRRLP_POPULATE_TX_DATA_Q_LEVEL

DESCRIPTION
  This function is called from the apps side and mainly acts as a wrapper to
  hdrrlp_get_pending_tx_data_queue_size

DEPENDENCIES
  None.

RETURN VALUE
  -1: Error, 0: Success
  The function arguments hold the return values
   
SIDE EFFECTS
  None.
===========================================================================*/
int8 hdrrlp_populate_tx_data_q_level
(
 /* The following 3 fields correspond to struct ps_flow_ioctl_tx_queue_level_type */
 uint32  *total_pending_cnt,
 uint8 rlp_id,
 dsrlp_ver_enum_type ver
)
{
  uint32 num_pkts;  /* Number of separate RLP packets */
  dsrlpi_sess_ctl_blk_type *rscb_ptr;   /* RLP-specific ctl block */
  uint8 rscb_index;  /* index in the rscb array where this record will be stored */
  uint8 p;           /* index to loop through DSRSP_NUM_ROUTES */
    sint15 ps_errno;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        /*-----------------------------------------------------------------------
          This method of finding the rscb index is a fix for the QVP application.
          But this is not a long term solution and needs to be changed.
        -----------------------------------------------------------------------*/
 
          rscb_index = dsrlp_get_rlp_rscb_index ( rlp_id, ver ); 
          if ( rscb_index ==  DSRLPI_INVALID_RSCB_INDEX )
          {
            ps_errno = DS_EFAULT;
            DATA_RLP_MSG1 (MSG_LEGACY_ERROR, "Cannot get a valid RSCB Index err:%d", ps_errno );
            return -1;
          }
                  
        /*-----------------------------------------------------------------------
          Get a pointer to the HDR RLP. Then find out the watermark
          level and the current pending data for this RLP 
        -----------------------------------------------------------------------*/
        rscb_ptr = &dsrlpi_rscb_array[rscb_index];

    /* If the RLP is not established then return an error */
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      if ( (rscb_ptr->rlp_list[p].state).state == DSRLPI_ESTABLISHED_STATE )
      {
        break;
      }
    }
    if (p == DSRSP_NUM_ROUTES)
    {
      DATA_RLP_MSG0( MSG_LEGACY_ERROR, "RLP is not in Established state" );
      ps_errno = DS_EFAULT;
      return -1;
    }

  /* ssrivats: current_new_data_cnt and wm_free_cnt are filled in 
     ds707_pkt_mgr_ioclt.c - may need to revisit */

  /* Total number of bytes pending to be sent. Includes new data for the flow 
  as well as retransmission data for the RLP */

    *total_pending_cnt = 0;
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      *total_pending_cnt += hdrrlp_get_pending_tx_data_queue_size(
                                     &num_pkts,
                                     &rscb_ptr->rlp_list[p]);
    }
  return 0; //Success
} /*hdrrlp_populate_tx_data_q_level*/

/*===========================================================================

FUNCTION HDRRLP_TX_FAILED_CB

DESCRIPTION
  This function is called by the PCP layer to inform RLP about packet failure
  due to MARQ.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrlp_tx_failed_cb
(
  void *tx_data_handle,
  void *pkt_rrcb_ptr           /* RLP-specific ctl block                   */
  
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr = (dsrlpi_rlp_ctl_blk_type*)pkt_rrcb_ptr;
  dsrlpi_cfg_info_type* cfg_ptr = (dsrlpi_cfg_info_type*)rrcb_ptr->cfg_ptr;
  uint32 local_tx_data_handle = (uint32)tx_data_handle;
  uint8 marq_index = 0;
  /*--------------------------------------------------------------------------*/


  DATA_RLP_MSG4 (MSG_LEGACY_MED, 
                  "RLP Tx[%d,%d] V[%d,%d] MARQ happened", 
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced );
  
  rex_enter_crit_sect(&cfg_ptr->crit_sect);
  cfg_ptr->dropped_marq_packet= TRUE;
  rex_leave_crit_sect(&cfg_ptr->crit_sect);

  if ( cfg_ptr->phys_link_nak_enabled_rev == FALSE )
  {
    DATA_RLP_MSG0( MSG_LEGACY_FATAL,
                  "Received MARQ indication when PHY layer NAK is disabled " );
    return;                  
  }

  hdrrlptxsh_parse_setid((uint32)tx_data_handle, NULL, NULL, NULL, &marq_index );
  
  hdrrlpmarq_handle_transmission_fail(pkt_rrcb_ptr, marq_index, local_tx_data_handle );

  return;

}

/*===========================================================================

FUNCTION HDRRLP_TX_STATUS_CB

DESCRIPTION
  This function is called by the PCP layer to inform RLP about the status of
  transmission of a packet.
  
  Currently since this code is expected to run over Rev 0 RTCMAC, this function
  is only called with status = TRUE, implying data was transmitted. The code
  ASSERTs this.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrlp_tx_status_cb
(
  uint8 combo, 
  hdrcp_pcp_carrier_tx_outcome_type tx_data_handle[],
  uint8 num_sets,
  void *pkt_rrcb_ptr  
)
{ 
  rex_enter_crit_sect(&(((dsrlpi_rlp_ctl_blk_type*)pkt_rrcb_ptr)->state.data_path_crit_sect));
  hdrrlptxsh_process_status( pkt_rrcb_ptr, combo, tx_data_handle, num_sets );
  rex_leave_crit_sect(&(((dsrlpi_rlp_ctl_blk_type*)pkt_rrcb_ptr)->state.data_path_crit_sect));

} 

/*===========================================================================

FUNCTION HDRRLP_GET_RETX_DATA

DESCRIPTION
  This function is called locally for the PCP layer in order to get 
  Re-transmit data. The size of the RLP packet is dependent on the  
  maximum number of bytes that is passed in as a parameter, and 
  determined by the reverse traffic channel MAC.  

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet to send, including headers.  A zero-length value
  indicates that the PCP layer should discontinue to query RLP for more data.

SIDE EFFECTS
  The *rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
uint16 hdrrlp_get_retx_data
(
  uint8 combo_id,
  uint8 carrier_id,
  uint16 max_bytes,            /* max number of bytes allowed to send      */
  dsm_item_type **rev_pkt_ptr, /* ptr to data packet built for TX          */
  void **tx_data_handle,        /* Handle for the data                      */
  void *pkt_rrcb_ptr           /* RLP-specific ctl block                   */
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;   /* RLP-specific ctl block          */
  dsrlpi_state_info_type   *state_ptr;  /* local state ptr                 */
  uint16                   pkt_len;     /* length of packet that was built */
  hdrrlptxph_type       *retx_handle_ptr;/*Handle accompanying retx data */
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  uint8                           rscb_index; /* Index in rscb array       */ 
  hdrrlprtx_reason_enum_type  reason;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Make sure the control block pointer is not NULL
  -------------------------------------------------------------------------*/
  ASSERT( pkt_rrcb_ptr != NULL );

  /*-------------------------------------------------------------------------
    Get the session control block and the fcn_ptr_table.
  -------------------------------------------------------------------------*/
  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type *) pkt_rrcb_ptr;
  state_ptr     = &rrcb_ptr->state;

  rscb_index = dsrlp_get_rlp_rscb_index(rrcb_ptr->cfg_ptr->sr_id, rrcb_ptr->cfg_ptr->ver);
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    rscb_index (corresponding to an sr_id).
  -------------------------------------------------------------------------*/ 
  if(rscb_index >= DSRLPI_INVALID_RSCB_INDEX)
  {
    DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                    "RLP TxRexmit[%d,%d] V[%d,%d] Get Retx Data Called with invalid sr_id", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced );
    return(0);
  }
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  /*-------------------------------------------------------------------------
    Set the data handle to NULL initially to indicate there is no data 
  -------------------------------------------------------------------------*/
  *tx_data_handle = NULL;

  /*-------------------------------------------------------------------------
    If flow is disabled for this RLP instance, it cannot transmit any data.
    Return  immediately and indicate zero-length packet, instructing the 
    PCP layer to no longer query RLP for data.
    -----------------------------------------------------------------------*/
  if (hdrcp_fcp_stream_is_open(rrcb_ptr->cfg_ptr->stream_id) == FALSE) 
  {
    return (0);
  }

  /*-------------------------------------------------------------------------
    If RLP is not established, then we cannot send any packets.  Return
    immediately and indicate zero-length packet, instructing the PCP layer
    to no longer query RLP for data.
    -----------------------------------------------------------------------*/
  if ((state_ptr->state != DSRLPI_ESTABLISHED_STATE) ||
      (state_ptr->reset_active == TRUE)
      )
  {

    DATA_RLP_MSG4 (MSG_LEGACY_MED, 
                    "RLP[%d,%d] V[%d,%d] RLP not established for xmit data.", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced);
    
    return (0);
  }

  /*-------------------------------------------------------------------------
    Check if the max size is greater than the RLP header that will be appended
    to the payload. If not, then, no payload can be sent. Return 0.
  -------------------------------------------------------------------------*/
  if ( max_bytes <= hdrrlp_get_header_size(rrcb_ptr->cfg_ptr))
  {
    return (0);
  }

  *rev_pkt_ptr = NULL;
  rex_enter_crit_sect(&state_ptr->data_path_crit_sect);

  
  /*-------------------------------------------------------------------------
    Updates the state variable combo_index
  -------------------------------------------------------------------------*/
  hdrrlpi_update_combo_index(rrcb_ptr, combo_id );
  
  /*-------------------------------------------------------------------------
    Determine if there is any data queued up for re-transmission.
    -----------------------------------------------------------------------*/
  if ( hdrrlprtx_has_rexmits(rrcb_ptr, rrcb_ptr->state.combo_index ) )
  {
    /*-----------------------------------------------------------------------
      Get a handle for the re-transmit data to send
      ---------------------------------------------------------------------*/
    if( (retx_handle_ptr = hdrrlptxph_new_handle(HDRRLP_RETX_HANDLE)) == NULL )
    {
      DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                      "RLP TxRexmit[%d,%d] V[%d,%d] No Free handle to allocate, cannot build retx data", 
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced );
      /* Release the Critical Section before returning */
      rex_leave_crit_sect(&state_ptr->data_path_crit_sect);

      return (0);
    }

    /*-----------------------------------------------------------------------
      Retransmitted data to send.  Extract from re-transmission queue and
      build.  If the requested retransmission cannot be fully contained 
      within the current allowable size, the NAK associated with the re-xmit
      is updated to reflect the number of bytes already sent.  The re-xmit
      will continue in the next round of packet building.  Build re-xmit 
      packet and return the corresponding packet length.  
      ---------------------------------------------------------------------*/
    pkt_len = hdrrlpi_tx_rexmit(rrcb_ptr, 
                                rev_pkt_ptr, 
                                &((hdrrlptxph_retx_type*)retx_handle_ptr)->handle_info,  
                                max_bytes,
                                &reason,
                                rrcb_ptr->state.combo_index );

    if (pkt_len != 0 )
    { 
      /* Set the handle type depending on the reason */
      hdrrlptxph_set_retx_handle_type(retx_handle_ptr, reason);
      
      *tx_data_handle = (void*)hdrrlptxsh_add_packet_handle(combo_id, 
                         carrier_id, retx_handle_ptr, rrcb_ptr); /* Update the return handle*/
      ((hdrrlptxph_retx_type*)retx_handle_ptr)->handle_info.set_id = (uint32)*tx_data_handle;

      if( *rev_pkt_ptr != NULL )
      {
        HDRCOM_DSM_SET_TRACER( (*rev_pkt_ptr), HDRCOM_DSM_RLP_TX ); 
        DSM_TOUCH_ITEM(*rev_pkt_ptr);
      }
      hdrrlp_data_activity_update(rscb_ptr);

      if (hdrrlp_debug.enable_tx_debug > 1 ) 
      {
        DATA_RLP_MSG6 (MSG_LEGACY_HIGH, 
                      "RLP Tx[%d,%d] V[%d,%d] Sending rexmit RLP data : len:0x%x handle_id:0x%x", 
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      pkt_len-hdrrlp_get_header_size(rrcb_ptr->cfg_ptr),
                      (uint32)tx_data_handle);
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Free the allocated re-tx handle if there are no bytes to transmit
        Also set the tx_data_handle to NULL to indicate there is no data to 
        send.
       --------------------------------------------------------------------*/
      hdrrlptxph_free_handle(retx_handle_ptr);
      *tx_data_handle = NULL;
    }
    /* Release the Critical Section before returning */
    rex_leave_crit_sect(&state_ptr->data_path_crit_sect);
    return(pkt_len);
  }

  /* Release the Critical Section before returning */
  rex_leave_crit_sect(&state_ptr->data_path_crit_sect);

    /*-----------------------------------------------------------------------
      There are no retransmit bytes to send. Return 0 bytes
     ----------------------------------------------------------------------*/
  return 0;
}  /* hdrrlp_get_retx_data() */



/*===========================================================================

FUNCTION HDRRLP_GET_NEW_OR_FLUSH_DATA

DESCRIPTION
  This function is called locally for the PCP layer. It builds any  new data to 
  be sent.  If there is no data to send, and the flush timer has expired, a 
  packet containing the last octet of the last packet that was sent will be 
  built. The size of the RLP packet is dependent on the  maximum number of 
  bytes that is passed in as a parameter, and determined by the reverse 
  traffic channel MAC.

  Note that the function only checks for new data. It does not check if there 
  is any retransmit data pending to be sent (except for the flush packets if
  there is no new data.)
  
DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet to send, including headers.  A zero-length value
  indicates that the PCP layer should discontinue to query RLP for more data.

SIDE EFFECTS
  The *rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
uint16 hdrrlp_get_new_or_flush_data
(
  uint8 combo_id,
  uint8 carrier_id,
  uint16 max_bytes,            /* max number of bytes allowed to send      */
  dsm_item_type **rev_pkt_ptr, /* ptr to data packet built for TX          */
  void **tx_data_handle,        /* Handle for the data                      */
  void *pkt_rrcb_ptr           /* RLP-specific ctl block                   */
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;   /* RLP-specific ctl block          */
  dsrlpi_state_info_type   *state_ptr;  /* local state ptr                 */
  uint16                   pkt_len;     /* length of packet that was built */
  hdrrlptxph_type  *newdata_handle_ptr; /* Handle accompanying new data  */
  hdrrlptxph_type  *flushdata_handle_ptr;/*Handle accompanying flush data*/
  hdrrlpi_new_data_pkt_handle_type *handle_info;

  uint16                   tx_item_q_cnt = 0; /* Helps debugging */
  uint16                   wm_data_cnt = 0;   /* Helps debugging */
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  uint8                           rscb_index; /* Index in rscb array        */ 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *tx_data_handle = NULL;
  /*-------------------------------------------------------------------------
    Make sure the control block pointer is not NULL
  -------------------------------------------------------------------------*/
  ASSERT( pkt_rrcb_ptr != NULL );

  /*-------------------------------------------------------------------------
    Get the session control block and the fcn_ptr_table.
  -------------------------------------------------------------------------*/
  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type *) pkt_rrcb_ptr;
  state_ptr     = &rrcb_ptr->state;

  rscb_index = dsrlp_get_rlp_rscb_index(rrcb_ptr->cfg_ptr->sr_id, rrcb_ptr->cfg_ptr->ver);
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    rscb_index (corresponding to an sr_id).
  -------------------------------------------------------------------------*/ 
  if(rscb_index >= DSRLPI_INVALID_RSCB_INDEX)
  {
    DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                    "RLP [%d,%d] V[%d,%d] get_new_or_flush_data() called with invalid sr_id", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced );
    return(0);
  }
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  /*-------------------------------------------------------------------------
    If flow is disabled for this RLP instance, it cannot transmit any data.
    Return  immediately and indicate zero-length packet, instructing the 
    PCP layer to no longer query RLP for data.
    -----------------------------------------------------------------------*/
  if (hdrcp_fcp_stream_is_open(rrcb_ptr->cfg_ptr->stream_id) == FALSE) 
  {
    return (0);
  }

  /*=========================================================================
    Determine if there is new data or flush data to send.  New data is sent 
    first. If the flush timer has expired, then build a flush timer packet.
    =======================================================================*/

  /*-------------------------------------------------------------------------
    Check if the max size is greater than the RLP header that will be appended
    to the payload. If not, then, no payload can be sent. Return 0.
  -------------------------------------------------------------------------*/
  if ( max_bytes <= hdrrlp_get_header_size(rrcb_ptr->cfg_ptr))
  {
    return (0);
  }

  *rev_pkt_ptr = NULL;

  rex_enter_crit_sect(&state_ptr->data_path_crit_sect);

  /*-------------------------------------------------------------------------
    If RLP is not established, then we cannot send any packets.  Return
    immediately and indicate zero-length packet, instructing the PCP layer
    to no longer query RLP for data.
    -----------------------------------------------------------------------*/
  if ((state_ptr->state != DSRLPI_ESTABLISHED_STATE) ||
      (state_ptr->reset_active == TRUE)
      )
  {

    DATA_RLP_MSG4 (MSG_LEGACY_MED, 
                    "RLP Tx[%d,%d] V[%d,%d] not established for xmit data.", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced );
    /* Release the Critical Section before returning */
    rex_leave_crit_sect(&state_ptr->data_path_crit_sect);
    return (0);
  }

  /*-------------------------------------------------------------------------
    If there is data available in the transmit watermark, then build a 
    new packet.
  -------------------------------------------------------------------------*/
  if (((tx_item_q_cnt = q_cnt(&rrcb_ptr->state.tx_item_q)) != 0) ||
      ((wm_data_cnt = hdrrlpi_get_new_data_count(rrcb_ptr)) != 0))
  {
    /*-------------------------------------------------------------------------
      Updates the state variable combo_index
    -------------------------------------------------------------------------*/
    hdrrlpi_update_combo_index(rrcb_ptr, combo_id );
    
    if (hdrrlp_debug.enable_tx_debug > 1 ) 
    {
      DATA_RLP_MSG6( MSG_LEGACY_HIGH, 
                    "RLP Tx[%d,%d] V[%d,%d] tx_item_q has %d and WM has %d",
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced,
                    tx_item_q_cnt,
                    wm_data_cnt );
    }

    if((newdata_handle_ptr = hdrrlptxph_new_handle(HDRRLP_NEW_DATA_HANDLE)) == NULL )
    {
      DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                      "RLP Tx[%d,%d] V[%d,%d] No free handle to allocate, cannot build new data", 
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced );
      /* Release the Critical Section before returning */
      rex_leave_crit_sect(&state_ptr->data_path_crit_sect);
      return (0);
    }

    /*-----------------------------------------------------------------------
      Build maximum-sized new packet and return the corresponding packet
      length.  
    -----------------------------------------------------------------------*/
    pkt_len = hdrrlpi_tx_new_data(rrcb_ptr, 
                                  rev_pkt_ptr, 
                                  &((hdrrlptxph_newdata_type*)newdata_handle_ptr)->handle_info, 
                                  max_bytes,
                                  rrcb_ptr->state.combo_index );

    if (pkt_len != 0 )
    {
      /*--------------------------------------------------------
        Make PCP handle point to our new data handle.
        ----------------------------------------------------------*/
      *tx_data_handle = (void*)hdrrlptxsh_add_packet_handle(combo_id, carrier_id, 
                                    newdata_handle_ptr, rrcb_ptr);
      ((hdrrlptxph_newdata_type*)newdata_handle_ptr)->handle_info.set_id = (hdrrlptxsh_handle_type)*tx_data_handle;
      if( *rev_pkt_ptr != NULL )
      {
        DSM_TOUCH_ITEM(*rev_pkt_ptr);
      }
      hdrrlp_data_activity_update(rscb_ptr);

      if (hdrrlp_debug.enable_tx_debug > 1 ) 
      {
        DATA_RLP_MSG6 (MSG_LEGACY_HIGH, 
                      "RLP Tx[%d,%d] V[%d,%d] Sending new RLP data : len 0x%x tx_data_handle:0x%x", 
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      pkt_len-hdrrlp_get_header_size(rrcb_ptr->cfg_ptr),
                      (uint32)*tx_data_handle);
      }                      
    }
    else
    {
      /*---------------------------------------------------------------------
        Free the allocated new data handle if there are no bytes to transmit
        Also set the tx_data_handle to NULL to indicate there is no data to 
        send.
       --------------------------------------------------------------------*/
      hdrrlptxph_free_handle(newdata_handle_ptr);
      *tx_data_handle = NULL;
    }
    /* Release the Critical Section before returning */
    rex_leave_crit_sect(&state_ptr->data_path_crit_sect);

    return(pkt_len);
  }

  /*-------------------------------------------------------------------------
    Determine status of the flush timer.  If the flush timer has expired,
    it indicates that no data has been sent for a period of time since the
    last packet has been sent.  Need to resend the last octet of the last
    packet that was sent.  If the flush timer has expired, it does not get
    activated until the next packet is sent.    
    -------------------------------------------------------------------------*/
  if ( rrcb_ptr->cfg_ptr->nak_enabled == TRUE &&
       rrcb_ptr->state.flush_timer_expired == TRUE && 
	    ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_VER_MFPA)
       || ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA || 
          rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA)
          && rrcb_ptr->state.flush_giveup_timer_expired == FALSE)))
  {
    /*-------------------------------------------------------------------------
      Updates the state variable combo_index, just in case the 
      flush packet is going on a different combo
    -------------------------------------------------------------------------*/
    hdrrlpi_update_combo_index(rrcb_ptr, combo_id );


    /*-------------------------------------------------------------------------
      If we have not transmitted a flush packet before, only then we go ahead
      and try to send a flush packet.
    -------------------------------------------------------------------------*/
    if  (rrcb_ptr->state.local_state[rrcb_ptr->state.combo_index].txed_flush_pkt == FALSE )
    {
      /*-----------------------------------------------------------------------
        Build the flush packet.  It is simply the last octet of the last frame
        that was sent.
      -----------------------------------------------------------------------*/
      if((flushdata_handle_ptr = hdrrlptxph_new_handle(HDRRLP_FLUSH_DATA_HANDLE)) == NULL )
      {
        /* Release the Critical Section before returning */
        rex_leave_crit_sect(&state_ptr->data_path_crit_sect);

        DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                        "RLP Tx[%d,%d] V[%d,%d] No free handle to allocate, cannot build flush data", 
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced );
        return (0);
      }
      
      pkt_len = hdrrlpi_tx_last_data_unit(rrcb_ptr, rev_pkt_ptr, max_bytes);

      if (pkt_len != 0 && *rev_pkt_ptr != NULL)
      {          
        dsm_item_type *duplicate_dsm_item_ptr = NULL;
        rrcb_ptr->state.local_state[rrcb_ptr->state.combo_index].txed_flush_pkt = TRUE;
        /*------------------------------------------------------------------------
        The flush packet (dsm item) is duped and saved on flush handle's 
        head_item_ptr. When the PCP calls status callback, it returns the handle. 
        The dsm item is processed (if necessary) and freed. 
        This mechanism is to facilitate the RLP logging mechanism.
        ------------------------------------------------------------------------*/
        handle_info = &((hdrrlptxph_newdata_type*)flushdata_handle_ptr)->handle_info;

        (void) dsm_dup_packet(&(duplicate_dsm_item_ptr), 
                 *rev_pkt_ptr, 0, (uint16)dsm_length_packet(*rev_pkt_ptr));

        if (NULL == duplicate_dsm_item_ptr) {
           /* cleanup */
           dsm_free_packet (rev_pkt_ptr);
           return (0);
        }
        handle_info->head_item_ptr = duplicate_dsm_item_ptr;

        *tx_data_handle = (void*) hdrrlptxsh_add_packet_handle(combo_id, carrier_id, 
                                    flushdata_handle_ptr, rrcb_ptr);
        ((hdrrlptxph_newdata_type*)flushdata_handle_ptr)->handle_info.set_id = (hdrrlptxsh_handle_type)*tx_data_handle;

        HDRCOM_DSM_SET_TRACER( (*rev_pkt_ptr), HDRCOM_DSM_RLP_TX ); 
        DSM_TOUCH_ITEM(*rev_pkt_ptr);
        hdrrlp_data_activity_update(rscb_ptr);

        /* Release the Critical Section before returning */
        rex_leave_crit_sect(&state_ptr->data_path_crit_sect);

        if (hdrrlp_debug.enable_tx_debug > 1 ) 
        {
          DATA_RLP_MSG6 (MSG_LEGACY_HIGH, 
                      "RLP Tx[%d,%d] V[%d,%d] Sending flush RLP data : len 0x%x tx_data_handle:0x%x", 
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced,
                      pkt_len-hdrrlp_get_header_size(rrcb_ptr->cfg_ptr),
                      (uint32)tx_data_handle);
        }                      

        return (pkt_len);
      }
      else
      {
        /*---------------------------------------------------------------------
          Free the allocated new data handle if there are no bytes to transmit
          Also set the tx_data_handle to NULL to indicate there is no data to 
          send.
        --------------------------------------------------------------------*/
        hdrrlptxph_free_handle(flushdata_handle_ptr);
        if (hdrrlp_debug.enable_tx_debug > 1 ) 
        {
        	DATA_RLP_MSG4 (MSG_LEGACY_HIGH, "RLP Tx[%d,%d] V[%d,%d] No data to send - Return 0",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced);
        }                
      /* Release the Critical Section before returning */
      rex_leave_crit_sect(&state_ptr->data_path_crit_sect);

        *tx_data_handle = NULL;
        return (0);
      }
    }  
  }
  /* Release the Critical Section before returning */
  rex_leave_crit_sect(&state_ptr->data_path_crit_sect);

  /*-------------------------------------------------------------------------
    If the logic makes it to here, then there is no more data to send.  
    Indicate zero-length packet, instructing the PCP layer to no longer query
    RLP for data.
    -------------------------------------------------------------------------*/
  if (hdrrlp_debug.enable_tx_debug > 1 ) 
  {
  DATA_RLP_MSG4 (MSG_LEGACY_HIGH, "RLP Tx[%d,%d] V[%d,%d] No data to send - Return 0",
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced);
  }
  return(0);
}  /* HDRRLP_GET_NEW_OR_FLUSH_DATA */


/*===========================================================================

FUNCTION HDRRLP_PROCESS_RLP_PKT

DESCRIPTION
  This function processes the payload of a received RLP data 
  packet.  The function is called on a per packet basis.  As data is 
  processed, the function updates RLP state variables as needed.  
  Out-of-sequence data may also cause this function to generate NAK list 
  entries used to build up a NAK signaling message on subsequent TX 
  processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RLP state variables V(R) and/or V(N) may get updated.  Entries may be 
  inserted into the NAK list.  Entries may be added or removed from the 
  resequencing buffer.  Signaling message processing may cause data to
  be queued for re-transmission and/or cause and RLP RESET.

===========================================================================*/
void hdrrlp_process_rlp_pkt
(
  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr,       /* RLP-specific ctl block   */
  dsm_item_type *fwd_pkt_ptr,      /* ptr to received RLP data packet      */
  dsrlpi_header_type          *rlp_header_ptr /* RLP Header info          */
)
{
  uint32                       rxed_seq =0;    /* received OTA seq         */
  uint32                       reseq_q_cnt;    /* cnt of nodes in reseq_q  */
  uint32                       rx_wm_cnt;    /* cnt of bytes in the rx_wm */
  dsrlpi_sess_ctl_blk_type    *rscb_ptr;     /* rlp-specific control block */
  uint8                        rscb_index;   /* Index in rscb array        */ 
  uint16                       data_len;
  boolean                      is_recursive;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rscb_index = dsrlp_get_rlp_rscb_index(rrcb_ptr->cfg_ptr->sr_id, rrcb_ptr->cfg_ptr->ver);
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    rscb_index (corresponding to an sr_id).
  -------------------------------------------------------------------------*/ 
  if(rscb_index >= DSRLPI_INVALID_RSCB_INDEX)
  {
    DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                    "RLP Tx[%d,%d] V[%d,%d] hdrrlp_process_rlp_pkt() called with invalid sr_id", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced );
    return;
  }
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  //Reset CTA timer if it is started
  hdrrlp_data_activity_update(rscb_ptr);

  HDRRLPSEQ_STORE_SEQ(rxed_seq, rlp_header_ptr->seq_num,
                         (rrcb_ptr->cfg_ptr->seq_modulus));

  /*---------------------------------------------------------------------
    Insert into resequencing queue.  This operation will update the
    context to reflect any data that was discarded as duplicate.
    --------------------------------------------------------------------*/
  DSM_TOUCH_ITEM(fwd_pkt_ptr);
  data_len = (uint16)dsm_length_packet(fwd_pkt_ptr);

  /* The last parameter in the function call has been added to overcome the problem described below..
     The insert method does not update the statistics in the log packet correctly if there
     is any out of order transmission in the same slot. The RLP packets with a lower sequence
     number (that were processed after RLP packets with higher sequence numbers)were not
     being counted as new bytes even though both were received in the same slot. This happens
     mainly in Rev B where there is a higher probability of such a case happening due to the
     multicarrier reception
     By passing the flag into the insert method, we enable it to update the statistics correctly.
  */
  is_recursive = FALSE;
  if (!hdrrlprsq_insert(rrcb_ptr, 
                        rxed_seq,
                        rlp_header_ptr->first_data_unit, 
                        rlp_header_ptr->last_data_unit,
                        fwd_pkt_ptr, 
                        rlp_header_ptr->qn_seq_included, 
                        is_recursive))
  {
    /* Must have all been duplicate... */
    return;
  }
  else
  {
    /* Update stats with q_cnt of reseq_q */
    reseq_q_cnt = q_cnt( &rrcb_ptr->state.reseq_q );
    if (reseq_q_cnt > rrcb_ptr->stats.rx_max_reseq_q_size)
    {
      rrcb_ptr->stats.rx_max_reseq_q_size = reseq_q_cnt;
    }
    /* Update rx stats for MMPA retx count */
    if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
        (!rlp_header_ptr->qn_seq_included))
    {
      rrcb_ptr->stats.rx_rexmit_fr_cnt += data_len;
      rrcb_ptr->stats.rx_rexmit_frames++;
    }
  }
  /*-------------------------------------------------------------------
    Pass all possible (depends on configured data unit and out
    of order allowed) data units in the resequencing buffer to
    the higher layer.
    ------------------------------------------------------------------*/
  hdrrlprsq_remove(rrcb_ptr);

  /* Update stats with q_cnt of rx_wm */
  rx_wm_cnt = dsm_queue_cnt( rrcb_ptr->cfg_ptr->rx_wm_ptr[rrcb_ptr->route] );
  if (rx_wm_cnt > rrcb_ptr->stats.rx_max_wm_q_size)
  {
    rrcb_ptr->stats.rx_max_wm_q_size = rx_wm_cnt;
  }
}

/*===========================================================================

FUNCTION HDRRLP_END_OF_SLOT_CB

DESCRIPTION
  Callback registered with PCP to indicate end of slot notification.
  All the packets received in the previous slot can now be processed in
  sorted order.
  
DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS

===========================================================================*/
static void hdrrlp_end_of_slot_cb( void* unused )
{

  dsrlpi_rlp_ctl_blk_type     *rrcb_ptr;       /* RLP-specific ctl block   */
  dsm_item_type               *fwd_pkt_ptr;
  dsrlpi_pkt_info_type        *pkt_info_ptr;
  uint16                       i;              /* loop counter             */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i<hdrrlp_num_rrcbs_used_in_slot; i++)
  {
    rrcb_ptr = hdrrlp_rrcbs_used_in_slot[i];
    
    /*-------------------------------------------------------------------------
      Sort the rlp packets received in the last slot based on RLP seq num
    -------------------------------------------------------------------------*/
    hdrrlpsort_sort_packets_in_slot(rrcb_ptr);

    while((pkt_info_ptr = (dsrlpi_pkt_info_type*)q_get(&rrcb_ptr->state.buffered_rx_q)) != NULL)
    {
      fwd_pkt_ptr = pkt_info_ptr->fwd_pkt_ptr;

      /*-------------------------------------------------------------------------
        Packets will now get processed by the rlp layer.
      ------------------------------------------------------------------------*/
      hdrrlp_process_rlp_pkt( rrcb_ptr, fwd_pkt_ptr, &pkt_info_ptr->rlp_header );

      /* Put the pkt_info_ptr back on the free_q */
      q_put( &hdrrlp_pkt_info_free_q, &(pkt_info_ptr->link) );
    }
  }

  /*-------------------------------------------------------------------------
    Processed all packets in last slot. 
    Reset variable for pkts to be received in next slot 
  ------------------------------------------------------------------------*/
  hdrrlp_num_rrcbs_used_in_slot = 0;

  /*------------------------------------------------------------------------
    Finished with received data processing, so return.
  ------------------------------------------------------------------------*/
  return;
} /* hdrrlp_end_of_slot_cb */

/*===========================================================================

FUNCTION HDRRLP_PUT_PKT_CB

DESCRIPTION
  This function is called by the PCP layer, to process a received RLP data 
  packet.  The function is called on a per packet basis.  As data is 
  processed, the function updates RLP state variables as needed.  
  Out-of-sequence data may also cause this function to generate NAK list 
  entries used to build up a NAK signaling message on subsequent TX 
  processing.

  In addition, the function will process any RLP signaling messages available
  on the received message queue.  This ensures that RLP signaling messages
  will have been processed, prior to processing incoming data.  At the end of
  processing a full HDR frame, PCP will call this function with a NULL 
  parameter, to ensure that all signaling messages for RLP have been 
  processed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RLP state variables V(R) and/or V(N) may get updated.  Entries may be 
  inserted into the NAK list.  Entries may be added or removed from the 
  resequencing buffer.  Signaling message processing may cause data to
  be queued for re-transmission and/or cause and RLP RESET.

===========================================================================*/
static void hdrrlp_put_pkt_cb
(
  dsm_item_type *fwd_pkt_ptr,     /* ptr to received RLP data packet       */
  void *fwd_pkt_rrcb_ptr          /* RLP-specific ctl block                */
)
{
  dsrlpi_rlp_ctl_blk_type    *rrcb_ptr;        /* RLP-specific ctl block   */
  dsrlpi_pkt_info_type       *pkt_info_ptr;   
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Make sure the control block pointer is not NULL
    -------------------------------------------------------------------------*/
  ASSERT( fwd_pkt_rrcb_ptr != NULL );

  /*-------------------------------------------------------------------------
    Get the session control block and the fcn_ptr_table.
    -------------------------------------------------------------------------*/
  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type *) fwd_pkt_rrcb_ptr;
  /*-------------------------------------------------------------------------
    Determine whether or not the fwd_pkt_ptr contains a valid DSM item.  If
    it is NULL, assume that this function was called at the end of PCP to
    flush out any possible RLP signaling messages from the receive message
    queue.  As such, there is no data to process and the function returns
    immediately.
    -----------------------------------------------------------------------*/
  if (fwd_pkt_ptr == NULL)
  {
    return;
  }

  if (hdrrlp_debug.enable_rx_debug > 1) 
  {
    DATA_RLP_MSG6 (MSG_LEGACY_MED, 
                   "RLP Rx[%d,%d] V[%d,%d]Data Packet received on Carrier:%d Sched_grp_id:%d", 
                   rrcb_ptr->cfg_ptr->sr_id,
                   "AB"[(rrcb_ptr->route)],
                   rrcb_ptr->cfg_ptr->ver,
                   rrcb_ptr->cfg_ptr->enhanced,
                   HDRCOM_DSM_GET_CARRIER_ID(fwd_pkt_ptr),
                   HDRCOM_DSM_GET_SCHED_GROUP_ID(fwd_pkt_ptr));
  }


  /*-------------------------------------------------------------------------
    If RLP is not configured, or flow is disabled for this RLP instance, it 
    cannot receive any data.  Free the received packet and return.
    -----------------------------------------------------------------------*/
  if ( (hdrcp_fcp_stream_is_open(rrcb_ptr->cfg_ptr->stream_id) == FALSE) ) 
  {
    dsm_free_packet(&fwd_pkt_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    If RLP is not established, then we cannot receive any data packets.  At
    this point, RLP must be in the RESET state, and is required to silently
    discard the RLP packet.
    -----------------------------------------------------------------------*/
  if (HDRRLP_IN_RESET(rrcb_ptr))
  {
    /*-----------------------------------------------------------------------
      Free the RLP packet dsm item and return.
      ---------------------------------------------------------------------*/
    DATA_RLP_MSG4 (MSG_LEGACY_MED, 
                    "RLP Rx[%d,%d] V[%d,%d] Data Packet received while not established", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced);

    dsm_free_packet(&fwd_pkt_ptr);
    return;
  }

#if defined(FEATURE_HDRRLP_LOGGING)
  if (log_status(hdrrlp_fr_rx_log.hdr.code))
  {
    hdrrlp_log_rx_rlp_frames(fwd_pkt_ptr);   /* Partial RLP log */
  }
#endif
  DSM_TOUCH_ITEM(fwd_pkt_ptr);

  /*------------------------------------------------------------------------
  Get a pkt_info_ptr from the free_q. This would be returned back to free_q
  after hdrrlp is done processing the packet upon receiving end of slot 
  notification.
  ------------------------------------------------------------------------*/
  if ((pkt_info_ptr = (dsrlpi_pkt_info_type*)q_get(&hdrrlp_pkt_info_free_q)) == NULL)
  {
    DATA_RLP_MSG0( MSG_LEGACY_ERROR, "Could not obtain pkt_info_ptr from free queue" );
    dsm_free_packet(&fwd_pkt_ptr);
    return;
  }

  /*------------------------------------------------------------------------
    Extract the RLP header and then process the RLP packet
  ------------------------------------------------------------------------*/
  if (hdrrlpi_extract_rlp_header( rrcb_ptr, &fwd_pkt_ptr, &(pkt_info_ptr->rlp_header) ) != E_SUCCESS)
  {
    DATA_RLP_MSG4( MSG_LEGACY_ERROR, "RLP Rx[%d,%d] V[%d,%d] Error extracting RLP header",
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced);
    /* dsm item already freed in hdrrlpi_extract_rlp_header() */
    /* Put the pkt_info_ptr back on the free_q */
    q_put( &hdrrlp_pkt_info_free_q, &(pkt_info_ptr->link) );
    return;
  }
 
  pkt_info_ptr->fwd_pkt_ptr = fwd_pkt_ptr;

  /*------------------------------------------------------------------------
    Keep track of all rrcb's that recv packets in this slot for easy 
    retrieval during packet processing upon receiving EOS notification.
  ------------------------------------------------------------------------*/
  if (q_cnt(&rrcb_ptr->state.buffered_rx_q) == 0)
  {
    hdrrlp_rrcbs_used_in_slot[hdrrlp_num_rrcbs_used_in_slot++] = rrcb_ptr;
  }

  /*------------------------------------------------------------------------
    Pass the packetinfo to RLP sorter to store this packet in RLP ctl block 
    till the end of slot notification is received from PCP
  ------------------------------------------------------------------------*/
  hdrrlpsort_buffer_packet( rrcb_ptr, pkt_info_ptr );

  return;
} /* HDRRLP_PUT_PKT_CB */

/*===========================================================================

FUNCTION HDRRLP_MSG_CB

DESCRIPTION
    This callback function invoked by HMP whenever a control message is 
    received for RLP.  This function queues up a received RLP signaling 
    message to the RLP receive signaling message queue.  

DEPENDENCIES
  The receive signaling message queue must have been initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_msg_cb
(
  dsm_item_type *msg_ptr,               /* ptr to received RLP message     */
  uint8 rlp_id                 /* RLP instance for which this msg is meant */                           
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;      /* RLP-specific control block   */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;      /* RLP-specific control block   */
  dsrlpi_state_info_type   *state_ptr;     /* local state ptr              */
  uint8                    rscb_index;     /* local RSCB index             */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Using the sr_id, get the session control block and fcn_ptr_table. 
  -------------------------------------------------------------------------*/
  ASSERT(( rlp_id == DSRLPI_HDR_AN_AUTH_SR_ID_VALUE) || 
         ( rlp_id == DSRLP_HDR_SN_RLP_SR_ID_VALUE));

  rscb_index = dsrlp_get_rlp_rscb_index( rlp_id, DSRLP_VER_HDR );
  if(rscb_index == DSRLPI_INVALID_RSCB_INDEX)
  {
    DATA_RLP_MSG0( MSG_LEGACY_ERROR, "RLP not initialized for signaling messages!" );
    dsm_free_packet(&msg_ptr);
    return;
  }

  rscb_ptr      = &dsrlpi_rscb_array[rscb_index]; 

  /* DPA only uses route A */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr      = &rscb_ptr->rlp_list[DSRSP_ROUTE_A]; 
  state_ptr     = &rrcb_ptr->state;  
  /*-------------------------------------------------------------------------
    Check to see if RLP is even up yet.
  -------------------------------------------------------------------------*/
  if ( (rrcb_ptr->cfg_ptr->tx_wm_ptr == NULL) ||
       (rrcb_ptr->state.state == DSRLPI_NULL_STATE))
  {
    DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                    "RLP [%d,%d] V[%d,%d] Not initialized for signalling messages", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced );
    dsm_free_packet(&msg_ptr);
    return;
  }

  /*-----------------------------------------------------------------------
    Process each signaling message.  
    ---------------------------------------------------------------------*/
  DATA_RLP_MSG4 (MSG_LEGACY_MED, "RLP Rx[%d,%d] V[%d,%d] Processing RLP SIG MSG ",
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced );

  hdrrlpi_rx_process_sig_msg(rrcb_ptr, state_ptr, msg_ptr);

  return;

} /* HDRRLP_MSG_CB */

#if defined(FEATURE_HDRRLP_LOGGING)
/*===========================================================================

FUNCTION HDRRLP_LOG_STATISTICS

DESCRIPTION
  Logs the current values of the HDR RLP statistics to the off-target 
  diagnostic tool. If we can't allocate space for the log packet on the DIAG
  ring buffer, we'll abort and try again next time.

  Input Parameters:
    rscb_index - Index of the RLP for which statistics are to be logged

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlp_log_statistics
( 
  uint8 rscb_index
)
{

  LOG_HDR_RLP_STATS_C_type  *rlp_stats_log_pkt = NULL;/* Log Packet ptr    */
  dsrlp_stats_info_type     *rlp_stats_ptr     = NULL;/* Ptr to Statistics */

  dsrlpi_sess_ctl_blk_type  *rscb_ptr;                /* rlp-specific block*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(rscb_index < DSRLP_MAX_RSCB_ELEMENTS)
  {
    rscb_ptr = &(dsrlpi_rscb_array[rscb_index]);

    /*-------------------------------------------------------------------------
      Every second we want to print a MSG_MED with the # of bytes rxed on the 
      Fwd link for HDR traffic. All the other activities for logging need to
      happen about every 4 seconds.
    --------------------------------------------------------------------------*/
  
    /* 1x has only one route */
    ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
           rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
    (void) dsrlp_get_stats_ex( rscb_index,
                     DSRSP_ROUTE_A,
                     &rlp_stats_ptr );
 
    if(log_status( LOG_HDR_RLP_STATS_C ))
    {
      /* We'll try... */
      rlp_stats_log_pkt = ( LOG_HDR_RLP_STATS_C_type *) 
        log_alloc( LOG_HDR_RLP_STATS_C, sizeof( LOG_HDR_RLP_STATS_C_type ));

      if( rlp_stats_log_pkt != NULL ) 
      {
        rlp_stats_log_pkt->service_id = dsrlp_get_sr_id( rscb_index );
        qw_equ(rlp_stats_log_pkt->reset_time, rlp_stats_ptr->reset_timestamp);
        rlp_stats_log_pkt->naks_received = rlp_stats_ptr->naks_received;
        rlp_stats_log_pkt->re_xmits_not_found =
                                  rlp_stats_ptr->re_xmits_not_found;
        rlp_stats_log_pkt->an_nak_bytes_requested = 
                                  rlp_stats_ptr->an_nak_bytes_requested;
        rlp_stats_log_pkt->rx_duplicate_bytes = rlp_stats_ptr->rx_dup_fr_cnt;
        rlp_stats_log_pkt->rx_rexmit_bytes    = 
                                  rlp_stats_ptr->rx_rexmit_fr_cnt;
        rlp_stats_log_pkt->rx_new_data_bytes  = 
                                  rlp_stats_ptr->rx_new_data_fr_cnt;
        rlp_stats_log_pkt->rx_total_bytes     = rlp_stats_ptr->rx_total_bytes;
        rlp_stats_log_pkt->tx_naks_cnt        = rlp_stats_ptr->tx_naks[0]; 
        rlp_stats_log_pkt->at_nak_bytes_requested = 
                                  rlp_stats_ptr->at_nak_bytes_requested;
        rlp_stats_log_pkt->tx_rexmit_bytes    =
                                  rlp_stats_ptr->tx_rexmit_fr_cnt;
        rlp_stats_log_pkt->tx_new_data_bytes  = 
                                  rlp_stats_ptr->tx_new_data_fr_cnt;
        rlp_stats_log_pkt->tx_total_bytes     = rlp_stats_ptr->tx_total_bytes;
        rlp_stats_log_pkt->aborts             = rlp_stats_ptr->aborts;
        rlp_stats_log_pkt->resets             = rlp_stats_ptr->resets;
        rlp_stats_log_pkt->at_reset_request_count = 
                                  rlp_stats_ptr->at_reset_request_count;
        rlp_stats_log_pkt->an_reset_ack_count = 
                                  rlp_stats_ptr->an_reset_ack_count;
        rlp_stats_log_pkt->an_reset_request_count = 
                                  rlp_stats_ptr->an_reset_request_count;

        log_commit( (log_type*) rlp_stats_log_pkt );

      }/* if */
    }/* if */
  }/* if */

} /* hdrrlp_log_statistics */
#endif /*FEATURE_HDRRLP_LOGGING*/

/*===========================================================================

FUNCTION HDRRLP_FLUSH_TIMER_CB

DESCRIPTION
    This callback function called whenever the flush timer expires.
    The function sets the RLP session's boolean flush_timer_expired variable
    to TRUE, indicating that the next Tx interrupt will generate an RLP
    flush packet.

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  Care must be taken to ensure mutual exclusion, hence the boolen
    update is protected by INTLOCKs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A flush packet may be generated at the next Tx build-frame interrupt.

===========================================================================*/
void hdrrlp_flush_timer_cb
(
  uint32 param                                       /* RLP control block  */
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* RLP-specific control block  */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a pointer to the Control Block of the RLP & get it's RLP ID
   ------------------------------------------------------------------------*/
  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type*)param;

  DATA_RLP_MSG4 (MSG_LEGACY_MED, "RLP[%d,%d] V[%d,%d] Flush Timer Exp", 
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced );

  /*-------------------------------------------------------------------------
    Clear the timer and set the boolean.
   ------------------------------------------------------------------------*/
  (void) rex_clr_timer( &rrcb_ptr->flush_timer);
  /*-----------------------------------------------------------------------
    Enter critical section to protect RLP state info.
  -----------------------------------------------------------------------*/ 
  DS_ENTER_CRIT_SECTION( &rrcb_ptr->state.data_path_crit_sect );
  rrcb_ptr->state.flush_timer_expired = TRUE;
  /*-----------------------------------------------------------------------
    Leave critical section.
  -----------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION( &rrcb_ptr->state.data_path_crit_sect );

} /* hdrrlp_flush_timer_cb */

/*===========================================================================

FUNCTION HDRRLP_ENHANCED_FLUSH_TIMER_CB

DESCRIPTION
    This callback function called whenever the flush timer expires.
    The function sets the RLP session's boolean flush_timer_expired variable
    to TRUE, indicating that the next Tx interrupt will generate an RLP
    flush packet.

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  Care must be taken to ensure mutual exclusion, hence the boolen
    update is protected by INTLOCKs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A flush packet may be generated at the next Tx build-frame interrupt.

===========================================================================*/
void hdrrlp_enhanced_flush_timer_cb
(
  uint32 param                                       /* RLP control block  */
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* RLP-specific control block  */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a pointer to the Control Block of the RLP & get it's RLP ID
   ------------------------------------------------------------------------*/
  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type*)param;

  DATA_RLP_MSG4 (MSG_LEGACY_MED, "RLP[%d,%d] V[%d,%d] Flush Timer Exp", 
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced );

  /*-------------------------------------------------------------------------
    Clear the timer and set the boolean.
   ------------------------------------------------------------------------*/
  (void) rex_clr_timer( &rrcb_ptr->flush_timer);
  (void) rex_clr_timer( &rrcb_ptr->flush_giveup_timer);
  (void) rex_set_timer( &rrcb_ptr->flush_giveup_timer, 
                      rrcb_ptr->cfg_ptr->nak_abort_timer_val);

  /*-----------------------------------------------------------------------
    Enter critical section to protect Data path.
  -----------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION( &rrcb_ptr->state.data_path_crit_sect );
  rrcb_ptr->state.flush_timer_expired = TRUE;
  rrcb_ptr->state.flush_giveup_timer_expired = FALSE;
  /*-----------------------------------------------------------------------
    Leave critical section.
  -----------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION( &rrcb_ptr->state.data_path_crit_sect );

} /* hdrrlp_enhanced_flush_timer_cb */

/*===========================================================================

FUNCTION HDRRLP_FLUSH_GIVEUP_TIMER_CB

DESCRIPTION
    This callback function called whenever the flush giveup timer expires.
    The function sets the RLP session's boolean flush_giveup_timer_expired variable
    to TRUE, indicating that the next Tx interrupt will not generate an RLP
    flush packet.

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  Care must be taken to ensure mutual exclusion, hence the boolen
    update is protected by INTLOCKs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A flush packet may be generated at the next Tx build-frame interrupt.

===========================================================================*/
void hdrrlp_flush_giveup_timer_cb
(
  uint32 param                                       /* RLP control block  */
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* RLP-specific control block  */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a pointer to the Control Block of the RLP & get it's RLP ID
   ------------------------------------------------------------------------*/
  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type*)param;

  DATA_RLP_MSG4 (MSG_LEGACY_MED, "RLP[%d,%d] V[%d,%d] Flush Giveup Timer Exp", 
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced );

   /*-------------------------------------------------------------------------
    Clear the timer and set the boolean.
   ------------------------------------------------------------------------*/
  (void) rex_clr_timer( &rrcb_ptr->flush_giveup_timer);

  /*-----------------------------------------------------------------------
    Enter critical section to protect Data path.
  -----------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION( &rrcb_ptr->state.data_path_crit_sect );
  rrcb_ptr->state.flush_giveup_timer_expired = TRUE;
  /*-----------------------------------------------------------------------
    Leave critical section.
  -----------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION( &rrcb_ptr->state.data_path_crit_sect );

} /* hdrrlp_flush_giveup_timer_cb */

/*===========================================================================

FUNCTION HDRRLP_INACTIVITY_TIMER_CB

DESCRIPTION
    This callback function called when the inactivity timer expires.The function 
	call the API to set the corresponding bit in the 64-bit global variable, 
	hdrrlp_enhanced_dormant_bitmap to 0. 

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  Care must be taken to ensure mutual exclusion, hence the boolen
    update is protected by INTLOCKs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The HDR data call could go to dormant.

===========================================================================*/
void hdrrlp_inactivity_timer_cb
(
  uint32 param                                       /* RLP control block  */
)
{
 dsrlpi_sess_ctl_blk_type *rscb_ptr;       /* RLP-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a pointer to the Control Block of the RLP & get it's RLP ID
   ------------------------------------------------------------------------*/
  rscb_ptr = (dsrlpi_sess_ctl_blk_type*)param;

  DATA_RLP_MSG2 (MSG_LEGACY_MED, "RLP[%d] V[%d] Enhanced_cta Inactivity Timer Exp", 
                 rscb_ptr->cfg.sr_id, 
                 rscb_ptr->cfg.ver );

  /*-------------------------------------------------------------------------
    Clear the timer and set the boolean.
   ------------------------------------------------------------------------*/
  (void) rex_clr_timer( &rscb_ptr->cfg.inactivity_timer);
  /*-----------------------------------------------------------------------
    Enter critical section to protect RLP config info.
  -----------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&dsrlp_cfg_activity_crit_sect);
  rscb_ptr->cfg.activity_bitmap = 0;
  /*-----------------------------------------------------------------------
    Leave critical section.
  -----------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&dsrlp_cfg_activity_crit_sect);
  hdrrlp_cta_clear_enhanced_idle_bit( rscb_ptr->cfg.sr_id, 
                                      rscb_ptr->cfg.ver, 
                                      rscb_ptr->cfg.enable_inactivity_timer );

} /* hdrrlp_inactivity_timer_cb */
/*===========================================================================

FUNCTION HDRRLP_RESET_TIMER_CB

DESCRIPTION
    This callback function called whenever the reset timer expires.
    If the timer expired, it indicates that the RLP instance has been in the
    RESET state for a duration greater than HDRRLP_MAX_RESET_TIME.  At this
    time, the connection is torn down as a fail-safe mechanism to prevent a
    potential RLP deadlock situation.

    The timer is identified by using the rlp_id that gets passed in the 
    callback.

    Note that since the timer is defined in hdrrlp_startup() which is called
    from the DS task, the Rex extended timer will run in the context of the
    DS task.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The connection is released.

===========================================================================*/
void hdrrlp_reset_timer_cb
(
  uint32 param                                       /* RLP control block  */
)
{
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* RLP-specific control block  */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a pointer to the Control Block of the RLP & get it's RLP ID
   ------------------------------------------------------------------------*/
  rrcb_ptr = (dsrlpi_rlp_ctl_blk_type*)param;

  DATA_RLP_MSG4 (MSG_LEGACY_MED, "RLP[%d,%d] V[%d,%d] Reset Timer Exp", 
                  rrcb_ptr->cfg_ptr->sr_id,
                  "AB"[(rrcb_ptr->route)],
                  rrcb_ptr->cfg_ptr->ver,
                  rrcb_ptr->cfg_ptr->enhanced );
  
  (void) rex_clr_timer( &rrcb_ptr->reset_timer);

  /*-------------------------------------------------------------------------
    Send a command to CM to tear down the connection.
   ------------------------------------------------------------------------*/
  hdrrlp_reset_timeout();

} /* hdrrlp_reset_timer_cb */



#ifdef FEATURE_DATA_RL_FLOW_CONTROL
/*===========================================================================

FUNCTION HDRRLP_RLFC_DELAY_TIMER_CB

DESCRIPTION
  This function is to be called to handle reverse link flow control delay timer
  timeout

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_rlfc_delay_timer_cb
(
  uint32 param /* NULL */
)
{
#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
  ds707_ccm_arb_rmac_type* ds707_ccm_arb_rmac_info_ptr = NULL;
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */
  ds707_ccm_rlfc_state_enum_type rlfc_state = RLFC_OFF;

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
  ds707_ccm_arb_rmac_info_ptr = ds707_get_ccm_arb_rmac_info_ptr();
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */
  rlfc_state = ds707_get_ccm_rlfc_state();
  if (rlfc_state == RLFC_DELAY)
  {
#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
    ds707_ccm_arb_rmac_info_ptr->enable_cpu_flow_control = TRUE;
    //RL flow control enable
    hdrl1_rmac_flow_control( ds707_ccm_arb_rmac_info_ptr->payload,
                          ds707_ccm_arb_rmac_info_ptr->enable_cpu_flow_control );
#else
    hdrl1_rmac_start_flow_control();
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */
    ds707_set_ccm_rlfc_state(RLFC_ON);
    DATA_RLP_MSG0 (MSG_LEGACY_HIGH, 
                   "CCM enable RMAC flow control");
  }
  else if (rlfc_state == RLFC_FREEZE)
  {
    rex_set_timer( &hdrrlp_rlfc_delay_timer,
    DS707_CCM_CPU_REPORT_PERIOD_DEFAULT * 50 + DS707_CCM_RLFC_DELAY_TIMER_DELTA_MSEC);
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, 
                  "CCM set RL flow control delay timer: %d state: %d", 
                  DS707_CCM_CPU_REPORT_PERIOD_DEFAULT * 50 + DS707_CCM_RLFC_DELAY_TIMER_DELTA_MSEC,
                  rlfc_state); 
  }

  
} /* hdrrlp_rlfc_delay_timer_cb */
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */

/*===========================================================================

FUNCTION HDRRLP_STARTUP

DESCRIPTION
  This function is to be called only once per power-up of the phone. The
  function initalizes the timers for logging RLP statistics as well as the
  RLP flush timer.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_startup( void )
{
  uint8 rscb_index;                         /* Index to RLP control block */
  dsrlpi_sess_ctl_blk_type *rscb_ptr;       /* RLP-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;       /* RLP-specific control block */
  int p;
  int i;                                    /* loop counter               */

/*- - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*-------------------------------------------------------------------------
      Initialize critical section.
    -------------------------------------------------------------------------*/

   DS_INIT_CRIT_SECTION( &dsrlp_cfg_activity_crit_sect );
   DS_INIT_CRIT_SECTION( &hdrrlp_enhanced_dormant_bitmap_crit_sect);

   hdrrlp_util_init();

  /*-------------------------------------------------------------------------
    Define timers per RLP instance.

  -------------------------------------------------------------------------*/
  for (rscb_index = DSRLP_HDR_BASE_INDEX;
       rscb_index < (DSRLP_MAX_NUM_HDR_RLP+DSRLP_HDR_BASE_INDEX); 
       rscb_index++ )
  {
    rscb_ptr   = &(dsrlpi_rscb_array[rscb_index]);
    /*-----------------------------------------------------------------------
      Enter critical section to protect RLP state info.
    -----------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&dsrlp_cfg_activity_crit_sect);
    rscb_ptr->cfg.activity_bitmap = 0;
    rscb_ptr->cfg.use_default_inactivity_timer_val = TRUE;
    rscb_ptr->cfg.enable_inactivity_timer = TRUE;
    /*-----------------------------------------------------------------------
      Leave critical section.
    -----------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&dsrlp_cfg_activity_crit_sect);
    rex_def_timer_ex( &rscb_ptr->cfg.inactivity_timer, 
                      hdrrlp_inactivity_timer_cb, 
                      (uint32)rscb_ptr );
    hdrrlp_cta_clear_enhanced_idle_bit( rscb_ptr->cfg.sr_id, 
                                        rscb_ptr->cfg.ver, 
                                        rscb_ptr->cfg.enable_inactivity_timer );
    DATA_RLP_MSG2( MSG_LEGACY_HIGH, "Enhanced_cta hdrrlp_startup() define the inactivity timer for sr_id: %d  ver: %d",
									rscb_ptr->cfg.sr_id,
                  rscb_ptr->cfg.ver );

    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      rrcb_ptr = &rscb_ptr->rlp_list[p];

      rex_def_timer_ex( &rrcb_ptr->flush_timer, 
                      hdrrlp_flush_timer_cb, 
                        (uint32)rrcb_ptr );
      rrcb_ptr->state.flush_timer_expired = FALSE;

      rex_def_timer_ex( &rrcb_ptr->reset_timer, 
                      hdrrlp_reset_timer_cb, 
                        (uint32)rrcb_ptr );
    }
  }

  /*-------------------------------------------------------------------------
    Initialize the free packet info pointer queue
   ------------------------------------------------------------------------*/
  (void)q_init(&hdrrlp_pkt_info_free_q);
  for (i = 0; i < HDRRLP_MAX_PKT_INFO_ENTRIES; i++)
  {
    q_put(&hdrrlp_pkt_info_free_q,
          q_link(&hdrrlp_pktinfo_pool[i], &hdrrlp_pktinfo_pool[i].link));
  }

  /*-------------------------------------------------------------------------
    Initialize the HDR RLP Handle Management modules
   ------------------------------------------------------------------------*/
  hdrrlptxph_powerup_init();
  hdrrlptxsh_powerup_init();
  
  /*-------------------------------------------------------------------------
    Initialize the HDR RLP Retransmit Module
   ------------------------------------------------------------------------*/
  hdrrlprtx_powerup_init();
  
  /*-------------------------------------------------------------------------
    Initialize the HDR RLP transmitted queue module startup.
   ------------------------------------------------------------------------*/
  hdrrlptxq_powerup_init();

#if defined(FEATURE_HDRRLP_LOGGING)
  hdrrlplog_log_startup();
#endif /* defined(FEATURE_HDRRLP_LOGGING) */
 
#ifdef FEATURE_DATA_RL_FLOW_CONTROL
  rex_def_timer_ex( &hdrrlp_rlfc_delay_timer, 
                    hdrrlp_rlfc_delay_timer_cb, 
                    (uint32)NULL );
#endif
} /* hdrrlp_startup */


/*===========================================================================

FUNCTION HDRRLP_INIT

DESCRIPTION
  This function registers RLP callbacks with PCP and starts the statistics 
  timers associated with the particular RLP
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_init
(
  uint8 rlp_id
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* RLP-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;        /* RLP-specific control block */
  uint8                     rscb_index;      /* index in the rscb array where
                                                this record will be stored */
  uint8 rtcmac_flow_id;    /* The RTCMAC Flow ID corresponding to this RLP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT((rlp_id == DSRLP_HDR_SN_RLP_SR_ID_VALUE) ||
         (rlp_id == DSRLPI_HDR_AN_AUTH_SR_ID_VALUE));

  /*-------------------------------------------------------------------------
  Select the index in dsrlpi_rscb_array where the record for this sr_id
  will be stored. 
  -------------------------------------------------------------------------*/
  rscb_index = dsrlp_get_rlp_rscb_index( rlp_id, DSRLP_VER_HDR );
  ASSERT( rscb_index < DSRLP_MAX_RSCB_ELEMENTS );

  /*-------------------------------------------------------------------------
    Below check will be useful if ASSERT is removed
  -------------------------------------------------------------------------*/
  if(rscb_index >= DSRLP_MAX_RSCB_ELEMENTS )
  {
    DATA_RLP_MSG1 (MSG_LEGACY_ERROR, "RLP ID %d not initialized",rlp_id); 
    return;
  }
  
  rscb_ptr   = &(dsrlpi_rscb_array[rscb_index]);
  
  /* Populate the stream id which is part of the rscb_ptr */
  if( rlp_id == (uint8)DSRLP_HDR_SN_RLP_SR_ID_VALUE )
  {
    (void) hdrcp_stream_get_stream( HDRHAI_DEF_PACKET_APP_SN, &(rscb_ptr->cfg).stream_id );
  }
  else if( rlp_id == (uint8)DSRLPI_HDR_AN_AUTH_SR_ID_VALUE )
  {
    (void) hdrcp_stream_get_stream( HDRHAI_DEF_PACKET_APP_AN, &(rscb_ptr->cfg).stream_id );
  }
  
  /* The The RTCMAC Flow corresponding to this stream and RLP ID */
  rtcmac_flow_id = hdrpac_get_rtcmac_flow( (rscb_ptr->cfg).stream_id, rlp_id );

  if( rtcmac_flow_id == HDRCP_PCP_INVALID_MAC_FLOW_ID )
  {
    DATA_RLP_MSG2 (MSG_LEGACY_ERROR, 
                    "RTCMAC Flow for stream %d RLP ID %d invalid",
                    (rscb_ptr->cfg).stream_id, rlp_id); 
    return;
  }

  /* DPA only uses one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-------------------------------------------------------------------------
    Register appropriate callback with PCP/HDRRX to send & receive data.
    note that the PCP callbacks are registered before the hdrrx callbacks.
    This is because the rx callbacks result in the PCP callbacks getting
    called (when not in data optimized path). If we initialize the HDRRX
    callbacks first, data may be received before the PCP callbacks are setup.

    NOTE:
    For DO-Rev0 RLP we dont need the rx buf empty call back, so register NULL
   ------------------------------------------------------------------------*/
   
  (void) hdrcp_pcp_register_receive_callbacks( (int)(rscb_ptr->cfg).stream_id,
                                     hdrrlp_put_pkt_cb,
                                     hdrrlp_end_of_slot_cb,
                                     (hdrcp_pcp_rx_buf_empty_notification_func_type) NULL,
                                     rrcb_ptr );

  (void) hdrcp_pcp_register_transmit_callbacks( rlp_id,
                                      rtcmac_flow_id,
                                      (int)(rscb_ptr->cfg).stream_id,
                                      hdrrlp_get_pending_tx_data_queue_size, 
                                      hdrrlp_get_retx_data, 
                                      hdrrlp_get_new_or_flush_data,
                                      hdrrlp_tx_status_cb,
                                      hdrrlp_tx_failed_cb,
                                      FALSE,
                                      rrcb_ptr );
  

} /* hdrrlp_init() */

/*===========================================================================

FUNCTION HDRRLP_RESET

DESCRIPTION
  Does the HDR RLP specific reset. Note that it should only be called from
  dsrlpi_reset, since the rest of the reset is done in dsrlpi_reset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  uint8                           rscb_index; /* Index in rscb array       */ 
  rscb_index = dsrlp_get_rlp_rscb_index(rrcb_ptr->cfg_ptr->sr_id, rrcb_ptr->cfg_ptr->ver);
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    rscb_index (corresponding to an sr_id)
  -------------------------------------------------------------------------*/
  if(rscb_index >= DSRLPI_INVALID_RSCB_INDEX)
  {
    DATA_RLP_MSG1( MSG_LEGACY_HIGH, "dsrlp_cleanup called with invalid sr_id: %d", 
      rrcb_ptr->cfg_ptr->sr_id );
    return;
  }
  
  /* Clear the flush timer since the tx queue has been reset in dsrlpi_reset() */
  (void) rex_clr_timer( &rrcb_ptr->flush_timer );
  rrcb_ptr->state.flush_timer_expired = FALSE;
  if (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
  {
	(void) rex_clr_timer( &rrcb_ptr->flush_giveup_timer );
	rrcb_ptr->state.flush_giveup_timer_expired = FALSE;
  }
}
  
/*===========================================================================

FUNCTION HDRRLP_CLEANUP

DESCRIPTION
  Does the HDR RLP specific cleanup. note that it shoudl only be called from
  dsrlp_cleanup, since the rest of the cleanup is done in dsrlp_cleanup.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlp_cleanup
(
  uint8 rlp_id
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* RLP-specific control block */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;        /* RLP-specific control block */
  uint8                     rscb_index;      /* index in the rscb array where
                                                this record will be stored */
  hdrcom_hai_stream_enum_type   stream_id;       /* Stream for the RLP         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(( rlp_id == DSRLPI_HDR_AN_AUTH_SR_ID_VALUE) || 
         ( rlp_id == DSRLP_HDR_SN_RLP_SR_ID_VALUE));

  /*-------------------------------------------------------------------------
  Select the index in dsrlpi_rscb_array where the record for this sr_id
  will be stored. 
  -------------------------------------------------------------------------*/
  rscb_index = dsrlp_get_rlp_rscb_index( rlp_id, DSRLP_VER_HDR );
  rscb_ptr   = &(dsrlpi_rscb_array[rscb_index]);
  
   stream_id = (rscb_ptr->cfg).stream_id;

  /*-------------------------------------------------------------------------
    Register appropriate callback with PCP/HDRRX to send & receive data.
    note that the PCP callbacks are registered before the hdrrx callbacks.
    This is because the rx callbacks result in the PCP callbacks getting
    called (when not in data optimized path). If we initialize the HDRRX
    callbacks first, data may be received before the PCP callbacks are setup.
   ------------------------------------------------------------------------*/

  (void) hdrcp_pcp_unregister_transmit_callbacks( (int)stream_id, rlp_id );
  
  (void) hdrcp_pcp_unregister_receive_callbacks( (int)stream_id );  
  
  /* DPA only uses one route */
  ASSERT(rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE &&
         rscb_ptr->cfg.ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  rrcb_ptr = &rscb_ptr->rlp_list[DSRSP_ROUTE_A];

  /*-----------------------------------------------------------------------
    Set the RLP state to NULL.
  -----------------------------------------------------------------------*/
  rrcb_ptr->state.state = DSRLPI_NULL_STATE;
 
  /*-------------------------------------------------------------------------
   Ensure that the reset timer is not running.
   ------------------------------------------------------------------------*/
  (void) rex_clr_timer(&rrcb_ptr->reset_timer);
  
  (void) rex_clr_timer( &rrcb_ptr->flush_timer );
  rrcb_ptr->state.flush_timer_expired = FALSE;
  
#if defined(FEATURE_HDRRLP_LOGGING)
  /*-------------------------------------------------------------------------
    Stop the statistics timers.
    Also, send the stats to the diag tool to keep it in sync.
   ------------------------------------------------------------------------*/
  dsrlp_clear_stats(rscb_index);
  hdrrlp_log_statistics( rscb_index );

#endif   /* FEATURE_HDRRLP_LOGGING */

#if defined(FEATURE_HDR_DSMREUSE_OPT)   
  /*-------------------------------------------------------------------------
    Clean up the Reusable DSM items 
  -------------------------------------------------------------------------*/
  HDRDSM_REUSE_DATA_PATH_ITEMS_CLEANUP();
#endif /* (FEATURE_HDR_DSMREUSE_OPT) */

  /*-------------------------------------------------------------------------
    Make sure that the PS task won't try to flow control FL once we leave
    the HDR system.
  --------------------------------------------------------------------------*/
} /* hdrrlp_cleanup() */

#if defined(FEATURE_HDRRLP_LOGGING)
/*===========================================================================

FUNCTION HDRRLP_RX_FRAME_LOG_INIT

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
log_hdrrlp_pkt_type * hdrrlp_rx_frame_log_init
(  
  void
)
{
  log_hdrrlp_pkt_type *frm_ptr = NULL; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Check if it is OK to log 
  -----------------------------------------------------------------------*/
  if (log_status(hdrrlp_fr_rx_log.hdr.code))
  {
    /*---------------------------------------------------------------------
       If exceeded the log buffer threshold, send the log right away 
    ---------------------------------------------------------------------*/
    if (hdrrlp_fr_rx_log.hdr.len >= HDRRLP_LOG_RLP_RX_FRM_BUF_THRESHOLD )
    {
      hdrrlp_rx_send_frames_log();
    }
    
    /* lint -e413 lint error reported due to FPOS defn in comdef.h*/
    frm_ptr = (log_hdrrlp_pkt_type *)
      &hdrrlp_fr_rx_log.hdrrlp_log_buf.logbuf[RXC_HDRRLP_FRAMES_OFFSET];
    /* lint +e413 */

    if (frm_ptr == &hdrrlp_fr_rx_log.hdrrlp_log_buf.pkt[0])
    {
      /*-------------------------------------------------------------------
       If it is the first packet, store the time stamp
      -------------------------------------------------------------------*/
      (void)time_get((unsigned long *) hdrrlp_fr_rx_log.hdr.ts);
    }

    /*---------------------------------------------------------------------
       Update the log header length 
    ---------------------------------------------------------------------*/
    hdrrlp_fr_rx_log.hdr.len += FPOS(log_hdrrlp_pkt_type, hdrrlp_payload);

    /*---------------------------------------------------------------------
       Also the log count
    ---------------------------------------------------------------------*/
    hdrrlp_fr_rx_log.count++;

    /*---------------------------------------------------------------------
       Take care of the reserved bit
    ---------------------------------------------------------------------*/
    frm_ptr->pkt_info.rsvd  = 0;
    
  }
  return frm_ptr;

} /* hdrrlp_rx_frame_log_init */

/*===========================================================================

FUNCTION HDRRLP_TX_FRAME_LOG_INIT

DESCRIPTION
  This function initializes the next frame packet, as well as the
  pointer to it, in the RLP frames log buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
log_hdrrlp_pkt_type * hdrrlp_tx_frame_log_init
(
  void
)
{
  log_hdrrlp_pkt_type *frm_ptr = NULL;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Check if it is OK to log 
  -----------------------------------------------------------------------*/
  if (log_status(hdrrlp_fr_tx_log.hdr.code))
  {
    /*---------------------------------------------------------------------
      If exceeded the log buffer threshold, send the log right away
    ---------------------------------------------------------------------*/
    if (hdrrlp_fr_tx_log.hdr.len >= HDRRLP_LOG_RLP_TX_FRM_BUF_THRESHOLD)
    {
      hdrrlp_tx_send_frames_log();
    }

   
    frm_ptr = (log_hdrrlp_pkt_type *)
      &hdrrlp_fr_tx_log.hdrrlp_log_buf.logbuf[TXC_HDRRLP_FRAMES_OFFSET];

    /*---------------------------------------------------------------------
      If it is the first packet, store the time stamp
    ---------------------------------------------------------------------*/
    if (frm_ptr == &hdrrlp_fr_tx_log.hdrrlp_log_buf.pkt[0])
    {      
      (void)time_get((unsigned long *) hdrrlp_fr_tx_log.hdr.ts);
    }

    /*---------------------------------------------------------------------
      Update the log buffer header length accordingly
    ---------------------------------------------------------------------*/
    hdrrlp_fr_tx_log.hdr.len += FPOS(log_hdrrlp_pkt_type, hdrrlp_payload);

    /*---------------------------------------------------------------------
      And also the log count
    ---------------------------------------------------------------------*/
    hdrrlp_fr_tx_log.count++;

    /*---------------------------------------------------------------------
      Take care of reserved bit
    ---------------------------------------------------------------------*/
    frm_ptr->pkt_info.rsvd  = 0;
    
  }

  return frm_ptr;

} /* hdrrlp_tx_frame_log_init() */


/*===========================================================================

FUNCTION HDRRLP_LOG_RX_RLP_FRAMES()

DESCRIPTION
  This function LOGS THE PARTIAL RX RLP FRAMES
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
void hdrrlp_log_rx_rlp_frames
(
  dsm_item_type *rlp_pkt_ptr
)
{
 
  log_hdrrlp_pkt_type *log_hdrrlp_pkt_ptr;  /* ptr to partical rlp frame */

/* - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (rlp_pkt_ptr == NULL ) 
  {
    return;
  }

  log_hdrrlp_pkt_ptr =  hdrrlp_rx_frame_log_init();

  if ( log_hdrrlp_pkt_ptr == NULL)
  {
    return;
  }
  else
  {

    /*---------------------------------------------------------------------
      Log rx DRC
    ---------------------------------------------------------------------*/
    log_hdrrlp_pkt_ptr->pkt_info.phy_chan_rate = (uint16)HDRCOM_DSM_GET_DRC(rlp_pkt_ptr);

    /*---------------------------------------------------------------------
      Log rx Chan bit
    ---------------------------------------------------------------------*/
    log_hdrrlp_pkt_ptr->pkt_info.channel = (uint16)HDRCOM_DSM_GET_CHAN_BIT(rlp_pkt_ptr);

    /*---------------------------------------------------------------------
      Log RLP packet length
    ---------------------------------------------------------------------*/
    log_hdrrlp_pkt_ptr->pkt_info.pkt_len = rlp_pkt_ptr->used;

    /*---------------------------------------------------------------------
      Log HSTR
    ---------------------------------------------------------------------*/
    log_hdrrlp_pkt_ptr->time_ref.STR = (uint8)hdrrlplog_get_hstr_count();

    /*---------------------------------------------------------------------
      To take care of the small RLP packets
    ---------------------------------------------------------------------*/
    if(log_hdrrlp_pkt_ptr->pkt_info.pkt_len >= LOG_MAX_HDRRLP_RX_HEADER_SIZE)
    {
      memscpy((void*)log_hdrrlp_pkt_ptr->hdrrlp_payload,
	      sizeof(log_hdrrlp_pkt_ptr->hdrrlp_payload),
	      rlp_pkt_ptr->data_ptr,
                           hdrrlp_fr_tx_log.pkt_block_info.header_len);
      hdrrlp_fr_rx_log.hdr.len += hdrrlp_fr_rx_log.pkt_block_info.header_len;
    }
    else
    {
      memscpy((void*)log_hdrrlp_pkt_ptr->hdrrlp_payload,
	      sizeof(log_hdrrlp_pkt_ptr->hdrrlp_payload),
              rlp_pkt_ptr->data_ptr,
                           log_hdrrlp_pkt_ptr->pkt_info.pkt_len);
      hdrrlp_fr_rx_log.hdr.len +=  log_hdrrlp_pkt_ptr->pkt_info.pkt_len ;
    }

  }
    
} /* hdrrlp_log_rx_rlp_frames() */

/*===========================================================================

FUNCTION HDRRLP_LOG_TX_RLP_FRAMES()

DESCRIPTION
  This function LOGS THE PARTIAL TX RLP FRAMES
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RLP partial frames log may be sent.

===========================================================================*/
void hdrrlp_log_tx_rlp_frames
(
  dsm_item_type *rlp_pkt_ptr
)
{

  log_hdrrlp_pkt_type *log_hdrrlp_pkt_ptr;  /* ptr to partical rlp frame */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --  - */

  if (rlp_pkt_ptr == NULL ) 
  {
    return;
  }

  log_hdrrlp_pkt_ptr =  hdrrlp_tx_frame_log_init();

  if ( log_hdrrlp_pkt_ptr == NULL)
  {
    return;
  }
  else
  {
    /*---------------------------------------------------------------------
      Get the Tx DRC, currently this is may not be a valid value 
    ---------------------------------------------------------------------*/
    log_hdrrlp_pkt_ptr->pkt_info.phy_chan_rate = (uint16)HDRCOM_DSM_GET_DRC(rlp_pkt_ptr);

    /*---------------------------------------------------------------------
      Get the Channel bin 
    ---------------------------------------------------------------------*/
    log_hdrrlp_pkt_ptr->pkt_info.channel = (uint16)HDRCOM_DSM_GET_CHAN_BIT(rlp_pkt_ptr);

    /*---------------------------------------------------------------------
      Store the length of the RLP packet
    ---------------------------------------------------------------------*/
    log_hdrrlp_pkt_ptr->pkt_info.pkt_len = (uint16)dsm_length_packet(rlp_pkt_ptr);

    /*---------------------------------------------------------------------
      Log the tx frame count, actually it is the time_since_prev
    ---------------------------------------------------------------------*/
    log_hdrrlp_pkt_ptr->time_ref.frame_count = hdrrlp_log_tx_frame_count;

    /*---------------------------------------------------------------------
      Reset it, till it is again set
    ---------------------------------------------------------------------*/
    hdrrlp_log_tx_frame_count = 0;

    /*---------------------------------------------------------------------
      To take care of the Flush packet tx scenario & and also Re-xmit
    ---------------------------------------------------------------------*/
    if(log_hdrrlp_pkt_ptr->pkt_info.pkt_len >= 
                                       LOG_MAX_HDRRLP_TX_HEADER_SIZE)
    {
      (void) dsm_extract((void *)(rlp_pkt_ptr),0,(void *)log_hdrrlp_pkt_ptr->
                hdrrlp_payload,hdrrlp_fr_tx_log.pkt_block_info.header_len);
      hdrrlp_fr_tx_log.hdr.len += 
                                hdrrlp_fr_tx_log.pkt_block_info.header_len;
 
    }
    else
    {
      (void) dsm_extract((void *)(rlp_pkt_ptr),0,(void *)log_hdrrlp_pkt_ptr->
                hdrrlp_payload,log_hdrrlp_pkt_ptr->pkt_info.pkt_len);
      hdrrlp_fr_tx_log.hdr.len += log_hdrrlp_pkt_ptr->pkt_info.pkt_len;
    }
  }
} /* hdrrlp_log_tx_rlp_frames() */

#endif  /* FEATURE_HDRRLP_LOGGING  */

/*===========================================================================
FUNCTION      HDRRLP_CTA_SET_ENHANCED_IDLE_BIT

DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_cta_set_enhanced_idle_bit
(
  uint8 rlp_id,
  dsrlp_ver_enum_type ver
)
{
  uint32 hi = 0;
  uint32 lo = 0;
  uint32 mask = 0;
 

  if ( rlp_id <= DSRLP_MAX_HDR_MRLP_FLOW_NUMBER - 1)
  {
    mask = 0x80000000 >> rlp_id;

    /*-----------------------------------------------------------------------
      Enter critical section to protect RLP state info.
    -----------------------------------------------------------------------*/

    DS_ENTER_CRIT_SECTION( &hdrrlp_enhanced_dormant_bitmap_crit_sect );
	  hi = qw_hi(hdrrlp_enhanced_dormant_bitmap);
      lo = qw_lo(hdrrlp_enhanced_dormant_bitmap);
      if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD || ver == DSRLP_VER_HDR)
	  {	
	    hi |= mask;
	  }
	  else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
	  {
            lo |= mask;
	  }
      qw_set(hdrrlp_enhanced_dormant_bitmap, hi, lo);
    /*-----------------------------------------------------------------------
     Leave critical section.
    -----------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION( &hdrrlp_enhanced_dormant_bitmap_crit_sect );

    if (ver == DSRLP_VER_HDR)
	{
	  DATA_RLP_MSG1( MSG_LEGACY_HIGH,"Enhanced_cta Rel0 RLP %d is busy",rlp_id);
	}
    else if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
	{	
	  DATA_RLP_MSG1( MSG_LEGACY_HIGH, "Enhanced_cta Fwd RLP %d is busy",rlp_id );
	}
	else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
	{
          DATA_RLP_MSG1( MSG_LEGACY_HIGH, "Enhanced_cta Rev RLP %d is busy",rlp_id );
	}
    DATA_RLP_MSG2( MSG_LEGACY_HIGH, "Enhanced_cta hdrrlp_enhanced_dormant_bitmap High:0x%x Low:0x%x", 
    hi, lo );
  }
} /* hdrrlp_cta_set_enhanced_idle_bit */

/*===========================================================================
FUNCTION      HDRRLP_CTA_CLEAR_ENHANCED_IDLE_BIT

DESCRIPTION   This function clears enhanced bitmap 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_cta_clear_enhanced_idle_bit
(
  uint8 rlp_id,
  dsrlp_ver_enum_type ver,
  boolean enable_inactivity_timer
)
{
   uint32 hi = 0;
   uint32 lo = 0;
   uint32 mask = 0;
   boolean   go_dormant = FALSE;
   int result = 0;
   qword temp;

   if ( rlp_id <= DSRLP_MAX_HDR_MRLP_FLOW_NUMBER - 1 )
   {
     mask = 0x80000000 >> rlp_id;
     qw_set(temp, hi, lo);
    /*-----------------------------------------------------------------------
      Enter critical section to protect RLP state info.
    -----------------------------------------------------------------------*/

     DS_ENTER_CRIT_SECTION( &hdrrlp_enhanced_dormant_bitmap_crit_sect );
     hi = qw_hi(hdrrlp_enhanced_dormant_bitmap);
     lo = qw_lo(hdrrlp_enhanced_dormant_bitmap);
     if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD || ver == DSRLP_VER_HDR)
     {
       hi &= ~mask;
     }
     else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
     {
       lo &= ~mask;  
     }
     result = qw_cmp(hdrrlp_enhanced_dormant_bitmap, temp);
     if (( hi == 0 ) && 
         ( lo == 0 ) && 
         ( result != 0 ) && 
         ( enable_inactivity_timer == TRUE ))
     {
       /* Need to go dormant */
       go_dormant = TRUE;
     }
     qw_set(hdrrlp_enhanced_dormant_bitmap, hi, lo);
     /*-----------------------------------------------------------------------
      Leave critical section.
     -----------------------------------------------------------------------*/
     DS_LEAVE_CRIT_SECTION( &hdrrlp_enhanced_dormant_bitmap_crit_sect );
     if (go_dormant)
     {
       /*in QTF test cases, hdrrx task is not started so dont set signal*/
#ifndef TEST_FRAMEWORK
       (void) rex_set_sigs( hdrrlp_util_get_hdrrx_tcb() , HDRRX_RLP_CTA_EXP_SIG );
#endif
     }
     if (ver == DSRLP_VER_HDR)
     {
       DATA_RLP_MSG1( MSG_LEGACY_HIGH,"Enhanced_cta Rel0 RLP %d is Idle",rlp_id);
     }
     else if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
     {
       DATA_RLP_MSG1( MSG_LEGACY_HIGH, "Enhanced_cta Fwd RLP %d is Idle",rlp_id );
     }
     else if (ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
     {
       DATA_RLP_MSG1( MSG_LEGACY_HIGH, "Enhanced_cta Rev RLP %d is Idle",rlp_id );
     }
     DATA_RLP_MSG2( MSG_LEGACY_HIGH, "Enhanced_cta hdrrlp_enhanced_dormant_bitmap High:0x%x Low:0x%x", hi, lo );
   }
} /* hdrrlp_cta_clear_enhanced_idle_bit */

/*===========================================================================
FUNCTION      HDRRLP_DATA_ACTIVITY_UPDATE

DESCRIPTION   This function is called from the rlp layer, every TX/RX interrupt If the
              input parameter is FALSE, then no data frames have been received in the 
			  past TX/RX inerrupt.  Otherwise at least one data frame was received or 
			  trasmitted.  If data has been data activity, then the idle timer should 
			  be reset, so that it doesn't expire.
  
              This function should be called from the RX/TX task.

DEPENDENCIES  

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_data_activity_update
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void) rex_clr_timer( &rscb_ptr->cfg.inactivity_timer );
  if (rscb_ptr->cfg.use_default_inactivity_timer_val == TRUE)
  {
    if ( hdrrlp_default_cta_val != 0 )
  	{  
	    (void) rex_set_timer( &rscb_ptr->cfg.inactivity_timer, hdrrlp_default_cta_val * 1000 );
	  }
  }
  else if (rscb_ptr->cfg.inactivity_timer_val != 0xFFFFFFFF)
  {
     /* 0xFFFFFFFF is reserved for infinity */
     (void) rex_set_timer( &rscb_ptr->cfg.inactivity_timer, rscb_ptr->cfg.inactivity_timer_val);
  }

  if (rscb_ptr->cfg.activity_bitmap == 0)
  {
    hdrrlp_cta_set_enhanced_idle_bit(rscb_ptr->cfg.sr_id, rscb_ptr->cfg.ver);
    /*-----------------------------------------------------------------------
      Enter critical section to protect RLP state info.
    -----------------------------------------------------------------------*/

    DS_ENTER_CRIT_SECTION(&dsrlp_cfg_activity_crit_sect);
    rscb_ptr->cfg.activity_bitmap = 1;
    /*-----------------------------------------------------------------------
      Leave critical section.
    -----------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&dsrlp_cfg_activity_crit_sect);
  }
} /* hdrrlp_data_activity_update() */

/*===========================================================================
FUNCTION      HDRRLP_PHY_LINK_DOWN_INACTIVITY_TIMER_HDLR

DESCRIPTION   This function is called from DS to handle Physical Link Down event for RLP 
              Inactivity timer.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_phy_link_down_inactivity_timer_hdlr(void)
{
  uint8 rscb_index;                         /* Index to RLP control block */
  dsrlpi_sess_ctl_blk_type *rscb_ptr;   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (rscb_index = DSRLP_HDR_BASE_INDEX; 
       rscb_index < (DSRLP_MAX_NUM_HDR_MRLP_REV + DSRLP_MAX_NUM_HDR_MRLP_FWD + DSRLP_MAX_NUM_HDR_RLP + DSRLP_HDR_BASE_INDEX);  
       rscb_index++ )
  {
    rscb_ptr   = &(dsrlpi_rscb_array[rscb_index]);
    if( rscb_ptr->is_hdrmrlp_active == TRUE || rscb_ptr->cfg.ver == DSRLP_VER_HDR )
    {
      /*-----------------------------------------------------------------------
        Enter critical section to protect RLP state info.
      -----------------------------------------------------------------------*/

      DS_ENTER_CRIT_SECTION( &dsrlp_cfg_activity_crit_sect );
      rscb_ptr->cfg.activity_bitmap = 0;
      rscb_ptr->cfg.enable_inactivity_timer = FALSE;
      /*-----------------------------------------------------------------------
       Leave critical section.
      -----------------------------------------------------------------------*/
      DS_LEAVE_CRIT_SECTION( &dsrlp_cfg_activity_crit_sect );
      (void)rex_clr_timer( &rscb_ptr->cfg.inactivity_timer );
      hdrrlp_cta_clear_enhanced_idle_bit(rscb_ptr->cfg.sr_id, 
                                         rscb_ptr->cfg.ver, 
                                         rscb_ptr->cfg.enable_inactivity_timer );
      DATA_RLP_MSG3( MSG_LEGACY_HIGH, 
                     "Enhanced_cta phy link DOWN clear the bitmap for sr_id: %d ver:%d flag: %d", 
                     rscb_ptr->cfg.sr_id, 
                     rscb_ptr->cfg.ver, 
                     rscb_ptr->cfg.enable_inactivity_timer);    
    }
  }
} /* hdrrlp_phy_link_down_inactivity_timer_hdlr */

/*===========================================================================
FUNCTION      HDRRLP_PHY_LINK_UP_INACTIVITY_TIMER_HDLR

DESCRIPTION   This function is called from DS to handle Physical Link Up event for RLP 
              Inactivity timer.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_phy_link_up_inactivity_timer_hdlr(void)
{
  uint8 rscb_index;                         /* Index to RLP control block */
  dsrlpi_sess_ctl_blk_type *rscb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (rscb_index = DSRLP_HDR_BASE_INDEX; 
       rscb_index < (DSRLP_MAX_NUM_HDR_MRLP_REV + DSRLP_MAX_NUM_HDR_MRLP_FWD + DSRLP_MAX_NUM_HDR_RLP + DSRLP_HDR_BASE_INDEX); 
       rscb_index++ )
  {
    rscb_ptr   = &(dsrlpi_rscb_array[rscb_index]);
    if( rscb_ptr->is_hdrmrlp_active == TRUE  || rscb_ptr->cfg.ver == DSRLP_VER_HDR )
    {
      /*-----------------------------------------------------------------------
        Enter critical section to protect RLP state info.
      -----------------------------------------------------------------------*/
      DS_ENTER_CRIT_SECTION(&dsrlp_cfg_activity_crit_sect);
      rscb_ptr->cfg.activity_bitmap = 1;
      rscb_ptr->cfg.enable_inactivity_timer = TRUE;
      /*-----------------------------------------------------------------------
        Leave critical section.
      -----------------------------------------------------------------------*/
      DS_LEAVE_CRIT_SECTION(&dsrlp_cfg_activity_crit_sect);
      (void) rex_clr_timer( &rscb_ptr->cfg.inactivity_timer );
      if (rscb_ptr->cfg.use_default_inactivity_timer_val == TRUE)
      {
        if (hdrrlp_default_cta_val != 0)
        {
          (void) rex_set_timer( &rscb_ptr->cfg.inactivity_timer, hdrrlp_default_cta_val * 1000 );
          DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Enhanced_cta phy link UP start inactivity timer (default):%d", hdrrlp_default_cta_val * 1000, 
			        rscb_ptr->cfg.sr_id, rscb_ptr->cfg.ver);
        }
      }
      else if (rscb_ptr->cfg.inactivity_timer_val != 0xFFFFFFFF)
      {
	    /* 0xFFFFFFFF is reserved for infinity */
        (void) rex_set_timer( &rscb_ptr->cfg.inactivity_timer, rscb_ptr->cfg.inactivity_timer_val);
        DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Enhanced_cta phy link UP start inactivity timer:%d", rscb_ptr->cfg.inactivity_timer_val, 
			      rscb_ptr->cfg.sr_id, rscb_ptr->cfg.ver);
      }
      hdrrlp_cta_set_enhanced_idle_bit(rscb_ptr->cfg.sr_id, rscb_ptr->cfg.ver);
      DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Enhanced_cta phy link UP set the bitmap for sr_id: %d ver:%d ", 
      rscb_ptr->cfg.sr_id, rscb_ptr->cfg.ver);
    }
  }
} /* hdrrlp_phy_link_up_inactivity_timer_hdlr */


/*===========================================================================
FUNCTION      HDRRLP_RESV_BOUND_TO_RLP_INACTIVITY_TIMER_HDLR

DESCRIPTION   This function is called from DS, to handle RLP inactivity timer ,
              when reservation bound to RLP 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_resv_bound_to_rlp_inactivity_timer_hdlr
(
  uint8  rscb_index,
  uint32 ds_flow_inactivity_val
)
{
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);
  /*--------------------------------------------------------------------
    Set the inactivity timer value for the RLP as the largest inactivity 
    timervalue for all DS flows bind to it    
   --------------------------------------------------------------------*/
  if (rscb_ptr->cfg.use_default_inactivity_timer_val == FALSE &&
      rscb_ptr->cfg.inactivity_timer_val < ds_flow_inactivity_val )
  {
    rscb_ptr->cfg.inactivity_timer_val = ds_flow_inactivity_val;
	DATA_RLP_MSG3( MSG_LEGACY_MED, 
                  "Enhanced_cta resv bind update the inactivity timer %d for RLP %d Ver %d", 
	                rscb_ptr->cfg.inactivity_timer_val, 
                  rscb_ptr->cfg.sr_id, 
                  rscb_ptr->cfg.ver);
  }
  else if (rscb_ptr->cfg.use_default_inactivity_timer_val == TRUE )
  {
    rscb_ptr->cfg.inactivity_timer_val = ds_flow_inactivity_val;
    rscb_ptr->cfg.use_default_inactivity_timer_val = FALSE;
    DATA_RLP_MSG3( MSG_LEGACY_MED,
                    "Enhanced_cta resv bind update the inactivity timer %d for RLP %d by Ver %d, overwrite default value", 
                    rscb_ptr->cfg.inactivity_timer_val, 
                    rscb_ptr->cfg.sr_id, 
                    rscb_ptr->cfg.ver);
  }
} /* hdrrlp_resv_bound_to_rlp_inactivity_timer_hdlr */

/*===========================================================================
FUNCTION      HDRRLP_RESV_UNBOUND_TO_RLP_INACTIVITY_TIMER_HDLR

DESCRIPTION   This function is called from DS, to handle RLP inactivity timer ,
              when reservation bound to RLP 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_resv_unbound_from_rlp_inactivity_timer_hdlr
(
  uint8  rscb_index,
  uint32 ds_flow_max_inactivity_timer_val
)
{
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(rscb_index >= DSRLP_MAX_RSCB_ELEMENTS)
  {
    DATA_RLP_MSG1( MSG_LEGACY_ERROR, 
		           "rscb_index is out of bounds. Index val is %d", rscb_index );
	return;
  }
  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);
  /*--------------------------------------------------------------------
    Set the inactivity timer value for the RLP as the largest inactivity 
    timervalue for all DS flows bind to it    
   --------------------------------------------------------------------*/
  if (ds_flow_max_inactivity_timer_val > 0)
  {
    rscb_ptr->cfg.inactivity_timer_val = ds_flow_max_inactivity_timer_val;
	  rscb_ptr->cfg.use_default_inactivity_timer_val = FALSE;
	  DATA_RLP_MSG3( MSG_LEGACY_MED, 
                  "Enhanced_cta resv unbind RLP %d ver %d inactivity timer %d", 
		              rscb_ptr->cfg.sr_id, 
                  rscb_ptr->cfg.ver, 
                  rscb_ptr->cfg.inactivity_timer_val);
  }
  else
  {
    /*-------------------------------------------------------------------------
      Clear the timer and set the boolean.
     ------------------------------------------------------------------------*/
    (void) rex_clr_timer( &rscb_ptr->cfg.inactivity_timer);
    /*-----------------------------------------------------------------------
      Enter critical section to protect RLP state info.
    -----------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION( &dsrlp_cfg_activity_crit_sect );
    rscb_ptr->cfg.activity_bitmap = 0;
    /*-----------------------------------------------------------------------
      Leave critical section.
    -----------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION( &dsrlp_cfg_activity_crit_sect );

    hdrrlp_cta_clear_enhanced_idle_bit( rscb_ptr->cfg.sr_id, 
                                        rscb_ptr->cfg.ver, 
                                        rscb_ptr->cfg.enable_inactivity_timer );

    rscb_ptr->cfg.use_default_inactivity_timer_val = TRUE;
    DATA_RLP_MSG3(MSG_LEGACY_MED, "Enhanced_cta resv unbind RLP %d ver %d "
                  "inactivity timer set as default %d Msec", 
                  rscb_ptr->cfg.sr_id,
                  rscb_ptr->cfg.ver,
                  hdrrlp_default_cta_val * 1000 );
  }
} /* hdrrlp_resv_unbound_from_rlp_inactivity_timer_hdlr */

/*===========================================================================
FUNCTION      HDRRLP_COPY_DEFAULT_CTA_VAL

DESCRIPTION   This function is called from DS to propagate the default CTA
              timer value to RLP
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_copy_default_cta_val(dsat_num_item_type cta_val)
{
  hdrrlp_default_cta_val = cta_val;
} /* hdrrlp_copy_default_cta_val */

/*===========================================================================
FUNCTION HDRRLP_GET_RLP_ID_FOR_DEF_APP_SUBTYPE

DESCRIPTION
  This function returns the RLP ID of the RLP that is configured for a
  particular Default Packet Application Subtype (AN or SN)
  
  It cannot be used for multiflow packet application since the a multiflow
  packet application can have multiple RLPs mapped to it.

DEPENDENCIES
  None. Note that unlike other mapping functions, this function is
  intentionally not dependant on the HDR session being opened. This is
  because the RLP ID for the SN and AN stream are fixed and will not
  change irrespective of if the session is opened or closed.

  Note that this enables the RLP to be initialized even before the
  HDR session is opened.

PARAMETERS
   app_subtype - Application Subtype who RLPID is to be determined

RETURN VALUE
  rlp_id for the RLP that is mapped to the inputted app subtype.
  The rlp_id is set to HDRRLP_INVALID_ID if no stream is mapped for the
  app_subtype.

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdrrlp_get_rlp_id_for_def_app_subtype
(
hdrcom_hai_app_subtype_enum_type app_subtype
)
{
  uint8 rlp_id = HDRRLP_INVALID_ID;  /* Application RLP ID to be returned                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( app_subtype )
  {
    case HDRHAI_DEF_PACKET_APP_SN:
      rlp_id = (uint8) DSRLP_HDR_SN_RLP_SR_ID_VALUE;
      break;
    case HDRHAI_DEF_PACKET_APP_AN:
      rlp_id = (uint8) DSRLPI_HDR_AN_AUTH_SR_ID_VALUE;
      break;
    default:
      rlp_id = HDRRLP_INVALID_ID;
      break;
  }

  return rlp_id;
} /* hdrrlp_get_rlp_id_for_def_app_subtype() */


#endif /* FEATURE_HDR */



/*===========================================================================

                      SU LEVEL API's

===========================================================================*/



/*===========================================================================

FUNCTION HDRRLP_QUEUE_MSG

DESCRIPTION
  This function builds up any pending RLP signaling messages and queues them
  directly to SLP, bypassing the conventional HMP layer queueing.  Based
  on the current state of RLP, the signaling message building can generate
  one of three signaling message types:

  RESET
    The current state of RLP is DSRLPI_RESET_STATE, in which the access 
    terminal is initiating an RLP reset.  Initialization processing is 
    performed during RX processing, which directs the TX processing to issue
    a RESET message.  RLP will remain in the RESET state until a RESET ACK 
    message has been received.

  RESET ACK
    The current state of RLP is DSRLPI_RESET_STATE, in which the access 
    terminal has received a RESET message from the access network.  In 
    response, the access terminal issues a RESET ACK signaling message, and 
    exits the DSRLPI_RESET state, into the DSRLPI_ESTABLISHED state.

  NAK
    The current state of RLP is DSRLPI_ESTABLISHED_STATE, in which the access
    terminal is issuing negative acknowledgement for outstanding RLP frames.
    
  In each case, the message is built-up, and passed directly to SLP.  SLP is
  responsible for de-allocating the dsm memory items used to build up the
  signaling message.

  In addition, since this function is called once per frame build interrupt,
  the function is used to update RLP timer values.  Timing information is 
  derived from the paramater time_since_prev_req.

DEPENDENCIES
  Assumes that a connection is open.  HMP need not know the message is being
  transmitted.

RETURN VALUE
  None

SIDE EFFECTS
  RLP signaling messages are queued on an SLP Tx queue.  

===========================================================================*/
void hdrrlp_queue_msg
(
  uint16 time_since_prev_req   /* increment of 26.667 ms since last tx req */
)
{
#ifdef FEATURE_HDR
  dsrlpi_sess_ctl_blk_type *rscb_ptr;      /* Flow-specific control block  */
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;      /* RLP-specific control block   */
  dsrlpi_state_info_type   *state_ptr;     /* local state ptr              */
  dsm_item_type            *item_ptr;      /* ptr to sig msg queued to SLP */
  uint32                   nak_id;         /* ID of NAK in NAK list        */
  uint8                    field;          /* field to insert into message */
  uint8                    hmp_hdr;        /* HMP header to append to msgs */
  uint8                    rlp_index;      /* Index to RLP control block   */
  uint8                    rlp_id;         /* RLP ID                       */
  boolean                  msgs_to_send;   /* indicates more msgs to send  */
  int                      p;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_HDRRLP_LOGGING
  /* To save for Partial RLP frame logging */
  hdrrlp_log_tx_frame_count = (uint8)time_since_prev_req;
#endif /* FEATURE_HDRRLP_LOGGING */

  /*-------------------------------------------------------------------------
    If the HDR session is not open when this function is called, then return
    immediately
  -------------------------------------------------------------------------*/
  if( !hdrcp_smp_session_is_open( HDRHAI_RADIO_LINK_PROTOCOL ) ||
      !hdrcp_stream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,"HDR session not open. Not q-ing any msgs");
    return;
  }
 
  /*-------------------------------------------------------------------------
    Determine messages to be queued for all registered RLP instances.
  -------------------------------------------------------------------------*/
  for( rlp_index = DSRLP_HDR_BASE_INDEX; 
       rlp_index < (DSRLP_HDR_BASE_INDEX + DSRLP_MAX_NUM_HDR_RLP); 
       rlp_index++ )
  {

    /*-----------------------------------------------------------------------
      Using the rlp_index_id, get the session control block and 
      fcn_ptr_table.
    -----------------------------------------------------------------------*/
    rscb_ptr      = &(dsrlpi_rscb_array[rlp_index]);
    rlp_id        = rscb_ptr->cfg.sr_id;

    /*-----------------------------------------------------------------------
      If the RLP Version pointer is NULL, then it has not been initialized,
      or if the RLP ID does not match the valid id's, skip this RLP instance 
    -----------------------------------------------------------------------*/
    if( ( rscb_ptr->cfg.ver_ptr == NULL ) ||
        ( ( rlp_id != DSRLP_HDR_SN_RLP_SR_ID_VALUE ) &&
          ( rlp_id != DSRLPI_HDR_AN_AUTH_SR_ID_VALUE ) ) )
    {
      continue;
    }

    /*-----------------------------------------------------------------------
      Determine the HMP header to add to the messages when they are queued to
      SLP based on the stream for which they are meant.
    -----------------------------------------------------------------------*/
    switch ( (rscb_ptr->cfg).stream_id )
    {
      case HDRHAI_STREAM1:
        hmp_hdr = (int)HDRRLP_STREAM1_HMP_HDR;
        break;
      case HDRHAI_STREAM2:
        hmp_hdr = (int)HDRRLP_STREAM2_HMP_HDR;
        break;
      case HDRHAI_STREAM3:
        hmp_hdr = (int)HDRRLP_STREAM3_HMP_HDR;
        break;
      default:
        DATA_RLP_MSG0( MSG_LEGACY_ERROR, "Invalid RLPID message to be queued" );
        return;
    }

    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {    
      rrcb_ptr  = &rscb_ptr->rlp_list[p];
      state_ptr = &rrcb_ptr->state;

    /*-----------------------------------------------------------------------
      Determine if there are even any signaling messages to send.
    -----------------------------------------------------------------------*/
    if (HDRRLP_SIG_MSGS_TO_SEND(state_ptr) == FALSE)
    {
      /*---------------------------------------------------------------------
        No messages to send so skip this RLP instance.
      ---------------------------------------------------------------------*/
      continue;
    }

    DATA_RLP_MSG4 (MSG_LEGACY_LOW, 
                    "RLP [%d,%d] V[%d,%d] Send RLP Sig msg for RLP ", 
                    rrcb_ptr->cfg_ptr->sr_id,
                    "AB"[(rrcb_ptr->route)],
                    rrcb_ptr->cfg_ptr->ver,
                    rrcb_ptr->cfg_ptr->enhanced);
    /*-----------------------------------------------------------------------
      Determine which type of message to send.
    -----------------------------------------------------------------------*/
    if( state_ptr->rst_to_send == TRUE  && 
        state_ptr->rst_ack_to_send == FALSE)
    {
      /*---------------------------------------------------------------------
        The mobile is initiating the RLP reset.  RLP RX-side processing has 
        indicated that a RESET message is to be transmitted.  Build up the 
        message for SLP by adding the message type.  Allocate a DSM item to 
        hold the signaling message.  Account for lower layer headers by 
        including a header offset.  Once the reset message is queued, enter 
        the RESET state, if not already in the RESET state.  As part of 
        synchronization between sender/receiver, if a reset ACK is pending, 
        we do not also want to send the reset.
      ---------------------------------------------------------------------*/
      item_ptr = dsm_offset_new_buffer
                 (
                   DSM_DS_POOL_SIZE(DSM_DS_SMALL_ITEM_SIZ),
                   HDRRLP_SIGHDR_OFFSET
                 );
      if (item_ptr == NULL)
      {
        DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                        "RLP [%d,%d] V[%d,%d] Memory Alloc Failed for RLP RST SIG MSG ", 
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced);
        return; 
      }

      field = HDRRLPI_SIG_RST;
      (void) dsm_pushdown_tail(&item_ptr,(void *)&field, 1,DSM_DS_SMALL_ITEM_POOL );

      /*---------------------------------------------------------------------
        Send the message directly to SLP without passing through HMP.  As 
        such, the HMP header must be added, prior to queueing for SLP.  This
        will then be queued directly to SLP.  SLP is responsible for freeing
        the memory items used for message building.  Note that since we have 
        created the new item with a specified offset, the dsm_pushdown() 
        will not allocate a new item.  
       --------------------------------------------------------------------*/
      field = hmp_hdr;
      (void) dsm_pushdown(&item_ptr, (void *)&field, 1, DSM_DS_SMALL_ITEM_POOL);

      /*---------------------------------------------------------------------
        Clear the RESET indication.
       --------------------------------------------------------------------*/
      state_ptr->rst_to_send = FALSE;

      /*---------------------------------------------------------------------
        Record statistics for number of AT reset requests 
       --------------------------------------------------------------------*/
        rrcb_ptr->stats.at_reset_request_count++;

      /*---------------------------------------------------------------------
        Queue directly to SLP.SLP is responsible for freeing the memory items
        used for message building.  Note that since we have created the new 
        item with a specified offset, the dsm_pushdown() will not allocate a 
        new item.  
       --------------------------------------------------------------------*/
      DATA_RLP_MSG4 (MSG_LEGACY_HIGH, 
                      "RLP[%d,%d] V[%d,%d] Queueing Reset to SLP", 
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced);

      (void) hdrcp_slp_send_msg(HDRHAI_RTC,                           /* HDR channel */
                      TRUE,                                 /* Reliable    */
                      HDRRLP_SIG_MSG_PRIORITY,     /* RLP Sig Msg Priority */
                      item_ptr,                      /* pointer to sig msg */
                      NULL,        /* callback after message has been sent */
                      NULL);                  /* user data for cb function */
    } /* RESET */
    else if (state_ptr->rst_ack_to_send == TRUE)
    {
      /*---------------------------------------------------------------------
        The mobile is responding to an RLP reset or reset ACK and a RESET ACK 
        message is to be transmitted.  Build up the message for SLP by adding 
        the message type.  Upon sending a RESET ACK message, RLP transitions 
        to the ESTABLISHED state.  Allocate a DSM item to hold the signaling 
        message. Account for lower layer headers by including a header offset
       --------------------------------------------------------------------*/
      item_ptr = dsm_offset_new_buffer
                 (
                   DSM_DS_POOL_SIZE(DSM_DS_SMALL_ITEM_SIZ),
                   HDRRLP_SIGHDR_OFFSET
                 );
      if (item_ptr == NULL)
      {
        DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                        "RLP [%d,%d] V[%d,%d] Memory Alloc Failed for RLP RSTACK SIG MSG ", 
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced);
        return; 
      }
      field = HDRRLPI_SIG_RSTACK;
      (void) dsm_pushdown_tail(&item_ptr, (void *)&field,1,DSM_DS_SMALL_ITEM_POOL );

      /*---------------------------------------------------------------------
        Send the message directly to SLP without passing through HMP.  As 
        such, the HMP header must be added, prior to queueing for SLP. 
       --------------------------------------------------------------------*/
      field = hmp_hdr;
      (void) dsm_pushdown(&item_ptr, (void *)&field, 1, DSM_DS_SMALL_ITEM_POOL);

      /*---------------------------------------------------------------------
        Clear the RESET ACK indication.  Also, if a RESET message is pending
        we need to clear that also.
       --------------------------------------------------------------------*/
      state_ptr->rst_ack_to_send = FALSE;
      if (state_ptr->rst_to_send == TRUE)
      {
        state_ptr->rst_to_send = FALSE;
      }

      /*---------------------------------------------------------------------
        If the reset was initiated by the mobile, upon sending a RESET ACK 
        frame, RLP transitions to the ESTABLISHED state.  Otherwise, should
        already be in the reset state 
       --------------------------------------------------------------------*/
      if (state_ptr->mob_initiated_reset == TRUE)
      {   
        /*-------------------------------------------------------------------
          Mobile initiated reset.  Leave the Reset state after having sent
          the reset ack message.
         ------------------------------------------------------------------*/
        (void) rex_clr_timer(&rrcb_ptr->reset_timer);
        state_ptr->state = DSRLPI_ESTABLISHED_STATE;
        DATA_RLP_MSG4 (MSG_LEGACY_HIGH, 
                        "RLP [%d,%d] V[%d,%d] RLP Established ", 
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced);
        state_ptr->mob_initiated_reset = FALSE;
      }
      
      /*---------------------------------------------------------------------
        Queue directly to SLP.SLP is responsible for freeing the memory items
        used for message building.  Note that since we have created the new 
        item with a specified offset, the dsm_pushdown() will not allocate a 
        new item.  
       --------------------------------------------------------------------*/
      DATA_RLP_MSG4 (MSG_LEGACY_HIGH, 
                      "RLP[%d,%d] V[%d,%d] Queueing Reset ACK to SLP", 
                      rrcb_ptr->cfg_ptr->sr_id,
                      "AB"[(rrcb_ptr->route)],
                      rrcb_ptr->cfg_ptr->ver,
                      rrcb_ptr->cfg_ptr->enhanced);

      (void) hdrcp_slp_send_msg(HDRHAI_RTC,                           /* HDR channel */
                      TRUE,                                 /* Reliable    */
                      HDRRLP_SIG_MSG_PRIORITY,     /* RLP Sig Msg Priority */
                      item_ptr,                      /* pointer to sig msg */
                      NULL,        /* callback after message has been sent */
                      NULL);                  /* user data for cb function */
    } /* RESET_ACK */
    else if (state_ptr->naks_to_send == TRUE)
    {
       /*--------------------------------------------------------------------
        RLP is established.  Determine if there are any NAK messages to 
        send, and if so, build them up from the NAK list entries.  Queue them 
        up for SLP.  Otherwise, just return.
      ---------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Make sure that there are indeed NAK list entries available.
       --------------------------------------------------------------------*/
        nak_id = hdrrlpnak_get_next(rrcb_ptr, 0, 0, 0);
      if (nak_id == 0)
      {
        state_ptr->naks_to_send = FALSE;
        continue; 
      }

      /*---------------------------------------------------------------------
        There are outstanding RLP octets that need to be NAK'ed.  Build up
        one or more NAK control messages and queue up for SLP transmission.
        This logic should really only ever generate a single NAK signaling
        message, however, should there be more than 64 outstanding NAKs, 
        another DSM item will be allocated, and another signaling message
        will be built up and queued.  The maximum size SLP SDU is 255 bytes.
        Accounting for header bytes, the maximum RLP message can only be
        xx bytes, leaving only xx NAK records.
       --------------------------------------------------------------------*/
      msgs_to_send = TRUE;
      while (msgs_to_send == TRUE)
      {
        /*-------------------------------------------------------------------
          NAK msgs to send.Allocate a DSM item to hold the signaling message.
          Account for lower layer headers by including a header offset.
        -------------------------------------------------------------------*/
        item_ptr = dsm_offset_new_buffer
                   (
                     DSM_DS_POOL_SIZE(DSM_DS_SMALL_ITEM_SIZ),
                     HDRRLP_SIGHDR_OFFSET
                   );

        if (item_ptr == NULL)
        {
          DATA_RLP_MSG4 (MSG_LEGACY_ERROR, 
                          "RLP[%d,%d] V[%d,%d] Memory Allocation Failed for NAK SIG MSG", 
                          rrcb_ptr->cfg_ptr->sr_id,
                          "AB"[(rrcb_ptr->route)],
                          rrcb_ptr->cfg_ptr->ver,
                          rrcb_ptr->cfg_ptr->enhanced);
          return;
        }

        /*-------------------------------------------------------------------
          Build the NAK records.
        -------------------------------------------------------------------*/
        msgs_to_send = hdrrlpi_build_nak_msg(rrcb_ptr, item_ptr);

        /*-------------------------------------------------------------------
          Pre-pend the NAK RLP header.
        -------------------------------------------------------------------*/
        field = HDRRLPI_SIG_NAK;
        (void) dsm_pushdown(&item_ptr, (void *)&field, 1, DSM_DS_SMALL_ITEM_POOL);


        /*-------------------------------------------------------------------
          Send the message directly to SLP without passing through HMP.  This
          will be queued directly to SLP.  SLP is responsible for freeing the
          memory items used for message building.
         ------------------------------------------------------------------*/
        field = hmp_hdr;
        (void) dsm_pushdown(&item_ptr, (void *)&field, 1, DSM_DS_SMALL_ITEM_POOL);
        DATA_RLP_MSG4 (MSG_LEGACY_MED, "RLP[%d,%d] V[%d,%d] Queueing RLP NAK to SLP",
                        rrcb_ptr->cfg_ptr->sr_id,
                        "AB"[(rrcb_ptr->route)],
                        rrcb_ptr->cfg_ptr->ver,
                        rrcb_ptr->cfg_ptr->enhanced);
        (void) hdrcp_slp_send_msg(HDRHAI_RTC,                         /* HDR channel */
                        FALSE,                              /* Best effort */
                        HDRRLP_SIG_MSG_PRIORITY,   /* RLP Sig Msg Priority */
                        item_ptr,                    /* pointer to sig msg */
                        NULL,      /* callback after message has been sent */
                        NULL);                /* user data for cb function */

      } /* msgs_to_sends */

      /*---------------------------------------------------------------------
        Clear the NAK indication.
       ----------------------------------------------------------------------*/
      state_ptr->naks_to_send = FALSE;

    } /* if NAKs to send */
    } /* for p < DSRSP_NUM_ROUTES */
  } /* for rlp_index < DSRLPI_MAX_SR_ID */

  /*-------------------------------------------------------------------------
    Completed queueing up control frames, so just return.
   ------------------------------------------------------------------------*/

#else
  DATA_RLP_MSG0 (MSG_LEGACY_ERROR, "hdrrlp_queue_msg() is not supported.");
#endif

  return;
} /* HDRRLP_QUEUE_MSG */

/*===========================================================================

FUNCTION HDRRLP_CLEAR_STATS

DESCRIPTION
  This function clears the RLP log statistics for all registered RLPs.
  
DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrlp_clear_stats( void )
{
#ifdef FEATURE_HDR
  uint8 i; /* Index Counter */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
   For each HDR RLP, clear the statistics and log a stats packet so that
   DM will be aware of the changes right away.
  -----------------------------------------------------------------------*/
  for(i=0;i<DSRLP_MAX_NUM_HDR_RLP;i++)
  {
    dsrlp_clear_stats( DSRLP_HDR_BASE_INDEX + i );
#if defined(FEATURE_HDRRLP_LOGGING)
    hdrrlp_log_statistics( (uint8) (DSRLP_HDR_BASE_INDEX + i) );
#endif 
  }
#else
  DATA_RLP_MSG0 (MSG_LEGACY_ERROR, "hdrrlp_clear_stats() is not supported.");
#endif

  return;
} /* hdrrlp_clear_stats() */

/*===========================================================================

FUNCTION HDRRLP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRRLP.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrlp_ind_cb
(
  hdrcom_ind_name_enum_type ind_name,
  void * ind_data_ptr
)
{
#ifdef FEATURE_HDR
  /* hdrrlp_util_is_task_hdrrx() is introduced in place of rex_self() to get 
   * the UT compiled
   */
  if (hdrrlp_util_is_task_hdrrx())
  {
    hdrrlp_process_ind(ind_name, (hdrcom_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrcp_rx_queue_ind( HDRHAI_RADIO_LINK_PROTOCOL, ind_name, ind_data_ptr );
  }
#else
  DATA_RLP_MSG0 (MSG_LEGACY_ERROR, "hdrrlp_ind_cb() is not supported.");
#endif

  return;
} /* hdrrlp_ind_cb */

/*===========================================================================

FUNCTION HDRRLP_PROCESS_IND

DESCRIPTION
  This function processes indications given to HDRRLP.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Name of the indication for HDRRLP to process
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrlp_process_ind
(
  hdrcom_ind_name_enum_type ind_name,
  hdrcom_ind_data_union_type * ind_data_ptr
)
{
#ifdef FEATURE_HDR
  switch (ind_name)
  {
    case HDRIND_LMAC_SCHEDULER_GROUP_CHANGED:
    {
      hdrrlpqn_process_scheduler_group_changed_ind();
      break;
    }

    default:
    {
      DATA_RLP_MSG1( MSG_LEGACY_ERROR, "Invalid indication: %d", ind_name );
      break;
    }
  }
#else
  DATA_RLP_MSG0 (MSG_LEGACY_ERROR, "hdrrlp_process_ind() is not supported.");
#endif
  return;
}/* hdrrlp_process_ind */


/*===========================================================================
FUNCTION      HDRRLP_CTA_EXP_SIG_HDLR

DESCRIPTION   This is the handler function for HDRRX_RLP_CTA_EXP_SIG, which is
              called from hdrrx task context. The signal is post from timer task
			  in the cta timer call back funtion, in order to void RPC call 
			  in timer context.
            
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void hdrrlp_cta_exp_sig_hdlr(void)
{
#ifdef FEATURE_HDR
  ds707_tc_state_type          *tc_state_ptr  = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  tc_state_ptr  = ds707_get_pri_tc_ptr();

 /*-----------------------------------------------------------------------
   1. check if the traffic channel is up in 1x or hdr
   2. process thiscommand only if traffic channel is up in hdr.
  -----------------------------------------------------------------------*/

  if(DS_ISDATA_HDR( tc_state_ptr->so)) //default tC pointer
  {
    ds707_enhanced_cta_go_dormant_hdlr();
  }
  else 
  {
    //Got signal when data call is not connected
    DATA_RLP_MSG0 (MSG_LEGACY_ERROR, "hdrrlp_cta_exp_sig_hdlr() cta go dormant ignored");
  }

#else
  DATA_RLP_MSG0 (MSG_LEGACY_ERROR, "hdrrlp_cta_exp_sig_hdlr() is not supported.");
#endif
  return;
}
