
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               P S _ P P P _ V E N D . C
                               
DESCRIPTION
  This file contains the data services PPP vendor extensions implementation
  (per RFC 2153)
  
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
  
EXTERNALIZED FUNCTIONS

ppp_vendor_register
  This function allows the client to register for an Organizationally-Unique
  Identifer (OUI)
 
ppp_vendor_send
  This function allows the client to send a Vendor-Specific packet to the
  peer

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c)2006- 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_vend.c#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/09/12    ash    Send Code-Rej when no vendor extensions are registered.
06/06/11    ash    Added support for PPP Partial Context
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
08/31/09    mga    Merged from eHRPD branch
07/14/09    pp     ppp_cb_lock changes.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
05/07/09    pp     CMI Phase-4: SU Level API Effort.
11/10/08    scb    Fixed QTF compilation warnings.
04/11/08    scb    Klocwork fixes
04/27/07    scb    Removed LINT High errors
04/02/07    ssh    ppp_vendor_send() now sends a command to PS
12/28/06    ssh    Created module
===========================================================================*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"
#include "customer.h"
#include "rex.h"
#include "task.h"

#include "ps_ppp_task.h"
#include "ps_ppp_vend.h"
#include "ps_pppi_vend.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi_fsm.h"
#include "ps_mem.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#define PPP_VS_LEN_MIN  12          /* Minimum length of a VS pkt          */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
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
)
{
  ppp_type               * ppp_cb_ptr = NULL;
  ppp_type               * ppp_cb_array = NULL;
  ppp_vendor_info_s_type * vendor_info_ptr = NULL;
  uint8                    index = 0;
  int32                    retval = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( device >= PPP_MAX_DEV  ) || ( input_f_ptr == NULL ) )
  {
    LOG_MSG_ERROR_3("ppp_vendor_register(): Invalid params, dev %d, oui 0x%x "
              "input_f_ptr = %p", device, oui, input_f_ptr);
    return -1;
  }
  
  ppp_cb_array = fsm_get_ppp_cb_array();  
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  
  for( index = 0 ; index < PPP_OUI_MAX ; index++ )
  {
    if( ppp_cb_ptr->vendor_cb_array[index].oui == 0 )
    {
      vendor_info_ptr = &(ppp_cb_ptr->vendor_cb_array[index]);
      vendor_info_ptr->oui         = oui;
      vendor_info_ptr->input_f_ptr = input_f_ptr;
      retval = 0;
      
      LOG_MSG_INFO2_2("Registered OUI 0x%x, dev %d", oui, device );
      break;
    }
  } /* for */

  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  return retval;

} /* ppp_vendor_register() */

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
)
{
  ppp_type               * ppp_cb_ptr = NULL;
  ppp_type               * ppp_cb_array = NULL;
  ppp_vendor_info_s_type * vendor_info_ptr = NULL;
  uint8                    index = 0;
  int32                    retval = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( device >= PPP_MAX_DEV  ) )
  {
    LOG_MSG_ERROR_2("ppp_vendor_deregister(): Invalid params, dev %d, oui 0x%x ",
                    device, oui);
    return -1;
  }
  
  ppp_cb_array = fsm_get_ppp_cb_array();  
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  
  for( index = 0 ; index < PPP_OUI_MAX ; index++ )
  {
    if( ppp_cb_ptr->vendor_cb_array[index].oui == oui )
    {
      vendor_info_ptr = &(ppp_cb_ptr->vendor_cb_array[index]);
      vendor_info_ptr->oui         = 0;
      vendor_info_ptr->input_f_ptr = NULL;
      retval = 0;
      
      LOG_MSG_INFO2_2("De-registered OUI 0x%x, dev %d", oui, device );
      break;
    }
  } /* for */

  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  return retval;

} /* ppp_vendor_deregister() */


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
)
{
  ppp_vendor_pkt_info_s_type  * pkt_info = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( ( device >= PPP_MAX_DEV ) || ( pkt_ptr == NULL ) )
  {
    LOG_MSG_ERROR_2("ppp_vendor_send(): Invalid params, dev %d, pkt_ptr = %p",
                    device, pkt_ptr);
    return -1;
  }
 
  LOG_MSG_INFO2_2("ppp_vendor_send(), proto 0x%x, id = %d", protocol, id);

  /*-------------------------------------------------------------------------
    Get a pkt info buffer and send a PPP_VENDOR_SEND_CMD
  -------------------------------------------------------------------------*/
  pkt_info = (ppp_vendor_pkt_info_s_type *)
                    ps_mem_get_buf(PS_MEM_PPP_CMD_BUF_TYPE);
      
  if(pkt_info == NULL)
  {
    LOG_MSG_ERROR_0("ppp_vendor_send(): Cannot obtain ppp_vendor_pkt_info_s_type"
                    " memory buffer");
    return -1;
  }
  pkt_info->device   = device;
  pkt_info->protocol = protocol;
  pkt_info->id       = id;
  pkt_info->pkt_ptr  = pkt_ptr;

  PPP_SEND_CMD( PPP_VENDOR_SEND_CMD, (void *)pkt_info );

  return 0;
} /* ppp_vendor_send() */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPPI_VENDOR_EXT_HDLR()

