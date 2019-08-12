/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ S L H C . C

GENERAL DESCRIPTION

  Routines to compress and uncompress tcp packets (for transmission
  over low speed serial lines).

  Copyright (c) 1989 Regents of the University of California.
  All rights reserved.

  Redistribution and use in source and binary forms are permitted
  provided that the above copyright notice and this paragraph are
  duplicated in all such forms and that any documentation,
  advertising materials, and other materials related to such
  distribution and use acknowledge that the software was developed
  by the University of California, Berkeley.  The name of the
  University may not be used to endorse or promote products derived
  from this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

   Van Jacobson (van@helios.ee.lbl.gov), Dec 31, 1989:
   - Initial distribution.


  modified for KA9Q Internet Software Package by
  Katie Stevens (dkstevens@ucdavis.edu)
  University of California, Davis
  Computing Services
   - 01-31-90  initial adaptation (from 1.19)
   PPP.05  02-15-90 [ks]
   PPP.08  05-02-90 [ks]  use PPP protocol field to signal compression
   PPP.15  09-90   [ks]   improve mbuf handling
   PPP.16  11-02   [karn] substantially rewritten to use NOS facilities

   - Feb 1991  Bill_Simpson@um.cc.umich.edu
       variable number of conversation slots
       allow zero or one slots
       separate routines
       status display


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/hc/src/ps_slhc.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/10/14    vl     Optimization of F3 messages
07/08/13    cx     Replace memcpy and memmove with memscpy and memsmove
09/24/09    ss     Fixed KW warnings.
09/21/09    ss     slhc_decompress_uncompressed_tcp: init ip, tcp headers
05/30/09    pp     slhc_compress: init ip, tcp hdrs before updating.
05/06/09    ss     SLHC De-coupling effort, getting the interface in sync with 
                   existing HC modules.
10/24/08    pp     Compile warning fixes.
10/22/08    dm     Modified TCP and IP4 headers to fix compiler warnings
08/15/05   mct     Fixed naming causing some issues w/C++ compilers.
05/20/05    ifk    Fixed compressor and decompressor to retain order of TCP
                   options.
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
10/13/04    vp     Replaced ntohip(), htonip() with ps_ip4_hdr_parse(),
                   ps_ip4_hdr_create(),and ntohtcp(), htontcp() with
                   tcp_hdr_parse(),tcp_hdr_create() and related changes.
08/20/04    usb    Fixed decompressor bug when tcp options are present for
                   SPECIAL_D and SPECIAL_I cases.
06/11/04    vp     Removed inclusion of internet.h and ps_ip.h. Included
                   ps_ip4.h. Changes for representation of IP addresses as
                   struct in_addr. Removed the use of macro ASSIGN.
11/11/03    aku/rc Passed parameter 'offset' in call to cksum().
11/03/03    mct    Optimized slhc_remember() so it only copies header and
                   doesn't need to call htontcp().
03/10/03    usb    Fixed slhc_init() to set recv_current slot to 0 instead
                   of 255. This prevents decompressor from using slot 255
                   when first uncompressed TCP packet is lost on the
                   connection without being detected.
02/02/03    usb    Added support for VJCH on all PPP devices.
01/31/03    usb    Added extra arg to ntohip() based on the new prototype
07/31/02    om     Added include of ps_iputil.h for IP utilities.
07/25/02    mvl    renamed file and did some cleanup.
02/21/02    pjb    Fix decompressor bug when TCP optlen != 0
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
08/08/01    aku    Added check for Null pointer in slhc_remember
07/23/01    pjb    Fixed lots of error checking code.  Save the  reserved bit
                   from the TCP header.
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter. Made changes to handle dsm_pushdown() returning
                   FALSE. Handled the case when htontcp() and htonip() return
                   NULL. Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK.
08/18/99    ak     Made fixes so compiles when FEATURE_DS turned off.
10/27/98    ldg    Moved "memory.h" include to below "customer.h" include.
01/27/98    ldg    Removed the DS_SLIM switch.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"

#include "ps_slhc.h"
#include "ps_system_heap.h"
#include "dsm.h"
#include "ps_ip4_hdr.h"
#include "ps_iputil.h"
#include "ps_tcp_hdr.h"
#include "ps_byte.h"
#include "msg.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                     DATA DECLARATIONS

===========================================================================*/

/* Bits in first octet of compressed packet */
/* flag bits for what changed in a packet */
#define NEW_C  0x40  
#define NEW_I  0x20
#define NEW_S  0x08
#define NEW_A  0x04
#define NEW_W  0x02
#define NEW_U  0x01

/* reserved, special-case values of above */
#define SPECIAL_I (NEW_S|NEW_W|NEW_U)          /* echoed interactive traffic */
#define SPECIAL_D (NEW_S|NEW_A|NEW_W|NEW_U)    /* unidirectional data */
#define SPECIALS_MASK (NEW_S|NEW_A|NEW_W|NEW_U)

#define TCP_PUSH_BIT 0x10

/*
 * data type and sizes conversion assumptions:
 *
 *  VJ code   KA9Q style     generic
 *  u_char    uint8              unsigned char     8 bits
 *  u_short   uint16            unsigned short   16 bits
 *  u_int       uint16            unsigned short   16 bits
 *  u_long    uint32             unsigned long    32 bits
 *  int          sint31             long                  32 bits
 */

/*
 * "state" data for each active tcp conversation on the wire.  This is
 * basically a copy of the entire IP/TCP header from the last packet
 * we saw from the conversation together with a small identifier
 * the transmit & receive ends of the line use to locate saved header.
 */
struct cstate 
{
  uint8  this_cstate;   /* connection id number (xmit) */
  struct cstate *next;  /* next in ring (xmit) */
  struct ip cs_ip;      /* ip/tcp hdr from most recent packet */
  struct tcp cs_tcp;
};

/*
 *  All the state data for one slhc instance.
     A pointer to this structure is used as the instance handle.
 */
