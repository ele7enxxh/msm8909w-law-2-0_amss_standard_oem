#ifndef PS_PPPI_VEND_H
#define PS_PPPI_VEND_H
/*===========================================================================

                           P S _ P P P I _ V E N D . H

DESCRIPTION
  This is the internal header file for the PPP vendor-specific module. 
  See the external header file ps_ppp_vend.h for the external interfaces for
  this module.

Copyright (c) 2006-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_pppi_vend.h#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/07/09    pp     CMI Phase-4: SU Level API Effort.
04/02/07    ssh    Added ppp_vendor_pkt_info_s_type to pass info to PS task
12/21/06    ssh    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"                        /* Customer Specific Features */
#include "comdef.h"
#include "ps_ppp_vend.h"
#include "ps_ppp_defs.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for buffer used to pass information from client context to PS context
  for vendor-specific packet to be sent to the peer.
---------------------------------------------------------------------------*/
typedef struct ppp_vendor_pkt_info_s_type
{
  ppp_dev_enum_type  device;        /* device to send pkt on               */
  uint16             protocol;      /* PPP protocol field of pkt           */
  uint8              id;            /* packet identifier                   */
  dsm_item_type    * pkt_ptr;       /* pkt to send                         */
} ppp_vendor_pkt_info_s_type;


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_VENDOR_EXT_HDLR()

DESCRIPTION
  This function handles incoming vendor-specific packets (code field 0) 
  containing proprietary vendor extensions for LCP and NCPs (IPCP, IPv6CP).
  The registered callback is invoked for the OUI for which the packet is 
  received. The PPP configuration header (code, id, len) should have been 
  removed and the item data_ptr should be pointing to the magic-number field.
  Also, length indicated in the hdr should be the length of the packet
  starting from the magic number field.

PARAMETERS
  device   : PPP device on which packet was received
  protocol : protocol field for the received packet
  hdr      : Pointer to the config header structure (code, id, len)
  pkt_ptr  : Pointer to the received packet. pkt_ptr->data_ptr should be 
             pointing to the magic number field, as indicated below.
             
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |     Code      |  Identifier   |            Length             | 
  -> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
     |                         Magic-Number                          |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                      OUI                      |     Kind      |   
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |    Value(s) ...
     +-+-+-+-+-+-+-+-+
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_vendor_ext_hdlr
(
  ppp_dev_enum_type     device,       /* (in) device on which pkt arrived  */
  uint16                protocol,     /* (in) PPP protocol field of pkt    */
  ppp_config_hdr_type * hdr,          /* (in) ptr to config hdr            */
  dsm_item_type       * pkt_ptr       /* (in) ptr to rcvd packet           */
);

/*===========================================================================
FUNCTION PPPI_VENDOR_SEND()

DESCRIPTION
  Sends a PPP vendor-specific packet. The caller is expected to provide the
  vendor-specific packet onwards (and inclusive) of the OUI field.
  
PARAMETERS
  device   : PPP device to send packet on
  protocol : protocol field for the packet
  id       : Identifier for this packet.
  pkt_ptr  : Pointer to the packet to be sent. This is expected to be
             pointing to the OUI field, filled in by the client.
             This MUST NOT be NULL.

RETURN VALUE
  None

DEPENDENCIES
  Must always be called in PS task context

SIDE EFFECTS
  None
===========================================================================*/
void pppi_vendor_send
(
  ppp_dev_enum_type  device,          /* (in) device to send pkt on        */
  uint16             protocol,        /* (in) PPP protocol field of pkt    */
  uint8              id,              /* (in) packet identifier            */
  dsm_item_type    * pkt_ptr          /* (in) pkt to send                  */
);
#endif /* _PS_PPPI_VEND_H */
