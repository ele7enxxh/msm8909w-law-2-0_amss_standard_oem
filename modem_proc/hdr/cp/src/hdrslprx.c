/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            H D R   S I G N A L I N G   L A Y E R   P R O T O C O L

GENERAL DESCRIPTION
  Receiveing functionality of HDR SLP (Reassembly, FCS check, acking
  of reliable messages).

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrslprx.c_v   1.10   01 Aug 2001 21:38:38   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrslprx.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/13   ukl     Added additional critical sections to guard SLP data structures.
03/08/12   wsh     Merged NLB support
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
08/18/11   cnx     Set CC channel bit for CC msg.
05/18/11   wsh     Added network load balancing support
12/06/06   sju     Make SLP reassembly buffers standard compliant.
11/15/06   yll     Removed DSM tracer field to save memory.
07/18/06   dna     Account for SLP header breaknig over DSM item boundary
04/28/05   dna     Still ACK reliable messages even if they are duplicates.
03/24/05   pba     Lint cleanup
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDRPROT SSID
08/01/01   dna     Added logging of ack sequence number valid bit of SLP 
                   header as part of the signaling message log packet
02/22/00   om      Created Module


===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "qw.h"

#include "hdrerrno.h"
#include "hdrhai.h"
#include "hdrhmp.h"
#include "hdrdsm.h"
#include "hdrsrch.h"
#include "hdrtrace.h"

#include "hdrslp.h"
#include "hdrslpi.h"
#include "hdrslptx.h"
#include "hdrslprx.h"
#include "hdrrup.h"

#ifdef FEATURE_HDR_REVC
#include "hdrlmac.h"  /* hdrlmac_is_linkid_on_cc_monitoring_chan() */
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

hdrslp_reassembly_buffer_struct_type  hdrslp_reassembly_buffer[2];
uint8                                 hdrslp_seqno_expected;
uint8                                 hdrslp_seqno_received;

#ifdef HDRSLP_TST_SVR
  /*** This function is used bu the SLP test server to ***/
  /*** induce fake errors. It is _not_ intended for    ***/
  /*** regular DMSS builds.                            ***/
  boolean hdrslp_tm_rx_error( dsm_item_type* msg_ptr );
#endif 

#ifndef HDRSLP_NO_LOGGING
  hdrslp_sdu_struct_type last_rx_sdu;
#endif

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSLP_PROCESS_RCVD_SIGNALING

DESCRIPTION
  This function processes received signaling data:
    1. Reassemble SDU fragments
    2. Do FCS check
    3. Maintain the bit array of the sequence space and check
       for duplicates
    4. Update list of SLP ACKs to be sent w/ next frame

  The function returns on unknown fragments, unfinished reassembly,
  FCS failure, and duplicate reliable SDUs. Otherwise, it calls
  hdrhmp_route_msg() for further processing.

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the SDU

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_rcvd_signaling( dsm_item_type *msg_ptr )
{
  uint8    seqno;
  uint16   ack_buffer_field;
  uint16   i;
  uint32   length;
  int      index;
  boolean  duplicate_msg_rxed = FALSE;

  uint8    slp_header[3]; /* Left justified; Last byte(s) may be unused */
  int      slp_header_size; /* Size of header on this fragment */
  boolean call_hdrslp_flush_reassembly_buf = FALSE;

#ifdef FEATURE_HDR_REVC
  hdrsrch_link_id_type link_id;
  boolean  pri_cc;
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
  uint8 prot_id;
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) dsm_extract( msg_ptr, 0, slp_header, 3);
  slp_header_size = HDRSLP_SDU_HDR_SIZE( slp_header );