struct slcompress 
{
  struct cstate *tstate;  /* transmit connection states (array)*/
  struct cstate *rstate;  /* receive connection states (array)*/

  uint8  tslot_limit;  /* highest transmit slot id (0-l)*/
  uint8  rslot_limit;  /* highest receive slot id (0-l)*/

  uint8  xmit_oldest;   /* oldest xmit in ring */
  uint8  xmit_current;  /* most recent xmit id */
  uint8  recv_current;  /* most recent rcvd id */

  uint8  flags;

#define SLF_TOSS  0x01  /* tossing rcvd frames until id received */

  uint32 sls_o_nontcp;        /* outbound non-TCP packets */
  uint32 sls_o_tcp;           /* outbound TCP packets */
  uint32 sls_o_uncompressed;  /* outbound uncompressed packets */
  uint32 sls_o_compressed;    /* outbound compressed packets */
  uint32 sls_o_searches;      /* searches for connection state */
  uint32 sls_o_misses;        /* times couldn't find conn. state */

  uint32 sls_i_uncompressed;  /* inbound uncompressed packets */
  uint32 sls_i_compressed;    /* inbound compressed packets */
  uint32 sls_i_error;         /* inbound error packets */
  uint32 sls_i_tossed;        /* inbound packets tossed because of error */
};


/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
static uint8 *encode
(
  uint8  *cp,
  uint16  n
);

static int32 decode
(
  struct dsm_item_s **bpp
);

static int32 slhc_decompress_compressed_tcp
(
  struct slcompress       *comp,
  struct dsm_item_s      **bpp
);

static int32 slhc_decompress_uncompressed_tcp
(
  struct slcompress       *comp,
  struct dsm_item_s      **bpp
);

static void initialize_context_list
(
  struct slcompress *comp
);

/*===========================================================================
MACRO slhc_memalloc
  Wrapper at SLHC Layer to access Modem Heap functions

DESCRIPTION
  Allocates the specified number of bytes of memory.
  Memory allocated should be freed with slhc_memfree().

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the newly allocated memory, or NULL if allocation failed.
===========================================================================*/
#define slhc_memalloc ps_system_heap_mem_alloc
 

/*===========================================================================
MACRO slhc_memfree
  Wrapper at SLHC Layer to access Modem Heap functions

DESCRIPTION
  Frees memory allocated with memalloc

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
#define slhc_memfree PS_SYSTEM_HEAP_MEM_FREE


/*===========================================================================

                     FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
STATIC FUNCTION ENCODE

DESCRIPTION
  Encodes the input 16 bit number n onto the 8 bit array cp.

DEPENDENCIES
  Non null arguments must be passed.

RETURN VALUE
  The updated pointer which now points to the next free location in the array.
  NULL in case of error.
===========================================================================*/
static uint8 *encode
(
  register uint8 *cp,
  uint16          n
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Sanity check */
  if(cp == NULL)
  {
    LOG_MSG_ERROR_0("encode: Null Argument");
    return NULL;
  }
  if(n >= 256 || n == 0)
  {
    *cp++ = 0;
    cp = put16(cp,n);
  }
  else
  {
    *cp++ = (uint8)n;
  }
  return cp;
}/* encode() */

/*===========================================================================
STATIC FUNCTION DECODE

DESCRIPTION
  Decodes the 16 bit number encoded onto the 8 bit array using the encode
  function.

DEPENDENCIES
  Non null values must be passed.

RETURN VALUE
  The decoded 16 bit number in case of success.
  -1 in case of error.
===========================================================================*/
static int32 decode
(
  struct dsm_item_s **bpp
)
{
  register int x;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Sanity check */
  if(bpp == NULL || *bpp == NULL)
  {
    LOG_MSG_ERROR_0("decode: Null Argument");
    return -1;
  }
  x = dsm_pull8( bpp);
  if(x == 0)
  {
    return dsm_pull16(bpp);  /* pull16 returns -1 on error */
  }
  else
  {
    return (int32)x;   /* -1 if PULLCHAR returned error */
  }
}/* decode() */

/*===========================================================================
STATIC FUNCTION INITIALIZE_CONTEXT_LIST

DESCRIPTION
  Initializes the existing context linked list.

DEPENDENCIES
  Non null arguments must be passed.

PARAMETERS
  struct slcompress *comp - SLHC handle.

RETURN VALUE
  None
===========================================================================*/
static void initialize_context_list
(
  struct slcompress *comp
)
{
  register struct cstate *ts;
  register uint16         i;

  /* Sanity check */
  if(comp == NULL)
  {
    LOG_MSG_ERROR_0("initialize_context_list: Null argument");
    return;
  }
  if((ts=comp->tstate) == NULL)
  {
    LOG_MSG_ERROR_0("initialize_context_list: Null tstate list");
    return;
  }
  
  comp->xmit_oldest  = 0;	
  comp->xmit_current = 255;
  comp->recv_current = 0;

  /* Refresh the circular linked list pointers */  
  /* Figure below depicts the linked list formed 
     ___________________________________________________                                    
    |                                                   |      
  [ 0 ] <- [ 1 ] <- [ 2 ] <- ..... <- [ tslot_limit ] <-|

  */  
  for(i = comp->tslot_limit; i > 0; --i)
  {
    ts[i].this_cstate = (uint8) i;
    ts[i].next = &(ts[i - 1]);
  }
  ts[0].next = &(ts[comp->tslot_limit]);
  ts[0].this_cstate = 0;

  return;
}/* initialize_context_list() */


