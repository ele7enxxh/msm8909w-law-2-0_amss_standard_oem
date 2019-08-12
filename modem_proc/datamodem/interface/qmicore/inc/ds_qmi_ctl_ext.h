#ifndef DS_QMI_CTL_EXT_H
#define DS_QMI_CTL_EXT_H
/*===========================================================================

                         D S _ Q M I _ C T L _ E X T. H

DESCRIPTION

 The QMI Control Service External header file.
 
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmi_ctl_ext.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/04/10    rt     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "qmi_svc_defs.h"
#include "ds_qmi_svc_ext.h"

/*===========================================================================

                               DATA TYPES

===========================================================================*/
/*===========================================================================
TYPEDEF QMI_CTL_CLIENT_REG_RESULT_TYPE

DESCRIPTION 
  Type for client registration result.
---------------------------------------------------------------------------*/
typedef struct
{
  qmux_service_e_type   service_id; /* QMI Service type           */ 
  qmi_instance_e_type   inst_id;    /* QMI Instance type          */ 
  uint8                 cl_id;      /* Service client ID          */ 
  uint16                tx_id;      /* Transaction ID             */ 
} qmi_ctl_client_reg_result_type;

/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_CTL_CLIENT_REG_RESULT()

DESCRIPTION
  To report if client registration succeeded or failed.

PARAMETERS
  result            : Client ID allocation result.
  client_reg_result : Client reg result.
  qmi_cmd           : reg/dereg.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void qmi_ctl_process_client_registration_result
(
  qmi_result_e_type                 result,
  qmi_ctl_client_reg_result_type *  client_reg_result,
  uint16                            qmi_cmd,
  qmi_error_e_type                  errval
);

#endif /* DS_QMI_CTL_EXT_H */

