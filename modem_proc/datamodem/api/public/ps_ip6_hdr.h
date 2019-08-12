#ifndef PS_IP6_HDR_H
#define PS_IP6_HDR_H
/** 
  @file ps_ip6_hdr.h 
  @brief 
   Contains common data declarations and function prototypes for
   IPv6 packet header.
*/

/*===========================================================================

                         P S _ I  P 6 _ H D R. H

DESCRIPTION
  Internet Protocol (IP) version 6 header file (RFC 2460)

EXTERNALIZED FUNCTIONS
  ps_ip6_hdr_create() This function is used to create outgoing IP6 headers 
                      for outgoing IP6 packets. This function creates the 
                      associated IP6 header using the parameters specified 
                      by the caller.
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
/*=============================================================================
Copyright (c) 2003-2015 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_ip6_hdr.h#1 $
$Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/15   kc      Added offset macro for fragment id
07/24/13   ash     Data Path Debug F3's & Routing support for IPsec + Fragmentation.
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/11/11   ssh     Doxygen style comments for IPv6 header.`
02/28/11   ash     Added API Doxygen documentation.
12/14/08   pp      Common Modem Interface: Public/Private API split.
02/07/08   ssh     Added ps_ip6_dest_opt_hdr_create()
11/09/07   ssh     Added support for MIPv6
10/12/05   mct     Fixed support for sending to v4/v4-map-v6 addrs over a
                   v6 socket.
03/02/05   vp      Addition of offset constant defines and IP6_MAXPLEN.
10/15/04   ifk     Added support for fragment header
07/30/04   vp      Declarations for routing header and renaming of 
                   ps_ip6_hdr_parse() to ps_ip6_base_hdr_parse().
06/11/04   vp      Replaced ICMP6_PTCL with PS_IPPROTO_ICMP6
04/20/04   mct     Misc fixes for V6.
04/05/04   sv      Featurized the header file.
08/20/03   aku     Initial version
===========================================================================*/
/* group: tcpUdpIpIp6 */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"
#include "ps_in.h"
#include "dsm.h"

/** @addtogroup constants 
  @{
*/
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define IP6LEN                          40
#define IP6_FRAG_HEADER_LEN              8
#define IP6_FRAG_HEADER_OPTIONS_OFFSET  42
  /**< Length of the base IPv6 header.  */
#define IP6_MAXPLEN 65535 
  /**< Maximum payload length allowed for the IPv6 packets. */ 
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/** IPv6 header types.  
*/
typedef ps_ip_protocol_enum_type ip6_hdr_enum_type;

/**
  @brief IPv6 base header. Refer to RFC 2460 @xhyperref{S14,[S14]}. 
*/ 
typedef struct
{
  unsigned int        version:4;           /**< 4-bit version.        */
  unsigned int        trf_cls:8;           /**< 8-bit traffic class.  */
  unsigned int        flow_cls:20;         /**< 20-bit flow class.    */
  uint16              payload_len;         /**< 16-bit payload length.      */
  byte                next_hdr;            /**< 8-bit next header.    */
  byte                hop_limit;           /**< 8-bit hop limit.      */
  /** 128-bit source address.     */
  struct ps_in6_addr  src_addr;
  /** 128-bit destination address. */
  struct ps_in6_addr  dst_addr;            

} ip6_base_hdr_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
#define IP6_BASE_HDR_NEXT_HDR_OFFSET 6      
  /**< Offset for next_hdr field in the IPv6 base header. */
#define IP6_BASE_HDR_PAYLOAD_LEN_OFFSET 4   
  /**< Offset for payload_len field in the IPv6 base header. */
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
@brief IPv6 routing header type. Refer to RFC 2460 @xhyperref{S14,[S14]}. 

  The routing header type uses the following format:
  @verbatim
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Next Header  |  Hdr Ext Len  |  Routing Type | Segments Left |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
:                      type-specific data                       :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  @endverbatim
*/ 
typedef struct
{
  byte   next_hdr;                 /**< Next header after the routing header. */
  byte   hdrlen;                   /**< Routing header length in 8-octet units 
                                        excluding the first 8 octets. */
  byte   type;                     /**< Type of routing header.               */
  byte   segments_left;            /**< Number of route segments remaining.   */
  uint32 reserved;                 /**< Reserved field. This value is 
                                        initialized to 0 and ignored on 
                                        reception. */
  /*      Type specific data comes here. Format determined by routing type   */
} ip6_routing_hdr_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
#define IP6_ROUTING_HDR_TYPE_0 0  /**< Routing header type. */
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
@brief IPv6 Hop-by-hop header type. Refer to RFC 2460 @xhyperref{S14,[S14]}. 

  The hop-by-hop header type for the Router Alert option uses the following
  format:\n
  @verbatim
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Next Header  |  Hdr Ext Len  |                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    
|                                                               |
:                      options                                  :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  @endverbatim

  The Router Alert option uses the following format:
  @verbatim   
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|0 0 0|0 0 1 0 1|0 0 0 0 0 0 1 0| Value (2 octets) |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                        length = 2
  @endverbatim