#ifdef FEATURE_HDR_REVC
  link_id = HDRDSM_GET_LINK_ID( msg_ptr );

  pri_cc = hdrlmac_is_linkid_on_cc_monitoring_chan(link_id);

  if ( HDRDSM_GET_CHAN_BIT( msg_ptr ) &&
       !pri_cc )
  {    
    HDR_MSG_PROT_2(MSG_LEGACY_MED, "Non-pri CC bcast pkt F? %d R? %d",
                   HDRSLP_SDU_FRAGMENT( slp_header ),
                   HDRSLP_SDU_RELIABLE( slp_header ));

    /* if CC msg and not on primary control channel */
    if ( HDRSLP_SDU_FRAGMENT( slp_header ) )
    {
      /* Currently we do not support fragmented MSG on non-pri CC channel.
         SP message may be fragmented but the it only happens rarely.
         Once a non-fragmented SP is received, QC is sufficient to keep
         NLB cache up-to-date */
      HDR_MSG_PROT_2(MSG_LEGACY_MED, "Drop SLPF msg 0x%x on link %d",
                     msg_ptr, link_id);
      dsm_free_packet( &msg_ptr );
    }
    else if ( HDRSLP_SDU_RELIABLE( slp_header ) )
    {
      /* Only CMAC broadcast message are passed thru by CMAC. CMAC
         broadcast message should be best-effort */
      HDR_MSG_PROT_2(MSG_LEGACY_MED, "Drop SLPD msg 0x%x on link %d",
                     msg_ptr, link_id);
      dsm_free_packet( &msg_ptr );
    }
    else
    {
      /* not fragmented and best-effort, can be delivered to HMP directly */
#ifndef HDRSLP_NO_LOGGING
      length = dsm_length_packet( msg_ptr );

      /* Log the message */
      hdrslp_log_msg( link_id,
                      (length - slp_header_size),
                      slp_header_size,
                      HDRDSM_GET_BCAST_BIT( msg_ptr ),
                      HDRDSM_GET_CHAN_BIT( msg_ptr )? HDRHAI_CC : HDRHAI_FTC,
                      (HDRDSM_GET_CHAN_BIT(msg_ptr) ?
                      HDRDSM_GET_CC_SYNC_BIT(msg_ptr) : FALSE),
                      last_rx_sdu.rel, 
                      last_rx_sdu.frg, 
                      last_rx_sdu.sno, 
                      last_rx_sdu.ano, 
                      last_rx_sdu.ano_valid,
                      msg_ptr );
#endif

      /* Strip header and deliver good SDUs to upper layer */
      (void) dsm_pullup( &msg_ptr, NULL, (uint16) slp_header_size );

      if ( msg_ptr != NULL )
      {
        DSM_TOUCH_ITEM( msg_ptr );
        hdrhmp_route_msg( msg_ptr );
      }
    }

    return;
  }
