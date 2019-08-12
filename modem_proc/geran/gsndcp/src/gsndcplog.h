#ifndef SNDCP_LOG_H
#define SNDCP_LOG_H

#include "geran_variation.h"
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           SNDCP Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging SNDCP packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsndcplog.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/13/02   ADG     Initial Revision.
===========================================================================*/

#include "comdef.h" /* for types */
#include "gsni.h"
#include "dsm.h"

/* INITIALISATION */
extern void sndcp_log_init(void);

/* LOG PACKETS */
extern void sndcp_log_active_nsapis(void);
extern void sndcp_log_nsapi_pdp_context(uint8 nsapi);
extern void sndcp_log_nsapi_state(uint8 nsapi);
extern void sndcp_log_pdu_stats(uint8 nsapi, uint8 zero_stats);
extern void sndcp_log_dl_tcp_hdr(dsm_item_type *npdu); 
extern void sndcp_log_ul_tcp_hdr(dsm_item_type *npdu);

/* EVENTS */
/* extern void sndcp_log_some_event(args); */

#endif /* SNDCP_LOG_H */
