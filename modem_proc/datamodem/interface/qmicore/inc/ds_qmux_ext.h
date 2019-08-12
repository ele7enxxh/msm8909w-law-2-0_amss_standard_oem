#ifndef _DS_QMUX_EXT_H
#define _DS_QMUX_EXT_H
/*===========================================================================

                         D S _ Q M U X _ E X T . H

DESCRIPTION

  The Data Services Qualcomm MSM Interface control channel message
  multiplexing protocol external header file.

  External entry point into the QMI module.

EXTERNALIZED FUNCTIONS

  qmux_process_rx_sig()
    Called by serving task in response to QMUX_RX_SIG

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmux_ext.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
12/10/12    svj    Support 8 concurrent PDN's 
09/18/11    wc     Nikel and MPSS.NI.1.1 port configuration
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
08/23/06    ks     Moved qmux_init() here as it is now called from ds_task().
                   Moved qmi cmd enum to qmi_defs.h.
07/11/06    ks     Renamed QMI GSDI READ/WRITE cmds.
07/06/06    ks     Added new cmds to process CM events in DS context.
09/08/05    ks     Added cmds for gsdi read/write async cbacks.
06/27/05    jd     Added qmux_process_cmd
05/31/05   jd/ks   Code review updates
05/11/05    ks     changed name from ds_qmux.h to ds_qmux_ext.h
03/08/05    ks     Separated into another file ds_qmux_svc.h
11/21/04    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dcc_task_defs.h"
#include "qmi_modem_task_cmd.h"
#include "qmi_svc_defs.h"
#include "rex.h"
#include "ds_qmi_svc_ext.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*===========================================================================

                                DATA TYPES

===========================================================================*/

#ifdef FEATURE_DATA_QMI
/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMUX_POWERUP_INIT()

  DESCRIPTION
    QMUX power up initialization.

  PARAMETERS
    None

  RETURN VALUE
    TRUE on successful initialization
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_powerup_init
(
  void
);

/*===========================================================================
  FUNCTION QMUX_INIT()

  DESCRIPTION
    Initialize specified QMUX instance.  SIO control IO path will be set up,
    and QMUX instance state will be reset to default.

  PARAMETERS
    qmi_instance : qmi link to initialize
    serving_tcb  : REX tcb pointer for serving task
                   (messages will be processed in this task context)

  RETURN VALUE
    TRUE on successful initialization
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_init
(
  void
);

/*===========================================================================
  FUNCTION QMI_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI cmd.

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
void qmi_process_cmd
(
  qmi_cmd_enum_type        cmd,
  void                   * user_data_ptr
);

/*===========================================================================
  FUNCTION QMUX_REG_VS_SERVICE_LIST()

  DESCRIPTION
    Register service table with QMUX

  PARAMETERS
    qmi_instance         : instance of QMI
    svc_list             : array pointer to all services for qmi instance
    num_services         : total number of services in list that
                           are to be registered

  RETURN VALUE
    TRUE if table registration was successful
    FALSE if table format/content was invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmux_reg_vs_service_list
(
  qmi_instance_e_type  qmi_instance,
  qmux_svc_info_type * svc_list,
  uint8                num_services
);

/*===========================================================================
  FUNCTION QMUX_REG_VS_SERVICE()

  DESCRIPTION
    Register the vendor specific service along with the QMI instance 
    and service configuration.  Function is used to register VS Services 
    ONLY.
   
  PARAMETERS
    qmi_instance : instance of QMI
    service      : service type
    cfg          : configuration parameters for the service instance

  RETURN VALUE
    NULL           -  service registration failed
    service handle -  qmux service handle 

  DEPENDENCIES
    The service list that should contain the services for a specified QMI 
    instance should be registered with QMUX using qmux_reg_service_list().

  SIDE EFFECTS
    None
===========================================================================*/
extern void * qmux_reg_vs_service
(
  qmi_instance_e_type     qmi_instance,
  qmux_service_e_type     service,
  qmux_svc_config_type *  cfg
);

#endif /* FEATURE_DATA_QMI */
#endif /* _DS_QMUX_EXT_H */
