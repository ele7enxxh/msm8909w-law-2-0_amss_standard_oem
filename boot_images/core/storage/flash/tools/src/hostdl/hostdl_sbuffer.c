
/*==================================================================
 *
 * FILE:        hostdl_packet.h
 *
 * SERVICES:    None
 *
 * DESCRIPTION:
 *   This file contains the packet buffering layer necessary between the
 *   packet layer and the NAND layer, as the NAND flash must be written
 *   without any gaps in the pages, since we cannot go back and fill them
 *   in later like can be done with NOR flash.  If a gap is detected, this
 *   layer makes a place for the data which would fill the gap and begins
 *   buffering every packet it receives until it receives the packet which
 *   fills the gap.  It will then write everything buffered to the NAND
 *   flash until it either encounters a subsequent gap, or it runs out
 *   of buffered data.
 *
 * Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_sbuffer.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 08/12/10     sv      Added unframed stream write command
 * 04/23/09     sv      Modified for Ehostdl Support
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "hostdl.h"
#include "hostdl_config.h"
#include "hostdl_packet.h"
#include "hostdl_sbuffer.h"
#include "hostdl_log.h"
#include "string.h"

/*============================================================================
                  HIGH LEVEL DESIGN DESCRIPTION FOR MODULE

  This module utilizes three major data structures:

    retransmit buffer - contains data being held until gaps are filled
    free list -  contains empty packets to be used to keep track of gaps
    waiting list - contains packets being held awaiting a gap to be filled

  The basic logic is as follows:

    When a gap is detected
      copy the data of the packet to the retransmit buffer
                     or
      update retransmit buffer pointers to account for a gap
      create a packet on the waiting list representing the gap

 Packets on the Gap Info list are added in strictly ascending address order.
 If a packet of data is received that fills a gap partially, the gap info
 packet representing that gap is split into multiple packets some of which
 represent gaps, some of which represent data received.

 Once a gap is detected, we must buffer data until the first gap is finally
 fulfilled by receiving its data.

 When a packet of data is received that fills the first gap in the list, we
 write that packet, remove its gap info from the list, and then write as many
 packets as we can until we reach either a gap, or the end of buffered data.

 The function rt_buffer() puts data in the retransmit buffer and is the only
 function which updates the tail pointer in the circular retransmit buffer.
 The function rt_unbuffer takes data out of the retransmi buffer and is the
 only function which updates the head pointer.

 The function add_gi_packet() is the only function to add gap info packets on
 the list.  The function free_gi_packet() is the only function that removes
 gap info packets from the list.  When a packet is removed from the gap info
 list, its next pointer is no longer valid, but the data within the packet
 has not yet been changed, so is still available for use until that packet
 is re-used later on.

============================================================================*/

/*--------------------------------------------------------------------------
 * Type Definitions
 *------------------------------------------------------------------------*/

/* Enumerated type for whether a Gap Info packet is a gap, data, or not
 * being used at all.
 */
typedef enum
{
  GAP,                        /* packet represents a gap */
  DATA,                       /* packet represents data  */
  EMPTY                       /* packet is not in use    */
}
ghostdl_type;

/* Enumerated type to tell rt_buffer whether to copy data or not.  Sometime
 * we call rt_buffer to reserve a gap in the buffer, and in that case,
 * there is nothing to copy, merely update pointers.
 */
typedef enum
{
  COPY,                       /* rt_buffer should copy data          */
  NOCOPY                      /* should not copy data, this is a gap */
}
copy_type;

/*--------------------------------------------------------------------------
 * Externs
 *------------------------------------------------------------------------*/

extern int hostdl_write_buffer_to_flash(uint8 * data, uint32 addr, uint32 len);
/*--------------------------------------------------------------------------
 * Data Definitions
 *------------------------------------------------------------------------*/

/* Lists of gap info structures and control pointers */
struct packet_gap
{
  struct packet_gap *next;      /* pointer to next packet                 */
  uint8 *buf_ptr;               /* index into buffer where data should go */
  uint32 image_addr;            /* address in image of beginning of gap   */
  uint32 length;                /* length of gap                          */
  ghostdl_type type;            /* contains gap or data?                  */
};

typedef struct packet_gap *packet_ghostdl_t;

/*--------------------------------------------------------------------------
 * Forward Function Declarations
 *------------------------------------------------------------------------*/
static packet_ghostdl_t free_gi_packet (uint32 addr);
static int add_gi_packet (uint8 * buf, uint32 addr, uint32 length);
static uint8 *rt_where (uint32 addr, uint32 length);
static uint8 *rt_buffer (uint8 * data, uint32 addr, uint32 len, copy_type copy);
static int rt_unbuffer (uint8 * data, uint32 addr, uint32 len);
static int buffer_packet (uint8 * data, uint32 addr, uint32 length);
static int write_to_next_gap (uint8 * data, uint32 addr, uint32 length);
static int stream_buffer_write (uint8 * data, uint32 addr, uint32 len);

