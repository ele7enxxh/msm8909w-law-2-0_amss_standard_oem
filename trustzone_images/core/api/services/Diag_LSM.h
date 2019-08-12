#ifndef DIAG_LSM_H
#define DIAG_LSM_H
/*===========================================================================

                   Diag Mapping Layer DLL declarations

DESCRIPTION
  Function declarations for Diag Service Mapping Layer
  

Copyright (c) 2007-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/Diag_LSM.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/05/09   mad     Added Event to announce completion of Diag initialization.
                   This can be used by msgoutput driver to switch Debug output
                   to QXDM.
05/10/09   as      Extended the scope of extern to include Diag_LSM_DeInit
01/10/09   as      Fixed extern for __cplusplus 
07/07/09   mad     Included comdef.h
12/02/08   mad     Changed function description for device-driver model diag
02/04/08   mad     Added declarations for Diag_LSM_Init and DeInit functions
                   to enable diag clients to call these directly.
                   Moved IDiagPkt handle declaration to an internal header 
                   file, Diag_LSMi.h
11/29/07   mad     Created File
===========================================================================*/

#include "comdef.h" /* for byte and boolean */
#include <windows.h> /* for TEXT */

#define WINDIAG_TASK_INIT_COMPLETE_EVT_NAME    TEXT("Windiag_Task_Initialized")

/*===========================================================================
FUNCTION   Diag_LSM_Init

DESCRIPTION
  Initializes the Diag Legacy Mapping Layer.

DEPENDENCIES
  Successful initialization requires Diag driver (windiag.dll) to be loaded and available in the system.
RETURN VALUE
  FALSE = failure, else TRUE

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
boolean Diag_LSM_Init (byte* pParam);

/*===========================================================================

FUNCTION    Diag_LSM_DeInit

DESCRIPTION
  De-Initialize the Diag service.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE = failure, else TRUE. 
  Currently all the internal boolean return functions called by
  this function just returns TRUE w/o doing anything.

SIDE EFFECTS
  None

===========================================================================*/
boolean Diag_LSM_DeInit(void);
#ifdef __cplusplus
}
#endif

#endif /* DIAG_LSM_H */
