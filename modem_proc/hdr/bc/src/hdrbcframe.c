/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R   B R O A D C A S T   F R A M I N G
       
                             P R O T O C O L       

GENERAL DESCRIPTION
  This module contains the implementation of the HDR Broadcast Framing
  Protocol.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.
  
  
  Copyright (c) 2002 - 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcframe.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------  
12/04/13   sat     Removed unused parameters in F3s.
05/24/11   ssu     Removed inclusion of "ps_hdlc_libi.h" 
12/01/09   suren   Added QChat optimization changes.
10/27/09   wsh     Fixed compiler warning for ARM compiler
01/30/09   etv     Merged the following changes to main branch
                   Check for FCS Included before checking Framing layer CRC.
09/18/08   smd     Added fix for off-target build.
07/08/08   etv     Updated for changes to hdlc_unframe API.
05/22/07   etv     Added NULL check before forwarding unframed packet.
02/28/07   etv     Fixed RVCT compiler warnings.
09/20/06   etv     Handled invalid deframer type gracefully.
06/30/06   etv     Refactored hdrbcframe_hdlc_deframe.
06/09/06   etv     Removed the line where hdlc_mode is set prior to calling 
                   hdlc_alloc_instance.
05/09/06   etv     changed inline to __inline.
04/26/06   etv     Handle 1-byte header prot ID case appropriately.
03/07/06   kss     Handle HDLC bad handle error case.
09/29/05   mpa     Moved flow enable/disable cmds from hdrbctask.
09/06/05   ssh     Support HDLC API changes.
08/28/05   dna     (mpa for) Updated bcovhd interface.
02/10/05   etv     Fixed lint errors. 
02/02/05   etv     Fixed the coding style to match the conventions.
11/22/04   kss     Added missing include of err.h
10/22/04   kss     Add a minimum length check to segment based deframing.
08/16/04   kss     Track callback for broadcast data,
                   take flow_id as parameter for disable.
07/19/04   kss     Use dsm items for segment deframing.
07/08/04   kss     Added include of hdrbcmaci.h
06/26/04   kss     Merged the HDLC framing support.
06/09/04   kss     Added FEATURE_HDR_BCMCS.
05/24/04   kss     Support 16-bit CRC (from Senthil's code)
04/24/04   kss     Ported from QC Corp.

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "customer.h"

#ifdef FEATURE_HDR_BCMCS

#include "hdrbctask.h"
#include "hdrbcframe.h"
#include "hdrbcsec.h"
#include "hdrbcmac.h"
#include "hdrbcmaci.h"
#include "hdrdebug.h"
#include "hdrtrace.h"
#include "crc.h"
#include "ps_hdlc_lib.h"

#include "dsm.h"
#include "err.h"
#ifdef FEATURE_HDR_QCHAT
#include "hdrbccp.h"
#include "hdrqchatupk.h"
#include "hdrqchatupk_opts.h"
#endif /*FEATURE_HDR_QCHAT*/

/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/


/* Macros for parsing the Segment-based Frame Header */
#define HDRBCFRAME_SEGMENT_HEADER_LEN          1

#define HDRBCFRAME_GET_FRAGMENT_LENGTH( header )              \
        (header & 0x7F)

#define HDRBCFRAME_GET_LAST_FRAGMENT( header )                \
        ((boolean)(header & 0x80))

/* CRC field length (16 bits) */
#define HDRBCFRAME_CRC_LEN             2

hdrbcframe_type hdrbcframe[HDRBCMAC_MAX_LOGICAL_CHANNELS];


 /* EJECT */
/*=============================================================================
                           FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION HDRBCFRAME_GET_CHANNEL_METRICS

DESCRIPTION
  This function copies framing protocol metrics into a provided structure
  (for logging, etc.)
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - The channel to get metrics from.
  chan_metrics - A structure to copy the data into.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_get_channel_metrics
(
  uint8 chan_idx,
  hdrlogbc_framing_metrics_type *chan_metrics
)
{
  memcpy( (void*)chan_metrics,
          (void*)&hdrbcframe[chan_idx].chan_metrics,  
          sizeof(hdrlogbc_framing_metrics_type) );

} /* hdrbcframe_get_channel_metrics */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_CLEAR_CHANNEL_METRICS

DESCRIPTION
  This function clears the channel metrics for the specified channel. The
  channel should be active.
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - The channel metrics to clear.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_clear_channel_metrics( uint8 chan_idx )
{
  memset( (void*)&hdrbcframe[chan_idx].chan_metrics, 0, 
          sizeof(hdrlogbc_framing_metrics_type));

} /* hdrbcframe_clear_channel_metrics */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_init( void )
{

  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i<HDRBCMAC_MAX_LOGICAL_CHANNELS;i++)
  {
    hdrbcframe[i].ref_count=0;
  }

#ifdef FEATURE_HDR_QCHAT
  if (hdrqchatupk_mcs_enabled())
  {
    hdrqchatupk_bcframe_init();
  }
#endif /*FEATURE_HDR_QCHAT*/

} /* hdrbcframe_init */


 /* EJECT */
