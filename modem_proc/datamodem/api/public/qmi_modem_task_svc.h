#ifndef QMI_MODEM_TASK_SVC_H
#define QMI_MODEM_TASK_SVC_H
/*===========================================================================

                        Q M I _ M O D E M _ T A S K _ S V C . H

DESCRIPTION

  QMI Modem Task services external header file. All definitions, structures,
  and functions needed for performing QMI Modem task to a specific services.
 
EXTERNALIZED DATA 
  qmi_sig_enum_type
    Set of supported signals for the QMI_MODEM task.
 
  qmi_sig_handler_type
    Callback prototype for signal handler.
 
EXTERNALIZED FUNCTIONS  
  qmi_enable_sig()
    Enable a specific signal for QMI task to process.

  qmi_disable_sig()
    Disable a specific signal from QMI task processing.
 
  qmi_set_signal()
    Sets a signal for the QMI_MODEM task 

  qmi_clear_signal()
    Clear a signal for the QMI_MODEM task.
 
  qmi_set_sig_handler()
    Sets signal handler for specific signal processed in QMI task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless Main Controller has created the QMI task
  and qmi_modem_task() has been invoked.

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/qmi_modem_task_svc.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
TYPEDEF QMI_SIG_ENUM_TYPE
 
DESCRIPTION 
  This enum contains the different signals that can be set on the QMI_MODEM 
  task. 
 
  The value of existing signals should not be changed while adding or
  deleting a signal.  Some signals (marked Reserved) have fixed value that
  should stay as it is.
===========================================================================*/
typedef enum
{
  QMI_CMD_Q_SIGNAL                =  1,   /* QMI command Q signal          */
  QMI_QMUX_WAIT_SIGNAL            =  2,   /* QMI QMUX sync wait signal     */
  QMI_DOG_HB_REPORT_SIGNAL        =  3,   /* Dog HB Report signal          */
  QMI_NV_CMD_SIGNAL               =  4,   /* NV cmd event signal           */
  QMI_SIG_MASK_CHANGE_SIGNAL      =  5,   /* Indicates sig mask has changed*/
  QMI_QMUX_RX_SIGNAL              =  7,   /* QMI QMUX Rx pkt arrived       */
  QMI_QMUX_TIMEOUT_SIGNAL         =  8,   /* QMI QMUX sync wait timeout    */
  
/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  QMI_MODEM_TASK_OFFLINE_SIGNAL   = 13,   /* Reserved TASK_OFFLINE_SIG     */
  QMI_MODEM_TASK_STOP_SIGNAL      = 14,   /* Reserved TASK_STOP_SIG        */
  QMI_MODEM_TASK_START_SIGNAL     = 15,   /* Reserved TASK_START_SIG       */

/*---------------------------------------------------------------------------
  BEWARE: Other signal need to be defined from here.
---------------------------------------------------------------------------*/
  QMI_QMUX_SERVICE_CTL_SIGNAL     = 16,  /* QMI service control signal     */
  QMI_QCSI_SERVICE_DPM_SIGNAL     = 17,  /* Data Port Mapper signal        */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_1 = 18,  /* QMUX QCSI shim layer service signal 1 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_2 = 19,  /* QMUX QCSI shim layer service signal 2 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_3 = 20,  /* QMUX QCSI shim layer service signal 3 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_4 = 21,  /* QMUX QCSI shim layer service signal 4 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_5 = 22,  /* QMUX QCSI shim layer service signal 5 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_6 = 23,  /* QMUX QCSI shim layer service signal 6 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_7 = 24,  /* QMUX QCSI shim layer service signal 7 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_8 = 25,  /* QMUX QCSI shim layer service signal 8 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_9 = 26,  /* QMUX QCSI shim layer service signal 9 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_10 = 27,  /* QMUX QCSI shim layer service signal 10 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_11 = 28,  /* QMUX QCSI shim layer service signal 11 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_12 = 29,  /* QMUX QCSI shim layer service signal 12 */
  QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_13 = 30,  /* QMUX QCSI shim layer service signal 12 */

  QMI_MAX_SIGNALS                        /* use next value for enum        */
} qmi_sig_enum_type;

/*===========================================================================
TYPEDEF QMI_SIG_HANDLER_TYPE
 
DESCRIPTION 
  Type definition for the signal handler.
 
PARAMETERS 
  sig             : Signal to be processed.
  user_data_ptr   : Signal specific user data.
 
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
typedef boolean (*qmi_sig_handler_type)
(
  qmi_sig_enum_type sig,
  void *            user_data_ptr
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_ENABLE_SIG()

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
void qmi_enable_sig
(
  qmi_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_DISABLE_SIG()

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
void qmi_disable_sig
(
  qmi_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the QMI_MODEM task and notifies the scheduler that there
  is processing to be performed in QMI_MODEM task. 
 
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
void qmi_set_signal
(
  qmi_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the QMI_MODEM task.
  
PARAMETERS 
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_clear_signal
(
  qmi_sig_enum_type sig                       
);

/*===========================================================================
FUNCTION QMI_SET_SIG_HANDLER()

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
qmi_sig_handler_type qmi_set_sig_handler
(
  qmi_sig_enum_type    sig,           
  qmi_sig_handler_type sig_handler,   
  void *               user_data_ptr  
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QMI_MODEM_TASK_SVC_H */
