/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ H D L C _ L I B . C


GENERAL DESCRIPTION
  The implementation of the HDLC library.

EXTERNALIZED FUNCTIONS

  hdlc_init_config()
    Initializes the config data structure with default values

  hdlc_alloc_instance()
    Allocates an instance for HDLC framing/unframing

  hdlc_free_instance()
    Resets and frees up the memory for the specified HDLC instance

  hdlc_reset()
    Reset the state of the HDLC instance.

  hdlc_errno()
    Returns the errno of the last operation on the specified HDLC instance

  hdlc_unframe()
    Unframes the passed DSM chain for the specified HDLC instance

  hdlc_frame()
    Frames the passed DSM chain for the specified HDLC instance

  hdlc_ioctl()
    Performs the specified operation on the specified HDLC instance

INITIALIZATION AND SEQUENCING REQUIREMENTS
    All functions in this library except hdlc_init_config() and
    hdlc_alloc_instance() need to take an instance handle -
    this is allocated in the hdlc_alloc_instance() call.
    An error will be returned if the handle is invalid.

Copyright (c)2006-2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_hdlc_lib.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/23/14    skc    Removing FEATURE_HDLC_HW_ACCEL, FEATURE_DATA_PS_HDLC_PP
10/30/13    cx     Fixed error when deframing the packet within two 
                   consecutive 7Ds  
10/21/13    vl     Removal of compiler warnings
10/25/12    op     Remove usage of fatal error log messages
05/18/12    ash    Reset last_byte_was_esc before returning unframed packet.
12/08/10    rp     RAM optimization changes.
12/15/10    sa     Including msm.h instead of msmhwiobase.h to avoid
                   CMI violation.
08/25/10    sch    Fixed hdlci_create_empty_dsm_item() to also account for data_pt
                   which is not word aligned.
08/04/10    rp     Added null check for frame_byte_ptr in 
                   hdlci_sw_frame function.
02/05/10    ss     Destroy the queue using q_destroy () when it is no longer 
                   needed to free up queue mutex.
08/31/09    mga    Merged from eHRPD branch
01/08/09    scb    Fixed QTF LINT errors.
12/19/08    scb    Added fix to ensure that s/w to h/w HDLC mode change
                   happens only on demand.
11/19/08    scb    Added P2 framer and deframer fixes.
11/12/08    scb    Added protocol processor specific functionality.
11/10/08    scb    Fixed QTF compilation warnings.
07/27/08    scb    Added a workaround for input buffer sizes of exactly 240
                   bytes for the HDLC hardware framer
07/28/08    scb    Added changes under FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED
05/04/08    scb    Added P2 framer deframer STA related functionality.
04/09/08    scb    Added fix to hdlc_ioctl to reset the ACCM value in the
                   instance for SET_MODE option.
12/19/07    scb    Added protocol processor framer and deframer functionality.
12/06/07    ifk    Added check for unaligned memory buffer in hdlci_hw_unframe()
09/27/07    ifk    Reworked HW framing and deframing code.
05/31/07    scb    Added a fix to hdlci_sw_frame to handle empty dsm chains
05/05/07    scb    Fixed High LINT errors
04/27/07    scb    Removed LINT suppressions
02/07/07    scb    Fixed Klocworks high errors
08/04/06    ssh    Fixed bug in hdlc_free_instance()
06/20/06    ssh    Cleanup.
06/07/06    ssh    Added hdlc_reset(), removed hdlc_unframe_reset()
04/20/06    ssh    Added new ioctls to switch HDLC mode HW <--> SW, get and
                   set unframer state. Miscellaneous changes.
04/10/06    ssh    Changed some LOG_MSG_INFO2s in data path to LOG_MSG_INFO3s
01/12/06    ssh    Changed hdlc_errno() return type to hdlc_errno_enum_type
11/15/05    ssh    Added support for software framing, review comments
10/20/05    ssh    Added support for hardware framing
10/07/05    ssh    Changes due to HDLC driver redesign
09/30/05    ssh    Review comments
09/20/05    ssh    Moved HDLC driver-specific code out, improved hardware
                   unframed packet handling, and some code cleanup.
09/14/05    ssh    Added new internal header file ps_hdlc_libi.h to includes,
                   and stubs for hdlc_frame() and hdlc_ioctl()
09/06/05    ssh    Replaced storage byte order dependent manipulation
                   in save and restore hardware state macros.
09/02/05    ssh    Added support for hardware acceleration, under
                   FEATURE_HDLC_HW_ACCEL
09/01/05    ssh    Now enforcing non-NULL config in hdlc_init_config, also
                   added default initialization of config values
05/19/05    ssh    Changed manipulation of the unframed packet chain in
                   hdlc_unframe() and hdlci_finalize_unframed_pkt(), removed
                   DUP'ing for a single 7E. Some other minor changes.
05/06/05    ks     Fixed return value in hdlc_unframe_reset().
05/05/05    ssh    Fixed hdlc_unframe_reset() and hdlc_errno().
11/05/04    mct    Fixed error where packet could be freed due to a small
                   edge condition.
08/24/04    mvl    Changed some message levels to reduce F3 clutter. Fix for
                   edge condition where FCS removal causes double free.
07/22/04    mvl    Fixed bug where packet was not being freed when a bad CRC
                   was found.
05/07/04    mvl    Now chaining by hand where we were doing dsm_append()
                   because of side-effect.
05/05/04    mvl    released into PS mainline.
03/24/04    mvl    created module.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ps_hdlc_lib.h"
#include "ps_hdlc_libi.h"
#include "ps_mem.h"
#include "crc.h"
#include "msg.h"
#include "amssassert.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Tuning the number of hdlc instance buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_HDLC_INSTANCE_BUF_SIZE  ((sizeof(hdlc_instance_type) + 3) & ~3)

/*---------------------------------------------------------------------------
  Macro to destroy the Pkt and framer queues 
----------------------------------------------------------------------------*/
#define PS_HDLC_INSTANCE_DESTROY_FRAMER_Q(hdlc_instance_ptr) 

#define PS_HDLC_INSTANCE_DESTROY_Q(hdlc_instance_ptr)   \
{                                                       \
  hdlc_instance_type *inst_ptr = (hdlc_instance_type *)hdlc_instance_ptr; \
  q_destroy(&(inst_ptr->unframed_pkt_q));   \
  PS_HDLC_INSTANCE_DESTROY_FRAMER_Q(inst_ptr); \
}

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define PS_HDLC_INSTANCE_BUF_NUM       40
#define PS_HDLC_INSTANCE_BUF_HIGH_WM   37
#define PS_HDLC_INSTANCE_BUF_LOW_WM     2

#else
/*---------------------------------------------------------------------------
  PS_HDLC_INSTANCE_BUF_NUM should be >= to number of PPP instances
---------------------------------------------------------------------------*/
#define PS_HDLC_INSTANCE_BUF_NUM       5
#define PS_HDLC_INSTANCE_BUF_HIGH_WM   4
#define PS_HDLC_INSTANCE_BUF_LOW_WM    1
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold hdlc_instance along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_hdlc_instance_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                    (
                                      PS_HDLC_INSTANCE_BUF_NUM,
                                      PS_HDLC_INSTANCE_BUF_SIZE
                                    )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to hdlc_instance_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * ps_hdlc_instance_buf_hdr[PS_HDLC_INSTANCE_BUF_NUM];
