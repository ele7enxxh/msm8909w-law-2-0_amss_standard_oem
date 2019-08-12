#ifndef DS_ONTARGET_TEST_SVC_H
#define DS_ONTARGET_TEST_SVC_H

/*=========================================================================*/
/*! 
  @file 
  ds_ontarget_test_svc.h

  @brief
  Header file for DS_ONTARGET_TEST task services.
  
  @details
  This file exports different services from DS_ONTARGET_TEST task. This file is the 
  interface to applications to use the DS_ONTARGET_TEST task. Clients can send a 
  command to be processed in DS_ONTARGET_TEST task using ds_ontarget_test_send_cmd() method. Each
  command has a handler (callback function) associated with it. Clients
  can set this command handler using ds_ontarget_test_set_cmd_handler() method.
  
  @exported_data
  ds_ontarget_test_cmd_enum_type
    Supported commands for DS_ONTARGET_TEST task.

  ds_ontarget_test_cmd_handler_type
    Callback prototype for command handler.

  ds_ontarget_test_signal_enum_type
    Set of supported signals for the DS_ONTARGET_TEST task.

  ds_ontarget_test_signal_handler_type
    Callback prototype for signal handler.


  @exported_functions
  ds_ontarget_test_set_cmd_handler()
    Set a handler to process a cmd in DS_ONTARGET_TEST context

  ds_ontarget_test_send_cmd() 
    Post a command to DS_ONTARGET_TEST task

  ds_ontarget_test_set_signal_handler()
    Sets signal handler for specific signal processed in DS_ONTARGET_TEST task.

  ds_ontarget_test_enable_signal()
    Adds specific signal for DS_ONTARGET_TEST task to process.

  ds_ontarget_test_disable_signal()
    Removes specific signal from DS_ONTARGET_TEST task processing.

  ds_ontarget_test_wait_for_callback()
    Makes the task to wait until ds_ontarget_test_post_callback() is called,
    or a timeout occurred.

  ds_ontarget_test_post_callback() 
    Notifies the task that a callback has arrived. The callback id information
    will be given in ds_ontarget_test_wait_for_callback()

  ds_ontarget_test_clear_callback_q()
    Clear any callback pending from the callback queue.

  ds_ontarget_test_stop_test_excecution()
    Stops a test execution. If the task is not currently running a test,
    This function does nothing.


  @note
  Please note that this file provides the interface which is used by 
  clients to send the command to DS_ONTARGET_TEST task and process it in that task. It 
  should be as such independent of the platform specific task details. 
  
  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/ds_ontarget_test_svc.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2010-06-10 ts   Added support for DS_ONTARGET_TEST_SIGNAL_BUS_SET_CMD
  2009-08-17 hm   Created module.

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "on_target_data_test_v01.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*! Indicate that callback didn't arrived */
#define CALLBACK_TIMEOUT (-1)

/*! Indicate that user has stopped the test execution */
#define CALLBACK_STOP_TEST (-2)

/*! Maximal length of test group name */
#define TEST_GROUP_NAME_MAX_LEN (32)

/*!
  @brief
  DS_ONTARGET_TEST task command definitions. 

  @details
  This enum lists all the commands that can be sent to DS_ONTARGET_TEST task for 
  processing. 
*/
typedef enum
{
  DS_ONTARGET_TEST_CMD_MIN                 = 0,
  DS_ONTARGET_TEST_START_CMD               = DS_ONTARGET_TEST_CMD_MIN,
  DS_ONTARGET_TEST_CMD_MAX,
  DS_ONTARGET_TEST_CMD_FORCE_32_BIT_SIGNED = 0x7FFFFFFF
} ds_ontarget_test_cmd_enum_type;


/*!
  @brief
  DS_ONTARGET_TEST command handler callback format.

  @details
  Each command that is listed in ds_ontarget_test_cmd_enum_type should have a command
  handler specified by the caller. The function ds_ontarget_test_set_cmd_handler() 
  should be used for setting the handler function. The following typedef
  lists the prototype of the handler function.
*/
typedef void (*ds_ontarget_test_cmd_handler_type)
(
  ds_ontarget_test_cmd_enum_type  cmd,           /* Command to be processed        */
  void                          * user_data_ptr  /* Command specific user data     */
);

/*!
  @brief
  DS_ONTARGET_TEST Task Signal Definitions.  

  @details
  This enum contains the different signals that can be set on the DS_ONTARGET_TEST 
  task. The value of existing signals should not be changed while adding or 
  deleting a signal.  Some signals (marked RESERVED) have fixed value that 
  should stay as it is.
*/
typedef enum
{
  DS_ONTARGET_TEST_SIGNAL_MIN                  =  1,   /*! Validation purposes only  */
  DS_ONTARGET_TEST_CMD_Q_SIGNAL                =  1,   /*! DS_ONTARGET_TEST command Q signal   */
  DS_ONTARGET_TEST_SIG_MASK_CHANGE_SIGNAL      =  5,   /*! Sig mask has changed      */

  DS_ONTARGET_TEST_SIGNAL_MAX,                         /*! use next value for enum   */
  DS_ONTARGET_TEST_SIGNAL_FORCE_32_BIT_SIGNED  = 0x7FFFFFFF
} ds_ontarget_test_signal_enum_type;

