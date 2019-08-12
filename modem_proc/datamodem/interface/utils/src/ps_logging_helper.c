/*===========================================================================

                   P S _ L O G G I N G _ H E L P E R . C

GENERAL DESCRIPTION
  This file defines helper routines to log data

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004-2010,2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_logging_helper.c#1 $
 $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/09/10    sy     Fixed type-punned compiler warnings.
10/24/08    pp     Fixed compile warning observed in OFFTARGET build.
08/18/06    mjp    Fixed IPHC and ROHC Logging
08/31/06    mjp    Added IPHC and ROHC Logging
07/17/06    mp     Fixed logging of zero length DPL packets
06/05/06    mp     Added DPL PPP Partial Logging
05/12/05    ks     Fixed Lint errors.
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow and link logging.
11/15/04   ks/msr  Sending values to DPLI_BUILD_LOG_PACKET. Fixed a bug
                   where DSM is checked to see if it is corrupted.
10/31/04   ks/msr  Created module.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "ps_svc.h"
#include "ps_logging.h"
#include "ps_iface.h"

#include "amssassert.h"
#include "ps_logging_helper.h"
#include "ds_Utils_DebugMsg.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif /*TEST_FRAMEWORK*/

/*===========================================================================
                            LOCAL DEFINITIONS
===========================================================================*/
/* Used to set final bit in logging packet */
#define DPLI_SEGNUM_FINAL_BIT  0x8000

/* 0xFFFFFFFF is used to indicate that a data item failed to allocate*/
#define DPL_LOG_ALLOC_FAILED   (0xFFFFFFFF)

/* Used to represent short negative one so overflow sets num to zero when
          incremented */
#define ONE_LESS_THAN_ZERO     0xFFFF

/*===========================================================================
MACRO    DPLI_BUILD_LOG_PACKET_HDR

DESCRIPTION
  Constructs the log packet header.

PARAMETERS
  log_ptr       : Pointer to the log packet
  iid           : IID associated with this log packet
  frame_seq_num : Sequence number for this log packet
  seg_num       : Segment number for this log packet

SIDE EFFECTS
  log_ptr is advanced past the constructed dpl log header

RETURN VALUE
  NONE
===========================================================================*/
#define DPLI_BUILD_LOG_PACKET_HDR(log_ptr, iid, frame_seq_num, seg_num)    \
  /*casting to void* to take care of type-punned compiler warning    */\
  log_ptr = put32(log_ptr, *(uint32 *)((void *) &(iid)));              \
  log_ptr = hput16(log_ptr, frame_seq_num);                            \
  log_ptr = hput16(log_ptr, seg_num);



