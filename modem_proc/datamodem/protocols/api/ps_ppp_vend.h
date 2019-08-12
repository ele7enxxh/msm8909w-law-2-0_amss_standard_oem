#ifndef PS_PPP_VEND_H
#define PS_PPP_VEND_H
/*===========================================================================

                           P S _ P P P _ V E N D . H

DESCRIPTION
  This file contains the definitions of data structures, defined and 
  enumerated constants and function prototypes required for the 
  data services PPP vendor extensions implementation (as per RFC 2153)

  The usage model for this module is described below.
  
  The client is expected to register for an OUI that it manages (e.g. the
  3GPP2-OUI, 0xCF002) using the register function, and provide an input
  callback function which is invoked when a Vendor-Specific packet 
  for the registered OUI is received. 
  
  Vendor-Specific packet format
  
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |     Code      |  Identifier   |            Length             | 
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
     |                         Magic-Number                          |
  -> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                      OUI                      |     Kind      |   
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |    Value(s) ...
     +-+-+-+-+-+-+-+-+
  
  
  The client is expected to parse and create the part of the packet following
  the -> mark above.
  
  RECEIVE PATH:
  The registered input callback function is invoked when a Vendor-Specific 
  packet for the registered OUI is received. The client is expected to process 
  the packet onwards (and inclusive of) the OUI field.
  
  TRANSMIT PATH:
  The client invokes the send function to send Vendor-Specific packets to
  the peer. The client is expected to provide the part of the packet 
  starting (and inclusive) of the OUI field.

Copyright (c) 2006-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_vend.h#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/06/11    ash    Added support for PPP Partial Context
03/23/11    op     Data SU API cleanup
08/31/09    mga    Merged from eHRPD branch
05/07/09    pp     CMI Phase-4: SU Level API Effort.
12/21/06    ssh    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_ppp_defs.h"
#include "dsm.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PPP_VENDOR_INPUT_PKT_INFO_TYPE

DESCRIPTION
  Type of the structure used to pass information about the received
  vendor-specific packet to the registered OUI module
---------------------------------------------------------------------------*/
typedef struct ppp_vendor_input_pkt_info_type
{
  uint16  protocol;                    /* protocol of the packet           */
  uint8   id;                          /* packet identifier                */
  uint16  len;                         /* packet length, starting from OUI */
  uint32  magic_number;                /* packet magic-number              */
  uint32  oui;                         /* OUI of packet                    */  
} ppp_vendor_input_pkt_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_VENDOR_INPUT_F_PTR_TYPE

DESCRIPTION
  Type of the callback function that is called when a vendor specific
  packet is received. This function MUST be provided by the client of this 
  module during registration, and should process the packets for the 
  registered OUI. 

PARAMETERS
  device: PPP device on which packet was received
  pkt_info_ptr: Information about the packet
  pkt_ptr: Pointer to the DSM item containing the packet, with the
           data_ptr pointing to the OUI field, as indicated below.

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |     Code      |  Identifier   |            Length             | 
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
     |                         Magic-Number                          |
  -> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                      OUI                      |     Kind      |   
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |    Value(s) ...
     +-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
typedef void (*ppp_vendor_input_f_ptr_type)
(
  ppp_dev_enum_type                device,       /* device pkt was rcvd on */
  dsm_item_type                  * pkt_ptr,      /* Pointer to pkt         */
  ppp_vendor_input_pkt_info_type * pkt_info_ptr  /* information about pkt  */
);

/*---------------------------------------------------------------------------
  Maximum number of OUIs supported
---------------------------------------------------------------------------*/
#define PPP_OUI_MAX   1
#define PPP_OUI_LEN   3             /* Length of the OUI field in VS pkt   */

/*---------------------------------------------------------------------------
  Type for PPP vendor specific control block 
---------------------------------------------------------------------------*/
typedef struct ppp_vendor_info_s_type
{
  uint32                       oui;          /* vendor-specific OUI        */
  ppp_vendor_input_f_ptr_type  input_f_ptr;  /* input callback function    */
} ppp_vendor_info_s_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PPP_VENDOR_REGISTER()

DESCRIPTION
  This function allows clients to register a input callback function for 
  vendor-specific packets received on a particular PPP device for a 
  particular OUI. 
  
PARAMETERS
  device      : PPP device to register for (Um, Rm etc.)
  oui         : OUI to register for
  input_f_ptr : callback that should be called with the incoming packet.
                This MUST NOT be NULL.

RETURN VALUE
   0 : Registration successful
  -1 : Registration failed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ppp_vendor_register
(
  ppp_dev_enum_type            device,      /* (in) device to register for */
  uint32                       oui,         /* (in) OUI to register for    */
  ppp_vendor_input_f_ptr_type  input_f_ptr  /* (in) input callback func    */
);

/*===========================================================================
FUNCTION PPP_VENDOR_DEREGISTER()

DESCRIPTION
  This function allows clients to de-register on a particular PPP device for a 
  particular OUI. 
  
PARAMETERS
  device      : PPP device to register for (Um, Rm etc.)
  oui         : OUI to register for
 
RETURN VALUE
   0 : De-registration successful
  -1 : De-registration failed
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ppp_vendor_deregister
(
  ppp_dev_enum_type            device,     /* (in) device to register for */
  uint32                       oui         /* (in) OUI to register for    */
);

/*===========================================================================
FUNCTION PPP_VENDOR_SEND()

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
   0 : Packet was sent successfully
  -1 : Failed to send packet

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ppp_vendor_send
(
  ppp_dev_enum_type  device,          /* (in) device to send pkt on        */
  uint16             protocol,        /* (in) PPP protocol field of pkt    */
  uint8              id,              /* (in) packet identifier            */
  dsm_item_type    * pkt_ptr          /* (in) pkt to send                  */
);

#endif /* _PS_PPP_VEND_H */