/*=============================================================================
 
FUNCTION HDRBCFRAME_DEFRAMER_RESET

DESCRIPTION
  This function resets the current deframer instance of the broadcast framing
  protocol. Based on the deframer_type field of the protocol, it takes 
  appropriate steps.
   
RETURN VALUE
  None
   
SIDE EFFECTS
  None.
  
=============================================================================*/
void hdrbcframe_deframer_reset
(
  int chan_idx
)
{
   hdrbcframe_type * chan = &hdrbcframe[chan_idx];
                                                  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (chan->deframer_type)
  {
    case HDRBCOVHD_FRAMING_HDLC:

      /* Free the in use instance of the HDLC deframer */
      (void) hdlc_free_instance(&(chan->deframer.hdlc_def.handle));
      break;

    case HDRBCOVHD_FRAMING_SEGMENT:
               
      /* Free any packet we were working on and reset CRC seed. */
      chan->deframer.seg_def.crc_value = chan->deframer.seg_def.crc_seed;

      if (chan->deframer.seg_def.unframed_pkt != NULL)
      {
        dsm_free_packet(&(chan->deframer.seg_def.unframed_pkt));
      }
      break;

    default:
      HDR_MSG_PROT(MSG_LEGACY_ERROR,"Unknown deframer type");
      break;
  }

} /* hdrbcframe_deframer_reset */


 /* EJECT */
/*=============================================================================
 
FUNCTION HDRBCFRAME_DEFRAMER_INITIALIZE

DESCRIPTION
  This function initializes the deframer instance of the broadcast framing
  protocol. Based on the deframer_type field of the protocol, it takes 
  appropriate steps.
   
RETURN VALUE
  None. 
   
SIDE EFFECTS
  None.
  
=============================================================================*/
void hdrbcframe_deframer_initialize
(
  int chan_idx
)
{
  hdrbcframe_type * chan = &hdrbcframe[chan_idx];
  hdlc_config_type  hdlc_config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (chan->deframer_type)
  {

    case HDRBCOVHD_FRAMING_HDLC:

      /* Get new instance of HDLC deframer */
      hdlc_init_config(&hdlc_config);
      chan->deframer.hdlc_def.handle = hdlc_alloc_instance(&hdlc_config);
      break;

    case HDRBCOVHD_FRAMING_SEGMENT:

      /* Init CRC and pkt pointer */
      if (chan->fcs_included)
      {
        chan->deframer.seg_def.crc_value  = CRC_16_STEP_SEED;
        
        /* Min length = CRC Length (2) + PPP header (2) */ 
        chan->deframer.seg_def.min_length = 4;
      }
      else
      {
        chan->deframer.seg_def.crc_value  = CRC_16_OK;
        
        /* Min length = PPP header (2) */
        chan->deframer.seg_def.min_length = 2;
      }

      chan->deframer.seg_def.unframed_pkt = NULL;
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "Failed init. Invalid deframer type %d",
                     chan->deframer_type);
      break;
  }

} /* hdrbcframe_deframer_initialize */

 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_FLOW_ENABLE

