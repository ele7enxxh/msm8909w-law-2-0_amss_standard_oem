#ifndef PS_RM_TASKI_H
#define PS_RM_TASKI_H
/*===========================================================================

                          P S _ R M _ T A S K I . H

DESCRIPTION
  NAT Data processing Task internal header file, has function, data 
  declarations for internal usage by PS_RM Task.

EXTERNALIZED FUNCTIONS
  ps_rm_default_sig_handler()
    Implicit handler for signals processed by PS_RM Task.

  ps_rm_default_cmd_handler()
    Implicit handler for commands processed by PS_RM Task.

  ps_rm_powerup()
    Cold start called before PS_RM task is started.

  ps_rm_process_cmd()
    Processes command signals in PS_RM Task.

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ps_rm_taski.h#1 $ 
  $DateTime: 2005/12/23 17:19:09

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "rex.h"
#include "queue.h"
#include "ps_rm_defs.h"
#include "ps_crit_sect.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Command queues handled by PS_RM task 
---------------------------------------------------------------------------*/
extern q_type ps_rm_cmd_q;
extern q_type ps_rm_cmd_free_q;

/*--------------------------------------------------------------------------
  The command buffer count.
---------------------------------------------------------------------------*/
#define PS_RM_CMD_BUF_CNT  (50)

/*--------------------------------------------------------------------------
  Datatype to hold signal, command handlers 
---------------------------------------------------------------------------*/
extern ps_rm_sig_handler_info_type ps_rm_sig_handler[PS_RM_MAX_SIGNALS];
extern ps_rm_cmd_handler_type ps_rm_cmd_handler[PS_RM_MAX_DEFINED_CMD_TYPES];

extern ps_rm_cmd_type ps_rm_cmd_buf[ PS_RM_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled for PS_RM main processing loop
---------------------------------------------------------------------------*/
extern rex_sigs_type ps_rm_enabled_sig_mask;

/*--------------------------------------------------------------------------
  PS_RM's own critical section
---------------------------------------------------------------------------*/
extern ps_crit_sect_type global_ps_rm_crit_section;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PS_RM_DEFAULT_SIG_HANDLER()

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean 
ps_rm_default_sig_handler
(
  ps_rm_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*===========================================================================

FUNCTION PS_RM_DEFAULT_CMD_HANDLER()

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
ps_rm_default_cmd_handler
(
  ps_rm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================
FUNCTION PS_RM_POWERUP()

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ps_rm_powerup
(
  void
);

/*===========================================================================

FUNCTION PS_RM_PROCESS_CMD()

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

boolean 
ps_rm_process_cmd
( 
  void
);

#endif /* PS_RM_TASKI_H */
