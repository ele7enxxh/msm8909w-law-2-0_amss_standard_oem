/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I G H     D A T A     R A T E

       R A D I O   L I N K   P R O T O C O L   I N T E R N A L   F I L E

GENERAL DESCRIPTION

  This file contains general functions that are called from the HDR
  RLP modules. The functions in this file are not exposed to modules outside
  of the hdrrlp modules.

  Function prototypes for the external functions in this file are given in
  dsrlpi.h
  
  Naming convention in this file is:
  All external functions start with hdrrlpi..
  All internal functions start with rlpi...
  All internal variables (in file scope) start with rlpi..
  All external variables start with dsrlpi.. and are defined in dsrlpi.h

  
EXTERNALIZED FUNCTIONS
  hdrrlpi_process_rx_sig_msg()
    Processes received RLP signaling messages.  These messages will have been
    queued from SLP onto a received signaling message queue.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  RLP configuration and initialization procedures.

 Copyright (c) 2000-2015 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrlpi.c#1 $

when       who        what, where, why
--------   ---        ------------------------------------------------------- 
05/16/14   sd         Fix to avoid privilege violation for accessing already 
                      deleted nak_id.
01/31/14   sd         Added critical sections to avoid race condition
                      while getting nak_id
06/18/13   sd         Removed multiple critical sections for dsrlpi_reset.
02/22/12   msh        Corrected condition for pull-up of dsm item 
11/18/11   vpk        HDR SU API cleanup
08/18/11   vpk        Fix for suprious NAKs 
07/26/11   vpk        Move away from hdrbit header
06/28/11   vpk        Update received count only when RLP is established
05/12/11   op         Migrated to MSG 2.0 macros
03/29/11   ack        crit sect guard for dsrlpi_reset
03/15/11   mg         Global variable cleanup
02/08/11   ls         Global variable cleanup
10/18/10   ms         Added debug messages for all Assert(0)s.
02/17/10   ps         Fixed KlocWork errors.
02/11/10   spn        Removed Asserts in the Transmit Path if DSM items are
                      not found
11/10/09   as         Cleared Windows Compiler warnings for MOB builds
06/10/09   spn        Featurize Quick Repeat of DNAKs
10/07/08   cs         Compiler warning fix.
08/18/08   spn        Added support to MARQ flush packets
09/06/08   as         Fixed Featurization for FEATURE_HDRRLP_LOGGING
05/21/08   spn        Fixed the Retx RLP Packet DUPing for Logging
05/14/08   spn        MMPA: Fixed Logging in case of OoO Transmissions
03/05/08   hs         Fixed Featurization for RLP Logging
09/24/07   vas        Fixed memory leak that would occur if system memory was low
08/31/07   vas/spn    Merged fix from 1H06. Removed resetting of RLP, during 
                      building a packet if there are not enough DSM items. It 
                      was messing up the handles queue to a bad state, causing a crash.
05/31/07   ac         Fix the memory leak in RLP tx path.
05/23/07   spn        Lint Fixes
05/22/07   spn        Added an ASSERT() if we get a NULL txq_ptr, while 
                      adding transmitted bytes to the transmitted queue.
05/18/07   ac         EMPA RSP debug message enhancement
03/22/07   ac         EMPA RLP logging error fixing.
02/28/07   spn        Removed Wrongly Reset Variables
02/07/07   ac         EMPA fix the crash during retransmit at line 1713.
12/21/06   spn        Integration of the SPD module into dsrlp.
12/19/06   sy         While RLP is in the middle of building an RLP packet and
                      runs out of memory, free the packet and reset the variables.
12/18/06   spn        Introduced DS_META_INFO in place of PS_META_INFO
09/28/06   sy         Added code to switch dsm pools[small/large]when
                      a pool is running low in memory. This will help
                      in draining the other pool for memory based flow
                      control to start.
08/08/06   ksu        EMPA: RLP Tx code review comments
06/06/06   ksu        EMPA: RLP Rx / Sig code review comments
05/30/06   ksu        EMPA: RLP init code review comments
05/01/06   sy         In case if we are transmitting new data less than 
                      max_allowed by PCP then it means the packet will 
                      go as Format B, so we should limit the size to 255 
                      including RLP header.
04/26/06   sy         Fixed  issue where the input to the macro DSRLPSEQ_PUT_HDR_SEQ()
                      is expecting a right-shifted modulus.
03/01/06   ksu        EMPA: properly use the dsm_seek_extract() api
02/07/06   ksu        EMPA: forward link RoHC testing
02/06/06   ksu        EMPA: reverse link RoHC testing
12/29/05   ksu        EMPA: trivial route selection protocol (RSP)
12/23/05   ksu        EMPA RSP: two rev watermarks per ip flow
11/28/05   ksu/sy     Fix problem with multiple nak records in one nak message.
12/04/05   ksu        EMPA segment sequencing: don't fragment for Format B
12/01/05   ksu        EMPA packet stream: segment sequencing (reverse link also)
11/22/05   ksu        parameterize HDRRLPI_MAX_NAK_LEN
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)
11/03/05   ksu        EMPA: packet stream, octet sequencing (reverse link only)
10/27/05   ksu        EMPA: octet stream, octet sequencing
10/21/05   sy         Updated code to support version2.0 RLP signalling messages.
10/21/05   sy         Restrict the transmitted bytes to 255 incase RLP can't
                      return all the bytes requested by PCP.    
10/10/05   ksu/sy     Eliminate hdrmrlpi_rx_process_nak()
10/10/05   ksu/sy     Eliminate hdrmrlpi_reset_proc()
09/29/05   ksu/sy     MFPA: short sequence numbers work for flow with naks disabled
09/28/05   ksu/sy     HDR: use one-shot abort timer instead of 20ms ticks
09/28/05   ksu/sy     Call hdr[m]rlpi_rx_process_sig_msg() directly from hdr[m]rlp_msg_cb().
09/23/05   ksu/sy     Don't populate Tx queue if naks are disabled.
09/21/05   ksu        Use local next sequence number for new data (format B)
09/19/05   sy         Don't process packet status if RLP is not in established
                      state.                   
09/06/05   ksu        don't send multiple flush packets in one format B subframe
08/11/05   tc         Add handle_tx_new_data_count to track multiple status 
                      callback.
08/08/05   sk/sy      Merged semi-dynamic QOS changes from Sandbox.
07/29/05   sy         Support for Rev A under FEATURE_HDR_QOS.
07/25/05   tc         Add tx_item_len to track multiple watermark data dequeue.
07/15/05   tc         Support multiple Format B connection layer packets in one
                      RTCMAC sub frame..
06/01/05   tc         Changes for RLP code in order to support Rev A HDR RTCMAC
09/09/04   vas        Preliminary QOS support.
04/04/03   ak         Handle new method of doing meta_info.
09/12/03   mpa        Converted F3 messages to use new MSG2.0 HDRDATA SSID
09/12/03   vas        Added code so that msg_ptr would be correctly freed
                      during Nak signaling msg processing if an empty Nak is
                      received.
08/21/03   vas        Code changes to be compatible with dsm 2.0
07/24/03   vas        Added code to free ps meta info.
03/12/03   vr         Ignore NAK from AN if RLP is already in reset state
02/06/03   vr         Removed some redundant F3 messages
10/22/02   js         Modified F3 messages to distinguish between AN and SN
                      RLP streams. 
08/30/02   vr         Added ASSERT if DSM item of len 0 found in tx_new_data
08/07/02   vr         Fixed not naking complete hole issue
07/18/02   vas        Moved dsrlpi_reset to be executed in the Rx task instead
                      of Tx task.
04/04/02   vr         Fixed payload size wraparound in hdrrlpi_tx_new_data
02/15/02   hcg        Added RLP reset failsafe timer to prevent RLP deadlock.
11/30/01   hcg        Fixed RLP seq MSG macros to display seq #'s correctly.
09/13/01   hcg        Updated to use new DSM macros.
08/15/01   hcg        Remove FCP from RLP signaling message processing. These
                      are now processed directly in the callback.
04/05/01   hcg        Added back in call to dsrlptxq_access_last_frame().
04/02/01   hcg        Changed calling convention to dsrlptxq_add_frame().
02/08/01   hcg        Fixed seq num comparison macro bug when getting re-xmit
                      data.  Fixed some MSG's.
03/03/00   hcg        Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"

#if defined(FEATURE_HDR)
#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "hdrrlpnak.h"
#include "hdrrlpseq.h"
#include "dsrlptxq.h"
#include "hdrrlptxph.h"
#include "hdrrlprtx.h"
#include "hdrrlp.h"
#include "hdrrlpi.h"
#include "hdrrlp_util.h"
#include "hdrcom_api.h"
#include "bit.h"
#include "amssassert.h"
#include "memory.h"
#include "err.h"
#include "ds1x_meta_info.h"
#include "rlp_event_defs.h"
#ifdef FEATURE_HDR_QOS
#include "hdrmrlpi.h"
#include "hdrmrlpsched.h"
#endif
#include "hdrrlplog.h"

#ifdef FEATURE_HDR_EMPA
#include "ps_rohc.h"
#include "ds707_pkt_mgri.h"
#include "ds707_sec_pkt_mgr_util.h"
#endif /* FEATURE_HDR_EMPA */

#ifdef FEATURE_HDR_REVB
#include "hdrrlpmlnak.h"
#include "hdrrlptxsh.h"
#endif /* FEATURE_HDR_REVB */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  EMPA packet stream: minimum size of a Route Protocol packet that may
  be fragmented in order to fit into a 256 bit OTA frame
---------------------------------------------------------------------------*/
#define HDRRLPI_MIN_RP_SIZE_FOR_FRAG( cfg_ptr ) (28 - HDRERLP_HEADER_SIZE( cfg_ptr ))
#define HDRMRLPI_EMPA_NAK_RESERVED_SIZE 2
#define HDRMRLPI_SEQUENCE_LENGTH_SIZE 8

/*===========================================================================
MACRO HDRRLPI_PACK_RLP_HEADER

DESCRIPTION
  Packs the RLP header based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  seq_num:    the sequence number for the packet
  data_ptr:   where to build the packet

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
INLINE void HDRRLPI_PACK_RLP_HEADER
(
  const dsrlpi_cfg_info_type* cfg_ptr,        /* configuration information */
  byte                      route,                       /* DSRLP_ROUTE_* */
  uint32                    seq_num,           /* starting sequence number */
  boolean                   first_data_unit,    /* first segment of packet */
  boolean                   last_data_unit,      /* last segment of packet */
  uint8                    *data_ptr
)
{
  uint16 offset;

  switch (cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      offset = HDRMRLP_STREAM_HEADER_SIZE;

      b_packd(HDRRLPSEQ_GET_SEQ(seq_num, cfg_ptr->seq_modulus),
              data_ptr, offset,
              cfg_ptr->seq_length );
      offset += cfg_ptr->seq_length;
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      offset = HDRMRLP_STREAM_HEADER_SIZE;

      b_packd( cfg_ptr->rlp_id_in_pkt,
               data_ptr, offset, 
               cfg_ptr->rlp_id_len_in_pkt );
      offset += cfg_ptr->rlp_id_len_in_pkt;

      if (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
      {
        offset = HDRMRLP_STREAM_HEADER_SIZE+cfg_ptr->rlp_id_len_in_pkt;

        b_packb( route,
                 data_ptr, offset, HDRRLP_MMPA_ROUTE_LEN );
        offset += HDRRLP_MMPA_ROUTE_LEN;

        b_packd( HDRRLPSEQ_GET_SEQ(seq_num,cfg_ptr->seq_modulus),
                 data_ptr, offset, cfg_ptr->seq_length );
        offset += cfg_ptr->seq_length;

        b_packb( first_data_unit ? 1 : 0,
                 data_ptr, offset, HDRRLP_MMPA_FDU_LEN );
        offset += HDRRLP_MMPA_FDU_LEN;

        b_packb( last_data_unit ? 1 : 0,
                 data_ptr, offset, HDRRLP_MMPA_LDU_LEN );
        offset += HDRRLP_MMPA_LDU_LEN;
      }
      else if (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA)
      {
        offset = HDRMRLP_STREAM_HEADER_SIZE+cfg_ptr->rlp_id_len_in_pkt;

        b_packb( route,
                 data_ptr, offset, HDRRLP_EMPA_ROUTE_LEN );
        offset += HDRRLP_EMPA_ROUTE_LEN;

        b_packd( HDRRLPSEQ_GET_SEQ(seq_num,cfg_ptr->seq_modulus),
                 data_ptr, offset, cfg_ptr->seq_length );
        offset += cfg_ptr->seq_length;

        b_packb( first_data_unit ? 1 : 0,
                 data_ptr, offset, HDRRLP_EMPA_FDU_LEN );
        offset += HDRRLP_EMPA_FDU_LEN;

        b_packb( last_data_unit ? 1 : 0,
                 data_ptr, offset, HDRRLP_EMPA_LDU_LEN );
        offset += HDRRLP_EMPA_LDU_LEN;
      }
      else  /* DPA / MFPA */
      {
        b_packd( HDRRLPSEQ_GET_SEQ(seq_num, cfg_ptr->seq_modulus),
                 data_ptr, offset,
                 cfg_ptr->seq_length );
        offset += cfg_ptr->seq_length;
      }
      break;
#endif // FEATURE_HDR_QOS

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)cfg_ptr->ver, cfg_ptr->sr_id, 0);
      break;
  }
}


/*===========================================================================
MACRO HDRRLPI_NAK_RLP_FLOW_SIZE

DESCRIPTION
  Returns the size of the RLPFlow field in the Nak message based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The RLPFlow field size

SIDE EFFECTS
  None.
===========================================================================*/
INLINE int HDRRLPI_NAK_RLP_FLOW_SIZE
(
  const dsrlpi_cfg_info_type* cfg_ptr      /* configuration information    */
)
{
  int rlp_flow_size = 0;

  switch (cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      /* no RLPFlow field in DPA */
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      if (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) 
      {
        rlp_flow_size = HDRRLP_MMPA_LINK_FLOW_ID_LEN;
      }
      else if (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA) 
      {
        rlp_flow_size = HDRRLP_EMPA_LINK_FLOW_ID_LEN;
      }
      else 
      {
        rlp_flow_size = HDRMRLPI_RLP_FLOW_SIZE;
      }
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)cfg_ptr->ver, cfg_ptr->sr_id, 0);
      break;
  }

  return rlp_flow_size;
}


/*===========================================================================
MACRO HDRRLPI_NAK_REQUESTS_SIZE

DESCRIPTION
  Returns the size of the NakRequests field in the Nak message based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The NakRequests field size

SIDE EFFECTS
  None.
===========================================================================*/
INLINE int HDRRLPI_NAK_REQUESTS_SIZE
(
  const dsrlpi_cfg_info_type* cfg_ptr      /* configuration information    */
)
{
  int nak_requests_size = 0;

  switch (cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      nak_requests_size = 8;
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      nak_requests_size = HDRMRLPI_NAK_REQUESTS_SIZE;
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)cfg_ptr->ver, cfg_ptr->sr_id, 0);
      break;
  }

  return nak_requests_size;
}


/*===========================================================================
MACRO HDRRLPI_NAK_RESERVED_SIZE

DESCRIPTION
  Returns the size of the Reserved field in the Nak message based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The Reserved field size

SIDE EFFECTS
  None.
===========================================================================*/
INLINE byte HDRRLPI_NAK_RESERVED_SIZE
(
  const dsrlpi_cfg_info_type* cfg_ptr
)
{
  byte nak_reserved_size = 0;

  switch (cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      nak_reserved_size = 2;
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      if (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA ||
          cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA) 
      {
        nak_reserved_size = 0;
      }
      else
      {
        nak_reserved_size = (8-((cfg_ptr->seq_length + HDRMRLPI_NAK_WINDOW_LEN_SIZE)%8))%8;
      } 
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)cfg_ptr->ver, cfg_ptr->sr_id, 0);
      break;
  }

  return nak_reserved_size;
}


/*===========================================================================
MACRO HDRRLPI_NAK_WINDOW_LEN_SIZE

DESCRIPTION
  Returns the size of the WindowLen field in the Nak message based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The WindowLen field size

SIDE EFFECTS
  None.
===========================================================================*/
INLINE int HDRRLPI_NAK_WINDOW_LEN_SIZE
(
  const dsrlpi_cfg_info_type* cfg_ptr
)
{
  int nak_window_len_size = 0;

  switch (cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      nak_window_len_size = 16;
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      if (cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA ||
          cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA)
      {
        nak_window_len_size = cfg_ptr->seq_length;
      }
      else
      {
        nak_window_len_size = HDRMRLPI_NAK_WINDOW_LEN_SIZE;
      }
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)cfg_ptr->ver, cfg_ptr->sr_id, 0);
      break;
  }

  return nak_window_len_size;
}

/*===========================================================================
MACRO HDRRLPI_MAX_NAK_LEN

DESCRIPTION
  Returns the max window length of a Nak record based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The WindowLen field size

SIDE EFFECTS
  None.
===========================================================================*/
INLINE uint32 HDRRLPI_MAX_NAK_LEN
(
  const dsrlpi_cfg_info_type* cfg_ptr
)
{
  int nak_window_len_size = HDRRLPI_NAK_WINDOW_LEN_SIZE(cfg_ptr);

  return ((1 << nak_window_len_size) - 1);
}

