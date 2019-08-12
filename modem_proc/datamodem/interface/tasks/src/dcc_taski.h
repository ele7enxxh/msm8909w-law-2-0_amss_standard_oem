#ifndef DCC_TASKI_H
#define DCC_TASKI_H
/*===========================================================================

                          D C C _ T A S K I . H

DESCRIPTION
  Data Common Control Task internal header file, has function, data 
  declarations for internal usage by DCC Task.

EXTERNALIZED FUNCTIONS
  dcc_default_sig_handler()
    Implicit handler for signals processed by DCC Task.

  dcc_default_cmd_handler()
    Implicit handler for commands processed by DCC Task.

  dcci_powerup()
    Cold start called before DCC task is started.

  dcci_process_cmd()
    Processes command signals in DCC Task.

Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/dcc_taski.h#1 $ 
  $DateTime: 2005/12/23 17:19:09

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/18/14    skc    Removing FEATURE_UW_FMC
10/09/12    sj     Increase DCCI_CMD_BUF_CNT for 8 PDN support
06/24/11    scb    Port changes for FMC into DCC task
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
09/27/10    ss     DYNAMIC WDOG support for DATA tasks.
07/06/09    am     extern-ed global_dcc_crit_section.
02/19/09    am     Created module.
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ds_Utils_DebugMsg.h"


#include "rex.h"
#include "queue.h"
#include "dcc_task_defs.h"
#include "ps_crit_sect.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Dog data structures [extern defs] for DCC task
---------------------------------------------------------------------------*/
extern dog_report_type dcc_dog_rpt_id;                   /*! dog report ID */

/*--------------------------------------------------------------------------
  Command queues handled by DCC task 
---------------------------------------------------------------------------*/
extern q_type dcci_cmd_q;
extern q_type dcci_cmd_free_q;

/*--------------------------------------------------------------------------
  The command buffer count.
  Count = (20) for Apps proc as mode handlers interfacing with DCC are less 
  than those on Modem.
---------------------------------------------------------------------------*/

#define DCCI_CMD_BUF_CNT  (600)


/*--------------------------------------------------------------------------
  Datatype to hold signal, command handlers 
---------------------------------------------------------------------------*/
extern dcci_sig_handler_info_type dcci_sig_handler[DCC_MAX_SIGNALS];
extern dcc_cmd_handler_type dcci_cmd_handler[DCC_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled for DCC main processing loop
---------------------------------------------------------------------------*/
extern rex_sigs_type dcci_enabled_sig_mask;

/*--------------------------------------------------------------------------
  DCC's own critical section
---------------------------------------------------------------------------*/
extern ps_crit_sect_type global_dcc_crit_section;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DCC_DEFAULT_SIG_HANDLER()

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

boolean 
dcc_default_sig_handler
(
  dcc_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*===========================================================================

FUNCTION DCC_DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the
  commands which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
dcc_default_cmd_handler
(
  dcc_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================
FUNCTION DCCI_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the DCC task. 
  
  This function performs initialization before DCC task is started.
  Note that initializations internal to DCC should be performed before
  initializing other components that run in DCC task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
dcci_powerup
(
  void
);

/*===========================================================================

FUNCTION DCCI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to DCC_CMD_Q_SIGNAL.  It retrieves the
  next command from the DCC command queue and processes the command by calling 
  the registered cmd handler.
  
DEPENDENCIES
    An item must be on the DCC command queue or an LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function 
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean 
dcci_process_cmd
( 
  void
);


#endif /* DCC_TASKI_H */
