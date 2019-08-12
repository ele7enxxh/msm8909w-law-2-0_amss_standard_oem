/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    I P   I N T E R N A L   U T I L I T Y

                             F U N C T I O N S


GENERAL DESCRIPTION
  IP header conversion routines
  Copyright 1991 Phil Karn, KA9Q


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iputil.c_v   1.2   31 Jan 2003 18:16:46   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_iputil.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/15   cx      Fix issue in checksum subtraction
10/20/14   xc      Added cksum_update
12/12/12   ds      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
06/24/09   am      Renamed hton/ntoh macros with ps_ prefix.
10/24/08   pp      Fixed compile warning observed in OFFTARGET build.
09/30/08   pp      Deprecated word from assembly block is re-worded w/ _dots_.
05/11/05   sv      Lint changes.
01/27/05   ssh     Changed include file name dsbyte.h to ps_byte.h
10/13/04   vp      Removed inclusion of ps_iphdr.h and included ps_ip4_hdr.h.
                   Removed ntohip() and htonip() functions. Added
                   buffer_cksum() function.
06/11/04   vp      Removed inclusion of psglobal.h and internet,h. Included
                   ps_in.h and ps_pkt_info.h. Changed byte ordering macros to
                   their PS versions. Changes for representation of IP
                   addresses as struct ps_in_addr.
11/11/03   rc/aku  Added 'offset' parameter to cksum() for IPv6 support.
                   Modified cksum() to handle the passed in offset.
08/18/03    sv     Changed code to use IPv6 addressess in TCP/UDP
                   pseudo header for checksum calculations.
01/31/03   usb     Added an extra arg to ntohip() to support hdr copying only
09/10/02   aku     Removed global Ip_addr.
08/05/02   usb     Moved lcsum() from ps_utils to this file.
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module.
07/23/01   pjb     Save the reserved bit of the TCP header.
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
12/14/98   ldg     Removed anforward-slash asterisk from inside a comment.
01/27/98   ldg     Removed the DS_SLIM switch.
06/25/97   jgr     Added ifdef FEATURE_DS over whole file
11/14/96   jjw     Changes to support Unwired Planet (UDP operation)
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "err.h"

#include "dsm.h"
#include "ps_iputil.h"
#include "ps_pkt_info.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*===========================================================================

                          INTERNAL FORWARD DECLARATIONS

===========================================================================*/

static uint16 eac
(
  register uint32 sum
);

/*===========================================================================

FUNCTION EAC

DESCRIPTION
  This function will perform end-around-carry adjustment

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

uint16 eac
(
  register uint32 sum     /* Carries in high order 16 bits */
)
{
  register uint16 csum;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while((csum = (uint16)(sum >> 16)) != 0)
  {
    sum = csum + (sum & 0xffffL);
  }
  return (uint16) (sum & 0xffffL);        /* Chops to 16 bits */
}

/*===========================================================================

FUNCTION     BUFFER_CKSUM()

DESCRIPTION  Compute the cksum of the passed buffer of data.

DEPENDENCIES Count should be even. Buffer has a uint16 boundary.

PARAMETERS   uint16*          - Buffer to be checksummed.
             uint16*          - Count of number of bytes in the buffer.

RETURN VALUE uint16  - Checksum.

SIDE EFFECTS None.
===========================================================================*/
uint16 buffer_cksum
(
  uint16          *buf,
  uint16          count
)
{
  uint32 sum = 0L;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(buf == NULL || (count & 1))
  {
    LOG_MSG_ERROR_0("NULL buffer or Invalid odd count passed to buffer_checksum()"
                    " as parameter");
    return 0;
  }

  sum = lcsum(buf, (count >> 1));
  return (uint16)(~eac(sum) & 0xffff);
}/* buffer_cksum() */