/*===========================================================================
FUNCTION SLHC_ALLOC

DESCRIPTION
  Allocates and returns a handle to the SLHC instance, or NULL on error.

DEPENDENCIES
  Slots must be in the range of SLHC_MIN_SLOTS to SLHC_MAX_SLOTS.

PARAMETERS
  uint32 rslots - The number of connection slots to allocate for the receiver.
  uint32 tslots - The number of conneciton slots to allocate for the transmitter.

RETURN VALUE
  non-NULL handle on success
  NULL on failure
===========================================================================*/
slhc_handle_type slhc_alloc
(
  uint32 rslots,
  uint32 tslots
)
{
  struct slcompress *comp;
  void              *comp_temp; /* Warning fix:
                                   Type-punned pointer */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Verify arguments */
  if( rslots < SLHC_MIN_SLOTS || 
      rslots > SLHC_MAX_SLOTS ||
      tslots < SLHC_MIN_SLOTS ||
      tslots > SLHC_MAX_SLOTS )
  {
    LOG_MSG_ERROR_2("slhc_alloc: rslots/tslots out of range (%d/%d).",
                     rslots,tslots);
    return NULL;
  }

  LOG_MSG_INFO2_2( "VJHC rslots: %d, tslots: %d", rslots, tslots);  

  /* Memory for the slhc handle structure allocated on the heap */
  comp_temp = slhc_memalloc( sizeof( struct slcompress));
  
  /* Check if memory allocated successfully */
  if ( comp_temp == NULL )
  {
    LOG_MSG_ERROR_0("slhc_alloc: Memory allocation for handle failed.");
    return NULL;
  }
  comp = (struct slcompress *)comp_temp;
  /* Initialize the allocated memory */
  memset( comp, 0, sizeof( struct slcompress));

  /*------------------------------------------------------------
    If Rslots and Tslots within limits allocate memory and set 
    pointers for receiver and transmitter states.
  --------------------------------------------------------------*/

  /* Memory for rslots number of cstates allocated on the heap */
  comp->rstate = (struct cstate *)slhc_memalloc( rslots*sizeof(struct cstate));
  /* Check if memory allocated successfully */
  if ( comp->rstate == NULL )
  {
    /* Free the memory allocated for handle */
    slhc_memfree(comp_temp);
  
    LOG_MSG_ERROR_0("slhc_alloc: Memory allocation for rstate failed.");
    return NULL;
  }
  memset( comp->rstate, 0, rslots*sizeof( struct cstate));	
  comp->rslot_limit = (uint8)(rslots - 1);

  /* Memory for tslots number of cstates allocated on the heap */
  comp->tstate = (struct cstate *)slhc_memalloc( tslots*sizeof(struct cstate));
  /* Check if memory allocated successfully */
  if ( comp->tstate == NULL )
  {
    /* Free the memory allocated for handle and rslots */
    slhc_free((void *)comp);
 
    LOG_MSG_ERROR_0("slhc_alloc: Memory allocation for tstate failed.");
    return NULL;
  }
  memset( comp->tstate, 0, tslots*sizeof( struct cstate));
  comp->tslot_limit = (uint8)(tslots - 1);

  /* Initialize the context list */
  initialize_context_list(comp);
  
  /* Convert the slhc structure pointer to a void pointer and return */
  return (slhc_handle_type)comp;
}/* slhc_alloc() */


/*===========================================================================
FUNCTION SLHC_FREE

DESCRIPTION
  Frees the SLHC instance.  The handle should not be used after a call to
  slhc_free().

DEPENDENCIES
  A valid handle must be passed

PARAMETERS
  slhc_handle_type handle - SLHC handle.

RETURN VALUE
  None
===========================================================================*/
void slhc_free
(
  slhc_handle_type handle
)
{
  /* Cast the received void pointer */
  struct slcompress *comp = (struct slcompress *)handle;  
  
  if ( comp == NULL )
  {
    LOG_MSG_ERROR_0( "slhc_free: NULL handle passed" );
    return;
  }

  /* Free receiver states */
  if( comp->rstate != NULL )
  {    
    slhc_memfree(comp->rstate);
  }

  /* Free transmitter states */
  if( comp->tstate != NULL )
  {
    slhc_memfree(comp->tstate);	
  }

  /* Free slhc handle */
  /* handle used instead of comp to avoid warning: type-punned pointer */
  slhc_memfree(handle); 

  return;
}/* slhc_free() */


