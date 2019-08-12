#ifndef QMI_MMODE_TASK_SVC_H
#define QMI_MMODE_TASK_SVC_H
/*===========================================================================

                        Q M I _ M M C P _ T A S K _ S V C . H

DESCRIPTION

  QMI Mmcp Task services external header file. All definitions, structures,
  and functions needed for performing QMI Modem task to a specific services.
 
EXTERNALIZED DATA 
  qmi_mmode_sig_enum_type
    Set of supported signals for the QMI_MMODE task.
 
  qmi_mmode_sig_handler_type
    Callback prototype for signal handler.
 
EXTERNALIZED FUNCTIONS  
  qmi_mmode_enable_sig()
    Enable a specific signal for QMI task to process.

  qmi_mmode_disable_sig()
    Disable a specific signal from QMI task processing.
 
  qmi_mmode_set_signal()
    Sets a signal for the QMI_MMODE task 

  qmi_mmode_clear_signal()
    Clear a signal for the QMI_MMODE task.
 
  qmi_mmode_set_sig_handler()
    Sets signal handler for specific signal processed in QMI task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless Main Controller has created the QMI task
  and QMI_MMODE_task() has been invoked.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_mmode_task_svc.h#1 $ 
  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/12    sk     Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
TYPEDEF QMI_MMODE_SIG_ENUM_TYPE
 
DESCRIPTION 
  This enum contains the different signals that can be set on the QMI_MMODE 
  task. 
 
  The value of existing signals should not be changed while adding or
  deleting a signal.  Some signals (marked Reserved) have fixed value that
  should stay as it is.
===========================================================================*/
typedef enum
{
  QMI_MMODE_CMD_Q_SIGNAL                =  1,   /* QMI command Q signal          */
  QMI_MMODE_DOG_RPT_TIMER_SIGNAL        =  2,   /* Watchdog Report signal        */
  QMI_MMODE_NV_CMD_SIGNAL               =  3,   /* NV cmd event signal           */
  QMI_MMODE_SIG_MASK_CHANGE_SIGNAL      =  4,   /* Indicates sig mask has changed*/
  QMI_MMODE_TASK_MSGR_SIGNAL            =  5,   /* MSGR signal                   */  
  
/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  QMI_MMODE_TASK_OFFLINE_SIGNAL   = 13,   /* Reserved TASK_OFFLINE_SIG     */
  QMI_MMODE_TASK_STOP_SIGNAL      = 14,   /* Reserved TASK_STOP_SIG        */
  QMI_MMODE_TASK_START_SIGNAL     = 15,   /* Reserved TASK_START_SIG       */

/*---------------------------------------------------------------------------
  BEWARE: Other signal need to be defined from here.
---------------------------------------------------------------------------*/
  QMI_MMODE_SERVICE_VOICE_SIGNAL   = 16,  /* Voice service signal           */

  QMI_MMODE_MAX_SIGNALS                        /* use next value for enum        */
} qmi_mmode_sig_enum_type;

/*===========================================================================
TYPEDEF QMI_MMODE_SIG_HANDLER_TYPE
 
DESCRIPTION 
  Type definition for the signal handler.
 
PARAMETERS 
  sig             : Signal to be processed.
  user_data_ptr   : Signal specific user data.
 
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
typedef boolean (*qmi_mmode_sig_handler_type)
(
  qmi_mmode_sig_enum_type sig,
  void *            user_data_ptr
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_MMODE_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the QMI modem task uses for rex_wait().
 
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
void qmi_mmode_enable_sig
(
  qmi_mmode_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_MMODE_DISABLE_SIG()

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
void qmi_mmode_disable_sig
(
  qmi_mmode_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_MMODE_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the QMI_MMODE task and notifies the scheduler that there
  is processing to be performed in QMI_MMODE task. 
 
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
void qmi_mmode_set_signal
(
  qmi_mmode_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_MMODE_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the QMI_MMODE task.
  
PARAMETERS 
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmode_clear_signal
(
  qmi_mmode_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_MMODE_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.
 
  If the handler is NULL, processing of the signal is defaulted. If the 
  signal handler is already set, then it is updated to use the new signal
  handler.
 
PARAMETERS 
  sig           : Signal for which the handler is to be set.
  sig_handler   : Callback function for handling signal enable.
  user_data_ptr : User data that would be used while the signal
                  handler is called.user data for signal handling 
                  can be set only once while setting the signal handler.
DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler, if any was set. NULL otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_mmode_sig_handler_type qmi_mmode_set_sig_handler
(
  qmi_mmode_sig_enum_type    sig,           
  qmi_mmode_sig_handler_type sig_handler,   
  void *               user_data_ptr  
);


#endif /* QMI_MMODE_TASK_SVC_H */