/*===========================================================================

FUNCTION CHKSUM

DESCRIPTION
  This function will checksum the passed memory item.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

uint16 cksum
(
  pseudo_header_type   *ph,
  register struct      dsm_item_s *m,
  uint16               len,
  uint16               offset
)
{
  register uint32 sum = 0L;
  register uint32 csum;
  register uint16 cnt, total;
  register uint8  *up;
  uint16          csum1;
  int             swap = FALSE;
  int             i;  /* loop variable */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sum pseudo-header, if present */

  if(ph != NULL)
  {
    for(i=0 ; i< 8; i++)
    {
      sum += ps_ntohs(ph->source.ps_s6_addr16[i]);
      sum += ps_ntohs(ph->dest.ps_s6_addr16[i]);
    }
    sum += ph->protocol;
    sum += ph->length;
  }

  /*-------------------------------------------------------------------------
    Traverse till we get to the item to checksum that is at offset bytes from
    the start of the packet.
  -------------------------------------------------------------------------*/
  while(m != NULL && offset >= m->used)
  {
    offset -= m->used;
    m = m->pkt_ptr;
  }

  /* Now do each mbuf on the chain */
  for(total = 0; m != NULL && total < len; m = m->pkt_ptr)
  {
    if (offset > 0)
    {
      cnt  = MIN(m->used - offset, len - total);
      up   = m->data_ptr + offset;
    }
    else
    {
      cnt  = MIN(m->used, len - total);
      up   = m->data_ptr;
    }
    csum = 0L;

    /* pointer dependent !!! */
    /**/
    if((long)up & 1)
    /**/
    {
      /* Handle odd leading byte */
      if(swap)
      {
        csum = *up++;
      }
      else
      {
        csum = (uint16)*up++ << 8;
      }
      cnt--;
      swap = !swap;
    }
    if(cnt > 1)
    {
      /* Have the primitive checksumming routine do most of
       * the work. At this point, up is guaranteed to be on
       * a short boundary
       */
      csum1 = lcsum((uint16 *)up, cnt >> 1);
      if(swap)
      {
        csum1 = (uint16)((csum1 << 8) | (csum1 >> 8));
      }
      csum += csum1;
    }
    /* Handle odd trailing byte */
    if(cnt & 1)
    {
      if(swap)
      {
        csum += up[--cnt];
      }
      else
      {
        csum += (uint16)up[--cnt] << 8;
      }
      swap = !swap;
    }
    sum   += csum;
    total += m->used - offset;
    /* Reset the offset field */
    offset = 0;
  }
  /* Do final end-around carry, complement and return */
  return (uint16)(~eac(sum) & 0xffff);
}


/*===========================================================================

FUNCTION CKSUM_UPDATE

DESCRIPTION
  This function calculates the new IP checksum given an old checksum and 
  old and new pseudo headers

  @param[in] old_ph   Old pseudo header
  @param[in] new_ph   New pseudo header
  @param[in] old_cksum  Old checksum

DEPENDENCIES
  None

RETURN VALUE
  16-bit checksum

SIDE EFFECTS
  None
===========================================================================*/
uint16 cksum_update
(
  pseudo_header_type   *old_ph,  /* old pseudo header */
  pseudo_header_type   *new_ph,  /* old pseudo header */
  uint16                old_cksum  /* old cksum in host order*/
)
{
  uint32 old_ph_cksum = 0;
  uint32 new_ph_cksum = 0;
  uint8  i = 0;
  
  /*------------------------------------------------------------
    Get 1's complement sum of old checksum
  -------------------------------------------------------------*/
  old_cksum = ~old_cksum;
  
  /*------------------------------------------------------------
    Subtract checksum for old psuedo header from old checksum
  -------------------------------------------------------------*/
  if(old_ph != NULL)
  { 
    /*----------------------------------------------------------
      Caculate the 1's complement sum of old pseudo header
    -----------------------------------------------------------*/
    for(i=0 ; i< 8; i++)
    {
      old_ph_cksum += ps_ntohs(old_ph->source.ps_s6_addr16[i]);
      old_ph_cksum += ps_ntohs(old_ph->dest.ps_s6_addr16[i]);
    }
    old_ph_cksum += old_ph->protocol;
    old_ph_cksum += old_ph->length;  
    old_ph_cksum = eac(old_ph_cksum) & 0xFFFF;
	
    /*-----------------------------------------------------------
      Subtract 1's comeplement sum of psudo header from
	  1's complement sum of old checksum
    -----------------------------------------------------------*/	
    if(old_cksum > old_ph_cksum)
    {
	  old_cksum = (uint16)(old_cksum - old_ph_cksum)&0xFFFF;
    }
    else if(old_cksum < old_ph_cksum)
    {
	  old_cksum = (uint16)(old_cksum - old_ph_cksum - 1)&0xFFFF;
    }
    else
    {
	  old_cksum = 0xFFFF;
    }
  }
  
  /*------------------------------------------------------------
    Add checksum for new psuedo header to old checksum
  -------------------------------------------------------------*/
  if(new_ph != NULL)
  {
    /*----------------------------------------------------------
      Caculate the 1's complement sum of new pseudo header
    -----------------------------------------------------------*/
    for(i=0 ; i< 8; i++)
    {
      new_ph_cksum += ps_ntohs(new_ph->source.ps_s6_addr16[i]);
      new_ph_cksum += ps_ntohs(new_ph->dest.ps_s6_addr16[i]);
    }
    new_ph_cksum += new_ph->protocol;
    new_ph_cksum += new_ph->length;    
	
    /*----------------------------------------------------------
      Add 1 complement sum of new psuedo header to 1's 
	  complement sum of old checksum
    -----------------------------------------------------------*/
    new_ph_cksum += old_cksum;
    old_cksum = (uint16)eac(new_ph_cksum) & 0xFFFF;
  }
  
  /*------------------------------------------------------------
    return final checksum
  -------------------------------------------------------------*/
  return ~old_cksum;
}


