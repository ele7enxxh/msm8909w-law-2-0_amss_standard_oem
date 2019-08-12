/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               I P v 4  H E A D E R  P A R S E  &  C R E A T E

                             F U N C T I O N S


GENERAL DESCRIPTION
  IP header conversion routines

EXTERNALIZED FUNCTIONS

  ps_ip4_hdr_parse()  This function is used to parse the IP4 header in the
                      incoming packet and provides the parsed header as an
                      output parameter.

  ps_ip4_hdr_create() This function is used to create outgoing IP4 headers for
                      outgoing IP4 packets. This function creates the
                      associated IP4 header using the parameters specified by
                      the caller.

Copyright (c) 2004-2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_ip4_hdr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/14    cx     Implement ps_ip4_hdr_parse_optimized().
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
07/29/07    rs     Added code for Router Alert option.
05/11/05    sv     Lint changes.
10/13/04    vp     Created the module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "err.h"
#include "dsm.h"
#include "memory.h"

#include "ps_iputil.h"
#include "ps_ip4_hdr.h"
#include "ds_Utils_DebugMsg.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================

FUNCTION     PS_IP4_HDR_PARSE()

DESCRIPTION  This function is used to parse the IP4 header in the
             incoming packet and provides the parsed header as an output
             parameter.

DEPENDENCIES None

PARAMETERS   dsm_item_type* - Dsm item containing the new packet.
             uint16*        - Offset which points to the start of packet.
             struct ip*     - Return parameter which gets filled with the
                          contents of ip header.

RETURN VALUE boolean - TRUE on success
                       FALSE on failure

SIDE EFFECTS The offset gets updated to end of ip header.
             The header is not actually removed from dsm item.
===========================================================================*/
boolean ps_ip4_hdr_parse
(
  dsm_item_type     * data,
  uint16            * offset,
  struct ip         * hdr
)
{
  uint16 ret;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ps_ip4_hdr_parse_optimized(data,offset,hdr))
  {
    return FALSE;
  }
  
  hdr->length   = ps_ntohs(hdr->length);
  hdr->id       = ps_ntohs(hdr->id);
  hdr->checksum = ps_ntohs(hdr->checksum);
  
  /*-------------------------------------------------------------------------
   If the header length is greater than 20 bytes (IPLEN) then the header
   includes options. Extract the options from the header.
  -------------------------------------------------------------------------*/
  if ( (hdr->optlen = (uint16)(hdr->ihl * 4) - IPLEN) != 0 )
  {
    if((ret = dsm_extract(data,*offset,(uint8*)(hdr->options),hdr->optlen)) !=
       hdr->optlen)
    {
      LOG_MSG_ERROR_2("Error in dsm_extract of options in ps_ip4_hdr_parse(),"
          "Expected %d, returned %d",hdr->optlen,ret);
      return FALSE;
    }
    *offset += hdr->optlen;
  }

  return TRUE;
} /* ps_ip4_hdr_parse() */

