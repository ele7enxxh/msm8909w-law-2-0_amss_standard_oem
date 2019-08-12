#ifndef L2_LOG_H
#define L2_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L2 Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging L2 packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/vcs/l2_log.h_v   1.0   07 Mar 2002 14:23:20   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_log.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
29/10/10   SJW      Added support for Dual SIM
02/11/02   JAC      Initial version
===========================================================================*/
#include "customer.h" /* for features */
#include "comdef.h" /* for types */
#include "log.h"

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/

LOG_RECORD_DEFINE(LOG_GSM_L2_STATE_C)
  byte sapi;
  byte l2_state;
  byte l2_event;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_L2_TRANSMISSION_STATUS_C)
  byte sapi;
  byte channel_type;
  byte VS;
  byte VA;
  byte VR;
  byte retransmission_counter;
  byte sequence_error;
  byte frame_type;
  byte message_entries;
  byte segment_entries;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_L2_OUTSTANDING_FRAME_C)
  byte sapi0_DCCH_outstanding_I_frame;
  byte sapi3_DCCH_outstanding_I_frame;
  byte sapi3_SACCH_outstanding_I_frame;
LOG_RECORD_END

extern void l2_log_state(l2_store_T *, byte, byte, l2_event_T);

extern void l2_log_transmission_status(l2_store_T *, byte, byte);

extern void l2_log_timeout(l2_store_T *, byte, byte, byte);

#endif /* L2_LOG_H */
