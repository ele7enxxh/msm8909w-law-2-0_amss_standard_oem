#ifndef LLC_LOG_H
#define LLC_LOG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            LLC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging LLC packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllclog.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/05/02   ADG     Initial Revision.
===========================================================================*/

#include "geran_variation.h"

#include "comdef.h" /* for types */
#include "gs.h"

/* INITIALISATION */
extern void llc_log_init( gas_id_t gas_id );

/* LOG PACKETS */
extern void llc_log_me_info(void);
extern void llc_log_sapi_states(uint8 sapi);
extern void llc_log_xid_info(uint8 sapi);
extern void llc_log_pdu_stats(uint8 sapi, uint8 zero_stats);
extern void llc_log_periodic_stats(uint8 sapi, uint8 zero_stats);
extern void llc_log_ota_message( gas_id_t     gas_id,
                                 boolean      downlink,
                                 uint8        message_type,
                                 uint16       message_length,
                                 const uint8 *message_ptr
                               );
extern void llc_log_ready_timer_status(gas_id_t gas_id);
extern void llc_log_send_ui_frame(gas_id_t gas_id, uint16 seq_num, uint8 sapi, uint16 pdu_len, uint8 cipher_mode);
extern void llc_log_recv_ui_frame(gas_id_t gas_id, uint16 seq_num, uint8 sapi, uint16 pdu_len, uint8 cipher_mode);


/* EVENTS */
/* extern void llc_log_some_event(args); */



extern uint8 gllc_gs_queue[ NUM_GERAN_DATA_SPACES ];

#endif /* LLC_LOG_H */
