#ifndef PS_RM_SVC_H
#define PS_RM_SVC_H
/*===========================================================================

                        P S _ R M _ S V C . H

DESCRIPTION

  Rm Data processing Task services header file. All definitions, structures,
  and functions needed for performing PS_RM task specific services.
  
EXTERNALIZED FUNCTIONS  
  
  ps_rm_enable_sig()
    Adds specific signal for PS_RM task to process.

  ps_rm_disable_sig()
    Removes specific signal from PS_RM task processing.

  ps_rm_set_sig_handler()
    Sets signal handler for specific signal processed in PS_RM task.

  ps_rm_send_cmd()
    Post a command to be handled in PS_RM task.

  ps_rm_set_cmd_handler()
    Sets command handler for command handled in PS_RM task.

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/ps_rm_svc.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#ifdef __cplusplus
extern "C"
{
#endif

#include "rex.h"
#include "ps_rm_defs.h"

/*===========================================================================

                      EXTERN DECLARATIONS

===========================================================================*/

extern rex_tcb_type           ps_rm_tcb;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PS_RM_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the PS_RMtask uses for rex_wait().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ps_rm_enable_sig
(
  ps_rm_sig_enum_type sig                       /* Signal to be enabled      */
);

/*===========================================================================

FUNCTION PS_RM_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ps_rm_disable_sig
(
  ps_rm_sig_enum_type sig                       /* Signal to be disabled     */
);

/*===========================================================================

FUNCTION PS_RM_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_sig_handler_type 
ps_rm_set_sig_handler
(
  ps_rm_sig_enum_type sig,             /* Signal assocaited with handler     */
  ps_rm_sig_handler_type sig_handler,  /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
);

/*===========================================================================

FUNCTION PS_RM_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS_RM task context.  The cmd is
  processed by calling the registered cmd handler, if any.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ps_rm_send_cmd
(
  ps_rm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================

FUNCTION PS_RM_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  The cmd handling
  can be disabled by setting a NULL handler

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_cmd_handler_type 
ps_rm_set_cmd_handler
(
  ps_rm_cmd_enum_type cmd,             /* Command name                      */
  ps_rm_cmd_handler_type cmd_handler   /* Cmd handler function pointer      */
);


/*===========================================================================

FUNCTION PS_RM_WAIT()

DESCRIPTION
  This function performs the PS_RM Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

===========================================================================*/

rex_sigs_type
ps_rm_wait
(
  rex_sigs_type requested_mask              /* mask of signals to wait for */
);

#ifdef __cplusplus
}
#endif


#endif /* PS_RM_SVC_H */
