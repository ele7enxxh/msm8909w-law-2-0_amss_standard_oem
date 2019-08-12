#ifndef RR_HZ_H
#define RR_HZ_H

/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2005-2013 Qualcomm Technologies, Inc.
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rr_hz.h#1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/06   sjw     Ported to MSM6280 for Sirius FEATURE_HOME_ZONE_SERVICE development

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"


#include "sys_plmn.h"
#include "geran_multi_sim.h"

typedef enum
{
  RR_HZ_STATE_IDLE,
  RR_HZ_STATE_CS_CONNECTION,
  RR_HZ_STATE_PS_CONNECTION,
  RR_HZ_STATE_NO_SERVICE
} rr_hz_state_ind_T;

/* State change callback function pointer type (5 parameters) */
typedef void (*rr_hz_state_cb_fn_T)(rr_hz_state_ind_T prev_state, rr_hz_state_ind_T curr_state, PLMN_id_T plmn, word lac, word cell_id);

#ifndef PERLUTF
extern boolean rr_hz_register_state_change_cb(rr_hz_state_cb_fn_T);
#else
#error code not present
#endif
extern void rr_hz_deregister_state_change_cb(void);
extern void rr_init_hz_info(const gas_id_t gas_id);

#endif /* RR_HZ_H */