DESCRIPTION
  This command enables a broadcast flow. The command will be queued and
  handled by the broadcast task context.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The Broadcast flow ID to enable
  data_cb - A callback function for deframed broadcast data. 
  sender  - Protocol sending the command
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_flow_enable
(
  uint32 flow_id,
  void (*data_cb)(dsm_item_type *ip_pkt),
  hdrhai_protocol_name_enum_type sender
)
{
  /* Temporary command buffer. */
  hdrbcframe_cmd_union_type cmd_buf;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "hdrbcframe_flow_enable(), flow_id %d",
                  flow_id );

  cmd_buf.name = HDRBCFRAME_FLOW_ENABLE_CMD;
  cmd_buf.sender = sender;
  cmd_buf.params.enable_flow.flow_id = flow_id;
  cmd_buf.params.enable_flow.data_cb = data_cb;

  hdrbctask_queue_cmd( HDRHAI_BROADCAST_FRAMING_PROTOCOL, 
                       &cmd_buf, sizeof(cmd_buf) );

} /* hdrbcframe_flow_enable */

 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_PROCESS_FLOW_ENABLE

DESCRIPTION
  The function enables a new channel in the HDR Broadcast Framing Protocol.
  A flow_id is taken as a parameter to open a new channel.  The return value
  is a handle (or index) for the assigned logical channel.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The desired flow.
  data_cb - Callback to function that processes the actual broadcast
            packets received.
  
RETURN VALUE
  logical channel index to use to refer to this channel.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrbcframe_process_flow_enable
(
  uint32 flow_id,
  void (*data_cb)(dsm_item_type *ip_pkt)
)
{
  int chan_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_QCHAT
  /* Check for ann flow */
  if(hdrqchatupk_mcs_enabled() && 
     hdrqchatupk_bcframe_process_announce_flow_enable(flow_id,data_cb))
  {
    return 1;
  }
  else
  {
#endif /*FEATURE_HDR_QCHAT*/

  /* Enable security protocol and get logical channel number for this
     flow */
  chan_idx = hdrbcsec_channel_enable( flow_id );

  HDR_MSG_PROT_2 ( MSG_LEGACY_HIGH,
                   "HDR MCS Framing enable: flow ID %d -> chan %d",
                   flow_id, 
                   chan_idx );

  /* If channel is valid... */
  if (chan_idx != -1)
  {

    if (hdrbcframe[chan_idx].ref_count == 0)
    {

      /* Store framing protocol config data for this channel */
      hdrbcframe[chan_idx].flow_id = flow_id;
      hdrbcframe[chan_idx].fcs_included = hdrbcovhd_is_fcs_included();  
      hdrbcframe[chan_idx].deframer_type = hdrbcovhd_get_framing_type();
      hdrbcframe[chan_idx].data_cb = data_cb;

      hdrbcframe_deframer_initialize(chan_idx);
      
      /* Reset metrics */
      hdrbcframe_clear_channel_metrics((uint8) chan_idx);

    }

    hdrbcframe[chan_idx].ref_count++;
  }

  return chan_idx; 

#ifdef FEATURE_HDR_QCHAT
  }
#endif /*FEATURE_HDR_QCHAT*/
} /* hdrbcframe_process_flow_enable */

 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_FLOW_DISABLE

DESCRIPTION
  Disables a broadcast flow.  The command will be queued and
  handled by the broadcast task context.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow ID to disable.
  sender  - Protocol sending the command
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_flow_disable
(
  uint32 flow_id,
  hdrhai_protocol_name_enum_type sender
)
{

  /* Temporary command buffer. */
  hdrbcframe_cmd_union_type cmd_buf;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "hdrbcframe_flow_disable(), flow ID (%d)", 
                  flow_id );

  cmd_buf.name = HDRBCFRAME_FLOW_DISABLE_CMD;
  cmd_buf.sender = sender;
  cmd_buf.params.enable_flow.flow_id = flow_id;

  hdrbctask_queue_cmd( HDRHAI_BROADCAST_FRAMING_PROTOCOL, 
                       &cmd_buf, sizeof(cmd_buf) );

} /* hdrbcframe_process_flow_disable */

 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_PROCESS_FLOW_DISABLE