static hdlc_instance_type  * ps_hdlc_instance_buf_ptr[PS_HDLC_INSTANCE_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*===========================================================================
  CONSTANTS AND MACROS
===========================================================================*/
/*---------------------------------------------------------------------------
  HDLC envelope constants
---------------------------------------------------------------------------*/
#define HDLC_FCS_START     0xffff /* Starting bit string for FCS           */
#define HDLC_FCS_FINAL     0xf0b8 /* FCS summed over frame and sender FCS  */
#define HDLC_ACCM_DEFAULT  0xffffffffUL /* Default ACCM                    */
#define HDLC_SP_CHAR       0x20  /* Framer escapes octets less than this,
                                    if flagged in the ACCM                 */

/*---------------------------------------------------------------------------
  Defines for DSM items used by HDLC library
---------------------------------------------------------------------------*/
#define HDLC_DSM_ITEM_POOL     DSM_DS_SMALL_ITEM_POOL
#define HDLC_DSM_ITEM_SIZ      DSM_DS_SMALL_ITEM_SIZ

/*---------------------------------------------------------------------------
  Length of the HDLC CRC field
---------------------------------------------------------------------------*/
#define HDLC_CRC_LEN          2

/*---------------------------------------------------------------------------
  Is the specified HDLC instance valid ?
---------------------------------------------------------------------------*/
#define hdlc_instance_is_valid( instance )                                  \
  ps_mem_is_valid((instance), PS_MEM_HDLC_INSTANCE_TYPE)

/*---------------------------------------------------------------------------
  FCS calculation
---------------------------------------------------------------------------*/
#define hdlc_fcs_16(fcs, c) ((fcs >> 8) ^ crc_16_l_table[(fcs^c) & 0x00ff])

/*---------------------------------------------------------------------------
  Is the unframed packet queue in the specified instance empty ?
---------------------------------------------------------------------------*/
#define HDLC_UNFRAMED_QUEUE_IS_EMPTY(instance)                              \
  (q_cnt(&instance->unframed_pkt_q) == 0)

/*---------------------------------------------------------------------------
  Should this byte be escaped ?
---------------------------------------------------------------------------*/
#define SHOULD_ESC_BYTE( byte, accm )                                       \
  ( ( byte  < HDLC_SP_CHAR && (accm & (1L << byte)) ) ||                    \
    ( byte == HDLC_ESC_ASYNC )                        ||                    \
    ( byte == HDLC_FLAG ) )

/*---------------------------------------------------------------------------
  Is the ACCM specified ok for using hardware ?
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  critical sections to protect access to framer and deframer hardware
---------------------------------------------------------------------------*/
static ps_crit_sect_type  hdlci_hw_deframer_crit_sect;
static ps_crit_sect_type  hdlci_hw_framer_crit_sect;
static boolean hdlci_hw_inited = FALSE;   


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
static int hdlci_sw_unframe
(
  void           *handle,
  dsm_item_type  **framed,
  dsm_item_type  **unframed,
  void           *user_data_ptr
);

static int hdlci_sw_frame
(
  void           *handle,
  dsm_item_type **unframed,
  dsm_item_type **framed,
  void           *user_data_ptr
);


/*===========================================================================
  Macros for HDLC mode and debug output
===========================================================================*/
/*---------------------------------------------------------------------------
  Is the HDLC instance currently using hardware-mode framing/unframing ?
  The HDLC instance being passed in must be a valid instance.  
---------------------------------------------------------------------------*/
#define USING_HARDWARE( instance )   ( ! USING_SOFTWARE (instance) )

/*---------------------------------------------------------------------------
  Is the HDLC instance currently using software-mode framing/unframing ?
---------------------------------------------------------------------------*/
#define USING_SOFTWARE( instance )                                          \
  ( ( instance->frame_f == hdlci_sw_frame ) &&               \
    ( instance->unframe_f == hdlci_sw_unframe ) )

/*---------------------------------------------------------------------------
  Macro to output a debug message indicating current HDLC mode
---------------------------------------------------------------------------*/
#define PRINT_HDLC_MODE( instance )                                         \
  if( USING_HARDWARE( instance ) )                                          \
  {                                                                         \
    LOG_MSG_INFO2("Instance 0x%x using hardware mode", instance, 0, 0);           \
  }                                                                         \
  else if( USING_SOFTWARE( instance ) )                                     \
  {                                                                         \
    LOG_MSG_INFO2("Instance 0x%x using software mode", instance, 0, 0);           \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    LOG_MSG_ERROR("Instance 0x%x in invalid mode", instance, 0, 0);             \
  }


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION HDLCI_FINALIZE_UNFRAMED_PKT()

DESCRIPTION
  This function is used to "finalize" an unframed packet. This esentially
  means chaining the whole thing together, and then trimming off the CRC.

PARAMETERS
  instance: pointer  to the HDLC unframing instance.
  last_item_ptr: pointer to the item containing the end of the frame.
  last_item_len: length of the unframed data in the last frame.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  instance->unframed_pkt_ptr will either be freed (i.e. be set to NULL, if an
    invalid packet is received) or will contain the entire unframed packet.
  instance->errno will be set if the final packet is bad (too short) or is
    empty.
===========================================================================*/
static void hdlci_finalize_unframed_pkt
(
  hdlc_instance_type *instance,
  dsm_item_type      *last_item_ptr,
  int                 last_item_len
)
{
  int framed_len = last_item_ptr->used - last_item_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If last_item_ptr is the first packet in the unframed chain (i.e.
    unframed_pkt_ptr is NULL), we set the start and tail for the unframed
    chain to this item. Else, we append it at the end of the chain.
  -------------------------------------------------------------------------*/
  if(instance->unframed_pkt_ptr == NULL)
  {
    instance->unframed_pkt_ptr = last_item_ptr;
    instance->unframed_pkt_tail_ptr = last_item_ptr;
  }
  else
  {
    last_item_len += instance->unframed_pkt_tail_ptr->used;
    dsm_append(&(instance->unframed_pkt_tail_ptr), &last_item_ptr);
  }

  /*-------------------------------------------------------------------------
    Make sure that between the unframed packet tail pointer and the last
    item at least the CRC + 1 byte is included.  This ensures that we don't
    end up with a free item at the end of unframed_pkt_ptr.
  -------------------------------------------------------------------------*/
  if (last_item_len <= (int)sizeof(uint16))
  {
    /*-----------------------------------------------------------------------
      The CRC is not contained in the last two items so trim the entire
      packet using its total current length - the size of a 16 CRC.  If this
      length becomes negative then free the packet (this means we have a
      packet that is less than the CRC size), then reset length to 0 (which
      will result in freeing the packet).
    -----------------------------------------------------------------------*/
    last_item_len = dsm_length_packet(instance->unframed_pkt_ptr) -
                      framed_len - sizeof(uint16);
    if(last_item_len < 0)
    {
      instance->ps_errno = HDLC_BAD_PACKET;
      last_item_len = 0;
    }
    else if(last_item_len == 0)
    {
      instance->ps_errno = HDLC_ZERO_LEN_PKT;
    }
    dsm_trim_packet(&(instance->unframed_pkt_ptr), last_item_len);
  } /* if(CRC not in last two items) */
  else
  {
    /*-----------------------------------------------------------------------
      The CRC + 1 is contained in the last two items so trim these two items
      to is the size of both items (used in tail ptr + last_item_len) minus
      the CRC length.
    -----------------------------------------------------------------------*/
    dsm_trim_packet(&(instance->unframed_pkt_tail_ptr),
                    last_item_len - sizeof(uint16));
  }

} /* hdlci_finalize_unframed_pkt() */


/*===========================================================================
FUNCTION HDLCI_SW_UNFRAME()

DESCRIPTION
  This function performs HDLC unframing in software. The function failing
  does not always mean that there is no data being generated.
  In the following two cases it is possible to have unframed data returned:
  1. HDLC_NO_MEMORY: This errno is generated when it is not possible to copy
     unframed data to a dsm item when there are two packets in a single item.
     In this case the fully unframed packet will be returned.
  2. HDLC_BAD_CRC: If the unframer has been configured to return packets in
     this case *unframed will contain data when this error occurs.

PARAMETERS
  handle: the instance of the unframer to use (stores any unframed packets
    and the current state of unframing.
  framed: value return parameter
    in:  reference to the framed data that needs unframing
    out: NULL if there is no more data to unframe or pointing to the next
         block of data that needs unframing.
  unframed: will be set to point to an unframed packet when one is available.
    Must be NULL when called.

RETURN VALUE
  0 on success
 -1 on failure - use hdlc_errno() for specifics of error.  The header file
                 has the definitions of the various errnos.

DEPENDENCIES
  None

SIDE EFFECTS
  On memory errors if the item being processed contains the start of a new
  packet, this packet will be lost.
===========================================================================*/
static int hdlci_sw_unframe
(
  void           *handle,          /* associated HDLC instance             */
  dsm_item_type  **framed,         /* address of ptr to framed pkt chain   */
  dsm_item_type  **unframed,       /* address of ptr to unframed pkt chain */
  void           *user_data_ptr
)
{
  hdlc_instance_type *instance = (hdlc_instance_type*)handle;
  dsm_item_type      *rest_of_chain = NULL;  /* chain following item being
                        processed                  */
  dsm_item_type      *left_over_data = NULL; /* 2nd packet in unframed DSM */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    pointers used to traverse the data payload of framed data in a DSM item

    end_ptr - points past the end of the last byte in the dsm item - it is
      only ever used to compare against to make sure we don't go past the
      end of the data in an item.
    write_ptr - is used as the insert point - that is when we have byte that
      is part of the unframed packet it will be written to this location
    read_ptr - is used to keep track of the current byte being read.

    The unframer is implemented as a simple state machine; there are three
    states: FINDING_7E, FINDING_SOP and UNFRAMING.  They are described
    below.
    The first part of unframing always involves finding the frame delimiter
    (0x7e) (FINDING_7E).  This is done by advancing the read_ptr until a 7E
    is found.
    The second part of unframing involves finding the beginning of the packet
    - this is required as it is possible to have multiple consecutive frame
    delimiters. This also is done by advancing the read_ptr until the first
    non-7E is found (obviously this MUST follow the discovery of a 7E either
    in the FINDING_7E or UNFRAMING states).

    NOTE: in the above two cases, it is possible that a DSM item will be
    completely consumed, and it will be necessary to continue on the next one.

    The final part is the actual unframing itself - this involves
    reading/unescaping characters (using read_ptr) and writting them to
    write_ptr, which is only incremented when a charater is actually written
    to that location.  On entering UNFRAMING things will look something like:

            +--+--+--+--+----+
            |7e 7e FF 03 ... |
            +--+--+--+--+----+
             ^     ^          ^
    write ---+     |          |
    read ----------+          |
    end ----------------------+

  -------------------------------------------------------------------------*/
  uint8 *end_ptr;
  uint8 *write_ptr;
  uint8 *read_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Process all of the data that has been passed in, at least until we have
    seen the end of a packet.
  -------------------------------------------------------------------------*/
  while(*framed != NULL)
  {
    /*-----------------------------------------------------------------------
      pull the first item off the chain and set rest_of_chain to next one.
      Intialize the read, write and end pointers.
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO3_2("hdlci_sw_unframe() processing item %p containing %d bytes",
                    *framed, (*framed)->used);
    rest_of_chain = (*framed)->pkt_ptr;
    (*framed)->pkt_ptr = NULL;
    write_ptr = (*framed)->data_ptr;
    read_ptr  = (*framed)->data_ptr;
    /*TODO: Bug when read_ptr = end_ptr */
    end_ptr   = (*framed)->data_ptr + (*framed)->used;

    /*-----------------------------------------------------------------------
      start processing the packet based on the current state of the unframer
    -----------------------------------------------------------------------*/
    switch(instance->state)
    {
    case FINDING_7E:
      LOG_MSG_INFO3_1("hdlci_sw_unframe() in FINDING_7E(%d) state",
                      instance->state);
      /*=====================================================================
        Find the first 7e = discard anything that is not a 7e.
      =====================================================================*/
      for(;*read_ptr != HDLC_FLAG && read_ptr < end_ptr; read_ptr++);

      /*---------------------------------------------------------------------
        We have now either gone to the end of the packet, or found a 7e. In
        the first case free the dsm buffer - set *framed to the next item in
        the chain.
      ---------------------------------------------------------------------*/
      if(read_ptr == end_ptr)
      {
        LOG_MSG_INFO3_1("hdlci_sw_unframe(): at end of item (in FINDING_7E state)",
                        instance->state);
        (void)dsm_free_buffer(*framed);
        *framed = rest_of_chain;
        continue;
      }

      /*---------------------------------------------------------------------
        We found a 7e change the state to FINDING_SOP, increment the read_ptr
        and then check if it is the last byte in the packet - if so free the
        buffer, and continue.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO3_0("hdlci_sw_unframe() changing to FINDING_SOP state");
      instance->state = FINDING_SOP;
      read_ptr++;
      if(read_ptr == end_ptr)
      {
        LOG_MSG_INFO3_1("hdlci_sw_unframe(): at end of item %p "
                        "(in FINDING_SOP state)", *framed);
        (void)dsm_free_buffer(*framed);
        *framed = rest_of_chain;
        continue;
      }
      /* fall through */


    case FINDING_SOP:
      LOG_MSG_INFO3_1("hdlci_sw_unframe() in FINDING_SOP(%d) state",
                      instance->state);
      /*=====================================================================
        find the start of the packet (toss any 7e characters we have found
      =====================================================================*/
      for(;*read_ptr ==  HDLC_FLAG && read_ptr < end_ptr; read_ptr++);

      /*---------------------------------------------------------------------
        We have now either run out of data or found the first non-7e
      ---------------------------------------------------------------------*/
      if(read_ptr == end_ptr)
      {
        LOG_MSG_INFO3_1("hdlci_sw_unframe(): at end of item %p (in FINDING_SOP state)",
                        *framed);
        (void)dsm_free_buffer(*framed);
        *framed = rest_of_chain;
        continue;
      }

      /*---------------------------------------------------------------------
        We have found the fist non-7e: change the state to UNFRAMING and
        reinitialize the Frame CRC
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO3_0("hdlci_sw_unframe() changed to UNFRAMING state");
      instance->state = UNFRAMING;
      instance->frame_crc = HDLC_FCS_START;
      /*fall through */

    case UNFRAMING:
      LOG_MSG_INFO3_1("hdlci_sw_unframe() in UNFRAMING(%d) state",
                      instance->state);

      /*=====================================================================
        We are now unframing!
      =====================================================================*/
      while(read_ptr < end_ptr)
      {
        /*-------------------------------------------------------------------
          If the current character is the HDLC flag we have found the end of
          the current packet (possibly the start of the next).
        -------------------------------------------------------------------*/
        if(*read_ptr == HDLC_FLAG)
        {
          LOG_MSG_INFO3_2("hdlci_sw_unframe() found packet boundary in %p @ position %d",
                          *framed, (read_ptr - (*framed)->data_ptr));

          /*-----------------------------------------------------------------
            We have found the end of the packet, but are there more bytes in
            this DSM item?  If so DUP them, so that they can be unframed in a
            subsequent call.

            +--+--+--+--+--+--+--+--+--+  The bytes that need to be dup'ed
            |unfr pkt| old |~  2nd pkt |  are between end and read.
            +--+--+--+--+--+--+--+--+--+  Note that (read_ptr + 1 == end_ptr)
                      ^     ^           ^ means that read_ptr is currently
            write ----+     |           | pointing to the last byte in the
            read -----------+           | DSM, so DUP only if
            end ------------------------+ end_ptr > read_ptr + 1
          -----------------------------------------------------------------*/
          if(end_ptr > read_ptr + 1)
          {
            LOG_MSG_INFO3_1("hdlci_sw_unframe() data after packet boundary in %p",
                            *framed);
            if(dsm_dup_packet(&left_over_data,
                              (*framed),
                              (uint16)(read_ptr-((*framed)->data_ptr)),
                                               /* offset */
                              (uint16)(end_ptr - read_ptr) /* count */) == 0)
            {
              LOG_MSG_ERROR_1(
              "hdlci_sw_unframe() DUP()ing data after '~' in %p failed", *framed);
              /*-------------------------------------------------------------
                we didn't dup any bytes - so return an error indicating we
                are out of memory.  Given that we are unframing in place
                there is no graceful way to recover (no longer a copy of the
                original unframed data).  So:
                - finalize the unframed data.
                - Set the state to FINDING_7E which will toss the rest of
                  this packet as we don't have the beginning of it.
                - set unframed to the unframed data that was finalized.
                - reset the unframed_pkt_ptr
                - Set *framed to rest_of_chain
                - return the 'no memory' error
              -------------------------------------------------------------*/
              hdlci_finalize_unframed_pkt(instance,
                                          *framed,
                                          write_ptr -((*framed)->data_ptr));
              LOG_MSG_INFO3_0("hdlci_sw_unframe() changing to FINDING_7E state");
              instance->state = FINDING_7E;
              *unframed = instance->unframed_pkt_ptr;
              instance->unframed_pkt_ptr = NULL;
              instance->unframed_pkt_tail_ptr = NULL;
              *framed = rest_of_chain;
              instance->ps_errno = HDLC_NO_MEMORY;
              instance->last_byte_was_esc = 0;
              return -1;

            } /* if(dupping failed) */
          } /* if(data after 7e) */

          /*-----------------------------------------------------------------
            Since 7e indiactes a packet boundary we could have another packet
            immediately following the current 7e so set the state to
            FINDING_SOP (at this point we know we have the start of another
            packet.
          -----------------------------------------------------------------*/
          LOG_MSG_INFO3_0("hdlci_sw_unframe() changing to FINDING_SOP state");
          instance->state = FINDING_SOP;

          /*-----------------------------------------------------------------
            We have a complete packet, so take the common steps as if we need
            to return the packet.
            - finalize the unframed data.
            - attach the rest_of_chain to left_over_data (if there is no
              left_over_data this will assign rest_of_chain to it)
            - have *framed point to whatever framed data that exists
              (i.e. left_over_data)
            - if there is no unframed data (i.e. previous packet was too
              small) then break out of the loop and proceed with next
              packet. Otherwise:
            - set *unframed to the unframed packet (unframed_pkt_ptr)
            - reset unframed_pkt_ptr as will be unframing new packet.
          -----------------------------------------------------------------*/
          hdlci_finalize_unframed_pkt(instance,
                                      *framed,
                                      write_ptr -((*framed)->data_ptr));
          dsm_append(&left_over_data, &rest_of_chain);
          *framed = left_over_data;
          left_over_data = NULL;
          *unframed = instance->unframed_pkt_ptr;
          instance->unframed_pkt_ptr = NULL;
          instance->unframed_pkt_tail_ptr = NULL;
          if( instance->ps_errno == HDLC_BAD_PACKET ||
              instance->ps_errno == HDLC_ZERO_LEN_PKT )
          {
            /*-----------------------------------------------------------------
              Clear last byte was esc in error cases,
              to allow next packet to be processed properly.
            -----------------------------------------------------------------*/
            instance->last_byte_was_esc = 0;
            return -1;
          }

          /*-----------------------------------------------------------------
            Verify the CRC
          -----------------------------------------------------------------*/
          if(instance->frame_crc == HDLC_FCS_FINAL)
          {
            LOG_MSG_INFO3_2("HDLC SW Deframer instance (%p), unframed packet (%p)",
                            instance, *unframed);
            /*-----------------------------------------------------------------
              Clear last byte was esc in error cases,
              to allow next packet to be processed properly.
            -----------------------------------------------------------------*/
            instance->last_byte_was_esc = 0;

            if(instance->ps_errno != HDLC_OK)
            {
              return -1;
            }

            return 0;
          }
          else
          {
            /*---------------------------------------------------------------
              The CRC is bad so either free the packet or return it anyway.
            ---------------------------------------------------------------*/
            LOG_MSG_ERROR_1("hdlci_sw_unframe() CRC failed on item %p",
                            *unframed);

            dsm_free_packet(unframed);
            instance->ps_errno = HDLC_BAD_CRC;

            /*-----------------------------------------------------------------
              Clear last byte was esc in error cases,
              to allow next packet to be processed properly.
            -----------------------------------------------------------------*/
            instance->last_byte_was_esc = 0;
            return -1;

          } /* if(bad CRC) */

        } /* if(found end of packet) */

        /*-------------------------------------------------------------------
          If the current byte is the escape character and the previous byte
          is not the escape character, store that it was,advance the read_ptr, 
          and continue the loop.
        -------------------------------------------------------------------*/
        if(*read_ptr == HDLC_ESC_FLAG
           && instance->last_byte_was_esc != HDLC_ESC_COMPL)
        {
          LOG_MSG_INFO3_2("hdlci_sw_unframe() found escape character in %p @ %d",
                          *framed, (read_ptr - (*framed)->data_ptr));
          instance->last_byte_was_esc = HDLC_ESC_COMPL;
          read_ptr++;
          /*-----------------------------------------------------------------
            This could be optimized by checking if we are at the end and if
            not do not contine, but fall through.
          -----------------------------------------------------------------*/
          continue;
        }

        /*-------------------------------------------------------------------
          At this point we know the current byte is not either the flag or
          the escape charater.  So XOR and copy the current read byte (the
          value of last_byte_was_esc will either be zero or the complement
          value, so always XORing is about as expensive as doing the if
          check).  Reset the escape indicator, calculate the FCS and move
          both the read and write pointer forward.
        -------------------------------------------------------------------*/
        *write_ptr = *read_ptr ^ instance->last_byte_was_esc;
        instance->last_byte_was_esc = 0;
        instance->frame_crc = hdlc_fcs_16(instance->frame_crc, *write_ptr);
        write_ptr++;
        read_ptr++;

      } /* while(data left in item) */

      /*---------------------------------------------------------------------
        We have consumed all of the data in the current item, so:
        - remove data following write_ptr so item only contains unframed data
        - add unframed data to chain of unframed data items.
        - reassign unframed_pkt_tail_ptr to point to this now-last-item - if
          it isn't already assigned there.
        - make sure that there is no left over data - whenever there is all
          of the chaining should have already happened as this is a scenario
          where we found the end of a packet and hte beginning of the next in
          the same item.  All of this is handled in the end-of-packet
          handling above.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO3_1("hdlci_sw_unframe() unframed item %p", *framed);
      dsm_trim_packet(framed, (write_ptr - ((*framed)->data_ptr)));
      /*---------------------------------------------------------------------
        If the trim causes *framed to become NULL, we don't have anything
        to append to the unframed dsm item chain. Else,
        1. If this is the first packet in the chain, we set unframed_pkt_ptr
           and unframed_pkt_tail_ptr to *framed
        2. Otherwise, chain it to the end of the unframed dsm item chain.
      ---------------------------------------------------------------------*/
      if(*framed != NULL)
      {
        if(instance->unframed_pkt_ptr == NULL)
        {
          instance->unframed_pkt_ptr = *framed;
          instance->unframed_pkt_tail_ptr = *framed;
        }
        else
        {
          ASSERT(instance->unframed_pkt_tail_ptr != NULL);
          instance->unframed_pkt_tail_ptr->pkt_ptr = *framed;
          instance->unframed_pkt_tail_ptr = *framed;
        }
      }
      ASSERT(left_over_data == NULL);
      *framed = rest_of_chain;
      break;

    default:
      LOG_MSG_ERROR_1("hdlci_sw_unframe() invalid state %d",
                      (uint16)instance->state);
      ASSERT(0);

    } /* switch(HDLC unframing state) */

  } /* while(there is data to unframe) */

  return 0;

} /* hdlci_sw_unframe() */


/*===========================================================================
FUNCTION HDLCI_SW_FRAME()

DESCRIPTION
  This function will frame the input packet based on the specified accm,
  append the CRC and trailing 7E flag and return the framed packet.
  It will do the entire process in software.

  The input packet pointer should be the complete packet to be framed
  including HDLC header and protocol field (if we desire to send them),
  but without the CRC.

  This implementation does not escape the DEL character(0x7f), or ACCM
  equivalents with the 8th bit set. Both these are optional per RFC 1662.
  If needed in the future, can provide a configuration item, or hdlc_ioctl()
  support for clients to indicate these.

  Excerpt from RFC 1662 PPP in HDLC-like Framing
  (Sec 7.1.  Async-Control-Character-Map (ACCM))
  ---------------------------------------------------------------------------
  For compatibility with 3309:1991/Amendment 2, the transmitter MAY escape
  DEL and ACCM equivalents with the 8th (most significant) bit set. No change
  is required in the receiving algorithm.

  Following ACCM negotiation, the transmitter SHOULD cease escaping DEL.
  ---------------------------------------------------------------------------

DEPENDENCIES
  - Assumes handle passed in is a valid HDLC instance
  - The negotiated ACCM must be set for this HDLC instance before calling
    this function.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
static int hdlci_sw_frame
(
  void           *handle,
  dsm_item_type **unframed,
  dsm_item_type **framed,
  void           *user_data_ptr
)
{
  hdlc_instance_type *instance          = NULL; /* HDLC instance           */
  dsm_item_type      *outgoing_head_ptr = NULL; /* first buffer            */
  dsm_item_type      *outgoing_tail_ptr = NULL; /* current buffer          */
  uint16              calc_fcs          = HDLC_FCS_START; /* init CRC      */
  uint16              byte_index;   /* counter through input bytes         */
  uint8               c_byte;       /* current input byte being processed  */
  uint8              *framed_byte_ptr; /* running pointer for framed bytes */
  uint16              framed_bytes; /* cntr for framed bytes in a dsm item */
  uint16              buffer_size;  /* size of the DSM buffer              */
  uint16              bytes_to_frame = 0;  /* size of the DSM buffer       */
  uint16              total_bytes_allocated = 0; /*lint --e{550} total bytes
                                                   allocated for framed data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  instance  = (hdlc_instance_type *)handle;

  bytes_to_frame = (uint16)dsm_length_packet(*unframed);

  if(bytes_to_frame == 0)
  {
    LOG_MSG_ERROR_0("HDLC software framer called with empty dsm chain");
    dsm_free_packet(unframed);
    *framed = NULL;
    instance->ps_errno = HDLC_ZERO_LEN_PKT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Allocate memory to hold framed packet.
  -------------------------------------------------------------------------*/
  if((outgoing_tail_ptr = hdlci_allocate_buffer(
      HDLC_DSM_ITEM_POOL, bytes_to_frame, TRUE)) == NULL)
  {
    LOG_MSG_ERROR_1("HDLC software framer out of memory: items = %d",
                    DSM_POOL_FREE_CNT(HDLC_DSM_ITEM_POOL));
    dsm_free_packet(unframed);
    *framed = NULL;
    instance->ps_errno = HDLC_NO_MEMORY;
    ASSERT(0);
    return -1;
  }

  total_bytes_allocated  = outgoing_tail_ptr->size;
  buffer_size      = outgoing_tail_ptr->size;
  framed_byte_ptr  = outgoing_tail_ptr->data_ptr;
  framed_bytes     = 0;
  
  if(framed_byte_ptr == NULL)
  {
    DATA_ERR_FATAL("Allocated DSM item has NULL data_ptr");
    return -1;
  }
  /*-------------------------------------------------------------------------
    Insert 7E at the start.
  -------------------------------------------------------------------------*/
  *framed_byte_ptr++ = HDLC_FLAG;
  framed_bytes++;

  /*-------------------------------------------------------------------------
    Build outgoing packet packet by pulling each byte from incoming packet,
    performing necessary FCS and stuffing along the way.
  -------------------------------------------------------------------------*/
  while(*unframed != NULL)
  {
    for( byte_index = 0; byte_index < (*unframed)->used; byte_index++ )
    {
      c_byte = (*unframed)->data_ptr[byte_index];

      /*---------------------------------------------------------------------
        Assuming a 16-bit CRC, need to ensure that there is room for 7 bytes
        in this DSM item: need 2 bytes each for current byte, low-order and
        high-order bytes of the CRC; and 1 byte for the trailing 7E flag.
        Else, need to get a new DSM item.
      ---------------------------------------------------------------------*/
      if(framed_bytes + 7 > buffer_size)
      {
        LOG_MSG_INFO3_0("Appending to packet");
        outgoing_tail_ptr->used = framed_bytes;
        dsm_append(&outgoing_head_ptr, &outgoing_tail_ptr);

        if((outgoing_tail_ptr =
              hdlci_allocate_buffer(HDLC_DSM_ITEM_POOL,
                (bytes_to_frame - total_bytes_allocated), TRUE)) == NULL)
        {
          /*-----------------------------------------------------------------
            No memory available, free packet and return
          -----------------------------------------------------------------*/
          LOG_MSG_ERROR_1("HDLC software framer out of memory: items = %d",
                          DSM_POOL_FREE_CNT(HDLC_DSM_ITEM_POOL));
          dsm_free_packet(&outgoing_head_ptr);
          dsm_free_packet(unframed);
          *framed = NULL;
          instance->ps_errno = HDLC_NO_MEMORY;
          ASSERT(0);
          return -1;
        }

        total_bytes_allocated += outgoing_tail_ptr->size;

        /*-------------------------------------------------------------------
          Size of the allocated buffer should be atleast 7 bytes
        -------------------------------------------------------------------*/
        ASSERT(outgoing_tail_ptr->size > 7);

        buffer_size     = outgoing_tail_ptr->size;
        framed_byte_ptr = outgoing_tail_ptr->data_ptr;
        framed_bytes    = 0;
      } /* if(framed_bytes + 7 > buffer_size) */

      /*---------------------------------------------------------------------
        Fold char value into FCS calculation then do character escaping.
      ---------------------------------------------------------------------*/
      calc_fcs = hdlc_fcs_16(calc_fcs, c_byte);
      if( SHOULD_ESC_BYTE(c_byte, instance->accm) )
      {
        *framed_byte_ptr++  = HDLC_ESC_ASYNC;
        *framed_byte_ptr++  = (uint8)(c_byte ^ HDLC_ESC_COMPL);
        framed_bytes       += 2;
      }
      else /* no escaping needed, just copy byte over */
      {
        *framed_byte_ptr++ = (uint8)c_byte;
        framed_bytes++;
      }
    } /* for */

    *unframed = dsm_free_buffer( *unframed );
  } /* while(*unframed != NULL) */


  /*-------------------------------------------------------------------------
    Escape the 2 CRC bytes if necessary, process low-order CRC byte first
  -------------------------------------------------------------------------*/
  calc_fcs ^= 0xffff;
  c_byte = calc_fcs & 0x00ff;

  if( SHOULD_ESC_BYTE(c_byte, instance->accm) )
  {
    *framed_byte_ptr++ = HDLC_ESC_ASYNC;
    *framed_byte_ptr++ = (uint8)(c_byte ^ HDLC_ESC_COMPL);
    framed_bytes      += 2;
  }
  else /* no escaping needed */
  {
    *framed_byte_ptr++ = (uint8)c_byte;
    framed_bytes++;
  }

  /*-------------------------------------------------------------------------
    Process high-order CRC byte
  -------------------------------------------------------------------------*/
  c_byte = (calc_fcs >> 8);
  if( SHOULD_ESC_BYTE(c_byte, instance->accm) )
  {
    *framed_byte_ptr++ = HDLC_ESC_ASYNC;
    *framed_byte_ptr++ = (uint8)(c_byte ^ HDLC_ESC_COMPL);
    framed_bytes      += 2;
  }
  else /* no escaping needed */
  {
    *framed_byte_ptr++ = (uint8)c_byte;
    framed_bytes++;
  }

  *framed_byte_ptr++ = HDLC_FLAG;
  framed_bytes++;

  outgoing_tail_ptr->used = framed_bytes;

  /*-------------------------------------------------------------------------
    We are ready with the framed packet, assign to *framed and return
  -------------------------------------------------------------------------*/
  dsm_append(&outgoing_head_ptr, &outgoing_tail_ptr);
  *framed = outgoing_head_ptr;

  LOG_MSG_INFO3_2("HDLC SW Framer instance (%p), framed packet (%p)",
           instance, *framed);

  return 0;
} /* hdlci_sw_frame() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION HDLC_POWERUP_INIT()

DESCRIPTION This function is called during the powerup of PS task. As a
            part of this powerup initialization function, the protocol
            processor module if present is initialized.

PARAMETERS

RETURN VALUE
  Returns 0 : If success
          -1 : If failed

DEPENDENCIES
  This

SIDE EFFECTS
===========================================================================*/
int hdlc_powerup_init
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return 0;
}
/*===========================================================================
FUNCTION PS_HDLC_LIB_INIT()

DESCRIPTION
  Initializes ps_hdlc_lib module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_hdlc_lib_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pool
  -------------------------------------------------------------------------*/
    if (PS_MEM_POOL_INIT_OPT(PS_MEM_HDLC_INSTANCE_TYPE,
                             ps_hdlc_instance_buf_mem,
                             PS_HDLC_INSTANCE_BUF_SIZE,
                             PS_HDLC_INSTANCE_BUF_NUM,
                             PS_HDLC_INSTANCE_BUF_HIGH_WM,
                             PS_HDLC_INSTANCE_BUF_LOW_WM,
                             NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                             (int *) ps_hdlc_instance_buf_hdr,
                             (int *) ps_hdlc_instance_buf_ptr
#else
                             NULL,
                             NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                             ) == -1)
    {
      DATA_ERR_FATAL("Can't init the module");
      return;
    }
} /* ps_hdlc_lib_init() */


