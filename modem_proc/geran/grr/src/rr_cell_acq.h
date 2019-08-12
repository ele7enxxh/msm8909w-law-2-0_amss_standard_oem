#ifndef RR_CELL_ACQ_H
#define RR_CELL_ACQ_H

/*============================================================================
  @file rr_cell_acq.h

  Signalling interface for the RR Cell Acquisition module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cell_acq.h#1 $ */

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
  @brief Determine whether RR cell acquisition state machine is suspending MAC.
 
  @param gas_id

  @return TRUE if RR cell acquisition state machine is suspending MAC, FALSE
          otherwise.
*/
extern boolean rr_cell_acq_pending_mac_suspension(
  const gas_id_t gas_id
);

/**
  @brief RR cell acquisition state machine.

  This is the RR cell acquisition state machine.

  @param event_in: An input event.
  @param message:  Optional input message.
  @param gas_id
*/
extern void rr_cell_acq_control(rr_event_T event_in, rr_cmd_bdy_type *message, const gas_id_t gas_id);

#endif /* RR_CELL_ACQ_H */