/*===========================================================================

FUNCTION     PS_IP4_HDR_PARSE_OPTIMIZED()

DESCRIPTION  This function is used to parse the IP4 header in the
             incoming packet and provides the parsed header as an output
             parameter.For optimize purpose, only flag_offset are in host
             order
             
DEPENDENCIES None

PARAMETERS   dsm_item_type* - Dsm item containing the new packet.
             uint16*        - Offset which points to the start of packet.
             struct ip*     - Return parameter which gets filled with the
                          contents of ip header.

RETURN VALUE boolean - TRUE on success
                       FALSE on failure

SIDE EFFECTS The offset gets updated to end of ip header.
             The header is not actually removed from dsm item.
===========================================================================*/
boolean 
ps_ip4_hdr_parse_optimized
(
  dsm_item_type     * data,
  uint16            * offset,
  struct ip         * hdr
)
{
  uint16    ip_hdr_len;      /* IP header length in bytes                  */
  uint32 ret;

  if( NULL == data || NULL == hdr || NULL == offset )
  {
    LOG_MSG_ERROR_0("NULL DSM or ipv4 header or offset"
                  " passed to ps_ip4_hdr_parse()");
    return FALSE;
  }

  if((ret = dsm_extract(data,*offset,(uint8*)hdr,IPLEN)) != IPLEN)
  {
    LOG_MSG_ERROR_2("Error in dsm_extract of IP header in ps_ip4_hdr_parse(),"
        "Expected IPLEN (%d) bytes, Extracted %d bytes",IPLEN,ret);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   IP header length is represented in 4 byte unit(i.e. number of 32 bit
   words in the header), ip_hdr_length is in bytes.
  -------------------------------------------------------------------------*/
  ip_hdr_len = (uint16)(hdr->ihl * 4);
  if((ip_hdr_len < IPLEN) || (ip_hdr_len > (IPLEN + IP_MAXOPT)))
  {
    LOG_MSG_ERROR_1("Header length (%d) is invalid",ip_hdr_len);
    return FALSE;
  }

  *offset += IPLEN;

  /*-------------------------------------------------------------------------

                               IP header format

       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |Version|  IHL  |Type of Service|          Total Length         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |         Identification        |Flags|      Fragment Offset    |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |  Time to Live |    Protocol   |         Header Checksum       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                       Source Address                          |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                    Destination Address                        |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                    Options                    |    Padding    |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

       IHL - Header Length
       Flags - Consist of 3 bits
               First bit is kept 0
           Second bit is Dont Fragment bit.
           Third bit is More Fragments bit.

  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
   Fragmentation offset is measured in 8 octet increments.
   Third bit is more fragments bit.
   Second bit is dont fragment bit.
  -------------------------------------------------------------------------*/
  hdr->flags_offset = ps_ntohs(((uint16*)hdr)[3]);
  hdr->off     = ( hdr->flags_offset  & 0x1fff);
  hdr->mf      = ( hdr->flags_offset  & 0x2000) ? 1 : 0;
  hdr->df      = ( hdr->flags_offset  & 0x4000) ? 1 : 0;
  hdr->congest = ( hdr->flags_offset  & 0x8000) ? 1 : 0;
  hdr->offset  = (uint16)hdr->off * 8;
  return TRUE;
}

/*===========================================================================

FUNCTION     PS_IP4_HDR_CREATE()

DESCRIPTION  This function is used to create outgoing IP4 headers for
             outgoing IP4 packets. This function creates the associated IP4
             header using the parameters specified by the caller.

DEPENDENCIES The caller should set offset and optlen fields of struct ip
             and should leave the off and ihl fields as it is.

PARAMETERS   dsm_item_type** - Dsm item for outgoing packet.
             struct ip*      - IP header which needs to be creted on outgoing
                           packet.
             int             - Flag which tells whether to copy the existing
                               checksum or calculate a new one.

RETURN VALUE boolean TRUE on success
                     FALSE on failure

SIDE EFFECTS None
===========================================================================*/
boolean ps_ip4_hdr_create
(
  dsm_item_type   **data_payload,
  struct ip       *hdr,
  int             cflag
)
{
  uint16 hdr_len;                        /*                  Header length */
  uint16 flags_offset;                   /* Fragmentation flags and offset */
  uint32 ret;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( data_payload == NULL || *data_payload == NULL  || NULL == hdr)
  {
    LOG_MSG_ERROR_0("NULL DSM item or ipv4 header in ps_ip4_hdr_create()");
    return FALSE;
  }

  if (hdr->router_alert == TRUE)
  {
  /*-------------------------------------------------------------------------
     Set one bit copy flag to 1 and two bit class to 0.
      Set five bit options to 20 (10100 in binary)
    -------------------------------------------------------------------------*/
    hdr->options[0] = hdr->options[0] & 0x80;
    hdr->options[0] = hdr->options[0] | 0x14;

  /*-------------------------------------------------------------------------
       Set length to 4. (Total length of the option)
   -------------------------------------------------------------------------*/
    hdr->options[1] = 4;

  /*-------------------------------------------------------------------------
       Set 16 bit router alert value to 0.
   -------------------------------------------------------------------------*/
    hdr->options[2] = 0;
    hdr->options[3] = 0;

  /*-------------------------------------------------------------------------
       Mark end of option list,
   -------------------------------------------------------------------------*/
    hdr->options[4] = 0;

  /*-------------------------------------------------------------------------
    Make the IP header 4 byte aligned
   -------------------------------------------------------------------------*/
    hdr->options[5] = 0;
    hdr->options[6] = 0;
    hdr->options[7] = 0;
    hdr->optlen += 8;
  }
  
  /*-------------------------------------------------------------------------
    Calculate the header length using the options length. Check to see
    whether the header length is not more than max allowed and it is 4 byte
    aligned.
  -------------------------------------------------------------------------*/
  hdr_len = IPLEN + hdr->optlen;
  if((hdr_len > (IPLEN + IP_MAXOPT)) || ((hdr_len & 0x3) != 0))
  {
    LOG_MSG_ERROR_1("Malformed options as IP header length including options is %d"
        ,hdr_len);
    return FALSE;
  }

  /*-------------------------------------------------------------------------

                               IP header format

       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |Version|  IHL  |Type of Service|          Total Length         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |         Identification        |Flags|      Fragment Offset    |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |  Time to Live |    Protocol   |         Header Checksum       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                       Source Address                          |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                    Destination Address                        |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                    Options                    |    Padding    |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

       IHL - Header Length
       Flags - Consist of 3 bits
               First bit is kept 0
           Second bit is Dont Fragment bit.
           Third bit is More Fragments bit.

  -------------------------------------------------------------------------*/

  hdr->ihl    = hdr_len >> 2;
  hdr->id     = ps_htons(hdr->id);
  hdr->length = ps_htons(hdr->length);

  flags_offset = ((uint16*)hdr)[3];
  ((uint16*)hdr)[3] = ps_htons((((uint16)hdr->congest) << 15) |
                            (((uint16)hdr->df) << 14) |
                            (((uint16)hdr->mf) << 13) |
                            ((uint16)(hdr->offset >> 3)));
  /*-------------------------------------------------------------------------
    The following segment of code regarding checksum needs to be rewritten
    when a dsm_seek_insert function is provided. We need to remove cflag
    from the signature of ps_ip4_hdr_create(). So when we get the proper
    dsm function to insert something on dsm item, we separate the creating
    of IP header and checksum calculating/copying. This function will just
    copy the existing checksum in IP header and the caller of this function
    will have the responsibility to calculate the new one (using the cksum()
    function as the IP header will already be pushed on to the dsm item) and
    insert the new checksum at proper place with the dsm_seek_insert function.
  -------------------------------------------------------------------------*/
  if(!cflag)
  {
    hdr->checksum = 0;
    hdr->checksum = buffer_cksum((uint16*)hdr,hdr_len);
  }
  hdr->checksum = ps_htons(hdr->checksum);

  /*-------------------------------------------------------------------------
    Push the created IP header on the dsm chain.
  -------------------------------------------------------------------------*/
  if ((ret = dsm_pushdown_packed( data_payload, ((uint8*)hdr), hdr_len,
                           DSM_DS_SMALL_ITEM_POOL))
      < hdr_len)
  {
    LOG_MSG_ERROR_2("Error in dsm_pushdown_packed() in ps_ip4_hdr_create()"
        "pushed only %d bytes, expected %d bytes",ret,hdr_len);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Restore the values in IP header which were changed.
  -------------------------------------------------------------------------*/
  hdr->id           = ps_ntohs(hdr->id);
  hdr->length       = ps_ntohs(hdr->length);
  hdr->checksum     = ps_ntohs(hdr->checksum);
  ((uint16*)hdr)[3] = flags_offset;

  return TRUE;

}/* ps_ip4_hdr_create() */
