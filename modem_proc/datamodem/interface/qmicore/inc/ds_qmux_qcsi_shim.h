#ifndef DS_QMUX_QCSI_SHIM_H
#define DS_QMUX_QCSI_SHIM_H
/*===========================================================================

                            Q M I _ F W I . H

DESCRIPTION

  The QMI QCSI Shim layer internal interface header file.

Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmux_qcsi_shim.h#1 $ 
   $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/15/11    sj     Created Module
===========================================================================*/
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm.h"

#include "ds_qmi_svc_ext.h"
#include "qmi_framework.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMUX_QCSI_SHIM_INIT()

DESCRIPTION
  This function initializes the QMUX QCSI shim layer. 
  
PARAMETERS
  None
  
RETURN VALUE
  None

DEPENDENCIES
  QMUX is initialized. 

SIDE EFFECTS
  None.
===========================================================================*/
void qmux_qcsi_shim_init
( 
  void
);

/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_PROCESS_CMD()

  DESCRIPTION
    This function processes a shim layer cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          :  QMI command to be processed
    user_data_ptr:  private data buffer containing the shim layer command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    Shim layer module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_qcsi_shim_process_cmd
(
  qmi_cmd_enum_type        cmd,
  void                   * user_data_ptr
);
/*===========================================================================
FUNCTION QMUX_QCSI_SHIM_REG()

DESCRIPTION
  This function registers the QMUX service with QCSI framework. 
  
PARAMETERS
  service       : QMUX service ID.
  
RETURN VALUE
  None

DEPENDENCIES
  ??. 

SIDE EFFECTS
  None.
===========================================================================*/
void qmux_qcsi_shim_reg
( 
  qmux_service_e_type     service
);
/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_PROCESS_CL_ALLOC_RESULT()

  DESCRIPTION
    Handle success or failure for client creation with QMUX service in QMI
    modem task

  PARAMETERS
    service : QMUX service ID
    qmux_client_id : Client ID that was previously identified by QMUX service
    reg_result : SUCCESS or FAILURE

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_qcsi_shim_process_cl_alloc_result
(
  qmux_service_e_type   service,
  uint8                 qmux_client_id, 
  qmi_result_e_type     reg_result
);

/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_SEND_RESP_IND()

  DESCRIPTION
    Sends the response or indication from QMUX framework to QCSI

  PARAMETERS
    common_hdr : Contains QMUX header parameters
    dsm_item : Ptr to response/indication contents
    msg_type : Either QMI_FLAG_MSGTYPE_RESP or QMI_FLAG_MSGTYPE_IND

  RETURN VALUE
    TRUE on SUCCESS, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean qmux_qcsi_shim_send_resp_ind
(
  qmi_framework_common_msg_hdr_type common_hdr, 
  dsm_item_type                     *dsm_item, 
  uint8                              msg_type
);

#endif /* DS_QMUX_QCSI_SHIM_H */

