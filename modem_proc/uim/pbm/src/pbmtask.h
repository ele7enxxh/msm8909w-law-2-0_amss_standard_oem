#ifndef PBMTASK_H
#define PBMTASK_H

/*===========================================================================

                           PHONEBOOK MANAGER TASK

DESCRIPTION
  Header file for Phonebook Manager Task.

  Copyright (c) 2002 - 2008 byQUALCOMM Technologies, Inc(QTI)  
  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmtask.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/23/11   PK      Adding the PBM init function into pbmtask.h 
08/28/08   sg      Featurize MMGSDI seek/search calls based on FEATURE_MMGSDI_ENHANCED_SEARCH
07/10/08   sg      Added PBM_GSDI_SEEK_CB to handle SEEK commands
05/08/08   clm     Queue up pbm notify requests to avoid calling them while in critical sections of pbm.
04/14/08   cvs     Fix compile warning
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
07/24/06   cvs     Fixes for unequal size multi pb set support
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
08/12/05   AT      Added support for UCS2 encoding.
03/08/05   PA      Support for PBM notifications.
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock.
09/29/04   PA      Initial revision.
===========================================================================*/
#include "comdef.h"
#include "task.h"


/*===========================================================================
FUNCTION PBM_TASK

DESCRIPTION
  The main PBM task.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void pbm_task(dword dummy);

/*===========================================================================
FUNCTION PBM_INIT_BEFORE_TASK_START

DESCRIPTION
  Initialization of PBM task before the task has been started
  critical sections heap and globals for conversion arrays are initialized.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_init_before_task_start( void );

#endif /* PBMTASK_H */