/*--------------------------------------------------------------------------
 * Local data
 *------------------------------------------------------------------------*/

/* Circular buffer to hold data while waiting for a dropped packet to be
 * re-transmitted.  This buffer is big enough to hold all the data
 * that is allowed to be in the sliding window.  Each packet will be a
 * linear array of data in strictly increasing addresses.  Since all packets
 * except the very last one are exactly UNFRAMED_MAX_DATA_LENGTH long, we will
 * always fill the buffer exactly to the end when it is time to wrap
 * around.  There is room for additional packets to allow the full
 * sliding window to be buffered with some space left over.
 */
static uint8 retrans_buf[(UNFRAMED_MAX_DATA_LENGTH * MAX_GAPS)];

/* Controls for retransmit buffer */
static uint8 *rt_head_ptr;      /* pointer to first packet                */
static uint8 *rt_tail_ptr;      /* pointer to last packet                 */
static int rt_count;            /* count of packets in buffer             */
static uint32 rt_base_addr;     /* base address of first packet in buffer */
static uint32 rt_tail_addr;     /* image address at tail pointer          */
static uint8 *rt_last_byte;     /* address of last byte of buffer         */

/* Array of packet_gaps to use.  This array must contain at least
 * the number of elements that is the window size.  Attempting to
 * add a new gap to the list and running out of gap packets indicates
 * that the host has caused a window overrun.  Window overrun is also
 * detected in the packet layer above, but we must also detect it
 * here since we are buffering.  We also have a pad factor built in,
 * so see the definition of MAX_GAPS for pad factor.  Packet layer will see
 * a window overrun before we use up the packets here, but we still must have
 * logic to catch not having any more buffering capability in this layer.
 */
static struct packet_gap ghostdl_info[MAX_GAPS];
int gi_size = sizeof (ghostdl_info) / sizeof (struct packet_gap);

/* Lists of packet gaps */
static packet_ghostdl_t gi_free_list;     /* Free list for waiting packets    */
static packet_ghostdl_t gi_waiting;       /* Head of list of waiting packets  */
static packet_ghostdl_t gi_waiting_tail;  /* End of list of waiting packets   */
static int gi_count;

/* Data to allow us to determine when we have missed a packet */
static uint32 next_image_addr;        /* Next address we expect to recv   */
static uint32 last_image_addr;        /* Highest address we have recv'd   */

/* Data to allow us to determine is a packet is a duplicate which we have
 * already received and written, but are receiving once again because the
 * ACK to the host was lost.
 */
static uint32 last_addr_written;      /* Highest address we have written  */
                                      /*   to the actual NAND flash       */

/*============================================================================

  FUNCTIONS

============================================================================*/

/*===========================================================================

  This function initializes all stream buffer data structures.  Must be called
  before calling public entry point stream_buffer().

===========================================================================*/
void sbuffer_init (void)
{
  int i;

  /* Dummy operation to make RVCT2.1 happy */
  gi_waiting_tail = (packet_ghostdl_t) gi_waiting_tail;

  FLASHLOG(1, ("sbuffer_init called\n"));

  KICK_WATCHDOG ();

  /* Initialize for buffering to wait for lost packets */
  ghostdl_info[0].next = NULL;

  /* Initialize free list. */
  for (i = 1; i < MAX_GAPS; i++)
  {
    ghostdl_info[i].next = &ghostdl_info[i - 1];
    ghostdl_info[i].buf_ptr = NULL;
    ghostdl_info[i].length = 0;
    ghostdl_info[i].image_addr = 0;
    ghostdl_info[i].type = EMPTY;
  }

  gi_free_list = &ghostdl_info[MAX_GAPS - 1];
  gi_waiting = NULL;
  gi_waiting_tail = NULL;
  gi_count = 0;

  rt_tail_ptr = retrans_buf;
  rt_head_ptr = retrans_buf;
  rt_count = 0;
  rt_base_addr = 0;
  rt_tail_addr = 0;

  /* Address of last byte in buffer to use to see if we are about
   * to wrap around
   */
  rt_last_byte = (uint8 *) (&retrans_buf[0] + (sizeof (retrans_buf)));

  next_image_addr = 0;
  last_image_addr = 0;
  last_addr_written = 0;

  FLASHLOG(1, ("rt begin 0x%x  rt end 0x%x\n", rt_head_ptr, rt_last_byte));
  FLASHLOG(1, ("Sbuffer Init completed\n\n"));


  KICK_WATCHDOG ();

}

/*===========================================================================

  This function copies the data from the stream buffer

===========================================================================*/
static int stream_buffer_write (uint8 * data, uint32 addr, uint32 len)
{
  CHECK_FOR_DATA ();

  /*
      Stuff the jtag variables with values that TRACE32 would have used to
      indicate to JNAND that a write is to take place.
  */

  if (TRUE != hostdl_write_buffer_to_flash(data,addr,len))
  {
    FLASHLOG(1,("sbw: failed\n"));
    return FALSE;
  }

  /* Since we successfully wrote this, update the variable which allows
   * us to determine if we are getting duplicate packets due to lost
   * ACKS to the host on successfully written packets.
   */
  last_addr_written = addr;

  return TRUE;
}



