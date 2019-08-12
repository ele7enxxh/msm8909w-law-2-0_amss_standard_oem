#ifndef _DS_QMI_AUTH_H
#define _DS_QMI_AUTH_H
/*===========================================================================

                         D S _ Q M I _ A U T H . H

DESCRIPTION

 The Data Services QMI Device Management Service header file.

EXTERNALIZED FUNCTIONS

   qmi_dms_init()
     Register the DMS service with QMUX for all applicable QMI links

Copyright (c) 2005-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_auth.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/17/10    kk     Removing DCC command buffer dependency from QMI services.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
09/15/08    ks     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_QMI_EAP
#include "ds_qmi_svc_ext.h"
#include "dcc_task_defs.h"


/*===========================================================================
  FUNCTION QMI_AUTH_INIT()

  DESCRIPTION
    Register the AUTH service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_auth_init
(
  void
);
#endif /* FEATURE_DATA_QMI_EAP */
#endif /* _DS_QMI_AUTH_H */
