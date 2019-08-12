/*============================================================================
  FILE:       sdtask.h

  OVERVIEW:   

              Copyright (c) 2012 Qualcomm Technologies Incorporated.
              All Rights Reserved.
              Qualcomm Confidential and Proprietary
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdtask.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/13   jvo     Added sdtask_is_init_complete
??                 First version
===========================================================================*/

#ifndef _SDTASK_H_
#define _SDTASK_H_

#include "comdef.h"
#include "queue.h"
#include "sd.h"
#include "mcfg_refresh.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ref_cnt_obj.h"

#ifdef __cplusplus
}
#endif

/* Event to indicate that SD task is ready to process commands */
#define SD_TASK_RCEVT_READY "sdtask:ready"

/*=============================================================================
  Framework for command dispatch in SD.  Note that clients of SD should not
  use this directly, but rather should use the external SD APIs.
=============================================================================*/


/*-----------------------------------------------------------------------------
  Forward declaration of the sd_cmd_t type so that we can use it in the
  function type declarations.
-----------------------------------------------------------------------------*/
typedef struct  sd_cmd_s sd_cmd_t;


/*-----------------------------------------------------------------------------
  execute_fn
    Signature of pointer to function that executes a command.
-----------------------------------------------------------------------------*/
typedef sd_ss_act_e_type  (*execute_fn)(sd_cmd_t *cmd);

/*-----------------------------------------------------------------------------
  dtor_fn
    Signature of pointer to function that cleans up any command-specific
    resources.  Note that any resources held in SD_CMD_HDR below are dealt
    with by the framework and MUST NOT be cleaned up in this function.
-----------------------------------------------------------------------------*/
typedef void  (*dtor_fn)(sd_cmd_t *cmd);

/*-----------------------------------------------------------------------------
  cmd_cb
    Signature of pointer to callback function to be executed when an
    asynchronous command has completed.
-----------------------------------------------------------------------------*/
typedef void  (*cmd_cb)(void *context);


/*-----------------------------------------------------------------------------
  Type for signals attached to commands.  This is used differently depending on
  whether SD is built for target (which uses BLAST signals) or for SUTE (which
  uses Windows HANDLES).
-----------------------------------------------------------------------------*/
typedef uint32  sd_signal_t;


/*-----------------------------------------------------------------------------
  SD_CMD_HDR
    This must be the first element in any command structure - it defines the
    base elements necessary for all commands.  The various components are:

    _REF_CNT_OBJ
      Fields required for reference-counting the commands.
    link
      Used to link the commands into the command queue.
    execute
      Pointer to the function to call in order to execute the command.  May not
      be NULL.
    dtor
      Pointer to the function to call to clean up any resources used by the
      specific command.  May be NULL if there are no command-specific resources;
      all resources used in SD_CMD_HDR will be cleaned up by the framework.
    cb
      Pointer to a callback function to be called on completion of an
      asynchronous command.
    context
      Pointer to the context to be passed in the call to the callback function
      called on completion of an asynchronous command.
    signal
      The signal associated with this command.  Asynchronous commands should
      have this set to NULL_SIGNAL.
    tcb_ptr
      TCB of task which queued request to SD.
      
    act
      The action to be returned as the return value of a synchronous API.
    act_ptr
      Pointer to the action structure to be filled in as a result of the command
      execution.  May be NULL.
-----------------------------------------------------------------------------*/
#define SD_CMD_HDR                \
  _REF_CNT_OBJ;                   \
  q_link_type       link;         \
  execute_fn        execute;      \
  sd_ss_e_type      ss;           \
  dtor_fn           dtor;         \
  cmd_cb            cb;           \
  void              *context;     \
  sd_signal_t       signal;       \
  rex_tcb_type      *tcb_ptr;     \
  sd_ss_act_e_type  act;          \
  sd_ss_act_s_type  *rtrn_act_ptr


/*-----------------------------------------------------------------------------
  sd_cmd_s
    The basic command type.  All commands simply extend this; when appropriate,
    a simple cast between this and the relevant command type is valid.
-----------------------------------------------------------------------------*/
struct sd_cmd_s
{
  SD_CMD_HDR;
};