/*===========================================================================
FUNCTION  HDRRLPI_GET_NEW_BUFFER

DESCRIPTION
  Allocates a DSM item.
 
PARAMETERS
  new_pkt_item_ptr:  The pointer to the dsm Item that is returned
  max_bytes:  The bytes that need to be accomodated. 
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean: TRUE if successful allocation of a DSM item, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
INLINE boolean hdrrlpi_get_new_buffer( dsm_item_type **new_pkt_item_ptr, uint32 max_bytes)
{
  dsm_mempool_id_enum_type pool_id;
  boolean                 mem_exist = TRUE;
  /*------------------------------------------------------------------------*/
    /*-------------------------------------------------------------------------
    Determine which pool to use to store the outgoing packet. If the
    determined pool is running low on memory, use the other pool.
  -------------------------------------------------------------------------*/
  pool_id = DSM_DS_POOL_SIZE(max_bytes);

  if (DSM_LESS_THAN_LINK_LAYER_FEW_FREE_ITEMS(pool_id))
  {
    pool_id = DSM_DS_OTHER_POOL_ID(pool_id);
    if (DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id))
    {
      DATA_RLP_MSG2(MSG_LEGACY_ERROR, "Running low in system memory, "
                    "Small_itm_cnt: %d, Large_itm_cnt: %d",
                    DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                    DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL));
      return FALSE;
    }
  }

  if(new_pkt_item_ptr == NULL)
  {
     DATA_RLP_MSG2(MSG_LEGACY_ERROR, "Could not allocate memory, "
                   "Small_itm_cnt: %d, Large_itm_cnt: %d", 
                   DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                   DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL));
     return FALSE;
  }

  *new_pkt_item_ptr = dsm_new_buffer(pool_id);

  if(*new_pkt_item_ptr == NULL)
  {
     DATA_RLP_MSG2(MSG_LEGACY_ERROR, "Could not allocate memory, "
                   "Small_itm_cnt: %d, Large_itm_cnt: %d", 
                   DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                   DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL));
     return FALSE;
  }

  return mem_exist;
} /* hdrrlpi_get_new_buffer() */

/*===========================================================================
MACRO HDRRLPI_DEQUEUE_TX_ITEM

DESCRIPTION
  Dequeues the next Tx item based on RLP version, and adds it to the tx_item_q
 
PARAMETERS
  rrcb_ptr:   RLP Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the next Tx item was dequeued, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
INLINE boolean HDRRLPI_DEQUEUE_TX_ITEM
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr
)
{
  dsm_item_type *new_pkt_item_ptr=NULL; /* new pkt chain         */
  ds1x_meta_info_type       meta_info;        /* pointer to meta info        */
  uint16                  meta_size;  
  boolean                 retval = FALSE;

  /* Get a new packet from the watermark(s) */
  switch (rrcb_ptr->cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      new_pkt_item_ptr = dsm_dequeue(rrcb_ptr->cfg_ptr->tx_wm_ptr);
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      new_pkt_item_ptr = hdrmrlpsched_dequeue(rrcb_ptr);
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)rrcb_ptr->cfg_ptr->ver, rrcb_ptr->cfg_ptr->sr_id, 0);
      break;
  }

  /* Append it to the tx_item_q */
  if (new_pkt_item_ptr != NULL)
  {
    /*-------------------------------------------------------------------------
      The meta info information is embedded in the data field.  extract
      and throw away
      -------------------------------------------------------------------------*/
    if ( (new_pkt_item_ptr->kind == DS1X_META_INFO_KIND) &&
         (rrcb_ptr->cfg_ptr->transmit_abort_timer_rev_val > 0) )
    {
      meta_size = dsm_pullup(&(new_pkt_item_ptr),
                             &meta_info,
                             sizeof(ds1x_meta_info_type)
                             );
      ASSERT(meta_size == sizeof(ds1x_meta_info_type));
      DATA_RLP_MSG2(MSG_LEGACY_MED, "Pulled up %d bytes from dsm item of type %d. ",
              meta_size, meta_info.ds1x_meta_kind);
      /*-------------------------------------------------------------------------
        Once we pull out the packet, reset the kind so that 
        there is no misinterpretation of the DSM ITEM 
      -------------------------------------------------------------------------*/
      new_pkt_item_ptr->kind = 0;        
    }

    q_put(&rrcb_ptr->state.tx_item_q, &new_pkt_item_ptr->link);
    retval =  TRUE;
  }

  return retval;
}

/*===========================================================================
MACRO HDRRLPI_TX_ITEM_Q_LOCAL_LEN_EXCL

DESCRIPTION
  Returns the length of the items in Tx item queue up to, but not including,
  local_seek_offset.  This is needed to detect packet boundaries in
  an EMPA packet stream.
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The the length of the items in Tx item queue up to, but not including,
  local_seek_offset

SIDE EFFECTS
  None.
===========================================================================*/
INLINE uint16 HDRRLPI_TX_ITEM_Q_LOCAL_LEN_EXCL
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
  uint8 combo_index
)
{
  dsm_item_type *item_ptr;
  uint16 num_tx_bytes = 0;

  if ((0 < rrcb_ptr->state.local_state[combo_index].seek_offset) &&
      ((item_ptr = (dsm_item_type *)q_check(&rrcb_ptr->state.tx_item_q)) != NULL))
  {
    num_tx_bytes = (uint16)dsm_length_packet(item_ptr);
    while ((num_tx_bytes < rrcb_ptr->state.local_state[combo_index].seek_offset) &&
           ((item_ptr = (dsm_item_type *)q_next(&rrcb_ptr->state.tx_item_q, &item_ptr->link)) != NULL))
    {
      num_tx_bytes += (uint16)dsm_length_packet(item_ptr);
    }
  }

  return num_tx_bytes;
}

/*===========================================================================
MACRO HDRRLPI_TX_ITEM_Q_LOCAL_LEN_INCL

DESCRIPTION
  Returns the length of the items in Tx item queue up to and including
  local_seek_offset.  This is needed to detect packet boundaries in
  an EMPA packet stream.
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The the length of the items in Tx item queue up to and including
  local_seek_offset

SIDE EFFECTS
  None.
===========================================================================*/
INLINE uint16 HDRRLPI_TX_ITEM_Q_LOCAL_LEN_INCL
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
  uint8 combo_index
)
{
  dsm_item_type *item_ptr;
  uint16 num_tx_bytes = 0;

  if ((item_ptr = (dsm_item_type *)q_check(&rrcb_ptr->state.tx_item_q)) != NULL)
  {
    num_tx_bytes = (uint16)dsm_length_packet(item_ptr);
    while ((num_tx_bytes <= rrcb_ptr->state.local_state[combo_index].seek_offset) &&
           ((item_ptr = (dsm_item_type *)q_next(&rrcb_ptr->state.tx_item_q, &item_ptr->link)) != NULL))
    {
      num_tx_bytes += (uint16)dsm_length_packet(item_ptr);
    }
  }

  return num_tx_bytes;
}

/*===========================================================================
MACRO HDRRLPI_TX_ITEM_Q_LEN

DESCRIPTION
  Returns the length of the Tx item queue
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The length of the Tx item queue

SIDE EFFECTS
  None.
===========================================================================*/
INLINE uint16 HDRRLPI_TX_ITEM_Q_LEN
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
)
{
  dsm_item_type *item_ptr;
  uint16 num_tx_bytes = 0;

  if ((item_ptr = (dsm_item_type *)q_check(&rrcb_ptr->state.tx_item_q)) != NULL)
  {
    num_tx_bytes += (uint16)dsm_length_packet(item_ptr);
    while ((item_ptr = (dsm_item_type *)q_next(&rrcb_ptr->state.tx_item_q, &item_ptr->link)) != NULL)
    {
      num_tx_bytes += (uint16)dsm_length_packet(item_ptr);
    }
  }

  return num_tx_bytes;
}
/*===========================================================================
MACRO HDRRLPI_TX_ITEM_Q_EXTRACT

DESCRIPTION
  This function copies len bytes at the given offset from the tx_item_q.

DEPENDENCIES
  None

PARAMETERS
  rscb_ptr: RLP Session Control Block pointer
  offset:   offset into packet
  buf:      buffer to put extracted bytes into
  len:      number of bytes to extract

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE void HDRRLPI_TX_ITEM_Q_EXTRACT
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
  uint16 offset,
  void *buf,
  uint16 len
)
{
  dsm_item_type* item_ptr = NULL;
  dsm_item_type* tmp_item_ptr;       /* Working ptr to packet              */
  uint16 tmp_len;
  uint16 tmp_off;
  uint16 bytes_remaining;

  /* Find the starting dsm item chain */
  if ((item_ptr = (dsm_item_type *)q_check(&rrcb_ptr->state.tx_item_q)) != NULL)
  {
    if (offset >= dsm_length_packet(item_ptr))
    {
      offset -= (uint16)dsm_length_packet(item_ptr);
      while ((item_ptr = (dsm_item_type *)q_next(&rrcb_ptr->state.tx_item_q, &item_ptr->link)) != NULL)
      {
        if (offset >= (uint16)dsm_length_packet(item_ptr))
        {
          offset -= (uint16)dsm_length_packet(item_ptr);
        }
        else
        {
          break;
        }
      }
    }
  }
  ASSERT(item_ptr != NULL);

  /* Now we have the starting dsm item chain, but we may still have an */
  /* offset into that dsm item chain. */
  tmp_off = 0;
  while (len > 0) 
  {
    /* Calculate the bytes remaining in the item_ptr from offset onwards */
    bytes_remaining =  (uint16)dsm_length_packet(item_ptr) - offset ;
    tmp_len = MIN(len, bytes_remaining );

    /* Use a temporary item ptr, as dsm_seek_extract() will modify it */
    /* to point to the item within the dsm item chain that contains */
    /* the offset.  The offset will also be modified, but we don't use */
    /* it after this. */
    tmp_item_ptr = item_ptr;
    if (!dsm_seek_extract( &tmp_item_ptr,
                           &offset,
                           (char*)buf+tmp_off,
                           &tmp_len))
    {
      ERR_FATAL("dsm_seek_extract bytes_to_copy:%d", tmp_len, 0, 0);
    }
    len -= tmp_len;

    /* If there is more to extract, then we have consumed the current */
    /* dsm item chain, so get the next dsm item chain from the tx item */
    /* queue.  Since we are at a new dsm item chain, there is no more */
    /* offset. */
    if (len > 0) 
    {
      tmp_off += tmp_len;
      offset = 0;
      item_ptr = (dsm_item_type *)q_next(&rrcb_ptr->state.tx_item_q, &item_ptr->link);
      if(item_ptr == NULL)
      {
        ASSERT(0);
        return;
      } 
    }
  }
}

/*===========================================================================
FUNCTION HDRRLPI_NUM_DATA_UNITS

DESCRIPTION
  Computes the number of data units based on RLP version and config
PARAMETERS
  rscb_ptr:  RLP Session Control Block pointer
  num_bytes: number of bytes of payload

DEPENDENCIES
  None.

RETURN VALUE
  The computed number of data units

SIDE EFFECTS
  None.
===========================================================================*/
uint16 HDRRLPI_NUM_DATA_UNITS
(
 const dsrlpi_cfg_info_type *cfg_ptr,
 uint16 num_bytes
)
{
  /* EMPA/MMPA segment sequencing: 1 data unit per rx'd RLP packet */
  if ((cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA ||
      cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
      cfg_ptr->segment_sequencing)
  {
    return 1;
  }
  else
  {
    return num_bytes;
  }
}
/*===========================================================================

            EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================
FUNCTION HDRRLPI_MOBILE_INITIATED_RESET

DESCRIPTION
  Mobile initiated reset based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  state_ptr:  local state ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrlpi_mobile_initiated_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr
)
{
  switch (rrcb_ptr->cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      rrcb_ptr->state.rst_to_send = TRUE;
      rrcb_ptr->state.mob_initiated_reset = TRUE;
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
      rrcb_ptr->state.reset_rx_ind_to_send = TRUE;
      break;

    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      rrcb_ptr->state.reset_tx_ind_to_send = TRUE;
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d",
                (int)rrcb_ptr->cfg_ptr->ver, rrcb_ptr->cfg_ptr->sr_id, 0);
      break;
  }

  hdrrlpi_reset_proc(rrcb_ptr);
}

/*===========================================================================
FUNCTION HDRRLPI_GET_NEW_DATA_COUNT

DESCRIPTION
  Returns the size of the new data based on RLP version
 
PARAMETERS
  rscb_ptr:   RLP Session Control Block pointer
  
DEPENDENCIES
  None.

RETURN VALUE
  The new data count

SIDE EFFECTS
  None.
===========================================================================*/
int hdrrlpi_get_new_data_count
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr
)
{
  int new_data_count = 0;

  switch (rrcb_ptr->cfg_ptr->ver) 
  {
    case DSRLP_VER_HDR:
      if (rrcb_ptr->cfg_ptr->tx_wm_ptr != NULL) 
      {
        new_data_count = rrcb_ptr->cfg_ptr->tx_wm_ptr->current_cnt;
      }
      break;

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      new_data_count = hdrmrlpsched_get_new_data_cnt(rrcb_ptr);
      break;
#endif /* FEATURE_HDR_QOS */

    default:
      ERR_FATAL("Unrecognized RLP ver:%d sr_id:%d", (int)rrcb_ptr->cfg_ptr->ver, rrcb_ptr->cfg_ptr->sr_id, 0);
      break;
  }

  return new_data_count;
}

