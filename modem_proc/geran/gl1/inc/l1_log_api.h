#ifndef L1_LOG_API_H
#define L1_LOG_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L1 Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging L1 packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_log_api.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
24/09/13   sp        CR449651 Define new log packet for burst metrics
01/21/02   atm      Initial version.
===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "rfcom.h"
#include "sys_type.h"

#include "gl1_msg.h" // for gl1_msg_chan_type enum
#include "gl1_defs.h"

#include "geran_dual_sim.h"

/* LOG PACKETS */
/* NOTE -- use a boolean return value if it can be logged on demand */
void    l1_log_burst_metrics          ( gl1_msg_chan_type      channel,
          gl1_msg_metrics_rpt   *rpt_ptr[], gas_id_t gas_id );

void    l1_log_burst_metrics_ver2          ( gl1_msg_chan_type      channel,
          gl1_msg_metrics_rpt   *rpt_ptr[], gas_id_t gas_id );

#endif /* L1_LOG_API_H */
