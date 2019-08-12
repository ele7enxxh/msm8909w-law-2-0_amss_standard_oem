#ifndef RR_DECODE_BCCH_H
#define RR_DECODE_BCCH_H

/*============================================================================
  @file rr_decode_bcch.h

  Header file for the Decode BCCH state machine

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_decode_bcch.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_events.h"
#include "rr_task.h"
#include "geran_multi_sim.h"

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
  @brief This should be called to initialise the BCCH Decode state machine before use
*/
extern void rr_decode_bcch_list_setup(const gas_id_t gas_id);

/**
  @brief The RR Decode BCCH state machine.

  @param event: An informational event or a command instructing 
  the state machine to take some action. 

  @param message: An optional message sent to the state machine 
  or one of it's sub-state machines.  Only valid if 'event' is 
  set to EV_INPUT_MESSAGE. 
*/
extern void rr_decode_bcch_list_control(
  rr_event_T event, 
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
);

#endif /* RR_DECODE_BCCH_H */

/* EOF */