/*===========================================================================

FUNCTION HDRRLPI_BUILD_NAK_MSG

DESCRIPTION
  This function is called as part of queuing signaling messages for transmit
  from RLP to SLP.  This function builds up NAK signaling messages based
  on entries in the NAK list.  The function also combines contiguous NAK 
  list entries to form a single NAK request record.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean value that indicates if more NAK messages are to be generated.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrlpi_build_nak_msg
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,  /* rlp-specific control block       */
  dsm_item_type            *item_ptr   /* ptr to sig msg to be built       */
)
{
  uint32           nak_id=0;            /* ID of current NAK list entry    */
  uint32           first_erased=0;        /* seq of first octet to be NAKed  */
  uint32           last_erased=0;         /* seq of last octet in NAK entry  */
  uint32           curr_entry_first=0;  /* tmp for first seq of entry      */
  uint32           curr_entry_last=0;   /* tmp for last seq of entry       */
  uint32           seq_modulus;         /* modulus for this version of RLP */
  uint32           window_len;          /* length of NAK                   */
  uint16           nak_requests=0;      /* number of NAK requests          */
  uint16           offset;              /* bit pos offset for bit pack     */
  uint32           nak_start;           /* starting seq # of NAK record    */
  uint32           nak_bytes_left;      /* Bytes remaining to be NAKed     */
  uint16           first_erased_size;   /* Size of first erased field      */
  uint16           window_len_size;     /* Size of Window Length field     */
#ifdef FEATURE_HDR_QOS
  uint16           tmp_seq_length;      /* Size of Sequence Length field   */
#endif /* FEATURE_HDR_QOS */
  byte             reserved_size;       /* Size of the Reserved field      */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    For Default RLP, we set the length of the first erased field to be the
    length of first erased, plus the length of the reserved fields.
    For multiflow packet app, there is no reserved field per Nak request.
  -------------------------------------------------------------------------*/   
  first_erased_size = rrcb_ptr->cfg_ptr->seq_length;
  window_len_size = HDRRLPI_NAK_WINDOW_LEN_SIZE(rrcb_ptr->cfg_ptr);
  reserved_size = HDRRLPI_NAK_RESERVED_SIZE(rrcb_ptr->cfg_ptr);

  /*-------------------------------------------------------------------------
    Obtain the first NAK list entry by passing nak_id of value 0.  The 
    boolean TRUE parameter is used to obtain octet-based NAK entries.  Get 
    the first/last seq pair and calculate the length of missing octets using
    modulo arithmetic.
  -------------------------------------------------------------------------*/   
  rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
  nak_id = hdrrlpnak_get_next(rrcb_ptr, 0, &first_erased, &last_erased);
  if (nak_id != 0)
  {
    hdrrlpnak_nak_was_sent(rrcb_ptr, nak_id);
  }
  else
  {
    // No NAKs in the list
    rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
    return FALSE;
  }
  
  seq_modulus = rrcb_ptr->cfg_ptr->seq_modulus;
  window_len = HDRRLPSEQ_DIFF_SEQ(last_erased, first_erased, seq_modulus);

  /*-------------------------------------------------------------------------
    Begin NAK building loop.  Build up each NAK request record and append to
    tail end of signaling message that is being built.  For each NAK request
    record, generate a FirstErased and WindowLen entry.  Any contiguous 
    NAK list entries are combined, prior to becoming a NAK request entry.
  -------------------------------------------------------------------------*/   

  offset = 0;
  while (nak_requests < HDRRLPI_MAX_NAK_RECORDS)
  {
    DATA_RLP_MSG7( MSG_LEGACY_MED,
                    "RLP Tx[%d,%d] V[%d,%d] NakId:%d Seq#[0x%x,0x%x) Bld Nak Msg", 
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced,
                    nak_id,
                    first_erased,
                    last_erased);
                    
    /*-----------------------------------------------------------------------
      Get the next NAK list entry.  Note, passing the last nak_id value
      ensures that the next consecutive NAK list entry is obtained.
    -----------------------------------------------------------------------*/
    nak_id = hdrrlpnak_get_next(rrcb_ptr, nak_id, &curr_entry_first, &curr_entry_last);

    /*-----------------------------------------------------------------------
      Determine if the two NAK list entries are contiguous.  The last
      sequence number is EXCLUSIVE, while the first sequence number is
      INCLUSIVE, so if they are contiguous, the numbers will be equal.

      If the total size of the hole to be NAKed (by abutting together
      contiguous NAK entries) exceeds the max window len (0xFFFF) of a 
      NAK record, treat these contiguous entries as non-contiguous and
      send separate NAK records.
    -----------------------------------------------------------------------*/
    if (nak_id != 0)
    {
      hdrrlpnak_nak_was_sent(rrcb_ptr, nak_id);
      if( ( ( (uint32) window_len + 
              (uint32) HDRRLPSEQ_DIFF_SEQ(curr_entry_last, curr_entry_first, 
                                       seq_modulus)
            )
            <= HDRRLPI_MAX_NAK_LEN(rrcb_ptr->cfg_ptr)
          ) &&
          (last_erased == curr_entry_first)
        )
      {
        /*-------------------------------------------------------------------
          The entries are contiguous.  Update the window length and the last
          erased.  Then, continue obtaining the next NAK list entries.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG6( MSG_LEGACY_MED,
                        "RLP Tx[%d,%d] V[%d,%d] Seq#[0x%x,0x%x) Contiguous Nak Entry", 
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced,
                        curr_entry_first,
                        curr_entry_last);

        window_len += HDRRLPSEQ_DIFF_SEQ(curr_entry_last, curr_entry_first, 
                                        seq_modulus);
        last_erased = curr_entry_last;
        continue;
      }
    } /* nak_id != 0 */

    /*-----------------------------------------------------------------------
      The entries are not contiguous or there are no more NAK list entries
      available, therefore a NakRequest record must be generated and copied
      into the signaling message.  Extract the 22 bit sequence number and 
      add to the message.  A total of 3 bytes are added to the message, 
      however the upper 2 bits of the MSB are reserved.  Then add the 
      window length into the next 2 bytes.

      Further, if the hole exceeds the max len that can be sent in one NAK
      record (0xFFFF), then create multiple NAK records for it.
    -----------------------------------------------------------------------*/
    nak_start = HDRRLPSEQ_GET_SEQ(first_erased, rrcb_ptr->cfg_ptr->seq_modulus);
    nak_bytes_left = window_len;

    do
    {
      /* DPA has the reserved bits at the start of the Nak record */
      if (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR) 
      {
        HDRBIT_PACK8( item_ptr, 0, offset, reserved_size );
        offset += reserved_size;
      }

      HDRBIT_PACK32( item_ptr, nak_start, offset, first_erased_size );
      offset += first_erased_size; 
      
      /*---------------------------------------------------------------------
        If there are less than 0xFFFF bytes left, create a NAK record for 
        this hole. Else create a NAK record with 0xFFFF bytes and process
        the remaining bytes separately.
      ---------------------------------------------------------------------*/
      if(nak_bytes_left <= HDRRLPI_MAX_NAK_LEN(rrcb_ptr->cfg_ptr))
      {
        HDRBIT_PACK32( item_ptr, nak_bytes_left, offset, window_len_size);
        offset += window_len_size; 
        DATA_RLP_MSG6( MSG_LEGACY_HIGH,
                        "RLP Tx[%d,%d] V[%d,%d] Seq#[0x%x, ) Window Len:%d, NAK MSG", 
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced,
                        nak_start,
                        nak_bytes_left );

        nak_bytes_left = 0;
        nak_start = 0;
      }
      else
      {
        HDRBIT_PACK32( item_ptr, HDRRLPI_MAX_NAK_LEN(rrcb_ptr->cfg_ptr), offset, window_len_size );
        offset += window_len_size; 
        DATA_RLP_MSG6( MSG_LEGACY_HIGH,
                        "RLP Tx[%d,%d] V[%d,%d] Seq#[0x%x, ) Window Len:%d, Tx NAK MSG", 
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced,
                        nak_start,
                        HDRRLPI_MAX_NAK_LEN(rrcb_ptr->cfg_ptr));

        nak_bytes_left -= HDRRLPI_MAX_NAK_LEN(rrcb_ptr->cfg_ptr); 
        nak_start += HDRRLPI_MAX_NAK_LEN(rrcb_ptr->cfg_ptr);
      }

#ifdef FEATURE_HDR_QOS
      /*------------------------------------------------------------------
       MFPA has the reserved bits at the end of the Nak record
       -----------------------------------------------------------------*/
      if (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        HDRBIT_PACK8( item_ptr, 0, offset, reserved_size );
        offset += reserved_size;
      }
#endif /* FEATURE_HDR_QOS */

      nak_requests++;
 
    } while((nak_bytes_left > 0)&& (nak_requests < HDRRLPI_MAX_NAK_RECORDS));

    /*---------------------------------------------------------------------
      Record Statistics.
    ---------------------------------------------------------------------*/
    rrcb_ptr->stats.at_nak_bytes_requested += window_len;
    rrcb_ptr->stats.at_nak_records_requested++;
    if (nak_id == 0)
    {
      /*---------------------------------------------------------------------
        No more NAK list entries to build.
      ---------------------------------------------------------------------*/
      break;
    }
    else
    {
      /*---------------------------------------------------------------------
        Begin a new NAK request record by updating the following variables.
      ---------------------------------------------------------------------*/
      first_erased = curr_entry_first;
      last_erased = curr_entry_last;
      window_len = HDRRLPSEQ_DIFF_SEQ(curr_entry_last, curr_entry_first,
                                     seq_modulus);
    }

  } /* while */
  rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));

#ifdef FEATURE_HDR_QOS
  /* EMPA/MMPA has the reserved bits at the end of the ReceiverStatus message, after VR */
  if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) 
  {
    HDRBIT_PACK32( item_ptr, rrcb_ptr->state.expected_seq_num, offset, window_len_size);
    offset += window_len_size;
    reserved_size = (8 - (offset%8)) % 8;
    HDRBIT_PACK8( item_ptr, (uint16)0, offset, reserved_size );
    offset += reserved_size;
  }
#endif /* FEATURE_HDR_QOS */

  /*-------------------------------------------------------------------------
    Finished building this NAK message.  Insert the number of NAK requests
    into the head of this packet chain.
  -------------------------------------------------------------------------*/
  (void)dsm_pushdown(&item_ptr, (void *)&nak_requests, 1, DSM_DS_SMALL_ITEM_POOL);
  DATA_RLP_MSG5( MSG_LEGACY_MED,
                  "RLP Tx[%d,%d] V[%d,%d] Total NAK Records:%d", 
                  rrcb_ptr->cfg_ptr->sr_id, 
                  "AB"[rrcb_ptr->route],
                  rrcb_ptr->cfg_ptr->ver, 
                  rrcb_ptr->cfg_ptr->enhanced,
                  nak_requests );
  rrcb_ptr->stats.tx_naks[0] += nak_requests;

#ifdef FEATURE_HDR_QOS
  /*-------------------------------------------------------------------------
   MFPA has a SequenceLength field for use by offline analysis tools
   --------------------------------------------------------------------------*/
  if (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
  {
    tmp_seq_length = rrcb_ptr->cfg_ptr->seq_length;
    (void)dsm_pushdown(&item_ptr, &tmp_seq_length, 1, DSM_DS_SMALL_ITEM_POOL);
  }
#endif /* FEATURE_HDR_QOS */

  /*-------------------------------------------------------------------------
    If the maximum NAK requests have been built into this signaling message, 
    there may be more NAK list entries to handle.  If so, the calling 
    function will call this function again to obtain the remaining NAK 
    requests.  Indicate via boolean return value if this function should be 
    called again.
  -------------------------------------------------------------------------*/
  DSM_TOUCH_ITEM(item_ptr);
  if (nak_requests < HDRRLPI_MAX_NAK_RECORDS)
  {
    return (FALSE);
  }
  else
  {
    return (TRUE);
  }
    
} /* HDRRLPI_BUILD_NAK_MSG */

/*===========================================================================

FUNCTION HDRRLPI_GET_UNREPORTED_QN_INSTANCES

DESCRIPTION
  This function gets the number of QN instances that are active and that
  have received a seq number which has not been reported in a Nak so far.
  It also fills the array with the largest sar seq numbers that have not
  been reported in a nak.
DEPENDENCIES
  None.

RETURN VALUE
  Number of QN instances which have an unreported seq number

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hdrrlpi_get_unreported_qn_instances
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,  /* rlp-specific control block       */
  uint32                   largest_sar_unreported[]
)
{
  dsrlp_qn_ctl_type *qn_ptr;
  uint8              rc = 0;
  uint8              i = 0;

  for (i = 0; i < DSRLP_MAX_QN_INSTANCES_PER_RLP; i++)
  {
    qn_ptr = &(rrcb_ptr->qn_instance_table[ i ]);
    if (qn_ptr->activated && !qn_ptr->largest_seq_num_reported)
    {
      largest_sar_unreported[ rc++ ] = qn_ptr->largest_sar_seq;
    }
  }
  return rc;
}

/*===========================================================================

FUNCTION HDRRLPI_BUILD_MULTILINK_NAK_MSG

DESCRIPTION
  This function is called as part of queuing signaling messages for transmit
  from RLP to SLP.  This function builds up MultiLink NAK signaling messages 
  based on entries in the Multilink NAK list. If a delayed nak entry is 
  included in the final MLNak, then the quick_repeat_ptr_ptr is set to 
  point to a struct of type hdrrlpmlnak_quick_repeat_list_entry_type, which
  contains the NAK windows to be quick-repeated.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean value that indicates if more NAK messages are to be generated.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrlpi_build_multilink_nak_msg
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,  /* rlp-specific control block       */
  dsm_item_type            *item_ptr,  /* ptr to sig msg to be built       */
  void                    **quick_repeat_ptr_ptr /* ptr-ptr to quick_repeat_entry */
)
{
  uint16           nak_requests=0;      /* number of NAK requests          */
  uint16           offset = 0;          /* bit pos offset for bit pack     */
  uint8            reserved_size;       /* Size of the Reserved field      */
  uint8            seq_length;          /* SAR sequence length configured  */
  uint32           largest_sar_unreported[ DSRLP_MAX_QN_INSTANCES_PER_RLP ];
  uint32           window_len;
  boolean          quick_nak_packed;    /* Is there a nak with DelayedNak=0
                                           packed in this multilink nak    */
  uint8            other_qn_count = 0;  /* Count of QN instances with no
                                           nak reported in this Nak        */
  uint8            i;
  hdrrlpmlnak_list_entry_type *nak_entry_ptr;  /*   nak list entry         */

#if defined(FEATURE_HDR_REVB_QUICKREPEATNAK)
  hdrrlpmlnak_quick_repeat_list_entry_type *quick_repeat_entry_ptr;
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( rrcb_ptr != NULL );

#if defined(FEATURE_HDR_REVB_QUICKREPEATNAK)
  quick_repeat_entry_ptr = NULL;
#endif
  quick_nak_packed = FALSE;
  seq_length = rrcb_ptr->cfg_ptr->seq_length;

  /*-------------------------------------------------------------------------
    Begin NAK building loop.  Build up each NAK request record and append to
    tail end of signaling message that is being built.
  -------------------------------------------------------------------------*/   
  while ((nak_requests < HDRRLPI_MAX_MULTILINK_NAK_RECORDS) &&
         (q_cnt( &rrcb_ptr->state.multilink_nak_q )) )
  {
    nak_entry_ptr = q_get( &rrcb_ptr->state.multilink_nak_q );
    
    ASSERT( nak_entry_ptr != NULL );
    
    DATA_RLP_MSG7( MSG_LEGACY_MED,
                    "RLP Tx[%d,%d] V[%d,%d] LE,TE[0x%x, 0x%x], Delayed:%d Building Multilink NAK", 
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced,
                    nak_entry_ptr->leading_edge,
                    nak_entry_ptr->trailing_edge,
                    (uint8)nak_entry_ptr->nak_type );

    
    HDRBIT_PACK8( item_ptr, (int)nak_entry_ptr->nak_type, offset, 1 );
    offset++;

    /* Update the stats as we pack the individual nak records */
    if (nak_entry_ptr->nak_type == HDRRLPMLNAK_TYPE_QUICK)
    {
      quick_nak_packed = TRUE;
      rrcb_ptr->stats.at_quick_nak_cnt++;
    }
    else /* Delayed Nak */
    {
      /* Nak window = trailing_edge - leading_edge - 1 */
      window_len = HDRRLPSEQ_DIFF_SEQ( nak_entry_ptr->trailing_edge,
                                       nak_entry_ptr->leading_edge,
                                       rrcb_ptr->cfg_ptr->seq_modulus );

      ASSERT( window_len > 0 );

      rrcb_ptr->stats.at_delayed_nak_cnt++;
      rrcb_ptr->stats.at_delayed_nak_data_unit_cnt += (window_len - 1);

#if defined(FEATURE_HDR_REVB_QUICKREPEATNAK)
      /* Add an entry into the quick_repeat_nak_q */
      if (quick_repeat_entry_ptr == NULL)
      {
        quick_repeat_entry_ptr = hdrrlpmlnak_get_free_quick_repeat_item();
      }

      ASSERT( quick_repeat_entry_ptr != NULL );

      quick_repeat_entry_ptr->leading_edges[ quick_repeat_entry_ptr->nak_count ] 
        = nak_entry_ptr->leading_edge;
      quick_repeat_entry_ptr->trailing_edges[ quick_repeat_entry_ptr->nak_count ] 
        = nak_entry_ptr->trailing_edge;
      quick_repeat_entry_ptr->nak_count++;
#endif    
    }

    HDRBIT_PACK8( item_ptr, nak_entry_ptr->leading_edge_included, offset, 1 );
    offset++;
    
    if (nak_entry_ptr->leading_edge_included)
    {
      HDRBIT_PACK32( item_ptr, nak_entry_ptr->leading_edge, offset, seq_length );
      offset += seq_length;
    }

    HDRBIT_PACK32( item_ptr, nak_entry_ptr->trailing_edge, offset, seq_length );
    offset += seq_length;
    
    nak_requests++;

    /* Return the nak_entry_ptr back to hdrrlpmlnak_free_q */
    hdrrlpmlnak_free_nak_entry( nak_entry_ptr );
  }

#if defined(FEATURE_HDR_REVB_QUICKREPEATNAK)
  /* Store the quick repeat info in the rrcb quick_repeat_q */
  if (quick_repeat_entry_ptr)
  {
    quick_repeat_entry_ptr->rrcb_ptr = rrcb_ptr;
    q_put( &(rrcb_ptr->state.quick_repeat_nak_q), &(quick_repeat_entry_ptr->link) );

    /* Return pointer to the quick_repeat info back to caller */
    *quick_repeat_ptr_ptr = (void*)quick_repeat_entry_ptr;
  }
  else
  {
    *quick_repeat_ptr_ptr = NULL;
  }
#endif  

  /*---------------------------------------------------------------------
    If this MultiLinkNak message reports any erased data units for which 
    the corresponding DelayedNak field is set to ‘0’, then include
    OtherQNCount field and set it to number of QN instances such that
    SAR packets have been received on that instance and LE and TE from
    that QN instance have not been reported in this MultiLink Nak msg.
  ---------------------------------------------------------------------*/
  if (quick_nak_packed)
  {
    other_qn_count = 
      hdrrlpi_get_unreported_qn_instances(rrcb_ptr, largest_sar_unreported);

    HDRBIT_PACK8( item_ptr, other_qn_count, offset, 8 );
    offset += 8;

    for (i = 0; i < other_qn_count; i++)
    {
      HDRBIT_PACK32( item_ptr, largest_sar_unreported[ i ], offset, seq_length );
      offset += seq_length;
    }
  }
  
  /*-------------------------------------------------------------------------
    If OtherQNCount is not included or if OtherQNCount is zero, include V(r).
  -------------------------------------------------------------------------*/
  if (!quick_nak_packed || other_qn_count == 0)
  {
    HDRBIT_PACK32( item_ptr, rrcb_ptr->state.expected_seq_num, offset, seq_length );
    offset += seq_length;
  }

  /* reserved bits to byte align the packet */
  reserved_size = (8 - (offset%8)) % 8;
  HDRBIT_PACK8( item_ptr, (uint16)0, offset, reserved_size );
  offset += reserved_size;

  /*-------------------------------------------------------------------------
    Finished building this NAK message.  Insert the number of NAK requests
    into the head of this packet chain.
  -------------------------------------------------------------------------*/
  (void)dsm_pushdown(&item_ptr, (void *)&nak_requests, 1, DSM_DS_SMALL_ITEM_POOL);
  DATA_RLP_MSG5( MSG_LEGACY_MED,
                  "RLP Tx[%d,%d] V[%d,%d] MultiLinkNAK ReportCount:%d", 
                  rrcb_ptr->cfg_ptr->sr_id, 
                  "AB"[rrcb_ptr->route],
                  rrcb_ptr->cfg_ptr->ver, 
                  rrcb_ptr->cfg_ptr->enhanced,
                  nak_requests);
  rrcb_ptr->stats.tx_naks[0] += nak_requests;

  /*-------------------------------------------------------------------------
   SequenceLength field
   --------------------------------------------------------------------------*/
  (void)dsm_pushdown(&item_ptr, &seq_length, 1, DSM_DS_SMALL_ITEM_POOL);

  /*-------------------------------------------------------------------------
    If the maximum NAK requests have been built into this signaling message, 
    there may be more NAK list entries to handle.  If so, the calling 
    function will call this function again to obtain the remaining NAK 
    requests.  Indicate via boolean return value if this function should be 
    called again.
  -------------------------------------------------------------------------*/
  DSM_TOUCH_ITEM(item_ptr);
  if (nak_requests < HDRRLPI_MAX_MULTILINK_NAK_RECORDS)
  {
    return (FALSE);
  }
  else
  {
    return (TRUE);
  }
    
} /* HDRRLPI_BUILD_MULTILINK_NAK_MSG */

/*===========================================================================

FUNCTION       HDRRLPI_GET_REXMIT_DATA

DESCRIPTION    This function gets the information on the packet to be
               re-transmitted.
               
DEPENDENCIES   This function assumes RLP is in DSRLPI_ESTABLISHED_STATE.
               This function should not be called if RLP cannot transmit new
           data packets.

RETURN VALUE   TRUE if a packet to rexmit has been found.  FALSE otherwise.

SIDE EFFECTS   NONE
===========================================================================*/

