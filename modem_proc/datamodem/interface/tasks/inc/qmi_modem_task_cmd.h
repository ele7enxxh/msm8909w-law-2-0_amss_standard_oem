#ifndef QMI_MODEM_TASK_CMD_H
#define QMI_MODEM_TASK_CMD_H
/*===========================================================================

                        Q M I _ M O D E M _ T A S K _ C M D . H

DESCRIPTION

  QMI Modem Task header file. All definitions, structures, and functions
  needed for performing QMI Modem task to a specific services.
 
EXTERNALIZED DATA 
  qmi_cmd_enum_type
    Supported commands for QMI_MODEM task.
 
  qmi_cmd_handler_type
    Callback prototype for command handler.
 
  qmii_cmd_type
    QMI_MODEM Task Command queues and Command Data Buffers
 
  qmi_cmd_type
    Struct used for all QMI_MODEM Task commands
 
  qmi_cmd_data_buf_type
    QMI_MODEM Task command user data type    
 
EXTERNALIZED FUNCTIONS 
  qmi_get_cmd_data_buf()
    Allocates a new command buffer to be used by caller to post commands to
    QMI_MODEM task.
 
  qmi_free_cmd_data_buf() 
    Deallocates the command buffer. 
 
  qmi_send_cmd()
    Post a command to QMI_MODEM task
 
  qmi_set_cmd_handler()
    Set a handler to process a cmd in QMI_MODEM task context
 
Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/qmi_modem_task_cmd.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
01/25/11    rk     Created module.
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
#include "queue.h"
#include "task.h"
#include "nv.h"
#include "sio.h"
#include "rdevmap.h"
#include "qmi_modem_task_svc.h"
#include "qmi_svc_defs.h"
#include "ds_qmi_ctl_ext.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
  QMI NV item and status.
===========================================================================*/
extern nv_item_type         qmi_nv_item;
extern nv_stat_enum_type    qmi_nv_status;

/*===========================================================================
TYPEDEF QMI_CMD_ENUM_TYPE
 
DESCRIPTION 
  This enum lists all the commands that can be sent to QMI_MODEM task for 
  processing. 
===========================================================================*/
typedef enum
{
  QMI_MIN_CMD                  = -1,
  QMI_CMD                      = 1,

#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  QMI_QMUX_QCSI_SHIM_CMD,

  /* Always must be last */
  QMI_MAX_DEFINED_CMD_TYPES
} qmi_cmd_enum_type;

/*===========================================================================
TYPEDEF QMI_CMD_HANDLER_TYPE
 
DESCRIPTION 
  Each command that is listed in qmi_cmd_enum_type should have a command
  handler specified by the caller. The function qmi_set_cmd_handler() 
  should be used for setting the handler function. The following typedef
  lists the prototype of the handler function.
===========================================================================*/
typedef void (*qmi_cmd_handler_type)
(
  qmi_cmd_enum_type cmd,           /* Actual command to be processed       */
  void *            user_data_ptr  /* Command specific user parameters     */
);

/*===========================================================================
TYPEDEF QMII_CMD_TYPE
 
DESCRIPTION 
  QMI Task Command queues and Command Data Buffers 
===========================================================================*/
typedef struct     
{
  q_link_type       link;	           /* For linking onto cmd queue          */
  qmi_cmd_enum_type cmd;             /* Actual command to be processed      */
  void *            user_data_ptr;   /* Command specific user parameters    */
} qmii_cmd_type;

