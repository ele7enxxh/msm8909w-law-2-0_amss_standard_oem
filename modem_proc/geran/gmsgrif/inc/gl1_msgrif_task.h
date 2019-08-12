#ifndef GL1_MSGRIF_TASK_H
#define GL1_MSGRIF_TASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GL1 MSGR Interface Header File

GENERAL DESCRIPTION
   This header file contains declarations relevant to L1 task processing.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmsgrif/inc/gl1_msgrif_task.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
07/04/14   cja     CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
14/10/13   pa      CR536820: Added GL1 Msgr If Task. Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#include "comdef.h"
#include "gl1_msgrif_init.h"
#include "queue.h"
#include "geran_multi_sim.h"
#include "geran_tasks.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
extern const geran_task_id_t gl1_msgrif_task_id[NUM_GERAN_DATA_SPACES];

extern q_type  gl1_msgrif_rf_task_msgr_q[NUM_GERAN_DATA_SPACES];
extern q_type  gl1_msgrif_rf_task_msgr_free_q[NUM_GERAN_DATA_SPACES];
extern gl1_msgrif_rf_task_msgr_t  gl1_msgrif_rf_task_msgr[NUM_GERAN_DATA_SPACES][GL1_MSGRIF_RF_TASK_MSGR_QUEUE_SIZE];

extern q_type gl1_msgrif_gfw_async_message_router_q[NUM_GERAN_DATA_SPACES];
extern q_type gl1_msgrif_gfw_async_message_router_free_q[NUM_GERAN_DATA_SPACES];
extern gl1_msgrif_gfw_async_msgr_t  gl1_msgrif_gfw_async_msgr[NUM_GERAN_DATA_SPACES][GL1_MSGRIF_GFW_ASYNC_MSGR_QUEUE_SIZE];

/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/

void gsm_gl1_msgrif_task (dword dummy);
void gl1_msgrif_main (dword unused);
rex_tcb_type* gl1_msgrif_task_tcb_read(gas_id_t gas_id);

#endif /* GL1_MSGRIF_TASK_H */
