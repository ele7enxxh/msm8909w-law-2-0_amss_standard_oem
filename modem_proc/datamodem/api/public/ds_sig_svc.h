#ifndef DS_SIG_SVC_H
#define DS_SIG_SVC_H

/*=========================================================================*/
/*! 
  @file 
  ds_sig_svc.h

  @brief
  Header file for DS_SIG task services.
  
  @details
  This file exports different services from DS_SIG task. This file is the 
  interface to applications to use the DS_SIG task. Clients can send a 
  command to be processed in DS_SIG task using ds_sig_send_cmd() method. Each
  command has a handler (callback function) associated with it. Clients
  can set this command handler using ds_sig_set_cmd_handler() method.
  
  @exported_data
  ds_sig_cmd_enum_type
    Supported commands for DS_SIG task.

  ds_sig_cmd_handler_type
    Callback prototype for command handler.

  ds_sig_signal_enum_type
    Set of supported signals for the DS_SIG task.

  ds_sig_signal_handler_type
    Callback prototype for signal handler.


  @exported_functions
  ds_sig_set_cmd_handler()
    Set a handler to process a cmd in DS_SIG context

  ds_sig_send_cmd() 
    Post a command to DS_SIG task

  ds_sig_set_signal_handler()
    Sets signal handler for specific signal processed in DS_SIG task.

  ds_sig_enable_signal()
    Adds specific signal for DS_SIG task to process.

  ds_sig_disable_signal()
    Removes specific signal from DS_SIG task processing.

  @note
  Please note that this file provides the interface which is used by 
  clients to send the command to DS_SIG task and process it in that task. It 
  should be as such independent of the platform specific task details. 
  
  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_sig_svc.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2010-06-10 ts   Added support for DS_SIG_SIGNAL_BUS_SET_CMD
  2009-08-17 hm   Created module.

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*!
  @brief
  DS_SIG task command definitions. 

  @details
  This enum lists all the commands that can be sent to DS_SIG task for 
  processing. 
*/
typedef enum
{
  DS_SIG_CMD_MIN                 = 0,
  DS_SIG_SIGNAL_DISPATCH_CMD     = DS_SIG_CMD_MIN,
  DS_SIG_SIGNAL_BUS_SET_CMD      = 1,
  DS_SIG_CMD_MAX,
  DS_SIG_CMD_FORCE_32_BIT_SIGNED = 0x7FFFFFFF
} ds_sig_cmd_enum_type;


/*!
  @brief
  DS_SIG command handler callback format.

  @details
  Each command that is listed in ds_sig_cmd_enum_type should have a command
  handler specified by the caller. The function ds_sig_set_cmd_handler() 
  should be used for setting the handler function. The following typedef
  lists the prototype of the handler function.
*/
typedef void (*ds_sig_cmd_handler_type)
(
  ds_sig_cmd_enum_type    cmd,           /* Command to be processed        */
  void                   *user_data_ptr  /* Command specific user data     */
);

/*!
  @brief
  DS_SIG Task Signal Definitions.  

  @details
  This enum contains the different signals that can be set on the DS_SIG 
  task. The value of existing signals should not be changed while adding or 
  deleting a signal.  Some signals (marked RESERVED) have fixed value that 
  should stay as it is.
*/
typedef enum
{
  DS_SIG_SIGNAL_MIN                  =  1,   /*! Validation purposes only  */
  DS_SIG_CMD_Q_SIGNAL                =  1,   /*! DS_SIG command Q signal   */
  DS_SIG_DOG_HB_REPORT_SIGNAL        =  3,   /*! Dog HB report signal      */
  DS_SIG_SIG_MASK_CHANGE_SIGNAL      =  5,   /*! Sig mask has changed      */

/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  DS_SIG_TASK_OFFLINE_SIGNAL         = 13,   /*! RESERVED TASK_OFFLINE_SIG */
  DS_SIG_TASK_STOP_SIGNAL            = 14,   /*! RESERVED TASK_STOP_SIG    */
  DS_SIG_TASK_START_SIGNAL           = 15,   /*! RESERVED TASK_START_SIG   */

  DS_SIG_SIGNAL_MAX,                         /*! use next value for enum   */
  DS_SIG_SIGNAL_FORCE_32_BIT_SIGNED  = 0x7FFFFFFF
} ds_sig_signal_enum_type;

