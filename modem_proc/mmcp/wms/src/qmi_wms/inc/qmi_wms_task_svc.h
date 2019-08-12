#ifndef QMI_WMS_TASK_SVC_H
#define QMI_WMS_TASK_SVC_H
/*===========================================================================

                        Q M I _ WMS _ T A S K _ S V C . H

DESCRIPTION

  QMI WMS Task services external header file. All definitions, structures,
  and functions needed for performing QMI WMS task to a specific services.

EXTERNALIZED DATA
  qmi_wms_sig_enum_type
    Set of supported signals for the QMI_WMS task.

  qmi_wms_sig_handler_type
    Callback prototype for signal handler.

EXTERNALIZED FUNCTIONS
  qmi_wms_enable_sig()
    Enable a specific signal for QMI task to process.

  qmi_wms_disable_sig()
    Disable a specific signal from QMI task processing.

  qmi_wms_set_signal()
    Sets a signal for the QMI_WMS task

  qmi_wms_clear_signal()
    Clear a signal for the QMI_WMS task.

  qmi_wms_set_sig_handler()
    Sets signal handler for specific signal processed in QMI task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless Main Controller has created the QMI task
  and QMI_WMS_task() has been invoked.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/inc/qmi_wms_task_svc.h#1 $
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
TYPEDEF QMI_WMS_SIG_ENUM_TYPE

DESCRIPTION
  This enum contains the different signals that can be set on the QMI_WMS
  task.

  The value of existing signals should not be changed while adding or
  deleting a signal.  Some signals (marked Reserved) have fixed value that
  should stay as it is.
===========================================================================*/
typedef enum
{
  QMI_WMS_CMD_Q_SIGNAL                =  1,   /* QMI command Q signal          */
  QMI_WMS_DOG_RPT_TIMER_SIGNAL        =  2,   /* Watchdog Report signal        */
  QMI_WMS_SIG_MASK_CHANGE_SIGNAL      =  4,   /* Indicates sig mask has changed*/

/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  QMI_WMS_TASK_OFFLINE_SIGNAL   = 13,   /* Reserved TASK_OFFLINE_SIG     */
  QMI_WMS_TASK_STOP_SIGNAL      = 14,   /* Reserved TASK_STOP_SIG        */
  QMI_WMS_TASK_START_SIGNAL     = 15,   /* Reserved TASK_START_SIG       */

/*---------------------------------------------------------------------------
  BEWARE: Other signal need to be defined from here.
---------------------------------------------------------------------------*/
  QMI_WMS_SERVICE_WMS_SIGNAL     = 18,  /* WMS service signal */
  QMI_WMS_MAX_SIGNALS                        /* use next value for enum        */
} qmi_wms_sig_enum_type;


/*===========================================================================
TYPEDEF QMI_WMS_SIG_HANDLER_TYPE

DESCRIPTION
Type definition for the signal handler.

PARAMETERS
sig : Signal to be processed.
user_data_ptr : Signal specific user data.

Returns TRUE if signal should be cleared from the set signal mask,
FALSE if further signal processing is needed and hence signal
should not be cleared.
===========================================================================*/
typedef boolean (*qmi_wms_sig_handler_type)
(
qmi_wms_sig_enum_type sig,
void * user_data_ptr
);



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_WMS_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the QMI WMS task uses for rex_wait().

PARAMETERS
  sig   : Signal to be enabled.

DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is
  defaulted (printing an err msg.)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_enable_sig
(
  qmi_wms_sig_enum_type sig
);

/*===========================================================================
FUNCTION QMI_WMS_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

PARAMETERS
  sig   : Signal to be disabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_disable_sig
(
  qmi_wms_sig_enum_type sig
);

/*===========================================================================
FUNCTION QMI_WMS_SET_SIGNAL()

DESCRIPTION
  Sets a signal for the QMI_WMS task and notifies the scheduler that there
  is processing to be performed in QMI_WMS task.

PARAMETERS
  sig   : Signal to be set.

DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is
  defaulted (printing an err msg).

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_set_signal
(
  qmi_wms_sig_enum_type sig
);

/*===========================================================================
FUNCTION QMI_WMS_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the QMI_WMS task.

PARAMETERS
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_clear_signal
(
  qmi_wms_sig_enum_type sig
);

#endif /* QMI_WMS_TASK_SVC_H */