/*===========================================================================
FUNCTION HDLC_INIT_CONFIG()

DESCRIPTION
  This function initializes the config data structure that was passed in to
  the default values for the various fields.

PARAMETERS
  config: pointer to the configuration data structure to intialize.
  Must not be passed in as NULL.

RETURN VALUE
  None

DEPENDENCIES
  IMPORTANT NOTE:
  Configuration for max_len and strip_ff03_if_present in hdlc_config_type
  is not supported currently.

SIDE EFFECTS
  None
===========================================================================*/
void hdlc_init_config
(
  hdlc_config_type *config
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(config == NULL)
  {
    DATA_ERR_FATAL("NULL Config passed to hdlc_init_config");
    return;
  }

  (void)memset(config, 0, sizeof(hdlc_config_type));

} /* hdlc_init_config() */


/*===========================================================================
FUNCTION HDLC_ALLOC_INSTANCE()

DESCRIPTION
  This function will allocate an instance of the HDLC engine, set the
  instance variables per the config passed in, and initialize the function
  pointers to do use either hardware or software mode depending on
  featurization.

PARAMETERS
  config: pointer to the configuration data structure.  Must not be passed
  in as NULL.

RETURN VALUE
  NULL: on failure
  pointer to handle instance.

DEPENDENCIES
  IMPORTANT NOTE:
  Configuration for max_len and strip_ff03_if_present in hdlc_config_type
  is not supported currently.

SIDE EFFECTS
  None
===========================================================================*/
void *hdlc_alloc_instance
(
  hdlc_config_type *config
)
{
  hdlc_instance_type *instance;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(config == NULL)
  {
    DATA_ERR_FATAL("NULL Config passed to HDLC Alloc");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Grab a HDLC instance from PS Mem allocator (which memsets the memory) so
    there is no need to explicitly set any of the members except those passed
    in via configuration, and the FCS which has a non-zero start value.
  -------------------------------------------------------------------------*/
  instance = (hdlc_instance_type *)ps_mem_get_buf(PS_MEM_HDLC_INSTANCE_TYPE);
  if(instance == NULL)
  {
    DATA_ERR_FATAL("Unable to allocate memory for HDLC instance");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Initialize the allocated hdlc instance to zero
  -------------------------------------------------------------------------*/
  memset(instance, 0, sizeof(hdlc_instance_type));

  /*-------------------------------------------------------------------------
    Store values in instance based on config passed
    NOTE: The max_len and strip_ff03_if_present configuration parameters
    are not supported currently.
  -------------------------------------------------------------------------*/
  instance->max_len               = config->max_len;
  instance->strip_ff03_if_present = config->strip_ff03_if_present;
  instance->accm                  = config->accm;

  /*-------------------------------------------------------------------------
    Initialize the unframed packet queue
  -------------------------------------------------------------------------*/
  (void) q_init(&(instance->unframed_pkt_q));

  /*-------------------------------------------------------------------------
    Initialize the CRC
  -------------------------------------------------------------------------*/
  instance->frame_crc = HDLC_FCS_START;

  /*-------------------------------------------------------------------------
    Set the function pointers to use HDLC software
  -------------------------------------------------------------------------*/
  instance->frame_f   = hdlci_sw_frame;
  instance->unframe_f = hdlci_sw_unframe;


  if(hdlci_hw_inited == FALSE)
  {
    PS_INIT_CRIT_SECTION(&hdlci_hw_framer_crit_sect);
    PS_INIT_CRIT_SECTION(&hdlci_hw_deframer_crit_sect);
    hdlci_hw_inited = TRUE;
  }

  LOG_MSG_INFO1_1("HDLC lib allocated HDLC instance %p", instance);
  PRINT_HDLC_MODE( instance );
  return instance;
} /* hdlc_alloc_instance() */


/*===========================================================================
FUNCTION HDLC_FREE_INSTANCE()

DESCRIPTION
  This function will reset the instance passed in and free the memory.

PARAMETERS
  instance: a referance to an instance pointer - this will be set to NULL

RETURN VALUE
  0 on success
 -1 on failure (which will only happen if the handle is invalid).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int hdlc_free_instance
(
  void **instance
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(instance == NULL)
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    verify that the instance is valid - if so free it, otherwise return error
  -------------------------------------------------------------------------*/
  if(hdlc_instance_is_valid(*instance))
  {
    if(hdlci_hw_inited == TRUE)
    {
      PS_DESTROY_CRIT_SECTION(&hdlci_hw_framer_crit_sect);
      PS_DESTROY_CRIT_SECTION(&hdlci_hw_deframer_crit_sect);
      hdlci_hw_inited = FALSE;
    }    
    (void)hdlc_reset( *instance );
    PS_HDLC_INSTANCE_DESTROY_Q( *instance );
    PS_MEM_FREE( *instance );
    return 0;
  }

  /*-------------------------------------------------------------------------
    the instance was invalid - set the reference pointer to NULL and return
    an error.
  -------------------------------------------------------------------------*/
  *instance = NULL;
  return -1;

} /* hdlc_free_instance() */


/*===========================================================================
FUNCTION HDLC_RESET()

DESCRIPTION
  This function will reset the state of the HDLC instance. This includes
  freeing up any partially unframed packets, trashing the unframed packet
  queue and resetting the state variables. It also resets the hardware
  framer and deframer. The parameters set originally via config (max_len,
  return_pkt_on_bad_crc, strip_ff03_if_present and accm) stay unchanged, as
  do the framing and unframing function pointers (frame_f and unframe_f).

PARAMETERS
  handle: pointer to the instance being freed.

RETURN VALUE
  0  : On Success
  -1 : On Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int hdlc_reset
(
  void * handle
)
{
  hdlc_instance_type * instance = (hdlc_instance_type *)handle;
  dsm_item_type      * item_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(hdlc_instance_is_valid(instance) == FALSE)
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Free any partially unframed packets, trash the unframed packet queue,
    and reset the state variables. Also reset the hardware framer and
    deframer. The parameters set originally via config (max_len,
    return_pkt_on_bad_crc, strip_ff03_if_present and accm) stay unchanged, as
    do the framing and unframing function pointers (frame_f and unframe_f).
  -------------------------------------------------------------------------*/
  dsm_free_packet( &(instance->unframed_pkt_ptr) );
  instance->unframed_pkt_tail_ptr = NULL;
  instance->ps_errno                 = HDLC_OK;
  instance->frame_crc             = HDLC_FCS_START;
  instance->last_byte_was_esc     = FALSE;
  instance->state                 = FINDING_7E;
  instance->data_state            = 0;
  while( ( item_ptr =
        (dsm_item_type *)q_get( &instance->unframed_pkt_q ) ) != NULL )
  {
    dsm_free_packet( &item_ptr );
  }
  return 0;

} /* hdlc_reset() */


