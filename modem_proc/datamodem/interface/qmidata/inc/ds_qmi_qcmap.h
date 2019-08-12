#ifndef _DS_QMI_QCMAP_H
#define _DS_QMI_QCMAP_H
/*===========================================================================

                         D S _ Q M I _ Q C M A P . H

DESCRIPTION

 The Data Services QMI Qualcomm Mobile Access Point header file.

EXTERNALIZED FUNCTIONS

   qmi_qcmap_init()
     Register the QCMAP service with QMUX for all applicable QMI links

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_qcmap.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/01/12    am     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

/*===========================================================================
  FUNCTION QMI_QCMAP_INIT()

  DESCRIPTION
    Register the QCMAP service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_qcmap_init
(
  void
);

#endif /* _DS_QMI_QCMAP_H */