/*===========================================================================

  This function handles buffering of data for inclusion of lost packets
  as necessary.  If no lost packets, it merely asks for the data to be
  written.  Once a lost packet is received, this function will flush
  any contiguous data from the gap buffer by requesting it to be written
  to flash.

  It will also enforce the rule that the only packets which are valid when
  received are in ascending address order, unless they are packets which
  were lost and are being retransmitted, or packets which are being
  re-transmitted because an ACK to the host was lost.

  If any packet is received which has an address less than the previous
  packet and we do not have a valid "gap" in the buffer waiting for it, an
  ACK will be generated to the host.  This is making the assumption that if
  we get a packet that we have already written, it is because the ACK we
  sent to the host was lost.  In that case, the host will send the packet
  again, and all we have to do is recognize that we already wrote a packet
  in that address range, and send the ACK.

===========================================================================*/
int stream_buffer (uint8 * data, uint32 addr, uint32 len)
{
  FLASHLOG(6, ("  sb:   data 0x%08x  addr 0x%08x len 0x%08x  \n",
               (unsigned int) data, (unsigned int) addr, (unsigned int) len));
  FLASHLOG(1,("sb:0x%08x ", (unsigned int) addr));

  CHECK_FOR_DATA ();

  if (!gi_waiting)
  {
    /* nothing is buffered */
    if (next_image_addr != addr)
    {
      if (addr <= last_addr_written)
      {
        /* We have detected a duplicate packet sent due to lost ACK to host.
         * All we have to do is ACK it, as we have already written it.
         * Returning TRUE will cause the packet layer to send the ACK.
         */
        return TRUE;
      }
      else
      {
        /* We have a gap, must buffer this */
        FLASHLOG(6, ("sb:  calling buffer_packet\n"));
        if (!buffer_packet (data, addr, len))
        {
          FLASHLOG(1,("sb:  could not buffer\n"));
          return FALSE;
        }
      }
    }
    else
    {
      /* no gap, just write it */
      if (!stream_buffer_write (data, addr, len))
        return FALSE;
      next_image_addr = addr + len;
      FLASHLOG(6, ("sb:  no gap, write - update next_image_addr to 0x%x\n",
                next_image_addr));
    }
  }
  else
  { /* Something is buffered */

    /* Must first check for a packet less than that we currently
     * have buffered, and if so, it represents a packet we already
     * have received.
     */
    if (addr < gi_waiting->image_addr)
    {
      /* We have detected a duplicate packet sent due to lost ACK to host.
       * All we have to do is ACK it, as we have already written it.
       * Returning TRUE will cause the packet layer to send the ACK.
       */
      return TRUE;
    }

    /* Does this packet fill the first gap? */
    if (addr == gi_waiting->image_addr)
    {
      FLASHLOG(6, ("  sb: unbuffering\n"));
      /* write packet and all packets to next gap */
      if (!write_to_next_gap (data, addr, len))
      {
        FLASHLOG(1,("sb:  wng failed\n"));
        return FALSE;
      }
    }
    else
    {
      FLASHLOG(6, ("  sb: buffering\n"));
      /* Buffer packet because it does not fill first gap */
      if (!buffer_packet (data, addr, len))
      {
        FLASHLOG(1,("sb:  could not buffer\n"));
        return FALSE;
      }

      if (addr >= next_image_addr)
      {
        next_image_addr = addr + len;
        FLASHLOG(6, ("sb:  not first gap - updating next_image_addr to 0x%x\n",
                  next_image_addr));
      }
    }
  }

  /* If we just either added to the end of the buffer, or if
   * we buffered a packet that is at the end of the buffer,
   * update address pointer that allow us to tell if we have a gap
   * in the following data packets received.  Don't update this if
   * the packet filled a gap.
   */
  if (addr > last_image_addr)
  {
    last_image_addr = addr;
  }

  return TRUE;
}

/*===========================================================================

  This function will dump the gap info list

===========================================================================*/
void dump_gi_list (void)
{
  packet_ghostdl_t this;

  /* nothing on list */
  if (gi_waiting == 0)
  {
    FLASHLOG(1,("Gap list is empty\n"));
    return;
  }

  FLASHLOG(1,("\nDumping GI list\n"));
  this = gi_waiting;
  while (this != NULL)
  {
    FLASHLOG(1,("gap @ 0x%x addr 0x%x len 0x%x next 0x%x\n",
              (int) this, (int) this->image_addr, (int) this->length,
              (int) this->next));
    this = this->next;
  }
  FLASHLOG(6, ("\n"));
}

/*===========================================================================
  This function will find the packet on the gap info list using the address
  and free it by moving entry to free list.  Even after the packet is freed,
  it's data is still available to be used until it is reused.  It is the
  caller's resposibility to zero out the packet data (image_addr, length,
  type) as necessary.  The caller must capture the "next" pointer before
  calling this function as the act of putting the packet back on the free
  list destroys the old "next" pointer.

===========================================================================*/

