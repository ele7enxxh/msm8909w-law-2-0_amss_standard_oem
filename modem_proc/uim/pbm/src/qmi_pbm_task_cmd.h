#ifndef QMI_PBM_TASK_CMD_H
#define QMI_PBM_TASK_CMD_H
/*===========================================================================

                        Q M I _ M M C P _ T A S K _ C M D . H

DESCRIPTION

  QMI Mmcp Task header file. All definitions, structures, and functions
  needed for performing QMI Modem task to a specific services.

EXTERNALIZED DATA
  qmi_pbm_cmd_enum_type
    Supported commands for QMI_PBM task.

  qmi_pbm_cmd_handler_type
    Callback prototype for command handler.

  qmi_pbmi_cmd_type
    QMI_PBM Task Command queues and Command Data Buffers

  qmi_pbm_cmd_type
    Struct used for all QMI_PBM Task commands

  qmi_pbm_cmd_data_buf_type
    QMI_PBM Task command user data type

EXTERNALIZED FUNCTIONS
  qmi_pbm_get_cmd_data_buf()
    Allocates a new command buffer to be used by caller to post commands to
    QMI_PBM task.

  qmi_pbm_free_cmd_data_buf()
    Deallocates the command buffer.

  qmi_pbm_send_cmd()
    Post a command to QMI_PBM task

  qmi_pbm_set_cmd_handler()
    Set a handler to process a cmd in QMI_PBM task context

Copyright (c) 2011QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_task_cmd.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */




#include "rex.h"
#include "queue.h"
#include "task.h"
#include "nv.h"
#include "sio.h"
#include "rdevmap.h"
#include "qmi_pbm_task_svc.h"
#include "dsm.h"
#include "qmi_framework.h"
#include "qmi_pbm_if.h"
#include "pbmutils.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
  QMI NV item and status.
===========================================================================*/
extern nv_item_type         qmi_pbm_nv_item;
extern nv_stat_enum_type    qmi_pbm_nv_status;

/*===========================================================================
TYPEDEF QMI_PBM_CMD_ENUM_TYPE

DESCRIPTION
  This enum lists all the commands that can be sent to QMI_PBM task for
  processing.
===========================================================================*/
typedef enum
{
  QMI_PBM_CMD_MIN_CMD= -1,

  /* Voice Service cmds */
  QMI_PBM_CMD_PBM_FW_CB= 1,
  QMI_PBM_CMD_PBM_IF_CB,
  QMI_PBM_CMD_PBM_EVT_CB,
  QMI_PBM_CMD_PBM_CB,
  QMI_PBM_CMD_PBM_MAX,
  /* Always must be last */
  QMI_PBM_MAX_DEFINED_CMD_TYPES
} qmi_pbm_cmd_enum_type;

typedef enum
{
  QMI_PBM_MIN,

  QMI_PBM_FW_INIT_CB,
  QMI_PBM_FW_ALLOC_CLID_CB,
  QMI_PBM_FW_DEALLOC_CLID_CB,
  QMI_PBM_FW_REQ_CB,

  QMI_PBM_MAX,

}qmi_pbm_cmd_id_type;

/*===========================================================================
TYPEDEF QMI_PBM_CMD_HANDLER_TYPE

DESCRIPTION
  Each command that is listed in qmi_pbm_cmd_enum_type should have a command
  handler specified by the caller. The function qmi_pbm_set_cmd_handler()
  should be used for setting the handler function. The following typedef
  lists the prototype of the handler function.
===========================================================================*/
typedef void (*qmi_pbm_cmd_handler_type)
(
  void *            user_data_ptr  /* Command specific user parameters     */
);

/*===========================================================================
TYPEDEF QMI_PBMI_CMD_TYPE

DESCRIPTION
  QMI Task Command queues and Command Data Buffers
===========================================================================*/
typedef struct
{
  q_link_type       link;	           /* For linking onto cmd queue          */
  qmi_pbm_cmd_enum_type cmd;             /* Actual command to be processed      */
  void *            user_data_ptr;   /* Command specific user parameters    */
} qmi_pbmi_cmd_type;

/*===========================================================================
TYPEDEF QMI_PBM_CMD_TYPE

DESCRIPTION
  Struct used for all QMI commands
===========================================================================*/
typedef struct
{
  uint16  id;
  union
  {
    union
    {
      struct
      {
        uint16 num_instances;
      }init_cb;
      struct
      {
        qmi_framework_common_msg_hdr_type msg_hdr;
      }alloc_clid;
      struct
      {
        qmi_framework_common_msg_hdr_type msg_hdr;
      }dealloc_clid;
      struct
      {
        qmi_framework_msg_hdr_type msg_hdr;
        dsm_item_type             *sdu_in;
      }req_cb;

    }qmi_fw_info;

  } data;
} qmi_pbm_common_cmd_type;

