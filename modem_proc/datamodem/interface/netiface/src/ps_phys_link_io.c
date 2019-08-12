/*=========================================================================*/
/*!
  @file
  ps_phys_link_io.c

  @brief
  This file abstracts the data path functionality of the PS phys link module.

  @see
  ps_iface.c

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_phys_link_io.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2014-10-09 ds  Adding FEATURE_DATA_PS_ROHC   
  2011-11-01 am  Mainlining opt DL for NAT support.
  2011-09-22 am  Added support for opt DL MBIM.
  2008-07-09 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_phys_link.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "dsm.h"
#include "ps_tx_meta_info.h"
#ifdef FEATURE_DATA_PS_ROHC
#include "ps_rohc.h"
#endif /* FEATURE_DATA_PS_ROHC */
#include "ps_iphc.h"

#ifdef FEATURE_DATA_PS_PPP
#include "ps_ppp.h"
#endif

#include "ps_utils.h"
#include "ps_mem.h"
#include "ps_crit_sect.h"

#ifdef FEATURE_DATACOMMON_PS_IFACE_IO

#include "ds_Utils_DebugMsg.h"
#include "ps_dpm_dl_opt.h"

/*===========================================================================
FUNCTION PS_PHYS_LINKI_DEFAULT_TX_CMD()

DESCRIPTION
  This function is the default TX function.  It is assigned to the tx
  function pointer when no explicit TX function is registered.

  It frees the data passed in.

PARAMETERS
  this_phys_link_ptr:    Phys link on which this is being called
  pkt_ref_ptr:       ref to dsm item ptr for Tx - freed
  meta_info_ptr:     pointer to meta info type - also freed.
  tx_cmd_info:       ignored

RETURN VALUE
  -1: indicating error

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_linki_default_tx_cmd
(
  ps_phys_link_type     * this_phys_link_ptr,
  dsm_item_type        ** pkt_ref_ptr,
  ps_tx_meta_info_type  * meta_info_ptr,
  void                  * tx_cmd_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    free the meta information, and the dsm packet
  -------------------------------------------------------------------------*/
  PS_TX_META_INFO_FREE(&meta_info_ptr);
  dsm_free_packet(pkt_ref_ptr);

  return;
} /* ps_phys_linki_default_tx_cmd() */

/*===========================================================================
FUNCTION PS_IFACE_SET_TX_FUNCTION()

DESCRIPTION
  This function is used to set the TX function.  Unsetting the function is
  done by passing in NULL for the tx_cmd parameter.

PARAMETERS
  this_iface_ptr:  interface on which to operate
  tx_cmd:          Tx command being set
  tx_cmd_info:     Info to pass to registered cmd handler

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_set_tx_function
(
  ps_phys_link_type            *this_phys_link_ptr,
  ps_phys_link_tx_cmd_f_ptr_type tx_cmd,
  void                          *tx_cmd_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    LOG_MSG_INFO1_0("ps_phys_link_set_tx_function(): "
                    "Invalid physlink ptr");
    ASSERT(0);
    return;
  }

  if(tx_cmd != NULL)
  {
    this_phys_link_ptr->tx_f_ptr = tx_cmd;
  }
  else
  {
    this_phys_link_ptr->tx_f_ptr = ps_phys_linki_default_tx_cmd;
  }

  this_phys_link_ptr->tx_cmd_info = tx_cmd_info;

} /* ps_phys_link_set_tx_function() */

