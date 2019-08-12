#ifndef QMI_MODEM_TASKI_H
#define QMI_MODEM_TASKI_H
/*===========================================================================

                          Q M I _ M O D E M _ T A S K I . H

DESCRIPTION
  QMI Modem Task internal header file, has function, data declarations for
  internal usage by QMI Modem Task.

EXTERNALIZED FUNCTIONS
  qmi_default_sig_handler()
    Implicit handler for signals processed by QMI Task.

  qmi_default_cmd_handler()
    Implicit handler for commands processed by QMI Task.

  qmii_powerup()
    Cold start called before QMI task is started.

  qmii_process_cmd()
    Processes command signals in QMI Task.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/qmi_modem_taski.h#1 $ 
  $DateTime: 2005/12/23 17:19:09

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       


#include "rex.h"
#include "queue.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_task_cmd.h"
#include "qmi_crit_sect.h"
#include "rcinit_rex.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers.
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_sig_handler_type  sig_handler;
  void *                user_data_ptr;
} qmii_sig_handler_info_type;

/*--------------------------------------------------------------------------
  Datatype to hold signal, command handlers. 
---------------------------------------------------------------------------*/
extern qmii_sig_handler_info_type qmii_sig_handler[QMI_MAX_SIGNALS];
extern qmi_cmd_handler_type qmii_cmd_handler[QMI_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Watchdog report id for QMI task.
---------------------------------------------------------------------------*/
extern dog_report_type qmi_dog_rpt_id;                   /* Dog report ID */

/*--------------------------------------------------------------------------
  Command queues handled by QMI task. 
---------------------------------------------------------------------------*/
extern q_type qmii_cmd_q;
extern q_type qmii_cmd_free_q;

/*--------------------------------------------------------------------------
  The command buffer count.
---------------------------------------------------------------------------*/
#define QMII_CMD_BUF_CNT  (300)
extern qmii_cmd_type qmii_cmd_buf[ QMII_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled for QMI main processing loop.
---------------------------------------------------------------------------*/
  extern rex_sigs_type qmii_enabled_sig_mask;

/*--------------------------------------------------------------------------
  QMI's own critical section.
---------------------------------------------------------------------------*/
extern qmi_crit_sect_type global_qmi_crit_section;

/*===========================================================================

                           PUBLIC MACRO DEFINITIONS

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define QMI_SET_SIGNAL( sig )                                             \
  {                                                                         \
    (void)rex_set_sigs( rex_qmi_modem_tcb,                                  \
                       (rex_sigs_type) 1 << (rex_sigs_type)( sig ) );       \
  }
#else
  #define QMI_SET_SIGNAL( sig )                                             \
    (void)rex_set_sigs( &qmi_modem_tcb, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )
#endif

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define QMI_CLR_SIGNAL( sig )                                             \
  {                                                                         \
    (void)rex_clr_sigs( rex_qmi_modem_tcb,                                  \
                        (rex_sigs_type) 1 << (rex_sigs_type)( sig ) );      \
  }
#else
  #define QMI_CLR_SIGNAL( sig )                                             \
    (void)rex_clr_sigs( &qmi_modem_tcb, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_DEFAULT_SIG_HANDLER()

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
qmi_default_sig_handler
(
  qmi_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*===========================================================================
FUNCTION QMI_DEFAULT_CMD_HANDLER()

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
void qmi_default_cmd_handler
(
  qmi_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================
FUNCTION QMII_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI_MODEM task. This
  function will perform all first time thru functions for the QMI_MODEM
  task. After this intiailzation, the QMI_MODEM task would signal the TMC
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
void qmii_powerup
(
  void
);

/*===========================================================================
FUNCTION QMII_PROCESS_CMD()

DESCRIPTION
  This function is called in response to QMI_CMD_Q_SIGNAL.  It retrieves the
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
boolean qmii_process_cmd
( 
  void
);

#endif /* QMI_MODEM_TASKI_H */
