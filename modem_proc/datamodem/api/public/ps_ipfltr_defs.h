#ifndef PS_IPFLTR_DEFS_H
#define PS_IPFLTR_DEFS_H

/*===========================================================================

                          P S _ I P F L T R _ D E F S . H

===========================================================================*/

/**
   @file ps_ipfltr_defs.h
   @brief Data structure definition for inbound IP filters
 */
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
Copyright (c) 2003-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ipfltr_defs.h_v   1.0   07 Feb 2003 20:14:44   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_ipfltr_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/07/11    sd     (Tech Pubs) Continued edit.
12/01/10    sd     (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
11/27/07    hm     Added ESP field for next header protocol
11/10/04    mct    Added more enums to check ranges for QoS.
06/11/04    vp     Included ps_in.h, Changed src_addr and dst_addr in ip_hdr 
                   struct ps_in_addr.
02/18/04    usb    Renamed enum, added next_prot field to the default filter
10/09/03    jd     fixed typo in IP filter type ICMP protocol header field
01/28/03    usb    created file
===========================================================================*/
/* group: tcpUdpIpIp6 */

#include "comdef.h"
#include "ps_in.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      GLOBAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @addtogroup datatypes 
  @{
*/
/**
  Type of IP filters.
*/
typedef enum
{
  IPFLTR_DEFAULT_TYPE = 0,   /**< Default filter parameters and execution 
                                  rules.  */
  IPFLTR_MBIM_TYPE = 1,     /**< MBIM filter parameters and execution 
                                  rules.  */                                  
  /** @cond
  */
  IPFLTR_MAX_TYPE            /**< Internal use only. */
  /** @endcond */
} ipfltr_type_enum_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                       
                       QoS FILTER VALIDATION RULES
                       
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  --Description moved to ProtocolService_mainpage.dox file--
  These are the rules for specifiying a QoS filter spec. If these critera 
  are not met the validation of the filter spec will fail, and QoS
  registration will not succeed.

  This section describes the validation rules and semantics for the IP filter 
  spec parameters.
  1. A bit in field_mask shall be set for each parameter specified, if a bit 
     is not set the corresponding parameter is ignored.
  2. If one or more parameters are incorrrectly specified, the bits in 
     err_mask indicate those parameters on return.  Hence in case of errors, 
     err_mask should be checked to see if a parameter validation error has 
     occurred which will be cleared otherwise.
  3. If a parameter consisting of multiple sub fields is specified, each 
     subfield shall also be specified.  Such parameters are,
       a.IPv4v6 src_addr and dst_addr
       b.IPv4 ToS
       c.IPv6 traffic class
       d.TCP/UDP src_port and dst_port
  4. Each filter will have an IP version associated with it which is either 
     v4 or v6 and is shall be specified by ip_vsn.  It is invalid to specify 
     v6 parameters for a v4 type filter and visa versa. 
  5. Not all combinations of filter parameters are allowed in each filter.  
     The following table describes the valid combinations.  Only those 
     attributes marked with an "X" may be specified for a single packet 
     filter. All marked attributes may be specified, but at least one shall 
     be specified.
             Table 1: Valid Packet Filter Attribute Combinations
                 
           Table 1                       Valid combination types
    Packet filter attribute	                  I   II  III  IV
    Source Address and Subnet Mask	          X   X    X   X
    Destination Address and Subnet Mask	      X   X    X   X
    Protocol Number (IPv4)/Next Header (IPv6)	X	  X    X
    Destination Port Range                    X
    Source Port Range                         X
    ICMP Msg Type                                 X
    ICMP Msg Code                                 X
    IPSec SPI                                          X
    TOS (IPv4)/Traffic Class (IPv6) and Mask	X   X    X   X
    Flow Label (IPv6)                                      X
    
  6. If a parameter from next header protocol (TCP, UDP etc.) is specified in
     a filter, the Protocol Number for v4 filters or Next Header for v6 
     filters shall be specified.
  7. In most cases IP source address in Tx filters and IP destination address
     in Rx filters is not required to be specified.  In case these values are,
     specified following requirements shall be met.       
       a. IP source address in Tx filters and IP destination address in Rx 
          filters can only have a single address value.  Hence subnet_mask 
          (for v4 filters) or prefix_len (for v6 filters) is ignored for 
          these parameters.
       b. These address values shall be same as the current IP address 
          assigned to the MS on the IP interface on which the QoS is being 
          requested.
       c. If the IP address on the interface changes (e.g. during a network 
          handoff), the client which requested QoS  is responsible for 
          updating the filters with the new address or else the flow may not
          be able to receive the desired treatment.
  8. IP destination address in Tx filters and IP source address in Rx filters
     can be specified as address ranges using the subnet_mask (for v4 filters)
     or prefix_len (for v6 filters) parameters.  A subnet_mask may be set to 
     all 1's or prefix_len may be set to 128 to specify a single address 
     value.
  9.A nonzero value for range may be set to specify a range of port numbers 
     starting from the value specified in port [port to port+range], 
     otherwise range shall be set to zero.
  10.A nonzero value for range may be set to specify a range of port numbers 
     starting from the value specified in port [port to port+range], otherwise
     range shall be set to zero.
  11.Certain fields like address, port numbers etc. shall be specified in 
     network byte order, everything else shall be in host byte order.  
     Following fields shall be specified in netwok byte order:
       a.IPv4 addresses
       b.IPv4 subnet masks
       c.IPv6 addresses (prefix length shall be in host order)
       d.TCP and UDP port numbers (port ranges shall be in host order)
       e.IPv6 foe label
---------------------------------------------------------------------------*/


/**
  IPv4 header information.
*/
typedef enum
{
  IPFLTR_MASK_IP4_NONE          = 0x00, 
    /**< No mask. */
  IPFLTR_MASK_IP4_SRC_ADDR      = 0x01, 
    /**< Mask for the IPv4 source address. */
  IPFLTR_MASK_IP4_DST_ADDR      = 0x02, 
    /**< Mask for the IPv4 destination address. */
  IPFLTR_MASK_IP4_NEXT_HDR_PROT = 0x04, 
    /**< Mask for the IPv4 next header. */
  IPFLTR_MASK_IP4_TOS           = 0x08, 
    /**< Mask for the IPv4 ToS. */
  IPFLTR_MASK_IP4_ALL           = 0x0f  
    /**< Mask for the IPv4 source address, destination address, next header, 
         and ToS. */
} ipfltr_ip4_hdr_field_mask_enum_type;

/**
  IPv6 header information.
*/
typedef enum
{
  IPFLTR_MASK_IP6_NONE          = 0x00, 
    /**< No mask. */
  IPFLTR_MASK_IP6_SRC_ADDR      = 0x01, 
    /**< Mask for the IPv6 source address. */
  IPFLTR_MASK_IP6_DST_ADDR      = 0x02, 
    /**< Mask for the IPv6 destination address. */
  IPFLTR_MASK_IP6_NEXT_HDR_PROT = 0x04, 
    /**< Mask for the IPv6 next header. */
  IPFLTR_MASK_IP6_TRAFFIC_CLASS = 0x08, 
    /**< Mask for the IPv6 traffic class. */
  IPFLTR_MASK_IP6_FLOW_LABEL    = 0x10, 
    /**< Mask for the IPv6 flow label. */
  IPFLTR_MASK_IP6_ALL           = 0x1f  
    /**< Mask for the IPv6 source address, destination address, next header, 
         traffic class, and flow label. */
} ipfltr_ip6_hdr_field_mask_enum_type;
		
/* Higher level protocol header parameters. */
 
/**
  TCP header information.
*/
typedef enum
{	
  IPFLTR_MASK_TCP_NONE          = 0x00, /**< No mask. */
  IPFLTR_MASK_TCP_SRC_PORT      = 0x01, /**< Mask for the TCP source port. */
  IPFLTR_MASK_TCP_DST_PORT      = 0x02, /**< Mask for the TCP destination port. */
  IPFLTR_MASK_TCP_ALL           = 0x03  /**< Mask for the TCP source port and 
                                             destination port. */
} ipfltr_tcp_hdr_field_mask_enum_type;
  
/**
  UDP header information.
*/
typedef enum
{	
  IPFLTR_MASK_UDP_NONE          = 0x00, /**< No mask. */
  IPFLTR_MASK_UDP_SRC_PORT      = 0x01, /**< Mask for the UDP source port. */
  IPFLTR_MASK_UDP_DST_PORT      = 0x02, /**< Mask for the UDP destination port. */
  IPFLTR_MASK_UDP_CHKSUM        = 0x04, /**< Mask for the UDP checksum. */
  IPFLTR_MASK_UDP_ALL           = 0x07  /**< Mask for the UDP source port, 
                                             destination port and checksum. */
} ipfltr_udp_hdr_field_mask_enum_type;
    
/**
  ICMP header fields.
*/
typedef enum
{
  IPFLTR_MASK_ICMP_NONE         = 0x00, /**< No mask. */
  IPFLTR_MASK_ICMP_MSG_TYPE     = 0x01, /**< Mask for the ICMP message type. */
  IPFLTR_MASK_ICMP_MSG_CODE     = 0x02, /**< Mask for the ICMP message code. */
  IPFLTR_MASK_ICMP_MSG_ID       = 0x04, /**< Mask for the ICMP message identifier */
  IPFLTR_MASK_ICMP_ALL          = 0x07  /**< Mask for the ICMP message type , 
                                             message code and message identifier. */
} ipfltr_icmp_hdr_field_mask_enum_type;

/**
  ESP header fields.
*/
typedef enum
{
  IPFLTR_MASK_ESP_NONE          = 0x00, /**< No mask. */
  IPFLTR_MASK_ESP_SPI           = 0x01, /**< Mask for the ESP security policy 
                                             index. */
  IPFLTR_MASK_ESP_ALL           = 0x01  /**< Mask for ESP header. */
} ipfltr_esp_hdr_field_mask_enum_type;

/**
  AH header fields.
*/
typedef enum
{
  IPFLTR_MASK_AH_NONE          = 0x00, /**< No mask. */
  IPFLTR_MASK_AH_SPI           = 0x01, /**< Mask for the AH security policy 
                                             index. */
  IPFLTR_MASK_AH_ALL           = 0x01  /**< Mask for AH header. */
} ipfltr_ah_hdr_field_mask_enum_type;

/**
  TCP UDP header fields.
*/
typedef enum
{   
  IPFLTR_MASK_TCP_UDP_NONE          = 0x00, 
    /**< No mask. */
  IPFLTR_MASK_TCP_UDP_SRC_PORT      = 0x01, 
    /**< Mask for the TCP UDP source port. */
  IPFLTR_MASK_TCP_UDP_DST_PORT      = 0x02, 
    /**< Mask for the TCP UDP destination port. */
  IPFLTR_MASK_TCP_UDP_ALL           = 0x03  
    /**< Mask for the TCP UDP destination port and destination port. */
} ipfltr_tcp_udp_hdr_field_mask_enum_type;

/** @name IP_FILTER_TYPE Filter Mask Types
  Following are the type definitions for all mask types used in #IP_FILTER_TYPE.
  @{
*/

/** IPv4 header field filter mask type. 
*/
typedef uint8 ipfltr_ip4_hdr_field_mask_type;

/** IPv6 header field filter mask type. 
*/
typedef uint8 ipfltr_ip6_hdr_field_mask_type;

/** TCP header field filter mask type.  
*/
typedef uint8 ipfltr_tcp_hdr_field_mask_type;

/** UDP header field filter mask type. 
*/
typedef uint8 ipfltr_udp_hdr_field_mask_type;

/** ICMP header field filter mask type.  
*/
typedef uint8 ipfltr_icmp_hdr_field_mask_type;

/** ESP header field filter mask type.  
*/
typedef uint8 ipfltr_esp_hdr_field_mask_type;

/** AH header field filter mask type.  
*/
typedef uint8 ipfltr_ah_hdr_field_mask_type;

/** TCP and UDP header field filter mask type.  
*/
typedef uint8 ipfltr_tcp_udp_hdr_field_mask_type;

/** @} */  /* end_name IP_FILTER_TYPE Filter Mask Types */

/*-----------------------------------------------------------------------------
TYPEDEF IP_FILTER_TYPE
-----------------------------------------------------------------------------*/
/**
  @brief IP filter parameters for a default filter type. 

  A default filter contains all the common parameters required for most
  filtering needs and is processed by a default set of rules (i.e., pattern
  matching on parameters).

  All the address/port number fields must be specified in network-byte order. 
  Everything else is in host-byte order.
*/
typedef struct
{
  ip_version_enum_type  ip_vsn; /**< IP version of the filter (IPv4 or IPv6). 
                                     This parameter is mandatory. */

  /** Filter parameter values. Only values set in field masks are valid. 
    The corresponding error mask is set when a parameter value is invalid. */
  union
  {

    /** IPv4 header filtering block. */
    struct
    {
      ipfltr_ip4_hdr_field_mask_type      field_mask;  
        /**< In mask for the IPv4 header filtering block. */
      ipfltr_ip4_hdr_field_mask_type      err_mask;    
        /**< Out mask for the IPv4 header filtering block. */

      /** Source address. */
      struct
      {
        struct ps_in_addr  addr;        
          /**< IPv4 source address. */
        struct ps_in_addr  subnet_mask; 
          /**< Subnet mask for the source address. */
      } src;

      /** Destination address. */
      struct
      {
        struct ps_in_addr  addr;        
          /**< IPv4 destination adress. */
        struct ps_in_addr  subnet_mask; 
          /**< Subnet mask for the destination adress. */
      } dst;
      
      /** ToS. */
      struct
      {
        uint8 val;  /**< ToS value. */
        uint8 mask; /**< ToS mask. */
      } tos;      
      
      uint8 next_hdr_prot; /**< Next header protocol. */
    } v4;

    /** IPv6 header filtering block. */
    struct
    {
      ipfltr_ip6_hdr_field_mask_type      field_mask;  
        /**< In mask for the IPv6 header filtering block. */
      ipfltr_ip6_hdr_field_mask_type      err_mask;    
        /**< Out mask for the IPv6 header filtering block. */

      /** Source address. */
      struct
      {
        /** IPv6 source address. */
        struct ps_in6_addr  addr;       
        uint8               prefix_len; 
          /**< Length of the prefix for the IPv6 source address. */
      } src;
      
      /** Destination address. */
      struct
      {
        /** IPv6 destination address. */
        struct ps_in6_addr addr;       
        uint8              prefix_len; 
          /**< Length of the prefix for the IPv6 destination address. */
      } dst;
      
      /** Traffic class. */
      struct
      {
        uint8   val;  /**< Traffic class value. */
        uint8   mask; /**< Traffic class mask. */
      } trf_cls;
      
      uint32   flow_label;     /**< Flow label. */
      uint8    next_hdr_prot;  /**< Transport-level protocol header. */
    } v6; 
  } ip_hdr;

  /** Transport Level Protocol header block: TCP, UDP, ICMP. The next_hdr_prot
    field in the IPv4 or IPv6 header must be set to specify a parameter from  
    the next_prot_hdr union block. */
  union
  {
    /** TCP header filtering block. */
    struct
    {
      ipfltr_tcp_hdr_field_mask_type      field_mask;  
        /**< In mask for the TCP header filtering block.   */
      ipfltr_tcp_hdr_field_mask_type      err_mask;    
        /**< Out mask for the TCP header filtering block.  */

      /** TCP source port and port range information. */
      struct
      {
        uint16    port;  /**< TCP source port. */
        uint16    range; /**< Range of the TCP source port. */
      } src;
    
      /** TCP destination port and port range information. */
      struct
      {
        uint16    port;  /**< TCP destination port. */
        uint16    range; /**< Range of the TCP destination port. */
      } dst;    
    } tcp;
    
    /** UDP header filtering block. */
    struct
    {
      ipfltr_udp_hdr_field_mask_type      field_mask;  
        /**< In mask for the UDP header filtering block. */
      ipfltr_udp_hdr_field_mask_type      err_mask;    
        /**< Out mask for the UDP header filtering block. */

      /** UDP source port and port range information. */
      struct
      {
        uint16    port;  /**< UDP source port. */
        uint16	  range; /**< Range of the UDP source port. */
      } src;
      
      /** UDP destination port and port range information. */
      struct
      {
        uint16    port;  /**< UDP destination port. */
        uint16    range; /**< Range of the UDP destination port. */
      } dst;

      uint16 chksum; /**< UDP checksum. */
    } udp;
    
    /** ICMP header filtering block. */
    struct
    {    
      ipfltr_icmp_hdr_field_mask_type     field_mask; 
        /**< In mask for the ICMP header filtering block. */
      ipfltr_icmp_hdr_field_mask_type     err_mask;   
        /**< Out mask for the ICMP header filtering block. */

      uint8   type; /**< ICMP type. */
      uint8   code; /**< ICMP code. */
      uint16  id;   /**< ICMP id.     */
    } icmp;
  
    /** ESP header filtering block. */
    struct
    {
      ipfltr_esp_hdr_field_mask_type     field_mask; 
        /**< In mask for the ESP header filtering block. */
      ipfltr_esp_hdr_field_mask_type     err_mask;   
        /**< Out mask for the ESP header filtering block. */

      uint32  spi; /**< Security policy index. */
    } esp;

    /** AH header filtering block. */
    struct
    {
      ipfltr_ah_hdr_field_mask_type     field_mask; 
        /**< In mask for the ESP header filtering block. */
      ipfltr_ah_hdr_field_mask_type     err_mask;   
        /**< Out mask for the ESP header filtering block. */

      uint32  spi; /**< Security policy index. */
    } ah;

    /** TCP+UDP [combined] header filtering block. */
    struct
    {
      ipfltr_tcp_udp_hdr_field_mask_type      field_mask;  
        /**< In mask for the TCP+UDP header filtering block. */
      ipfltr_tcp_udp_hdr_field_mask_type      err_mask;    
        /**< Out mask for the TCP+UDP header filtering block. */
  
      /** TCP+UDP source port and port range. */
      struct
      {
        uint16    port;  /**< TCP+UDP source port. */
        uint16    range; /**< Range of the TCP+UDP source port. */
      } src;
  
      /** TCP+UDP destination port and port range. */
      struct
      {
        uint16    port;  /**< TCP+UDP destination port. */
        uint16    range; /**< Range of the TCP+UDP destination port. */
      } dst; 

    } tcp_udp_port_range;
  } next_prot_hdr;

  /** IP filter auxillary information. */
  struct
  { 
    uint16 fi_id;          /**< Filter ID. */
    uint16 fi_precedence;  /**< Filter precedence. */
  } ipfltr_aux_info;

} ip_filter_type;

/**
  @brief IP filter specification.
*/
typedef struct
{
  uint8             num_filters;      /**< Number of filters in the list. */
  ip_filter_type    *list_ptr;        /**< Pointer to the list of filters. */
} ip_filter_spec_type;

/** @} */  /* end_addtogroup datatypes */

#endif /* PS_IPFLTR_DEFS_H */