/*===========================================================================
FUNCTION PS_PHYS_LINK_TX_CMD()

DESCRIPTION

PARAMETERS
  this_phys_link_ptr:ptr to physlink control block on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received for tx
  meta_info_ptr:     ptr to meta info (per pkt information)

RETURN VALUE
  0: if the packet was transmitted
 -1: if not

DEPENDENCIES
  None

SIDE EFFECTS
  The memory that was passed in will be freed by one of the tx functions.
===========================================================================*/
int ps_phys_link_tx_cmd
(
  ps_phys_link_type                           *this_phys_link_ptr,
  dsm_item_type                              **pkt_ref_ptr,
  ps_phys_link_higher_layer_proto_enum_type    protocol,
  ps_tx_meta_info_type                        *meta_info_ptr
)
{
  errno_enum_type err;
  iphc_packet_type_e_type iphc_pkt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pkt_ref_ptr == NULL || *pkt_ref_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_phys_link_tx_cmd(): Null packet to transmit");
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Phys link can be NULL for LTE where phys links are dynamically assigned.
    If a call is torn down in the middle of data transfer, primary phys link
    could be NULL
  -------------------------------------------------------------------------*/
  if (this_phys_link_ptr == NULL)
  {
    LOG_MSG_INFO1_0("ps_phys_link_tx_cmd(): "
                    "NULL physlink");
    dsm_free_packet(pkt_ref_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  if (!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    LOG_MSG_INFO1_0("ps_phys_link_tx_cmd(): "
                    "Invalid physlink ptr");
    ASSERT(0);
    dsm_free_packet(pkt_ref_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    HDR compression
  -------------------------------------------------------------------------*/
  switch(this_phys_link_ptr->hdr_comp_protocol)
  {
#ifdef FEATURE_DATA_PS_ROHC
    case PS_PHYS_LINK_HDR_COMP_PROTOCOL_ROHC:
      if(rohc_compress(this_phys_link_ptr->hdr_comp_handle,
                       NULL,
                       NULL,
                       pkt_ref_ptr,
                       &err) == -1)
      {
        LOG_MSG_ERROR_1("ps_phys_link_tx_cmd(): "
                        "Couldn't compress with ROHC: err %d. Dropping packet",
                        err);
        dsm_free_packet(pkt_ref_ptr);
        PS_TX_META_INFO_FREE(&meta_info_ptr);
        return -1;
      }
      else
      {
        protocol = PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_ROHC;
      }
    break;
#endif /* FEATURE_DATA_PS_ROHC */

    case PS_PHYS_LINK_HDR_COMP_PROTOCOL_IPHC:
      /*-------------------------------------------------------------------
        If IPHC is set up, attempt IPHC compression
      -------------------------------------------------------------------*/
      if(iphc_compress(this_phys_link_ptr->hdr_comp_handle,
                       pkt_ref_ptr,
                       &iphc_pkt) == -1)
      {
        LOG_MSG_ERROR_0("ps_phys_link_tx_cmd(): "
                        "Couldn't compress with IPHC. Dropping packet");
        dsm_free_packet(pkt_ref_ptr);
        PS_TX_META_INFO_FREE(&meta_info_ptr);
        return -1;
      }
      else
      {
        protocol = PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IPHC;
      }
    break;

    case PS_PHYS_LINK_HDR_COMP_PROTOCOL_NONE:
      break;

    default:
      LOG_MSG_INFO3_1("ps_phys_link_tx_cmd(): "
                      "Un recognized header compression protocol (%d)",
                      this_phys_link_ptr->hdr_comp_protocol);
      break;
  }

  /*-------------------------------------------------------------------------
   Link Layer
  -------------------------------------------------------------------------*/
  if (this_phys_link_ptr->link_protocol_info.link_protocol !=
        PS_PHYS_LINK_LINK_PROTOCOL_NONE &&
      this_phys_link_ptr->link_protocol_info.link_tx_f_ptr != NULL)
  {
    (void) this_phys_link_ptr->link_protocol_info.link_tx_f_ptr
           (
             this_phys_link_ptr->link_protocol_info.link_protocol_handle,
             pkt_ref_ptr,
             protocol,
             meta_info_ptr
           );
  }

  /*-------------------------------------------------------------------------
   If the link layer is a library we have to transmit the packet.
    Transmit function
  -------------------------------------------------------------------------*/
  if (*pkt_ref_ptr != NULL)
  {
    this_phys_link_ptr->tx_f_ptr(this_phys_link_ptr,
                                 pkt_ref_ptr,
                                 meta_info_ptr,
                                 this_phys_link_ptr->tx_cmd_info);
  }

  return 0;
}


/*===========================================================================
FUNCTION PS_PHYS_LINK_INPUT()

DESCRIPTION


PARAMETERS
  this_phys_link_ptr:ptr to physlink control block on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (per pkt information)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_input
(
  const ps_phys_link_type  * this_phys_link_ptr,
  dsm_item_type           ** pkt_ref_ptr,
  ps_rx_meta_info_type     * meta_info_ptr
)
{
  dsm_item_type            * fb_pkt = NULL;
  ip_version_enum_type       pkt_ver;
  errno_enum_type            err;
  iphc_packet_type_e_type    pkt_type = IPHC_PACKET_TYPE_NORMAL_IPV4;
  ps_dpm_bearer_info_type   * bearer_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    LOG_MSG_INFO1_0("ps_phys_link_input(): "
                    "Invalid physlink ptr");
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
   Link Layer.
   TODO: Link layer should return packet type.
  -------------------------------------------------------------------------*/
  if (this_phys_link_ptr->link_protocol_info.link_protocol !=
        PS_PHYS_LINK_LINK_PROTOCOL_NONE &&
    this_phys_link_ptr->link_protocol_info.link_rx_f_ptr != NULL)
  {
    (void) this_phys_link_ptr->link_protocol_info.link_rx_f_ptr
           (
             this_phys_link_ptr->link_protocol_info.link_protocol_handle,
             this_phys_link_ptr->framing_handle,
             pkt_ref_ptr
           );
  }

  /*-------------------------------------------------------------------------
   if the link layer is a library
  -------------------------------------------------------------------------*/
  if(*pkt_ref_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      decompress header
    -----------------------------------------------------------------------*/
    switch(this_phys_link_ptr->hdr_comp_protocol)
    {

      case PS_PHYS_LINK_HDR_COMP_PROTOCOL_NONE:
        break;
#ifdef FEATURE_DATA_PS_ROHC
      case PS_PHYS_LINK_HDR_COMP_PROTOCOL_ROHC:
        if(rohc_decompress(this_phys_link_ptr->hdr_comp_handle,
                           NULL,
                           NULL,
                           pkt_ref_ptr,
                           &pkt_ver,
                           &fb_pkt,
                           0,
                           &err) == -1)
        {
          LOG_MSG_INFO1_1("ps_phys_link_input(): "
                          "Couldn't decompress with ROHC err (%d). Dropping packet",
                          err);
          dsm_free_packet(pkt_ref_ptr);
          PS_RX_META_INFO_FREE(&meta_info_ptr);
          return -1;
        }

        if(fb_pkt != NULL)
        {
          LOG_MSG_INFO1_0("ps_phys_link_input(): "
                          "No support for forwarding feedback");
          dsm_free_packet(&fb_pkt);
        }

        break;
#endif /* FEATURE_DATA_PS_ROHC */

      case PS_PHYS_LINK_HDR_COMP_PROTOCOL_IPHC:
        if (iphc_decompress(this_phys_link_ptr->hdr_comp_handle,
                            pkt_ref_ptr,
                            &pkt_type,
                            &fb_pkt) == -1)
        {
          LOG_MSG_INFO1_0("ps_phys_link_input(): "
                          "IPHC decompress error");
          dsm_free_packet(pkt_ref_ptr);
          PS_RX_META_INFO_FREE(&meta_info_ptr);
          return -1;
        }

        break;

      default:
        LOG_MSG_INFO1_1("ps_phys_link_input(): "
                        "Unrecognized header compression protocol (%d)",
                        this_phys_link_ptr->hdr_comp_protocol);
        break;
    }

    /*-----------------------------------------------------------------------
      Send it to upper layer.
    -----------------------------------------------------------------------*/
    bearer_ptr = ps_dpmi_get_bearer_from_phys_link(this_phys_link_ptr);
    (void)ps_dl_phys_opt_rx_pkt(this_phys_link_ptr, pkt_ref_ptr, meta_info_ptr, bearer_ptr );
  }

  return 0;
}


/*===========================================================================
FUNCTION PS_PHYS_LINK_SET_LINK_PROTOCOL

DESCRIPTION
  Set the link protocol.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  protocol:  Link protocol
  handle :  link protocol handle.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  Set the link protocol
===========================================================================*/
int ps_phys_link_set_link_protocol
(
  ps_phys_link_type                      *this_phys_link_ptr,
  ps_phys_link_link_protocol_enum_type    link_protocol,
  ps_phys_link_link_protocol_handle_type  handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    LOG_MSG_INFO1_0("ps_phys_link_set_link_protocol(): "
                    "Invalid physlink ptr");
    ASSERT(0);
    return -1;
  }

  switch(link_protocol)
  {
#ifdef FEATURE_DATA_PS_PPP
    case PS_PHYS_LINK_LINK_PROTOCOL_PPP:
      this_phys_link_ptr->link_protocol_info.link_tx_f_ptr =
        (ps_phys_link_link_tx_f_ptr_type) ppp_output;
      this_phys_link_ptr->link_protocol_info.link_rx_f_ptr =
        (ps_phys_link_link_rx_f_ptr_type) ppp_input;
      this_phys_link_ptr->link_protocol_info.link_protocol_handle =
        (void *)handle.ppp_device;
      break;
#endif

    case PS_PHYS_LINK_LINK_PROTOCOL_NONE:
      this_phys_link_ptr->higher_layer_protocol =
        handle.none_handle.high_protocol;
      this_phys_link_ptr->rx_cmd_info = handle.none_handle.handle;
        /* By pass PPP */
      this_phys_link_ptr->link_protocol_info.link_tx_f_ptr = NULL;
      this_phys_link_ptr->link_protocol_info.link_rx_f_ptr = NULL;
    break;

    default:
      LOG_MSG_INFO1_1("ps_phys_link_set_link_protocol(): "
                      "Un supported link protocol (%d)", link_protocol);
      return -1;
  }

  this_phys_link_ptr->link_protocol_info.link_protocol = link_protocol;
  return 0;
}


/*===========================================================================
FUNCTION PS_PHYS_LINK_SET_HDR_COMP_HANDLE

DESCRIPTION
  Set the header compression handle

PARAMETERS
  this_phys_link_ptr: pointer to physlink
  protocol:  Header compression protocol
  handle :  Header compression protocol handle.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

int ps_phys_link_set_hdr_comp_handle
(
  ps_phys_link_type                        *this_phys_link_ptr,
  ps_phys_link_hdr_comp_protocol_enum_type  hdr_comp_protocol,
  void                                     *hdr_comp_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(hdr_comp_protocol > PS_PHYS_LINK_HDR_COMP_PROTOCOL_MAX)
  {
    LOG_MSG_INFO1_1("ps_phys_link_set_hdr_comp_handle(): "
                    "Unsupported compression protocol (%d)",
                    hdr_comp_protocol);
    return -1;
  }

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    LOG_MSG_INFO1_0("ps_phys_link_set_hdr_comp_handle(): "
                    "Invalid physlink ptr");
    ASSERT(0);
    return -1;
  }

  this_phys_link_ptr->hdr_comp_protocol = hdr_comp_protocol;
  this_phys_link_ptr->hdr_comp_handle = hdr_comp_handle;

  return 0;
}


/*===========================================================================
FUNCTION PS_PHYS_LINK_SET_FRAMING_HANDLE

DESCRIPTION
  Set the framing compression handle

PARAMETERS
  this_phys_link_ptr: pointer to physlink
  protocol:  Framing/unframing protocol
  handle :  Framing/unframing protocol handle.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

int ps_phys_link_set_framing_handle
(
  ps_phys_link_type                       *this_phys_link_ptr,
  ps_phys_link_framing_protocol_enum_type  framing_protocol,
  void                                    *framing_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(framing_protocol > PS_PHYS_LINK_FRAMING_PROTOCOL_MAX)
  {
    LOG_MSG_INFO1_1("ps_phys_link_set_framing_handle(): "
                    "Unsupported framing protocol (%d)", framing_protocol);
    return -1;
  }

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    LOG_MSG_INFO1_0("ps_phys_link_set_framing_handle(): "
                    "Invalid physlink ptr");
    ASSERT(0);
    return -1;
  }

  this_phys_link_ptr->framing_protocol = framing_protocol;
  this_phys_link_ptr->framing_handle = framing_handle;

  return 0;
}

#endif