/*===========================================================================
FUNCTION SLHC_COMPRESS

DESCRIPTION
  Takes the SLHC instance handle and the packet to compress. Returns the
  compressed packet in bpp.

DEPENDENCIES
  A valid handle must be passed.
  Non null arguments must be passed.

PARAMETERS
  slhc_handle_type handle - SLHC handle.
  struct dsm_item_s **bpp - Packet to be compressed.
  int compress_cid - Flag which enables or disables connection id compression.
  slhc_packet_type_e_type *pkt_type - Packet type after compression.

RETURN VALUE
  -1 in case of failure.
  0 in case of success.
===========================================================================*/
int32 slhc_compress
(
  slhc_handle_type         handle,
  struct dsm_item_s      **bpp,
  int32                    compress_cid,
  slhc_packet_type_e_type *pkt_type
)
{  
  register struct cstate *ocs;
  register struct cstate *lcs;
  register struct cstate *cs;
  uint16          hlen = 0;
  register struct tcp *oth;
  register uint16 deltaS, deltaA;
  register uint16 changes = 0;
  uint8           new_seq[16];
  register uint8  *cp = new_seq;
  struct   tcp    th;
  struct   ip     iph;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Casting the received void pointer to slhc handle structure */
  struct slcompress *comp = (struct slcompress *)handle;

  /* Sanity Checks */
  if (comp == NULL         ||
      comp->tstate == NULL || 
      bpp == NULL          ||
     *bpp == NULL          ||
      pkt_type == NULL )
  {
    LOG_MSG_ERROR_0( "slhc_compress: NULL args" );
    return SLHC_FAILED;
  }

  /* Initialize new_seq array */
  memset((void *)new_seq, 0, sizeof(new_seq));

  /* Init ip, tcp headers */
  memset((void *)&th, 0, sizeof(th));
  memset((void *)&iph, 0, sizeof(iph));

  /* Set the default pkt_type to be returned in case of error. */
  *pkt_type = SLHC_PACKET_TYPE_ERROR;

  lcs = ocs = &(comp->tstate[comp->xmit_oldest]);
  cs = lcs->next;
  
  /* Extract IP header */
  if(ps_ip4_hdr_parse(*bpp,&hlen,&iph) == FALSE)
  {
    LOG_MSG_ERROR_0("slhc_compress: Could not parse IP header.");
    return SLHC_FAILED;
  }

  /* Bail if this packet isn't TCP, or is an IP fragment */
  if(iph.protocol != (uint8)PS_IPPROTO_TCP || iph.offset != 0 || iph.mf)
  {
    /* Send as regular IP */
    if(iph.protocol != (uint8)PS_IPPROTO_TCP)
    {
      comp->sls_o_nontcp++;
    }
    else
    {
      comp->sls_o_tcp++;
    }

    *pkt_type = SLHC_PACKET_TYPE_IP;
    return SLHC_SUCCESS;
  }

  /* Extract TCP header */
  if(tcp_hdr_parse(*bpp,&hlen,&th) == FALSE)
  {
    LOG_MSG_ERROR_0("slhc_compress: Could not parse TCP header.");
    return SLHC_FAILED;
  }

  /*  Bail if the TCP packet isn't `compressible' (i.e., ACK isn't set or
   *  some other control bit is set).
   */
  if(th.syn || th.fin || th.rst || !th.ack_flag)
  {
    /* TCP connection stuff; send as regular IP */
    comp->sls_o_tcp++;

    *pkt_type = SLHC_PACKET_TYPE_IP;
    return SLHC_SUCCESS;
  }

  /*
   * Packet is compressible -- we're going to send either a
   * COMPRESSED_TCP or UNCOMPRESSED_TCP packet.  Either way,
   * we need to locate (or create) the connection state.
   *
   * States are kept in a circularly linked list with
   * xmit_oldest pointing to the end of the list.  The
   * list is kept in lru order by moving a state to the
   * head of the list whenever it is referenced.  Since
   * the list is short and, empirically, the connection
   * we want is almost always near the front, we locate
   * states via linear search.  If we don't find a state
   * for the datagram, the oldest state is (re-)used.
   */
  for ( ; ; )
  {
    if( iph.source.ps_s_addr == cs->cs_ip.source.ps_s_addr
     && iph.dest.ps_s_addr == cs->cs_ip.dest.ps_s_addr
     && th.source == cs->cs_tcp.source
     && th.dest == cs->cs_tcp.dest)
    {
      goto found;
    }
      
    /* if current equal oldest, at end of list */
    if ( cs == ocs )
    {
      break;
    }
    lcs = cs;
    cs = cs->next;
    comp->sls_o_searches++;
  }
  
  /*
   * Didn't find it -- re-use oldest cstate.  Send an
   * uncompressed packet that tells the other side what
   * connection number we're using for this conversation.
   *
   * Note that since the state list is circular, the oldest
   * state points to the newest and we only need to set
   * xmit_oldest to update the lru linkage.
   */
  comp->sls_o_misses++;
  comp->xmit_oldest = lcs->this_cstate;
  LOG_MSG_INFO3_0( "TX Uncompressed: Re-use not found");
  goto uncompressed;

  found:  	
    /*
    * Found it -- move to the front on the connection list.
    */
    if(lcs == ocs)
    {
      /* found at most recently used */
    }
    else if (cs == ocs)
    {
      /* found at least recently used */
      comp->xmit_oldest = lcs->this_cstate;
    }
    else
    {
      /* more than 2 elements */
      lcs->next = cs->next;
      cs->next = ocs->next;
      ocs->next = cs;
    }
	
    /*
    * Make sure that only what we expect to change changed.
    * Check the following:
    * IP protocol version, header length & type of service.
    * The "Don't fragment" bit.
    * The time-to-live field.
    * The TCP header length.
    * IP options, if any.
    * TCP options, if any.
    * If any of these things are different between the previous &
    * current datagram, we send the current datagram `uncompressed'.
    */
    oth = &cs->cs_tcp;

    if(iph.version != cs->cs_ip.version || iph.optlen   != cs->cs_ip.optlen
           || iph.tos	 != cs->cs_ip.tos	  || iph.df != cs->cs_ip.df
           || iph.ttl	 != cs->cs_ip.ttl	  || th.optlen	  != cs->cs_tcp.optlen
           || ((iph.optlen > 0) && (iph.optlen <= IP_MAXOPT) &&
               (memcmp(iph.options,cs->cs_ip.options,iph.optlen)) != 0)
           || ((th.optlen > 0) && (th.optlen <= TCP_MAXOPT) && 
               (memcmp(th.options,cs->cs_tcp.options,th.optlen)) != 0))
    {
      LOG_MSG_INFO3_0( "TX Uncompressed: The big OR");
      goto uncompressed;
    }
    /*
    * Figure out which of the changing fields changed.  The
    * receiver expects changes in the order: urgent, window,
    * ack, seq (the order minimizes the number of temporaries
    * needed in this section of code).
    */
    if(th.urg)
    {
      deltaS = th.up;
      cp = encode(cp,deltaS);
      changes |= NEW_U;
    }
    else if(th.up != oth->up)
    {
      /* argh! URG not set but urp changed -- a sensible
      * implementation should never do this but RFC793
      * doesn't prohibit the change so we have to deal
      * with it. */

      LOG_MSG_INFO3_0( "TX Uncompressed: URP Changed" );
      goto uncompressed;
    }
    if((deltaS = (uint16)(th.wnd - oth->wnd)) != 0)
    {
      cp = encode(cp,deltaS);
      changes |= NEW_W;
    }
    if((deltaA = (uint16)(th.ack - oth->ack)) != 0L)
    {
      if((th.ack - oth->ack) > 0x0000ffff)
      {
        LOG_MSG_INFO3_0( "TX Uncompressed: deltaA check" );
        goto uncompressed;
      }
      cp = encode(cp,deltaA);
      changes |= NEW_A;
    }
    if((deltaS = (uint16)(th.seq - oth->seq)) != 0L)
    {
      if((th.seq - oth->seq) > 0x0000ffff)
      {
        LOG_MSG_INFO3_0( "TX Uncompressed: deltaS check" );
        goto uncompressed;
      }
      cp = encode(cp,deltaS);
      changes |= NEW_S;
    }

    switch(changes)
    {
      case 0:
        /* Nothing changed. If this packet contains data and the
        * last one didn't, this is probably a data packet following
        * an ack (normal on an interactive connection) and we send
        * it compressed.  Otherwise it's probably a retransmit,
        * retransmitted ack or window probe.  Send it uncompressed
        * in case the other side missed the compressed version.
        */
        if(iph.length != cs->cs_ip.length && cs->cs_ip.length == hlen)
        {
          break;
        }
        LOG_MSG_INFO3_0( "TX Uncompressed: Re-TX or probe" );
        goto uncompressed;
	
      case SPECIAL_I:
      case SPECIAL_D:
        /* actual changes match one of our special case encodings --
        * send packet uncompressed.
        */
        LOG_MSG_INFO3_0( "TX Uncompressed: Special Encoding" );
        goto uncompressed;
	
      case NEW_S|NEW_A:
        if(deltaS == deltaA &&
           deltaS == cs->cs_ip.length - hlen)
        {
          /* special case for echoed terminal traffic */
          changes = SPECIAL_I;
          cp = new_seq;
        }
        break;
	
      case NEW_S:
        if(deltaS == cs->cs_ip.length - hlen)
        {
          /* special case for data xfer */
          changes = SPECIAL_D;
          cp = new_seq;
        }
        break;

      default:
        break;
    }
    deltaS = (uint16)(iph.id - cs->cs_ip.id);
    if(deltaS != 1)
    {
      cp = encode(cp,deltaS);
      changes |= NEW_I;
    }
    if(th.psh)
    {
      changes |= TCP_PUSH_BIT;
    }
    /* Grab the cksum before we overwrite it below.  Then update our
    * state with this packet's header.
    */
    deltaA = th.checksum;
    memscpy((char*)&(cs->cs_ip),sizeof(cs->cs_ip),(char*)&(iph),sizeof(iph));
    memscpy((char*)&(cs->cs_tcp),sizeof(cs->cs_tcp),(char*)&(th),sizeof(th));
    /* We want to use the original packet as our compressed packet.
    * (cp - new_seq) is the number of bytes we need for compressed
    * sequence numbers.	In addition we need one byte for the change
    * mask, one for the connection id and two for the tcp checksum.
    * So, (cp - new_seq) + 4 bytes of header are needed.
    */
    deltaS = (uint16)(cp - new_seq);
    /*-----------------------------------------------------------------------
              Remove the ip and TCP header
    -----------------------------------------------------------------------*/    
    if(dsm_pullup(bpp, NULL, hlen) != hlen)
    { 
      LOG_MSG_ERROR_0("slhc_compress: Pullup IP/TCP headers failed.");
      return SLHC_FAILED;
    }

    if(compress_cid == 0 || comp->xmit_current != cs->this_cstate)
    {
      if (dsm_pushdown(bpp, NULL, deltaS + 4, 
                            DSM_DS_SMALL_ITEM_POOL) < deltaS+4)
      {
        dsm_free_packet(bpp);
        LOG_MSG_ERROR_0("slhc_compress: Pushdown failed.");
        return SLHC_FAILED;
      }
      cp = (*bpp)->data_ptr;
      *cp++ = (uint8)(changes | NEW_C);
      *cp++ = cs->this_cstate;
      comp->xmit_current = cs->this_cstate;
    }
    else
    {
      if (dsm_pushdown(bpp, NULL, deltaS + 3, 
                                  DSM_DS_SMALL_ITEM_POOL) < deltaS+3)
      {
        dsm_free_packet(bpp);
        LOG_MSG_ERROR_0("slhc_compress: Pushdown failed.");
        return SLHC_FAILED;
      }
      cp = (*bpp)->data_ptr;
      *cp++ = (uint8)changes;
    }
    cp = put16(cp,(uint16)deltaA);	/* Write TCP checksum */
    memscpy(cp,(*bpp)->size - (cp-(*bpp)->data_ptr),new_seq,(uint16)deltaS);  /* Write list of deltas */
    comp->sls_o_compressed++;
	
    *pkt_type = SLHC_PACKET_TYPE_COMPRESSED_TCP;
    return SLHC_SUCCESS;

  uncompressed:
    /* Update connection state cs & send uncompressed packet (i.e.,
    * a regular ip/tcp packet but with the 'conversation id' we hope
    * to use on future compressed packets in the protocol field).
    */
  LOG_MSG_INFO2_0( "TXing Uncompressed Packet");
  iph.protocol = cs->this_cstate;
  memscpy((char*)&(cs->cs_ip),sizeof(cs->cs_ip),(char*)&(iph),sizeof(iph));
  memscpy((char*)&(cs->cs_tcp),sizeof(cs->cs_tcp),(char*)&(th),sizeof(th));
  comp->xmit_current = cs->this_cstate;
  comp->sls_o_uncompressed++;

  if(dsm_pullup(bpp, NULL, (uint16)(iph.ihl*4)) != iph.ihl*4)
  {
    LOG_MSG_ERROR_0("slhc_compress: Pullup IP header failed.");
    return SLHC_FAILED;
  }
  if (*bpp == NULL)
  {
    LOG_MSG_ERROR_0("slhc_compress: Null dsm item.");
    return SLHC_FAILED;
  }
  if(ps_ip4_hdr_create(bpp, &iph, IP_CS_OLD) == FALSE)
  {
    LOG_MSG_ERROR_0("slhc_compress: Could not create IP header.");
    return SLHC_FAILED;
  }

  *pkt_type = SLHC_PACKET_TYPE_UNCOMPRESSED_TCP;
  return SLHC_SUCCESS;
} /* slhc_compress() */


