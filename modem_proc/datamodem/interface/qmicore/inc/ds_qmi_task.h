#ifndef _DS_QMI_TASK_H
#define _DS_QMI_TASK_H
/*===========================================================================

                         D S _ Q M I _ T A S K . H

DESCRIPTION

 The Data Services Qualcomm MSM Interface Task header file.

EXTERNALIZED FUNCTIONS

  qmi_task_set_qos_cmd_handler
    Specify the callback function that should be invoked when we get a QMI CMD
    for QoS service to handle

Copyright (c) 2007-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmi_task.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/17/10    kk     Removing DCC command buffer dependency from QMI services.
11/21/04    ks     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ds_qmux_ext.h"

/*---------------------------------------------------------------------------
  callback function to be invoked when processing QMI cmd
---------------------------------------------------------------------------*/
typedef void (* qmi_task_svc_cmd_hdlr_cb_f_type)
(
  void * cmd_ptr
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION QMI_TASK_SET_SVC_CMD_HANDLER()

  DESCRIPTION
    Specify the callback function that should be invoked when we get a QMI CMD
    for a given service

  PARAMETERS
    svc      : QMI service type for which the callback is being set
    cmd_hdlr : Command handler to invoke when a command is received

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_task_set_svc_cmd_handler
(
  qmux_service_e_type              svc,
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr_cb
);
/*===========================================================================
  FUNCTION QMI_PROCESS_INIT_LEGACY_SERVICES()

  DESCRIPTION
    This function initialize the QMI legacy services running on DCC task.

  PARAMETERS
    cmd          :  QMI command to be processed
    user_data_ptr:  private data buffer containing the QMI command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_process_init_legacy_services
(
  dcc_cmd_enum_type cmd,        /* Actual command to be processed         */
  void *user_data_ptr
);

/*===========================================================================
  FUNCTION QMI_PROCESS_LEGACY_CMD()

  DESCRIPTION
    This function processes a QMI cmd running on legacy services in DCC task.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          :  QMI command to be processed
    user_data_ptr:  private data buffer containing the QMI command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_process_legacy_cmd
(
  dcc_cmd_enum_type        cmd,
  void *                   user_data_ptr
);

/*===========================================================================
  FUNCTION QMI_TASK_SET_COMMON_CMD_HANDLER()

  DESCRIPTION
    Specify the callback function that should be invoked when we get a QMI CMD

  PARAMETERS
    cmd_hdlr : Command handler to invoke when a command is received

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_task_set_common_cmd_handler
(
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr_cb
);

/*===========================================================================
  FUNCTION QMI_TASK_SET_QMUX_CMD_HANDLER()

  DESCRIPTION
    Specify the callback function that should be invoked when we get a
    QMUX CMD

  PARAMETERS
    cmd_hdlr : Command handler to invoke when a command is received

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_task_set_qmux_cmd_handler
(
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr_cb
);

/*===========================================================================
  FUNCTION QMI_TASK_SET_QMAP_CMD_HANDLER

  DESCRIPTION
    Specify the callback function that should be invoked when we get a
    QMAP CMD

  PARAMETERS
    cmd_hdlr : Command handler to invoke when a command is received

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_task_set_qmap_cmd_handler
(
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr_cb
);

#endif /* _DS_QMI_TASK_H */