/*!
  @brief  
  Type definition for the signal handler.

  @details
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
*/
typedef boolean (*ds_sig_signal_handler_type)
(
  ds_sig_signal_enum_type   sig,           /* Signal to be processed       */
  void                     *user_data_ptr  /* Parameter carrying user data */
);


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================*/
/*!
  @function
  ds_sig_send_cmd

  @brief
  Send command to be processed in DS_SIG task.

  @details
  This method is used to send a command to the DS_SIG task for processing. 
  The command handler registered with the command being sent is executed
  when the DS_SIG task is run the next time. The DS_SIG task is serialized 
  through a command Q, so any pending commands in the DS_SIG task would be
  executed before the posted command. 

  @params[in]  cmd - Command to send to DS SIG task.
  @params[in]  user_data_ptr - User data sent with the command. The 
               command handler would be called with this user data.

  @dependencies
  None.

  @return
  None.

  @note
  None.
*/
/*=========================================================================*/
void ds_sig_send_cmd
(
  ds_sig_cmd_enum_type   cmd,            /* Command to be processed        */
  void                  *user_data_ptr   /* Command specific user data     */
);

/*=========================================================================*/
/*!
  @function
  ds_sig_set_cmd_handler()

  @brief
  This function sets a handler for the specified cmd type.  If a NULL cmd
  handler is passed, then the command handling is defaulted (see 
  ds_sigi_default_cmd_handler). The command handler can be updated on the 
  fly by calling this funciton again.

  @params[in]  cmd - Command for which we want to set handler.
  @params[in]  cmd_handler - Callback function to be registered as handler.

  @dependencies
  None.

  @return
  Previously registered cmd handler

  @note
  None.
*/
/*=========================================================================*/
ds_sig_cmd_handler_type ds_sig_set_cmd_handler
(
  ds_sig_cmd_enum_type      cmd,         /* Command name                   */
  ds_sig_cmd_handler_type   cmd_handler  /* Cmd handler function pointer   */
);


/*=========================================================================*/
/*!
  @function
  ds_sig_enable_signal

  @brief
  Sets a signal for the DS_SIG task.

  @details
  This function notifies the scheduler that there is processing to be 
  performed in DS_SIG task. 

  @params[in]  sig - Signal to be enabled.

  @dependencies
  Signal handler must be set in advance. Otherwise signal handling is 
  defaulted (printing an err msg.)

  @return
  None.

  @note
  None.
*/
/*=========================================================================*/
void ds_sig_enable_signal
(
  ds_sig_signal_enum_type   sig               /* Signal to be enabled      */
);

/*=========================================================================*/
/*!
  @function
  ds_sig_disable_signal

  @brief
  Clear a signal for the DS_SIG task.

  @details
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

  @params[in]  sig - Signal to be disabled.

  @dependencies
  None.

  @return
  None.

  @note
  None.
*/
/*=========================================================================*/
void ds_sig_disable_signal
(
  ds_sig_signal_enum_type      sig            /* Signal to be disabled     */
);

/*=========================================================================*/
/*!
  @function
  ds_sig_set_signal_handler

  @brief
  This function sets a handler for the specified signal.

  @details
  If the handler is NULL, processing of the signal is defaulted. If the 
  signal handler is already set, then it is updated to use the new signal
  handler.

  @param[in]  sig - Signal for which the handler is to be set.
  @param[in]  sig_handler - Callback function for handling signal enable.
  @param[in]  user_data_ptr - User data that would be used while the signal
              handler is called. As opposed to commands where user_data is 
              specific to a give command, user data for signal handling 
              can be set only once while setting the signal handler.

  @dependencies
  None.

  @return
  Old signal handler, if any was set. NULL otherwise.

  @note
  None.
*/
/*=========================================================================*/
ds_sig_signal_handler_type ds_sig_set_signal_handler
(
  ds_sig_signal_enum_type      sig,          /* Signal assoc with handler  */
  ds_sig_signal_handler_type   sig_handler,  /* Signal handler callback    */
  void                        *user_data_ptr /* User data for handler      */
);

/*=========================================================================*/
/*!
  @function
  ds_sig_is_current_task

  @brief
  Checks if the current task is DS_SIG.

  @return TRUE  - If current task is DS_SIG
  @return FALSE - If current task is not DS_SIG
*/
/*=========================================================================*/
boolean ds_sig_is_current_task
(
  void
);


#ifdef __cplusplus
}
#endif


#endif /* DS_SIG_SVC_H */

