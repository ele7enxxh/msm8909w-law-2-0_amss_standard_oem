#ifndef _DS_QMI_ATP_H
#define _DS_QMI_ATP_H
/*===========================================================================

                         D S _ Q M I _ A T P . H

DESCRIPTION

 The Data Services QMI Application Traffic Pairing header file.

EXTERNALIZED FUNCTIONS

   qmi_atp_init()
     Initialize the QMI-ATP service and register with QCSI

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_atp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/13/13    sj     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

/*===========================================================================
  FUNCTION QMI_ATP_INIT()

  DESCRIPTION
    Initialize the QMI-ATP service and register with QCSI

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_atp_init
(
  void
);

#endif /* _DS_QMI_ATP_H */