#endif /* FEATURE_HDR_REVC */

  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++  S L P - F  +++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

  /* Account for the case where the SLP header broke over a DSM item 
   * boundary. 
   */

  /* Largest header is 3 bytes.  If this is a header only ack with a 1 or
   * 2 byte header, the first byte(s) will be in extracted into slp_header and
   * dsm_extract will return a 1 or 2 instead of 3.  Otherwise, for a 
   * 1 or 2 byte header the last bytes of slp_header[] will be the first
   * bytes of the message which will not be used.
   */

  switch (slp_header_size)
  {
    case 1:
      HDR_MSG_PROT_1 (MSG_LEGACY_MED, "slp_header (1 byte) %x", slp_header[0]);
      break;
    case 2:
      HDR_MSG_PROT_2 (MSG_LEGACY_MED, "slp_header (2 bytes) %x:%x", 
                      slp_header[0],slp_header[1]);
      break;
    case 3:
      HDR_MSG_PROT_3 (MSG_LEGACY_MED, "slp_header (3 bytes) %x:%x:%x", 
                      slp_header[0],slp_header[1],slp_header[2]);
      break;
  }

  /* Shortcut to the reassembly buffer index */
  index = HDRSLP_RXA_BUF( msg_ptr );

  /* Check if this is a fragment or an entire SDU */
  if ( HDRSLP_SDU_FRAGMENT( slp_header ) )
  {
    if (hdrslp_verbose_debug)
    {
      HDR_MSG_PROT (MSG_LEGACY_MED,  "=RX= Rxed fragmented SDU"); 
    }

    if ( HDRSLP_SDU_FIRST( slp_header ) )
    {
      /* Receive SDU with Begin field == 1. Flush reassembly buffer */
      hdrslp_flush_reassembly_buf( index );

      hdrslp_enter_crit_sect();
	  
      hdrslp_reassembly_buffer[index].buffer_ptr = msg_ptr;
      hdrslp_reassembly_buffer[index].length = 
        dsm_length_packet( msg_ptr ) - slp_header_size;
      hdrslp_reassembly_buffer[index].frg_seqno =
        HDRSLP_SDU_FRG_SEQNO( slp_header );
      if (hdrslp_verbose_debug)
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "=RX= RX 1st frg, seqno=%d", 
                     hdrslp_reassembly_buffer[index].frg_seqno);
      }

      /* Increment the expected fragment sequence number modulo 64 */
      hdrslp_reassembly_buffer[index].frg_seqno++;
      hdrslp_reassembly_buffer[index].frg_seqno %= 64;

      hdrslp_leave_crit_sect();

      /* Indicate that we don't have a complete message yet */
      msg_ptr = NULL;
    }
    else
    {
      hdrslp_enter_crit_sect();
      /* Check if we are currently reassembling and whether the tag is ok */
      if ( (hdrslp_reassembly_buffer[index].buffer_ptr != NULL) && 
           (HDRSLP_SDU_FRG_SEQNO( slp_header ) == 
            hdrslp_reassembly_buffer[index].frg_seqno) )
      {
        if ( HDRSLP_SDU_LAST( slp_header ) ) 
        {
          hdrslp_reassembly_buffer[index].last_frg =
            TRUE;
        }

        /* Update dsm item to point to payload only */
        (void) dsm_pullup( &msg_ptr, NULL, slp_header_size );

        hdrslp_reassembly_buffer[index].length += 
          dsm_length_packet( msg_ptr );
        dsm_append( 
          &(hdrslp_reassembly_buffer[index].buffer_ptr),
          &msg_ptr );
  
        /* Increment the expected fragment sequence number modulo 64 */
        hdrslp_reassembly_buffer[index].frg_seqno++;
        hdrslp_reassembly_buffer[index].frg_seqno %= 64;

        if ( hdrslp_reassembly_buffer[index].last_frg )
        {
          /* The buffer now contains a normal SDU w/ one header,        */
          /* use msg_ptr as the pointer to it for everything following  */
          /* and mark the reassembly buffer as empty.                   */
          msg_ptr = 
            hdrslp_reassembly_buffer[index].buffer_ptr;
          hdrslp_reassembly_buffer[index].buffer_ptr = 
            NULL;
          hdrslp_reassembly_buffer[index].last_frg = FALSE;

          hdrslp_stats.rx_fragment_msg_cnt++;

#ifndef HDRSLP_NO_LOGGING
          last_rx_sdu.frg = TRUE;
#endif
        }
        else
        {
          /* Indicate that we don't have a complete message yet */
          msg_ptr = NULL;

          if (hdrslp_verbose_debug)
          {
            HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "=RX= RX cont. frg, tag=%d", 
                         hdrslp_reassembly_buffer[index].frg_seqno);
          }
        }
      }
      else
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "RX unexp. frg, seqno=%d", 
                       HDRSLP_SDU_FRG_SEQNO( slp_header ));

        /* Rx unexp. frg seq no, Flush reassembly buffer 
         * C.S0024-A v3.0, 2.6.4.3.5 - Receiver Requirements:
         * If the SequenceNumber of the current SLP-F packet is not one greater 
         * than SequenceNumber of the last SLP-F packet whose payload was written 
         * to the reassembly buffer, then the receiver shall discard the contents 
         * of the re-assembly buffer
         */
	/*setting the flag to call the function outside the critical section*/
        call_hdrslp_flush_reassembly_buf = TRUE;

        /* Discard the packet - no fragment expected or wrong tag */
        dsm_free_packet( &msg_ptr );

        /* Update stats */
        hdrslp_stats.rx_out_of_order_cnt++;
      }
      
      hdrslp_leave_crit_sect();

      if(call_hdrslp_flush_reassembly_buf == TRUE)
      {
         hdrslp_flush_reassembly_buf( index );
      }
    }

    /* Check if the reassembly processing resulted in a complete msg */
    if ( msg_ptr == NULL ) return;    
  }
  else /* SDU was not fragmented */
  {
#ifndef HDRSLP_NO_LOGGING
      last_rx_sdu.frg = FALSE;
#endif

      /* Receive non-fragmented SDU, Flush reassembly buffer 
       * C.S0024-A v3.0, 2.6.4.3.5 - Receiver Requirements:
       * - If the SLP-F header Fragmented field is ‘0’, then the receiver shall 
       *   assume the SLP-F header Begin field is ‘1’, the SLP-F header End field 
       *   is ‘1’ and the SLP-F header SequenceNumber is ‘0’. 
       * - If the SequenceNumber of the current SLP-F packet is not one greater 
       *   than SequenceNumber of the last SLP-F packet whose payload was written 
       *   to the reassembly buffer, then the receiver shall discard the contents 
       *   of the re-assembly buffer.
       * - If the Begin field is ‘1’, then the receiver shall discard the contents 
       *   of the re-assembly buffer and set the Sync flag to ‘1’.
       */
      hdrslp_flush_reassembly_buf( index );
  }

  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++  S L P - D  +++++++++++++++++++++++++++++ */
  /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

  if ( HDRSLP_SDU_FRAGMENT( slp_header ) )
  {
    /* If the msg_ptr being processed by SLP-D was reassembled from fragments,
     * then the slp_header used by SLP-D is the header from the first 
     * fragment, which needs to be re-extracted from the message since
     * slp_header currently holds the header from the last fragment.
     */
    (void) dsm_extract( msg_ptr, 0, slp_header, 3);
    slp_header_size = HDRSLP_SDU_HDR_SIZE( slp_header );

    switch (slp_header_size)
    {
      case 1:
        HDR_MSG_PROT_1 (MSG_LEGACY_MED, "slpd_header (1 byte) %x", 
                        slp_header[0]);
        break;
      case 2:
        HDR_MSG_PROT_2 (MSG_LEGACY_MED, "slpd_header (2 bytes) %x:%x", 
                        slp_header[0],slp_header[1]);
        break;
      case 3:
        HDR_MSG_PROT_3 (MSG_LEGACY_MED, "slpd_header (3 bytes) %x:%x:%x", 
                        slp_header[0],slp_header[1],slp_header[2]);
        break;
    }
  }

  /* Update SLP xmit side with received ACK seqno.              */
  seqno = HDRSLP_SDU_ACK_SEQNO( slp_header );
  if ( seqno != HDRSLP_SDU_SEQNO_INVALID )
  {
    hdrslp_process_tx_ack( seqno );
  }

