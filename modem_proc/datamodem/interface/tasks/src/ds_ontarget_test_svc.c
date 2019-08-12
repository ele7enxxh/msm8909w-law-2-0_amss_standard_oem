/*===========================================================================
  FILE: ds_ontarget_test_svc.c

  OVERVIEW: This file implements the services provided by the DS_ONTARGET_TEST task.

  DEPENDENCIES: None

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ds_ontarget_test_svc.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2012-09-02 aa   Created module

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ds_ontarget_test_svc.h"
#include "ds_ontarget_test_task.h"
#include "ds_ontarget_test_taski.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"

/*===========================================================================

  TEST CALLBACK DEFINITIONS1

===========================================================================*/
#define DS_ONTARGET_TEST_CALLBACK_SIG (0x00080000)
extern q_type ds_ontarget_test_callback_q; // Holds callbacks occurred
static rex_timer_type* ds_ontarget_test_callback_timer = NULL;

#define DS_ONTARGET_TEST_CALLBACK_TIMEOUT (60 * 1000) // 1 minute

typedef struct
{
  q_link_type           link;              /* For linking onto cmd queue   */
  int                   callback_id;       /* Holds callback id. -1 for timeout */
} ds_ontarget_test_callback_q_node_type;


/*===========================================================================

                          PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

ds_ontarget_test_cmd_handler_type ds_ontarget_test_set_cmd_handler
(
  ds_ontarget_test_cmd_enum_type      cmd,         /* Command name                   */
  ds_ontarget_test_cmd_handler_type   cmd_handler  /* Cmd handler function pointer   */
)
{
  ds_ontarget_test_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  if (DS_ONTARGET_TEST_CMD_MIN > cmd || DS_ONTARGET_TEST_CMD_MAX <= cmd)
  {
    LOG_MSG_ERROR_1 ("ds_ontarget_test_set_cmd_handler(): "
                     "Inv cmd %d", cmd);
    ASSERT (0);
    PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);
    return NULL;
  }

  old_handler = ds_ontarget_testi_cmd_handler[cmd];

  ds_ontarget_testi_cmd_handler[cmd] = (cmd_handler != NULL) ?
                                        cmd_handler :
                                        ds_ontarget_testi_default_cmd_handler;

  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  return old_handler;
} /* ds_ontarget_test_set_cmd_handler() */



int ds_ontarget_test_send_cmd
(
  ds_ontarget_test_cmd_enum_type   cmd,             /* Command to be processed       */
  void                            *user_data_ptr    /* Command specific user data    */
)
{
  ds_ontarget_testi_cmd_type      *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DS_ONTARGET_TEST_CMD_MIN > cmd || DS_ONTARGET_TEST_CMD_MAX <= cmd)
  {
    LOG_MSG_ERROR_1 ("ds_ontarget_test_send_cmd(): "
                     "Inv cmd %d", cmd);
    ASSERT (0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal DS_ONTARGET_TEST task
  -------------------------------------------------------------------------*/
  cmd_ptr = (ds_ontarget_testi_cmd_type *) q_get (&ds_ontarget_testi_cmd_free_q);

  if (NULL == cmd_ptr)
  {
    LOG_MSG_ERROR_0("ds_ontarget_test_send_cmd(): "
                    "DS_ONTARGET_TEST task cmd buffers unavailable");
    return -1;
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;

    /*-----------------------------------------------------------------------
      Making the operation atomic so that race conditions do not occur
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);
    q_put (&ds_ontarget_testi_cmd_q, &cmd_ptr->link);
    DS_ONTARGET_TESTI_SET_SIGNAL (DS_ONTARGET_TEST_CMD_Q_SIGNAL);
    PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);
  }
  return 0;
} /* ds_ontarget_test_send_cmd() */



ds_ontarget_test_signal_handler_type  ds_ontarget_test_set_signal_handler
(
  ds_ontarget_test_signal_enum_type     sig,          /* Signal assoc with handler   */
  ds_ontarget_test_signal_handler_type  sig_handler,  /* Signal handler callback     */
  void                       *user_data_ptr /* User data for the handler   */
)
{
  ds_ontarget_test_signal_handler_type     old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DS_ONTARGET_TEST_SIGNAL_MIN > sig || DS_ONTARGET_TEST_SIGNAL_MAX <= sig)
  {
    LOG_MSG_ERROR_1 ("ds_ontarget_test_set_signal_handler(): "
                     "Inv signal %d", sig);
    ASSERT (0);
    return NULL;
  }

  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  old_handler = ds_ontarget_testi_sig_handler[sig].sig_handler;
  ds_ontarget_testi_sig_handler[sig].sig_handler = (sig_handler != NULL) ?
                                          sig_handler :
                                          ds_ontarget_testi_default_sig_handler;

  ds_ontarget_testi_sig_handler[sig].user_data_ptr = user_data_ptr;

  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  return old_handler;
} /* ds_ontarget_test_set_sig_handler() */

void  ds_ontarget_test_enable_signal
(
  ds_ontarget_test_signal_enum_type sig        /* Signal to be enabled */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DS_ONTARGET_TEST_SIGNAL_MIN > sig || DS_ONTARGET_TEST_SIGNAL_MAX <= sig)
  {
    LOG_MSG_ERROR_1 ("ds_ontarget_test_enable_signal(): "
                     "Inv signal %d", sig);
    ASSERT (0);
    return;
  }

  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  if (ds_ontarget_testi_sig_handler[sig].sig_handler == 
      ds_ontarget_testi_default_sig_handler)
  {
    LOG_MSG_ERROR_1 ("ds_ontarget_test_enable_signal(): "
                     "Signal %d enabled without a handler", sig);
  }

  ds_ontarget_testi_enabled_sig_mask |= ((rex_sigs_type) 1 << (rex_sigs_type) sig);

  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  DS_ONTARGET_TESTI_SET_SIGNAL (DS_ONTARGET_TEST_SIG_MASK_CHANGE_SIGNAL);
} /* ds_ontarget_test_enable_signal() */

void ds_ontarget_test_disable_signal
(
  ds_ontarget_test_signal_enum_type sig                 /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  ds_ontarget_testi_enabled_sig_mask &= ~((rex_sigs_type) 1 << (rex_sigs_type) sig);

  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);

} /* ds_ontarget_test_disable_signal() */

