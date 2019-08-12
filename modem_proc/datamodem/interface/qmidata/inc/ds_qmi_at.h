#ifndef _DS_QMI_AT_H
#define _DS_QMI_AT_H
/*===========================================================================

                         D S _ Q M I _ A T . H

DESCRIPTION

 The Data Services QMI Atcop Service header file.


Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_at.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/17/10    kk     Removing DCC command buffer dependency from QMI services.
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_QMI_AT
#include "ds_qmi_defs.h"
#include "ds_qmi_if_ext.h"
#include "dsatclient.h"


/*===========================================================================
  FUNCTION QMI_AT_INIT()

  DESCRIPTION
    Register the AT service with QMUX for all applicable QMI links

  PARAMETERS
    QMI Instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_at_init
(
  void
);
#endif /* FEATURE_DATA_QMI_AT */
#endif /* _DS_QMI_AT_H */
