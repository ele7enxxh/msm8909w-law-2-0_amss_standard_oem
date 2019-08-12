#ifndef PS_IP4_HDR_H
#define PS_IP4_HDR_H
/** 
  @file ps_ip4_hdr.h 
  @brief 
   Contains common data declarations and function prototypes for
   IPvv packet header.
*/

/*===========================================================================

                         P S _ I  P 4 _ H D R. H

DESCRIPTION
  Internet Protocol version 4 header file. (RFC 791).

EXTERNALIZED FUNCTIONS

  ps_ip4_hdr_parse()  This function is used to parse the IP4 header in the 
                      incoming packet and provides the parsed header as an 
                      output parameter.

  ps_ip4_hdr_create() This function is used to create outgoing IP4 headers for 
                      outgoing IP4 packets. This function creates the 
                      associated IP4 header using the parameters specified by 
                      the caller. 

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the tcpUdpIpIp6 group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_ip4_hdr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/14    cx     Implement ps_ip4_hdr_parse_optimized().
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup. Edited 
                   Doxygen markup and comments.
07/20/11    gs     Removed inclusion of <target.h>
04/17/09    pp     Modified IPv4 header to use UINT32_T(unsigned int) to fix
                   compile warning.
12/28/08    pp     Common Modem Interface: Public/Private API split.
10/22/08    dm     Modified TCP and IP4 headers to fix compiler warnings
07/28/07    rs     Added router_alert field to struct ip.
10/13/04    vp     Created the module.
===========================================================================*/
/* group: tcpUdpIpIp6 */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "ps_in.h"
#include "dsm.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/** @addtogroup constants 
  @{
*/
#define IP_CS_OLD  1   /**< use saved checksum */
#define IP_CS_NEW  0   /**< calculate checksum */

#define IPLEN      20  /**< Length of standard IP header */
#define IP_MAXOPT  40  /**< Largest option field, bytes */

#define UDP_ENCAPSULATION_LEN 8 /**< UDP Encapsulation header length */

#define OPT_COPIED  0x80  /**< Copied-on-fragmentation flag */
#define OPT_CLASS   0x60  /**< Option class */
#define OPT_NUMBER  0x1f  /**< Option number */

/* IP option numbers */

#define IP_EOL    0      /**< End of options list */
#define IP_NOOP   1      /**< No Operation */
#define IP_SECURITY  2   /**< Security parameters */
#define IP_LSROUTE  3    /**< Loose Source Routing */
#define IP_TIMESTAMP  4  /**< Internet Timestamp */
#define IP_RROUTE  7     /**< Record Route */
#define IP_STREAMID  8   /**< Stream ID */
#define IP_SSROUTE  9    /**< Strict Source Routing */

/* Timestamp option flags */

#define TS_ONLY   0      /**< Time stamps only */
#define TS_ADDRESS  1    /**< Addresses + Time stamps */
#define TS_PRESPEC  3    /**< Prespecified addresses only */

#define IP_DEF_TTL 255   /**< Default TTL value */
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
@brief IPv4 Header type. Refer to RFC 791 @xhyperref{Sxx,[Sxx]}. 

  The IPv4 Header format:
  @verbatim

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
  @endverbatim
*/ 
struct ip
{
  /*-------------------------------------------------------------------------
   ANSI C: Bit wise fields should be defined with "int" type ONLY. 
   AMSS "uint32" uses unsigned long and cannot be used to define Bit fields.
   Current "unit32" definition in comdef.h violates ANSI C rule, hence 
   created new MACRO to define unsigned int
  -------------------------------------------------------------------------*/
#define UINT32_T unsigned int
  UINT32_T         ihl:4;              /**<                 IP header length */
  UINT32_T         version:4;          /**<                       IP version */
  UINT32_T         tos:8;              /**<                  Type of service */
  UINT32_T         length:16;          /**<                     Total length */
  UINT32_T         id:16;              /**<                   Identification */
  UINT32_T         congest:1;          /**< Congestion experienced bit (exp) */
  UINT32_T         df:1;               /**<              Don't fragment flag */
  UINT32_T         mf:1;               /**<              More Fragments flag */
  UINT32_T         off:13;             /**<  Fragment offset in 8 byte units */
  uint8             ttl;               /**<                     Time to live */
  uint8             protocol;          /**<                         Protocol */
  uint16            checksum;          /**<                  Header checksum */
  struct ps_in_addr source;            /**<                   Source address */
  struct ps_in_addr dest;              /**<              Destination address */
  uint8             options[IP_MAXOPT];/**<                    Options field */
  /* Following are not part of IPv4 hdr,included here for processing info. */
  uint16            optlen;            /**<   Length of options field, bytes */
  uint16            offset;            /**<         Fragment offset in bytes */
  boolean           router_alert;      /**< Routers should examine this packet
                                            more closely                     */
  uint16            flags_offset;      /**<   Container for flags and offset */
};
/** @} */ /* end_addtogroup datatypes */

/*===========================================================================
FUNCTION     PS_IP4_HDR_PARSE()
===========================================================================*/
/** @ingroup ps_ip4_hdr_parse
  This function is used to parse the IP4 header in the incoming packet and  
  provides the parsed header as an output parameter

  @datatypes
  #dsm_item_type\n
  #ip

  @vertspace
  @param[in] data         Dsm item containing the new packet.
  @param[in, out] offset  Offset which points to the start of packet\n
                          @note1hang The offset gets updated to end of 
                           ip header after the parsing.
  @param[in,out] hdr IPv4 header.
 
  @return
  TRUE -- IPv4 header parsed successfully.\n
  FALSE -- Failure.

  @dependencies 
  None.
  @newpage
*/
boolean ps_ip4_hdr_parse
(
  dsm_item_type     * data,
  uint16            * offset,
  struct ip         * hdr
);

/*===========================================================================
FUNCTION     PS_IP4_HDR_PARSE_OPTIMIZED()
===========================================================================*/
/** @ingroup ps_ip4_hdr_parse
  This function is used to parse the IP4 header in the incoming packet 
  and provides the parsed header as an output parameter.For optimize 
  purpose, only flag_offset are in host order

  @datatypes
  #dsm_item_type\n
  #ip

  @vertspace
  @param[in] data         Dsm item containing the new packet.
  @param[in, out] offset  Offset which points to the start of packet\n
                          @note1hang The offset gets updated to end of 
                           ip header after the parsing.
  @param[in,out] hdr IPv4 header.
 
  @return
  TRUE -- IPv4 header parsed successfully.\n
  FALSE -- Failure.

  @dependencies 
  The offset gets updated to end of ip header.
  The header is not actually removed from dsm item.
  @newpage
*/
boolean 
ps_ip4_hdr_parse_optimized
(
  dsm_item_type     * data,
  uint16            * offset,
  struct ip         * hdr
);

/*===========================================================================
FUNCTION     PS_IP4_HDR_CREATE()
===========================================================================*/
/** @ingroup ps_ip4_hdr_create
  This function is used to create outgoing IP4 headers for outgoing IP4 
  packets. This function creates the associated IP4 header using the 
  parameters specified by the caller

  @datatypes
  #dsm_item_type\n
  #ip

  @vertspace
  @param[in, out]  data_payload IPv4 packet payload.\n
                               @note1hang The header is added before the
                               payload.
  @param[in] hdr   IPv4 header.
  @param[in] cflag Flag which tells whether to copy the existing checksum 
                   or calculate a new one
  @return
  TRUE -- IPv4 header created successfully.\n
  FALSE -- Failure.

  @dependencies 
  None.
  @newpage
*/
boolean ps_ip4_hdr_create
(
  dsm_item_type   **data_payload,
  struct ip       *hdr,
  int             cflag
);

#ifdef __cplusplus
}
#endif

#endif /* PS_IP4_HDR_H */