boolean ds_ontarget_test_is_current_task
(
  void 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  return (boolean) (rex_self() == &ds_ontarget_test_tcb);

} /* ds_ontarget_test_is_current_task() */

static void ds_ontarget_test_callback_timeout
(
  unsigned long dummy
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Add the CALLBACK_TIMEOUT to the callback queue
  -----------------------------------------------------------------------*/
  ds_ontarget_test_post_callback(CALLBACK_TIMEOUT);
}

int ds_ontarget_test_wait_for_callback
(
  void
)
{
  rex_sigs_type                           sig_result;
  ds_ontarget_test_callback_q_node_type*  callback_node = NULL;
  int                                     callback_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);
  
  sig_result = rex_get_sigs(&ds_ontarget_test_tcb);

  ASSERT(NULL == ds_ontarget_test_callback_timer);

  ds_ontarget_test_callback_timer = rex_create_timer_ex(ds_ontarget_test_callback_timeout, 0L);
  rex_set_timer(ds_ontarget_test_callback_timer, DS_ONTARGET_TEST_CALLBACK_TIMEOUT);

  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);
  
  /*-----------------------------------------------------------------------
    Enter into the Wait only if no callback signal occurred
    -----------------------------------------------------------------------*/
  if (0 == (sig_result & DS_ONTARGET_TEST_CALLBACK_SIG))
  {
    sig_result = rex_wait(DS_ONTARGET_TEST_CALLBACK_SIG);
    if (0 == (sig_result & DS_ONTARGET_TEST_CALLBACK_SIG))
    {
      LOG_MSG_ERROR_0("ds_ontarget_test_wait_for_callback(): "
                      "rex_wait() failed");
      ASSERT(0);
      return -1;
    }
  }

  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);
  
  if (NULL != ds_ontarget_test_callback_timer) {
    rex_clr_timer(ds_ontarget_test_callback_timer);
    rex_delete_timer_ex(ds_ontarget_test_callback_timer);
    ds_ontarget_test_callback_timer = NULL;
  }
  
  (void) rex_clr_sigs(&ds_ontarget_test_tcb, DS_ONTARGET_TEST_CALLBACK_SIG);
  if (0 == q_cnt(&ds_ontarget_test_callback_q)) {
    LOG_MSG_ERROR_0("ds_ontarget_test_wait_for_callback(): "
                    "callback queue empty");
    PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);
    ASSERT(0);
    return -1;
  }
  
  callback_node = (ds_ontarget_test_callback_q_node_type *) q_get (&ds_ontarget_test_callback_q);
  if (0 == callback_node)
  {
    LOG_MSG_ERROR_0("ds_ontarget_test_wait_for_callback(): "
                    "callback_node in NULL");
    PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);
    return -1;
  }

  callback_id = callback_node->callback_id;
  PS_SYSTEM_HEAP_MEM_FREE(callback_node);

  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  return callback_id;
}

void ds_ontarget_test_post_callback
(
  int callback_id
)
{
  ds_ontarget_test_callback_q_node_type*  callback_node = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Add the callback id to the callback queue
  -----------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);
  
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(callback_node, sizeof(ds_ontarget_test_callback_q_node_type),
                                   ds_ontarget_test_callback_q_node_type*);
  if (NULL == callback_node) {
    LOG_MSG_ERROR_0("ds_ontarget_test_post_callback(): "
                    "No Memory for allocating callback node");
    PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);
    return;
  }

  callback_node->callback_id = callback_id;
  q_put(&ds_ontarget_test_callback_q, &callback_node->link);
  rex_set_sigs(&ds_ontarget_test_tcb, DS_ONTARGET_TEST_CALLBACK_SIG);

  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);
}

void ds_ontarget_test_clear_callback_q
(
  void
)
{
  ds_ontarget_test_callback_q_node_type*  callback_node = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);
  
  if (NULL != ds_ontarget_test_callback_timer) {
    rex_clr_timer(ds_ontarget_test_callback_timer);
    rex_delete_timer_ex(ds_ontarget_test_callback_timer);
    ds_ontarget_test_callback_timer = NULL;
  }
  
  (void) rex_clr_sigs(&ds_ontarget_test_tcb, DS_ONTARGET_TEST_CALLBACK_SIG);
  
  callback_node = (ds_ontarget_test_callback_q_node_type *) q_get (&ds_ontarget_test_callback_q);
  while (NULL != callback_node) {
    PS_SYSTEM_HEAP_MEM_FREE(callback_node);
    callback_node = (ds_ontarget_test_callback_q_node_type *) q_get (&ds_ontarget_test_callback_q);
  }

  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);
}

void ds_ontarget_test_stop_test_excecution
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_ontarget_test_post_callback(CALLBACK_STOP_TEST);
}
