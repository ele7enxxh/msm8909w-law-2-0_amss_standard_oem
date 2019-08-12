#ifndef PSI_CMD_H
#define PSI_CMD_H
/*===========================================================================

                           P S I _ C M D . H

DESCRIPTION

  Protocol Task local header file. All definitions, structures, and functions
  needed internal to protocol task services.

EXTERNALIZED FUNCTIONS

  psi_process_cmd()
    Procsses a PS task cmd and calls appropriate handler function

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/psi_cmd.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/10   sy     Initial version of file


===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ds_Utils_DebugMsg.h"

#include "ps_svc.h"
#include "queue.h"
#include "dss_config.h"


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/


/*--------------------------------------------------------------------------
  Data types to hold cmd handlers
---------------------------------------------------------------------------*/
extern ps_cmd_handler_type psi_cmd_handler[PS_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  PS Task Command queues and Command Data Buffers
---------------------------------------------------------------------------*/

typedef struct
{
  q_link_type link;		           /* For linking onto cmd queue             */
  ps_cmd_enum_type cmd;          /* Actual command to be processed         */
  void *user_data_ptr;           /* Command specific user parameters       */
} psi_cmd_type;

#define PSI_CMD_BUF_CNT  (500)

extern psi_cmd_type psi_cmd_buf[ PSI_CMD_BUF_CNT];

extern q_type psi_cmd_q;
extern q_type psi_cmd_free_q;



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PSI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to PS_CMD_Q_SIGNAL.  It retrieves the
  next command from the PS command queue and processes the command by calling
  the registered cmd handler.

DEPENDENCIES
    An item must be on the PS command queue or an LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean psi_process_cmd
(
  void
);


/*===========================================================================

FUNCTION DEFAULT_CMD_HANDLER()

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
extern void default_cmd_handler
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);


#endif /* PSI_CMD_H */

