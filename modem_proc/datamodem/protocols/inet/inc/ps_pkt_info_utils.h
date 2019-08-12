#ifndef PS_PKT_INFO_UTILS_H
#define PS_PKT_INFO_UTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ P K T _ I N F O _ U T I L S. H

DESCRIPTION
  Header containing packet information which needs to be passed through 
  layers.

Copyright (c) 2004-2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_pkt_info_utils.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/20/12    dvk    Made changes for accessing tcp_ack_prio_enabled global 
                   variable from a accessor function.
09/14/11    sm     Added TCP ACK PRIO NV item check for dynamic enablement
                   of ACK tagging.
08/11/11    sm     API ps_pkt_info_parse_packet added to do explicit parsing.
10/18/10    pp     API ps_transport_ptcl_pkt_info_generate added.
01/25/05    sv     Created the file.
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "ps_tx_meta_info.h"
#include "IxErrno.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  TCP_ACK_NONE = 0,
  TCP_ACK_REG,
  TCP_ACK_PIGGYBACK,
  TCP_ACK_MAX = 0xFF
}tcp_ack_type;

/*===========================================================================
FUNCTION PS_TX_IP_PKT_INFO_GENERATE()

DESCRIPTION
  This function processes an IP pakcet and generate IP layer [v4/v6] pkt 
  information used for routing and filtering.

DEPENDENCIES
  None.

PARAMETERS
  pkt_ptr            - IP packet to parse
  meta_info_ref_ptr  - Pointer to a meta info block.
  next_hdr           - next_hdr after processing IPv4/v6 header
  ip_offset          - Offset into DSM item after reading IP header
  ps_errno           - Error code in case of failure cases

RETURN VALUE
  0 for successful processing of the pkt, -1 in case of error

SIDE EFFECTS
  TX meta info is generated which contains IP packet information.

NOTE
  This does not handle fragment queue-ing.
===========================================================================*/
int ps_tx_ip_pkt_info_generate
(
  dsm_item_type                       **pkt_ref_ptr,
  ps_tx_meta_info_type                **mi_ref_ptr,
  ps_ip_protocol_enum_type             *next_hdr,
  uint16                               *ip_offset,
  errno_enum_type                      *ps_errno
);

/*===========================================================================
FUNCTION PS_TRANSPORT_PTCL_PKT_INFO_GENERATE()

DESCRIPTION
  This function processes a DSM pakcet and generate Transport Layer [TCP/UDP/ICMP] pkt 
  information used for filtering.
  protocol - needs to be passed in, to avoid IPv4 / IPv6 check with in this function.

DEPENDENCIES
  None.

PARAMETERS
  pkt_ptr            - DSM packet to parse
  pkt_info_ptr     - Pointer to a meta info block.
  protocol           - next_hdr after processing IPv4/v6 header
  dsm_offset       - Offset into DSM item to read Transport header

RETURN VALUE
  0 for successful processing of the pkt, -1 in case of error

SIDE EFFECTS
  None

NOTE
  Generates ONLY the passed protocol specific packet info is generated.
===========================================================================*/
int ps_transport_ptcl_pkt_info_generate
(
  dsm_item_type                        *pkt_ref_ptr,
  ip_pkt_info_type                     *pkt_info_ptr,
  uint8                                 protocol,
  uint16                                dsm_offset
);

/*===========================================================================
FUNCTION PS_TX_PKT_INFO_GENERATE()

DESCRIPTION
  This function processes an IP pakcet and generate pkt information used
  for routing and filtering.
  
DEPENDENCIES
  None.
  
PARAMETERS
  pkt_ptr            - IP packet to parse
  meta_info_ref_ptr  - Reference to a an optional ptr to meta info block.  
RETURN VALUE
  0 for successful processing of the pkt, -1 in case of error

SIDE EFFECTS
  Meta info is generated which contains packet information and may contain
  filtering results for fragmented packets.

===========================================================================*/

extern int ps_tx_pkt_info_generate
(
  dsm_item_type                       **pkt_ptr,
  ps_tx_meta_info_type                **meta_info_ref_ptr,
  errno_enum_type                      *ps_errno
);

/*===========================================================================
FUNCTION PS_RX_PKT_INFO_GENERATE()

DESCRIPTION
  This function processes an IP pakcet and generate pkt information used
  for routing and filtering.
  
DEPENDENCIES
  None.
  
PARAMETERS
  pkt_ptr            - IP packet to parse
  meta_info_ref_ptr  - Reference to a an optional ptr to meta info block.  
RETURN VALUE
  0 for successful processing of the pkt, -1 in case of error

SIDE EFFECTS
  RX Meta info is generated which contains packet information and may contain
  filtering results for fragmented packets.

===========================================================================*/
extern int ps_rx_pkt_info_generate
(
  dsm_item_type                       **pkt_ptr,
  ps_rx_meta_info_type                **meta_info_ref_ptr,
  errno_enum_type                      *ps_errno
);