DESCRIPTION
  The function disables a channel in the HDR Broadcast Framing Protocol.
  The handle returned by hdrbcframe_channel_enable() is used to indicate 
  which channel to close.
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - The channel to disable.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_process_flow_disable
(
  uint32 flow_id
)
{
  int chan_idx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_QCHAT
    /* Check for ann flow */
  if(hdrqchatupk_mcs_enabled() &&
     hdrqchatupk_bcframe_process_announce_flow_disable(flow_id))
  {
    return;
  }
  else
  {
#endif /*FEATURE_HDR_QCHAT*/
  /* Disable security layer (for this flow) and get channel index. */
  chan_idx = hdrbcsec_channel_disable( flow_id );
  
  HDR_MSG_PROT_2 ( MSG_LEGACY_HIGH,
                   "HDR MCS Framing disable: flow %d (%d)",
                   flow_id, 
                   chan_idx );

  /* If the channel index is valid... */
  if (chan_idx != -1) 
  { 

    if (hdrbcframe[chan_idx].ref_count != 0)
    {

      hdrbcframe[chan_idx].ref_count = 0;
    
      hdrbcframe_deframer_reset(chan_idx);
    }
    else
    {
      /* Error! */
      HDR_MSG_PROT_1 ( MSG_LEGACY_ERROR,
                       "Disable fail; inactive chan %d", 
                       chan_idx );
    }
  }

#ifdef FEATURE_HDR_QCHAT
  }
#endif /*FEATURE_HDR_QCHAT*/
} /* hdrbcframe_process_flow_disable */

 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_DISCARD_PROT_ID                              LOCAL

DESCRIPTION
  This function discards the protocol ID from the framing packet.

  When using PPP, this protocol ID belongs to PPP. It can carry a protocol 
  such as IP. Likewise when using BCMCS, the protocol field belongs to BCMCS. 
  As it turns out, BCMCS borrows from PPP RFC 1661 in this regard and allows 
  for the same encoding for the value of that field.
 
DEPENDENCIES
  None.

PARAMETERS
  framed_pkt - DSM item containing the broadcast packet whose PPP header should
               be removed.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
__inline static void hdrbcframe_discard_prot_id
(
   dsm_item_type **framed_pkt
)
{
  uint8 prot_id = 0;
    /* BCMCS Protocol ID */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------
    The protocol ID could either be single-byte or two-byte encoding.  
    Two-byte encoding:    First byte is always even and the second byte odd.
    Single-byte encoding: First byte is always odd. 
    ---------------------------------------------------------------------*/

  /* Read one byte */
  (void) dsm_pullup( framed_pkt,
                     &prot_id,
                     HDRBCFRAME_MIN_PROT_ID_LEN );

  /* Check if the byte read is odd? */
  if ( HDRBCFRAME_IS_BYTE_ODD(prot_id) == FALSE)
  {
    /* First byte is not odd. So throw-away one more byte... */
    (void) dsm_pullup( framed_pkt, 
                       &prot_id,
                       HDRBCFRAME_MIN_PROT_ID_LEN );
  }

} /* hdrbcframe_discard_prot_id */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_SEGMENT_DEFRAME

DESCRIPTION
  The function processes Broadcast Framing layer packets.
  
DEPENDENCIES
  None.

PARAMETERS
  data     - A pointer to the packet data
  length   - length of the data
  chan_idx - the channel the data arrived on.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_segment_deframe
