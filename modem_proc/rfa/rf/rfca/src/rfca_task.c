/*
@file
  rfca_task.c

@brief
 RFCA Task

*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_task.c#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/27/14   aki     Added call to sleep init
06/10/13   aki     Mainline RFCA 
12/27/12   aki     Use and initialize rex_crit_sect_type
12/03/12   aki     Use TASK_STOP_SIG for now, call rcinit_handshake_term()
10/05/12   aki     Cleanup
04/26/12   aki     Removed task_init
03/01/12   aki     Documentation update
01/25/12   aki     Diag V2 delayed response implementation
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"

#include "rcinit.h"
#include "task.h"

#include "diagcmd.h"
#include "rfca.h"
#include "rfca_task.h"
#include "msg.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "queue.h"

extern rex_crit_sect_type RfcaCommandQueueLock;

static const diagpkt_user_table_entry_type rfca_tbl_delay_rsp[] =
{
  {FTM_RFCA_C, FTM_RFCA_C, RFCA_diag},
};

void rfca_task (dword ignored)
{
  rex_sigs_type task_sigs;

  (void) ignored;

  RFCA_sleep_init();

  q_init( &RFCA_task_cmd_q); 
  q_init( &RFCA_log_msg_q);  

  rex_init_crit_sect(&RfcaCommandQueueLock);

  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F, (uint16)DIAG_SUBSYS_FTM, rfca_tbl_delay_rsp);

  rcinit_handshake_startup();

  MSG(MSG_SSID_FTM, MSG_LEGACY_HIGH, "RFCA task started");

  /* Loop forever handling commands */
  for (;;)
  {
    task_sigs = rex_wait( TASK_STOP_SIG       | /* Stop the RFCA task    */
                          RFCA_CMD_Q_SIG           | /* Process new incoming messages */
                          RFCA_FTM_SEQ_LOG_MSG_SIG   /* FTM Sequencer log messages */
                          );

    if ( 0 != (task_sigs & TASK_STOP_SIG) )
    {
      MSG (MSG_SSID_FTM, MSG_LEGACY_HIGH, "TASK_STOP_SIG received");
      (void) rex_clr_sigs (rex_self(), TASK_STOP_SIG);
      task_sigs &= ~TASK_STOP_SIG;
      rcinit_handshake_term();
      break;
    }

    if (0 != ( task_sigs & RFCA_CMD_Q_SIG) )
    {
      (void) rex_clr_sigs( rex_self(), RFCA_CMD_Q_SIG);
      task_sigs &= ~RFCA_CMD_Q_SIG;
      RFCA_req();
    }

    if ( 0 != (task_sigs & RFCA_FTM_SEQ_LOG_MSG_SIG) )
    {
      (void) rex_clr_sigs( rex_self(), RFCA_FTM_SEQ_LOG_MSG_SIG);
      task_sigs &= ~RFCA_FTM_SEQ_LOG_MSG_SIG;
      RFCA_log_req();
    }    

  }
} /* RFCA_task */