static packet_ghostdl_t free_gi_packet (uint32 addr)
{
  packet_ghostdl_t this;
  packet_ghostdl_t next, prev;
  packet_ghostdl_t tmp;

  CHECK_FOR_DATA ();

  /* nothing to free, we should have not been called */
  if (gi_waiting == 0)
  {
    return NULL;
  }

  next = prev; /* Dummy operation to make RVCT2.1 happy */

  this = gi_waiting;

  /* First try the simple case of first packet before looping */
  if (this->image_addr == addr)
  {
    /* take packet off waiting list */
    gi_waiting = this->next;
    FLASHLOG(6, ("free_gi_packet: removed packet - gi_waiting 0x%x\n",
         gi_waiting));
    if (gi_waiting == NULL)
      gi_waiting_tail = gi_waiting;

    /* add packet to free list */
    this->next = gi_free_list;
    gi_free_list = this;
    gi_count--;
    FLASHLOG(6, ("free_gi_packet: gi_count 0x%x\n", gi_count));
    return this;
  }
  else
  {
    /* set up to loop over the packet list searching for address match */
    tmp = this->next;
    prev = this;
    this = tmp;
    next = this->next;;
  }

  /* Loop, attempting to find the packet in the waiting list */
  while (this && (this->image_addr != addr))
  {
    tmp = next->next;
    prev = this;
    this = next;
    next = tmp;
  }

  /* Now "this" is either at the end of the list, or is pointing
   * to the packet we want to remove from the free list
   */
  if (this == NULL)
  {
    /* we did not find the packet */
    return NULL;
  }
  else
  {
    if (this->next == NULL)
    {
      /* Packet is at the end of the list */
      prev = NULL;
    }
    else
    {
      /* packet is in the middle of the list */
      prev = this->next;
    }
  }

  /* add packet to free list */
  this->next = gi_free_list;
  gi_free_list = this;
  gi_count--;
  FLASHLOG(6, ("free_gi_packet: gi_count 0x%x\n", gi_count));

  /* if we emptied the list, update the tail pointer */
  if (gi_waiting == NULL)
    gi_waiting_tail = gi_waiting;

  return this;
}