/*===========================================================================

                          INTERNAL FUNCTION DEFINITONS

===========================================================================*/
/*===========================================================================
FUNCTION    DPLI_LOG_FULL_PACKET

DESCRIPTION
  Logs a packet by potentially splitting it to multiple segments

PARAMETERS
  item_ptr      : pointer to the dsm chain to be logged
  len           : length of the packet to be logged
  iid           : IID of the data log point
  frame_seq_num : sequence number for the packet being logged

RETURN VALUE
  TRUE  : packet was logged
  FALSE : packet was not logged

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dpli_log_full_packet
(
  dsm_item_type  * item_ptr,
  uint32           len,
  dpl_iid_type     iid,
  uint16           frame_seq_num
)
{
  uint8   * log_ptr;           /* pointer to log buffer                    */
  uint8   * return_ptr;
  uint16    offset;            /* byte offset into DSM chain               */
  uint16    seg_num;           /* segment number sent in each logged frame */
  uint16    payload_len;       /* # of bytes sent in log packet            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset      = 0;
  seg_num     = 0;

  /*-------------------------------------------------------------------------
    Although dsm_seek_extract() modifies this value when we call
    DPLI_BUILD_LOG_PACKET(), value is reset to MAX_PAYLOAD when
    dsm_seek_extract() succeeds. So there is no need to reset the value
    before each call to DLI_BUILD_LOG_PACKET().
  -------------------------------------------------------------------------*/
  payload_len = MAX_PAYLOAD;

  /*-------------------------------------------------------------------------
    Log full-length segments while outstanding payload is > maximum log
    frame length.

    NOTE: Loop is unrolled in to two parts to optimize this function. So if
          body of this loop changes, change other part also.
  -------------------------------------------------------------------------*/
  while (len > MAX_PAYLOAD)
  {
    log_ptr = (uint8 *) log_alloc(LOG_DATA_PROTOCOL_LOGGING_C,
                                  MAX_PAYLOAD + DPL_TOTAL_HDR_SIZE);

    if (NULL == log_ptr)
    {
      LOG_MSG_INFO2_0("dpli_log_full_packet(): "
                      "log_alloc failed");
      return;
    }

    /*-----------------------------------------------------------------------
      Have to reset max_payload every time as DPLI_BUILDLOG_PACKET changes
      its value
    -----------------------------------------------------------------------*/
    return_ptr =  log_ptr;
    log_ptr    += LOG_HDR_SIZE;

    DPLI_BUILD_LOG_PACKET(&item_ptr,
                          log_ptr,
                          &offset,
                          iid,
                          &payload_len,
                          frame_seq_num,
                          seg_num++);

    if (MAX_PAYLOAD != payload_len)
    {
      LOG_MSG_ERROR_1("dpli_log_full_packet(): "
                      "DSM chain is corrupted, 0x%p", item_ptr);
      return;
    }

    log_commit (return_ptr);

    /*-----------------------------------------------------------------------
      Check for segment number wrap around
    -----------------------------------------------------------------------*/
    seg_num &= 0x7FFF;
    len     -= MAX_PAYLOAD;
  }

  /*-------------------------------------------------------------------------
    This part builds the log packet when len becomes < = MAX_PAYLOAD.
    len can never be zero as the condition used in above loop is
    "len > MAX_PAYLOAD". So here "0 < len <= MAXPAYLOAD".
  -------------------------------------------------------------------------*/
  log_ptr = (uint8 *) log_alloc(LOG_DATA_PROTOCOL_LOGGING_C,
                                len + DPL_TOTAL_HDR_SIZE);

  if (log_ptr == NULL)
  {
    LOG_MSG_INFO2_0("dpli_log_full_packet(): "
                    "log_alloc failed");
    return;
  }

  /*-------------------------------------------------------------------------
    As this is the last segment, set the F bit (most significant bit) of
    the segment number to 1
  -------------------------------------------------------------------------*/
  seg_num     |= DPLI_SEGNUM_FINAL_BIT;
  return_ptr  =  log_ptr;
  log_ptr     += LOG_HDR_SIZE;
  /**/
  payload_len = (uint16)len;
  /**/

  DPLI_BUILD_LOG_PACKET(&item_ptr,
                        log_ptr,
                        &offset,
                        iid,
                        &payload_len,
                        frame_seq_num,
                        seg_num);

  if (len != payload_len)
  {
    LOG_MSG_ERROR_1("dpli_log_full_packet(): "
                    "DSM chain is corrupted, 0x%p", item_ptr);
    log_free(return_ptr);
    return;
  }

  log_commit (return_ptr);

  return;

} /* dpli_log_full_packet() */



