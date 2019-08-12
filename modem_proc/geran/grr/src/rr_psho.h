#ifndef RR_PSHO_H
#define RR_PSHO_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_psho.h

GENERAL DESCRIPTION
   This module contains functions for dealing with PS handover

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_psho.h#1 $  

18/07/2006 WXU Initial submit

===========================================================================*/
#include "geran_variation.h"
#include "comdef.h"
#include "customer.h"
#include "comdef.h"
#include "geran_multi_sim.h"

#ifdef FEATURE_GPRS_PS_HANDOVER

#define T3218_TIMEOUT     1000  //1//constant 1 second

typedef ps_handover_command_t psho_details_t;

extern rr_event_T rr_psho_control(rr_event_T event_in, rr_cmd_bdy_type *message_ptr, const gas_id_t gas_id);
extern boolean rr_psho_in_progress(const gas_id_t gas_id);
extern void rr_psho_save_details(const psho_details_t *psho_cmd, const gas_id_t gas_id);
extern psho_details_t * rr_psho_get_details(const gas_id_t gas_id);
extern void rr_psho_clean_details(const gas_id_t gas_id);


extern void rr_psho_task_start_init(void);

#endif /* FEATURE_GPRS_PS_HANDOVER */

#endif /* RR_PSHO_H */

/* EOF */