/*===========================================================================

  This function will take a packet from the free list and add it
  to the waiting list in ascending address order.

  When adding a packet that represents data, we must create packets
  to represents any gaps to either side of the data and mark the data
  as data and the gaps as gaps in the "type" field of the structure.

===========================================================================*/
static int add_gi_packet (uint8 * buf, uint32 addr, uint32 length)
{
  packet_ghostdl_t this, this2;
  packet_ghostdl_t next, prev;
  packet_ghostdl_t tmp;
  int old_len;
  uint8 *dest2;
  uint32 ghostdl_addr;
  uint32 ghostdl_length;

  FLASHLOG(7, ("agp:  buf 0x%x  addr 0x%x  len 0x%x\n",
            (int) buf, (int) addr, (int) length));

  CHECK_FOR_DATA ();

  /* Error out if no free packets. */
  if (gi_free_list == NULL)
  {
    FLASHLOG(1,("add_gi_packet:  no free packets\n"));
    return FALSE;
  }

  /* Handle simple case of no packets waiting, add to end of list.
   * This ends up handling the special case of the first packet,
   * because we have to add a packet representing a gap, once we
   * get the first packet that is not in order.
   */
  if (gi_waiting == 0)
  {
    this = gi_free_list;
    gi_free_list = gi_free_list->next;
    gi_waiting = this;
    this->next = NULL;
    FLASHLOG(6, ("add_gi_packet: last packet this 0x%x next to NULL\n", this));
    gi_waiting_tail = this;

    /* Store the passed data in the packet */
    this->buf_ptr = buf;
    this->image_addr = addr;
    this->length = length;
    this->type = GAP;
    gi_count++;
    FLASHLOG(6, ("add_gi_packet 1: gi_count 0x%x\n\n", gi_count));
    return TRUE;
  }

  /* Insert the packet in the waiting list in increasing address order.
   * If necessary, split up a packet into multiple packets.  This happens
   * when we get a packet that partially fills a gap, but does not
   * fill it completely. If we get here, the packet is always data,
   * not a new gap.  However, the image address of the data may suggest
   * that we have another gap, in which case, we must create another
   * gap packet.
   */
  prev = gi_waiting;
  next = gi_waiting->next;
  while ((next != NULL) && (addr > next->image_addr))
  {
    tmp = next->next;
    prev = next;
    next = tmp;
  }

  if ((next == NULL))
  {
    /* Add packet to end of list.  If there is a gap between prev
     * and this packet, must add gap packet to account for gap.
     */
    if ((prev->image_addr + prev->length) == addr)
    {
      /* No extra gap to account for. */
      this = gi_free_list;
      gi_free_list = gi_free_list->next;
      prev->next = this;
      this->next = NULL;
      FLASHLOG(6, ("add_gi_packet: last packet this 0x%x next to NULL\n", this));
      this->buf_ptr = buf;
      this->image_addr = addr;
      this->length = length;
      this->type = DATA;
      gi_count++;
      FLASHLOG(6,("add_gi_packet 2: gi_count 0x%x\n\n", gi_count));
    }
    else
    {
      /* Must add a packet for the gap. */
      ghostdl_addr = prev->image_addr + prev->length;
      ghostdl_length = addr - ghostdl_addr;
      dest2 = rt_where (ghostdl_addr, ghostdl_length);
      if (dest2 == NULL)
      {
        FLASHLOG(6, ("add_gi_packet:  dest2 not in rt buffer\n"));
        return FALSE;
      }
      this2 = gi_free_list;
      gi_free_list = gi_free_list->next;
      this = gi_free_list;
      gi_free_list = gi_free_list->next;

      /* prev -> this2  -> this  (where this2 represents gap and this
       * represents data beyond gap)
       */
      prev->next = this2;
      this2->next = this;
      this->next = NULL;

      this2->buf_ptr = dest2;
      this2->image_addr = ghostdl_addr;
      this2->length = ghostdl_length;
      this2->type = GAP;
      gi_count++;
      FLASHLOG(6, ("add_gi_packet 3: gi_count 0x%x\n\n", gi_count));

      this->buf_ptr = buf;
      this->image_addr = addr;
      this->length = length;
      this->type = DATA;
      gi_count++;
      FLASHLOG(6, ("add_gi_packet 4: gi_count 0x%x\n\n", gi_count));
    }
  }
  else if (next->image_addr == addr)
  {
    /* Insert packet in middle of list.
     * We already have this packet, it is probably because
     * it was formerly a gap and now it is data. If the current
     * packet does not fill the gap totally, we may need to split
     * it into more than one packet.
     */
    if (next->type == DATA)
    {
      /* We already have this data, it is a bogus request */
      FLASHLOG(6, ("add_gi_packet:  duplicate data\n"));
      return FALSE;
    }

    /* This is data actually filling all or part of a gap,
     * need to determine which, and whether we split a packet.
     */
    if (next->length < length)
    {
      /* We have an error because we are trying to put data in
       * a gap which is too small for it.
       */
      FLASHLOG(6, ("add_gi_packet: gap is too small for data\n"));
      return FALSE;
    }

    if (next->length == length)
    {
      /* Fills gap exactly, just modify packet */
      next->buf_ptr = buf;
      next->image_addr = addr;
      next->length = length;
      next->type = DATA;
      /* IMPORTANT: We are filling a gap packet but not incrementing
              the gi_count, where as, every rt_buffer transaction
              increments the rt_count,so decrement the rt_count here... */
      rt_count--;
    }
    else
    {
      /* Fills gap partially at the beginning of a gap packet,
       * need to add a packet, and split the found packet
       * into two packets.
       */
      this = gi_free_list;
      gi_free_list = gi_free_list->next;

      prev->next = this;
      this->next = next;
      next->length -= length;
      next->image_addr +=length;
      this->buf_ptr = buf;
      this->image_addr = addr;
      this->length = length;
      this->type = DATA;
      gi_count++;
      FLASHLOG(6, ("add_gi_packet 5: gi_count 0x%x\n\n", gi_count));
    }
  }
  else
  {
    /* This packet either goes after previous, or fills part of
     * previous gap.
     */

    /* Packet is somewhere within previous packet range */
    if (prev->type == DATA)
    {
      /* Invalid to insert in middle of data packet */
      FLASHLOG(1,("add_gi_packet:  insert in middle of data packet\n"));
      return FALSE;
    }

    if ((prev->image_addr + prev->length) > (addr + length))
      {
      /* Packet is in middle of previous, we have to split
       * previous into three packets - gap, data, gap.  Prev packet
       * is already GAP, so we do not need to change type of field.
       */
      this = gi_free_list;
      gi_free_list = gi_free_list->next;
      this2 = gi_free_list;
      gi_free_list = gi_free_list->next;

      old_len = prev->length;
      prev->length = addr - prev->image_addr;
      this2->length = old_len - (prev->length + length);

      prev->next = this;
      this->next = this2;
      this2->next = next;

      this->buf_ptr = buf;
      this->image_addr = addr;
      this->length = length;
      this->type = DATA;
      gi_count++;
      FLASHLOG(6, ("add_gi_packet 6: gi_count 0x%x\n\n", gi_count));

      this2->image_addr = this->image_addr + this->length;
      dest2 = rt_where (this2->image_addr, this2->length);
      if (dest2 == NULL)
        {
        FLASHLOG(1,("add_gi_packet:  not in rt buffer\n"));
        return FALSE;
      }
      this2->buf_ptr = dest2;
      this2->type = GAP;
      gi_count++;
      FLASHLOG(6, ("add_gi_packet 7: gi_count 0x%x\n\n", gi_count));
    }
    else
    {
      /* Packet is at the end of previous packet, split
       * into two packets.
       */
      this = gi_free_list;
      gi_free_list = gi_free_list->next;

      prev->next = this;
      this->next = next;
      prev->length -= length;
      this->buf_ptr = buf;
      this->image_addr = addr;
      this->length = length;
      this->type = DATA;
      gi_count++;
      FLASHLOG(6, ("add_gi_packet 8: gi_count 0x%x\n\n", gi_count));
    }
  }
  return TRUE;
}