boolean hdrrlpi_get_rexmit_data
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* Ptr to RLP ctl block       */
  hdrrlprtx_type               **rexmit_info, /* holds rexmit info          */
  uint16                       *data_len,
  uint8                        combo_index
)
{
  hdrrlprtx_type       *retx_ptr;     
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - -*/

  /*-------------------------------------------------------------------------
    DO_REV_A_RLP.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Get info from rexmit queue on the packet needs retransmitting.
  -------------------------------------------------------------------------*/
  retx_ptr = hdrrlprtx_get_info_next_retx_frame( rrcb_ptr, combo_index );
  if (  retx_ptr  == NULL )
  {
    DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                    "RLP TxRexmit[%d,%d] V[%d,%d] No packets to rexmit, but in rexmit func, The TX Element may have been free'ed up", 
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    return FALSE;
  }                               
  
  
  
  *rexmit_info = retx_ptr;
  *data_len = (uint16)dsm_length_packet(((hdrrlptxq_type*)(retx_ptr->tx_queue_item_ptr))->dsm_chain_ptr)  
                 - hdrrlp_get_header_size(rrcb_ptr->cfg_ptr);
  return (TRUE);
} /* HDRRLPI_GET_REXMIT_DATA */

/*===========================================================================

FUNCTION HDRRLPI_TX_REXMIT

DESCRIPTION
  This function is called as part of building RLP data packets for transmit.
  It is used to build up a packet for retransmission by looking into the
  retransmit list.

  The size of the RLP packet is dependent on the  maximum number of bytes 
  that is passed in as a parameter.  If the packet to retransmit is larger
  than the allowed size, then the packet is split.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet that is built, including headers.  A zero-length value
  indicates that the packet could not be found.

SIDE EFFECTS
  The *rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
uint16 hdrrlpi_tx_rexmit
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,      /* ptr to RLP specific ctl blk  */
  dsm_item_type            **rev_pkt_ptr,  /* ptr to pkt to build          */
  hdrrlprtx_pkt_handle_type   *handle_ptr,    /* ptr to handle that shd accompany data */
  uint16                   max_bytes,        /* max size allowed pk to build */         
  hdrrlprtx_reason_enum_type *reason,
  uint8                    combo_index

)
{
  hdrrlptxq_type          *txq_ptr;  /* struct passed to txq func   */
  hdrrlprtx_type          *rexmit_info;   /* holds rexmit info           */
  dsm_item_type           *pkt_item_ptr;    /* temp. working DSM item ptr. */
  uint32                  nak_first_seq;    /* first seq in rexmit list    */
  uint32                  nak_last_seq;     /* last seq in rexmit list     */
  uint32                  last_seq_sent;
  uint32                  pkt_first_seq;    /* seq of first byte to rexmit */
  uint32                  pkt_last_seq;     /* seq of last byte to rexmit  */
  uint32                  seq_modulus;      /* sequence number modulus     */
  uint16                  data_payload_len; /* length of payload to copy   */
  uint16                  copy_len;         /* amount of data copied       */
  uint16                  dsm_offset;       /* offset into DSM item to copy*/
  uint16                  pkt_copy_len;     /* amount to copy into current */
  uint8                   seq_buf[4];
  uint8                   rlp_header_size;  /* Size of the RLP header      */
  uint16                  offset;
  boolean                 first_data_unit = FALSE;
  boolean                 last_data_unit = FALSE;
  hdrrlp_debug_ctl        *hdrrlp_debug_ptr = NULL;
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( combo_index < HDRCP_PCP_MAX_COMBOS );
  
  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();
  /*-------------------------------------------------------------------------
    Determine if there are any packets that need retransmission by looking
    into the retransmit queue.  If no retransmissions are required, return
    from the function indicating that there are no bytes were copied.  The
    function fills in the txq_item structure from the transmit queue, and
    the rexmit_info structure from the re-transmit list.
  -------------------------------------------------------------------------*/
  if (hdrrlpi_get_rexmit_data(rrcb_ptr, &rexmit_info, &data_payload_len, combo_index) == FALSE)
  {
    return (0);
  }
  ASSERT( ( data_payload_len > 0 ) &&
          ( rexmit_info != NULL ) ) ;
        
  /*-------------------------------------------------------------------------
    If there are rexmit data that needs to be sent, then at this point 
    the tx_queue_ptr need to be filled in, if it not present ( as the rexmit
    info was as a result of a NAK ) then the above function call should 
    fill this up. 
  -------------------------------------------------------------------------*/        
  ASSERT(rexmit_info->tx_queue_item_ptr != NULL );  
  txq_ptr = (hdrrlptxq_type*)rexmit_info->tx_queue_item_ptr;
  ASSERT(txq_ptr->dsm_chain_ptr);
  seq_modulus = rrcb_ptr->cfg_ptr->seq_modulus;
  /*-------------------------------------------------------------------------
    The packet to rexmit has been found.  Use some local vars for 
    readability.  Perform calculations on how much data will be copied.  
    Initially determine the seq num of the last byte in the dsm item chain.
  -------------------------------------------------------------------------*/
  nak_first_seq    = HDRRLPSEQ_ADD_SEQ( rexmit_info->first_seq, 
                                        rexmit_info->next_seq_tx_offset[combo_index], 
                                        seq_modulus);
  nak_last_seq     = rexmit_info->last_seq;

  

  
  
  /*-------------------------------------------------------------------------
    Determine the size of the RLP header based on the version of RLP.
    If it is the default Packet Application, then the RLP header is just
    the length of the sequence number plus stream header (22 bits + 2 bits). 
    If it is the multiflow RLP, then the header length is the length of the
    sequence number, the RLP ID and the stream header
  -------------------------------------------------------------------------*/
  rlp_header_size = hdrrlp_get_header_size(rrcb_ptr->cfg_ptr);
  if (rlp_header_size  > 4 )
  {
    DATA_RLP_MSG0( MSG_LEGACY_ERROR, "Bad RLP header" );
    return (0);
   }

  /*-------------------------------------------------------------------------
    First sequence number of packet found in transmit queue.
  -------------------------------------------------------------------------*/
  pkt_first_seq     = txq_ptr->seq_num;


  /*-------------------------------------------------------------------------
    Obtain the last sequence number by adding payload length to the first
    sequence number in modulo arithmetic.
  -------------------------------------------------------------------------*/
  pkt_last_seq = HDRRLPSEQ_ADD_SEQ(pkt_first_seq, 
                                    HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, data_payload_len),
                                    seq_modulus);

  /* EMPA segment sequencing: retx entire data units (may be multiple octets) */
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
      rrcb_ptr->cfg_ptr->segment_sequencing)
  {
    pkt_copy_len = (uint16)dsm_length_packet(txq_ptr->dsm_chain_ptr);

    /* Can't fragment a single data unit */
    if (pkt_copy_len > max_bytes) 
    {
      DATA_RLP_MSG6( MSG_LEGACY_HIGH,
                      "RLP TxRexmit[%d,%d] V[%d,%d] Not Enough Room in stream to send retx data unit (%d > %d)", 
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced,
                      pkt_copy_len,
                      max_bytes);
      /*-----------------------------------------------------------------------
        Try again later.  Return 0 bytes.
        -----------------------------------------------------------------------*/
      return (0);
    }

    if (dsm_dup_packet(&pkt_item_ptr, txq_ptr->dsm_chain_ptr, 0, pkt_copy_len) !=
        pkt_copy_len)
    {
      DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                      "RLP TxRexmit[%d,%d] V[%d,%d] Could not dup a packet successfully", 
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);

      return (0);                      
    }
    /* Subsequent code assumes payload length only */
    pkt_copy_len -= rlp_header_size;
  }
  else
  {
    /*-------------------------------------------------------------------------
      Now determine how many bytes are to be copied out of this DSM packet 
      chain.  Use some local vars for readability.
    -------------------------------------------------------------------------*/
    if (HDRRLPSEQ_GE_SEQ_CHECK(nak_last_seq, pkt_last_seq, seq_modulus))
    {
      /*-----------------------------------------------------------------------
        Copy from nak first sequence number to end of packet.
      -----------------------------------------------------------------------*/
      copy_len = (uint16)HDRRLPSEQ_DIFF_SEQ(pkt_last_seq, nak_first_seq, seq_modulus);
    }
    else
    {
      /*-----------------------------------------------------------------------
      Copy from nak first sequence number to nak last sequence number.
      -----------------------------------------------------------------------*/
      copy_len = (uint16)HDRRLPSEQ_DIFF_SEQ(nak_last_seq, nak_first_seq, seq_modulus);
    }

    /*-------------------------------------------------------------------------
      Figure out offset into dsm packet chain to figure out where to start
      copying the data payload.  Account for any header that may be in the
      packet.
    -------------------------------------------------------------------------*/
    dsm_offset = (uint16)HDRRLPSEQ_DIFF_SEQ(nak_first_seq, pkt_first_seq, seq_modulus);
    dsm_offset += rlp_header_size;

    /*-------------------------------------------------------------------------
      Calculate how much can be copied into the packet.  Note that the
      HDRRLP_HEADER_SIZE includes the 2-bit stream ID bits.
    -------------------------------------------------------------------------*/
    pkt_copy_len = MIN(copy_len, (max_bytes - rlp_header_size));

    /*-------------------------------------------------------------------------
     In case we are transmitting less than the max_bytes allowed, then it 
      means that the packet will go as a Format B packet. In this case, the
      format B packet maximum allowed size is 255. Thus, we need to restrict 
      the packet copy length to be 255- rlp_header.
    -------------------------------------------------------------------------*/
    if( pkt_copy_len < (max_bytes - rlp_header_size))
    {
      pkt_copy_len = MIN( pkt_copy_len, (HDRPCP_FMT_B_MAX_PKT_SIZE - rlp_header_size));
    }
    if (pkt_copy_len == 0)
    {
      DATA_RLP_MSG4( MSG_LEGACY_FATAL,
                      "RLP TxRexmit[%d,%d] V[%d,%d] Illegal pkt_copy_len in rexmit data", 
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);
      hdrrlprtx_reset(rrcb_ptr);
      return (0);
    }

    ASSERT(pkt_copy_len > 0);

    if (dsm_dup_packet(&pkt_item_ptr, txq_ptr->dsm_chain_ptr, dsm_offset, pkt_copy_len) !=
        pkt_copy_len)
    {
      DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                      "RLP TxRexmit[%d,%d] V[%d,%d] Could not dup a packet successfully", 
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);

      return (0);                      

    }

    /* EMPA/MMPA packet stream: indicate the first/last segments of the Route Protocol packet */
    if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
         rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
         rrcb_ptr->cfg_ptr->packet_stream)
    {
      if (nak_first_seq == pkt_first_seq)
      {
        offset = HDRMRLP_STREAM_HEADER_SIZE+rrcb_ptr->cfg_ptr->rlp_id_len_in_pkt+HDRRLP_EMPA_ROUTE_LEN+
          rrcb_ptr->cfg_ptr->seq_length;
        (void)dsmbit_unpack8(txq_ptr->dsm_chain_ptr, &first_data_unit, offset, HDRRLP_EMPA_FDU_LEN);
      }
      if (nak_last_seq == pkt_last_seq)
      {
        offset = HDRMRLP_STREAM_HEADER_SIZE+rrcb_ptr->cfg_ptr->rlp_id_len_in_pkt+HDRRLP_EMPA_ROUTE_LEN+
          rrcb_ptr->cfg_ptr->seq_length+HDRRLP_EMPA_LDU_LEN;
        (void)dsmbit_unpack8(txq_ptr->dsm_chain_ptr, &last_data_unit, offset, HDRRLP_EMPA_LDU_LEN);
      }
    }

  /*-------------------------------------------------------------------------
    Start building up the packet.  Add the 22-bit starting sequence number 
    for the packet into the first 3 bytes of the item.  
  -------------------------------------------------------------------------*/
    HDRRLPI_PACK_RLP_HEADER(rrcb_ptr->cfg_ptr,
                            rrcb_ptr->route,
                            nak_first_seq,
                            first_data_unit,
                            last_data_unit,
                            seq_buf);
  

    if ( dsm_pushdown(&pkt_item_ptr, seq_buf, rlp_header_size, 
                 DSM_DS_SMALL_ITEM_POOL) != rlp_header_size )
    {
      dsm_free_packet( &pkt_item_ptr );
      DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                      "RLP TxRexmit[%d,%d] V[%d,%d] Could not dup a packet successfully", 
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);

      return (0);                      
    }

    if (first_data_unit == TRUE)
    { 
      rrcb_ptr->stats.tx_first_data_unit++;
    }
    if (last_data_unit == TRUE)
    {
      rrcb_ptr->stats.tx_last_data_unit++;
    }

  }

  *rev_pkt_ptr = pkt_item_ptr;

  /*-------------------------------------------------------------------------
    Update the retransmit list.  Refresh on what the vars mean:
    
    nak_first_seq = the seq num of the first byte to transmit, as requested
                    by the peer.
                    
    nak_last_seq  = the seq num of the last byte to transmit, as requested
                    by the peer.  This is calculated from the WindowLen
                    value received by the peer.

    pkt_copy_len      = number of bytes we wanted to copy.
  -------------------------------------------------------------------------*/
  last_seq_sent = HDRRLPSEQ_ADD_SEQ(nak_first_seq, 
                               HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, pkt_copy_len),
                               seq_modulus);
  if (hdrrlp_debug_ptr->enable_tx_debug) 
  {
    DATA_RLP_MSG8( MSG_LEGACY_HIGH,
                  "RLP TxRexmit[%d,%d] V[%d,%d] Seq#[0x%x,0x%x), len:0x%x, Type:%d", 
                  rrcb_ptr->cfg_ptr->sr_id, 
                  "AB"[rrcb_ptr->route],
                  rrcb_ptr->cfg_ptr->ver, 
                  rrcb_ptr->cfg_ptr->enhanced,
                  nak_first_seq,
                  last_seq_sent,
                  pkt_copy_len,
                  rexmit_info->reason );
  }                  
  
  /*-------------------------------------------------------------------------
   DO_REV_A_RLP.
  -------------------------------------------------------------------------*/
  /*------------------------------------------------------------------------
    Set the fields in the handle that will help identify the built data when
    status regarding it's transmission is received.
  ------------------------------------------------------------------------*/
  handle_ptr->first_seq = nak_first_seq;
  handle_ptr->pkt_copy_len = pkt_copy_len;
  handle_ptr->retx_item_ptr = rexmit_info;
  *reason = rexmit_info->reason;
  

  /*------------------------------------------------------------------------
  The retransmitted packet (dsm item) is duped and saved on handle's 
  head_item_ptr. When the PCP calls status callback, it returns the handle. 
  The dsm item is processed (if necessary) and freed. 
  This mechanism is to facilitate the RLP logging mechanism.
  The dup is made including the rlp_header

  If cannot dup, free the created item, and return. the handle is freed by
  the caller.
  ------------------------------------------------------------------------*/
  if ( dsm_dup_packet( &(handle_ptr->head_item_ptr), 
                        pkt_item_ptr, 
                        0, 
                        (pkt_copy_len+rlp_header_size) ) 
                                       != (pkt_copy_len+rlp_header_size ) )
  {
     dsm_free_packet( &pkt_item_ptr );
     DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                  "RLP TxRexmit[%d,%d] V[%d,%d] Cannot Dup ", 
                  rrcb_ptr->cfg_ptr->sr_id, 
                  "AB"[rrcb_ptr->route],
                  rrcb_ptr->cfg_ptr->ver, 
                  rrcb_ptr->cfg_ptr->enhanced);
    return(0);                  
  }
                        
  if (hdrrlp_debug_ptr->enable_tx_debug) 
  {
    DATA_RLP_MSG6( MSG_LEGACY_MED,
                  "RLP TxRexmit[%d,%d] V[%d,%d] Handle Info Seq#[0x%x,), len:0x%x ", 
                  rrcb_ptr->cfg_ptr->sr_id, 
                  "AB"[rrcb_ptr->route],
                  rrcb_ptr->cfg_ptr->ver, 
                  rrcb_ptr->cfg_ptr->enhanced,
                  handle_ptr->first_seq,
                  handle_ptr->pkt_copy_len);
  }                
  
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
       rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
       rrcb_ptr->cfg_ptr->segment_sequencing)
  {
	  /*-------------------------------------------------------------------------
      For segment sequencing, one segment is sent so increment the tx_offset
      but NUM DATA UNITS
	  -------------------------------------------------------------------------*/
	  rexmit_info->next_seq_tx_offset[combo_index] += HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, pkt_copy_len);
  }
  else
  {
    /*-------------------------------------------------------------------------
      So far nak_first_seq and nak_last_seq are the ones from the retx item. 
      
      Increment nak_first_seq and see if all requested bytes have been
      rexmitted.  Update list accordingly.
    -------------------------------------------------------------------------*/
    nak_first_seq = HDRRLPSEQ_ADD_SEQ(nak_first_seq, 
                                      HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, pkt_copy_len),
                                      seq_modulus);
    /*-------------------------------------------------------------------------
      Update the offset 
    -------------------------------------------------------------------------*/                                  
    rexmit_info->next_seq_tx_offset[combo_index] += HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, pkt_copy_len);                                  
    if (HDRRLPSEQ_GE_SEQ_CHECK(nak_first_seq, nak_last_seq, seq_modulus))
    {
		  /*-------------------------------------------------------------------------
		    Entire NAK entry is rexmitted So set the awaiting status for this retransmit 
		    entry to be TRUE.		
		  -------------------------------------------------------------------------*/
      ASSERT(nak_first_seq == nak_last_seq);
      if ( HDRRLPSEQ_ADD_SEQ(rexmit_info->first_seq, 
                                      rexmit_info->next_seq_tx_offset[combo_index],
                                      seq_modulus) != rexmit_info->last_seq )
      {
        /* rexmit_info->first_seq + rexmit_info->next_seq_tx_offset[combo_index] != rexmit_info->last_seq */
		
        ASSERT(0);
      }                                            
    }
    else
    {
      if (  HDRRLPSEQ_GE_SEQ_CHECK (
                                    HDRRLPSEQ_ADD_SEQ(rexmit_info->first_seq, 
                                        rexmit_info->next_seq_tx_offset[combo_index],
                                        seq_modulus),
                                    rexmit_info->last_seq,
                                    seq_modulus ) )
      {
        /* rexmit_info->first_seq + rexmit_info->next_seq_tx_offset[combo_index] >= rexmit_info->last_seq */
		
        ASSERT(0);
      }                                        		  
    }
  }

  /*-------------------------------------------------------------------------
    Return the total number of bytes copied.
  -------------------------------------------------------------------------*/
  return (pkt_copy_len + rlp_header_size);  

} /* HDRRLPI_TX_REXMIT */

