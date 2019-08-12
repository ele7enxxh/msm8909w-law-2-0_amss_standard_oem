/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              T C P H D R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcphdr.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_tcphdr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/13   cx      Replace memcpy and memmove with memscpy and memsmove
11/22/12   ssb     Fixed KW Error
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
08/18/10   sa      Use dsm_insert instead of manipulating data_ptr directly.
01/11/10   ss      Lint warning fixes.
09/24/09   ss      Fixed KW warnings.
06/24/09   am     Renamed hton/ntoh macros with ps_ prefix.
10/22/08   dm      Modified TCP and IP4 headers to fix compiler warnings
10/12/06   hm      Fixed code for negotiating timestamp when tsval sent in
                   the SYN is 0.
05/17/06   rt      Initialized window_scale option in tcp_hdr_parse().
05/25/05   ifk     In tcp_hdr_create() form options if tcp optlen is 0.
05/12/05   rt      Fixed lint errors.
04/06/05   msr     Checking if pseudo header is NULL before updating its
                   length in tcp_hdr_create().
01/27/05   ssh     Changed include file name dsbyte.h to ps_byte.h
12/20/04   vp      Removed the extra increment of pointer while parsing NOOP.
10/15/04   vp      Zero out the options before creating TCP hdr.
                   Fix the timestamp option creating and parsing.
10/13/04   vp      Replaced ntohtcp(), htontcp() with tcp_hdr_parse/create().
09/20/04   jd      A couple optimizations
06/15/04   sv      Added support for parsing SACK option kind.
06/11/04   vp      Removed inclusion of psglobal.h and internet.h. Included
                   ps_pkt_info.h.
04/30/04   sv      Added support to parse timestamp and SACK_PERMITTED options.
04/19/04   mct     Fixed issue with ntohtcp corrupting tcp packets when
                   the options field is present.
11/11/03   rc/aku  Passed additional parameter 'offset' in call to cksum().
11/03/03   mct     Updated ntohtcp() to use new boolean value to determine
                   whether header should be copied or extracted.
09/11/03   sv      Modified htontcp to return if dsm_pushdown fails.
07/31/02   usb     Added include ps_iputil.h
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module.
08/14/01   pjb     Fixed dsm packet size overflow problem
07/23/01   pjb     Changed call to dsm_offset_new_buffer to reserve space
                   for header.  Save the reserved bit from the TCP header.
08/25/00   rc      Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
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

/* TCP header conversion routines
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992 William Allen Simpson
 */
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "amssassert.h"
#include "ps_byte.h"
#include "dsm.h"
#include "ps_iputil.h"
#include "ps_tcp.h"
#include "memory.h"
#include "ps_pkt_info.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/
#define OPTLEN_BOUND_CHECK_RET(index, size, error_msg, retval)    \
{                                                                 \
    if ((index) >= (size)) {                                      \
        LOG_MSG_ERROR("OPTLEN_BOUND_CHECK_RET: %s, Invalid value=%d", \
                  (error_msg), (index), 0 );                      \
        return(retval);                                           \
    }                                                             \
}

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/*===========================================================================

FUNCTION TCP_HDR_CREATE()

DESCRIPTION
  This function will Convert TCP header in host format into mbuf ready for
  transmission, link in data (if any). If ph != NULL, compute checksum,
  otherwise take checksum from tcph->checksum

PARAMETERS
  dsm_item_type**    - DSM item with the outgoing packet.
  struct tcp*        - TCP header.
  Pseudo_header_type - Pseudo header for checksum calculations.

DEPENDENCIES
  Source and destination port in passed TCP header should be in network
  order.
  The pseudo header length should only have the data size in it as TCP
  header size is not known before call to this function.

RETURN VALUE
  boolean - TRUE on success and FALSE on failure.

SIDE EFFECTS
  None
===========================================================================*/

