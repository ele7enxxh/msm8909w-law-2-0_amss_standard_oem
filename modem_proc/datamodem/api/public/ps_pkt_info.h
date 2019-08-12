#ifndef PS_PKT_INFO_H
#define PS_PKT_INFO_H
/**
   @file ps_pkt_info.h
   @brief containing packet information which needs to be passed through
  layers.
 */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ P K T _ I N F O . H

DESCRIPTION
  Header containing packet information which needs to be passed through
  layers.

Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the ipUtilities group 
      description in the dms_mainpage.dox file. 
=============================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_pkt_info.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
03/11/11    am     Added ICMP ID field to pkt_info struct.
02/24/10    pp     secips_ipsec_info_type moved from SEC to PS as 
                   ps_ipsec_info_type.
11/10/09    ss     IP Multicast feature additions.
12/14/08    pp     Common Modem Interface: Public/Private API split.
10/12/05    mct    Fixed support for sending to v4/v4-map-v6 addrs over a
                   v6 socket.
08/16/05    sv     Added SPI filed to ESP and AH pakcet info for fragment
                   filtering.
05/11/05    sv     Added ESP and AH headers to packet info.
04/20/05    sv     Added fraghdl to pktinfo.
10/13/04    vp     Removed inclusion of ps_iphdr.h and included ps_ip4_hdr.h.
06/11/04    vp     Created the file.
===========================================================================*/
/* group: ipUtilities */
#include "comdef.h"
#include "ps_in.h"
#include "ps_ip4_hdr.h"
#include "ps_ip6_hdr.h"

/** @addtogroup datatypes
  @{
*/
/**
  @brief Pseudo-header for TCP and UDP checksumming.
*/ 
typedef struct
{
  struct ps_in6_addr source;   /**< IP source */
  struct ps_in6_addr dest;     /**< IP destination */
  uint16 length;               /**< Data field length */
  uint8  protocol;             /**< Protocol */
} pseudo_header_type;

/**
  @brief IP packet information needed by ipfilter (IP header + extra info)
*/ 
struct ip_pkt_info_s
{
  ip_version_enum_type  ip_vsn; /**< IP version */
  union
  {
    struct ip    v4;            /**< IPv4 header */
    ip6_hdr_type v6;            /**< IPv6 header */
  } ip_hdr;

  pseudo_header_type    pseudo_hdr; /**< Pseudo header */

  struct
  {
    uint32 spi;                 /**< ESP header - SPI */
    void* esp_handle;           /**< ESP handle */
  } esp_hdr;

  struct
  {
    uint32 spi;                 /**< AH header - SPI */
    void* ah_handle;            /**< AH handle */
  } ah_hdr;

  union
  {
    struct
    {
      uint8       type;              /**< ICMP message type               */
      uint8       code;              /**< ICMP message code               */
      uint16      id;                /**< ICMP ID for echo/reply          */
    } icmp;

    struct
    {
      uint16      src_port;          /**< Source UDP port number          */
      uint16      dst_port;          /**< Destination UDP port number     */
    } udp;

    struct
    {
      uint16      src_port;          /**< Source TCP port number          */
      uint16      dst_port;          /**< Destination TCP port number     */
    } tcp;

  } ptcl_info;                       /**< Protocol specific information   */

  boolean     ipsec_required;        /**< Packet need sESP processing        */
  boolean     is_secure;             /**< Packet is secured by IPSec         */
  boolean     is_brdcst;             /**< Packet has a broadcast destination */
  boolean     is_local;              /**< Identifies packets for local host  */
  boolean     is_pkt_info_valid;     /**< Used to avoid multiple packet info 
                                        generations */
  boolean     is_mcast_loop;         /**< Multicast datagram-looping enabled */
  void*       if_ptr;                /**< Iface on which IP pkt is rcv'ed    */
  void*       fraghdl;               /**< Fragment handler                   */
};

typedef struct ip_pkt_info_s       ip_pkt_info_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup macros_ipUtilities
  @{
*/
#define PS_SECIPS_MAX_TUNNELS 2       /**< Maximum IPSEC tunnels supported */ 
#define PS_SECIPS_MAX_IFACE_LIST (PS_SECIPS_MAX_TUNNELS +2) /**< Maximum IPSEC IFACEs supported */ 
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup datatypes
  @{
*/
/**
  @brief Per Packet IPSec info - part of Meta Info struct.
*/ 
typedef struct
{
  void*           esp_sa_ptr;            /**< ESP SA pointer        */
  void*           ah_sa_ptr;             /**< AH SA pointer         */
  void*           ipsec_handle;          /**< IPSEC handle          */
  void*           iface_list[PS_SECIPS_MAX_IFACE_LIST]; /**< IPSec iface list */
  uint8           iface_cnt;             /**< IPSec iface count     */
  ps_ip_addr_type next_gw_addr;          /**< Next Gateway address  */
  uint32          ipsec_header_size;     /**< IPSEC header size     */
  uint32          user_id;               /* *< User Identification  */
}ps_ipsec_info_type;
/** @} */ /* end_addtogroup datatypes */
#endif /* PS_PKT_INFO_H */
