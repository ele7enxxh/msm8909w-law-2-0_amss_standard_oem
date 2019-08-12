#ifndef RR_GPRS_MAIN_H
#define RR_GPRS_MAIN_H

/*============================================================================
  @file rr_gprs_main.h

  Header file for the rr_gprs_main.c module.

                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_main.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
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

extern rr_event_T rr_gprs_process_gprs_messages(
  rr_cmd_bdy_type *,
  const gas_id_t gas_id
);

extern rr_mac_status_t rr_macinfo_get_mac_status(const gas_id_t gas_id);

extern boolean grr_get_l1_in_test_mode(const gas_id_t gas_id);

extern void grr_set_test_mode_band(
  rr_internal_band_T,
  const gas_id_t gas_id
);

extern rr_internal_band_T grr_get_test_mode_band(const gas_id_t gas_id);

extern void rr_gprs_main_init(const gas_id_t gas_id);
extern void rr_gprs_main_task_start_init(void);

#endif /* RR_GPRS_MAIN_H */

/* EOF */

