#ifndef RR_IDLE_DRX_H
#define RR_IDLE_DRX_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             rr_idle_drx.h

GENERAL DESCRIPTION
   This module contains functions for dealing with Idle DRX change.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_conn_establish.h_v   1.1   08 Jul 2002 19:26:28   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_idle_drx.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
26/04/15    TS      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
#include "rr_events.h"
#include "geran_multi_sim.h"

#ifdef FEATURE_IDLE_DRX_SCALING
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern void rr_idle_drx_init(const gas_id_t gas_id);
extern boolean rr_idle_drx_start_scaling_suspend_timer(const gas_id_t gas_id);
extern byte rr_idle_drx_get_extended_dsc(const gas_id_t gas_id);
extern boolean rr_idle_drx_scaling_in_use(const gas_id_t gas_id);
extern boolean rr_idle_drx_is_nmeas_off(const gas_id_t gas_id);
extern void rr_idle_drx_check_dsc(const int dsc_max, const int dsc, const gas_id_t gas_id);
extern void rr_idle_drx_bs_pa_mfrms_changed(const int old_dsc_max, const gas_id_t gas_id);
extern void rr_idle_drx_control(rr_event_T drx_event, rr_cmd_bdy_type* msg_ptr, const gas_id_t gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */

#endif /* RR_IDLE_DRX_H */

/* EOF */

