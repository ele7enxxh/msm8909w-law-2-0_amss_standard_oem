/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E
                        Task related functions

GENERAL DESCRIPTION
  This module is created for handling the RCINIT shutdown signal
  start LPM for network deregistration and acknowledge to RCINIT


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2011-2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmshutdown.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/13   KC      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "mmcp_variation.h"
#include "customer.h"

#include "rcinit.h"
#include "rcevt_rex.h"

#include "cm.h"       /* Interface to CM client service.                   */
#include "cm_v.h"
#include "cm_i.h"
#include "cmtaski.h"
#include "cmidbg.h"
#include "cmshutdown.h"


static cm_client_id_type cm_shutdown_client_id = (cm_client_id_type) -1;

void cm_shutdown_ack_to_rcinit(void)
{
  RCEVT_SIGEX_SIGREX term_sig;

  term_sig.signal = get_cm_tcb();
  term_sig.mask = TASK_STOP_SIG;

  CM_MSG_HIGH_0("acknowledged to RCINIT");

  rcinit_unregister_term_group(RCINIT_GROUP_7, RCEVT_SIGEX_TYPE_SIGREX, &term_sig);
  rcinit_handshake_term();

}

static void cm_shutdown_ph_event_cb
(
  cm_ph_event_e_type        ph_event,
  const cm_ph_info_s_type * ph_info_ptr
)
{
  if (ph_event == CM_PH_EVENT_OPRT_MODE
     && ph_info_ptr->oprt_mode == SYS_OPRT_MODE_LPM)
  {
    cm_shutdown_ack_to_rcinit();
  }
  else if (ph_event != CM_PH_EVENT_OPRT_MODE)
  {
    CM_ERR_1("unsubscribed event %d",ph_event);
  }

}

static void cm_shutdown_cmd_ph_cmd_cb_func(
    void                    *data_block_ptr,
    cm_ph_cmd_e_type        ph_cmd,
    cm_ph_cmd_err_e_type    ph_cmd_err
)
{
  if (ph_cmd_err != CM_PH_CMD_ERR_NOERR)
  {
    CM_ERR_2( "Cmd. client cmd=%d, err=%d",ph_cmd,ph_cmd_err);
    cm_shutdown_ack_to_rcinit();
  }
}

boolean cm_shutdown_start(void)
{
  cm_client_status_e_type cm_status;
  CM_MSG_HIGH_0("cm_shutdown_start");

  cm_status = cm_client_init(CM_CLIENT_TYPE_FLOATING4, &cm_shutdown_client_id);
  if (cm_status != CM_CLIENT_OK)
  {
    CM_ERR_2("cm_client_init failed, %d %d",cm_status,cm_shutdown_client_id);
    return FALSE;
  }

  cm_status = cm_client_ph_reg( cm_shutdown_client_id,
              cm_shutdown_ph_event_cb,  /* cm_ph_event_f_type */
              CM_CLIENT_EVENT_REG,
              CM_PH_EVENT_OPRT_MODE,
              CM_PH_EVENT_OPRT_MODE,
              NULL);

  if (cm_status != CM_CLIENT_OK)
  {
   CM_ERR_2("cm_client_ph_reg failed, %d %d",cm_status,cm_shutdown_client_id);
   return FALSE;
  }

  cm_status = cm_client_act(cm_shutdown_client_id);
  if (cm_status != CM_CLIENT_OK)
  {
   CM_ERR_2("cm_client_act failed, %d %d",cm_status,cm_shutdown_client_id);
   return FALSE;
  }

  if (cm_ph_cmd_oprt_mode(
    cm_shutdown_cmd_ph_cmd_cb_func,
    NULL,
    cm_shutdown_client_id,
    SYS_OPRT_MODE_LPM
  ) != TRUE)
  {
   CM_ERR_2("cm_ph_cmd_oprt_mode failed, %d %d",cm_status,cm_shutdown_client_id);
   return FALSE;
  }
  return TRUE;
}