/*===========================================================================
FUNCTION SLHC_DECOMPRESS

DESCRIPTION
  This function accepts the SLHC handle and the compressed packet and returns 
  the compressed packet in bpp. 

DEPENDENCIES
  A valid handle must be passed.
  A valid packet type must be passed.
  Non null packet should be sent in all cases except when the packet type is 
  SLHC_PACKET_TYPE_ERROR

PARAMETERS
  slhc_handle_type handle - SLHC handle.
  struct dsm_item_s **bpp - Packet to be decompressed.
  slhc_packet_type_e_type *pkt_type - Packet type.

RETURN VALUE
  -1 in case of error (e.g. invalid arguments, decompression fails or pkt_type
  equals SLHC_PACKET_TYPE_ERROR).
  0 if the packet is returned as it is.
  Length of the decompressed packet if decompression was successful
===========================================================================*/
int32 slhc_decompress
(
  slhc_handle_type         handle,
  struct dsm_item_s      **bpp,
  slhc_packet_type_e_type  pkt_type
)
{   
  int ret = SLHC_FAILED;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Casting the received void pointer to slhc handle structure */
  struct slcompress *decomp = (struct slcompress *)handle;

  switch(pkt_type)
  {
    case SLHC_PACKET_TYPE_IP:
      /* Return the packet as it is */
      ret = SLHC_SUCCESS;
      break;

    case SLHC_PACKET_TYPE_ERROR:
      /* Sanity check */
      if (decomp == NULL) 
      {
        LOG_MSG_ERROR_0( "slhc_decomp: NULL handle");
        return SLHC_FAILED;
      }
      /* Setting toss flag */
      decomp->flags |= SLF_TOSS;     
      ret = SLHC_SUCCESS;
      break;
      
    case SLHC_PACKET_TYPE_COMPRESSED_TCP:
      ret = slhc_decompress_compressed_tcp( decomp, bpp );
      break;
      
    case SLHC_PACKET_TYPE_UNCOMPRESSED_TCP:
      ret = slhc_decompress_uncompressed_tcp( decomp, bpp );
      break;
    
    default:
      LOG_MSG_ERROR_1( "slhc_decomp: Unknown packet type %d", pkt_type);
      break;
  } 

  return ret;
}/* slhc_decompress() */


