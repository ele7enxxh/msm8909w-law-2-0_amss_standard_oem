#ifndef RR_PROCEDURE_H
#define RR_PROCEDURE_H

/*! \file rr_procedure.h 
 
  This module contains functionality related to co-ordination of dedicated procedures.
 
                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_procedure.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_l2.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
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
 * \brief Checks whether there is an RR procedure currently ongoing or not.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if a procedure is ongoing; FALSE otherwise
 */
extern boolean rr_ded_proc_in_progress(
  const gas_id_t gas_id
);

/*!
 * \brief Top-level controller for dedicated procedures.
 * 
 * \param proc_dispatch_event 
 * \param new_message 
 * \param gas_id 
 * 
 * \return rr_event_T 
 */
extern rr_event_T rr_handle_procedures(
  rr_event_T proc_dispatch_event,
  void *new_message,
  const gas_id_t gas_id
);

/*!
 * \brief Resets all SAPIs in response to a DL_ESTABLISH_IND for SAPI0.
 * 
 * \param msg_ptr - DL_ESTABLISH_IND message
 * \param gas_id 
 */
extern void rr_handle_l2_reestablishment(
  dl_establish_ind_T *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Resets L2 and RR SAPI controls in response to a lower layer failure.
 * 
 * \param gas_id 
 */
extern void rr_reset_sapis_due_to_failure(
  const gas_id_t gas_id
);

/*!
 * \brief Processes the SAPI0 DL_DATA_ACK_IND for a specific L3 message number.
 * 
 * \param l3_msg_number 
 * \param gas_id 
 */
extern void rr_handle_signalling_acknowledgement(
  uint8 l3_msg_number,
  const gas_id_t gas_id
);


#endif /* #ifndef RR_PROCEDURE_H */

/* EOF */

