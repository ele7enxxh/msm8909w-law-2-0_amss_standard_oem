#ifndef RR_GPRS_MEAS_RPT_CONTROL_H
#define RR_GPRS_MEAS_RPT_CONTROL_H

/** @file */

/*============================================================================
  @file rr_gprs_meas_rpt_control.h

  This is the header file for the rr_gprs_meas_rpt_control.c module.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_meas_rpt_control.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_events.h"
#include "rr_gprs_defs.h"
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

#ifdef FEATURE_WCDMA

extern void rr_gprs_clear_3g_measurement(const gas_id_t gas_id);

#endif /* FEATURE_WCDMA */

extern boolean rr_meas_reporting_is_active(const gas_id_t gas_id);

extern void rr_gprs_meas_rpt_control_task_start_init(void);

extern void rr_gprs_meas_rpt_control_handle_l2g_cco_network_control_order(
  uint8 network_control_order,
  const gas_id_t gas_id
);


extern void rr_utran_priority_params_merge_mods(
  csn_utran_priority_params_ie_t * dst,
  csn_utran_priority_params_ie_t * mods
);

extern void rr_eutran_params_merge_csn_mods(
  rr_eutran_params_t     * dst,
  csn_eutran_params_ie_t * mods
);

/**
  @brief Merges one E-UTRAN Parameters Description in to another one.

  GPRS E-UTRAN Measurement Parameters from src will overwrite any present
  in dst.

  E-UTRAN Neighbour Cells and E-UTRAN Not Allowed Cells from src will be
  appended to those in dst.

  @param dst The destination to which the parameters will be merged.
  @param src The source parameters to merge in to the destination.
*/
extern void csn_eutran_params_merge_csn_mods(
  csn_eutran_params_ie_t * dst,
  csn_eutran_params_ie_t * mods
);


#endif /* RR_GPRS_MEAS_RPT_CONTROL_H */

/* EOF */


