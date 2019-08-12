#ifndef RFCOMMON_CMD_PROCESSING_H
#define RFCOMMON_CMD_PROCESSING_H
/*!
   @file
   rfcommon_cmd_processing.h

   @brief
   contains declarations for RFcommon APIs to register, deregister RFM UMIDs with MSGR as well as call the dispatcher for the MSGR cmds.

   @details

*/

/*===========================================================================
Copyright (c) 2002 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/inc/rfcommon_cmd_processing.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/02/13   sb     Initial version 
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34) 

============================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "msgr.h"
#include "msgr_types.h"
#include "rfa_msgr.h"
#include "rfcom.h"
#include "rf_cmd_interface.h"


/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define RFCOMMON_REQ_CNT(x) \
    (sizeof(x)/sizeof(msgr_umid_type))




boolean rfcommon_msgr_register( msgr_client_t *client_id, msgr_id_t id );

boolean rfcommon_msgr_deregister( msgr_client_t *client_id );

boolean rfcommon_rf_apps_task_register(void);

boolean rfcommon_rf_apps_task_deregister(void);

void rfcommon_dispatch_init(rf_task_num_type task);

void rfcommon_req_dispatcher( rf_cmd_type* req_ptr );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* RFCOMMON_CMD_PROCESSING_H */