/*===========================================================================
FUNCTION    DPLI_LOG_PPP_PARTIAL_PACKET

DESCRIPTION
  Logs a packet potentially only partially and possibly by splitting it
  to multiple segments

PARAMETERS
  item_ptr          : pointer to the dsm chain to be logged
  dpl_id            : IID associated with this packet
  log_flag_link_cb  : pointer to the ppp link layer control block

RETURN VALUE
  TRUE  : partial packet was logged
  FALSE : partial packet was not logged

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dpli_log_ppp_partial_packet
(
  const dsm_item_type  *  item_ptr,
  dpl_iid_type            dpl_id,
  dpl_link_dir_cb_type *  log_flag_link_cb
)
{
  uint8   * log_ptr;         /* pointer to log buffer                      */
  uint8   * return_ptr;      /* save the start of log buffer here          */
  uint8   * src_ptr;         /* address of current data byte observed      */
  uint8   * src_end_ptr;     /* address of current data byte observed      */
  uint8   * dest_ptr;        /* where to copy payload (output log buffer)  */
  uint32    pbytes_logged;   /* partial bytes logged of the ppp packet     */
  uint32    snaplen;         /* Current snaplength                         */
  uint16    seg_num;         /* segment number sent in each logged frame   */
  uint16    dest_room_left;  /* length of the data to be logged            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Setup state
  -------------------------------------------------------------------------*/
  seg_num       = log_flag_link_cb->seg_num[DPL_IID_LINKPROT_PPP_IN_HDLC];
  pbytes_logged = log_flag_link_cb->partial_bytes_logged;
  snaplen       = log_flag_link_cb->snaplen;
  if (snaplen == SNAPLEN_ALL)
  {
    snaplen = 65535;
  }

  if ((NULL == item_ptr)||(NULL == item_ptr->data_ptr))
  {
    return;
  }
  src_ptr       = item_ptr->data_ptr;
  src_end_ptr   = src_ptr + item_ptr->used;

  /*-------------------------------------------------------------------------
    Loop until all input PPP data is logged
  -------------------------------------------------------------------------*/
  while (item_ptr)
  {
    /*-----------------------------------------------------------------------
      Check if the snap length has been reached and if so run through looking
      for 7E
    -----------------------------------------------------------------------*/
    if (pbytes_logged == snaplen)
    {
      while (*src_ptr != 0x7E)
      {
        while (*src_ptr != 0x7E && src_ptr != src_end_ptr)
        {
          src_ptr++;
        }
        /* Check if we got to the end of the current input DSM item */
        if (src_ptr == src_end_ptr)
        {
          /* Go to next DSM item */
          item_ptr = item_ptr->pkt_ptr;
          if (item_ptr == 0)
          {
             /* There is nothing more to read, log and leave */
             log_flag_link_cb->seg_num[DPL_IID_LINKPROT_PPP_IN_HDLC]
               = seg_num;
             log_flag_link_cb->partial_bytes_logged
               = pbytes_logged;
             return;
          }
          src_ptr      = item_ptr->data_ptr;
          src_end_ptr  = src_ptr + item_ptr->used;
        }
        else /* We reached a 7E */
        {
          pbytes_logged = 0;
          break;
        }
      }
    }

    /*-----------------------------------------------------------------------
      Alocate full-length log segment to work with
    -----------------------------------------------------------------------*/
    log_ptr = (uint8 *) log_alloc(LOG_DATA_PROTOCOL_LOGGING_C,
                                  MAX_PAYLOAD + DPL_TOTAL_HDR_SIZE);

    if (NULL == log_ptr)
    {
      LOG_MSG_INFO2_0("dpli_log_ppp_partial_packet(): "
                      "log_alloc failed");
      pbytes_logged = DPL_LOG_ALLOC_FAILED;
      break;  /* make sure to break to save back in control block */
    }

    /*-------------------------------------------------------------------------
      At the start of a new packet this is reset to the payload size
    -------------------------------------------------------------------------*/
    return_ptr     =  log_ptr;
    log_ptr        += LOG_HDR_SIZE;
    dest_ptr       =  log_ptr + DPL_LOG_HDR_SIZE;
    dest_room_left =  MAX_PAYLOAD;

    /*-----------------------------------------------------------------------
      Create log header
    -----------------------------------------------------------------------*/
    /**/
    DPLI_BUILD_LOG_PACKET_HDR(
      log_ptr,
      dpl_id,
      log_flag_link_cb->seq_num[DPL_IID_LINKPROT_PPP_IN_HDLC],
      seg_num );
    /* log ptr is now pointing after the DPL header */

    /**/

    /*-----------------------------------------------------------------------
      Run through Data creating one log packet (segment) at a time
    -----------------------------------------------------------------------*/
    while (dest_room_left)
    {
      /*---------------------------------------------------------------------
        Run through Data in the input DSM
      ---------------------------------------------------------------------*/
      while (src_ptr != src_end_ptr && dest_room_left)
      {
        if (*src_ptr == 0x7E)
        {
          /* 7E indicates the start of a packet.  There could be multiple
             7Es back to back, in which case stack them all at the start of
             the current packet.  If there is any data between 7E's, then the
             7E delineates the next packet.

             Hence, if we  */
          if (pbytes_logged <= 1)
          {
            /* All 7E's in a row are considered one byte */
            pbytes_logged = 1;

            /* Copy to the data log */
            *dest_ptr = *src_ptr;
            dest_ptr++;
            dest_room_left--;
          }
          else
          {
            /*---------------------------------------------------------------
              a 7E has been encountered after non 7E data.
              - shorten log packet,
              - reset last 2 bytes of segnum to include finish bit (0x8000)
            ---------------------------------------------------------------*/
            log_shorten( return_ptr,
                         DPL_TOTAL_HDR_SIZE + MAX_PAYLOAD - dest_room_left );
            dest_room_left = 0;

            (void) hput16(log_ptr - 2, seg_num | DPLI_SEGNUM_FINAL_BIT);

            /* a new ppp packet is starting so increment sequence number
                                                and byte count */
            log_flag_link_cb->seq_num[DPL_IID_LINKPROT_PPP_IN_HDLC]++;
            seg_num = ONE_LESS_THAN_ZERO;
            pbytes_logged = 0;
            break; /* leave iterating throgh DSM to send log packet */
          }
        }
        /* Check if we have reached the snaplength */
        else if( (pbytes_logged == snaplen) &&
                 (pbytes_logged != DPL_LOG_ALLOC_FAILED) )
        {
          /*-----------------------------------------------------------------
            Snaplen characters have been encountered
            - shorten log packet,
            - reset last 2 bytes of segnum to include finish bit (0x8000)
          -----------------------------------------------------------------*/
          log_shorten( return_ptr,
                       DPL_TOTAL_HDR_SIZE + MAX_PAYLOAD - dest_room_left );
          dest_room_left = 0;

          (void) hput16(log_ptr - 2, seg_num | DPLI_SEGNUM_FINAL_BIT);

          /* a new ppp packet is starting so increment sequence number
                                                and byte count */
          log_flag_link_cb->seq_num[DPL_IID_LINKPROT_PPP_IN_HDLC]++;
          seg_num = ONE_LESS_THAN_ZERO;
          break; /* leave iterating throgh DSM to send log packet */
        }
        /* If bytes are to be logged still, log this non 7E byte */
        else if( (pbytes_logged < snaplen) &&
                 (pbytes_logged != DPL_LOG_ALLOC_FAILED) )
        {
          /* Copy to the data log */
          *dest_ptr = *src_ptr;
          dest_ptr++;
          dest_room_left--;
          pbytes_logged++;
        }
        /* Move to next element in the DSM */
        src_ptr++;
      }

      /* Check if we got to the end of the current input DSM item */
      if (src_ptr == src_end_ptr)
      {
        /* Go to next DSM item */
        item_ptr = item_ptr->pkt_ptr;
        if (item_ptr == 0)
        {
           /* Send Packet if there is nothing more to read */
           /* shorten  log packet */
           log_shorten( return_ptr,
                        DPL_TOTAL_HDR_SIZE + MAX_PAYLOAD - dest_room_left );
           dest_room_left = 0;
           break;
        }
        src_ptr      = item_ptr->data_ptr;
        src_end_ptr  = src_ptr + item_ptr->used;
      }
    }

    /*-----------------------------------------------------------------------
      Dispatch the log packet containing the current segment, then
      increment the segment number count (accounting for wraparound) in
      preparation for the next segment.
    -----------------------------------------------------------------------*/
    log_commit (return_ptr);
    seg_num++;
    seg_num &= 0x7FFF;
  }

  /*-------------------------------------------------------------------------
    Run out of data. Need to save state to the control block
  -------------------------------------------------------------------------*/
  log_flag_link_cb->seg_num[DPL_IID_LINKPROT_PPP_IN_HDLC] = seg_num;
  log_flag_link_cb->partial_bytes_logged                  = pbytes_logged;

} /* dpli_log_ppp_partial_packet() */