boolean tcp_hdr_create
(
  dsm_item_type      **bpp,
  struct tcp         *tcph,
  pseudo_header_type *ph
)
{
  uint16 hdrlen;
  uint16 checksum;
  uint32 sack_option_cnt;
  uint32 count;
  uint16 hdr_cksum = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (*bpp == NULL &&
      (*bpp = dsm_offset_new_buffer(DSM_DS_LARGE_ITEM_POOL,
                    PS_MAX_HDR)) == NULL)
  {
    LOG_MSG_ERROR_0("No memory to send the packet in tcp_hdr_create()");
    return FALSE;
  }

  if(tcph == NULL)
  {
    LOG_MSG_ERROR_0("Null TCP header passed in tcp_hdr_create()");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |          Source Port          |       Destination Port        |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                        Sequence Number                        |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                    Acknowledgment Number                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |  Data |           |U|A|P|R|S|F|                               |
       | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
       |       |           |G|K|H|T|N|N|                               |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           Checksum            |         Urgent Pointer        |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                    Options                    |    Padding    |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                             data                              |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

                               TCP Header Format
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    First fill out the TCP options in the header so that we know the option
    length which is part of header length. We assume that all extra option
    info in tcph is already in network order for example tcph->mss,
    tcph->tsval etc.
  -------------------------------------------------------------------------*/
  if (tcph->optlen == 0)
  {
    memset(tcph->options,0,TCP_MAXOPT);
    /*-----------------------------------------------------------------------
      TCP MSS option.
    -----------------------------------------------------------------------*/
    if(tcph->syn && tcph->mss)
    {
      tcph->options[tcph->optlen++] = (uint8)TCP_MSS_KIND;
      tcph->options[tcph->optlen++] = TCP_MSS_LENGTH;
      tcph->mss = ps_htons(tcph->mss);
      memscpy((tcph->options + tcph->optlen),
               TCP_MAXOPT - tcph->optlen,
               &(tcph->mss),
               sizeof(uint16));
      tcph->optlen += sizeof(uint16);
      tcph->mss = ps_ntohs(tcph->mss);
    }
    /*-----------------------------------------------------------------------
      Timestamp option.
    -----------------------------------------------------------------------*/
    if(tcph->ts_present)
    {
      tcph->options[tcph->optlen++] = (uint8)TCP_TIME_STAMP_KIND;
      tcph->options[tcph->optlen++] = TCP_TIME_STAMP_LENGTH;

      tcph->tsval = ps_htonl(tcph->tsval);
      tcph->tsecr = ps_htonl(tcph->tsecr);
      memscpy((tcph->options + tcph->optlen),
              TCP_MAXOPT - tcph->optlen,
              &(tcph->tsval),sizeof(uint32));
      tcph->optlen += sizeof(uint32);
      memscpy((tcph->options + tcph->optlen),
              TCP_MAXOPT - tcph->optlen,
              &(tcph->tsecr),sizeof(uint32));
      tcph->optlen += sizeof(uint32);
      tcph->tsval = ps_ntohl(tcph->tsval);
      tcph->tsecr = ps_ntohl(tcph->tsecr);
    }
    /*-----------------------------------------------------------------------
      TCP SACK PERMITTED option.
    -----------------------------------------------------------------------*/
    if(tcph->sack_permitted && tcph->syn)
    {
      tcph->options[tcph->optlen++] = (uint8)TCP_SACK_PERMITTED_KIND;
      tcph->options[tcph->optlen++] = TCP_SACK_PERMITTED_LENGTH;
    }

    /*-----------------------------------------------------------------------
      TCP window scale option.
    -----------------------------------------------------------------------*/
    if(tcph->window_scale_enable && tcph->syn)
    {
      tcph->options[tcph->optlen++] = (uint8)TCP_WINDOW_SCALE_KIND;
      tcph->options[tcph->optlen++] = TCP_WINDOW_SCALE_OPTION_LENGTH;
      tcph->options[tcph->optlen++] = tcph->window_scale;
    }

    /*-----------------------------------------------------------------------
    TCP SACK option.
    -----------------------------------------------------------------------*/
    if(tcph->sack_block_cnt)
    {
      /*---------------------------------------------------------------------
        Number of SACK options that can be added to the options is
        (TCP_MAXOPT - Current_optlen - 2)/TCP_UNIT_SACK_OPT_SIZE
      ---------------------------------------------------------------------*/
      sack_option_cnt = (TCP_MAXOPT - tcph->optlen - 2)/TCP_UNIT_SACK_OPT_SIZE;
      sack_option_cnt = MIN(sack_option_cnt, tcph->sack_block_cnt);
      tcph->options[tcph->optlen++] = (uint8)TCP_SACK_KIND;
      tcph->options[tcph->optlen++] = (uint8)(sack_option_cnt*TCP_UNIT_SACK_OPT_SIZE+2);

      for(count = 0;count < sack_option_cnt; count++)
      {
        tcph->sack_blocks[count].start_seqno =
                              ps_htonl(tcph->sack_blocks[count].start_seqno);
        tcph->sack_blocks[count].end_seqno =
                              ps_htonl(tcph->sack_blocks[count].end_seqno);

        OPTLEN_BOUND_CHECK_RET(tcph->optlen, (TCP_MAXOPT - sizeof(uint32) + 1),  
                               "TCP optlen exceeds limit.",
                               FALSE);
        memscpy(tcph->options + tcph->optlen,
               TCP_MAXOPT - tcph->optlen,
               (uint8 *)&tcph->sack_blocks[count].start_seqno,
               sizeof(uint32));
        tcph->optlen += sizeof(uint32);
        OPTLEN_BOUND_CHECK_RET(tcph->optlen, (TCP_MAXOPT - sizeof(uint32) + 1), 
                               "TCP optlen exceeds limit.",
                               FALSE);
        memscpy(tcph->options + tcph->optlen,
                TCP_MAXOPT - tcph->optlen,
                (uint8 *)&tcph->sack_blocks[count].end_seqno,
                sizeof(uint32));
        tcph->optlen += sizeof(uint32);

        tcph->sack_blocks[count].start_seqno =
                              ps_ntohl(tcph->sack_blocks[count].start_seqno);
        tcph->sack_blocks[count].end_seqno =
                              ps_ntohl(tcph->sack_blocks[count].end_seqno);
      }
    }

    /*-----------------------------------------------------------------------
      Add padding.
    -----------------------------------------------------------------------*/
    if(tcph->optlen & 0x3)
    {
      tcph->optlen = (((tcph->optlen >>2)+1) <<2);
    }
  }
  ASSERT(tcph->optlen <= TCP_MAXOPT);

  hdrlen = TCPLEN + tcph->optlen;

  /*-------------------------------------------------------------------------
    Source and destination ports are already in network order in tcph.
  -------------------------------------------------------------------------*/
  tcph->seq = ps_htonl(tcph->seq);
  tcph->ack = ps_htonl(tcph->ack);
  /*-------------------------------------------------------------------------
    Divide the hdrlen by 4 since it is stored in 4 byte units in hlen filed
    of the header.
  -------------------------------------------------------------------------*/
  tcph->hlen = hdrlen >> 2;
  tcph->wnd = ps_htons(tcph->wnd);
  checksum = tcph->checksum;

  if(ph == NULL)
  {
    tcph->checksum = ps_htons(tcph->checksum);
  }
  else
  {
    tcph->checksum = 0;

    /*-----------------------------------------------------------------------
      We need to update the Pseudo header length as it does not have the TCP
      header length included in it as it was not known before a call to this.
    -----------------------------------------------------------------------*/
    ph->length += hdrlen;
  }
  tcph->up  = ps_htons(tcph->up);

  /*-------------------------------------------------------------------------
    Push the header on the dsm item.
  -------------------------------------------------------------------------*/
  if ((count = dsm_pushdown_packed( bpp, ((uint8*)tcph), hdrlen,
                                    DSM_DS_SMALL_ITEM_POOL))
      < hdrlen)
  {
    LOG_MSG_ERROR_2("Error in dsm_pushdown_packed() in tcp_hdr_create()"
        "pushed only %d bytes, expected %d bytes",count,hdrlen);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Recompute checksum, if requested
    Use dsm_insert API to insert bytes into particular location in DSM packet
    chain. Manipulating data_ptr might result in DSM corruption {if the
    data_ptr doesnt have enough space to accomodate checksum}.
  -------------------------------------------------------------------------*/
  if(ph != NULL)
  {
    // (void)put16( (*bpp)->data_ptr + 16, cksum(ph,*bpp,ph->length,0) );
    hdr_cksum = cksum(ph,*bpp,ph->length,0);
    hdr_cksum = ps_htons(hdr_cksum); /* Host to Network Byte Ordering */
    if (dsm_insert(bpp,
                   DSM_DS_SMALL_ITEM_POOL,
                   16,
                   (void*)&hdr_cksum, /* 2 byte checksum */
                   sizeof(uint16)) != sizeof(uint16))
    {
      LOG_MSG_ERROR_0("Error in inserting checksum in tcp_hdr_create()");
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Restore the values which were changed to network order in TCP header
    so that if somebody is using those later after calling this function
    then they will find the values in proper order.
  -------------------------------------------------------------------------*/
  tcph->seq = ps_ntohl(tcph->seq);
  tcph->ack = ps_ntohl(tcph->ack);
  tcph->wnd = ps_ntohs(tcph->wnd);
  tcph->checksum = checksum;
  tcph->up  = ps_ntohs(tcph->up);

  return TRUE;
}/* tcp_hdr_create() */

/*===========================================================================

FUNCTION TCP_HDR_PARSE()

DESCRIPTION
  This function will update the appropriate fields in passed TCP header
  after extracting information from dsm item.

PARAMETERS
  dsm_item_type * - DSM item containing the packet.
  uint16 *        - Offset where the TCP header is in the DSM item.
  struct tcp *    - TCP header to be filled.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE on success and FALSE on failure.

SIDE EFFECTS
  Sets the value of offset to after the TCP header in dsm item.
===========================================================================*/
boolean tcp_hdr_parse
(
  dsm_item_type *bpp,
  uint16        *offset,
  struct tcp    *tcph
)
{
  int   hdrlen,i,j,ret;
  int kind,kindlen;
  uint8 *cp;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bpp == NULL || NULL == tcph || NULL == offset)
  {
    LOG_MSG_ERROR_0("NULL DSM or TCP or offset passed to tcp_hdr_parse()");
    return FALSE;
  }


  if((ret = dsm_extract(bpp, *offset,(uint8*)tcph,TCPLEN)) != TCPLEN)
  {
    LOG_MSG_ERROR_2("Error in dsm_extract of TCP header in tcp_hdr_parse(),"
        "Expected TCPLEN (%d) bytes, Extracted %d bytes",TCPLEN,ret);
    return FALSE;
  }

  *offset += TCPLEN;

  /*-------------------------------------------------------------------------
    Header length is stored in multiple of 4 bytes.
  -------------------------------------------------------------------------*/
  hdrlen = tcph->hlen * 4;
  if(hdrlen < TCPLEN || hdrlen > (TCPLEN + TCP_MAXOPT))
    {
    LOG_MSG_INFO1_1("Invalid header length %d",hdrlen);
    return FALSE;
    }

  tcph->seq       = ps_ntohl(tcph->seq);
  tcph->ack       = ps_ntohl(tcph->ack);
  tcph->wnd       = ps_ntohs(tcph->wnd);
  tcph->checksum  = ps_ntohs(tcph->checksum);
  tcph->up        = ps_ntohs(tcph->up);

  tcph->mss      = 0;
  tcph->sack_permitted = FALSE;
  tcph->tsval    = 0;
  tcph->tsecr    = 0;
  tcph->ts_present      = FALSE;
  tcph->sack_block_cnt = 0;

  tcph->window_scale_enable = FALSE;
  tcph->window_scale        = 0;

  if ( (tcph->optlen = (uint8)(hdrlen - TCPLEN)) == 0 )
    {
    /*-----------------------------------------------------------------------
      No options. We are done.
    -----------------------------------------------------------------------*/
    return TRUE;
    }

  if((ret = dsm_extract(bpp,*offset,tcph->options,tcph->optlen))
     != tcph->optlen )
  {
    LOG_MSG_ERROR_2("Error in dsm_extract of options in tcp_hdr_parse(),"
        "Expected %d, returned %d",tcph->optlen,ret);
    return FALSE;
  }

  *offset += tcph->optlen;

  /*-------------------------------------------------------------------------
    Process options
  -------------------------------------------------------------------------*/
  for( cp=tcph->options, i=tcph->optlen; i>0;)
  {
    kind = *cp++;

    /*-----------------------------------------------------------------------
      Process single-byte options
    -----------------------------------------------------------------------*/
    switch( kind)
    {
      case TCP_EOL_KIND:
        /*-------------------------------------------------------------------
          End of options list
        -------------------------------------------------------------------*/
        return TRUE;

      case TCP_NOOP_KIND:
        i--;
        continue;

      default:
        break;
    }
    /*-----------------------------------------------------------------------
      All other options have a length field
    -----------------------------------------------------------------------*/
    kindlen = *cp++;

    switch( kind)
    {
      case TCP_MSS_KIND:
        if(kindlen == TCP_MSS_LENGTH)
        {
          tcph->mss = get16(cp);
        }
        else
        {
          LOG_MSG_INFO1_1(" Malformed TCP MSS option: kindlen %d", kindlen);
          return FALSE;
        }
        break;

      case TCP_SACK_PERMITTED_KIND:
        if(kindlen == TCP_SACK_PERMITTED_LENGTH)
        {
          tcph->sack_permitted = TRUE;
        }
        else
        {
          LOG_MSG_INFO1_1(" Malformed TCP SACK PERMITTED option: kindlen %d",
              kindlen);
          return FALSE;
        }
        break;

      case TCP_SACK_KIND:

        /*-------------------------------------------------------------------
          Valid SACK option has a length of 8n + 2. where n is the number of
          SACK blocks
        -------------------------------------------------------------------*/
        if (((kindlen - 2) & 0x7) != 0)
        {
          LOG_MSG_INFO1_1(" Malformed TCP SACK options: kindlen %d", kindlen);
          return FALSE;
        }

        tcph->sack_block_cnt = (uint8)(((unsigned int)(kindlen - 2)) >> 3);
        if (tcph->sack_block_cnt > TCP_SACK_OPT_MAX_BLOCKS)
        {
          LOG_MSG_INFO1_1(" Malformed TCP SACK options: kindlen %d", kindlen);
          return FALSE;
        }

        for(j=0;j<tcph->sack_block_cnt;j++)
        {
          memscpy(&tcph->sack_blocks[j].start_seqno, 
                  sizeof(tcph->sack_blocks[j].start_seqno),
                  (cp + 8*j),
                  sizeof(uint32));
          memscpy(&tcph->sack_blocks[j].end_seqno,
                  sizeof(tcph->sack_blocks[j].end_seqno),
                  (cp +4+ 8*j),
                  sizeof(uint32));
          tcph->sack_blocks[j].start_seqno =
                                 ps_ntohl(tcph->sack_blocks[j].start_seqno);
          tcph->sack_blocks[j].end_seqno =
                                 ps_ntohl(tcph->sack_blocks[j].end_seqno);
        }
        break;

    case TCP_TIME_STAMP_KIND:
        if(kindlen == TCP_TIME_STAMP_LENGTH)
        {
          memscpy(&tcph->tsval,sizeof(tcph->tsval),cp,sizeof(uint32));
          memscpy(&tcph->tsecr,sizeof(tcph->tsecr),(cp+4),sizeof(uint32));
          tcph->tsval = ps_ntohl(tcph->tsval);
          tcph->tsecr = ps_ntohl(tcph->tsecr);
          tcph->ts_present   = TRUE;
        }
        else
        {
          LOG_MSG_INFO1_1(" Malformed TCP TIME STAMP option: kindlen %d",
                   kindlen);
          return FALSE;
        }
        break;

      case TCP_WINDOW_SCALE_KIND:
        if(kindlen == TCP_WINDOW_SCALE_OPTION_LENGTH  &&
           *cp < TCP_MAX_WINDOW_SCALE)
        {
          tcph->window_scale = *cp;
          tcph->window_scale_enable = TRUE;
        }
        else
        {
          LOG_MSG_INFO1_2(" Malformed TCP WINDOW SCALE option: kindlen %d  scale %d",
                   kindlen,*cp);
          return FALSE;
        }
        break;

      default:
        LOG_MSG_INFO2_1 ("Unrecognized TCP option %d!  ignoring", kind);
        break;
    }
    /*-----------------------------------------------------------------------
      Enforce legal minimum
    -----------------------------------------------------------------------*/
    kindlen  = MAX(2,kindlen);
    i       -= kindlen;
    cp      += kindlen - 2;
  }
  return TRUE;

} /* tcp_hdr_parse() */