/*===========================================================================
FUNCTION HDLC_ERRNO()

DESCRIPTION
  This function will return the errno of the last operation on the instance
  passed in.

PARAMETERS
  instance: the instance on which the errno is being checked.

RETURN VALUE
  0 if there is no error
  one of the values defined in the HDLC errno enum (look in the header file)
  Note: the following are currently not implemented:
  HDLC_PKT_TOO_LONG (there is currently no support for limiting packet
  lengths).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
hdlc_errno_enum_type hdlc_errno
(
  void *instance
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    make sure the instance is valid, if it isn't return bad handle, otherwise
    return the errno stored in the instance control block;
  -------------------------------------------------------------------------*/
  if(hdlc_instance_is_valid(instance) == FALSE)
  {
    return HDLC_BAD_HANDLE;
  }

  return ((hdlc_instance_type*)instance)->ps_errno;

} /* hdlc_errno() */


/*===========================================================================
FUNCTION HDLC_UNFRAME()

DESCRIPTION
  This function performs the HDLC unframing.  The function returning -1 does
  not always mean that there is no data being generated.  In the following
  cases it is possible to have unframed data returned:
    1. HDLC_NO_MEMORY: This errno is generated when it is not possible to
       copy unframed data to a dsm item when there are two packets in a
       single item.  In this case the fully unframed packet will be returned.
    2. HDLC_BAD_CRC: if the unframer has been configured to return packets in
      this case *unframed will contain data when this error occurs.
    3. HDLC_MORE_PKTS_AVAIL: If the unframer has more packets queued up
       internally for the framed chain passed in. The caller of the API is
       expected to call the function again to retrieve these packets.

   Clients of this API should invoke the API until *framed goes NULL AND
   the errno returned is HDLC_OK. This recommended semantic is shown with
   an example below.

   ==*==*==*==*==*==*==*==   BEGIN CODE SNIPPET   =*=*==*==*==*==*==*==*==*==

   int ps_errno = HDLC_OK;
   int retval = 0;

   while((*framed != NULL) || (ps_errno != HDLC_OK))
   {
     *unframed = NULL;
     retval = hdlc_unframe(instance, framed, unframed, user_data_ptr);
     ps_errno = hdlc_errno(instance);
     if(retval < 0)
     {
       switch(ps_errno)
       {
         case HDLC_MORE_PKTS_AVAIL:
           use_unframed_pkt();
           continue;

         case HDLC_BAD_HANDLE:
           dsm_free_packet(framed);
           dsm_free_packet(unframed);
           break;

         default:
           break;
       }

       if(*unframed == NULL)
       {
         continue;
       }
     }
     use_unframed_pkt(unframed);
   }

   ==*==*==*==*==*==*==*==*   END CODE SNIPPET   ==*=*==*==*==*==*==*==*==*==

PARAMETERS
  handle: the instance of the unframer to use (stores any unframed packets
    and the current state of unframing.
  framed: value return parameter
    in:  reference to the framed data that needs unframing
    out: NULL if there is no more data to unframe or pointing to the next
         block of data that needs unframing.
  unframed: will be set to point to an unframed packet when one is available.
    Must be NULL when called.

RETURN VALUE
  0 on success
 -1 on failure or if the unframer has more packets to return:
   - use hdlc_errno() for specifics of error.  The header file
                 has the definitions of the various errnos.

DEPENDENCIES
  None

SIDE EFFECTS
  on memory errors if the item being processed contains the start of a new
  packet, this packet will be lost.
===========================================================================*/
int hdlc_unframe
(
  void           *handle,
  dsm_item_type **framed,
  dsm_item_type **unframed,
  void           *user_data_ptr
)
{
  hdlc_instance_type *instance = (hdlc_instance_type*)handle;
  int retval = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(hdlc_instance_is_valid(instance) == FALSE)
  {
    return -1;
  }

  if(framed == NULL || unframed == NULL)
  {
    instance->ps_errno = HDLC_BAD_PARAM;
    return -1;
  }

  /*-------------------------------------------------------------------------
    The parameters are valid, so start errno in OK state
  -------------------------------------------------------------------------*/
  instance->ps_errno = HDLC_OK;

  *unframed = NULL;

  retval = instance->unframe_f(instance, framed, unframed,
                                 user_data_ptr);


  return retval;
} /* hdlc_unframe() */