/*===========================================================================
TYPEDEF QMI_CMD_TYPE
 
DESCRIPTION 
  Struct used for all QMI commands
===========================================================================*/
typedef struct
{
  uint16  id;
  union
  {
    struct
    {
      void *   user_data_ptr;
    }fw;
    struct
    {
      void *  sp;
    } ctl;
    struct
    {
      void*     state_handle;
      void*     data;
    } qmi_svc_pwrsave;
    struct
    {
      sio_port_id_type       sio_port;
      void*     data;
    } qmux_port;
    struct
    {
      rdm_device_enum_type   device;
      rdm_assign_status_type status;
    } qmux_assign;
    struct
    {
      uint8             clid;
      void *            svc_handle;
      dsm_item_type *   sdu_in;
    }legacy_svc_resp;

    struct
    {
      qmi_result_e_type    result;
      qmi_error_e_type     errval;
      qmi_ctl_client_reg_result_type  cl_reg_info;
    } fw_cl_resp;

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
    struct
    {
      qmux_service_e_type   service_id; /* QMI Service type*/ 
      qmi_instance_e_type   qmi_inst;
      boolean               is_legacy_svc;
    } ctl_avail_ind;
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

    struct
    {
      boolean            result;
      int32              ep_id;
    } rmnet_inst;

    struct
    {
      qmi_instance_e_type qmi_inst;
      boolean             asserted;
    } dtr_status;
  } data;
} qmi_cmd_type;

/*===========================================================================
TYPEDEF QMI_CMD_TYPE
 
DESCRIPTION 
  QMI command user data type
===========================================================================*/
typedef struct
{
  union
  {
    qmi_cmd_type    qmi;
    void *          user_data_ptr;
  }cmd;
} qmi_cmd_data_buf_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_GET_CMD_DATA_BUF()

DESCRIPTION
  Allocates a new command buffer to be used by caller to post commands to
  QMI_MODEM task.
 
PARAMETERS 
  None.
    
DEPENDENCIES
  None.

RETURN VALUE
  Ptr to new command data buffer.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_cmd_data_buf_type * qmi_get_cmd_data_buf
(
  void
);

/*===========================================================================
FUNCTION QMI_FREE_CMD_DATA_BUF()

DESCRIPTION
  Frees QMI_MODEM task command buffer.
 
PARAMETERS 
  None.
 
DEPENDENCIES
  Command buffer should have been allocated by a prior call to
  qmi_get_cmd_data_buf().

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_free_cmd_data_buf
(
  void *cmd_ptr                  /* Command buffer to be freed             */
);

/*===========================================================================
FUNCTION QMI_SEND_CMD()
  Send command to be processed in QMI_MODEM task.
 
DESCRIPTION
  This method is used to send a command to the QMI_MODEM task for processing. 
  The command handler registered with the command being sent is executed
  when the QMI_MODEM task is run the next time. The QMI_MODEM task is serialized 
  through a command Q, so any pending commands in the QMI_MODEM task would be
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
void qmi_send_cmd
(
  qmi_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================
FUNCTION QMI_SET_CMD_HANDLER() 
 
DESCRIPTION
  This function sets a handler for the specified cmd type.  If a NULL cmd
  handler is passed, then the command handling is defaulted (see 
  qmi_default_cmd_handler). The command handler can be updated on the 
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
qmi_cmd_handler_type qmi_set_cmd_handler
(
  qmi_cmd_enum_type cmd,             /* Command name                      */
  qmi_cmd_handler_type cmd_handler   /* Cmd handler function pointer      */
);

#ifdef FEATURE_REX_SIGS_EXT
/*===========================================================================
FUNCTION QMI_MODEM_TASK_WAIT()

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
rex_sigs_type * qmi_modem_task_wait
(
  rex_sigs_type *requested_mask             /* mask of signals to wait for */
);

#else
/*===========================================================================
FUNCTION QMI_MODEM_TASK_WAIT()

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
rex_sigs_type qmi_modem_task_wait
(
  rex_sigs_type requested_mask              /* mask of signals to wait for */
);

#endif /* FEATURE_REX_SIGS_EXT */

/*===========================================================================
FUNCTION QMI_GET_NV_ITEM

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
nv_stat_enum_type qmi_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
);

/*===========================================================================
FUNCTION QMI_PUT_NV_ITEM

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
nv_stat_enum_type qmi_put_nv_item
(
  nv_items_enum_type item_code,                /* which item               */
  nv_item_type *data_ptr                       /* pointer to data for item */
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* QMI_MODEM_TASK_CMD_H */
