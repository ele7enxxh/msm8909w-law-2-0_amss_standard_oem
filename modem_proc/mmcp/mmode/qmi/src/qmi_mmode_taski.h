#ifndef QMI_MMODE_TASKI_H
#define QMI_MMODE_TASKI_H
/*===========================================================================

                          Q M I _ M M O D E _ T A S K I . H

DESCRIPTION
  QMI Mmcp Task internal header file, has function, data declarations for
  internal usage by QMI Mmcp Task.

EXTERNALIZED FUNCTIONS
  qmi_mmode_default_sig_handler()
    Implicit handler for signals processed by QMI Task.

  qmi_mmode_default_cmd_handler()
    Implicit handler for commands processed by QMI Task.

  qmi_mmodei_powerup()
    Cold start called before QMI task is started.

  qmi_mmodei_process_cmd()
    Processes command signals in QMI Task.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_mmode_taski.h#1 $ 

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12     sk     Created module.
===========================================================================*/

#include "comdef.h"
#include "customer.h"       
#include "rex.h"
#include "queue.h"
#include "qmi_mmode_task_svc.h"
#include "qmi_mmode_task_cmd.h"
#include "qmi_mmode_crit_sect.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers.
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_mmode_sig_handler_type  sig_handler;
  void *                user_data_ptr;
} qmi_mmodei_sig_handler_info_type;

/*--------------------------------------------------------------------------
  Datatype to hold signal, command handlers. 
---------------------------------------------------------------------------*/
extern qmi_mmodei_sig_handler_info_type qmi_mmodei_sig_handler[QMI_MMODE_MAX_SIGNALS];
extern qmi_mmode_cmd_handler_type qmi_mmodei_cmd_handler[QMI_MMODE_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Watchdog timer for QMI MMODE task.
---------------------------------------------------------------------------*/
extern dog_report_type  qmi_mmode_dog_rpt_var;                        /* Dog report ID */

/*--------------------------------------------------------------------------
  Command queues handled by QMI MMODE task. 
---------------------------------------------------------------------------*/
extern q_type qmi_mmodei_cmd_q;
extern q_type qmi_mmodei_cmd_free_q;

/*--------------------------------------------------------------------------
  The command buffer count.
---------------------------------------------------------------------------*/
#define QMI_MMODEI_CMD_BUF_CNT  (300)
extern qmi_mmodei_cmd_type qmi_mmodei_cmd_buf[ QMI_MMODEI_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled for QMI main processing loop.
---------------------------------------------------------------------------*/
extern rex_sigs_type qmi_mmodei_enabled_sig_mask;

/*--------------------------------------------------------------------------
  QMI's own critical section.
---------------------------------------------------------------------------*/
//extern qmi_mmode_crit_sect_type global_qmi_mmode_crit_section;

/*--------------------------------------------------------------------------
  Pointer to qmi_mmode_task_tcb
---------------------------------------------------------------------------*/
extern rex_tcb_type *qmi_mmode_tcb_ptr;

/*===========================================================================

                           PUBLIC MACRO DEFINITIONS

===========================================================================*/
  
#define QMI_MMODE_SET_SIGNAL( sig )                                               \
  (void)rex_set_sigs( qmi_mmode_tcb_ptr, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

#define QMI_MMODE_CLR_SIGNAL( sig )                                               \
  (void)rex_clr_sigs( qmi_mmode_tcb_ptr, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

#define QMI_MMODE_OFFSETOF(type,member) (((char*)(&((type*)1)->member))-((char*)1))

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_MMODE_DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean 
qmi_mmode_default_sig_handler
(
  qmi_mmode_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*===========================================================================
FUNCTION QMI_MMODE_DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the
  commands which do not have a registered handler. It merely prints a warning
  message.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmode_default_cmd_handler
(
  qmi_mmode_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================
FUNCTION QMI_MMODEI_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI_MMODE task. This
  function will perform all first time thru functions for the QMI_MMODE
  task. After this intiailzation, the QMI_MMODE task would signal the TMC
  that it is ready to accept requests from other tasks. Any initializations
  that are NOT dependent upon other tasks should be performed here.
 
PARAMETERS 
  None.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmodei_powerup
(
  void
);

/*===========================================================================
FUNCTION QMI_MMODEI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to QMI_MMODE_CMD_Q_SIGNAL.  It retrieves the
  next command from the QMI command queue and processes the command by calling 
  the registered cmd handler.
 
PARAMETERS 
  None.
  
DEPENDENCIES
  An item must be on the QMI command queue or an ERR_FATAL will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function 
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean qmi_mmodei_process_cmd
( 
  void
);

/*===========================================================================

FUNCTION QMI_MMODEI_PROCESS_MSGR()

DESCRIPTION
  This function is called in response to QMI_MMODE_TASK_MSGR_SIGNAL. It 
  processes the commands from the DCC msgr queue.

DEPENDENCIES
  An item must be on the QMI_MMODE msgr queue.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_mmodei_process_msgr
(
  void
);

#endif /* QMI_MMODE_TASKI_H */