/*===========================================================================

  This function will find the place in the retransmit buffer where a
  packet should go and return a pointer to it.  It is called when we
  have to split up a packet into three packets, and we have to obtain
  a pointer for the data for the third packet.

===========================================================================*/
static uint8 * rt_where (uint32 addr, uint32 length)
{
  uint8 *dest_ptr;
  int head_offset;
  int end_offset;
  int begin_offset;

  CHECK_FOR_DATA ();

  /* It is an error if buffer is full */
  if ((rt_count >= MAX_GAPS) ||
      ((rt_head_ptr == rt_tail_ptr) && (rt_count != 0)))
    return NULL;

  /* No matter where the data goes, the logic for finding where in
   * the buffer it belongs is the same.
   * Look from the current position forward to find where to put
   * this packet. The variable rt_head_ptr points to data that has an image
   * address that is rt_base_addr.
   */
  head_offset = addr - rt_base_addr;
  if ((rt_head_ptr + head_offset) >= rt_last_byte)
  {
    /* Destination for data wraps */
    end_offset = (int) &rt_last_byte - (int) &rt_head_ptr;
    begin_offset = head_offset - end_offset;
    dest_ptr = &retrans_buf[0] + begin_offset;

    /* Check to see if we will go beyond head ptr */
    if ((dest_ptr + length) >= rt_head_ptr)
      return NULL;
  }
  else
  {
    /* Destination for data does not wrap */
    dest_ptr = &rt_head_ptr[0] + head_offset;

    /* Check to see if we will go beyond head ptr */
    if ((dest_ptr + length) > rt_last_byte)
      return NULL;
  }
  /* Even though we did not add any data, we must increment rt_count,
   * because eventually, we will pull this packet out, and decrement
   * rt_count while taking it out.  This is based on the assumption
   * that this function is called when we received data that causes us
   * to split a gap packet and need to find the address of the gap.
   */
  rt_count++;
  FLASHLOG(6, ("rt_where: rt_count 0x%x\n\n", rt_count));

  return dest_ptr;
}

/*===========================================================================

  This function will remove the packet from the retransmit buffer.
  This is done by merely manipulating pointers, as the data does not really
  need to be "removed".  The packet must be at the head of the retransmit
  buffer, as that is the only place from which we are allowed to remove
  a packet.

===========================================================================*/
static
int rt_unbuffer (uint8 * data, uint32 addr, uint32 len)
{
  int end_offset;

  CHECK_FOR_DATA ();

  /* Bail out on any bogus conditions */
  if (rt_count == 0)
  {
    FLASHLOG(1,("rt_unbuffer:  buffer empty\n"));
    return FALSE;
  }

  if (rt_head_ptr != data)
  {
    FLASHLOG(1,("rt_unbuffer:  data 0x%x not at head ptr 0x %x\n", data,
              rt_head_ptr));
    return FALSE;
  }

  if (rt_base_addr != addr)
  {
    FLASHLOG(1,("rt_unbuffer:  addr 0x%x not at rt_base_addr 0x%x\n",
              addr, rt_base_addr));
    return FALSE;
  }

  /* Make sure we can remove whole length without wrapping */
  end_offset = (int) rt_last_byte - (int) rt_head_ptr;
  if ((int)len > end_offset)
  {
    FLASHLOG(1,("rt_unbuffer:  wrap - rt_head 0x%x   rt_tail  0x%x  eo 0x%x\n",
              rt_head_ptr, rt_tail_ptr, end_offset));
    return FALSE;
  }

  FLASHLOG(5, ("rt_unbuffer: addr 0x%x from buf 0x%x len 0x%x\n",
               addr, rt_head_ptr, len));

  rt_head_ptr += len;
  if (rt_head_ptr == rt_last_byte)
  {
    rt_head_ptr = &retrans_buf[0];
  }
  rt_count--;
  FLASHLOG(5, ("rt_unbuffer:  dec rt_count to 0x%x\n", rt_count));
  if (rt_count == 0)
  {
    FLASHLOG(6, ("\n\nrt_unbuffer:  rt_count went to 0, gi_waiting is 0x%x\n",
        gi_waiting));
    FLASHLOG(6, ("\n\n"));
  }
  rt_base_addr = addr + len;

  FLASHLOG(5, ("rt_unbuffer:  update rt_base_addr to 0x%x\n", rt_base_addr));

  return TRUE;
}


