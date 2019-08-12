#ifndef _DS_QMI_DFS_H
#define _DS_QMI_DFS_H
/*===========================================================================

                         D S _ Q M I _ D S D . H

DESCRIPTION

 The Data Services QMI Data Service Determination header file.

EXTERNALIZED FUNCTIONS

   qmi_DFS_init()
     Initialize the QMI-DFS service and register with QCSI

Copyright (c) 2012 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_dfs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/07/12    bh     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

/*===========================================================================
  FUNCTION QMI_DFS_INIT()

  DESCRIPTION
    Initialize the QMI-DFS service and register with QCSI

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_dfs_init
(
  void
);

#endif /* _DS_QMI_DFS_H */