/*===========================================================================

FUNCTION HDRRLPI_TX_NEW_DATA

DESCRIPTION
  This function is called as part of building RLP data packets for transmit.
  It is used to build up a new data packet for transmission from the 
  available data in the transmit watermark.

  The size of the RLP packet is dependent on the  maximum number of bytes 
  that is passed in as a parameter.  

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet that is built, including headers.  A zero-length value
  indicates that no data was available for transmit.

SIDE EFFECTS
  The *rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
uint16 hdrrlpi_tx_new_data
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,      /* ptr to RLP specific ctl blk  */
  dsm_item_type            **rev_pkt_ptr,  /* ptr to pkt to build          */
  hdrrlpi_new_data_pkt_handle_type *handle_ptr,    /* ptr to handle that   *
                                                    * shd accompany data   */
  uint16                   max_bytes,      /* max size allowed pk to build */         
  uint8                    combo_index

)
{
  dsrlpi_state_info_type  *state_ptr;       /* local state ptr             */
  dsm_item_type           *head_item_ptr=NULL; /* head of pkt chain        */
  dsm_item_type           *tail_item_ptr=NULL; /* end of pkt chain         */
  dsm_item_type           *new_pkt_item_ptr=NULL; /* new pkt chain         */
  dsm_item_type           *tmp_item_ptr; 

  /*-------------------------------------------------------------------
  DO_REV_A_RLP.
  -------------------------------------------------------------------*/
  uint16                  bytes_to_copy;    /* number of bytes to copy     */
  uint16                  total_bytes=0;    /* total bytes from wm         */
  uint16                  payload_in_packet; /* length of payload in pkt   */
  uint32                  payload_in_wm=0;   /* length of payload in wm    */
  uint8                   rlp_header_size;  /* Size of the RLP header      */ 
  boolean                 first_data_unit = FALSE;
  boolean                 last_data_unit = FALSE;
  uint32                  start_sequence_number = 0;
  hdrrlp_debug_ctl        *hdrrlp_debug_ptr = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( combo_index < HDRCP_PCP_MAX_COMBOS );

  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();
  /*-------------------------------------------------------------------------
    Initializing variables that will be used later in the function.
  -------------------------------------------------------------------------*/
  state_ptr           = &rrcb_ptr->state;

  /*-----------------------------------------------------------------------
   Count of handles on which tx layer reported status. This must be zero
   when building a new batch of handles, as it is cleared after the last
   handle's status is reported. 
   ----------------------------------------------------------------------------*/
  /*lint -save -e527 suppress code unreachable warning 527*/
  if (state_ptr->local_state[combo_index].new_handle_status_rcvd_count != 0)
  {
    ERR_FATAL("Error new handles_built_count:%d state_handle_count:%d", 
            state_ptr->local_state[combo_index].new_handles_built_count, 
            state_ptr->local_state[combo_index].new_handle_status_rcvd_count,
            0);
    return(0);
  }
  /*lint -restore suppress code unreachable warning 527*/


  /*-------------------------------------------------------------------------
    Are we beginning to build a new set of handles? If yes then 
    handles_built_count should be zero. Initialize the local_seek_offset to
    seek_offset (since last batch of handles built).
  -------------------------------------------------------------------------*/
  if (state_ptr->local_state[combo_index].new_handles_built_count == 0) 
  {
    state_ptr->local_state[combo_index].seek_offset = state_ptr->seek_offset;
    state_ptr->local_state[combo_index].next_seq_num = state_ptr->next_seq_num;
  }
  else
  {
    //TODO: See what can we do about this HDRRLPI_TX_ITEM_Q_LOCAL_LEN_EXCL() iterations.
    /*-------------------------------------------------------------------------
      We are in the middle of a batch of handles being built for PCP.
      Check the local_seek_offset is later than seek_offset and not over
      the number of bytes in tx_item_q.
    -------------------------------------------------------------------------*/
    /*lint -save -e527 suppress code unreachable warning 527*/
    if ((state_ptr->local_state[combo_index].seek_offset <= state_ptr->seek_offset) ||
        (state_ptr->local_state[combo_index].seek_offset > HDRRLPI_TX_ITEM_Q_LOCAL_LEN_EXCL(rrcb_ptr, combo_index)))
    {
      ERR_FATAL("Incorrect tx new data handles_built_count:%d", 
          state_ptr->local_state[combo_index].new_handles_built_count, 0,0);
      return (0);
    }
    /*lint -restore suppress code unreachable warning 527*/

  }
  /*---------------------------------------------------------------------
   EMPA segment sequencing: if retransmissions are enabled, LIMIT to 
   255 bytes to force this RLP packet into a Format B connection 
   layer packet, as it is hard to flush or retransmit a Format A 
   connection layer packet unless the channel conditions are exactly 
   the same 
   ----------------------------------------------------------------------*/
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
      rrcb_ptr->cfg_ptr->segment_sequencing &&
      rrcb_ptr->cfg_ptr->nak_enabled )
  {
    max_bytes = (uint16)MIN(max_bytes, HDRPCP_FMT_B_MAX_PKT_SIZE);
  }

  /*-------------------------------------------------------------------------
   Determine the size of the RLP header based on the version of RLP.
  -------------------------------------------------------------------------*/
  rlp_header_size = hdrrlp_get_header_size(rrcb_ptr->cfg_ptr);
  if (rlp_header_size >= max_bytes)
  {
    DATA_RLP_MSG6( MSG_LEGACY_MED,
                          "RLP Tx[%d,%d] V[%d,%d] rlp_header_size >= max_bytes, can't send anything ( %d >= %d )", 
                          rrcb_ptr->cfg_ptr->sr_id, 
                          "AB"[rrcb_ptr->route],
                          rrcb_ptr->cfg_ptr->ver, 
                          rrcb_ptr->cfg_ptr->enhanced,
                          rlp_header_size,
                          max_bytes );
    return(0); 
  }

  /*-------------------------------------------------------------------------
    Special processing if on a packet boundary in the tx_item_q.
   -------------------------------------------------------------------------*/
  if (state_ptr->local_state[combo_index].seek_offset == HDRRLPI_TX_ITEM_Q_LOCAL_LEN_EXCL(rrcb_ptr, combo_index))
  {
    /*-------------------------------------------------------------------------
      If the current tx_item_q is consumed, get a new IP packet from the 
      transmit watermark.  Items should be available in the watermark, 
      otherwise this function should not have been called.
    -------------------------------------------------------------------------*/
    if (state_ptr->local_state[combo_index].seek_offset == HDRRLPI_TX_ITEM_Q_LEN(rrcb_ptr))
    {
      if (HDRRLPI_DEQUEUE_TX_ITEM(rrcb_ptr) == FALSE)
      {
        return (0);
      }
    }

    /*-------------------------------------------------------------------------
     EMPA packet stream: if we are here, then local_seek_offset is at 
     the start of a new Route Protocol packet 
     -------------------------------------------------------------------------*/
    if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
        rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
        rrcb_ptr->cfg_ptr->packet_stream)
    {

      /*--------------------------------------------------------------------
       EMPA segment sequencing: Don't fragment a Route Protocol 
       packet than can be transmitted in a 256 bit OTA frame (RLP 
       packet = 28 bytes min).  This is designed to aid a (future) 
       receiver that is able to utilize "link layer assisted" RoHC 
       compressed EVRC VoIP. 
       ------------------------------------------------------------------*/
      if (rrcb_ptr->cfg_ptr->segment_sequencing) 
      {

        /* compute the length of the new Route Protocol packet */
        tmp_item_ptr = (dsm_item_type *)q_check(&rrcb_ptr->state.tx_item_q);
        ASSERT(tmp_item_ptr != NULL);

        payload_in_packet = (uint16)dsm_length_packet(tmp_item_ptr);
        payload_in_wm = payload_in_packet;
        while ((payload_in_wm <= rrcb_ptr->state.local_state[combo_index].seek_offset) &&
               ((tmp_item_ptr = (dsm_item_type *)q_next(&rrcb_ptr->state.tx_item_q, &tmp_item_ptr->link)) != NULL))
        {
          payload_in_packet = (uint16)dsm_length_packet(tmp_item_ptr);
          payload_in_wm += payload_in_packet;
        }

        if (payload_in_packet <= HDRRLPI_MIN_RP_SIZE_FOR_FRAG(rrcb_ptr->cfg_ptr) &&
            payload_in_packet > (max_bytes - rlp_header_size))
        {
          DATA_RLP_MSG7( MSG_LEGACY_MED,
                          "RLP Tx[%d,%d] V[%d,%d]  Seq#[0x%x,) Route Protocol Packet too small to fragment ( %d > %d )", 
                          rrcb_ptr->cfg_ptr->sr_id, 
                          "AB"[rrcb_ptr->route],
                          rrcb_ptr->cfg_ptr->ver, 
                          rrcb_ptr->cfg_ptr->enhanced,
                          state_ptr->local_state[combo_index].next_seq_num,
                          (payload_in_packet + rlp_header_size),
                          max_bytes );
          /*-----------------------------------------------------------------------
            Try again later.  Return 0 bytes.
          -----------------------------------------------------------------------*/
          return (0);
        }
      }

      /* indicate the first segment of the Route Protocol packet */
      first_data_unit = TRUE;
    } /* If EMPA Packet Stream */ 
  } /* If on packet boundary */

  if(state_ptr->local_state[combo_index].seek_offset >= 
                      HDRRLPI_TX_ITEM_Q_LOCAL_LEN_INCL(rrcb_ptr, combo_index))
  {
    ASSERT(0);
    return 0;
  }

  /*-------------------------------------------------------------------------
    payload_in_packet contains the maximum number of RLP data bytes allowed
    to transmit.  payload_in_wm contains the number of bytes available in
    the watermark (limited to one route protocol packet in an EMPA packet stream).
  -------------------------------------------------------------------------*/
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
      rrcb_ptr->cfg_ptr->packet_stream)
  {
    payload_in_wm   = HDRRLPI_TX_ITEM_Q_LOCAL_LEN_INCL(rrcb_ptr, combo_index) - 
                                            state_ptr->local_state[combo_index].seek_offset;
  }
  else
  {
    payload_in_wm   = HDRRLPI_TX_ITEM_Q_LEN(rrcb_ptr) - state_ptr->local_state[combo_index].seek_offset;
    payload_in_wm  += hdrrlpi_get_new_data_count(rrcb_ptr);
  }

  /*-------------------------------------------------------------------------
    In case we are transmitting less than the max_bytes allowed, then it 
    means that the packet will go as a Format B packet. In this case, the
    format B packet maximum allowed size is 255. Thus, we need to restrict 
    the packet copy length to be 255- rlp_header.
  -------------------------------------------------------------------------*/
  if( payload_in_wm < (uint32)(max_bytes - rlp_header_size))
  {
    payload_in_packet = (uint16)MIN( payload_in_wm, 
                             (uint8)(HDRPCP_FMT_B_MAX_PKT_SIZE - rlp_header_size));
  }
  else
  {
    /*-------------------------------------------------------------------------
       we are able to build a pkt as per PCP's given size.
    -------------------------------------------------------------------------*/
    payload_in_packet = max_bytes - rlp_header_size;
  }

  /*----------------------------------------------------------------------------
   EMPA packet stream: indicate the last segment of the Route Protocol packet 
   ----------------------------------------------------------------------------*/
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
      rrcb_ptr->cfg_ptr->packet_stream &&
      (payload_in_packet == payload_in_wm))
  {
    last_data_unit = TRUE;
  }

  /*-------------------------------------------------------------------------
    Since we know the size of the payload, get the bytes from the watermark 
    so that we can start packing the frame.
  -------------------------------------------------------------------------*/
  while((HDRRLPI_TX_ITEM_Q_LEN(rrcb_ptr) - state_ptr->local_state[combo_index].seek_offset)
        < payload_in_packet)
  { 
     /*-------------------------------------------------------------------
       EMPA packet stream: this code should not be reached since we 
       never transmit more that one higher layer packet in an RLP
       packet. 
     ----------------------------------------------------------------*/
      ASSERT(!((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
                rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
                rrcb_ptr->cfg_ptr->packet_stream));

    /*-------------------------------------------------------------------
     If there is no new data in this new dsm chain from watermark,
     then break from the loop.
    -------------------------------------------------------------------*/
    if (HDRRLPI_DEQUEUE_TX_ITEM(rrcb_ptr) == FALSE) 
    {
      break;
    }
  }

 /*-------------------------------------------------------------------
    Get DSM memory item for building the packet.
    This is not the entire memory for the, if we require more memory 
    we get it later
  -------------------------------------------------------------------*/
  if(hdrrlpi_get_new_buffer(&new_pkt_item_ptr, max_bytes) == FALSE)
  {
     return 0;
  }
  tail_item_ptr = head_item_ptr = new_pkt_item_ptr;

  
  /*-------------------------------------------------------------------------
    Start building up the packet.  Add the 22-bit starting sequence number 
    for the packet into the first 3 bytes of the item.  
    -------------------------------------------------------------------------*/
  start_sequence_number = state_ptr->local_state[combo_index].next_seq_num;
  HDRRLPI_PACK_RLP_HEADER(rrcb_ptr->cfg_ptr,
                          rrcb_ptr->route,
                          start_sequence_number,
                          first_data_unit,
                          last_data_unit,
                          new_pkt_item_ptr->data_ptr);

  new_pkt_item_ptr->used += rlp_header_size;
  total_bytes += rlp_header_size;

  /*-------------------------------------------------------------------------
    Set the overhead header bytes to the size of the packet that has been 
    built until now. (only header bytes have been filled).
    Fill in payload from the tx_item_q into the new packet. 
    payload_in_packet is the number of bytes to be copied.

  -------------------------------------------------------------------------*/
  while (payload_in_packet > 0)
  {
    /*-----------------------------------------------------------------------
      Get a new item for the RLP packet if the old one is all used up
    -----------------------------------------------------------------------*/
    if ( new_pkt_item_ptr->size == new_pkt_item_ptr->used)
    {
      if (hdrrlpi_get_new_buffer(&new_pkt_item_ptr, payload_in_packet) == FALSE)
      {
        /*--------------------------------------------------------------------
          We are not able to build a packet because of memory constraints.
          Reset the RLP state variables, since we changed only the 
          local_nex_seq_num, before this point we reset it.
        --------------------------------------------------------------------*/
        dsm_free_packet( &head_item_ptr); 
        return 0;
      }
      tail_item_ptr->pkt_ptr = new_pkt_item_ptr;
      tail_item_ptr          = new_pkt_item_ptr;
    } /* if need more new items */

    /*-----------------------------------------------------------------------
      Pull up bytes from the tx_item_q. Number of bytes pulled up is the 
      minimum of the payload space, length in tx_item_q and available
      room in the new_pkt_item_ptr (limited to one route protocol packet 
      in an EMPA packet stream).
    -----------------------------------------------------------------------*/

    ASSERT( new_pkt_item_ptr->size >= new_pkt_item_ptr->used);
    bytes_to_copy = MIN( payload_in_packet, 
                          (new_pkt_item_ptr->size - new_pkt_item_ptr->used));

    HDRRLPI_TX_ITEM_Q_EXTRACT(rrcb_ptr,
                              state_ptr->local_state[combo_index].seek_offset,
                              new_pkt_item_ptr->data_ptr+new_pkt_item_ptr->used,
                              bytes_to_copy);

    state_ptr->local_state[combo_index].seek_offset += bytes_to_copy;    
    payload_in_packet      -= bytes_to_copy;
    new_pkt_item_ptr->used += bytes_to_copy;   
    total_bytes += bytes_to_copy;

    ASSERT( new_pkt_item_ptr->size >= new_pkt_item_ptr->used);
  } /* while copying bytes */

  if ( payload_in_packet > 0)
  {
    DATA_RLP_MSG5( MSG_LEGACY_ERROR,
                    "RLP Tx[%d,%d] V[%d,%d] %d bytes missing from watermark on making RLP Packet", 
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced,
                    payload_in_packet);
  /*--------------------------------------------------------------------
      We are not able to build a packet because of memory constraints. 
      Reset the RLP state variables and also free the packet.
    ---------------------------------------------------------------------*/
    dsm_free_packet( &head_item_ptr);
    state_ptr->local_state[combo_index].seek_offset -= (total_bytes - rlp_header_size);

    return(0);
  }

  /*------------------------------------------------------------------------
    F3 message printing of the packet build.
  ------------------------------------------------------------------------*/
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
      rrcb_ptr->cfg_ptr->packet_stream)
  {
    if (first_data_unit == TRUE)
    {
      rrcb_ptr->stats.tx_first_data_unit++;
    }
    if (last_data_unit == TRUE)
    {
      rrcb_ptr->stats.tx_last_data_unit++;
    }
  }

  if (hdrrlp_debug_ptr->enable_tx_debug) 
  {
    DATA_RLP_MSG9( MSG_LEGACY_MED,
                    "RLP Tx[%d,%d] V[%d,%d] Seq#[0x%x,0x%x) fdu:%d, ldu:%x, len:%d Tx New Data", 
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced,
                    state_ptr->local_state[combo_index].next_seq_num,
                    HDRRLPSEQ_ADD_SEQ( state_ptr->local_state[combo_index].next_seq_num,
                                        HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr,
                                        (total_bytes - rlp_header_size)),
                                        rrcb_ptr->cfg_ptr->seq_modulus ),
                    first_data_unit,
                    last_data_unit,
                    (total_bytes - rlp_header_size));
  }
  /*-------------------------------------------------------------------------
    Duplicate this packet chain and give the DUP to PCP.  By duplicating, the
    PCP layer is able to free the packet without consequence. The packet
    chain will be deleted from the transmit queue as
    part of transmit queue management.
  -------------------------------------------------------------------------*/
  if (head_item_ptr != NULL)
  {
    if (dsm_dup_packet(&tmp_item_ptr, head_item_ptr, 0, total_bytes) !=
        total_bytes)
    {
      DATA_RLP_MSG4( MSG_LEGACY_ERROR,
                      "RLP Tx[%d,%d] V[%d,%d] Cannot Dup", 
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);
    /*--------------------------------------------------------------------
        We are not able to build a packet because of memory constraints. 
        Reset the RLP state variables and also free the packet.
      ---------------------------------------------------------------------*/
      dsm_free_packet( &head_item_ptr);
      state_ptr->local_state[combo_index].seek_offset -= (total_bytes - rlp_header_size);

      return(0);

    }

    if ( tmp_item_ptr  == NULL )
    {
      DATA_RLP_MSG0( MSG_LEGACY_FATAL, "DSM item allocation failed" );
      dsm_free_packet(&head_item_ptr);
      state_ptr->local_state[combo_index].seek_offset -= (total_bytes - rlp_header_size);
      return 0;
    }
      
    HDRCOM_DSM_SET_TRACER(tmp_item_ptr, HDRCOM_DSM_RLP_TX);

    *rev_pkt_ptr = tmp_item_ptr;

    /*----------------------------------------------------------------------------
      Since we are successful in building the packet, lets do some 
      book keeping stuff.
    ----------------------------------------------------------------------------*/
    state_ptr->local_state[combo_index].next_seq_num = 
                      HDRRLPSEQ_ADD_SEQ( state_ptr->local_state[combo_index].next_seq_num,
                                          HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, 
                                                                  (total_bytes - rlp_header_size)),
                                          rrcb_ptr->cfg_ptr->seq_modulus );
    /*----------------------------------------------------------------------------
     Counter to track the number of handles built so far for PCP in this subframe.
    ----------------------------------------------------------------------------*/
    state_ptr->local_state[combo_index].new_handles_built_count += 1; 

    /*------------------------------------------------------------------------
    Set the fields in the handle that will help identify the built data when
    status regarding it's transmission is received.
    ------------------------------------------------------------------------*/
    handle_ptr->head_item_ptr = head_item_ptr;
    handle_ptr->payload_size = (total_bytes - rlp_header_size);
    handle_ptr->ID = state_ptr->local_state[combo_index].new_handles_built_count;
    if (hdrrlp_debug_ptr->enable_tx_debug>1) 
    {
      DATA_RLP_MSG6( MSG_LEGACY_MED,
                      "RLP Tx[%d,%d] V[%d,%d] Assigned HandleID:%d, combo_index:%d",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced,
                      handle_ptr->ID,
                      combo_index);
    }
  }
  else
  {
    ERR_FATAL("head_item_ptr did not succeed!",0,0,0);
  }

  /* -------------------------------------------------------------------------
    Return the total number of bytes copied.
  -------------------------------------------------------------------------*/
  return (total_bytes);

} /* HDRRLPI_TX_NEW_DATA */