/*===========================================================================

  This function will place the packet in the retransmit buffer.  The
  data from any individual packet will be guaranteed to not wrap from
  the end of the buffer to the beginning, as we have a buffer which
  contains an exact multiple of the packet size.  The only packet which
  is ever NOT exactly the packet size is the packet containing the last
  chunk of data in the image.  This is not necessarily the last packet,
  but will be the last packet in the circular buffer if we dropped a
  packet earlier which comes in after the "last packet".

===========================================================================*/
static uint8 * rt_buffer (uint8 * data, uint32 addr, uint32 length,
              copy_type copy)
{
  uint8 *dest_ptr;
  int head_offset;
  int end_offset;
  int begin_offset;
  int tail_offset;

  CHECK_FOR_DATA ();

  /* It is an error if buffer is full */
  if ((rt_count >= MAX_GAPS) ||
      ((rt_head_ptr == rt_tail_ptr) && (rt_count != 0)))
  {
    FLASHLOG(1,("rt_buffer:  buffer full\n"));
    return NULL;
  }

  /* If this is the first packet being buffered, save away the
   * image address of the first byte of data that should be in the
   * buffer.  It may not be the address of this packet or
   * the packet immediately preceding this one.  The rt_base_addr
   * allows us to later calculate where in the buffer data in the
   * middle of the saved image belongs.
   */
  if (rt_count == 0)
  {
    FLASHLOG(6, ("rt_buffer:  update rt_base_addr to next_image_addr 0x%x\n",
              next_image_addr));
    rt_base_addr = next_image_addr;
  }

  /* No matter where the data goes, the logic for finding where in
   * the buffer it belongs is the same.
   * Look from the current position forward to find where to put
   * this packet. The variable rt_head_ptr points to data that has an image
   * address that is rt_base_addr.
   */
  head_offset = addr - rt_base_addr;
  if ((rt_head_ptr + head_offset) >= rt_last_byte)
  {
    /* Destination for data wraps */
    end_offset = (int)rt_last_byte - (int)rt_head_ptr;
    begin_offset = head_offset - end_offset;
    dest_ptr = &retrans_buf[0] + begin_offset;

    /* Check to see if we will go beyond head ptr */
    if ((dest_ptr + length) >= rt_head_ptr)
    {
      FLASHLOG(1,("rt_buffer:  beyond head ptr\n"));
      return NULL;
    }
  }
  else
  {
    /* Destination for data does not wrap */
    dest_ptr = rt_head_ptr + head_offset;

    /* Check to see if we will go beyond head ptr */
    if ((dest_ptr + length) > rt_last_byte)
    {
      FLASHLOG(1,("rt_buffer:  beyond head ptr 2\n"));
      FLASHLOG(1,("   rt_head_ptr 0x%x  head_offset  0x%x rt_base_addr 0x%x\n",
                rt_head_ptr, head_offset, rt_base_addr));
      FLASHLOG(1,("   rt_tail_ptr 0x%x  rt_buf @ 0x%x rt_last_byte 0x%x\n",
                rt_tail_ptr, &retrans_buf[0], rt_last_byte));
      return NULL;
    }
  }

  /* Update the tail pointer if this address is larger than the
   * address previously stored at tail pointer, otherwise, we
   * will be putting data between head and tail, so no update
   * is necesary.
   */
  tail_offset = (addr + length) - rt_tail_addr;
  if (addr > rt_tail_addr)
  {
    rt_tail_addr = addr;
    if ((rt_tail_ptr + tail_offset) > rt_last_byte)
    {
      /* Tail pointer must wrap */
      end_offset = (int)rt_last_byte - (int)rt_tail_ptr;
      begin_offset = tail_offset - end_offset;
      rt_tail_ptr = &retrans_buf[0] + begin_offset;
      FLASHLOG(5, ("rt_buffer:  tail pointer wrap to 0x%x\n", rt_tail_ptr));
    }
    else
    {
      /* Tail pointer does not wrap */
      rt_tail_ptr = dest_ptr + length;
      FLASHLOG(6, ("rt_buffer:  tail pointer update to 0x%x\n", rt_tail_ptr));
    }
  }

  FLASHLOG(5, ("rt_buffer: addr 0x%x to buf 0x%x len 0x%x\n", addr, dest_ptr,
         length));

  /* Copy data to destination pointer found above, unless it is a gap.
   * Only the caller knows this, so it passes a parameter to tell us whether
   * or not to actually do the copy.
   */
  if (copy == COPY)
  {
    hostdl_memcpy (dest_ptr, data, length);
  }

  rt_count++;

  FLASHLOG(5, ("rt_buffer:  inc rt_count to 0x%x\n", rt_count));

  return dest_ptr;
}

