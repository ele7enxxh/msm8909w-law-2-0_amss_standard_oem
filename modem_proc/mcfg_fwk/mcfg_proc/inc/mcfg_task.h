#ifndef MCFG_TASK_H
#define MCFG_TASK_H
/*==============================================================================

                M O D E M   C O N F I G   T A S K

GENERAL DESCRIPTION
  Task-related functionality exposed to other MCFG modules.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/inc/mcfg_task.h#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who   what, where, why
-------- ---   ----------------------------------------------------------------
09/07/12 bd    Created file

==============================================================================*/

#include "queue.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

typedef enum mcfg_task_cmd_id_e {
  MCFG_TASK_CMD_MIN = 0,

  MCFG_TASK_CMD_UIM = 1,
  MCFG_TASK_CMD_UTILS,
  MCFG_TASK_CMD_REFRESH,

  MCFG_TASK_CMD_MAX
} mcfg_task_cmd_id_e_type;

typedef struct mcfg_task_cmd_s {
  q_link_type             link;
  mcfg_task_cmd_id_e_type cmd_id;
  void                   *data;
} mcfg_task_cmd_s_type;

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_task_cmd_alloc

  DESCRIPTION
    Allocates and initializes a command structure. The pointer returned
    by this function (if not NULL) must be either passed to
    mcfg_task_cmd_send() to have the command processed from the context of
    the MCFG task, or passed to mcfg_task_cmd_free() to abort the command.

  DEPENDENCIES
    None

  PARAMETERS
    cmd_id    [in]
    data_size [in] Size of data to allocate in the cmd->data structure. Can
    be 0. The resulting cmd->data buffer will be zero-initialized.

  RETURN VALUE
    Pointer to command structure, or NULL on failure

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_task_cmd_s_type *mcfg_task_cmd_alloc
(
  mcfg_task_cmd_id_e_type cmd_id,
  uint32                  data_size
);

/*===========================================================================

  FUNCTION mcfg_task_cmd_free

  DESCRIPTION
    Frees memory associated with a command allocated by mcfg_task_cmd_alloc().
    This is only expected to be called from outside mcfg_task in the event
    of an unexpected error. It MUST NOT be called during regular command
    processing, as the mcfg_task layer calls it internally.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_task_cmd_free
(
  mcfg_task_cmd_s_type *cmd
);

/*===========================================================================

  FUNCTION mcfg_task_cmd_send

  DESCRIPTION
    Posts a command to the MCFG task for processing.

  DEPENDENCIES
    The command structure must have been allocated and initialized via
    mcfg_task_cmd_alloc().

  PARAMETERS
    None

  RETURN VALUE
    TRUE on success, FALSE on failure

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_task_cmd_send
(
  mcfg_task_cmd_s_type *cmd
);

/*===========================================================================

  FUNCTION mcfg_task_bootup_completed

  DESCRIPTION
    This function check if MCFG task is fully booted up or not

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    TRUE if MCFG task is fully booted up

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_task_bootup_completed
(
  void
);

#endif /* MCFG_TASK_H */
