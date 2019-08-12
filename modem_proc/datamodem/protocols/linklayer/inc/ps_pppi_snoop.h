#ifndef PS_PPPI_SNOOP_H
#define PS_PPPI_SNOOP_H
/*===========================================================================

                         P S _ P P P I _ S N O O P . H

DESCRIPTION

 The Data Services Internal Snoop Header File. 

Copyright (c) 2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_snoop.h_v   1.2   13 Feb 2003 14:12:46   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_pppi_snoop.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/09   pp      Initial release. Created as part of CMI Phase-4: SU Level 
                   API Effort [Split from ps_pppi_snoop.h].
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ps_ppp_defs.h"
#include "dsm.h"
#include "ps_rx_meta_info.h"

/*---------------------------------------------------------------------------
  Type that is used as the return value for snoop_early_proto_detect().  It
  is used to determine what action needs to be taken on the return from that
  function.
---------------------------------------------------------------------------*/
typedef enum
{
  SNOOP_IGNORE_PKT,        /* packet not interesting, ignore it            */
  SNOOP_DO_UNFRAMING,      /* has protocol we are looking for, unframe it! */
  SNOOP_NEXT_TWO_BYTES     /* need two more bytes to determine action      */
} snoop_proto_ret_val_enum_type;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                      PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION  SNOOP_INIT()

DESCRIPTION
  This function initiailzes the snoop data structures, namely it zeros out
  the data structure that contains all of the call back functions.  This
  function is called whenever a non packet call is made (i.e. sockets,
  async...)

DEPENDENCIES
  It is assumed that one can zero out a multidimentional array by knowing
  it's base address, the range of all of it's indices and the size of the
  base data type.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void snoop_init(void);

/*===========================================================================
FUNCTION  SNOOP_UNREG_ALL()

DESCRIPTION
  This function removes all callback functions that are registered for ALL
  protocols.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void snoop_unreg_all(void);

/*===========================================================================
FUNCTION  SNOOP_EARLY_PROTO_DETECT()

DESCRIPTION
  This function does early protocol detection of unframed PPP packets. It is
  passed a dsm_item that contains either the first two or four UNESCAPED
  bytes of a PPP packet. These are used to determine if this is a packet we
  are interested in.

  The item_ptr argument must have either 2 or 4 bytes.

  The item_ptr->kind field specifies whether this pkt. was on the Um or the
  Rm interface.

DEPENDENCIES
  This function expects that the first two or four bytes of the PPP packet
  are the first two or four bytes of the first DSM item.

RETURN VALUE
  One of three values:
  SNOOP_IGNORE_PKT:     Return to looking only for the 7E
  SNOOP_DO_UNFRAMING:   Unframe whole packet are send to snoop
  SNOOP_NEXT_TWO_BYTES: Not enough information here, need to look at next
                            two bytes.

SIDE EFFECTS
  None
===========================================================================*/
snoop_proto_ret_val_enum_type snoop_early_proto_detect
(
  ppp_dev_enum_type     in_iface,       /* iface (Rm or Um) pkt arrived on */
  const dsm_item_type * item_ptr        /* Item containing input PPP pkt.  */
);

/*===========================================================================
FUNCTION  SNOOP_PROTO_MSG_DETECT()

DESCRIPTION
  This function is given a pointer to a dsm_item_ptr and looks at the
  protocol and msg fields to determine if a callback function has been
  registered for this protocol and message combination.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item.
  It is also expected that the FF03 has been removed from the front and that
  the protocol ID has been stripped from the packet. The protocol of the 
  packet is known and is passed as an input parameter.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT      packet processing complete, ignore it
  SNOOP_CB_SNOOP_AGAIN     call snoop callback again with the same DSM item -
                           implies that there is a new callback that needs 
                           to process this packet
  SNOOP_CB_PROCESS_PKT     send pkt up the stack for further processing

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type snoop_proto_msg_detect
(
  ppp_dev_enum_type iface,             /* iface (Rm or Um) pkt coming on   */
  uint16            protocol,          /* protocol of packet               */
  dsm_item_type   **item_head_ptr,     /* pointer to packet chain of data  */
  ps_rx_meta_info_type **meta_info_ref_ptr/* Pointer to ref to PS meta info*/
);

#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION  SNOOP_IS_REGISTRTION_PKT()

DESCRIPTION
  This function is given a pointer to a dsm_item_ptr and looks at the
  packet to determine if it is a MIP registration reply packet or an
  agent advertisement.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item.
  It is also expected that the FF03 has been removed from the front and that
  the protocol ID has been stripped from the packet. The protocol of the 
  packet is known and is passed as an input parameter.

RETURN VALUE
  TRUE: The packet is a MIP registration related packet (process internally)
  FALSE: The packet is not a MIP reg. pkt (can forward to laptop)

SIDE EFFECTS
  None
===========================================================================*/
boolean snoop_is_registration_pkt
(
  dsm_item_type **item_head_ptr,       /* pointer to packet chain of data  */
  uint16 protocol                      /* protocol of packet               */
);
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* PS_PPPI_SNOOP_H */
