#ifndef PS_IP6I_HDR_H
#define PS_IP6I_HDR_H
/*===========================================================================

                         P S _ I  P 6 I _ H D R. H

DESCRIPTION
  Internet Protocol (IP) version 6 Internal header file (RFC 2460)

EXTERNALIZED FUNCTIONS
  ps_ip6_hdr_parse()  This function is used to process the next IP6 header in
                      the incoming IP6 packet and provides the parsed header 
                      as an output parameter.
                        
  
Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_ip6i_hdr.h#1 $
$Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/14    ssh    IP Translation Engine 464XLAT 
12/14/08    pp     Created module as part of Common Modem Interface: 
                   Public/Private API split.
===========================================================================*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "ps_ip6_hdr.h"
#include "ps_icmp6_msg.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION     PS_IP6_BASE_HDR_PARSE()

DESCRIPTION  This function is used to process the next IP6 header in the 
             incoming IP6 packet and provides the parsed header as an output 
             parameter.

DEPENDENCIES None

RETURN VALUE boolean - TRUE on success
                       FALSE on failure

SIDE EFFECTS None
===========================================================================*/
boolean ps_ip6_base_hdr_parse
(
  dsm_item_type     *data,
  uint16            *offset,
  ip6_hdr_type      *hdr
);

/*===========================================================================
FUNCTION     PS_IP6_FRAG_HDR_PARSE()

DESCRIPTION  This function is used to parse the IPv6 fragment header in the 
             incoming IP6 packet and returns it as an output parameter.

DEPENDENCIES None

RETURN VALUE boolean - TRUE on success
                       FALSE on failure

SIDE EFFECTS None
===========================================================================*/
boolean ps_ip6_frag_hdr_parse
(
  dsm_item_type *pkt_ptr,
  uint16        *offset,
  ip6_hdr_type  *hdr
);

/*===========================================================================

FUNCTION     PS_IP6_ROUTING_HDR_PARSE()

DESCRIPTION  This function is used to process the routing header in the
             incoming IP6 header.

DEPENDENCIES None

PARAMETERS   dsm_item_type*     - Ptr to the dsm item containing the packet.
             uint16*            - Ptr to the offset of the start of this hdr.
             ip6_hdr_type*      - Pointer to the base IP6 header.
             ps_icmp6_msg_type* - Pointer to ICMP6 header in case of error.

RETURN VALUE boolean - TRUE on success
                       FALSE on failure

SIDE EFFECTS Advances the offset by the length of the routing header.
             Sets the hdr->hdr_type to the next hdr after this hdr.
===========================================================================*/
boolean ps_ip6_routing_hdr_parse
(
  dsm_item_type     *data,
  uint16            *offset,
  ip6_hdr_type      *hdr,
  ps_icmp6_msg_type *icmp6
);

#ifdef FEATURE_DATA_PS_MIPV6
/*===========================================================================

FUNCTION     PS_IP6_DEST_OPT_HDR_CREATE()

DESCRIPTION  This function is used to add a destination option header
             to the packet.
             
DEPENDENCIES None

RETURN VALUE TRUE on success
             FALSE on failure
                         
SIDE EFFECTS None
===========================================================================*/
boolean ps_ip6_dest_opt_hdr_create
(
  dsm_item_type         **dsm_ptr_ptr,
  ip6_dest_opt_hdr_type  *dest_opt_hdr
);
#endif /* FEATURE_DATA_PS_MIPV6 */


#endif /* PS_IP6I_HDR_H */
