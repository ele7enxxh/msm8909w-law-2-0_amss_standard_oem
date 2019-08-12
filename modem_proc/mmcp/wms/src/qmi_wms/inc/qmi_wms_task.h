#ifndef QMI_WMS_TASK_H
#define QMI_WMS_TASK_H
/*===========================================================================

                          Q M I _ W M S _ T A S K . H

DESCRIPTION
  This is the header file for the QMI WMS Task. Contained herein are the
  functions needed to initialize all the modules that execute in QMI WMS
  task context and the main task processing loop.

EXTERNALIZED FUNCTIONS
  qmi_wms_task()
    QMI Task entry point and main processing loop.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/inc/qmi_wms_task.h#1 $
  $DateTime: 2005/12/23 17:19:09

when        who    what, where, why
--------    ---    ----------------------------------------------------------
25/01/2011  sk     Created module.
===========================================================================*/

#include "comdef.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_WMS_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the
  QMI WMS task.

PARAMETERS
  dummy : Ignored.

DEPENDENCIES
  None.

RETURN VALUE
  Does not Return.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_task
(
  uint32 dummy
);

#endif /* QMI_WMS_TASK_H */