DESCRIPTION
  This function handles incoming vendor-specific packets (code field 0) 
  containing proprietary vendor extensions for LCP and NCPs (IPCP, IPv6CP).
  The registered callback is invoked for the OUI for which the packet is 
  received. 
  The PPP configuration header (code, id, len) should have been removed
  and the item data_ptr should be pointing to the magic-number field.
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
)
{
  uint8                    index = 0;
  ppp_type               * ppp_cb_ptr = NULL;
  ppp_type               * ppp_cb_array = NULL;
  uint16                   pkt_len = 0;
  uint32                   oui = 0x0;
  uint32                   magic_number;
  ppp_vendor_info_s_type * vendor_info_ptr = NULL;
  ppp_vendor_input_pkt_info_type pkt_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( device >= PPP_MAX_DEV ) || ( hdr == NULL ) || ( pkt_ptr == NULL ) )
  {
    LOG_MSG_ERROR_0("pppi_vendor_hdlr: Invalid params, dev %d" );
    return;
  }

  ASSERT( hdr->code == VENDOR_SPECIFIC );

  pkt_len = hdr->length;
  
  /*-------------------------------------------------------------------------
    Validate packet length. If packet length is less than 8 bytes, discard 
    it. The minimum length of a vendor-specific packet is 12 bytes; note that
    the header (4 bytes) has already been stripped by the caller and 
    pkt_ptr->data_ptr is pointing as indicated below.

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |     Code      |  Identifier   |            Length             | 
  -> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
     |                         Magic-Number                          |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                      OUI                      |     Kind      |   
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |    Value(s) ...
     +-+-+-+-+-+-+-+-+
  -------------------------------------------------------------------------*/  
  if( ( pkt_len < ( PPP_VS_LEN_MIN - CONFIG_HDR_LEN ) ) )
  {
    LOG_MSG_ERROR_1("Dev %d rxed malformed VS pkt, discarding", device );
    dsm_free_packet( &pkt_ptr );
    return;
  }
  
  /*-------------------------------------------------------------------------
    Remove the magic-number
  -------------------------------------------------------------------------*/
  if ( dsm_pullup( &pkt_ptr, &magic_number, LCP_MAGIC_LEN ) < LCP_MAGIC_LEN )
  {
    dsm_free_packet( &pkt_ptr );
    return;
  }
  magic_number = ps_ntohl( magic_number );
  pkt_len -= LCP_MAGIC_LEN;

  /*-------------------------------------------------------------------------
    Extract the OUI 
  -------------------------------------------------------------------------*/
  if( dsm_extract( pkt_ptr, 0, &oui, PPP_OUI_LEN ) != PPP_OUI_LEN )
  {
    LOG_MSG_ERROR_1("dsm_extract() failed, pkt = 0x%x", pkt_ptr);
    dsm_free_packet( &pkt_ptr );
    return;
  }

  oui <<= 8;                /* lshift needed since OUI is only 3-byte wide */
  oui = ps_ntohl(oui );
      
  ppp_cb_array = fsm_get_ppp_cb_array();      
  ppp_cb_ptr = ppp_cb_array + (uint8)device;
  
  /*-------------------------------------------------------------------------
    If a callback is registered for this OUI, call it
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  
  for( index = 0 ; index < PPP_OUI_MAX ; index++ )
  {
    if( ppp_cb_ptr->vendor_cb_array[index].oui == oui )
    {
      LOG_MSG_INFO2_1("Invoking registered function for OUI 0x%x", oui);
      pkt_info.protocol     = protocol;
      pkt_info.id           = hdr->identifier;
      pkt_info.len          = pkt_len;
      pkt_info.magic_number = magic_number;
      pkt_info.oui          = oui;
      
      vendor_info_ptr = &(ppp_cb_ptr->vendor_cb_array[index]);
      vendor_info_ptr->input_f_ptr( device, pkt_ptr, &pkt_info );
      
      PS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      return;
    }
  } /* for */
  
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  
  /*-------------------------------------------------------------------------
    No Vendor extension registered, send LCP CODE REJ.
    see RFC 1661 Section 5.6 Code-Reject
    Put LCP header back on to restroe incoming packet.
  -------------------------------------------------------------------------*/
  hdr->length += CONFIG_HDR_LEN;  /* restore length */
  pkt_ptr = htoncnf( hdr, pkt_ptr );  /* put header back on */
  if ( NULL != pkt_ptr)
  {
    (void)fsm_send( &ppp_cb_ptr->fsm[LCP], 
                     CODE_REJ, 
                     hdr->identifier, 
                     pkt_ptr );
  }

} /* pppi_vendor_ext_hdlr() */


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
)
{
  ppp_fsm_type * fsm_ptr = NULL;
  ppp_type     * ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( ( device >= PPP_MAX_DEV ) || ( pkt_ptr == NULL ) )
  {
    LOG_MSG_ERROR_2("pppi_vendor_send(): Invalid params, dev %d, pkt_ptr = %p",
                    device, pkt_ptr);
    return;
  }

  ASSERT(PPP_IS_CURR_TASK_OWNER());

  ppp_cb_array = fsm_get_ppp_cb_array();
  switch( protocol )
  {
    case PPP_LCP_PROTOCOL:
      fsm_ptr = &(ppp_cb_array[device].fsm[PPP_PROTOCOL_LCP]);
      if( fsm_ptr->allow_vendor_pkt == TRUE )
      {
        (void)fsm_send( fsm_ptr, VENDOR_SPECIFIC, id, pkt_ptr );
        return;
      }
      else
      {
        LOG_MSG_ERROR_3("Failed to send vendor-specific pkt id %d, dev %d, "
                  "proto 0x%x", id, device, protocol);
        dsm_free_packet( &pkt_ptr );
        return;
      }

    case PPP_IPCP_PROTOCOL:
    case PPP_IPV6CP_PROTOCOL:
    case PPP_VSNP_PROTOCOL:
    case PPP_VSP_PROTOCOL:
    case PPP_VSNCP_PROTOCOL:
    case PPP_VSAP_PROTOCOL:
    default:
      LOG_MSG_INFO2_2("Vendor-specific packets not supported for proto 0x%x, dev %d",
                      protocol, device);
      dsm_free_packet( &pkt_ptr );
      return;
      
  } /* switch( protocol ) */

} /* pppi_vendor_send() */