/*===========================================================================
FUNCTION HDLC_FRAME()

DESCRIPTION
  This function will frame the incoming packet.
  The input packet pointer (unframed) should be the complete packet to be
  framed including HDLC header and protocol field (if we desire to send
  them), but without the CRC.

PARAMETERS
    handle: The HDLC instance to use
    unframed: Points to the packet that needs framing.
    framed  : Points to the framed packet, *framed must be NULL when called.

DEPENDENCIES
  The ACCM must be set for this HDLC instance before calling this function.

RETURN VALUE
  0 on success
 -1 on failure - use hdlc_errno() for specifics of error.  The header file
                 has the definitions of the various errnos.

SIDE EFFECTS
  None
===========================================================================*/
int hdlc_frame
(
  void           *handle,
  dsm_item_type **unframed,
  dsm_item_type **framed,
  void           *user_data_ptr
)
{
  hdlc_instance_type *instance = (hdlc_instance_type *)handle;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(hdlc_instance_is_valid(instance) == FALSE)
  {
    return -1;
  }

  if( framed    == NULL ||
      unframed  == NULL ||
      *unframed == NULL ||
      *framed   != NULL )          /* *framed should be NULL on the way in */
  {
    instance->ps_errno = HDLC_BAD_PARAM;
    return -1;
  }

  /*-------------------------------------------------------------------------
    The parameters are valid, so start errno in OK state
  -------------------------------------------------------------------------*/
  instance->ps_errno = HDLC_OK;

  /*-------------------------------------------------------------------------
    Call the function pointer to invoke the framing function
  -------------------------------------------------------------------------*/
  return instance->frame_f(instance, unframed, framed,
                           user_data_ptr);
} /* hdlc_frame() */