*/ 
typedef struct
{
  byte     next_hdr;      /**< Next header after the hop-by-hop header. */
  byte     hdrlen;        /**< Hop-by-hop header length in 8-octet units 
                               excluding the first 8 octets. */
  byte     opt_type_0;    /**< Option type 0 of the options header. */
  byte     opt_type_1;    /**< Option type 1 of the options header. */  
  uint16   opt_value;     /**< Value field for the Router Alert option. */                            
  uint16   pad_rtr_alert; /**< Two-byte padding for the Router Alert option. */ 
} ip6_hopbyhop_hdr_type;  
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
#define IP6_HOPBYHOP_HDR_TYPE_0 5       
  /**< Hop-by-hop header type 0. */
#define IP6_HOPBYHOP_RTR_ALERT_TYPE_1 2 
  /**< Hop-by-hop header type 1 (Router Alert option).   */

#define IP6_ROUTING_HDR_TYPE_2               2 
  /**< Routing header type 2. */
#define IP6_ROUTING_HDR_TYPE_2_HDR_EXT_LEN   2 
  /**< Routing header type 2 header with an extended length. */
#define IP6_ROUTING_HDR_TYPE_2_SEG_LEFT      1 
  /**< Routing header type 2 segments left. */
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/*---------------------------------------------------------------------------


---------------------------------------------------------------------------*/
/**
@brief IPv6 fragment header type. Refer to RFC 2460 @xhyperref{S14,[S14]}. 


  The fragment header type uses the following format:
  @verbatim
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Next Header  |  Reserved     |  Fragment Offset        |Res|M|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      Identification                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  @endverbatim
*/ 
typedef PACKED struct PACKED_POST ip6_frag_hdr_type
{
  uint8  next_hdr;           /**< Header after the fragment header. */
  uint8  reserved;           /**< Reserved field. The value is 0 on Tx and is 
                                  ignored on Rx. */
  uint16 offset_flags;       /**< Fragment offset and flags. */
  uint32 id;                 /**< Fragment identification. */
} ip6_frag_hdr_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
#define IP6_FRAG_HDR_OFFSET_FIELD_OFFSET 2  
  /**< Offset for offset_flags field in the IPv6 fragment header. */
#define IP6_FRAG_HDR_ID_FIELD_OFFSET     4
  /**< Offset for Identification field in the IPv6 fragment header. */
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  @brief IPv6 destination options header type. Refer to RFC 2460 @xhyperref{S14,[S14]}. 

  The Destination options header type uses the following format:
  @verbatim
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Next Header  |  Hdr Ext Len  |                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
|                                                               |
.                                                               .
.                            Options                            .
.                                                               .
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  @endverbatim
*/ 
typedef struct
{
  byte   next_hdr;      /**< Next header after this header. */
  byte   hdrlen;        /**< Header length in 8-octet units excluding the first 
                             8 octets. */
  /* Options come here: in TLV format */
} ip6_dest_opt_hdr_type;

/*---------------------------------------------------------------------------
  Typedef for ip_hdr_enum_type struct
---------------------------------------------------------------------------*/
/**
  @brief IPv6 header types. 
*/ 
typedef struct
{
  ip6_hdr_enum_type     hdr_type;           /**< IPv6 header type */
  /** IPv6 header body with IPv6 extension headers. */
  struct
  {
    ip6_base_hdr_type         base_hdr;     /**< IPv6 base header. */
    ip6_hopbyhop_hdr_type     hop_hdr;      /**< IPv6 hop-by-hop options header. */
    ip6_routing_hdr_type      rt_hdr;       /**< IPv6 routing header. */
    ip6_frag_hdr_type         frag_hdr;     /**< IPv6 fragment header. */
    ip6_dest_opt_hdr_type     dest_opt_hdr; /**< IPv6 destination options header. */
  }hdr_body;

} ip6_hdr_type;
/** @} */ /* end_addtogroup datatypes */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION     PS_IP6_HDR_CREATE()
===========================================================================*/
/** @ingroup ps_ip6_hdr_create
  Creates the outgoing IPv6 headers for outgoing IPv6 packets. This function 
  creates the associated IPv6 header using the parameters specified by the 
  caller.

  @datatypes
  #dsm_item_type\n
  #ip6_hdr_type

  @vertspace
  @param[in,out] data_payload  IPv6 packet payload.\n
                               @note1hang The header is added before the
                               payload.
  @param[in,out] hdr           IPv6 header.
 
  @return
  TRUE -- Outgoing IPv6 headers are created successfully.\n
  FALSE -- Failure.

  @dependencies 
  None.
  @newpage
*/
boolean ps_ip6_hdr_create
(
  dsm_item_type   **data_payload,
  ip6_hdr_type    *hdr
);

#endif /* PS_IP6_HDR_H */
