#ifndef __PS_CLAT_H_
#define __PS_CLAT_H_
/*===========================================================================

                              P S _ C L A T . H
                   
DESCRIPTION
  The header file for CLAT external declarations.

EXTERNALIZED FUNCTIONS

 Copyright (c)2014-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_clat.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/11/15    kc     Added function ps_clat_ul_ipa_ip6_optimized()
10/10/14    ssh    IP Translation Engine 464XLAT  
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#ifdef FEATURE_DATA_PS_464XLAT
#include "dsm.h"
#include "ps_iface.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_CLAT_UL_IPA_IP6_OPTIMIZED

DESCRIPTION
  Function to fragment an IPv6 packet if needed. If fragmentation is not
  needed the original packet is put back to pkt_buf, otherwise fragments
  are created and put into pkt_buf. It is passed the dsm item
  containing the IPv6 packet, an array in which to return the packets,
  and size of the array.

DEPENDENCIES
  The passed dsm item contains an IPv6 packet in the format of
  IPv6 header + fragment header + data
  The passed array must have been initialized.

RETURN VALUE
  0 in case of success with the datagrams returned in the passed queue
  -1 in case of error

SIDE EFFECTS
  None
===========================================================================*/
int ps_clat_ul_ipa_ip6_optimized
(
  dsm_item_type **pkt_ptr,       /* DSM item containing the IPv6 packet */
  dsm_item_type **pkt_buf,       /* Array to return the packets in */
  uint16          pkt_buf_size,  /* Array size */
  ps_iface_type * iface_ptr,     /* iface ptr */   
  void          * instance       /* clat instance */
);

/*===========================================================================
FUNCTION  PS_CLAT_IS_ENABLED()

DESCRIPTION
  Indicates whether CLAT is turned ON or not.

PARAMETERS
  None

DEPENDENCIES

RETURN VALUE
  TRUE if CLAT is ON.
  FALSE if CLAT is OFF.

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_clat_is_enabled
(
  void
);

/*===========================================================================
FUNCTION ps_clat_ip_to_ip6()

DESCRIPTION
  Performs UL packet translation from IPv4 to IPv6
  This function:
  (1) Accepts incoming IPv4 packets (complete or fragmented)
  (2) Performs RFC 6145 v4 to v6 header translation and returns
      an IPv4 packet (complete or fragment)
 
  Packet formats for IPv6 and IPv4
  
  IPv6 Header
  ===========
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |Version| Traffic Class |           Flow Label                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |         Payload Length        |  Next Header  |   Hop Limit   |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                                                               |
       +                                                               +
       |                                                               |
       +                         Source Address                        +
       |                                                               |
       +                                                               +
       |                                                               |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                                                               |
       +                                                               +
       |                                                               |
       +                      Destination Address                      +
       |                                                               |
       +                                                               +
       |                                                               |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  

  IPv6 Fragment Header
  ====================
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Next Header  |   Reserved    |      Fragment Offset    |Res|M|
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                         Identification                        |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  IPv4 Header
  ===========
        0                   1                   2                   3   
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
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

PARAMETERS
  instance  (IN) : Pointer to the CLAT instance
  iface_ptr (IN) : Pointer to the iface on which the IPv4 packet came in
  pkt_ptr   (IN/OUT): IN: Pointer to the IPv4 packet to be translated
                    OUT: Pointer to the translated IPV6 packet

DEPENDENCIES
  None

RETURN VALUE
  0 on success (packet was successfully translated, pkt_ptr points to the 
                translated packet)
 -1 on failure (packet was not translated, pkt_ptr is not valid anymore)

SIDE EFFECTS
  None
===========================================================================*/
int ps_clat_ip_to_ip6
(
  dsm_item_type ** pkt_ptr,
  ps_iface_type *  iface_ptr,
  void          *  instance
);

/*===========================================================================
FUNCTION PS_CLAT_IP6_TO_IP()

DESCRIPTION
  Performs DL packet translation from IPv6 to IPv4
  This function:
  (1) Accepts incoming IPv6 packets (complete or fragmented)
  (2) Performs RFC 6145 v6 to v4 header translation and returns
      an IPv4 packet (complete or fragment)
 
  Packet formats for IPv6 and IPv4
  
  IPv6 Header
  ===========
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |Version| Traffic Class |           Flow Label                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |         Payload Length        |  Next Header  |   Hop Limit   |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                                                               |
       +                                                               +
       |                                                               |
       +                         Source Address                        +
       |                                                               |
       +                                                               +
       |                                                               |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                                                               |
       +                                                               +
       |                                                               |
       +                      Destination Address                      +
       |                                                               |
       +                                                               +
       |                                                               |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  

  IPv6 Fragment Header
  ====================
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Next Header  |   Reserved    |      Fragment Offset    |Res|M|
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                         Identification                        |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  IPv4 Header
  ===========
        0                   1                   2                   3   
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
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

PARAMETERS
  instance (IN): Pointer to the CLAT instance
  iface_ptr (IN) : Pointer to the iface on which the IPv6 packet came in
  pkt_ptr  (IN/OUT): IN: Pointer to the IPv6 packet to be translated
                    OUT: Pointer to the translated IPV4 packet

DEPENDENCIES
  None

RETURN VALUE
  0 on success (packet was successfully translated, pkt_ptr points to the 
                translated packet)
 -1 on failure (packet was not translated, pkt_ptr is not valid anymore)

SIDE EFFECTS
  None
===========================================================================*/
int ps_clat_ip6_to_ip
(
  dsm_item_type ** pkt_ptr,
  ps_iface_type *  iface_ptr,
  void          *  instance
);
#endif /* FEATURE_DATA_PS_464XLAT */
#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6*/
#endif /* __PS_CLAT_H_ */