/*===========================================================================

FUNCTION HDRRLPI_TX_LAST_DATA_UNIT

DESCRIPTION
  This function is called as part of building RLP data packets for transmit.
  If the RLP flush timer has expired, indicating that no data has been sent
  for a specified amount of time, RLP will send a packet containing the
  last data unit of the last packet that was sent.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the packet that is built, including headers.  A zero-length value
  indicates that the packet could not be found.

SIDE EFFECTS
  The **rev_pkt_ptr dsm item may point to a newly allocated dsm item chain.
===========================================================================*/
uint16 hdrrlpi_tx_last_data_unit
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,      /* ptr to RLP specific ctl blk  */
  dsm_item_type            **rev_pkt_ptr_ptr,       /* ptr to pkt to build */
  uint16                   max_bytes       /* max size allowed pk to build */         
)
{
  hdrrlptxq_type          *txq_item_ptr;    /* struct passed to txq func   */
  uint32                  pkt_last_seq;     /* seq of last byte to rexmit  */
  uint16                  pkt_len;          /* length of data payload      */
  uint16                  copy_len;         /* length of data payload      */
  uint8                   last_octet;       /* last octet in packet        */

  uint8                   rlp_header_size;  /* Size of the RLP header      */
  boolean                 first_data_unit = FALSE;
  boolean                 last_data_unit = FALSE;
  uint16                  offset;
  uint32                  num_data_units;
  dsm_item_type           *tmp_item_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Determine the size of the RLP header based on the version of RLP.
    If it is the default Packet Application, then the RLP header is just
    the length of the sequence number plus stream header (22 bits + 2 bits). 
    If it is the multiflow RLP, then the header length is the length of the
    sequence number, the RLP ID and the stream header
  -------------------------------------------------------------------------*/
  rlp_header_size = hdrrlp_get_header_size(rrcb_ptr->cfg_ptr);

  /*-------------------------------------------------------------------------
    Obtain the last transmitted packet.  The function returns the useable
    payload length of the packet.  This is used in calculating the new
    sequence number of the "last octet" packet.
  -------------------------------------------------------------------------*/
  if ( (pkt_len = hdrrlptxq_access_last_frame(rrcb_ptr, &txq_item_ptr)) == 0)
  {
    /*-----------------------------------------------------------------------
      No packets available on the transmit queue.
    -----------------------------------------------------------------------*/
    return (0);
  }

  /* EMPA/MMPA segment sequencing: send the entire last packet (one data */
  /* unit may be multiple octets) */
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
      rrcb_ptr->cfg_ptr->segment_sequencing)
  {
    pkt_len += rlp_header_size;
    if(pkt_len != dsm_length_packet(txq_item_ptr->dsm_chain_ptr))
    {
      ASSERT(0);
      return 0;
    }

    /* Can't fragment a single data unit */
    if (pkt_len > max_bytes) 
    {
      DATA_RLP_MSG6( MSG_LEGACY_HIGH,
                      "RLP Tx[%d,%d] V[%d,%d] Not enough room in stream to send flush data unit (%d > %d)",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced,
                      pkt_len,
                      max_bytes );
      /*-----------------------------------------------------------------------
        Try again later.  Return 0 bytes.
        -----------------------------------------------------------------------*/
      return (0);
    }

    if ( (dsm_dup_packet(rev_pkt_ptr_ptr, txq_item_ptr->dsm_chain_ptr, 0, pkt_len) !=
        pkt_len) || ( *rev_pkt_ptr_ptr == NULL ) )
    {
      DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                      "RLP Tx[%d,%d] V[%d,%d] Cannot Dup",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);
      /*-----------------------------------------------------------------------
        Try again later.  Return 0 bytes.
        -----------------------------------------------------------------------*/
      return (0);

    }

    HDRCOM_DSM_SET_TRACER((*rev_pkt_ptr_ptr), HDRCOM_DSM_RLP_TX);

    return pkt_len;
  }

  /*-------------------------------------------------------------------------
    Calculate the sequence number of the new packet to send. 
  -------------------------------------------------------------------------*/
  num_data_units = HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, pkt_len) - 1;
  pkt_last_seq = HDRRLPSEQ_ADD_SEQ(txq_item_ptr->seq_num,
                                    num_data_units,
                                    (rrcb_ptr->cfg_ptr->seq_modulus));
  
  /* EMPA/MMPA packet stream: indicate the first/last segments of the Route Protocol packet */
  if ((rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
      rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA) &&
      rrcb_ptr->cfg_ptr->packet_stream)
  {
    if (pkt_last_seq == txq_item_ptr->seq_num)
    {
      offset = HDRMRLP_STREAM_HEADER_SIZE+rrcb_ptr->cfg_ptr->rlp_id_len_in_pkt+HDRRLP_EMPA_ROUTE_LEN+
        rrcb_ptr->cfg_ptr->seq_length;
      (void)dsmbit_unpack8(txq_item_ptr->dsm_chain_ptr, &first_data_unit, offset, HDRRLP_EMPA_FDU_LEN);
    }

    offset = HDRMRLP_STREAM_HEADER_SIZE+rrcb_ptr->cfg_ptr->rlp_id_len_in_pkt+HDRRLP_EMPA_ROUTE_LEN+
      rrcb_ptr->cfg_ptr->seq_length+HDRRLP_EMPA_FDU_LEN;
    (void)dsmbit_unpack8(txq_item_ptr->dsm_chain_ptr, &last_data_unit, offset, HDRRLP_EMPA_LDU_LEN);
    ASSERT(last_data_unit); /* flush packets always contain the last data unit */
  }
  
  /*-------------------------------------------------------------------------
    Obtain the last octet from this last packet.
  -------------------------------------------------------------------------*/
  pkt_len += (rlp_header_size - 1);
  copy_len = 1;
  tmp_item_ptr = txq_item_ptr->dsm_chain_ptr;
  if (dsm_seek_extract(&tmp_item_ptr, &pkt_len, 
                                    (void *)&last_octet, &copy_len) == FALSE)
  {
    /*-----------------------------------------------------------------------
      Error in getting last octet.
    -----------------------------------------------------------------------*/
    return (0);
  }

  /*-------------------------------------------------------------------------
    Obtain a new item to store the "last octet" packet.
  -------------------------------------------------------------------------*/
  *rev_pkt_ptr_ptr = dsm_new_buffer
                 (
                   DSM_DS_POOL_SIZE(
                                    (word)(copy_len >> 
                                           DSRLPI_ITEM_SIZ_DIVISOR)
                                   )
                 );
  if (*rev_pkt_ptr_ptr == NULL)
  {
    DATA_RLP_MSG5( MSG_LEGACY_FATAL,
                    "RLP Tx[%d,%d] V[%d,%d] Memory allocation failed (%d bytes)",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced,
                    copy_len );
    /*-----------------------------------------------------------------------
      No bytes copied.  Return 0 bytes.
    -----------------------------------------------------------------------*/
    return (0);
  }

  /*-------------------------------------------------------------------------
    Start building up the packet.  Add the 22-bit sequence number for the 
    packet into the first 3 bytes of the item.  
  -------------------------------------------------------------------------*/
  HDRRLPI_PACK_RLP_HEADER(rrcb_ptr->cfg_ptr,
                          rrcb_ptr->route,
                          pkt_last_seq,
                          first_data_unit,
                          last_data_unit,
                          (*rev_pkt_ptr_ptr)->data_ptr);

  (*rev_pkt_ptr_ptr)->used += rlp_header_size;
    
  /*-------------------------------------------------------------------------
    And now just add the last octet.
  -------------------------------------------------------------------------*/
  (void)dsm_pushdown_tail(rev_pkt_ptr_ptr, (void *)&last_octet, 1,
                    DSM_DS_SMALL_ITEM_POOL);

   if ( *rev_pkt_ptr_ptr == NULL )
   {
     DATA_RLP_MSG0( MSG_LEGACY_FATAL, "DSM pushdown operation failed" );
     return (0);
   } 
   
  /*-------------------------------------------------------------------------
    Return the total number of bytes copied.
  -------------------------------------------------------------------------*/
  copy_len = (uint16)dsm_length_packet(*rev_pkt_ptr_ptr);

  if (first_data_unit == TRUE)
	{
		rrcb_ptr->stats.tx_first_data_unit++;
	}
  if (last_data_unit == TRUE)
	{
		rrcb_ptr->stats.tx_last_data_unit++;
	}
  DATA_RLP_MSG6( MSG_LEGACY_MED,
                  "RLP Tx[%d,%d] V[%d,%d] Seq#[0x%x, ) len:%d Tx Flush Data",
                  rrcb_ptr->cfg_ptr->sr_id, 
                  "AB"[rrcb_ptr->route],
                  rrcb_ptr->cfg_ptr->ver, 
                  rrcb_ptr->cfg_ptr->enhanced,
                  pkt_last_seq,
                  copy_len );

  DSM_TOUCH_ITEM(*rev_pkt_ptr_ptr);
  return (copy_len);

} /* HDRRLPI_TX_LAST_DATA_UNIT */


/*===========================================================================

FUNCTION HDRRLPI_RX_PROCESS_SIG_MSG

DESCRIPTION
  Processes the messages on the RLP receive signaling queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlpi_rx_process_sig_msg
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,         /* RLP-specific ctl block    */
  dsrlpi_state_info_type   *state_ptr,        /* local state ptr           */
  dsm_item_type            *msg_ptr           /* ptr to the message        */
)
{
  uint8          msg_id;         /* type of rx signaling message           */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    The message ID is the first byte of the header.  Obtain the message ID
    and process accordingly.  
    -----------------------------------------------------------------------*/
  if(HDRRLPI_UNPACK_MSG_ID( msg_ptr, &msg_id ) != E_SUCCESS)
  {
    DATA_RLP_MSG4( MSG_LEGACY_FATAL,
                    "RLP Rx[%d,%d] V[%d,%d] Error Extracting message ID from RLP MSG",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    return;
  }

  DSM_TOUCH_ITEM(msg_ptr);
  switch (msg_id)
  {
    case HDRRLPI_SIG_RST:
      /*-------------------------------------------------------------------
        RLP Reset message.  Respond with sending a RESET ACK & change RLP
        to be in Reset state.
        -------------------------------------------------------------------*/
      DATA_RLP_MSG4( MSG_LEGACY_MED,
                      "RLP Rx[%d,%d] V[%d,%d] RLP Reset Received ",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);
      state_ptr->rst_ack_to_send = TRUE;
      /*-------------------------------------------------------------------
        RLP reset was initiated by the network.  If not already in the
        reset state, transition to reset state and reset RLP.  As part of 
        initialization, the signaling queue is also flushed, so that 
        pending signaling messages will not go out.  Otherwise, do nothing.
        Also, start the fail-safe timer to prevent potential RLP deadlock
        situation.
        ------------------------------------------------------------------*/
      if (state_ptr->state == DSRLPI_ESTABLISHED_STATE)
      {
        DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                        "RLP Rx[%d,%d] V[%d,%d] Entering RESET State ",
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced);
        hdrrlpi_reset_proc(rrcb_ptr);
      }

      rrcb_ptr->stats.an_reset_request_count++;
      break;

    case HDRRLPI_SIG_RSTACK:
      /*-------------------------------------------------------------------
        RLP Reset ACK message.  If in the RESET state, an acknowledgement
        may be generated.
        -------------------------------------------------------------------*/
      if (state_ptr->state == DSRLPI_RESET_STATE)
      {
        /*-----------------------------------------------------------------
          In the RESET state. Determine whether or not to send a RESET ACK.
          -----------------------------------------------------------------*/
        if (state_ptr->mob_initiated_reset == TRUE)
        {
          /*---------------------------------------------------------------
            Mobile originated RESET.  Need to send RESET ACK.  RLP will
            leave the RESET state after the RESET ACK message is queued as
            part of transmit processing.
            ---------------------------------------------------------------*/
          DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                          "RLP Rx[%d,%d] V[%d,%d] Respond with RESET ACK ",
                          rrcb_ptr->cfg_ptr->sr_id, 
                          "AB"[rrcb_ptr->route],
                          rrcb_ptr->cfg_ptr->ver, 
                          rrcb_ptr->cfg_ptr->enhanced);
          state_ptr->rst_ack_to_send = TRUE;
          rrcb_ptr->stats.an_reset_ack_count++;
        }
        else
        {
          /*---------------------------------------------------------------
            Not a mobile originated RESET.  This is an acknowlegement to 
            an acknowledgement that was already sent, so leave the RESET
            state and go to ESTABLISHED.
            ---------------------------------------------------------------*/
          DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                          "RLP Rx[%d,%d] V[%d,%d] RESET ACK received, Established ",
                          rrcb_ptr->cfg_ptr->sr_id, 
                          "AB"[rrcb_ptr->route],
                          rrcb_ptr->cfg_ptr->ver, 
                          rrcb_ptr->cfg_ptr->enhanced);
          (void)rex_clr_timer(&rrcb_ptr->reset_timer);
          state_ptr->state = DSRLPI_ESTABLISHED_STATE;

          /*---------------------------------------------------------------
            Cancel any pending RESET ACKs to send.
            ---------------------------------------------------------------*/
          if (state_ptr->rst_ack_to_send == TRUE)
          {
            state_ptr->rst_ack_to_send = FALSE;
          }
        }

      } /* if RESET */
      else
      {
        /*-----------------------------------------------------------------
          Not in the reset state, so ignore.
          -----------------------------------------------------------------*/
        DATA_RLP_MSG4( MSG_LEGACY_MED,
                        "RLP Rx[%d,%d] V[%d,%d] RESET ACK received while not in RESET State ",
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced);
      }
      break;

    case HDRRLPI_SIG_NAK:
      /*-------------------------------------------------------------------
        RLP NAK message.  Process each NAK record individually.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG4( MSG_LEGACY_MED,
                        "RLP Rx[%d,%d] V[%d,%d] NAK message received ",
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced);
      hdrrlpi_rx_process_nak(rrcb_ptr, &msg_ptr);
      break;

    default:
      /*-------------------------------------------------------------------
        Unknown message type.  Log an error.
        -------------------------------------------------------------------*/
      DATA_RLP_MSG5( MSG_LEGACY_FATAL,
                      "RLP Rx[%d,%d] V[%d,%d] Unknown RLP Signalling message type:%x received ",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced,
                      msg_id);

  } /* switch */

    /*-----------------------------------------------------------------------
      Free the DSM packet chain.
      -----------------------------------------------------------------------*/
  dsm_free_packet(&msg_ptr);

  return;

} /* HDRRLPI_RX_PROCESS_SIG_MSG */

