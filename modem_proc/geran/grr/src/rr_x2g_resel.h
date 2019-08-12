#ifndef RR_X2G_RESEL_H
#define RR_X2G_RESEL_H

/*! \file rr_x2g_resel.h 
 
  This module implements a state machine that controls all types of reselection from other RATs
  to the GSM RAT.
 
                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_x2g_resel.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

#include "comdef.h"
#include "rr_events.h"
#include "rr_task.h"
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
 * \brief Initialise the module, allocating any memory as necessary.
 *  
 *  This function MUST be called before any of this module's other APIs are used.
 *  
 * \param gas_id 
 */
extern void rr_x2g_resel_init(
  const gas_id_t gas_id
);

/*!
 * \brief Deinitialises the module, freeing any associated memory.
 * 
 * \param gas_id 
 */
extern void rr_x2g_resel_deinit(
  const gas_id_t gas_id
);

/*!
 * \brief Determines if an X2G with NACC procedure is in progress.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if an X2G with NACC procedure is in progress, FALSE otherwise.
 */
extern boolean rr_x2g_resel_with_nacc_in_progress(
  const gas_id_t gas_id
);

/*!
 * \brief This is the main state machine for this module.
 * 
 * \param message - An input message for this module.
 * \param gas_id 
 */
extern void rr_x2g_resel_control(
  rr_cmd_bdy_type * message,
  const gas_id_t gas_id
);

#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#endif /* RR_X2G_RESEL_H */

/* EOF */