#ifndef HDRSLP_NO_LOGGING
  last_rx_sdu.ano = HDRSLP_SDU_ACK_SEQNO( slp_header );
  if (last_rx_sdu.ano == HDRSLP_SDU_SEQNO_INVALID)
  {
    last_rx_sdu.ano_valid = FALSE;
    if (HDRSLP_SDU_FULL_HDR( slp_header ))
    {
      /* We should log the real value the AN put in the header */
      last_rx_sdu.ano = 
        HDRSLP_SDU_FRAGMENT( slp_header ) ? 
          ((slp_header[2] & 0x70) >> 4) : ((slp_header[1] & 0x70) >> 4);
    }
  }
  else
  {
    last_rx_sdu.ano_valid = TRUE;
  }
#endif

#ifdef HDRSLP_TST_SVR
  /*** This is used by the SLP test server to induce rx errors ***/
  if ( hdrslp_tm_rx_error( msg_ptr ) )
  {
    dsm_free_packet( &msg_ptr );
    return;
  }
#endif

  /* Check and update sequence number array */
  if ( HDRSLP_SDU_RELIABLE( slp_header ) )
  {
    /* store sequence number locally to save some repeated logic  */
    seqno = HDRSLP_SDU_SEQNO( slp_header );

    /* Compare sequence numbers in mod 8 arithmetic:              */
    if ( (seqno + 12 - hdrslp_seqno_expected) % 8 < 4 )
    {
      /* Got sequence number smaller than expected - it's either  */
      /* a duplicate or a previously missing SDU.                 */
      if ( hdrslp_seqno_received & (0x01 << seqno) )
      {
        /* SDU is a duplicate, discard it but still send an ACK */
        HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,  "RX duplicate - rcvd %d / exp %d\n",
                       seqno, hdrslp_seqno_expected);
        
        dsm_free_packet( &msg_ptr );
        duplicate_msg_rxed = TRUE;

        /* Update stats */
        hdrslp_stats.rx_duplicate_cnt++;
      }
      else
      {
        hdrslp_seqno_received |= (0x01 << seqno);
      }
    }
    else
    {
      /* Got expected sequence number - move sequence space one up */
      if ( seqno == hdrslp_seqno_expected )
      {
#ifdef TARGET_OS_SOLARIS
        printf( "slp: Got expected seqno %d, erasing bit %d\n",
                seqno, ((seqno + 4) % 8) );
#endif
        hdrslp_seqno_received |= (0x01 << seqno);
        hdrslp_seqno_received &= ~(uint8)(0x01 << ((seqno + 4) % 8));
        hdrslp_seqno_expected = (hdrslp_seqno_expected + 1) % 8;
      }
      else
      {
        /* Got sequence number larger than expected, set the   */
        /* next four sequence numbers to zero and mark the     */
        /* received one.                                       */
        if (hdrslp_verbose_debug)
        {
          HDR_MSG_PROT_2 (MSG_LEGACY_MED,  "RX seqno %d > exp %d\n", 
                       seqno, hdrslp_seqno_expected);
        }
#ifdef TARGET_OS_SOLARIS
        printf( "slp: Got > expected seqno %d, exp = %d\n",
                seqno, hdrslp_seqno_expected );
#endif
        hdrslp_seqno_received |= (0x01 << seqno);
        for ( i=1; i<=4; i++ )
          hdrslp_seqno_received &= ~(uint8)(0x01 << ((seqno + i) % 8));
        hdrslp_seqno_expected = (seqno + 1) % 8;
      }
    }

