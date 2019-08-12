#ifndef _DS_QMI_WDA_H
#define _DS_QMI_WDA_H
/*===========================================================================

                         D S _ Q M I _ W D A . H

DESCRIPTION

 The Data Services QMI Wireless Data Administrative Service header file.

EXTERNALIZED FUNCTIONS

   qmi_wda_init()
     Register the WDA service with QMUX for all applicable QMI links

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_wda.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/02/11    sy     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

/*===========================================================================
  FUNCTION QMI_WDA_INIT()

  DESCRIPTION
    Register the WDA service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_wda_init
(
  void
);

#endif /* _DS_QMI_WDA_H */
