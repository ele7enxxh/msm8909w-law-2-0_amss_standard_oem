#ifndef RR_TEST_H
#define RR_TEST_H

/*! \file rr_test.h 
 
  This is the header file for rr_test.c.
  This module contains SIM & loop-back test support.
 
                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_test.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/*!
 * \brief Handler for test-related messages.
 * 
 * \param new_message (in)
 * \param gas_id (in)
 */
extern void rr_handle_test_messages(
  void *new_message,
  const gas_id_t gas_id
);

/*!
 * \brief Opens the TCH loop.
 * 
 * \param gas_id (in)
 */
extern void rr_tm_open_tch_loop(
  const gas_id_t gas_id
);

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
extern void rr_test_task_start_init(
  void
);

#endif /* #ifndef RR_TEST_H */

/* EOF */

