#ifndef RR_RR_H
#define RR_RR_H

/*! \file rr_rr.h 
 
  This module contains structures relating to internal RR messages.
 
                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_rr.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_message_set.h"
#include "rr_message_header.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Sends a message on the RR<->RR signalling interface.
 * 
 * \param header_ptr - Pointer to the rr_message_header element of the message to be sent. This must be the 
 *                     first element of the structure.
 * \param message_id - The message ID of the message being sent. This will be copied into the message_id 
 *                     element of the IMH_T structure found within the message header. 
 * \param message_length - The length of the message, including the header, in bytes.
 * \param rr_message_set - The message set that the message to be sent belongs to.
 * \param gas_id - GAS-ID
 */
extern void rr_rr_send(
  rr_message_header_t *header_ptr,
  uint8 message_id,
  uint16 message_length,
  rr_message_set_e rr_message_set,
  gas_id_t gas_id
);

#endif /* RR_RR_H */

/* EOF */

