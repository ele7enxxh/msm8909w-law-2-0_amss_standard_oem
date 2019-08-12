/*============================================================================
  @file rr_msgr.h

  @brief This module provides a wrapper function to send a message via
         message router, and print the message name as a OMsg: debug string.

                Copyright (c) 2009-2014 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_msgr.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"

#include "geran_variation.h"
#include "geran_multi_sim.h"
#include "msgr.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Sends a message via MessageRouter.

  This function will pring a debug string indicating the message being sent,
  then send that message via MessageRouter.

  @param msg_ptr Pointer to message to be sent. Note that the first member of
         the message that is pointed to by this pointer must be of
         msgr_hdr_struct_type type.
  @param msg_len Total message length (header and payload) in bytes.
  @return E_BAD_ADDRESS if msg_ptr is NULL, otherwise the return value of
          rr_msgr_send is returned.
*/
extern errno_enum_type rr_msgr_send(
  msgr_hdr_struct_type * msg_ptr,
  uint32                 msg_len,
  const gas_id_t         gas_id
);