/*-----------------------------------------------------------------------------
  Command structures for an API

  To create a command for a specific API, start by defining the structure for
  the command.  This is done by starting the structure with SD_CMD_HDR and then
  adding a structure member for each argument (except for "rtrn_act_ptr"), like

  typedef struct
  {
    SD_CMD_HDR;
    int     param1;
    uint16  param2;
  } my_sd_cmd;

  Write the function that will execute the command.  This should be a static
  function that takes a pointer to the command as its only parameter give it
  the name of your API preceeded by an underscore.  This function will do its
  work based on the parameters that were stored in the command.  It should store
  the return value for the API in the command's "act" member.

  static void _my_sd_api(
    sd_cmd_t  *cmd
    )
  {
    if (cmd->param1)
    { ... }
    ...

    cmd->act = <action to take>;
  }

  The API function itself does nothing more than create the command, fill in
  the members of the command with the API parameters and queue the command
  (which will block the thread calling the API).  Once execution resumes, the
  API function will gather the return value from the command, release its
  reference on the command, and return.

  sd_ss_act_e_type my_sd_api(
    int               param1,
    uint16            param2,
    sd_ss_act_s_type  *rtrn_act_ptr
    )
  {
    my_sd_cmd *cmd = (my_sd_cmd *) sdtask_sd_cmd_new(
                                      sizeof(my_sd_cmd),
                                      _my_sd_api,
                                      rtrn_act_ptr,
                                      NULL
                                      );

    cmd->param1 = param1;
    cmd->param2 = param2;

    sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
    
    act = cmd->act;
    ref_cnt_obj_release(cmd);

    return act;
  }    
    
-----------------------------------------------------------------------------*/





#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
  sdtask_sd_cmd_new
    Allocates a command structure of the requested size and initializes the
    structure of the base command.

  Parameters:
    size
      The size of the command structure - just pass sizeof() your command
      structure.
    execute
      Pointer to the function to call in order to execute the command.  Cannot
      be NULL.
    act_ptr
      Pointer to an action structure to fill out as a result of the command
      execution.  May be NULL.
    dtor
      Pointer to the function to call to clean up any resources allocated by
      the specific command (resources used by the framework will be cleaned up
      by the framework).  This will generally be NULL.

  Returns:
    Pointer to a newly allocated and initialized command.  Cast the return
    value to your command structure.

  Note:
    SD commands are reference-counted objects.  DO NOT free the command when
    you are done with it; rather, call ref_cnt_obj_release() on it to release
    your reference.  The command will be cleaned up and freed when its reference
    count goes to zero.
-----------------------------------------------------------------------------*/
sd_cmd_t        *sdtask_sd_cmd_new(
                  size_t            size,
                  execute_fn        execute,
                  sd_ss_act_s_type  *act_ptr,
                  dtor_fn           dtor
                  );

/*-----------------------------------------------------------------------------
  sdtask_queue_put_cmd_wait
    Add a command to the SD command queue and block the calling thread until
    the command has been executed.

  Parameters:
    cmd
      Pointer to the command allocated by a call to sdtask_sd_cmd_new.  Cast
      your command structure to sd_cmd_t *.

  Returns:
    Returns the action to be taken as a result of this command.

  Note:
    This function will ERR_FATAL if there are no more signals available to
    wait on.  There are 31 signals, meaning that there are a maximum of 31
    commands that can be in the queue and blocking a thread.  This should not
    be an issue, as there are not that many tasks that call into SD.
-----------------------------------------------------------------------------*/
sd_ss_act_e_type  sdtask_queue_put_cmd_wait(
                    sd_cmd_t          *cmd
                    );

/*-----------------------------------------------------------------------------
  sdtask_queue_put_cmd_wait
    Add a command to the SD command queue but do not block the calling thread;
    instead, return immediately.

  Parameters:
    cmd
      Pointer to the command allocated by a call to sdtask_sd_cmd_new.  Cast
      your command structure to sd_cmd_t *.
    cb
      A pointer to the function to call when command execution is completed.
    context
      A pointer to a context that is passed as a parameter to the callback
      function.

  Returns:
    Nothing.
-----------------------------------------------------------------------------*/
void            sdtask_queue_put_cmd_no_wait(
                  sd_cmd_t          *cmd,
                  cmd_cb            cb,
                  void              *context
                  );

/*----------------------------------------------------------------------------
  sdtask_init_before_start
    Initialize the SD command queue and signals.
----------------------------------------------------------------------------*/
void sdtask_init_before_start(
  void
  );


/*----------------------------------------------------------------------------
  sdtask_is_init_complete
    Returns whether SD task has completed initialization.
----------------------------------------------------------------------------*/
boolean sdtask_is_init_complete(
  void
  );

/*=========================================================================== 
 
FUNCTION sdtask_mcfg_refresh_cb

DESCRIPTION
  This is a callback function used by MCFG for NV/EFS refresh purpose
  This function puts the received refresh info into SD command queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdtask_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info);

/*===========================================================================
FUNCTION SDTASK_MCFG_REFRESH_REGISTER

DESCRIPTION
This function call registers SD Task with MCFG for NV/EFS refresh purpose
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void sdtask_mcfg_refresh_register(void);


#ifdef __cplusplus
}
#endif

#endif  /* _SDTASK_H_ */

