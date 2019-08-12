#ifndef SECSSLTASK_H
#define SECSSLTASK_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E C U R E   S O C K E T S   L A Y E R   T A S K
                             
GENERAL DESCRIPTION
  This is the task level file for SSL (Secure Sockets Layer). It defines 
  the timers, signals, queues and other necessary handlings for the SSL
  task.

EXTERNALIZED FUNCTIONS
  secssl_wait() - SSL task wait until a requested signal is set.
  secssl_cmd()  - send a command to SSL task.
  secssl_task() - entry point for the SSL task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  MC must initialize the task and set the start signal for this task, 
  before any other functions can be called.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/secssltask.h_v   1.4   30 Aug 2003 20:13:08   omichael  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/chipset/src/secssltask.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/03   om      Added SECSSL_EVENT_SIG.
02/14/03   om      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "cmd.h"
#include "rex.h"

#include "secerrno.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
extern rex_tcb_type secssl_tcb;

/*--------------------------------------------------------------------------
  Security Service Signals
---------------------------------------------------------------------------*/
/* The next 3 sigs are never used explicitly in the security services code.  
 * They are here as place-holders, so that when adding signals, the user does 
 * not re-defined signals already defined system-wide in task.h.  These 
 * signals are referred by their proper name (i.e., TASK_STOP_SIG) in this 
 * Security services code.
 */

#define SECSSL_START_SIG         0x8000    /* Reserved for TASK_START_SIG  */

#define SECSSL_STOP_SIG          0x4000    /* Reserved for TASK_STOP_SIG   */

#define SECSSL_OFFLINE_SIG       0x2000    /* Reserved for TASK_OFFLINE_SIG*/

/* The following sigs are all okay, and can be allocated freely */
#define SECSSL_DOG_RPT_TIMER_SIG 0x0001    /* signal set when dog expires  */

#define SECSSL_NAGLE_CB_SIG      0x0002    /* signal set when a nagle mode */
                                           /* timer expires                */
#define SECSSL_NV_CMD_SIG        0x0004    /* signal when NV cmd is done   */

#define SECSSL_CMD_Q_SIG         0x0008    /* signal for cmds in SSL queue */

#define SECSSL_OP_COMPLETE_SIG   0x0010    /* signal for cmd completion    */

#define SECSSL_KEYGEN_SIG        0x0020    /* signal for key gener. compl. */

#define SECSSL_DNS_LOOKUP_SIG    0x0040    /* signal from DNS when done    */

#define SECSSL_SOCKET_CB_SIG     0x0080    /* signal set when a socket     */
                                           /* action has completed.        */
#define SECSSL_NETWORK_CB_SIG    0x0100    /* signal set when a network    */
                                           /* action has completed.        */
#define SECSSL_EVENT_SIG         0x0200    /* signal set when a event      */
                                           /* is pending for any session.  */
#define SECSSL_ABORT_SIG         0x0400    /* signal to abort a connect()  */
                                           /* or handshake operation       */
#define SECSSL_LOG_TIMER_SIG     0x0800    /* signal to periodically log   */

/*--------------------------------------------------------------------------
  SSL task command names. 
---------------------------------------------------------------------------*/
typedef enum
{
  SECSSL_CMD_NULL = 0,                  /* For ssltask internal use only!! */
  SECSSL_OPEN_SESSION,                  /* Connect a SSL client session    */
  SECSSL_CLOSE_SESSION,                 /* Close a SSL session             */
  SECSSL_CLOSE_NETWORK,                 /* PPP close for a SSL session     */
  SECSSL_RESUME_SESSION,                /* Resume a SSL session            */
  SECSSL_CONTINUE_SESSION,              /* Continue a suspended SSL session*/
  SECSSL_READ,                          /* Read under the SSL session      */
  SECSSL_WRITE,                         /* Write under the SSL session     */
  SECSSL_ACCEPT_SESSION,                /* A SSL server accept session with a SSL client */
  SECSSL_CMD_MAX                        /* For ssltask internal use only!! */
} secssl_cmd_name_type;

/*--------------------------------------------------------------------------
  SSL task command status mode type. This specifies that
  a command is executed in asynchronized or synchronized mode.
---------------------------------------------------------------------------*/
typedef enum 
{
  SECSSL_EXEC_MODE_SYNC,
  SECSSL_EXEC_MODE_ASYNC
} secssl_cmd_mode_type;

/*--------------------------------------------------------------------------
  SSL task command status enum type. This servers as a 
  semaphore to check if the command has finished processing.
---------------------------------------------------------------------------*/
typedef enum
{
  SECSSL_DONE_S,                        /* General OK return               */
  SECSSL_BUSY_S,                        /* Command is being processed      */
  SECSSL_BADCMD_S,                      /* Invalid command                 */
  SECSSL_BADPARM_S                      /* Bad Parameters                  */
} secssl_cmd_status_type;

/*--------------------------------------------------------------------------
  Command header type.  Each command to SEC task starts with this header. 
---------------------------------------------------------------------------*/
typedef struct
{
  cmd_hdr_type            cmd_hdr;      /* Command header                  */
  secssl_cmd_name_type    cmd;          /* Command name                    */
  secssl_cmd_mode_type    mode;         /* Async vs sync command mode      */
  secssl_cmd_status_type  status;       /* Completion code status          */
  secerrno_enum_type      ret_code;     /* Return code of the SSL command  */
} secssl_cmd_hdr_type;