/*===========================================================================
STATIC FUNCTION SLHC_DECOMPRESS_COMPRESSED_TCP

DESCRIPTION
  This function handles the decompression of a SLHC_PACKET_TYPE_COMPRESSED_TCP 
  packet. The decompressed packet is returned in bpp.

DEPENDENCIES
  A valid handle must be passed.

PARAMETERS
  struct slcompress *comp - SLHC handle.
  struct dsm_item_s **bpp - Packet to be uncompressed.

RETURN VALUE
  -1 in case of error (e.g., invalid arguments or decompression fails)
  Length of the decompressed packet if decompression successful.
===========================================================================*/
static int32 slhc_decompress_compressed_tcp
(
  struct slcompress       *comp,
  struct dsm_item_s      **bpp
)
{
  register int               changes;
  long                       x;
  register struct tcp	    *thp;
  register struct cstate    *cs;
  int                        len;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Null checks */
  if ( comp == NULL || bpp == NULL || *bpp == NULL )
  {
    LOG_MSG_ERROR_0( "slhc_decomp: NULL args" );
    return SLHC_FAILED;
  }

  /* We've got a compressed packet; read the change byte */
  comp->sls_i_compressed++;
  if(dsm_length_packet(*bpp) < 3)
  {
    comp->sls_i_error++;
    LOG_MSG_ERROR_0("slhc_decomp: Compressed packet has insufficient size");
    return SLHC_FAILED;
  }
  changes = dsm_pull8( bpp);
  if(changes & NEW_C)
  {
    /* Make sure the state index is in range, then grab the state.
     * If we have a good state index, clear the 'discard' flag.
     */
    x = dsm_pull8( bpp);
    if(x < 0 || x > comp->rslot_limit)
    {
      goto bad;
    }

    comp->flags &=~ SLF_TOSS;
    comp->recv_current = (uint8)x;
  }
  else
  {
    /* this packet has an implicit state index.  If we've
    * had a line error since the last time we got an
    * explicit state index, we have to toss the packet. */
    if(comp->flags & SLF_TOSS)
    {
      comp->sls_i_tossed++;
      LOG_MSG_ERROR_0("slhc_decomp: Toss flag set");
      return SLHC_FAILED;
    }
  }
  cs  = &comp->rstate[comp->recv_current];
  thp = &cs->cs_tcp;

  if((x = dsm_pull16(bpp)) == -1)  /* Read the TCP checksum */
  {
    goto bad;
  }
  thp->checksum = (uint16)x;

  thp->psh = (changes & TCP_PUSH_BIT) ? 1 : 0;

  switch(changes & SPECIALS_MASK)
  {
    case SPECIAL_I:   /* Echoed terminal traffic */
    {
      register uint16 i;

      i	= (uint16)(cs->cs_ip.length -
                  (IPLEN + cs->cs_ip.optlen + TCPLEN + cs->cs_tcp.optlen));
      thp->ack += i;
      thp->seq += i;
    }
    break;

    case SPECIAL_D: 	/* Unidirectional data */
      thp->seq += cs->cs_ip.length -
                      (IPLEN + cs->cs_ip.optlen + TCPLEN + cs->cs_tcp.optlen);
      break;

    default:
      if(changes & NEW_U)
      {
        thp->urg = 1;
        if((x = decode(bpp)) == -1)
        {
          goto bad;
        }
        thp->up = (uint16)x;
      }
      else
      {
        thp->urg = 0;
      }
      if(changes & NEW_W)
      {
        if((x = decode(bpp)) == -1)
        {
          goto bad;
        }
        thp->wnd += (uint16)x;
      }
      if(changes & NEW_A)
      {
        if((x = decode(bpp)) == -1)
        {
          goto bad;
        }
        thp->ack += (unsigned int)x;
      }
      if(changes & NEW_S)
      {
        if((x = decode(bpp)) == -1)
        {
          goto bad;
        }
        thp->seq += (unsigned int)x;
      }
      break;
  }
  if(changes & NEW_I)
  {
    if((x = decode(bpp)) == -1)
    {
      goto bad;
    }
    cs->cs_ip.id += (uint16)x;
  }
  else
  {
    cs->cs_ip.id++;
  }

  /*
   * At this point, bpp points to the first byte of data in the
   * packet.  Put the reconstructed TCP and IP headers back on the
   * packet.  Recalculate IP checksum (but not TCP checksum).
   */
  len = (int)( dsm_length_packet(*bpp) + IPLEN + cs->cs_ip.optlen + 
                                        TCPLEN + thp->optlen );

  cs->cs_ip.length = (uint16)len;

  if(tcp_hdr_create(bpp,thp,NULL) == FALSE)
  { 
    LOG_MSG_ERROR_0("slhc_decomp: Could not create tcp header");
    return SLHC_FAILED;
  }
  if (*bpp == NULL)
  {
    LOG_MSG_ERROR_0("slhc_decomp: Null dsm item");
    return SLHC_FAILED;
  }
  if(ps_ip4_hdr_create(bpp,&(cs->cs_ip),IP_CS_NEW) == FALSE)
  {
    LOG_MSG_ERROR_0("slhc_decomp: Could not create ip header");
    return SLHC_FAILED;
  }
  return len;

  bad:
    comp->sls_i_error++;
    /* Setting toss flag */
    comp->flags |= SLF_TOSS;
    return SLHC_FAILED;
} /* slhc_decompress_compressed_tcp() */


