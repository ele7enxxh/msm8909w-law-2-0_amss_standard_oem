#ifndef RR_INIT_H
#define RR_INIT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_init.h

GENERAL DESCRIPTION
   This module contains functions for the rr init function.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_init.h#1 $

===========================================================================*/

#include "rr_defs.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
extern void rr_init_geran_started(const gas_id_t gas_id);
extern void rr_init_geran_stopped(const gas_id_t gas_id);
extern void rr_init_modem_online(const gas_id_t gas_id);
extern void rr_init_modem_offline(const gas_id_t gas_id);
extern void rr_init_task_start_init(void);
extern void rr_init_recovery_init(const gas_id_t gas_id);
extern void rr_gprs_initialise(const gas_id_t gas_id);

#endif