/*===========================================================================
TYPEDEF QMI_PBM_CMD_TYPE

DESCRIPTION
  QMI command user data type
===========================================================================*/
typedef struct
{
  union
  {
    qmi_pbm_common_cmd_type   *qmi_ptr;
    qmi_pbm_if_info_item_s *pbm_if_data_ptr;
    void *          user_data_ptr;
  }cmd;
} qmi_pbm_cmd_data_buf_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_PBM_GET_CMD_DATA_BUF()

DESCRIPTION
  Allocates a new command buffer to be used by caller to post commands to
  QMI_PBM task.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Ptr to new command data buffer.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_pbm_cmd_data_buf_type * qmi_pbm_get_cmd_data_buf
(
  void
);

/*===========================================================================
FUNCTION QMI_PBM_FREE_CMD_DATA_BUF()

DESCRIPTION
  Frees QMI_PBM task command buffer.

PARAMETERS
  None.

DEPENDENCIES
  Command buffer should have been allocated by a prior call to
  qmi_pbm_get_cmd_data_buf().

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbm_free_cmd_data_buf
(
  void *cmd_ptr                  /* Command buffer to be freed             */
);

/*===========================================================================
FUNCTION QMI_PBM_SEND_CMD()
  Send command to be processed in QMI_PBM task.

DESCRIPTION
  This method is used to send a command to the QMI_PBM task for processing.
  The command handler registered with the command being sent is executed
  when the QMI_PBM task is run the next time. The QMI_PBM task is serialized
  through a command Q, so any pending commands in the QMI_PBM task would be
  executed before the posted command.

PARAMETERS
  cmd           - Command to send to QMI MODEM task.
  user_data_ptr - User data sent with the command. The
                  command handler would be called with this user data.
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void qmi_pbm_send_cmd
(
  qmi_pbm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

void qmi_pbm_if_process_cmd
(
  void                   * user_data_ptr
);



/*===========================================================================
FUNCTION QMI_PBM_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  If a NULL cmd
  handler is passed, then the command handling is defaulted (see
  qmi_pbm_default_cmd_handler). The command handler can be updated on the
  fly by calling this funciton again.

PARAMETERS
  cmd         - Command for which we want to set handler.
  cmd_handler - Callback function to be registered as handler.

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.
===========================================================================*/
qmi_pbm_cmd_handler_type qmi_pbm_set_cmd_handler
(
  qmi_pbm_cmd_enum_type cmd,             /* Command name                      */
  qmi_pbm_cmd_handler_type cmd_handler   /* Cmd handler function pointer      */
);

#ifdef FEATURE_REX_SIGS_EXT
/*===========================================================================
FUNCTION QMI_PBM_TASK_WAIT()

DESCRIPTION
  This function performs the QMI modem Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

PARAMETERS
  requested_mask : QMI modem task sig mask.

DEPENDENCIES
  None.

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared.
===========================================================================*/
rex_sigs_type * qmi_pbm_task_wait
(
  rex_sigs_type *requested_mask             /* mask of signals to wait for */
);

#else
/*===========================================================================
FUNCTION QMI_PBM_TASK_WAIT()

DESCRIPTION
  This function performs the QMI Modem Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

PARAMETERS
  requested_mask : QMI Modem Task SIG mask.

DEPENDENCIES
  None.

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared.
===========================================================================*/
rex_sigs_type qmi_pbm_task_wait
(
  rex_sigs_type requested_mask              /* mask of signals to wait for */
);

#endif /* FEATURE_REX_SIGS_EXT */

/*===========================================================================
FUNCTION QMI_PBM_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

PARAMETERS
  item_code : Item to get.
  data_ptr  : Pointer where to put the item.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

SIDE EFFECTS
  While this function is running all other QMI Modem task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.
===========================================================================*/
nv_stat_enum_type qmi_pbm_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
);

/*===========================================================================
FUNCTION QMI_PBM_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

PARAMETERS
  item_code : Item to set.
  data_ptr  : Pointer to data for item.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

DEPENDENCIES
  This function can only be called from QMI modem task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

SIDE EFFECTS
  While this function is running all other QMI modem task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.
===========================================================================*/
nv_stat_enum_type qmi_pbm_put_nv_item
(
  nv_items_enum_type item_code,                /* which item               */
  nv_item_type *data_ptr                       /* pointer to data for item */
);



#endif /* QMI_PBM_TASK_CMD_H */