/*===========================================================================
STATIC FUNCTION SLHC_DECOMPRESS_UNCOMPRESSED_TCP

DESCRIPTION
  This function handles the decompression of a SLHC_PACKET_TYPE_UNCOMPRESSED_TCP
  packet.

DEPENDENCIES
  A valid handle must be passed.

PARAMETERS
  struct slcompress *comp - SLHC handle.
  struct dsm_item_s **bpp - Packet to be uncompressed.

RETURN VALUE
  -1 in case of error (e.g. invalid arguments or decompression fails)
  Length of the decompressed packet if decompression successful.
===========================================================================*/
static int32 	slhc_decompress_uncompressed_tcp
(
  struct slcompress       *comp,
  struct dsm_item_s      **bpp
)
{
  register struct cstate    *cs;
  struct		  ip	     iph;
  struct		  tcp	     th;
  uint16				     len;
  uint16				     hdrlen;
  uint16		             tcp_hdrlen = 0;
  int			 		     slot;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Null checks */
  if ( comp == NULL || bpp == NULL || *bpp == NULL )
  {
    LOG_MSG_ERROR_0( "slhc_decomp: NULL args" );
    return SLHC_FAILED;
  }

  /* Init ip, tcp headers */
  memset((void *)&th, 0, sizeof(th));
  memset((void *)&iph, 0, sizeof(iph));

  /* Sneak a peek at the IP header's IHL field to find its length */
  hdrlen = ((*bpp)->data_ptr[0] & 0xf) << 2;
  if(hdrlen < IPLEN)
  {
    /* The IP header length field is too small to be valid */
    comp->sls_i_error++;

    /* Setting toss flag */
    comp->flags |= SLF_TOSS;

    LOG_MSG_ERROR_0("slhc_decomp: IP header has insufficient size.");
    return SLHC_FAILED;
  }
  len = (uint16)dsm_length_packet(*bpp);  /* Actual length of whole packet */
  hdrlen = 0;
  if(ps_ip4_hdr_parse(*bpp,&hdrlen,&iph) == FALSE)
  {
    comp->sls_i_error++;

    /* Setting toss flag */
    comp->flags |= SLF_TOSS;

    return SLHC_FAILED;
  }

  /*-------------------------------------------------------------------------
	Remove the ip header
  -------------------------------------------------------------------------*/
  if(dsm_pullup(bpp, NULL, hdrlen) != hdrlen)
  {
    comp->sls_i_error++;

    /* Setting toss flag */
    comp->flags |= SLF_TOSS;

    return SLHC_FAILED;
  }
  /* Verify indicated length <= actual length */
  if(iph.length > len)
  {
    /* Packet has been truncated, or header is garbage */
    comp->sls_i_error++;

    /* Setting toss flag */
    comp->flags |= SLF_TOSS;

    LOG_MSG_ERROR_0("slhc_decomp: Packet length less than specified in"
                    " IP header");
    return SLHC_FAILED;
  }
  /* Verify conn ID */
  slot = iph.protocol;
  if(slot > comp->rslot_limit)
  {
    /* Out of range */
    comp->sls_i_error++;

    /* Setting toss flag */
    comp->flags |= SLF_TOSS;

    LOG_MSG_ERROR_0("slhc_decomp: Connection id greater than limit.");
    return SLHC_FAILED;
  }
  iph.protocol = (uint8)PS_IPPROTO_TCP;  /* Replace conn ID with IPPROTO_TCP */

  /* Extract TCP header and replace both headers
   * Neither header checksum is recalculated
   * Only copy header/no removal for ntohtcp()
   */
  if(tcp_hdr_parse(*bpp,&tcp_hdrlen,&th) == FALSE)
  {
    comp->sls_i_error++;

    /* Setting toss flag */
    comp->flags |= SLF_TOSS;

    return SLHC_FAILED;
  }

  if(ps_ip4_hdr_create(bpp,&iph,IP_CS_OLD) == FALSE)
  {
    comp->sls_i_error++;

    /* Setting toss flag */
    comp->flags |= SLF_TOSS;

    return SLHC_FAILED;
  }

  /* Checksum IP header (now that protocol field is TCP again) */
  if(cksum(NULL,*bpp,hdrlen,0) != 0)
  {
    /* Bad IP header checksum; discard */
    comp->sls_i_error++;

    /* Setting toss flag */
    comp->flags |= SLF_TOSS;

    LOG_MSG_ERROR_0("slhc_decomp: Invalid IP header checksum.");
    return SLHC_FAILED;
  }

  /* Update local state */
  comp->recv_current = (uint8)slot;
  cs				 = &comp->rstate[slot];
  comp->flags		&=~ SLF_TOSS;

  memscpy((char*)&(cs->cs_ip),sizeof(cs->cs_ip),(char*)&(iph),sizeof(iph));
  memscpy((char*)&(cs->cs_tcp),sizeof(cs->cs_tcp),(char*)&(th),sizeof(th));
  comp->sls_i_uncompressed++;
  return len;
}/* slhc_decompress_uncompressed_tcp() */