/*===========================================================================
  FUNCTION PS_PKT_INFO_FILTER_TX_PKT()

DESCRIPTION
  This function processes an IP packet and then executes all the filters 
  in an iface installed by the spcecific client.

  The filtering results and information extracted from IP packet is stored in
  the meta info which is passed along with the packet down the stack. 
  If the packet has an associated meta info, the information already 
  available in meta info is used as is and remaining fields are updated.

  If the packet is a fragment, is filterable and has other fragments which
  have been queued earlier then the fraghdl argument will be non-NULL.  The
  caller should call ps_iface_ipfltr_get_queued_fragments() to get the
  queued fragments.  If the fragment cannot be filtered currently it is
  queued and ps_errno is set to E_WOULD_BLOCK.  The passed pkt pointer is set
  to NULL and meta info is freed.  No further action should be taken by the
  caller on the fragment.
  
DEPENDENCIES
  IP pkt information should be passed inside meta info or a valid IP packet
  should be provided.
  
PARAMETERS
  iface_ptr   - Iface to pick filters from for execution
  client_id   - Filtering client id  
  pkt_ref_ptr - Ptr to IP pkt to process
  mi_ref_ptr  - Reference to a an optional ptr to meta info block
  fraghdl     - Cookie indicating presence of other fragments.
                Retruns non-NULL if fragments present.
  ps_errno       - Error value returned by the function
  
RETURN VALUE
  0 for successful processing of the pkt, -1 in case of error
  In case of presence of fragments the fraghdl argument is set to a
  non-NULL value.  In case of error the error value is placed in ps_errno

SIDE EFFECTS
  Meta info is generated which contains the filtering result and extracted 
  pkt information if packet processing is successful.  In case of a fragment
  if filtering cannot be performed because of insufficient information the
  fragment is queued and E_WOULD_BLOCK returned as ps_errno.  If a fragment is
  filterable and there are already queued fragments for the same fragmented
  datagram fraghdl argument is set to non-NULL.

===========================================================================*/
extern int ps_pkt_info_filter_tx_pkt
( 
  ps_iface_type                        *iface_ptr,  
  ps_iface_ipfltr_client_id_enum_type   client_id,
  dsm_item_type                       **pkt_ref_ptr,
  ps_tx_meta_info_type                **mi_ref_ptr,
  errno_enum_type                      *ps_errno
);

/*===========================================================================
  FUNCTION PS_PKT_INFO_FILTER_RX_PKT()

DESCRIPTION
  This function processes an IP packet and then executes all the filters 
  in an iface installed by the spcecific client.

  The filtering results and information extracted from IP packet is stored in
  the meta info which is passed along with the packet up the stack. 
  If the packet has an associated meta info, the information already 
  available in meta info is used as is and remaining fields are updated.

  If the packet is a fragment, is filterable and has other fragments which
  have been queued earlier then the fraghdl argument will be non-NULL.  The
  caller should call ps_iface_ipfltr_get_queued_fragments() to get the
  queued fragments.  If the fragment cannot be filtered currently it is
  queued and ps_errno is set to E_WOULD_BLOCK.  The passed pkt pointer is set
  to NULL and meta info is freed.  No further action should be taken by the
  caller on the fragment.
  
DEPENDENCIES
  IP pkt information should be passed inside meta info or a valid IP packet
  should be provided.
  
PARAMETERS
  iface_ptr   - Iface to pick filters from for execution
  client_id   - Filtering client id  
  pkt_ref_ptr - Ptr to IP pkt to process
  mi_ref_ptr  - Reference to a an optional ptr to meta info block
  fraghdl     - Cookie indicating presence of other fragments.
                Retruns non-NULL if fragments present.
  ps_errno       - Error value returned by the function
  
RETURN VALUE
  0 for successful processing of the pkt, -1 in case of error
  In case of presence of fragments the fraghdl argument is set to a
  non-NULL value.  In case of error the error value is placed in ps_errno

SIDE EFFECTS
  Meta info is generated which contains the filtering result and extracted 
  pkt information if packet processing is successful.  In case of a fragment
  if filtering cannot be performed because of insufficient information the
  fragment is queued and E_WOULD_BLOCK returned as ps_errno.  If a fragment is
  filterable and there are already queued fragments for the same fragmented
  datagram fraghdl argument is set to non-NULL.

===========================================================================*/
extern int ps_pkt_info_filter_rx_pkt
( 
  ps_iface_type                        *iface_ptr,  
  ps_iface_ipfltr_client_id_enum_type   client_id,
  dsm_item_type                       **pkt_ref_ptr,
  ps_rx_meta_info_type                **mi_ref_ptr,
  errno_enum_type                      *ps_errno
);

/*===========================================================================
FUNCTION     PS_PKT_INFO_CHECK_TCP_ACK

DESCRIPTION  
  This function is used to find out if the packet is a regular
  tcp ack packet or a piggy back ack packet.

DEPENDENCIES 
  None

PARAMETERS   
  pkt_ptr          - DSM item containing the ip packet.
  ps_errno         - Error code in case of failure cases.

RETURN VALUE
 TCP_ACK_REG       - If regular TCP ACK  
 TCP_ACK_PIGGYBACK - Piggybacked TCP ACK
 TCP_ACK_NONE      - Not a TCP &/ ACK packet
 

SIDE EFFECTS 
 None
===========================================================================*/
tcp_ack_type ps_pkt_info_check_tcp_ack
(
  dsm_item_type    *pkt_ptr,
  errno_enum_type  *ps_errno
);

/*===========================================================================
FUNCTION      PS_PKT_INFO_SET_TCP_ACK_PRIO_ENABLED

DESCRIPTION   This function sets the tcp_ack_prio_enabled variable to the 
              specified value.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps_pkt_info_set_tcp_ack_prio_enabled(uint8 is_enabled);

/*===========================================================================
FUNCTION      PS_PKT_INFO_GET_TCP_ACK_PRIO_ENABLED

DESCRIPTION   This function returns the value of tcp_ack_prio_enabled variable

DEPENDENCIES  None.

RETURN VALUE  Value of tcp_ack_prio_enabled variable

SIDE EFFECTS  None.
===========================================================================*/
uint8 ps_pkt_info_get_tcp_ack_prio_enabled( void );

#ifdef __cplusplus
}
#endif
#endif
