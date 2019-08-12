#ifndef HDRMRLPSIG_H
#define HDRMRLPSIG_H

/*===========================================================================

          M U L T I - F L O W   R A D I O   L I N K   P R O T O C O L 
                  

DESCRIPTION
  This contains the declarations for the Multiflow Radio Link Protocol.

Copyright (c) 2004-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlpsig.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
10/27/05   ksu     EMPA: octet stream, octet sequencing
09/28/05   ksu/sy  Call hdr[m]rlpi_rx_process_sig_msg() directly from hdr[m]rlp_msg_cb()
06/10/05   vas/vrk added feature FEATURE_HDR_QOS
03/25/05   sy      Added support for Reservation signalling messages and deleted
                   the RLP signalling messages.
09/16/04   vas     Changed function prototype
04/04/04   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_HDR_QOS

#include "dsrlpi.h"
#include "hdrmrlp.h"
/* <EJECT> */

/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRMRLPI_SEND_FWD_RLP_SIG_MSG

DESCRIPTION
  This function builds up any pending Forward RLP messages and queues them
  directly to SLP, bypassing the conventional HMP layer queueing.
      
  In each case, the message is built-up, and passed directly to SLP.  SLP is
  responsible for de-allocating the dsm memory items used to build up the
  signaling message.

DEPENDENCIES
  Assumes that a connection is open.  HMP need not know the message is being
  transmitted.

RETURN VALUE
  None

SIDE EFFECTS
  RLP signaling messages are queued on an SLP Tx queue.  

===========================================================================*/
void hdrmrlpi_send_fwd_rlp_sig_msg
(
 dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
 uint8  hmp_hdr /* The HMP Header to be appended to the signaling message */
);

/*===========================================================================

FUNCTION HDRMRLPI_SEND_REV_RLP_SIG_MSG

DESCRIPTION
  This function builds up any pending Reverse RLP messages and queues them
  directly to SLP, bypassing the conventional HMP layer queueing.
      
  In each case, the message is built-up, and passed directly to SLP.  SLP is
  responsible for de-allocating the dsm memory items used to build up the
  signaling message.

DEPENDENCIES
  Assumes that a connection is open.  HMP need not know the message is being
  transmitted.

RETURN VALUE
  None

SIDE EFFECTS
  RLP signaling messages are queued on an SLP Tx queue.  

===========================================================================*/
void hdrmrlpi_send_rev_rlp_sig_msg
(
 dsrlpi_rlp_ctl_blk_type *rrcb_ptr,
 uint8  hmp_hdr /* The HMP Header to be appended to the signaling message */
);

/*===========================================================================

FUNCTION HDRMRLPI_SEND_ACCEPT_MSG

DESCRIPTION
  This function builds up any pending RLP Accept messages and queues them
  directly to SLP, bypassing the conventional HMP layer queueing.
      
  In each case, the message is built-up, and passed directly to SLP.  SLP is
  responsible for de-allocating the dsm memory items used to build up the
  signaling message.

DEPENDENCIES
  HMP need not know the message is being transmitted.

RETURN VALUE
  None

SIDE EFFECTS
  RLP signaling messages are queued on an SLP Tx queue.  

===========================================================================*/
void hdrmrlpi_send_accept_msg
(
 hdrmrlp_app_ctl_blk_type *pacb_ptr, /* Application Control Block - AN/SN  */
 hdrcom_hai_stream_enum_type stream_id   /* Stream to send message on          */
);


/*===========================================================================

FUNCTION HDRMRLPI_RX_PROCESS_SIG_MSG

DESCRIPTION

PARAMETERS
  pacb_ptr - Pointer to the Packet Application Control Block
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void hdrmrlpi_rx_process_sig_msg
(
  hdrmrlp_app_ctl_blk_type *pacb_ptr,
  dsm_item_type            *msg_ptr,          /* ptr to the message        */
  hdrcom_hai_app_subtype_enum_type app_subtype /* Application Subtype          */
);

/*===========================================================================

FUNCTION  HDRMRLPI_STARTUP

DESCRIPTION
  Called once at powerup.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpi_startup( void );

#endif /* FEATURE_HDR_QOS */
#endif /* HDRMRLPSIG_H */
