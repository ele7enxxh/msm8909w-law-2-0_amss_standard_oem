#ifndef _EXTERN_MMTASK_H_
#define _EXTERN_MMTASK_H_
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/extern_mmtask.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#include "mm_v.h"
#ifdef FEATURE_LTE
#include "emm_rrc_emm_if.h"
#include "emm_esm_if_msg.h"
#include "emm.h"
#include "emm_timer_sigs.h"
#endif
#include "rex.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
extern rex_sigs_type mm_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
);

extern void mm_emm_process_messages(void);

#ifdef FEATURE_LTE
extern void emm_process_esm_messages(void);
extern void emm_process_rrc_messages(void);

extern msgr_client_t    esm_client;
extern msgr_client_t    rrc_client;
extern msgr_client_t    tlb_client;
#endif

extern q_type mm_cmd_q;

#ifdef FEATURE_LTE

#define ESM_EMM_CMD_Q_SIZE 25
#define RRC_EMM_CMD_Q_SIZE 25

extern q_type emm_esm_cmd_q_free;
extern q_type esm_esm_cmd_q_msg;
extern emm_cmd_type esm_bufs[ESM_EMM_CMD_Q_SIZE];
extern q_type rrc_emm_cmd_q_free;
extern q_type rrc_emm_cmd_q_msg;
extern emm_rrc_cmd_type rrc_bufs[RRC_EMM_CMD_Q_SIZE];
extern msgr_client_t esm_client;
extern msgr_client_t rrc_client;
#endif /*FEATURE_LTE*/

extern boolean get_nas_msg
(
  rex_sigs_type wait_sigs,
  msgr_client_t esm_client,
  msgr_client_t rrc_client,
  msgr_client_t tlb_client
);

extern boolean mm_read_queue(q_type *queue,void *mm_process_fp,mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr, rex_sigs_type sigs);
#endif /* _EXTERN_MMTASK_H_ */

