#ifndef RR_CGPS_H
#define RR_CGPS_H

/*!
 * \file rr_cgps.h 
 *  
 * This module contains interface functionality between GRR and location software (CGPS)
 *  
 *              Copyright (c) 2014 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cgps.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"

#ifdef FEATURE_CGPS_UMTS_CP_GSM

#include "comdef.h"
#include "rr_lsm_if.h"
#include "rr_multi_sim.h"
#include "rr_task.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern void rr_cgps_process_rr_apdu_req(
  rr_apdu_id_T apdu_id,
  const rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
);

extern void rr_cgps_process_apdu_data_seg(
  const rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
);

extern void rr_cgps_apdu_in_progress_check(
  const rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
);

extern void rr_cgps_lcs_activity_check(
  const rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
);

extern void rr_cgps_process_cgps_event_req(
  const rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
);

extern void rr_cgps_init(const gas_id_t gas_id);

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
extern void rr_cgps_task_start_init(void);

#endif /* #ifdef FEATURE_CGPS_UMTS_CP_GSM */

#endif /* #ifndef RR_CGPS_H */

