#ifndef RR_CIPHER_H
#define RR_CIPHER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_cipher.h

GENERAL DESCRIPTION
   This module contains functions for dealing with ciphers.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cipher.h#1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "sys_cnst.h"
#include "environ.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         CONSTANT DECLARATIONS

===========================================================================*/
#define RR_CIPHER_KEY_SEQ_NOT_AVAILABLE 0x07
#define OPTIONAL_MOBILE_IDENTITY_IEI    0x17

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern rr_event_T rr_cipher_control(rr_event_T, void *, const gas_id_t gas_id);
extern boolean    rr_cipher_mode_supported(cipher_algorithm_T, const gas_id_t gas_id);
extern void       rr_confirm_ciphering_completed(const gas_id_t gas_id);
extern void       rr_set_ciphering_mode_command_received(boolean, const gas_id_t gas_id);
extern boolean    rr_get_ciphering_mode_command_received(const gas_id_t gas_id);
extern byte       rr_get_cipher_key_sequence_number(const gas_id_t gas_id);
extern void       rr_ciphering_has_started(boolean ciphering_started, const gas_id_t gas_id);
extern boolean    rr_has_ciphering_started(const gas_id_t gas_id);

#endif /* RR_CIPHER_H */