/*===========================================================================
FUNCTION SLHC_IOCTL

DESCRIPTION
  Sets or gets SLHC parameters.

DEPENDENCIES
  A valid handle and ioctl_type must be passed.

PARAMETERS
  slhc_handle_type handle - SLHC handle.
  slhc_ioctl_e_type ioctl_type - IOCTL type.
  slhc_ioctl_u_type *ioctl_data - Depending on the ioctl type this is used to 
                                  return information or retrieve information.

RETURN VALUE
  -1 in case of error (e.g., invalid arguments)
   0 in case of success
===========================================================================*/
int32 slhc_ioctl
(
  slhc_handle_type         handle,		   /* Instance of SLHC	           */
  slhc_ioctl_e_type        ioctl_type,	   /* The IOCTL                    */
  const slhc_ioctl_u_type *ioctl_data 	   /* Associated ioctl information */
)
{
  int    ret;
  uint8  rslots;
  uint8  tslots;
  void  *rstate_temp;
  void  *tstate_temp;

  /* Casting the received void pointer to slhc handle structure */
  struct slcompress *comp = (struct slcompress *)handle;
  
  /* Sanity Check */
  if (comp == NULL)
  {
    LOG_MSG_ERROR_0( "slhc_ioctl: NULL handle passed" );
    return SLHC_FAILED;
  }

  ret = SLHC_FAILED;
  switch( ioctl_type )
  {
    case SLHC_IOCTL_CHANNEL_REINITIALIZATION:
      /* Validate ioctl_data */		
      if(ioctl_data == NULL)
      {
        LOG_MSG_INFO2_0("slhc_ioctl: NULL ioctl_data passed");
        return SLHC_FAILED;
      }	  	
      /* Extract rslots and tslots from ioctl_data */
      rslots = (uint8)ioctl_data->options.rslots;
      tslots = (uint8)ioctl_data->options.tslots;

      /* Verify arguments */
      if( rslots < SLHC_MIN_SLOTS || 
          rslots > SLHC_MAX_SLOTS ||
          tslots < SLHC_MIN_SLOTS ||
          tslots > SLHC_MAX_SLOTS )
      {
        LOG_MSG_ERROR_2("slhc_ioctl: rslots/tslots out of range (%d/%d).",
                         rslots,tslots);
        return SLHC_FAILED;
      }

      /* Check if rslots and tslots value has changed */
      if( rslots != comp->rslot_limit+1 || 
          tslots != comp->tslot_limit+1 )
      { 
        /* Memory for rslots number of cstates allocated on the heap */
        rstate_temp = slhc_memalloc( rslots*sizeof(struct cstate));		
        
        /* Check if memory allocated successfully */
        if ( rstate_temp == NULL )
        { 	   
          LOG_MSG_ERROR_0("slhc_ioctl: Memory allocation for rstate failed.");
          return SLHC_FAILED;
        }
        memset( rstate_temp, 0, rslots*sizeof( struct cstate));        
  
        /* Memory for tslots number of cstates allocated on the heap */
        tstate_temp = slhc_memalloc( tslots*sizeof(struct cstate));		
        
        /* Check if memory allocated successfully */
        if ( tstate_temp == NULL )
        {
          /* Free memory allocated for rslots */
          slhc_memfree(rstate_temp);

          LOG_MSG_ERROR_0("slhc_ioctl: Memory allocation for tstate failed.");
          return SLHC_FAILED;
        }
        memset( tstate_temp, 0, tslots*sizeof( struct cstate));        

        /* New memory allocation successful. */
        /* Now free the existing memory and change rstate and tstate pointers
                    in comp */
        if( comp->rstate != NULL )
        {
          slhc_memfree(comp->rstate);
        }
        /* Free transmitter states */
        if( comp->tstate != NULL )
        {
          slhc_memfree(comp->tstate); 
        }

        comp->rstate = (struct cstate *)rstate_temp;
        comp->rslot_limit = (uint8)(rslots - 1);

        comp->tstate = (struct cstate *)tstate_temp;
        comp->tslot_limit = (uint8)(tslots - 1);
      }

      /* Reinitialize contexts */
      initialize_context_list(comp);	  
      ret = SLHC_SUCCESS;
      break;
    
    case SLHC_IOCTL_CONTEXT_REINITIALIZATION:
      initialize_context_list(comp);
      ret = SLHC_SUCCESS;  	
      break;
      
    case SLHC_IOCTL_SET_TOSS:
      LOG_MSG_INFO2_0( "Setting Toss flag");
      comp->flags |= SLF_TOSS;
      ret = SLHC_SUCCESS;
      break;

    default:
      LOG_MSG_ERROR_0("slhc_ioctl: Invalid IOCTL type passed");
      break;		
  }

  return ret;
}/* slhc_ioctl() */
