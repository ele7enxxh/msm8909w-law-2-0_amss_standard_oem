#ifndef RR_SAPI3_H
#define RR_SAPI3_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_sapi3.h

GENERAL DESCRIPTION
   This module contains RR SAPI3 interface definitions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sapi3.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/08   sjw     Reworked release code for SAPI3, added new MDL_ERROR_IND handler
06/09/06   sjw     Add interface functions for message tracking
07/19/05    ws     Branch from GSM_RR.06.01.35 into new geran structure


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "geran_dual_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern rr_event_T rr_control_SAPI3_link(void *, const gas_id_t gas_id);
extern byte       rr_sapi3_get_next_message_number(const gas_id_t gas_id);
extern void       rr_sapi3_process_dl_data_ack_ind(byte, const gas_id_t gas_id);
extern void       rr_reset_sapi3(const gas_id_t gas_id);
extern void       rr_release_sapi3(const gas_id_t gas_id);

#endif