(
   dsm_item_type* framed_pkt,
   uint16 length,
   int chan_idx
)
{

  uint8 crc_field[2];

  uint16 bytes_remaining;
  dsm_item_type *dup_pkt = NULL;  
  uint8 frame_header;
  uint16   fragment_length;
  boolean last_fragment;
  uint16 length_unframed;
  hdrbcframe_segment_deframer *seg;
  
  boolean fcs_included = hdrbcframe[chan_idx].fcs_included;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  seg = &hdrbcframe[chan_idx].deframer.seg_def;
  
  bytes_remaining = length;

  /* Step through packet */
  while (bytes_remaining)
  {

    /* Grab framing header */
    (void) dsm_pullup( &framed_pkt, &frame_header, 
                       HDRBCFRAME_SEGMENT_HEADER_LEN );
    bytes_remaining--;

    /* Get length and last fragment fields */
    fragment_length = HDRBCFRAME_GET_FRAGMENT_LENGTH( frame_header );
    last_fragment = HDRBCFRAME_GET_LAST_FRAGMENT( frame_header );
 
    /* Error checking. */ 
    if ( bytes_remaining < fragment_length )
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH,"Bogus header; dropping framing pkt.");
      
      fragment_length = 0;

      /* Free the packet we were working on; it was corrupt. */
      dsm_free_packet(&(seg->unframed_pkt));
      seg->crc_value = seg->crc_seed;

    }

    if ( fragment_length )
    {
      bytes_remaining-=fragment_length;

      /* If this fragment doesn't take up entire packet, dup the rest of the
         packet for later processing. */
      if (bytes_remaining)    
      {
        (void) dsm_dup_packet( &dup_pkt, 
                               framed_pkt, 
                               fragment_length, 
                               bytes_remaining );
        dsm_trim_packet(&framed_pkt, fragment_length);

      } 
    
      /* Update crc */
      if (fcs_included)
      {

        seg->crc_value =  crc_16_step( (uint16) seg->crc_value, 
                                       framed_pkt->data_ptr, 
                                       fragment_length );
      }

      /* Append packet to what we have so far */
      if ( seg->unframed_pkt )
      {
        /* framed_pkt is NULL after this op */
        dsm_append( &(seg->unframed_pkt), &framed_pkt );
      }
      else
      {
        seg->unframed_pkt = framed_pkt;
        framed_pkt = NULL;
      }
    
      /* If this is the last fragment of the higher layer packet... */
      if (last_fragment)
      {

        /*  Since last_fragment was set, we should have an entire upper 
            layer packet in seq->unframed_pkt. */

        length_unframed = dsm_length_packet(seg->unframed_pkt);

        /* Is packet good? */
        if ( ( (!fcs_included) || (seg->crc_value == CRC_16_OK) ) && 
            (length_unframed > seg->min_length))
        {
          length_unframed -= seg->min_length;

          if (fcs_included)
          {
            /* Remove CRC */
            (void) dsm_pullup_tail( &(seg->unframed_pkt),
                                    crc_field,
                                    HDRBCFRAME_CRC_LEN );
          }

          /* We need to deframe the ppp-encapsulation before passing it to 
             higher layers.. */
          hdrbcframe_discard_prot_id(&seg->unframed_pkt);

          /* Update metrics */
          hdrbcframe[chan_idx].chan_metrics.bytes_delivered += length_unframed;
          hdrbcframe[chan_idx].chan_metrics.packets_delivered++;

          /* Pass up good data */
          hdrbcframe[chan_idx].data_cb(seg->unframed_pkt); 
        }
        else
        {

          /* Drop bad packets */
          hdrbcframe[chan_idx].chan_metrics.packets_dropped++;
          dsm_free_packet(&(seg->unframed_pkt));
          
        }

        seg->unframed_pkt = NULL;
        seg->crc_value = seg->crc_seed;

      }

      framed_pkt = dup_pkt;  
      dup_pkt = NULL;

    } 
    else
    {
      /* If segment length is zero, assume rest of packet is also zero 
         and toss packet. */
      bytes_remaining = 0;
    }

  }

  /* Free packet if there is anything there */
  if (framed_pkt)
  {
    dsm_free_packet(&framed_pkt);
  }
  
} /* hdrbcframe_segment_deframe */


 /* EJECT */
