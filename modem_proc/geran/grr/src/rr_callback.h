/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Callback

GENERAL DESCRIPTION
  RR callback functionality.  Provides callback for multiple regstrations
  (set by RR_MAX_NUM_OF_REG_CB).  User registers a function with the 
  callback handler.  This function(s) is triggered by the callback handler when
  the data item being monitored has changed.  
  
  Currently provided:
    1) Serving cell callback - Provides the cell_id of the serving cell.
    2) Best six neighbour cells callback -  Provides cell_id's of the best
       six neighbour cells.

  Both neighbour cell and serving cell id changes are serviced by:
  
  Register a cell id callback call function:
    rr_register_cell_change_ind_cb()

  Deregister a cell id callback function call:
    rr_deregister_cell_change_ind_cb()

  RR notifies the callback handler of an update in serving cell_id:
    rr_notify_cell_id_change()

  RR notifies the callback handler of an update in neigbour cell_id(s):
    rr_handle_ncell_callback()

  Reset the current stored cell_ids call:
    rr_init_cell_change_cb_data()

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_defs.h_v   1.20   17 Jul 2002 11:50:34   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_callback.h#1 $   $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/10   SW      Updated for Dual SIM callback registration
16/06/08   RN      Created file

*============================================================================*/
#ifndef RR_CALLBACK_H
#define RR_CALLBACK_H

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_RR_CELL_CALLBACK

#include "comdef.h"
#include "rr_gprs_defs.h"
#include "rr_resel_calcs.h"
#include "rr_lai.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/
extern void rr_notify_cell_id_change(
  word,
  best_six_ncells_t,
  LAI_T,
  const gas_id_t gas_id
);

extern void rr_init_cell_change_cb_data(
  const gas_id_t gas_id
);

extern void rr_handle_ncell_callback(
  best_six_ncells_t,
  const gas_id_t gas_id
);

#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

#endif /* #ifndef RR_CALLBACK_H */

/* EOF */

