/*===========================================================================
  FILE: ds_ontarget_test_taski.c

  OVERVIEW: 
    DataCommon Control Task internal API file. Contains APIs and definitions
    used internal to DS_ONTARGET_TEST Task processing. 

  EXTERNALIZED FUNCTIONS
    ds_ontarget_testi_default_sig_handler()
      Implicit handler for signals processed by DS_ONTARGET_TEST Task.

    ds_ontarget_testi_default_cmd_handler()
      Implicit handler for commands processed by DS_ONTARGET_TEST Task.

    ds_ontarget_testi_powerup()
      Cold start called before DS_ONTARGET_TEST task is started.

    ds_ontarget_testi_init()
      After task start initialization of the DS_ONTARGET_TEST task.

    ds_ontarget_testi_process_cmd()
      Processes command signals in DS_ONTARGET_TEST Task.

  Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ds_ontarget_test_taski.c#1 $
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

#include "ds_ontarget_test_task.h"
#include "ds_ontarget_test_svc.h"
#include "ds_ontarget_test_taski.h"
#include "task.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "DSS_OnTargetSanityTests.h"
#include "ds_qmi_otdt.h"
#include "stringl/stringl.h"

/*===========================================================================

                            PUBLIC DATA DEFINITIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  DS_ONTARGET_TEST's own critical section
---------------------------------------------------------------------------*/
ps_crit_sect_type ds_ontarget_test_crit_sect;

