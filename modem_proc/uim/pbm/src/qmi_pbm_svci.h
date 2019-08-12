#ifndef _QMI_PBM_SVCI_H
#define _QMI_PBM_SVCI_H
/*===========================================================================

                         Q M I _ M M C P _ S V C I . H

DESCRIPTION

 The Data Services QMI Definitions header file. 
 Contains definitions common to all QMI services.


Copyright (c) 2004-2011QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

     $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_svci.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/26/11    sk     Initial version
===========================================================================*/

#include "comdef.h"
#include "customer.h"


/*===========================================================================
FUNCTION QMI_PBM_SVC_INIT()

DESCRIPTION
  This function initializes various qmi services in qmi pbm task.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void qmi_pbm_svc_init
(
  void
);


#endif /* _QMI_PBM_SVCI_H */