/*===========================================================================

  This function will call functions to add a packet to the gap info list
  and to add the actual data to the circular data buffer.

===========================================================================*/
static int
buffer_packet (uint8 * data, uint32 addr, uint32 length)
{
  int gi_result;
  uint8 *buf_result;
  uint32 ghostdl_length;

  CHECK_FOR_DATA ();

  /* First, handle special case of no gaps buffered.  In this case,
   * we must add a first dummy packet onto the list that represents
   * the gap, and then a packet which represents the packet we just
   * received.
   */

  if (!gi_waiting)
  {
    /* Need to add a gap in the retrans buffer */
    ghostdl_length = addr - next_image_addr;
    FLASHLOG(6, ("buffer_packet: call rt_buffer for gap at 0x%x\n",
             next_image_addr));
    buf_result = rt_buffer (0, next_image_addr, ghostdl_length, NOCOPY);
    if (buf_result)
    {
      /* The address in the buffer for this special first gap packet
       * is going to be rt_head_ptr
       */
      gi_result = add_gi_packet (rt_head_ptr, next_image_addr, ghostdl_length);
      if (!gi_result)
      {
        FLASHLOG(1,("buffer_packet:  agp failed\n"));
        return FALSE;
      }
    }
  }

  /* Now, add a packet for the data we just received */
  buf_result = rt_buffer (data, addr, length, COPY);
  if (buf_result)
  {
    gi_result = add_gi_packet (buf_result, addr, length);
    if (!gi_result)
    {
      FLASHLOG(1,("buffer_packet:  agp failed\n"));
      return FALSE;
    }
  }
  else
  {
    // FLASHLOG(1,("buffer_packet:  rt_buffer\n"));
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

  This function is called when it has been determined that the packet
  just received fills the first gap.  This function then writes this packet
  and all subsequent packets up to the end of the buffered data, or the
  next gap, whichever comes first.

===========================================================================*/
static int
write_to_next_gap (uint8 * data, uint32 addr, uint32 length)
{
  packet_ghostdl_t freed_packet, this, next;
  uint32 new_len;
  uint32 new_addr;
  uint8 *new_buf;
  int status;

  FLASHLOG(3, ("wng before\n"));

  CHECK_FOR_DATA ();

  /* Sanity checks.  The first packet must be a GAP, as the caller
   * determined that the address matches the first packet.  And it
   * must be at least as big as the length of the packet we are trying
   * to write.
   */
  if ((gi_waiting->type != GAP) || (gi_waiting->length < length))
  {
    return FALSE;
  }

  /* write the currently received packet */
  status = stream_buffer_write (data, addr, length);
  if (status == FALSE)
  {
    return FALSE;
  }

  /* The gap packet was freed, now need to free up the retransmit buffer
   * for the data we wrote.
   */
  status = rt_unbuffer (rt_head_ptr, addr, length);
  if (status == FALSE)
  {
    return FALSE;
  }

  /* write any remaining buffered packets to next gap or end of data */

  /* Maybe handle splitting the first gap packet. */
  if (gi_waiting->length > length)
  {
    /* Must shrink gap packet, we are done.  Do not modify
     * the packet until rt_where() returns a good pointer.
     */
    this = gi_waiting;
    new_len = this->length - length;
    new_addr = this->image_addr + length;
    new_buf = rt_where (new_addr, new_len);
    if (new_buf == NULL)
    {
      return FALSE;
    }

    /* Everything OK, actually shrink the gap packet. */
    this->length = new_len;
    this->image_addr = new_addr;
    this->buf_ptr = new_buf;
    return TRUE;
  }

  /* By getting this far, we know that we filled the first gap packet
   * exactly.  Remove it from the list.
   */
  freed_packet = free_gi_packet (addr);
  if (freed_packet == NULL)
  {
    return FALSE;
  }
  else
  {
    freed_packet->type = EMPTY;
  }
  FLASHLOG(3, ("wng:  freed packet addr 0x%x", freed_packet->image_addr));

  /* There may be some more data to write.  Write until the next gap
   * or the end of buffered data, whichever is first.
   */
  this = gi_waiting;
  next = this->next;
  while ((this != NULL) && (this->type == DATA))
  {
    status = stream_buffer_write (this->buf_ptr, this->image_addr,
                                  this->length);
    if (status == FALSE)
    {
      return FALSE;
    }

    /* Free the packet */
    freed_packet = free_gi_packet (this->image_addr);
    if (freed_packet == NULL)
    {
      return FALSE;
    }
    else
    {
      freed_packet->type = EMPTY;
    }

    /* The gap packet was freed, now need to free up the retransmit buffer
     * for the data we wrote
     */
    status = rt_unbuffer (this->buf_ptr, this->image_addr, this->length);
    if (status == FALSE)
    {
      return FALSE;
    }

    FLASHLOG(3, ("wng:  freed packet addr 0x%x", freed_packet->image_addr));

    /* Can't use this->next directly, as free_gi_packet re-arranged the
     * pointers on us.  So, we use the "next" variable to capture the
     * next pointer before we free the packet and then use the result
     * when we return from free_gi_packet.
     */
    this = next;
    if (this != NULL)
    {
      next = this->next;
    }
  } /* while */

  FLASHLOG(3, ("wng after\n"));
  FLASHLOG(3, ("wng exit\n"));
  return TRUE;
}
