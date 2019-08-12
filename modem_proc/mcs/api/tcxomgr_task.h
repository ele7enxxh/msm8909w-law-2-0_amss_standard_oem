#ifndef TCXOMGR_TASK_H
#define TCXOMGR_TASK_H

/*===========================================================================

                   TCXO Manager Task Public Header File

DESCRIPTION
  This file contains all the exported definitions for the TCXO Manager task.

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/tcxomgr_task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/09   ag      Initial release.

===========================================================================*/

#include "comdef.h"
#include "rex.h"


/******************************************************************************
                 Signals used by TCXO Manager task
******************************************************************************/


/******************************************************************************
                 TCXO Manager Task prototypes.
******************************************************************************/

/*===========================================================================

FUNCTION TCXOMGR_SET_SUBSIGS

DESCRIPTION    This function sets the signals for TCXO Manager task.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void tcxomgr_set_sigs ( rex_sigs_type sigs );


/*===========================================================================

FUNCTION TCXOMGR_TASK

DESCRIPTION
  The TCXO Manager task main function. It performs task
  initialization then continuously handles TCXO Manager signals.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/

extern void tcxomgr_task (dword dummy );

#endif /* TCXOMGR_TASK_H */
