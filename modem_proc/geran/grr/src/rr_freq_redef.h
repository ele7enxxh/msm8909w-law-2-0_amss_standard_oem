#ifndef RR_FREQ_REDEF_H
#define RR_FREQ_REDEF_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_freq_redef.h

GENERAL DESCRIPTION
   This module contains functions for dealing with frequency redefinitions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_freq_redef.h#1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

rr_event_T rr_handle_frequency_redefinition_msg(const gas_id_t gas_id);
rr_event_T rr_handle_frequency_redefinition_ind(void *, const gas_id_t gas_id);

#endif