/*=============================================================================
 
FUNCTION HDRBCFRAME_HDLC_DEFRAME

DESCRIPTION
  Performs HDLC deframing using HDLC library functions.
   
PARAMETERS
  framed    - Pointer to head of DSM chain containing the broadcast frame to be 
              unframed.
  length    - Length of the packet
  chan_idx  - Logical channel index.
   
RETURN VALUE
  None. 
   
SIDE EFFECTS
  None.
  
=============================================================================*/
void hdrbcframe_hdlc_deframe
(
   dsm_item_type * framed,
   uint16 length,
   int chan_idx

   /*lint -esym(715,length)
     ignore lint error symbol (length) not used */
)
{

  void* instance = hdrbcframe[chan_idx].deframer.hdlc_def.handle;
    /* HDLC deframer instance's handle  */

  uint16 length_unframed = 0;
    /* Length (in bytes) of unframed packets */

  dsm_item_type* unframed = NULL;
    /* Pointer to DSM item containing unframed pkt */

  int err_number = HDLC_OK;
    /* Error code */

  boolean done_unframing = (framed == NULL);
    /* Are we done unframing? */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------
    HDLC Unframing is done only when hdlc_unframe returns
      1. HDLC_OK and framed is NULL or
      2. HDLC_BAD_HANDLE. 
    Continue while we are not done...
   ---------------------------------------------------------------------*/
  while (!done_unframing)
  {
    /* Initialize unframed DSM item */
    unframed = NULL; 

    /* Call HDLC unframer */
    (void) hdlc_unframe(instance, &framed, &unframed, NULL);

    /* Get the specifics of unframer error */
    err_number = hdlc_errno(instance);   

    /* If hdlc succeeds unframing data, pass the data and continue unframing if 
       necessary... */
    if ( (err_number == HDLC_OK) || (err_number == HDLC_MORE_PKTS_AVAIL) )
    {
      /*--------------------------------------------------------------------
                               UNFRAMING SUCCESSFUL
       -------------------------------------------------------------------*/

      /* If the unframer has more packets available... */     
      if (err_number == HDLC_MORE_PKTS_AVAIL)
      {
        /*----------------------------------------------------------------
          HDLC has unframed more data than the dsm item "unframed" could 
          hold. So, it sets the framed to NULL(if the entire frame was 
          unframed) and returns HDLC_MORE_PKTS_AVAIL. hdlc_unframe should 
          be called again to retrieve the unframed data stored internally 
          within HDLC library.
         ---------------------------------------------------------------*/
        HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                     "HDLC Unframer: More packets available");
      }
      else
      {
        /* If err_number is HDLC_OK we are done unframing the packet, only
           if "framed" is NULL */
        done_unframing = (framed == NULL);
      }

      /* If the unframer returned non-null unframed packet back... */
      if (unframed != NULL)
      {
        /* We need to deframe and discard the protocol ID before passing it 
           to higher layers. */
        hdrbcframe_discard_prot_id(&unframed);

        /* The DSM pull-up in the above function could drive "unframed"
           DSM item to NULL, if the item had only protocol ID. Checking
           for NULL again... */
        if (unframed != NULL)
        {
          /* Find num bytes unframed */
          length_unframed = dsm_length_packet(unframed);

          /* Update statistics */
          hdrbcframe[chan_idx].chan_metrics.bytes_delivered += length_unframed;
          hdrbcframe[chan_idx].chan_metrics.packets_delivered++;

          /* Pass up good data */
          hdrbcframe[chan_idx].data_cb(unframed); 
        }
        else
        {
          HDR_MSG_PROT_2( 
            MSG_LEGACY_MED, 
            "Unframed pkt had only protocol ID. Bytes: %d, pkts: %d", 
            hdrbcframe[chan_idx].chan_metrics.bytes_delivered, 
            hdrbcframe[chan_idx].chan_metrics.packets_delivered);
        }
      }

    } /* (err_number == HDLC_OK) || (err_number == HDLC_MORE_PKTS_AVAIL) */

    /*--------------------------------------------------------------------
                                  ERROR HANDLING
     -------------------------------------------------------------------*/
    else
    {
      /* Increment the number of packets dropped... */
      hdrbcframe[chan_idx].chan_metrics.packets_dropped++;

      /* Print statistics */
      HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
                      "HDLC Error FlowID:%d errNo %d Fail %u Pass %u",
                      hdrbcframe[chan_idx].flow_id,
                      err_number,
                      hdrbcframe[chan_idx].chan_metrics.packets_dropped,
                      hdrbcframe[chan_idx].chan_metrics.packets_delivered);

      /* Handle error case */
      switch(err_number)
      {

        /* Unframed a packet with Bad CRC. Print an error message and try 
           to unframe the rest */
        case HDLC_BAD_CRC:
          HDR_MSG_PROT(MSG_LEGACY_HIGH,"Bad CRC in frame");
          break;

        /* Bad Handle case: Don't continue further. Just give up */
        case HDLC_BAD_HANDLE:
          done_unframing = TRUE;
          dsm_free_packet(&framed);
          break;
          
        /* No action for default case */
        default:
          break;

      } /* switch(err_number) */

      /* For all error cases, discard unframed if non-null */
      if (unframed != NULL)
      { 
         HDR_MSG_PROT(MSG_LEGACY_HIGH, "HDLC error: Freeing unframed packet");
         dsm_free_packet(&unframed);
      }

    } /* else (error) */

  } /* while (!done_unframing) */
  
} /* hdrbcframe_hdlc_deframe */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_PUT_PKT

