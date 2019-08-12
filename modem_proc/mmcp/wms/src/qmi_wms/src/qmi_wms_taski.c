/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ WMS _T A S K I . C

DESCRIPTION
  QMI WMS Task internal API file. Contains APIs and definitions used
  internal to QMI WMS Task processing.

EXTERNALIZED FUNCTIONS
  qmi_wms_default_sig_handler()
    Implicit handler for signals processed by QMI Task.

  qmi_wms_default_cmd_handler()
    Implicit handler for commands processed by QMI Task.

  qmi_wmsi_powerup()
    Cold start called before QMI task is started.

  qmi_wmsi_process_cmd()
    Processes command signals in QMI Task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI APIs can be used unless QMI WMS task is created and
  started.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/src/qmi_wms_taski.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "dog.h"
#include "qmi_wms_taski.h"
#include "qmi_wms_task_svc.h"
#include "qmi_wms_crit_sect.h"
#include "amssassert.h"
#include "msg.h"

#include "wmsutils.h"
/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
qmi_wmsi_sig_handler_info_type qmi_wmsi_sig_handler[QMI_WMS_MAX_SIGNALS];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
rex_sigs_type qmi_wmsi_enabled_sig_mask[QMI_WMS_SIG_ARR_LEN];
#else
rex_sigs_type qmi_wmsi_enabled_sig_mask;
#endif /* FEATURE_REX_SIGS_EXT */

/*--------------------------------------------------------------------------
  QMI's own critical section
---------------------------------------------------------------------------*/
qmi_wms_crit_sect_type global_qmi_wms_crit_section;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION QMI_WMS_DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean qmi_wms_default_sig_handler
(
  qmi_wms_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_ERROR_1("qmi_wms_default_sig_handler(): Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* qmi_wms_default_sig_handler() */

/*===========================================================================
FUNCTION QMI_WMS_SVC_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI WMS task.

  This function performs initialization before QMI WMS task is started.
  Note that initializations internal to QMI should be performed before
  initializing other components that run in QMI WMS task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wmsi_powerup
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("qmi_wmsi_powerup(): QMI WMS Task Powerup");

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/

  qmi_wms_init_crit_section(&global_qmi_wms_crit_section);


  /*------------------------------------------------------------------------
    Initialize all the signal handlers
  -------------------------------------------------------------------------*/
  for( i = 0; i < (int) QMI_WMS_MAX_SIGNALS; i++ )
  {
    qmi_wmsi_sig_handler[i].sig_handler = qmi_wms_default_sig_handler;
    qmi_wmsi_sig_handler[i].user_data_ptr = NULL;
  }


  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to QMI Task
  -------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
  QMI_SET_SIG(qmi_wmsi_enabled_sig_mask, QMI_WMS_TASK_STOP_SIGNAL);
  QMI_SET_SIG(qmi_wmsi_enabled_sig_mask, QMI_WMS_TASK_OFFLINE_SIGNAL);
  QMI_SET_SIG(qmi_wmsi_enabled_sig_mask, QMI_WMS_DOG_RPT_TIMER_SIGNAL);
  QMI_SET_SIG(qmi_wmsi_enabled_sig_mask, QMI_WMS_SIG_MASK_CHANGE_SIGNAL);
#else
  qmi_wmsi_enabled_sig_mask = ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_WMS_TASK_STOP_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_WMS_TASK_OFFLINE_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_WMS_DOG_RPT_TIMER_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_WMS_SIG_MASK_CHANGE_SIGNAL );
#endif /* FEATURE_REX_SIGS_EXT */

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/
} /* qmi_wmsi_powerup() */




