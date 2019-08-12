#ifndef CB_TASK_V_H
#define CB_TASK_V_H
/*===========================================================================

               CB TASK HEADER FILE (CB_TASK.H)

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  Cell Broadcast task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cb_task_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/04   vdr     Added delete_page_req definition
07/14/04   vdr     Added new primitives to cb_command_body_type
06/24/03   vdr     Initial version


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"



#include "comdef.h"
#include "cbwmsif.h"
#include "queue.h"
#include "cb_rr.h"
#include "cb_l2.h"
#include "cbl1if.h"

#include "rlcbmcif.h"
#include "rrcbmcif.h"
#include "bmcl1if.h"

#include "ms_timer.h"



#include "cb_reg.h"

#include "cbwmsif.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/





typedef union cb_command_body_tag
{
  IMH_T                   message_header ;

  rr_cell_change_ind_T    cell_change_ind ;

  cb_search_list_req_type search_list_req ;

  dl_cb_block_ind_T       cb_block_ind ;

  l1_cb_sched_expiry_ind_type cb_schedule_expired_ind ;

  l1_cb_scheduling_cnf_type cb_scheduling_cnf;

  l1_cb_missing_block_ind_type cb_missing_block ;

  l1_cb_skip_cnf_type     cb_skip_cnf ;

  cb_delete_page_req_type delete_page_req ;

  cb_delete_all_req_type  delete_all_req ;

  cb_dual_standby_status_ind_type dual_standby_status_ind;

  rrcbmc_cell_change_ind_type rrc_cell_change_ind ;

  dl_bmc_block_ind_T            bmc_block_ind;

  l1_bmc_sched_expiry_ind_type  l1_bmc_sched_expiry_ind ;

  l1_bmc_skip_cnf_type          l1_bmc_skip_cnf ;

  l1_bmc_scheduling_cnf_type    l1_bmc_scheduling_cnf ;

  l1_bmc_dropped_pdu_ind_type   l1_bmc_dropped_pdu_ind;

  rrc_etws_prim_notification_ind_type   rrc_etws_prim_notification_ind;
  timer_expiry_T                timer_expiry;



  reg_service_state_ind_T service_state_ind;
  cb_reg_stat_chgd_req_s_type   multisim_status_ind;

} cb_command_body_type ;




/*===========================================================================

FUNCTION CB_PUT_CMD

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cb_put_cmd(cb_command_body_type *cmd_ptr) ;


/*===========================================================================

FUNCTION CB_GET_CMD

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern cb_command_body_type *cb_get_cmd(byte message_set, byte message_id) ;




#endif /* #ifndef CB_TASK_V_H */
