#ifndef RRCSEND_H
#define RRCSEND_H
/*===========================================================================
              R R C S E N D  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  SEND CHAIN.
  
Copyright (c) 2000-2003, 2007-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsend.h_v   1.3   30 Aug 2001 18:33:16   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsend.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
05/08/09   ss      Updated Copyright Information
01/13/01   ram     Updated copyright.
10/12/02   rj      Added prototype for function rrcsend_send_ul_sdu_with_mui.
08/30/10   rj      Added prototype for rrcsend_send_rrc_status_message
                   function.
03/22/01   rj      Added additional enum for invalid RLC Size.
02/02/01   rj      Added additional enums for rrcsend_status_e_type to 
                   incorporate AM mode transmition. Added additional 
                   parameters like RRC procedure and L2 Ack status to the 
                   rrcsend_send_ul_sdu function.
12/12/00   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "comdef.h"
#include "queue.h"
#include "rex.h"
#include "rrcdata_v.h"
#include "uecomdef.h"

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/


/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

typedef enum
{
  RRCSEND_SUCCESS,                          /* Successfultransmission */
  RRCSEND_LOGICAL_CHANNEL_NOT_SETUP,        /* Logical Channel was not setup*/
  RRCSEND_LOGICAL_CHANNEL_NOT_SUPPORTED,    /* Logical Channel is not supported */
  RRCSEND_INCOMPATIBLE_L2ACK_REQUEST,       /* Indicates L2 ACK is requested for
                                               Transparent and Unacknowledged 
                                               mode transmission */
  RRCSEND_INVALID_RB_ID,                    /* Identifies invalid RB Id. */
  RRCSEND_FAILED_TO_ENCODE,                 /* ASN1 encoding failed */
  RRCSEND_FAILED_CONVERT_SDU_TO_PACKECT,    /* Failed Convert SDU to DSM Item */
  RRCSEND_FAILED_TO_ENQUEUE_SDU,            /* Failed to enqueue the DSM item
                                               in the watermark queue */
  RRCSEND_INVALID_TM_RLC_SIZE_RECEIVED      /* Failed to get a valid RLC SIZE
                                               to enqueue  TM SDU */
}rrcsend_status_e_type; 

typedef enum
{
  RRCSEND_L2ACK_NOT_REQUIRED,               /* L2 Ack not required */
  RRCSEND_L2ACK_REQUIRED                    /* L2 Ack required */
}rrcsend_l2ack_status_e_type;
/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION rrcsend_send_ul_sdu

DESCRIPTION
  This function will encode the uplink OTA messages using ASN1. Then converts
  the encoded message into DSM Item and places the DSM iterm in the watermark
  queue pointed by the Logical Channel ID.

DEPENDENCIES
  The l2ack_status  should not be set RRCSEND_L2ACK_REQUIRED for Transparent
  and Unacknowleded mode transmission.

RETURN VALUE
  rrcsend_status_e_type : If it is successful, then it returns RRCSEND_SUCCESS. 
  Otherwise it returns one  of the causes for failure.


SIDE EFFECTS
  None
===========================================================================*/

rrcsend_status_e_type rrcsend_send_ul_sdu
(    
  rrc_proc_e_type  rrc_procedure,          /* RRC procedure */
  void             *msg_ptr,               /* Pointer to the Uplink OTA 
                                              message */
  rlc_lc_id_type   lc_id,                  /* Logical Channel Id */
  rrcsend_l2ack_status_e_type l2ack_status /* Indicates whether L2 Ack 
                                              required or not */
);
/*===========================================================================

FUNCTION rrcsend_send_ul_sdu_with_mui

DESCRIPTION
  This function will encode the uplink OTA messages using ASN1. Then converts
  the encoded message into DSM Item and places the DSM iterm in the watermark
  queue pointed by the Logical Channel ID.

DEPENDENCIES
  The l2ack_status  should not be set RRCSEND_L2ACK_REQUIRED for Transparent
  and Unacknowleded mode transmission.

RETURN VALUE
  rrcsend_status_e_type : If it is successful, then it returns RRCSEND_SUCCESS. 
  Otherwise it returns one  of the causes for failure.


SIDE EFFECTS
  None
===========================================================================*/

rrcsend_status_e_type rrcsend_send_ul_sdu_with_mui
(    
  rrc_proc_e_type  rrc_procedure,          /* RRC procedure */
  void             *msg_ptr,               /* Pointer to the Uplink OTA 
                                              message */
  rlc_lc_id_type   lc_id,                  /* Logical Channel Id */
  rrcsend_l2ack_status_e_type l2ack_status, 
                                           /* Indicates whether L2 Ack 
                                              required or not */
  uint8            *mui                    /* MUI tagged to message
                                              that is transmitted */

);

/*===========================================================================

FUNCTION RRCSEND_SEND_RRC_STATUS_MESSAGE

DESCRIPTION
  This function sends an RRC Status Message. This function is typically called
  when the ASN.1 decoder fails or when there is a semantic error in a message.

DEPENDENCIES
  An uplink DCCH must be set up.

RETURN VALUE
  SUCCESS if the message was successfully sent, otherwise FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/

uecomdef_status_e_type rrcsend_send_rrc_status_message
(
  uint32 protocol_error_choice,             // The protocol error cause to be placed in
                                            // the message
  rrc_RRC_TransactionIdentifier transaction_id,
                                           // Transaction Id received

  rrc_ReceivedMessageType received_msg_id, 
                                           // Identification of the Recvd message.
  rrc_proc_e_type rrc_procedure,           // RRC procedure
  rlc_lc_id_type   lc_id,                  /* Logical Channel Id */
  rrcsend_l2ack_status_e_type l2ack_status /* Indicates whether L2 Ack 
                                              required or not */
);
/*==========================================================================*/

#endif /* RRCSEND_H */

/*==========================================================================*/