#ifdef TARGET_OS_SOLARIS
    {
      int ii;

      printf( "slp: rcvd seqno = " );
      for ( ii=7; ii>=0; ii-- ) if ( hdrslp_seqno_received & (0x01 << ii ) )
                                  printf( "|" ); else printf( "." );
      printf( "\n" );
    }
#endif

    /* Update scheduling of Ack's to be sent - each ack field  */
    /* in the buffer has these fields:                         */
    /*                            8       0                    */
    /*                             u0tttsss                    */
    /* where u - used, s - seqno, t - timer.                   */
    /* Initially the field is set to used, seqno, 5 frames.    */
    /* Updating the field is atomic and does not need to be    */
    /* protected with intlocks.                                */

    ack_buffer_field = 0xa8 | seqno;
    hdrslp_rx_ack_buffer[hdrslp_rx_ack_buffer_write_index] = ack_buffer_field;
    hdrslp_rx_ack_buffer_write_index = 
      (hdrslp_rx_ack_buffer_write_index + 1) % 4;

    if (duplicate_msg_rxed)
    {
      return;  
    }

    hdrslp_stats.rx_rel_msg_cnt++;

#ifndef HDRSLP_NO_LOGGING
    last_rx_sdu.rel = TRUE;
    last_rx_sdu.sno = seqno;
#endif
    if (hdrslp_verbose_debug)
    {
      HDR_MSG_PROT_3 (MSG_LEGACY_MED,  "RX rel msg, len=%d, seq=%d, ack=%d",
                   dsm_length_packet( msg_ptr ), 
                   seqno,
                   HDRSLP_SDU_ACK_SEQNO( slp_header ) );
    }
  }
  else
  {
    /* We got a best-effort message */
    hdrslp_stats.rx_bef_msg_cnt++;

#ifndef HDRSLP_NO_LOGGING
    last_rx_sdu.rel = FALSE;
    last_rx_sdu.sno = 0xff;
#endif
    if (hdrslp_verbose_debug)
    {
      HDR_MSG_PROT_2 (MSG_LEGACY_MED,  "RX b-e msg, len=%d, ack=%d", 
                   dsm_length_packet( msg_ptr ), 
                   HDRSLP_SDU_ACK_SEQNO( slp_header ));
    }
  }

  /* Update the stats */
  length = dsm_length_packet( msg_ptr );
  hdrslp_stats.rx_byte_cnt += length;
  hdrslp_stats.rx_msg_cnt++; 
  hdrslp_stats.rx_min_msg_size = MIN( hdrslp_stats.rx_min_msg_size, length );
  hdrslp_stats.rx_max_msg_size = MAX( hdrslp_stats.rx_max_msg_size, length );

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_SIGNALING_LINK_PROTOCOL )
       && !hdrrup_is_tc_assigned (HDRHAI_SIGNALING_LINK_PROTOCOL ) )
  {
    (void) dsm_extract( msg_ptr, (uint16)slp_header_size, &prot_id, 1 );

    /* 1. All SAP msgs are on CC;
     * 2. If SAP is not in OPEN state, all incoming msgs are on CC */
    if ( prot_id == HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ||
         !hdrsap_connection_is_open( HDRHAI_SIGNALING_LINK_PROTOCOL ) )
    {
      HDRDSM_SET_CHAN_BIT( msg_ptr );
    }
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifndef HDRSLP_NO_LOGGING
  /* Log the message */
  hdrslp_log_msg( (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( msg_ptr ),
                  (length - slp_header_size),
                  slp_header_size,
                  HDRDSM_GET_BCAST_BIT( msg_ptr ),
                  HDRDSM_GET_CHAN_BIT( msg_ptr )? HDRHAI_CC : HDRHAI_FTC,
                  (HDRDSM_GET_CHAN_BIT(msg_ptr) ?
                  HDRDSM_GET_CC_SYNC_BIT(msg_ptr) : FALSE),
                  last_rx_sdu.rel, 
                  last_rx_sdu.frg, 
                  last_rx_sdu.sno, 
                  last_rx_sdu.ano, 
                  last_rx_sdu.ano_valid,
                  msg_ptr );
#endif

  /* Strip header and deliver good SDUs to upper layer */
  (void) dsm_pullup( &msg_ptr, NULL, (uint16) slp_header_size );

  if (msg_ptr == NULL)
  {
    /* This was an SLP ACK */
    HDRTRACE_SLP_ACK_MSG();
  }
  else
  {
    DSM_TOUCH_ITEM( msg_ptr );
    hdrhmp_route_msg( msg_ptr );
  }

} /* hdrslp_process_rcvd_signaling() */