/*!
  @brief  
  Type definition for the signal handler.

  @details
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
*/
typedef boolean (*ds_ontarget_test_signal_handler_type)
(
  ds_ontarget_test_signal_enum_type   sig,           /* Signal to be processed       */
  void                              * user_data_ptr  /* Parameter carrying user data */
);

/*!
  @brief
  User data structure for tests

  @details
  This structure is used for user data ptr
*/

typedef struct
{
  char test_group_name[TEST_GROUP_NAME_MAX_LEN];
  /*! Test Group Name */
  int test_number;
  /*! Test Number */
  otdt_task_priority_enum_v01 priority;
  /*! Priority of the task that runs the test */
  void* qmi_handle;
  /*! Handle for ds_qmi_otdt_send_ind */
}ds_ontarget_test_cmd_data_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================*/
/*!
  @function
  ds_ontarget_test_send_cmd

  @brief
  Send command to be processed in DS_ONTARGET_TEST task.

  @details
  This method is used to send a command to the DS_ONTARGET_TEST task for processing. 
  The command handler registered with the command being sent is executed
  when the DS_ONTARGET_TEST task is run the next time. The DS_ONTARGET_TEST task is serialized 
  through a command Q, so any pending commands in the DS_ONTARGET_TEST task would be
  executed before the posted command. 

  @params[in]  cmd - Command to send to DS SIG task.
  @params[in]  user_data_ptr - User data sent with the command. The 
               command handler would be called with this user data.

  @dependencies
  None.

  @return
  0  success
  -1 failure

  @note
  None.
*/
/*=========================================================================*/
int ds_ontarget_test_send_cmd
(
  ds_ontarget_test_cmd_enum_type   cmd,            /* Command to be processed        */
  void                           * user_data_ptr   /* Command specific user data     */
);

/*=========================================================================*/
/*!
  @function
  ds_ontarget_test_set_cmd_handler()

  @brief
  This function sets a handler for the specified cmd type.  If a NULL cmd
  handler is passed, then the command handling is defaulted (see 
  ds_ontarget_testi_default_cmd_handler). The command handler can be updated on the 
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
ds_ontarget_test_cmd_handler_type ds_ontarget_test_set_cmd_handler
(
  ds_ontarget_test_cmd_enum_type      cmd,         /* Command name                   */
  ds_ontarget_test_cmd_handler_type   cmd_handler  /* Cmd handler function pointer   */
);


/*=========================================================================*/
/*!
  @function
  ds_ontarget_test_enable_signal

  @brief
  Sets a signal for the DS_ONTARGET_TEST task.

  @details
  This function notifies the scheduler that there is processing to be 
  performed in DS_ONTARGET_TEST task. 

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
void ds_ontarget_test_enable_signal
(
  ds_ontarget_test_signal_enum_type   sig               /* Signal to be enabled      */
);

/*=========================================================================*/
/*!
  @function
  ds_ontarget_test_disable_signal

  @brief
  Clear a signal for the DS_ONTARGET_TEST task.

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
void ds_ontarget_test_disable_signal
(
  ds_ontarget_test_signal_enum_type      sig            /* Signal to be disabled     */
);

/*=========================================================================*/
/*!
  @function
  ds_ontarget_test_set_signal_handler

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
ds_ontarget_test_signal_handler_type ds_ontarget_test_set_signal_handler
(
  ds_ontarget_test_signal_enum_type      sig,          /* Signal assoc with handler  */
  ds_ontarget_test_signal_handler_type   sig_handler,  /* Signal handler callback    */
  void                                 * user_data_ptr /* User data for handler      */
);

/*=========================================================================*/
/*!
  @function
  ds_ontarget_test_is_current_task

  @brief
  Checks if the current task is DS_ONTARGET_TEST.

  @return TRUE  - If current task is DS_ONTARGET_TEST
  @return FALSE - If current task is not DS_ONTARGET_TEST
*/
/*=========================================================================*/
boolean ds_ontarget_test_is_current_task
(
  void
);

/*=========================================================================*/
/*!
  @function
  ds_ontarget_test_wait_for_callback

  @brief
  Makes the task to wait until ds_ontarget_test_post_callback() is called,
  or a timeout occurred.

  @return callback_id as posted on ds_ontarget_test_post_callback
*/
/*=========================================================================*/
int ds_ontarget_test_wait_for_callback
(
  void
);

/*=========================================================================*/
/*!

  @function
  ds_ontarget_test_post_callback
  
  @brief
  Notifies the task that a callback has arrived. The callback id information
  will be given in ds_ontarget_test_wait_for_callback()

  @return 
  None.
*/
/*=========================================================================*/
void ds_ontarget_test_post_callback
(
  int callback_id
);

/*=========================================================================*/
/*!

  @function
  ds_ontarget_test_clear_callback_q

  @brief
  Clear any callback pending from the callback queue.

  @return
  None.
*/
/*=========================================================================*/
void ds_ontarget_test_clear_callback_q
(
  void
);

/*=========================================================================*/
/*!

  @function
  ds_ontarget_test_stop_test_excecution

  @brief
  Stops a test execution. If the task is not currenly running a test,
  This function does nothing.

  @return
  None.
*/
/*=========================================================================*/
void ds_ontarget_test_stop_test_excecution
(
  void
);



#ifdef __cplusplus
}
#endif


#endif /* DS_ONTARGET_TEST_SVC_H */

