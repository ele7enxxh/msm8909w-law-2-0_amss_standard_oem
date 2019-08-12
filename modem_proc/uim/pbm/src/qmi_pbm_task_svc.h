#ifndef QMI_PBM_TASK_SVC_H
#define QMI_PBM_TASK_SVC_H
/*===========================================================================

                        Q M I _ PBM _ T A S K _ S V C . H

DESCRIPTION

  QMI Mmcp Task services external header file. All definitions, structures,
  and functions needed for performing QMI Modem task to a specific services.
 
EXTERNALIZED DATA 
  qmi_pbm_sig_enum_type
    Set of supported signals for the QMI_PBM task.
 
  qmi_pbm_sig_handler_type
    Callback prototype for signal handler.
 
EXTERNALIZED FUNCTIONS  
  qmi_pbm_enable_sig()
    Enable a specific signal for QMI task to process.

  qmi_pbm_disable_sig()
    Disable a specific signal from QMI task processing.
 
  qmi_pbm_set_signal()
    Sets a signal for the QMI_PBM task 

  qmi_pbm_clear_signal()
    Clear a signal for the QMI_PBM task.
 
  qmi_pbm_set_sig_handler()
    Sets signal handler for specific signal processed in QMI task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless Main Controller has created the QMI task
  and QMI_PBM_task() has been invoked.

Copyright (c) 2011,2014 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_task_svc.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/14    NR     QTF CRM MOB Changes
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#if (defined FEATURE_UIM_TEST_FRAMEWORK && defined FEATURE_MODEM_RCINIT)
#error code not present
#endif

/*===========================================================================
TYPEDEF QMI_PBM_SIG_ENUM_TYPE
 
DESCRIPTION 
  This enum contains the different signals that can be set on the QMI_PBM 
  task. 
 
  The value of existing signals should not be changed while adding or
  deleting a signal.  Some signals (marked Reserved) have fixed value that
  should stay as it is.
===========================================================================*/
typedef enum
{
  QMI_PBM_CMD_Q_SIGNAL                =  1,   /* QMI command Q signal          */
  QMI_PBM_DOG_RPT_TIMER_SIGNAL        =  2,   /* Watchdog Report signal        */
  QMI_PBM_NV_CMD_SIGNAL               =  3,   /* NV cmd event signal           */
  QMI_PBM_SIG_MASK_CHANGE_SIGNAL      =  4,   /* Indicates sig mask has changed*/
  
/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  QMI_PBM_TASK_OFFLINE_SIGNAL   = 13,   /* Reserved TASK_OFFLINE_SIG     */
#if (defined FEATURE_UIM_TEST_FRAMEWORK && defined FEATURE_MODEM_RCINIT)
  #error code not present
#else
  QMI_PBM_TASK_STOP_SIGNAL      = 14,   /* Reserved TASK_STOP_SIG        */
#endif
  QMI_PBM_TASK_START_SIGNAL     = 15,   /* Reserved TASK_START_SIG       */

/*---------------------------------------------------------------------------
  BEWARE: Other signal need to be defined from here.
---------------------------------------------------------------------------*/

  QMI_PBM_SERVICE_NAS_SIGNAL     = 17,  /* Network Access Service Signal  */
  QMI_PBM_SERVICE_PBM_SIGNAL     = 18,  /* PBM service    signal          */

  QMI_PBM_MAX_SIGNALS                        /* use next value for enum        */
} qmi_pbm_sig_enum_type;

/*===========================================================================
TYPEDEF QMI_PBM_SIG_HANDLER_TYPE
 
DESCRIPTION 
  Type definition for the signal handler.
 
PARAMETERS 
  sig             : Signal to be processed.
  user_data_ptr   : Signal specific user data.
 
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
typedef boolean (*qmi_pbm_sig_handler_type)
(
  qmi_pbm_sig_enum_type sig,
  void *            user_data_ptr
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_PBM_ENABLE_SIG()

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
void qmi_pbm_enable_sig
(
  qmi_pbm_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_PBM_DISABLE_SIG()

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
void qmi_pbm_disable_sig
(
  qmi_pbm_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_PBM_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the QMI_PBM task and notifies the scheduler that there
  is processing to be performed in QMI_PBM task. 
 
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
void qmi_pbm_set_signal
(
  qmi_pbm_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_PBM_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the QMI_PBM task.
  
PARAMETERS 
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbm_clear_signal
(
  qmi_pbm_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_PBM_SET_SIG_HANDLER()

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
qmi_pbm_sig_handler_type qmi_pbm_set_sig_handler
(
  qmi_pbm_sig_enum_type    sig,           
  qmi_pbm_sig_handler_type sig_handler,   
  void *               user_data_ptr  
);



#endif /* QMI_PBM_TASK_SVC_H */