/*===========================================================================

FUNCTION HDRRLPI_RX_PROCESS_NAK

DESCRIPTION
  This function is called as part of processing received RLP signalling 
  messages and processes NAK messages.
  
  The NAK signalling messages parses the number of NAK requests, and then
  for each NAK request, parses the FirstErased and WindowLen fields.

  Each NAK request will generate entries into the retransmit list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Items will be queued on to the retransmit list.
===========================================================================*/
void hdrrlpi_rx_process_nak
(  
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,         /* RLP-specific ctl block    */
  dsm_item_type            **nak_msg_ptr    /* ptr to received NAK message  */
)
{
  dsrlpi_state_info_type   *state_ptr;      /* local state ptr             */
  uint32                    first_erased;   /* seq of first octet erased   */
  uint32                    nak_first_seq;  /* internal seq of first erased*/
  uint32                    nak_last_seq;   /* internal seq of last erased */
  uint32                    window_len;     /* number of octets erased     */
  uint16                    offset;       /* offset into the signaling msg */
  hdrcom_errno_enum_type        status;       /* status of bit unpack routine  */
  uint8                     nak_requests; /* number of NAK requests in msg */
  uint8                     i;            /* local loop counter            */
  byte                      reserved_size; /* Size of reserved field       */

  event_rlp_nak_rev_link_payload_type rlp_rev_link_nak_event_payload;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  state_ptr = &rrcb_ptr->state;

  /*-------------------------------------------------------------------------
    Ignore the NAK if RLP is already resetting.
  -------------------------------------------------------------------------*/
  if(state_ptr->state == DSRLPI_RESET_STATE)
  {
    DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                    "RLP Rx[%d,%d] V[%d,%d] NAK message received when RLP in reset state - ignoring ",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    return;
  }

  /*-------------------------------------------------------------------------
    If Naks are disabled for this RLP, then ignore the received Nak message.
    -------------------------------------------------------------------------*/
  if(rrcb_ptr->cfg_ptr->nak_enabled == FALSE )
  {
    DATA_RLP_MSG4( MSG_LEGACY_ERROR,
                    "RLP Rx[%d,%d] V[%d,%d] NAKs disabled, Ignoring",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    return;
  }

  if ((nak_msg_ptr == NULL) || (*nak_msg_ptr == NULL))
  {
	DATA_RLP_MSG4( MSG_LEGACY_HIGH,
					"RLP Rx[%d,%d] V[%d,%d] Empty NAK message",
					rrcb_ptr->cfg_ptr->sr_id, 
					"AB"[rrcb_ptr->route],
					rrcb_ptr->cfg_ptr->ver, 
					rrcb_ptr->cfg_ptr->enhanced);
	return;
  }

  DSM_TOUCH_ITEM(*nak_msg_ptr);

  offset = HDRRLPI_MSG_ID_SIZE + HDRRLPI_NAK_RLP_FLOW_SIZE(rrcb_ptr->cfg_ptr);

#ifdef FEATURE_HDR_QOS
  /*-----------------------------------------------------------------------
   MFPA has a SequenceLength field for use by offline analysis tools
  ------------------------------------------------------------------------*/
  if (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
  {
    if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_EMPA || 
        rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
    {
      offset += HDRRLP_EMPA_ROUTE_LEN + 
        HDRMRLPI_EMPA_NAK_RESERVED_SIZE; /* 1 route, 2 reserved */
    }
    offset += HDRMRLPI_SEQUENCE_LENGTH_SIZE;
  }
#endif /* FEATURE_HDR_QOS */

  /*-------------------------------------------------------------------------
    Obtain the number of NAK requests in the message.
  -------------------------------------------------------------------------*/
  status = HDRBIT_UNPACK8(*nak_msg_ptr, &nak_requests, offset,
                          HDRRLPI_NAK_REQUESTS_SIZE(rrcb_ptr->cfg_ptr));
  offset += HDRRLPI_NAK_REQUESTS_SIZE(rrcb_ptr->cfg_ptr);

  if (status != E_SUCCESS)
  {
    DATA_RLP_MSG4( MSG_LEGACY_ERROR,
                    "RLP Rx[%d,%d] V[%d,%d] Error unpacking RLP NAK Record ",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    return;
  }

  if (nak_requests == 0)
  {
    DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                    "RLP Rx[%d,%d] V[%d,%d] Empty NAK message",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    return;
  }

  /*-----------------------------------------------------------------------
    Determine size of the Reserved field for this RLP
    -----------------------------------------------------------------------*/
  reserved_size = HDRRLPI_NAK_RESERVED_SIZE(rrcb_ptr->cfg_ptr);

  /*=========================================================================
    Loop on the number of NAK requests to parse out the individual NAK
    request fields.  The NAK request is of the following format:

    DPA:
    -------------------------------------------------------------
         |  Reserved |      FirstErased          |     WindowLen        |
    -------------------------------------------------------------
               2                   22                     16

    MFPA:
    -------------------------------------------------------------
    |      FirstErased          |     WindowLen       |  Reserved |
    -------------------------------------------------------------
    seq_len                     16                    0-7

    EMPA:
    --------------------------------------------------------
    |      FirstErased          |     WindowLen             |
    --------------------------------------------------------

    seq_len                     seq_len                

  =========================================================================*/
  for (i=0; i<nak_requests; i++)
  {
    window_len = 0;
    nak_first_seq = nak_last_seq = first_erased = 0;

    /* DPA has reserved field at start of Nak record */
    if (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR) 
    {
      offset += reserved_size;
    }

    /*-----------------------------------------------------------------------
      Obtain the FirstErased field and store it in internal sequence number
      representation.
    -----------------------------------------------------------------------*/
    status = HDRBIT_UNPACK32(*nak_msg_ptr, &first_erased, offset,
                             rrcb_ptr->cfg_ptr->seq_length);
    offset += rrcb_ptr->cfg_ptr->seq_length;
    if (status != E_SUCCESS)
    {
      DATA_RLP_MSG4( MSG_LEGACY_ERROR,
                      "RLP Rx[%d,%d] V[%d,%d] Error unpacking RLP NAK Record ",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);
      return;
    }

    HDRRLPSEQ_STORE_SEQ(nak_first_seq, first_erased,
                         (rrcb_ptr->cfg_ptr->seq_modulus));

    /*-----------------------------------------------------------------------
      Obtain the WindowLen field.
    -----------------------------------------------------------------------*/
    status = HDRBIT_UNPACK32(*nak_msg_ptr, &window_len, offset,
                             HDRRLPI_NAK_WINDOW_LEN_SIZE(rrcb_ptr->cfg_ptr));
    offset += HDRRLPI_NAK_WINDOW_LEN_SIZE(rrcb_ptr->cfg_ptr);
    if (status != E_SUCCESS)
    {
      DATA_RLP_MSG4( MSG_LEGACY_ERROR,
                      "RLP Rx[%d,%d] V[%d,%d] Error unpacking RLP NAK Record ",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);
      return;
    }
   
    DATA_RLP_MSG6( MSG_LEGACY_HIGH,
                    "RLP Rx[%d,%d] V[%d,%d] Seq#[0x%x, ) Len:%x NAK request",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced,
                    first_erased,
                    window_len);
    rlp_rev_link_nak_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
    rlp_rev_link_nak_event_payload.start_seq_num = first_erased;
    rlp_rev_link_nak_event_payload.window_len = window_len;
    event_report_payload (EVENT_RLP_REV_LINK_NAK, 
                              sizeof(event_rlp_nak_rev_link_payload_type),
                              &rlp_rev_link_nak_event_payload);

    rrcb_ptr->stats.naks_received++;
    rrcb_ptr->stats.an_nak_bytes_requested += window_len;

    /*-----------------------------------------------------------------------
      Use combination of FirstErased and WindowLen to create the first/last
      seq pair using modulo arithmetic.  This value is used to insert an 
      entry into retransmit list.  
    -----------------------------------------------------------------------*/
    nak_last_seq = HDRRLPSEQ_ADD_SEQ(nak_first_seq,
                                      window_len,
                                      rrcb_ptr->cfg_ptr->seq_modulus);
    /*-----------------------------------------------------------------------
      Determine if the range of the NAK request is greater than or equal to 
      V(S).  If so, need to reset, as the Access Network in NAKing data that
      we have not sent.      
    -----------------------------------------------------------------------*/
    if (HDRRLPSEQ_GE_SEQ_CHECK(nak_last_seq, state_ptr->next_seq_num,
                              rrcb_ptr->cfg_ptr->seq_modulus))
    {
      /*---------------------------------------------------------------------
        Invalid NAK.  Invoke RLP reset procedures.
      ---------------------------------------------------------------------*/
      DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                      "RLP Rx[%d,%d] V[%d,%d] Invalid NAK Received",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);
      hdrrlpi_mobile_initiated_reset(rrcb_ptr);
      return;
    }

    /*-----------------------------------------------------------------------
      Create entry for re-xmit list.
    -----------------------------------------------------------------------*/
    if( hdrrlprtx_put_rexmit_info( rrcb_ptr,
                                  nak_first_seq,
                                  nak_last_seq, 
                                  HDRRLPRTX_NAK_RTX,
                                  NULL ) == FALSE )
    {
      /*---------------------------------------------------------------------
        Unable to insert into the retransmit list.  Reset RLP.
      ---------------------------------------------------------------------*/
      DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                      "RLP Rx[%d,%d] V[%d,%d] Seq#[0x%x, ) Unable to insert into Re-xmit list",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced);
      hdrrlpi_mobile_initiated_reset(rrcb_ptr);
      return;
    }

#ifdef FEATURE_HDR_QOS
    /* MFPA has reserved field at end of Nak record */
    if (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
    {
      offset += reserved_size;
    }
#endif /* FEATURE_HDR_QOS */
  }
  return;
} /* HDRRLPI_RX_PROCESS_NAK */

#ifdef FEATURE_HDR_EMPA
#if 0 
/*===========================================================================

FUNCTION HDRRLP_ROHC_IOCTL_RESET_ELR_CB

DESCRIPTION
  Isuues a ROHC_IOCTL to reset the Seq No. registered with the given ROHC 
  instance. This function needs to be remoted for 7600.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None  .

===========================================================================*/
void hdrrlp_rohc_ioctl_reset_elr_cb(byte sr_id, dsrlp_ver_enum_type ver, byte route)
{
  ds707_tc_state_type *tc_state_ptr;
  rohc_handle_type rohc_handle; /* handle to the rohc instance 
                                 * whose RLP Seq No. is to be reset 
                                 */
  errno_enum_type err_num;        /* Contains error code returned by rohc_ioctl */
  int             i;            /* loop index */

  /* Scroll through and find the correct tc_state
   */
  for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
  {
    tc_state_ptr = ds707_get_sec_tc_ptr(i);    
    if (ds707_sec_pkt_is_tc_state_allocated(tc_state_ptr) == FALSE) 
    {      
      continue;
    }

    /* Issue the ROHC_IOCTL on flow and route protocol rohc instances 
     * after identifying the correct tc_state
     */
    if ((tc_state_ptr->fwd_rlp.flow.flow == sr_id) &&
        (tc_state_ptr->fwd_rlp.flow.ver == ver))
    {
      if ((rohc_handle = tc_state_ptr->fwd_rlp.flow_protocol_rohc_handle) != NULL)
      {
        (void)rohc_ioctl(tc_state_ptr->fwd_rlp.flow_protocol_rohc_handle,
                         ROHC_IOCTL_RESET_ELR,
                         NULL,
                         &err_num);      
      }
      if ((rohc_handle = 
           tc_state_ptr->fwd_rlp.route_protocol_rohc_handle[route]) != NULL)
      {
        (void)rohc_ioctl(rohc_handle,
                         ROHC_IOCTL_RESET_ELR,
                         NULL,
                         &err_num);      
      }
      break;
    }
  }
  if (i == DS707_NUM_PHYS_LINKS)
  {                            
    DATA_RLP_MSG0( MSG_LEGACY_HIGH,
                    "RLP Rx ROHC_IOCTL_RESET_ELR: cannot find tc_state ");
  }
  return;         
}
#endif 
#endif /* FEATURE_HDR_EMPA */

/*===========================================================================

FUNCTION HDRRLPI_RESET_PROC

DESCRIPTION
  Invokes HDR RLP Reset Procedures.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Invokes mobile-initated reset procedures.

===========================================================================*/
void hdrrlpi_reset_proc
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr          /* RLP-specific ctl block    */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
  {
    /* Update the rx processing stats to be logged after reset processing is done */
    rrcb_ptr->state.rx_proc_state.event_type = DSRLPI_RX_PROC_EVENT_RESET;
    rrcb_ptr->state.rx_proc_state.vn_before = rrcb_ptr->state.needed_seq_num;
    rrcb_ptr->state.rx_proc_state.vr_before = rrcb_ptr->state.expected_seq_num;
  }

  /*-------------------------------------------------------------------------
    Perform Reset and then set state variables for subsequent transmit-side
    processing.
  -------------------------------------------------------------------------*/
  dsrlpi_reset(rrcb_ptr);

  rrcb_ptr->state.state = DSRLPI_RESET_STATE;
  
  if (rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
  {
    #ifdef FEATURE_HDRRLP_LOGGING
    rrcb_ptr->state.rx_proc_state.vn_after = rrcb_ptr->state.needed_seq_num;
    rrcb_ptr->state.rx_proc_state.vr_after = rrcb_ptr->state.expected_seq_num;
    
    /* Log the Fwd RX Processing log type */
    hdrrlplog_multilink_fwd_log_rx_processing( rrcb_ptr );
    #endif /* FEATURE_HDRRLP_LOGGING */
  }

  DATA_RLP_MSG4( MSG_LEGACY_HIGH,
                  "RLP [%d,%d] V[%d,%d] Setting RLP reset failsafe timer",
                  rrcb_ptr->cfg_ptr->sr_id, 
                  "AB"[rrcb_ptr->route],
                  rrcb_ptr->cfg_ptr->ver, 
                  rrcb_ptr->cfg_ptr->enhanced);
  (void)rex_clr_timer(&rrcb_ptr->reset_timer);
  (void)rex_set_timer(&rrcb_ptr->reset_timer, 
                HDRRLPI_RESET_TIMER_VAL_MSEC);  

#if 0
#ifdef FEATURE_HDR_EMPA
  /* If EMPA is negotiated issue rohc_iotcl to reset the RLP Seq. No
   * registered with the ROHC instance
   */
  if (hdrcp_stream_stream_is_assigned (HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      hdrcp_stream_stream_is_assigned (HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
      hdrrlp_rohc_ioctl_reset_elr_cb(rrcb_ptr->cfg_ptr->sr_id, 
                                     rrcb_ptr->cfg_ptr->ver,
                                     rrcb_ptr->route);
  }
#endif /* FEATURE_HDR_EMPA */
#endif

} /* HDRRLPI_RESET_PROC */


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
               DO_REV_A_RLP PUBLIC FUNCTION DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/


/*===========================================================================

FUNCTION       HDRRLPI_NEW_DATA_PACKET_STATUS

DESCRIPTION    Update packet status of the new data packet transmission.
               TRUE if the packet was accepted.
               FALSE if the packet was cancelled.
               
DEPENDENCIES   We assume if we get (status = TRUE) it is true for all the 
               RLP packets given in that subframe, and similar for FALSE.
               All the handles needs to be given in the same order as is 
               given to PCP

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
hdrrlptxq_type* hdrrlpi_new_data_packet_status
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,       /* current RLP ctl blk         */
  hdrrlpi_new_data_pkt_handle_type *handle, /* handle to determine packet  */
  boolean                  pkt_status,           /* packet status          */
  uint8                    combo_index,
  boolean                  retry_again      /* To denote that it was a 
                                               FALSE RETRANSMISSION */
)
{
  hdrrlptxq_type          *txq_pkt_ptr = NULL;
  dsrlpi_state_info_type  *state_ptr = NULL;       /* local state ptr             */
  dsm_item_type           *head_item_ptr=NULL; /* head of pkt chain        */
  dsm_item_type           *item_ptr = NULL;
  #if defined(FEATURE_HDRRLP_LOGGING)
  hdrrlp_tx_packet_info_type tx_packet_info; /* used to pass info into the logging function*/
  #endif /* FEATURE_HDRRLP_LOGGING */
  hdrrlp_debug_ctl        *hdrrlp_debug_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initializing variables that will be used later in the function.
  -------------------------------------------------------------------------*/
  state_ptr        = &rrcb_ptr->state;
  head_item_ptr    = handle->head_item_ptr;
  hdrrlp_debug_ptr = get_hdrrlp_debug_ptr();

  /*------------------------------------------------------------
   If RLP is not in established state, then we don't need to
   process the packet status, as the rscb pointer gets reset 
   and the data is no longer valid.
   ------------------------------------------------------------*/
  if(rrcb_ptr->state.state != DSRLPI_ESTABLISHED_STATE)
  {
    DATA_RLP_MSG4( MSG_LEGACY_FATAL,
                    "RLP Tx[%d,%d] V[%d,%d] Pkt Status success, RLP NOT Established, Ignoring",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    return NULL;
  }

  /*---------------------------------------------------------------------
   Update the counter only when RLP is connected. Keep a counter of 
   number of handles received from PCP so far.
  ---------------------------------------------------------------------*/
  state_ptr->local_state[combo_index].new_handle_status_rcvd_count += 1;
  if(state_ptr->local_state[combo_index].new_handle_status_rcvd_count != handle->ID)
  {
    DATA_RLP_MSG4( MSG_LEGACY_FATAL,
                    "RLP Tx[%d,%d] V[%d,%d] Handles are not returned in the same order as given",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    state_ptr->local_state[combo_index].new_handle_status_rcvd_count -= 1;
    return NULL;
  }

  /*-----------------------------------------------------------------------
     Packet was accepted.
    -----------------------------------------------------------------------*/
  if (pkt_status == TRUE)
  {
     
     ASSERT(head_item_ptr != NULL);
    /*-----------------------------------------------------------------------
      Populate the TX_Q Item and add it to the transmitted Q
    -----------------------------------------------------------------------*/    
    {
      txq_pkt_ptr = hdrrlptxq_new_txq_item(rrcb_ptr);
      txq_pkt_ptr->seq_num = state_ptr->next_seq_num;
      txq_pkt_ptr->dsm_chain_ptr = head_item_ptr;
      HDRCOM_DSM_SET_TRACER(head_item_ptr, HDRCOM_DSM_RLP_TXQ);
      hdrrlptxq_add_frame (rrcb_ptr, txq_pkt_ptr);
    }

    if(hdrrlp_debug_ptr->enable_tx_debug)
    {
      DATA_RLP_MSG7( MSG_LEGACY_MED,
                      "RLP Tx[%d,%d] V[%d,%d] Seq#[0x%x,0x%x) Len:0x%x Pkt Status success",
                      rrcb_ptr->cfg_ptr->sr_id, 
                      "AB"[rrcb_ptr->route],
                      rrcb_ptr->cfg_ptr->ver, 
                      rrcb_ptr->cfg_ptr->enhanced,
                      state_ptr->next_seq_num,
                      HDRRLPSEQ_ADD_SEQ( (state_ptr->next_seq_num), 
                                          HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr,
                                                                 handle->payload_size),
                                                                 (rrcb_ptr->cfg_ptr->seq_modulus)),
                      handle->payload_size);    
    }
    
    state_ptr->next_seq_num = HDRRLPSEQ_ADD_SEQ( (state_ptr->next_seq_num), 
                                  HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr,
                                                         handle->payload_size),
                                  (rrcb_ptr->cfg_ptr->seq_modulus));
                                  
    /*-----------------------------------------------------------------------
     Log the packes only if the packet got accepted by the lower layers.
     If retry Again is FALSE, then it means that this packet was accepted by 
     lower layers and also this was not cancelled because of Out Of Order 
     transmissions. 
     If it was true, it was cancelled by Lower layers, and needs to be 
     FALSE_RETX'ed because of out-or-order transmissions. And hence we 
     should not log it as it did not make it out of the AT.
    -----------------------------------------------------------------------*/
    if(((rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE) ||
          (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR) )  &&
        ( retry_again == FALSE ) )  
    {
        #if defined(FEATURE_HDRRLP_LOGGING)
        tx_packet_info.rrcb_ptr = rrcb_ptr;
        tx_packet_info.rev_pkt_ptr = handle->head_item_ptr;
        #ifdef FEATURE_HDR_REVB
        if(rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
        {
          hdrrlptxsh_parse_setid(handle->set_id, 
                               NULL, 
                               &tx_packet_info.app_specific_info.mmpa_tx_partial_log_packet_info.carrier_id, 
                               NULL, 
                               NULL);
          tx_packet_info.app_specific_info.mmpa_tx_partial_log_packet_info.tx_reason = (int)HDRRLP_NEW_DATA_HANDLE; /* There are different
          transmission reasons. All possible cases are covered by the Handle enums. Hence they are being reused*/
        }
        #endif /* FEATURE_HDR_REVB */
        hdrrlp_tx_rlp_frames_logging(&tx_packet_info);
#endif /* defined(FEATURE_HDRRLP_LOGGING) */
      /*
        Update the stats, only if we will not be retrying. 
      */
      rrcb_ptr->stats.tx_new_data_fr_cnt += handle->payload_size; 
      rrcb_ptr->stats.tx_total_bytes += handle->payload_size;
      rrcb_ptr->stats.tx_new_data_frames ++;
      rrcb_ptr->stats.tx_new_data_units += HDRRLPI_NUM_DATA_UNITS(rrcb_ptr->cfg_ptr, handle->payload_size);
      rrcb_ptr->stats.tx_total_frames ++;
    }
  }
    else
    {
      /*-----------------------------------------------------------------------
        Packet was cancelled.
      -----------------------------------------------------------------------*/      
      if (hdrrlp_debug_ptr->enable_tx_debug) 
      {
        DATA_RLP_MSG6( MSG_LEGACY_MED,
                        "RLP Tx[%d,%d] V[%d,%d] handle_id:%d combo_index:%d Pkt Status cancel",
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced,
                        handle->ID,
                        combo_index );
      }
    }
 
   
    
      /*
        Since we are all done with our bookeeping, we can free the tx_item_q
        item information.
      */
    if ( pkt_status == TRUE )
    {
        /*
          Free the TXQ for the part we have received status for.
        */
        state_ptr->seek_offset += handle->payload_size;
        while ((item_ptr = (dsm_item_type *)q_check(&rrcb_ptr->state.tx_item_q)) != NULL)
        {
          if (state_ptr->seek_offset >= dsm_length_packet(item_ptr))
          {
            state_ptr->seek_offset -= (uint16)dsm_length_packet(item_ptr);
            (void)q_get(&rrcb_ptr->state.tx_item_q);
            dsm_free_packet(&item_ptr);
          } 
          else
          {
            break;
          }
        }
    }
    
    /* 
      This is the last handle received.
      Time to reset the variables that keep track of packet building.
      So far we have received all the handles that we have given to PCP.
    */    
    if (state_ptr->local_state[combo_index].new_handle_status_rcvd_count == state_ptr->local_state[combo_index].new_handles_built_count)
    {
      if (hdrrlp_debug_ptr->enable_tx_debug) 
      {
        DATA_RLP_MSG5( MSG_LEGACY_MED,
                        "RLP Tx[%d,%d] V[%d,%d] Resettting Local Variables for ComboIndex:%d ",
                        rrcb_ptr->cfg_ptr->sr_id, 
                        "AB"[rrcb_ptr->route],
                        rrcb_ptr->cfg_ptr->ver, 
                        rrcb_ptr->cfg_ptr->enhanced,
                        combo_index);
      }
      hdrrlp_reset_new_data_local_state( state_ptr, combo_index );
    }/* last handle received */

    /*
      If the Packet was not accepted we have to free the packet.
      If NAKs are disabled we have to free the packet, as the packet
      is not stored in the transmitted queue.
    */
    if ( pkt_status == FALSE ) 
    {
      dsm_free_packet(&head_item_ptr);
    }
    return txq_pkt_ptr;
} /* hdrrlpi_new_data_packet_status() */



/*===========================================================================

FUNCTION       HDRRLPI_FLUSH_DATA_PACKET_STATUS

DESCRIPTION    Update packet status of the flush data packet transmission.
               TRUE if the packet was accepted.
               FALSE if the packet was cancelled.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpi_flush_data_packet_status
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,       /* current RLP ctl blk         */
  hdrrlpi_new_data_pkt_handle_type *handle,
  boolean                  status,           /* packet status               */
  uint8                    combo_index
)
{
  #if defined(FEATURE_HDRRLP_LOGGING)
  hdrrlp_tx_packet_info_type tx_packet_info; /* used to pass info into the logging function*/
  #endif /* FEATURE_HDRRLP_LOGGING */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (status == TRUE)
  {
   /*-----------------------------------------------------------------------
    Log the packes only if the PCP layer returns status = TRUE
    -----------------------------------------------------------------------*/
#if defined(FEATURE_HDRRLP_LOGGING)
    if((rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE) ||
       (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR))
    {
      if((handle != NULL) && (handle->head_item_ptr != NULL))
      {
        tx_packet_info.rrcb_ptr = rrcb_ptr;
        tx_packet_info.rev_pkt_ptr = handle->head_item_ptr;
        #ifdef FEATURE_HDR_REVB
        if(rrcb_ptr->cfg_ptr->enhanced == DSRLP_ENHANCED_VER_MMPA)
        {
          hdrrlptxsh_parse_setid(handle->set_id, 
                               NULL, 
                               &tx_packet_info.app_specific_info.mmpa_tx_partial_log_packet_info.carrier_id, 
                               NULL, 
                               NULL);
          tx_packet_info.app_specific_info.mmpa_tx_partial_log_packet_info.tx_reason = (int)HDRRLP_FLUSH_DATA_HANDLE;/* There are different
          transmission reasons. All possible cases are covered by the Handle enums. Hence they are being reused*/
        }
        #endif /* FEATURE_HDR_REVB */
        hdrrlp_tx_rlp_frames_logging(&tx_packet_info);
      }
    }
#endif /* defined(FEATURE_HDRRLP_LOGGING) */
  
    /*-----------------------------------------------------------------------
      Set Flush Timer expired to FALSE, as we got a success on Flush Packet
      given.
    -----------------------------------------------------------------------*/
    DATA_RLP_MSG4( MSG_LEGACY_MED,
                    "RLP Tx[%d,%d] V[%d,%d] Pkt Status success, Flush Data ",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    rrcb_ptr->state.flush_timer_expired = FALSE;                
  }
  else
  {
    /*-----------------------------------------------------------------------
      Do nothing; was not set to FALSE in
      hdrrlp_get_new_or_flush_data() in case of flush packet
    -----------------------------------------------------------------------*/
    DATA_RLP_MSG4( MSG_LEGACY_MED,
                    "RLP Tx[%d,%d] V[%d,%d] Pkt Status cancel, Flush Data ",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
  }

  /*-----------------------------------------------------------------------
  The packet was duped and stored on handle when packed was being built 
  and given to PCP. This duped packet needs to be freed as soon as the 
  handle is given back from PCP. Make sure that this dsm item is freed
  irrespective of whether the status is TRUE or FALSE
  -----------------------------------------------------------------------*/
  if((handle != NULL) && (handle->head_item_ptr != NULL))
  {
    dsm_free_packet(&(handle->head_item_ptr));
  }
  
  /* Reset the flag which states that a flush packet for this combo is sent */
  rrcb_ptr->state.local_state[combo_index].txed_flush_pkt = FALSE;                                  

} /* hdrrlpi_flush_data_packet_status() */



/*===========================================================================
FUNCTION HDRRLPI_EXTRACT_RLP_HEADER

DESCRIPTION
  This function extracts the RLP header out of the received forward link pkt
  for DPA only
  
DEPENDENCIES
  None.

RETURN VALUE
  hdrcom_errno_enum_type
  E_SUCCESS - If header parsing is successful. Other errors if not.

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrrlpi_extract_rlp_header
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,       /* returns RLP-specific ctl block  */
  dsm_item_type           **fwd_pkt_ptr_ptr,    /* ptr to received RLP data packet */
  dsrlpi_header_type      *rlp_header_ptr  /* rlp_header to be filled up      */
)
{
  uint8                        offset;
  hdrcom_errno_enum_type           status;         /* Status of operation      */
  uint16                       data_len;       /* RLP packet length        */
  uint8                        hdr_length_in_bytes; /* Header length(bytes)*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initialize the rlp_header structure. Make sure qn_seq_included is set
    to 0 because it is used later to determine if the packet needs to
    be processed by QN layer. It should be set to 1 only for first time
    transmitted MMPA packets.
    ------------------------------------------------------------------------*/
  rlp_header_ptr->qn_seq_included = 0;
  rlp_header_ptr->qn_seq_num = 0;
  rlp_header_ptr->seq_num = 0;


  /*-------------------------------------------------------------------------
  The sequence number are in the first few bytes of the RLP packet, with 
  the 2 most significant bits of the most significant byte being the 
  remnant stream ID used by PCP.

  DPA:

    MSB                                                         LSB
    ------------------------------------------------------------
    |  stream id |        RLP SEQ number  |    data payload ...
    ------------------------------------------------------------
    2                   22                   x

  -------------------------------------------------------------------------*/
  offset = HDRRLP_STREAM_HEADER_SIZE;

  status = HDRBIT_UNPACK32(*fwd_pkt_ptr_ptr, 
                          &(rlp_header_ptr->seq_num), 
                          offset,
                          rrcb_ptr->cfg_ptr->seq_length);
  offset += rrcb_ptr->cfg_ptr->seq_length;
    
  if (status != E_SUCCESS)
  {
    DATA_RLP_MSG4( MSG_LEGACY_FATAL,
                    "RLP Rx[%d,%d] V[%d,%d] Error unpacking header for Fwd RLP ",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    dsm_free_packet(fwd_pkt_ptr_ptr);
    return status;
  }

  /*------------------------------------------------------------------------
    Pull up the RLP Header bytes so that the only bytes remaining in the dsm
    packet are the data packets
    ------------------------------------------------------------------------*/
  hdr_length_in_bytes = offset/8;

  data_len = dsm_pullup( fwd_pkt_ptr_ptr, NULL, hdr_length_in_bytes);
  if (data_len != hdr_length_in_bytes)
  {
    dsm_free_packet(fwd_pkt_ptr_ptr);    
    return E_FAILURE;
  }

  /* Check to see if there is any data in RLP packet after dsm_pullup */
  if (*fwd_pkt_ptr_ptr == NULL)
  {
    DATA_RLP_MSG4( MSG_LEGACY_FATAL,
                    "RLP Rx[%d,%d] V[%d,%d] No data in RLP Packet ",
                    rrcb_ptr->cfg_ptr->sr_id, 
                    "AB"[rrcb_ptr->route],
                    rrcb_ptr->cfg_ptr->ver, 
                    rrcb_ptr->cfg_ptr->enhanced);
    return E_FAILURE;
  }

  return E_SUCCESS;
} 

/*===========================================================================
FUNCTION HDRRLPI_UPDATE_COMBO_INDEX

DESCRIPTION
  Updates the combo index for which current packet building is taking place
  
PARAMETERS
  combo_id: The current combo id for which packet building is being
            requested

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates the 
===========================================================================*/
void hdrrlpi_update_combo_index
( 
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
  uint8 combo_id 
)
{

  uint8 curr_combo_index = rrcb_ptr->state.combo_index;
  dsrlpi_per_combo_type *handles_per_combo = rrcb_ptr->state.handles_per_combo;
  
  ASSERT( rrcb_ptr->state.combo_index < HDRCP_PCP_MAX_COMBOS );
  if ( curr_combo_index < HDRCP_PCP_MAX_COMBOS - 1 )
  { 
    if ( handles_per_combo[curr_combo_index].combo_id != combo_id )
    {
      if ( handles_per_combo[curr_combo_index].combo_id == HDRCP_PCP_NO_COMBO_ID_ACCEPTED )
      {
        /* 
          After every subframe all the combo_ids shall be reset,
          And if this is the first combo to be built,then  the 
          combo_id should be NO_COMBO_ID_ACCEPTED.  
          Hence the ASSERT. For all other cases, it will be a 
          valid combo_index
        */
        ASSERT( curr_combo_index == 0 );
      }
      else
      {
        /* 
          Only if there were packets built in the curr combo, do we need to update the combo
          index. Or else we can store packets in this combo itself.
        */
        if ( q_cnt(& (rrcb_ptr->state.handles_per_combo[curr_combo_index].carrier_handles_q) ) > 0 )
        {
          rrcb_ptr->state.combo_index++;
        }  
      }  
      rrcb_ptr->state.handles_per_combo[rrcb_ptr->state.combo_index].combo_id = combo_id;
    }
  }  
  else
  {
    if ( rrcb_ptr->state.handles_built == 0 )
    {
      /*
        First packet to be built, and it is possible that the combo_index is ZERO
      */
      if ( rrcb_ptr->state.combo_index == 0 )
      {
        rrcb_ptr->state.handles_per_combo[curr_combo_index].combo_id = combo_id;
      }  
    }
    else
    {
      /* 
         Already reached max combo index,  so should not be called anymore, 
         for a different combo_id, and we have atleast one packet built.
      */
      ASSERT(handles_per_combo[curr_combo_index].combo_id == combo_id);    
    }  
  }
  ASSERT( rrcb_ptr->state.combo_index < HDRCP_PCP_MAX_COMBOS );
} 


/*===========================================================================

FUNCTION HDRRLPI_RESET_RXBUFFER

DESCRIPTION
  This function resets the buffered_rx_q and returns all elements to the
  hdrrlp_pkt_info_free_q.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlpi_reset_rxbuffer
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr       /* RLP-specific ctl block         */
)
{
  dsrlpi_pkt_info_type    *pkt_info_ptr = NULL;
  q_type                  *pkt_info_free_q_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  pkt_info_ptr = (dsrlpi_pkt_info_type*)q_get(&rrcb_ptr->state.buffered_rx_q);
  pkt_info_free_q_ptr = get_hdrrlp_pkt_info_free_q_ptr();
  while ( pkt_info_ptr != NULL )
  {
    /* Put the pkt_info_ptr back on the free_q */
    q_put( pkt_info_free_q_ptr, &(pkt_info_ptr->link) );
    pkt_info_ptr = (dsrlpi_pkt_info_type*)q_get(&rrcb_ptr->state.buffered_rx_q);
  }
}
#endif /* FEATURE_HDR */