/*--------------------------------------------------------------------------
  Generic command parameter type provides allocates storage for user
  command parameters rather than pointing to user stack space for the 
  command item (space which disappears before the command is executed
  when the user task has higher priority than SEC).  
---------------------------------------------------------------------------*/
typedef struct 
{
  void * ptr1;      /* Pointer 1 */
  void * ptr2;      /* Pointer 2 */
  void * ptr3;      /* Pointer 3 */
  void * ptr4;      /* Pointer 4 */
  void * ptr5;      /* Pointer 5 */
  uint32 val1;      /* Value 1   */
  uint32 val2;      /* Value 2   */
} secssl_cmd_param_type;

typedef struct
{
  secssl_cmd_hdr_type    hdr;
  secssl_cmd_param_type  param;
} secssl_cmd_type;

/*--------------------------------------------------------------------------
  Other command structure.
---------------------------------------------------------------------------*/

extern q_type secssl_cmd_q;          /* For SSL session related commands.  */


/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/


/*==========================================================================
 FUNCTION SECSSL_WAIT
  
 DESCRIPTION 
   This function performs the SSL task waiting function. This function
   will not return until a signal from the requested mask is set.  To 
   ensure that the watchdog stays happy, this function internally makes
   sure that the watchdog is petted periodically.
   
   Note that if the task just uses rex_wait instead of sec_wait, then
   the task could wait on a signal for a long time, without ever petting
   the watchdog.  The watchdog expects all tasks to report in periodically
   (indicating good software operation); failure for this task to pet the
   watchdog will cause the phone to reset.  So each task must make its own
   specific wait routine, so that it takes care to pet the watchdog
   periodically.
   
 DEPENDENCIES 
   CAN ONLY BE CALLED FROM THE SSL TASK CONTEXT.  IF CALLED FROM 
   ANOTHER TASK, THEN THAT TASK WILL HANG FOREVER...

 RETURN VALUE 
   A signal mask respresenting the signals which are currently set.  At
   least one signal in the requested mask will be set.  Any other signals
   which are set, which may or may not be spec'd in the signal mask, are
   also returned.
  
 SIDE EFFECTS
   None
===========================================================================*/
extern rex_sigs_type secssl_wait
(
  rex_sigs_type requested_mask              /* mask of signals to wait for */
);


/*===========================================================================

FUNCTION SECSSL_GET_CMD_BUF

DESCRIPTION
  This function will get a command buffer from secssl_cmd_free_q.
  
DEPENDENCIES
  The SSL task must have been started up already. Secssl_cmd_free_q has been
  initiated using q_init.

RETURN VALUE
  If there is a free buffer available, a pointer to this buffer is returned.
  Otherwise, a NULL pointer is returned.

SIDE EFFECTS
  None.
===========================================================================*/
extern secssl_cmd_type* secssl_get_cmd_buf
(
  void
);


/*===========================================================================

FUNCTION SECSSL_FREE_CMD_BUF

DESCRIPTION
  This function will free a command buffer from secssl_cmd_free_q.
  
DEPENDENCIES
  The SSL task must have been started up already. Secssl_cmd_free_q has been
  initiated using q_init.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void secssl_free_cmd_buf
(
  secssl_cmd_type     *sec_cmd_ptr
);


/*===========================================================================

FUNCTION SECSSL_CMD

DESCRIPTION
  This function enqueues a command to the SSL task's appropriate 
  command queue based on its command type, and sets a signal to inform 
  the security task. Note that this function runs at the caller's context.

  The function returns to the caller after setting the status to busy 
  indicaton.  This does _not_ mean that the command has been executed yet, 
  just that it is waiting to be executed.

  An example is as follows:

  secssl_cmd_type *cmd_ptr;
  cmd_ptr = sec_get_cmd_buf_else_err_fatal();    -- Get the ssl cmd buffer
  cmd_ptr->hdr.cmd = SECSSL_OPEN_SESSION;        -- Set the command type
  cmd_ptr->param = &ssl_open_param;              -- Set the command param.
  
  secssl_cmd(cmd_ptr);                           -- Enqueue the command.
  

DEPENDENCIES
  The SSL task must have been started up already.  All of the items in
  the secssl_cmd_type must be already set. 

RETURN VALUE
  None directly.  The status variable of the secssl_cmd_type will be
  updated to reflect the current status of this command as it is 
  processed.

SIDE EFFECTS
  The secssl_cmd_type is placed on the SSL command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

extern void secssl_cmd
(
  secssl_cmd_type*      cmd_ptr
);


/*===========================================================================

FUNCTION SECSSL_TASK

DESCRIPTION
  This function is the entry point and main processing loop for the
  SSL task.  This function contains the infinite while loop.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void secssl_task
( 
  dword dummy                            /* unused dummy var               */
);


#endif /* FEATURE_SEC_SSL */

#endif /* SECSSLTASK_H */