/*--------------------------------------------------------------------------
  Command Buffers. Configuration of DS_ONTARGET_TESTI_CMD_BUF_CNT is heuristic.
---------------------------------------------------------------------------*/
#define DS_ONTARGET_TESTI_CMD_BUF_CNT   (100)
ds_ontarget_testi_cmd_type ds_ontarget_testi_cmd_buf[DS_ONTARGET_TESTI_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Command Queues.
---------------------------------------------------------------------------*/
q_type ds_ontarget_testi_cmd_q;
q_type ds_ontarget_testi_cmd_free_q;
q_type ds_ontarget_test_callback_q;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
ds_ontarget_testi_sig_handler_info_type ds_ontarget_testi_sig_handler[DS_ONTARGET_TEST_SIGNAL_MAX];
ds_ontarget_test_cmd_handler_type       ds_ontarget_testi_cmd_handler[DS_ONTARGET_TEST_CMD_MAX];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type ds_ontarget_testi_enabled_sig_mask;

/*===========================================================================

                          INTERNAL HELPER FUNCTIONS

===========================================================================*/
#define TASK_PRIORITY_DETLA  (5)
rex_priority_type ds_ontarget_testi_get_rex_pri
(
  otdt_task_priority_enum_v01 priority
)
{
  switch(priority)
  {
    case OTDT_PRIORITY_ABOVE_DS_TASK_V01:
      return rcinit_lookup_prio("ds") + TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_BELOW_DS_TASK_V01:
      return rcinit_lookup_prio("ds") - TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_ABOVE_QMI_MODEM_TASK_V01:
      return rcinit_lookup_prio("qmi_modem") + TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_BELOW_QMI_MODEM_TASK_V01:
      return rcinit_lookup_prio("qmi_modem") - TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_ABOVE_DCC_TASK_V01:
      return rcinit_lookup_prio("dcc") + TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_BELOW_DCC_TASK_V01:
      return rcinit_lookup_prio("dcc") - TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_ABOVE_DS_SIG_TASK_V01:
      return rcinit_lookup_prio("ds_sig") + TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_BELOW_DS_SIG_TASK_V01:
      return rcinit_lookup_prio("ds_sig") - TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_ABOVE_PS_TASK_V01:
      return rcinit_lookup_prio("ps") + TASK_PRIORITY_DETLA;

    case OTDT_PRIORITY_BELOW_PS_TASK_V01:
      return rcinit_lookup_prio("ps") - TASK_PRIORITY_DETLA;

    default:
      LOG_MSG_ERROR_1("ds_ontarget_testi_get_rex_pri(): "
                      "Unknown priority %d", priority);
      break;
  }
  return (rex_priority_type)0;
  
}


/*===========================================================================

                         INTERNAL COMMAND HANDLERS

===========================================================================*/
void ds_ontarget_test_start_test
(
  ds_ontarget_test_cmd_enum_type    cmd,
  void *                            user_data_ptr
)
{
  int                                start_index;
  int                                end_index;
  int                                index;
  int                                res;
  int                                dss_errno;
  rex_priority_type                  old_prio;
  rex_priority_type                  new_prio;
  ds_ontarget_test_cmd_data_type *   test_data_ptr = 
    (ds_ontarget_test_cmd_data_type *)user_data_ptr;
  qmi_core_server_error_type         qmi_result;
  otdt_test_status_ind_msg_v01       qmi_response;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == test_data_ptr) {
    DATA_ERR_FATAL("ds_ontarget_test_start_test(): NULL user_data");
    return;
  }

  old_prio = rex_get_pri();

  /*-------------------------------------------------------------------------
    test_number 0 means that we need to run all the tests in the group
  -------------------------------------------------------------------------*/
  if (0 == test_data_ptr->test_number) {
    res = DSS_OntargetTestGetNumberOfTestsInGroup(test_data_ptr->test_group_name,
                                                  &end_index);
    if (DSS_SUCCESS != res) {
      LOG_MSG_ERROR_0("ds_ontarget_test_start_test(): "
                      "No such group as given");
      
      memset(&qmi_response, 0, sizeof(otdt_test_status_ind_msg_v01));
      (void) strlcpy(qmi_response.test_group, test_data_ptr->test_group_name, TEST_GROUP_MAX_LENGTH_V01);
      qmi_response.test_number = test_data_ptr->test_number;
      qmi_response.test_result = OTDT_TEST_FAIL_V01;
      qmi_response.dss_errno_valid = TRUE;
      qmi_response.dss_errno = DS_EFAULT;

      qmi_result = ds_qmi_otdt_send_ind(test_data_ptr->qmi_handle,
                                                            &qmi_response);
 
      LOG_MSG_INFO1_1("ds_ontarget_test_start_test(): "
                      "ds_qmi_otdt_send_ind() returned %d", (int) qmi_result);
      return;
    }
    start_index = 1;
  }
  else {
    start_index = end_index = test_data_ptr->test_number;
  }

  for (index = start_index; index <= end_index; index++) {
    /*-----------------------------------------------------------------------
      Clear callback queue from previous tests
    -----------------------------------------------------------------------*/
    ds_ontarget_test_clear_callback_q();

    /*-----------------------------------------------------------------------
      Set the test priority, if valid
    -----------------------------------------------------------------------*/
    if (OTDT_PRIORITY_DEFAULT_V01 != test_data_ptr->priority) {
      new_prio = ds_ontarget_testi_get_rex_pri(test_data_ptr->priority);
      if ((rex_priority_type)0 == new_prio) {
        LOG_MSG_ERROR_0("ds_ontarget_testi_get_rex_pri() returned error."
                        "Running in default priority");
        new_prio = old_prio;
      }
      (void) rex_set_pri(new_prio);
    } 

    LOG_MSG_INFO1_2("ds_ontarget_test_start_test(): "
                    "Starting Test: Test %d, Task priority %d",
                    index, (int) rex_get_pri());
    
    /*-----------------------------------------------------------------------
      Run the test
    -----------------------------------------------------------------------*/
    res = DSS_OntargetTestStart(test_data_ptr->test_group_name,
                                index,
                                &dss_errno);
  
    LOG_MSG_INFO1_2("ds_ontarget_test_start_test(): "
                    "Test Ended. Result %d dss_errno %d", res, dss_errno);

    /*-----------------------------------------------------------------------
      Send indication to QMI 
    -----------------------------------------------------------------------*/
    memset(&qmi_response, 0, sizeof(otdt_test_status_ind_msg_v01));
    (void) strlcpy(qmi_response.test_group, test_data_ptr->test_group_name, TEST_GROUP_MAX_LENGTH_V01);
    qmi_response.test_number = index;
    if (DSS_SUCCESS == res) {
      qmi_response.test_result = OTDT_TEST_SUCCESS_V01;
    }
    else {
      if (CALLBACK_STOP_TEST == dss_errno) {
        qmi_response.test_result = OTDT_TEST_STOPPED_V01;
      }
      else {
        qmi_response.test_result = OTDT_TEST_FAIL_V01;
        qmi_response.dss_errno_valid = TRUE;
        qmi_response.dss_errno = dss_errno;
      }
    }

    qmi_result = ds_qmi_otdt_send_ind(test_data_ptr->qmi_handle,
                                                          &qmi_response);
  
    LOG_MSG_INFO1_1("ds_ontarget_test_start_test(): "
                    "ds_qmi_otdt_send_ind() returned %d", (int) qmi_result);

    // if test was stopped, stop the rest of the tests as well
    if (CALLBACK_STOP_TEST == dss_errno) {
      rex_set_pri(old_prio);
      break;
    }
  }

  rex_set_pri(old_prio);
}


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
boolean ds_ontarget_testi_default_sig_handler
(
  ds_ontarget_test_signal_enum_type sig,   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("ds_ontarget_testi_default_sig_handler(): "
                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* ds_ontarget_testi_default_sig_handler() */

void ds_ontarget_testi_default_cmd_handler
(
  ds_ontarget_test_cmd_enum_type cmd, /* Actual command to be processed    */
  void *user_data_ptr                 /* Command specific user parameters  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("ds_ontarget_testi_default_cmd_handler(): "
                  "Command %d, no handler registered", cmd);

} /* ds_ontarget_testi_default_cmd_handler() */

void ds_ontarget_testi_powerup
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0 ("ds_ontarget_testi_powerup(): "
                   "DS_ONTARGET_TEST Task Powerup");

  /*------------------------------------------------------------------------
    Init the DS_ONTARGET_TEST task critical section.
  -------------------------------------------------------------------------*/
  PS_INIT_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  /*-------------------------------------------------------------------------
    Initialize the callback queue
  -------------------------------------------------------------------------*/
  (void) q_init (&ds_ontarget_test_callback_q);

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init (&ds_ontarget_testi_cmd_q);
  (void) q_init (&ds_ontarget_testi_cmd_free_q);

  for (i = 0; i < DS_ONTARGET_TESTI_CMD_BUF_CNT; i++)
  {
    q_put(&ds_ontarget_testi_cmd_free_q, q_link(&ds_ontarget_testi_cmd_buf[i], &ds_ontarget_testi_cmd_buf[i].link));
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  for (i = 0; i < DS_ONTARGET_TEST_SIGNAL_MAX; i++)
  {
    ds_ontarget_testi_sig_handler[i].sig_handler = ds_ontarget_testi_default_sig_handler;
    ds_ontarget_testi_sig_handler[i].user_data_ptr = NULL;
  }

  for (i = 0; i < DS_ONTARGET_TEST_CMD_MAX; i++)
  {
    ds_ontarget_testi_cmd_handler[i] = ds_ontarget_testi_default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to DS_ONTARGET_TEST Task
  -------------------------------------------------------------------------*/
  ds_ontarget_testi_enabled_sig_mask = 
    ((rex_sigs_type) 1 << (rex_sigs_type) DS_ONTARGET_TEST_CMD_Q_SIGNAL)          |
    ((rex_sigs_type) 1 << (rex_sigs_type) DS_ONTARGET_TEST_SIG_MASK_CHANGE_SIGNAL);

} /* ds_ontarget_testi_powerup() */

void ds_ontarget_testi_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0 ("ds_ontarget_testi_init(): "
                   "DS_ONTARGET_TEST After task start init");

  /* Register DS ONTARGET TEST command handlers */
  ds_ontarget_test_set_cmd_handler(DS_ONTARGET_TEST_START_CMD, ds_ontarget_test_start_test);

} /* ds_ontarget_testi_init() */

boolean  ds_ontarget_testi_process_cmd
(
  void
)
{
  static byte         ds_ontarget_test_cmds_proc_hi_score = 0;
  ds_ontarget_testi_cmd_type   *cmd_ptr;
  byte                processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    LOG_MSG_ERROR if no command is available.
  -------------------------------------------------------------------------*/
  if (q_cnt(&ds_ontarget_testi_cmd_q) == 0)
  {
    DATA_ERR_FATAL("ds_ontarget_testi_process_cmd(): No DS_ONTARGET_TEST command to process");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while ((cmd_ptr = (ds_ontarget_testi_cmd_type *) q_get (&ds_ontarget_testi_cmd_q)) != NULL)
  {
    if (cmd_ptr->cmd >= DS_ONTARGET_TEST_CMD_MAX)
    {
      LOG_MSG_ERROR_1("ds_ontarget_testi_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      ASSERT (0);
      q_put(&ds_ontarget_testi_cmd_free_q, &cmd_ptr->link);
      continue;
    }

    processed++;
    ds_ontarget_testi_cmd_handler[cmd_ptr->cmd] (cmd_ptr->cmd, cmd_ptr->user_data_ptr);

    q_put(&ds_ontarget_testi_cmd_free_q, &cmd_ptr->link);
  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer DS_ONTARGET_TEST processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if (q_cnt(&ds_ontarget_testi_cmd_q) == 0)
  {
    DS_ONTARGET_TESTI_CLR_SIGNAL(DS_ONTARGET_TEST_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION (&ds_ontarget_test_crit_sect);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if (processed > ds_ontarget_test_cmds_proc_hi_score)
  {
    ds_ontarget_test_cmds_proc_hi_score = processed;
    LOG_MSG_INFO1_1 ("ds_ontarget_testi_process_cmd(): "
                     "New high # DS_ONTARGET_TEST cmds processed: %d",
                     processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear DS_ONTARGET_TEST_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the DS_ONTARGET_TEST_CMD_SIG is set, leading to LOG_MSG_ERROR above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* ds_ontarget_testi_process_cmd() */