/*===========================================================================

FUNCTION  lcsum

DESCRIPTION
  This function computes the 1's-complement sum of the given data buffer.
  This uses a inline assembly routine for the 186 processor which reduces
  CPU utilization greatly.

  The inline assembly uses an unwound loop method to reduce iterations
  of loop.

  buf: ptr to the array of bytes that need to be checksummed.
  cnt: number of bytes that need to be checksummed.

DEPENDENCIES
  None

RETURN VALUE
  The ones complement sum of the bytes in the buffer.

SIDE EFFECTS
  None
===========================================================================*/
/**/
uint16 lcsum
(
  uint16 *buf,
  uint16 cnt
)
{
#if defined(T_ARM) || defined(T_WINNT) || defined (TARGET_OS_SOLARIS) || \
  defined (FEATURE_QDSP6) || defined (T_QDSP6)
  uint32 sum = 0;
  uint16 result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while(cnt-- != 0)
    sum += *buf++;
  result = eac(sum);

  /* Swap the result because of the (char *) to (int *) type punning */

  result = ((uint16)(result << 8)) | ((uint16)(result >> 8));

  return result;

// fixme: mbonnet - when WOULD we do asm? we should be explicit, instead of defaulting  
#else
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

_asm{
      push ds

        lds     si,buf          ; ds:si = buf

        mov     cx,cnt          ; cx = cnt
        cld                     ; autoincrement si
        mov     ax,cx
        shr     cx,1            ; cx /= 16, number of loop iterations
        shr     cx,1
        shr     cx,1
        shr     cx,1

        inc     cx              ; make fencepost adjustment for 1st pass
        and     ax,15           ; ax = number of words modulo 16
        lea     bx,l0           ; Load bx with last address
        sub     bx,ax           ; Now index into instr. being used
        shl     ax,1            ; *=2; each set of instr. is 3 bytes
        sub     bx,ax           ; index into jump table
        clc                     ; initialize carry = 0
        mov     dx,0            ; clear accumulated sum
        jmp     bx              ; jump into loop

; Note, since assumption is made about the number of bytes in each set
; of loadsw and adc ( 3 bytes), the order of instructions inside the
; loop must NOT be changed
;
; Here the real work  _g.e.t.s_ done. The numeric labels on the lodsw instructions
; are the targets for the indirect jump we just made.
;
; Each label corresponds to a possible remainder of (count / 16), while
; the number of times around the loop is determined by the quotient.
;
; The loop iteration count in cx has been incremented by one to adjust for
; the first pass.
;
deloop: lodsw
        adc     dx,ax
l15:    lodsw
        adc     dx,ax
l14:    lodsw
        adc     dx,ax
l13:    lodsw
        adc     dx,ax
l12:    lodsw
        adc     dx,ax
l11:    lodsw
        adc     dx,ax
l10:    lodsw
        adc     dx,ax
l9:     lodsw
        adc     dx,ax
l8:     lodsw
        adc     dx,ax
l7:     lodsw
        adc     dx,ax
l6:     lodsw
        adc     dx,ax
l5:     lodsw
        adc     dx,ax
l4:     lodsw
        adc     dx,ax
l3:     lodsw
        adc     dx,ax
l2:     lodsw
        adc     dx,ax
l1:     lodsw
        adc     dx,ax
l0:     loop    deloop          ; :-)

        adc     dx,0            ; get last carries
        adc     dx,0
        mov     ax,dx           ; result into ax
        xchg    al,ah           ; byte swap result (8088 is little-endian)

        ; return value is in ax.

 pop ds
} /* end _asm */
#endif /* not T_ARM */

} /* lcsum() */
/**/

