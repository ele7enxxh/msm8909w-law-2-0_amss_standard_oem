#ifndef PS_ICMP_ERROR_H
#define PS_ICMP_ERROR_H

/*===========================================================================

                         P S _ I C M P _ E R R O R . H

DESCRIPTION
  Internet Control Message Protocol (ICMP) (RFC 792)
  Internet Control Message Protocol Version 6 (ICMPv6) (RFC 2463)

EXTERNALIZED FUNCTIONS
  icmpv4_error_send()
    Create and send an ICMPv4 error message.
  
  icmpv4_error()
    Generate an ICMPv4 error. Wrapper around icmpv4_error_send().
    
  icmpv6_error_send()
    Create and send an ICMPv6 error message.

  icmpv6_error()
    Generate an ICMPv6 error. Wrapper around icmpv4_error_send().

  icmp_send_unreach_error()
    Function to send ICMP(v4/v6) destination unreachable error from the 
    transport layers. This function calls v4 or v6 error send depending on
    the IP info.

  icmp_send_quench_error()
    Function to send ICMPv4 source quench error from the transport layers. 

   
Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_icmp_error.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/05   vp      Created the module.
===========================================================================*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "target.h"
#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "ps_pkt_info.h"
#include "ps_icmp_hdr.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_icmp6_msg.h"
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION     ICMPV4_ERROR_SEND()

DESCRIPTION  This function is used to send ICMPv4 error messages. 

DEPENDENCIES Passed dsm item must contain the complete offending IP packet. 

PARAMETERS   dsm_item_type**   - Dsm item containing the offending packet.
             struct icmp*      - Structure describing the icmp error to send.
             ip_pkt_info_type* - IP info for the outgoing message.
             

RETURN VALUE None 

SIDE EFFECTS None. 
===========================================================================*/ 
void icmpv4_error_send
(
  dsm_item_type   **data,
  struct icmp      *icmp,
  ip_pkt_info_type *pkt_info
);

/*===========================================================================

FUNCTION     ICMPV4_ERROR()

DESCRIPTION  This function is used to create ICMPv4 error messages from 
             passed type and code values. It is a wrapper around function
             icmpv4_error_send(). 

DEPENDENCIES Passed dsm item must contain the complete offending IP packet. 

PARAMETERS   dsm_item_type**   - Dsm item containing the offending packet.
             uint8             - Type of ICMPv4 error.
             uint8             - Code of ICMPv4 error.
             ip_pkt_info_type* - IP info for the outgoing message.
             

RETURN VALUE None 

SIDE EFFECTS None. 
===========================================================================*/ 
void icmpv4_error
(
  dsm_item_type   **data,
  uint8             type,
  uint8             code,
  ip_pkt_info_type *pkt_info
);

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================

FUNCTION     ICMPV6_ERROR_SEND()

DESCRIPTION  This function is used to send ICMPv6 error messages. 

DEPENDENCIES Passed dsm item must contain the complete offending IPv6 packet. 

PARAMETERS   dsm_item_type**    - Dsm item containing the offending packet.
             ps_icmp6_msg_type* - Structure describing the icmp error to send.
             ip_pkt_info_type*  - IP info for the outgoing message.
             

RETURN VALUE None 

SIDE EFFECTS None. 
===========================================================================*/ 
void icmpv6_error_send
(
  dsm_item_type    **data,
  ps_icmp6_msg_type *icmp6,
  ip_pkt_info_type  *pkt_info
);

/*===========================================================================

FUNCTION     ICMPV6_ERROR()

DESCRIPTION  This function is used to create ICMPv6 error messages from 
             passed type and code values. It is a wrapper around function
             icmpv6_error_send(). 

DEPENDENCIES Passed dsm item must contain the complete offending IPv6 packet. 

PARAMETERS   dsm_item_type**   - Dsm item containing the offending packet.
             uint8             - Type of ICMPv6 error.
             uint8             - Code of ICMPv6 error.
             ip_pkt_info_type* - IP info for the outgoing message.
             

RETURN VALUE None 

SIDE EFFECTS None. 
===========================================================================*/ 
void icmpv6_error
(
  dsm_item_type   **data,
  uint8             type,
  uint8             code,
  ip_pkt_info_type *pkt_info
);
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================

FUNCTION     ICMP_SEND_UNREACH_ERROR()

DESCRIPTION  Function to send ICMP(v4/v6) destination unreachable error from 
             the transport layers. This function calls v4 or v6 error send 
             depending on the IP info. 

DEPENDENCIES Passed dsm item must contain the complete offending IP(v6) pkt. 

PARAMETERS   dsm_item_type**   - Dsm item containing the offending packet.
             ip_pkt_info_type* - IP info for the outgoing message.
             

RETURN VALUE None 

SIDE EFFECTS None. 
===========================================================================*/ 
void icmp_send_unreach_error 
(
  dsm_item_type   **data,
  ip_pkt_info_type *pkt_info
);

#endif /* PS_ICMP_ERROR_H */