DESCRIPTION
  The function processes Broadcast Framing layer packets.
  
DEPENDENCIES
  None.

PARAMETERS
  data     - A pointer to the packet data
  length   - length of the data
  chan_idx - the channel the data arrived on.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_put_pkt
(
  dsm_item_type* data, 
  uint16 length,
  int chan_idx
)
{
  
  hdrbcovhd_framing_type new_framing = hdrbcovhd_get_framing_type();
  boolean           new_fcs_included = hdrbcovhd_is_fcs_included();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* We don't expect empty packets */
  if ( data == NULL )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,"Unexpected NULL pointer");
    return;
  }

  /* We have received packet for processing, update incoming metrics */
  hdrbcframe[chan_idx].chan_metrics.packet_count++;
  hdrbcframe[chan_idx].chan_metrics.byte_count += length;   

  /* Has framing changed? */
  if (( hdrbcframe[chan_idx].deframer_type != new_framing) ||
      ( hdrbcframe[chan_idx].fcs_included != new_fcs_included))
  {
    /* Reset the old deframer */
    hdrbcframe_deframer_reset(chan_idx);

    /* Initialize the new framing type */
    hdrbcframe[chan_idx].deframer_type = new_framing;
    hdrbcframe[chan_idx].fcs_included = new_fcs_included;
      
    hdrbcframe_deframer_initialize(chan_idx);
  }
  
  /* Deframe based on configured framing type */
  switch ( hdrbcframe[chan_idx].deframer_type )
  {
    case HDRBCOVHD_FRAMING_HDLC:
      hdrbcframe_hdlc_deframe(data, length, chan_idx);
      break;

    case HDRBCOVHD_FRAMING_SEGMENT:
      hdrbcframe_segment_deframe(data, length, chan_idx);
      break;

    default:
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                     "Dropped packet. Invalid Deframer %d Logical ch: %d",
                     hdrbcframe[chan_idx].deframer_type,
                     chan_idx);

      /* Free the DSM item */
      dsm_free_packet(&data);
      break;
  }

  /* Periodic status update */
  if ((hdrbcframe[chan_idx].chan_metrics.packet_count % 100) == 0)
  {
    HDR_MSG_PROT_3 ( MSG_LEGACY_HIGH,
                     "MCS Framing FlowID %d Stats: Fail %u Pass %u",
                     hdrbcframe[chan_idx].flow_id, 
                     hdrbcframe[chan_idx].chan_metrics.packets_dropped,
                     hdrbcframe[chan_idx].chan_metrics.packets_delivered );
  }

} /* hdrbcframe_put_pkt */

 /* EJECT */
/*===========================================================================

FUNCTION HDRBCFRAME_PROCESS_CMD

DESCRIPTION
  This function processes the commands for this protocol that were queued
  on the task command queue from a different task context.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - address of command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_process_cmd
(
  hdrbcframe_cmd_union_type *cmd_ptr
)
{

  HDRTRACE_CMD(HDRHAI_BROADCAST_FRAMING_PROTOCOL, cmd_ptr->name, 
               cmd_ptr->sender, 0);

  switch ( cmd_ptr->name )
  {
    case HDRBCFRAME_FLOW_ENABLE_CMD:
      /*-----------------------------------------------------------------------
      Flow Enable command.
      -----------------------------------------------------------------------*/
      (void) hdrbcframe_process_flow_enable(cmd_ptr->params.enable_flow.flow_id, 
                                       cmd_ptr->params.enable_flow.data_cb);
    break;

    case HDRBCFRAME_FLOW_DISABLE_CMD:
      /*-----------------------------------------------------------------------
      Flow Disable command.
      -----------------------------------------------------------------------*/
      hdrbcframe_process_flow_disable(cmd_ptr->params.disable_flow.flow_id);
    break;

    default:
      /*-----------------------------------------------------------------------
      Unrecognized command.
      -----------------------------------------------------------------------*/
      ERR("BCFRAME: Received Unrecognized command:%d", cmd_ptr->name, 0, 0 );
    break;
  }

} /* hdrbcframe_process_cmd() */

#endif /* FEATURE_HDR_BCMCS */