/*===========================================================================
FUNCTION HDLC_IOCTL

DESCRIPTION
  Performs the required operation on an HDLC instance.
  Takes an HDLC instance, the type of ioctl and associated data. Depending on
  the IOCTL the data may be used to return or pass information.

PARAMETERS
      handle: The HDLC instance to use
  ioctl_type: The HDLC ioctl
  ioctl_data: Associated ioctl information
       errno: Error code returned, if any error

DEPENDENCIES
  None

RETURN VALUE
  -1 in case of error (e.g., invalid arguments)
   0 in case of success

SIDE EFFECTS
  The ACCM or HDLC mode for the specified instance may be changed.
===========================================================================*/
int hdlc_ioctl
(
  void                 * handle,
  hdlc_ioctl_e_type      ioctl_type,
  hdlc_ioctl_u_type    * ioctl_data,
  hdlc_errno_enum_type * ps_errno
)
{
  hdlc_instance_type   * instance = (hdlc_instance_type *)handle;
  hdlc_mode_enum_type    current_mode = HDLC_SW;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == ps_errno)
  {
    LOG_MSG_ERROR_0("hdlc_ioctl(): ps_errno is NULL");
    return -1;
  }

  if(NULL == ioctl_data)
  {
    LOG_MSG_ERROR_0("hdlc_ioctl(): ioctl data pointer is NULL");
    *ps_errno = HDLC_BAD_PARAM;
     return -1;
  }

  if(hdlc_instance_is_valid(instance) == FALSE)
  {
    *ps_errno = HDLC_BAD_HANDLE;
    return -1;
  }

  *ps_errno = HDLC_OK;
  LOG_MSG_INFO2_2("hdlc_ioctl(0x%x) called for instance %p",
                  ioctl_type, instance);

  if( USING_HARDWARE( instance ) )
  {
  }
  else if (USING_SOFTWARE( instance ) )
  {
    current_mode = HDLC_SW;
  }
  else
  {
    LOG_MSG_INFO2_1("Instance 0x%x in invalid mode", instance);
    *ps_errno = HDLC_FAILURE;
    return -1;
  }

  switch( ioctl_type )
  {
    /*-----------------------------------------------------------------------
      Ioctl to get current value of accm for the specified instance
    -----------------------------------------------------------------------*/
    case HDLC_IOCTL_GET_ACCM:
      ioctl_data->accm = instance->accm;
      break;

    /*-----------------------------------------------------------------------
      Ioctl to set the accm value for the specified instance
    -----------------------------------------------------------------------*/
    case HDLC_IOCTL_SET_ACCM:
      instance->accm = ioctl_data->accm;
      LOG_MSG_INFO2_2("HDlC instance 0x%x, accm set to 0x%x",
                      instance, instance->accm);
      break;

    /*-----------------------------------------------------------------------
      Ioctl to get current HDLC mode for the specified instance
    -----------------------------------------------------------------------*/
    case HDLC_IOCTL_GET_HDLC_MODE:
      ioctl_data->hdlc_mode = current_mode;
      break;

    /*-----------------------------------------------------------------------
      Ioctl to set HDLC mode for the specified instance.
      This always returns success if FEATURE_HDLC_HW_ACCEL is not defined.
    -----------------------------------------------------------------------*/
    case HDLC_IOCTL_SET_HDLC_MODE:
      LOG_MSG_INFO2_1("HW acceleration not supported, current mode unchanged for "
                      "instance 0x%x", instance);
      break;

    /*-----------------------------------------------------------------------
      Ioctl to get current state of the unframer( FINDING_7E, FINDING_SOP
      or UNFRAMING )
    -----------------------------------------------------------------------*/
    case HDLC_IOCTL_GET_UNFRAMER_STATE:
      switch( current_mode )
      {
        case HDLC_SW:
          ioctl_data->unframe_state = instance->state;
          break;
        default:
          *ps_errno = HDLC_FAILURE;
          return -1;
      } /* switch( current_mode ) */
      LOG_MSG_INFO2_2("Instance 0x%x, unframer state is 0x%x",
                      instance, ioctl_data->unframe_state);
      break;

    /*-----------------------------------------------------------------------
      Ioctl to set unframer state for the specified instance.
      TODO: Implement for setting unframer state for the hardware.
    -----------------------------------------------------------------------*/
    case HDLC_IOCTL_SET_UNFRAMER_STATE:
      switch( current_mode )
      {
        case HDLC_SW:
          switch( ioctl_data->unframe_state )
          {
            case FINDING_7E:
            case FINDING_SOP:
              dsm_free_packet( &(instance->unframed_pkt_ptr) );
              instance->unframed_pkt_tail_ptr = NULL;
              instance->frame_crc             = HDLC_FCS_START;
              instance->last_byte_was_esc     = 0;
              instance->ps_errno                 = HDLC_OK;
              instance->state                 = ioctl_data->unframe_state;
              LOG_MSG_INFO1_2("Instance 0x%x, software unframer state set to 0x%x",
                              instance, instance->state);
              break;

            case UNFRAMING:
              LOG_MSG_ERROR_1("Instance 0x%x, state set to UNFRAMING disallowed",
                              instance);
              *ps_errno = HDLC_INVALID_OP;
              return -1;

            default:
              *ps_errno = HDLC_FAILURE;
              return -1;
          } /* switch( ioctl_data->unframe_state ) */
          break;

        default:
          ASSERT( 0 );
          *ps_errno = HDLC_FAILURE;
          return -1;
      } /* switch( current_mode ) */
      break;

    default:
      LOG_MSG_ERROR_2("hdlc_ioctl(): Invalid operation %d requested, instance %p",
                      ioctl_type, instance);
      *ps_errno = HDLC_INVALID_OP;
      return -1;
  } /* switch( ioctl_type ) */

  return 0;
} /* hdlc_ioctl() */

