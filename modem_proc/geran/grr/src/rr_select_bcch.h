#ifndef RR_SELECT_BCCH_H
#define RR_SELECT_BCCH_H

/*! \file rr_select_bcch.h 
 
  This header file contains the definitions and external function prototypes
  needed to interact with the RR-SELECT-BCCH module.
  Also see the associated interface file, rr_select_bcch_if.h.
 
  The RR-SELECT-BCCH module allows a given cell to be selected, by requesting L1
  to tune to the BCCH and read the system information. Early camping can be
  performed if required. A reselection power-scan can also be performed
  first to verify that the cell is present, if required.
 
  The process is started by a RR-SELECT-BCCH IMSG, which specifies the various
  options for selecting the cell, performing reselection power-scan,
  early-camping, etc. The module will perform the required action, and report
  the result with a RR-SELECT-BCCH OMSG. The module will also guard against a
  system information reception time-out.
 
  The list of options are detailed in the interface description, see
  rr_select_bcch_if.h
 
                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_select_bcch.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
#include "geran_dual_sim.h"

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
 * \brief Initialisation function.
 * 
 * \param gas_id 
 */
extern void rr_select_bcch_init(
  const gas_id_t gas_id
);

/*!
 * \brief This is the main RR-SELECT-BCCH state machine.
 *
 *  This is the main RR-SELECT-BCCH state machine. This state machine, which coordinates the RR_SELECT-BCCH
 *  procedures. Input messages can be any that are a member of rr_cmd_bdy_type.
 *  Note: Legacy RR events are not used in this module.
 * 
 * \param msg_ptr - input messages
 * \param gas_id 
 */
extern void rr_select_bcch_control(
  rr_cmd_bdy_type *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Returns a text string representing the given RR_SELECT_BCCH_IMSG/OMSG message.
 * 
 * \param msg_ptr - input message
 * 
 * \return char* 
 */
extern char *rr_select_bcch_message_name(const rr_cmd_bdy_type *msg_ptr);

/*!
 * \brief Indicates if a SI acquisition is currently in progress.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if an SI acquisition is currently in progress, FALSE otherwise
 */
extern boolean rr_select_bcch_si_acq_in_progress(const gas_id_t gas_id);

#endif /* RR_SELECT_BCCH_H */

/* EOF */

